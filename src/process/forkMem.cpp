/*
 * forkMem.cpp
 *
 *  Created on: 2015Äê8ÔÂ26ÈÕ
 *      Author: Hong
 */
#include "../config.h"
#if FORKMEM_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define CMD_FREE	"free -m"
const int MAX_MEM_SIZE = 0x200000;	/* 2MB */

unsigned char gMemory[MAX_MEM_SIZE] = {0};

static int ForkMem_fork(int argc, char *argv[]);

/*==============main=============*/
int ForkMem(int argc, char *argv[])
{
	int ret;

	ret = ForkMem_fork(argc, argv);

	return ret;
}

int ForkMem_fork(int argc, char *argv[])
{
	unsigned char localMemory[MAX_MEM_SIZE];
	pid_t childPid;
	printf("father: pid[%d]\n", getpid());
	system(CMD_FREE);

	memset(gMemory, 'g', sizeof(gMemory)/2);
	printf("\nfather: memset global memory\n");
	system(CMD_FREE);

	memset(localMemory, 'l', sizeof(localMemory)/2);
	printf("\nfather: memset local memory\n");
	system(CMD_FREE);

	childPid = fork();
	if (childPid < 0)
	{
		perror("perror: fork()");	exit(-1);
	}
	else if (0 == childPid)
	{
		printf("\n\nchild: pid[%d]\n", getpid());
		system(CMD_FREE);

		memset(gMemory, 'G', sizeof(gMemory));
		printf("\nchild: memset global memory\n");
		system(CMD_FREE);

		memset(localMemory, 'L', sizeof(localMemory));
		printf("\nchild: memset local memory\n");
		system(CMD_FREE);
		printf("child: exit.\n");
	}
	else
	{
		sleep(2);
		printf("father: global[%c] local[%c]\n", gMemory[0], localMemory[0]);
		printf("father: exit.\n");
	}

	return 0;
}

#endif /* FORKMEM_CPP */
/* end file */
