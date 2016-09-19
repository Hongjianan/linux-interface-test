/*
 * getLocalIP.cpp
 *
 *  Created on: 2015Äê7ÔÂ16ÈÕ
 *      Author: Hong
 */
#include "../config.h"
#if GETLOCALIP_CPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>		/* include the config about net */

#include <ifaddrs.h>

int GetLocalAddr(char *retAddr, const char *device, int familyType);

int GetHostTest_getifaddrs(int argc, char *argv[]);

/*==============main=================*/
int GetLocalIP(int argc, char *argv[])
{
	int ret;

	ret = GetHostTest_getifaddrs(argc, argv);

	return ret;
}

int GetHostTest_getifaddrs(int argc, char *argv[])
{
	static const char inetName[]	= "AF_INET";
	static const char inet6Name[]	= "AF_INET6";

	struct ifaddrs *addrHeader;
	if (-1 == getifaddrs(&addrHeader))
	{
		perror("getifaddrs fail");
		exit(errno);
	}

	int family;
	const char *familyName = NULL;
	int addrLen;
	char addrStr[NI_MAXHOST] = {0};
	char maskStr[NI_MAXHOST] = {0};
	struct ifaddrs *ifAddr;

	for (ifAddr = addrHeader; ifAddr; ifAddr = ifAddr->ifa_next)
	{
		family = ifAddr->ifa_addr->sa_family;

		if (AF_UNIX == family)
		{
			printf("get family==AF_UNIX.\n");
			continue;
		}
		else if (AF_INET == family)
		{
			familyName	= &inetName[0];
			addrLen		= sizeof(struct sockaddr_in);
		}
		else if (AF_INET6 == family)
		{
			familyName  = &inet6Name[0];
			addrLen		= sizeof(struct sockaddr_in6);
		}
		else
		{
			printf("get family error.\n");
			continue;
		}

		if (family == AF_INET || family == AF_INET6)
		{
			getnameinfo(ifAddr->ifa_addr, addrLen, addrStr, sizeof(addrStr), NULL, 0, NI_NUMERICHOST);
			getnameinfo(ifAddr->ifa_netmask, addrLen, maskStr, sizeof(maskStr), NULL, 0, NI_NUMERICHOST);

			printf("device:%s\n"
					"\tfamily:%s\taddr:%s\tmask:%s\n",
					ifAddr->ifa_name, familyName, addrStr, maskStr);
		}
	}

	freeifaddrs(addrHeader);

	return 0;
}


int GetLocalAddr(char *retAddr, const char *device, int familyType)
{
	if (NULL == retAddr || NULL == device || (AF_INET != familyType && AF_INET6 != familyType))
	{
		printf("argument error.\n");
		return -1;
	}

	struct ifaddrs *addrHeader;
	if (-1 == getifaddrs(&addrHeader))
	{
		perror("getifaddrs fail");
		return -1;
	}

	int family;
	int addrLen;
	char addrStr[NI_MAXHOST] = {0};
	struct ifaddrs *ifAddr;

	for (ifAddr = addrHeader; ifAddr; ifAddr = ifAddr->ifa_next)
	{
		family = ifAddr->ifa_addr->sa_family;

		if (!strcmp(device, ifAddr->ifa_name) && familyType == family)
		{
			getnameinfo(ifAddr->ifa_addr, addrLen, addrStr, sizeof(addrStr), NULL, 0, NI_NUMERICHOST);
			break;
		}
	}

	freeifaddrs(addrHeader);
	if (ifAddr)
	{
		strcpy(retAddr, addrStr);
		return 0;
	}

	return -1;
}


#endif /* GETLOCALIP_CPP*/
/* end file */
