//global debug level: PRJ_DBG_LVL
#include "PrjCfg.h"

#include "SysCommon.h"
#include "AppLib.h"

#include "UIFramework.h"
#include "NVTUserCommand.h"
#include "NvtUser.h"

#if (UI_FUNC == ENABLE)
#include "UIAppMovie.h"  // Isiah, implement YUV merge mode of recording func.
#include "UIAppMovie_Param.h"
#include "UIMovieMapping.h"
#if(WIFI_AP_FUNC==ENABLE)
#include "UIFrameworkExt.h"
#include "UIMenuWndWiFiWait.h"
#endif
#if(WIFI_FUNC==ENABLE)
#include "UIModeWifi.h"
#endif

#endif

//local debug level: THIS_DBGLVL
#define THIS_DBGLVL         2 // 0=FATAL, 1=ERR, 2=WRN, 3=UNIT, 4=FUNC, 5=IND, 6=MSG, 7=VALUE, 8=USER
///////////////////////////////////////////////////////////////////////////////
#define __MODULE__          SysInCmd
#define __DBGLVL__          ((THIS_DBGLVL>=PRJ_DBG_LVL)?THIS_DBGLVL:PRJ_DBG_LVL)
#define __DBGFLT__          "*" //*=All, [mark]=CustomClass
#include "DebugModule.h"
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// key input
///////////////////////////////////////////////////////////////////////////////

//#Fix warning message
//UINT32  uiKeyTmpMsk;

static BOOL Cmd_key_power(CHAR *strCmd)
{
	//if(uiKeyTmpMsk & FLGkey_XXX)
	{
		DBG_DUMP("Power Key\r\n");
		//Ux_PostEvent(NVTEVT_KEY_POWER, 1, NVTEVT_KEY_PRESS);
		Ux_PostEvent(NVTEVT_SYSTEM_SHUTDOWN, 1, 0);
		return TRUE;
	}
}
static BOOL Cmd_key_delete(CHAR *strCmd)
{
	//if(uiKeyTmpMsk & FLGkey_DEL)
	{
		DBG_DUMP("Del Key\r\n");
		Ux_PostEvent(NVTEVT_KEY_DEL, 1, NVTEVT_KEY_PRESS);
		return TRUE;
	}
}
BOOL Cmd_key_menu(CHAR *strCmd)
{
	//if(uiKeyTmpMsk & FLGkey_MENU)
	{
		DBG_DUMP("Menu Key\r\n");
		Ux_PostEvent(NVTEVT_KEY_MENU, 1, NVTEVT_KEY_PRESS);
		return TRUE;
	}
}
static BOOL Cmd_key_mode(CHAR *strCmd)
{
	//if(uiKeyTmpMsk & FLGkey_MODE)
	{
		DBG_DUMP("Mode Key\r\n");
		Ux_PostEvent(NVTEVT_KEY_MODE, 1, NVTEVT_KEY_PRESS);
		return TRUE;
	}
}
static BOOL Cmd_key_playback(CHAR *strCmd)
{
	//if(uiKeyTmpMsk & FLGkey_PLAYBACK)
	{
		DBG_DUMP("Playback Key\r\n");
		Ux_PostEvent(NVTEVT_KEY_PLAYBACK, 1, NVTEVT_KEY_PRESS);
		return TRUE;
	}
}

