#include <stdio.h>
#include <stdlib.h>

#include <mmnger/mmnger_virtual.h>
#include <mmnger/mmnger_phys.h>
#include <mmnger/context_management.h>
#include <interrupts/interrupts.h>
#include <interrupts/isr.h>
#include <multitasking/task.h>

#include <multiboot.h>
#include <screen/screen.h>
#include <interrupts/timer.h>

#include "../keyboard/keyboard.h"
#include "./kernel.h"

void main(multiboot_info_t *mbt, heap_t *bootstrap_heap)
{
	init_gdt();
	set_current_heap(bootstrap_heap);
	clear_screen();

	// init base memmory mangemnt
	pmmngr_init(mbt);
	init_vmmngr();

	init_context();

	init_screen(mbt);

	clear_screen();

	// init interrupts
	kprint("Starting init idt....\n");

	init_idt();
	init_timer();

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

	pmmngr_change_heap();

	init_tasking();

	task_t *new_task = create_task(test_heap);

	kprint("halt...\n");
	// for (;;)
	// {
	// 	// int x = 1;
	// 	__asm__("hlt");
	// 	lock_scheduler();
	// 	schedule();
	// 	unlock_scheduler();
	// }

	exit_task_function();
}

void test_heap()
{
	milli_sleep(5000);

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