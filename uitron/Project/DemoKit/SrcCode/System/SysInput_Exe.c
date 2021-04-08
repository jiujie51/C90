/*
    System Input Callback

    System Callback for Input Module.

    @file       SysInput_Exe.c
    @ingroup    mIPRJSYS

    @note

    Copyright   Novatek Microelectronics Corp. 2010.  All rights reserved.
*/

////////////////////////////////////////////////////////////////////////////////
#include "SysCommon.h"
#include "AppCommon.h"
////////////////////////////////////////////////////////////////////////////////
#include "UIFrameworkExt.h"
#include "UICommon.h"
#include "AppLib.h"
#include "GxInput.h"
#include "GxPower.h"
#if (IPCAM_FUNC != ENABLE)
#include "SoundData.h"
#endif
#if (GSENSOR_FUNCTION == ENABLE)
#include "GSensor.h"
#endif
//global debug level: PRJ_DBG_LVL
#include "PrjCfg.h"
//local debug level: THIS_DBGLVL
#define THIS_DBGLVL         2 // 0=FATAL, 1=ERR, 2=WRN, 3=UNIT, 4=FUNC, 5=IND, 6=MSG, 7=VALUE, 8=USER
///////////////////////////////////////////////////////////////////////////////
#define __MODULE__          SysInputExe
#define __DBGLVL__          ((THIS_DBGLVL>=PRJ_DBG_LVL)?THIS_DBGLVL:PRJ_DBG_LVL)
#define __DBGFLT__          "*" //*=All, [mark]=CustomClass
#include "DebugModule.h"
///////////////////////////////////////////////////////////////////////////////

#include "SysInput_API.h"
#include "gpio.h"

#include "WavPlay.h"
#include "rtc.h"
#include "DxDisplay.h"
#include "DxCommon.h"
#include "DxPower.h"
#if(STARTWDT_FUNCTION==ENABLE)
#include "wdt.h"
#endif

//#NT#Refine code for continue key
#define    BURSTKEY_DEBOUNCE     200//ms
#define    BURSTKEY_INTERVAL     800//ms
#define TOUCH_TIMER_CNT            1//20ms
#define    DOUBLECLICK_INTERVAL     500//ms

#if (USER_DEF_TOUCH == ENABLE)
#define TOUCH_PANEL_W      1280//1920
#define TOUCH_PANEL_H      320//384
#define TOUCH_MOVE_THRESHOLD_W  8
#define TOUCH_MOVE_THRESHOLD_H  6
#define TOUCH_SLIDE_THRESHOLD_W 128// 192
#define TOUCH_SLIDE_THRESHOLD_H  64//72

#define TOUCH_FILTER_THRESHOLD_W  128// 192
#define TOUCH_FILTER_THRESHOLD_H  64// 136

#define TOUCH_MOVE_DIR_UP	 1
#define TOUCH_MOVE_DIR_DOWN  2
#define TOUCH_MOVE_DIR_LEFT  3
#define TOUCH_MOVE_DIR_RIGHT 4

#define TOUCH_MOVE_EVENT_SEND_INTERVAL 2  // 2*20MS

//#MT#lyb -20200318 -begin
//drv value
#define TOUCH_PANEL_DRV_W       1920
#define TOUCH_PANEL_DRV_H       384
//#MT#lyb -20200318 -end

#endif

int SX_TIMER_DET_KEY_ID = -1;
int SX_TIMER_DET_TOUCH_ID = -1;
int SX_TIMER_DET_PWR_ID = -1;
int SX_TIMER_DET_MODE_ID = -1;
int SX_TIMER_AUTO_INPUT_ID = -1;
int SX_TIMER_DET_GSENSOR_ID = -1;
int SX_TIMER_DET_USER_ID = -1;
int SX_TIMER_DET_ALS_BL_ID = -1;
int SX_TIMER_WAVPLAYCONTROL_ID = -1;
void UI_DetPwrKey(void);
void UI_DetNormalKey(void);
void UI_DetStatusKey(void);
void UI_DetCustom1Key(void);
void UI_DetUserFunc(void);
void UI_DetALS_BLCtrl(void); //200ms
void UI_DetWavPlayControl(void);
BOOL SysInput_isLcdBackLightOn(void);
void SysInput_SetLcdBackLight(BOOL status);
#if(USER_DEF_TOUCH == ENABLE)
void Ux_PostTouchEvent(NVTEVT evt, UINT32 paramNum, INT32 DataX, INT32 DataY);
void UI_DetTouch(void);
#endif

#if (POWERKEY_FUNCTION == ENABLE)
SX_TIMER_ITEM(Input_DetPKey, UI_DetPwrKey, 5, FALSE)
#endif
#if (NORMALKEY_FUNCTION == ENABLE)
SX_TIMER_ITEM(Input_DetNKey, UI_DetNormalKey, 1, FALSE)//20ms
#endif
#if (STATUSKEY_FUNCTION == ENABLE)
SX_TIMER_ITEM(Input_DetSKey, UI_DetStatusKey, 7, FALSE)
#endif
#if (GSENSOR_FUNCTION == ENABLE)
SX_TIMER_ITEM(Input_DetC1Key, UI_DetCustom1Key, 3, FALSE)
#endif
SX_TIMER_ITEM(Input_DetUserFunc, UI_DetUserFunc, 50, FALSE)

#if !defined(_TOUCH_OFF_)
#if(USER_DEF_TOUCH == ENABLE)
SX_TIMER_ITEM(Input_DetTP, UI_DetTouch, TOUCH_TIMER_CNT, FALSE)
#else
SX_TIMER_ITEM(Input_DetTP, GxTouch_DetTP, TOUCH_TIMER_CNT, FALSE)
#endif
SX_TIMER_ITEM(Input_DetALS_BLCtrl, UI_DetALS_BLCtrl, 20, FALSE) //400ms检测一次
#endif
SX_TIMER_ITEM(Input_WavPlayControl, UI_DetWavPlayControl, 1, FALSE)

static BOOL         m_uiAnyKeyUnlockEn        = FALSE;

#if !defined(_TOUCH_OFF_)
static TOUCH_OBJ g_TouchTable[] = {
#if(USER_DEF_TOUCH == DISABLE)
	{TP_GESTURE_PRESS,         NVTEVT_PRESS,        DEMOSOUND_SOUND_KEY_TONE},
	{TP_GESTURE_MOVE,          NVTEVT_MOVE,         0},

	{TP_GESTURE_HOLD,          NVTEVT_HOLD,         0},
	{TP_GESTURE_RELEASE,       NVTEVT_RELEASE,      DEMOSOUND_SOUND_KEY_TONE},
	{TP_GESTURE_CLICK,         NVTEVT_CLICK,        0},
	{TP_GESTURE_SLIDE_LEFT,    NVTEVT_SLIDE_LEFT,   0},
	{TP_GESTURE_SLIDE_RIGHT,   NVTEVT_SLIDE_RIGHT,  0},
	{TP_GESTURE_SLIDE_UP,      NVTEVT_SLIDE_UP,     0},
	{TP_GESTURE_SLIDE_DOWN,    NVTEVT_SLIDE_DOWN,   0}
#else
#endif
};
#endif

