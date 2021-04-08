/*
    Copyright   Novatek Microelectronics Corp. 2018.  All rights reserved.

    @file       ImageApp_MovieMulti.h
    @ingroup    mIImageApp

    @note       This header file is updated by ImageApp_MovieMulti 0.00.000

    @date       2018/05/15
*/

#ifndef IMAGEAPP_MOVIEMULTI_H
#define IMAGEAPP_MOVIEMULTI_H


#include "ImageStream.h"
#include "ImageApp_MovieCommon.h"
#include "ipl_alg_infor.h"
#include "ImageUnit_BsMux.h"

/**
    @addtogroup mIImageApp
*/
//@{

enum {
	MOVIEMULTI_PARAM_START = 0x0000ED000,
	// ipl group
	MOVIEMULTI_PARAM_IPL_GROUP_BEGIN = MOVIEMULTI_PARAM_START,
	MOVIEMULTI_PARAM_IPL_INFO = MOVIEMULTI_PARAM_IPL_GROUP_BEGIN,                   /// imagepipe
	MOVIEMULTI_PARAM_IPL_SCALE_QUALITY_RATIO,                                       /// imagepipe
	MOVIEMULTI_PARAM_IPL_SCALE_MATHOD_SMALL,                                        /// imagepipe
	MOVIEMULTI_PARAM_IPL_SCALE_MATHOD_LARGE,                                        /// imagepipe
	MOVIEMULTI_PARAM_IPL_FORCED_IMG_SIZE,                                           /// var + imagepipe
	MOVIEMULTI_PARAM_IPL_USER_IMG_SIZE,                                             /// var + imagepipe
	MOVIEMULTI_PARAM_IPL_MIRROR,                                                    /// imagepipe
	MOVIEMULTI_PARAM_IPL_GROUP_END,
	// image group
	MOVIEMULTI_PARAM_IMG_GROUP_BEGIN = MOVIEMULTI_PARAM_START + 0x0100,
	MOVIEMULTI_PARAM_SIZE = MOVIEMULTI_PARAM_IMG_GROUP_BEGIN,                       /// vdoenc + bsmux
	MOVIEMULTI_PARAM_FRAMERATE,                                                     /// vdoenc + bsmux
	MOVIEMULTI_PARAM_DAR,                                                           /// vdoenc + bsmux
	MOVIEMULTI_PARAM_ETHCAM_IME_CROP,
	MOVIEMULTI_PARAM_IMG_MIRROR,
	MOVIEMULTI_PARAM_ETHCAM_DECIMG_CROP,
	MOVIEMULTI_PARAM_MAIN_IME_CROP,
	MOVIEMULTI_PARAM_WIFI_IME_CROP,
	MOVIEMULTI_PARAM_IMG_GROUP_END,
	// video codec group
	MOVIEMULTI_PARAM_CODEC_GROUP_BEGIN = MOVIEMULTI_PARAM_START + 0x0200,
	MOVIEMULTI_PARAM_VDO_ENCODER_OBJ = MOVIEMULTI_PARAM_CODEC_GROUP_BEGIN,          /// vdoenc
	MOVIEMULTI_PARAM_CODEC,                                                         /// vdoenc + bsmux
	MOVIEMULTI_PARAM_PROFILE,                                                       /// vdoenc
	MOVIEMULTI_PARAM_TARGETRATE,                                                    /// vdoenc + bsmux
	MOVIEMULTI_PARAM_INITQP,                                                        /// vdoenc
	MOVIEMULTI_PARAM_MINQP,                                                         /// vdoenc
	MOVIEMULTI_PARAM_MAXQP,                                                         /// vdoenc
	MOVIEMULTI_PARAM_AQ_INFO,                                                       /// vdoenc
	MOVIEMULTI_PARAM_CBR_INFO,                                                      /// vdoenc
	MOVIEMULTI_PARAM_ENCBUF_MS,                                                     /// vdoenc
	MOVIEMULTI_PARAM_ENCBUF_RESERVED_MS,                                            /// vdoenc
	MOVIEMULTI_PARAM_TIMELAPSE_TIME,                                                /// vdoenc
	MOVIEMULTI_PARAM_SKIP_FRAME,                                                    /// vdoenc
	MOVIEMULTI_PARAM_3DNR_CB,                                                       /// vdoenc
	MOVIEMULTI_PARAM_MIN_I_RATIO,                                                   /// vdoenc
	MOVIEMULTI_PARAM_MIN_P_RATIO,                                                   /// vdoenc
	MOVIEMULTI_PARAM_LEVEL_IDC,                                                     /// vdoenc
	MOVIEMULTI_PRARM_ETHCAM_DEC_INFO,                                               /// vdodec
	MOVIEMULTI_PARAM_REC_COMPRESS_ENABLE,                                           /// vdoenc
	MOVIEMULTI_PARAM_VDOENC_UIPRJMODE,                                              /// vdoenc
	MOVIEMULTI_PARAM_VDOENC_BS_RESERVED_SIZE,                                  /// vdoenc
	MOVIEMULTI_PARAM_CODEC_GROUP_END,
	// imgcap group
	MOVIEMULTI_PARAM_IMGCAP_GROUP_BEGIN = MOVIEMULTI_PARAM_START + 0x0300,
	MOVIEMULTI_PARAM_IMGCAP_MAXSIZE = MOVIEMULTI_PARAM_IMGCAP_GROUP_BEGIN,          /// var
	MOVIEMULTI_PARAM_IMGCAP_SIZE,                                                   /// var
	MOVIEMULTI_PARAM_IMGCAP_EXIF_EN,                                                /// vdoenc
	MOVIEMULTI_PARAM_IMGCAP_ALLOC_SNAPSHOT_BUF,                                     /// vdoenc
	MOVIEMULTI_PARAM_IMGCAP_THUMB_WIDTH,
	MOVIEMULTI_PARAM_IMGCAP_GROUP_END,
	// audio codec group
	MOVIEMULTI_PARAM_AUD_GROUP_BEGIN = MOVIEMULTI_PARAM_START + 0x0400,
	MOVIEMULTI_PARAM_AUD_ENCODER_OBJ = MOVIEMULTI_PARAM_AUD_GROUP_BEGIN,            /// audenc
	MOVIEMULTI_PARAM_AUD_SAMPLERATE,                                                /// audenc
	MOVIEMULTI_PARAM_AUD_SAMPLERATE_FILEINFO,                                       /// bsmux
	MOVIEMULTI_PARAM_AUD_CHS,                                                       /// audin
	MOVIEMULTI_PARAM_AUD_CHS_FILEINFO,                                              /// bsmux
	MOVIEMULTI_PARAM_AUD_CODEC,                                                     /// audenc
	MOVIEMULTI_PARAM_AUD_CODEC_FILEINFO,                                            /// bsmux
	MOVIEMULTI_PARAM_AUD_ADTS_HEADER,                                               /// audenc
	MOVIEMULTI_PARAM_AUD_FILETYPE,                                                  /// audenc
	MOVIEMULTI_PARAM_AUD_ENCBUF_MS,                                                 /// audenc
	MOVIEMULTI_PARAM_AUD_CAP_FUNC_EN,                                               /// var
	MOVIEMULTI_PARAM_AUD_CAP_REG_CB,                                                /// userproc
	MOVIEMULTI_PARAM_AUD_MUTE_ENC_FUNC_EN,                                          /// audenc
	MOVIEMULTI_PARAM_AUD_MUTE_ENC,                                                  /// audenc
	MOVIEMULTI_PARAM_AUD_CHS_NUM,                                                   /// audenc
	MOVIEMULTI_PARAM_AUD_CAP_OUTPUT_FMT,						   /// var
	MOVIEMULTI_PARAM_AUD_GROUP_END,
	// file group
	MOVIEMULTI_PARAM_FILE_GROUP_BEGIN = MOVIEMULTI_PARAM_START + 0x0500,
	MOVIEMULTI_PARAM_FILE_FORMAT = MOVIEMULTI_PARAM_FILE_GROUP_BEGIN,               /// audenc + vdoenc + bsmux
	MOVIEMULTI_PARAM_REC_FORMAT,                                                    /// vdoenc + bsmux
	MOVIEMULTI_PARAM_FILE_STRGID,                                                   /// bsmux
	MOVIEMULTI_PARAM_FILE_SEAMLESSSEC,                                              /// vdoenc + bsmux
	MOVIEMULTI_PARAM_FILE_ROLLBACKSEC,                                              /// bsmux
	MOVIEMULTI_PARAM_FILE_KEEPSEC,                                                  /// bsmux
	MOVIEMULTI_PARAM_FILE_ENDTYPE,                                                  /// bsmux
	MOVIEMULTI_PARAM_FILE_OVERLAP,                                                  /// bsmux
	MOVIEMULTI_PARAM_MAX_FILE,                                                      /// bsmux
	MOVIEMULTI_PARAM_FILE_EMRON,                                                    /// bsmux
	MOVIEMULTI_PARAM_FILE_PLAYFRAMERATE,                                            /// bsmux
	MOVIEMULTI_PARAM_FILE_BUFRESSEC,                                                /// bsmux
	MOVIEMULTI_PARAM_PRECALC_BUFFER,                                                /// bsmux
	MOVIEMULTI_PARAM_GPS_DATA,                                                      /// bsmux
	MOVIEMULTI_PARAM_FILE_TRIGEMR,                                                  /// bsmux
	MOVIEMULTI_PARAM_RO_DEL_TYPE,                                                   /// var
	MOVIEMULTI_PARAM_RO_DEL_PERCENT,                                                /// var
	MOVIEMULTI_PARAM_RO_DEL_NUM,                                                    /// var
	MOVIEMULTI_PARAM_RO_DEL_CHK_LAST,                                               /// var
	MOVIEMULTI_PARAM_USER_DATA,                                                     /// bsmux
	MOVIEMULTI_PARAM_FILE_GOP_SIZE,                                                 /// bsmux
	MOVIEMULTI_PRARM_ETHCAM_REC_INFO,                                               /// var
	MOVIEMULTI_PARAM_FILE_LOOP_MAX_CUT,                                             /// bsmux
	MOVIEMULTI_PARAM_FILE_ROLLBACKSEC_CRASH,                                        /// var
	MOVIEMULTI_PARAM_FILE_KEEPSEC_CRASH,                                            /// var
	MOVIEMULTI_PARAM_CUSTOM_DATA,                                                   /// bsmux
	MOVIEMULTI_PARAM_EMR_BSQ_INCARD,                                                /// bsmux + fileout
	MOVIEMULTI_PARAM_WRITE_BLKSIZE,                                                 /// bsmux
	MOVIEMULTI_PARAM_FLUSH_FREQUENCY,                                               /// bsmux
	MOVIEMULTI_PARAM_FILE_FRONT_MOOV,                                               /// bsmux
	MOVIEMULTI_PARAM_FILE_SETCRASH_NOT_RO,                                               /// var
	MOVIEMULTI_PARAM_FILE_GROUP_END,
	// display group
	MOVIEMULTI_PARAM_DISP_GROUP_BEGIN = MOVIEMULTI_PARAM_START + 0x0600,
	MOVIEMULTI_PARAM_DISP_REG_CB = MOVIEMULTI_PARAM_DISP_GROUP_BEGIN,
	MOVIEMULTI_PARAM_DISP_DIR,
	MOVIEMULTI_PARAM_DISP_DUAL_EN,
	MOVIEMULTI_PARAM_DISP_FPS,
	MOVIEMULTI_PARAM_DISP_USE_PRJ_USERPROC_CB,
	MOVIEMULTI_PARAM_DISP_IME_CROP,
	MOVIEMULTI_PARAM_DISP_OFF,
	MOVIEMULTI_PARAM_DISP_GROUP_END,
	// wifi group
	MOVIEMULTI_PARAM_WIFI_GROUP_BEGIN = MOVIEMULTI_PARAM_START + 0x0700,
	MOVIEMULTI_PARAM_WIFI_REG_CB = MOVIEMULTI_PARAM_WIFI_GROUP_BEGIN,
	MOVIEMULTI_PARAM_WIFI_REG_BS_CB,
	MOVIEMULTI_PARAM_WIFI_GROUP_END,
	// misc group
	MOVIEMULTI_PARAM_MISC_GROUP_BEGIN = MOVIEMULTI_PARAM_START + 0x0900,
	MOVIEMULTI_PARAM_USER_CB = MOVIEMULTI_PARAM_MISC_GROUP_BEGIN,                   /// var
	MOVIEMULTI_PARAM_D2D_MODE,
	MOVIEMULTI_PARAM_YUV_COMPRESS,
	MOVIEMULTI_PARAM_TV_RANGE,                                                      /// imagepipe(510) + vdoenc
	MOVIEMULTI_PARAM_RAW_BUF_NUM,                                                   /// D2D mode : set one/two raw buf num, default is 3
	MOVIEMULTI_PARAM_FILEDB_FILTER,
	MOVIEMULTI_PRARM_FILEDB_MAX_MUM,
	MOVIEMULTI_RARAM_ETHCAM_RX_FUNC_EN,
	MOVIEMULTI_PARAM_ETHCAM_TX_BS_CB,
	MOVIEMULTI_RARAM_ETHCAM_RX_AUD_FROM_TX_FUNC_EN,
	MOVIEMULTI_PARAM_DEBUG_MSG_EN,
	MOVIEMULTI_PRARM_DELETE_FILTER,
	MOVIEMULTI_PARAM_MISC_GROUP_END,
};

