#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <interrupts/isr.h>

#define SYSCALL_COUNT 6

static void syscall_handler(registers_t regs);
void init_syscalls();








#endif