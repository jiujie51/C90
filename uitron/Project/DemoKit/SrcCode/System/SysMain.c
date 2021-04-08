////////////////////////////////////////////////////////////////////////////////
#include "SysCommon.h"
////////////////////////////////////////////////////////////////////////////////
#include "UIFramework.h"
#include "UIFrameworkExt.h"
#include "AppLib.h"

#include "FileSysTsk.h"

#ifdef __ECOS
#include "ecosmain.h"
#endif

//Mode
#include "UIModeMain.h"
#if (PHOTO_MODE == ENABLE)
#include "UIModePhoto.h"
#endif
#include "UIModeMovie.h"
#if (PLAY_MODE == ENABLE)
#include "UIModePlayback.h"
#include "UIModePlayMenu.h"
#endif
#if (USB_MODE == ENABLE)
#include "UIModeUsbDisk.h"
#include "UIModeUsbPTP.h"
#include "UIModeUsbCam.h"
#include "UIModeUsbMenu.h"
#endif
#if (SLEEP_MODE == ENABLE)
#include "UIModeSleep.h"
#endif
#if (UPDFW_MODE == ENABLE)
#include "UIModeUpdFw.h"
#endif
#if(WIFI_FUNC==ENABLE)
#include "UIModeWifi.h"
#endif
#if (UCTRL_FUNC)
#include "UCtrlMain.h"
#endif
#if defined(_ENABLE_)
#include "ASR.h"
#endif

#if (_ALPHAVISION_LICENSE_ == ENABLE)
#include "Anfa_License.h"
#endif

#include "UIModeIPCam.h"
//global debug level: PRJ_DBG_LVL
#include "PrjCfg.h"
#include "UIModePlayMenu.h"

//#include "UIModePlayMenu.h"
//local debug level: THIS_DBGLVL
#define THIS_DBGLVL         2 // 0=FATAL, 1=ERR, 2=WRN, 3=UNIT, 4=FUNC, 5=IND, 6=MSG, 7=VALUE, 8=USER
///////////////////////////////////////////////////////////////////////////////
#define __MODULE__          SysMain
#define __DBGLVL__          ((THIS_DBGLVL>=PRJ_DBG_LVL)?THIS_DBGLVL:PRJ_DBG_LVL)
#define __DBGFLT__          "*" //*=All, [mark]=CustomClass
#include "DebugModule.h"

#include "wdt.h"

extern void System_BootStart(void);
extern void System_BootEnd(void);
extern void System_ShutDownStart(void);
extern void System_ShutDownEnd(void);

BOOL g_bIsInitSystemFinish = TRUE;

int SX_TIMER_BURNIN_AUTORUN_ID = -1;

//////////////////////////////////////
UINT32 UI_EventTraceLvl = 0;

int UserMainProc_Init(void);
int UserMainProc_Exit(void);

#if(STARTWDT_FUNCTION==ENABLE)
static BOOL bWDTStatus = FALSE;
BOOL SysIsWDTWorking(void)
{
	return bWDTStatus;
}
void SysStartWDT(void)
{
	if(bWDTStatus == FALSE)
	{		
		wdt_close();
		Delay_DelayUs(1000); //wait message output complet
		
		if(wdt_open()){
			DBG_ERR("wdt open failed!\r\n");
		}
		wdt_setConfig(WDT_CONFIG_ID_MODE, WDT_MODE_RESET);
		wdt_setConfig(WDT_CONFIG_ID_TIMEOUT, 5000); // 5s time out
		wdt_enable();
		wdt_clearTimeout();
		DBG_DUMP("wdt open success\r\n");
	    bWDTStatus = TRUE;
	}
	
}
void SysStopWDT(void)
{
       if(bWDTStatus == TRUE)
       {
            bWDTStatus = FALSE;
            wdt_clearTimeout(); 
            wdt_trigger();
            wdt_disable();
            wdt_close();
			Delay_DelayUs(1000); //wait message output complet
       }
}
void SysFeedWDT(void)
{
	if(bWDTStatus == TRUE)
	{
		wdt_clearTimeout(); 
		wdt_trigger();
	}
}
/*
extern UINT32 gWDT_AE_Cnt;
static UINT32 ae_hang_cnt=0;
void WDT_ResetSensorVDSignal_AECnt(void)
{
	ae_hang_cnt=0;
}
void WDT_SensorVDSignal_Check(UINT32 overcnt)
{
    static UINT32 pre_wdt_ae_cnt=0xffffffff;
    if(pre_wdt_ae_cnt == gWDT_AE_Cnt){
        ae_hang_cnt++;
        //debug_msg(" ====ae_hang_cnt:%d=======overcnt:%d===== \r\n",ae_hang_cnt,overcnt);
        if(ae_hang_cnt>overcnt){
            DBG_ERR("mmmmmmmmm  Gvd hang reset\r\n");
            wdt_waitTimeout();
        }
    }else
       ae_hang_cnt = 0;
    
    pre_wdt_ae_cnt = gWDT_AE_Cnt;
    if(gWDT_AE_Cnt > 0xfffff)
        gWDT_AE_Cnt = 0;
}*/
#endif


