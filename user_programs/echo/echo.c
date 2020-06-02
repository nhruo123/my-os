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

	char buffer[BUFF_SIZE];
	int offset = 0;

	int read = sys_read(argv[1], buffer, offset, BUFF_SIZE);
	if (read < 0)
	{
		printf("bad file name\n");
		return;
	}
	while (read != 0)
	{
		offset += read;
		for (int i = 0; i < read; i++)
		{
			putchar(buffer[i]);
		}
		read = sys_read(argv[1], buffer, offset, BUFF_SIZE);
	}
}