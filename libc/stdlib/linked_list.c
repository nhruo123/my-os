#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "linked_list.h"

static bool is_region_good_for_alloc(list_node_t *node, size_t size, size_t alignment)
{
    uint32_t offset = find_alligned_node_offset(node, alignment);

    // TODO TEST FOR OVERFLOW
    if (offset + size > node->size)
    {
        return false;
    }

    size_t front_leftover = node->size - offset - size;

    if (front_leftover > 0 && front_leftover < (sizeof(list_node_t) + sizeof(node_footer_t)))
    {
        return false;
    }

    size_t back_leftover = offset;

    if (back_leftover > 0 && back_leftover < (sizeof(list_node_t) + sizeof(node_footer_t)))
    {
        return false;
    }

    return true;
}

// TODO change to ofir's formula
// (allign - 1)-(x-1)%allign
uint32_t find_alligned_node_offset(list_node_t *node, size_t alignment) {
    uint32_t usable_mem = ((uint32_t)node + sizeof(list_node_t));

    uint32_t aligned_mem;
    if ((usable_mem % alignment) == 0)
    {
        aligned_mem = usable_mem;
    }
    else
    {
        aligned_mem = usable_mem + (alignment - (usable_mem % alignment));
    }

    uint32_t offset = aligned_mem - usable_mem;

    return offset;
}

list_node_t * find_region(list_node_t ** out_head_pointer, size_t size, size_t alignment) {
    list_node_t ** next_pointer_of_prev = out_head_pointer;
    list_node_t * current = *next_pointer_of_prev;
    list_node_t * next = current->next;

        while(current != NULL) {
        if(is_region_good_for_alloc(current, size , alignment)) {
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
    node_footer_t * new_footer = (void *)address + size + sizeof(list_node_t);

    new_footer->node = new_head;

    new_head->is_free = true;
    new_head->size = size;
    new_head->next = previous_head;

    return new_head;
}

list_node_t * skip_node(list_node_t * head_node, list_node_t * node_to_skip) {
    list_node_t * current = head_node;
    list_node_t * next = current->next;

    if(head_node == node_to_skip) {
        return head_node->next;
    }
    
    while(next != node_to_skip) {
        current = current->next;
        next = current->next;
    }

    current->next = next->next;

    return head_node;
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