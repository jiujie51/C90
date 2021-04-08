/**
 * Novatek SANYO LV8044 lens driver.
 * 
 * Reference lens driver implementation.
 * 
 * @file LensDrv_LV8044.c
 * @note Nothing.
 *
 * Copyright (C) 2018 Novatek Microelectronics Corp. All rights reserved.
 *
 */

#include "LensDrv_LV8044.h"


// prototype declaration
static void     lensLV8044_Init(PLENS_DEVICE_OBJ plens_dev);
static void     lensLV8044_GetDriverName(INT8 *ic_name);
static UINT32   lensLV8044_GetSignal(MD_SIGNAL signal);
static void     lensLV8044_SetSignal(MD_SIGNAL signal, UINT32 on);
static UINT32   lensLV8044_GetInitState(void);
static UINT32   lensLV8044_GetBusyState(void);
static INT32    lensLV8044_ZoomInit(UINT32 stage);
static UINT32   lensLV8044_ZoomGetSpeed(void);
static void     lensLV8044_ZoomSetSpeed(LENS_SPEED_CATEGORY category);
static UINT32   lensLV8044_ZoomGetMinSection(void);
static UINT32   lensLV8044_ZoomGetMaxSection(void);
static UINT32   lensLV8044_ZoomGetSection(void);
static INT32    lensLV8044_ZoomGetSectionPosition(void);
static INT32    lensLV8044_ZoomGetPosition(void);
static void     lensLV8044_ZoomSetPosition(INT32 position, INT32 dir);
static INT32    lensLV8044_ZoomGo2Section(UINT32 section);
static INT32    lensLV8044_ZoomGo2Position(INT32 position);
static INT32    lensLV8044_ZoomPress(UINT32 zoom);
static UINT32   lensLV8044_ZoomRelease(void);
static INT32    lensLV8044_ZoomRetract(void);
static INT32    lensLV8044_FocusInit(void);
static UINT32   lensLV8044_FocusGetSpeed(void);
static void     lensLV8044_FocusSetSpeed(LENS_SPEED_CATEGORY category);
static UINT32   lensLV8044_FocusGetFocalLength(UINT32 zoom_section);
static void     lensLV8044_FocusSetFocalLength(UINT32 zoom_section, UINT32 focal_length);
static INT32    lensLV8044_FocusGetDefultTable(UINT32 section, UINT32 focus_distance);
static INT32    lensLV8044_FocusGetTable(UINT32 section, UINT32 focus_distance);
static INT32    lensLV8044_FocusGetRange(FOCUS_RANGE range);
static INT32    lensLV8044_FocusGetPosition(void);
static void     lensLV8044_FocusSetPosition(INT32 position, INT32 dir);
static INT32    lensLV8044_FocusGo2Position(INT32 position, BOOL limit_range);
static INT32    lensLV8044_FocusPress(UINT32 focus);
static UINT32   lensLV8044_FocusRelease(void);
static INT32    lensLV8044_FocusRetract(FOCUS_RETRACT_POSITION position);
static void     lensLV8044_ApertureInit(void);
static UINT32   lensLV8044_ApertureGetFNo(UINT32 zoom_section, UINT32 iris_pos);
static IRIS_POS lensLV8044_ApertureGetPosition(void);
static void     lensLV8044_ApertureGo2Position(IRIS_POS position);
static void     lensLV8044_ApertureSetLumFlux(UINT32 flux);
static void     lensLV8044_ShutterSetState(MOTOR_SHUTTER_ACT state, UINT32 param);
static void     lensLV8044_IrcutSetState(MOTOR_IRCUT_ACT state, UINT32 param);
static INT32    lensLV8044_LensRetract(void);
static INT32    lensLV8044_LensReset(void);
static INT32    lensLV8044_LensPowerOff(void);
static UINT32   lensLV8044_LensGetCapInfo(void);
static void     lensLV8044_MakeAFTable(void);
static INT32    lensLV8044_SetCaliData(INT16 *pcal_tbl, UINT32 tbl_size);
static UINT32   lensLV8044_GetZoomFocusTable(UINT32 tbl_idx, LENS_ZOOM_FOCUS_TAB *zf_tbl);
static UINT32   lensLV8044_SetZoomFocusTable(UINT32 tbl_idx, LENS_ZOOM_FOCUS_TAB *zf_tbl);


static LENSDRV_INFO g_LensDrvLv8044 =
{
    .LensDrv.init                    = lensLV8044_Init,
    .LensDrv.getDriverName           = lensLV8044_GetDriverName,
    .LensDrv.getSignal               = lensLV8044_GetSignal,
    .LensDrv.setSignal               = lensLV8044_SetSignal,
    .LensDrv.getInitState            = lensLV8044_GetInitState,
    .LensDrv.getBusyState            = lensLV8044_GetBusyState,

    .LensDrv.zoom_init               = lensLV8044_ZoomInit,
    .LensDrv.zoom_getSpeed           = lensLV8044_ZoomGetSpeed,
    .LensDrv.zoom_setSpeed           = lensLV8044_ZoomSetSpeed,
    .LensDrv.zoom_getMinSection      = lensLV8044_ZoomGetMinSection,
    .LensDrv.zoom_getMaxSection      = lensLV8044_ZoomGetMaxSection,
    .LensDrv.zoom_getSection         = lensLV8044_ZoomGetSection,
    .LensDrv.zoom_getSectionPosition = lensLV8044_ZoomGetSectionPosition,
    .LensDrv.zoom_getPosition        = lensLV8044_ZoomGetPosition,
    .LensDrv.zoom_setPostion         = lensLV8044_ZoomSetPosition,
    .LensDrv.zoom_go2Section         = lensLV8044_ZoomGo2Section,
    .LensDrv.zoom_go2Position        = lensLV8044_ZoomGo2Position,
    .LensDrv.zoom_press              = lensLV8044_ZoomPress,
    .LensDrv.zoom_release            = lensLV8044_ZoomRelease,
    .LensDrv.zoom_retract            = lensLV8044_ZoomRetract,

    .LensDrv.focus_init              = lensLV8044_FocusInit,
    .LensDrv.focus_getSpeed          = lensLV8044_FocusGetSpeed,
    .LensDrv.focus_setSpeed          = lensLV8044_FocusSetSpeed,
    .LensDrv.focus_getFocalLength    = lensLV8044_FocusGetFocalLength,
    .LensDrv.focus_setFocalLength    = lensLV8044_FocusSetFocalLength,
    .LensDrv.focus_getDefaultTable   = lensLV8044_FocusGetDefultTable,
    .LensDrv.focus_getTable          = lensLV8044_FocusGetTable,
    .LensDrv.focus_getRange          = lensLV8044_FocusGetRange,
    .LensDrv.focus_getPostion        = lensLV8044_FocusGetPosition,
    .LensDrv.focus_setPostion        = lensLV8044_FocusSetPosition,
    .LensDrv.focus_go2Position       = lensLV8044_FocusGo2Position,
    .LensDrv.focus_press             = lensLV8044_FocusPress,
    .LensDrv.focus_release           = lensLV8044_FocusRelease,
    .LensDrv.focus_retract           = lensLV8044_FocusRetract,

    .LensDrv.aperture_init           = lensLV8044_ApertureInit,
    .LensDrv.aperture_getFNo         = lensLV8044_ApertureGetFNo,
    .LensDrv.aperture_getPosition    = lensLV8044_ApertureGetPosition,
    .LensDrv.aperture_go2Position    = lensLV8044_ApertureGo2Position,
    .LensDrv.aperture_setLumFlux     = lensLV8044_ApertureSetLumFlux,

    .LensDrv.shutter_setState        = lensLV8044_ShutterSetState,

    .LensDrv.ircut_setState          = lensLV8044_IrcutSetState,

    .LensDrv.lensRetract             = lensLV8044_LensRetract,
    .LensDrv.lensReset               = lensLV8044_LensReset,
    .LensDrv.lensPowerOff            = lensLV8044_LensPowerOff,
    .LensDrv.lensGetCapInfo          = lensLV8044_LensGetCapInfo,
    .LensDrv.makeAFTable             = lensLV8044_MakeAFTable,
    .LensDrv.setCaliData             = lensLV8044_SetCaliData,
    .LensDrv.getZoomFocusTable       = lensLV8044_GetZoomFocusTable,
    .LensDrv.setZoomFocusTable       = lensLV8044_SetZoomFocusTable,

    .LensDrv.pMotor       = NULL,
    .LensDrv.pLensCtrlTsk = NULL,

    .LensCtrlTsk = { NULL },

    .uiFocalLengthMin = LENS_FOCAL_LENGTH_WIDE,    // unit: um
    .uiFocalLengthMax = LENS_FOCAL_LENGTH_TELE,    // unit: um

    .uiZoomSecPos   = ZOOM_SECTION_WIDE,
    .iFocusCaliOfst = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    .eAperturePos   = IRIS_POS_CLOSE,
    .eShutterState  = MOTOR_SHUTTER_NORMAL,
    .eIrCutState    = MOTOR_IRCUT_OPEN,
};

