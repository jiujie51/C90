//This source code is generated by UI Designer Studio.
#include "UIFramework.h"
#include "UIFrameworkExt.h"
#include "NVTToolCommand.h"
#include "UIMenuWndWiFiModuleLinkRes.c"
#include "UIFlow.h"
#include "UIAppNetwork.h"

//---------------------UIMenuWndWiFiModuleLinkCtrl Debug Definition -----------------------------
#define _UIMENUWNDWIFIMODULELINK_ERROR_MSG        1
#define _UIMENUWNDWIFIMODULELINK_TRACE_MSG        0

#if (_UIMENUWNDWIFIMODULELINK_ERROR_MSG&(PRJ_DBG_LVL>=PRJ_DBG_LVL_ERR))
#define UIMenuWndWiFiModuleLinkErrMsg(...)            debug_msg ("^R UIMenuWndWiFiModuleLink: "__VA_ARGS__)
#else
#define UIMenuWndWiFiModuleLinkErrMsg(...)
#endif

#if (_UIMENUWNDWIFIMODULELINK_TRACE_MSG&(PRJ_DBG_LVL>=PRJ_DBG_LVL_TRC))
#define UIMenuWndWiFiModuleLinkTraceMsg(...)          debug_msg ("^B UIMenuWndWiFiModuleLink: "__VA_ARGS__)
#else
#define UIMenuWndWiFiModuleLinkTraceMsg(...)
#endif

//---------------------UIMenuWndWiFiModuleLinkCtrl Global Variables -----------------------------
//---------------------UIMenuWndWiFiModuleLinkCtrl Prototype Declaration  -----------------------

//---------------------UIMenuWndWiFiModuleLinkCtrl Public API  ----------------------------------

//---------------------UIMenuWndWiFiModuleLinkCtrl Private API  ---------------------------------
void UIMenuWndWiFiModuleLink_UpdateData(void);

//---------------------UIMenuWndWiFiModuleLinkCtrl Control List---------------------------
CTRL_LIST_BEGIN(UIMenuWndWiFiModuleLink)
CTRL_LIST_ITEM(UIMenuWndWiFiModuleLink_StaticICN_WiFi)
CTRL_LIST_ITEM(UIMenuWndWiFiModuleLink_StatusTXT_WiFiMode)
CTRL_LIST_ITEM(UIMenuWndWiFiModuleLink_StaticTXT_SSID)
CTRL_LIST_ITEM(UIMenuWndWiFiModuleLink_StaticTXT_Key)
CTRL_LIST_ITEM(UIMenuWndWiFiModeLink_Tab_RefreshAndWiFiOff)
CTRL_LIST_ITEM(Button_Refresh)
CTRL_LIST_ITEM(Button_WiFiOff)
CTRL_LIST_END

//----------------------UIMenuWndWiFiModuleLinkCtrl Event---------------------------
INT32 UIMenuWndWiFiModuleLink_OnOpen(VControl *, UINT32, UINT32 *);
INT32 UIMenuWndWiFiModuleLink_OnClose(VControl *, UINT32, UINT32 *);
INT32 UIMenuWndWiFiModuleLink_OnChildClose(VControl *, UINT32, UINT32 *);
INT32 UIMenuWndWiFiModuleLink_OnBattery(VControl *, UINT32, UINT32 *);
INT32 UIMenuWndWiFiModuleLink_OnTimer(VControl *, UINT32, UINT32 *);
INT32 UIMenuWndWiFiModuleLink_OnMovieFull(VControl *, UINT32, UINT32 *);
INT32 UIMenuWndWiFiModuleLink_OnMovieWrError(VControl *, UINT32, UINT32 *);
INT32 UIMenuWndWiFiModuleLink_OnStorageSlow(VControl *, UINT32, UINT32 *);
INT32 UIMenuWndWiFiModuleLink_OnWiFiState(VControl *, UINT32, UINT32 *);
INT32 UIMenuWndWiFiModuleLink_OnStorageInit(VControl *, UINT32, UINT32 *);
INT32 UIMenuWndWiFiModuleLink_OnStorageChange(VControl *, UINT32, UINT32 *);

