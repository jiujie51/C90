
#include <stdio.h>
#include <string.h>
#include "AppControl.h"
//Common
#include "SysKer.h"
#include "SysCfg.h"
#include "PrjCfg.h"
#include "AppLib.h"
#include "usb.h"
#include "PrjCfg.h"
#include "UIAppCommon.h"
#include "dma.h"
#include "Audio.h"
#include "nvtmpp.h"
#include "SysCommon.h"

#if 1
//Lib
#include "UVAC.h"
#include "GxUSB.h"
#include "UIAppUsbCam.h"
#include "Utility.h"
#include "sensor.h"
#include "ImageApp_UsbMovie.h"
#include "ImageUnit_UsbUVAC.h"
#include "ImageUnit_AudIn.h"
#include "ImageUnit_AudEnc.h"
#include "UIPhotoInfo.h"
#include "UIInfo.h"
#include "ImageUnit_VdoOut.h"
#include "ImageUnit_VdoIn.h"
#include "ImageUnit_ImagePipe.h"
#include "ImageUnit_VdoEnc.h"
#include "ImageUnit_UserProc.h"
#include "UIMovieMapping.h"
#include "FileDB.h"
#include "interrupt.h"
#if (VIDEO_FUNC_MJPG == ENABLE)
#include "VideoCodec_MJPG.h"
#endif
#if (VIDEO_FUNC_H264 == ENABLE)
#include "VideoCodec_H264.h"
#endif
#include "AudioCodec_PCM.h"
#include "NMediaRecVdoEnc.h"
#include "ae_ui_info.h"
#if(STAMP_FUNC==ENABLE)
#include "MovieStamp.h"
#include "videosprite.h"
#endif

#define THIS_DBGLVL         2 // 0=FATAL, 1=ERR, 2=WRN, 3=UNIT, 4=FUNC, 5=IND, 6=MSG, 7=VALUE, 8=USER
///////////////////////////////////////////////////////////////////////////////
#define __MODULE__          UIApp_UVAC
#define __DBGLVL__          ((THIS_DBGLVL>=PRJ_DBG_LVL)?THIS_DBGLVL:PRJ_DBG_LVL)
#define __DBGFLT__          "*" //*=All, [mark]=CustomClass
#include "DebugModule.h"
///////////////////////////////////////////////////////////////////////////////
#define UVAC_VENDCMD_CURCMD                         0x01
#define UVAC_VENDCMD_VERSION                        0x02

#define UVAC_PRJ_SET_LIVEVIEW_SAVEFILE          0  //1V + 1F
#define UVAC_PRJ_SET_LIVEVIEW_1V_ONLY           1  //1V only
#define UVAC_PRJ_SET_LIVEVIEW_2V                2  //2V

#define UVAC_MAX_WIDTH		1920
#define UVAC_MAX_HEIGHT		1080
#define UVAC_MAX_FRAMERATE	30

#define UVAC_CT_PU_FUNC          DISABLE

#define UVAC_USE_SPECIFIED_BUF     DISABLE
#define IPC_RESERVED_SIZE   (0xDF000)

#if UVAC_CT_PU_FUNC
// Define Bits Containing Capabilities of the control for the Get_Info request (section 4.1.2 in the UVC spec 1.1)
#define SUPPORT_GET_REQUEST                 0x01
#define SUPPORT_SET_REQUEST                 0x02
#define DISABLED_DUE_TO_AUTOMATIC_MODE      0x04
#define AUTOUPDATE_CONTROL                  0x08
#define ASNCHRONOUS_CONTROL                 0x10
#define RESERVED_BIT5                       0x20
#define RESERVED_BIT6                       0x40
#define RESERVED_BIT7                       0x80

//Camera Terminal Control Selectors
#define CT_CONTROL_UNDEFINED                0x00
#define CT_SCANNING_MODE_CONTROL            0x01
#define CT_AE_MODE_CONTROL                  0x02
#define CT_AE_PRIORITY_CONTROL              0x03
#define CT_EXPOSURE_TIME_ABSOLUTE_CONTROL 0x04
#define CT_EXPOSURE_TIME_RELATIVE_CONTROL 0x05
#define CT_FOCUS_ABSOLUTE_CONTROL           0x06
#define CT_FOCUS_RELATIVE_CONTROL           0x07
#define CT_FOCUS_AUTO_CONTROL               0x08
#define CT_IRIS_ABSOLUTE_CONTROL            0x09
#define CT_IRIS_RELATIVE_CONTROL            0x0A
#define CT_ZOOM_ABSOLUTE_CONTROL            0x0B
#define CT_ZOOM_RELATIVE_CONTROL            0x0C
#define CT_PANTILT_ABSOLUTE_CONTROL         0x0D
#define CT_PANTILT_RELATIVE_CONTROL         0x0E
#define CT_ROLL_ABSOLUTE_CONTROL            0x0F
#define CT_ROLL_RELATIVE_CONTROL            0x10
#define CT_PRIVACY_CONTROL                  0x11

//Processing Unit Control Selectors
#define PU_CONTROL_UNDEFINED                0x00
#define PU_BACKLIGHT_COMPENSATION_CONTROL 0x01
#define PU_BRIGHTNESS_CONTROL               0x02
#define PU_CONTRAST_CONTROL                 0x03
#define PU_GAIN_CONTROL                     0x04
#define PU_POWER_LINE_FREQUENCY_CONTROL 0x05
#define PU_HUE_CONTROL                      0x06
#define PU_SATURATION_CONTROL               0x07
#define PU_SHARPNESS_CONTROL                0x08
#define PU_GAMMA_CONTROL                    0x09
#define PU_WHITE_BALANCE_TEMPERATURE_CONTROL 0x0A
#define PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL 0x0B
#define PU_WHITE_BALANCE_COMPONENT_CONTROL 0x0C
#define PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL 0x0D
#define PU_DIGITAL_MULTIPLIER_CONTROL       0x0E
#define PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL 0x0F
#define PU_HUE_AUTO_CONTROL                 0x10
#define PU_ANALOG_VIDEO_STANDARD_CONTROL 0x11
#define PU_ANALOG_LOCK_STATUS_CONTROL       0x12
#endif

#if (1)//(DISABLE == UVAC_MODE_2_PATH) //only support 1V
#define UVAC_PRJ_SET_THIS           UVAC_PRJ_SET_LIVEVIEW_1V_ONLY
#else
#define UVAC_PRJ_SET_THIS           UVAC_PRJ_SET_LIVEVIEW_2V
//#define UVAC_PRJ_SET_THIS           UVAC_PRJ_SET_LIVEVIEW_SAVEFILE
//#define UVAC_PRJ_SET_THIS           UVAC_PRJ_SET_LIVEVIEW_1V_ONLY
#endif


#if (0)//(UVAC_PRJ_SET_THIS == UVAC_PRJ_SET_LIVEVIEW_SAVEFILE)
//Single Video liveview and movie-recording a file
#define UVAC_2PATH                  DISABLE
#define UVAC_SAVE_FILE              ENABLE
#define UVAC_TEST_FOR_CPU_EXCEPTION_MFK         DISABLE
#elif (UVAC_PRJ_SET_THIS == UVAC_PRJ_SET_LIVEVIEW_1V_ONLY)
//Single Video liveview only
#define UVAC_2PATH                  DISABLE
#define UVAC_SAVE_FILE              DISABLE
#define UVAC_TEST_FOR_CPU_EXCEPTION_MFK         ENABLE  //ENABLE DISABLE
#else
//Two Video liveview
#define UVAC_2PATH                  ENABLE
#define UVAC_SAVE_FILE              DISABLE
#define UVAC_TEST_FOR_CPU_EXCEPTION_MFK         ENABLE  //ENABLE DISABLE
#endif

#define UVAC_WINUSB_INTF            DISABLE   //ENABLE    DISABLE


UVAC_VEND_DEV_DESC gUIUvacDevDesc = {0};
UINT8 gUIUvacVendCmdVer[8] = {'1', '.', '0', '8', '.', '0', '0', '7'};

