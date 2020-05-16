#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <misc/system.h>
#include <multitasking/task.h>

void enter_user_space_program(void *entry_point, int argc, char **argv, void *stack_top)
{
    asm volatile("cli");

    asm volatile(
        "movl %3, %%esp\n" // set stack
        "pushl %2 \n"
        "pushl %1 \n"
        "movw $0x20, %%ax\n"     // user data segment
        "movw %%ax, %%ds\n"
        "movw %%ax, %%es\n"
        "movw %%ax, %%fs\n"
        "movw %%ax, %%gs\n"
        "movl %%esp, %%eax\n"
        "pushl $0x20\n" // push ues data segment for iret
        "pushl %%eax\n" // push stack location for iret
        "pushf\n"       // push eflasg for iret
        "popl %%eax\n"  // set interrupt flag to true for iret to set
		"orl  $0x200, %%eax\n"
		"pushl %%eax\n"
        "pushl $0x18\n" // push user code segment for iret
        "pushl %0\n"    // push entry point for irete (eip) 
        "iret\n"        // make cpu think we return from an interrupt in order to enter uesr space
        : : "m"(entry_point), "r"(argc), "r"(argv), "r"(stack_top) : "%ax", "%esp", "%eax"
    );
}
