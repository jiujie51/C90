
#include "UIFramework.h"
#include "UIFrameworkExt.h"
#include "UIFlow.h"
#include "JpgEnc.h"
#include "Exif.h"
#include "Utility.h"
#include "Audio.h"
#include "HwClock.h"
//#include "PhotoTsk.h"

static CHAR    g_RecMaxTimeStr[20] = {0};
static UINT32  g_RecMaxTime = 0;
static CHAR    g_RecCurrTimeStr[20] = {0};
static UINT32  g_RecCurrTime = 0;
//static CHAR    date_str[20] = {0};
//static CHAR    time_str[20] = {0};

//MOVIE_SIZE_TAG
#if 0
static CHAR    *resolution_Buf[MOVIE_SIZE_ID_MAX] = {
	[MOVIE_SIZE_FRONT_2880x2160P50] = "UHD P50",
	[MOVIE_SIZE_FRONT_3840x2160P30] = "UHD P30",
	[MOVIE_SIZE_FRONT_2880x2160P24] = "UHD P24",
	[MOVIE_SIZE_FRONT_2704x2032P60] = "2.7K P60",
	[MOVIE_SIZE_FRONT_2560x1440P80] = "QHD P80",
	[MOVIE_SIZE_FRONT_2560x1440P60] = "QHD P60",
	[MOVIE_SIZE_FRONT_2560x1440P30] = "QHD P30",
	[MOVIE_SIZE_FRONT_2304x1296P30] = "3MHD P30",
	[MOVIE_SIZE_FRONT_1920x1080P120] = "FHD P120",
	[MOVIE_SIZE_FRONT_1920x1080P96] = "FHD P96",
	[MOVIE_SIZE_FRONT_1920x1080P60] = "FHD P60",
	[MOVIE_SIZE_FRONT_1920x1080P30] = "FHD P30",
	[MOVIE_SIZE_FRONT_1280x720P120] = "HD P120",
	[MOVIE_SIZE_FRONT_1280x720P240] = "HD P240",
	[MOVIE_SIZE_FRONT_1280x720P60] = "HD P60",
	[MOVIE_SIZE_FRONT_1280x720P30] = "HD P30",
	[MOVIE_SIZE_FRONT_848x480P30] = "WVGA P30",
	[MOVIE_SIZE_FRONT_640x480P240] = "VGA P240",
	[MOVIE_SIZE_FRONT_640x480P30] = "VGA P30",
	[MOVIE_SIZE_FRONT_320x240P30] = "QVGA P30",
	[MOVIE_SIZE_DUAL_2560x1440P30_2560x1440P30] = "QHD P30+QHD P30",
	[MOVIE_SIZE_DUAL_2560x1440P30_1280x720P30] = "QHD P30+HD P30",
	[MOVIE_SIZE_DUAL_2304x1296P30_1280x720P30] = "3MHD P30+HD P30",
	[MOVIE_SIZE_DUAL_1920x1080P60_1920x1080P60] = "FHD P60+FHD P60",
	[MOVIE_SIZE_DUAL_1920x1080P30_1920x1080P30] = "FHD P30+FHD P30",
	[MOVIE_SIZE_DUAL_1920x1080P30_1280x720P30] = "FHD P30+HD P30",
	[MOVIE_SIZE_DUAL_1920x1080P30_848x480P30] = "FHD P30+WVGA P30",
	[MOVIE_SIZE_CLONE_1920x1080P30_1920x1080P30] = "FHD P30+FHD P30",
	[MOVIE_SIZE_CLONE_1920x1080P30_1280x720P60] = "FHD P30+HD P60",
	[MOVIE_SIZE_CLONE_1920x1080P30_1280x720P30] = "FHD P30+HD P30",
	[MOVIE_SIZE_CLONE_2560x1440P30_848x480P30] = "QHD P30+WVGA P30",
	[MOVIE_SIZE_CLONE_2304x1296P30_848x480P30] = "3MHD P30+WVGA P30",
	[MOVIE_SIZE_CLONE_1920x1080P60_848x480P30] = "FHD P60+WVGA P30",
	[MOVIE_SIZE_CLONE_1920x1080P60_640x360P30] = "FHD P60+VGA P30",
	[MOVIE_SIZE_CLONE_1920x1080P30_848x480P30] = "FHD P30+WVGA P30",
	[MOVIE_SIZE_CLONE_2048x2048P30_480x480P30] = "2048x2048 P30 + 480x480 P30",
	[MOVIE_SIZE_CLONE_1280x720P60_1280x720P60] = "HD P60 + HD P60",
};
#endif

