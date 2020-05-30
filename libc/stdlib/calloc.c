#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void *calloc(size_t num, size_t size)
{
    void *objects = malloc(size * num);
    memset(objects, 0, size * num);
    
    return objects;
}