const UINT32 g_uiFNumMappLv8044[] = { 16, 20, 24, 28, 34, 40, 48, 56, 68, 96, 110, 140, 160, 192, 220 };

UINT32 g_uiLv8044SpiSema = 0;


void LensDrv_InstallID(void)
{
    OS_CONFIG_SEMPHORE(g_uiLv8044SpiSema, 0, 1, 1);
}


//----------------------------------------------------------------------------------------------------------------------
// general
//----------------------------------------------------------------------------------------------------------------------
PLENS_TAB DrvLens_GetLens(void)
{
    g_LensDrvLv8044.LensDrv.pMotor = DrvLens_GetMotor();
    g_LensDrvLv8044.LensDrv.pLensCtrlTsk = &g_LensDrvLv8044.LensCtrlTsk;

    return &g_LensDrvLv8044.LensDrv;
}


static void lensLV8044_Init(PLENS_DEVICE_OBJ plens_dev)
{
    DBG_DUMP("%s %s\r\n", LENS_DRV_NAME, LENS_DRV_ID);
}


static void lensLV8044_GetDriverName(INT8 *ic_name)
{
    memcpy(ic_name, LENS_DRV_NAME, sizeof(LENS_DRV_NAME));
    ic_name[sizeof(LENS_DRV_NAME)] = '\0';
}


static UINT32 lensLV8044_GetSignal(MD_SIGNAL signal)
{
    return g_LensDrvLv8044.LensDrv.pMotor->getSignal(signal);
}


static void lensLV8044_SetSignal(MD_SIGNAL signal, UINT32 on)
{
    g_LensDrvLv8044.LensDrv.pMotor->setSignal(signal, on);
}


static UINT32 lensLV8044_GetInitState(void)
{
    return g_LensDrvLv8044.LensDrv.pMotor->getInitState();
}


static UINT32 lensLV8044_GetBusyState(void)
{
    return g_LensDrvLv8044.LensDrv.pMotor->getBusyState();
}


//----------------------------------------------------------------------------------------------------------------------
// zoom
//----------------------------------------------------------------------------------------------------------------------
static UINT32 LV8044_CalcFocusRange(INT32 *pmin_value, INT32 *pmax_value, INT32 zoom_pos, BOOL limit_range)
{
    UINT32 i = ZOOM_SECTION_WIDE;
    INT32 tmp, fmin, fmax;
    LENSMOTOR_INFO *pmotor_info = LV8044_GetMotorInfo();

    if (limit_range == TRUE) {
        fmin = pmotor_info->ZoomFocusTbl[ZOOM_SECTION_WIDE].iFocusMin;
        fmax = pmotor_info->ZoomFocusTbl[ZOOM_SECTION_WIDE].iFocusMax;

        zoom_pos = (zoom_pos < 0) ? pmotor_info->LensMotorConfig[1].iCurrPosition : zoom_pos;

        for (i = ZOOM_SECTION_WIDE; i < ZOOM_SECTION_NUM; i++) {
            if (pmotor_info->ZoomFocusTbl[i].iZoomPosition >= zoom_pos) {
                tmp = (i != ZOOM_SECTION_WIDE) ? (pmotor_info->ZoomFocusTbl[i].iZoomPosition - pmotor_info->ZoomFocusTbl[i-1].iZoomPosition) : 1;

                if (tmp != 0) {
                    fmin = (i != ZOOM_SECTION_WIDE) ? (pmotor_info->ZoomFocusTbl[i-1].iFocusMin
                         + (pmotor_info->ZoomFocusTbl[i].iFocusMin - pmotor_info->ZoomFocusTbl[i-1].iFocusMin) * (zoom_pos - pmotor_info->ZoomFocusTbl[i-1].iZoomPosition) / tmp) : fmin;
                    fmax = (i != ZOOM_SECTION_WIDE) ? (pmotor_info->ZoomFocusTbl[i-1].iFocusMax
                         + (pmotor_info->ZoomFocusTbl[i].iFocusMax - pmotor_info->ZoomFocusTbl[i-1].iFocusMax) * (zoom_pos - pmotor_info->ZoomFocusTbl[i-1].iZoomPosition) / tmp) : fmax;
                } else {
                    DBG_ERR("incorrect pmotor_info->ZoomFocusTbl[]!\r\n");
                }
                break;
            }
        }
    } else {
        fmin = pmotor_info->ZoomFocusTbl[ZOOM_SECTION_WIDE].iFocusMin;
        fmax = pmotor_info->ZoomFocusTbl[ZOOM_SECTION_TELE].iFocusMax;
    }

    *pmin_value = fmin;
    *pmax_value = fmax;

    return i;
}


