/*
 * fcntlTest.cpp
 *
 *  Created on: Jan 2, 2015
 *      Author: hong
 */
#include "../config.h"
#if FCNTLTEST_CPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#define STDIN		0
#define STDOUT		1
#define STDERR		2

void HandlerRead(int signo)
{
	char textBuf[218] = {0};
	int r;
	r = read(STDIN, textBuf, sizeof(textBuf)-1);
	textBuf[r] = '\0';
	printf("::%s", textBuf);
}

int FcntlTest_ASYNC(int argc, char *argv[]);

/*===============main==============*/
int FcntlTest(int argc, char *argv[])
{
	int ret;

	ret = FcntlTest_ASYNC(argc, argv);

	return  ret;
}


int FcntlTest_ASYNC(int argc, char *argv[])
{
	fcntl(STDIN, F_SETFL, O_ASYNC);
	fcntl(STDIN, F_SETOWN, getpid());
	signal(SIGIO, HandlerRead);

	int cnt = 0;
	while (1)
	{
		cnt++;
		if (0 == (cnt % 5))
		{
			printf("getpid()=%d\n", getpid());
		}
		sleep(1);
	}

	return 0;
}

#endif /* FCNTLTEST_CPP */
/* end file */