void FlowMovie_IconDrawMaxRecTime(VControl *pCtrl)
{
	if (GxStrg_GetDevice(0) == NULL) {
		//No Storage, direct to return.
		return;
	}

	memset((void *)g_RecMaxTimeStr, 0, sizeof(g_RecMaxTimeStr));
	g_RecMaxTime = MovieExe_GetMaxRecSec();

	if (g_RecMaxTime <= 2) {
		FlowMovie_SetRecMaxTime(0);
	} else if (g_RecMaxTime > 86399) {
		///23:59:59
		FlowMovie_SetRecMaxTime(86399);
	} else {
		FlowMovie_SetRecMaxTime(g_RecMaxTime);
	}

	if (System_GetState(SYS_STATE_CARD)  == CARD_REMOVED) {
		snprintf(g_RecMaxTimeStr, 20, "%02d:%02d:%02d", 0, 0, 0);
	} else {
		snprintf(g_RecMaxTimeStr, 20, "%02d:%02d:%02d", g_RecMaxTime / 3600, (g_RecMaxTime % 3600) / 60, (g_RecMaxTime % 3600) % 60);
	}

	UxStatic_SetData(pCtrl, STATIC_VALUE, Txt_Pointer(g_RecMaxTimeStr));
	UxCtrl_SetShow(pCtrl, TRUE);
}

void FlowMovie_IconHideMaxRecTime(VControl *pCtrl)
{
	UxCtrl_SetShow(pCtrl, FALSE);
}
extern BOOL  bflag_EthLinkFinish;

void FlowMovie_IconDrawRecTime(VControl *pCtrl)
{
//#NT#2016/03/07#KCHong -begin
//#NT#Low power timelapse function
#if (TIMELAPSE_LPR_FUNCTION == ENABLE)
	UINT32 TimelapseMode = 0;
#endif
//#NT#2016/03/07#KCHong -end

	switch (gMovData.State) {
	case MOV_ST_REC:
	case MOV_ST_REC | MOV_ST_ZOOM:
		//#NT#2016/03/07#KCHong -begin
		//#NT#Low power timelapse function
#if (TIMELAPSE_LPR_FUNCTION == ENABLE)
		if (MovieTLLPR_GetStatus() == (TL_FLOW_LPR | TL_STATE_RECORD)) {
			TimelapseMode = UI_GetData(FL_MOVIE_TIMELAPSE_REC);
			if (TimelapseMode >= TL_LPR_TIME_MIN_PERIOD) {
				if (TimelapseMode == MOVIE_TIMELAPSEREC_OFF) {
					snprintf(g_RecCurrTimeStr, 20, "OFF");
				} else if (TimelapseMode == MOVIE_TIMELAPSEREC_1SEC) {
					snprintf(g_RecCurrTimeStr, 20, "1 SEC");
				} else if (TimelapseMode == MOVIE_TIMELAPSEREC_5SEC) {
					snprintf(g_RecCurrTimeStr, 20, "5 SEC");
				} else if (TimelapseMode == MOVIE_TIMELAPSEREC_10SEC) {
					snprintf(g_RecCurrTimeStr, 20, "10 SEC");
				} else if (TimelapseMode == MOVIE_TIMELAPSEREC_30SEC) {
					snprintf(g_RecCurrTimeStr, 20, "30 SEC");
				} else if (TimelapseMode == MOVIE_TIMELAPSEREC_1MIN) {
					snprintf(g_RecCurrTimeStr, 20, "1 MIN");
				} else if (TimelapseMode == MOVIE_TIMELAPSEREC_5MIN) {
					snprintf(g_RecCurrTimeStr, 20, "5 MIN");
				} else if (TimelapseMode == MOVIE_TIMELAPSEREC_10MIN) {
					snprintf(g_RecCurrTimeStr, 20, "10 MIN");
				} else if (TimelapseMode == MOVIE_TIMELAPSEREC_30MIN) {
					snprintf(g_RecCurrTimeStr, 20, "30 MIN");
				} else if (TimelapseMode == MOVIE_TIMELAPSEREC_1HOUR) {
					snprintf(g_RecCurrTimeStr, 20, "1 HOUR");
				} else if (TimelapseMode == MOVIE_TIMELAPSEREC_2HOUR) {
					snprintf(g_RecCurrTimeStr, 20, "2 HOUR");
				} else if (TimelapseMode == MOVIE_TIMELAPSEREC_3HOUR) {
					snprintf(g_RecCurrTimeStr, 20, "3 HOUR");
				} else if (TimelapseMode == MOVIE_TIMELAPSEREC_1DAY) {
					snprintf(g_RecCurrTimeStr, 20, "1 DAY");
				}
				UxStatic_SetData(pCtrl, STATIC_VALUE, Txt_Pointer(g_RecCurrTimeStr));
				UxCtrl_SetShow(pCtrl, TRUE);
				break;
			}
		}
#endif
		//#NT#2016/03/07#KCHong -end
		g_RecCurrTime = FlowMovie_GetRecCurrTime();
		memset((void *)g_RecCurrTimeStr, 0, sizeof(g_RecCurrTimeStr));
		snprintf(g_RecCurrTimeStr, 20, "%02d:%02d:%02d", g_RecCurrTime / 3600, (g_RecCurrTime % 3600) / 60, (g_RecCurrTime % 3600) % 60);
		UxStatic_SetData(pCtrl, STATIC_VALUE, Txt_Pointer(g_RecCurrTimeStr));
		
		//UxCtrl_SetShow(pCtrl, TRUE);//CANCLE THIS FUNCTION 0728
		break;
	}
}

