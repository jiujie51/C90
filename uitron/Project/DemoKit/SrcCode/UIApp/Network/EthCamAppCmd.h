#ifndef _ETHCAMAPPCMD_H_
#define _ETHCAMAPPCMD_H_

#include "Type.h"
#include "EthCamCmdParser.h"
#include "EthCamSocket.h"

#define PRI_ETHCAM_CMD_SND               6
#define PRI_ETHCAM_CMD_RCV               5 //6
#define STKSIZE_ETHCAM_CMD_SND           4096
#define STKSIZE_ETHCAM_CMD_RCV           4096

#define FLG_ETHCAM_CMD_OPENED            			FLGPTN_BIT(0)// 1
#define FLG_ETHCAM_CMD_STOP							FLGPTN_BIT(1)// 2
#define FLG_ETHCAM_CMD_IDLE							FLGPTN_BIT(2)// 4
#define FLG_ETHCAM_CMD_SND							FLGPTN_BIT(3)// 8
#define FLG_ETHCAM_CMD_RCV							FLGPTN_BIT(4)// 16
#define FLG_ETHCAM_CMD_SETIP						FLGPTN_BIT(5)// 32
#define FLG_ETHCAM_CMD_GETFRM						FLGPTN_BIT(6)// 64



#define ETHCAM_CMD_ACK                              9000
#define ETHCAM_CMD_STARTUP                          9001
#define ETHCAM_CMD_GET_TX_MOVIE_TBR                 9002
#define ETHCAM_CMD_GET_TX_MOVIE_REC_SIZE            9003
#define ETHCAM_CMD_GET_TX_MOVIE_FPS                 9004
#define ETHCAM_CMD_GET_TX_MOVIE_GOP                 9005
#define ETHCAM_CMD_GET_TX_MOVIE_CODEC               9006
#define ETHCAM_CMD_GET_TX_SPS_PPS                   9007
#define ETHCAM_CMD_TX_STREAM_START                  9008
#define ETHCAM_CMD_TX_STREAM_STOP                   9009
#define ETHCAM_CMD_GET_TX_MOVIE_THUMB               9010
#define ETHCAM_CMD_GET_TX_MOVIE_RAW_ENCODE          9011
#define ETHCAM_CMD_GET_TX_REC_INFO                  9012
//ONLY for IQ tuning
#define ETHCAM_CMD_TX_RTSP_START                    9013
#define ETHCAM_CMD_SYNC_TIME                        9014
#define ETHCAM_CMD_SYNC_MENU_SETTING                9015
#define ETHCAM_CMD_TX_RESET_I                       9016
#define ETHCAM_CMD_GET_TX_DEC_INFO                  9017
#define ETHCAM_CMD_GET_FRAME                        9018
#define ETHCAM_CMD_TX_RESET_QUEUE                   9019
#define ETHCAM_CMD_TX_FWUPDATE_FWSEND               9020
#define ETHCAM_CMD_TX_FWUPDATE_START                9021

#define ETHCAM_CMD_TX_DISP_CROP	                  	9022
#define ETHCAM_CMD_TX_POWEROFF                      9023
#define ETHCAM_CMD_TX_IO_STATUS                     9024
#define ETHCAM_CMD_TX_FW_VERSION                    9025
#define ETHCAM_CMD_SET_TX_IP_RESET                  9026
#define ETHCAM_CMD_SET_TX_SYS_INFO                  9027
#define ETHCAM_CMD_DUMP_TX_BS_INFO                  9028
#define ETHCAM_CMD_CHANGE_MODE	                    9029
#define ETHCAM_CMD_IPERF_TEST                       	9034
//#if  (AV_LCA_FUNC == ENABLE)
#define ETHCAM_CMD_TX_LCAINFO                       9030
#define ETHCAM_CMD_TX_LCAWRN                        9031
#define ETHCAM_CMD_RX_LCAWRN						9039
#define ETHCAM_CMD_RX_LCASTOP						9040
//#endif
#define ETHCAM_CMD_TX_SLIDEUP   					9032
#define ETHCAM_CMD_TX_SLIDEDOWN 					9033
#define ETHCAM_CMD_SET_TX_CODEC_SRCTYPE    				9035
#define ETHCAM_CMD_SET_TX_FLIP                      	9036
#define ETHCAM_CMD_SET_TX_AUDCAP                 	9037
#define ETHCAM_CMD_SET_TX_AUDINFO                  	9038
//cj_mio
typedef enum
{
	USER_AVLCA_WRN_NONE,
	USER_AVLCA_WRN_LEVEL_L,
	USER_AVLCA_WRN_LEVEL_H,
	USER_AVLCA_WRN_LEVEL_MAX

}USER_AVLCA_WARNMSG;
//cj_mio


