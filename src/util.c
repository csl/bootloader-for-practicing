/////////////////////////////////////////////////////////////////////////////////
// Copyright(c) 2001-2002 Hybus Co,.ltd. All rights reserved.
//
// Module name:
//      util.c
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


#include "util.h"

bool DoMemCpy(CMD_TBL *cptr, int argc, char **argv){
	ulong	dest, src, len;

	if (argc < 4){
		printf(cptr->usage);
		return false;
	}
	if (!HexToInt(argv[1], &dest, 32) || !HexToInt(argv[2], &src, 32) ||
			!HexToInt(argv[3], &len, 32)){
		printf("Illegal character is used.\n");
		return false;
	}
	
	MemCpy((char *)dest, (char *)src, len);
	return true;
}	// DoMemCpy.


bool DoMemDump(CMD_TBL *cptr, int argc, char **argv){
	ulong	i, addr, len, endptr;
	char	value;

	if (argc < 3){
		printf(cptr->usage);
		return false;
	}
	if (!HexToInt(argv[1], &addr, 32) || !HexToInt(argv[2], &len, 32)){
		printf("Illegal character is used.\n");
		return false;
	}
	endptr = addr + len;
	
	for (i=0; addr < endptr; i++, ((char *)addr)++){
		value = ADDR8(addr);

		if ((i)%4==0) printf("0x");
		printf("%02x", value);
		if ((i+1)%4==0){
			printf("  ");
		}
		if ((i+1)%16==0) printf("\n");
	}
	return true;
}	// DoMemDump.


bool DoHexDump(CMD_TBL *cptr, int argc, char **argv){
	long addr, len;
	
	if (argc < 3){
		printf(cptr->usage);
		return false;
	}
	if (!HexToInt(argv[1], &addr, 32) || !HexToInt(argv[2], &len, 32)){
		printf("Illugal character is used.\n");
		return false;
	}
	HexDump((char *)addr, (int)len);
	return true;
}	// DoHexDump.


bool DoMemCmp(CMD_TBL *cptr, int argc, char **argv){
	ulong addr1, addr2;
	int len;
	if (argc < 4){
		printf(cptr->usage);
		return false;
	}
	if (!HexToInt(argv[1], &addr1, 32) || !HexToInt(argv[2], &addr2, 32) ||
			!HexToInt(argv[3], &len, 32)){
		printf("Illugal character is used.\n");
		return false;
	}

	if (MemCmp((char *)addr1, (char *)addr2, len)==0)
			printf("equil.\n");
	else printf("not equil.\n");
	return true;
}	// DoMemCmp.


bool DoMemSet(CMD_TBL *cptr, int argc, char **argv){
	char	*addr;
	char	c;
	int		len;

	if (argc < 4){
		printf(cptr->usage);
		return false;
	}
	if (!HexToInt(argv[1], &addr, 32) ||
			!HexToInt(argv[2], &c, 8) || !HexToInt(argv[3], &len, 32)){
		printf("Illugal character is used.\n");
		return false;
	}
	MemSet(addr, c, len);

	return true;
}	// DoMemSet.


bool DoReadFromReg(CMD_TBL *cptr, int argc, char **argv){
	void	*addr=0;
	char	c;
	short	s;
	long	l;
	
	if (argc < 3){
		printf(cptr->usage);
		return false;
	}
	if (!HexToInt(argv[2], &addr, 32)){
		printf("Illugal character is used.\n");
		return false;
	}

	printf("\taddress : 0x%08lx\n", (long)addr);
	printf("\tvalue   : 0x");
	
	switch (argv[1][0]){
		case 'c' :
			c = ADDR8(addr);
			printf("%02x", c);
			break;
		case 's' :
			s = ADDR16(addr);
			printf("%04x", s);
			break;
		case 'l' :
			l = ADDR32(addr);
			printf("%08lx", l);
			break;
		default :
			printf("Error.\n");
			return -1;
			break;
	}
	printf(".\n");
	return true;
}	// DoReadFromReg.


bool DoWriteToReg(CMD_TBL *cptr, int argc, char **argv){
	char *addr=0;
	char c;
	short s;
	long l;
	
	if (argc < 4){
		printf(cptr->usage);
		return false;
	}
	if (!HexToInt(argv[2], &addr, 32)){
		printf("Illugal character is used.\n");
		return false;
	}

	switch (argv[1][0]){
		case 'c' :
			if (!HexToInt(argv[3], &c, 8)){
				printf("Illugal character is used.\n");
				return false;
			}
			ADDR8(addr) = c;
			break;
		case 's' :
			if (!HexToInt(argv[3], &s, 16)){
				printf("Illugal character is used.\n");
				return false;
			}
			ADDR16(addr) = s;
			break;
		case 'l' :
			if (!HexToInt(argv[3], &l, 32)){
				printf("Illugal character is useqd.\n");
				return false;
			}
			ADDR32(addr) = l;
			break;
		default :
			printf("write [c/s/l] [addr] [value].\n");
			return false;
			break;
	}
	return true;
}	// DoWriteToReg.
