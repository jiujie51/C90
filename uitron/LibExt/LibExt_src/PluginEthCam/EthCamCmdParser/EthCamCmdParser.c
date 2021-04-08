#include "EthCamCmdParser.h"
#include "FileSysTsk.h"
#include "stdio.h"
#include "NvtVerInfo.h"
#include "EthCamCmdParserInt.h"
#include "EthCamSocket.h"

NVTVER_VERSION_ENTRY(EthCamCmdParser, 1, 00, 000, 00)

#define ETHCAMCMD_STRCPY(dst, src, dst_size)  do { strncpy(dst, src, dst_size-1); dst[dst_size-1] = '\0'; } while(0)
#define ETHCAMCMD_MIMETYPE_MAXLEN             40   ///<  mime type max length,refer to CYG_HFS_MIMETYPE_MAXLEN


static ETHCAM_CMD_ENTRY *g_pCmdTab;
static EthCamCmd_DefCB *gDefReturnFormat = 0;
static EthCamCmd_EventHandle *gEventHandle = 0;
static EthCamCmd_APPStartupCheck *gAppStartupChecker = NULL;
static UINT32 g_result[ETHCAM_PATH_ID_MAX] = {0};
static UINT32 g_receiver = 0;
static UINT32 g_app_startup_cmd = 0;
static UINT32 g_curEthCamCmd = 0;
static char   g_parStr[ETHCAM_PAR_STR_LEN];
extern UINT32 EthCamCmd_WaitFinish(UINT32 path_id, FLGPTN waiptn);
extern void   EthCamCmd_Lock(UINT32 path_id);
extern void   EthCamCmd_Unlock(UINT32 path_id);
typedef enum {
	ETHCAMCMD_PAR_NULL = 0,
	ETHCAMCMD_PAR_NUM,
	ETHCAMCMD_PAR_STR,
	ENUM_DUMMY4WORD(ETHCAMCMD_PAR_TYPE)
} ETHCAMCMD_PAR_TYPE;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void EthCamCmd_ReceiveCmd(UINT32 enable)
{
	DBG_IND("EthCamCmd_ReceiveCmd %d\r\n", enable);
	UINT16 i;
	if (enable) {
		if (g_receiver) {
			DBG_WRN("is enabled\r\n");
			return ;
		}

		if ((!g_pCmdTab) || (!gEventHandle)) {
			DBG_ERR("no Cmd Table or event handle\r\n");
			return;
		}
		for(i=0;i<ETHCAM_PATH_ID_MAX;i++){
			clr_flg(FLG_ID_ETHCAMCMD[i], 0xFFFFFFFF);
		}

		g_receiver = enable;
	} else {
		if (!g_receiver) {
			DBG_WRN("is disabled\r\n");
			return ;
		}

		g_receiver = enable;
		//UI want to close,some app cmd might wait UI finish
		//can not wait flag by UI,therefore set flag for app cmd
		for(i=0;i<ETHCAM_PATH_ID_MAX;i++){
			EthCamCmd_Done(i, 0xFFFFFFFF, ETHCAM_CMD_TERMINATE);
		}
	}
}

void EthCamCmd_UnlockString(UINT32 path_id, char *string)
{
	sig_sem(ETHCAMSTR_SEM_ID[path_id]);
}

char *EthCamCmd_LockString(UINT32 path_id)
{
	wai_sem(ETHCAMSTR_SEM_ID[path_id]);
	return g_parStr;
}

void EthCamCmd_Lock(UINT32 path_id)
{
	DBG_IND("Lock\r\n");
	wai_sem(ETHCAMCMD_SEM_ID[path_id]);
}
void EthCamCmd_Unlock(UINT32 path_id)
{
	DBG_IND("Unlock\r\n");
	sig_sem(ETHCAMCMD_SEM_ID[path_id]);
}
void EthCamCmd_Done(UINT32 path_id, FLGPTN flag, UINT32 result)
{
	DBG_IND("flag %x result %d\r\n", flag, result);
	g_result[path_id] = result;
	set_flg(FLG_ID_ETHCAMCMD[path_id], flag);
}

UINT32 EthCamCmd_WaitFinish(UINT32 path_id, FLGPTN waiptn)
{
	FLGPTN uiFlag;

	DBG_IND("waiptn %x\r\n", waiptn);
	wai_flg(&uiFlag, FLG_ID_ETHCAMCMD[path_id], waiptn, (TWF_ORW));
	return g_result[path_id];
}

void EthCamCmd_ClrFlg(UINT32 path_id, FLGPTN flag)
{
	DBG_IND("clear %x\r\n", flag);
	clr_flg(FLG_ID_ETHCAMCMD[path_id], flag);

}
void EthCamCmd_SetExecTable(ETHCAM_CMD_ENTRY *pAppCmdTbl)
{
	g_pCmdTab = pAppCmdTbl;
}
ETHCAM_CMD_ENTRY *EthCamCmd_GetExecTable(void)
{
	return g_pCmdTab;
}
void EthCamCmd_SetDefautCB(UINT32 defaultCB)
{
	gDefReturnFormat = (EthCamCmd_DefCB *)defaultCB;
}
void EthCamCmd_SetEventHandle(UINT32 eventHandle)
{
	gEventHandle = (EthCamCmd_EventHandle *)eventHandle;
}
void EthCamCmd_SetAppStartupChecker(UINT32 app_startupchecker)
{
	gAppStartupChecker = (EthCamCmd_APPStartupCheck *)app_startupchecker;
}
void EthCamCmd_SetAppStartupCmdCode(UINT32 cmd_code)
{
	g_app_startup_cmd = cmd_code;
}


