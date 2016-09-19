/*
 * udpSendAndRecvGdpu.cpp
 *
 *  Created on: 2016-1-8
 *      Author: hongjianan
 */

#if 0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef char				Int8;
typedef unsigned char		Uint8;
typedef short				Int16;
typedef unsigned short		Uint16;
typedef int					Int32;
typedef unsigned int		Uint32;
typedef long long			Int64;
typedef unsigned long long	Uint64;

typedef Int32				STATUS;
typedef Int8				BOOL;

#define APP_STATUS_SUCCESS		(0)
#define APP_STATUS_ERROR		(-1)


STATUS BuildAndSendDlGtpPdu(const Uint8 msgType, const Uint32 teid,
		const BOOL pdcpPduNumberExtHdrFlag, const Uint16 pdcpSn, const Uint8 qci,
		const Uint32 dlDataIdx, const Uint32 dlDataLen)
{
	BOOL tpduPresent;
	Uint8 *pDscp, *pTpdu;
	Int32 ret, numTxBufs, tpduLen = 0;
	static S_Buffer sTxBuffer[MAX_NUM_TX_BUFS];

	/* Populate Downlink PDU Building Input. */
	_sDownlinkPduBuildingInput.sL5Hdrs.msgType = GTPU_HDR_MSG_TYPE_GPDU; /* Note: This value is assigned temporarily
	                                                                        for building GTP-U Headers in BUILD_DOWNLINK_PDU(). */
	_sDownlinkPduBuildingInput.sL5Hdrs.pldLen = dlDataLen;
	_sDownlinkPduBuildingInput.sL5Hdrs.teid = teid;
	if (pdcpPduNumberExtHdrFlag)
	{
		_sDownlinkPduBuildingInput.sL5Hdrs.pdcpPduNum = pdcpSn;
	}
	else
	{
		_sDownlinkPduBuildingInput.sL5Hdrs.pdcpPduNum = 0XFFFF;
	}

	_sDownlinkPduBuildingInput.sL4Hdr.srcPort = (Uint16)teid;
	_sDownlinkPduBuildingInput.sL4Hdr.dstPort = _localPortNumber;

	if (IP_ADDR_VER_4 == _sLocalIpAddr.ver)
	{
		pDscp = &_sDownlinkPduBuildingInput.sL3Hdrs.sIpv4Hdr.dscp;
		memcpy(_sDownlinkPduBuildingInput.sL3Hdrs.sIpv4Hdr.dstAddr, _sLocalIpAddr.addr, IP_ADDR_VER_4);

		_sDownlinkPduBuildingInput.sL2Hdr.etherType = L3_PROTOCOL_TYPE_IPV4;
	}
	else if (IP_ADDR_VER_6 == _sLocalIpAddr.ver)
	{
		pDscp = &_sDownlinkPduBuildingInput.sL3Hdrs.sIpv6Hdrs.dscp;
		memcpy(_sDownlinkPduBuildingInput.sL3Hdrs.sIpv6Hdrs.dstAddr, _sLocalIpAddr.addr, IP_ADDR_VER_6);

		_sDownlinkPduBuildingInput.sL2Hdr.etherType = L3_PROTOCOL_TYPE_IPV6;
	}
	else
	{
		printf("IP Version (%u) unidentified!\n", _sLocalIpAddr.ver);
		return APP_STATUS_ERROR;
	}

	memcpy(_sDownlinkPduBuildingInput.sL2Hdr.dstMac, _localMacAddr, 6);

	if ((qci >= QCI_NORMAL_MIN) && (qci <= QCI_NORMAL_MAX))
	{
		*pDscp = gQciToIpDscpTable[qci];
	}
	else
	{
		*pDscp = gQciToIpDscpTable[QCI_NORMAL_MAX];
	}

	/* Build Downlink PDU. */
	numTxBufs = BUILD_DOWNLINK_PDU(sTxBuffer, MAX_NUM_TX_BUFS, &_sDownlinkPduBuildingInput, &tpduPresent);
	if (4 != numTxBufs)
	{
		printf("Fail to build Downlink PDU!\n");
		return APP_STATUS_ERROR;
	}
	else
	{
		((S_GtpuHdrMandFlds*)sTxBuffer[0].buffer)->msgType = msgType; /* Note: The real value is assigned
		                                                                 after building GTP-U Headers in BUILD_DOWNLINK_PDU(). */
	}

	if (tpduPresent)
	{
		pTpdu = _dlData[dlDataIdx];
		tpduLen = dlDataLen;
	}
	else
	{
		pTpdu = NULL;
	}

	/* Send Downlink PDU. */
	ret = SendPdu(_dlS1x2uQueHdl, pTpdu, tpduLen, sTxBuffer, numTxBufs);
	if (APP_STATUS_SUCCESS != ret)
	{
		printf("Fail to send Downlink PDU!\n");
		return APP_STATUS_ERROR;
	}

	return APP_STATUS_SUCCESS;
}
#endif

/* End of File */
