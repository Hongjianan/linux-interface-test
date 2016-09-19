/*
 * forkTest.cpp
 *
 *  Created on: 2014-10-30
 *      Author: hong
 */
#include "../config.h"
#if FORKTEST_CPP

#include <iostream>
#include <stdio.h>
#include <unistd.h>

int gValue = 1;

static int ForkTest_Value(int argc, char *argv[]);
//static int ForkTest_2(int argc, char *argv[]);

/*===============main===============*/
int ForkTest(int argc, char *argv[])
{
	int ret;

	ret = ForkTest_Value(argc, argv);
//	ret = ForkTest_2(argc, argv);

	return ret;
}

int ForkTest_Value(int argc, char *argv[])
{
	pid_t child;
	gValue = 10;

	child = fork();
	if (child < 0)
	{
		std::cout<< "fork fail."<< std::endl;
	}
	if (0 == child)
	{
		printf("child 1: value[%d]\n", gValue);
		gValue = 20;
		sleep(2);
		printf("child 2: value[%d]\n", gValue);
	}
	else
	{
		sleep(1);
		printf("father 1: value[%d]\n", gValue);
		gValue = 30;
	}

	return 0;
}

int ForkTest_2( int argc, char *argv[] )
{
	pid_t child;
	std::cout<< "father pid is "<< getpid()<< std::endl;
	std::cout<< "times ppid ---> pid ---> child_pid"<< std::endl;

	for( int i=0; i<2; i++)
	{
		child = fork();
		if( child < 0 )
		{
			std::cout<< "fork fail."<< std::endl;
		}
		if( 0 == child )
		{
			printf("%d Into child  %d ---> %d ---> %d\n", i, getppid(), getpid(), child);

		}
		else
		{
			printf("%d Into father %d ---> %d ---> %d\n", i, getppid(), getpid(), child);
		}
	}
	return 0;
}

#endif /* FORKTEST_CPP */
/* end file */
