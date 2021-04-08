/**
 * Novatek PANASONIC AN41908 motor driver.
 * 
 * Reference motor driver implementation.
 * 
 * @file LensMotor_AN41908.c
 * @note Nothing.
 *
 * Copyright (C) 2017 Novatek Microelectronics Corp. All rights reserved.
 *
 */

#include "gpio.h"
#include "spi.h"
#include "LensMotor_AN41908.h"


// prototype declaration
static void   motorAN41908_Init(PLENS_DEVICE_OBJ plens_dev);
static UINT32 motorAN41908_GetSignal(MD_SIGNAL signal);
static void   motorAN41908_SetSignal(MD_SIGNAL signal, UINT32 on);
static UINT32 motorAN41908_GetInitState(void);
static UINT32 motorAN41908_GetBusyState(void);
static UINT32 motorAN41908_ZoomGetSpeed(void);
static void   motorAN41908_ZoomSetState(MOTOR_ZOOM_ACT state, UINT32 param);
static UINT32 motorAN41908_FocusGetSpeed(void);
static BOOL   motorAN41908_FocusSetState(MOTOR_FOCUS_ACT state, UINT32 param);
static void   motorAN41908_FocusZoomMove(BOOL far_dir, INT32 fsteps, INT32 fbacklash, BOOL tele_dir, INT32 zsteps, INT32 zbacklash);
static void   motorAN41908_ApertureSetState(MOTOR_APERTURE_ACT state, UINT32 param);
static void   motorAN41908_ShutterSetState(MOTOR_SHUTTER_ACT state, UINT32 param);
static void   motorAN41908_IrcutSetState(MOTOR_IRCUT_ACT state, UINT32 param);


//----------------------------------------------------------------------------------------------------------------------
// general
//----------------------------------------------------------------------------------------------------------------------
LENSMOTOR_INFO* AN41908_GetMotorInfo(void)
{
    return &g_LensMotorInfo;
}


PMOTOR_TAB DrvLens_GetMotor(void)
{
    DBG_MSG("%s\r\n", __func__);

    g_LensMotorInfo.LensMotor.init               = motorAN41908_Init;
    g_LensMotorInfo.LensMotor.getSignal          = motorAN41908_GetSignal;
    g_LensMotorInfo.LensMotor.setSignal          = motorAN41908_SetSignal;
    g_LensMotorInfo.LensMotor.getInitState       = motorAN41908_GetInitState;
    g_LensMotorInfo.LensMotor.getBusyState       = motorAN41908_GetBusyState;
    g_LensMotorInfo.LensMotor.zoom_getSpeed      = motorAN41908_ZoomGetSpeed;
    g_LensMotorInfo.LensMotor.zoom_setState      = motorAN41908_ZoomSetState;
    g_LensMotorInfo.LensMotor.focus_getSpeed     = motorAN41908_FocusGetSpeed;
    g_LensMotorInfo.LensMotor.focus_setState     = motorAN41908_FocusSetState;
    g_LensMotorInfo.LensMotor.focus_zoom_setMove = motorAN41908_FocusZoomMove;
    g_LensMotorInfo.LensMotor.aperture_setState  = motorAN41908_ApertureSetState;
    g_LensMotorInfo.LensMotor.shutter_setState   = motorAN41908_ShutterSetState;
    g_LensMotorInfo.LensMotor.ircut_setState     = motorAN41908_IrcutSetState;

    return &g_LensMotorInfo.LensMotor;
}


static INT32 AN41908_InitSPI(void)
{
    spi_setConfig(MOTOR_AN41908_SPI_CH, SPI_CONFIG_ID_BUSMODE, SPI_MODE_3);
    spi_setConfig(MOTOR_AN41908_SPI_CH, SPI_CONFIG_ID_FREQ, 10000000);    // bus clock = 4 MHz (1~5 MHz)
    spi_setConfig(MOTOR_AN41908_SPI_CH, SPI_CONFIG_ID_MSB_LSB, SPI_LSB);
    spi_setConfig(MOTOR_AN41908_SPI_CH, SPI_CONFIG_ID_CS_ACT_LEVEL, SPI_CS_ACT_LEVEL_HIGH);
    spi_setConfig(MOTOR_AN41908_SPI_CH, SPI_CONFIG_ID_CS_CK_DLY, 2);      // 2 usec

    if (spi_open(MOTOR_AN41908_SPI_CH) != E_OK) {
        return -1;
    }

    spi_setTransferLen(MOTOR_AN41908_SPI_CH, SPI_TRANSFER_LEN_1BYTE);    // 1 byte mode in 1 packet

    return 0;
}


static void AN41908_Write2SPI(UINT32 spi_cmd1, UINT32 spi_cmd2, UINT32 spi_cmd3, UINT32 spi_cmd4)
{
    UINT32 curr_stamp, time_diff;
    static UINT32 last_stamp = 0;

    do {
        curr_stamp = Perf_GetCurrent();
        time_diff = (curr_stamp >= last_stamp) ? (curr_stamp - last_stamp) : ((4294967295UL - last_stamp) + curr_stamp);
        LENS_DELAY_US(100);
    } while (time_diff < 1200);    // separate 2 SPI cmds depending on 0x20[7:0] + 0x22[7:0]

    loc_cpu();

    gpio_setPin(g_LensMotorInfo.uiPinIn[LENS_IO_IN_2]);
    gpio_clearPin(g_LensMotorInfo.uiPinIn[LENS_IO_IN_2]);

    spi_writePacket(MOTOR_AN41908_SPI_CH, 3, (spi_cmd1 & 0x00FFFF3F));

    if (spi_cmd2 != 0) {
        spi_writePacket(MOTOR_AN41908_SPI_CH, 3, (spi_cmd2 & 0x00FFFF3F));
    }

    if (spi_cmd3 != 0) {
        spi_writePacket(MOTOR_AN41908_SPI_CH, 3, (spi_cmd3 & 0x00FFFF3F));
    }

    if (spi_cmd4 != 0) {
        spi_writePacket(MOTOR_AN41908_SPI_CH, 3, (spi_cmd4 & 0x00FFFF3F));
    }

    unl_cpu();

    last_stamp = Perf_GetCurrent();
}


static UINT32 AN41908_Read4SPI(UINT8 addr)
{
    UINT32 rd_data[2] = { 0 };

    spi_setCSActive(MOTOR_AN41908_SPI_CH, TRUE);

    spi_setConfig(MOTOR_AN41908_SPI_CH, SPI_CONFIG_ID_ENG_PKT_COUNT, 1);      // send 1 packet
    spi_writeSingle(MOTOR_AN41908_SPI_CH, (UINT32)((addr & 0x3F) | 0x40));    // transmit 8-bit address

    spi_readSingle(MOTOR_AN41908_SPI_CH, &rd_data[0]);                        // receive 16-bit data
    spi_readSingle(MOTOR_AN41908_SPI_CH, &rd_data[1]);

    spi_setCSActive(MOTOR_AN41908_SPI_CH, FALSE);

    return ((rd_data[1] << 8) & 0xFF00) | (rd_data[0] & 0xFF);
}


