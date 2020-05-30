#include <mmnger/mmnger_phys.h>
#include <mmnger/context_management.h>
#include <mmnger/mmnger_virtual.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <multitasking/task.h>


void flushTLB()
{
    asm volatile("movl	%cr3,%eax; movl	%eax,%cr3");
}

size_t get_page_directory_index(void *virtualaddr)
{
    return (uint32_t)virtualaddr >> 22;
}

size_t get_page_table_index(void *virtualaddr)
{
    return (uint32_t)virtualaddr >> 12 & 0X3FF;
}

uint32_t get_page_address_from_indexes(uint32_t page_directory_index, uint32_t page_table_index)
{
    return ((page_directory_index << 22) + (page_table_index << 12));
}

void init_vmmngr()
{
    extern uint32_t VIRT_BASE;

    current_page_dir = (page_directory_t)get_page_address_from_indexes(LOOP_BACK_TABLE, LOOP_BACK_TABLE);
    reserved_temp_table = (page_directory_t)get_page_address_from_indexes(LOOP_BACK_TABLE, RESERVED_TEMP_TABLE);
    stack_page_table = (page_directory_t)get_page_address_from_indexes(LOOP_BACK_TABLE, STACK_TABLE);

    kernel_start_address = (uint32_t)&VIRT_BASE;
}

void *get_physaddr(void *virtualaddr)
{

    if (!vmmngr_test_is_page_mapped(virtualaddr))
    {
        return (void *)0;
    }

    size_t pd_index = get_page_directory_index(virtualaddr);
    size_t pt_index = get_page_table_index(virtualaddr);

    page_table_t page_table = (page_table_t)get_page_address_from_indexes(LOOP_BACK_TABLE,pd_index);

    return (void *)((page_table[pt_index].physical_address << 12) + ((size_t)virtualaddr & 0xFFF));
}

bool vmmngr_test_is_page_mapped(void *virtualaddr)
{

    size_t pd_index = get_page_directory_index(virtualaddr);
    size_t pt_index = get_page_table_index(virtualaddr);

    if (!current_page_dir[pd_index].flags & PRESENT_PAGE)
    {
        return false;
    }

    page_table_t page_table = (page_table_t)(0xFFC00000 + (pd_index << 12));

    if (!(page_table[pt_index].flags & PRESENT_PAGE))
    {
        return false;
    }

    return true;
}

void vmmngr_alloc_page(void *virtualaddr, void *physaddr, uint16_t flags)
{

    lock_kernel_stuff();

    if (vmmngr_test_is_page_mapped(virtualaddr))
    {
        pmmngr_free_page(physaddr);
        unlock_kernel_stuff();
        return;
    }

    size_t pd_index = get_page_directory_index(virtualaddr);
    size_t pt_index = get_page_table_index(virtualaddr);

    if (!(current_page_dir[pd_index].flags & PRESENT_PAGE))
    {
        vmmngr_alloc_page_table(pd_index, flags);
    }

    page_table_t page_table = (page_table_t)(0xFFC00000 + (pd_index << 12));

    page_table[pt_index].physical_address = (uint32_t)physaddr >> 12;
    page_table[pt_index].flags = flags;

    flushTLB();
    unlock_kernel_stuff();
}

bool vmmngr_alloc_page_table(uint32_t page_table_index, uint16_t flags)
{   
    lock_kernel_stuff();
    uint32_t free_address = (uint32_t)pmmngr_alloc_page();
    if(free_address == NULL) {
        unlock_kernel_stuff();
        return false;
    }
    current_page_dir[page_table_index].flags = flags;
    current_page_dir[page_table_index].physical_address =  free_address >> 12;

    flushTLB();

    unlock_kernel_stuff();

    return true;
}

void vmmngr_alloc_page_and_phys(void *virtualaddr, uint16_t flags)
{
    void *free_block = pmmngr_alloc_page();
    if(free_block == OUT_OF_MEMORY) {
        // FKING PANIC
        abort();
    }
    vmmngr_alloc_page(virtualaddr, free_block, flags);
}

void vmmngr_free_page(void *virtualaddr)
{
    lock_kernel_stuff();
    if (!vmmngr_test_is_page_mapped(virtualaddr))
    {
        unlock_kernel_stuff();
        return;
    }

    size_t pd_index = get_page_directory_index(virtualaddr);
    size_t pt_index = get_page_table_index(virtualaddr);

    page_table_t page_table_to_free = (page_table_t)(0xFFC00000 + (pd_index << 12));

    page_table_to_free[pt_index].flags &= (~PRESENT_PAGE);
    flushTLB();
    unlock_kernel_stuff();
}

void vmmngr_free_page_and_phys(void *virtualaddr)
{
    pmmngr_free_page(get_physaddr(virtualaddr));
    vmmngr_free_page(virtualaddr);
}
