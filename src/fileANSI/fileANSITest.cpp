/*
 * fileANSITest.cpp
 *
 *  Created on: 2015-2-21
 *      Author: Administrator
 */
#include "../config.h"
#if FILEANSITEST_CPP

#include <stdio.h>
#include <stdlib.h0>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int FileANSITest_fopen_fclose(int argc, char *argv[]);

/*================main================*/
int FileANSITest(int argc, char *argv[])
{
	int ret;

	ret = FileANSITest_fopen_fclose(argc, argv);

	return ret;
}


int FileANSITest_fopen_fclose(int argc, char *argv[])
{
	FILE *fp;
	return 0;
}

#endif /* FILEANSITEST_CPP */
/* end file */
