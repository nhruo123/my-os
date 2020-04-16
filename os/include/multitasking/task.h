#ifndef TASK_H
#define TASK_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <mmnger/context_management.h>
#include <mmnger/mmnger_virtual.h>

#define RUNNING         0
#define READY_TO_RUN    1
#define TERMINATED_TASK 2
#define SLEEPING_TASK   3


typedef struct task_regs_s {
    uint32_t esp, esp0;
    address_space_t address_space;
} task_regs_t; // size is (4 + 4 + 4) == 12

typedef struct task_s {
    task_regs_t regs;
    struct task_s *next_task;
    uint32_t status;
    uint32_t millisecond_used;
    uint32_t sleep_expiry;
} task_t; // size is 12 + 4 + 4 + 4 == (24)


extern task_t *current_active_task;
extern task_t *sleeping_task_list;

extern task_t *ready_to_run_list;
extern task_t *last_task;

void exit_task_function();
static void start_task_function();
static void *push_to_other_stack(uint32_t value, void* stack_top);


void lock_scheduler(); 
void unlock_scheduler();

void lock_kernel_stuff();
void unlock_kernel_stuff();


void milli_sleep(uint32_t milliseconds);
void milli_sleep_until(uint32_t until_when);

void update_time_used();
void push_task_back_to_ready(task_t *task);

void block_current_task(uint32_t reason);
void unblock_task(task_t * task);

void init_tasking();
task_t * create_task(void (* entry_point)());
void schedule();




extern void switch_task(task_t *new_task);


#endif