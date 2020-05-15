#ifndef _STRING_H
#define _STRING_H 1

#include <sys/cdefs.h>

#include <stddef.h>

int memcmp(const void *, const void *, size_t);
void *memcpy(void *__restrict, const void *__restrict, size_t);
void *memmove(void *, const void *, size_t);
void *memset(void *, int, size_t);
size_t strlen(const char *);
size_t strcmp(char *str1, char *str2);
char *strcat(char *dest, const char *src);
char *strcpy(char *dest, const char *src);
char *strchr(const char *str, int c);
char *strrchr(const char *str, int c);
char *strpbrk(char *str1, const char *str2);

#endif