/////////////////////////////////////////////////////////////////////////////////
// Copyright(c) 2001-2002 Hybus Co,.ltd. All rights reserved.
//
// Module name:
//      command.c
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

#include "main.h"
#include "command.h"
#include "time.h"
#include "scc.h"
#include "bootp.h"
#include "tftp.h"
#include "flash.h"
#include "util.h"
#include "stdarg.h"

bool DoResetTerminal	(CMD_TBL *cptr, int argc, char **argv);
bool DoPrintStatus		(CMD_TBL *cptr, int argc, char **argv);
bool DoTest				(CMD_TBL *cptr, int argc, char **argv);
bool DoReboot			(CMD_TBL *cptr, int argc, char **argv);


void foo(char *fmt, ...);



#define CMD_TBL_BOOT																				\
				{"boot", DoBootKernel,																\
				"  boot                             Booting the Kernel.\n",						\
				"  boot                             Booting the Kernel.\n"						\
				}


#define CMD_TBL_RELOAD																				\
				{"reload", DoReload,																\
				"  reload {kernel}          Copy to SDRAM from Flash in Area.\n",			\
				"  reload {kernel}          Copy to SDRAM from Flash in Area.\n"			\
				}

#define CMD_TBL_STATUS																				\
				{"status", DoPrintStatus,															\
				"  status                           View loader status.\n",						\
				0,																					\
				"  status                           View loader status.\n"						\
				}


#define CMD_TBL_REBOOT																				\
				{"reboot", DoReboot,															\
				"  reboot                           Software reboot.\n",						\
				0,																					\
				"  reboot                           Software reboot.\n"						\
				}
#define CMD_TBL_TEST																				\
				{"test", DoTest,																	\
				0,																					\
				0,																					\
				0																					\
				}

#define CMD_TBL_END		{0, 0, 0, 0, 0}



CMD_TBL cmdTbl[] = {
	CMD_TBL_RELOAD,
	CMD_TBL_BOOTP,
	CMD_TBL_TFTP,
	CMD_TBL_FLASH,
	CMD_TBL_ERASE,
	CMD_TBL_LOCK,
	CMD_TBL_UNLOCK,
	CMD_TBL_BOOT,
	CMD_TBL_MEMCPY,
	CMD_TBL_MEMDUMP,
	CMD_TBL_HEXDUMP,
	CMD_TBL_MEMCMP,
	CMD_TBL_MEMSET,
	CMD_TBL_WRITE,
	CMD_TBL_READ,
	CMD_TBL_STATUS,
	CMD_TBL_REBOOT,
	CMD_TBL_TEST,
	CMD_TBL_END
};


void DisplayPrompt(char *prompt)
{
	if(prompt == NULL) {
		printf(PACKAGE "> ");
	} else {
		printf(prompt);
	}
}


int GetCommand(char *cmd, int len, int timeout)
{
	char			c;
	int				i, rdCnt, rdMax = len-1;
	volatile long	endTime=GetTime()+timeout*HZ;
	
	for (rdCnt=0, i=0; rdCnt < rdMax;){
		// try to get a byte from the serial port.
		while (!SerialInputByte(&c)){
			if (GetTime() > endTime){
				cmd[i++] = '\0';
				return rdCnt;
			}
		}

		if ((c=='\r') || (c=='\n')){
			cmd[i++] = '\0';
			// print newline.
			printf("\n");
			return rdCnt;
		} else if (c == '\b'){
			if(i > 0){
				i--;
				rdCnt--;
				// cursor one position back.
				printf("\b \b");
			}
		} else {
			cmd[i++] = c;
			rdCnt++;
			// print character.
			printf("%c", c);
		}
	}
	return(rdCnt);
}	// GetCommand.


int GetArgs(char *s, char **argv)
{
	int args = 0;

	if (!s || *s=='\0') return 0;
	while (args < MAX_ARGS){
		// skip space and tab.
		while ((*s==' ') || (*s=='\t')) s++;

		// check end of line.
		if (*s=='\0'){
			argv[args] = 0;
			return args;
		}

		// start get arg.
		argv[args++] = s;


		// remove ' ' and '\t'.
		while (*s && (*s!=' ') && (*s!='\t')) s++;
		// end of line.
		if (*s=='\0'){
			argv[args] = 0;
			return args;
		}
		*s++ = '\0';
	}
	return args;
}	// GetArgs.


bool DoBootKernel(CMD_TBL *cptr, int argc, char **argv)
{
	long opt[2];
	void (*theKernel)(int zero, int arch);

	if (argc!=1 && argc!=3 && argc!=4)
	{
		printf(cptr->usage);
		return false;
	}

	switch (argc)
	{
		// boot
		case 1 :
			opt[0] = 0;
			opt[1] = 200;
			theKernel = (void (*)(int, int))KERNEL_DRAM_BASE;
			break;
		default :
			printf(cptr->usage);
			return false;
	}

	// we assume that the kernel is in place.
	// See linux/include/asm-arm/system.h for architecture numbers.
	printf("\nStarting kernel ...\n\n");
	theKernel(opt[0], opt[1]);

	printf("Hey, the kernel returned! This should not happen.\n");
	return true;
}



bool DoResetTerminal(CMD_TBL *cptr, int argc, char **argv)
{
	int i;

	printf("ClearTerminal");
	for(i=0; i<200; i++) printf("\n");

	printf("ResetTerminal");
	return true;
}

bool DoReboot(CMD_TBL *cptr, int argc, char **argv)
{

	void (*restart)(void) = 0;

	printf("Restarting...\r\n");
	restart();

	return true;
}

bool DoReload(CMD_TBL *cptr, int argc, char **argv)
{
	ulong *src=0, *dest=0;
	int len;

	if (!StrCmp(argv[1], "kernel"))
	{
		src  = (ulong *)KERNEL_SRAM_BASE;
		dest = (ulong *)KERNEL_DRAM_BASE;
		len  = KERNEL_MAX_SIZE;
		printf("Loading kernel from flash...\n");
	}
	else {
		printf(cptr->usage);
		return false;
	}

	MemCpy32(dest, src, len/4);
	printf("Done.\n");
	return true;
}	// DoReload.


void ClearLine(void)
{
	printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
		   "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	return;
}


bool DoPrintStatus(CMD_TBL *cptr, int argc, char **argv)
{
	printf("Bootloader         : " PACKAGE "\n");
	printf("Version            : " VERSION "\n");

	return true;
}


bool DoTest(CMD_TBL *cptr, int argc, char **argv)
{
	return true;
}
