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

#ifndef _TFTP_H_65478236478614934324
#define _TFTP_H_65478236478614934324

#include "main.h"

#define CMD_TBL_TFTP																				\
				{"tftp", DoTftp,																	\
				"  tftp [file] {loader/kernel/root} Run Tftp. Get file and store in Area.\n"		\
				"  tftp [file] [addr]               Run Tftp. Get file and store in addr.\n",		\
				"  tftp [file] {loader/kernel/root} Run Tftp. Get file and store in Area.\n",		\
				"  tftp [file] {loader/kernel/root} Run Tftp. Get file and store in Area.\n"		\
				"  tftp [file] [addr]               Run Tftp. Get file and store in addr.\n",		\
				}

// Prototypes.
bool			DoTftp(CMD_TBL *cptr, int argc, char **argv);	// Begin TFTP get.
bool			TftpRx(char *rxBuf, int len);

extern ushort	tftpPort;
extern ushort	tftpHostPort;
extern ushort	tftpClientPort;

#endif		// end _TFTP_H_65478236478614934324.
