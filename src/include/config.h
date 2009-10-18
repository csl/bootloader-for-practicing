/////////////////////////////////////////////////////////////////////////////////
// Copyright(c) 2001-2002 Hybus Co,.ltd. All rights reserved.
//  
// Module name:
//      config.h 
//  
// Description:
//  
// Updata: 
//      2002.10		Created		 bedguy
///////////////////////////////////////////////////////////////////////////////

#ifndef _CONFIG_H_4718034674012374874802
#define _CONFIG_H_4718034674012374874802

/*------------------Must select XHYPER255A or XHYPER255B---------------------*/

//#define XHYPER255A
#define XHYPER255B

/*---------------------------------------------------------------------------*/

///// Serial //////////////////////////////////////////////////////////////////

#define SERIAL_SPEED			0x08
#define SERIAL_DOWNLOAD_SPEED		0x08

#define USE_SERIAL_UUCODEC

#ifdef XHYPER255A
#define FLASHBIT                        16
#define FLASHNUM                        1
#define FLASHSIZE                       (0x01000000)    // 16 MB.
#else
#define FLASHBIT			16
#define FLASHNUM			2
#define FLASHSIZE			(0x02000000)	// 32 MB.
#endif

#define USE_CS0_FLASH					

#define SRAM_BASE_ADDR			(0x00000000)
#define SRAM_SIZE			FLASHSIZE		

#ifdef XHYPER255A
#define SRAM_BLOCK_SIZE                 (0x00020000)
#else
#define SRAM_BLOCK_SIZE			(0x00040000)
#endif

///// Dynamic Memory //////////////////////////////////////////////////////////
#define DRAM_BASE_ADDR			(0xA0000000)
#define DRAM_SIZE			(0x02000000)


///// Package Information /////////////////////////////////////////////////////
#ifdef XHYPER255A
#define PACKAGE				"MT608"	// Name of package.
#define VERSION				"R2"		// Version number of package.
#else
#define PACKAGE				"MT608"	// Name of package.
#define VERSION				"R2"		// Version number of package.
#endif

// Position of BOOT LOADER.
#define LOADER_DRAM_MAX_SIZE		(0x00010000)
#define LOADER_SRAM_MAX_SIZE		(0x00040000)
#define LOADER_DRAM_BASE		(DRAM_BASE_ADDR+DRAM_SIZE-LOADER_DRAM_MAX_SIZE)
#define STACK_POINT			(DRAM_BASE_ADDR+DRAM_SIZE-LOADER_DRAM_MAX_SIZE-4)

// Position of kernel.
#define KERNEL_SRAM_BASE		(0x000C0000)
#define KERNEL_DRAM_BASE		(0xA1000000)

#define KERNEL_MAX_SIZE			(0x00100000)
#define NUM_KERNEL_BLOCKS		(KERNEL_MAX_SIZE / SRAM_BLOCK_SIZE)

// Position of root.
#define ROOT_SRAM_BASE			(0x001C0000)
#define ROOT_DRAM_BASE			(0xA0000000)
#ifdef XHYPER255A
#define ROOT_MAX_SIZE			(0x00E00000)
#else
#define ROOT_MAX_SIZE			(0x01E00000)
#endif
#define NUM_RAMDISK_BLOCKS		(RAMDISK_MAX_LEN / SRAM_BLOCK_SIZE)

// Position of loader.
#define LOADER_SRAM_BASE		(0x00000000)
#define LOADER_MAX_SIZE			(0x00040000)
#define NUM_LOADER_BLOCKS		(LOADER_MAX_LEN / SRAM_BLOCK_SIZE)

///// Newwork /////////////////////////////////////////////////////////////////
#define CLIENT_ETHER			{0x12, 0x34, 0x56, 0x78, 0x9A, 0x00}
#define MAX_PKT_SIZE			608

#define DELAY_BEFORE_BOOT		5

#endif		// end _CONFIG_H_4718034674012374874802.
