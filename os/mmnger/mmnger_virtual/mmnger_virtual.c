#include <mmnger/mmnger_phys.h>
#include <mmnger/mmnger_virtual.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>


// STACK PAGE POTENTIAL ADDRESS SPACE IS    0XFF7FF000 => 0x‭FF400‬000

// STACK INITIALLY MAPPED ADDRESS SPACE IS 0xFF7FF000 => 0xFF7E6000
#define STACK_PAGES 25 
#define STACK_SIZE STACK_PAGES * 4096 // 100 kb stack
#define STACK_TOP 0xFF7FF000
#define STACK_BOTTOM STACK_TOP - STACK_SIZE

static void flushTLB() {
    asm("movl	%cr3,%eax; movl	%eax,%cr3");
}

// we take adventace of self mapping
page_directory_t current_page_dir = (page_directory_t)0xFFFFF000;
page_table_t stack_page_table = (page_table_t)0xFFFFD000;

static size_t get_page_directory_index (void * virtualaddr) {
    return (uint32_t)virtualaddr >> 22;
}

static size_t get_page_table_index (void * virtualaddr) {
    return (uint32_t)virtualaddr >> 12 & 0X3FF;
}

static uint32_t get_page_address_from_indexes (uint32_t page_directory_index, uint32_t page_table_index) {
    return ((page_directory_index << 22) + (page_table_index << 12));
}

void * get_physaddr(void * virtualaddr)
{

    if (! vmmngr_test_is_page_mapped(virtualaddr)) {
        return (void *)0;
    }

    size_t pd_index = get_page_directory_index(virtualaddr);
    size_t pt_index = get_page_table_index(virtualaddr);
 
 
    page_table_t page_table = (page_table_t) (0xFFC00000 + (pd_index << 12));
 
    return (void *)((page_table[pt_index].pysical_address) + ((size_t)virtualaddr & 0xFFF));
}

bool vmmngr_test_is_page_mapped (void* virtualaddr) {
    
    size_t pd_index = get_page_directory_index(virtualaddr);
    size_t pt_index = get_page_table_index(virtualaddr);

    
    if(! current_page_dir[pd_index].flags & PRESENT_PAGE) {
        return false;
    }

    page_table_t page_table = (page_table_t) (0xFFC00000 + (pd_index << 12));

    if(! (page_table[pt_index].flags & PRESENT_PAGE)) {
        return false;
    }

    return true;
}

void vmmngr_alloc_page (void* virtualaddr, void* physaddr, uint16_t flags) {
    if(vmmngr_test_is_page_mapped(virtualaddr)) {
        return;
    }

    size_t pd_index = get_page_directory_index(virtualaddr);
    size_t pt_index = get_page_table_index(virtualaddr);


    if(! (current_page_dir[pd_index].flags & PRESENT_PAGE)) {
        uint32_t free_block = (uint32_t)pmmngr_alloc_block();
        
        current_page_dir[pd_index].flags = (flags & 0XFFF);
        current_page_dir[pd_index].pysical_address = free_block >> 12;
        flushTLB();
    }

    page_table_t page_table = (page_table_t) (0xFFC00000 + (pd_index << 12));

    page_table[pt_index].pysical_address = (uint32_t)physaddr >> 12;
    page_table[pt_index].flags = flags;

    flushTLB();
}

void vmmngr_alloc_page_and_phys (void* virtualaddr, uint16_t flags) {
    void* free_block = pmmngr_alloc_block();
    vmmngr_alloc_page(virtualaddr, free_block, flags);
}

void vmmngr_free_page (void* virtualaddr) {
    if(! vmmngr_test_is_page_mapped(virtualaddr)) {
        return;
    }

    size_t pd_index = get_page_directory_index(virtualaddr);
    size_t pt_index = get_page_table_index(virtualaddr);

    uint32_t * PT = (uint32_t *) (0xFFC00000 + (pd_index << 12));

    PT[pt_index] = PT[pt_index] & (~ PRESENT_PAGE);
    flushTLB();
}

void vmmngr_free_page_and_phys (void* virtualaddr) {
    pmmngr_free_block(get_physaddr(virtualaddr));
    vmmngr_free_page(virtualaddr);
}

void init_vmmngr() {

    uint32_t kernel_flags = PRESENT_PAGE | READ_WRITE_PAGE ;

    extern uint32_t VIRT_BASE;
    uint32_t kernel_adress_start = &VIRT_BASE;

    size_t max_address_index = (ADDRES_SPACE >> 22);

    // init all page tables entrys
    for (size_t table_index = kernel_adress_start >> 22; table_index <= max_address_index; table_index++)
    {
        if(!(current_page_dir[table_index].flags & PRESENT_PAGE)) {
            uint32_t physical_adder = (uint32_t)pmmngr_alloc_block();

            current_page_dir[table_index].flags = kernel_flags;
            current_page_dir[table_index].pysical_address = physical_adder >> 12;

            memset(get_page_address_from_indexes(LOOP_BACK_TABLE,table_index),0,PAGE_SIZE);
        }
    }

    // create new stack
    
    for (size_t page_index = get_page_table_index(STACK_TOP) - STACK_PAGES; 
        page_index <=  get_page_table_index(STACK_TOP); 
        page_index++)
    {
        uint32_t free_block_adder = (uint32_t)pmmngr_alloc_block();

        stack_page_table[page_index].pysical_address = free_block_adder >> 12;
        stack_page_table[page_index].flags = kernel_flags;
    }

    // copy form old stack

    extern size_t bootstrap_stack_bottom;
    extern size_t bootstrap_stack_top;

    size_t bootstrap_stack_bottom_pointer = (size_t)(&bootstrap_stack_bottom);
    size_t bootstrap_stack_top_pointer = (size_t)(&bootstrap_stack_top);

    size_t bootstrap_stack_size = bootstrap_stack_top_pointer - bootstrap_stack_bottom_pointer;

    memcpy(STACK_TOP - bootstrap_stack_size, bootstrap_stack_bottom_pointer, bootstrap_stack_size);

    size_t esp_val;
    size_t ebp_val;

    asm("movl %%esp, %0":"=r"(esp_val));
    asm("movl %%ebp, %0":"=r"(ebp_val));
    
    size_t new_esp = STACK_TOP - (bootstrap_stack_top_pointer - esp_val);
    size_t new_ebp = STACK_TOP - (bootstrap_stack_top_pointer - ebp_val);

    asm("movl %0, %%esp"::"r"(new_esp));    
    asm("movl %0, %%ebp"::"r"(new_ebp));

    

    flushTLB();
    
}