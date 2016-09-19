/*
 * segfaultTest.cpp
 *
 *  Created on: 2014-12-30
 *      Author: hong
 */
#include "../config.h"
#if SEGFAULTTEST_CPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/resource.h>


void SegfaultFunc_NULLPoint(void);
void SegfaultFunc_String(void);
void SegfaultFunc_RecursionInfinite(void);

int SetCoreFile(void);


int SegfaultTest_CoreDump(int argc, char *argv[]);

int tmp_02(int a)
{
	printf("tmp_02() %d\n", a);
	return 0;
}

int tmp_03(void)
{
	printf("tmp_03()\n");
	return 0;
}

void tmp_01(void)
{
	printf("tmp_01()\n");
	tmp_02(10);
	tmp_03();
}

void tmp_04(void)
{
	SegfaultFunc_NULLPoint();
}

/*=================main===============*/
int SegfaultTest(int argc, char *argv[])
{
	int ret;

	ret = SegfaultTest_CoreDump(argc, argv);

	return ret;
}


int SegfaultTest_CoreDump(int argc, char *argv[])
{
	(void)SetCoreFile();
	SegfaultFunc_NULLPoint();
//	SegfaultFunc_String();
//	SegfaultFunc_RecursionInfinite();

	return 0;
}

int SetCoreFile(void)
{
	/* set core size */
	struct rlimit res;
	if (-1 == getrlimit(RLIMIT_CORE, &res))
	{
		perror("getrlimit fail");
		return -1;
	}
	res.rlim_cur = res.rlim_max;
	if (-1 == setrlimit(RLIMIT_CORE, &res))
	{
		perror("setrlimit fail");
		return -2;
	}
	/* set core position */
	char cmd[128] = "echo \"/home/core-%e-%p-%t\" > /proc/sys/kernel/core_pattern";

	if (-1 == system(cmd))
	{
		perror("system fail");
		return -3;
	}

	return 0;
}

void SegfaultFunc_NULLPoint(void)
{
	int *p = NULL;
	*p = 100;
}

void SegfaultFunc_String(void)
{
	char *pszText = "Hello World.\n";
	*pszText = 'H';
}

void SegfaultFunc_RecursionInfinite(void)
{
	static int loop = 0;
	printf("%d\n", loop++);
	SegfaultFunc_RecursionInfinite();
}

#endif /* SEGFAULTTEST_CPP */
/* end file */
