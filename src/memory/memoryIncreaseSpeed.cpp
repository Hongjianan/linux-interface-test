/*
 * memoryIncreaseSpeed.cpp
 *
 *  Created on: 2015-5-21
 *      Author: hongjianan
 */

/*
 * 	Ŀ�ģ�
 * 		��linux������ ����status��VmRSS��¼���ڴ�ʹ������Ƿ��ǰ��� ҳ�Ĵ�С��4KB��Ϊ��С��λ�����ģ�
 *
 * 	���ۣ�
 * 		һ���������ǵģ����ǲ���������΢�е�ƫ����һ��������
 *
 */
#include "../config.h"
#if MEMORYINCREASESPEED_CPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>


int MemoryIncreaseSpeed_Heap(int argc, char *argv[]);
int MemoryIncreaseSpeed_Stack(int argc, char *argv[]);

/*================main==============*/
int MemoryIncreaseSpeed(int argc, char *argv[])
{
	int ret;

	ret = MemoryIncreaseSpeed_Heap(argc, argv);

	return ret;
}


int MemoryIncreaseSpeed_Heap(int argc, char *argv[])
{
	if (3 != argc)
	{
		printf("usage:%s [per memory size byte] [sleep time ms]\n", argv[0]);
		exit(-1);
	}

	int sizeByte = atoi(argv[1]);
	int sleepTimeMs = atoi(argv[2]);
	int pid = getpid();
	printf("pid is %d\n", pid);

	void *pTmp = NULL;
	char cmdLine[128] = {0};
	for (int i = 0; i < 1000; ++i)
	{
		pTmp = malloc(sizeByte);
		if (NULL == pTmp)
		{
			perror("malloc fail");
			continue;
		}
		bzero(pTmp, sizeByte);
		sprintf(cmdLine, "echo [%03d]; grep VmRSS /proc/%d/status", i, pid);
		system(cmdLine);

		usleep(1000 * sleepTimeMs);
	}

	return 0;
}


int MemoryIncreaseSpeed_Stack(int argc, char *argv[])
{
	if (3 != argc)
	{
		printf("usage:%s [per memory size byte] [sleep time ms]\n", argv[0]);
		exit(-1);
	}

	int sizeByte = atoi(argv[1]);
	int sleepTimeMs = atoi(argv[2]);
	printf("pid is %d\n", getpid());

	void *pTmp = NULL;
	for (int i = 0; i < 1000; ++i)
	{
		pTmp = malloc(sizeByte);
		if (NULL == pTmp)
		{
			perror("malloc fail");
			continue;
		}
		bzero(pTmp, sizeByte);

		usleep(1000 * sleepTimeMs);
	}

	return 0;
}


#endif /* MEMORYINCREASESPEED_CPP */
/* end file */
