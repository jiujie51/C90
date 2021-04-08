////////////////////////////////////////////////////////////////////////////////
#include "SysCommon.h"
////////////////////////////////////////////////////////////////////////////////
#include "UIFramework.h"
#include "UIBackgroundObj.h"
#include "Utility.h"
#include "DpofVendor.h"

#include "FileSysTsk.h"
#include "DCF.h"
#include "UIInfo.h"
#include "StrgDef.h"
#include "UIPlayComm.h"

//#NT#2012/10/30#Calvin Chang#Generate Audio NR pattern by noises of zoom, focus and iris -begin
#include "UIAppMovie.h"
#include "UIAppPhoto.h"
#include "Lens.h"
//#NT#2012/10/30#Calvin Chang -end
#include "UIAppNetwork.h"
#include "DxType.h"
#include "Dx.h"
#include "PlaybackTsk.h"
//#include "DialogWarn.h"
#include "FileDB.h"
#include "dma.h"
#include "UISetup.h"
//#NT#2016/05/30#Lincy Lin -begin
//#NT#Support logfile function
#if (LOGFILE_FUNC==ENABLE)
#include "LogFile.h"
extern BOOL bSysLogfile;
#endif
#if(UPDFW_MODE==ENABLE)
#include "UIModeUpdFw.h"
#endif
#if(UPDFW_MODE==ENABLE)
#include "UIModeUpdFw.h"
#include "SysMain.h"
#endif
#include "UIFlowMovieFuncs.h"
#if (defined(_NVT_ETHREARCAM_TX_) || defined(_NVT_ETHREARCAM_RX_))
#include "WifiAppCmd.h"
#include "EthCamAppCmd.h"
#include "ImageApp_MovieMulti.h"

#include "EthsockCliIpcAPI.h"
#include "EthCamSocket.h"
#include "EthCamAppSocket.h"
#include "EthCamAppNetwork.h"
#include "NvtIpcAPI.h"
#endif
#include "EthCamSocket.h"

//#NT#2016/05/30#Lincy Lin -end
///////////////////////////////////////////////////////////////////////////////
#define __MODULE__          UIBKGObj
#define __DBGLVL__          6 // 0=FATAL, 1=ERR, 2=WRN, 3=UNIT, 4=FUNC, 5=IND, 6=MSG, 7=VALUE, 8=USER
#define __DBGFLT__          "*" //*=All, [mark]=CustomClass
#include "DebugModule.h"
///////////////////////////////////////////////////////////////////////////////
#define PB_COPY2CARD_NEW_DIR            0x00
#define PB_COPY2CARD_APPEND             0x01
#define PB_COPY2CARD_NO_RENAME          0x02
#define PB_COPY2CARD_NEW_DIR_ORG_FILE   0x03

static UINT32 Background_ProtectAll(BOOL bLock); //for any mode protect/unproteck all
static UINT32 Background_DeleteAll(void); //for any mode delete all
static UINT32 BackgroundDelAll(void);
static UINT32 BackgroundCopy2Card(void);
#if(COPYCARD2CARD_FUNCTION == ENABLE)
static UINT32 BackgroundCopyCard1ToCard2(void);
static UINT32 BackgroundCopyCard2ToCard1(void);
#endif
static UINT32 BackgroundFormat(void);
static UINT32 BackgroundFormatCard(void);
static UINT32 BackgroundFormatNand(void);
static UINT32 BackgroundNumReset(void);
#if (USE_DPOF==ENABLE)
static UINT32 BackgroundSetDPOF(void);
#endif
static UINT32 BackgroundSetProtect(void);
static UINT32 BackgroundDummyWait(void);
static UINT32 BackgroundFWUpdate(void);
#if (PHOTO_MODE==ENABLE)
static UINT32 BackgroundDZoomIn(void);
static UINT32 BackgroundDZoomOut(void);
#endif
static UINT32 BackgroundPIMProcess(void);
//#NT#2012/10/30#Calvin Chang#Generate Audio NR pattern by noises of zoom, focus and iris -begin
static UINT32 BackgroundAudiNRLensAction(void);
//#NT#2012/10/30#Calvin Chang -end
static UINT32 BackgroundWiFiOn(void);
#if(WIFI_AP_FUNC==ENABLE)
static UINT32 BackgroundWiFiClearACL(void);
#endif
static UINT32 BackgroundStopRecProcess(void);
#if (defined(_NVT_ETHREARCAM_TX_) || defined(_NVT_ETHREARCAM_RX_))
static UINT32 BackgroundEthCamSocketOpen(void);
static UINT32 BackgroundEthCamSocketCliCmdOpen(void);
static UINT32 BackgroundEthCamSocketCliDataOpen(void);
static UINT32 BackgroundEthCamGetTxInfo(void);
static UINT32 BackgroundEthCamTriggerThumb(void);
static UINT32 BackgroundEthCamTxStreamStart(void);
static UINT32 BackgroundEthCamTxDecInfo(void);
static UINT32 BackgroundEthCamTxRecInfo(void);
static UINT32 BackgroundEthCamTxDispOpenSocketStreamStart(void);
static UINT32 BackgroundEthCamTxDispOpenSocketStream(void);
static UINT32 BackgroundEthCamTxRecOpenSocketStreamStart(void);
static UINT32 BackgroundEthCamSyncTime(void);
static UINT32 BackgroundEthCamDecErr(void);
static UINT32 BackgroundEthCamDecErrRestart(void);
static UINT32 BackgroundEthCamRawEncodeResult(void);
static UINT32 BackgroundEthCamSetTxSysInfo(void);
static UINT32 BackgroundEthCamSetTxSlideUp(void);
static UINT32 BackgroundEthCamSetTxSlideDown(void);
static UINT32 BackgroundEthCamEthLinkNotify(void);
static UINT32 BackgroundEthCamIperfTest(void);
static UINT32 BackgroundEthCamCheckPortReady(void);
static UINT32 BackgroundEthCamUpdateUIInfo(void);
static UINT32 BackgroundEthCamFWUpdate(void);
#endif
#if (AUDPROC_FUNC == ENABLE)
static UINT32 BackgroundAudProcInit(void);
#endif

static UINT32 g_uiDpofOPMode = PLAYDPOF_SETONE;
static UINT32 g_uiDpofPrtNum = 0;
static UINT32 g_uiDpofDateOn = FALSE;
static UINT32 g_uiProtectType = SETUP_PROTECT_ALL;
static UINT32 g_uiWaitTime = 1;
static UINT32 *g_pDzoomStop = NULL;
static UINT32 g_uiEthcamTriggerThumbPathId[ETHCAM_PATH_ID_MAX] = {0};
static UINT32 g_uiEthcamDecErrPathId = 0;
static UINT32 g_uiEthcamEthLinkNotifySta = 0;
static UINT32 g_uiEthcamCheckPortReadyIPAddr = 0;

UINT32 gUIStopBckGndDummyWait = FALSE;
#if (defined(_NVT_ETHREARCAM_TX_))
extern int SX_TIMER_ETHCAM_LINKDET_ID;
#endif
#if (defined(_NVT_ETHREARCAM_RX_))
extern int SX_TIMER_ETHCAM_ETHLINKRETRY_ID;
#endif
#if (USE_FILEDB==ENABLE)

static FILEDB_INIT_OBJ gBKFDBInitObj = {
	"A:\\",  //rootPath
	NULL,  //defaultfolder
	NULL,  //filterfolder
	TRUE,  //bIsRecursive
	TRUE,  //bIsCyclic
	TRUE,  //bIsMoveToLastFile
	TRUE, //bIsSupportLongName
	FALSE, //bIsDCFFileOnly
	TRUE,  //bIsFilterOutSameDCFNumFolder
	{'N', 'V', 'T', 'I', 'M'}, //OurDCFDirName[5]
	{'I', 'M', 'A', 'G'}, //OurDCFFileName[4]
	FALSE,  //bIsFilterOutSameDCFNumFile
	FALSE, //bIsChkHasFile
	60,    //u32MaxFilePathLen
	10000,  //u32MaxFileNum
	(FILEDB_FMT_JPG | FILEDB_FMT_AVI | FILEDB_FMT_MOV | FILEDB_FMT_MP4 | FILEDB_FMT_TS), //fileFilter
	0,     //u32MemAddr
	0,     //u32MemSize
	NULL   //fpChkAbort

};
#endif
BKG_JOB_ENTRY gBackgroundExtFuncTable[] = {
	{NVTEVT_BKW_DELALL,             BackgroundDelAll},
	{NVTEVT_BKW_COPY2CARD,          BackgroundCopy2Card},
#if(COPYCARD2CARD_FUNCTION == ENABLE)
	{NVTEVT_BKW_COPYCARD1ToCARD2,   BackgroundCopyCard1ToCard2},
	{NVTEVT_BKW_COPYCARD2ToCARD1,   BackgroundCopyCard2ToCard1},
#endif
	{NVTEVT_BKW_FORMAT,             BackgroundFormat},
	{NVTEVT_BKW_FORMAT_CARD,        BackgroundFormatCard},
	{NVTEVT_BKW_FORMAT_NAND,        BackgroundFormatNand},
	{NVTEVT_BKW_NUM_RESET,          BackgroundNumReset},
#if (USB_MODE==ENABLE)
#if (USE_DPOF==ENABLE)
	{NVTEVT_BKW_SETDPOF,            BackgroundSetDPOF},
#endif
#endif
	{NVTEVT_BKW_SETPROTECT,         BackgroundSetProtect},
	{NVTEVT_BKW_DUMMY_WAIT,         BackgroundDummyWait},
	{NVTEVT_BKW_FW_UPDATE,          BackgroundFWUpdate},
#if (PHOTO_MODE==ENABLE)
	{NVTEVT_BKW_DZOOM_IN,           BackgroundDZoomIn},
	{NVTEVT_BKW_DZOOM_OUT,          BackgroundDZoomOut},
#endif
	{NVTEVT_BKW_PIM_PROCESS,        BackgroundPIMProcess},
	//#NT#2012/10/30#Calvin Chang#Generate Audio NR pattern by noises of zoom, focus and iris -begin
	{NVTEVT_BKW_ANR_LENS_ACTION,    BackgroundAudiNRLensAction},
	//#NT#2012/10/30#Calvin Chang -end
	{NVTEVT_BKW_WIFI_ON,            BackgroundWiFiOn},
#if(WIFI_AP_FUNC==ENABLE)
	{NVTEVT_BKW_WIFI_CLEARACL,      BackgroundWiFiClearACL},
#endif
	{NVTEVT_BKW_STOPREC_PROCESS,    BackgroundStopRecProcess},
#if (defined(_NVT_ETHREARCAM_TX_) || defined(_NVT_ETHREARCAM_RX_))
	{NVTEVT_BKW_ETHCAM_SOCKET_OPEN,    BackgroundEthCamSocketOpen},
	{NVTEVT_BKW_ETHCAM_SOCKETCLI_CMD_OPEN,    BackgroundEthCamSocketCliCmdOpen},
	{NVTEVT_BKW_ETHCAM_SOCKETCLI_DATA_OPEN,    BackgroundEthCamSocketCliDataOpen},
	{NVTEVT_BKW_GET_ETHCAM_TX_INFO,    BackgroundEthCamGetTxInfo},
	{NVTEVT_BKW_TRIGGER_THUMB,    BackgroundEthCamTriggerThumb},
	{NVTEVT_BKW_TX_STREAM_START,    BackgroundEthCamTxStreamStart},


	{NVTEVT_BKW_GET_ETHCAM_TX_DECINFO,    					BackgroundEthCamTxDecInfo},
	{NVTEVT_BKW_ETHCAM_SOCKETCLI_DISP_DATA_OPEN_START,    	BackgroundEthCamTxDispOpenSocketStreamStart},
	{NVTEVT_BKW_ETHCAM_SOCKETCLI_DISP_DATA_OPEN,    	BackgroundEthCamTxDispOpenSocketStream},
	{NVTEVT_BKW_GET_ETHCAM_TX_RECINFO,    					BackgroundEthCamTxRecInfo},
	{NVTEVT_BKW_ETHCAM_SOCKETCLI_REC_DATA_OPEN_START,    	BackgroundEthCamTxRecOpenSocketStreamStart},
	{NVTEVT_BKW_ETHCAM_SYNC_TIME,    						BackgroundEthCamSyncTime},

	{NVTEVT_BKW_ETHCAM_DEC_ERR,    BackgroundEthCamDecErr},
	{NVTEVT_BKW_ETHCAM_DEC_ERR_RESTART,    BackgroundEthCamDecErrRestart},
	{NVTEVT_BKW_ETHCAM_RAW_ENCODE_RESULT,    BackgroundEthCamRawEncodeResult},
	{NVTEVT_BKW_ETHCAM_SET_TX_SYSINFO,    BackgroundEthCamSetTxSysInfo},
	{NVTEVT_BKW_ETHCAM_SET_TX_SLIDEUP, BackgroundEthCamSetTxSlideUp},
	{NVTEVT_BKW_ETHCAM_SET_TX_SLIDEDOWN, BackgroundEthCamSetTxSlideDown},
	{NVTEVT_BKW_ETHCAM_ETHLINKNOTIFY,    BackgroundEthCamEthLinkNotify},
	{NVTEVT_BKW_ETHCAM_IPERF_TEST,    BackgroundEthCamIperfTest},
	{NVTEVT_BKW_ETHCAM_CHECKPORT_READY,    BackgroundEthCamCheckPortReady},
	{NVTEVT_BKW_ETHCAM_UPDATE_UI,    BackgroundEthCamUpdateUIInfo},
	{NVTEVT_BKW_ETHCAM_FW_UPDATE,	BackgroundEthCamFWUpdate},
	#if (AUDPROC_FUNC == ENABLE)
	{NVTEVT_BKW_AUDPROC_INIT,	BackgroundAudProcInit},
	#endif
#endif

	{0,                             0},
};


