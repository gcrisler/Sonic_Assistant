
/*******************************************************************************
 *
 *  system_pre_init.c - system_pre_init.c
 *
 ******************************************************************************/

/*
 * The function _system_pre_init it called by the start-up code before
 * "main" is called, and before data segment initialization is
 * performed.
 *
 * This is a template file, modify to perform any initialization that
 * should take place early.
 *
 * The return value of this function controls if data segment
 * initialization should take place. If 0 is returned, it is bypassed.
 *
 * For the MSP430 microcontroller family, please consider disabling
 * the watchdog timer here, as it could time-out during the data
 * segment initialization.
 */

#include <intrinsics.h>
#include <stdint.h>
#include "msp430.h"


int _system_pre_init(void)

{
    /* Insert your low-level initializations here */

    /* Disable Watchdog timer to prevent reset during */
    /* int32_t variable initialization sequences. */
    // Stop WDT
    WDTCTL = WDTPW + WDTHOLD;

    /*
     * Configure CS module
     * MCLK  = 16 MHz from DCOCLK
     * SMCLK = 8MHz from DCOCLK
     * ACLK  = LFXTCLK expected to have a 32.768 KHz
     */
	// Unlock CS registers
	CSCTL0_H = CSKEY >> 8;
    // Set DCO to 16MHz
#if (USS_PULSE_MODE == 2)
    CSCTL1 = DCORSEL | DCOFSEL_4;
    // Configure wait states to be able to use 16 MHz MCLK
    FRCTL0 = (FRCTLPW | NWAITS_2);
    // Configure clock dividers all dividers
    CSCTL3 = (DIVA__1 | DIVS__2 | DIVM__1);
#else
    // Set DCO to 8MHz
    CSCTL1 = DCORSEL | DCOFSEL_3;
    // Configure clock dividers all dividers
    CSCTL3 = (DIVA__1 | DIVS__1 | DIVM__1);
#endif
    // Set SMCLK = MCLK = DCO, ACLK = LFXTCLK
    CSCTL2 = SELA__LFXTCLK | SELS__DCOCLK | SELM__DCOCLK;
	CSCTL4 |= (LFXTDRIVE_3);
	CSCTL4 &= ~(LFXTOFF);
	CSCTL0_H = 0;

	// GPIO Configuration
	PAOUT = 0;
	PADIR = 0xFFFF;

	PBOUT = 0;
	PBDIR = 0xFFFF;

	PCOUT = 0;
	PCDIR = 0xFFFF;

	PDOUT = 0;
	PDDIR = 0xFFFF;

    PEOUT = 0;
    PEDIR = 0xFFFF;

    /*
     * Configure LFXT GPIO pins and start
     */
	PJSEL0 |= BIT4 | BIT5;

	// Disable the GPIO power-on default high-impedance mode to activate
	// previously configured port settings
	PM5CTL0 &= ~LOCKLPM5;


    /*==================================*/
    /* Choose if segment initialization */
    /* should be done or not.           */
    /* Return: 0 to omit initialization */
    /* 1 to run initialization          */
    /*==================================*/
    return(1);
}
