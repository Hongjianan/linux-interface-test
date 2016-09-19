/*
 * pthreadTest.cpp
 *
 *  Created on: 2015-9-22
 *      Author: hongjianan
 */
#include "../config.h"
#if PTHREADCANCELTEST_CPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <sys/syscall.h>

void *ThreadPrint(void *arg);

static int PthreadcancelTest_pthread_cancel(int argc, char *argv[]);
/*===============main================*/
int PthreadcancelTest(int argc, char *argv[])
{
	int ret;

	ret = PthreadcancelTest_pthread_cancel(argc, argv);

	return ret;
}

int PthreadcancelTest_pthread_cancel(int argc, char *argv[])
{
	printf("main tid[%d]\n", syscall(SYS_gettid));
	pthread_t childThread;
	childThread = pthread_create(&childThread, NULL, ThreadPrint, NULL);

	sleep(3);

	if (pthread_cancel(childThread))
	{
		perror("perror pthread_cancel");
		return -1;
	}

	sleep(10);
	void *ret;
	if (pthread_join(childThread, &ret))
	{
		perror("perror pthread_join");
	}
	if (ret == PTHREAD_CANCELED)
	{
		printf("child thread was canceled\n");
	}
	else
	{
		printf("child thread wasn't canceled (shouldn't happen!)\n");
	}


	return 0;
}


void *ThreadPrint(void *arg)
{
	if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL))
	{
		perror("perror pthread_setcancelstate");
		return NULL;
	}

	printf("thread tid[%d]\n", syscall(SYS_gettid));
	int loop;
	for (loop = 0; ; ++loop)
	{
		pthread_testcancel();
//		printf("thread print[%d]\n", loop);
//		sleep(1);
		loop++;
		pthread_testcancel();
	}

	return NULL;
}


#endif /* PTHREADCANCELTEST_CPP */
/* end file */
