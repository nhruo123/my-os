#include <stdint.h>
#include <stdio.h>
#include <mmnger/context_management.h>
#include <mmnger/mmnger_virtual.h>
#include <multitasking/task.h>
#include <stdlib.h>
#include <interrupts/timer.h>
#include <limits.h>
#include <stdarg.h>

task_t **global_tasks_list;

task_list_t *task_lists;

task_t *current_active_task;

uint32_t IRQ_disable_counter = 0;

uint32_t postpone_task_switches_counter = 0;
uint32_t task_switches_postponed_flag = 0;
bool is_multitasking_init = false;

uint32_t last_tick_counter;
uint32_t cpu_idle_time;
uint32_t current_time_slice_remaining;
uint32_t next_pid;

void exit_task_function()
{
    lock_kernel_stuff();

    task_t *wating_task = pop_task_form_list(&current_active_task->tasks_wating_for_exit);

    while (wating_task != NULL)
    {
        unblock_task(wating_task);
        wating_task = pop_task_form_list(&current_active_task->tasks_wating_for_exit);
    }

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

static uint32_t general_list_index_from_status(uint32_t list_status)
{
    return task_lists + (list_status - LOWEST_GENERAL_STATUS);
}

void add_task_to_general_list(uint32_t list_status, task_t *task)
{
    add_task_to_list(general_list_index_from_status(list_status), task);
}

task_t *pop_task_form_general_list(uint32_t list_status)
{
    pop_task_form_list(general_list_index_from_status(list_status));
}
task_t *peek_into_general_list(uint32_t list_status)
{
    peek_into_list(general_list_index_from_status(list_status));
}

void add_task_to_list(task_list_t *list, task_t *task)
{
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

task_t *pop_task_form_list(task_list_t *list)
{
    if (list->first_task != NULL)
    {
        task_t *poped_task = list->first_task;
        list->first_task = poped_task->next_task;
        poped_task->next_task = NULL;
        if (list->first_task == NULL)
        {
            list->last_task = NULL;
        }
        return poped_task;
    }
    else
    {
        return NULL;
    }
}

task_t *peek_into_list(task_list_t *list)
{
    return list->first_task;
}

void init_tasking()
{
    global_tasks_list = calloc(MAX_PID, sizeof(task_t));
    next_pid = 1;

    is_multitasking_init = true;
    last_tick_counter = 0;
    cpu_idle_time = 0;
    current_time_slice_remaining = 0;

    current_active_task = calloc(1, sizeof(task_t));

    current_active_task->pid = 0;
    current_active_task->regs.address_space = get_current_address_space();
    current_active_task->status = RUNNING;
    current_active_task->millisecond_used = 0;

    task_lists = calloc((HIGHEST_GENERAL_STATUS - LOWEST_GENERAL_STATUS + 1), sizeof(task_list_t));

    global_tasks_list[0] = current_active_task;
}

// loock needs to called before calling this!!
void schedule()
{
    if (!is_multitasking_init)
    {
        return;
    }

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

void wait_for_task_to_exit(task_t *task)
{
    lock_kernel_stuff();

    if(task->status != TERMINATED_TASK) {
        add_task_to_list(&task->tasks_wating_for_exit, current_active_task);
        block_current_task(WAITING_FOR_TASK_EXIT);
    }

    unlock_kernel_stuff();
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

    if (current_active_task->status == RUNNING)
    {
        add_task_to_general_list(READY_TO_RUN, current_active_task);
        current_active_task->status = READY_TO_RUN;
    }

    switch_task(new_task);
}

static task_t *create_empty_task()
{
    lock_kernel_stuff();

    if (next_pid >= MAX_PID)
    {
        printf("RAN OUT OF  PIDS!!!\n");
        abort();
    }

    task_t *task = calloc(1, sizeof(task_t));

    address_space_t new_address_space = create_new_address_space();
    task->regs.address_space = new_address_space;

    task->status = READY_TO_RUN;
    task->next_task = NULL;
    task->millisecond_used = 0;
    task->pid = next_pid;

    global_tasks_list[next_pid] = task;

    next_pid++;

    unlock_kernel_stuff();
    return task;
}

task_t *create_task(void (*entry_point)(), uint32_t argc, ...)
{
    task_t *task = create_empty_task();
    page_dir_entry_t old_dir = mount_address_space_on_temp_dir(task->regs.address_space);

    page_dir_entry_t new_stack_page_dir = *((page_dir_entry_t *)&reserved_temp_table[STACK_TABLE]);
    // now resrved temp table is the new address space stack (we dont need output cuz its old_dir)
    mount_page_dir_on_temp_dir(new_stack_page_dir);

    void *top_of_new_stack = (void *)get_page_address_from_indexes(RESERVED_TEMP_TABLE, 1023);
    void *start_of_new_stack = top_of_new_stack;

    // params
    va_list parameters;
    va_start(parameters, argc);

    for (size_t index = 0; index < argc; index++)
    {
        uint32_t current_arg = va_arg(parameters, uint32_t);
        top_of_new_stack = push_to_other_stack(current_arg, top_of_new_stack);
    }

    va_end(parameters);

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

uint32_t fork()
{
    lock_kernel_stuff();
    task_t *new_task = create_empty_task();

    new_task->user_stack_top = current_active_task->user_stack_top;
    new_task->regs.esp0 = current_active_task->regs.esp0;
    new_task->user_heap = current_active_task->user_heap;

    address_space_t new_adder_space = new_task->regs.address_space;
    page_dir_entry_t old_reserved_temp_table = mount_address_space_on_temp_dir(new_adder_space);

    page_directory_t page_dir = (page_directory_t)get_page_address_from_indexes(LOOP_BACK_TABLE, RESERVED_TEMP_TABLE);

    for (size_t page_table_index = 0; (page_table_index * PAGE_SIZE * PAGE_SIZE) < KERNEL_SPACE; page_table_index++)
    {
        // we set the new addres space to the cloned page table
        page_dir_entry_t new_page_table = clone_page_table(page_table_index);
        page_dir[page_table_index] = new_page_table;
    }

    register volatile uint32_t *ebp_as_pointer asm ("ebp");
    register volatile uint32_t ebp asm ("ebp");
    register volatile uint32_t edi asm ("edi");
    register volatile uint32_t esi asm ("esi");
    register volatile uint32_t ebx asm ("ebx");

    ebx = esi = edi = 1;


    
    mount_page_dir_on_temp_dir(page_dir[STACK_TABLE]);
    void *top_of_new_stack = (void *)get_page_address_from_indexes(RESERVED_TEMP_TABLE, 0);
    top_of_new_stack = ((uint32_t)top_of_new_stack & 0xFFC00000) | ((ebp + 0x4) & 0x3FFFFF);
    
    top_of_new_stack = push_to_other_stack((uint32_t)start_task_function, top_of_new_stack); // start func location

    top_of_new_stack = push_to_other_stack(0, top_of_new_stack);         // start func parameter
    top_of_new_stack = push_to_other_stack((uint32_t)fork_wrapper, top_of_new_stack); // start func location
    
    top_of_new_stack = push_to_other_stack(*(ebp_as_pointer - 3), top_of_new_stack);         // start func location ebx
    top_of_new_stack = push_to_other_stack(*(ebp_as_pointer - 2), top_of_new_stack);         // start func location esi
    top_of_new_stack = push_to_other_stack(*(ebp_as_pointer - 1), top_of_new_stack);         // start func location edi
    top_of_new_stack = push_to_other_stack(*ebp_as_pointer, top_of_new_stack); // start func location ebp

    new_task->regs.esp  = (get_page_address_from_indexes(STACK_TABLE,0) & 0xFFC00000) | ((uint32_t)top_of_new_stack & 0x3FFFFF);

    mount_page_dir_on_temp_dir(old_reserved_temp_table);

    add_task_to_general_list(READY_TO_RUN, new_task);

    unlock_kernel_stuff();

    return new_task->pid;
}

void waitpid(uint32_t pid)
{
    if(pid < MAX_PID && global_tasks_list[pid] != NULL && global_tasks_list[pid]->status != TERMINATED_TASK)
    {
        wait_for_task_to_exit(global_tasks_list[pid]);
    }
}