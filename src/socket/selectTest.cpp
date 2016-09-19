/*
 * selectTest.cpp
 *
 *  Created on: 2015-2-25
 *      Author: Hong
 */
#include "../config.h"
#if SELECTTEST_CPP

#include "../common/baseTypedef.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <error.h>


Int32 SelectTest_input(Int32 argc, char *argv[]);

/*=================main=================*/
Int32 SelectTest(Int32 argc, char *argv[])
{
	Int32 ret;

	ret = SelectTest_input(argc, argv);

	return ret;
}


Int32 SelectTest_input(Int32 argc, char *argv[])
{
	/* set fd_set */
	fd_set readSet;
	FD_ZERO(&readSet);
	FD_SET(0, &readSet);

	ssize_t rc;
	int i;
	char buffer[10] = {0};
	while (1)
	{
		/* set file to be async */
		rc = select(1, &readSet, 0, 0, (struct timeval *)NULL);

		for (i = 0; ;++i)
		{
			rc = read(0, buffer, sizeof(buffer)-1);
			if (rc > 0)
			{
				buffer[rc] = '\0';
				printf("[%03d] :%s\n", i, buffer);
				if (rc != sizeof(buffer)-1)
				{
					break;
				}
			}
			else if (-1 == rc)
			{
				perror("read error");
				exit(-1);
			}
			else if (0 == rc)
			{
				i = 0;
				break;
			}
		}
	}

	return 0;
}


#endif /* SELECTTEST_CPP */
/* end file */