UINT32 Background_CopyTo(BOOL isCopyToCard, UINT32 copMode)
{
	//#NT#2016/06/27#Lincy Lin -begin
	//#NT#Reduce IPCAM used memory pool size
	UINT32 uiTempBufAddr, uiTempBufSize;
	INT32  uiError_Code = FST_STA_OK;
	//HNVT_STRG pSrcObj, pDstObj, pOriObj;
	DX_HANDLE pSrcDXH = 0, pDstDXH = 0, pOriDXH = 0;
	COPYTO_BYNAME_INFO CopyInfo;
#if USE_FILEDB
	FILEDB_HANDLE fileDbHandle;
	UINT32        fileDbbufSize = 0x200000, fileDbbufAddr, FileNumber;
#endif

	uiTempBufAddr = SxCmd_GetTempMem(POOL_SIZE_APP);
	uiTempBufSize = POOL_SIZE_APP;

	// reserved 2MB for FileDB using
#if USE_FILEDB
	uiTempBufSize -= fileDbbufSize;
	fileDbbufAddr = uiTempBufAddr + uiTempBufSize;
#endif

	//debug_msg("uiTempBufAddr = 0x%x, uiTempBufSize = 0x%x\r\n",uiTempBufAddr,uiTempBufSize);
	//debug_msg("fileDbbufAddr = 0x%x, fileDbbufSize = 0x%x\r\n",fileDbbufAddr,fileDbbufSize);

	UI_SetData(FL_IsCopyToCarding, TRUE);

	if (isCopyToCard) {
		FileSys_GetStrgObj(&pDstDXH);
		pOriDXH = pDstDXH;
		// --> TODO:remove begin
#if (defined(_EMBMEM_NAND_) || defined(_EMBMEM_SPI_NAND_))
		{
			pSrcDXH = Dx_GetObject(DX_CLASS_STORAGE_EXT | USER_DX_TYPE_EMBMEM_FAT);
		}
#else
		//pSrcObj = (HNVT_STRG)sdio2_getStorageObject(STRG_OBJ_FAT1);
		//sdio2_setDetectCardExistHdl(LibFs_CheckCardInserted);
		//sdio2_setDetectCardProtectHdl(SLibFs_CheckCardWP);
#endif
		// --> TODO:remove end
	} else {
		//FileSys_GetStrgObj(&pOriObj);
		FileSys_GetStrgObj(&pOriDXH);
		//pSrcObj = pOriObj;
		pSrcDXH = pOriDXH;
		// --> TODO:remove begin
#if (defined(_EMBMEM_NAND_) || defined(_EMBMEM_SPI_NAND_))
		{
			pDstDXH = Dx_GetObject(DX_CLASS_STORAGE_EXT | USER_DX_TYPE_EMBMEM_FAT);
		}
#else
		//pDstObj = (HNVT_STRG)sdio2_getStorageObject(STRG_OBJ_FAT1);
		//sdio2_setDetectCardExistHdl(LibFs_CheckCardInserted);
		//sdio2_setDetectCardProtectHdl(SLibFs_CheckCardWP);
#endif
	}
	memset(&CopyInfo, 0, sizeof(CopyInfo));
	CopyInfo.pSrcStrg = pSrcDXH;
	CopyInfo.pDstStrg = pDstDXH;
	CopyInfo.pBuf = (char *)uiTempBufAddr;
	CopyInfo.uiBufSize = uiTempBufSize;

#if (USE_FILEDB == ENABLE)
	{
		if (pOriDXH != pSrcDXH) {
			if (FileSys_ChangeDisk(pSrcDXH) != FSS_OK) {
				DBG_ERR("ChangeDisk fail\r\n");
			}
		}
		//  create fileDB for copy
		{
			FILEDB_INIT_OBJ   FilDBInitObj = {0};
			CHAR              rootPath[20] = "A:\\";

			FilDBInitObj.rootPath = rootPath;
			FilDBInitObj.bIsRecursive = TRUE;
			FilDBInitObj.bIsCyclic = TRUE;
			FilDBInitObj.bIsMoveToLastFile = TRUE;
			FilDBInitObj.bIsSupportLongName = TRUE;
			FilDBInitObj.bIsDCFFileOnly = FALSE;
			FilDBInitObj.bIsChkHasFile = FALSE;
			FilDBInitObj.u32MaxFilePathLen = 60;
			FilDBInitObj.u32MaxFileNum = 20000;
			FilDBInitObj.fileFilter = FILEDB_FMT_JPG | FILEDB_FMT_AVI | FILEDB_FMT_MOV | FILEDB_FMT_MP4 | FILEDB_FMT_TS;
			FilDBInitObj.u32MemAddr = dma_getCacheAddr(fileDbbufAddr);
			FilDBInitObj.u32MemSize = fileDbbufSize;
			fileDbHandle = FileDB_Create(&FilDBInitObj);
		}
		// copy file one by one
		{
			UINT32                   i = 0, CurrIndex;
			PFILEDB_FILE_ATTR        pFileAttr = 0;

			CurrIndex = FileDB_GetCurrFileIndex(fileDbHandle);
			FileNumber = FileDB_GetTotalFileNum(fileDbHandle);
			for (i = 0; i < FileNumber; i++) {
				pFileAttr = FileDB_SearhFile(fileDbHandle, i);
				if (!pFileAttr) {
					DBG_ERR("no pFileAttr\r\n");
					break;
				}
				CopyInfo.pSrcPath = pFileAttr->filePath;
				CopyInfo.pDstPath = pFileAttr->filePath;
				debug_msg("Copy file %s\r\n", pFileAttr->filePath);
				uiError_Code = FileSys_CopyToByName(&CopyInfo);
				if (uiError_Code != FST_STA_OK) {
					DBG_ERR("err %d\r\n", uiError_Code);
					break;
				}
			}
			if (FileNumber) {
				FileDB_SearhFile(fileDbHandle, CurrIndex);
			}
		}
		FileDB_Release(fileDbHandle);
	}
#endif
	//#NT#2016/06/27#Lincy Lin -end
	if (pOriDXH != pSrcDXH) {
		if (FileSys_ChangeDisk(pOriDXH) != FSS_OK) {
			DBG_ERR("ChangeDisk fail\r\n");
		}
	}

#if (USE_FILEDB == ENABLE)
	{
		UINT32  curIndex;

		fileDbHandle = 0;
		FileDB_Refresh(fileDbHandle);
		FileNumber = FileDB_GetTotalFileNum(fileDbHandle);
		curIndex = FileNumber - 1;
		if (FileNumber) {
			FileDB_SearhFile(fileDbHandle, curIndex);
		}
#if (PLAY_MODE==ENABLE)
		if (System_GetState(SYS_STATE_CURRMODE) == PRIMARY_MODE_PLAYBACK) {
			PB_OpenSpecFileBySeq(curIndex + 1, TRUE);
			PB_WaitCommandFinish(PB_WAIT_INFINITE);
		}
#endif
	}
#endif
#if (USE_DCF == ENABLE)
	{
		DCF_Refresh();
		DCF_SetCurIndex(DCF_GetDBInfo(DCF_INFO_TOL_FILE_COUNT));

#if (PLAY_MODE==ENABLE)
		if (System_GetState(SYS_STATE_CURRMODE) == PRIMARY_MODE_PLAYBACK) {
			PB_OpenSpecFileBySeq(DCF_GetDBInfo(DCF_INFO_CUR_INDEX), TRUE);
			PB_WaitCommandFinish(PB_WAIT_INFINITE);
		}
#endif
	}
#endif

	UI_SetData(FL_IsCopyToCarding, FALSE);
    SxCmd_RelTempMem(uiTempBufAddr);
	return uiError_Code;
}


UINT32 BackgroundDelAll(void)
{
	INT32 ret;
#if (PLAY_MODE==ENABLE)
	if (System_GetState(SYS_STATE_CURRMODE) == PRIMARY_MODE_PLAYBACK) {
		UIPlay_Delete(PB_DELETE_ALL);
		ret = TRUE;//dummy return value
	} else
#endif
	{
		ret = Background_DeleteAll();
	}
	return (UINT32)ret;
}

//for any mode delete all,
//UI flow need to update orgianl FileDB data,check FL_DeleteAllFlag
UINT32 Background_DeleteAll(void)
{
#if (USE_FILEDB == ENABLE)
	FILEDB_HANDLE      FileDBHdl = 0;
	PFILEDB_FILE_ATTR  FileAttr = NULL;
	INT32              ret = E_OK;
	INT32              FileNum, i;
	PFILEDB_INIT_OBJ   pFDBInitObj = &gBKFDBInitObj;

	UI_SetData(FL_DeleteAllFlag, TRUE);

	pFDBInitObj->u32MemAddr = SxCmd_GetTempMem(POOL_SIZE_FILEDB);

	if (!pFDBInitObj->u32MemAddr) {
		return FALSE;
	}

	pFDBInitObj->u32MemSize = POOL_SIZE_FILEDB;
	FileDBHdl = FileDB_Create(pFDBInitObj);

	FileNum = FileDB_GetTotalFileNum(FileDBHdl);
	for (i = FileNum - 1; i >= 0; i--) {
		FileAttr = FileDB_SearhFile(FileDBHdl, i);
		if (FileAttr) {
			if (M_IsReadOnly(FileAttr->attrib)) {
				continue;
			}
			ret = FileSys_DeleteFile(FileAttr->filePath);
			DBG_IND("i = %04d path=%s\r\n", i, FileAttr->filePath);
			if (ret != FST_STA_OK) {
				goto FDB_Delete_Err;
			} else {
				FileDB_DeleteFile(FileDBHdl, i);
			}
		} else {
			goto FDB_Delete_Err;
		}
	}
	FileDB_Release(FileDBHdl);
    SxCmd_RelTempMem(pFDBInitObj->u32MemAddr);
	return (UINT32)ret;

FDB_Delete_Err:
	DBG_ERR("FDB_Delete err\r\n");
	FileDB_Release(FileDBHdl);
    SxCmd_RelTempMem(pFDBInitObj->u32MemAddr);
	return E_SYS;
#else
	DBG_ERR("not support!\r\n");
	return E_SYS;
#endif
}
UINT32 BackgroundCopy2Card(void)
{
	return Background_CopyTo(TRUE, PB_COPY2CARD_NEW_DIR);
}

UINT32 BackgroundCopy2Nand(void)
{
	return Background_CopyTo(FALSE, PB_COPY2CARD_NEW_DIR);
}

