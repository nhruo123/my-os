#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <interrupts/isr.h>
#include <interrupts/syscall.h>

static void *syscalls[SYSCALL_COUNT] = {
    abort,
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

DEFN_SYSCALL0(abort, 0)