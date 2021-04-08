#include <stdio.h>
#include "UIFlow.h"
#include "UISetup.h"

// --------------------------------------------------------------------------
// OPTIONS
// --------------------------------------------------------------------------
// WIFI Options
#if 0// (WIFI_AP_FUNC == ENABLE)
TMDEF_BEGIN_OPTIONS(WIFI)
TMDEF_OPTION_TEXT_S(WIFI_OFF, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(WIFI_ON, TM_OPTION_ENABLE)
TMDEF_END_OPTIONS()
#endif
// MOVIE_AUDIO Options
TMDEF_BEGIN_OPTIONS(MOVIE_AUDIO)
TMDEF_OPTION_TEXT_S(MOVIE_AUDIO_OFF, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(MOVIE_AUDIO_ON, TM_OPTION_ENABLE)
TMDEF_END_OPTIONS()

// MOVIE_CYCLIC_REC Options
TMDEF_BEGIN_OPTIONS(MOVIE_CYCLIC_REC)
//TMDEF_OPTION_TEXT_S(MOVIE_CYCLICREC_OFF, TM_OPTION_DISABLE)
TMDEF_OPTION_TEXT_S(MOVIE_CYCLICREC_1MIN, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(MOVIE_CYCLICREC_3MIN, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(MOVIE_CYCLICREC_5MIN, TM_OPTION_ENABLE)
TMDEF_END_OPTIONS()

// VOLUME Options
TMDEF_BEGIN_OPTIONS(VOLUME)
TMDEF_OPTION_TEXT_S(VOLUME_OFF, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(VOLUME_LOW, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(VOLUME_MED, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(VOLUME_HIGH, TM_OPTION_ENABLE)
TMDEF_END_OPTIONS()

// URGSENSE Options
TMDEF_BEGIN_OPTIONS(URGSENSE)
TMDEF_OPTION_TEXT_S(GSENSOR_OFF, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(GSENSOR_LOW, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(GSENSOR_MED, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(GSENSOR_HIGH, TM_OPTION_ENABLE)
TMDEF_END_OPTIONS()

// PARK Options
TMDEF_BEGIN_OPTIONS(PARK)
TMDEF_OPTION_TEXT_S(MOVIE_PARK_OFF, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(MOVIE_PARK_LOW, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(MOVIE_PARK_MED, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(MOVIE_PARK_HIGH, TM_OPTION_ENABLE)
TMDEF_END_OPTIONS()

#if (AV_LCA_FUNC == ENABLE)
// ALARM Options
TMDEF_BEGIN_OPTIONS(ALARM)
TMDEF_OPTION_TEXT_S(ALARM_LCA_OFF, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(ALARM_LCA_ON, TM_OPTION_ENABLE)
TMDEF_END_OPTIONS()
#endif
		
#if(_ANFA_ADAS_MENU_ == ENABLE)
TMDEF_BEGIN_OPTIONS(ANFA_ADAS)
TMDEF_OPTION_TEXT_S(ANFA_ADAS_OFF, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(ANFA_ADAS_ON, TM_OPTION_ENABLE)
TMDEF_END_OPTIONS()
#endif

TMDEF_BEGIN_OPTIONS(MOVIE_BACKADJUST)
TMDEF_OPTION_TEXT_S(MOVIE_BACKADJUST_OFF, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(MOVIE_BACKADJUST_ON, TM_OPTION_ENABLE)
TMDEF_END_OPTIONS()

// SCREENLIGHT Options
TMDEF_BEGIN_OPTIONS(SCREENLIGHT)
TMDEF_OPTION_TEXT_S(SCREENLIGHT_MANUAL, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(SCREENLIGHT_AUTO, TM_OPTION_ENABLE)
TMDEF_END_OPTIONS()

// MOVIE_SIZE Options
//TMDEF_BEGIN_OPTIONS(MOVIE_SIZE)
//TMDEF_OPTION_TEXT_S(WIFI_OFF, TM_OPTION_ENABLE)
//TMDEF_OPTION_TEXT_S(WIFI_ON, TM_OPTION_ENABLE)
//TMDEF_END_OPTIONS()

// --------------------------------------------------------------------------
// ITEMS
// --------------------------------------------------------------------------
int MenuCustom_APP(UINT32 uiMessage, UINT32 uiParam)
{
    Ux_OpenWindow(&UIMenuWndSetupWiFiInfoCtrl, 0);
    return TMF_PROCESSED;
}
int MenuCustom_WiFiInfo(UINT32 uiMessage, UINT32 uiParam)
{
    Ux_OpenWindow(&UIMenuWndSetupWiFiInfoCtrl, 1,1);
    return TMF_PROCESSED;
}
int MenuCustom_DateTime(UINT32 uiMessage, UINT32 uiParam)
{
    Ux_OpenWindow(&UIMenuWndSetupDateTimeCtrl, 0);
    return TMF_PROCESSED;
}

int MenuCustom_Format(UINT32 uiMessage, UINT32 uiParam)
{
	if (System_GetState(SYS_STATE_CARD)== CARD_REMOVED)
    {
	Ux_OpenWindow(&UIFlowWndWrnMsgCtrl, 2, UIFlowWndWrnMsg_StatusTXT_Msg_STRID_PLEASE_INSERT_SD, FLOWWRNMSG_TIMER_3SEC);
        return NVTEVT_CONSUME;
    }
    Ux_OpenWindow(&UIMenuWndSetupFormatConfirmCtrl, 1, uiParam);
    return TMF_PROCESSED;
}

int MenuCustom_Default(UINT32 uiMessage, UINT32 uiParam)
{
    Ux_OpenWindow(&UIMenuWndSetupDefaultSettingCtrl, 1, uiParam);
    return TMF_PROCESSED;
}

int MenuCustom_Version(UINT32 uiMessage, UINT32 uiParam)
{
    Ux_OpenWindow(&UIMenuWndSetupVersionCtrl, 0);
    return TMF_PROCESSED;
}


// Setup Menu Items
TMDEF_BEGIN_ITEMS(SETUP)
#if 0//(WIFI_AP_FUNC == ENABLE)
TMDEF_ITEM_CUSTOM(APP, MenuCustom_APP)
TMDEF_ITEM_CUSTOM(WIFIINFO, MenuCustom_WiFiInfo)
TMDEF_ITEM_TEXTID(WIFI)
#endif
TMDEF_ITEM_CUSTOM(DATE_TIME, MenuCustom_DateTime)
TMDEF_ITEM_TEXTID(MOVIE_AUDIO)
//TMDEF_ITEM_TEXTID(MOVIE_SIZE)
TMDEF_ITEM_TEXTID(MOVIE_CYCLIC_REC)
TMDEF_ITEM_TEXTID(VOLUME)
TMDEF_ITEM_TEXTID(URGSENSE)
TMDEF_ITEM_TEXTID(PARK)
#if(AV_LCA_FUNC == ENABLE)
TMDEF_ITEM_TEXTID(ALARM)
#endif
#if(_ANFA_ADAS_MENU_)
TMDEF_ITEM_TEXTID(ANFA_ADAS)
#endif
TMDEF_ITEM_TEXTID(MOVIE_BACKADJUST)
//TMDEF_ITEM_TEXTID(SCREENLIGHT)
TMDEF_ITEM_CUSTOM(FORMAT, MenuCustom_Format)
TMDEF_ITEM_CUSTOM(DEFAULT, MenuCustom_Default)
TMDEF_ITEM_CUSTOM(VERSION, MenuCustom_Version)
TMDEF_END_ITEMS()

// --------------------------------------------------------------------------
// PAGES
// --------------------------------------------------------------------------
// Setup Menu Pages
TMDEF_BEGIN_PAGES(SETUP)
TMDEF_PAGE_TEXT_ICON(SETUP)
TMDEF_END_PAGES()

TMDEF_EMNU(gSetupMenu, SETUP, Setup_MenuCallback)

// --------------------------------------------------------------------------
// Menu Callback
// --------------------------------------------------------------------------
int Setup_MenuCallback(UINT32 uiMessage, UINT32 uiParam)
{
    UINT16  uwItemId;
    UINT16  uwOption;

    if (uiMessage == TMM_CONFIRM_OPTION) {
        uwItemId = LO_WORD(uiParam);
        uwOption = HI_WORD(uiParam);

        switch (uwItemId) {
            case IDM_VOLUME:
                Ux_SendEvent(&UISetupObjCtrl, NVTEVT_EXE_BEEPVOLUME, 1, uwOption);
            break;
            case IDM_SCREENLIGHT:
                Ux_SendEvent(&UISetupObjCtrl, NVTEVT_EXE_SCREENLIGHT, 1, uwOption);
            break;
            case IDM_URGSENSE:
                Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_GSENSOR, 1, uwOption);
            break;
            case IDM_PARK:
                Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_MOVIE_PARK, 1, uwOption);
            break;
			case IDM_MOVIE_BACKADJUST:
				UI_SetData(FL_MOVIE_BACKADJUST_MENU, uwOption);
			    Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_MOVIE_BACKADJUST, 1, uwOption);
			break;
            case IDM_MOVIE_AUDIO:
                Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_MOVIE_AUDIO, 1, uwOption);
            break;
            case IDM_MOVIE_SIZE:
                //Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_MOVIESIZE, 1, uwOption);
            break;
            case IDM_MOVIE_CYCLIC_REC:
				 UI_SetData(FL_MOVIE_CYCLIC_REC_MENU, uwOption);
				debug_msg("--cj be uwOption:%d \r\n", uwOption);
				//uwOption = uwOption+1;
                //Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_CYCLIC_REC, 1, uwOption);//20200819 zjf	cancel	
				debug_msg("--cj af uwOption:%d \r\n", uwOption);
            break;
            case IDM_ALARM:
                Ux_SendEvent(&UISetupObjCtrl, NVTEVT_EXE_ALARM_LCA, 1, uwOption);
            break;
			#if(_ANFA_ADAS_MENU_)
			case IDM_ANFA_ADAS:
				Ux_SendEvent(&UISetupObjCtrl, NVTEVT_EXE_ANFA_ADAS, 1, uwOption);
			break;
			#endif
		case IDM_SETLCDBRIGHTLVL:
			 Ux_SendEvent(&UISetupObjCtrl, NVTEVT_EXE_SETLCDBRIGHTLVL, 1, uwOption);

			break;
            case IDM_WIFI:
				debug_msg(" uwOption:%d \r\n",uwOption);
                if(uwOption == WIFI_ON)
                {
                	UI_SetData(FL_WIFI,WIFI_ON);

                    if (UI_GetData(FL_WIFI_LINK) == WIFI_LINK_NG)
                    {
                    CHKPNT;
                        if(CheckWiFiMapTbl()!=E_OK)
                        {
                            UI_SetData(FL_WIFI_LINK,WIFI_LINK_NG);
                            return TMF_PROCESSED;
                        }
						 #if(WIFI_UI_FLOW_VER == WIFI_UI_VER_1_0)
                    		Ux_OpenWindow(&UIMenuWndWiFiWaitCtrl, 0);
                		 #endif
                        UI_SetData(FL_WIFI_LINK,WIFI_LINK_LINKING);
							CHKPNT;
                        BKG_PostEvent(NVTEVT_BKW_WIFI_ON);
                    }
                }
                else
                {
                    UI_SetData(FL_WIFI,WIFI_OFF);
                    if(UI_GetData(FL_WIFI_LINK)==WIFI_LINK_OK)
                    {
                        UI_SetData(FL_WIFI_LINK,WIFI_LINK_DISCONNECTING);
                        Ux_SendEvent(0,NVTEVT_EXE_WIFI_STOP,0);
                    }
                }
                //#NT#2016/03/23#Isiah Chang -end
            break;
			default:
                debug_err(("err id %d\r\n", uwItemId));
            break;
        }
    }

    return TMF_PROCESSED;
}
