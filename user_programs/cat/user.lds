ENTRY(_start)
SECTIONS
{
  . = 1M;
  .text : ALIGN(4k) {
    *(.text.*)
    *(.text) 
  }

  .rodata : ALIGN(4k) {
		*(.rodata.*)
		*(.rodata)
	}

  .data : ALIGN(4k) {
    *(.data.*)
    *(.data)
  }

  .bss : ALIGN(4k) {
    *(.bss.*)
    *(.bss) 
  }
}
