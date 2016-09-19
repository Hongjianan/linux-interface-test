/*
 * getoptlongTest.cpp
 *
 *  Created on: Jan 10, 2015
 *      Author: hong
 */
#include "../config.h"
#if GETOPTLONGTEST_CPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>


# define NO_ARG       no_argument
# define REQ_ARG      required_argument
# define OPT_ARG      optional_argument

static const char *optionStr = "as:m:d:";
static const struct option options[] =
{
	{"add",			no_argument, 		NULL, 'a'},
	{"sub",			required_argument, 	NULL, 's'},
	{"multiple",	required_argument, 	NULL, 'm'},
	{"divide",		optional_argument, 	NULL, 'd'},
	{ 0 }
};

int GetoptlongTest_OnlyOption(int argc, char *argv[]);

/*==================main================*/
int GetoptlongTest(int argc, char *argv[])
{
	int ret;

	ret = GetoptlongTest_OnlyOption(argc, argv);

	return ret;
}


int GetoptlongTest_OnlyOption(int argc, char *argv[])
{
	for (int idx = 0; idx < argc; ++idx)
	{
		printf("argv[%d] = %s\n", idx, argv[idx]);
	}

	char *programName = strrchr(argv[0], '/') + 1;
	printf("program name is %s.\n", programName);
	printf("==============\n");

	int opt;

	while (-1 != (opt = getopt_long(argc, argv, optionStr, options, NULL)))
	{
		switch (opt)
		{
		case 'a':
		case 's':
		case 'm':
		case 'd':
			printf("option is %c\n", opt);
			printf("optind is %d\n", optind);
			printf("argv[optind-1] is %s\n", argv[optind - 1]);
			printf("optarg is %s\n", optarg);
			printf("optarg is %p\n", optarg);
			printf("------------\n");

//			printf("optopt is %d\n", optopt);
//			printf("optreset is %d\n", optreset);
//			printf("opterr is %d\n", opterr);
			break;

		default:
			printf("usage:%s [asmd].\n", argv[0]);
			exit(-1);
		}
	}

	if (optind < argc)
	{
		while (optind < argc)
			printf("%s ", argv[optind++]);
		printf("\n");
	}

	exit(EXIT_SUCCESS);
}

#endif /* GETOPTLONGTEST_CPP */
/* end file */
