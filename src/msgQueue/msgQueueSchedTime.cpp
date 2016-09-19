/*
 * msgQueueSAROneProcess.cpp
 *
 *  Created on: 2015-8-26
 *      Author: hongjianan
 */
#include "../config.h"
#if MSGQUEUE_SCHED_TIME_CPP

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <error.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include "common/useTime.h"

#define MAX_MSG_LEN		(sizeof(int))
#define MAX_MSG_NUM		10

char *msgQueueName = "/hong";
int		gRangeTime;

mqd_t gCommMsgQueue;
struct timeval	gBeginTime;
int				gUseTime;

void *ThreadRecvMsg(void *arg);
void *ThreadSendMsg(void *arg);

int MsgQueueSchedTime_UseTime(int argc, char* argv[]);

/*=====================main====================*/
int MsgQueueSchedTime(int argc, char* argv[])
{
	int ret;

	ret = MsgQueueSchedTime_UseTime(argc, argv);

	return ret;
}

int MsgQueueSchedTime_UseTime(int argc, char* argv[])
{
	if (2 != argc)
	{
		printf("usage: [rangeTime]\n");
		return -1;
	}

	gRangeTime = atoi(argv[1]);

	/* create message queue */
	struct mq_attr mqAttr = {0};
	mqAttr.mq_maxmsg	= MAX_MSG_NUM;
	mqAttr.mq_msgsize	= MAX_MSG_LEN;
	gCommMsgQueue = mq_open(msgQueueName, O_CREAT| O_RDWR, 0644, &mqAttr);
	if (-1 == gCommMsgQueue)
	{
		perror("mq_open error"), exit(-1);
	}
	printf("create message queue [%s] success\n", msgQueueName);

	if (0 != mq_getattr(gCommMsgQueue, &mqAttr))
	{
		perror("perror: mq_setattr");
		exit(-1);
	}
	printf("mq_getattr: mq_maxmsg = %d, mq_msgsize = %d\n", mqAttr.mq_maxmsg, mqAttr.mq_msgsize);

	pthread_t threadMsg;
	if (pthread_create(&threadMsg, NULL, ThreadRecvMsg, NULL))
	{
		perror("pthread_create failed");
		return -1;
	}

	sleep(1);

	if (pthread_create(&threadMsg, NULL, ThreadSendMsg, NULL))
	{
		perror("pthread_create failed");
		return -1;
	}

	while (1)
	{
		sleep(3600);
	}

	return 0;
}

void *ThreadRecvMsg(void *arg)
{
	printf("=======thread receive message pid[%d]========\n", syscall(SYS_gettid));

	int			recvLen;
	uint32_t	prio;
	int			recvData;

	int idx;
	for (idx = 0; ; ++idx)
	{
		recvLen = mq_receive(gCommMsgQueue, (char*)&recvData, MAX_MSG_LEN, &prio);
		if (recvLen < 0)
		{
			perror("mq_receive() error");
		}
		else if (0 == recvLen)
		{
			printf("[%03d]rece msg length is zero.\n", idx);
		}

		gUseTime = EndCntTime(&gBeginTime);
		if (gUseTime > gRangeTime)
		{
			printf("idx[%d]: recvData[%d] use time [%dus].\n", idx, recvData, gUseTime);
		}
	}

	return NULL;
}

void *ThreadSendMsg(void *arg)
{
	sleep(1);
	printf("=======thread send message pid[%d]========\n", syscall(SYS_gettid));

	int idx;
	for (idx = 0; ; ++idx)
	{
		usleep(1000);
		if (mq_send(gCommMsgQueue, (char*)&idx, sizeof(idx), 90))
		{
			perror("mq_send failed");
		}

		StartCntTime(&gBeginTime);
	}

	return NULL;
}


#endif /* MSGQUEUE_SCHED_TIME_CPP */
/* end file */
