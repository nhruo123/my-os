#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <sys/cdefs.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct list_node_s {
    size_t size;
    bool is_free; // TODO ADD A OPTION TO CHACK ADJACENT NODES FOR MEMO FRAGMENTATION
    struct ListNode* next;
} list_node_t;

typedef struct node_footer_s {
    list_node_t * node;
} node_footer_t;

typedef struct heap_s {
    list_node_t * start_node;
    void* start_address;
    void* end_address;
    void* max_end_address;
    bool is_kernel_only;
    bool is_read_only;
    bool is_heap_static;
} heap_t;

__attribute__((__noreturn__))
void abort(void);


heap_t * self_map_heap(heap_t heap);
void* malloc_h (size_t size, heap_t * heap);
void *aligned_malloc_h(size_t size, size_t alignment, heap_t *heap);
void free_h (void* ptr, heap_t * heap);


void set_current_heap(heap_t * heap);
heap_t * get_current_heap();
void* malloc (size_t size);
void *aligned_malloc(size_t size, size_t alignment);
void free (void* ptr);

char* itoa ( int value, char * str, int base );
char* utoa ( unsigned int value, char * str, int base );

#endif