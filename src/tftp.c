/////////////////////////////////////////////////////////////////////////////////
// Copyright(c) 2001-2002 Hybus Co,.ltd. All rights reserved.
//
// Module name:
//      tftp.c
//
// Description:
//
//
// Author:
//      bedguy
//
// Created:
//      2002.10
//
///////////////////////////////////////////////////////////////////////////////

#include "net.h"
#include "tftp.h"
#include "bootp.h"

#define TFTP_PORT					69	// Well known TFTP port #
#define TFTP_TIMEOUT				5		// Seconds to tftpTimeout for a lost pkt
#define TIMEOUT_COUNT				10		// # of tftpTimeouts before giving up

// TFTP operations.
#define TFTP_RRQ					1
#define TFTP_WRQ					2
#define TFTP_DATA					3
#define TFTP_ACK					4
#define TFTP_ERROR					5


#define TFTP_NONE					0
#define TFTP_STATE_RRQ				1
#define TFTP_STATE_DATA				2
#define TFTP_STATE_TOO_LARGE		3
#define TFTP_STATE_BAD_MAGIC		4
#define TFTP_STATE_SUCCESS			5
#define TFTP_STATE_FAILURE			6


static short	tftpState = 0;
static unsigned short	tftpBlock = 0;
static short	tftpLastBlock = 0;
static ulong	loadAddr = 0;
ushort			tftpPort = 69;
ushort			tftpHostPort=0;
ushort			tftpClientPort = 7777;
static char		requestFile[128];
static ulong	tftpTimeout;

// Prototypes.
bool			TftpStart(char *filename);
void			TftpTx(char *txBuf);

bool DoTftp(CMD_TBL *cptr, int argc, char **argv)
{
	if (argc!=3){
		printf(cptr->usage);
		return false;
	}
	if (!StrCmp(argv[2], "kernel"))
	{
		loadAddr = (long)KERNEL_DRAM_BASE;
	}
	else if (!StrCmp(argv[2], "root"))
	{
		loadAddr = (long)ROOT_DRAM_BASE;
	}
	else if (!StrCmp(argv[2], "loader"))
	{
		loadAddr = (long)LOADER_DRAM_BASE;
	}
	else {
		if (!HexToInt(argv[2], &loadAddr, 32))
		{
			printf(cptr->usage);
			return false;
		}
	}

	// IP
	if (clientIP==0){
		printf("No IP. Bootp start...\n");
		if (DoBootp(0, 0,0)==false){
			printf("\tBootp failed. try again.\n");
			return false;
		}
	}
	return TftpStart(argv[1]);	// argv[1] : filename.
}	// DoTftp.


bool TftpStart(char *filename)
{
	char		*txPktBuf, *rxPktBuf;

	txPktBuf = PktBuf;
	rxPktBuf = PktBuf;
	
	// check filename.
	if (!filename || !filename[0]){
		printf("boot file name is not exists.\n");
		return false;
	}
	
	printf("TFTP Start...\n");
	
	printf("\tHost   (server) IP       : ");
	PrintIPAddr(hostIP);
	printf("\n");
	
	printf("\tClient (target) IP       : ");
	PrintIPAddr(clientIP);
	printf("\n");

	StrCpy(requestFile, filename);
	printf("\tLoading Filename         : %s\n", requestFile);
	printf("\tSave Address             : 0x%08lx\n", loadAddr);
	
	printf("\nLoading start...\n");

	protocol = PROT_TFTP;
	tftpState = TFTP_STATE_RRQ;
	tftpHostPort = 0;

	TftpTx(txPktBuf);

	tftpTimeout = GetTime() + TFTP_TIMEOUT * HZ;
	
	while (tftpState != TFTP_STATE_SUCCESS && tftpState != TFTP_STATE_FAILURE)
	{
		RxPacket(rxPktBuf);
		if (GetTime()>tftpTimeout)
		{
			printf("\n\tTftp is failed. Try again.\n\n");
			tftpState=TFTP_STATE_FAILURE;
		}
	}

	protocol = NOPROTOCOL;
	tftpState = TFTP_NONE;
	tftpBlock = 0;
	tftpHostPort = 0;
	return true;
}	// TftpStart.


