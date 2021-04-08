/**
 * Novatek PRINCETON PT5111 motor driver header.
 * 
 * Reference PT5111 motor driver header.
 * 
 * @file LensMotor_PT5111.h
 * @note Nothing.
 *
 * Copyright (C) 2019 Novatek Microelectronics Corp. All rights reserved.
 *
 */

#ifndef _LENSMOTOR_PT5111_H_
#define _LENSMOTOR_PT5111_H_

#include "Type.h"
#include "SysKer.h"
#include "Lens_PT5111.h"


extern UINT32 _SECTION(".kercfg_data") g_uiPt5111SpiSema;


//=============================================================================
// constant define
//=============================================================================


//=============================================================================
// struct & flag definition
//=============================================================================
typedef enum
{
    MOTOR_FOCUS = 0,
    MOTOR_ZOOM,
    MOTOR_IRIS,
    MOTOR_IRCUT,
    ENUM_DUMMY4WORD(MOTOR_SEL)
} MOTOR_SEL;


typedef struct
{
    union {
        struct {
            UINT8 a5:1;
            UINT8 a4:1;
            UINT8 a3:1;
            UINT8 a2:1;
            UINT8 a1:1;
            UINT8 a0:1;

            UINT8 d9:1;
            UINT8 d8:1;
            UINT8 d7:1;
            UINT8 d6:1;
            UINT8 d5:1;
            UINT8 d4:1;
            UINT8 d3:1;
            UINT8 d2:1;
            UINT8 d1:1;
            UINT8 d0:1;
        } A_D;

        UINT16 addr_data;        // register address + data
    };
} SPI_DATA_FMT;


