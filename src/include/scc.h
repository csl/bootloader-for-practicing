/////////////////////////////////////////////////////////////////////////////////
// Copyright(c) 2005 2111010@uestc. All rights reserved.
//  
// Module name:
//      scc.h
//  
// Description:
//  
//  
// Author:
//      lijilong
//  
// Created:
//      2005
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCC_H_42342134324234
#define _SCC_H_42342134324234

#include "main.h"

typedef struct {
	union {
		ushort	io_rtx_data16;	// data read and write (4 bytes °ø°£).
		ulong	io_rtx_data32;
	} _BUF1;
	ushort		io_txcmd;				// transmit command (0144h).
	ushort		io_txlen;				// transmit data length (0146h).
	ushort		io_int;					// Interrupt Status Queue (0120h).
	ushort		io_ppptr;				// Packet Page Pointer.
	union {
		ushort	io_ppdata16;	// Packet Page Data (16 bits).
		ulong	io_ppdata32;	// Packet Page Data (32 bits).
	} _BUF2;
} ETH_IO_MAP;

// GPIO register
#define GPIOREG_BASE 0x40E00000
#define GAFREG0_L (*((volatile ulong *)(GPIOREG_BASE + 0x54))) //gpio(15) alternate function
#define GAFREG1_U (*((volatile ulong *)(GPIOREG_BASE + 0x60))) //gpio(49) alternale function
#define GAFREG2_L (*((volatile ulong *)(GPIOREG_BASE + 0x64))) //gpio(79) alternale function

#define GAFREG0_VALUE 0x80000000  //gpio15 alternate function
#define GAFREG1_VALUE 0x00000A2A  //0x00000008 gpio48,49,50,52,53 alternate function
#define GAFREG2_VALUE 0x80000000  //gpio 79 alternate function

#define GPDREG0 (*((volatile ulong *)(GPIOREG_BASE + 0x0c)))
#define GPDREG1 (*((volatile ulong *)(GPIOREG_BASE + 0x10)))
#define GPDREG2 (*((volatile ulong *)(GPIOREG_BASE + 0x14)))

#define GPD0_VALUE 0x00008000//GP15 OUT
#define GPD1_VALUE 0x00370000//x00370000 //GP48,49,50,52,53 OUT
#define GPD2_VALUE 0x00008000//gp79 out

// IO Mapped Packet Page.
#define	IO_BASE		0x04000000
#define BANK_SELECT	*((volatile short *)(IO_BASE + 0x0000000E))	// Bank select.

//BANK select value
#define BANK0 0x0000
#define BANK1 0x0001
#define BANK2 0x0002
#define BANK3 0x0003

//regisers in bank0
#define IO_TCR	    *((volatile short *)(IO_BASE + 0x00000000))// transmission control reg.
#define IO_EPHSR    *((volatile short *)(IO_BASE + 0x00000002))// EPH status reg.
#define IO_RCR	    *((volatile short *)(IO_BASE + 0x00000004))// Receive control reg.
#define IO_ECR	    *((volatile short *)(IO_BASE + 0x00000006))// ETH counter reg.
#define IO_MIR	    *((volatile short *)(IO_BASE + 0x00000008))// memory information reg.
#define IO_RPCR	    *((volatile short *)(IO_BASE + 0x0000000A))// receive/PHY control reg.

#define RXEN 0x0100
//enable pad
#define PADEN 0x0080
//registers in bank1
#define IO_CR       *((volatile short *)(IO_BASE + 0x00000000))// configuration reg.
#define IO_BAR      *((volatile short *)(IO_BASE + 0x00000002))// base address reg(not be used).
#define MAC_ADDR0   *((volatile uchar *)(IO_BASE + 0x00000004))// MAC address0.
#define MAC_ADDR1   *((volatile uchar *)(IO_BASE + 0x00000005))// MAC address2.
#define MAC_ADDR2   *((volatile uchar *)(IO_BASE + 0x00000006))// MAC address4.
#define MAC_ADDR3   *((volatile uchar *)(IO_BASE + 0x00000007))
#define MAC_ADDR4   *((volatile uchar *)(IO_BASE + 0x00000008))
#define MAC_ADDR5   *((volatile uchar *)(IO_BASE + 0x00000009))
#define IO_GPR       *((volatile unsigned short *)(IO_BASE + 0x0000000A))// general purpose reg(not be used).
#define IO_CTR      *((volatile short *)(IO_BASE + 0x0000000C))// control reg.

