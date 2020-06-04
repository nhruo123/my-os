#ifndef _EXCEPTIONS_H
#define _EXCEPTIONS_H 1

#define PRESENT_PAGE_FAULT      0b00001
#define WRITE_PAGE_FAULT        0b00010
#define USER_MODE_PAGE_FAULT    0b00100

void page_fault_handler(registers_t* regs);


#endif