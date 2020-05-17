#include <stdint.h>
#include <stdio.h>
#include <interrupts/isr.h>
#include <interrupts/timer.h>
#include <io/io.h>
#include <multitasking/task.h>

uint32_t millisecond_since_boot;

void init_timer()
{
    millisecond_since_boot = 0;
    register_interrupt_handler(IRQ0, timer_interrupt_handler);

    uint32_t divisor = 1193180 / 1000;

    outb(0x43, 0x36);

    // Divisor has to be sent byte-wise, so split here into upper/lower bytes.
    uint8_t l = (uint8_t)(divisor & 0xFF);
    uint8_t h = (uint8_t)((divisor >> 8) & 0xFF);

    // Send the frequency divisor.
    outb(0x40, l);
    outb(0x40, h);
}

void timer_interrupt_handler(registers_t* regs)
{
    lock_kernel_stuff();

    millisecond_since_boot++;

    task_t *sleeping_task = pop_task_form_general_list(SLEEPING_TASK);

    if (sleeping_task != NULL)
    {
        task_list_t *tmp_sleeping_list = malloc(sizeof(task_list_t));

        do
        {
            add_task_to_list(tmp_sleeping_list, sleeping_task);
            sleeping_task = pop_task_form_general_list(SLEEPING_TASK);
        } while (sleeping_task != NULL);

        task_t *task_to_check = pop_task_form_list(tmp_sleeping_list);

        do
        {
            if (task_to_check->sleep_expiry <= millisecond_since_boot)
            {
                unblock_task(task_to_check);
            }
            else
            {
                add_task_to_general_list(SLEEPING_TASK, task_to_check);
            }

            task_to_check = pop_task_form_list(tmp_sleeping_list);

        } while (task_to_check != NULL);

        free(tmp_sleeping_list);
    }

    if (current_time_slice_remaining != ONLY_TASK_RUNNING)
    {
        current_time_slice_remaining--;
        if (current_time_slice_remaining == ONLY_TASK_RUNNING)
        {
            schedule();
        }
    }

    unlock_kernel_stuff();
}