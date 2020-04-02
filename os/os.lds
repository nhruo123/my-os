ENTRY(_bootstrap_start)


VIRT_BASE = 0xC0000000;

SECTIONS
{
	. = 1M;

	_kernel_bootstrap_start = .;
	.boot : ALIGN(4k) {
		*(.multiboot)
		*(.bootstrap)
		*(.bootstrap.bss)
		*(.bootstrap.*)
		. = ALIGN(4k);
	}

	_kernel_bootstrap_end = .;



	. = . + VIRT_BASE;
	_kernel_start = . ;
	_kernel_physical_start = _kernel_start - VIRT_BASE;

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
	.bss : AT(ADDR(.bss) - VIRT_BASE) ALIGN(4K) {
		*(.bss.*)
		*(.bss)
		*(COMMON)
	}

	. = ALIGN(4k);
	_kernel_end = .;
	_kernel_physical_end = _kernel_end - VIRT_BASE;
}