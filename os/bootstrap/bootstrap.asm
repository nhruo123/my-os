section .bootstrap
global _bootstrap_start:function (_bootstrap_start.end - _bootstrap_start)
_bootstrap_start:
    extern init_bit_map
    extern stack_top
    extern _higher_half
    extern _start


    mov esp, stack_top - 0xC0000000 ; set up stack 
    push ebx
    push ebx
    push ebx
    push ebx
    call init_bit_map

    mov ecx, cr0
	or ecx, 0x80010000
	mov cr0, ecx

    lea eax, [_higher_half]
    jmp eax
.end: