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


	void * ptr1 =  malloc(1, bootstrap_heap);
	void * ptr2 =  malloc(1, bootstrap_heap);
	void * ptr3 =  malloc(1, bootstrap_heap);
	void * ptr4 =  malloc(1, bootstrap_heap);

	free(ptr1, bootstrap_heap);
	free(ptr2, bootstrap_heap);
	free(ptr4, bootstrap_heap);
	free(ptr3, bootstrap_heap);

	void * ptr5 =  malloc(4, bootstrap_heap);


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
