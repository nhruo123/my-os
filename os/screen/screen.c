#include "vga.h"
#include "../string/string.h"
#include <stdint.h>

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

void terminalInit(void)
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0XC03FF000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void terminal_setcolor(uint8_t color)
{
	terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void print_char(char c)
{
	if (terminal_column >= VGA_WIDTH) {
		terminal_column = 0;
		terminal_row++;
	}

	if(terminal_row >= VGA_HEIGHT) {
		terminalInit();
	}

	if (c == '\n') {
		terminal_column = 0;
		terminal_row++;
	} else {
		terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
		terminal_column++;
	}
}

void terminal_write(const char* data, size_t size)
{
	if (size > (VGA_HEIGHT - terminal_row) * VGA_WIDTH - terminal_column - 1) {
		terminalInit();
	}
	for (size_t i = 0; i < size; i++) {
		print_char(data[i]);
	}
}

void kprint(const char* data)
{
	terminal_write(data, strlen(data));
}
