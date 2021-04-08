#ifndef _LT8918_INT_H
#define _LT8918_INT_H

#include "i2c.h"

#define CMD_DELAY		0xffffffff

#define LT8918_PATTERN_MODE 0
#define LT8918_NORMAL_MODE 1
#define LT8918_MODE LT8918_NORMAL_MODE

typedef struct
{
	I2C_SESSION		I2C_Channel;
	UINT32			I2C_RegBytes;
	I2C_PINMUX		I2C_PinMux;
	I2C_BUS_CLOCK	I2C_BusClock;
	UINT32			I2C_Slave_WAddr;
	UINT32			I2C_Slave_RAddr;
} LT8918_I2C_INFO, *PLT8918_I2C_INFO;

typedef struct {
	UINT32 addr;		///< address
	UINT32 data;		///< data
} LT8918_CMD;

extern UINT32 LT8918_GetInitTbl(LT8918_CMD **cmd);
extern ER LT8918_WriteReg(UINT32 addr, UINT32 data);
extern ER LT8918_ReadReg(UINT32 addr, UINT32* data);
extern I2C_STS LT8918_I2C_Transmit(UINT32 i2c_w_addr, UINT32 reg_addr, UINT32 value);
extern I2C_STS LT8918_I2C_Receive(UINT32 i2c_w_addr, UINT32 i2c_r_addr, UINT32 reg_addr, UINT32 *value);
#if (LT8918_MODE == LT8918_PATTERN_MODE)
extern ER LT8918_Init(void);
extern void LT8918_Pattern_cycle_test(void);
#else
extern ER LT8918_Init(void);
extern void vdLT8918L_Normal_Mode(void);
#endif
#endif