typedef enum {
	MOVIE_IPL_SIZE_AUTO = 0,
	MOVIE_IPL_SIZE_MAIN,
	MOVIE_IPL_SIZE_CLONE,
	MOVIE_IPL_SIZE_WIFI,
	MOVIE_IPL_SIZE_ALG,
	MOVIE_IPL_SIZE_DISP,
	MOVIE_IPL_SIZE_USER,
	ENUM_DUMMY4WORD(MOVIE_IPL_SIZE)
} MOVIE_IPL_SIZE;

typedef enum {
	RO_DEL_TYPE_PERCENT = 0,
	RO_DEL_TYPE_NUM,
	ENUM_DUMMY4WORD(MOVIE_RO_DEL_TYPE)
} MOVIE_RO_DEL_TYPE;

typedef struct {
	IPL_SEL_ITEM   item;
	UINT32  value;
} MOVIEMULTI_RECODE_IPL_INFO;

typedef struct {
	USIZE size;
	UINT32 fps;
} MOVIEMULTI_IPL_SIZE_INFO;

typedef struct {
	UINT32 aud_rate;
	UINT32 aud_ch;
	UINT32 aud_ch_num;
} MOVIEMULTI_AUDIO_INFO;

typedef struct {
	UINT32 MaxImgLink;
	UINT32 MaxDispLink;
	UINT32 MaxWifiLink;
	UINT32 MaxAudCapLink;
	UINT32 MaxEthCamLink;
} MOVIEMULTI_MAX_LINK_INFO;

