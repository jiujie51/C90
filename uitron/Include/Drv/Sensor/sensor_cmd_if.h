/**
    @file       sensor_i2c.h

    @ingroup    mIDrvSensor
    @brief      Define sensor internal function
    @note      Sensor Driver APIs

    Copyright   Novatek Microelectronics Corp. 2010.  All rights reserved.
*/
#ifndef _SENSOR_I2C_H
#define _SENSOR_I2C_H


#include "Type.h"
#include "i2c.h"
#include "sif.h"
#include "top.h"
#include "sensor_if.h"
#include "Delay.h"


/**
    I2C Configuration Item
*/
typedef enum {
	SENSOR_I2C_CONFIG_PINMX = 0,          ///< I2C config pimux
	SENSOR_I2C_CONFIG_CLOCK = 1,          ///< I2C config clock
	SENSOR_I2C_CONFIG_MAX_NUM,
	ENUM_DUMMY4WORD(SENSOR_I2C_CONFIG)
} SENSOR_I2C_CONFIG;

/**
    I2C sensor data format
*/
typedef enum {
	I2CFMT_1B1B,
	I2CFMT_1B2B,
	I2CFMT_2B1B,
	I2CFMT_2B2B,
	ENUM_DUMMY4WORD(I2CFMT)
} I2CFMT;

/**
    sensor I2C table
*/
typedef struct {
	ER(*open)(SENSOR_ID id);
	ER(*close)(SENSOR_ID id);
	BOOL (*is_opened)(SENSOR_ID id);

	void (*set_config)(SENSOR_ID id, SENSOR_I2C_CONFIG config_id, UINT32 config_value);
	UINT32(*get_config)(SENSOR_ID id, SENSOR_I2C_CONFIG config_id);

	ER(*write)(SENSOR_ID id, UINT32 reg_addr, UINT32 value, I2CFMT i2c_fmt);
	ER(*read)(SENSOR_ID id, UINT32 reg_addr, UINT32 *value, I2CFMT i2c_fmt);

	ER(*write_dma)(SENSOR_ID id, UINT32 reg_buf_addr, UINT32 cmd_cnt, UINT32 sync_vd_num, I2CFMT i2c_fmt);

	ER(*write_with_slvaddr)(SENSOR_ID id, UINT32 reg_addr, UINT32 value, I2CFMT i2c_fmt_input, UINT8 slvaddr);
	ER(*read_with_slvaddr)(SENSOR_ID id, UINT32 reg_addr, UINT32 *value, I2CFMT i2c_fmt_input, UINT8 slvaddr);
} SENSOR_CMD_I2C_OBJ;

/**
    sensor SIF table
*/
typedef struct {
	ER(*open)(SENSOR_ID id);
	ER(*close)(SENSOR_ID id);
	BOOL (*is_opened)(SENSOR_ID id);

	void (*set_config)(SENSOR_ID id, SIF_CONFIG_ID config_id, UINT32 ui_config);
	UINT32(*get_config)(SENSOR_ID id, SIF_CONFIG_ID config_id);

	void(*write)(SENSOR_ID id, UINT32 ui_data_0, UINT32 ui_data_1, UINT32 ui_data_2);

	// data write when dma enable, data_addr need to keep buffer until sif write
	void(*write_dma_latch)(SENSOR_ID id, UINT32 data_addr, UINT32 ui_data_len);

	// data write when SIE vd signal, data_addr need to keep buffer until sif write
	void(*write_vd_latch)(SENSOR_ID id, UINT32 data_addr, UINT32 data_num, UINT32 ui_data0_len, UINT32 ui_data1_len, UINT32 ui_data2_len);

} SENSOR_CMD_SIF_OBJ;

/**
    sensor TX/RX table
*/

/**
    TX Module sel
*/
typedef enum {
	SEN_TXMODULE_THCV231 = 1,
	ENUM_DUMMY4WORD(SEN_TXMODULE_SEL)
} SEN_TXMODULE_SEL;

/**
    RX Module sel
*/
typedef enum {
	SEN_RXMODULE_THCV236 = 1,
	ENUM_DUMMY4WORD(SEN_RXMODULE_SEL)
} SEN_RXMODULE_SEL;

/**
    TX/RX Configuration Item
*/
typedef enum {
	SEN_TXRX_CONFIG_POL_DLY_US = 0,          ///< interval of each polling
	SEN_TXRX_CONFIG_POL_TIMEOUT_CNT,         ///< polling times
	SEN_TXRX_CONFIG_MAX_NUM,
	ENUM_DUMMY4WORD(SEN_TXRX_CONFIG)
} SEN_TXRX_CONFIG;

/**
    debug item
*/
typedef enum {
	SEN_TXRX_DBG_DUMPINIT,	// dump tx/rx status
	ENUM_DUMMY4WORD(SEN_TXRX_DBG_ITEM)
} SEN_TXRX_DBG_ITEM;


typedef struct {
	ER(*init)(SENSOR_ID id, SEN_TXMODULE_SEL tx_module, SEN_RXMODULE_SEL rx_module); // need to init first
	ER(*uninit)(SENSOR_ID id);

	BOOL(*det_plug_in)(SENSOR_ID id, I2CFMT i2c_fmt_input);

	void (*set_config)(SENSOR_ID id, SEN_TXRX_CONFIG config_id, UINT32 ui_config);
	UINT32(*get_config)(SENSOR_ID id, SEN_TXRX_CONFIG config_id);

	void(*dump_debug_info)(SENSOR_ID id, SEN_TXRX_DBG_ITEM item);

} SENSOR_CMD_TXRX_OBJ;

/**
    extern API
*/
extern SENSOR_CMD_I2C_OBJ *sensor_cmd_get_i2c_ctrl_obj(void);
extern SENSOR_CMD_SIF_OBJ *sensor_cmd_get_sif_ctrl_obj(void);
extern SENSOR_CMD_TXRX_OBJ *sensor_cmd_get_txrx_ctrl_obj(void);

// i2c setting
extern void sensor_i2c_set_timeout(SENSOR_I2C i2c_bus, UINT32 timeout); // Setting I2C driver transmit/receive timeout value, must set before sensor_cmd_get_i2c_ctrl_obj()->(open)(SENSOR_ID id)
extern void sensor_i2c_set_rst(SENSOR_I2C i2c_bus, BOOL en); // set en==1: if trnasmit or receive fail, i2c wiil reset module and send 9 clk (in unlock)
extern void sensor_i2c_set_scl_timeout_nochk(SENSOR_I2C i2c_bus, BOOL nochk); // set nochk==1: disable scl timeout interrupt, must set before sensor_cmd_get_i2c_ctrl_obj()->(open)(SENSOR_ID id)

#endif