typedef struct _VEND_CMD_ {
	UINT32 cmd;
	UINT8 data[36]; //<= 60
} VEND_CMD, *PVEND_CMD;

typedef struct {
	UINT32  uiWidth;
	UINT32  uiHeight;
	UINT32  uiVidFrameRate;
	UINT32  uiH264TBR;
	UINT32  uiMJPEGTBR;
} UVC_TARGET_SIZE_PARAM;

static UVC_TARGET_SIZE_PARAM g_UvcVidSizeTable[] = {
	//H264      //MJPEG
	{1920,  1080,   30,     1800 * 1024,  3 * 1800 * 1024},
	{1280,  720,    30,     800 * 1024,   4 * 800 * 1024},
	{ 640,  480,    30,     400 * 1024,   4 * 400 * 1024},
	{ 320,  240,    30,     200 * 1024,   4 * 200 * 1024},
};


/**
* 1.NVT_UI_UVAC_RESO_CNT < UVAC_VID_RESO_MAX_CNT
* 2.Discrete fps:
*       The values of fps[] must be in a decreasing order and 0 shall be put to the last one.
*       The element, fpsCnt, must be correct for the values in fps[]
* 3.Must be a subset of g_MovieRecSizeTable[] for MFK-available.
*
*/
static UVAC_VID_RESO gUIUvacVidReso[NVT_UI_UVAC_RESO_CNT] = {
	{1920,  1080,   1,      30,      0,      0},        //16:9
	{1280,   720,   1,      30,      0,      0},        //16:9
	//{ 848,   480,   1,      30,      0,      0},      //Skype didn't support this resolution.
	{ 640,   480,   1,      30,      0,      0},        //4:3
	{ 320,   240,   1,      30,      0,      0}         //4:3
};

//NVT_UI_UVAC_AUD_SAMPLERATE_CNT <= UVAC_AUD_SAMPLE_RATE_MAX_CNT
UINT32 gUIUvacAudSampleRate[NVT_UI_UVAC_AUD_SAMPLERATE_CNT] = {
	NVT_UI_FREQUENCY_32K
};

