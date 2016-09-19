/*
 * statisticsTest.cpp
 *
 *  Created on: 2015-10-13
 *      Author: Hong
 */
#include "../config.h"
#if STATISTICSTEST_CPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>


struct Statistics
{
	unsigned long long count;
};

struct StatisticsPer
{
	unsigned long long countPer;
};

struct Statistics				gData	 	= { 0 };
struct StatisticsPer			gDataPer[2] = { {0} };
volatile struct StatisticsPer	*gpDataPer	= &gDataPer[0];

char		gDataPerIdx	= 0;
const char	gIdxArray[2]= {1, 0};

static inline char GetCurDataPerIdx(void)
{
	return gDataPerIdx;
}

static inline char GetBakDataPerIdx(void)
{
	return gIdxArray[gDataPerIdx];
}

static inline struct StatisticsPer* GetCurDataPer(void)
{
	return &gDataPer[GetCurDataPerIdx()];
}

static inline struct StatisticsPer* GetBakDataPer(void)
{
	return &gDataPer[GetBakDataPerIdx()];
}

static inline void UpdateDataPer(void)
{
	gDataPerIdx = gIdxArray[gDataPerIdx];
	gpDataPer	= GetCurDataPer();

	__sync_synchronize();
}

void* ThreadShowData_DoubleStore(void *arg);

int StatisticsTest_DoubleStore(int argc, char *argv[]);
/*==============main=================*/
int StatisticsTest(int argc, char *argv[])
{
	int ret;

	ret = StatisticsTest_DoubleStore(argc, argv);

	return ret;
}

int StatisticsTest_DoubleStore(int argc, char *argv[])
{
	pthread_t threadCount;
	if (pthread_create(&threadCount, NULL, ThreadShowData_DoubleStore, NULL))
	{
		perror("pthread_create() fail");
		exit(-1);
	}

	unsigned int sleepTimeUS = 1000;
	volatile unsigned long long *pCountPer;

	unsigned int loop;
	for (loop = 0; ; ++loop)
	{
		pCountPer = &gpDataPer->countPer;
		usleep(1000 * 100);
		(*pCountPer)++;
//		usleep(sleepTimeUS);
	}

	return 0;
}

#define SLEEP_TIME_S		5
#define CONFLICT_TIME_S		1
void* ThreadShowData_DoubleStore(void *arg)
{
	unsigned int sleepTimeS = SLEEP_TIME_S - CONFLICT_TIME_S;

	unsigned int loop;
	for (loop = 0; ; ++loop)
	{
		bzero(GetBakDataPer(), sizeof(struct StatisticsPer));
		UpdateDataPer();

#ifdef CONFLICT_TIME_S
		sleep(CONFLICT_TIME_S);
#endif

		gData.count += GetBakDataPer()->countPer;

		printf("[%d]: count[%llu] allCount[%llu]\n",
				loop, GetBakDataPer()->countPer, gData.count);
		fflush(stdout);

#ifdef CONFLICT_TIME_S
		if (sleepTimeS > 0)
		{
			sleep(sleepTimeS);
		}
#else
		sleep(sleepTimeS);
#endif

	}

	return NULL;
}

#endif /* STATISTICSTEST_CPP */
/* end file */
