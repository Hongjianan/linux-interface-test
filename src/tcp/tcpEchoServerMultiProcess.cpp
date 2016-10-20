/*
 * tcpEchoServerMultiProcess.cpp
 *
 *  Created on: 2015-7-21
 *      Author: Hong
 */
#include "../config.h"
#if TCP_ECHO_SERVER_MULTI_PROCESS_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <wait.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#define PORT_SERVER		20001
#define PORT_CLIENT		54321

const char *gLocalIP = "127.0.0.1";

void *ThreadServerResponse(void *arg);
void sigHandler_SIGCHLD(int signo);

int InitInetSocket(const char *localIp, unsigned short localPort, const char *proto);
int ConnectServer(int sockedFd, const char *ip, unsigned int port);
int ServerRecv(const int sockFd);

int TcpEchoServerMultiProcess_Client(int argc, char *argv[]);
int TcpEchoServerMultiProcess_Server(int argc, char *argv[]);
int TcpEchoServerMultiThread_Server(int argc, char *argv[]);

/*=============main==============*/
int TcpEchoServerMultiProcess(int argc, char *argv[])
{
	int ret;

//	ret = TcpEchoServerMultiProcess_Client(argc, argv);
//	ret = TcpEchoServerMultiProcess_Server(argc, argv);
	ret = TcpEchoServerMultiThread_Server(argc, argv);

	return ret;
}