EVENT_BEGIN(UIMenuWndWiFiModuleLink)
EVENT_ITEM(NVTEVT_OPEN_WINDOW, UIMenuWndWiFiModuleLink_OnOpen)
EVENT_ITEM(NVTEVT_CLOSE_WINDOW, UIMenuWndWiFiModuleLink_OnClose)
EVENT_ITEM(NVTEVT_CHILD_CLOSE, UIMenuWndWiFiModuleLink_OnChildClose)
EVENT_ITEM(NVTEVT_BATTERY, UIMenuWndWiFiModuleLink_OnBattery)
EVENT_ITEM(NVTEVT_TIMER, UIMenuWndWiFiModuleLink_OnTimer)
EVENT_ITEM(NVTEVT_CB_MOVIE_OVERTIME, UIMenuWndWiFiModuleLink_OnMovieFull) // the same handling as storage full (may need to show special message)
EVENT_ITEM(NVTEVT_CB_MOVIE_FULL, UIMenuWndWiFiModuleLink_OnMovieFull)
EVENT_ITEM(NVTEVT_CB_MOVIE_LOOPREC_FULL, UIMenuWndWiFiModuleLink_OnMovieFull)
EVENT_ITEM(NVTEVT_CB_MOVIE_WR_ERROR, UIMenuWndWiFiModuleLink_OnMovieWrError)
EVENT_ITEM(NVTEVT_CB_MOVIE_SLOW, UIMenuWndWiFiModuleLink_OnStorageSlow)
EVENT_ITEM(NVTEVT_WIFI_STATE, UIMenuWndWiFiModuleLink_OnWiFiState)
EVENT_ITEM(NVTEVT_STORAGE_INIT, UIMenuWndWiFiModuleLink_OnStorageInit)
EVENT_ITEM(NVTEVT_STORAGE_CHANGE, UIMenuWndWiFiModuleLink_OnStorageChange)
EVENT_END


void UIMenuWndWiFiModuleLink_UpdateData(void)
{
	static char buf1[32], buf2[32];
	char *pMacAddr;
	UIMenuStoreInfo *ptMenuStoreInfo = UI_GetMenuInfo();

	if (UI_GetData(FL_WIFI_LINK) == WIFI_LINK_OK) {
		if (UI_GetData(FL_NetWorkMode) == NET_AP_MODE || UI_GetData(FL_NetWorkMode) == NET_WPS_AP_PBC_MODE || UI_GetData(FL_NetWorkMode) == NET_STATION_MODE) {
			if (ptMenuStoreInfo->strSSID[0] == 0) {
#if (MAC_APPEN_SSID==ENABLE)
				pMacAddr = (char *)UINet_GetMAC();
				snprintf(buf1, sizeof(buf1), "SSID:%s%02x%02x", UINet_GetSSID(),  pMacAddr[4], pMacAddr[5]);// pMacAddr[0], pMacAddr[1], pMacAddr[2], pMacAddr[3],% 02x%02x%02x%02x
#else
				snprintf(buf1, sizeof(buf1), "SSID:%s", UINet_GetSSID());
#endif
			} else {
				snprintf(buf1, sizeof(buf1), "SSID:%s", UINet_GetSSID());
			}


			snprintf(buf2, sizeof(buf2), "PWA2:%s", UINet_GetPASSPHRASE());
			UxStatic_SetData(&UIMenuWndWiFiModuleLink_StaticTXT_SSIDCtrl, STATIC_VALUE, Txt_Pointer(buf1));
			UxStatic_SetData(&UIMenuWndWiFiModuleLink_StaticTXT_KeyCtrl, STATIC_VALUE, Txt_Pointer(buf2));
		}
	} else {
		snprintf(buf1, sizeof(buf1), "%s:Fail", "Conntecting");
		UxStatic_SetData(&UIMenuWndWiFiModuleLink_StaticTXT_SSIDCtrl, STATIC_VALUE, Txt_Pointer(buf1));
		UxStatic_SetData(&UIMenuWndWiFiModuleLink_StaticTXT_KeyCtrl, STATIC_VALUE, STRID_NULL);
	}
}
INT32 UIMenuWndWiFiModuleLink_OnOpen(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
CHKPNT;
    //after enter link wnd ,set wifi sub mode,avoid wifi on change normal movie mode
	System_ChangeSubMode(SYS_SUBMODE_WIFI);
	UIMenuWndWiFiModuleLink_UpdateData();
	CHKPNT;
	UxTab_SetData(&UIMenuWndWiFiModeLink_Tab_RefreshAndWiFiOffCtrl, TAB_FOCUS, 0);
	Ux_DefaultEvent(pCtrl, NVTEVT_OPEN_WINDOW, paramNum, paramArray);
	CHKPNT;
#if defined(_NVT_ETHERNET_EQOS_) || defined(_CPU2_LINUX_)
CHKPNT;
	//#NT#2016/03/17#YongChang Qui -begin
	//#NT#This flow was for testing purpose and should be removed once cardv linux is added
#if (NETWORK_4G_MODULE == ENABLE)
	Ux_PostEvent(NVTEVT_WIFI_AUTHORIZED_OK, 1, 0);
#endif
	//#NT#2016/03/17#YongChang Qui -end
#endif
	return NVTEVT_CONSUME;
}
INT32 UIMenuWndWiFiModuleLink_OnClose(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	Ux_DefaultEvent(pCtrl, NVTEVT_CLOSE_WINDOW, paramNum, paramArray);
	return NVTEVT_CONSUME;
}
INT32 UIMenuWndWiFiModuleLink_OnChildClose(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	UIMenuWndWiFiModuleLink_UpdateData();
	Ux_DefaultEvent(pCtrl, NVTEVT_CHILD_CLOSE, paramNum, paramArray);
	return NVTEVT_CONSUME;
}
INT32 UIMenuWndWiFiModuleLink_OnBattery(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	return NVTEVT_CONSUME;
}
INT32 UIMenuWndWiFiModuleLink_OnTimer(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	Ux_DefaultEvent(pCtrl, NVTEVT_TIMER, paramNum, paramArray);
	return NVTEVT_CONSUME;
}

