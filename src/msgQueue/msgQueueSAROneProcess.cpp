/*
 * msgQueueSAROneProcess.cpp
 *
 *  Created on: 2015-8-26
 *      Author: hongjianan
 */
#include "../config.h"
#if MSGQUEUESARONEPROCESS_CPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <error.h>
#include <pthread.h>

#define MAX_MSG_LEN		1024 * 12
#define MAX_MSG_NUM		10

char *msgQueueName = NULL;
int msgLen;
int gTimes;
int sleepTimeUS;
int maxSize;

char *gSendBuf = NULL;

void *ThreadRecvMsg(void *arg);
void *ThreadSendMsg(void *arg);


int MsgQueueSAROneProcess_SendAndRecv(int argc, char* argv[]);

/*=====================main====================*/
int MsgQueueSAROneProcess(int argc, char* argv[])
{
	int ret;

	ret = MsgQueueSAROneProcess_SendAndRecv(argc, argv);

	return ret;
}

int MsgQueueSAROneProcess_SendAndRecv(int argc, char* argv[])
{
	if (6 != argc)
	{
		printf("usage: [name] [msgSize] [len] [times] [sleepTime]\n");
		exit(-1);
	}

	msgQueueName = argv[1];
	maxSize = atoi(argv[2]);
	msgLen = atoi(argv[3]);
	gTimes = atoi(argv[4]);
	sleepTimeUS = atoi(argv[5]);

	/* create message queue */
	struct mq_attr mqAttr = {0};
	mqAttr.mq_maxmsg	= MAX_MSG_NUM;
	mqAttr.mq_msgsize	= maxSize;
	mqd_t mqd = mq_open(msgQueueName, O_CREAT| O_RDWR, 0644, &mqAttr);
	if (-1 == mqd)
	{
		perror("mq_open error"), exit(-1);
	}
	printf("create message queue [%s] success\n", msgQueueName);

	if (0 != mq_getattr(mqd, &mqAttr))
	{
		perror("perror: mq_setattr");
		exit(-1);
	}
	printf("mq_getattr: mq_maxmsg = %d, mq_msgsize = %d\n", mqAttr.mq_maxmsg, mqAttr.mq_msgsize);

	pthread_t threadMsg;
	pthread_create(&threadMsg, NULL, ThreadRecvMsg, &mqd);
	for (int i = 0; i < 10; ++i)
	{
		pthread_create(&threadMsg, NULL, ThreadSendMsg, &mqd);
		sleep(6);
	}

	while (1)
	{

	}

	return 0;
}

void *ThreadRecvMsg(void *arg)
{
	mqd_t mqd = *((mqd_t*)arg);

	int i;
	char recvBuf[MAX_MSG_LEN];
	int recvLen;
	unsigned int prio;
	for (i = 0; ; ++i)
	{
		recvLen = mq_receive(mqd, recvBuf, MAX_MSG_LEN, &prio);
		if (recvLen > 0)
		{
			recvBuf[recvLen] = '\0';
			printf("[%03d]rece msg len[%d] prio[%d]\n", i, recvLen, prio);
		}
		else if (0 == recvLen)
		{
			printf("[%03d]rece msg length is zero.\n", i);
			sleep(1);
		}
		else if (recvLen < 0)
		{
			perror("recv error");
			sleep(1);
		}
	}

	close(mqd);

	return NULL;
}

void *ThreadSendMsg(void *arg)
{
	mqd_t mqd = mq_open(msgQueueName, O_RDWR);
	if (-1 == mqd)
	{
		perror("mq_open error"), exit(-1);
	}
	printf("open message queue [%s] success\n", msgQueueName);

	gSendBuf = (char*)malloc(sizeof(char) * msgLen);
	if (NULL == gSendBuf)
	{
		printf("malloc fail\n");
		exit(-1);
	}

	memset(gSendBuf, 'h', msgLen);
	gSendBuf[msgLen - 1] = '\0';

	int idx;
	for (idx = 0; idx < gTimes; ++idx)
	{
		mq_send(mqd, gSendBuf, strlen(gSendBuf), 90);
		usleep(sleepTimeUS);
	}

	close(mqd);

	return NULL;
}


#endif /* MSGQUEUESARONEPROCESS_CPP */
/* end file */