#if(COPYCARD2CARD_FUNCTION == ENABLE)
//#NT#2016/10/18#Niven Cho -begin
//#NT#mount sd-2 as "B:\\"
typedef enum {
	WARN_REMOVE_USB = 0,
	WARN_CONNECT_FAIL,
	WARN_UNKNOWN_FILE,
	WARN_NO_PICTURE,
	WARN_LENS_ERROR,
	WARN_CARD_ERROR,
	WARN_BUILT_IN_MEMORY_ERROR,
	WARN_CARD_UNFORMAT,
	WARN_WRITE_PROTECT,
	WARN_CARD_FULL,
	WARN_MEM_FULL,
	WARN_FILE_PROTECT,
	WARN_DEL_PROTECT,
	WARN_BATT_EMPTY,
	WARN_PAN_ERROR,
	WARN_COPY_CARD_FULL,
	WARN_COPY_CARD_PROTECT,
	WARN_COPY_NO_PICTURE,
	WRN_FW_UPDATE_COMPLETE,
	WARN_RECORD_SLOW_MEDIA,
	WRN_FW_UPDATING,
	WRN_FW_CHECK,
	WRN_NOT_DETECT_REDEYE,
	WRN_EXCEED_MAX_FOLDER,
	WRN_TOO_MANY_PIC,
	WARN_TYPE_MAX
} WARN_TYPE;
//#NT#2016/10/18#Niven Cho -end
UINT32 Background_CopyCardToCard(BOOL IsPrimaryCard, UINT32 copMode)
{
	UINT32 uiTempBufAddr, uiTempBufSize, FileNumber;
	INT32  uiError_Code = FST_STA_OK;
	//HNVT_STRG pSrcObj, pDstObj, pOriObj;
	DX_HANDLE pSrcDXH, pDstDXH, pOriDXH;
	COPYTO_BYNAME_INFO CopyInfo;
#if USE_FILEDB
	FILEDB_HANDLE fileDbHandle, fileDbbufSize = 0x200000, fileDbbufAddr;
#endif
#if (USE_DCF == ENABLE)
	UINT32 uiThisFileFormat;
	UINT32 DirID, FileID;
	UINT32 NewDirID, NewFileID, i;
	CHAR   srcPath[DCF_FULL_FILE_PATH_LEN], dstPath[DCF_FULL_FILE_PATH_LEN];
#endif


	uiTempBufAddr = SxCmd_GetTempMem(POOL_SIZE_APP);
	uiTempBufSize = POOL_SIZE_APP;

	// reserved 2MB for FileDB using
#if USE_FILEDB
	uiTempBufSize -= fileDbbufSize;
	fileDbbufAddr = uiTempBufAddr + uiTempBufSize;
#endif

	UI_SetData(FL_IsCopyToCarding, TRUE);

	if (IsPrimaryCard) {
		FileSys_GetStrgObj(&pDstDXH);
		pOriDXH = pDstDXH;
		{
			pSrcDXH = Dx_GetObject(DX_CLASS_STORAGE_EXT | DX_TYPE_CARD2);
		}
	} else {
		FileSys_GetStrgObj(&pOriDXH);
		pSrcDXH = pOriDXH;
		{
			pDstDXH = Dx_GetObject(DX_CLASS_STORAGE_EXT | DX_TYPE_CARD2);
		}
	}

	memset(&CopyInfo, 0, sizeof(CopyInfo));
	CopyInfo.pSrcStrg = pSrcDXH;
	CopyInfo.pDstStrg = pDstDXH;
	CopyInfo.pBuf = (char *)uiTempBufAddr;
	CopyInfo.uiBufSize = uiTempBufSize;

#if (USE_FILEDB == ENABLE)
	{
		if (pOriDXH != pSrcDXH) {
			if (FileSys_ChangeDisk(pSrcDXH) != FSS_OK) {
				DBG_ERR("ChangeDisk fail\r\n");
			}
		}
		//  create fileDB for copy
		{
			FILEDB_INIT_OBJ   FilDBInitObj = {0};
			CHAR              rootPath[20] = "A:\\";

			FilDBInitObj.rootPath = rootPath;
			FilDBInitObj.bIsRecursive = TRUE;
			FilDBInitObj.bIsCyclic = TRUE;
			FilDBInitObj.bIsMoveToLastFile = TRUE;
			FilDBInitObj.bIsSupportLongName = TRUE;
			FilDBInitObj.bIsDCFFileOnly = FALSE;
			FilDBInitObj.bIsChkHasFile = FALSE;
			FilDBInitObj.u32MaxFilePathLen = 60;
			FilDBInitObj.u32MaxFileNum = 20000;
			FilDBInitObj.fileFilter = FILEDB_FMT_JPG | FILEDB_FMT_AVI | FILEDB_FMT_MOV | FILEDB_FMT_MP4 | FILEDB_FMT_TS;
			FilDBInitObj.u32MemAddr = dma_getCacheAddr(fileDbbufAddr);
			FilDBInitObj.u32MemSize = fileDbbufSize;
			fileDbHandle = FileDB_Create(&FilDBInitObj);
		}
		// copy file one by one
		{
			UINT32                   i = 0, CurrIndex;
			PFILEDB_FILE_ATTR        pFileAttr;

			CurrIndex = FileDB_GetCurrFileIndex(fileDbHandle);
			FileNumber = FileDB_GetTotalFileNum(fileDbHandle);
			for (i = 0; i < FileNumber; i++) {
				pFileAttr = FileDB_SearhFile(fileDbHandle, i);
				CopyInfo.pSrcPath = pFileAttr->filePath;
				CopyInfo.pDstPath = pFileAttr->filePath;
				debug_msg("Copy file %s\r\n", pFileAttr->filePath);
				uiError_Code = FileSys_CopyToByName(&CopyInfo);

				if (uiError_Code != FST_STA_OK) {
					if (uiError_Code == FST_STA_NOFREE_SPACE) {
						uiError_Code = WARN_CARD_FULL;
					}
					break;
				}
			}
			if (FileNumber) {
				FileDB_SearhFile(fileDbHandle, CurrIndex);
			}
		}
		FileDB_Release(fileDbHandle);
	}
#endif
#if (USE_DCF == ENABLE)
	{
		//if (pOriObj==pSrcObj)
		if (pOriDXH == pSrcDXH) {
			// get copy file number
			FileNumber = DCF_GetDBInfo(DCF_INFO_TOL_FILE_COUNT);

			// get dst max ID
			//FileSys_ChangeDisk(pDstObj);
			if (FileSys_ChangeDisk(pDstDXH) != FSS_OK) {
				DBG_ERR("ChangeDisk fail\r\n");
			}
			DCF_ScanObj();
			DCF_GetNextID(&NewDirID, &NewFileID);
			//FileSys_ChangeDisk(pOriObj);
			if (FileSys_ChangeDisk(pOriDXH) != FSS_OK) {
				DBG_ERR("ChangeDisk fail\r\n");
			}
			DCF_ScanObj();
		}
		// pOriObj == pDstObj
		else {
			// get dst max ID
#if 0
			DCF_GetNextID(&NewDirID, &NewFileID);
#else // Rescan DCF if not scanned yet.
			if (!DCF_GetNextID(&NewDirID, &NewFileID)) {
				DCF_ScanObj();
				DCF_GetNextID(&NewDirID, &NewFileID);
			}
#endif
			// get copy file number
			//FileSys_ChangeDisk(pSrcObj);
			if (FileSys_ChangeDisk(pSrcDXH) != FSS_OK) {
				DBG_ERR("ChangeDisk fail\r\n");
			}
			DCF_ScanObj();

			FileNumber = DCF_GetDBInfo(DCF_INFO_TOL_FILE_COUNT);
		}
		if (NewDirID == ERR_DCF_DIR_NUM && NewFileID == ERR_DCF_FILE_NUM) {
			uiError_Code = WRN_EXCEED_MAX_FOLDER;
			goto L_CopyToCard_Exit;
		}
		switch (copMode) {
		case PB_COPY2CARD_NEW_DIR:
			if (NewFileID != MIN_DCF_FILE_NUM) {
				NewDirID++;
				NewFileID = MIN_DCF_FILE_NUM;
				if (NewDirID > MAX_DCF_DIR_NUM) {
					uiError_Code = WRN_EXCEED_MAX_FOLDER;
					goto L_CopyToCard_Exit;
				}
			}
		// continue run append code should not add break here
		case PB_COPY2CARD_APPEND:
			for (i = 1; i <= FileNumber; i++) {
				// get src path
				DCF_SetCurIndex(i);
				DCF_GetObjInfo(i, &DirID, &FileID, &uiThisFileFormat);
				DCF_GetObjPath(i, uiThisFileFormat, srcPath);
				CopyInfo.pSrcPath = srcPath;

				// get dst path
				DCF_MakeObjPath(NewDirID, NewFileID, uiThisFileFormat, dstPath);
				CopyInfo.pDstPath = dstPath;
				uiError_Code = FileSys_CopyToByName(&CopyInfo);
				if (uiError_Code != FST_STA_OK) {
					if (uiError_Code == FST_STA_NOFREE_SPACE) {
						uiError_Code = WARN_CARD_FULL;
					}
					break;
				}
				NewFileID++;
				if (NewFileID > MAX_DCF_FILE_NUM) {
					NewFileID = 1;
					NewDirID++;
					if (NewDirID > MAX_DCF_DIR_NUM) {
						uiError_Code = WRN_EXCEED_MAX_FOLDER;
						goto L_CopyToCard_Exit;
					}
				}
			}
			break;

		case PB_COPY2CARD_NEW_DIR_ORG_FILE: {
				UINT32 tmpDirID  = 0;

				// Fix bug 0050970: Show no picture warning message if no file in Nand when doing copy to card function.
				if (!FileNumber) {
					uiError_Code = WARN_COPY_NO_PICTURE ;
					goto L_CopyToCard_Exit;
				}

				for (i = 1; i <= FileNumber; i++) {
					// get src path
					DCF_SetCurIndex(i);
					DCF_GetObjInfo(i, &DirID, &FileID, &uiThisFileFormat);
					DCF_GetObjPath(i, uiThisFileFormat, srcPath);
					CopyInfo.pSrcPath = srcPath;

					// check if need to new dir
					if (tmpDirID != DirID) {
						tmpDirID = DirID;
						NewDirID++;
						if (NewDirID > MAX_DCF_DIR_NUM) {
							uiError_Code = WRN_EXCEED_MAX_FOLDER;
							goto L_CopyToCard_Exit;
						}
					}

					// get dst path
					DCF_MakeObjPath(NewDirID, FileID, uiThisFileFormat, dstPath);
					CopyInfo.pDstPath = dstPath;
					uiError_Code = FileSys_CopyToByName(&CopyInfo);
					if (uiError_Code != FST_STA_OK) {
						if (uiError_Code == FST_STA_NOFREE_SPACE) {
							uiError_Code = WARN_CARD_FULL;
						}

						break;
					}
				}
			}
			break;
		case PB_COPY2CARD_NO_RENAME:

		default:
			debug_msg("Not Support Copy command %d\r\n", copMode);
			break;
		}
	}
L_CopyToCard_Exit:
#endif

	if (pOriDXH != pSrcDXH) {
		if (FileSys_ChangeDisk(pOriDXH) != FSS_OK) {
			DBG_ERR("ChangeDisk fail\r\n");
		}
	}

#if (USE_FILEDB == ENABLE)
	{
		UINT32  curIndex;

		fileDbHandle = 0;
		FileDB_Refresh(fileDbHandle);
		FileNumber = FileDB_GetTotalFileNum(fileDbHandle);

		curIndex = FileNumber - 1;
		if (FileNumber) {
			FileDB_SearhFile(fileDbHandle, curIndex);
		}
#if (PLAY_MODE==ENABLE)
		if (System_GetState(SYS_STATE_CURRMODE) == PRIMARY_MODE_PLAYBACK) {
			PB_OpenSpecFileBySeq(curIndex + 1, TRUE);
			PB_WaitCommandFinish(PB_WAIT_INFINITE);
		}
#endif
	}
#endif
#if (USE_DCF == ENABLE)
	{
		DCF_Refresh();
		DCF_SetCurIndex(DCF_GetDBInfo(DCF_INFO_TOL_FILE_COUNT));
#if (PLAY_MODE==ENABLE)
		if (System_GetState(SYS_STATE_CURRMODE) == PRIMARY_MODE_PLAYBACK) {
			PB_OpenSpecFileBySeq(DCF_GetDBInfo(DCF_INFO_CUR_INDEX), TRUE);
			PB_WaitCommandFinish(PB_WAIT_INFINITE);
		}
#endif
	}
#endif

	UI_SetData(FL_IsCopyToCarding, FALSE);
	SxCmd_RelTempMem(uiTempBufAddr);
	return uiError_Code;
}

UINT32 BackgroundCopyCard1ToCard2(void)
{
	return Background_CopyCardToCard(FALSE, PB_COPY2CARD_NEW_DIR);
}

