#include "idt.h"
#include <stdbool.h>
#include <string.h>
#include <io/io.h>
#include <interrupts/isr.h>
#include <interrupts/interrupts.h>


#define PIC1_COMMAND 0x20
#define PIC1_DATA (PIC1_COMMAND + 1)
#define PIC2_COMMAND 0xA0
#define PIC2_DATA (PIC2_COMMAND + 1)

#define ICW1 0x10
#define ICW4 0x01

static void init_pics(uint8_t pic1_offset, uint8_t pic2_offset);
static void idt_set_gate(uint8_t index, uint32_t base, uint16_t selector, uint8_t ring, bool is_present);

static void idt_flush(idt_ptr_t idt_ptr) {
	// load idt
	asm("lidt %0" :: "m"(idt_ptr));

	// set interrupt flag
	asm("sti");
}

idt_entry_t idt_entries[256];
isr_t interrupt_handlers[];

void init_idt() {
    
    asm("cli");

    init_pics(0x20, 0x28); // remap pics

    idt_ptr_t idt_ptr = {0};

    idt_ptr.size = sizeof(idt_entry_t) * 256 - 1;
    idt_ptr.base = (uint32_t)idt_entries;

    memset(idt_entries, 0, sizeof(idt_entry_t)*256);
    memset(interrupt_handlers, 0, sizeof(isr_t)*256);
    
    // sets up cpu interrupt
    idt_set_gate(0, (uint32_t)isr0, 0x08, 0x3, true);
    idt_set_gate(1, (uint32_t)isr1, 0x08, 0x3, true);
    idt_set_gate(2, (uint32_t)isr2, 0x08, 0x3, true);
    idt_set_gate(3, (uint32_t)isr3, 0x08, 0x3, true);
    idt_set_gate(4, (uint32_t)isr4, 0x08, 0x3, true);
    idt_set_gate(5, (uint32_t)isr5, 0x08, 0x3, true);
    idt_set_gate(6, (uint32_t)isr6, 0x08, 0x3, true);
    idt_set_gate(7, (uint32_t)isr7, 0x08, 0x3, true);
    idt_set_gate(8, (uint32_t)isr8, 0x08, 0x3, true);
    idt_set_gate(9, (uint32_t)isr9, 0x08, 0x3, true);

    idt_set_gate(10, (uint32_t)isr10, 0x08, 0x3, true);
    idt_set_gate(11, (uint32_t)isr11, 0x08, 0x3, true);
    idt_set_gate(12, (uint32_t)isr12, 0x08, 0x3, true);
    idt_set_gate(13, (uint32_t)isr13, 0x08, 0x3, true);
    idt_set_gate(14, (uint32_t)isr14, 0x08, 0x3, true);
    idt_set_gate(15, (uint32_t)isr15, 0x08, 0x3, true);
    idt_set_gate(16, (uint32_t)isr16, 0x08, 0x3, true);
    idt_set_gate(17, (uint32_t)isr17, 0x08, 0x3, true);
    idt_set_gate(18, (uint32_t)isr18, 0x08, 0x3, true);
    idt_set_gate(19, (uint32_t)isr19, 0x08, 0x3, true);

    idt_set_gate(20, (uint32_t)isr20, 0x08, 0x3, true);
    idt_set_gate(21, (uint32_t)isr21, 0x08, 0x3, true);
    idt_set_gate(22, (uint32_t)isr22, 0x08, 0x3, true);
    idt_set_gate(23, (uint32_t)isr23, 0x08, 0x3, true);
    idt_set_gate(24, (uint32_t)isr24, 0x08, 0x3, true);
    idt_set_gate(25, (uint32_t)isr25, 0x08, 0x3, true);
    idt_set_gate(26, (uint32_t)isr26, 0x08, 0x3, true);
    idt_set_gate(27, (uint32_t)isr27, 0x08, 0x3, true);
    idt_set_gate(28, (uint32_t)isr28, 0x08, 0x3, true);
    idt_set_gate(29, (uint32_t)isr29, 0x08, 0x3, true);

    idt_set_gate(30, (uint32_t)isr30, 0x08, 0x3, true);
    idt_set_gate(31, (uint32_t)isr31, 0x08, 0x3, true);

    // set up PIC interrupts

    idt_set_gate(32, (uint32_t)irq0, 0x08, 0x3, true);
    idt_set_gate(33, (uint32_t)irq1, 0x08, 0x3, true);
    idt_set_gate(34, (uint32_t)irq2, 0x08, 0x3, true);
    idt_set_gate(35, (uint32_t)irq3, 0x08, 0x3, true);
    idt_set_gate(36, (uint32_t)irq4, 0x08, 0x3, true);
    idt_set_gate(37, (uint32_t)irq5, 0x08, 0x3, true);
    idt_set_gate(38, (uint32_t)irq6, 0x08, 0x3, true);
    idt_set_gate(39, (uint32_t)irq7, 0x08, 0x3, true);
    idt_set_gate(40, (uint32_t)irq8, 0x08, 0x3, true);
    
    idt_set_gate(41, (uint32_t)irq9, 0x08, 0x3, true);
    idt_set_gate(42, (uint32_t)irq10, 0x08, 0x3, true);
    idt_set_gate(43, (uint32_t)irq11, 0x08, 0x3, true);
    idt_set_gate(44, (uint32_t)irq12, 0x08, 0x3, true);
    idt_set_gate(45, (uint32_t)irq13, 0x08, 0x3, true);
    idt_set_gate(46, (uint32_t)irq14, 0x08, 0x3, true);
    idt_set_gate(47, (uint32_t)irq15, 0x08, 0x3, true);


    idt_flush(idt_ptr);
}

static void init_pics(uint8_t pic1_offset, uint8_t pic2_offset) {
	/* send ICW1 */
	outb(PIC1_COMMAND, ICW1 | ICW4);
	outb(PIC1_COMMAND, ICW1 | ICW4);

	/* send ICW2 */
	outb(PIC1_DATA, pic1_offset);	/* remap */
	outb(PIC2_DATA, pic2_offset);	/*  pics */

	/* send ICW3 */
	outb(PIC1_DATA, 4);	/* IRQ2 -> connection to slave */
	outb(PIC2_DATA, 2);

	/* send ICW4 */
	outb(PIC1_DATA, ICW4);
	outb(PIC2_DATA, ICW4);

	/* disable all IRQs but keybord */
	outb(PIC1_DATA, 0xFC);
	outb(PIC2_DATA, 0xFF);
}

static void idt_set_gate(uint8_t index, uint32_t base, uint16_t selector, uint8_t ring, bool is_present) {
    idt_entries[index].base_lowerbits = base & 0xFFFF;
    idt_entries[index].base_higherbits = (base >> 16) & 0XFFFF;

    idt_entries[index].selector = selector;
    idt_entries[index].zero = 0;

    idt_entries[index].type_attr = 0b0110;


    idt_entries[index].type_attr = 0b1110; // set to 32bit interrupt gate
    
    idt_entries[index].type_attr = ((ring & 0b11) << 5) | idt_entries[index].type_attr; //set ring

    if(is_present) {
        idt_entries[index].type_attr = (0b1 << 7) | idt_entries[index].type_attr;
    }
}