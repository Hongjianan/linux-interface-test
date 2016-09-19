#include "../config.h"
#if UDPSENDTEST_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>

#define SERVER_PORT 30000 // define the defualt connect port id

#define TEST_VERSION "1.04"

#define CLIENT_PORT 30001
#define BUFFER_SIZE 20000
#define REUQEST_MESSAGE "welcome to connect the server.\n"
#define FILEMAME "/mnt/flash/test.log"
#define TRANS_TIME 100
#define DELAY_SEC 3
#define TRAS_MUN 10000
#define DIR_LOG "/mnt/flash"

int writeData(int sockfd, const unsigned char *buf,int dataLen,unsigned char *addr,int len);

void printftime(void)
{
	time_t now = time(NULL);
	struct tm* t = localtime (&now);
	printf(" [%04d-%02d-%02d %02d:%02d:%02d] ",
			(t->tm_year + 1900),
			t->tm_mon+1,
			t->tm_mday,
			t->tm_hour,
			t->tm_min,
			t->tm_sec
	);
}

void usage(char *name)
{
	printf("usage: %s client_address server_address send_size rev_size\n",name);
}

int num1 = 0;
int num2 = 0;

int UdpSendTest(int argc, char **argv)
{
	int clifd;
	int length = 0;
	int byte_send = 0;
	int fd = -1;
	int len;
	char str[100];
	struct sockaddr_in servaddr,cliaddr;
	socklen_t socklen = sizeof(servaddr);
	unsigned char bufsend[BUFFER_SIZE];
	unsigned char bufrecv[BUFFER_SIZE];
	int send_size = 0;
	int rec_size = 0;

	int cnt = 0;
	int rectimes = 0;
	int sendtimes = 0;
	char *end,*argv_t;
	struct timezone tz;
	struct timeval old, now,diff;
	printf(" VER: %s. BUILD TIME(%s %s).\n", TEST_VERSION, __DATE__, __TIME__);
#if 1
	if (argc < 5)
	{
		usage(argv[0]);
		exit(1);
	}
#endif

	if ((clifd = socket(AF_INET,SOCK_DGRAM,0)) < 0)
	{
		printf("create socket error!\n");
		exit(1);
	}

	bzero(&cliaddr,sizeof(cliaddr));
	cliaddr.sin_family = AF_INET;
	cliaddr.sin_port = htons(CLIENT_PORT);
	cliaddr.sin_addr.s_addr = inet_addr (argv[1]);
	printf("client clientip=%s\n",argv[1]);

	/* set send message length */
	cnt = strlen(argv[3])+1;
	argv_t =(char *)malloc(cnt);
	strncpy(argv_t, argv[3],cnt);
	send_size = (int)strtoul(argv[3],&end,10);
	if(argv_t != NULL) free(argv_t);

	cnt = strlen(argv[4])+1;
	argv_t =(char *)malloc(cnt);
	strncpy(argv_t, argv[4],cnt);
	rec_size = (int)strtoul(argv[4],&end,10);
	if(argv_t != NULL) free(argv_t);

	int bcastOpt = 1;
	setsockopt(clifd,SOL_SOCKET,SO_BROADCAST,(char *)&bcastOpt,4);
	setsockopt(clifd,SOL_SOCKET,SO_REUSEADDR,(char *)&bcastOpt,4);

	//#if 0
	int recv_bufSize = 0;
	socklen_t socketLen_r;
	int send_bufSize = 0;
	socklen_t socketLen_s;
	socketLen_r = sizeof(recv_bufSize);
	socketLen_s = sizeof(send_bufSize);
	getsockopt(clifd, SOL_SOCKET, SO_RCVBUF,(void *)&recv_bufSize,&socketLen_r);
	printf("before setsockopt SO_RCVBUF linux recv bufSize = %d,socketLen=%d\n",recv_bufSize,socketLen_r);
	getsockopt(clifd, SOL_SOCKET, SO_SNDBUF,(void*)&send_bufSize,&socketLen_s);
	printf("before setsockopt SO_SNDBUF linux send bufSize = %d,socketLen=%d\n",send_bufSize,socketLen_s);
	int socketBufSize = 128*1024;
	if (recv_bufSize < socketBufSize)
	{
		recv_bufSize = socketBufSize;
		setsockopt(clifd, SOL_SOCKET, SO_RCVBUF,(void *)&recv_bufSize,sizeof(recv_bufSize));
		getsockopt(clifd, SOL_SOCKET, SO_RCVBUF,(void *)&recv_bufSize,&socketLen_r);
		printf("after setsockopt SO_RCVBUF linux recv bufSize = %d,socketLen=%d\n",recv_bufSize,socketLen_r);
	}

	if (send_bufSize < socketBufSize)
	{
		send_bufSize = socketBufSize;
		setsockopt(clifd, SOL_SOCKET, SO_SNDBUF,(void*)&send_bufSize,sizeof(send_bufSize));
		getsockopt(clifd, SOL_SOCKET, SO_SNDBUF,(void*)&send_bufSize,&socketLen_s);
		printf("after setsockopt SO_SNDBUF linux send bufSize = %d,socketLen=%d\n",send_bufSize,socketLen_s);
	}
	//#endif


	if (bind(clifd,(struct sockaddr*)&cliaddr,sizeof(cliaddr))<0)
	{
		printf("bind to port %d failure!\n",CLIENT_PORT);
	}

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr (argv[2]);
	servaddr.sin_port = htons(SERVER_PORT);
	printf("server serverip=%s\n",argv[2]);

	while(1)
	{

		gettimeofday(&old,&tz);
		byte_send =writeData (clifd, (unsigned char*)bufsend, send_size,(unsigned char *)&servaddr,socklen);
		gettimeofday(&now,&tz);

		if (now.tv_usec >= old.tv_usec)
		{
			diff.tv_sec = now.tv_sec-old.tv_sec;
			diff.tv_usec = now.tv_usec-old.tv_usec;
		}
		else
		{
			diff.tv_sec = now.tv_sec-old.tv_sec-1;
			diff.tv_usec = 1000000+now.tv_usec-old.tv_usec;
		}

		printf("udpclient send data:");
		printftime();
		printf("no.[%d] size=%d bytes,seq=%d,time=%ds %dus.\n\r",num2, byte_send, sendtimes++,(int)diff.tv_sec,(int)diff.tv_usec);
		//sleep(1);

	}
	close(fd);
	close(clifd);

	return 0;
}

