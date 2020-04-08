.section .bootstrap.bss

.align 16
.global bootstrap_heap_start
.global bootstrap_heap_end
bootstrap_heap_start:
.skip 65536 # 64 KiB
bootstrap_heap_end:

.align 16
.global bootstrap_stack_bottom
.global bootstrap_stack_top
bootstrap_stack_bottom:
.skip 65536 # 64 KiB
bootstrap_stack_top:



.section .bootstrap
.global _bootstrap_start
.type _bootstrap_start, @function
_bootstrap_start:

    movl $bootstrap_stack_top , %esp # set up stack 
    pushl %ebx
    pushl %ebx
    pushl %ebx
    pushl %ebx
    pushl %ebx

    call init_bit_map

    movl %cr0, %ecx
	orl $0x80010000, %ecx
	movl %ecx, %cr0

    

    leal _higher_half , %eax
    jmp *%eax
.end:

.size _bootstrap_start, . - _bootstrap_start