void FlowMovie_IconDrawRec(VControl *pCtrl)
{
	UxState_SetData(pCtrl, STATE_CURITEM, UIFlowWndMovie_Status_REC_ICON_REC_TRANSPAENT);
}

void FlowMovie_IconHideRec(VControl *pCtrl)
{
	UxState_SetData(pCtrl, STATE_CURITEM, UIFlowWndMovie_Status_REC_ICON_REC_TRANSPAENT);
}


//#MT#lyb -20200331 -begin
/*
w = y + [y/4] + [c/4] -2c + [26(m+1)/10] + d - 1

y: year(the last two numbers);
c: current century - 1;
m: month(1 , 2 shift to 13, 14; y-1, c-1 )
d: day
*/
UINT32 CalWeek_CurDay(UINT32 year, UINT32 month, UINT32 day)
{
    UINT32 century = 0, yeartemp = 0, CurWeek_day = 0;

    if(month == 1 || month == 2)
    {
        year = year - 1;
        month = month + 12;
    }
    century = year / 100 + 1 - 1;
    yeartemp = year % 100;

    CurWeek_day = yeartemp + (UINT32)(yeartemp/4) + (UINT32)(century/4) - 2*century + (UINT32)(26*(month+1)/10) + day - 1;
    CurWeek_day = CurWeek_day % 7;

    return CurWeek_day;
}
//#MT#lyb -20200331 -end

void FlowMovie_IconDrawDateTime(void)
{
    struct tm Curr_DateTime;
    Curr_DateTime = HwClock_GetTime(TIME_ID_CURRENT);

    static CHAR date_strMonth[20] = {0}, date_strDay[3] = {0};
    CHAR cTime_HourH, cTime_HourL, cTime_MinH, cTime_MinL;
    static BOOL bDot_draw = FALSE;

    //Date
    snprintf(date_strMonth, sizeof(date_strMonth), "%02d", Curr_DateTime.tm_mon);
    snprintf(date_strDay, 3, "%02d", Curr_DateTime.tm_mday);
    UxStatic_SetData(&UIFlowWndMovie_Static_DateTxtMCtrl, STATIC_VALUE, Txt_Pointer(date_strMonth));
    UxStatic_SetData(&UIFlowWndMovie_Static_DateTxtDCtrl, STATIC_VALUE, Txt_Pointer(date_strDay));

    //Time
    cTime_HourH = Curr_DateTime.tm_hour / 10;
    cTime_HourL = Curr_DateTime.tm_hour % 10;
    cTime_MinH = Curr_DateTime.tm_min / 10;
    cTime_MinL = Curr_DateTime.tm_min % 10;
    UxState_SetData(&UIFlowWndMovie_Status_TimeHourHCtrl, STATE_CURITEM, cTime_HourH);
    UxState_SetData(&UIFlowWndMovie_Status_TimeHourLCtrl, STATE_CURITEM, cTime_HourL);
    UxState_SetData(&UIFlowWndMovie_Status_TimeMinHCtrl, STATE_CURITEM, cTime_MinH);
    UxState_SetData(&UIFlowWndMovie_Status_TimeMinLCtrl, STATE_CURITEM, cTime_MinL);

    //week
    Curr_DateTime.tm_wday = CalWeek_CurDay(Curr_DateTime.tm_year, Curr_DateTime.tm_mon, Curr_DateTime.tm_mday);
    UxState_SetData(&UIFlowWndMovie_Status_WeekCtrl, STATE_CURITEM, Curr_DateTime.tm_wday);


    //DOT
    if(bDot_draw){
        bDot_draw = FALSE;
        UxCtrl_SetShow(&UIFlowWndMovie_Static_MarkCtrl, FALSE);
    } else {
        bDot_draw = TRUE;
        UxCtrl_SetShow(&UIFlowWndMovie_Static_MarkCtrl, TRUE);
    }

    UxCtrl_SetShow(&UIFlowWndMovie_Panel_DatetimeCtrl, TRUE);
}