//=========
//for wdt
#if (SYSWDT_FUNC  ==  ENABLE)

#define PRI_SYS_WDT          30
#define STKSIZE_SYS_WDT        2048//
UINT32 SYS_WDT_TASK_ID = 0;
UINT32 FLG_ID_SYS_WDT_TASK = 0;
extern void SysWdtTsk(void);


static BOOL bWdtTaskOpened = FALSE;

BOOL Sys_Wdt_Open(void)
{
    sta_tsk(SYS_WDT_TASK_ID,0);
    loc_cpu();
	bWdtTaskOpened = TRUE;
    unl_cpu();
		return TRUE;
}


BOOL Sys_Wdt_Close(void)
{
	if(!bWdtTaskOpened)
		return FALSE;
	
    ter_tsk(SYS_WDT_TASK_ID);
    loc_cpu();
		bWdtTaskOpened = FALSE;
    unl_cpu();
		return TRUE;

}


void	Sys_Wdt_InstallID(void)
{
    OS_CONFIG_TASK(SYS_WDT_TASK_ID, PRI_SYS_WDT, STKSIZE_SYS_WDT, SysWdtTsk);
    OS_CONFIG_FLAG(FLG_ID_SYS_WDT_TASK);

}


void SysWdtTsk(void)
{

	wdt_open();
	wdt_setConfig(WDT_CONFIG_ID_MODE,WDT_MODE_RESET);
	wdt_setConfig(WDT_CONFIG_ID_TIMEOUT,8000);	//ms
	wdt_trigger();
	wdt_enable();

	while(1)
	{
        	Delay_DelayMs(1000);

		if( System_GetState(SYS_STATE_CURRMODE) ==  PRIMARY_MODE_MOVIE ) 
		{
				//debug_msg("mmmmmm feed dog __movie\r\n");
				wdt_clearTimeout();
				wdt_trigger();
			
		}else {
				wdt_clearTimeout();
				wdt_trigger();
				//debug_msg("mmmmmm feed dog  __nomovie\r\n");

		}

	}

}
#endif
//=========

void UserWaitEvent(NVTEVT wait_evt, UINT32 *wait_paramNum, UINT32 *wait_paramArray)
{
	NVTEVT evt = 0; //fix for CID 45083
	UINT32 paramNum = 0; //fix for CID 45084
	UINT32 paramArray[MAX_MESSAGE_PARAM_NUM] = {0}; //fix for CID 45085
	BOOL bQuit = FALSE;

	while (!bQuit) {
		PROFILE_TASK_IDLE();
		Ux_WaitEvent(&evt, &paramNum, paramArray);
		if (evt) {
			INT32 result = NVTEVT_PASS;
#if (UCTRL_FUNC)
			BOOL  isUctrlEvent = FALSE;
#endif
			PROFILE_TASK_BUSY();
			if (UI_EventTraceLvl >= 5) {
				DBG_DUMP("MSG: get event 0x%08x!\r\n", evt);
			}

#if (UCTRL_FUNC)
			if (evt & UCTRL_EVENT_MASK) {
				isUctrlEvent = TRUE;
				evt &= ~UCTRL_EVENT_MASK;
			}
#endif

			if (evt == wait_evt) {
				if (wait_paramNum) {
					(*wait_paramNum) = paramNum;
				}
				if (wait_paramArray) {
					memcpy(wait_paramArray, paramArray, sizeof(UINT32)*MAX_MESSAGE_PARAM_NUM);
				}
				result = NVTEVT_CONSUME;
				bQuit = TRUE;
			}
			if (IS_APP_EVENT(evt)) {
				result = Ux_AppDispatchMessage(evt, paramNum, paramArray);
			}
			/*
			{
			VControl* pObj;
			if(Ux_GetRootWindow(&pObj) != NVTRET_OK)
			    continue;
			}
			*/
#if(UI_FUNC==ENABLE)
			if (result != NVTEVT_CONSUME) {
				if (System_GetState(SYS_STATE_CURRMODE) != SYS_MODE_UNKNOWN) {
					result = Ux_WndDispatchMessage(evt, paramNum, paramArray);
				}
			}
#endif
			if ((UI_EventTraceLvl > 0) && (result != NVTEVT_CONSUME)) {
				DBG_DUMP("^YWRN: MSG: skip event 0x%08x!\r\n", evt);
			}

#if(UI_FUNC==ENABLE)
			if (System_GetState(SYS_STATE_CURRMODE) != SYS_MODE_UNKNOWN) {
				Ux_Redraw();
			}
#endif

#if (UCTRL_FUNC)
			// run uctrl callback
			if (isUctrlEvent) {
				UctrlMain_EventEnd(0, evt);
			}
#endif
		}
	}
}

