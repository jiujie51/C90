#include "PrjCfg.h"
#include "EthCamAppNetwork.h"



#include "UIAppWiFiCmd.h"
#include "UIInfo.h"
#include "UsockIpcAPI.h"
#include "SysCfg.h"

#include "UIAppNetwork.h"
#include "EthCamAppCmd.h"
#include "SysMain.h"
#include "SysCommon.h"
#include "UIAppNetwork.h"
#include "EthCamSocket.h"
#include "EthCamAppSocket.h"
#include "UIBackgroundObj.h"
#include "UIAppMovie.h"
#include "ImageApp_MovieMulti.h"
#include "UIModeUpdFw.h"
#include "UIControlWnd.h"
#include "UIFlow.h"
#include "NvtIpcAPI.h"
#include "mdcmdio.h"
#include "gpio.h"
#include "wdt.h"
#include "SwTimer.h"

#define THIS_DBGLVL         2 // 0=FATAL, 1=ERR, 2=WRN, 3=UNIT, 4=FUNC, 5=IND, 6=MSG, 7=VALUE, 8=USER
///////////////////////////////////////////////////////////////////////////////
#define __MODULE__          EthCamAppNetwork
#define __DBGLVL__          ((THIS_DBGLVL>=PRJ_DBG_LVL)?THIS_DBGLVL:PRJ_DBG_LVL)
#define __DBGFLT__          "*" //*=All, [mark]=CustomClass
#include "DebugModule.h"
///////////////////////////////////////////////////////////////////////////////
#if (defined(_NVT_ETHREARCAM_TX_))
#define NET_IP_ETH0            "192.168.0.12"
#else
#define NET_IP_ETH0            "192.168.0.3"
#endif
#define NET_SRVIP_ETH0         "192.168.0.3"
#define NET_LEASE_START_ETH0   "192.168.0.12"
#define NET_LEASE_END_ETH0     	"192.168.0.13"
#define UDP_PORT 2222
#define FIX_IP_MAX_LEN              16

#define ETHCAM_CLR_BIT(reg, bit)   ((reg) &= ~(1 << (bit)))
#define ETHCAM_SET_BIT(reg, bit)   ((reg) |= (1 << (bit)))
#if (defined(_NVT_ETHREARCAM_TX_) || defined(_NVT_ETHREARCAM_RX_))
static UINT32 g_bEthCamEthLinkStatus[ETHCAM_PATH_ID_MAX]={ETHCAM_LINK_DOWN,ETHCAM_LINK_DOWN};
static UINT32 g_bEthCamPrevEthLinkStatus[ETHCAM_PATH_ID_MAX]={ETHCAM_LINK_DOWN,ETHCAM_LINK_DOWN};
int SX_TIMER_ETHCAM_LINKDET_ID = -1;

#if defined(_NVT_ETHREARCAM_TX_)
UINT32 EthCamTxHB = 0;
static BOOL g_isWDTReset = FALSE;
static UINT32 g_CheckVDCnt =0;
static UINT32 g_PrevCheckAECnt =0;
static UINT32 g_WDTCnt =0;
#endif

#if (defined(_NVT_ETHREARCAM_RX_))
UINT32 EthCamHB1[ETHCAM_PATH_ID_MAX]={0}, EthCamHB2 = 0;
#if (ETH_REARCAM_CAPS_COUNT>=2)
#define ETHHUB_LINK_STATUS_REG		1
#define ETHHUB_LINK_STATUS_REG_BIT	2
#define ETHHUB_LINK_AN_COMPLETE_BIT	5
#define ETHHUB_LINK_CTRL_REG			0
#define ETHHUB_LINK_CTRL_REG_ISOLATE_BIT 10
int SX_TIMER_ETHCAM_ETHHUB_LINKDET_ID = -1;
UINT32  EthCamEthHub_LinkStatus[ETHCAM_PATH_ID_MAX]={0};
UINT32  EthCamEthHub_PrevLinkStatus[ETHCAM_PATH_ID_MAX]={0};
UINT32  EthCamEthHub_ANStatus[ETHCAM_PATH_ID_MAX]={0};
UINT16 ETHCAM_ETHHUB_PHY_PORT[ETHCAM_PATH_ID_MAX]={0, 2};//, 5, 6, 7};
static BOOL g_isChkPortReady = FALSE;
static UINT32 g_ChkPortReadyIPAddr = 0;
static UINT32 g_PrevSrvCliConnCliMacAddr[2]={0};
static UINT32 g_SrvCliConnCliMacAddrCnt=0;
static SWTIMER_ID  g_EthHubChkPortReadyTimeID=SWTIMER_NUM;
static SWTIMER_ID  g_EthHubPortReadySendCmdTimeID=SWTIMER_NUM;
static UINT32  g_isEthHubPortReadySendCmdTimerOpen=0;
static UINT32 g_isEthHubPortReadyCmdAck = 0;
static UINT32  g_EthHubChkPortReadyTimerRunning=0;
static BOOL g_bLastTxDet     = FALSE;
static BOOL g_bLastTxStatus  = FALSE;
static UINT32 g_isEthCamIPConflict=0;
#endif
int SX_TIMER_ETHCAM_ETHLINKRETRY_ID = -1;
#if defined(_CPU2_LINUX_)
static UINT32 gEthCamDhcpSrvConnIpAddr=0;//[ETHCAM_MAX_TX_NUM] = {0}; //for DHCP lease is 1,keep Mac of IP
static BOOL g_isUdpServerStart = FALSE;
#endif
BOOL g_bEthCamApp_IperfTest=0;
#endif
#if (defined(_NVT_ETHREARCAM_TX_))
ETHCAM_SOCKET_INFO SocketInfo[ETHCAM_PATH_ID_MAX]={{"192.168.0.12", 8887,8888,8899},{"192.168.0.13",8887,8888,8899}};
#else
#if (ETH_REARCAM_CAPS_COUNT>=2)
ETHCAM_SOCKET_INFO SocketInfo[]={{"0.0.0.0", 8887,8888,8899},{"0.0.0.0",8887,8888,8899}};
#else
ETHCAM_SOCKET_INFO SocketInfo[]={{"192.168.0.12", 8887,8888,8899}};
#endif
#endif
UINT32 ipstr2int(char* ip_addr)
{
	UINT32 nRet = 0;
	UINT16 i = 0;
	CHAR chBuf[16] = "";

	memcpy(chBuf, ip_addr ,15);
	//DBG_DUMP("ip_addr=%s\r\n",ip_addr);

	CHAR* szBuf = strtok(chBuf,".");
	//DBG_DUMP("szBuf=%s\r\n",szBuf);
	while(NULL != szBuf)
	{
		nRet += atoi(szBuf)<<((3-i)*8);
		szBuf = strtok(NULL,".");
		i++;
	}
	nRet=((nRet & 0xFF)<< 24) + ((nRet & 0xFF00)<< 8) + ((nRet & 0xFF0000)>>8) + ((nRet & 0xFF000000)>> 24);
	return nRet;
}
BOOL EthCamNet_IsIPConflict(void)
{
#if (defined(_NVT_ETHREARCAM_RX_) && (ETH_REARCAM_CAPS_COUNT>=2))
	return g_isEthCamIPConflict;
#else
	return 0;
#endif
}

