/*
 * getAuthority.cpp
 *
 *  Created on: 2015-8-25
 *      Author: hongjianan
 */
#include "../config.h"
#if GETAUTHORITY_CPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

const char *passwordRoot = "hjnhong1\n";

int GetAuthority_Root(int argc, char *argv[]);

/*================main================*/
int GetAuthority(int argc, char *argv[])
{
	int ret;

	ret = GetAuthority_Root(argc, argv);

	return ret;
}


int GetAuthority_Root(int argc, char *argv[])
{
	system("whoami");

	int childPid;
	childPid = fork();
	if (0 == childPid)
	{
		system("su");
	}
	else
	{
		sleep(1);

		system("echo hjnhong1 > /dev/pts/0");
#if 0
		if (strlen(passwordRoot) != fprintf(stdout, "%s", passwordRoot))
		{
			perror("write error");
			exit(-1);
		}
		fflush(stdout);
#endif
		system("whoami");
	}

	return 0;
}

#endif /* GETAUTHORITY_CPP */
/* end file */