static BOOL Cmd_key_movie(CHAR *strCmd)
{
	//if(uiKeyTmpMsk & FLGkey_PLAYBACK)
	{
		DBG_DUMP("Movie Key\r\n");
		Ux_PostEvent(NVTEVT_KEY_MOVIE, 1, NVTEVT_KEY_PRESS);
		return TRUE;
	}
}
static BOOL Cmd_key_up(CHAR *strCmd)
{
	//if(uiKeyTmpMsk & FLGkey_UP)
	{
		DBG_DUMP("Up Key\r\n");
		Ux_PostEvent(NVTEVT_KEY_UP, 1, NVTEVT_KEY_PRESS);
		return TRUE;
	}
}
static BOOL Cmd_key_down(CHAR *strCmd)
{
	//if(uiKeyTmpMsk & FLGkey_DOWN)
	{
		DBG_DUMP("Down Key\r\n");
		Ux_PostEvent(NVTEVT_KEY_DOWN, 1, NVTEVT_KEY_PRESS);
		return TRUE;
	}
}
static BOOL Cmd_key_right(CHAR *strCmd)
{
	//if(uiKeyTmpMsk & FLGkey_RIGHT)
	{
		DBG_DUMP("Right Key\r\n");
		Ux_PostEvent(NVTEVT_KEY_RIGHT, 1, NVTEVT_KEY_PRESS);
		return TRUE;
	}
}
static BOOL Cmd_key_left(CHAR *strCmd)
{
	//if(uiKeyTmpMsk & FLGkey_LEFT)
	{
		DBG_DUMP("Left Key\r\n");
		Ux_PostEvent(NVTEVT_KEY_LEFT, 1, NVTEVT_KEY_PRESS);
		return TRUE;
	}
}
static BOOL Cmd_key_enter(CHAR *strCmd)
{
	//if(uiKeyTmpMsk & FLGkey_ENTER)
	{
		DBG_DUMP("Enter Key\r\n");
		Ux_PostEvent(NVTEVT_KEY_ENTER, 1, NVTEVT_KEY_PRESS);
		return TRUE;
	}
}
static BOOL Cmd_key_zoomin(CHAR *strCmd)
{
	//if(uiKeyTmpMsk & FLGkey_ZOOMIN)
	{
		DBG_DUMP("Zoom In Key\r\n");
		Ux_PostEvent(NVTEVT_KEY_ZOOMIN, 1, NVTEVT_KEY_PRESS);
		return TRUE;
	}
}
static BOOL Cmd_key_zoomout(CHAR *strCmd)
{
	//if(uiKeyTmpMsk & FLGkey_ZOOMOUT)
	{
		DBG_DUMP("Zoom Out Key\r\n");
		Ux_PostEvent(NVTEVT_KEY_ZOOMOUT, 1, NVTEVT_KEY_PRESS);
		return TRUE;
	}
}

static BOOL Cmd_key_zoomin_release(CHAR *strCmd)
{
	//if(uiKeyTmpMsk & FLGkey_ZOOMIN)
	{
		DBG_DUMP("Zoom In Key release\r\n");
		Ux_PostEvent(NVTEVT_KEY_ZOOMIN, 1, NVTEVT_KEY_RELEASE);
		return TRUE;
	}
}

static BOOL Cmd_key_zoomout_release(CHAR *strCmd)
{
	//if(uiKeyTmpMsk & FLGkey_ZOOMOUT)
	{
		DBG_DUMP("Zoom Out Key release\r\n");
		Ux_PostEvent(NVTEVT_KEY_ZOOMOUT, 1, NVTEVT_KEY_RELEASE);
		return TRUE;
	}
}

static BOOL Cmd_key_zoomrelease(CHAR *strCmd)
{
	{
		DBG_DUMP("zoom release Key\r\n");
		Ux_PostEvent(NVTEVT_KEY_ZOOMRELEASE, 1, NVTEVT_KEY_ZOOMRELEASE);
		return TRUE;
	}
}

static BOOL Cmd_key_shutter1(CHAR *strCmd)
{
	//if(uiKeyTmpMsk & FLGkey_SHUTTER1)
	{
		DBG_DUMP("Shutter1 Key\r\n");
		Ux_PostEvent(NVTEVT_KEY_SHUTTER1, 1, NVTEVT_KEY_PRESS);
		return TRUE;
	}
}
#if (WIFI_AP_FUNC==ENABLE)
extern void FlowWiFiMovie_Stop2Idle(void);
#endif
extern void FlowPhoto_DoCapture(void);
static BOOL Cmd_key_shutter2(CHAR *strCmd)
{
#if(WIFI_AP_FUNC==ENABLE)
	if (System_GetState(SYS_STATE_CURRSUBMODE) == SYS_SUBMODE_WIFI) {
#if(IPCAM_FUNC!=ENABLE)
		//FlowWiFiMovie_Stop2Idle();
#endif
	}
#endif
	//if(uiKeyTmpMsk & FLGkey_SHUTTER2)
	{
		DBG_DUMP("Shutter2 Key\r\n");
		Ux_PostEvent(NVTEVT_KEY_SHUTTER2, 1, NVTEVT_KEY_PRESS);
		return TRUE;
	}
}

static BOOL Cmd_key_shutter1rel(CHAR *strCmd)
{
	{
		DBG_DUMP("Shutter1 release Key\r\n");
		Ux_PostEvent(NVTEVT_KEY_SHUTTER1, 1, NVTEVT_KEY_RELEASE);
		return TRUE;
	}
}

static BOOL Cmd_key_shutter2rel(CHAR *strCmd)
{
	{
		DBG_DUMP("Shutter2 release Key\r\n");
		Ux_PostEvent(NVTEVT_KEY_SHUTTER2, 1, NVTEVT_KEY_RELEASE);
		return TRUE;
	}
}

