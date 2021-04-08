/**
 * Novatek PRINCETON PT5111 motor driver.
 * 
 * Reference motor driver implementation.
 * 
 * @file LensMotor_PT5111.c
 * @note Nothing.
 *
 * Copyright (C) 2019 Novatek Microelectronics Corp. All rights reserved.
 *
 */

#include "gpio.h"
#include "spi.h"
#include "LensMotor_PT5111.h"


// prototype declaration
static void   motorPT5111_Init(PLENS_DEVICE_OBJ plens_dev);
static UINT32 motorPT5111_GetSignal(MD_SIGNAL signal);
static void   motorPT5111_SetSignal(MD_SIGNAL signal, UINT32 on);
static UINT32 motorPT5111_GetInitState(void);
static UINT32 motorPT5111_GetBusyState(void);
static UINT32 motorPT5111_ZoomGetSpeed(void);
static void   motorPT5111_ZoomSetState(MOTOR_ZOOM_ACT state, UINT32 param);
static UINT32 motorPT5111_FocusGetSpeed(void);
static BOOL   motorPT5111_FocusSetState(MOTOR_FOCUS_ACT state, UINT32 param);
static void   motorPT5111_FocusZoomMove(BOOL far_dir, INT32 fsteps, INT32 fbacklash, BOOL tele_dir, INT32 zsteps, INT32 zbacklash);
static void   motorPT5111_ApertureSetState(MOTOR_APERTURE_ACT state, UINT32 param);
static void   motorPT5111_ShutterSetState(MOTOR_SHUTTER_ACT state, UINT32 param);
static void   motorPT5111_IrcutSetState(MOTOR_IRCUT_ACT state, UINT32 param);


//----------------------------------------------------------------------------------------------------------------------
// general
//----------------------------------------------------------------------------------------------------------------------
LENSMOTOR_INFO* PT5111_GetMotorInfo(void)
{
    return &g_LensMotorInfo;
}


PMOTOR_TAB DrvLens_GetMotor(void)
{
    DBG_MSG("%s\r\n", __func__);

    g_LensMotorInfo.LensMotor.init               = motorPT5111_Init;
    g_LensMotorInfo.LensMotor.getSignal          = motorPT5111_GetSignal;
    g_LensMotorInfo.LensMotor.setSignal          = motorPT5111_SetSignal;
    g_LensMotorInfo.LensMotor.getInitState       = motorPT5111_GetInitState;
    g_LensMotorInfo.LensMotor.getBusyState       = motorPT5111_GetBusyState;
    g_LensMotorInfo.LensMotor.zoom_getSpeed      = motorPT5111_ZoomGetSpeed;
    g_LensMotorInfo.LensMotor.zoom_setState      = motorPT5111_ZoomSetState;
    g_LensMotorInfo.LensMotor.focus_getSpeed     = motorPT5111_FocusGetSpeed;
    g_LensMotorInfo.LensMotor.focus_setState     = motorPT5111_FocusSetState;
    g_LensMotorInfo.LensMotor.focus_zoom_setMove = motorPT5111_FocusZoomMove;
    g_LensMotorInfo.LensMotor.aperture_setState  = motorPT5111_ApertureSetState;
    g_LensMotorInfo.LensMotor.shutter_setState   = motorPT5111_ShutterSetState;
    g_LensMotorInfo.LensMotor.ircut_setState     = motorPT5111_IrcutSetState;

    return &g_LensMotorInfo.LensMotor;
}


static INT32 PT5111_InitSPI(void)
{
    spi_setConfig(MOTOR_PT5111_SPI_CH, SPI_CONFIG_ID_BUSMODE, SPI_MODE_3);
    spi_setConfig(MOTOR_PT5111_SPI_CH, SPI_CONFIG_ID_FREQ, 10000000);    // bus clock = 4 MHz (~6.2 MHz)
    spi_setConfig(MOTOR_PT5111_SPI_CH, SPI_CONFIG_ID_MSB_LSB, SPI_MSB);
    spi_setConfig(MOTOR_PT5111_SPI_CH, SPI_CONFIG_ID_CS_ACT_LEVEL, SPI_CS_ACT_LEVEL_HIGH);
    spi_setConfig(MOTOR_PT5111_SPI_CH, SPI_CONFIG_ID_CS_CK_DLY, 0);      // unit: usec

    if (spi_open(MOTOR_PT5111_SPI_CH) != E_OK) {
        return -1;
    }

    spi_setTransferLen(MOTOR_PT5111_SPI_CH, SPI_TRANSFER_LEN_2BYTES);    // 2 bytes mode in 1 packet

    return 0;
}


