#ifndef TASK_H
#define TASK_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <mmnger/context_management.h>
#include <mmnger/mmnger_virtual.h>


#define LOWEST_GENERAL_STATUS   1

#define RUNNING                 0
#define READY_TO_RUN            1
#define TERMINATED_TASK         2
#define SLEEPING_TASK           3

#define HIGHEST_GENERAL_STATUS  3

#define WAITING_FOR_LOCK        4
#define WAITING_FOR_TASK_EXIT   5
#define WAITING_FOR_HARDWARE    6

#define TAKS_TIME_SLICE         50 // im millisecond
#define ONLY_TASK_RUNNING       0

extern uint32_t IRQ_disable_counter;
extern uint32_t postpone_task_switches_counter;
extern uint32_t task_switches_postponed_flag;
extern bool is_multitasking_init;

typedef struct task_regs_s {
    uint32_t esp, esp0;
    address_space_t address_space;
} task_regs_t; // size is (4 + 4 + 4) == 12


struct task_list_s {
    struct task_s* first_task;
    struct task_s* last_task;
};

typedef struct task_list_s task_list_t;

typedef struct task_s {
    task_regs_t regs;
    struct task_s *next_task;
    uint32_t status;
    uint32_t millisecond_used;
    uint32_t sleep_expiry;
    struct task_list_s tasks_wating_for_exit;
    uint32_t pid;
    struct heap_s user_heap;
    uint32_t user_stack_top;
} task_t; // size is 12 + 4 + 4 + 4 == (24)



typedef struct semaphore_s {
    uint32_t max_count;
    uint32_t current_count;
    task_list_t task_list;
} semaphore_t;

extern uint32_t current_time_slice_remaining;

extern task_t *current_active_task;
extern task_list_t *task_lists;



void add_task_to_list(task_list_t* list, task_t* task);
task_t* pop_task_form_list(task_list_t* list);
task_t *peek_into_list(task_list_t* list);

void add_task_to_general_list(uint32_t list_status, task_t *task);
task_t *pop_task_form_general_list(uint32_t list_status);
task_t *peek_into_general_list(uint32_t list_status);

void exit_task_function();
static void start_task_function();
static void *push_to_other_stack(uint32_t value, void* stack_top);


void wait_for_task_to_exit(task_t* task);

void lock_scheduler(); 
void unlock_scheduler();

void lock_kernel_stuff();
void unlock_kernel_stuff();

semaphore_t *create_semaphore(uint32_t max_count);
semaphore_t *create_mutex();
void acquire_semaphore(semaphore_t * semaphore);
void acquire_mutex(semaphore_t * semaphore);
void release_semaphore(semaphore_t * semaphore);
void release_mutex(semaphore_t * semaphore);


void milli_sleep(uint32_t milliseconds);
void milli_sleep_until(uint32_t until_when);

void update_time_used();

void block_current_task(uint32_t reason);
void unblock_task(task_t * task);

void init_tasking();
task_t *create_task(void (*entry_point)(),uint32_t argc, ...);
void schedule();


void switch_task_warpper(task_t *new_task);

extern void switch_task(task_t *new_task);


#endif