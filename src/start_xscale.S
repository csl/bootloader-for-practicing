/////////////////////////////////////////////////////////////////////////////////
// Copyright(c) 2001-2002 Hybus Co,.ltd. All rights reserved.
//
// Module name:
//      start_xscale.S
//
// Description:
//
//
// Author:
//      lijilong
//
// Created:
//      2002.10
//
///////////////////////////////////////////////////////////////////////////////

#include <start_xscale.h>
#include <config.h>

.text
.extern _ld_text_start			//int ld-xcale script file
.extern _ld_text_and_data_size


///////////////////////////////////////////////////////////////////////////////
///////////		Vector Table				///////////////
///////////////////////////////////////////////////////////////////////////////
.globl _start
_start:
        b       reset


reset:
        // Mask All interrupt 
        //#define INTERRUPT_CONTROL_BASE 0x40D00000
        ldr     r12, =INTERRUPT_CONTROL_BASE
        ldr     r0, =0x00000000
        str     r0, [r12, #ICMR]	//Interrupt Controller Mask Register
        str     r0, [r12, #ICLR]	//Interrupt Controller Level Register (determined IRQ or FIQ)

	// Initialize GPIO
        bl      gpio_init

	// Initialize SDRAM
        bl      init_sdram

	// Copy Bootloader from Flash to SDRAM
	bl	copy_to_ram
	
	// Loading kernel image
        ldr	r4, =KERNEL_SRAM_BASE
        ldr	r5, =KERNEL_DRAM_BASE
        ldr	r6, =KERNEL_MAX_SIZE
        add	r6, r6, r4
repeat:
        ldmia   r4!, {r0-r3, r7-r10}
        stmia   r5!, {r0-r3, r7-r10}
        cmp             r4, r6
        blt             repeat

        ldr	sp, =_ld_stack_address

        // Jump to c_main
        ldr	r0, =c_main
        mov	pc, r0

die:
	b	die


///////////////////////////////////////////////////////////////////////////////
///////////		Initialize GPIO				///////////////
///////////////////////////////////////////////////////////////////////////////

gpio_init:
    //#define GPIO_BASE         0x40E00000
    ldr     r12, =GPIO_BASE
    
    //Develper's Manual P.120   
    //Choice GPIO alternate function: PIN: in or out
    ldr     r0, =GAFR0L_VALUE		//#define GAFR0L_VALUE      0x8000 0000 (8x4bit)
    str     r0, [r12, #GAFR0_L]         //GPIO alternate function select register GPIO[15:0]
    ldr     r0, =GAFR0U_VALUE		//#define GAFR0U_VALUE      0xA500 0010
    str     r0, [r12, #GAFR0_U]		//GPIO alternate function select register GPIO[31:16]

    ldr     r0, =GAFR1L_VALUE		//#define GAFR1L_VALUE      0x6000 8018
    str     r0, [r12, #GAFR1_L]		//GPIO alternate function select register GPIO[47:32]
    ldr     r0, =GAFR1U_VALUE		//#define GAFR1U_VALUE      0xAAA5 AAAA
    str     r0, [r12, #GAFR1_U]		//GPIO alternate function select register GPIO[63:48]	

    ldr     r0, =GAFR2L_VALUE		//#define GAFR2L_VALUE      0xAAA0 000A
    str     r0, [r12, #GAFR2_L]		//GPIO alternate function select register GPIO[79:64]
    ldr     r0, =GAFR2U_VALUE		//#define GAFR2U_VALUE      0x0000 0002
    str     r0, [r12, #GAFR2_U]		//GPIO alternate function select register GPIO[80]
    
    //Develper's Manual P.113    
    ldr     r0, =GPSR0_VALUE		//#define GPSR0_VALUE       0x00408030//0x00588020//00588030//0x00588020
    str     r0, [r12, #GPSR0]		//GPIO Pin-Level Register GPIO<31:0>
    ldr     r0, =GPSR1_VALUE		//#define GPSR1_VALUE       0x00BFA882
    str     r0, [r12, #GPSR1]		//GPIO Pin-Level Register GPIO<63:32>
    ldr     r0, =GPSR2_VALUE		//#define GPSR2_VALUE       0x0001C000
    str     r0, [r12, #GPSR2]		//GPIO Pin-Level Register GPIO<80:64>

    
    //Develper's Manual P.115
    ldr     r0, =GPCR0_VALUE		//#define GPCR0_VALUE       0xC0031100//0xC0033110//0xC00311D0
    str     r0, [r12, #GPCR0]		//GPIO Pin Output Clear Register GPIO<31:0>
    ldr     r0, =GPCR1_VALUE		//#define GPCR1_VALUE       0xFC400300
    str     r0, [r12, #GPCR1]		//GPIO Pin Output Clear Register GPIO <63:32>
    ldr     r0, =GPCR2_VALUE		//#define GPCR2_VALUE       0x00003FFF
    str     r0, [r12, #GPCR2]		//GPIO Pin Output Clear Register GPIO <80:64>

    /*
    Develper's Manual P.112
    0 – Pin configured as an input.
    1 – Pin configured as an output.
    */

    ldr     r0, =GPDR0_VALUE		//#define GPDR0_VALUE       0xC0439330     //0xC05BE830//0xC05BB130//0xC05B91F0
    str     r0, [r12, #GPDR0]		//GPIO Pin Direction Register GPIO<31:0>
    ldr     r0, =GPDR1_VALUE		//#define GPDR1_VALUE       0xFCFFAB82
    str     r0, [r12, #GPDR1]		//GPIO Pin Direction Register GPIO<63:32>
    ldr     r0, =GPDR2_VALUE		//#define GPDR2_VALUE       0x0001FFFF
    str     r0, [r12, #GPDR2]		//GPIO Pin Direction Register GPIO<80:64>

    // Clear the peripheral control registers bits
    //Develper's Manual P.91
    //#define PSSR              0x40F00004
    //#define PSSR_PH           0x00000010
    //#define PSSR_RDH          0x00000020
    ldr     r1, =PSSR			//Power Manager Sleep Status Register (PSSR)
    ldr     r2, =(PSSR_RDH | PSSR_PH)	//Read Disable Hold && Peripheral Control Hold.
    str     r2, [r1]

    //lr(r14）
    mov     pc, lr

///////////////////////////////////////////////////////////////////////////////
///////////	 	Initialize SDRAM			///////////////
///////////////////////////////////////////////////////////////////////////////


init_sdram:
        mov	r10, lr
        //#define CLOCK_MANAGER_BASE 0x41300000
        ldr     r12, =CLOCK_MANAGER_BASE

	//Develper's Manual P.98
        ldr     r0, =CKEN_VALUE		//#define CKEN_VALUE        0x00000040
        str     r0, [r12, #CKEN]	//Clock Enable Register

	//Develper's Manual P.100
        ldr     r0, =OSCC_VALUE		//#define OSCC_VALUE        0x00000002
        str     r0, [r12, #OSCC]	//Oscillator Configuration Register

#if 1
//TODO: Issuing a FCS on a B1 seems to make it crash
//shortly after. Figure out why...

init_clock_reg:		/* PXA250 Clock Register initialization */
	//Develper's Manual P.96
        ldr     r0, =CCCR_VALUE		//#define CCCR_VALUE        0x00000161	// |0010|0100|0001 0x241
        str     r0, [r12, #CCCR]	//Core Clock Configuration Register

set_core_clock:		/* PXA250 Core Clock is set to Turbo Mode */
        mov     r1, #0
        mcr     p14, 0, r1, c6, c0, 0   //p14, c6, c0
        mov     r1, #3
        mcr     p14, 0, r1, c6, c0, 0   //p14, c6, c0

	//Develper's Manual P.141
	//#define OSCR              0x40A00010
	ldr	r1, =OSCR		//OS Timer Count Register			
	ldr	r0, =0
	str	r0, [r1]

set_os_timer:		/* OS Timer initialization */
	//Waiting OSCR
        ldr     r0, =0x300
wait_for_clock:
        ldr	r2, [r1]
        cmp     r0, r2
        bne     wait_for_clock
#endif

//Step 1 in Intel's code

	//Memory Control Registers
	//#define MEM_CTL_BASE      0x48000000
        ldr     r12, =MEM_CTL_BASE

	//Develper's Manual P.229
	/* Static Memory Control Register 0(MSC0): Nonburst ROM or Flash Memory  */
        ldr     r0, =MSC0_VALUE		//#define MSC0_VALUE        0x7ff87ff8	//0x7ff87ff8//0xFFF03FF0//0x23F223F2//0x3FF83FF0
        str     r0, [r12, #MSC0]

        //Intel's code reads it back to make sure it works...
        //    ldr     r0, [r12, #MSC0]

	/* Static Memory Control Register 1(MSC1): Variable Latency I/O (VLIO)  */
        ldr     r0, =MSC1_VALUE		//#define MSC1_VALUE        0x12BC5554      //0x12BC8240//0x5AA85AA8
        str     r0, [r12, #MSC1]	
        ldr     r0, [r12, #MSC1]
	/* Static Memory Control Register 2(MSC2): SRAM */
        ldr     r0, =MSC2_VALUE 	//#define MSC2_VALUE        0x7FF87FF1	//0x7FF17FF1//0x24482448
        str     r0, [r12, #MSC2]	
        ldr     r0, [r12, #MSC2]

	/* MECR: Expansion Memory (PCMCIA/Compact Flash) Bus Configuration Register */
	//Develper's Manual P.245
        ldr     r0, =MECR_VALUE		//#define MECR_VALUE        0x00000000	//0x00000002
        str     r0, [r12, #MECR]
	
	/* MCMEM0: Card Interface slot 0 timing */
	//Develper's Manual P.242
        ldr     r0, =MCMEM0_VALUE 	//#define MCMEM0_VALUE      0x00010504	//0x00020308, 0x00004204
        str     r0, [r12, #MCMEM0]	//Card interface Common Memory Space Socket 0 Timing Configuration

	/* MCMEM1: Card Interface slot 1 timing */
        ldr     r0, =MCMEM1_VALUE 	//#define MCMEM1_VALUE      0x00010504	//0x00020308
        str     r0, [r12, #MCMEM1]	//Card interface Common Memory Space Socket 1 Timing Configuration
	
	/* MCATT0: Card Interface Attribute Space Timing, slot 0 */
	//Develper's Manual P.243
        ldr     r0, =MCATT0_VALUE 	//#define MCATT0_VALUE      0x00010504	//0x00020308
        str     r0, [r12, #MCATT0]    	//Card interface Attribute Space Socket 0 Timing Configuration

	/* MCATT1: Card Interface Attribute Space Timing, slot 1 */
        ldr     r0, =MCATT1_VALUE 	//#define MCATT1_VALUE      0x00010504	//0x00020308
        str     r0, [r12, #MCATT1]	//Card interface Attribute Space Socket 1 Timing Configuration
	
	/* MCIO0: Card Interface I/O Space Timing, slot 0 */	
	//Develper's Manual P.243
        ldr     r0, =MCIO0_VALUE	//#define MCIO0_VALUE       0x00004715	//0x00020308
        str     r0, [r12, #MCIO0]	//Card interface I/O Space Socket 0 Timing Configuration

	/* MCIO1: Card Interface I/O Space Timing, slot 1 */
        ldr     r0, =MCIO1_VALUE 	//#define MCIO1_VALUE       0x00004715	//0x00020308
        str     r0, [r12, #MCIO1]	//Card interface I/O Space Socket 1 Timing Configuration

        //according to Intel's comments, we're extracting the DRI

	// Loading the MDREFR in a way that will make it configure
	// correctly is a multi-step process.  Please read Section
	// 6.12 of the PXA Developers Manual For more Details

	// 1.
	// The first step requires that we set K0RUN and E0PIN while
	// configuring DRI and clearing KXFREE.  All other values
	// MUST be left alone

	/* SDRAM Controller initialized at 100MHz */
	//Develper's Manual P.196
        ldr     r0, =MDREFR_VALUE	//#define MDREFR_VALUE      0x000BC108 <0x03c00fff>		//0x03CDF016//0x000BC018//0x00038016
        ldr     r3, [r12, #MDREFR]	//SDRAM Refresh Control Registe

        ldr     r1, =0xFFF
        and     r0, r0, r1

	// Make the DRI we read from MDREFR what MDREFR_VALUE says it is.
	// We also Free KXFREE the free running bits.
        bic     r3, r3, r1
	bic	r3, r3, #0x03800000
        orr     r3, r3, r0

        //Write it back
        str     r3, [r12, #MDREFR]	//SDRAM Refresh Control Register

	// 2.
	// We don't have Synchronous Static Memory and don't want to
	// mess with SXCNFG or the like so we are leaving out this
	// step.

	// 3.
	// We don't bother to do this step as it does not seem to have
	// been done previously (actually maybe Self-Refresh Disable should
	// be here.)

        // 4.
	// Here we will setup the SDCLK's but WILL NOT enable them.  We
	// need to reload MDREFR for this.

	//Develper's Manual P.196
        ldr     r0, =MDREFR_VALUE	//#define MDREFR_VALUE      0x000BC108//0x03CDF016//0x000BC018//0x00038016
	ldr	r1, =0xF6000		//Mask of SDCLK's settings minus EXPIN
	and	r0, r0, r1
        bic     r3, r3, r1
	orr	r3, r3, r0

        str     r3, [r12, #MDREFR]	//SDRAM Refresh Control Register
	ldr	r3, [r12, #MDREFR]	//SDRAM Refresh Control Register

	// 4.
        // Although I think that this should be at #3 It is here cause this
	// is where it was originally.  This will turn off Self-Refresh.

	//Develper's Manual P.196
        bic     r3, r3, #0x00400000
        str     r3, [r12, #MDREFR]	//SDRAM Refresh Control Register

	// 5.
	// Finally, we Enable the Various SDCLK's and let it run.
	// Also, enable the free-running clocks (not mentioned in the manual).

	//Develper's Manual P.196
	ldr	r0, =MDREFR_VALUE
	ldr	r1, =0x03809000
	and	r0, r0, r1
	orr	r3, r3, r0
        str     r3, [r12, #MDREFR]	//SDRAM Refresh Control Register
        nop
        nop

//Step 4 in Intel's code

	//Develper's Manual P.190
        ldr     r0, =MDCNFG_VALUE	//#define MDCNFG_VALUE      0x00001AC9//0x0B000BC9

        //disable all sdram banks
        bic     r0, r0, #0x00000003
        bic     r0, r0, #0x00030000

        //program banks 0/1 for 32 bit bus width
        bic     r0, r0, #0x00000004

	//test with 16 bit bus width
//	orr	r0, r0, #0x00000004

        //write MDCNFG, without enabling SDRAM banks
        str     r0, [r12, #MDCNFG]	

//Step 5 in Intel's code

        ldr     r0, =OSCR
	mov	r1, #0
	str	r1, [r0]

        /* SDRAM Controller initialized at 50MHz. */
        /* pause for ~200usec for SDRAM to init */

        //pause for approx 200 usecs
        ldr     r4, =0x300

sdram_dly:

        ldr     r1, [r0]
        cmp     r4, r1
        bgt     sdram_dly

//Step 6 in Intel's code

        //turn everything off
        mov     r0, #0x78
        mcr     p15, 0, r0, c1, c0, 0


//Step 7 in Intel's code

        //Access memory that has not been enabled for CBR refresh cycles (8)
	ldr	r0, =SDRAM_BASE			//SDRAM Bank 0
        str     r0, [r0]
        str     r0, [r0]
        str     r0, [r0]
        str     r0, [r0]
        str     r0, [r0]
        str     r0, [r0]
        str     r0, [r0]
        str     r0, [r0]

        //Step 8 is blank in Intel's code, though they mention dcache should
        //be enabled here if it is desired (we don't care)


        //Step 9
	//Develper's Manual P.190
        ldr     r0, [r12, #MDCNFG]		//SDRAM Configuration Register 0

        //enable bank 0 (what about bank 1?)
	//Develper's Manual P.190
        orr     r0, r0, #0x00000001
        str     r0, [r12, #MDCNFG]		//SDRAM Configuration Register 0

        //Step 10
	//Develper's Manual P.194
        //write MDMRS again
	///* and do an MRS */
        ldr     r0, =MDMRS_VALUE		//#define MDMRS_VALUE       0x00000000	
        str     r0, [r12, #MDMRS]		//MRS value to be written to SDRAM

        //Step 11
        //are we A1_Cotulla?
	//Develper's Manual P.196
        ldr     r0, [r12, #MDREFR]		//SDRAM Refresh Control Register

	ldr	r11, =0xFFEFFFFF
	and	r0, r0, r11

        str     r0, [r12, #MDREFR]

        mov     pc, r10

///////////////////////////////////////////////////////////////////////////////
///////////	 Copy Bootloader from Flash to SDRAM		///////////////
///////////////////////////////////////////////////////////////////////////////

copy_to_ram:
	mov	r8, lr


	ldr	r0, =0
	ldr	r1, =_ld_text_start
	ldr	r2, =_ld_text_and_data_size

copy_loop:
	ldr	r3, [r0]
	str	r3, [r1]
	add	r0, r0, #4
	add	r1, r1, #4
	subs	r2, r2, #4
	bne	copy_loop

	mov	pc, r8