static BOOL Cmd_key_custom1(CHAR *strCmd)
{
	{
		DBG_DUMP("Custom1 Key\r\n");
		Ux_PostEvent(NVTEVT_KEY_CUSTOM1, 1, NVTEVT_KEY_RELEASE);
		return TRUE;
	}
}

#if(WIFI_AP_FUNC==ENABLE)
#include "UIMenuWndWiFiWait.h"
static BOOL Cmd_key_i(CHAR *strCmd)
{
	DBG_DUMP("I Key\r\n");

#if(IPCAM_FUNC!=ENABLE)
//#NT#2016/03/23#Isiah Chang -begin
//#NT#add new Wi-Fi UI flow.
#if(WIFI_UI_FLOW_VER == WIFI_UI_VER_2_0)
	if (UI_GetData(FL_WIFI_LINK) == WIFI_LINK_NG) {
		BKG_PostEvent(NVTEVT_BKW_WIFI_ON);
	}
#else
	Ux_OpenWindow(&UIMenuWndWiFiWaitCtrl, 0);
#endif
//#NT#2016/03/23#Isiah Chang -end
#endif
	BKG_PostEvent(NVTEVT_BKW_WIFI_ON);
	return TRUE;

}
#endif
static BOOL Cmd_key_next(CHAR *strCmd)
{
	DBG_DUMP("next Key\r\n");
	Ux_PostEvent(NVTEVT_KEY_NEXT, 1, NVTEVT_KEY_PRESS);
	return TRUE;
}
//#NT#2016/03/16#Niven Cho -begin
//#NT#add key previous
static BOOL Cmd_key_prev(CHAR *strCmd)
{
	DBG_DUMP("prev Key\r\n");
	Ux_PostEvent(NVTEVT_KEY_PREV, 1, NVTEVT_KEY_PRESS);
	return TRUE;
}
//#NT#2016/03/16#Niven Cho -end
static BOOL Cmd_key_select(CHAR *strCmd)
{
	DBG_DUMP("select Key\r\n");
	Ux_PostEvent(NVTEVT_KEY_SELECT, 1, NVTEVT_KEY_PRESS);
	return TRUE;
}
static BOOL Cmd_key_cut(CHAR *strCmd)
{
#if(MOVIE_MODE==ENABLE)
	UINT32 ircut = 0;

	sscanf_s(strCmd, "%d ", &ircut);
	DBG_DUMP("ircut %d\r\n", ircut);
	Ux_PostEvent(NVTEVT_EXE_MOVIE_IR_CUT,         1,  ircut);
#endif
	return TRUE;
}

#if 0//(CURL_FUNC == ENABLE)
static void CurlCmd_dumpmem(UINT32 Addr, UINT32 size)
{
	UINT32 i, j;
	DBG_DUMP("\r\n Addr=0x%x, size =0x%x\r\n", Addr, size);


	for (j = Addr; j < (Addr + size); j += 0x10) {
		DBG_DUMP("^R 0x%8x:", j);
		for (i = 0; i < 16; i++) {
			DBG_DUMP("0x%2x,", *(char *)(j + i));
		}
		DBG_DUMP("\r\n");
	}
	DBG_DUMP("Data %s\r\n", (char *)Addr);

}


static void curl_rw_cb(UINT32 MessageID, UINT32 Status, UINT32 isContine, UINT32 Param3)
{
	UINT32 *bContue = (UINT32 *)isContine;

	if (bContue) {
		// set contine false to stop curl r/w
		//*bContue = FALSE;
	}
}
#endif

