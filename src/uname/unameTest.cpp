/*
 * unameTest.cpp
 *
 *  Created on: Dec 19, 2014
 *      Author: hong
 */
#include "../config.h"
#if UNAMETEST_CPP

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/utsname.h>

int UnameTest_uname(int argc, char *argv[]);

/*==============main===============*/
int UnameTest(int argc, char *argv[])
{
	int ret;

	ret = UnameTest_uname(argc, argv);

	return ret;
}

int UnameTest_uname(int argc, char *argv[])
{
	struct utsname name;

	if (0 == uname(&name))
	{
		printf("domainname:%s\nmachine:%s\nnodename:%s\nrelease:%s\nsysname:%s\nversion:%s\n", \
				name.domainname, name.machine, name.nodename, name.release, name.sysname, name.version);
	}
	else
	{
		perror("uname failed"), exit(-1);
	}

	return 0;
}

#endif /* UNAMETEST_CPP */
/* end file */
