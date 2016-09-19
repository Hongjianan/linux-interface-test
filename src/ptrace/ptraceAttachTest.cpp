/*
 * ptraceAttachTest.cpp
 *
 *  Created on: 2014-11-1
 *      Author: hong
 */
#include "../config.h"
#if PTRACEATTACHTEST_CPP

#include <sys/ptrace.h>	/* ptrace() */
#include <sys/types.h>
#include <sys/wait.h>	/* wait() and waitpid() */
#include <sys/reg.h>   /* ORIG_EAX etc */
#include <sys/user.h>	/* struct user_regs_struct */
#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>		/* atoi() WIFEXITED() */

int PtraceAttachTest_Attach(int argc, char *argv[]);

int PtraceAttachTest(int argc, char *argv[])
{
	int ret;

	ret = PtraceAttachTest_Attach(argc, argv);

	return ret;
}

int PtraceAttachTest_Attach(int argc, char *argv[])
{
	pid_t traced_process;
	struct user_regs_struct regs;
	long ins;
#if 0
	if( argc != 2 )
	{
		printf("Usage: %s <pid to be traced>\n",argv[0]);
		exit(1);
	}
#endif

	traced_process = fork();
	if( traced_process< -1 )
	{
		printf("Fork fail!\n"),exit(-1);
	}
	if( 0==traced_process )	/* into child process */
	{
		ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		int i;
		for(i = 0;i < 3; ++i)
		{
			printf("My counter: %d\n", i);
			//sleep(1);
		}
		return 0;
	}
	else	/* into father process */
	{
		ptrace(PTRACE_ATTACH, traced_process, NULL, NULL);
		int status;
		while(1)
		{
			sleep(1);
			waitpid( traced_process, &status, WNOHANG);
			if( WIFEXITED(status) )	break;
			ptrace(PTRACE_GETREGS, traced_process, NULL, &regs);
			ins = ptrace(PTRACE_PEEKTEXT, traced_process, regs.eip, NULL);
			printf("EIP: %lx Instruction executed: %lx\n", regs.eip, ins);
			ptrace(PTRACE_DETACH, traced_process, NULL, NULL);
		}
	}

	return 0;
}

#endif /* #if 0 */
/* end file */
