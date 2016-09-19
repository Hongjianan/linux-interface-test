/*
 * msgQueueRecv.cpp
 *
 *  Created on: 2015-7-30
 *      Author: hongjianan
 */
#include "../config.h"
#if MSGQUEUERECV_CPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <errno.h>
#include <signal.h>

int gTotalMsgLen, gTotalMsgNum;

void SigHandler(int signo);
void DisplayResult(void);
void DumpBuf(int length, char *buf);


int MsgQueueRecv_RecvMsg(int argc, char* argv[]);

int MsgQueueRecv(int argc, char* argv[])
{
	int ret;

	ret = MsgQueueRecv_RecvMsg(argc, argv);

	return ret;
}

int MsgQueueRecv_RecvMsg(int argc, char* argv[])
{
	if (3 != argc)
	{
		printf("usage : <msgName> <max receive message number>\n"), exit(-1);
	}
	int loopTimes = atoi(argv[2]);

	char msgQueueName[128] = {0};
	msgQueueName[0] = '/';
	strcpy(msgQueueName + 1, argv[1]);

	if (SIG_ERR == signal(SIGINT, SigHandler))
	{
		perror("signal fail");
		exit(-1);
	}

	/* create message queue */
	mqd_t mqd = mq_open(msgQueueName, O_RDWR);
	if (-1 == mqd)
	{
		printf("errno=%d\n", errno);
		perror("mq_open error"), exit(-1);
	}
	printf("create message queue [%s] success\n", msgQueueName);

	struct mq_attr mqAttr = {0};
	if (-1 == mq_getattr(mqd, &mqAttr))
	{
		perror("mq_getattr error"), exit(-1);
	}
	int maxMsgLen = mqAttr.mq_maxmsg;

	/* receive message */
	int i;
	char recvBuf[512];
	int recvLen;
	unsigned int msgPrio;
	for (i = 0, gTotalMsgLen = 0, gTotalMsgNum = 0; i < loopTimes; ++i)
	{
		recvLen = mq_receive(mqd, recvBuf, maxMsgLen, &msgPrio);
		if (recvLen > 0)
		{
			++gTotalMsgNum;
			gTotalMsgLen += recvLen;

			recvBuf[recvLen] = '\0';
			printf("rece msg prio[%d]:%s[end]\n", msgPrio, recvBuf);
			DumpBuf(recvLen, recvBuf);
		}
		else if (0 == recvLen)
		{
			printf("rece msg length is zero.\n");
		}
		else if (recvLen < 0)
		{
			perror("recv error");
			break;
		}
	}

	DisplayResult();

	return 0;
}

void DumpBuf(int length, char *buf)
{
	for (int i = 0; i < length; ++i)
	{
		printf("%02x ", buf[i]);
		if (0 != i && 0 == (i % 16))
			printf("\n");
	}
	printf("\n");
}

void DisplayResult(void)
{
	printf("the ending: receive message number[%d], total length[%d]byte.\n",
			gTotalMsgNum, gTotalMsgLen);
}

void SigHandler(int signo)
{
	DisplayResult();

	if (SIG_ERR == signal(signo, SIG_DFL))
	{
		perror("signal SIGINT fail"), exit(-1);
	}

	raise(signo);
}

#endif /* MSGQUEUETEST_CPP */
/* end file */