extern void System_InstallAppObj(void);

BOOL bUI_Quit = FALSE;
void UI_Quit(void)
{
	bUI_Quit = TRUE;
}

/////////////////////////////////////////////////////////////////////////////

void System_InstallModeObj(void)
{
	//register project mode
	PRIMARY_MODE_MAIN = System_AddMode(&gModeMain); //an empty mode for calibration
#if (PLAY_MODE == ENABLE)
	PRIMARY_MODE_PLAYBACK = System_AddMode(&gModePlay);
#endif
#if (PHOTO_MODE == ENABLE)
	PRIMARY_MODE_PHOTO = System_AddMode(&gModePhoto);
#endif
#if (MOVIE_MODE == ENABLE)
	PRIMARY_MODE_MOVIE = System_AddMode(&gModeMovie);
#endif
#if (USB_MODE == ENABLE)
	PRIMARY_MODE_USBMSDC = System_AddMode(&gModeUsbDisk);
	PRIMARY_MODE_USBMENU = System_AddMode(&gModeUsbMenu);
	PRIMARY_MODE_USBPCC = System_AddMode(&gModeUsbCam);
#endif
#if (SLEEP_MODE == ENABLE)
	PRIMARY_MODE_SLEEP = System_AddMode(&gModeSleep);
#endif
#if (IPCAM_MODE == ENABLE)
	PRIMARY_MODE_IPCAM = System_AddMode(&gModeIPCam);
#endif
#if (UPDFW_MODE == ENABLE)
	PRIMARY_MODE_UPDFW = System_AddMode(&gModeUpdFw);
	SYS_SUBMODE_UPDFW = System_AddSubMode(&gSubModeUpdFw);
#endif
#if(WIFI_FUNC==ENABLE)
	SYS_SUBMODE_WIFI = System_AddSubMode(&gSubModeWiFi);
#endif
#if (UVC_MULTIMEDIA_FUNC == ENABLE)
	SYS_SUBMODE_UVC = System_AddSubMode(&gSubModeUvc);
#endif
	PRIMARY_MODE_PLAYMENU = System_AddMode(&gModePlayMenu);

}

