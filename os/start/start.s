# Declare constants for the multiboot header.
.set MBALIGN,    1 << 0            # align loaded modules on page boundaries
.set MEMINFO,    1 << 1            # provide memory map
.set SELGRAPHIC, 1 << 2            # select graphics mod
.set FLAGS,      MBALIGN | MEMINFO | SELGRAPHIC # this is the Multiboot 'flag' field
.set MAGIC,      0x1BADB002        # 'magic number' lets bootloader find the header
.set CHECKSUM,  -(MAGIC + FLAGS)   # checksum of above, to prove we are multiboot
 
# Declare a multiboot header that marks the program as a kernel.
.section .multiboot
.align 4
	.long MAGIC
	.long FLAGS
	.long CHECKSUM
	.skip 20
	.long 1 # mode_type
	.long 80 # width
	.long 50 # height
	.long 0 # depth


.section .text
.global _higher_half
.type _higher_half, @function
_higher_half:
    call main

	cli
.hang:	hlt
	jmp .hang

.size _higher_half, . - _higher_half
