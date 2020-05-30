#include <io/io.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <interrupts/isr.h>
#include <keyboard/keyboard.h>
#include <multitasking/task.h>

static bool caps_is_pressed = false;
static bool shift_is_pressed = false;
static char char_keyboard_buffer[MAX_KEYBOARD_BUFFER_SIZE] = {0};
static size_t buffer_index = 0;
static semaphore_t *keyboard_lock;
static task_list_t *waiting_for_keyboard;

static void keyboard_call(registers_t *regs)
{
    unsigned char scan_code = inb(0x60);

    if (scan_code == 0x3A)
    {
        caps_is_pressed = !caps_is_pressed;
        return;
    }

    if (scan_code == 0x2A || scan_code == 0x36)
    {
        shift_is_pressed = true;
        return;
    }
    if (scan_code == 0xAA || scan_code == 0xB6)
    {
        shift_is_pressed = false;
        return;
    }

    if (translate_scan_code_to_assci(scan_code) != 0)
    {

        if (buffer_index >= MAX_KEYBOARD_BUFFER_SIZE)
        {
            buffer_index = 0;
        }

        char_keyboard_buffer[buffer_index] = translate_scan_code_to_assci(scan_code);
        buffer_index++;

        while (peek_into_list(waiting_for_keyboard) != NULL)
        {
            unblock_task(pop_task_form_list(waiting_for_keyboard));
        }
    }
}

// zero is an error
static char translate_scan_code_to_assci(char code)
{
    if (code == 0x39) // space
        return ' ';

    if (code == 0xE) // back space
        return '\r';

    if (code == 0xF) // tab space
        return '\t';

    if (code >= 0x02 && code <= 0x0B) // numbers 0-9
    {
        if (shift_is_pressed)
            return "!@#$%^&*()_+"[code - 0x02];
        else
            return "1234567890-="[code - 0x02];
    }

    if (code >= 0x10 && code <= 0x19) // Q-P
    {
        if (caps_is_pressed || shift_is_pressed)
            return "QWERTYUIOP"[code - 0x10];
        else
            return "qwertyuiop"[code - 0x10];
    }
    if (code >= 0x1E && code <= 0x26) // A-L
    {
        if (caps_is_pressed || shift_is_pressed)
            return "ASDFGHJKL"[code - 0x1E];
        else
            return "asdfghjkl"[code - 0x1E];
    }
    if (code >= 0x2C && code <= 0x32) // Z-M
    {
        if (caps_is_pressed || shift_is_pressed)
            return "ZXCVBNM"[code - 0x2C];
        else
            return "zxcvbnm"[code - 0x2C];
    }

    if (code >= 0x1A && code <= 0x1C) // [- \n
    {
        if (shift_is_pressed)
            return "{}\n}"[code - 0x1A];
        else
            return "[]\n"[code - 0x1A];
    }

    if (code >= 0x27 && code <= 0x29) // ;- `
    {
        if (shift_is_pressed)
            return ":\"~"[code - 0x27];
        else
            return ";'`"[code - 0x27];
    }

    if (code >= 0x33 && code <= 0x35) // ,- /
    {
        if (shift_is_pressed)
            return "<>?"[code - 0x33];
        else
            return ",./"[code - 0x33];
    }

    return 0;
}

void init_keyboard()
{
    waiting_for_keyboard = calloc(1, sizeof(task_list_t));
    keyboard_lock = create_mutex();

    register_interrupt_handler(33, &keyboard_call);
}

char get_char()
{
    char char_to_return;
    acquire_mutex(keyboard_lock);
    if (buffer_index == 0)
    {
        add_task_to_list(waiting_for_keyboard, current_active_task);
        block_current_task(WAITING_FOR_HARDWARE);
    }

    buffer_index--;
    char_to_return = char_keyboard_buffer[buffer_index];
    

    release_mutex(keyboard_lock);

    return char_to_return;
}