/////////////////////////////////////////////////////////////////////////////////
// Copyright(c) 2001-2002 Hybus Co,.ltd. All rights reserved.
//
// Module name:
//      flash.c
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

#include "flash.h"

#ifdef XHYPER255A
#define IT(x) 	(*((volatile unsigned short *)(x)))
#else
#define IT(x) 	(*((volatile unsigned int *)(x)))
#endif

typedef enum {
	ACT_PGM,
	ACT_ERASE,
	ACT_LOCK,
	ACT_UNLOCK,
	ACT_WRITE_BUF,
} FLASH_ACTION;
	

// Static function defs.
bool			EraseFlashBlocks(FUNIT *addr, ulong len);
static bool		EraseOneFlashBlock(FUNIT *addr);
static bool		FlashLock(FUNIT *addr);
static bool		FlashUnlock();
bool			FullStatusCheck(volatile FUNIT *addr, FLASH_ACTION which);

bool DoWriteToFlashBlocks(CMD_TBL *cptr, int argc, char **argv)
{
	long	dest=0, src=0, len, wsize;
	
	switch (argc){
		case 2 :
			if (!StrCmp(argv[1], "kernel")){
				dest = (long)KERNEL_SRAM_BASE;
				src  = (long)KERNEL_DRAM_BASE;
				len  = (long)KERNEL_MAX_SIZE;
				printf("Saving kernel to flash...\n");
			}
			else if (!StrCmp(argv[1], "root")){
				dest = (long)ROOT_SRAM_BASE;
				src  = (long)ROOT_DRAM_BASE;
				len  = (long)ROOT_MAX_SIZE;
				printf("Saving root to flash...\n");
			}
			else if (!StrCmp(argv[1], "loader")){
				dest = (long)LOADER_SRAM_BASE;
				src  = (long)LOADER_DRAM_BASE;
				len  = (long)LOADER_MAX_SIZE;
				printf("Saving bootloader to flash...\n");
			}

			else {
				printf(cptr->usage);
				return false;	
			}
			break;
		// "flash [dest] [src] [len]".
		case 4 :
			if (!HexToInt(argv[1], &dest, 32) || !HexToInt(argv[2], &src, 32) ||
					!HexToInt(argv[3], &len, 32)){
				printf(cptr->usage);
				return false;
			}
			if (dest % FLASH_BLOCK_SIZE){
				printf("dest is not Flash Block Base.\n");
				return false;
			}
			break;
		default :
			printf(cptr->usage);
			return false;
			break;
	}

	// erase flash blocks.
	printf("Erase flash blocks from 0x%08lx to 0x%08lx.\n", dest, (long)dest+len-1);
	if (!EraseFlashBlocks((FUNIT *)dest, len)) return false;
	printf("\tDone.\n");

	// write to flash.
	printf("Write to flash...\n");

	wsize = FLASH_BLOCK_SIZE;

    do{
            printf("\tWriting at 0x%08lx...", (ulong)dest);
            if (!WriteToFlashBuffer((FUNIT *)dest, (FUNIT *)src)) break;

            dest += FLASH_BLOCK_SIZE;
            src += FLASH_BLOCK_SIZE;
			ClearLine();
    } while((len -= FLASH_BLOCK_SIZE) >0);

	printf("\tWriting at 0x%08lx\r\n", (ulong)dest);
	printf("\tDone\n");

	IT(dest) = FlashCMD(READ_ARRAY);	
	return true;
}	// DoWriteToFlashBlocks.


