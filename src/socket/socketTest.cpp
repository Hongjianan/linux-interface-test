/*
 * socketTest.cpp
 *
 *  Created on: 2014-10-31
 *      Author: Hong
 */
/*
 *	conclusion:
 *		1. setsockopt£º¿ÉÉèÖÃµÄ×îÐ¡Öµ£º256
 *						           ×î´óÖµ£º2048000
 *		default recieve buffer length is 112640
 *		the return bufLen is 1024000
 *		after set recieve buffer length is 2048000
 */

#include "../config.h"
#if SOCKETTEST_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/un.h>	/* struct sockaddr_un*/
#include <errno.h>

const char *gLoopBackIP 		= "127.0.0.1";
const unsigned short LocalPort 	= 54321;


int SocketTest_SocketFileRecv(int argc, char *argv[]);
int SocketTest_SocketFileSend(int argc, char *argv[]);

/*===============main================*/
int SocketTest(int argc, char *argv[])
{
	int ret;

//	ret = SocketTest_SocketFileSend(argc, argv);
	ret = SocketTest_SocketFileRecv(argc, argv);

	return ret;
}

int SocketTest_SocketFileRecv( int argc, char *argv[] )
{
	int fd;
	char recvBuf[1024];

	fd = socket(AF_UNIX, SOCK_DGRAM, 0);	//ï¿½Ôµï¿½Ä£ï¿½ï¿½ï¿½ï¿½Ö»ï¿½ï¿½ï¿½Ã±ï¿½ï¿½Ä¸ï¿½Ê½SOCK_DGRAM, UNIXï¿½Ä¼ï¿½
	if (-1 == fd)
	{
		perror("socket error"), exit(-1);
	}

	struct sockaddr_un clientAddr;
	bzero(&clientAddr, sizeof(clientAddr));
	clientAddr.sun_family = AF_UNIX;
	memcpy(clientAddr.sun_path, "./hong.socket", sizeof("./hong.socket"));

	int ret;
	ret = bind(fd, (struct sockaddr*)&clientAddr, sizeof(clientAddr));
	if( 0 != ret )	printf("bind fail.\n"), exit(-1);

	ret = read(fd, recvBuf, sizeof(recvBuf));
	if (0 == ret)
	{
		printf("internet close.\n"), exit(-1);
	}
	else if (-1 == ret)
	{
		printf("recve fail.\n"), exit(-1);
	}
	else if (ret > 0)
	{
		recvBuf[ret] = '\0';
		printf("recv data is %s\n", recvBuf);
	}

	/* 5. close socket */
	close(fd);

	/* 6. remove socket */
	unlink("./hong.socket");

	return 0;
}

int SocketTest_SocketFileSend( int argc, char *argv[] )
{
	/* 1.ï¿½ï¿½ï¿½ï¿½socket */
	int fd;
	fd = socket( AF_UNIX, SOL_DGRAM, 0);
	if( 0==fs )	std::cout<< "Create socket fail."<< std::endl,exit(-1);

	/* 2.ï¿½ï¿½ï¿½ï¿½sockaddr */
	struct sockaddr_un serverAddr;
	bzero( &serverAddr, sizeof(serverAddr) );
	serverAddr.sun_family = AF_UNIX;
	memcpy( serverAddr.sun_path, "./hong.socket", sizeof("./hong.socket") );

	/* 3.connect socket */
	int ret;
	ret = connect(fd, (struct sockaddr*)&serverAddr, sizeof(serverAddr) );
	if( 0 != ret )	std::cout<< "Bind socket fail."<< std::endl,exit(-1);

	/* 4.send data */
	ret = write( fd, "hello hong\n", strlen("hello hong\n") );

	/* 5.close socket */
	close(fd);
	/* 6. */

	return 0;
}


#endif /* SOCKETTEST_CPP */
/* end file */
