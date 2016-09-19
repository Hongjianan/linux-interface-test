/*
 * icmpTest.cpp
 *
 *  Created on: 2015-7-22
 *      Author: hongjianan
 */
#include "../config.h"
#if RAWSOCKETDUMP_CPP

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

typedef uint8_t 	BOOL;
#define TRUE		(1)
#define FALSE		(0)

#define	ETHERTYPE_IP		0x0800		/* IP */
#define	ETHERTYPE_VLAN		0x8100		/* VLAN */

#define MAC_ADDR_LEN			(6)
#define IPV4_ADDR_STR_LEN		(16)

#define ETH_HEADER_LEN		14
#define VLAN_HEADER_LEN		4
#define IP_HEADER_LEN		20
#define UDP_HEADER_LEN		8

#define PACKET_HEADER_LEN	(ETH_HEADER_LEN + IP_HEADER_LEN + UDP_HEADER_LEN)

#define DEFAULT_VLAN_TCI	1

#define MAX_FRAGMENT_NUM	2
#define MAX_FRAGMENT_LEN	1500

#define DUMP_JUMP_ERROR		(-10)

/* VLAN header definition */
struct vlan_hdr
{
	uint16_t tci;
	uint16_t type;
};

struct LocalNetInfo
{
	char localMacAddr[MAC_ADDR_LEN];
	in_addr_t	localIpv4Addr;
};

struct ProgramArgs
{
	char *pDeviceName;
};

struct FilterArgs
{
	struct in_addr	sSrcIpv4Addr;
	struct in_addr	sDstIpv4Addr;
	uint16_t 		srcPort;
	uint16_t 		dstPort;
};

struct PacketParseResult
{
	uint8_t  		srcMacAddr[MAC_ADDR_LEN];
	uint8_t  		dstMacAddr[MAC_ADDR_LEN];
	struct in_addr	sSrcIpv4Addr;
	struct in_addr	sDstIpv4Addr;
	char			srcIpv4AddrStr[IPV4_ADDR_STR_LEN];
	char			dstIpv4AddrStr[IPV4_ADDR_STR_LEN];
	uint16_t 		srcPort;
	uint16_t 		dstPort;
	uint16_t		dataLen;
	uint8_t			ipChkCorrect;
	uint8_t			udpChkCorrect;
};

struct ProgramArgs			gProgArgs;
struct LocalNetInfo			gNetInfo;
struct FilterArgs			gPackFilter;
struct PacketParseResult	gPackParseResult;

int  InitRawPacketSokcet(void);
int  SetLocalDevice(const char *pDeviceName, const char *pMacAddr);
int  GetDeviceMacAndIp(const char *pDeviceName, char *pMacAddr, in_addr_t *pIpv4Addr);
int  InitFilter(struct FilterArgs *pFilter);
void RecvPacket(int sockFd);

static inline BOOL FilterPacket(const struct PacketParseResult *pResult);
static inline void DumpBuffer(const uint8_t *pBuffer, const uint32_t bufLen);

static inline uint16_t CalcIpv4HdrCheckSum(void *buf, uint16_t len);
static inline uint16_t CalcUdpCheckSum(struct udphdr *pUdpHdr, uint16_t len, uint32_t srcAddr, uint32_t dstAddr);

int RawSocketDump_Udp(int argc, char *argv[]);
/*=====================main===================*/
int RawSocketDump(int argc, char *argv[])
{
	int ret;

	ret = RawSocketDump_Udp(argc, argv);

	return ret;
}

int RawSocketDump_Udp(int argc, char *argv[])
{
	if (2 != argc)
	{
		printf("usage:[deviceName]\n");
		exit(-1);
	}

	gProgArgs.pDeviceName = argv[1];

	/* 1. init sock */
	int sockFd;
	sockFd = InitRawPacketSokcet();
	if (-1 == sockFd)
	{
		printf("InitRAWSokcet fail.\n");
		exit(-1);
	}

	/* 2. get local addr */
	if (GetDeviceMacAndIp(gProgArgs.pDeviceName, gNetInfo.localMacAddr, &gNetInfo.localIpv4Addr))
	{
		printf("GetDeviceMacAndIp fail.\n");
		exit(-1);
	}

	/* 3. set raw send */
	if (SetLocalDevice(gProgArgs.pDeviceName, gNetInfo.localMacAddr))
	{
		printf("SetLocalDevice fail.\n");
		exit(-1);
	}

	/* 4. Init filter */
	InitFilter(&gPackFilter);

	/* 4. dump udp packet */
	RecvPacket(sockFd);

	return 0;
}

