//global debug level: PRJ_DBG_LVL
#include "PrjCfg.h"

#include "SysCommon.h"
#include "SysCfg.h"
#include "AppLib.h"
#include "UIAppCommon.h"
#include "UIModeMovie.h"
#include "UIAppMovie.h"  // Isiah, implement YUV merge mode of recording func.
#include "UIAppMovie_Param.h"
#include "UIMovieMapping.h"

#include "UIModePhoto.h"
#include "UIAppPhoto.h"
#include "UIPhotoMapping.h"
#if (USE_FILEDB==ENABLE)
#include "FileDB.h"
#endif
#include "ImageApp_MovieCommon.h"
#include "ImageApp_MovieMulti.h"
#include "AppDisp_PipView.h"
#if (defined(_NVT_ETHREARCAM_TX_) || defined(_NVT_ETHREARCAM_RX_))
#include "EthCamAppCmd.h"
#endif
#include "SysCommon.h"
#include "UIMovieMapping.h"
//local debug level: THIS_DBGLVL
#define THIS_DBGLVL         2 // 0=FATAL, 1=ERR, 2=WRN, 3=UNIT, 4=FUNC, 5=IND, 6=MSG, 7=VALUE, 8=USER
///////////////////////////////////////////////////////////////////////////////
#define __MODULE__          UiAppMovieCmd
#define __DBGLVL__          ((THIS_DBGLVL>=PRJ_DBG_LVL)?THIS_DBGLVL:PRJ_DBG_LVL)
#define __DBGFLT__          "*" //*=All, [mark]=CustomClass
#include "DebugModule.h"
///////////////////////////////////////////////////////////////////////////////

static BOOL Cmd_movie(CHAR *strCmd)
{
	UINT32 menuIdx, value;

	sscanf_s(strCmd, "%d %d", &menuIdx, &value);

	if (menuIdx == 0) {
#if (MOVIE_REC_YUVMERGE == ENABLE)
		BOOL bEnable;
		UINT32 uiInterval;
		DBG_DUMP("Test merge record\r\n");
		sscanf_s(strCmd, "%d %d %d", &menuIdx, &bEnable, &uiInterval);
		FlowMovie_RecSetYUVMergeRecInterval(uiInterval);
		FlowMovie_RecSetYUVMergeRecCounter(0);
		FlowMovie_RecSetYUVMergeMode(bEnable);
		if (bEnable) {
			debug_msg("Enable YUV merge record mode, interval=%d\r\n", uiInterval);
		} else {
			debug_msg("Disable YUV merge record mode\r\n");
		}
#endif
	} else if (menuIdx == 1) {
#if(MOVIE_MODE==ENABLE)
		DBG_DUMP("ADAS(LDWS+FCW) %d\r\n", value);
		Ux_SendEvent(&CustomMovieObjCtrl,   NVTEVT_EXE_MOVIE_LDWS,          1,  value);
		Ux_SendEvent(&CustomMovieObjCtrl,   NVTEVT_EXE_MOVIE_FCW,           1,  value);
#endif
	} else if (menuIdx == 2) {
#if(MOVIE_MODE==ENABLE)
		DBG_DUMP("Urgent Protect Manual %d\r\n", value);
		Ux_SendEvent(&CustomMovieObjCtrl,   NVTEVT_EXE_MOVIE_PROTECT_MANUAL, 1,  value);
#endif
	} else if (menuIdx == 3) {
#if(MOVIE_MODE==ENABLE)
		DBG_DUMP("Image Rotation %d\r\n", value);
		Ux_SendEvent(&CustomMovieObjCtrl,   NVTEVT_EXE_MOVIE_SENSOR_ROTATE, 1,  value);
#endif
	} else if (menuIdx == 5) {
#if(MOVIE_MODE==ENABLE)
		DBG_DUMP("Movie RSC %d\r\n", value);
		Ux_SendEvent(&CustomMovieObjCtrl,   NVTEVT_EXE_MOVIE_RSC,          1,  value);
#endif
	} else if (menuIdx == 7) {
#if(PHOTO_MODE==ENABLE)
		UI_SetData(FL_DUAL_CAM, value);
#if(defined(_NVT_ETHREARCAM_RX_))
		MovieExe_EthCam_ChgDispCB(UI_GetData(FL_DUAL_CAM));
#endif
		DBG_DUMP("Sensor index %d\r\n", value);
		Ux_SendEvent(&CustomMovieObjCtrl,   NVTEVT_EXE_DUALCAM,      1, SysGetFlag(FL_DUAL_CAM));
#endif
	} else if (menuIdx == 11) {
#if(MOVIE_MODE==ENABLE)
		DBG_DUMP("Movie WDR index %d\r\n", value);
		Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_MOVIE_WDR, 1, value);
#endif
	} else if (menuIdx == 13) {
		DBG_DUMP("enter value=%d\r\n", value);
		if(value>=DUALCAM_FRONT && value<DUALCAM_SETTING_MAX )
		{
		}
		else
		{
		    value=DUALCAM_FRONT;
		}
#if(MOVIE_MODE==ENABLE)
		DBG_DUMP("out value=%d\r\n", value);
		UI_SetData(FL_DUAL_CAM, value);
		UI_SetData(FL_DUAL_CAM_MENU, value);

		//Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_IMAGE_RATIO, 1, GetMovieSizeRatio(UI_GetData(FL_MOVIE_SIZE)));
		PipView_SetStyle(UI_GetData(FL_DUAL_CAM));
#endif
	}

	return TRUE;
}
#if 0 //format free
static BOOL Cmd_movie_SetFmtFree(CHAR *strCmd)
{
	CHAR drive = 'A';
	UINT32 is_format_free;

	sscanf_s(strCmd, "%d", &is_format_free);

	ImageApp_MovieMulti_SetFormatFree(drive, is_format_free);

	return TRUE;
}