static BOOL Cmd_key_curl(CHAR *strCmd)
{
#if 0 //(CURL_FUNC == ENABLE)

#include "NetFs.h"

	static char tmpbuff[0x400000];
	UINT32 size, pos;
	char   ip[60] = "192.168.0.3";
	char   rootdir[10] = "A:\\";
	char   path[60] = "A:\\test.bin";

	//sscanf(strCmd,"%d %d %d %d %d %d %d %d",&g_lviewframeRate,&g_lviewTargetRate,&g_lviewQueueFrame,&g_lviewPort,&g_lviewTaskPri, &g_hwmemcpy, &g_maxJpgSize, &g_reqDelay);
	sscanf(strCmd, "%d %s", &size, path);

	FST_FILE  filehdl;


	NetFs_SetParam(NETFS_PARAM_REMOTE_IP, (UINT32)ip);
	NetFs_SetParam(NETFS_PARAM_ROOT_DIR, (UINT32)rootdir);
	NetFs_SetParam(NETFS_PARAM_USE_SSL, FALSE);

	// test1
	pos = 0;
	filehdl = NetFs_OpenFile(path, FST_OPEN_READ);
	NetFs_SeekFile(filehdl, 0, FST_SEEK_SET);
	size = 0x200000;
	NetFs_ReadFile(filehdl, (UINT8 *)tmpbuff, &size, 0, NULL);
	pos += size;
	NetFs_SeekFile(filehdl, pos, FST_SEEK_SET);
	size = 0x200000;
	NetFs_ReadFile(filehdl, (UINT8 *)tmpbuff + pos, &size, 0, curl_rw_cb);
	pos += size;
	NetFs_CloseFile(filehdl);

	size = 0x200000;
	filehdl = FileSys_OpenFile("A:\\CURL\\test1.bin", FST_OPEN_WRITE | FST_CREATE_ALWAYS);
	FileSys_WriteFile(filehdl, (UINT8 *)tmpbuff, &size, 0, NULL);
	FileSys_CloseFile(filehdl);

	pos = 0;
	filehdl = NetFs_OpenFile("A:\\CURL\\testuplo.bin", FST_OPEN_WRITE | FST_CREATE_ALWAYS);

	NetFs_SetUploadSrcFilePath(filehdl, "A:\\test.bin");
	size = 0x100000;
	NetFs_WriteFile(filehdl, (UINT8 *)tmpbuff + pos, &size, 0, curl_rw_cb);
	pos += size;
	size = 0x000080;
	NetFs_WriteFile(filehdl, (UINT8 *)tmpbuff + pos, &size, 0, NULL);
	pos += size;
	size = 0x000080;
	NetFs_WriteFile(filehdl, (UINT8 *)tmpbuff + pos, &size, 0, NULL);
	pos += size;
	size = 0x0fff00;
	NetFs_WriteFile(filehdl, (UINT8 *)tmpbuff + pos, &size, 0, NULL);
	pos += size;
	NetFs_CloseFile(filehdl);


	/*

	// test2
	pos = 0;
	filehdl = NetFs_OpenFile(path,FST_OPEN_READ);
	NetFs_SeekFile(filehdl,0,FST_SEEK_SET);
	size = 0x10000;
	NetFs_ReadFile(filehdl, (UINT8 *)tmpbuff, &size, 0, NULL);
	pos += size;
	NetFs_SeekFile(filehdl,pos,FST_SEEK_SET);
	size = 0x10000;
	NetFs_ReadFile(filehdl, (UINT8 *)tmpbuff+pos, &size, 0, NULL);
	pos += size;
	NetFs_CloseFile(filehdl);

	size = 0x20000;
	filehdl = FileSys_OpenFile("A:\\CURL\\test2.bin",FST_OPEN_WRITE|FST_CREATE_ALWAYS);
	FileSys_WriteFile(filehdl, (UINT8 *)tmpbuff, &size, 0, NULL);
	FileSys_CloseFile(filehdl);

	// test3
	pos = 0;
	filehdl = NetFs_OpenFile(path,FST_OPEN_READ);
	NetFs_SeekFile(filehdl,0,FST_SEEK_SET);
	size = 0x20000;
	NetFs_ReadFile(filehdl, (UINT8 *)tmpbuff, &size, 0, NULL);
	pos += size;
	NetFs_SeekFile(filehdl,pos,FST_SEEK_SET);
	size = 0x20000;
	NetFs_ReadFile(filehdl, (UINT8 *)tmpbuff+pos, &size, 0, NULL);
	pos += size;
	NetFs_CloseFile(filehdl);

	size = 0x40000;
	filehdl = FileSys_OpenFile("A:\\CURL\\test3.bin",FST_OPEN_WRITE|FST_CREATE_ALWAYS);
	FileSys_WriteFile(filehdl, (UINT8 *)tmpbuff, &size, 0, NULL);
	FileSys_CloseFile(filehdl);

	// test4
	pos = 0;
	filehdl = NetFs_OpenFile(path,FST_OPEN_READ);
	size = 0x50000;
	NetFs_ReadFile(filehdl, (UINT8 *)tmpbuff, &size, 0, NULL);
	pos += size;
	size = 0x50000;
	NetFs_ReadFile(filehdl, (UINT8 *)tmpbuff+pos, &size, 0, NULL);
	pos += size;
	NetFs_CloseFile(filehdl);

	size = 0xA0000;
	filehdl = FileSys_OpenFile("A:\\CURL\\test4.bin",FST_OPEN_WRITE|FST_CREATE_ALWAYS);
	FileSys_WriteFile(filehdl, (UINT8 *)tmpbuff, &size, 0, NULL);
	FileSys_CloseFile(filehdl);
	*/
#endif
	return TRUE;
}
SXCMD_BEGIN(key, "key command")
SXCMD_ITEM("power", Cmd_key_power, "power key")
SXCMD_ITEM("delete", Cmd_key_delete, "delete key")
SXCMD_ITEM("menu", Cmd_key_menu, "menu key")
SXCMD_ITEM("mode", Cmd_key_mode, "mode key")
SXCMD_ITEM("playback", Cmd_key_playback, "playback key")
SXCMD_ITEM("movie", Cmd_key_movie, "movie key")
SXCMD_ITEM("up", Cmd_key_up, "up key")
SXCMD_ITEM("down", Cmd_key_down, "down key")
SXCMD_ITEM("right", Cmd_key_right, "right key")
SXCMD_ITEM("left", Cmd_key_left, "left key")
SXCMD_ITEM("enter", Cmd_key_enter, "enter key")
SXCMD_ITEM("zoomin", Cmd_key_zoomin, "zoomin key pressed")
SXCMD_ITEM("zoomout", Cmd_key_zoomout, "zoomout key pressed")
SXCMD_ITEM("zoominrel", Cmd_key_zoomin_release, "zoomin key released")
SXCMD_ITEM("zoomoutrel", Cmd_key_zoomout_release, "zoomout key released")
SXCMD_ITEM("zoomrel", Cmd_key_zoomrelease, "zoom key released")
SXCMD_ITEM("shutter1", Cmd_key_shutter1, "shutter1 key pressed")
SXCMD_ITEM("shutter2", Cmd_key_shutter2, "suutter2 key pressed")
SXCMD_ITEM("shutter1rel", Cmd_key_shutter1rel, "shutter1 key released")
SXCMD_ITEM("shutter2rel", Cmd_key_shutter2rel, "shutter2 key released")
SXCMD_ITEM("custom1", Cmd_key_custom1, "custom1 key")
#if(WIFI_AP_FUNC==ENABLE)
SXCMD_ITEM("i", Cmd_key_i, "i key pressed")
#endif
SXCMD_ITEM("next", Cmd_key_next, "next key")
SXCMD_ITEM("select", Cmd_key_select, "select key")
SXCMD_ITEM("cut %", Cmd_key_cut, "cut key")
SXCMD_ITEM("curl %", Cmd_key_curl, "curl key")
//#NT#2016/03/16#Niven Cho -begin
//#NT#add key previous
SXCMD_ITEM("prev", Cmd_key_prev, "prev key")
//#NT#2016/03/16#Niven Cho -end
SXCMD_END()

