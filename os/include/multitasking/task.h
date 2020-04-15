#ifndef TASK_H
#define TASK_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <mmnger/context_management.h>
#include <mmnger/mmnger_virtual.h>

typedef struct task_regs_s {
    uint32_t esp, esp0;
    address_space_t address_space;
} task_regs_t;

typedef struct task_s {
    task_regs_t regs;
    struct task_s *next_task;
} task_t;


extern task_t *current_active_task;



void init_tasking();
task_t * create_task(void (* entry_point)());

extern void switch_task(task_t *new_task);



#endif