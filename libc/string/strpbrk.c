#include <string.h>

char *strpbrk(char *str1, const char *str2)
{

    size_t str_to_find_len = strlen(str1);
    size_t scanned_str_len = strlen(str2);

    if(str_to_find_len > scanned_str_len) {
        return NULL;
    }

    for (size_t i = 0; i <= (scanned_str_len - str_to_find_len); i++)
    {
        if (memcmp(str1, str2 + i, str_to_find_len) != 0)
        {
            return str2 + i;
        }
    }

    return NULL;
}