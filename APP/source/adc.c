/*
 * adc.c
 *
 *  Created on: 2025��4��3��
 *      Author: ych
 */
#include"adc.h"

static void  adc_soc_select();
static void epwm_adc_soc();


void init_adc()
{
    InitAdc();  // For this example, init the ADC

    AdcOffsetSelfCal();

    adc_soc_select();

    epwm_adc_soc();
}

static void epwm_adc_soc()
{
    EPwm1Regs.ETSEL.bit.SOCAEN  = 1;    // Enable SOC on A group
    EPwm1Regs.ETSEL.bit.SOCASEL = 4;    // Select SOC from from CPMA on upcount
    EPwm1Regs.ETPS.bit.SOCAPRD  = 1;    // Generate pulse on 1st event

}

static void  adc_soc_select()
{
    EALLOW;
  	AdcRegs.ADCSAMPLEMODE.all=0;           // Single sample mode

//����TI�Ŀ�����飬������26��CYCLE��������ʱ�䣬�Դﵽ550NS
    //Other invalid ACQPS  selections: 10h, 11h, 12h, 13h, 14h, 1Dh, 1Eh, 1Fh, 20h,
    // 21h, 2Ah, 2Bh, 2Ch, 2Dh, 2Eh, 37h, 38h, 39h, 3Ah, 3Bh
//  AdcRegs.ADCSOC10CTL.bit.ACQPS = 0x22 ;

	AdcRegs.ADCSOC0CTL.bit.TRIGSEL = 5;//EPWM1����SOCA
	AdcRegs.ADCSOC0CTL.bit.ACQPS = 0x08;//ѡ���������ʱ��
	AdcRegs.ADCSOC0CTL.bit.CHSEL = 0;//ѡ��ADCͨ��

	AdcRegs.SOCPRICTL.bit.RRPOINTER = 0x01;//�������� ��һ��SOCʱ��������ʱ��

    AdcRegs.SOCPRICTL.bit.SOCPRIORITY = 0x00;

	EDIS;
}
