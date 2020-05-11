#include <stdint.h>
#include <stdio.h>
#include <mmnger/context_management.h>
#include <mmnger/mmnger_virtual.h>
#include <multitasking/task.h>
#include <stdlib.h>
#include <interrupts/timer.h>

task_list_t *task_lists;

task_t *current_active_task;

// task_t *ready_to_run_list;
// task_t *last_ready_to_run_task;

uint32_t IRQ_disable_counter;

uint32_t postpone_task_switches_counter;
uint32_t task_switches_postponed_flag;

uint32_t last_tick_counter;
uint32_t cpu_idle_time;
uint32_t current_time_slice_remaining;

void exit_task_function()
{
    lock_kernel_stuff();

    add_task_to_general_list(TERMINATED_TASK, current_active_task);

    block_current_task(TERMINATED_TASK);

    unlock_kernel_stuff();
}

static void start_task_function()
{
    unlock_scheduler();
}

static void *push_to_other_stack(uint32_t value, void *stack_top)
{
    void *return_value = stack_top - sizeof(value);

    *((uint32_t *)return_value) = value;

    return return_value;
}


static uint32_t general_list_index_from_status(uint32_t list_status) {
    return task_lists + (list_status - LOWEST_GENERAL_STATUS);
}

void add_task_to_general_list(uint32_t list_status, task_t *task)
{
    add_task_to_list(general_list_index_from_status(list_status),  task);
}

task_t *pop_task_form_general_list(uint32_t list_status)
{
    pop_task_form_list(general_list_index_from_status(list_status));
}
task_t *peek_into_general_list(uint32_t list_status) 
{
    peek_into_list(general_list_index_from_status(list_status));
}

void add_task_to_list(task_list_t* list, task_t* task) {
    if (list->first_task == NULL)
    {
        list->first_task = task;
        list->last_task = task;
    }
    else
    {
        list->last_task->next_task = task;
        list->last_task = task;
    }
}

task_t* pop_task_form_list(task_list_t* list) {
    if (list->first_task != NULL)
    {
        task_t* poped_task = list->first_task;
        list->first_task = poped_task->next_task;
        if(list->first_task == NULL) {
            list->last_task = NULL;
        }
        return poped_task;
    }
    else
    {
        return NULL;
    }
}

task_t *peek_into_list(task_list_t* list) {
    return list->first_task;
}

void init_tasking()
{
    postpone_task_switches_counter = 0;
    task_switches_postponed_flag = 0;
    IRQ_disable_counter = 0;

    last_tick_counter = 0;
    cpu_idle_time = 0;
    current_time_slice_remaining = 0;

    current_active_task = malloc(sizeof(task_t));
    current_active_task->regs.address_space = get_current_address_space();
    current_active_task->status = RUNNING;
    current_active_task->millisecond_used = 0;

    task_lists = malloc(sizeof(task_list_t) * (HIGHEST_GENERAL_STATUS - LOWEST_GENERAL_STATUS + 1));
    memset(task_lists, 0, sizeof(task_list_t) * (HIGHEST_GENERAL_STATUS - LOWEST_GENERAL_STATUS + 1));

}

// loock needs to called before calling this!!
void schedule()
{
    if (postpone_task_switches_counter != 0)
    {
        task_switches_postponed_flag = 1;
        return;
    }
    if (peek_into_general_list(READY_TO_RUN) != NULL)
    {
        
        task_t *task = pop_task_form_general_list(READY_TO_RUN);
        switch_task_warpper(task);
    }
    else if (current_active_task->status == RUNNING)
    {
        return;
    }
    else
    {
        task_t *last_active_task = current_active_task;
        current_active_task = NULL;

        postpone_task_switches_counter++;

        do
        {
            asm("STI;");
            asm("HLT;");
            asm("CLI;");
        } while (peek_into_general_list(READY_TO_RUN) == NULL);

        update_time_used();

        current_active_task = last_active_task;

        task_t *read_task = pop_task_form_general_list(READY_TO_RUN);

        postpone_task_switches_counter--;

        if (read_task != current_active_task)
        {
            switch_task_warpper(read_task);
        }
        else
        {
            current_active_task->status = RUNNING;
        }
    }
}