static BOOL Cmd_movie_SetFldInfo(CHAR *strCmd)
{
	CHAR   fld_full_path[128] = {0};
	UINT32 fld_ratio = 0;
	UINT64 fixed_size = 0;
	UINT32 is_set;

	sscanf_s(strCmd, "%d", &is_set);

	if (is_set) {

	//Set "Infinite"
	snprintf(fld_full_path, sizeof(fld_full_path), "A:\\Infinite\\");
	fld_ratio = 50;
	fixed_size = 190 * 1024 * 1024ULL;
	ImageApp_MovieMulti_SetFolderInfo(fld_full_path, fld_ratio, fixed_size);

	//Set "Event"
	snprintf(fld_full_path, sizeof(fld_full_path), "A:\\Event\\");
	fld_ratio = 10;
	fixed_size = 30 * 1024 * 1024ULL;
	ImageApp_MovieMulti_SetFolderInfo(fld_full_path, fld_ratio, fixed_size);

	//Set "Parking"
	snprintf(fld_full_path, sizeof(fld_full_path), "A:\\Parking\\");
	fld_ratio = 10;
	fixed_size = 30 * 1024 * 1024ULL;
	ImageApp_MovieMulti_SetFolderInfo(fld_full_path, fld_ratio, fixed_size);

	//Set "Manual"
	snprintf(fld_full_path, sizeof(fld_full_path), "A:\\Manual\\");
	fld_ratio = 10;
	fixed_size = 30 * 1024 * 1024ULL;
	ImageApp_MovieMulti_SetFolderInfo(fld_full_path, fld_ratio, fixed_size);

	//Set "Timelapse"
	snprintf(fld_full_path, sizeof(fld_full_path), "A:\\Timelapse\\");
	fld_ratio = 10;
	fixed_size = 30 * 1024 * 1024ULL;
	ImageApp_MovieMulti_SetFolderInfo(fld_full_path, fld_ratio, fixed_size);

	//Set "Image"
	snprintf(fld_full_path, sizeof(fld_full_path), "A:\\Image\\");
	fld_ratio = 10;
	fixed_size = 30 * 1024 * 1024ULL;
	ImageApp_MovieMulti_SetFolderInfo(fld_full_path, fld_ratio, fixed_size);

	} else {

	//Set "Movie"
	snprintf(fld_full_path, sizeof(fld_full_path), "A:\\Novatek\\Movie\\");
	fld_ratio = 10;//70;
	fixed_size = 190 * 1024 * 1024ULL;
	ImageApp_MovieMulti_SetFolderInfo(fld_full_path, fld_ratio, fixed_size);

	//Set "EMR"
	snprintf(fld_full_path, sizeof(fld_full_path), "A:\\Novatek\\EMR\\");
	fld_ratio = 80;//20;
	fixed_size = 30 * 1024 * 1024ULL;
	ImageApp_MovieMulti_SetFolderInfo(fld_full_path, fld_ratio, fixed_size);

	//Set "Photo"
	snprintf(fld_full_path, sizeof(fld_full_path), "A:\\Novatek\\Photo\\");
	fld_ratio = 10;
	fixed_size = 30 * 1024 * 1024ULL;
	ImageApp_MovieMulti_SetFolderInfo(fld_full_path, fld_ratio, fixed_size);

	}
	return TRUE;
}