static void LV8044_UpdateFocusPosition(LENSMOTOR_INFO *pmotor_info)
{
    UINT32 cmd_param = 0;
    INT32 set_step, focus_pos, near_bound, inf_bound;

    LV8044_CalcFocusRange(&near_bound, &inf_bound, -1, TRUE);

    if ((pmotor_info->LensMotorConfig[0].iCurrPosition < near_bound) || (pmotor_info->LensMotorConfig[0].iCurrPosition > inf_bound)) {
        focus_pos = (near_bound + inf_bound) / 2;

        if (focus_pos != pmotor_info->LensMotorConfig[0].iCurrPosition) {
            LV8044_TransmitCmd(pmotor_info->LensMotorConfig[0].eChnelSel, LV8044_CMD_SET_CURR, 0);
            LENS_DELAY_MS(2);    // add delay for motor power high ready

            if (focus_pos > pmotor_info->LensMotorConfig[0].iCurrPosition) {
                set_step = focus_pos - pmotor_info->LensMotorConfig[0].iCurrPosition;

                cmd_param = (pmotor_info->LensMotorConfig[0].iMoveDir != MOVE_DIR_FWD) ? ((pmotor_info->LensMotorConfig[0].iBacklashValue << 16) + set_step) : ((0 << 16) + set_step);
                pmotor_info->LensMotorConfig[0].bSetFinalStep = TRUE;
                g_LensDrvLv8044.LensDrv.pMotor->focus_setState(MOTOR_FOCUS_FWD, cmd_param);
            } else {
                set_step = pmotor_info->LensMotorConfig[0].iCurrPosition - focus_pos;

                cmd_param = (pmotor_info->LensMotorConfig[0].iMoveDir != MOVE_DIR_BWD) ? ((pmotor_info->LensMotorConfig[0].iBacklashValue << 16) + set_step) : ((0 << 16) + set_step);
                pmotor_info->LensMotorConfig[0].bSetFinalStep = TRUE;
                g_LensDrvLv8044.LensDrv.pMotor->focus_setState(MOTOR_FOCUS_BWD, cmd_param);
            }
        }
    }
}


static INT32 lensLV8044_ZoomInit(UINT32 stage)
{
    UINT32 cmd_param = 0, wait_cnt = 0;
    INT32 set_step;
    LENSMOTOR_INFO *pmotor_info = LV8044_GetMotorInfo();

    if (g_LensDrvLv8044.LensDrv.pMotor->zoom_setState == NULL) {
        DBG_ERR("%s zoom_setState = NULL!\r\n", __func__);

        return ERR_OPERATION_FAIL;
    }

    pmotor_info->LensMotorConfig[1].bMove2Home = FALSE;

    LV8044_TransmitCmd(pmotor_info->LensMotorConfig[1].eChnelSel, LV8044_CMD_SET_CURR, 0);
    LENS_DELAY_MS(2);    // add delay for motor power high ready

    set_step = pmotor_info->ZoomFocusTbl[ZOOM_SECTION_TELE].iZoomPosition + INIT_ZOOM_POS
             + pmotor_info->LensMotorConfig[1].iInitPosBias + (TELE_BOUND/10);

    cmd_param = (0 << 16) + set_step;
    g_LensDrvLv8044.LensDrv.pMotor->zoom_setState(MOTOR_ZOOM_BWD, cmd_param);

#if 1    // increase go home consistency
    LV8044_TransmitCmd(pmotor_info->LensMotorConfig[1].eChnelSel, LV8044_CMD_ENABLE, 0);    // release motor excitation

    LENS_DELAY_MS(500);

    // tighten motor and gear
    LV8044_TransmitCmd(pmotor_info->LensMotorConfig[1].eChnelSel, LV8044_CMD_ENABLE, 1);    // tighten motor and gear

    set_step = TELE_BOUND / 20;

    cmd_param = (0 << 16) + set_step;
    g_LensDrvLv8044.LensDrv.pMotor->zoom_setState(MOTOR_ZOOM_BWD, cmd_param);
#endif

    set_step = LENS_MAX(INIT_ZOOM_POS+pmotor_info->ZoomFocusTbl[ZOOM_SECTION_WIDE].iZoomPosition+pmotor_info->LensMotorConfig[1].iInitPosBias, 0);

    cmd_param = (pmotor_info->LensMotorConfig[1].iMoveDir != MOVE_DIR_FWD) ? ((pmotor_info->LensMotorConfig[1].iBacklashValue << 16) + set_step) : ((0 << 16) + set_step);
    pmotor_info->LensMotorConfig[1].bSetFinalStep = TRUE;
    g_LensDrvLv8044.LensDrv.pMotor->zoom_setState(MOTOR_ZOOM_FWD, cmd_param);

    pmotor_info->LensMotorConfig[1].iCurrPosition = pmotor_info->ZoomFocusTbl[ZOOM_SECTION_WIDE].iZoomPosition;

    // wait for focus move finish
    while ((pmotor_info->LensMotorConfig[0].iWorkStatus == MOTOR_STATUS_BUSY) && (wait_cnt++ < 400)) {
        LENS_DELAY_MS(5);
    }

    LV8044_UpdateFocusPosition(pmotor_info);

    pmotor_info->LensMotorConfig[1].bMove2Home = TRUE;

    return ERR_OK;
}

static UINT32 lensLV8044_ZoomGetSpeed(void)
{
    return g_LensDrvLv8044.LensDrv.pMotor->zoom_getSpeed();
}

static void lensLV8044_ZoomSetSpeed(LENS_SPEED_CATEGORY category)
{
    UINT32 curr_speed = 0, zoom_speed = 20;

    if (g_LensDrvLv8044.LensDrv.pMotor->zoom_getSpeed != NULL) {
        curr_speed = g_LensDrvLv8044.LensDrv.pMotor->zoom_getSpeed();
    } else {
        DBG_ERR("%s zoom_getSpeed = NULL!\r\n", __func__);

        return;
    }

    if (category == LENS_SPEED_VERY_LOW) {
        zoom_speed = 20;
    } else if (category == LENS_SPEED_LOW) {
        zoom_speed = 40;
    } else if (category == LENS_SPEED_MEDIUM) {
        zoom_speed = 60;
    } else if (category == LENS_SPEED_HIGH) {
        zoom_speed = 80;
    } else if (category == LENS_SPEED_VERY_HIGH) {
        zoom_speed = 100;
    }

    if (curr_speed != zoom_speed) {
        if (g_LensDrvLv8044.LensDrv.pMotor->zoom_setState != NULL) {
            g_LensDrvLv8044.LensDrv.pMotor->zoom_setState(MOTOR_ZOOM_SETSPEED, zoom_speed);
        }
    } else {
        DBG_MSG("set current zoom speed is the same!\r\n");
    }
}


static UINT32 lensLV8044_ZoomGetMinSection(void)
{
    return ZOOM_SECTION_WIDE;
}


static UINT32 lensLV8044_ZoomGetMaxSection(void)
{
    return ZOOM_SECTION_TELE;
}


static UINT32 LV8044_ZoomSearchSection(void)
{
    UINT32 sec_pos;
    INT32 sec_zone_pts;
    LENSMOTOR_INFO *pmotor_info = LV8044_GetMotorInfo();

    for (sec_pos = ZOOM_SECTION_WIDE; sec_pos < ZOOM_SECTION_TELE; sec_pos++) {
        sec_zone_pts = (pmotor_info->ZoomFocusTbl[sec_pos].iZoomPosition + pmotor_info->ZoomFocusTbl[sec_pos+1].iZoomPosition) / 2;

        if (pmotor_info->LensMotorConfig[1].iCurrPosition <= sec_zone_pts) {
            break;
        }
    }

    sec_pos = LENS_CLAMP(sec_pos, ZOOM_SECTION_WIDE, ZOOM_SECTION_TELE);

    return sec_pos;
}


static UINT32 lensLV8044_ZoomGetSection(void)
{
    return g_LensDrvLv8044.uiZoomSecPos;
}


static INT32 lensLV8044_ZoomGetSectionPosition(void)
{
    LENSMOTOR_INFO *pmotor_info = LV8044_GetMotorInfo();

    return LENS_MAX(pmotor_info->ZoomFocusTbl[g_LensDrvLv8044.uiZoomSecPos].iZoomPosition, 0);
}


