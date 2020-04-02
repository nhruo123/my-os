#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "linked_list.h"

static bool is_region_good_for_alloc(list_node_t * node, size_t size) {
    size_t alloc_size = size + sizeof(list_node_t);
    size_t leftover = node->size - size;

    if(node->size < size) {
        return false;
    }

    if(leftover > 0 && leftover <= sizeof(list_node_t)) {
        return false;
    }

    return true;
}

list_node_t * find_region(list_node_t ** out_head_pointer, size_t size) {
    list_node_t ** next_pointer_of_prev = out_head_pointer;
    list_node_t * current = *next_pointer_of_prev;
    list_node_t * next = current->next;
    
    while(current != NULL) {
        if(is_region_good_for_alloc(current, size)) {
            *next_pointer_of_prev = next; // skip current
            current->next = NULL;
            return current;
        }
        
        next_pointer_of_prev = &(current->next);
        current = current->next;
        if( next != NULL) {
            next = next->next;
        }
        
    }

    return NULL;
}

list_node_t * add_free_region(list_node_t * previous_head, uint32_t address, size_t size) {
    list_node_t * new_head = (void *)address;
    new_head->is_free = true;
    new_head->size = size;
    new_head->next = previous_head;

    return new_head;
}





// list_node_t * get_head_to_space() {
//     extern uint32_t heap_start;
//     extern uint32_t heap_end;
    
//     if(! is_khead_init) {
//         khead = NULL;
//         khead = add_free_region(khead,(uint32_t)&heap_start, (uint32_t)&heap_end - (uint32_t)&heap_start);
//         is_khead_init = true;
//     }

//     return khead;
// }

// void set_head_to_space(list_node_t * head) {
//     #if defined(__is_libk)
//     khead = head;
//     #else
//     uhead = head;
//     #endif
// }