UINT32 BackgroundCopyCard2ToCard1(void)
{
	return Background_CopyCardToCard(TRUE, PB_COPY2CARD_NEW_DIR);
}
#endif


UINT32 BackgroundFormat(void)
{
	int ret;

	DX_HANDLE   pStrgDXH = 0;
	FileSys_GetStrgObj(&pStrgDXH);
	//#NT#2016/05/30#Lincy Lin -begin
	//#NT#Support logfile function
#if (LOGFILE_FUNC==ENABLE)
	LogFile_Suspend();
#endif
	//#NT#2016/05/30#Lincy Lin -end
	ret = FileSys_FormatDisk(pStrgDXH, FALSE);
	DBG_DUMP("FormatDisk ret=%d\r\n", ret);
	if (ret == FST_STA_OK) {
		// reset file ID (for FileDB)
#if USE_FILEDB
		if (UI_GetData(FL_IsUseFileDB)) {
			Ux_SendEvent(&UISetupObjCtrl, NVTEVT_FILEID_RESET, 0);
		}
#endif
#if (USE_DCF == ENABLE)
		//reset next id 100 , 1
		DCF_SetNextID(MIN_DCF_DIR_NUM, MIN_DCF_FILE_NUM);
		UI_SetData(FL_DCF_DIR_ID, MIN_DCF_DIR_NUM);
		UI_SetData(FL_DCF_FILE_ID, MIN_DCF_FILE_NUM);

		UI_SetData(FL_IsDCIMReadOnly, FALSE);
#endif
#if (PLAY_MODE==ENABLE)
		if (System_GetState(SYS_STATE_CURRMODE) == PRIMARY_MODE_PLAYBACK) {
			UIPlay_PlaySingle(PB_SINGLE_CURR);
		}
#endif
		//#NT#2016/05/30#Lincy Lin -begin
		//#NT#Support logfile function
#if (LOGFILE_FUNC==ENABLE)
		LogFile_ReOpen();
#endif
		//#NT#2016/05/30#Lincy Lin -end
	}
	if (System_GetState(SYS_STATE_CURRMODE) == PRIMARY_MODE_MOVIE){
		MovieExe_ResetFileSN();
	}else if (System_GetState(SYS_STATE_CURRMODE) == PRIMARY_MODE_PHOTO){
#if(PHOTO_MODE==ENABLE)
	    PhotoExe_ResetFileSN();
#endif
	}

	return (UINT32)ret;
}

UINT32 BackgroundFormatCard(void)
{
	int ret;
        CHAR sTF_FORMAT_MARK[] = TF_FORMAT_MARK_NAME;

	//DX_HANDLE pStrgDev = Dx_GetObject(DX_CLASS_STORAGE_EXT|DX_TYPE_CARD1);
	//UINT32 hStrgObj = Dx_Getcaps(pStrgDev, STORAGE_CAPS_HANDLE, 0);
	//ret = FileSys_FormatDisk((HNVT_STRG)hStrgObj, FALSE);
	DX_HANDLE pStrgDXH = Dx_GetObject(DX_CLASS_STORAGE_EXT | DX_TYPE_CARD1);
	//#NT#2016/05/30#Lincy Lin -begin
	//#NT#Support logfile function
#if (LOGFILE_FUNC==ENABLE)
	if(bSysLogfile)
	{
	LogFile_Suspend();
		LogFile_Close();
	}
#endif
	//#NT#2016/05/30#Lincy Lin -end
	ret = FileSys_FormatDisk(pStrgDXH, FALSE);
	DBG_DUMP("FormatDisk ret=%d\r\n", ret);

	if (ret == FST_STA_OK) {
		// reset file ID (for FileDB)
#if USE_FILEDB
		if (UI_GetData(FL_IsUseFileDB)) {
			Ux_SendEvent(&UISetupObjCtrl, NVTEVT_FILEID_RESET, 0);
		}
#endif
#if (USE_DCF == ENABLE)
		//reset next id 100 , 1
		DCF_SetNextID(MIN_DCF_DIR_NUM, MIN_DCF_FILE_NUM);
		UI_SetData(FL_DCF_DIR_ID, MIN_DCF_DIR_NUM);
		UI_SetData(FL_DCF_FILE_ID, MIN_DCF_FILE_NUM);
		UI_SetData(FL_IsDCIMReadOnly, FALSE);
#endif
#if (PLAY_MODE==ENABLE)
		if (System_GetState(SYS_STATE_CURRMODE) == PRIMARY_MODE_PLAYBACK) {
			UIPlay_PlaySingle(PB_SINGLE_CURR);
		}
#endif
		//#NT#2016/05/30#Lincy Lin -begin
		//#NT#Support logfile function
#if (LOGFILE_FUNC==ENABLE)
		if(bSysLogfile)
		   {
		   LogFile_ReOpen();
		}
#endif
		//#NT#2016/05/30#Lincy Lin -end
		#if 1
		 FST_FILE filehdl2,filehdl3;
		 filehdl3 = FileSys_OpenFile((char *)sTF_FORMAT_MARK,FST_OPEN_READ);
		 if(filehdl3==NULL)
		 {
		   FileSys_MakeDir("A:\\MISC\\");
		   filehdl2 = FileSys_OpenFile((char *)sTF_FORMAT_MARK,FST_CREATE_ALWAYS|FST_OPEN_WRITE);
		     if(filehdl2)
		   {
				FileSys_CloseFile(filehdl2);
				debug_err(("[card]Create Directory	file to A:\\MISC\\MIO_C90.txt \r\n"));
				Delay_DelayMs(500);
				DX_HANDLE pStrgDevOld = GxStrg_GetDevice(0);
				GxStrg_CloseDevice(0);
				GxStrg_OpenDevice(0, pStrgDevOld);
				FileSys_WaitFinish();
				filehdl2 = NULL;
				filehdl2 = FileSys_OpenFile((char *)sTF_FORMAT_MARK,FST_OPEN_READ);
				
				//#NT#2018/11/08#Eric_Ding - begin //#NT# 
		   		FileSys_SetAttrib("A:\\MISC\\",FST_ATTRIB_HIDDEN,TRUE);	//hidden
				//#NT#2018/11/08#Eric_Ding - end

				 if(filehdl2)
				 {
					  FileSys_CloseFile(filehdl2);
					  debug_err(("[card]double check file to A:\\MISC\\MIO_C90.txt\r\n"));
					//  g_Format_ok = 1;
                     
				 }else{
				 	debug_err(("[card]double check file to A:\\MISC\\MIO_C90.txt!!!!! \r\n"));
					//g_Format_ok = 0;
					//g_CardFail =TRUE;

				 }
		     	}else{
		     	
		     		}
		 	}
#endif
	}

	if (System_GetState(SYS_STATE_CURRMODE) == PRIMARY_MODE_MOVIE){
		MovieExe_ResetFileSN();
	}else if (System_GetState(SYS_STATE_CURRMODE) == PRIMARY_MODE_PHOTO){
#if(PHOTO_MODE==ENABLE)
	    PhotoExe_ResetFileSN();
#endif
	}

//#MT#lyb -20200413 -begin
	//Ux_SendEvent(0, NVTEVT_SYSTEM_MODE, 1, System_GetState(SYS_STATE_CURRMODE));
	//Ux_PostEvent(NVTEVT_SYSTEM_MODE, 1, System_GetState(SYS_STATE_CURRMODE));
	Ux_PostEvent( NVTEVT_SYSTEM_MODE, 1, PRIMARY_MODE_MOVIE);
CHKPNT;
//#MT#lyb -20200413 -end
	return (UINT32)ret;
}
UINT32 BackgroundFormatNand(void)
{
	int ret;

	//DX_HANDLE pStrgDev = Dx_GetObject(DX_CLASS_STORAGE_EXT|DX_TYPE_EMBMEM1);
	//UINT32 hStrgObj = Dx_Getcaps(pStrgDev, STORAGE_CAPS_HANDLE, 0);
	//ret = FileSys_FormatDisk((HNVT_STRG)hStrgObj, FALSE);
	DX_HANDLE pStrgDXH = Dx_GetObject(DX_CLASS_STORAGE_EXT | USER_DX_TYPE_EMBMEM_FAT);
	//#NT#2016/05/30#Lincy Lin -begin
	//#NT#Support logfile function
#if (LOGFILE_FUNC==ENABLE)
	LogFile_Suspend();
#endif
	//#NT#2016/05/30#Lincy Lin -end
	ret = FileSys_FormatDisk(pStrgDXH, FALSE);
	DBG_DUMP("FormatDisk ret=%d\r\n", ret);

	if (ret == FST_STA_OK) {
		// reset file ID (for FileDB)
#if USE_FILEDB
		if (UI_GetData(FL_IsUseFileDB)) {
			Ux_SendEvent(&UISetupObjCtrl, NVTEVT_FILEID_RESET, 0);
		}
#endif
#if (USE_DCF == ENABLE)
		//reset next id 100 , 1
		DCF_SetNextID(MIN_DCF_DIR_NUM, MIN_DCF_FILE_NUM);
		UI_SetData(FL_DCF_DIR_ID, MIN_DCF_DIR_NUM);
		UI_SetData(FL_DCF_FILE_ID, MIN_DCF_FILE_NUM);
		UI_SetData(FL_IsDCIMReadOnly, FALSE);
#endif
#if (PLAY_MODE==ENABLE)
		if (System_GetState(SYS_STATE_CURRMODE) == PRIMARY_MODE_PLAYBACK) {
			UIPlay_PlaySingle(PB_SINGLE_CURR);
		}
#endif
		//#NT#2016/05/30#Lincy Lin -begin
		//#NT#Support logfile function
#if (LOGFILE_FUNC==ENABLE)
		LogFile_ReOpen();
#endif
		//#NT#2016/05/30#Lincy Lin -end
	}

	if (System_GetState(SYS_STATE_CURRMODE) == PRIMARY_MODE_MOVIE){
		MovieExe_ResetFileSN();
	}else if (System_GetState(SYS_STATE_CURRMODE) == PRIMARY_MODE_PHOTO){
#if(PHOTO_MODE==ENABLE)
	    PhotoExe_ResetFileSN();
#endif
	}
	return (UINT32)ret;
}

UINT32 BackgroundNumReset(void)
{
#if (USE_DCF == ENABLE)
	UINT32 fid = 0, did = 0;

	//#Get DCF Folder/FIle ID
	DCF_GetNextID((UINT32 *)&did, (UINT32 *)&fid);
	debug_err(("SetupExe_OnNumReset(), DirNum = %d, FileNum = %d\r\n", did, fid));

	if (did < MAX_DCF_DIR_NUM) {
		//reset
		DCF_SetNextID(did + 1, 1);

		//#Get DCF Folder/FIle ID
		DCF_GetNextID((UINT32 *)&did, (UINT32 *)&fid);

		SwTimer_DelayMs(0x2000); //for ui show wait window for 2 second
		debug_err(("SetupExe_OnNumReset(), DirNum = %d, FileNum = %d\r\n", did, fid));

		return TRUE;
	} else {
		debug_err(("did over %d\r\n", MAX_DCF_DIR_NUM));
		return FALSE;
	}
#else
	return TRUE;
#endif
}
#if (USE_DPOF == ENABLE)
UINT32 BackgroundSetDPOF(void)
{
	INT32 ret;

	if (g_uiDpofOPMode == PLAYDPOF_SETONE)
		//Ux_SendEvent(&UIPlayObjCtrl,NVTEVT_SET_DPOFONE,2, g_uiDpofPrtNum, g_uiDpofDateOn);
	{
		DpofVendor_SetDPOFQty(DPOF_OP_SET_CURR, g_uiDpofPrtNum, g_uiDpofDateOn);
	} else {
		//#NT#2010/05/31#[0010985]Lily Kao -begin
		//#NT#DPOF-All from the current or the first picture
		//Ux_SendEvent(&UIPlayObjCtrl,NVTEVT_SET_DPOFALL_FROM_FIRST,2, g_uiDpofPrtNum, g_uiDpofDateOn);
		//#NT#2010/05/31#[0010985]Lily Kao -end
		DpofVendor_SetDPOFQty(DPOF_OP_SET_ALL, g_uiDpofPrtNum, g_uiDpofDateOn);
	}
	ret = TRUE;//dummy return value
	return (UINT32)ret;
}
#endif
UINT32 BackgroundSetProtect(void)
{
#if (PLAY_MODE==ENABLE)
	if (System_GetState(SYS_STATE_CURRMODE) == PRIMARY_MODE_PLAYBACK) {
		INT32 ret;
		UIPlay_Protect(g_uiProtectType);
		ret = TRUE;//dummy return value
		return (UINT32)ret;
	} else
#endif
	{
		if (g_uiProtectType == SETUP_PROTECT_ALL) {
			return Background_ProtectAll(TRUE);
		} else {
			return Background_ProtectAll(FALSE);
		}
	}
}

