#include "PrjCfg.h"

#if (defined(_NVT_ETHREARCAM_TX_) || defined(_NVT_ETHREARCAM_RX_))
#include "WifiAppCmd.h"
#include "NVTToolCommand.h"
#include "UIInfo.h"
#include "UsockIpcAPI.h"
#include "nvtmpp.h"
#include "SysCfg.h"
#include "WifiAppXML.h"
#include "UIAppPhoto.h"
#include "ImageUnit_VdoDec.h"
#include "UIAppMovie.h"
#include "UIAppNetwork.h"
#include "EthCamAppCmd.h"
#include "ImageApp_MovieCommon.h"
#include "ImageApp_MovieMulti.h"
#include "SysMain.h"
#include "SysCommon.h"
#include "UIMovieMapping.h"
#include "UIAppMovie.h"
#include "UIAppNetwork.h"
#include "UIModeUpdFw.h"
#include "EthCamSocket.h"
#include "EthCamAppSocket.h"
#include "EthCamAppNetwork.h"
#include "HwClock.h"
#include "ImageApp_MovieMulti.h"
#include "timer.h"
#include "MsdcNvtCb.h"
#include "NvtIpcAPI.h"
#if( defined(_NVT_ETHREARCAM_TX_) && AV_LCA_FUNC == ENABLE)
#include "av_lca_lib.h"
#endif
#include "ImageUnit_ImgTrans.h"
#include "ImageUnit_AudDec.h"

#define THIS_DBGLVL         2 // 0=FATAL, 1=ERR, 2=WRN, 3=UNIT, 4=FUNC, 5=IND, 6=MSG, 7=VALUE, 8=USER
///////////////////////////////////////////////////////////////////////////////
#define __MODULE__          EthCamAppCmd
#define __DBGLVL__          ((THIS_DBGLVL>=PRJ_DBG_LVL)?THIS_DBGLVL:PRJ_DBG_LVL)
#define __DBGFLT__          "*" //*=All, [mark]=CustomClass
#include "DebugModule.h"
///////////////////////////////////////////////////////////////////////////////


UINT32 ETHCAM_CMD_SND_FLG_ID = 0;
UINT32 ETHCAM_CMD_RCV_FLG_ID = 0;
UINT32 ETHCAM_CMD_SND_TSK_ID = 0;
UINT32 ETHCAM_CMD_RCV_TSK_ID = 0;

UINT32 ETHCAM_CMD_SNDDATA1_SEM_ID = 0;
UINT32 ETHCAM_CMD_SNDDATA2_SEM_ID = 0;
UINT32 ETHCAM_CMD_SNDCMD_SEM_ID = 0;

#if(defined(_NVT_ETHREARCAM_TX_))
UINT32 ETHCAM_CMD_SNDFRAME_SEM_ID = 0;
UINT32 ETHCAM_CMD_SNDFRAME_CLONE_SEM_ID = 0;
extern int SX_TIMER_ETHCAM_LINKDET_ID;
ETHCAM_TX_SYS_INFO sEthCamSysInfo={0};//for Tx
ETHCAM_TX_CODEC_SRCTYPE sEthCamCodecSrctype={0};//for Tx
UINT32  g_EthCamCmdGetFrame_SndNum = 0;
UINT32 g_EthCamTxIsAudCapEn=0;
#endif
#if(defined(_NVT_ETHREARCAM_RX_))
UINT32 ETHCAM_WIFICB_VDOFRM_SEM_ID = 0;
UINT32 ETHCAM_DISP_DATA_SEM_ID = 0;
extern UINT32 EthCamHB1[ETHCAM_PATH_ID_MAX], EthCamHB2;
ETHCAM_TX_SYS_INFO sEthCamTxSysInfo[ETHCAM_PATH_ID_MAX]={0};//for Rx
ETHCAM_TX_CODEC_SRCTYPE sEthCamCodecSrctype[ETHCAM_PATH_ID_MAX]={0};//for Rx
UINT32 g_EthCamCfgTxAudCap=0;
#endif
static BOOL g_bEthCamCmdOpened = FALSE;
static ETHCAM_XML_RESULT *g_pEthCamCmdRstTbl=NULL;
ETHCAM_TX_DEC_INFO sEthCamTxDecInfo[ETHCAM_PATH_ID_MAX]={0};
ETHCAM_TX_REC_INFO sEthCamTxRecInfo[ETHCAM_PATH_ID_MAX]={0};
INT32 g_SocketCmd_Status[ETHCAM_PATH_ID_MAX]={0};
ETHCAM_TX_AUDINFO sEthCamTxAudInfo[ETHCAM_PATH_ID_MAX]={0};

//ETHCAM_MENU_SETTING g_sEthCamMenuSetting={0};
ETHCAM_FWUD sEthCamFwUd={0};
static TIMER_ID     g_EthCamCmd_TimerID = 0;

#if(defined(_NVT_ETHREARCAM_RX_))
static TIMER_ID     g_EthCamCmdGetFrame_TimerID = TIMER_NUM;

static void EthCamCmd_GetFrameTimerCB(UINT32 TimerId)
{
	set_flg(ETHCAM_CMD_SND_FLG_ID,FLG_ETHCAM_CMD_GETFRM);
}
#endif

void EthCamCmd_GetFrameTimerEn(BOOL En)
{
#if(defined(_NVT_ETHREARCAM_RX_))
	if(En){
		if(sEthCamTxSysInfo[0].PullModeEn){
			timer_pausePlay(g_EthCamCmdGetFrame_TimerID, TIMER_STATE_PLAY);
		}else{
			DBG_ERR("PullModeEn not enable!\r\n");
		}
	}else{
		timer_pausePlay(g_EthCamCmdGetFrame_TimerID, TIMER_STATE_PAUSE);
	}
#endif
}
static void EthCamCmd_TimeOutCB(UINT32 TimerId)
{
	//EthCamCmd_Done(ETHCAM_CMD_TIMEOUT, ETHCAM_RET_TIMEOUT);
}

ER EthCamCmdTsk_Open(void)
{
	if (g_bEthCamCmdOpened==1) {
		return E_SYS;
	}

#if(defined(_NVT_ETHREARCAM_RX_))
	if(sEthCamTxSysInfo[0].PullModeEn){
		if (timer_open(&g_EthCamCmdGetFrame_TimerID, EthCamCmd_GetFrameTimerCB) != E_OK) {
			DBG_ERR("open GetFrame timer fail\r\n");
		}
		// timeout =1 sec
		timer_cfg(g_EthCamCmdGetFrame_TimerID, 30 * 1000/*ms*/, TIMER_MODE_FREE_RUN | TIMER_MODE_ENABLE_INT, TIMER_STATE_PAUSE);
	}
#endif

	if (timer_open(&g_EthCamCmd_TimerID, EthCamCmd_TimeOutCB) != E_OK) {
		DBG_ERR("open timer fail\r\n");
	}
	// timeout =1 sec
	timer_cfg(g_EthCamCmd_TimerID, 3000 * 1000/*ms*/, TIMER_MODE_FREE_RUN | TIMER_MODE_ENABLE_INT, TIMER_STATE_PAUSE);

	// clear flag first
	clr_flg(ETHCAM_CMD_SND_FLG_ID, 0xFFFFFFFF);
	clr_flg(ETHCAM_CMD_RCV_FLG_ID, 0xFFFFFFFF);


	sta_tsk(ETHCAM_CMD_SND_TSK_ID, 0);
	sta_tsk(ETHCAM_CMD_RCV_TSK_ID, 0);
#if(defined(_NVT_ETHREARCAM_TX_))
	g_EthCamCmdGetFrame_SndNum=0;
#endif
	g_bEthCamCmdOpened=1;
	return E_OK;
}

ER EthCamCmdTsk_Close(void)
{
	FLGPTN FlgPtn;

	if (g_bEthCamCmdOpened==0) {
		return E_SYS;
	}

	// stop task
	set_flg(ETHCAM_CMD_SND_FLG_ID, FLG_ETHCAM_CMD_STOP);
	wai_flg(&FlgPtn, ETHCAM_CMD_SND_FLG_ID, FLG_ETHCAM_CMD_IDLE, TWF_ORW | TWF_CLR);

	set_flg(ETHCAM_CMD_RCV_FLG_ID, FLG_ETHCAM_CMD_STOP);
	wai_flg(&FlgPtn, ETHCAM_CMD_RCV_FLG_ID, FLG_ETHCAM_CMD_IDLE, TWF_ORW | TWF_CLR);

	ter_tsk(ETHCAM_CMD_SND_TSK_ID);
	ter_tsk(ETHCAM_CMD_RCV_TSK_ID);

#if(defined(_NVT_ETHREARCAM_RX_))
	if(g_EthCamCmdGetFrame_TimerID !=TIMER_NUM){
		timer_pausePlay(g_EthCamCmdGetFrame_TimerID, TIMER_STATE_PAUSE);
		timer_close(g_EthCamCmdGetFrame_TimerID);
	}
#endif
	timer_close(g_EthCamCmd_TimerID);

	g_bEthCamCmdOpened=0;
	return E_OK;
}


void EthCamCmd_SndData1Lock(void)
{
    DBG_IND("SndTsk Lock\r\n");
    wai_sem(ETHCAM_CMD_SNDDATA1_SEM_ID);
}
void EthCamCmd_SndData1Unlock(void)
{
    DBG_IND("SndTsk Unlock\r\n");
    sig_sem(ETHCAM_CMD_SNDDATA1_SEM_ID);
}
void EthCamCmd_SndData2Lock(void)
{
    DBG_IND("SndTsk Lock\r\n");
    wai_sem(ETHCAM_CMD_SNDDATA2_SEM_ID);
}
void EthCamCmd_SndData2Unlock(void)
{
    DBG_IND("SndTsk Unlock\r\n");
    sig_sem(ETHCAM_CMD_SNDDATA2_SEM_ID);
}
void EthCamCmd_SndCmdLock(void)
{
    DBG_IND("SndTsk Lock\r\n");
    wai_sem(ETHCAM_CMD_SNDCMD_SEM_ID);
}
void EthCamCmd_SndCmdUnlock(void)
{
    DBG_IND("SndTsk Unlock\r\n");
    sig_sem(ETHCAM_CMD_SNDCMD_SEM_ID);
}

//#define MAX_RETRY_CNT (100)
INT32 EthCamCmd_Send(ETHCAM_PATH_ID path_id, char* addr, int* size)
{
	INT32 result=0;
	static UINT32 errCnt=0;

	EthCamCmd_SndCmdLock();
#if(defined(_NVT_ETHREARCAM_TX_))
	result=EthCamSocket_Send(path_id, ETHSOCKET_CMD, addr, size);
#else
	result=EthCamSocket_Send(path_id, ETHSOCKETCLI_CMD, addr, size);
#endif
	EthCamCmd_SndCmdUnlock();
	if(result!=0){
		if(errCnt % 20 == 0){
			DBG_ERR("[%d]result=%d, size=%d\r\n",path_id,result,*size);
		}
		errCnt++;
	}else{
		errCnt=0;
	}

	return result;
}
void EthCamCmdSnd_Tsk(void)
{
	FLGPTN  FlgPtn;
#if (defined(_NVT_ETHREARCAM_RX_))
	UINT32 GetFrmNum=1;
#endif
	DBG_IND("EthCamCmdSnd_Tsk() start\r\n");

	kent_tsk();
	//UINT32          uiTime;

	while (ETHCAM_CMD_SND_TSK_ID) {
		set_flg(ETHCAM_CMD_SND_FLG_ID, FLG_ETHCAM_CMD_IDLE);
		PROFILE_TASK_IDLE();
		wai_flg(&FlgPtn, ETHCAM_CMD_SND_FLG_ID, FLG_ETHCAM_CMD_STOP|FLG_ETHCAM_CMD_SND|FLG_ETHCAM_CMD_GETFRM, TWF_ORW | TWF_CLR);
		PROFILE_TASK_BUSY();
		clr_flg(ETHCAM_CMD_SND_FLG_ID, FLG_ETHCAM_CMD_IDLE);
		if (FlgPtn & FLG_ETHCAM_CMD_STOP){
			break;
		}

		if (FlgPtn & FLG_ETHCAM_CMD_SND) {
			DBG_IND("FLG_ETHCAM_CMD_SND\r\n");
			#if (defined(_NVT_ETHREARCAM_TX_))
			EthCamCmd_Send(0, (char*)sEthCamSendCmdInfo.ParserBuf, (int*)&sEthCamSendCmdInfo.BufSize);
			#endif
			#if (defined(_NVT_ETHREARCAM_RX_))
			EthCamCmd_Send(sEthCamSendCmdInfo.path_id, (char*)sEthCamSendCmdInfo.ParserBuf, (int*)&sEthCamSendCmdInfo.BufSize);
			#endif
		}
		if (FlgPtn & FLG_ETHCAM_CMD_GETFRM) {
			DBG_IND("FLG_ETHCAM_CMD_GETFRM\r\n");
			#if (defined(_NVT_ETHREARCAM_RX_))
			if(sEthCamTxSysInfo[0].PullModeEn){
				static UINT32 EthCamPreHB1[ETHCAM_PATH_ID_MAX]={0};
				if(EthCamNet_GetEthLinkStatus(ETHCAM_PATH_ID_1)==ETHCAM_LINK_UP && EthCamNet_GetPrevEthLinkStatus(ETHCAM_PATH_ID_1)==ETHCAM_LINK_UP){
					if(EthCamHB1[ETHCAM_PATH_ID_1]>2 && EthCamPreHB1[ETHCAM_PATH_ID_1]!=EthCamHB1[ETHCAM_PATH_ID_1]){// 4 sec
						DBG_WRN("reSend Strm Start[0]\r\n");
						EthCamPreHB1[ETHCAM_PATH_ID_1]=EthCamHB1[ETHCAM_PATH_ID_1];
						EthCam_SendXMLCmd(ETHCAM_PATH_ID_1, ETHCAM_PORT_DATA1 ,ETHCAM_CMD_TX_STREAM_START, 0);
					}
					//uiTime = Perf_GetCurrent();
					if(socketCliEthData1_IsConn(ETHCAM_PATH_ID_1)){
						EthCam_SendXMLCmd(ETHCAM_PATH_ID_1, ETHCAM_PORT_DEFAULT ,ETHCAM_CMD_GET_FRAME, GetFrmNum);
					}
					//DBG_DUMP("t1: %dus\r\n", Perf_GetCurrent() - uiTime);
				}
				if(EthCamNet_GetEthLinkStatus(ETHCAM_PATH_ID_2)==ETHCAM_LINK_UP && EthCamNet_GetPrevEthLinkStatus(ETHCAM_PATH_ID_2)==ETHCAM_LINK_UP){
					if(EthCamHB1[ETHCAM_PATH_ID_2]>2 && EthCamPreHB1[ETHCAM_PATH_ID_2]!=EthCamHB1[ETHCAM_PATH_ID_2]){// 4 sec
						DBG_WRN("reSend Strm Start[1]\r\n");
						EthCamPreHB1[ETHCAM_PATH_ID_2]=EthCamHB1[ETHCAM_PATH_ID_2];
						EthCam_SendXMLCmd(ETHCAM_PATH_ID_2, ETHCAM_PORT_DATA1 ,ETHCAM_CMD_TX_STREAM_START, 0);
					}
					if(socketCliEthData1_IsConn(ETHCAM_PATH_ID_2)){
						EthCam_SendXMLCmd(ETHCAM_PATH_ID_2, ETHCAM_PORT_DEFAULT ,ETHCAM_CMD_GET_FRAME, GetFrmNum);
					}
				}
			}
			#endif
		}
	}
	ext_tsk();
}

//#define MAX_DATA_SIZE (230 * 1024)
INT32 EthCamData1_Send(char* addr, int* size)
{
	INT32 result=0;
	static UINT32 errCnt=0;
#if(defined(_NVT_ETHREARCAM_RX_))
	ETHCAM_PATH_ID path_id=ETHCAM_PATH_ID_1;
#endif

	EthCamCmd_SndData1Lock();

#if(defined(_NVT_ETHREARCAM_TX_))
	result=EthCamSocket_Send(0, ETHSOCKET_DATA1, addr, size);
#else
	result=EthCamSocket_Send(path_id, ETHSOCKETCLI_DATA1, addr, size);
#endif
	EthCamCmd_SndData1Unlock();

	if(result!=0){
		if(errCnt % 20 == 0){
			DBG_ERR("result=%d, size=%d\r\n",result,*size);
		}
		errCnt++;
	}else{
		errCnt=0;
	}

	return result;
}
INT32 EthCamData2_Send(char* addr, int* size)
{
	INT32 result=0;
	static UINT32 errCnt=0;
#if(defined(_NVT_ETHREARCAM_RX_))
	ETHCAM_PATH_ID path_id=ETHCAM_PATH_ID_1;
#endif
	EthCamCmd_SndData2Lock();

#if(defined(_NVT_ETHREARCAM_TX_))
	result=EthCamSocket_Send(0, ETHSOCKET_DATA2, addr, size);
#else
	result=EthCamSocket_Send(path_id, ETHSOCKETCLI_DATA2, addr, size);
#endif
 	EthCamCmd_SndData2Unlock();
	if(result!=0){
		if(errCnt % 20 == 0){
			DBG_ERR("result=%d\r\n",result);
		}
		errCnt++;
	}

	return result;
}

void EthCamCmdRcvHandler_Tsk(void)
{
	FLGPTN  FlgPtn;
#if (defined(_NVT_ETHREARCAM_TX_) && (ETH_REARCAM_CAPS_COUNT>=2))
	UINT16 i;
#endif
	DBG_IND("EthCamCmdRcvHandler_Tsk() start\r\n");

	kent_tsk();

	while (ETHCAM_CMD_RCV_TSK_ID) {
		set_flg(ETHCAM_CMD_RCV_FLG_ID, FLG_ETHCAM_CMD_IDLE);
		PROFILE_TASK_IDLE();
		wai_flg(&FlgPtn, ETHCAM_CMD_RCV_FLG_ID, FLG_ETHCAM_CMD_STOP|FLG_ETHCAM_CMD_RCV|FLG_ETHCAM_CMD_SETIP, TWF_ORW | TWF_CLR);
		PROFILE_TASK_BUSY();
		clr_flg(ETHCAM_CMD_RCV_FLG_ID, FLG_ETHCAM_CMD_IDLE);
		if (FlgPtn & FLG_ETHCAM_CMD_STOP){
			break;
		}
		if (FlgPtn & FLG_ETHCAM_CMD_RCV) {
#if (defined(_NVT_ETHREARCAM_TX_) && (ETH_REARCAM_CAPS_COUNT>=2))
			DBG_IND("FLG_ETHCAM_CMD_RCV\r\n");
			for(i=0;i<g_EthCamCmdGetFrame_SndNum;i++){
				MovieExe_EthCamRecId1_SendFrm();
				MovieExe_EthCamCloneId1_SendFrm();
			}
			EthCam_SendXMLStatus(0, ETHCAM_PORT_DEFAULT, ETHCAM_CMD_GET_FRAME, ETHCAM_RET_OK);
#endif
		}
		if (FlgPtn & FLG_ETHCAM_CMD_SETIP) {
#if (defined(_NVT_ETHREARCAM_TX_) && (ETH_REARCAM_CAPS_COUNT>=2))
			DBG_IND("FLG_ETHCAM_CMD_SETIP\r\n");
			System_CPU2_WaitReady();
			EthCamNet_SetTxIPAddr();
#endif
		}
	}
	ext_tsk();
}
void EthCamCmdHandler_InstallID(void)
{
	OS_CONFIG_SEMPHORE(ETHCAM_CMD_SNDDATA1_SEM_ID, 0, 1, 1);
	OS_CONFIG_SEMPHORE(ETHCAM_CMD_SNDDATA2_SEM_ID, 0, 1, 1);
	OS_CONFIG_SEMPHORE(ETHCAM_CMD_SNDCMD_SEM_ID, 0, 1, 1);

	OS_CONFIG_FLAG(ETHCAM_CMD_SND_FLG_ID);
	OS_CONFIG_FLAG(ETHCAM_CMD_RCV_FLG_ID);
	OS_CONFIG_TASK(ETHCAM_CMD_SND_TSK_ID,    PRI_ETHCAM_CMD_SND,   STKSIZE_ETHCAM_CMD_SND,   EthCamCmdSnd_Tsk);
	OS_CONFIG_TASK(ETHCAM_CMD_RCV_TSK_ID,     PRI_ETHCAM_CMD_RCV,       STKSIZE_ETHCAM_CMD_RCV,        EthCamCmdRcvHandler_Tsk);

#if(defined(_NVT_ETHREARCAM_TX_))
	OS_CONFIG_SEMPHORE(ETHCAM_CMD_SNDFRAME_SEM_ID, 0, 1, 1);
	OS_CONFIG_SEMPHORE(ETHCAM_CMD_SNDFRAME_CLONE_SEM_ID, 0, 1, 1);
#endif

#if(defined(_NVT_ETHREARCAM_RX_) && WIFI_FUNC==ENABLE)
	OS_CONFIG_SEMPHORE(ETHCAM_WIFICB_VDOFRM_SEM_ID, 0, 1, 1);
#endif

#if(defined(_NVT_ETHREARCAM_RX_) && ETH_REARCAM_CAPS_COUNT==1)
	OS_CONFIG_SEMPHORE(ETHCAM_DISP_DATA_SEM_ID, 0, 1, 1);
#endif
}

