/*
 * signalTest.cpp
 *
 *  Created on: 2014-11-3
 *      Author: hong
 */
#include "../config.h"
#if SIGNALTEST_CPP

#include <stdio.h>
#include <stdlib.h>	/* atoi() abort() */
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/resource.h>


void SigHandler(int signo, siginfo_t *info, void *nothing);
void SigHandler_SIGINT(int signo);
void RaiseSigToSys(int signo);

int SignalTest_signal(int argc, char* argv[]);
int SignalTest_SendSignal_0(int argc, char* argv[]);
int SignalTest_abort(int argc, char* argv[]);

int SignalTest_SigSendAndReceive(int argc, char* argv[]);
int SignalTest_SendSignal(int argc, char* argv[]);

/*================main==============*/
int SignalTest(int argc, char* argv[])
{
	int ret;

	ret = SignalTest_signal(argc, argv);
//	ret = SignalTest_SendSignal_0(argc, argv);
//	ret = SignalTest_abort(argc, argv);
//	ret = SignalTest_SigSendAndReceive(argc, argv);
//	ret = SignalTest_SendSignal(argc, argv);

	return ret;
}


int SignalTest_signal(int argc, char* argv[])
{
	printf("SignalTest_signal ...\n");

	if (SIG_ERR == signal(SIGINT, SigHandler_SIGINT))
	{
		perror("signal SIGINT fail"), exit(-1);
	}

	while (1)
	{
		sleep(60);
	}

	return 0;
}


int SignalTest_SendSignal_0(int argc, char* argv[])
{
	if(2 != argc)
	{
		printf("usage: [pid]\n"), exit(-1);
	}

	int ret;
	ret = kill(atoi(argv[1]), 1);

	if(0 == ret)
	{
		printf("kill signal send success, pid:%s is exist.\n", argv[1]);
	}
	else if (-1 == ret)
	{
		printf("kill signal send fail, pid:%s is not exist.\n", argv[1]);
	}

	return 0;
}


int SignalTest_abort(int argc, char* argv[])
{
	struct rlimit res;

	res.rlim_max = RLIM_INFINITY;
	res.rlim_cur = RLIM_INFINITY;

	if (-1 == setrlimit(RLIMIT_CORE, &res))
	{
		perror("setrlimit fail");
	}

	abort();

	return 0;
}


int SignalTest_SigSendAndReceive(int argc, char* argv[])
{
	if(argc != 2)
	{
		printf("usage: [signo]\n"), exit(-1);
	}

	int signo;
	signo = atoi( argv[1] );
	struct sigaction act;
	sigemptyset( &act.sa_mask );
	act.sa_flags = SA_SIGINFO;
	act.sa_sigaction = SigHandler;

	union sigval mysigval;
	char data[10]="123456789";
	mysigval.sival_ptr = data;

	if( sigaction( signo, &act, NULL)<0 )
	{
		printf("install signal error.\n");
	}

	/* send signal to specified process */
	while(1)
	{
		printf("wait for the signal.\n");
		sigqueue(getpid(), signo, mysigval);
		sleep(1);
	}

	return 0;
}


int SignalTest_SendSignal(int argc, char* argv[])
{
	if(argc != 3)
	{
		printf("usage: [pid][signo]\n"), exit(-1);
	}

	int pid;
	pid = atoi(argv[1]);

	int signo;
	signo = atoi(argv[2]);
	struct sigaction act;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_SIGINFO;
	act.sa_sigaction = SigHandler;

	union sigval mysigval;
	char data[10]="123456789";
	mysigval.sival_ptr = data;

	if (sigaction(signo, &act, NULL) < 0)
	{
		printf("install signal error.\n");
	}

	/* send signal to specified process */

	sigqueue(pid, signo, mysigval);

	return 0;
}


void SigHandler(int signo, siginfo_t *info, void *nothing)
{
	printf("receive signal number is %d\n", signo);
}

void SigHandler_SIGINT(int signo)
{
	printf("SigHandler_SIGINT = %d\n", signo);

	RaiseSigToSys(signo);
}

void RaiseSigToSys(int signo)
{
	if (SIG_ERR == signal(signo, SIG_DFL))
	{
		perror("signal SIGINT fail"), exit(-1);
	}
	raise(signo);
}

#endif /* SIGNALTEST_CPP */
/* end file */
