/*
 * pthreadTest.cpp
 *
 *  Created on: 2015-9-22
 *      Author: hongjianan
 */
#include "../config.h"
#if PTHREAD_KEY_TEST_CPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

static pthread_once_t	gOnce = PTHREAD_ONCE_INIT;
static pthread_key_t	gKey;

void *ThreadKey(void *arg);

/*===============main================*/
static int PthreadKeyTest_Create(int argc, char *argv[]);
int PthreadKeyTest(int argc, char *argv[])
{
	int ret;

	ret = PthreadKeyTest_Create(argc, argv);

	return ret;
}

int PthreadKeyTest_Create(int argc, char *argv[])
{
	pthread_t tmp;
	int idx;
	for (idx = 0; idx < 2; ++idx)
	{
		pthread_create(&tmp, NULL, ThreadKey, NULL);
	}

	sleep(5);

	return 0;
}

void Destructor(void *buf)
{
	printf("Destructor running.\n");
	free(buf);
}

void CreateKey(void)
{
	if (pthread_key_create(&gKey, Destructor))
	{
		perror("pthread_key_create failed");
		exit(errno);
	}
}

void *ThreadKey(void *arg)
{
	pthread_key_t key;
	if (pthread_once(&gOnce, CreateKey))
	{
		perror("pthread_key_create failed");
		exit(errno);
	}
	printf("Key[%d]\n", key);


	return NULL;
}


#endif /* PTHREAD_KEY_TEST_CPP */
/* end file */
