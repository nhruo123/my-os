.macro ISR_NOERRCODE n
.global isr\n
    isr\n:
    cli
    pushl $0 # push dummy error code
    pushl $\n
    jmp isr_common_stub
.endm

.macro ISR_ERRCODE n
.global isr\n
    isr\n:
    cli
    pushl $\n
    jmp isr_common_stub
.endm

.macro IRQ irq_num, isr_num
.global irq\irq_num
    irq\irq_num:
    cli
    pushl $0
    pushl $\isr_num
    jmp irq_common_stub

.endm

# this will call a c function after it saved the proccessor state and set up the kernel segment
isr_common_stub:
   pusha    # save all regs eax, ebx....

   movw %ds, %ax # save ds in the lower 16 bits of eax
   pushl %eax # push it on the stack

   movw $0x10, %ax  # load the kernel data segment descriptor
   movw %ax, %ds
   movw %ax, %es
   movw %ax, %fs
   movw %ax, %gs

   call isr_handler

   popl %eax # set to the original data segment descriptor
   movw %ax, %ds
   movw %ax, %es
   movw %ax, %fs
   movw %ax, %gs

   popa    # Pops edi,esi,ebp...
   addl $8, %esp  # Cleans up the pushed error code and pushed ISR number

   sti
   iret # pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP



# same as isr_common_but but calls irq_handler
irq_common_stub:
   pusha    # save all regs eax, ebx....

   movw %ds, %ax # save ds in the lower 16 bits of eax
   pushl %eax # push it on the stack

   movw $0x10, %ax  # load the kernel data segment descriptor
   movw %ax, %ds
   movw %ax, %es
   movw %ax, %fs
   movw %ax, %gs

   call irq_handler

   popl %eax # set to the original data segment descriptor
   movw %ax, %ds
   movw %ax, %es
   movw %ax, %fs
   movw %ax, %gs

   popa    # Pops edi,esi,ebp...
   addl $8, %esp  # Cleans up the pushed error code and pushed ISR number

   sti
   iret # pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP




ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE 8
ISR_NOERRCODE 9

ISR_ERRCODE 10
ISR_ERRCODE 11
ISR_ERRCODE 12
ISR_ERRCODE 13
ISR_ERRCODE 14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_NOERRCODE 17
ISR_NOERRCODE 18
ISR_NOERRCODE 19

ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29

ISR_NOERRCODE 30
ISR_NOERRCODE 31


ISR_NOERRCODE 128

IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39

IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47
