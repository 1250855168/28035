/*
 * init_gpio_out.c
 *
 *  Created on: 2025��4��3��
 *      Author: ych
 */
#include"gpio_out.h"

static init_gpio3_out();

void init_gpio_out()
{
	init_gpio3_out();
}

static init_gpio3_out()
{
    EALLOW;
    GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;
    EDIS;
}
