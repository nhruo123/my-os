#include <string.h>

char *strchr(const char *str, int c) {
    size_t index = 0;

    while(str[index]) {
        if(str[index] == c) {
            return str + index;
        }
        index++;
    }

    return NULL;
}