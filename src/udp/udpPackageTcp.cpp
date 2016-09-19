/*
 * udpPackageTcp.cpp
 *
 *  Created on: 2015-7-22
 *      Author: hongjianan
 */
#include "../config.h"
#if UDPPACKAGETCP_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

#include <utmpx.h>
#define _GNU_SOURCE
#include <sched.h>
#include <pthread.h>

#define PORT_SEND	12345
#define PORT_RECV	54321

#pragma pack(1)

struct IpHeader
{
	unsigned char	version		: 4;
	unsigned char	length		: 4;
	unsigned char	serviceType;
	unsigned short	totalLen;
	unsigned short	ipId;
	unsigned short	fragOffSet;
	unsigned char	ttl;
	unsigned char	proto;
	unsigned short	checkSum;
	unsigned short	srcPort;
	unsigned short	dstPort;
};

struct TcpHeader
{
	unsigned int	seqSn;
	unsigned int 	ackSn;
	unsigned short	headerLen	: 4;
	unsigned short	flag		: 4;
	unsigned short	winSize;
	unsigned short	checkSum;
};

struct ip;
struct tcphdr

const char *g_localIP = "127.0.0.1";

int gPacketSend = 0;
int gPacketRecv = 0;
int gPacketError = 0;
FILE *gErrFd = NULL;
FILE *gSnoFd = NULL;

void SigHandler(int signo);
bool SetCpu(int cpuNo);
void TaskSetCpu(const char *programName, int cpuNo);
bool GetCpuMask(pid_t pID, int *cpuNo);
bool SetPrio(int prio);

int InitInetSocket(const char *localIP, const unsigned int localPort, const char *protol);

int UdpTest_Receive(int argc, char *argv[]);
int UdpTest_Send(int argc, char *argv[]);

/*================main=============*/
int UdpPackageTcp(int argc, char *argv[])
{
	int ret;

	ret = UdpPackageTcp_Receive(argc, argv);
//	ret = UdpPackageTcp_Send(argc, argv);

	return ret;
}


int UdpPackageTcp_Receive(int argc, char *argv[])
{
	if (4 != argc)
	{
		printf("usage:[localIP] [cpuNo string] [prio]\n");
		exit(-1);
	}
	char *programName;
	programName = strrchr(argv[0], '/') + 1;

	char localIP[16] = {0};
	memcpy(localIP, argv[1], strlen(argv[1]));
	int cpuNoStr = atoi(argv[2]);
	int prio = atoi(argv[3]);
	int cpuNo;

	if (SIG_ERR == signal(SIGINT, SigHandler))
	{
		perror("signal fail");
		exit(-1);
	}

	if (11 != cpuNoStr)
	{
#if 1
		TaskSetCpu(programName, cpuNoStr);
#else
		for (; cpuNoStr > 0; cpuNoStr /= 10)
		{
			cpuNo = cpuNoStr % 10;
			SetCpu(cpuNo);
		}
#endif
	}
	if (prio > 0)
	{
		SetPrio(prio);
	}

	usleep(100000);

	int udpFd = InitInetSocket(localIP, PORT_RECV, "udp");
	if (-1 == udpFd)
    {
        printf("InitInetSocket fail\n");
        exit(-1);
    }

	/* 3. recv message */
	gSnoFd = fopen("sno.log", "wb");
	gErrFd = fopen("err.log", "wb");

	unsigned char newSno = 0;
	unsigned char oldSno = 0xff;
	unsigned char oldCpuNo = 0;
	int recvLen;
	char recvBuf[4096] = {0};
	struct sockaddr_in addrSend;	/* to save send address */
	/* two effects: 1. incoming length of addrSend; 2. save length of addrSend return */
	socklen_t len;

	int i;
	for (i = 0; ; ++i)
	{
		len = sizeof(addrSend);
		recvLen = recvfrom(udpFd, (void *)recvBuf, sizeof(recvBuf) - 1, 0, (struct sockaddr *)&addrSend, &len);

		if (recvLen > 0)
		{
			++gPacketRecv;
			newSno = recvBuf[0];
			if ((unsigned char)(oldSno + 1) != newSno)
			{
				++gPacketError;
				fprintf(gErrFd, "[%d]: oldSno[%u] newSno[%u] oldCpu[%u] curCpu[%u]\n",
						i, oldSno, newSno, oldCpuNo, recvBuf[1]);
			}
			oldSno	 = newSno;
			oldCpuNo = recvBuf[1];

			/* print sno to file */
			if (0 == (newSno % 10))
			{
				fprintf(gSnoFd, "\n");
			}
			fprintf(gSnoFd, "%03u(%u) ", newSno, oldCpuNo);
			if (0xff == newSno)
			{
				fprintf(gSnoFd, "\n");
			}


		}
		else if (0 == recvLen)
		{
			printf("net down.\n");
			break;
		}
		else if (-1 == recvLen)
		{
			printf("net broken.\n");
			break;
		}

	}/* while (1) */

	/* 4. close socket file */
	close(udpFd);

	return 0;
}