/////////////////////////////////////////////////////////////////////////////
INT32 System_UserKeyFilter(NVTEVT evt, UINT32 paramNum, UINT32 *paramArray);
INT32 System_UserTouchFilter(NVTEVT evt, UINT32 paramNum, UINT32 *paramArray);
#if (USB_MODE == ENABLE)
BOOL gIsUSBChargePreCheck = FALSE;
#endif
UINT32 bCalConsumeFps =FALSE;
UINT32 ConsumeEvtCnt =0;
UINT32 EvtSum=0;
UINT32 ConsumeFps =0;
UINT32 ConsumeUnit = 40;
#include "UIInfo.h"
BOOL bNOACC = FALSE;
BOOL g_EnterParkingMode_CloseBL = FALSE;
void UserMainProc(void)
{
	NVTEVT evt = 0; //fix for CID 45081
	UINT32 paramNum = 0; //fix for CID 45082
	UINT32 paramArray[MAX_MESSAGE_PARAM_NUM] = {0};
	//debug_msg("event loop - begin!\r\n");
	TM_BOOT_BEGIN("flow", "preboot");
	//////////////////////////////////////////////////////////////
	UserMainProc_Init();
	//////////////////////////////////////////////////////////////

// USB_CONNECT_CHARGER
	debug_msg("--cj  GxUSB_GetConnectType:%d ",GxUSB_GetConnectType());
//#MT#lyb -20200403 -begin
    if(GxSystem_GetPowerOnSource() == GX_PWRON_SRC_PWR_VBAT) 
    {
        debug_msg("^Y================= Power ON source is GSensor!!!================= \r\n");
		if (!rtc_getPWR2Status() )
		{
			g_EnterParkingMode_CloseBL = TRUE;
			DBGD(g_EnterParkingMode_CloseBL);
		}
    } 
    else if(GxSystem_GetPowerOnSource() == GX_PWRON_SRC_PWR_VBUS)
    {
		System_SetGsensorPwrOn(TRUE); // CJ  2020 0426
        debug_msg("^Y================= Power ON source is VBus!!!================= \r\n");
    }
    else if(GxSystem_GetPowerOnSource() == GX_PWRON_SRC_PWR_SW)
    {
    //按键开机
        debug_msg("^Y================= Power ON source is SW!!!================= \r\n");
	
    }
    else if(GxSystem_GetPowerOnSource() == GX_PWRON_SRC_PWR_SW2)
    {// ACC开机
        debug_msg("^Y================= Power ON source is SW2!!!================= \r\n");
		//add cj 20190612
		if (UI_GetData(FL_MOVIE_PARK) == MOVIE_PARK_OFF )
		{
		 //暂时取消
			//rtc_resetShutdownTimer();
			debug_msg("cj-------ACC open,but FL_MOVIE_PARK =  %d ,so poweroff\r\n",UI_GetData(FL_MOVIE_PARK) );
			//rtc_poweroffPWR();
			//while(1);
		}
    }
    else//GX_PWRON_SRC_PWR_ALM
    {
        debug_msg("^Y================= Power ON source is ALM!!!================= \r\n");
    }

	if (! rtc_getPWR2Status() )
	{
		bNOACC = TRUE;
		DBGD(bNOACC);
	}
	
    debug_msg("wdt_getResetNum: %d\r\n", wdt_getResetNum());
//#MT#lyb -20200403 -end
#if(STARTWDT_FUNCTION==ENABLE)
	SysStartWDT();
#endif



    
#if(UCTRL_FUNC == ENABLE)
	System_OnUctrl();
#endif
	System_InstallAppObj(); //install VControl type list of App Object and UIControl Object
	System_InstallModeObj(); //install SYS_MODE objects
	TM_BOOT_END("flow", "preboot");
	//CHKPNT;
	Delay_DelayMs(200);
	Ux_SendEvent(0, NVTEVT_SYSTEM_BOOT, 1, 1);

#if (POWERON_FAST_BOOT == ENABLE)
	System_Debug_Msg(TRUE);
#endif

#if (_BOARD_DRAM_SIZE_ > 0x04000000 && !defined(_NVT_ETHREARCAM_RX_))
#ifdef __ECOS
	ecos_main();
#endif
#endif
	while (!bUI_Quit) {
		PROFILE_TASK_IDLE();
		Ux_WaitEvent(&evt, &paramNum, paramArray);
		if (evt) {
			INT32 result = NVTEVT_PASS;
			UINT32 EvtBegin=0,EvtEnd=0;
#if (UCTRL_FUNC)
			BOOL  isUctrlEvent = FALSE;
#endif
			PROFILE_TASK_BUSY();
			if (UI_EventTraceLvl >= 5) {
				DBG_DUMP("MSG: get event 0x%08x!\r\n", evt);
			}

			if(bCalConsumeFps) {
				EvtBegin = Perf_GetCurrent();
				ConsumeEvtCnt++;
			}
#if (UCTRL_FUNC)
			if (evt & UCTRL_EVENT_MASK) {
				isUctrlEvent = TRUE;
				evt &= ~UCTRL_EVENT_MASK;
			}
#endif
#if(UI_FUNC==ENABLE)
			if (IS_KEY_EVENT(evt)) {
				result = System_UserKeyFilter(evt, paramNum, paramArray);
			}

#if !defined(_TOUCH_OFF_)
           #if(USER_DEF_TOUCH == DISABLE)
			if (result != NVTEVT_CONSUME) {
				if (IS_TOUCH_EVENT(evt)) {
					result = System_UserTouchFilter(evt, paramNum, paramArray);
				}
			}
			#endif
#endif
#endif

			if (result != NVTEVT_CONSUME) {
				if (IS_APP_EVENT(evt)) {
					result = Ux_AppDispatchMessage(evt, paramNum, paramArray);
				}
			}
#if(UI_FUNC==ENABLE)
			if (result != NVTEVT_CONSUME) {
				if (System_GetState(SYS_STATE_CURRMODE) != SYS_MODE_UNKNOWN) {
					result = Ux_WndDispatchMessage(evt, paramNum, paramArray);
				}
			}
#endif
			if ((UI_EventTraceLvl > 0) && (result != NVTEVT_CONSUME)) {
				DBG_DUMP("^YWRN: MSG: skip event 0x%08x!\r\n", evt);
			}

#if(UI_FUNC==ENABLE)
			if (System_GetState(SYS_STATE_CURRMODE) != SYS_MODE_UNKNOWN) {
				Ux_Redraw();
			}
#endif
#if 0 //background task wait done flag,need to remove
			// A special case.
			// UI background is busy until finishing the event handler of BACKGROUND_DONE
			if (evt == NVTEVT_BACKGROUND_DONE) {
				BKG_Done();
			}
#endif
#if (UCTRL_FUNC)
			// run uctrl callback
			if (isUctrlEvent) {
				UctrlMain_EventEnd(0, evt);
			}
#endif


			if(bCalConsumeFps) {
				EvtEnd = Perf_GetCurrent();

				EvtSum += (EvtEnd-EvtBegin);
				if(ConsumeEvtCnt%ConsumeUnit==0){
					ConsumeFps = ConsumeEvtCnt*1000000/EvtSum;
					DBG_DUMP("^Y1 evt cost %d us (get evt fps %d)\r\n",EvtSum/ConsumeEvtCnt,ConsumeFps);
					//reset count,calculate next 40 event
					EvtSum=0;
					ConsumeEvtCnt=0;
				}
			}
			//return result;

			// Avoid system hang-up when Ist losing event. Check Ist losing event here!
			extern void dev_checkIstLosingEvent(void);
			dev_checkIstLosingEvent();
		}
	}

	//////////////////////////////////////////////////////////////
	UserMainProc_Exit();
	//////////////////////////////////////////////////////////////
	GxSystem_PowerOff();
	//debug_msg("event loop - end!\r\n");
}
extern void LogFile_DumpMemAndSwReset(void);