static inline int ParseEthHdr(struct ether_header *pEthHdr)
{
	if (htons(ETHERTYPE_IP) != pEthHdr->ether_type)
	{
		printf("EthHdr: Not ip packet.\n");
		return DUMP_JUMP_ERROR;
	}

	memcpy(gPackParseResult.srcMacAddr, pEthHdr->ether_shost, sizeof(gPackParseResult.srcMacAddr));
	memcpy(gPackParseResult.dstMacAddr, pEthHdr->ether_dhost, sizeof(gPackParseResult.dstMacAddr));

	return 0;
}

static inline int ParseIpv4Hdr(struct ip *pIpHdr)
{
	if (IPVERSION != pIpHdr->ip_v)
	{
//		printf("IpHdr: Not ipv4.\n");
		return DUMP_JUMP_ERROR;
	}

	if (IPPROTO_UDP != pIpHdr->ip_p)
	{
//		printf("IpHdr: Not udp packet.\n");
		return DUMP_JUMP_ERROR;
	}

	if (CalcIpv4HdrCheckSum(pIpHdr, sizeof(struct ip)))
	{
		gPackParseResult.ipChkCorrect = 0;
		return -1;
	}

	gPackParseResult.ipChkCorrect	= 1;
	memcpy(&gPackParseResult.sSrcIpv4Addr, &pIpHdr->ip_src, sizeof(struct in_addr));
	memcpy(&gPackParseResult.sDstIpv4Addr, &pIpHdr->ip_dst, sizeof(struct in_addr));

	strncpy(gPackParseResult.srcIpv4AddrStr, inet_ntoa(pIpHdr->ip_src), sizeof(gPackParseResult.srcIpv4AddrStr));
	strncpy(gPackParseResult.dstIpv4AddrStr, inet_ntoa(pIpHdr->ip_dst), sizeof(gPackParseResult.dstIpv4AddrStr));

	return 0;
}

static inline int ParseUdpHdr(struct udphdr *pUdpHdr)
{
	gPackParseResult.udpChkCorrect	= 1;
	gPackParseResult.srcPort		= ntohs(pUdpHdr->source);
	gPackParseResult.dstPort		= ntohs(pUdpHdr->dest);
	gPackParseResult.dataLen		= ntohs(pUdpHdr->len) - sizeof(struct udphdr);

	return 0;
}

static inline int ParsePacketHdr(const uint8_t *pBuffer)
{
	struct ether_header *pEthHdr	= (struct ether_header*)pBuffer;
	struct ip			*pIpHdr 	= (struct ip*)(pEthHdr + 1);
	struct udphdr		*pUdpHdr	= (struct udphdr*)(pIpHdr + 1);

	if (DUMP_JUMP_ERROR == ParseEthHdr(pEthHdr))
	{
//		printf("ParseEthHdr failed.\n");
		return DUMP_JUMP_ERROR;
	}

	if (DUMP_JUMP_ERROR == ParseIpv4Hdr(pIpHdr))
	{
//		printf("ParseIpv4Hdr failed.\n");
		return DUMP_JUMP_ERROR;
	}

	if (DUMP_JUMP_ERROR == ParseUdpHdr(pUdpHdr))
	{
//		printf("ParseUdpHdr failed.\n");
		return DUMP_JUMP_ERROR;
	}

	return 0;
}

static inline int ShowParseResult(struct PacketParseResult *pParse)
{
	if (FilterPacket(pParse))
	{
		printf("srcMac[%02x:%02x:%02x:%02x:%02x:%02x] dstMac[%02x:%02x:%02x:%02x:%02x:%02x] srcIp[%s] dstIp[%s] "
				"srcPort[%u] dstPort[%u] dataLen[%u] ipSum[%u]\n",
				pParse->srcMacAddr[0], pParse->srcMacAddr[1], pParse->srcMacAddr[2],
				pParse->srcMacAddr[3], pParse->srcMacAddr[4], pParse->srcMacAddr[5],
				pParse->dstMacAddr[0], pParse->dstMacAddr[1], pParse->dstMacAddr[2],
				pParse->dstMacAddr[3], pParse->dstMacAddr[4], pParse->dstMacAddr[5],
				pParse->srcIpv4AddrStr, pParse->dstIpv4AddrStr,
				pParse->srcPort, pParse->dstPort, pParse->dataLen, pParse->ipChkCorrect);

		if (!pParse->ipChkCorrect)
		{
			return -1;
		}
	}

	return 0;
}

