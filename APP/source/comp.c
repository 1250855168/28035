/*
 * comp.c
 *
 *  Created on: 2025��4��3��
 *      Author: ych
 */
#include"comp.h"

static void InitComp1(void);

void init_comp()
{
	InitComp1();
}

static void InitComp1(void)
{
    EALLOW;
    //---------------------------------------------------------------------
    // ����AIO2���Ź���
    //---------------------------------------------------------------------
    // AIO2��ΪCOMP1A���루ģ���źţ�
    // ����оƬ�ֲ᣺AIOMUX1.AIO2[1:0] = 10b
    GpioCtrlRegs.AIOMUX1.bit.AIO2 = 2; // 2 = AIO2��ΪCOMP1A����

    //---------------------------------------------------------------------
    // ����COMP1�Ƚ���
    //---------------------------------------------------------------------
    Comp1Regs.COMPCTL.bit.CMPINV = 0;      // ���������
    Comp1Regs.COMPCTL.bit.COMPSOURCE = 0;  // �Ƚ�Դ���ڲ�DAC
    Comp1Regs.COMPCTL.bit.SYNCSEL = 1;      // ͬ������������첽������
    Comp1Regs.COMPCTL.bit.QUALSEL = 0x0;    // ���˲�
    Comp1Regs.COMPCTL.bit.COMPDACEN = 1;   // ʹ�ܱȽ�����DAC

    // �����ڲ�DAC��ֵ������DACΪ10λ���ο���ѹ3.3V��
    // ����ֵ = (Ŀ���ѹ / 3.3) * 1023
    //float target_voltage = 1.0; // 1.6V��ֵ
    Comp1Regs.DACVAL.all = 50;

    //---------------------------------------------------------------------
    // ������Ҫ����
    //---------------------------------------------------------------------
    AdcRegs.COMPHYSTCTL.bit.COMP1_HYST_DISABLE = 1; // �����ͻ�
    EDIS;
}
