/*
 * icmpTest.cpp
 *
 *  Created on: 2015-7-22
 *      Author: hongjianan
 */
#include "../config.h"
#if RAWSOCKETUDPSENDTEST_CPP

#define _BSD_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <netdb.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <linux/if_packet.h>

#include <net/if.h>


#if 0
struct sockaddr_ll
{
	uint16_t	sll_family;
	__be16		sll_protocol;
	int			sll_ifindex;
	uint16_t	sll_hatype;
	uint8_t	sll_pkttype;
	uint8_t	sll_halen;
	uint8_t	sll_addr[8];
};
#endif

typedef uint8_t BOOL;

#define	ETHERTYPE_IP		0x0800		/* IP */
#define	ETHERTYPE_VLAN		0x8100		/* VLAN */


#define ETH_HEADER_LEN		14
#define VLAN_HEADER_LEN		4
#define IP_HEADER_LEN		20
#define UDP_HEADER_LEN		8

#define PACKET_HEADER_LEN	(ETH_HEADER_LEN + IP_HEADER_LEN + UDP_HEADER_LEN)

#define DEFAULT_VLAN_TCI	1

#define MAX_FRAGMENT_NUM	2
#define MAX_FRAGMENT_LEN	1500

/* VLAN header definition */
struct vlan_hdr
{
	uint16_t tci;
	uint16_t type;
};

__thread uint16_t gIpId;

const char *localEthInterface	= "fm1-mac5";

const char *localIp				= "192.168.197.1";
const char localMac[6]			= {0x00, 0x27, 0x1d, 0x00, 0x00, 0x01};	/* 0x00 0x01 0x04 0x07*/
const uint16_t localPort	= 50301;

const char *remotIp				= "192.168.197.241";
const char remotMac[6]			= {0x00, 0x27, 0x1d, 0x00, 0x00, 0x00};
const uint16_t remotPort	= 50311;

//const char *remotIp				= "192.168.197.2";
//const char remotMac[6]			= {0x00, 0x27, 0x1d, 0x00, 0x00, 0x04};
//const uint16_t remotPort	= 50302;

uint8_t* gSendBuf[2] = { NULL };

int  InitRawPacketSokcet(void);
int  SetLocalDevice(const char *pDeviceName);
void SendUdpPacketLoops(int sockFd, uint16_t tid, int packetNum, int payloadLen, int intervalSendTimeUS);

static inline uint16_t BuildEthHdr(void* const pBuffer, uint8_t *srcMac, uint8_t *dstMac, BOOL vlanFlag);
static inline uint16_t BuildIpv4Hdr(void* const pBuffer, const char *srcAddr, const char *dstAddr, uint16_t payLoadLen, BOOL checkFlag);
static inline uint16_t BuildUdpHdr(void* const pBuffer, uint16_t srcPort, uint16_t dstPort, uint16_t payLoadLen, BOOL checkFlag);


int RawSocketUdpSendTest_Tx(int argc, char *argv[]);
/*=====================main===================*/
int RawSocketUdpSendTest(int argc, char *argv[])
{
	int ret;

	ret = RawSocketUdpSendTest_Tx(argc, argv);

	return ret;
}

int RawSocketUdpSendTest_Tx(int argc, char *argv[])
{
	if (5 != argc)
	{
		printf("usage:[pakcetLen] [packetNum] [intervalTimeUS] [tid]\n");
		exit(-1);
	}

	int packLen			= atoi(argv[1]);
	int packNum			= atoi(argv[2]);
	int intervalTimeUS	= atoi(argv[3]);
	int tid				= atoi(argv[4]);

	/* 1. init sock */
	int sockFd;
	sockFd = InitRawPacketSokcet();
	if (-1 == sockFd)
	{
		printf("InitRAWSokcet fail.\n");
		exit(-1);
	}

	/* 2. set raw send */
	if (SetLocalDevice(localEthInterface))
	{
		printf("SetLocalDevice fail.\n");
		exit(-1);
	}

	/* 3. init send buffer */
	if (PrepareSendBuf(num, size, gSendBuf))
	/* 3. */
	SendUdpPacketLoops(sockFd, tid, packNum, packLen, intervalTimeUS);

	return 0;
}

