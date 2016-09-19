/*
 * spinlock.cpp
 *
 *  Created on: 2015-3-9
 *      Author: hongjianan
 */
#include "../config.h"
#if SPINLOCK_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <linux/spinlock.h>
#include <error.h>

#define THREAD_NUM	3

static spinlock_t g_spinlock;
unsigned long long g_number = 0;

int Spinlock_lock(int argc, char *argv[]);

/*==============main==============*/
int Spinlock(int argc, char *argv[])
{
	int ret;

	ret = Spinlock(argc, argv);

	return ret;
}


int Spinlock(int argc, char *argv[])
{
	/* 1.init spinlock */
	g_spinlock = SPIN_LOCK_UNLOCK;

	/* 2.create multiple thread */
	pthread_t dummy;
	for (int i = 0; i < THREAD_NUM; ++i)
	{
		if (0 != pthread_create(&dummy, NULL, ThreadAddNum, NULL))
		{
			perror("pthread_create fail");
			exit(errno);
		}
	}

	/* 3.no exit */
	int min = 0;
	while (1)
	{
		sleep(60);
		printf("run time %d minite.\n", min);
	}

	return 0;
}


void* ThreadAddNum(void *arg)
{
	unsigned long long oldNum;
	pthread_t threadID = pthread_self();

	while (1)
	{
		spin_lock(&g_spinlock);
		if (oldNum > g_number)
		{
			printf("number go back\n");
		}
		oldNum = ++g_number;
		spin_unlock(&g_spinlock);
	}

	return 0;
}


#endif /* SPINLOCK_CPP */
/* end file */
