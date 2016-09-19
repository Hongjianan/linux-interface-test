/*
 * tcpSelect.cpp
 *
 *  Created on: Jan 1, 2015
 *      Author: hong
 */
#include "../config.h"
#if TCPSELECT_CPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>


int TcpSelect_Server(int argc, char *argv[]);

/*==============main===============*/
int TcpSelect(int argc, char *argv[])
{
	int ret;

	ret = TcpSelect_Server(argc, argv);

	return ret;
}

#endif /* TCPSELECT_CPP */
/* end file */