///< error status callback
void UserErrCb(UXUSER_CB_TYPE evt, UINT32 p1, UINT32 p2, UINT32 p3)
{
	static BOOL g_has_dump = FALSE;
	if (g_has_dump == FALSE) {
		g_has_dump = TRUE;
		Ux_DumpEvents();
		SxCmd_DoCommand("ker dump");
	}
	#if (LOGFILE_FUNC==ENABLE)
	LogFile_DumpMemAndSwReset();
	#endif
}


void UserCalConsumeFps(UINT32 bEn,UINT32 unit)
{
	if(bEn){
		//reset count,calculate next 40 event
		EvtSum=0;
		ConsumeEvtCnt=0;
		if(unit)
			ConsumeUnit = unit;
	}
	bCalConsumeFps = bEn;

	DBG_DUMP("bCalConsumeFps %d ConsumeUnit %d\r\n",bCalConsumeFps,ConsumeUnit);
}


//////////////////////////////////////////////////////////////

#if (POWERON_FAST_BOOT == ENABLE)

// Services
extern void UserMainProc2(void);
extern void UserMainProc3(void);
extern void UserMainProc4(void);
extern void UserMainProc5(void);
extern void UserMainProc6(void);
extern void UserMainProc7(void);
extern void UserMainProc8(void);
UINT32 USERINIT_FLG_ID = 0;
UINT32 USEREXIT_FLG_ID = 0;
UINT32 USERPROC2_ID = 0;
UINT32 USERPROC3_ID = 0;
UINT32 USERPROC4_ID = 0;
UINT32 USERPROC5_ID = 0;
UINT32 USERPROC6_ID = 0;
UINT32 USERPROC7_ID = 0;
UINT32 USERPROC8_ID = 0;
//#define PRI_NVTUSER                 10
//#define PRI_FILESYS                 10
//#define PRI_FSBACKGROUND            9
#define PRI_USERPROC2               12 //ensure its priority is lower then PRI_NVTUSER
#define PRI_USERPROC3               12 //ensure its priority is lower then PRI_NVTUSER
#define PRI_USERPROC4               12 //ensure its priority is lower then PRI_NVTUSER
#define PRI_USERPROC5               12 //ensure its priority is lower then PRI_NVTUSER
#define PRI_USERPROC6               12 //ensure its priority is lower then PRI_NVTUSER
#define PRI_USERPROC7               12 //ensure its priority is lower then PRI_NVTUSER
#define PRI_USERPROC8               12 //ensure its priority is higher then PRI_NVTUSER (for network IPC)
#define STKSIZE_USERPROC2           2048
#define STKSIZE_USERPROC3           2048
#define STKSIZE_USERPROC4           2048
#define STKSIZE_USERPROC5           2048
#define STKSIZE_USERPROC6           2048
#define STKSIZE_USERPROC7           2048
#define STKSIZE_USERPROC8           2048
#endif

