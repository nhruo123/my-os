.section .text
.global flush_gdt
.type flush_gdt, @function
flush_gdt:
    movl 4(%esp), %eax
    lgdt (%eax)
	ljmp $0x08, $._flush_cs



._flush_cs:

	movw $0x10 , %cx
	movw %cx , %ds
	movw %cx , %ss
	movw %cx , %es
	movw %cx , %gs
    ret


.section .text
.global flush_tss
.type flush_tss, @function
flush_tss:
	movl 4(%esp), %eax

	ltr %ax
ret
