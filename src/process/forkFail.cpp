/*
 * forkFail.cpp
 *
 *  Created on: 2015-8-27
 *      Author: hongjianan
 */
#include "../config.h"
#if FORKFAIL_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define CMD_FREE		"free -m"
#define MAX_MEM_SIZE	0xC800000	/* 200MB */

char globalMemory[MAX_MEM_SIZE];

int ForkFail(int argc, char *argv[])
{
	if (3 != argc)
	{
		printf("usage: [father memset size MB] [child memset size MB]\n");
		exit(-1);
	}
	int fatherMemLen = 1024 * 1024 * atoi(argv[1]);
	int childMemLen  = 1024 * 1024 * atoi(argv[2]);

	printf("father: =======\n");
	system(CMD_FREE);

	memset(globalMemory, 'a', fatherMemLen);
	if ('a' == globalMemory[fatherMemLen - 1])
	{
		printf("father: memset sucess.\n");
	}
	else
	{
		printf("father: memset fail.\n");
	}

	printf("father: =======\n");
	system(CMD_FREE);

	sleep(1);
	pid_t pidChild = fork();
	if(0 == pidChild)
	{
		printf("child: =======\n");
		system(CMD_FREE);

		memset(globalMemory, 'b', childMemLen);
		if ('b' == globalMemory[childMemLen - 1])
		{
			printf("child: memset sucess.\n");
		}
		else
		{
			printf("child: memset fail.\n");
		}

		printf("child: =======\n");
		system(CMD_FREE);

		printf("++++++child exit++++++++\n");

		exit(0);
	}
	else if(-1 == pidChild)
	{
		printf("fork fail, errno[%d][%s]\n", errno, strerror(errno));
	}

	printf("globalMemory[0] = %c\n", globalMemory[0]);
	sleep(2);
	printf("globalMemory[0] = %c\n", globalMemory[0]);

	printf("++++++father exit++++++++\n");

	return 0;
}

#endif /* FORKFAIL_CPP */
/* end file */
