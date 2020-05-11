#include <stdint.h>
#include <stdio.h>
#include <mmnger/context_management.h>
#include <mmnger/mmnger_virtual.h>
#include <multitasking/task.h>
#include <stdlib.h>
#include <interrupts/timer.h>


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


semaphore_t *create_semaphore(uint32_t max_count) {
    semaphore_t *semaphore;
 
    semaphore = malloc(sizeof(semaphore_t));
    if(semaphore != NULL) {
        semaphore->max_count = max_count;
        semaphore->current_count = 0;
        semaphore->task_list.first_task = NULL;
        semaphore->task_list.last_task = NULL;
    }

    return semaphore;
}
 
semaphore_t *create_mutex() {
    return create_semaphore(1);
}

void acquire_semaphore(semaphore_t * semaphore) {
    lock_kernel_stuff();
    if(semaphore->current_count < semaphore->max_count) {
        semaphore->current_count++;
    } else {
        current_active_task->next_task = NULL;
        add_task_to_list(&semaphore->task_list , current_active_task);
        block_current_task(WAITING_FOR_LOCK);
    }
    unlock_kernel_stuff();
}
 
void acquire_mutex(semaphore_t * semaphore) {
    acquire_semaphore(semaphore);
}

void release_semaphore(semaphore_t * semaphore) {
    lock_kernel_stuff();

    task_t* task_to_release = pop_task_form_list(&semaphore->task_list);
    if(task_to_release != NULL) { 
        unblock_task(task_to_release);
    } else {
        semaphore->current_count--;
    }
    unlock_kernel_stuff();
}
 
void release_mutex(semaphore_t * semaphore) {
    release_semaphore(semaphore);
}