typedef struct {
	USIZE DispIMESize;
	URECT DispIMEWin;
} MOVIEMULTI_DISP_IME_CROP_INFO;

typedef struct {
	USIZE IMESize;
	URECT IMEWin;
} MOVIEMULTI_IME_CROP_INFO;

typedef struct {
	USIZE size;
	URECT win;
} MOVIEMULTI_CROP_INFO;

typedef void (*MOVIE_DISP_PIP_PROCESS_CB)(void);
typedef void (*MOVIE_WIFI_PIP_PROCESS_CB)(void);
typedef void (*ETHCAM_SND_FUNC)(char* addr, int* size);

//typedef NMEDIAREC_CALCSEC_SETTING2 MOVIEMULTI_CALCSEC_SETTING2;
//CID 129717
typedef struct {
	UINT64    givenSpace;  //in: free space
	NMEDIAREC_CALCSEC_SETTING info[8];
} MOVIEMULTI_CALCSEC_SETTING2;

typedef struct {
	MOVIE_CFG_REC_ID        rec_id;
	CHAR*   movie;
	CHAR*   emr;
	CHAR*   snapshot;
	BOOL   file_naming;
} MOVIEMULTI_RECODE_FOLDER_NAMING;

typedef struct {
	UINT32    rotate_dir;
	UINT32  pip_view;
} MOVIEMULTI_CFG_DISP_INFO;