_ALIGNED(64) static UINT16 m_UVACSerialStrDesc3[] = {
	0x0320,                             // 20: size of String Descriptor = 32 bytes
	// 03: String Descriptor type
	'9', '6', '6', '8', '0',            // 96611-00000-001 (default)
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

static UVAC_VDO_IN_INFO g_isf_vid_in_info[UVAC_VID_IN_MAX] = {
	{UVAC_VID_IN_1, ISF_OUT1},
	{UVAC_VID_IN_1, ISF_OUT3}
};

//====== Movie Mode Setting ======
static BOOL bSDIsSlow = FALSE;

/*VIEW_SRC gMovie_src_uvac = {
	TRUE, //BYPASS DISP-SRV, DIRECT OUTPUT TO DISPLAY
	VIEW_HANDLE_PRIMARY,
	NULL,
};*/

void xUvac_SetVideoMaxBuf(UINT32 OutPortIndex, PNVT_USBMOVIE_VID_RESO pVidReso);
void xUvac_FreeVideoMaxBuf(UINT32 OutPortIndex);

#if UVAC_USE_SPECIFIED_BUF
static BOOL GetUvcSpecifiedBuf(PMEM_RANGE p_buf)
{
	NVTMPP_VB_BLK  blk;
	BOOL ret = FALSE;
	static NVTMPP_MODULE g_module = MAKE_NVTMPP_MODULE('U', 'V', 'A', 'C', 0, 0, 0, 0);

	nvtmpp_vb_add_module(g_module);
	blk = nvtmpp_vb_get_block(g_module, NVTMPP_VB_INVALID_POOL, IPC_RESERVED_SIZE, NVTMPP_DDR_1);
	if (NVTMPP_VB_INVALID_BLK == blk || NVTMPP_ER_UNKNOWW_MODULE == blk) {
		DBG_ERR("ge blk fail(%d)\r\n", blk);
	} else {
		p_buf->Addr = nvtmpp_vb_block2addr(blk);
		if (p_buf->Addr) {
			p_buf->Size = IPC_RESERVED_SIZE;
			ret = TRUE;
		}
	}
	return ret;
}
#endif
static void GetUvcTBRAspRatio(UINT32 codec, UINT32 width, UINT32 height, UINT32 fps, UINT32 *pTBR, UINT32 *pAspRatio)
{
	UINT32 tbr = 0x400000;
	UINT32 ratio = MEDIAREC_DAR_DEFAULT;
	UINT32 i = 0;
	UINT32 ItemNum = sizeof(g_UvcVidSizeTable) / sizeof(UVC_TARGET_SIZE_PARAM);
	DBG_IND("codec=%d,w=%d,%d,fps=%d,pTBR=0x%x,pAspRatio=0x%x\r\n", codec, width, height, fps, pTBR, pAspRatio);
	for (i = 0; i < ItemNum; i++) {
		if ((g_UvcVidSizeTable[i].uiWidth == width) && (g_UvcVidSizeTable[i].uiHeight == height) && (g_UvcVidSizeTable[i].uiVidFrameRate == fps)) {
			if (codec == MEDIAVIDENC_H264) {
				tbr = g_UvcVidSizeTable[i].uiH264TBR;
			} else { //MJPEG
				tbr = g_UvcVidSizeTable[i].uiMJPEGTBR;
			}
			break;
		}
	}
	if (ItemNum <= i) {
		DBG_ERR("No Match Reso(%d, %d, %d) in g_UvcVidSizeTable\r\n", width, height, fps);
	}
	DBG_IND(":w=%d,%d,fps=%d,tbr=%d,ratio=%d\r\n", width, height, fps, tbr, ratio);
	if (pTBR) {
		*pTBR = tbr;
	} else {
		DBG_ERR("NULL Input pTBR\r\n");
	}
	if (pAspRatio) {
		*pAspRatio = ratio;
	} else {
		DBG_ERR("NULL Input pAspRatio\r\n");
	}

}
void xUvac_SetSDSlow(BOOL IsSlow)
{
	bSDIsSlow = IsSlow;
}
void xUvac_MFKRecordCB(UINT32 uiEventID, UINT32 param)
{
	DBG_IND("evt=%d,p=%d\r\n", uiEventID, param);
}
void xUvac_Movie_IPLCB(IPL_PROC_ID Id, IPL_CBMSG uiMsgID, void *Data)
{
	DBG_IND(":%d,0x%x;\r\n", (UINT32)uiMsgID, (UINT32)Data);
}
void xUvac_Movie_RecordCB(UINT32 uiEventID, UINT32 param)
{
	switch (uiEventID) {
	case MOVREC_EVENT_RESULT_OVERTIME:
		DBG_DUMP("EVENT %d: Overtime!\r\n", uiEventID);
		break;
	case MOVREC_EVENT_RESULT_FULL:
		DBG_ERR("EVENT %d: Full!\r\n", uiEventID);
		break;
	case MOVREC_EVENT_RESULT_HW_ERR:
	case MOVREC_EVENT_RESULT_NOTREADY:
	case MOVREC_EVENT_RESULT_WRITE_ERR:
		DBG_ERR("EVENT %d: HW error!\r\n", uiEventID);
		break;
	case MOVREC_EVENT_THRESHOLD:
		//DBG_IND("EVENT: uiSeconds %d\r\n", (UINT32)param);
		break;
	case MOVREC_EVENT_ENCONE_OK:
		//DBG_IND("ENCODE Ok\r\n");
		break;
	case MOVREC_EVENT_RESULT_NORMAL:
		DBG_IND("NORMAL\r\n");
		break;
	case MOVREC_EVENT_RESULT_SLOW:
	case MOVREC_EVENT_RESULT_CUT_FAIL:
		DBG_ERR("EVENT %d: Slow card!\r\n", uiEventID);
		break;
	case MOVREC_EVENT_AUDBSCB:
		DBG_IND("AUD SCB\r\n");
		break;
	case MOVREC_EVENT_IMEDIRECT_STOPH264:
		//SMediaRec_Vtrig_NoKickH264(1);
		DBG_DUMP("^R======Time stop h264!!!!\r\n");
		break;
	default:
		//DBG_DUMP(":%d, 0x%x;\r\n",(UINT32)uiEventID, (UINT32)param);
		break;
	}
}
void xUvac_Movie_CaptureCB(IMG_CAP_CBMSG Msg, void *Data)
{
	DBG_IND(":%d, 0x%x; Do nothing now.\r\n", (UINT32)Msg, (UINT32)Data);
}

void xUvac_MFKRegCB(void)
{
	ImageUnit_Begin(ISF_IPL(g_isf_vid_in_info[0].vid_in), 0);
	ImageUnit_SetParam(ISF_CTRL, IMAGEPIPE_PARAM_IPL_CB, (UINT32)xUvac_Movie_IPLCB);
	ImageUnit_End();
}

__inline void xUvac_SetMFKRecParam(PNVT_USBMOVIE_VID_RESO pVidReso1, PNVT_USBMOVIE_VID_RESO pVidReso2)
{
	UINT32  uiWidth, uiHeight;
	UINT32  uiVidCodec, uiVidFrameRate, uiH264GopType;
	UINT32  uiTargetBitrate = 0;
	UINT32  uiDispAspectRatio = 0;
	UINT32  uiAudCodec, uiAudSampleRate, uiAudChannelType, uiAudChannelNum;
#if UVAC_2PATH
	//UINT32  uiFileType2, uiWidth2, uiHeight2, uiTargetBitrate2, uiDispAspectRatio2, uiVidCodec2, uiVidFrameRate2;
	UINT32  uiWidth2, uiHeight2, uiTargetBitrate2, uiDispAspectRatio2, uiVidCodec2, uiVidFrameRate2;
	UINT32 ratioW2 = NVT_USBMOVIE_CROPRATIO_W_16;
	UINT32 ratioH2 = NVT_USBMOVIE_CROPRATIO_H_9;
#endif

	UINT32 ratioW = NVT_USBMOVIE_CROPRATIO_W_16;
	UINT32 ratioH = NVT_USBMOVIE_CROPRATIO_H_9;
	DBG_IND(":pVidReso1=0x%x,pVidReso2=0x%x,senCnt=%d\r\n", pVidReso1, pVidReso2, nSensor);

	/*if (nSensor != 1) {
		DBG_ERR("Not support number of sensor=%d\r\n", nSensor);
		nSensor = 1;
	}*/

	if (((1280 == pVidReso1->width) && (720 == pVidReso1->height)) || \
		((1920 == pVidReso1->width) && (1080 == pVidReso1->height))|| \
		((848 == pVidReso1->width) && (480 == pVidReso1->height))) {
		ratioW = NVT_USBMOVIE_CROPRATIO_W_16;
		ratioH = NVT_USBMOVIE_CROPRATIO_H_9;
	} else if (((640 == pVidReso1->width) && (480 == pVidReso1->height)) || \
			((320 == pVidReso1->width) && (240 == pVidReso1->height))) {
			ratioW = NVT_USBMOVIE_CROPRATIO_W_4;
			ratioH = NVT_USBMOVIE_CROPRATIO_H_3;
	} else {
		DBG_WRN("Need to add new: %d, %d, %d, %d\r\n", pVidReso1->width, pVidReso1->height, pVidReso1->fps, pVidReso1->codec);
	}

	#if UVAC_2PATH
	if (((1280 == pVidReso2->width) && (720 == pVidReso2->height)) || \
		((1920 == pVidReso2->width) && (1080 == pVidReso2->height))|| \
		((848 == pVidReso2->width) && (480 == pVidReso2->height))) {
		ratioW2 = NVT_USBMOVIE_CROPRATIO_W_16;
		ratioH2 = NVT_USBMOVIE_CROPRATIO_H_9;
	} else if (((640 == pVidReso2->width) && (480 == pVidReso2->height)) || \
			((320 == pVidReso2->width) && (240 == pVidReso2->height))) {
			ratioW2 = NVT_USBMOVIE_CROPRATIO_W_4;
			ratioH2 = NVT_USBMOVIE_CROPRATIO_H_3;
	} else {
		DBG_WRN("Need to add new: %d, %d, %d, %d\r\n", pVidReso2->width, pVidReso2->height, pVidReso2->fps, pVidReso2->codec);
	}
	#endif

	DBG_IND("@@Codec1=%d, Codec2=%d@@\r\n", pVidReso1->codec, pVidReso2->codec);

	uiAudCodec  = AUDENC_OUTPUT_UNCOMPRESSION; //MEDIAREC_ENC_PCM

	if (pVidReso1->codec == UVAC_VIDEO_FORMAT_H264) {
		uiVidCodec  = MEDIAVIDENC_H264;
	} else {
		uiVidCodec  = MEDIAVIDENC_MJPG;
	}

	DBG_IND("uiAudCodec=%d\r\n", uiAudCodec);
	uiWidth             = pVidReso1->width;         	 // image width
	uiHeight            = pVidReso1->height;        	 // image height
	uiVidFrameRate      = pVidReso1->fps;         	     // video frame rate
	GetUvcTBRAspRatio(uiVidCodec, uiWidth, uiHeight, uiVidFrameRate, &uiTargetBitrate, &uiDispAspectRatio);
	uiH264GopType       = MEDIAREC_GOP_IPONLY;           // H.264 GOP type (IP only)
	//uiH264GopType       = MEDIAREC_H264GOP_IPB;          // H.264 GOP type (IPB)
	DBG_DUMP("CB-V1 W=%d, %d, fps=%d, TBR=%d, Ratio=%d, codec=%d, Gop=%d\r\n", \
			 uiWidth, uiHeight, uiVidFrameRate, uiTargetBitrate, uiDispAspectRatio, uiVidCodec, uiH264GopType);

	#if UVAC_2PATH
	if (pVidReso2->codec == UVAC_VIDEO_FORMAT_H264) {
		uiVidCodec2  = MEDIAVIDENC_H264;
	} else {
		uiVidCodec2  = MEDIAVIDENC_MJPG;
	}

	uiWidth2             = pVidReso2->width;         	 // image width
	uiHeight2            = pVidReso2->height;        	 // image height
	uiVidFrameRate2      = pVidReso2->fps;         	     // video frame rate
	GetUvcTBRAspRatio(uiVidCodec2, uiWidth2, uiHeight2, uiVidFrameRate2, &uiTargetBitrate2, &uiDispAspectRatio2);
	uiH264GopType       = MEDIAREC_GOP_IPONLY;           // H.264 GOP type (IP only)
	//uiH264GopType       = MEDIAREC_H264GOP_IPB;          // H.264 GOP type (IPB)
	DBG_DUMP("CB-V2 W=%d, %d, fps=%d, TBR=%d, Ratio=%d, codec=%d, Gop=%d\r\n", \
			 uiWidth2, uiHeight2, uiVidFrameRate2, uiTargetBitrate2, uiDispAspectRatio2, uiVidCodec2, uiH264GopType);
	#endif

	//ui3DNRLevel         = 0;           // H.264 3DNR disable
	uiAudSampleRate     = gUIUvacAudSampleRate[0];       // audio sampling rate
	uiAudChannelType    = AUDIO_CH_STEREO;               // audio channel type
	uiAudChannelNum 	= 2;


	//---------------------------------------------------------------------------------------------
	// set video parameters
	//---------------------------------------------------------------------------------------------
	//select parameter for input path 0 (default)

	// IPL
	ImageUnit_Begin(ISF_IPL(g_isf_vid_in_info[0].vid_in), 0);
		ImageUnit_SetVdoAspectRatio(ISF_IN1, ratioW, ratioH);
	ImageUnit_End();

	ImageUnit_Begin(&ISF_UserProc, 0);
		ImageUnit_SetVdoImgSize(ISF_IN1, uiWidth, uiHeight);
	ImageUnit_End();

	#if UVAC_2PATH
	ImageUnit_Begin(ISF_IPL(g_isf_vid_in_info[1].vid_in), 0);
		ImageUnit_SetVdoAspectRatio(ISF_IN1, ratioW2, ratioH2);
	ImageUnit_End();

	ImageUnit_Begin(&ISF_UserProc, 0);
		ImageUnit_SetVdoImgSize(ISF_IN2, uiWidth2, uiHeight2);
	ImageUnit_End();
	#endif

	ImageUnit_Begin(&ISF_VdoEnc, 0);
		ImageUnit_SetVdoImgSize(ISF_IN1, uiWidth, uiHeight);
		//ImageUnit_SetVdoAspectRatio(ISF_IN1, ratioW, ratioH);
		ImageUnit_SetParam(ISF_OUT1, VDOENC_PARAM_CODEC, uiVidCodec);
		if (uiVidCodec == MEDIAVIDENC_MJPG) {
			#if (VIDEO_FUNC_MJPG == ENABLE)
			MP_VDOENC_ENCODER *pObj = MP_MjpgEnc_getVideoObject(0);
			ImageUnit_SetParam(ISF_OUT1, VDOENC_PARAM_ENCODER_OBJ, (UINT32)pObj);
			#else
			ImageUnit_SetParam(ISF_OUT1, VDOENC_PARAM_ENCODER_OBJ, 0);
			DBG_ERR("Vdo codec = %d not support\r\n", uiVidCodec);
			#endif
		} else {
			#if (VIDEO_FUNC_H264 == ENABLE)
			MP_VDOENC_ENCODER *pObj = MP_H264Enc_getVideoObject(0);
			ImageUnit_SetParam(ISF_OUT1, VDOENC_PARAM_ENCODER_OBJ, (UINT32)pObj);
			#else
			ImageUnit_SetParam(ISF_OUT1, VDOENC_PARAM_ENCODER_OBJ, 0);
			DBG_ERR("Vdo codec = %d not support\r\n", uiVidCodec);
			#endif
		}

		ImageUnit_SetParam(ISF_OUT1, VDOENC_PARAM_GOPTYPE, MEDIAREC_GOP_IPONLY);

		ImageUnit_SetParam(ISF_OUT1, VDOENC_PARAM_RECFORMAT, MEDIAREC_LIVEVIEW);
		///init QP , for only first GOP, please do not change these value
		ImageUnit_SetParam(ISF_OUT1, VDOENC_PARAM_INITQP, 20);
		ImageUnit_SetParam(ISF_OUT1, VDOENC_PARAM_MINQP, 20);
		ImageUnit_SetParam(ISF_OUT1, VDOENC_PARAM_MAXQP, 51);

		ImageUnit_SetVdoFramerate(ISF_IN1, ISF_VDO_FRC(uiVidFrameRate, 1));
		ImageUnit_SetParam(ISF_OUT1, VDOENC_PARAM_GOPNUM, 15);
		ImageUnit_SetParam(ISF_OUT1, VDOENC_PARAM_TARGETRATE, uiTargetBitrate);
		ImageUnit_SetParam(ISF_OUT1, VDOENC_PARAM_ENCBUF_MS, 1500);
		ImageUnit_SetParam(ISF_OUT1, VDOENC_PARAM_ENCBUF_RESERVED_MS, 1300);

		#if UVAC_2PATH
		ImageUnit_SetVdoImgSize(ISF_IN2, uiWidth2, uiHeight2);
		ImageUnit_SetParam(ISF_OUT2, VDOENC_PARAM_CODEC, uiVidCodec2);
		if (uiVidCodec2 == MEDIAVIDENC_MJPG) {
			#if (VIDEO_FUNC_MJPG == ENABLE)
			MP_VDOENC_ENCODER *pObj = MP_MjpgEnc_getVideoObject(1);
			ImageUnit_SetParam(ISF_OUT2, VDOENC_PARAM_ENCODER_OBJ, (UINT32)pObj);
			#else
			ImageUnit_SetParam(ISF_OUT2, VDOENC_PARAM_ENCODER_OBJ, 0);
			DBG_ERR("Vdo codec = %d not support\r\n", uiVidCodec);
			#endif
		} else {
			#if (VIDEO_FUNC_H264 == ENABLE)
			MP_VDOENC_ENCODER *pObj = MP_H264Enc_getVideoObject(1);
			ImageUnit_SetParam(ISF_OUT2, VDOENC_PARAM_ENCODER_OBJ, (UINT32)pObj);
			#else
			ImageUnit_SetParam(ISF_OUT2, VDOENC_PARAM_ENCODER_OBJ, 0);
			DBG_ERR("Vdo codec = %d not support\r\n", uiVidCodec);
			#endif
		}
		ImageUnit_SetParam(ISF_OUT2, VDOENC_PARAM_GOPTYPE, MEDIAREC_GOP_IPONLY);

		ImageUnit_SetParam(ISF_OUT2, VDOENC_PARAM_RECFORMAT, MEDIAREC_LIVEVIEW);
		///init QP , for only first GOP, please do not change these value
		ImageUnit_SetParam(ISF_OUT2, VDOENC_PARAM_INITQP, 20);
		ImageUnit_SetParam(ISF_OUT2, VDOENC_PARAM_MINQP, 20);
		ImageUnit_SetParam(ISF_OUT2, VDOENC_PARAM_MAXQP, 51);

		ImageUnit_SetVdoFramerate(ISF_IN2, ISF_VDO_FRC(uiVidFrameRate2, 1));
		ImageUnit_SetParam(ISF_OUT2, VDOENC_PARAM_GOPNUM, 15);
		ImageUnit_SetParam(ISF_OUT2, VDOENC_PARAM_TARGETRATE, uiTargetBitrate2);
		ImageUnit_SetParam(ISF_OUT2, VDOENC_PARAM_ENCBUF_MS, 1500);
		ImageUnit_SetParam(ISF_OUT2, VDOENC_PARAM_ENCBUF_RESERVED_MS, 1300);
		#endif
	ImageUnit_End();
	
#if(STAMP_FUNC==ENABLE)
   {

        STAMP_COLOR StampColorBg = {RGB_GET_Y( 16,  16,  16), RGB_GET_U( 16,  16,  16), RGB_GET_V( 16,  16,  16)}; // date stamp background color
        STAMP_COLOR StampColorFr = {RGB_GET_Y( 16,  16,  16), RGB_GET_U( 16,  16,  16), RGB_GET_V( 16,  16,  16)}; // date stamp frame color
        STAMP_COLOR StampColorFg = {RGB_GET_Y(224, 224, 192), RGB_GET_U(224, 224, 192), RGB_GET_V(224, 224, 192)}; // date stamp foreground color

        UINT32      uiStampAddr;
        UINT32      uiVidEncId=0;
        UINT32 		i, mask, sensor_enable;

        mask = 1;
		sensor_enable = System_GetEnableSensor();
        for (i = 0; i < SENSOR_CAPS_COUNT; i++) 
		{
            if ((sensor_enable & mask)) 
			{
                uiVidEncId = i;
                //DBG_DUMP("main uiVidEncId=%d\r\n",uiVidEncId);
                DBGD(uiWidth);
                DBGD(uiHeight);
                uiStampAddr=MovieStamp_GetBufAddr(uiVidEncId, MovieStamp_CalcBufSize(uiWidth, uiHeight));//POOL_SIZE_DATEIMPRINT/2);

                MovieStamp_SetBuffer(uiVidEncId, uiStampAddr, MovieStamp_CalcBufSize(uiWidth, uiHeight));//POOL_SIZE_DATEIMPRINT/2);

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
                    uiWidth,
                    uiHeight,
#if defined (WATERLOGO_FUNCTION) && (WATERLOGO_FUNCTION == ENABLE)
            (WATERLOGO_BUFFER *)&g_WaterLogo);
#else
                    NULL);
#endif
            }

			mask <<= 1;
		}

         MovieStamp_Enable();
    }
