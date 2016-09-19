/*
 * backtraceTest.cpp
 *
 *  Created on: 2014-12-4
 *      Author: hong
 */
#include "../config.h"
#if BACKTRACETEST_CPP

#include <stdio.h>
#include <stdlib.h>		/* atoi() WIFEXITED() */
#include <unistd.h>
#include <sys/user.h>	/* struct user_regs_struct */
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>	/* wait() and waitpid() */
#include <execinfo.h>	/* ptrace() */
#include <signal.h>
#include <errno.h>


#define STACK_TRACE_SIZE	1024


void PrintTrace(void);
void Myfunc_01(int cnt);
void Myfunc_02(void);
void Myfunc_03(void);
void FuncSegfault(void);


int BacktraceTest_backtrace(int argc, char* argv[]);
int BacktraceTest_Child(int argc, char* argv[]);
int BacktraceTest_PrintTrace(int argc, char* argv[]);
int BacktraceTest_PrintTrace_02(int argc, char* argv[]);

/*====================main====================*/
int BacktraceTest(int argc, char* argv[])
{
	int ret = 0;

//	ret = BacktraceTest_backtrace(argc, argv);
//	ret = BacktraceTest_Child(argc, argv);
	ret = BacktraceTest_PrintTrace(argc, argv);
//	ret = BacktraceTest_PrintTrace_02(argc, argv);

	return ret;
}

int BacktraceTest_backtrace(int argc, char* argv[])
{
	if( argc<2 )
	{
		printf("usage:[program name]\n"),exit(-1);
	}

	pid_t childPid;
	childPid = fork();	/* create child process */
	if( childPid< -1 )
	{
		printf("Fork fail!\n"),exit(-1);
	}
	if( 0==childPid )	/* into child process */
	{
		printf("start child process of %d pid.\n", getpid());

		/* before this code, the child process name is same as the father process. */
		/* execl let child process name change,but pid don't change. */
		if( -1==execl( argv[1], argv[1], (char*)NULL) )	/* argv[1] is program name. */
		{
			perror("execl child process failed"),exit(100);
		}
	}
	else	/* into father process */
	{
		int status;
		pid_t exitPid = 0;

		while(1)
		{
			exitPid = waitpid( childPid, &status, 0);
			if( exitPid == childPid )	/* child process exit */
			{
				/* child process exit normally */
				if( WIFEXITED(status) )
				{
					printf("the child process %d exit normally.\n",exitPid);
					printf("the return code is %d.\n",WEXITSTATUS(status));
					break;
				}
				/* child process exit abnormally */
				if( WIFSIGNALED(status) )
				{
					printf("the child process %d exit abnormally.\n",exitPid);
					printf("signal is %d\n", WTERMSIG(status) );
					printf("abnormally exit status is : %d\n", status);
					break;
				}

			}/* if( exitPid == childPid ) */
		}/* while(1) */

		printf("father process of %d pid exit.\n", getpid());
	}

	return 0;
}

int BacktraceTest_Child(int argc, char* argv[])
{
	printf("into BacktraceTest_Child pid is %d...\n", getpid());
	sleep(10);

	void* array[10];
	size_t size;
	char** strings;

	size = backtrace(array, 10);
	strings = backtrace_symbols( array, size);
	printf("get backtrace_symbols count is %d\n", size);
	printf("get stack info is:\n");
	unsigned int j;
	for( j=0; j<size; j++ )
	{
		printf("%s\n", (char*)strings[j]);
	}
	usleep(500);
	free(strings);
	strings = NULL;

	return 100;
}


int BacktraceTest_PrintTrace(int argc, char* argv[])
{
	if (argc != 2)
	{
		printf("usage:[roll times]\n"), exit(-1);
	}

	Myfunc_01(atoi(argv[1]));
	return 0;
}

int BacktraceTest_PrintTrace_02(int argc, char* argv[])
{
	Myfunc_03();
	printf("===================\n===================\n");
	Myfunc_02();

	return 0;
}


void PrintTrace(void)
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
		Myfunc_02();
}

void Myfunc_02(void)
{
	PrintTrace();
}

void Myfunc_03(void)
{
	printf("Myfunc_03()\n");
	Myfunc_02();
}

void FuncSegfault(void)
{
	int *p = NULL;
	*p = 100;
}

#endif
/* end file */
