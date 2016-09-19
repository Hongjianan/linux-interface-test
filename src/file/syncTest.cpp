/*
 * syncTest.cpp
 *
 *  Created on: 2015-2-20
 *      Author: Administrator
 */
#include "../config.h"
#if SYNCTEST_CPP

#include "../common/baseTypedef.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>	/* sync */
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int SyncTest_sync_fsync_flock(int argc, char *argv[]);

/*==========main========*/
int SyncTest(int argc, char *argv[])
{
	int ret;

	ret = SyncTest_sync_fsync_flock(argc, argv);

	return ret;
}

#define CNT 5
int SyncTest_sync_fsync_flock(int argc, char *argv[])
{
	char filename[] = "/home/tmp.txt";

	int fd;
	if (-1 == (fd = open(filename, O_RDWR| O_CREAT, 0644)))
	{
		printf("open \"%s\" error:%s", filename, strerror(errno));
		exit(errno);
	}
	printf("open \"%s\" success\n", filename);

	if (-1 == flock(fd, LOCK_EX))
	{
		printf("flock \"%s\" error:%s", filename, strerror(errno));
		exit(errno);
	}
	printf("flock \"%s\" success\n", filename);

	char buffer[512] = {0};
	for (int i = 0; i < CNT; ++i)
	{
		sprintf(buffer, "[%03d]%s\n", i, filename);
		if (-1 == write(fd, buffer, strlen(buffer)))
		{
			printf("write \"%s\" error:%s", filename, strerror(errno));
			exit(errno);
		}
		sleep(1);
	}

	sync();
	printf("sync all open file success\n");

	if (-1 == fsync(fd))
	{
		printf("fsync \"%s\" error:%s", filename, strerror(errno));
		exit(errno);
	}
	printf("fsync \"%s\" file success\n", filename);

	if (-1 == flock(fd, LOCK_UN))
	{
		printf("flock \"%s\" error:%s", filename, strerror(errno));
		exit(errno);
	}
	printf("flock \"%s\" success\n", filename);

	close(fd);

	return 0;
}
#endif /* SYNCTEST_CPP */
/* end file */