INT32 AN41908_TransmitCmd(AN41908_CHNEL_NUM chnel, AN41908_CMD_TYPE cmd_type, INT32 cmd_arg1, INT32 cmd_arg2)
{
    INT32 cmd_status = 0, cmd_idx, reg_tmp;
    UINT32 i, spi_data[4] = { 0 }, wr_idx[4] = { 0 };
    UINT32 *pwr_reg;
    MOTOR_CONFIG *pmotor_cfg;

    pmotor_cfg = (g_LensMotorInfo.LensMotorConfig[0].eChnelSel == chnel) ? &g_LensMotorInfo.LensMotorConfig[0]
              : ((g_LensMotorInfo.LensMotorConfig[1].eChnelSel == chnel) ? &g_LensMotorInfo.LensMotorConfig[1] : NULL);

    if (pmotor_cfg == NULL) {
        return -1;
    }

    pwr_reg = (UINT32 *)&g_LensMotorInfo.LensMotorReg.REG[0];

    switch (cmd_type) {
    case AN41908_SET_MOTOR_CFG:
        cmd_idx = cmd_arg1 & AN41908_REG_UNKNOWN;

        if ((cmd_idx >= AN41908_PULSE1_START_TIME) && (cmd_idx <= AN41908_IRCUT_CTL)) {
            spi_data[0] = (cmd_arg1 & 0x00FFFF00) + cmd_idx;
            spi_data[1] = cmd_arg2 & (0x00FFFF00 | AN41908_REG_UNKNOWN);
        } else {
            cmd_status = -1;
        }

        for (i = 0; i < LENS_ARRAY_SIZE(g_ucRegAddressOfst); i++) {
            if (cmd_idx == g_ucRegAddressOfst[i]) {
                wr_idx[0] = i;
                break;
            }
        }
        break;

    case AN41908_SET_MOTOR_SPD:
        spi_data[0] = AN41908_PULSE_FREQUENCY;
        spi_data[0] += ((cmd_arg1 & 0xFFFF) << 8);
        break;

    case AN41908_SET_MOTOR_STEP:
        if (pmotor_cfg->eChnelSel == AN41908_CHNEL1) {
            spi_data[0] = AN41908_PULSE1_MOVE_STEP;
            spi_data[1] = (cmd_arg1 != 0) ? (((UINT32)pmotor_cfg->wMaxPulseWidth << 8) + AN41908_PULSE1_MAX_DUTY_WIDTH)
                                          : ((STANDBY_PWR_PULSE_WIDTH << 8) + AN41908_PULSE1_MAX_DUTY_WIDTH);
        } else if (pmotor_cfg->eChnelSel == AN41908_CHNEL3) {
            spi_data[0] = AN41908_PULSE2_MOVE_STEP;
            spi_data[1] = (cmd_arg1 != 0) ? (((UINT32)pmotor_cfg->wMaxPulseWidth << 8) + AN41908_PULSE2_MAX_DUTY_WIDTH)
                                          : ((STANDBY_PWR_PULSE_WIDTH << 8) + AN41908_PULSE2_MAX_DUTY_WIDTH);
        } else {
            cmd_status = -1;
        }

        if (pmotor_cfg->eMicroStepMode == AN41908_MODE_64MACRO) {
            reg_tmp = 0x3000;
        } else if (pmotor_cfg->eMicroStepMode == AN41908_MODE_128MACRO) {
            reg_tmp = 0x2000;
        } else if (pmotor_cfg->eMicroStepMode == AN41908_MODE_256MACRO) {
            reg_tmp = 0x1000;
        } else {
            reg_tmp = 0;
            cmd_status = -1;
        }

        if (pmotor_cfg->iDefPositiveDir == 0) {
            spi_data[0] += (cmd_arg1 >= 0) ? (((0x0100 | pmotor_cfg->iExciteEnable | reg_tmp) | (cmd_arg1 & 0x00FF)) << 8)
                                           : (((pmotor_cfg->iExciteEnable | reg_tmp) | (LENS_ABS(cmd_arg1) & 0x00FF)) << 8);
        } else {
            spi_data[0] += (cmd_arg1 >= 0) ? (((pmotor_cfg->iExciteEnable | reg_tmp) | (cmd_arg1 & 0x00FF)) << 8)
                                           : (((0x0100 | pmotor_cfg->iExciteEnable | reg_tmp) | (LENS_ABS(cmd_arg1) & 0x00FF)) << 8);
        }
        break;

    case AN41908_SET_MOTOR_MOVE:
        if (chnel == AN41908_CHNEL1) {
            spi_data[0] = AN41908_PULSE1_MOVE_STEP;
            spi_data[1] = (cmd_arg1 != 0) ? (((UINT32)g_LensMotorInfo.LensMotorConfig[0].wMaxPulseWidth << 8) + AN41908_PULSE1_MAX_DUTY_WIDTH)
                                          : ((STANDBY_PWR_PULSE_WIDTH << 8) + AN41908_PULSE1_MAX_DUTY_WIDTH);
            spi_data[2] = AN41908_PULSE2_MOVE_STEP;
            spi_data[3] = (cmd_arg2 != 0) ? (((UINT32)g_LensMotorInfo.LensMotorConfig[1].wMaxPulseWidth << 8) + AN41908_PULSE2_MAX_DUTY_WIDTH)
                                          : ((STANDBY_PWR_PULSE_WIDTH << 8) + AN41908_PULSE2_MAX_DUTY_WIDTH);
        } else {
            spi_data[0] = AN41908_PULSE2_MOVE_STEP;
            spi_data[1] = (cmd_arg1 != 0) ? (((UINT32)g_LensMotorInfo.LensMotorConfig[0].wMaxPulseWidth << 8) + AN41908_PULSE2_MAX_DUTY_WIDTH)
                                          : ((STANDBY_PWR_PULSE_WIDTH << 8) + AN41908_PULSE2_MAX_DUTY_WIDTH);
            spi_data[2] = AN41908_PULSE1_MOVE_STEP;
            spi_data[3] = (cmd_arg2 != 0) ? (((UINT32)g_LensMotorInfo.LensMotorConfig[1].wMaxPulseWidth << 8) + AN41908_PULSE1_MAX_DUTY_WIDTH)
                                          : ((STANDBY_PWR_PULSE_WIDTH << 8) + AN41908_PULSE1_MAX_DUTY_WIDTH);
        }

        if (pmotor_cfg->eMicroStepMode == AN41908_MODE_64MACRO) {
            reg_tmp = 0x3000;
        } else if (pmotor_cfg->eMicroStepMode == AN41908_MODE_128MACRO) {
            reg_tmp = 0x2000;
        } else if (pmotor_cfg->eMicroStepMode == AN41908_MODE_256MACRO) {
            reg_tmp = 0x1000;
        } else {
            reg_tmp = 0;
            cmd_status = -1;
        }

        if (g_LensMotorInfo.LensMotorConfig[0].iDefPositiveDir == 0) {
            spi_data[0] += (cmd_arg1 >= 0) ? (((0x0100 | g_LensMotorInfo.LensMotorConfig[0].iExciteEnable | reg_tmp) | (cmd_arg1 & 0x00FF)) << 8)
                                           : (((g_LensMotorInfo.LensMotorConfig[0].iExciteEnable | reg_tmp) | (LENS_ABS(cmd_arg1) & 0x00FF)) << 8);
        } else {
            spi_data[0] += (cmd_arg1 >= 0) ? (((g_LensMotorInfo.LensMotorConfig[0].iExciteEnable | reg_tmp) | (cmd_arg1 & 0x00FF)) << 8)
                                           : (((0x0100 | g_LensMotorInfo.LensMotorConfig[0].iExciteEnable | reg_tmp) | (LENS_ABS(cmd_arg1) & 0x00FF)) << 8);
        }
        if (g_LensMotorInfo.LensMotorConfig[1].iDefPositiveDir == 0) {
            spi_data[2] += (cmd_arg2 >= 0) ? (((0x0100 | g_LensMotorInfo.LensMotorConfig[1].iExciteEnable | reg_tmp) | (cmd_arg2 & 0x00FF)) << 8)
                                           : (((g_LensMotorInfo.LensMotorConfig[1].iExciteEnable | reg_tmp) | (LENS_ABS(cmd_arg2) & 0x00FF)) << 8);
        } else {
            spi_data[2] += (cmd_arg2 >= 0) ? (((g_LensMotorInfo.LensMotorConfig[1].iExciteEnable | reg_tmp) | (cmd_arg2 & 0x00FF)) << 8)
                                           : (((0x0100 | g_LensMotorInfo.LensMotorConfig[1].iExciteEnable | reg_tmp) | (LENS_ABS(cmd_arg2) & 0x00FF)) << 8);
        }
        break;

    default:
        cmd_status = -1;
        break;
    }

    wr_idx[0] = LENS_MIN(wr_idx[0], LENS_ARRAY_SIZE(g_ucRegAddressOfst)-1);
    wr_idx[1] = LENS_MIN(wr_idx[1], LENS_ARRAY_SIZE(g_ucRegAddressOfst)-1);
    wr_idx[2] = LENS_MIN(wr_idx[2], LENS_ARRAY_SIZE(g_ucRegAddressOfst)-1);
    wr_idx[3] = LENS_MIN(wr_idx[3], LENS_ARRAY_SIZE(g_ucRegAddressOfst)-1);

    if (cmd_status != -1) {
        AN41908_Write2SPI(spi_data[0], spi_data[1], spi_data[2], spi_data[3]);

        pwr_reg[wr_idx[0]] = spi_data[0];

        if (cmd_type != AN41908_SET_MOTOR_CFG) {
            if (spi_data[1] != 0) {
                pwr_reg[wr_idx[1]] = spi_data[1];
            }
            if (spi_data[2] != 0) {
                pwr_reg[wr_idx[2]] = spi_data[2];
            }
            if (spi_data[3] != 0) {
                pwr_reg[wr_idx[3]] = spi_data[3];
            }
        }
    } else {
        DBG_MSG("%s err!\r\n", __func__);
    }

    return cmd_status;
}


