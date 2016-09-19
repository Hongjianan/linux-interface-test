/*
 * tcpTest.cpp
 *
 *  Created on: Dec 20, 2014
 *      Author: hong
 */
#include "../config.h"
#if TCPTEST_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>

#define PORT_SERVER		12345
#define PORT_CLIENT		54321
const char *loopIP		= "127.0.0.1";
const char *g_localIP	= "192.168.0.105";

int GetLocalAddr(char *retAddr, const char *device, int familyType);

int InitInetSocket(const char *strIP, const unsigned short port, const char *proto);
int ConnectToServer(int clientFd, const char *serverIP, const unsigned short serverPort);

int TcpTest_ServerRollAccept(int argc, char *argv[]);
int TcpTest_ServerAcceptOneClient(int argc, char *argv[]);

/* another program */
int TcpTest_Client(int argc, char *argv[]);

/*=============main==============*/
int TcpTest(int argc, char *argv[])
{
	int ret;

//	ret = TcpTest_ServerRollAccept(argc, argv);
	ret = TcpTest_ServerAcceptOneClient(argc, argv);
//	ret = TcpTest_Client(argc, argv);

	return ret;
}


int TcpTest_ServerRollAccept(int argc, char *argv[])
{
	/* 1. create server socket */
	int serverFd = InitInetSocket(g_localIP, PORT_SERVER, "tcp");
	if (-1 == serverFd)
	{
		printf("CreateInetSocket fail.\n"), exit(-1);
	}

	/* 2. listen */
	if (-1 == listen(serverFd, 10))
	{
		(void)printf("3.listen fail :%m");
		close(serverFd);
		exit(-1);
	}
	(void)printf("3.listen success\n");

	/* 4. accept */
	int clientFd;
	struct sockaddr_in addrClient;
	socklen_t len;

	while (1)
	{
		len = sizeof(addrClient);
		clientFd = accept(serverFd, (struct sockaddr *)&addrClient, &len);
		if (-1 == clientFd)
		{
			(void)printf("4.accept failed :%m");
			close(serverFd);
			exit(-1);
		}
		(void)printf("[accept] new client connect, IP[%s] PORT[%d]\n",
					inet_ntoa(addrClient.sin_addr), ntohs(addrClient.sin_port));
	}

	return 0;
}


int TcpTest_ServerAcceptOneClient(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("usage:[recvBuf length byte]\n"), exit(-1);
	}

	/* 1. create server socket */
	char localAddr[32];
	if (-1 == GetLocalAddr(localAddr, "eth0", AF_INET))
	{
		printf("GetLocalAddr fail.\n"), exit(-1);
	}

	int serverFd = InitInetSocket(localAddr, PORT_SERVER, "tcp");
	if (-1 == serverFd)
	{
		printf("CreateInetSocket fail.\n"), exit(-1);
	}

	/* 2. listen */
	if (-1 == listen(serverFd, 10))
	{
		(void)printf("3.listen fail :%m");
		close(serverFd);
		exit(-1);
	}
	(void)printf("3.listen success\n");

	/* 4. accept */
	int clientFd;
	struct sockaddr_in addrClient;
	socklen_t len;
	len = sizeof(addrClient);

	clientFd = accept(serverFd, (struct sockaddr *)&addrClient, &len);
	if (-1 == clientFd)
	{
		(void)printf("4.accept failed :%m");
		close(clientFd);
		exit(-1);
	}
	(void)printf("[accept] new client connect, IP:%s \t PORT:%d\n",
				inet_ntoa(addrClient.sin_addr), ntohs(addrClient.sin_port));

	/* 5. get client message */
	int recvLen = 0;
	char recvBuf[1024] = {0};
	while (1)
	{
		recvLen = recv(clientFd, recvBuf, atoi(argv[1]), MSG_WAITALL);
		if (recvLen > 0)
		{
			recvBuf[recvLen] = '\0';
			printf("recv client message len[%d]\n%s\n\n", recvLen, recvBuf);
		}
		else if (0 == recvLen)
		{
			printf("client disconnect\n");
			break;
		}
		else if (-1 == recvLen)
		{
			printf("net broken\n");
			break;
		}
	}

	close(clientFd);
	close(serverFd);

	return 0;
}


int TcpTest_Client(int argc, char *argv[])
{
	/* 1. create client socket */
	int clientFd = InitInetSocket(g_localIP, PORT_CLIENT, "tcp");
	if (-1 == clientFd)
	{
		printf("CreateInetSocket fail.\n"), exit(-1);
	}
	(void)printf("1.InitInetSocket success\n");

	/* 2. connect to server */
	if (0 != ConnectToServer(clientFd, g_localIP, PORT_SERVER))
	{
		printf("ConnectToServer fail.\n"), exit(-1);
	}
	printf("2.ConnectToServer success\n");

	/* 3. send message */
	char sendBuf[1024] = {0};
	sprintf(sendBuf,"[%03d]:aaaaaaaaabbbbbbbbbcccccccccdddddddddeeeeeeeee\n", 0);

	int i;
	int tmpLen;
	for(i = 0; i<=99; i++)
	{
		tmpLen = sprintf(sendBuf,"[%03d", i);
		sendBuf[tmpLen] = ']';
		send(clientFd, (void *)sendBuf, strlen(sendBuf), 0);
	}

	/* 4. close socket file */
	close(clientFd);

	return 0;
}


int InitInetSocket(const char *strIP, const unsigned short port, const char *proto)
{
	if (NULL == strIP || NULL == proto)
	{
		printf("InitInetSocket argument error.\n");
		return -1;
	}

	/* 1. create server socket file */
	int sockFd = socket(AF_INET, SOCK_STREAM, getprotobyname(proto)->p_proto);
	if (-1 == sockFd)
	{
		(void)printf("1.socket failed :%m\n");
		return -1;
	}
	(void)printf("1.socket success.\n");

	/* 2. bind */
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(strIP);
	addr.sin_port = htons(port);
	if (-1 == bind(sockFd, (struct sockaddr *)&addr, sizeof(addr)))
	{
		(void)printf("2.bind failed :%m");
		close(sockFd);
		return -1;
	}
	else
	{
		printf("bind success: IP[%s] PORT[%d]\n", strIP, port);
	}

	return sockFd;
}


int ConnectToServer(int clientFd, const char *serverIP, const unsigned short serverPort)
{
	struct sockaddr_in addrServer;

	addrServer.sin_family = AF_INET;
	inet_aton(serverIP, &addrServer.sin_addr);
	addrServer.sin_port = htons(serverPort);

	if (-1 == connect(clientFd, (struct sockaddr *)&addrServer, sizeof(addrServer)))
	{
		printf("connect to server failed :%m\n");
		close(clientFd);
		return errno;
	}

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
			addrLen = (AF_INET == familyType) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);
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

#endif /* TCPTEST_CPP */
/* end file */
