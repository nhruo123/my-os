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

section .text
global _start:function (_start.end - _start)
_start:

; 	extern _kernel_start
; 	extern _kernel_end


; 	mov edi, boot_page_table1 - 0xC0000000 ; gets the physical addrs of the first boot page
; 	mov esi, 0 ; first address to map is 0
; 	mov ecx, 1023 ; we will map only 1023 the last one will be the vga buffer

; ._map_loop:
; 	;cmp esi, _kernel_start - 0xC0000000 ; we will skip all adresses that are not in the kernel
; 	;jl ._point_to_next_page

; 	cmp esi, _kernel_end - 0xC0000000  ; we are done mapping the kernel
; 	jge ._map_vga_buffer

; 	mov edx, esi ; get the physical address of the page
; 	or edx, 0x003 ; set w/ring0/present
; 	mov [edi], edx ; write it to table 

; ._point_to_next_page:


; 	add esi, 4096 ; add page size to index (page is 4096 bytes)
; 	add edi , 4 ; add the size of the last entry to the pointer of the current page
; 	loop ._map_loop

; ._map_vga_buffer:
; 	mov dword [boot_page_table1 - 0xC0000000 + 1023 * 4], 0x000B8007 ; map VGA to 0XC03FF000

; 	mov dword [boot_page_directory - 0xC0000000 + 0 * 4], boot_page_table1 - 0xC0000000 + 0x003  ; identity map the kernel 
; 	mov dword [boot_page_directory - 0xC0000000 + 768 * 4], boot_page_table1 - 0xC0000000 + 0x003 ; higher half map

; 	; Set cr3 to the address of the boot_page_directory.
; 	mov ecx, boot_page_directory - 0xC0000000
; 	mov cr3, ecx

	

	; Enable paging and the write-protect bit.
	mov ecx, cr0
	or ecx, 0x80010000
	mov cr0, ecx

	; Jump to higher half with an absolute jump. 
	
	lea ecx, [_higher_half]
	jmp ecx

.end:


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