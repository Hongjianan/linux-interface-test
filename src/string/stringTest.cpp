/*
 * stringTest.cpp
 *
 *  Created on: 2015-9-27
 *      Author: hongjianan
 */
#include "../config.h"
#if STRINGTEST_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


int StringTest_memmove(int argc, char *argv[]);

/*================main==============*/
int StringTest(int argc, char *argv[])
{
	int ret;

	ret = StringTest_memmove(argc, argv);

	return ret;
}


int StringTest_memmove(int argc, char *argv[])
{
	int array[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

	memmove(&array[8], &array[9], sizeof(array[0]) * 1);

	int idx;
	for (idx = 0; idx < sizeof(array)/sizeof(array[0]); ++idx)
	{
		printf("%d ", array[idx]);
	}
	putchar('\n');

	return 0;
}

#endif /* STRINGTEST_CPP */
/* end file */
