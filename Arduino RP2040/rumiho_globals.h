#pragma once

#ifndef _RUMIHO_GLOBALS_H
#define _RUMIHO_GLOBALS_H

#define IR_PIN_C A3
#define IR_PIN_R A6
#define IR_PIN_L A7

extern int valueIR_C;
extern int valueIR_R;
extern int valueIR_L;

extern int minValue[3], maxValue[3], threshold[3];

#endif //_RUMIHO_GLOBALS_H