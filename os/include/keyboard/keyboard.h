#ifndef _KEYBORD_H
#define _KEYBORD_H 1

#include <io/io.h>
#include <stdio.h>
#include <interrupts/isr.h>

#define MAX_KEYBOARD_BUFFER_SIZE 512

static void keyboard_call(registers_t regs);
static char translate_scan_code_to_assci(char code);
void init_keyboard();
char get_char();

#endif