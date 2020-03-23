.section .bootstrap
.global _bootstrap_start
.type _bootstrap_start, @function
_bootstrap_start:

    movl $stack_top - 0xC0000000 , %esp # set up stack 
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