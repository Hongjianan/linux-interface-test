/*
 * msgQueueUnlink.cpp
 *
 *  Created on: 2015-4-10
 *      Author: hongjianan
 */
#include "../config.h"
#if MSGQUEUEUNLINK_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <error.h>

int MsgQueueUnlink_mq_unlink(int argc, char* argv[]);

int MsgQueueUnlink(int argc, char* argv[])
{
	int ret;

	ret = MsgQueueUnlink_mq_unlink( argc, argv);

	return ret;
}

int MsgQueueUnlink_mq_unlink(int argc, char* argv[])
{
	mqd_t queId = 0;
	char name[100] = "/eeeeeee";
	if(-1 != (queId = mq_open(name, O_RDONLY)))    /* 此消息队列已经存在 */
	{
		printf("queId = 0x%08X\n", queId);
		if(-1 == mq_close(queId))
		{
			perror("mq_close fail");
		}

		if(-1 == mq_unlink(name))
		{
			perror("mq_unlink fail");
		}
	}

	return 0;
}


#endif /* MSGQUEUEUNLINK_CPP */
/* end file */
