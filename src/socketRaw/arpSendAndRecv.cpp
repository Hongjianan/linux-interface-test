/*
 * arpSendAndRecv.cpp
 *
 *  Created on: 2015-12-18
 *      Author: Hong
 */
#include "../config.h"
#if ARP_SEND_AND_RECV_CPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/ether.h>
#include <net/if.h>
#include <netinet/ip.h>

typedef int STATUS;

#define APP_STATUS_SUCCESS	(0)
#define APP_STATUS_ERROR	(-1)

#define IPV4_LEN			(4)
#define MAC_ADDR_LEN		(6)
#define MAC_DEVICE_NAME		"eth0"

const char localIpv4Addr[IPV4_LEN] = { 192, 168, 0, 107 };
const char remotIpv4Addr[IPV4_LEN] = { 192, 168, 0, 103 };

const uint8_t bcastMacAddr[MAC_ADDR_LEN] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };    //ARP广播地址
const uint8_t localMacAddr[MAC_ADDR_LEN] = { 0x00, 0x0c, 0x29, 0xa6, 0x7a, 0x1c };

int SendArpPack(int sockfd, struct sockaddr_ll *pDstAddrll);
int RecvArpPack(int sockfd, struct sockaddr_ll *pAddrll);

struct ArpPack
{
    struct ether_header		sEthHdr;
    struct ether_arp 		sArp;
};

int ArpSendAndRecv(int argc, char *argv[])
{
	int sockFd;
	int rtval = -1;
	struct sockaddr_ll 	pDstAddrll;
	struct ifreq 		sLocalMacIf;

	sockFd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
	if (sockFd < 0)
	{
		printf("socket failed, errno=[%d][%s].\n", errno, strerror(errno)); fflush(stdout);
		return APP_STATUS_ERROR;
	}

	bzero(&sLocalMacIf, sizeof(struct ifreq));
	strcpy(sLocalMacIf.ifr_name, MAC_DEVICE_NAME);

	if (ioctl(sockFd, SIOCGIFINDEX, &sLocalMacIf))
	{
		printf("ioctl SIOCGIFINDEX failed, errno=[%d][%s]\n", errno, strerror(errno)); fflush(stdout);
		return APP_STATUS_ERROR;
	}

	bzero(&pDstAddrll, sizeof(struct sockaddr_ll));
	pDstAddrll.sll_family	= AF_PACKET;
	pDstAddrll.sll_ifindex	= sLocalMacIf.ifr_ifindex;
	pDstAddrll.sll_protocol	= htons(ETH_P_ARP);

	while (1)
	{
		if (APP_STATUS_SUCCESS != SendArpPack(sockFd, &pDstAddrll))
		{
			printf("Send arp socket failed!\n"); fflush(stdout);
			continue;
		}

		if (APP_STATUS_SUCCESS != RecvArpPack(sockFd, &pDstAddrll))
		{
			printf("Recv arp socket failed!\n"); fflush(stdout);
			continue;
		}

		sleep(1);
	}

	return 0;
}

STATUS SendArpPack(int sockfd, struct sockaddr_ll *pDstAddrll)
{
	struct ArpPack	sPacket;
	bzero(&sPacket, sizeof(struct ArpPack));

	/* Build mac header. */
	memcpy(sPacket.sEthHdr.ether_dhost, bcastMacAddr, MAC_ADDR_LEN);
	memcpy(sPacket.sEthHdr.ether_shost, localMacAddr, MAC_ADDR_LEN);
	sPacket.sEthHdr.ether_type = htons(ETH_P_ARP);

	/* Build arp header. */
	sPacket.sArp.ea_hdr.ar_hrd = htons(ARPHRD_ETHER);    /* 硬件类型 */
	sPacket.sArp.ea_hdr.ar_pro = htons(ETHERTYPE_IP);    /* 协议类型 ETHERTYPE_IP | ETH_P_IP */
	sPacket.sArp.ea_hdr.ar_hln = MAC_ADDR_LEN;           /* 硬件地址长度 */
	sPacket.sArp.ea_hdr.ar_pln = IPV4_LEN;               /* 协议地址长度 */
	sPacket.sArp.ea_hdr.ar_op  = htons(ARPOP_REQUEST);   /* ARP请求操作 */

	memcpy(sPacket.sArp.arp_sha, localMacAddr,  MAC_ADDR_LEN);   /* 源MAC地址 */
	memcpy(sPacket.sArp.arp_spa, localIpv4Addr, IPV4_LEN);     	/* 源IP地址 */
	memcpy(sPacket.sArp.arp_tha, bcastMacAddr,  MAC_ADDR_LEN);   /* 目的MAC地址 */
	memcpy(sPacket.sArp.arp_tpa, remotIpv4Addr, IPV4_LEN);     	/* 目的IP地址 */

	int sendLen = sendto(sockfd, &sPacket, sizeof(struct ArpPack), 0, (struct sockaddr*)pDstAddrll, sizeof(struct sockaddr_ll));
	if (sendLen < 0)
	{
		printf("sendto failed, errno=[%d][%s]\n", errno, strerror(errno)); fflush(stdout);
		return APP_STATUS_ERROR;
	}

	return APP_STATUS_SUCCESS;
}


STATUS RecvArpPack(int sockfd, struct sockaddr_ll *peer_addr)
{
	struct ArpPack	sPacket;
	bzero(&sPacket, sizeof(struct ArpPack));

	int recvLen = recvfrom(sockfd, &sPacket, sizeof(struct ArpPack), 0, NULL, NULL);
	if (recvLen > 0)
	{
		//判断是否接收到数据并且是否为回应包
		if (htons(ARPOP_REPLY) == sPacket.sArp.ea_hdr.ar_op)
		{
			uint8_t idx;
			for (idx = 0; idx < 6; ++idx)
			{
				printf("%u ", sPacket.sArp.arp_sha[idx]); fflush(stdout);
			}
			putchar('\n'); fflush(stdout);
			return APP_STATUS_SUCCESS;
		}
	}

	return APP_STATUS_ERROR;
}

#endif /* ARP_SEND_AND_RECV_CPP */

/* End of File */
