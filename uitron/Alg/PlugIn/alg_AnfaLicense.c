
#include "Type.h"
#include "stdio.h"

#include "UISetup.h"
#include "UIFlow.h"
#include "UIInfo.h"
#include "UIAppNetwork.h"
#include "PStore.h"

//#include <cJSON.h>
#include "CurlNvtAPI.h"


#include "DhcpNvt.h"
#include "Anfa_License.h"
#include "MsdcNvtCb.h"
#include "UIAppUsbDisk.h"
#include "UsbDevDef.h"
#include "usb.h"


#define PRI_ANFAALG_AUTHPROC             12

#define STKSIZE_ANFAALG_AUTHPROC       1024*4

UINT32 FLG_ID_ANFAALG_AUTH = 0;
UINT32 ANFAALG_AUTHPROC_TSK_ID = 0;

#define ANFAALG_AUTH_FLAG_DATAREADY      1<<1
#define AUTH_FLAG_NETREADY       1<<2
#define AUTH_FLAG_MACREADY       1<<3
#define AUTH_FLAG_ALL            0xFFFFFFFF


extern void Anfa_GetLicensePro(void);

extern void Anfa_Register_GetMacAddr_CB(int(*GetMacFun)(char *mac));
extern void Anfa_Register_WifiConnect_CB(void(*SetWifiConnect)(CHAR *ssid,CHAR *passwd));
extern void Anfa_Register_USBConnect_CB(INT32(*SetUSBConnect)(void));


#if 0//(_ALPHAVISION_LICENSE_)
char *cJsonMemPool = NULL;
void App_cJSON_Init(void)
{
    if (cJsonMemPool == NULL) {
        
        cJsonMemPool = (char *)OS_GetMempoolAddr(POOL_ID_CJSON); 
        if (cJsonMemPool == NULL) {
            debug_msg("Get cJsonMemPool failed\n");
            return;
        }
        cJSON_Init(cJsonMemPool, POOL_SIZE_CJSON);
    } else {
        debug_msg("cJsonMemPool has been alloced");
    }


}
#endif


INT32 AnfaAlgAuthProc_Init(void)
{
	char *name="user01";
	AVLic_SetUserID(name);
	AV_SetUIDFormat(1);//0==mac,1==chipid
	sta_tsk(ANFAALG_AUTHPROC_TSK_ID, 0);

	return 0;
}

INT32 AnfaAlgAuthProc_UnInit(void)
{

	ter_tsk(ANFAALG_AUTHPROC_TSK_ID);
	return 0;
}

void AnfaAlgAuth_NetReady(void)
{
    set_flg(FLG_ID_ANFAALG_AUTH, AUTH_FLAG_NETREADY);
}


void AnfaAlgAuth_MacAddrReady(void)
{
    set_flg(FLG_ID_ANFAALG_AUTH, AUTH_FLAG_MACREADY);

}

static int get_wifi_mac(char *hwmacaddr)
{

	char *tmpMAC = UINet_GetMAC();

    snprintf(hwmacaddr, 13, "%02X%02X%02X%02X%02X%02X",
        tmpMAC[0], tmpMAC[1], tmpMAC[2], tmpMAC[3], tmpMAC[4], tmpMAC[5]);
    debug_msg("^B Read WiFi MAC: %s\r\n", hwmacaddr);


	return 0;


}


VOID start_wifi_station_connect(CHAR *ssid,CHAR *passwd)
{
    UINT32 mode = 1;
    UINT32 security = 1;

    if (UI_GetData(FL_WIFI_LINK)==WIFI_LINK_OK)
	{
        Ux_SendEvent(0,NVTEVT_EXE_WIFI_STOP,0);
    }

	debug_msg("anfa_wifi_sta_connect=%d\r\n",UI_GetData(FL_WIFI_LINK));


   UI_SetData(FL_NetWorkMode, mode);
   #if 0
    UINet_SetAuthType(security);
    if (mode == NET_STATION_MODE) {
        UINet_SetSSID(ssid, strlen(ssid));
        UINet_SetPASSPHRASE(passwd, strlen(passwd));
        if (security == 0) {
            UINet_SetAuthType(NET_AUTH_NONE);
        } else {
            UINet_SetAuthType(NET_AUTH_WPA2);
        }
    }
    BKG_PostEvent(NVTEVT_BKW_WIFI_ON);
	#endif
    
}

static INT32 set_usb_connect(VOID)
{
	#if (MSDCVENDOR_NVT==ENABLE)
	if(GxUSB_GetConnectType() == USB_CONNECT_PC)
	{

		MSDCNVTCB_OPEN Open = {0};
		//int_setConfig(INT_CONFIG_ID_USB3_GIC_DESTINATION, CC_CORE_CA53_CORE1);
		Open.fpUSBMakerInit = USBMakerInit_UMSD;
		MsdcNvtCb_Open(&Open);
		return 1;
	}

	return 0;

	#else
	return 0;
	#endif
    
}



void AnfaAlgAuth_VoicePrompts(UINT32 index)
{
//example

	/*if(index==0)
	{
		UIVoice_Play(VOICE_WARNING_TONE);

	}
	else
	{
		UIVoice_Play(BEEP_DANGER);
	}*/

	
	if(index==0)//success
	{
		Ux_OpenWindow(&UIFlowWndWrnMsgCtrl, 2, UIFlowWndWrnMsg_StatusTXT_Msg_STRID_PLEASE_INSERT_SD, FLOWWRNMSG_TIMER_2SEC);

	}
	else if(index==1)//Authorized
	{

		Ux_OpenWindow(&UIFlowWndWrnMsgCtrl, 2, UIFlowWndWrnMsg_StatusTXT_Msg_STRID_CARD_FULL, FLOWWRNMSG_TIMER_2SEC);
	}
	else//fail
	{
		Ux_OpenWindow(&UIFlowWndWrnMsgCtrl, 2, UIFlowWndWrnMsg_StatusTXT_Msg_STRID_NO_IMAGE, FLOWWRNMSG_TIMER_2SEC);
	}
	

}



void AnfaAlgAuthProc_InstallID(void)
{

	OS_CONFIG_TASK(ANFAALG_AUTHPROC_TSK_ID, PRI_ANFAALG_AUTHPROC, STKSIZE_ANFAALG_AUTHPROC, Anfa_GetLicensePro);
	OS_CONFIG_FLAG(FLG_ID_ANFAALG_AUTH);

	Anfa_Register_GetMacAddr_CB(get_wifi_mac);

	Anfa_Register_WifiConnect_CB(start_wifi_station_connect);

	Anfa_Register_USBConnect_CB(set_usb_connect);

	AV_SelectSNSaveArea(USE_PSTORE);

	#ifdef USER_DX_TYPE_EMBMEM_USER0
	AV_SetUserEMBMEMType(USER_DX_TYPE_EMBMEM_USER0);
	#endif
}



