#include <stdint.h>
#include <stdio.h>
#include <mmnger/context_management.h>
#include <mmnger/mmnger_virtual.h>
#include <multitasking/task.h>
#include <stdlib.h>
#include <interrupts/timer.h>

task_t *current_active_task;

task_t *ready_to_run_list;
task_t *last_ready_to_run_task;

task_t *termintaed_task_list;
task_t *blocked_task_list;

task_t *sleeping_task_list;

uint32_t IRQ_disable_counter;

uint32_t last_tick_counter;

uint32_t postpone_task_switches_counter;
uint32_t task_switches_postponed_flag;

static void exit_task_function()
{
    lock_scheduler();
    termintaed_task_list = current_active_task;
    current_active_task->next_task = termintaed_task_list;

    current_active_task->status = TERMINATED_TASK;
    schedule();
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

void init_tasking()
{
    postpone_task_switches_counter = 0;
    task_switches_postponed_flag = 0;

    IRQ_disable_counter = 0;
    last_tick_counter = 0;

    current_active_task = malloc(sizeof(task_t));
    current_active_task->regs.address_space = get_current_address_space();
    current_active_task->status = RUNNING;
    current_active_task->millisecond_used = 0;

    sleeping_task_list = NULL;
    blocked_task_list = NULL;
    ready_to_run_list = NULL;
    last_ready_to_run_task = NULL;
    termintaed_task_list = NULL;
}

// loock needs to called before calling this!!
void schedule()
{
    if (postpone_task_switches_counter != 0)
    {
        task_switches_postponed_flag = 1;
        return;
    }
    if (ready_to_run_list != NULL)
    {
        task_t *task = ready_to_run_list;
        ready_to_run_list = task->next_task;
        switch_task(task);
    }
}

// locks for scheduler
void lock_scheduler()
{
    asm("CLI;");
    IRQ_disable_counter++;
}

void unlock_scheduler()
{
    IRQ_disable_counter--;
    if (IRQ_disable_counter == 0)
    {
        asm("STI;");
    }
}

// locks for kernel use

void lock_kernel_stuff()
{
    asm("CLI;");
    IRQ_disable_counter++;
    postpone_task_switches_counter++;
}

void unlock_kernel_stuff()
{
    postpone_task_switches_counter--;
    if (postpone_task_switches_counter == 0)
    {
        if (task_switches_postponed_flag != 0)
        {
            task_switches_postponed_flag = 0;
            schedule();
        }
    }
    IRQ_disable_counter--;
    if (IRQ_disable_counter == 0)
    {
        asm("STI;");
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

    current_active_task->next_task = sleeping_task_list;
    sleeping_task_list = current_active_task;

    unlock_kernel_stuff();

    block_current_task(SLEEPING_TASK);
}

void block_current_task(uint32_t reason)
{
    lock_scheduler();

    current_active_task->next_task = blocked_task_list;
    blocked_task_list = current_active_task;

    current_active_task->status = reason;

    schedule();
    unlock_scheduler();
}

void unblock_task(task_t *task)
{
    lock_scheduler();

    task->status = READY_TO_RUN;
    if (ready_to_run_list == NULL)
    {
        switch_task(task);
    }
    else
    {
        last_ready_to_run_task->next_task = task;
        last_ready_to_run_task = task;
    }
    unlock_scheduler();
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
    if (ready_to_run_list == NULL)
    {
        ready_to_run_list = task;
        last_ready_to_run_task = task;
    }
    else
    {
        last_ready_to_run_task->next_task = task;
        last_ready_to_run_task = task;
    }

    return task;
}

void update_time_used()
{
    uint32_t current_count = millisecond_since_boot;
    uint32_t elapsed = current_count - last_tick_counter;
    last_tick_counter = current_count;
    current_active_task->millisecond_used += elapsed;
}

void push_task_back_to_ready(task_t *task) {
    if(ready_to_run_list == NULL) {
        ready_to_run_list = task;
        
    }

    task->next_task = last_ready_to_run_task;
    last_ready_to_run_task = task;
    
}