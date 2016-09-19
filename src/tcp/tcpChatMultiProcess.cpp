/*
 * tcpChatMultiProcess.cpp
 *
 *  Created on: Dec 23, 2014
 *      Author: hong
 */
#include "../config.h"
#if TCPCHATMULTIPROCESS_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <curses.h>

#define SEND_LENGTH		0

#define PORT_SERVER		12345
#define PORT_CLIENT		54321

const char *g_localIP = "192.168.0.121";


void InitUI(WINDOW *winInfo, WINDOW *winMsg);
void destroy(int &socketFd, WINDOW *winInfo, WINDOW *winMsg);

int CreateSocketFd(int &socketFd);
int BindAddr(int &socketFd, const char *ip, unsigned int port);
int ConnectToServer(int &sockedFd, const char *ip, unsigned int port);
int ListenClient(int &socketFd);
int ServerRecv(const int &socket, const struct sockaddr_in &sockaddr, void *buf, size_t size, int flags);
int ClientRecv(const int &socket, void *buf, size_t size, int flags);

int TcpChatMultiProcess_ClientCurses(int argc, char *argv[]);
int TcpChatMultiProcess_Client(int argc, char *argv[]);
int TcpChatMultiProcess_Server(int argc, char *argv[]);

/*=============main==============*/
int TcpChatMultiProcess(int argc, char *argv[])
{
	int ret;

//	ret = TcpChatMultiProcess_ClientCurses(argc, argv);

	ret = TcpChatMultiProcess_Client(argc, argv);
//	ret = TcpChatMultiProcess_Server(argc, argv);

	return ret;
}


int TcpChatMultiProcess_ClientCurses(int argc, char *argv[])
{
	int clientSocketFd = 0;
	CreateSocketFd(clientSocketFd);
	ConnectToServer(clientSocketFd, g_localIP, PORT_SERVER);

	WINDOW *winInfo = NULL, *winMsg = NULL;
	InitUI(winInfo, winMsg);

	pid_t childPid = fork();
	if (0 == childPid) /* into child process : input characters, send message */
	{
		int rc;
		char sendBuf[1024] = {0};
		while (1)
		{
			rc = mvwgetstr(winMsg, 1, 1, sendBuf);
			sendBuf[rc] = '\0';
			send(clientSocketFd, sendBuf, rc, 0);
#if 0
	//		mvwaddstr(winMsg, 1, 1, sendBuf);
			refresh();
			wrefresh(winMsg);
			wmove(winMsg, 1, 1);
#else
			wclear(winMsg);
			box(winMsg, 0, 0);
			refresh();
			wrefresh(winMsg);
			wmove(winMsg, 1, 1);
#endif
		}
	}
	else if (-1 != childPid) /* into father process : show characters, receive message */
	{
		int rc;
		char recvBuf[1024] = {0};
		int line = 1;
		while (1)
		{
			rc = recv(clientSocketFd, recvBuf, sizeof(recvBuf), 0);
			recvBuf[rc] = '\0';

			mvwaddstr(winInfo, line, 1, recvBuf);
			line++;
			if (line >= (LINES - 3))
			{
				wclear(winInfo);
				box(winInfo, 0, 0);
				line = 1;
			}
			refresh();
			wrefresh(winMsg);
			wmove(winMsg, 1, 1);
		}
	}
	else
	{
		printf("fork failed :%m\n");
		destroy(clientSocketFd, winInfo, winMsg);
		exit(-1);
	}

	destroy(clientSocketFd, winInfo, winMsg);

	return 0;
}


