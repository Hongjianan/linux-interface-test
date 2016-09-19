/*
 * loggerTest.cpp
 *
 *  Created on: 2015-6-25
 *      Author: hongjianan
 */
/*
 *	conclusion:
 *		1、fprintf 和 printf一样都是线程安全的，所有多线程同时打印也没有问题。
 */
#include "../config.h"
#if LOGGERTEST_CPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "logger.h"

#define THREAD_NUM	6

void *ThreadPrint(void *arg);

int LoggerTest_OneThread(int argc, char *argv[]);
int LoggerTest_MuliThread(int argc, char *argv[]);

/*================main==============*/
int LoggerTest(int argc, char *argv[])
{
	int ret;

//	ret = LoggerTest_OneThread(argc, argv);
	ret = LoggerTest_MuliThread(argc, argv);

	return 0;
}


int LoggerTest_OneThread(int argc, char *argv[])
{
	if (4 != argc)
	{
		printf("usage:%s [log name][fileSize][B or KB or MB]\n", argv[0]);
		exit(-1);
	}

	const char *fileName = argv[1];

	int maxFileSize;
	if (!strcmp("B", argv[3]))
	{
		maxFileSize = atoi(argv[2]);
	}
	else if (!strcmp("KB", argv[3]))
	{
		maxFileSize = atoi(argv[2]) * 1024;
	}
	else if (!strcmp("MB", argv[3]))
	{
		maxFileSize = atoi(argv[2]) * 1024 * 1024;
	}
	else
	{
		printf("parament error\n");
		return -1;
	}

	InitLog(&gLogInfo, fileName, maxFileSize);

	int idx;
	for (idx = 0; idx < 100; ++idx)
	{
		LOG_DBG(1, "[%04d]LoggerTest_OneThread test.\n", idx);
	}

	return 0;
}


int LoggerTest_MuliThread(int argc, char *argv[])
{
	if (4 != argc)
	{
		printf("usage:%s [log name][fileSize][B or KB or MB]\n", argv[0]);
		exit(-1);
	}

	const char *fileName = argv[1];

	int maxFileSize;
	if (!strcmp("B", argv[3]))
	{
		maxFileSize = atoi(argv[2]);
	}
	else if (!strcmp("KB", argv[3]))
	{
		maxFileSize = atoi(argv[2]) * 1024;
	}
	else if (!strcmp("MB", argv[3]))
	{
		maxFileSize = atoi(argv[2]) * 1024 * 1024;
	}
	else
	{
		printf("parament error\n");
		return -1;
	}

	InitLog(&gLogInfo, fileName, maxFileSize);

	int thxId[THREAD_NUM];
	pthread_t threadId;
	for (int idx = 0; idx < THREAD_NUM; ++idx)
	{
		thxId[idx] = idx;
		pthread_create(&threadId, NULL, ThreadPrint, &thxId[idx]);
	}

	sleep(2);

	return 0;
}

void *ThreadPrint(void *arg)
{
	int threadId = *((int*)arg);
	for (int idx = 0; idx < 1000; ++idx)
	{
		LOG_DBG(1, "[Thread:%02d][%04d]LoggerTest_OneThread test.\n", threadId, idx);
	}

	return NULL;
}


#endif /* LOGGERTEST_CPP */
/* end file */
