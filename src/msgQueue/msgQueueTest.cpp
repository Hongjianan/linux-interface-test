/*
 * msgQueueTest.cpp
 *
 *  Created on: 2014-11-5
 *      Author: hong
 */
#include "../config.h"
#if MSGQUEUETEST_CPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <error.h>

mqd_t CreateMsgQueue(const char *msgQueueName, int maxMsgNum, int maxMsgSize);
int   MsgQueueSendLoop(mqd_t msgQueue, char *sendBuf, size_t bufLen, int sendLoops, int intervalTime);
void  ShowMsgQueueAttr(mqd_t msgFd, struct mq_attr *attr);
void  ShowMsgQueueFlag(mqd_t msgFd, struct mq_attr *attr);

int MsgQueueTest_SendMsg(int argc, char* argv[]);
int MsgQueueTest_RecvMsg(int argc, char* argv[]);
int MsgQueueTest_Create(int argc, char* argv[]);

int MsgQueueTest(int argc, char* argv[])
{
	int ret;

//	ret = MsgQueueTest_RecvMsg(argc, argv);
	ret = MsgQueueTest_SendMsg(argc, argv);
//	ret = MsgQueueTest_Create(argc, argv);

	return ret;
}

#if 0
int MsgQueueTest_RecvMsg(int argc, char* argv[])
{
	if(2 != argc)
	{
		printf("usage:[msgQueue name]\n"), exit(-1);
	}

	/* name */
	char msgQueueName[128] = {0};
	msgQueueName[0] = '/';
	strcpy(msgQueueName + 1, argv[1]);

	/* is create */
	unsigned char isCreate = argv[2][0];

	/* create message queue */

	if ('y' == isCreate)
	{

	}

	/* create message queue */
	mqd_t mqdCreate = mq_open(msgQueueName, O_CREAT| O_RDWR, 0644, NULL);
	if( -1 == mqdCreate)
	{
		perror("mq_open error"),exit(-1);
	}
	else
	{
		printf("create message queue success\n");
	}
	/* receive message */
	struct mq_attr getAttr = {0};
	if( -1 == mq_getattr( mqd, &getAttr) )
	{
		perror("mq_getattr error"),exit(-1);
	}
	char *msg = NULL;
	if (NULL == (msg = (char*)calloc( getAttr.mq_msgsize, sizeof(char))))
	{
		perror("calloc error"), exit(-1);
	}
	int recvLen = 0;
	unsigned int msgPrio;

	sleep(atoi(argv[2]));
	while (1)
	{
		if ((recvLen = mq_receive(mqd, msg, getAttr.mq_msgsize, &msgPrio)) < 0)
		{
			perror("mq_receive error");
			if (-1 == mq_close(mqd))
			{
				perror("mq_close erro"),exit(-1);
			}
		}
		msg[recvLen] = 0;
		printf("rece size=%d, prio=%d, msg=%s", receicedLen, msgPrio, msg);
	}
	/* close message queue */
	if (-1 == mq_close(mqd))
	{
		perror("mq_close error"), exit(-1);
	}
	if (-1 == unlink(argv[1]))
	{
		perror("unlink error"), exit(-1);
	}

	free(msg);

	return 0;
}
#endif

int MsgQueueTest_SendMsg(int argc, char* argv[])
{
	if(5 != argc)
	{
		printf("usage:[msgQueueName] [sendMsgNum] [send interval time us] [sendBlock(y/n)]\n"), exit(-1);
	}

	/* name */
	char msgQueueName[128] = {0};
	msgQueueName[0] = '/';
	strcpy(msgQueueName + 1, argv[1]);

	int sendMsgNum = atoi(argv[2]);
	int intervalTime = atoi(argv[3]);
	char isBlock = argv[4][0];

	/* create message queue */
	mqd_t mqdOpen = mq_open(msgQueueName, O_WRONLY);
	if (-1 == mqdOpen)
	{
		perror("mq_open error"), exit(-1);
	}
	printf("open message queue[%s] success\n", msgQueueName);

	/* set msgQueue attribute */
	struct mq_attr attr = {0};
	ShowMsgQueueAttr(mqdOpen, &attr);

	attr.mq_flags = ('y' == isBlock) ? (attr.mq_flags & ~O_NONBLOCK) : (attr.mq_flags | O_NONBLOCK);
	if (-1 == mq_setattr(mqdOpen, &attr, NULL))
	{
		perror("mq_getattr error"), exit(-1);
	}

	/* send message */
	if (-1 == mq_getattr(mqdOpen, &attr))
	{
		perror("mq_getattr error"),exit(-1);
	}
	int maxMsgLen = attr.mq_msgsize;

	char *sendBuf;
	sendBuf = (char*)memalign(alignof(int), maxMsgLen * sizeof(char));
	if (NULL == sendBuf)
	{
		perror("memalign error"), exit(-1);
	}

	/* send message */
	MsgQueueSendLoop(mqdOpen, sendBuf, maxMsgLen, );

	free(sendBuf);

	return 0;
}


