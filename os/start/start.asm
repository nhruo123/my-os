; Declare constants for the multiboot header.
MBALIGN  equ  1 << 0            ; align loaded modules on page boundaries
MEMINFO  equ  1 << 1            ; provide memory map
FLAGS    equ  MBALIGN | MEMINFO ; this is the Multiboot 'flag' field
MAGIC    equ  0x1BADB002        ; 'magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + FLAGS)   ; checksum of above, to prove we are multiboot
 
; Declare a multiboot header that marks the program as a kernel.
section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM
 
; make space for stack pages
section .bss
align 16
stack_bottom:
resb 16384 ; 16 KiB
stack_top:
global stack_top

section .bss
align 4096
boot_page_directory:
resb 4096
boot_page_table1:
resb 4096

section .data
gdt:

 .gdt_null:
 dq 0

 .gdt_code:
 dw 0xFFFF ; limit
 dw 0x0 ; base
 db 0 ; base
 db 10011010b
 db 11001111b
 db 0

 .gdt_data:
 dw 0xFFFF
 dw 0x0
 db 0
 db 10010010b
 db 11001111b
 db 0 
.gdt_end:


gdt_desc:
 dw gdt.gdt_end - gdt - 1
 dd gdt


global _higher_half:function (_higher_half.end - _higher_half)
_higher_half:

	;mov dword [boot_page_directory + 0], 0

	; Reload crc3 to force a TLB flush so the changes to take effect.
	;mov ecx, cr3 
	;mov cr3, ecx

	lgdt [gdt_desc]
	
	jmp 0x08:(.flush_cs)
	
    .flush_cs:
    mov cx, 0x10
	mov ds, cx
	mov ss, cx
	mov es, cx
	mov gs, cx
	mov fs, cx


    extern main
    call main

	cli
.hang:	hlt
	jmp .hang
.end: