#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

static char base_10_translation(char digit) {
    return digit + 48;
}

static char base_2_translation(char digit) {
    if(digit == 1) {
        return '1';
    }

    return '0';
}


char* itoa (int value, char * str, int base) {
    char * ret_val = str;
    char * start_str = str;
    bool is_negative = false;
    if(value < 0) {
        *str = '-';
        str++;
        start_str = str;
        value = abs(value);
    }
    else if(value == 0) {
        *str = '0';
        str++;
    }

    while(value != 0) {
        char currnt_digit = (char)(value % base);
        value = value / base;
        char translated_digit;
        if(base == 10) {
            translated_digit = base_10_translation(currnt_digit);
        } else if(base == 2) {
            translated_digit = base_2_translation(currnt_digit);
        } else {
            // TODO IMPLEMENT MORE TRANSLATIONS
            translated_digit = currnt_digit;
        }


        *str = translated_digit;
        str++;
    }
    *str = 0;
    // we reverse the int for the print
    size_t str_size = (size_t)(str - start_str);
    for (size_t i = 0; i < str_size / 2; i++)
    {
        char c1 = start_str[i];
        char c2 = start_str[str_size - i - 1];

        start_str[str_size - i - 1] = c1;
        start_str[i] = c2;
            
    }
    
    
    return ret_val;
}