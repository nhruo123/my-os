#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

struct ListNode {
    size_t size;
    bool is_free; // TODO ADD A OPTION TO CHACK ADJACENT NODES FOR MEMO FRAGMENTATION
    struct ListNode* next;
};


struct ListNode * get_head_to_space();
void set_head_to_space(struct ListNode * head);
struct ListNode * add_free_region(struct ListNode * previous_head, uint32_t adress, size_t size);
struct ListNode * find_region(struct ListNode ** out_head_pointer, size_t size);

