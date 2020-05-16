#include <stdio.h>
#include <string.h>


#if defined(__is_libk)
#include <keyboard/keyboard.h>
#else
#include <syscall_wrapper/syscall_wrapper.h>
#endif

int getchar() {


#if defined(__is_libk)
    return get_char();
#else
    return sys_get_char();
#endif

}