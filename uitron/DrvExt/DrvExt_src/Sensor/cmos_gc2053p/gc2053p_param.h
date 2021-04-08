/**
    GC2053P sensor parameter .


    @file       GC2053P_param.h
    @ingroup    mISYSAlg
    @note       Nothing (or anything need to be mentioned).

    Copyright   Novatek Microelectronics Corp. 2011.  All rights reserved.
*/
#ifndef _GC2053P_PARAM_H_
#define _GC2053P_PARAM_H_

#include "sensor.h"

/**
     Sensor mode
     plz map to enum: SENSOR_MODE sequenitally and continuously
*/
#define MODE_MAX    2               //total senMode supported by this sensor

#define FPS_DEPEND_ON_EXPT 0
#define FPS_MUL_FPS        1

// get sensor information
extern SENSOR_DRV_TAB *sensor_get_drv_tab_GC2053P(void);
extern SENSOR_DRV_INFO *get_sensor_drv_info_GC2053P(void);
extern SENSOR_MODE_INFO *get_mode_info_GC2053P(SENSOR_MODE mode);
extern UINT32 get_cmd_tab_GC2053P(SENSOR_MODE mode, SENSOR_CMD **cmd_tab);
extern SEN_CMD_INFO *get_sensor_cmd_info_GC2053P(void);
extern SEN_IF_INFO *get_sensor_if_info_GC2053P(void);
// frame rate
extern void set_sensor_cur_frame_rate_GC2053P(SENSOR_ID id, SENSOR_MODE mode, UINT32 frame_rate);
extern UINT32 get_sensor_cur_frame_rate_GC2053P(SENSOR_ID id, SENSOR_MODE mode);
extern void set_sensor_chgmode_frame_rate_GC2053P(SENSOR_ID id, SENSOR_MODE mode, UINT32 frame_rate);
extern UINT32 get_sensor_chgmode_frame_rate_GC2053P(SENSOR_ID id, SENSOR_MODE mode);
#endif
