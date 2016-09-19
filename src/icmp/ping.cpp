/*
 * ping.cpp
 *
 *  Created on: 2015-7-9
 *      Author: Hong
 * ping program.
 */
#include "../config.h"
#if PING_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>

struct SendArgs
{
	int 				sockFd;
	struct sockaddr_in *remoteAddr;
	int 				packetNum;
	int 				dataLen;
	int 				intervalTimeMS;
};

struct RecvArgs
{
	int 				sockFd;
	struct sockaddr_in	*remoteAddr;
	int 				packetNum;
	int 				recvBufLen;
	unsigned char		*pCmpData;
};

#define PROTOCOL_NAME	"icmp"

const char ICMP_HEAD_LEN 	= 8;
const char *LoopBackIP		= "127.0.0.1";

unsigned char *gICMPPacket	= NULL;
unsigned char *gpDataBuf 	= NULL;

int  gPid = 0;
char gResFlag = false;


int InitRAWSokcet(const char *protocol);
void SendIcmpPacket(int sockFd, struct sockaddr_in *remoteAddr, int packetNum, int dataLen, int intervalTimeMS);
int SetIcmpHead(unsigned char *icmpBuf, int dataLen, int packetSno, int pid);
unsigned short CalcCheckSum(unsigned short *bufAddr, int len);

void RecvIcmpPacket(int sockFd, struct sockaddr_in *pRemoteAddr, int packetNum, int recvBufLen, unsigned char *pCmpData);
int UnpackIcmpPacket(unsigned char *pBufData, int bufLen, struct timeval *recvTime, struct in_addr *sourceAddr);


void *ThreadSend(void *arg);
void *ThreadRecv(void *arg);

/*=============main==============*/
int Ping(int argc, char *argv[])
{
	if (5 != argc)
	{
		printf("usage:[remoteIP] [packetNum] [dataLen] [intervalTimeMS]\n");
		exit(-1);
	}
	printf("sizeof struct icmp = %d\n", sizeof(struct icmp));

	char remoteIP[16] = {0};
	strcpy(remoteIP, argv[1]);
	int packNum = atoi(argv[2]);
	int dataLen = atoi(argv[3]);
	int timeMS	= atoi(argv[4]);

	gPid = getpid();
	/* 1. */
	int sockFd;
	sockFd = InitRAWSokcet(PROTOCOL_NAME);
	if (-1 == sockFd)
	{
		printf("InitRAWSokcet fail.\n");
		exit(-1);
	}

	/* 2. */
	struct sockaddr_in remoteAddr = {0};
	remoteAddr.sin_family		= AF_INET;
	remoteAddr.sin_addr.s_addr	= inet_addr(remoteIP);

	/* 3. */
	pthread_t threadSend, threadRecv;

	struct SendArgs sendArg = {0};
	sendArg.sockFd			= sockFd;
	sendArg.remoteAddr		= &remoteAddr;
	sendArg.packetNum		= packNum;
	sendArg.dataLen			= dataLen;
	sendArg.intervalTimeMS	= timeMS;
	if (0 != pthread_create(&threadSend, NULL, ThreadSend, (void *)&sendArg))
	{
		perror("pthread_create ThreadSend");
		exit(-1);
	}

	struct RecvArgs recvArg = {0};
	recvArg.sockFd			= sockFd;
	recvArg.remoteAddr		= &remoteAddr;
	recvArg.packetNum		= packNum;
	recvArg.recvBufLen		= dataLen + 50;
	recvArg.pCmpData		= gpDataBuf;
	if (0 != pthread_create(&threadRecv, NULL, ThreadRecv, (void *)&recvArg))
	{
		perror("pthread_create ThreadRecv");
		exit(-1);
	}

	pthread_join(threadSend, NULL);

	return 0;
}

void *ThreadSend(void *arg)
{
	struct SendArgs *sendArg = (struct SendArgs*)arg;

	SendIcmpPacket(sendArg->sockFd, sendArg->remoteAddr, sendArg->packetNum, sendArg->dataLen, sendArg->intervalTimeMS);

	return NULL;
}