void EthCam_GetCmdStr(UINT32 cmd, UINT32 par, UINT8 *addr, UINT32 *size)
{
	//char pbuf[] = "/?custom=1&cmd=xxxx&par=0";
	char pbuf[128] = {0};
	sprintf(pbuf, "/?custom=1&cmd=%d&par=%d", cmd, par);
	*size=strlen(pbuf)+1;

	memcpy(addr, pbuf, *size);
	DBG_DUMP("bufSize=%d\r\n",*size);
	DBG_DUMP("cmd=%s\r\n",addr);
}
static void EthCamCmd_TimeOutTimerCB1(UINT32 uiEvent)
{
	EthCamCmd_Done(ETHCAM_PATH_ID_1, ETHCAM_CMD_TIMEOUT, ETHCAM_RET_TIMEOUT);
}
static void EthCamCmd_TimeOutTimerCB2(UINT32 uiEvent)
{
	EthCamCmd_Done(ETHCAM_PATH_ID_2, ETHCAM_CMD_TIMEOUT, ETHCAM_RET_TIMEOUT);
}

char pSendXMLCmdBuf[128] = {0};
//UINT32 EthCam_SendXMLCmd(UINT32 cmd,UINT32 par)
INT32 EthCam_SendXMLCmd(ETHCAM_PATH_ID path_id, ETHCAM_PORT_TYPE port_type, UINT32 cmd,UINT32 par)
{
	INT32 result=0;
	UINT32 len =0;

	CHAR* dest_ip[ETHCAM_PATH_ID_MAX]={SocketInfo[0].ip,SocketInfo[1].ip};
	UINT32 socket_port[ETHCAM_PATH_ID_MAX][ETHCAM_PORT_DATA_MAX]={{SocketInfo[0].port[2], SocketInfo[0].port[0], SocketInfo[0].port[1]},
																{SocketInfo[1].port[2], SocketInfo[1].port[0], SocketInfo[1].port[1]}};
	SWTIMER_ID   timer_id;
	BOOL         isOpenTimerOK = TRUE;
	SWTIMER_CB EventHandler;

	//sprintf(pSendXMLCmdBuf, "/?custom=1&cmd=%d&par=%d", cmd, par);
	sprintf(pSendXMLCmdBuf, "%s:%d/?custom=1&cmd=%d&par=%d", dest_ip[path_id], socket_port[path_id][port_type], cmd, par);
	len=strlen(pSendXMLCmdBuf)+1;
	//debug_msg("%s\r\n",pSendXMLCmdBuf);
	DBG_IND("len=%d, cmd=%s\r\n",len,pSendXMLCmdBuf);
	EthCamCmd_ClrFlg(path_id, (ETHCAM_CMD_DONE|ETHCAM_CMD_TIMEOUT));
	if(path_id==ETHCAM_PATH_ID_1){
		EventHandler=(SWTIMER_CB)EthCamCmd_TimeOutTimerCB1;
	}else{
		EventHandler=(SWTIMER_CB)EthCamCmd_TimeOutTimerCB2;
	}
	if (SwTimer_Open(&timer_id, EventHandler) != E_OK) {
		DBG_ERR("open timer fail\r\n");
		isOpenTimerOK = FALSE;
	} else {
		SwTimer_Cfg(timer_id, 3000 /*ms*/, SWTIMER_MODE_FREE_RUN);
		SwTimer_Start(timer_id);
	}

	//timer_pausePlay(g_EthCamCmdTimeOut_TimerID, TIMER_STATE_PLAY);

	result= EthCamCmd_Send(path_id, (char *)pSendXMLCmdBuf, (int*)&len);
	//DBG_DUMP("result=%d\r\n",result);

	if(result==0){

		result =EthCamCmd_WaitFinish(path_id, (ETHCAM_CMD_DONE|ETHCAM_CMD_TIMEOUT));

		if(ETHCAM_RET_TIMEOUT == result){
			DBG_ERR("cmd Timeout!cmd=%s\r\n",pSendXMLCmdBuf);
		}
	}else{
		DBG_ERR("result=%d, %s\r\n",result,pSendXMLCmdBuf);
	}
	if (isOpenTimerOK) {
		SwTimer_Stop(timer_id);
		SwTimer_Close(timer_id);
	}
	debug_msg("--cj SendXMLCmd  result=%d\r\n",result);
	return result;
}
INT32 EthCam_SendXMLData(ETHCAM_PATH_ID path_id, UINT8* addr, UINT32 size)
{
	UINT32 result=0;
	EthCamCmd_ClrFlg(path_id, ETHCAM_CMD_DONE);
	//timer_pausePlay(g_EthCamCmd_TimerID, TIMER_STATE_PLAY);

	result= EthCamCmd_Send(path_id, (char *)addr, (int*)&size);
	if(result==0){

		result=EthCamCmd_WaitFinish(path_id, (ETHCAM_CMD_DONE|ETHCAM_CMD_TIMEOUT));
		if(ETHCAM_RET_TIMEOUT == result){
			DBG_ERR("cmd Timeout!\r\n");
		}
	}else{
		DBG_ERR("result=%d\r\n",result);
	}
	//timer_pausePlay(g_EthCamCmd_TimerID, TIMER_STATE_PAUSE);
	debug_msg(" --cj SendXMLData   result=%d\r\n",result);
	return result;
}

INT32 EthCam_SendXMLStatusCB(ETHCAM_PATH_ID path_id, ETHCAM_PORT_TYPE port_type, UINT32 cmd, UINT32 status)
{
	static char bufArry[128];
	static UINT32 len = 0;
	INT32 result = E_OK;
	char *buf = bufArry;
	UINT32 sendlen = 0;
	UINT32 bufSize = 128;
	memset(bufArry, 0, sizeof(bufArry));
	//CHAR* dest_ip[ETHCAM_PATH_ID_MAX]={gEthCamTxIP[0].ip,gEthCamTxIP[1].ip};
	CHAR* dest_ip[ETHCAM_PATH_ID_MAX]={SocketInfo[0].ip,SocketInfo[1].ip};
	//UINT32 socket_port[ETHCAM_PORT_DATA_MAX]={ETH_CMD_SOCKET_PORT, ETH_DATA1_SOCKET_PORT, ETH_DATA2_SOCKET_PORT};
	UINT32 socket_port[ETHCAM_PATH_ID_MAX][ETHCAM_PORT_DATA_MAX]={{SocketInfo[0].port[2], SocketInfo[0].port[0], SocketInfo[0].port[1]},
																{SocketInfo[1].port[2], SocketInfo[1].port[0], SocketInfo[1].port[1]}};

	//len = XML_snprintf(&buf, &bufSize,  DEF_XML_HEAD);
	len = XML_snprintf(&buf, &bufSize, "%s:%d<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n" ,dest_ip[path_id], socket_port[path_id][port_type]);
	len = XML_snprintf(&buf, &bufSize, DEF_XML_RET, cmd, status);

	len = buf - bufArry;
	sendlen = len;

	//DBG_DUMP("%d\r\n",len);
	//DBG_DUMP("%s\r\n",bufArry);

	if (len < 128) {
		#if (defined(_NVT_ETHREARCAM_TX_))
		memcpy(sEthCamSendCmdInfo.ParserBuf, bufArry, len);
		sEthCamSendCmdInfo.BufSize=len;
		set_flg(ETHCAM_CMD_SND_FLG_ID,FLG_ETHCAM_CMD_SND);
		#else
		DBG_IND(" %s %d\r\n", bufArry, sendlen);
		result = EthCamCmd_Send(path_id, (char*)bufArry, (int *)&len);
		if (sendlen != len) {
			result = E_SYS;
			DBG_ERR("sent %d error,should %d\r\n", len, sendlen);
		}
		#endif
	} else {
		DBG_ERR("len %d err\r\n", len);
		result = E_SYS;
	}

	return result;

}

INT32 EthCam_SendXMLStatus(ETHCAM_PATH_ID path_id, ETHCAM_PORT_TYPE port_type, UINT32 cmd, UINT32 status)
{
	static char bufArry[128];
	static UINT32 len = 0;
	INT32 result = E_OK;
	char *buf = bufArry;
	UINT32 sendlen = 0;
	UINT32 bufSize = 128;
	memset(bufArry, 0, sizeof(bufArry));
	//CHAR* dest_ip[ETHCAM_PATH_ID_MAX]={gEthCamTxIP[0].ip,gEthCamTxIP[1].ip};
	CHAR* dest_ip[ETHCAM_PATH_ID_MAX]={SocketInfo[0].ip,SocketInfo[1].ip};
	//UINT32 socket_port[ETHCAM_PORT_DATA_MAX]={ETH_CMD_SOCKET_PORT, ETH_DATA1_SOCKET_PORT, ETH_DATA2_SOCKET_PORT};
	UINT32 socket_port[ETHCAM_PATH_ID_MAX][ETHCAM_PORT_DATA_MAX]={{SocketInfo[0].port[2], SocketInfo[0].port[0], SocketInfo[0].port[1]},
																{SocketInfo[1].port[2], SocketInfo[1].port[0], SocketInfo[1].port[1]}};

	//len = XML_snprintf(&buf, &bufSize,  DEF_XML_HEAD);
	len = XML_snprintf(&buf, &bufSize, "%s:%d<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n" ,dest_ip[path_id], socket_port[path_id][port_type]);
	len = XML_snprintf(&buf, &bufSize, DEF_XML_RET, cmd, status);

	len = buf - bufArry;
	sendlen = len;

	//DBG_DUMP("%d\r\n",len);
	//DBG_DUMP("%s\r\n",bufArry);

	if (len < 128) {
		#if (defined(_NVT_ETHREARCAM_TX_))
		#if 0
		memcpy(sEthCamSendCmdInfo.ParserBuf, bufArry, len);
		sEthCamSendCmdInfo.BufSize=len;
		set_flg(ETHCAM_CMD_SND_FLG_ID,FLG_ETHCAM_CMD_SND);
		#else
		result = EthCamCmd_Send(0, (char*)bufArry, (int*)&len);
		#endif
		#else
		DBG_IND(" %s %d\r\n", bufArry, sendlen);
		result = EthCamCmd_Send(path_id, (char*)bufArry, (int *)&len);
		if (sendlen != len) {
			result = E_SYS;
			DBG_ERR("sent %d error,should %d\r\n", len, sendlen);
		}
		#endif
	} else {
		DBG_ERR("len %d err\r\n", len);
		result = E_SYS;
	}

	return result;

}
INT32 EthCam_SendXMLStr(ETHCAM_PATH_ID path_id, ETHCAM_PORT_TYPE port_type, UINT32 cmd, CHAR* str)
{
	static char bufArry[256];
	static UINT32 len = 0;
	INT32 result = E_OK;
	char *buf = bufArry;
	UINT32 sendlen = 0;
	UINT32 bufSize = 256;
	memset(bufArry, 0, sizeof(bufArry));
	CHAR* dest_ip[ETHCAM_PATH_ID_MAX]={SocketInfo[0].ip,SocketInfo[1].ip};
	UINT32 socket_port[ETHCAM_PATH_ID_MAX][ETHCAM_PORT_DATA_MAX]={{SocketInfo[0].port[2], SocketInfo[0].port[0], SocketInfo[0].port[1]},
																{SocketInfo[1].port[2], SocketInfo[1].port[0], SocketInfo[1].port[1]}};

	//len = XML_snprintf(&buf, &bufSize,  DEF_XML_HEAD);
	len = XML_snprintf(&buf, &bufSize, "%s:%d<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n" ,dest_ip[path_id], socket_port[path_id][port_type]);
	len = XML_snprintf(&buf, &bufSize, DEF_XML_STR, cmd, 0, str);

	len = buf - bufArry;
	sendlen = len;

	//DBG_DUMP("%d\r\n",len);
	//DBG_DUMP("Str bufArry=%s\r\n",bufArry);
	EthCamCmd_ClrFlg(path_id, (ETHCAM_CMD_DONE|ETHCAM_CMD_TIMEOUT));

	if (len < 256) {
		#if (defined(_NVT_ETHREARCAM_TX_))
		result = EthCamCmd_Send(0, (char*)bufArry, (int*)&len);
		if (sendlen != len || result !=0) {
			result = E_SYS;
			DBG_ERR("sent error %d, %d\r\n", len, sendlen);
		}else{
			result = EthCamCmd_WaitFinish(path_id, (ETHCAM_CMD_DONE|ETHCAM_CMD_TIMEOUT));
		}
		#else
		DBG_IND(" %s %d\r\n", bufArry, sendlen);
		result = EthCamCmd_Send(path_id, (char*)bufArry, (int *)&len);
		if (sendlen != len) {
			result = E_SYS;
			DBG_ERR("sent %d error,should %d\r\n", len, sendlen);
		}
		#endif
	} else {
		DBG_ERR("len %d err\r\n", len);
		result = E_SYS;
	}

	return result;

}
INT32 EthCam_SendXMLValue(ETHCAM_PATH_ID path_id, ETHCAM_PORT_TYPE port_type, UINT32 cmd, UINT64 value)
{
	static char bufArry[256];
	static UINT32 len = 0;
	INT32 result = E_OK;
	char *buf = bufArry;
	UINT32 sendlen = 0;
	UINT32 bufSize = 256;
	memset(bufArry, 0, sizeof(bufArry));
	CHAR* dest_ip[ETHCAM_PATH_ID_MAX]={SocketInfo[0].ip,SocketInfo[1].ip};
	UINT32 socket_port[ETHCAM_PATH_ID_MAX][ETHCAM_PORT_DATA_MAX]={{SocketInfo[0].port[2], SocketInfo[0].port[0], SocketInfo[0].port[1]},
																{SocketInfo[1].port[2], SocketInfo[1].port[0], SocketInfo[1].port[1]}};

	//len = XML_snprintf(&buf, &bufSize,  DEF_XML_HEAD);
	len = XML_snprintf(&buf, &bufSize, "%s:%d<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n" ,dest_ip[path_id], socket_port[path_id][port_type]);
	len = XML_snprintf(&buf, &bufSize, DEF_XML_VALUE, cmd, 0, value);

	len = buf - bufArry;
	sendlen = len;

	//DBG_DUMP("%d\r\n",len);
	//DBG_DUMP("Value bufArry=%s\r\n",bufArry);
	EthCamCmd_ClrFlg(path_id, (ETHCAM_CMD_DONE|ETHCAM_CMD_TIMEOUT));

	if (len < 256) {
		#if (defined(_NVT_ETHREARCAM_TX_))
		result = EthCamCmd_Send(0, (char*)bufArry, (int*)&len);
		if (sendlen != len || result !=0) {
			result = E_SYS;
			DBG_ERR("sent error %d, %d\r\n", len, sendlen);
		}else{
			result = EthCamCmd_WaitFinish(path_id, (ETHCAM_CMD_DONE|ETHCAM_CMD_TIMEOUT));
		}
		#else
		DBG_IND(" %s %d\r\n", bufArry, sendlen);
		result = EthCamCmd_Send(path_id, (char*)bufArry, (int *)&len);
		if (sendlen != len) {
			result = E_SYS;
			DBG_ERR("sent %d error,should %d\r\n", len, sendlen);
		}
		#endif
	} else {
		DBG_ERR("len %d err\r\n", len);
		result = E_SYS;
	}

	return result;

}

