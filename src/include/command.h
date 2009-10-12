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

#ifndef _COMMAND_H_748126374816423412
#define _COMMAND_H_748126374816423412

#include "main.h"

#define MAX_COMMANDS			20
#define MAX_ARGS				20


struct _CMD_TBL {
	char 	*cmd;		// 이 함수를 호출할 명령어.
	bool	(*run)(struct _CMD_TBL *cptr, int argc, char **argv);	// function point.
	char	*usage;		// command가 잘 못되었을 때 나올 message.
	char	*help;		// help에서 보여줄 message.
	char	*helpMore;
};
typedef struct _CMD_TBL			CMD_TBL;

extern CMD_TBL cmdTbl[];

// Prototypes.
void	DisplayPrompt(char *prompt);
int		GetCommand(char *command, int len, int timeout);
int		GetArgs(char *s, char **args);
bool	DoBootKernel(CMD_TBL *cptr, int argc, char **argv);
bool	DoReload(CMD_TBL *cptr, int argc, char **argv);
void	ClearLine(void);

#endif		// end _COMMAND_H_748126374816423412.