void EthCamNet_EthHubPortIsolate(UINT32 port_id, BOOL port_isolate)
{
#if (ETHCAM_CHECK_PORTREADY == ENABLE)
#if (defined(MDC_GPIO) && defined(MDIO_GPIO))
	UINT32  read_data;
	UINT32  status;
	if(port_id>1){
		DBG_ERR("port_id=%d out of range!\r\n",port_id);
		return;
	}
	smiInit(MDC_GPIO, MDIO_GPIO);
	smiRead(ETHCAM_ETHHUB_PHY_PORT[port_id], ETHHUB_LINK_CTRL_REG, &read_data);
	//DBG_DUMP("read_data=0x%x\r\n",read_data);
	status= (read_data==0xffff) ?  0: ((read_data & (1<< ETHHUB_LINK_CTRL_REG_ISOLATE_BIT)) >> ETHHUB_LINK_CTRL_REG_ISOLATE_BIT);
	//DBG_DUMP("[%d]read_data=0x%x, now_sta=%d\r\n",port_id,read_data,status);
	if(status == (UINT32)port_isolate){
		return;
	}
	if(port_isolate){
		DBG_DUMP("[%d]ISOLATE ON\r\n",port_id);
		ETHCAM_SET_BIT(read_data, ETHHUB_LINK_CTRL_REG_ISOLATE_BIT);
	}else{
		DBG_DUMP("[%d]ISOLATE OFF\r\n",port_id);
		ETHCAM_CLR_BIT(read_data, ETHHUB_LINK_CTRL_REG_ISOLATE_BIT);
	}
	//DBG_DUMP("read_data BIT=0x%x\r\n",read_data);
	smiWrite(ETHCAM_ETHHUB_PHY_PORT[port_id], ETHHUB_LINK_CTRL_REG, read_data);
#endif
#endif
}
#if defined(_CPU2_LINUX_)
UINT32 EthCamNet_GetDhcpSrvConnIpAddr(void)
{
	return gEthCamDhcpSrvConnIpAddr;
}
int EthCamNet_UDPSocket_Recv(char *addr, int size)
{
#if(defined(_NVT_ETHREARCAM_RX_))
	char cmdname[64];
	unsigned int CliIPAddr=0;
	unsigned int CliMacAddr[2]={0};
	char chCliIPAddr[FIX_IP_MAX_LEN]={0};
	char chCliMacAddr[20]={0};

	sscanf_s(addr, "%s %d %d %d", cmdname, 50, &CliIPAddr, &CliMacAddr[0], &CliMacAddr[1]);
	if(strcmp(cmdname,"cliconnipnotify")==0){
		//SxTimer_SetFuncActive(SX_TIMER_ETHCAM_ETHLINKRETRY_ID, FALSE);
		snprintf(chCliIPAddr, FIX_IP_MAX_LEN, "%d.%d.%d.%d", (CliIPAddr & 0xFF), (CliIPAddr >> 8) & 0xFF, (CliIPAddr >> 16) & 0xFF, (CliIPAddr >> 24) & 0xFF);
		snprintf(chCliMacAddr, 20, "%02x:%02x:%02x:%02x:%02x:%02x", (CliMacAddr[0] & 0xFF), (CliMacAddr[0] >> 8) & 0xFF, (CliMacAddr[0] >> 16) & 0xFF, (CliMacAddr[0] >> 24) & 0xFF , (CliMacAddr[1]) & 0xFF ,(CliMacAddr[1] >> 8) & 0xFF);
		DBG_DUMP("Get=%s ,CliIPAddr=0x%x, %s, CliMac=%s\r\n", cmdname, CliIPAddr,chCliIPAddr, chCliMacAddr);

		strcpy(cmdname, "");
		snprintf(cmdname, sizeof(cmdname) - 1, "cliconnipnotify udp %d %d %d",CliIPAddr,CliMacAddr[0],CliMacAddr[1]);
		#if(ETH_REARCAM_CAPS_COUNT>=2)
			//EthCam_SrvCliConnIPNofity(cmdname);
			EthCamNet_SrvCliConnIPAddrNofity(cmdname);
		#else
		if(gEthCamDhcpSrvConnIpAddr==0){
			strcpy(SocketInfo[0].ip, chCliIPAddr);
			strcpy(SocketInfo[0].mac, chCliMacAddr);
			strcpy(cmdname, "");
			snprintf(cmdname, sizeof(cmdname) - 1, "SrvCliConnIPLink %d %d",ipstr2int(SocketInfo[0].ip), ETHCAM_LINK_UP);
			CHKPNT;
			EthCamNet_EthLinkStatusNotify(cmdname);
			gEthCamDhcpSrvConnIpAddr=CliIPAddr;
		}
		#endif
	}
#endif
	return 1;
}
void EthCamNet_UDPocket_Notify(int status, int parm)
{
	switch (status) {
	case CYG_ETHSOCKET_UDP_STATUS_CLIENT_REQUEST: {
		}
		break;
	}
}
#endif
void EthCamNet_UDPSocketOpen(void)
{
#if defined(_CPU2_LINUX_)
	//get client static ip
	ER rt;
	ETHSOCKIPC_OPEN EthsockIpcOpen = {0};
	if(g_isUdpServerStart){
		return;
	}
	g_isUdpServerStart=TRUE;
	EthsockIpcOpen.sharedSendMemAddr=OS_GetMempoolAddr(POOL_ID_ETHUDPSOCK_IPC);
	EthsockIpcOpen.sharedSendMemSize=POOL_SIZE_ETHUDPSOCK_IPC;
	EthsockIpcOpen.sharedRecvMemAddr=OS_GetMempoolAddr(POOL_ID_ETHUDPSOCK_IPC)+POOL_SIZE_ETHUDPSOCK_IPC;
	EthsockIpcOpen.sharedRecvMemSize=POOL_SIZE_ETHUDPSOCK_IPC;
	if (E_OK !=(rt = EthsockIpc_Open(&EthsockIpcOpen))) {
		if(rt != E_OBJ){
			DBG_ERR("EthsockIpc_Open fail\r\n");
		}
		return;
	}
	ethsocket_install_obj  udpsocket_obj = {0};
	DBG_DUMP("open udp usocket\r\n");
	udpsocket_obj.notify = EthCamNet_UDPocket_Notify;
	udpsocket_obj.recv = EthCamNet_UDPSocket_Recv;
	udpsocket_obj.portNum = 2222;
	udpsocket_obj.threadPriority = 8;
	udpsocket_obj.sockbufSize = 1024; //default 8192
	EthsockIpc_Udp_Init(&udpsocket_obj);
#endif
}
void EthCamNet_UDPSocketClose(void)
{
#if defined(_CPU2_LINUX_)
	g_isUdpServerStart=FALSE;
	EthsockIpc_Udp_Uninit();
	EthsockIpc_Uninit();
	EthsockIpc_Close();
#endif
}

void EthCamNet_EthLinkRetry(void)
{
#if defined(_CPU2_LINUX_)
	if(gEthCamDhcpSrvConnIpAddr==0){
		char udp_cmd[50]={0};
		int udp_cmdsize=sizeof(udp_cmd);
		udp_cmdsize=snprintf(udp_cmd, sizeof(udp_cmd) - 1, "cliconncheck");
		//DBG_DUMP("udp cmd=%s, sz=%d\r\n",udp_cmd, udp_cmdsize);
		EthsockIpc_Udp_Sendto(NET_LEASE_START_ETH0, UDP_PORT, udp_cmd, &udp_cmdsize);
	}
#endif
}
UINT32 EthCamNet_GetEthLinkStatus(ETHCAM_PATH_ID path_id)
{
	return g_bEthCamEthLinkStatus[path_id];
}
UINT32 EthCamNet_GetPrevEthLinkStatus(ETHCAM_PATH_ID path_id)
{
	return g_bEthCamPrevEthLinkStatus[path_id];
}
void EthCamNet_SetPrevEthLinkStatus(ETHCAM_PATH_ID path_id, UINT32 LinkStatus)
{
	g_bEthCamPrevEthLinkStatus[path_id]=LinkStatus;
}

