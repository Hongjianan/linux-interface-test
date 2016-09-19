/*
 * systemTest.cpp
 *
 *  Created on: 2015-8-26
 *      Author: hongjianan
 */
#include "../config.h"
#if SYSTEMTEST_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int SystemTest_system(int argc, char *argv[]);
int SystemTest_wait(int argc, char *argv[]);

/*==============main==============*/
int SystemTest(int argc, char *argv[])
{
	int ret;

//	ret = SystemTest_system(argc, argv);
	ret = SystemTest_wait(argc, argv);

	return ret;
}


int SystemTest_system(int argc, char *argv[])
{
	printf("pid [%d]\n", getpid());	fflush(stdout);
	sleep(10);

	int status = 0;

	status = system("echo xxx > hong");
	printf("echo xxx > hong: [status=%d]\n", status);	fflush(stdout);
	sleep(2);

	status = system("./right.sh");
	printf("./right.sh: [status=%d]\n", status);	fflush(stdout);
	sleep(2);

	status = system("./wrong.sh");
	printf("./wrong.sh: [status=%d]\n", status);	fflush(stdout);
	sleep(2);

	return 0;
}

int SystemTest_wait(int argc, char *argv[])
{
	printf("pid [%d]\n", getpid());	fflush(stdout);

	int status = 0;

	printf("before call system(sleep 5)");	fflush(stdout);
	status = system("sleep 5");
	printf("after call system(sleep 5) [status=%d]\n", status);	fflush(stdout);

	return 0;
}

#endif /* SYSTEMTEST_CPP */
/* end file */
