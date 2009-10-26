/////////////////////////////////////////////////////////////////////////////////
// Copyright(c) 2001-2002 Hybus Co,.ltd. All rights reserved.
//
// Module name:
//      bootp.c
//
// Description:
//
//
// Author:
//      lijilong
//
// Created:
//      2002.10
//
///////////////////////////////////////////////////////////////////////////////

#include "time.h"
#include "net.h"
#include "bootp.h"
#include "scc.h"

#define TIMEOUT							20		// bootp request
#define PORT_BOOTPS						67		// BOOTP server UDP port.
#define PORT_BOOTPC						68		// BOOTP client UDP port.

#define BOOTP_FAILURE					0
#define BOOTP_SUCCESS					1
#define BOOTP_CONTINUE					2

ulong	bootpID;
ulong	hostIP;
short	bootpState;
ushort	bootps=67;			// bootp server port #.
ushort	bootpc=68;			// bootp client port #.

uchar	broadcastEther[6]={0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
ulong	broadcastIP = 0xffffffff;
ulong	noIP		= 0x00000000;


bool	BootpTx(void);
void	SetBootpHeader(char *bootpHeader);

void  sendtest()
{
     int i;
     uchar *txPktBuf,rxPktBuf;
     unsigned char testpkt[MAX_PKT_SIZE];
     txPktBuf=testpkt;
     rxPktBuf=testpkt;
     MemSet(txPktBuf,0,MAX_PKT_SIZE);
     SetBootpHeader((char *)(txPktBuf+ETHER_HDR_SIZE+IP_HDR_SIZE+UDP_HDR_SIZE));
     SetUdpHeader((char *)(txPktBuf+ETHER_HDR_SIZE+IP_HDR_SIZE),bootps,bootpc,BOOTP_HDR_SIZE);
     SetIPHeader((char *)(txPktBuf+ETHER_HDR_SIZE),noIP,broadcastIP,UDP_HDR_SIZE+BOOTP_HDR_SIZE);
     SetEtherHeader(txPktBuf,broadcastEther,PROT_IP);
     for(i=0;i<10000;i++){
        EthTx(testpkt,ETHER_HDR_SIZE+IP_HDR_SIZE+UDP_HDR_SIZE+BOOTP_HDR_SIZE);
        EthRx(rxPktBuf);
        
     }
   
}

bool DoBootp(CMD_TBL *cptr, int argc, char **argv)
{
	return BootpTx();
}


bool BootpTx(void)
{
	int			i;
	char		*txPktBuf, *rxPktBuf;
	long		delay;		// delay :
	
	txPktBuf = PktBuf;
	rxPktBuf = PktBuf;

	bootpState = BOOTP_CONTINUE;
	protocol = PROT_BOOTP;

	// make boop packet.
	MemSet(txPktBuf, 0, MAX_PKT_SIZE);
	SetBootpHeader((char *)(txPktBuf+ETHER_HDR_SIZE+IP_HDR_SIZE+UDP_HDR_SIZE));
	SetUdpHeader((char *)(txPktBuf+ETHER_HDR_SIZE+IP_HDR_SIZE), bootps, bootpc, BOOTP_HDR_SIZE);
	SetIPHeader((char *)(txPktBuf+ETHER_HDR_SIZE), noIP, broadcastIP, UDP_HDR_SIZE+BOOTP_HDR_SIZE);
	SetEtherHeader(txPktBuf, broadcastEther, PROT_IP);

	if (!NetInit()) return false;

	// view info.
	printf("Our Ethernet address is ");
	PrintEthAddr(clientEther);
	printf(".\n");
	printf("\tSending bootp packet...\n");
	
	// bootp operation.
	for (i=0; i<TIMEOUT; i++){
		// transmit bootp packet to host.
		printf(".");
		if (!TxPacket(txPktBuf, ETHER_HDR_SIZE+IP_HDR_SIZE+UDP_HDR_SIZE+BOOTP_HDR_SIZE)) break;
		// receive bootp packet from host.
		delay = GetTime()+HZ;
		while (GetTime()<delay && bootpState==BOOTP_CONTINUE){
			RxPacket(rxPktBuf);
			if (bootpState==BOOTP_SUCCESS) break;
		}
		if (bootpState==BOOTP_SUCCESS) break;
	}
	printf("\n");

	protocol = NOPROTOCOL;
	
	if (bootpState==BOOTP_SUCCESS){
		printf("Bootp Packet received.\n");
		
		printf("\tHost   (server) Ethernet : ");
		PrintEthAddr(hostEther);
		printf("\n");
		
		printf("\tHost   (server) IP       : ");
		PrintIPAddr(hostIP);
		printf("\n");

		printf("\tClient (target) Ethernet : ");
		PrintEthAddr(clientEther);
		printf("\n");
		
		printf("\tClient (target) IP       : ");
		PrintIPAddr(clientIP);
		printf("\n");

		printf("\n");
		return true;
	}
	else {
		printf("Bootp packet is not received.\n\n");
		return false;
	}
	return true;
}	// BootpTx.


bool BootpRx(char *bootpHeader, short len){
	BOOTP_HEADER	*bhp = (BOOTP_HEADER *)bootpHeader;

	if (len!=BOOTP_HDR_SIZE)		return false;

	// check bootp.
	if (bhp->bh_opcode!=2)			return false;
	if (bhp->bh_htype!=1)			return false;
	if (bhp->bh_hlen!=6)			return false;
	if (MemCmp(&bhp->bh_tid, &bootpID, 4)) return false;
	if (MemCmp(&bhp->bh_chaddr, &clientEther, 6)) return false;

	// get infomation from bootp packet.
	MemCpy(&clientIP, &bhp->bh_yiaddr, 4);
	MemCpy(&hostIP  , &bhp->bh_siaddr, 4);

	bootpState = BOOTP_SUCCESS;
	return true;
}	// BootpRx.


void SetBootpHeader(char *bootpHeader){
	BOOTP_HEADER	*bhp = (BOOTP_HEADER *)bootpHeader;

	bhp->bh_opcode = OP_BOOTREQUEST;			// 1 : request		2 : reply.
	bhp->bh_htype  = HWT_ETHER;					// 10 Base Ethernet : 1.
	bhp->bh_hlen   = HWL_ETHER;					// 10 Base Ethernet : 6.
	bhp->bh_hops   = 0;							// client에서 0으로 setting. gateway가 사용.
	bhp->bh_secs   = SWAP16(GetTime() / HZ);	// 

	MemSet(&(bhp->bh_ciaddr), 0, 4);
	MemSet(&(bhp->bh_yiaddr), 0, 4);
	MemSet(&(bhp->bh_siaddr), 0, 4);
	MemSet(&(bhp->bh_giaddr), 0, 4);
	MemCpy((char *)bhp->bh_chaddr, clientEther, 6);

	bootpID = ((ulong)clientEther[2] << 24)
		| ((ulong)clientEther[3] << 16)
		| ((ulong)clientEther[4] << 8)
		| (ulong)clientEther[5];
	bootpID += GetTime();
	MemCpy(&(bhp->bh_tid), &(bootpID), 4);
}	// SetBootpHeader.
