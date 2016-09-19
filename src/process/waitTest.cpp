/*
 * waitTest.cpp
 *
 *  Created on: 2014-10-30
 *      Author: hong
 */
#include "../config.h"
#if WAITTEST_CPP

#include "include/includeForkTest.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

int WaitTest_1( int argc, char *argv[] );
int WaitTest_Sleep( int argc, char *argv[] );
int WaitTest_WIfExited_And_WExitStatus( int argc, char *argv[] );
int WaitTest_Waitpid( int argc, char *argv[] );

int WaitTest( int argc, char *argv[] )
{
	int ret;

//	ret = WaitTest_1( argc, argv);
//	ret = WaitTest_Sleep( argc, argv);
	ret = WaitTest_WIfExited_And_WExitStatus( argc, argv);
//	ret = WaitTest_Waitpid( argc, argv);

	return ret;
}

int WaitTest_1( int argc, char *argv[] )
{
	int status;
	pid_t child;
	int ret;
	child = fork();
	if( child < 0 )
	{
		std::cout<< "fork fail"<<std::endl;
		exit(-1);
	}
	if( 0 == child )	/* into child process */
	{
		printf("child pid is %d\n", getpid() );
		execl("/bin/ls", "ls", NULL);
		printf("child pid is %d\n", getpid() );
	}
	else	/* into father process */
	{
		printf("father pid is %d\n", getpid() );
		ret = wait( &status );
		if( -1 == ret )
		{
			std::cout<< "father process have no child process."<<std::endl;
			exit(-1);
		}
	}
	return 0;
}

int WaitTest_Sleep( int argc, char *argv[] )
{
	pid_t child;
	child = fork();
	if( child<0 )
	{
		printf("fork fail!\n");
		exit(-1);
	}

	if( 0==child )
	{
		printf("This is child process with pid of %d\n",getpid());
		sleep(2);
	}else
	{
		pid_t exitPid;
		exitPid = wait(NULL);
		printf("I catched a child process with pid of %d\n",exitPid);
	}
	return 0;
}

int WaitTest_WIfExited_And_WExitStatus( int argc, char *argv[] )
{
	if( argc<2 )
	{
		printf("usage:[child ret num]\n"),exit(-1);
	}
	int retnum = atoi( argv[1] );
	int status;
	pid_t childPid,exitPid;
	childPid = fork();
	if( childPid<0 )
	{
		printf("error ocurred!\n");
		exit(-1);
	}

	if( childPid==0 )
	{
		printf("This is child process with pid of %d.\n",getpid());
		sleep( 10 );
		exit( retnum );
	}
	else
	{
		while( 1 )
		{
			exitPid = waitpid( childPid, &status, 0);

			if( exitPid == childPid )	/* child process exit */
			{
				if( WIFEXITED(status) )
				{
					printf("the child process %d exit normally.\n",exitPid);
					printf("the return code is %d.\n",WEXITSTATUS(status));
					break;
				}

				if( WIFSIGNALED(status) )
				{
					printf("the child process %d exit abnormally.\n",exitPid);
					printf("signal is %d\n", WTERMSIG(status) );
					printf("abnormally exit status is : %p\n", status);
					break;
				}
			}
		}/* while(1) */
		printf("exit father process\n");
	}
	return 0;
}

int WaitTest_Waitpid( int argc, char *argv[] )
{
	pid_t child, exitPid;
	child = fork();
	if( child<0 )
		printf("Error occured on forking.\n");
	else if(child==0)
	{
		sleep(5);
		exit(0);
	}
	do
	{
		exitPid = waitpid(child, NULL, WNOHANG);
		if( exitPid == 0 )
		{
			printf("No child exited\n");
			sleep(1);
		}
	}while(exitPid==0);
	if(exitPid==child)
		printf("successfully get child %d\n", exitPid);
	else
		printf("some error occured\n");
	return 0;
}

#endif/* WAITTEST_CPP */
/* end file */