int InitRawPacketSokcet(void)
{
	int sockFd;

	/* create socket type of SOCK_RAW need root authority. */
	sockFd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_IP));
	if (-1 == sockFd)
	{
		perror("socket fail");
		return -1;
	}

	return sockFd;
}

int SetLocalDevice(const char *pDeviceName)
{
	/* set local device */
	struct sockaddr_ll device;
	bzero(&device, sizeof(struct sockaddr_ll));

	device.sll_family	= AF_PACKET;
	device.sll_halen	= htons(sizeof(localMac));
	memcpy(device.sll_addr, localMac, sizeof(localMac));

	device.sll_ifindex = if_nametoindex(pDeviceName);
	if (!device.sll_ifindex)
	{
		perror("if_nametointer() faiil");
		return (-1);
	}
	printf("%d: index is %d\n", localEthInterface, device.sll_ifindex);

	return 0;
}

int PrepareSendBuf(const uint8_t num, const uint16_t size, uint8_t **pBufferArray)
{
	int idx;
	for (idx = 0; idx < num; ++idx)
	{
		pBufferArray[idx] = (uint8_t*)malloc(sizeof(uint8_t) * size);
		if (NULL == pBufferArray[idx])
		{
			perror("malloc fail"), exit(errno);
			return -1;
		}
	}

	return 0;
}


void SendUdpPacketLoops(int sockFd, uint16_t tid, int packetNum, int payloadLen, int intervalSendTimeUS)
{
	static int txPacks = 0;

	int packLen = PACKET_HEADER_LEN + payloadLen;
	uint8_t *packBuf = (uint8_t*)malloc(sizeof(uint8_t) * packLen);
	if (NULL == packBuf)
	{
		perror("malloc fail"), exit(errno);
	}

	struct ether_header	*pEthHdr	= (struct ether_header*)packBuf;
	struct ip 			*pIpHdr		= (struct ip*)(pEthHdr + 1);
	struct udphdr		*pUdpHdr	= (struct udphdr*)(pIpHdr + 1);

	uint16_t *pTid		= (uint16_t*)(pUdpHdr + 1);
	uint32_t *pSno		= (uint32_t*)(pTid + 1);
	uint8_t  *pPadding	= (uint8_t*) (pSno + 1);

	int paddingLen	 = payloadLen - sizeof(uint16_t) - sizeof(uint32_t);
	int ipPadloadLen = payloadLen + sizeof(struct udphdr);

	/* set padding */
	int idx;
	for (idx = 0; idx < paddingLen; ++idx)
	{
		pPadding[idx] = 'a' + (idx % 26);	/* 26 letter */
	}

	/* set header */
	*pTid = tid;
	(void)BuildEthHdr(pEthHdr, localMac, remotMac, 0);


	int sendLen;
	uint32_t sno;
	for (sno = 0; (0==packetNum) || (sno<packetNum); ++sno)
	{
		/* fill sno */
		*pSno = htonl(sno);

		if (sno & (uint32_t)(0x01))
		{
			isDoUdpCheckSum = 0;
		}
		else
		{
			isDoUdpCheckSum = 1;
		}

		BuildIpv4Hdr(pIpHdr, localIp, remotIp, ipPadloadLen, 1);
		BuildUdpHdr(pUdpHdr, pIpHdr, localPort, remotPort, payloadLen, isDoUdpCheckSum);

		sendLen = sendto(sockFd, packBuf, packLen, 0, (struct sockaddr*)&device, sizeof(device));
		if (sendLen >= 0)
		{
			++txPacks;
		}
		else if (-1 == sendLen)
		{
			perror("sendto failed");
			break;
		}

		if (intervalSendTimeUS)
		{
			usleep(intervalSendTimeUS);
		}
	}

	/* 5. close socket file */
	close(sockFd);

	printf("Total send [%d] packet\n", txPacks);
}


static inline uint16_t CalcIpv4HdrCheckSum(void *buf, uint16_t len)
{
    uint32_t sum		= 0;
    uint16_t *word	= (uint16_t*)buf;

    for (; len > 1; len -= 2)
    {
        sum += *word++;
        if (sum & 0x80000000)
        {
        	sum = (sum & 0xffff) + (sum >> 16);
        }
    }

    if (len)
    {
        sum += *(uint8_t*)word;
    }

    sum =  (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);

    return (uint16_t)~sum;
}

