

#include "DSP28x_Project.h"


void  adc_soc_select();

void epwm1_init();

void epwm_adc_soc();

void init_epwm1_tz();

void InitComp1(void);

void MemCopy(Uint16 *SourceAddr, Uint16* SourceEndAddr, Uint16* DestAddr);

__interrupt void epwm1_timer_isr(void);

__interrupt void epwm1_tzint_isr(void);

Uint16 TEST1 = 0;

#define DC_DB_TIME_83NS    5   // 1/60mHz *4 =67ns

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
    EALLOW;  // This is needed to write to EALLOW protected registers
    PieVectTable.EPWM1_TZINT = &epwm1_tzint_isr;
    PieVectTable.EPWM1_INT = &epwm1_timer_isr;
    EDIS;    // This is needed to disable write to EALLOW protected registers



    //开CPU中断
    IER |= M_INT1;                              // Enable CPU INT1
    IER |= M_INT2;
    IER |= M_INT3;

    //开PIE中断
    PieCtrlRegs.PIEIER2.bit.INTx1 = 1; //TZ中断
    PieCtrlRegs.PIEIER3.bit.INTx1 = 1;//EPWM1中断

    InitAdc();  // For this example, init the ADC
    AdcOffsetSelfCal();

    InitEPwm1Gpio();

    InitComp1();

    adc_soc_select();

    epwm1_init();

    epwm_adc_soc();

    init_epwm1_tz();

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
    		//GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;
    	}
    }


}

