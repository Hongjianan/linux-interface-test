/*
 * fifoTest.cpp
 *
 *  Created on: 2015-1-10
 *      Author: hong
 */
#include "../config.h"
#if FIFOTEST_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

int gFifoFd = 0;
char gFifoName[128] = {0};

enum ExitLevel
{
	EXIT = 0, UNLINK, CLOSE
};

void perrorExit(const char *info, int level)
{
	perror(info);

	if ((level--) > 0)
		unlink(gFifoName);
	if (level > 0)
		close(gFifoFd);

	exit(EXIT_FAILURE);
}

int RegisterSignal(int signo);
void SigHandler(int signo, siginfo_t *info, void *deummy);

int FifoTest_mkfifoSendProcess(int argc, char *argv[]);
int FifoTest_mkfifoRecvProcess(int argc, char *argv[]);

/*==============main==============*/
int FifoTest(int argc, char *argv[])
{
	int ret;

//	ret = FifoTest_mkfifoRecvProcess(argc, argv);
	ret = FifoTest_mkfifoSendProcess(argc, argv);

	return ret;
}


int FifoTest_mkfifoRecvProcess(int argc, char *argv[])
{
	/* # arguments judgement */
	if (2 != argc)
	{
		printf("Usage: %s [fifo name]\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	memcpy(gFifoName, argv[1], strlen(argv[1]));

	/* # register signal of SIGKILL 9 */
	if (0 == RegisterSignal(SIGINT))
	{
		printf("RegisterSignal success");
	}

	/* # create fifo file */
	if (-1 == mkfifo(gFifoName, 0666))
	{
		perrorExit("mkfifo fail", EXIT);
	}
	printf("mkfifo success\n");

	/* # open fifo file */
	gFifoFd = open(gFifoName, O_RDONLY);
	if (-1 == gFifoFd)
	{
		perrorExit("open fail", UNLINK);
	}
	printf("open success\n");

	/* # read data in fifo file */
	char messageBuf[512] = {0};
	int cnt;
	int rc = 0;
	for (cnt = 0; ; ++cnt)
	{
		rc = read(gFifoFd, messageBuf, sizeof(messageBuf));
		if (-1 == rc)
		{
			perrorExit("read fail", CLOSE);
		}
		else if (0 == rc)
		{
			printf("read end of message\n");
		}
		else
		{
			messageBuf[rc] = '\0';
			printf("[%03d]message :%s\n", cnt, messageBuf);
		}
	}
	/* # close fifo file */
	close(gFifoFd);

	/* # destory fifo file */
	unlink(gFifoName);

	return 0;
}


int FifoTest_mkfifoSendProcess(int argc, char *argv[])
{
	/* # arguments judgement */
	if (2 != argc)
	{
		printf("Usage: %s [fifo name]\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	memcpy(gFifoName, argv[1], strlen(argv[1]));

	/* # register signal */
	if (0 == RegisterSignal(SIGINT))
	{
		printf("RegisterSignal success");
	}

	/* # open fifo file */
	gFifoFd = open(gFifoName, O_WRONLY);
	if (-1 == gFifoFd)
	{
		perrorExit("open fail", UNLINK);
	}
	printf("open success\n");

	/* # read data in fifo file */
	char messageBuf[512] = {0};
	int cnt;
	int rc = 0;
	for (cnt = 0; ; ++cnt)
	{
		sprintf(messageBuf, "[%03d]:message", cnt);
		rc = write(gFifoFd, messageBuf, strlen(messageBuf));
		if (-1 == rc)
		{
			perror("write fail");
			close(gFifoFd);
		}
		else
		{
			printf("[%03d]send success\n", cnt);
		}
		sleep(1);
	}
	/* # close fifo file */
	close(gFifoFd);

	return 0;
}


int RegisterSignal(int signo)
{
	struct sigaction act;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_SIGINFO;
	act.sa_sigaction = SigHandler;

	if (-1 == sigaction(signo, &act, NULL))
	{
		perrorExit("sigaction fail", EXIT);
	}

	return 0;
}

void SigHandler(int signo, siginfo_t *info, void *deummy)
{
	printf("get signal is %d\n", signo);
	close(gFifoFd);
	unlink(gFifoName);
}


#endif /* FIFOTEST_CPP */
/* end file */
