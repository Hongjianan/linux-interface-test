/*
 * getHostTest.cpp
 *
 *  Created on: Dec 19, 2014
 *      Author: hong
 */
#include "../config.h"
#if GETHOSTTEST_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>		/* include the config about net */

int GetHostTest_gethostent(int argc, char *argv[]);
int GetHostTest_gethostbyname(int argc, char *argv[]);
//int GetHostTest_getprotobyaddr(int argc, char *argv[]);

int GetHostTest_getprotoent(int argc, char *argv[]);
int GetHostTest_getprotobyname(int argc, char *argv[]);
//int GetHostTest_getprotobynumber(int argc, char *argv[]);

/*==============main=================*/
int GetHostTest(int argc, char *argv[])
{
	int ret;

//	ret = GetHostTest_gethostent(argc, argv);
//	ret = GetHostTest_gethostbyname(argc, argv);

//	ret = GetHostTest_getprotoent(argc, argv);
	ret = GetHostTest_getprotobyname(argc, argv);

	return ret;
}

int GetHostTest_gethostent(int argc, char *argv[])
{
	/* open net database file , "1" means keep connecting */
	sethostent(1);

	struct hostent *ent;
	int i;
	for (i = 0; NULL != (ent = gethostent()); i++)
	{
		printf("local host %04d\n", i+1 );

		if (ent->h_addr_list[i])
		{
			printf("%s ip :%hhu.%hhu.%hhu.%hhu\t length is %d byte\n",
					ent->h_name, \
					ent->h_addr_list[i][0], ent->h_addr_list[i][1], ent->h_addr_list[i][2], ent->h_addr_list[i][3], \
					ent->h_length);
		}
		else
		{
			printf("%s ip is null.\n", ent->h_name);
		}
		putchar('\n');
	}

	endhostent();
	return 0;
}

int GetHostTest_gethostbyname(int argc, char *argv[])
{
	const char *hostName = "www.baidu.com";

	struct hostent *ent;
	ent = gethostbyname(hostName);

	/* print host ip */
	int i;
	for (i = 0; ent->h_addr_list[i]; i++)
	{
		printf("%s ip is: %hhu.%hhu.%hhu.%hhu\n", hostName,
				ent->h_addr_list[i][0], ent->h_addr_list[i][1], ent->h_addr_list[i][2], ent->h_addr_list[i][3]);
	}

	/* print host aliase */
	for (i = 0; ent->h_aliases[i]; i++)
	{
		printf("%s aliase is: %s\n", hostName, ent->h_aliases[i]);
	}
	if (0 == i)
		printf("%s have no aliase\n", hostName);

	return 0;
}

int GetHostTest_getprotoent(int argc, char *argv[])
{
	setprotoent(1);

	struct protoent *proto;
	while (NULL != (proto = getprotoent()))
	{
		printf("%s\tnumber:%04d\taliase is %s\n", proto->p_name, proto->p_proto, proto->p_aliases[0]);
	}

	endprotoent();

	return 0;
}

int GetHostTest_getprotobyname(int argc, char *argv[])
{
	struct protoent *proto = NULL;
	const char *protoName = "UDP";
	if (NULL != (proto = getprotobyname(protoName)))
	{
		printf("%s number:%04d \t", proto->p_name, proto->p_proto);
		int i;
		for(i = 0; proto->p_aliases[i]; i++)
		{
			printf("%04d aliase:%s\t", i, proto->p_aliases[i]);
		}
		putchar('\n');
	}
	else
	{
		perror("getprotobyname failed,"), exit(-1);
	}

	return 0;
}

#endif /* GETHOSTTEST_CPP*/
/* end file */
