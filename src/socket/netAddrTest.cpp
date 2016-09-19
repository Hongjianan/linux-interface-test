/*
 * netAddr.cpp
 *
 *  Created on: Dec 18, 2014
 *      Author: hong
 */
#include "../config.h"
#if NETADDRTEST_CPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>	/* inet_addr */
#include <arpa/inet.h>

int NetAddrTest_Exchange(int argc, char *argv[]);
int NetAddrTest_GetNetAndHost(int argc, char *argv[]);

/*==============main=================*/
int NetAddrTest(int argc, char *argv[])
{
	int ret;

//	ret = NetAddrTest_Exchange(argc, argv);
	ret = NetAddrTest_GetNetAndHost(argc, argv);

	return ret;
}

int NetAddrTest_Exchange(int argc, char *argv[])
{
	char nip2[4] = {192, 168, 0, 26};
	int *pnip2 = (int *)nip2;

	/* integer to ascii */
	in_addr_t nip = (192 << 24) | (168 << 16) | (0 << 8) | 26;	/* integer */
	struct in_addr snip = {nip};	/* sip.s_addr = nip; */
	printf("ntoa %s\n", inet_ntoa(snip));	/* net byte */

	/* ascii to integer */
	const char *sip = "192.168.0.26";
	int isip = inet_addr(sip); /* net byte */
	printf("big   endian %u.%u.%u.%u\n", (isip >> 24) & 255, (isip >> 16) & 255, (isip >> 8) & 255, isip & 255); /* big endian */

	printf("small endian %u.%u.%u.%u\n", isip & 255, (isip >> 8) & 255, (isip >> 16) & 255, (isip >> 24) & 255); /* small endian */

	snip.s_addr = isip;
	printf("ntoa %s\n", inet_ntoa(snip));	/* net byte */

	return 0;
}

int NetAddrTest_GetNetAndHost(int argc, char *argv[])
{
	const char *cIP = "192.168.0.26";
	struct in_addr nIP = {0};
	if(0 == inet_aton(cIP, &nIP))
	{
		perror("inet_aton failed"), exit(-1);
	}

	struct in_addr sIP = {0};

	in_addr_t localName = inet_lnaof(nIP);
	sIP.s_addr = localName;
	printf("%s localName is :%s\n", cIP, inet_ntoa(sIP));

	in_addr_t netName = inet_netof(nIP);
	sIP.s_addr = netName;
	printf("%s netName is :%s\n", cIP, inet_ntoa(sIP));

	sIP = inet_makeaddr(netName, localName);
	printf("make addr is %s\n", inet_ntoa(sIP));

	return 0;
}


#endif
/* end file */