UINT32 Background_ProtectAll(BOOL bLock)
{
#if (USE_FILEDB == ENABLE)
	FILEDB_HANDLE      FileDBHdl = 0;
	PFILEDB_FILE_ATTR  FileAttr = NULL;
	INT32              ret;
	INT32              FileNum, i;
	PFILEDB_INIT_OBJ   pFDBInitObj = &gBKFDBInitObj;

	pFDBInitObj->u32MemAddr = SxCmd_GetTempMem(POOL_SIZE_FILEDB);

	if (!pFDBInitObj->u32MemAddr) {
		return FALSE;
	}

	pFDBInitObj->u32MemSize = POOL_SIZE_FILEDB;
	FileDBHdl = FileDB_Create(pFDBInitObj);
	//FileDB_DumpInfo(FileDBHdl);
	FileNum = FileDB_GetTotalFileNum(FileDBHdl);

	for (i = 0; i < FileNum; i++) {
		FileAttr = FileDB_SearhFile(FileDBHdl, i);
		if (FileAttr) {
			ret = FileSys_SetAttrib(FileAttr->filePath, FST_ATTRIB_READONLY, bLock);
			if (ret == FST_STA_OK) {
				if (bLock) {
					FileAttr->attrib |= FS_ATTRIB_READ;
				} else {
					FileAttr->attrib &= (~FS_ATTRIB_READ);
				}
			} else {
				goto FDB_Protect_Err;
			}
		} else {
			goto FDB_Protect_Err;
		}
	}
	FileDB_Release(FileDBHdl);
    SxCmd_RelTempMem(pFDBInitObj->u32MemAddr);

	return E_OK;

FDB_Protect_Err:
	DBG_ERR("bLock=%d err\r\n", bLock);
	FileDB_Release(FileDBHdl);
    SxCmd_RelTempMem(pFDBInitObj->u32MemAddr);

	return FALSE;
#else
	DBG_ERR("not support!\r\n");
	return FALSE;
#endif
}

UINT32 BackgroundDummyWait_Stop(void)
{
	//debug_msg("BackgroundDummyWait_Stop:%d,%d\r\n", gUIStopBckGndDummyWait, g_uiWaitTime);
	if (g_uiWaitTime) {
		gUIStopBckGndDummyWait = TRUE;
	}
	return g_uiWaitTime;
}
//#NT#2010/06/23#Lily Kao -begin
UINT32 BackgroundDummyWait(void)
{
	UINT32 i = g_uiWaitTime;

	gUIStopBckGndDummyWait = FALSE;
	while (i) {
		//debug_msg("BackgroundDummyWait:%d,%d,%d\r\n", gUIStopBckGndDummyWait, g_uiWaitTime,i);
		Delay_DelayMs(1);
		i--;
		if (gUIStopBckGndDummyWait == TRUE) {
			//debug_msg("BackgroundDummyWait break:%d,%d\r\n", gUIStopBckGndDummyWait, g_uiWaitTime);
			gUIStopBckGndDummyWait = FALSE;
			g_uiWaitTime = 0;
			break;
		}
	}
	return 1;
}

extern UINT32   System_OnStrg_UploadFW(UINT32 uiStorage);

static UINT32 BackgroundFWUpdate(void)
{
#if (IPCAM_FUNC != ENABLE)
	UINT32 result = 0;
	INT curMode = System_GetState(SYS_STATE_CURRMODE) ;

	System_ChangeSubMode(SYS_SUBMODE_UPDFW);
	Ux_SendEvent(0, NVTEVT_SYSTEM_MODE, 1, PRIMARY_MODE_UPDFW);
	result = System_OnStrg_UploadFW(0);

	if (result != 0) {
		Ux_SendEvent(0, NVTEVT_SYSTEM_MODE, 1, curMode);
		System_ChangeSubMode(SYS_SUBMODE_NORMAL);

	}
#if (defined(_NVT_ETHREARCAM_TX_) || defined(_NVT_ETHREARCAM_RX_))
	if(sEthCamFwUd.FwAddr && sEthCamFwUd.FwSize ){
		Reset_MenuInfo_AfUploadFW();

		// reboot
		GxSystem_EnableSWReset(0);
		System_PowerOff(SYS_POWEROFF_NORMAL);
	}
#endif

	return result;
#else
	debug_msg("_TODO -> %s:%d, task(%x,%s) caller(%pF)\r\n", __FUNCTION__, __LINE__,
			  OS_GetTaskID(), OS_GetTaskName(), __builtin_return_address(0));

	return 0;
#endif
}

#if (PHOTO_MODE==ENABLE)
extern void PhotoExe_DZoomInBK(UINT32 *pIsDzoomStop);
extern void PhotoExe_DZoomOutBK(UINT32 *pIsDzoomStop);

static UINT32 BackgroundDZoomIn(void)
{
	PhotoExe_DZoomInBK(g_pDzoomStop);
	return TRUE;
}

static UINT32 BackgroundDZoomOut(void)
{
	PhotoExe_DZoomOutBK(g_pDzoomStop);
	return TRUE;
}
#endif

//#NT#2012/10/23#Hideo Lin -begin
//#NT#For picture in movie processing
static UINT32 BackgroundPIMProcess(void)
{
	Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_MOVIE_REC_STOP, 0);
	return NVTRET_OK;
}
//#NT#2012/10/23#Hideo Lin -end

//#NT#2012/10/30#Calvin Chang#Generate Audio NR pattern by noises of zoom, focus and iris -begin
static UINT32 BackgroundAudiNRLensAction(void)
{
#if (LENS_FUNCTION == ENABLE)
	UINT32 i, uiLoop;

	Lens_ZoomSetSpeed(ZOOM_SPEED_LOW);

	Delay_DelayMs(3000); // Wait 2 seconds

	// 1. Zoom In/out + Focus in 3 seconds
	debug_err(("Zoom In/out + Focus in 3 seconds!\r\n"));
	uiLoop = 1;
	for (i = 0; i < uiLoop; i++) {
		Lens_ZoomIn();
		Lens_FocusDoAction(50); // position = 45 ~ 600;
		Lens_FocusDoAction(600); // position = 45 ~ 600;
		Delay_DelayMs(150);
		Lens_ZoomStop();
		Delay_DelayMs(25);
		Lens_ZoomOut();
		Lens_FocusDoAction(50); // position = 45 ~ 600;
		Lens_FocusDoAction(600); // position = 45 ~ 600;
		Delay_DelayMs(150);
		Lens_ZoomStop();
		Delay_DelayMs(25);
	}

	Delay_DelayMs(6000); // Wait 6 seconds

	//2. Focus Forward N steps in 3 seconds
	debug_err(("Focus Forward N steps in 3 seconds!\r\n"));
	uiLoop = 6;
	for (i = 0; i < uiLoop; i++) {
		Lens_FocusDoAction(50); // position = 45 ~ 600;
		Lens_FocusDoAction(450); // position = 45 ~ 600;

	}

	Delay_DelayMs(7000); // Wait 7 seconds

	//3. Iris switch in 3 seconds
	debug_err(("Iris switch in 3 seconds!\r\n"));
	uiLoop = 15;
	for (i = 0; i < uiLoop; i++) {
		Lens_ApertureSetPosition(IRIS_POS_SMALL);
		Delay_DelayMs(50);
		Lens_ApertureSetPosition(IRIS_POS_BIG);
		Delay_DelayMs(50);
	}

	Delay_DelayMs(4000);

	Lens_ZoomSetSpeed(ZOOM_SPEED_HIGH);

	// Record stop!
	Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_MOVIE_REC_STOP, 0);

	debug_err(("Record Stop!\r\n"));
#endif

	return NVTRET_OK;
}
//#NT#2012/10/30#Calvin Chang -end
static UINT32 BackgroundWiFiOn(void)
{
#if (WIFI_FUNC == ENABLE)
	Ux_SendEvent(0, NVTEVT_EXE_WIFI_START, 0);
#endif
	return NVTRET_OK;
}
#if(WIFI_AP_FUNC==ENABLE)

static UINT32 BackgroundWiFiClearACL(void)
{
	Delay_DelayMs(ACL_TIME);
	UINet_ClearACLTable();
	return NVTRET_OK;
}
#endif
static UINT32 BackgroundStopRecProcess(void)
{CHKPNT;
	//debug_err(("StopRec\r\n\n\n\n\n"));
	FlowMovie_StopRec();
        return NVTRET_OK;
}

#if (defined(_NVT_ETHREARCAM_TX_) || defined(_NVT_ETHREARCAM_RX_))
static UINT32 BackgroundEthCamSocketOpen(void)
{
#if (defined(_NVT_ETHREARCAM_TX_))
	socketEthCmd_Open();

	while(MovieExe_GetCommonMemInitFinish()==0){
		Delay_DelayMs(5);
	}

	socketEthData_Open(ETHSOCKIPC_ID_0);
#if 1//(ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
	socketEthData_Open(ETHSOCKIPC_ID_1);
#endif
#endif
	return NVTRET_OK;
}

