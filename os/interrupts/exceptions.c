#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <interrupts/isr.h>
#include <interrupts/interrupts.h>
#include <interrupts/exceptions.h>
#include <multitasking/task.h>


void page_fault_handler(registers_t *regs) {
    if((regs->err_code & USER_MODE_PAGE_FAULT) == 0)
    {
        printf("Page fault exeptions in kernel mode!!!! \n");

        printf("eip was: 0x%x \n", regs->eip);

        abort();
    } 
    else
    {
        printf("Page fault at user mode killing current task....\n");
        exit_task_function();
    }
}