#endif
	//---------------------------------------------------------------------------------------------
	// set audio parameters
	//---------------------------------------------------------------------------------------------
	// Audio

	MP_AUDENC_ENCODER *pObj = MP_PCMEnc_getAudioEncodeObject();

	ImageUnit_Begin(&ISF_AudIn, 0);
		ImageUnit_SetParam(ISF_IN1, AUDIN_PARAM_CHANNEL, uiAudChannelType);
	ImageUnit_End();

	ImageUnit_Begin(&ISF_AudEnc, 0);
		ImageUnit_SetParam(ISF_IN1, AUDENC_PARAM_RECFORMAT, MEDIAREC_LIVEVIEW); //no record file
		ImageUnit_SetParam(ISF_IN1, AUDENC_PARAM_CODEC, AUDENC_ENCODER_PCM);
		ImageUnit_SetParam(ISF_IN1, AUDENC_PARAM_ENCODER_OBJ, (UINT32)pObj);
		ImageUnit_SetAudSample(ISF_IN1, 16, uiAudChannelNum, uiAudSampleRate);
		ImageUnit_SetParam(ISF_OUT1, AUDENC_PARAM_PORT_OUTPUT_FMT, uiAudCodec);
	ImageUnit_End();
}

void xUvac_ConfigMovieSizeCB(PNVT_USBMOVIE_VID_RESO pVidReso1, PNVT_USBMOVIE_VID_RESO pVidReso2)
{
	xUvac_SetMFKRecParam(pVidReso1, pVidReso2);
}

