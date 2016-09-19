/*
 * errorTest.cpp
 *
 *  Created on: 2015-2-19
 *      Author: Hong
 */
#include "../config.h"
#if ERRORTEST_CPP

#include "../common/baseTypedef.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


Int32 ErrorTest_strerror(Int32 argc, char *argv[]);

/*=================main================*/
Int32 ErrorTest(Int32 argc, char *argv[])
{
	Int32 ret;

	ret = ErrorTest_strerror(argc, argv);

	return ret;
}


Int32 ErrorTest_strerror(Int32 argc, char *argv[])
{
	if (2 != argc)
	{
		printf("usage:%s [errno count]\n", argv[0]);
		exit(-1);
	}

	Int32 loop = atoi(argv[1]);
	char *msg = NULL;
	for (Int32 i = 0; i < loop; ++i)
	{
		msg = strerror(i);
		printf("errno %03d: %s\n", i, msg);
	}

	return 0;
}

#endif /* ERRORTEST_CPP */
/* end file */
