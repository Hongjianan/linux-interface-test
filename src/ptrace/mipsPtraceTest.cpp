/*
 * mipsPtrace.cpp
 *
 *  Created on: 2014-12-4
 *      Author: hong
 */
#include "../config.h"
#if MIPSPTRACETEST_CPP

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <signal.h>

#include "include/mipsRegs.h"

void handlerSIGSEGV(pid_t childPid, int regRange);
void segfault(void);

int ChildProgram(int argc, char *argv[]);

int MipsPtraceTest_TraceTillExit(int argc, char *argv[]);
int MipsPtraceTest_GetRegs(int argc, char *argv[]);
int MipsPtraceTest_Program(int argc, char *argv[]);
int MipsPtraceTest_KillChild(int argc, char *argv[]);
int MipsPtraceTest_AttachAndDetach(int argc, char *argv[]);

/*===============main===================*/
int MipsPtraceTest(int argc,char *argv[])
{
	int ret;

//	ret = MipsPtraceTest_GetRegs(argc, argv);
//	ret = MipsPtraceTest_Program(argc, argv);
//	ret = MipsPtraceTest_KillChild(argc, argv);
//	ret = MipsPtraceTest_AttachAndDetach(argc, argv);
	ret = MipsPtraceTest_TraceTillExit(argc, argv);
//	ret = ChildProgram(argc, argv);

	return ret;
}


int MipsPtraceTest_TraceTillExit(int argc, char *argv[])
{
	if (2 != argc)
	{
		printf("usage:[child name]\n"), exit(-1);
	}
	int regRange = atoi(argv[3]);
	printf("father pid[%d]\n", getpid());

	pid_t childPid = fork();
	/* into child process */
	if (0 == childPid)
	{
		if (-1 == ptrace(PTRACE_TRACEME, 0, NULL, NULL))
		{
			perror("ptrace PTRACE_TRACEME fail\n"), exit(-1);
		}
		printf("child  pid[%d],ptrace PTRACE_TRACEME success\n", getpid());

		execl(argv[1], argv[1], (char *)NULL);
	}
	/* into father process */
	else if (childPid > 0)
	{
		sleep(1);
		int childStatus;
		pid_t exitPid;

		while (1)
		{
			if (-1 == ptrace(PTRACE_CONT, childPid, NULL, NULL))
			{
				perror("ptrace PTRACE_CONT fail\n"), exit(-1);
			}
			printf("ptrace PTRACE_CONT success\n");

			exitPid = waitpid(childPid, &childStatus, 0);
			printf("father pid[%d] childStatus:[0x%08X]\n", getpid(), childStatus);

			/* not child process exit */
			if (exitPid != childPid)
				continue;

			/* return true if child exit */
			if (WIFEXITED(childStatus))
			{
				printf("father pid[%d] [WIFEXITED], Exit [%d], status [0x%08X]\n", \
						getpid(), WEXITSTATUS(childStatus), childStatus);
				break;
			}

			/* return true if child exit causing signal */
			if (WIFSIGNALED(childStatus))
			{
				printf("father pid[%d] [WIFSIGNALED], Signal [%d], status [0x%08X]\n", \
						getpid(), WTERMSIG(childStatus), childStatus);
				if (WCOREDUMP(childStatus))
				{
					printf("father pid[%d] [WCOREDUMP], CoreDump [%d], status [0x%08X]\n", \
							getpid(), WTERMSIG(childStatus), childStatus);
				}
				continue;
			}

			/* return true if child exit causing signal */
			if (WIFSTOPPED(childStatus))
			{
				printf("father pid[%d] [WIFSTOPPED], StopSig [%d], status [0x%08X]\n", \
						getpid(), WSTOPSIG(childStatus), childStatus);
				if (SIGSEGV == WSTOPSIG(childStatus))
				{
					handlerSIGSEGV(childPid, regRange);
				}
				else
				{
					printf("stop reason is not segfault, father process exit.\n");
				}

				if (-1 == ptrace(PTRACE_CONT, childPid, NULL, NULL))
				{
					perror("ptrace PTRACE_CONT fail\n"), exit(-1);
				}
				sleep(2);
				break;
			}
		}/* while(1) */

#if 0
		printf("exit father process\n");
		exitPid = waitpid(childPid, NULL, 0);
		regs = ptrace(PTRACE_PEEKUSER, childPid, 4 * atoi(argv[1]), NULL);
		printf("Got number of %d regs is %ld.\n", atoi(argv[1]), regs);
		ptrace(PTRACE_CONT, childPid, NULL, NULL);
#endif
	} /* else if (childPid > 0) */
	else
	{
		perror("fork child process fail\n"), exit(-1);
	}

	return 0;
}

