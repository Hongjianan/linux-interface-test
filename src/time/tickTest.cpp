/*
 * clockTest.cpp
 *
 *  Created on: 2015-6-15
 *      Author: hongjianan
 */
/*
 * conclusion:
 * 		1.	real time:
 * 			sys  time:
 * 			user time:
 * 		2. times(): 返回的是真实流逝时间。
 * 					指针入参返回的分配的CPU时间片的个数，每一片的时间为10ms.但是不一定每一片都能用完,或者会被其他程序抢占。
 *
 */
#include "../config.h"
#if TICKTEST_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>	/* gettimeofday()  getrusage() */
#include <sys/times.h>	/* times() */
#include <time.h>		/* clock() */
#include <sys/resource.h>
#include <error.h>

int ticksPerSec = 0;
timeval startVal, endVal;


int TickTest_clock(int argc, char *argv[]);
int TickTest_times(int argc, char *argv[]);
int TickTest_getrusage(int argc, char *argv[]);


/*==============main==============*/
int TickTest(int argc, char *argv[])
{
	ticksPerSec = sysconf(_SC_CLK_TCK);
	printf("CLOCKS_PER_SEC = %d\n", CLOCKS_PER_SEC);
	printf("_SC_CLK_TCK = %d\n", ticksPerSec);

	int ret;

	ret = TickTest_clock(argc, argv);
//	ret = TickTest_times(argc, argv);

	return ret;
}


int TickTest_clock(int argc, char *argv[])
{
	clock_t startTick  = 0, endTick = 0;
	clock_t startClock = 0, endClock = 0;

	startClock = clock();
	startTick  = times(NULL);
	gettimeofday(&startVal, NULL);

	for (int i = 0; i < 100000000; ++i);
	sleep(1);

	endClock = clock();
	endTick  = times(NULL);
	gettimeofday(&endVal, NULL);

	printf("<1> clock    [start:%d end:%d dela:%d]\n", startClock, endClock, endClock - startClock);
	printf("<1> clock    [start:%d end:%d dela:%d]\n", startTick, endTick, endTick - startTick);
	printf("<2> gettimeofday: use %ds %dus\n",
			endVal.tv_sec - startVal.tv_sec, endVal.tv_usec - startVal.tv_usec);

	return 0;
}


int TickTest_times(int argc, char *argv[])
{
	struct tms	startTms,	endTms;
	clock_t 	startTick,	endTick;

	startTick = times(&startTms);
	gettimeofday(&startVal, NULL);

	for (int i = 0; i < 100000000; ++i);
	sleep(1);

	endTick = times(&endTms);
	gettimeofday(&endVal, NULL);

	printf("<1> times [start:%d end:%d dela:%d] use %lfs\n",
			startTick, endTick, endTick - startTick, (double)(endTick - startTick) / ticksPerSec);

	printf("    times user  time [start:%d end:%d dela:%d] use:[%dms]\n",
			startTms.tms_utime, endTms.tms_utime, endTms.tms_utime - startTms.tms_utime, (endTms.tms_utime - startTms.tms_utime) * (1000 / ticksPerSec));

	printf("    times sys   time [start:%d end:%d dela:%d] use:[%dms]\n",
			startTms.tms_stime, endTms.tms_stime, endTms.tms_stime - startTms.tms_stime, (endTms.tms_stime - startTms.tms_stime) * (1000 / ticksPerSec));

	printf("    times cuser time [start:%d end:%d dela:%d] use:[%dms]\n",
			startTms.tms_cutime, endTms.tms_cutime, endTms.tms_cutime - startTms.tms_cutime, (endTms.tms_cutime - startTms.tms_cutime) * (1000 / ticksPerSec));

	printf("    times csys  time [start:%d end:%d dela:%d] use:[%dms]\n",
			startTms.tms_cstime, endTms.tms_cstime, endTms.tms_cstime - startTms.tms_cstime, (endTms.tms_cstime - startTms.tms_cstime) * (1000 / ticksPerSec));

	printf("<2> gettimeofday: use %ds %dus\n",
			endVal.tv_sec - startVal.tv_sec, endVal.tv_usec - startVal.tv_usec);

	return 0;
}


#endif /* SPINLOCK_CPP */
/* end file */
