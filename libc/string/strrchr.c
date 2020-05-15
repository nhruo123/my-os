#include <string.h>

char *strrchr(const char *str, int c) {
    size_t index = strlen(str) - 1;

    while(index != 0) {
        if(str[index] == c) {
            return str + index;
        }
        index--;
    }

    return NULL;
}