#if (defined(_NVT_ETHREARCAM_RX_))
#include "FileSysTsk.h"
#include "EthCamAppCmd.h"
#include "UIModeUpdFw.h"
#include "UIFlow.h"
#include "EthCamAppSocket.h"
#include "EthCamSocket.h"
#include "EthCamAppNetwork.h"

static BOOL Cmd_ethcam_tx_fwupdate(CHAR *strCmd)
{
	UINT32 uiFwAddr=0, uiFwSize=0;
	FST_FILE hFile;
	static char uiUpdateFWName[64] = "A:\\FW671_AA.bin"; // EthcamTxFW
	INT32 fst_er;
	UINT32 EthCamCmdRET[ETH_REARCAM_CAPS_COUNT]={0};

	UINT32 i;
	for (i=0; i<ETH_REARCAM_CAPS_COUNT; i++){

		if(EthCamNet_GetEthLinkStatus(i)==ETHCAM_LINK_UP){
			EthCam_SendXMLCmd(ETHCAM_PATH_ID_1+i, ETHCAM_PORT_DATA1 ,ETHCAM_CMD_TX_STREAM_STOP, 0);
#if (ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
			EthCam_SendXMLCmd(ETHCAM_PATH_ID_1+i, ETHCAM_PORT_DATA2 ,ETHCAM_CMD_TX_STREAM_STOP, 0);
#endif
			if(System_GetState(SYS_STATE_CURRMODE) !=PRIMARY_MODE_UPDFW){
				System_ChangeSubMode(SYS_SUBMODE_UPDFW);
				Ux_SendEvent(0, NVTEVT_SYSTEM_MODE, 1, PRIMARY_MODE_UPDFW);
			}
			if(uiFwSize==0 && uiFwAddr==0){
				uiFwSize = (UINT32)FileSys_GetFileLen(uiUpdateFWName);
				uiFwAddr = OS_GetMempoolAddr(POOL_ID_APP);

				hFile = FileSys_OpenFile(uiUpdateFWName, FST_OPEN_READ);
				if (hFile != 0) {
					fst_er = FileSys_ReadFile(hFile, (UINT8 *)uiFwAddr, &uiFwSize, 0, NULL);
					FileSys_CloseFile(hFile);
					if (fst_er != FST_STA_OK) {
						DBG_ERR("FW bin read fail\r\n");
						return TRUE;
					}
				}
			}
			DBG_DUMP("Total FwSize=%d\r\n",uiFwSize);
			EthCamSocketCli_SetCmdSendSizeCB(ETHCAM_PATH_ID_1+i, (UINT32)&socketCliEthCmd_SendSizeCB);
			EthCam_SendXMLCmd(ETHCAM_PATH_ID_1+i, ETHCAM_PORT_DEFAULT ,ETHCAM_CMD_TX_FWUPDATE_FWSEND, uiFwSize);
			EthCamCmdRET[i]=EthCam_SendXMLData(ETHCAM_PATH_ID_1+i, (UINT8 *)uiFwAddr, uiFwSize);
			EthCamSocketCli_SetCmdSendSizeCB(ETHCAM_PATH_ID_1+i, 0);

		}
	}

	for (i=0; i<ETH_REARCAM_CAPS_COUNT; i++){
		if(EthCamNet_GetEthLinkStatus(i)==ETHCAM_LINK_UP){
			if(EthCamCmdRET[i]==ETHCAM_RET_OK){
				DBG_DUMP("[%d]Send FW update  Start\r\n", i);
				EthCam_SendXMLCmd(ETHCAM_PATH_ID_1+i, ETHCAM_PORT_DEFAULT ,ETHCAM_CMD_TX_FWUPDATE_START, 0);
			}else{
				DBG_ERR("[%d]FW send error, %d\r\n",i,EthCamCmdRET[i]);
			}
		}
	}
	
	CHKPNT;
	Ux_CloseWindow(&UIFlowWndWaitMomentCtrl, 0);
	CHKPNT;
	Ux_OpenWindow(&UIFlowWndWaitMomentCtrl, 1, UIFlowWndWaitMoment_StatusTXT_Msg_STRID_ETHCAM_UDFW_START);

	return TRUE;
}

