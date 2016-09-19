/*
 * recursiveTest.cpp
 *
 *  Created on: 2014-12-15
 *      Author: hong
 */
#include "../config.h"
#if RECURSIVETEST_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t gmutex;

int RecursiveTest_Recursive(int argc, char *argv[]);
/*=================main================*/
int RecursiveTest(int argc, char *argv[])
{
	int ret;

	ret = RecursiveTest_Recursive(argc, argv);

	return ret;
}

int RecursiveTest_Recursive(int argc, char *argv[])
{
	if (0 != pthread_mutex_init(&gmutex, NULL))
	{
		perror("pthread_mutex_init failed"), exit(-1);
	}
	return 0;
}
#endif/* RECURSIVETEST_CPP */
/* end file */
