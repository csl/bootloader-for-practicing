/////////////////////////////////////////////////////////////////////////////////
// Copyright(c) 2001-2002 Hybus Co,.ltd. All rights reserved.
//  
// Module name:
//     	bootp.h 
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

#ifndef _BOOTP_H_1251453153242423432
#define _BOOTP_H_1251453153242423432

#include "main.h"


bool	DoBootp(CMD_TBL *cptr, int argc, char **argv);
bool	BootpRx(char *rxBuf, short len);

extern ushort	bootps;
extern ushort	bootpc;

#define CMD_TBL_BOOTP																				\
				{"bootp", DoBootp,																	\
				"  bootp                            Run Bootp. Get ip and host infomation.\n",	\
				0,																					\
				"  bootp                            Run Bootp. Get ip and host infomation.\n"		\
				}

#endif		// end _BOOTP_H_1251453153242423432.
