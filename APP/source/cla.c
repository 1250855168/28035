/*
 * cla.c
 *
 *  Created on: 2025��4��3��
 *      Author: ych
 */
#include "cla.h"

void init_cla()
{
	// Initialize the CLA registers
	EALLOW;

	// ��������1������2����ڵ�ַ
	Cla1Regs.MVECT1 = (Uint16)((Uint32)&Cla1Task1 - (Uint32)&Cla1Prog_Start);
	Cla1Regs.MVECT2 = (Uint16)((Uint32)&Cla1Task2 - (Uint32)&Cla1Prog_Start);

	// ��������1�Ĵ���ԴΪ ePWM1
	Cla1Regs.MPISRCSEL1.bit.PERINT1SEL = CLA_INT1_EPWM1INT;

	// ���ó��� RAM ������ RAM
	Cla1Regs.MMEMCFG.bit.PROGE = 1; // ���ó��� RAM
	Cla1Regs.MMEMCFG.bit.RAM1E = 1; // �������� RAM

	// ʹ�� IACK�����������������
	Cla1Regs.MCTL.bit.IACKE = 1;

	// ʹ������1������2
	Cla1Regs.MIER.all = (M_INT2 | M_INT1);

	EDIS;

	// ǿ�ƴ�������2����ѡ��
	Cla1ForceTask2();
}
