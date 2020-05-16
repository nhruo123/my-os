#include <stdio.h>
#include <stdlib.h>

__attribute__((__noreturn__))
void abort(void) {
#if defined(__is_libk)
	// TODO: Add proper kernel panic.
	printf("kernel: panic: abort()\n");
#else
	#include <syscall_wrapper/syscall_wrapper.h>
	sys_exit(1);
	// TODO: Abnormally terminate the process as if by SIGABRT.
#endif
	while (1) { }
	__builtin_unreachable();
}
