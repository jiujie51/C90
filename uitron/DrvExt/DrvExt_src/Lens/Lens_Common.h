/**
 * Novatek lens common header.
 * 
 * Lens common header.
 * 
 * @file Lens_Common.h
 * @note Nothing.
 *
 * Copyright (C) 2017 Novatek Microelectronics Corp. All rights reserved.
 *
 */

#ifndef _LENS_COMMON_H_
#define _LENS_COMMON_H_

#include "Type.h"
#include "Utility.h"


//=============================================================================
// compile switch
//=============================================================================
#define CALI_USAGE    // for calibration usage


//=============================================================================
// macro definition
//=============================================================================
#define LENS_DELAY_US(x)      Delay_DelayUs(x)
#define LENS_DELAY_MS(x)      Delay_DelayMs(x)

#define LENS_ABS(X)          (((X) < 0) ? -(X) : (X))
#define LENS_MAX(A,B)        (((A) > (B)) ? (A) : (B))
#define LENS_MIN(A,B)        (((A) < (B)) ? (A) : (B))
#define LENS_CLAMP(X,L,H)    (((X) > (H)) ? (H) : (((X) < (L)) ? (L) : (X)))
#define LENS_ARRAY_SIZE(X)   (sizeof((X)) / sizeof((X[0])))


//=============================================================================
// constant define
//=============================================================================
#define LOGICAL_LOW_LEVEL           0
#define LOGICAL_HI_LEVEL            1

#define MOVE_DIR_INIT               0
#define MOVE_DIR_FWD                1
#define MOVE_DIR_BWD                (-1)

#define MOTOR_STATUS_FREE           0
#define MOTOR_STATUS_BUSY           1


// motor speed
typedef enum
{
    SPEED_OFF  = 0,
    SPEED_FULL = 100,
    SPEED_MAX_IDX
} MOTOR_SPEED;


//=============================================================================
// struct & flag definition
//=============================================================================
typedef struct
{
    LENS_TAB LensDrv;

    LENSCTRL_TASK_API LensCtrlTsk;

    UINT32 uiFocalLengthMin;    // unit: um
    UINT32 uiFocalLengthMax;    // unit: um

    UINT32 uiZoomSecPos;
    INT32 iFocusCaliOfst[ZOOM_SECTION_NUM];
    IRIS_POS eAperturePos;
    MOTOR_SHUTTER_ACT eShutterState;
    MOTOR_IRCUT_ACT eIrCutState;
} LENSDRV_INFO;


#endif    // _LENS_COMMON_H_


