/*
 * arp.c
 *
 *  Created on: 2015-12-19
 *      Author: hongjianan
 */
#include "../config.h"
#if RAWSOCKETARP_CPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <arpa/inet.h>
#include <netinet/ether.h>
#include <net/if.h>
#include "../common/baseTypedef.h"

#define MAX_ARP_REQUEST_TIMES		(10)
#define ARP_REQUEST_PERIOD_MS		(5000)

#define LOCAL_MAC_DEVICE_NAME		"fm1-mac2"

#define	ETHERTYPE_IP				0x0800		/* IP */
#define	ETHERTYPE_VLAN				0x8100		/* VLAN */

#define IPV4_ADDR_LEN				(4)
#define MAC_ADDR_LEN				(6)
#define IPV4_ADDR_STR_LEN			(16)

#define ETH_HEADER_LEN				(14)
#define VLAN_HEADER_LEN				(4)
#define MAX_ARP_PACK_LEN			(ETH_HEADER_LEN + VLAN_HEADER_LEN + 28)

#define DEFAULT_ARP_PCAP			(2)

/* VLAN header definition */
struct vlan_hdr
{
	Uint16 tci;

#define DEI_NO_DROP		(0)
	Uint16 type;
};

const Uint8 bcastMacAddr[MAC_ADDR_LEN] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
//const Uint8 localMacAddr[MAC_ADDR_LEN] = { 0x00, 0x27, 0x1d, 0x00, 0x00, 0x0b };
const Uint8 localMacAddr[MAC_ADDR_LEN] = { 0x00, 0x27, 0x1d, 0x28, 0x9b, 0xd8 };
const Uint8 localIpAddr[IPV4_ADDR_LEN] = { 10, 10, 218, 100 };
const Uint8 dstIpAddr[IPV4_ADDR_LEN] = { 10, 10, 218, 50 };
static Uint16 gVlanId = 200;	/* vlan id: if == 0, no vlan. */

STATUS GetMacAddrFromArp(const Uint8 *pDstIpv4Addr, Uint8 *pDstMacAddr);

/*===============main=================*/
int RawSocketArp(int argc, char *argv[])
{
	int ret;

	Uint8 dstMacAddr[MAC_ADDR_LEN];
	if (APP_STATUS_SUCCESS != GetMacAddrFromArp(dstIpAddr, dstMacAddr))
	{
		printf("GetMacAddrFromArp failed.\n");
	}
	else
	{
		printf("arp get: %u.%u.%u.%u %x:%x:%x:%x:%x:%x\n",
				dstIpAddr[0], dstIpAddr[1], dstIpAddr[2], dstIpAddr[3],
				dstMacAddr[0], dstMacAddr[1], dstMacAddr[2], dstMacAddr[3], dstMacAddr[4], dstMacAddr[5]);
	}

	return ret;
}

static inline void dump_hex(volatile uint8_t *data, uint32_t count)
{
	uint32_t i;
	for (i = 0; i < count; i++)
	{
		if(!(i%16))
			printf("\n%04x  ", i);
		else if(!(i%8))
			printf(" ");
		printf("%02x ", *data++);
	}
	printf("\n");
}

static inline void GetLocalMacAndIpv4Addr(Uint8 *pMacAddr, Uint8 *pIpv4Addr)
{
	memcpy(pMacAddr,  &localMacAddr[0], MAC_ADDR_LEN);
	memcpy(pIpv4Addr, &localIpAddr[0], IPV4_ADDR_LEN);
}

static inline BOOL IsArpPacket(const void *pBuffer)
{
	struct ether_header *pEthHdr = (struct ether_header*)pBuffer;

	if (htons(ETH_P_ARP) == pEthHdr->ether_type)
	{
		return TRUE;
	}

	return FALSE;
}

static inline Uint16 BuildArpEthHdr(void* const pBuffer, const Uint8 *pSrcMacAddr, const Uint8 *pDstMacAddr, Uint16 tci)
{
	Uint16 hdrLen = ETH_HEADER_LEN;
	struct ether_header *pEthHdr = (struct ether_header*)pBuffer;
	struct vlan_hdr		*pVlanHdr;

	/* Build Ethernet Header. */
	memcpy(pEthHdr->ether_shost, pSrcMacAddr, MAC_ADDR_LEN);
	memcpy(pEthHdr->ether_dhost, pDstMacAddr, MAC_ADDR_LEN);

	if (!tci)
	{
		pEthHdr->ether_type = htons(ETH_P_ARP);
	}
	else	/* vlan */
	{
		pVlanHdr	= (struct vlan_hdr*)(pEthHdr + 1);

		pEthHdr->ether_type = htons(ETHERTYPE_VLAN);
		pVlanHdr->tci		= htons(tci);
		pVlanHdr->type		= htons(ETH_P_ARP);

		hdrLen += VLAN_HEADER_LEN;
	}

	return hdrLen;
}

