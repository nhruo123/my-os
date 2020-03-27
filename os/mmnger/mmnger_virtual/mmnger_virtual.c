#include <mmnger/mmnger_phys.h>
#include <mmnger/mmnger_virtual.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>


static void flushTLB() {
    asm("movl	%cr3,%eax; movl	%eax,%cr3");
}

// we take adventace of self mapping
uint32_t * PD = (uint32_t *)0xFFFFF000; 

void * get_physaddr(void * virtualaddr)
{

    if (! vmmngr_test_is_page_mapped(virtualaddr)) {
        return (void *)0;
    }

    uint32_t pd_index = (uint32_t)virtualaddr >> 22;
    uint32_t pt_index = (uint32_t)virtualaddr >> 12 & 0X3FF;
 
 
    uint32_t * PT = (uint32_t *) (0xFFC00000 + (pd_index << 12));
 
    return (void *)((PT[pt_index] & ~0xFFF) + ((uint32_t)virtualaddr & 0xFFF));
}

bool vmmngr_test_is_page_mapped (void* virtualaddr) {
    
    uint32_t pd_index = (uint32_t)virtualaddr >> 22;
    uint32_t pt_index = (uint32_t)virtualaddr >> 12 & 0X3FF;

    if(! PD[pd_index] & PRESENT_PAGE) {
        return false;
    }

    uint32_t * PT = (uint32_t *) (0xFFC00000 + (pd_index << 12));

    if(! PT[pt_index] & PRESENT_PAGE) {
        return false;
    }

    return true;
}

void vmmngr_alloc_page (void* virtualaddr, void* physaddr, uint16_t flags) {
    if(vmmngr_test_is_page_mapped(virtualaddr)) {
        return;
    }

    uint32_t pd_index = (uint32_t)virtualaddr >> 22;
    uint32_t pt_index = (uint32_t)virtualaddr >> 12 & 0X3FF;


    if(! PD[pd_index] & PRESENT_PAGE) {
        uint32_t pmem = (uint32_t)pmmngr_alloc_block();
        PD[pd_index] = (pmem & ~0xFFF) | (flags & 0XFFF);
        flushTLB();
    }

    uint32_t * PT = (uint32_t *) (0xFFC00000 + (pd_index << 12));

    PT[pt_index] = ((uint32_t)physaddr & ~0xFFF) | (flags & 0XFFF);
    flushTLB();
}

void vmmngr_free_page (void* virtualaddr) {
    if(! vmmngr_test_is_page_mapped(virtualaddr)) {
        return;
    }

    uint32_t pd_index = (uint32_t)virtualaddr >> 22;
    uint32_t pt_index = (uint32_t)virtualaddr >> 12 & 0X3FF;

    uint32_t * PT = (uint32_t *) (0xFFC00000 + (pd_index << 12));

    PT[pt_index] = PT[pt_index] & (~ PRESENT_PAGE);
    flushTLB();
}