void XML_ListResult(UINT32 cmd,  UINT32* list_value, UINT32 list_total_cnt ,HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType)
{
	char  *buf = (char *)bufAddr;
	UINT32 xmlBufSize = *bufSize;
	CHAR list_name[30] ={0};
	UINT32 i;

	XML_snprintf(&buf, &xmlBufSize, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n<LIST>\n");
	XML_snprintf(&buf, &xmlBufSize, "<Cmd>%d</Cmd>\n", cmd);

	for(i=0;i<list_total_cnt;i++){
		sprintf(list_name,"<ITEM>%d</ITEM>\n",list_value[i]);
		DBG_IND("i=%d, list_name=%s\r\n",i,list_name);
		XML_snprintf(&buf, &xmlBufSize, list_name);
	}

	XML_snprintf(&buf, &xmlBufSize, "</LIST>\n");
	*bufSize = (HFS_U32)(buf) - bufAddr;
}
INT32 EthCamApp_CheckIfEthCamMode(void)
{
#if (defined(_NVT_ETHREARCAM_TX_) || defined(_NVT_ETHREARCAM_RX_))
	return TRUE;
#else
	return FALSE;
#endif
}

void EthCamCmd_Init(void)
{
	EthCamCmd_SetExecTable(Cmd_ethcam);
	EthCamCmd_SetResultTable(EthCamXMLResultTbl);

	EthCamCmd_SetDefautCB((UINT32)XML_DefaultFormat);
	EthCamCmd_SetEventHandle((UINT32)Ux_PostEvent);
	EthCamCmd_SetAppStartupChecker((UINT32)EthCamApp_CheckIfEthCamMode);

	EthCamCmd_ReceiveCmd(TRUE);

 ///---->only test
 #ifdef MIO_LCA_TEST
	EthCamRX_para_init(TRUE);
 #endif
}
//XML parser
int EthCamCmdXML_skip_first_line(char *xml_buf){

    ////start code
    int xml_offset=0;

    if(*xml_buf == '<' &&
       *(xml_buf+1) == '?' &&
       *(xml_buf+2) == 'x' &&
       *(xml_buf+3) == 'm' &&
       *(xml_buf+4) == 'l'
       ){
        xml_offset ++;
        while(1){
            if(*(xml_buf+xml_offset)=='>'){
                xml_offset ++;
                break;
            }
            xml_offset ++;
        }
    }
    else{
        DBG_ERR("xml parser error, first char not '<' (%s) \r\n",xml_buf);
        return -1;
    }
    return xml_offset;
}

int EthCamCmdXML_remove_escape(char *xml_buf){

    int offset=0;
    while (1){
        if(*(xml_buf+offset)==0x0d || *(xml_buf+offset)==0x0a || *(xml_buf+offset)==' '){
            offset++;
            continue;
        }
        break;
    }
     return offset;
}
UINT32 EthCamCmdXML_get_tag_name(char *xml_buf, char *output){

    UINT32 offset=0;

    while(1){
        if(*(xml_buf+offset)!='>'){
            offset++;
            if(strlen(xml_buf) < offset){
                DBG_ERR("get not find tag '>' error! xml_buf=%s\r\n",xml_buf);
                return -1;
            }
            continue;
        }
        break;
    }
    if(*xml_buf == '<' && *(xml_buf+1) == '/'){
        strncpy(output,xml_buf+2,offset-2);
        output[offset -2]='\0';

    }
    else if(*xml_buf == '<'){
        strncpy(output,xml_buf+1,offset-1);
        output[offset -1]='\0';
    }
    else{
       DBG_ERR("parsing tag name error first buf=%x xml_buf=%s\r\n",*xml_buf,xml_buf);
       return -1;
    }
    if(strstr(output," ")){
       //////there is a attribute, need to_do!!!
    }
    offset++; ///skip '>'
    return offset;
}

UINT32 EthCamCmdXML_get_tag_value(char *xml_buf, char *output){

    UINT32 offset=0;
    while(1){
        if(*(xml_buf+offset)!='<'){
            offset++;
            if(strlen(xml_buf) < offset){
                DBG_ERR("get not find tag '<' error! xml_buf=%s\r\n",xml_buf);
                return -1;
            }
            continue;
        }
        break;
    }
    strncpy(output,xml_buf,offset);
    output[offset]='\0';

    return offset;
}
INT32 EthCamCmdXML_parsing_leaf(char *xml_buf, char *name, char *value){

    INT32 offset=0;
    INT32 tmp_offset=0;
    tmp_offset = EthCamCmdXML_get_tag_value(xml_buf, value);
    if(tmp_offset< 0){
        DBG_ERR("EthCamCmdXML_get_tag_value error  tmp_offset=%d\r\n",tmp_offset);
        return -1;
    }
    offset += tmp_offset;
    tmp_offset = EthCamCmdXML_get_tag_name(xml_buf+offset,name);//get end flag
    if(tmp_offset< 0){
        DBG_ERR("EthCamCmdXML_get_tag_name error  tmp_offset=%d\r\n",tmp_offset);
        return -1;
    }
    offset += tmp_offset;
    tmp_offset = EthCamCmdXML_remove_escape(xml_buf+offset);// remove \r \n
    if(tmp_offset< 0){
        DBG_ERR("EthCamCmdXML_remove_escape error  tmp_offset=%d\r\n",tmp_offset);
        return -1;
    }
    offset += tmp_offset;
    return offset;
}
//typedef enum _ETHCAM_CMD_ID{
//    ETHCAM_CMD_9001      = 9001,
//    ETHCAM_CMD_9002      = 9002,
//    ETHCAM_CMD_9003      = 9003,
//}ETHCAM_CMD_ID;

INT32 EthCamCmdXML_parsing_default_format(char *xml_buf, void * output)
{
    char current_name[64]={0};
    char current_value[128]={0};
    INT32 offset =0;
    INT32 tmp_offset=0;
    INT32 function_flag=0;
    INT32 pass_flag=0;

    //ETHCAM_XML_DEFAULT_FORMAT *output_data = (ETHCAM_XML_DEFAULT_FORMAT *)output;
    ETHCAM_XML_DEFAULT_FORMAT output_data={0};
    ETHCAM_XML_CB_REGISTER *reg = (ETHCAM_XML_CB_REGISTER *)output;
    memset(&output_data, 0, sizeof(ETHCAM_XML_DEFAULT_FORMAT));
    if(reg){
        output_data.path_id=reg->path_id;
        output_data.port_type=reg->port_type;
    }


    tmp_offset = EthCamCmdXML_get_tag_name(xml_buf+offset,current_name);

    if(tmp_offset < 0){
        DBG_ERR("get_tag_name error offset=%d\r\n",offset);
        return -1;
    }

    offset += tmp_offset;

    while(1){
        if(!strcmp(current_name, "Function")){
            function_flag++;
            if(function_flag > 1){
                /////xml parsing finish
                break;
            }
            tmp_offset = EthCamCmdXML_remove_escape(xml_buf+offset);// remove \r \n
            if(tmp_offset< 0){
                DBG_ERR("EthCamCmdXML_remove_escape error Function tmp_offset=%d\r\n",tmp_offset);
                return -1;
            }
            offset += tmp_offset;
        }
        else if(!strcmp(current_name,"Cmd")){
            tmp_offset = EthCamCmdXML_parsing_leaf(xml_buf+ offset, current_name, current_value);
            if(tmp_offset<0){
                DBG_ERR("EthCamCmdXML_parsing_leaf error tmp_offset=%d\r\n",tmp_offset);
                return -1;
            }

            offset += tmp_offset;
            output_data.cmd = atoi(current_value);
        }
        else if(!strcmp(current_name,"Status")){
            tmp_offset = EthCamCmdXML_parsing_leaf(xml_buf+ offset, current_name, current_value);
            if(tmp_offset<0){
                DBG_ERR("EthCamCmdXML_parsing_leaf error tmp_offset=%d\r\n",tmp_offset);
                return -1;
            }

            offset += tmp_offset;
            output_data.status = atoi(current_value);
            g_SocketCmd_Status[reg->path_id]=output_data.status;
            if(pass_flag==0){
                pass_flag=1;
            }
            else{
                DBG_ERR("EthCamCmdXML pass_flag error pass_flag=%d\r\n",pass_flag);
                return -1;
            }
        }
        else{
            DBG_ERR("tag name not support (%s)\r\n",current_name);
            return -1;
        }
        tmp_offset = EthCamCmdXML_get_tag_name(xml_buf+offset,current_name);
        if(tmp_offset< 0){
            DBG_ERR("EthCamCmdXML_get_tag_name error Function tmp_offset=%d\r\n",tmp_offset);
            return -1;
        }
        offset += tmp_offset;
        if(pass_flag == 1){
            ////get data finish , send data to CB function
            ////next current_name is List, it is the last data
            if(!strcmp(current_name,"Function")){
                if(reg && reg->EthCamXML_data_CB){
	                reg->EthCamXML_data_CB(1,(void *)&output_data);
                }
            }
            else{
                if(reg && reg->EthCamXML_data_CB){
	                reg->EthCamXML_data_CB(0,(void *)&output_data);
                }
            }
            pass_flag = 0;
            DBG_IND("cmd=%d, status=%d\r\n",output_data.cmd, output_data.status);
            memset(&output_data, 0, sizeof(output_data));
        }
    }
    return output_data.status;
}


INT32 EthCamCmdXML_parsing_value_result(char *xml_buf, void * output)
{
    char current_name[64]={0};
    char current_value[128]={0};
    INT32 offset =0;
    INT32 tmp_offset=0;
    INT32 function_flag=0;
    INT32 pass_flag=0;
    //ETHCAM_XML_VALUE_RESULT *output_data = (ETHCAM_XML_VALUE_RESULT *)output;
    ETHCAM_XML_VALUE_RESULT output_data={0};
    ETHCAM_XML_CB_REGISTER *reg = (ETHCAM_XML_CB_REGISTER *)output;
    memset(&output_data, 0, sizeof(ETHCAM_XML_VALUE_RESULT));
    if(reg){
        output_data.path_id=reg->path_id;
        output_data.port_type=reg->port_type;
    }

    tmp_offset = EthCamCmdXML_get_tag_name(xml_buf+offset,current_name);

    if(tmp_offset < 0){
        DBG_ERR("get_tag_name error offset=%d\r\n",offset);
        return -1;
    }

    offset += tmp_offset;

    while(1){
        if(!strcmp(current_name, "Function")){
            function_flag++;
            if(function_flag > 1){
                /////xml parsing finish
                break;
            }
            tmp_offset = EthCamCmdXML_remove_escape(xml_buf+offset);// remove \r \n
            if(tmp_offset< 0){
                DBG_ERR("EthCamCmdXML_remove_escape error Function tmp_offset=%d\r\n",tmp_offset);
                return -1;
            }
            offset += tmp_offset;
        }
        else if(!strcmp(current_name,"Cmd")){
            tmp_offset = EthCamCmdXML_parsing_leaf(xml_buf+ offset, current_name, current_value);
            if(tmp_offset<0){
                DBG_ERR("EthCamCmdXML_parsing_leaf error tmp_offset=%d\r\n",tmp_offset);
                return -1;
            }
            offset += tmp_offset;
            output_data.cmd = atoi(current_value);
        }
        else if(!strcmp(current_name,"Status")){
            tmp_offset = EthCamCmdXML_parsing_leaf(xml_buf+ offset, current_name, current_value);
            if(tmp_offset<0){
                DBG_ERR("EthCamCmdXML_parsing_leaf error tmp_offset=%d\r\n",tmp_offset);
                return -1;
            }
            offset += tmp_offset;
            output_data.status = atoi(current_value);
            g_SocketCmd_Status[reg->path_id]=output_data.status;
        }
        else if(!strcmp(current_name,"Value")){
            tmp_offset = EthCamCmdXML_parsing_leaf(xml_buf+ offset, current_name, current_value);
            if(tmp_offset<0){
                DBG_ERR("EthCamCmdXML_parsing_leaf error tmp_offset=%d\r\n",tmp_offset);
                return -1;
            }
            offset += tmp_offset;
            output_data.value= atoi(current_value);
            if(pass_flag==0){
                pass_flag=1;
            }
            else{
                DBG_ERR("EthCamCmdXML pass_flag error pass_flag=%d\r\n",pass_flag);
                return -1;
            }
        }
        else{
            DBG_ERR("tag name not support (%s)\r\n",current_name);
            return -1;
        }
        tmp_offset = EthCamCmdXML_get_tag_name(xml_buf+offset,current_name);
        if(tmp_offset< 0){
            DBG_ERR("EthCamCmdXML_get_tag_name error Function tmp_offset=%d\r\n",tmp_offset);
            return -1;
        }
        offset += tmp_offset;
        if(pass_flag == 1){
            ////get data finish , send data to CB function
            ////next current_name is List, it is the last data
            if(!strcmp(current_name,"Function")){
                if(reg && reg->EthCamXML_data_CB){
                	reg->EthCamXML_data_CB(1,(void *)&output_data);
                }
            }
            else{
                if(reg && reg->EthCamXML_data_CB){
                	reg->EthCamXML_data_CB(0,(void *)&output_data);
                }
            }
            pass_flag = 0;
            DBG_IND("cmd=%d, status=%d, value=%d\r\n",output_data.cmd, output_data.status,output_data.value);
            memset(&output_data, 0, sizeof(output_data));
        }
    }
    return 0;
}
INT32 EthCamCmdXML_parsing_string_result(char *xml_buf, void * output)
{
    char current_name[64]={0};
    char current_value[128]={0};
    INT32 offset =0;
    INT32 tmp_offset=0;
    INT32 function_flag=0;
    INT32 pass_flag=0;
    //ETHCAM_XML_VALUE_RESULT *output_data = (ETHCAM_XML_VALUE_RESULT *)output;
    ETHCAM_XML_STRING_RESULT output_data={0};
    ETHCAM_XML_CB_REGISTER *reg = (ETHCAM_XML_CB_REGISTER *)output;
    memset(&output_data, 0, sizeof(ETHCAM_XML_STRING_RESULT));
    if(reg){
        output_data.path_id=reg->path_id;
        output_data.port_type=reg->port_type;
    }

    tmp_offset = EthCamCmdXML_get_tag_name(xml_buf+offset,current_name);

    if(tmp_offset < 0){
        DBG_ERR("get_tag_name error offset=%d\r\n",offset);
        return -1;
    }

    offset += tmp_offset;

    while(1){
        if(!strcmp(current_name, "Function")){
            function_flag++;
            if(function_flag > 1){
                /////xml parsing finish
                break;
            }
            tmp_offset = EthCamCmdXML_remove_escape(xml_buf+offset);// remove \r \n
            if(tmp_offset< 0){
                DBG_ERR("EthCamCmdXML_remove_escape error Function tmp_offset=%d\r\n",tmp_offset);
                return -1;
            }
            offset += tmp_offset;
        }
        else if(!strcmp(current_name,"Cmd")){
            tmp_offset = EthCamCmdXML_parsing_leaf(xml_buf+ offset, current_name, current_value);
            if(tmp_offset<0){
                DBG_ERR("EthCamCmdXML_parsing_leaf error tmp_offset=%d\r\n",tmp_offset);
                return -1;
            }
            offset += tmp_offset;
            output_data.cmd = atoi(current_value);
        }
        else if(!strcmp(current_name,"Status")){
            tmp_offset = EthCamCmdXML_parsing_leaf(xml_buf+ offset, current_name, current_value);
            if(tmp_offset<0){
                DBG_ERR("EthCamCmdXML_parsing_leaf error tmp_offset=%d\r\n",tmp_offset);
                return -1;
            }
            offset += tmp_offset;
            output_data.status = atoi(current_value);
            g_SocketCmd_Status[reg->path_id]=output_data.status;
        }
        else if(!strcmp(current_name,"String")){
            tmp_offset = EthCamCmdXML_parsing_leaf(xml_buf+ offset, current_name, current_value);
            if(tmp_offset<0){
                DBG_ERR("EthCamCmdXML_parsing_leaf error tmp_offset=%d\r\n",tmp_offset);
                return -1;
            }
            offset += tmp_offset;
            strncpy(output_data.string,current_value,sizeof(output_data.string));
            output_data.string[sizeof(output_data.string)-1]='\0';
            if(pass_flag==0){
                pass_flag=1;
            }
            else{
                DBG_ERR("EthCamCmdXML pass_flag error pass_flag=%d\r\n",pass_flag);
                return -1;
            }
        }
        else{
            DBG_ERR("tag name not support (%s)\r\n",current_name);
            return -1;
        }
        tmp_offset = EthCamCmdXML_get_tag_name(xml_buf+offset,current_name);
        if(tmp_offset< 0){
            DBG_ERR("EthCamCmdXML_get_tag_name error Function tmp_offset=%d\r\n",tmp_offset);
            return -1;
        }
        offset += tmp_offset;
        if(pass_flag == 1){
            ////get data finish , send data to CB function
            ////next current_name is List, it is the last data
            if(!strcmp(current_name,"Function")){
                if(reg && reg->EthCamXML_data_CB){
                	reg->EthCamXML_data_CB(1,(void *)&output_data);
                }
            }
            else{
                if(reg && reg->EthCamXML_data_CB){
                	reg->EthCamXML_data_CB(0,(void *)&output_data);
                }
            }
            pass_flag = 0;
            DBG_IND("cmd=%d, status=%d, string=%s\r\n",output_data.cmd, output_data.status,output_data.string);
            memset(&output_data, 0, sizeof(output_data));
        }
    }
    return 0;
}
static INT32 EthCamCmdXML_parsing_list(char *xml_buf, void * reg_data){

    char current_name[64]={0};
    char current_value[128]={0};
    INT32 offset =0;
    INT32 tmp_offset=0;
    INT32 list_flag=0;
    INT32 total_count=0;

    ETHCAM_XML_CB_REGISTER *reg = (ETHCAM_XML_CB_REGISTER *)reg_data;
    ETHCAM_XML_LIST output_data={0};
    if(reg){
        output_data.path_id=reg->path_id;
        output_data.port_type=reg->port_type;
    }

    tmp_offset = EthCamCmdXML_get_tag_name(xml_buf+offset,current_name);
    if(tmp_offset < 0){
        DBG_ERR("get_tag_name error offset=%d\r\n",offset);
        return -1;
    }
    offset += tmp_offset;
    while(1){
        if(!strcmp(current_name, "LIST")){
            list_flag++;
            if(list_flag > 1){
                /////xml parsing finish
                break;
            }
            tmp_offset = EthCamCmdXML_remove_escape(xml_buf+offset);// remove \r \n
            if(tmp_offset< 0){
                DBG_ERR("EthCamCmdXML_remove_escape error Function tmp_offset=%d\r\n",tmp_offset);
                return -1;
            }
            offset += tmp_offset;
        }
        else if(!strcmp(current_name,"Cmd")){
            tmp_offset = EthCamCmdXML_parsing_leaf(xml_buf+ offset, current_name, current_value);
            if(tmp_offset<0){
                DBG_ERR("EthCamCmdXML_parsing_leaf error tmp_offset=%d\r\n",tmp_offset);
                return -1;
            }
            offset += tmp_offset;
        }
        else if(!strcmp(current_name, "ITEM")){
            tmp_offset = EthCamCmdXML_parsing_leaf(xml_buf+ offset, current_name, current_value);
            if(tmp_offset< 0){
                DBG_ERR("EthCamCmdXML_remove_escape error Function tmp_offset=%d\r\n",tmp_offset);
                return -1;
            }
            offset += tmp_offset;
            output_data.item[total_count] = atoi(current_value);
            DBG_IND("item=%d, total_count=%d\r\n",output_data.item[total_count],total_count);
            total_count++;
            output_data.total_item_cnt=total_count;
        }
        else{
            DBG_ERR("tag name not support (%s)\r\n",current_name);
            return -1;
        }
        tmp_offset = EthCamCmdXML_get_tag_name(xml_buf+offset,current_name);
        if(tmp_offset< 0){
            DBG_ERR("EthCamCmdXML_get_tag_name error Function tmp_offset=%d\r\n",tmp_offset);
            return -1;
        }
        offset += tmp_offset;
    }
    if(list_flag > 1){
        ////get data finish , send data to CB function
        ////next current_name is List, it is the last data
        if(!strcmp(current_name,"LIST")){
            if(reg && reg->EthCamXML_data_CB){
                reg->EthCamXML_data_CB(1,(void *)&output_data);
            }
        }
        else{
            if(reg && reg->EthCamXML_data_CB){
                reg->EthCamXML_data_CB(0,(void *)&output_data);
            }
        }
        list_flag = 0;
        memset(&output_data, 0, sizeof(output_data));
    }

    return 0;
}
INT32 EthCamCmdXML_GetCmdId(char *xml_buf)
{
    if(strlen(xml_buf)<=0){
        DBG_ERR("EthCam xml buf len error len=%d\r\n",strlen(xml_buf));
        return -1;
    }
    INT32 offset=0;
    INT32 tmp_offset=0;
    tmp_offset = EthCamCmdXML_skip_first_line(xml_buf);
    if(tmp_offset < 0){
        DBG_ERR("skip_first_line parsing error offset=%d\r\n",offset);
        return -1;
    }
    offset = offset + tmp_offset;
    tmp_offset = EthCamCmdXML_remove_escape(xml_buf+offset);
    if(tmp_offset < 0){
        DBG_ERR("EthCamCmdXML_remove_escape parsing error offset=%d\r\n",offset);
        return -1;
    }
    offset = offset + tmp_offset;


    char current_name[64]={0};
    char current_value[128]={0};
    INT32 function_flag=0;

    xml_buf=xml_buf+offset;
    tmp_offset = EthCamCmdXML_get_tag_name(xml_buf,current_name);

    if(tmp_offset < 0){
        DBG_ERR("get_tag_name error offset=%d\r\n",offset);
        return -1;
    }

    offset =0;
    offset += tmp_offset;
    while(1){
        if(!strcmp(current_name, "Function")){
            function_flag++;
            if(function_flag > 1){
                /////xml parsing finish
                break;
            }
            tmp_offset = EthCamCmdXML_remove_escape(xml_buf+offset);// remove \r \n
            if(tmp_offset< 0){
                DBG_ERR("EthCamCmdXML_remove_escape error Function tmp_offset=%d\r\n",tmp_offset);
                return -1;
            }
            offset += tmp_offset;
        }
        else if(!strcmp(current_name, "LIST")){
            function_flag++;
            if(function_flag > 1){
                /////xml parsing finish
                break;
            }
            tmp_offset = EthCamCmdXML_remove_escape(xml_buf+offset);// remove \r \n
            if(tmp_offset< 0){
                DBG_ERR("EthCamCmdXML_remove_escape error Function tmp_offset=%d\r\n",tmp_offset);
                return -1;
            }
            offset += tmp_offset;
        }
        else if(!strcmp(current_name,"Cmd")){
            tmp_offset = EthCamCmdXML_parsing_leaf(xml_buf+ offset, current_name, current_value);
            if(tmp_offset<0){
                DBG_ERR("EthCamCmdXML_parsing_leaf error tmp_offset=%d\r\n",tmp_offset);
                return -1;
            }
            offset += tmp_offset;
            //output_data->cmd = atoi(current_value);
            return atoi(current_value);
        }
        else if(!strcmp(current_name,"Status")){
            tmp_offset = EthCamCmdXML_parsing_leaf(xml_buf+ offset, current_name, current_value);
            if(tmp_offset<0){
                DBG_ERR("EthCamCmdXML_parsing_leaf error tmp_offset=%d\r\n",tmp_offset);
                return -1;
            }
            offset += tmp_offset;
            //output_data->status = atoi(current_value);
            DBG_DUMP("status=%d\r\n",atoi(current_value));
        }
        else{
            DBG_ERR("tag name not support (%s)\r\n",current_name);
            return -1;
        }
        tmp_offset = EthCamCmdXML_get_tag_name(xml_buf+offset,current_name);
        if(tmp_offset< 0){
            DBG_ERR("EthCamCmdXML_get_tag_name error Function tmp_offset=%d\r\n",tmp_offset);
            return -1;
        }
        offset += tmp_offset;
    }

    return -1;
}
INT32 EthCamCmdXML_parser(INT32 cmd_id,char *xml_buf ,void* output)
{
    if(cmd_id < 0){
        DBG_ERR("EthCam cmd_id error cmd_id=%d \r\n",cmd_id);
        return -1;
    }
    if(strlen(xml_buf)<=0){
        DBG_ERR("EthCam xml buf len error len=%d\r\n",strlen(xml_buf));
        return -1;
    }
    INT32 offset=0;
    INT32 tmp_offset=0;
    tmp_offset = EthCamCmdXML_skip_first_line(xml_buf);
    if(tmp_offset < 0){
        DBG_ERR("skip_first_line parsing error offset=%d\r\n",offset);
        return -1;
    }
    offset = offset + tmp_offset;
    tmp_offset = EthCamCmdXML_remove_escape(xml_buf+offset);
    if(tmp_offset < 0){
        DBG_ERR("EthCamCmdXML_remove_escape parsing error offset=%d\r\n",offset);
        return -1;
    }

    offset = offset + tmp_offset;

    INT32 i = 0;
    UINT32 result_type=0;
    ETHCAM_XML_RESULT *EthCamXMLResultTbl=EthCamCmd_GetResultTable();
    while (EthCamXMLResultTbl[i].cmd != 0) {
        if (cmd_id == EthCamXMLResultTbl[i].cmd) {
            result_type=EthCamXMLResultTbl[i].result_type;
            break;
        }
        i++;
    }
    DBG_IND("result_type=%d\r\n",result_type);

    INT32 ret =0;
    //switch (cmd_id){
    switch (result_type){

        //case ETHCAM_CMD_STARTUP:
        case ETHCAM_XML_RESULT_TYPE_DEFAULT_FORMAT:
            ret = EthCamCmdXML_parsing_default_format(xml_buf+offset, output);
            if(ret < 0){
                DBG_ERR("DEFAULT_FORMAT %d error ret =%d\r\n",cmd_id,ret);
            }
            break;
        case ETHCAM_XML_RESULT_TYPE_VALUE_RESULT:
            ret = EthCamCmdXML_parsing_value_result(xml_buf+offset, output);
            if(ret < 0){
                DBG_ERR("VALUE_RESULT %d error ret =%d\r\n",cmd_id,ret);
            }
            break;
        case ETHCAM_XML_RESULT_TYPE_LIST:
            ret = EthCamCmdXML_parsing_list(xml_buf+offset, output);
            if(ret < 0){
                DBG_ERR("LIST %d error ret =%d\r\n",cmd_id,ret);
            }
            break;
        case ETHCAM_XML_RESULT_TYPE_STRING_RESULT:
            ret = EthCamCmdXML_parsing_string_result(xml_buf+offset, output);
            if(ret < 0){
                DBG_ERR("STRING_RESULT %d error ret =%d\r\n",cmd_id,ret);
            }
            break;
        default:
            DBG_ERR("--- cj ---EthCam cmd %d  %x not support!\r\n",cmd_id, cmd_id);
            ret=ETHCAM_RET_CMD_NOT_FOUND;
            return ret;
    }
    if(ret < 0){
        DBG_DUMP("xml_buf=%s\r\n",xml_buf);
    }
    return ret;
}

void EthCam_GetDest(char *path,  ETHCAM_PATH_ID *path_id, ETHCAM_PORT_TYPE *port_type)
{
	char dest_ip[16];
	UINT32 dest_port = 80;
	UINT i ,j;
	//CHAR* ip_base[ETHCAM_PATH_ID_MAX]={gEthCamTxIP[0].ip,gEthCamTxIP[1].ip};
	CHAR* ip_base[ETHCAM_PATH_ID_MAX]={SocketInfo[0].ip,SocketInfo[1].ip};

	//UINT32 port_base[ETHCAM_PATH_ID_MAX][ETHCAM_PORT_DATA_MAX]={ETH_CMD_SOCKET_PORT, ETH_DATA1_SOCKET_PORT, ETH_DATA2_SOCKET_PORT};
	UINT32 port_base[ETHCAM_PATH_ID_MAX][ETHCAM_PORT_DATA_MAX]={{SocketInfo[0].port[2], SocketInfo[0].port[0], SocketInfo[0].port[1]},
																{SocketInfo[1].port[2], SocketInfo[1].port[0], SocketInfo[1].port[1]}};

	UINT32 port_type_base[ETHCAM_PATH_ID_MAX][ETHCAM_PORT_DATA_MAX]={{ETHCAM_PORT_DEFAULT, ETHCAM_PORT_DATA1, ETHCAM_PORT_DATA2},
																{ETHCAM_PORT_DEFAULT, ETHCAM_PORT_DATA1, ETHCAM_PORT_DATA2}};


	sscanf(path, "%15[^:]:%99d", dest_ip, &dest_port);
	DBG_IND("dest_ip=%s, dest_port=%d\r\n",dest_ip,dest_port);

	if(strcmp(dest_ip, ip_base[0])==0){
		*path_id=ETHCAM_PATH_ID_1;
	}else{
		*path_id=ETHCAM_PATH_ID_2;
	}
	for(i=0;i<ETHCAM_PATH_ID_MAX;i++){
		for(j=0;j<ETHCAM_PORT_DATA_MAX;j++){
			if(dest_port==port_base[i][j]){
				*port_type=port_type_base[i][j];
				break;
			}
		}
	}
}

int EthCam_STARTUP(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount)
{
	XML_DefaultFormat(ETHCAM_CMD_STARTUP, ETHCAM_RET_OK, bufAddr, bufSize, mimeType);
	return ETHCAM_CMD_GETDATA_RETURN_OK;
}

int EthCam_GetMovieTBR(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount)
{
#if(defined(_NVT_ETHREARCAM_TX_))
	ETHCAM_PATH_ID path_id;
	ETHCAM_PORT_TYPE port_type;
	EthCam_GetDest(path, &path_id, &port_type);
	DBG_DUMP("path_id=%d, port_type=%d\r\n",path_id,port_type);

	if(port_type==ETHCAM_PORT_DATA1){
		XML_ValueResult(ETHCAM_CMD_GET_TX_MOVIE_TBR, MovieExe_GetTBR(_CFG_REC_ID_1), bufAddr, bufSize, mimeType);
	}else{
		XML_ValueResult(ETHCAM_CMD_GET_TX_MOVIE_TBR, MovieExe_GetTBR(_CFG_CLONE_ID_1), bufAddr, bufSize, mimeType);
	}
#endif
	return ETHCAM_CMD_GETDATA_RETURN_OK;
}
int EthCam_GetMovieFps(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount)
{
#if(defined(_NVT_ETHREARCAM_TX_))
	ETHCAM_PATH_ID path_id;
	ETHCAM_PORT_TYPE port_type;
	EthCam_GetDest(path, &path_id, &port_type);
	if(port_type==ETHCAM_PORT_DATA1){
		XML_ValueResult(ETHCAM_CMD_GET_TX_MOVIE_FPS, MovieExe_GetFps(_CFG_REC_ID_1), bufAddr, bufSize, mimeType);
	}else{
		XML_ValueResult(ETHCAM_CMD_GET_TX_MOVIE_FPS, MovieExe_GetFps(_CFG_CLONE_ID_1), bufAddr, bufSize, mimeType);
	}
#endif
	return ETHCAM_CMD_GETDATA_RETURN_OK;
}
int EthCam_GetMovieCodec(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount)
{
#if(defined(_NVT_ETHREARCAM_TX_))
	ETHCAM_PATH_ID path_id;
	ETHCAM_PORT_TYPE port_type;
	EthCam_GetDest(path, &path_id, &port_type);
	if(port_type==ETHCAM_PORT_DATA1){
		XML_ValueResult(ETHCAM_CMD_GET_TX_MOVIE_CODEC, MovieExe_GetCodec(_CFG_REC_ID_1), bufAddr, bufSize, mimeType);
	}else{
		XML_ValueResult(ETHCAM_CMD_GET_TX_MOVIE_CODEC, MovieExe_GetCodec(_CFG_CLONE_ID_1), bufAddr, bufSize, mimeType);
	}
#endif
	return ETHCAM_CMD_GETDATA_RETURN_OK;
}

int EthCam_GetMovieRecSize(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount)
{
#if(defined(_NVT_ETHREARCAM_TX_))
	ETHCAM_PATH_ID path_id;
	ETHCAM_PORT_TYPE port_type;
	EthCam_GetDest(path, &path_id, &port_type);
	UINT32 list_value[2]={0};

	if(port_type==ETHCAM_PORT_DATA1){
		list_value[0]=MovieExe_GetWidth(_CFG_REC_ID_1);
		list_value[1]=MovieExe_GetHeight(_CFG_REC_ID_1);
	}else{
		list_value[0]=MovieExe_GetWidth(_CFG_CLONE_ID_1);
		list_value[1]=MovieExe_GetHeight(_CFG_CLONE_ID_1);
	}
	XML_ListResult(ETHCAM_CMD_GET_TX_MOVIE_REC_SIZE, list_value, sizeof(list_value)/sizeof(UINT32),  bufAddr, bufSize, mimeType);
#endif
	return ETHCAM_CMD_GETDATA_RETURN_OK;
}
int EthCam_GetMovieGop(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount)
{
#if(defined(_NVT_ETHREARCAM_TX_))
	ETHCAM_PATH_ID path_id;
	ETHCAM_PORT_TYPE port_type;
	EthCam_GetDest(path, &path_id, &port_type);
	if(port_type==ETHCAM_PORT_DATA1){
 		XML_ValueResult(ETHCAM_CMD_GET_TX_MOVIE_GOP, MovieExe_GetGOP(_CFG_REC_ID_1), bufAddr, bufSize, mimeType);
	}else{
		XML_ValueResult(ETHCAM_CMD_GET_TX_MOVIE_GOP, MovieExe_GetGOP(_CFG_CLONE_ID_1), bufAddr, bufSize, mimeType);
	}
#endif
	return ETHCAM_CMD_GETDATA_RETURN_OK;
}

int EthCam_GetMovieSPS_PPS(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount)
{
#if(defined(_NVT_ETHREARCAM_TX_))

	//UINT8 tem_buf[64];
	UINT8 tem_buf[128];
	MEM_RANGE desc;
	MEM_RANGE sps;
	MEM_RANGE pps;
	MEM_RANGE vps;
	ETHCAM_PATH_ID path_id;
	ETHCAM_PORT_TYPE port_type;
	UINT32 codec;
	EthCam_GetDest(path, &path_id, &port_type);

	desc.Addr=(UINT32)tem_buf;
	desc.Size=sizeof(tem_buf);
	codec= (sEthCamCodecSrctype.VCodec == MOVIE_CODEC_H265)?_CFG_CODEC_H265:_CFG_CODEC_H264;
	if(port_type==ETHCAM_PORT_DATA1){
		//ImageApp_MovieMulti_GetDesc(_CFG_REC_ID_1, _CFG_CODEC_H265, &desc, &sps, &pps, &vps);
		ImageApp_MovieMulti_GetDesc(_CFG_REC_ID_1, codec, &desc, &sps, &pps, &vps);
	}else{
		//ImageApp_MovieMulti_GetDesc(_CFG_CLONE_ID_1, _CFG_CODEC_H265, &desc, &sps, &pps, &vps);
		ImageApp_MovieMulti_GetDesc(_CFG_CLONE_ID_1, codec, &desc, &sps, &pps, &vps);
	}
	UINT32 list_value[desc.Size];
	DBG_IND("desc[%d]:\r\n",desc.Size);

	UINT8 *pBuf;
	pBuf=(UINT8 *)desc.Addr;

	UINT32 i;
	for(i=0;i<desc.Size;i++){
		list_value[i]=pBuf[i];
		DBG_IND("0x%x, ", pBuf[i]);
	}
	DBG_IND("\r\n");
#if 0

	DBG_DUMP("\r\nsps[%d]:\r\n",sps.Size);

	pBuf=(UINT8 *)sps.Addr;
	for(i=0;i<sps.Size;i++){
		DBG_DUMP("0x%x, ", pBuf[i]);
	}
	DBG_DUMP("\r\npps[%d]:\r\n",pps.Size);

	pBuf=(UINT8 *)pps.Addr;
	for(i=0;i<pps.Size;i++){
		DBG_DUMP("0x%x, ", pBuf[i]);
	}
	DBG_DUMP("\r\nvps[%d]:\r\n",vps.Size);
	pBuf=(UINT8 *)vps.Addr;
	for(i=0;i<vps.Size;i++){
		DBG_DUMP("0x%x, ", pBuf[i]);
	}
	DBG_DUMP("\r\n");
#endif
	XML_ListResult(ETHCAM_CMD_GET_TX_SPS_PPS, list_value, desc.Size,  bufAddr, bufSize, mimeType);
#endif
	//UINT32 list_value[]={0x0, 0x0, 0x0, 0x1, 0x67, 0x64, 0x0, 0x33, 0xac, 0x15, 0x4a, 0x7, 0x80, 0x22, 0x7e, 0x59, 0xb8, 0x8, 0x8, 0xa, 0x0, 0x0, 0x7, 0xd0, 0x0, 0x3, 0xa9, 0x80, 0x8,
	//			0x0, 0x0, 0x0, 0x1, 0x68, 0xee, 0x3c, 0xb0};

	//XML_ListResult(ETHCAM_CMD_GET_SPS_PPS, list_value, sizeof(list_value)/sizeof(UINT32),  bufAddr, bufSize, mimeType);
	return ETHCAM_CMD_GETDATA_RETURN_OK;
}
int EthCam_TxStreamStart(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount)
{
#if(defined(_NVT_ETHREARCAM_TX_))
	ETHCAM_PATH_ID path_id;
	ETHCAM_PORT_TYPE port_type;
	EthCam_GetDest(path, &path_id, &port_type);
	DBG_DUMP("TxStreamStart port\r\n",port_type);
	if(port_type==ETHCAM_PORT_DATA1){
		MovieExe_EthCamTxStart(_CFG_REC_ID_1);
	}else{
		MovieExe_EthCamTxStart(_CFG_CLONE_ID_1);
	}
	EthCamNet_SetTxHB(0);
	SxTimer_SetFuncActive(SX_TIMER_ETHCAM_LINKDET_ID, TRUE);

#endif
	XML_DefaultFormat(ETHCAM_CMD_TX_STREAM_START, ETHCAM_RET_OK, bufAddr, bufSize, mimeType);
	return ETHCAM_CMD_GETDATA_RETURN_OK;
}
int EthCam_TxStreamStop(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount)
{
#if(defined(_NVT_ETHREARCAM_TX_))
	ETHCAM_PATH_ID path_id;
	ETHCAM_PORT_TYPE port_type;
	char  *pch = 0;
	static UINT32 Par = 0;
	if ((pch = strchr(argument + strlen(CMD_STR), '&')) !=0) {
		if (strncmp(pch, PAR_STR, strlen(PAR_STR)) == 0) {
			sscanf_s(pch + strlen(PAR_STR), "%d", &Par);
		}
	}
	DBG_DUMP("TxStreamStop Par=%d\r\n", Par);
	if(Par){
		MovieExe_EthCamTxStop(_CFG_REC_ID_1);
		MovieExe_EthCamTxStop(_CFG_CLONE_ID_1);
	}else{
		EthCam_GetDest(path, &path_id, &port_type);
		if(port_type==ETHCAM_PORT_DATA1){
			MovieExe_EthCamTxStop(_CFG_REC_ID_1);
		}else{
			MovieExe_EthCamTxStop(_CFG_CLONE_ID_1);
		}
	}
#endif
	Delay_DelayMs(100);
	XML_DefaultFormat(ETHCAM_CMD_TX_STREAM_STOP, ETHCAM_RET_OK, bufAddr, bufSize, mimeType);
	return ETHCAM_CMD_GETDATA_RETURN_OK;
}

int EthCam_GetMovieThumb(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount)
{
#if(defined(_NVT_ETHREARCAM_TX_))
	ImageApp_MovieMulti_EthCam_TxTrigThumb((0 | ETHCAM_TX_MAGIC_KEY));
#endif
	XML_DefaultFormat(ETHCAM_CMD_GET_TX_MOVIE_THUMB, ETHCAM_RET_OK, bufAddr, bufSize, mimeType);
	return ETHCAM_CMD_GETDATA_RETURN_OK;
}
int EthCam_GetMovieRawEncode(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount)
{
#if(defined(_NVT_ETHREARCAM_TX_))
	if (System_GetState(SYS_STATE_CURRMODE) != PRIMARY_MODE_MOVIE) {
		XML_DefaultFormat(ETHCAM_CMD_GET_TX_MOVIE_RAW_ENCODE, ETHCAM_RET_ERR, bufAddr, bufSize, mimeType);
		DBG_ERR("not movie mode\r\n");
		return ETHCAM_CMD_GETDATA_RETURN_OK;
	}

	//680,510 media not record,also can raw encode
	if(ImageApp_MovieMulti_IsStreamRunning(_CFG_REC_ID_1 | ETHCAM_TX_MAGIC_KEY)){
		Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_MOVIE_REC_RAWENC, 0);
	} else {
		DBG_ERR("Not in recording state\r\n");

		XML_DefaultFormat(ETHCAM_CMD_GET_TX_MOVIE_RAW_ENCODE, ETHCAM_RET_ERR, bufAddr, bufSize, mimeType);
		return ETHCAM_CMD_GETDATA_RETURN_OK;
	}
#endif
	XML_DefaultFormat(ETHCAM_CMD_GET_TX_MOVIE_RAW_ENCODE, ETHCAM_RET_OK, bufAddr, bufSize, mimeType);
	return ETHCAM_CMD_GETDATA_RETURN_OK;
}
int EthCam_GetMovieRecInfo(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount)
{
#if(defined(_NVT_ETHREARCAM_TX_))
	MOVIE_RECODE_INFO MovieRecordInfo=MovieExe_GetRecInfo(_CFG_REC_ID_1);
	MOVIEMULTI_ETHCAM_REC_INFO EthCamRecInfo={0};
	EthCamRecInfo.width = MovieRecordInfo.size.w;
	EthCamRecInfo.height = MovieRecordInfo.size.h;
	EthCamRecInfo.vfr = MovieRecordInfo.frame_rate;
	EthCamRecInfo.tbr = MovieRecordInfo.target_bitrate;
	EthCamRecInfo.ar = MovieRecordInfo.dar;
	EthCamRecInfo.gop = MovieRecordInfo.cbr_info.uiGOP;
	EthCamRecInfo.codec = MovieRecordInfo.codec;
	EthCamRecInfo.aud_codec = MovieRecordInfo.aud_codec;
	EthCamRecInfo.rec_mode = MovieRecordInfo.rec_mode;
	EthCamRecInfo.rec_format = MovieRecordInfo.file_format;



	UINT32 list_value[sizeof(MOVIEMULTI_ETHCAM_REC_INFO)/sizeof(UINT32)];
	UINT32 list_total_cnt=sizeof(MOVIEMULTI_ETHCAM_REC_INFO)/sizeof(UINT32);
	UINT32 *pBuf;
	pBuf=(UINT32 *)&EthCamRecInfo;

	UINT32 i;
	DBG_DUMP("MovieRecordInfo=\r\n");
	for(i=0;i<list_total_cnt;i++){
		list_value[i]=pBuf[i];
		DBG_DUMP("0x%x, ", pBuf[i]);
	}
	DBG_DUMP("\r\n");
	XML_ListResult(ETHCAM_CMD_GET_TX_REC_INFO, list_value, list_total_cnt,  bufAddr, bufSize, mimeType);
#endif
	return ETHCAM_CMD_GETDATA_RETURN_OK;
}
int EthCam_GetMovieDecInfo(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount)
{
#if(defined(_NVT_ETHREARCAM_TX_))
	ETHCAM_TX_DEC_INFO MovieDecInfo={0};
	ETHCAM_PATH_ID path_id;
	ETHCAM_PORT_TYPE port_type;
	MOVIE_CFG_REC_ID RecId;
	EthCam_GetDest(path, &path_id, &port_type);
	if(port_type==ETHCAM_PORT_DATA1){
		RecId=_CFG_REC_ID_1;
	}else{
		RecId=_CFG_CLONE_ID_1;
	}
	MovieDecInfo.bStarupOK=0;
	#if (ETHCAM_TX_DISPLAY_FLIP == DISABLE)
	MovieDecInfo.Flip=0;
	#else
	MovieDecInfo.Flip=1;
	#endif
	MovieDecInfo.Tbr=MovieExe_GetTBR(RecId);
	MovieDecInfo.Fps=MovieExe_GetFps(RecId);
	MovieDecInfo.Width=MovieExe_GetWidth(RecId);
	MovieDecInfo.Height=MovieExe_GetHeight(RecId);
	MovieDecInfo.Gop=MovieExe_GetGOP(RecId);
	MovieDecInfo.Codec=MovieExe_GetCodec(RecId);

	UINT32 list_value[sizeof(ETHCAM_TX_DEC_INFO)/sizeof(UINT32)];
	UINT32 list_total_cnt=8;//sizeof(ETHCAM_TX_DEC_INFO)/sizeof(UINT32);
	UINT32 *pBuf;
	pBuf=(UINT32 *)&MovieDecInfo;

	UINT32 i;
	DBG_DUMP("MovieDecInfo=\r\n");
	for(i=0;i<list_total_cnt;i++){
		list_value[i]=pBuf[i];
		DBG_DUMP("0x%x, ", pBuf[i]);
	}
	DBG_DUMP("\r\n");
	XML_ListResult(ETHCAM_CMD_GET_TX_DEC_INFO, list_value, list_total_cnt,  bufAddr, bufSize, mimeType);
#endif
	return ETHCAM_CMD_GETDATA_RETURN_OK;
}
int EthCam_TxRtspStart(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount)
{
#if(defined(_NVT_ETHREARCAM_TX_))
	MsdcNvtCb_Net(TRUE);

	UINet_RtspInit();
	Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_MOVIE_STRM_START, 0);
