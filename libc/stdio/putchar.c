#include <stdio.h>

#if defined(__is_libk)
#include <screen/screen.h>
#else
#include <syscall_wrapper/syscall_wrapper.h>
#endif

 

int putchar(int chr) {
#if defined(__is_libk)
	char c = (char) chr;
	print_char(c);
#else
	sys_put_char(chr);
#endif
	return chr;
}
