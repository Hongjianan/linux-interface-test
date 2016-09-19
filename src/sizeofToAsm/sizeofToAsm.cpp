/*
 * sizeofToAsm.cpp
 *
 *  Created on: 2015-4-17
 *      Author: hongjianan
 */
#include "../config.h"
#if SIZEOFTOASM_CPP

#include <stdio.h>

/*
 * sizeof(int) sizeof(variable)
 * 都是在编译时已经确定的了
 *
 * */
int SizeofToAsm_sizeof( int argc, char* argv[]);
/*==================main=============*/
int SizeofToAsm( int argc, char* argv[])
{
	int ret;
	ret = SleepTest_1( argc, argv);
	return 0;
}

int SizeofToAsm_sizeof( int argc, char* argv[])
{
	int size;
	int tmp;
	size = sizeof(size);

	tmp = 100;

	size = 4;

	return 0;
}


#endif /* SIZEOFTOASM_CPP */
/* end file */