void TftpTx(char *txPktBuf)
{
	char		*pktPtr = (char *)(txPktBuf+ETHER_HDR_SIZE+IP_HDR_SIZE+UDP_HDR_SIZE);
	ushort		hostPort = 0;
	int			len = 0;

	MemSet(txPktBuf, 0, MAX_PKT_SIZE);
	// make tftp header.
	switch (tftpState)
	{
		// tftp request packet
		case TFTP_STATE_RRQ:
			*((ushort *)pktPtr)++ = SWAP16(TFTP_RRQ);
			StrCpy((char *)pktPtr, requestFile);
			pktPtr += StrLen(requestFile) + 1;
			StrCpy((char *)pktPtr, "octet");
			pktPtr += 5 + 1;
			len = pktPtr - (txPktBuf+ETHER_HDR_SIZE+IP_HDR_SIZE+UDP_HDR_SIZE);
			hostPort = tftpPort;
			break;

		// data packet acknowledge
		case TFTP_STATE_DATA:
			*((ushort *)pktPtr)++ = SWAP16(TFTP_ACK);
			*((ushort *)pktPtr)++ = SWAP16(tftpBlock);
			len = pktPtr - (txPktBuf+ETHER_HDR_SIZE+IP_HDR_SIZE+UDP_HDR_SIZE);
			hostPort = tftpHostPort;
			break;

		// error packet
		case TFTP_STATE_TOO_LARGE:
			*((ushort *)pktPtr)++ = SWAP16(TFTP_ERROR);
			*((ushort *)pktPtr)++ = SWAP16(3);
			StrCpy((char *)pktPtr, "File too large");
			pktPtr += 14 + 1;
			len = pktPtr - (txPktBuf+ETHER_HDR_SIZE+IP_HDR_SIZE+UDP_HDR_SIZE);
			hostPort = tftpHostPort;

			break;

		// error packet
		case TFTP_STATE_BAD_MAGIC:
			*((ushort *)pktPtr)++ = SWAP16(TFTP_ERROR);
			*((ushort *)pktPtr)++ = SWAP16(2);
			StrCpy((char *)pktPtr, "File has bad magic");
			pktPtr += 18 + 1;
			len = pktPtr - (txPktBuf+ETHER_HDR_SIZE+IP_HDR_SIZE+UDP_HDR_SIZE);
			hostPort = tftpHostPort;
			break;
		default :
			return;
	}

	// Ethernet, IP, UDP Header
	SetUdpHeader((char *)(txPktBuf+ETHER_HDR_SIZE+IP_HDR_SIZE), hostPort, tftpClientPort, len);
	SetIPHeader((char *)(txPktBuf+ETHER_HDR_SIZE), clientIP, hostIP, UDP_HDR_SIZE+len);
	SetEtherHeader(txPktBuf, hostEther, PROT_IP);

	// tftp packet
	TxPacket(txPktBuf, ETHER_HDR_SIZE+IP_HDR_SIZE+UDP_HDR_SIZE+len);
	return;
}	// TftpTx.


bool TftpRx(char *rxPktBuf, int len)
{
	char			*pktPtr = rxPktBuf;
	short			value;
	unsigned long		rlen=0;			// tftp· (buffer).
	
	// handling received data.
	value = SWAP16(*(short *)pktPtr);	// packet type.
	pktPtr+=2;
	len-=2;
	switch (value){
		case TFTP_RRQ :
		case TFTP_WRQ :
		case TFTP_ACK :
		default:
			break;
		// tftp data packet.
		case TFTP_DATA :
			// error. check data index.
			if (len < 2) return false;
			tftpBlock = SWAP16(*(ushort *)pktPtr);	// tftp data packet
			pktPtr += 2;
			len -= 2;
			// tftp receive.
			if (( (tftpBlock)%128) == 0 ){	// each 64 kbyte.
				ClearLine();
				rlen = (unsigned long)tftpBlock * 512;
				printf("\t0x%08lx bytes received.", rlen);
			}

			// request
			if (tftpState == TFTP_STATE_RRQ){
				tftpState = TFTP_STATE_DATA;
				tftpLastBlock = 0;

				if (tftpBlock != 1){
					tftpState = TFTP_STATE_FAILURE;
					return false;
				}
			}
			
			if (tftpBlock <= tftpLastBlock) break;

			tftpLastBlock = tftpBlock;
	
			MemCpy((char *)(loadAddr+(tftpBlock-1)*512), (char *)pktPtr, len);

			tftpTimeout = GetTime() + TFTP_TIMEOUT * HZ;
			
			TftpTx(rxPktBuf);

			if (len < 512){
				// We received the whole thing.  Try to run it.
				ClearLine();
				rlen = ((long)tftpBlock-1) * 512 + len;
				printf("\t0x%08lx (%ld) bytes received.\n", rlen, rlen);
				printf("\ttftp done.\n\n");
				tftpState = TFTP_STATE_SUCCESS;
			}
			break;
		// error
		case TFTP_ERROR :
			// tftp RRQ or tftp ack pakcet has some error.
			printf("\nTFTP error: '%s' (%d)\n", pktPtr+2, SWAP16(*(ushort *)pktPtr));
			tftpState = TFTP_STATE_FAILURE;
			break;
	}
	return true;
}	// TftpRx.
