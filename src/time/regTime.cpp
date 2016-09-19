/*
 * regTime.cpp
 *
 *  Created on: 2015-8-19
 *      Author: hongjianan
 */
#include "../config.h"
#if REGTIME_CPP

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


static Uint64 CPU_TIMESTAMP(void)
{
#if defined(__i386__)
    Uint64 ret;
    __asm__ volatile ("rdtsc" : "=A" (ret) );
    return ret;
#elif defined(__x86_64__) || defined(__amd64__)
    Uint64 low, high;
    __asm__ volatile ("rdtsc" : "=a" (low), "=d" (high));
    return (high << 32) | low;
#elif defined(__powerpc64__) || defined(__ppc64__)
    Uint64 tb;
    __asm__ volatile (\
        "mfspr %0, 268"
        : "=r" (tb));
    return tb;
#elif defined(__powerpc__) || defined(__ppc__)
    // This returns a time-base, which is not always precisely a cycle-count.
    Uint32 tbu, tbl, tmp;
    __asm__ volatile (\
        "0:\n"
        "mftbu %0\n"
        "mftbl %1\n"
        "mftbu %2\n"
        "cmpw %0, %2\n"
        "bne- 0b"
        : "=r" (tbu), "=r" (tbl), "=r" (tmp));
    return (((Uint64) tbu << 32) | tbl);
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (Uint64)tv.tv_sec * UINT64_C(1000000) + (Uint64)tv.tv_usec;
#endif
}

void ModuleBeginTimeUS(Uint64 *beginTime)
{
    *beginTime = CPU_TIMESTAMP();
}

Uint64 ModuleUseTimeUS(Uint64 beginTime)
{
    Uint64 endTime = CPU_TIMESTAMP();

    return ((endTime - beginTime) / 1600 * 38);
}


#endif /* REGTIME_CPP */
/* end file */
