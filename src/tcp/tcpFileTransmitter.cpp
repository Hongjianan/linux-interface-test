/*
 * fileTransmitter.cpp
 *
 *  Created on: Dec 21, 2014
 *      Author: hong
 */
#include "../config.h"
#if TCPFILETRANSMITTER_CPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT_RECEIVER		54321
#define PORT_SENDER			12345

const char *g_localIP = "192.168.0.121";

int TcpFileTransmitter_Receiver(int argc, char *argv[]);	/* server */
int TcpFileTransmitter_Sender(int argc, char *argv[]);		/* client */

/*==================main=================*/
int TcpFileTransmitter(int argc, char *argv[])
{
	int ret;

	ret = TcpFileTransmitter_Receiver(argc, argv);
//	ret = TcpFileTransmitter_Sender(argc, argv);

	return ret;
}


int TcpFileTransmitter_Sender(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("usage:[fileName]\n"), exit(-1);
	}

	int rc;
	int len;
	/* 1. create socket */
	int senderFd = socket(AF_INET, SOCK_STREAM, getprotobyname("tcp")->p_proto);
	if (-1 == senderFd)
	{
		printf("1.socket failed :%m\n"), exit(-1);
	}
	printf("1.socket success\n");

	/* 2. connect to server */
	struct sockaddr_in addrReceiver;
	addrReceiver.sin_family = AF_INET;
	inet_aton(g_localIP, &addrReceiver.sin_addr);
	addrReceiver.sin_port = htons(PORT_RECEIVER);

	rc = connect(senderFd, (struct sockaddr *)&addrReceiver, sizeof(addrReceiver));
	if (-1 == rc)
	{
		printf("2.connect to receiver failed :%m\n");
		close(senderFd);
		exit(-1);
	}
	printf("2.connect to receiver success\n");

	/* 3. open file */
	int fileSendFd;
	fileSendFd = open(argv[1], O_RDONLY);
	if (-1 == fileSendFd)
	{
		printf("3.open file failed :%m\n");
		close(senderFd);
		exit(-1);
	}
	printf("3.open file success\n");

	/* 4. send file name */
	len = strlen(argv[1]);
	rc = send(senderFd, (void *)&len, sizeof(len), 0);
	if (-1 == rc)
	{
		printf("4:send file name length failed :%m\n");
		close(fileSendFd), close(senderFd);
		exit(-1);
	}

	rc = send(senderFd, (void *)argv[1], len, 0);
	if (-1 == rc)
	{
		printf("4:send file name failed :%m\n");
		close(fileSendFd), close(senderFd);
		exit(-1);
	}

	/* 5. send file data */
	int size;
	char readBuf[128];
	while (1)
	{
		size = read(fileSendFd, readBuf, sizeof(readBuf));
		if (size > 0)
		{
			/* send data length first */
			rc = send(senderFd, (void *)&size, sizeof(size), 0);
			if (-1 == rc)
			{
				printf("5:send readBuf length failed :%m\n");
				close(fileSendFd), close(senderFd);
				exit(-1);
			}

			/* send data */
			rc = send(senderFd, (void *)readBuf, size, 0);
			if (-1 == rc)
			{
				printf("5:send readBuf failed :%m\n");
				close(fileSendFd), close(senderFd);
				exit(-1);
			}
		}
		else if (0 == size || -1 == size)
		{
			break;
		}
	}

	rc = send(senderFd, &size, sizeof(size), 0);

	close(fileSendFd), close(senderFd);
	printf("sender ending.\n");

	return 0;
}


int TcpFileTransmitter_Receiver(int argc, char *argv[])
{
	/* 1. create server socket file */
	int serverFd;
	if (-1 == (serverFd = socket(AF_INET, SOCK_STREAM, getprotobyname("tcp")->p_proto)))
		(void)printf("1.socket failed :%m\n"), exit(-1);
	(void)printf("1.socket success\n");

	/* 2. bind */
	struct sockaddr_in addrServer;
	addrServer.sin_family = AF_INET;
	addrServer.sin_addr.s_addr = inet_addr(g_localIP);
	addrServer.sin_port = htons(PORT_RECEIVER);
	if (-1 == bind(serverFd, (struct sockaddr *)&addrServer, sizeof(addrServer)))
	{
		(void)printf("2.bind failed :%m");
		close(serverFd);
		exit(-1);
	}
	(void)printf("2.bind success\n");

	/* 3. listen */
	if (-1 == listen(serverFd, 10))	/* 10 means: in the same time, */
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
		close(clientFd), close(serverFd);
		exit(-1);
	}
	(void)printf("[accept] new client connect, IP:%s \t PORT:%d\n", \
					inet_ntoa(addrClient.sin_addr), ntohs(addrClient.sin_port));

	/* 5. receive file name */
	int rc;
	char recvBuf[512] = {0};

	rc = recv(clientFd, &len, sizeof(len), MSG_WAITALL);
	rc = recv(clientFd, recvBuf, len, MSG_WAITALL);

	/* 6. create file */
	int recvFileFd;
	recvFileFd = open(recvBuf, O_RDWR | O_CREAT, 0666);
	while (1)
	{
		/* get data length */
		rc = recv(clientFd, &len, sizeof(len), MSG_WAITALL);
		if (0 == rc)
		{
			printf("client disconnect\n");
			break;
		}
		else if (-1 == rc)
		{
			printf("net broken\n");
			break;
		}

		if (0 == len)	break;

		/* get data */
		rc = recv(clientFd, recvBuf, len, MSG_WAITALL);
		if (0 == rc)
		{
			printf("client disconnect\n");
			break;
		}
		else if (-1 == rc)
		{
			printf("net broken\n");
			break;
		}

		/* write data to file */
		write(recvFileFd, recvBuf, len);
	}

	close(recvFileFd);
	close(clientFd);
	close(serverFd);

	return 0;
}

#endif /* FILETRANSMITTER_CPP */
/* end file */
