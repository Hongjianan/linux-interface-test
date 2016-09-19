/*
 * udpSend.cpp
 *
 *  Created on: 2015-9-8
 *      Author: hongjianan
 */
#include "../config.h"
#if UDPSEND_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

const char *g_localIP = "127.0.0.1";
struct timeval gTimeBegin, gTimeEnd;
int gPacketSend;

int InitInetSocket(const char *localIP, const unsigned int localPort, const char *protol);
int ConnectServer(int clientFd, const char *serverIP, const unsigned short serverPort);

int UdpSend_Send(int argc, char *argv[]);
/*================main=============*/
int UdpSend(int argc, char *argv[])
{
	int ret;

	ret = UdpSend_Send(argc, argv);

	return ret;
}

int UdpSend_Send(int argc, char *argv[])
{
	int idx;
	if (8 != argc)
	{
		printf("usage:[localIP] [localPort] [remoteIP] [remotePort] [packetNum] [packetLen] [sleepUS]\n");
		exit(-1);
	}
	char *programName;
	programName = strrchr(argv[0], '/') + 1;

	char localIP[16]  = {0};
	char remoteIP[16] = {0};
	unsigned short localPort, remotePort;

	memcpy(localIP,  argv[1], strlen(argv[1]));
	memcpy(remoteIP, argv[3], strlen(argv[3]));
	localPort  = atoi(argv[2]);
	remotePort = atoi(argv[4]);

	int packetNum	= atoi(argv[5]);
	int packetLen	= atoi(argv[6]);
	int sleepTimeUS	= atoi(argv[7]);


	if (0 == packetLen)
	{
		printf("packetLen too short.\n"), exit(-1);
	}

	usleep(100000);

	int udpFd = InitInetSocket(localIP, localPort, "udp");
	if (-1 == udpFd)
	{
	    printf("InitInetSocket fail\n");
	    exit(-1);
	}

	/* 3. set destination address */
	if (0 != ConnectServer(udpFd, remoteIP, remotePort))
	{
		close(udpFd);
		perror("connect failed"), exit(-1);
	}

	char *sendBuf = NULL;
	sendBuf = (char*)malloc(sizeof(char) * (packetLen + 1));
	if (NULL == sendBuf)
	{
		perror("malloc fail"), exit(-1);
	}

	for (idx = 0; idx < packetLen; ++idx)
	{
		sendBuf[idx] = 'a' + (idx % 25);
	}
	sendBuf[packetLen] = '\0';

	struct timeval beginTime, endTime;
	gettimeofday(&beginTime, NULL);

	int sendLen = strlen(sendBuf);
	for (idx = 0; idx < packetNum; ++idx)
	{
		sendLen = send(udpFd, sendBuf, sendLen, 0);
		if (sendLen >= 0)
		{
			++gPacketSend;
		}
		else if (-1 == sendLen)
		{
			perror("sendto failed");
			break;
		}
		usleep(sleepTimeUS);
	}

	/* 5. close socket file */
	close(udpFd);

	gettimeofday(&endTime, NULL);

	printf("use time: %ds %dus. total send [%d] packet\n",
	        endTime.tv_sec - beginTime.tv_sec, endTime.tv_usec - beginTime.tv_usec, gPacketSend);

	return 0;
}

int InitInetSocket(const char *localIP, const unsigned int localPort, const char *protol)
{
    /* 1. create udp socket */
    int sockFd = socket(AF_INET, SOCK_DGRAM, getprotobyname(protol)->p_proto);
    if (-1 == sockFd)
    {
        perror("socket failed");
        return -1;
    }
    printf("socket success.\n");

    /* 2.1 设置地址重用 */
    int tmp = 1;
    if(-1 == setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(tmp)))
    {
        close(sockFd);
        perror("setsockopt failed");
        return -1;
    }

    /* 2. bind local address to socket */
    struct sockaddr_in addrLocal;
    addrLocal.sin_family = AF_INET;
    addrLocal.sin_port = htons(localPort);
    inet_aton(localIP, &addrLocal.sin_addr);
    if (-1 == bind(sockFd, (struct sockaddr *)&addrLocal, sizeof(addrLocal)))
    {
        close(sockFd);
        perror("bind failed");
        return -1;
    }
    printf("bind success ip[%s] port[%u].\n", localIP, localPort);

    return sockFd;
}

int ConnectServer(int clientFd, const char *serverIP, const unsigned short serverPort)
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


#endif /* UDPSEND_CPP */
/* end file */
