/*
 * cla.c
 *
 *  Created on: 2025年4月3日
 *      Author: ych
 */
#include "cla.h"

void init_cla()
{
	// Initialize the CLA registers
	EALLOW;

	// 设置任务1和任务2的入口地址
	Cla1Regs.MVECT1 = (Uint16)((Uint32)&Cla1Task1 - (Uint32)&Cla1Prog_Start);
	Cla1Regs.MVECT2 = (Uint16)((Uint32)&Cla1Task2 - (Uint32)&Cla1Prog_Start);

	// 设置任务1的触发源为 ePWM1
	Cla1Regs.MPISRCSEL1.bit.PERINT1SEL = CLA_INT1_EPWM1INT;

	// 启用程序 RAM 和数据 RAM
	Cla1Regs.MMEMCFG.bit.PROGE = 1; // 启用程序 RAM
	Cla1Regs.MMEMCFG.bit.RAM1E = 1; // 启用数据 RAM

	// 使能 IACK，允许软件触发任务
	Cla1Regs.MCTL.bit.IACKE = 1;

	// 使能任务1和任务2
	Cla1Regs.MIER.all = (M_INT2 | M_INT1);

	EDIS;

	// 强制触发任务2（可选）
	Cla1ForceTask2();
}
