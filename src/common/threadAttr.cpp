/*
 * threadAttr.cpp
 *
 *  Created on: 2016-2-24
 *      Author: hongjianan
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#define _GNU_SOURCE
#include <pthread.h>
#define __USE_GNU
#include <sched.h>

#include "threadAttr.h"

#ifndef __CYGWIN__
STATUS SetSelfPrio(const Int32 prio)
{
	struct sched_param param;
	param.sched_priority = prio;

	if (0 != sched_setscheduler(0, SCHED_FIFO, &param))
	{
		perror("sched_setscheduler fail");
		return APP_STATUS_ERROR;
	}

	return APP_STATUS_SUCCESS;
}

static STATUS UsdpaaBindCpu(pthread_t tid, const Uint32 cpuNo)
{
	cpu_set_t cpuSet;
	CPU_ZERO(&cpuSet);
	CPU_SET(cpuNo, &cpuSet);

	if (0 != pthread_setaffinity_np(tid, sizeof(cpuSet), &cpuSet))
	{
		return APP_STATUS_ERROR;
	}

	return APP_STATUS_SUCCESS;
}

STATUS UsdpaaBindSelfToCpu(const Uint32 cpuNo)
{
	return UsdpaaBindCpu(pthread_self(), cpuNo);
}

static STATUS UsdpaaNoBindCpu(pthread_t tid)
{
	cpu_set_t cpuSet;
	CPU_ZERO(&cpuSet);

	Uint8 cpuNo;
	for (cpuNo = 0; cpuNo < 8; ++cpuNo)
	{
		CPU_SET(cpuNo, &cpuSet);
	}

	if (0 != pthread_setaffinity_np(tid, sizeof(cpuSet), &cpuSet))
	{
		return APP_STATUS_ERROR;
	}

	return APP_STATUS_SUCCESS;
}

STATUS UsdpaaNoBindSelfToCpu(void)
{
	return UsdpaaNoBindCpu(pthread_self());
}
#endif

/* end of file */