static UINT16 AN41908_ReceiveCmd(AN41908_CHNEL_NUM chnel, AN41908_CMD_TYPE cmd_type, INT32 cmd_arg)
{
    UINT8 i, rd_idx = 0;
    INT32 cmd_status = 0;
    UINT32 *prd_reg;
    MOTOR_CONFIG *pmotor_cfg;

    pmotor_cfg = (g_LensMotorInfo.LensMotorConfig[0].eChnelSel == chnel) ? &g_LensMotorInfo.LensMotorConfig[0]
              : ((g_LensMotorInfo.LensMotorConfig[1].eChnelSel == chnel) ? &g_LensMotorInfo.LensMotorConfig[1] : NULL);

    if (pmotor_cfg == NULL) {
        return 0xFFFF;
    }

    prd_reg = (UINT32 *)&g_LensMotorInfo.LensMotorReg.REG[0];

    switch (cmd_type) {
    case AN41908_SET_MOTOR_CFG:
        rd_idx = (UINT8)(cmd_arg & AN41908_REG_UNKNOWN);

        for (i = 0; i < LENS_ARRAY_SIZE(g_ucRegAddressOfst); i++) {
            if (rd_idx == g_ucRegAddressOfst[i]) {
                rd_idx = i;
                break;
            }
        }

        cmd_status = (i >= LENS_ARRAY_SIZE(g_ucRegAddressOfst)) ? -1 : 0;
        break;

    case AN41908_SET_MOTOR_SPD:
        rd_idx = 14;    // AN41908_PULSE_FREQUENCY
        break;

    case AN41908_SET_MOTOR_STEP:
    case AN41908_SET_MOTOR_MOVE:
        if (pmotor_cfg->eChnelSel == AN41908_CHNEL1) {
            rd_idx = 18;    // AN41908_PULSE1_MOVE_STEP;
        } else if (pmotor_cfg->eChnelSel == AN41908_CHNEL3) {
            rd_idx = 22;    // AN41908_PULSE2_MOVE_STEP;
        } else {
            cmd_status = -1;
        }
        break;

    default:
        cmd_status = -1;
        break;
    }

    rd_idx = LENS_MIN(rd_idx, LENS_ARRAY_SIZE(g_ucRegAddressOfst)-1);

    if (cmd_status != -1) {
        return (UINT16)prd_reg[rd_idx];
    } else {
        DBG_MSG("%s err!\r\n", __func__);

        return 0xFFFF;
    }
}


