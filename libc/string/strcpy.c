#include <string.h>

char *strcpy(char *dest, const char *src) {
    char *old_dest = dest;

    while (*src != '\0')
    {
        *dest = *src; 
        dest++;
        src++;
    }

    *dest = *src;
    
    return old_dest;
}