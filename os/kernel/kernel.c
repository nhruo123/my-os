#include <stdio.h>
#include <stdlib.h>

#include <mmnger/mmnger_virtual.h>
#include <mmnger/mmnger_phys.h>
#include <interrupts/interrupts.h>
#include <interrupts/isr.h>

#include "../keyboard/keyboard.h"
#include "../multiboot.h"

void main(multiboot_info_t *mbt, char *pmm, int block_count, int block_size) {
	terminalInit();
	pmmngr_init(block_count, block_size, pmm);
	printf("Hello world, from my kernal!\n");
	printf("This is a digit: %d ,and onther one %d \n", 1234, -1234);

	printf("%%		%%Wow look %% a string %% %s and then a char %c \n", "1234", '&');

	kprint("Starting init idt....\n");

	init_idt();

	register_interrupt_handler(33, &keyboard_call);

	kprint("Done init idt\n");

	extern uint32_t heap_start;
	extern uint32_t heap_end;

	heap_t static_heap;
	static_heap.start_address = &heap_start;
	static_heap.end_address = &heap_end;

	heap_t * self_maped_heap = self_map_heap(static_heap);

	size_t loop = 100;
	for (size_t i = 0; i < loop; i++)
	{
		void *ptr = malloc(sizeof(char), self_maped_heap);
		void *ptr_hw = malloc(sizeof(uint16_t), self_maped_heap);
		*(char *)ptr = 'x';
		free(ptr, self_maped_heap);
	}

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
