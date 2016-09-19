/*
 * signalCatch.cpp
 *
 *  Created on: 2015-6-28
 *      Author: Hong
 */
#include "../config.h"
#if SIGNALCATCH_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/resource.h>
#include <errno.h>
#include <pthread.h>

void *ThreadPrint(void *arg);
int OpenCoreDump(void);
void SegfaulHandler(int signo);


int SignalCatch_CatchAndRaiseSegfault(int argc, char *argv[]);
int SignalCatch_OtherThreadWillExit(int argc, char *argv[]);

/*==============main===============*/
int SignalCatch(int argc, char *argv[])
{
	int ret;

//	ret = SignalCatch_catchAndRaiseSegfault(argc, argv);
	ret = SignalCatch_OtherThreadWillExit(argc, argv);

	return ret;
}


int SignalCatch_CatchAndRaiseSegfault(int argc, char *argv[])
{
	(void)OpenCoreDump();

	/* catch segfault */
	signal(SIGSEGV, SegfaulHandler);

#if 0

	int *p = NULL;
	*p = 100;

#else

	/* wait for receive SIGSEGV signal (kill -11 pid) */
	int loop;
	for (loop = 0; ; ++loop);

#endif

	return 0;
}

int SignalCatch_OtherThreadWillExit(int argc, char *argv[])
{
	/* catch segfault */
	signal(SIGSEGV, SegfaulHandler);

	pthread_t tmp;
	pthread_create(&tmp, NULL, ThreadPrint, NULL);
	sleep(3);

#if 1

	int *p = NULL;
	*p = 100;

#else

	/* wait for receive SIGSEGV signal (kill -11 pid) */
	int loop;
	for (loop = 0; ; ++loop);

#endif

	return 0;
}


void SegfaulHandler(int signo)
{
	printf("signal[%d] is catched by SegfaulHandler[addr=0x%p].\n", signo, SegfaulHandler);
	sighandler_t pFun = signal(signo, SIG_DFL);
	printf("sighandler_t [addr=0x%p].\n", pFun);

	while (1)
	{
		printf("run SegfaulHandler\n");
		sleep(3);
	}

	raise(signo);
}


void *ThreadPrint(void *arg)
{
	int loop;
	for (loop = 0; ; ++loop)
	{
		printf("[%d]thread print\n", loop);
		fflush(stdout);
		sleep(1);
	}

	return NULL;
}

int OpenCoreDump(void)
{
	/* open core dump */
	struct rlimit res;
	if (-1 == getrlimit(RLIMIT_CORE, &res))
	{
		perror("getrlimit fail");
		return -1;
	}
	printf("rlim_cur=%#x, rlim_max=%#x\n", res.rlim_cur, res.rlim_max);

	res.rlim_cur = res.rlim_max = RLIM_SAVED_MAX;
	if (-1 == setrlimit(RLIMIT_CORE, &res))
	{
		perror("setrlimit fail");
		return -1;
	}

	return 0;
}

#endif /* SIGNALCATCH_CPP */
/* end file */