static BOOL Cmd_ethcam_set_tx_link_status(CHAR *strCmd)
{
	UINT32 path_id = 0;
	UINT32 link_status = 0;

	sscanf_s(strCmd, "%d %d", &path_id,&link_status);
	DBG_DUMP("path_id=%d, link_status=%d\r\n", path_id,link_status);
	if(link_status==1){
		if(path_id==0){
			EthCamNet_EthLinkStatusNotify("EthRestart 201369792 2");
		}else{
			EthCamNet_EthLinkStatusNotify("EthRestart 218147008 2");
		}
	}else if(link_status==2){
		EthCamNet_EthLinkStatusNotify("EthRestart 201369792 2");
		EthCamNet_EthLinkStatusNotify("EthRestart 218147008 2");
	}else{
		if(path_id==0){
			EthCamNet_EthLinkStatusNotify("EthRestart 201369792 1");
		}else{
			EthCamNet_EthLinkStatusNotify("EthRestart 218147008 1");
		}
	}
	return TRUE;
}
#include "gpio.h"
#include "mdcmdio.h"
static BOOL Cmd_get_eth_hub_status(CHAR *strCmd)
{
#if (defined(MDC_GPIO) && defined(MDIO_GPIO))
	UINT32  read_data;
	UINT32  link_status;
	#define LINK_STATUS_REG_BIT 2
	#define LINK_CTRL_REG_POWERDOWN_BIT 11

	#define LINK_CTRL_REG		0
	#define LINK_STATUS_REG		1
	#define AN_ADVER_REG		4
	#define AN_LINKPAR_REG		5
	smiInit(MDC_GPIO, MDIO_GPIO);
	smiRead(0, LINK_STATUS_REG, &read_data);
	link_status= (read_data==0xffff) ?  0: ((read_data & (1<< LINK_STATUS_REG_BIT)) >> LINK_STATUS_REG_BIT);
	DBG_DUMP("[0]=0x%x, sta=%d\r\n",read_data,link_status);
	smiRead(2, LINK_STATUS_REG, &read_data);
	link_status= (read_data==0xffff) ?  0: ((read_data & (1<< LINK_STATUS_REG_BIT)) >> LINK_STATUS_REG_BIT);
	DBG_DUMP("[1]=0x%x, sta=%d\r\n",read_data,link_status);
	smiRead(5, LINK_STATUS_REG, &read_data);
	link_status= (read_data==0xffff) ?  0: ((read_data & (1<< LINK_STATUS_REG_BIT)) >> LINK_STATUS_REG_BIT);
	DBG_DUMP("[2]=0x%x, sta=%d\r\n",read_data,link_status);
	smiRead(6, LINK_STATUS_REG, &read_data);
	link_status= (read_data==0xffff) ?  0: ((read_data & (1<< LINK_STATUS_REG_BIT)) >> LINK_STATUS_REG_BIT);
	DBG_DUMP("[3]=0x%x, sta=%d\r\n",read_data,link_status);
	smiRead(7, LINK_STATUS_REG, &read_data);
	link_status= (read_data==0xffff) ?  0: ((read_data & (1<< LINK_STATUS_REG_BIT)) >> LINK_STATUS_REG_BIT);
	DBG_DUMP("[4]=0x%x, sta=%d\r\n",read_data,link_status);

	smiRead(0, LINK_CTRL_REG, &read_data);
	DBG_DUMP("0[0]=0x%x\r\n",read_data);
	smiRead(2, LINK_CTRL_REG, &read_data);
	DBG_DUMP("0[1]=0x%x\r\n",read_data);
	smiRead(5, LINK_CTRL_REG, &read_data);
	DBG_DUMP("0[2]=0x%x\r\n",read_data);
	smiRead(6, LINK_CTRL_REG, &read_data);
	DBG_DUMP("0[3]=0x%x\r\n",read_data);
	smiRead(7, LINK_CTRL_REG, &read_data);
	DBG_DUMP("0[4]=0x%x\r\n",read_data);


	smiRead(0, AN_ADVER_REG, &read_data);
	DBG_DUMP("4[0]=0x%x\r\n",read_data);
	smiRead(2, AN_ADVER_REG, &read_data);
	DBG_DUMP("4[1]=0x%x\r\n",read_data);
	smiRead(5, AN_ADVER_REG, &read_data);
	DBG_DUMP("4[2]=0x%x\r\n",read_data);
	smiRead(6, AN_ADVER_REG, &read_data);
	DBG_DUMP("4[3]=0x%x\r\n",read_data);
	smiRead(7, AN_ADVER_REG, &read_data);
	DBG_DUMP("4[4]=0x%x\r\n",read_data);


	smiRead(0, AN_LINKPAR_REG, &read_data);
	DBG_DUMP("5[0]=0x%x\r\n",read_data);
	smiRead(2, AN_LINKPAR_REG, &read_data);
	DBG_DUMP("5[1]=0x%x\r\n",read_data);
	smiRead(5, AN_LINKPAR_REG, &read_data);
	DBG_DUMP("5[2]=0x%x\r\n",read_data);
	smiRead(6, AN_LINKPAR_REG, &read_data);
	DBG_DUMP("5[3]=0x%x\r\n",read_data);
	smiRead(7, AN_LINKPAR_REG, &read_data);
	DBG_DUMP("5[4]=0x%x\r\n",read_data);
#endif
	return TRUE;
}
UINT32 g_TestEthHubForceLink[2]={0xff, 0xff};
static BOOL Cmd_ethcam_set_tx_hub_link(CHAR *strCmd)
{
	UINT32 path_id = 0;
	UINT32 hub_link = 0;

	sscanf_s(strCmd, "%d %d", &path_id,&hub_link);
	DBG_WRN("path_id=%d, hub_link=%d\r\n", path_id,hub_link);
	if(path_id==2){
		g_TestEthHubForceLink[0]=hub_link;
		g_TestEthHubForceLink[1]=hub_link;
	}else{
		g_TestEthHubForceLink[path_id]=hub_link;
	}
	return TRUE;
}
static BOOL Cmd_ethcam_chk_tx_running(CHAR *strCmd)
{
	UINT32 i;
	for(i=0;i<ETH_REARCAM_CAPS_COUNT;i++){
		DBG_DUMP("BsFrameCnt[%d]=%d\r\n",i, socketCliEthData1_GetBsFrameCnt(i));
	}
	return TRUE;
}

