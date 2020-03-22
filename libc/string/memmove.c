#include <string.h>

void* memmove(void* s1, const void* s2, size_t size) {
    unsigned char * temp_buf[size];

    memcpy(temp_buf, s2, size);

    memcpy(s1, temp_buf, size);

    return s1;
}
