/*
 * timerTest.cpp
 *
 *  Created on: 2015-5-7
 *      Author: hongjianan
 */
#include "../config.h"
#if TIMERTEST_CPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <error.h>

int gCnt = 0;

struct timeval timePrio;
struct timeval timeRear;

void ThreadTimer(union sigval v);

int TimerTest_CreateTimer(int argc, char *argv[]);

/*==============main==============*/
int TimerTest(int argc, char *argv[])
{
	int ret;

	ret = TimerTest_CreateTimer(argc, argv);

	return ret;
}


int TimerTest_CreateTimer(int argc, char *argv[])
{
	/* 1. create timer here */
	timer_t timerId;
	struct sigevent se;
	memset(&se, 0, sizeof(se));

	se.sigev_value.sival_int = 110;
	se.sigev_notify			 = SIGEV_THREAD;
//	se.sigev_signo			 = SIGALRM;
	se.sigev_signo			 = SIGEV_THREAD;
	se.sigev_notify_function = ThreadTimer;

	timer_create(CLOCK_REALTIME, &se, &timerId);

	/* 2. settimer */
	struct itimerspec it;

	/* first timing using */
	it.it_value.tv_sec	= 0;
	it.it_value.tv_nsec = 1000000;
	/* after first timing using, T = 1ms */
	it.it_interval.tv_sec	= 0;
	it.it_interval.tv_nsec	= 1000000;

	gettimeofday(&timePrio, NULL);
	timer_settime(timerId, 0, &it, NULL);

	sleep(1);

	printf("======end cnt = %d======\n", gCnt);

	return 0;
}


void ThreadTimer(union sigval v)
{
	++gCnt;

	gettimeofday(&timeRear, NULL);
	printf("using time : %dus.\n", timeRear.tv_usec - timePrio.tv_usec);
	memcpy(&timePrio, &timeRear, sizeof(timePrio));

}


#endif /* TIMERTEST_CPP */
/* end file */
