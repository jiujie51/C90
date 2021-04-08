////////////////////////////////////////////////////////////////////////////////
#include "SysCommon.h"
#include "AppCommon.h"
#include "NvtIpcAPI.h"
#include "SxCmd.h"
////////////////////////////////////////////////////////////////////////////////

//global debug level: PRJ_DBG_LVL
#include "PrjCfg.h"
#include "SysCfg.h"
//local debug level: THIS_DBGLVL
#define THIS_DBGLVL         2 // 0=FATAL, 1=ERR, 2=WRN, 3=UNIT, 4=FUNC, 5=IND, 6=MSG, 7=VALUE, 8=USER
///////////////////////////////////////////////////////////////////////////////
#define __MODULE__          SysIPCExe
#define __DBGLVL__          ((THIS_DBGLVL>=PRJ_DBG_LVL)?THIS_DBGLVL:PRJ_DBG_LVL)
#define __DBGFLT__          "*" //*=All, [mark]=CustomClass
#include "DebugModule.h"
///////////////////////////////////////////////////////////////////////////////
#if (PTY_FUNC == ENABLE)
#include "nvt_ivot_ptyd_api.h"
#endif

extern ER efuse_get_unique_id(UINT32 * id_L, UINT32 * id_H);

#ifdef __ECOS
#include <cyg/infra/maincmd.h>
static BOOL CPU2_Cmd(CHAR *pStr)
{
	if (pStr[0] == '?') {
		listmaincmd();
	} else {
		runmaincmd(pStr);
	}
	return TRUE;
}
#elif defined(_CPU2_LINUX_)
static BOOL CPU2_Cmd(CHAR *pStr)
{
	static char cmd[256] = {0};
	NVTIPC_SYS_MSG sysMsg;

	strncpy(cmd, pStr, sizeof(cmd)-1);
	strncat(cmd, " &", sizeof(cmd)-1);
	sysMsg.sysCmdID = NVTIPC_SYSCMD_SYSCALL_REQ;
	sysMsg.DataAddr = (UINT32)cmd;
	sysMsg.DataSize = strlen(cmd) + 1;
	if (NvtIPC_MsgSnd(NVTIPC_SYS_QUEUE_ID, NVTIPC_SENDTO_CORE2, &sysMsg, sizeof(sysMsg)) < 0) {
		DBG_ERR("NvtIPC_MsgSnd\r\n");
	}
	return TRUE;
}
#endif


#if defined(_CPU2_ECOS_)
static BOOL eCos2_Cmd(CHAR *pStr)
{
	NVTIPC_SYS_MSG sysMsg;
	sysMsg.sysCmdID = NVTIPC_SYSCMD_UART_REQ;
	sysMsg.DataAddr = (UINT32)pStr;
	sysMsg.DataSize = strlen(pStr) + 1;

	if (NvtIPC_MsgSnd(NVTIPC_SYS_QUEUE_ID, NVTIPC_SENDTO_CORE2, &sysMsg, sizeof(sysMsg)) < 0) {
		DBG_ERR("NvtIPC_MsgSnd\r\n");
	}
	return TRUE;
}
#endif

#if defined(_DSP1_FREERTOS_)
BOOL DSP1_Cmd(CHAR *pStr)
{
	NVTIPC_SYS_MSG sysMsg;

	sysMsg.sysCmdID = NVTIPC_SYSCMD_UART_REQ;
	sysMsg.DataAddr = (UINT32)pStr;
	sysMsg.DataSize = strlen(pStr) + 1;
	if (NvtIPC_MsgSnd(NVTIPC_SYS_QUEUE_ID, NVTIPC_SENDTO_CORE3, &sysMsg, sizeof(sysMsg)) < 0) {
		DBG_ERR("NvtIPC_MsgSnd\r\n");
	}
	return TRUE;
}
#endif
#if defined(_DSP2_FREERTOS_)
BOOL DSP2_Cmd(CHAR *pStr)
{
	NVTIPC_SYS_MSG sysMsg;

	sysMsg.sysCmdID = NVTIPC_SYSCMD_UART_REQ;
	sysMsg.DataAddr = (UINT32)pStr;
	sysMsg.DataSize = strlen(pStr) + 1;
	if (NvtIPC_MsgSnd(NVTIPC_SYS_QUEUE_ID, NVTIPC_SENDTO_CORE4, &sysMsg, sizeof(sysMsg)) < 0) {
		DBG_ERR("NvtIPC_MsgSnd\r\n");
	}
	return TRUE;
}
#endif