#endif
	XML_DefaultFormat(ETHCAM_CMD_TX_RTSP_START, ETHCAM_RET_OK, bufAddr, bufSize, mimeType);
	return ETHCAM_CMD_GETDATA_RETURN_OK;
}
int EthCam_SyncTime(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount)
{
#if(defined(_NVT_ETHREARCAM_TX_))
	if (argument && segmentCount==0) {
		XML_DefaultFormat(ETHCAM_CMD_SYNC_TIME, ETHCAM_RET_CONTINUE, bufAddr, bufSize, mimeType);
		return ETHCAM_CMD_GETDATA_RETURN_CONTINUE;
	}else{
		struct tm Curr_DateTime;// =(struct tm)bufAddr;
		memcpy((UINT8*)&Curr_DateTime, (UINT8*)bufAddr, sizeof(struct tm));
		DBG_DUMP("year=%d,mon=%d,day=%d,hour=%d,min=%d,sec=%d\r\n", Curr_DateTime.tm_year,Curr_DateTime.tm_mon,Curr_DateTime.tm_mday,Curr_DateTime.tm_hour,Curr_DateTime.tm_min,Curr_DateTime.tm_sec);
		HwClock_SetTime(TIME_ID_CURRENT, Curr_DateTime, 0);
	}
#endif
	XML_DefaultFormat(ETHCAM_CMD_SYNC_TIME, ETHCAM_RET_OK, bufAddr, bufSize, mimeType);
	return ETHCAM_CMD_GETDATA_RETURN_OK;
}
int EthCam_SyncMenuSetting(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount)
{
#if(defined(_NVT_ETHREARCAM_TX_))
	char  *pch = 0;
	static UINT32 bChgMode = 0;
	if (argument && segmentCount==0) {
		if ((pch = strchr(argument + strlen(CMD_STR), '&')) !=0) {
			if (strncmp(pch, PAR_STR, strlen(PAR_STR)) == 0) {
				sscanf_s(pch + strlen(PAR_STR), "%d", &bChgMode);
			}
		}
		DBG_DUMP("bChgMode=%d\r\n", bChgMode);
		XML_DefaultFormat(ETHCAM_CMD_SYNC_MENU_SETTING, ETHCAM_RET_CONTINUE, bufAddr, bufSize, mimeType);
		return ETHCAM_CMD_GETDATA_RETURN_CONTINUE;
	}else{
		ETHCAM_MENU_SETTING sEthCamMenuSetting={0};
		BOOL bReOpen = 0;
		memcpy((UINT8*)&sEthCamMenuSetting, (UINT8*)bufAddr, sizeof(ETHCAM_MENU_SETTING));
		DBG_DUMP("1size=%d,WDR=%d,EV=%d,DateImprint=%d,SensorRotate=%d,Codec=%d,TimeLapse=%d\r\n", sEthCamMenuSetting.Size,sEthCamMenuSetting.WDR,sEthCamMenuSetting.EV,sEthCamMenuSetting.DateImprint,sEthCamMenuSetting.SensorRotate,sEthCamMenuSetting.Codec,sEthCamMenuSetting.TimeLapse);

		DBG_DUMP("2size=%d,WDR=%d,EV=%d,DateImprint=%d,SensorRotate=%d,Codec=%d,TimeLapse=%d\r\n", UI_GetData(FL_MOVIE_SIZE),UI_GetData(FL_MOVIE_WDR),UI_GetData(FL_EV),UI_GetData(FL_MOVIE_DATEIMPRINT),UI_GetData(FL_MOVIE_SENSOR_ROTATE),UI_GetData(FL_MOVIE_CODEC),UI_GetData(FL_MOVIE_TIMELAPSE_REC));
		#if 0
		if(sEthCamMenuSetting.Size != UI_GetData(FL_MOVIE_SIZE)){
			//UI_SetData(FL_MOVIE_SIZE, sEthCamMenuSetting.Size);
			bReOpen=1;
		}
		#endif
		if(sEthCamMenuSetting.WDR != UI_GetData(FL_MOVIE_WDR)){
			UI_SetData(FL_MOVIE_WDR, sEthCamMenuSetting.WDR);
			bReOpen=1;
		}
		if(sEthCamMenuSetting.EV != UI_GetData(FL_EV)){
			UI_SetData(FL_EV, sEthCamMenuSetting.EV);
			Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_MOVIE_EV, 1, UI_GetData(FL_EV));
		}
		if(sEthCamMenuSetting.DateImprint != UI_GetData(FL_MOVIE_DATEIMPRINT)){
			UI_SetData(FL_MOVIE_DATEIMPRINT, sEthCamMenuSetting.DateImprint);
			MovieExe_EthCamTxDateStampConfig();
		}
		if(sEthCamMenuSetting.SensorRotate != UI_GetData(FL_MOVIE_SENSOR_ROTATE)){
			UI_SetData(FL_MOVIE_SENSOR_ROTATE, sEthCamMenuSetting.SensorRotate);
			Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_MOVIE_SENSOR_ROTATE, 1, UI_GetData(FL_MOVIE_SENSOR_ROTATE));
			//ImageApp_MovieMulti_ResetIFrame(_CFG_REC_ID_1);
			//MovieExe_EthCamRecId1_ResetBsQ();
			//ImageApp_MovieMulti_ResetIFrame(_CFG_CLONE_ID_1);
			//MovieExe_EthCamCloneId1_ResetBsQ();
		}
		//if(sEthCamMenuSetting.Codec != UI_GetData(FL_MOVIE_CODEC)){
		if(sEthCamMenuSetting.Codec != sEthCamCodecSrctype.VCodec){
			UI_SetData(FL_MOVIE_CODEC, sEthCamMenuSetting.Codec);
			sEthCamCodecSrctype.VCodec=sEthCamMenuSetting.Codec;
			//Ux_SendEvent(&CustomMovieObjCtrl,   NVTEVT_EXE_MOVIE_CODEC,         1,  UI_GetData(FL_MOVIE_CODEC));
			bReOpen=1;
		}
		if(sEthCamMenuSetting.TimeLapse != UI_GetData(FL_MOVIE_TIMELAPSE_REC)){
			UI_SetData(FL_MOVIE_TIMELAPSE_REC, sEthCamMenuSetting.TimeLapse);
			bReOpen=1;
		}


		DBG_DUMP("bReOpen=%d, bChgMode=%d\r\n", bReOpen,bChgMode);

		XML_DefaultFormat(ETHCAM_CMD_SYNC_MENU_SETTING, ETHCAM_RET_OK, bufAddr, bufSize, mimeType);

		//if(bReOpen || bChgMode){
		if(bChgMode){
			Ux_PostEvent(NVTEVT_SYSTEM_MODE, 1, System_GetState(SYS_STATE_CURRMODE));
		}
	}
