/*
 * sleepTest.cpp
 *
 *  Created on: 2014-10-31
 *      Author: hong
 */
#include "../config.h"
#if SLEEPTEST_CPP

#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <sys/wait.h>

int SleepTest_1( int argc, char* argv[]);

int SleepTest( int argc, char* argv[])
{
	int ret;
	ret = SleepTest_1( argc, argv);
	return 0;
}

int SleepTest_1( int argc, char* argv[])
{
	static int sumTime = 0;
	while(1)
	{
		usleep(1000);
		++sumTime;
		printf( "time is %dhour, %dminite\n", sumTime/3600, (sumTime-sumTime/3600*3600)/60 );
	}
	return 0;
}


#endif /* SLEEPTEST_CPP */
/* end file */
