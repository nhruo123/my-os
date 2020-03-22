#ifndef MMNAGER_PHYS_H
#define MMNAGER_PHYS_H

#include <stdint.h>
#include <stddef.h>

#define OUT_OF_MEMORY 0;

void pmmngr_init (size_t block_count, size_t block_size, uint32_t bitmap_adress);

void* pmmngr_alloc_blocks(size_t blocks);

void* pmmngr_alloc_block();

void pmmngr_free_blocks (void* addr, size_t blocks);

void pmmngr_free_block (void* addr);


#endif