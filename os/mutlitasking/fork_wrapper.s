
# int fork_wrapper(int new_pid);

.text
.global fork_wrapper
.type fork_wrapper, @function
fork_wrapper:
    popl %eax
    ret