void xUvac_SetVideoMaxBuf(UINT32 OutPortIndex, PNVT_USBMOVIE_VID_RESO pVidReso)
{
	NMI_VDOENC_MAX_MEM_INFO MaxMemInfo = {0};
	UINT32 target_br = 0;
	UINT32 disp_aspect_ratio = 0;

	GetUvcTBRAspRatio(MEDIAVIDENC_MJPG, pVidReso->width, pVidReso->height, pVidReso->fps, &target_br, &disp_aspect_ratio);

	MaxMemInfo.uiCodec          = MEDIAVIDENC_H264;
	MaxMemInfo.uiWidth          = pVidReso->width;
	MaxMemInfo.uiHeight         = pVidReso->height;
	MaxMemInfo.uiTargetByterate = target_br;
	MaxMemInfo.uiEncBufMs       = 1500;
	MaxMemInfo.uiRecFormat      = MEDIAREC_LIVEVIEW;
	MaxMemInfo.uiSVCLayer       = 0;
	MaxMemInfo.uiLTRInterval	= 0;
	MaxMemInfo.uiRotate         = 0;
	MaxMemInfo.bAllocSnapshotBuf= 0;

	ImageUnit_Begin(&ISF_VdoEnc, 0);
	ImageUnit_SetParam(OutPortIndex, VDOENC_PARAM_MAX_MEM_INFO, (UINT32) &MaxMemInfo);
	ImageUnit_End();
}

void xUvac_FreeVideoMaxBuf(UINT32 OutPortIndex)
{
	NMI_VDOENC_MAX_MEM_INFO MaxMemInfo = {0};

	MaxMemInfo.bRelease = TRUE;

	ImageUnit_Begin(&ISF_VdoEnc, 0);
	ImageUnit_SetParam(OutPortIndex, VDOENC_PARAM_MAX_MEM_INFO, (UINT32) &MaxMemInfo);
	ImageUnit_End();
}

void xUvac_InitSetMFK(void)
{
	NVT_USBMOVIE_VID_RESO vidReso1, vidReso2;

	xUvac_MFKRegCB();

	vidReso1.width = vidReso2.width = gUIUvacVidReso[NVT_UI_UVAC_RESO_IDX_DEF].width;
	vidReso1.height = vidReso2.height = gUIUvacVidReso[NVT_UI_UVAC_RESO_IDX_DEF].height;
	vidReso1.fps = vidReso2.fps = gUIUvacVidReso[NVT_UI_UVAC_RESO_IDX_DEF].fps[0];
	vidReso1.codec = vidReso2.codec =  UVAC_VIDEO_FORMAT_H264;

	xUvac_SetVideoMaxBuf(ISF_OUT1, &vidReso1);
	#if UVAC_2PATH
	xUvac_SetVideoMaxBuf(ISF_OUT2, &vidReso2);
	#endif

	xUvac_SetMFKRecParam(&vidReso1, &vidReso2);

	// IPL
	ImageUnit_Begin(ISF_IPL(g_isf_vid_in_info[0].vid_in), 0);
		/////set IPL SIZE
		ImageUnit_SetVdoImgSize(ISF_IN1, UVAC_MAX_WIDTH, UVAC_MAX_HEIGHT); //user perferred IPL input w,h (= main stream size)
		/////set IPL FPS
		ImageUnit_SetVdoFramerate(ISF_IN1, ISF_VDO_FRC(UVAC_MAX_FRAMERATE,1));//user perferred IPL input fps (= main stream fps)
		ImageUnit_SetVdoAspectRatio(ISF_IN1, 16, 9);
	ImageUnit_End();

	#if UVAC_2PATH
	ImageUnit_Begin(ISF_IPL(g_isf_vid_in_info[1].vid_in), 0);
		/////set IPL SIZE
		ImageUnit_SetVdoImgSize(ISF_IN1, UVAC_MAX_WIDTH, UVAC_MAX_HEIGHT); //user perferred IPL input w,h (= main stream size)
		/////set IPL FPS
 		ImageUnit_SetVdoFramerate(ISF_IN1, ISF_VDO_FRC(UVAC_MAX_FRAMERATE,1));//user perferred IPL input fps (= main stream fps)
		ImageUnit_SetVdoAspectRatio(ISF_IN1, 16, 9);
	ImageUnit_End();
	#endif

}
//======= end movie mode related setting  ======


//0:OK
//1...:TBD
static UINT32 xUvacVendReqCB(PUVAC_VENDOR_PARAM pParam)
{
	if (pParam) {
		DBG_IND("bHostToDevice       = 0x%x\r\n", pParam->bHostToDevice);
		DBG_IND("uiReguest       = 0x%x\r\n",     pParam->uiReguest);
		DBG_IND("uiValue       = 0x%x\r\n",       pParam->uiValue);
		DBG_IND("uiIndex       = 0x%x\r\n",       pParam->uiIndex);
		DBG_IND("uiDataAddr       = 0x%x\r\n",    pParam->uiDataAddr);
		DBG_IND("uiDataSize       = 0x%x\r\n",    pParam->uiDataSize);
	} else {
		DBG_ERR(" Input parameter NULL\r\n");
	}
	return 0;
}
//0:OK
//1...:TBD
static UINT32 xUvacVendReqIQCB(PUVAC_VENDOR_PARAM pParam)
{
	if (pParam) {
		DBG_IND("bHostToDevice       = 0x%x\r\n", pParam->bHostToDevice);
		DBG_IND("uiReguest       = 0x%x\r\n",     pParam->uiReguest);
		DBG_IND("uiValue       = 0x%x\r\n",       pParam->uiValue);
		DBG_IND("uiIndex       = 0x%x\r\n",       pParam->uiIndex);
		DBG_IND("uiDataAddr       = 0x%x\r\n",    pParam->uiDataAddr);
		DBG_IND("uiDataSize       = 0x%x\r\n",    pParam->uiDataSize);
	} else {
		DBG_ERR(" Input parameter NULL\r\n");
	}
	return 0;
}
static void xUSBMakerInit_UVAC(UVAC_VEND_DEV_DESC *pUVACDevDesc)
{
	pUVACDevDesc->pManuStringDesc = (UVAC_STRING_DESC *)m_UVACManuStrDesc;
	pUVACDevDesc->pProdStringDesc = (UVAC_STRING_DESC *)m_UVACProdStrDesc;

	pUVACDevDesc->pSerialStringDesc = (UVAC_STRING_DESC *)m_UVACSerialStrDesc3;

	pUVACDevDesc->VID = USB_VID;
	pUVACDevDesc->PID = USB_PID_PCCAM;

	pUVACDevDesc->fpVendReqCB = xUvacVendReqCB;
	pUVACDevDesc->fpIQVendReqCB = xUvacVendReqIQCB;

}

