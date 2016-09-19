/*
 * fileTest.cpp
 *
 *  Created on: 2015-2-20
 *      Author: Hong
 */
#include "../config.h"
#if FILETEST_CPP

#include "../common/baseTypedef.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>


Int32 FileTest_create_remove_mkstemp(Int32 argc, char *argv[]);
Int32 FileTest_open_close_write_read(Int32 argc, char *argv[]);
Int32 FileTest_lseek(Int32 argc, char *argv[]);


/*=================main===============*/
Int32 FileTest(Int32 argc, char *argv[])
{
	Int32 ret;

//	ret = FileTest_create_remove_mkstemp(argc, argv);
//	ret = FileTest_open_close_write_read(argc, argv);
	ret = FileTest_lseek(argc, argv);

	return ret;
}


Int32 FileTest_create_remove_mkstemp(Int32 argc, char *argv[])
{
	if (5 != argc)
	{
		printf("usage: %s [filename] [isRemove<0,1>] [tmp filename] [isRemove<0,1>]\n", argv[0]);
		exit(-1);
	}
	char filename[128], tmpfilename[128];
	memcpy(filename, argv[1], strlen(argv[1])>128 ? 128: (strlen(argv[1])+1));
	memcpy(tmpfilename, argv[3], strlen(argv[3])>128 ? 128: (strlen(argv[3])+1));

	Int32 fd, tmpFd;
	/* if file exist, ok. will clear origin file. */
	if (-1 == (fd = creat(filename, 0644)))
	{
		perror("create file error");
		exit(errno);
	}
	printf("create file:[%s] success\n", filename);

	if (1 == atoi(argv[2]))
	{
		if (remove(filename))
		{
			perror("remove error");
			return errno;
		}
		printf("remove [%s] success\n", filename);
	}

	if (-1 == (tmpFd = mkstemp(tmpfilename)))
	{
		perror("create file error");
		exit(errno);
	}
	printf("create file:[%s] success\n", tmpfilename);

	if (1 == atoi(argv[4]))
	{
		if (remove(tmpfilename))
		{
			perror("remove error");
			return errno;
		}
		printf("remove [%s] success\n", tmpfilename);
	}

	return 0;
}


Int32 FileTest_open_close_write_read(Int32 argc, char *argv[])
{
	if (3 != argc)
	{
		printf("usage: %s [filename]\n", argv[0]);
		exit(-1);
	}
	char filename[128];
	memcpy(filename, argv[1], strlen(argv[1])>128 ? 128: strlen(argv[1]));

	Int32 fd;
#if 1
	/* if file exist, ok. equal to open file, not create file */
	if (-1 == (fd = open(filename, O_RDWR| O_CREAT, 0644)))
#else
	/* if file exist, fail. */
	if (-1 == (fd = open(filename, O_RDWR| O_CREAT| O_EXCL, 0644)))
#endif
	{
		perror("open file error");
		exit(errno);
	}
	printf("open file:[%s] success\n", filename);

	const char *strExit = "exit";
	char rdwrBuf[512] = {0};

	for(Int32 i = 0; ; ++i)
	{
		getline();
	}

	return 0;
}

Int32 FileTest_lseek(Int32 argc, char *argv[])
{
	/* dealing with arguments */
	if (3 != argc)
	{
		printf("usage: %s [filename]\n", argv[0]);
		exit(-1);
	}
	char filename[128];
	memcpy(filename, argv[1], strlen(argv[1])>128 ? 128: strlen(argv[1]));

	Int32 fd;
	/* if file exist, ok. equal to open file, not create file */
	if (-1 == (fd = open(filename, O_RDWR| O_CREAT, 0644)))
	{
		perror("open file error");
		exit(errno);
	}
	printf("1. open file:[%s] success\n", filename);

	const char *strExit = "exit";
	char rdwrBuf[512] = {0};

	for(Int32 i = 0; ; ++i)
	{
		getline();
	}

	close(fd);

	remove(filename);

	return 0;
}

#endif /* FILETEST_CPP */
/* end file */
