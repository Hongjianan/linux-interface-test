/*
 * udpTest.cpp
 *
 *  Created on: Dec 20, 2014
 *      Author: hong
 */
#include "../config.h"
#if UDPORDERSENDRECV_CPP

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

#include <utmpx.h>
#define _GNU_SOURCE
#include <sched.h>
#include <pthread.h>

//#define PRINT_RIGHT_SNO

#define MAX_SEND_THREAD_NUM		18
#define RANDOM_NUMBER_LOOP		10000

struct TxAllStatistics
{
	unsigned long long	txAllPacket;
};

struct TxAllStatisticsPer
{
	unsigned long long	txAllPacketPer;
	unsigned long long	txAllBytePer;
	char				isNeedClearData;
};


struct RxAllStatistics
{
	unsigned long long	rxAllPacket;
	unsigned long long	rxAllByte;
	uint32_t		rxAllError;
};

struct RxAllStatisticsPer
{
	unsigned long long	rxAllPacketPer;
	unsigned long long	rxAllBytePer;
	uint32_t		rxAllErrorPer;
};

struct RxThdStatistics
{
	uint32_t expectSno;
	uint32_t rxPaceket;

	unsigned long long rxByte;
	uint32_t rxError;
};


FILE *gErrFd = NULL;

int gPrintTimeS  = 0;
int gSleepTimeUS = 0;
int gMinPackLen	 = 0;
int gMaxPackLen	 = 0;
int gOpenRightSnoRecord = 0;
int gSendThreadIdx;

/* random */
uint16_t gRandSendLen[RANDOM_NUMBER_LOOP];

/* KPI */
/* send statistics */
struct TxAllStatistics		gTxAllData			= { 0 };
struct TxAllStatisticsPer	gTxAllDataPer[2]	= { 0 };
volatile struct TxAllStatisticsPer	*gpTxAllDataPer = &gTxAllDataPer[0];

/* receive statistics*/
struct RxAllStatistics		gRxAllData = { 0 };
struct RxAllStatisticsPer	gRxAllDataPer[2] = { 0 };
volatile struct RxAllStatisticsPer	*gpRxAllDataPer = &gRxAllDataPer[0];

struct RxThdStatistics		gRxThdData[MAX_SEND_THREAD_NUM] = { {0} };

static unsigned char		gDataPerIdx = 0;
static const unsigned char	gDataPerIdxChange[2] = {1, 0};

static inline unsigned char GerCurDataPerIdx(void);
static inline unsigned char GerBakDataPerIdx(void);

static inline struct TxAllStatisticsPer* GerTxCurDataPer(void);
static inline struct TxAllStatisticsPer* GerTxBakDataPer(void);
static inline void UpdateTxDataPer(void);

static inline struct RxAllStatisticsPer* GerRxCurDataPer(void);
static inline struct RxAllStatisticsPer* GerRxBakDataPer(void);
static inline void UpdateRxDataPer(void);

void RxSigHandler(int signo);
void TxSigHandler(int signo);
bool SetCpu(int cpuNo);
void TaskSetCpu(const char *programName, int cpuNo);
bool GetCpuMask(pid_t pID, int *cpuNo);
bool SetPrio(int prio);
int  InitCheckSno(void);
void CheckSno(uint16_t newTid, uint32_t newSno);

int InitInetSocket(const char *localIP, const uint32_t localPort, const char *protol);
int ConnectServer(int clientFd, const char *serverIP, const uint16_t serverPort);

char* PrepareSendBuf(int dataLen);
void  InitRandomLenBuf(uint16_t minLen, uint16_t maxLen);
static inline uint16_t GetSendLen(void);

void *ThreadRecvCount(void *arg);
void *ThreadSendCount(void *arg);

int UdpOrderSendRecv_Rx(int argc, char *argv[]);
int UdpOrderSendRecv_Tx(int argc, char *argv[]);
/*================main=============*/
int UdpOrderSendRecv(int argc, char *argv[])
{
	int ret;

//	ret = UdpOrderSendRecv_Rx(argc, argv);
	ret = UdpOrderSendRecv_Tx(argc, argv);

	return ret;
}

