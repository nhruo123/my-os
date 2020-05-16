#ifndef KERNEL_H
#define KERNEL_H 1

void main(multiboot_info_t *mbt, heap_t *bootstrap_heap);

void start_shell();
void test_heap();

#endif