/*
 * fgetsTest.cpp
 *
 *  Created on: 2015-8-6
 *      Author: hongjianan
 */
#include "../config.h"
#if FGETSTEST_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <mqueue.h>


#define MAX_INPUT_BUF	10
#define MAX_THREAD_NUM	1

struct ThreadManager
{
	int threadNum;
	struct HeadList;
};

enum ThreadStatus
{
	THREAD_INIT			= 0,
	THREAD_INIT_DONE,
	THREAD_QUIT,
	THREAD_RUNNING,
	THREAD_SLEEPING,
	THREAD_SUSPEND,
	THREAD_BLOCK
};

struct ThreadMsg
{
	int				id;
	pthread_t		pid;
	char			msg;
	unsigned int 	status;
};

const char *gStrUsage =
	"h: show help\n"
	"q: quit\n"
	"s: thread turn to sleeping status\n"
	"r: thread turn to running  status\n"
	"b: thread turn to block  status\n"
	"w: thread turn to waiting  status\n";


void *ThreadBlockRecv(void *arg);
void InputLoop(struct ThreadMsg *pThreadMsg, int threadNum);
void ProgramUsage(void);
void ProgramStatus(struct ThreadMsg *pThreadMsg, int threadNum, enum ThreadStatus status);

int FgetsTest_fgets(int argc, char *argv[]);

/*================main==============*/
int FgetsTest(int argc, char *argv[])
{
	int ret;

	ret = FgetsTest_fgets(argc, argv);

	return ret;
}


int FgetsTest_fgets(int argc, char *argv[])
{
	printf("pid = %d\n", getpid());
	printf("%s", gStrUsage);

	struct ThreadMsg *pThreadMsg;
	pThreadMsg = (struct ThreadMsg*)malloc(sizeof(struct ThreadMsg) * MAX_THREAD_NUM);

	int idx;
	for (idx = 0; idx < MAX_THREAD_NUM; ++idx)
	{
		pThreadMsg[idx].id		= idx;
		pThreadMsg[idx].status	= THREAD_INIT;

		if (0 != pthread_create(&pThreadMsg[idx].pid, NULL, ThreadBlockRecv, &pThreadMsg[idx]))
		{
			perror("perror: pthread_create()");
			exit(-1);
		}
	}

	InputLoop(pThreadMsg, MAX_THREAD_NUM);

	int exitNum;
	for (exitNum = 0; exitNum < MAX_THREAD_NUM; )
	{
		for (idx = 0; idx < MAX_THREAD_NUM; ++idx)
		{
			if (0 == pthread_tryjoin_np(pThreadMsg[idx].pid, NULL))
			{
				++exitNum;
				printf("[%02d] is really exit.\n", pThreadMsg[idx].id);
			}
		}
	}

	free(pThreadMsg);

	return 0;
}


void InputLoop(struct ThreadMsg *pThreadMsg, int threadNum)
{
	char inBuf[MAX_INPUT_BUF + 1];
	char inChar;
	int loop;
	for (loop = 0; ; ++loop)
	{
		if (NULL != fgets(inBuf, sizeof(inBuf), stdin))
		{
			inChar = inBuf[0];
			switch (inChar)
			{
			case 'h':
				ProgramUsage();
				break;

			case 'q':
				ProgramStatus(pThreadMsg, threadNum, THREAD_QUIT);
				goto THE_END;

			case 's':
				ProgramStatus(pThreadMsg, threadNum, THREAD_SLEEPING);
				break;

			case 'r':
				ProgramStatus(pThreadMsg, threadNum, THREAD_RUNNING);
				break;

			case 'b':
				ProgramStatus(pThreadMsg, threadNum, THREAD_BLOCK);
				break;

			default:
				printf("error default.\n");
				break;
			}
		}
		else
		{
			perror("perror: fgets()");
		}
	}

THE_END:
	return;
}


void *ThreadBlockRecv(void *arg)
{
	struct ThreadMsg *msg = (struct ThreadMsg*)arg;

	int isQuit, runLoop;
	for (isQuit = 0, runLoop = 0; !isQuit; )
	{
		switch(msg->status)
		{
		case THREAD_INIT:
			msg->status = THREAD_RUNNING;
			break;

		case THREAD_RUNNING:
			++runLoop;
			if (0 == runLoop % 50000000)
			{
				printf("[%02d] is running.\n", msg->id);
			}
			break;

		case THREAD_SLEEPING:
			sleep(1);
			break;

		case THREAD_QUIT:
			isQuit = 1;
			break;

		case THREAD_BLOCK:
			{
				int recvLen;
				printf("thread go to block status.\n");
				char recvBuf[1024];
				struct mq_attr attr = {0};
				attr.mq_maxmsg  = 10;
				attr.mq_msgsize = 10;
				mqd_t mqd = mq_open("/tmp", O_CREAT| O_RDWR, 0644, &attr);
				if (-1 == mqd)
				{
					perror("perror mq_open()");
					exit(-1);
				}

				recvLen = mq_receive(mqd, recvBuf, sizeof(recvBuf), NULL);
				if (recvLen > 0)
				{
					recvBuf[recvLen] = '\0';
					printf("[len:%d]%s\n", recvLen, recvBuf);
				}
				else if (0 == recvLen)
				{
					printf("recvLen = 0\n");
				}
				else
				{
					perror("perror mq_receive");
				}
				sleep(2);
			}
			break;

		default:
			printf("error default.\n");
			break;
		}
	}

	printf("[%02d] thread exit.\n", msg->id);

	return NULL;
}

void ProgramUsage(void)
{
	printf("%s", gStrUsage);
}

void ProgramStatus(struct ThreadMsg *pThreadMsg, int threadNum, enum ThreadStatus status)
{
	for (int idx = 0; idx < threadNum; ++idx)
	{
		pThreadMsg[idx].status = status;
	}
}

#endif /* FGETSTEST_CPP */
/* end file */