static INT32 lensLV8044_ZoomGetPosition(void)
{
    LENSMOTOR_INFO *pmotor_info = LV8044_GetMotorInfo();

    return (pmotor_info->LensMotorConfig[1].iMoveDir << 16) | pmotor_info->LensMotorConfig[1].iCurrPosition;
}


static void lensLV8044_ZoomSetPosition(INT32 position, INT32 dir)
{
    LENSMOTOR_INFO *pmotor_info = LV8044_GetMotorInfo();

    pmotor_info->LensMotorConfig[1].iCurrPosition = position;
    pmotor_info->LensMotorConfig[1].iMoveDir = (dir >= 0) ? MOVE_DIR_FWD : MOVE_DIR_BWD;

    g_LensDrvLv8044.uiZoomSecPos = LV8044_ZoomSearchSection();

    pmotor_info->LensMotorConfig[1].bMove2Home = TRUE;
}


static INT32 lensLV8044_ZoomGo2Section(UINT32 section)
{
    INT32 zset_step, fset_step, zrest_step, acc_step = 0;
    INT32 zoom_tar_pos, focus_tar_pos, focus_ofst, near_bound, inf_bound, zstep_size;

    LENSMOTOR_INFO *pmotor_info = LV8044_GetMotorInfo();

    section = LENS_CLAMP(section, ZOOM_SECTION_WIDE, ZOOM_SECTION_TELE);
    zoom_tar_pos = pmotor_info->ZoomFocusTbl[section].iZoomPosition;

    LV8044_CalcFocusRange(&near_bound, &inf_bound, -1, TRUE);

    focus_ofst = LENS_MAX((pmotor_info->LensMotorConfig[0].iCurrPosition - near_bound), 0);
    zstep_size = LENS_MIN(4, pmotor_info->LensMotorConfig[1].iMaxStepSize);

    if (pmotor_info->LensMotorConfig[1].bMove2Home == FALSE) {
        DBG_ERR("%s not initialized yet!\r\n", __func__);

        return 0;
    }

    if (g_LensDrvLv8044.LensDrv.pMotor->zoom_setState == NULL) {
        DBG_ERR("%s zoom_setState = NULL!\r\n", __func__);

        return 0;
    }

    if (zoom_tar_pos != pmotor_info->LensMotorConfig[1].iCurrPosition) {
        LV8044_TransmitCmd(pmotor_info->LensMotorConfig[1].eChnelSel, LV8044_CMD_ENABLE, 1);
        LV8044_TransmitCmd(pmotor_info->LensMotorConfig[0].eChnelSel, LV8044_CMD_ENABLE, 1);
        LENS_DELAY_MS(16);    // add delay for motor power on ready

        if (zoom_tar_pos > pmotor_info->LensMotorConfig[1].iCurrPosition) {
            // compensate zoom & focus backlash first
            if ((pmotor_info->LensMotorConfig[1].iBacklashValue != 0) || (pmotor_info->LensMotorConfig[0].iBacklashValue != 0)) {
                zset_step = (pmotor_info->LensMotorConfig[1].iMoveDir != MOVE_DIR_FWD) ? pmotor_info->LensMotorConfig[1].iBacklashValue : 0;
                fset_step = (pmotor_info->LensMotorConfig[0].iMoveDir != MOVE_DIR_FWD) ? pmotor_info->LensMotorConfig[0].iBacklashValue : 0;

                g_LensDrvLv8044.LensDrv.pMotor->focus_zoom_setMove(TRUE, 0, fset_step, TRUE, 0, zset_step);
            }

            zrest_step = zoom_tar_pos - pmotor_info->LensMotorConfig[1].iCurrPosition;

            do {
                zset_step = (zrest_step > zstep_size) ? zstep_size : zrest_step;
                zrest_step -= zset_step;
                acc_step += zset_step;

                if ((zrest_step <= 0) || (pmotor_info->LensMotorConfig[1].bSetMove2Stop == TRUE)) {
                    zrest_step = 0;
                    pmotor_info->LensMotorConfig[1].bSetFinalStep = pmotor_info->LensMotorConfig[0].bSetFinalStep = TRUE;
                }

                LV8044_CalcFocusRange(&near_bound, &inf_bound, (pmotor_info->LensMotorConfig[1].iCurrPosition + zset_step), TRUE);
                focus_tar_pos = LENS_CLAMP((near_bound + focus_ofst), near_bound, inf_bound);
                fset_step = (focus_tar_pos >= pmotor_info->LensMotorConfig[0].iCurrPosition) ? (focus_tar_pos - pmotor_info->LensMotorConfig[0].iCurrPosition) : 0;

                g_LensDrvLv8044.LensDrv.pMotor->focus_zoom_setMove(TRUE, fset_step, 0, TRUE, zset_step, 0);
            } while (zrest_step > 0);
        } else if (zoom_tar_pos < pmotor_info->LensMotorConfig[1].iCurrPosition) {
            // compensate zoom & focus backlash first
            if ((pmotor_info->LensMotorConfig[1].iBacklashValue != 0) || (pmotor_info->LensMotorConfig[0].iBacklashValue != 0)) {
                zset_step = (pmotor_info->LensMotorConfig[1].iMoveDir != MOVE_DIR_BWD) ? pmotor_info->LensMotorConfig[1].iBacklashValue : 0;
                fset_step = (pmotor_info->LensMotorConfig[0].iMoveDir != MOVE_DIR_BWD) ? pmotor_info->LensMotorConfig[0].iBacklashValue : 0;

                g_LensDrvLv8044.LensDrv.pMotor->focus_zoom_setMove(FALSE, 0, fset_step, FALSE, 0, zset_step);
            }

            zrest_step = pmotor_info->LensMotorConfig[1].iCurrPosition - zoom_tar_pos;

            do {
                zset_step = (zrest_step > zstep_size) ? zstep_size : zrest_step;
                zrest_step -= zset_step;
                acc_step += zset_step;

                if ((zrest_step <= 0) || (pmotor_info->LensMotorConfig[1].bSetMove2Stop == TRUE)) {
                    zrest_step = 0;
                    pmotor_info->LensMotorConfig[1].bSetFinalStep = pmotor_info->LensMotorConfig[0].bSetFinalStep = TRUE;
                }

                LV8044_CalcFocusRange(&near_bound, &inf_bound, (pmotor_info->LensMotorConfig[1].iCurrPosition - zset_step), TRUE);
                focus_tar_pos = LENS_CLAMP((near_bound + focus_ofst), near_bound, inf_bound);
                fset_step = (pmotor_info->LensMotorConfig[0].iCurrPosition >= focus_tar_pos) ? (pmotor_info->LensMotorConfig[0].iCurrPosition - focus_tar_pos) : 0;

                g_LensDrvLv8044.LensDrv.pMotor->focus_zoom_setMove(FALSE, fset_step, 0, FALSE, zset_step, 0);
            } while (zrest_step > 0);
        }

        g_LensDrvLv8044.uiZoomSecPos = section;
    } else {
        DBG_MSG("the same with current zoom position!\r\n");
    }

    return acc_step;
}


