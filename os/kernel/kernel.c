#include <stdio.h>
#include <stdlib.h>

#include <mmnger/mmnger_virtual.h>
#include <mmnger/mmnger_phys.h>
#include <mmnger/context_management.h>
#include <interrupts/interrupts.h>
#include <interrupts/isr.h>

#include <multiboot.h>
#include <screen/screen.h>

#include "../keyboard/keyboard.h"

void main(multiboot_info_t *mbt, heap_t *bootstrap_heap, char *pmm, uint32_t block_count, uint32_t block_size)
{
	set_current_heap(bootstrap_heap);
	clear_screen();
	printf("block_count = %d \n", block_count);
	printf("block_size = %d \n", block_size);

	// init base memmory mangemnt
	pmmngr_init(mbt);
	init_vmmngr();
	
	init_context();

	init_screen(mbt);

	clear_screen();

	// init interrupts
	kprint("Starting init idt....\n");

	init_idt();

	register_interrupt_handler(33, &keyboard_call);

	kprint("Done init idt\n");

	// init real heap
	extern uint32_t _kernel_end;
	heap_t kernel_heap_def = {0};
	kernel_heap_def.start_node = NULL;
	kernel_heap_def.start_address = &_kernel_end;
	kernel_heap_def.end_address = kernel_heap_def.start_address;
	kernel_heap_def.max_end_address = kernel_heap_def.end_address + (PAGE_SIZE * 1024) * 10;
	kernel_heap_def.is_read_only = false;
	kernel_heap_def.is_kernel_only = true;
	kernel_heap_def.is_heap_static = false;

	heap_t *kernel_heap = self_map_heap(kernel_heap_def);
	set_current_heap(kernel_heap);

	test_heap();

	uint32_t context_test = 0;

	address_space_t current_address_space = get_current_address_space();

	address_space_t *new_address_space = create_new_address_space();

	printf("context_test physical address = 0x%x , and value is = %d \n", get_physaddr(&context_test), context_test);

	set_current_address_space(*new_address_space);

	context_test = 123;

	printf("context_test physical address = 0x%x , and value is = %d \n", get_physaddr(&context_test), context_test);

	set_current_address_space(current_address_space);

	printf("context_test physical address = 0x%x , and value is = %d \n", get_physaddr(&context_test), context_test);

	kprint("halt...\n");
	for (;;)
	{
		// int x = 1;
		__asm__("hlt");
	}
}

void test_heap()
{
	clear_screen();
	print_heap(get_current_heap());

	void *ptr1 = malloc(1);
	void *ptr2 = malloc(1);
	void *ptr3 = malloc(1);
	void *ptr4 = malloc(1);

	clear_screen();
	print_heap(get_current_heap());

	free(ptr1);
	free(ptr2);
	free(ptr4);
	free(ptr3);

	clear_screen();
	print_heap(get_current_heap());

	void *ptr5 = malloc(4);

	clear_screen();
	print_heap(get_current_heap());

	void *ptr6 = malloc(PAGE_SIZE * 3);

	clear_screen();
	print_heap(get_current_heap());

	void *ptr7 = aligned_malloc(8, 8);

	clear_screen();
	print_heap(get_current_heap());

	void *ptr8 = aligned_malloc(8, 0x1000);

	clear_screen();
	print_heap(get_current_heap());

	free(ptr5);
	free(ptr6);
	free(ptr7);
	free(ptr8);

	clear_screen();
	print_heap(get_current_heap());

	void *ptr9 = malloc(1);

	clear_screen();
	print_heap(get_current_heap());
}