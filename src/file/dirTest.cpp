/*
 * dirTest.cpp
 *
 *  Created on: 2015-2-19
 *      Author: Hong
 */
#include "../config.h"
#if DIRTEST_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>


int DirTest_mkdir_rmdir(int argc, char *argv[]);
int DirTest_opendir(int argc, char *argv[]);
int DirTest_getcwd(int argc, char *argv[]);
/*=================main==============*/
int DirTest(int argc, char *argv[])
{
	int ret;

//	ret = DirTest_mkdir_rmdir(argc, argv);
//	ret = DirTest_opendir(argc, argv);
	ret = DirTest_getcwd(argc, argv);

	return ret;
}


int DirTest_mkdir_rmdir(int argc, char *argv[])
{
	if (3 != argc)
	{
		printf("usage: %s [path] [isRemove<0,1>]\n", argv[0]);
		exit(-1);
	}

	char path[128] = {0};
	strcpy(path, argv[1]);

	if (mkdir(path, 0644))
	{
		perror("mkdir error");
		return errno;
	}
	printf("mkdir [%s] success\n", path);

	if (1 == atoi(argv[2]))
	{
		if (rmdir(path))
		{
			perror("rmdir error");
			return errno;
		}
		printf("rmdir [%s] success\n", path);
	}

	return 0;
}

int DirTest_opendir(int argc, char *argv[])
{
	if (2 != argc)
	{
		printf("usage: %s [path]\n", argv[0]);
		exit(-1);
	}

	char dirPath[128] = {0};
	strcpy(dirPath, argv[1]);

	DIR *pDir;
	struct dirent *pDirData;
	pDir = opendir(dirPath);
	if (NULL == pDir)
	{
		perror("opendir error.");
		return -1;
	}

	int loop;
	for (loop = 0; pDirData = readdir(pDir); ++loop)
	{
		printf("[%02u] %s\n", loop, pDirData->d_name);
	}

	closedir(pDir);

	return 0;
}

int DirTest_getcwd(int argc, char *argv[])
{
	char curPath[128];
	if (NULL == getcwd(curPath, sizeof(curPath)))
	{
		perror("getcwd() fail");	exit(-1);
	}

	printf("current work directory is [%s]\n", curPath);
	return 0;
}

#endif /* DIRTEST_CPP */
/* end file */