static INT32 lensLV8044_ZoomGo2Position(INT32 position)
{
    INT32 zset_step, fset_step, zrest_step, acc_step = 0;
    INT32 focus_tar_pos, focus_ofst, near_bound, inf_bound, zstep_size;
    LENSMOTOR_INFO *pmotor_info = LV8044_GetMotorInfo();

    if (pmotor_info->LensMotorConfig[1].bMove2Home == FALSE) {
        DBG_ERR("%s not initialized yet!\r\n", __func__);

        return 0;
    }

    if (g_LensDrvLv8044.LensDrv.pMotor->zoom_setState == NULL) {
        DBG_ERR("%s zoom_setState = NULL!\r\n", __func__);

        return 0;
    }

    LV8044_CalcFocusRange(&near_bound, &inf_bound, -1, TRUE);

    focus_ofst = LENS_MAX((pmotor_info->LensMotorConfig[0].iCurrPosition - near_bound), 0);
    zstep_size = LENS_MIN(4, pmotor_info->LensMotorConfig[1].iMaxStepSize);

    if ((position >= pmotor_info->ZoomFocusTbl[ZOOM_SECTION_WIDE].iZoomPosition) && (position <= pmotor_info->ZoomFocusTbl[ZOOM_SECTION_TELE].iZoomPosition)) {
        if (position != pmotor_info->LensMotorConfig[1].iCurrPosition) {
            LV8044_TransmitCmd(pmotor_info->LensMotorConfig[1].eChnelSel, LV8044_CMD_ENABLE, 1);
            LV8044_TransmitCmd(pmotor_info->LensMotorConfig[0].eChnelSel, LV8044_CMD_ENABLE, 1);
            LENS_DELAY_MS(16);    // add delay for motor power on ready
        }

        pmotor_info->LensMotorConfig[1].bSetMove2Stop = FALSE;

        if (position > pmotor_info->LensMotorConfig[1].iCurrPosition) {
            // compensate zoom & focus backlash first
            if ((pmotor_info->LensMotorConfig[1].iBacklashValue != 0) || (pmotor_info->LensMotorConfig[0].iBacklashValue != 0)) {
                zset_step = (pmotor_info->LensMotorConfig[1].iMoveDir != MOVE_DIR_FWD) ? pmotor_info->LensMotorConfig[1].iBacklashValue : 0;
                fset_step = (pmotor_info->LensMotorConfig[0].iMoveDir != MOVE_DIR_FWD) ? pmotor_info->LensMotorConfig[0].iBacklashValue : 0;

                g_LensDrvLv8044.LensDrv.pMotor->focus_zoom_setMove(TRUE, 0, fset_step, TRUE, 0, zset_step);
            }

            zrest_step = position - pmotor_info->LensMotorConfig[1].iCurrPosition;

            do {
                zset_step = (zrest_step > zstep_size) ? zstep_size : zrest_step;
                zrest_step -= zset_step;
                acc_step += zset_step;

                if ((zrest_step <= 0) || (pmotor_info->LensMotorConfig[1].bSetMove2Stop == TRUE)) {
                    zrest_step = 0;
                    pmotor_info->LensMotorConfig[1].bSetFinalStep = pmotor_info->LensMotorConfig[0].bSetFinalStep = TRUE;
                }

                LV8044_CalcFocusRange(&near_bound, &inf_bound, (pmotor_info->LensMotorConfig[1].iCurrPosition + zset_step), TRUE);
                focus_tar_pos = LENS_MIN((near_bound + focus_ofst), inf_bound);

                g_LensDrvLv8044.LensDrv.pMotor->focus_zoom_setMove(TRUE, (focus_tar_pos - pmotor_info->LensMotorConfig[0].iCurrPosition), 0, TRUE, zset_step, 0);
            } while (zrest_step > 0);
        } else if (position < pmotor_info->LensMotorConfig[1].iCurrPosition) {
            // compensate zoom & focus backlash first
            if ((pmotor_info->LensMotorConfig[1].iBacklashValue != 0) || (pmotor_info->LensMotorConfig[0].iBacklashValue != 0)) {
                zset_step = (pmotor_info->LensMotorConfig[1].iMoveDir != MOVE_DIR_BWD) ? pmotor_info->LensMotorConfig[1].iBacklashValue : 0;
                fset_step = (pmotor_info->LensMotorConfig[0].iMoveDir != MOVE_DIR_BWD) ? pmotor_info->LensMotorConfig[0].iBacklashValue : 0;

                g_LensDrvLv8044.LensDrv.pMotor->focus_zoom_setMove(FALSE, 0, fset_step, FALSE, 0, zset_step);
            }

            zrest_step = pmotor_info->LensMotorConfig[1].iCurrPosition - position;

            do {
                zset_step = (zrest_step > zstep_size) ? zstep_size : zrest_step;
                zrest_step -= zset_step;
                acc_step += zset_step;

                if ((zrest_step <= 0) || (pmotor_info->LensMotorConfig[1].bSetMove2Stop == TRUE)) {
                    zrest_step = 0;
                    pmotor_info->LensMotorConfig[1].bSetFinalStep = pmotor_info->LensMotorConfig[0].bSetFinalStep = TRUE;
                }

                LV8044_CalcFocusRange(&near_bound, &inf_bound, (pmotor_info->LensMotorConfig[1].iCurrPosition - zset_step), TRUE);
                focus_tar_pos = LENS_MIN((near_bound + focus_ofst), inf_bound);

                g_LensDrvLv8044.LensDrv.pMotor->focus_zoom_setMove(FALSE, (pmotor_info->LensMotorConfig[0].iCurrPosition - focus_tar_pos), 0, FALSE, zset_step, 0);
            } while (zrest_step > 0);

            g_LensDrvLv8044.uiZoomSecPos = LV8044_ZoomSearchSection();
        } else {
            DBG_MSG("%s step=0!\r\n", __func__);
        }
    } else {
        DBG_MSG("set zoom position is out of range!\r\n");
    }

    return acc_step;
}


static INT32 lensLV8044_ZoomPress(UINT32 zoom)
{
    INT32 target_pos;
    LENSMOTOR_INFO *pmotor_info = LV8044_GetMotorInfo();

    if (zoom == ZOOM_OUT) {
        target_pos = pmotor_info->ZoomFocusTbl[ZOOM_SECTION_WIDE].iZoomPosition;
    } else if (zoom == ZOOM_IN) {
        target_pos = pmotor_info->ZoomFocusTbl[ZOOM_SECTION_TELE].iZoomPosition;
    } else {
        DBG_MSG("%s out of move mode\r\n", __func__);

        return ERR_OPERATION_FAIL;
    }

    lensLV8044_ZoomGo2Position(target_pos);

    return ERR_OK;
}


static UINT32 lensLV8044_ZoomRelease(void)
{
    LENSMOTOR_INFO *pmotor_info = LV8044_GetMotorInfo();

    pmotor_info->LensMotorConfig[1].bSetMove2Stop = TRUE;

    return 0;
}


static INT32 lensLV8044_ZoomRetract(void)
{
    return ERR_OK;
}