int MipsPtraceTest_GetRegs(int argc, char *argv[])
{
	if (argc < 4)
	{
		printf("usage:[regno][arg0][arg0]\n"), exit(-1);
	}
	printf("father pid is %d\n", getpid());

	pid_t exitPid;
	long regs;

	pid_t childPid = fork();
	if (0 == childPid)
	{
		ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		execl("./child", "child", argv[2], argv[3], (char*)NULL);
	}
	else if (childPid > 0)
	{
		int status;
		while (1)
		{
			exitPid = waitpid( childPid, &status, 0);

			if (exitPid == childPid)	/* child process exit */
			{
				if (WIFEXITED(status))
				{
					printf("the child process %d exit normally.\n",exitPid);
					printf("the return code is %d.\n", WEXITSTATUS(status));
					break;
				}

				if (WIFSIGNALED(status))
				{
					printf("the child process %d exit abnormally.\n",exitPid);
					printf("signal is %d\n", WTERMSIG(status) );
					printf("abnormally exit status is : %p\n", status);
					break;
				}
			}
		}/* while(1) */

#if 0
		printf("exit father process\n");
		exitPid = waitpid(childPid, NULL, 0);
		regs = ptrace(PTRACE_PEEKUSER, childPid, 4 * atoi(argv[1]), NULL);
		printf("Got number of %d regs is %ld.\n", atoi(argv[1]), regs);
		ptrace(PTRACE_CONT, childPid, NULL, NULL);
#endif

	}
	else
	{
		perror("fork child process fail\n"), exit(-1);
	}

	return 0;
}


int MipsPtraceTest_Program(int argc, char *argv[])
{
	if (argc < 3) {
		printf("usage:[arg 1][arg 2]\n"), exit(-1);
	}

	int arg0 = atoi(argv[1]);
	int arg1 = atoi(argv[2]);


	return 0;
}


int MipsPtraceTest_KillChild(int argc, char *argv[])
{
	if (2 != argc)
	{
		printf("usage:[sleep time]\n"), exit(-1);
	}

	pid_t childPid;
	childPid = fork();
	if (0 == childPid)	/* into child process */
	{
		printf("start child process\n");

		while (1)
		{
			printf("child pid %d\n", getpid());
			sleep(1);
		}

		return getpid();
	}
	else if (-1 != childPid)	/* into father process */
	{
		if (-1 == ptrace(PTRACE_ATTACH, childPid, NULL, NULL))
		{
			perror("ptrace PTRACE_ATTACH failed"), exit(-1);
		}
		printf("pid:%d attach success\n", getpid());

		sleep(atoi(argv[1]));
		if (-1 == ptrace(PTRACE_KILL, childPid, NULL, NULL))
		{
			perror("ptrace PTRACE_KILL failed"), exit(-1);
		}
		printf("pid:%d kill success\n", getpid());
	}
	else
	{
		perror("fork failed"), exit(-1);
	}

	return getpid();
}


int MipsPtraceTest_AttachAndDetach(int argc, char *argv[])
{
	if (2 != argc)
	{
		printf("usage:[sleep time]\n"), exit(-1);
	}

	pid_t childPid;
	childPid = fork();
	if (0 == childPid)	/* into child process */
	{
#if 0
		if (-1 == ptrace(PTRACE_TRACEME, childPid, NULL, NULL))
		{
			perror("ptrace PTRACE_ATTACH failed"), exit(-1);
		}
#endif
		printf("start child process\n");

		while (1)
		{
			printf("child pid %d\n", getpid());
			sleep(1);
		}

		return getpid();
	}
	else if (-1 != childPid)	/* into father process */
	{
		if (-1 == ptrace(PTRACE_ATTACH, childPid, NULL, NULL))
		{
			perror("ptrace PTRACE_ATTACH failed"), exit(-1);
		}
		printf("pid:%d attach pid:%d success\n", getpid(), childPid);

		if (-1 == ptrace(PTRACE_DETACH, childPid, NULL, NULL))
		{
			perror("ptrace PTRACE_DETACH failed"), exit(-1);
		}
		printf("pid:%d detach pid:%d success\n", getpid(), childPid);

		printf("end father process pid:%d\n", getpid());
	}
	else
	{
		perror("fork failed"), exit(-1);
	}

	return getpid();
}