static void PT5111_Write2SPI(UINT16 spi_cmd1, UINT16 spi_cmd2, UINT16 spi_cmd3, UINT16 spi_cmd4, UINT16 spi_cmd5, UINT16 spi_cmd6)
{
    loc_cpu();

    if (spi_cmd1 != 0xFFFF) {
        spi_writePacket(MOTOR_PT5111_SPI_CH, 1, (UINT32)spi_cmd1);
        LENS_DELAY_US(1);
    }
    if (spi_cmd2 != 0xFFFF) {
        spi_writePacket(MOTOR_PT5111_SPI_CH, 1, (UINT32)spi_cmd2);
        LENS_DELAY_US(1);
    }
    if (spi_cmd3 != 0xFFFF) {
        spi_writePacket(MOTOR_PT5111_SPI_CH, 1, (UINT32)spi_cmd3);
        LENS_DELAY_US(1);
    }
    if (spi_cmd4 != 0xFFFF) {
        spi_writePacket(MOTOR_PT5111_SPI_CH, 1, (UINT32)spi_cmd4);
        LENS_DELAY_US(1);
    }
    if (spi_cmd5 != 0xFFFF) {
        spi_writePacket(MOTOR_PT5111_SPI_CH, 1, (UINT32)spi_cmd5);
        LENS_DELAY_US(1);
    }
    if (spi_cmd6 != 0xFFFF) {
        spi_writePacket(MOTOR_PT5111_SPI_CH, 1, (UINT32)spi_cmd6);
        LENS_DELAY_US(1);
    }

    unl_cpu();
}


#if 0
static UINT32 PT5111_Read4SPI(UINT16 addr)
{
    UINT32 rd_data = 0;

    spi_setCSActive(MOTOR_PT5111_SPI_CH, TRUE);

    spi_writePacket(MOTOR_PT5111_SPI_CH, 1, (UINT32)(addr & 0xF300));

    spi_readSingle(MOTOR_PT5111_SPI_CH, &rd_data);    // receive 8-bit data

    spi_setCSActive(MOTOR_PT5111_SPI_CH, FALSE);

    return (rd_data & 0xFF);
}
#endif


