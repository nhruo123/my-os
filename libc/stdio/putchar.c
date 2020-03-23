#include <stdio.h>

#if defined(__is_libk)
#include <screen/screen.h>
#endif

int putchar(int chr) {
#if defined(__is_libk)
	char c = (char) chr;
	print_char(c);
#else
	// TODO: Implement stdio and the write system call.
#endif
	return chr;
}