void FlowMovie_IconHideDateTime(void)
{
    UxCtrl_SetShow(&UIFlowWndMovie_Panel_DatetimeCtrl, FALSE);
}


void FlowMovie_IconDrawTopState(void)
{
    //set status UIFlowWndMovie_Status_Icon0Ctrl, Icon1, etc.
    //debug_msg("status: %d %d %d %d \r\n", GxUSB_GetIsUSBPlug(), UI_GetData(FL_MOVIE_AUDIO ), UI_GetData(FL_WIFI ), UI_GetData(FL_MOVIE_PARK) );
	//BATTERY 
/*	if(GxUSB_GetIsUSBPlug())

	{
		UxState_SetData(&UIFlowWndMovie_Status_Icon3Ctrl, STATE_CURITEM, UIFlowWndMovie_Status_Icon3_ICON_TOP_BATTERY);
		UxCtrl_SetShow(&UIFlowWndMovie_Status_Icon3Ctrl, TRUE);

	}else {
		UxCtrl_SetShow(&UIFlowWndMovie_Status_Icon3Ctrl, FALSE);

	}
	//*/
	// AUDIO
	if (UI_GetData(FL_MOVIE_AUDIO ) == MOVIE_AUDIO_ON)
	{
		UxState_SetData(&UIFlowWndMovie_Status_Icon0Ctrl, STATE_CURITEM, UIFlowWndMovie_Status_Icon0_ICON_TOP_AUDIO);
		UxCtrl_SetShow(&UIFlowWndMovie_Status_Icon0Ctrl, TRUE);

	}else if ( UI_GetData(FL_MOVIE_AUDIO ) == MOVIE_AUDIO_OFF )
	{
		UxState_SetData(&UIFlowWndMovie_Status_Icon0Ctrl, STATE_CURITEM, UIFlowWndMovie_Status_Icon0_ICON_TOP_AUDIO_OFF);
		UxCtrl_SetShow(&UIFlowWndMovie_Status_Icon0Ctrl, TRUE);
	}
	
	//WIFI 
	if (UI_GetData(FL_WIFI ) == WIFI_ON)
	{
		CHKPNT;
		UxState_SetData(&UIFlowWndMovie_Status_Icon1Ctrl, STATE_CURITEM, UIFlowWndMovie_Status_Icon1_ICON_TOP_WIFI);
		UxCtrl_SetShow(&UIFlowWndMovie_Status_Icon1Ctrl, TRUE);

	}else if (UI_GetData(FL_WIFI ) == WIFI_OFF)
	{
		CHKPNT;
		UxCtrl_SetShow(&UIFlowWndMovie_Status_Icon1Ctrl, FALSE);
	}
	//PARK
	  
	if (UI_GetData(FL_MOVIE_PARK) != MOVIE_PARK_OFF)
	{
	  
		UxState_SetData(&UIFlowWndMovie_Status_Icon2Ctrl, STATE_CURITEM, UIFlowWndMovie_Status_Icon2_ICON_TOP_PARK);
		UxCtrl_SetShow(&UIFlowWndMovie_Status_Icon2Ctrl, TRUE);
	}else if (UI_GetData(FL_MOVIE_PARK) == MOVIE_PARK_OFF)
	{
		UxCtrl_SetShow(&UIFlowWndMovie_Status_Icon2Ctrl, FALSE);
	}
	//分辨率
	//	UxState_SetData(&UIFlowWndMovie_Status_Icon3Ctrl, STATE_CURITEM, UIFlowWndMovie_Status_Icon3_ICON_TOP_2K);
	//if(EthCamNet_GetEthLinkStatus(0) != ETHCAM_LINK_UP ||(!bflag_EthLinkFinish) )
	{
	//	UxState_SetData(&UIFlowWndMovie_StatusTextCtrl, STATE_CURITEM, UIFlowWndMovie_StatusText_STRID_1920P30);
	}
	
	
	if ( gMovData.State == MOV_ST_REC )
	{
	
	}else  {
		UxCtrl_SetShow(&UIFlowWndMovie_Static_RecTxtCtrl, FALSE);

	}
	if (UIStorageCheck(STORAGE_CHECK_ERROR, NULL) == TRUE)
	{
		UxCtrl_SetShow(&UIFlowWndMovie_Static_RecTxtCtrl, FALSE);

	}
	
}

