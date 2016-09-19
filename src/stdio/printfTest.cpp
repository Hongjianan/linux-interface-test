/*
 * printfTest.cpp
 *
 *  Created on: Dec 29, 2014
 *      Author: hong
 */
#include "../config.h"
#if PRINTFTEST_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


int PrintfTest_fprintf(int argc, char *argv[]);

/*================main==============*/
int PrintfTest(int argc, char *argv[])
{
	int ret;

	ret = PrintfTest_fprintf(argc, argv);

	return ret;
}


int PrintfTest_fprintf(int argc, char *argv[])
{
	FILE *file = NULL;
	file = fopen("/home/hong/tmp.txt", "wb");
	fprintf(file, "_(%s)\n", "nihao");
	return 0;
}

#endif /* PRINTFTEST_CPP */
/* end file */
