#include <stdint.h>
#include "../multiboot.h"


#define mmap_set(bit)  \
    pmm[bit / 8] |= (1 << (bit % 8));

#define mmap_unset(bit)  \
    pmm[bit / 8] &= ~ (1 << (bit % 8));

extern uint32_t * _kernel_physical_end;
extern uint32_t * _kernel_physical_start;
extern uint32_t * VIRT_BASE;
extern uint32_t * _kernel_start;
typedef struct my_memory_map {
	uint32_t size;
	uint32_t base_addr_low,base_addr_high;
	uint32_t length_low,length_high;
	uint32_t type;
} my_memory_map_t ;

typedef my_memory_map_t mmap_entry_t;

uint8_t * pmm __attribute__ ((section(".bootstrap.data"))) = 0;
uint32_t block_count __attribute__ ((section(".bootstrap.data"))) = 0;
uint32_t block_size __attribute__ ((section(".bootstrap.data"))) = 4096;

int __attribute__ ((section(".bootstrap"))) bootstrap_alloc_first () {

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

void __attribute__ ((section(".bootstrap"))) init_bit_map(multiboot_info_t* mbt, char * pmm_out, int block_count_out, int block_size_out) {
    block_count = (((mbt->mem_lower) + (mbt->mem_upper)) * 1024) / block_size;
    uint32_t alligned_kernel_physical_end =  &_kernel_physical_end;

    while ((alligned_kernel_physical_end & 0xfff) != 0)
        alligned_kernel_physical_end++;

    pmm = (uint8_t *) alligned_kernel_physical_end;

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
	uint32_t kernel_blocks = (alligned_kernel_physical_end - (uint32_t) &_kernel_physical_start + block_count) / block_size;

    for (uint32_t i = 0 ; i <= kernel_blocks; i++) {
        mmap_set(kernel_address + i);
    }


    mmap_set(0);


    uint32_t flag_mask = ~0xfff;

    uint32_t * pd = (uint32_t *)bootstrap_alloc_first();

    pd[1023] = ((uint32_t)pd & flag_mask) | 0x3 ; //self map

    // TODO print error if kernel size is overflowing
    for(uint32_t page_table_index = 0 ; page_table_index <= kernel_blocks / 1024; page_table_index++) { 
        uint32_t * pt = (uint32_t *)bootstrap_alloc_first();

        uint32_t page_table_entry = ((uint32_t)pt & flag_mask) | 0x3;
        pd[(uint32_t)&_kernel_physical_start / 0x400000 + page_table_index] =  page_table_entry;//identrty map
        pd[(uint32_t)&_kernel_start / 0x400000 + page_table_index] = page_table_entry; // real map

        uint32_t page_index = 0;
        if( page_table_index == 0 ) {
            page_index = (uint32_t)&_kernel_physical_start >> 12 & 0x3FF; // 10 bit mask to get the page index 
        }
        for(; page_index < 1024; page_index++) {
            pt[page_index] = ((page_index * 0x1000 + page_table_index * 0x400000 ) & flag_mask) | 0x3;
        } 
    }

    asm("movl %0, %%eax; movl %%eax, %%cr3;"::"r"(pd));

    pmm_out = pmm;
    block_count_out = block_count;
    block_size_out = block_size;

}
