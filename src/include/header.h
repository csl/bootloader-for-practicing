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

#ifndef _PROTOCOL_H_42342134324234
#define _PROTOCOL_H_42342134324234

#include <types.h>

// cs8900a packet page.
typedef struct {
	// Bus Interface Registers
	ulong	pp_id;				// Product identification.
	uchar	pp_res1[28];		// reserved.
	ushort	pp_baddr;			// ip base address.
	ushort	pp_int_num;			// interrupt number.
	ushort	pp_dchan;			// DMA Channel Number.
	ushort	pp_dstart_frame;	// DMA Start of Frame.
	ushort	pp_dframe_cnt;		// DMA Frame Count.
	ushort	pp_rx_dbyte_cnt;	// RxDMA Byte Count.
	ulong	pp_mbase_addr;		// Memory Base Address Register.
	ulong	pp_bprom_base_addr;	// Boot PROM Base Address.
	ulong	pp_bprom_addr_mask;	// Boot PROM Address Mask.
	uchar	pp_res2[8];			// reserved.
	ushort	pp_eeprom_cmd;		// EEPROM Command.
	ushort	pp_eeprom_data;		// EEPROM Data.
	uchar	pp_res3[12];		// reserved.
	ushort	pp_rxf_byte_cnt;	// Received Frame Byte Counter.
	uchar	pp_res4[174];		// reserved.
	// Status and Control Registers.
	uchar	pp_conf_ctrl[32];	// Configuration & Control Registers.
	uchar	pp_state_event[32];	// Status & Event Registers.
	uchar	pp_res5[4];
	// Initiate Transmit Registers.
	ushort	pp_txcmd;			// TxCMD (transmit command).
	ushort	pp_txlen;			// TxLENGTH (transmit length).
	uchar	pp_res6[8];			// reserved.
	// Address Filter Registers.
	uchar	pp_addr_filter[8];	// Logical Address Filter.
	uchar	pp_iaddr[6];		// Individual Address.
	uchar	pp_res7[674];
	// Frame Location.
	ushort	pp_rxstatus;		// RxStatus.
	ushort	pp_rxlen;			// RxLength.
	uchar	pp_rxfloc[1532];	// Rx Frame Location.
	uchar	pp_txfloc[1536];	// Tx Frame Location.
} PACKET_PAGE;					// cs8900a.


// Ethernet Header.
// ethernet header를 사용하면 et_protlen까지만 사용하며,
// et_protlen은 protocol type으로 사용한다.
typedef struct {
	uchar		et_dest[6];		// Destination Mac Address.
	uchar		et_src[6];		// Source Mac Address.
	ushort		et_protlen;		// if ethernet header protocol, else length.
	uchar		et_dsap;		// DSAP.
	uchar		et_ssap;		// SSAP.
	uchar		et_ctl;			// contorl.
	uchar		et_snap1;		// SNAP.
	uchar		et_snap2;
	uchar		et_snap3;
	ushort		et_prot;		// protocol.
} ETH_HEADER;
#define	ETHER_HDR_SIZE			14
#define	E802_HDR_SIZE			22
#define PROT_IP					0x0800
#define	PROT_ARP				0x0806
#define PROT_RARP				0x8035


// IP Header.
typedef struct {
	char	ip_hl_v;	// version and header length(version은 현재 4, length는 4 byte 단위).
	char	ip_tos;		// type of service.
	short	ip_len;		// total length (IP Header를 포함한 길이).
	short	ip_id;		// identification.
	short	ip_off;		// fragment offset field.
	char	ip_ttl;		// time to live (원하는 값, 0x40 or 0xff).
	char	ip_p;		// protocol (UDP:17).
	short	ip_chksum;	// checksum.
	long	ip_src;		// source ip address.
	long	ip_dest;	// destination ip address.
} IP_HEADER;
#define IP_HDR_SIZE		20


// udp header.
typedef struct {
	ushort	udp_src;	// udp source port.
	ushort	udp_dest;	// udp destination port.
	ushort	udp_len;	// length of udp packet.
	ushort	udp_chksum;	// checksum.
} UDP_HEADER;
#define UDP_HDR_SIZE	8


// Address Resolution Protocol (ARP) header.
typedef struct {
	ushort		ar_hrd;		// Format of hardware address.
	ushort		ar_pro;		// Format of protocol address.
	uchar		ar_hln;		// Length of hardware address.
	uchar		ar_pln;		// Length of protocol address.
	ushort		ar_op;		// Operation.
	uchar		ar_data[0];
#if 0
	uchar		ar_sha[];	// Sender hardware address.
	uchar		ar_spa[];	// Sender protocol address.
	uchar		ar_tha[];	// Target hardware address.
	uchar		ar_tpa[];	// Target protocol address.
#endif // 0
} ARP_HEADER;
#define ARP_HDR_SIZE			(8+20)	// Size assuming ethernet.
// for ar_hrd of ARP_HEADER.
#define ARP_ETHER				1		// Ethernet hardware address.
// for ar_op of ARP_HEADER.
#define ARPOP_REQUEST			1		// Request to resolve address.
#define ARPOP_REPLY				2		// Response to previous request.
#define RARPOP_REQUEST			3		// Request to resolve address.
#define RARPOP_REPLY			4		// Response to previous request.

// BOOTP header.
typedef struct {
	uchar		bh_opcode;		// Operation.
	uchar		bh_htype;		// Hardware type.
	uchar		bh_hlen;		// Hardware address length.
	uchar		bh_hops;		// Hop count (gateway thing).
	ulong		bh_tid;			// Transaction ID.
	ushort		bh_secs;		// Seconds since boot.
	ushort		bh_res1;		// Reserved(alignment).
	ulong		bh_ciaddr;		// Client IP address.
	ulong		bh_yiaddr;		// Your (client) IP address.
	ulong		bh_siaddr;		// Server IP address(Server가 setting).
	ulong		bh_giaddr;		// Gateway IP address.
	uchar		bh_chaddr[16];	// Client hardware address(Mac Address).
	char		bh_sname[64];	// Server host name.
	char		bh_file[128];	// Boot file name.
	char		bh_vend[64];	// Vendor information.
} BOOTP_HEADER;
#define BOOTP_HDR_SIZE			300
#define BOOTP_SIZE				(ETHER_HDR_SIZE + IP_HDR_SIZE + UDP_HDR_SIZE + BOOTP_HDR_SIZE)
#define OP_BOOTREQUEST			1
#define OP_BOOTREPLY	 		2
#define HWT_ETHER	 			1
#define HWL_ETHER			    0x06


#endif			// end _PROTOCOL_H_42342134324234.