static BOOL Cmd_movie_FMAllocate(CHAR *strCmd)
{
	CHAR drive = 'A';
	UINT32 is_set;

	sscanf_s(strCmd, "%d", &is_set);

	if (!is_set) {
		return TRUE;
	}

	ImageApp_MovieMulti_FMAllocate(drive);

	return TRUE;
}
static BOOL Cmd_movie_SetCyclicRec(CHAR *strCmd)
{
	CHAR dir_path[32]="A:\\Novatek\\Movie\\";
	UINT32 is_on;

	sscanf_s(strCmd, "%d", &is_on);

	DBG_DUMP("is_set=%d\r\n", is_on);

	ImageApp_MovieMulti_SetCyclicRec(dir_path, is_on);

	return TRUE;
}

static BOOL Cmd_movie_format_free_testflow(CHAR *strCmd)
{
	SxCmd_DoCommand("uimovie setfmtfree 1");
	SxCmd_DoCommand("uimovie setfldinfo 0");
	SxCmd_DoCommand("filesys format");
	SxCmd_DoCommand("uimovie fmallocate 1");
	SxCmd_DoCommand("uimovie cyclicrec 0");

	return TRUE;
}
#endif
BOOL Cmd_movie_dumpfdb(CHAR *strCmd)
{
#if (USE_FILEDB==ENABLE)
	FileDB_DumpInfo(0);
#if defined(_EMBMEM_EMMC_) && (FS_MULTI_STRG_FUNC==ENABLE)
	FileDB_DumpInfo(1);
#endif
#endif
	return TRUE;
}

#if(defined(_NVT_ETHREARCAM_TX_))
BOOL Cmd_movie_eth_tx_start(CHAR *strCmd)
{
	UINT32 value;
	sscanf_s(strCmd, "%d", &value);
	if(value==0){
		MovieExe_EthCamTxStart(_CFG_REC_ID_1);
	}else{
		MovieExe_EthCamTxStart(_CFG_CLONE_ID_1);
	}
	return TRUE;
}
BOOL Cmd_movie_eth_tx_stop(CHAR *strCmd)
{
	UINT32 value;
	sscanf_s(strCmd, "%d", &value);
	if(value==0){
		MovieExe_EthCamTxStop(_CFG_REC_ID_1);
	}else{
		MovieExe_EthCamTxStop(_CFG_CLONE_ID_1);
	}
	return TRUE;
}
#endif
BOOL Cmd_movie_HDR(CHAR *strCmd)
{
	UINT32 value;

	sscanf_s(strCmd, "%d", &value);
	if(value){
		DBG_DUMP("HDR On\r\n");
		Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_SHDR, 1, MOVIE_HDR_ON);

	}else{
		DBG_DUMP("HDR Off\r\n");
		Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_SHDR, 1, MOVIE_HDR_OFF);
	}
	Ux_SendEvent(0, NVTEVT_SYSTEM_MODE, 1, System_GetState(SYS_STATE_CURRMODE));

	return TRUE;
}

#if (defined(_NVT_ETHREARCAM_TX_) || defined(_NVT_ETHREARCAM_RX_))
BOOL Cmd_movie_get_desc(CHAR *strCmd)
{
	UINT8 tem_buf[50];
	MEM_RANGE desc;
	MEM_RANGE sps;
	MEM_RANGE pps;
	MEM_RANGE vps;
	UINT32 i;
	UINT8 *pBuf;
	desc.Addr=(UINT32)tem_buf;
	desc.Size=50;
	ImageApp_MovieMulti_GetDesc(_CFG_REC_ID_1, _CFG_CODEC_H265, &desc, &sps, &pps, &vps);
	DBG_DUMP("\r\ndesc[%d]:\r\n",desc.Size);
	pBuf=(UINT8 *)desc.Addr;
	for(i=0;i<desc.Size;i++){
		DBG_DUMP("0x%x, ", pBuf[i]);
	}
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

	return TRUE;
}
BOOL Cmd_movie_trigger_thumb(CHAR *strCmd)
{
#if(defined(_NVT_ETHREARCAM_TX_))
	ImageApp_MovieMulti_EthCam_TxTrigThumb((0 | ETHCAM_TX_MAGIC_KEY));
#else
	UINT16 i;
	for(i=0;i<ETH_REARCAM_CAPS_COUNT;i++){
		EthCam_SendXMLCmd(i, ETHCAM_PORT_DATA1 ,ETHCAM_CMD_GET_TX_MOVIE_THUMB, 0);
	}

#endif
	return TRUE;
}
BOOL Cmd_movie_trigger_rawencode(CHAR *strCmd)
{
#if(defined(_NVT_ETHREARCAM_TX_))
	if (System_GetState(SYS_STATE_CURRMODE) != PRIMARY_MODE_MOVIE) {
		DBG_ERR("not movie mode\r\n");
		return TRUE;
	}

	//680,510 media not record,also can raw encode
	if(ImageApp_MovieMulti_IsStreamRunning(_CFG_REC_ID_1 | ETHCAM_TX_MAGIC_KEY)){
		Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_MOVIE_REC_RAWENC, 0);
	} else {
		DBG_ERR("Not in recording state\r\n");

		return TRUE;
	}
