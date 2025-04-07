

#include"DSP28x_Project.h"
#include"main.h"


void MemCopy(Uint16 *SourceAddr, Uint16* SourceEndAddr, Uint16* DestAddr);

__interrupt void epwm1_timer_isr(void);

__interrupt void epwm1_tzint_isr(void);

__interrupt void xint1_isr(void);

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
	MemCopy(&Cla1funcsLoadStart, &Cla1funcsLoadEnd,
	               &Cla1funcsRunStart);

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


    //初始化外设
    init_adc();
    init_comp();
    init_epwm();
    init_epwm_tz();
    init_timer();
    init_xint();
    init_gpio_out();
    init_cla();

    //中断地址入口
    EALLOW;  // This is needed to write to EALLOW protected registers
    PieVectTable.EPWM1_TZINT = &epwm1_tzint_isr;
    PieVectTable.EPWM1_INT = &epwm1_timer_isr;
    PieVectTable.XINT1 = &xint1_isr;
    EDIS;    // This is needed to disable write to EALLOW protected registers

    //开CPU中断
    IER |= M_INT1;                              // Enable CPU INT1
    IER |= M_INT2;
    IER |= M_INT3;

    //开PIE中断
    PieCtrlRegs.PIEIER2.bit.INTx1 = 1; //TZ中断
    PieCtrlRegs.PIEIER3.bit.INTx1 = 1;//EPWM1中断
    PieCtrlRegs.PIEIER1.bit.INTx4 = 1;//外部中断1

    EALLOW;
    EPwm1Regs.ETSEL.bit.INTEN = 1;// ENABLE EPMW1_ET_INT
    EPwm1Regs.TZEINT.bit.OST = 1;// ENABLE EPMW1_TZ使能单次触发中断
    XIntruptRegs.XINT1CR.bit.ENABLE = 1;//Enable XINT1
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
    	if(ClatoCpuVar.test == 1)
    	{
    		CputoClaVar.test = 1;
            GPIO3_SET();
    	}
    }


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


//   EALLOW;
//   EPwm1Regs.TZCLR.bit.OST = 1;
//   EPwm1Regs.TZCLR.bit.INT = 1;
//   EDIS;

    //
    // Acknowledge this interrupt to receive more interrupts from group 2
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;
}

__interrupt void
xint1_isr(void)
{

    //
    // Acknowledge this interrupt to get more from group 1
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

//
// cpu_timer0_isr - Cpu Timer0 ISR
//
//__interrupt void
//cpu_timer0_isr(void)
//{
//
//    //
//    // Acknowledge this interrupt to receive more interrupts from group 1
//    //
//    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
//}
