# Declare constants for the multiboot header.
.set MBALIGN,    1 << 0            # align loaded modules on page boundaries
.set MEMINFO,    1 << 1            # provide memory map
.set FLAGS,      MBALIGN | MEMINFO # this is the Multiboot 'flag' field
.set MAGIC,      0x1BADB002        # 'magic number' lets bootloader find the header
.set CHECKSUM,  -(MAGIC + FLAGS)   # checksum of above, to prove we are multiboot
 
# Declare a multiboot header that marks the program as a kernel.
.section .multiboot
.align 4
	.long MAGIC
	.long FLAGS
	.long CHECKSUM
 
# make space for stack pages
.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:
.global stack_top

.section .bss
.align 4096
boot_page_directory:
.skip  4096
boot_page_table1:
.skip 4096

.section .data
gdt:

 .gdt_null:
 .quad 0

 .gdt_code:
 .word 0xFFFF # limit
 .word 0x0 # base
 .byte 0 # base
 .byte 0b10011010
 .byte 0b11001111
 .byte 0

 .gdt_data:
 .word 0xFFFF
 .word 0x0
 .byte 0
 .byte 0b10010010
 .byte 0b11001111
 .byte 0 
.gdt_end:


gdt_desc:
 .word .gdt_end - gdt - 1
 .long gdt


.section .text
.global _higher_half
.type _higher_half, @function
_higher_half:

	lgdt gdt_desc
	
	ljmp $0x08, $._flush_cs
	
    ._flush_cs:
    movw $0x10 , %cx
	movw %cx , %ds
	movw %cx , %ss
	movw %cx , %es
	movw %cx , %gs
	movw %cx , %fs


    call main

	cli
.hang:	hlt
	jmp .hang

.size _higher_half, . - _higher_half
