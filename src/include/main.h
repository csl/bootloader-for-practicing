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

#ifndef _MAIN_H_465241632646492421432
#define _MAIN_H_465241632646492421432

#include "types.h"
#include "config.h"
#include "time.h"
#include "serial.h"
#include "lstring.h"
#include "command.h"

#define ADDR32(A)			(*((volatile ulong *)(A)))
#define ADDR16(A)			(*((volatile ushort *)(A)))
#define ADDR8(A)			(*((volatile uchar *)(A)))

typedef struct {
	long	terminalSpeed;
	long	downloadSpeed;
} LOADER_STATUS;

// Byte swapping.
#define SWAP8(A)		(A)
#define SWAP16(A)		((((A)&0x00ff)<<8) | ((A)>>8))
#define SWAP32(A)		((((A)&0x000000ff)<<24) | (((A)&0x0000ff00)<<8) | (((A)&0x00ff0000)>>8) | (((A)&0xff000000)>>24))


#define NEWLINE					"\n"

extern LOADER_STATUS	status;

#endif		// end _MAIN_H_465241632646492421432.
