#include <string.h>

void* memset(void* memPoint, int replace, size_t size) {
    
    unsigned char * mem = (unsigned char *) memPoint;
    

    for (size_t index = 0; index < size; index ++) {
        mem[index] = (unsigned char) replace;
    }

    return memPoint;
}