#include <string.h>

size_t strcmp(char *str1, char *str2) {
    size_t index = 0;
    size_t flag = 0;

    while (flag == 0)
    {
        if (str1[index] > str2[index])
        {
            flag = 1;
        }
        else if (str1[index] < str2[index])
        {
            flag = -1;
        }

        if (str1[index] == '\0')
        {
            break;
        }

        index++;
    }

    return flag;
}
