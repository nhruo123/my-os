#include <string.h>

void* memcpy(void* s1, const void* s2, size_t size) {
    unsigned char * buf1 = (unsigned char * )s1;
    const unsigned char * buf2 = (const unsigned char * )s2;


    for(size_t i = 0 ; i < size ; i++) {
        buf1[i] = buf2[i];
    }

    return s1;
}