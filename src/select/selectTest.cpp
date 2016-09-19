/*
 * selectTest.cpp
 *
 *  
 *
 *  Created on: 2016-3-27
 *      Author: Hong
 */
#include "../config.h"
#if SELECT_TEST_CPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>

static int SelectTest_IO(int argc, char *argv[]);

/*===============main===============*/
int SelectTest(int argc, char *argv[])
{
	int ret;

	ret = SelectTest_IO(argc, argv);

	return ret;
}

static int SelectTest_IO(int argc, char *argv[])
{
	int maxFd = 0;

	/* get max fd */
	int inFd = fileno(stdin);
	int outFd = fileno(stdout);

	if (inFd > maxFd)
		maxFd = inFd;
	if (outFd > maxFd)
		maxFd = outFd;

	maxFd++;

	/* set fd_set */
	struct fd_set inSet, outSet;

	char buf[1024];
	int fdNum;
	while (1)
	{
		FD_ZERO(&inSet);
		FD_ZERO(&outSet);

		FD_SET(inFd, &inSet);
	//	FD_SET(inFd, &outSet);

	//	FD_SET(outFd, &inSet);
	//	FD_SET(outFd, &outSet);

		fdNum = select(maxFd, &inSet, NULL, NULL, NULL);	/* block until i/o */
		if (fdNum)
		{
			fprintf(stderr, "select fd num is %d.\n", fdNum);
			if (FD_ISSET(inFd, &inSet))
			{
				int readLen;
				fprintf(stderr, "stdin is in inSet.\n");
				if ((readLen = read(inFd, buf, sizeof(buf))) > 0)
				{
					buf[readLen] = '\0';
					fprintf(stderr, "input:%s\n", buf);
				}
			}
//			if (FD_ISSET(outFd, &inSet))
//			{
//				fprintf(stderr, "outFd is in inSet.\n");
//			}

//			if (FD_ISSET(inFd, &outSet))
//			{
//				fprintf(stderr, "stdin is in outSet.\n");
//			}
//
//			if (FD_ISSET(outFd, &outSet))
//			{
//				fprintf(stderr, "outFd is in outSet.\n");
//			}
		}

		sleep(1);
	}

	return 0;
}

#endif	/* SELECT_TEST_CPP */
/* end of file */
