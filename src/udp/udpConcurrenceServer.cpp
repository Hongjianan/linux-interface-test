/*
 * udpConcurrenceServer.cpp
 *
 *  Created on: 2015-4-12
 *      Author: Hong
 */
#include "../config.h"
#if UDPCONCURRENCESERVER_CPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int UdpConcurrenceServer_Server(Int32 argc, char *argv[]);
int UdpConcurrenceServer_Client(Int32 argc, char *argv[]);

/*======================main====================*/
int UdpConcurrenceServer(Int32 argc, char *argv[])
{
	int ret;

	ret = UdpConcurrenceServer_Server(argc, argv);
//	ret = UdpConcurrenceServer_Client(argc, argv);

	return ret;
}


int UdpConcurrenceServer_Server(Int32 argc, char *argv[])
{
	/* 1. create socket */
	int serverFd = socket(AF_INET, SOCK_DGRAM, getprotobyname("udp")->p_p_proto);
}

#endif /* UDPCONCURRENCESERVER_CPP */
/* end file */
