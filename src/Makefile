#////////////////////////////////////////////////////////////////////////////////
# Copyright(c) 2001-2002 Hybus Co,.ltd. All rights reserved.
#
# Module name:
#      Makefile
#
# Description:
#
#
# Author:
#      bedguy
#
# Created:
#      2002.10
#
#//////////////////////////////////////////////////////////////////////////////

CC = /usr/local/hybus-arm-linux-R1.1/bin/arm-linux-gcc
OBJCOPY = /usr/local/hybus-arm-linux-R1.1/bin/arm-linux-objcopy
INCLUDE = -I./include
CFLAGS = -O2 -nostdinc -mcpu=strongarm110 -mapcs-32 -fomit-frame-pointer -fPIC $(INCLUDE)  -Wall $(CFLAGSII)
OCFLAGS = -O binary -R .note -R .comment -S
LDFLAGS = -static -nostdlib
ELF32_LDFLAGS = -Wl,-T,ld-xscale
COMPILE = $(CC) $(CFLAGS)



CFILES = $(wildcard *.c)
SFILES = $(wildcard *.S)

OBJ_FILES = $(SFILES:%.S=%.o) $(CFILES:%.c=%.o)

#OBJ_FILES = start.o main.o command.o serial.o time.o flash.o util.o uucodec.o scc.o net.o bootp.o tftp.o

BIN_FILE = x-boot255




all : $(OBJ_FILES)
	$(CC) $(LDFLAGS) -o $(BIN_FILE)-elf32 $(ELF32_LDFLAGS) $(OBJ_FILES) -lgcc
	$(OBJCOPY) $(OCFLAGS) $(BIN_FILE)-elf32 $(BIN_FILE)

dep :
	arm-linux-gcc -M $(CFILES) $(INCLUDE)

.s.o :
	$(COMPILE) -c $<

.S.o :
	$(COMPILE) -c $<

.c.o :
	$(COMPILE) -c $<



clean :
	rm -rf *.o
	rm -rf $(BIN_FILE) $(BIN_FILE)-elf32
