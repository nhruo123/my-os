#include <stdlib.h>
#include <stdio.h>
#include <syscall_wrapper/syscall_wrapper.h>
#include <string.h>

void main()
{
	printf("Wellcome to my shell form printf\n");
	while (true)
	{
		run_command();
	}
}

void run_command()
{
	char buffer[201];
	memset(buffer, 0, sizeof(char) * 201);
	for (int i = 0; (i < 200); i++)
	{
		char c = getchar();
		putchar(c);
		if (c == '\n')
		{
			int pid = sys_fork();
			if (pid == 0)
			{
				sys_exec(buffer, 0, NULL);
				printf("file not found for file for file name \"%s\"\n", buffer);
				sys_exit(0);
			}
			else
			{
				sys_waitpid(pid);
			}

			return;
		}
		buffer[i] = c;
	}
}
