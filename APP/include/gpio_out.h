/*
 * gpio_out.h
 *
 *  Created on: 2025Äê4ÔÂ3ÈÕ
 *      Author: ych
 */

#ifndef APP_INCLUDE_GPIO_OUT_H_
#define APP_INCLUDE_GPIO_OUT_H_

#include "DSP2803x_Device.h"     // DSP2803x Headerfile Include File
#include "DSP2803x_Examples.h"   // DSP2803x Examples Include File

#define GPIO3_SET()     GpioDataRegs.GPASET.bit.GPIO3 = 1
#define GPIO3_CLEAR()     GpioDataRegs.GPACLEAR.bit.GPIO3 = 1
#define GPIO3_TOGGLE()     GpioDataRegs.GPATOGGLE.bit.GPIO3 = 1

void init_gpio_out();


#endif /* APP_INCLUDE_GPIO_OUT_H_ */