//wifi, audio, etc.
void FlowMovie_IconDrawTopPanel(void)
{
    //wifi, audio, etc.
    FlowMovie_IconDrawTopState();

    UxCtrl_SetShow(&UIFlowWndMovie_Panel_TopNormalCtrl, TRUE);

    //hide Urg Panel
    FlowMovie_IconHideRecURGPanel();
}

void FlowMovie_IconHideTopPanel(void)
{
    UxCtrl_SetShow(&UIFlowWndMovie_Panel_TopNormalCtrl, FALSE);
}

void FlowMovie_IconDrawRecURGPanel(void)
{
    UxCtrl_SetShow(&UIFlowWndMovie_Panel_RecUrgCtrl, TRUE);

    //hide normal panel
    FlowMovie_IconHideTopPanel();
}

void FlowMovie_IconHideRecURGPanel(void)
{
    UxCtrl_SetShow(&UIFlowWndMovie_Panel_RecUrgCtrl, FALSE);
    UxCtrl_SetShow(&UIFlowWndMovie_Panel_EMRBarCtrl, FALSE);
    UxCtrl_SetShow(&UIFlowWndMovie_Static_ENRTimeCtrl, FALSE);
    UxCtrl_SetShow(&UIFlowWndMovie_ConfirmPanelCtrl, FALSE);

}

//屏幕亮度调节
void FlowMovie_IconHideBackLightLvl_Panel(void)
{
 TESTLOG("");
    UxCtrl_SetShow(&UIFlowWndMovie_BackLightLvl_PanelCtrl, FALSE);
	    UxCtrl_SetShow(&UIFlowWndMovie_BackLightLvl_BarCtrl, FALSE);
	   UxCtrl_SetShow(&UIFlowWndMovie_LCDStaticCtrl, FALSE);

}
void FlowMovie_DrawBrightnessBar(BOOL bDraw)
{
 TESTLOG("");
	if( bDraw )
	{	DBGD(SysGetFlag(FL_LCD_BRIGHTNESS));
		UxProgressBar_SetData(&UIFlowWndMovie_BackLightLvl_BarCtrl,PROBAR_CURSTP, SysGetFlag(FL_LCD_BRIGHTNESS));
	   	UxProgressBar_SetData(&UIFlowWndMovie_BackLightLvl_BarCtrl,PROBAR_TOTSTP,99);
	}
    UxCtrl_SetShow(&UIFlowWndMovie_BackLightLvl_PanelCtrl, bDraw);
    UxCtrl_SetShow(&UIFlowWndMovie_BackLightLvl_BarCtrl, bDraw);
	UxCtrl_SetShow(&UIFlowWndMovie_LCDStaticCtrl, bDraw);
}


void FlowMovie_IconHideLCA_ALARM(void)
{
    UxCtrl_SetShow(&UIFlowWndMovie_Status_Alarm_LCtrl, FALSE);
    UxCtrl_SetShow(&UIFlowWndMovie_Status_Alarm_RCtrl, FALSE);
}

void FlowMovie_IconHideBehindProgressBar (void)
{
    UxCtrl_SetShow(&UIFlowWndMovie_Panel_BehindCtrl, FALSE);
    UxCtrl_SetShow(&UIFlowWndMovie_BehindProgressBarCtrl, FALSE);
    UxCtrl_SetShow(&UIFlowWndMovie_BTN_BehindUPCtrl, FALSE);
    UxCtrl_SetShow(&UIFlowWndMovie_BTN_BehindDownCtrl, FALSE);

}

void FlowMovie_UpdateIcons(BOOL bShow)
{
    if (bShow == FALSE || SysGetFlag(FL_LCD_DISPLAY) == DISPOUT_OFF) {
        FlowMovie_IconHideRec(&UIFlowWndMovie_Status_RECCtrl);
        FlowMovie_IconHideDateTime();
		FlowMovie_IconHideBackLightLvl_Panel();
		FlowMovie_IconHideBehindProgressBar();

    } else {
    CHKPNT;
        FlowMovie_IconHideRec(&UIFlowWndMovie_Status_RECCtrl);
        FlowMovie_IconDrawDateTime();
//	FlowMovie_IconHideBackLightLvl_Panel();
        FlowMovie_IconDrawTopPanel();
        FlowMovie_IconHideLCA_ALARM();
    }
}

