#ifndef KERNEL_H
#define KERNEL_H 1

void main(multiboot_info_t *mbt, heap_t *bootstrap_heap);

void test_task_switch();
void test_heap();

#endif