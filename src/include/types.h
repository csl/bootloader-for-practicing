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

#ifndef _TYPES_H_4782374832742374327423
#define _TYPES_H_4782374832742374327423

typedef unsigned long	ulong;
typedef unsigned short	ushort;
typedef unsigned char	uchar;
typedef unsigned int	uint;

#ifndef __cplusplus
typedef int				bool;
#define	true			1
#define false			0
#endif

// print in hex value.
// type= 8 : print in format "ff".
// type=16 : print in format "ffff".
// type=32 : print in format "ffffffff".
typedef enum {
	VAR_LONG=32,
	VAR_SHORT=16,
	VAR_CHAR=8
} VAR_TYPE;

#ifndef NULL
#define NULL (void *)0
#endif

#endif		// end _TYPES_H_4782374832742374327423.
