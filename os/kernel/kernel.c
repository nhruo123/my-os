#include <stdio.h>
#include <stdlib.h>

#include <mmnger/mmnger_virtual.h>
#include <mmnger/mmnger_phys.h>
#include <interrupts/interrupts.h>
#include <interrupts/isr.h>

#include "../keyboard/keyboard.h"
#include "../multiboot.h"

void main(multiboot_info_t *mbt, heap_t * bootstrap_heap, char *pmm, int block_count, int block_size) {
	terminalInit();
	pmmngr_init(block_count, block_size, pmm);
	printf("Hello world, from my kernal!\n");
	printf("This is a digit: %d ,and onther one %d \n", 1234, -1234);

	printf("%%		%%Wow look %% a string %% %s and then a char %c \n", "1234", '&');

	kprint("Starting init idt....\n");

	init_idt();

	register_interrupt_handler(33, &keyboard_call);

	kprint("Done init idt\n");

	extern uint32_t _kernel_end;
	heap_t kernel_heap_def = {0};
	kernel_heap_def.start_node = NULL;
	kernel_heap_def.start_address = &_kernel_end + PAGE_SIZE * 1000;
	kernel_heap_def.end_address = kernel_heap_def.start_address;
	kernel_heap_def.max_end_address = kernel_heap_def.end_address + PAGE_SIZE * 1024;
	kernel_heap_def.is_read_only = false;
	kernel_heap_def.is_kernel_only = true;
	kernel_heap_def.is_heap_static = false;

	heap_t * kernel_heap = self_map_heap(kernel_heap_def);

	char* new_pmm = malloc(block_count / 8, kernel_heap);
	new_pmm = memcpy(new_pmm, pmm, block_count / 8);

	pmmngr_init(block_count, block_size, new_pmm);

	void * ptr1 =  malloc(1, kernel_heap);
	void * ptr2 =  malloc(1, kernel_heap);
	void * ptr3 =  malloc(1, kernel_heap);
	void * ptr4 =  malloc(1, kernel_heap);

	free(ptr1, kernel_heap);
	free(ptr2, kernel_heap);
	free(ptr4, kernel_heap);
	free(ptr3, kernel_heap);

	void * ptr5 =  malloc(4, kernel_heap);

	void * ptr6 =  malloc(PAGE_SIZE * 3, kernel_heap);


	void *block = pmmngr_alloc_block();
	void *virt_addr = (void *)0x4000000;

	vmmngr_alloc_page(virt_addr, block, PRESENT_PAGE | READ_WRITE_PAGE);

	((char *)virt_addr)[0] = 'x';

	vmmngr_free_page(virt_addr);
	pmmngr_free_block(block);

	// int zero = 0/0;

	// __asm__("int $0x20");

	kprint("halt...\n");
	for (;;)
	{
		// int x = 1;
		__asm__("hlt");
	}
}
