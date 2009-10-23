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
//      lijilong
//
// Created:
//      2005.9
//
///////////////////////////////////////////////////////////////////////////////

#include "main.h"
#include "scc.h"

#define FND     0x14000000 

#define FND_CS0 (*((volatile unsigned short *)(FND))) 
#define FND_CS1 (*((volatile unsigned short *)(FND + 0x00100000))) 
#define FND_CS2 (*((volatile unsigned short *)(FND + 0x00200000))) 
#define FND_CS3 (*((volatile unsigned short *)(FND + 0x00300000)))

extern void sendtest();

LOADER_STATUS	status;

// function prototype.
bool	DoPrintfHelp		(int argc, char **argv);


void c_main()
{
	int		i;
	bool	autoboot=true;
	char	cmd[128];
	long	timeout;
	int		argc=0;
	char	*argv[MAX_ARGS];
	CMD_TBL	*cptr;
	 
	// initiate status.
	status.terminalSpeed = SERIAL_SPEED;
	status.downloadSpeed = SERIAL_DOWNLOAD_SPEED;

	/* initiate serial and timer */
	// serial and timer init.
	SerialInit(status.terminalSpeed);
         printf("\n\nFFUART has been initiated");

	TimerInit();

	//// printf the required GPL string. //////////////////////////////////////
	printf("\n\n");
	printf(" "PACKAGE "-" VERSION "\n Copyright 2005 Embedded Group at 211.1010@UESTC\n");
	printf(" Support:211.1010@UESTC\n");
	printf("\n");

	//MemCpy((char *)KERNEL_DRAM_BASE, (char *)KERNEL_SRAM_BASE, KERNEL_MAX_SIZE);	// kernel image reload

	while (1) ;
	EthInit();
        //sendtest();

	///// wait 10 seconds before starting autoboot. ///////////////////////////
	printf("Autoboot in progress, press any key to stop ");

	for (i=0; i<3; i++){
		timeout = GetTime()+1;	// 1ÃÊ°£ Delay

		printf(".");
		while (GetTime()<timeout){
			if ((FFLSR & 0x00000001)){	// Serial
				FFRBR;
				autoboot = false;
				break;
			}
		}
		if (autoboot==false) break;
	}


	/* No key was pressed, so proceed booting the kernel. */
	if (autoboot)
	{
		printf("Autoboot started.\n");
		for (cptr=cmdTbl; cptr->cmd; cptr++){
			if (!StrCmp(cptr->cmd, "boot")) break;
		}
		DoBootKernel(cptr, 1, 0);
	}


	/* Key was pressed, so proceed command mode. */
	printf("\nAutoboot aborted\n");
	printf("Type \"help\" to get a list of commands\n");

	// the command loop. endless, of course.
	for(;;) {
		DisplayPrompt("bootloader>");

		// wait an hour to get a command.
		GetCommand(cmd, 128, 3600);
		if (!cmd || !cmd[0]) continue;
		
		argc = GetArgs(cmd, argv);
		for (cptr=cmdTbl; cptr->cmd; cptr++){
			if (!StrCmp(argv[0], cptr->cmd)){
				(cptr->run)(cptr, argc, argv);
				break;
			}
		}
		if (!StrCmp(argv[0], "help") || !StrCmp(argv[0], "?")){
			DoPrintfHelp(argc, argv);
		} else if (!(cptr->cmd)){
			printf("\tUnknown command : %s\n", argv[0]);
		}
	}
} // CMain.


bool DoPrintfHelp(int argc, char **argv){
	CMD_TBL	*cptr;

	if (argc==1){
		printf("Help for " PACKAGE "-" VERSION "\n");
		printf("The following commands are supported :\n");
		printf("  help                             Help for commands.\n");

		for (cptr=cmdTbl; cptr->cmd; cptr++){
				if (cptr->helpMore) printf(cptr->helpMore);
		}
		printf("\n");
	}

	else {
		printf("\tUnknown command : %s\n", argv[0]);
	}
	return true;
}
