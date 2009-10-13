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
.extern _ld_text_start
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
        str     r0, [r12, #ICMR]  //Interrupt Controller Mask Register
        str     r0, [r12, #ICLR]  //Interrupt Controller Level Register

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

    ldr     r0, =GAFR0L_VALUE
    str     r0, [r12, #GAFR0_L]
    ldr     r0, =GAFR0U_VALUE
    str     r0, [r12, #GAFR0_U]

    ldr     r0, =GAFR1L_VALUE
    str     r0, [r12, #GAFR1_L]
    ldr     r0, =GAFR1U_VALUE
    str     r0, [r12, #GAFR1_U]

    ldr     r0, =GAFR2L_VALUE
    str     r0, [r12, #GAFR2_L]
    ldr     r0, =GAFR2U_VALUE
    str     r0, [r12, #GAFR2_U]

    ldr     r0, =GPSR0_VALUE
    str     r0, [r12, #GPSR0]
    ldr     r0, =GPSR1_VALUE
    str     r0, [r12, #GPSR1]
    ldr     r0, =GPSR2_VALUE
    str     r0, [r12, #GPSR2]

    ldr     r0, =GPCR0_VALUE
    str     r0, [r12, #GPCR0]
    ldr     r0, =GPCR1_VALUE
    str     r0, [r12, #GPCR1]
    ldr     r0, =GPCR2_VALUE
    str     r0, [r12, #GPCR2]

    ldr     r0, =GPDR0_VALUE
    str     r0, [r12, #GPDR0]
    ldr     r0, =GPDR1_VALUE
    str     r0, [r12, #GPDR1]
    ldr     r0, =GPDR2_VALUE
    str     r0, [r12, #GPDR2]

    // Clear the peripheral control register bits
    ldr     r1, =PSSR
    ldr     r2, =(PSSR_RDH | PSSR_PH)
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

        ldr     r0, =CKEN_VALUE		//#define CKEN_VALUE        0x00000040
        str     r0, [r12, #CKEN]		//Clock Enable Register
        ldr     r0, =OSCC_VALUE		//#define OSCC_VALUE        0x00000002
        str     r0, [r12, #OSCC]		//Oscillator Configuration Register

#if 1
//TODO: Issuing a FCS on a B1 seems to make it crash
//shortly after. Figure out why...
        ldr     r0, =CCCR_VALUE
        str     r0, [r12, #CCCR]
        mov     r1, #3
        mcr     p14, 0, r1, c6, c0, 0

	ldr	r1, =OSCR
	ldr	r0, =0
	str	r0, [r1]

        ldr     r0, =0x300
wait_for_clock:
        ldr	r2, [r1]
        cmp     r0, r2
        bne     wait_for_clock
#endif

        //Step 1 in Intel's code
        ldr     r12, =MEM_CTL_BASE

        ldr     r0, =MSC0_VALUE
        str     r0, [r12, #MSC0]
        //Intel's code reads it back to make sure it works...
    //    ldr     r0, [r12, #MSC0]

        ldr     r0, =MSC1_VALUE
        str     r0, [r12, #MSC1]
        ldr     r0, [r12, #MSC1]

        ldr     r0, =MSC2_VALUE
        str     r0, [r12, #MSC2]
        ldr     r0, [r12, #MSC2]

        ldr     r0, =MECR_VALUE
        str     r0, [r12, #MECR]

        ldr     r0, =MCMEM0_VALUE
        str     r0, [r12, #MCMEM0]

        ldr     r0, =MCMEM1_VALUE
        str     r0, [r12, #MCMEM1]

        ldr     r0, =MCATT0_VALUE
        str     r0, [r12, #MCATT0]

        ldr     r0, =MCATT1_VALUE
        str     r0, [r12, #MCATT1]

        ldr     r0, =MCIO0_VALUE
        str     r0, [r12, #MCIO0]

        ldr     r0, =MCIO1_VALUE
        str     r0, [r12, #MCIO1]

        //according to Intel's comments, we're extracting the DRI

	// Loading the MDREFR in a way that will make it configure
	// correctly is a multi-step process.  Please read Section
	// 6.12 of the PXA Developers Manual For more Details

	// 1.
	// The first step requires that we set K0RUN and E0PIN while
	// configuring DRI and clearing KXFREE.  All other values
	// MUST be left alone
        ldr     r0, =MDREFR_VALUE
        ldr     r3, [r12, #MDREFR]
        ldr     r1, =0xFFF
        and     r0, r0, r1

	// Make the DRI we read from MDREFR what MDREFR_VALUE says it is.
	// We also Free KXFREE the free running bits.
        bic     r3, r3, r1
	bic	r3, r3, #0x03800000
        orr     r3, r3, r0

        //Write it back
        str     r3, [r12, #MDREFR]


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
        ldr     r0, =MDREFR_VALUE
	ldr	r1, =0xF6000	// Mask of SDCLK's settings minus EXPIN
	and	r0, r0, r1
        bic     r3, r3, r1
	orr	r3, r3, r0

        str     r3, [r12, #MDREFR]
	ldr	r3, [r12, #MDREFR]

	// 4.
        // Although I think that this should be at #3 It is here cause this
	// is where it was originally.  This will turn off Self-Refresh.
        bic     r3, r3, #0x00400000
        str     r3, [r12, #MDREFR]

	// 5.
	// Finally, we Enable the Various SDCLK's and let it run.
	// Also, enable the free-running clocks (not mentioned in the manual).
	ldr	r0, =MDREFR_VALUE
	ldr	r1, =0x03809000
	and	r0, r0, r1
	orr	r3, r3, r0
        str     r3, [r12, #MDREFR]
        nop
        nop

        //Step 4 in Intel's code
        ldr     r0, =MDCNFG_VALUE

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
	ldr	r0, =SDRAM_BASE
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
        ldr     r0, [r12, #MDCNFG]

        //enable bank 0 (what about bank 1?)
        orr     r0, r0, #0x00000001
        str     r0, [r12, #MDCNFG]

        //Step 10
        //write MDMRS again
        ldr     r0, =MDMRS_VALUE
        str     r0, [r12, #MDMRS]


        //Step 11
        //are we A1_Cotulla?
        ldr     r0, [r12, #MDREFR]

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

