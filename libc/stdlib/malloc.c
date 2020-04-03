#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
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

heap_t *self_map_heap(heap_t heap)
{
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
        return NULL;
    }


    free_node->is_free = false;

    void *return_addr = (void *)free_node + sizeof(list_node_t);

    // if no memmory dose not fit exectly we split it into smaller parts
    if (size != free_node->size)
    {
        free_node->size = size;
        head = add_free_region(head, return_addr + size + sizeof(node_footer_t), free_node->size - size - sizeof(list_node_t) - sizeof(node_footer_t));
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
    list_node_t *next_adjacent_block = (void *)heap->start_node + heap->start_node->size + sizeof(list_node_t) + sizeof(node_footer_t);
    if (next_adjacent_block < heap->end_address && next_adjacent_block->is_free)
    {
        combine_nodes(heap->start_node, next_adjacent_block, heap);
    }

    // try tp combine prev ajacent block
    node_footer_t *prev_ajacent_block_footer = (void *)heap->start_node - sizeof(node_footer_t);
    if (prev_ajacent_block_footer > heap->start_address)
    {
        list_node_t *prev_adjacent_block = prev_ajacent_block_footer->node;
        if (prev_adjacent_block >= heap->start_address && prev_adjacent_block->is_free)
        {
            combine_nodes(prev_adjacent_block, heap->start_node, heap);
        }
    }
}
