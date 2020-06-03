#include <stdlib.h>
#include <stdio.h>
#include <syscall_wrapper/syscall_wrapper.h>
#include <string.h>

#define BFF_SIZE 400
#define ARG_SIZE 100
#define ARG_COUNT 5
void main()
{
	printf("Wellcome to my shell form printf\n");

	while (true)
	{
		putchar('>');
		run_command();
	}
}

void run_command()
{
	int argc = 0;

	char buffer[BFF_SIZE];
	char file_name[BFF_SIZE];
	char *args[ARG_COUNT];

	memset(buffer, 0, sizeof(char) * BFF_SIZE);
	memset(file_name, 0, sizeof(char) * BFF_SIZE);

	for (int i = 0; i < ARG_COUNT; i++)
	{
		args[i] = malloc(ARG_SIZE);
		memset(args[i], 0, ARG_SIZE);
	}

	for (int i = 0; (i < BFF_SIZE - 1); i++)
	{
		char c = getchar();
		putchar(c);

		if (c == '\n')
		{
			char *cur_arg = strchr(buffer, ' ');

			int file_name_len;
			if (cur_arg != NULL)
			{
				file_name_len = (cur_arg - buffer);
			}
			else
			{
				file_name_len = strlen(buffer);
			}
			memcpy(file_name, buffer, file_name_len);

			while (cur_arg != NULL)
			{
				int arg_len;

				cur_arg++;
				char *next_space = strchr(cur_arg, ' ');

				if (next_space != NULL)
				{
					arg_len = (next_space - cur_arg);
				}
				else
				{
					arg_len = strlen(cur_arg);
				}
				memcpy(args[argc], cur_arg, arg_len);
				argc++;
				cur_arg = next_space;
			}

			int pid = sys_fork();
			if (pid == 0)
			{
				sys_exec(file_name, argc, args);
				printf("file not found for file for file name \"%s\"\n", file_name);
				sys_exit(0);
			}
			else
			{
				sys_waitpid(pid);
			}

			for (int i = 0; i < ARG_COUNT; i++)
			{
				free(args[i]);
			}
			return;
		}
		buffer[i] = c;
	}

	for (int i = 0; i < ARG_COUNT; i++)
	{
		free(args[i]);
	}
}
