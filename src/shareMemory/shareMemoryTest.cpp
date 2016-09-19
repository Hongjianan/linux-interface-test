/*
 * shareMemoryTest.cpp
 *
 *  Created on: 2014-11-6
 *      Author: hong
 */
#include "../config.h"
#if SHAREMEMORYTEST_CPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

int ShmOpen(const char *shmName, int shmFlag, unsigned int shmSize);


int ShareMemoryTest_Open(int argc, char* argv[]);
int ShareMemoryTest_Write(int argc, char* argv[]);
int ShareMemoryTest_Read(int argc, char* argv[]);

/* two process */
/*================main==================*/
int ShareMemoryTest(int argc, char* argv[])
{
	int ret = 0;

//	ret = ShareMemoryTest_Open(argc, argv);
	ret = ShareMemoryTest_Write(argc, argv);
//	ret = ShareMemoryTest_Read(argc, argv);

	return ret;
}

int ShareMemoryTest_Open(int argc, char* argv[])
{
	if (3 != argc)
	{
		printf("usage:<Share Memory Name> <Memory Size>\n"), exit(-1);
	}
	const char *shmName = argv[1];
	unsigned int shmSize = atoi(argv[2]);

	if (-1 == ShmOpen(shmName, O_CREAT| O_RDWR, shmSize))
	{
		printf("ShmOpen fail.\n");
	}

	return 0;
}

int ShareMemoryTest_Write(int argc, char* argv[])
{
	if (3 != argc)
	{
		printf("usage:<Share Memory Name> <Memory Size>\n"), exit(-1);
	}
	const char *shmName = argv[1];
	unsigned int shmSize = atoi(argv[2]);

	/* map share memory to process memory */
	int shmFd = ShmOpen(shmName, O_CREAT| O_RDWR, shmSize);
	if (-1 == shmFd)
	{
		printf("ShmOpen fail.\n");
		exit(-1);
	}

	void *mapAddr = NULL;
	mapAddr = mmap(NULL, shmSize, PROT_WRITE, MAP_SHARED, shmFd, 0);
	/* write data to sharm memory */
	strcpy((char*)mapAddr, "hello hongjianan\n");

	/* sleep let other process can open shm. */
	sleep(10);
	shm_unlink(shmName);

	return 0;
}

int ShareMemoryTest_Read(int argc, char* argv[])
{
	if (3 != argc)
	{
		printf("usage:<Share Memory Name> <Memory Size>\n"), exit(-1);
	}
	const char *shmName = argv[1];
	unsigned int shmSize = atoi(argv[2]);

	/* map share memory to process memory */
	int shmFd = ShmOpen(shmName, O_RDONLY, 0);
	if (-1 == shmFd)
	{
		printf("ShmOpen fail.\n");
		exit(-1);
	}

	void *mapAddr = NULL;
	mapAddr = mmap(NULL, shmSize, PROT_READ, MAP_SHARED, shmFd, 0);
	/* write data to sharm memory */
	printf("share memory data is :%s\n", (char*)mapAddr);

	return 0;
}

int ShmOpen(const char *shmName, int shmFlag, unsigned int shmSize)
{
	if ('/' != shmName[0])
	{
		printf("Share Memory Name should be begin with '/'.\n");
		return -1;
	}

	/* open share memory */
	int shmID = shm_open(shmName, shmFlag, 0644);
	if (-1 == shmID)
	{
		perror("shm_open error");
		return -1;
	}
	else
	{
		printf("shm_open success, shm is %d\n", shmID);
	}

	struct stat infoBuf;
	fstat(shmID, &infoBuf);
	printf("before ftruncate....\n");
	printf("shmID:%d stat of st_uid:%d\n",   shmID, infoBuf.st_uid);
	printf("shmID:%d stat of st_gid:%d\n",   shmID, infoBuf.st_gid);
	printf("shmID:%d stat of st_size:%ld\n", shmID, infoBuf.st_size);

	/* change memory size */
	if (shmFlag & O_CREAT)
	{
		if (-1 == ftruncate(shmID, shmSize))
		{
			perror("ftruncate error");
			return -1;
		}
		else
		{
			printf("ftruncate success, share memory size if %d\n", shmSize);
		}

		/* show memory information */
		fstat(shmID, &infoBuf);
		printf("after ftruncate....\n");
		printf("shmID:%d stat of st_uid:%d\n",   shmID, infoBuf.st_uid);
		printf("shmID:%d stat of st_gid:%d\n",   shmID, infoBuf.st_gid);
		printf("shmID:%d stat of st_size:%ld\n", shmID, infoBuf.st_size);
	}

	return shmID;
}


#endif /* SHAREMEMORYTEST_CPP */
/* end file */