static UINT32 BackgroundEthCamSocketCliCmdOpen(void)
{
#if (defined(_NVT_ETHREARCAM_RX_))
	UINT32 i;

	for (i=0; i<ETH_REARCAM_CAPS_COUNT; i++){
		if(EthCamNet_GetEthLinkStatus(i)==ETHCAM_LINK_UP && (EthCamNet_GetEthLinkStatus(i) != EthCamNet_GetPrevEthLinkStatus(i))){
			//CHKPNT;
			socketCliEthCmd_Open(i);
		}
	}
#endif
	return NVTRET_OK;
}
static UINT32 BackgroundEthCamSocketCliDataOpen(void)
{
#if (defined(_NVT_ETHREARCAM_RX_))
	UINT32 i;
	for (i=0; i<ETH_REARCAM_CAPS_COUNT; i++){
		if(socketCliEthCmd_IsConn(i) && sEthCamTxDecInfo[i].bStarupOK && EthCamNet_GetEthLinkStatus(i)==ETHCAM_LINK_UP && (EthCamNet_GetEthLinkStatus(i) != EthCamNet_GetPrevEthLinkStatus(i))){
#if(ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
			CHKPNT;
			socketCliEthData_Open(i, ETHSOCKIPCCLI_ID_1);
#endif
			socketCliEthData_Open(i, ETHSOCKIPCCLI_ID_0);
		}
	}
#endif
	return NVTRET_OK;
}
static UINT32 BackgroundEthCamTxDecInfo(void)
{
#if(defined(_NVT_ETHREARCAM_RX_))
	ETHCAM_PORT_TYPE port_type=ETHCAM_PORT_DATA1;
	UINT32 i;
	for (i=0; i<ETH_REARCAM_CAPS_COUNT; i++){
		if(EthCamNet_GetEthLinkStatus(i)==ETHCAM_LINK_UP && (EthCamNet_GetEthLinkStatus(i) != EthCamNet_GetPrevEthLinkStatus(i))){
			#if (ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
			port_type =ETHCAM_PORT_DATA2;
			#else
			port_type =ETHCAM_PORT_DATA1;
			#endif

			EthCam_SendXMLCmd(i, port_type ,ETHCAM_CMD_GET_TX_DEC_INFO, 0);
			EthCam_SendXMLCmd(i, port_type ,ETHCAM_CMD_GET_TX_SPS_PPS, 0);

			EthCamSocketCli_SetDescSize(i, ETHSOCKIPCCLI_ID_1,  sEthCamTxDecInfo[i].DescSize);
			sEthCamTxDecInfo[i].bStarupOK=1;
			//get tx dec info
			MOVIEMULTI_ETHCAM_DEC_INFO dec_info={0};

			dec_info.width = sEthCamTxDecInfo[i].Width;
			dec_info.height = sEthCamTxDecInfo[i].Height;
			dec_info.gop = sEthCamTxDecInfo[i].Gop;
			dec_info.codec = sEthCamTxDecInfo[i].Codec;
			dec_info.header_size = sEthCamTxDecInfo[i].DescSize;
			dec_info.header = sEthCamTxDecInfo[i].Desc;
			//DBG_DUMP("dec_info, w=%d, h=%d, GOP=%d, Codec=%d\r\n",dec_info.width ,dec_info.height,dec_info.gop,dec_info.codec);
			//DBG_DUMP("Desc=0x%x, DescSize=%d\r\n",sEthCamTxDecInfo.Desc,sEthCamTxDecInfo.DescSize);

			ImageApp_MovieMulti_SetParam(_CFG_ETHCAM_ID_1+i, MOVIEMULTI_PRARM_ETHCAM_DEC_INFO, (UINT32)&dec_info);
			//ImageApp_MovieMulti_EthCamLinkForDisp(_CFG_ETHCAM_ID_1, ENABLE, TRUE);

			#if (ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
			CHKPNT;
			socketCliEthData2_ConfigRecvBuf(i);
			#else
			CHKPNT;
			socketCliEthData1_ConfigRecvBuf(i);
			#endif
		}
	}
#endif
	return NVTRET_OK;
}
static UINT32 BackgroundEthCamTxRecInfo(void)
{
#if(defined(_NVT_ETHREARCAM_RX_))
	UINT32 i;
	for (i=0; i<ETH_REARCAM_CAPS_COUNT; i++){
		if(EthCamNet_GetEthLinkStatus(i)==ETHCAM_LINK_UP && (EthCamNet_GetEthLinkStatus(i) != EthCamNet_GetPrevEthLinkStatus(i))){
			EthCam_SendXMLCmd(i, ETHCAM_PORT_DATA1 ,ETHCAM_CMD_GET_TX_REC_INFO, 0);
			MOVIEMULTI_ETHCAM_REC_INFO EthCamRecInfo={0};
			memcpy(&EthCamRecInfo, &sEthCamTxRecInfo[i], sizeof(MOVIEMULTI_ETHCAM_REC_INFO));
			EthCam_SendXMLCmd(i, ETHCAM_PORT_DATA1 ,ETHCAM_CMD_GET_TX_SPS_PPS, 0);
			EthCamSocketCli_SetDescSize(i, ETHSOCKIPCCLI_ID_0,  sEthCamTxRecInfo[i].DescSize);

			DBG_DUMP("rec_format[%d]=%d\r\n",i,sEthCamTxRecInfo[i].rec_format);
			EthCamRecInfo.rec_format=_CFG_FILE_FORMAT_MOV;//cj 0526 //_CFG_FILE_FORMAT_MP4;
			EthCamRecInfo.aud_codec=ETHCAM_AUDIO_FORMAT;
			sEthCamTxRecInfo[i].rec_format=_CFG_FILE_FORMAT_MOV;
			sEthCamTxRecInfo[i].aud_codec=ETHCAM_AUDIO_FORMAT;
			ImageApp_MovieMulti_SetParam(_CFG_ETHCAM_ID_1+i, MOVIEMULTI_PRARM_ETHCAM_REC_INFO, (UINT32)&EthCamRecInfo);
			#if (ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
			//CHKPNT;
			socketCliEthData1_ConfigRecvBuf(i);
			#endif
		}
	}
#endif
	return NVTRET_OK;
}
static UINT32 BackgroundEthCamGetTxInfo(void)
{
#if(defined(_NVT_ETHREARCAM_RX_))
	UINT32 i;
	for (i=0; i<ETH_REARCAM_CAPS_COUNT; i++){
		if(socketCliEthCmd_IsConn(i) && (socketCliEthData1_IsConn(i)==0) && EthCamNet_GetEthLinkStatus(i)==ETHCAM_LINK_UP && (EthCamNet_GetEthLinkStatus(i) != EthCamNet_GetPrevEthLinkStatus(i))){
			//DBG_DUMP("BackgroundEthCamGetTxInfo, path_id=%d\r\n",i);
			sEthCamTxDecInfo[i].bStarupOK=0;
			EthCam_SendXMLCmd(i, ETHCAM_PORT_DEFAULT,ETHCAM_CMD_STARTUP, 0);
			if(sEthCamTxDecInfo[i].bStarupOK){
				#if 1
				//sync time
				CHKPNT;
#if  (AV_LCA_FUNC == ENABLE)
				EthCam_SendXMLCmd(ETHCAM_PATH_ID_1,ETHCAM_PORT_DEFAULT,ETHCAM_CMD_RX_LCASTOP,1);
				Delay_DelayMs(100);
#endif
				EthCam_SendXMLCmd(i, ETHCAM_PORT_DEFAULT ,ETHCAM_CMD_SYNC_TIME, 0);
				struct tm Curr_DateTime ={0};
				Curr_DateTime = HwClock_GetTime(TIME_ID_CURRENT);
				EthCam_SendXMLData(i, (UINT8 *)&Curr_DateTime, sizeof(struct tm));
#if  (AV_LCA_FUNC == ENABLE)
				EthCam_SendXMLCmd(ETHCAM_PATH_ID_1,ETHCAM_PORT_DEFAULT,ETHCAM_CMD_RX_LCASTOP,0);
#endif
				#endif

				#if 0
				//sync menu
				EthCam_SendXMLCmd(i, ETHCAM_PORT_DEFAULT ,ETHCAM_CMD_SYNC_MENU_SETTING, 0);
				ETHCAM_MENU_SETTING sEthCamMenuSetting;
				#if (ETH_REARCAM_CAPS_COUNT>=2)
				sEthCamMenuSetting.Size=MOVIE_SIZE_FRONT_1920x1080P30;
				#else
				sEthCamMenuSetting.Size=MOVIE_SIZE_CLONE_1920x1080P30_1280x720P30;
				#endif
				sEthCamMenuSetting.WDR=UI_GetData(FL_MOVIE_WDR);
				sEthCamMenuSetting.EV=UI_GetData(FL_EV);
				sEthCamMenuSetting.DateImprint=UI_GetData(FL_MOVIE_DATEIMPRINT);
				sEthCamMenuSetting.SensorRotate=UI_GetData(FL_MOVIE_SENSOR_ROTATE);
				sEthCamMenuSetting.Codec=UI_GetData(FL_MOVIE_CODEC);//MOVIE_CODEC_H264;//UI_GetData(FL_MOVIE_CODEC);
#if defined(_NVT_ETHREARCAM_CAPS_COUNT_1_)
				sEthCamMenuSetting.TimeLapse=UI_GetData(FL_MOVIE_TIMELAPSE_REC);
#else
				sEthCamMenuSetting.TimeLapse=MOVIE_TIMELAPSEREC_OFF;
#endif
				EthCam_SendXMLData(i, (UINT8 *)&sEthCamMenuSetting, sizeof(ETHCAM_MENU_SETTING));
				#endif

				ETHCAM_PORT_TYPE port_type=ETHCAM_PORT_DATA1;
				#if (ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
				port_type =ETHCAM_PORT_DATA2;
				#else
				port_type =ETHCAM_PORT_DATA1;
				#endif

				EthCam_SendXMLCmd(i, ETHCAM_PORT_DATA1 ,ETHCAM_CMD_GET_TX_REC_INFO, 0);

				EthCam_SendXMLCmd(i, port_type ,ETHCAM_CMD_GET_TX_DEC_INFO, 0);

				EthCam_SendXMLCmd(i, ETHCAM_PORT_DATA1 ,ETHCAM_CMD_GET_TX_SPS_PPS, 0);
				EthCamSocketCli_SetDescSize(i, ETHSOCKIPCCLI_ID_0,  sEthCamTxRecInfo[i].DescSize);

				#if (ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
				EthCam_SendXMLCmd(i, port_type ,ETHCAM_CMD_GET_TX_SPS_PPS, 0);
				EthCamSocketCli_SetDescSize(i, ETHSOCKIPCCLI_ID_1,  sEthCamTxDecInfo[i].DescSize);
				#else
				EthCamSocketCli_SetDescSize(i, ETHSOCKIPCCLI_ID_0,  sEthCamTxDecInfo[i].DescSize);
				#endif

				//get tx dec info
				MOVIEMULTI_ETHCAM_DEC_INFO dec_info;

				dec_info.width = sEthCamTxDecInfo[i].Width;
				dec_info.height = sEthCamTxDecInfo[i].Height;
				dec_info.gop = sEthCamTxDecInfo[i].Gop;
				dec_info.codec = sEthCamTxDecInfo[i].Codec;
				dec_info.header_size = sEthCamTxDecInfo[i].DescSize;
				dec_info.header = sEthCamTxDecInfo[i].Desc;
				//DBG_IND("[%d]dec_info, w=%d, h=%d, GOP=%d, Codec=%d\r\n",i,dec_info.width ,dec_info.height,dec_info.gop,dec_info.codec);
				//DBG_IND("[%d]Desc=0x%x, DescSize=%d\r\n",i,sEthCamTxDecInfo[i].Desc,sEthCamTxDecInfo[i].DescSize);
				DBG_DUMP("[%d]dec_info DescSize=%d, Desc[0]=%x,%x,%x,%x,%x\r\n",i,sEthCamTxDecInfo[i].DescSize, sEthCamTxDecInfo[i].Desc[0],sEthCamTxDecInfo[i].Desc[1],sEthCamTxDecInfo[i].Desc[2],sEthCamTxDecInfo[i].Desc[3],sEthCamTxDecInfo[i].Desc[4]);
				if(sEthCamTxDecInfo[i].DescSize==0){
					DBG_ERR("[%d]TxDecInfo DescSize 0!!\r\n",i,sEthCamTxDecInfo[i].DescSize);
					return NVTRET_ERROR;
				}
				ImageApp_MovieMulti_SetParam((_CFG_ETHCAM_ID_1 + i), MOVIEMULTI_PRARM_ETHCAM_DEC_INFO, (UINT32)&dec_info);

				//EthCam_SendXMLCmd(i, ETHCAM_PORT_DATA1 ,ETHCAM_CMD_GET_TX_REC_INFO, 0);
				MOVIEMULTI_ETHCAM_REC_INFO EthCamRecInfo={0};
				memcpy(&EthCamRecInfo, &sEthCamTxRecInfo[i], sizeof(MOVIEMULTI_ETHCAM_REC_INFO));
				//EthCam_SendXMLCmd(i, ETHCAM_PORT_DATA1 ,ETHCAM_CMD_GET_TX_SPS_PPS, 0);

				//DBG_IND("rec_format=%d\r\n",sEthCamTxRecInfo[i].rec_format);
				if(sEthCamTxRecInfo[i].DescSize==0){
					DBG_ERR("[%d]RecInfo DescSize 0!!\r\n",i,sEthCamTxRecInfo[i].DescSize);
					return NVTRET_ERROR;
				}
				//DBG_IND("[%d]RecInfo Desc=0x%x, DescSize=%d\r\n",i,sEthCamTxRecInfo[i].Desc,sEthCamTxRecInfo[i].DescSize);
				DBG_DUMP("[%d]RecInfo DescSize=%d, Desc[0]=%x,%x,%x,%x,%x\r\n",i,sEthCamTxRecInfo[i].DescSize,sEthCamTxRecInfo[i].Desc[0],sEthCamTxRecInfo[i].Desc[1],sEthCamTxRecInfo[i].Desc[2],sEthCamTxRecInfo[i].Desc[3],sEthCamTxRecInfo[i].Desc[4]);

				EthCamRecInfo.rec_format=_CFG_FILE_FORMAT_MOV;//cj 0526 //_CFG_FILE_FORMAT_MP4; 
				EthCamRecInfo.aud_codec=ETHCAM_AUDIO_FORMAT;
				sEthCamTxRecInfo[i].rec_format=_CFG_FILE_FORMAT_MOV;
				sEthCamTxRecInfo[i].aud_codec=ETHCAM_AUDIO_FORMAT;
				ImageApp_MovieMulti_SetParam((_CFG_ETHCAM_ID_1 + i), MOVIEMULTI_PRARM_ETHCAM_REC_INFO, (UINT32)&EthCamRecInfo);
				//ImageApp_MovieMulti_EthCamLinkForDisp((_CFG_ETHCAM_ID_1 + i), ENABLE, TRUE);
				MovieExe_SetEthcamEncBufSec(ETHCAM_PATH_ID_1+i, 5);
				CHKPNT;
				socketCliEthData1_ConfigRecvBuf(i);
				socketCliEthData2_ConfigRecvBuf(i);

			}else{
				DBG_ERR("bStarupOK Fail, path_id=%d\r\n",i);
			}
		}
	}
#endif
	return NVTRET_OK;
}
static UINT32 BackgroundEthCamTriggerThumb(void)
{
	CHKPNT;
	DBGD(g_uiEthcamTriggerThumbPathId);

#if(defined(_NVT_ETHREARCAM_RX_))
	UINT32 i;
	for (i=0; i<ETH_REARCAM_CAPS_COUNT; i++){
		//DBG_DUMP("[%d]BsPort2Recid=%d, %d\r\n",i,g_uiThumbPathId,ImageApp_MovieMulti_BsPort2EthCamlink(g_uiThumbPathId));
		if(EthCamNet_GetEthLinkStatus(i)==ETHCAM_LINK_UP && ImageApp_MovieMulti_BsPort2EthCamlink(g_uiEthcamTriggerThumbPathId[i])==(_CFG_ETHCAM_ID_1+i)){
			DBG_DUMP("BK Trigger Thumb[%d][%d]\r\n", i, g_uiEthcamTriggerThumbPathId[i]);
			EthCam_SendXMLCmd(i, ETHCAM_PORT_DATA1 ,ETHCAM_CMD_GET_TX_MOVIE_THUMB, 0);
		}
	}
#endif
	return NVTRET_OK;
}
static UINT32 BackgroundEthCamTxDispOpenSocketStreamStart(void)
{
#if(defined(_NVT_ETHREARCAM_RX_))
	UINT32 i;
	for (i=0; i<ETH_REARCAM_CAPS_COUNT; i++){
		if(EthCamNet_GetEthLinkStatus(i)==ETHCAM_LINK_UP && (EthCamNet_GetEthLinkStatus(i) != EthCamNet_GetPrevEthLinkStatus(i))){
#if(ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
			CHKPNT;
			socketCliEthData_Open(i, ETHSOCKIPCCLI_ID_1);
			ImageApp_MovieMulti_EthCamLinkForDisp(_CFG_ETHCAM_ID_1+i, ENABLE, TRUE);
			EthCam_SendXMLCmd(i, ETHCAM_PORT_DATA2 ,ETHCAM_CMD_TX_STREAM_START, 0);
#else
			socketCliEthData_Open(i, ETHSOCKIPCCLI_ID_0);
			ImageApp_MovieMulti_EthCamLinkForDisp(_CFG_ETHCAM_ID_1+i, ENABLE, TRUE);
			EthCam_SendXMLCmd(i, ETHCAM_PORT_DATA1 ,ETHCAM_CMD_TX_STREAM_START, 0);
#endif
		}
	}
#endif
	return NVTRET_OK;
}
static UINT32 BackgroundEthCamTxDispOpenSocketStream(void)
{
#if(defined(_NVT_ETHREARCAM_RX_))
	UINT32 i;
	for (i=0; i<ETH_REARCAM_CAPS_COUNT; i++){
		if(EthCamNet_GetEthLinkStatus(i)==ETHCAM_LINK_UP && (EthCamNet_GetEthLinkStatus(i) != EthCamNet_GetPrevEthLinkStatus(i))){
#if(ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
			socketCliEthData_Open(i, ETHSOCKIPCCLI_ID_1);
#else
			socketCliEthData_Open(i, ETHSOCKIPCCLI_ID_0);
#endif
		}
	}
#endif
	return NVTRET_OK;
}
static UINT32 BackgroundEthCamTxRecOpenSocketStreamStart(void)
{
#if(defined(_NVT_ETHREARCAM_RX_))
#if(ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
	UINT32 i;
	for (i=0; i<ETH_REARCAM_CAPS_COUNT; i++){
		if(EthCamNet_GetEthLinkStatus(i)==ETHCAM_LINK_UP && (EthCamNet_GetEthLinkStatus(i) != EthCamNet_GetPrevEthLinkStatus(i))){
			socketCliEthData_Open(i, ETHSOCKIPCCLI_ID_0);CHKPNT;
			EthCam_SendXMLCmd(i ,ETHCAM_PORT_DATA1 ,ETHCAM_CMD_TX_STREAM_START, 0);
			EthCamNet_SetPrevEthLinkStatus(i, ETHCAM_LINK_UP);
		}
	}
#endif
#endif
	return NVTRET_OK;
}


