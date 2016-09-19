/*
 * EnvTest.cpp
 *
 *  Created on: 2014-11-15
 *      Author: hongjianan
 */
#include "../config.h"
#if ENVTEST_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int EnvTest_Getenv(int argc, char* argv[]);

int EnvTest(int argc, char* argv[])
{
	int ret;

	ret = EnvTest_Getenv( argc, argv);

	return ret;
}

int EnvTest_Getenv(int argc, char* argv[])
{
	char *envIperf = NULL;
	envIperf = getenv("IPERF_VALIDATION");
	printf("get env is %s\n", envIperf);
	return 0;
}

#endif /* ENVTEST_CPP */
/* end file */
