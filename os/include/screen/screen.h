#ifndef _SCREEN_H
#define _SCREEN_H 1

#include <stdint.h>
#include <stddef.h>
#include <multiboot.h>

void init_screen(multiboot_info_t *mbt);

void kprint(const char* data);
void print_char(char c);
void clear_screen(void);
void terminal_write(const char* data, size_t size);

#endif