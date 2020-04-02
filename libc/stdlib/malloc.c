#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "linked_list.h"


heap_t * self_map_heap(heap_t heap) {
    heap.start_node = add_free_region(NULL,heap.start_address,heap.end_address - heap.start_address -  sizeof(list_node_t));
    void* new_heap_location = malloc(sizeof(heap_t), &heap);

    memcpy(new_heap_location, &heap, sizeof(heap_t));


    return (heap_t *)new_heap_location;
}

void* malloc (size_t size, heap_t * heap) {
    list_node_t * head = heap->start_node;
    list_node_t * free_addr =  find_region(&head, size);

    if(free_addr == NULL) {
        return NULL;
    }

    free_addr->is_free = false;

    void * return_addr = (void *)free_addr + sizeof(list_node_t);
    
    // prefrect fit dosent need a new memory region
    if(size != free_addr->size) {
        free_addr->size = size;
        head = add_free_region(head,return_addr + size, free_addr->size - size - sizeof(list_node_t));
    }

    heap->start_node = head;

    return return_addr;
}

void free (void* ptr, heap_t * heap) {
    list_node_t * head = heap->start_node;

    list_node_t * ptr_node = (list_node_t *)(ptr - sizeof(list_node_t));

    head = add_free_region(head, ptr_node, ptr_node->size);

    heap->start_node = head;
}
