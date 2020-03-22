# include <string.h>

int memcmp(const void* s1, const void* s2, size_t size) {
    const char * buf1 = (const char *)s1;
    const char * buf2 = (const char *)s2;

    for(size_t i = 0 ; i < size; i++) {
        if(buf1[i] > buf2[i]) {
            return -1;
        }
        else if (buf1[i] < buf2[i])
        {
            return 1;
        }
        
    }

    return 0;
}