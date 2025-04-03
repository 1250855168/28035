/*
 * timer.c
 *
 *  Created on: 2025Äê4ÔÂ3ÈÕ
 *      Author: ych
 */
#include"timer.h"

#define timer0_is_set()   CpuTimer0Regs.TCR.bit.TIF==1
#define timer0_clear() 	  CpuTimer0Regs.TCR.bit.TIF

static void init_timer0();

void init_timer()
{
	init_timer0();
}

static void init_timer0()
{
    //
    // Configure CPU-Timer 0 to interrupt every 500 milliseconds:
    // 60MHz CPU Freq, 50 millisecond Period (in uSeconds)
    //
    ConfigCpuTimer(&CpuTimer0, 60, 500000);

    //
    // Use write-only instruction to set TSS bit = 0
    //
    CpuTimer0Regs.TCR.all = 0x4001;
}
