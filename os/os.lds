ENTRY(_start)


VIRT_BASE = 0xC0000000;

SECTIONS
{
	. = 1M;
	.boot : ALIGN(4k) {
		*(.multiboot)

		. = ALIGN(4k);
	}
	. = . + VIRT_BASE;
	_kernel_start = . ;
	.text : AT(ADDR(.text) - VIRT_BASE) ALIGN(4k) {
		*(.text.*)
		*(.text)
	}

	/* Read-only data. */
	.rodata : AT(ADDR(.rodata) - VIRT_BASE) ALIGN(4k) {
		*(.rodata.*)
		*(.rodata)
	}

	/* Read-write data (initialized) */
	.data : AT(ADDR(.data) - VIRT_BASE) ALIGN(4K) {
		*(.data.*)
		*(.data)
	}

	/* Read-write data (uninitialized) and stack */
	.bss : AT(ADDR(.bss) - VIRT_BASE) {
		*(.bss.*)
		*(.bss)
		*(COMMON)
	}

	_kernel_end = .;
}