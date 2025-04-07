/*
 * cla.h
 *
 *  Created on: 2025Äê4ÔÂ3ÈÕ
 *      Author: ych
 */

#ifndef APP_INCLUDE_CLA_H_
#define APP_INCLUDE_CLA_H_
#include "DSP28x_Project.h"

void init_cla();

//extern Uint32 Cla1Task1;
//extern Uint32 Cla1Task2;
//extern Uint32 Cla1Task3;
//extern Uint32 Cla1Task4;
//extern Uint32 Cla1Task5;
//extern Uint32 Cla1Task6;
//extern Uint32 Cla1Task7;
//extern Uint32 Cla1Task8;
//extern Uint32 Cla1Prog_Start;
//
//extern Uint32 Cla1T1End;
//extern Uint32 Cla1T2End;
//extern Uint32 Cla1T3End;
//extern Uint32 Cla1T4End;
//extern Uint32 Cla1T5End;
//extern Uint32 Cla1T6End;
//extern Uint32 Cla1T7End;
//extern Uint32 Cla1T8End;

extern Uint16 Cla1Prog_Start;
extern Uint16 Cla1funcsLoadStart;
extern Uint16 Cla1funcsLoadEnd;
extern Uint16 Cla1funcsRunStart;
extern Uint16 Cla1funcsLoadSize;
extern Uint16 Cla1mathTablesLoadStart;
extern Uint16 Cla1mathTablesRunStart;
extern Uint16 Cla1mathTablesLoadSize;

__interrupt void Cla1Task1();
__interrupt void Cla1Task2();

struct CputoClaVarStruct
{
	Uint16 test;
};
extern volatile struct CputoClaVarStruct CputoClaVar;


struct ClatoCpuVarStruct
{
	Uint16 test;
};
extern volatile struct ClatoCpuVarStruct ClatoCpuVar;

#endif /* APP_INCLUDE_CLA_H_ */
