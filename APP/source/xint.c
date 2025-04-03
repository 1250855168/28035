/*
 * xint.c
 *
 *  Created on: 2025年4月3日
 *      Author: ych
 */
#include"xint.h"

static void init_gpio2_in();
static void init_gpio2_xint1();

//只能使用0-32gpio
void init_xint()
{
	init_gpio2_in();
	init_gpio2_xint1();
}

static void init_gpio2_in()
{
    EALLOW;
    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 0;        // GPIO2
    GpioCtrlRegs.GPADIR.bit.GPIO2 = 0;         // input
    GpioCtrlRegs.GPAQSEL1.bit.GPIO2 = 0;       // XINT1 Synch to SYSCLKOUT only
    GpioCtrlRegs.GPACTRL.bit.QUALPRD0 = 0xFF;  // Each sampling window is 510*SYSCLKOUT
    EDIS;
}

static void init_gpio2_xint1()
{
    EALLOW;
    GpioIntRegs.GPIOXINT1SEL.bit.GPIOSEL = 2;   // XINT1 is GPIO2
    EDIS;

    XIntruptRegs.XINT1CR.bit.POLARITY = 1;      // Rising edge interrupt

    XIntruptRegs.XINT1CR.bit.ENABLE = 1;        // Enable XINT1
}
