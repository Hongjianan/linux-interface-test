/*
 * atexitTest.cpp
 *
 *  Created on: 2015-9-16
 *      Author: hongjianan
 */
#include "../config.h"
#if ATEXITTEST_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

void ExitHandler(void);

void *ThreadPrint(void *arg);

int AtexitTest_atexit(int argc, char *argv[]);
int AtexitTest_OtherThreadTest(int argc, char *argv[]);
/*==============main===============*/
int AtexitTest(int argc, char *argv[])
{
	int ret;

//	ret = AtexitTest_atexit(argc, argv);
	ret = AtexitTest_OtherThreadTest(argc, argv);

	return ret;
}

int AtexitTest_OtherThreadTest(int argc, char *argv[])
{
	int ret;
	ret = sysconf(_SC_ATEXIT_MAX);
	printf("ATEXIT_MAX = %ld\n", ret);

	ret = atexit(ExitHandler);
	if (ret)
	{
		perror("perror: atexit()");
		exit(EXIT_FAILURE);
	}

	pthread_t tmp;
	pthread_create(&tmp, NULL, ThreadPrint, NULL);
	sleep(3);

	return 0;
}

int AtexitTest_atexit(int argc, char *argv[])
{
	int ret;
	ret = sysconf(_SC_ATEXIT_MAX);
	printf("ATEXIT_MAX = %ld\n", ret);

	ret = atexit(ExitHandler);
	if (ret)
	{
		perror("perror: atexit()");
		exit(EXIT_FAILURE);
	}

//#define USE_EXIT

#ifdef USE_EXIT
	exit(EXIT_SUCCESS);
#else
	return 0;
#endif
}

void *ThreadPrint(void *arg)
{
	int loop;
	for (loop = 0; ; ++loop)
	{
		printf("[%d]thread print\n", loop);
		fflush(stdout);
		sleep(1);
	}

	return NULL;
}

void ExitHandler(void)
{
	int loop;
	for (loop = 0; loop < 2; ++loop)
	{
		printf("[%03d]call exit()\n", loop);
		sleep(1);
	}

	/* continue call back exit, if don't call atexit, process will exit. */
	if (atexit(ExitHandler))
	{
		perror("perror: atexit()");
		exit(EXIT_FAILURE);
	}
}


#endif /* ATEXITTEST_CPP */
/* end file */
