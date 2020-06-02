#include <syscall_wrapper/syscall_wrapper.h>
#include <fs/fs_defs.h>

DEFN_SYSCALL1(exit, 0, int)

DEFN_SYSCALL1(put_char, 1, char)
DEFN_SYSCALL0(get_char, 2)

DEFN_SYSCALL1(map_page, 3,void *)


DEFN_SYSCALL0(fork, 4)
DEFN_SYSCALL1(waitpid, 5, int)

DEFN_SYSCALL3(exec, 6, char *, int, char **)

DEFN_SYSCALL2(mk_file, 7, char *, int)
DEFN_SYSCALL3(readdir, 8, char *, dir_entry_t *, int)
DEFN_SYSCALL4(read, 9, char *,  char *, int, int)
DEFN_SYSCALL4(write, 10, char *,  char *, int, int)
DEFN_SYSCALL2(stats, 11, char *, file_stats_t *)


DEFN_SYSCALL0(heap_loc, 12)