#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "linked_list.h"


void* malloc (size_t size) {
    struct ListNode * head = get_head_to_space();
    struct ListNode * free_addr =  find_region(&head, size);

    if(free_addr == NULL) {
        return NULL;
    }

    free_addr->is_free = false;

    void * return_addr = (void *)free_addr + sizeof(struct ListNode);
    
    // prefrect fit dosent need a new memory region
    if(size != free_addr->size) {
        free_addr->size = size;
        head = add_free_region(head,return_addr + size, free_addr->size - size - sizeof(struct ListNode));
    }

    set_head_to_space(head);

    return return_addr;
}

void free (void* ptr) {
    struct ListNode * head = get_head_to_space();

    struct ListNode * ptr_node = (struct ListNode *)(ptr - sizeof(struct ListNode));

    head = add_free_region(head, ptr_node, ptr_node->size);

    set_head_to_space(head);
}
