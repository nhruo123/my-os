#include <stdint.h>
#include <stdio.h>
#include <interrupts/isr.h>
#include <interrupts/timer.h>
#include <io/io.h>
#include <multitasking/task.h>

uint32_t millisecond_since_boot;

void init_timer() {
    millisecond_since_boot = 0;
    register_interrupt_handler(IRQ0, timer_interrupt_handler);

    uint32_t divisor = 1193180 / 1000;

    outb(0x43, 0x36);

   // Divisor has to be sent byte-wise, so split here into upper/lower bytes.
   uint8_t l = (uint8_t)(divisor & 0xFF);
   uint8_t h = (uint8_t)((divisor>>8) & 0xFF );

   // Send the frequency divisor.
   outb(0x40, l);
   outb(0x40, h);
}

void timer_interrupt_handler(registers_t regs) {
    lock_kernel_stuff();
    
    millisecond_since_boot++;

    task_t *next_task;
    task_t *this_task;

    next_task = sleeping_task_list;
    sleeping_task_list = NULL;

    while (next_task != NULL)
    {
        this_task = next_task;
        next_task = next_task->next_task;

        if(this_task->sleep_expiry <= millisecond_since_boot) {
            unblock_task(this_task);
        } else
        {
            this_task->next_task = sleeping_task_list;
            sleeping_task_list = this_task;
        }
    }

    if(current_time_slice_remaining != ONLY_TASK_RUNNING) {
        current_time_slice_remaining--;
        if(current_time_slice_remaining == ONLY_TASK_RUNNING) {
            schedule();
        }
    }
    
    unlock_kernel_stuff();
}