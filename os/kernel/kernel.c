#include <stdio.h>
#include <stdlib.h>

#include <mmnger/mmnger_virtual.h>
#include <mmnger/mmnger_phys.h>
#include <mmnger/context_management.h>
#include <interrupts/interrupts.h>
#include <interrupts/isr.h>
#include <multitasking/task.h>
#include <fs/vfs.h>
#include <fs/ustars_fs.h>

#include <misc/elf.h>

#include <disks/disk.h>
#include <disks/ramdisk.h>

#include <multiboot.h>
#include <screen/screen.h>
#include <interrupts/timer.h>
#include <interrupts/exceptions.h>
#include <interrupts/syscall.h>

#include <keyboard/keyboard.h>
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
	register_interrupt_handler(14, page_fault_handler);
	init_syscalls();

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
	init_heap_mutex();
	init_tasking();
	init_timer();

	init_keyboard();

	// printf("type 15 times to coninue \n");
	// for (size_t i = 0; i < 15; i++)
	// {
	// 	putchar(getchar());
	// }
	// putchar('\n');

	init_disks();

	disk_t *ram_disk = create_ram_disk_form_mbt("initrd", "initrd", mbt);

	register_disk(ram_disk);

	init_vfs();

	filesystem_t *test_fs = create_ustar_fs("test fs");

	if (test_fs->probe(ram_disk) == 0)
	{
		ram_disk->fs = test_fs;
		mount_disk(ram_disk, "a");
	}

	task_t *new_task = create_task(test_heap, 0);

	wait_for_task_to_exit(new_task);

	create_task(start_shell, 1, "a:hello");

	kprint("halt...\n");

	exit_task_function();
}

void start_shell(char * shell_name)
{
	char file_value[200] = {0};
	char file_name[200] = {0};

	dir_entry_t dir_entry;
	file_stats_t stats;

	char *test = "a:";
	readdir_vfs(test, &dir_entry, 0);
	strcpy(file_name, test);
	strcpy(strchr(file_name, ':') + 1, dir_entry.filename);
	read_vfs(file_name, file_value, 0, 200);
	stats_vfs(file_name, &stats);
	printf("dir entry is: %s \nAt size: %d \nAnd the file contet is: %s \n", dir_entry.filename, stats.size, file_value);

	exec(shell_name, 0, NULL);
}

void test_heap()
{
	milli_sleep(500);
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