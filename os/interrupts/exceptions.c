#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <interrupts/isr.h>
#include <interrupts/interrupts.h>
#include <interrupts/exceptions.h>


void page_fault_handler(registers_t regs) {
    printf("Page fault exeptions \n");

    printf("eip was: 0x%x \n", regs.eip);



    abort();
}