INT32 UIMenuWndWiFiModuleLink_OnWiFiState(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	if (paramNum == 2) {
		if (UI_GetData(FL_NetWorkMode) == NET_AP_MODE || UI_GetData(FL_NetWorkMode) == NET_WPS_AP_PBC_MODE) {
			UIMenuWndWiFiModuleLinkTraceMsg("ap  state %d result %d\r\n", paramArray[0], paramArray[1]);
			UINT32 state = paramArray[0];
			INT32 result = paramArray[1];
			// wifi phase status
			if (state == NET_STATE_WIFI_CB) {
				if (result == NVT_WIFI_AP_READY) {
					UIMenuWndWiFiModuleLinkTraceMsg("ap mode OK\r\n");
				} else { //if(result == NVT_WIFI_STA_STATUS_PORT_AUTHORIZED)
					UIMenuWndWiFiModuleLinkTraceMsg("ap state %d result %d\r\n", paramArray[0], paramArray[1]);
				}

			}
		} else { // station mode
			UIMenuWndWiFiModuleLinkTraceMsg("sta  state %d result %d\r\n", paramArray[0], paramArray[1]);
			UINT32 state = paramArray[0];
			INT32 result = paramArray[1];

			if (state == NET_STATE_DHCP_START) {
				if (result == 0) {
					UIMenuWndWiFiModuleLinkTraceMsg("station mode OK\r\n");
				} else {
					UIMenuWndWiFiModuleLinkTraceMsg("DHCP fail or wifi module failed\r\n");
				}
			} else if (state == NET_STATE_WIFI_CB) {
				if (result == NVT_WIFI_LINK_STATUS_CONNECTED) {

				} else {

				}
			} else if (state == NET_STATE_WIFI_CONNECTING) {
				if (result == NET_CONNECTING_TIMEOUT) {
					UIMenuWndWiFiModuleLinkTraceMsg("wifi module failed\r\n");
				}
			}
		}
	} else {
		UIMenuWndWiFiModuleLinkErrMsg("err paramNum %d\r\n", paramNum);
	}
	return NVTEVT_CONSUME;
}

INT32 UIMenuWndWiFiModuleLink_OnStorageInit(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
#if (SDHOTPLUG_FUNCTION == ENABLE)
	if (UIStorageCheck(STORAGE_CHECK_ERROR, NULL) == TRUE) {
		UIMenuWndWiFiModuleLinkErrMsg("card err,removed\r\n");
	} else {
		UIMenuWndWiFiModuleLinkErrMsg("card insert\r\n");
	}
#endif
	return NVTEVT_CONSUME;
}
INT32 UIMenuWndWiFiModuleLink_OnStorageChange(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
#if (SDHOTPLUG_FUNCTION == ENABLE)
	UIMenuWndWiFiModuleLinkErrMsg("card remove\r\n");
#endif
	return NVTEVT_CONSUME;
}


