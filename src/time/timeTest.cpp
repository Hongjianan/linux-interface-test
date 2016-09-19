/*
 * timeTest.cpp
 *
 *  Created on: 2015-2-19
 *      Author: Hong
 */
#include "../config.h"
#if TIMETEST_CPP

#include "../common/baseTypedef.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <memory.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>


Int32 TimeTest_time_gmtime_localtime_ctime_asctime_mktime(Int32 argc, char *argv[]);
Int32 TimeTest_gettimeofday_settimeofday(Int32 argc, char *argv[]);

/*================main================*/
Int32 TimeTest(Int32 argc, char *argv[])
{
	Int32 ret;

//	ret = TimeTest_time_gmtime_localtime_ctime_asctime_mktime(argc, argv);
	ret = TimeTest_gettimeofday_settimeofday(argc, argv);

	return ret;
}


Int32 TimeTest_time_gmtime_localtime_ctime_asctime_mktime(Int32 argc, char *argv[])
{
	time_t t1 = time(NULL);
	time_t t2;
	t2 = time(&t2);
	printf("Now: %lds from 1970-1-1\n", t1);
	printf("Now: %lds from 1970-1-1\n", t2);

	struct tm *stm;
	stm = gmtime(&t1);
	printf("tm_zone = %d\n"
			"tm_year = %d\n"
			"tm_mon = %d\n"
			"tm_mday = %d\n"
			"tm_hour = %d\n"
			"tm_min = %d\n"
			"tm_sec = %d\n"
			"tm_yday = %d\n"
			"tm_wday = %d\n"
			"tm_isdst = %d\n"
			"tm_gmtoff = %d\n", \
			stm->tm_zone, stm->tm_year + 1900, stm->tm_mon + 1, stm->tm_mday, \
			stm->tm_hour + 8, stm->tm_min, stm->tm_sec, \
			stm->tm_yday, stm->tm_wday, stm->tm_isdst, stm->tm_gmtoff);

	struct tm *stmlocal = localtime(&t1);
	printf("tm_zone = %d\n"
			"tm_year = %d\n"
			"tm_mon = %d\n"
			"tm_mday = %d\n"
			"tm_hour = %d\n"
			"tm_min = %d\n"
			"tm_sec = %d\n"
			"tm_yday = %d\n"
			"tm_wday = %d\n"
			"tm_isdst = %d\n"
			"tm_gmtoff = %d\n", \
			stmlocal->tm_zone, stmlocal->tm_year + 1900, stmlocal->tm_mon + 1, stmlocal->tm_mday, \
			stmlocal->tm_hour, stmlocal->tm_min, stmlocal->tm_sec, \
			stmlocal->tm_yday, stmlocal->tm_wday, stmlocal->tm_isdst, stmlocal->tm_gmtoff);

	printf("date: %s", ctime(&t1));
	printf("date: %s", asctime(stm));

	printf("seconds:%d\n", mktime(stm));
	printf("seconds:%d\n", mktime(stmlocal));


	return 0;
}


Int32 TimeTest_gettimeofday_settimeofday(Int32 argc, char *argv[])
{
	struct timeval startTime, endTime;
	struct timezone tz = {0};
	bzero(&startTime, sizeof(startTime));
	bzero(&endTime, sizeof(endTime));

	if (gettimeofday(&startTime, &tz))
	{
		perror("gettimeofday error");
		return errno;
	}
	printf("%lds, %ldus\nUTC diff is:%dmin\n Is summer time:%d\n",\
			startTime.tv_sec, startTime.tv_usec, tz.tz_minuteswest, tz.tz_dsttime);

	startTime.tv_sec += 300;
	if (settimeofday(&startTime, &tz))
	{
		perror("settimeofday error");
		return errno;
	}

	if (gettimeofday(&startTime, &tz))
	{
		perror("gettimeofday error");
		return errno;
	}
	printf("%lds, %ldus\n", startTime.tv_sec, startTime.tv_usec);

	startTime.tv_sec -= 300;
	if (settimeofday(&startTime, &tz))
	{
		perror("settimeofday error");
		return errno;
	}

	if (gettimeofday(&startTime, &tz))
	{
		perror("gettimeofday error");
		return errno;
	}
	printf("%lds, %ldus\n", startTime.tv_sec, startTime.tv_usec);

	return 0;
}


#endif /* TIMETEST_CPP */
/* end file */