int TcpChatMultiProcess_Client(int argc, char *argv[])
{
	int clientSocketFd = 0;
	CreateSocketFd(clientSocketFd);
	ConnectToServer(clientSocketFd, g_localIP, PORT_SERVER);

	int rc, len;
	char messageBuf[1024] = {0};
	pid_t childPid = fork();
	if (0 == childPid) /* into child process : input characters, send message */
	{
		while (1)
		{
			rc = read(0, (void *)messageBuf, sizeof(messageBuf) - 1);
#if SEND_LENGTH
			rc = send(clientSocketFd, (void *)&len, sizeof(int), 0);
			if (-1 == rc)
			{
				perror("send[ERR]"), exit(-1);
			}
#endif
			rc = send(clientSocketFd, messageBuf, rc, 0);
			if (-1 == rc)
			{
				perror("send[ERR]");
				break;
			}
			printf("client send success---->\n");
		}
	}
	else if (-1 != childPid) /* into father process : show characters, receive message */
	{
		while (1)
		{
#if SEND_LENGTH
			/* receive client information */
			rc = ClientRecv(clientSocketFd, messageBuf, sizeof(int), MSG_WAITALL);
			if (rc <=0)	break;
			len = *(int *)messageBuf;

			rc = ClientRecv(clientSocketFd, messageBuf, len, MSG_WAITALL);
			if (rc <=0)	break;
			messageBuf[rc] = '\0';
			printf("%s", messageBuf);


			/* receive client message */
			rc = ClientRecv(clientSocketFd, messageBuf, sizeof(int), MSG_WAITALL);
			if (rc <=0)	break;
			len = *(int *)messageBuf;
#endif
			rc = ClientRecv(clientSocketFd, messageBuf, sizeof(messageBuf), 0);
			if (rc <=0)	break;
			messageBuf[rc] = '\0';
			printf("%s", messageBuf);
		}
	}
	else
	{
		printf("fork failed :%m\n");
		close(clientSocketFd);
		exit(-1);
	}

	close(clientSocketFd);

	return 0;
}


