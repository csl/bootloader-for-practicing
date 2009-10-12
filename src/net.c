/////////////////////////////////////////////////////////////////////////////////
// Copyright(c) 2001-2002 Hybus Co,.ltd. All rights reserved.
//
// Module name:
//      net.c
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
#include "scc.h"
#include "bootp.h"
#include "tftp.h"


uchar	hostEther[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
ulong	hostIP=0;
#ifdef	CLIENT_ETHER
uchar	clientEther[6] = CLIENT_ETHER;
#else
uchar	clientEther[6];
#endif
ulong	clientIP=0;
short	protocol=NOPROTOCOL;
short	ipID;

uchar	PktBuf[MAX_PKT_SIZE];
//uchar   SendPktBuf[MAX_PKT_SIZE];

bool TxPacket(char *txPktBuf, int len){
	return EthTx(txPktBuf, len);
}	// TxPacket.


bool RxPacket(char *rxPktBuf){
	return EthRx(rxPktBuf);
}


bool NetInit(){
	return EthInit();
}


void RxPacketHandle(char *rxPktBuf, long len){
	ETH_HEADER		*et = (ETH_HEADER *)rxPktBuf;
	IP_HEADER		*ip = (IP_HEADER *)(rxPktBuf+ETHER_HDR_SIZE);
	UDP_HEADER		*udp = (UDP_HEADER *)(rxPktBuf+ETHER_HDR_SIZE+IP_HDR_SIZE);
	ARP_HEADER		*arp = (ARP_HEADER *)(rxPktBuf+ETHER_HDR_SIZE);
	ushort			prot;

	// get protocol type.
	// IP   : 0x0800.
	// ARP  : 0x0806.
	// RARP : 0x8035.
	prot = SWAP16(et->et_protlen);

	switch (prot){
		case PROT_ARP:
			// error check.
			if (len<ARP_HDR_SIZE) return;
			if (SWAP16(arp->ar_op) != ARPOP_REQUEST) return;
			if (SWAP16(arp->ar_hrd) != ARP_ETHER) return;
			if (SWAP16(arp->ar_pro) != PROT_IP) return;
			if (SWAP16(arp->ar_hln != 6)) return;
			if (SWAP16(arp->ar_pln != 4)) return;
			if (clientIP==0 || MemCmp(&(arp->ar_data[16]), &clientIP, 4)) return;

			// ethernet header.
			MemCpy(&rxPktBuf[0], (char *)et->et_src, 6);
			MemCpy(&rxPktBuf[6], (char *)clientEther, 6);
			*(short *)(&rxPktBuf[12]) = SWAP16(PROT_ARP);
			// ARP packet.
			// hard type.
			*(short *)(&rxPktBuf[14]) = SWAP16(0x0001);
			// prot type.
			*(short *)(&rxPktBuf[16]) = SWAP16(0x0800);
			// hardware address size.
			rxPktBuf[18]              = 0x06;
			// prot size.
			rxPktBuf[19]              = 0x04;
			// op.
			*(short *)(&rxPktBuf[20]) = SWAP16(ARPOP_REPLY);
			// Sender Ethernet Address.
			MemCpy(&(rxPktBuf[32]), &(rxPktBuf[22]), 6);
			// Sender IP Address.
			MemCpy(&(rxPktBuf[38]), &(rxPktBuf[28]), 4);
			// Target Ethernet Address.
			MemCpy(&(rxPktBuf[22]), clientEther, 6);
			// Target IP Address.
			MemCpy(&(rxPktBuf[28]), &(clientIP), 4);
			// ARP packet을 보냄.
			EthTx(rxPktBuf, ETHER_HDR_SIZE+ARP_HDR_SIZE);
			break;
		case PROT_IP:
			// error check.
			// length check.
			if (len < ETHER_HDR_SIZE+IP_HDR_SIZE+UDP_HDR_SIZE) return;	// udp만 사용하므로 udp 포함하여 길이 check.
			if (len < ETHER_HDR_SIZE+SWAP16(ip->ip_len)) return;
			// 현재 ip version은 4.
			if ((ip->ip_hl_v & 0xf0) != 0x40) return;
			// Can't deal fragments.
			if (ip->ip_off & SWAP16(0x1fff)) return;
			// check checksum.
			if (!IPChksumOK((char *)ip, IP_HDR_SIZE / 2)) return;
			// client의 ip와 받은 packet의 ip 비교.
			if (clientIP && MemCmp(&ip->ip_dest, &clientIP, 4)) return;
			// udp인지 check. 17은 udp의 번호.
			if (ip->ip_p!=17) return;

			// packet을 처리할 수 있는 함수를 호출.
			switch (protocol){
				case PROT_BOOTP :
					// check udp.
					if (udp->udp_src!=SWAP16(bootps) || udp->udp_dest!=SWAP16(bootpc)) break;
					// bootp 처리 함수 호출. 올바른 Bootp Reply이면, Host의 Ethernet 주소를 기록함.
					if (BootpRx((char *)(rxPktBuf+ETHER_HDR_SIZE+IP_HDR_SIZE+UDP_HDR_SIZE),
							SWAP16(udp->udp_len)-UDP_HDR_SIZE)==true){
						MemCpy(hostEther, (char *)et->et_src,6);
					}
					break;
				case PROT_TFTP :
					// ckeck udp.
					if (udp->udp_src!=SWAP16(tftpPort) && (tftpHostPort!=0 && udp->udp_src!=SWAP16(tftpHostPort))) break;
					if (udp->udp_dest!=SWAP16(tftpPort) && udp->udp_dest!=SWAP16(tftpClientPort)) break;
					if (tftpHostPort==0) tftpHostPort=SWAP16(udp->udp_src);
					// tftp 처리 함수 호출.
					TftpRx((char *)(rxPktBuf+ETHER_HDR_SIZE+IP_HDR_SIZE+UDP_HDR_SIZE),
							SWAP16(udp->udp_len)-UDP_HDR_SIZE);
					break;
				default :
					break;
			}
			break;
	}
	return;
}	// RxPacketHandle.


int IPChksumOK(char *ptr, int len){
	return !((IPChksum(ptr, len) + 1) & 0xfffe);
}	// IPCksumOk.


unsigned IPChksum(char *ptr, int len){
	ulong		xsum;

	xsum = 0;
	while (len-- > 0)
		xsum += *((ushort *)ptr)++;
	xsum = (xsum & 0xffff) + (xsum >> 16);
	xsum = (xsum & 0xffff) + (xsum >> 16);
	return xsum & 0xffff;
}	// IPChksum.


void SetEtherHeader(char *ethHeader, char *hEth, ushort prot){
	volatile ETH_HEADER		*et = (ETH_HEADER *)ethHeader;
	
	MemCpy((char *)et->et_dest, hEth, 6);
	MemCpy((char *)et->et_src, clientEther, 6);
	et->et_protlen = SWAP16(prot);	// set 800 (ip).
	return;
}	// SetEtherHeader.


void SetIPHeader(char *ipHeader, ulong clientIP, ulong hostIP, ushort len){
	volatile IP_HEADER	*ip = (volatile IP_HEADER *)ipHeader;

	// If the data is an odd number of bytes, zero the
	// byte after the last byte so that the checksum will work.
	if (len & 1) ipHeader[IP_HDR_SIZE + len] = 0;

	// Construct an IP header(need to set no fragment bit - XXX).
	ip->ip_hl_v                   = 0x45;						// 현재 ip version 4, IP_HDR_SIZE / 4 (not including UDP)
	ip->ip_tos                    = 0;							// type of service. 현재 network에서 지원안함. 항상 0.
	ip->ip_len                    = SWAP16(IP_HDR_SIZE + len);	// total length of ip packet.
	ip->ip_id                     = SWAP16(ipID++);				// identifier.
	ip->ip_off                    = SWAP16(0x0000);				// No fragmentation.
	ip->ip_ttl                    = 64;							// time to live.
	ip->ip_p                      = 17;							// UDP.
	ip->ip_chksum                 = 0;
	MemCpy((char *)&(ip->ip_src), &(clientIP), 4);
	MemCpy((char *)&(ip->ip_dest), &(hostIP), 4);
//	(volatile long)ip->ip_src     = clientIP;	
//	(volatile long)ip->ip_dest    = hostIP;
	ip->ip_chksum = ~IPChksum((char *)ip, IP_HDR_SIZE / 2);
}	// SetIPHeader.


void SetUdpHeader(char *udpHeader, ushort hPort, ushort cPort, ushort len){
	volatile UDP_HEADER		*udp = (UDP_HEADER *)udpHeader;

	udp->udp_dest    = SWAP16(hPort);
	udp->udp_src     = SWAP16(cPort);
	udp->udp_len     = SWAP16(UDP_HDR_SIZE + len);
	udp->udp_chksum  = 0;
	return;
}	// SetUdpHeader.


void PrintIPAddr(long ip){
	ip = SWAP32(ip);
	printf("%d.%d.%d.%d", (char)((ip>>24)&0xff), (char)((ip>>16)&0xff),
					(char)((ip>>8)&0xff), (char)((ip>>0)&0xff));
	return;
}	// NetPrintIPaddr.


void PrintEthAddr(char *ethAddr){
	int		i;

	for (i=0; i<6; i++){
		if (i && i%2==0) printf(" ");
		printf("%02x", ethAddr[i]);
	}
	return;
}
