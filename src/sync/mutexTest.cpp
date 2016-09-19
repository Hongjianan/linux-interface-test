/*
 * mutexTest.cpp
 *
 *  Created on: 2014-11-7
 *      Author: hong
 */
#include "../config.h"
#if MUTEXTEST_CPP

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include "../common/threadAttr.h"

#define CORE_OFFSET			(1)
#define	A_THREAD_PRIO		(90)
#define	B_THREAD_PRIO		(86)
#define	C_THREAD_PRIO		(80)

#define	A_THREAD_NUM		(4)
#define	B_THREAD_NUM		(6)
#define	C_THREAD_NUM		(1)

#define ALL_THREAD_NUM		(A_THREAD_NUM + B_THREAD_NUM + C_THREAD_NUM)

typedef enum
{
	CNT_ADD = 0,
	CNT_SUB = 1
} E_CNTTYPE;

struct ThreadInfo
{
	int			threadId;
	E_CNTTYPE	eCntType;
	int			usleepTime;
	int			*pCntNum;

	int			threadPrio;
	int 		isBindCpu;
};

struct ErrorStat
{
	int			lockErrno;
	uint32_t	perLockError;
	uint64_t	allLockError;

	int			unlockErrno;
	uint32_t	perUnlockError;
	uint64_t	allUnlockError;
};

pthread_mutex_t gPrioMutex;

int gCntNumA = 0;
int gCntNumB = 0;
int gCntNumC = 0;

__thread int gThreadId = 0;
struct ErrorStat gErrorStat[ALL_THREAD_NUM] = { 0 };

void* ThreadCnt(void* arg);

Int32 CreateMutex(pthread_mutex_t *pMutex);
static inline void Lock(pthread_mutex_t *pThread_mutex_t);
static inline void Unlock(pthread_mutex_t *pThread_mutex_t);

int MutexTest_PrioMutex(int argc, char* argv[]);

/*==============main===============*/
int MutexTest(int argc, char* argv[])
{
	int ret;

	ret = MutexTest_PrioMutex(argc, argv);

	return ret;
}


int MutexTest_PrioMutex(int argc, char* argv[])
{
	if (4 != argc)
	{
		printf("usage: [is bind cpu] [thread sleep time(us)] [diaplay time(s)]\n"), exit(-1);
	}
	int isBindCpu	= atoi(argv[1]);
	int usleepTime	= atoi(argv[2]);
	int displayTime = atoi(argv[3]);
	int threadPrio, threadNum;

	if (CreateMutex(&gPrioMutex))
	{
		printf("CreateMutex failed.\n");
		return -1;
	}

	pthread_t threadTmp;
	struct ThreadInfo *pThreadInfo = NULL;
	Uint32 loops, idx, threadCnt;
	int *pCntNum = NULL;

	int threadNumTable[3]  = {A_THREAD_NUM, B_THREAD_NUM, C_THREAD_NUM};
	int threadPrioTable[3] = {A_THREAD_PRIO, B_THREAD_PRIO, C_THREAD_PRIO};
	int *threadCntTable[3] = {&gCntNumA, &gCntNumB, &gCntNumC};
	threadCnt = 0;
	for (idx = 0; idx < sizeof(threadNumTable)/sizeof(threadNumTable[0]); ++idx)
	{
		threadNum	= threadNumTable[idx];
		threadPrio	= threadPrioTable[idx];
		pCntNum		= threadCntTable[idx];

		for (loops = 0; loops < threadNum; ++loops)
		{
			pThreadInfo = (struct ThreadInfo*)malloc(sizeof(struct ThreadInfo));
			if (!pThreadInfo)
			{
				printf("malloc failed.\n");
				return -1;
			}

			pThreadInfo->threadId	= threadCnt++;
			pThreadInfo->usleepTime	= usleepTime;
			pThreadInfo->threadPrio	= threadPrio;
			pThreadInfo->isBindCpu	= isBindCpu;
			pThreadInfo->pCntNum	= pCntNum;

			if ((threadNum / (loops + 1)) < 2)
			{
				pThreadInfo->eCntType = CNT_ADD;
			}
			else
			{
				pThreadInfo->eCntType = CNT_SUB;
			}

			if (pthread_create(&threadTmp, NULL, ThreadCnt, pThreadInfo))
			{
				perror("pthread_create ThreadCnt error"), exit(-1);
			}
		}
	}

	/* display running time */
	for (loops = 0; ; ++loops)
	{
		sleep(displayTime);
		printf("loop[%u]:  numberA[%d] numberB[%d] numberC[%d]\n",
				loops, gCntNumA, gCntNumB, gCntNumC);

		for (idx = 0; idx < ALL_THREAD_NUM; ++idx)
		{
			if (gErrorStat[idx].perLockError)
			{
				gErrorStat[idx].allLockError += gErrorStat[idx].perLockError;

				printf("======[ERROR]: threadId[%d] lock error perNum[%u] allNum[%llu], errno[%d][%s]\n",
						idx, gErrorStat[idx].perLockError, gErrorStat[idx].allLockError,
						gErrorStat[idx].lockErrno, strerror(gErrorStat[idx].lockErrno));

				gErrorStat[idx].perLockError = 0;
			}

			if (gErrorStat[idx].perUnlockError)
			{
				gErrorStat[idx].allUnlockError += gErrorStat[idx].perUnlockError;

				printf("======[ERROR]: threadId[%d] unlock error perNum[%u] allNum[%llu], errno[%d][%s]\n",
						idx, gErrorStat[idx].perUnlockError, gErrorStat[idx].allUnlockError,
						gErrorStat[idx].unlockErrno, strerror(gErrorStat[idx].unlockErrno));

				gErrorStat[idx].perUnlockError = 0;
			}
		}

	}

	return 0;
}

