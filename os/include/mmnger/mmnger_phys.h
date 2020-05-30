#ifndef MMNAGER_PHYS_H
#define MMNAGER_PHYS_H

#include <stdint.h>
#include <stddef.h>
#include <multiboot.h>

#define OUT_OF_MEMORY NULL


void pmmngr_init(multiboot_info_t *mbt);

void pmmngr_change_heap();

void* pmmngr_alloc_pages(size_t blocks);

void* pmmngr_alloc_pages();

void pmmngr_free_pages (void* addr, size_t blocks);

void pmmngr_free_page (void* addr);


#endif