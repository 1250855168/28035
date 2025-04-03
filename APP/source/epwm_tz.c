/*
 * epwm_tz.c
 *
 *  Created on: 2025��4��3��
 *      Author: ych
 */
#include"epwm_tz.h"

static void init_epwm1_tz();

void init_epwm_tz()
{
	init_epwm1_tz();
}

static void init_epwm1_tz()
{
    EALLOW;
    //---------------------------------------------------------------------
    // ѡ��TZ����Դ
    //---------------------------------------------------------------------
    // DCAH���߲ࣩ����Դ = COMP1���
    EPwm1Regs.DCTRIPSEL.bit.DCAHCOMPSEL = DC_COMP1OUT;
    EPwm1Regs.DCTRIPSEL.bit.DCALCOMPSEL = DC_COMP1OUT; // �Ͳ಻ʹ��

    //---------------------------------------------------------------------
    // ���ô�������
    //---------------------------------------------------------------------
    EPwm1Regs.TZDCSEL.bit.DCAEVT1 = TZ_DCAH_HI; // DCAEVT1��DCAH�ߵ�ƽ����
    EPwm1Regs.DCACTL.bit.EVT1SRCSEL = DC_EVT1;   // �¼�ԴΪDCAEVT1
    EPwm1Regs.DCACTL.bit.EVT1FRCSYNCSEL = DC_EVT_SYNC; // ͬ��·��

    //---------------------------------------------------------------------
    // ʹ��TZ����
    //---------------------------------------------------------------------
    EPwm1Regs.TZSEL.bit.DCAEVT1 = 1;      // ʹ��DCAEVT1����TZ
    EPwm1Regs.TZCTL.bit.TZA = TZ_FORCE_LO; // ����ʱǿ��EPWM1A�����
    EPwm1Regs.TZCTL.bit.TZB = TZ_FORCE_LO; // ����ʱǿ��EPWM1B�����

    //---------------------------------------------------------------------
    // �����־ & ʹ���ж�
    //---------------------------------------------------------------------
    EPwm1Regs.TZCLR.all = 0x0007;         // �������TZ��־
    EPwm1Regs.TZEINT.bit.OST = 1;         // ʹ�ܵ��δ����ж�
    EDIS;
}
