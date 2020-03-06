;******************************************
; sample.asm
; A Sample Program
;******************************************
mov eax, 1
add eax, 1

mov al, 0x61
mov bl, 0x4
mov cx, 0x10
call PutChar

PutChar:
 push bp
 mov bp, sp
 
 mov ah, 0x09 ;; select BIOS function
 mov al, al   ;; char to print
 mov bh, 0x0  ;; page number
 mov bl, bl   ;; color
 mov cx, cx   ;; repeat factor
 int 0x10

 pop bp
 ret
