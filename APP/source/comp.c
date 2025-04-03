/*
 * comp.c
 *
 *  Created on: 2025年4月3日
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