int writeData(int sockfd, const unsigned char *buf,int dataLen,unsigned char *addr,int len)
{

	if ( ( NULL == buf ) || ( -1 == sockfd ) || ( 0 == dataLen ) )
	{
		return -1;
	}
	int bytesTotal = dataLen;
	int bytesSent = 0;
	num2 = (buf[3] <<24) + (buf[2] << 16) + (buf[1] <<8) + buf[0];

	while (bytesSent < bytesTotal)
	{
		ssize_t n = 0;
		void* p = (void*)(buf + bytesSent);
		n= sendto ( sockfd, p,bytesTotal - bytesSent,0,(struct sockaddr *)addr,len);

		if (n<=0)
		{
			if (n < 0 && errno == EINTR)
			{
				printf("UdpClient writeData , sockfd = %d bytesSent=%d,bytesTotal=%d errno = %d errorNo=%s,continue send\n", sockfd, bytesSent,dataLen,errno,strerror (errno ) );
				continue; /* call write again */
			}
			else if (n < 0 && errno == EWOULDBLOCK)
			{
				printf("UdpClient writeData , sockfd = %d bytesSent=%d,bytesTotal=%d errno = %d errorNo=%s,continue send\n", sockfd, bytesSent,dataLen,errno,strerror (errno ) );
				continue; /* call write again */
			}
			else
			{
				//if (GlobalVar::getTcpLinkState() == TCP_LINK_CONNECTED)  GlobalVar::setTcpLinkState(TCP_LINK_DISCONNECT);
				printf("UdpClient writeData fail, sockfd = %d bytesSent=%d,bytesTotal=%d errno = %d errorNo=%s, will be reconnect UDP connection\n", sockfd, bytesSent,dataLen,errno,strerror (errno ) );
				return -1; /* error */
			}
		}
		bytesSent += n;
	}

	return bytesSent;
}

#endif /* UDPSENDTEST_CPP */
/* end file */