static UINT32 BackgroundEthCamTxStreamStart(void)
{
#if(defined(_NVT_ETHREARCAM_RX_))
	UINT32 i;
	for (i=0; i<ETH_REARCAM_CAPS_COUNT; i++){
		//DBG_WRN("GetEthLinkStatus[%d]=%d, GetPrevEthLinkStatus[%d]=%d\r\n",i,EthCamNet_GetEthLinkStatus(i),i,EthCamNet_GetPrevEthLinkStatus(i));
		if(socketCliEthData1_IsConn(i) && EthCamNet_GetEthLinkStatus(i)==ETHCAM_LINK_UP && (EthCamNet_GetEthLinkStatus(i) != EthCamNet_GetPrevEthLinkStatus(i))){
			DBG_DUMP("TxStream Open path id=%d\r\n",i);
			ImageApp_MovieMulti_EthCamLinkForDisp((_CFG_ETHCAM_ID_1 + i), ENABLE, TRUE);
#if(ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
			EthCam_SendXMLCmd(i, ETHCAM_PORT_DATA2 ,ETHCAM_CMD_TX_STREAM_START, 0);
#endif
			EthCam_SendXMLCmd(i, ETHCAM_PORT_DATA1 ,ETHCAM_CMD_TX_STREAM_START, 0);
			EthCamNet_SetPrevEthLinkStatus(i, ETHCAM_LINK_UP);
		}else{
#if (ETH_REARCAM_CAPS_COUNT>=2)
			if(socketCliEthData1_IsConn(i) && EthCamNet_GetEthHub_LinkStatus(i) && EthCamNet_GetEthLinkStatus(i)==ETHCAM_LINK_UP && socketCliEthData1_IsRecv(i)==0){
				DBG_DUMP("TxStream restart path id=%d\r\n",i);
				ImageApp_MovieMulti_EthCamLinkForDisp((_CFG_ETHCAM_ID_1 + i), ENABLE, TRUE);
				EthCam_SendXMLCmd(i, ETHCAM_PORT_DATA1 ,ETHCAM_CMD_TX_STREAM_START, 0);
			}
#endif
		}
	}
	EthCamCmd_GetFrameTimerEn(1);
#endif
	return NVTRET_OK;
}
static UINT32 BackgroundEthCamSyncTime(void)
{
#if(defined(_NVT_ETHREARCAM_RX_))
	UINT32 i;
	for (i=0; i<ETH_REARCAM_CAPS_COUNT; i++){
		if(EthCamNet_GetEthLinkStatus(i)==ETHCAM_LINK_UP && (EthCamNet_GetEthLinkStatus(i) != EthCamNet_GetPrevEthLinkStatus(i))){
			//sync time
			//debug_msg("--cj ethcam:%d %d",EthCamNet_GetEthLinkStatus(i), EthCamNet_GetPrevEthLinkStatus(i));
			CHKPNT;
#if  (AV_LCA_FUNC == ENABLE)
			EthCam_SendXMLCmd(ETHCAM_PATH_ID_1,ETHCAM_PORT_DEFAULT,ETHCAM_CMD_RX_LCASTOP,1);
			Delay_DelayMs(100);
#endif
			EthCam_SendXMLCmd(i, ETHCAM_PORT_DEFAULT ,ETHCAM_CMD_SYNC_TIME, 0);
			struct tm Curr_DateTime ={0};
			Curr_DateTime = HwClock_GetTime(TIME_ID_CURRENT);
			EthCam_SendXMLData(i, (UINT8 *)&Curr_DateTime, sizeof(struct tm));
#if  (AV_LCA_FUNC == ENABLE)
			EthCam_SendXMLCmd(ETHCAM_PATH_ID_1,ETHCAM_PORT_DEFAULT,ETHCAM_CMD_RX_LCASTOP,0);
#endif
#if 0
			//sync menu
			EthCam_SendXMLCmd(i, ETHCAM_PORT_DEFAULT ,ETHCAM_CMD_SYNC_MENU_SETTING, 0);
			ETHCAM_MENU_SETTING sEthCamMenuSetting;
			#if (ETH_REARCAM_CAPS_COUNT>=2)
			sEthCamMenuSetting.Size=MOVIE_SIZE_FRONT_1920x1080P30;
			#else
			sEthCamMenuSetting.Size=MOVIE_SIZE_CLONE_1920x1080P30_1280x720P30;
			#endif
			sEthCamMenuSetting.WDR=UI_GetData(FL_MOVIE_WDR);
			sEthCamMenuSetting.EV=UI_GetData(FL_EV);
			sEthCamMenuSetting.DateImprint=UI_GetData(FL_MOVIE_DATEIMPRINT);
			sEthCamMenuSetting.SensorRotate=UI_GetData(FL_MOVIE_SENSOR_ROTATE);
			sEthCamMenuSetting.Codec=UI_GetData(FL_MOVIE_CODEC);//MOVIE_CODEC_H264;//UI_GetData(FL_MOVIE_CODEC);
#if defined(_NVT_ETHREARCAM_CAPS_COUNT_1_)
			sEthCamMenuSetting.TimeLapse=UI_GetData(FL_MOVIE_TIMELAPSE_REC);
#else
			sEthCamMenuSetting.TimeLapse=MOVIE_TIMELAPSEREC_OFF;
#endif
			EthCam_SendXMLData(i, (UINT8 *)&sEthCamMenuSetting, sizeof(ETHCAM_MENU_SETTING));
#endif
		}
	}
#endif
	return NVTRET_OK;
}
static UINT32 BackgroundEthCamDecErr(void)
{
#if(defined(_NVT_ETHREARCAM_RX_))
	UINT32 DecErrPathId = g_uiEthcamDecErrPathId;
	EthCamCmd_GetFrameTimerEn(0);
	EthCam_SendXMLCmd(DecErrPathId, ETHCAM_PORT_DATA1 ,ETHCAM_CMD_TX_STREAM_STOP, 0);
	socketCliEthData1_SetRecv(DecErrPathId, 0);
#if(ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
	EthCam_SendXMLCmd(DecErrPathId, ETHCAM_PORT_DATA2 ,ETHCAM_CMD_TX_STREAM_STOP, 0);

	socketCliEthData2_SetRecv(DecErrPathId, 0);
#endif
	MovieExe_EthCam_ChgDispCB(UI_GetData(FL_DUAL_CAM));
	EthCamCmd_GetFrameTimerEn(1);
	socketCliEth_DecErrTimerClose();
	socketCliEth_DecErrTimerOpen();
#endif
	return NVTRET_OK;
}
static UINT32 BackgroundEthCamDecErrRestart(void)
{
#if(defined(_NVT_ETHREARCAM_RX_))
	UINT32 DecErrPathId = g_uiEthcamDecErrPathId;
	socketCliEth_DecErrTimerClose();
	EthCamNet_LinkDetStreamRestart(DecErrPathId);
#endif
	return NVTRET_OK;
}

static UINT32 BackgroundEthCamRawEncodeResult(void)
{
	if(!ImageApp_MovieMulti_IsStreamRunning(_CFG_REC_ID_1)
		#if (SENSOR_CAPS_COUNT>=2)
		&& !ImageApp_MovieMulti_IsStreamRunning(_CFG_REC_ID_2)
		#endif
	){
		if(ImageApp_MovieMulti_IsStreamRunning(_CFG_ETHCAM_ID_1)
			|| ImageApp_MovieMulti_IsStreamRunning(_CFG_ETHCAM_ID_2)){
			MovieExe_TriggerPIMResultManual(0);
		}
	}
	return NVTRET_OK;
}

