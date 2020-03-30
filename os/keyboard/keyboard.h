#include <io/io.h>
#include <stdio.h>
#include <interrupts/isr.h>


static void keyboard_call(registers_t regs) {
    unsigned char scan_code = inb(0x60);
    
    printf("keyboard interrupt was called, int_no is: %d , and scan code is %c \n", regs.int_no, scan_code);
}