#endif
	//XML_DefaultFormat(ETHCAM_CMD_SYNC_MENU_SETTING, ETHCAM_RET_OK, bufAddr, bufSize, mimeType);
	return ETHCAM_CMD_GETDATA_RETURN_OK;
}
int EthCam_TxResetI(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount)
{
#if(defined(_NVT_ETHREARCAM_TX_))
	ETHCAM_PATH_ID path_id;
	ETHCAM_PORT_TYPE port_type;
	EthCam_GetDest(path, &path_id, &port_type);

	if(port_type==ETHCAM_PORT_DATA1){
		ImageApp_MovieMulti_ResetIFrame(_CFG_REC_ID_1);
	}else{
		ImageApp_MovieMulti_ResetIFrame(_CFG_CLONE_ID_1);
	}
#endif
	XML_DefaultFormat(ETHCAM_CMD_TX_RESET_I, ETHCAM_RET_OK, bufAddr, bufSize, mimeType);
	return ETHCAM_CMD_GETDATA_RETURN_OK;
}
int EthCam_TxResetQueue(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount)
{
#if(defined(_NVT_ETHREARCAM_TX_))

	MovieExe_EthCamRecId1_ResetBsQ();
	MovieExe_EthCamCloneId1_ResetBsQ();

#endif
	XML_DefaultFormat(ETHCAM_CMD_TX_RESET_QUEUE, ETHCAM_RET_OK, bufAddr, bufSize, mimeType);
	return ETHCAM_CMD_GETDATA_RETURN_OK;
}
int EthCam_TxPowerOff(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount)
{
#if(defined(_NVT_ETHREARCAM_TX_))

	Ux_PostEvent(NVTEVT_SYSTEM_SHUTDOWN, 1, 0);
#endif
	XML_DefaultFormat(ETHCAM_CMD_TX_POWEROFF, ETHCAM_RET_OK, bufAddr, bufSize, mimeType);
	return ETHCAM_CMD_GETDATA_RETURN_OK;
}
int EthCam_TxFWVersion(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount)
{
#if(defined(_NVT_ETHREARCAM_TX_))
	ETHCAM_PATH_ID path_id;
	ETHCAM_PORT_TYPE port_type;
	char FWVersion[ETHCAM_PATH_ID_MAX][30]={"2019-12-12-Ver1.00","2019-12-12-Ver1.02"};
	EthCam_GetDest(path, &path_id, &port_type);
	XML_StringResult(ETHCAM_CMD_TX_FW_VERSION, FWVersion[path_id], bufAddr, bufSize, mimeType);
#endif

	return ETHCAM_CMD_GETDATA_RETURN_OK;
}
int EthCam_SetTxIPReset(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount)
{
#if(defined(_NVT_ETHREARCAM_TX_))

	SysSetFlag(FL_ETHCAM_TX_IP_ADDR, 0);

#endif
	XML_DefaultFormat(ETHCAM_CMD_SET_TX_IP_RESET, ETHCAM_RET_OK, bufAddr, bufSize, mimeType);
	return ETHCAM_CMD_GETDATA_RETURN_OK;
}

ETHCAM_TX_SET_OFFESET SlideOffsetInfo;

int  EthCam_SetTxSlideUP(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount)
{
	UINT32 curDispOffset = 0;
	#if(defined(_NVT_ETHREARCAM_TX_))
	if (argument && segmentCount==0) {
		XML_DefaultFormat(ETHCAM_CMD_TX_SLIDEUP, ETHCAM_RET_CONTINUE, bufAddr, bufSize, mimeType);
		return ETHCAM_CMD_GETDATA_RETURN_CONTI_NEED_ACKDATA;
	}else{
		memcpy((UINT8*)&SlideOffsetInfo, (UINT8*)bufAddr, sizeof(ETHCAM_TX_SET_OFFESET));
		DBG_DUMP("curDispOffset=%d\r\n", SlideOffsetInfo.curDispOffset);
		if(SysGetFlag(FL_DUAL_CAM) == DUALCAM_FRONT)
	    	{
	            curDispOffset = SysGetFlag(FL_SENSOR1_DISP_OFFSET);
	            if(curDispOffset < (SEN_DISP_OFFSET_SETTING_MAX - 1))
	              curDispOffset ++;
	            SysSetFlag(FL_SENSOR1_DISP_OFFSET, curDispOffset);
	            
	            #if IMG_FULL_DISP
	            MovieExe_SetIMECrop(_CFG_REC_ID_1);
	            #endif  
	    	}
    		else
	    	{
				curDispOffset = SysGetFlag(FL_SENSOR2_DISP_OFFSET);
				if(curDispOffset < (SEN_DISP_OFFSET_SETTING_MAX - 1))
				  curDispOffset ++;
				SysSetFlag(FL_SENSOR2_DISP_OFFSET, curDispOffset);
				
	            #if IMG_FULL_DISP
	  			MovieExe_SetIMECrop(_CFG_REC_ID_2);
	            #endif	
	    	}
	}
	if (SlideOffsetInfo.curDispOffset >0 ){
		XML_ValueResult(ETHCAM_CMD_TX_SLIDEUP, sizeof(ETHCAM_TX_SET_OFFESET), bufAddr, bufSize, mimeType);
	}else{
		XML_ValueResult(ETHCAM_CMD_TX_SLIDEUP, 0, bufAddr, bufSize, mimeType);
	}
#endif
	//XML_DefaultFormat(ETHCAM_CMD_TX_SLIDEUP, ETHCAM_RET_OK, bufAddr, bufSize, mimeType);
	return ETHCAM_CMD_GETDATA_RETURN_OK;
}


int EthCam_SetTxSysInfo(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount)
{
//CHKPNT;
#if(defined(_NVT_ETHREARCAM_TX_))
	ETHCAM_TX_SYS_INFO SysInfo;
	static UINT32 VdoEncBufSec = 0;
	if (argument && segmentCount==0) {
		char  *pch = 0;
		if((pch = strchr(argument + strlen(CMD_STR), '&')) !=0) {
			if (strncmp(pch, PAR_STR, strlen(PAR_STR)) == 0) {
				sscanf_s(pch + strlen(PAR_STR), "%d", &VdoEncBufSec);
			}
		}
		DBG_DUMP("VdoEncBufSec=%d\r\n", VdoEncBufSec);
		//CHKPNT;
		XML_DefaultFormat(ETHCAM_CMD_SET_TX_SYS_INFO, ETHCAM_RET_CONTINUE, bufAddr, bufSize, mimeType);
		return ETHCAM_CMD_GETDATA_RETURN_CONTI_NEED_ACKDATA;
	}else{
	//CHKPNT;
		memcpy((UINT8*)&SysInfo, (UINT8*)bufAddr, sizeof(ETHCAM_TX_SYS_INFO));
		DBG_DUMP("PullModeEn=%d, CloneDisplayPathEn=%d\r\n", SysInfo.PullModeEn,SysInfo.CloneDisplayPathEn);
	}

	if((SysInfo.PullModeEn>=0 && SysInfo.PullModeEn<=1)
		&& (SysInfo.CloneDisplayPathEn>=0 && SysInfo.CloneDisplayPathEn<=1)
		){
		sEthCamSysInfo.PullModeEn=SysInfo.PullModeEn;
		sEthCamSysInfo.CloneDisplayPathEn=SysInfo.CloneDisplayPathEn;
		if((VdoEncBufSec!=0) && VdoEncBufSec<=20){
			MovieExe_EthCamRecId1_SetVdoEncBufSec(VdoEncBufSec);
		}
		XML_DefaultFormat(ETHCAM_CMD_SET_TX_SYS_INFO,  sizeof(ETHCAM_TX_SYS_INFO), bufAddr, bufSize, mimeType);

	}else{
		sEthCamSysInfo.PullModeEn=0;
		sEthCamSysInfo.CloneDisplayPathEn=1;
		DBG_ERR("USE Default Value ,PullModeEn=%d, CloneDisplayPathEn=%d\r\n", sEthCamSysInfo.PullModeEn,sEthCamSysInfo.CloneDisplayPathEn);
		XML_DefaultFormat(ETHCAM_CMD_SET_TX_SYS_INFO, ETHCAM_RET_ERR, bufAddr, bufSize, mimeType);
	}
#endif
	//XML_DefaultFormat(ETHCAM_CMD_SET_TX_SYS_INFO, ETHCAM_RET_OK, bufAddr, bufSize, mimeType);
	return ETHCAM_CMD_GETDATA_RETURN_OK;
}

int EthCam_SetTxCodecSrctype(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount)
{
#if(defined(_NVT_ETHREARCAM_TX_))
	ETHCAM_TX_CODEC_SRCTYPE CodecSrctype={0};

	if (argument && segmentCount==0) {
		XML_DefaultFormat(ETHCAM_CMD_SET_TX_CODEC_SRCTYPE, ETHCAM_RET_CONTINUE, bufAddr, bufSize, mimeType);
		return ETHCAM_CMD_GETDATA_RETURN_CONTI_NEED_ACKDATA;
	}else{
		memcpy((UINT8*)&CodecSrctype, (UINT8*)bufAddr, sizeof(ETHCAM_TX_CODEC_SRCTYPE));
		DBG_DUMP("VCode=%d, SrcType=%d\r\n", CodecSrctype.VCodec,CodecSrctype.Srctype);
	}

	DBG_DUMP("sizeof(ETHCAM_TX_CODEC_SRCTYPE)=%d\r\n", sizeof(ETHCAM_TX_CODEC_SRCTYPE));
	if((CodecSrctype.VCodec>=MOVIE_CODEC_H264 && CodecSrctype.VCodec<=MOVIE_CODEC_H265)
		&& (CodecSrctype.Srctype>=ETHCAM_TX_SYS_SRCTYPE_67 && CodecSrctype.Srctype<=ETHCAM_TX_SYS_SRCTYPE_68)){

		sEthCamCodecSrctype.VCodec=CodecSrctype.VCodec;
		sEthCamCodecSrctype.Srctype=CodecSrctype.Srctype;

		if (System_GetState(SYS_STATE_CURRMODE) == PRIMARY_MODE_MOVIE || System_GetState(SYS_STATE_NEXTMODE) == PRIMARY_MODE_MOVIE) {
			Ux_SendEvent(&CustomMovieObjCtrl,   NVTEVT_EXE_MOVIE_CODEC,         1,  sEthCamCodecSrctype.VCodec);
		}

		if(sEthCamCodecSrctype.Srctype==ETHCAM_TX_SYS_SRCTYPE_57){
			ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1, MOVIEMULTI_PARAM_VDOENC_UIPRJMODE, 0x57500000);
			ImageApp_MovieMulti_SetParam(_CFG_CLONE_ID_1, MOVIEMULTI_PARAM_VDOENC_UIPRJMODE, 0x57500000);
		}

		XML_ValueResult(ETHCAM_CMD_SET_TX_CODEC_SRCTYPE, sizeof(ETHCAM_TX_CODEC_SRCTYPE), bufAddr, bufSize, mimeType);
	}else{
		sEthCamCodecSrctype.VCodec=MOVIE_CODEC_H264;
		sEthCamCodecSrctype.Srctype=ETHCAM_TX_SYS_SRCTYPE_67;

		DBG_ERR("USE Default Value ,VCodec=%d, Srctype=%d\r\n", sEthCamCodecSrctype.VCodec,sEthCamCodecSrctype.Srctype);
		if (System_GetState(SYS_STATE_CURRMODE) == PRIMARY_MODE_MOVIE || System_GetState(SYS_STATE_NEXTMODE) == PRIMARY_MODE_MOVIE) {
			Ux_SendEvent(&CustomMovieObjCtrl,   NVTEVT_EXE_MOVIE_CODEC,         1,  sEthCamCodecSrctype.VCodec);
		}

		XML_ValueResult(ETHCAM_CMD_SET_TX_CODEC_SRCTYPE, 0, bufAddr, bufSize, mimeType);
	}
#endif
	//XML_DefaultFormat(ETHCAM_CMD_SET_TX_CODEC_SRCTYPE, ETHCAM_RET_OK, bufAddr, bufSize, mimeType);
	return ETHCAM_CMD_GETDATA_RETURN_OK;
}

int EthCam_DumpTxBsInfo(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount)
{
#if(defined(_NVT_ETHREARCAM_TX_))
	ImageApp_MovieMulti_EthCamTxRecId1_DumpBsInfo();
	ImageApp_MovieMulti_EthCamTxCloneId1_DumpBsInfo();
#endif
	XML_DefaultFormat(ETHCAM_CMD_DUMP_TX_BS_INFO, ETHCAM_RET_OK, bufAddr, bufSize, mimeType);
	return ETHCAM_CMD_GETDATA_RETURN_OK;
}

int EthCam_TxFwUpdate(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount)
{
#if(defined(_NVT_ETHREARCAM_TX_))
	//UINT32 uiWorkAddr, uiWorkSize;
	static UINT32 uiFwAddr=0, uiFwSize=0;
	static UINT32 uiRecvFwSize=0;
	char  *pch = 0;
	static UINT32 FwSendStart=0;
	DBG_IND("path =%s, argument = %s, bufAddr = 0x%x, bufSize =0x%x , segmentCount  =%d , putStatus = %d\r\n", path, argument, bufAddr, bufSize, segmentCount);
	//FWSRV_CMD FwSrvCmd = {0};

	UINT32 cmd = 0, par = 0;

	if (argument && segmentCount==0) {
		if (strncmp(argument, CMD_STR, strlen(CMD_STR)) == 0) {
			sscanf_s(argument+ strlen(CMD_STR), "%d", &cmd);
			DBG_DUMP("Recv cmd=%d\r\n",cmd);
		}
		pch = strchr(argument + strlen(CMD_STR), '&');
		if (pch) {
			if (strncmp(pch, PAR_STR, strlen(PAR_STR)) == 0) {
				sscanf_s(pch + strlen(PAR_STR), "%d", &par);
			}
		}
		if(cmd==ETHCAM_CMD_TX_FWUPDATE_FWSEND){
			if(uiFwSize==0){
				System_ChangeSubMode(SYS_SUBMODE_UPDFW);
				Ux_SendEvent(0, NVTEVT_SYSTEM_MODE, 1, PRIMARY_MODE_UPDFW);
				//uiFwAddr=EthCam_GetTxFwUpdateBufAddr(uiFwSize);
				uiFwAddr = OS_GetMempoolAddr(POOL_ID_APP);
			}
			uiFwSize=par;
			DBG_DUMP("Total FwSize=%d\r\n",uiFwSize);
			if(uiFwSize==0){
				DBG_ERR("uiFwSize Error ,uiFwSize=%d\r\n",uiFwSize);
			}

			if(uiFwSize!=0 && uiRecvFwSize < uiFwSize){
				DBG_DUMP("FWSEND Start\r\n");
				FwSendStart=1;
				XML_DefaultFormat(cmd, ETHCAM_RET_CONTINUE, bufAddr, bufSize, mimeType);
				return ETHCAM_CMD_GETDATA_RETURN_CONTINUE;
			}else{
				DBG_ERR("uiFwSize Error ,TotalSz=%d, RecvSz=%d\r\n",uiFwSize,uiRecvFwSize);
				XML_DefaultFormat(cmd, ETHCAM_RET_ERR, bufAddr, bufSize, mimeType);
				return CYG_HFS_CB_GETDATA_RETURN_ERROR;
			}
		}else if(cmd==ETHCAM_CMD_TX_FWUPDATE_START){

			DBG_DUMP("FWUPDATE_START\r\n");
			sEthCamFwUd.FwAddr=uiFwAddr;
			sEthCamFwUd.FwSize=uiFwSize;
			sEthCamFwUd.cmd=ETHCAM_CMD_TX_FWUPDATE_START;
			EthCam_GetDest(path, &sEthCamFwUd.path_id, &sEthCamFwUd.port_type);
			BKG_PostEvent(NVTEVT_BKW_FW_UPDATE);
			XML_DefaultFormat(cmd, ETHCAM_RET_CONTINUE, bufAddr, bufSize, mimeType);
			return ETHCAM_CMD_GETDATA_RETURN_OK;//CYG_HFS_CB_GETDATA_RETURN_OK;
		}else{
			DBG_ERR("argument=%s\r\n",argument);
			return CYG_HFS_CB_GETDATA_RETURN_ERROR;
		}
	}else{
		if(FwSendStart){
			if(uiFwSize!=0 && uiRecvFwSize < uiFwSize){
				hwmem_open();
				hwmem_memcpy((UINT32)(uiFwAddr + uiRecvFwSize), bufAddr, *bufSize);
				hwmem_close();
				uiRecvFwSize+=*bufSize;
				DBG_IND("RecvFwSize=%d, TotalFwSize=%d\r\n",uiRecvFwSize,uiFwSize);
#if 0 //write to card
				char path[30];
				sprintf(path, "A:\\TX_RecvFw.bin");
				DBG_DUMP("TX_RecvFw: %s\r\n", path);

				static  FST_FILE fhdl = 0;
				UINT32      fileSize = 0;

				if(fhdl==0){
					fhdl = FileSys_OpenFile(path, FST_CREATE_ALWAYS | FST_OPEN_WRITE);
					FileSys_SeekFile(fhdl, 0, FST_SEEK_SET);
				}else{
					FileSys_SeekFile(fhdl, 0, FST_SEEK_END);
				}
				fileSize = *bufSize;
				FileSys_WriteFile(fhdl, (UINT8 *)bufAddr, &fileSize, 0, NULL);
				if(uiRecvFwSize>=uiFwSize){
					DBG_DUMP("Write Finish!\r\n");
					FileSys_CloseFile(fhdl);
					fhdl = 0;
				}
#endif
				if(uiRecvFwSize>=uiFwSize){
					DBG_DUMP("Fw Recv Finish! RecvFwSize=%d, TotalFwSize=%d\r\n",uiRecvFwSize,uiFwSize);
					FwSendStart=0;
					XML_DefaultFormat(ETHCAM_CMD_TX_FWUPDATE_FWSEND, ETHCAM_RET_OK, bufAddr, bufSize, mimeType);
					return ETHCAM_CMD_GETDATA_RETURN_OK;
				}else{
					//XML_DefaultFormat(ETHCAM_CMD_TX_FWUPDATE_FWSEND, ETHCAM_RET_CONTINUE, bufAddr, bufSize, mimeType);
					return ETHCAM_CMD_GETDATA_RETURN_CONTINUE;
				}
			}else{
				DBG_ERR("argument=%s\r\n",argument);

#if 0
				// verify firmware
				FWSRV_VERIFY_FW Verify = {0};
				Verify.Buf.uiAddr = uiFwAddr;
				Verify.Buf.uiSize = uiFwSize;
				FwSrvCmd.Idx = FWSRV_CMD_IDX_VERIFY_FW;
				FwSrvCmd.In.pData = &Verify;
				FwSrvCmd.In.uiNumByte = sizeof(Verify);
				FwSrvCmd.Prop.bExitCmdFinish = TRUE;
				if (FwSrv_Cmd(&FwSrvCmd) != FWSRV_ER_OK) {
					DBG_ERR("check sum failed.\r\n");
					return CYG_HFS_CB_GETDATA_RETURN_ERROR;
				}
#endif

				return CYG_HFS_CB_GETDATA_RETURN_ERROR;
			}
		}
	}
#endif
	return ETHCAM_CMD_GETDATA_RETURN_OK;
}
int EthCam_GetFrame(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount)
{
#if(defined(_NVT_ETHREARCAM_TX_))
	char  *pch = 0;
	if (g_EthCamCmdGetFrame_SndNum==0){
		if ((pch = strchr(argument + strlen(CMD_STR), '&')) !=0){
			if (strncmp(pch, PAR_STR, strlen(PAR_STR)) == 0) {
				sscanf_s(pch + strlen(PAR_STR), "%d", &g_EthCamCmdGetFrame_SndNum);
			}
		}
	}
	g_EthCamCmdGetFrame_SndNum = (g_EthCamCmdGetFrame_SndNum==0)? 1 : g_EthCamCmdGetFrame_SndNum;
	//DBG_DUMP("SndNum=%d\r\n", g_EthCamCmdGetFrame_SndNum);

	set_flg(ETHCAM_CMD_RCV_FLG_ID,FLG_ETHCAM_CMD_RCV);
	EthCamNet_SetTxHB(0);
	//XML_DefaultFormat(ETHCAM_CMD_GET_FRAME, ETHCAM_CMD_GETDATA_RETURN_WAIT, bufAddr, bufSize, mimeType);
#endif
	return ETHCAM_CMD_GETDATA_RETURN_WAIT;
}
int EthCam_TxDispCropSet(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount)
{
#if(defined(_NVT_ETHREARCAM_TX_))
	if (argument && segmentCount==0) {
		XML_DefaultFormat(ETHCAM_CMD_TX_DISP_CROP, ETHCAM_RET_CONTINUE, bufAddr, bufSize, mimeType);
		return ETHCAM_CMD_GETDATA_RETURN_CONTINUE;
	}else{
		MOVIEMULTI_IME_CROP_INFO CropInfo={0};
		memcpy((UINT8*)&CropInfo, (UINT8*)bufAddr, sizeof(MOVIEMULTI_IME_CROP_INFO));
		DBG_DUMP("CropSize w=%d, h=%d, CropWin x=%d,y=%d,w=%d,h=%d\r\n", CropInfo.IMESize.w,CropInfo.IMESize.h,CropInfo.IMEWin.x,CropInfo.IMEWin.y,CropInfo.IMEWin.w,CropInfo.IMEWin.h);
		ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1, MOVIEMULTI_PARAM_ETHCAM_IME_CROP, (UINT32)&CropInfo);
	}