void UserMainProc_InstallID(void)
{
#if (POWERON_FAST_BOOT == ENABLE)
	OS_CONFIG_FLAG(USERINIT_FLG_ID);
	OS_CONFIG_FLAG(USEREXIT_FLG_ID);
	OS_CONFIG_TASK(USERPROC2_ID,     PRI_USERPROC2,       STKSIZE_USERPROC2,        UserMainProc2);
	OS_CONFIG_TASK(USERPROC3_ID,     PRI_USERPROC3,       STKSIZE_USERPROC3,        UserMainProc3);
	OS_CONFIG_TASK(USERPROC4_ID,     PRI_USERPROC4,       STKSIZE_USERPROC4,        UserMainProc4);
	OS_CONFIG_TASK(USERPROC5_ID,     PRI_USERPROC5,       STKSIZE_USERPROC5,        UserMainProc5);
	OS_CONFIG_TASK(USERPROC6_ID,     PRI_USERPROC6,       STKSIZE_USERPROC6,        UserMainProc6);
	OS_CONFIG_TASK(USERPROC7_ID,     PRI_USERPROC7,       STKSIZE_USERPROC7,        UserMainProc7);
	OS_CONFIG_TASK(USERPROC8_ID,     PRI_USERPROC8,       STKSIZE_USERPROC8,        UserMainProc8);
#endif
}

BOOL bSystemFlow = 1;

int UserMainProc_Init(void)
{
	System_BootStart();
	bSystemFlow = 1;
#if (POWERON_FAST_BOOT == ENABLE)
	INIT_CLRFLAG(FLGINIT_MASKALL);

	INIT_SETFLAG(FLGINIT_BEGIN); //set by itself
	sta_tsk(USERPROC2_ID, 0);
	sta_tsk(USERPROC3_ID, 0);
	sta_tsk(USERPROC4_ID, 0);
	sta_tsk(USERPROC5_ID, 0);
	sta_tsk(USERPROC6_ID, 0);
	sta_tsk(USERPROC7_ID, 0);
	sta_tsk(USERPROC8_ID, 0);
#endif
//CHKPNT;
	SystemInit();

#if (POWERON_FAST_BOOT == ENABLE)
	INIT_SETFLAG(FLGINIT_END);
#endif

#if defined(_ASR_ENABLE_)
		ASR_Init();
#endif

	System_BootEnd();
	return 0;
}

int UserMainProc_Exit(void)
{
	System_ShutDownStart();
	bSystemFlow = 0;

#if defined(_ASR_ENABLE_)
		ASR_UnInit();
#endif
#if (_ALPHAVISION_LICENSE_ == ENABLE)
    AnfaAlgAuthProc_UnInit();
#endif

#if (POWERON_FAST_BOOT == ENABLE)
	EXIT_CLRFLAG(FLGEXIT_MASKALL);

	EXIT_SETFLAG(FLGEXIT_BEGIN); //set by itself
	sta_tsk(USERPROC2_ID, 0);
	sta_tsk(USERPROC3_ID, 0);
	sta_tsk(USERPROC4_ID, 0);
	sta_tsk(USERPROC5_ID, 0);
	sta_tsk(USERPROC6_ID, 0);
	sta_tsk(USERPROC7_ID, 0);
	sta_tsk(USERPROC8_ID, 0);
#endif
	SystemExit();

#if (POWERON_FAST_BOOT == ENABLE)
	EXIT_SETFLAG(FLGEXIT_END);
#endif
	System_ShutDownEnd();
	return 0;
}

