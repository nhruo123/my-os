#ifndef MMNAGER_VIRTUAL_H
#define MMNAGER_VIRTUAL_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>


#define LOOP_BACK_TABLE     1023  
#define ADDRES_SPACE        0xFFFFFFFF
#define PAGE_SIZE           4096 // 4 KB

#define PRESENT_PAGE        0b000000001
#define READ_WRITE_PAGE     0b000000010
#define USER_PAGE           0b000000100
#define WRITE_THROUGH_PAGE  0b000001000
#define CACHE_PAGE          0b000010000
#define ACCESSED_BIT        0b000100000
#define DIRTY_BIT           0b001000000
#define RESERVED_BIT        0b010000000
#define GLOBAL_PAGE         0b100000000

typedef struct page_entry_s {
    uint32_t flags : 12;
    uint32_t pysical_address : 20;
} page_entry_s;

typedef page_entry_s * page_table_t;

typedef struct table_page_entry_s {
    uint32_t flags : 12;
    uint32_t pysical_address : 20;
} table_page_entry_s;

typedef table_page_entry_s * page_directory_t;

typedef struct table_dir_entry_s {
    uint32_t flags : 12;
    uint32_t pysical_address : 20;
} table_dir_entry_s;


static size_t get_page_directory_index (void * virtualaddr);

static size_t get_page_table_index (void * virtualaddr);
static uint32_t get_page_address_from_indexes (uint32_t page_directory_index, uint32_t page_table_index);

void * get_physaddr(void * virtualaddr);

bool vmmngr_test_is_page_mapped (void* virtualaddr);

void vmmngr_alloc_page (void* virtualaddr, void* physaddr, uint16_t flags);
void vmmngr_alloc_page_and_phys (void* virtualaddr, uint16_t flags);


void vmmngr_free_page (void* virtualaddr);
void vmmngr_free_page_and_phys (void* virtualaddr);

void init_vmmngr();

#endif