#endif
	XML_DefaultFormat(ETHCAM_CMD_TX_DISP_CROP, ETHCAM_RET_OK, bufAddr, bufSize, mimeType);

	return CYG_HFS_CB_GETDATA_RETURN_OK;
}

int EthCam_ChangeMode(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount)
{
#if(defined(_NVT_ETHREARCAM_TX_))
	Ux_PostEvent(NVTEVT_SYSTEM_MODE, 1, System_GetState(SYS_STATE_CURRMODE));
#endif
	XML_DefaultFormat(ETHCAM_CMD_CHANGE_MODE, ETHCAM_RET_OK, bufAddr, bufSize, mimeType);
	return ETHCAM_CMD_GETDATA_RETURN_OK;
}
int EthCam_IperfTest(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount)
{
#if(defined(_NVT_ETHREARCAM_TX_))

	char ipccmd[64];
	NVTIPC_SYS_MSG sysMsg;
	NVTIPC_I32 ipcErr = 0;

	System_ChangeSubMode(SYS_SUBMODE_UPDFW);
	Ux_SendEvent(0, NVTEVT_SYSTEM_MODE, 1, PRIMARY_MODE_UPDFW);

	snprintf(ipccmd, sizeof(ipccmd) - 1, "ethcamiperf iperf -c 192.168.0.3 -u -b 100M -t 60 -i 1 -w 128k");
	sysMsg.sysCmdID = NVTIPC_SYSCMD_SYSCALL_REQ;
	sysMsg.DataAddr = (UINT32)ipccmd;
	sysMsg.DataSize = strlen(ipccmd) + 1;
	if ((ipcErr = NvtIPC_MsgSnd(NVTIPC_SYS_QUEUE_ID, NVTIPC_SENDTO_CORE2, &sysMsg, sizeof(sysMsg))) < 0) {
		DBG_ERR("Failed to NVTIPC_SYS_QUEUE_ID\r\n");
	}
#endif
	XML_DefaultFormat(ETHCAM_CMD_IPERF_TEST, ETHCAM_RET_OK, bufAddr, bufSize, mimeType);
	return ETHCAM_CMD_GETDATA_RETURN_OK;
}
#if 0
int EthCam_TxLCAInfo(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount)
{
#if(defined(_NVT_ETHREARCAM_TX_))
#if (AV_LCA_FUNC == ENABLE)

	UINT32 list_value[20*4 + 2 ]={0};

	AVLCA_VEHICLE_INFO VehicleInfo={0};
	AV_GetLCAVehicleInfo(&VehicleInfo);
	if(VehicleInfo.iVehicleNum){
		UINT8 i;
		for(i=0;i<VehicleInfo.iVehicleNum;i++){
			//DBG_DUMP("VehicleNum[%d]=[%d][%d,%d,%d,%d]\r\n",VehicleInfo.iVehicleNum,i,VehicleInfo.iVehicleRect[i].x,VehicleInfo.iVehicleRect[i].y,VehicleInfo.iVehicleRect[i].w,VehicleInfo.iVehicleRect[i].h) ;
			list_value[4*i +0]=VehicleInfo.iVehicleRect[i].x;
			list_value[4*i +1]=VehicleInfo.iVehicleRect[i].y;
			list_value[4*i +2]=VehicleInfo.iVehicleRect[i].w;
			list_value[4*i +3]=VehicleInfo.iVehicleRect[i].h;
		}
	}
	AVLCA_WARNING_RESULT WarnRslt={0};
	AV_GetLCAWarnRslt(&WarnRslt);
	if(WarnRslt.LWarnStatus || WarnRslt.RWarnStatus){
		//DBG_DUMP("LWarnSta=%d, RWarnSta%d\r\n",WarnRslt.LWarnStatus,WarnRslt.RWarnStatus) ;
		list_value[20*4+0]=WarnRslt.LWarnStatus;
		list_value[20*4+1]=WarnRslt.RWarnStatus;
	}

	XML_ListResult(ETHCAM_CMD_TX_LCAINFO, list_value, sizeof(list_value)/sizeof(UINT32),  bufAddr, bufSize, mimeType);
	return ETHCAM_CMD_GETDATA_RETURN_OK;
#endif
#endif
	XML_DefaultFormat(ETHCAM_CMD_TX_LCAINFO, ETHCAM_RET_OK, bufAddr, bufSize, mimeType);
	return ETHCAM_CMD_GETDATA_RETURN_OK;
}
#endif
int EthCam_SetTxFlip(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount)
{
#if(defined(_NVT_ETHREARCAM_TX_))

	char  *pch = 0;
	UINT32 bFlipOn = 0;
	if ((pch = strchr(argument + strlen(CMD_STR), '&')) !=0) {
		if (strncmp(pch, PAR_STR, strlen(PAR_STR)) == 0) {
			sscanf_s(pch + strlen(PAR_STR), "%d", &bFlipOn);
		}
	}
	DBG_DUMP("bFlipOn=%d\r\n", bFlipOn);


	ImageUnit_Begin(ISF_IPL(0), 0);
	ImageUnit_SetVdoDirection(ISF_IN1, (bFlipOn == 1) ? ISF_VDO_DIR_MIRRORX : ISF_VDO_DIR_NONE);
	ImageUnit_End();

	ImageUnit_Begin(&ISF_ImgTrans, 0);
	ImageUnit_SetVdoDirection(ISF_OUT1,  (bFlipOn == 1) ? ISF_VDO_DIR_MIRRORX : ISF_VDO_DIR_NONE);
	ImageUnit_End();
#endif
	XML_DefaultFormat(ETHCAM_CMD_SET_TX_FLIP, ETHCAM_RET_OK, bufAddr, bufSize, mimeType);
	return ETHCAM_CMD_GETDATA_RETURN_OK;
}
int EthCam_SetTxAudCap(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount)
{
#if(defined(_NVT_ETHREARCAM_TX_))

	char  *pch = 0;
	UINT32 bAudCapOn = 0;
	if ((pch = strchr(argument + strlen(CMD_STR), '&')) !=0) {
		if (strncmp(pch, PAR_STR, strlen(PAR_STR)) == 0) {
			sscanf_s(pch + strlen(PAR_STR), "%d", &bAudCapOn);
		}
	}
	g_EthCamTxIsAudCapEn=bAudCapOn? 1: 0;
	DBG_DUMP("bAudCapOn=%d\r\n", bAudCapOn);
#endif
	XML_DefaultFormat(ETHCAM_CMD_SET_TX_AUDCAP, ETHCAM_RET_OK, bufAddr, bufSize, mimeType);
	return ETHCAM_CMD_GETDATA_RETURN_OK;
}

int EthCam_SetTxAudInfo(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount)
{
#if(defined(_NVT_ETHREARCAM_TX_))
	#if 0
	ETHCAM_TX_AUDINFO TxAudInfo={0};
	MOVIEMULTI_AUDIO_INFO MovieAudInfo=MovieExe_GetAudInfo();
	TxAudInfo.ACodec=AUDDEC_DECODER_AAC;
	TxAudInfo.AudCh=MovieAudInfo.aud_ch;
	TxAudInfo.AudChCnt=MovieAudInfo.aud_ch_num;
	TxAudInfo.AudRate=MovieAudInfo.aud_rate;

	UINT32 list_value[sizeof(ETHCAM_TX_AUDINFO)/sizeof(UINT32)];
	UINT32 list_total_cnt=sizeof(ETHCAM_TX_AUDINFO)/sizeof(UINT32);
	UINT32 *pBuf;
	pBuf=(UINT32 *)&TxAudInfo;

	UINT32 i;
	DBG_DUMP("AudInfo=\r\n");
	for(i=0;i<list_total_cnt;i++){
		list_value[i]=pBuf[i];
		DBG_DUMP("0x%x, ", pBuf[i]);
	}
	DBG_DUMP("\r\n");
	#else
	if (argument && segmentCount==0) {
		XML_DefaultFormat(ETHCAM_CMD_SET_TX_AUDINFO, ETHCAM_RET_CONTINUE, bufAddr, bufSize, mimeType);
		return ETHCAM_CMD_GETDATA_RETURN_CONTINUE;
	}else{
		DBG_DUMP("IsAudStreamRunning=%d\r\n", ImageApp_MovieMulti_IsAudStreamRunning());
		if(ImageApp_MovieMulti_IsAudStreamRunning()==0){
			MOVIEMULTI_AUDIO_INFO MovieAudInfo;
			memcpy((UINT8*)&MovieAudInfo, (UINT8*)bufAddr, sizeof(MOVIEMULTI_AUDIO_INFO));
			DBG_DUMP("AudInfo aud_ch=%d,aud_ch_num=%d,aud_rate=%d\r\n", MovieAudInfo.aud_ch,MovieAudInfo.aud_ch_num,MovieAudInfo.aud_rate);
			ImageApp_MovieMulti_Config(MOVIE_CONFIG_AUDIO_INFO, (UINT32) &MovieAudInfo);
			ImageApp_MovieMulti_SetParam(0, MOVIEMULTI_PARAM_AUD_SAMPLERATE, MovieAudInfo.aud_rate);
			ImageApp_MovieMulti_SetParam(0, MOVIEMULTI_PARAM_AUD_CHS,        MovieAudInfo.aud_ch);
			ImageApp_MovieMulti_SetParam(0, MOVIEMULTI_PARAM_AUD_CHS_NUM,    MovieAudInfo.aud_ch_num);
		}else{
			DBG_ERR("Audio Stream Running!!\r\n");
		}
	}
	#endif
	XML_DefaultFormat(ETHCAM_CMD_SET_TX_AUDINFO, ETHCAM_RET_OK, bufAddr, bufSize, mimeType);
#endif
	return ETHCAM_CMD_GETDATA_RETURN_OK;
}

int EthCam_TxLCARslt(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount)
{
	debug_msg("path =%s, argument = %s, bufAddr = 0x%x, bufSize =0x%x , segmentCount  =%d , putStatus = %d\r\n", path, argument, bufAddr, bufSize, segmentCount);
#if 1// (defined(_NVT_ETHREARCAM_TX_))
	if (argument && segmentCount==0) {
		CHKPNT;
		XML_DefaultFormat(ETHCAM_CMD_RX_LCAWRN, ETHCAM_RET_CONTINUE, bufAddr, bufSize, mimeType);
		return ETHCAM_CMD_GETDATA_RETURN_CONTINUE;
	}else{
		/*
	char  *pch = 0;
	UINT32 cmd = 0, par = 0;
	if (argument && segmentCount==0) {
		if (strncmp(argument, CMD_STR, strlen(CMD_STR)) == 0) {
			sscanf_s(argument+ strlen(CMD_STR), "%d", &cmd);
			DBG_DUMP("Recv cmd=%d\r\n",cmd);
		}
		pch = strchr(argument + strlen(CMD_STR), '&');
		if (pch) {
			if (strncmp(pch, PAR_STR, strlen(PAR_STR)) == 0) {
				sscanf_s(pch + strlen(PAR_STR), "%d", &par);
			}
		}
	}
	debug_msg("----------par = %s\r\n",&par);
	*/
	UINT8 LcaResult[128]={0};
	memcpy((CHAR*)&LcaResult, (UINT8*)bufAddr, 128);
	DBG_DUMP("sizeof(LcaRslt)=%d\r\n", sizeof(LcaResult));

	XML_StringResult(ETHCAM_CMD_RX_LCAWRN, (CHAR*)LcaResult, bufAddr, bufSize, mimeType);
	}
#endif
	XML_DefaultFormat(ETHCAM_CMD_RX_LCAWRN, ETHCAM_RET_OK, bufAddr, bufSize, mimeType);
	return ETHCAM_CMD_GETDATA_RETURN_OK;
}

//======================XML Cmd Result CB Satrt======================
void EthCam_Startup_CB(INT32 bEnd, void *output_data)
{
	ETHCAM_XML_DEFAULT_FORMAT *output=(ETHCAM_XML_DEFAULT_FORMAT*)output_data;
	DBG_IND("bEnd=%d, cmd=%d, status=%d\r\n",bEnd,output->cmd, output->status);
	if(output->status == ETHCAM_RET_OK){
		DBG_DUMP("Startup OK\r\n");
		sEthCamTxDecInfo[output->path_id].bStarupOK=1;
	}
}
void EthCam_MovieTBR_CB(INT32 bEnd, void *output_data)
{
	ETHCAM_XML_VALUE_RESULT *output=(ETHCAM_XML_VALUE_RESULT*)output_data;
	DBG_IND("bEnd=%d, cmd=%d, status=%d, value=%d\r\n",bEnd,output->cmd, output->status,output->value);
	DBG_IND("Tx TBR=%d\r\n",output->value);
	DBG_IND("path_id=%d, port_type=%d\r\n",output->path_id,output->port_type);

	sEthCamTxDecInfo[output->path_id].Tbr=output->value;
}
void EthCam_MovieFps_CB(INT32 bEnd, void *output_data)
{
	ETHCAM_XML_VALUE_RESULT *output=(ETHCAM_XML_VALUE_RESULT*)output_data;
	DBG_IND("bEnd=%d, cmd=%d, status=%d, value=%d\r\n",bEnd,output->cmd, output->status,output->value);
	DBG_IND("Tx Fps=%d\r\n",output->value);
	sEthCamTxDecInfo[output->path_id].Fps=output->value;
}
void EthCam_MovieGOP_CB(INT32 bEnd, void *output_data)
{
	ETHCAM_XML_VALUE_RESULT *output=(ETHCAM_XML_VALUE_RESULT*)output_data;
	DBG_IND("bEnd=%d, cmd=%d, status=%d, value=%d\r\n",bEnd,output->cmd, output->status,output->value);
	DBG_IND("Tx GOP=%d\r\n",output->value);
	sEthCamTxDecInfo[output->path_id].Gop=output->value;
}
void EthCam_MovieCodec_CB(INT32 bEnd, void *output_data)
{
	ETHCAM_XML_VALUE_RESULT *output=(ETHCAM_XML_VALUE_RESULT*)output_data;
	DBG_IND("bEnd=%d, cmd=%d, status=%d, value=%d\r\n",bEnd,output->cmd, output->status,output->value);
	DBG_IND("Tx Codec=%d\r\n",output->value);
	sEthCamTxDecInfo[output->path_id].Codec=output->value;
}

