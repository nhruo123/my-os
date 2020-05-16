#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <interrupts/isr.h>
#include <mmnger/mmnger_virtual.h>
#include <multitasking/task.h>
#include <interrupts/syscall.h>
#include <stdlib.h>
#include <misc/elf.h>

static int allocate_page_for_user(void *loc)
{
    if (loc < 0X30000000)
    {
        vmmngr_alloc_page_and_phys(loc, USER_FLAGS);
        return 0;
    }

    return 1;
}

static void user_exec(void *loc, int argc, char** argv)
{
    exec(loc,argc,argv);
}

static int user_start_task(void *loc, int argc, char** argv)
{
    create_task(user_exec,3, loc, argc, argv);
    return 0;
}

static int user_start_task_and_block(void *loc, int argc, char** argv)
{
    task_t* task = create_task(user_exec,3, loc, argc, argv);
    wait_for_task_to_exit(task);
    return 0;
}



static void *syscalls[SYSCALL_COUNT] = {
    exit_task_function,
    putchar,
    getchar,
    allocate_page_for_user,
    user_start_task,
    user_start_task_and_block,
};

void init_syscalls()
{
    register_interrupt_handler(0x80, syscall_handler);
}

static void syscall_handler(registers_t regs)
{
    if (regs.eax >= SYSCALL_COUNT)
    {
        return;
    }

    void *handler_location = syscalls[regs.eax];

    uint32_t ret_value;
    asm volatile(" \
     push %1; \
     push %2; \
     push %3; \
     push %4; \
     push %5; \
     call *%6; \
     pop %%ebx; \
     pop %%ebx; \
     pop %%ebx; \
     pop %%ebx; \
     pop %%ebx; \
   "
                 : "=a"(ret_value)
                 : "r"(regs.edi), "r"(regs.esi), "r"(regs.edx), "r"(regs.ecx), "r"(regs.ebx), "r"(handler_location));

    regs.eax = ret_value;
}