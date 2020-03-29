#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <screen/screen.h>
#include <math.h>


int printf(const char* format, ...) {
    va_list parameters;
    va_start(parameters, format);

    int written = 0;

    while (*format != '\0')
    {
        size_t left_print_size = INT_MAX - written;

        if(format[0] != '%') {
            size_t current_size = 1;
			while (format[current_size] != '\0' && format[current_size] != '%') {
                current_size++;
            }
            if(current_size > left_print_size) { 
                return -1; // cant print more then max int chars
            }
            terminal_write(format, current_size);
            format += current_size;
            written += current_size;
            continue;
        }

        if(format[0] == '%') {
            format++;
            const char * formater_start = format; // set the formater as the format char and leave the format as the rest 

            if(formater_start[0] == '%') {
                if (left_print_size < 1)
                {
                    return -1;
                }
                if (putchar('%') == EOF)
                {
                    return -1;
                }
                written++;
                format++;
            } else if (formater_start[0] == 'c') {
                char c = (char)va_arg(parameters, int);

                if (left_print_size < 1)
                {
                    return -1;
                }
                if (putchar(c) == EOF)
                {
                    return -1;
                }

                format++;
                written++;
            } else if (formater_start[0] == 's') {
                const char* str = va_arg(parameters, const char*);
                size_t len = strlen(str);
                if(left_print_size < len) {
                    return -1;
                }

                terminal_write(str, len);

                format++;
                written += len;
            } else if (formater_start[0] == 'd') {
                int int_to_print = va_arg(parameters, int);
                char string_holder[50];
                itoa(int_to_print, string_holder, 10);

                size_t len = strlen(string_holder);
                
                if(left_print_size < len) {
                    return -1;
                }
                terminal_write(string_holder, len);

                format++;
                written += len;
            } else { // unsuported or bad formating
                size_t len = strlen(format);

                if(left_print_size < len) {
                    return -1;
                }

                terminal_write(format, len);

                format += len;
                written += len;
            }  
        }
    }
    
    va_end(parameters);
    return written;
}