void InitComp1(void)
{
    EALLOW;
    //---------------------------------------------------------------------
    // 配置AIO2引脚功能
    //---------------------------------------------------------------------
    // AIO2作为COMP1A输入（模拟信号）
    // 根据芯片手册：AIOMUX1.AIO2[1:0] = 10b
    GpioCtrlRegs.AIOMUX1.bit.AIO2 = 2; // 2 = AIO2作为COMP1A输入

    //---------------------------------------------------------------------
    // 配置COMP1比较器
    //---------------------------------------------------------------------
    Comp1Regs.COMPCTL.bit.CMPINV = 0;      // 不反向输出
    Comp1Regs.COMPCTL.bit.COMPSOURCE = 0;  // 比较源：内部DAC
    Comp1Regs.COMPCTL.bit.SYNCSEL = 1;      // 同步输出（避免异步抖动）
    Comp1Regs.COMPCTL.bit.QUALSEL = 0x0;    // 无滤波
    Comp1Regs.COMPCTL.bit.COMPDACEN = 1;   // 使能比较器和DAC

    // 设置内部DAC阈值（假设DAC为10位，参考电压3.3V）
    // 计算值 = (目标电压 / 3.3) * 1023
    //float target_voltage = 1.0; // 1.6V阈值
    Comp1Regs.DACVAL.all = 50;

    //---------------------------------------------------------------------
    // 其他必要配置
    //---------------------------------------------------------------------
    AdcRegs.COMPHYSTCTL.bit.COMP1_HYST_DISABLE = 1; // 禁用滞回
    EDIS;
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

//
// InitEPwm1Gpio - This function initializes GPIO pins to function as EPwm1
//

void epwm1_init()
{
	EALLOW;
	EPwm1Regs.TBPRD = 200;//周期值
	EPwm1Regs.TBPHS.half.TBPHS = 1;//=1 可以使用TBCTL.bit.SWFSYNC
	EPwm1Regs.TBCTL.bit.PHSEN = TB_ENABLE;//Slave module

	EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;	// 写入为0没有效果  =1同步一次epwm的初始计数值
	EPwm1Regs.TBCTL.bit.PHSDIR = TB_UP;		//Phase Direction Down

	EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;	// up/down Count
	EPwm1Regs.TBCTL.bit.PRDLD = TB_SHADOW;//设置TB为影子加载

	//TBCLK=SYSCLKOUT/(HSPCLKDIV * CLKDIV)=SYSCLKOUT
	EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;				//Timebase clock pre-scale
	EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;			//High speed time pre-scale

	//事件触发方式
	EPwm1Regs.ETSEL.bit.INTSEL = ET_CTR_PRD;			// Select INT on Prd event
	EPwm1Regs.ETSEL.bit.INTEN = 1;// Disable INT
	EPwm1Regs.ETPS.bit.INTPRD = ET_1ST;				// Generate INT on 1st event
	EPwm1Regs.ETCLR.all = 0x000F ; 		//clear all ETFLAG

	//
	// Setup compare
	//
	//互补发波
	EPwm1Regs.CMPB = 195;
	//触发ADC时间
	EPwm1Regs.CMPA.half.CMPA = 5;

	//影子寄存器的加载时间
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
    EPwm1Regs.DBRED =DC_DB_TIME_83NS;
    EPwm1Regs.DBFED =DC_DB_TIME_83NS;

	//高精度分辨
	EPwm1Regs.HRCNFG.all = 0x0;//
	EPwm1Regs.HRCNFG.bit.HRLOAD = HR_CTR_ZERO;//HR_CTR_ZERO_PRD;
	EPwm1Regs.HRCNFG.bit.CTLMODE = HR_CMP;
	EPwm1Regs.HRCNFG.bit.EDGMODE = HR_BEP;//rising and falling edge

	EPwm1Regs.HRCNFG.bit.AUTOCONV = 0x01;  // Enable autoconversion
	EPwm1Regs.HRPCTL.bit.HRPE = 0x01; // Turn on high-resolution period control.
	EPwm1Regs.HRPCTL.bit.TBPHSHRLOADE = 1;

	EPwm1Regs.TBCTL.bit.SWFSYNC = 1;   //同步一次epwm的初始计数值
	EDIS;

}



void init_epwm1_tz()
{
    EALLOW;
    //---------------------------------------------------------------------
    // 选择TZ触发源
    //---------------------------------------------------------------------
    // DCAH（高侧）触发源 = COMP1输出
    EPwm1Regs.DCTRIPSEL.bit.DCAHCOMPSEL = DC_COMP1OUT;
    EPwm1Regs.DCTRIPSEL.bit.DCALCOMPSEL = DC_COMP1OUT; // 低侧不使能

    //---------------------------------------------------------------------
    // 配置触发条件
    //---------------------------------------------------------------------
    EPwm1Regs.TZDCSEL.bit.DCAEVT1 = TZ_DCAH_HI; // DCAEVT1由DCAH高电平触发
    EPwm1Regs.DCACTL.bit.EVT1SRCSEL = DC_EVT1;   // 事件源为DCAEVT1
    EPwm1Regs.DCACTL.bit.EVT1FRCSYNCSEL = DC_EVT_SYNC; // 同步路径

    //---------------------------------------------------------------------
    // 使能TZ动作
    //---------------------------------------------------------------------
    EPwm1Regs.TZSEL.bit.DCAEVT1 = 1;      // 使能DCAEVT1触发TZ
    EPwm1Regs.TZCTL.bit.TZA = TZ_FORCE_LO; // 触发时强制EPWM1A输出低
    EPwm1Regs.TZCTL.bit.TZB = TZ_FORCE_LO; // 触发时强制EPWM1B输出低

    //---------------------------------------------------------------------
    // 清除标志 & 使能中断
    //---------------------------------------------------------------------
    EPwm1Regs.TZCLR.all = 0x0007;         // 清除所有TZ标志
    EPwm1Regs.TZEINT.bit.OST = 1;         // 使能单次触发中断
    EDIS;
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


__interrupt void
epwm1_timer_isr(void)
{

	GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;
    //
    // Clear INT flag for this timer
    //
    EPwm1Regs.ETCLR.bit.INT = 1;

    //
    // Acknowledge this interrupt to receive more interrupts from group 3
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}


__interrupt void
epwm1_tzint_isr(void)
{

    //
    // Leave these flags set so we only take this
    // interrupt once
    //

	TEST1++;

   EALLOW;
   EPwm1Regs.TZCLR.bit.OST = 1;
   EPwm1Regs.TZCLR.bit.INT = 1;
   EDIS;

    //
    // Acknowledge this interrupt to receive more interrupts from group 2
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;
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