void *ThreadRecv(void *arg)
{
	struct RecvArgs *recvArg = (struct RecvArgs*)arg;

	RecvIcmpPacket(recvArg->sockFd, recvArg->remoteAddr, recvArg->packetNum, recvArg->recvBufLen, recvArg->pCmpData);

	return NULL;
}

int InitRAWSokcet(const char *protocol)
{
	int sockFd;

	/* create socket type of SOCK_RAW need root authority. */
	sockFd = socket(AF_INET, SOCK_RAW, getprotobyname(protocol)->p_proto);
	if (-1 == sockFd)
	{
		perror("socket fail");
		return -1;
	}

	setuid(getuid());

	/* set socket option */
	int recvBufLen;
	socklen_t valueLen;

	if (0 != getsockopt(sockFd, SOL_SOCKET, SO_RCVBUF, (void*)&recvBufLen, &valueLen))
	{
		perror("getsockopt fail");
	}
	else
	{
		printf("socket default receive buffer length is %d\n", recvBufLen);
	}

	recvBufLen = 1024 * 10;	/* 10KB */
	if (0 != setsockopt(sockFd, SOL_SOCKET, SO_RCVBUF, &recvBufLen, sizeof(recvBufLen)))
	{
		perror("setsockopt fail");
	}

	if (0 != getsockopt(sockFd, SOL_SOCKET, SO_RCVBUF, (void*)&recvBufLen, &valueLen))
	{
		perror("getsockopt fail");
	}
	else
	{
		printf("after set socket receive buffer length is %d\n", recvBufLen);
	}

	return sockFd;
}

void SendIcmpPacket(int sockFd, struct sockaddr_in *remoteAddr, int packetNum, int dataLen, int intervalTimeMS)
{
	int idx;
	int packetLen = ICMP_HEAD_LEN + dataLen;
	int sendLen;

	gICMPPacket = (unsigned char*)malloc(sizeof(char) * packetLen);
	if (NULL == gICMPPacket)
	{
		perror("malloc fail"), exit(errno);
	}

	gpDataBuf = gICMPPacket + ICMP_HEAD_LEN;

	for (idx = 0; idx < dataLen; ++idx)
	{
		gpDataBuf[idx] = 'a' + (idx % 26);	/* 26 letter */
	}

	for (idx = 0; idx < packetNum; ++idx)
	{
		SetIcmpHead(gICMPPacket, dataLen, idx, gPid);

		sendLen = sendto(sockFd, gICMPPacket, packetLen, 0, (struct sockaddr*)remoteAddr, sizeof(struct sockaddr_in));
		if (sendLen != packetLen)
		{
			printf("sendto fail\n");
			continue;
		}
		else
		{
			printf("sendto success:[%d]\n", idx);
		}

		usleep(1000 * intervalTimeMS);

		if (!gResFlag)
		{
			for (int i = 0; i < 4; ++i)
			{
				if (!gResFlag)
				{
					usleep(1000 * intervalTimeMS);
				}
				else
				{
					break;
				}
			}
			if (!gResFlag)
			{
				printf("icmp response overtime.\n");
			}
		}
		gResFlag = false;
	}
}

void RecvIcmpPacket(int sockFd, struct sockaddr_in *pRemoteAddr, int packetNum, int recvBufLen, unsigned char *pCmpData)
{
	int idx;
	unsigned char *pRecvBuf = NULL;
	int recvLen;
	struct timeval recvTime;
	struct sockaddr_in remoteAddr;
	socklen_t sockLen;
	printf("sockFd=%d addr=%s packetNum=%d recvBufLen=%d\n", sockFd, inet_ntoa(remoteAddr.sin_addr), packetNum, recvBufLen);

	pRecvBuf = (unsigned char*)malloc(sizeof(unsigned char) * recvBufLen);
	if (NULL == pRecvBuf)
	{
		perror("malloc fail"), exit(errno);
	}

	for (idx = 0; idx < packetNum; ++idx)
	{
		sockLen = sizeof(remoteAddr);

		recvLen = recvfrom(sockFd, pRecvBuf, recvBufLen, 0, (struct sockaddr*)&remoteAddr, &sockLen);
		if (recvLen > 0)
		{
			if (remoteAddr.sin_addr.s_addr == pRemoteAddr->sin_addr.s_addr)
			{
				gResFlag = true;
				gettimeofday(&recvTime, NULL);
				UnpackIcmpPacket(pRecvBuf, recvLen, &recvTime, &remoteAddr.sin_addr);
			}
		}
		else
		{
//			printf("ping not recv.\n");
		}
	}
}

