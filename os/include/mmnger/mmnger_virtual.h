#ifndef MMNAGER_VIRTUAL_H
#define MMNAGER_VIRTUAL_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#define PAGE_SIZE 4096

#define PRESENT_PAGE 0b1

#define READ_WRITE_PAGE 0b10

#define USER_PAGE 0b100



void * get_physaddr(void * virtualaddr);

bool vmmngr_test_is_page_mapped (void* virtualaddr);

void vmmngr_alloc_page (void* virtualaddr, void* physaddr, uint16_t flags);
void vmmngr_alloc_page_and_phys (void* virtualaddr, uint16_t flags);


void vmmngr_free_page (void* virtualaddr);
void vvmmngr_free_page_and_phys (void* virtualaddr);

#endif