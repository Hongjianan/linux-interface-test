/*
 * mipsPtraceAttachTest.cpp
 *
 *  Created on: 2014-12-4
 *      Author: hong
 */
#include "../config.h"
#if MIPSWAITPIDTEST_CPP

#include <stdio.h>
#include <stdlib.h>		/* atoi() WIFEXITED() */
#include <unistd.h>
#include <sys/user.h>	/* struct user_regs_struct */
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>	/* wait() and waitpid() */
#include <sys/ptrace.h>	/* ptrace() */
#include <errno.h>


int MipsWaitpidTest_WIFEXITED(int argc, char* argv[]);
int MipsWaitpidTest_ExitAndGetRegs(int argc, char* argv[]);

/*====================main====================*/
int MipsWaitpidTest(int argc, char* argv[])
{
	int ret;

	ret = MipsWaitpidTest_WIFEXITED(argc, argv);
//	ret = MipsWaitpidTest_ExitAndGetRegs(argc, argv);

	return ret;
}

int MipsWaitpidTest_WIFEXITED(int argc, char* argv[])
{
	long ins;

	pid_t childPid;
	childPid = fork();
	if( childPid< -1 )
	{
		printf("Fork fail!\n"),exit(-1);
	}
	if( 0==childPid )	/* into child process */
	{
		ptrace( PTRACE_TRACEME, 0, NULL, NULL);
		int i;
		for( i=0; i<3; ++i)
		{
			printf("counter: %d\n", i);
			sleep(1);
		}
		return -1;
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
					printf("abnormally exit status is : %p\n", status);
					break;
				}

			}/* if( exitPid == childPid ) */
		}/* while(1) */

		printf("father process of %d pid exit.\n", getpid());
		return 0;
	}
}


int MipsWaitpidTest_ExitAndGetRegs(int argc, char* argv[])
{

	return 0;
}
#endif
/* end file */
