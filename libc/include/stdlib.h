#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <sys/cdefs.h>
#include <stddef.h>

__attribute__((__noreturn__))
void abort(void);

void* malloc (size_t size);
void free (void* ptr);
char* itoa ( int value, char * str, int base );

#endif