static inline Uint16 BuildArpHdr(void* const pBuffer, const Uint8 *pSrcMacAddr, const Uint8 *pBcastMacAddr,
								const Uint8 *pSrcIpv4Addr, const Uint8 *pDstIpv4Addr)
{
	/* Build arp header. */
	struct ether_arp *pArpHdr = (struct ether_arp*)pBuffer;

	pArpHdr->ea_hdr.ar_hrd = htons(ARPHRD_ETHER);
	pArpHdr->ea_hdr.ar_pro = htons(ETHERTYPE_IP);
	pArpHdr->ea_hdr.ar_hln = MAC_ADDR_LEN;
	pArpHdr->ea_hdr.ar_pln = IPV4_ADDR_LEN;
	pArpHdr->ea_hdr.ar_op  = htons(ARPOP_REQUEST);

	memcpy(pArpHdr->arp_sha, pSrcMacAddr,  MAC_ADDR_LEN);
	memcpy(pArpHdr->arp_spa, pSrcIpv4Addr, IPV4_ADDR_LEN);
	memcpy(pArpHdr->arp_tha, bcastMacAddr, MAC_ADDR_LEN);
	memcpy(pArpHdr->arp_tpa, pDstIpv4Addr, IPV4_ADDR_LEN);

	return sizeof(struct ether_arp);
}

STATUS InitArpSocket(Int32 *pSockFd)
{
	Int32 sockFd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
	if (sockFd < 0)
	{
		LOG_ERROR(MODULE_ID_SYS_SERVICE, "socket failed, errno=[%d][%s].\n", errno, strerror(errno));
		return APP_STATUS_ERROR;
	}

	/* 设置socket文件为非阻塞 */
	Int32 socketFlags;
	if(-1 == (socketFlags = fcntl(sockFd, F_GETFL, 0)))
	{
		LOG_ERROR(MODULE_ID_SYS_SERVICE, "fcntl F_GETFL fail! errno=[%d][%s]\n", errno, strerror(errno));
		return APP_STATUS_ERROR;
	}
	if(-1 == fcntl(sockFd, F_SETFL, (socketFlags | O_NONBLOCK)))
	{
		LOG_ERROR(MODULE_ID_SYS_SERVICE, "fcntl F_SETFL fail! errno=[%d][%s]\n", errno, strerror(errno));
		return APP_STATUS_ERROR;
	}

	*pSockFd = sockFd;

	return APP_STATUS_SUCCESS;
}

STATUS SendArpPack(Int32 sockFd, const Uint8 *pSrcMacAddr, const Uint8 *pSrcIpv4Addr, const Uint8 *pDstIpv4Addr, Uint16 vlanId, struct sockaddr_ll *pDstAddrll)
{
	Uint16 packLen;
	Uint8 arpPack[MAX_ARP_PACK_LEN];

	/* Build mac header. */
	Uint16 tci;
	if (vlanId)
	{
		tci = (DEFAULT_ARP_PCAP << 13) | (DEI_NO_DROP << 12) | vlanId;
	}
	else
	{
		tci = 0;
	}
	packLen  = BuildArpEthHdr(arpPack, pSrcMacAddr, bcastMacAddr, tci);

	packLen += BuildArpHdr(&arpPack[packLen], pSrcMacAddr, bcastMacAddr, pSrcIpv4Addr, pDstIpv4Addr);

	Int32 sendLen = sendto(sockFd, &arpPack, packLen, 0, (struct sockaddr*)pDstAddrll, sizeof(struct sockaddr_ll));
	if (packLen != sendLen)
	{
		LOG_ERROR(MODULE_ID_SYS_SERVICE, "sendto failed, sendLen[%d] errno=[%d][%s]\n", sendLen, errno, strerror(errno));
		return APP_STATUS_ERROR;
	}

	return APP_STATUS_SUCCESS;
}

