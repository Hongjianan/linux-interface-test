/*
 * getoptTest.cpp
 *
 *  Created on: Jan 9, 2015
 *      Author: hong
 */
#include "../config.h"
#if GETOPTTEST_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


int GetoptTest_getopt(int argc, char *argv[]);

/*================main==============*/
int GetoptTest(int argc, char *argv[])
{
	int ret;

	ret = GetoptTest_getopt(argc, argv);

	return ret;
}


int GetoptTest_getopt(int argc, char *argv[])
{
	const char *opts = "abn:";
	char opt = 0;
	char name[32] = {0};
	while (-1 != (opt = getopt(argc, argv, opts)))
	{
		switch (opt)
		{
		case 'a':
			printf("choose a.\n");
			break;
		case 'b':
			printf("choose b.\n");
			break;
		case 'n':
			printf("choose n.\n");
			memcpy(name, optarg, strlen(optarg));
			break;
		default:	/* '?' */
			fprintf(stderr, "Usage: %s [-a] [-b] [-n name]\n", argv[0]);
			exit(EXIT_FAILURE);
			break;
		}
	}

	if (optind >= argc)
	{
		printf("input other arguments\n");
		exit(EXIT_FAILURE);
	}

	if ('\0' != name[0])
	{
		printf("name is %s\n", name);
	}
	else
	{
		printf("you have no input name\n");
	}

	return 0;
}


#endif /* GETOPTTEST_CPP */
/* end file */
