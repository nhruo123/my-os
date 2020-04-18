#include <string.h>

char *strcat(char *dest, const char *src) {
    char* ptr = dest + strlen(dest);

    while (*src != '\0')
    {
        *ptr = *src;
        src++;
        ptr++;
    }

    *ptr = '\0';

    return dest;
}