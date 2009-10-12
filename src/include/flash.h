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

#ifndef _FLASH_H_4671264823649239432432
#define _FLASH_H_4671264823649239432432

#include "main.h"

#define CMD_TBL_FLASH																				\
				{"flash", DoWriteToFlashBlocks,														\
				"  flash {loader/kernel/root}       Copy to Flash from SDRAM of Area.\n"			\
				"  flash [dest] [src] [len]         Copy to Flash from src to dest.\n",			\
				"  flash {loader/kernel/root}       Copy to Flash from SDRAM.\n",					\
				"  flash {loader/kernel/root}       Copy to Flash from SDRAM of Area.\n"			\
				"  flash [dest] [src] [len]         Copy to Flash from src to dest.\n"			\
				}

#define CMD_TBL_ERASE																				\
				{"erase", DoEraseFlashBlocks,														\
				"  erase [addr]                     Erase One Flash Block.\n"						\
				"  erase [addr] [len]               Erase Flash Blocks.\n"						\
				"  erase {loader/kernel/root}       Erase Flash Blocks of Area.\n",				\
				0,																					\
				"  erase [addr]                     Erase One Flash Block.\n"						\
				"  erase [addr] [len]               Erase Flash Blocks.\n"						\
				"  erase {loader/kernel/root}       Erase Flash Blocks of Area.\n"				\
				}

#define CMD_TBL_LOCK																				\
				{"lock", DoFlashLock,																\
				"  lock [addr]                      Set Flash Lock-Bit of One Block.\n"			\
				"  lock [addr] [len]                Set Flash Lock-Bit of Blocks.\n"				\
				"  lock {kernel/root}               Set Flash Lock-Bit of Area.\n",				\
				"  lock {kernel/root}               Set Flash Lock-Bit of Area.\n",				\
				"  lock [addr]                      Set Flash Lock-Bit of One Block.\n"			\
				"  lock [addr] [len]                Set Flash Lock-Bit of Blocks.\n"				\
				"  lock {kernel/root}               Set Flash Lock-Bit of Area.\n"				\
				}


#define CMD_TBL_UNLOCK																				\
				{"unlock", DoFlashUnlock,															\
				"  unlock                           Clear All Flash Lock-Bit.\n",					\
				"  unlock                           Clear All Flash Lock-Bit.\n",					\
				"  unlock                           Clear All Flash Lock-Bit.\n"					\
				}


#define FLASH_BASE		SRAM_BASE_ADDR
#define FLASH_BLOCK_SIZE	SRAM_BLOCK_SIZE
#define FLASH_SIZE		SRAM_SIZE

#if (FLASHBIT*FLASHNUM)==8
#define FUNIT                           uchar
#define FBOUND                          0x1
#elif (FLASHBIT*FLASHNUM)==16
#define FUNIT                           ushort
#define FBOUND                          0x2
#elif (FLASHBIT*FLASHNUM)==32
#define FUNIT                           ulong
#define FBOUND                          0x4
#endif

#ifdef XHYPER255A
// Flash Commands.
#define READ_ARRAY              (0xFF)
#define READ_ID                 (0x90)
#define READ_QUERY              (0x98)
#define STATUS_READ             (0x70)
#define STATUS_CLEAR            (0x50)
#define WRITE_BUF               (0xE8)
#define PGM_SETUP               (0x40)
#define ERASE_SETUP             (0x20)
#define ERASE_CONFIRM           (0xD0)
#define ERASE_PROGRAM_SUSPEND   (0xB0)
#define ERASE_PROGRAM_RESUME    (0xD0)
#define FLASH_CFG               (0xB8)
#define FLASH_SETUP             (0x60)
#define SET_READ_CFG            (0x03)
#define BLOCK_LOCK_BIT_SET      (0x01)
#define BLOCK_LOCK_BIT_CLEAR    (0xD0)
#define PROTECTION_PROGRAM      (0xC0)
#define WRITE_BUF_COMFIRM       (0xD0)

// Status bits.
#define STATUS_BUSY             (0x80)
#define STATUS_ERR_FILTER       (0x7E)
#define STATUS_LOCK_DETECT      (0x02)
#define STATUS_VOLT_RANG_ERR    (0x08)
#define STATUS_CMD_SEQ_ERR      (0x30)
#define STATUS_PGM_ERR          (0x10)
#define STATUS_LOCK_ERR         (0x10)
#define STATUS_ERASE_ERR        (0x20)
#define STATUS_UNLOCK_ERR       (0x20)
#define STATUS_LOCK_ERR_FILTER  (0x38)

#define FlashCMD(A)		((FUNIT)(A))

#else

// Flash Commands.
#define READ_ARRAY		(0x00FF00FF)
#define READ_ID			(0x00900090)
#define READ_QUERY		(0x00980098)
#define	STATUS_READ		(0x00700070)
#define	STATUS_CLEAR		(0x00500050)
#define WRITE_BUF		(0x00E800E8)
#define	PGM_SETUP		(0x00400040)
#define ERASE_SETUP		(0x00200020)
#define ERASE_CONFIRM		(0x00D000D0)
#define ERASE_PROGRAM_SUSPEND	(0x00B000B0)
#define ERASE_PROGRAM_RESUME	(0x00D000D0)
#define FLASH_CFG		(0x00B800B8)
#define FLASH_SETUP		(0x00600060)
#define SET_READ_CFG		(0x00030003)
#define BLOCK_LOCK_BIT_SET	(0x00010001)
#define BLOCK_LOCK_BIT_CLEAR	(0x00D000D0)
#define PROTECTION_PROGRAM	(0x00C000C0)
#define WRITE_BUF_COMFIRM	(0x00D000D0)

// Status bits.
#define STATUS_BUSY		(0x00800080)
#define STATUS_ERR_FILTER	(0x007E007E)
#define STATUS_LOCK_DETECT	(0x00020002)
#define STATUS_VOLT_RANG_ERR	(0x00080008)
#define STATUS_CMD_SEQ_ERR	(0x00300030)
#define STATUS_PGM_ERR		(0x00100010)
#define STATUS_LOCK_ERR		(0x00100010)
#define STATUS_ERASE_ERR	(0x00200020)
#define STATUS_UNLOCK_ERR	(0x00200020)
#define STATUS_LOCK_ERR_FILTER	(0x00380038)

#define FlashCMD(A)		((FUNIT)(A))
#endif

bool	DoWriteToFlashBlocks(CMD_TBL *cptr, int argc, char **argv);
bool	DoEraseFlashBlocks	(CMD_TBL *cptr, int argc, char **argv);
bool	DoFlashLock			(CMD_TBL *cptr, int argc, char **argv);
bool	DoFlashUnlock		(CMD_TBL *cptr, int argc, char **argv);

bool	WriteToFlash		(void *dest, void *src, ulong len);
bool	WriteToFlashBuffer	(void *dest, void *src);

#endif		// end _FLASH_H_4671264823649239432432.