typedef struct {
	UINT32  width;
	UINT32  height;
	UINT32  gop;
	UINT32  codec;
	UINT32  header_size;
	UINT8  *header;
} MOVIEMULTI_ETHCAM_DEC_INFO;

typedef struct {
	UINT32  width;
	UINT32  height;
	UINT32  vfr;
	UINT32  tbr;
	UINT32  ar;
	UINT32  gop;
	UINT32  codec;
	UINT32  aud_codec;
	UINT32  rec_mode;
	UINT32  rec_format;
} MOVIEMULTI_ETHCAM_REC_INFO;

#define ETHCAM_TX_MAGIC_KEY        0x67100000

#define MOVIEMULTI_DISK_MAX_DIR_NUM   8 //default

typedef struct {
	CHAR dir_path[128];
	UINT64 dir_size;
	UINT32 dir_ratio_of_disk;
	UINT32 file_num;
	UINT64 file_avg_size;
	UINT64 falloc_size;
	BOOL is_falloc;
	BOOL is_use;
} MOVIEMULTI_DIR_INFO;

typedef struct {
	UINT64 disk_size;
	UINT64 free_size;
	MOVIEMULTI_DIR_INFO dir_info[MOVIEMULTI_DISK_MAX_DIR_NUM];
	UINT64 other_size;
	UINT32 dir_num;
	UINT32 dir_depth;
	BOOL is_format_free; //0:init 1:true 2:false
} MOVIEMULTI_DISK_INFO;

