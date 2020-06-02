#ifndef _SYSCALL_WRAPPER_H
#define _SYSCALL_WRAPPER_H

#include <fs/fs_defs.h>


// taken from jamesmolly guild this seems like a good macro

#define DECL_SYSCALL0(fn) int sys_##fn(void);
#define DECL_SYSCALL1(fn, p1) int sys_##fn(p1);
#define DECL_SYSCALL2(fn, p1,p2) int sys_##fn(p1,p2);
#define DECL_SYSCALL3(fn, p1,p2,p3) int sys_##fn(p1,p2,p3);
#define DECL_SYSCALL4(fn, p1,p2,p3,p4) int sys_##fn(p1,p2,p3,p4);
#define DECL_SYSCALL5(fn, p1,p2,p3,p4,p5) int sys_##fn(p1,p2,p3,p4,p5);

#define DEFN_SYSCALL0(fn, num) \
int sys_##fn(void) \
{ \
  int a; \
  asm volatile("int $0x80" : "=a" (a) : "0" (num)); \
  return a; \
}

#define DEFN_SYSCALL1(fn,  num, P1) \
int sys_##fn(P1 p1) \
{ \
  int a; \
  asm volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((int)p1)); \
  return a; \
}

#define DEFN_SYSCALL2(fn, num, P1, P2) \
int sys_##fn(P1 p1, P2 p2) \
{ \
  int a; \
  asm volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((int)p1), "c" ((int)p2)); \
  return a; \
}

#define DEFN_SYSCALL3(fn, num, P1, P2, P3) \
int sys_##fn(P1 p1, P2 p2, P3 p3) \
{ \
  int a; \
  asm volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((int)p1), "c" ((int)p2), "d"((int)p3)); \
  return a; \
}

#define DEFN_SYSCALL4(fn, num, P1, P2, P3, P4) \
int sys_##fn(P1 p1, P2 p2, P3 p3, P4 p4) \
{ \
  int a; \
  asm volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((int)p1), "c" ((int)p2), "d" ((int)p3), "S" ((int)p4)); \
  return a; \
}

#define DEFN_SYSCALL5(fn, num) \
int sys_##fn(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) \
{ \
  int a; \
  asm volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((int)p1), "c" ((int)p2), "d" ((int)p3), "S" ((int)p4), "D" ((int)p5)); \
  return a; \
}


DECL_SYSCALL1(exit, int)

DECL_SYSCALL1(put_char, char)
DECL_SYSCALL0(get_char)

DECL_SYSCALL1(map_page, void *)


DECL_SYSCALL0(fork)
DECL_SYSCALL1(waitpid, int)

DECL_SYSCALL3(exec, char *, int, char **)

DECL_SYSCALL2(mk_file, char *, int)
DECL_SYSCALL3(readdir, char *, dir_entry_t *, int)
DECL_SYSCALL4(read, char *,  char *, int, int)
DECL_SYSCALL4(write, char *,  char *, int, int)


#endif