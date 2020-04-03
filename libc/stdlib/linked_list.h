#ifndef _LINKED_LIST_H
#define _LINKED_LIST_H 1

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>



list_node_t * get_head_to_space();
void set_head_to_space(list_node_t * head);
list_node_t * add_free_region(list_node_t * previous_head, uint32_t adress, size_t size);
list_node_t * find_region(list_node_t ** out_head_pointer, size_t size);
list_node_t * skip_node(list_node_t * head_node, list_node_t * node_to_skip);

#endif