//----------------------UIMenuWndWiFiModuleLink_StaticICN_WiFiCtrl Event---------------------------
EVENT_BEGIN(UIMenuWndWiFiModuleLink_StaticICN_WiFi)
EVENT_END

//----------------------UIMenuWndWiFiModuleLink_StatusTXT_WiFiModeCtrl Event---------------------------
EVENT_BEGIN(UIMenuWndWiFiModuleLink_StatusTXT_WiFiMode)
EVENT_END

//----------------------UIMenuWndWiFiModuleLink_StaticTXT_SSIDCtrl Event---------------------------
EVENT_BEGIN(UIMenuWndWiFiModuleLink_StaticTXT_SSID)
EVENT_END

//----------------------UIMenuWndWiFiModuleLink_StaticTXT_KeyCtrl Event---------------------------
EVENT_BEGIN(UIMenuWndWiFiModuleLink_StaticTXT_Key)
EVENT_END

//----------------------UIMenuWndWiFiModeLink_Tab_RefreshAndWiFiOffCtrl Event---------------------------
INT32 UIMenuWndWiFiModeLink_Tab_RefreshAndWiFiOff_OnKeyLeft(VControl *, UINT32, UINT32 *);
INT32 UIMenuWndWiFiModeLink_Tab_RefreshAndWiFiOff_OnKeyRight(VControl *, UINT32, UINT32 *);
//INT32 UIMenuWndWiFiModeLink_Tab_RefreshAndWiFiOff_OnKeyEnter(VControl *, UINT32, UINT32 *);
INT32 UIMenuWndWiFiModeLink_Tab_Authorized_OK(VControl *, UINT32, UINT32 *);
EVENT_BEGIN(UIMenuWndWiFiModeLink_Tab_RefreshAndWiFiOff)
EVENT_ITEM(NVTEVT_KEY_NEXT, UIMenuWndWiFiModeLink_Tab_RefreshAndWiFiOff_OnKeyRight)
//EVENT_ITEM(NVTEVT_KEY_SELECT, UIMenuWndWiFiModeLink_Tab_RefreshAndWiFiOff_OnKeyEnter)
EVENT_ITEM( NVTEVT_WIFI_AUTHORIZED_OK, UIMenuWndWiFiModeLink_Tab_Authorized_OK)
EVENT_END


INT32 UIMenuWndWiFiModeLink_Tab_RefreshAndWiFiOff_OnKeyRight(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	UINT32  uiKeyAct;

	uiKeyAct = paramArray[0];
	switch (uiKeyAct) {
	case NVTEVT_KEY_PRESS:
		Ux_SendEvent(pCtrl, NVTEVT_NEXT_ITEM, 0);
		break;
	}

	return NVTEVT_CONSUME;
}
INT32 UIMenuWndWiFiModeLink_Tab_RefreshAndWiFiOff_OnKeyEnter(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
CHKPNT;
	//UINT32  uiKeyAct;
#if 0
	uiKeyAct = paramArray[0];
	switch (uiKeyAct) {
	case NVTEVT_KEY_PRESS:
		switch (UxTab_GetData(&UIMenuWndWiFiModeLink_Tab_RefreshAndWiFiOffCtrl, TAB_FOCUS)) {
		case UIMenuWndWiFiModeLink_Tab_RefreshAndWiFiOff_Button_Refresh:
			break;
		case UIMenuWndWiFiModeLink_Tab_RefreshAndWiFiOff_Button_WiFiOff:
			if (UI_GetData(FL_WIFI_LINK) == WIFI_LINK_OK) {
			//#NT#2016/03/08#Niven Cho -begin
			//#NT#because it is long time closing the linux-wifi, we don't change mode to movie video ahead.
			//#NT#use Ux_SendEvent(0,NVTEVT_EXE_WIFI_STOP, 0),not post event
#if (WIFI_FUNC == ENABLE)
				INT32 curMode = System_GetState(SYS_STATE_CURRMODE);
				if ( curMode!= PRIMARY_MODE_MOVIE) {
			    	Ux_SendEvent(0,NVTEVT_EXE_WIFI_STOP, 0);
					Ux_CloseWindow(&UIFlowWndMovieCtrl,0);
			    	Ux_PostEvent(NVTEVT_SYSTEM_MODE, 2, curMode,SYS_SUBMODE_NORMAL);
				} else {
					VControl *pWnd =0 ;
					Ux_GetWindowByIndex(&pWnd, 1);

					Ux_SendEvent(0,NVTEVT_EXE_WIFI_STOP, 0);
					System_ChangeSubMode(SYS_SUBMODE_NORMAL);
					Ux_CloseWindow(pWnd, 0);
				}
#endif
			//#NT#2016/03/08#Niven Cho -end
				//#NT#2016/03/08#Niven Cho -end
			} else {
				Ux_CloseWindow(&UIMenuWndWiFiModuleLinkCtrl, 0);
			}
			break;
		}
		break;
	}
#endif 
	return NVTEVT_CONSUME;
}

