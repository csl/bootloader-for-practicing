/////////////////////////////////////////////////////////////////////////////////
// Copyright(c) 2001-2002 Hybus Co,.ltd. All rights reserved.
//  
// Module name:
//      main.c
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

#ifndef _NET_H_532453245234534534
#define _NET_H_532453245234534534

#include "main.h"
#include "header.h"


// for cpu byte order (big or little endian).
// Byte swapping.
#ifndef SWAP8
#define SWAP8(A)		(A)
#define SWAP16(A)		((((A)&0x00ff)<<8) | ((A)>>8))
#define SWAP32(A)		((((A)&0x000000ff)<<24) | (((A)&0x0000ff00)<<8) | (((A)&0x00ff0000)>>8) | (((A)&0xff000000)>>24))
#endif

#define MAX_PKT_SIZE			608


#define NOPROTOCOL				0
#define PROT_BOOTP				68
#define PROT_TFTP				69

// 전역변수.
extern uchar		broadcastEther[6];	// Ethernet boardcast address.
extern ulong		broadcastIP;
extern uchar		clientEther[6];		// Our ethernet address.
extern ulong		clientIP;			// Our IP addr (0 = unknown).
extern uchar		hostEther[6];		// Boot server enet address.
extern ulong		hostIP;				// Server IP addr (0 = unkn).
extern short		protocol;

extern uchar		PktBuf[MAX_PKT_SIZE];
//extern unsigned		IPID;				// IP ID (counting).

bool		TxPacket(char *pkt, int len);
bool		RxPacket(char *pkt);
bool		NetInit();
void		RxPacketHandle(char *pkt, long len);
void		SetEtherHeader(char *buf, char *htEth, ushort prot);
void		SetIPHeader(char *ipHeader, ulong clientIP, ulong hostIP, ushort len);
void		SetUdpHeader(char *udpHeader, ushort hPort, ushort cPort, ushort len);
unsigned	IPChksum(char *ptr, int len);
int			IPChksumOK(char *ptr, int len);
void		PrintIPAddr(long ipAddr);
void		PrintEthAddr(char *ethAddr);


#endif		// end _NET_H_532453245234534534.