int ChildProgram(int argc, char *argv[])
{
	if (2 != argc)
	{
		printf("usage:[loop times]\n"), exit(-1);
	}
	int loop = atoi(argv[1]);

	printf("child  pid[%d].\n", getpid());
	for (int i = 1; i <= loop; i++)
	{
		printf("child  pid[%d], <%d>\n", getpid(), i);
		sleep(1);
	}

	if (1 == loop)
	{
		segfault();
	}

	printf("child  pid[%d], exit.\n", getpid());

	return 0;
}


void handlerSIGSEGV(pid_t childPid, int regRange)
{
	int i;
#if 0 /* TEST FAIL:Can't use PTRACE_PEEKUSER to get general register */
	/* use ptrace get ra, sp, bad registers */
	unsigned long reg;
	for (i = 0; i < regRange; i++)
	{
		reg = ptrace(PTRACE_PEEKUSER, childPid, 4 * i, NULL);
		printf("reg:[%02d] [0x%08X].\n", i, reg);
	}
#endif

#if 1
	unsigned long regs[100];
	if (-1 == syscall(SYS_ptrace, (enum __ptrace_request)PTRACE_GETREGS, childPid, NULL, (long)regs))
	{
		perror("ptrace PTRACE_GETREGS fail"), exit(-1);
	}
	printf("ptrace PTRACE_GETREGS success.\n");

	int regsLength = sizeof(regs)/sizeof(unsigned long);
	for (i = 0; i < 120; i++)
	{
		printf("general reg<%03d> [0x%08X]\n", i, regs[i]);
	}
#if 0
	printf("CP0 cp0_status   [0x%08X]\n", regs.cp0_status);
	printf("CP0 hi           [0x%08X]\n", regs.hi);
	printf("CP0 lo           [0x%08X]\n", regs.lo);
	printf("CP0 cp0_badvaddr [0x%08X]\n", regs.cp0_badvaddr);
	printf("CP0 cp0_cause    [0x%08X]\n", regs.cp0_cause);
	printf("CP0 cp0_epc      [0x%08X]\n", regs.cp0_epc);
#endif

#endif

#if 0
	printf("===============Stack data1===========\n");
	__fpCopy = __sp;

	for (i = 0; i < 200; i++)
	{
		/* print function name */
		printf("<%03d> sp=[0x%08X] *sp=[0x%08X]  \n", i, __fpCopy, *__fpCopy);
		__fpCopy++;
	}

#if 0
	/* jump two function stack */
	for (i = 0; i < 2; i++)
	{
		for (; (unsigned int)__sp != (*__sp - 8); __sp++);

		__ra = (unsigned int *)*(++__sp);
		++__sp;
	}

//	__asm__ volatile("mfc0 %0, $8" : "=r"(__bad));	/* bad */
//	__asm__ volatile("move %0, $t7" : "=r"(__bad));
	printf("[%02d] ra=[0x%08X] *ra=[0x%08X]\n", 0, __bad, *__bad);
//	dladdr(__bad, &dlip);
//	printf("[%s]\n", dlip.dli_sname);
#endif

	for (i = 1; (unsigned int)__sp < 0x7FFFFFFF; i++)
	{
		for (; (unsigned int)__sp != (*__sp - 8); __sp++);

		__ra = (unsigned int *)*(++__sp);
		++__sp;
		printf("<%03d> sp=[0x%08X] *sp=[0x%08X]  \n", i, __sp, *__sp);
		printf("[%02d] ra=[0x%08X]\n", i, __ra);
		dladdr(__ra, &dlip);
		printf("[%s]\n", dlip.dli_sname);
	}
#endif

	printf("child stop cause segement fault.\n");
}

void segfault(void)
{
	int *p = NULL;
	*p = 1234;
}

#endif /* MIPSPTRACETEST_CPP */
/* end file */