//=============================================================================
// motor driver parameters
//=============================================================================
static LENSMOTOR_INFO g_LensMotorInfo =
{
    .uiPinIn = { 0 },

    .LensMotorReg.wReg[0]  = 0x0000,    // chnel12 
    .LensMotorReg.wReg[1]  = 0xFFFF,    // chnel12 pulse num_l
    .LensMotorReg.wReg[2]  = 0xFFFF,    // chnel12 pulse num_h
    .LensMotorReg.wReg[3]  = 0x0CC3,    // chnel12 pulse rate (unit: 1.6 usec)
    .LensMotorReg.wReg[4]  = 0x11E0,    // chnel12 64 micro-step + pulse rate unit to 1.6 usec
    .LensMotorReg.wReg[5]  = 0xFFFF,    // chnel12 acc/dec config
    .LensMotorReg.wReg[6]  = 0xFFFF,    // chnel12 acc/dec rate1
    .LensMotorReg.wReg[7]  = 0xFFFF,    // chnel12 acc/dec rate2
    .LensMotorReg.wReg[8]  = 0xFFFF,    // chnel12 acc/dec rate3
    .LensMotorReg.wReg[9]  = 0xFFFF,    // chnel12 acc/dec rate4
    .LensMotorReg.wReg[10] = 0x2830,    // chnel12 pre-excitation time to 2.46 msec (unit: 0.82 msec)
    .LensMotorReg.wReg[11] = 0x2C40,    // chnel12 post-excitation time to 3.28 msec (unit: 0.82 msec)
    .LensMotorReg.wReg[12] = 0xFFFF,    // RSV
    .LensMotorReg.wReg[13] = 0x3400,    // chnel12 max out voltage to 5.0V
    .LensMotorReg.wReg[14] = 0xFFFF,    // chnel12 micro-step current pos
    .LensMotorReg.wReg[15] = 0x3E80,    // chnel12 operation ON

    .LensMotorReg.wReg[16] = 0x4000,    // chnel34 
    .LensMotorReg.wReg[17] = 0xFFFF,    // chnel34 pulse num_l
    .LensMotorReg.wReg[18] = 0xFFFF,    // chnel34 pulse num_h
    .LensMotorReg.wReg[19] = 0x4CC3,    // chnel34 pulse rate (unit: 1.6 usec)
    .LensMotorReg.wReg[20] = 0x51E0,    // chnel34 64 micro-step + pulse rate unit to 1.6 usec
    .LensMotorReg.wReg[21] = 0xFFFF,    // chnel34 acc/dec config
    .LensMotorReg.wReg[22] = 0xFFFF,    // chnel34 acc/dec rate1
    .LensMotorReg.wReg[23] = 0xFFFF,    // chnel34 acc/dec rate2
    .LensMotorReg.wReg[24] = 0xFFFF,    // chnel34 acc/dec rate3
    .LensMotorReg.wReg[25] = 0xFFFF,    // chnel34 acc/dec rate4
    .LensMotorReg.wReg[26] = 0x6830,    // chnel34 pre-excitation time to 2.46 msec (unit: 0.82 msec)
    .LensMotorReg.wReg[27] = 0x6C40,    // chnel34 post-excitation time to 3.28 msec (unit: 0.82 msec)
    .LensMotorReg.wReg[28] = 0xFFFF,    // DC motor output control
    .LensMotorReg.wReg[29] = 0x7400,    // chnel34 max out voltage to 5.0V
    .LensMotorReg.wReg[30] = 0xFFFF,    // chnel34 micro-step current pos
    .LensMotorReg.wReg[31] = 0x7E80,    // chnel34 operation ON

    .LensMotorReg.wReg[32] = 0x8000,    // chnel57
    .LensMotorReg.wReg[33] = 0xFFFF,    // chnel57 pulse num_l
    .LensMotorReg.wReg[34] = 0xFFFF,    // chnel57pulse num_h
    .LensMotorReg.wReg[35] = 0x8CC3,    // chnel57 pulse rate (unit: 1.6 usec)
    .LensMotorReg.wReg[36] = 0x91E0,    // chnel57 pulse rate unit to 1.6 usec
    .LensMotorReg.wReg[37] = 0xFFFF,    // chnel57 acc/dec config
    .LensMotorReg.wReg[38] = 0xFFFF,    // chnel57 acc/dec rate1
    .LensMotorReg.wReg[39] = 0xFFFF,    // chnel57 acc/dec rate2
    .LensMotorReg.wReg[40] = 0xFFFF,    // chnel57 acc/dec rate3
    .LensMotorReg.wReg[41] = 0xFFFF,    // chnel57 acc/dec rate4
    .LensMotorReg.wReg[42] = 0xA830,    // chnel57 pre-excitation time to 2.46 msec (unit: 0.82 msec)
    .LensMotorReg.wReg[43] = 0xAC40,    // chnel57 post-excitation time to 3.28 msec (unit: 0.82 msec)
    .LensMotorReg.wReg[44] = 0xFFFF,    // PWM driving config
    .LensMotorReg.wReg[45] = 0xFFFF,    // PWM duty cycle1
    .LensMotorReg.wReg[46] = 0xFFFF,    // PWM duty cycle2
    .LensMotorReg.wReg[47] = 0xBC00,    // chnel57 operation OFF

    .LensMotorReg.wReg[48] = 0xFFFF,    // RSV
    .LensMotorReg.wReg[49] = 0xC480,    // SDO + EXT12 + EXT34 output pin
    .LensMotorReg.wReg[50] = 0xFFFF,    // PI1~3 out control
    .LensMotorReg.wReg[51] = 0xCE00,    // power ON + chnel12 & chnel34 OFF
    .LensMotorReg.wReg[52] = 0xFFFF,    // RSV
    .LensMotorReg.wReg[53] = 0xFFFF,    //
    .LensMotorReg.wReg[54] = 0xFFFF,    // chnel6 control
    .LensMotorReg.wReg[55] = 0xFFFF,    // chnel7 control
    .LensMotorReg.wReg[56] = 0xFFFF,    // software reset
    .LensMotorReg.wReg[57] = 0xFFFF,    // chnel5~7 exclusive control
    .LensMotorReg.wReg[58] = 0xFFFF,    // chnel select for status read
    .LensMotorReg.wReg[59] = 0xFFFF,    // chnel12 output flag
    .LensMotorReg.wReg[60] = 0xFFFF,    // chnel34 output flag
    .LensMotorReg.wReg[61] = 0xFFFF,    // chnel57 output flag
    .LensMotorReg.wReg[62] = 0xFFFF,    // RSV
    .LensMotorReg.wReg[63] = 0xFC0C,    // chnel12 & 34 operation disable

    .LensMotorConfig[0].eChnelSel       = PT5111_CHNEL1,          // *
    .LensMotorConfig[0].eMicroStepMode  = PT5111_MODE_64MACRO,
    .LensMotorConfig[0].iDefPositiveDir = 1,                      // *
    .LensMotorConfig[0].uiWaveFreq      = 4480,
    .LensMotorConfig[0].iWorkStatus     = MOTOR_STATUS_FREE,
    .LensMotorConfig[0].iMotorStatus    = MOTOR_STATUS_FREE,
    .LensMotorConfig[0].iMaxStepSize    = 16,                     // for stop immediately
    .LensMotorConfig[0].bMove2Home      = FALSE,
    .LensMotorConfig[0].iCurrPosition   = NEAR_BOUND,
    .LensMotorConfig[0].iMoveDir        = MOVE_DIR_INIT,
    .LensMotorConfig[0].eMoveSpeed      = PT5111_MOTOR_SPD_2X,    // *
    .LensMotorConfig[0].iBacklashValue  = FOCUS_BACKLASH_CORR,
    .LensMotorConfig[0].iDlyTime        = FOCUS_PSUM_DELAY,
    .LensMotorConfig[0].bSetMove2Stop   = FALSE,
    .LensMotorConfig[0].iInitPosBias    = 0,

    .LensMotorConfig[1].eChnelSel       = PT5111_CHNEL3,          // *
    .LensMotorConfig[1].eMicroStepMode  = PT5111_MODE_64MACRO,
    .LensMotorConfig[1].iDefPositiveDir = 0,                      // *
    .LensMotorConfig[1].uiWaveFreq      = 4160,
    .LensMotorConfig[1].iWorkStatus     = MOTOR_STATUS_FREE,
    .LensMotorConfig[1].iMotorStatus    = MOTOR_STATUS_FREE,
    .LensMotorConfig[1].iMaxStepSize    = 16,                     // for stop immediately
    .LensMotorConfig[1].bMove2Home      = FALSE,
    .LensMotorConfig[1].iCurrPosition   = WIDE_BOUND,
    .LensMotorConfig[1].iMoveDir        = MOVE_DIR_INIT,
    .LensMotorConfig[1].eMoveSpeed      = PT5111_MOTOR_SPD_1X,    // *
    .LensMotorConfig[1].iBacklashValue  = ZOOM_BACKLASH_CORR,
    .LensMotorConfig[1].iDlyTime        = ZOOM_PSUM_DELAY,
    .LensMotorConfig[1].bSetMove2Stop   = FALSE,
    .LensMotorConfig[1].iInitPosBias    = 0,

                       // zoom_pos  focus_min  focus_max
    .ZoomFocusTbl[0]  = {   0,           0,    0 },
    .ZoomFocusTbl[1]  = { WIDE_BOUND,    0,  150 },        // * 1
    .ZoomFocusTbl[2]  = {  52,         165,  315 },        // * 2
    .ZoomFocusTbl[3]  = { 114,         340,  490 },        // * 3
    .ZoomFocusTbl[4]  = { 186,         505,  655 },        // * 4
    .ZoomFocusTbl[5]  = { 268,         660,  810 },        // * 5
    .ZoomFocusTbl[6]  = { 355,         785,  935 },        // * 6
    .ZoomFocusTbl[7]  = { 453,         885, 1035 },        // * 7
    .ZoomFocusTbl[8]  = { 561,         975, 1125 },        // * 8
    .ZoomFocusTbl[9]  = { 669,        1045, 1195 },        // * 9
    .ZoomFocusTbl[10] = { 777,        1090, 1240 },        // * 10
    .ZoomFocusTbl[11] = { TELE_BOUND, 1125, INF_BOUND },   // * 11

                          // 0.05   0.1  0.15   0.2   0.3   0.5   0.7   1.0   1.4   2.0   3.0  infinite (unit: meter)
    .FocusDistanceTbl[0]  = {    0,   60,   61,   62,   63,   64,   65,   66,   67,   68,   69,   70 },
    .FocusDistanceTbl[1]  = {    0,   60,   61,   62,   63,   64,   65,   66,   67,   68,   69,   70 },    // * 1
    .FocusDistanceTbl[2]  = {   52,  240,  241,  242,  243,  244,  245,  246,  247,  248,  249,  250 },    // * 2
    .FocusDistanceTbl[3]  = {  114,  415,  416,  417,  418,  419,  420,  421,  422,  423,  424,  425 },    // * 3
    .FocusDistanceTbl[4]  = {  186,  580,  581,  582,  583,  584,  585,  586,  587,  588,  589,  590 },    // * 4
    .FocusDistanceTbl[5]  = {  268,  735,  736,  737,  738,  739,  745,  741,  742,  743,  744,  745 },    // * 5
    .FocusDistanceTbl[6]  = {  355,  860,  861,  862,  863,  864,  865,  866,  867,  868,  869,  870 },    // * 6
    .FocusDistanceTbl[7]  = {  453,  960,  961,  962,  963,  964,  965,  966,  967,  968,  969,  970 },    // * 7
    .FocusDistanceTbl[8]  = {  561, 1050, 1051, 1052, 1053, 1054, 1055, 1056, 1057, 1058, 1059, 1060 },    // * 8
    .FocusDistanceTbl[9]  = {  669, 1120, 1121, 1122, 1123, 1124, 1125, 1126, 1127, 1128, 1129, 1130 },    // * 9
    .FocusDistanceTbl[10] = {  777, 1165, 1166, 1167, 1168, 1169, 1170, 1171, 1172, 1173, 1174, 1175 },    // * 10
    .FocusDistanceTbl[11] = {  900, 1200, 1201, 1202, 1203, 1204, 1205, 1206, 1207, 1208, 1209, 1210 },    // * 11
};


#endif    // _LENSMOTOR_PT5111_H_