//----------------------------------------------------------------------------------------------------------------------
// focus
//----------------------------------------------------------------------------------------------------------------------
static INT32 lensLV8044_FocusInit(void)
{
    UINT32 cmd_param = 0;
    INT32 set_step;
    LENSMOTOR_INFO *pmotor_info = LV8044_GetMotorInfo();

    if (g_LensDrvLv8044.LensDrv.pMotor->focus_setState == NULL) {
        DBG_ERR("%s focus_setState = NULL!\r\n", __func__);

        return ERR_OPERATION_FAIL;
    }

    pmotor_info->LensMotorConfig[0].bMove2Home = FALSE;

    LV8044_TransmitCmd(pmotor_info->LensMotorConfig[0].eChnelSel, LV8044_CMD_SET_CURR, 0);
    LENS_DELAY_MS(2);    // add delay for motor power high ready

    set_step = pmotor_info->ZoomFocusTbl[ZOOM_SECTION_TELE].iFocusMax + INIT_FOCUS_POS
             + pmotor_info->LensMotorConfig[0].iInitPosBias + (INF_BOUND/10);

    cmd_param = (0 << 16) + set_step;
    g_LensDrvLv8044.LensDrv.pMotor->focus_setState(MOTOR_FOCUS_BWD, cmd_param);

#if 1    // increase go home consistency
    LV8044_TransmitCmd(pmotor_info->LensMotorConfig[0].eChnelSel, LV8044_CMD_ENABLE, 0);    // release motor excitation

    LENS_DELAY_MS(500);

    // tighten motor and gear
    LV8044_TransmitCmd(pmotor_info->LensMotorConfig[0].eChnelSel, LV8044_CMD_ENABLE, 1);    // tighten motor and gear

    set_step = INF_BOUND / 20;

    cmd_param = (0 << 16) + set_step;
    g_LensDrvLv8044.LensDrv.pMotor->focus_setState(MOTOR_FOCUS_BWD, cmd_param);
#endif

    set_step = LENS_MAX(INIT_FOCUS_POS+pmotor_info->ZoomFocusTbl[ZOOM_SECTION_WIDE].iFocusMin+pmotor_info->LensMotorConfig[0].iInitPosBias, 0);

    cmd_param = (pmotor_info->LensMotorConfig[0].iMoveDir != MOVE_DIR_FWD) ? ((pmotor_info->LensMotorConfig[0].iBacklashValue << 16) + set_step) : ((0 << 16) + set_step);
    pmotor_info->LensMotorConfig[0].bSetFinalStep = TRUE;
    g_LensDrvLv8044.LensDrv.pMotor->focus_setState(MOTOR_FOCUS_FWD, cmd_param);

    pmotor_info->LensMotorConfig[0].iCurrPosition = pmotor_info->ZoomFocusTbl[ZOOM_SECTION_WIDE].iFocusMin;
    pmotor_info->LensMotorConfig[0].bMove2Home = TRUE;

    return ERR_OK;
}


static UINT32 lensLV8044_FocusGetSpeed(void)
{
    return g_LensDrvLv8044.LensDrv.pMotor->focus_getSpeed();
}


static void lensLV8044_FocusSetSpeed(LENS_SPEED_CATEGORY category)
{
    UINT32 curr_speed = 0, focus_speed = 20;

    if (g_LensDrvLv8044.LensDrv.pMotor->focus_getSpeed != NULL) {
        curr_speed = g_LensDrvLv8044.LensDrv.pMotor->focus_getSpeed();
    } else {
        DBG_ERR("%s focus_getSpeed = NULL!\r\n", __func__);

        return;
    }

    if (category == LENS_SPEED_VERY_LOW) {
        focus_speed = 20;
    } else if (category == LENS_SPEED_LOW) {
        focus_speed = 40;
    } else if (category == LENS_SPEED_MEDIUM) {
        focus_speed = 60;
    } else if (category == LENS_SPEED_HIGH) {
        focus_speed = 80;
    } else if (category == LENS_SPEED_VERY_HIGH) {
        focus_speed = 100;
    }

    if (curr_speed != focus_speed) {
        if (g_LensDrvLv8044.LensDrv.pMotor->focus_setState != NULL) {
            g_LensDrvLv8044.LensDrv.pMotor->focus_setState(MOTOR_FOCUS_SETSPEED, focus_speed);
        }
    } else {
        DBG_MSG("set current focus speed is the same!\r\n");
    }
}


static UINT32 lensLV8044_FocusGetFocalLength(UINT32 zoom_section)
{
    UINT32 focal_length;

    zoom_section = LENS_CLAMP(zoom_section, ZOOM_SECTION_WIDE, ZOOM_SECTION_TELE);

    if (zoom_section > ZOOM_SECTION_WIDE) {
        focal_length = g_LensDrvLv8044.uiFocalLengthMin + (zoom_section - ZOOM_SECTION_WIDE)
                    * (g_LensDrvLv8044.uiFocalLengthMax - g_LensDrvLv8044.uiFocalLengthMin + (ZOOM_SECTION_TELE - ZOOM_SECTION_WIDE) / 2) / (ZOOM_SECTION_TELE - ZOOM_SECTION_WIDE);
    } else {
        focal_length = g_LensDrvLv8044.uiFocalLengthMin;
    }

    return focal_length;
}


static void lensLV8044_FocusSetFocalLength(UINT32 zoom_section, UINT32 focal_length)
{
    zoom_section = LENS_CLAMP(zoom_section, ZOOM_SECTION_WIDE, ZOOM_SECTION_TELE);

    if (zoom_section == ZOOM_SECTION_WIDE) {
        g_LensDrvLv8044.uiFocalLengthMin = LENS_CLAMP(focal_length, LENS_FOCAL_LENGTH_WIDE, g_LensDrvLv8044.uiFocalLengthMax);
    } else if (zoom_section == ZOOM_SECTION_TELE) {
        g_LensDrvLv8044.uiFocalLengthMax = LENS_CLAMP(focal_length, g_LensDrvLv8044.uiFocalLengthMin, LENS_FOCAL_LENGTH_TELE);
    }
}


static INT32 lensLV8044_FocusGetDefultTable(UINT32 section, UINT32 focus_distance)
{
    LENSMOTOR_INFO *pmotor_info = LV8044_GetMotorInfo();

    section = LENS_CLAMP(section, ZOOM_SECTION_WIDE, ZOOM_SECTION_TELE);
    focus_distance = LENS_MIN(focus_distance, FOCUS_DISTANCE_TABLE_NUM-1);

    return pmotor_info->FocusDistanceTbl[section].iLensPosition[focus_distance];
}


static INT32 lensLV8044_FocusGetTable(UINT32 section, UINT32 focus_distance)
{
    LENSMOTOR_INFO *pmotor_info = LV8044_GetMotorInfo();

    section = LENS_CLAMP(section, ZOOM_SECTION_WIDE, ZOOM_SECTION_TELE);
    focus_distance = LENS_MIN(focus_distance, FOCUS_DISTANCE_TABLE_NUM-1);

    return (pmotor_info->FocusDistanceTbl[section].iLensPosition[focus_distance] + g_LensDrvLv8044.iFocusCaliOfst[section]);
}


static INT32 lensLV8044_FocusGetRange(FOCUS_RANGE range)
{
    INT32 low_bound, high_bound, ret_value = 0;

    LV8044_CalcFocusRange(&low_bound, &high_bound, -1, TRUE);

    if (range == FOCUS_RANGE_START) {
        ret_value = low_bound;
    } else if (range == FOCUS_RANGE_END) {
        ret_value = high_bound;
    } else if (range == FOCUS_RANGE_BOTH) {
        ret_value = (high_bound << 16) + low_bound;
    }

    return ret_value;
}


static INT32 lensLV8044_FocusGetPosition(void)
{
    LENSMOTOR_INFO *pmotor_info = LV8044_GetMotorInfo();

    return (pmotor_info->LensMotorConfig[0].iMoveDir << 16) | pmotor_info->LensMotorConfig[0].iCurrPosition;
}


