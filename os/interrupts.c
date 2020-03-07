#include <stdint.h>
#include "vga.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA (PIC1_COMMAND + 1)
#define PIC2_COMMAND 0xA0
#define PIC2_DATA (PIC1_COMMAND + 1)

#define ICW1 0x10
#define ICW4 0x01

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

/* init_pics()
 * init the PICs and remap them
 */
void init_pics(int pic1_offset, int pic2_offset) {
	/* send ICW1 */
	outb(PIC1_COMMAND, ICW1 | ICW4);
	outb(PIC1_COMMAND, ICW1 | ICW4);

	/* send ICW2 */
	outb(PIC1_DATA, pic1_offset);	/* remap */
	outb(PIC2_DATA, pic2_offset);	/*  pics */

	/* send ICW3 */
	outb(PIC1_DATA, 4);	/* IRQ2 -> connection to slave */
	outb(PIC1_DATA, 2);

	/* send ICW4 */
	outb(PIC1_DATA, ICW4);
	outb(PIC1_DATA, ICW4);

	/* disable all IRQs but keybord */
	outb(PIC2_DATA, 0xFD);
	outb(PIC1_DATA, 0xFF);

}

void master_eoi(void) {
	outb(PIC1_COMMAND, 0x20);
}

void slave_eoi() {
	outb(PIC2_COMMAND, 0x20);
	master_eoi();
}

struct IDT_entry {
	unsigned short int offset_lowerbits;
	unsigned short int selector;
	unsigned char zero;
	unsigned char type_attr;
	unsigned short int offset_higherbits;
};

struct IDT_pointer {
	unsigned short int idt_size;
	unsigned int idt_offset;
};

struct IDT_entry IDT[256];

void div_by_zero_handler() {
	__asm__("pushal");
    
    print("INT 0 was called! div by zero !!! \n");
	master_eoi();

    __asm__("popal; leave; iret"); /* BLACK MAGIC! */
	
}

void irq0_handler() {
	__asm__("pushal");
    
    print("INT 0 \n");
	master_eoi();

    __asm__("popal; leave; iret"); /* BLACK MAGIC! */
	
}

void irq1_handler() {
	__asm__("pushal");
    
    print("INT 1 \n");
	master_eoi();

    __asm__("popal; leave; iret"); /* BLACK MAGIC! */
}

void load_itd(struct IDT_pointer idt_ptr) {
	// load idt
	__asm__("lidt %0" :: "m"(idt_ptr));

	// set interrupt flag
	asm("sti");
}


void init_idt(void) {
	init_pics(32, 40); // remap pics

	unsigned long idt_address;
	struct IDT_pointer idt_ptr;

	unsigned long irq_div_zero_address = (unsigned long)div_by_zero_handler; 
	IDT[32].offset_lowerbits = irq_div_zero_address & 0xffff;
	IDT[32].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[32].zero = 0;
	IDT[32].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[32].offset_higherbits = (irq_div_zero_address & 0xffff0000) >> 16;

	unsigned long irq0_address = (unsigned long)irq0_handler; 
	IDT[32].offset_lowerbits = irq0_address & 0xffff;
	IDT[32].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[32].zero = 0;
	IDT[32].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[32].offset_higherbits = (irq0_address & 0xffff0000) >> 16;

	unsigned long irq1_address = (unsigned long)irq1_handler; 
	IDT[33].offset_lowerbits = irq1_address & 0xffff;
	IDT[33].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[33].zero = 0;
	IDT[33].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[33].offset_higherbits = (irq1_address & 0xffff0000) >> 16;


	idt_ptr.idt_size = (sizeof (struct IDT_entry) * 256);
	idt_ptr.idt_offset = (unsigned int)IDT;

	load_itd(idt_ptr);
}
