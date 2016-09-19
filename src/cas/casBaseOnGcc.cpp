/*
 * casBaseOnGcc.cpp
 *
 *  Created on: 2015-3-17
 *      Author: hongjianan
 */
#include "../config.h"
#if CASBASEONGCC_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#define LOOP_NUM	2000000

static int countSync = 0;
static int countNoSync = 0;


void* ThreadFunc(void *arg)
{
	for (int i = 0; i < LOOP_NUM; ++i)
	{
		__sync_fetch_and_add(&countSync, 1);
		++countNoSync;
	}

	return NULL;
}

int CaseBaseOnGcc__sync_fetch_and_add(int argc, char *argv[]);

/*================main=================*/
int CaseBaseOnGcc(int argc, char *argv[])
{
	int ret;

	ret = CaseBaseOnGcc__sync_fetch_and_add(argc, argv);

	return ret;
}


int CaseBaseOnGcc__sync_fetch_and_add(int argc, char *argv[])
{
	pthread_t threadTmp;

	for (int i = 0; i < 20; ++i)
	{
		if (0 != pthread_create(&threadTmp, NULL, ThreadFunc, NULL))
		{
			perror("pthread_create fail");
		}
	}

	sleep(3);

	printf("countSync = %d\n", countSync);
	printf("countNoSync = %d\n", countNoSync);

	return 0;
}


#endif /* CASBASEONGCC_CPP */
/* end file */