INT32 UIMenuWndWiFiModeLink_Tab_Authorized_OK(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
CHKPNT;  
	Ux_OpenWindow(&UIMenuWndWiFiMobileLinkOKCtrl, 0);
  
	return NVTEVT_CONSUME;
}

//----------------------Button_RefreshCtrl Event---------------------------
INT32 Button_Refresh_OnTouchPanelClick(VControl *, UINT32, UINT32 *);
INT32 Button_WiFiOff_OnTouchPanelClick(VControl *, UINT32, UINT32 *);

EVENT_BEGIN(Button_Refresh)
EVENT_ITEM(NVTEVT_CLICK, Button_Refresh_OnTouchPanelClick)
EVENT_END

//----------------------Button_WiFiOffCtrl Event---------------------------
EVENT_BEGIN(Button_WiFiOff)
EVENT_ITEM(NVTEVT_CLICK, Button_WiFiOff_OnTouchPanelClick)

EVENT_END

INT32 Button_Refresh_OnTouchPanelClick(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
CHKPNT;       
	UINT32  uiKeyAct;

	uiKeyAct = paramArray[0];
	
	return NVTEVT_CONSUME;
}

INT32 Button_WiFiOff_OnTouchPanelClick(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
CHKPNT;
	UINT32  uiKeyAct;

	uiKeyAct = paramArray[0];
	
		
			if (UI_GetData(FL_WIFI_LINK) == WIFI_LINK_OK) {
			//#NT#2016/03/08#Niven Cho -begin
			//#NT#because it is long time closing the linux-wifi, we don't change mode to movie video ahead.
			//#NT#use Ux_SendEvent(0,NVTEVT_EXE_WIFI_STOP, 0),not post event
#if (WIFI_FUNC == ENABLE)
				INT32 curMode = System_GetState(SYS_STATE_CURRMODE);
				if ( curMode!= PRIMARY_MODE_MOVIE) {
			    	Ux_SendEvent(0,NVTEVT_EXE_WIFI_STOP, 0);
					Ux_CloseWindow(&UIFlowWndMovieCtrl,0);
			    	Ux_PostEvent(NVTEVT_SYSTEM_MODE, 2, curMode,SYS_SUBMODE_NORMAL);
				} else {
					VControl *pWnd =0 ;
					Ux_GetWindowByIndex(&pWnd, 1);

					Ux_SendEvent(0,NVTEVT_EXE_WIFI_STOP, 0);
					System_ChangeSubMode(SYS_SUBMODE_NORMAL);
					Ux_CloseWindow(pWnd, 0);
				}
#endif
			//#NT#2016/03/08#Niven Cho -end
				//#NT#2016/03/08#Niven Cho -end
			} else {
				Ux_CloseWindow(&UIMenuWndWiFiModuleLinkCtrl, 0);
			}

	return NVTEVT_CONSUME;
}
INT32 UIMenuWndWiFiModuleLink_OnMovieFull(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	return WiFiCmd_OnMovieFull(pCtrl, paramNum, paramArray);
}
INT32 UIMenuWndWiFiModuleLink_OnMovieWrError(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	return WiFiCmd_OnMovieWrError(pCtrl, paramNum, paramArray);
}
INT32 UIMenuWndWiFiModuleLink_OnStorageSlow(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	return WiFiCmd_OnStorageSlow(pCtrl, paramNum, paramArray);
}

