#include "vga.h"
#include <string.h>
#include <stdint.h>
#include <screen/screen.h>
#include <multiboot.h>
#include <mmnger/context_management.h>
#include <mmnger/mmnger_virtual.h>

static size_t VGA_WIDTH = 80;
static size_t VGA_HEIGHT = 25;
static uint16_t *terminal_buffer = 0xB8000;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;

void init_screen(multiboot_info_t *mbt)
{
	uint32_t new_framebuffer_addr = get_page_address_from_indexes(SCREEN_TABLE, 0);
	size_t bytes_in_screen = (((mbt->framebuffer_height + mbt->framebuffer_width) * mbt->framebuffer_bpp) / 8);

	for (size_t bytes_allocated = 0; bytes_allocated < bytes_in_screen; bytes_allocated += PAGE_SIZE)
	{
		vmmngr_alloc_page(new_framebuffer_addr + (bytes_allocated),
						  mbt->framebuffer_addr_lower + (bytes_allocated),
						  KERNEL_FLAGS);
	}

	terminal_buffer = new_framebuffer_addr;
	VGA_WIDTH = mbt->framebuffer_width;
	VGA_HEIGHT = mbt->framebuffer_height;
}

void clear_screen(void)
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	for (size_t y = 0; y < VGA_HEIGHT; y++)
	{
		for (size_t x = 0; x < VGA_WIDTH; x++)
		{
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
	if (terminal_column >= VGA_WIDTH)
	{
		terminal_column = 0;
		terminal_row++;
	}

	if (terminal_row >= VGA_HEIGHT)
	{
		clear_screen();
	}

	if (c == '\n')
	{
		terminal_column = 0;
		terminal_row++;
	}
	else
	{
		terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
		terminal_column++;
	}
}

void terminal_write(const char *data, size_t size)
{
	if (size > (VGA_HEIGHT - terminal_row) * VGA_WIDTH - terminal_column - 1)
	{
		clear_screen();
	}
	for (size_t i = 0; i < size; i++)
	{
		print_char(data[i]);
	}
}

void kprint(const char *data)
{
	terminal_write(data, strlen(data));
}
