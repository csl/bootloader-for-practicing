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

#ifndef _UTIL_H_467326478126341264324
#define _UTIL_H_467326478126341264324

#include "main.h"

#define CMD_TBL_MEMCPY																				\
				{"memcpy", DoMemCpy,																\
				"  memcpy [dest] [src] [len]        Copy to SDRAM from Flash or SDRAM.\n",			\
				0,																					\
				"  memcpy [dest] [src] [len]        Copy to SDRAM from Flash or SDRAM.\n"			\
				}

#define CMD_TBL_MEMDUMP																				\
				{"memdump", DoMemDump,																\
				"  memdump [addr] [len]             Dump Memory.\n",								\
				0,																					\
				"  memdump [addr] [len]             Dump Memory.\n"									\
				}

#define CMD_TBL_HEXDUMP																				\
				{"hexdump", DoHexDump,																\
				"  hexdump [addr] [len]             Dump Memory.\n",								\
				0,																					\
				"  hexdump [addr] [len]             Dump Memory.\n"									\
				}

#define CMD_TBL_MEMCMP																				\
				{"memcmp", DoMemCmp,																\
				"  memcmp [addr1] [addr2] [len]     Compare data in addr1 and addr2.\n",			\
				0,																					\
				"  memcmp [addr1] [addr2] [len]     Compare data in addr1 and addr2.\n"				\
				}

#define CMD_TBL_MEMSET																				\
				{"memset", DoMemSet,																\
				"  memset [addr] [value] [len]      Fill Memory with value.\n",						\
				0,																					\
				"  memset [addr] [value] [len]      Fill Memory with value.\n"						\
				}
#define CMD_TBL_WRITE																				\
				{"write", DoWriteToReg,																\
				"  write {c/s/l} [addr] [value]     Write in addr. c:8 s:16 l:32 bits.\n",			\
				0,																					\
				"  write {c/s/l} [addr] [value]     Write in addr. c:8 s:16 l:32 bits.\n"			\
				}

#define CMD_TBL_READ																				\
				{"read", DoReadFromReg,																\
				"  read {c/s/l} [addr]              Read in addr. c:8 s:16 l:32 bits.\n",			\
				0,																					\
				"  read {c/s/l} [addr]              Read in addr. c:8 s:16 l:32 bits.\n"			\
				}



int		DoMemCpy		(CMD_TBL *cptr, int argc, char **argv);
int		DoMemDump		(CMD_TBL *cptr, int argc, char **argv);
int		DoHexDump		(CMD_TBL *cptr, int argc, char **argv);
bool	DoMemCmp		(CMD_TBL *cptr, int argc, char **argv);
int		DoMemSet		(CMD_TBL *cptr, int argc, char **argv);
int		DoWriteToReg	(CMD_TBL *cptr, int argc, char **argv);
int		DoReadFromReg	(CMD_TBL *cptr, int argc, char **argv);

#endif		// end _UTIL_H_467326478126341264324.