extern void ImageApp_MovieMulti_InstallID(void);

extern ISF_RV ImageApp_MovieMulti_Open(void);
extern ISF_RV ImageApp_MovieMulti_Close(void);

extern ISF_RV ImageApp_MovieMulti_Config(UINT32 config_id, UINT32 value);

extern ISF_RV ImageApp_MovieMulti_FileOption(MOVIE_RECODE_FILE_OPTION *prec_option);
extern ISF_RV ImageApp_MovieMulti_ImgLinkForDisp(MOVIE_CFG_REC_ID id, UINT32 action, BOOL allow_pull);
extern ISF_RV ImageApp_MovieMulti_ImgLinkForDisp2(MOVIE_CFG_REC_ID id, UINT32 action, BOOL allow_pull);
extern ISF_RV ImageApp_MovieMulti_ImgLinkForStreaming(MOVIE_CFG_REC_ID id, UINT32 action, BOOL allow_pull);
extern ISF_RV ImageApp_MovieMulti_ImgLinkForAlg(MOVIE_CFG_REC_ID id, MOVIE_CFG_ALG_PATH path, UINT32 action, BOOL allow_pull);

extern ISF_RV ImageApp_MovieMulti_EthCamLinkForDisp(MOVIE_CFG_REC_ID id, UINT32 action, BOOL allow_pull);
extern ISF_RV ImageApp_MovieMulti_EthCamRxRecStart(MOVIE_CFG_REC_ID id);
extern ISF_RV ImageApp_MovieMulti_EthCamRxRecStop(MOVIE_CFG_REC_ID id);
extern ISF_RV ImageApp_MovieMulti_RecPause(MOVIE_CFG_REC_ID id);
extern ISF_RV ImageApp_MovieMulti_RecResume(MOVIE_CFG_REC_ID id);
extern UINT32 ImageApp_MovieMulti_GetEthCamVdoDecInPort(MOVIE_CFG_REC_ID id);
extern UINT32 ImageApp_MovieMulti_GetEthCamDemuxInPort(MOVIE_CFG_REC_ID id);
extern UINT32 ImageApp_MovieMulti_GetEthCamAudBsmuxInPort(MOVIE_CFG_REC_ID id);
extern UINT32 ImageApp_MovieMulti_GetDesc(MOVIE_CFG_REC_ID id, UINT32 codec, PMEM_RANGE buf, PMEM_RANGE sps, PMEM_RANGE pps, PMEM_RANGE vps);
extern ISF_RV ImageApp_MovieMulti_EthCam_TxTrigThumb(UINT32 portid);
extern ISF_RV ImageApp_MovieMulti_EthCamTxRecId1_SetRawEncodeBuff(UINT32 WorkAddr, UINT WorkSize);
extern ISF_RV ImageApp_MovieMulti_EthCamTxRecId1_DumpBsInfo(void);
extern ISF_RV ImageApp_MovieMulti_EthCamTxCloneId1_DumpBsInfo(void);
extern ISF_RV ImageApp_MovieMulti_EthCamTxRecId1_CBLink(ISF_PORT *pPort, ISF_DATA *pData , UINT32 SndFunc);
extern ISF_RV ImageApp_MovieMulti_EthCamTxCloneId1_CBLink(ISF_PORT *pPort, ISF_DATA *pData, UINT32 SndFunc);
extern ISF_RV ImageApp_MovieMulti_EthCamTxRecId1AudCap_CBLink(ISF_PORT *pPort, ISF_DATA *pData);

