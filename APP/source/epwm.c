/*
 * epwm.c
 *
 *  Created on: 2025��4��3��
 *      Author: ych
 */
#include"epwm.h"

static void epwm1_init();
static void init_epwm1_gpio(void);

void init_epwm()
{
	epwm1_init();
	init_epwm1_gpio();
}

static void init_epwm1_gpio(void)
{
    EALLOW;

    //
    // Disable internal pull-up for the selected output pins
    // for reduced power consumption
    // Pull-ups can be enabled or disabled by the user.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAPUD.bit.GPIO0 = 1;    // Disable pull-up on GPIO0 (EPWM1A)
    GpioCtrlRegs.GPAPUD.bit.GPIO1 = 1;    // Disable pull-up on GPIO1 (EPWM1B)

    //
    // Configure EPWM-1 pins using GPIO regs
    // This specifies which of the possible GPIO pins will be
    // EPWM1 functional pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;   // Configure GPIO0 as EPWM1A
    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;   // Configure GPIO1 as EPWM1B

    EDIS;
}

static void epwm1_init()
{
	EALLOW;
	EPwm1Regs.TBPRD = 200;//����ֵ
	EPwm1Regs.TBPHS.half.TBPHS = 1;//=1 ����ʹ��TBCTL.bit.SWFSYNC
	EPwm1Regs.TBCTL.bit.PHSEN = TB_ENABLE;//Slave module

	EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;	// д��Ϊ0û��Ч��  =1ͬ��һ��epwm�ĳ�ʼ����ֵ
	EPwm1Regs.TBCTL.bit.PHSDIR = TB_UP;		//Phase Direction Down

	EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;	// up/down Count
	EPwm1Regs.TBCTL.bit.PRDLD = TB_SHADOW;//����TBΪӰ�Ӽ���

	//TBCLK=SYSCLKOUT/(HSPCLKDIV * CLKDIV)=SYSCLKOUT
	EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;				//Timebase clock pre-scale
	EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;			//High speed time pre-scale

	//�¼�������ʽ
	EPwm1Regs.ETSEL.bit.INTSEL = ET_CTR_PRD;			// Select INT on Prd event
	EPwm1Regs.ETSEL.bit.INTEN = 1;// ENABLE INT
	EPwm1Regs.ETPS.bit.INTPRD = ET_1ST;				// Generate INT on 1st event
	EPwm1Regs.ETCLR.all = 0x000F ; 		//clear all ETFLAG

	//
	// Setup compare
	//
	//��������
	EPwm1Regs.CMPB = 195;
	//����ADCʱ��
	EPwm1Regs.CMPA.half.CMPA = 5;

	//Ӱ�ӼĴ����ļ���ʱ��
	EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
	EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
	EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;

	//
	// Set actions
	//
	EPwm1Regs.AQCTLA.bit.PRD = AQ_SET;    // Set PWM1A on event A, up count
	EPwm1Regs.AQCTLA.bit.ZRO = AQ_CLEAR;  // Clear PWM1A on event A, down count

	EPwm1Regs.AQCTLB.bit.ZRO = AQ_SET;   // Set PWM1B on event B, up count
	EPwm1Regs.AQCTLB.bit.PRD = AQ_CLEAR; // Clear PWM1B on event B, down count

    EPwm1Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
    EPwm1Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;
    EPwm1Regs.DBCTL.bit.IN_MODE = DBA_ALL;
    EPwm1Regs.DBRED =DC_DB_TIME_83NS;//�����ӳ�
    EPwm1Regs.DBFED =DC_DB_TIME_83NS;//�½��ӳ�

	//�߾��ȷֱ�
	EPwm1Regs.HRCNFG.all = 0x0;//
	EPwm1Regs.HRCNFG.bit.HRLOAD = HR_CTR_ZERO;//HR_CTR_ZERO_PRD;
	EPwm1Regs.HRCNFG.bit.CTLMODE = HR_CMP;
	EPwm1Regs.HRCNFG.bit.EDGMODE = HR_BEP;//rising and falling edge

	EPwm1Regs.HRCNFG.bit.AUTOCONV = 0x01;  // Enable autoconversion
	EPwm1Regs.HRPCTL.bit.HRPE = 0x01; // Turn on high-resolution period control.
	EPwm1Regs.HRPCTL.bit.TBPHSHRLOADE = 1;

	EPwm1Regs.TBCTL.bit.SWFSYNC = 1;   //ͬ��һ��epwm�ĳ�ʼ����ֵ
	EDIS;

}
