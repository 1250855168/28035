/*
 * epwm.h
 *
 *  Created on: 2025Äê4ÔÂ3ÈÕ
 *      Author: ych
 */

#ifndef APP_INCLUDE_EPWM_H_
#define APP_INCLUDE_EPWM_H_

#include "DSP2803x_Device.h"     // DSP2803x Headerfile Include File
#include "DSP2803x_Examples.h"   // DSP2803x Examples Include File

#define DC_DB_TIME_83NS    5   // 1/60mHz *4 =67ns

void init_epwm();

#endif /* APP_INCLUDE_EPWM_H_ */