UINT8 xUvacWinUSBCB(UINT32 ControlCode, UINT8 CS, UINT8 *pDataAddr, UINT32 *pDataLen)
{
	DBG_IND("xUvacWinUSBCB ctrl=0x%x, cs=0x%x, pData=0x%x, len=%d\r\n", ControlCode, CS, pDataAddr, *pDataLen);
	return TRUE;
}
UINT8 xUvacEUVendCmdCB_Idx1(UINT32 ControlCode, UINT8 CS, UINT8 *pDataAddr, UINT32 *pDataLen)
{
	static UINT32 vendCmd = NVT_UI_UVAC_VENDCMD_CURCMD;
	UINT8 vendCBRet = TRUE;
	UINT32 cmdLen = sizeof(VEND_CMD);
	PVEND_CMD pVendCmd = (PVEND_CMD)pDataAddr;
	UINT8 *pData = pDataAddr;

	switch (CS) {
	case GET_INFO:
		DBG_IND("[VendCmd --GET_INFO] ctrl=0x%x, cs=0x%x, pData=0x%x, len=%d, vendCmd=%d\r\n", ControlCode, CS, pData, *pDataLen, vendCmd);
		*pData = 0x03;
		*pDataLen = 1;
		vendCBRet = TRUE;
		break;
	case GET_LEN:
		DBG_IND("[VendCmd --GET_LEN] ctrl=0x%x, cs=0x%x, pData=0x%x, len=%d, vendCmd=%d\r\n", ControlCode, CS, pData, *pDataLen, vendCmd);
		*pData++ = (cmdLen & 0xFF);
		*pData++ = ((cmdLen >> 8) & 0xFF);
		*pDataLen = 2;
		vendCBRet = TRUE;
		break;
	case GET_RES:
		DBG_IND("[VendCmd --GET_RES] ctrl=0x%x, cs=0x%x, pData=0x%x, len=%d, vendCmd=%d\r\n", ControlCode, CS, pData, *pDataLen, vendCmd);
		*pData = 0x01;
		*pDataLen = 1;
		vendCBRet = TRUE;
		break;
	case GET_MAX:
		DBG_IND("[VendCmd --GET_MAX] ctrl=0x%x, cs=0x%x, pData=0x%x, len=%d, vendCmd=%d\r\n", ControlCode, CS, pData, *pDataLen, vendCmd);
		memset(pData, 0xff, cmdLen);
		*pDataLen = cmdLen;
		vendCBRet = TRUE;
		break;
	case GET_MIN:
		DBG_IND("[VendCmd --GET_MIN] ctrl=0x%x, cs=0x%x, pData=0x%x, len=%d, vendCmd=%d\r\n", ControlCode, CS, pData, *pDataLen, vendCmd);
		break;
	case GET_DEF:
		DBG_IND("[VendCmd --GET_DEF] ctrl=0x%x, cs=0x%x, pData=0x%x, len=%d, vendCmd=%d\r\n", ControlCode, CS, pData, *pDataLen, vendCmd);
		memset(pData, 0, cmdLen);
		*pDataLen = cmdLen;
		vendCBRet = TRUE;
		break;
	case GET_CUR:
		DBG_IND("[VendCmd --GET_CUR] ctrl=0x%x, cs=0x%x, pData=0x%x, len=%d, vendCmd=%d\r\n", ControlCode, CS, pData, *pDataLen, vendCmd);
		if (NVT_UI_UVAC_VENDCMD_CURCMD == vendCmd) {
			vendCmd = pVendCmd->data[0];
			*pDataLen = cmdLen;
			vendCBRet = TRUE;
		} else if (UVAC_VENDCMD_VERSION == vendCmd) {
			memcpy((void *)pData, gUIUvacVendCmdVer, 8);
			*pDataLen = cmdLen;
			vendCBRet = TRUE;
		} else {
			//TBD: temporarely no needed
			vendCBRet = FALSE;
		}
		break;
	case SET_CUR:
		DBG_IND("[VendCmd --SET_CUR] ctrl=0x%x, cs=0x%x, pData=0x%x, len=%d, vendCmd=%d\r\n", ControlCode, CS, pData, *pDataLen, vendCmd);
		if (NVT_UI_UVAC_VENDCMD_CURCMD == vendCmd) {
			vendCmd = pVendCmd->data[0];
			vendCBRet = TRUE;
		} else if (UVAC_VENDCMD_VERSION == vendCmd) {
			vendCBRet = FALSE;//Version can not be set.
		} else {
			//TBD: temporarely no needed
			vendCBRet = FALSE;
		}
		break;
	default:
		DBG_IND("[VendCmd --Unknow] ctrl=0x%x, cs=0x%x, pData=0x%x, len=%d, vendCmd=%d\r\n", ControlCode, CS, pData, *pDataLen, vendCmd);
		vendCBRet = FALSE;
		break;
	}
	return vendCBRet;
}
#if (THIS_DBGLVL>=5 &&(UVAC_CT_PU_FUNC))
static void DumpMemory(UINT32 Addr, UINT32 Size, UINT32 Alignment)
{
	UINT32 i;
	UINT8 *pBuf = (UINT8 *)Addr;
	for (i = 0; i < Size; i++) {
		if (i % Alignment == 0) {
			DBG_DUMP("\r\n");
		}
		DBG_DUMP("0x%02X ", *(pBuf + i));
	}
	DBG_DUMP("\r\n");
}
#endif
#if UVAC_CT_PU_FUNC
static BOOL xUvacCT_CB(UINT32 CS, UINT8 request, UINT8 *pData, UINT32 *pDataLen)
{
	BOOL ret = FALSE;

	#if (THIS_DBGLVL>=5)
	DBG_DUMP("%s: CS=0x%x, request = 0x%X,  pData=0x%x, len=%d\r\n", __func__, CS, request, pData, *pDataLen);
	if (SET_CUR == request) {
		DumpMemory((UINT32) pData, *pDataLen, 16);
	}
	#endif
	switch (CS){
	case CT_AE_MODE_CONTROL://0x02:
		//ret = TRUE;//if supported
		break;
	case CT_AE_PRIORITY_CONTROL://0x03:
		//ret = TRUE;//if supported
		break;
	//...
	default:
		break;
	}

	if (ret && request == SET_CUR) {
		*pDataLen = 0;
	}
	return ret;
}
static BOOL xUvac_PU_Brightness(UINT8 request, UINT8 *pData, UINT32 *pDataLen)
{
	BOOL ret = TRUE;

	//just a sample for range:0~200   default:100
	switch(request)
    {
    	case GET_INFO:
    		*pDataLen = 1;
            pData[0] = SUPPORT_GET_REQUEST | SUPPORT_SET_REQUEST;
            break;
        case GET_MIN:
        	*pDataLen = 2;
	        pData[0] = 0;
	        pData[1] = 0;
        	break;
        case GET_MAX:
        	*pDataLen = 2;
	        pData[0] = 0xC8;//200
	        pData[1] = 0;
        	break;
        case GET_RES:
            *pDataLen = 2;
	        pData[0] = 1;
	        pData[1] = 0;
            break;
        case GET_DEF:
            *pDataLen = 2;
	        pData[0] = 0x64;//100
	        pData[1] = 0;
            break;
        case GET_CUR:
            *pDataLen = 2;
            //return current xxx value for USB Host
	        pData[0] = 0x6;//just a hard code sample
	        pData[1] = 0;
            break;
        case SET_CUR:
			DBG_DUMP("%s: pData=0x%X 0x%X =%d\r\n", __func__, pData[1], pData[0], (pData[1]<<8)+pData[0] );
			//set brightness
            break;
        default:
			ret = FALSE;
            break;
    }
    return ret;
}
static BOOL xUvac_PU_Contrast(UINT8 request, UINT8 *pData, UINT32 *pDataLen)
{
	BOOL ret = TRUE;

	//just a sample for range:0~200   default:100
	switch(request)
    {
    	case GET_INFO:
    		*pDataLen = 1;
            pData[0] = SUPPORT_GET_REQUEST | SUPPORT_SET_REQUEST;
            break;
        case GET_MIN:
        	*pDataLen = 2;
	        pData[0] = 0;
	        pData[1] = 0;
        	break;
        case GET_MAX:
        	*pDataLen = 2;
	        pData[0] = 0xC8;//200
	        pData[1] = 0;
        	break;
        case GET_RES:
            *pDataLen = 2;
	        pData[0] = 1;
	        pData[1] = 0;
            break;
        case GET_DEF:
            *pDataLen = 2;
	        pData[0] = 0x64;//100
	        pData[1] = 0;
            break;
        case GET_CUR:
            *pDataLen = 2;
            //return current xxx value for USB Host
	        pData[0] = 0x6;//just a hard code sample
	        pData[1] = 0;
            break;
        case SET_CUR:
			DBG_DUMP("%s: pData=0x%X 0x%X =%d\r\n", __func__, pData[1], pData[0], (pData[1]<<8)+pData[0] );
			//set contrast
            break;
        default:
			ret = FALSE;
            break;
    }
    return ret;
}
static BOOL xUvac_PU_PowerLineFreq(UINT8 request, UINT8 *pData, UINT32 *pDataLen)
{
	BOOL ret = TRUE;

	switch(request)
    {
    	case GET_INFO:
    		*pDataLen = 1;
            pData[0] = SUPPORT_GET_REQUEST | SUPPORT_SET_REQUEST;
            break;
        case GET_DEF:
            *pDataLen = 1;
	        pData[0] = 2;

            break;
        case GET_CUR:
            *pDataLen = 1;
            //return current xxx value for USB Host
	        pData[0] = 0x2;//just a hard code sample
            break;
        case SET_CUR:
			DBG_DUMP("%s: pData[0]=0x%X \r\n", __func__, pData[0]);
			//set contrast
            break;
        default:
			ret = FALSE;
            break;
    }
    return ret;
}
static BOOL xUvacPU_CB(UINT32 CS, UINT8 request, UINT8 *pData, UINT32 *pDataLen)
{
	BOOL ret = FALSE;

	#if (THIS_DBGLVL>=5)
	DBG_DUMP("%s: CS=0x%x, request = 0x%X,  pData=0x%x, len=%d\r\n", __func__, CS, request, pData, *pDataLen);
	if (SET_CUR == request) {
		DumpMemory((UINT32) pData, *pDataLen, 16);
	}
	#endif
	switch (CS){
	case PU_BACKLIGHT_COMPENSATION_CONTROL://0x01
		//ret = TRUE;//if supported
		break;
	case PU_BRIGHTNESS_CONTROL://0x02:
		ret = xUvac_PU_Brightness(request, pData, pDataLen);
		break;
	case PU_CONTRAST_CONTROL://0x03:
		ret = xUvac_PU_Contrast(request, pData, pDataLen);
		break;
	case PU_POWER_LINE_FREQUENCY_CONTROL:
		ret = xUvac_PU_PowerLineFreq(request, pData, pDataLen);
		break;
	//...
	default:
		break;
	}

	if (ret && request == SET_CUR) {
		*pDataLen = 0;
	}
	return ret;
}
#endif