int MsgQueueTest_Create(int argc, char* argv[])
{
	if(5 != argc)
	{
		printf("usage:[msgQueue Name] [maxMsgSize] [maxMsgNum] [Block(y/n)]\n"), exit(-1);
	}

	/* name */
	char msgQueueName[128] = {0};
	msgQueueName[0] = '/';
	strcpy(msgQueueName + 1, argv[1]);

	/* attribute */
	int maxMsgSize	= atoi(argv[2]);
	int maxMsgNum	= atoi(argv[3]);
	char isBlock	= argv[4][0];

	/* create message queue */
	mqd_t mqdCreate = CreateMsgQueue(msgQueueName, maxMsgNum, maxMsgSize);
	if (-1 == mqdCreate)
	{
		perror("mq_open error"), exit(-1);
	}
	printf("create message queue[%s] success\n", msgQueueName);

	struct mq_attr attr = {0};
	attr.mq_flags = ('y' == isBlock) ? (attr.mq_flags & ~O_NONBLOCK) : (attr.mq_flags | O_NONBLOCK);
	if (-1 == mq_setattr(mqdCreate, &attr, NULL))
	{
		perror("mq_getattr error"), exit(-1);
	}
	ShowMsgQueueAttr(mqdCreate, &attr);

	/* create message queue */
	mqd_t mqdOpen = mq_open(msgQueueName, O_RDWR);
	if (-1 == mqdOpen)
	{
		perror("mq_open error"), exit(-1);
	}
	printf("open message queue[%s] success\n", msgQueueName);

	ShowMsgQueueAttr(mqdOpen, &attr);

	attr.mq_flags = ('n' == isBlock) ? (attr.mq_flags & ~O_NONBLOCK) : (attr.mq_flags | O_NONBLOCK);
	if (-1 == mq_setattr(mqdOpen, &attr, NULL))
	{
		perror("mq_getattr error"), exit(-1);
	}

	/* */
	ShowMsgQueueFlag(mqdOpen, &attr);
	ShowMsgQueueFlag(mqdCreate, &attr);

	/* close */
	close(mqdCreate);
	close(mqdOpen);

	return 0;
}

mqd_t CreateMsgQueue(const char *msgQueueName, int maxMsgNum, int maxMsgSize)
{
	struct mq_attr attr = {0};
	attr.mq_maxmsg  = maxMsgNum;
	attr.mq_msgsize = maxMsgSize;
	mqd_t mqFd = mq_open(msgQueueName, O_CREAT| O_RDWR, 0644, &attr);
	if (-1 == mqFd)
	{
		perror("mq_open error");
		return -1;
	}

	return mqFd;
}

int MsgQueueSendLoop(mqd_t msgQueue, char *sendBuf, size_t bufLen, int sendLoops, int intervalTime)
{
	if (msgQueue < 3 || NULL == sendBuf || 0 == bufLen || 0 == sendLoops)
	{
		printf("argument error.\n");
		return -1;
	}

	for (int idx = 0; idx < bufLen; ++idx)
	{
		sendBuf[idx] = '0' + (idx % 10);
	}

	int *sendTimesStamp = (int*)sendBuf;
	for (int times = 0; times < sendLoops; ++times)
	{
		*sendTimesStamp = times;
		if (-1 == mq_send(msgQueue, sendBuf, bufLen, 90))
		{
			perror("mq_send error");
			if (-1 == mq_close(msgQueue))
			{
				perror("mq_close error");
			}
			return -1;
		}
		usleep(intervalTime);
	}

	return 0;
}

void ShowMsgQueueFlag(mqd_t msgFd, struct mq_attr *attr)
{
	if (-1 == mq_getattr(msgFd, attr))
	{
		perror("mq_getattr error"), exit(-1);
	}
	printf("[mqd:0x%08x]mq_flags is %s\n", msgFd, (attr->mq_flags & O_NONBLOCK) ? "O_NONBLOCK" : "O_BLOCK");
}

void ShowMsgQueueAttr(mqd_t msgFd, struct mq_attr *attr)
{
	if (-1 == mq_getattr(msgFd, attr))
	{
		perror("mq_getattr error"), exit(-1);
	}
	printf("[mqd:0x%08x]mq_flags   is %s\n",  msgFd, (attr->mq_flags & O_NONBLOCK) ? "O_NONBLOCK" : "O_BLOCK");
	printf("[mqd:0x%08x]mq_maxmsg  is %ld\n", msgFd, attr->mq_maxmsg);
	printf("[mqd:0x%08x]mq_msgsize is %ld\n", msgFd, attr->mq_msgsize);
	printf("[mqd:0x%08x]mq_curmsgs is %ld\n", msgFd, attr->mq_curmsgs);
}

#endif /* MSGQUEUETEST_CPP */
/* end file */