#define ETHCAM_CMD_ROOT        '/'
#define ETHCAM_CMD_CUSTOM_TAG  '?'


#define ETHCAM_RET_ACK                   0
#define ETHCAM_RET_OK                    1
#define ETHCAM_RET_CONTINUE          	 2
#define ETHCAM_RET_TIMEOUT           	 3

#define ETHCAM_RET_ERR                  -1
#define ETHCAM_RET_CMD_NOT_FOUND            	ETHCAM_CMD_NOT_FOUND



#define ETHCAM_RET_FW_OK               0   // Update FW to NAND OK
#define ETHCAM_RET_FW_INVALID_STG      (-19)   // Invalid source storage
#define ETHCAM_RET_FW_READ_ERR         (-18)   // FW doesn't exist or read error
#define ETHCAM_RET_FW_READ_CHK_ERR     (-17)   // Read FW checksum failed, might be error
#define ETHCAM_RET_FW_WRITE_ERR        (-16)   // Write FW to NAND error
#define ETHCAM_RET_FW_READ2_ERR        (-15)   // Read FW from NAND failed (for write checking)
#define ETHCAM_RET_FW_WRITE_CHK_ERR    (-14)   // Write FW checksum failed
#define ETHCAM_RET_INVLID_MODE         (-13)   // Must be main mode

#define ETHCAM_RET_FW_OFFSET           (-20)   // FW update offset


/* ETHCAM_RET_FW_xxx refere to
#define UPDNAND_STS_FW_OK               0   // Update FW to NAND OK
#define UPDNAND_STS_FW_INVALID_STG      1   // Invalid source storage
#define UPDNAND_STS_FW_READ_ERR         2   // FW doesn't exist or read error
#define UPDNAND_STS_FW_READ_CHK_ERR     3   // Read FW checksum failed, might be error
#define UPDNAND_STS_FW_WRITE_ERR        4   // Write FW to NAND error
#define UPDNAND_STS_FW_READ2_ERR        5   // Read FW from NAND failed (for write checking)
#define UPDNAND_STS_FW_WRITE_CHK_ERR    6   // Write FW checksum failed
#define UPDNAND_STS_INVLID_MODE         7   // Must be main mode
*/

#define ETHCAM_CMD_DONE                   	 FLGPTN_BIT(7)
#define ETHCAM_CMD_TIMEOUT                   FLGPTN_BIT(8)




//#define ETH_DATA1_SOCKET_PORT  8887
//#define ETH_DATA2_SOCKET_PORT  8888
//#define ETH_CMD_SOCKET_PORT  8899

#define ETH_DATA_SOCKET_MAX  2

#define MAX_I_FRAME_SZIE 685*1024


typedef enum _ETHCAM_XML_RESULT_TYPE {
	ETHCAM_XML_RESULT_TYPE_DEFAULT_FORMAT =1,
	ETHCAM_XML_RESULT_TYPE_VALUE_RESULT,
	ETHCAM_XML_RESULT_TYPE_STRING_RESULT,
	ETHCAM_XML_RESULT_TYPE_LIST,
	ENUM_DUMMY4WORD(ETHCAM_XML_RESULT_TYPE)
} ETHCAM_XML_RESULT_TYPE;

typedef enum _ETHCAM_TX_SYS_SRCTYPE {
	ETHCAM_TX_SYS_SRCTYPE_67 =1,
	ETHCAM_TX_SYS_SRCTYPE_57,
	ETHCAM_TX_SYS_SRCTYPE_68,
	ENUM_DUMMY4WORD(ETHCAM_TX_SYS_SRCTYPE)
} ETHCAM_TX_SYS_SRCTYPE;


/////cmd need usging CB function
typedef struct _ETHCAM_XML_CB_REGISTER{
    void (* EthCamXML_data_CB)(INT32 bEnd, void *output_data);
    ETHCAM_PATH_ID path_id;
    ETHCAM_PORT_TYPE port_type;
}ETHCAM_XML_CB_REGISTER;


typedef struct _ETHCAM_XML_RESULT{
    INT32 cmd;
    UINT32 result_type;
    ETHCAM_XML_CB_REGISTER output_cb;
}ETHCAM_XML_RESULT;

