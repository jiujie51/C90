/**
 * Novatek PRINCETON PT5111 driver header.
 * 
 * Reference PT5111 driver header.
 * 
 * @file Lens_PT5111.h
 * @note Nothing.
 *
 * Copyright (C) 2019 Novatek Microelectronics Corp. All rights reserved.
 *
 */

#ifndef _LENS_PT5111_H_
#define _LENS_PT5111_H_

#include "Type.h"
#include "DxLens.h"
#include "Lens_Common.h"


#define __MODULE__          LENSDRV_PT5111
#define __DBGLVL__          2      // 0=FATAL, 1=ERR, 2=WRN, 3=UNIT, 4=FUNC, 5=IND, 6=MSG, 7=VALUE, 8=USER
#define __DBGFLT__          "*"    // *=All, [mark]=CustomClass
#include "DebugModule.h"


//=============================================================================
// compile switch
//=============================================================================
#define EXT_OUT_PIN_EXIST          0    // *
#define ISR_RISING_EDGE_TRIGGER    1


//=============================================================================
// constant define
//=============================================================================
#define LENS_DRV_NAME               "PRIN-PT5111"
#define LENS_DRV_ID                 "nvt-ver0.99"     // *


//=============================================================================
// HW pin configuration
//=============================================================================
#define MOTOR_PT5111_SPI_CH        SPI_ID_2          // *


//=============================================================================
// lens & motor parameters
//=============================================================================
#define LENS_FOCAL_LENGTH_WIDE      2700     // * 2.7 mm
#define LENS_FOCAL_LENGTH_TELE      13500    // * 13.5 mm

#define NEAR_BOUND                  0        // *
#define INF_BOUND                   1275     // * 1550 totally
#define WIDE_BOUND                  0        // *
#define TELE_BOUND                  900      // * 1400 totally
#define INIT_FOCUS_POS              15       // *
#define INIT_ZOOM_POS               440      // *

#define FOCUS_BACKLASH_CORR         9        // * correction for focus backlash
#define ZOOM_BACKLASH_CORR          22       // * correction for zoom backlash

#if !EXT_OUT_PIN_EXIST
#define FOCUS_PSUM_DELAY            66       // * unit: msec 80(3200)
#define ZOOM_PSUM_DELAY             71       // * unit: msec 80(3200)
#else
#define FOCUS_PSUM_DELAY            1
#define ZOOM_PSUM_DELAY             1
#endif


//=============================================================================
// register definition
//=============================================================================
#define LENS_REG_DEF_BEGIN(name)     \
typedef union                        \
{                                    \
    UINT16 Reg;                      \
    struct                           \
    {                                \

#define LENS_REG_DEF_BIT(field, bits)\
        unsigned field : bits;

#define LENS_REG_DEF_END(name)       \
    } Bit;                           \
} T_##name;


// REG
LENS_REG_DEF_BEGIN(PT5111_REG)
    LENS_REG_DEF_BIT(BIT0_15, 16)
LENS_REG_DEF_END(PT5111_REG)


typedef struct
{
    UINT16 wReg[64];
} PT5111_REGISTER;


//=============================================================================
// struct & flag definition
//=============================================================================
typedef enum
{
    PT5111_CHNEL1 = 0,
    PT5111_CHNEL3,
    PT5111_CHNEL_NULL,
    ENUM_DUMMY4WORD(PT5111_CHNEL_NUM)
} PT5111_CHNEL_NUM;

typedef enum
{
    PT5111_MODE_64MACRO  = 16,    // depending on gaer ratio
    PT5111_MODE_128MACRO = 32,
    PT5111_MODE_256MACRO = 64,
    PT5111_MODE_UNKNOWN  = 0xFF
} PT5111_MACROSTEP_MODE;

typedef enum
{
    PT5111_MOTOR_SPD_QUARTER = 1,
    PT5111_MOTOR_SPD_HALF    = 2,
    PT5111_MOTOR_SPD_1X      = 4,
    PT5111_MOTOR_SPD_2X      = 8,
    PT5111_MOTOR_SPD_4X      = 16,
    ENUM_DUMMY4WORD(PT5111_MOTOR_SPEED)
} PT5111_MOTOR_SPEED;

typedef enum
{
    PT5111_SET_POWER_CTRL = 0,
    PT5111_SET_OPERATE_CTRL,
    PT5111_SET_MOTOR_SPD,
    PT5111_SET_PULSE_NUM,
    PT5111_SET_FZ_OUTPUT_PULSE,
    ENUM_DUMMY4WORD(PT5111_CMD_TYPE)
} PT5111_CMD_TYPE;


typedef struct
{
    PT5111_CHNEL_NUM eChnelSel;
    PT5111_MACROSTEP_MODE eMicroStepMode;
    INT32 iDefPositiveDir;
    UINT32 uiWaveFreq;
    volatile INT32 iWorkStatus;
    volatile INT32 iMotorStatus;
    INT32 iMaxStepSize;
    BOOL bMove2Home;
    volatile INT32 iCurrPosition;
    INT32 iMoveDir;
    PT5111_MOTOR_SPEED eMoveSpeed;
    INT32 iBacklashValue;
    INT32 iDlyTime;
    BOOL bSetMove2Stop;
    INT32 iInitPosBias;
} MOTOR_CONFIG;


typedef struct
{
    MOTOR_TAB LensMotor;

    UINT32 uiPinIn[LENS_IO_MAX];    // GPIO related variable

    PT5111_REGISTER LensMotorReg;

    MOTOR_CONFIG LensMotorConfig[2];    // for focus & zoom

    LENS_ZOOM_FOCUS_TAB ZoomFocusTbl[ZOOM_SECTION_NUM];

    LENS_FOCUS_DISTANCE_TAB FocusDistanceTbl[ZOOM_SECTION_NUM];
} LENSMOTOR_INFO;


//=============================================================================
// external functions
//=============================================================================
extern LENSMOTOR_INFO* PT5111_GetMotorInfo(void);
extern INT32 PT5111_TransmitCmd(PT5111_CHNEL_NUM chnel, PT5111_CMD_TYPE cmd_type, INT32 cmd_arg1, INT32 cmd_arg2);


#endif    // _LENS_PT5111_H_