int UdpTest_Send(int argc, char *argv[])
{
	if (7 != argc)
	{
		printf("usage:[localIP] [remoteIP] [packetNum] [sleepTimeUS] [cpuNo] [prio]\n");
		exit(-1);
	}
	char *programName;
	programName = strrchr(argv[0], '/') + 1;

	char localIP[16]  = {0};
	char remoteIP[16] = {0};
	int packetNum = atoi(argv[3]);
	int sleepTimeUS = atoi(argv[4]);
	memcpy(localIP, argv[1], strlen(argv[1]));
	memcpy(remoteIP, argv[2], strlen(argv[2]));

	int cpuNoStr = atoi(argv[5]);
	int prio  	 = atoi(argv[6]);
	int cpuNo;

	if (11 != cpuNoStr)
	{
#if 1
		TaskSetCpu(programName, cpuNoStr);
#else
		for (; cpuNoStr > 0; cpuNoStr /= 10)
		{
			cpuNo = cpuNoStr % 10;
			SetCpu(cpuNo);
		}
#endif
	}
	if (prio > 0)
	{
		SetPrio(prio);
	}
	usleep(100000);

	int udpFd = InitInetSocket(localIP, PORT_SEND, "udp");
	if (-1 == udpFd)
	{
	    printf("InitInetSocket fail\n");
	    exit(-1);
	}

	/* 3. set destination address */
	struct sockaddr_in addrDest;
	addrDest.sin_family = AF_INET;
	addrDest.sin_addr.s_addr = inet_addr(remoteIP);
	addrDest.sin_port = htons(PORT_RECV);

	/* 4. send message */
	if (-1 == connect(udpFd, (struct sockaddr *)&addrDest, sizeof(addrDest)))
	{
		close(udpFd);
		perror("connect failed"), exit(-1);
	}

	char sendBuf[1200];
	memset(sendBuf, 'h', sizeof(sendBuf));
	int sendLen = sizeof(sendBuf);

	struct timeval beginTime, endTime;
	gettimeofday(&beginTime, NULL);

	int idx;
	unsigned char curCpuNo;
	for (idx = 0; idx <= packetNum; idx++)
	{
		sendBuf[0] = idx % 256;
		sendBuf[1] = (unsigned char)sched_getcpu();	/* 获取当前线程运行所在cpu */

		sendLen = send(udpFd, sendBuf, sendLen, 0);
		if (sendLen >= 0)
		{
			++gPacketSend;
//			printf("sendto success.\n");
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


bool SetCpu(int cpuNo)
{
	cpu_set_t mask;  //processor 0
	CPU_ZERO(&mask);
	CPU_SET(cpuNo, &mask);

	if (0 != sched_setaffinity(0, sizeof(mask), &mask))
	{
		perror("sched_setaffinity fail");
		return false;
	}

	return true;
}

bool SetPrio(int prio)
{
	struct sched_param param;
	param.sched_priority = prio;

	if (0 != sched_setscheduler(0, SCHED_FIFO, &param))
	{
		perror("sched_setscheduler fail");
		return false;
	}

	return true;
}

void TaskSetCpu(const char *programName, int cpuNo)
{
	char command[64] = {0};

	sprintf(command, "taskset -p -c %d `pgrep %s`", cpuNo, programName);
	system(command);

	sprintf(command, "taskset -p `pgrep %s`", programName);
	system(command);
}

void SigHandler(int signo)
{
	fflush(gErrFd);
	fflush(gSnoFd);
	fclose(gErrFd);
	fclose(gSnoFd);

	printf("total received packet %d, error packet %d.\n", gPacketRecv, gPacketError);

	if (SIG_ERR == signal(signo, SIG_DFL))
	{
		perror("signal SIGINT fail"), exit(-1);
	}
	raise(signo);
}

#endif /* UDPTEST_CPP */
/* end file */