bool WriteToFlashBuffer(void *dest, void *src)
{
#ifdef XHYPER255A
	int             i;
        volatile FUNIT *s1=dest, *s2=src, result, *end=s1+(FLASH_BLOCK_SIZE/FBOUND);
        ushort writeBuf = 32;

        while (s1<end){ 
                *s1 = FlashCMD(WRITE_BUF);                      

                do {
                        *s1 = FlashCMD(STATUS_READ);
                        result = *s1; 
                 } while  ((~result & STATUS_BUSY));                     

                *s1 = (writeBuf)/2 - 1;

                for (i=0; i<(writeBuf)/FBOUND; i++)
			*s1++ = *s2++;

		IT((s1)) = FlashCMD(WRITE_BUF_COMFIRM);

		do {
			result = IT(s1);
		} while((result & STATUS_BUSY ) != STATUS_BUSY);
		IT(s1) = FlashCMD(READ_ARRAY);
        }
#else
	ulong		i=0;
	volatile uchar *s1=(uchar *)dest; 
	uchar *s2=(uchar *)src;
	
	ushort  writeBuf = 32;
		
	while(i < FLASH_BLOCK_SIZE)
	{
		int cnt = writeBuf / sizeof(ushort);
		int result;
			
		do {
			IT(s1) = WRITE_BUF;
			result = IT(s1);
		} while((result & STATUS_BUSY ) != STATUS_BUSY);

		IT(s1) = FlashCMD((writeBuf / sizeof(ushort) -1) | \
				((writeBuf / sizeof(ushort) -1 ) << (sizeof(ulong) * 4)));

		while(cnt--)
		{
				IT((s1+i)) = IT(s2);
				i += sizeof(ulong);
				s2 += sizeof(ulong);
		}
		IT((s1)) = FlashCMD(WRITE_BUF_COMFIRM);

		do {
			result = IT(s1);
		} while((result & STATUS_BUSY ) != STATUS_BUSY);

		IT(s1) = FlashCMD(READ_ARRAY);
	}
#endif
	return true;
}


bool EraseFlashBlocks(FUNIT *addr, ulong len)
{
	FUNIT	*ptr, *end=addr+len/FBOUND;

	// erase block loop.
	for (ptr=addr; ptr<end; ptr+=FLASH_BLOCK_SIZE/FBOUND){
		ClearLine();
		printf("\tErase Block at : 0x%08lx.", (long)ptr);
		// erase block.
		if (!EraseOneFlashBlock(ptr)){
			return false;
		}
	}
	printf("\n");
	return true;
}


bool DoEraseFlashBlocks(CMD_TBL *cptr, int argc, char **argv)
{
	ulong	addr=0, len, end;
	
	switch (argc){
        case 2 :
            if (!StrCmp(argv[1], "kernel")){
                addr = (long)KERNEL_SRAM_BASE;
                len  = (long)KERNEL_MAX_SIZE;
            }
            else if (!StrCmp(argv[1], "root")){
                addr = (long)ROOT_SRAM_BASE;
                len  = (long)ROOT_MAX_SIZE;
            }
            else if (!StrCmp(argv[1], "loader")){
                addr = (long)LOADER_SRAM_BASE;
                len  = (long)LOADER_MAX_SIZE;
            } 
            else {
                printf(cptr->usage);
                return false;
            }
			if ((long)addr%FLASH_BLOCK_SIZE){
				printf("addr is not flash block base.\n");
				return false;
			}

		    // erase flash blocks.
		    printf("Erase flash blocks from 0x%08lx to 0x%08lx.\n", addr, (long)addr+len-1);
		    if (!EraseFlashBlocks((FUNIT *)addr, len)) return false;

		    printf("\tDone.\n");	
        
		    break;

			// "erase [addr] [len]".
	case 3 :
		// error.
		if (!HexToInt(argv[2], &len, 32)){
			printf(cptr->usage);
			return false;
		}
		if ((long)addr%FLASH_BLOCK_SIZE){
			printf("addr is not flash block base.\n");
			return false;
		}

		// erase flash blocks.
		end = addr + len;
		for (; addr<end; addr+=FLASH_BLOCK_SIZE){
			if (!EraseOneFlashBlock((FUNIT *)addr)) return false;
		}
		break;
	// error.
	default :
		printf(cptr->usage);
		return false;
		break;
	}
	return true;
}	// DoEraseOneFlashBlock.


static bool EraseOneFlashBlock(FUNIT *addr)
{
	volatile FUNIT	*s=addr;

	// error.
	if ((long)addr%FLASH_BLOCK_SIZE) return false;
	
	// do command.
	*s = FlashCMD(ERASE_SETUP);
	*s = FlashCMD(ERASE_CONFIRM);

	if (!FullStatusCheck(s, ACT_ERASE)) return false;
	return true;
}	// EraseOne.