typedef struct _ETHCAM_XML_DEFAULT_FORMAT{
    INT32 cmd;
    ETHCAM_PATH_ID path_id;
    ETHCAM_PORT_TYPE port_type;
    INT32 status;
}ETHCAM_XML_DEFAULT_FORMAT;

typedef struct _ETHCAM_XML_VALUE_RESULT{
    INT32 cmd;
    ETHCAM_PATH_ID path_id;
    ETHCAM_PORT_TYPE port_type;
    INT32 status;
    INT32 value;
}ETHCAM_XML_VALUE_RESULT;
typedef struct _ETHCAM_XML_STRING_RESULT{
    INT32 cmd;
    ETHCAM_PATH_ID path_id;
    ETHCAM_PORT_TYPE port_type;
    INT32 status;
    char string[128];
}ETHCAM_XML_STRING_RESULT;

typedef struct _ETHCAM_XML_LIST{
    INT32 cmd;
    ETHCAM_PATH_ID path_id;
    ETHCAM_PORT_TYPE port_type;
    INT32 item[128];
    UINT32 total_item_cnt;
}ETHCAM_XML_LIST;

typedef struct {
	UINT32  bStarupOK;
	UINT32  Flip;
	UINT32  Tbr;
	UINT32  Fps;
	UINT32  Width;
	UINT32  Height;
	UINT32  Gop;
	UINT32  Codec;
	UINT32  DescSize;
	UINT32  SPSSize;
	UINT32  PPSSize;
	UINT32  VPSSize;
	UINT8   Desc[128];
} ETHCAM_TX_DEC_INFO;

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
	UINT32  DescSize;
	UINT32  SPSSize;
	UINT32  PPSSize;
	UINT32  VPSSize;
	UINT8   Desc[128];
} ETHCAM_TX_REC_INFO;

typedef struct {
	UINT32  FwAddr;
	UINT32  FwSize;
	INT32 cmd;
	ETHCAM_PATH_ID path_id;
	ETHCAM_PORT_TYPE port_type;
} ETHCAM_FWUD;

typedef struct {
	UINT32 Size;
	UINT32 WDR;
	UINT32 EV;
	UINT32 DateImprint;
	UINT32 SensorRotate;
	UINT32 Codec;
	UINT32 TimeLapse;
} ETHCAM_MENU_SETTING;

typedef struct {
	UINT32  PullModeEn;
	UINT32  CloneDisplayPathEn;
	UINT32  bCmdOK;
} ETHCAM_TX_SYS_INFO;

typedef struct{
	UINT32 curDispOffset;
//	UINT32 curFlDualCAM;
}ETHCAM_TX_SET_OFFESET;

typedef struct  
{
	INT32      Enable_LCA;
	INT32 	   LCA_Get_Valid;
	INT32      LCA_Happen_Direct;
#define  LCA_HAPPEN_RIGHT  (1)	
#define  LCA_HAPPEN_LEFT   (2)

	INT32	   LCA_Num;
	IRECT	   LCARect[20]; //{x,y,w,h}
 	 
} MIO_LCA_FUNC_INFO;
typedef struct {
	UINT32  VCodec;  //MOVIE_CODEC_H265, MOVIE_CODEC_H264
	ETHCAM_TX_SYS_SRCTYPE  Srctype;
	UINT32  bCmdOK;
} ETHCAM_TX_CODEC_SRCTYPE;

typedef struct {
	UINT32  ACodec;  //AUDDEC_DECODER_AAC
	UINT32 	AudRate;
	UINT32 	AudCh;
	UINT32 	AudChCnt;
} ETHCAM_TX_AUDINFO;
extern ETHCAM_TX_DEC_INFO sEthCamTxDecInfo[ETHCAM_PATH_ID_MAX];
extern ETHCAM_TX_REC_INFO sEthCamTxRecInfo[ETHCAM_PATH_ID_MAX];
//extern ETHCAM_MENU_SETTING g_sEthCamMenuSetting[ETHCAM_PATH_ID_MAX];
extern ETHCAM_CMD_ENTRY Cmd_ethcam[];
extern ETHCAM_XML_RESULT EthCamXMLResultTbl[];
extern ETHCAM_FWUD sEthCamFwUd;
extern ETHCAM_TX_AUDINFO sEthCamTxAudInfo[ETHCAM_PATH_ID_MAX];
#if(defined(_NVT_ETHREARCAM_TX_))
extern ETHCAM_TX_SYS_INFO sEthCamSysInfo;
extern ETHCAM_TX_CODEC_SRCTYPE sEthCamCodecSrctype;
extern UINT32 g_EthCamTxIsAudCapEn;
#endif
#if(defined(_NVT_ETHREARCAM_RX_))
extern ETHCAM_TX_SYS_INFO sEthCamTxSysInfo[ETHCAM_PATH_ID_MAX];
extern ETHCAM_TX_CODEC_SRCTYPE sEthCamCodecSrctype[ETHCAM_PATH_ID_MAX];
extern UINT32 g_EthCamCfgTxAudCap;
#endif

