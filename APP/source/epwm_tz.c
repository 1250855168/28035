/*
 * epwm_tz.c
 *
 *  Created on: 2025年4月3日
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
