/*
 * fileExist.cpp
 *
 *  Created on: 2015-9-21
 *      Author: hongjianan
 */
#include "../config.h"
#if FILEEXIST_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>


int FileExist_access(int argc, char *argv[]);

/*================main==============*/
int FileExist(int argc, char *argv[])
{
	int ret;

	ret = FileExist_access(argc, argv);

	return 0;
}


int FileExist_access(int argc, char *argv[])
{
	const char *fileName = "/tmp/hong";

	int ret;
	ret = access(fileName, F_OK);

	if (!ret)	/* return 0 meanings file exist. */
	{
		printf("file: %s is exist.\n", fileName);
	}
	else
	{
		printf("file: %s is not exist.\n", fileName);
	}

	return 0;
}

#endif /* FILEEXIST_CPP */
/* end file */
