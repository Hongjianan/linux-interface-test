/*
 * tmp.cpp
 *
 *  Created on: 2015-4-8
 *      Author: hongjianan
 */
/*
 * segfaultTest.cpp
 *
 *  Created on: 2014-12-30
 *      Author: hong
 */
#include "../config.h"
#if TMPTEST_CPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>


/*=============main=============*/
int TmpTest(int argc,char* argv[])
{
	if (2 != argc)
	{
		printf("usage:[return num]\n");
		exit(-1);
	}

	int *p = NULL;
	*p = 100;

	return atoi(argv[1]);
}


#endif /* TMPTEST_CPP */
/* end file */