static BOOL Cmd_ethcam_ethboot(CHAR *strCmd)
{
	//A://ethcam/
	Ux_OpenWindow(&UIFlowWndWaitMomentCtrl, 1, UIFlowWndWaitMoment_StatusTXT_Msg_STRID_ETHCAM_UDFW_SENDFW);
	System_ChangeSubMode(SYS_SUBMODE_UPDFW);
	Ux_SendEvent(0, NVTEVT_SYSTEM_MODE, 1, PRIMARY_MODE_UPDFW);
	UINT32 j;
	for (j=0; j<ETH_REARCAM_CAPS_COUNT; j++){
		EthCamSocketCli_Close(j, ETHSOCKIPCCLI_ID_2);
	}
	EthCamNet_Ethboot();
	return TRUE;
}
static BOOL Cmd_ethcam_tx_ip_reset(CHAR *strCmd)
{
	UINT32 i;
	for(i=0;i<ETH_REARCAM_CAPS_COUNT;i++){
		EthCam_SendXMLCmd(ETHCAM_PATH_ID_1+i, ETHCAM_PORT_DEFAULT ,ETHCAM_CMD_SET_TX_IP_RESET, 0);
	}
	return TRUE;
}
static BOOL Cmd_ethcam_iperftest(CHAR *strCmd)
{
	EthCamNet_IperfTest();
	return TRUE;
}	
static BOOL Cmd_ethcam_set_tx_flip(CHAR *strCmd)
{
	UINT32 flip = 0;

	sscanf_s(strCmd, "%d ", &flip);
	DBG_DUMP("flip %d\r\n", flip);
	EthCam_SendXMLCmd(ETHCAM_PATH_ID_1, ETHCAM_PORT_DATA1 ,ETHCAM_CMD_TX_STREAM_STOP, 0);
#if (ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
	EthCam_SendXMLCmd(ETHCAM_PATH_ID_1, ETHCAM_PORT_DATA2 ,ETHCAM_CMD_TX_STREAM_STOP, 0);
#endif

	EthCam_SendXMLCmd(ETHCAM_PATH_ID_1, ETHCAM_PORT_DEFAULT ,ETHCAM_CMD_SET_TX_FLIP, flip);

	EthCam_SendXMLCmd(ETHCAM_PATH_ID_1, ETHCAM_PORT_DATA1 ,ETHCAM_CMD_TX_STREAM_START, 0);
#if (ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
	EthCam_SendXMLCmd(ETHCAM_PATH_ID_1, ETHCAM_PORT_DATA2 ,ETHCAM_CMD_TX_STREAM_START, 0);
#endif
	return TRUE;
}

