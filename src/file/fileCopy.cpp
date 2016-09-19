/*
 * fileCopy.cpp
 *
 *  Created on: 2015-2-20
 *      Author: Administrator
 */
#include "../config.h"
#if FILECOPY_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>


int FileCopy(int argc, char *argv[])
{
	if (3 != argc)
	{
		printf("usage: %s [file] [new file]\n", argv[0]);
		exit(-1);
	}

	int fdOrigin, fdNew;
	if (-1 == (fdOrigin = open(argv[1], O_RDONLY)))
	{
		printf("open \"%s\" error:%s", argv[1], strerror(errno));
		exit(errno);
	}

	if (-1 == (fdNew = open(argv[2], O_WRONLY| O_CREAT| O_EXCL)))
	{
		printf("open \"%s\" error:%s", argv[2], strerror(errno));
		exit(errno);
	}

	int lengthRead, lengthWritten;
	char buffer[512];
	printf("start copy\n");
	for (int i = 0; ; ++i)
	{
		if (-1 == (lengthRead = read(fdOrigin, buffer, sizeof(buffer)-1)))
		{
			printf("read \"%s\" error:%s", argv[1], strerror(errno));
			exit(errno);
		}
		buffer[lengthRead] = '\0';

		if (0 != lengthRead)
		{
			if (-1 == (lengthWritten = write(fdNew, buffer, lengthRead)))
			{
				printf("write \"%s\" error:%s", argv[2], strerror(errno));
				exit(errno);
			}
		}
		else
		{
			break;
		}
	}

	printf("end copy\n");

	return 0;
}

#endif /* FILECOPY_CPP */
/* end file */