int UdpOrderSendRecv_Rx(int argc, char *argv[])
{
	if (8 != argc)
	{
		printf("usage:[localIP] [localPort] [isRightRecord] [cpuNo string] [prio] [sleepUS] [printTimeS]\n");
		printf("cpuNo string = 11 ignore, prio = 0 ignore.\n");
		exit(-1);
	}
	char *programName;
	programName = strrchr(argv[0], '/') + 1;

	char localIP[16] = {0};
	memcpy(localIP, argv[1], strlen(argv[1]));
	uint16_t localPort = atoi(argv[2]);
	gOpenRightSnoRecord = atoi(argv[3]);
	int cpuNoStr = atoi(argv[4]);
	int prio	 = atoi(argv[5]);
	gSleepTimeUS = atoi(argv[6]);
	gPrintTimeS  = atoi(argv[7]);

	if (SIG_ERR == signal(SIGINT, RxSigHandler))
	{
		perror("signal fail");
		exit(-1);
	}

	if (SIG_ERR == signal(SIGTERM, RxSigHandler))
	{
		perror("signal fail");
		exit(-1);
	}

	/* bind cpu */
	if (11 != cpuNoStr)
	{
		int cpuNo;
#if 0
		TaskSetCpu(programName, cpuNoStr);
#else
		for (; cpuNoStr > 0; cpuNoStr /= 10)
		{
			cpuNo = cpuNoStr % 10;
			SetCpu(cpuNo);
		}
#endif
	}

	/* set prio */
	if (prio > 0)
	{
		SetPrio(prio);
	}

	int udpFd = InitInetSocket(localIP, localPort, "udp");
	if (-1 == udpFd)
    {
        printf("InitInetSocket fail\n");
        exit(-1);
    }

	if (InitCheckSno())
	{
		fprintf(stderr, "InitCheckSno() fail\n");
		return -1;
	}

	pthread_t tmp;
	pthread_create(&tmp, NULL, ThreadRecvCount, NULL);

	/* 3. recv message */
	int recvLen;
	char recvBuf[4096] = {0};
	struct sockaddr_in addrSend;	/* to save send address */
	/* two effects: 1. incoming length of addrSend; 2. save length of addrSend return */
	socklen_t len;

	uint16_t	*pThreadIdx	= (uint16_t*)recvBuf;
	uint32_t	*pSno		= (uint32_t*)(pThreadIdx + 1);

	uint16_t	threadIdx;
	uint32_t	newSno;

	int loop;
	for (loop = 0; ; ++loop)
	{
		len = sizeof(addrSend);
		recvLen = recvfrom(udpFd, (void *)recvBuf, sizeof(recvBuf) - 1, 0, (struct sockaddr *)&addrSend, &len);

		if (recvLen > 0)
		{
			++gpRxAllDataPer->rxAllPacketPer;
			gpRxAllDataPer->rxAllBytePer += recvLen;

			threadIdx = ntohs(*pThreadIdx);
			newSno = ntohl(*pSno);

			CheckSno(threadIdx, newSno);
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

		if (gSleepTimeUS)
		{
			usleep(gSleepTimeUS);
		}

	}/* for (loop = 0; ; ++loop) */

	/* 4. close socket file */
	close(udpFd);

	return 0;
}

int UdpOrderSendRecv_Tx(int argc, char *argv[])
{
	int idx;
	if (12 != argc)
	{
		printf("usage:[localIP] [localPort] [remoteIP] [remotePort] [cpuNo] [prio] [minPackLen] [maxPackLen] [packetNum] [sleepUS] [threadIdx]\n");
		printf("      cpuNo=11, don't bind cpu; prio=0, don't set prio.\n");
		exit(-1);
	}
	char *programName;
	programName = strrchr(argv[0], '/') + 1;

	char localIP[16]  = {0};
	char remoteIP[16] = {0};
	uint16_t localPort, remotePort;

	memcpy(localIP,  argv[1], strlen(argv[1]));
	memcpy(remoteIP, argv[3], strlen(argv[3]));
	localPort  = atoi(argv[2]);
	remotePort = atoi(argv[4]);

	int cpuNoStr	= atoi(argv[5]);
	int prio		= atoi(argv[6]);
	gMinPackLen		= atoi(argv[7]);
	gMaxPackLen		= atoi(argv[8]);
	int packetNum	= atoi(argv[9]);
	int sleepTimeUS	= atoi(argv[10]);
	gSendThreadIdx	= atoi(argv[11]);

	if (SIG_ERR == signal(SIGINT, TxSigHandler))
	{
		perror("signal fail");
		exit(-1);
	}

	if (SIG_ERR == signal(SIGTERM, TxSigHandler))
	{
		perror("signal fail");
		exit(-1);
	}

	pthread_t tmp;
	if (pthread_create(&tmp, NULL, ThreadSendCount, NULL))
	{
		fprintf(stderr, "pthread_create() fail.\n");
		return -1;
	}

	if (0 == gMaxPackLen)
	{
		printf("gMaxPackLen too short.\n"), exit(-1);
	}

	int cpuNo;
	if (11 != cpuNoStr)
	{
#if 0
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

	char *pSendBuf = PrepareSendBuf(gMaxPackLen);
	InitRandomLenBuf(gMinPackLen, gMaxPackLen);

	int sendLen;
	uint16_t	*pIdx = (uint16_t*)pSendBuf;
	uint32_t	*pSno = (uint32_t*)(pIdx + 1);

	*pIdx = htons(gSendThreadIdx);

	uint32_t sno;
	for (sno = 0; (0==packetNum) || (sno < packetNum); ++sno)
	{
		/* fill sno */
		*pSno = htonl(sno);

		sendLen = GetSendLen();

		sendLen = send(udpFd, pSendBuf, sendLen, 0);
		if (sendLen >= 0)
		{
			++gpTxAllDataPer->txAllPacketPer;
			++gTxAllData.txAllPacket;
			gpTxAllDataPer->txAllBytePer += sendLen;
		}
		else if (-1 == sendLen)
		{
			perror("sendto failed");
			break;
		}

		if (sleepTimeUS)
		{
			usleep(sleepTimeUS);
		}
	}

	/* 5. close socket file */
	close(udpFd);

	printf("Total send [%d] packet\n", gTxAllData.txAllPacket);

	return 0;
}

char* PrepareSendBuf(int dataLen)
{
	if (!dataLen)
		return NULL;

	char* pSendBuf;
	pSendBuf = (char*)malloc(sizeof(char) * (dataLen + 1));
	if (NULL == pSendBuf)
	{
		perror("malloc fail"), exit(-1);
	}

	int idx;
	for (idx = (2 + 4); idx < dataLen; ++idx)
	{
		pSendBuf[idx] = 'a' + (idx % 25);
	}
	pSendBuf[dataLen - 1] = '\0';

	return pSendBuf;
}

void InitRandomLenBuf(uint16_t minLen, uint16_t maxLen)
{
	if (minLen > maxLen)
	{
		printf("InitRandomLenBuf failed, minLen > maxLen\n");
		exit(-1);
	}

	uint32_t idx;
	uint16_t deltaLen = maxLen - minLen;

	if (0 == deltaLen)
	{
		for (idx = 0; idx < RANDOM_NUMBER_LOOP; ++idx)
		{
			gRandSendLen[idx] = minLen;
		}
	}
	else
	{
		srandom(521);

		for (idx = 0; idx < RANDOM_NUMBER_LOOP; ++idx)
		{
			gRandSendLen[idx] = minLen + (random() % deltaLen);
		}
	}
}


static inline uint16_t GetSendLen(void)
{
	static uint16_t sIdx = 0;

	if (RANDOM_NUMBER_LOOP == sIdx)
	{
		sIdx = 0;
	}

	return gRandSendLen[sIdx++];
}


#define UPDATE_CONFLICT_TIME_S	1
#define COUNT_MBPS	(8.0 / 1024 / 1024)

static inline unsigned char GerCurDataPerIdx(void);
static inline unsigned char GerBakDataPerIdx(void);

static inline struct TxAllStatisticsPer* GerTxCurDataPer(void);
static inline struct TxAllStatisticsPer* GerTxBakDataPer(void);
static inline void UpdateTxDataPer(void);

static inline struct RxAllStatisticsPer* GerRxCurDataPer(void);
static inline struct RxAllStatisticsPer* GerRxBakDataPer(void);
static inline void UpdateRxDataPer(void);

void *ThreadRecvCount(void *arg)
{
	struct RxAllStatisticsPer *rxSaveCurDataPer;

	FILE *pFile = fopen("recordRecv.log", "wb");
	int sleepTimeS = gPrintTimeS - UPDATE_CONFLICT_TIME_S;

	float perRxRate;
	int loop;
	for (loop = 0; ; ++loop)
	{
		rxSaveCurDataPer = GerRxCurDataPer();
		UpdateRxDataPer();
		sleep(UPDATE_CONFLICT_TIME_S);

		gRxAllData.rxAllPacket	+= rxSaveCurDataPer->rxAllPacketPer;
		gRxAllData.rxAllByte	+= rxSaveCurDataPer->rxAllBytePer;
		gRxAllData.rxAllError	+= rxSaveCurDataPer->rxAllErrorPer;
		perRxRate = rxSaveCurDataPer->rxAllBytePer * COUNT_MBPS / gPrintTimeS;

		fprintf(stderr, "recv: loop[%d] pack[%llu] [%5.1f]Mbps err[%u]\n",
				loop, rxSaveCurDataPer->rxAllPacketPer, perRxRate, rxSaveCurDataPer->rxAllErrorPer);

		fprintf(pFile,  "recv: loop[%d] pack[%llu] [%5.1f]Mbps err[%u]\n",
				loop, rxSaveCurDataPer->rxAllPacketPer, perRxRate, rxSaveCurDataPer->rxAllErrorPer);

		if (sleepTimeS > 0)
		{
			sleep(sleepTimeS);
		}
	}
}

#define ALL_SLEEP_TIME_S	10
void *ThreadSendCount(void *arg)
{
	struct TxAllStatisticsPer *txSaveCurDataPer;

	FILE *pFile = fopen("recordSend.log", "wb");
	int sleepTimeS = ALL_SLEEP_TIME_S - UPDATE_CONFLICT_TIME_S;

	float perRxRate;

	int loop;
	for (loop = 0; ; ++loop)
	{
		txSaveCurDataPer = GerTxCurDataPer();
		UpdateTxDataPer();
		sleep(UPDATE_CONFLICT_TIME_S);

		perRxRate = txSaveCurDataPer->txAllBytePer * COUNT_MBPS / ALL_SLEEP_TIME_S;

		fprintf(stderr, "sendTid[%d]: loop[%d] pack[%llu] [%5.1f]Mbps\n",
				gSendThreadIdx, loop, txSaveCurDataPer->txAllPacketPer, perRxRate);

		fprintf(pFile,  "sendTid[%d]: loop[%d] pack[%llu] [%5.1f]Mbps\n",
				gSendThreadIdx, loop, txSaveCurDataPer->txAllPacketPer, perRxRate);

		sleep(sleepTimeS);
	}
}

int InitInetSocket(const char *localIP, const uint32_t localPort, const char *protol)
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

    int recvBufSize;
    socklen_t valLen = 4;
	if(getsockopt(sockFd, SOL_SOCKET, SO_RCVBUF,(char*)&recvBufSize, &valLen)< 0)
	{

	}
	printf("recvBufSize = %d\n", recvBufSize);
	fflush(stdout);
	exit(-1);

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

int ConnectServer(int clientFd, const char *serverIP, const uint16_t serverPort)
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

void RxSigHandler(int signo)
{
	fflush(gErrFd);
	fclose(gErrFd);

	printf("total received packet[%llu], error[%d].\n", gRxAllData.rxAllPacket, gRxAllData.rxAllError);
	fflush(stdout);

	if (SIG_ERR == signal(signo, SIG_DFL))
	{
		perror("signal SIGINT fail"), exit(-1);
	}
	raise(signo);
}

void TxSigHandler(int signo)
{
	printf("total send [%llu] packet\n", gTxAllData.txAllPacket);

	if (SIG_ERR == signal(signo, SIG_DFL))
	{
		perror("signal SIGINT fail"), exit(-1);
	}
	raise(signo);
}


/* check sno */
int OpenFile(void)
{
	char fileName[32];

	sprintf(fileName, "errRecv.log");
	gErrFd = fopen(fileName, "wb");
	if (NULL == gErrFd)
	{
		perror("perror fopen");
		return -1;
	}

	return 0;
}

int InitCheckSno(void)
{
	if (OpenFile())
	{
		fprintf(stderr, "OpenFile error\n");
		return -1;
	}

	return 0;
}

void CheckSno(uint16_t newTid, uint32_t newSno)
{
	if (newTid >= MAX_SEND_THREAD_NUM)
	{
		fprintf(stderr, "==========[ERROR]: tid[%d] newSno[%d]\n", newTid, newSno);
		fprintf(gErrFd, "==========[ERROR]: tid[%d] newSno[%d]\n", newTid, newSno);
		return;
	}

	if (gRxThdData[newTid].expectSno != newSno)
	{
		++gpRxAllDataPer->rxAllErrorPer;
		++gRxThdData[newTid].rxError;

		fprintf(gErrFd, "=ERR=: tid[%d] recvNum[%u] exceptSno[%u] newSno[%u]\n",
				newTid, gRxThdData[newTid].rxPaceket, gRxThdData[newTid].expectSno, newSno);
		fflush(gErrFd);
	}
	else
	{
		if (gOpenRightSnoRecord)
		{
			fprintf(gErrFd, "right: tid[%d] recvNum[%u] exceptSno[%u] newSno[%u]\n",
					newTid, gRxThdData[newTid].rxPaceket, gRxThdData[newTid].expectSno, newSno);
			fflush(gErrFd);
		}
	}

	++gRxThdData[newTid].rxPaceket;
	gRxThdData[newTid].expectSno = newSno + 1;

}

/* KPI */
static inline unsigned char GerCurDataPerIdx(void)
{
	return gDataPerIdx;
}

static inline unsigned char GerBakDataPerIdx(void)
{
	return gDataPerIdxChange[gDataPerIdx];
}

static inline struct TxAllStatisticsPer* GerTxCurDataPer(void)
{
	return &gTxAllDataPer[GerCurDataPerIdx()];
}

static inline struct TxAllStatisticsPer* GerTxBakDataPer(void)
{
	return &gTxAllDataPer[GerBakDataPerIdx()];
}

static inline void UpdateTxDataPer(void)
{
	bzero(GerTxBakDataPer(), sizeof(struct TxAllStatisticsPer));
	gDataPerIdx		= GerBakDataPerIdx();
	gpTxAllDataPer	= GerTxCurDataPer();
	__sync_synchronize();
}

static inline struct RxAllStatisticsPer* GerRxCurDataPer(void)
{
	return &gRxAllDataPer[GerCurDataPerIdx()];
}

static inline struct RxAllStatisticsPer* GerRxBakDataPer(void)
{
	return &gRxAllDataPer[GerBakDataPerIdx()];
}

static inline void UpdateRxDataPer(void)
{
	bzero(GerRxBakDataPer(), sizeof(struct RxAllStatisticsPer));
	gDataPerIdx		= GerBakDataPerIdx();
	gpRxAllDataPer	= GerRxCurDataPer();
	__sync_synchronize();
}

#endif /* UDPORDERSENDRECV_CPP */
/* end file */
