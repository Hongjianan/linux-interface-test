/*
 * semaphoreTest.cpp
 *
 *  Created on: 2014��11��5��
 *      Author: Hong
 */
#include "../config.h"
#if SEMAPHORETEST_CPP

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>

int SemaphoreTest_WaitProcess(int argc, char* argv[]);	/* beside SemaphoreTest_PostProcess() */
int SemaphoreTest_PostProcess(int argc, char* argv[]);

int SemaphoreTest(int argc, char* argv[])
{
	int ret;
	ret = SemaphoreTest_WaitProcess( argc, argv);
//	ret = SemaphoreTest_PostProcess( argc, argv);
	return ret;
}

int SemaphoreTest_WaitProcess(int argc, char* argv[])
{
	if(argc < 2)
	{
		printf("please input 1 parament: SemaphoreName\n"),exit(-1);
	}
	if( ''argv[1][0])
	sem_t* pSem;

	pSem = sem_open( argv[1], O_CREAT, 0777, 0);
	while(1)
	{
		sem_wait( pSem );
	}
	return 0;
}


int SemaphoreTest_PostProcess(int argc, char* argv[])
{
	sem_t sem;
	return 0;
}

#endif
/* end file */
