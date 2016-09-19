/*
 * useTime.h
 *
 *  Created on: 2016-3-10
 *      Author: hongjianan
 */

#ifndef _USE_TIME_H_
#define _USE_TIME_H_

#include <stdio.h>
#include <string.h>
#include <sys/time.h>

static inline void StartCntTime(struct timeval *beginTime)
{
	if (gettimeofday(beginTime, NULL))
	{
		perror("gettimeofday() failed");
	}
}

/* return us */
static inline int EndCntTime(struct timeval *beginTime)
{
	struct timeval endTime;
	if (gettimeofday(&endTime, NULL))
	{
		perror("gettimeofday() failed");
	}

	return (1000 * 1000 * (endTime.tv_sec - beginTime->tv_sec) + (endTime.tv_usec - beginTime->tv_usec));
}


#endif /* _USE_TIME_H_ */