static inline uint16_t CalcUdpCheckSum(struct udphdr *pUdpHdr, uint16_t len, uint32_t srcAddr, uint32_t dstAddr)
{
	const uint16_t *buf = (const uint16_t *)pUdpHdr;
	uint16_t	*ipSrc = (uint16_t *)&srcAddr, *ipDst = (uint16_t *)&dstAddr;
	uint32_t sum;
	uint32_t length = len;

	//Calculate the sum
	sum = 0;
	while(len > 1)
	{
		sum += *buf++;
		if(sum & 0x80000000)
			sum = (sum & 0xFFFF) + (sum >> 16);

		len -= 2;
	}

	if(len & 1)//Add the padding if the packet length is odd
		sum += *((uint8_t*)buf);

	//Add the pseudo-header
	sum += *(ipSrc++);
	sum += *ipSrc;

	sum += *(ipDst++);
	sum += *ipDst;

	sum += htons(IPPROTO_UDP);
	sum += htons(length);

	//Add the carries
	while(sum >> 16)
		sum = (sum & 0xFFFF) + (sum >> 16);

	//Return the one's complement of sum
	return (uint16_t)(~sum);
}

static inline uint16_t BuildUdpHdr(void* const pBuffer, uint16_t srcPort, uint16_t dstPort, uint16_t payLoadLen, BOOL checkFlag)
{
	uint16_t hdrLen = sizeof(struct udphdr);
	struct udphdr* pUdpHdr = (struct udphdr*)pBuffer;

	/* Build UDP Header. */
	pUdpHdr->source = htons(srcPort);
	pUdpHdr->dest	= htons(dstPort);
	pUdpHdr->len	= htons(hdrLen + payLoadLen);
	pUdpHdr->check	= 0; /* Note: UDP Checksum is generated by DPAA Frame Manager. */

	return hdrLen;
}

static inline uint16_t BuildIpv4Hdr(void* const pBuffer, const char *srcAddr, const char *dstAddr, uint16_t payLoadLen, BOOL checkFlag)
{
	uint16_t hdrLen = sizeof(struct ip);
	struct ip* pIpHdr = (struct ip*)pBuffer;

	/* Build IPv4 Header. */
	pIpHdr->ip_v	= IPVERSION;
	pIpHdr->ip_hl	= hdrLen / 4;
	pIpHdr->ip_tos	= 0;
	pIpHdr->ip_len	= htons(hdrLen + payLoadLen);
	pIpHdr->ip_id	= gIpId++;
	pIpHdr->ip_off	= 0;
	pIpHdr->ip_ttl	= 128;
	pIpHdr->ip_p	= 0x11;

	inet_aton(srcAddr, &pIpHdr->ip_src);
	inet_aton(dstAddr, &pIpHdr->ip_dst);

	pIpHdr->ip_sum = 0;
	pIpHdr->ip_sum = CalcIpv4HdrCheckSum(pIpHdr, sizeof(*pIpHdr));

	return hdrLen;
}

static inline uint16_t BuildEthHdr(void* const pBuffer, uint8_t *srcMac, uint8_t *dstMac, BOOL vlanFlag)
{
	uint16_t hdrLen = ETH_HEADER_LEN;
	struct ether_header *pEthHdr	= (struct ether_header*)pBuffer;
	struct vlan_hdr		*pVlanHdr	= (struct vlan_hdr*)(pEthHdr + 1);

	/* Build Ethernet Header. */
	memcpy(pEthHdr->ether_shost, srcMac, 6);
	memcpy(pEthHdr->ether_dhost, dstMac, 6);


	if (!vlanFlag)
	{
		pEthHdr->ether_type = htons(ETHERTYPE_IP);
	}
	else	/* vlan */
	{
		pEthHdr->ether_type = htons(ETHERTYPE_VLAN);
		pVlanHdr->tci		= htons(DEFAULT_VLAN_TCI);
		pVlanHdr->type		= htons(ETHERTYPE_IP);

		hdrLen += VLAN_HEADER_LEN;
	}

	return hdrLen;
}


#endif /* RAWSOCKETUDPSENDTEST_CPP */
/* end file */
