/*
 * research-pthread_mutex_t.cpp
 *
 *  Created on: 2014-11-9
 *      Author: hong
 */

#include "../config.h"
#if RESEARCH_PTHREAD_MUTEX_T_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

union mutex_char
{
	char byteOfMutex[100];
	pthread_mutex_t mutex;
};

union mutex_char gMutexAndChar;

int Research_pthread_mutex_t_NONInitialMutex(int argc, char* argv[]);

int Research_pthread_mutex_t(int argc, char* argv[])
{
	int ret;

	ret = Research_pthread_mutex_t_NONInitialMutex( argc, argv);
	return ret;
}

int Research_pthread_mutex_t_NONInitialMutex(int argc, char* argv[])
{
	int i;
	for( i=0; i<100; i++)
	{
		printf("i=%d: %d\n", i, (int)gMutexAndChar.byteOfMutex[i]);
	}
	printf("============\n");
	pthread_mutex_init( &gMutexAndChar.mutex, NULL );

	for( i=0; i<100; i++)
	{
		printf("i=%d: %d\n", i, (int)gMutexAndChar.byteOfMutex[i]);
	}

	return 0;
}


#endif /* RESEARCH_PTHREAD_MUTEX_T_CPP */
/* end file */
