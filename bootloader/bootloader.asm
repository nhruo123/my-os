;******************************************
; Bootloader.asm200 operating system: from 0 to 1
; A Simple Bootloader
;******************************************

[BITS 16]

start: jmp boot

disk_id:
 db 0

gdt:

gdt_null:
 dq 0

gdt_code:
 dw 0xFFFF ; limit
 dw 0x0 ; base
 db 0 ; base
 db 10011010b
 db 11001111b
 db 0

gdt_data:
 dw 0xFFFF
 dw 0x0
 db 0
 db 10010010b
 db 11001111b
 db 0 
gdt_end:


gdt_desc:
   dw gdt_end - gdt - 1
   dd gdt

boot:
 cli ; no interrupts

 mov [disk_id], dl

load_os:
 mov ax, 0x50
 
 ;; set the buffer
 mov es, ax
 xor bx, bx
 
 mov al, 10 ; read 10 sector
 mov ch, 0 ; track 0
 mov cl, 2 ; sector to read (The second sector)
 mov dh, 0 ; head number
 mov dl, [disk_id] ; disk id
 mov ah, 0x02 ; read sectors from disk
 int 0x13 ; call the BIOS routine

lineA20:
 in al, 0x92 ; turn on a20 line
 or al, 2
 out 0x92, al


load_gdt:
 cli ; no interrupts
 xor ax, ax
 mov ds, ax
 lgdt [gdt_desc]
 
 mov eax, cr0
 or al, 0x01 ; enter protected mode
 mov cr0, eax 
 

 jmp 0x08:clear_pipe

 [BITS 32]

clear_pipe:

 mov ax, 0x10
 mov ds, ax
 mov ss, ax
 mov esp, 0x90000

 jmp [0x500 + 0x18] ; jump and execute the sector!


times 446 - ($-$$) db 0

db 0x80 ;; bootable
db 0x0 ;; starting head
db 0x1 ;; sector + cylinder
db 0x0 ;; cylinder
db 0x7f ;; system id
db 0x0 ;; ending head
db 0x0 ;; ending sector + ending cylinder
db 0x0 ;; ending cylinder
dd 0x0 ;; relative sector
dd 0x1 ;; total sectors in partition


times 16 db 0 ;; 2nd entry
times 16 db 0 ;; 3rd entry
times 16 db 0 ;; 4th entry

dw 0xAA55 ; Boot Signiture
