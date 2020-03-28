#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "linked_list.h"

#if defined(__is_libk)
    static struct ListNode * khead;
    bool is_khead_init = false;
#else
	static struct ListNode * uhead;
    bool is_uhead_init = true;
#endif

static bool is_region_good_for_alloc(struct ListNode * node, size_t size) {
    size_t alloc_size = size + sizeof(struct ListNode);
    size_t leftover = node->size - size;

    if(node->size < size) {
        return false;
    }

    if(leftover > 0 && leftover <= sizeof(struct ListNode)) {
        return false;
    }

    return true;
}

struct ListNode * find_region(struct ListNode ** out_head_pointer, size_t size) {
    struct ListNode ** next_pointer_of_prev = out_head_pointer;
    struct ListNode * current = *next_pointer_of_prev;
    struct ListNode * next = current->next;
    
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

struct ListNode * add_free_region(struct ListNode * previous_head, uint32_t address, size_t size) {
    struct ListNode * new_head = (void *)address;
    new_head->is_free = true;
    new_head->size = size;
    new_head->next = previous_head;

    return new_head;
}





struct ListNode * get_head_to_space() {
    #if defined(__is_libk)
    extern uint32_t heap_start;
    extern uint32_t heap_end;
    
    if(! is_khead_init) {
        khead = NULL;
        khead = add_free_region(khead,(uint32_t)&heap_start, (uint32_t)&heap_end - (uint32_t)&heap_start);
        is_khead_init = true;
    }

    return khead;
        
    #else
        // TODO ADD A WAY TO FIND HEAP IN PROCESS
    #endif
}

void set_head_to_space(struct ListNode * head) {
    #if defined(__is_libk)
    khead = head;
    #else
    uhead = head;
    #endif
}