void EthCam_MovieRecSize_CB(INT32 bEnd, void *output_data)
{
	ETHCAM_XML_LIST *output=(ETHCAM_XML_LIST*)output_data;
	DBG_IND("bEnd=%d, cmd=%d, total_item_cnt=%d\r\n",bEnd,output->cmd, output->total_item_cnt);
	if(output->total_item_cnt==2){
		DBG_IND("Tx Width=%d, Height=%d\r\n",output->item[0], output->item[1]);
		sEthCamTxDecInfo[output->path_id].Width=output->item[0];
		sEthCamTxDecInfo[output->path_id].Height=output->item[1];
	}
}
/*
	H264_NALU_TYPE_SPS = 7,
	H264_NALU_TYPE_PPS = 8,

	H265_NALU_TYPE_VPS = 32,
	H265_NALU_TYPE_SPS = 33,
	H265_NALU_TYPE_PPS = 34,
*/
void EthCam_MovieSPS_PPS_CB(INT32 bEnd, void *output_data)
{
	ETHCAM_XML_LIST *output=(ETHCAM_XML_LIST*)output_data;
	UINT32 i;
	DBG_IND("bEnd=%d, cmd=%d, total_item_cnt=%d\r\n",bEnd,output->cmd, output->total_item_cnt);
	#if (ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
	if(output->port_type == ETHCAM_PORT_DATA1){
		sEthCamTxRecInfo[output->path_id].DescSize=output->total_item_cnt;
		//memcpy(sEthCamTxInfo.Desc,output->item,  output->total_item_cnt);
		DBG_IND("Rec Tx SPS[%d]= {",output->total_item_cnt);
		sEthCamTxRecInfo[output->path_id].SPSSize=0;
		sEthCamTxRecInfo[output->path_id].PPSSize=0;
		sEthCamTxRecInfo[output->path_id].VPSSize=0;
		if(sEthCamTxRecInfo[output->path_id].codec == MEDIAVIDENC_H264){
		for(i=0;i<output->total_item_cnt;i++){
			sEthCamTxRecInfo[output->path_id].Desc[i]=output->item[i];
				if(sEthCamTxRecInfo[output->path_id].SPSSize==0 && output->item[i] ==0  && output->item[i+1] ==0 && output->item[i+2] ==0 && output->item[i+3] ==1 && (output->item[i+4] & 0x1F) == 0x8){
					sEthCamTxRecInfo[output->path_id].SPSSize=i;
				}
			DBG_IND("0x%x, ",output->item[i]);
		}
		DBG_IND("}\r\n");
			sEthCamTxRecInfo[output->path_id].PPSSize=output->total_item_cnt-sEthCamTxRecInfo[output->path_id].SPSSize;
		}else{
			for(i=0;i<output->total_item_cnt;i++){
				sEthCamTxRecInfo[output->path_id].Desc[i]=output->item[i];
				if(sEthCamTxRecInfo[output->path_id].VPSSize==0 && output->item[i] ==0  && output->item[i+1] ==0 && output->item[i+2] ==0 && output->item[i+3] ==1 && (output->item[i+4] >>0x1) == 33){
					sEthCamTxRecInfo[output->path_id].VPSSize=i;
				}
				if(sEthCamTxRecInfo[output->path_id].SPSSize==0 && output->item[i] ==0  && output->item[i+1] ==0 && output->item[i+2] ==0 && output->item[i+3] ==1 && (output->item[i+4] >>0x1) == 34){
					sEthCamTxRecInfo[output->path_id].SPSSize=i-sEthCamTxRecInfo[output->path_id].VPSSize;
				}
				DBG_IND("0x%x, ",output->item[i]);
			}
			DBG_IND("}\r\n");
			sEthCamTxRecInfo[output->path_id].PPSSize=output->total_item_cnt-sEthCamTxRecInfo[output->path_id].VPSSize-sEthCamTxRecInfo[output->path_id].SPSSize;
		}
		DBG_DUMP("Rec SPSSize=%d, PPSSize=%d, VPSSize=%d\r\n",sEthCamTxRecInfo[output->path_id].SPSSize, sEthCamTxRecInfo[output->path_id].PPSSize,sEthCamTxRecInfo[output->path_id].VPSSize);

	}else if(output->port_type == ETHCAM_PORT_DATA2){
		sEthCamTxDecInfo[output->path_id].DescSize=output->total_item_cnt;
		//memcpy(sEthCamTxInfo.Desc,output->item,  output->total_item_cnt);
		DBG_IND("Dec Tx SPS[%d]= {",output->total_item_cnt);
		sEthCamTxDecInfo[output->path_id].SPSSize=0;
		sEthCamTxDecInfo[output->path_id].PPSSize=0;
		sEthCamTxDecInfo[output->path_id].VPSSize=0;
		if(sEthCamTxDecInfo[output->path_id].Codec == MEDIAVIDENC_H264){
		for(i=0;i<output->total_item_cnt;i++){
			sEthCamTxDecInfo[output->path_id].Desc[i]=output->item[i];
				if(sEthCamTxDecInfo[output->path_id].SPSSize==0 && output->item[i] ==0  && output->item[i+1] ==0 && output->item[i+2] ==0 && output->item[i+3] ==1 && (output->item[i+4] & 0x1F) == 0x8){
					sEthCamTxDecInfo[output->path_id].SPSSize=i;
				}
			DBG_IND("0x%x, ",output->item[i]);
		}
		DBG_IND("}\r\n");
			sEthCamTxDecInfo[output->path_id].PPSSize=output->total_item_cnt-sEthCamTxDecInfo[output->path_id].SPSSize;
		}else{
			for(i=0;i<output->total_item_cnt;i++){
				sEthCamTxDecInfo[output->path_id].Desc[i]=output->item[i];
				if(sEthCamTxDecInfo[output->path_id].VPSSize==0 && output->item[i] ==0  && output->item[i+1] ==0 && output->item[i+2] ==0 && output->item[i+3] ==1 && (output->item[i+4] >>0x1) == 33){
					sEthCamTxDecInfo[output->path_id].VPSSize=i;
				}
				if(sEthCamTxDecInfo[output->path_id].SPSSize==0 && output->item[i] ==0  && output->item[i+1] ==0 && output->item[i+2] ==0 && output->item[i+3] ==1 && (output->item[i+4] >>0x1) == 34){
					sEthCamTxDecInfo[output->path_id].SPSSize=i-sEthCamTxDecInfo[output->path_id].VPSSize;
				}
				DBG_IND("0x%x, ",output->item[i]);
			}
			DBG_IND("}\r\n");
			sEthCamTxDecInfo[output->path_id].PPSSize=output->total_item_cnt-sEthCamTxDecInfo[output->path_id].VPSSize-sEthCamTxDecInfo[output->path_id].SPSSize;
		}
		DBG_DUMP("Dec SPSSize=%d, PPSSize=%d, VPSSize=%d\r\n",sEthCamTxDecInfo[output->path_id].SPSSize, sEthCamTxDecInfo[output->path_id].PPSSize,sEthCamTxDecInfo[output->path_id].VPSSize);

	}
	#else
	DBG_IND("path_id= %d, port_type=%d\r\n",output->path_id,output->port_type);

	sEthCamTxRecInfo[output->path_id].DescSize=output->total_item_cnt;
	//memcpy(sEthCamTxInfo.Desc,output->item,  output->total_item_cnt);
	DBG_IND("Rec Tx SPS[%d]= {",output->total_item_cnt);
	sEthCamTxRecInfo[output->path_id].SPSSize=0;
	sEthCamTxRecInfo[output->path_id].PPSSize=0;
	sEthCamTxRecInfo[output->path_id].VPSSize=0;
	if(sEthCamTxRecInfo[output->path_id].codec == MEDIAVIDENC_H264){
	for(i=0;i<output->total_item_cnt;i++){
		sEthCamTxRecInfo[output->path_id].Desc[i]=output->item[i];
			if(sEthCamTxRecInfo[output->path_id].SPSSize==0 && output->item[i] ==0  && output->item[i+1] ==0 && output->item[i+2] ==0 && output->item[i+3] ==1 && (output->item[i+4] & 0x1F) == 0x8){
				sEthCamTxRecInfo[output->path_id].SPSSize=i;
			}
		DBG_IND("0x%x, ",output->item[i]);
	}
	DBG_IND("}\r\n");
		sEthCamTxRecInfo[output->path_id].PPSSize=output->total_item_cnt-sEthCamTxRecInfo[output->path_id].SPSSize;
	}else{
		for(i=0;i<output->total_item_cnt;i++){
			sEthCamTxRecInfo[output->path_id].Desc[i]=output->item[i];
			if(sEthCamTxRecInfo[output->path_id].VPSSize==0 && output->item[i] ==0  && output->item[i+1] ==0 && output->item[i+2] ==0 && output->item[i+3] ==1 && (output->item[i+4] >>0x1) == 33){
				sEthCamTxRecInfo[output->path_id].VPSSize=i;
			}
			if(sEthCamTxRecInfo[output->path_id].SPSSize==0 && output->item[i] ==0  && output->item[i+1] ==0 && output->item[i+2] ==0 && output->item[i+3] ==1 && (output->item[i+4] >>0x1) == 34){
				sEthCamTxRecInfo[output->path_id].SPSSize=i-sEthCamTxRecInfo[output->path_id].VPSSize;
			}
			DBG_IND("0x%x, ",output->item[i]);
		}
		DBG_IND("}\r\n");
		sEthCamTxRecInfo[output->path_id].PPSSize=output->total_item_cnt-sEthCamTxRecInfo[output->path_id].VPSSize-sEthCamTxRecInfo[output->path_id].SPSSize;
	}
	DBG_DUMP("Rec SPSSize=%d, PPSSize=%d, VPSSize=%d\r\n",sEthCamTxRecInfo[output->path_id].SPSSize, sEthCamTxRecInfo[output->path_id].PPSSize,sEthCamTxRecInfo[output->path_id].VPSSize);

	sEthCamTxDecInfo[output->path_id].DescSize=output->total_item_cnt;
	//memcpy(sEthCamTxInfo.Desc,output->item,  output->total_item_cnt);
	DBG_IND("Dec Tx SPS[%d]= {",output->total_item_cnt);
	sEthCamTxDecInfo[output->path_id].SPSSize=0;
	sEthCamTxDecInfo[output->path_id].PPSSize=0;
	sEthCamTxDecInfo[output->path_id].VPSSize=0;
	if(sEthCamTxDecInfo[output->path_id].Codec == MEDIAVIDENC_H264){
	for(i=0;i<output->total_item_cnt;i++){
		sEthCamTxDecInfo[output->path_id].Desc[i]=output->item[i];
			if(sEthCamTxDecInfo[output->path_id].SPSSize==0 && output->item[i] ==0  && output->item[i+1] ==0 && output->item[i+2] ==0 && output->item[i+3] ==1 && (output->item[i+4] & 0x1F) == 0x8){
				sEthCamTxDecInfo[output->path_id].SPSSize=i;
			}
		DBG_IND("0x%x, ",output->item[i]);
	}
	DBG_IND("}\r\n");
		sEthCamTxDecInfo[output->path_id].PPSSize=output->total_item_cnt-sEthCamTxDecInfo[output->path_id].SPSSize;
	}else{
		for(i=0;i<output->total_item_cnt;i++){
			sEthCamTxDecInfo[output->path_id].Desc[i]=output->item[i];
			if(sEthCamTxDecInfo[output->path_id].VPSSize==0 && output->item[i] ==0  && output->item[i+1] ==0 && output->item[i+2] ==0 && output->item[i+3] ==1 && (output->item[i+4] >>0x1) == 33){
				sEthCamTxDecInfo[output->path_id].VPSSize=i;
			}
			if(sEthCamTxDecInfo[output->path_id].SPSSize==0 && output->item[i] ==0  && output->item[i+1] ==0 && output->item[i+2] ==0 && output->item[i+3] ==1 && (output->item[i+4] >>0x1) == 34){
				sEthCamTxDecInfo[output->path_id].SPSSize=i-sEthCamTxDecInfo[output->path_id].VPSSize;
			}
			DBG_IND("0x%x, ",output->item[i]);
		}
		DBG_IND("}\r\n");
		sEthCamTxDecInfo[output->path_id].PPSSize=output->total_item_cnt-sEthCamTxDecInfo[output->path_id].VPSSize-sEthCamTxDecInfo[output->path_id].SPSSize;
	}
	DBG_DUMP("Dec SPSSize=%d, PPSSize=%d, VPSSize=%d\r\n",sEthCamTxDecInfo[output->path_id].SPSSize, sEthCamTxDecInfo[output->path_id].PPSSize,sEthCamTxDecInfo[output->path_id].VPSSize);
	#endif
}
void EthCam_MovieThumb_CB(INT32 bEnd, void *output_data)
{
	ETHCAM_XML_DEFAULT_FORMAT *output=(ETHCAM_XML_DEFAULT_FORMAT*)output_data;
	DBG_DUMP("bEnd=%d, cmd=%d, status=%d\r\n",bEnd,output->cmd, output->status);
}

void EthCam_MovieRawEncode_CB(INT32 bEnd, void *output_data)
{
	ETHCAM_XML_DEFAULT_FORMAT *output=(ETHCAM_XML_DEFAULT_FORMAT*)output_data;
	DBG_IND("bEnd=%d, cmd=%d, status=%d\r\n",bEnd,output->cmd, output->status);
	if(output->status == ETHCAM_RET_OK){
		DBG_IND("RawEncode OK\r\n");
 	}else{
		DBG_ERR("RawEncode Fail\r\n");
 	}
}
void EthCam_MovieRecInfo_CB(INT32 bEnd, void *output_data)
{
	ETHCAM_XML_LIST *output=(ETHCAM_XML_LIST*)output_data;
	UINT32 i;
	DBG_IND("MovieRecInfo_CB bEnd=%d, cmd=%d, total_item_cnt=%d\r\n",bEnd,output->cmd, output->total_item_cnt);
	UINT32 *pBuf;
	pBuf=(UINT32 *)&sEthCamTxRecInfo[output->path_id];
	for(i=0;i<output->total_item_cnt;i++){
		pBuf[i]=output->item[i];
	}
	DBG_DUMP("[%d]RecInfo, ar=%d, aud_codec=%d, codec=%d, gop=%d, vfr=%d\r\n",output->path_id,sEthCamTxRecInfo[output->path_id].ar,sEthCamTxRecInfo[output->path_id].aud_codec,sEthCamTxRecInfo[output->path_id].codec,sEthCamTxRecInfo[output->path_id].gop,sEthCamTxRecInfo[output->path_id].vfr);
	DBG_DUMP("[%d]height=%d, rec_format=%d, rec_mode=%d, tbr=%d, width=%d\r\n",output->path_id,sEthCamTxRecInfo[output->path_id].height,sEthCamTxRecInfo[output->path_id].rec_format,sEthCamTxRecInfo[output->path_id].rec_mode,sEthCamTxRecInfo[output->path_id].tbr,sEthCamTxRecInfo[output->path_id].width);
}
void EthCam_MovieDecInfo_CB(INT32 bEnd, void *output_data)
{
	ETHCAM_XML_LIST *output=(ETHCAM_XML_LIST*)output_data;
	UINT32 i;
	DBG_IND("MovieDecInfo_CB bEnd=%d, cmd=%d, total_item_cnt=%d\r\n",bEnd,output->cmd, output->total_item_cnt);
	UINT32 *pBuf;
	pBuf=(UINT32 *)&sEthCamTxDecInfo[output->path_id];
	for(i=1;i<output->total_item_cnt;i++){
		pBuf[i]=output->item[i];
	}
	DBG_DUMP("[%d]DecInfo, Tbr=%d, Fps=%d, Width=%d, Height=%d, Gop=%d, Codec=%d\r\n",output->path_id,sEthCamTxDecInfo[output->path_id].Tbr,sEthCamTxDecInfo[output->path_id].Fps,sEthCamTxDecInfo[output->path_id].Width,sEthCamTxDecInfo[output->path_id].Height,sEthCamTxDecInfo[output->path_id].Gop,sEthCamTxDecInfo[output->path_id].Codec);
}

void EthCam_TxFwUpdate_CB(INT32 bEnd, void *output_data)
{
	ETHCAM_XML_DEFAULT_FORMAT *output=(ETHCAM_XML_DEFAULT_FORMAT*)output_data;
	DBG_DUMP("bEnd=%d, cmd=%d, status=%d\r\n",bEnd,output->cmd, output->status);
	if(output->cmd == ETHCAM_CMD_TX_FWUPDATE_START ){
		if(output->status == ETHCAM_RET_FW_OK){
			DBG_DUMP("TxFwUpdate OK\r\n");
		}else if(output->status != ETHCAM_RET_CONTINUE){
			DBG_ERR("TxFwUpdate status=%d\r\n",output->status);
		}
	}
}
void EthCam_SyncTime_CB(INT32 bEnd, void *output_data)
{
	CHKPNT;

	//ETHCAM_XML_DEFAULT_FORMAT *output=(ETHCAM_XML_DEFAULT_FORMAT*)output_data;
	//DBG_IND("cmd=%d, status=%d\r\n",output->cmd, output->status);
}
void EthCam_SyncMenuSetting_CB(INT32 bEnd, void *output_data)
{
	ETHCAM_XML_DEFAULT_FORMAT *output=(ETHCAM_XML_DEFAULT_FORMAT*)output_data;
	DBG_DUMP("cmd=%d, status=%d\r\n",output->cmd, output->status);
}
void EthCam_GetFrame_CB(INT32 bEnd, void *output_data)
{
	//ETHCAM_XML_VALUE_RESULT *output=(ETHCAM_XML_VALUE_RESULT*)output_data;
	//DBG_IND("path_id=%d, cmd=%d, status=%d\r\n",output->path_id,output->cmd, output->status);
}
void EthCam_TxIOStatus_CB(INT32 bEnd, void *output_data)
{
	ETHCAM_XML_VALUE_RESULT *output=(ETHCAM_XML_VALUE_RESULT*)output_data;
	DBG_DUMP("TxIOStatus cmd=%d, path_id=%d, status=%d, value=%d\r\n",output->cmd, output->path_id, output->status,output->value);
}
extern char gEthcamFWVersion[33];
void EthCam_TxFWVersion_CB(INT32 bEnd, void *output_data)
{
	ETHCAM_XML_STRING_RESULT *output=(ETHCAM_XML_STRING_RESULT*)output_data;
	DBG_DUMP("TxFWVersion cmd=%d, path_id=%d, status=%d, string=%s\r\n",output->cmd, output->path_id,output->status,output->string);
	//get tx fwversion
	memcpy(gEthcamFWVersion, output->string, 33);
	//CHKPNT;
	//debug_msg("ethcam version:%s\r\n",gEthcamFWVersion);
}
void EthCam_SetTxIPReset_CB(INT32 bEnd, void *output_data)
{
	ETHCAM_XML_DEFAULT_FORMAT *output=(ETHCAM_XML_DEFAULT_FORMAT*)output_data;
	DBG_DUMP("cmd=%d, status=%d\r\n",output->cmd, output->status);
}

void EthCam_SetTxSysInfo_CB(INT32 bEnd, void *output_data)
{
	ETHCAM_XML_DEFAULT_FORMAT *output=(ETHCAM_XML_DEFAULT_FORMAT*)output_data;
	DBG_DUMP("SetTxSysInfo cmd=%d, path_id=%d, status=%d\r\n",output->cmd, output->path_id,output->status);
#if(defined(_NVT_ETHREARCAM_RX_))
	if(output->status== sizeof(ETHCAM_TX_SYS_INFO)){
		DBG_DUMP("SetTxSysInfo cmd OK\r\n");
		sEthCamTxSysInfo[output->path_id].bCmdOK=1;
	}else if(output->status== ETHCAM_RET_CONTINUE){
	}else{
		DBG_ERR("SetTxSysInfo cmd FAIL!!!\r\n");
		sEthCamTxSysInfo[output->path_id].bCmdOK=0;
	}
#endif
}
void EthCam_SetTxCodecSrctype_CB(INT32 bEnd, void *output_data)
{
	ETHCAM_XML_VALUE_RESULT *output=(ETHCAM_XML_VALUE_RESULT*)output_data;
	DBG_DUMP("SetTxCodecSrctype cmd=%d, path_id=%d, status=%d, value=%d\r\n",output->cmd, output->path_id, output->status,output->value);
#if(defined(_NVT_ETHREARCAM_RX_))
	if(output->value== sizeof(ETHCAM_TX_SYS_INFO)){
		DBG_DUMP("SetTxCodecSrctype cmd OK\r\n");
		sEthCamCodecSrctype[output->path_id].bCmdOK=1;
	}else{
		DBG_ERR("SetTxCodecSrctype cmd FAIL!!!\r\n");
		sEthCamCodecSrctype[output->path_id].bCmdOK=0;
	}
#endif
}
void EthCam_SetTxFlip_CB(INT32 bEnd, void *output_data)
{
	ETHCAM_XML_DEFAULT_FORMAT *output=(ETHCAM_XML_DEFAULT_FORMAT*)output_data;
	DBG_DUMP("SetTxFlip cmd=%d, status=%d\r\n",output->cmd, output->status);
}
void EthCam_SetTxAudCap_CB(INT32 bEnd, void *output_data)
{
	ETHCAM_XML_DEFAULT_FORMAT *output=(ETHCAM_XML_DEFAULT_FORMAT*)output_data;
	DBG_DUMP("SetTxAudCap cmd=%d, status=%d\r\n",output->cmd, output->status);
}

void EthCam_SetTxAudInfo_CB(INT32 bEnd, void *output_data)
{
	ETHCAM_XML_DEFAULT_FORMAT *output=(ETHCAM_XML_DEFAULT_FORMAT*)output_data;
	DBG_DUMP("GetTxAudInfo_CB cmd=%d, status=%d\r\n",output->cmd, output->status);
}
void EthCam_TxSlideUp_CB(INT32 bEnd, void *output_data)
{
	ETHCAM_XML_VALUE_RESULT *output=(ETHCAM_XML_VALUE_RESULT*)output_data;
	DBG_DUMP("SetTxSetOffset cmd=%d, path_id=%d, status=%d, value=%d\r\n",output->cmd, output->path_id, output->status,output->value);
	if(output->value== sizeof(ETHCAM_TX_SET_OFFESET)){
		DBG_DUMP("SetTxSetOffset cmd OK\r\n");
	}else{
		DBG_ERR("SetTxSetOffset cmd FAIL!!!\r\n");
	}
}

void EthCam_TxSlideDown_CB(INT32 bEnd, void *output_data)
{
	ETHCAM_XML_VALUE_RESULT *output=(ETHCAM_XML_VALUE_RESULT*)output_data;
	DBG_DUMP("SetTxSetOffset cmd=%d, path_id=%d, status=%d, value=%d\r\n",output->cmd, output->path_id, output->status,output->value);
	if(output->value== sizeof(ETHCAM_TX_SET_OFFESET)){
		DBG_DUMP("SetTxSetOffset cmd OK\r\n");
	}else{
		DBG_ERR("SetTxSetOffset cmd FAIL!!!\r\n");
	}
}
//======================XML Cmd Result CB End======================



void EthCamCmd_SetResultTable(ETHCAM_XML_RESULT *pAppCmdTbl)
{
	g_pEthCamCmdRstTbl = pAppCmdTbl;
}
ETHCAM_XML_RESULT *EthCamCmd_GetResultTable(void)
{
	if(g_pEthCamCmdRstTbl==NULL){
		DBG_ERR("Tbl NULL\r\n");
		return NULL;
	}
	return g_pEthCamCmdRstTbl;
}



#if (AV_LCA_FUNC == ENABLE)
#include "av_lca_lib.h"

//static INT32  pre_iVehNum=0;
INT32 flag_lca_adas;

int EthCam_TxLCAInfo(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount) 
{
//CHKPNT;
#if(defined(_NVT_ETHREARCAM_RX_))

    debug_msg("[mio_rx]:EthCam_TxLCAInfo:%s[len=%d]\r\n", bufAddr,bufSize);
    XML_DefaultFormat(ETHCAM_CMD_TX_LCAINFO, ETHCAM_RET_OK, bufAddr, bufSize, mimeType);
	
    return CYG_HFS_CB_GETDATA_RETURN_OK;

#endif

#if (defined(_NVT_ETHREARCAM_TX_))

	static INT32	pre_iVehNum=0;
	UINT32 list_value[5*2 + 3 + 10*4 ]={0};
	INT32 i = 0;

	//-----
	IPOINT AV_InstallCoord[5]={0};//总共5点坐标
	
	AV_GetInstallCoord(AV_InstallCoord);

	for(i=0;i<5;i++)
	{
		list_value[2*i+0]=AV_InstallCoord[i].x;
		list_value[2*i+1]=AV_InstallCoord[i].y;

		//debug_msg("i=%d,list_value=%d,%d\r\n",i,list_value[2*i+0],list_value[2*i+1]);
	}

	//--------

	AVLCA_WARNING_RESULT LcaRlt;

	AVLCA_WARNING_RESULT *pLcaRlt= &LcaRlt;

	AV_GetLCAWarnRslt(pLcaRlt);


	list_value[10] = pLcaRlt->LWarnStatus;//报警标志

	list_value[11] = pLcaRlt->RWarnStatus;//报警标志


	AVLCA_VEHICLE_INFO LcaVehicleRlt;

	AVLCA_VEHICLE_INFO *pLcaVehicleRlt= &LcaVehicleRlt;

	AV_GetLCAVehicleInfo(pLcaVehicleRlt);

	
	list_value[12] = pLcaVehicleRlt->iVehicleNum;//检测到的车辆数目

	#if  0//kevin_mio_tx
	debug_msg("[mio_tx]:iVehicleNum=%d[titk=%d]\r\n", pLcaVehicleRlt->iVehicleNum,Perf_GetCurrent());
	#endif

	for(i=0;i<pLcaVehicleRlt->iVehicleNum;i++)
	{

		list_value[4*i+13] = pLcaVehicleRlt->iVehicleRect[i].x;//车辆坐标

		list_value[4*i+14] = pLcaVehicleRlt->iVehicleRect[i].y;

		list_value[4*i+15] = pLcaVehicleRlt->iVehicleRect[i].w;

		list_value[4*i+16] = pLcaVehicleRlt->iVehicleRect[i].h;
	}


	//debug_msg("list_value=%d,%d\r\n",list_value[0],list_value[1]);

	flag_lca_adas = FALSE;
	if(pre_iVehNum != pLcaVehicleRlt->iVehicleNum)
		flag_lca_adas = TRUE;
		
	XML_ListResult(ETHCAM_CMD_TX_LCAINFO, list_value, sizeof(list_value)/sizeof(UINT32),  bufAddr, bufSize, mimeType);

	pre_iVehNum= pLcaVehicleRlt->iVehicleNum;
    return CYG_HFS_CB_GETDATA_RETURN_OK;

#endif

}


int EthCam_TxLCAStop(char *path, char *argument, HFS_U32 bufAddr, HFS_U32 *bufSize, char *mimeType, HFS_U32 segmentCount)
{
	debug_msg("path =%s, argument = %s, bufAddr = 0x%x, bufSize =0x%x , segmentCount  =%d , putStatus = %d\r\n", path, argument, bufAddr, bufSize, segmentCount);
#if(defined(_NVT_ETHREARCAM_RX_))
	CHKPNT;
	
#endif
#if defined(_NVT_ETHREARCAM_TX_)
	char  *pch = 0;
	UINT32 cmd = 0, par = 0;
	if (argument && segmentCount==0) {
		if (strncmp(argument, CMD_STR, strlen(CMD_STR)) == 0) {
			sscanf_s(argument+ strlen(CMD_STR), "%d", &cmd);
			DBG_DUMP("Recv cmd=%d\r\n",cmd);
		}
		pch = strchr(argument + strlen(CMD_STR), '&');
		if (pch) {
			if (strncmp(pch, PAR_STR, strlen(PAR_STR)) == 0) {
				sscanf_s(pch + strlen(PAR_STR), "%d", &par);
			}
		}
	g_Synctime=par;
	DBGD(g_Synctime);	
	}
#endif
	XML_DefaultFormat(ETHCAM_CMD_RX_LCASTOP, ETHCAM_RET_OK, bufAddr, bufSize, mimeType);
	return ETHCAM_CMD_GETDATA_RETURN_OK;
}



