
# void switch_task(task_t *new_task);

.text
.global switch_task
.type switch_task, @function
switch_task:

    .extern current_active_task
    
    .extern kernel_tss

    .extern ready_to_run_list
    .extern last_ready_to_run_task
    .extern update_time_used

    .extern postpone_task_switches_counter
    .extern task_switches_postponed_flag


    cmpl $0, (postpone_task_switches_counter)
    je .continue
    movl $1 , (task_switches_postponed_flag)
    
    popl %ecx
    call push_task_back_to_ready
    pushl %ecx
    ret


    # save old task's state

    # note, we need to save ebx, esi, edi, esp, ebp, eflags,
    # all the other registers are saved by gcc calling conventions
    .continue:
    pushl %ebx
    pushl %esi
    pushl %edi
    pushl %ebp


    movl (current_active_task), %edi # edi is current task
    call update_time_used # updated used time
    movl %esp, 0(%edi)  # saves current esp into the struct at esp offset is (0)
    cmpl $0 , 16(%edi) # test if the current task was running or ended status ofset is(16) 
    jne .done_updating_old_task
   

    .update_old_task:
    movl $1, 16(%edi) # sets current task to ready to run

    cmpl $0, (ready_to_run_list) # test if ready_to_run_list is emptey
    jnz .not_only_task
    jmp .only_task

    .only_task:
    movl %edi, (ready_to_run_list) # if so put it ontop of the ready_to_run_list
    movl %edi, (last_ready_to_run_task) # and set last_ready_to_run_task to it cuz the list is only itself
    jmp .done_updating_old_task


    .not_only_task:
    movl (last_ready_to_run_task), %esi # esi is last task
    movl %edi, 12(%esi) # sets last task next task to the old task last_ready_to_run_task->next_tast = old_task
    movl (last_ready_to_run_task), %edi # sets the last_ready_to_run_task_pointer to old task last_ready_to_run_task = old_task
    jmp .done_updating_old_task


    .done_updating_old_task:

    # load next task's state
    movl $5, %eax
    movl 0(%esp, %eax, 4), %esi # make esi point to new_task we need to go 5 down the stack cuz we push 4 + 1 return adder
    movl %esi, (current_active_task) # set current task to the next one
    movl $0, 16(%esi) # sets current active task to running

    movl 0(%esi), %esp # make esp point to new task esp (esp offset is (0))
    
    movl 4(%esi), %ebx # load esp0 into ebx offset is (4)
    movl %ebx, (kernel_tss + 4) # move ebx to kernel_tss->esp0
    

    movl 8(%esi), %eax # make eax the next address space (address space offset is (8))
    movl %eax, %cr3 # load new address space
    
    popl %ebp
    popl %edi
    popl %esi
    popl %ebx

    ret

