/*
 * cloneTest.cpp
 *
 *  Created on: 2015-8-27
 *      Author: hongjianan
 */
#include "../config.h"
#if CLONETEST_CPP

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include <sched.h>


#define STACK_SIZE (1024 * 10)    /* Stack size for cloned child */

int childFunc(void *arg);

static int CloneTest_clone( int argc, char *argv[] );

/*===============main================*/
int CloneTest(int argc, char *argv[])
{
	int ret;

	ret = CloneTest_clone(argc, argv);

	return ret;
}

int CloneTest_clone(int argc, char *argv[])
{
	if (argc < 2)
	{
	   fprintf(stderr, "Usage: %s <child-hostname>\n", argv[0]);
	   exit(EXIT_SUCCESS);
	}
	char *childName = argv[1];

	pid_t childPid;
	printf("father: pid[%d]\n", getpid());

	char *stack;                    /* Start of stack buffer */
	char *stackTop;                 /* End of stack buffer */

	/* Allocate stack for child */
	stack = (char*)malloc(STACK_SIZE);
	if (stack == NULL)
	{
		perror("perror: malloc");
		exit(-1);
	}
	stackTop = stack + STACK_SIZE;  /* Assume stack grows downward */
	int flag = CLONE_PARENT;

	childPid = clone(childFunc, stackTop, flag, childName);
	if (childPid == -1)
	{
		perror("perror: clone");
		exit(-1);
	}
	printf("clone() returned %ld\n", childPid);

	/* Parent falls through to here */
	sleep(1);

	return 0;
}

int childFunc(void *arg)
{
   printf("child: pid[%d]\n", getpid());

   return 0;           /* Child terminates now */
}


#endif /* CLONETEST_CPP */
/* end file */