static void system_poweroff(void)
{
	DBG_IND("system_poweroff\r\n");
	Ux_PostEvent(NVTEVT_SYSTEM_SHUTDOWN, 1, 0);
}

static void system_reboot(void)
{
	DBG_IND("system_reboot\r\n");
#if !defined(_CPU1_ECOS_) && (IPCAM_FUNC == ENABLE)
	//IPCam, do nothing
#elif defined(_CLOCK_RTC_)
	GxSystem_EnableHWReset(0);
#elif defined(_CLOCK_DRTC_)
	GxSystem_EnableSWReset(0); //it may cause damage on peripheral, cause their power are not be reset here.
#else
	#error (unknown CLOCK setting?)
#endif
	Ux_PostEvent(NVTEVT_SYSTEM_SHUTDOWN, 1, 0);
}
#include "EthCamAppNetwork.h"
#if (defined(_NVT_ETHREARCAM_TX_) || defined(_NVT_ETHREARCAM_RX_))
#include "EthCamAppCmd.h"
BOOL bSendIpcReadySuccess=0;
BOOL bEthLinkNotifyAckSuccess=0;
BOOL bNotifyChipIdSuccess=0;
static SYSCALL_CMD_TBL ipccmd_tbl[] =
{
	{"ethlinknotify", (SYSCALL_CMD_FP)EthCamNet_EthLinkStatusNotify},
#if (defined(_NVT_ETHREARCAM_RX_))// && ETH_REARCAM_CAPS_COUNT>=2)
	{"cliconnipnotify", (SYSCALL_CMD_FP)EthCamNet_SrvCliConnIPAddrNofity},
#endif
	{"", NULL}                                                                  // this item should be the last item
};
#endif
void System_IPCSendReady(void)
{
#if (defined(_NVT_ETHREARCAM_RX_) && !defined(_CPU2_LINUX_))
	NVTIPC_SYS_MSG sysMsg;
	NVTIPC_I32 ipcErr = 0;
	if(bSendIpcReadySuccess==FALSE){
		char ipccmd[64];
		snprintf(ipccmd, sizeof(ipccmd) - 1, "ethcamipcready");
		sysMsg.sysCmdID = NVTIPC_SYSCMD_SYSCALL_REQ;
		sysMsg.DataAddr = (UINT32)ipccmd;
		sysMsg.DataSize = strlen(ipccmd) + 1;
		if ((ipcErr = NvtIPC_MsgSnd(NVTIPC_SYS_QUEUE_ID, NVTIPC_SENDTO_CORE2, &sysMsg, sizeof(sysMsg))) < 0) {
			//DBG_ERR("Failed to NVTIPC_SYS_QUEUE_ID\r\n");
			bSendIpcReadySuccess=FALSE;
		}else{
			bSendIpcReadySuccess=TRUE;
		}
	}
#endif
#if (defined(_NVT_ETHREARCAM_TX_))
	NVTIPC_SYS_MSG sysMsg;
	NVTIPC_I32 ipcErr = 0;
	if(bEthLinkNotifyAckSuccess==FALSE){
		sysMsg.sysCmdID = NVTIPC_SYSCMD_SYSCALL_ACK;
		sysMsg.DataAddr = 0;
		sysMsg.DataSize = 0;
		if ((ipcErr = NvtIPC_MsgSnd(NVTIPC_SYS_QUEUE_ID, NVTIPC_SENDTO_CORE2, &sysMsg, sizeof(sysMsg))) < 0) {
			//DBG_ERR("Failed to EthLinkNotifyAck\r\n");
			bEthLinkNotifyAckSuccess=FALSE;
		}else{
			//DBG_DUMP("OK to EthLinkNotifyAck\r\n");
			bEthLinkNotifyAckSuccess=TRUE;
			System_IPCSendChipId();
		}
	}
#endif
}
void System_IPCSendChipId(void)
{
#if (defined(_NVT_ETHREARCAM_TX_) && (ETH_REARCAM_CAPS_COUNT>=2))
	NVTIPC_SYS_MSG sysMsg;
	NVTIPC_I32 ipcErr = 0;
	if(bNotifyChipIdSuccess==FALSE){
		UINT32 ChipId_h=0, ChipId_l=0;
		if(efuse_get_unique_id(&ChipId_l, &ChipId_h) >= 0){
			//DBG_DUMP("unique ID[0x%08x][0x%08x] success\r\n", ChipId_h, ChipId_l);
		}else{
		#if 0//(defined(_MODEL_ETHCAM1_TX_64M_) || defined(_MODEL_ETHCAM2_TX_64M_) || defined(_MODEL_ETHCAM2_TX_64M_Alg_))
			ChipId_h=0x12345678;
			ChipId_l=0x87654321;
		#else
			DBG_ERR("unique ID[0x%08x][0x%08x] error\r\n", ChipId_h, ChipId_l);
			return;
		#endif
		}
		char ipccmd[64];
		snprintf(ipccmd, sizeof(ipccmd) - 1, "chipid %d %d",ChipId_h,ChipId_l);
		sysMsg.sysCmdID = NVTIPC_SYSCMD_SYSCALL_REQ;
		sysMsg.DataAddr = (UINT32)ipccmd;
		sysMsg.DataSize = strlen(ipccmd) + 1;
		if ((ipcErr = NvtIPC_MsgSnd(NVTIPC_SYS_QUEUE_ID, NVTIPC_SENDTO_CORE2, &sysMsg, sizeof(sysMsg))) < 0) {
			//DBG_ERR("Failed to SendChipId\r\n");
			bNotifyChipIdSuccess=FALSE;
		}else{
			//DBG_DUMP("OK to SendChipId\r\n");
			bNotifyChipIdSuccess=TRUE;
		}
	}
#endif
}
void System_OnIPC(void)
{
	NVTIPC_OPEN_OBJ   openObj = {0};
	TM_BOOT_BEGIN("ipc", "init");

	openObj.workBuffAddr = OS_GetMempoolAddr(POOL_ID_NVTIPC);
	openObj.workBuffSize = OS_GetMempoolSize(POOL_ID_NVTIPC);
#if (defined(_NVT_ETHREARCAM_TX_) || defined(_NVT_ETHREARCAM_RX_))
	openObj.SysCmdTbl = ipccmd_tbl;
#else
	openObj.SysCmdTbl = NULL;
#endif
	openObj.uartCmdFp = (UART_CMD_FP)SxCmd_DoCommand;
	openObj.poweroffFp = system_poweroff;
	openObj.rebootFp = system_reboot;
#if defined(_CPU2_LINUX_) || defined(_CPU2_ECOS_)
	openObj.bIsSupportCoresSem = TRUE;
#endif
	NvtIPC_Open(&openObj);

#if defined(_CPU2_LINUX_) || defined(__ECOS)
	SxCmd_RegCmdMode(SX_MODE_CPU2, "CPU2", '#', CPU2_Cmd);
#endif
#if defined(_CPU2_ECOS_)
	SxCmd_RegCmdMode(SX_MODE_DSP2, "eCos2", '&', eCos2_Cmd);
#endif
#if defined(_DSP1_FREERTOS_)
	SxCmd_RegCmdMode(SX_MODE_DSP1, "DSP1", '@', DSP1_Cmd);
#endif
#if defined(_DSP2_FREERTOS_)
	SxCmd_RegCmdMode(SX_MODE_DSP2, "DSP2", '&', DSP2_Cmd);
#endif

#if (PTY_FUNC == ENABLE)
	nvt_ivot_ptyd_open();
#endif
	System_IPCSendReady();
	TM_BOOT_END("ipc", "init");
}