extern BOOL autoRec;//111
extern BOOL bGetTxFirmware;
void EthCamNet_EthLinkStatusNotify(char *cmd)
{
	//CHKPNT;
char cmdname[40];
	UINT32 path_id_fr_eth=ETHCAM_PATH_ID_MAX;
	UINT32 TxIPAddr=0;
	CHAR chTxIPAddr[ETHCAM_SOCKETCLI_IP_MAX_LEN];
	UINT32 status;
	UINT32 path_id;
	UINT16 i=0;
#if (defined(_NVT_ETHREARCAM_RX_))
	UINT32 AllPathLinkStatus=0;
#endif
	ETHCAM_SOCKET_INFO EthSocketInfo[ETHCAM_PATH_ID_MAX];
	sscanf_s(cmd, "%s %d %d", cmdname, 40, &TxIPAddr, &status);
	snprintf(chTxIPAddr, ETHCAM_SOCKETCLI_IP_MAX_LEN, "%d.%d.%d.%d", (TxIPAddr & 0xFF), (TxIPAddr >> 8) & 0xFF, (TxIPAddr >> 16) & 0xFF, (TxIPAddr >> 24) & 0xFF);

	DBG_DUMP("Get = %s ,status=%d, TxIPAddr=0x%x, %s\r\n", cmdname, status,TxIPAddr,chTxIPAddr);

#if (defined(_NVT_ETHREARCAM_RX_) && ETH_REARCAM_CAPS_COUNT>=2)
	if(SxTimer_GetFuncActive(SX_TIMER_ETHCAM_ETHHUB_LINKDET_ID)==0){
		//DBG_DUMP("Active ETHHUB Timer\r\n");
		SxTimer_SetFuncActive(SX_TIMER_ETHCAM_ETHHUB_LINKDET_ID, TRUE);
	}
#endif

	if(TxIPAddr==0){
		//DBG_ERR("Tx No IP!!\r\n");
		return;
	}

	for(i=0;i<ETHCAM_PATH_ID_MAX;i++){
		if(strcmp(chTxIPAddr,SocketInfo[i].ip)==0){
			path_id_fr_eth=i;
			break;
		}
	}
	//DBG_DUMP("path_id_fr_eth=%d\r\n", path_id_fr_eth);

	if(path_id_fr_eth>=ETHCAM_PATH_ID_MAX){
		DBG_ERR("path id error %d\r\n", path_id_fr_eth);
		return;
	}
	if(strcmp(SocketInfo[i].ip,"0.0.0.0")==0){
		DBG_ERR("SocketInfo[%d].ip error ,ip=%s\r\n",i, SocketInfo[i].ip);
		return;
	}

	#if (defined(_NVT_ETHREARCAM_TX_))
		SysSetFlag(FL_ETHCAM_TX_IP_ADDR,TxIPAddr);
		memcpy(&EthSocketInfo[0], &SocketInfo[path_id_fr_eth],sizeof(ETHCAM_SOCKET_INFO));
		path_id=0; //path_id always 0 for Tx
	#else
		memcpy(&EthSocketInfo[0], &SocketInfo[0],sizeof(ETHCAM_SOCKET_INFO)*ETH_REARCAM_CAPS_COUNT);
		path_id=path_id_fr_eth;
	#endif

	g_bEthCamPrevEthLinkStatus[path_id]=g_bEthCamEthLinkStatus[path_id];
	if(status==ETHCAM_LINK_UP){
		//CHKPNT;
		DBG_DUMP("link up\r\n");
		g_bEthCamEthLinkStatus[path_id]=ETHCAM_LINK_UP;
		EthCamSocket_SetEthLinkStatus(path_id, ETHCAM_LINK_UP);
		#if (defined(_NVT_ETHREARCAM_TX_))
		EthCamSocket_SetInfo(&EthSocketInfo[0]);
		BKG_PostEvent(NVTEVT_BKW_ETHCAM_SOCKET_OPEN);
		EthCamTxHB = 0;
		#else
		EthCamSocketCli_SetSvrInfo(EthSocketInfo, ETH_REARCAM_CAPS_COUNT);
		//if ((System_IsModeChgClose()==0 && System_GetState(SYS_STATE_CURRMODE) == PRIMARY_MODE_MOVIE) || System_GetState(SYS_STATE_NEXTMODE) == PRIMARY_MODE_MOVIE) {
		if ((System_IsModeChgClose()==0 && System_GetState(SYS_STATE_CURRMODE) == PRIMARY_MODE_MOVIE) || (System_GetState(SYS_STATE_NEXTMODE) == PRIMARY_MODE_MOVIE)
			 || (System_GetState(SYS_STATE_CURRSUBMODE) == SYS_SUBMODE_UPDFW &&g_bEthCamApp_IperfTest==1)) {
			EthCamSocketCli_ReConnect(path_id, 0 ,0);
			BKG_PostEvent(NVTEVT_BKW_ETHCAM_SOCKETCLI_CMD_OPEN);

			EthCamHB1[path_id] = 0;
			EthCamHB2 = 0;
			if(g_bEthCamApp_IperfTest==0){
				SxTimer_SetFuncActive(SX_TIMER_ETHCAM_LINKDET_ID, TRUE);
			}
		}else{
			SxTimer_SetFuncActive(SX_TIMER_ETHCAM_ETHLINKRETRY_ID, FALSE);
			DBG_ERR("NOT movie mode!!\r\n");
		}
		#endif
	}else if(status==ETHCAM_LINK_DOWN){
		DBG_DUMP("link down\r\n");
		g_bEthCamEthLinkStatus[path_id]=ETHCAM_LINK_DOWN;
		EthCamSocket_SetEthLinkStatus(path_id, ETHCAM_LINK_DOWN);

#if (defined(_NVT_ETHREARCAM_TX_))
		EthCamTxHB = 0;
		SxTimer_SetFuncActive(SX_TIMER_ETHCAM_LINKDET_ID, FALSE);

		//stop TX Stream
		if(ImageApp_MovieMulti_IsStreamRunning(_CFG_REC_ID_1 | ETHCAM_TX_MAGIC_KEY)){
			MovieExe_EthCamTxStop(_CFG_REC_ID_1);
		}
		if(ImageApp_MovieMulti_IsStreamRunning(_CFG_CLONE_ID_1 | ETHCAM_TX_MAGIC_KEY)){
			MovieExe_EthCamTxStop(_CFG_CLONE_ID_1);
		}
		MovieExe_EthCamRecId1_ResetBsQ();

		EthCamSocket_Close();
#else
		//clear ip, for Tx unplug and power off
		if(strcmp(cmdname,"EthHubLink")==0){
			//DBG_WRN("clear IP addr\r\n");
			strcpy(SocketInfo[path_id].ip, "0.0.0.0");
			memset(SocketInfo[path_id].mac,0,ETHCAM_SOCKETCLI_MAC_MAX_LEN);
		}
#if (ETH_REARCAM_CAPS_COUNT>=2)
		g_PrevSrvCliConnCliMacAddr[0]=0;
		g_PrevSrvCliConnCliMacAddr[1]=0;
		g_SrvCliConnCliMacAddrCnt=0;
#endif
		EthCamNet_EthHubChkPortReadyTimerClose();
		EthCamNet_EthHubPortReadySendCmdTimerClose();
		if (System_GetState(SYS_STATE_CURRSUBMODE) == SYS_SUBMODE_UPDFW && g_bEthCamApp_IperfTest==0) {
			UxState_SetData(&UIFlowWndWaitMoment_StatusTXT_MsgCtrl, STATE_CURITEM, UIFlowWndWaitMoment_StatusTXT_Msg_STRID_ETHCAM_UDFW_FINISH);
			//Ux_OpenWindow(&UIFlowWndWaitMomentCtrl, 1, UIFlowWndWaitMoment_StatusTXT_Msg_STRID_ETHCAM_UDFW_FINISH);
			Delay_DelayMs(1000);
			CHKPNT;
			Ux_CloseWindow(&UIFlowWndWaitMomentCtrl, 0);
			FileSys_DeleteFile("A:\\FW671_AA.bin");//MT CJ 2020/420 //EthcamTxFW
			autoRec = FALSE;//11
			bGetTxFirmware = FALSE;
			System_ChangeSubMode(SYS_SUBMODE_NORMAL);
			Ux_SendEvent(0, NVTEVT_SYSTEM_MODE, 1, PRIMARY_MODE_MOVIE);
		}
		for(i=0;i<ETHCAM_PATH_ID_MAX;i++){
			if(g_bEthCamEthLinkStatus[i]==ETHCAM_LINK_UP){
				AllPathLinkStatus++;
			}
		}
		if(AllPathLinkStatus==0){
			EthCamCmd_GetFrameTimerEn(0);
			#if (ETH_REARCAM_CAPS_COUNT>=2)
			g_isChkPortReady=0;
			g_ChkPortReadyIPAddr=0;
			#endif
		}
		//for unplug and ethcam cmd not finish
		EthCamCmd_Done(path_id, 0xFFFFFFFF, ETHCAM_CMD_TERMINATE);

		socketCliEthData1_SetRecv(path_id, 0);
		socketCliEthData2_SetRecv(path_id, 0);
		if (System_GetState(SYS_STATE_CURRMODE) == PRIMARY_MODE_MOVIE) {
			if(ImageApp_MovieMulti_IsStreamRunning(_CFG_ETHCAM_ID_1+ path_id)){
				ImageApp_MovieMulti_EthCamRxRecStop(_CFG_ETHCAM_ID_1+ path_id);
			}
			ImageApp_MovieMulti_EthCamLinkForDisp((_CFG_ETHCAM_ID_1 + path_id), DISABLE, TRUE);
			DBGD(UI_GetData(FL_DUAL_CAM));
			MovieExe_EthCam_ChgDispCB(UI_GetData(FL_DUAL_CAM));
		}
		EthCamSocketCli_ReConnect(path_id, 0, 0);
		EthCamSocketCli_Close(path_id, ETHSOCKIPCCLI_ID_0);
		EthCamSocketCli_Close(path_id, ETHSOCKIPCCLI_ID_1);
		EthCamSocketCli_Close(path_id, ETHSOCKIPCCLI_ID_2);

		if(AllPathLinkStatus==0){
			SxTimer_SetFuncActive(SX_TIMER_ETHCAM_ETHLINKRETRY_ID, FALSE);

			SxTimer_SetFuncActive(SX_TIMER_ETHCAM_LINKDET_ID, FALSE);
			EthCamHB1[path_id] = 0;
			EthCamHB2 = 0;
			#if (ETH_REARCAM_CAPS_COUNT>=2)
			EthCamNet_EthHubPortIsolate(0, 0);
			EthCamNet_EthHubPortIsolate(1, 1);
			EthCamNet_DetConnInit();
			#endif
			//g_isEthCamIPConflict=0;
		}
		#if defined(_CPU2_LINUX_)
		gEthCamDhcpSrvConnIpAddr=0;
		#endif
		g_bEthCamPrevEthLinkStatus[path_id]=ETHCAM_LINK_DOWN;
#endif
	}
}
#if (defined(_NVT_ETHREARCAM_TX_))
void EthCamNet_SetTxHB(UINT32 value)
{
	EthCamTxHB=value;
}
UINT32 EthCamNet_GetTxHB(void)
{
	return EthCamTxHB;
}
#endif
void EthCamNet_LinkDetStreamRestart(UINT32 pathid)
{
	CHAR cmd[40]={0};
	snprintf(cmd, 40, "EthRestart %d %d", ipstr2int(SocketInfo[pathid].ip),ETHCAM_LINK_DOWN);
	CHKPNT;
	EthCamNet_EthLinkStatusNotify(cmd);
	snprintf(cmd, 40, "EthRestart %d %d", ipstr2int(SocketInfo[pathid].ip),ETHCAM_LINK_UP);
	EthCamNet_EthLinkStatusNotify(cmd);
}
void EthCamNet_LinkDet(void)
{
#if (defined(_NVT_ETHREARCAM_RX_))
	UINT32 j;
	//CHAR cmd[40]={0};
	UINT32 HBReset=0;

	for (j=0; j<ETH_REARCAM_CAPS_COUNT; j++){
#if (ETH_REARCAM_CLONE_FOR_DISPLAY == DISABLE)
		//DBG_DUMP("EthCamHB1[%d]=%d\r\n",j, EthCamHB1[j]);
		EthCamHB1[j] ++;
		if (EthCamHB1[j] && !(EthCamHB1[j] % 5)) {
#else
		//DBG_DUMP("EthCamHB1/2=%d/%d\r\n", EthCamHB1[j], EthCamHB2);
#if defined(_NVT_ETHREARCAM_CAPS_COUNT_2_)
		EthCamHB1[j] ++;
#endif
		EthCamHB2 ++;
		if ((EthCamHB1[j] && !(EthCamHB1[j] % 5)) || (EthCamHB2 && !(EthCamHB2 % 5))) {
#endif
			if(g_bEthCamEthLinkStatus[j]==ETHCAM_LINK_UP){
				DBG_WRN("EthCamHBActive HB1[%d]=%d, HB2=%d\r\n", j,EthCamHB1[j],EthCamHB2);
				EthCamNet_LinkDetStreamRestart(j);
				//EthCamNet_EthLinkStatusNotify("EthRestart 201369792 1");
				//EthCamNet_EthLinkStatusNotify("EthRestart 201369792 2");
			}
			//if (kchk_flg(ETHCAM_CMD_SND_FLG_ID, FLG_ETHCAM_CMD_SND) == FLG_ETHCAM_CMD_SND){
			//}
		}
#if (ETH_REARCAM_CLONE_FOR_DISPLAY == DISABLE)
		if (EthCamHB1[j] > 15) {
#else
		if (EthCamHB1[j] > 15 || EthCamHB2 > 15) {
#endif
			//SxTimer_SetFuncActive(SX_TIMER_ETHCAM_LINKDET_ID, FALSE);
			EthCamHB1[j] = 0;
			EthCamHB2 = 0;
		}
	}
	for(j=0;j<ETH_REARCAM_CAPS_COUNT;j++){
		if(EthCamHB1[j]==0){
			HBReset++;
		}
	}
#if (ETH_REARCAM_CLONE_FOR_DISPLAY == DISABLE)
	if(HBReset==ETH_REARCAM_CAPS_COUNT){
#else
	if(HBReset==ETH_REARCAM_CAPS_COUNT || EthCamHB2==0){
#endif
		SxTimer_SetFuncActive(SX_TIMER_ETHCAM_LINKDET_ID, FALSE);
	}
#endif
//#if (defined(_NVT_ETHREARCAM_TX_) && ETH_REARCAM_CAPS_COUNT>=2)
#if (defined(_NVT_ETHREARCAM_TX_))
	if(sEthCamSysInfo.PullModeEn){
		EthCamTxHB ++;
		if(EthCamTxHB >=6){// 3 sec
			//hCamNet_EthLinkStatusNotify("EthLinkDown 0 1");
			if(ImageApp_MovieMulti_IsStreamRunning(_CFG_REC_ID_1 | ETHCAM_TX_MAGIC_KEY)){
				DBG_WRN("NoHB Stop Stream\r\n");

				MovieExe_EthCamTxStop(_CFG_REC_ID_1);
				EthCamTxHB=0;
			}
		}
	}
	EthCamNet_CheckVDCount();
#endif
}
#if (defined(_NVT_ETHREARCAM_RX_))
#if (ETH_REARCAM_CAPS_COUNT>=2)
static void EthCamNet_EthHubPortReadySendCmdTimer_TimeOutCB(UINT32 uiEvent)
{
	//DBG_DUMP("ReadySendCmdTimer_TimeOut\r\n");
	if(g_isEthHubPortReadyCmdAck==0){
		BKG_PostEvent(NVTEVT_BKW_ETHCAM_CHECKPORT_READY);
	}
}
static void EthCamNet_EthHubChkPortReadyTimer_TimeOutCB(UINT32 uiEvent)
{
	//DBG_DUMP("ReadyTimer_TimeOut\r\n");
	g_EthHubChkPortReadyTimerRunning=0;
}
#endif
void EthCamNet_EthHubPortReadySendCmdTimerOpen(void)
{
#if (ETH_REARCAM_CAPS_COUNT>=2)
	SWTIMER_CB EventHandler;
	if(g_isEthHubPortReadySendCmdTimerOpen){
		DBG_WRN("timer already open\r\n");
		return;
	}
	EventHandler=(SWTIMER_CB)EthCamNet_EthHubPortReadySendCmdTimer_TimeOutCB;
	if (SwTimer_Open(&g_EthHubPortReadySendCmdTimeID, EventHandler) != E_OK) {
		DBG_ERR("open timer fail\r\n");
	} else {
		SwTimer_Cfg(g_EthHubPortReadySendCmdTimeID, 200 /*ms*/, SWTIMER_MODE_ONE_SHOT);
		SwTimer_Start(g_EthHubPortReadySendCmdTimeID);
		g_isEthHubPortReadySendCmdTimerOpen=1;
	}
	//DBG_DUMP("g_EthHubPortReadySendCmdTimeID=%d\r\n",g_EthHubPortReadySendCmdTimeID);
#endif
}
void EthCamNet_EthHubPortReadySendCmdTimerClose(void)
{
#if (ETH_REARCAM_CAPS_COUNT>=2)
	if(g_isEthHubPortReadySendCmdTimerOpen==0){
		return;
	}
	if(g_EthHubPortReadySendCmdTimeID !=SWTIMER_NUM){
		//DBG_DUMP("Port Ready close timer\r\n");

		SwTimer_Stop(g_EthHubPortReadySendCmdTimeID);
		SwTimer_Close(g_EthHubPortReadySendCmdTimeID);
	}
	g_EthHubPortReadySendCmdTimeID=SWTIMER_NUM;
	g_isEthHubPortReadySendCmdTimerOpen=0;
#endif
}
void EthCamNet_EthHubChkPortReadyTimerOpen(void)
{
#if (ETH_REARCAM_CAPS_COUNT>=2)
	SWTIMER_CB EventHandler;
	EventHandler=(SWTIMER_CB)EthCamNet_EthHubChkPortReadyTimer_TimeOutCB;
	if (SwTimer_Open(&g_EthHubChkPortReadyTimeID, EventHandler) != E_OK) {
		DBG_ERR("open timer fail\r\n");
	} else {
		SwTimer_Cfg(g_EthHubChkPortReadyTimeID, 300 /*ms*/, SWTIMER_MODE_ONE_SHOT);
		SwTimer_Start(g_EthHubChkPortReadyTimeID);
		g_EthHubChkPortReadyTimerRunning=1;
	}
	//DBG_DUMP("g_EthHubChkPortReadyTimeID=%d\r\n",g_EthHubChkPortReadyTimeID);
#endif
}
void EthCamNet_EthHubChkPortReadyTimerClose(void)
{
#if (ETH_REARCAM_CAPS_COUNT>=2)
	if(g_EthHubChkPortReadyTimeID !=SWTIMER_NUM){
		DBG_DUMP("Port Ready close timer\r\n");

		SwTimer_Stop(g_EthHubChkPortReadyTimeID);
		SwTimer_Close(g_EthHubChkPortReadyTimeID);
	}
	g_EthHubChkPortReadyTimeID=SWTIMER_NUM;
#endif
}
void EthCamNet_EthHubChkPortReady(void)
{
#if (ETH_REARCAM_CAPS_COUNT>=2)
#if (ETHCAM_CHECK_PORTREADY == ENABLE)
	EthCamNet_EthHubLinkDet();
	if(EthCamEthHub_LinkStatus[0] && EthCamEthHub_LinkStatus[1]){
		if(ipstr2int(SocketInfo[0].ip) &&  g_isChkPortReady==0){
		EthCamNet_EthHubPortIsolate(1, 0);
		g_isEthHubPortReadyCmdAck=0;
		g_ChkPortReadyIPAddr=((ipstr2int(SocketInfo[0].ip) == 201369792) ? 218147008: 201369792);
		AppBKW_SetData(BKW_ETHCAM_CHECK_PORT_READY_IP,g_ChkPortReadyIPAddr );
		BKG_PostEvent(NVTEVT_BKW_ETHCAM_CHECKPORT_READY);
		g_isChkPortReady=1;
		}
	}else if(EthCamEthHub_LinkStatus[1] && g_isChkPortReady==0){

		EthCamNet_EthHubPortIsolate(1, 0);
		g_isEthHubPortReadyCmdAck=0;

		AppBKW_SetData(BKW_ETHCAM_CHECK_PORT_READY_IP,0 );
		BKG_PostEvent(NVTEVT_BKW_ETHCAM_CHECKPORT_READY);

		g_isChkPortReady=1;
	}
#endif
#endif
}
void EthCamNet_SrvCliConnIPAddrNofity(char *cmd)
{
	char cmdname[50]={0};
	char cmdname2[50]={0};
	UINT32 CliIPAddr=0;
	UINT32 CliMacAddr[2]={0};
	#if (ETHCAM_CHECK_PORTREADY == ENABLE)
	static char prev_cmdname[50]={0};
	static char prev_cmdname2[50]={0};
	#endif
	CHAR chCliIPAddr[ETHCAM_SOCKETCLI_IP_MAX_LEN]={0};
	CHAR chCliMacAddr[ETHCAM_SOCKETCLI_MAC_MAX_LEN]={0};


	sscanf_s(cmd, "%s %s %d %d %d", cmdname, sizeof(cmdname), cmdname2, sizeof(cmdname2), &CliIPAddr, &CliMacAddr[0], &CliMacAddr[1]);
	snprintf(chCliIPAddr, ETHCAM_SOCKETCLI_IP_MAX_LEN, "%d.%d.%d.%d", (CliIPAddr & 0xFF), (CliIPAddr >> 8) & 0xFF, (CliIPAddr >> 16) & 0xFF, (CliIPAddr >> 24) & 0xFF);
	snprintf(chCliMacAddr, ETHCAM_SOCKETCLI_MAC_MAX_LEN, "%02x:%02x:%02x:%02x:%02x:%02x", (CliMacAddr[0] & 0xFF), (CliMacAddr[0] >> 8) & 0xFF, (CliMacAddr[0] >> 16) & 0xFF, (CliMacAddr[0] >> 24) & 0xFF , (CliMacAddr[1]) & 0xFF ,(CliMacAddr[1] >> 8) & 0xFF);
	DBG_DUMP("Get=%s ,cmd2=%s, CliIPAddr=0x%x, %s, CliMac=%s\r\n", cmdname, cmdname2, CliIPAddr,chCliIPAddr, chCliMacAddr);

	#if (ETH_REARCAM_CAPS_COUNT>=2)
	if((g_PrevSrvCliConnCliMacAddr[0]!= CliMacAddr[0]) || (g_PrevSrvCliConnCliMacAddr[1]!= CliMacAddr[1])){
		g_SrvCliConnCliMacAddrCnt++;
	}
#if (ETHCAM_CHECK_PORTREADY == ENABLE)
	DBG_DUMP("CliConCnt=%d\r\n",g_SrvCliConnCliMacAddrCnt);
	g_PrevSrvCliConnCliMacAddr[0]= CliMacAddr[0];
	g_PrevSrvCliConnCliMacAddr[1]= CliMacAddr[1];

	if((strcmp(cmdname,"cliconnipnotify")==0)
		&& (strcmp(cmdname2,"dhcpsrv")==0)
		&& g_isChkPortReady==0){
		DBG_DUMP("isChkPortReady=0 return\r\n");
		g_isEthHubPortReadyCmdAck=0;
		AppBKW_SetData(BKW_ETHCAM_CHECK_PORT_READY_IP, 0);
		while(BKG_GetExecFuncTable()->event==0){
			Delay_DelayMs(5);
		}
		BKG_PostEvent(NVTEVT_BKW_ETHCAM_CHECKPORT_READY);
		return;
	}
	if((strcmp(cmdname,"cliconnipnotify")==0) && (strcmp(cmdname2,"udp")==0)){
		g_isEthHubPortReadyCmdAck=1;
	}
	strcpy(prev_cmdname, cmdname);
	strcpy(prev_cmdname2, cmdname2);
#endif
	UINT16  i=0;
	for(i=0;i<ETHCAM_PATH_ID_MAX;i++){
		if(strcmp(SocketInfo[i].mac,chCliMacAddr)==0){
			//DBG_ERR("Mac Addr Already Exist!! SocketInfo[%d].mac=%s\r\n",i, SocketInfo[i].mac);
			return;
		}
		if(strcmp(SocketInfo[i].ip,chCliIPAddr)==0 ){
			//DBG_WRN("IP Already Exist!! SocketInfo[%d].ip=%s\r\n",i, SocketInfo[i].ip);
			if(strcmp(SocketInfo[i].mac,chCliMacAddr)!=0 ){
				DBG_ERR("[%d]IP conflict!! old=%s, new=%s\r\n",i, SocketInfo[i].mac, chCliMacAddr);
				if(socketCliEthCmd_IsConn(ETHCAM_PATH_ID_1+i)){
					EthCam_SendXMLCmd(ETHCAM_PATH_ID_1+i, ETHCAM_PORT_DEFAULT ,ETHCAM_CMD_SET_TX_IP_RESET, 0);
					if(EthCam_SendXMLCmd(ETHCAM_PATH_ID_1+i, ETHCAM_PORT_DEFAULT ,ETHCAM_CMD_TX_POWEROFF, 0)==ETHCAM_RET_ERR){
						g_isEthCamIPConflict=1;
					}
				}else{
					g_isEthCamIPConflict=1;
				}
			}
			return;
		}
	}

	EthCamNet_EthHubLinkDet();
	for(i=0;i<ETHCAM_PATH_ID_MAX;i++){
		//DBG_DUMP("EthHub_LinkStatus[%d]=%d, EthLinkStatus[%d]=%d\r\n", i,EthCamEthHub_LinkStatus[i],i,g_bEthCamEthLinkStatus[i]);
		//AN is OK, but not linkup
		if(EthCamEthHub_LinkStatus[i]==0 && EthCamEthHub_ANStatus[i]){
			EthCamNet_EthHubLinkDet();
		}
#if (ETHCAM_CHECK_PORTREADY == ENABLE)
		if(EthCamEthHub_LinkStatus[0] && EthCamEthHub_LinkStatus[1]){
			if(!(ipstr2int(SocketInfo[0].ip)==0 && ipstr2int(SocketInfo[1].ip)==0)){
				EthCamNet_EthHubPortIsolate(1, 0);
			}
		}else{
			EthCamNet_EthHubPortIsolate(1, 0);
		}
		EthCamNet_EthHubLinkDet();
#endif
		if(EthCamEthHub_LinkStatus[i] && (g_bEthCamEthLinkStatus[i] !=  (EthCamEthHub_LinkStatus[i]+1))){
			if(ipstr2int(SocketInfo[i].ip)==0){
				#if (ETHCAM_CHECK_PORTREADY == ENABLE)
				DBG_DUMP("SrvConn %d,%d,%d\r\n",EthCamEthHub_LinkStatus[0] ,EthCamEthHub_LinkStatus[1],g_isChkPortReady);
				if(EthCamEthHub_LinkStatus[0] && EthCamEthHub_LinkStatus[1] ){
					DBG_DUMP("EthHub ALL UP\r\n");
					if(ipstr2int(SocketInfo[0].ip)==0 && ipstr2int(SocketInfo[1].ip)==0 && g_isChkPortReady==0){
						EthCamNet_EthHubPortIsolate(1, 0);
						g_isEthHubPortReadyCmdAck=0;
						g_ChkPortReadyIPAddr=((ipstr2int(chCliIPAddr) == 201369792) ? 218147008: 201369792);
						AppBKW_SetData(BKW_ETHCAM_CHECK_PORT_READY_IP,g_ChkPortReadyIPAddr );
						while(BKG_GetExecFuncTable()->event==0){
							Delay_DelayMs(5);
						}
						BKG_PostEvent(NVTEVT_BKW_ETHCAM_CHECKPORT_READY);
						g_isChkPortReady=1;
					}
				}else if(EthCamEthHub_LinkStatus[1] && g_isChkPortReady==0){

					g_isEthHubPortReadyCmdAck=0;
					g_ChkPortReadyIPAddr=((ipstr2int(chCliIPAddr) == 201369792) ? 218147008: 201369792);
					AppBKW_SetData(BKW_ETHCAM_CHECK_PORT_READY_IP,g_ChkPortReadyIPAddr );
					while(BKG_GetExecFuncTable()->event==0){
						Delay_DelayMs(5);
					}
					BKG_PostEvent(NVTEVT_BKW_ETHCAM_CHECKPORT_READY);
					g_isChkPortReady=1;
				}else{
					g_isChkPortReady=1;
				}
				#endif
				strcpy(SocketInfo[i].ip, chCliIPAddr);
				strcpy(SocketInfo[i].mac, chCliMacAddr);
				//DBG_DUMP("Srv SocketInfo[%d].ip=%s\r\n",i,SocketInfo[i].ip);
				snprintf(cmdname, sizeof(cmdname) - 1, "SrvCliConnIPLink %d %d",ipstr2int(SocketInfo[i].ip), (EthCamEthHub_LinkStatus[i] == 1) ? ETHCAM_LINK_UP : ETHCAM_LINK_DOWN);
				CHKPNT;
				EthCamNet_EthLinkStatusNotify(cmdname);
				break;
			}
		}
#if 0
		else{
			if(strcmp(SocketInfo[i].ip, chCliIPAddr)==0){
				DBG_ERR("IP Exist !!SocketInfo[%d].ip=%s\r\n",i,SocketInfo[i].ip);
				break;
			}
		}
#endif
	}
	#else
	strcpy(SocketInfo[0].ip, chCliIPAddr);
	strcpy(SocketInfo[0].mac, chCliMacAddr);
	DBG_DUMP("SocketInfo[0].ip=%s\r\n",SocketInfo[0].ip);
	snprintf(cmdname, sizeof(cmdname) - 1, "SrvCliConnIPLink %d %d",ipstr2int(SocketInfo[0].ip), ETHCAM_LINK_UP);
	CHKPNT;
	EthCamNet_EthLinkStatusNotify(cmdname);
	#endif
}
#if (ETH_REARCAM_CAPS_COUNT>=2)

void EthCamNet_NotifyEthHubConn(ETHCAM_PATH_ID path_id, BOOL link_status)
{
	NVTIPC_SYS_MSG SysMsg;
	CHAR szCmd[40] = {0};

	snprintf(szCmd, sizeof(szCmd) - 1, "ethcamnotifyconn %d %d",path_id,link_status);
	SysMsg.sysCmdID = NVTIPC_SYSCMD_SYSCALL_REQ;
	SysMsg.DataAddr = (UINT32)szCmd;
	SysMsg.DataSize = (UINT32)strlen(szCmd)+1;
	if (NvtIPC_MsgSnd(NVTIPC_SYS_QUEUE_ID, NVTIPC_SENDTO_CORE2, &SysMsg, sizeof(SysMsg)) < 0) {
	    DBG_ERR("NvtIPC_MsgSnd(SYSCALL_REQ)\r\n");
	}
}

UINT32 EthCamNet_GetEthHub_LinkStatus(ETHCAM_PATH_ID path_id)
{
	return EthCamEthHub_LinkStatus[path_id];
}

void EthCamNet_EthHubLinkDetInit(void)
{
	smiInit(MDC_GPIO, MDIO_GPIO);
}
void EthCamNet_DetConnInit(void)
{
	g_bLastTxDet     = FALSE;
	g_bLastTxStatus  = FALSE;
}
static BOOL EthCamNet_DetConn(BOOL ConnSta)
{
	BOOL        bConn=0;
#if (ETHCAM_CHECK_PORTREADY == ENABLE)
	BOOL        bCurTxDet, bCurTxStatus;
	bCurTxDet = ConnSta;
	//DBG_DUMP("^BTx DetPlug > lastDet = %d, currDet = %d\r\n", g_bLastTxDet, bCurTxDet);
	bCurTxStatus = (BOOL)(bCurTxDet & g_bLastTxDet);
	//DBG_DUMP("^BTx DetPlug > last = %d, curr = %d\r\n", g_bLastTxStatus, bCurTxStatus);
	if (bCurTxStatus != g_bLastTxStatus) {
		if (bCurTxStatus == TRUE) {
			bConn= 1;
		}else {
			//DBG_DUMP("^BTx Unplug\r\n");
			bConn= 0;
		}
	}else if ((g_bLastTxDet == TRUE) && (bCurTxDet == FALSE)) {
		//DBG_DUMP("^BTx Fast Unplug 2\r\n");
		bConn= 0;
		bCurTxDet = FALSE;
		bCurTxStatus = (BOOL)(bCurTxDet & g_bLastTxDet);
	}
	g_bLastTxDet     = bCurTxDet;
	g_bLastTxStatus  = bCurTxStatus;
#endif
	return bConn;
}
extern UINT32 g_TestEthHubForceLink[2];
void EthCamNet_EthHubLinkDet(void)
{
	UINT32  read_data[ETHCAM_PATH_ID_MAX]={0};
	UINT16  i, j;
	char cmd[40];

	for(i=0;i<ETHCAM_PATH_ID_MAX;i++){
		smiRead(ETHCAM_ETHHUB_PHY_PORT[i], ETHHUB_LINK_STATUS_REG, &read_data[i]);
		EthCamEthHub_LinkStatus[i]= (read_data[i]==0xffff) ?  0: ((read_data[i] & (1<< ETHHUB_LINK_STATUS_REG_BIT)) >> ETHHUB_LINK_STATUS_REG_BIT);
		if(g_TestEthHubForceLink[i]==1 || g_TestEthHubForceLink[i]==0){
			//DBG_WRN("ForceLink[%d]=%d\r\n",i,g_TestEthHubForceLink[i]);
			EthCamEthHub_LinkStatus[i]=g_TestEthHubForceLink[i];
		}
		EthCamEthHub_ANStatus[i]= (read_data[i]==0xffff) ?  0: ((read_data[i] & (1<< ETHHUB_LINK_AN_COMPLETE_BIT)) >> ETHHUB_LINK_AN_COMPLETE_BIT);
		//DBG_DUMP("port[%d]=0x%x, sta=%d, pre_sta=%d, EthLinkStatus=%d, AN=%d\r\n",ETHCAM_ETHHUB_PHY_PORT[i],read_data[i],EthCamEthHub_LinkStatus[i], EthCamEthHub_PrevLinkStatus[i], g_bEthCamEthLinkStatus[i],EthCamEthHub_ANStatus[i]);
		if(EthCamEthHub_PrevLinkStatus[i] !=EthCamEthHub_LinkStatus[i]){
#if 1
			for (j=0; j<ETH_REARCAM_CAPS_COUNT; j++){
				if(ImageApp_MovieMulti_IsStreamRunning(_CFG_ETHCAM_ID_1+j)){
					DBG_ERR("STOPREC!!!\r\n");
					BKG_PostEvent(NVTEVT_BKW_STOPREC_PROCESS);
					break;
				}
			}
			//stop stream for exist path for hot plugin
			if(EthCamEthHub_LinkStatus[i]){
				for (j=0; j<ETH_REARCAM_CAPS_COUNT; j++){
					if(EthCamNet_GetEthLinkStatus(j)==ETHCAM_LINK_UP){
						if(EthCamNet_GetPrevEthLinkStatus(j)==ETHCAM_LINK_UP && socketCliEthData1_IsRecv(j)){//start stream is OK
							DBG_WRN("stop path id=%d\r\n",j);
							EthCamCmd_GetFrameTimerEn(0);
							EthCam_SendXMLCmd(j, ETHCAM_PORT_DATA1 ,ETHCAM_CMD_TX_STREAM_STOP, 0);
							socketCliEthData1_SetRecv(j, 0);
						}else{ //connecting, stream not start
							DBG_WRN("path id[%d] is connecting\r\n",j);
						}
					}
				}
}
#endif
			if(g_bEthCamEthLinkStatus[i] !=  (EthCamEthHub_LinkStatus[i]+1)){
				//DBG_ERR("SocketInfo[%d].ip=0x%x\r\n",i,ipstr2int(SocketInfo[i].ip));
				if(ipstr2int(SocketInfo[i].ip)){
					if(EthCamEthHub_LinkStatus[i]){
						SxTimer_SetFuncActive(SX_TIMER_ETHCAM_ETHLINKRETRY_ID, TRUE);
					}
					snprintf(cmd, sizeof(cmd) - 1, "EthHubLink %d %d",ipstr2int(SocketInfo[i].ip), (EthCamEthHub_LinkStatus[i] == 1) ? ETHCAM_LINK_UP : ETHCAM_LINK_DOWN);
					CHKPNT;
					EthCamNet_EthLinkStatusNotify(cmd);
				}else{
					//DBG_ERR("SocketInfo[%d].ip=0x%x, HubSta=%d\r\n",i,ipstr2int(SocketInfo[i].ip),EthCamEthHub_LinkStatus[i]);
				}
				//EthCamNet_NotifyEthHubConn(i, (EthCamEthHub_LinkStatus[i] == 1) ? ETHCAM_LINK_UP : ETHCAM_LINK_DOWN);
			}
			EthCamEthHub_PrevLinkStatus[i]=EthCamEthHub_LinkStatus[i];
		}
		//DBG_DUMP("port[%d]=0x%x, sta=%d, EthLinkStatus=%d\r\n",ETHCAM_ETHHUB_PHY_PORT[i],read_data[i],EthCamEthHub_LinkStatus[i], g_bEthCamEthLinkStatus[i]);
	}
	//DBG_DUMP("LinkStatus=%d, %d, %d, %d\r\n",EthCamEthHub_LinkStatus[0], EthCamEthHub_LinkStatus[1] ,g_bEthCamPrevEthLinkStatus[1], g_bEthCamEthLinkStatus[1] );
	if(EthCamEthHub_LinkStatus[0]==0 && EthCamEthHub_LinkStatus[1]  && g_bEthCamPrevEthLinkStatus[1]==ETHCAM_LINK_DOWN && (g_bEthCamEthLinkStatus[1] !=  (EthCamEthHub_LinkStatus[1]+1))){
		 if(g_isChkPortReady==0 && EthCamNet_DetConn(EthCamEthHub_LinkStatus[1])){
			DBG_DUMP("singleTx2\r\n");
			EthCamNet_EthHubPortIsolate(1, 0);
			g_isEthHubPortReadyCmdAck=0;
			AppBKW_SetData(BKW_ETHCAM_CHECK_PORT_READY_IP,0);
			BKG_PostEvent(NVTEVT_BKW_ETHCAM_CHECKPORT_READY);
			g_isChkPortReady=1;
		}
	}		
}
#endif


void EthCamNet_IperfTest(void)
{
	char ipccmd[64];
	NVTIPC_SYS_MSG sysMsg;
	NVTIPC_I32 ipcErr = 0;

	EthCam_SendXMLCmd(ETHCAM_PATH_ID_1, ETHCAM_PORT_DATA1 ,ETHCAM_CMD_TX_STREAM_STOP, 0);
#if (ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
	EthCam_SendXMLCmd(ETHCAM_PATH_ID_1, ETHCAM_PORT_DATA2 ,ETHCAM_CMD_TX_STREAM_STOP, 0);
#endif

	System_ChangeSubMode(SYS_SUBMODE_UPDFW);
	Ux_SendEvent(0, NVTEVT_SYSTEM_MODE, 1, PRIMARY_MODE_UPDFW);

	UI_ClearOSD(CLRID_IDX_BLACK);

	Ux_CloseWindow(&UIFlowWndMovieCtrl, 0);

	Ux_OpenWindow(&UIFlowWndWaitMomentCtrl, 1, UIFlowWndWaitMoment_StatusTXT_Msg_STRID_PLEASE_WAIT);


	snprintf(ipccmd, sizeof(ipccmd) - 1, "ethcamiperf iperf -s");
	sysMsg.sysCmdID = NVTIPC_SYSCMD_SYSCALL_REQ;

	sysMsg.DataAddr = (UINT32)ipccmd;
	sysMsg.DataSize = strlen(ipccmd) + 1;
	if ((ipcErr = NvtIPC_MsgSnd(NVTIPC_SYS_QUEUE_ID, NVTIPC_SENDTO_CORE2, &sysMsg, sizeof(sysMsg))) < 0) {
		DBG_ERR("Failed to NVTIPC_SYS_QUEUE_ID\r\n");
	}
	EthCam_SendXMLCmd(ETHCAM_PATH_ID_1, ETHCAM_PORT_DEFAULT ,ETHCAM_CMD_IPERF_TEST, 0);
	g_bEthCamApp_IperfTest=1;
}
#endif
#endif

void EthCamNet_SetTxIPAddr(void)
{
#if (defined(_NVT_ETHREARCAM_TX_) && (ETH_REARCAM_CAPS_COUNT>=2))

	DBG_DUMP("FL_ETHCAM_TX_IP_ADDR=0x%x, %d.%d.%d.%d\r\n",SysGetFlag(FL_ETHCAM_TX_IP_ADDR), (SysGetFlag(FL_ETHCAM_TX_IP_ADDR) & 0xFF), (SysGetFlag(FL_ETHCAM_TX_IP_ADDR) >> 8) & 0xFF, (SysGetFlag(FL_ETHCAM_TX_IP_ADDR) >> 16) & 0xFF, (SysGetFlag(FL_ETHCAM_TX_IP_ADDR) >> 24) & 0xFF);
	NVTIPC_SYS_MSG SysMsg;
	CHAR szCmd[40] = {0};
	static BOOL bSendIpAddrSuccess=0;

	if(bSendIpAddrSuccess==0){
		snprintf(szCmd, sizeof(szCmd) - 1, "ethcamsettxip %d",SysGetFlag(FL_ETHCAM_TX_IP_ADDR));
		SysMsg.sysCmdID = NVTIPC_SYSCMD_SYSCALL_REQ;
		SysMsg.DataAddr = (UINT32)szCmd;
		SysMsg.DataSize = (UINT32)strlen(szCmd)+1;
		DBG_DUMP("kchk_flg=0x%x\r\n",kchk_flg(ETHCAM_CMD_RCV_FLG_ID, FLG_ETHCAM_CMD_SETIP));
		if (NvtIPC_MsgSnd(NVTIPC_SYS_QUEUE_ID, NVTIPC_SENDTO_CORE2, &SysMsg, sizeof(SysMsg)) < 0) {
			bSendIpAddrSuccess=0;
			DBG_ERR("NvtIPC_MsgSnd(SYSCALL_REQ)\r\n");
			if(kchk_flg(ETHCAM_CMD_RCV_FLG_ID, FLG_ETHCAM_CMD_SETIP)==0){
				EthCamCmdTsk_Open();
				set_flg(ETHCAM_CMD_RCV_FLG_ID,FLG_ETHCAM_CMD_SETIP);
			}
		}else{
			bSendIpAddrSuccess=1;
		}
	}
#endif
}

#include "NvtIpcAPI.h"
#include "FsIpcAPI.h"
#define ETHCAMBOOT_ECOS_VER        20190628
void EthCamNet_Ethboot(void)
{
#if defined(_CPU2_LINUX_)
	char ipccmd[64];
	NVTIPC_SYS_MSG sysMsg;
	NVTIPC_I32 ipcErr = 0;

	snprintf(ipccmd, sizeof(ipccmd) - 1, "udpsvd -vE 0 69 tftpd /mnt/sd/ethcam &");
	sysMsg.sysCmdID = NVTIPC_SYSCMD_SYSCALL_REQ;
	sysMsg.DataAddr = (UINT32)ipccmd;
	sysMsg.DataSize = strlen(ipccmd) + 1;
	if ((ipcErr = NvtIPC_MsgSnd(NVTIPC_SYS_QUEUE_ID, NVTIPC_SENDTO_CORE2, &sysMsg, sizeof(sysMsg))) < 0) {
		DBG_ERR("Failed to NVTIPC_SYS_QUEUE_ID\r\n");
	}
#else
	char ipccmd[64];
	NVTIPC_SYS_MSG sysMsg;
	NVTIPC_I32 ipcErr = 0;
	FSIPC_OPEN_OBJ OpenObj;


	snprintf(ipccmd, sizeof(ipccmd) - 1, "ethcamboot_dhcp");
	sysMsg.sysCmdID = NVTIPC_SYSCMD_SYSCALL_REQ;
	sysMsg.DataAddr = (UINT32)ipccmd;
	sysMsg.DataSize = strlen(ipccmd) + 1;
	if ((ipcErr = NvtIPC_MsgSnd(NVTIPC_SYS_QUEUE_ID, NVTIPC_SENDTO_CORE2, &sysMsg, sizeof(sysMsg))) < 0) {
		DBG_ERR("Failed to NVTIPC_SYS_QUEUE_ID\r\n");
	}

	OpenObj.uiMemPoolAddr = OS_GetMempoolAddr(POOL_ID_FSIPC);
	OpenObj.uiMemPoolSize = OS_GetMempoolSize(POOL_ID_FSIPC);

	if (E_OK != FsIpc_Open(&OpenObj)) {
		DBG_ERR("FsIpc_Open fail\r\n");
		return;
	}

	if (E_OK != FsIpc_Mount("/sdcard")) {
		DBG_ERR("FsIpc_Mount fail\r\n");
		return;
	}

	//snprintf(ipccmd, sizeof(ipccmd) - 1, "ethcamboot -open %d /sdcard/ethcamboot", ETHCAMBOOT_ECOS_VER);
	snprintf(ipccmd, sizeof(ipccmd) - 1, "ethcamboot -open %d", ETHCAMBOOT_ECOS_VER);
	sysMsg.sysCmdID = NVTIPC_SYSCMD_SYSCALL_REQ;
	sysMsg.DataAddr = (UINT32)ipccmd;
	sysMsg.DataSize = strlen(ipccmd) + 1;
	if ((ipcErr = NvtIPC_MsgSnd(NVTIPC_SYS_QUEUE_ID, NVTIPC_SENDTO_CORE2, &sysMsg, sizeof(sysMsg))) < 0) {
		DBG_ERR("Failed to NVTIPC_SYS_QUEUE_ID\r\n");
	}
#endif
}
void EthCamNet_SetSysWdtReset(UINT32 uMS)
{
#if (defined(_NVT_ETHREARCAM_TX_))
	UINT32 uiUserData =  MAKEFOURCC('E','C','T','X');

	wdt_open();
	wdt_setConfig(WDT_CONFIG_ID_USERDATA,uiUserData);
	DBG_DUMP("SET USERDATA=0x%x\r\n",uiUserData);

	wdt_setConfig(WDT_CONFIG_ID_MODE,WDT_MODE_RESET);
	wdt_setConfig(WDT_CONFIG_ID_TIMEOUT,uMS);	//ms
	wdt_trigger();
	wdt_enable();
	DBG_ERR("[WDT] EthCamNet SysWdtReset when after %d ms\r\n",uMS);
#endif
}

void EthCamNet_CheckVDCount(void)
{
#if (defined(_NVT_ETHREARCAM_TX_))
	return;
	IPL_CUR_FRAMECNT_INFO frm_info;

	frm_info.id = IPL_ID_1;
	frm_info.pnext = NULL;
	IPL_GetCmd(IPL_GET_IPL_CUR_FRAMECNT_INFOR, (void *)&frm_info);
	//DBG_DUMP("frame_cnt=%d\r\n",frm_info.frame_cnt);
	//DBG_DUMP("dirbuf_err_cnt=%d\r\n",frm_info.dirbuf_err_cnt);

	g_CheckVDCnt=frm_info.frame_cnt;

	if(g_CheckVDCnt == g_PrevCheckAECnt) {
		g_WDTCnt++;
	}
	else{
		g_PrevCheckAECnt = g_CheckVDCnt;
		g_WDTCnt=0;
	}
	if (g_WDTCnt >3)
	{
		g_WDTCnt= 0;
		if(g_isWDTReset == FALSE)
		{
			EthCamNet_SetSysWdtReset(500);
			g_isWDTReset = TRUE;
		}
	}
#endif
}