STATUS RecvArpPack(Int32 sockFd, const Uint8 *pDstIpv4Addr, Uint8 *pDstMacAddr)
{
	Uint8 arpPack[MAX_ARP_PACK_LEN];

	struct ether_arp *pArpHdr;
	pArpHdr = (struct ether_arp*)&arpPack[ETH_HEADER_LEN];

	while (recvfrom(sockFd, &arpPack, sizeof(arpPack), 0, NULL, NULL) > 0)
	{
		dump_hex(arpPack, MAX_ARP_PACK_LEN);

		if (!IsArpPacket(arpPack))
		{
			LOG_WARN(MODULE_ID_SYS_SERVICE, "receive not arp packets!\n");
			continue;
		}

		if (htons(ARPOP_REPLY) == pArpHdr->ea_hdr.ar_op)
		{
			Uint32 idx;
			for (idx = 0; idx < IPV4_ADDR_LEN; ++idx)
			{
				if (pDstIpv4Addr[idx] != pArpHdr->arp_spa[idx])
				{
					break;
				}
			}

			if (IPV4_ADDR_LEN == idx)
			{
				memcpy(pDstMacAddr, pArpHdr->arp_sha, MAC_ADDR_LEN);
				return APP_STATUS_SUCCESS;
			}
		}
	}

	return APP_STATUS_ERROR;
}

STATUS GetMacAddrFromArp(const Uint8 *pDstIpv4Addr, Uint8 *pDstMacAddr)
{
	struct sockaddr_ll 	pDstAddrll;
	struct ifreq 		sLocalMacIf;

	Int32 sockFd;
	if (APP_STATUS_SUCCESS != InitArpSocket(&sockFd))
	{
		LOG_ERROR(MODULE_ID_SYS_SERVICE, "InitArpSocket failed!\n");
		return APP_STATUS_ERROR;
	}

	bzero(&sLocalMacIf, sizeof(struct ifreq));
	strcpy(sLocalMacIf.ifr_name, LOCAL_MAC_DEVICE_NAME);

	if (ioctl(sockFd, SIOCGIFINDEX, &sLocalMacIf))
	{
		LOG_ERROR(MODULE_ID_SYS_SERVICE, "ioctl SIOCGIFINDEX failed, errno=[%d][%s].\n", errno, strerror(errno));
		return APP_STATUS_ERROR;
	}

	bzero(&pDstAddrll, sizeof(struct sockaddr_ll));
	pDstAddrll.sll_family	= AF_PACKET;
	pDstAddrll.sll_ifindex	= sLocalMacIf.ifr_ifindex;
	pDstAddrll.sll_protocol	= htons(ETH_P_ARP);

	/* get local net information. */
	Uint8 localMacAddr[MAC_ADDR_LEN]	= {0};
	Uint8 localIpv4Addr[IPV4_ADDR_LEN]	= {0};
	GetLocalMacAndIpv4Addr((Uint8*)&localMacAddr, (Uint8*)&localIpv4Addr);

	Uint32 loops;
	for (loops = 0; loops < MAX_ARP_REQUEST_TIMES; ++loops)
	{
		if (APP_STATUS_SUCCESS != SendArpPack(sockFd, (Uint8*)&localMacAddr, (Uint8*)&localIpv4Addr, pDstIpv4Addr, gVlanId, &pDstAddrll))
		{
			LOG_ERROR(MODULE_ID_SYS_SERVICE, "Send arp request failed.\n");
			continue;
		}

		usleep(ARP_REQUEST_PERIOD_MS);

		if (APP_STATUS_SUCCESS == RecvArpPack(sockFd, pDstIpv4Addr, pDstMacAddr))
		{

			char addArpCmd[256] = { 0 };
			snprintf(addArpCmd, sizeof(addArpCmd), "arp -s %u.%u.%u.%u %x:%x:%x:%x:%x:%x",
					pDstIpv4Addr[0], pDstIpv4Addr[1], pDstIpv4Addr[2], pDstIpv4Addr[3],
					pDstMacAddr[0], pDstMacAddr[1], pDstMacAddr[2], pDstMacAddr[3], pDstMacAddr[4], pDstMacAddr[5]);
			if (system(addArpCmd))
			{
				LOG_ERROR(MODULE_ID_SYS_SERVICE, "system() failed, command[%s].\n", addArpCmd);
				continue;
			}

			close(sockFd);

			return APP_STATUS_SUCCESS;
		}
	}

	close(sockFd);

	return APP_STATUS_ERROR;
}

#endif
/* End of File */