void* ThreadCnt(void* arg)
{
	pthread_mutex_t *mutex = &gPrioMutex;
	struct ThreadInfo *pInfo = (struct ThreadInfo*)arg;

	int threadId		= pInfo->threadId;
	int usleepTime		= pInfo->usleepTime;
	E_CNTTYPE eCntType	= pInfo->eCntType;
	int *pCntNum		= pInfo->pCntNum;
	int threadPrio		= pInfo->threadPrio;
	int isBindCpu		= pInfo->isBindCpu;

	gThreadId			= threadId;

	/* set thread attribute */
	if (threadPrio)
	{
		if (SetSelfPrio(threadPrio))
		{
			printf("thread[%d]: SetSelfPrio(%d) failed.\n", threadId, threadPrio);
			return NULL;
		}
	}

	if (isBindCpu)
	{
		if (UsdpaaBindSelfToCpu((threadId % 7) + CORE_OFFSET))
		{
			printf("thread[%d]: UsdpaaBindSelfToCpu failed.\n", threadId);
			return NULL;
		}
	}

	/* Waitting for every thread is ready. */
	sleep(1);

	while (1)
	{
		Lock(mutex);

		if (CNT_ADD == eCntType)
		{
			++(*pCntNum);
//			printf("++++ prio[%d]\n", threadPrio);
		}
		else if (CNT_SUB == eCntType)
		{
			--(*pCntNum);
//			printf("---- prio[%d]\n", threadPrio);
		}
		else
		{
			printf("thread[%d]: E_CNTTYPE[%d] error.\n", threadId, eCntType);
		}

		if (C_THREAD_PRIO == threadPrio)
		{
			usleep(2 * usleepTime + 1);
		}

		Unlock(mutex);

		if (usleepTime)
		{
			usleep(usleepTime);
		}
	}

	return NULL;
}

Int32 CreateMutex(pthread_mutex_t *pMutex)
{
	pthread_mutexattr_t attr;
	Int32 status;

	status = pthread_mutexattr_init(&attr);/* 互斥量初始化  */
	if(0 != status)
	{
		pthread_mutexattr_destroy(&attr);
		printf("pthread_mutexattr_init fail, errno%s\n", strerror(errno));
		return -1;
	}
	status = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);/* 设置嵌套锁 */
	if(0 != status)
	{
		pthread_mutexattr_destroy(&attr);
		printf("pthread_mutexattr_settype fail, errno%s\n", strerror(errno));
		return -1;
	}

	status = pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);/* 优先级继承  */
	if(0 != status)
	{
		pthread_mutexattr_destroy(&attr);
		printf("pthread_mutexattr_setprotocol fail, errno%s\n", strerror(errno));
		return -1;
	}

	status = pthread_mutex_init(pMutex, &attr);
	if(0 != status)
	{
		printf("pthread_mutex_init fail, errno%s\n", strerror(errno));
		pthread_mutexattr_destroy(&attr);
		return -1;
	}

	pthread_mutexattr_destroy(&attr);

	return status;
}

static inline void Lock(pthread_mutex_t *pThread_mutex_t)
{
	if (pthread_mutex_lock(pThread_mutex_t))
	{
		gErrorStat[gThreadId].lockErrno = errno;
		gErrorStat[gThreadId].perLockError++;
	}
}

static inline void Unlock(pthread_mutex_t *pThread_mutex_t)
{
	if (pthread_mutex_unlock(pThread_mutex_t))
	{
		gErrorStat[gThreadId].unlockErrno = errno;
		gErrorStat[gThreadId].perUnlockError++;
	}
}

#endif /* MUTEXTEST_CPP */
/* end file */