int TcpEchoServerMultiProcess_Server(int argc, char *argv[])
{
	if (0 != signal(SIGCHLD, sigHandler_SIGCHLD))
	{
		perror("signal fail"), exit(-1);
	}

	/* 1. create server socket */
	int serverFd = InitInetSocket(gLocalIP, PORT_SERVER, "tcp");
	if (-1 == serverFd)
	{
		printf("CreateInetSocket fail.\n"), exit(-1);
	}

	/* 2. listen */
	if (-1 == listen(serverFd, 10))
	{
		printf("3.listen fail :%m");
		close(serverFd);
		exit(-1);
	}
	printf("3.listen success\n");

	/* 4. accept */
	int clientFd;
	struct sockaddr_in clientAddr;
	socklen_t len;
	int childPid;

	while (1)
	{
		len = sizeof(clientAddr);
		clientFd = accept(serverFd, (struct sockaddr *)&clientAddr, &len);
		if (-1 == clientFd)
		{
			printf("4.accept failed :%m");
			close(serverFd);
			exit(-1);
		}
		printf("[accept] new client connect, IP[%s] PORT[%d]\n",
				inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

		childPid = fork();
		if (0 == childPid)
		{
			int ret;

			ret = ServerRecv(clientFd);
			close(clientFd);

			return ret;
		}

		close(clientFd);
	}

	return 0;
}


int TcpEchoServerMultiThread_Server(int argc, char *argv[])
{
	/* 1. create server socket */
	int serverFd = InitInetSocket(gLocalIP, PORT_SERVER, "tcp");
	if (-1 == serverFd)
	{
		printf("CreateInetSocket fail.\n"), exit(-1);
	}

	/* 2. listen */
	if (-1 == listen(serverFd, 10))
	{
		printf("3.listen fail :%m");
		close(serverFd);
		exit(-1);
	}
	printf("3.listen success\n");

	/* 4. accept */
	int clientFd;
	struct sockaddr_in clientAddr;
	socklen_t len;

	while (1)
	{
		len = sizeof(clientAddr);
		clientFd = accept(serverFd, (struct sockaddr *)&clientAddr, &len);
		if (-1 == clientFd)
		{
			printf("4.accept failed :%m");
			close(serverFd);
			exit(-1);
		}
		printf("[accept] new client connect, IP[%s] PORT[%d]\n",
				inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

		pthread_t threadRecv;
		pthread_create(&threadRecv, NULL, ThreadServerResponse, &clientFd);
	}

	return 0;
}


int TcpEchoServerMultiProcess_Client(int argc, char *argv[])
{
	if (3 != argc)
	{
		printf("usage: %s <sendTimes> <sleepTimeS>\n", argv[0]+2);
		exit(-1);
	}
	int sendTimes  = atoi(argv[1]);
	int sleepTimeS = atoi(argv[2]);

	/* 1. create client socket */
	int clientFd = InitInetSocket(0, 0, "tcp");
	if (-1 == clientFd)
	{
		printf("CreateInetSocket fail.\n"), exit(-1);
	}
	(void)printf("1.InitInetSocket success\n");

	/* 2. connect to server */
	if (0 != ConnectServer(clientFd, gLocalIP, PORT_SERVER))
	{
		printf("ConnectToServer fail.\n"), exit(-1);
	}
	printf("2.ConnectToServer success\n");
	sleep(1);

	/* 3. send message */
	unsigned short pid = getpid();
	char sendBuf[1024] = {0};
	sprintf(sendBuf, "[pid:%05u]:aaaaaaaa\n", pid);

	int idx;
	for (idx = 0; idx < sendTimes; idx++)
	{
		sprintf(sendBuf, "[pid:%05u][%03d]:aaaaaaaa\n", pid, idx);
		send(clientFd, (void *)sendBuf, strlen(sendBuf), 0);
	}

//	sleep(sleepTimeS);

	/* 4. close socket file */
	close(clientFd);

	return 0;
}


int InitInetSocket(const char *localIp, unsigned short localPort, const char *proto)
{
	int sockFd = socket(AF_INET, SOCK_STREAM, getprotobyname(proto)->p_proto);
	if (-1 == sockFd)
	{
		perror("socket fail"), exit(-1);
	}
	printf("create socket success.\n");

	if (NULL != localIp && 0 != localPort)
	{
		struct sockaddr_in localAddr = {0};
		localAddr.sin_family = AF_INET;
		localAddr.sin_addr.s_addr = inet_addr(localIp);
		localAddr.sin_port = htons(localPort);

		if (-1 == bind(sockFd, (struct sockaddr *)&localAddr, sizeof(localAddr)))
		{
			perror("bind fail");
			close(sockFd);
			exit(-1);
		}
		printf("bind success: ip[%s] port[%d].\n", localIp, localPort);
	}

	return sockFd;
}

int ConnectServer(int socketFd, const char *remoteIp, unsigned int remotePort)
{
	struct sockaddr_in serverAddr = {0};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(remoteIp);
	serverAddr.sin_port = htons(remotePort);

	if (-1 == connect(socketFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)))
	{
		printf("connect failed :%m\n");
		return -1;
	}
	printf("connect success\n");

	return 0;
}

int ServerRecv(const int sockFd)
{
	unsigned char recvBuf[512];
	int dataLen;
	pid_t pid = getpid();

    dataLen = recv(sockFd, recvBuf, sizeof(recvBuf), 0);
    if (dataLen > 0)
    {
        recvBuf[dataLen] = '\0';
        printf("[pid:%05u print] recv length[%d]\n  %s******\n", pid, dataLen, recvBuf);
        send(sockFd, "666", 3, 0);
        close(sockFd);
    }
    else if (0 == dataLen)
    {
        printf("[pid:%05u print]:client exit\n", pid);
    }
    else if (-1 == dataLen)
    {
        printf("net error\n");
        return -1;
    }

	return 0;
}

void *ThreadServerResponse(void *arg)
{
	static int threadCnt = 0;
	++threadCnt;

	int ret;
	int sockFd = *(int*)arg;
	ret = ServerRecv(sockFd);
	if (0 != ret)
	{
		printf("ServerRecv[%d] error.\n", threadCnt);
		exit(-1);
	}
	else
	{
		printf("ServerRecv[%d] exit.\n", threadCnt);
	}

	return NULL;
}

void sigHandler_SIGCHLD(int signo)
{
	pid_t childPid;

	childPid = wait(NULL);
	printf("[father print]: child[pid %d] exit normally.\n", childPid);

}

#endif /* TCP_ECHO_SERVER_MULTI_PROCESS_CPP */
/* end file */
