

#include "DSP28x_Project.h"


void  adc_soc_select();

void epwm1_init();

void epwm_adc_soc();

void MemCopy(Uint16 *SourceAddr, Uint16* SourceEndAddr, Uint16* DestAddr);


/**
 * main.c
 */
int main(void)
{

    //
    // Step 1. Initialize System Control:
    // PLL, WatchDog, enable Peripheral Clocks
    // This example function is found in the DSP2803x_SysCtrl.c file.
    //
    InitSysCtrl();

  	MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd,
  	               &RamfuncsRunStart);

    InitFlash();


    //
    // Step 2. Initialize GPIO:
    // This example function is found in the DSP2803x_Gpio.c file and
    // illustrates how to set the GPIO to it's default state.
    //
    // InitGpio();  // Skipped for this example

    //
    // Step 3. Clear all interrupts and initialize PIE vector table:
    // Disable CPU interrupts
    //
    DINT;

    //
    // Initialize the PIE control registers to their default state.
    // The default state is all PIE interrupts disabled and flags
    // are cleared.
    // This function is found in the DSP2803x_PieCtrl.c file.
    //
    InitPieCtrl();

    //
    // Disable CPU interrupts and clear all CPU interrupt flags:
    //
    IER = 0x0000;
    IFR = 0x0000;

    //
    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    // This will populate the entire table, even if the interrupt
    // is not used in this example.  This is useful for debug purposes.
    // The shell ISR routines are found in DSP2803x_DefaultIsr.c.
    // This function is found in DSP2803x_PieVect.c.
    //
    InitPieVectTable();

    //
    // Step 4. Initialize the Device Peripheral. This function can be
    //         found in DSP2803x_CpuTimers.c
    //
    InitCpuTimers();   // For this example, only initialize the Cpu Timers

    //
    // Configure CPU-Timer 0 to interrupt every 500 milliseconds:
    // 60MHz CPU Freq, 50 millisecond Period (in uSeconds)
    //
    ConfigCpuTimer(&CpuTimer0, 60, 500000);

    //
    // Use write-only instruction to set TSS bit = 0
    //
    CpuTimer0Regs.TCR.all = 0x4001;


    //中断地址入口

    //开CPU中断

    //开PIE中断

    InitAdc();  // For this example, init the ADC
    AdcOffsetSelfCal();

    adc_soc_select();

    epwm1_init();

    epwm_adc_soc();

    //
    // Configure GPIO34 as a GPIO output pin
    //
    EALLOW;
    GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;
    EDIS;

    //
    // Enable global Interrupts and higher priority real-time debug events:
    //
    EINT;       // Enable Global interrupt INTM
    ERTM;       // Enable Global realtime interrupt DBGM

    //
    // Step 6. IDLE loop. Just sit and loop forever (optional):
    //
    for(;;)
    {
    	if(CpuTimer0Regs.TCR.bit.TIF==1)
    	{
    		CpuTimer0Regs.TCR.bit.TIF = 1;
    		GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;
    	}
    }


}



void  adc_soc_select()
{
    EALLOW;
  	AdcRegs.ADCSAMPLEMODE.all=0;           // Single sample mode

//根据TI的勘误表建议，增加了26个CYCLE采样保持时间，以达到550NS
    //Other invalid ACQPS  selections: 10h, 11h, 12h, 13h, 14h, 1Dh, 1Eh, 1Fh, 20h,
    // 21h, 2Ah, 2Bh, 2Ch, 2Dh, 2Eh, 37h, 38h, 39h, 3Ah, 3Bh
//  AdcRegs.ADCSOC10CTL.bit.ACQPS = 0x22 ;

	AdcRegs.ADCSOC0CTL.bit.TRIGSEL = 5;//EPWM1触发SOCA
	AdcRegs.ADCSOC0CTL.bit.ACQPS = 0x08;//选择采样保持时间
	AdcRegs.ADCSOC0CTL.bit.CHSEL = 0;//选择ADC通道

	AdcRegs.SOCPRICTL.bit.RRPOINTER = 0x01;//储存两个 用一个SOC时间做缓冲时间

    AdcRegs.SOCPRICTL.bit.SOCPRIORITY = 0x00;

	EDIS;
}

void epwm1_init()
{

		EPwm1Regs.TBPRD = 30000000;//周期值
		EPwm1Regs.TBPHS.half.TBPHS = 1;//=1 可以使用TBCTL.bit.SWFSYNC
		EPwm1Regs.TBCTL.bit.PHSEN = TB_ENABLE;//Slave module

		EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;	// 写入为0没有效果  =1同步一次epwm的初始计数值
		EPwm1Regs.TBCTL.bit.PHSDIR = TB_UP;		//Phase Direction Down

		EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;	// up/down Count
		EPwm1Regs.TBCTL.bit.PRDLD = TB_SHADOW;

		//TBCLK=SYSCLKOUT/(HSPCLKDIV * CLKDIV)=SYSCLKOUT
		EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;				//Timebase clock pre-scale
		EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;			//High speed time pre-scale

		//事件触发方式
		EPwm1Regs.ETSEL.bit.INTSEL = ET_CTR_PRD;			// Select INT on Prd event
		EPwm1Regs.ETSEL.bit.INTEN = 0;// Disable INT
		EPwm1Regs.ETPS.bit.INTPRD = ET_1ST;				// Generate INT on 1st event
		EPwm1Regs.ETCLR.all = 0x000F ; 		//clear all ETFLAG

		//触发ADC时间
		EPwm1Regs.CMPA.half.CMPA = 10000000;
		//EPwm1Regs.CMPB = EPWM1_MIN_CMPB;

		//影子寄存器的加载时间
		EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
		EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
		EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
		EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;

		//高精度分辨
	    EPwm1Regs.HRCNFG.all = 0x0;//
	    EPwm1Regs.HRCNFG.bit.HRLOAD = HR_CTR_ZERO;//HR_CTR_ZERO_PRD;
		EPwm1Regs.HRCNFG.bit.CTLMODE = HR_CMP;
		EPwm1Regs.HRCNFG.bit.EDGMODE = HR_BEP;//rising and falling edge

		EPwm1Regs.HRCNFG.bit.AUTOCONV = 0x01;  // Enable autoconversion
	    EPwm1Regs.HRPCTL.bit.HRPE = 0x01; // Turn on high-resolution period control.
	    EPwm1Regs.HRPCTL.bit.TBPHSHRLOADE = 1;

		//EPwm1Regs.TBCTL.bit.SWFSYNC = 1;   //同步一次epwm的初始计数值

}

void epwm_adc_soc()
{

    EPwm1Regs.ETSEL.bit.SOCAEN  = 1;    // Enable SOC on A group
    EPwm1Regs.ETSEL.bit.SOCASEL = 4;    // Select SOC from from CPMA on upcount
    EPwm1Regs.ETPS.bit.SOCAPRD  = 1;    // Generate pulse on 1st event

}


void MemCopy(Uint16 *SourceAddr, Uint16* SourceEndAddr, Uint16* DestAddr)
{
    while(SourceAddr < SourceEndAddr)
    {
       *DestAddr++ = *SourceAddr++;
    }
    return;
}

////
//// cpu_timer0_isr - Cpu Timer0 ISR
////
//__interrupt void
//cpu_timer0_isr(void)
//{
//    CpuTimer0.InterruptCount++;
//
//    //
//    // Toggle GPIO34 once per 500 milliseconds
//    //
//    GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;
//
//    //
//    // Acknowledge this interrupt to receive more interrupts from group 1
//    //
//    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
//}
