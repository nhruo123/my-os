#include <mmnger/mmnger_phys.h>
#include <mmnger/context_management.h>
#include <mmnger/mmnger_virtual.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <multitasking/task.h>


uint32_t kernel_start_address = NULL;

page_directory_t current_page_dir = NULL;
page_table_t reserved_temp_table = NULL;
page_table_t stack_page_table = NULL;

address_space_t get_current_address_space()
{
    address_space_t current_address_sapce;
    asm("movl %%cr3, %0;"
        : "=r"(current_address_sapce));
    return current_address_sapce;
}

void set_current_address_space(address_space_t new_address_space)
{
    // ofir's shit or somthing
    // asm("popl %%ecx; popl %%edx; movl %0, %%eax; movl %%eax, %%cr3;pushl %%edx;pushl %%ecx;"
    // :
    // :"r"(new_address_space.physical_address));
    asm("movl (%%ebp), %%ecx; movl 4(%%ebp), %%edx; movl %0, %%eax; movl %%eax, %%cr3;movl %%ecx, (%%ebp); movl %%edx, 4(%%ebp);"
        :
        : "r"(new_address_space.physical_address));
}

// !!!!!THIS MUST BE CALLED FORM THE FIRST FUNCTION AFTER BOOTSTRAP STAGE!!!!
void init_context()
{
    // Create Page tables over all kernel space
    for (size_t page_table_index = get_page_directory_index(kernel_start_address);
         page_table_index < get_page_directory_index(MAX_ADDRES);
         page_table_index++)
    {
        if (!(current_page_dir[page_table_index].flags & PRESENT_PAGE))
        {
            bool is_alloc_good = vmmngr_alloc_page_table(page_table_index, KERNEL_FLAGS);
            

            memset(get_page_address_from_indexes(LOOP_BACK_TABLE, page_table_index), 0, PAGE_SIZE);

        }
    }

    // create new stack

    for (size_t page_index = get_page_table_index(STACK_TOP) - STACK_PAGES;
         page_index <= get_page_table_index(STACK_TOP);
         page_index++)
    {
        stack_page_table[page_index].physical_address = (uint32_t)pmmngr_alloc_page() >> 12;
        stack_page_table[page_index].flags = KERNEL_FLAGS;
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

    asm("movl %%esp, %0"
        : "=r"(esp_val));
    asm("movl %%ebp, %0"
        : "=r"(ebp_val));

    size_t new_esp = STACK_TOP - (bootstrap_stack_top_pointer - esp_val);
    size_t new_ebp = STACK_TOP - (bootstrap_stack_top_pointer - ebp_val);

    asm("movl %0, %%esp" ::"r"(new_esp));
    asm("movl %0, %%ebp" ::"r"(new_ebp));

    size_t old_ebp;

    asm("movl (%%ebp), %0"
        : "=r"(old_ebp));

    size_t fixed_old_ebp = STACK_TOP - (bootstrap_stack_top_pointer - old_ebp);
    asm("movl %0, (%%ebp)" ::"r"(fixed_old_ebp));

    flushTLB();
}

page_dir_entry_t clone_page_table(size_t page_table_index)
{
    lock_kernel_stuff();
    page_dir_entry_t old_reserved_temp_table = current_page_dir[RESERVED_TEMP_TABLE];

    page_dir_entry_t dir_entry_to_copy = current_page_dir[page_table_index];
    page_dir_entry_t new_page_dir_entry = {0};
    new_page_dir_entry.physical_address = (uint32_t)pmmngr_alloc_page() >> 12;
    new_page_dir_entry.flags = dir_entry_to_copy.flags;

    current_page_dir[RESERVED_TEMP_TABLE] = new_page_dir_entry;
    flushTLB();

    page_table_t page_table_to_copy = (page_table_t)get_page_address_from_indexes(LOOP_BACK_TABLE, page_table_index);
    page_table_t new_page_table = (page_table_t)get_page_address_from_indexes(LOOP_BACK_TABLE, RESERVED_TEMP_TABLE);

    for (size_t page_index = 0; page_index < PAGES_IN_TABLE; page_index++)
    {

        if (page_table_to_copy[page_index].flags & PRESENT_PAGE)
        {
            new_page_table[page_index].flags = page_table_to_copy[page_index].flags;
            new_page_table[page_index].physical_address = (uint32_t)pmmngr_alloc_page() >> 12;
            flushTLB();

            void *current_page_address = get_page_address_from_indexes(page_table_index, page_index);
            void *new_page_address = get_page_address_from_indexes(RESERVED_TEMP_TABLE, page_index);

            memcpy(new_page_address, current_page_address, PAGE_SIZE);
        }
    }

    current_page_dir[RESERVED_TEMP_TABLE] = old_reserved_temp_table;
    flushTLB();

    unlock_kernel_stuff();
    return new_page_dir_entry;
}

address_space_t create_new_address_space()
{
    page_dir_entry_t old_reserved_temp_table = current_page_dir[RESERVED_TEMP_TABLE];

    address_space_t new_page_dir_entry;

    new_page_dir_entry.physical_address = (uint32_t)pmmngr_alloc_page();

    current_page_dir[RESERVED_TEMP_TABLE].flags = KERNEL_FLAGS;
    current_page_dir[RESERVED_TEMP_TABLE].physical_address = new_page_dir_entry.physical_address >> 12;
    flushTLB();


    page_directory_t new_page_dir = (page_directory_t)get_page_address_from_indexes(LOOP_BACK_TABLE, RESERVED_TEMP_TABLE);

    // set new page dir with 0 cuz real memroy is random 
    memset(new_page_dir, 0x0, PAGE_SIZE);

    // copy all page tables beside the loopback
    for (size_t table_index = kernel_start_address >> 22; table_index < (MAX_ADDRES >> 22); table_index++)
    {
        if (table_index == STACK_TABLE)
        {
            continue;
        }
        new_page_dir[table_index] = current_page_dir[table_index];
    }

    // change loop back map for new page dir
    new_page_dir[LOOP_BACK_TABLE].flags = KERNEL_FLAGS;
    new_page_dir[LOOP_BACK_TABLE].physical_address = new_page_dir_entry.physical_address >> 12;
    

    // // TODO REMOVE THIS AND MANGAGE VGA
    // size_t vga_index = get_page_directory_index(0xB8000);
    // new_page_dir[vga_index] = current_page_dir[vga_index];

    // clone stack page table
    new_page_dir[STACK_TABLE] = clone_page_table(STACK_TABLE);

    current_page_dir[RESERVED_TEMP_TABLE] = old_reserved_temp_table;
    flushTLB();

    return new_page_dir_entry;
}

static page_dir_entry_t none_locking_mount_page_dir_on_temp_dir(page_dir_entry_t dir_to_mount) {
    page_dir_entry_t old_dir = current_page_dir[RESERVED_TEMP_TABLE];
    current_page_dir[RESERVED_TEMP_TABLE] = dir_to_mount;
    
    flushTLB();
    return old_dir;
}

page_dir_entry_t mount_page_dir_on_temp_dir(page_dir_entry_t dir_to_mount) {
    lock_kernel_stuff();

    page_dir_entry_t to_return = none_locking_mount_page_dir_on_temp_dir(dir_to_mount);

    unlock_kernel_stuff();

    return to_return;
}



page_dir_entry_t mount_address_space_on_temp_dir(address_space_t adress_space_to_mount) {
    lock_kernel_stuff();

    page_dir_entry_t dummy_dir_entry;
    dummy_dir_entry.flags = KERNEL_FLAGS;
    dummy_dir_entry.physical_address = adress_space_to_mount.physical_address >> 12;

    unlock_kernel_stuff();
    
    return none_locking_mount_page_dir_on_temp_dir(dummy_dir_entry);
}