extern void EthCamCmdHandler_InstallID(void) _SECTION(".kercfg_text");
extern UINT32 _SECTION(".kercfg_data") ETHCAM_CMD_SND_FLG_ID;
extern UINT32 _SECTION(".kercfg_data")  ETHCAM_CMD_RCV_FLG_ID;
extern UINT32 _SECTION(".kercfg_data")  ETHCAM_CMD_SND_TSK_ID;
extern UINT32 _SECTION(".kercfg_data")  ETHCAM_CMD_RCV_TSK_ID;

extern UINT32 _SECTION(".kercfg_data")  ETHCAM_CMD_SNDDATA1_SEM_ID;
extern UINT32 _SECTION(".kercfg_data")  ETHCAM_CMD_SNDDATA2_SEM_ID;
extern UINT32 _SECTION(".kercfg_data")  ETHCAM_CMD_SNDCMD_SEM_ID;

#if(defined(_NVT_ETHREARCAM_TX_))
extern UINT32 _SECTION(".kercfg_data")  ETHCAM_CMD_SNDFRAME_SEM_ID;
extern UINT32 _SECTION(".kercfg_data")  ETHCAM_CMD_SNDFRAME_CLONE_SEM_ID;
#endif
#if(defined(_NVT_ETHREARCAM_RX_))
extern UINT32 _SECTION(".kercfg_data")  ETHCAM_WIFICB_VDOFRM_SEM_ID;
extern UINT32 _SECTION(".kercfg_data")  ETHCAM_DISP_DATA_SEM_ID;
#endif

extern ER EthCamCmdTsk_Open(void);
extern ER EthCamCmdTsk_Close(void);

extern INT32 EthCamCmdXML_parser(INT32 cmd_id,char *xml_buf ,void* output);
extern INT32 EthCamCmdXML_GetCmdId(char *xml_buf);
extern void EthCamCmd_Init(void);
extern INT32  EthCam_SendXMLCmd(ETHCAM_PATH_ID path_id, ETHCAM_PORT_TYPE port_type, UINT32 cmd,UINT32 par);
extern INT32 EthCam_SendXMLData(ETHCAM_PATH_ID path_id, UINT8* addr, UINT32 size);
extern INT32 EthCam_SendXMLStatusCB(ETHCAM_PATH_ID path_id, ETHCAM_PORT_TYPE port_type, UINT32 cmd, UINT32 status);
extern INT32 EthCam_SendXMLStatus(ETHCAM_PATH_ID path_id, ETHCAM_PORT_TYPE port_type, UINT32 cmd, UINT32 status);
extern INT32 EthCam_SendXMLStr(ETHCAM_PATH_ID path_id, ETHCAM_PORT_TYPE port_type, UINT32 cmd, CHAR* str);
extern INT32 EthCam_SendXMLValue(ETHCAM_PATH_ID path_id, ETHCAM_PORT_TYPE port_type, UINT32 cmd, UINT64 value);
extern INT32 EthCamCmd_Send(ETHCAM_PATH_ID path_id, char* addr, int* size);
extern void EthCamCmd_SetResultTable(ETHCAM_XML_RESULT *pAppCmdTbl);
extern ETHCAM_XML_RESULT *EthCamCmd_GetResultTable(void);
extern void EthCamCmd_SndData1Lock(void);
extern void EthCamCmd_SndData1Unlock(void);
extern void EthCamCmd_SndData2Lock(void);
extern void EthCamCmd_SndData2Unlock(void);
extern INT32 EthCamData1_Send(char* addr, int* size);
extern INT32 EthCamData2_Send(char* addr, int* size);
extern void EthCam_GetDest(char *path,  ETHCAM_PATH_ID *path_id, ETHCAM_PORT_TYPE *port_type);
extern void EthCamCmd_GetFrameTimerEn(BOOL En);

#if (AV_LCA_FUNC == ENABLE)  //mio_c90
//#define MIO_LCA_TEST

extern void EthCamRX_para_init(UINT32 LCA_Func_Enable_flag);
extern UINT32 EthCamRX_Get_Vehicle_Status(void);

#endif

#endif //_ETHCAMAPPCMD_H_