#if FZ_PLS_PIN_EXIST
static void AN41908_GpioIsr(UINT32 gpio_pin)
{
#if ISR_RISING_EDGE_TRIGGER
    if (gpio_getIntStatus(g_LensMotorInfo.uiPinIn[LENS_IO_FOCUS_PIINT])) {
#else
    if (!gpio_getIntStatus(g_LensMotorInfo.uiPinIn[LENS_IO_FOCUS_PIINT])) {
#endif
        g_LensMotorInfo.LensMotorConfig[0].iMotorStatus = MOTOR_STATUS_FREE;

        gpio_clearIntStatus(g_LensMotorInfo.uiPinIn[LENS_IO_FOCUS_PIINT]);
        //DBG_DUMP("focus GPIO ISR\r\n");
#if ISR_RISING_EDGE_TRIGGER
    } else if (gpio_getIntStatus(g_LensMotorInfo.uiPinIn[LENS_IO_ZOOM_PIINT])) {
#else
    } else if (!gpio_getIntStatus(g_LensMotorInfo.uiPinIn[LENS_IO_ZOOM_PIINT])) {
#endif
        g_LensMotorInfo.LensMotorConfig[1].iMotorStatus = MOTOR_STATUS_FREE;

        gpio_clearIntStatus(g_LensMotorInfo.uiPinIn[LENS_IO_ZOOM_PIINT]);
        //DBG_DUMP("zoom GPIO ISR\r\n");
    }
}
#endif


static INT32 AN41908_WaitMoveEnd(MOTOR_SEL motor_sel, const UINT32 timeout_ms)
{
#if FZ_PLS_PIN_EXIST
    UINT32 cnt = 0;
    MOTOR_CONFIG *pmotor_cfg;

    pmotor_cfg = (motor_sel == MOTOR_FOCUS) ? &g_LensMotorInfo.LensMotorConfig[0] : &g_LensMotorInfo.LensMotorConfig[1];

    while (pmotor_cfg->iMotorStatus != MOTOR_STATUS_FREE) {
        if (cnt++ >= timeout_ms) {
            DBG_ERR("%s timeout pin(%u)!\r\n", __func__, motor_sel);

            return -1;
        }

        LENS_DELAY_MS(1);
    }
#endif

    return 0;
}


static UINT32 AN41908_GlobalFreq(UINT32 freq)
{
    UINT32 i, output_freq;

    output_freq = LENS_CLAMP(freq, g_uiPwmFreqTbl[0][0], g_uiPwmFreqTbl[0][MAX_PWM_TAB_SIZE-1]);    // unit: Hz

    for (i = 0; i < MAX_PWM_TAB_SIZE; i++) {
        if (output_freq <= g_uiPwmFreqTbl[0][i]) {
            break;
        }
    }

    i = LENS_MIN(i, MAX_PWM_TAB_SIZE-1);

    return g_uiPwmFreqTbl[1][i];
}


static UINT32 AN41908_Speed2Frequency(AN41908_MOTOR_SPEED motor_speed)
{
    UINT32 ret_reg, rotate_freq;

    rotate_freq = (g_LensMotorInfo.LensMotorConfig[0].uiPwmFreq + 240/2) / 240;    // 240 is our max pulse number

    switch (motor_speed) {
    case AN41908_MOTOR_SPD_4X:
        rotate_freq *= 4;
        break;

    case AN41908_MOTOR_SPD_2X:
        rotate_freq *= 2;
        break;

    case AN41908_MOTOR_SPD_1X:
    default:
        rotate_freq *= 1;
        break;

    case AN41908_MOTOR_SPD_HALF:
        rotate_freq /= 2;
        break;

    case AN41908_MOTOR_SPD_QUARTER:
        rotate_freq /= 4;
        break;
    }

    ret_reg = (27000000 / rotate_freq + 240 * 24 / 2) / (240 * 24);

    return ret_reg;
}


static INT32 AN41908_MotorInit(MOTOR_SEL motor_sel)
{
    UINT16 rd_reg, reg_tmp;
    UINT32 wt_reg;
    MOTOR_CONFIG *pmotor_cfg;

    pmotor_cfg = (motor_sel == MOTOR_FOCUS) ? &g_LensMotorInfo.LensMotorConfig[0] :
                ((motor_sel == MOTOR_ZOOM)  ? &g_LensMotorInfo.LensMotorConfig[1] : NULL);

    if ((motor_sel >= MOTOR_IRIS) || (pmotor_cfg == NULL)) {
        DBG_MSG("%s unknown motor channel!\r\n", __func__);

        return -1;
    }

    // update global PWM frequency
    wt_reg = AN41908_GlobalFreq(10*pmotor_cfg->uiPwmFreq);
    reg_tmp = (wt_reg >> 8) & 0x001F;    // save out PWMMOD value

    if ((rd_reg = AN41908_ReceiveCmd(pmotor_cfg->eChnelSel, AN41908_SET_MOTOR_SPD, 0)) == 0xFFFF) {
        DBG_MSG("motor IC may have no existence!\r\n");

        return -1;
    }

    wt_reg |= (rd_reg & 0x00FF);
    AN41908_TransmitCmd(pmotor_cfg->eChnelSel, AN41908_SET_MOTOR_SPD, wt_reg, 0);

    pmotor_cfg->wMaxPulseWidth = 11 * 8 * reg_tmp / 10;    // set 110% peak pulse width
    pmotor_cfg->wMaxPulseWidth = LENS_CLAMP(pmotor_cfg->wMaxPulseWidth, 0x10, 0xFF);
    pmotor_cfg->wMaxPulseWidth |= ((pmotor_cfg->wMaxPulseWidth << 8) & 0xFF00);

    // update respective PWM frequency for achieving desired moving speed
    wt_reg = AN41908_Speed2Frequency(pmotor_cfg->eMoveSpeed) & 0xFFFF;

    if (pmotor_cfg->eChnelSel == AN41908_CHNEL1) {
        AN41908_TransmitCmd(pmotor_cfg->eChnelSel, AN41908_SET_MOTOR_CFG, (wt_reg << 8) | AN41908_PULSE1_STEP_CYCLE, 0);
    } else if (pmotor_cfg->eChnelSel == AN41908_CHNEL3) {
        AN41908_TransmitCmd(pmotor_cfg->eChnelSel, AN41908_SET_MOTOR_CFG, (wt_reg << 8) | AN41908_PULSE2_STEP_CYCLE, 0);
    }

    return 0;
}


static INT32 AN41908_SetMotorPosition(MOTOR_SEL motor_sel, INT32 step_pos)
{
    INT32 dly_time;
    MOTOR_CONFIG *pmotor_cfg;

    pmotor_cfg = (motor_sel == MOTOR_FOCUS) ? &g_LensMotorInfo.LensMotorConfig[0] :
                ((motor_sel == MOTOR_ZOOM)  ? &g_LensMotorInfo.LensMotorConfig[1] : NULL);

    if (pmotor_cfg == NULL) {
        DBG_MSG("%s unknown motor channel!\r\n", __func__);

        return -1;
    }

    wai_sem(g_uiAn41908SpiSema);

    if (step_pos != 0) {
        pmotor_cfg->iWorkStatus = pmotor_cfg->iMotorStatus = MOTOR_STATUS_BUSY;

        AN41908_TransmitCmd(pmotor_cfg->eChnelSel, AN41908_SET_MOTOR_STEP, (INT32)pmotor_cfg->eMicroStepMode*step_pos, 0);

#if FZ_PLS_PIN_EXIST
        dly_time = pmotor_cfg->iDlyTime;
#else
        dly_time = LENS_MAX(pmotor_cfg->iDlyTime * LENS_ABS(step_pos) / pmotor_cfg->iMaxStepSize, 2);
#endif
        LENS_DELAY_MS(dly_time);

        pmotor_cfg->iMotorStatus = MOTOR_STATUS_FREE;
    } else {
        AN41908_TransmitCmd(pmotor_cfg->eChnelSel, AN41908_SET_MOTOR_STEP, 0, 0);

        pmotor_cfg->iWorkStatus = MOTOR_STATUS_FREE;
    }

    sig_sem(g_uiAn41908SpiSema);

    return step_pos;
}


static void AN41908_SetMotorMove(INT32 focus_steps, INT32 zoom_steps)
{
    INT32 dly_time[2];
    MOTOR_CONFIG *pfocus_motor_cfg, *pzoom_motor_cfg;

    pfocus_motor_cfg = &g_LensMotorInfo.LensMotorConfig[0];
    pzoom_motor_cfg = &g_LensMotorInfo.LensMotorConfig[1];

    wai_sem(g_uiAn41908SpiSema);

    if (focus_steps != 0) {
        pfocus_motor_cfg->iWorkStatus = pfocus_motor_cfg->iMotorStatus = MOTOR_STATUS_BUSY;
    } else {
        pfocus_motor_cfg->iWorkStatus = MOTOR_STATUS_FREE;
    }

    if (zoom_steps != 0) {
        pzoom_motor_cfg->iWorkStatus = pzoom_motor_cfg->iMotorStatus = MOTOR_STATUS_BUSY;
    } else {
        pzoom_motor_cfg->iWorkStatus = MOTOR_STATUS_FREE;
    }

    AN41908_TransmitCmd(pfocus_motor_cfg->eChnelSel, AN41908_SET_MOTOR_MOVE, (INT32)pfocus_motor_cfg->eMicroStepMode*focus_steps, (INT32)pzoom_motor_cfg->eMicroStepMode*zoom_steps);

    dly_time[0] = LENS_MAX(pfocus_motor_cfg->iDlyTime * LENS_ABS(focus_steps) / pfocus_motor_cfg->iMaxStepSize, 2);
    dly_time[1] = LENS_MAX(pzoom_motor_cfg->iDlyTime * LENS_ABS(zoom_steps) / pzoom_motor_cfg->iMaxStepSize, 2);
    dly_time[0] = LENS_MAX(dly_time[0], dly_time[1]);
    LENS_DELAY_MS(dly_time[0]);

    if (focus_steps != 0) {
        pfocus_motor_cfg->iMotorStatus = MOTOR_STATUS_FREE;
    }

    if (zoom_steps != 0) {
        pzoom_motor_cfg->iMotorStatus = MOTOR_STATUS_FREE;
    }

    sig_sem(g_uiAn41908SpiSema);
}


static void motorAN41908_Init(PLENS_DEVICE_OBJ plens_dev)
{
    UINT32 i, init_data, rd_data;
    UINT32 *pinit_reg;

    // initialize GPIO
    for (i = 0; i < LENS_IO_MAX; i++) {
        g_LensMotorInfo.uiPinIn[i] = plens_dev->pGPIOArray[i];
    }

    if (AN41908_InitSPI() == 0) {
        // reset motor driver IC
        if (g_LensMotorInfo.uiPinIn[LENS_IO_RESET] > 0) {
            gpio_clearPin(g_LensMotorInfo.uiPinIn[LENS_IO_RESET]);
            LENS_DELAY_MS(5);
            gpio_setPin(g_LensMotorInfo.uiPinIn[LENS_IO_RESET]);
        }

#if FZ_PLS_PIN_EXIST
        // setup focus & zoom PLS GPIO interrupt
        gpio_open();

    #if ISR_RISING_EDGE_TRIGGER
        gpio_setIntTypePol(g_LensMotorInfo.uiPinIn[LENS_IO_FOCUS_PIINT], GPIO_INTTYPE_EDGE, GPIO_INTPOL_POSHIGH);
        gpio_setIntTypePol(g_LensMotorInfo.uiPinIn[LENS_IO_ZOOM_PIINT], GPIO_INTTYPE_EDGE, GPIO_INTPOL_POSHIGH);
    #else
        gpio_setIntTypePol(g_LensMotorInfo.uiPinIn[LENS_IO_FOCUS_PIINT], GPIO_INTTYPE_EDGE, GPIO_INTPOL_NEGLOW);
        gpio_setIntTypePol(g_LensMotorInfo.uiPinIn[LENS_IO_ZOOM_PIINT], GPIO_INTTYPE_EDGE, GPIO_INTPOL_NEGLOW);
    #endif

        gpio_setIntIsr(g_LensMotorInfo.uiPinIn[LENS_IO_FOCUS_PIINT], AN41908_GpioIsr);
        gpio_setIntIsr(g_LensMotorInfo.uiPinIn[LENS_IO_ZOOM_PIINT], AN41908_GpioIsr);

        gpio_clearIntStatus(g_LensMotorInfo.uiPinIn[LENS_IO_FOCUS_PIINT]);
        gpio_clearIntStatus(g_LensMotorInfo.uiPinIn[LENS_IO_ZOOM_PIINT]);
        gpio_enableInt(g_LensMotorInfo.uiPinIn[LENS_IO_FOCUS_PIINT]);
        gpio_enableInt(g_LensMotorInfo.uiPinIn[LENS_IO_ZOOM_PIINT]);

        gpio_close();
#endif

        // write initial table to AN41908 register
        pinit_reg = (UINT32 *)&g_LensMotorInfo.LensMotorReg.REG[0];

        for (i = 0; i < LENS_ARRAY_SIZE(g_ucRegAddressOfst); i++) {
            if (g_ucRegAddressOfst[i] != AN41908_IRIS_ADC_OUTPUT) {
                init_data = (pinit_reg[i] << 8) + (UINT32)g_ucRegAddressOfst[i];
                AN41908_Write2SPI(init_data, 0, 0, 0);
                LENS_DELAY_MS(2);
            }

            rd_data = AN41908_Read4SPI(g_ucRegAddressOfst[i]);

            DBG_MSG("SPI read back @ 0x%02X = 0x%04X\r\n", g_ucRegAddressOfst[i], rd_data);
        }

        AN41908_MotorInit(MOTOR_ZOOM);
        AN41908_MotorInit(MOTOR_FOCUS);
    } else {
        DBG_ERR("AN41908_InitSPI err!\r\n");
    }
}


static UINT32 motorAN41908_GetSignal(MD_SIGNAL signal)
{
    UINT32 io_status = 0;
    T_VD_FZ_POLARITY_0B reg_0x0B;
    T_PULSE1_MOVE_STEP_24 reg_0x24;
    T_PULSE2_MOVE_STEP_29 reg_0x29;

    DBG_MSG("%s signal=%u\r\n", __func__, signal);

    switch (signal) {
    case MD_SIGNAL_STANDBY:
        reg_0x0B.Reg = g_LensMotorInfo.LensMotorReg.REG[11].Reg;    // IRIS
        reg_0x24.Reg = g_LensMotorInfo.LensMotorReg.REG[18].Reg;    // focus or zoom
        reg_0x29.Reg = g_LensMotorInfo.LensMotorReg.REG[22].Reg;    // zoom or focus

        if (g_LensMotorInfo.LensMotorConfig[0].eChnelSel == AN41908_CHNEL1) {
            io_status = (reg_0x24.Bit.ENDISAB) ? (io_status | 0x01) : io_status;
            io_status = (reg_0x29.Bit.ENDISCD) ? (io_status | 0x02) : io_status;
        } else if (g_LensMotorInfo.LensMotorConfig[0].eChnelSel == AN41908_CHNEL3) {
            io_status = (reg_0x29.Bit.ENDISCD) ? (io_status | 0x01) : io_status;
            io_status = (reg_0x24.Bit.ENDISAB) ? (io_status | 0x02) : io_status;
        }

        io_status = (reg_0x0B.Bit.PDWNB) ? (io_status | 0x04) : io_status;
        break;

    case MD_SIGNAL_ZPI:
        break;

    case MD_SIGNAL_ZPR:
        break;

    case MD_SIGNAL_FPI:
        break;

    default:
        DBG_ERR("%s unknown!\r\n", __func__);
        break;
    }

    return io_status;
}


static void motorAN41908_SetSignal(MD_SIGNAL signal, UINT32 on)
{
    T_VD_FZ_POLARITY_0B reg_0x0B;
    T_PULSE1_MOVE_STEP_24 reg_0x24;
    T_PULSE2_MOVE_STEP_29 reg_0x29;

    DBG_MSG("%s signal=%u, on/off=%u\r\n", __func__, signal, (on != 0 ? TRUE : FALSE));

    switch (signal) {
    case MD_SIGNAL_STANDBY:    // power on/off motor driver
        reg_0x0B.Reg = g_LensMotorInfo.LensMotorReg.REG[11].Reg;    // IRIS
        reg_0x24.Reg = g_LensMotorInfo.LensMotorReg.REG[18].Reg;    // focus or zoom
        reg_0x29.Reg = g_LensMotorInfo.LensMotorReg.REG[22].Reg;    // zoom or focus
        reg_0x0B.Bit.PDWNB = reg_0x24.Bit.ENDISAB = reg_0x29.Bit.ENDISCD = (on > 0) ? 1 : 0;

        AN41908_Write2SPI((reg_0x0B.Reg << 8) + AN41908_VD_FZ_POLARITY, 0, 0, 0);
        AN41908_Write2SPI((reg_0x24.Reg << 8) + AN41908_PULSE1_MOVE_STEP, 0, 0, 0);
        AN41908_Write2SPI((reg_0x29.Reg << 8) + AN41908_PULSE2_MOVE_STEP, 0, 0, 0);
        g_LensMotorInfo.LensMotorReg.REG[11].Reg = reg_0x0B.Reg;
        g_LensMotorInfo.LensMotorReg.REG[18].Reg = reg_0x24.Reg;
        g_LensMotorInfo.LensMotorReg.REG[22].Reg = reg_0x29.Reg;
        break;

    case MD_SIGNAL_ZPI:    // enable ZPI output signal
        break;

    case MD_SIGNAL_ZPR:    // enable ZPR output signal
        break;

    case MD_SIGNAL_FPI:    // enable FPI output signal
        break;

    default:
        DBG_ERR("%s unknown!\r\n", __func__);
        break;
    }
}


static UINT32 motorAN41908_GetInitState(void)
{
    return (UINT32)((g_LensMotorInfo.LensMotorConfig[0].bMove2Home << 1) | g_LensMotorInfo.LensMotorConfig[1].bMove2Home);
}


static UINT32 motorAN41908_GetBusyState(void)
{
    return (UINT32)((g_LensMotorInfo.LensMotorConfig[0].iWorkStatus << 1) | g_LensMotorInfo.LensMotorConfig[1].iWorkStatus);
}


//----------------------------------------------------------------------------------------------------------------------
// zoom
//----------------------------------------------------------------------------------------------------------------------
static INT32 AN41908_ZoomMove(BOOL tele_dir, INT32 steps, INT32 gear_backlash)
{
    INT32 set_step = 0;
    MOTOR_CONFIG *pzoom_motor_cfg;

    pzoom_motor_cfg = &g_LensMotorInfo.LensMotorConfig[1];

    if (AN41908_WaitMoveEnd(MOTOR_ZOOM, 200) == 0) {
        set_step = steps + gear_backlash;

        if (set_step != 0) {
            pzoom_motor_cfg->iMoveDir = (tele_dir == TRUE) ? MOVE_DIR_FWD : MOVE_DIR_BWD;
        }

        if (tele_dir == TRUE) {
            AN41908_SetMotorPosition(MOTOR_ZOOM, set_step);
            pzoom_motor_cfg->iCurrPosition += steps;
        } else {
            AN41908_SetMotorPosition(MOTOR_ZOOM, -set_step);
            pzoom_motor_cfg->iCurrPosition -= steps;
        }
    }

    return set_step;
}


static void AN41908_ZoomSetSpeed(UINT32 speed)
{
    static BOOL ever_entry = FALSE;
    static INT32 zoom_dly_time = ZOOM_PSUM_DELAY;
    static AN41908_MOTOR_SPEED zoom_move_spd = AN41908_MOTOR_SPD_1X;
    UINT32 i, tmp, idx = 1, wt_reg;
    MOTOR_CONFIG *pzoom_motor_cfg, *pfocus_motor_cfg;

    pzoom_motor_cfg = &g_LensMotorInfo.LensMotorConfig[1];
    pfocus_motor_cfg = &g_LensMotorInfo.LensMotorConfig[0];

    if (ever_entry == FALSE) {
        ever_entry = TRUE;

        zoom_move_spd = pzoom_motor_cfg->eMoveSpeed;
        zoom_dly_time = pzoom_motor_cfg->iDlyTime;
    }

    speed = LENS_CLAMP(speed, SPEED_FULL/5, SPEED_FULL);
    tmp = (speed / (SPEED_FULL / 5)) - 1;

    if (tmp != 0) {
        for (i = 0; i < tmp; i++) {
            idx *= 2;
        }
    }

    pzoom_motor_cfg->eMoveSpeed = (AN41908_MOTOR_SPEED)idx;
    pzoom_motor_cfg->iDlyTime = zoom_dly_time * zoom_move_spd / pzoom_motor_cfg->eMoveSpeed;

    // update respective PWM frequency for achieving desired moving speed
    wt_reg = AN41908_Speed2Frequency(pzoom_motor_cfg->eMoveSpeed) & 0xFFFF;

    while ((pfocus_motor_cfg->iMotorStatus == MOTOR_STATUS_BUSY) || (pzoom_motor_cfg->iMotorStatus == MOTOR_STATUS_BUSY)) {
        LENS_DELAY_MS(1);
    }

    if (pzoom_motor_cfg->eChnelSel == AN41908_CHNEL1) {
        if (pfocus_motor_cfg->iWorkStatus == MOTOR_STATUS_BUSY) {
            AN41908_TransmitCmd(AN41908_CHNEL1, AN41908_SET_MOTOR_CFG, (wt_reg << 8) | AN41908_PULSE1_STEP_CYCLE, ((pfocus_motor_cfg->iExciteEnable | 0x3000) << 8) | AN41908_PULSE2_MOVE_STEP);
        } else if (pzoom_motor_cfg->iWorkStatus == MOTOR_STATUS_BUSY) {
            AN41908_TransmitCmd(AN41908_CHNEL1, AN41908_SET_MOTOR_CFG, (wt_reg << 8) | AN41908_PULSE1_STEP_CYCLE, ((pzoom_motor_cfg->iExciteEnable | 0x3000) << 8) | AN41908_PULSE1_MOVE_STEP);
        } else {
            AN41908_TransmitCmd(AN41908_CHNEL1, AN41908_SET_MOTOR_CFG, (wt_reg << 8) | AN41908_PULSE1_STEP_CYCLE, 0);
        }
    } else if (pzoom_motor_cfg->eChnelSel == AN41908_CHNEL3) {
        if (pfocus_motor_cfg->iWorkStatus == MOTOR_STATUS_BUSY) {
            AN41908_TransmitCmd(AN41908_CHNEL3, AN41908_SET_MOTOR_CFG, (wt_reg << 8) | AN41908_PULSE2_STEP_CYCLE, ((pfocus_motor_cfg->iExciteEnable | 0x3000) << 8) | AN41908_PULSE1_MOVE_STEP);
        } else if (pzoom_motor_cfg->iWorkStatus == MOTOR_STATUS_BUSY) {
            AN41908_TransmitCmd(AN41908_CHNEL3, AN41908_SET_MOTOR_CFG, (wt_reg << 8) | AN41908_PULSE2_STEP_CYCLE, ((pzoom_motor_cfg->iExciteEnable | 0x3000) << 8) | AN41908_PULSE2_MOVE_STEP);
        } else {
            AN41908_TransmitCmd(AN41908_CHNEL3, AN41908_SET_MOTOR_CFG, (wt_reg << 8) | AN41908_PULSE2_STEP_CYCLE, 0);
        }
    }
}


static UINT32 motorAN41908_ZoomGetSpeed(void)
{
    UINT32 i, tmp, idx = 1;

    tmp = (UINT32)g_LensMotorInfo.LensMotorConfig[1].eMoveSpeed;

    for (i = 0; i < 5; i++) {
        tmp /= 2;

        if (tmp != 0) {
            idx++;
        } else {
            break;
        }
    }

    return LENS_CLAMP(idx*SPEED_FULL/5, SPEED_FULL/5, SPEED_FULL);
}


static void motorAN41908_ZoomSetState(MOTOR_ZOOM_ACT state, UINT32 param)
{
    DBG_MSG("%s, state=%d, param=%d\r\n", __func__, state, param);

    switch (state) {
    case MOTOR_ZOOM_FWD:
        AN41908_ZoomMove(TRUE, (INT32)(param & 0xFFFF), (INT32)((param >> 16) & 0xFFFF));
        break;

    case MOTOR_ZOOM_BWD:
        AN41908_ZoomMove(FALSE, (INT32)(param & 0xFFFF), (INT32)((param >> 16) & 0xFFFF));
        break;

    case MOTOR_ZOOM_SETSPEED:
        AN41908_ZoomSetSpeed(param);
        break;

    default:
        DBG_ERR("%s unknown!\r\n", __func__);
        break;
    }
}


//----------------------------------------------------------------------------------------------------------------------
// focus
//----------------------------------------------------------------------------------------------------------------------
static INT32 AN41908_FocusMove(BOOL far_dir, INT32 steps, INT32 gear_backlash)
{
    INT32 set_step = 0;
    MOTOR_CONFIG *pfocus_motor_cfg;

    pfocus_motor_cfg = &g_LensMotorInfo.LensMotorConfig[0];

    if (AN41908_WaitMoveEnd(MOTOR_FOCUS, 200) == 0) {
        set_step = steps + gear_backlash;

        if (set_step != 0) {
            pfocus_motor_cfg->iMoveDir = (far_dir == TRUE) ? MOVE_DIR_FWD : MOVE_DIR_BWD;
        }

        if (far_dir == TRUE) {
            AN41908_SetMotorPosition(MOTOR_FOCUS, set_step);
            pfocus_motor_cfg->iCurrPosition += steps;
        } else {
            AN41908_SetMotorPosition(MOTOR_FOCUS, -set_step);
            pfocus_motor_cfg->iCurrPosition -= steps;
        }
    }

    return set_step;
}


static void motorAN41908_FocusZoomMove(BOOL far_dir, INT32 fsteps, INT32 fbacklash, BOOL tele_dir, INT32 zsteps, INT32 zbacklash)
{
    INT32 fset_step, zset_step;
    MOTOR_CONFIG *pfocus_motor_cfg, *pzoom_motor_cfg;

    pfocus_motor_cfg = &g_LensMotorInfo.LensMotorConfig[0];
    pzoom_motor_cfg = &g_LensMotorInfo.LensMotorConfig[1];

    fset_step = fsteps + fbacklash;
    zset_step = zsteps + zbacklash;

    if (fset_step != 0) {
        pfocus_motor_cfg->iMoveDir = (far_dir == TRUE) ? MOVE_DIR_FWD : MOVE_DIR_BWD;
    }

    if (zset_step != 0) {
        pzoom_motor_cfg->iMoveDir = (tele_dir == TRUE) ? MOVE_DIR_FWD : MOVE_DIR_BWD;
    }

    if ((far_dir == TRUE) && (tele_dir == TRUE)) {
        AN41908_SetMotorMove(fset_step, zset_step);
        pfocus_motor_cfg->iCurrPosition += fsteps;
        pzoom_motor_cfg->iCurrPosition += zsteps;
    } else if ((far_dir == TRUE) && (tele_dir == FALSE)) {
        AN41908_SetMotorMove(fset_step, -zset_step);
        pfocus_motor_cfg->iCurrPosition += fsteps;
        pzoom_motor_cfg->iCurrPosition -= zsteps;
    } else if ((far_dir == FALSE) && (tele_dir == TRUE)) {
        AN41908_SetMotorMove(-fset_step, zset_step);
        pfocus_motor_cfg->iCurrPosition -= fsteps;
        pzoom_motor_cfg->iCurrPosition += zsteps;
    } else if ((far_dir == FALSE) && (tele_dir == FALSE)) {
        AN41908_SetMotorMove(-fset_step, -zset_step);
        pfocus_motor_cfg->iCurrPosition -= fsteps;
        pzoom_motor_cfg->iCurrPosition -= zsteps;
    }
}


static void AN41908_FocusSetSpeed(UINT32 speed)
{
    static BOOL ever_entry = FALSE;
    static INT32 focus_dly_time = FOCUS_PSUM_DELAY;
    static AN41908_MOTOR_SPEED focus_move_spd = AN41908_MOTOR_SPD_1X;
    UINT32 i, tmp, idx = 1, wt_reg;
    MOTOR_CONFIG *pfocus_motor_cfg, *pzoom_motor_cfg;

    pfocus_motor_cfg = &g_LensMotorInfo.LensMotorConfig[0];
    pzoom_motor_cfg = &g_LensMotorInfo.LensMotorConfig[1];

    if (ever_entry == FALSE) {
        ever_entry = TRUE;

        focus_move_spd = pfocus_motor_cfg->eMoveSpeed;
        focus_dly_time = pfocus_motor_cfg->iDlyTime;
    }

    speed = LENS_CLAMP(speed, SPEED_FULL/5, SPEED_FULL);
    tmp = (speed / (SPEED_FULL / 5)) - 1;

    if (tmp != 0) {
        for (i = 0; i < tmp; i++) {
            idx *= 2;
        }
    }

    pfocus_motor_cfg->eMoveSpeed = (AN41908_MOTOR_SPEED)idx;
    pfocus_motor_cfg->iDlyTime = focus_dly_time * focus_move_spd / pfocus_motor_cfg->eMoveSpeed;

    // update respective PWM frequency for achieving desired moving speed
    wt_reg = AN41908_Speed2Frequency(pfocus_motor_cfg->eMoveSpeed) & 0xFFFF;

    while ((pfocus_motor_cfg->iMotorStatus == MOTOR_STATUS_BUSY) || (pzoom_motor_cfg->iMotorStatus == MOTOR_STATUS_BUSY)) {
        LENS_DELAY_MS(1);
    }

    if (pfocus_motor_cfg->eChnelSel == AN41908_CHNEL1) {
        if (pfocus_motor_cfg->iWorkStatus == MOTOR_STATUS_BUSY) {
            AN41908_TransmitCmd(AN41908_CHNEL1, AN41908_SET_MOTOR_CFG, (wt_reg << 8) | AN41908_PULSE1_STEP_CYCLE, ((pfocus_motor_cfg->iExciteEnable | 0x3000) << 8) | AN41908_PULSE1_MOVE_STEP);
        } else if (pzoom_motor_cfg->iWorkStatus == MOTOR_STATUS_BUSY) {
            AN41908_TransmitCmd(AN41908_CHNEL1, AN41908_SET_MOTOR_CFG, (wt_reg << 8) | AN41908_PULSE1_STEP_CYCLE, ((pzoom_motor_cfg->iExciteEnable | 0x3000) << 8) | AN41908_PULSE2_MOVE_STEP);
        } else {
            AN41908_TransmitCmd(AN41908_CHNEL1, AN41908_SET_MOTOR_CFG, (wt_reg << 8) | AN41908_PULSE1_STEP_CYCLE, 0);
        }        
    } else if (pfocus_motor_cfg->eChnelSel == AN41908_CHNEL3) {
        if (pfocus_motor_cfg->iWorkStatus == MOTOR_STATUS_BUSY) {
            AN41908_TransmitCmd(AN41908_CHNEL3, AN41908_SET_MOTOR_CFG, (wt_reg << 8) | AN41908_PULSE2_STEP_CYCLE, ((pfocus_motor_cfg->iExciteEnable | 0x3000) << 8) | AN41908_PULSE2_MOVE_STEP);
        } else if (pzoom_motor_cfg->iWorkStatus == MOTOR_STATUS_BUSY) {
            AN41908_TransmitCmd(AN41908_CHNEL3, AN41908_SET_MOTOR_CFG, (wt_reg << 8) | AN41908_PULSE2_STEP_CYCLE, ((pzoom_motor_cfg->iExciteEnable | 0x3000) << 8) | AN41908_PULSE1_MOVE_STEP);
        } else {
            AN41908_TransmitCmd(AN41908_CHNEL3, AN41908_SET_MOTOR_CFG, (wt_reg << 8) | AN41908_PULSE2_STEP_CYCLE, 0);
        }
    }
}


static UINT32 motorAN41908_FocusGetSpeed(void)
{
    UINT32 i, tmp, idx = 1;

    tmp = (UINT32)g_LensMotorInfo.LensMotorConfig[0].eMoveSpeed;

    for (i = 0; i < 5; i++) {
        tmp /= 2;

        if (tmp != 0) {
            idx++;
        } else {
            break;
        }
    }

    return LENS_CLAMP(idx*SPEED_FULL/5, SPEED_FULL/5, SPEED_FULL);
}


static BOOL motorAN41908_FocusSetState(MOTOR_FOCUS_ACT state, UINT32 param)
{
    DBG_MSG("%s, state=%d, param=%d\r\n", __func__, state, param);

    switch (state) {
    case MOTOR_FOCUS_FWD:
        AN41908_FocusMove(TRUE, (INT32)(param & 0xFFFF), (INT32)((param >> 16) & 0xFFFF));
        break;

    case MOTOR_FOCUS_BWD:
        AN41908_FocusMove(FALSE, (INT32)(param & 0xFFFF), (INT32)((param >> 16) & 0xFFFF));
        break;

    case MOTOR_FOCUS_SETSPEED:
        AN41908_FocusSetSpeed(param);
        break;

    default:
        DBG_ERR("%s unknown!\r\n", __func__);
        break;
    }

    return TRUE;
}


//----------------------------------------------------------------------------------------------------------------------
// aperture
//----------------------------------------------------------------------------------------------------------------------
static void motorAN41908_ApertureSetState(MOTOR_APERTURE_ACT state, UINT32 param)
{
    DBG_MSG("not support!\r\n");

    switch (state) {
    case MOTOR_APERTURE_NORMAL:
        break;

    case MOTOR_APERTURE_DUMMYLOAD:
        break;

    case MOTOR_APERTURE_FWD:
        break;

    case MOTOR_APERTURE_BWD:
        break;

    case MOTOR_APERTURE_EXCITE_ON:
        break;

    case MOTOR_APERTURE_EXCITE_OFF:
        break;

    case MOTOR_APERTURE_RESET:
        break;

    default:
        DBG_ERR("%s unknown!\r\n", __func__);
        break;
    }
}


//----------------------------------------------------------------------------------------------------------------------
// shutter
//----------------------------------------------------------------------------------------------------------------------
static void motorAN41908_ShutterSetState(MOTOR_SHUTTER_ACT state, UINT32 param)
{
    DBG_MSG("not support!\r\n");

    switch (state) {
    case MOTOR_SHUTTER_NORMAL:
        break;

    case MOTOR_SHUTTER_CAP:
        break;

    case MOTOR_SHUTTER_DUMMYLOAD:
        break;

    case MOTOR_SHUTTER_PSEUDO_OPEN:
        break;

    case MOTOR_SHUTTER_PSEUDO_CLOSE:
        break;

    default:
        DBG_ERR("%s unknown!\r\n", __func__);
        break;
    }
}


//----------------------------------------------------------------------------------------------------------------------
// IR cut
//----------------------------------------------------------------------------------------------------------------------
static void motorAN41908_IrcutSetState(MOTOR_IRCUT_ACT state, UINT32 param)
{
#if 0
    T_IRCUT_CONTROL_2C reg_0x2C;

    param = LENS_MIN(param, 200);

    reg_0x2C.Reg = g_LensMotorInfo.LensMotorReg.REG[24].Reg;

    if (state == MOTOR_IRCUT_OPEN) {    // 1 => open
        reg_0x2C.Bit.IR2 = 1;
        reg_0x2C.Bit.IR1 = 0;
    } else if (state == MOTOR_IRCUT_CLOSE) {    // 2 => close
        reg_0x2C.Bit.IR2 = 0;
        reg_0x2C.Bit.IR1 = 1;
    }

    AN41908_Write2SPI((reg_0x2C.Reg << 8) + AN41908_IRCUT_CTL, 0, 0, 0);
    g_LensMotorInfo.LensMotorReg.REG[24].Reg = reg_0x2C.Reg;

    LENS_DELAY_MS(param);

    // switch back to standby mode
    reg_0x2C.Bit.IR2 = 0;
    reg_0x2C.Bit.IR1 = 0;

    AN41908_Write2SPI((reg_0x2C.Reg << 8) + AN41908_IRCUT_CTL, 0, 0, 0);
    g_LensMotorInfo.LensMotorReg.REG[24].Reg = reg_0x2C.Reg;
#else
    param = LENS_MIN(param, 200);

    if (state == MOTOR_IRCUT_OPEN) {
        gpio_setPin(g_LensMotorInfo.uiPinIn[LENS_IO_IN_0]);
    } else if (state == MOTOR_IRCUT_CLOSE) {
        gpio_setPin(g_LensMotorInfo.uiPinIn[LENS_IO_IN_1]);
    }

    LENS_DELAY_MS(param);

    gpio_clearPin(g_LensMotorInfo.uiPinIn[LENS_IO_IN_0]);
    gpio_clearPin(g_LensMotorInfo.uiPinIn[LENS_IO_IN_1]);
#endif
}


