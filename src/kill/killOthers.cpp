/*
 * killOthers.cpp
 *
 *  Created on: 2015-3-31
 *      Author: hongjianan
 */
#include "../config.h"
#if KILLOTHERS_CPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int KillOthers_UnknownPid(int argc, char *argv[]);

/*================main==============*/
int KillOthers(int argc, char *argv[])
{
	int ret;
	ret = KillOthers_UnknownPid( argc, argv);
	return 0;
}


int KillOthers_UnknownPid(int argc, char *argv[])
{
	if (2 != argc)
	{
		printf("usage:%s [pid name]\n", argv[0]);
		exit(-1);
	}

	char cmdLine[128] = {0};
	sprintf(cmdLine, "ps -ef | grep %s > /tmp/pidName.txt\n", argv[1]);

	system(cmdLine);

	system("rm -f /tmp/pidName.txt");



	return 0;
}


#endif /* KILLOTHERS_CPP */
/* end file */
