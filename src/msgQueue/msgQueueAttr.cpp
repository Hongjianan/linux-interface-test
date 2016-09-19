/*
 * msgQueueAttr.cpp
 *
 *  Created on: 2015-8-6
 *      Author: hongjianan
 */
#include "../config.h"
#if MSGQUEUEATTR_CPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <error.h>
#include <pthread.h>

#define MAX_MSG_LEN		1
#define MAX_MSG_NUM		1

void *ThreadChangeAttr(void *arg);
void *ThreadSendMsg(void *arg);


int MsgQueueAttr_RecvChangeBlockToNonBlock(int argc, char* argv[]);

int MsgQueueAttr(int argc, char* argv[])
{
	int ret;

	ret = MsgQueueAttr_RecvChangeBlockToNonBlock(argc, argv);

	return ret;
}

int MsgQueueAttr_RecvChangeBlockToNonBlock(int argc, char* argv[])
{
	const char *msgQueueName = "/MsgQueueAttr";

	/* create message queue */
	struct mq_attr mqAttr = {0};
	mqAttr.mq_maxmsg	= MAX_MSG_NUM;
	mqAttr.mq_msgsize	= MAX_MSG_LEN;
	mqd_t mqd = mq_open(msgQueueName, O_CREAT| O_RDWR, 0644, &mqAttr);
	if (-1 == mqd)
	{
		perror("mq_open error"), exit(-1);
	}
	printf("create message queue [%s] success\n", msgQueueName);

	mqAttr.mq_flags |= O_NONBLOCK;
	if (0 != mq_setattr(mqd, &mqAttr, NULL))
	{
		perror("perror: mq_setattr");
		exit(-1);
	}
	printf("mq_setattr O_NONBLOCK success.\n");

	pthread_t threadChangeAttr;
	pthread_create(&threadChangeAttr, NULL, ThreadChangeAttr, &mqd);
	pthread_create(&threadChangeAttr, NULL, ThreadSendMsg, &mqd);

	/* receive message */
	printf("start receive...\n");

	int i;
	char recvBuf[512];
	int recvLen;
	unsigned int prio;
	for (i = 0; ; ++i)
	{
		recvLen = mq_receive(mqd, recvBuf, MAX_MSG_LEN, &prio);
		if (recvLen > 0)
		{
			recvBuf[recvLen] = '\0';
			printf("[%03d]rece msg prio[%d]:%s[end]\n", i, prio, recvBuf);
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

	return 0;
}

void *ThreadChangeAttr(void *arg)
{
	mqd_t mqd = *((mqd_t*)arg);

	sleep(1);
	printf("change attr.\n");

	struct mq_attr mqAttr = {0};

	mqAttr.mq_flags &= ~O_NONBLOCK;
	if (0 != mq_setattr(mqd, &mqAttr, NULL))
	{
		perror("perror: mq_setattr");
		exit(-1);
	}
	printf("mq_setattr O_BLOCK success.\n");


	sleep(5);

	mqAttr.mq_flags |= O_NONBLOCK;
	if (0 != mq_setattr(mqd, &mqAttr, NULL))
	{
		perror("perror: mq_setattr");
		exit(-1);
	}
	printf("mq_setattr O_NONBLOCK success.\n");

	return NULL;
}

void *ThreadSendMsg(void *arg)
{
	mqd_t mqd = *((mqd_t*)arg);

	sleep(7);
	printf("ThreadSendMsg start.\n");

	char sendBuf = 'a';
	mq_send(mqd, &sendBuf, MAX_MSG_LEN, 90);

	return NULL;
}


#endif /* MSGQUEUEATTR_CPP */
/* end file */