#endif
#if (defined(_NVT_ETHREARCAM_TX_))
#include "EthCamAppNetwork.h"
static BOOL Cmd_ethcam_tx_ip_set(CHAR *strCmd)
{
	UINT32 ip_addr_item = 0;
	UINT32 ip_addr = 0;

	sscanf_s(strCmd, "%d", &ip_addr_item);
	DBG_DUMP("input=%d\r\n", ip_addr_item);

	if(ip_addr_item==0){
		ip_addr=0;
		DBG_DUMP("ip_addr: 0.0.0.0\r\n");
	}else if(ip_addr_item==1){
		ip_addr=ipstr2int("192.168.0.12");
		DBG_DUMP("ip_addr: 192.168.0.12\r\n");
	}else if(ip_addr_item==2){
		ip_addr=ipstr2int("192.168.0.13");
		DBG_DUMP("ip_addr: 192.168.0.13\r\n");
	}else{
		DBG_ERR("input wrong parameter\r\n");
	}

	SysSetFlag(FL_ETHCAM_TX_IP_ADDR, ip_addr);

	return TRUE;
}
#endif

SXCMD_BEGIN(ts, "test command")
#if (defined(_NVT_ETHREARCAM_RX_))
SXCMD_ITEM("ethcamfwud", Cmd_ethcam_tx_fwupdate, "EthCam Tx Fw Update")
SXCMD_ITEM("ethcamethboot", Cmd_ethcam_ethboot, "EthCam Eth Boot Update")
SXCMD_ITEM("ethlk %d", Cmd_ethcam_set_tx_link_status, "EthCam Set Tx Link")
SXCMD_ITEM("gethub", Cmd_get_eth_hub_status, "Get eth hub status")
SXCMD_ITEM("hublk %d", Cmd_ethcam_set_tx_hub_link, "Set eth hub link")
SXCMD_ITEM("chkrun", Cmd_ethcam_chk_tx_running, "Check Tx path is running")
SXCMD_ITEM("ipreset", Cmd_ethcam_tx_ip_reset, "Reset Tx IP save in pstore")
SXCMD_ITEM("ethcamiperf", Cmd_ethcam_iperftest, "Run iperf test")
SXCMD_ITEM("txflip %d", Cmd_ethcam_set_tx_flip, "Set tx flip")
#endif
#if (defined(_NVT_ETHREARCAM_TX_))
SXCMD_ITEM("ipset %d", Cmd_ethcam_tx_ip_set, "Set Tx IP save in pstore")
#endif
SXCMD_END()