INT32 PT5111_TransmitCmd(PT5111_CHNEL_NUM chnel, PT5111_CMD_TYPE cmd_type, INT32 cmd_arg1, INT32 cmd_arg2)
{
    UINT16 spi_data[6] = { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF };
    INT32 cmd_status = 0;

    switch (cmd_type) {
    case PT5111_SET_POWER_CTRL:
        spi_data[0] = 0xCE00;

        if (cmd_arg1 != 0) {
            spi_data[0] |= 0x0100;
        }
        if (cmd_arg2 != 0) {
            spi_data[0] |= 0x00C0;
        }
        break;

    case PT5111_SET_OPERATE_CTRL:
        spi_data[0] = 0xFC0C;

        if (cmd_arg1 != 0) {
            spi_data[0] |= 0x0200;
        }
        if (cmd_arg2 != 0) {
            spi_data[0] |= 0x0100;
        }
        break;

    case PT5111_SET_MOTOR_SPD:
        if (chnel == PT5111_CHNEL1) {
            spi_data[0] = 0x0C00 + (UINT16)(cmd_arg1 & 0x03FF);
        } else if (chnel == PT5111_CHNEL3) {
            spi_data[0] = 0x4C00 + (UINT16)(cmd_arg1 & 0x03FF);
        } else {
            cmd_status = -1;
        }
        break;

    case PT5111_SET_PULSE_NUM:
        if (chnel == PT5111_CHNEL1) {
            if (cmd_arg2 != 0) {
                if (cmd_arg1 == 0) {    // iDefPositiveDir
                    spi_data[0] = (cmd_arg2 > 0) ? 0x0000 : 0x0200;
                } else {
                    spi_data[0] = (cmd_arg2 > 0) ? 0x0200 : 0x0000;
                }

                spi_data[1] = 0x0400 + (UINT16)(LENS_ABS(cmd_arg2) & 0x03FF);
                spi_data[2] = 0x0800 + (UINT16)((LENS_ABS(cmd_arg2) >> 10) & 0x03FF);
            }
        } else if (chnel == PT5111_CHNEL3) {
            if (cmd_arg2 != 0) {
                if (cmd_arg1 == 0) {    // iDefPositiveDir
                    spi_data[0] = (cmd_arg2 > 0) ? 0x4000 : 0x4200;
                } else {
                    spi_data[0] = (cmd_arg2 > 0) ? 0x4200 : 0x4000;
                }

                spi_data[1] = 0x4400 + (UINT16)(LENS_ABS(cmd_arg2) & 0x03FF);
                spi_data[2] = 0x4800 + (UINT16)((LENS_ABS(cmd_arg2) >> 10) & 0x03FF);
            }
        } else {
            cmd_status = -1;
        }
        break;

    case PT5111_SET_FZ_OUTPUT_PULSE:
        if (chnel == PT5111_CHNEL1) {
            if (cmd_arg1 != 0) {
                if (g_LensMotorInfo.LensMotorConfig[0].iDefPositiveDir == 0) {    // focus
                    spi_data[0] = (cmd_arg1 > 0) ? 0x0000 : 0x0200;
                } else {
                    spi_data[0] = (cmd_arg1 > 0) ? 0x0200 : 0x0000;
                }
            }
            spi_data[1] = 0x0400 + (UINT16)(LENS_ABS(cmd_arg1) & 0x03FF);
            spi_data[2] = 0x0800 + (UINT16)((LENS_ABS(cmd_arg1) >> 10) & 0x03FF);

            if (cmd_arg2 != 0) {
                if (g_LensMotorInfo.LensMotorConfig[1].iDefPositiveDir == 0) {    // zoom
                    spi_data[3] = (cmd_arg2 > 0) ? 0x4000 : 0x4200;
                } else {
                    spi_data[3] = (cmd_arg2 > 0) ? 0x4200 : 0x4000;
                }
            }
            spi_data[4] = 0x4400 + (UINT16)(LENS_ABS(cmd_arg2) & 0x03FF);
            spi_data[5] = 0x4800 + (UINT16)((LENS_ABS(cmd_arg2) >> 10) & 0x03FF);
        } else if (chnel == PT5111_CHNEL3) {
            if (cmd_arg1 != 0) {
                if (g_LensMotorInfo.LensMotorConfig[0].iDefPositiveDir == 0) {    // focus
                    spi_data[0] = (cmd_arg1 > 0) ? 0x4000 : 0x4200;
                } else {
                    spi_data[0] = (cmd_arg1 > 0) ? 0x4200 : 0x4000;
                }
            }
            spi_data[1] = 0x4400 + (UINT16)(LENS_ABS(cmd_arg1) & 0x03FF);
            spi_data[2] = 0x4800 + (UINT16)((LENS_ABS(cmd_arg1) >> 10) & 0x03FF);

            if (cmd_arg2 != 0) {
                if (g_LensMotorInfo.LensMotorConfig[1].iDefPositiveDir == 0) {    // zoom
                    spi_data[3] = (cmd_arg2 > 0) ? 0x0000 : 0x0200;
                } else {
                    spi_data[3] = (cmd_arg2 > 0) ? 0x0200 : 0x0000;
                }
            }
            spi_data[4] = 0x0400 + (UINT16)(LENS_ABS(cmd_arg2) & 0x03FF);
            spi_data[5] = 0x0800 + (UINT16)((LENS_ABS(cmd_arg2) >> 10) & 0x03FF);
        } else {
            cmd_status = -1;
        }
        break;

    default:
        cmd_status = -1;
        break;
    }

    if (cmd_status != -1) {
        PT5111_Write2SPI(spi_data[0], spi_data[1], spi_data[2], spi_data[3], spi_data[4], spi_data[5]);
    } else {
        DBG_MSG("%s err!\r\n", __func__);
    }

    return cmd_status;
}


#if EXT_OUT_PIN_EXIST
static void PT5111_GpioIsr(UINT32 gpio_pin)
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