static void lensLV8044_FocusSetPosition(INT32 position, INT32 dir)
{
    LENSMOTOR_INFO *pmotor_info = LV8044_GetMotorInfo();

    pmotor_info->LensMotorConfig[0].iCurrPosition = position;
    pmotor_info->LensMotorConfig[0].iMoveDir = (dir >= 0) ? MOVE_DIR_FWD : MOVE_DIR_BWD;

    pmotor_info->LensMotorConfig[0].bMove2Home = TRUE;
}


static INT32 lensLV8044_FocusGo2Position(INT32 position, BOOL limit_range)
{
    UINT32 cmd_param = 0;
    INT32 set_step, rest_step, acc_step = 0, near_bound, inf_bound;
    LENSMOTOR_INFO *pmotor_info = LV8044_GetMotorInfo();

    if (pmotor_info->LensMotorConfig[0].bMove2Home == FALSE) {
        DBG_ERR("%s not initialized yet!\r\n", __func__);

        return 0;
    }

    if (g_LensDrvLv8044.LensDrv.pMotor->focus_setState == NULL) {
        DBG_ERR("%s focus_setState = NULL!\r\n", __func__);

        return 0;
    }

    LV8044_CalcFocusRange(&near_bound, &inf_bound, -1, limit_range);

    if ((position >= near_bound) && (position <= inf_bound)) {
        if (position != pmotor_info->LensMotorConfig[0].iCurrPosition) {
            LV8044_TransmitCmd(pmotor_info->LensMotorConfig[0].eChnelSel, LV8044_CMD_SET_CURR, 0);
            LENS_DELAY_MS(2);    // add delay for motor power high ready
        }

        pmotor_info->LensMotorConfig[0].bSetMove2Stop = FALSE;

        if (position > pmotor_info->LensMotorConfig[0].iCurrPosition) {
            // compensate backlash first
            if ((pmotor_info->LensMotorConfig[0].iBacklashValue != 0) && (pmotor_info->LensMotorConfig[0].iMoveDir != MOVE_DIR_FWD)) {
                cmd_param = (pmotor_info->LensMotorConfig[0].iBacklashValue << 16) + 0;
                g_LensDrvLv8044.LensDrv.pMotor->focus_setState(MOTOR_FOCUS_FWD, cmd_param);
            }

            rest_step = position - pmotor_info->LensMotorConfig[0].iCurrPosition;

            do {
                set_step = (rest_step > pmotor_info->LensMotorConfig[0].iMaxStepSize)
                          ? pmotor_info->LensMotorConfig[0].iMaxStepSize : rest_step;
                cmd_param = (0 << 16) + set_step;
                rest_step -= set_step;
                acc_step += set_step;

                if ((rest_step <= 0) || (pmotor_info->LensMotorConfig[0].bSetMove2Stop == TRUE)) {
                    rest_step = 0;
                    pmotor_info->LensMotorConfig[0].bSetFinalStep = TRUE;
                }

                g_LensDrvLv8044.LensDrv.pMotor->focus_setState(MOTOR_FOCUS_FWD, cmd_param);
            } while (rest_step > 0);
        } else if (position < pmotor_info->LensMotorConfig[0].iCurrPosition) {
            // compensate backlash first
            if ((pmotor_info->LensMotorConfig[0].iBacklashValue != 0) && (pmotor_info->LensMotorConfig[0].iMoveDir != MOVE_DIR_BWD)) {
                cmd_param = (pmotor_info->LensMotorConfig[0].iBacklashValue << 16) + 0;
                g_LensDrvLv8044.LensDrv.pMotor->focus_setState(MOTOR_FOCUS_BWD, cmd_param);
            }

            rest_step = pmotor_info->LensMotorConfig[0].iCurrPosition - position;

            do {
                set_step = (rest_step > pmotor_info->LensMotorConfig[0].iMaxStepSize)
                          ? pmotor_info->LensMotorConfig[0].iMaxStepSize : rest_step;
                cmd_param = (0 << 16) + set_step;
                rest_step -= set_step;
                acc_step += set_step;

                if ((rest_step <= 0) || (pmotor_info->LensMotorConfig[0].bSetMove2Stop == TRUE)) {
                    rest_step = 0;
                    pmotor_info->LensMotorConfig[0].bSetFinalStep = TRUE;
                }

                g_LensDrvLv8044.LensDrv.pMotor->focus_setState(MOTOR_FOCUS_BWD, cmd_param);
            } while (rest_step > 0);
        } else {
            DBG_MSG("%s step=0!\r\n", __func__);
        }
    } else {
        DBG_MSG("set focus position is out of range!\r\n");
    }

    return acc_step;
}


static INT32 lensLV8044_FocusPress(UINT32 focus)
{
    INT32 target_pos, low_bound = 0, high_bound = 0;
    LENSMOTOR_INFO *pmotor_info = LV8044_GetMotorInfo();

    LV8044_CalcFocusRange(&low_bound, &high_bound, -1, TRUE);

    if (focus == FOCUS_WIDE) {
        target_pos = pmotor_info->ZoomFocusTbl[ZOOM_SECTION_WIDE].iFocusMin;
    } else if (focus == FOCUS_TELE) {
        target_pos = pmotor_info->ZoomFocusTbl[ZOOM_SECTION_TELE].iFocusMax;
    } else if (focus == FOCUS_WIDE_SIDE) {    // move to focus range min pos
        target_pos = low_bound;
    } else if (focus == FOCUS_WIDE_TELE_MIDDLE) {    // move to focus range (min+max)/2 pos
        target_pos = (low_bound + high_bound) / 2;
    } else if (focus == FOCUS_TELE_SIDE) {    // move to focus range max pos
        target_pos = high_bound;
    } else {
        DBG_MSG("%s out of move mode\r\n", __func__);

        return ERR_OPERATION_FAIL;
    }

    lensLV8044_FocusGo2Position(target_pos, FALSE);

    return ERR_OK;
}


static UINT32 lensLV8044_FocusRelease(void)
{
    LENSMOTOR_INFO *pmotor_info = LV8044_GetMotorInfo();

    pmotor_info->LensMotorConfig[0].bSetMove2Stop = TRUE;

    return 0;
}


static INT32 lensLV8044_FocusRetract(FOCUS_RETRACT_POSITION position)
{
    return ERR_OK;
}


//----------------------------------------------------------------------------------------------------------------------
// aperture
//----------------------------------------------------------------------------------------------------------------------
static void lensLV8044_ApertureInit(void)
{
    g_LensDrvLv8044.eAperturePos = IRIS_POS_BIG;

    lensLV8044_ApertureGo2Position(IRIS_POS_BIG);
}


static UINT32 lensLV8044_ApertureGetFNo(UINT32 zoom_section, UINT32 iris_pos)
{
    UINT32 fno_value, focal_length;

    zoom_section = LENS_CLAMP(zoom_section, ZOOM_SECTION_WIDE, ZOOM_SECTION_TELE);

    fno_value = g_uiFNumMappLv8044[g_LensDrvLv8044.eAperturePos];

    if (zoom_section > ZOOM_SECTION_WIDE) {
        focal_length = g_LensDrvLv8044.uiFocalLengthMin + (zoom_section - ZOOM_SECTION_WIDE)
                    * (g_LensDrvLv8044.uiFocalLengthMax - g_LensDrvLv8044.uiFocalLengthMin + (ZOOM_SECTION_TELE - ZOOM_SECTION_WIDE) / 2) / (ZOOM_SECTION_TELE - ZOOM_SECTION_WIDE);
    } else {
        focal_length = g_LensDrvLv8044.uiFocalLengthMin;
    }

    fno_value *= (focal_length / g_LensDrvLv8044.uiFocalLengthMin);

    return fno_value;
}


