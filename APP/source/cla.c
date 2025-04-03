/*
 * cla.c
 *
 *  Created on: 2025Äê4ÔÂ3ÈÕ
 *      Author: ych
 */
#include"cla.h"

void init_cla()
{
	// Initialize the CLA registers
	EALLOW;
    Cla1Regs.MVECT1 = (Uint16)((Uint32)&Cla1Task1 - (Uint32)&Cla1Prog_Start);
    Cla1Regs.MVECT2 = (Uint16)((Uint32)&Cla1Task2 - (Uint32)&Cla1Prog_Start);
	// EPWM1 starts Task 1
	Cla1Regs.MPISRCSEL1.bit.PERINT1SEL = CLA_INT1_EPWM1INT;

	// Map CLA program memory to the CLA
	Cla1Regs.MMEMCFG.bit.PROGE = 1;

	// Enable IACK to start tasks via software
	Cla1Regs.MCTL.bit.IACKE = 1;

	// Enable Task 2,1
	Cla1Regs.MIER.all = (M_INT2 |M_INT1);
    EDIS;

  	// Force CLA task 2.
	Cla1ForceTask2();
}

