#include <stdint.h>
#include <stdlib.h>

#include "../multiboot.h"


#define mmap_set(bit)  \
    pmm[bit / 8] |= (1 << (bit % 8));

#define mmap_unset(bit)  \
    pmm[bit / 8] &= ~ (1 << (bit % 8));

extern uint32_t * _kernel_physical_end;
extern uint32_t * _kernel_physical_start;
extern uint32_t * VIRT_BASE;
extern uint32_t * _kernel_start;
extern uint32_t * _kernel_bootstrap_start;
extern uint32_t * _kernel_bootstrap_end;
extern uint32_t bootstrap_heap_start;
extern uint32_t bootstrap_heap_end;

typedef struct my_memory_map {
	uint32_t size;
	uint32_t base_addr_low,base_addr_high;
	uint32_t length_low,length_high;
	uint32_t type;
} mmap_entry_t ;

uint8_t * pmm __attribute__ ((section(".bootstrap.data"))) = 0;
uint32_t block_count __attribute__ ((section(".bootstrap.data"))) = 0;
uint32_t block_size __attribute__ ((section(".bootstrap.data"))) = 4096;

uint32_t __attribute__ ((section(".bootstrap"))) bootstrap_alloc_first () {

	for (uint32_t i=0; i< block_count / 8; i++)
		if (pmm[i] != 0XFF)
			for (uint32_t j=0; j<8; j++) {		

				int bit = 1 << j;
				if (! (pmm[i] & bit) ) {
                    uint32_t frame = i*8+j;
                    mmap_set(frame);
                    return frame * block_size;
                }
			}

	return -1;
}

void __attribute__ ((section(".bootstrap"))) bootstrap_map(uint32_t physical_adress_start,uint32_t virtual_adress_start , uint32_t blocks, uint32_t * pd) {
    uint32_t address_mask = ~0xfff;
    uint32_t page_flags = 0x3;

    uint32_t page_table_index_offset = virtual_adress_start >> 22 & 0x3FF;
    uint32_t pyhsical_offset = 0;
    for(uint32_t page_table_index =  page_table_index_offset ; (page_table_index - page_table_index_offset) <= blocks / 1024; page_table_index++) { 
        uint32_t * pt = (uint32_t *)bootstrap_alloc_first();

        pd[page_table_index] = ((uint32_t)pt & address_mask) | page_flags ;

        uint32_t page_index = 0;
        
        if( page_table_index == page_table_index_offset ) {
            page_index = virtual_adress_start >> 12 & 0x3FF; // 10 bit mask to get the page index 
        }
        for(; page_index < 1024; page_index++) {
            pt[page_index] = (physical_adress_start + pyhsical_offset * 0x1000 & address_mask) | page_flags;
            pyhsical_offset++;
        } 
    }
}

void __attribute__ ((section(".bootstrap"))) init_bit_map(multiboot_info_t* mbt, char * pmm_out, int block_count_out, int block_size_out) {
    heap_t static_heap = { 0 };
    static_heap.is_kernel_only = true;
    static_heap.is_read_only = false;
    static_heap.is_heap_static = true;
    static_heap.max_end_address = &bootstrap_heap_end;
    static_heap.start_address = &bootstrap_heap_start;
    static_heap.end_address = &bootstrap_heap_end;

    // Calls self_map_heap with VIRT_BASE offset
    heap_t * self_mapped_heap = ((heap_t *(*)(heap_t))(self_map_heap - (uint32_t)&VIRT_BASE))(static_heap);

    // TODO: this block_count is supose to be the max memmory adress represented in blocks of 4k but its not the right calc
    block_count = ((1024 * 1024 + (mbt->mem_upper)) * 1024) / block_size;

    // allocate space for pmm with malloc from the real adress of the function before mapping
    multiboot_info_t *new_mbt = ((multiboot_info_t * (*)(size_t, heap_t))(malloc - (uint32_t)&VIRT_BASE)(sizeof(multiboot_info_t), &self_mapped_heap));

    // allocate space for pmm with malloc from the real adress of the function before mapping
    uint8_t * pmm = ((uint8_t *(*)(size_t, heap_t))(malloc - (uint32_t)&VIRT_BASE)(block_count / 8, &self_mapped_heap));

    // memcopy mbt to new mbt
    for (size_t i = 0; i < sizeof(multiboot_info_t); i++) {
        ((char *)new_mbt)[i] = ((char *)mbt)[i];
    }
    

    // memset to pmm to all bytes 1
    for(uint32_t i = 0; i < block_count / 8; i++) {
        pmm[i] = 0xFF;
    }

    mmap_entry_t* entry = mbt->mmap_addr;
	while(entry < mbt->mmap_addr + mbt->mmap_length) {
		if(entry->type == MULTIBOOT_MEMORY_AVAILABLE && entry->base_addr_low != 0) {                
            uint32_t align = entry->base_addr_low / block_size;
            uint32_t blocks = entry->length_low / block_size;
            for (uint32_t i = 0 ; i <= blocks; i++) {
                mmap_unset(align + i);
            }
        }
		entry = (mmap_entry_t*) ((uint32_t) entry + entry->size + sizeof(entry->size));
	}

	uint32_t kernel_address = ((uint32_t) &_kernel_physical_start) / block_size;
	uint32_t kernel_blocks = ((uint32_t)&_kernel_physical_end - (uint32_t) &_kernel_physical_start /* + block_count */ ) / block_size;

    for (uint32_t i = 0 ; i <= kernel_blocks; i++) {
        mmap_set(kernel_address + i);
    }


    mmap_set(0);


    uint32_t address_mask = ~0xfff;
    uint32_t page_flags = 0x3;

    uint32_t * pd = (uint32_t *)bootstrap_alloc_first();

    pd[1023] = ((uint32_t)pd & address_mask) | page_flags ; //self map

    bootstrap_map((uint32_t)&_kernel_physical_start,(uint32_t)&_kernel_start, kernel_blocks, pd); // maps kernel

    uint32_t bootstrap_size = ((uint32_t)&_kernel_bootstrap_end - (uint32_t)&_kernel_bootstrap_start) / block_size;
    bootstrap_map((uint32_t)&_kernel_bootstrap_start,(uint32_t)&_kernel_bootstrap_start, bootstrap_size, pd); // maps bootsrap ljmp code

    bootstrap_map(0xB8000,0xB8000, 1, pd); // maps bootsrap ljmp code

    asm("movl %0, %%eax; movl %%eax, %%cr3;"::"r"(pd));

    mbt = new_mbt;
    pmm_out = pmm;
    block_count_out = block_count;
    block_size_out = block_size;
}