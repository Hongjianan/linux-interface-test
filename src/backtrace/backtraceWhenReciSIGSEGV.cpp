/*
 * backtraceWhenReciSIGSEGV.cpp
 *
 *  Created on: 2015-1-1
 *      Author: hong
 */
#include "../config.h"
#if BACKTRACEWHENRECVSIGSEGV_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <execinfo.h>
#include <signal.h>


void Myfunc_01(int cnt);
void FuncSegfault(void);

void RegisterSignal(int signo);
void PrintStack(void);
void SigHandler(int signo, siginfo_t *info, void *dummy);


int BacktraceWhenRecvSIGSEGV_PrintStack(int argc, char *argv[]);

/*======================main======================*/
int BacktraceWhenRecvSIGSEGV(int argc, char *argv[])
{
	int ret;

	ret = BacktraceWhenRecvSIGSEGV_PrintStack(argc, argv);

	return ret;
}


int BacktraceWhenRecvSIGSEGV_PrintStack(int argc, char *argv[])
{
	if (2 != argc)
	{
		printf("usage:[loop times]\n"), exit(-1);
	}

	int loopTimes = atoi(argv[1]);

	RegisterSignal(SIGSEGV);

	Myfunc_01(loopTimes);

	return 0;
}


void RegisterSignal(int signo)
{
	struct sigaction act;
	sigemptyset( &act.sa_mask );
	act.sa_flags = SA_SIGINFO;
	act.sa_sigaction = SigHandler;

	if (sigaction(signo, &act, NULL) < 0)
	{
		printf("install signal error.\n"), exit(-1);
	}
}


void SigHandler(int signo, siginfo_t *info, void *dummy)
{
	printf("get signo is %d\n", signo);

	PrintStack();

	signal(signo, SIG_DFL);
	raise(signo);
}

#define STACK_TRACE_SIZE 1024
void PrintStack(void)
{
	void *stackInfo[STACK_TRACE_SIZE];
	char **pszStrings;
	int rc;
	rc = backtrace(stackInfo, STACK_TRACE_SIZE);
	if (0 == rc)
	{
		perror("[FAIL]backtrace"), exit(-1);
	}

	pszStrings = backtrace_symbols(stackInfo, rc);

	if (NULL == pszStrings)
	{
		perror("backtrace_symbols"), exit(-1);
	}

	int i;
	printf("backtrace stack is %d\n", rc);
	for (i = 0; i < rc; i++)
	{
		printf("<%d>:%s\n", i+1, pszStrings[i]);
	}
	free(pszStrings);
}

void Myfunc_01(int cnt)
{
	if (cnt > 0)
		Myfunc_01(--cnt);
	else
		FuncSegfault();
}

void FuncSegfault(void)
{
	int *p = NULL;
	*p = 100;
}


#endif /* BACKTRACEWHENRECISIGSEGV_CPP */
/* end file */
