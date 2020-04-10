#ifndef CONTEXT_MANGEMENT_H
#define CONTEXT_MANGEMENT_H 1

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

typedef struct page_table_entry_s {
    uint32_t flags : 12;
    uint32_t physical_address : 20;
} page_table_entry_t;

typedef page_table_entry_t * page_table_t;

typedef struct page_dir_entry_s {
    uint32_t flags : 12;
    uint32_t physical_address : 20;
} page_dir_entry_t;

typedef page_dir_entry_t * page_directory_t;

typedef struct address_space_s {
    uint32_t physical_address;
} address_space_t;


#define LOOP_BACK_TABLE     1023  
#define RESERVED_TEMP_TABLE 1022
#define STACK_TABLE         1021
#define SCREEN_TABLE        1020

// STACK PAGE POTENTIAL ADDRESS SPACE IS    0XFF7FF000 => 0x‭FF400‬000

// STACK INITIALLY MAPPED ADDRESS SPACE IS 0xFF7FF000 => 0xFF7E6000
#define STACK_PAGES 25 
#define STACK_SIZE STACK_PAGES * 4096 // 100 kb stack
#define STACK_TOP 0xFF7FF000
#define STACK_BOTTOM STACK_TOP - STACK_SIZE


extern uint32_t kernel_start_address;

extern page_directory_t current_page_dir;
extern page_table_t reserved_temp_table;
extern page_table_t stack_page_table;


// utils functions
address_space_t get_current_address_space();
void set_current_address_space(address_space_t new_address_space);


void init_context();

page_dir_entry_t clone_page_table(size_t page_table_index);
address_space_t *create_new_address_space();



#endif
