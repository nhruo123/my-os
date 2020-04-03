#ifndef _SCREEN_H
#define _SCREEN_H 1

#include <stdint.h>
#include <stddef.h>

void kprint(const char* data);
void print_char(char c);
void terminalInit(void);
void terminal_write(const char* data, size_t size);

#endif