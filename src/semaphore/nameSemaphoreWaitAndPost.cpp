/*
 * nameSemaphoreWaitAndPost.cpp
 *
 *  Created on: 2014-11-5
 *      Author: hong
 */
#include "../config.h"
#if NAMESEMAPHOREWAITANDPOST_CPP

#include "include/includeSemaphoreTest.h"
#include "../aaacommon/printLog.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

int NameSemaphoreWaitAndPost_Wait(int argc, char* argv[]);
int NameSemaphoreWaitAndPost_Post(int argc, char* argv[]);

int NameSemaphoreWaitAndPost(int argc, char* argv[])
{
	PRINT_START( "NameSemaphoreWaitAndPost" );

	int ret;
	ret = NameSemaphoreWaitAndPost_Wait( argc, argv);
//	ret = NameSemaphoreWaitAndPost_Post( argc, argv);

	PRINT_END( "NameSemaphoreWaitAndPost" );
	return ret;
}

int NameSemaphoreWaitAndPost_Wait(int argc, char* argv[])
{
	PRINT_START( "NameSemaphoreWaitAndPost_Wait" );

	if( argc<3 )
	{
		printf("usage: <SemaphoreName> <DisplayTime>\n"),exit(-1);
	}
	if( '/' != argv[1][0] )
	{
		printf("please input Semaphore Name begin with '/'\n"),exit(-1);
	}
	printf("ProcessWaitSem Start...\n");

	pid_t pid = getpid();
	int sumTime = 0;
	int usTime = 0;
	int displayTime = atoi( argv[2] );
	sem_t *sem;
	sem = sem_open( argv[1], O_CREAT, 0777, 0);
	if( NULL==sem )
	{
		perror("sem_open fail"),exit(-1);
	}
	else
	{
		printf("sem_open success,create sem success.\n");
	}
	while(1)
	{
		sem_wait( sem );
		++usTime;
		if( usTime>=2000 )
		{
			usTime = 0;
			++sumTime;
			if( 0==(sumTime%displayTime) )
			{
				printf("wait pid:%d,running time is %dhour,%dminite\n",\
						pid, sumTime/3600, (sumTime-sumTime/3600*3600)/60 );
			}
		}
	}
	sem_close( sem );
	unlink( argv[1] );

	PRINT_END( "NameSemaphoreWaitAndPost_Wait" );
	return 0;
}

int NameSemaphoreWaitAndPost_Post(int argc, char* argv[])
{
	PRINT_START( "NameSemaphoreWaitAndPost_Post" );

	if( argc<2 )
	{
		printf("usage: <SemaphoreName>\n"),exit(-1);
	}
	if( '/' != argv[1][0] )
	{
		printf("please input Semaphore Name begin with '/'\n"),exit(-1);
	}
	printf("ProcessPostSem Start...\n");

	sem_t *sem;
	sem = sem_open( argv[1], O_EXCL);
	if( NULL==sem )
	{
		perror("sem_open fail"),exit(-1);
	}
	else
	{
		printf("sem_open success.\n");
	}
	while(1)
	{
		if( 0 != sem_post(sem) )
		{
			perror("sem_post fail");
		}
		usleep(500);
	}

	PRINT_END( "NameSemaphoreWaitAndPost_Post" );
	return 0;
}

#endif /* NAMESEMAPHOREWAITANDPOST_CPP */
/* end file */