void milli_sleep(uint32_t milliseconds)
{
    milli_sleep_until(millisecond_since_boot + milliseconds);
}

void milli_sleep_until(uint32_t until_when)
{
    if (until_when < millisecond_since_boot)
    {
        return;
    }

    lock_kernel_stuff();

    current_active_task->sleep_expiry = until_when;
    add_task_to_general_list(SLEEPING_TASK, current_active_task);
    unlock_kernel_stuff();

    block_current_task(SLEEPING_TASK);
}

void block_current_task(uint32_t reason)
{
    lock_scheduler();

    current_active_task->status = reason;

    schedule();
    unlock_scheduler();
}

void unblock_task(task_t *task)
{
    lock_scheduler();

    task->status = READY_TO_RUN;
    if (peek_into_general_list(READY_TO_RUN) == NULL)
    {
        switch_task_warpper(task);
    }
    else
    {
        add_task_to_general_list(READY_TO_RUN, task);
    }
    unlock_scheduler();
}

void switch_task_warpper(task_t *new_task)
{
    if (postpone_task_switches_counter != 0)
    {
        task_switches_postponed_flag = 1;
        add_task_to_general_list(READY_TO_RUN, new_task);
        return;
    }

    if (current_active_task == NULL)
    {
        current_time_slice_remaining = 0;
    }
    else if ((peek_into_general_list(READY_TO_RUN) == NULL) && (current_active_task->status != RUNNING))
    {
        current_time_slice_remaining = 0;
    }
    else
    {
        current_time_slice_remaining = TAKS_TIME_SLICE;
    }

    update_time_used();

    if(current_active_task->status == RUNNING) {
        add_task_to_general_list(READY_TO_RUN, current_active_task);
        current_active_task->status = READY_TO_RUN;
    }
    

    switch_task(new_task);
}

task_t *create_task(void (*entry_point)())
{
    task_t *task = malloc(sizeof(task_t));

    address_space_t new_address_space = create_new_address_space();
    task->regs.address_space = new_address_space;

    task->status = READY_TO_RUN;
    task->next_task = NULL;
    task->millisecond_used = 0;

    page_dir_entry_t old_dir = mount_address_space_on_temp_dir(new_address_space);

    page_dir_entry_t new_stack_page_dir = *((page_dir_entry_t *)&reserved_temp_table[STACK_TABLE]);
    // now resrved temp table is the new address space stack (we dont need output cuz its old_dir)
    mount_page_dir_on_temp_dir(new_stack_page_dir);

    void *top_of_new_stack = (void *)get_page_address_from_indexes(RESERVED_TEMP_TABLE, 1023);
    void *start_of_new_stack = top_of_new_stack;

    top_of_new_stack = push_to_other_stack((uint32_t)exit_task_function, top_of_new_stack); // push exit func location

    top_of_new_stack = push_to_other_stack((uint32_t)entry_point, top_of_new_stack); // push entry point

    top_of_new_stack = push_to_other_stack((uint32_t)start_task_function, top_of_new_stack); // start func location

    top_of_new_stack = push_to_other_stack(0, top_of_new_stack);         // start func location ebx
    top_of_new_stack = push_to_other_stack(0, top_of_new_stack);         // start func location esi
    top_of_new_stack = push_to_other_stack(0, top_of_new_stack);         // start func location edi
    top_of_new_stack = push_to_other_stack(STACK_TOP, top_of_new_stack); // start func location ebp

    mount_page_dir_on_temp_dir(old_dir); // we dont need out put cuz its new_stack_page_dir

    task->regs.esp = STACK_TOP - (uint32_t)(start_of_new_stack - top_of_new_stack);
    task->regs.esp0 = task->regs.esp;

    // adds the task for ready to run list
    add_task_to_general_list(READY_TO_RUN, task);

    return task;
}

void update_time_used()
{
    uint32_t current_count = millisecond_since_boot;
    uint32_t elapsed = current_count - last_tick_counter;
    last_tick_counter = current_count;

    if (current_active_task == NULL)
    {
        cpu_idle_time += elapsed;
    }
    else
    {
        current_active_task->millisecond_used += elapsed;
    }
}