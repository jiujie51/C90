
#ifndef _GSENSOR_DMA380_H
#define _GSENSOR_DMA380_H
#include "Type.h"
#include "IOInit.h"
#include "pad.h"
#include "top.h"
#include "gpio.h"
#include "adc.h"
#include "pwm.h"

#if 0
#define STATUS_REG          0x00
#define OUT_X_MSB           0x01
#define OUT_X_LSB           0x02
#define OUT_Y_MSB           0x03
#define OUT_Y_LSB           0x04
#define OUT_Z_MSB           0x05
#define OUT_Z_LSB           0x06
#define SYSMOD_REG          0x0B
#define INT_SOURCE          0x0C
#define WHO_AM_I            0x0D
#define TRANSIENT_SRC       0x1E
#define OUT_X_REG           0x41
#define OUT_Y_REG           0x42
#define OUT_Z_REG           0x43
#define CTRL_REG1           0x44
#define CTRL_REG2           0x45
#define CTRL_REG3           0x46
#define CTRL_REG4           0x47
#define CTRL_REG5           0x48
#define XYZOR_REG           0x49
#define CTRL_REG6           0x4A
#define CTRL_REG7           0x4C
#define CTRL_REG8           0x4D

#define SW_RST              0x53

#define DRAMD07_WRITE_ID    0x38
#define DRAMD07_READ_ID     0x39
#define DRAMD07_DRV_ID      0x07
#else
#define DA380_READ_ID      0x4f
#define DA380_WRITE_ID    0x4e

#define NSA_REG_SPI_I2C                 0x00
#define NSA_REG_WHO_AM_I                0x01
#define NSA_REG_ACC_X_LSB               0x02
#define NSA_REG_ACC_X_MSB               0x03
#define NSA_REG_ACC_Y_LSB               0x04
#define NSA_REG_ACC_Y_MSB               0x05
#define NSA_REG_ACC_Z_LSB               0x06
#define NSA_REG_ACC_Z_MSB               0x07 
#define NSA_REG_G_RANGE                 0x0f
#define NSA_REG_ODR_AXIS_DISABLE        0x10
#define NSA_REG_POWERMODE_BW            0x11
#define NSA_REG_SWAP_POLARITY           0x12
#define NSA_REG_FIFO_CTRL               0x14
#define NSA_REG_INTERRUPT_SETTINGS1     0x16
#define NSA_REG_INTERRUPT_SETTINGS2     0x17
#define NSA_REG_INTERRUPT_MAPPING1      0x19
#define NSA_REG_INTERRUPT_MAPPING2      0x1a
#define NSA_REG_INTERRUPT_MAPPING3      0x1b
#define NSA_REG_INT_PIN_CONFIG          0x20
#define NSA_REG_INT_LATCH               0x21
#define NSA_REG_ACTIVE_DURATION         0x27
#define NSA_REG_ACTIVE_THRESHOLD        0x28
#define NSA_REG_TAP_DURATION            0x2A
#define NSA_REG_TAP_THRESHOLD           0x2B
#define NSA_REG_CUSTOM_OFFSET_X         0x38
#define NSA_REG_CUSTOM_OFFSET_Y         0x39
#define NSA_REG_CUSTOM_OFFSET_Z         0x3a
#define NSA_REG_ENGINEERING_MODE        0x7f
#define NSA_REG_SENSITIVITY_TRIM_X      0x80
#define NSA_REG_SENSITIVITY_TRIM_Y      0x81
#define NSA_REG_SENSITIVITY_TRIM_Z      0x82
#define NSA_REG_COARSE_OFFSET_TRIM_X    0x83
#define NSA_REG_COARSE_OFFSET_TRIM_Y    0x84
#define NSA_REG_COARSE_OFFSET_TRIM_Z    0x85
#define NSA_REG_FINE_OFFSET_TRIM_X      0x86
#define NSA_REG_FINE_OFFSET_TRIM_Y      0x87
#define NSA_REG_FINE_OFFSET_TRIM_Z      0x88
#define NSA_REG_SENS_COMP               0x8c
#define NSA_REG_SENS_COARSE_TRIM        0xd1
extern BOOL GSensor_DA380_open(void);
extern BOOL GSensor_DA380_close(void);
extern BOOL GSensor_DA380_GetStatus(Gsensor_Data *GS_Data);
extern BOOL GSensor_DA380_ParkingMode(void);
extern BOOL GSensor_DA380_CrashMode(void);
extern void GSensor_DA380_SetSensitivity(GSENSOR_SENSITIVITY GSensorSensitivity);
extern 	void  GSensor_DA380_Get3AxisData(INT32 *pX, INT32 *pY, INT32 *pZ);
extern 	void  GSensor_DA380_ClearIntSts(void);



extern UINT DA380_read_data(short *x, short *y, short *z);

#endif
#endif

