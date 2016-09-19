/*
 * topTest.cpp
 *
 *  Created on: 2014-12-9
 *      Author: hong
 */
#include "../config.h"
#if TOPTEST_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int TopTest(int argc, char* argv[])
{
	/* this program is strange . To be continue research. */
	for(int i = 0; i < 10; ++i) {
		printf("before top ----------------------------\n");
		system("top >> top.txt");
	}
	return 0;
}

#endif
/* end file */