static IRIS_POS lensLV8044_ApertureGetPosition(void)
{
    return g_LensDrvLv8044.eAperturePos;
}


static void lensLV8044_ApertureGo2Position(IRIS_POS position)
{
    if (position > IRIS_POS_CLOSE) {
        DBG_ERR("%s parameters error: (%d)\r\n", __func__, position);
    } else {
        if (g_LensDrvLv8044.LensDrv.pMotor->aperture_setState != NULL) {
            g_LensDrvLv8044.LensDrv.pMotor->aperture_setState(MOTOR_APERTURE_NORMAL, position);
            g_LensDrvLv8044.eAperturePos = position;
        } else {
            DBG_MSG("%s aperture_setState = NULL!\r\n", __func__);
        }
    }
}

static void lensLV8044_ApertureSetLumFlux(UINT32 flux)
{

}


//----------------------------------------------------------------------------------------------------------------------
// shutter
//----------------------------------------------------------------------------------------------------------------------
static void lensLV8044_ShutterSetState(MOTOR_SHUTTER_ACT state, UINT32 param)
{
    if (state > MOTOR_SHUTTER_CAP) {
        DBG_ERR("%s parameters error: (%d)\r\n", __func__, state);
    } else {
        if (g_LensDrvLv8044.LensDrv.pMotor->shutter_setState != NULL) {
            g_LensDrvLv8044.LensDrv.pMotor->shutter_setState(state, param);
            g_LensDrvLv8044.eShutterState = state;
        } else {
            DBG_MSG("%s shutter_setState = NULL!\r\n", __func__);
        }
    }
}


//----------------------------------------------------------------------------------------------------------------------
// IR cut
//----------------------------------------------------------------------------------------------------------------------
static void lensLV8044_IrcutSetState(MOTOR_IRCUT_ACT state, UINT32 param)
{
    if (state > MOTOR_IRCUT_CLOSE) {
        DBG_ERR("%s parameters error: (%d)\r\n", __func__, state);
    } else {
        if (g_LensDrvLv8044.LensDrv.pMotor->ircut_setState != NULL) {
            g_LensDrvLv8044.LensDrv.pMotor->ircut_setState(state, param);
            g_LensDrvLv8044.eIrCutState = state;
        } else {
            DBG_MSG("%s ircut_setState = NULL!\r\n", __func__);
        }
    }
}


//----------------------------------------------------------------------------------------------------------------------
// miscellaneous
//----------------------------------------------------------------------------------------------------------------------
static INT32 lensLV8044_LensRetract(void)
{
    return ERR_OK;
}


static INT32 lensLV8044_LensReset(void)
{
    return ERR_OK;
}


static INT32 lensLV8044_LensPowerOff(void)
{
    if (g_LensDrvLv8044.LensDrv.pMotor->setSignal != NULL) {
        g_LensDrvLv8044.LensDrv.pMotor->setSignal(MD_SIGNAL_STANDBY, 0);

        return ERR_OK;
    } else {
        DBG_MSG("%s setSignal = NULL!\r\n", __func__);

        return ERR_OPERATION_FAIL;
    }
}


static UINT32 lensLV8044_LensGetCapInfo(void)
{
    // return lens capability
    return LENS_SUPPORT_ZOOM | LENS_SUPPORT_FOCUS;
}


static void lensLV8044_MakeAFTable(void)
{
    static BOOL ever_entry = FALSE;
    UINT32 i;
    static LENS_ZOOM_FOCUS_TAB zoom_focus_tbl[ZOOM_SECTION_NUM] = {{ .iZoomPosition = 0, .iFocusMin = 0, .iFocusMax = 0 }};
    LENSMOTOR_INFO *pmotor_info = LV8044_GetMotorInfo();

    if (ever_entry == FALSE) {
        ever_entry = TRUE;

        for (i = ZOOM_SECTION_WIDE; i <= ZOOM_SECTION_TELE; i++) {
            zoom_focus_tbl[i].iFocusMin = pmotor_info->ZoomFocusTbl[i].iFocusMin;
            zoom_focus_tbl[i].iFocusMax = pmotor_info->ZoomFocusTbl[i].iFocusMax;
        }
    }

    // read lens calibration data from PStore
    for (i = ZOOM_SECTION_WIDE; i <= ZOOM_SECTION_TELE; i++) {
        zoom_focus_tbl[i].iFocusMin += g_LensDrvLv8044.iFocusCaliOfst[i];
        zoom_focus_tbl[i].iFocusMax += g_LensDrvLv8044.iFocusCaliOfst[i];
    }
}


static INT32 lensLV8044_SetCaliData(INT16 *pcal_tbl, UINT32 tbl_size)
{
    UINT32 i;

    if ((pcal_tbl != NULL) && (tbl_size == ZOOM_SECTION_TELE)) {
        for (i = ZOOM_SECTION_WIDE; i <= ZOOM_SECTION_TELE; i++) {
            g_LensDrvLv8044.iFocusCaliOfst[i] = pcal_tbl[i];
            DBG_DUMP("zoom_sec#%d cali_focus_offset=%d\r\n", i, g_LensDrvLv8044.iFocusCaliOfst[i]);
        }
    } else {
        DBG_ERR("pcal_tbl = NULL or tbl_size NG!\r\n");

        return ERR_OPERATION_FAIL;
    }

    return ERR_OK;
}


static UINT32 lensLV8044_GetZoomFocusTable(UINT32 tbl_idx, LENS_ZOOM_FOCUS_TAB *zf_tbl)
{
    LENSMOTOR_INFO *pmotor_info = LV8044_GetMotorInfo();

    if (tbl_idx == 0xFFFF) {    // all table
        memcpy(zf_tbl, &pmotor_info->ZoomFocusTbl[0].iZoomPosition, ZOOM_SECTION_NUM*sizeof(LENS_ZOOM_FOCUS_TAB));
    } else {    // single table
        tbl_idx = LENS_MIN(tbl_idx, ZOOM_SECTION_NUM-1);

        memcpy(zf_tbl, &pmotor_info->ZoomFocusTbl[tbl_idx].iZoomPosition, sizeof(LENS_ZOOM_FOCUS_TAB));
    }

    return tbl_idx;
}

static UINT32 lensLV8044_SetZoomFocusTable(UINT32 tbl_idx, LENS_ZOOM_FOCUS_TAB *zf_tbl)
{
    LENSMOTOR_INFO *pmotor_info = LV8044_GetMotorInfo();

    if (tbl_idx == 0xFFFF) {    // all table
        memcpy(&pmotor_info->ZoomFocusTbl[0].iZoomPosition, zf_tbl, ZOOM_SECTION_NUM*sizeof(LENS_ZOOM_FOCUS_TAB));
    } else {    // single table
        tbl_idx = LENS_MIN(tbl_idx, ZOOM_SECTION_NUM-1);

        memcpy(&pmotor_info->ZoomFocusTbl[tbl_idx].iZoomPosition, zf_tbl, sizeof(LENS_ZOOM_FOCUS_TAB));
    }

    return tbl_idx;
}