extern ISF_RV ImageApp_MovieMulti_RecStart(MOVIE_CFG_REC_ID id);
extern ISF_RV ImageApp_MovieMulti_RecStop(MOVIE_CFG_REC_ID id);

extern ISF_RV ImageApp_MovieMulti_TriggerSnapshot(MOVIE_CFG_REC_ID id);

extern ISF_RV ImageApp_MovieMulti_SetParam(MOVIE_CFG_REC_ID id, UINT32 param, UINT32 value);
extern ISF_RV ImageApp_MovieMulti_GetParam(MOVIE_CFG_REC_ID id, UINT32 param, UINT32* value);

extern ISF_RV ImageApp_MovieMulti_StreamingStart(MOVIE_CFG_REC_ID id);
extern ISF_RV ImageApp_MovieMulti_StreamingStop(MOVIE_CFG_REC_ID id);

extern ISF_RV ImageApp_MovieMulti_DispStart(MOVIE_CFG_REC_ID id);
extern ISF_RV ImageApp_MovieMulti_DispStop(MOVIE_CFG_REC_ID id);

extern ISF_RV ImageApp_MovieMulti_UvacStart(MOVIE_CFG_REC_ID id);
extern ISF_RV ImageApp_MovieMulti_UvacStop(MOVIE_CFG_REC_ID id);

extern ISF_RV ImageApp_MovieMulti_AudCapStart(MOVIE_CFG_REC_ID id);
extern ISF_RV ImageApp_MovieMulti_AudCapStop(MOVIE_CFG_REC_ID id);

extern ISF_RV ImageApp_MovieMulti_ResetIFrame(MOVIE_CFG_REC_ID id);

extern UINT32 ImageApp_MovieMulti_GetDispDataPort(MOVIE_CFG_REC_ID id);
extern UINT32 ImageApp_MovieMulti_GetDispDataPort2(MOVIE_CFG_REC_ID id);
extern UINT32 ImageApp_MovieMulti_GetStreamingDataPort(MOVIE_CFG_REC_ID id);
extern UINT32 ImageApp_MovieMulti_GetAlgDataPort(MOVIE_CFG_REC_ID id, MOVIE_CFG_ALG_PATH path);
extern UINT32 ImageApp_MovieMulti_GetVdoEncPort(MOVIE_CFG_REC_ID id);
extern UINT32 ImageApp_MovieMulti_GetDispImgTransPort(MOVIE_CFG_REC_ID id);
extern UINT32 ImageApp_MovieMulti_GetStrmBsDemuxInPort(MOVIE_CFG_REC_ID id);