//configuration of IO_CTR
#define AUTORELEASE   0x0820 

//information of configuration reg.
#define EPHEN_WAIT 0x9000 //Enable EHP.add several waits on ARDY because interface asynchronise bus.nCNTRL not be used.external phy is disable.

//registers in bank2
#define IO_MMUCR    *((volatile short *)(IO_BASE + 0x00000000))// MMU command reg.
#define IO_PNR      *((volatile char *)(IO_BASE + 0x00000002))//packet number reg in tx area.
#define IO_ARR      *((volatile char *)(IO_BASE + 0x00000003))//allocation result number reg.
#define IO_FIFO     *((volatile short *)(IO_BASE + 0x00000004))// FIFO ports reg.
#define IO_PTR      *((volatile short *)(IO_BASE + 0x00000006))// pointer reg.
#define IO_DATA     *((volatile short *)(IO_BASE + 0x00000008))// data reg.
#define IO_ISR      *((volatile char  *)(IO_BASE + 0x0000000C))// interrupt status reg.
#define IO_MSK      *((volatile char  *)(IO_BASE + 0x0000000C))// interrupt mask reg.
#define RCV_DATA    *((volatile unsigned long *)(IO_BASE + 0x00000008))//receive data reg.
#define LEFT_SHORT  *((volatile unsigned short *)(IO_BASE + 0x00000008))
#define LEFT_CHAR   *((volatile unsigned char *)(IO_BASE + 0x00000008))

//value of pointer register
#define RCV 0x8000
#define AUTOINCR 0x4000
#define READ 0x2000

//MMU command
#define ALLOCMEM 0x0020
#define ENQPACKET 0x00C0 

//registers in bank3
#define IO_MT01     *((volatile short  *)(IO_BASE + 0x00000000))// muticast table reg01.
#define IO_MT23     *((volatile short  *)(IO_BASE + 0x00000002))// muticast table reg23.
#define IO_MT45     *((volatile short  *)(IO_BASE + 0x00000004))// muticast table reg45.
#define IO_MT67     *((volatile short  *)(IO_BASE + 0x00000006))// muticast table reg67.
#define IO_MGMT     *((volatile ushort  *)(IO_BASE + 0x00000008))// management interface reg.
#define IO_REV      *((volatile short  *)(IO_BASE + 0x0000000A))// revision reg.
#define IO_ERCV     *((volatile short  *)(IO_BASE + 0x0000000C))// early rcv reg.

#define MDOE_MDCLK  0x0c

//register in PHY
#define PHY_CNTR              0x00
#define PHY_STATUS            0x01
#define PHY_ID1               0x02
#define PHY_ID2               0x03
#define AUTONEG_ADVERTISE     0x04
#define REMOTE_CAPABILITY     0x05
#define PHY_CFG1              0x10
#define PHY_CFG2              0x11
#define STATUS_OUT            0x12
#define PHY_MASK              0x13

#define MDOE 0x0008
#define MDCLK 0x0004
#define MDO  0x0001


bool	EthInit();					// Initialize the device.
bool	EthTx(void *pkt, int len);	// Send a packet.

#define IO_REV      *((volatile short  *)(IO_BASE + 0x0000000A))// revision reg.
#define IO_ERCV     *((volatile short  *)(IO_BASE + 0x0000000C))// early rcv reg.

bool	EthInit();					// Initialize the device.
bool	EthTx(void *pkt, int len);	// Send a packet.
bool	EthRx(void *pkt);			// Check for received packets.
void	EthHalt(void);				// stop SCC.
void	EthRestart(void);			// restart scc.
void   SetMiiFrame(uchar * framebuf,uchar read,uchar write,uchar regadd,ushort data);
void   SendReadFrame(uchar * framebuf,ushort *ptr);
void   RecMiiFrame(ushort *recptr);
void   SendWriteFrame(uchar *SendFrame);
void   ReadReg(uchar reg,ushort *ptr);
void   WriteReg(uchar reg,ushort data);

#endif			// end _SCC_H_42342134324234.
