#include <stdint.h>
#include <stdio.h>
#include <mmnger/context_management.h>
#include <mmnger/mmnger_virtual.h>
#include <multitasking/task.h>
#include <stdlib.h>

task_t *current_active_task;

void init_tasking() {
    current_active_task = malloc(sizeof(task_t));
    current_active_task->regs.address_space = get_current_address_space();
}

task_t * create_task(void (* entry_point)()) {
    task_t *task = malloc(sizeof(task_t));
    address_space_t new_address_space = create_new_address_space();
    task->regs.address_space = new_address_space;
    task->regs.esp = STACK_TOP - (4 * 5); // we push eip and the 4 regs into the stack so we remove (4 * 5)
    task->regs.esp0 = task->regs.esp;

    page_dir_entry_t old_dir = mount_address_space_on_temp_dir(new_address_space);

    page_dir_entry_t new_stack_page_dir = *((page_dir_entry_t *)&reserved_temp_table[STACK_TABLE]);
    // now resrved temp table is the new address space stack (we dont need output cuz its old_dir)
    mount_page_dir_on_temp_dir(new_stack_page_dir);

    // now we push the eip onto the stack
    void *top_of_new_stack = (void *)get_page_address_from_indexes(RESERVED_TEMP_TABLE,1023);
    top_of_new_stack = (top_of_new_stack - (4));
    *((uint32_t *)top_of_new_stack) = (uint32_t)entry_point;



    mount_page_dir_on_temp_dir(old_dir); // we dont need out put cuz its new_stack_page_dir

    return task;
}