.section .text

.global _start
_start:
.extern set_current_heap

	movl $12, %eax
	int $0x80

	pushl %eax
	call set_current_heap
	popl %eax
	
	call main

	movl $0, %eax
	int $0x80
.size _start, . - _start