extern UINT32 ImageApp_MovieMulti_Recid2ImgLink(MOVIE_CFG_REC_ID id);
extern UINT32 ImageApp_MovieMulti_Recid2BsPort(MOVIE_CFG_REC_ID id);
extern UINT32 ImageApp_MovieMulti_Recid2Table(MOVIE_CFG_REC_ID id);
extern UINT32 ImageApp_MovieMulti_BsPort2Table(UINT32 port);
extern UINT32 ImageApp_MovieMulti_BsPort2Imglink(UINT32 port);
extern UINT32 ImageApp_MovieMulti_BsPort2EthCamlink(UINT32 port);
extern UINT32 ImageApp_MovieMulti_BsPort2Recid(UINT32 port);
extern UINT32 ImageApp_MovieMulti_VePort2Imglink(UINT32 port);

extern UINT32 ImageApp_MovieMulti_IsStreamRunning(MOVIE_CFG_REC_ID id);
extern UINT32 ImageApp_MovieMulti_GetFreeRec(void);
extern UINT32 ImageApp_MovieMulti_GetFreeRec2(MOVIEMULTI_CALCSEC_SETTING2 *pSetting);
extern UINT32 ImageApp_MovieMulti_GetRemainSize(CHAR drive, UINT64 *p_size);
extern UINT32 ImageApp_MovieMulti_SetReservedSize(CHAR drive, UINT64 size, UINT32 is_loop);
extern UINT32 ImageApp_MovieMulti_SetExtendCrashInfo(UINT32 unit, UINT32 max_num, UINT32 enable);
extern ISF_RV ImageApp_MovieMulti_TrigOnCardEMR(MOVIE_CFG_REC_ID rec_id);
extern ISF_RV ImageApp_MovieMulti_StopOnCardEMR(MOVIE_CFG_REC_ID rec_id);
extern UINT32 ImageApp_MovieMulti_IsAudStreamRunning(void);

/**
	Set format free feature.

	This api will set out format free feature (TRUE or FALSE)

	@note
	-# If format free is set out, it need to set folder info before formatting.

	@param[in] drive        The specific drive name. (e.g. 'A')
	@param[in] is_on        Turn on/off the format free feature. (e.g. TRUE)

	@return
		- @b 0      : Success.
		- @b Others : Has some error.
*/
extern INT32 ImageApp_MovieMulti_SetFormatFree(CHAR drive, BOOL is_on);

/**
	Set the information of folders.

	This api will set the necessary information of storage allocation

	@note
	-# Information used by format free feature and folder loop del.

	@param[in] p_path       The specific full path of folder. (e.g. "A:\\Movie\\")
	@param[in] ratio        The proportional storage assignment. (e.g. 30)
	@param[in] f_size       Fixed size of each file in folder. (e.g. 180 * 1024 * 1024ULL)
	@note
	-# Fixed size of file should not less than (target bitrate * record interval).

	@return
		- @b 0      : Success.
		- @b Others : Has some error.
*/
extern INT32 ImageApp_MovieMulti_SetFolderInfo(CHAR *p_path, UINT32 ratio, UINT64 f_size);

/**
	Get the setting information of folders last time.

	This api will get the setting information from user last time

	@note
	-# Information used by format free feature and folder loop del.

	@param[in]  p_path      The specific full path of folder. (e.g. "A:\\Movie\\")
	@param[out] ratio       The proportional storage assignment. (e.g. 30)
	@param[out] f_size      Fixed size of each file in folder. (e.g. 180 * 1024 * 1024ULL)

	@return
		- @b 0      : Success.
		- @b Others : Has some error.
*/
extern INT32 ImageApp_MovieMulti_GetFolderInfo(CHAR *p_path, UINT32 *ratio, UINT64 *f_size);

/**
	Allocate storage space (format free feature).

	This api will allocate the entire storage space by fixed size file allocation method

	@note
	-# Need to set format free featrue TRUE and set information of folders before formatting.
	-# Need to call this api right after formatting.

	@param[in] drive        The specific drive name. (e.g. 'A')

	@return
		- @b 0      : Success.
		- @b Others : Has some error.
*/
extern INT32 ImageApp_MovieMulti_FMAllocate(CHAR drive);