#if (POWERON_FAST_BOOT == ENABLE)
void UserMainProc2(void)
{
	kent_tsk();
	if (bSystemFlow == 1) {
		INIT_WAITFLAG(FLGINIT_BEGIN2); //wait until trigger begin
		SystemInit2();
		INIT_SETFLAG(FLGINIT_END2);
	} else {
		EXIT_WAITFLAG(FLGEXIT_BEGIN2); //wait until trigger begin
		SystemExit2();
		EXIT_SETFLAG(FLGEXIT_END2);
	}
	ext_tsk();
}

void UserMainProc3(void)
{
	kent_tsk();
	if (bSystemFlow == 1) {
		INIT_WAITFLAG(FLGINIT_BEGIN3); //wait until trigger begin
		SystemInit3();
		INIT_SETFLAG(FLGINIT_END3);
	} else {
		EXIT_WAITFLAG(FLGEXIT_BEGIN3); //wait until trigger begin
		SystemExit3();
		EXIT_SETFLAG(FLGEXIT_END3);
	}
	ext_tsk();
}

void UserMainProc4(void)
{
	kent_tsk();
	if (bSystemFlow == 1) {
		INIT_WAITFLAG(FLGINIT_BEGIN4); //wait until trigger begin
		SystemInit4();
		INIT_SETFLAG(FLGINIT_END4);
	} else {
		EXIT_WAITFLAG(FLGEXIT_BEGIN4); //wait until trigger begin
		SystemExit4();
		EXIT_SETFLAG(FLGEXIT_END4);
	}
	ext_tsk();
}

void UserMainProc5(void)
{
	kent_tsk();
	if (bSystemFlow == 1) {
		INIT_WAITFLAG(FLGINIT_BEGIN5); //wait until trigger begin
		SystemInit5();
		INIT_SETFLAG(FLGINIT_END5);
	} else {
		EXIT_WAITFLAG(FLGEXIT_BEGIN5); //wait until trigger begin
		SystemExit5();
		EXIT_SETFLAG(FLGEXIT_END5);
	}
	ext_tsk();
}

void UserMainProc6(void)
{
	kent_tsk();
	if (bSystemFlow == 1) {
		INIT_WAITFLAG(FLGINIT_BEGIN6); //wait until trigger begin
		SystemInit6();
		INIT_SETFLAG(FLGINIT_END6);
	} else {
		EXIT_WAITFLAG(FLGEXIT_BEGIN6); //wait until trigger begin
		SystemExit6();
		EXIT_SETFLAG(FLGEXIT_END6);
	}
	ext_tsk();
}

void UserMainProc7(void)
{
	kent_tsk();
	if (bSystemFlow == 1) {
		INIT_WAITFLAG(FLGINIT_BEGIN7); //wait until trigger begin
		SystemInit7();
		INIT_SETFLAG(FLGINIT_END7);
	} else {
		EXIT_WAITFLAG(FLGEXIT_BEGIN7); //wait until trigger begin
		SystemExit7();
		EXIT_SETFLAG(FLGEXIT_END7);
	}
	ext_tsk();
}

void UserMainProc8(void)
{
	kent_tsk();
	if (bSystemFlow == 1) {
		INIT_WAITFLAG(FLGINIT_BEGIN8); //wait until trigger begin
		SystemInit8();
		INIT_SETFLAG(FLGINIT_END8);
	} else {
		EXIT_WAITFLAG(FLGEXIT_BEGIN8); //wait until trigger begin
		SystemExit8();
		EXIT_SETFLAG(FLGEXIT_END8);
	}
	ext_tsk();
}

#endif

void System_WaitFS(void)
{
	static BOOL is_fs_ready = FALSE;
    if (!is_fs_ready) {
		//CHKPNT;
		TM_BOOT_BEGIN("Fs", "WaitAttach");
		#if (POWERON_FAST_BOOT == ENABLE)
		if (INIT_CHKFLAG(FLGINIT_STRGATH) == 0){
		#else
		// POWERON_WAIT_FS_READY set ENABLE, already wait on System_OnBoot()
		if (POWERON_WAIT_FS_READY == DISABLE){
		#endif
			DBG_IND("Wait FileSys b\r\n");
			UserWaitEvent(NVTEVT_STRG_ATTACH, NULL, NULL);
			DBG_DUMP("Wait FileSys e\r\n");
		}
		TM_BOOT_END("Fs", "WaitAttach");
		is_fs_ready = TRUE;
    }

}