void RecvPacket(int sockFd)
{
	uint32_t  	length;
	uint8_t		recvBuf[65536];

	for (; ;)
	{
		if ((length = recv(sockFd, recvBuf, sizeof(recvBuf), 0)) > 0)
		{
			if (DUMP_JUMP_ERROR != ParsePacketHdr(recvBuf))
			{
				if (ShowParseResult(&gPackParseResult))
				{
					DumpBuffer(recvBuf, length);
				}
			}
		}
	}
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

int SetLocalDevice(const char *pDeviceName, const char *pMacAddr)
{
	/* set local device */
	struct sockaddr_ll device;
	bzero(&device, sizeof(struct sockaddr_ll));

	device.sll_family	= AF_PACKET;
	device.sll_halen	= htons(MAC_ADDR_LEN);
	memcpy(device.sll_addr, pMacAddr, MAC_ADDR_LEN);

	device.sll_ifindex = if_nametoindex(pDeviceName);
	if (!device.sll_ifindex)
	{
		perror("if_nametointer() faiil");
		return (-1);
	}

	return 0;
}


int GetDeviceMacAndIp(const char *pDeviceName, char *pMacAddr, in_addr_t *pIpv4Addr)
{
	struct sockaddr_in	*sin;
	struct ifreq	netCfg = { { {0} } };

	int sockFd = socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == sockFd)
	{
		perror("socket() failed\n");
		return -1;
	}

	/* get mac address. */
	strncpy(netCfg.ifr_name, pDeviceName, sizeof(netCfg.ifr_name));

	if (ioctl(sockFd, SIOCGIFHWADDR, &netCfg))
	{
		perror("ioctl() SIOCGIFHWADDR failed\n");
		return -1;
	}
	memcpy(pMacAddr, netCfg.ifr_hwaddr.sa_data, MAC_ADDR_LEN);

	/* get ip address. */
	if (ioctl(sockFd, SIOCGIFADDR, &netCfg))
	{
		perror("ioctl() SIOCGIFADDR failed\n");
		return -1;
	}
	sin = (struct sockaddr_in*)&netCfg.ifr_addr;
	memcpy(pIpv4Addr, &sin->sin_addr.s_addr, sizeof(in_addr_t));

	return 0;
}

static int InitDefaultFilter(struct FilterArgs *pFilter)
{
	inet_aton("192.168.197.254", &pFilter->sDstIpv4Addr);
	inet_aton("192.168.197.3",   &pFilter->sSrcIpv4Addr);

	pFilter->srcPort = 50000;
	pFilter->dstPort = 50302;

	return 0;
}

int InitFilter(struct FilterArgs *pFilter)
{
	InitDefaultFilter(pFilter);

	return 0;
}


static inline BOOL FilterPacket(const struct PacketParseResult *pResult)
{
	if (pResult->srcPort != gPackFilter.srcPort ||
		pResult->dstPort != gPackFilter.srcPort ||
		pResult->sSrcIpv4Addr.s_addr != gPackFilter.sSrcIpv4Addr.s_addr ||
		pResult->sDstIpv4Addr.s_addr != gPackFilter.sDstIpv4Addr.s_addr)
	{
		return FALSE;
	}

	return TRUE;
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

static inline void DumpBuffer(const uint8_t *pBuffer, const uint32_t bufLen)
{
	uint32_t idx;
	for (idx = 0; idx < bufLen; ++idx)
	{
		if (0 == (idx & 0x0f))
		{
			printf("\n[%04x]: ", idx);
		}
		else if (0 == (idx & 0x07))
		{
			printf("  ");
		}

		printf("%02x ", pBuffer[idx]);
	}
	putchar('\n');
}


#endif /* RAWSOCKETDUMP_CPP */
/* end file */