static UINT32 BackgroundEthCamSetTxSysInfo(void)
{
#if(defined(_NVT_ETHREARCAM_RX_))
	UINT32 i;
	UINT32 TxVdoEncBufSec=3; //default=3
	for (i=0; i<ETH_REARCAM_CAPS_COUNT; i++){
		if(socketCliEthCmd_IsConn(i) && EthCamNet_GetEthLinkStatus(i)==ETHCAM_LINK_UP && (EthCamNet_GetEthLinkStatus(i) != EthCamNet_GetPrevEthLinkStatus(i))){
			sEthCamTxSysInfo[i].bCmdOK=0;
			EthCam_SendXMLCmd(i, ETHCAM_PORT_DEFAULT ,ETHCAM_CMD_SET_TX_SYS_INFO, TxVdoEncBufSec);
			EthCam_SendXMLData(i, (UINT8 *)&sEthCamTxSysInfo[i], sizeof(ETHCAM_TX_SYS_INFO));
			DBG_DUMP("bCmdOK[%d]=%d\r\n",i,sEthCamTxSysInfo[i].bCmdOK);

			if(sEthCamTxSysInfo[i].bCmdOK==1){
				EthCam_SendXMLCmd(i, ETHCAM_PORT_DEFAULT ,ETHCAM_CMD_SET_TX_CODEC_SRCTYPE, 0);
				EthCam_SendXMLData(i, (UINT8 *)&sEthCamCodecSrctype[i], sizeof(ETHCAM_TX_CODEC_SRCTYPE));

				EthCam_SendXMLCmd(i, ETHCAM_PORT_DEFAULT ,ETHCAM_CMD_SET_TX_AUDCAP, g_EthCamCfgTxAudCap);
				EthCam_SendXMLCmd(i, ETHCAM_PORT_DEFAULT ,ETHCAM_CMD_SET_TX_AUDINFO, 0);
				MOVIEMULTI_AUDIO_INFO MovieAudInfo=MovieExe_GetAudInfo();
				EthCam_SendXMLData(i, (UINT8 *)&MovieAudInfo, sizeof(MOVIEMULTI_AUDIO_INFO));
			}
		}
	}
#endif
	return NVTRET_OK;
}
static UINT32 BackgroundEthCamEthLinkNotify(void)
{
#if defined(_CPU2_LINUX_)
#if(defined(_NVT_ETHREARCAM_RX_))
	UINT32 EthLinkNotifySta = g_uiEthcamEthLinkNotifySta;
	if(EthLinkNotifySta == ETHCAM_LINK_UP){
		EthCamNet_UDPSocketOpen();

		if(SxTimer_GetFuncActive(SX_TIMER_ETHCAM_ETHLINKRETRY_ID)==0){
			//DBG_DUMP("Active ETHLINKRETRY Timer\r\n");
			SxTimer_SetFuncActive(SX_TIMER_ETHCAM_ETHLINKRETRY_ID, TRUE);
		}
	}else if(EthLinkNotifySta == ETHCAM_LINK_DOWN){
		if(EthCamNet_GetDhcpSrvConnIpAddr()){
			char ipccmd[40];
			snprintf(ipccmd, sizeof(ipccmd) - 1, "ethlinknotify %d %d",EthCamNet_GetDhcpSrvConnIpAddr(), ETHCAM_LINK_DOWN);
			EthCamNet_EthLinkStatusNotify(ipccmd);
		}
	}
#endif
#endif
	return NVTRET_OK;
}
static UINT32 BackgroundEthCamIperfTest(void)
{
#if(defined(_NVT_ETHREARCAM_RX_))
	EthCamNet_IperfTest();
#endif
	return NVTRET_OK;
}
static UINT32 BackgroundEthCamCheckPortReady(void)
{
#if (ETHCAM_CHECK_PORTREADY == ENABLE)
#if(defined(_NVT_ETHREARCAM_RX_))

	char ipccmd[64]={0};
	NVTIPC_SYS_MSG sysMsg;
	NVTIPC_I32 ipcErr = 0;

	snprintf(ipccmd, sizeof(ipccmd) - 1, "ethcamcliconncheck %d",g_uiEthcamCheckPortReadyIPAddr);

	sysMsg.sysCmdID = NVTIPC_SYSCMD_SYSCALL_REQ;
	sysMsg.DataAddr = (UINT32)ipccmd;
	sysMsg.DataSize = strlen(ipccmd) + 1;
	DBG_DUMP("^CBKCheckPortReady ipccmd=%s\r\n",ipccmd);

	if ((ipcErr = NvtIPC_MsgSnd(NVTIPC_SYS_QUEUE_ID, NVTIPC_SENDTO_CORE2, &sysMsg, sizeof(sysMsg))) < 0) {
		DBG_ERR("Failed to NVTIPC_SYS_QUEUE_ID\r\n");
	}
	EthCamNet_EthHubPortReadySendCmdTimerOpen();
#endif
#endif
	return NVTRET_OK;
}
static UINT32 BackgroundEthCamUpdateUIInfo(void)
{
#if(defined(_NVT_ETHREARCAM_RX_))
	FlowMovie_UpdateIcons(1);
#endif
	return NVTRET_OK;
}

UINT32 g_EthcamFwSize = 0;
UINT32 g_EthcamFwUpdateTimes = 0;//for waitmomentwindow update progressbar totail bar
static UINT32 BackgroundEthCamFWUpdate(void)
{
	UINT32 uiFwAddr, uiFwSize;
	FST_FILE hFile;
	static char uiUpdateFWName[64] ="A:\\FW671_AA.bin" ;//"FW671_AA";// "A:\\EthcamTxFW.bin";
	INT32 fst_er;
	UINT32 EthCamCmdRET=0;
	
    BOOL bRet = NVTRET_OK;
	
	uiFwSize = (UINT32)FileSys_GetFileLen(uiUpdateFWName);
	uiFwAddr = OS_GetMempoolAddr(POOL_ID_APP);

	hFile = FileSys_OpenFile(uiUpdateFWName, FST_OPEN_READ);
	if (hFile != 0) {
		fst_er = FileSys_ReadFile(hFile, (UINT8 *)uiFwAddr, &uiFwSize, 0, NULL);
		FileSys_CloseFile(hFile);
		if (fst_er != FST_STA_OK) {
			DBG_ERR("FW bin read fail\r\n");
			UxState_SetData(&UIFlowWndWaitMoment_StatusTXT_MsgCtrl, STATE_CURITEM, UIFlowWndWaitMoment_StatusTXT_Msg_STRID_ETHCAM_FW_LOSTED);
		
			bRet = NVTRET_ERROR;
            return bRet;
		}
	}
	g_EthcamFwSize = uiFwSize;
	DBGD(g_EthcamFwSize);
	if(g_EthcamFwSize!=0){
		CHKPNT;
		g_EthcamFwUpdateTimes = g_EthcamFwSize * 2 / (2222428/28) ;	
	}
	DBGD(g_EthcamFwUpdateTimes);
	
	DBG_DUMP("Total FwSize=%d\r\n",uiFwSize);
	EthCamSocketCli_SetCmdSendSizeCB(ETHCAM_PATH_ID_1, (UINT32)&socketCliEthCmd_SendSizeCB);
	EthCam_SendXMLCmd(ETHCAM_PATH_ID_1, ETHCAM_PORT_DEFAULT ,ETHCAM_CMD_TX_FWUPDATE_FWSEND, uiFwSize);
	EthCamCmdRET=EthCam_SendXMLData(ETHCAM_PATH_ID_1, (UINT8 *)uiFwAddr, uiFwSize);
	EthCamSocketCli_SetCmdSendSizeCB(ETHCAM_PATH_ID_1, 0);	
	
	if(EthCamCmdRET==ETHCAM_RET_OK){
        bRet = NVTRET_OK;
		DBG_DUMP("Send FW update Start\r\n");
		EthCam_SendXMLCmd(ETHCAM_PATH_ID_1, ETHCAM_PORT_DEFAULT ,ETHCAM_CMD_TX_FWUPDATE_START, 0);
	}else{
        bRet = NVTRET_ERROR;
		DBG_ERR("FW send error, %d\r\n",EthCamCmdRET);
	}
	CHKPNT;
	//UxState_SetData(&UIFlowWndWaitMoment_StatusTXT_MsgCtrl, STATE_CURITEM, UIFlowWndWaitMoment_StatusTXT_Msg_STRID_ETHCAM_UDFW_START);
	return bRet;
}


extern ETHCAM_TX_SET_OFFESET SlideOffsetInfo;

static UINT32 BackgroundEthCamSetTxSlideUp(void)
{
CHKPNT;
	#if(defined(_NVT_ETHREARCAM_RX_))
	UINT32 i;
	for (i=0; i<ETH_REARCAM_CAPS_COUNT; i++){
		debug_msg(" --SetTxSlideUp cj %d %d ",EthCamNet_GetEthLinkStatus(i),  socketCliEthData1_IsRecv(ETHCAM_PATH_ID_1 ) );
		if(EthCamNet_GetEthLinkStatus(i)==ETHCAM_LINK_UP && (socketCliEthData1_IsRecv(ETHCAM_PATH_ID_1 ))){
			CHKPNT;
			EthCam_SendXMLCmd(i, ETHCAM_PORT_DEFAULT ,ETHCAM_CMD_TX_SLIDEUP, 0);
			EthCam_SendXMLData(i, (UINT8 *)&SlideOffsetInfo, sizeof(ETHCAM_TX_SET_OFFESET));
		}
	}
			if ( SlideOffsetInfo.curDispOffset ==400)
			{
				SlideOffsetInfo.curDispOffset = 0;
			}

#endif
	return NVTRET_OK;
}

static UINT32 BackgroundEthCamSetTxSlideDown(void)
{
CHKPNT;
	#if(defined(_NVT_ETHREARCAM_RX_))
	UINT32 i;
	for (i=0; i<ETH_REARCAM_CAPS_COUNT; i++){
		debug_msg(" --SetTxSlidedown cj %d %d ",EthCamNet_GetEthLinkStatus(i),  socketCliEthData1_IsRecv(ETHCAM_PATH_ID_1 ) );
		if(EthCamNet_GetEthLinkStatus(i)==ETHCAM_LINK_UP && (socketCliEthData1_IsRecv(ETHCAM_PATH_ID_1 ))){
			CHKPNT;
			EthCam_SendXMLCmd(i, ETHCAM_PORT_DEFAULT ,ETHCAM_CMD_TX_SLIDEDOWN, 0);
			EthCam_SendXMLData(i, (UINT8 *)&SlideOffsetInfo, sizeof(ETHCAM_TX_SET_OFFESET));
		}
	}
	if ( SlideOffsetInfo.curDispOffset ==400)
	{
			SlideOffsetInfo.curDispOffset = 0;
	}
#endif
	return NVTRET_OK;
}

#endif
#if (AUDPROC_FUNC == ENABLE)
#include "AudProc.h"

static UINT32 BackgroundAudProcInit(void)
{
	CHKPNT;
	AudProc_Init();
	return NVTRET_OK;
}
#endif

void AppBKW_SetData(BKW_DATA_SET attribute, UINT32 value)
{
	switch (attribute) {
	case BKW_DPOF_MODE:
		g_uiDpofOPMode = value;
		break;
	case BKW_DPOF_NUM:
		g_uiDpofPrtNum = value;
		break;
	case BKW_DPOF_DATE:
		g_uiDpofDateOn = value;
		break;
	case BKW_PROTECT_TYPE:
		g_uiProtectType = value;
		break;
	case BKW_WAIT_TIME:
		g_uiWaitTime = value;
		break;
	case BKW_DZOOM_STOP:
		g_pDzoomStop = (UINT32 *)value;
		break;
	case BKW_ETHCAM_TRIGGER_THUMB_PATHID_P0:
		g_uiEthcamTriggerThumbPathId[0] = value;
		break;
	case BKW_ETHCAM_TRIGGER_THUMB_PATHID_P1:
		g_uiEthcamTriggerThumbPathId[1] = value;
		break;
	case BKW_ETHCAM_DEC_ERR_PATHID:
		g_uiEthcamDecErrPathId = value;
		break;
	case BKW_ETHCAM_ETHLINK_NOTIFY_STA:
		g_uiEthcamEthLinkNotifySta = value;
		break;
	case BKW_ETHCAM_CHECK_PORT_READY_IP:
		DBG_DUMP("SET PortReadyIP =%d\r\n",value);
		g_uiEthcamCheckPortReadyIPAddr = value;
		break;		
	default:
		debug_err(("[AppBKW_SetData]no this attribute"));
		break;
	}

}
UINT32 AppBKW_GetData(BKW_DATA_SET attribute)
{
	UINT32 value=0xffff;
	switch (attribute) {
	case BKW_ETHCAM_TRIGGER_THUMB_PATHID_P0:
		value=g_uiEthcamTriggerThumbPathId[0];
		break;
	case BKW_ETHCAM_TRIGGER_THUMB_PATHID_P1:
		value=g_uiEthcamTriggerThumbPathId[1];
		break;
	default:
		debug_err(("[AppBKW_GetData]no this attribute"));
		break;
	}
	return value;
}


