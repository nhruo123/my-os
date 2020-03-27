#include <mmnger/mmnger_phys.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>


//! size of physical memory
static	uint32_t _mmngr_memory_size = 0;

//! size of physical memory
static	uint32_t _mmngr_block_count = 0;
 
//! memory map bit array. Each bit represents a memory block
static	char * _mmngr_memory_map = 0;

static uint32_t _mmngr_block_size = 0;

static void mmap_set (int bit) {
	_mmngr_memory_map[bit / 8] |= (1 << (bit % 8));
}

static void mmap_unset (int bit) {
	_mmngr_memory_map[bit / 8] &= ~ (1 << (bit % 8));
}

static bool mmap_test (int bit) {
	return _mmngr_memory_map[bit / 8] &  (1 << (bit % 8));
}
static uint32_t pmmngr_get_block_count() {
	return _mmngr_block_count;
} 

static uint32_t mmap_get_first_free_frames(size_t frames_count) {

	for (size_t i = 0; i < pmmngr_get_block_count(); i++) {
		if (! mmap_test(i)) {
			bool is_size_good = true;
			for(size_t j = 0 ; j < frames_count; j++) {
				if (!mmap_test(i + j)) {
					continue;
				} else
				{
					i += j;
					is_size_good = false;
				}
			}

			if(is_size_good) {
				return i;
			}
		}
	}
	return OUT_OF_MEMORY;
}

static uint32_t mmap_get_first_free_frame() {
	mmap_get_first_free_frames(1);
}


void pmmngr_init (size_t block_count, size_t block_size, char * bitmap_adress) {
	_mmngr_memory_size	= block_count * block_size;
	_mmngr_memory_map	= bitmap_adress;
	_mmngr_block_count	= block_count;
	_mmngr_block_size	= block_size;
}


void* pmmngr_alloc_blocks(size_t blocks) {
	int frame = mmap_get_first_free_frames(blocks);
 
	if (frame == 0){
		return OUT_OF_MEMORY;	//out of memory
	}

	for (int i = 0; i < blocks; i++) {
		mmap_set(frame + i);		
	}

 
	return frame * _mmngr_block_size; // adress
}


void* pmmngr_alloc_block() {
	return pmmngr_alloc_blocks(1);
}

void pmmngr_free_blocks (void* addr, size_t blocks) {

	int frame = (uint32_t)addr / _mmngr_block_size;

	for(int i = 0; i < blocks; i ++) {
		mmap_unset (frame + i);
	}
}

void pmmngr_free_block (void* addr) {
	pmmngr_free_blocks(addr, 1);
}

