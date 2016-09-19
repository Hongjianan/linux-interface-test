/*
 * prctlTest.cpp
 *
 *  Created on: 2015Äê4ÔÂ8ÈÕ
 *      Author: Hong
 */

#include "../config.h"
#if PRCTLTEST_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/prctl.h>
#include <errno.h>


int PrctlTest_PR_SET_NAME(int argc, char *argv[]);

/*==============main===============*/
int PrctlTest(int argc, char *argv[])
{
	int ret;

	ret = PrctlTest_PR_SET_NAME(argc, argv);

	return ret;
}


int PrctlTest_PR_SET_NAME(int argc, char *argv[])
{
	if (0 != prctl(PR_SET_NAME, "hong2", NULL, NULL, NULL))
	{
		perror("prctl PR_SET_NAME fail");
		exit(errno);
	}

	getchar();

	return 0;
}


#endif /* FIFOTEST_CPP */
/* end file */

