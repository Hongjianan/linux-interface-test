/*
 * udpMuliSendOneRecv.cpp
 *
 *  Created on: Feb 27, 2015
 *      Author: freescale
 */
#include "../config.h"
#if UDPMULISENDONERECV_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <errno.h>
#include <sys/ioctl.h>

const char *g_localIP = "127.0.0.1";
const unsigned short g_recvPort = 54321;
const unsigned short g_sendPort = 50000;
int sleepTimems;
int recvBufSize;
char sendText[1000];

int gSockFd;

struct ThreadSendArg
{
	unsigned short sendPort;
	unsigned int   cpu;
};

int CreateSendSocket(int *pSockFd, unsigned short port);

void *ThreadRecv(void *arg);
void *ThreadSend(void *arg);

/*==================main===================*/
int UdpMuliSendOneRecv(int argc, char *argv[])
{
	if (0 != CreateSendSocket(&gSockFd, g_sendPort))
	{
		printf("CreateSendSocket fail\n");
		exit(errno);
	}

	int i;
	pthread_t threadRecv, threadSend;

	if (-1 == pthread_create(&threadRecv, NULL, ThreadRecv, (void *)&g_recvPort))
	{
		perror("pthread_create fail");
		exit(errno);
	}
	sleep(1);
	for (i = 0; i < 6; ++i)
	{
		if (-1 == pthread_create(&threadSend, NULL, ThreadSend, (void*)&i))
		{
			printf("pthread_create %d fail:%s\n", i, strerror(errno));
			exit(errno);
		}
		printf("pthread_create success [%d]\n", i);
		usleep(100000);
	}

	for (i = 0; ; ++i)
	{
		printf("run loop %d*30s\n", i);
		sleep(30);
	}

	return 0;
}


void *ThreadRecv(void *arg)
{
	unsigned short port = *((unsigned short*)arg);
	pthread_t pid = pthread_self();

	/* 1. create udp socket */
	int udpFd = socket(AF_INET, SOCK_DGRAM, getprotobyname("udp")->p_proto);
	if (-1 == udpFd)
		perror("socket failed"), exit(-1);
	printf("[thread pid:%zu]socket success.\n", pid);

	/* 2. setsockopt */
	char recvBuf[1024] = {0};
#if 0
	if(-1 == setsockopt(udpFd, SOL_SOCKET, SO_RCVBUF, (char*)&recvBuf, sizeof(recvBufSize)))
	{
		perror("recv setsockopt failed");
	}
#endif

	/* 3. bind local address to socket */
	struct sockaddr_in addrLocal;
	addrLocal.sin_family = AF_INET;
	addrLocal.sin_port = htons(port);
	inet_aton(g_localIP, &addrLocal.sin_addr);
	if (-1 == bind(udpFd, (struct sockaddr*)&addrLocal, sizeof(addrLocal)))
	{
		close(udpFd);
		perror("bind failed"), exit(-1);
	}
	printf("[thread pid:%zu]bind success.\n", pid);

	/* 4. recv message */
	int rc;
	struct sockaddr_in addrSend;	/* to save send address */
	/* two effects: 1. incoming length of addrSend; 2. save length of addrSend return */
	socklen_t len;

	for (int i = 0; ; ++i)
	{
		len = sizeof(addrSend);
		rc = recvfrom(udpFd, (void *)recvBuf, sizeof(recvBuf)-1, 0, (struct sockaddr *)&addrSend, &len);

		if (rc > 0)
		{
			recvBuf[rc] = '\0';
			printf("[%09d]Source IP :%s PORT :%d Receive data length :%d.num = %c\n", \
					i, inet_ntoa(addrSend.sin_addr), ntohs(addrSend.sin_port), rc, recvBuf[0]);
			if (recvBuf[0] != recvBuf[rc-1])
			{
				printf("recvBuf no equal, exit\n");
				exit(errno);
			}
		}
		else if (0 == rc)
		{
			printf("net down.\n");
			break;
		}
		else if (-1 == rc)
		{
			printf("net broken.\n");
			break;
		}

	}/* while (1) */

	/* 4. close socket file */
	close(udpFd);
	return 0;
}


void* ThreadSend(void *arg)
{
	unsigned short num = *((int*)arg);
	pthread_t pid = pthread_self();

	char sendBuf[1024];
	memset(sendBuf, '0'+num, sizeof(sendBuf)-1);
	sendBuf[sizeof(sendBuf)-1] = '\0';
	unsigned int textLength = strlen(sendBuf);

	int rc;
	while (1)
	{
		if ((rc = send(gSockFd, sendBuf, textLength, 0)) >= 0)
		{
//			printf("send success.\n");
		}

		else if (-1 == rc)
		{
			perror("send failed");
			break;
		}
	}

	/* 5. close socket file */
	close(gSockFd);

	return 0;
}


int CreateSendSocket(int *pSockFd, unsigned short port)
{
	/* 1. create socket */
	int udpFd;
	if (-1 == (udpFd = socket(AF_INET, SOCK_DGRAM, getprotobyname("udp")->p_proto)))
		perror("socket failed"), exit(-1);
	printf("CreateSendSocket: create socket success.\n");

	/* also can send message even if don't bind port. receiver also can get ip and port of sender. */
	/* kernel driver will allocate port to proccess. */
	/* 2. bind local address to socket */
	struct sockaddr_in sendAddr;
	sendAddr.sin_family = AF_INET;
	sendAddr.sin_port = htons(port);
	inet_aton(g_localIP, &(sendAddr.sin_addr));
	if (-1 == bind(udpFd, (struct sockaddr *)&sendAddr, sizeof(sendAddr)))
	{
		close(udpFd);
		perror("bind failed"), exit(-1);
	}
	printf("CreateSendSocket: bind success.\n");

	/* 3. set destination address */
	struct sockaddr_in addrDest;
	addrDest.sin_family = AF_INET;
	addrDest.sin_addr.s_addr = inet_addr(g_localIP);
	addrDest.sin_port = htons(g_recvPort);

	/* 4. send message */
	/* connect -- send  ;  no connect -- sendto*/
	int rc;
	char sendBuf[1024];

/* connect destination address to socket file, before sending message. */
	if (-1 == connect(udpFd, (struct sockaddr *)&addrDest, sizeof(addrDest)))
	{
		close(udpFd);
		perror("connect failed"), exit(-1);
	}

	*pSockFd = udpFd;

	return 0;
}

#endif /* UDPMULISENDONERECV_CPP */
/* end file */