int TcpChatMultiProcess_Server(int argc, char *argv[])
{
	int serverSocketFd = 0;
	CreateSocketFd(serverSocketFd);
	BindAddr(serverSocketFd, g_localIP, PORT_SERVER);
	ListenClient(serverSocketFd);

	int *clientSocketFd;
	clientSocketFd = (int *)mmap(0, sizeof(int) * 100, PROT_WRITE | PROT_READ, MAP_ANONYMOUS | MAP_SHARED, 0, 0);
	memset(clientSocketFd, -1, sizeof(int) * 100);
	pid_t childPid = 0;
	/* when accept client socket, lasted child process will creates its child process. */
	int index = 0;
	struct sockaddr_in clientAddr;
	socklen_t len = sizeof(clientAddr);
	while (1)
	{
		clientSocketFd[index] = accept(serverSocketFd, (struct sockaddr *)&clientAddr, &len);
		if (-1 == clientSocketFd[index])
			break;
		printf("new client connect,IP:%s PORT:%05d\n", \
				inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
		childPid = fork();
		if (0 == childPid) /* into child process : input characters, send message */
		{
			index++;
			continue;
		}
		else if (-1 != childPid) /* into father process : show characters, receive message */
		{
			char clientInfo[64] = {0};
			sprintf(clientInfo, "IP:%s PORT:%05d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
			int clientInfoLen = strlen(clientInfo);

			int rc = 0, i = 0, len = 0;
			char messageBuf[1024] = {0};
			while (1)
			{
				/* receive message from client */
#if SEND_LENGTH
				rc = ServerRecv(clientSocketFd[index], clientAddr, messageBuf, sizeof(int), MSG_WAITALL);
				if (rc <= 0)	exit(-1);
				len = *(int *)messageBuf;
#endif
				rc = ServerRecv(clientSocketFd[index], clientAddr, messageBuf, sizeof(messageBuf), 0);
				if (rc <= 0)	exit(-1);
				len = rc;
				messageBuf[rc] = '\0';
				printf("%s\n", messageBuf);

				/* broadcast message to every client */

				for (i = 0; i < 100; i++)
				{
					if (-1 != clientSocketFd[i])
					{
#if SEND_LENGTH
						/* send client informatioin */
						rc = send(clientSocketFd[i], (void *)&clientInfoLen, sizeof(int), 0);
						if (-1 == rc)
						{
							perror("send[ERR]"), exit(-1);
						}

						rc = send(clientSocketFd[i], clientInfo, clientInfoLen, 0);
						if (-1 == rc)
						{
							perror("send[ERR]"), exit(-1);
						}

						/* send client message */
						rc = send(clientSocketFd[i], (void *)&len, sizeof(int), 0);
						if (-1 == rc)
						{
							perror("send[ERR]"), exit(-1);
						}
#endif
						rc = send(clientSocketFd[i], messageBuf, len, 0);
						if (-1 == rc)
						{
							perror("send[ERR]"), exit(-1);
						}
					}
				}
			}
		}
		else
		{
			printf("fork failed :%m\n");
			exit(-1);
		}
	} /* while (1) */

	close(serverSocketFd);
	close(clientSocketFd[index]);

	if (-1 == munmap(clientSocketFd, sizeof(int) * 100))
	{
		perror("munmap[ERR]"), exit(-1);
	}

	return 0;
}


void InitUI(WINDOW *winInfo, WINDOW *winMsg)
{
	initscr();
	winInfo = derwin(stdscr, (LINES - 5), COLS, 0, 0);
	winMsg = derwin(stdscr, 5, COLS, (LINES - 5), 0);

	box(winInfo, 0, 0);
	box(winMsg, 0, 0);

	refresh();
	wrefresh(winInfo);
	wrefresh(winMsg);
}

int CreateSocketFd(int &socketFd)
{
	socketFd = socket(AF_INET, SOCK_STREAM, getprotobyname("tcp")->p_proto);
	if (-1 == socketFd)
	{
		printf("create socket failed :%m\n"), exit(-1);
	}
	printf("create socket success\n");

	return 0;
}

int BindAddr(int &socketFd, const char *ip, unsigned int port)
{
	struct sockaddr_in localaAddr = {0};
	localaAddr.sin_family = AF_INET;
	localaAddr.sin_addr.s_addr = inet_addr(ip);
	localaAddr.sin_port = htons(port);

	if (-1 == bind(socketFd, (struct sockaddr *)&localaAddr, sizeof(localaAddr)))
	{
		printf("bind failed :%m\n");
		close(socketFd);
		exit(-1);
	}
	printf("bind success\n");

	return 0;
}

int ConnectToServer(int &socketFd, const char *ip, unsigned int port)
{
	struct sockaddr_in serverAddr = {0};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(ip);
	serverAddr.sin_port = htons(port);

	if (-1 == connect(socketFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)))
	{
		printf("connect failed :%m\n");
		close(socketFd);
		exit(-1);
	}
	printf("connect success\n");

	return 0;
}

int ListenClient(int &socketFd)
{
	int ret;
	ret = listen(socketFd, 10);
	if (-1 == ret)
	{
		printf("listen failed :%m\n");
		close(socketFd);
		exit(-1);
	}

	return 0;
}

int ServerRecv(const int &socket, const struct sockaddr_in &sockaddr, void *buf, size_t size, int flags)
{
	int rc = recv(socket, buf, size, flags);
	if (0 == rc)
	{
		printf("client exit.IP:%s PORT:%d\n", \
				inet_ntoa(sockaddr.sin_addr), ntohs(sockaddr.sin_port));
		exit(-1);
	}
	else if (-1 == rc)
	{
		printf("net error\n");
		exit(-1);
	}

	return rc;
}


int ClientRecv(const int &socket, void *buf, size_t size, int flags)
{
	int rc = recv(socket, buf, size, flags);
	if (0 == rc)
	{
		printf("server close.\n");
		exit(-1);
	}
	else if (-1 == rc)
	{
		printf("net error\n");
		exit(-1);
	}

	return rc;
}

void destroy(int &socketFd, WINDOW *winInfo, WINDOW *winMsg)
{
	getch();
	close(socketFd);
	endwin();
}

#endif /* TCPCHAT_CPP */
/* end file */