int SetIcmpHead(unsigned char *icmpBuf, int dataLen, int packetSno, int pid)
{
	struct icmp *icmpHead;
	icmpHead = (struct icmp*)icmpBuf;

	int icmpPacketLen = ICMP_HEAD_LEN + dataLen;

	icmpHead->icmp_type		= ICMP_ECHO;
	icmpHead->icmp_code		= 0;
	icmpHead->icmp_cksum	= 0;
	icmpHead->icmp_seq		= packetSno;
	icmpHead->icmp_id		= pid;

	struct timeval *tVal;
	tVal = (struct timeval*)icmpHead->icmp_data;
	gettimeofday(tVal, NULL);

	/* checkout sum */
	icmpHead->icmp_cksum = CalcCheckSum((unsigned short*)icmpBuf, icmpPacketLen);


	return 0;
}

int UnpackIcmpPacket(unsigned char *pBufData, int bufLen, struct timeval *recvTime, struct in_addr *sourceAddr)
{
	int				ipHeaderLen;
	struct ip 		*pIPHeader;
	struct icmp 	*pICMPHeader;
	struct timeval	*timeSend;
	double 			rtt;

	/* 求ip报头长度,即ip报头的长度标志乘4,头长度指明头中包含的4字节字的个数。可接受的最小值是5，最大值是15 */
	pIPHeader = (struct ip*)pBufData;
	ipHeaderLen = pIPHeader->ip_hl<<2;

	pICMPHeader = (struct icmp*)(pBufData + ipHeaderLen);

	bufLen -= ipHeaderLen;	/*ICMP报头及ICMP数据报的总长度*/
	if (bufLen < 8)			/*小于ICMP报头长度则不合理*/
	{
		printf("ICMP packet length is less than 8.\n");
		return -1;
	}
	/*确保所接收的是我所发的的ICMP的回应*/
	if ((pICMPHeader->icmp_type == ICMP_ECHOREPLY) && (pICMPHeader->icmp_id == gPid))
	{
		/* 接收和发送的时间差, 以毫秒为单位计算 */
		timeSend = (struct timeval*)pICMPHeader->icmp_data;
		rtt = (recvTime->tv_sec  - timeSend->tv_sec) * 1000 +
			  (recvTime->tv_usec - timeSend->tv_usec) / 1000;
		/*显示相关信息*/
		printf("from %s response: byte=%d seq=%u ttl=%d rtt=%.3f ms\n",
				inet_ntoa(*sourceAddr), bufLen-ICMP_HEAD_LEN, pICMPHeader->icmp_seq, pIPHeader->ip_ttl, rtt);

		return 0;
	}

	return -1;
}

unsigned short CalcCheckSum(unsigned short *bufAddr, int len)
{
	int nleft = len;
	int sum = 0;
	unsigned short *w = bufAddr;
	unsigned short answer = 0;

	while (nleft > 1)
	{
		sum		+= *w++;
		nleft	-= 2;
	}

	if (1 == nleft)
	{
		*(unsigned char*)(&answer) = *(unsigned char*)w;
		sum+=answer;
	}
	sum = (sum>>16)+(sum&0xffff);
	sum += (sum>>16);
	answer =~ sum;
	return answer;
}


int ShowResult()
{
	printf("--- %s ping statistics ---\n", );
//	printf("%d packets transmitted, %d received, %d% packet loss, time ms\n");
}

#endif /* PING_CPP */
/* end file */