/**
	Scan storage space (format free feature).

	This api will scan the entire storage space

	@note

	@param[in]  drive       The specific drive name. (e.g. 'A')
	@param[out] p_disk_info The data structure of disk information. (e.g. 'A')

	@return
		- @b 0      : Success.
		- @b Others : Has some error.
*/
extern INT32 ImageApp_MovieMulti_FMScan(CHAR drive, MOVIEMULTI_DISK_INFO* p_disk_info);

/**
	Set loop delete files by folder.

	This api will set out folder loop delete feature (TRUE or FALSE)

	@note
	-# If loop delete by folder is set out, it also need to set folder info.
	-# The default situation will only consider the drive space to delete files.
	-# If set TRUE, it will change to take care of each folder's capacity.
	-# If set FLASE, it will change back to the default situation.

	@param[in] drive        The specific drive name. (e.g. 'A')
	@param[in] is_on        Turn on/off folder loop delete feature. (e.g. TRUE)

	@return
		- @b 0      : Success.
		- @b Others : Has some error.
*/
extern INT32 ImageApp_MovieMulti_SetFolderLoop(CHAR drive, BOOL is_on);

/**
	Resize storage space.

	This api will resize the entire storage space by checking each folder's ratio.

	@note
	-# Need to set floder loop del TRUE and set information of folders before resizing.
	-# This api might delete folders and files. Should alert users before using it.

	@param[in] drive        The specific drive name. (e.g. 'A')

	@return
		- @b 0      : Success.
		- @b Others : Has some error.
*/
extern INT32 ImageApp_MovieMulti_FMResize(CHAR drive);

extern INT32 ImageApp_MovieMulti_FMDump(CHAR drive);
extern INT32 ImageApp_MovieMulti_FMOpen(CHAR drive);
extern INT32 ImageApp_MovieMulti_FMClose(CHAR drive);
extern INT32 ImageApp_MovieMulti_FMConfig(MOVIE_CFG_FDB_INFO *p_cfg);
extern void ImageApp_MovieMulti_FMSetSortBySN(CHAR *pDelimStr, UINT32 nDelimCount, UINT32 nNumOfSn);

/**
    set crash the assigned rec id to backup files

    set crash the assigned rec id to backup files

    @param[in] MOVIE_CFG_REC_ID id

    @return ISF_RV
*/
extern ISF_RV ImageApp_MovieMulti_SetCrash(MOVIE_CFG_REC_ID rec_id, UINT32 is_set);
extern ISF_RV ImageApp_MovieMulti_TrigEMR(MOVIE_CFG_REC_ID rec_id);
extern ISF_RV ImageApp_MovieMulti_SetCutNow(MOVIE_CFG_REC_ID rec_id);

extern void ImageApp_MovieMulti_Root_Path(CHAR * prootpath, UINT32 rec_id);
extern ISF_RV ImageApp_MovieMulti_Folder_Naming(MOVIEMULTI_RECODE_FOLDER_NAMING *pfolder_naming);
extern BOOL ImageApp_MovieMulti_isFileNaming(MOVIE_CFG_REC_ID rec_id);
extern ISF_RV ImageApp_MovieMulti_RegUserCB(MovieUserEventCb *fncb);

extern ISF_RV ImageApp_MovieMulti_EthCamTxStart(MOVIE_CFG_REC_ID id);
extern ISF_RV ImageApp_MovieMulti_EthCamTxStop(MOVIE_CFG_REC_ID id);
extern UINT32 ImageApp_MovieMulti_GetEthUserProcPort(MOVIE_CFG_REC_ID id);

extern INT32 ImageApp_MovieMulti_SetCyclicRec(CHAR *p_path, BOOL is_on);
extern INT32 ImageApp_MovieMulti_SetExtStrType(CHAR drive, UINT32 type);
extern INT32 ImageApp_MovieMulti_SetUseHiddenFirst(CHAR drive, BOOL value);

//@}
#endif//IMAGEAPP_MOVIEMULTI_H

