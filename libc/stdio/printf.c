#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static bool print(const char* data, size_t length) {
	const unsigned char* bytes = (const unsigned char*) data;
	for (size_t i = 0; i < length; i++)
		if (putchar(bytes[i]) == EOF)
			return false;
	return true;
}

int printf(const char* restrict format, ...) {
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
            if(!print(format, current_size)) { 
                return -1; // put char retrun error
            }  
            format += current_size;
            written += current_size;
            continue;
        }

        if(format[0] == '%') {
            format++;
            const char * formater_start = format[0]; // set the formater as the format char and leave the format as the rest 

            if(formater_start[0] == '%') {
                if(putchar('%') == EOF) {
                    return -1;
                }
                if (left_print_size == 0) {
                    return -1;
                }

                written++;
                format++;
            } else if (formater_start[0] == 'c') {
                char c = (char) va_arg(parameters, int);
                
                
                if(putchar(c) == EOF) {
                    return -1;
                }
                if (left_print_size == 0) {
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

                if(!print(str, len)) {
                    return -1;
                }

                format++;
                written += len;
            } else { // unsuported or bad formating
                size_t len = strlen(format);

                if(left_print_size < len) {
                    return -1;
                }

                if(!print(format, len)) {
                    return -1;
                }

                format += len;
                written += len;
            }  
        }
    }
    
    va_end(parameters);
    return written;
}
