#include <stdlib.h>
#include <stdio.h>
#include <syscall_wrapper/syscall_wrapper.h>
#include <string.h>

#define BUFF_SIZE 100

void main(int argc, char **argv)
{

	if (argc < 2)
	{
		return;
	}

	printf("%s\n",argv[1]);
}