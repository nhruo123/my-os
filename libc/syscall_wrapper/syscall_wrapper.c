#include <syscall_wrapper/syscall_wrapper.h>

DEFN_SYSCALL1(exit, 0, int)

DEFN_SYSCALL1(put_char, 1, char)
DEFN_SYSCALL0(get_char, 2)

DEFN_SYSCALL1(map_page, 3, void *)

DEFN_SYSCALL3(start_task, 4, char *, int, char **)
DEFN_SYSCALL3(start_task_and_block, 5, char *, int, char **)