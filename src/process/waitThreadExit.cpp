/*
 * waitThreadExit.cpp
 *
 *  Created on: 2015-4-8
 *      Author: hongjianan
 *  Can't wait thread exit, because process is exit,when pthread call exit() function.
 */
#include "../config.h"
#if WAITTHREADEXIT_CPP

#include "include/includeForkTest.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include <sys/syscall.h>

pid_t g_childThread;

void *ThreadExit(void *arg);

int WaitThreadExit_waitpid( int argc, char *argv[] );

int WaitThreadExit( int argc, char *argv[] )
{
	int ret;

	ret = WaitThreadExit_waitpid( argc, argv);

	return ret;
}

int WaitThreadExit_waitpid( int argc, char *argv[] )
{
	int status;
	pid_t exitPid;
	int ret;
	pthread_t childThread;
	if (0 != pthread_create(&childThread, NULL, ThreadExit, NULL))
	{
		perror("pthread_create error");
		exit(-1);
	}
	sleep(1);

	while (1)
	{
		exitPid = waitpid(g_childThread, &status, 0);
		if (exitPid == g_childThread)	/* child process exit */
		{
			printf("status = %d\n", status);
			if (WIFEXITED(status))
			{
				printf("the child process %d exit normally.\n",exitPid);
				printf("the return code is %d.\n",WEXITSTATUS(status));
				break;
			}

			if (WIFSIGNALED(status))
			{
				printf("the child process %d exit abnormally.\n",exitPid);
				printf("signal is %d\n", WTERMSIG(status) );
				printf("abnormally exit status is : %p\n", status);
				break;
			}

		}
	}
	return 0;
}

void *ThreadExit(void *arg)
{
	printf("ThreadExit start\n");
	g_childThread = syscall(SYS_gettid);
	sleep(10);
	printf("ThreadExit end\n");

}


#endif/* WAITTHREADEXIT_CPP */
/* end file */
