
# void switch_task(task_t *new_task);

.text
.global switch_task
.type switch_task, @function
switch_task:

    .extern current_active_task
    .extern kernel_tss
    # save old task's state

    # note, we need to save ebx, esi, edi, esp, ebp, eflags,
    # all the other registers are saved by gcc calling conventions

    pushl %ebx
    pushl %esi
    pushl %edi
    pushl %ebp

    movl (current_active_task), %edi # edi is current task
    movl %esp, 0(%edi)  # saves current esp into the struct at esp offset is (0)

    # load next task's state
    movl $5, %eax
    movl 0(%esp, %eax, 4), %esi # make esi point to new_task we need to go 5 down the stack cuz we push 4 + 1 return adder
    movl %esi, (current_active_task) # set current task to the next one

    movl 0(%esi), %esp # make esp point to new task esp (esp offset is (0))
    
    movl 4(%esi), %ebx # load esp0 into ebx offset is (4)
    movl %ebx, (kernel_tss + 4) # move ebx to kernel_tss->esp0
    

    movl 8(%esi), %eax # make eax the next address space (address space offset is (8))
    movl %eax, %cr3 # load new address space

    popl %ebx
    popl %esi
    popl %edi
    popl %ebp

    ret