#if(STAMP_FUNC==ENABLE)
void UsbCamExe_UserEventCb(UINT32 id, USBMOVIE_USER_CB_EVENT event_id, UINT32 value)
{
	switch ((UINT32)event_id) {
	case USBMOVIE_USER_CB_EVENT_STAMP_CB: {
    		if (value) {
					UINT32 uiVidCodec;
    	            MP_VDOENC_YUV_SRC* p_yuv_src   = NULL;
    	            p_yuv_src = (MP_VDOENC_YUV_SRC*)value;

					ImageUnit_Begin(&ISF_VdoEnc, 0);
			        uiVidCodec = ImageUnit_GetParam(&ISF_VdoEnc, ISF_OUT1, VDOENC_PARAM_CODEC);
			        DBG_IND("VidCodec:%d\r\n", uiVidCodec);
			        ImageUnit_End();
					
					if(uiVidCodec == MEDIAVIDENC_MJPG) {
						videosprite_draw_ext(p_yuv_src->uiVidPathID,
							        p_yuv_src->uiYAddr,
							        p_yuv_src->uiCbAddr,
							        p_yuv_src->uiCrAddr,
							        p_yuv_src->uiYLineOffset,
							        p_yuv_src->uiWidth,
							        p_yuv_src->uiHeight);
					} else {
    	                videosprite_draw_btn_ext(p_yuv_src->uiVidPathID,
    	                p_yuv_src->uiYAddr,
    	                p_yuv_src->uiCbAddr,
    	                p_yuv_src->uiCrAddr,
    	                p_yuv_src->uiYLineOffset,
    	                p_yuv_src->uiWidth,
    	                p_yuv_src->uiHeight);
    	            }    	            
        	    }
        	}		
		break;
		
	default:
		break;
	}
}
#endif

static void xUvac_AE_SetUIInfo(UINT32 index, UINT32 value)
{
	UINT32 i;

	for (i=0;i<IPL_ID_MAX_NUM;i++){
		if (System_GetEnableSensor() & (SENSOR_1 << i)){
			ae_set_ui_info(IPL_PATH(i), index, value);
		}
	}
}

static INT32 UVACExe_InitCommonMem(void)
{

	NVTMPP_ER        ret;
	NVTMPP_VB_CONF_S st_conf;

	memset(&st_conf, 0, sizeof(NVTMPP_VB_CONF_S));
	st_conf.ddr_mem[0].Addr = OS_GetMempoolAddr(POOL_ID_APP);
	st_conf.ddr_mem[0].Size = OS_GetMempoolSize(POOL_ID_APP);

	st_conf.max_pool_cnt = 64;

	st_conf.common_pool[0].blk_size = 1920 * 1080 * 3 / 2 + 1024;
	st_conf.common_pool[0].blk_cnt = 4 * 3;
	st_conf.common_pool[0].ddr = NVTMPP_DDR_1;
	st_conf.common_pool[1].blk_size = 1080 * 720 * 3 / 2 + 1024;
	st_conf.common_pool[1].blk_cnt = 4 * 3;
	st_conf.common_pool[1].ddr = NVTMPP_DDR_1;
#if UVAC_USE_SPECIFIED_BUF
	st_conf.common_pool[2].blk_size = IPC_RESERVED_SIZE + 1024;
	st_conf.common_pool[2].blk_cnt = 1;
	st_conf.common_pool[2].ddr = NVTMPP_DDR_1;
	st_conf.use_reserved_mem = TRUE;
#endif
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
	return 0;

}


