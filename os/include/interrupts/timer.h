#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <stdio.h>
#include <interrupts/isr.h>

extern uint32_t millisecond_since_boot;

void init_timer();

void timer_interrupt_handler(registers_t *regs);

#endif