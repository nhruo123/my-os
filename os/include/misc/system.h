#ifndef _SYSTEM_H
#define _SYSTEM_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

void enter_user_space_program(void* entry_point, int argc, char** argv, void* stack_top);


#endif