#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <interrupts/isr.h>
#include <mmnger/mmnger_virtual.h>
#include <mmnger/context_management.h>
#include <multitasking/task.h>
#include <interrupts/syscall.h>
#include <stdlib.h>
#include <misc/elf.h>
#include <misc/elf.h>
#include <fs/vfs.h>

static int allocate_page_for_user(void *loc)
{
    if (loc < KERNEL_SPACE)
    {
        vmmngr_alloc_page_and_phys(loc, USER_FLAGS);
        return 0;
    }

    return 1;
}


static void *syscalls[SYSCALL_COUNT] = {
    // general
    exit_task_function,

    // io
    putchar,
    getchar,
    // virtual mem
    allocate_page_for_user,

    // multi tasking
    fork,
    waitpid,

    // core images
    exec,

    // fs
    mk_file_vfs,
    readdir_vfs,
    read_vfs,
    write_vfs,
};

void init_syscalls()
{
    register_interrupt_handler(0x80, syscall_handler);
}

static void syscall_handler(registers_t *regs)
{

    if (regs->eax >= SYSCALL_COUNT)
    {
        return;
    }

    void *handler_location = syscalls[regs->eax];

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
                 : "r"(regs->edi), "r"(regs->esi), "r"(regs->edx), "r"(regs->ecx), "r"(regs->ebx), "r"(handler_location));

    regs->eax = ret_value;
}