static INT32 PT5111_WaitMoveEnd(MOTOR_SEL motor_sel, const UINT32 timeout_ms)
{
#if EXT_OUT_PIN_EXIST
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


static INT32 PT5111_MotorInit(MOTOR_SEL motor_sel)
{
    UINT32 wt_reg;
    MOTOR_CONFIG *pmotor_cfg;

    pmotor_cfg = (motor_sel == MOTOR_FOCUS) ? &g_LensMotorInfo.LensMotorConfig[0] :
                ((motor_sel == MOTOR_ZOOM)  ? &g_LensMotorInfo.LensMotorConfig[1] : NULL);

    if ((motor_sel >= MOTOR_IRIS) || (pmotor_cfg == NULL)) {
        DBG_MSG("%s unknown motor channel!\r\n", __func__);

        return -1;
    }

    // calculate pulse rate depending on eMoveSpeed for achieving desired moving speed
    wt_reg = 40000000 / pmotor_cfg->uiWaveFreq / 16 / pmotor_cfg->eMoveSpeed;
    wt_reg = LENS_CLAMP(wt_reg, 1, 1023);

    PT5111_TransmitCmd(pmotor_cfg->eChnelSel, PT5111_SET_MOTOR_SPD, (INT32)wt_reg, 0);

    return 0;
}


static INT32 PT5111_SetMotorPosition(MOTOR_SEL motor_sel, INT32 step_pos)
{
    INT32 pulse_num = 0, dly_time;
    MOTOR_CONFIG *pmotor_cfg;

    pmotor_cfg = (motor_sel == MOTOR_FOCUS) ? &g_LensMotorInfo.LensMotorConfig[0] :
                ((motor_sel == MOTOR_ZOOM)  ? &g_LensMotorInfo.LensMotorConfig[1] : NULL);

    if (pmotor_cfg == NULL) {
        DBG_MSG("%s unknown motor channel!\r\n", __func__);

        return -1;
    }

    wai_sem(g_uiPt5111SpiSema);

    if (step_pos != 0) {
        pmotor_cfg->iWorkStatus = pmotor_cfg->iMotorStatus = MOTOR_STATUS_BUSY;

        if (pmotor_cfg->eChnelSel == PT5111_CHNEL1) {
            PT5111_TransmitCmd(PT5111_CHNEL_NULL, PT5111_SET_POWER_CTRL, 1, 0);
        } else {
            PT5111_TransmitCmd(PT5111_CHNEL_NULL, PT5111_SET_POWER_CTRL, 0, 1);
        }

        pulse_num = (motor_sel == MOTOR_FOCUS) ? ((INT32)pmotor_cfg->eMicroStepMode * 2 * step_pos) : ((INT32)pmotor_cfg->eMicroStepMode * step_pos);

        PT5111_TransmitCmd(pmotor_cfg->eChnelSel, PT5111_SET_PULSE_NUM, pmotor_cfg->iDefPositiveDir, pulse_num);

        if (pmotor_cfg->eChnelSel == PT5111_CHNEL1) {
            PT5111_TransmitCmd(PT5111_CHNEL_NULL, PT5111_SET_OPERATE_CTRL, 1, 0);
        } else {
            PT5111_TransmitCmd(PT5111_CHNEL_NULL, PT5111_SET_OPERATE_CTRL, 0, 1);
        }

#if EXT_OUT_PIN_EXIST
        dly_time = pmotor_cfg->iDlyTime;
#else
        dly_time = pmotor_cfg->iDlyTime * LENS_ABS(step_pos) / pmotor_cfg->iMaxStepSize + 6;
#endif
        LENS_DELAY_MS(dly_time);

        pmotor_cfg->iMotorStatus = MOTOR_STATUS_FREE;
    } else {
        PT5111_TransmitCmd(PT5111_CHNEL_NULL, PT5111_SET_POWER_CTRL, 0, 0);

        pmotor_cfg->iWorkStatus = MOTOR_STATUS_FREE;
    }

    sig_sem(g_uiPt5111SpiSema);

    return step_pos;
}


static void PT5111_SetMotorMove(INT32 focus_steps, INT32 zoom_steps)
{
    INT32 pwr_on[2] = { 0, 0 }, dly_time[2];
    MOTOR_CONFIG *pfocus_motor_cfg, *pzoom_motor_cfg;

    pfocus_motor_cfg = &g_LensMotorInfo.LensMotorConfig[0];
    pzoom_motor_cfg = &g_LensMotorInfo.LensMotorConfig[1];

    wai_sem(g_uiPt5111SpiSema);

    if (focus_steps != 0) {
        pwr_on[pfocus_motor_cfg->eChnelSel] = 1;
        pfocus_motor_cfg->iWorkStatus = pfocus_motor_cfg->iMotorStatus = MOTOR_STATUS_BUSY;
    } else {
        pfocus_motor_cfg->iWorkStatus = MOTOR_STATUS_FREE;
    }

    if (zoom_steps != 0) {
        pwr_on[pzoom_motor_cfg->eChnelSel] = 1;
        pzoom_motor_cfg->iWorkStatus = pzoom_motor_cfg->iMotorStatus = MOTOR_STATUS_BUSY;
    } else {
        pzoom_motor_cfg->iWorkStatus = MOTOR_STATUS_FREE;
    }

    PT5111_TransmitCmd(PT5111_CHNEL_NULL, PT5111_SET_POWER_CTRL, pwr_on[0], pwr_on[1]);

    PT5111_TransmitCmd(pfocus_motor_cfg->eChnelSel, PT5111_SET_FZ_OUTPUT_PULSE, (INT32)pfocus_motor_cfg->eMicroStepMode*2*focus_steps, (INT32)pzoom_motor_cfg->eMicroStepMode*zoom_steps);

    PT5111_TransmitCmd(PT5111_CHNEL_NULL, PT5111_SET_OPERATE_CTRL, pwr_on[0], pwr_on[1]);

    dly_time[0] = pfocus_motor_cfg->iDlyTime * LENS_ABS(focus_steps) / pfocus_motor_cfg->iMaxStepSize + 6;
    dly_time[1] = pzoom_motor_cfg->iDlyTime * LENS_ABS(zoom_steps) / pzoom_motor_cfg->iMaxStepSize + 6;
    dly_time[0] = LENS_MAX(dly_time[0], dly_time[1]);
    LENS_DELAY_MS(dly_time[0]);

    if (focus_steps != 0) {
        pfocus_motor_cfg->iMotorStatus = MOTOR_STATUS_FREE;
    }

    if (zoom_steps != 0) {
        pzoom_motor_cfg->iMotorStatus = MOTOR_STATUS_FREE;
    }

    sig_sem(g_uiPt5111SpiSema);
}


static void motorPT5111_Init(PLENS_DEVICE_OBJ plens_dev)
{
    UINT16 *pinit_reg;
    UINT32 i;

    // initialize GPIO
    for (i = 0; i < LENS_IO_MAX; i++) {
        g_LensMotorInfo.uiPinIn[i] = plens_dev->pGPIOArray[i];
    }

    if (PT5111_InitSPI() == 0) {
        // reset motor driver IC
        if (g_LensMotorInfo.uiPinIn[LENS_IO_RESET] > 0) {
            gpio_clearPin(g_LensMotorInfo.uiPinIn[LENS_IO_RESET]);
            LENS_DELAY_MS(2);
            gpio_setPin(g_LensMotorInfo.uiPinIn[LENS_IO_RESET]);
        }

#if EXT_OUT_PIN_EXIST
        // setup focus & zoom EXT GPIO interrupt
        gpio_open();

    #if ISR_RISING_EDGE_TRIGGER
        gpio_setIntTypePol(g_LensMotorInfo.uiPinIn[LENS_IO_FOCUS_PIINT], GPIO_INTTYPE_EDGE, GPIO_INTPOL_POSHIGH);
        gpio_setIntTypePol(g_LensMotorInfo.uiPinIn[LENS_IO_ZOOM_PIINT], GPIO_INTTYPE_EDGE, GPIO_INTPOL_POSHIGH);
    #else
        gpio_setIntTypePol(g_LensMotorInfo.uiPinIn[LENS_IO_FOCUS_PIINT], GPIO_INTTYPE_EDGE, GPIO_INTPOL_NEGLOW);
        gpio_setIntTypePol(g_LensMotorInfo.uiPinIn[LENS_IO_ZOOM_PIINT], GPIO_INTTYPE_EDGE, GPIO_INTPOL_NEGLOW);
    #endif

        gpio_setIntIsr(g_LensMotorInfo.uiPinIn[LENS_IO_FOCUS_PIINT], PT5111_GpioIsr);
        gpio_setIntIsr(g_LensMotorInfo.uiPinIn[LENS_IO_ZOOM_PIINT], PT5111_GpioIsr);

        gpio_clearIntStatus(g_LensMotorInfo.uiPinIn[LENS_IO_FOCUS_PIINT]);
        gpio_clearIntStatus(g_LensMotorInfo.uiPinIn[LENS_IO_ZOOM_PIINT]);
        gpio_enableInt(g_LensMotorInfo.uiPinIn[LENS_IO_FOCUS_PIINT]);
        gpio_enableInt(g_LensMotorInfo.uiPinIn[LENS_IO_ZOOM_PIINT]);

        gpio_close();
#endif

        // write initial table to PT5111 register
        pinit_reg = (UINT16 *)&g_LensMotorInfo.LensMotorReg.wReg[0];

        for (i = 0; i < 64; i++) {
            if (pinit_reg[i] != 0xFFFF) {
                PT5111_Write2SPI(pinit_reg[i], 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF);
                LENS_DELAY_MS(1);
            }
        }

        PT5111_MotorInit(MOTOR_ZOOM);
        PT5111_MotorInit(MOTOR_FOCUS);
    } else {
        DBG_ERR("PT5111_InitSPI err!\r\n");
    }
}


static UINT32 motorPT5111_GetSignal(MD_SIGNAL signal)
{
    DBG_MSG("not support get signal!\r\n");

    return 0;
}


static void motorPT5111_SetSignal(MD_SIGNAL signal, UINT32 on)
{
    DBG_MSG("not support set signal!\r\n");
}


static UINT32 motorPT5111_GetInitState(void)
{
    return (UINT32)((g_LensMotorInfo.LensMotorConfig[0].bMove2Home << 1) | g_LensMotorInfo.LensMotorConfig[1].bMove2Home);
}


static UINT32 motorPT5111_GetBusyState(void)
{
    return (UINT32)((g_LensMotorInfo.LensMotorConfig[0].iWorkStatus << 1) | g_LensMotorInfo.LensMotorConfig[1].iWorkStatus);
}


//----------------------------------------------------------------------------------------------------------------------
// zoom
//----------------------------------------------------------------------------------------------------------------------
static INT32 PT5111_ZoomMove(BOOL tele_dir, INT32 steps, INT32 gear_backlash)
{
    INT32 set_step = 0;
    MOTOR_CONFIG *pzoom_motor_cfg;

    pzoom_motor_cfg = &g_LensMotorInfo.LensMotorConfig[1];

    if (PT5111_WaitMoveEnd(MOTOR_ZOOM, 200) == 0) {
        set_step = steps + gear_backlash;

        if (set_step != 0) {
            pzoom_motor_cfg->iMoveDir = (tele_dir == TRUE) ? MOVE_DIR_FWD : MOVE_DIR_BWD;
        }

        if (tele_dir == TRUE) {
            PT5111_SetMotorPosition(MOTOR_ZOOM, set_step);
            pzoom_motor_cfg->iCurrPosition += steps;
        } else {
            PT5111_SetMotorPosition(MOTOR_ZOOM, -set_step);
            pzoom_motor_cfg->iCurrPosition -= steps;
        }
    }

    return set_step;
}


static void PT5111_ZoomSetSpeed(UINT32 speed)
{
    static BOOL ever_entry = FALSE;
    static INT32 zoom_dly_time = ZOOM_PSUM_DELAY;
    static PT5111_MOTOR_SPEED zoom_move_spd = PT5111_MOTOR_SPD_1X;
    UINT32 i, tmp, idx = 1, wt_reg, time_cnt = 0;
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

    pzoom_motor_cfg->eMoveSpeed = (PT5111_MOTOR_SPEED)idx;
    pzoom_motor_cfg->iDlyTime = zoom_dly_time * zoom_move_spd / pzoom_motor_cfg->eMoveSpeed;

    while (((pfocus_motor_cfg->iMotorStatus == MOTOR_STATUS_BUSY) || (pzoom_motor_cfg->iMotorStatus == MOTOR_STATUS_BUSY)) && (++time_cnt < 1000)) {
        LENS_DELAY_MS(1);
    }

    // calculate pulse rate depending on eMoveSpeed for achieving desired moving speed
    wt_reg = 40000000 / pzoom_motor_cfg->uiWaveFreq / 16 / pzoom_motor_cfg->eMoveSpeed;
    wt_reg = LENS_CLAMP(wt_reg, 1, 1023);

    PT5111_TransmitCmd(pzoom_motor_cfg->eChnelSel, PT5111_SET_MOTOR_SPD, (INT32)wt_reg, 0);
}


static UINT32 motorPT5111_ZoomGetSpeed(void)
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


static void motorPT5111_ZoomSetState(MOTOR_ZOOM_ACT state, UINT32 param)
{
    DBG_MSG("%s, state=%d, param=%d\r\n", __func__, state, param);

    switch (state) {
    case MOTOR_ZOOM_FWD:
        PT5111_ZoomMove(TRUE, (INT32)(param & 0xFFFF), (INT32)((param >> 16) & 0xFFFF));
        break;

    case MOTOR_ZOOM_BWD:
        PT5111_ZoomMove(FALSE, (INT32)(param & 0xFFFF), (INT32)((param >> 16) & 0xFFFF));
        break;

    case MOTOR_ZOOM_SETSPEED:
        PT5111_ZoomSetSpeed(param);
        break;

    default:
        DBG_ERR("%s unknown!\r\n", __func__);
        break;
    }
}


//----------------------------------------------------------------------------------------------------------------------
// focus
//----------------------------------------------------------------------------------------------------------------------
static INT32 PT5111_FocusMove(BOOL far_dir, INT32 steps, INT32 gear_backlash)
{
    INT32 set_step = 0;
    MOTOR_CONFIG *pfocus_motor_cfg;

    pfocus_motor_cfg = &g_LensMotorInfo.LensMotorConfig[0];

    if (PT5111_WaitMoveEnd(MOTOR_FOCUS, 200) == 0) {
        set_step = steps + gear_backlash;

        if (set_step != 0) {
            pfocus_motor_cfg->iMoveDir = (far_dir == TRUE) ? MOVE_DIR_FWD : MOVE_DIR_BWD;
        }

        if (far_dir == TRUE) {
            PT5111_SetMotorPosition(MOTOR_FOCUS, set_step);
            pfocus_motor_cfg->iCurrPosition += steps;
        } else {
            PT5111_SetMotorPosition(MOTOR_FOCUS, -set_step);
            pfocus_motor_cfg->iCurrPosition -= steps;
        }
    }

    return set_step;
}


static void motorPT5111_FocusZoomMove(BOOL far_dir, INT32 fsteps, INT32 fbacklash, BOOL tele_dir, INT32 zsteps, INT32 zbacklash)
{
    INT32 fset_step, zset_step;
    MOTOR_CONFIG *pfocus_motor_cfg, *pzoom_motor_cfg;

    pfocus_motor_cfg = &g_LensMotorInfo.LensMotorConfig[0];
    pzoom_motor_cfg = &g_LensMotorInfo.LensMotorConfig[1];

    fset_step = LENS_ABS(fsteps) + LENS_ABS(fbacklash);
    zset_step = LENS_ABS(zsteps) + LENS_ABS(zbacklash);

    if (fset_step != 0) {
        pfocus_motor_cfg->iMoveDir = (far_dir == TRUE) ? MOVE_DIR_FWD : MOVE_DIR_BWD;
    }

    if (zset_step != 0) {
        pzoom_motor_cfg->iMoveDir = (tele_dir == TRUE) ? MOVE_DIR_FWD : MOVE_DIR_BWD;
    }

    if ((far_dir == TRUE) && (tele_dir == TRUE)) {
        PT5111_SetMotorMove(fset_step, zset_step);
        pfocus_motor_cfg->iCurrPosition += fsteps;
        pzoom_motor_cfg->iCurrPosition += zsteps;
    } else if ((far_dir == TRUE) && (tele_dir == FALSE)) {
        PT5111_SetMotorMove(fset_step, -zset_step);
        pfocus_motor_cfg->iCurrPosition += fsteps;
        pzoom_motor_cfg->iCurrPosition -= zsteps;
    } else if ((far_dir == FALSE) && (tele_dir == TRUE)) {
        PT5111_SetMotorMove(-fset_step, zset_step);
        pfocus_motor_cfg->iCurrPosition -= fsteps;
        pzoom_motor_cfg->iCurrPosition += zsteps;
    } else if ((far_dir == FALSE) && (tele_dir == FALSE)) {
        PT5111_SetMotorMove(-fset_step, -zset_step);
        pfocus_motor_cfg->iCurrPosition -= fsteps;
        pzoom_motor_cfg->iCurrPosition -= zsteps;
    }
}


static void PT5111_FocusSetSpeed(UINT32 speed)
{
    static BOOL ever_entry = FALSE;
    static INT32 focus_dly_time = FOCUS_PSUM_DELAY;
    static PT5111_MOTOR_SPEED focus_move_spd = PT5111_MOTOR_SPD_1X;
    UINT32 i, tmp, idx = 1, wt_reg, time_cnt = 0;
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

    pfocus_motor_cfg->eMoveSpeed = (PT5111_MOTOR_SPEED)idx;
    pfocus_motor_cfg->iDlyTime = focus_dly_time * focus_move_spd / pfocus_motor_cfg->eMoveSpeed;

    while (((pfocus_motor_cfg->iMotorStatus == MOTOR_STATUS_BUSY) || (pzoom_motor_cfg->iMotorStatus == MOTOR_STATUS_BUSY)) && (++time_cnt < 1000)) {
        LENS_DELAY_MS(1);
    }

    // calculate pulse rate depending on eMoveSpeed for achieving desired moving speed
    wt_reg = 40000000 / pfocus_motor_cfg->uiWaveFreq / 16 / pfocus_motor_cfg->eMoveSpeed;
    wt_reg = LENS_CLAMP(wt_reg, 1, 1023);

    PT5111_TransmitCmd(pfocus_motor_cfg->eChnelSel, PT5111_SET_MOTOR_SPD, (INT32)wt_reg, 0);
}


static UINT32 motorPT5111_FocusGetSpeed(void)
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


static BOOL motorPT5111_FocusSetState(MOTOR_FOCUS_ACT state, UINT32 param)
{
    DBG_MSG("%s, state=%d, param=%d\r\n", __func__, state, param);

    switch (state) {
    case MOTOR_FOCUS_FWD:
        PT5111_FocusMove(TRUE, (INT32)(param & 0xFFFF), (INT32)((param >> 16) & 0xFFFF));
        break;

    case MOTOR_FOCUS_BWD:
        PT5111_FocusMove(FALSE, (INT32)(param & 0xFFFF), (INT32)((param >> 16) & 0xFFFF));
        break;

    case MOTOR_FOCUS_SETSPEED:
        PT5111_FocusSetSpeed(param);
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
static void motorPT5111_ApertureSetState(MOTOR_APERTURE_ACT state, UINT32 param)
{
    DBG_MSG("not support aperture!\r\n");

    switch (state) {
    case MOTOR_APERTURE_NORMAL:
        break;

    case MOTOR_APERTURE_FWD:
        break;

    case MOTOR_APERTURE_BWD:
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
static void motorPT5111_ShutterSetState(MOTOR_SHUTTER_ACT state, UINT32 param)
{
    DBG_MSG("not support shutter!\r\n");

    switch (state) {
    case MOTOR_SHUTTER_NORMAL:
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
static void motorPT5111_IrcutSetState(MOTOR_IRCUT_ACT state, UINT32 param)
{
    param = LENS_MIN(param, 200);

    if ((g_LensMotorInfo.uiPinIn[LENS_IO_IN_0] != 0) && (g_LensMotorInfo.uiPinIn[LENS_IO_IN_1] != 0)) {
        if (state == MOTOR_IRCUT_OPEN) {
            gpio_setPin(g_LensMotorInfo.uiPinIn[LENS_IO_IN_0]);
        } else if (state == MOTOR_IRCUT_CLOSE) {
            gpio_setPin(g_LensMotorInfo.uiPinIn[LENS_IO_IN_1]);
        }

        LENS_DELAY_MS(param);

        gpio_clearPin(g_LensMotorInfo.uiPinIn[LENS_IO_IN_0]);
        gpio_clearPin(g_LensMotorInfo.uiPinIn[LENS_IO_IN_1]);
    } else {
        DBG_ERR("no assign IRCut GPIO!\r\n");
    }
}


