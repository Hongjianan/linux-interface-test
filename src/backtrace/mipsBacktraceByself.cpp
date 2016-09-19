/*
 * realizeBacktraceByself.cpp
 *
 *  Created on: 2014-12-30
 *      Author: hong
 */
#include "../config.h"
#if MIPSBACKTRACEBYSELF_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <execinfo.h>
#include <signal.h>

#include <dlfcn.h>


void PrintStack(void);
void SignalFunc(int signo, siginfo_t *info, void *dummy);
int InstallSignal(int signo);
void Segfault(void);
void **GetEBP(int dummy);

int MipsBacktraceByself_01(int argc, char *argv[]);
int MipsBacktraceByself_02(int argc, char *argv[]);

/*====================main=====================*/
int MipsBacktraceByself(int argc, char *argv[])
{
	int ret;

//	ret = RealizeBacktraceByself_01(argc, argv);
	ret = RealizeBacktraceByself_02(argc, argv);

	return ret;
}


int MipsBacktraceByself_01(int argc, char *argv[])
{
	InstallSignal(SIGSEGV);
	Segfault();

	return 0;
}


int MipsBacktraceByself_02(int argc, char *argv[])
{
	int dummy = 0;
	int frame = 0;
	Dl_info dlip;
	void **ebp = GetEBP(dummy);
	void **ret = NULL;

	printf("+++++++++++++++++++++++++++++++++++++++++++\n");
	printf("Stack backtrace:\n");
	while (ebp)
	{
		ret = ebp + 1;
		dladdr(*ret, &dlip);
		printf("Frame %d:[ebp=0x%p][ret=0x%p] %s\n", frame++, *ebp, *ret, dlip.dli_sname);
		ebp = (void **)(*ebp);
	}
	printf("--------------------------------------------\n");

	return 0;
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

int InstallSignal(int signo)
{
	struct sigaction act;
	sigemptyset( &act.sa_mask );
	act.sa_flags = SA_SIGINFO;
	act.sa_sigaction = SignalFunc;

	if (sigaction(signo, &act, NULL) < 0)
	{
		printf("install signal error.\n");
		return -1;
	}
	printf("install signal %d success.\n", signo);

	return 0;
}

void SignalFunc(int signo, siginfo_t *info, void *demy)
{
	printf("+++++++SignalFunc++++++++\n");
	PrintStack();
	raise(signo);
	printf("-----end SignalFunc------\n");
}

void Segfault(void)
{
	int *p = NULL;
	*p = 100;
}


void **GetEBP(int dummy)
{
	void **ebp = (void **)&dummy - 2;
	return (ebp);
}

#endif /* MIPSBACKTRACEBYSELF_CPP */
/* end file */