#if defined(_KEY_METHOD_4KEY_)
static KEY_OBJ g_KeyTable[] = {
#if defined(_UI_STYLE_CARDV_)
	//POWER KEY
	{FLGKEY_KEY_POWER,    KEY_PRESS,       NVTEVT_KEY_POWER_REL,        0,                0},
	//NORMAL KEY
	{FLGKEY_MENU,         KEY_PRESS,       NVTEVT_KEY_MENU,         NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_MODE,         KEY_RELEASE,     NVTEVT_KEY_MODE,         NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_UP,           KEY_PRESS,       NVTEVT_KEY_UP,           NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_UP,           KEY_CONTINUE,    NVTEVT_KEY_UP,           NVTEVT_KEY_CONTINUE,     0},
	{FLGKEY_UP,           KEY_RELEASE,     NVTEVT_KEY_UP,           NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_DOWN,         KEY_PRESS,       NVTEVT_KEY_DOWN,         NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_DOWN,         KEY_CONTINUE,    NVTEVT_KEY_DOWN,         NVTEVT_KEY_CONTINUE,     0},
	{FLGKEY_DOWN,         KEY_RELEASE,     NVTEVT_KEY_DOWN,         NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_SHUTTER2,     KEY_PRESS,       NVTEVT_KEY_SHUTTER2,     NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_SHUTTER2,     KEY_RELEASE,     NVTEVT_KEY_SHUTTER2,     NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_ENTER,        KEY_PRESS,       NVTEVT_KEY_ENTER,        NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_ENTER,        KEY_RELEASE,     NVTEVT_KEY_ENTER,        NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_CUSTOM1,      KEY_PRESS,       NVTEVT_KEY_CUSTOM1,      NVTEVT_KEY_PRESS,       DEMOSOUND_SOUND_NONE},
	{FLGKEY_CUSTOM1,      KEY_RELEASE,     NVTEVT_KEY_CUSTOM1,      NVTEVT_KEY_RELEASE,     0},
#else// _UI_STYLE_SPORTCAM_
	//POWER KEY
	{FLGKEY_KEY_POWER,    KEY_CONTINUE,     NVTEVT_KEY_POWER_REL,        0,                0},
	//NORMAL KEY
	{FLGKEY_UP,           KEY_PRESS,       NVTEVT_KEY_PREV,           NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_UP,           KEY_CONTINUE,    NVTEVT_KEY_PREV,           NVTEVT_KEY_CONTINUE,     0},
	{FLGKEY_UP,           KEY_RELEASE,     NVTEVT_KEY_PREV,           NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_DOWN,         KEY_PRESS,       NVTEVT_KEY_NEXT,         NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_DOWN,         KEY_CONTINUE,    NVTEVT_KEY_NEXT,         NVTEVT_KEY_CONTINUE,     0},
	{FLGKEY_DOWN,         KEY_RELEASE,     NVTEVT_KEY_NEXT,         NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_LEFT,         KEY_PRESS,       NVTEVT_KEY_MOVIE,         NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	//{FLGKEY_LEFT,         KEY_CONTINUE,    NVTEVT_KEY_PREV,         NVTEVT_KEY_CONTINUE,     0},
	//{FLGKEY_LEFT,         KEY_RELEASE,     NVTEVT_KEY_PREV,         NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_SHUTTER2,     KEY_PRESS,       NVTEVT_KEY_SHUTTER2,     NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_SHUTTER2,     KEY_RELEASE,     NVTEVT_KEY_SHUTTER2,     NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_RIGHT,        KEY_PRESS,       NVTEVT_KEY_SELECT,        NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_RIGHT,        KEY_CONTINUE,    NVTEVT_KEY_SELECT,        NVTEVT_KEY_CONTINUE,     0},
	{FLGKEY_RIGHT,        KEY_RELEASE,     NVTEVT_KEY_SELECT,        NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_CUSTOM1,      KEY_PRESS,       NVTEVT_KEY_CUSTOM1,      NVTEVT_KEY_PRESS,       DEMOSOUND_SOUND_NONE},
	{FLGKEY_CUSTOM1,      KEY_RELEASE,     NVTEVT_KEY_CUSTOM1,      NVTEVT_KEY_RELEASE,     0},

#endif
};
#else
//_KEY_METHOD_2KEY_ (for SPORTCAM only)
static KEY_OBJ g_KeyTable[] = {
	//POWER KEY
//	{FLGKEY_KEY_POWER,    KEY_RELEASE,     NVTEVT_KEY_POWER_REL,        0,                0},
	//NORMAL KEY
	{FLGKEY_ENTER,        KEY_PRESS,       NVTEVT_KEY_ENTER,        NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_MENU,         KEY_PRESS,       NVTEVT_KEY_MENU,         NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_MODE,         KEY_PRESS,       NVTEVT_KEY_MODE,         NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_PLAYBACK,     KEY_PRESS,       NVTEVT_KEY_PLAYBACK,     NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_UP,           KEY_PRESS,       NVTEVT_KEY_UP,           NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_UP,           KEY_CONTINUE,    NVTEVT_KEY_UP,           NVTEVT_KEY_CONTINUE,     0},
	{FLGKEY_UP,           KEY_RELEASE,     NVTEVT_KEY_UP,           NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_DOWN,         KEY_PRESS,       NVTEVT_KEY_DOWN,         NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_DOWN,         KEY_CONTINUE,    NVTEVT_KEY_DOWN,         NVTEVT_KEY_CONTINUE,     0},
	{FLGKEY_DOWN,         KEY_RELEASE,     NVTEVT_KEY_DOWN,         NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_LEFT,         KEY_PRESS,       NVTEVT_KEY_SELECT,         NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_LEFT,         KEY_CONTINUE,    NVTEVT_KEY_SELECT,         NVTEVT_KEY_CONTINUE,     0},
	{FLGKEY_LEFT,         KEY_RELEASE,     NVTEVT_KEY_SELECT,         NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_RIGHT,        KEY_PRESS,       NVTEVT_KEY_NEXT,        NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_RIGHT,        KEY_CONTINUE,    NVTEVT_KEY_NEXT,        NVTEVT_KEY_CONTINUE,     0},
	{FLGKEY_RIGHT,        KEY_RELEASE,     NVTEVT_KEY_NEXT,        NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_DOWN,         KEY_PRESS,       NVTEVT_KEY_DOWN,         NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_DOWN,         KEY_CONTINUE,    NVTEVT_KEY_DOWN,         NVTEVT_KEY_CONTINUE,     0},
	{FLGKEY_DOWN,         KEY_RELEASE,     NVTEVT_KEY_DOWN,         NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_ZOOMIN,       KEY_PRESS,       NVTEVT_KEY_ZOOMIN,       NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_ZOOMIN,       KEY_CONTINUE,    NVTEVT_KEY_ZOOMIN,       NVTEVT_KEY_CONTINUE,     0},
	{FLGKEY_ZOOMIN,       KEY_RELEASE,     NVTEVT_KEY_ZOOMIN,       NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_ZOOMOUT,      KEY_PRESS,       NVTEVT_KEY_ZOOMOUT,      NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_ZOOMOUT,      KEY_CONTINUE,    NVTEVT_KEY_ZOOMOUT,      NVTEVT_KEY_CONTINUE,     0},
	{FLGKEY_ZOOMOUT,      KEY_RELEASE,     NVTEVT_KEY_ZOOMOUT,      NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_SHUTTER2,     KEY_PRESS,       NVTEVT_KEY_SHUTTER2,     NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_SHUTTER2,     KEY_RELEASE,     NVTEVT_KEY_SHUTTER2,     NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_SHUTTER1,     KEY_PRESS,       NVTEVT_KEY_SHUTTER1,     NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_SHUTTER1,     KEY_RELEASE,     NVTEVT_KEY_SHUTTER1,     NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_CUSTOM1,      KEY_PRESS,       NVTEVT_KEY_CUSTOM1,      NVTEVT_KEY_PRESS,       DEMOSOUND_SOUND_NONE},
	{FLGKEY_CUSTOM1,      KEY_RELEASE,     NVTEVT_KEY_CUSTOM1,      NVTEVT_KEY_RELEASE,     0},
};
#endif
void KeySoundCB(UINT32 uiSoundID)
{
	if (uiSoundID) {
#if(UI_FUNC==ENABLE)
		UISound_Play(uiSoundID);
#endif
	}
}

UINT32 Input_GroupStatus2Event(UINT32 status)
{
	UINT32 i = 0;
	for (i = 0; i < sizeof(g_KeyTable) / sizeof(KEY_OBJ); i++) {
		if ((g_KeyTable[i].uiKeyFlag == STATUS_KEY_GROUP1) && (g_KeyTable[i].status == status)) {
			return g_KeyTable[i].uiKeyEvent;
		}
	}
	return 0;
}

/**
  convert GPIO key to UI key event
  [InputCB internal API]

  @param UINT32 keys: Input key code detected from GPIO mapping
  @return UINT32: NVTEVT
**/
#if 0
static UINT32 Input_Key2Evt(UINT32 keys)
{
	if (keys & FLGKEY_MOVIE) {
		return NVTEVT_KEY_MOVIE;
	}
	if (keys & FLGKEY_I) {
		return NVTEVT_KEY_I;
	}
	if (keys & FLGKEY_MODE) {
		return NVTEVT_KEY_MODE;
	}
	if (keys & FLGKEY_PLAYBACK) {
		return NVTEVT_KEY_PLAYBACK;
	}
	if (keys & FLGKEY_MENU) {
		return NVTEVT_KEY_MENU;
	}
	if (keys & FLGKEY_FACEDETECT) {
		return NVTEVT_KEY_FACEDETECT;
	}
	if (keys & FLGKEY_DEL) {
		return NVTEVT_KEY_DEL;
	}
	if (keys & FLGKEY_LEFT) {
		return NVTEVT_KEY_LEFT;
	}
	if (keys & FLGKEY_RIGHT) {
		return NVTEVT_KEY_RIGHT;
	}
	if (keys & FLGKEY_ENTER) {
		return NVTEVT_KEY_ENTER;
	}
	if (keys & FLGKEY_SHUTTER1) {
		return NVTEVT_KEY_SHUTTER1;
	}
	if (keys & FLGKEY_SHUTTER2) {
		return NVTEVT_KEY_SHUTTER2;
	}
	if (keys & FLGKEY_ZOOMOUT) {
		return NVTEVT_KEY_ZOOMOUT;
	}
	if (keys & FLGKEY_ZOOMIN) {
		return NVTEVT_KEY_ZOOMIN;
	}
	if (keys & FLGKEY_UP) {
		return NVTEVT_KEY_UP;
	}
	if (keys & FLGKEY_DOWN) {
		return NVTEVT_KEY_DOWN;
	} else {
		return NVTEVT_NULL;
	}
}
#endif
//just for backward compatible
void Input_SetKeyMask(KEY_STATUS uiMode, UINT32 uiMask)
{
	SysMan_SetKeyMask(uiMode, uiMask);
}
UINT32 Input_GetKeyMask(KEY_STATUS uiMode)
{
	return SysMan_GetKeyMask(uiMode);
}
void Input_SetKeySoundMask(KEY_STATUS uiMode, UINT32 uiMask)
{
	SysMan_SetKeySoundMask(uiMode, uiMask);
}
UINT32 Input_GetKeySoundMask(KEY_STATUS uiMode)
{
	return SysMan_GetKeySoundMask(uiMode);
}

/**
    reset all mask,usually in new winodw onOpen
*/
void Input_ResetMask(void)
{
	// Set key mask as default
	SysMan_SetKeyMask(KEY_PRESS, FLGKEY_KEY_MASK_DEFAULT);
	SysMan_SetKeyMask(KEY_RELEASE, FLGKEY_KEY_MASK_DEFAULT);
	SysMan_SetKeyMask(KEY_CONTINUE, FLGKEY_KEY_MASK_NULL);
#if !defined(_TOUCH_OFF_)
	SysMan_SetTouchMask(TOUCH_MASK_DEFAULT);
#endif
	SysMan_SetKeySoundMask(KEY_PRESS, FLGKEY_SOUND_MASK_DEFAULT);
	SysMan_SetKeySoundMask(KEY_RELEASE, FLGKEY_KEY_MASK_NULL);
	SysMan_SetKeySoundMask(KEY_CONTINUE, FLGKEY_SOUND_MASK_DEFAULT);
}
/**
    for some case,press any key unlock all,and post NVTEVT_KEY_PRESS
    ex:Timelapse or smile detect
*/
void Input_SetAnyKeyUnlock(BOOL en)
{
	m_uiAnyKeyUnlockEn = en;
}


UINT32 Input_Key2Mode(UINT32 keys)
{
#if (STATUSKEY_FUNCTION == ENABLE)
	switch (keys) {
	case NVTEVT_KEY_STATUS1:
		return DSC_MODE_MOVIE;
	case NVTEVT_KEY_STATUS2:
		return DSC_MODE_PHOTO_MANUAL;
	case NVTEVT_KEY_STATUS3:
		return DSC_MODE_PHOTO_MANUAL;
	case NVTEVT_KEY_STATUS4:
		return DSC_MODE_PHOTO_MANUAL;
	case NVTEVT_KEY_STATUS5:
		return DSC_MODE_PHOTO_MANUAL;
	case NVTEVT_KEY_STATUS6:
		return DSC_MODE_PHOTO_SCENE;
	case NVTEVT_KEY_STATUS7:
		return DSC_MODE_PHOTO_MANUAL;
	case NVTEVT_KEY_STATUS8:
		return DSC_MODE_PHOTO_MANUAL;
	case NVTEVT_KEY_STATUS9:
		return DSC_MODE_PHOTO_MANUAL;
	case NVTEVT_KEY_STATUS10:
		return DSC_MODE_PHOTO_MANUAL;
	default:
		return DSC_MODE_PHOTO_AUTO;
	}
#else
	return 0;
#endif
}
extern SX_CMD_ENTRY key[];
extern SX_CMD_ENTRY ts[];
#if (defined(_NVT_ETHREARCAM_RX_) ||defined(_NVT_ETHREARCAM_TX_))
extern void EthCamNet_LinkDet(void);
extern int SX_TIMER_ETHCAM_LINKDET_ID;
#if defined(_NVT_ETHREARCAM_RX_)
SX_TIMER_ITEM(ETHCAM_LinkDet, EthCamNet_LinkDet, 100, FALSE)
#else
SX_TIMER_ITEM(ETHCAM_LinkDet, EthCamNet_LinkDet, 25, FALSE)
#endif
#endif

#if (defined(_NVT_ETHREARCAM_RX_))
#if (ETH_REARCAM_CAPS_COUNT>=2)
extern void EthCamNet_EthHubLinkDetInit(void);
extern void EthCamNet_EthHubLinkDet(void);
extern int SX_TIMER_ETHCAM_ETHHUB_LINKDET_ID;
SX_TIMER_ITEM(ETHCAM_EthHubLinkDet, EthCamNet_EthHubLinkDet, 50, FALSE)
#endif
extern void EthCamNet_EthLinkRetry(void);
extern int SX_TIMER_ETHCAM_ETHLINKRETRY_ID;
SX_TIMER_ITEM(ETHCAM_EthHubLinkRetry, EthCamNet_EthLinkRetry, 50, FALSE)
#endif
void System_OnInputInit(void)
{
	//PHASE-1 : Init & Open Drv or DrvExt
	{
		GxKey_RegCB(Key_CB);         //Register CB function of GxInput
#if !defined(_TOUCH_OFF_)       
#if(USER_DEF_TOUCH == DISABLE)
		GxTouch_RegCB(Touch_CB);     //Register CB function of GxInput
#endif
#endif
		GxKey_Init();
		SxCmd_AddTable(key);
		SxCmd_AddTable(ts);
#if (defined(_NVT_ETHREARCAM_RX_) ||defined(_NVT_ETHREARCAM_TX_))
		SX_TIMER_ETHCAM_LINKDET_ID = SxTimer_AddItem(&Timer_ETHCAM_LinkDet);
#endif
#if (defined(_NVT_ETHREARCAM_RX_))
#if (ETH_REARCAM_CAPS_COUNT>=2)
		EthCamNet_EthHubLinkDetInit();
		SX_TIMER_ETHCAM_ETHHUB_LINKDET_ID = SxTimer_AddItem(&Timer_ETHCAM_EthHubLinkDet);
		SxTimer_SetFuncActive(SX_TIMER_ETHCAM_ETHHUB_LINKDET_ID, TRUE);
#endif
		SX_TIMER_ETHCAM_ETHLINKRETRY_ID = SxTimer_AddItem(&Timer_ETHCAM_EthHubLinkRetry);
#endif
	}
	//PHASE-2 : Init & Open Lib or LibExt
	{
		//1.砞﹚init
		//2.砞﹚CB,
		GxKey_SetContDebounce(BURSTKEY_DEBOUNCE / SxTimer_GetData(SXTIMER_TIMER_BASE));
		GxKey_SetRepeatInterval(BURSTKEY_INTERVAL / SxTimer_GetData(SXTIMER_TIMER_BASE));
		SysMan_RegKeySoundCB(KeySoundCB);
		SysMan_RegKeyTable(g_KeyTable, sizeof(g_KeyTable) / sizeof(KEY_OBJ));
		//3.爹SxJob狝叭 ---------> System Job
		//4.爹SxTimer狝叭 ---------> Detect Job

#if (POWERKEY_FUNCTION == ENABLE)
		SX_TIMER_DET_PWR_ID = SxTimer_AddItem(&Timer_Input_DetPKey);
#endif
#if (NORMALKEY_FUNCTION == ENABLE)
		SX_TIMER_DET_KEY_ID = SxTimer_AddItem(&Timer_Input_DetNKey);
#endif
#if (STATUSKEY_FUNCTION == ENABLE)
		SX_TIMER_DET_MODE_ID = SxTimer_AddItem(&Timer_Input_DetSKey);
#endif
#if (GSENSOR_FUNCTION == ENABLE)
		SX_TIMER_DET_GSENSOR_ID = SxTimer_AddItem(&Timer_Input_DetC1Key);
#endif
#if !defined(_TOUCH_OFF_)
		SX_TIMER_DET_TOUCH_ID = SxTimer_AddItem(&Timer_Input_DetTP);
#endif
SX_TIMER_WAVPLAYCONTROL_ID = SxTimer_AddItem(&Timer_Input_WavPlayControl);
SX_TIMER_DET_USER_ID = SxTimer_AddItem(&Timer_Input_DetUserFunc);
SX_TIMER_DET_ALS_BL_ID = SxTimer_AddItem(&Timer_Input_DetALS_BLCtrl);

#if (STATUSKEY_FUNCTION == ENABLE)
		GxKey_DetStatusKey();
#endif

#if (POWERKEY_FUNCTION == ENABLE)
		SxTimer_SetFuncActive(SX_TIMER_DET_PWR_ID, TRUE);
#endif
#if (NORMALKEY_FUNCTION == ENABLE)
		SxTimer_SetFuncActive(SX_TIMER_DET_KEY_ID, TRUE);
#endif
#if (STATUSKEY_FUNCTION == ENABLE)
		SxTimer_SetFuncActive(SX_TIMER_DET_MODE_ID, TRUE);
#endif
#if (GSENSOR_FUNCTION == ENABLE)
		SxTimer_SetFuncActive(SX_TIMER_DET_GSENSOR_ID, TRUE);
#endif


    SxTimer_SetFuncActive(SX_TIMER_DET_USER_ID, TRUE);
	SxTimer_SetFuncActive(SX_TIMER_DET_ALS_BL_ID, TRUE);
	ClearWavQueue();
	SxTimer_SetFuncActive(SX_TIMER_WAVPLAYCONTROL_ID, TRUE);
#if !defined(_TOUCH_OFF_)
		GxTouch_Init();
		GxTouch_SetCtrl(GXTCH_DOUBLE_CLICK_INTERVAL,
						DOUBLECLICK_INTERVAL / TOUCH_TIMER_CNT / SxTimer_GetData(SXTIMER_TIMER_BASE));
		SysMan_RegTouchSoundCB(KeySoundCB);
		SysMan_RegTouchTable(g_TouchTable, sizeof(g_TouchTable) / sizeof(TOUCH_OBJ));
		SxTimer_SetFuncActive(SX_TIMER_DET_TOUCH_ID, TRUE);
#endif
	}
}

void System_OnInputExit(void)
{
	//PHASE-2 : Close Lib or LibExt
	{
	}
	//PHASE-1 : Close Drv or DrvExt
	{
	}
}

/////////////////////////////////////////////////////////////////////////////
#if  1 //(ALS_BL_CTRL == ENABLE) //// cj 0716
UINT32 uiLCDBackLightCtrlPercent = 100;
UINT32 uiLCDBackLightCtrlPercentPrev = 100;
UINT32 uiLCDBackLightCtrlPercentCtrlPrev = 100;
extern UINT32 GetEV(void);
#define  	LCD_LVL_MAX 	100
#define  	LCD_LVL_DEF	49
#define  	LCD_LVL_MIN	  1	
#define  	EV_LVL_MAX		1100//1840
#define  	EV_LVL_DEF		735//855	
#define  	EV_LVL_MIN		300
#define 	BL_UP_UNIT 		((LCD_LVL_MAX-LCD_LVL_DEF)*100/(EV_LVL_MAX -EV_LVL_DEF))
#define 	BL_LOWER_UNIT 	((LCD_LVL_MIN -LCD_LVL_DEF)*100/(EV_LVL_MIN -EV_LVL_DEF )) 
#define 	BL_Proportion 	((LCD_LVL_MAX -LCD_LVL_MIN)/(EV_LVL_MAX -EV_LVL_MIN )*0.14) 
UINT32 AdjLCDBackLightLvl(void)
{
	UINT32 adj_bl_value = 0;
//获取当前EV值
		UINT32 CurEvValue;
		CurEvValue = GetEV();
		//debug_msg("\r\n CurEvValue:%d \r\n",CurEvValue);

//确定当前EV值对应的LCD Lvl
//增量
	if ( CurEvValue > EV_LVL_MAX  )
	{
		CurEvValue = EV_LVL_MAX -EV_LVL_MIN ;
	}
	else 
	if ( CurEvValue   < EV_LVL_MIN )
	{
		CurEvValue = 1;
	}
	else
	{
		CurEvValue -= EV_LVL_MIN ;
	}

	#if 0
	if ( CurEvValue >  EV_LVL_DEF )
	{
		adj_bl_value = (CurEvValue - EV_LVL_DEF)*BL_UP_UNIT;
	}else{
		adj_bl_value = (CurEvValue -EV_LVL_DEF )*BL_LOWER_UNIT;
	}
	
	adj_bl_value /= 100;
	
//确定当前EV值
	adj_bl_value = adj_bl_value+LCD_LVL_DEF;
	#endif
	//debug_msg("\r\n CurEvValue:%d \r\n",CurEvValue);
	adj_bl_value = CurEvValue *100 / ( EV_LVL_MAX -EV_LVL_MIN ); 	// (float_t)(100.0/EV_LVL_MAX);//BL_Proportion;
//debug_msg("adj_bl_value:%d \r\n", adj_bl_value);

return adj_bl_value;
	
}

void UI_DetALS_BLCtrl(void) //200ms
{
   uiLCDBackLightCtrlPercent = AdjLCDBackLightLvl();
//	debug_msg("uiLCDBackLightCtrlPercent:%d \r\n",uiLCDBackLightCtrlPercent);
   if(uiLCDBackLightCtrlPercent > 100)
   	{
      		uiLCDBackLightCtrlPercent = 100;
   	}
	if ( uiLCDBackLightCtrlPercent < 1)
	{
			uiLCDBackLightCtrlPercent = 1;
	}
   uiLCDBackLightCtrlPercentCtrlPrev = uiLCDBackLightCtrlPercentPrev;
   uiLCDBackLightCtrlPercentPrev = uiLCDBackLightCtrlPercent;

   //	debug_msg("uiLCDBackLightCtrlPercent:%d \r\n",uiLCDBackLightCtrlPercent);

}

static UINT32 BLC_Cur = 100;
//20ms的周期变化十次, 之后uiLCDBackLightCtrlPercent == uiLCDBackLightCtrlPercentCtrlPrev  1s之后再次变化
void UI_BLC_SeamlessCtrl(void)
{
   #if 1
   static UINT32 BLC_AdjSingleRun = 0;
   if(SysGetFlag(FL_MOVIE_BACKADJUST) == MOVIE_BACKADJUST_ON)
   {
       BLC_AdjSingleRun = abs(uiLCDBackLightCtrlPercent - uiLCDBackLightCtrlPercentCtrlPrev) / 20;	//变化增量缩小20倍
	//debug_msg("BLC_AdjSingleRun:%d \r\n",BLC_AdjSingleRun);
	   
       if(BLC_AdjSingleRun < 1)
       	{
       		BLC_AdjSingleRun = 1;
       }
       if(uiLCDBackLightCtrlPercent > uiLCDBackLightCtrlPercentCtrlPrev)	//增加背光亮度
       {
          BLC_Cur += BLC_AdjSingleRun;
          if(BLC_Cur <= uiLCDBackLightCtrlPercent)		//计算后选择增量最小的
          {
             System_SetLCDBackLightLvlSeamless(BLC_Cur);
          }
          else
          {
             BLC_Cur = uiLCDBackLightCtrlPercent;
             System_SetLCDBackLightLvlSeamless(BLC_Cur);		
          }
       }
       else			//减少背光亮度
       {
          BLC_Cur -= BLC_AdjSingleRun;
          if(BLC_Cur >= uiLCDBackLightCtrlPercent)	  	//计算后选择增量最小的
          {
             System_SetLCDBackLightLvlSeamless(BLC_Cur);
          }
          else
          {
             BLC_Cur = uiLCDBackLightCtrlPercent;
             System_SetLCDBackLightLvlSeamless(BLC_Cur);
          }       
       }
	   //DBGD(BLC_Cur);
	   //	debug_msg("BLC_Cur:%d \r\n",BLC_Cur);
		SysSetFlag(FL_LCD_BRIGHTNESS, BLC_Cur);
   }
   #endif
}
#endif



//#MT#lyb -20200403 -begin
static BOOL bParkPwrOn;
static INT32 iParkPwrOffTime = -1;
UINT32 uiAccPowerOffCount;
BOOL bAccPowerOff = FALSE;
BOOL System_GetGsensorPwrOn(void)
{
    return bParkPwrOn;
}
void System_SetGsensorPwrOn(BOOL bValue)
{
    bParkPwrOn = bValue;
}
void System_DecParkPwroffTimeCount(void)
{
    if(iParkPwrOffTime >= 0)
    {
        iParkPwrOffTime --;
        if(iParkPwrOffTime == -1)
        {
        CHKPNT;
            Ux_PostEvent(NVTEVT_KEY_POWER_REL, 1, NVTEVT_KEY_RELEASE); 
        }
    }
}
void System_SetParkPwroffTimeCount(INT32 iValue)
{
    iParkPwrOffTime = iValue;
}
//#MT#lyb -20200403 -end
UINT32 DrvKey_DetAcc(void)
{
   return rtc_getPWR2Status();
}
extern BOOL DetVbatShutdown(void );
void UI_DetPwrKey(void)
{
	static BOOL isBATLow_shutdowning = FALSE;
	if (!UI_IsForceLock()) {
		GxKey_DetPwrKey();
	}
	if(isBATLow_shutdowning == FALSE){
		if(DetVbatShutdown()){
			if(!DrvKey_DetAcc()){
				CHKPNT;
				isBATLow_shutdowning = TRUE;
				//Ux_PostEvent(NVTEVT_SYSTEM_SHUTDOWN, 1, 0);
				Ux_PostEvent(NVTEVT_KEY_POWER_REL, 1, NVTEVT_KEY_RELEASE);
			}
		}
	}
}

static UINT32 uiSHUTTER1CurKey = 0, uiSHUTTER1PrevKey = 0;
static UINT32 uiSHUTTER1HoldTimeCount;
BOOL  isLCDOff = TRUE;
#include "DxDisplay.h"
extern void SetBLStatus(BOOL status );
void UI_DetLongKey(void)
{
		#if 1 // 2号按键
	  uiSHUTTER1CurKey = rtc_getPWRStatus(); //DrvKey_DetPowerKey();//rtc_getPWRStatus();
	  if(uiSHUTTER1CurKey )
	   uiSHUTTER1HoldTimeCount ++;
	//  debug_msg("\r\n   uiSHUTTER1PrevKey:%d uiSHUTTER1CurKey :%d  uiSHUTTER1HoldTimeCount:%d\r\n",
	//  				    uiSHUTTER1PrevKey, 	uiSHUTTER1CurKey,	uiSHUTTER1HoldTimeCount);
	  if ((uiSHUTTER1PrevKey) && !(uiSHUTTER1CurKey ) && uiSHUTTER1HoldTimeCount < 50 )
	  {
	 //   debug_msg("\r\n   uiSHUTTER1PrevKey:%d uiSHUTTER1CurKey :%d  uiSHUTTER1HoldTimeCount:%d\r\n",
	 // 				    uiSHUTTER1PrevKey, 	uiSHUTTER1CurKey,	uiSHUTTER1HoldTimeCount);

		if  (  isLCDOff == TRUE  )
		{
			isLCDOff = FALSE;
			SetBLStatus(isLCDOff);
			debug_msg("-----turn    off ---\r\n");
		}else{
			isLCDOff = TRUE;
			SetBLStatus(isLCDOff);
			debug_msg("-----turn  on---\r\n");
		}
		uiSHUTTER1HoldTimeCount = 0;
	  }
	  else if((uiSHUTTER1CurKey )&& uiSHUTTER1HoldTimeCount == 50*5)//50是1秒
	  {

	  	uiSHUTTER1HoldTimeCount = 0;
	  	Ux_PostEvent(NVTEVT_KEY_POWER_REL, 1, NVTEVT_KEY_RELEASE);
		debug_msg("\r\n --cj  NVTEVT_KEY_POWER_REL 5s  power off\r\n ");
	  }

	  if((!(uiSHUTTER1CurKey)))
	   uiSHUTTER1HoldTimeCount  = 0;
	  uiSHUTTER1PrevKey = uiSHUTTER1CurKey;
#endif

}

void UI_DetNormalKey(void)
{
	static UINT32 keyDetectCount = 0;
	if (!UI_IsForceLock()) {
		GxKey_DetNormalKey();

		if (keyDetectCount < 4) {
			keyDetectCount++;
		}

		if (keyDetectCount == 2) {
			//recover the key detection after system boot up
			GxKey_SetFirstKeyInvalid(KEY_PRESS, 0);
		}
	}
	UI_DetLongKey();

	UI_BLC_SeamlessCtrl();
}



void UI_DetStatusKey(void)
{
//check mode key
	if ((!UI_IsForceLock()) && (!UI_IsForceLockStatus())) {
		GxKey_DetStatusKey();
	}
}


#if (GSENSOR_FUNCTION == ENABLE)
void UI_DetCustom1Key(void)
{
    BOOL   bGSensorStatus = FALSE;
    Gsensor_Data GS_Data = {0};

    static INT32 iCustom1FilterCount = -1;

    bGSensorStatus = GSensor_GetStatus(&GS_Data);
//	debug_msg("--cj bGSensorStatus:%d  %d\r\n ",bGSensorStatus, iCustom1FilterCount );
    if ((bGSensorStatus == TRUE) && (iCustom1FilterCount == -1)) 
    {
        debug_msg("Gsensor Triggered!!\r\n");
        iCustom1FilterCount = 0;
        //if(!System_GetGsensorPwrOn())
		if (UI_GetData(FL_MOVIE_CYCLIC_REC) == MOVIE_CYCLICREC_OFF)
		{
	        debug_msg("Gsensor off, do nothing!!\r\n");
		}else
		{
            Ux_PostEvent(NVTEVT_KEY_CUSTOM1, 1, NVTEVT_KEY_PRESS);
		}
    }

    if(iCustom1FilterCount >= 0)
    {
        iCustom1FilterCount ++;
        if(iCustom1FilterCount >= 25)
            iCustom1FilterCount = -1;
    }

    
}
#endif


#if (USER_DEF_TOUCH == ENABLE)

#define MASK_BITMAP_W   11
#define MASK_ALL        0xffffffff
static UINT32 uiTouchEventMask = 0;
static UINT32 uiTouchEventMaskBitMap[MASK_BITMAP_W][2] = 
{
    {NVTEVT_PRESS, 1<<1},
    {NVTEVT_RELEASE, 1<<2},
    {NVTEVT_MOVE, 1<<3},
    {NVTEVT_CLICK, 1<<4},
    {NVTEVT_DOUBLECLICK, 1<<5},
    {NVTEVT_USER_SLIDE_LEFT, 1<<6},
    {NVTEVT_USER_SLIDE_RIGHT, 1<<7},
    {NVTEVT_USER_SLIDE_UP, 1<<8},
    {NVTEVT_USER_SLIDE_DOWN, 1<<9},
    {NVTEVT_HOLD, 1<<10},    
    {NVTEVT_USER_RELEASE, 1<<11},
};

void  UI_CancelTouchEventMask(UINT32 event)
{CHKPNT;
	UINT32 i;
	if(event == 0)
	{
	   return;
	}
	else
	{
	   for(i = 0; i < MASK_BITMAP_W; i ++)
	   {
		 if(event == uiTouchEventMaskBitMap[i][0])
		 {
			uiTouchEventMask &= (~uiTouchEventMaskBitMap[i][1]);
			return;
		 }
	   }
	}

}

UINT32 UI_GetTouchEventMask(void)
{
   return uiTouchEventMask;
}
void UI_RestoreTouchEventMask(UINT32 mask)
{
   uiTouchEventMask = mask;
   debug_msg("%s:uiTouchEventMask %08x\r\n",__func__, uiTouchEventMask);
}

void  UI_SetTouchEventMask(UINT32 event)
{CHKPNT;
   UINT32 i;
   if(event == 0)
   {
      uiTouchEventMask = 0;
   }
   else if(event == MASK_ALL)
   {
      uiTouchEventMask = MASK_ALL;
   }
   else
   {
      for(i = 0; i < MASK_BITMAP_W; i ++)
      {
        if(event == uiTouchEventMaskBitMap[i][0])
        {
           uiTouchEventMask |= uiTouchEventMaskBitMap[i][1];
		   return;
        }
      }
   }
}

BOOL  UI_DecideTouchEventMask(UINT32 event)
{
   UINT32 i;
   UINT32 event_bitmap = 0;
   for(i = 0; i < MASK_BITMAP_W; i ++)
   {
      if(event == uiTouchEventMaskBitMap[i][0])
      {
         event_bitmap = uiTouchEventMaskBitMap[i][1];
         break;
      }
   }
   //DBGD(i);
   //DBGD(event);
   //DBGD(event_bitmap);
   //DBGD(uiTouchEventMask);
   if(event_bitmap != 0)
   {
      if((uiTouchEventMask & event_bitmap) != 0)
      return TRUE;
   }
   return FALSE;
}
extern BOOL DetTP_IsPenDown(void);
extern void DetTP_GetXY(INT32 *pX, INT32 *pY);

void UI_DetTouch(void)
{
    BOOL   bTouchPenDown = FALSE;
    static INT32 iTouchX, iTouchY;
  //  static INT32 iTouchX2, iTouchY2;
	
	static UINT32 uiLowTime;
	static UINT32 uiHighHoldTime;

	static BOOL bPressFlag;
    static BOOL bIdleFlag;
	static BOOL bReleaseFlag;
	static INT32 iPressedX, iPressedY;
	static UINT32 uiMoveDir;
	static INT32 iMoveDistanceX, iMoveDistanceY;

    static BOOL bMoveFlag;
//	static UINT32 uiMoveDistanceThr;
//	static UINT32 uiSlideDistanceThr;
	
	static BOOL bDoubleClickFlagDecide;
	static BOOL bDoubleClickFlag;
	static BOOL bDoubleClickFlagExe;
	
    static BOOL bFilterWorking = FALSE;
	static UINT32 bCountTouchTimeForFilter = 0;

	static UINT32 iTouchX_Prev, iTouchY_Prev;
	static BOOL bRememberMoveFlag = FALSE;
	bTouchPenDown = DetTP_IsPenDown();
	if(bTouchPenDown)
	{
       //DetTP_GetXY2Point(&iTouchX,&iTouchY,&iTouchX2,&iTouchY2); 
       DetTP_GetXY(&iTouchX,&iTouchY);

        //#MT#lyb -20200318 -begin
        if(!(iTouchX < 0 || iTouchY < 0))
        {
            //TESTLOG("%d, %d", iTouchX, iTouchY);
            iTouchX = TOUCH_PANEL_W - iTouchX * TOUCH_PANEL_W / TOUCH_PANEL_DRV_W;
            iTouchY = TOUCH_PANEL_H - iTouchY * TOUCH_PANEL_H / TOUCH_PANEL_DRV_H;
        }
        //#MT#lyb -20200318 -end


	//   debug_msg("Liwk----Touch(%d, %d)\r\n",iTouchX,  iTouchY);
	 	iTouchX = iTouchX*1.01 - 5.82;
	 	iTouchY =  iTouchY*1.01	-9.85;
	 	debug_msg("----Touch(X:%d, Y:%d)  uiLowTime:%d\r\n",iTouchX,iTouchY,uiLowTime);

       if(iTouchX < 0 || iTouchY < 0)
       {
         bTouchPenDown = FALSE;
		 bCountTouchTimeForFilter = 0;
	   }
	   else
       {
          if(bFilterWorking)
          {CHKPNT;
              iTouchX_Prev = iTouchX;
              iTouchY_Prev = iTouchY;
		//cj add 0721 
		//TouchX_Prev = iTouchX_Prev;
	//	TouchY_Prev = iTouchX_Prev;
		//cj add 0721
			  bCountTouchTimeForFilter ++;
              return;
          }
		  else if(bCountTouchTimeForFilter > 0 && (abs(iTouchX_Prev - iTouchX) > TOUCH_FILTER_THRESHOLD_W || abs(iTouchY_Prev - iTouchY) > TOUCH_FILTER_THRESHOLD_H))
          {
              bFilterWorking = TRUE;
              debug_msg("^R Filter working!!\r\n");
              Ux_PostTouchEvent(NVTEVT_USER_RELEASE, 2, iTouchX_Prev, iTouchY_Prev);	   
              Ux_PostTouchEvent(NVTEVT_RELEASE, 2, iTouchX_Prev, iTouchY_Prev);
			  //debug_msg("--cj post event 0000000000");
              bIdleFlag = TRUE;
              bPressFlag = FALSE;
              bReleaseFlag = FALSE;
              bDoubleClickFlag = FALSE; 
              bDoubleClickFlagExe = FALSE;
              bDoubleClickFlagDecide = FALSE;
              uiHighHoldTime = 0;
              uiLowTime = 0;
              iTouchX_Prev = iTouchX;
              iTouchY_Prev = iTouchY;
			  bCountTouchTimeForFilter++ ;
              return;
          }
		  bCountTouchTimeForFilter ++;
		  
           uiLowTime = 0;
           // DetTP_GetXY(&iTouchX,&iTouchY);
           if(bIdleFlag || bDoubleClickFlagDecide)
           {
              bPressFlag = TRUE;
              bReleaseFlag = FALSE;
              if(bDoubleClickFlagDecide)
              	{
             	 	CHKPNT;
               //  bDoubleClickFlag = TRUE;
              	}
              bDoubleClickFlagDecide = FALSE;
              iPressedX = iTouchX;
              iPressedY = iTouchY;
              bIdleFlag = FALSE;
              Ux_PostTouchEvent(NVTEVT_PRESS, 2, iTouchX, iTouchY);
			  //debug_msg("-- post NVTEVT_PRESS  event 1111111111111");

           }
           else if(bPressFlag)
           {
              uiHighHoldTime ++;
              if(uiHighHoldTime > 2)
              {
              //debug_msg("iTouchX:%d iPressedX:%d  %d %d\r\n",iTouchX,iPressedX,bMoveFlag,uiHighHoldTime );
                 iMoveDistanceX = iTouchX - iPressedX;
                 iMoveDistanceY = iTouchY - iPressedY;
				 
				 if(bMoveFlag && (uiHighHoldTime % TOUCH_MOVE_EVENT_SEND_INTERVAL) == 0)
				 {
				     Ux_PostTouchEvent(NVTEVT_USER_MOVE, 2, iTouchX, iTouchY);
				//	 Ux_PostTouchEvent(NVTEVT_MOVE, 2, iTouchX, iTouchY);
				     //debug_msg("-- post NVTEVT_USER_MOVE event 222222222222");

				 }
                 if(abs(iMoveDistanceX) > TOUCH_MOVE_THRESHOLD_W || abs(iMoveDistanceY) > TOUCH_MOVE_THRESHOLD_H)
                 {
                    if(/*(abs(iTouchX_Prev - iTouchX) > 3 || abs(iTouchY_Prev - iTouchY) > 3) &&*/ !bMoveFlag)//finger move?
                    {
                       bMoveFlag = TRUE;
					   bRememberMoveFlag = TRUE;
                       Ux_PostTouchEvent(NVTEVT_USER_MOVE, 2, iTouchX, iTouchY);
				//	   					 Ux_PostTouchEvent(NVTEVT_MOVE, 2, iTouchX, iTouchY);

				  //debug_msg("-- post NVTEVT_USER_MOVE event 3333333333333333");

                    }
                    if(abs(iMoveDistanceX) > TOUCH_SLIDE_THRESHOLD_W || abs(iMoveDistanceY) > TOUCH_SLIDE_THRESHOLD_H)
                    {
                       if(abs(iMoveDistanceX) < abs(iMoveDistanceY))
                       {
                          if(iMoveDistanceY < 0)
                          {
                             uiMoveDir = TOUCH_MOVE_DIR_UP;
                          }
                          else
                          {
                             uiMoveDir = TOUCH_MOVE_DIR_DOWN;
                          }
                       }
                       else
                       {
                          if(iMoveDistanceX < 0)
                          {
                             uiMoveDir = TOUCH_MOVE_DIR_LEFT;
                          }
                          else
                          {
                             uiMoveDir = TOUCH_MOVE_DIR_RIGHT;
                          }
                       }
                    }
                 }
              }
              if(uiHighHoldTime == 15 && !bMoveFlag)
              {
                 Ux_PostTouchEvent(NVTEVT_HOLD, 2, iTouchX, iTouchY);
				 //debug_msg("-- post event 444444444444444");

              }
           }
           iTouchX_Prev = iTouchX;
           iTouchY_Prev = iTouchY;
       }
	}
    if(!bTouchPenDown)
    {
       bCountTouchTimeForFilter = 0;
       bMoveFlag = FALSE;
	   bFilterWorking = FALSE;
       uiLowTime ++;
    }
	if(uiLowTime >= 10)//200ms	//低电平时间大于200ms   无触摸
	{
	   bIdleFlag = TRUE;
	   bPressFlag = FALSE;
	   bReleaseFlag = FALSE;
	#if  0 // cj 20200730if 1==>if 0 //// CJ  change
	   if(bDoubleClickFlagDecide == TRUE && !UI_DecideTouchEventMask(NVTEVT_DOUBLECLICK) && !bRememberMoveFlag)
	   {
        //   Ux_PostTouchEvent(NVTEVT_CLICK, 2, iPressedX, iPressedY);
		   debug_msg("-- post event NVTEVT_CLICK 55555555555555555 %d\r\n", uiLowTime);

	   }
	   #endif
	   bRememberMoveFlag = FALSE;
	   bDoubleClickFlag = FALSE; 
	   uiHighHoldTime = 0;
	   uiMoveDir = 0;
       bDoubleClickFlagExe = FALSE;
       bDoubleClickFlagDecide = FALSE;
	}
	else if(uiLowTime > 1 && bPressFlag)
	{
	   bPressFlag = FALSE;
	   bReleaseFlag = TRUE;	   
	   Ux_PostTouchEvent(NVTEVT_USER_RELEASE, 2, iTouchX, iTouchY);   
	   Ux_PostTouchEvent(NVTEVT_RELEASE, 2, iTouchX, iTouchY);
	   //debug_msg("--cj post event 666666666666\r\n");
	   	#if 1 // cj 20200730if 0==>if 1 // CJ  ADD 
		if ( !bRememberMoveFlag)
		{
			 if(bMoveFlag == FALSE)
          	{
				Ux_PostTouchEvent(NVTEVT_CLICK, 2, iPressedX, iPressedY);
				debug_msg("-- post event NVTEVT_CLICK aaaaaaaaaaaaaaaaa %d \r\n",bDoubleClickFlagDecide  );
			}
		}
		#endif
	   if(UI_DecideTouchEventMask(NVTEVT_DOUBLECLICK) && !bRememberMoveFlag)
	   {
	   CHKPNT;
	  
          if(bMoveFlag == FALSE)
          {
              Ux_PostTouchEvent(NVTEVT_CLICK, 2, iPressedX, iPressedY);
			  debug_msg("-- post event NVTEVT_CLICK 7777777777777");

              bDoubleClickFlagExe = FALSE;
              bDoubleClickFlagDecide = FALSE;
              bIdleFlag = TRUE;
              bPressFlag = FALSE;
              bReleaseFlag = FALSE;
              bDoubleClickFlag = FALSE; 
          }
	   }
	   else
       {
          if(uiHighHoldTime < 10 && bMoveFlag == FALSE) // 200ms press for 200ms or under
          {
          #if 1
              if(bDoubleClickFlag == TRUE)
              { 
 				 bIdleFlag = TRUE;
 				 bPressFlag = FALSE;
 				 bReleaseFlag = FALSE;
 				 bDoubleClickFlag = FALSE; 
				 CHKPNT;
 				 bDoubleClickFlagExe = TRUE;
 				 bDoubleClickFlagDecide = FALSE;
                 Ux_PostTouchEvent(NVTEVT_USER_DOUBLECLICK, 2, iPressedX, iPressedY);
				 debug_msg("-- post NVTEVT_USER_DOUBLECLICK event 888888888888888");
                 //Ux_PostTouchEvent(NVTEVT_DOUBLECLICK, 2, iPressedX, iPressedY);
              }
		  #endif
          }	   
	   }
	   if(uiMoveDir > 0)
	   { 
	       if(uiHighHoldTime < 30)
	       {
               switch(uiMoveDir)
               {
				case TOUCH_MOVE_DIR_UP:			 	
    				Ux_PostTouchEvent(NVTEVT_USER_SLIDE_UP, 2, iTouchX, iTouchY);
					debug_msg("-- post event 99999999999999");
    				break;
				case TOUCH_MOVE_DIR_DOWN:
					 debug_msg("-- post event aaaaaaaaaaa");
					 Ux_PostTouchEvent(NVTEVT_USER_SLIDE_DOWN, 2, iTouchX, iTouchY);
    				 break;
				case TOUCH_MOVE_DIR_LEFT:
    				 Ux_PostTouchEvent(NVTEVT_USER_SLIDE_LEFT, 2, iTouchX, iTouchY);
					 debug_msg("-- post event bbbbbbbbbbbbbb");
    				 break;
				case TOUCH_MOVE_DIR_RIGHT:
    				 Ux_PostTouchEvent(NVTEVT_USER_SLIDE_RIGHT, 2, iTouchX, iTouchY); 
					 debug_msg("-- post event ccccccccccccccccccc");
    				 break;
				default:
    			 	 break;
               }
	       }
	       uiMoveDir = 0;
	   }
	   
	   uiHighHoldTime = 0;

	   if(uiLowTime < 10 && bDoubleClickFlagExe == FALSE)
	   	{
	   //	CHKPNT;
	   	  bDoubleClickFlagDecide = TRUE;
	   	}
	   
	   if(bDoubleClickFlagExe == TRUE)
	   	{
	   	//	CHKPNT;
	   	  bDoubleClickFlagExe = FALSE;
	   	}
	   
	}
}

char* sEventName[21]={
    "NVTEVT_PRESS",
   	"NVTEVT_RELEASE",
   	"NVTEVT_MOVE",
   	"NVTEVT_CLICK",
   	"NVTEVT_DOUBLECLICK",
   	"NVTEVT_SLIDE_LEFT",
   	"NVTEVT_SLIDE_RIGHT",
   	"NVTEVT_SLIDE_UP",
   	"NVTEVT_SLIDE_DOWN",
   	"NVTEVT_HOLD",
   	"NVTEVT_USER_CLICK",
   	"NVTEVT_USER_DOUBLECLICK",
   	"NVTEVT_USER_SLIDE_LEFT",
   	"NVTEVT_USER_SLIDE_RIGHT",
   	"NVTEVT_USER_SLIDE_UP",
   	"NVTEVT_USER_SLIDE_DOWN",
   	"unknown",
};
//NVTEVT_USER_DOUBLECLICK
char * GetPressEventName(UINT32 param)
{
   if(param >= NVTEVT_TOUCH_EVT_START && param <= NVTEVT_TOUCH_EVT_START + 20)
   {
	   return sEventName[param - NVTEVT_TOUCH_EVT_START];
   }
   else
   	   return sEventName[20];
}

static UINT32 uiMaskEventStored = 0;

void Ux_PostTouchEvent(NVTEVT evt, UINT32 paramNum, INT32 DataX, INT32 DataY)
{ 
	//debug_msg("%s %d\r\n",GetPressEventName(evt),KeyScan_GetPlugDev());
	//debug_msg("--TouchEvent is %d\r\n",evt);
   if(KeyScan_GetPlugDev() != PLUG_HDMI && KeyScan_GetPlugDev() != PLUG_TV)
   {
       //debug_msg("%s\r\n",GetPressEventName(evt));
       if(!SysInput_isLcdBackLightOn())
	   {CHKPNT;
	      if(NVTEVT_PRESS == evt)
	      {
		     SysInput_SetLcdBackLight(TRUE);
			 //save touch event mask
			 uiMaskEventStored = UI_GetTouchEventMask();
			// DBGD(uiMaskEventStored);
			 UI_SetTouchEventMask(MASK_ALL);
		  }
          if(paramNum == 2)
          {
             if(!UI_DecideTouchEventMask(evt))
                Ux_PostEvent(evt, paramNum, DataX, DataY);
          }
          else if(paramNum == 0)
          {
             if(!UI_DecideTouchEventMask(evt))
                Ux_PostEvent(evt, 0);	      
          }	  
		  return;
	   }
	   
	   if(NVTEVT_PRESS == evt && MASK_ALL == UI_GetTouchEventMask())
	   {
	      //restore touch event mask
	      UI_RestoreTouchEventMask(uiMaskEventStored);
	   }
	   if(paramNum == 2)
	   {//CHKPNT;
	      if(!UI_DecideTouchEventMask(evt)){
		  	//CHKPNT;
             Ux_PostEvent(evt, paramNum, DataX, DataY);
	      }
	   }
	   else if(paramNum == 0)
	   {
	      if(!UI_DecideTouchEventMask(evt))
		     Ux_PostEvent(evt, 0);	      
	   }
   }
}

#endif
/////////////////////////////////////////////////////////////////////////////
extern void System_ResetDetCloseLenCount(void);
extern void System_ResetPowerSaveCount(void);
BOOL g_bConsumeStatus = FALSE;

#include "EthCamAppCmd.h"
INT32 System_UserKeyFilter(NVTEVT evt, UINT32 paramNum, UINT32 *paramArray)
{
	UINT32 key = evt;
	if (IN_RANGE_EVENT(key, NVTEVT_KEY_STATUS_START, NVTEVT_KEY_STATUS_END)) { //Status class
		System_ResetDetCloseLenCount();
		System_ResetPowerSaveCount();
#if (STATUSKEY_FUNCTION == ENABLE)
		DBG_IND("^Bgroup key event=0x%x\r\n", key);

		if (g_bConsumeStatus) {
			g_bConsumeStatus = 0;
		} else {
			UINT32 uiDscMode = Input_Key2Mode(key);
			DBG_IND("^YDscMode=0x%x\r\n", uiDscMode);
			//FlowMode_OnKeyMode(uiDscMode);
			UI_Switch_DscMode(uiDscMode, DSCMODE_SWITCH_FORCE, DSCMODE_SWITCHDIR_DONT_CARE);
		}
#endif
		return NVTEVT_CONSUME;
	} else if (IN_RANGE_EVENT(key, NVTEVT_KEY_BUTTON_START, NVTEVT_KEY_BUTTON_END)) { //Button class
		if (IN_RANGE_EVENT(key, NVTEVT_KEY_PRESS_START, NVTEVT_KEY_PRESS_END)) { //Press key
			if (key == NVTEVT_KEY_POWER) {
				return NVTEVT_CONSUME;
			}

			System_ResetDetCloseLenCount();
			if (GxPower_GetControl(GXPWR_CTRL_SLEEP_LEVEL) > 1) { // drop key if sleep level > 1
				//NOTE! do not set GXPWR_CTRL_SLEEP_RESET before get GXPWR_CTRL_SLEEP_LEVEL,
				//  because GXPWR_CTRL_SLEEP_LEVEL status maybe alter by sleep wakeup flow~
				System_ResetPowerSaveCount();
				return NVTEVT_CONSUME;
			} else {
				System_ResetPowerSaveCount();
				if (m_uiAnyKeyUnlockEn) {
					Ux_PostEvent(NVTEVT_KEY_PRESS_START, 1, key);
					return NVTEVT_CONSUME;
				}
				return NVTEVT_PASS;
			}
		} else if (IN_RANGE_EVENT(key, NVTEVT_KEY_CONTINUE_START, NVTEVT_KEY_CONTINUE_END)) { //Contine key
			if (key == NVTEVT_KEY_POWER_CONT) {
				return NVTEVT_CONSUME;
			}

			System_ResetDetCloseLenCount();
			System_ResetPowerSaveCount();
			return NVTEVT_PASS;
		} else if (IN_RANGE_EVENT(key, NVTEVT_KEY_RELEASE_START, NVTEVT_KEY_RELEASE_END)) { //Release key
			if (key == NVTEVT_KEY_POWER_REL) {
				System_ResetDetCloseLenCount();
				if (GxPower_GetControl(GXPWR_CTRL_SLEEP_LEVEL) > 1) { // drop key if sleep level > 1
					//NOTE! do not set GXPWR_CTRL_SLEEP_RESET before get GXPWR_CTRL_SLEEP_LEVEL,
					//  because GXPWR_CTRL_SLEEP_LEVEL status maybe alter by sleep wakeup flow~
					System_ResetPowerSaveCount();
				} else {
					//#NT#2016/03/07#KCHong -begin
					//#NT#Low power timelapse function
#if (TIMELAPSE_LPR_FUNCTION == ENABLE)
					MovieTLLPR_Process(TIMELAPSE_FROM_PWRKEY);
#endif
					//#NT#2016/03/07#KCHong -end
					int j=0;
					for (j=0; j<ETH_REARCAM_CAPS_COUNT; j++)
					{
					CHKPNT;
						EthCam_SendXMLCmd(j, ETHCAM_PORT_DEFAULT ,ETHCAM_CMD_TX_POWEROFF, 0);
					}
					System_PowerOff(SYS_POWEROFF_NORMAL);
					
				}
				return NVTEVT_CONSUME;
			}

			System_ResetDetCloseLenCount();
			System_ResetPowerSaveCount();
			return NVTEVT_PASS;
		}
	}
	return NVTEVT_PASS;
}

#if !defined(_TOUCH_OFF_)
INT32 System_UserTouchFilter(NVTEVT evt, UINT32 paramNum, UINT32 *paramArray)
{
	INT32 Ret = NVTEVT_PASS;
	//DBG_DUMP("^Bevt=%d, point=(%d, %d)\r\n", evt - NVTEVT_PRESS, paramArray[0], paramArray[1]);
	if (evt >= NVTEVT_PRESS && evt <= NVTEVT_SLIDE_DOWN) {
		if (GxPower_GetControl(GXPWR_CTRL_SLEEP_LEVEL) > 1) { // drop key if sleep level > 1
			//NOTE! do not set GXPWR_CTRL_SLEEP_RESET before get GXPWR_CTRL_SLEEP_LEVEL,
			//  because GXPWR_CTRL_SLEEP_LEVEL status maybe alter by sleep wakeup flow~
			System_ResetPowerSaveCount();
			return NVTEVT_CONSUME;
		} else {
			System_ResetPowerSaveCount();
			return NVTEVT_PASS;
		}
	}
	return Ret;
}
#endif

BOOL SysInput_isLcdBackLightOn(void)
{
    return gpio_getPin(P_GPIO_11);
}
void SysInput_SetLcdBackLight(BOOL status)
{
   if(status)
    gpio_setPin(P_GPIO_11);
   else
   	gpio_clearPin(P_GPIO_11);
}
#include "UIFlowMovieFuncs.h"
extern BOOL isEnterParkingMode;
extern BOOL bNOACC ;
UINT32 NOACCCount = 0;
char         gev_cMovieStampStr[13][256] = {0};
char         g_EVMovieStampStr[13][256];
BOOL isOpenMsgWin = FALSE;
extern BOOL g_EnterParkingMode_CloseBL;
static BOOL g_WDTStatus = FALSE;
void UI_DetUserFunc(void)
{
    System_DecParkPwroffTimeCount();

	// 0716
	UINT32 i;
	UINT32 Ipl_id=ImageApp_MovieMulti_VePort2Imglink(i);
	if(Ipl_id<IPL_ID_MAX_NUM){
		IQS_GetLogString(Ipl_id, &g_EVMovieStampStr[i][0], 256);
	}
	AdjLCDBackLightLvl();

#if  0//MOVIE_AE_LOG
			////<< spark add AE function
			UINT32 Ipl_id=ImageApp_MovieMulti_VePort2Imglink(i);
			if(Ipl_id<IPL_ID_MAX_NUM){
				IQS_GetLogString(Ipl_id, &gev_cMovieStampStr[i][0], 256);
				debug_msg("--EV :%s",gev_cMovieStampStr);
			}
#endif
	if ( bNOACC && !DetVbatShutdown())    //no acc open and bat is full
	{	DBGD(NOACCCount);
		if(GxSystem_GetPowerOnSource() != GX_PWRON_SRC_PWR_VBAT){
			if( NOACCCount == 63) // 13s
			{
				if ( gMovData.State == MOV_ST_REC)
				{
					Ux_PostEvent(NVTEVT_KEY_SHUTTER2, 1, NVTEVT_KEY_PRESS);
					Delay_DelayMs(500);
				}
				Ux_PostEvent(NVTEVT_KEY_POWER_REL, 1, NVTEVT_KEY_RELEASE);
			}
			NOACCCount++;
		}else if(GxSystem_GetPowerOnSource() == GX_PWRON_SRC_PWR_VBAT)
		{
			if(!isEnterParkingMode) 	//zjf add 20200825 exit parking EMR do not power off 
			{
				uiAccPowerOffCount = 0;
				NOACCCount=0;
			}
		}

		if ( DrvKey_DetAcc() )
		{
			bNOACC = FALSE;
			NOACCCount= 0;
		}
			
	}
	
    if(!DrvKey_DetAcc() && !DetVbatShutdown())// no acc and bat is full 
    {   
        if(System_GetGsensorPwrOn()||bNOACC){
            uiAccPowerOffCount = 0;
        } else{
            uiAccPowerOffCount ++;
			//CHKPNT;
			//DBGD(uiAccPowerOffCount);

			if(UI_GetData(FL_MOVIE_PARK) != MOVIE_PARK_OFF){
				if((!isEnterParkingMode) && (!bNOACC)){
					//CHKPNT;
					isEnterParkingMode = TRUE;
					UI_SetData(FL_MOVIE_BACKADJUST, FALSE);	
					Ux_OpenWindow(&UIFlowWndWaitMomentCtrl, 1, UIFlowWndWaitMoment_StatusTXT_Msg_STRID_ENTER_PARKING);
				}
			}else if(!isOpenMsgWin){
				isOpenMsgWin = TRUE;
				Ux_OpenWindow(&UIFlowWndWaitMomentCtrl, 1, UIFlowWndWaitMoment_StatusTXT_Msg_STRID_ENTER_SHUTDOWN);				
			}
	            if(uiAccPowerOffCount >= 15)
	            {
	                if((uiAccPowerOffCount % 5) == 0)// if power off interrupted, do it again every 5 seconds
	                {
	                    debug_msg("Liwk ------ ACC power off!!!\r\n");

					if ( gMovData.State == MOV_ST_REC)
					{
						Ux_PostEvent(NVTEVT_KEY_SHUTTER2, 1, NVTEVT_KEY_PRESS);
						Delay_DelayMs(500);
					}
					CHKPNT;
					Ux_CloseWindow(&UIFlowWndWaitMomentCtrl,0);
                	Ux_PostEvent(NVTEVT_KEY_POWER_REL, 1, NVTEVT_KEY_RELEASE);
                }
            }
        }
    }else{  // if ACC On, Cancel Park poweroff time count;
      
    	if(isOpenMsgWin == TRUE)
    	{
			isOpenMsgWin = FALSE;
			Ux_CloseWindow(&UIFlowWndWaitMomentCtrl,0);
		}
        uiAccPowerOffCount = 0;
		if(isEnterParkingMode){
			isEnterParkingMode	=FALSE;
			Ux_CloseWindow(&UIFlowWndWaitMomentCtrl,0);
		}		
        if(System_GetGsensorPwrOn())
        {  
            debug_msg("ACC IN\r\n");
            System_SetGsensorPwrOn(FALSE);
            System_SetParkPwroffTimeCount(-1);
			g_EnterParkingMode_CloseBL = FALSE;
			SetBLStatus(TRUE);
			if(UI_GetData(FL_MOVIE_BACKADJUST_MENU) != UI_GetData(FL_MOVIE_BACKADJUST)){
				if(UI_GetData(FL_MOVIE_BACKADJUST_MENU) == MOVIE_BACKADJUST_ON){
					UI_SetData(FL_MOVIE_BACKADJUST,UI_GetData(FL_MOVIE_BACKADJUST_MENU));
				}
			}else{
				System_SetLCDBackLightLvlSeamless(99);
			}
			//DX_HANDLE cDispDev = 0;
			//Dx_GetObject(DX_CLASS_DISPLAY_EXT|DX_TYPE_LCD); 
			//((DX_OBJECT *)cDispDev)->pfState(DXSET|DRVDISP_STATE_BACKLIGHT, DRVDISP_BACKLIGHT_ON);
        }
    }

	
#if(STARTWDT_FUNCTION==ENABLE)
	SysFeedWDT();
#endif
    
}



static char currPlayFileName[MAX_WAV_NAME_LEN] ;
static UINT32 currPlayTimes;
//extern BOOL	CompareCurrWavFile(char *pWavFileNane, UINT32 nPlayTime);


BOOL	CompareCurrWavFile(char *pWavFileNane, UINT32 nPlayTime)
{
	if(strncmp(pWavFileNane,currPlayFileName,MAX_WAV_NAME_LEN) !=0)
	{
		return FALSE;
	}
	
	if(nPlayTime !=currPlayTimes)
		return FALSE;
	return TRUE;

}

UINT32 GetCurrPlayWavTimes(void)
{
	return currPlayTimes;
}


char *GetWavFileNamePlaying(void)
{
	return currPlayFileName;
}


/*******************************************
each 20ms 
*******************************************/
void UI_DetWavPlayControl(void)
{

	static char FileName[MAX_WAV_NAME_LEN];
	static UINT32  uPlayTimes;
	static UINT8  uPlayInterval;  // 1lsb is 10ms
	static UINT32 PauseCnt;
	switch(GetWavPlayCtrlStatus())
	{
		case WAVPLAYCTRL_IDEL:
			if (GxSound_IsPlaying() == FALSE)
			{
				//debug_msg("mmmm put wav\r\n");
				FileName[0] = 0;  
				WavPlayCtrl_GetWavToPlay(FileName,&uPlayTimes,&uPlayInterval);
				if(FileName[0] != 0)
				{

					//debug_msg("mmmmb %s, %d\r\n",FileName,uPlayTimes);

					if(uPlayTimes >500)
					{
						//GxSound_Play_Repeat(-1, 1000);  //max is 1000
						SetWavPlayCtrlStatus(WAVPLAYCTRL_INFINITE_LOOP);
					}
					else if(uPlayTimes >0)
					{
						//debug_msg("mmmm play times =%d\r\n",uPlayTimes);
						//GxSound_Play_Repeat(-1, uPlayTimes);  //max is 1000
						SetWavPlayCtrlStatus(WAVPLAYCTRL_INFINITE_LOOP);
					}
					else
					{
						//GxSound_Play_Repeat(-1, 1);  //max is 1000
						SetWavPlayCtrlStatus(WAVPLAYCTRL_LOOP_ON);
						
						
					}

					WavPlay_PlayWavData(FileName);

					//debug_msg("wwwww wav id =%d\r\n",GetWavWavIdByFileName(FileName));

					
					#if 1
					memcpy(currPlayFileName, FileName, MAX_WAV_NAME_LEN);
					currPlayTimes = uPlayTimes;
					#endif

					
				}

			}
			break;


		case WAVPLAYCTRL_LOOP_ON:
			if (GxSound_IsPlaying() == FALSE)
			{
				 //play N times
				if(--uPlayTimes == 0)
				{
					SetWavPlayCtrlStatus(WAVPLAYCTRL_IDEL);
				}
				else
				{
					SetWavPlayCtrlStatus(WAVPLAYCTRL_LOOP_PAUSE);
					//TO SET PAUSE TIME
					// Pause time is different from each wav
					PauseCnt = uPlayInterval/4;	

				}

			}
			break;



		case WAVPLAYCTRL_LOOP_PAUSE:
			if(PauseCnt >1 )
			{
				PauseCnt--;
			}
			else 
			{
				SetWavPlayCtrlStatus(WAVPLAYCTRL_LOOP_ON);
				
				WavPlay_PlayWavData(FileName);			
			}
			

			break;


		case WAVPLAYCTRL_INFINITE_LOOP:
			if (GxSound_IsPlaying() == FALSE)
			{
				SetWavPlayCtrlStatus(WAVPLAYCTRL_IDEL);

			}
			break;



	}


}

