/*
 * tcpEchoServerSelect.cpp
 *
 *  Created on: 2015-7-24
 *      Author: Hong
 */
#include "../config.h"
#if TCP_ECHO_SERVER_SELECT_CPP

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
#include <sys/time.h>
#include <sys/select.h>
#include <sys/fcntl.h>

#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define PORT_SERVER		12345

const char *gLocalIP = "192.168.0.111";
int gMaxFd;

void *ThreadRecv(void *arg);

int InitInetSocket(const char *localIp, unsigned short localPort, const char *proto);
int ConnectServer(int sockedFd, const char *ip, unsigned int port);
int ServerRecv(fd_set *pFdSet);

int TcpEchoServerSelect_Client(int argc, char *argv[]);
int TcpEchoServerSelect_Server(int argc, char *argv[]);

/*=============main==============*/
int TcpEchoServerSelect(int argc, char *argv[])
{
	int ret;

//	ret = TcpEchoServerSelect_Client(argc, argv);
	ret = TcpEchoServerSelect_Server(argc, argv);

	return ret;
}


int TcpEchoServerSelect_Server(int argc, char *argv[])
{
	/* 1. create server socket */
	int serverFd = InitInetSocket(gLocalIP, PORT_SERVER, "tcp");
	if (-1 == serverFd)
	{
		printf("CreateInetSocket fail.\n"), exit(-1);
	}

	//ÉèÎª·Ç×èÈû
#if 0
	if (-1 == fcntl(serverFd, F_SETFL, O_NONBLOCK))
	{
		printf("Set server socket nonblock failed\n");
		exit(1);
	}
#endif

	/* 2. listen */
	if (-1 == listen(serverFd, 10))
	{
		printf("3.listen fail :%m");
		close(serverFd);
		exit(-1);
	}
	printf("3.listen success\n");

	/* 4. accept and recv */
	char recvBuf[1024];
	int saveClientFdSet[1024] = {-1};	/* roll array */
	int rearIdx, frontIdx = 0;
	int acceptFd;
	fd_set sockFdSet;

	for (int maxFd = serverFd + 1, minFd = serverFd + 1; ;)
	{
		struct timeval waitTime = {0, 100};
		FD_ZERO(&sockFdSet);
		FD_SET(serverFd, &sockFdSet);
		for (int idx = 0; idx < maxFd; ++idx)
		{
			if (-1 != saveClientFdSet[idx])
			{
				FD_SET(saveClientFdSet[idx], &sockFdSet);
			}
		}

		int fdCnt = select(maxFd, &sockFdSet, NULL, NULL, 0);
		if (fdCnt > 0)
		{
			if (FD_ISSET(serverFd, &sockFdSet))
			{
				struct sockaddr_in clientAddr;
				socklen_t len = sizeof(clientAddr);
				acceptFd = accept(serverFd, (struct sockaddr *)&clientAddr, &len);
				if (-1 == acceptFd)
				{
					printf("4.accept failed :%m");
					close(serverFd);
					exit(-1);
				}
				printf("[acceptFd=%d] new client connect, IP[%s] PORT[%d]\n",
						acceptFd, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
				if (acceptFd >= maxFd)
				{
					maxFd = acceptFd + 1;
				}
				saveClientFdSet[acceptFd] = acceptFd;
				--fdCnt;
			}

			for (int iTimes = 0, sockFd = 0; iTimes < fdCnt && sockFd < maxFd; ++sockFd)
			{
				if (FD_ISSET(sockFd, &sockFdSet))
				{
					++iTimes;
					while (1)
					{
						int dataLen = recv(sockFd, recvBuf, sizeof(recvBuf), MSG_DONTWAIT))
						if (dataLen > 0)
						{
							recvBuf[dataLen] = '\0';
							printf("recv length[%d]:\n%s[message end]\n", dataLen, recvBuf);
						}
						else if (0 == dataLen)
						{
							printf("[sockFd:%d]client exit\n", sockFd);
							saveClientFdSet[sockFd] = -1;
							close(sockFd);
							break;
						}
						else if (-1 == dataLen)
						{
							if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
								continue;
							printf("[sockFd:%d]net error\n", sockFd);
							saveClientFdSet[sockFd] = -1;
							close(sockFd);
							break;
						}
					}
				}
			}
		}
		else if (fdCnt < 0)
		{
			perror("select error");
			exit(-1);
		}
	}

	return 0;
}


int TcpEchoServerSelect_Client(int argc, char *argv[])
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

int ServerRecv(fd_set *pFdSet)
{
	unsigned char recvBuf[1024];
	int dataLen;
	int iTimes;
	int fdCnt;
	int sockFd;
	int oldMaxFd = 0;

	struct timeval waitTime = {0, 1000};
	for (; ;)
	{
		fdCnt = select(gMaxFd, pFdSet, NULL, NULL, &waitTime);
		if (oldMaxFd != gMaxFd)
		{
			oldMaxFd = gMaxFd;
			printf("gMaxFd = %d\n", gMaxFd);
		}
		if (fdCnt > 0)
		{
			for (iTimes = 0, sockFd = 0; iTimes < fdCnt && sockFd < gMaxFd; ++sockFd)
			{
				if (FD_ISSET(sockFd, pFdSet))
				{
					++iTimes;
					dataLen = recv(sockFd, recvBuf, sizeof(recvBuf), MSG_DONTWAIT);
					if (dataLen > 0)
					{
						recvBuf[dataLen] = '\0';
						printf("recv length[%d]:\n%s[message end]\n", dataLen, recvBuf);
					}
					else if (0 == dataLen)
					{
						printf("client exit\n");
						FD_CLR(sockFd, pFdSet);
					}
					else if (-1 == dataLen)
					{
						printf("net error\n");
						FD_CLR(sockFd, pFdSet);
						return -1;
					}
				}
			}
		}
		else if (0 == fdCnt)
		{

		}
		else
		{
			perror("select error");
			exit(-1);
		}
	}

	return 0;
}

void *ThreadRecv(void *arg)
{
	int ret;
	fd_set *pClientFdSet = (fd_set*)arg;
	ret = ServerRecv(pClientFdSet);
	if (0 != ret)
	{
		printf("ServerRecv error.\n");
		exit(-1);
	}
	else
	{
		printf("ServerRecv exit.\n");
	}

	return NULL;
}


#endif /* TCPECHOSERVERSELECT_CPP */
/* end file */
