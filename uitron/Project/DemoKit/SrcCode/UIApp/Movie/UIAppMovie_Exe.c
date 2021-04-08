////////////////////////////////////////////////////////////////////////////////
#include "SysCommon.h"
//#include "AppCommon.h"
////////////////////////////////////////////////////////////////////////////////
#include "HwClock.h"
#include "UIAppCommon.h"
#include "UIAppMovie.h"
#include "UIAppPhoto.h"
#include "NVTUserCommand.h"
#include "UIMovieMapping.h"
#include "UIPhotoMapping.h"
#include "SysCfg.h" //for memory POOL_ID
#include "Audio.h"
#if FS_MULTI_STRG_FUNC
#include "DxStorage.h"
#endif
#if(PLAY_MODE==ENABLE)
#include "UIAppPlay.h"
#endif
#include "GxDisplay.h"

#include "sensor.h"
#include "DxSensor.h"

#if(STAMP_FUNC==ENABLE)
#include "MovieStamp.h"
#endif
#include "UIMode.h"
#include "PrjCfg.h"
#if _TODO
#include "IQS_Utility.h"
#endif
#include "SysMain.h"
#if !defined(_GSensor_None_)
#include "GSensor.h"
#endif
#include "UIAppMovie_Param.h"
//#NT#2016/06/14#Charlie Chang -begin
//#NT# for Contrast set
#include "iq_api.h"
//#NT#2016/06/14#Charlie Chang -end
//#NT#2013/05/15#Calvin Chang#Add customer's user data to MOV/MP4 file format -begin
#include "MovieUdtaVendor.h"
//#NT#2013/05/15#Calvin Chang -end
#if 1//(PIP_VIEW_FUNC == ENABLE)
#include "AppDisp_PipView.h"
#endif
#if(WIFI_FUNC==ENABLE)
#include "UIModeWifi.h"
#endif
#if(WIFI_AP_FUNC==ENABLE)
#include "WifiAppCmd.h"
#include "RtspNvtApi.h"
#include "WifiAppXML.h"
#include "UIAppWiFiCmd.h"
#endif
#include "UIPhotoFuncInfo.h"
#if (!defined(_Gyro_None_) && (MOVIE_RSC == ENABLE))
#include "rsc_common.h"
#endif
#include "ImageApp_Photo.h"
#include "ImageApp_MovieMulti.h"
#include "movieinterface_def.h"
#include "ImageUnit_AudIn.h"
#include "ImageUnit_ImgTrans.h"
#include "ImageUnit_UserProc.h"
#include "ImageUnit_VdoEnc.h"
#include "gpio.h"
#include "UIControlWnd.h"
#include "dma.h"
#if (defined(_BSP_NA51023_) && _TODO)
#include "dis_alg_fw.h"
#endif

#if(UCTRL_FUNC==ENABLE)
#include "UCtrlAppMovie.h"
#include "UCtrlMain.h"
#endif

#if (LOG_DBGINFO_IN_GPS_SECTION == ENABLE)
#include "mp4log.h"
#endif  // #if (LOG_DBGINFO_IN_GPS_SECTION == ENABLE)
#include "nvtmpp.h"
#include "videosprite.h"
#if (VIDEOSPRITEIPC==ENABLE)
#include "VideoSpriteIpc.h"
#endif
#include "iq_ui_info.h"
#include "ae_ui_info.h"
#include "af_ui_info.h"
#include "awb_ui_info.h"
#include "UIAppMovie_CommPoolTbl.h"
#include "alg_MovieExe.h"
#include "NMediaRecImgCap.h"
#if (AUDIO_FUNC_AAC == ENABLE)
#include "AudioCodec_AAC.h"
#endif
#if (VIDEO_FUNC_MJPG == ENABLE)
#include "VideoCodec_MJPG.h"
#endif
#if (VIDEO_FUNC_H264 == ENABLE)
#include "VideoCodec_H264.h"
#endif
#if (VIDEO_FUNC_H265 == ENABLE)
#include "VideoCodec_H265.h"
#endif

#include "FileDB.h"
#include "UIFlow.h"
#include "EthCamAppCmd.h"
#include "EthCamAppSocket.h"
#include "EthCamAppNetwork.h"
#include "EthCamSocket.h"
#include "UIModeUpdFw.h"
#if (USE_EXIF == ENABLE)
#include "Exif.h"
#include "ExifVendor.h"
#endif
#include "SxTimer.h"
#include "ImageUnit_VdoDec.h"
#include "timer.h"

#if defined(_ASR_ENABLE_)
#include "ASR.h"
#endif
#if (AUDPROC_FUNC == ENABLE)
#include "AudProc.h"
#endif
#include "ImageUnit_AudEnc.h"
#define TEST_510_64MB       DISABLE

//720p60
UINT8 EthCamSPSDefaultAddr[36]={0x00 ,0x00 ,0x00 ,0x01 ,0x67 ,0x64 ,0x00 ,0x33 ,0xac ,0x15 ,0x4a ,0x05 ,0x0 ,0x5b ,0xa6 ,0xe0 ,0x20 ,0x20 ,0x28 ,0x00 ,0x00 ,0x1f ,0x40 ,0x00 ,0x0e ,0xa6 ,0x00,0x20
		            ,0x00 ,0x00 ,0x00 ,0x01 ,0x68 ,0xee ,0x3c ,0xb0};


//#NT#2016/09/20#Bob Huang -begin
//#NT#Support HDMI Display with 3DNR Out
#if (_3DNROUT_FUNC == ENABLE)
BOOL gb3DNROut = FALSE;
#endif
//#NT#2016/09/20#Bob Huang -end

extern void System_DispSourceByTarget(ISIZE *pImgSize, USIZE *pAspectRatio);
extern UINT32 System_GetEnableDisp(void);
extern UINT32 System_GetEnableSensor(void);
extern void SMediaRec_SetEncPath(UINT32 uiVidEncId, MP_VDOENC_PATH path);

static void MovieExe_CalcFDDispCord(URECT *dispCord);
static void MovieExe_CalcOSDDispCord(URECT *dispCord);
#if(defined(_NVT_ETHREARCAM_RX_))
static USERPROC_TRIG_PROC_VAL MovieExe_EthCam_PIP_WifiCB(void);
#endif
#define THIS_DBGLVL         2 // 0=FATAL, 1=ERR, 2=WRN, 3=UNIT, 4=FUNC, 5=IND, 6=MSG, 7=VALUE, 8=USER
///////////////////////////////////////////////////////////////////////////////
#define __MODULE__          UiAppMovie
#define __DBGLVL__          ((THIS_DBGLVL>=PRJ_DBG_LVL)?THIS_DBGLVL:PRJ_DBG_LVL)
#define __DBGFLT__          "*" //*=All, [mark]=CustomClass
//#define __DBGFLT__          "[cap]"
//#define __DBGFLT__          "[cb]"
//#define __DBGFLT__          "[set]"
//#define __DBGFLT__          "[zoom]"
//#define __DBGFLT__          "[af]"
#include "DebugModule.h"

#define FILE_SN_MAX		999999

//---------------------UIAPP_MOVIE Global Variables----------

BOOL _g_bFirstMovie = TRUE;
BOOL _g_bFirstMovieRec = FALSE;
BOOL bIsWiFiRecorded = FALSE;
static BOOL g_bAllowRawEnc=TRUE;

#define RTSP_MAX_CLIENT         1


static USIZE IMAGERATIO_SIZE[IMAGERATIO_MAX_CNT] = {
	{9, 16}, //IMAGERATIO_9_16
	{2, 3}, //IMAGERATIO_2_3
	{3, 4}, //IMAGERATIO_3_4
	{1, 1}, //IMAGERATIO_1_1
	{4, 3}, //IMAGERATIO_4_3
	{3, 2}, //IMAGERATIO_3_2
	{16, 9}, //IMAGERATIO_16_9
};
#if _TODO
#if((WIFI_AP_FUNC==ENABLE)&&(SENSOR_CAPS_COUNT == 2))
static USIZE MOVIE_HTTPLVIEW_SIZE[IMAGERATIO_MAX_CNT] = {
	{HTTP_MJPG_W_4, HTTP_MJPG_H_3}, //IMAGERATIO_9_16
	{HTTP_MJPG_W_4, HTTP_MJPG_H_3}, //IMAGERATIO_2_3
	{HTTP_MJPG_W_4, HTTP_MJPG_H_3}, //IMAGERATIO_3_4
	{HTTP_MJPG_W_4, HTTP_MJPG_H_3}, //IMAGERATIO_1_1
	{HTTP_MJPG_W_4, HTTP_MJPG_H_3}, //IMAGERATIO_4_3
	{HTTP_MJPG_W_4, HTTP_MJPG_H_3}, //IMAGERATIO_3_2
	{HTTP_MJPG_W_16, HTTP_MJPG_H_9}, //IMAGERATIO_16_9
};
#endif
#endif

static BOOL    bSDIsSlow    = FALSE;
static UINT32 g_FileSerialNum = 0;
//#NT#2016/06/08#Lincy Lin -begin
//#NT#Implement generic OSD and video drawing mechanism for ALG function
URECT  gMovieFdDispCord;
URECT  gMovieOsdDispCord;
//#NT#2016/06/08#Lincy Lin -end
// For UCTRL use.
static UINT32 g_uiAudChannel = MOVREC_AUDTYPE_RIGHT;
static UINT32 g_uiAudSampleRate = 32000;
static UINT32 g_uiAudCodec = MOVAUDENC_AAC;
#if(defined(_NVT_ETHREARCAM_RX_))
static IMG_BUF g_EthcamWifiCBImg[ETH_REARCAM_CAPS_COUNT] = {0};
#endif
//-------------------------------------------------------------------------------------------------
extern MOVIE_RECODE_INFO gMovie_Rec_Info[SENSOR_MAX_NUM];
extern MOVIE_RECODE_INFO gMovie_Clone_Info[SENSOR_MAX_NUM];
extern MOVIE_STRM_INFO gMovie_Strm_Info;
extern MOVIE_RECODE_FILE_OPTION gMovie_Rec_Option;
extern MOVIEMULTI_RECODE_FOLDER_NAMING gMovie_Folder_Naming[SENSOR_MAX_NUM];
extern MOVIEMULTI_RECODE_FOLDER_NAMING gMovie_Clone_Folder_Naming[SENSOR_MAX_NUM];
extern MOVIEMULTI_RECODE_FOLDER_NAMING gMovie_EthCam_Folder_Naming[1];
extern MOVIEMULTI_AUDIO_INFO   gMovie_Audio_Info;
extern MOVIEMULTI_CFG_DISP_INFO gMovie_Disp_Info;
extern SX_CMD_ENTRY uimovie[];
#if (defined(_NVT_ETHREARCAM_RX_) ||defined(_NVT_ETHREARCAM_TX_))
extern int SX_TIMER_ETHCAM_LINKDET_ID;
#endif
#if(defined(_NVT_ETHREARCAM_RX_))
extern UINT32 EthCamHB1[ETHCAM_PATH_ID_MAX], EthCamHB2;
static UINT32 g_uiEthcamEncBufSec[]= {3, 3};
extern int SX_TIMER_ETHCAM_LINKDET_ID;
#if (ETH_REARCAM_CAPS_COUNT>=2)
extern int SX_TIMER_ETHCAM_ETHHUB_LINKDET_ID;
#define ETHCAM_DISP_MAX_QUEUE 0
#else
UINT32 g_isEthCamDispDecErr[ETHCAM_PATH_ID_MAX]={0};
#define ETHCAM_DISP_MAX_QUEUE 0
#if(ETHCAM_DISP_MAX_QUEUE>0)
static SEND_FRM_BSQ g_EthCamDispQ={0};
static UINT8 g_EthCamDispPoolAddr[ETHCAM_DISP_MAX_QUEUE*sizeof(ISF_DATA)]={0};
static TIMER_ID  g_EthCamDisp_TimerID = 0;
#endif
#endif
#else
	#define ETHCAM_DISP_MAX_QUEUE 0
#endif
#if (defined(_NVT_ETHREARCAM_TX_))
extern UINT32 EthCamTxHB;
static NVTMPP_VB_POOL g_EthCamRecId1_GetBsMapTblPool= NVTMPP_VB_INVALID_POOL;
static UINT32 g_EthCamRecId1_GetBsMapTblPoolAddr=0;
SEND_FRM_BSQ g_RecId1SendFrmBsQ={0};
static NVTMPP_VB_POOL g_EthCamCloneId1_GetBsMapTblPool= NVTMPP_VB_INVALID_POOL;
static UINT32 g_EthCamCloneId1_GetBsMapTblPoolAddr=0;
SEND_FRM_BSQ g_CloneId1SendFrmBsQ={0};
static UINT32 g_EthCamRecId1_VdoEncBufSec=3;
#endif
static UINT32 gMovie_InitCommonMemFinish=0;
//-------------------------------------------------------------------------------------------------

#if (MOVIE_MODE_D2D_FUNC == ENABLE)
static BOOL	  bMovieD2DModeEn = TRUE;
#else
static BOOL	  bMovieD2DModeEn = FALSE;
#endif
void MovieExe_SetMovieD2DModeEn(BOOL Enable)
{
	bMovieD2DModeEn = Enable;
}

//#NT#2016/05/31#Ben Wang -begin
//#NT#Add UVC multimedia function.
#if (MOVIE_UVAC_FUNC == ENABLE || IPCAM_UVC_FUNC == ENABLE)
#include "ImageUnit_UsbUVAC.h"
#include "UVAC.h"

static UVAC_VID_RESO gUIUvacVidReso[1] = {
	{1920,  1080,   1,      30,      0,      0},        //16:9
};

//NVT_UI_UVAC_AUD_SAMPLERATE_CNT <= UVAC_AUD_SAMPLE_RATE_MAX_CNT
static UINT32 gUIUvacAudSampleRate[1] = {
	32000
};

_ALIGNED(64) static UINT16 m_UVACSerialStrDesc3[] = {
	0x0320,                             // 20: size of String Descriptor = 32 bytes
	// 03: String Descriptor type
	'9', '6', '6', '7', '0',            // 96670-00000-001 (default)
	'0', '0', '0', '0', '0',
	'0', '0', '1', '0', '0'
};

_ALIGNED(64) const static UINT8 m_UVACManuStrDesc[] = {
	USB_VENDER_DESC_STRING_LEN * 2 + 2, // size of String Descriptor = 6 bytes
	0x03,                       // 03: String Descriptor type
	USB_VENDER_DESC_STRING
};

_ALIGNED(64) const static UINT8 m_UVACProdStrDesc[] = {
	USB_PRODUCT_DESC_STRING_LEN * 2 + 2, // size of String Descriptor = 6 bytes
	0x03,                       // 03: String Descriptor type
	USB_PRODUCT_DESC_STRING
};

static UVAC_VEND_DEV_DESC gUIMovUvacDevDesc = {0};

static void xUSBMakerInit_UVAC(UVAC_VEND_DEV_DESC *pUVACDevDesc)
{
	pUVACDevDesc->pManuStringDesc = (UVAC_STRING_DESC *)m_UVACManuStrDesc;
	pUVACDevDesc->pProdStringDesc = (UVAC_STRING_DESC *)m_UVACProdStrDesc;
	pUVACDevDesc->pSerialStringDesc = (UVAC_STRING_DESC *)m_UVACSerialStrDesc3;
	pUVACDevDesc->VID = USB_VID;
	pUVACDevDesc->PID = USB_PID_PCCAM;
}

#if 0
static UINT32 xUvac_StartVideoCB(UVAC_VID_DEV_CNT vidDevIdx, UVAC_STRM_INFO *pStrmInfo)
{
	return 0;
}

static void xUvac_StopVideoCB(UVAC_VID_DEV_CNT vidDevIdx)
{
}

static UINT32 xUvac_SetVolumeCB(UINT32 volume)
{
	return 0;
}
#endif

#if(UVC_MULTIMEDIA_FUNC == ENABLE)
#include "USBCDC.h"
static BOOL CdcPstnReqCB(CDC_COM_ID ComID, UINT8 Code, UINT8 *pData, UINT32 *pDataLen)
{
	BOOL bSupported = TRUE;
	CDCLineCoding LineCoding;
	DBGD(ComID);
	switch (Code) {
	case REQ_GET_LINE_CODING:
		DBG_DUMP("Get Line Coding\r\n");
		LineCoding.uiBaudRateBPS = 115200;
		LineCoding.uiCharFormat = 0;//CDC_LINEENCODING_OneStopBit;
		LineCoding.uiParityType = 0;//CDC_PARITY_None;
		LineCoding.uiDataBits = 8;
		*pDataLen = sizeof(LineCoding);
		memcpy(pData, &LineCoding, *pDataLen);
		break;
	case REQ_SET_LINE_CODING:
		DBG_DUMP("Set Line Coding\r\n");
		if (*pDataLen == sizeof(LineCoding)) {
			memcpy(&LineCoding, pData, *pDataLen);
		} else {
			bSupported = FALSE;
		}
		break;
	case REQ_SET_CONTROL_LINE_STATE:
		DBG_DUMP("Control Line State = 0x%X\r\n", *(UINT16 *)pData);
		//debug console test
		if (*(UINT16 *)pData == 0x3) { //console ready
		}
		break;
	default:
		bSupported = FALSE;
		break;
	}
	return bSupported;
}
#endif
#endif

//#NT#2016/05/31#Ben Wang -end
void MovieExe_IPL_SetIInfo(UINT32 index, UINT32 value)
{
    MOVIEMULTI_RECODE_IPL_INFO ipl_info = {0};
    UINT8 i;

    ipl_info.item  = index;
    ipl_info.value = value;
    for (i = 0; i < SENSOR_CAPS_COUNT; i++)	{
        ImageApp_MovieMulti_SetParam(i, MOVIEMULTI_PARAM_IPL_INFO, (UINT32) &ipl_info);
        //ImageApp_Movie_SetParam(_CFG_REC_ID_1, MOVIE_PARAM_IPL_INFO, (UINT32) &ipl_info);
    }
}

void MovieExe_IQ_SetUIInfo(UINT32 index, UINT32 value)
{
	if (System_GetEnableSensor() & SENSOR_1) {
		iq_set_ui_info(IPL_PATH(0),index,value);
	}
	if (System_GetEnableSensor() & SENSOR_2) {
		iq_set_ui_info(IPL_PATH(1),index,value);
	}
}

void MovieExe_AE_SetUIInfo(UINT32 index, UINT32 value)
{
	if (System_GetEnableSensor() & SENSOR_1) {
		ae_set_ui_info(IPL_PATH(0),index,value);
	}
	if (System_GetEnableSensor() & SENSOR_2) {
		ae_set_ui_info(IPL_PATH(1),index,value);
	}
}

void MovieExe_AF_SetUIInfo(UINT32 index, UINT32 value)
{
	if (System_GetEnableSensor() & SENSOR_1) {
		af_set_ui_info(IPL_PATH(0),index,value);
	}
	if (System_GetEnableSensor() & SENSOR_2) {
		af_set_ui_info(IPL_PATH(1),index,value);
	}
}

void MovieExe_AWB_SetUIInfo(UINT32 index, UINT32 value)
{
	if (System_GetEnableSensor() & SENSOR_1) {
		awb_set_ui_info(IPL_PATH(0),index,value);
	}
	if (System_GetEnableSensor() & SENSOR_2) {
		awb_set_ui_info(IPL_PATH(1),index,value);
	}
}


void MovieExe_Cap_SetIInfo(UINT32 index, UINT32 value)
{
#if (MOVIE_PIM_CAPTURE == ENABLE)
	ImageUnit_Begin(&ISF_Cap, 0);
	ImageUnit_SetParam(ISF_CTRL, index, value);
	ImageUnit_End();
#endif
}

ISIZE Movie_GetBufferSize(void) //return USER CUSTOM buffer size
{
	return GxVideo_GetDeviceSize(DOUT1);
}

void Movie_SetSDSlow(BOOL IsSlow)
{
	bSDIsSlow = IsSlow;
}

UINT32 Movie_GetAudChannel(void)
{
	return g_uiAudChannel;
}

void Movie_SetAudChannel(UINT32 uiAudChannel)
{
	g_uiAudChannel = uiAudChannel;
}

UINT32 Movie_GetAudSampleRate(void)
{
	return g_uiAudSampleRate;
}

void Movie_SetAudSampleRate(UINT32 uiAudSampleRate)
{
	g_uiAudSampleRate = uiAudSampleRate;
}

UINT32 Movie_GetAudCodec(void)
{
	return g_uiAudCodec;
}

void Movie_SetAudCodec(UINT32 uiAudCodec)
{
	g_uiAudCodec = uiAudCodec;
}

void MovieExe_SetDateStamp(void)
{

}
UINT32 MovieExe_GetTBR(MOVIE_CFG_REC_ID  rec_id)
{
	if(rec_id==_CFG_REC_ID_1){
		return gMovie_Rec_Info[_CFG_REC_ID_1].target_bitrate;
	}else{
		return gMovie_Clone_Info[rec_id-_CFG_CLONE_ID_1].target_bitrate;
}
}
UINT32 MovieExe_GetWidth(MOVIE_CFG_REC_ID  rec_id)
{
	if(rec_id==_CFG_REC_ID_1){
		return gMovie_Rec_Info[_CFG_REC_ID_1].size.w;
	}else{
		return gMovie_Clone_Info[rec_id-_CFG_CLONE_ID_1].size.w;
}
}
UINT32 MovieExe_GetHeight(MOVIE_CFG_REC_ID  rec_id)
{
	if(rec_id==_CFG_REC_ID_1){
		return gMovie_Rec_Info[_CFG_REC_ID_1].size.h;
	}else{
		return gMovie_Clone_Info[rec_id-_CFG_CLONE_ID_1].size.h;
}
}
UINT32 MovieExe_GetFps(MOVIE_CFG_REC_ID  rec_id)
{
	if(rec_id==_CFG_REC_ID_1){
		return gMovie_Rec_Info[_CFG_REC_ID_1].frame_rate;
	}else{
		return gMovie_Clone_Info[rec_id-_CFG_CLONE_ID_1].frame_rate;
}
}
UINT32 MovieExe_GetGOP(MOVIE_CFG_REC_ID  rec_id)
{
	if(rec_id==_CFG_REC_ID_1){
		return gMovie_Rec_Info[_CFG_REC_ID_1].cbr_info.uiGOP;
	}else{
		return gMovie_Clone_Info[rec_id-_CFG_CLONE_ID_1].cbr_info.uiGOP;
}
}
UINT32 MovieExe_GetCodec(MOVIE_CFG_REC_ID  rec_id)
{
	if(rec_id==_CFG_REC_ID_1){
		return gMovie_Rec_Info[_CFG_REC_ID_1].codec;
	}else{
		return gMovie_Clone_Info[rec_id-_CFG_CLONE_ID_1].codec;
	}
}
MOVIE_RECODE_INFO MovieExe_GetRecInfo(MOVIE_CFG_REC_ID  rec_id)
{
	if(rec_id==_CFG_REC_ID_1){
		return gMovie_Rec_Info[_CFG_REC_ID_1];
	}else{
		return gMovie_Clone_Info[rec_id-_CFG_CLONE_ID_1];
	}
}
MOVIEMULTI_AUDIO_INFO MovieExe_GetAudInfo(void)
{
	return gMovie_Audio_Info;
}

UINT32 MovieExe_GetEmrRollbackSec(void)
{
	return gMovie_Rec_Option.emr_sec;
}
#if(defined(_NVT_ETHREARCAM_RX_))
UINT32 MovieExe_GetEthcamEncBufSec(ETHCAM_PATH_ID  path_id)
{
	return g_uiEthcamEncBufSec[path_id];
}
void MovieExe_SetEthcamEncBufSec(ETHCAM_PATH_ID  path_id, UINT32 Sec)
{
	g_uiEthcamEncBufSec[path_id]=Sec;
}
#endif

void MovieExe_CalcDispSize(ISIZE *pDispSize, ISIZE *pDevSize, ISIZE *pDevRatio, ISIZE *pImgRatio)
{
	if (((pDevRatio->w * 100) / pDevRatio->h) > ((pImgRatio->w * 100) / pImgRatio->h)) { // device aspect ratio > image ratio
		pDispSize->h = pDevSize->h;
		pDispSize->w = ALIGN_ROUND_16((pDevSize->w * pImgRatio->w * pDevRatio->h) / (pImgRatio->h * pDevRatio->w));
	} else {
		pDispSize->w = pDevSize->w;
		pDispSize->h = ALIGN_ROUND_4((pDevSize->h * pImgRatio->h * pDevRatio->w) / (pImgRatio->w * pDevRatio->h));
	}
}

void MovieExe_GetRecSize(UINT32 rec_id, ISIZE *rec_size)
{
	rec_size->w = gMovie_Rec_Info[rec_id].size.w;
	rec_size->h = gMovie_Rec_Info[rec_id].size.h;
}

#if MOVIE_COMMPOOL_AUTO_SETTING
// just for general case, not for special case usage!
UINT32 MovieExe_CalcCommPoolSize(UINT32 w, UINT32 h, BOOL compress)
{
	if (compress) {
		return ((w * h * 9) / 8 + 1024);
	} else {
		#if MOVIE_DIS
		return ((ALIGN_CEIL_16(w*11/10) * ALIGN_CEIL_16(h*11/10) * 3) / 2 + 1024);
		#else
		return ((w * h * 3) / 2 + 1024);
		#endif
	}
}
INT32 MovieExe_InitCommonMem(void)
{
	UINT32 movie_size_idx;
	UINT32 i, rec_type;
	USIZE  rec[SENSOR_MAX_NUM] = {0};
	USIZE  clone[SENSOR_MAX_NUM] = {0};
	USIZE  wifi, pool[16];
	ISIZE  disp;
	UINT32 pool_id = 0;

	movie_size_idx = UI_GetData(FL_MOVIE_SIZE);
	rec_type = MovieMapping_GetRecType(movie_size_idx);
	disp = GxVideo_GetDeviceSize(DOUT1);
	debug_msg("disp.h=%d,disp.w=%d\r\n",disp.h,disp.w);
	MovieMapping_GetStreamInfo(movie_size_idx, (UINT32) &gMovie_Strm_Info);

	memset(g_movie_common_pool_tbl, 0, sizeof(g_movie_common_pool_tbl));

	// main movie, ipl id 0
	rec[0].w = MovieMapping_GetWidth(movie_size_idx, 0);
	rec[0].h = MovieMapping_GetHeight(movie_size_idx, 0);
	#if ((IME_TMNR_FUNC == DISABLE) && (MOVIE_YUV_COMPRESS == ENABLE))
	g_movie_common_pool_tbl[pool_id].blk_size = MovieExe_CalcCommPoolSize(rec[0].w, rec[0].h, TRUE);
	pool[pool_id].w = (rec[0].w * 3) / 4;
	pool[pool_id].h = rec[0].h;
	#else
	g_movie_common_pool_tbl[pool_id].blk_size = MovieExe_CalcCommPoolSize(rec[0].w, rec[0].h, FALSE);
	DBGH(g_movie_common_pool_tbl[pool_id].blk_size);
	pool[pool_id].w = rec[0].w;
	pool[pool_id].h = rec[0].h;
	#endif
	g_movie_common_pool_tbl[pool_id].blk_cnt = 4;
	g_movie_common_pool_tbl[pool_id].ddr = NVTMPP_DDR_1;
	pool_id++;

	if (rec_type == MOVIE_REC_TYPE_FRONT || rec_type == MOVIE_REC_TYPE_CLONE) {

		// display
		g_movie_common_pool_tbl[pool_id].blk_size = MovieExe_CalcCommPoolSize(disp.w, disp.h, FALSE);
		DBGH(g_movie_common_pool_tbl[pool_id].blk_size);
		#if (OSD_USE_ROTATE_BUFFER == ENABLE)
		g_movie_common_pool_tbl[pool_id].blk_cnt = 5;
		#else
		g_movie_common_pool_tbl[pool_id].blk_cnt = 4;
		#endif
		g_movie_common_pool_tbl[pool_id].ddr = NVTMPP_DDR_1;
		pool[pool_id].w = disp.w;
		pool[pool_id].h = disp.h;
		pool_id++;

		// wifi, uvc
		wifi.w = gMovie_Strm_Info.size.w;
		wifi.h = gMovie_Strm_Info.size.h;
		g_movie_common_pool_tbl[pool_id].blk_size = MovieExe_CalcCommPoolSize(wifi.w, wifi.h, FALSE);
		g_movie_common_pool_tbl[pool_id].blk_cnt = 4;
		g_movie_common_pool_tbl[pool_id].ddr = NVTMPP_DDR_1;
		pool[pool_id].w = wifi.w;
		pool[pool_id].h = wifi.h;
		pool_id++;

		if (rec_type == MOVIE_REC_TYPE_CLONE) {
			// clone movie, ipl id 0, should be the same as wifi!
			clone[0].w = MovieMapping_GetCloneWidth(movie_size_idx, 0);
			clone[0].h = MovieMapping_GetCloneHeight(movie_size_idx, 0);
			if ((clone[0].w == wifi.w) && (clone[0].h == wifi.h)) {
				g_movie_common_pool_tbl[pool_id-1].blk_cnt = 5; // or 6?
			} else {
				DBG_ERR("Clone movie size should be the same as WiFi!\r\n");
				g_movie_common_pool_tbl[pool_id].blk_size = MovieExe_CalcCommPoolSize(clone[0].w, clone[0].h, FALSE);
				g_movie_common_pool_tbl[pool_id].blk_cnt = 4;
				g_movie_common_pool_tbl[pool_id].ddr = NVTMPP_DDR_1;
				pool[pool_id].w = clone[0].w;
				pool[pool_id].h = clone[0].h;
				pool_id++;
			}
		}

	} else if (rec_type == MOVIE_REC_TYPE_DUAL || rec_type == MOVIE_REC_TYPE_DUAL_CLONE) {

		// main movie, ipl id 1
		rec[1].w = MovieMapping_GetWidth(movie_size_idx, 1);
		rec[1].h = MovieMapping_GetHeight(movie_size_idx, 1);
		g_movie_common_pool_tbl[pool_id].blk_size = MovieExe_CalcCommPoolSize(rec[1].w, rec[1].h, FALSE);
		g_movie_common_pool_tbl[pool_id].blk_cnt = 4;
		g_movie_common_pool_tbl[pool_id].ddr = NVTMPP_DDR_1;
		pool[pool_id].w = rec[1].w;
		pool[pool_id].h = rec[1].h;
		pool_id++;

		// display: pip view
		g_movie_common_pool_tbl[pool_id].blk_size = MovieExe_CalcCommPoolSize(disp.w, disp.h, FALSE);
		#if (OSD_USE_ROTATE_BUFFER == ENABLE)
		g_movie_common_pool_tbl[pool_id].blk_cnt = 7;
		#else
		g_movie_common_pool_tbl[pool_id].blk_cnt = 6;
		#endif
		g_movie_common_pool_tbl[pool_id].ddr = NVTMPP_DDR_1;
		pool[pool_id].w = disp.w;
		pool[pool_id].h = disp.h;
		pool_id++;

		// wifi, uvc: pip view
		wifi.w = gMovie_Strm_Info.size.w;
		wifi.h = gMovie_Strm_Info.size.h;
		g_movie_common_pool_tbl[pool_id].blk_size = MovieExe_CalcCommPoolSize(wifi.w, wifi.h, FALSE);
		g_movie_common_pool_tbl[pool_id].blk_cnt = 5;
		g_movie_common_pool_tbl[pool_id].ddr = NVTMPP_DDR_1;
		pool[pool_id].w = wifi.w;
		pool[pool_id].h = wifi.h;
		pool_id++;

		if (rec_type == MOVIE_REC_TYPE_DUAL_CLONE) {
			// clone movie, ipl id 0 and 1, should be the same as wifi!
			clone[0].w = MovieMapping_GetCloneWidth(movie_size_idx, 0);
			clone[0].h = MovieMapping_GetCloneHeight(movie_size_idx, 0);
			clone[1].w = MovieMapping_GetCloneWidth(movie_size_idx, 1);
			clone[1].h = MovieMapping_GetCloneHeight(movie_size_idx, 1);
			if ((clone[0].w == wifi.w) && (clone[0].w == clone[1].w) && (clone[0].h == wifi.h) && (clone[0].h == clone[1].h)) {
				g_movie_common_pool_tbl[pool_id-1].blk_cnt = 8;
			} else {
				DBG_ERR("Clone movie size should be the same as WiFi!\r\n");
				if ((clone[0].w == clone[1].w) && (clone[0].h == clone[1].h)) {
					g_movie_common_pool_tbl[pool_id].blk_size = MovieExe_CalcCommPoolSize(clone[0].w, clone[0].h, FALSE);
					g_movie_common_pool_tbl[pool_id].blk_cnt = 8;
					g_movie_common_pool_tbl[pool_id].ddr = NVTMPP_DDR_1;
					pool[pool_id].w = clone[0].w;
					pool[pool_id].h = clone[0].h;
					pool_id++;
				} else {
					DBG_ERR("Clone movie size for IPL 0 and 1 should be the same!\r\n");
					g_movie_common_pool_tbl[pool_id].blk_size = MovieExe_CalcCommPoolSize(clone[0].w, clone[0].h, FALSE);
					g_movie_common_pool_tbl[pool_id].blk_cnt = 4;
					g_movie_common_pool_tbl[pool_id].ddr = NVTMPP_DDR_1;
					pool[pool_id].w = clone[0].w;
					pool[pool_id].h = clone[0].h;
					pool_id++;

					g_movie_common_pool_tbl[pool_id].blk_size = MovieExe_CalcCommPoolSize(clone[1].w, clone[1].h, FALSE);
					g_movie_common_pool_tbl[pool_id].blk_cnt = 4;
					g_movie_common_pool_tbl[pool_id].ddr = NVTMPP_DDR_1;
					pool[pool_id].w = clone[1].w;
					pool[pool_id].h = clone[1].h;
					pool_id++;
				}
			}
		}

	}

	// alg
	#if (_TSR_FUNC_|| MOVIE_OT_FUNC)
	g_movie_common_pool_tbl[pool_id].blk_size = MovieExe_CalcCommPoolSize(1920, 1080, FALSE);
	g_movie_common_pool_tbl[pool_id].blk_cnt = 2;
	g_movie_common_pool_tbl[pool_id].ddr = NVTMPP_DDR_1;
	pool[pool_id].w = 1920;
	pool[pool_id].h = 1080;
	pool_id++;
	#endif

	#if (_ADAS_FUNC_ == ENABLE)
	g_movie_common_pool_tbl[pool_id].blk_size = (720 * 480 + 1024) * 2; // special case
	g_movie_common_pool_tbl[pool_id].blk_cnt = 4;
	g_movie_common_pool_tbl[pool_id].ddr = NVTMPP_DDR_1;
	pool[pool_id].w = 720;
	pool[pool_id].h = 480;
	pool_id++;
	#endif


    NVTMPP_ER        ret;
    NVTMPP_VB_CONF_S st_conf;
    NVT_VB_CPOOL_S   *p_comm_pool;

    memset(&st_conf, 0, sizeof(NVTMPP_VB_CONF_S));
    st_conf.ddr_mem[0].Addr = OS_GetMempoolAddr(POOL_ID_APP);
    st_conf.ddr_mem[0].Size = OS_GetMempoolSize(POOL_ID_APP);

#if _TODO
    if (POOL_ID_APP_ARBIT2)
#else
    if (0)
#endif
    {
        st_conf.ddr_mem[1].Addr = OS_GetMempoolAddr(POOL_ID_APP_ARBIT2);
        st_conf.ddr_mem[1].Size = OS_GetMempoolSize(POOL_ID_APP_ARBIT2);
    }
    st_conf.max_pool_cnt = 64;
    st_conf.use_reserved_mem = TRUE;

	p_comm_pool = &g_movie_common_pool_tbl[0];
	for (i=0;i<NVTMPP_VB_MAX_COMM_POOLS;i++){
		if (p_comm_pool->blk_size == 0)
			break;
		DBG_DUMP("^C[%d]blk_size 0x%x, w %d, h %d, blk_cnt %d, ddr %d\r\n",
			i, p_comm_pool->blk_size, pool[i].w, pool[i].h, p_comm_pool->blk_cnt, p_comm_pool->ddr);
		st_conf.common_pool[i] = *p_comm_pool;
		p_comm_pool++;
	}

    ret = nvtmpp_vb_set_conf(&st_conf);
    if (NVTMPP_ER_OK != ret) {
        DBG_ERR("nvtmpp set vb err: %d\r\n", ret);
        return -1;
    }

    ret = nvtmpp_vb_init();
    if (NVTMPP_ER_OK != ret) {
        DBG_ERR("nvtmpp init vb err: %d\r\n", ret);
        return -1;
    }
    gMovie_InitCommonMemFinish=1;
    return 0;
}
#else
INT32 MovieExe_InitCommonMem(void)
{
    NVTMPP_ER        ret;
    NVTMPP_VB_CONF_S st_conf;
	UINT32           i;
    NVT_VB_CPOOL_S   *p_comm_pool;

    memset(&st_conf, 0, sizeof(NVTMPP_VB_CONF_S));
    st_conf.ddr_mem[0].Addr = OS_GetMempoolAddr(POOL_ID_APP);
    st_conf.ddr_mem[0].Size = OS_GetMempoolSize(POOL_ID_APP);

#if _TODO
    if (POOL_ID_APP_ARBIT2)
#else
    if (0)
#endif
    {
        st_conf.ddr_mem[1].Addr = OS_GetMempoolAddr(POOL_ID_APP_ARBIT2);
        st_conf.ddr_mem[1].Size = OS_GetMempoolSize(POOL_ID_APP_ARBIT2);
    }
    st_conf.max_pool_cnt = 64;
	st_conf.use_reserved_mem = TRUE;
    p_comm_pool = &g_movie_common_pool_tbl[0];
	for (i=0;i<NVTMPP_VB_MAX_COMM_POOLS;i++){
		if (p_comm_pool->blk_size == 0)
			break;
		st_conf.common_pool[i] = *p_comm_pool;
		p_comm_pool++;
	}

    ret = nvtmpp_vb_set_conf(&st_conf);
    if (NVTMPP_ER_OK != ret) {
        DBG_ERR("nvtmpp set vb err: %d\r\n", ret);
        return -1;
    }
    ret = nvtmpp_vb_init();
    if (NVTMPP_ER_OK != ret) {
        DBG_ERR("nvtmpp init vb err: %d\r\n", ret);
        return -1;
    }
    gMovie_InitCommonMemFinish=1;
    return 0;
}
#endif

INT32 MovieExe_ExitCommonMem(void)
{
	NVTMPP_ER        ret;
	ret = nvtmpp_vb_exit();
	if (NVTMPP_ER_OK != ret) {
		DBG_ERR("nvtmpp init vb err: %d\r\n", ret);
		return -1;
	}
	gMovie_InitCommonMemFinish=0;

	return 0;
}

//-------------------------------------------------------------------------------------------------
void MovieExe_RSC_SetSwitch(UINT32 index, UINT32 value)
{
#if (!defined(_Gyro_None_) && (MOVIE_RSC == ENABLE))

#if !defined(_IPL1_IPL_FAKE_)
	// RSC just support IPL 1
	if (gMovie_Rec_Info[0].vid_in == _CFG_VID_IN_1)
        RSC_SetSwitch(IPL_PATH_1, index, value);
#endif

#endif
}

INT32 MovieExe_OnMovieSize(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	UINT32 uiSelect = 0;
	//UINT32 uiRatioIdx = 0;

	if (paramNum > 0) {
		uiSelect = paramArray[0];
	}

	DBG_IND("uiSelect %d\r\n", uiSelect);

	UI_SetData(FL_MOVIE_SIZE, uiSelect);

	return NVTEVT_CONSUME;
}

static USIZE MovieExe_RatioSizeConvert(USIZE *devSize, USIZE *devRatio, USIZE *Imgratio)
{
	USIZE  resultSize = *devSize;

	if ((!devRatio->w) || (!devRatio->h)) {
		DBG_ERR("devRatio w=%d, h=%d\r\n", devRatio->w, devRatio->h);
	} else if ((!Imgratio->w) || (!Imgratio->h)) {
		DBG_ERR("Imgratio w=%d, h=%d\r\n", Imgratio->w, Imgratio->h);
	} else {
		if (((float)Imgratio->w / Imgratio->h) >= ((float)devRatio->w / devRatio->h)) {
			resultSize.w = devSize->w;
			resultSize.h = ALIGN_ROUND_4(devSize->h * Imgratio->h / Imgratio->w * devRatio->w / devRatio->h);
		} else {
			resultSize.h = devSize->h;
			resultSize.w = ALIGN_ROUND_16(devSize->w * Imgratio->w / Imgratio->h * devRatio->h / devRatio->w);
		}
	}
	return resultSize;
}
//#NT#2016/06/08#Lincy Lin -begin
//#NT#Implement generic OSD and video drawing mechanism for ALG function
static void MovieExe_CalcFDDispCord(URECT *dispCord)
{
	UINT32 ImageRatioIdx = 0;
	USIZE  ImageRatioSize = {0};
	URECT  fdDispCoord;
	ISIZE  dev1size;
	USIZE  dev1Ratio;
	USIZE  finalSize = {0};

	ImageRatioIdx = GetMovieSizeRatio(UI_GetData(FL_MOVIE_SIZE));
	ImageRatioSize = IMAGERATIO_SIZE[ImageRatioIdx];

	//1.get current device size (current mode)
	dev1size = GxVideo_GetDeviceSize(DOUT1);
	//2.get current device aspect Ratio
	dev1Ratio = GxVideo_GetDeviceAspect(DOUT1);
	finalSize = MovieExe_RatioSizeConvert((USIZE *)&dev1size, &dev1Ratio, &ImageRatioSize);
	fdDispCoord.w = finalSize.w;
	fdDispCoord.h = finalSize.h;
	if (finalSize.w == (UINT32)dev1size.w) {
		fdDispCoord.x = 0;
		fdDispCoord.y = (dev1size.h - finalSize.h) >> 1;
	} else {
		fdDispCoord.y = 0;
		fdDispCoord.x = (dev1size.w - finalSize.w) >> 1;

	}
	*dispCord = fdDispCoord;
}

static void MovieExe_CalcOSDDispCord(URECT *dispCord)
{
	UINT32 ImageRatioIdx = 0;
	USIZE  ImageRatioSize = {0};
	URECT  OsdDispCoord;
	ISIZE  dev1size;
	USIZE  dev1Ratio;
	USIZE  finalSize = {0};

	ImageRatioIdx = GetMovieSizeRatio(UI_GetData(FL_MOVIE_SIZE));
	ImageRatioSize = IMAGERATIO_SIZE[ImageRatioIdx];

	//1.get current device size (current mode)
	dev1size.w = OSD_W;
	dev1size.h = OSD_H;
	//2.get current device aspect Ratio
	//#NT#2016/06/16#Lincy Lin -begin
	//#NT#Fix osd display ratio mismatch video bug
	//dev1Ratio.w = OSD_W;
	//dev1Ratio.h = OSD_H;
	dev1Ratio = GxVideo_GetDeviceAspect(DOUT1);
	//#NT#2016/06/16#Lincy Lin -end
	DBGD(dev1Ratio.w);
	DBGD(dev1Ratio.h);
	DBGD(ImageRatioSize.w);
	DBGD(ImageRatioSize.h);
	finalSize = MovieExe_RatioSizeConvert((USIZE *)&dev1size, &dev1Ratio, &ImageRatioSize);
	OsdDispCoord.w = finalSize.w;
	OsdDispCoord.h = finalSize.h;

	if (OsdDispCoord.w == (UINT32)dev1size.w) {
		OsdDispCoord.x = 0;
		OsdDispCoord.y = (dev1size.h - finalSize.h) >> 1;
	} else {
		OsdDispCoord.y = 0;
		OsdDispCoord.x = (dev1size.w - finalSize.w) >> 1;

	}
	*dispCord = OsdDispCoord;
}
//#NT#2016/06/08#Lincy Lin -end

//-------------------------------------------------------------------------------------------------
INT32 MovieExe_OnImageRatio(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	UINT32  ImageRatioIdx = 0;
	USIZE   ImageRatioSize = {0};
	//#NT#2016/06/03#Brain Yen -begin
	//#NT#for special aspect panel and pipview
#if (PIP_VIEW_LR == ENABLE || defined(_LCDTYPE_TG078UW006A0_DSI_) ||  defined(_Disp_IFPA_LCD1_MD090TL01_))
	USIZE DeviceRatioSize = GxVideo_GetDeviceAspect(DOUT1);
#endif
	//#NT#2016/06/03#Brain Yen -end

	DBG_IND("\r\n");

	if (paramNum > 0) {
		ImageRatioIdx = paramArray[0];    // parameter 0: image size
	}
	DBG_MSG("ImageRatioIdx =%d\r\n", ImageRatioIdx);

	//check if imagestream is already open?
	if (ImageStream_IsOpen(&ISF_Stream[0]) != ISF_OK) {
		// ignore this change
		return NVTEVT_CONSUME;
	}

	if (ImageRatioIdx >= IMAGERATIO_MAX_CNT) {
		DBG_ERR("ImageRatioIdx =%d\r\n", ImageRatioIdx);
		return NVTEVT_CONSUME;
	}

	ImageRatioSize = IMAGERATIO_SIZE[ImageRatioIdx];
	DBG_MSG("ImageRatioSize=%d:%d\r\n", ImageRatioSize.w, ImageRatioSize.h);
	//#NT#2016/06/08#Lincy Lin -begin
	//#NT#Implement generic OSD and video drawing mechanism for ALG function
	MovieExe_CalcFDDispCord(&gMovieFdDispCord);
	MovieExe_CalcOSDDispCord(&gMovieOsdDispCord);
	//#NT#2016/06/08#Lincy Lin -end

	//1. Set Display image ratio
	if (System_GetEnableDisp() & DISPLAY_1) {
		ImageUnit_Begin(&ISF_VdoOut1, 0);
		ImageUnit_SetVdoImgSize(ISF_IN1, 0, 0); //buffer size = full device size
		//#NT#2016/06/03#Brain Yen -begin
		//#NT#for special aspect panel and pipview
#if (PIP_VIEW_LR == ENABLE || defined(_LCDTYPE_TG078UW006A0_DSI_) || defined(_Disp_IFPA_LCD1_MD090TL01_))
		ImageUnit_SetVdoAspectRatio(ISF_IN1, DeviceRatioSize.w, DeviceRatioSize.h);
#elif (SBS_VIEW_FUNC == ENABLE)
		ImageUnit_SetVdoAspectRatio(ISF_IN1, ImageRatioSize.w << 1, ImageRatioSize.h);
#else
		ImageUnit_SetVdoAspectRatio(ISF_IN1, ImageRatioSize.w, ImageRatioSize.h);
#endif
		//#NT#2016/06/03#Brain Yen -end
		ImageUnit_SetVdoPreWindow(ISF_IN1, 0, 0, 0, 0);  //window range = full device range
		ImageUnit_End();
	}
	if (System_GetEnableDisp() & DISPLAY_2) {
		ImageUnit_Begin(&ISF_VdoOut2, 0);
		ImageUnit_SetVdoImgSize(ISF_IN1, 0, 0); //buffer size = full device size
#if (SBS_VIEW_FUNC == ENABLE)
		ImageUnit_SetVdoAspectRatio(ISF_IN1, ImageRatioSize.w << 1, ImageRatioSize.h);
#else
		ImageUnit_SetVdoAspectRatio(ISF_IN1, ImageRatioSize.w, ImageRatioSize.h);
#endif
		ImageUnit_SetVdoPreWindow(ISF_IN1, 0, 0, 0, 0);  //window range = full device range
		ImageUnit_End();
	}
	//Set UserDraw Size
	{
		ISIZE devSize = GxVideo_GetDeviceSize(DOUT1);
		System_DispSourceByTarget(&devSize, &ImageRatioSize);
	}

	//2. Set Preview image ratio, and direction
	if (System_GetEnableSensor() & SENSOR_1) {
		ImageUnit_Begin(ISF_IPL(0), 0);
		ImageUnit_SetVdoAspectRatio(ISF_IN1, ImageRatioSize.w, ImageRatioSize.h);
		//#NT#2016/03/02#Niven Cho -begin
		//#NT#Use chip filp instead of sensor flip
		ImageUnit_SetVdoDirection(ISF_IN1, (UI_GetData(FL_MOVIE_SENSOR_ROTATE) == SEN_ROTATE_ON) ? FLIP_V : FLIP_NONE);
		//#NT#2016/03/02#Niven Cho -end
		ImageUnit_End();
	}
	if (System_GetEnableSensor() & SENSOR_2) {
		ImageUnit_Begin(ISF_IPL(1), 0);
		ImageUnit_SetVdoAspectRatio(ISF_IN1, ImageRatioSize.w, ImageRatioSize.h);
		ImageUnit_SetVdoDirection(ISF_IN1, 0);
		ImageUnit_End();
	}
	//ImageStream_UpdateSize();
	ImageStream_UpdateAll(&ISF_Stream[0]);

	return NVTEVT_CONSUME;
}

//-------------------------------------------------------------------------------------------------
INT32 MovieExe_OnMovieQuality(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
#if _TODO
	UINT32 uiQuality = 0;
	UINT32 uiSize;

	if (paramNum > 0) {
		uiQuality = paramArray[0];
	}

	UI_SetData(FL_MOVIE_QUALITY, uiQuality);
	uiSize = UI_GetData(FL_MOVIE_SIZE);

	MovRec_ChangeParameter(MOVREC_RECPARAM_TARGETRATE, GetMovieTargetBitrate(uiSize, uiQuality), 0, 0);
#endif

	return NVTEVT_CONSUME;
}

//-------------------------------------------------------------------------------------------------
INT32 MovieExe_OnWB(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	UINT32 uiSelect = 0;

	if (paramNum) {
		uiSelect = paramArray[0];
	}

	UI_SetData(FL_WB, uiSelect);
	MovieExe_AWB_SetUIInfo(AWB_UI_WB, Get_WBValue(uiSelect));

	return NVTEVT_CONSUME;
}

//-------------------------------------------------------------------------------------------------
INT32 MovieExe_OnColor(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	UINT32 uiSelect = 0;

	if (paramNum) {
		uiSelect = paramArray[0];
	}

	UI_SetData(FL_MOVIE_COLOR, uiSelect);
	MovieExe_IQ_SetUIInfo(IQ_UI_IMAGEEFFECT, Get_ColorValue(uiSelect));

	return NVTEVT_CONSUME;
}

//-------------------------------------------------------------------------------------------------
INT32 MovieExe_OnEV(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	UINT32 uiSelect = 0;

	if (paramNum) {
		uiSelect = paramArray[0];
	}

	UI_SetData(FL_EV, uiSelect);
	//photo and movie use the same EV value,should syncronize photo EV setting
	Photo_SetUserIndex(PHOTO_USR_EV, uiSelect);
	MovieExe_AE_SetUIInfo(AE_UI_EV, Get_EVValue(uiSelect));

	return NVTEVT_CONSUME;
}
//-------------------------------------------------------------------------------------------------

//#NT#2016/06/03#Charlie Chang -begin
//#NT# add contrast ,audio in, flip mirror, quality set
INT32 MovieExe_OnContrast(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{

	if (paramNum != 1) {
		DBG_ERR("paramNum =%d error!!!\r\n", paramNum);
		return NVTEVT_CONSUME;
	}
	UINT32 uiSelect = 0;
	uiSelect = paramArray[0];
	if (uiSelect >= MOVIE_CONTRAST_ID_MAX) {
		DBG_ERR("Contrast uiSelect =%d error\r\n", uiSelect);
		return NVTEVT_CONSUME;
	}
	MovieExe_IQ_SetUIInfo(IQ_UI_CONTRAST, uiSelect);

	DBG_DUMP("Contrast:%d\r\n", uiSelect);
	UI_SetData(FL_MOVIE_CONTRAST, uiSelect);

	return NVTEVT_CONSUME;
}
INT32 MovieExe_OnAudioIn(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{

	DBG_ERR("MovieExe_OnAudioin not implement need to do!!!!!!!!!!\r\n");

	return NVTEVT_CONSUME;
}
INT32 MovieExe_OnAudioInSR(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{

	DBG_ERR("MovieExe_OnAudioInSR not implement need to do!!!!!!!!!!\r\n");

	return NVTEVT_CONSUME;
}
INT32 MovieExe_OnFipMirror(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
#if _TODO
	if (paramNum < 1) {
		DBG_ERR("paramNum =%d error!!!\r\n", paramNum);
		return NVTEVT_CONSUME;
	}
	UINT32 uiSelect = 0;
	uiSelect = paramArray[0];
	UINT32 ipl_id = 0;

	if (uiSelect >= MOVIE_FLIP_MIRROR_ID_MAX) {
		DBG_ERR("flip value=%d error\r\n", uiSelect);
		return NVTEVT_CONSUME;
	}
	UI_SetData(FL_MOVIE_CONTRAST, uiSelect);
	ImageUnit_Begin(ISF_IPL(0), 0);
	if (uiSelect == MOVIE_FLIP_MIRROR_H_V) {
		DBG_DUMP("movie flip_H_V\r\n");
		ImageUnit_SetVdoDirection(ISF_IN1, ISF_VDO_DIR_MIRRORX|ISF_VDO_DIR_MIRRORY);
	} else if (uiSelect == MOVIE_FLIP_MIRROR_H) {
		DBG_DUMP("movie flip x\r\n");
		ImageUnit_SetVdoDirection(ISF_IN1, ISF_VDO_DIR_MIRRORX);
	} else if (uiSelect == MOVIE_FLIP_MIRROR_V) {
		DBG_DUMP("movie flip y\r\n");
		ImageUnit_SetVdoDirection(ISF_IN1, ISF_VDO_DIR_MIRRORY);
	} else {
		DBG_DUMP("movie flip_NONE\r\n");
		ImageUnit_SetVdoDirection(ISF_IN1, ISF_VDO_DIR_NONE);
	}
	ImageUnit_End();
#endif

	return NVTEVT_CONSUME;
}
INT32 MovieExe_OnQualitySet(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{

	if (paramNum < 1) {
		DBG_ERR("paramNum =%d error!!!\r\n", paramNum);
		return NVTEVT_CONSUME;
	}
	UINT32 uiSelect = 0;
	uiSelect = paramArray[0];

	switch (uiSelect) {

	case MOVIE_QUALITY_SET_MAX:
		UI_SetData(FL_MOVIE_SIZE, MOVIE_SIZE_FRONT_1920x1080P60);
		break;
	case MOVIE_QUALITY_SET_HIGH:
		UI_SetData(FL_MOVIE_SIZE, MOVIE_SIZE_FRONT_1280x720P30);
		break;
	case MOVIE_QUALITY_SET_MIDDLE:
		UI_SetData(FL_MOVIE_SIZE, MOVIE_SIZE_FRONT_848x480P30);
		break;
	case MOVIE_QUALITY_SET_LOW:
		UI_SetData(FL_MOVIE_SIZE, MOVIE_SIZE_FRONT_640x480P30);
		break;
	case MOVIE_QUALITY_SET_MIN:
		UI_SetData(FL_MOVIE_SIZE, MOVIE_SIZE_FRONT_320x240P30);
		break;
	default:
		DBG_ERR("uiSelect =%d error!! \r\n", uiSelect);
		return NVTEVT_CONSUME;
	}
	//reopen movie mode
	Ux_SendEvent(0, NVTEVT_SYSTEM_MODE, 1, PRIMARY_MODE_MOVIE);
	return NVTEVT_CONSUME;
}
//#NT#2016/06/03#Charlie Chang -end
//-------------------------------------------------------------------------------------------------
INT32 MovieExe_OnMovieAudio(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	UINT32 uiSelect = 0;

	if (paramNum) {
		uiSelect = paramArray[0];
	}

	UI_SetData(FL_MOVIE_AUDIO, uiSelect);

	if (uiSelect == MOVIE_AUDIO_OFF) {
#if defined(_ASR_ENABLE_)
		ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1, MOVIEMULTI_PARAM_AUD_MUTE_ENC, TRUE);
#elif (AUDPROC_FUNC == ENABLE)
    ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1, MOVIEMULTI_PARAM_AUD_MUTE_ENC, TRUE);
#else
		ImageUnit_Begin(&ISF_AudIn, 0);
		ImageUnit_SetParam(ISF_CTRL, AUDIN_PARAM_VOL_IMM, RECMOVIE_AUD_OFF); //set mute!
		ImageUnit_End();
#endif
		DBG_DUMP("===Movie Rec Set Vol = %d\r\n", RECMOVIE_AUD_OFF);
	} else {
		UINT32 current_vol = UI_GetData(FL_MovieAudioRecIndex);
		ImageUnit_Begin(&ISF_AudIn, 0);
		ImageUnit_SetParam(ISF_CTRL, AUDIN_PARAM_VOL_IMM, current_vol); //restore to current vol
		ImageUnit_End();
#if defined(_ASR_ENABLE_)
		ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1, MOVIEMULTI_PARAM_AUD_MUTE_ENC, FALSE);
#elif (AUDPROC_FUNC == ENABLE)
    ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1, MOVIEMULTI_PARAM_AUD_MUTE_ENC, FALSE);
#endif
		DBG_DUMP("===Movie Rec Set Vol = %d\r\n", current_vol);
	}

	return NVTEVT_CONSUME;
}

INT32 MovieExe_OnMovieAudioRecVolume(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	UINT32 uhSelect = 0;
	UINT32 current_vol = UI_GetData(FL_MovieAudioRecIndex);
	if (paramNum > 0) {
		uhSelect = paramArray[0];
	}
	DBG_IND("+-idx=%d\r\n", uhSelect);
	if (current_vol != uhSelect) {
		UI_SetData(FL_MovieAudioRecIndex, uhSelect);
		ImageUnit_Begin(&ISF_AudIn, 0);
		ImageUnit_SetParam(ISF_CTRL, AUDIN_PARAM_VOL_IMM, uhSelect); //set current vol
		ImageUnit_End();
		DBG_DUMP("===Movie Rec Set Vol = %d\r\n", uhSelect);
	}
	return NVTEVT_CONSUME;
}

INT32 MovieExe_OnMovieAudioRec(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	UINT32 uhSelect = 0;
	UINT32 current_audio_stream = UI_GetData(FL_MovieAudioRec);
	if (paramNum > 0) {
		uhSelect = paramArray[0];
	}
	DBG_IND("+-idx=%d\r\n", uhSelect);
	if (current_audio_stream != uhSelect) {
		UI_SetData(FL_MovieAudioRec, uhSelect);
	}
	return NVTEVT_CONSUME;
}

INT32 MovieExe_OnDigitalZoom(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	UINT32 uhSelect = 0;
	if (paramNum > 0) {
		uhSelect = paramArray[0];
	}
	DBG_IND("+-MovieExe_OnDigitalZoom:idx=%d\r\n", uhSelect);
	UI_SetData(FL_Dzoom, uhSelect);
	if (uhSelect == DZOOM_OFF) {
		UI_SetData(FL_DzoomIndex, DZOOM_10X);
	}
	return NVTEVT_CONSUME;
}

INT32 MovieExe_OnDis(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	UINT32 uiSelect = 0;

	if (paramNum) {
		uiSelect = paramArray[0];
	}

	DBG_IND("idx=%d\r\n", uiSelect);

	UI_SetData(FL_MovieDisIndex, uiSelect);

    if (uiSelect == MOVIE_DIS_ON) {
        // enable DIS
		MovieExe_IPL_SetIInfo(IPL_SEL_DISCOMP_IMM, SEL_DISCOMP_ON);
    } else {
        // enable DIS
		MovieExe_IPL_SetIInfo(IPL_SEL_DISCOMP_IMM, SEL_DISCOMP_OFF);
    }

#if 0
	//if (UI_GetData(FL_ModeIndex) == DSC_MODE_MOVIE)
	{

#if (MOVIE_DIS == ENABLE)
		if (uiSelect == MOVIE_DIS_ON) {
			// disable RSC
			UI_SetData(FL_MovieRSCIndex, MOVIE_RSC_OFF);
#if (MOVIE_RSC == ENABLE)
			//rsc tmp
			MovieExe_RSC_SetSwitch(SEL_RSC_RUNTIME, SEL_RSC_OFF);
#endif
			// enable DIS
			MovieExe_IPL_SetIInfo(IPL_SEL_DISCOMP_IMM, SEL_DISCOMP_ON);
		} else {
			MovieExe_IPL_SetIInfo(IPL_SEL_DISCOMP_IMM, SEL_DISCOMP_OFF);
		}
#endif
	}
#endif

	return NVTEVT_CONSUME;
}

INT32 MovieExe_OnMCTF(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	UINT32 uiSelect = 0;

	if (paramNum) {
		uiSelect = paramArray[0];
	}

	UI_SetData(FL_MovieMCTFIndex, uiSelect);

	return NVTEVT_CONSUME;
}

INT32 MovieExe_OnEdge(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	UINT32 uiSelect = 0;

	if (paramNum) {
		uiSelect = paramArray[0];
	}
	UI_SetData(FL_EDGE, uiSelect);

	if (uiSelect == MOVIE_EDGE_ON) {
		MovieExe_IQ_SetUIInfo(IQ_UI_IPPEDGE, IQ_EDGE_ON);
	} else {
		MovieExe_IQ_SetUIInfo(IQ_UI_IPPEDGE, IQ_EDGE_OFF);
	}
	return NVTEVT_CONSUME;
}

INT32 MovieExe_OnNR(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	UINT32 uiSelect = 0;

	if (paramNum) {
		uiSelect = paramArray[0];
	}

	UI_SetData(FL_NR, uiSelect);

	if (uiSelect == MOVIE_NR_ON) {
		MovieExe_IQ_SetUIInfo(IQ_UI_IPPNR, IQ_NR_ON);
	} else {
		MovieExe_IQ_SetUIInfo(IQ_UI_IPPNR, IQ_NR_OFF);
	}
	return NVTEVT_CONSUME;
}

INT32 MovieExe_OnRSC(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{

#if (MOVIE_RSC == ENABLE)

	UINT32 uiSelect = 0;
	UINT32 uiFrameRate;

	if (paramNum) {
		uiSelect = paramArray[0];
	}

	UI_SetData(FL_MovieRSCIndex, uiSelect);

#if (MOVIE_MULTI_RECORD_FUNC)
	uiFrameRate = GetMovieFrameRate_2p(0, UI_GetData(FL_MOVIE_SIZE));
#else
	uiFrameRate = GetMovieFrameRate(UI_GetData(FL_MOVIE_SIZE));
#endif

#if (!defined(_Gyro_None_) && (MOVIE_RSC == ENABLE))
	if (uiSelect == MOVIE_RSC_ON) {
		// disable DIS
		UI_SetData(FL_MovieDisIndex, MOVIE_DIS_OFF);
#if (MOVIE_DIS == ENABLE)
		MovieExe_IPL_SetIInfo(IPL_SEL_DISCOMP_IMM, SEL_DISCOMP_OFF);
#endif
		// enable RSC if necessary (only support frame rate smaller than 60fps)
		//rsc tmp
		if ((UI_GetData(FL_MovieRSCIndex) == MOVIE_RSC_ON) &&
			(gMovie_Rec_Info[0].frame_rate <= 120)) {
			MovieExe_RSC_SetSwitch(SEL_RSC_RUNTIME, SEL_RSC_ON);

			if (UI_GetData(FL_MOVIE_HDR) == MOVIE_HDR_ON) {
				Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_SHDR, 1, SHDR_OFF);
			}
		} else {
			MovieExe_RSC_SetSwitch(SEL_RSC_RUNTIME, SEL_RSC_OFF);
		}
	} else {
		MovieExe_RSC_SetSwitch(SEL_RSC_RUNTIME, SEL_RSC_OFF);
	}
#endif
#endif
	return NVTEVT_CONSUME;
}

INT32 MovieExe_OnWDR(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	UINT32 uiSelect = 0;

	if (paramNum) {
		uiSelect = paramArray[0];
	}

	if (uiSelect == MOVIE_WDR_ON) {
		UI_SetData(FL_MOVIE_WDR, MOVIE_WDR_ON);
		MovieExe_IPL_SetIInfo(IPL_SEL_WDR_IMM, SEL_WDR_AUTO);

		if (UI_GetData(FL_MOVIE_HDR) == MOVIE_HDR_ON) {
			Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_SHDR, 1, MOVIE_HDR_OFF);
		}
	} else {
		UI_SetData(FL_MOVIE_WDR, MOVIE_WDR_OFF);
		MovieExe_IPL_SetIInfo(IPL_SEL_WDR_IMM, SEL_WDR_OFF);
	}

	return NVTEVT_CONSUME;
}

INT32 MovieExe_OnSHDR(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UINT32 uhSelect = 0;

    DBG_IND("%d \r\n", paramArray[0]);

	if (paramNum > 0) {
		uhSelect = paramArray[0];
	}

    UI_SetData(FL_MOVIE_HDR, uhSelect);

	if (UI_GetData(FL_MOVIE_HDR) == MOVIE_HDR_ON) {
		MovieExe_IPL_SetIInfo(IPL_SEL_SHDR, SEL_SHDR_ON);
	} else {
		MovieExe_IPL_SetIInfo(IPL_SEL_SHDR, SEL_SHDR_OFF);
	}


#if 0
#if SHDR_FUNC
	UINT32 uhSelect = 0;
	UINT32 bChangeMode = FALSE;

	DBG_IND("%d \r\n", paramArray[0]);
	if (paramNum > 0) {
		uhSelect = paramArray[0];
	}

	if (paramNum > 1) {
		bChangeMode = paramArray[1];
	}

	UI_SetData(FL_MOVIE_HDR, uhSelect);

	if (UI_GetData(FL_MOVIE_HDR) == MOVIE_HDR_ON) {
		MovieExe_IPL_SetIInfo(IPL_SEL_SHDR, SEL_SHDR_ON);
	} else {
		MovieExe_IPL_SetIInfo(IPL_SEL_SHDR, SEL_SHDR_OFF);
	}

	if (UI_GetData(FL_MOVIE_HDR) == MOVIE_HDR_ON) { // && UI_GetData(FL_MOVIE_WDR) == MOVIE_WDR_ON)
		UI_SetData(FL_MOVIE_WDR, MOVIE_WDR_OFF);
		UI_SetData(FL_MovieRSCIndex, MOVIE_RSC_OFF);
	}

	// need to off to preview again
	if (bChangeMode) {
		Ux_PostEvent(NVTEVT_SYSTEM_MODE, 1, System_GetState(SYS_STATE_CURRMODE));
	} else {
		DBG_WRN("should change mode\r\n");
	}
#endif
#endif

	return NVTEVT_CONSUME;
}

INT32 MovieExe_OnDefog(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	UINT32 uiSelect = 0;
	DBG_IND("%d \r\n", paramArray[0]);
	if (paramNum > 0) {
		uiSelect = paramArray[0];
	}

	if (uiSelect == MOVIE_DEFOG_ON) {
		UI_SetData(FL_MOVIE_DEFOG, MOVIE_DEFOG_ON);
		MovieExe_IPL_SetIInfo(IPL_SEL_DEFOG_IMM, SEL_DEFOG_ON);

	} else {
		UI_SetData(FL_MOVIE_DEFOG, MOVIE_DEFOG_OFF);
		MovieExe_IPL_SetIInfo(IPL_SEL_DEFOG_IMM, SEL_DEFOG_OFF);
	}

	return NVTEVT_CONSUME;
}

INT32 MovieExe_OnSensorRotate(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	UINT32 uiSelect = 0;

	if (paramNum) {
		uiSelect = paramArray[0];
	}
	//#NT#2016/03/02#Niven Cho -begin
	//#NT#Use chip filp instead of sensor flip
	//#NT#2018/02/14#KCHong -begin
	//#NT#support sensor rotate setting
	if (uiSelect == SEN_ROTATE_ON) {
		UI_SetData(FL_MOVIE_SENSOR_ROTATE, SEN_ROTATE_ON);
		gMovie_Rec_Info[0].sensor_rotate = TRUE;
	} else {
		UI_SetData(FL_MOVIE_SENSOR_ROTATE, SEN_ROTATE_OFF);
		gMovie_Rec_Info[0].sensor_rotate = FALSE;
	}
	//#NT#2018/02/14#KCHong -end
	if (System_GetEnableSensor() & SENSOR_1) {
		ImageUnit_Begin(ISF_IPL(0), 0);
		ImageUnit_SetVdoDirection(ISF_IN1, (UI_GetData(FL_MOVIE_SENSOR_ROTATE) == SEN_ROTATE_ON) ? ISF_VDO_DIR_MIRRORXY : ISF_VDO_DIR_NONE);
		ImageUnit_End();
	}
	//#NT#2016/03/02#Niven Cho -end
	if (System_GetEnableSensor() & SENSOR_2) {
		#if 0 // Use sensor rotate function.
		ImageUnit_Begin(ISF_IPL(1), 0);
		ImageUnit_SetVdoDirection(ISF_IN1, 0);
		ImageUnit_End();
		#else // Use IPL mirror/flip function instead for CCIR sensor.
		ImageApp_MovieMulti_SetParam(_CFG_REC_ID_2, MOVIEMULTI_PARAM_IPL_MIRROR, (UI_GetData(FL_MOVIE_SENSOR_ROTATE) == SEN_ROTATE_ON) ? ISF_VDO_DIR_MIRRORY : ISF_VDO_DIR_NONE);
		#endif
	}
	//ImageStream_UpdateSize();
	ImageStream_UpdateAll(&ISF_Stream[0]);
	return NVTEVT_CONSUME;
}

INT32 MovieExe_OnIRCut(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
//#NT#2016/02/22#Niven Cho -begin
//#NT#add IR_CUT_FUNC to enable IR CUT function.
#if (IR_CUT_FUNC == ENABLE)
	UINT32 uiSelect = 0;

	if (paramNum) {
		uiSelect = paramArray[0];
	}

	DBG_IND("%d \r\n", uiSelect);
	if (uiSelect == MOVIE_IR_CUT_ON) {
		UI_SetData(FL_MOVIE_IR_CUT, MOVIE_IR_CUT_ON);
		MovieExe_IQ_SetUIInfo(IQ_UI_IRCUT, IQ_IRCUT_DAY);
		UIMovie_IRCutCtrl(ON);
	}
#if defined(_UI_STYLE_IPCAM_)
	else if (uiSelect == MOVIE_IR_CUT_AUTO) {
		UI_SetData(FL_MOVIE_IR_CUT, MOVIE_IR_CUT_AUTO);
		MovieExe_IQ_SetUIInfo(IQ_UI_IRCUT,  IQ_IRCUT_AUTO);
	}
#endif
	else {
		UI_SetData(FL_MOVIE_IR_CUT, MOVIE_IR_CUT_OFF);
		MovieExe_IQ_SetUIInfo(IQ_UI_IRCUT,  IQ_IRCUT_NIGHT);
		UIMovie_IRCutCtrl(OFF);
	}
#endif
//#NT#2016/02/22#Niven Cho -end
	return NVTEVT_CONSUME;
}

INT32 MovieExe_OnGdc(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
#if(MOVIE_GDC_FUNC ==ENABLE)
	UINT32 uhSelect = 0;
	if (paramNum > 0) {
		uhSelect = paramArray[0];
	}

	DBG_IND("%d \r\n", uhSelect);
	if (uhSelect == TRUE) {
#if (!defined(_Gyro_None_) && (MOVIE_RSC == ENABLE))
		if (UI_GetData(FL_MovieRSCIndex) == MOVIE_RSC_ON) {
			MovieExe_RSC_SetSwitch(SEL_RSC_RUNTIME, SEL_RSC_OFF);
			DBG_WRN("GDC is enable, so RSC is forced off\r\n");
		}
#endif
		MovieExe_IQ_SetUIInfo(IQ_UI_GDCCOMP, IQ_GDCCOMP_ON);
	} else {
		MovieExe_IQ_SetUIInfo(IQ_UI_GDCCOMP, IQ_GDCCOMP_OFF);
	}
#endif
	return NVTEVT_CONSUME;
}

INT32 MovieExe_OnSmear(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	UINT32 uhSelect = 0;
	if (paramNum > 0) {
		uhSelect = paramArray[0];
	}
	DBG_IND("+-idx=%d\r\n", uhSelect);

	UI_SetData(FL_MovieSmearIndex, uhSelect);
#if(MOVIE_SMEAR_R_FUNC ==ENABLE)
	AppPhoto_SetData(&CustomPhotoObjCtrl, _SmearModeEn,
					 (uhSelect == MOVIE_SMEAR_ON) ? _SmearMode_on : _SmearMode_off);
#endif
	return NVTEVT_CONSUME;
}

//-------------------------------------------------------------------------------------------------
INT32 MovieExe_OnDateImprint(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	UINT32 uiSelect = 0;

	if (paramNum) {
		uiSelect = paramArray[0];
	}

	UI_SetData(FL_MOVIE_DATEIMPRINT, uiSelect);

	return NVTEVT_CONSUME;
}
//-------------------------------------------------------------------------------------------------
INT32 MovieExe_OnGSENSOR(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	UINT32 uiSelect = 0;

	if (paramNum) {
		uiSelect = paramArray[0];
	}

	UI_SetData(FL_GSENSOR, uiSelect);

#if !defined(_GSensor_None_)
	switch (UI_GetData(FL_GSENSOR)) {
	case GSENSOR_OFF:
		GSensor_SetSensitivity(GSENSOR_SENSITIVITY_OFF);
		break;
	case GSENSOR_LOW:
		GSensor_SetSensitivity(GSENSOR_SENSITIVITY_LOW);
		break;
	case GSENSOR_MED:
		GSensor_SetSensitivity(GSENSOR_SENSITIVITY_MED);
		break;
	case GSENSOR_HIGH:
		GSensor_SetSensitivity(GSENSOR_SENSITIVITY_HIGH);
		break;
	default:
		GSensor_SetSensitivity(GSENSOR_SENSITIVITY_OFF);
		break;
	}
#endif

	//debug_err(("MovieExe_OnGSENSOR =%d\r\n ",FL_GSENSOR));
	return NVTEVT_CONSUME;
}


//#MT#lyb -20200403 -begin
extern void GSensor_SetMovieParkSensitivity(UINT32 MovieParkSensitivity);
INT32 MovieExe_OnMoviePark(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
#if (GSENSOR_FUNCTION == ENABLE)
    UINT32 uiSelect = 0;
    if (paramNum) {
        uiSelect = paramArray[0];
    }
    UI_SetData(FL_MOVIE_PARK, uiSelect);  
	if ( UI_GetData(FL_MOVIE_PARK) == MOVIE_PARK_OFF)
	{
	    GSensor_SetMovieParkSensitivity(0);
	}else{
		GSensor_SetMovieParkSensitivity(uiSelect);
	}
#endif
    return NVTEVT_CONSUME;
}
//#MT#lyb -20200403 -end
extern UIMenuStoreInfo  currentInfo;
BOOL  Restart_Rec = FALSE;
INT32 MovieExe_OnSetLCDBRIGHTLVL(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
//CHKPNT;
 	UINT32 uiSelect = 0;
    if (paramNum > 0) {
        uiSelect = paramArray[0];
    }
	DBG_IND("uhSelect %d \r\n", uiSelect);
	//	debug_msg("uhSelect %d \r\n", uhSelect);
	UI_SetData(FL_LCD_BRIGHTNESS,uiSelect);
	currentInfo.strBLLvl = uiSelect;
	GxVideo_SetDeviceCtrl(DOUT1, DISPLAY_DEVCTRL_BRIGHTLVL,uiSelect);
	Save_MenuInfo();
}
extern BOOL g_EnterParkingMode_CloseBL;
INT32 MovieExe_OnSetMovieBackAdjust(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	UINT32 uiSelect = 0;
    if (paramNum > 0) {
        uiSelect = paramArray[0];
    }
	DBG_IND("uhSelect %d \r\n", uiSelect);
	if(UI_GetData(FL_MOVIE_BACKADJUST_MENU) != UI_GetData(FL_MOVIE_BACKADJUST)&&g_EnterParkingMode_CloseBL == FALSE){
			
		UI_SetData(FL_MOVIE_BACKADJUST, uiSelect);  
	}
		
}

//-------------------------------------------------------------------------------------------------
INT32 MovieExe_OnCyclicRec(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	UINT32 uiSelect = 0;
	UINT32 i, mask;
	UINT32 movie_rec_mask, clone_rec_mask;

	uiSelect = paramNum ? paramArray[0] : MOVIE_CYCLICREC_3MIN;
	UINT32 uhSelect = 0;
    if (paramNum > 0) {
   //     uhSelect = paramArray[0];
    }
#if  0//(WIFI_FINALCAM_APP_STYLE == ENABLE)
		{
			BOOL bReOpen = FALSE;
			if(UI_GetData(FL_MOVIE_CYCLIC_REC_MENU) != UI_GetData(FL_MOVIE_CYCLIC_REC))
			{
			        UI_SetData(FL_MOVIE_CYCLIC_REC, UI_GetData(FL_MOVIE_CYCLIC_REC_MENU));
			        bReOpen = TRUE;

			}
			UI_SetData(FL_MOVIE_CYCLIC_REC, uiSelect);
			
			FlowWiFiMovie_Stop2Idle();  // Stop IPL setting first.

			Delay_DelayMs(100);  // Could be fine tuned.

		//	bReOpen = FlowMovie_CheckReOpenItem();

			if (bReOpen == TRUE) {
				Ux_SendEvent(0, NVTEVT_SYSTEM_MODE, 1, System_GetState(SYS_STATE_CURRMODE));
			} else {
				FlowWiFiMovie_StopRec();
			}
		}
#endif
//debug_msg(" MovieExe_OnCyclicRec %d %d \r\n",UI_GetData(FL_MOVIE_CYCLIC_REC_MENU), UI_GetData(FL_MOVIE_CYCLIC_REC));
	if  (UI_GetData(FL_MOVIE_CYCLIC_REC_MENU) != UI_GetData(FL_MOVIE_CYCLIC_REC) 
		 && System_GetState(SYS_STATE_CURRSUBMODE) != SYS_SUBMODE_WIFI ) {
			Restart_Rec= TRUE;
			CHKPNT;
	}
	UI_SetData(FL_MOVIE_CYCLIC_REC, uiSelect);
    //UI_SetData(FL_MOVIE_CYCLIC_REC_MENU, uiSelect);
	UINT32 ItemIndex = 0;
//debug_msg("--cj uiSelect:%d \r\n",uiSelect );
	ItemIndex = uiSelect ;
	switch (ItemIndex ) { // uiSelect
	case MOVIE_CYCLICREC_1MIN:
		gMovie_Rec_Option.seamless_sec = 60;
		break;

	case MOVIE_CYCLICREC_3MIN:
		gMovie_Rec_Option.seamless_sec = 180;
		break;

	case MOVIE_CYCLICREC_5MIN:
		gMovie_Rec_Option.seamless_sec = 300;
		break;

	case MOVIE_CYCLICREC_10MIN:
		gMovie_Rec_Option.seamless_sec = 600;
		break;

	case MOVIE_CYCLICREC_OFF:
		gMovie_Rec_Option.seamless_sec = 900;
		break;

	default:
		gMovie_Rec_Option.seamless_sec = 300;
		break;
	}

	movie_rec_mask = Movie_GetMovieRecMask();
	clone_rec_mask = Movie_GetCloneRecMask();
	gMovie_Rec_Option.end_type= (uiSelect == MOVIE_CYCLICREC_OFF)? MOVREC_ENDTYPE_CUT_TILLCARDFULL : MOVREC_ENDTYPE_CUTOVERLAP;

	mask = 1;
	for (i = 0; i < SENSOR_CAPS_COUNT; i++) {
		if (movie_rec_mask & mask) {
			ImageApp_MovieMulti_SetParam(gMovie_Rec_Info[i].rec_id, MOVIEMULTI_PARAM_FILE_SEAMLESSSEC, gMovie_Rec_Option.seamless_sec);
			ImageApp_MovieMulti_SetParam(gMovie_Rec_Info[i].rec_id, MOVIEMULTI_PARAM_FILE_ENDTYPE, gMovie_Rec_Option.end_type);
		}

		if (clone_rec_mask & mask) {
			ImageApp_MovieMulti_SetParam(gMovie_Clone_Info[i].rec_id, MOVIEMULTI_PARAM_FILE_SEAMLESSSEC, gMovie_Rec_Option.seamless_sec);
			ImageApp_MovieMulti_SetParam(gMovie_Clone_Info[i].rec_id, MOVIEMULTI_PARAM_FILE_ENDTYPE,gMovie_Rec_Option.end_type);
		}

		mask <<= 1;
	}

#if(defined(_NVT_ETHREARCAM_RX_))
	for (i = 0; i < ETH_REARCAM_CAPS_COUNT; i++) {
		ImageApp_MovieMulti_SetParam((_CFG_ETHCAM_ID_1 + i), MOVIEMULTI_PARAM_FILE_SEAMLESSSEC, gMovie_Rec_Option.seamless_sec);
		ImageApp_MovieMulti_SetParam((_CFG_ETHCAM_ID_1 + i), MOVIEMULTI_PARAM_FILE_ENDTYPE, gMovie_Rec_Option.end_type);
	}
#endif




	return NVTEVT_CONSUME;
}


//-------------------------------------------------------------------------------------------------
INT32 MovieExe_OnMotionDet(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	UINT32 uiSelect = 0;

	if (paramNum) {
		uiSelect = paramArray[0];
	}

	UI_SetData(FL_MOVIE_MOTION_DET, uiSelect);

	return NVTEVT_CONSUME;
}

#if _QR_FUNC_
#include "ImageUnit_UserProc.h"
#include "ImageApp_MovieCommon.h"
#include "ImageApp_MovieMulti.h"

#include <cyg/zbar/zbar.h>
typedef void (*QRDATA_CB)(UINT32 p1, UINT32 p2,UINT32 p3);

static UINT32 gQRLock=TRUE;
static UINT32 gQRProcessRate = 5;
static QRDATA_CB gQRDataCB=NULL;

void QR_Lock(BOOL bLock)
{
    gQRLock = bLock;
    DBG_IND("gQRLock %d\r\n",gQRLock);
}
UINT32 QR_Processs(UINT32 uiOutAddrY, UINT32 w, UINT32 h)
{
    zbar_image_scanner_t *scanner = NULL;
    zbar_image_t *img;
    zbar_image_t *test;
    UINT32 bFind = 0;

    scanner = zbar_image_scanner_create();

    /* configure the reader */
    zbar_image_scanner_set_config(scanner, 0, ZBAR_CFG_ENABLE, 1);

    img = zbar_image_create();
    zbar_image_set_size(img, w, h);
    zbar_image_set_format(img, MAKEFOURCC('Y','8','0','0'));
    zbar_image_set_data(img, (const void *)uiOutAddrY, w*h, NULL);

    test = zbar_image_convert(img, MAKEFOURCC('Y','8','0','0'));

    if(!test)
        return(2);
    #if 0
    DBG_IND("converted: %d x %d (%lx) %08lx 0x%x\r\n",
           zbar_image_get_width(test),
           zbar_image_get_height(test),
           zbar_image_get_data_length(test),
           zbar_image_get_format(test),
           zbar_image_get_data(test));
    #endif
    /* scan the image for barcodes */
    zbar_scan_image(scanner, test);

    /* extract results */
    const zbar_symbol_t *symbol = zbar_image_first_symbol(test);
    for(; symbol; symbol = zbar_symbol_next(symbol)) {

        /* do something useful with results */
        zbar_symbol_type_t typ = zbar_symbol_get_type(symbol);
        const char *data = zbar_symbol_get_data(symbol);
        /*
        DBG_IND("decoded %s symbol \"%s\"\n",
               zbar_get_symbol_name(typ), data);
        */
        if(gQRDataCB)
        {
            bFind = 1;
            gQRDataCB((UINT32)zbar_get_symbol_name(typ),(UINT32)data,0);
        }
    }
    /* clean up */
    zbar_image_destroy(test);
    zbar_image_destroy(img);
    zbar_image_scanner_destroy(scanner);

    if(bFind)
        return TRUE;
    else
        return FALSE;
}
void QR_DataCb(UINT32 parm1,UINT32 parm2,UINT32 parm3)
{
    if(parm1){
        char *QRData = (char *)parm2;
		#if 0
        char *pSeparator=0;
        char pSSID[WSC_MAX_SSID_LEN];
        char pPwd[MAX_WEP_KEY_LEN];
		#endif

        DBG_DUMP("decoded %s symbol \"%s\"\r\n", parm1, QRData);

        //SSID & Password sample for WiFi
        #if 0
        pSeparator=strchr(QRData,'%');
        if(pSeparator)
        {
            DBGS(pSeparator);
            memset(pSSID,0,WSC_MAX_SSID_LEN);
            memset(pPwd,0,MAX_WEP_KEY_LEN);

            snprintf(pPwd,MAX_WEP_KEY_LEN-1,"%s",pSeparator+1);
            QRData[pSeparator-QRData] =0;
            snprintf(pSSID,WSC_MAX_SSID_LEN-1,"%s",QRData);
        }
        else
        {
            DBG_ERR("data no pSeparator ex: SSID\%PWD\r\n");
        }
        #endif
   }
}

void Photo_QRInit(QRDATA_CB CB, UINT32 ProcessRate)
{
    gQRProcessRate = ProcessRate;
    gQRDataCB = CB;
    gQRLock = FALSE;
}

void Photo_QRProcess(MEM_RANGE *buf, MEM_RANGE *cachebuf)
{
    if(!gQRLock)
    {
        //IPL_IME_BUF_ADDR CurInfo;
        //UINT32           srcW,srcH;
        //UINT32           PxlAddr[3];
        //UINT32           LineOff[3];
        static UINT32    gQRcount = 0;

        #if 0
        CurInfo.Id = IPL_ID_1;

        IPL_GetCmd(IPL_GET_IME_CUR_BUF_ADDR, (void *)&CurInfo);

        srcW = CurInfo.ImeP2.Ch[0].Width;
        srcH = CurInfo.ImeP2.Ch[0].Height;
        LineOff[0] = CurInfo.ImeP2.Ch[0].LineOfs;
        PxlAddr[0] = CurInfo.ImeP2.PixelAddr[0];
        #else
		ISF_DATA isf_data;
		ISF_UNIT *pUnit;
		ISF_PORT *pSrc;
		IMG_BUF  *pBuf = NULL;

		pUnit = &ISF_UserProc;
		pSrc = ImageUnit_Out(pUnit, ImageApp_MovieMulti_GetAlgDataPort(_CFG_REC_ID_1, _CFG_ALG_PATH3));

        if (ImageUnit_IsAllowPull(pSrc) && ImageUnit_PullData(pSrc, (ISF_DATA *)&isf_data, 0) == ISF_OK) {

			pBuf = (IMG_BUF *)&isf_data.Desc[0];

			DBG_IND("^M pBuf: w %d, h %d, y_addr 0x%08x, uv_addr 0x%08x, y_loff %d, uv_loff %d, fmt %d\r\n",
			     pBuf->Width, pBuf->Height, pBuf->PxlAddr[0], pBuf->PxlAddr[1], pBuf->LineOffset[0],
			     pBuf->LineOffset[1], pBuf->PxlFmt);

			#if 0 // save RAW frame data.
			FST_FILE pFile = FileSys_OpenFile("A:\\Y_data.raw", FST_CREATE_ALWAYS | FST_OPEN_WRITE);
			UINT32 size;
			if (pFile) {
			    size = pBuf->Width * pBuf->Height;
			    FileSys_WriteFile(pFile, (UINT8 *)pBuf->PxlAddr[0], &size, 0, NULL);
			    FileSys_CloseFile(pFile);
			}
			#endif

			gQRcount++;
			if(gQRcount >= gQRProcessRate) {
			   gQRcount = 0;
			   if(QR_Processs(pBuf->PxlAddr[0], pBuf->Width, pBuf->Height)==TRUE)
			   {
			       gQRLock = TRUE;
			   }
			}

			ImageUnit_ReleaseData(&isf_data);
        }
        #endif
    }
}
PHOTO_FUNC_INFO PhotoFuncInfo_qr =
{
#if 1
	{
		PHOTO_PRIORITY_M,       ///< function hook process Id.
		IPL_PATH_1,             ///< IPL Id.
		PHOTO_ISR_SIE_VD,       ///< isr tigger event
		"dummy",
		NULL,                   ///< init fp, only execute at first process
		Photo_QRProcess,        ///< process fp
		NULL,                   ///< process end fp
		NULL,                   ///< get working buffer fp
		NULL,                   ///< get working cache buffer fp
	},
#else
    {
    PHOTO_ID_2,             ///< function hook process Id.
    PHOTO_ISR_IME_FRM_END,  ///< isr tigger event
    NULL,                   ///< init fp, only execute at first process
    Photo_QRProcess,        ///< process fp
    NULL,                   ///< process end fp
    NULL,                   ///< get working buffer fp
    NULL,                   ///< get working cache buffer fp
    },
#endif
    NULL  ///< point next Function Obj, last Function must be set NULL
};
#endif

static void MovieExe_InitPhotoFunc(void)
{
	PHOTO_FUNC_INFO *phead = NULL;
	//
	// Setup IPL information.
	//
	phead = &PhotoFuncInfo_dummy;
	phead->pNext = NULL; //mark end

    #if _QR_FUNC_
    {
        PHOTO_FUNC_INFO* pfunc = &PhotoFuncInfo_qr;
        PHOTO_FUNC_INFO* pcurr = 0;
        pcurr = phead; while(pcurr->pNext) pcurr = pcurr->pNext; //find last func
        pcurr->pNext = pfunc; //append this func
        pfunc->pNext = NULL; //mark end
        #if 0
        BufferSize.w = DISPLAY_VDO_W;
        BufferSize.h = DISPLAY_VDO_H;
        DeviceSize.w = DISPLAY_VDO_W;
        DeviceSize.h = DISPLAY_VDO_H;
		#endif

        Photo_QRInit(QR_DataCb,10);

    }
    #endif

	alg_Movie_InitPhotoFunc(phead);

	UIPhotoFunc_Open(phead);
}

void MovieExe_ResetFileSN(void)
{
//CHKPNT;
	g_FileSerialNum = 0;
}

UINT32 MovieExe_GetFileSN(void)
{
	//CHKPNT;

	return g_FileSerialNum;
}

BOOL MovieExe_CheckSNFull(void)
{
	return (g_FileSerialNum >= FILE_SN_MAX);
}

static void MovieExe_FileNamingCB(MOVIE_CFG_REC_ID id, char *pFileName)
{
	//debug_msg("--cj 00 g_FileSerialNum:%d ", g_FileSerialNum);
    static struct tm   CurDateTime = {0};

	g_FileSerialNum++;

	if (g_FileSerialNum > FILE_SN_MAX) {
		g_FileSerialNum = FILE_SN_MAX;
		Ux_PostEvent(NVTEVT_CB_MOVIE_FULL, 0);
	}

    CurDateTime = HwClock_GetTime(TIME_ID_CURRENT);
	
#if(SENSOR_CAPS_COUNT>=2 || ((defined(_NVT_ETHREARCAM_RX_)) && (ETH_REARCAM_CAPS_COUNT == 2)))
    char    NH_endChar='A';
    if(id < _CFG_REC_ID_MAX){
        NH_endChar+=id;
    }else if(id <_CFG_CLONE_ID_MAX){
        NH_endChar+=(id-_CFG_CLONE_ID_1);
    }else if(id <_CFG_ETHCAM_ID_MAX){
        NH_endChar+=(id-_CFG_ETHCAM_ID_1);
    }

    snprintf(pFileName, NMC_TOTALFILEPATH_MAX_LEN, "%04d%02d%02d%02d%02d%02d_%06d%c",
            CurDateTime.tm_year, CurDateTime.tm_mon, CurDateTime.tm_mday,
            CurDateTime.tm_hour, CurDateTime.tm_min, CurDateTime.tm_sec, g_FileSerialNum, NH_endChar);
#else
//CHKPNT;
if (id == 0) { // main movie
		snprintf(pFileName, NMC_TOTALFILEPATH_MAX_LEN, "%04d_%02d%02d_%02d%02d%02d_%06dF",
				 CurDateTime.tm_year, CurDateTime.tm_mon, CurDateTime.tm_mday,
				 CurDateTime.tm_hour, CurDateTime.tm_min, CurDateTime.tm_sec,g_FileSerialNum);
	} else { // clone movie
		snprintf(pFileName, NMC_TOTALFILEPATH_MAX_LEN, "%04d_%02d%02d_%02d%02d%02d_%06dB",
				 CurDateTime.tm_year, CurDateTime.tm_mon, CurDateTime.tm_mday,
				 CurDateTime.tm_hour, CurDateTime.tm_min, CurDateTime.tm_sec,g_FileSerialNum);
	}
	
/*	
//debug_msg("-----cj uiPathId:%d \r\n",id);
    snprintf(pFileName, NMC_TOTALFILEPATH_MAX_LEN, "%04d%02d%02d%02d%02d%02d_%06d",
            CurDateTime.tm_year, CurDateTime.tm_mon, CurDateTime.tm_mday,
            CurDateTime.tm_hour, CurDateTime.tm_min, CurDateTime.tm_sec, g_FileSerialNum);
	//*/
#endif	
}

static void MovieExe_PhotoFileNamingCB(MOVIE_CFG_REC_ID id, char *pFileName)
{
    static struct tm   CurDateTime = {0};
//	debug_msg("--cj 11 g_FileSerialNum:%d ", g_FileSerialNum);

	g_FileSerialNum++;

	if (g_FileSerialNum > FILE_SN_MAX) {
		g_FileSerialNum = FILE_SN_MAX;
		Ux_PostEvent(NVTEVT_CB_MOVIE_FULL, 0);
	}

    CurDateTime = HwClock_GetTime(TIME_ID_CURRENT);
	debug_msg("-----cj uiPathId:%d \r\n",id);

#if(SENSOR_CAPS_COUNT>=2 || ((defined(_NVT_ETHREARCAM_RX_)) && (ETH_REARCAM_CAPS_COUNT == 2)))
    char    NH_endChar='A';
    if(id < _CFG_REC_ID_MAX){
        NH_endChar+=id;
    }else if(id <_CFG_CLONE_ID_MAX){
        NH_endChar+=(id-_CFG_CLONE_ID_1);
    }else if(id <_CFG_ETHCAM_ID_MAX){
        NH_endChar+=(id-_CFG_ETHCAM_ID_1);
    }
    snprintf(pFileName, NMC_TOTALFILEPATH_MAX_LEN, "%04d%02d%02d%02d%02d%02d_%06d%c",
            CurDateTime.tm_year, CurDateTime.tm_mon, CurDateTime.tm_mday,
            CurDateTime.tm_hour, CurDateTime.tm_min, CurDateTime.tm_sec, g_FileSerialNum, NH_endChar);
#else
if (id == 0) { // main movie
		snprintf(pFileName, NMC_TOTALFILEPATH_MAX_LEN, "%04d_%02d%02d_%02d%02d%02d_%06dF",
				 CurDateTime.tm_year, CurDateTime.tm_mon, CurDateTime.tm_mday,
				 CurDateTime.tm_hour, CurDateTime.tm_min, CurDateTime.tm_sec,g_FileSerialNum);
	} else { // clone movie
		snprintf(pFileName, NMC_TOTALFILEPATH_MAX_LEN, "%04d_%02d%02d_%02d%02d%02d_%06dB",
				 CurDateTime.tm_year, CurDateTime.tm_mon, CurDateTime.tm_mday,
				 CurDateTime.tm_hour, CurDateTime.tm_min, CurDateTime.tm_sec,g_FileSerialNum);
	}
/*
    snprintf(pFileName, NMC_TOTALFILEPATH_MAX_LEN, "%04d%02d%02d%02d%02d%02d_%06d",
            CurDateTime.tm_year, CurDateTime.tm_mon, CurDateTime.tm_mday,
            CurDateTime.tm_hour, CurDateTime.tm_min, CurDateTime.tm_sec, g_FileSerialNum);
	//*/
#endif
}


#define MOVIE_TIMELAPSE_TIME           60000
#define MOVIE_TIMELAPSE_PLAYFRAMERATE     30

#if(defined(_NVT_ETHREARCAM_RX_))
static void MovieExe_EthCam_PIP_WifiCB_PutVdoFrm(NMI_VDODEC_RAW_INFO *info)
{
	UINT32             Width;
	UINT32             Height;
	#if (MOVIE_UVAC_FUNC == DISABLE)
	if (System_GetState(SYS_STATE_CURRSUBMODE) != SYS_SUBMODE_WIFI) {
		return;
	}
	#endif

	wai_sem(ETHCAM_WIFICB_VDOFRM_SEM_ID);

	Width = ImageUnit_GetParam(&ISF_VdoDec, ISF_OUT1, VDODEC_PARAM_WIDTH);
	Height = ImageUnit_GetParam(&ISF_VdoDec, ISF_OUT1, VDODEC_PARAM_HEIGHT);

	UINT32  LineOff[MAX_PLANE_NUM];
	UINT32  PxlAddr[MAX_PLANE_NUM];

	PxlAddr[0] = info->uiYAddr;
	PxlAddr[1] = info->uiUVAddr;
	LineOff[0] = ALIGN_CEIL_64(Width);
	LineOff[1] = ALIGN_CEIL_64(Width);

	if(info->PathID>=ETH_REARCAM_CAPS_COUNT){
		DBG_ERR("PathID over flow =%d\r\n", info->PathID);
		sig_sem(ETHCAM_WIFICB_VDOFRM_SEM_ID);
		return;
	}

	GxImg_InitBufEx(&g_EthcamWifiCBImg[info->PathID], Width, Height, GX_IMAGE_PIXEL_FMT_YUV420, LineOff, PxlAddr);
	//DBG_DUMP("P[%d] W=%d, %d, PxlAddr[0]=0x%x\r\n", info->PathID,g_EthcamWifiCBImg[info->PathID].Width,g_EthcamWifiCBImg[info->PathID].Height,g_EthcamWifiCBImg[info->PathID].PxlAddr[0]);
	sig_sem(ETHCAM_WIFICB_VDOFRM_SEM_ID);
}

static IMG_BUF *MovieExe_EthCam_PIP_WifiCB_GetVdoFrm(UINT32 PathID)
{
	if(PathID>=ETH_REARCAM_CAPS_COUNT){
		DBG_ERR("PathID over flow =%d\r\n", PathID);
		return NULL;
	}
	return (IMG_BUF *)&g_EthcamWifiCBImg[PathID];
}
#endif
#define MOVIE_ROOT_PATH         "A:\\Novatek\\"
#define MOVIE_ROOT_PATH2        "B:\\Novatek\\"

//static void MovieExe_UserEventCb(MOVIE_CFG_REC_ID id, MOVIE_USER_CB_EVENT event_id, UINT32 value)
static void MovieExe_UserEventCb(UINT32 id, MOVIE_USER_CB_EVENT event_id, UINT32 value)
{
	switch ((UINT32)event_id) {
	case MOVIE_USER_CB_EVENT_STAMP_CB: {
        		if (value) {
        	            MP_VDOENC_YUV_SRC* p_yuv_src   = NULL;
        	            p_yuv_src = (MP_VDOENC_YUV_SRC*)value;
#if (VIDEOSPRITE_STAMP_FUNC == ENABLE)
        	            NMI_IMGCAP_YUV * pImgCapYUV = (NMI_IMGCAP_YUV *)value;

        	            if ((pImgCapYUV->thumb== 2) && (UI_GetData(FL_MOVIE_PIM) == MOVIE_PIM_ON) && (UI_GetData(FL_MOVIE_DATEIMPRINT) == MOVIE_DATEIMPRINT_ON))//2:PIM; 1:Thumb
        	            {
        	                videosprite_draw_ext(pImgCapYUV->path_id, pImgCapYUV->y_addr, pImgCapYUV->u_addr, pImgCapYUV->v_addr, pImgCapYUV->y_loff,  pImgCapYUV->img_w, pImgCapYUV->img_h);
        	            }
#if (THUMB_STAMP_FUNC == ENABLE)
        	            else if(pImgCapYUV->thumb== 1 && (UI_GetData(FL_MOVIE_DATEIMPRINT) == MOVIE_DATEIMPRINT_ON))//2:PIM; 1:Thumb
        	            {
            	                //set coe thumb data
        	                MovieStamp_UpdateData_Thumb(pImgCapYUV->path_id, FALSE);
        	                videosprite_draw_ext(pImgCapYUV->path_id, pImgCapYUV->y_addr, pImgCapYUV->u_addr, pImgCapYUV->v_addr, pImgCapYUV->y_loff,  pImgCapYUV->img_w, pImgCapYUV->img_h);
        	                //restore main stream coe data
        	                MovieStamp_RestoreData(pImgCapYUV->path_id);
        	            }
#endif
        	            else// if(UI_GetData(FL_MOVIE_DATEIMPRINT) == MOVIE_DATEIMPRINT_ON)
#endif
        	            {
        	                if (System_GetState(SYS_STATE_CURRSUBMODE) == SYS_SUBMODE_WIFI && gMovie_Strm_Info.codec == _CFG_CODEC_MJPG) {
						        if (ImageApp_MovieMulti_GetVdoEncPort(_CFG_STRM_ID_1) == p_yuv_src->uiVidPathID) {
							        videosprite_draw_ext(p_yuv_src->uiVidPathID,
							        p_yuv_src->uiYAddr,
							        p_yuv_src->uiCbAddr,
							        p_yuv_src->uiCrAddr,
							        p_yuv_src->uiYLineOffset,
							        p_yuv_src->uiWidth,
							        p_yuv_src->uiHeight);
						        }else{
							        videosprite_draw_btn_ext(p_yuv_src->uiVidPathID,
							        p_yuv_src->uiYAddr,
							        p_yuv_src->uiCbAddr,
							        p_yuv_src->uiCrAddr,
							        p_yuv_src->uiYLineOffset,
							        p_yuv_src->uiWidth,
							        p_yuv_src->uiHeight);
        	                    }
        	                }else{
						        videosprite_draw_btn_ext(p_yuv_src->uiVidPathID,
						        p_yuv_src->uiYAddr,
						        p_yuv_src->uiCbAddr,
						        p_yuv_src->uiCrAddr,
						        p_yuv_src->uiYLineOffset,
						        p_yuv_src->uiWidth,
						        p_yuv_src->uiHeight);
        	                }
         	            }
       	
        	            if (UI_GetData(FL_MOVIE_DATEIMPRINT) == MOVIE_DATEIMPRINT_ON) {
#if (THUMB_STAMP_FUNC == ENABLE)
        		        NMI_IMGCAP_YUV * pCapYUV = (NMI_IMGCAP_YUV *)value;

        	            if (pCapYUV->thumb==1) {
        	                MovieStamp_CopyData_Thumb(pCapYUV->path_id,
        	                pCapYUV->y_addr,
        	                pCapYUV->u_addr,
        	                pCapYUV->v_addr,
        	                pCapYUV->y_loff,
        	                pCapYUV->img_w);
        	            }
        	            else
#endif
        	            {
        	                MovieStamp_CopyData(p_yuv_src->uiVidPathID,
        	                p_yuv_src->uiYAddr,
        	                p_yuv_src->uiCbAddr,
        	                p_yuv_src->uiCrAddr,
        	                p_yuv_src->uiYLineOffset,
        	                p_yuv_src->uiWidth);
        	            }
        	        }
        	    }
        	}
		break;
	case MOVIE_USER_CB_EVENT_REC_ONE_SECOND: {
			#if (LOG_DBGINFO_IN_GPS_SECTION == ENABLE)
			if (value == 0) {
				mp4log_ResetFrmCnt();
			}
			#endif  // #if (LOG_DBGINFO_IN_GPS_SECTION == ENABLE)
			//debug_msg("value:%d \r\n",value);
			Ux_PostEvent(NVTEVT_CB_MOVIE_REC_ONE_SEC, 1,  value);
		}
		break;

	case MOVIE_USER_CB_EVENT_GET_GPS_DATA: {
			#if (LOG_DBGINFO_IN_GPS_SECTION == ENABLE)
			#if (GPS_FUNCTION == ENABLE)
			RMCINFO RMCInfo;
			GPSRec_GetRMCDate(&RMCInfo);
			mp4log_AddLog(0, (char *)&RMCInfo, sizeof(RMCINFO));
			#endif  // #if (GPS_FUNCTION == ENABLE)
			MEM_RANGE log_buf, *ptr = (MEM_RANGE *)value;
			if (mp4log_ToggleBuf(&(log_buf.Addr), NULL) == E_OK) {
				ptr->Addr = log_buf.Addr;
				ptr->Size = mp4log_GetTotalLogSize() * 1024 - 16;
			}
			#endif  // #if (LOG_DBGINFO_IN_GPS_SECTION == ENABLE)
		}
		break;

    case MOVIE_USER_CB_EVENT_FILENAMING_MOV_CB:
	case MOVIE_USER_CB_EVENT_FILENAMING_EMR_CB: {
            // if MOVIE_CFG_FILE_NAMING is TRUE, will callback for naming file
            CHAR *pFileName = (CHAR *) value;
			CHKPNT;
            if (event_id == MOVIE_USER_CB_EVENT_FILENAMING_EMR_CB) {
                #if defined(_EMBMEM_EMMC_) && (FS_MULTI_STRG_FUNC==ENABLE)
                UINT32 uiDxState = 0;
                DX_HANDLE pStrgDev = Dx_GetObject(DX_CLASS_STORAGE_EXT | FS_DX_TYPE_DRIVE_A);
                if (Dx_GetState((DX_HANDLE)pStrgDev, STORAGE_STATE_INSERT, &uiDxState) != DX_OK || uiDxState == FALSE) {
                    ImageApp_MovieMulti_Root_Path(MOVIE_ROOT_PATH, gMovie_Folder_Naming[0].rec_id); // Trigger EMR to eMMC if no card.
                } else {
                    ImageApp_MovieMulti_Root_Path(MOVIE_ROOT_PATH2, gMovie_Folder_Naming[0].rec_id);
                }
                #endif
            } else {
                ImageApp_MovieMulti_Root_Path(MOVIE_ROOT_PATH, gMovie_Folder_Naming[0].rec_id);
            }

            // Make custome file name
            MovieExe_FileNamingCB(id, pFileName);
        }
        break;
	case MOVIE_USER_CB_EVENT_FILENAMING_SNAPSHOT_CB:{
            // if MOVIE_CFG_FILE_NAMING is TRUE, will callback for naming file
            CHAR *pFileName = (CHAR *) value;

            // Make custome file name
            MovieExe_PhotoFileNamingCB(id, pFileName);
        }
        break;

	case MOVIE_USER_CB_ERROR_CARD_SLOW: {
		DBG_ERR("Slow card!,id=%d\r\n", value);
		exam_msg("slow card\r\n"); // AUTO_TEST
		Ux_PostEvent(NVTEVT_CB_MOVIE_SLOW, 0);
		}
		break;

	case MOVIE_USER_CB_ERROR_CARD_WR_ERR: {
		// Card write error
		DBG_ERR("EVENT %d: card write error!\r\n", event_id);
		Ux_PostEvent(NVTEVT_CB_MOVIE_WR_ERROR, 0);
		}
		break;

	case MOVIE_USER_CB_EVENT_SNAPSHOT_OK: {
		if(value){
                     NMI_IMGCAP_JPG *jpg  = (NMI_IMGCAP_JPG *)value;
                     if (jpg->thumb !=TRUE)
                     {
                            DBG_IND("EVENT %d: snapshop ok!\r\n", event_id);
                            Ux_PostEvent(NVTEVT_CB_RAWENC_OK, 0);
#if(defined(_NVT_ETHREARCAM_TX_))
                            if(ImageApp_MovieMulti_IsStreamRunning(_CFG_REC_ID_1 | ETHCAM_TX_MAGIC_KEY)){
                            	g_bAllowRawEnc=TRUE;
                            }
#endif
                     }else{
                            DBG_IND("pathid %d: thumb ok!\r\n", jpg->path_id);
                     }
		}
		}
		break;

	case MOVIE_USER_CB_ERROR_SNAPSHOT_ERR: {
		if(value){
                     NMI_IMGCAP_JPG *jpg  = (NMI_IMGCAP_JPG *)value;
                     if (jpg->thumb !=TRUE)
                     {
                		DBG_ERR("EVENT %d: snapshop err!\r\n", event_id);
                		g_bAllowRawEnc=TRUE;
                		Ux_PostEvent(NVTEVT_CB_RAWENC_ERR, 0);
                     }else{
                            DBG_IND("pathid %d: thumb err!\r\n", jpg->path_id);
                     }
		}
		}
		break;

	case MOVIE_USER_CB_EVENT_CUTFILE: {
		// Cyclic record cut file callback
        DBG_IND("EVENT 0x%08x: Cut file end!\r\n", event_id);
		}
		break;

	case MOVIE_USER_CB_ERROR_SEAMLESS_REC_FULL: {
			Ux_PostEvent(NVTEVT_CB_MOVIE_FULL, 0);
		}
		break;
	
	case MOVIE_USER_CB_EVENT_OVERTIME: {
		DBG_IND("EVENT 0x%08x: Overtime!\r\n", event_id);
		Ux_PostEvent(NVTEVT_CB_MOVIE_OVERTIME, 0);
		}
		break;
	
	case MOVIE_USER_CB_EVENT_EMR_FILE_COMPLETED:{
		Ux_PostEvent(NVTEVT_CB_EMR_COMPLETED, 2, id, event_id);
		}
		break;
	
	case MOVIE_USER_CB_EVENT_CARSH_FILE_COMPLETED:{
        debug_msg("=====MOVIE_USER_CB_EVENT_CARSH_FILE_COMPLETED=====\r\n");
		Ux_PostEvent(NVTEVT_CB_EMR_COMPLETED, 2, id, event_id);
		}
		break;
	
	case MOVIE_USER_CB_EVENT_CLOSE_FILE_COMPLETED:{
		DBG_IND(" =====MOVIE_USER_CB_EVENT_CLOSE_FILE_COMPLETED=====  \r\n");
		CHAR *p_ext_name = (CHAR *) value;
		if(strcmp(p_ext_name, "JPG") == 0 || strcmp(p_ext_name, "jpg") == 0){
			DBG_IND("Rawenc Completed\r\n");
			g_bAllowRawEnc=TRUE;
		}
		}
		break;
	
	case MOVIE_USER_CB_EVENT_CLOSE_RESULT: {
		if (value == MOVREC_EVENT_RESULT_NORMAL) {
			// normal stop
			DBG_IND("EVENT 0x%08x: REC_FILE_COMPLETED!\r\n", event_id);
		   	#if (SENSOR_INSERT_MASK != 0 || defined(_NVT_ETHREARCAM_RX_))
				UINT32 i;
				for (i=0; i<SENSOR_CAPS_COUNT; i++){
					if (ImageApp_MovieMulti_IsStreamRunning(gMovie_Rec_Info[i].rec_id)){
						return;
				}
			}
			#endif

			#if(defined(_NVT_ETHREARCAM_RX_))
				UINT32 j;
				for (j=0; j<ETH_REARCAM_CAPS_COUNT; j++){
					if (ImageApp_MovieMulti_IsStreamRunning(_CFG_ETHCAM_ID_1+j)){
						return;
					}
				}
			#endif
			Ux_PostEvent(NVTEVT_CB_MOVIE_REC_FINISH, 1, value);
		} else if (value == MOVREC_EVENT_RESULT_WRITE_ERR) {
			// write error
			DBG_ERR("EVENT %d: HW error!\r\n", event_id);
			Ux_PostEvent(NVTEVT_CB_MOVIE_WR_ERROR, 0);
		} else if (value == MOVREC_EVENT_LOOPREC_FULL) {
			// loop record full
			DBG_ERR("EVENT 0x%08x: looprec full!\r\n", event_id);
		} else if (value == MOVREC_EVENT_RESULT_SLOW) {
			// slow
			// do nothing due to restart flow in controlled by MOVIE_USER_CB_ERROR_CARD_SLOW callback messge.
			DBG_ERR("EVENT 0x%08x: Slow card!\r\n", event_id);
		} else if (value == MOVREC_EVENT_RESULT_FULL) {
			DBG_IND("EVENT 0x%08x: REC_FILE_COMPLETED!\r\n", event_id);
			// card full
			DBG_ERR("EVENT %d: Full!\r\n", event_id);
			Ux_PostEvent(NVTEVT_CB_MOVIE_FULL, 0);
		}
		}
		break;
	
	case MOVIE_USER_CB_EVENT_RAWENC_PREPARED:{
#if (USE_EXIF == ENABLE)
			EXIF_HANDLE_ID HandleID;

			HandleID = value;
			ExifVendor_Write0thIFD(HandleID);
			ExifVendor_WriteExifIFD(HandleID);
			ExifVendor_Write0thIntIFD(HandleID);
			ExifVendor_WriteGPSIFD(HandleID);
#endif
		}
		break;
	
#if(defined(_NVT_ETHREARCAM_RX_))
	case MOVIE_USRR_CB_EVENT_TRIGGER_ETHCAM_THUMB:{
		UINT32 path_id=value;
		UINT32 i;
		DBG_DUMP("TRIGGER_ETHCAM_THUMB path_id=%d\r\n", path_id);
		//EthCam_SendXMLCmd(ETHCAM_PATH_ID_1, ETHCAM_PORT_DATA1 ,ETHCAM_CMD_GET_TX_MOVIE_THUMB, 0);
		for (i=0; i<ETH_REARCAM_CAPS_COUNT; i++){
			if(EthCamNet_GetEthLinkStatus(i)==ETHCAM_LINK_UP && ImageApp_MovieMulti_BsPort2EthCamlink(path_id)==(_CFG_ETHCAM_ID_1+i)){
				if(i==0){
					AppBKW_SetData(BKW_ETHCAM_TRIGGER_THUMB_PATHID_P0, path_id);
				}else{
					AppBKW_SetData(BKW_ETHCAM_TRIGGER_THUMB_PATHID_P1, path_id);
				}
			}
		}
		BKG_PostEvent(NVTEVT_BKW_TRIGGER_THUMB);
		}
		break;
#endif

	case MOVIE_USER_CB_EVENT_FILE_DELETED:{
		CHAR *p_path = (CHAR *) value;
		if (NULL != p_path) {
			//CHKPNT;
			DBG_IND("^R %s deleted\r\n", p_path);
						debug_msg("^R %s deleted\r\n", p_path);

		}
		}
		break;
	
	case MOVIE_USER_CB_ERROR_DEC_ERROR: {
	#if(defined(_NVT_ETHREARCAM_RX_))
			if(g_isEthCamDispDecErr[0] ==0  && g_isEthCamDispDecErr[1] ==0){
				NMI_VDODEC_RAW_INFO *info = (NMI_VDODEC_RAW_INFO *) value;
				if(info && info->uiRawAddr==MAKEFOURCC('D', 'C', 'E', 'R') && info->uiRawSize==0
					&&( info->PathID>=ETHCAM_PATH_ID_1 && info->PathID< ETHCAM_PATH_ID_MAX)){
			    		DBG_ERR("Dec err!,skip disp[%d]\r\n",info->PathID);
					g_isEthCamDispDecErr[info->PathID]=TRUE;
					break;
				}
			}
		UINT32 path_id=value;
		// VdoDec Dec Error
	    		DBG_ERR("Dec err!,path_id=%d\r\n", path_id);
			if(ImageApp_MovieMulti_IsStreamRunning(_CFG_ETHCAM_ID_1)
				#if (ETH_REARCAM_CAPS_COUNT >= 2)
				|| ImageApp_MovieMulti_IsStreamRunning(_CFG_ETHCAM_ID_2)
				#endif
			){
				Ux_PostEvent(NVTEVT_CB_MOVIE_SLOW, 0);
			}
			if(path_id>=ETHCAM_PATH_ID_1 && path_id< ETHCAM_PATH_ID_MAX){
				g_isEthCamDispDecErr[path_id]=FALSE;
				AppBKW_SetData(BKW_ETHCAM_DEC_ERR_PATHID, path_id);
				BKG_PostEvent(NVTEVT_BKW_ETHCAM_DEC_ERR);
			}
	#endif		
		}
		break;

#if(defined(_NVT_ETHREARCAM_RX_))
	case MOVIE_USER_CB_EVENT_DEC_ONEFRAME: {
		NMI_VDODEC_RAW_INFO *info = (NMI_VDODEC_RAW_INFO *) value;
		//DBG_DUMP("a=%x,s=%d, %x, %x\r\n", info->uiRawAddr, info->uiRawSize, info->uiYAddr, info->uiUVAddr);
		MovieExe_EthCam_PIP_WifiCB_PutVdoFrm(info);
		}
		break;
#endif

	}
}
void MovieExe_TriggerPIMResultManual(UINT32 value)
{
	NMI_IMGCAP_JPG jpg={0};
	jpg.thumb=FALSE;
	MovieExe_UserEventCb(0, MOVIE_USER_CB_EVENT_SNAPSHOT_OK, (UINT32)&jpg);
}
#if IMG_FULL_DISP
 UINT32 g_PipViewStyle = 0xffff;

UINT32 PipView_SetDispImgOnLcdStartY(UINT32 step, UINT32 imgH,UINT32 dispH)
{
     UINT32 uiStartY;
     if(step >= SEN_DISP_OFFSET_SETTING_MAX)
        step = (SEN_DISP_OFFSET_SETTING_MAX - 1);
     uiStartY = ALIGN_CEIL_4(((imgH - dispH)/(SEN_DISP_OFFSET_SETTING_MAX - 1)) * step);
     return uiStartY;
}

extern INT32 CurStepPosBehind;
void MovieExe_SetIMECrop(UINT32 rec_id)
{
	UINT32 i;
	ISIZE  disp_size = GxVideo_GetDeviceSize(DOUT1);
	USIZE  disp_aspect_ratio = GxVideo_GetDeviceAspect(DOUT1);
	MOVIEMULTI_DISP_IME_CROP_INFO DispIMEInfo;

	i = rec_id;
	gMovie_Rec_Info[i].disp_ratio.w = disp_aspect_ratio.w;
	gMovie_Rec_Info[i].disp_ratio.h = disp_aspect_ratio.h;
	debug_msg("--cj %d %d  %d %d  %d\r\n",UI_GetData(FL_DUAL_CAM), gMovie_Rec_Info[i].ratio.w, gMovie_Rec_Info[i].disp_ratio.h,
		gMovie_Rec_Info[i].disp_ratio.w, gMovie_Rec_Info[i].ratio.h );
	
	if ((gMovie_Rec_Info[i].ratio.w * gMovie_Rec_Info[i].disp_ratio.h) > (gMovie_Rec_Info[i].disp_ratio.w * gMovie_Rec_Info[i].ratio.h)) {
		DispIMEInfo.DispIMESize.h = disp_size.h;
		DispIMEInfo.DispIMESize.w = ALIGN_CEIL_4((DispIMEInfo.DispIMESize.h * gMovie_Rec_Info[i].ratio.w) / gMovie_Rec_Info[i].ratio.h);
		DispIMEInfo.DispIMEWin.w = disp_size.w;
		DispIMEInfo.DispIMEWin.h = disp_size.h;
		DispIMEInfo.DispIMEWin.x = ALIGN_CEIL_4((DispIMEInfo.DispIMESize.w - DispIMEInfo.DispIMEWin.w) / 2);
		DispIMEInfo.DispIMEWin.y = 0;
		//CHKPNT;
	} else {
		if (UI_GetData(FL_DUAL_CAM) == DUALCAM_LR_FULL) {
			gMovie_Rec_Info[i].disp_ratio.w = disp_aspect_ratio.w;
			gMovie_Rec_Info[i].disp_ratio.h = disp_aspect_ratio.h * 2;

			if ((gMovie_Rec_Info[i].ratio.w * gMovie_Rec_Info[i].disp_ratio.h) > (gMovie_Rec_Info[i].disp_ratio.w * gMovie_Rec_Info[i].ratio.h)) {
				DispIMEInfo.DispIMESize.h = disp_size.h;
				DispIMEInfo.DispIMESize.w = ALIGN_CEIL_4((DispIMEInfo.DispIMESize.h * gMovie_Rec_Info[i].ratio.w) / gMovie_Rec_Info[i].ratio.h);
				DispIMEInfo.DispIMEWin.w = disp_size.w / 2;
				DispIMEInfo.DispIMEWin.h = disp_size.h;
				DispIMEInfo.DispIMEWin.x = ALIGN_CEIL_4((DispIMEInfo.DispIMESize.w - DispIMEInfo.DispIMEWin.w) / 2);
				DispIMEInfo.DispIMEWin.y = 0;
			} else {
				DispIMEInfo.DispIMESize.w = disp_size.w / 2;
				DispIMEInfo.DispIMESize.h = ALIGN_CEIL_4((DispIMEInfo.DispIMESize.w * gMovie_Rec_Info[i].ratio.h) / gMovie_Rec_Info[i].ratio.w);
				DispIMEInfo.DispIMEWin.w = disp_size.w / 2;
				DispIMEInfo.DispIMEWin.h = disp_size.h;
				DispIMEInfo.DispIMEWin.x = 0;
				DispIMEInfo.DispIMEWin.y = ALIGN_CEIL_4((DispIMEInfo.DispIMESize.h - DispIMEInfo.DispIMEWin.h) / 2);
			}
		} else {
			DispIMEInfo.DispIMESize.w = disp_size.w;
			DispIMEInfo.DispIMESize.h = ALIGN_CEIL_4((DispIMEInfo.DispIMESize.w * gMovie_Rec_Info[i].ratio.h) / gMovie_Rec_Info[i].ratio.w);
			DispIMEInfo.DispIMEWin.w = disp_size.w;
			DispIMEInfo.DispIMEWin.h = disp_size.h;
			DispIMEInfo.DispIMEWin.x = 0;
		#if(!defined(_NVT_ETHREARCAM_RX_))
			switch(i)
			{
			   case _CFG_REC_ID_1:
                   DispIMEInfo.DispIMEWin.y = PipView_SetDispImgOnLcdStartY(SysGetFlag(FL_SENSOR1_DISP_OFFSET), DispIMEInfo.DispIMESize.h, DispIMEInfo.DispIMEWin.h);
                   debug_msg("^B TX Liwk------sensor 1 DispIMEInfo.DispIMEWin.y:%d\r\n",DispIMEInfo.DispIMEWin.y);
				   break;
			   case _CFG_REC_ID_2:
			   	   DispIMEInfo.DispIMEWin.y = PipView_SetDispImgOnLcdStartY(SysGetFlag(FL_SENSOR2_DISP_OFFSET), DispIMEInfo.DispIMESize.h, DispIMEInfo.DispIMEWin.h);
				   debug_msg("^B TX Liwk------sensor 2 DispIMEInfo.DispIMEWin.y:%d\r\n",DispIMEInfo.DispIMEWin.y);
				   break;
			   default:
                   DispIMEInfo.DispIMEWin.y = ALIGN_CEIL_4((DispIMEInfo.DispIMESize.h - DispIMEInfo.DispIMEWin.h) / 2);
                   break;
			}
		#else
			if(i == _CFG_REC_ID_1){  
			//CHKPNT;
			  //DispIMEInfo.DispIMEWin.y = PipView_SetDispImgOnLcdStartY(SysGetFlag(FL_SENSOR1_DISP_OFFSET), DispIMEInfo.DispIMESize.h, DispIMEInfo.DispIMEWin.h);
				DispIMEInfo.DispIMEWin.y =  SysGetFlag(FL_SENSOR1_DISP_OFFSET)*CurStepPosBehind; 
			  debug_msg("^B RX Liwk------sensor 1 DispIMEInfo.DispIMEWin.y:%d %d\ %d %d %d\ %d\r\n",DispIMEInfo.DispIMEWin.y, DispIMEInfo.DispIMESize.h, DispIMEInfo.DispIMEWin.h,DispIMEInfo.DispIMESize.w,gMovie_Rec_Info[i].ratio.h,gMovie_Rec_Info[i].ratio.w);
			}else{
				CHKPNT;
			  DispIMEInfo.DispIMEWin.y = ALIGN_CEIL_4((DispIMEInfo.DispIMESize.h - DispIMEInfo.DispIMEWin.h) / 2);
			  debug_msg("^B RX Liwk------sensor 2 DispIMEInfo.DispIMEWin.y:%d\r\n",DispIMEInfo.DispIMEWin.y);
			}
			#endif

		}
	}

	//DBG_DUMP("^C DispIMEInfo[%d]: size w %d, h %d, Win x %d, y %d, w %d, h %d\r\n",
	//	i, DispIMEInfo.DispIMESize.w, DispIMEInfo.DispIMESize.h, DispIMEInfo.DispIMEWin.x, DispIMEInfo.DispIMEWin.y, DispIMEInfo.DispIMEWin.w, DispIMEInfo.DispIMEWin.h);

	ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1 + i, MOVIEMULTI_PARAM_DISP_IME_CROP, (UINT32)&DispIMEInfo);
}
void MovieExe_SetIMECropCustom(UINT32 rec_id, ISIZE  *in_size, USIZE *in_aspect_ratio)
{
	UINT32 i;
	ISIZE  disp_size = *in_size;
	USIZE  disp_aspect_ratio = *in_aspect_ratio;
	MOVIEMULTI_DISP_IME_CROP_INFO DispIMEInfo;

	i = rec_id;
	gMovie_Rec_Info[i].disp_ratio.w = disp_aspect_ratio.w;
	gMovie_Rec_Info[i].disp_ratio.h = disp_aspect_ratio.h;

	if ((gMovie_Rec_Info[i].ratio.w * gMovie_Rec_Info[i].disp_ratio.h) > (gMovie_Rec_Info[i].disp_ratio.w * gMovie_Rec_Info[i].ratio.h)) {
		DispIMEInfo.DispIMESize.h = disp_size.h;
		DispIMEInfo.DispIMESize.w = ALIGN_CEIL_4((DispIMEInfo.DispIMESize.h * gMovie_Rec_Info[i].ratio.w) / gMovie_Rec_Info[i].ratio.h);
		DispIMEInfo.DispIMEWin.w = disp_size.w;
		DispIMEInfo.DispIMEWin.h = disp_size.h;
		DispIMEInfo.DispIMEWin.x = ALIGN_CEIL_4((DispIMEInfo.DispIMESize.w - DispIMEInfo.DispIMEWin.w) / 2);
		DispIMEInfo.DispIMEWin.y = 0;
	} else {
		DispIMEInfo.DispIMESize.w = disp_size.w;
		DispIMEInfo.DispIMESize.h = ALIGN_CEIL_4((DispIMEInfo.DispIMESize.w * gMovie_Rec_Info[i].ratio.h) / gMovie_Rec_Info[i].ratio.w);
		DispIMEInfo.DispIMEWin.w = disp_size.w;
		DispIMEInfo.DispIMEWin.h = disp_size.h;
		DispIMEInfo.DispIMEWin.x = 0;
		DispIMEInfo.DispIMEWin.y = ALIGN_CEIL_4((DispIMEInfo.DispIMESize.h - DispIMEInfo.DispIMEWin.h) / 2);
	}

	if ((DispIMEInfo.DispIMEWin.x +DispIMEInfo.DispIMEWin.w > DispIMEInfo.DispIMESize.w) || (DispIMEInfo.DispIMEWin.y + DispIMEInfo.DispIMEWin.h > DispIMEInfo.DispIMESize.h)) {
		DBG_ERR("Window out of range! %d+%d>%d or %d+%d>%d\r\n",DispIMEInfo.DispIMEWin.x, DispIMEInfo.DispIMEWin.w, DispIMEInfo.DispIMESize.w, DispIMEInfo.DispIMEWin.y, DispIMEInfo.DispIMEWin.h, DispIMEInfo.DispIMESize.h);
	}

	ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1 + i, MOVIEMULTI_PARAM_DISP_IME_CROP, (UINT32)&DispIMEInfo);
}


void MovieExe_EthCamSetISECrop(UINT32 rec_id)
{
//CHKPNT;
	//ImageTrans Crop ->Scale->Rotate
	ISIZE  device_size = GxVideo_GetDeviceSize(DOUT1);
	USIZE  device_aspect_ratio = GxVideo_GetDeviceAspect(DOUT1);
	USIZE image_size={0};
	URECT DispWin={0};
	if(rec_id<_CFG_ETHCAM_ID_1 || rec_id >=_CFG_ETHCAM_ID_MAX){
		DBG_ERR("rec_id error %d\r\n",rec_id);
	}
	UINT32 idx=rec_id-_CFG_ETHCAM_ID_1;
	image_size.w=sEthCamTxDecInfo[idx].Width;
	image_size.h=sEthCamTxDecInfo[idx].Height;
	gMovie_Rec_Info[0].disp_ratio.w = device_aspect_ratio.w;
	gMovie_Rec_Info[0].disp_ratio.h = device_aspect_ratio.h;
	DBG_IND("^C disp_aspect_ratio w%d, h %d, ratio w %d, h %d\r\n",
		gMovie_Rec_Info[0].disp_ratio.w,gMovie_Rec_Info[0].disp_ratio.h,gMovie_Rec_Info[0].ratio.w,gMovie_Rec_Info[0].ratio.h);

	if ((gMovie_Rec_Info[0].ratio.w * device_aspect_ratio.h) > (device_aspect_ratio.w * gMovie_Rec_Info[0].ratio.h)) {
		//CHKPNT;
		DispWin.h = image_size.h;	
		DispWin.w = ALIGN_CEIL_4((DispWin.h * device_aspect_ratio.w) / device_aspect_ratio.h);
		DispWin.x = ALIGN_CEIL_4((image_size.w - DispWin.w) / 2);
		DispWin.y = 0;
	} else {
		//CHKPNT;
		DispWin.w = image_size.w;
		DispWin.h = ALIGN_CEIL_4((DispWin.w * device_aspect_ratio.h) / device_aspect_ratio.w);
		DispWin.x = 0;
		DispWin.y = ALIGN_CEIL_4((image_size.h - DispWin.h) / 2);
	}
	DBG_IND("^C EthCamSetISECrop device_size: size w %d, h %d, Win x %d, y %d, w %d, h %d\r\n",
		device_size.w, device_size.h, DispWin.x, DispWin.y, DispWin.w, DispWin.h);

	MOVIEMULTI_CROP_INFO CropInfo={0};
	CropInfo.size.w =  device_size.w;//1280
	CropInfo.size.h =  device_size.h;//320
	CropInfo.win.w = DispWin.w;//1920;
	CropInfo.win.h = DispWin.h;//480;
	CropInfo.win.x = DispWin.x;//0;
	CropInfo.win.y = DispWin.y;//300

debug_msg("^B before  RXCJ------sensor 2 CropInfo.win.y :%d\r\n",CropInfo.win.y );
//curDispOffset

	//CropInfo.win.y = CurStepPosBehind;
	CropInfo.win.y =  SysGetFlag(FL_IMGDISP_POS_STEP_ID1)*CurStepPosBehind; 
//	CropInfo.win.y= PipView_SetDispImgOnLcdStartY(SysGetFlag(FL_SENSOR2_DISP_OFFSET), CropInfo.size.h, CropInfo.win.h);
debug_msg("after  RXCJ------sensor 2 CropInfo.win.y :%d\r\n",CropInfo.win.y );

	//DBG_DUMP("^C size w %d, h %d, Win x %d, y %d, w %d, h %d\r\n",
	//	CropInfo.size.w, CropInfo.size.h, CropInfo.win.x, CropInfo.win.y,CropInfo.win.w, CropInfo.win.h);

	if (((CropInfo.win.x +CropInfo.win.w) > image_size.w) || ((CropInfo.win.y + CropInfo.win.h) > image_size.h)) {
		DBG_ERR("Window out of range! %d+%d>%d or %d+%d>%d\r\n",CropInfo.win.x, CropInfo.win.w, image_size.w, CropInfo.win.y, CropInfo.win.h, image_size.h);
	}  
	ImageApp_MovieMulti_SetParam(rec_id, MOVIEMULTI_PARAM_ETHCAM_DECIMG_CROP, (UINT32)&CropInfo);

}
void MovieExe_EthCamSetISECropCustom(UINT32 rec_id, ISIZE  *in_size, USIZE *in_aspect_ratio)
{
	//ImageTrans Crop ->Scale->Rotate
	ISIZE  device_size = *in_size;
	USIZE  device_aspect_ratio = *in_aspect_ratio;
	USIZE image_size={0};
	URECT DispWin={0};
	if(rec_id<_CFG_ETHCAM_ID_1 || rec_id >=_CFG_ETHCAM_ID_MAX){
		DBG_ERR("rec_id error %d\r\n",rec_id);
	}
	UINT32 idx=rec_id-_CFG_ETHCAM_ID_1;
	image_size.w=sEthCamTxDecInfo[idx].Width;
	image_size.h=sEthCamTxDecInfo[idx].Height;
	gMovie_Rec_Info[0].disp_ratio.w = device_aspect_ratio.w;
	gMovie_Rec_Info[0].disp_ratio.h = device_aspect_ratio.h;
	//DBG_DUMP("^C disp_aspect_ratio w%d, h %d, ratio w %d, h %d\r\n",
	//	gMovie_Rec_Info[0].disp_ratio.w,gMovie_Rec_Info[0].disp_ratio.h,gMovie_Rec_Info[0].ratio.w,gMovie_Rec_Info[0].ratio.h);

	if ((gMovie_Rec_Info[0].ratio.w * device_aspect_ratio.h) > (device_aspect_ratio.w * gMovie_Rec_Info[0].ratio.h)) {
		DispWin.h = image_size.h;
		DispWin.w = ALIGN_CEIL_4((DispWin.h * device_aspect_ratio.h) / device_aspect_ratio.w);

		DispWin.x = ALIGN_CEIL_4((image_size.w - DispWin.w) / 2);
		DispWin.y = 0;
	} else {
		DispWin.w = image_size.w;
		DispWin.h = ALIGN_CEIL_4((DispWin.w * device_aspect_ratio.h) / device_aspect_ratio.w);
		DispWin.x = 0;
		DispWin.y = ALIGN_CEIL_4((image_size.h - DispWin.h) / 2);
	}

	MOVIEMULTI_CROP_INFO CropInfo={0};
	CropInfo.size.w =  device_size.w;//1280
	CropInfo.size.h =  device_size.h;//320
	CropInfo.win.w = DispWin.w;//1920;
	CropInfo.win.h = DispWin.h;//480;
	CropInfo.win.x = DispWin.x;//0;
	CropInfo.win.y = DispWin.y;//300

	if ((CropInfo.win.x +CropInfo.win.w > image_size.w) || (CropInfo.win.y + CropInfo.win.h > image_size.h)) {
		DBG_ERR("Window out of range! %d+%d>%d or %d+%d>%d\r\n",CropInfo.win.x, CropInfo.win.w, image_size.w, CropInfo.win.y, CropInfo.win.h, image_size.h);
	}
	ImageApp_MovieMulti_SetParam(rec_id, MOVIEMULTI_PARAM_ETHCAM_DECIMG_CROP, (UINT32)&CropInfo);
}
#endif
#if (SENSOR_CAPS_COUNT == 2 && !defined(_NVT_ETHREARCAM_RX_))
static void MovieExe_2sensor_PIP_DispCB(void)
{
   // PIP view
	ISF_DATA	isf_data1, isf_data2, isf_new_data;
	ISF_PORT	*pSrc1, *pSrc2;
	ISF_PORT	*pDist;
	IMG_BUF		dst_img;
	UINT32		img_buf_size, buff_addr, dst_w, dst_h, dst_pxlfmt;
	UINT32		rotate_dir = 0;
	UINT32		VdoOut_w, VdoOut_h;

	APPDISP_VIEW_DRAW	pip_draw = {0};
	APPDISP_VIEW_DRAW	*pdraw = &pip_draw;

#if IMG_FULL_DISP
	static UINT32 skip_count = 0;
	if (g_PipViewStyle != UI_GetData(FL_DUAL_CAM)) {
		if (skip_count == 0) {
			UINT32 i;
			for (i = 0; i < SENSOR_CAPS_COUNT; i++) {
				MovieExe_SetIMECrop(i);CHKPNT;
			}
		}
		skip_count++;
		if (skip_count >= 6) {
			skip_count = 0;
			g_PipViewStyle = UI_GetData(FL_DUAL_CAM);
		} else {
			return;
		}
	}
#endif

    pSrc1 = ImageUnit_Out(&ISF_UserProc, ImageApp_MovieMulti_GetDispDataPort(_CFG_REC_ID_1));
    pSrc2 = ImageUnit_Out(&ISF_UserProc, ImageApp_MovieMulti_GetDispDataPort(_CFG_REC_ID_2));
	pDist = ImageUnit_In(&ISF_ImgTrans,  ImageApp_MovieMulti_GetDispImgTransPort(_CFG_DISP_ID_1));
    if (pSrc1 && pSrc2) {
    	if (ImageUnit_IsAllowPull(pSrc1) && ImageUnit_PullData(pSrc1, (ISF_DATA *)&isf_data1, 0) == ISF_OK) {
			pdraw->p_src_img[0] = (IMG_BUF *)&isf_data1.Desc[0];
		}
		if (ImageUnit_IsAllowPull(pSrc2) && ImageUnit_PullData(pSrc2, (ISF_DATA *)&isf_data2, 0) == ISF_OK) {
			pdraw->p_src_img[1] = (IMG_BUF *)&isf_data2.Desc[0];
		}

		ImageUnit_GetVdoImgSize(&ISF_VdoOut1, ISF_IN1, &VdoOut_w, &VdoOut_h);
		ImageUnit_GetVdoDirection(&ISF_VdoOut1, ISF_IN1, &rotate_dir);
		if (((rotate_dir & ISF_VDO_DIR_ROTATE_270) == ISF_VDO_DIR_ROTATE_270) || \
			((rotate_dir & ISF_VDO_DIR_ROTATE_90) == ISF_VDO_DIR_ROTATE_90) ) {
			dst_w = VdoOut_h;
			dst_h = VdoOut_w;
		} else {
			dst_w = VdoOut_w;
			dst_h = VdoOut_h;
		}

		dst_pxlfmt = GX_IMAGE_PIXEL_FMT_YUV420;
		img_buf_size = GxImg_CalcRequireSize(dst_w, dst_h, dst_pxlfmt, GXIMAGE_LINEOFFSET_ALIGN(16));
		buff_addr = ImageUnit_NewData(img_buf_size, &isf_new_data);
		if (buff_addr == 0) {
			DBG_ERR("Get buffer fail\r\n");
			goto isf_data_rel;
		}
		if (pdraw->p_src_img[0]) {
			isf_new_data.TimeStamp = isf_data1.TimeStamp;
		}
		else if (pdraw->p_src_img[1]) {
			isf_new_data.TimeStamp = isf_data2.TimeStamp;
		}
		GxImg_InitBuf(&dst_img, dst_w, dst_h, dst_pxlfmt, GXIMAGE_LINEOFFSET_ALIGN(16), buff_addr, img_buf_size);
		memcpy(&isf_new_data.Desc[0], &dst_img, sizeof(IMG_BUF));
		pdraw->p_dst_img = &dst_img;
		pdraw->viewcnt   = 2;
		DBG_IND("p_src_img 0 w =%d, h=%d \r\n",pdraw->p_src_img[0]->Width,pdraw->p_src_img[0]->Height);
		DBG_IND("p_src_img 1 w =%d, h=%d \r\n",pdraw->p_src_img[1]->Width,pdraw->p_src_img[1]->Height);
		DBG_IND("p_dst_img   w =%d, h=%d \r\n",pdraw->p_dst_img->Width,pdraw->p_dst_img->Height);
		AppDispView_OnDraw(pdraw);
		if (ImageUnit_IsAllowPush(pDist)) {
			ImageUnit_PushData(pDist, (ISF_DATA *)&isf_new_data, 0);
		}
		ImageUnit_ReleaseData(&isf_new_data);
isf_data_rel:
		if (pdraw->p_src_img[0]) {
			ImageUnit_ReleaseData(&isf_data1);
		}
		if (pdraw->p_src_img[1]) {
			ImageUnit_ReleaseData(&isf_data2);
		}
    }
}

static void MovieExe_2sensor_PIP_WifiCB(void)
{
    // PIP view
	ISF_DATA         isf_data1, isf_data2, isf_new_data;
	ISF_PORT         *pSrc1, *pSrc2;
	ISF_PORT         *pDist;
	IMG_BUF          dst_img;
	UINT32           img_buf_size, buff_addr, dst_w, dst_h, dst_pxlfmt;

	APPDISP_VIEW_DRAW	pip_draw = {0};
	APPDISP_VIEW_DRAW	*pdraw = &pip_draw;

	#if (MOVIE_UVAC_FUNC == DISABLE)
    if (System_GetState(SYS_STATE_CURRSUBMODE) != SYS_SUBMODE_WIFI) {
        return;
    }
	#endif

	pSrc1 = ImageUnit_Out(&ISF_UserProc, ImageApp_MovieMulti_GetStreamingDataPort(_CFG_REC_ID_1));
    pSrc2 = ImageUnit_Out(&ISF_UserProc, ImageApp_MovieMulti_GetStreamingDataPort(_CFG_REC_ID_2));
	pDist = ImageUnit_In(&ISF_VdoEnc, (ImageApp_MovieMulti_GetVdoEncPort(_CFG_STRM_ID_1) - ISF_OUT_BASE + ISF_IN_BASE));
    if (pSrc1 && pSrc2) {
    	if (ImageUnit_IsAllowPull(pSrc1) && ImageUnit_PullData(pSrc1, (ISF_DATA *)&isf_data1, 0) == ISF_OK) {
			pdraw->p_src_img[0] = (IMG_BUF *)&isf_data1.Desc[0];
		}
		if (ImageUnit_IsAllowPull(pSrc2) && ImageUnit_PullData(pSrc2, (ISF_DATA *)&isf_data2, 0) == ISF_OK) {
			pdraw->p_src_img[1] = (IMG_BUF *)&isf_data2.Desc[0];
		}
		ImageUnit_GetVdoImgSize(&ISF_VdoEnc, (ImageApp_MovieMulti_GetVdoEncPort(_CFG_STRM_ID_1) - ISF_OUT_BASE + ISF_IN_BASE), &dst_w, &dst_h);
		if (dst_w == 0 || dst_h == 0) {
			goto isf_data_rel;
		}
		dst_pxlfmt = GX_IMAGE_PIXEL_FMT_YUV420;
		img_buf_size = GxImg_CalcRequireSize(dst_w, dst_h, dst_pxlfmt, GXIMAGE_LINEOFFSET_ALIGN(16));
		buff_addr = ImageUnit_NewData(img_buf_size, &isf_new_data);
		if (buff_addr == 0) {
			DBG_ERR("Get buffer fail\r\n");
			goto isf_data_rel;
		}
		if (pdraw->p_src_img[0]) {
			isf_new_data.TimeStamp = isf_data1.TimeStamp;
		}
		else if (pdraw->p_src_img[1]) {
			isf_new_data.TimeStamp = isf_data2.TimeStamp;
		}
		GxImg_InitBuf(&dst_img, dst_w, dst_h, dst_pxlfmt, GXIMAGE_LINEOFFSET_ALIGN(16), buff_addr, img_buf_size);
		memcpy(&isf_new_data.Desc[0], &dst_img, sizeof(IMG_BUF));
		pdraw->p_dst_img = &dst_img;
		pdraw->viewcnt   = 2;
		DBG_IND("p_src_img 0 w =%d, h=%d \r\n",pdraw->p_src_img[0]->Width,pdraw->p_src_img[0]->Height);
		DBG_IND("p_src_img 1 w =%d, h=%d \r\n",pdraw->p_src_img[1]->Width,pdraw->p_src_img[1]->Height);
		DBG_IND("p_dst_img w =%d, h=%d \r\n",pdraw->p_dst_img->Width,pdraw->p_dst_img->Height);
		AppDispView_OnDraw(pdraw);
		if (ImageUnit_IsAllowPush(pDist)) {
			ImageUnit_PushData(pDist, (ISF_DATA *)&isf_new_data, 0);
		}
		ImageUnit_ReleaseData(&isf_new_data);
isf_data_rel:
		if (pdraw->p_src_img[0]) {
			ImageUnit_ReleaseData(&isf_data1);
		}
		if (pdraw->p_src_img[1]) {
			ImageUnit_ReleaseData(&isf_data2);
		}
    }
}
#endif
#if (SENSOR_CAPS_COUNT == 2)
static void MovieExe_2sensor_HotPlug_Disp(void)
{
	UINT32 u32CurrSensorEn = System_GetEnableSensor();
	UINT32 u32PrevSensorEn = System_GetPrevEnableSensor();
	UINT32 u32Mask;
	UINT32 i;

#if defined(_SENSOR1_CMOS_OFF_)
	u32CurrSensorEn &= ~SENSOR_1;
	u32PrevSensorEn &= ~SENSOR_1;
#endif
#if defined(_SENSOR2_CMOS_OFF_)
	u32CurrSensorEn &= ~SENSOR_2;
	u32PrevSensorEn &= ~SENSOR_2;
#endif

	debug_msg("^M%s: u32CurrSensorEn = 0x%x\r\n", __func__, u32CurrSensorEn);
	debug_msg("^M%s: u32PrevSensorEn = 0x%x\r\n", __func__, u32PrevSensorEn);

	u32Mask = 1;
	for (i=0; i<SENSOR_CAPS_COUNT; i++){
		if ((u32PrevSensorEn & u32Mask) != (u32CurrSensorEn & u32Mask)){
			if (u32CurrSensorEn & u32Mask){
				ImageApp_MovieMulti_ImgLinkForDisp(i, ENABLE, TRUE);
			}else{
				ImageApp_MovieMulti_ImgLinkForDisp(i, DISABLE, TRUE);
			}
		}

		u32Mask <<= 1;
	}//for (i=0; i<SENSOR_CAPS_COUNT; i++)
}

static void MovieExe_2sensor_HotPlug_WiFi(void)
{
	UINT32 u32CurrSensorEn = System_GetEnableSensor();
	UINT32 u32PrevSensorEn = System_GetPrevEnableSensor();
	UINT32 u32Mask;
	UINT32 i;

#if defined(_SENSOR1_CMOS_OFF_)
	u32CurrSensorEn &= ~SENSOR_1;
	u32PrevSensorEn &= ~SENSOR_1;
#endif
#if defined(_SENSOR2_CMOS_OFF_)
	u32CurrSensorEn &= ~SENSOR_2;
	u32PrevSensorEn &= ~SENSOR_2;
#endif

	//debug_msg("^M%s: u32CurrSensorEn = 0x%x\r\n", __func__, u32CurrSensorEn);
	//debug_msg("^M%s: u32PrevSensorEn = 0x%x\r\n", __func__, u32PrevSensorEn);

	u32Mask = 1;
	for (i=0; i<SENSOR_CAPS_COUNT; i++){
		if ((u32PrevSensorEn & u32Mask) != (u32CurrSensorEn & u32Mask)){
			if (u32CurrSensorEn & u32Mask){
				ImageApp_MovieMulti_ImgLinkForStreaming(i, ENABLE, TRUE);
			}else{
				ImageApp_MovieMulti_ImgLinkForStreaming(i, DISABLE, TRUE);
			}
		}

		u32Mask <<= 1;
	}//for (i=0; i<SENSOR_CAPS_COUNT; i++)

}

static void MovieExe_2sensor_HotPlug_Rec(void)
{
	UINT32 sensor_enable = System_GetEnableSensor(); // sensor enabled
    UINT32 i, u32Mask;
	UINT32 isRec;

	if (System_GetState(SYS_STATE_CURRSUBMODE) == SYS_SUBMODE_WIFI){
		// not considering the case that all sensors can be plugged out temporarily
		isRec = ImageApp_MovieMulti_IsStreamRunning(_CFG_REC_ID_1) |
				ImageApp_MovieMulti_IsStreamRunning(_CFG_REC_ID_2);
	} else {
		isRec = (FlowMovie_GetMovDataState() == MOV_ST_REC) ? 1 : 0;
	}

    u32Mask = 1;
	if (isRec) {
		for (i=0; i<SENSOR_CAPS_COUNT; i++){
			if ((sensor_enable & u32Mask)/*plug*/ && (!ImageApp_MovieMulti_IsStreamRunning(gMovie_Rec_Info[i].rec_id))/*not recording*/){
				ImageApp_MovieMulti_RecStart(gMovie_Rec_Info[i].rec_id);
			}else if (!(sensor_enable & u32Mask)/*unplug*/ && ImageApp_MovieMulti_IsStreamRunning(gMovie_Rec_Info[i].rec_id)/*recording*/){
				ImageApp_MovieMulti_RecStop(gMovie_Rec_Info[i].rec_id);
			}
			u32Mask <<= 1;
		}//for (i=0; i<SENSOR_CAPS_COUNT; i++)
	}//if (FlowMovie_GetMovDataState() == MOV_ST_REC)
}

#endif
static void MovieExe_ImageStreamOpenMulti(void)
{
#if(defined(_NVT_ETHREARCAM_RX_))
	#if defined(_ASR_ENABLE_) // Enable AudCap if _ASR_ENABLE_.
	MOVIEMULTI_MAX_LINK_INFO MaxLinkInfo = {SENSOR_CAPS_COUNT, 1, 1, 1, ETH_REARCAM_CAPS_COUNT};   //Img, Disp, Wifi, AudCap, EthCam
	#elif (AUDPROC_FUNC == ENABLE)
	MOVIEMULTI_MAX_LINK_INFO MaxLinkInfo = {SENSOR_CAPS_COUNT, 1, 1, 1, ETH_REARCAM_CAPS_COUNT};   //Img, Disp, Wifi, AudCap, EthCam
	#else
	MOVIEMULTI_MAX_LINK_INFO MaxLinkInfo = {SENSOR_CAPS_COUNT, 1, 1, 0, ETH_REARCAM_CAPS_COUNT};   //Img, Disp, Wifi, AudCap, EthCam
	#endif
	ImageApp_MovieMulti_Config(MOVIE_CONFIG_MAX_LINK_INFO, (UINT32) &MaxLinkInfo);
	gMovie_Rec_Option.emr_on = _CFG_EMR_OFF;
#endif
#if (SENSOR_CAPS_COUNT == 2 && !defined(_NVT_ETHREARCAM_RX_))
	ImageApp_MovieMulti_SetParam(_CFG_DISP_ID_1, MOVIEMULTI_PARAM_DISP_REG_CB, (UINT32)MovieExe_2sensor_PIP_DispCB);
	ImageApp_MovieMulti_SetParam(_CFG_STRM_ID_1, MOVIEMULTI_PARAM_WIFI_REG_CB, (UINT32)MovieExe_2sensor_PIP_WifiCB);
#elif (defined(_NVT_ETHREARCAM_RX_))
	ImageApp_MovieMulti_SetParam(_CFG_STRM_ID_1, MOVIEMULTI_PARAM_WIFI_REG_CB, (UINT32)MovieExe_EthCam_PIP_WifiCB);
#endif
}
static void MovieExe_SetVdoCodecByRecID(MOVIE_CFG_REC_ID rec_id, MOVIE_CFG_CODEC codec_type)
{
	UINT32 pathID;

	//if (System_GetState(SYS_STATE_CURRMODE) == PRIMARY_MODE_MOVIE) {
		ImageApp_MovieMulti_SetParam(rec_id, MOVIEMULTI_PARAM_CODEC, codec_type);
		ImageApp_MovieMulti_SetParam(rec_id, MOVIEMULTI_PARAM_PROFILE, (codec_type == _CFG_CODEC_H264) ? _CFG_PROFILE_HIGH : _CFG_PROFILE_MAIN);
	//}

	pathID = (UINT32)ImageApp_MovieMulti_GetVdoEncPort(rec_id);

	if (codec_type == _CFG_CODEC_MJPG) {
		#if (VIDEO_FUNC_MJPG == ENABLE)
			MP_VDOENC_ENCODER *pObj = MP_MjpgEnc_getVideoObject((MP_VDOENC_ID)pathID);
			ImageApp_MovieMulti_SetParam(rec_id, MOVIEMULTI_PARAM_VDO_ENCODER_OBJ, (UINT32)pObj);
		#else
			ImageApp_MovieMulti_SetParam(rec_id, MOVIEMULTI_PARAM_VDO_ENCODER_OBJ, 0);
			DBG_ERR("[MOVIE][%d] codec = %d not support\r\n", pathID, codec_type);
		#endif
	}

	if (codec_type == _CFG_CODEC_H264) {
		#if (VIDEO_FUNC_H264 == ENABLE)
			MP_VDOENC_ENCODER *pObj = MP_H264Enc_getVideoObject((MP_VDOENC_ID)pathID);
			ImageApp_MovieMulti_SetParam(rec_id, MOVIEMULTI_PARAM_VDO_ENCODER_OBJ, (UINT32)pObj);
		#else
			ImageApp_MovieMulti_SetParam(rec_id, MOVIEMULTI_PARAM_VDO_ENCODER_OBJ, 0);
			DBG_ERR("[MOVIE][%d] codec = %d not support\r\n", pathID, codec_type);
		#endif
	}

	if (codec_type == _CFG_CODEC_H265) {
		#if (VIDEO_FUNC_H265 == ENABLE)
			MP_VDOENC_ENCODER *pObj = MP_H265Enc_getVideoObject((MP_VDOENC_ID)pathID);
			ImageApp_MovieMulti_SetParam(rec_id, MOVIEMULTI_PARAM_VDO_ENCODER_OBJ, (UINT32)pObj);
		#else
			ImageApp_MovieMulti_SetParam(rec_id, MOVIEMULTI_PARAM_VDO_ENCODER_OBJ, 0);
			DBG_ERR("[MOVIE][%d] codec = %d not support\r\n", pathID, codec_type);
		#endif
	}
}
#if(defined(_NVT_ETHREARCAM_TX_))
void MovieExe_EthCamRecId1_SetVdoEncBufSec(UINT32 Sec)
{
	g_EthCamRecId1_VdoEncBufSec= Sec;
}
UINT32 MovieExe_EthCamRecId1_GetVdoEncBufSec(void)
{
	g_EthCamRecId1_VdoEncBufSec = (g_EthCamRecId1_VdoEncBufSec==0)? 3 : g_EthCamRecId1_VdoEncBufSec;
	return g_EthCamRecId1_VdoEncBufSec;
}
UINT32 MovieExe_EthCamRecId1_GetBsMapTblAddr(UINT32 blk_size)
{
        NVTMPP_VB_BLK  blk=0;
        CHAR pool_name[20] ={0};

        //DBG_DUMP("socketCli_GetRecvBufAddr blk_size=%d\r\n",blk_size);

        if(g_EthCamRecId1_GetBsMapTblPool==NVTMPP_VB_INVALID_POOL)  {
            sprintf(pool_name,"RecId1_BsMapTbl");
            g_EthCamRecId1_GetBsMapTblPool=nvtmpp_vb_create_pool(pool_name, blk_size , 1, NVTMPP_DDR_1);
            if (NVTMPP_VB_INVALID_POOL == g_EthCamRecId1_GetBsMapTblPool)	{
        		DBG_ERR("create private pool err\r\n");
        		return NVTEVT_CONSUME;
            }

            blk = nvtmpp_vb_get_block(0, g_EthCamRecId1_GetBsMapTblPool, blk_size, NVTMPP_DDR_1);
            if (NVTMPP_VB_INVALID_BLK == blk) {
        		DBG_ERR("get vb block err\r\n");
                	return NVTEVT_CONSUME;
            }
            g_EthCamRecId1_GetBsMapTblPoolAddr=nvtmpp_vb_block2addr(blk);
        }
        if(g_EthCamRecId1_GetBsMapTblPoolAddr == 0)
    		DBG_ERR("get buf addr err\r\n");
        return g_EthCamRecId1_GetBsMapTblPoolAddr;
}
void MovieExe_EthCamRecId1_DestroyBsMapTblBuff(void)
{
	INT32 ret;
	if(g_EthCamRecId1_GetBsMapTblPool != NVTMPP_VB_INVALID_POOL){
		ret = nvtmpp_vb_destroy_pool(g_EthCamRecId1_GetBsMapTblPool);
		if (NVTMPP_ER_OK != ret) {
			DBG_ERR("destory pool error pool 0x%x   ret=%d\r\n", g_EthCamRecId1_GetBsMapTblPool, ret);
		}
		g_EthCamRecId1_GetBsMapTblPool=NVTMPP_VB_INVALID_POOL;
		g_EthCamRecId1_GetBsMapTblPoolAddr=0;
	}
	memset(&g_RecId1SendFrmBsQ ,0 ,sizeof(SEND_FRM_BSQ));
}

static void MovieExe_EthCamRecId1_PutBsQ(ISF_DATA *pData)
{
	static UINT32 errCnt=0;
	wai_sem(ETHCAM_CMD_SNDFRAME_SEM_ID);
	//ISF_VIDEO_STREAM_BUF *pVS = (ISF_VIDEO_STREAM_BUF *)&pData->Desc[0];
	//DBG_DUMP("0x%x, DataSize=%d ,0x%x\n\r",pVS->DataAddr,pVS->DataSize);
	if ((g_RecId1SendFrmBsQ.GetQ== g_RecId1SendFrmBsQ.PutQ) && (g_RecId1SendFrmBsQ.bFull == TRUE)) {
		if(errCnt % 100 == 0){
			DBG_ERR("RecId1SendFrm Queue is Full! Q=%d\n\r", g_RecId1SendFrmBsQ.GetQ);
		}
		errCnt++;
	}else{
		if(g_RecId1SendFrmBsQ.Queue){
			g_RecId1SendFrmBsQ.PutQ = (g_RecId1SendFrmBsQ.PutQ + 1) % (MovieExe_EthCamRecId1_GetVdoEncBufSec()*MovieExe_GetFps(_CFG_REC_ID_1));

			hwmem_open();
			hwmem_memcpy((UINT32)&g_RecId1SendFrmBsQ.Queue[g_RecId1SendFrmBsQ.PutQ], (UINT32)pData, sizeof(ISF_DATA));
			hwmem_close();
		}
		if (g_RecId1SendFrmBsQ.GetQ == g_RecId1SendFrmBsQ.PutQ) { // Check Queue full
			g_RecId1SendFrmBsQ.bFull = TRUE;
		}
		errCnt=0;
	}
	sig_sem(ETHCAM_CMD_SNDFRAME_SEM_ID);
}

static void MovieExe_EthCamRecId1_GetBsQ(ISF_DATA *pData)
{
	static UINT32 errCnt=0;
	wai_sem(ETHCAM_CMD_SNDFRAME_SEM_ID);
	if ((g_RecId1SendFrmBsQ.GetQ== g_RecId1SendFrmBsQ.PutQ) && (g_RecId1SendFrmBsQ.bFull == FALSE)) {
		if(errCnt % 100 == 0){
		//DBG_ERR("RecId1SendFrm Queue is Empty! Q=%d\n\r",g_RecId1SendFrmBsQ.GetQ);
		}
		errCnt++;
	}else{
		if(g_RecId1SendFrmBsQ.Queue){

			g_RecId1SendFrmBsQ.GetQ = (g_RecId1SendFrmBsQ.GetQ + 1) % (MovieExe_EthCamRecId1_GetVdoEncBufSec()*MovieExe_GetFps(_CFG_REC_ID_1));
			hwmem_open();
			hwmem_memcpy((UINT32)pData, (UINT32)&g_RecId1SendFrmBsQ.Queue[g_RecId1SendFrmBsQ.GetQ], sizeof(ISF_DATA));
			hwmem_close();
		}
		if (g_RecId1SendFrmBsQ.GetQ == g_RecId1SendFrmBsQ.PutQ) { // Check Queue full
			g_RecId1SendFrmBsQ.bFull = FALSE;
		}
		errCnt=0;
	}
	sig_sem(ETHCAM_CMD_SNDFRAME_SEM_ID);
}
void MovieExe_EthCamRecId1_SendFrm(void)
{
	ISF_DATA isf_Data={0};
	ISF_VIDEO_STREAM_BUF *pVS;
	MovieExe_EthCamRecId1_GetBsQ(&isf_Data);
	pVS = (ISF_VIDEO_STREAM_BUF *)&isf_Data.Desc[0];
	if(pVS->DataAddr && pVS->DataSize){
		ImageApp_MovieMulti_EthCamTxRecId1_CBLink(0, &isf_Data, (UINT32)&EthCamData1_Send);
	}
}
void MovieExe_EthCamRecId1_ResetBsQ(void)
{
	if(sEthCamSysInfo.PullModeEn){
		wai_sem(ETHCAM_CMD_SNDFRAME_SEM_ID);
		g_RecId1SendFrmBsQ.bFull = 0;
		g_RecId1SendFrmBsQ.GetQ = 0;
		g_RecId1SendFrmBsQ.PutQ  = 0;
		sig_sem(ETHCAM_CMD_SNDFRAME_SEM_ID);
	}
}

static USERPROC_TRIG_PROC_VAL MovieExe_EthCamRecId1_CB(ISF_PORT *pPort, ISF_DATA *pData)
{
	if(sEthCamSysInfo.PullModeEn){
		MovieExe_EthCamRecId1_PutBsQ(pData);
	}else{
		ImageApp_MovieMulti_EthCamTxRecId1_CBLink(pPort, pData, (UINT32)&EthCamData1_Send);
	}
	return USERPROC_TRIG_PROC_NONE;
}
#if 1//(ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
UINT32 MovieExe_EthCamCloneId1_GetBsMapTblAddr(UINT32 blk_size)
{
        NVTMPP_VB_BLK  blk=0;
        CHAR pool_name[20] ={0};

        //DBG_DUMP("socketCli_GetRecvBufAddr blk_size=%d\r\n",blk_size);

        if(g_EthCamCloneId1_GetBsMapTblPool==NVTMPP_VB_INVALID_POOL)  {
            sprintf(pool_name,"CloneId1_BsMapTbl");
            g_EthCamCloneId1_GetBsMapTblPool=nvtmpp_vb_create_pool(pool_name, blk_size , 1, NVTMPP_DDR_1);
            if (NVTMPP_VB_INVALID_POOL == g_EthCamCloneId1_GetBsMapTblPool)	{
        		DBG_ERR("create private pool err\r\n");
        		return NVTEVT_CONSUME;
            }
            blk = nvtmpp_vb_get_block(0, g_EthCamCloneId1_GetBsMapTblPool, blk_size, NVTMPP_DDR_1);
            if (NVTMPP_VB_INVALID_BLK == blk) {
        		DBG_ERR("get vb block err\r\n");
                	return NVTEVT_CONSUME;
            }
            g_EthCamCloneId1_GetBsMapTblPoolAddr=nvtmpp_vb_block2addr(blk);
        }
        if(g_EthCamCloneId1_GetBsMapTblPoolAddr == 0)
    		DBG_ERR("get buf addr err\r\n");
        return g_EthCamCloneId1_GetBsMapTblPoolAddr;
}
void MovieExe_EthCamCloneId1_DestroyBsMapTblBuff(void)
{
	INT32 ret;
	if(g_EthCamCloneId1_GetBsMapTblPool != NVTMPP_VB_INVALID_POOL){
		ret = nvtmpp_vb_destroy_pool(g_EthCamCloneId1_GetBsMapTblPool);
		if (NVTMPP_ER_OK != ret) {
			DBG_ERR("destory pool error pool 0x%x   ret=%d\r\n", g_EthCamCloneId1_GetBsMapTblPool, ret);
		}
		g_EthCamCloneId1_GetBsMapTblPool=NVTMPP_VB_INVALID_POOL;
		g_EthCamCloneId1_GetBsMapTblPoolAddr=0;
	}
	memset(&g_CloneId1SendFrmBsQ ,0 ,sizeof(SEND_FRM_BSQ));
}
static void MovieExe_EthCamCloneId1_PutBsQ(ISF_DATA *pData)
{
	static UINT32 errCnt=0;
	wai_sem(ETHCAM_CMD_SNDFRAME_CLONE_SEM_ID);
	if ((g_CloneId1SendFrmBsQ.GetQ== g_CloneId1SendFrmBsQ.PutQ) && (g_CloneId1SendFrmBsQ.bFull == TRUE)) {
		if(errCnt % 100 == 0){
			DBG_ERR("CloneId1SendFrm Queue is Full! Q=%d\n\r", g_CloneId1SendFrmBsQ.GetQ);
		}
		errCnt++;
	}else{
		if(g_CloneId1SendFrmBsQ.Queue){
			g_CloneId1SendFrmBsQ.PutQ = (g_CloneId1SendFrmBsQ.PutQ + 1) % (3*MovieExe_GetFps(_CFG_CLONE_ID_1));
			hwmem_open();
			hwmem_memcpy((UINT32)&g_CloneId1SendFrmBsQ.Queue[g_CloneId1SendFrmBsQ.PutQ], (UINT32)pData, sizeof(ISF_DATA));
			hwmem_close();
		}
		if (g_CloneId1SendFrmBsQ.GetQ == g_CloneId1SendFrmBsQ.PutQ) { // Check Queue full
			g_CloneId1SendFrmBsQ.bFull = TRUE;
		}
	}
	sig_sem(ETHCAM_CMD_SNDFRAME_CLONE_SEM_ID);
}
static void MovieExe_EthCamCloneId1_GetBsQ(ISF_DATA *pData)
{
	wai_sem(ETHCAM_CMD_SNDFRAME_CLONE_SEM_ID);
	if ((g_CloneId1SendFrmBsQ.GetQ== g_CloneId1SendFrmBsQ.PutQ) && (g_CloneId1SendFrmBsQ.bFull == FALSE)) {
	}else{
		if(g_CloneId1SendFrmBsQ.Queue){
			g_CloneId1SendFrmBsQ.GetQ = (g_CloneId1SendFrmBsQ.GetQ + 1) % (3*MovieExe_GetFps(_CFG_CLONE_ID_1));
			hwmem_open();
			hwmem_memcpy((UINT32)pData, (UINT32)&g_CloneId1SendFrmBsQ.Queue[g_CloneId1SendFrmBsQ.GetQ], sizeof(ISF_DATA));
			hwmem_close();
		}
		if (g_CloneId1SendFrmBsQ.GetQ == g_CloneId1SendFrmBsQ.PutQ) { // Check Queue full
			g_CloneId1SendFrmBsQ.bFull = FALSE;
		}
	}
	sig_sem(ETHCAM_CMD_SNDFRAME_CLONE_SEM_ID);
}
void MovieExe_EthCamCloneId1_SendFrm(void)
{
	if(sEthCamSysInfo.CloneDisplayPathEn){
		ISF_DATA isf_Data={0};
		ISF_VIDEO_STREAM_BUF *pVS;
		MovieExe_EthCamCloneId1_GetBsQ(&isf_Data);
		pVS = (ISF_VIDEO_STREAM_BUF *)&isf_Data.Desc[0];
		if(pVS->DataAddr && pVS->DataSize){
			ImageApp_MovieMulti_EthCamTxCloneId1_CBLink(0, &isf_Data, (UINT32)&EthCamData2_Send);
		}
	}
}
void MovieExe_EthCamCloneId1_ResetBsQ(void)
{
	if(sEthCamSysInfo.CloneDisplayPathEn){
		if(sEthCamSysInfo.PullModeEn){
			wai_sem(ETHCAM_CMD_SNDFRAME_CLONE_SEM_ID);
			g_CloneId1SendFrmBsQ.bFull = 0;
			g_CloneId1SendFrmBsQ.GetQ = 0;
			g_CloneId1SendFrmBsQ.PutQ  = 0;
			sig_sem(ETHCAM_CMD_SNDFRAME_CLONE_SEM_ID);
		}
	}
}
static USERPROC_TRIG_PROC_VAL MovieExe_EthCamCloneId1_CB(ISF_PORT *pPort, ISF_DATA *pData)
{
	if(sEthCamSysInfo.CloneDisplayPathEn){
		if(sEthCamSysInfo.PullModeEn){
			MovieExe_EthCamCloneId1_PutBsQ(pData);
		}else{
		ImageApp_MovieMulti_EthCamTxCloneId1_CBLink(pPort, pData, (UINT32)&EthCamData2_Send);
		}
	}
	return USERPROC_TRIG_PROC_NONE;
}
static USERPROC_TRIG_PROC_VAL MovieExe_EthCamTxRecId1AudCap_CB(ISF_PORT *pPort, ISF_DATA *pData)
{
	ImageApp_MovieMulti_EthCamTxRecId1AudCap_CBLink(pPort, pData);
	return USERPROC_TRIG_PROC_NONE;
}
#endif
void MovieExe_EthCamTxDateStampConfig(void)
{
#if 1
	if (UI_GetData(FL_MOVIE_DATEIMPRINT) == MOVIE_DATEIMPRINT_ON) {
		UINT32		uiStampAddr;
		STAMP_COLOR StampColorBg = {RGB_GET_Y( 16,	16,  16), RGB_GET_U( 16,  16,  16), RGB_GET_V( 16,	16,  16)}; // date stamp background color
		STAMP_COLOR StampColorFr = {RGB_GET_Y( 16,	16,  16), RGB_GET_U( 16,  16,  16), RGB_GET_V( 16,	16,  16)}; // date stamp frame color
		STAMP_COLOR StampColorFg = {RGB_GET_Y(224, 224, 192), RGB_GET_U(224, 224, 192), RGB_GET_V(224, 224, 192)}; // date stamp foreground color
		UINT32 path_id=ImageApp_MovieMulti_GetVdoEncPort(_CFG_REC_ID_1);

		uiStampAddr=MovieStamp_GetBufAddr(path_id, MovieStamp_CalcBufSize(gMovie_Rec_Info[_CFG_REC_ID_1].size.w, gMovie_Rec_Info[_CFG_REC_ID_1].size.h));//POOL_SIZE_DATEIMPRINT/2);
		MovieStamp_SetBuffer(path_id, uiStampAddr, MovieStamp_CalcBufSize(gMovie_Rec_Info[_CFG_REC_ID_1].size.w, gMovie_Rec_Info[_CFG_REC_ID_1].size.h));//POOL_SIZE_DATEIMPRINT/2);

		MovieStamp_SetColor(path_id, &StampColorBg, &StampColorFr, &StampColorFg);
		MovieStamp_Setup(
			path_id,
			STAMP_ON |
			STAMP_AUTO |
			STAMP_DATE_TIME |
			STAMP_BOTTOM_RIGHT |
			STAMP_POS_NORMAL |
			STAMP_BG_TRANSPARENT |
			STAMP_YY_MM_DD |
			STAMP_IMG_420UV,
			gMovie_Rec_Info[_CFG_REC_ID_1].size.w,
			gMovie_Rec_Info[_CFG_REC_ID_1].size.h,
#if defined (WATERLOGO_FUNCTION) && (WATERLOGO_FUNCTION == ENABLE)
			(WATERLOGO_BUFFER *)&g_WaterLogo);
#else
			NULL);
#endif

		MovieStamp_Enable();
	}else{
		MovieStamp_Disable();
	}
#endif
}
void MovieExe_EthCamTxStart(MOVIE_CFG_REC_ID  rec_id)
{
	if (SysGetFlag(FL_MOVIE_TIMELAPSE_REC) == MOVIE_TIMELAPSEREC_OFF) {
		if (rec_id == _CFG_REC_ID_1) {
			ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1, MOVIEMULTI_PARAM_REC_FORMAT, _CFG_REC_TYPE_AV);
			ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1, MOVIEMULTI_PARAM_TIMELAPSE_TIME, 0);
			ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1, MOVIEMULTI_PARAM_SKIP_FRAME, 3);
		}
		if (rec_id == _CFG_CLONE_ID_1) {
			ImageApp_MovieMulti_SetParam(_CFG_CLONE_ID_1, MOVIEMULTI_PARAM_REC_FORMAT, _CFG_REC_TYPE_AV);
			ImageApp_MovieMulti_SetParam(_CFG_CLONE_ID_1, MOVIEMULTI_PARAM_TIMELAPSE_TIME, 0);
			ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1, MOVIEMULTI_PARAM_SKIP_FRAME, 3);
		}
	} else {
		if (rec_id == _CFG_REC_ID_1) {
			ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1, MOVIEMULTI_PARAM_REC_FORMAT, _CFG_REC_TYPE_TIMELAPSE);
			ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1, MOVIEMULTI_PARAM_TIMELAPSE_TIME, Get_MovieTimeLapseValue(SysGetFlag(FL_MOVIE_TIMELAPSE_REC)));
			ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1, MOVIEMULTI_PARAM_SKIP_FRAME, 0);
		}
		if (rec_id == _CFG_CLONE_ID_1) {
			ImageApp_MovieMulti_SetParam(_CFG_CLONE_ID_1, MOVIEMULTI_PARAM_REC_FORMAT, _CFG_REC_TYPE_AV);
			ImageApp_MovieMulti_SetParam(_CFG_CLONE_ID_1, MOVIEMULTI_PARAM_TIMELAPSE_TIME, 0);
			ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1, MOVIEMULTI_PARAM_SKIP_FRAME, 3);
		}
	}

	if (rec_id == _CFG_REC_ID_1) {
		UINT32 AudCapFuncEn = FALSE;
		ImageApp_MovieMulti_GetParam(_CFG_REC_ID_1, MOVIEMULTI_PARAM_AUD_CAP_FUNC_EN, &AudCapFuncEn);
		if(AudCapFuncEn){
			ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1, MOVIEMULTI_PARAM_VDOENC_BS_RESERVED_SIZE,  (2*1024));
		}
	}
	if (rec_id == _CFG_REC_ID_1) {
		if(sEthCamSysInfo.PullModeEn){
			if(g_RecId1SendFrmBsQ.Queue==NULL && g_EthCamRecId1_GetBsMapTblPoolAddr==0){
				g_EthCamRecId1_GetBsMapTblPoolAddr = MovieExe_EthCamRecId1_GetBsMapTblAddr(sizeof(ISF_DATA)*MovieExe_EthCamRecId1_GetVdoEncBufSec()*MovieExe_GetFps(_CFG_REC_ID_1));
				g_RecId1SendFrmBsQ.Queue=(ISF_DATA *)g_EthCamRecId1_GetBsMapTblPoolAddr;
				memset((void*)g_EthCamRecId1_GetBsMapTblPoolAddr,0,sizeof(ISF_DATA)*MovieExe_EthCamRecId1_GetVdoEncBufSec()*MovieExe_GetFps(_CFG_REC_ID_1));
			}
		}

		ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1, MOVIEMULTI_PARAM_ETHCAM_TX_BS_CB, (UINT32)MovieExe_EthCamRecId1_CB);
		if(g_EthCamTxIsAudCapEn){
			ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1, MOVIEMULTI_PARAM_AUD_CAP_REG_CB, (UINT32)MovieExe_EthCamTxRecId1AudCap_CB);
		}
		if(_CFG_REC_ID_1==rec_id && ImageApp_MovieMulti_IsStreamRunning(_CFG_REC_ID_1 | ETHCAM_TX_MAGIC_KEY)){
			DBG_WRN("rec_id=%d already start\r\n",rec_id);
			return;
		}
		MovieExe_EthCamRecId1_ResetBsQ();

		ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1, MOVIEMULTI_PARAM_ENCBUF_MS, MovieExe_EthCamRecId1_GetVdoEncBufSec()*1000);
		ImageApp_MovieMulti_EthCamTxStart(_CFG_REC_ID_1);
		if(g_EthCamTxIsAudCapEn){
			ImageApp_MovieMulti_AudCapStart(_CFG_REC_ID_1);
		}
		MovieExe_EthCamTxDateStampConfig();
	}
#if 1//(ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
	if(sEthCamSysInfo.CloneDisplayPathEn){
		if (rec_id == _CFG_CLONE_ID_1) {
			if (sEthCamSysInfo.PullModeEn) {
				if (g_CloneId1SendFrmBsQ.Queue == NULL && g_EthCamCloneId1_GetBsMapTblPoolAddr == 0) {
					g_EthCamCloneId1_GetBsMapTblPoolAddr = MovieExe_EthCamCloneId1_GetBsMapTblAddr(sizeof(ISF_DATA)*3*MovieExe_GetFps(_CFG_CLONE_ID_1));
					g_CloneId1SendFrmBsQ.Queue = (ISF_DATA *)g_EthCamCloneId1_GetBsMapTblPoolAddr;
					memset((void*)g_EthCamCloneId1_GetBsMapTblPoolAddr,0,sizeof(ISF_DATA)*3*MovieExe_GetFps(_CFG_CLONE_ID_1));
				}
			}
			ImageApp_MovieMulti_SetParam(_CFG_CLONE_ID_1, MOVIEMULTI_PARAM_ETHCAM_TX_BS_CB, (UINT32)MovieExe_EthCamCloneId1_CB);

			if (rec_id == _CFG_CLONE_ID_1 && ImageApp_MovieMulti_IsStreamRunning(_CFG_CLONE_ID_1 | ETHCAM_TX_MAGIC_KEY)) {
				DBG_WRN("rec_id=%d already start\r\n",rec_id);
				return;
			}
			MovieExe_EthCamCloneId1_ResetBsQ();

			ImageApp_MovieMulti_EthCamTxStart(_CFG_CLONE_ID_1);
		}
	}
#endif

}

void MovieExe_EthCamTxStop(MOVIE_CFG_REC_ID  rec_id)
{
	if(rec_id==_CFG_REC_ID_1){
		if(g_EthCamTxIsAudCapEn){
			ImageApp_MovieMulti_AudCapStop(_CFG_REC_ID_1);
		}
		ImageApp_MovieMulti_EthCamTxStop(_CFG_REC_ID_1);
		MovieExe_EthCamRecId1_ResetBsQ();
	}
#if 1//(ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
	if(sEthCamSysInfo.CloneDisplayPathEn){
		if(rec_id==_CFG_CLONE_ID_1){
			ImageApp_MovieMulti_EthCamTxStop(_CFG_CLONE_ID_1);
			MovieExe_EthCamCloneId1_ResetBsQ();
		}
	}
#endif
}
#endif
#if(defined(_NVT_ETHREARCAM_RX_))//Eth cam Receiver data
static USERPROC_TRIG_PROC_VAL MovieExe_EthCam_PIP_WifiCB(void)
{
    // PIP view
	ISF_PORT         *pSrc[SENSOR_CAPS_COUNT+ETH_REARCAM_CAPS_COUNT];
	ISF_DATA         isf_data[SENSOR_CAPS_COUNT+ETH_REARCAM_CAPS_COUNT]={0};

	ISF_PORT         *pDist;
	APPDISP_VIEW_DRAW	pip_draw = {0};
	APPDISP_VIEW_DRAW	*pdraw = &pip_draw;
	UINT32 i, u32Mask;
	UINT32 sensor_enable = System_GetEnableSensor();
	BOOL  bPullData[SENSOR_CAPS_COUNT+ETH_REARCAM_CAPS_COUNT] = {0};

	#if (MOVIE_UVAC_FUNC == DISABLE)
	if (System_GetState(SYS_STATE_CURRSUBMODE) != SYS_SUBMODE_WIFI) {
		return USERPROC_TRIG_PROC_NONE;
	}
	#endif

	pDist = ImageUnit_In(&ISF_VdoEnc, (ImageApp_MovieMulti_GetVdoEncPort(_CFG_STRM_ID_1) - ISF_OUT_BASE + ISF_IN_BASE));

	u32Mask = 1;
	pdraw->viewcnt = 0;

	for (i=0; i<(SENSOR_CAPS_COUNT & SENSOR_ON_MASK); i++){
		if (sensor_enable & u32Mask){
			pSrc[i] = ImageUnit_Out(&ISF_UserProc, ImageApp_MovieMulti_GetStreamingDataPort(_CFG_REC_ID_1+i));

			if (ImageUnit_IsAllowPull(pSrc[i]) && pSrc[i]){
				if (ImageUnit_PullData(pSrc[i], (ISF_DATA *)&isf_data[i], 100) != ISF_OK){
					DBG_ERR("pSrc[%d] PullData Failed\r\n", i);
					goto isf_data_rel;
				}else{
					pdraw->viewcnt++;
					pdraw->p_src_img[i] = (IMG_BUF *)&isf_data[i].Desc[0];
					bPullData[i] = TRUE;
					DBG_IND("p_src_img 0 w =%d, h=%d \r\n",pdraw->p_src_img[i]->Width,pdraw->p_src_img[i]->Height);
				}
			}
		}
		u32Mask <<= 1;
	} //for (i=0; i<SENSOR_CAPS_COUNT; i++)

#if(((SENSOR_CAPS_COUNT & SENSOR_ON_MASK) + ETH_REARCAM_CAPS_COUNT)==1)
	pdraw->viewcnt++;
#endif

	ISF_RV ret;

	UINT32 j;
	for (i=_CFG_ETHCAM_ID_1; i<(_CFG_ETHCAM_ID_1+ETH_REARCAM_CAPS_COUNT); i++){
		#if(((SENSOR_CAPS_COUNT & SENSOR_ON_MASK) + ETH_REARCAM_CAPS_COUNT)==1)
		j=i-_CFG_ETHCAM_ID_1+(SENSOR_CAPS_COUNT);
		#else
		j=i-_CFG_ETHCAM_ID_1+(SENSOR_CAPS_COUNT & SENSOR_ON_MASK);
		#endif
#if(ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
		if (socketCliEthData2_IsRecv(i-_CFG_ETHCAM_ID_1))
#else
		if (socketCliEthData1_IsRecv(i-_CFG_ETHCAM_ID_1))
#endif
		{
			wai_sem(ETHCAM_WIFICB_VDOFRM_SEM_ID);
			pdraw->p_src_img[j]=MovieExe_EthCam_PIP_WifiCB_GetVdoFrm((i-_CFG_ETHCAM_ID_1));
			sig_sem(ETHCAM_WIFICB_VDOFRM_SEM_ID);
			if(pdraw->p_src_img[j]){
				if(!(pdraw->p_src_img[j]->flag & MAKEFOURCC('V','R','A','W'))) {
					DBG_IND("pSrc[%d] VdoDec not start!\r\n", j);
					goto isf_data_rel;
				}else{
					pdraw->viewcnt++;
				}
			}else{
				DBG_ERR("pSrc[%d] GetVdoFrm Failed\r\n", j);
				goto isf_data_rel;
			}
		}
	}


#if(((SENSOR_CAPS_COUNT == 1)  && (ETH_REARCAM_CAPS_COUNT == 1)))// || (defined(_CMOS_SIEPATGEN_) && (SENSOR_CAPS_COUNT == 1) && (ETH_REARCAM_CAPS_COUNT == 2)))

	UINT32           img_buf_size , buff_addr, dst_w, dst_h, dst_pxlfmt;
	ISF_DATA         isf_new_data;
	IMG_BUF          dst_img;

	ImageUnit_GetVdoImgSize(&ISF_VdoEnc, (ImageApp_MovieMulti_GetVdoEncPort(_CFG_STRM_ID_1) - ISF_OUT_BASE + ISF_IN_BASE), &dst_w, &dst_h);

	//DBG_DUMP("dst_w=%d, %d\r\n",dst_w,dst_h);

	dst_pxlfmt = GX_IMAGE_PIXEL_FMT_YUV420;
	img_buf_size = GxImg_CalcRequireSize(dst_w, dst_h, dst_pxlfmt, GXIMAGE_LINEOFFSET_ALIGN(16));
	buff_addr = ImageUnit_NewData(img_buf_size, &isf_new_data);
	if (buff_addr == 0) {
		DBG_ERR("Get buffer fail\r\n");
		goto isf_data_rel;
	}
	if (pdraw->p_src_img[0] && isf_data[0].TimeStamp) {
		isf_new_data.TimeStamp = isf_data[0].TimeStamp;
	}else if (pdraw->p_src_img[1] && isf_data[1].TimeStamp) {
		isf_new_data.TimeStamp = isf_data[1].TimeStamp;
	}else{
		isf_new_data.TimeStamp = HwClock_GetLongCounter();
	}
	GxImg_InitBuf(&dst_img, dst_w, dst_h, dst_pxlfmt, GXIMAGE_LINEOFFSET_ALIGN(16), buff_addr, img_buf_size);
	//memcpy(&isf_new_data.Desc[0], &dst_img, sizeof(IMG_BUF));
	hwmem_open();
	hwmem_memcpy((UINT32)&isf_new_data.Desc[0], (UINT32)&dst_img, sizeof(IMG_BUF));
	hwmem_close();
	pdraw->p_dst_img = &dst_img;
	pdraw->viewcnt   = 2;
	AppDispView_OnDraw(pdraw);
	if (ImageUnit_IsAllowPush(pDist)) {
		if ((ret=ImageUnit_PushData(pDist, (ISF_DATA *)&isf_new_data, 0)) != ISF_OK){
			DBG_IND("ImageUnit_PushData Failed, %d, hData=0x%x\r\n", ret,isf_new_data.hData);
		}
	}
	if ((ret=ImageUnit_ReleaseData(&isf_new_data)) != ISF_OK){
		DBG_IND("Release New Data Failed, %d, hData=0x%x\r\n", ret,isf_new_data.hData);
	}

#else//#elif((SENSOR_CAPS_COUNT >= 0) && (ETH_REARCAM_CAPS_COUNT >= 2))
	UINT32           img_buf_size , buff_addr, dst_w, dst_h, dst_pxlfmt;
	ISF_DATA         isf_new_data;
	IMG_BUF          dst_img;
#if 0
	if(UI_GetData(FL_DUAL_CAM) == DUALCAM_FRONT){//Tx1 ////Rx Sensor2
		if(pdraw->p_src_img[1]){
			pdraw->p_dst_img = pdraw->p_src_img[1];
			AppDispView_OnDraw(pdraw);
			ImageUnit_PushData(pDist, (ISF_DATA *)&isf_data[1], 0);
		}
	}else if(UI_GetData(FL_DUAL_CAM) == DUALCAM_BEHIND){
		if(pdraw->p_src_img[2]){
			pdraw->p_dst_img = pdraw->p_src_img[2];
			AppDispView_OnDraw(pdraw);
			ImageUnit_PushData(pDist, (ISF_DATA *)&isf_data[2], 0);
		}
	}else
#endif
	{
		ImageUnit_GetVdoImgSize(&ISF_VdoEnc, (ImageApp_MovieMulti_GetVdoEncPort(_CFG_STRM_ID_1) - ISF_OUT_BASE + ISF_IN_BASE), &dst_w, &dst_h);

		//DBG_DUMP("dst_w=%d, %d\r\n",dst_w,dst_h);

		dst_pxlfmt = GX_IMAGE_PIXEL_FMT_YUV420;
		img_buf_size = GxImg_CalcRequireSize(dst_w, dst_h, dst_pxlfmt, GXIMAGE_LINEOFFSET_ALIGN(16));
		buff_addr = ImageUnit_NewData(img_buf_size, &isf_new_data);
		if (buff_addr == 0) {
			DBG_ERR("Get buffer fail\r\n");
			goto isf_data_rel;
		}
		if (pdraw->p_src_img[0] &&  isf_data[0].TimeStamp) {
			isf_new_data.TimeStamp = isf_data[0].TimeStamp;
		}else if (pdraw->p_src_img[1] &&  isf_data[1].TimeStamp) {
			isf_new_data.TimeStamp = isf_data[1].TimeStamp;
		}else if (pdraw->p_src_img[2] &&  isf_data[2].TimeStamp) {
			isf_new_data.TimeStamp = isf_data[2].TimeStamp;
		}else{
			isf_new_data.TimeStamp = HwClock_GetLongCounter();
		}
		GxImg_InitBuf(&dst_img, dst_w, dst_h, dst_pxlfmt, GXIMAGE_LINEOFFSET_ALIGN(16), buff_addr, img_buf_size);
		//memcpy(&isf_new_data.Desc[0], &dst_img, sizeof(IMG_BUF));
		hwmem_open();
		hwmem_memcpy((UINT32)&isf_new_data.Desc[0], (UINT32)&dst_img, sizeof(IMG_BUF));
		hwmem_close();

		pdraw->p_dst_img = &dst_img;

		//pdraw->viewcnt   = 3;
		pdraw->viewcnt   = (SENSOR_CAPS_COUNT & SENSOR_ON_MASK) + ETH_REARCAM_CAPS_COUNT;

		AppDispView_OnDraw(pdraw);
		if (ImageUnit_IsAllowPush(pDist)) {
			if ((ret=ImageUnit_PushData(pDist, (ISF_DATA *)&isf_new_data, 0)) != ISF_OK){
				DBG_IND("ImageUnit_PushData Failed, %d, hData=0x%x\r\n", ret,isf_new_data.hData);
			}
		}
		if ((ret=ImageUnit_ReleaseData(&isf_new_data)) != ISF_OK){
			DBG_IND("Release New Data Failed, %d, hData=0x%x\r\n", ret,isf_new_data.hData);
		}
	}
#endif
isf_data_rel:
	for (i=0; i<(SENSOR_CAPS_COUNT+ETH_REARCAM_CAPS_COUNT); i++){
		if (bPullData[i]){
			if ((ret=ImageUnit_ReleaseData(&isf_data[i])) != ISF_OK){
				DBG_IND("[%d]ReleaseData Failed, %d, hData=0x%x\r\n", i,ret,isf_data[i].hData);
			}
		}
	}
	return USERPROC_TRIG_PROC_NONE;
}
#if (ETH_REARCAM_CAPS_COUNT==1 && ETHCAM_DISP_MAX_QUEUE>0)
static void MovieExe_EthCamDisp_PutQ(ISF_DATA *pData)
{
	static UINT32 errCnt=0;
	wai_sem(ETHCAM_DISP_DATA_SEM_ID);
	//ISF_VIDEO_STREAM_BUF *pVS = (ISF_VIDEO_STREAM_BUF *)&pData->Desc[0];
	//DBG_DUMP("0x%x, DataSize=%d ,0x%x\n\r",pVS->DataAddr,pVS->DataSize);
	if ((ETHCAM_DISP_MAX_QUEUE >1) && (g_EthCamDispQ.GetQ== g_EthCamDispQ.PutQ) && (g_EthCamDispQ.bFull == TRUE)) {
		if(errCnt % 100 == 0){
			DBG_ERR("EthCamDisp Queue is Full! Q=%d\n\r", g_EthCamDispQ.GetQ);
		}
		errCnt++;
	}
	//mark for drop old data
	//else
	{
		if(g_EthCamDispQ.Queue){
			g_EthCamDispQ.PutQ = (g_EthCamDispQ.PutQ + 1) % (ETHCAM_DISP_MAX_QUEUE);

			if(g_EthCamDispQ.Queue[g_EthCamDispQ.PutQ].Desc[0]){
				ImageUnit_ReleaseData(&g_EthCamDispQ.Queue[g_EthCamDispQ.PutQ]);
			}

			hwmem_open();
			hwmem_memcpy((UINT32)&g_EthCamDispQ.Queue[g_EthCamDispQ.PutQ], (UINT32)pData, sizeof(ISF_DATA));
			hwmem_close();
		}
		if (g_EthCamDispQ.GetQ == g_EthCamDispQ.PutQ) { // Check Queue full
			g_EthCamDispQ.bFull = TRUE;
		}
	}
	sig_sem(ETHCAM_DISP_DATA_SEM_ID);
}

static void MovieExe_EthCamDisp_GetQ(ISF_DATA *pData)
{
	wai_sem(ETHCAM_DISP_DATA_SEM_ID);
	if ((g_EthCamDispQ.GetQ== g_EthCamDispQ.PutQ) && (g_EthCamDispQ.bFull == FALSE)) {
		//DBG_ERR("EthCamDisp Queue is Empty! Q=%d\n\r",g_EthCamDispQ.GetQ);
	}else{
		if(g_EthCamDispQ.Queue){

			g_EthCamDispQ.GetQ = (g_EthCamDispQ.GetQ + 1) % (ETHCAM_DISP_MAX_QUEUE);
			hwmem_open();
			hwmem_memcpy((UINT32)pData, (UINT32)&g_EthCamDispQ.Queue[g_EthCamDispQ.GetQ], sizeof(ISF_DATA));
			hwmem_close();
		}
		if (g_EthCamDispQ.GetQ == g_EthCamDispQ.PutQ) { // Check Queue full
			g_EthCamDispQ.bFull = FALSE;
		}
	}
	sig_sem(ETHCAM_DISP_DATA_SEM_ID);
}
void MovieExe_EthCamDisp_PushData(void)
{
	ISF_DATA isf_Data={0};
	ISF_PORT  *pDist;
	pDist = ImageUnit_In(&ISF_ImgTrans,  ImageApp_MovieMulti_GetDispImgTransPort(_CFG_DISP_ID_1));

	MovieExe_EthCamDisp_GetQ(&isf_Data);

	if (isf_Data.Desc[0]){//ImageUnit_IsAllowPush(pDist)) {
		ImageUnit_PushData(pDist, (ISF_DATA *)&isf_Data, 100);
	}
}
void MovieExe_EthCamDisp_ResetQ(void)
{
	wai_sem(ETHCAM_DISP_DATA_SEM_ID);
	g_EthCamDispQ.bFull = 0;
	g_EthCamDispQ.GetQ = 0;
	g_EthCamDispQ.PutQ  = 0;
	sig_sem(ETHCAM_DISP_DATA_SEM_ID);
}
static void EthCamCmd_DispTimerCB(UINT32 TimerId)
{
	MovieExe_EthCamDisp_PushData();
}
#endif
static USERPROC_TRIG_PROC_VAL MovieExe_EthCam_PIP_DispCB(ISF_PORT *pPort, ISF_DATA *pData)
{
#if 0
	ISF_PORT *pDist;
	pDist = ImageUnit_In(&ISF_ImgTrans, ImageApp_MovieMulti_GetDispImgTransPort(_CFG_DISP_ID_1));

	ImageUnit_PushData(pDist, pData, 0);
#else
	ISF_DATA  isf_data[SENSOR_CAPS_COUNT+ETH_REARCAM_CAPS_COUNT] = {0};
	ISF_PORT  *pSrc[SENSOR_CAPS_COUNT+ETH_REARCAM_CAPS_COUNT] = {0}, *pDist;
	BOOL  bPullData[SENSOR_CAPS_COUNT+ETH_REARCAM_CAPS_COUNT] = {0};
	UINT32 sensor_enable = System_GetEnableSensor();
	UINT32 i, j, u32Mask;
	//UINT32 i,  u32Mask;
	APPDISP_VIEW_DRAW    pip_draw = {0};
	APPDISP_VIEW_DRAW    *pdraw = &pip_draw;
	ISF_RV ret;

#if IMG_FULL_DISP
	static UINT32 skip_count = 0;
	if (g_PipViewStyle != UI_GetData(FL_DUAL_CAM)) {
		ISIZE  device_size = GxVideo_GetDeviceSize(DOUT1);
		USIZE  device_aspect_ratio = GxVideo_GetDeviceAspect(DOUT1);
		ISIZE  in_device_size;
		USIZE  in_device_aspect_ratio;
		BOOL is_set_tx=0;

		if (skip_count == 0) {
			UINT32 i;
			for (i = 0; i < SENSOR_CAPS_COUNT; i++) {
				if(UI_GetData(FL_DUAL_CAM)>=DUALCAM_CUSTOM_1){
					if(UI_GetData(FL_DUAL_CAM) == DUALCAM_CUSTOM_2){//Tx1, Rx, Tx2, 1:2:1
						in_device_size.w=ALIGN_CEIL_4(device_size.w/2);
						in_device_size.h=device_size.h;
						in_device_aspect_ratio.w=device_aspect_ratio.w/2;
						in_device_aspect_ratio.h=device_aspect_ratio.h;
						MovieExe_SetIMECropCustom(i, &in_device_size, &in_device_aspect_ratio);
					}else if(UI_GetData(FL_DUAL_CAM) == DUALCAM_CUSTOM_3){//Tx1, Rx, Tx2, 1:1:1
						in_device_size.w=ALIGN_CEIL_4(device_size.w/3);
						in_device_size.h=device_size.h;
						in_device_aspect_ratio.w=device_aspect_ratio.w/3;
						in_device_aspect_ratio.h=device_aspect_ratio.h;
						MovieExe_SetIMECropCustom(i, &in_device_size, &in_device_aspect_ratio);
					}else if(UI_GetData(FL_DUAL_CAM) == DUALCAM_CUSTOM_4){// Rx, Tx1, 1:1
						in_device_size.w=ALIGN_CEIL_4(device_size.w/2);
						in_device_size.h=device_size.h;
						in_device_aspect_ratio.w=device_aspect_ratio.w/2;
						in_device_aspect_ratio.h=device_aspect_ratio.h;
						MovieExe_SetIMECropCustom(i, &in_device_size, &in_device_aspect_ratio);
					}
				}else{CHKPNT;
				MovieExe_SetIMECrop(i);
			}
		}
			if(UI_GetData(FL_DUAL_CAM) == DUALCAM_CUSTOM_1){//Tx1, Tx2
				in_device_size.w=ALIGN_CEIL_4(device_size.w/2);
				in_device_size.h=device_size.h;
				in_device_aspect_ratio.w=device_aspect_ratio.w/2;
				in_device_aspect_ratio.h=device_aspect_ratio.h;
				is_set_tx=1;
			}else if(UI_GetData(FL_DUAL_CAM) == DUALCAM_CUSTOM_2){//Tx1, Rx, Tx2, 1:2:1
				in_device_size.w=ALIGN_CEIL_4(device_size.w/4);
				in_device_size.h=device_size.h;
				in_device_aspect_ratio.w=device_aspect_ratio.w/4;
				in_device_aspect_ratio.h=device_aspect_ratio.h;
				is_set_tx=1;
			}else if(UI_GetData(FL_DUAL_CAM) == DUALCAM_CUSTOM_3){//Tx1, Rx, Tx2, 1:1:1
				in_device_size.w=ALIGN_CEIL_4(device_size.w/3);
				in_device_size.h=device_size.h;
				in_device_aspect_ratio.w=device_aspect_ratio.w/3;
				in_device_aspect_ratio.h=device_aspect_ratio.h;
				is_set_tx=1;
			}else if(UI_GetData(FL_DUAL_CAM) == DUALCAM_CUSTOM_4){// Rx, Tx1, 1:1
				in_device_size.w=ALIGN_CEIL_4(device_size.w/2);
				in_device_size.h=device_size.h;
				in_device_aspect_ratio.w=device_aspect_ratio.w/2;
				in_device_aspect_ratio.h=device_aspect_ratio.h;
				is_set_tx=1;
			}else{
				if(socketCliEthData1_IsRecv(ETHCAM_PATH_ID_1)){
					MovieExe_EthCamSetISECrop(_CFG_ETHCAM_ID_1);
				}
				if(socketCliEthData1_IsRecv(ETHCAM_PATH_ID_2)){
					MovieExe_EthCamSetISECrop(_CFG_ETHCAM_ID_2);
				}
			}
			if(is_set_tx){
				if(socketCliEthData1_IsRecv(ETHCAM_PATH_ID_1)){
					MovieExe_EthCamSetISECropCustom(_CFG_ETHCAM_ID_1,&in_device_size, &in_device_aspect_ratio);
				}
				if(socketCliEthData1_IsRecv(ETHCAM_PATH_ID_2)){
					MovieExe_EthCamSetISECropCustom(_CFG_ETHCAM_ID_2,&in_device_size, &in_device_aspect_ratio);
				}
			}
		}
		skip_count++;
		if (skip_count >= 6) {
			skip_count = 0;
			g_PipViewStyle = UI_GetData(FL_DUAL_CAM);
		} else {
			return USERPROC_TRIG_PROC_NONE;
		}
	}
#endif

	u32Mask = 1;
	pdraw->viewcnt = 0;
	if(UI_GetData(FL_DUAL_CAM) != DUALCAM_BEHIND){
		for (i=0; i<(SENSOR_CAPS_COUNT & SENSOR_ON_MASK); i++){
			if (sensor_enable & u32Mask){
				pSrc[i] = ImageUnit_Out(&ISF_UserProc, ImageApp_MovieMulti_GetDispDataPort(i));
				if (ImageUnit_IsAllowPull(pSrc[i]) && pSrc[i]){
					if (ImageUnit_PullData(pSrc[i], (ISF_DATA *)&isf_data[i], 1) != ISF_OK){
						DBG_ERR("pSrc[%d] PullData Failed\r\n", i);
						//ImageUnit_ReleaseData(&isf_data[i]);
						//return USERPROC_TRIG_PROC_NONE;
						goto isf_data_rel;
					}else{
						pdraw->viewcnt++;
						pdraw->p_src_img[i] = (IMG_BUF *)&isf_data[i].Desc[0];
						bPullData[i] = TRUE;
						DBG_IND("p_src_img 0 w =%d, h=%d \r\n",pdraw->p_src_img[i]->Width,pdraw->p_src_img[i]->Height);
					}
				}
			}
			u32Mask <<= 1;
		} //for (i=0; i<SENSOR_CAPS_COUNT; i++)
	}
#if(((SENSOR_CAPS_COUNT & SENSOR_ON_MASK) + ETH_REARCAM_CAPS_COUNT)==1)
	pdraw->viewcnt++;
#endif


	for (i=_CFG_ETHCAM_ID_1; i<(_CFG_ETHCAM_ID_1+ETH_REARCAM_CAPS_COUNT); i++){
		if (1){//ImageApp_MovieMulti_IsStreamRunning(i)){
			#if(((SENSOR_CAPS_COUNT & SENSOR_ON_MASK) + ETH_REARCAM_CAPS_COUNT)==1)
			j=i-_CFG_ETHCAM_ID_1+(SENSOR_CAPS_COUNT);
			#else
			j=i-_CFG_ETHCAM_ID_1+(SENSOR_CAPS_COUNT & SENSOR_ON_MASK);
			#endif
			if(ImageApp_MovieMulti_GetDispDataPort(i)>=0 && ImageApp_MovieMulti_GetDispDataPort(i)!= 0xffffffff){
				pSrc[j] = ImageUnit_Out(&ISF_UserProc, ImageApp_MovieMulti_GetDispDataPort(i));
#if(ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
				if (socketCliEthData2_IsRecv(i-_CFG_ETHCAM_ID_1) && ImageUnit_IsAllowPull(pSrc[j]) && pSrc[j])
#else
				if (socketCliEthData1_IsRecv(i-_CFG_ETHCAM_ID_1) && ImageUnit_IsAllowPull(pSrc[j]) && pSrc[j])
#endif
				{
					if ((ret=ImageUnit_PullData(pSrc[j], (ISF_DATA *)&isf_data[j], 1)) != ISF_OK){
						if(ret != ISF_ERR_QUEUE_EMPTY){
							DBG_ERR("Eth pSrc[%d] PullData Failed\r\n", j);
						}
						goto isf_data_rel;
					}else{
						if(g_isEthCamDispDecErr[i-_CFG_ETHCAM_ID_1]==FALSE){
						pdraw->viewcnt++;
						pdraw->p_src_img[j] = (IMG_BUF *)&isf_data[j].Desc[0];
						bPullData[j] = TRUE;
						DBG_IND("eth p_src_img[%d]  w =%d, h=%d \r\n",j,pdraw->p_src_img[j]->Width,pdraw->p_src_img[j]->Height);
						}else{
							DBG_ERR("DispDecErr pSrc[%d] skip\r\n", j);
							ImageUnit_ReleaseData(&isf_data[j]);
						}
					}
				}
			}
		}
	}
	pDist = ImageUnit_In(&ISF_ImgTrans,  ImageApp_MovieMulti_GetDispImgTransPort(_CFG_DISP_ID_1));
#if(((SENSOR_CAPS_COUNT == 1)  && (ETH_REARCAM_CAPS_COUNT == 1)))
	BOOL bBehindFlip=0;
#if (DUALCAM_PIP_BEHIND_FLIP == ENABLE)
 	//bBehindFlip=0; //if Tx Flip
 	bBehindFlip=1; //if Tx NO Flip
#endif
#if 1
	if(UI_GetData(FL_DUAL_CAM) == DUALCAM_FRONT || (UI_GetData(FL_DUAL_CAM) == DUALCAM_BEHIND && bBehindFlip==0)){
		if(UI_GetData(FL_DUAL_CAM) == DUALCAM_FRONT){
			pdraw->p_dst_img = pdraw->p_src_img[0];
		}else{// if(UI_GetData(FL_DUAL_CAM) == DUALCAM_BEHIND || UI_GetData(FL_DUAL_CAM) == DUALCAM_BOTH2){
			pdraw->p_dst_img = pdraw->p_src_img[1];
		}
		DBG_IND("p_dst_img  w =%d, h=%d \r\n",pdraw->p_dst_img->Width,pdraw->p_dst_img->Height);
		pdraw->viewcnt   = 2;
		AppDispView_OnDraw(pdraw);
		#if (ETHCAM_DISP_MAX_QUEUE==0)
		if(UI_GetData(FL_DUAL_CAM) == DUALCAM_FRONT){
			ImageUnit_PushData(pDist, (ISF_DATA *)&isf_data[0], 0);
		}else{// if(UI_GetData(FL_DUAL_CAM) == DUALCAM_BEHIND){
			ImageUnit_PushData(pDist, (ISF_DATA *)&isf_data[1], 0);
		}
		#else
		if(UI_GetData(FL_DUAL_CAM) == DUALCAM_FRONT){
			ImageUnit_PushData(pDist, (ISF_DATA *)&isf_data[0], 0);
		}else{// if(UI_GetData(FL_DUAL_CAM) == DUALCAM_BEHIND){
			MovieExe_EthCamDisp_PutQ((ISF_DATA *)&isf_data[1]);

#if (DUALCAM_PIP_BEHIND_FLIP == ENABLE)
			if (UI_GetData(FL_DUAL_CAM) == DUALCAM_BEHIND){
			#if ((VDO_ROTATE_DIR == ISF_VDO_DIR_ROTATE_90) || (VDO_ROTATE_DIR == ISF_VDO_DIR_ROTATE_270))
			GxVideo_SetDeviceCtrl(DOUT1, DISPLAY_DEVCTRL_DIRECT, MIRROR_Y);
			#else
			GxVideo_SetDeviceCtrl(DOUT1, DISPLAY_DEVCTRL_DIRECT, MIRROR_X);
			#endif
			}else{
				GxVideo_SetDeviceCtrl(DOUT1, DISPLAY_DEVCTRL_DIRECT, MIRROR_DISCARD);
			}
#endif
			return USERPROC_TRIG_PROC_NONE;
		}
		#endif
	}else{
		UINT32           img_buf_size=0 , buff_addr=0, dst_w=0, dst_h=0, dst_pxlfmt=0;
		ISF_DATA         isf_new_data={0};
		IMG_BUF          dst_img={0};
		UINT32		rotate_dir = 0;
		UINT32		VdoOut_w=0, VdoOut_h=0;
		ImageUnit_GetVdoImgSize(&ISF_VdoOut1, ISF_IN1, &VdoOut_w, &VdoOut_h);
		ImageUnit_GetVdoDirection(&ISF_VdoOut1, ISF_IN1, &rotate_dir);

		if (((rotate_dir & ISF_VDO_DIR_ROTATE_270) == ISF_VDO_DIR_ROTATE_270) || \
			((rotate_dir & ISF_VDO_DIR_ROTATE_90) == ISF_VDO_DIR_ROTATE_90) ) {
			dst_w = VdoOut_h;
			dst_h = VdoOut_w;
		} else {
			dst_w = VdoOut_w;
			dst_h = VdoOut_h;
		}
		dst_pxlfmt = GX_IMAGE_PIXEL_FMT_YUV420;
		img_buf_size = GxImg_CalcRequireSize(dst_w, dst_h, dst_pxlfmt, GXIMAGE_LINEOFFSET_ALIGN(16));

		buff_addr = ImageUnit_NewData(img_buf_size, &isf_new_data);
		if (buff_addr == 0) {
			DBG_ERR("Get buffer fail\r\n");
			goto isf_data_rel;
		}
		if (pdraw->p_src_img[0]) {
			isf_new_data.TimeStamp = isf_data[0].TimeStamp;
		}
		else if (pdraw->p_src_img[1]) {
			isf_new_data.TimeStamp = isf_data[1].TimeStamp;
		}
		GxImg_InitBuf(&dst_img, dst_w, dst_h, dst_pxlfmt, GXIMAGE_LINEOFFSET_ALIGN(16), buff_addr, img_buf_size);
		#if 0
		hwmem_open();
		hwmem_memcpy((UINT32)&isf_new_data.Desc[0], (UINT32)&dst_img, sizeof(IMG_BUF));
		hwmem_close();
		#else
		IMG_BUF *pDesc=(IMG_BUF*)&isf_new_data.Desc[0];
		pDesc->flag=dst_img.flag;
		pDesc->Width=dst_img.Width;
		pDesc->Height=dst_img.Height;
		pDesc->PxlFmt=dst_img.PxlFmt;
		pDesc->PxlAddr[0]=dst_img.PxlAddr[0];
		pDesc->PxlAddr[1]=dst_img.PxlAddr[1];
		pDesc->PxlAddr[2]=dst_img.PxlAddr[2];
		pDesc->LineOffset[0]=dst_img.LineOffset[0];
		pDesc->LineOffset[1]=dst_img.LineOffset[1];
		pDesc->LineOffset[2]=dst_img.LineOffset[2];
		pDesc->ScaleRatio=dst_img.ScaleRatio;
		pDesc->RealCoord=dst_img.RealCoord;
		pDesc->Metadata.Addr=dst_img.Metadata.Addr;
		pDesc->Metadata.Size=dst_img.Metadata.Size;
		#endif
		pdraw->p_dst_img = &dst_img;


		DBG_IND("p_dst_img  w =%d, h=%d \r\n",pdraw->p_dst_img->Width,pdraw->p_dst_img->Height);
		pdraw->viewcnt   = 2;

		AppDispView_OnDraw(pdraw);

		if (ImageUnit_IsAllowPush(pDist)) {
			ImageUnit_PushData(pDist, (ISF_DATA *)&isf_new_data, 0);
		}
		ImageUnit_ReleaseData(&isf_new_data);
	}
    #if (DUALCAM_PIP_BEHIND_FLIP == ENABLE)
    if (UI_GetData(FL_DUAL_CAM) == DUALCAM_BEHIND){
		#if ((VDO_ROTATE_DIR == ISF_VDO_DIR_ROTATE_90) || (VDO_ROTATE_DIR == ISF_VDO_DIR_ROTATE_270))
		GxVideo_SetDeviceCtrl(DOUT1, DISPLAY_DEVCTRL_DIRECT, MIRROR_Y);	
		#else
		GxVideo_SetDeviceCtrl(DOUT1, DISPLAY_DEVCTRL_DIRECT, MIRROR_X);	
		#endif
   	}else{
   		GxVideo_SetDeviceCtrl(DOUT1, DISPLAY_DEVCTRL_DIRECT, MIRROR_DISCARD);	
   	}
	#endif

#else
	if(UI_GetData(FL_DUAL_CAM) == DUALCAM_FRONT || UI_GetData(FL_DUAL_CAM) == DUALCAM_BOTH){
		pdraw->p_dst_img = pdraw->p_src_img[0];
	}else{// if(UI_GetData(FL_DUAL_CAM) == DUALCAM_BEHIND || UI_GetData(FL_DUAL_CAM) == DUALCAM_BOTH2){
		pdraw->p_dst_img = pdraw->p_src_img[1];
	}
	DBG_IND("p_dst_img  w =%d, h=%d \r\n",pdraw->p_dst_img->Width,pdraw->p_dst_img->Height);
	AppDispView_OnDraw(pdraw);
	if(UI_GetData(FL_DUAL_CAM) == DUALCAM_FRONT || UI_GetData(FL_DUAL_CAM) == DUALCAM_BOTH){
		ImageUnit_PushData(pDist, (ISF_DATA *)&isf_data[0], 0);
	}else{// if(UI_GetData(FL_DUAL_CAM) == DUALCAM_BEHIND || UI_GetData(FL_DUAL_CAM) == DUALCAM_BOTH2){
		ImageUnit_PushData(pDist, (ISF_DATA *)&isf_data[1], 0);
	}
#endif

#else//#elif((SENSOR_CAPS_COUNT >= 0) && (ETH_REARCAM_CAPS_COUNT >= 2))
	UINT32           img_buf_size , buff_addr, dst_w, dst_h, dst_pxlfmt;
	ISF_DATA         isf_new_data;
	IMG_BUF          dst_img;
	UINT32		rotate_dir = 0;
	UINT32		VdoOut_w, VdoOut_h;

#if (DUALCAM_PIP_BEHIND_FLIP == DISABLE)
    if (!(SENSOR_CAPS_COUNT & SENSOR_ON_MASK)) {
		if ((socketCliEthData1_IsRecv(ETHCAM_PATH_ID_1)) && (!socketCliEthData1_IsRecv(ETHCAM_PATH_ID_2))) {
			UI_SetData(FL_DUAL_CAM, DUALCAM_FRONT);
			UI_SetData(FL_DUAL_CAM_MENU, DUALCAM_FRONT);
		} else if ((!socketCliEthData1_IsRecv(ETHCAM_PATH_ID_1)) && (socketCliEthData1_IsRecv(ETHCAM_PATH_ID_2))) {
			UI_SetData(FL_DUAL_CAM, DUALCAM_BEHIND);
			UI_SetData(FL_DUAL_CAM_MENU, DUALCAM_BEHIND);	
		}
	}
	if(UI_GetData(FL_DUAL_CAM) == DUALCAM_FRONT){//Tx1
		if (SENSOR_CAPS_COUNT & SENSOR_ON_MASK) {
		if(pdraw->p_src_img[1]){
			pdraw->p_dst_img = pdraw->p_src_img[1];
			AppDispView_OnDraw(pdraw);
			ImageUnit_PushData(pDist, (ISF_DATA *)&isf_data[1], 0);
			}
		} else {
			if(pdraw->p_src_img[0]){
				pdraw->p_dst_img = pdraw->p_src_img[0];
				AppDispView_OnDraw(pdraw);
				ImageUnit_PushData(pDist, (ISF_DATA *)&isf_data[0], 0);
			}		
		}
	}else if(UI_GetData(FL_DUAL_CAM) == DUALCAM_BEHIND){//Tx2
	    if (SENSOR_CAPS_COUNT & SENSOR_ON_MASK) {
		if(pdraw->p_src_img[2]){
			pdraw->p_dst_img = pdraw->p_src_img[2];
			AppDispView_OnDraw(pdraw);
			ImageUnit_PushData(pDist, (ISF_DATA *)&isf_data[2], 0);
		}
	    } else {
			if(pdraw->p_src_img[1]){
				pdraw->p_dst_img = pdraw->p_src_img[1];
				AppDispView_OnDraw(pdraw);
				ImageUnit_PushData(pDist, (ISF_DATA *)&isf_data[1], 0);
			}	    
	    }
	#if (SENSOR_CAPS_COUNT & SENSOR_ON_MASK)
	}else if(UI_GetData(FL_DUAL_CAM) == DUALCAM_BOTH2){ //Rx Sensor
		if(pdraw->p_src_img[0]){
			pdraw->p_dst_img = pdraw->p_src_img[0];
			AppDispView_OnDraw(pdraw);
			ImageUnit_PushData(pDist, (ISF_DATA *)&isf_data[0], 0);
		}
        #endif	
	}else
#endif
	{
		ImageUnit_GetVdoImgSize(&ISF_VdoOut1, ISF_IN1,&VdoOut_w, &VdoOut_h);
		ImageUnit_GetVdoDirection(&ISF_VdoOut1, ISF_IN1, &rotate_dir);
		if (((rotate_dir & ISF_VDO_DIR_ROTATE_270) == ISF_VDO_DIR_ROTATE_270) || \
			((rotate_dir & ISF_VDO_DIR_ROTATE_90) == ISF_VDO_DIR_ROTATE_90) ) {
			dst_w = VdoOut_h;
			dst_h = VdoOut_w;
		} else {
			dst_w = VdoOut_w;
			dst_h = VdoOut_h;
		}

		dst_pxlfmt = GX_IMAGE_PIXEL_FMT_YUV420;
		img_buf_size = GxImg_CalcRequireSize(dst_w, dst_h, dst_pxlfmt, GXIMAGE_LINEOFFSET_ALIGN(16));
		buff_addr = ImageUnit_NewData(img_buf_size, &isf_new_data);
		if (buff_addr == 0) {
			DBG_ERR("Get buffer fail\r\n");
			goto isf_data_rel;
		}
		if (pdraw->p_src_img[0]) {
			isf_new_data.TimeStamp = isf_data[0].TimeStamp;
		}
		else if (pdraw->p_src_img[1]) {
			isf_new_data.TimeStamp = isf_data[1].TimeStamp;
		}
		else if (pdraw->p_src_img[2]) {
			isf_new_data.TimeStamp = isf_data[2].TimeStamp;
		}
		GxImg_InitBuf(&dst_img, dst_w, dst_h, dst_pxlfmt, GXIMAGE_LINEOFFSET_ALIGN(16), buff_addr, img_buf_size);
		memcpy(&isf_new_data.Desc[0], &dst_img, sizeof(IMG_BUF));
		pdraw->p_dst_img = &dst_img;

		//pdraw->viewcnt   = 3;
		pdraw->viewcnt   = (SENSOR_CAPS_COUNT & SENSOR_ON_MASK) + ETH_REARCAM_CAPS_COUNT;

		AppDispView_OnDraw(pdraw);
		if (ImageUnit_IsAllowPush(pDist)) {
			ImageUnit_PushData(pDist, (ISF_DATA *)&isf_new_data, 0);
		}
		ImageUnit_ReleaseData(&isf_new_data);
	}
#endif
isf_data_rel:
	for (i=0; i<(SENSOR_CAPS_COUNT+ETH_REARCAM_CAPS_COUNT); i++){
		if (bPullData[i]){
			ImageUnit_ReleaseData(&isf_data[i]);
		}
	}

#endif
	return USERPROC_TRIG_PROC_NONE;
}

void MovieExe_EthCam_ChgDispCB(UINT32 DualCam)
{
	UINT32 DualCamChk=DUALCAM_BEHIND;//ETH_REARCAM_CAPS_COUNT==1, show behind sensor
#if((ETH_REARCAM_CAPS_COUNT+SENSOR_CAPS_COUNT)  >2)
	DualCamChk=DUALCAM_BOTH;//show four division
#endif
	DBG_IND("DispDataPort=%d, %d, %d\r\n", ImageApp_MovieMulti_GetDispDataPort(_CFG_ETHCAM_ID_1),ImageApp_MovieMulti_GetDispDataPort(_CFG_ETHCAM_ID_2),ImageApp_MovieMulti_GetDispDataPort(_CFG_REC_ID_1));

#if(ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
	DBG_DUMP("MovieExe_EthCam_ChgDispCB IsSocketGetData=%d, %d\r\n",socketCliEthData2_IsRecv(ETHCAM_PATH_ID_1),DualCam);
	if(socketCliEthData2_IsRecv(ETHCAM_PATH_ID_1)&& (DualCam==DualCamChk)){
#else
	DBG_DUMP("MovieExe_EthCam_ChgDispCB IsSocketGetData=%d, %d, %d\r\n",socketCliEthData1_IsRecv(ETHCAM_PATH_ID_1), socketCliEthData1_IsRecv(ETHCAM_PATH_ID_2),DualCam);
	if((socketCliEthData1_IsRecv(ETHCAM_PATH_ID_1) || socketCliEthData1_IsRecv(ETHCAM_PATH_ID_2))&& (DualCam==DualCamChk)){
#endif

#if(ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
	if(socketCliEthData1_IsRecv(ETHCAM_PATH_ID_1)==0){
		BKG_PostEvent(NVTEVT_BKW_ETHCAM_SYNC_TIME);
		//BKG_PostEvent(NVTEVT_BKW_GET_ETHCAM_TX_RECINFO);
		CHKPNT;
			BKG_PostEvent(NVTEVT_BKW_ETHCAM_SOCKETCLI_REC_DATA_OPEN_START);
	}
#endif

#if((ETH_REARCAM_CAPS_COUNT+SENSOR_CAPS_COUNT)  == 2)
		UI_SetData(FL_DUAL_CAM, DUALCAM_BEHIND);
		UI_SetData(FL_DUAL_CAM_MENU, DUALCAM_BEHIND);
		#if IMG_FULL_DISP
		#if(ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
			if(socketCliEthData2_IsRecv(ETHCAM_PATH_ID_1)){
				//CHKPNT; // here 
				MovieExe_EthCamSetISECrop(_CFG_ETHCAM_ID_1);
			}
		#else
			if(socketCliEthData1_IsRecv(ETHCAM_PATH_ID_1)){
				MovieExe_EthCamSetISECrop(_CFG_ETHCAM_ID_1);
			}
		#endif
		#endif
		#if(ETHCAM_DISP_MAX_QUEUE>0)
		if (timer_open(&g_EthCamDisp_TimerID, EthCamCmd_DispTimerCB) != E_OK) {
			DBG_ERR("open EthCamDisp timer fail\r\n");
		}
		timer_cfg(g_EthCamDisp_TimerID, 16600/*ms*/, TIMER_MODE_FREE_RUN | TIMER_MODE_ENABLE_INT, TIMER_STATE_PAUSE);
		#endif
#else//#elif(ETH_REARCAM_CAPS_COUNT >= 2)
		UI_SetData(FL_DUAL_CAM, DUALCAM_BOTH);
		UI_SetData(FL_DUAL_CAM_MENU, DUALCAM_BOTH);
		#if IMG_FULL_DISP
		if(socketCliEthData1_IsRecv(ETHCAM_PATH_ID_1)){
			MovieExe_EthCamSetISECrop(_CFG_ETHCAM_ID_1);
		}
		if(socketCliEthData1_IsRecv(ETHCAM_PATH_ID_2)){
			MovieExe_EthCamSetISECrop(_CFG_ETHCAM_ID_2);
		}
		#endif
#endif
		//Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_IMAGE_RATIO, 1, GetMovieSizeRatio(UI_GetData(FL_MOVIE_SIZE)));
		ImageUnit_Begin(&ISF_UserProc, 0);
		ImageUnit_SetParam(ImageApp_MovieMulti_GetDispDataPort(_CFG_REC_ID_1), USERPROC_PARAM_PUSH_CALLBACK_IMM, (UINT32)NULL);
		if(socketCliEthData1_IsRecv(ETHCAM_PATH_ID_1)
			#if(ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
			|| socketCliEthData2_IsRecv(ETHCAM_PATH_ID_1)
			#endif
			){
			#if(ETHCAM_DISP_MAX_QUEUE>0)
			g_EthCamDispQ.Queue=(ISF_DATA *)g_EthCamDispPoolAddr;
			memset((void*)g_EthCamDispPoolAddr,0,sizeof(ISF_DATA)*ETHCAM_DISP_MAX_QUEUE);
			MovieExe_EthCamDisp_ResetQ();
			timer_pausePlay(g_EthCamDisp_TimerID, TIMER_STATE_PLAY);
			#endif
			CHKPNT;
			ImageUnit_SetParam(ImageApp_MovieMulti_GetDispDataPort(_CFG_ETHCAM_ID_1), USERPROC_PARAM_PUSH_CALLBACK_IMM, (UINT32)MovieExe_EthCam_PIP_DispCB);
#if(ETH_REARCAM_CAPS_COUNT >= 2)
			ImageUnit_SetParam(ImageApp_MovieMulti_GetDispDataPort(_CFG_ETHCAM_ID_2), USERPROC_PARAM_PUSH_CALLBACK_IMM, (UINT32)NULL);
#endif
		}else{
#if(ETH_REARCAM_CAPS_COUNT >= 2)
			ImageUnit_SetParam(ImageApp_MovieMulti_GetDispDataPort(_CFG_ETHCAM_ID_1), USERPROC_PARAM_PUSH_CALLBACK_IMM, (UINT32)NULL);
			ImageUnit_SetParam(ImageApp_MovieMulti_GetDispDataPort(_CFG_ETHCAM_ID_2), USERPROC_PARAM_PUSH_CALLBACK_IMM, (UINT32)MovieExe_EthCam_PIP_DispCB);
#endif
		}
		ImageUnit_End();
	}else{
#if((ETH_REARCAM_CAPS_COUNT+SENSOR_CAPS_COUNT)  == 2)//#if(ETH_REARCAM_CAPS_COUNT == 1)
		UI_SetData(FL_DUAL_CAM, DUALCAM_FRONT);//default front sensor
		UI_SetData(FL_DUAL_CAM_MENU, DUALCAM_FRONT);

		#if (ETH_REARCAM_CAPS_COUNT==1 && ETHCAM_DISP_MAX_QUEUE)
		UINT32 j;
		if(g_EthCamDisp_TimerID){
			timer_close(g_EthCamDisp_TimerID);
			g_EthCamDisp_TimerID=0;
		}
		for (j=0; j<ETHCAM_DISP_MAX_QUEUE; j++){
			if(g_EthCamDispQ.Queue[j].Desc[0]){
				ImageUnit_ReleaseData(&g_EthCamDispQ.Queue[j]);
			}
		}
		#endif

#else//#elif(ETH_REARCAM_CAPS_COUNT >= 2)
		UI_SetData(FL_DUAL_CAM, DUALCAM_BOTH);//default four division
		UI_SetData(FL_DUAL_CAM_MENU, DUALCAM_BOTH);
#endif
		//Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_IMAGE_RATIO, 1, GetMovieSizeRatio(UI_GetData(FL_MOVIE_SIZE)));

		ImageUnit_Begin(&ISF_UserProc, 0);
		ImageUnit_SetParam(ImageApp_MovieMulti_GetDispDataPort(_CFG_ETHCAM_ID_1), USERPROC_PARAM_PUSH_CALLBACK_IMM, (UINT32)NULL);
#if(ETH_REARCAM_CAPS_COUNT >= 2)
		ImageUnit_SetParam(ImageApp_MovieMulti_GetDispDataPort(_CFG_ETHCAM_ID_2), USERPROC_PARAM_PUSH_CALLBACK_IMM, (UINT32)NULL);
#endif
		CHKPNT;
		ImageUnit_SetParam(ImageApp_MovieMulti_GetDispDataPort(_CFG_REC_ID_1), USERPROC_PARAM_PUSH_CALLBACK_IMM, (UINT32)MovieExe_EthCam_PIP_DispCB);
		ImageUnit_End();
	}
}
#endif
UINT32 MovieExe_GetCommonMemInitFinish(void)
{
	return gMovie_InitCommonMemFinish;
}

extern USIZE DispIPLSize;
extern URECT DispIPLWin;

static void MovieExe_ImageStreamOpen_MOVIE(void)
{
#if (USE_FILEDB==ENABLE)
	MOVIE_CFG_FDB_INFO fdb_info[2] = {0};  // Max 2 storage.
#endif
	UINT32 movie_size_idx, size_idx = 0;
	UINT32 movie_aspect_ratio_idx = 0;
	UINT32 i, rec_type;
	UINT32 mask, ipl_id = 0, setting_count = 0;
	UINT32 movie_rec_mask, clone_rec_mask;
	g_bAllowRawEnc=TRUE;

#if(defined(_NVT_ETHREARCAM_RX_) || defined(_NVT_ETHREARCAM_TX_))
	ImageApp_MovieMulti_SetParam(0, MOVIEMULTI_PARAM_DEBUG_MSG_EN,    TRUE);
#endif

	MovieExe_ImageStreamOpenMulti();

	movie_size_idx = UI_GetData(FL_MOVIE_SIZE);
	rec_type = MovieMapping_GetRecType(movie_size_idx);
	//movie_rec_mask = Movie_GetMovieRecMask();
	//clone_rec_mask = Movie_GetCloneRecMask();

	movie_rec_mask = SENSOR_ALL; //All ImagePipe must be initialized first, but they isn't running state
	clone_rec_mask = SENSOR_ALL;

	mask = 1;
	for (i = 0; i < SENSOR_CAPS_COUNT; i++)	{

		if (rec_type == MOVIE_REC_TYPE_FRONT || rec_type == MOVIE_REC_TYPE_CLONE) { // single recording
			if (movie_rec_mask & mask) { // active sensor, for movie recording
				ipl_id = 0; // get data from the mapping table of single recording, so IPL ID = 0
				size_idx = movie_size_idx;
			}
#if 0//CID 131892
			else { // inactive sensor, just for display
				ipl_id = 0; // get data from the mapping table of single recording, so IPL ID = 0
				if (rec_type == MOVIE_REC_TYPE_CLONE) {
					size_idx = MOVIE_SIZE_CLONE_1920x1080P30_848x480P30; // default, use the mapping table of FHD + clone
				} else {
					size_idx = MOVIE_SIZE_FRONT_1920x1080P30; // default, use the mapping table of single FHD
				}
			}
#endif
		} else if (rec_type == MOVIE_REC_TYPE_DUAL) { // dual recording
			if ((movie_rec_mask & mask) && (setting_count < 2)) { // active sensor, for movie recording
#if (SENSOR_CAPS_COUNT >=2)//CID 129679
				if (setting_count == 0) { // 1st active sensor
					ipl_id = 0; // get data from the mapping table of dual recording, 1st IPL ID = 0
				} else { // 2nd active sensor
					ipl_id = 1; // get data from the mapping table of dual recording, 2nd IPL ID = 1
				}
#else
				ipl_id = 0; // get data from the mapping table of dual recording, 1st IPL ID = 0
#endif
				size_idx = movie_size_idx;
				setting_count++;
			}
#if 0//CID 131892
			else { // inactive sensor, just for display
				ipl_id = 0; // get data from the mapping table of single recording, so IPL ID = 0
				size_idx = MOVIE_SIZE_FRONT_1920x1080P30; // default, use the mapping table of single FHD
			}
#endif
		}

		// set main movie
		gMovie_Rec_Info[i].size.w = MovieMapping_GetWidth(size_idx, ipl_id);
		gMovie_Rec_Info[i].size.h = MovieMapping_GetHeight(size_idx, ipl_id);
		gMovie_Rec_Info[i].raw_enc_size.w = MovieMapping_GetWidth(size_idx, ipl_id);
		gMovie_Rec_Info[i].raw_enc_size.h = MovieMapping_GetHeight(size_idx, ipl_id);
		gMovie_Rec_Info[i].frame_rate = MovieMapping_GetFrameRate(size_idx, ipl_id);
		gMovie_Rec_Info[i].target_bitrate = MovieMapping_GetTargetBitrate(size_idx, ipl_id);
		gMovie_Rec_Info[i].dar = MovieMapping_GetDispAspectRatio(size_idx, ipl_id);
		#if(defined(_NVT_ETHREARCAM_TX_))
		if(socketEthCmd_IsOpen()){
			gMovie_Rec_Info[i].codec = (sEthCamCodecSrctype.VCodec == MOVIE_CODEC_H265)?_CFG_CODEC_H265:_CFG_CODEC_H264;
		}else{
			gMovie_Rec_Info[i].codec = (UI_GetData(FL_MOVIE_CODEC) == MOVIE_CODEC_H265)?_CFG_CODEC_H265:_CFG_CODEC_H264;
		}
		#else
		gMovie_Rec_Info[i].codec = (UI_GetData(FL_MOVIE_CODEC) == MOVIE_CODEC_H265)?_CFG_CODEC_H265:_CFG_CODEC_H264;
		#endif
		//#NT#2018/02/14#KCHong -begin
	    //#NT#support sensor rotate setting
		gMovie_Rec_Info[i].sensor_rotate = (UI_GetData(FL_MOVIE_SENSOR_ROTATE) == SEN_ROTATE_ON) ? TRUE : FALSE;
		//#NT#2018/02/14#KCHong -end
		MovieMapping_GetAqInfo(size_idx, ipl_id, (UINT32)&gMovie_Rec_Info[i].aq_info);
		MovieMapping_GetCbrInfo(size_idx, ipl_id, (UINT32)&gMovie_Rec_Info[i].cbr_info);
		movie_aspect_ratio_idx = MovieMapping_GetImageRatio(size_idx, ipl_id);
		switch (movie_aspect_ratio_idx) {
		case IMAGERATIO_16_9:
			gMovie_Rec_Info[i].ratio.w = 16;
			gMovie_Rec_Info[i].ratio.h = 9;
			gMovie_Rec_Info[i].disp_ratio.w = 16;
			gMovie_Rec_Info[i].disp_ratio.h = 9;
			break;
		case IMAGERATIO_3_2:
			gMovie_Rec_Info[i].ratio.w = 3;
			gMovie_Rec_Info[i].ratio.h = 2;
			gMovie_Rec_Info[i].disp_ratio.w = 3;
			gMovie_Rec_Info[i].disp_ratio.h = 2;
			break;
		default:
			gMovie_Rec_Info[i].ratio.w = 4;
			gMovie_Rec_Info[i].ratio.h = 3;
			gMovie_Rec_Info[i].disp_ratio.w = 4;
			gMovie_Rec_Info[i].disp_ratio.h = 3;
			break;
		}

		#if IMG_FULL_DISP
		MovieExe_SetIMECrop(i);
		g_PipViewStyle = UI_GetData(FL_DUAL_CAM);
		#endif

#if MOVIE_DIS
    gMovie_Rec_Info[i].imagepipe_func |= IPL_FUNC_SIE_CH4_ETH;
#endif

#if SHDR_FUNC
if (UI_GetData(FL_MOVIE_HDR) == MOVIE_HDR_ON) {
    gMovie_Rec_Info[i].imagepipe_func |= IPL_FUNC_SHDR;
}else{
    gMovie_Rec_Info[i].imagepipe_func =  gMovie_Rec_Info[i].imagepipe_func & (~IPL_FUNC_SHDR);
}
#endif

#if WDR_FUNC
    gMovie_Rec_Info[i].imagepipe_func |= IPL_FUNC_WDR;
#endif

#if DEFOG_FUNC
    gMovie_Rec_Info[i].imagepipe_func |= IPL_FUNC_DEFOG;
#endif

#if IME_TMNR_FUNC
	gMovie_Rec_Info[i].imagepipe_func |= IPL_FUNC_IME_TMNR;
	gMovie_Rec_Info[i].imagepipe_func2 |= IMAGEPIPE_FUNC2_TMNRSHARE;
#endif

#if (_ADAS_FUNC_ == ENABLE)
    gMovie_Rec_Info[i].imagepipe_func |= IPL_FUNC_ADAS;
#endif  // #if (_ADAS_FUNC_ == ENABLE)

#if (VIDEOSPRITE_MOSAIC == ENABLE)
    gMovie_Rec_Info[i].imagepipe_func |= IPL_FUNC_PRIMACY_MASK;
#endif

#if (MOVIE_RSC == ENABLE)
#if !defined(_Gyro_None_)
	if ((UI_GetData(FL_MovieRSCIndex) == MOVIE_RSC_ON) &&
		(gMovie_Rec_Info[i].frame_rate <= 120)) {
		MovieExe_RSC_SetSwitch(SEL_RSC_CHANGEMODE, SEL_RSC_ON);
		gMovie_Rec_Info[i].imagepipe_func2 |= IMAGEPIPE_FUNC2_RSCBUF;
	} else {
		MovieExe_RSC_SetSwitch(SEL_RSC_CHANGEMODE, SEL_RSC_OFF);
		gMovie_Rec_Info[i].imagepipe_func2 &= ~IMAGEPIPE_FUNC2_RSCBUF;
	}
#endif
#endif
		// set clone movie
		if (rec_type == MOVIE_REC_TYPE_CLONE) {
			gMovie_Clone_Info[i].size.w = MovieMapping_GetCloneWidth(size_idx, ipl_id);
			gMovie_Clone_Info[i].size.h = MovieMapping_GetCloneHeight(size_idx, ipl_id);
			gMovie_Clone_Info[i].raw_enc_size.w = MovieMapping_GetCloneWidth(size_idx, ipl_id);
			gMovie_Clone_Info[i].raw_enc_size.h = MovieMapping_GetCloneHeight(size_idx, ipl_id);
			gMovie_Clone_Info[i].frame_rate = MovieMapping_GetCloneFrameRate(size_idx, ipl_id);
			gMovie_Clone_Info[i].target_bitrate = MovieMapping_GetCloneTargetBitrate(size_idx, ipl_id);
			gMovie_Clone_Info[i].dar = MovieMapping_GetCloneDispAspectRatio(size_idx, ipl_id);
			#if(defined(_NVT_ETHREARCAM_TX_))
			if(socketEthCmd_IsOpen()){
				gMovie_Clone_Info[i].codec = (sEthCamCodecSrctype.VCodec == MOVIE_CODEC_H265)?_CFG_CODEC_H265:_CFG_CODEC_H264;
			}else{
			gMovie_Clone_Info[i].codec = (UI_GetData(FL_MOVIE_CODEC) == MOVIE_CODEC_H265)?_CFG_CODEC_H265:_CFG_CODEC_H264;
			}
			#else
				gMovie_Clone_Info[i].codec = (UI_GetData(FL_MOVIE_CODEC) == MOVIE_CODEC_H265)?_CFG_CODEC_H265:_CFG_CODEC_H264;
			#endif
			MovieMapping_GetCloneAqInfo(size_idx, ipl_id, (UINT32)&gMovie_Clone_Info[i].aq_info);
			MovieMapping_GetCloneCbrInfo(size_idx, ipl_id, (UINT32)&gMovie_Clone_Info[i].cbr_info);
			movie_aspect_ratio_idx = MovieMapping_GetCloneImageRatio(size_idx, ipl_id);
			switch (movie_aspect_ratio_idx) {
			case IMAGERATIO_16_9:
				gMovie_Clone_Info[i].ratio.w = 16;
				gMovie_Clone_Info[i].ratio.h = 9;
				gMovie_Clone_Info[i].disp_ratio.w = 16;
				gMovie_Clone_Info[i].disp_ratio.h = 9;
				break;
			case IMAGERATIO_3_2:
				gMovie_Clone_Info[i].ratio.w = 3;
				gMovie_Clone_Info[i].ratio.h = 2;
				gMovie_Clone_Info[i].disp_ratio.w = 3;
				gMovie_Clone_Info[i].disp_ratio.h = 2;
				break;
			default:
				gMovie_Clone_Info[i].ratio.w = 4;
				gMovie_Clone_Info[i].ratio.h = 3;
				gMovie_Clone_Info[i].disp_ratio.w = 4;
				gMovie_Clone_Info[i].disp_ratio.h = 3;
				break;
			}
		}

		mask <<= 1;
	}

    // Display
    ImageApp_MovieMulti_SetParam(_CFG_DISP_ID_1, MOVIEMULTI_PARAM_DISP_DIR, SysVideo_GetDirbyID(DOUT1));
    //gMovie_Disp_Info.rotate_dir = SysVideo_GetDir();
    //ImageApp_MovieMulti_Config(MOVIE_CONFIG_DISP_INFO, (UINT32) &gMovie_Disp_Info);

    // Audio
    debug_msg("audio ch:%d\r\n",gMovie_Audio_Info.aud_ch);
    ImageApp_MovieMulti_Config(MOVIE_CONFIG_AUDIO_INFO, (UINT32) &gMovie_Audio_Info);

#if (USE_EXIF == ENABLE)
	// enable EXIF for image capture
	for (i = 0; i < SENSOR_CAPS_COUNT; i++) {
		ImageApp_MovieMulti_SetParam(gMovie_Rec_Info[i].rec_id, MOVIEMULTI_PARAM_IMGCAP_EXIF_EN, TRUE);
	}
#endif

	// Recording file
	USIZE sz = {1920, 1080};
    //#NT#2019/12/31# Philex Lin - begin
    for (i = 0; i < SENSOR_CAPS_COUNT; i++) {
        if (MovieMapping_GetWidth(size_idx, i)*MovieMapping_GetHeight(size_idx, i)>(sz.w*sz.h)) {
            sz.w = MovieMapping_GetWidth(size_idx, i);
            sz.w = MovieMapping_GetHeight(size_idx, i);
        }
    }
    //#NT#2019/12/31# Philex Lin - end
	ImageApp_MovieMulti_SetParam(0, MOVIEMULTI_PARAM_IMGCAP_MAXSIZE, (UINT32) &sz);
	for (i = 0; i < SENSOR_CAPS_COUNT; i++) {
		ImageApp_MovieMulti_Config(MOVIE_CONFIG_RECORD_INFO, (UINT32) &gMovie_Rec_Info[i]);
		ImageApp_MovieMulti_Config(MOVIE_CONFIG_RECORD_INFO, (UINT32) &gMovie_Clone_Info[i]);
		ImageApp_MovieMulti_Config(MOVIE_CONFIG_ALG_INFO, (UINT32) &(gMovie_Alg_Info[i]));
	}

	// Folder Naming
#if (USE_FILEDB==ENABLE)
	fdb_info[0].drive = 'A';
	fdb_info[0].p_root_path = MOVIE_ROOT_PATH;
	fdb_info[0].mem_range.Addr = dma_getCacheAddr(OS_GetMempoolAddr(POOL_ID_FILEDB));
	fdb_info[0].mem_range.Size = POOL_SIZE_FILEDB;
	ImageApp_MovieMulti_FMConfig(&fdb_info[0]);
	ImageApp_MovieMulti_FMSetSortBySN("_", 3, 6);


	fdb_info[1].drive = 'B';
	fdb_info[1].p_root_path = MOVIE_ROOT_PATH2;
	fdb_info[1].mem_range.Addr = fdb_info[0].mem_range.Addr + POOL_SIZE_FILEDB;
	fdb_info[1].mem_range.Size = POOL_SIZE_FILEDB;
	ImageApp_MovieMulti_FMConfig(&fdb_info[1]);
	ImageApp_MovieMulti_FMSetSortBySN("_", 3, 6);	
#endif
	for (i = 0; i < SENSOR_CAPS_COUNT; i++)	{
		ImageApp_MovieMulti_Root_Path(MOVIE_ROOT_PATH, gMovie_Folder_Naming[i].rec_id);
		ImageApp_MovieMulti_Folder_Naming((MOVIEMULTI_RECODE_FOLDER_NAMING*)&gMovie_Folder_Naming[i]);

		ImageApp_MovieMulti_Root_Path(MOVIE_ROOT_PATH, gMovie_Clone_Folder_Naming[i].rec_id);
		ImageApp_MovieMulti_Folder_Naming((MOVIEMULTI_RECODE_FOLDER_NAMING*)&gMovie_Clone_Folder_Naming[i]);
	}

	// EthCam
#if (defined(_NVT_ETHREARCAM_RX_))
	for (i = 0; i < ETH_REARCAM_CAPS_COUNT; i++)	{
		ImageApp_MovieMulti_Root_Path(MOVIE_ROOT_PATH, (_CFG_ETHCAM_ID_1 + i));
		ImageApp_MovieMulti_Folder_Naming((MOVIEMULTI_RECODE_FOLDER_NAMING*)&gMovie_EthCam_Folder_Naming[i]);
	}
#endif

	// File option
	ImageApp_MovieMulti_FileOption((MOVIE_RECODE_FILE_OPTION *) &gMovie_Rec_Option);


	//#NT#2018/04/02#IsiahChang -begin
	//#NT#To support Wi-Fi command 2025 BRC.
	// Streaming
    MovieMapping_GetStreamInfo(UI_GetData(FL_MOVIE_SIZE), (UINT32) &gMovie_Strm_Info);
    ImageApp_MovieMulti_Config(MOVIE_CONFIG_STREAM_INFO, (UINT32) &gMovie_Strm_Info);
	//#NT#2018/04/02#IsiahChang -end


#if 0
	static MOVIE_ALG_INFO ImgSetting = {
		// ipl 1 path 3
		GX_IMAGE_PIXEL_FMT_YUV420_PACKED,		// format
		{640, 480},								// image size
		{0, 0, 0, 0},							// window size, x/y/w/h
		FALSE,									// preview enable
		// ipl 1 path 4
		0,										// format
		{0, 0},									// image size
		{0, 0, 0, 0},							// window size, x/y/w/h
		FALSE,									// preview enable
	};
	ImageApp_Movie_SetParam(_CFG_REC_ID_1, MOVIE_PARAM_ALG_PARAM_IPL1_PATH3, (UINT32) &ImgSetting);
#endif


#if (YUV_TV_RANGE == ENABLE)
	ImageApp_Movie_SetParam(gMovie_Rec_Info[0].rec_id, MOVIE_PARAM_TV_RANGE, 1);
#endif

	#if (AUDIO_FUNC_AAC == ENABLE)
		MP_AUDENC_ENCODER *pObj = MP_AACEnc_getAudioEncodeObject();
		ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1, MOVIEMULTI_PARAM_AUD_ENCODER_OBJ, (UINT32)pObj);
	#else
		ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1, MOVIEMULTI_PARAM_AUD_ENCODER_OBJ, 0);
		DBG_ERR("aud codec = %d not support\r\n", gMovie_Rec_Info[0].aud_codec);
	#endif

    // Config timelapse time
	for (i = 0; i < SENSOR_CAPS_COUNT; i++) {
		if (gMovie_Rec_Info[i].rec_mode == _CFG_REC_TYPE_TIMELAPSE) {
			ImageApp_MovieMulti_SetParam(gMovie_Rec_Info[i].rec_id, MOVIEMULTI_PARAM_TIMELAPSE_TIME, MOVIE_TIMELAPSE_TIME);
			ImageApp_MovieMulti_SetParam(gMovie_Rec_Info[i].rec_id, MOVIEMULTI_PARAM_FILE_PLAYFRAMERATE, MOVIE_TIMELAPSE_PLAYFRAMERATE);
		}
    }

#if (SENSOR_CAPS_COUNT == 1)
	if (bMovieD2DModeEn) {
		ImageApp_MovieMulti_SetParam(gMovie_Rec_Info[0].rec_id, MOVIEMULTI_PARAM_D2D_MODE, TRUE);
	} else {
		ImageApp_MovieMulti_SetParam(gMovie_Rec_Info[0].rec_id, MOVIEMULTI_PARAM_D2D_MODE, FALSE);
	}
#else
#if (_BOARD_DRAM_SIZE_ == 0x04000000) //RAW+ CCIR
	ImageApp_MovieMulti_SetParam(gMovie_Rec_Info[0].rec_id, MOVIEMULTI_PARAM_D2D_MODE, FALSE);
	ImageApp_MovieMulti_SetParam(gMovie_Rec_Info[1].rec_id, MOVIEMULTI_PARAM_D2D_MODE, TRUE);
	ImageApp_MovieMulti_SetParam(gMovie_Rec_Info[1].rec_id, MOVIEMULTI_PARAM_RAW_BUF_NUM, 2);
#else
	ImageApp_MovieMulti_SetParam(gMovie_Rec_Info[0].rec_id, MOVIEMULTI_PARAM_D2D_MODE, TRUE);
	ImageApp_MovieMulti_SetParam(gMovie_Rec_Info[1].rec_id, MOVIEMULTI_PARAM_D2D_MODE, TRUE);
#endif
#endif

#if (((_BOARD_DRAM_SIZE_ == 0x04000000) || (MOVIE_YUV_COMPRESS == ENABLE)) && (!defined(_MODEL_ETHCAM_4K_FHD_RX_)))
	//Disable , Use Graphic movie stamp;
	//Enable , Use COE(VIDEOSPRITE_STAMP_FUNC) Movie Stamp
	#if defined(_NVT_ETHREARCAM_TX_)
	#if (ETHCAM_TX_DISPLAY_FLIP == ENABLE)
	ImageApp_MovieMulti_SetParam(gMovie_Rec_Info[0].rec_id, MOVIEMULTI_PARAM_YUV_COMPRESS, FALSE);
	#else
	ImageApp_MovieMulti_SetParam(gMovie_Rec_Info[0].rec_id, MOVIEMULTI_PARAM_YUV_COMPRESS, TRUE);
	#endif
	#else
	ImageApp_MovieMulti_SetParam(gMovie_Rec_Info[0].rec_id, MOVIEMULTI_PARAM_YUV_COMPRESS, TRUE);
	#endif
#if (SENSOR_CAPS_COUNT == 2) //CCIR Sensor, NO YUV Compress
	ImageApp_MovieMulti_SetParam(gMovie_Rec_Info[1].rec_id, MOVIEMULTI_PARAM_YUV_COMPRESS, FALSE);
	//panel is 960x180, sensor2 is 848x480
	ImageApp_MovieMulti_SetParam(_CFG_REC_ID_2, MOVIEMULTI_PARAM_IPL_FORCED_IMG_SIZE, MOVIE_IPL_SIZE_MAIN);
#endif
#else
	#if((defined(_NVT_ETHREARCAM_RX_) && !defined(_SENSOR1_CMOS_OFF_) && (MOVIE_UVAC_FUNC== DISABLE)) || ((IME_TMNR_FUNC == DISABLE) && (MOVIE_YUV_COMPRESS == ENABLE)))
	#if(_ANFA_ADAS_FUNC_ == ENABLE)
	ImageApp_MovieMulti_SetParam(gMovie_Rec_Info[0].rec_id, MOVIEMULTI_PARAM_YUV_COMPRESS, FALSE);
	#else
	ImageApp_MovieMulti_SetParam(gMovie_Rec_Info[0].rec_id, MOVIEMULTI_PARAM_YUV_COMPRESS, TRUE);
	#if (defined(_MODEL_ETHCAM_4K_FHD_RX_))
	ImageApp_MovieMulti_SetParam(gMovie_Rec_Info[0].rec_id, MOVIEMULTI_PARAM_REC_COMPRESS_ENABLE, TRUE);
	ImageApp_MovieMulti_SetParam(gMovie_Rec_Info[0].rec_id, MOVIEMULTI_PARAM_IMGCAP_ALLOC_SNAPSHOT_BUF, TRUE);
	#endif
	#endif
#endif
#endif
#if MOVIE_UVAC_FUNC
    // Uvac
	UVAC_VID_RESO_ARY uvacVidResoAry = {0};
	UVAC_AUD_SAMPLERATE_ARY uvacAudSampleRateAry = {0};
	UVAC_INFO    UvacInfo = {0};

	// UVC streaming size is the same as WiFi
	gUIUvacVidReso[0].width = gMovie_Strm_Info.size.w;
	gUIUvacVidReso[0].height = gMovie_Strm_Info.size.h;
	uvacVidResoAry.aryCnt = 1;
	uvacVidResoAry.pVidResAry = &gUIUvacVidReso[0];

	gUIUvacAudSampleRate[0] = gMovie_Audio_Info.aud_rate;
	uvacAudSampleRateAry.aryCnt = 1;
	uvacAudSampleRateAry.pAudSampleRateAry = &gUIUvacAudSampleRate[0];

	xUSBMakerInit_UVAC(&gUIMovUvacDevDesc);
	#if 0
	#if defined(_USB1_DEVICE_)
	int_setConfig(INT_CONFIG_ID_USB3_GIC_DESTINATION, CC_CORE_CA53_CORE1);
	UVAC_SetObject(UVAC_TYPE_U3);
	#elif defined(_USB2_DEVICE_)
	UVAC_SetObject(UVAC_TYPE_U2);
	#endif
	#endif

	UvacInfo.channel = UVAC_CHANNEL_1V1A;
	#if 0
	UvacInfo.fpStartVideoCB = xUvac_StartVideoCB;
	UvacInfo.fpStopVideoCB = xUvac_StopVideoCB;
	UvacInfo.fpSetVolCB = xUvac_SetVolumeCB;
	#endif
	ImageUnit_Begin(&ISF_UsbUVAC, 0);
		ImageUnit_SetParam(ISF_CTRL, UVAC_PARAM_VID_RESO_ARY, (UINT32)&uvacVidResoAry);
		ImageUnit_SetParam(ISF_CTRL, UVAC_PARAM_AUD_SAMPLERATE_ARY, (UINT32)&uvacAudSampleRateAry);
		ImageUnit_SetParam(ISF_CTRL, UVAC_PARAM_UVAC_VEND_DEV_DESC, (UINT32)&gUIMovUvacDevDesc);
		ImageUnit_SetParam(ISF_CTRL, UVAC_PARAM_VID_FMT_TYPE, IMGUNIT_UVAC_VIDEO_FORMAT_H264_ONLY);
		ImageUnit_SetParam(ISF_CTRL, UVAC_PARAM_SUSPEND_CLOSE_IMM, TRUE);
		ImageUnit_SetParam(ISF_CTRL, UVAC_PARAM_UVAC_INFO, (UINT32)&UvacInfo);
		#if(UVC_MULTIMEDIA_FUNC == ENABLE)
		ImageUnit_SetParam(ISF_CTRL, UVAC_PARAM_CDC_ENABLE, TRUE);
		ImageUnit_SetParam(ISF_CTRL, UVAC_PARAM_CDC_PSTN_REQUEST_CB, (UINT32)CdcPstnReqCB);
		#endif
	ImageUnit_End();
#endif

	// User Callback
	ImageApp_MovieMulti_RegUserCB(MovieExe_UserEventCb);

#if(defined(_NVT_ETHREARCAM_TX_))
#if defined(_MODEL_MIO_ETHCAM_TX_)
	MOVIEMULTI_IME_CROP_INFO CloneIMEInfo;
#if defined(_Disp_IFPA_LCD1_MD090TL01_)
	CloneIMEInfo.IMESize.w = 1920;
	CloneIMEInfo.IMESize.h = 1080;
	CloneIMEInfo.IMEWin.x = 0;		
	CloneIMEInfo.IMEWin.y = 300;
	CloneIMEInfo.IMEWin.w = 1280;//1920;
	CloneIMEInfo.IMEWin.h = 320;//384;
#endif
	
	ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1, MOVIEMULTI_PARAM_ETHCAM_IME_CROP, (UINT32)&CloneIMEInfo);
#endif

	#if (ETHCAM_TX_DISPLAY_FLIP == ENABLE)
	// display horizontal flip:
	// flip IPL then flip recording image, so display image will be flipped without spending additional time
	ImageUnit_Begin(ISF_IPL(0), 0);
	ImageUnit_SetVdoDirection(ISF_IN1, ISF_VDO_DIR_MIRRORX);
	ImageUnit_End();
	ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1, MOVIEMULTI_PARAM_IMG_MIRROR, ISF_VDO_DIR_MIRRORX);
	#endif

	ImageApp_MovieMulti_SetParam(_CFG_DISP_ID_1, MOVIEMULTI_PARAM_DISP_OFF, TRUE);
#if 0//(ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
	MOVIEMULTI_IPL_SIZE_INFO ipl_sz;
	ipl_sz.size.w = 1920;
	ipl_sz.size.h = 1080;
	ipl_sz.fps = 60;
	//set sensor mode p60 for display, fps is the first sorting
	ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1, MOVIEMULTI_PARAM_IPL_USER_IMG_SIZE, (UINT32)&ipl_sz);
	ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1, MOVIEMULTI_PARAM_IPL_FORCED_IMG_SIZE, MOVIE_IPL_SIZE_USER);

	ImageApp_MovieMulti_SetParam(_CFG_CLONE_ID_1, MOVIEMULTI_PARAM_ETHCAM_TX_BS_CB, (UINT32)MovieExe_EthCamCloneId1_CB);
#endif
	//Eth cam Sender data
	ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1, MOVIEMULTI_PARAM_ETHCAM_TX_BS_CB, (UINT32)MovieExe_EthCamRecId1_CB);
	ImageApp_MovieMulti_SetParam(_CFG_CLONE_ID_1, MOVIEMULTI_PARAM_ETHCAM_TX_BS_CB, (UINT32)MovieExe_EthCamCloneId1_CB);
#endif
#if(defined(_NVT_ETHREARCAM_RX_))
	//Use Project callback
	ImageApp_MovieMulti_SetParam(_CFG_ETHCAM_ID_1, MOVIEMULTI_RARAM_ETHCAM_RX_FUNC_EN, TRUE);
	ImageApp_MovieMulti_SetParam(_CFG_DISP_ID_1, MOVIEMULTI_PARAM_DISP_USE_PRJ_USERPROC_CB, TRUE);
#endif

	// Set audio encoder buffer here, the minimum requirement is (rollback sec + 3) * 1000;
	ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1, MOVIEMULTI_PARAM_AUD_ENCBUF_MS, 15000);

	ImageApp_MovieMulti_Open();
#if(defined(_NVT_ETHREARCAM_RX_))
	//For PIP VIEW
	UI_SetData(FL_DUAL_CAM, DUALCAM_FRONT);
CHKPNT;

	MovieExe_EthCam_ChgDispCB(UI_GetData(FL_DUAL_CAM));
#endif
	//#NT#2018/06/11#KCHong -begin
	//#NT#For MovieMulti Test
	#if (SENSOR_CAPS_COUNT == 2)
	if(System_GetEnableSensor() == (SENSOR_1|SENSOR_2)){
#if !defined(_SENSOR1_CMOS_OFF_)
	ImageApp_MovieMulti_ImgLinkForDisp(_CFG_REC_ID_1, ENABLE, TRUE);
#endif
#if !defined(_SENSOR2_CMOS_OFF_)
	ImageApp_MovieMulti_ImgLinkForDisp(_CFG_REC_ID_2, ENABLE, TRUE);
#endif
	}else if (System_GetEnableSensor() == SENSOR_1){
#if !defined(_SENSOR1_CMOS_OFF_)
		ImageApp_MovieMulti_ImgLinkForDisp(_CFG_REC_ID_1, ENABLE, TRUE);
#endif
#if !defined(_SENSOR2_CMOS_OFF_)
		ImageApp_MovieMulti_ImgLinkForDisp(_CFG_REC_ID_2, DISABLE, TRUE);
#endif
	}
	#elif (SENSOR_CAPS_COUNT == 4)
#if !defined(_SENSOR1_CMOS_OFF_)
	ImageApp_MovieMulti_ImgLinkForDisp(_CFG_REC_ID_1, ENABLE, TRUE);
#endif
#if !defined(_SENSOR2_CMOS_OFF_)
	ImageApp_MovieMulti_ImgLinkForDisp(_CFG_REC_ID_2, ENABLE, TRUE);
#endif
#if !defined(_SENSOR3_CMOS_OFF_)
	ImageApp_MovieMulti_ImgLinkForDisp(_CFG_REC_ID_3, ENABLE, TRUE);
#endif
#if !defined(_SENSOR4_CMOS_OFF_)
	ImageApp_MovieMulti_ImgLinkForDisp(_CFG_REC_ID_4, ENABLE, TRUE);
#endif
	#else
	#if defined(_NVT_ETHREARCAM_TX_)
	// do nothing
	#elif defined(_NVT_ETHREARCAM_RX_)
#if !defined(_SENSOR1_CMOS_OFF_)
	ImageApp_MovieMulti_ImgLinkForDisp(_CFG_REC_ID_1, ENABLE, TRUE);
#endif
	#else
#if !defined(_SENSOR1_CMOS_OFF_)
	ImageApp_MovieMulti_ImgLinkForDisp(_CFG_REC_ID_1, ENABLE, FALSE);
#endif
	#endif
	#endif
	ImageApp_MovieMulti_DispStart(_CFG_DISP_ID_1);
	//#NT#2018/06/11#KCHong -end

#if(defined(_NVT_ETHREARCAM_RX_))
	for (i = 0; i < ETH_REARCAM_CAPS_COUNT; i++) {
		if(socketCliEthData1_IsConn(i)==0){
			// set recode info
			MOVIEMULTI_ETHCAM_REC_INFO rec_info;
			rec_info.width = 1920;
			rec_info.height = 1080;
			rec_info.vfr = 30;
			rec_info.tbr = 1200 * 1024;
			rec_info.ar = _CFG_DAR_DEFAULT;
			rec_info.gop = 15;
			rec_info.codec = _CFG_CODEC_H264;
			rec_info.aud_codec = _CFG_AUD_CODEC_AAC;
			rec_info.rec_mode = _CFG_REC_TYPE_AV;
			rec_info.rec_format = _CFG_FILE_FORMAT_MOV;//cj 0526 //_CFG_FILE_FORMAT_MP4;
			ImageApp_MovieMulti_SetParam((_CFG_ETHCAM_ID_1 + i), MOVIEMULTI_PRARM_ETHCAM_REC_INFO, (UINT32)&rec_info);
		}
		#if (ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
		if(socketCliEthData2_IsConn(i)==0){
			// set decode info
			MOVIEMULTI_ETHCAM_DEC_INFO dec_info;
			dec_info.width = 1280;
			dec_info.height = 720;
			dec_info.gop = 15;
			dec_info.codec = _CFG_CODEC_H264;
			dec_info.header_size = sizeof(EthCamSPSDefaultAddr);
			dec_info.header = EthCamSPSDefaultAddr;
			ImageApp_MovieMulti_SetParam((_CFG_ETHCAM_ID_1 + i), MOVIEMULTI_PRARM_ETHCAM_DEC_INFO, (UINT32)&dec_info);
		}
		#else
		if(socketCliEthData1_IsConn(i)==0){
			// set decode info
			MOVIEMULTI_ETHCAM_DEC_INFO dec_info;
			dec_info.width = 1920;
			dec_info.height = 1080;
			dec_info.gop = 15;
			dec_info.codec = _CFG_CODEC_H264;
			dec_info.header_size = sizeof(EthCamSPSDefaultAddr);
			dec_info.header = EthCamSPSDefaultAddr;
			ImageApp_MovieMulti_SetParam((_CFG_ETHCAM_ID_1 + i), MOVIEMULTI_PRARM_ETHCAM_DEC_INFO, (UINT32)&dec_info);
		}
		#endif
		//ImageApp_MovieMulti_EthCamLinkForDisp(_CFG_ETHCAM_ID_1, ENABLE, TRUE);
	}
#endif

#if( (_BOARD_DRAM_SIZE_ > 0x04000000)||( defined(_NVT_ETHREARCAM_TX_) && AV_LCA_FUNC == ENABLE))

	UINT32 u32CurrSensorEn = System_GetEnableSensor();
#if defined(_SENSOR1_CMOS_OFF_)
	u32CurrSensorEn &= ~SENSOR_1;
#endif
#if defined(_SENSOR2_CMOS_OFF_)
	u32CurrSensorEn &= ~SENSOR_2;
#endif

	// For ALG function
	mask = 1;
	for (i = 0; i < SENSOR_CAPS_COUNT; i++) {
		if (gMovie_Alg_Info[_CFG_REC_ID_1 + i].path13.ImgSize.w && gMovie_Alg_Info[_CFG_REC_ID_1 + i].path13.ImgSize.h) {
			if (u32CurrSensorEn & mask) {
				ImageApp_MovieMulti_ImgLinkForAlg((_CFG_REC_ID_1 + i), _CFG_ALG_PATH3, ENABLE, TRUE);
			} else {
			    ImageApp_MovieMulti_ImgLinkForAlg((_CFG_REC_ID_1 + i), _CFG_ALG_PATH3, DISABLE, TRUE);
			}
		}
		if (gMovie_Alg_Info[_CFG_REC_ID_1 + i].path14.ImgSize.w && gMovie_Alg_Info[_CFG_REC_ID_1 + i].path14.ImgSize.h) {
			if (u32CurrSensorEn & mask) {
				ImageApp_MovieMulti_ImgLinkForAlg((_CFG_REC_ID_1 + i), _CFG_ALG_PATH4, ENABLE, TRUE);
			} else {
			    ImageApp_MovieMulti_ImgLinkForAlg((_CFG_REC_ID_1 + i), _CFG_ALG_PATH4, DISABLE, TRUE);
			}
		}
		mask <<= 1;
	}
#endif
	// set video codec for main movie and clone movie
	mask = 1;
	for (i = 0; i < SENSOR_CAPS_COUNT; i++) {
		if (movie_rec_mask & mask) {
			MovieExe_SetVdoCodecByRecID(gMovie_Rec_Info[i].rec_id, gMovie_Rec_Info[i].codec);
		}

		if (clone_rec_mask & mask) {
			MovieExe_SetVdoCodecByRecID(gMovie_Clone_Info[i].rec_id, gMovie_Clone_Info[i].codec);
		}

		mask <<= 1;
	}

	// set video codec for streaming
	MovieExe_SetVdoCodecByRecID(gMovie_Strm_Info.strm_id, gMovie_Strm_Info.codec);

	if (gMovie_Strm_Info.codec == _CFG_CODEC_MJPG) {
		PhotoExe_InitNetHttp();
	}

    if (UI_GetData(FL_MovieDisIndex) == MOVIE_DIS_ON) {
        // enable DIS
		MovieExe_IPL_SetIInfo(IPL_SEL_DISCOMP_IMM, SEL_DISCOMP_ON);
    } else {
        // disble DIS
		MovieExe_IPL_SetIInfo(IPL_SEL_DISCOMP_IMM, SEL_DISCOMP_OFF);
    }

    //CARDV_680-582
    Ux_SendEvent(&CustomMovieObjCtrl,   NVTEVT_EXE_MOVIE_WDR,           1,  SysGetFlag(FL_MOVIE_WDR));

    Ux_SendEvent(&CustomMovieObjCtrl,   NVTEVT_EXE_SHDR,                1,  UI_GetData(FL_MOVIE_HDR));

#if(WIFI_AP_FUNC==ENABLE)
    if (System_GetState(SYS_STATE_CURRSUBMODE) == SYS_SUBMODE_WIFI) {
    	Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_MOVIE_STRM_START, 1,gMovie_Strm_Info.strm_id);
        Ux_PostEvent(NVTEVT_WIFI_EXE_MODE_DONE,1,E_OK);
    }
#endif

#if MOVIE_UVAC_FUNC
	Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_MOVIE_UVAC_START, 0);
#endif

#if(defined(_NVT_ETHREARCAM_RX_))
	EthCamNet_EthHubChkPortReady();
	for (i=0; i<ETH_REARCAM_CAPS_COUNT; i++){
		if(EthCamNet_GetEthLinkStatus(i)==ETHCAM_LINK_UP && socketCliEthCmd_IsConn(i)==0){
			EthCamSocketCli_ReConnect(i, 0 ,0);
		}
	}
	for (i=0; i<ETH_REARCAM_CAPS_COUNT; i++){
		if(EthCamNet_GetEthLinkStatus(i)==ETHCAM_LINK_UP){
			if(socketCliEthCmd_IsConn(i)==0){
				//here open ethcam's cmd and data communication
				BKG_PostEvent(NVTEVT_BKW_ETHCAM_SOCKETCLI_CMD_OPEN);
			}
			if(SxTimer_GetFuncActive(SX_TIMER_ETHCAM_LINKDET_ID)==0){
				SxTimer_SetFuncActive(SX_TIMER_ETHCAM_LINKDET_ID, TRUE);
				EthCamHB1[i] = 0;
				EthCamHB2 = 0;
			}
		}
	}
#endif
#if(defined(_NVT_ETHREARCAM_TX_))
	if(EthCamNet_GetEthLinkStatus(ETHCAM_PATH_ID_1)==ETHCAM_LINK_UP && socketEthCmd_IsOpen()==0){
		BKG_PostEvent(NVTEVT_BKW_ETHCAM_SOCKET_OPEN);
	}
#endif
}

static void MovieExe_InitVideosprite(void){
	static int init = 0;

	if(init)
		return;

	videosprite_open();

#if (VIDEOSPRITEIPC != ENABLE)
	if(videosprite_set_bookkeep_buf(OS_GetMempoolAddr(POOL_ID_VDS_BK), POOL_SIZE_VDS_BK)){
		DBG_ERR("Fail to set videosprite's bookkeep buffer\r\n");
		return ;
	}
#endif

	init = 1;
}

#if (VIDEOSPRITEIPC==ENABLE)
static void MovieExe_InitVideoSpriteIpc(void){
	static int init = 0;
	if(!init)
		VideoSpriteIpc_Open();

	init = 1;
}
#endif

#if (LOG_DBGINFO_IN_GPS_SECTION == ENABLE)
static NVTMPP_VB_POOL mp4log_pool = 0;

static void MovieExe_InitMp4DbgLog(void)
{
	NVTMPP_VB_BLK  blk;
	UINT32         blk_size = ALIGN_CEIL_64(1024*100*2);    // Max GPS buffer 100K * 2

	mp4log_Open();
	alg_mp4log_AddSection();

    // Add debug log sub-section here
    // Ex: mp4log_AddSection("TEST", 1, &DbgLogTestID);

	if ((blk_size = mp4log_GetTotalLogSize()) != 0) {
		blk_size = ALIGN_CEIL_64(blk_size * 1024 * 2);

		mp4log_pool = nvtmpp_vb_create_pool("MP4Log", blk_size, 1, NVTMPP_DDR_1);
		if (NVTMPP_VB_INVALID_POOL == mp4log_pool)	{
			DBG_ERR("create private pool err\r\n");
			return;
		}
		blk = nvtmpp_vb_get_block(0, mp4log_pool, blk_size, NVTMPP_DDR_1);
		if (NVTMPP_VB_INVALID_BLK == blk) {
			DBG_ERR("get vb block err\r\n");
			return;
		}
		mp4log_SetMem(nvtmpp_vb_block2addr(blk), blk_size);
	}
}

static void MovieExe_UnInitMp4DbgLog(void)
{
	NVTMPP_ER ret;

	mp4log_Close();

	//destroy mempool
	ret = nvtmpp_vb_destroy_pool(mp4log_pool);
	if (ret != NVTMPP_ER_OK) {
		DBG_ERR("destory mp4log pool fail (ret = %d)\r\n", ret);
	}
}
#endif  // #if (LOG_DBGINFO_IN_GPS_SECTION == ENABLE)
static BOOL bRecoveryOneTime= TRUE;
extern void SetBLStatus(BOOL status );
UINT32 CustomFileNameCurCount=0;
#define MAX_FILE_SN	999999
BOOL File_Repair_Finish = FALSE;
void GetLastFile_SN(void)
{
	FILEDB_HANDLE  fileDbHandle;
	FILEDB_INIT_OBJ   FilDBInitObj={0};
	PFILEDB_FILE_ATTR  pFileAttr;
	INT32 FileNum;
	INT32 FileSN;
	CHAR	rootPath[20]="A:\\Novatek\\";

	FilDBInitObj.rootPath = rootPath;
	FilDBInitObj.bIsRecursive = TRUE;
	FilDBInitObj.bIsCyclic = TRUE;
	FilDBInitObj.bIsMoveToLastFile = TRUE;
	FilDBInitObj.bIsSupportLongName = TRUE;
	FilDBInitObj.bIsDCFFileOnly = FALSE;
	FilDBInitObj.bIsChkHasFile = FALSE;
	FilDBInitObj.u32MaxFilePathLen = 60;
	FilDBInitObj.u32MaxFileNum = 20000;
	FilDBInitObj.fileFilter = FILEDB_FMT_MOV|FILEDB_FMT_JPG;
	FilDBInitObj.u32MemAddr = OS_GetMempoolAddr(POOL_ID_APP);
	FilDBInitObj.u32MemSize = POOL_SIZE_APP;
	FilDBInitObj.SortSN_Delim= "_";
	FilDBInitObj.SortSN_DelimCount=3;
	FilDBInitObj.SortSN_CharNumOfSN=6;
	fileDbHandle = FileDB_Create(&FilDBInitObj);
	FileDB_SortBy(fileDbHandle,FILEDB_SORT_BY_SN,FALSE);
	FileNum = FileDB_GetTotalFileNum(fileDbHandle);

	if(FileNum == 0)
	{
		CustomFileNameCurCount=0;
	}
	else
	{
		pFileAttr = FileDB_CurrFile(fileDbHandle);
		FileSN = FileDB_GetSNByName(fileDbHandle,pFileAttr->filename);
		CustomFileNameCurCount = FileSN;
	}
	g_FileSerialNum = CustomFileNameCurCount;
	FileDB_Release(fileDbHandle);

	debug_msg("\r\n===CustomFileNameCurCount:%d %d ===\r\n",CustomFileNameCurCount,g_FileSerialNum);
	if(CustomFileNameCurCount>=MAX_FILE_SN)
	{
		Ux_PostEvent(NVTEVT_CB_MOVIE_FULL, 0);

		//g_CardFull=TRUE;
	}
}

void CheckFileRecovery(void)
{
	//System_WaitFS();
	FST_FILE    pFile;
	UINT32      uiReadSize = 64;//BURNIN_SETTING_SIZE;
	CHAR        cDataBuf[64] ;
	CHAR      *pCh= NULL;
	UINT32      uiMp4totalSrc = 0;
	static BOOL bNeedRepair1 =FALSE;
	static BOOL bNeedRepair2 =FALSE;

	FILEDB_HANDLE  fileDbHandle;
	FILEDB_INIT_OBJ   FilDBInitObj={0};
	PFILEDB_FILE_ATTR  pFileAttr;
	INT32 FileNum;
	CHAR              rootPath[20]="A:\\Novatek\\";

	CHAR    chaFullName[64] = { 0 };
	CHAR    chaFullName2[64] = { 0 };
	INT32  FileSN;
	//CHKPNT;
	FilDBInitObj.rootPath = rootPath;
	FilDBInitObj.bIsRecursive = TRUE;
	FilDBInitObj.bIsCyclic = TRUE;
	FilDBInitObj.bIsMoveToLastFile = TRUE;
	FilDBInitObj.bIsSupportLongName = TRUE;
	FilDBInitObj.bIsDCFFileOnly = FALSE;
	FilDBInitObj.bIsChkHasFile = FALSE;
	FilDBInitObj.u32MaxFilePathLen = 60;
	FilDBInitObj.u32MaxFileNum = 20000;
	FilDBInitObj.fileFilter = FILEDB_FMT_MOV;
	FilDBInitObj.u32MemAddr = OS_GetMempoolAddr(POOL_ID_APP);
	FilDBInitObj.u32MemSize = POOL_SIZE_APP;
	FilDBInitObj.SortSN_Delim= "_";
	FilDBInitObj.SortSN_DelimCount=3;
	FilDBInitObj.SortSN_CharNumOfSN=6;
	fileDbHandle = FileDB_Create(&FilDBInitObj);
	//CHKPNT;
	FileDB_SortBy(fileDbHandle,FILEDB_SORT_BY_SN,FALSE);
	FileNum = FileDB_GetTotalFileNum(fileDbHandle);
	
	//FileSN = FileDB_GetSNByName(fileDbHandle,pFileAttr->filename);
		//debug_msg(" --cj FileSN: %d \r\n",FileSN);
	
	if(FileNum)
	{
		debug_msg(" --cj  FileNum:%d \r\n",FileNum);
		pFileAttr = FileDB_SearhFile(fileDbHandle,FileNum-1);
		strcpy(chaFullName, pFileAttr->filePath);
		debug_msg("FileNum-1 :pFileAttr->%s\r\n",pFileAttr->filePath);
#if 1
		pFile = FileSys_OpenFile(pFileAttr->filePath, FST_OPEN_READ);
		if (pFile)
		{
			//CHKPNT;
			FileSys_SeekFile(pFile,40,  FST_SEEK_SET);
			FileSys_ReadFile(pFile, (UINT8 *)cDataBuf, &uiReadSize, 0, NULL);
			FileSys_CloseFile(pFile);

			pCh = strstr(cDataBuf, "tima");	//BURNIN_TOTALSRC_STRING
			debug_msg("Recovery 1 ----cDataBuf[4]=%x- %x\r\n",cDataBuf[4], *(pCh+4));  
			if(cDataBuf[4] != 0) 
			{
				//debug_msg("Recovery 1 check file tima->%x \r\n",*(pCh+4) );//uiMp4totalSrc
					bNeedRepair1 = TRUE;
				debug_msg("Recovery 1 check file tima->%x %d \r\n",*(pCh+4), bNeedRepair1 );
			}
		}
#endif
		if (FileNum>1)
		{
			//CHKPNT;
			pFileAttr = FileDB_SearhFile(fileDbHandle,FileNum-2);
			strcpy(chaFullName2, pFileAttr->filePath);
#if 1
			pFile = FileSys_OpenFile(pFileAttr->filePath, FST_OPEN_READ);
			debug_msg("FileNum-2 :pFileAttr->%s\r\n",pFileAttr->filePath);

			if (pFile)
			{
				FileSys_SeekFile(pFile,40,  FST_SEEK_SET);
				FileSys_ReadFile(pFile, (UINT8 *)cDataBuf, &uiReadSize, 0, NULL);
				FileSys_CloseFile(pFile);
   
				pCh = strstr(cDataBuf, "tima");	//BURNIN_TOTALSRC_STRING
				debug_msg("Recovery 2 ----cDataBuf[4]=%x- %x\r\n",cDataBuf[4], *(pCh+4));
				if(cDataBuf[4] != 0) 
				{
					bNeedRepair2 = TRUE;
				}
			
			}
#endif
		}
	}
	FileDB_Refresh(fileDbHandle);
		FileSys_WaitFinish();
		
	FileDB_Release(fileDbHandle);
	//CHKPNT;
	//
	if((FileNum))//&&(bNeedRepair1)
	{
		
		//CHKPNT;
		if ((bNeedRepair1))
		{
			debug_msg("Recovery 1 =%s\r\n",chaFullName);
			SMediaPlay_FileRecovery(chaFullName, OS_GetMempoolAddr(POOL_ID_APP), POOL_SIZE_APP);
			File_Repair_Finish = TRUE;
		}
		if((FileNum>1)&&(bNeedRepair2))
		{
			debug_msg("Recovery 2 =%s\r\n",chaFullName2);
			//CHKPNT;
			SMediaPlay_FileRecovery(chaFullName2, OS_GetMempoolAddr(POOL_ID_APP), POOL_SIZE_APP);
			bNeedRepair2=FALSE;
		}
		bNeedRepair1 = FALSE;
	}
	//CHKPNT;
}
#include "DxDisplay.h"
extern BOOL bGetTxFirmware;
INT32 MovieExe_OnOpen(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
CHKPNT;
	//call default
	
	if(bflag_EthLinkFinish == FALSE ){
		CHKPNT;
		SetEthPowerOn();
	}
	Ux_DefaultEvent(pCtrl, NVTEVT_EXE_OPEN, paramNum, paramArray);

	SxCmd_AddTable(uimovie);

	#if 1
	//SetBLStatus(TRUE);
	//GxVideo_SetDeviceCtrl(DOUT1, DISPLAY_DEVCTRL_BACKLIGHT, DRVDISP_BACKLIGHT_ON);
	System_WaitFS();
	//debug_msg(" --cj:SYS_STATE_CARD:%d \r\n",System_GetState(SYS_STATE_CARD));
	if((System_GetState(SYS_STATE_CARD)  == CARD_INSERTED))
	{
	//CHKPNT;
		if(bRecoveryOneTime == TRUE)
		{
			bRecoveryOneTime = FALSE;
			System_WaitFS();
			GetLastFile_SN();
			CheckFileRecovery();
		}
	}
	#endif

	if (MovieExe_InitCommonMem() < 0) {
		return NVTEVT_CONSUME;
	}
#if (DUALCAM_PIP_BEHIND_FLIP == ENABLE)
    //allocate buffer for DUALCAM_PIP_BEHIND_FLIP function to rotate behind video.
    {
        UINT32 uiAddr = 0, uiSize = 0;
        //ISIZE  devSize = GxVideo_GetDeviceSize(DOUT1);

        // WiFi output size is 848*480.
        //uiSize = (848 * 480 * 3 / 2) + 1024;
        uiSize = (gMovie_Strm_Info.size.w * gMovie_Strm_Info.size.h * 3 / 2) + 1024;
        uiAddr = PipView_BFLIP_GetBufAddr(uiSize);
        PipView_BFLIP_SetBuffer(uiAddr, uiSize);
    }
#endif
#if USE_FILEDB
	UI_SetData(FL_IsUseFileDB, 1);
#else
	UI_SetData(FL_IsUseFileDB, 0);
#endif

    MovieExe_InitPhotoFunc();

	MovieExe_InitVideosprite();

	#if (VIDEOSPRITEIPC==ENABLE)
	MovieExe_InitVideoSpriteIpc();
	#endif

	#if (LOG_DBGINFO_IN_GPS_SECTION == ENABLE)
	MovieExe_InitMp4DbgLog();
	#endif  // #if (LOG_DBGINFO_IN_GPS_SECTION == ENABLE)

	Movie_RegCB();

#if defined(_ASR_ENABLE_)
	MOVIEMULTI_MAX_LINK_INFO MaxLinkInfo = {SENSOR_CAPS_COUNT, 1, 1, 1, ETH_REARCAM_CAPS_COUNT};   //Img, Disp, Wifi, AudCap, EthCam
	ImageApp_MovieMulti_Config(MOVIE_CONFIG_MAX_LINK_INFO, (UINT32) &MaxLinkInfo);
	ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1, MOVIEMULTI_PARAM_AUD_CAP_FUNC_EN, TRUE);
	ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1, MOVIEMULTI_PARAM_AUD_CAP_REG_CB, (UINT32)ASR_UserProc_PushCB);
	ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1, MOVIEMULTI_PARAM_AUD_MUTE_ENC_FUNC_EN, TRUE);
#elif (defined(_NVT_ETHREARCAM_TX_))
	MOVIEMULTI_MAX_LINK_INFO MaxLinkInfo = {SENSOR_CAPS_COUNT, 1, 1, 1, ETH_REARCAM_CAPS_COUNT};   //Img, Disp, Wifi, AudCap, EthCam
	ImageApp_MovieMulti_Config(MOVIE_CONFIG_MAX_LINK_INFO, (UINT32) &MaxLinkInfo);
	ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1, MOVIEMULTI_PARAM_AUD_CAP_FUNC_EN, TRUE);
	ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1, MOVIEMULTI_PARAM_AUD_CAP_REG_CB, (UINT32)MovieExe_EthCamTxRecId1AudCap_CB);
	ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1, MOVIEMULTI_PARAM_AUD_CAP_OUTPUT_FMT, AUDENC_OUTPUT_COMPRESSION);
#elif (AUDPROC_FUNC == ENABLE)
	MOVIEMULTI_MAX_LINK_INFO MaxLinkInfo = {SENSOR_CAPS_COUNT, 1, 1, 1, ETH_REARCAM_CAPS_COUNT};   //Img, Disp, Wifi, AudCap, EthCam
	ImageApp_MovieMulti_Config(MOVIE_CONFIG_MAX_LINK_INFO, (UINT32) &MaxLinkInfo);
	//AudProc_Init();
	BKG_PostEvent(NVTEVT_BKW_AUDPROC_INIT);// must be BKG,or confilct with Tx FW update
	ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1, MOVIEMULTI_PARAM_AUD_CAP_FUNC_EN, TRUE);
	ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1, MOVIEMULTI_PARAM_AUD_CAP_REG_CB, (UINT32)AudProc_UserProc_PushCB);
	ImageApp_MovieMulti_SetParam(_CFG_REC_ID_1, MOVIEMULTI_PARAM_AUD_MUTE_ENC_FUNC_EN, TRUE);
#endif
	MovieExe_ImageStreamOpen_MOVIE();
#if defined(_ASR_ENABLE_)
	ImageApp_MovieMulti_AudCapStart(_CFG_REC_ID_1);
#elif (AUDPROC_FUNC == ENABLE)
	ImageApp_MovieMulti_AudCapStart(_CFG_REC_ID_1);
#endif

#if (FS_FUNC == ENABLE)
	System_WaitFS();
	// set reserved size
	Movie_SetResvSize();
#endif
#if USE_FILEDB
	ImageApp_MovieMulti_FMOpen('A');
    #if defined(_EMBMEM_EMMC_) && (FS_MULTI_STRG_FUNC==ENABLE)
	ImageApp_MovieMulti_FMOpen('B');
	#endif
	{
		// sample code to get file serial number; assume the FileDB handle created by ImageApp is 0
		PFILEDB_FILE_ATTR  pFileAttr;

		if (FileDB_GetTotalFileNum(0)) {
			pFileAttr = FileDB_CurrFile(0);
			g_FileSerialNum = FileDB_GetSNByName(0, pFileAttr->filename);
		//	debug_msg(" --cj 88 g_FileSerialNum: %d",g_FileSerialNum);
		} else {
			g_FileSerialNum = 0;
		}

        #if defined(_EMBMEM_EMMC_) && (FS_MULTI_STRG_FUNC==ENABLE)
		if (FileDB_GetTotalFileNum(1)) {
            UINT32 uiFileSN = 0;
			pFileAttr = FileDB_CurrFile(1);
			uiFileSN = FileDB_GetSNByName(1, pFileAttr->filename);
            if(uiFileSN > g_FileSerialNum) {
                g_FileSerialNum = uiFileSN;
            }
		}
        #endif
	}
#endif

	//update display coordinate
	MovieExe_CalcFDDispCord(&gMovieFdDispCord);
	MovieExe_CalcOSDDispCord(&gMovieOsdDispCord);
	System_IPCSendReady();
	return NVTEVT_CONSUME;
}
extern BOOL EthDisconRebooting;
INT32 MovieExe_OnClose(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
//	UIPhotoFunc_Close();

#if(defined(_NVT_ETHREARCAM_RX_))
	UINT32 j;
#if (ETH_REARCAM_CAPS_COUNT==1 && ETHCAM_DISP_MAX_QUEUE)
	if(g_EthCamDisp_TimerID){
		timer_close(g_EthCamDisp_TimerID);
		g_EthCamDisp_TimerID=0;
	}
	for (j=0; j<ETHCAM_DISP_MAX_QUEUE; j++){
		if(g_EthCamDispQ.Queue[j].Desc[0]){
			ImageUnit_ReleaseData(&g_EthCamDispQ.Queue[j]);
		}
	}
#endif
	for (j=0; j<ETH_REARCAM_CAPS_COUNT; j++){
		if(ImageApp_MovieMulti_IsStreamRunning(_CFG_ETHCAM_ID_1+ j)){
			ImageApp_MovieMulti_EthCamRxRecStop(_CFG_ETHCAM_ID_1+ j);
		}

		if (System_GetState(SYS_STATE_NEXTSUBMODE) == SYS_SUBMODE_UPDFW
			&& System_GetState(SYS_STATE_CURRSUBMODE)  == SYS_SUBMODE_UPDFW) {
			EthCamCmd_GetFrameTimerEn(0);
			EthCamSocketCli_ReConnect(j, 0, 0);
			EthCamSocketCli_Close(j, ETHSOCKIPCCLI_ID_0);
			EthCamSocketCli_Close(j, ETHSOCKIPCCLI_ID_1);
			socketCliEthData_DestroyAllBuff(j);
			break;
		}else if(EthCamNet_GetEthLinkStatus(j)==ETHCAM_LINK_DOWN){
			socketCliEthData_DestroyAllBuff(j);
			continue;
		}else{
			EthCamCmd_GetFrameTimerEn(0);
			ImageApp_MovieMulti_EthCamLinkForDisp((_CFG_ETHCAM_ID_1 + j), DISABLE, TRUE);
			if(socketCliEthData1_IsRecv(j)
				#if (ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
				&& socketCliEthData2_IsRecv(j)
				#endif
			){
				EthCam_SendXMLCmd(j, ETHCAM_PORT_DEFAULT ,ETHCAM_CMD_TX_STREAM_STOP, 2);
			}else{
			if(socketCliEthData1_IsRecv(j)){
				EthCam_SendXMLCmd(j, ETHCAM_PORT_DATA1 ,ETHCAM_CMD_TX_STREAM_STOP, 0);
			}
#if (ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
			if(socketCliEthData2_IsRecv(j)){
				EthCam_SendXMLCmd(j, ETHCAM_PORT_DATA2 ,ETHCAM_CMD_TX_STREAM_STOP, 0);
			}
#endif
			}
			if(System_GetShutdownBegin()){
				EthCam_SendXMLCmd(j, ETHCAM_PORT_DEFAULT ,ETHCAM_CMD_TX_POWEROFF, 0);
			}else{
				EthCam_SendXMLCmd(j, ETHCAM_PORT_DEFAULT ,ETHCAM_CMD_CHANGE_MODE, 0);
			}
			EthCamCmd_Done(j, 0xFFFFFFFF, ETHCAM_CMD_TERMINATE);

			socketCliEthData1_SetRecv(j, 0);
			socketCliEthData2_SetRecv(j, 0);

			EthCamSocketCli_ReConnect(j, 0, 0);
			EthCamSocketCli_Close(j, ETHSOCKIPCCLI_ID_0);
			EthCamSocketCli_Close(j, ETHSOCKIPCCLI_ID_1);
			EthCamSocketCli_Close(j, ETHSOCKIPCCLI_ID_2);
			socketCliEthData_DestroyAllBuff(j);
			//for next background Tsk link
			EthCamNet_SetPrevEthLinkStatus(j, ETHCAM_LINK_DOWN);
		}
		EthCamHB1[j] = 0;
	}
	SxTimer_SetFuncActive(SX_TIMER_ETHCAM_LINKDET_ID, FALSE);
	EthCamHB2 = 0;

	if((bflag_EthLinkFinish == FALSE && bGetTxFirmware == FALSE && EthDisconRebooting)){
		CHKPNT;
		SetEthPowerOff();
	}
	
#endif
#if(defined(_NVT_ETHREARCAM_TX_))
	EthCamTxHB = 0;
	SxTimer_SetFuncActive(SX_TIMER_ETHCAM_LINKDET_ID, FALSE);
	if (System_GetState(SYS_STATE_NEXTSUBMODE) == SYS_SUBMODE_UPDFW
		&& System_GetState(SYS_STATE_CURRSUBMODE)  == SYS_SUBMODE_UPDFW) {
		#if (ETH_REARCAM_CAPS_COUNT>=2)
		MovieExe_EthCamRecId1_DestroyBsMapTblBuff();
		MovieExe_EthCamCloneId1_DestroyBsMapTblBuff();
		#endif
		socketEthData_DestroyAllBuff();
	}else if(EthCamNet_GetEthLinkStatus(ETHCAM_PATH_ID_1)==ETHCAM_LINK_DOWN){
	}else{
		if(ImageApp_MovieMulti_IsStreamRunning(_CFG_REC_ID_1 | ETHCAM_TX_MAGIC_KEY)){
			MovieExe_EthCamTxStop(_CFG_REC_ID_1);
		}
		if(ImageApp_MovieMulti_IsStreamRunning(_CFG_CLONE_ID_1 | ETHCAM_TX_MAGIC_KEY)){
			MovieExe_EthCamTxStop(_CFG_CLONE_ID_1);
		}
		//EthCamSocket_Close();
		socketEth_Close();
		#if (ETH_REARCAM_CAPS_COUNT>=2)
		MovieExe_EthCamRecId1_DestroyBsMapTblBuff();
		MovieExe_EthCamCloneId1_DestroyBsMapTblBuff();
		#endif
		socketEthData_DestroyAllBuff();
	}
#endif
//	Delay_DelayMs(1000);
    UIPhotoFunc_Close();

	//MovieExe_FileDB_Release();
#if(WIFI_AP_FUNC==ENABLE)

    if (System_GetState(SYS_STATE_CURRSUBMODE) == SYS_SUBMODE_WIFI) {
    	Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_MOVIE_STRM_STOP, 1,gMovie_Strm_Info.strm_id);
    }
	
#endif

    MovieStamp_Disable();
	MovieStamp_DestroyBuff();

#if MOVIE_UVAC_FUNC
	#if 1 // if you just want to suspend UVAC, please change to #if 0
		ImageUnit_Begin(&ISF_UsbUVAC, 0);
		ImageUnit_SetParam(ISF_CTRL, UVAC_PARAM_SUSPEND_CLOSE_IMM, FALSE);
		ImageUnit_End();
	#endif

	Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_MOVIE_UVAC_STOP, 0);
#endif

	//#NT#2018/06/11#KCHong -begin
	//#NT#For MovieMulti Test
	ImageApp_MovieMulti_DispStop(_CFG_DISP_ID_1);
	#if (SENSOR_CAPS_COUNT == 2)
#if !defined(_SENSOR1_CMOS_OFF_)
	ImageApp_MovieMulti_ImgLinkForDisp(_CFG_REC_ID_1, DISABLE, TRUE);
#endif
#if !defined(_SENSOR2_CMOS_OFF_)
	ImageApp_MovieMulti_ImgLinkForDisp(_CFG_REC_ID_2, DISABLE, TRUE);
#endif
	#elif (SENSOR_CAPS_COUNT == 4)
#if !defined(_SENSOR1_CMOS_OFF_)
	ImageApp_MovieMulti_ImgLinkForDisp(_CFG_REC_ID_1, DISABLE, TRUE);
#endif
#if !defined(_SENSOR2_CMOS_OFF_)
	ImageApp_MovieMulti_ImgLinkForDisp(_CFG_REC_ID_2, DISABLE, TRUE);
#endif
#if !defined(_SENSOR3_CMOS_OFF_)
	ImageApp_MovieMulti_ImgLinkForDisp(_CFG_REC_ID_3, DISABLE, TRUE);
#endif
#if !defined(_SENSOR4_CMOS_OFF_)
	ImageApp_MovieMulti_ImgLinkForDisp(_CFG_REC_ID_4, DISABLE, TRUE);
#endif
	#else
	#if defined(_NVT_ETHREARCAM_TX_)
	// do nothing
	#elif defined(_NVT_ETHREARCAM_RX_)
#if !defined(_SENSOR1_CMOS_OFF_)
	ImageApp_MovieMulti_ImgLinkForDisp(_CFG_REC_ID_1, DISABLE, TRUE);
#endif
	#else
#if !defined(_SENSOR1_CMOS_OFF_)
	ImageApp_MovieMulti_ImgLinkForDisp(_CFG_REC_ID_1, DISABLE, FALSE);
#endif
	#endif
	#endif
	//#NT#2018/06/11#KCHong -end

#if( (_BOARD_DRAM_SIZE_ > 0x04000000)||( defined(_NVT_ETHREARCAM_TX_) && AV_LCA_FUNC == ENABLE))
	UINT32 i, mask;
	UINT32 u32CurrSensorEn = System_GetEnableSensor();
#if defined(_SENSOR1_CMOS_OFF_)
	u32CurrSensorEn &= ~SENSOR_1;
#endif
#if defined(_SENSOR2_CMOS_OFF_)
	u32CurrSensorEn &= ~SENSOR_2;
#endif
	// For ALG function
	mask = 1;
	for (i = 0; i < SENSOR_CAPS_COUNT; i++) {
		if (gMovie_Alg_Info[_CFG_REC_ID_1 + i].path13.ImgSize.w && gMovie_Alg_Info[_CFG_REC_ID_1 + i].path13.ImgSize.h) {
			if (u32CurrSensorEn & mask) {
				ImageApp_MovieMulti_ImgLinkForAlg((_CFG_REC_ID_1 + i), _CFG_ALG_PATH3, DISABLE, TRUE);
			}
		}
		if (gMovie_Alg_Info[_CFG_REC_ID_1 + i].path14.ImgSize.w && gMovie_Alg_Info[_CFG_REC_ID_1 + i].path14.ImgSize.h) {
			if (u32CurrSensorEn & mask) {
				ImageApp_MovieMulti_ImgLinkForAlg((_CFG_REC_ID_1 + i), _CFG_ALG_PATH4, DISABLE, TRUE);
			}
		}
		mask <<= 1;
	}
#endif

#if defined(_ASR_ENABLE_)
	ImageApp_MovieMulti_AudCapStop(_CFG_REC_ID_1);
#endif
#if (AUDPROC_FUNC == ENABLE)
	ImageApp_MovieMulti_AudCapStop(_CFG_REC_ID_1);
	AudProc_UnInit();
#endif

	ImageApp_MovieMulti_Close();

	ImageApp_MovieMulti_FMClose('A');
    #if defined(_EMBMEM_EMMC_) && (FS_MULTI_STRG_FUNC==ENABLE)
	ImageApp_MovieMulti_FMClose('B');
	#endif
	#if (LOG_DBGINFO_IN_GPS_SECTION == ENABLE)
	MovieExe_UnInitMp4DbgLog();
	#endif  // #if (LOG_DBGINFO_IN_GPS_SECTION == ENABLE)

#if (DUALCAM_PIP_BEHIND_FLIP == ENABLE)
	PipView_BFLIP_DestroyBuff();
#endif
	if (MovieExe_ExitCommonMem() < 0) {
		return NVTEVT_CONSUME;
	}

    #if (DUALCAM_PIP_BEHIND_FLIP == ENABLE)
    if (UI_GetData(FL_DUAL_CAM) == DUALCAM_BEHIND){
		GxVideo_SetDeviceCtrl(DOUT1, DISPLAY_DEVCTRL_DIRECT, MIRROR_DISCARD);	
    }
	#endif

	//call default
	Ux_DefaultEvent(pCtrl, NVTEVT_EXE_CLOSE, paramNum, paramArray);

	return NVTEVT_CONSUME;
}


static void MovieExe_SetRecParamByRecID(MOVIE_CFG_REC_ID rec_id)
{
	// set record type (video only, video and audio, time-lapse, ...)
	if (SysGetFlag(FL_MOVIE_TIMELAPSE_REC) == MOVIE_TIMELAPSEREC_OFF) {
		if (SysGetFlag(FL_MovieAudioRec) == MOVIE_AUD_REC_OFF) {
			ImageApp_MovieMulti_SetParam(rec_id, MOVIEMULTI_PARAM_REC_FORMAT, _CFG_REC_TYPE_VID);
		} else {
			ImageApp_MovieMulti_SetParam(rec_id, MOVIEMULTI_PARAM_REC_FORMAT, _CFG_REC_TYPE_AV);
		}
		ImageApp_MovieMulti_SetParam(rec_id, MOVIEMULTI_PARAM_TIMELAPSE_TIME, 0);
		ImageApp_MovieMulti_SetParam(rec_id, MOVIEMULTI_PARAM_FILE_ENDTYPE, (SysGetFlag(FL_MOVIE_CYCLIC_REC) == MOVIE_CYCLICREC_OFF) ? MOVREC_ENDTYPE_CUT_TILLCARDFULL : MOVREC_ENDTYPE_CUTOVERLAP);
	} else {
		ImageApp_MovieMulti_SetParam(rec_id, MOVIEMULTI_PARAM_REC_FORMAT, _CFG_REC_TYPE_TIMELAPSE);
		ImageApp_MovieMulti_SetParam(rec_id, MOVIEMULTI_PARAM_TIMELAPSE_TIME, Get_MovieTimeLapseValue(SysGetFlag(FL_MOVIE_TIMELAPSE_REC)));
		ImageApp_MovieMulti_SetParam(rec_id, MOVIEMULTI_PARAM_FILE_PLAYFRAMERATE, MOVIE_TIMELAPSE_PLAYFRAMERATE);
		ImageApp_MovieMulti_SetParam(rec_id, MOVIEMULTI_PARAM_FILE_ENDTYPE, (SysGetFlag(FL_MOVIE_CYCLIC_REC) == MOVIE_CYCLICREC_OFF) ? MOVREC_ENDTYPE_CUT_TILLCARDFULL : MOVREC_ENDTYPE_CUTOVERLAP);
	}
}

INT32 MovieExe_OnRecStart(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UINT32 skip_frame =0 ;
    UINT32 uifile_buffer_reserved_sec = 7; // cj 0604 for test slow card 10;  // 7~29 seconds.
    UINT32 i, mask;
    UINT32 movie_rec_mask, clone_rec_mask;
#if (_BOARD_DRAM_SIZE_ == 0x04000000)
    uifile_buffer_reserved_sec =7;
#endif
    #if 0
    // JIRA
    ISIZE devSize = GxVideo_GetDeviceSize(DOUT1);

    if (devSize.w >= 3840 && gMovie_Rec_Info.size.w >= 3840)
    {
        DBG_ERR("can't record 4K movie under 4K HDMI!\r\n");
        return NVTEVT_CONSUME;
    }
    #endif

	//#NT# save rec status
	UI_SetData(FL_MOVIE_REC, 1);

	// Set UDTA and GPS data
    Movie_SetUserData();

	movie_rec_mask = Movie_GetMovieRecMask();
	clone_rec_mask = Movie_GetCloneRecMask();

	mask = 1;
	for (i = 0; i < SENSOR_CAPS_COUNT; i++) {
		if ((movie_rec_mask & mask) || (SENSOR_INSERT_MASK & mask)) {
			MovieExe_SetRecParamByRecID(gMovie_Rec_Info[i].rec_id);
		}

		if (clone_rec_mask & mask) {
			MovieExe_SetRecParamByRecID(gMovie_Clone_Info[i].rec_id);
		}

		mask <<= 1;
	}

	// Set skip frame
	if ((SysGetFlag(FL_MOVIE_AUDIO) == MOVIE_AUDIO_OFF) ||                      // Video only
		(SysGetFlag(FL_MOVIE_TIMELAPSE_REC) != MOVIE_TIMELAPSEREC_OFF)) {       // Timelapse mode
		skip_frame = 0;
	} else {
		// Note: need adjust skip frame for different frame rate
		skip_frame = 3;
	}

	for (i = 0; i < SENSOR_CAPS_COUNT; i++) {
		ImageApp_MovieMulti_SetParam(gMovie_Rec_Info[i].rec_id, MOVIEMULTI_PARAM_SKIP_FRAME, skip_frame);
		// Set buffer reserved second. Default 10 (7~29).
		ImageApp_MovieMulti_SetParam(gMovie_Rec_Info[i].rec_id, MOVIEMULTI_PARAM_FILE_BUFRESSEC, uifile_buffer_reserved_sec);
#if (SENSOR_CAPS_COUNT == 1)
		//ImageApp_MovieMulti_SetParam(gMovie_Rec_Info[i].rec_id, MOVIEMULTI_PARAM_PRECALC_BUFFER, 40 * 1024 * 1024);
#endif
	}
#if(SENSOR_CAPS_COUNT & SENSOR_ON_MASK)
    if (UI_GetData(FL_MOVIE_DATEIMPRINT) == MOVIE_DATEIMPRINT_ON) {

        STAMP_COLOR StampColorBg = {RGB_GET_Y( 16,  16,  16), RGB_GET_U( 16,  16,  16), RGB_GET_V( 16,  16,  16)}; // date stamp background color
        STAMP_COLOR StampColorFr = {RGB_GET_Y( 16,  16,  16), RGB_GET_U( 16,  16,  16), RGB_GET_V( 16,  16,  16)}; // date stamp frame color
        STAMP_COLOR StampColorFg = {RGB_GET_Y(224, 224, 192), RGB_GET_U(224, 224, 192), RGB_GET_V(224, 224, 192)}; // date stamp foreground color

        UINT32      uiStampAddr;
        UINT32      uiVidEncId=0;
#if (STAMP_PRIVATE_POOL == DISABLE)
        // use POOL_ID_DATEIMPRINT as movie data stamp buffer
        uiStampAddr = OS_GetMempoolAddr(POOL_ID_DATEIMPRINT);


        MovieStamp_SetBuffer(uiVidEncId, uiStampAddr, POOL_SIZE_DATEIMPRINT/2);

        MovieStamp_SetColor(uiVidEncId, &StampColorBg, &StampColorFr, &StampColorFg);

        MovieStamp_Setup(
            uiVidEncId,
            STAMP_ON |
            STAMP_AUTO |
            STAMP_DATE_TIME |
#if MOVIE_AE_LOG
			STAMP_TOP_LEFT |
#else
            STAMP_BOTTOM_RIGHT |
#endif
            STAMP_POS_NORMAL |
            STAMP_BG_TRANSPARENT |
            STAMP_YY_MM_DD |
            STAMP_IMG_420UV,
            gMovie_Rec_Info[0].size.w,
            gMovie_Rec_Info[0].size.h,
            NULL);

#else
        UINT32 movie_rec_mask, clone_rec_mask;
        UINT32 i, mask;
        movie_rec_mask = Movie_GetMovieRecMask();
        clone_rec_mask = Movie_GetCloneRecMask();

        mask = 1;
        for (i = 0; i < SENSOR_CAPS_COUNT; i++) {

            //DBG_DUMP("i=%d, main_rec=%d ,clone_rec=%d\r\n",i, (movie_rec_mask & mask), (clone_rec_mask & mask));
            //DBG_DUMP("i=%d, rec_id=%d ,path_id=%d\r\n",i, gMovie_Rec_Info[i].rec_id, ImageApp_MovieMulti_GetVdoEncPort(gMovie_Rec_Info[i].rec_id));

            if ((movie_rec_mask & mask) || (SENSOR_INSERT_MASK & mask)) {
                uiVidEncId=ImageApp_MovieMulti_GetVdoEncPort(gMovie_Rec_Info[i].rec_id);
				
                //DBG_ERR("main uiVidEncId=%d\r\n",uiVidEncId);
                uiStampAddr=MovieStamp_GetBufAddr(uiVidEncId, MovieStamp_CalcBufSize(gMovie_Rec_Info[i].size.w, gMovie_Rec_Info[i].size.h));//POOL_SIZE_DATEIMPRINT/2);

                MovieStamp_SetBuffer(uiVidEncId, uiStampAddr, MovieStamp_CalcBufSize(gMovie_Rec_Info[i].size.w, gMovie_Rec_Info[i].size.h));//POOL_SIZE_DATEIMPRINT/2);

                MovieStamp_SetColor(uiVidEncId, &StampColorBg, &StampColorFr, &StampColorFg);

                MovieStamp_Setup(
                    uiVidEncId,
                    STAMP_ON |
                    STAMP_AUTO |
                    STAMP_DATE_TIME |
#if MOVIE_AE_LOG
        			STAMP_TOP_LEFT |
#else
                    STAMP_BOTTOM_RIGHT |
#endif
                    STAMP_POS_NORMAL |
                    STAMP_BG_TRANSPARENT |
                    STAMP_YY_MM_DD |
                    STAMP_IMG_420UV,
                    gMovie_Rec_Info[i].size.w,
                    gMovie_Rec_Info[i].size.h,
#if defined (WATERLOGO_FUNCTION) && (WATERLOGO_FUNCTION == ENABLE)
            (WATERLOGO_BUFFER *)&g_WaterLogo);
#else
                    NULL);
#endif
            }
            if ((clone_rec_mask & mask) || (SENSOR_INSERT_MASK & mask)) {
                uiVidEncId=ImageApp_MovieMulti_GetVdoEncPort(gMovie_Clone_Info[i].rec_id);
                //DBG_ERR("clone uiVidEncId=%d\r\n",uiVidEncId);
                uiStampAddr=MovieStamp_GetBufAddr(uiVidEncId, MovieStamp_CalcBufSize(gMovie_Clone_Info[i].size.w, gMovie_Clone_Info[i].size.h));//POOL_SIZE_DATEIMPRINT/2);

                MovieStamp_SetBuffer(uiVidEncId, uiStampAddr, MovieStamp_CalcBufSize(gMovie_Clone_Info[i].size.w, gMovie_Clone_Info[i].size.h));//POOL_SIZE_DATEIMPRINT/2);

                MovieStamp_SetColor(uiVidEncId, &StampColorBg, &StampColorFr, &StampColorFg);

                MovieStamp_Setup(
                    uiVidEncId,
                    STAMP_ON |
                    STAMP_AUTO |
                    STAMP_DATE_TIME |
#if MOVIE_AE_LOG
        			STAMP_TOP_LEFT |
#else
                    STAMP_BOTTOM_LEFT |
#endif
                    STAMP_POS_NORMAL |
                    STAMP_BG_TRANSPARENT |
                    STAMP_YY_MM_DD |
                    STAMP_IMG_420UV,
					gMovie_Clone_Info[i].size.w,
					gMovie_Clone_Info[i].size.h,
#if defined (WATERLOGO_FUNCTION) && (WATERLOGO_FUNCTION == ENABLE)
            (WATERLOGO_BUFFER *)&g_WaterLogo);
#else
                    NULL);
#endif
		}

		mask <<= 1;
	}
#endif



#if (THUMB_STAMP_FUNC == ENABLE)
	UINT32 ThmWid, ThmHei;

	MovieStampThumb_SetBuffer();

	if (gMovie_Rec_Info[0].dar == _CFG_DAR_16_9) {
		ThmWid = NMI_IMGCAP_16_9_W;
		ThmHei = NMI_IMGCAP_16_9_H;
	} else {
		if (gMovie_Rec_Info[0].size.w > gMovie_Rec_Info[0].size.h) { // max support 1:1 image
			ThmWid = NMI_IMGCAP_16_9_W; // fixed thumbnail width
			ThmHei = ALIGN_CEIL_16((gMovie_Rec_Info[0].size.h * ThmWid) / gMovie_Rec_Info[0].size.w);
		} else {
			DBG_ERR("Get thumb size error! w=%d h=%d\r\n", gMovie_Rec_Info[0].size.w, gMovie_Rec_Info[0].size.h);
			return NVTEVT_CONSUME;
		}
	}
	
	MovieStamp_Setup_Thumb(
			0,
            STAMP_ON |
            STAMP_AUTO |
            STAMP_DATE_TIME |
            STAMP_BOTTOM_RIGHT |
            STAMP_POS_NORMAL |
            STAMP_BG_TRANSPARENT |
            STAMP_YY_MM_DD |
            STAMP_IMG_420UV,
            ThmWid,
            ThmHei,
            NULL);
#endif

#if 0
		{
			UINT16 i;
			UPOINT Pos[5]={
				{10,203},
				{120,20},
				{1000,903},
				{1500,5},
				{400,400},
				};
			char *Name[5]={"0017597:00000","1234567:00000","1:23AP00012","12345670000M","1:234556700000"};

			for(i=0;i<3;i++){
				if(strlen(Name[i])){
					MovieStamp_DrawMultiStamp(0, i, &Pos[i], Name[i]);
				}
			}
			for(i=3;i<5;i++){
				if(strlen(Name[i])){
					MovieStamp_DrawMultiStamp(2, i, &Pos[i], Name[i]);
				}
			}
		}
#endif

         MovieStamp_Enable();
    }else{
         MovieStamp_Disable();
    }
#endif
#if 0
#if(defined(_NVT_ETHREARCAM_RX_))
	for (i = 0; i < (SENSOR_CAPS_COUNT & SENSOR_ON_MASK); i++) {
#else
	for (i = 0; i < SENSOR_CAPS_COUNT; i++) {
#endif
		ImageUnit_Begin(&ISF_BsMux, 0);
		ImageUnit_SetParam(ImageApp_MovieMulti_Recid2BsPort(_CFG_REC_ID_1 + i), BSMUXER_PARAM_FLUSH_SEC, 10);
		ImageUnit_SetParam(ImageApp_MovieMulti_Recid2BsPort(_CFG_REC_ID_1 + i), BSMUXER_PARAM_WRITE_BLKSIZE, 0x200000);
		ImageUnit_End();
	}
#endif
	alg_MovieExe_OnRecStart();

	//#NT#2017/10/26#Adam Su -begin
	//#NT#AUTO_TEST
	exam_msg("record start\r\n");
	//#NT#2017/10/26#Adam Su -end

	// trigger record

#if 0//(defined(_NVT_ETHREARCAM_RX_) && (_BOARD_DRAM_SIZE_ <= 0x08000000) && (((SENSOR_CAPS_COUNT & SENSOR_ON_MASK) + ETH_REARCAM_CAPS_COUNT) > 2))
	DBG_WRN("Rx sensor not record\r\n");
#else
	mask = 1;
#if(defined(_NVT_ETHREARCAM_RX_))
	for (i = 0; i < (SENSOR_CAPS_COUNT & SENSOR_ON_MASK); i++) {
#else
	for (i = 0; i < SENSOR_CAPS_COUNT; i++) {
#endif
		if (movie_rec_mask & mask) {
			//#NT#2019/12/17#Philex Lin -begin
        	//#NT#Add EMR_BSQ_INCARD setting
			#if PMOVIE_BSQINCARD_FUNC
            if (gMovie_Rec_Option.emr_on==_CFG_MAIN_EMR_LOOP)
            {
                ImageApp_MovieMulti_SetParam(gMovie_Rec_Info[i].rec_id, MOVIEMULTI_PARAM_EMR_BSQ_INCARD, TRUE);
                ImageApp_MovieMulti_SetParam(gMovie_Rec_Info[i].rec_id, MOVIEMULTI_PARAM_FILE_ROLLBACKSEC, 30);
            }
			#endif
    	    //#NT#2019/12/17#Philex Lin -end
			ImageApp_MovieMulti_RecStart(gMovie_Rec_Info[i].rec_id);
		}

		if (clone_rec_mask & mask) {
			ImageApp_MovieMulti_RecStart(gMovie_Clone_Info[i].rec_id);
		}

		mask <<= 1;
	}
#endif

#if(defined(_NVT_ETHREARCAM_RX_))
	for (i = 0; i < ETH_REARCAM_CAPS_COUNT; i++) {
		if(socketCliEthData1_IsRecv(ETHCAM_PATH_ID_1 +i) && ImageApp_MovieMulti_IsStreamRunning(_CFG_ETHCAM_ID_1 + i)==0){
			ImageApp_MovieMulti_SetParam((_CFG_ETHCAM_ID_1 + i), MOVIEMULTI_RARAM_ETHCAM_RX_AUD_FROM_TX_FUNC_EN, g_EthCamCfgTxAudCap);
#if defined(_NVT_ETHREARCAM_CAPS_COUNT_1_)
			EthCam_SendXMLCmd(i, ETHCAM_PORT_DATA1 ,ETHCAM_CMD_TX_STREAM_STOP, 0);
#endif
			MovieExe_SetRecParamByRecID(_CFG_ETHCAM_ID_1 + i);
			ImageApp_MovieMulti_SetParam((_CFG_ETHCAM_ID_1 + i), MOVIEMULTI_PARAM_FILE_BUFRESSEC, uifile_buffer_reserved_sec);
			ImageApp_MovieMulti_EthCamRxRecStart(_CFG_ETHCAM_ID_1 + i);
#if defined(_NVT_ETHREARCAM_CAPS_COUNT_1_)
			socketCliEthData1_RecvResetParam(i);
			EthCam_SendXMLCmd(i, ETHCAM_PORT_DATA1 ,ETHCAM_CMD_TX_STREAM_START, 0);
#endif
		}
//#if(ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
		//EthCam_SendXMLCmd(ETHCAM_PATH_ID_1, ETHCAM_PORT_DATA1 ,ETHCAM_CMD_TX_STREAM_START, 0);
//#endif
	}
#endif
	// disable auto power off/USB detect timer
	KeyScan_EnableMisc(FALSE);
	gMovData.State = MOV_ST_REC;
	return NVTEVT_CONSUME;

}

INT32 MovieExe_OnStrmStart(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
//#NT#2018/06/15#KCHong -begin
//#NT#multirec test
#if 0
    UINT32 id = gMovie_Strm_Info.strm_id;

    if (paramNum) {
        id = paramArray[0];
    }
    ImageApp_Movie_Streaming_Start(id);
#else
	#if (SENSOR_CAPS_COUNT == 2)
#if !defined(_SENSOR1_CMOS_OFF_)
	ImageApp_MovieMulti_ImgLinkForStreaming(_CFG_REC_ID_1, ENABLE, TRUE);
#endif
	if (System_GetEnableSensor() == (SENSOR_1 | SENSOR_2)){
#if !defined(_SENSOR2_CMOS_OFF_)
		ImageApp_MovieMulti_ImgLinkForStreaming(_CFG_REC_ID_2, ENABLE, TRUE);
#endif
	}else if (System_GetEnableSensor() == SENSOR_1 ){
#if !defined(_SENSOR2_CMOS_OFF_)
		ImageApp_MovieMulti_ImgLinkForStreaming(_CFG_REC_ID_2, DISABLE, TRUE);
#endif
	}
	#else
#if !defined(_SENSOR1_CMOS_OFF_)
	#if(defined(_NVT_ETHREARCAM_RX_))
	ImageApp_MovieMulti_ImgLinkForStreaming(_CFG_REC_ID_1, ENABLE, TRUE);
	#else
	ImageApp_MovieMulti_ImgLinkForStreaming(_CFG_REC_ID_1, ENABLE, FALSE);
#endif
	#endif
	#endif
	ImageApp_MovieMulti_StreamingStart(_CFG_STRM_ID_1);
#endif
//#NT#2018/06/15#KCHong -end
#if (defined(_NVT_ETHREARCAM_RX_) && (SENSOR_CAPS_COUNT & SENSOR_ON_MASK)==0  && ETH_REARCAM_CAPS_COUNT >=2)
	return NVTEVT_CONSUME;
#endif
#if(SENSOR_CAPS_COUNT & SENSOR_ON_MASK)
	if (UI_GetData(FL_MOVIE_DATEIMPRINT) == MOVIE_DATEIMPRINT_ON) {
	UINT32		uiStampAddr;
	STAMP_COLOR StampColorBg = {RGB_GET_Y( 16,	16,  16), RGB_GET_U( 16,  16,  16), RGB_GET_V( 16,	16,  16)}; // date stamp background color
	STAMP_COLOR StampColorFr = {RGB_GET_Y( 16,	16,  16), RGB_GET_U( 16,  16,  16), RGB_GET_V( 16,	16,  16)}; // date stamp frame color
	STAMP_COLOR StampColorFg = {RGB_GET_Y(224, 224, 192), RGB_GET_U(224, 224, 192), RGB_GET_V(224, 224, 192)}; // date stamp foreground color
	UINT32 path_id=ImageApp_MovieMulti_GetVdoEncPort(_CFG_STRM_ID_1);

#if (STAMP_PRIVATE_POOL == DISABLE)

	// use POOL_ID_DATEIMPRINT as movie data stamp buffer
	uiStampAddr = OS_GetMempoolAddr(POOL_ID_DATEIMPRINT);
	MovieStamp_SetBuffer(path_id, uiStampAddr+POOL_SIZE_DATEIMPRINT/2, POOL_SIZE_DATEIMPRINT/2);
#else
	uiStampAddr=MovieStamp_GetBufAddr(path_id, MovieStamp_CalcBufSize(gMovie_Strm_Info.size.w, gMovie_Strm_Info.size.h));//POOL_SIZE_DATEIMPRINT/2);
	MovieStamp_SetBuffer(path_id, uiStampAddr, MovieStamp_CalcBufSize(gMovie_Strm_Info.size.w, gMovie_Strm_Info.size.h));//POOL_SIZE_DATEIMPRINT/2);
#endif

	MovieStamp_SetColor(path_id, &StampColorBg, &StampColorFr, &StampColorFg);

	MovieStamp_Setup(
		path_id,
		STAMP_ON |
		STAMP_AUTO |
		STAMP_DATE_TIME |
#if MOVIE_AE_LOG
		STAMP_TOP_LEFT |
#else
		STAMP_BOTTOM_LEFT |
#endif
		STAMP_POS_NORMAL |
		STAMP_BG_TRANSPARENT |
		STAMP_YY_MM_DD |
		STAMP_IMG_420UV,
		gMovie_Strm_Info.size.w,
		gMovie_Strm_Info.size.h,
#if defined (WATERLOGO_FUNCTION) && (WATERLOGO_FUNCTION == ENABLE)
		(WATERLOGO_BUFFER *)&g_WaterLogo);
#else
		NULL);
#endif

        MovieStamp_Enable();
	}
#endif
	return NVTEVT_CONSUME;
}

INT32 MovieExe_OnRecStep(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	return NVTEVT_CONSUME;
}

INT32 MovieExe_OnRecStop(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	UINT32 i, mask;
	UINT32 movie_rec_mask, clone_rec_mask;

	UI_SetData(FL_MOVIE_REC, 0);
	movie_rec_mask = Movie_GetMovieRecMask();
	clone_rec_mask = Movie_GetCloneRecMask();

	mask = 1;
	for (i = 0; i < SENSOR_CAPS_COUNT; i++) {
		if (movie_rec_mask & mask) {
			ImageApp_MovieMulti_RecStop(gMovie_Rec_Info[i].rec_id);
		}

		if (clone_rec_mask & mask) {
			ImageApp_MovieMulti_RecStop(gMovie_Clone_Info[i].rec_id);
		}

		mask <<= 1;
	}

	alg_MovieExe_OnRecStop();

#if(defined(_NVT_ETHREARCAM_RX_))
//#if(ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
//	EthCam_SendXMLCmd(ETHCAM_PATH_ID_1, ETHCAM_PORT_DATA1 ,ETHCAM_CMD_TX_STREAM_STOP, 0);
//#endif
	for (i = 0; i < ETH_REARCAM_CAPS_COUNT; i++) {
		ImageApp_MovieMulti_EthCamRxRecStop(_CFG_ETHCAM_ID_1 + i);
	}
#endif

	// Enable auto power off/USB detect timer
	KeyScan_EnableMisc(TRUE);
	//jira:CARDV_680-150, IVOT_N00017_CO-235
	if (UI_GetData(FL_MOVIE_DATEIMPRINT) == MOVIE_DATEIMPRINT_ON && ImageApp_MovieMulti_IsStreamRunning(_CFG_STRM_ID_1)==0) {
		MovieStamp_Disable();
	}
//g_bRecStopFinish = 1;

if (UIFlowWndMovie_GetRecLockStatus() == TRUE)
{
	UIFlowWndMovie_SetRecLockStatus(FALSE);
}
	gMovData.State = MOV_ST_VIEW;

	return NVTEVT_CONSUME;
}

INT32 MovieExe_OnStrmStop(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
//#NT#2018/06/15#KCHong -begin
//#NT#multirec test
#if 0
    UINT32 id = gMovie_Strm_Info.strm_id;

    if (paramNum) {
        id = paramArray[0];
    }
    ImageApp_Movie_Streaming_Stop(id);
#else
	ImageApp_MovieMulti_StreamingStop(_CFG_STRM_ID_1);
	#if (SENSOR_CAPS_COUNT == 2)
#if !defined(_SENSOR1_CMOS_OFF_)
	ImageApp_MovieMulti_ImgLinkForStreaming(_CFG_REC_ID_1, DISABLE, TRUE);
#endif
	if (System_GetEnableSensor() == (SENSOR_1 | SENSOR_2)){
#if !defined(_SENSOR2_CMOS_OFF_)
		ImageApp_MovieMulti_ImgLinkForStreaming(_CFG_REC_ID_2, DISABLE, TRUE);
#endif
	}
	#else
#if !defined(_SENSOR1_CMOS_OFF_)
	ImageApp_MovieMulti_ImgLinkForStreaming(_CFG_REC_ID_1, DISABLE, FALSE);
#endif
	#endif
#endif
//#NT#2018/06/15#KCHong -end
	return NVTEVT_CONSUME;
}

INT32 MovieExe_OnUvacStart(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	#if (SENSOR_CAPS_COUNT == 2)
	ImageApp_MovieMulti_ImgLinkForStreaming(_CFG_REC_ID_1, ENABLE, TRUE);
	if (System_GetEnableSensor() == (SENSOR_1 | SENSOR_2)) {
		ImageApp_MovieMulti_ImgLinkForStreaming(_CFG_REC_ID_2, ENABLE, TRUE);
	} else if (System_GetEnableSensor() == SENSOR_1) {
		ImageApp_MovieMulti_ImgLinkForStreaming(_CFG_REC_ID_2, DISABLE, TRUE);
	}
	#else
	#if defined(_NVT_ETHREARCAM_RX_)
	ImageApp_MovieMulti_ImgLinkForStreaming(_CFG_REC_ID_1, ENABLE, TRUE);
	#else
	ImageApp_MovieMulti_ImgLinkForStreaming(_CFG_REC_ID_1, ENABLE, FALSE);
	#endif
	#endif
	ImageApp_MovieMulti_UvacStart(_CFG_UVAC_ID_1);

#if MOVIE_UVAC_FUNC // if you want to enable data stamp on UVAC, please change to #if 1
	if (UI_GetData(FL_MOVIE_DATEIMPRINT) == MOVIE_DATEIMPRINT_ON) {
	UINT32		uiStampAddr;
	STAMP_COLOR StampColorBg = {RGB_GET_Y( 16,	16,  16), RGB_GET_U( 16,  16,  16), RGB_GET_V( 16,	16,  16)}; // date stamp background color
	STAMP_COLOR StampColorFr = {RGB_GET_Y( 16,	16,  16), RGB_GET_U( 16,  16,  16), RGB_GET_V( 16,	16,  16)}; // date stamp frame color
	STAMP_COLOR StampColorFg = {RGB_GET_Y(224, 224, 192), RGB_GET_U(224, 224, 192), RGB_GET_V(224, 224, 192)}; // date stamp foreground color
	UINT32 path_id=ImageApp_MovieMulti_GetVdoEncPort(_CFG_UVAC_ID_1);


#if (STAMP_PRIVATE_POOL == DISABLE)
	// use POOL_ID_DATEIMPRINT as movie data stamp buffer
	uiStampAddr = OS_GetMempoolAddr(POOL_ID_DATEIMPRINT);
	MovieStamp_SetBuffer(path_id, uiStampAddr+POOL_SIZE_DATEIMPRINT/2, POOL_SIZE_DATEIMPRINT/2);
#else
	uiStampAddr=MovieStamp_GetBufAddr(path_id, MovieStamp_CalcBufSize(gUIUvacVidReso[0].width, gUIUvacVidReso[0].height));//POOL_SIZE_DATEIMPRINT/2);
	MovieStamp_SetBuffer(path_id, uiStampAddr, MovieStamp_CalcBufSize(gUIUvacVidReso[0].width, gUIUvacVidReso[0].height));//POOL_SIZE_DATEIMPRINT/2);
#endif

	MovieStamp_SetColor(path_id, &StampColorBg, &StampColorFr, &StampColorFg);

	MovieStamp_Setup(
		path_id,
		STAMP_ON |
		STAMP_AUTO |
		STAMP_DATE_TIME |
		STAMP_BOTTOM_RIGHT |
		STAMP_POS_NORMAL |
		STAMP_BG_TRANSPARENT |
		STAMP_YY_MM_DD |
		STAMP_IMG_420UV,
		gUIUvacVidReso[0].width,
		gUIUvacVidReso[0].height,
#if defined (WATERLOGO_FUNCTION) && (WATERLOGO_FUNCTION == ENABLE)
		(WATERLOGO_BUFFER *)&g_WaterLogo);
#else
		NULL);
#endif

        MovieStamp_Enable();
	}
#endif
	return NVTEVT_CONSUME;
}

INT32 MovieExe_OnUvacStop(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	ImageApp_MovieMulti_UvacStop(_CFG_UVAC_ID_1);
	#if (SENSOR_CAPS_COUNT == 2)
	ImageApp_MovieMulti_ImgLinkForStreaming(_CFG_REC_ID_1, DISABLE, TRUE);
	if (System_GetEnableSensor() == (SENSOR_1 | SENSOR_2)) {
		ImageApp_MovieMulti_ImgLinkForStreaming(_CFG_REC_ID_2, DISABLE, TRUE);
	}
	#else
	#if defined(_NVT_ETHREARCAM_RX_)
	ImageApp_MovieMulti_ImgLinkForStreaming(_CFG_REC_ID_1, DISABLE, TRUE);
	#else
	ImageApp_MovieMulti_ImgLinkForStreaming(_CFG_REC_ID_1, DISABLE, FALSE);
	#endif
	#endif

	return NVTEVT_CONSUME;
}

INT32 MovieExe_OnRecPIM(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	return NVTEVT_CONSUME;
}

INT32 MovieExe_OnRecRawEnc(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	UINT32 i, mask, movie_rec_mask;

 	movie_rec_mask = Movie_GetMovieRecMask();
	mask = 1;
	if (g_bAllowRawEnc == FALSE){
		DBG_ERR("RawEnc Busy....\r\n");
		return NVTEVT_CONSUME;
	}

	g_bAllowRawEnc = FALSE;
	for (i = 0; i < SENSOR_CAPS_COUNT; i++) {
#if(defined(_NVT_ETHREARCAM_TX_))
		if (movie_rec_mask & mask) {
#else//Rx and Normal
		if (movie_rec_mask & mask && ImageApp_MovieMulti_IsStreamRunning(i)) {
#endif
			if (ImageApp_MovieMulti_TriggerSnapshot(gMovie_Rec_Info[i].rec_id) != ISF_OK) {
				// set g_bAllowRawEnc flag for next time trigger due to snapshot is failed this time
				g_bAllowRawEnc = TRUE;
				return NVTEVT_CONSUME;
			}
		}
		mask <<= 1;
	}
#if(defined(_NVT_ETHREARCAM_RX_))
	for(i=0;i<ETH_REARCAM_CAPS_COUNT;i++){
		if(socketCliEthData1_IsRecv(ETHCAM_PATH_ID_1 +i)){
			EthCam_SendXMLCmd(i, ETHCAM_PORT_DATA1 ,ETHCAM_CMD_GET_TX_MOVIE_RAW_ENCODE, 0);
		}
	}

#endif
	return NVTEVT_CONSUME;
}

INT32 MovieExe_OnCallback(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	NVTEVT event;

	event = paramArray[0];

	DBG_IND("[cb]%d\r\n", event);
	switch (event) {
	case NVTEVT_ALGMSG_FLASH:
		DBG_IND("[cb]NVTEVT_ALGMSG_FLASH\r\n");
		break;

	case NVTEVT_ALGMSG_QVSTART:
		DBG_IND("[cb]NVTEVT_ALGMSG_QVSTART\r\n");
		break;

	case NVTEVT_ALGMSG_JPGOK:
		DBG_IND("[cb]NVTEVT_ALGMSG_JPGOK\r\n");
		break;

	case NVTEVT_ALGMSG_CAPFSTOK:
		DBG_IND("[cb]NVTEVT_ALGMSG_CAPFSTOK\r\n");
		break;


	case NVTEVT_ALGMSG_CAPTUREEND:
		DBG_IND("[cb]NVTEVT_ALGMSG_CAPTUREEND \r\n");
		break;

	case NVTEVT_ALGMSG_DZOOMSTEP:
		DBG_IND("[cb]NVTEVT_ALGMSG_DZOOMSTEP \r\n");
		break;

	case NVTEVT_ALGMSG_PREVIEW_STABLE:
		DBG_IND("[cb]NVTEVT_ALGMSG_PREVIEW_STABLE \r\n");
		break;

	default:

		break;
	}
	return NVTEVT_PASS;
}

INT32 MovieExe_OnVideoChange(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
#if (PIP_VIEW_FUNC == ENABLE)
	PipView_SetStyle(UI_GetData(FL_DUAL_CAM));
#endif
	//DBGD(Perf_GetCurrent());
	if (System_GetState(SYS_STATE_CURRMODE) == PRIMARY_MODE_MOVIE) {
		Ux_SendEvent(0, NVTEVT_EXE_IMAGE_RATIO, 1, GetMovieSizeRatio(UI_GetData(FL_MOVIE_SIZE)));
	}
	//DBGD(Perf_GetCurrent());
	return NVTEVT_PASS; //PASS this event to UIWnd!
}


INT32 MovieExe_OnDualcam(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
#if  1// _TODO
	UINT32 uhSelect = 0;
	if (paramNum > 0) {
		uhSelect = paramArray[0];
	}

	DBG_IND("%d \r\n", uhSelect);
	debug_msg("%d \r\n", uhSelect);

#if (SENSOR_CAPS_COUNT == 1)
	//DBG_ERR("single sensor, not support this option!\r\n");//rear cam is ethcam
#endif
#if  1//(SENSOR_CAPS_COUNT == 2)

	UI_SetData(FL_DUAL_CAM, uhSelect);

	// also change the capture id
	//PhotoExe_OnCaptureID(pCtrl, paramNum, paramArray);
	//if (localInfo->DualCam != uhSelect)
#if (PIP_VIEW_FASTSWITCH==ENABLE)
	{
		Ux_SendEvent(0, NVTEVT_SENSOR_DISPLAY, 1, (SENSOR_1 | SENSOR_2)); //for Always trigger PIP View
		//#NT#2015/11/25#Niven Cho#[87393][88541] -begin
		//Here be invoked at startup without in any mode
		if (System_GetState(SYS_STATE_CURRMODE) == PRIMARY_MODE_MOVIE) {
			Ux_SendEvent(0, NVTEVT_EXE_IMAGE_RATIO, 1, GetMovieSizeRatio(UI_GetData(FL_MOVIE_SIZE)));
		}
		PipView_SetStyle(UI_GetData(FL_DUAL_CAM));
		//#NT#2015/11/25#Niven Cho -end
	}
#else
	{
		// set display display
		if (uhSelect == DUALCAM_FRONT) {
			CHKPNT;
			Ux_SendEvent(0, NVTEVT_SENSOR_DISPLAY, 1, SENSOR_1);
		}
		if (uhSelect == DUALCAM_BEHIND) {
			CHKPNT;
			Ux_SendEvent(0, NVTEVT_SENSOR_DISPLAY, 1, SENSOR_2);
		}
		if (uhSelect == DUALCAM_BOTH) {
			CHKPNT;
			Ux_SendEvent(0, NVTEVT_SENSOR_DISPLAY, 1, (SENSOR_1 | SENSOR_2));
		}
	}
#endif //(PIP_VIEW_FASTSWITCH==ENABLE)

#if 0//(WIFI_AP_FUNC==ENABLE)
	if  ((System_GetState(SYS_STATE_CURRSUBMODE) == SYS_SUBMODE_WIFI)
		// config pip view parameter
		{
			USIZE   HttpLviewSize;
			UINT32  ImageRatioIdx;

			ImageRatioIdx = GetMovieSizeRatio(UI_GetData(FL_MOVIE_SIZE));
			HttpLviewSize = MOVIE_HTTPLVIEW_SIZE[ImageRatioIdx];
			PhotoExe_CfgNetHttpPIP(HttpLviewSize, UI_GetData(FL_DUAL_CAM));

		}
	}
#endif

#endif

#endif
	return NVTEVT_CONSUME;
}

//#NT#2016/06/08#Lincy Lin -begin
//#NT#Implement generic OSD and video drawing mechanism for ALG function
static void MovieExe_CallBackUpdateInfo(UINT32 callBackEvent)
{
#if(UI_FUNC==ENABLE)
	VControl *pCurrnetWnd;

	Ux_GetFocusedWindow(&pCurrnetWnd);
	Ux_SendEvent(pCurrnetWnd, NVTEVT_UPDATE_INFO, 1, callBackEvent);
#endif
}
//#NT#2016/06/08#Lincy Lin -end


INT32 MovieExe_OnProtectAuto(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	UINT32 uiSelect    = 0;
	UINT32 i;

	if (paramNum) {
		uiSelect = paramArray[0];
	}
	UI_SetData(FL_MOVIE_URGENT_PROTECT_AUTO, uiSelect);

	if (UI_GetData(FL_MOVIE_URGENT_PROTECT_AUTO) == MOVIE_URGENT_PROTECT_AUTO_ON ||
		UI_GetData(FL_MOVIE_URGENT_PROTECT_MANUAL) == MOVIE_URGENT_PROTECT_MANUAL_ON) {
		if (gMovie_Rec_Option.emr_on == _CFG_EMR_OFF) {
			if (GetMovieRecType_2p(UI_GetData(FL_MOVIE_SIZE)) == MOVIE_REC_TYPE_FRONT) {
#if (_BOARD_DRAM_SIZE_ == 0x04000000)
				gMovie_Rec_Option.emr_on = _CFG_EMR_SET_CRASH;  // set crash
#else
				#if PMOVIE_BSQINCARD_FUNC
				gMovie_Rec_Option.emr_on = _CFG_MAIN_EMR_LOOP;//_CFG_MAIN_EMR_BOTH;  // main + emr
				#else
				gMovie_Rec_Option.emr_on = _CFG_MAIN_EMR_BOTH;  // main + emr
				#endif
#endif
#if(defined(_NVT_ETHREARCAM_RX_))
				gMovie_Rec_Option.emr_on = _CFG_EMR_SET_CRASH;  // set crash
#endif
			} else {
				gMovie_Rec_Option.emr_on = _CFG_EMR_SET_CRASH;  // set crash
			}
			//DBGH(gMovie_Rec_Option.emr_on);
			for (i = 0; i < SENSOR_CAPS_COUNT; i++) {
				ImageApp_MovieMulti_SetParam(gMovie_Rec_Info[i].rec_id, MOVIEMULTI_PARAM_FILE_ROLLBACKSEC, gMovie_Rec_Option.emr_sec);
				ImageApp_MovieMulti_SetParam(gMovie_Rec_Info[i].rec_id, MOVIEMULTI_PARAM_FILE_KEEPSEC, gMovie_Rec_Option.keep_sec);
				ImageApp_MovieMulti_SetParam(gMovie_Rec_Info[i].rec_id, MOVIEMULTI_PARAM_FILE_EMRON, gMovie_Rec_Option.emr_on);
			}
#if(defined(_NVT_ETHREARCAM_RX_))
			for (i = 0; i < ETH_REARCAM_CAPS_COUNT; i++) {
				ImageApp_MovieMulti_SetParam((_CFG_ETHCAM_ID_1 + i), MOVIEMULTI_PARAM_FILE_ROLLBACKSEC, gMovie_Rec_Option.emr_sec);
				ImageApp_MovieMulti_SetParam((_CFG_ETHCAM_ID_1 + i), MOVIEMULTI_PARAM_FILE_KEEPSEC, gMovie_Rec_Option.keep_sec);
				ImageApp_MovieMulti_SetParam((_CFG_ETHCAM_ID_1 + i), MOVIEMULTI_PARAM_FILE_EMRON, gMovie_Rec_Option.emr_on);
			}
#endif
		}
	} else {
		if (gMovie_Rec_Option.emr_on != _CFG_EMR_OFF) {
			gMovie_Rec_Option.emr_on = _CFG_EMR_OFF;
			for (i = 0; i < SENSOR_CAPS_COUNT; i++) {
				ImageApp_MovieMulti_SetParam(gMovie_Rec_Info[i].rec_id, MOVIEMULTI_PARAM_FILE_EMRON, gMovie_Rec_Option.emr_on);
			}
#if(defined(_NVT_ETHREARCAM_RX_))
			for (i = 0; i < ETH_REARCAM_CAPS_COUNT; i++) {
				ImageApp_MovieMulti_SetParam((_CFG_ETHCAM_ID_1 + i), MOVIEMULTI_PARAM_FILE_EMRON, gMovie_Rec_Option.emr_on);
			}
#endif
		}
	}

	return NVTEVT_CONSUME;
}

INT32 MovieExe_OnProtectManual(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	UINT32 uiSelect    = 0;
	UINT32 i;

	if (paramNum) {
		uiSelect = paramArray[0];
	}
	UI_SetData(FL_MOVIE_URGENT_PROTECT_MANUAL, uiSelect);

	if (UI_GetData(FL_MOVIE_URGENT_PROTECT_AUTO) == MOVIE_URGENT_PROTECT_AUTO_ON ||
		UI_GetData(FL_MOVIE_URGENT_PROTECT_MANUAL) == MOVIE_URGENT_PROTECT_MANUAL_ON) {
		if (gMovie_Rec_Option.emr_on == _CFG_EMR_OFF) {
			if (GetMovieRecType_2p(UI_GetData(FL_MOVIE_SIZE)) == MOVIE_REC_TYPE_FRONT) {
#if (_BOARD_DRAM_SIZE_ == 0x04000000)
				gMovie_Rec_Option.emr_on = _CFG_EMR_SET_CRASH;  // set crash
#else
				#if PMOVIE_BSQINCARD_FUNC
				gMovie_Rec_Option.emr_on = _CFG_MAIN_EMR_LOOP;//_CFG_MAIN_EMR_BOTH;  // main + emr
				#else
				gMovie_Rec_Option.emr_on = _CFG_MAIN_EMR_BOTH;  // main + emr
				#endif
#endif
#if(defined(_NVT_ETHREARCAM_RX_))
				gMovie_Rec_Option.emr_on = _CFG_EMR_SET_CRASH;  // set crash
#endif
			} else {
				gMovie_Rec_Option.emr_on = _CFG_EMR_SET_CRASH; // set crash
			}
			DBGH(gMovie_Rec_Option.emr_on);
			for (i = 0; i < SENSOR_CAPS_COUNT; i++) {
				ImageApp_MovieMulti_SetParam(gMovie_Rec_Info[i].rec_id, MOVIEMULTI_PARAM_FILE_ROLLBACKSEC, gMovie_Rec_Option.emr_sec);
				ImageApp_MovieMulti_SetParam(gMovie_Rec_Info[i].rec_id, MOVIEMULTI_PARAM_FILE_KEEPSEC, gMovie_Rec_Option.keep_sec);
				ImageApp_MovieMulti_SetParam(gMovie_Rec_Info[i].rec_id, MOVIEMULTI_PARAM_FILE_EMRON, gMovie_Rec_Option.emr_on);
			}
#if(defined(_NVT_ETHREARCAM_RX_))
			for (i = 0; i < ETH_REARCAM_CAPS_COUNT; i++) {
				ImageApp_MovieMulti_SetParam((_CFG_ETHCAM_ID_1 + i), MOVIEMULTI_PARAM_FILE_ROLLBACKSEC, gMovie_Rec_Option.emr_sec);
				ImageApp_MovieMulti_SetParam((_CFG_ETHCAM_ID_1 + i), MOVIEMULTI_PARAM_FILE_KEEPSEC, gMovie_Rec_Option.keep_sec);
				ImageApp_MovieMulti_SetParam((_CFG_ETHCAM_ID_1 + i), MOVIEMULTI_PARAM_FILE_EMRON, gMovie_Rec_Option.emr_on);
			}
#endif
		}
	} else {
		if (gMovie_Rec_Option.emr_on != _CFG_EMR_OFF) {
			gMovie_Rec_Option.emr_on = _CFG_EMR_OFF;
			for (i = 0; i < SENSOR_CAPS_COUNT; i++) {
				ImageApp_MovieMulti_SetParam(gMovie_Rec_Info[i].rec_id, MOVIEMULTI_PARAM_FILE_EMRON, gMovie_Rec_Option.emr_on);
			}
#if(defined(_NVT_ETHREARCAM_RX_))
			for (i = 0; i < ETH_REARCAM_CAPS_COUNT; i++) {
				ImageApp_MovieMulti_SetParam((_CFG_ETHCAM_ID_1 + i), MOVIEMULTI_PARAM_FILE_EMRON, gMovie_Rec_Option.emr_on);
			}
#endif
		}
	}

	return NVTEVT_CONSUME;
}


INT32 MovieExe_OnLDWS(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	return alg_MovieExe_OnLDWS(pCtrl, paramNum, paramArray);
}

INT32 MovieExe_OnFCW(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	return alg_MovieExe_OnFCW(pCtrl, paramNum, paramArray);
}

INT32 MovieExe_OnPauseResume(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	return NVTEVT_CONSUME;
}

void FlowMovie_FileDBCreate(void)
{
#if 0//USE_FILEDB
	MEM_RANGE Pool;

	Pool.Addr = dma_getCacheAddr(OS_GetMempoolAddr(POOL_ID_FILEDB));
	Pool.Size = POOL_SIZE_FILEDB;
	ImageUnit_Begin(&ISF_CamFile, 0);
	ImageUnit_SetParam(ISF_CTRL, CAMFILE_PARAM_USE_FILEDB, (UINT32)&Pool);
	ImageUnit_End();
	// config FDB root
//#NT#2016/08/12#Hideo Lin -begin
//#NT#For small size clone movie
#if (CUSTOM_NAMING_RULE == DISABLE)
	// sample code for FileDB Naming Rule
	// disable custom naming
	ImageUnit_Begin(&ISF_CamFile, 0);
	ImageUnit_SetParam(ISF_CTRL, CAMFILE_PARAM_CUSTOM_NAMING, FALSE);
	ImageUnit_End();

	// set root folder
	ImageApp_CamFile_SetFolder(FILEDB_CARDV_ROOT);

	// set folder name of movie, RO, and photo (sample code)
#if 0
	ImageApp_CamFile_SetMovieFolder(0, "MOVIE");
	ImageApp_CamFile_SetROFolder(0, "LOCK");
	ImageApp_CamFile_SetEMRFolder(0, "EMERGENCY");
	ImageApp_CamFile_SetPhotoFolder(0, "PHOTO");
#endif

	//#NT#2016/03/07#KCHong -begin
	//#NT#Low power timelapse function
#if ((YOUKU_SDK == ENABLE) || (TIMELAPSE_LPR_FUNCTION == ENABLE))
	char RootFolder[NMC_OTHERS_MAX_LEN];
	// enable custom naming
	ImageUnit_Begin(&ISF_CamFile, 0);
	ImageUnit_SetParam(ISF_CTRL, CAMFILE_PARAM_CUSTOM_NAMING, TRUE);
	ImageUnit_End();
	// set root folder
	snprintf(RootFolder, NMC_OTHERS_MAX_LEN, "A:\\%s\\", FILEDB_CARDV_ROOT);
	ImageApp_CamFile_SetRootFolder(RootFolder);
	// set folder name of 1st movie path
	ImageApp_CamFile_SetMovieFolder(0, "MOVIE");
	//ImageApp_CamFile_SetROFolder(0, "RO");
	ImageApp_CamFile_SetPhotoFolder(0, "PHOTO");
#if (SAVE_DUAL_STREAM_FUNC == ENABLE)
	// set folder name of 2nd movie path
	ImageApp_CamFile_SetMovieFolder(1, WIFI_MOVIE_FOLDER);
#endif
#if ((SAVE_DUAL_STREAM_FUNC == ENABLE) || (TIMELAPSE_LPR_FUNCTION == ENABLE))
	// set custom file name
	//manual naming method
	ImageApp_CamFile_SetFileNamingMethod(FILE_NAMING_MANUAL);
	// register callback functions for manual file naming method
	ImageApp_CamFile_RegMovieFileNamingCB(UIMovie_MovieFileNamingCB);
	ImageApp_CamFile_RegPhotoFileNamingCB(UIMovie_PhotoFileNamingCB);
	ImageApp_CamFile_RegEMRMovFileNamingCB(UIMovie_EMRMovFileNamingCB);
#endif
#if (CUT_MOVIE_FUNC == ENABLE)
	ImageApp_CamFile_SetROFolder(1, CUT_MOVIE_FOLDER);
#endif
#endif//(YOUKU_SDK == ENABLE)
	//#NT#2016/03/07#KCHong -end
#else // sample code for Custom Naming Rule

	// enable custom naming
	ImageUnit_Begin(&ISF_CamFile, 0);
	ImageUnit_SetParam(ISF_CTRL, CAMFILE_PARAM_CUSTOM_NAMING, TRUE);
	ImageUnit_End();

#if (SMALL_CLONE_MOVIE == DISABLE)
	// set root folder
#if (FS_MULTI_STRG_FUNC == DISABLE)
	ImageApp_CamFile_SetRootFolder("A:\\CarDV\\");
#else
	ImageApp_CamFile_SetRootFolderByPath(0, "A:\\CarDV\\");
	ImageApp_CamFile_SetRootFolderByPath(1, "B:\\CarDV\\");
#endif

	// set folder name of 1st movie path
	ImageApp_CamFile_SetMovieFolder(0, "Movie_Front");
	ImageApp_CamFile_SetROFolder(0, "RO_Front");
	ImageApp_CamFile_SetEMRFolder(0, "EMR_Front");
	ImageApp_CamFile_SetPhotoFolder(0, "Photo_Front");

	// set folder name of 2nd movie path
	ImageApp_CamFile_SetMovieFolder(1, "Movie_Rear");
	ImageApp_CamFile_SetROFolder(1, "RO_Rear");
	ImageApp_CamFile_SetEMRFolder(1, "EMR_Rear");
	ImageApp_CamFile_SetPhotoFolder(1, "Photo_Rear");

	// set custom file name
#if 0 // manual naming method
	{
		ImageApp_CamFile_SetFileNamingMethod(FILE_NAMING_MANUAL);
		// register callback functions for manual file naming method
		ImageApp_CamFile_RegMovieFileNamingCB(UIMovie_MovieFileNamingCB);
		ImageApp_CamFile_RegPhotoFileNamingCB(UIMovie_PhotoFileNamingCB);
		ImageApp_CamFile_RegEMRMovFileNamingCB(UIMovie_EMRMovFileNamingCB);
	}
#elif 1 // semi-auto naming method
	{
		switch (SysGetFlag(FL_DATE_FORMAT)) {
		case DATE_FORMAT_DMY:
			ImageApp_CamFile_SetFileNamingYMD(FILE_NAMING_DATEFMT_DMY);
			break;
		case DATE_FORMAT_MDY:
			ImageApp_CamFile_SetFileNamingYMD(FILE_NAMING_DATEFMT_MDY);
			break;
		case DATE_FORMAT_YMD:
		default:
			ImageApp_CamFile_SetFileNamingYMD(FILE_NAMING_DATEFMT_YMD);
			break;
		}
		ImageApp_CamFile_SetFileNamingMethod(FILE_NAMING_SEMIAUTO);
		ImageApp_CamFile_SetFileNamingType(FILE_NAMING_SEMIAUTO_TYPE2);
		ImageApp_CamFile_SetFileNameEndChar(0, ENABLE, "_FR");
		ImageApp_CamFile_SetFileNameEndChar(1, ENABLE, "_RE");
		ImageApp_CamFile_SetFileNameSerialNum(0, DISABLE);
		ImageApp_CamFile_SetFileNameSerialNum(1, DISABLE);
		ImageApp_CamFile_SetSemiAutoFileNaming(); // activate semi-auto file naming
	}
#else // auto naming method => need to do nothing
#endif
#else // small size clone movie
	// set root folder
#if (FS_MULTI_STRG_FUNC == DISABLE)
	ImageApp_CamFile_SetRootFolder(DV_ROOT_PATH);
#else
	ImageApp_CamFile_SetRootFolderByPath(0, DV_ROOT_PATH);
	ImageApp_CamFile_SetRootFolderByPath(1, "B:\\CarDV\\");
#endif

	// set folder name of 1st movie path
	ImageApp_CamFile_SetMovieFolder(0, MOVIE_FOLDER);
	ImageApp_CamFile_SetROFolder(0, MOVIE_RO_FOLDER);
	ImageApp_CamFile_SetEMRFolder(0, MOVIE_RO_FOLDER);
	ImageApp_CamFile_SetPhotoFolder(0, PHOTO_FOLDER);

	// set folder name of 2nd movie path
	ImageApp_CamFile_SetMovieFolder(1, MOVIE_SHARE_FOLDER);
	ImageApp_CamFile_SetROFolder(1, MOVIE_SHARE_RO_FOLDER);
	ImageApp_CamFile_SetEMRFolder(1, MOVIE_SHARE_RO_FOLDER);
	ImageApp_CamFile_SetPhotoFolder(1, PHOTO_FOLDER);

	// set custom file name
	ImageApp_CamFile_SetFileNamingMethod(FILE_NAMING_MANUAL);
	// register callback functions for manual file naming method
	ImageApp_CamFile_RegMovieFileNamingCB(UIMovie_MovieFileNamingCB);
	//ImageApp_CamFile_RegEMRMovFileNamingCB(UIMovie_EMRMovieFileNamingCB);
	ImageApp_CamFile_RegPhotoFileNamingCB(UIMovie_PhotoFileNamingCB);
#endif
#endif
//#NT#2016/08/12#Hideo Lin -end
#endif
}

INT32 SetupExe_OnFILEDB_FILEID_RESET(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray);

void FlowMovie_FileDBCreate_Fast(void)
{
	if (System_GetState(SYS_STATE_POWERON) == SYSTEM_POWERON_NORMAL) {
#if 0//USE_FILEDB
		MEM_RANGE Pool;

		Pool.Addr = dma_getCacheAddr(OS_GetMempoolAddr(POOL_ID_FILEDB));
		Pool.Size = POOL_SIZE_FILEDB;
		ImageUnit_Begin(&ISF_CamFile, 0);
		ImageUnit_SetParam(ISF_CTRL, CAMFILE_PARAM_USE_FILEDB, (UINT32)&Pool);
		ImageUnit_End();
		// config FDB root
		ImageApp_CamFile_SetFolder(FILEDB_CARDV_ROOT);
		// create FDB
		ImageUnit_Begin(&ISF_CamFile, 0);
		ImageUnit_SetParam(ISF_CTRL, CAMFILE_PARAM_MOVIE_FILEDB_CRATE, 1);
		ImageUnit_End();
		// reset FDB file id
		if (UI_GetData(FL_IsUseFileDB)) {
			//Ux_SendEvent(&UISetupObjCtrl,NVTEVT_FILEID_RESET,0);
			SetupExe_OnFILEDB_FILEID_RESET(NULL, 0, NULL);
		}
#endif
	}
}

BOOL FlowMovie_CheckReOpenItem(void)
{
	BOOL bReOpen = FALSE, bMovieSizeMenu = FALSE;
	#if (defined(_NVT_ETHREARCAM_RX_))
	BOOL bTxStreamReStart = FALSE;
	#endif

	if (UI_GetData(FL_MOVIE_SIZE_MENU) != UI_GetData(FL_MOVIE_SIZE)) {
		// Set resolution on mode open to avoid wrong preview image.
		UI_SetData(FL_MOVIE_SIZE, UI_GetData(FL_MOVIE_SIZE_MENU));
		bReOpen = TRUE;
		bMovieSizeMenu = TRUE;
	}

//#MT#lyb -20200409 -begin
    if(UI_GetData(FL_MOVIE_CYCLIC_REC_MENU) != UI_GetData(FL_MOVIE_CYCLIC_REC))
    {
        UI_SetData(FL_MOVIE_CYCLIC_REC, UI_GetData(FL_MOVIE_CYCLIC_REC_MENU));
        bReOpen = TRUE;

    }
//#MT#lyb -20200409 -end




	// if multi recored, set EMR mode to _CFG_EMR_MODE4 (set crash)
	if (UI_GetData(FL_MOVIE_URGENT_PROTECT_AUTO) == MOVIE_URGENT_PROTECT_AUTO_ON) {
		Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_MOVIE_PROTECT_AUTO, 1, MOVIE_URGENT_PROTECT_AUTO_ON);
	}
	if (UI_GetData(FL_MOVIE_URGENT_PROTECT_MANUAL) == MOVIE_URGENT_PROTECT_MANUAL_ON) {
		Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_MOVIE_PROTECT_MANUAL, 1, MOVIE_URGENT_PROTECT_MANUAL_ON);
	}

	if (UI_GetData(FL_MOVIE_HDR_MENU) != UI_GetData(FL_MOVIE_HDR)) {
		if (UI_GetData(FL_MOVIE_HDR_MENU) == MOVIE_HDR_ON) {
			UI_SetData(FL_MOVIE_WDR, MOVIE_WDR_OFF);
			UI_SetData(FL_MOVIE_WDR_MENU, MOVIE_WDR_OFF);
			UI_SetData(FL_MovieRSCIndex, MOVIE_RSC_OFF);
			UI_SetData(FL_MovieRSCIndex_MENU, MOVIE_RSC_OFF);
		}
		UI_SetData(FL_MOVIE_HDR, UI_GetData(FL_MOVIE_HDR_MENU));
		bReOpen = TRUE;
	}

	if (UI_GetData(FL_MOVIE_WDR_MENU) != UI_GetData(FL_MOVIE_WDR)) {

		if (UI_GetData(FL_MOVIE_WDR_MENU) == MOVIE_WDR_ON) {
			UI_SetData(FL_MOVIE_HDR, MOVIE_HDR_OFF);
			UI_SetData(FL_MOVIE_HDR_MENU, MOVIE_HDR_OFF);
		}
		UI_SetData(FL_MOVIE_WDR, UI_GetData(FL_MOVIE_WDR_MENU));
		bReOpen = TRUE;
	}

	if (UI_GetData(FL_MOVIE_DEFOG_MENU) != UI_GetData(FL_MOVIE_DEFOG)) {
		UI_SetData(FL_MOVIE_DEFOG, UI_GetData(FL_MOVIE_DEFOG_MENU));
		bReOpen = TRUE;
	}

	if (UI_GetData(FL_MovieRSCIndex_MENU) != UI_GetData(FL_MovieRSCIndex)) {

		if (UI_GetData(FL_MovieRSCIndex_MENU) == MOVIE_RSC_ON) {
			UI_SetData(FL_MOVIE_HDR, MOVIE_HDR_OFF);
			UI_SetData(FL_MOVIE_HDR_MENU, MOVIE_HDR_OFF);
		}
		UI_SetData(FL_MovieRSCIndex, UI_GetData(FL_MovieRSCIndex_MENU));
		bReOpen = TRUE;
	}

#if !defined(_SENSOR2_CMOS_OFF_)
	if (bMovieSizeMenu) {
		switch (UI_GetData(FL_MOVIE_SIZE_MENU)) {
		case MOVIE_SIZE_FRONT_2880x2160P24:
		case MOVIE_SIZE_FRONT_2560x1440P30:
		case MOVIE_SIZE_FRONT_1920x1080P60:
		case MOVIE_SIZE_FRONT_1280x720P120:
		case MOVIE_SIZE_FRONT_1280x720P60:
		case MOVIE_SIZE_FRONT_640x480P240:
			//#NT#2016/06/17#Brain Yen -begin
			//#NT#for special aspect panel and Pipview LR
#if (PIP_VIEW_LR == ENABLE)
			UI_SetData(FL_DUAL_CAM_MENU, DUALCAM_LR_FRONT_FULL);
#else
			UI_SetData(FL_DUAL_CAM_MENU, DUALCAM_FRONT);
#endif
			//#NT#2016/06/17#Brain Yen -end
			break;

		default:
			//#NT#2016/06/17#Brain Yen -begin
			//#NT#for special aspect panel and Pipview LR
#if (PIP_VIEW_LR == ENABLE)
			if (System_GetEnableSensor() & SENSOR_2) {
				UI_SetData(FL_DUAL_CAM_MENU, DUALCAM_LR_16_9);
			} else {
				UI_SetData(FL_DUAL_CAM_MENU, DUALCAM_LR_FRONT_FULL);
			}
#else
			if (System_GetEnableSensor() & SENSOR_2) {
				UI_SetData(FL_DUAL_CAM_MENU, DUALCAM_BOTH);
			} else {
				UI_SetData(FL_DUAL_CAM_MENU, DUALCAM_FRONT);
			}
#endif
			//#NT#2016/06/17#Brain Yen -end
			break;
		}


	}
#endif

	if (UI_GetData(FL_DUAL_CAM_MENU) != UI_GetData(FL_DUAL_CAM)) {
		if (bMovieSizeMenu == FALSE) {
			switch (UI_GetData(FL_MOVIE_SIZE_MENU)) {
			case MOVIE_SIZE_FRONT_2880x2160P24:
			case MOVIE_SIZE_FRONT_2560x1440P30:
			case MOVIE_SIZE_FRONT_1920x1080P60:
			case MOVIE_SIZE_FRONT_1280x720P120:
			case MOVIE_SIZE_FRONT_1280x720P60:
			case MOVIE_SIZE_FRONT_640x480P240:
				break;

			default:
				Ux_PostEvent(NVTEVT_EXE_DUALCAM, 1, UI_GetData(FL_DUAL_CAM_MENU));
				break;
			}
		} else {
			UI_SetData(FL_DUAL_CAM, UI_GetData(FL_DUAL_CAM_MENU));
#if (PIP_VIEW_FUNC == ENABLE)
			PipView_SetStyle(UI_GetData(FL_DUAL_CAM));
#endif
		}

		if (bMovieSizeMenu == FALSE) {
			bReOpen = FALSE;    // because in NVTEVT_EXE_DUALCAM, NVTEVT_EXE_MOVIE_DUALCAM will invoke ReOpen
		}
	}

#if (WIFI_FINALCAM_APP_STYLE == ENABLE || YOUKU_SDK == ENABLE)
	{
		UINT32 uiCurRecWidth = 0, uiCurRecHeight = 0;
		UINT32 uiTargetWidth = 0, uiTargetHeight = 0;

#if _TODO
		MovRec_GetInfo(MOVREC_GETINFO_WIDTH_HEIGHT_P2, &uiCurRecWidth, &uiCurRecHeight, 0);
#endif

		DBG_DUMP("^YCurW:%d, CurH:%d\r\n", uiCurRecWidth, uiCurRecHeight);

		switch (UI_GetData(FL_WIFI_MOVIE_APP_PREVIEW_SIZE)) {
		default:
		case WIFI_MOVIE_APP_PREVIEW_SIZE_VGA_16_9:  // 640 x 360
			uiTargetWidth = 640;
			uiTargetHeight = 360;
			break;

		case WIFI_MOVIE_APP_PREVIEW_SIZE_720P:      // 1280 x 720
			uiTargetWidth = 1280;
			uiTargetHeight = 720;
			break;

		case WIFI_MOVIE_APP_PREVIEW_SIZE_WVGA:      // 848 x 480
			uiTargetWidth = 848;
			uiTargetHeight = 480;
			break;

		case WIFI_MOVIE_APP_PREVIEW_SIZE_VGA_4_3:   // 640 x 480
			uiTargetWidth = 640;
			uiTargetHeight = 480;
			break;

		case WIFI_MOVIE_APP_PREVIEW_SIZE_QVGA:      // 320 x 240
			uiTargetWidth = 320;
			uiTargetHeight = 240;
			break;
		}

		if (uiTargetWidth != uiCurRecWidth || uiTargetHeight != uiCurRecHeight) {
			bReOpen = TRUE;
		}
	}
#elif (UCTRL_APP_MOVIE_FEATURE_SETGET == ENABLE)
	// check channel 1
	{
		UINT32 uiCurRecWidth = 0, uiCurRecHeight = 0;
		UINT32 uiTargetWidth = 0, uiTargetHeight = 0;
		UINT32 channelId = 0;

#if _TODO
		MovRec_GetInfo(MOVREC_GETINFO_WIDTH_HEIGHT, &uiCurRecWidth, &uiCurRecHeight, 0);
#endif
		DBG_DUMP("^YPath 1-> CurW:%d, CurH:%d\r\n", uiCurRecWidth, uiCurRecHeight);

		uiTargetWidth = GetMovieSizeWidth_2p(channelId, 0);
		uiTargetHeight = GetMovieSizeHeight_2p(channelId, 0);
		if (uiTargetWidth != uiCurRecWidth || uiTargetHeight != uiCurRecHeight) {
			bReOpen = TRUE;
		}
	}
	// check channel 2
	{
		UINT32 uiCurRecWidth = 0, uiCurRecHeight = 0;
		UINT32 uiTargetWidth = 0, uiTargetHeight = 0;
		UINT32 channelId = 1;

#if _TODO
		MovRec_GetInfo(MOVREC_GETINFO_WIDTH_HEIGHT_P2, &uiCurRecWidth, &uiCurRecHeight, 0);
#endif
		DBG_DUMP("^YPath 2-> CurW:%d, CurH:%d\r\n", uiCurRecWidth, uiCurRecHeight);

		uiTargetWidth = GetMovieSizeWidth_2p(channelId, 0);
		uiTargetHeight = GetMovieSizeHeight_2p(channelId, 0);

		if (uiTargetWidth != uiCurRecWidth || uiTargetHeight != uiCurRecHeight) {
			bReOpen = TRUE;
		}
	}
#endif

	if (UI_GetData(FL_MOVIE_LDWS) != UI_GetData(FL_MOVIE_LDWS_MENU)) {
		UI_SetData(FL_MOVIE_LDWS, UI_GetData(FL_MOVIE_LDWS_MENU));
		bReOpen = TRUE;
	}

	if (UI_GetData(FL_MOVIE_FCW) != UI_GetData(FL_MOVIE_FCW_MENU)) {
		UI_SetData(FL_MOVIE_FCW, UI_GetData(FL_MOVIE_FCW_MENU));
		bReOpen = TRUE;
	}

	if (UI_GetData(FL_MOVIE_TIMELAPSE_REC) != UI_GetData(FL_MOVIE_TIMELAPSE_REC_MENU)) {
		if ((UI_GetData(FL_MOVIE_TIMELAPSE_REC) == MOVIE_TIMELAPSEREC_OFF) &&
			(UI_GetData(FL_MOVIE_LDWS) == MOVIE_LDWS_ON || UI_GetData(FL_MOVIE_FCW) == MOVIE_FCW_ON)) {
			bReOpen = TRUE;
		}
		UI_SetData(FL_MOVIE_TIMELAPSE_REC, UI_GetData(FL_MOVIE_TIMELAPSE_REC_MENU));
	}
	if (UI_GetData(FL_MOVIE_CODEC) != UI_GetData(FL_MOVIE_CODEC_MENU)) {
		UI_SetData(FL_MOVIE_CODEC, UI_GetData(FL_MOVIE_CODEC_MENU));
		#if (defined(_NVT_ETHREARCAM_RX_))
		bReOpen = TRUE;
		#else
		Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_MOVIE_CODEC, 1, UI_GetData(FL_MOVIE_CODEC_MENU));
		#endif
	}
	if (UI_GetData(FL_MOVIE_SENSOR_ROTATE) != UI_GetData(FL_MOVIE_SENSOR_ROTATE_MENU)) {
		UI_SetData(FL_MOVIE_SENSOR_ROTATE, UI_GetData(FL_MOVIE_SENSOR_ROTATE_MENU));
		#if (defined(_NVT_ETHREARCAM_RX_))
		bTxStreamReStart = TRUE;
		#endif
		Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_MOVIE_SENSOR_ROTATE, 1, UI_GetData(FL_MOVIE_SENSOR_ROTATE_MENU));
	}
#if (defined(_NVT_ETHREARCAM_RX_))
	UINT16 j;
	UINT32 AllPathLinkStatus=0;
	EthCamCmd_GetFrameTimerEn(0);
	//jira:IVOT_SQA_N00004-998
	if(UI_GetData(FL_ResetFlag)){
		bReOpen=TRUE;
		UI_SetData(FL_ResetFlag, FALSE);
	}
	
#if (ETH_REARCAM_CAPS_COUNT>=2)
	Delay_DelayMs(200);//wait pull cmd finish
#endif
	if(bReOpen){
		for(j=0;j<ETH_REARCAM_CAPS_COUNT;j++){
			if(socketCliEthCmd_IsConn(j) && EthCamNet_GetEthLinkStatus(j)==ETHCAM_LINK_UP){
				ImageApp_MovieMulti_EthCamLinkForDisp((_CFG_ETHCAM_ID_1 + j), DISABLE, TRUE);
				EthCam_SendXMLCmd(j, ETHCAM_PORT_DATA1 ,ETHCAM_CMD_TX_STREAM_STOP, 0);
				#if (ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
				EthCam_SendXMLCmd(j, ETHCAM_PORT_DATA2 ,ETHCAM_CMD_TX_STREAM_STOP, 0);
				#endif
			}
		}
	}else if(bTxStreamReStart){
		for(j=0;j<ETH_REARCAM_CAPS_COUNT;j++){
			if(socketCliEthCmd_IsConn(j) && EthCamNet_GetEthLinkStatus(j)==ETHCAM_LINK_UP){
				EthCam_SendXMLCmd(j, ETHCAM_PORT_DATA1 ,ETHCAM_CMD_TX_STREAM_STOP, 0);
				#if (ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
				EthCam_SendXMLCmd(j, ETHCAM_PORT_DATA2 ,ETHCAM_CMD_TX_STREAM_STOP, 0);
				#endif
			}
		}
	}
#if 0
	ETHCAM_MENU_SETTING sEthCamMenuSetting[ETH_REARCAM_CAPS_COUNT]={0};
	for(j=0;j<ETH_REARCAM_CAPS_COUNT;j++){
		if(socketCliEthCmd_IsConn(j) && EthCamNet_GetEthLinkStatus(j)==ETHCAM_LINK_UP){
			AllPathLinkStatus++;
			EthCam_SendXMLCmd(j, ETHCAM_PORT_DEFAULT ,ETHCAM_CMD_SYNC_MENU_SETTING, bReOpen);
			#if (ETH_REARCAM_CAPS_COUNT>=2)
			sEthCamMenuSetting[j].Size=MOVIE_SIZE_FRONT_1920x1080P30;//MOVIE_SIZE_CLONE_1920x1080P30_1280x720P60;//UI_GetData(FL_MOVIE_SIZE);
			#else
			sEthCamMenuSetting[j].Size=MOVIE_SIZE_CLONE_1920x1080P30_1280x720P30;//UI_GetData(FL_MOVIE_SIZE);
			#endif
			sEthCamMenuSetting[j].WDR=UI_GetData(FL_MOVIE_WDR);
			sEthCamMenuSetting[j].EV=UI_GetData(FL_EV);
			sEthCamMenuSetting[j].DateImprint=UI_GetData(FL_MOVIE_DATEIMPRINT);
			sEthCamMenuSetting[j].SensorRotate=UI_GetData(FL_MOVIE_SENSOR_ROTATE);
			sEthCamMenuSetting[j].Codec=MOVIE_CODEC_H264;//UI_GetData(FL_MOVIE_CODEC);
#if defined(_NVT_ETHREARCAM_CAPS_COUNT_1_)
			sEthCamMenuSetting[j].TimeLapse=UI_GetData(FL_MOVIE_TIMELAPSE_REC);
#else
			sEthCamMenuSetting[j].TimeLapse=MOVIE_TIMELAPSEREC_OFF;
#endif
			DBG_DUMP("Size=%d\r\n", sEthCamMenuSetting[j].Size);
			EthCam_SendXMLData(j, (UINT8 *)&sEthCamMenuSetting[j], sizeof(ETHCAM_MENU_SETTING));
			//if(memcmp(&g_sEthCamMenuSetting, &sEthCamMenuSetting, sizeof(ETHCAM_MENU_SETTING))){
			//	bReOpenMovie=1;
			//}
			//memcpy(&g_sEthCamMenuSetting[j], &sEthCamMenuSetting[j], sizeof(ETHCAM_MENU_SETTING));
			if(bReOpen){
				socketCliEthData1_SetRecv(j, 0);
				socketCliEthData2_SetRecv(j, 0);

				EthCamSocketCli_Close(j, ETHSOCKIPCCLI_ID_0);
				EthCamSocketCli_Close(j, ETHSOCKIPCCLI_ID_1);
				EthCamSocketCli_Close(j, ETHSOCKIPCCLI_ID_2);
			}
		}
	}
#endif
	if(!bReOpen && AllPathLinkStatus){
		 if(bTxStreamReStart){
			for(j=0;j<ETH_REARCAM_CAPS_COUNT;j++){
				if(socketCliEthCmd_IsConn(j) && EthCamNet_GetEthLinkStatus(j)==ETHCAM_LINK_UP){
					EthCam_SendXMLCmd(j, ETHCAM_PORT_DATA1 ,ETHCAM_CMD_TX_STREAM_START, 0);
					#if (ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
					EthCam_SendXMLCmd(j, ETHCAM_PORT_DATA2 ,ETHCAM_CMD_TX_STREAM_START, 0);
					#endif
				}
			}
		}
		EthCamCmd_GetFrameTimerEn(1);
	}
#endif

	return bReOpen;
}



//#NT#2016/06/08#Lincy Lin -begin
//#NT#Implement generic OSD and video drawing mechanism for ALG function

INT32 MovieExe_OnALGEnd(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	//Flush ALG event before draw
	Ux_FlushEventByRange(NVTEVT_EXE_MOVIE_ALGEND, NVTEVT_EXE_MOVIE_ALGEND);
	MovieExe_CallBackUpdateInfo(UIAPPPHOTO_CB_ALGEND);
	return NVTEVT_CONSUME;
}
//#NT#2016/06/08#Lincy Lin -end


INT32 MovieExe_OnADASSetCropWin(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	URECT CropWin;

	CropWin.x = (paramArray[0] & 0xffff0000) >> 16;
	CropWin.y = (paramArray[0] & 0x0000ffff);
	CropWin.w = (paramArray[1] & 0xffff0000) >> 16;
	CropWin.h = (paramArray[1] & 0x0000ffff);

	//DBG_DUMP("UI: Set crop = %d/%d/%d/%d\r\n", CropWin.x, CropWin.y, CropWin.w, CropWin.h);
	ImageUnit_Begin(ISF_IPL(0), 0);
		ImageUnit_SetVdoPostWindow(ISF_OUT4, CropWin.x, CropWin.y, CropWin.w, CropWin.h);
	ImageUnit_End();
	ImageStream_UpdateAll(&ISF_Stream[0]);
	return NVTEVT_CONSUME;
}

INT32 MovieExe_OnMotionDetRun(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	if (paramNum >= 1) {
		*(UINT32 *)(paramArray[0]) = Movie_md_GetResult();
	}
	return NVTEVT_CONSUME;
}

INT32 MovieExe_OnCodec(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	UINT32 uiSelect = 0;
    UINT32 uiValue = 0;
	UINT32 i, movie_rec_mask, clone_rec_mask, mask = 1;

	if (paramNum > 0) {
		uiSelect = paramArray[0];
	}

	DBG_IND("uiSelect %d\r\n", uiSelect);
	UI_SetData(FL_MOVIE_CODEC, uiSelect);

    switch(uiSelect){
        case MOVIE_CODEC_H264:
            uiValue = _CFG_CODEC_H264;
        break;
        case MOVIE_CODEC_H265:
            uiValue = _CFG_CODEC_H265;
        break;
        case MOVIE_CODEC_MJPG:
            uiValue = _CFG_CODEC_MJPG;
        break;
        default:
            DBG_ERR("not support\r\n");
    }

	movie_rec_mask = Movie_GetMovieRecMask();
	clone_rec_mask = Movie_GetCloneRecMask();

	for (i = 0; i < SENSOR_CAPS_COUNT; i++) {
		if (movie_rec_mask & mask) {
			MovieExe_SetVdoCodecByRecID(gMovie_Rec_Info[i].rec_id, uiValue);
			#if(defined(_NVT_ETHREARCAM_TX_))
			gMovie_Rec_Info[i].codec=uiValue;
			#endif
		}

		if (clone_rec_mask & mask) {
			MovieExe_SetVdoCodecByRecID(gMovie_Clone_Info[i].rec_id, uiValue);
			#if(defined(_NVT_ETHREARCAM_TX_))
			gMovie_Clone_Info[i].codec=uiValue;
			#endif
		}

		mask <<= 1;
	}

	return NVTEVT_CONSUME;
}

INT32 MovieExe_OnSensorHotPlug(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
#if (SENSOR_CAPS_COUNT == 2)
	 MovieExe_2sensor_HotPlug_Disp();

	 if (System_GetState(SYS_STATE_CURRSUBMODE) == SYS_SUBMODE_WIFI){
	 	MovieExe_2sensor_HotPlug_WiFi();
	 } else {
		 #if MOVIE_UVAC_FUNC
		 MovieExe_2sensor_HotPlug_WiFi();
		 #endif
	 }

	 MovieExe_2sensor_HotPlug_Rec();
#endif

	return NVTEVT_CONSUME;
}
INT32 MovieExe_OnEthcamHotPlug(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
#if(defined(_NVT_ETHREARCAM_RX_))
	UINT32 isRec=0;
	ETHCAM_PATH_ID path_id=_CFG_ETHCAM_ID_1;

	if (paramNum > 0) {
		path_id = paramArray[0];
	}else{
		DBG_ERR("paramNum=%d\r\n",paramNum);
	}

	if (System_GetState(SYS_STATE_CURRSUBMODE) == SYS_SUBMODE_WIFI){
		// not considering the case that all sensors can be plugged out temporarily
		isRec = ImageApp_MovieMulti_IsStreamRunning(_CFG_REC_ID_1)
	               #if (SENSOR_CAPS_COUNT >= 2)
	               | ImageApp_MovieMulti_IsStreamRunning(_CFG_REC_ID_2)
	               #endif
	               | ImageApp_MovieMulti_IsStreamRunning(_CFG_ETHCAM_ID_1)
	               #if (ETH_REARCAM_CAPS_COUNT >= 2)
			  | ImageApp_MovieMulti_IsStreamRunning(_CFG_ETHCAM_ID_2)
			  #endif
			  ;
	} else {
		isRec = (FlowMovie_GetMovDataState() == MOV_ST_REC) ? 1 : 0;
	}
	BKG_PostEvent(NVTEVT_BKW_ETHCAM_UPDATE_UI);

	DBG_DUMP("isRec=%d,%d\r\n",isRec, ImageApp_MovieMulti_IsStreamRunning(_CFG_ETHCAM_ID_1+ path_id));
	if (isRec && ImageApp_MovieMulti_IsStreamRunning(_CFG_ETHCAM_ID_1+ path_id)==0) {
		UINT32 uifile_buffer_reserved_sec = 7;  // 7~29 seconds.
#if (_BOARD_DRAM_SIZE_ == 0x04000000)
		uifile_buffer_reserved_sec =7;
#endif
		MovieExe_SetRecParamByRecID(_CFG_ETHCAM_ID_1 + path_id);
		ImageApp_MovieMulti_SetParam((_CFG_ETHCAM_ID_1 + path_id), MOVIEMULTI_PARAM_FILE_BUFRESSEC, uifile_buffer_reserved_sec);
		ImageApp_MovieMulti_EthCamRxRecStart(_CFG_ETHCAM_ID_1+path_id);
	}
#endif

	return NVTEVT_CONSUME;
}

////////////////////////////////////////////////////////////

EVENT_ENTRY CustomMovieObjCmdMap[] = {
	{NVTEVT_EXE_OPEN,                 MovieExe_OnOpen                     },
	{NVTEVT_EXE_CLOSE,                MovieExe_OnClose                    },
	{NVTEVT_EXE_MOVIESIZE,            MovieExe_OnMovieSize                },
	{NVTEVT_EXE_IMAGE_RATIO,          MovieExe_OnImageRatio               },
	{NVTEVT_EXE_MOVIEQUALITY,         MovieExe_OnMovieQuality             },
	{NVTEVT_EXE_WB,                   MovieExe_OnWB                       },
	{NVTEVT_EXE_MOVIECOLOR,           MovieExe_OnColor                    },
	{NVTEVT_EXE_MOVIE_EV,             MovieExe_OnEV                       },
	{NVTEVT_EXE_MOVIE_AUDIO,          MovieExe_OnMovieAudio               },
	{NVTEVT_EXE_MOVIE_AUDIO_VOL,      MovieExe_OnMovieAudioRecVolume      },
	{NVTEVT_EXE_MOVIE_AUDIO_REC,      MovieExe_OnMovieAudioRec            },
	{NVTEVT_EXE_MOVIEDZOOM,           MovieExe_OnDigitalZoom              },
	{NVTEVT_EXE_MOVIEDIS,             MovieExe_OnDis                      },
	{NVTEVT_EXE_MOVIE_MCTF,           MovieExe_OnMCTF                     },
	{NVTEVT_EXE_MOVIE_EDGE,           MovieExe_OnEdge                     },
	{NVTEVT_EXE_MOVIE_NR,             MovieExe_OnNR                       },
	{NVTEVT_EXE_MOVIE_RSC,            MovieExe_OnRSC                      },
	{NVTEVT_EXE_MOVIE_WDR,            MovieExe_OnWDR                      },
	{NVTEVT_EXE_SHDR,                 MovieExe_OnSHDR                     },
	{NVTEVT_EXE_MOVIE_SENSOR_ROTATE,  MovieExe_OnSensorRotate             },
	{NVTEVT_EXE_MOVIE_IR_CUT,         MovieExe_OnIRCut                    },
	{NVTEVT_EXE_MOVIEGDC,             MovieExe_OnGdc                      },
	{NVTEVT_EXE_MOVIESMEAR,           MovieExe_OnSmear                    },
	{NVTEVT_EXE_MOVIE_DATE_IMPRINT,   MovieExe_OnDateImprint              },
	{NVTEVT_EXE_GSENSOR,              MovieExe_OnGSENSOR                  },

//#MT#lyb -20200403 -begin
    {NVTEVT_EXE_MOVIE_PARK,          MovieExe_OnMoviePark                 },
//#MT#lyb -20200403 -end
	{NVTEVT_EXE_MOVIE_BACKADJUST, MovieExe_OnSetMovieBackAdjust},
	{NVTEVT_EXE_SETLCDBRIGHTLVL,  MovieExe_OnSetLCDBRIGHTLVL},

    
	{NVTEVT_EXE_CYCLIC_REC,           MovieExe_OnCyclicRec                },
	{NVTEVT_EXE_MOTION_DET,           MovieExe_OnMotionDet                },
	{NVTEVT_CALLBACK,                 MovieExe_OnCallback                 },
	{NVTEVT_EXE_MOVIE_REC_START,      MovieExe_OnRecStart                 },
	{NVTEVT_EXE_MOVIE_REC_STEP,       MovieExe_OnRecStep                  },
	{NVTEVT_EXE_MOVIE_REC_STOP,       MovieExe_OnRecStop                  },
	{NVTEVT_EXE_MOVIE_REC_PIM,        MovieExe_OnRecPIM                   },
	{NVTEVT_EXE_MOVIE_REC_RAWENC,     MovieExe_OnRecRawEnc                },
	{NVTEVT_VIDEO_CHANGE,             MovieExe_OnVideoChange              },
	{NVTEVT_EXE_DUALCAM,              MovieExe_OnDualcam                  },
	{NVTEVT_EXE_MOVIE_PROTECT_AUTO,   MovieExe_OnProtectAuto              },
	{NVTEVT_EXE_MOVIE_PROTECT_MANUAL, MovieExe_OnProtectManual            },

	{NVTEVT_EXE_MOVIE_LDWS,           MovieExe_OnLDWS                     },
	{NVTEVT_EXE_MOVIE_FCW,            MovieExe_OnFCW                      },
	{NVTEVT_EXE_VIDEO_PAUSE_RESUME,   MovieExe_OnPauseResume              },
	//#NT#2016/03/25#KCHong -begin
	//#NT#New ADAS
	{NVTEVT_CB_ADAS_SETCROPWIN,       MovieExe_OnADASSetCropWin           },
	//#NT#2016/03/25#KCHong -end
	{NVTEVT_EXE_MOVIE_ALGEND,         MovieExe_OnALGEnd                   },
	//#NT#2016/06/03#Charlie Chang -begin
	//#NT# support contrast, audioIn, flip, quality set
	{NVTEVT_EXE_MOVIE_CONTRAST,       MovieExe_OnContrast                 },
	{NVTEVT_EXE_MOVIE_AUDIOIN,        MovieExe_OnAudioIn                  },
	{NVTEVT_EXE_MOVIE_AUDIOIN_SR,     MovieExe_OnAudioInSR                },
	{NVTEVT_EXE_MOVIE_FLIP_MIRROR,    MovieExe_OnFipMirror                },
	{NVTEVT_EXE_MOVIE_QUALITY_SET,    MovieExe_OnQualitySet               },
	//#NT#2016/06/03#Charlie Chang -end
	{NVTEVT_EXE_MOTION_DET_RUN,       MovieExe_OnMotionDetRun             },
	{NVTEVT_EXE_MOVIE_STRM_START,     MovieExe_OnStrmStart                },
	{NVTEVT_EXE_MOVIE_STRM_STOP,      MovieExe_OnStrmStop                 },
	{NVTEVT_EXE_MOVIE_UVAC_START,	  MovieExe_OnUvacStart				  },
	{NVTEVT_EXE_MOVIE_UVAC_STOP,	  MovieExe_OnUvacStop				  },
	{NVTEVT_EXE_MOVIE_CODEC,          MovieExe_OnCodec                    },
	{NVTEVT_EXE_MOVIE_SENSORHOTPLUG,  MovieExe_OnSensorHotPlug            },
	{NVTEVT_EXE_MOVIE_ETHCAMHOTPLUG,  MovieExe_OnEthcamHotPlug            },
	{NVTEVT_EXE_MOVIE_DEFOG,            MovieExe_OnDefog},
	{NVTEVT_NULL,                     0},  //End of Command Map
};

CREATE_APP(CustomMovieObj, APP_SETUP)