//UINT32 g_LCAWarnRslt[2]={0};
UINT32 g_LCAWarnRslt[2]={0};
UINT32 g_VehicleNum = 0;
URECT g_VehicleRect[10]={0};

IPOINT g_LCAInstallCoord[5]={0};
extern UINT32 EthCamRX_update_Vehicle_LCA_INFO(void);
//cj_mio


//cj_mio

void EthCam_LCAInfo_CB(INT32 bEnd, void *output_data) //
{
	#if(defined(_NVT_ETHREARCAM_RX_))
	ETHCAM_XML_LIST *output=(ETHCAM_XML_LIST*)output_data;
	
	//debug_msg("[mio_rx]:bEnd=%d, cmd=%d, total_item_cnt=%d\r\n",bEnd,output->cmd, output->total_item_cnt);
	UINT32  i =0;
	
	if(output->total_item_cnt==53){

		for(i=0;i<5;i++)
		{
			g_LCAInstallCoord[i].x = output->item[2*i+0];
			g_LCAInstallCoord[i].y = output->item[2*i+1];
		}

		g_LCAWarnRslt[0] = (UINT32)output->item[10];
		g_LCAWarnRslt[1] = (UINT32)output->item[11];
		g_VehicleNum = (UINT32)output->item[12];

//#MT#lyb -20200331 -begin
#if 1
    if((g_LCAWarnRslt[0] != 0) || (g_LCAWarnRslt[1] != 0))
    {
        //CHKPNT;
        Ux_PostEvent(NVTEVT_USER_LCA_RESULT, 2, g_LCAWarnRslt[0], g_LCAWarnRslt[1]);
    }
#endif
//#MT#lyb -20200331 -end

		for(i=0;i<g_VehicleNum;i++)
		{
			#if  0//kevin_mio
			DBG_DUMP("\r\n" "[mio_rx]:");
				{
					UINT32 j=0;
					for(j=0;j<20;j++)
					DBG_DUMP( "%d,",output->item[j]);
				}
			#endif
			
			//DBG_DUMP("\r\n[kevin_ethcam]:rx getTx(%d,%d,%d,%d)\r\n",output->item[4*i+3], output->item[4*i+4], output->item[4*i+5], output->item[4*i+6]);
			g_VehicleRect[i].x = output->item[4*i+13];
			g_VehicleRect[i].y = output->item[4*i+14]; 
			g_VehicleRect[i].w = output->item[4*i+15]; 
			g_VehicleRect[i].h = output->item[4*i+16];	
		}
	
		EthCamRX_update_Vehicle_LCA_INFO();
	}
	#endif
}


void EthCam_RxGetLCAWrnRslt_CB(INT32 bEnd, void *output_data) //
{
#if(defined(_NVT_ETHREARCAM_RX_))
	ETHCAM_XML_STRING_RESULT *output=(ETHCAM_XML_STRING_RESULT*)output_data;
	//debug_msg("EthCam_RxGetLCAWrnRslt_CB cmd=%d, path_id=%d, status=%d, string=%s\r\n",output->cmd, output->path_id,output->status,output->string);

	AVLCA_WARNING_RESULT LCA_Result ={0};
	//debug_msg("output->string;%s\r\n",output->string);
	AV_GetTXRslt(output->string,&LCA_Result);

	debug_msg("LCA_Result(%d,%d,%d)\r\n",LCA_Result.LWarnStatus,LCA_Result.MWarnStatus,LCA_Result.RWarnStatus);
	if(LCA_Result.LWarnStatus!=0 || LCA_Result.MWarnStatus!=0 || LCA_Result.RWarnStatus!=0)
	Ux_PostEvent(NVTEVT_USER_LCA_RESULT, 3, LCA_Result.RWarnStatus,LCA_Result.MWarnStatus,LCA_Result.LWarnStatus);
#endif
	#if 0//(defined(_NVT_ETHREARCAM_RX_))
	ETHCAM_XML_VALUE_RESULT *output=(ETHCAM_XML_VALUE_RESULT*)output_data;
	//DBG_DUMP("TxIOStatus cmd=%d, path_id=%d, status=%d, value=%X\r\n",output->cmd, output->path_id, output->status,output->value);

	g_LCAWarnRslt[0] = ((UINT32)output->value&0x0f0)>>4;
	g_LCAWarnRslt[1] = (UINT32)output->value&0x00f;

	debug_msg("^Yg_LCAWarnRslt(%d,%d)\r\n",g_LCAWarnRslt[0],g_LCAWarnRslt[1]);

    if(output->value != 0)
    {
        Ux_PostEvent(NVTEVT_USER_LCA_RESULT, 2, g_LCAWarnRslt[0], g_LCAWarnRslt[1]);
    }
    
	#endif
}




MIO_LCA_FUNC_INFO sEthCamRx_Vehicle_Info;

void EthCamRX_para_init(UINT32 LCA_Func_Enable_flag)
{
	
	sEthCamRx_Vehicle_Info.Enable_LCA = LCA_Func_Enable_flag;
	DBG_DUMP("[kevin_ethcam_rx]:rx_init success!\r\n");
	return;
}

UINT32 EthCamRX_update_Vehicle_LCA_INFO(void)
{

	UINT32 i;
	
	//g_LCAWarnRslt[0]
	
	//g_LCAWarnRslt[1] 
    
	
	if(g_VehicleNum)
		sEthCamRx_Vehicle_Info.LCA_Get_Valid =TRUE;
	else
		sEthCamRx_Vehicle_Info.LCA_Get_Valid =FALSE;

	sEthCamRx_Vehicle_Info.LCA_Num = g_VehicleNum;
	
	for(i=0;i<g_VehicleNum;i++)
	{
	    
		sEthCamRx_Vehicle_Info.LCARect[i].x =g_VehicleRect[i].x; 
		sEthCamRx_Vehicle_Info.LCARect[i].y =g_VehicleRect[i].y;
		sEthCamRx_Vehicle_Info.LCARect[i].w =g_VehicleRect[i].w;
		sEthCamRx_Vehicle_Info.LCARect[i].h =g_VehicleRect[i].h; 	
        
		DBG_DUMP("[kevin_ethcam_rx]:lca_ud vehicleNum=%d[%d]!\r\n",sEthCamRx_Vehicle_Info.LCA_Num,g_VehicleNum);
	}


	//sng_msg
	
	return 0;
}


UINT32 EthCamRX_Get_Vehicle_Status(void)  //sEthCamRx_Vehicle_Info
{
   //UINT32 i;
   
    //case 1:  //pCoord_get, to match displaysize_for_UIlayout
    #if 0
       if(sEthCamRx_Vehicle_Info.LCA_Get_Valid){
	   for(i=0;i<5;i++)
		{
			g_LCAInstallCoord[i].x   //1280
			g_LCAInstallCoord[i].y    //720


		}
       	}
	   #endif


	//case 2: // get valid_lca_info
	if(sEthCamRx_Vehicle_Info.Enable_LCA ){
     	//to be done @here
     	//--> sEthCamRx_Vehicle_Info
     	
	}else
		DBG_DUMP("[kevin_ethcam_rx]:lca_func_disalbe!\r\n");
		
	
	return 0;
}
#endif	//(AV_LCA_FUNC == ENABLE)



//==************************++++++++++++++++++***********************======================
ETHCAM_CMD_BEGIN(ethcam)
ETHCAM_CMD_ITEM(ETHCAM_CMD_STARTUP, 0, (UINT32)EthCam_STARTUP, 0, FL_NULL)

ETHCAM_CMD_ITEM(ETHCAM_CMD_GET_TX_MOVIE_TBR, 0, (UINT32)EthCam_GetMovieTBR, 0, FL_NULL)
ETHCAM_CMD_ITEM(ETHCAM_CMD_GET_TX_MOVIE_REC_SIZE, 0, (UINT32)EthCam_GetMovieRecSize, 0, FL_NULL)
ETHCAM_CMD_ITEM(ETHCAM_CMD_GET_TX_MOVIE_FPS, 0, (UINT32)EthCam_GetMovieFps, 0, FL_NULL)
ETHCAM_CMD_ITEM(ETHCAM_CMD_GET_TX_MOVIE_CODEC, 0, (UINT32)EthCam_GetMovieCodec, 0, FL_NULL)
ETHCAM_CMD_ITEM(ETHCAM_CMD_GET_TX_SPS_PPS, 0, (UINT32)EthCam_GetMovieSPS_PPS, 0, FL_NULL)
ETHCAM_CMD_ITEM(ETHCAM_CMD_GET_TX_MOVIE_GOP, 0, (UINT32)EthCam_GetMovieGop, 0, FL_NULL)
ETHCAM_CMD_ITEM(ETHCAM_CMD_TX_STREAM_START, 0, (UINT32)EthCam_TxStreamStart, 0, FL_NULL)
ETHCAM_CMD_ITEM(ETHCAM_CMD_TX_STREAM_STOP, 0, (UINT32)EthCam_TxStreamStop, 0, FL_NULL)
ETHCAM_CMD_ITEM(ETHCAM_CMD_GET_TX_MOVIE_THUMB, 0, (UINT32)EthCam_GetMovieThumb, 0, FL_NULL)
ETHCAM_CMD_ITEM(ETHCAM_CMD_GET_TX_MOVIE_RAW_ENCODE, 0, (UINT32)EthCam_GetMovieRawEncode, 0, FL_NULL)
ETHCAM_CMD_ITEM(ETHCAM_CMD_GET_TX_REC_INFO, 0, (UINT32)EthCam_GetMovieRecInfo, 0, FL_NULL)
ETHCAM_CMD_ITEM(ETHCAM_CMD_TX_RTSP_START, 0, (UINT32)EthCam_TxRtspStart, 0, FL_NULL)
ETHCAM_CMD_ITEM(ETHCAM_CMD_SYNC_TIME, 0, (UINT32)EthCam_SyncTime, 0, FL_NULL)
ETHCAM_CMD_ITEM(ETHCAM_CMD_SYNC_MENU_SETTING, 0, (UINT32)EthCam_SyncMenuSetting, 0, FL_NULL)
ETHCAM_CMD_ITEM(ETHCAM_CMD_TX_RESET_I, 0, (UINT32)EthCam_TxResetI, 0, FL_NULL)
ETHCAM_CMD_ITEM(ETHCAM_CMD_TX_FWUPDATE_FWSEND, 0, (UINT32)EthCam_TxFwUpdate, 0, FL_NULL)
ETHCAM_CMD_ITEM(ETHCAM_CMD_TX_FWUPDATE_START, 0, (UINT32)EthCam_TxFwUpdate, 0, FL_NULL)
ETHCAM_CMD_ITEM(ETHCAM_CMD_GET_TX_DEC_INFO, 0, (UINT32)EthCam_GetMovieDecInfo, 0, FL_NULL)
ETHCAM_CMD_ITEM(ETHCAM_CMD_TX_DISP_CROP, 0, (UINT32)EthCam_TxDispCropSet, 0, FL_NULL)
ETHCAM_CMD_ITEM(ETHCAM_CMD_GET_FRAME, 0, (UINT32)EthCam_GetFrame, 0, FL_NULL)
ETHCAM_CMD_ITEM(ETHCAM_CMD_TX_RESET_QUEUE, 0, (UINT32)EthCam_TxResetQueue ,0, FL_NULL)
ETHCAM_CMD_ITEM(ETHCAM_CMD_TX_POWEROFF, 0, (UINT32)EthCam_TxPowerOff ,0, FL_NULL)
ETHCAM_CMD_ITEM(ETHCAM_CMD_TX_FW_VERSION, 0, (UINT32)EthCam_TxFWVersion ,0, FL_NULL)
ETHCAM_CMD_ITEM(ETHCAM_CMD_SET_TX_IP_RESET, 0, (UINT32)EthCam_SetTxIPReset ,0, FL_NULL)
ETHCAM_CMD_ITEM(ETHCAM_CMD_SET_TX_SYS_INFO, 0, (UINT32)EthCam_SetTxSysInfo ,0, FL_NULL)
ETHCAM_CMD_ITEM(ETHCAM_CMD_DUMP_TX_BS_INFO, 0, (UINT32)EthCam_DumpTxBsInfo ,0, FL_NULL)
ETHCAM_CMD_ITEM(ETHCAM_CMD_CHANGE_MODE, 0, (UINT32)EthCam_ChangeMode ,0, FL_NULL)
ETHCAM_CMD_ITEM(ETHCAM_CMD_IPERF_TEST, 0, (UINT32)EthCam_IperfTest ,0, FL_NULL)
ETHCAM_CMD_ITEM(ETHCAM_CMD_SET_TX_CODEC_SRCTYPE, 0, (UINT32)EthCam_SetTxCodecSrctype ,0, FL_NULL)
ETHCAM_CMD_ITEM(ETHCAM_CMD_SET_TX_FLIP, 0, (UINT32)EthCam_SetTxFlip, 0, FL_NULL)
ETHCAM_CMD_ITEM(ETHCAM_CMD_SET_TX_AUDCAP, 0, (UINT32)EthCam_SetTxAudCap, 0, FL_NULL)
ETHCAM_CMD_ITEM(ETHCAM_CMD_SET_TX_AUDINFO, 0, (UINT32)EthCam_SetTxAudInfo, 0, FL_NULL)

#if (AV_LCA_FUNC == ENABLE)  //  if (AV_LCA_FUNC == ENABLE)
ETHCAM_CMD_ITEM(ETHCAM_CMD_TX_LCAINFO, 0, (UINT32)EthCam_TxLCAInfo, 0, FL_NULL) 
ETHCAM_CMD_ITEM(ETHCAM_CMD_RX_LCASTOP, 0, (UINT32)EthCam_TxLCAStop, 0, FL_NULL) 
#endif
ETHCAM_CMD_ITEM(ETHCAM_CMD_TX_SLIDEUP, 0, (UINT32)EthCam_SetTxSlideUP,0, FL_NULL)

ETHCAM_CMD_END()


ETHCAM_XML_RESULT EthCamXMLResultTbl[]={
	{ETHCAM_CMD_STARTUP, 						ETHCAM_XML_RESULT_TYPE_DEFAULT_FORMAT, 		EthCam_Startup_CB},
	{ETHCAM_CMD_GET_TX_MOVIE_TBR, 				ETHCAM_XML_RESULT_TYPE_VALUE_RESULT, 			EthCam_MovieTBR_CB},
	{ETHCAM_CMD_GET_TX_MOVIE_REC_SIZE, 		ETHCAM_XML_RESULT_TYPE_LIST, 					EthCam_MovieRecSize_CB},
	{ETHCAM_CMD_GET_TX_MOVIE_FPS, 				ETHCAM_XML_RESULT_TYPE_VALUE_RESULT, 			EthCam_MovieFps_CB},
	{ETHCAM_CMD_GET_TX_MOVIE_GOP, 			ETHCAM_XML_RESULT_TYPE_VALUE_RESULT, 			EthCam_MovieGOP_CB},
	{ETHCAM_CMD_GET_TX_MOVIE_CODEC, 			ETHCAM_XML_RESULT_TYPE_VALUE_RESULT, 			EthCam_MovieCodec_CB},
	{ETHCAM_CMD_GET_TX_SPS_PPS, 				ETHCAM_XML_RESULT_TYPE_LIST, 					EthCam_MovieSPS_PPS_CB},
	{ETHCAM_CMD_TX_STREAM_START, 				ETHCAM_XML_RESULT_TYPE_DEFAULT_FORMAT, 		NULL},
	{ETHCAM_CMD_TX_STREAM_STOP, 				ETHCAM_XML_RESULT_TYPE_DEFAULT_FORMAT, 		NULL},
	{ETHCAM_CMD_GET_TX_MOVIE_THUMB, 			ETHCAM_XML_RESULT_TYPE_DEFAULT_FORMAT, 		EthCam_MovieThumb_CB},
	{ETHCAM_CMD_GET_TX_MOVIE_RAW_ENCODE, 	ETHCAM_XML_RESULT_TYPE_DEFAULT_FORMAT, 		EthCam_MovieRawEncode_CB},
	{ETHCAM_CMD_GET_TX_REC_INFO, 				ETHCAM_XML_RESULT_TYPE_LIST, 					EthCam_MovieRecInfo_CB},
	{ETHCAM_CMD_TX_RTSP_START, 				ETHCAM_XML_RESULT_TYPE_DEFAULT_FORMAT, 		NULL},
	{ETHCAM_CMD_SYNC_TIME, 					ETHCAM_XML_RESULT_TYPE_DEFAULT_FORMAT, 		EthCam_SyncTime_CB},
	{ETHCAM_CMD_SYNC_MENU_SETTING, 			ETHCAM_XML_RESULT_TYPE_DEFAULT_FORMAT, 		EthCam_SyncMenuSetting_CB},
	{ETHCAM_CMD_TX_RESET_I, 					ETHCAM_XML_RESULT_TYPE_DEFAULT_FORMAT, 		NULL},
	{ETHCAM_CMD_GET_TX_DEC_INFO, 				ETHCAM_XML_RESULT_TYPE_LIST, 					EthCam_MovieDecInfo_CB},
	{ETHCAM_CMD_TX_FWUPDATE_FWSEND, 			ETHCAM_XML_RESULT_TYPE_DEFAULT_FORMAT, 		EthCam_TxFwUpdate_CB},
	{ETHCAM_CMD_TX_FWUPDATE_START, 			ETHCAM_XML_RESULT_TYPE_DEFAULT_FORMAT, 		EthCam_TxFwUpdate_CB},
	{ETHCAM_CMD_TX_DISP_CROP, 					ETHCAM_XML_RESULT_TYPE_DEFAULT_FORMAT, 		NULL},
	{ETHCAM_CMD_GET_FRAME, 					ETHCAM_XML_RESULT_TYPE_DEFAULT_FORMAT, 		EthCam_GetFrame_CB},
	{ETHCAM_CMD_TX_RESET_QUEUE, 				ETHCAM_XML_RESULT_TYPE_DEFAULT_FORMAT, 		NULL},
	{ETHCAM_CMD_TX_POWEROFF, 					ETHCAM_XML_RESULT_TYPE_DEFAULT_FORMAT, 		NULL},
	{ETHCAM_CMD_TX_IO_STATUS, 					ETHCAM_XML_RESULT_TYPE_VALUE_RESULT, 			EthCam_TxIOStatus_CB},
	{ETHCAM_CMD_TX_FW_VERSION, 				ETHCAM_XML_RESULT_TYPE_STRING_RESULT, 			EthCam_TxFWVersion_CB},
	{ETHCAM_CMD_SET_TX_IP_RESET, 				ETHCAM_XML_RESULT_TYPE_DEFAULT_FORMAT, 		EthCam_SetTxIPReset_CB},
	{ETHCAM_CMD_SET_TX_SYS_INFO, 				ETHCAM_XML_RESULT_TYPE_DEFAULT_FORMAT, 		EthCam_SetTxSysInfo_CB},
	{ETHCAM_CMD_TX_SLIDEUP, 				ETHCAM_XML_RESULT_TYPE_VALUE_RESULT, 		EthCam_TxSlideUp_CB},
	{ETHCAM_CMD_TX_SLIDEDOWN, 				ETHCAM_XML_RESULT_TYPE_VALUE_RESULT, 		EthCam_TxSlideDown_CB},
	{ETHCAM_CMD_CHANGE_MODE,					ETHCAM_XML_RESULT_TYPE_DEFAULT_FORMAT,		NULL},
	#if (AV_LCA_FUNC == ENABLE)
	{ETHCAM_CMD_TX_LCAINFO, 			ETHCAM_XML_RESULT_TYPE_LIST, 			EthCam_LCAInfo_CB},
	{ETHCAM_CMD_RX_LCASTOP, 				ETHCAM_XML_RESULT_TYPE_DEFAULT_FORMAT, 		NULL},//
	{ETHCAM_CMD_RX_LCAWRN,			ETHCAM_XML_RESULT_TYPE_STRING_RESULT,		EthCam_RxGetLCAWrnRslt_CB},//
	#endif
	{ETHCAM_CMD_DUMP_TX_BS_INFO, 				ETHCAM_XML_RESULT_TYPE_DEFAULT_FORMAT, 		NULL},
	{ETHCAM_CMD_CHANGE_MODE, 					ETHCAM_XML_RESULT_TYPE_DEFAULT_FORMAT, 		NULL},
	{ETHCAM_CMD_IPERF_TEST, 					ETHCAM_XML_RESULT_TYPE_DEFAULT_FORMAT, 		NULL},
	{ETHCAM_CMD_SET_TX_CODEC_SRCTYPE, 		ETHCAM_XML_RESULT_TYPE_VALUE_RESULT, 			EthCam_SetTxCodecSrctype_CB},
	{ETHCAM_CMD_SET_TX_FLIP, 					ETHCAM_XML_RESULT_TYPE_DEFAULT_FORMAT, 		EthCam_SetTxFlip_CB},
	{ETHCAM_CMD_SET_TX_AUDCAP, 				ETHCAM_XML_RESULT_TYPE_DEFAULT_FORMAT, 		EthCam_SetTxAudCap_CB},
	{ETHCAM_CMD_SET_TX_AUDINFO, 				ETHCAM_XML_RESULT_TYPE_DEFAULT_FORMAT, 		EthCam_SetTxAudInfo_CB},
};
#endif