INT32 UVACExe_ExitCommonMem(void)
{
	NVTMPP_ER        ret;
	ret = nvtmpp_vb_exit();
	if (NVTMPP_ER_OK != ret) {
		DBG_ERR("nvtmpp init vb err: %d\r\n", ret);
		return -1;
	}
	return 0;
}
INT32 UVACExe_OnOpen(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	UVAC_VID_RESO_ARY uvacVidResoAry = {0};
	UVAC_AUD_SAMPLERATE_ARY uvacAudSampleRateAry = {0};

	DBG_IND(" ++\r\n");
	Ux_DefaultEvent(pCtrl, NVTEVT_EXE_OPEN, paramNum, paramArray); //=>System_OnSensorAttach

#if (defined(_BSP_NA51000_) && _TODO)
	int_setConfig(INT_CONFIG_ID_USB3_GIC_DESTINATION, CC_CORE_CA53_CORE1);
#endif

	if (GxUSB_GetIsUSBPlug()) {

		if (UVACExe_InitCommonMem() < 0) {
			return NVTEVT_CONSUME;
		}
#if UVAC_USE_SPECIFIED_BUF
		{
			MEM_RANGE uvc_specified_buf = {0};
			if (GetUvcSpecifiedBuf(&uvc_specified_buf)) {
				ImageApp_UsbMovie_Config(USBMOVIE_CFG_UVC_BUF, (UINT32)&uvc_specified_buf);
				DBG_DUMP("specified uvc buf (0x%x,0x%X)\r\n", uvc_specified_buf.Addr, uvc_specified_buf.Size);
			}
		}
#endif
		ImageApp_UsbMovie_Config(USBMOVIE_CFG_VDOIN_INFO, (UINT32)&g_isf_vid_in_info);
		ImageApp_UsbMovie_Config(USBMOVIE_CFG_SET_MOVIE_SIZE_CB, (UINT32)xUvac_ConfigMovieSizeCB);

#if UVAC_2PATH
		DBG_IND("UVAC_2PATH Enable, Set Channel:%d\r\n", (UINT32)UVAC_CHANNEL_2V2A);
		ImageApp_UsbMovie_Config(USBMOVIE_CFG_CHANNEL, UVAC_CHANNEL_2V2A);
#else
		DBG_IND("UVAC_2PATH Disable, Set Channel:%d\r\n", (UINT32)UVAC_CHANNEL_1V1A);
		ImageApp_UsbMovie_Config(USBMOVIE_CFG_CHANNEL, UVAC_CHANNEL_1V1A);
#endif

#if (defined(_BSP_NA51023_) && _TODO)
		ImageApp_UsbMovie_Config(USBMOVIE_CFG_TBR_H264, NVT_USBMOVIE_TBR_H264_LOW);
		DBG_IND("TBR-H264:%d\r\n", ImageApp_UsbMovie_GetConfig(USBMOVIE_CFG_TBR_H264));

		ImageApp_UsbMovie_Config(USBMOVIE_CFG_TBR_MJPG, NVT_USBMOVIE_TBR_MJPG_DEFAULT);
		DBG_IND("TBR-MJPG:%d\r\n", ImageApp_UsbMovie_GetConfig(USBMOVIE_CFG_TBR_MJPG));
#endif
#if(STAMP_FUNC==ENABLE)
		ImageApp_UsbMovie_Config(USBMOVIE_CFG_USER_CB_EVENT, (UINT32)UsbCamExe_UserEventCb);
#endif
		ImageUnit_Begin(&ISF_UsbUVAC, 0);
		uvacVidResoAry.aryCnt = NVT_UI_UVAC_RESO_CNT;//MOVIE_SIZE_ID_MAX;
		uvacVidResoAry.pVidResAry = &gUIUvacVidReso[0];
#if NVT_UI_UVAC_RESO_INTERNAL
		DBG_DUMP("Use UVAC internal resolution table\r\n");
#else
		ImageUnit_SetParam(ISF_CTRL, UVAC_PARAM_VID_RESO_ARY, (UINT32)&uvacVidResoAry);
		DBG_IND("Vid Reso:%d\r\n", uvacVidResoAry.aryCnt);
#endif

		uvacAudSampleRateAry.aryCnt = NVT_UI_UVAC_AUD_SAMPLERATE_CNT;
		uvacAudSampleRateAry.pAudSampleRateAry = &gUIUvacAudSampleRate[0];
		ImageUnit_SetParam(ISF_CTRL, UVAC_PARAM_AUD_SAMPLERATE_ARY, (UINT32)&uvacAudSampleRateAry);
		DBG_IND("Aud SampleRate:%d\r\n", uvacAudSampleRateAry.aryCnt);

		xUSBMakerInit_UVAC(&gUIUvacDevDesc);
		ImageUnit_SetParam(ISF_CTRL, UVAC_PARAM_UVAC_VEND_DEV_DESC, (UINT32)&gUIUvacDevDesc);
		DBG_IND("VID=0x%x, PID=0x%x\r\n", gUIUvacDevDesc.VID, gUIUvacDevDesc.PID);

		DBG_IND("VendCb:0x%x\r\n", (UINT32)xUvacEUVendCmdCB_Idx1);
		ImageUnit_SetParam(ISF_CTRL, UVAC_PARAM_EU_VENDCMDCB_ID01, (UINT32)xUvacEUVendCmdCB_Idx1);
		//ImageUnit_SetParam(ISF_CTRL, UVAC_PARAM_EU_CTRL, (UINT32)0x01);
#if UVAC_CT_PU_FUNC
		ImageUnit_SetParam(ISF_CTRL, UVAC_PARAM_CT_CB, (UINT32)xUvacCT_CB);
		ImageUnit_SetParam(ISF_CTRL, UVAC_PARAM_PU_CB, (UINT32)xUvacPU_CB);
#endif
#if UVAC_WINUSB_INTF
		DBG_IND("Enable WinUSB\r\n");
		ImageUnit_SetParam(ISF_CTRL, UVAC_PARAM_WINUSB_ENABLE, TRUE);
		DBG_IND("WinUSBCb:0x%x\r\n", (UINT32)xUvacWinUSBCB);
		ImageUnit_SetParam(ISF_CTRL, UVAC_PARAM_WINUSB_CB, (UINT32)xUvacWinUSBCB);
#else
		DBG_IND("DISABLE WinUSB\r\n");
		ImageUnit_SetParam(ISF_CTRL, UVAC_PARAM_WINUSB_ENABLE, FALSE);
#endif
		ImageUnit_End();

#if (SENSOR_CAPS_COUNT == 1)
		ImageApp_UsbMovie_Config(USBMOVIE_CFG_D2D_MODE, TRUE);
#else
		ImageApp_UsbMovie_Config(USBMOVIE_CFG_D2D_MODE, FALSE);
#endif

		xUvac_AE_SetUIInfo(AE_UI_EV, AE_EV_00);
		DBG_IND("Set Parameter for MFK\r\n");
		xUvac_InitSetMFK();

		DBG_IND("+ImageApp_UsbMovie_Open\r\n");
		ImageApp_UsbMovie_Open();
		DBG_IND("-ImageApp_UsbMovie_Open\r\n");

#if (UVAC_TEST_FOR_CPU_EXCEPTION_MFK && USE_FILEDB)
		//DBG_DUMP("^M======== Work Around Test for CPU exception in MFK for 2 live-view ========\r\n");
		//xUvac_InitFileDB();
#endif

		//DBG_IND("======== Dump ImgUnit Mem ========\r\n");
		//ImageStream_DumpInfo();
	} else {
		DBG_ERR("USB NOT connected!\r\n");
	}
	return NVTEVT_CONSUME;
}
INT32 UVACExe_OnClose(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	DBG_IND(" +\r\n");
	
#if(STAMP_FUNC==ENABLE)
   	MovieStamp_Disable();
	MovieStamp_DestroyBuff();
#endif
	ImageApp_UsbMovie_Close();

	xUvac_FreeVideoMaxBuf(ISF_OUT1);
	#if UVAC_2PATH
	xUvac_FreeVideoMaxBuf(ISF_OUT2);
	#endif

	if (UVACExe_ExitCommonMem() < 0) {
		return NVTEVT_CONSUME;
	}

	#if (LINUX_MSDC == ENABLE)
	int_setConfig(INT_CONFIG_ID_USB3_GIC_DESTINATION, CC_CORE_CA53_CORE2);
	#endif

	Ux_DefaultEvent(pCtrl, NVTEVT_EXE_CLOSE, paramNum, paramArray);
	DBG_IND(" ---\r\n");
	return NVTEVT_CONSUME;
}
#else
INT32 UVACExe_OnOpen(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	return NVTEVT_CONSUME;
}
INT32 UVACExe_OnClose(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	return NVTEVT_CONSUME;
}
#endif

////////////////////////////////////////////////////////////
EVENT_ENTRY CustomUSBPCCObjCmdMap[] = {
	{NVTEVT_EXE_OPEN,               UVACExe_OnOpen},
	{NVTEVT_EXE_CLOSE,              UVACExe_OnClose},
	{NVTEVT_NULL,                   0},  //End of Command Map
};

CREATE_APP(CustomUSBPCCObj, APP_SETUP)