#else
	UINT16 i;
	for(i=0;i<ETH_REARCAM_CAPS_COUNT;i++){
		EthCam_SendXMLCmd(i, ETHCAM_PORT_DATA1 ,ETHCAM_CMD_GET_TX_MOVIE_RAW_ENCODE, 0);
	}
#endif
	return TRUE;
}
BOOL Cmd_movie_disp_crop(CHAR *strCmd)
{
#if(defined(_NVT_ETHREARCAM_RX_))
#if(ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
	UINT32 value;
	sscanf_s(strCmd, "%d", &value);
	DBG_DUMP("value=%d\r\n",value);
	MOVIEMULTI_IME_CROP_INFO CropInfo ={0};
	CropInfo.IMESize.w = 1920;
	CropInfo.IMESize.h = 1080;
	CropInfo.IMEWin.x = 0;
	CropInfo.IMEWin.w = 1920;
	CropInfo.IMEWin.h = 384;
	if(value>=0 && ((value + CropInfo.IMEWin.h) < CropInfo.IMESize.h)){
		CropInfo.IMEWin.y = value;
		EthCam_SendXMLCmd(ETHCAM_PATH_ID_1, ETHCAM_PORT_DEFAULT ,ETHCAM_CMD_TX_DISP_CROP, 0);
		EthCam_SendXMLData(ETHCAM_PATH_ID_1, (UINT8 *)&CropInfo, (UINT32)sizeof(MOVIEMULTI_IME_CROP_INFO));
	}else{
		DBG_ERR("input out of range\r\n");
	}

#endif
#endif
	return TRUE;
}
#endif

SXCMD_BEGIN(uimovie, "uimovie command")
SXCMD_ITEM("movie %", Cmd_movie, "movie mode test")
SXCMD_ITEM("dumpfdb",   Cmd_movie_dumpfdb,   "dump filedb 0")
#if 0 //format free
SXCMD_ITEM("setfmtfree %", Cmd_movie_SetFmtFree, "Set Format Free")
SXCMD_ITEM("setfldinfo %", Cmd_movie_SetFldInfo, "Set Folder Info")
SXCMD_ITEM("fmallocate ", Cmd_movie_FMAllocate, "FM Allocate")
SXCMD_ITEM("cyclicrec %d", Cmd_movie_SetCyclicRec, "Set Cyclic Rec")
SXCMD_ITEM("fmtest", Cmd_movie_format_free_testflow, "FM test flow")
#endif
#if(defined(_NVT_ETHREARCAM_TX_))
SXCMD_ITEM("ethstart %",   Cmd_movie_eth_tx_start,   "eth stream tx start")
SXCMD_ITEM("ethstop %",   Cmd_movie_eth_tx_stop,   "eth stream tx stop")
#endif
SXCMD_ITEM("hdr %",   Cmd_movie_HDR,   "HDR on/off")
#if (defined(_NVT_ETHREARCAM_TX_) || defined(_NVT_ETHREARCAM_RX_))
SXCMD_ITEM("desc",   Cmd_movie_get_desc,   "get current resolution desc")
SXCMD_ITEM("thumb",   Cmd_movie_trigger_thumb,   "trigger thumb")
SXCMD_ITEM("pim",   Cmd_movie_trigger_rawencode,   "trigger raw encode")
#endif
#if(defined(_NVT_ETHREARCAM_RX_))
SXCMD_ITEM("ethcamcrop %",   Cmd_movie_disp_crop,   "ethcam set tx disp crop")
#endif
SXCMD_END()
