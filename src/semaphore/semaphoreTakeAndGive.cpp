/*
 * semTakeAndGive.cpp
 *
 *  Created on: 2014-10-31
 *      Author: hong
 */
#include "../config.h"
#if SEMAPHORETAKEANDGIVE_CPP

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>

sem_t g_sem;

bool SetPrio( int prio)
{
	int ret;

	struct sched_param param;
	param.__sched_priority = prio;
	ret = sched_setscheduler( 0, SCHED_FIFO, &param);
	if( 0 != ret )
	{
		printf("setscheduler fail.\n");
		return false;
	}
	return true;
}

void* ThreadPostSem(void *args)/* prio 90 */
{
	SetPrio(90);
	printf("ThreadPostSem Start...\n");
	while(1)
	{
		if(0 != sem_post(&g_sem) )
		{
			printf("sem_post fail\n");
		}
		usleep(500);
	}

	return 0;
}

void* ThreadWaitSem(void *args)/* prio 120 */
{
	printf("ThreadWaitSem Start...\n");
	while(1)
	{
		sem_wait( &g_sem );
	}

	return 0;
}

int SemaphoreTakeAndGive(int argc, char* argv[])
{
	if( argc < 2 )
	{
		printf("please input 1 parament: time\n"),exit(-1);
	}
	printf("Version: 1.1\n");
	printf("SemaphoreWaitAndPost start...\n");

	sem_init( &g_sem, 0, 0);

	int ret;
	pthread_t threadPostSem;
	ret = pthread_create( &threadPostSem, NULL, ThreadPostSem, NULL );

	pthread_t threadWaitSem;
	ret = pthread_create( &threadWaitSem, NULL, ThreadWaitSem, NULL );

	int displayTime = atoi( argv[1] );
	int sumTime = 0;

	while( 1 )
	{
		sleep( displayTime );
		sumTime += displayTime;
		printf("pid=%d:\t time is %d hour, %d minite\n", getpid(), sumTime/3600, (sumTime-sumTime/3600*3600)/60 );
	}

	return 0;
}

#endif /* SEMAPHORETAKEANDGIVE_CPP */
/* end file */
