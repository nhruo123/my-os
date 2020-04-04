#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <mmnger/mmnger_virtual.h>
#include "linked_list.h"

static void combine_nodes(list_node_t *first, list_node_t *second, heap_t *heap)
{
    if (first > second)
    {
        combine_nodes(second, first, heap);
        return;
    }
    if (first == second)
    {
        abort();
    }

    heap->start_node = skip_node(heap->start_node, first);
    heap->start_node = skip_node(heap->start_node, second);

    first->size += second->size + sizeof(node_footer_t) + sizeof(list_node_t);
    heap->start_node = add_free_region(heap->start_node, first, first->size);
}

static try_combine_adjacent_nodes(heap_t * heap, list_node_t * node_to_combine) {
    list_node_t *next_adjacent_block = (void *)node_to_combine + node_to_combine->size + sizeof(list_node_t) + sizeof(node_footer_t);
    if (next_adjacent_block < heap->end_address && next_adjacent_block->is_free)
    {
        combine_nodes(node_to_combine, next_adjacent_block, heap);
    }

    // try tp combine prev ajacent block
    node_footer_t *prev_ajacent_block_footer = (void *)node_to_combine - sizeof(node_footer_t);
    if (prev_ajacent_block_footer > heap->start_address)
    {
        list_node_t *prev_adjacent_block = prev_ajacent_block_footer->node;
        if (prev_adjacent_block >= heap->start_address && prev_adjacent_block->is_free)
        {
            combine_nodes(prev_adjacent_block, node_to_combine, heap);
        }
    }
}

static void add_page_to_heap(heap_t *heap) {
    uint16_t flags = PRESENT_PAGE;

    if(!heap->is_kernel_only) {
        flags = flags | USER_PAGE;
    }
    if(!heap->is_read_only) {
        flags = flags | READ_WRITE_PAGE;
    }

    vmmngr_alloc_page_and_phys(heap->end_address, flags);
    heap->start_node = add_free_region(heap->start_node, heap->end_address, PAGE_SIZE - sizeof(list_node_t) - sizeof(node_footer_t));
    heap->end_address += PAGE_SIZE;

    try_combine_adjacent_nodes(heap, heap->start_node);
}

heap_t *self_map_heap(heap_t heap)
{
    if(heap.end_address - heap.start_address == 0) {
        add_page_to_heap(&heap);
    }
    heap.start_node = add_free_region(NULL, heap.start_address, heap.end_address - heap.start_address - sizeof(list_node_t) - sizeof(node_footer_t));
    void *new_heap_location = malloc(sizeof(heap_t), &heap);

    memcpy(new_heap_location, &heap, sizeof(heap_t));

    return (heap_t *)new_heap_location;
}

void *malloc(size_t size, heap_t *heap)
{
    list_node_t *head = heap->start_node;
    list_node_t *free_node = find_region(&head, size);
    

    if (free_node == NULL)
    {
        if(!heap->is_heap_static && heap->max_end_address > heap->end_address) {
            add_page_to_heap(heap);
            return malloc(size, heap);
        }
        return NULL;
    }


    free_node->is_free = false;

    void *return_addr = (void *)free_node + sizeof(list_node_t);

    // if no memmory dose not fit exectly we split it into smaller parts
    if (size != free_node->size)
    {
        head = add_free_region(head, return_addr + size + sizeof(node_footer_t), free_node->size - size - sizeof(list_node_t) - sizeof(node_footer_t));
        free_node->size = size;
    }

    // modifiy footer of the malloced block
    node_footer_t *free_node_footer = (void *)free_node + free_node->size + sizeof(list_node_t);
    free_node_footer->node = free_node;

    heap->start_node = head;

    return return_addr;
}

void free(void *ptr, heap_t *heap)
{

    list_node_t *free_node = (list_node_t *)(ptr - sizeof(list_node_t));

    heap->start_node = add_free_region(heap->start_node, free_node, free_node->size);

    // try to combine next adjacent block
    try_combine_adjacent_nodes(heap, heap->start_node);
    
}