bool DoFlashLock(CMD_TBL *cptr, int argc, char **argv)
{
	long	i, addr, len, cnt;
	// error.
	if (argc < 2){
		printf(cptr->usage);
		return false;
	}

	switch (argc){
		// "lock [addr]".
		case 2 :
			if (!StrCmp(argv[1], "loader")){
				addr = (long)LOADER_SRAM_BASE;
				len  = (long)LOADER_SRAM_MAX_SIZE;
			}
			else if (!StrCmp(argv[1], "kernel")){
				addr = (long)KERNEL_SRAM_BASE;
				len  = (long)KERNEL_MAX_SIZE;
			}
			else if (!StrCmp(argv[1], "root")){
				addr = (long)ROOT_SRAM_BASE;
				len  = (long)ROOT_MAX_SIZE;
			}
			else {
				if (!HexToInt(argv[1], &addr, 32)){
					printf(cptr->usage);
					return false;
				}
				len = (long)FLASH_BLOCK_SIZE;
			}
			break;
		// "lock [addr] [len]".
		default :
			printf(cptr->usage);
			return false;
			break;
	}
	// lock of flash blocks.
	cnt = len/FLASH_BLOCK_SIZE + ((len&(FLASH_BLOCK_SIZE-1)) ? 1 : 0);
	for (i=0; i<cnt; i++, addr+=FLASH_BLOCK_SIZE){
		if (!FlashLock((FUNIT *)addr)) return false;
	}
	return true;
}	// DoFlashLock.


bool DoFlashUnlock(CMD_TBL *cptr, int argc, char **argv)
{
	// error.
	if (argc != 1){
		printf(cptr->usage);
		return false;
	}

	// Unlock of whole flash blocks.
	if (!FlashUnlock()) return false;
	return true;
}	// DoFlashUnlock.


bool FlashLock(FUNIT *addr)
{
	volatile FUNIT	*s=addr;

	// error.
	if ((long)addr % FLASH_BLOCK_SIZE){
		printf("addr is not flash block base.\n");
		return false;
	}
	
	// do command.
	*s = FlashCMD(FLASH_SETUP);
	*s = FlashCMD(BLOCK_LOCK_BIT_SET);
	
	if (!FullStatusCheck(s, ACT_LOCK)) return false;
	return true;
}	// FlashLock.


bool FlashUnlock(void)
{
	volatile FUNIT	*s=(FUNIT *)FLASH_BASE;

	// do command.
	*s = FlashCMD(FLASH_SETUP);
	*s = FlashCMD(BLOCK_LOCK_BIT_CLEAR);
	
	if (!FullStatusCheck(s, ACT_UNLOCK)) return false;
	return true;
}	// FlashUnlock.



// error true return.
bool FullStatusCheck(volatile FUNIT *addr, FLASH_ACTION which)
{
	FUNIT	result;
	
	// delay before command is done.
	do {
		*addr = FlashCMD(STATUS_READ);
		result = *addr;
	} while  ((~result & STATUS_BUSY));
	*addr = FlashCMD(READ_ARRAY);

	return true;
	// if no error, return true.
	if ((result & STATUS_ERR_FILTER) == 0) return true;

	// if error, print out error message.
	if (result & STATUS_LOCK_DETECT){
		printf("Block Lock-bit detected.\n\n");
	} else if (result & STATUS_VOLT_RANG_ERR){
		printf("Voltage Range Error.\n\n");
	} else if ((result & STATUS_CMD_SEQ_ERR) == STATUS_CMD_SEQ_ERR){
		printf("Command Sequence Error.\n\n");
  	} else if ((result & STATUS_PGM_ERR) && ACT_PGM){
  	printf("\nWrite error at address 0x%08lx\n\n", (long)addr);
	} else if ((result & STATUS_LOCK_ERR) && ACT_LOCK){
		printf("Set Lock-Bit Error.\n\n");
	} else if ((result & STATUS_ERASE_ERR) && ACT_ERASE){
		printf("Fail to erase. retry again.\n\n");
	} else if ((result & STATUS_UNLOCK_ERR) && ACT_UNLOCK){
		printf("Clear Lock-Bit Error.\n\n");
	}
	else {
		printf("Unknown Error.\n\n");
	}

	*addr = FlashCMD(STATUS_CLEAR);
	*addr = FlashCMD(READ_ARRAY);
	return true;
}