UINT32 EthCamCmd_GetCurCmd(void)
{
	return g_curEthCamCmd;
}
static UINT32 EthCamCmd_DispatchCmd(UINT32 path_id, UINT32 cmd, ETHCAMCMD_PAR_TYPE parType, UINT32 par, UINT32 *UserCB)
{
	int i = 0;
	UINT32 ret = 0;

	g_curEthCamCmd = 0;

	if (g_receiver) {
		if (!gAppStartupChecker) {
			DBG_ERR("Call EthCamCmd_SetAppStartupChecker() first!\r\n");
			return ETHCAM_CMD_NOT_FOUND;
		}

		if (gAppStartupChecker() || cmd == g_app_startup_cmd) {
			while (g_pCmdTab[i].cmd != 0) {
				if (cmd == g_pCmdTab[i].cmd) {
					//UINT32 t1,t2;
					//t1 = Perf_GetCurrent()/1000;
					DBG_IND("cmd:%d evt:%x par:%d CB:%x wait:%x\r\n", g_pCmdTab[i].cmd, g_pCmdTab[i].event, par, g_pCmdTab[i].usrCB, g_pCmdTab[i].Waitflag);
					g_curEthCamCmd = cmd;
					if (g_pCmdTab[i].Waitflag) {
						clr_flg(FLG_ID_ETHCAMCMD[path_id], g_pCmdTab[i].Waitflag);
						g_result[path_id] = 0;
					}
					if (gEventHandle && g_pCmdTab[i].event) {
						if (parType == ETHCAMCMD_PAR_NUM) {
							gEventHandle(g_pCmdTab[i].event, 1, par);
						} else if (parType == ETHCAMCMD_PAR_STR) {
							char  *parStr;//post event,data in stack would release
							parStr = EthCamCmd_LockString(path_id);
							memset(parStr, '\0', ETHCAM_PAR_STR_LEN);
							sscanf_s((char *)par, "%s", parStr, ETHCAM_PAR_STR_LEN);
							gEventHandle(g_pCmdTab[i].event, 1, parStr);
						} else {
							gEventHandle(g_pCmdTab[i].event, 0);
						}
					}
					if (g_pCmdTab[i].Waitflag) {
						ret = EthCamCmd_WaitFinish(path_id, g_pCmdTab[i].Waitflag);
					}
					if (g_pCmdTab[i].usrCB) {
						*UserCB = (UINT32)g_pCmdTab[i].usrCB;
					}
					//t2 = Perf_GetCurrent()/1000;
					//DBG_ERR("time %d ms\r\n",t2-t1);
					DBG_IND("ret %d\r\n", ret);
					g_curEthCamCmd = 0;
					return ret;
				}
				i++;
			}
		} else {
			DBG_ERR("should perform ETHCAMAPP_CMD_APP_STARTUP first!\r\n");
			return ETHCAM_CMD_STATE_ERR;
		}
	} else {
		DBG_ERR("should EthCamCmd_ReceiveCmd enable\r\n");
	}
	return ETHCAM_CMD_NOT_FOUND;

}
INT32 EthCamCmd_GetData(UINT32 path_id, char *path, char *argument, UINT32 bufAddr, UINT32 *bufSize, char *mimeType, UINT32 segmentCount)
{
	UINT32            cmd = 0, par = 0;
	char             *pch = 0;
	UINT32            ret = 0;
	UINT32            UserCB = 0;
	//char              *parStr;//post event,data in stack would release
	DBG_IND("Data2 path = %s, argument -> %s, mimeType= %s, bufsize= %d, segmentCount= %d\r\n", path, argument, mimeType, *bufSize, segmentCount);


	if (segmentCount == ETHCAM_CMD_GETDATA_SEGMENT_ERROR_BREAK) {
		DBG_ERR("segmentCount %d\r\n", segmentCount);
		EthCamCmd_Unlock(path_id);
		return ETHCAM_CMD_GETDATA_RETURN_ERROR;
	}

	if (strncmp(argument, CMD_STR, strlen(CMD_STR)) == 0) {
		if (segmentCount == 0) {
			EthCamCmd_Lock(path_id);
		}
		sscanf_s(argument + strlen(CMD_STR), "%d", &cmd);
		pch = strchr(argument + strlen(CMD_STR), '&');
		if (pch) {
			if (strncmp(pch, PAR_STR, strlen(PAR_STR)) == 0) {
				sscanf_s(pch + strlen(PAR_STR), "%d", &par);
				ret = EthCamCmd_DispatchCmd(path_id, cmd, ETHCAMCMD_PAR_NUM, par, &UserCB);
			} else if (strncmp(pch, PARS_STR, strlen(PARS_STR)) == 0) {
				//DBG_ERR("%s\r\n",pch+strlen(PARS_STR));
				ret = EthCamCmd_DispatchCmd(path_id ,cmd, ETHCAMCMD_PAR_STR, (UINT32)pch + strlen(PARS_STR), &UserCB);
			}
		} else {
			ret = EthCamCmd_DispatchCmd(path_id, cmd, ETHCAMCMD_PAR_NULL, 0, &UserCB);
		}
		if (UserCB) {
			UINT32 hfs_result;
			hfs_result = ((EthCamCmd_getCustomData *)UserCB)(path, argument, bufAddr, bufSize, mimeType, segmentCount);

			if (hfs_result != ETHCAM_CMD_GETDATA_RETURN_CONTINUE && hfs_result != ETHCAM_CMD_GETDATA_RETURN_CONTI_NEED_ACKDATA) {
				EthCamCmd_Unlock(path_id);
			}
			return hfs_result;
		} else { //default return value xml
			if (gDefReturnFormat) {
				gDefReturnFormat(cmd, ret, bufAddr, bufSize, mimeType);
				EthCamCmd_Unlock(path_id);
				return ETHCAM_CMD_GETDATA_RETURN_OK;
			} else {
				DBG_ERR("no default CB\r\n");
				EthCamCmd_Unlock(path_id);
				return ETHCAM_CMD_GETDATA_RETURN_ERROR;
			}
		}
	} else { //for test,list url command
		UINT32            len = 0;
		char             *buf = (char *)bufAddr;
		FST_FILE          filehdl = 0;
		char              pFilePath[32];
		UINT32            fileLen = *bufSize;

		snprintf(pFilePath, sizeof(pFilePath), "%s", HTML_PATH); //html of all command list
		filehdl = FileSys_OpenFile(pFilePath, FST_OPEN_READ);
		if (filehdl) {
			// set the data mimetype
			ETHCAMCMD_STRCPY(mimeType, "text/html", ETHCAMCMD_MIMETYPE_MAXLEN);
			FileSys_ReadFile(filehdl, (UINT8 *)buf, &fileLen, 0, 0);
			FileSys_CloseFile(filehdl);
			*bufSize = fileLen;
			*(buf + fileLen) = '\0';
		} else {
			ETHCAMCMD_STRCPY(mimeType, "text/html", ETHCAMCMD_MIMETYPE_MAXLEN);
			len = snprintf(buf, *bufSize, "no %s file", HTML_PATH);
			buf += len;
			*bufSize = (UINT32)(buf) - bufAddr;

		}
	}
	return ETHCAM_CMD_GETDATA_RETURN_OK;
}
INT32 EthCamCmd_PutData(UINT32 path_id, char *path, char *argument, UINT32 bufAddr, UINT32 bufSize, UINT32 segmentCount, UINT32 putStatus)  ///< Callback function for put custom data.
{
	UINT32            cmd = 0, par = 0;
	char             *pch = 0;
	UINT32            ret = 0;
	UINT32            UserCB = 0;

	DBG_IND("path =%s, argument = %s, bufAddr = 0x%x, bufSize =0x%x , segmentCount  =%d , putStatus = %d\r\n", path, argument, bufAddr, bufSize, segmentCount, putStatus);

	if (strncmp(argument, CMD_STR, strlen(CMD_STR)) == 0) {
		if (segmentCount == 0) {
			EthCamCmd_Lock(path_id);
		}
		sscanf_s(argument + strlen(CMD_STR), "%d", &cmd);
		pch = strchr(argument + strlen(CMD_STR), '&');
		if (pch) {
			if (strncmp(pch, PAR_STR, strlen(PAR_STR)) == 0) {
				sscanf_s(pch + strlen(PAR_STR), "%d", &par);
				ret = EthCamCmd_DispatchCmd(path_id, cmd, ETHCAMCMD_PAR_NUM, par, &UserCB);
			} else if (strncmp(pch, PARS_STR, strlen(PARS_STR)) == 0) {
				ret = EthCamCmd_DispatchCmd(path_id, cmd, ETHCAMCMD_PAR_STR, (UINT32)pch + strlen(PARS_STR), &UserCB);
			}
		} else {
			ret = EthCamCmd_DispatchCmd(path_id, cmd, ETHCAMCMD_PAR_NULL, 0, &UserCB);
		}

		if ((ret == 0) && (UserCB)) {
			UINT32 hfs_result;
			hfs_result = ((EthCamCmd_puttCustomData *)UserCB)(path, argument, bufAddr, bufSize, segmentCount, putStatus);

			if (putStatus == ETHCAM_CMD_PUT_STATUS_FINISH) {
				EthCamCmd_Unlock(path_id);
			}
			return hfs_result;
		} else {
			DBG_ERR("no default CB\r\n");
			EthCamCmd_Unlock(path_id);
			return ETHCAM_CMD_UPLOAD_FAIL_WRITE_ERROR;
		}
	}

	return ETHCAM_CMD_UPLOAD_OK;
}
