#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
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

static try_combine_adjacent_nodes(heap_t *heap, list_node_t *node_to_combine)
{
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

static void add_page_to_heap(heap_t *heap)
{
    uint16_t flags = PRESENT_PAGE;

    if (!heap->is_kernel_only)
    {
        flags = flags | USER_PAGE;
    }
    if (!heap->is_read_only)
    {
        flags = flags | READ_WRITE_PAGE;
    }

    vmmngr_alloc_page_and_phys(heap->end_address, flags);
    heap->start_node = add_free_region(heap->start_node, heap->end_address, PAGE_SIZE - sizeof(list_node_t) - sizeof(node_footer_t));
    heap->end_address += PAGE_SIZE;

    try_combine_adjacent_nodes(heap, heap->start_node);
}

heap_t *self_map_heap(heap_t heap)
{
    if (heap.end_address - heap.start_address == 0)
    {
        add_page_to_heap(&heap);
    }
    heap.start_node = add_free_region(NULL, heap.start_address, heap.end_address - heap.start_address - sizeof(list_node_t) - sizeof(node_footer_t));
    void *new_heap_location = malloc(sizeof(heap_t), &heap);

    memcpy(new_heap_location, &heap, sizeof(heap_t));

    return (heap_t *)new_heap_location;
}

void *aligned_malloc(size_t size, size_t alignment, heap_t *heap)
{
    if (alignment % 4 != 0)
    {
        abort();
    }

    if (size % 4 != 0)
    {
        size += 4 - (size % 4);
    }

    list_node_t *free_node = find_region(&heap->start_node, size, alignment);

    if (free_node == NULL)
    {
        if (!heap->is_heap_static && heap->max_end_address > heap->end_address)
        {
            add_page_to_heap(heap);
            return aligned_malloc(size, alignment, heap);
        }
        return NULL;
    }

    uint32_t offset = find_alligned_node_offset(free_node, alignment);

    
    list_node_t free_node_copy = {0};
    memcpy(&free_node_copy, free_node, sizeof(list_node_t));

    // If there is offset we need to create a back block
    if (offset > 0)
    {
        heap->start_node = add_free_region(
            heap->start_node,
            (void *)free_node,
            offset - sizeof(list_node_t) - sizeof(node_footer_t));
    }

    // if there is left over memory we need to create a front block
    if ((free_node_copy.size - offset - size) != 0)
    {
        heap->start_node = add_free_region(
            heap->start_node,
            (void *)free_node + offset + size + sizeof(node_footer_t) + sizeof(list_node_t),
            free_node_copy.size - size - offset - sizeof(list_node_t) - sizeof(node_footer_t));
    }

    // modifiy the block itself
    free_node = ((void *)free_node) + offset;
    free_node->is_free = false;
    free_node->size = size;

    // modifiy footer of the malloced block
    // Offset is already added by free_node init
    node_footer_t *free_node_footer = (void *)free_node + size + sizeof(list_node_t);
    free_node_footer->node = free_node;

    return (void *)free_node + sizeof(list_node_t);
}

void *malloc(size_t size, heap_t *heap)
{
    aligned_malloc(size, 4, heap);
}

void free(void *ptr, heap_t *heap)
{

    list_node_t *free_node = (list_node_t *)(ptr - sizeof(list_node_t));

    heap->start_node = add_free_region(heap->start_node, free_node, free_node->size);

    // try to combine next adjacent block
    try_combine_adjacent_nodes(heap, heap->start_node);
}

void print_heap(heap_t *heap)
{
    
    printf("heap = 0x%x = {start_address: 0x%x , end_address: 0x%x , max_end_address: 0x%x , is_kernel_only: %d , is_read_only: %d , is_heap_static: %d } \n",
     (uint32_t)heap, (uint32_t)heap->start_address, (uint32_t)heap->end_address,
     (uint32_t)heap->max_end_address, (uint32_t)heap->is_kernel_only ,
     (uint32_t)heap->is_read_only , (uint32_t)heap->is_heap_static);

    printf("\n");
    size_t index = 0;
    list_node_t * node = heap->start_node;
    

    while (node != NULL)
    {
        node_footer_t * footer = (void *)node + node->size + sizeof(list_node_t);
        if (footer->node != node)
        {
            printf("ERROR FOOTER ISNT POINTING TO NODE \n");
        }

        printf("node(%d) = 0x%x{ size: %d , is_free: %d , next: 0x%x  } \n", 
        index, (uint32_t)node, node->size, (uint32_t)node->is_free, (uint32_t)node->next);
        printf("\n");

        printf("footer(%d) = 0x%x{ node: 0x%x  } \n", index, (uint32_t)footer, footer->node);
        printf("\n");

        node = node->next;
        index++;
    }
}