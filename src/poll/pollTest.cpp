/*
 * pollTest.cpp
 *
 *  
 *
 *  Created on: 2016-3-29
 *      Author: Hong
 */

#include "../config.h"
#if POLL_TEST_CPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <sys/time.h>
#include <sys/types.h>

static int PollTest_IO(int argc, char *argv[]);

/*===============main===============*/
int PollTest(int argc, char *argv[])
{
	int ret;

	ret = PollTest_IO(argc, argv);

	return ret;
}

static int PollTest_IO(int argc, char *argv[])
{
	int fdNum = 0;

	/* get max fd */
	int inFd = fileno(stdin);
	int outFd = fileno(stdout);
	fdNum = 2;

	struct pollfd pollArray[2];
	bzero(pollArray, sizeof(pollArray));

	pollArray[0].fd = inFd;
	pollArray[0].events = POLLIN | POLLOUT;

	pollArray[1].fd = outFd;
	pollArray[1].events = POLLIN | POLLOUT;

	int ret;
	while (1)
	{
		ret = poll(pollArray, fdNum, 0);	/* block until i/o */
		if (ret > 0)
		{
			int idxFd;
			for (idxFd = 0; idxFd < 2; ++idxFd)
			{
				if (pollArray[idxFd].revents & POLLIN)
				{
					fprintf(stderr, "fd[%d] is POLLIN.\n", pollArray[idxFd].fd);
				}
				if (pollArray[idxFd].revents & POLLOUT)
				{
					fprintf(stderr, "fd[%d] is POLLOUT.\n", pollArray[idxFd].fd);
				}
			}
		}
		sleep(1);
	}

	return 0;
}

#endif	/* POLL_TEST_CPP */
/* end of file */
