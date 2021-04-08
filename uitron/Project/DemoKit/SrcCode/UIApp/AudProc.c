#include "AudProc.h"
#include "FileSysTsk.h"
#include "SwTimer.h"
#include "UISetup.h"
#include "UIFlow.h"
#include "UIAppNetwork.h"
#include "PStore.h"
#if (VOICE_FUNCTION == ENABLE)
#include "DxVoice.h"
#endif

#if (AUDPROC_FUNC == ENABLE)

#define THIS_DBGLVL         2 // 0=FATAL, 1=ERR, 2=WRN, 3=UNIT, 4=FUNC, 5=IND, 6=MSG, 7=VALUE, 8=USER
///////////////////////////////////////////////////////////////////////////////
#define __MODULE__          AudProc
#define __DBGLVL__          ((THIS_DBGLVL>=PRJ_DBG_LVL)?THIS_DBGLVL:PRJ_DBG_LVL)
#define __DBGFLT__          "*" //*=All, [mark]=CustomClass
#include "DebugModule.h"

#include "SysCfg.h"
#include "HwMem.h"

#define AUDPROC_ECOS_VER        20180921

#define PS_AUTH_INFO            "AUTH_INFO"

#define USE_CPU2    ENABLE

#define PRI_AUDPROC             10
#define STKSIZE_AUDPROC       2048

#if (USE_CPU2 == ENABLE)
static NVTIPC_I32 msqid = -1;
UINT32 AUDPROC_TSK_ID = 0;
#endif

static AISpeechAuthInfo stAuthInfo;

static UINT32 is_audproc_opened = 0;
static UINT32 AudBufInAddr[5];       // 5 block to store audio in data
static UINT32 AudBufOutAddr[5];      // 5 block to send audio to cpu2
static UINT32 AudBufOutSize[5];      // 5 block to send audio to cpu2
static UINT32 AudBufIdx;
static UINT32 AudDataCnt;

extern BOOL g_PhotoByApp;


#define PRI_AUTHPROC             12
#define STKSIZE_AUTHPROC       2048*4

UINT32 FLG_ID_AUTH = 0;
UINT32 AUTHPROC_TSK_ID = 0;

const unsigned char  auth_provision[] =
{
0x9D,  0x99,  0xC9,  0x9B,  0x99,  0xC8,  0x9E,  0xC6,  
0xCB,  0x9A,  0x99,  0xCA,  0xCF,  0x9D,  0xC7,  0x9E,  
0xC6,  0xCD,  0xCB,  0xCE,  0x9A,  0xC6,  0x9C,  0xC7,  
0xC9,  0x9B,  0xC9,  0x9D,  0xCC,  0xC9,  0xCD,  0xC9,  
0xCF,  0xCD,  0xC9,  0xC9,  0x9C,  0x9C,  0xCA,  0xCD,  
0x84,  0xDD,  0x91,  0x9E,  0x8B,  0x96,  0x89,  0x9A,  
0xB6,  0x91,  0x89,  0x90,  0x94,  0x9A,  0xBC,  0x90,  
0x8D,  0x9A,  0xAB,  0x86,  0x8F,  0x9A,  0x8C,  0xDD,  
0xC5,  0xA4,  0xDD,  0x9C,  0x91,  0xD1,  0x9E,  0x8C,  
0x8D,  0xD1,  0x8D,  0x9A,  0x9C,  0xDD,  0xD3,  0xDD,  
0x9C,  0x91,  0xD1,  0x98,  0x8D,  0x9E,  0x92,  0xDD,  
0xD3,  0xDD,  0x9C,  0x91,  0xD1,  0x8C,  0x9A,  0x91,  
0x8B,  0xD1,  0x8C,  0x86,  0x91,  0xDD,  0xD3,  0xDD,  
0x9C,  0x91,  0xD1,  0x88,  0x9E,  0x94,  0x9A,  0x8A,  
0x8F,  0xDD,  0xD3,  0xDD,  0x9C,  0x91,  0xD1,  0x9B,  
0x91,  0x91,  0xDD,  0xD3,  0xDD,  0x9C,  0x91,  0xD1,  
0x8C,  0x9B,  0x8C,  0xDD,  0xD3,  0xDD,  0x9C,  0x91,  
0xD1,  0x9B,  0x93,  0x98,  0x8D,  0x9A,  0x8C,  0xDD,  
0xD3,  0xDD,  0x9C,  0x91,  0xD1,  0x88,  0x9E,  0x94,  
0x9A,  0x8A,  0x8F,  0x8D,  0x9A,  0x9C,  0xDD,  0xA2,  
0xD3,  0xDD,  0x89,  0x9A,  0x8D,  0x8C,  0x96,  0x90,  
0x91,  0xDD,  0xC5,  0xDD,  0x9E,  0x96,  0x9A,  0x91,  
0x98,  0x96,  0x91,  0x9A,  0xD2,  0xCD,  0xD1,  0xC6,  
0xD1,  0xCB,  0xDD,  0xD3,  0xDD,  0x9B,  0x9A,  0x89,  
0x96,  0x9C,  0x9A,  0xAD,  0x9A,  0x98,  0x96,  0x8C,  
0x8B,  0x8D,  0x86,  0xDD,  0xC5,  0xDD,  0x97,  0x8B,  
0x8B,  0x8F,  0xC5,  0xA3,  0xD0,  0xA3,  0xD0,  0x9E,  
0x8A,  0x8B,  0x97,  0xD1,  0x9E,  0x8F,  0x96,  0xD1,  
0x9E,  0x96,  0x8C,  0x8F,  0x9A,  0x9A,  0x9C,  0x97,  
0xD1,  0x9C,  0x90,  0x92,  0xA3,  0xD0,  0x9B,  0x9A,  
0x89,  0x96,  0x9C,  0x9A,  0xDD,  0xD3,  0xDD,  0x9B,  
0x9A,  0x89,  0x96,  0x9C,  0x9A,  0xB6,  0x9B,  0xAB,  
0x86,  0x8F,  0x9A,  0xDD,  0xC5,  0xDD,  0x92,  0x9E,  
0x9C,  0xDD,  0xD3,  0xDD,  0x9A,  0x87,  0x8F,  0x96,  
0x8D,  0x9A,  0xDD,  0xC5,  0xDD,  0xCD,  0xCF,  0xCA,  
0xCF,  0xD2,  0xCE,  0xCD,  0xD2,  0xCD,  0xC6,  0xDD,  
0xD3,  0xDD,  0x9E,  0x8F,  0x8F,  0xB4,  0x9A,  0x86,  
0xDD,  0xC5,  0xDD,  0xCE,  0xCA,  0xCC,  0xC7,  0xCE,  
0xCD,  0xC6,  0xCE,  0xC6,  0xCD,  0xCB,  0xCA,  0xC7,  
0xCB,  0x9C,  0x9B,  0xDD,  0x82,  
};

extern BOOL gbNoNeedVocieNotice;
void AudCmd_EventHandle(UINT32 cmd)
{
  switch(cmd)
  {
    case CMD_AISPEECH_TAKE_PHOTO:
	case CMD_AISPEECH_TAKE_PHOTO2:
		CHKPNT;
		 Ux_PostEvent(NVTEVT_KEY_UP, 1, NVTEVT_KEY_PRESS);
		 break;
    case CMD_AISPEECH_VIDEO_REC:
		CHKPNT;
		// Ux_PostEvent(NVTEVT_KEY_LOCK, 1, NVTEVT_KEY_PRESS);
		Ux_PostEvent(NVTEVT_KEY_CUSTOM1, 1, NVTEVT_KEY_RELEASE);//ï¿½ï¿½ï¿½ï¿½Â¼ï¿½ï¿½
		 break;		
    case CMD_AISPEECH_OPEN_AUDIO:
		CHKPNT;
		 if (UI_GetData(FL_MOVIE_AUDIO) == MOVIE_AUDIO_OFF)
		 {
			//UIVoice_Play(1, VOICE_AUDIO_ON);
            UI_SetData(FL_MOVIE_AUDIO, MOVIE_AUDIO_ON);	
			UISound_Play(DEMOSOUND_SOUND_AUDIO_ON_TONE);
			Ux_SendEvent(&CustomMovieObjCtrl,	NVTEVT_EXE_MOVIE_AUDIO, 		1,	SysGetFlag(FL_MOVIE_AUDIO));
		 }
		 else
		 {
		    //UIVoice_Play(1, VOICE_AUDIO_ON);
			UISound_Play(DEMOSOUND_SOUND_AUDIO_ON_TONE);
		 }
		 FlowMovie_UpdateIcons(TRUE);
         break;
		 
	case CMD_AISPEECH_CLOSE_AUDIO:
		CHKPNT;
         if (UI_GetData(FL_MOVIE_AUDIO) == MOVIE_AUDIO_ON)
         {
		   // UIVoice_Play(1, VOICE_AUDIO_OFF);	
			UISound_Play(DEMOSOUND_SOUND_AUDIO_OFF_TONE);
            UI_SetData(FL_MOVIE_AUDIO, MOVIE_AUDIO_OFF);
			Ux_SendEvent(&CustomMovieObjCtrl,	NVTEVT_EXE_MOVIE_AUDIO, 		1,	SysGetFlag(FL_MOVIE_AUDIO));
         }
		 else		 	
		 {
		  //  UIVoice_Play(1, VOICE_AUDIO_OFF);
			UISound_Play(DEMOSOUND_SOUND_AUDIO_OFF_TONE);
         }
		 FlowMovie_UpdateIcons(TRUE);
		 break;	 
		 
    case CMD_AISPEECH_OPEN_HOTSPOT:
		CHKPNT;
		/*
        if (UI_GetData(FL_WIFI_LINK) != WIFI_LINK_OK)
        {
            #if(WIFI_UI_FLOW_VER == WIFI_UI_VER_1_0)
            #if(WIFI_UI_DIRECT_MOBILE_LINKOK == ENABLE)
               BKG_PostEvent(NVTEVT_BKW_WIFI_ON);
            #endif
            #endif
        }	
		//UIVoice_Play(1,VOICE_WIFI_ON);	 
		UISound_Play(DEMOSOUND_SOUND_WIFI_ON_TONE);
		//SysSetFlag(FL_AUTO_WIFI,AUTO_WIFI_ON);*/
		 break;
		 
	case CMD_AISPEECH_CLOSE_HOTSPOT:
		CHKPNT;
		/*
		 if(UI_GetData(FL_WIFI_LINK) == WIFI_LINK_OK)
		 {
			 Ux_SendEvent(0, NVTEVT_EXE_WIFI_STOP, 0);
		 }
		 
         if(System_GetState(SYS_STATE_CURRSUBMODE) == SYS_SUBMODE_WIFI)
         {
            if(WiFiCmd_GetStatus() == WIFI_MOV_ST_RECORD)
               gbNoNeedVocieNotice = TRUE;
            #if (WIFI_UI_FLOW_VER == WIFI_UI_VER_2_0 || WIFI_UI_DIRECT_MOBILE_LINKOK == ENABLE)
            System_ChangeToNormalMode();
            #endif
         }
		UIVoice_Play(1,VOICE_WIFI_OFF);	 
		UISound_Play(DEMOSOUND_SOUND_WIFI_OFF_TONE);
		SysSetFlag(FL_AUTO_WIFI,AUTO_WIFI_OFF);
		*/
		 break;
		 
    default:
		 break;
		 
  }
   
}
void AuthProcTsk(void);

void AudProcTsk(void)
{
#if (USE_CPU2 == ENABLE)
	NVTIPC_I32 ipcErr = 0;
	AUDPROC_RESULT ipcMsgRcv;
    int seq = 0;
	CHKPNT;
	kent_tsk();

	while (1) {
		CHKPNT;
		ipcErr = NvtIPC_MsgRcv(msqid, &ipcMsgRcv, sizeof(ipcMsgRcv));
		// release buffer here!
		DBGD(ipcErr);
		#if 1
		UINT32 uiEvent=(UINT32)ipcMsgRcv.count;
		//#if(_VERSON_TYPE_ != _VERSON_SHOULANG_)
		if(uiEvent != 100)
		{
			debug_msg("Liwk ------- AuProcTsk Event:%d\r\n",uiEvent);
			#if 1//(_VERSON_TYPE_ == _VERSON_SYX_NORMAL_)
			//if(SysGetFlag(FL_OPENNING_AUDIO) == OPENNING_AUDIO_ON)
			#endif
			AudCmd_EventHandle(uiEvent);
		}
		//#endif
		#endif
		
		if(seq++%10 == 0)DBG_DUMP("Rcv cnt%d, result=%d, cmd=%d\r\n", ipcMsgRcv.count, ipcMsgRcv.result, ipcMsgRcv.count);
	}
	ext_tsk();
#endif
}

void AudProc_InstallID(void)
{
#if (USE_CPU2 == ENABLE)
	OS_CONFIG_TASK(AUDPROC_TSK_ID, PRI_AUDPROC, STKSIZE_AUDPROC, AudProcTsk);
#endif
	if(AUDPROC_TSK_ID !=0){
		debug_msg("audproc task install success!\r\n");
	}
 //   OS_CONFIG_TASK(AUTHPROC_TSK_ID, PRI_AUTHPROC, STKSIZE_AUTHPROC, AuthProcTsk);
   // OS_CONFIG_FLAG(FLG_ID_AUTH);
}

INT32 AudProc_Init(void)
{
	UINT32 i;
    UINT32 uiAuthInfo = 0;

	if (is_audproc_opened == 1) {
		DBG_ERR("AudProc is already opened!\n");
		return -1;
	}
    
#if (USE_CPU2 == ENABLE)
	CHAR ipccmd[64];
	NVTIPC_SYS_MSG sysMsg;
	NVTIPC_I32 ipcErr = 0;
    debug_msg("Liwk ------------  AudProc_Init!!\r\n");
    //DBG_ERR("Failed to NVTIPC_SYS_QUEUE_ID\r\n");

	if (msqid < 0) {
		msqid = NvtIPC_MsgGet(NvtIPC_Ftok("audproc"));
	}
    get_authorization_info(&stAuthInfo);
    uiAuthInfo = (UINT32)&stAuthInfo;
	snprintf(ipccmd, sizeof(ipccmd) - 1, "audproc -open %d %d", AUDPROC_ECOS_VER, uiAuthInfo);
	sysMsg.sysCmdID = NVTIPC_SYSCMD_SYSCALL_REQ;
	sysMsg.DataAddr = (UINT32)ipccmd;
	sysMsg.DataSize = strlen(ipccmd) + 1;
	if ((ipcErr = NvtIPC_MsgSnd(NVTIPC_SYS_QUEUE_ID, NVTIPC_SENDTO_CORE2, &sysMsg, sizeof(sysMsg))) < 0) {
		DBG_ERR("Failed to NVTIPC_SYS_QUEUE_ID\r\n");
	}
	sta_tsk(AUDPROC_TSK_ID, 0);
#endif

	for (i = 0; i < 5; i ++) {
		AudBufInAddr[i] = OS_GetMempoolAddr(POOL_ID_AUDPROC) + i * OS_GetMempoolSize(POOL_ID_AUDPROC) / 5;
		AudBufOutAddr[i] = OS_GetMempoolAddr(POOL_ID_AUDPROC) + i * 1920;
		AudBufOutSize[i] = (i == 4) ? 2560 : 1920;
		//DBG_DUMP("AudBufInAddr[%d] =%x\r\n", i, AudBufInAddr[i]);
		//DBG_DUMP("AudBufOutAddr[%d]=%x, size=%x\r\n", i, AudBufOutAddr[i], AudBufOutSize[i]);
	}
	AudBufIdx = 0;
	AudDataCnt = 0;

	is_audproc_opened = 1;

	return 0;
}

INT32 AudProc_UnInit(void)
{
	if (is_audproc_opened == 0) {
		DBG_ERR("AudProc is already closed!\n");
		return -1;
	}
#if (USE_CPU2 == ENABLE)
	CHAR ipccmd[64];
	NVTIPC_SYS_MSG sysMsg;
	NVTIPC_I32 ipcErr = 0;

	snprintf(ipccmd, sizeof(ipccmd) - 1, "audproc -close");
	sysMsg.sysCmdID = NVTIPC_SYSCMD_SYSCALL_REQ;
	sysMsg.DataAddr = (UINT32)ipccmd;
	sysMsg.DataSize = strlen(ipccmd) + 1;
	if ((ipcErr = NvtIPC_MsgSnd(NVTIPC_SYS_QUEUE_ID, NVTIPC_SENDTO_CORE2, &sysMsg, sizeof(sysMsg))) < 0) {
		DBG_ERR("Failed to NVTIPC_SYS_QUEUE_ID\r\n");
	}
	ter_tsk(AUDPROC_TSK_ID);

	if (msqid >= 0) {
		NvtIPC_MsgRel(msqid);
		msqid = -1;
	}
#endif
	is_audproc_opened = 0;

	return 0;
}

USERPROC_TRIG_PROC_VAL AudProc_UserProc_PushCB(ISF_PORT *pPort, ISF_DATA *pData)
{
#if (USE_CPU2 == ENABLE)
	UINT32 i;
	ISF_AUDIO_STREAM_BUF *pAudBuf = (ISF_AUDIO_STREAM_BUF *)&(pData->Desc[0]);
	NVTIPC_I32 ipcErr = 0;
	AUDPROC_DATA ipcMsgSnd;

	for (i = 0; i < pAudBuf->DataSize / 2048; i ++) {
		hwmem_open();
		hwmem_memcpy(AudBufInAddr[AudBufIdx], pAudBuf->DataAddr, 2048);
		hwmem_close();
		ipcMsgSnd.count = AudDataCnt;
		ipcMsgSnd.addr = NvtIPC_GetPhyAddr(AudBufOutAddr[AudBufIdx]);
		ipcMsgSnd.size = AudBufOutSize[AudBufIdx];
		//DBG_DUMP("Get AudIn Sample %x(%x) (%d/%d)\r\n", AudBufInAddr[AudBufIdx], ipcMsgSnd.addr, AudBufIdx, AudDataCnt);
		ipcErr = NvtIPC_MsgSnd(msqid, NVTIPC_SENDTO_CORE2, &ipcMsgSnd, sizeof(ipcMsgSnd));
		AudBufIdx = (AudBufIdx + 1) % 5;
		AudDataCnt ++;
	}
#endif
	return USERPROC_TRIG_PROC_NONE;
}

////////////////////////////////////AIENGINE TEST CODE//////////////////////////////////////////////////////////
#include "aiengine.h"
#define AUTH_REQ_HEADER_PARAM \
        "GET /%s HTTP/1.1\r\n" \
        "Host: %s\r\n" \
        "Connection:keep-alive\r\n" \
        "\r\n"

//#define AISPEECH_AUTH_PROVISION_PATH "A:\\aiengine.provision"
//#define AISPEECH_AUTH_INFO_PATH "A:\\aiengine.serialNumber"

#define AUTH_FLAG_DATAREADY      1<<1
#define AUTH_FLAG_NETREADY       1<<2
#define AUTH_FLAG_ALL            0xFFFFFFFF

char res_body[256];
char ap_ssid[32] = {0};
char ap_passwd[26] = {0};
char auth_server[16] = {0};
extern char gMacAddr[6];


static int get_device_id(char deviceId[64], const char *type) {
//    memcpy(deviceId, "123456111145", 12);
	//memcpy(deviceId, "987654321012", 12);
    sprintf(deviceId, "%02x%02x%02x%02x%02x%02x", 
            gMacAddr[0], gMacAddr[1], gMacAddr[2], 
            gMacAddr[3], gMacAddr[4], gMacAddr[5]);

	return 0;
}

int http_request(const char *url, char *body)
{
    return 0;
}

int http_get_response(const char *res)
{
    return 0;
}

// static int provision_http_request(void *userdata, const char *url, char response[1024])
static int provision_http_request(void *userdata, const char *url, char **response)
{
    return 0;
}

BOOL wifi_sta_connect(char *ssid, char *passwd)
{
    
	return TRUE;
}

int check_test_file(void)
{
	return 0;
}

int auth_check_info(char *buf, int *psize)
{	

	return -1;
}


int auth_save_info(const char *buf, int size)
{	

	return 0;
}

int get_authorization_info(AISpeechAuthInfo *pAuthInfo)
{
	return 0;
}
int is_aispeech_auth(void){
    int rval = 0;
    AISpeechAuthInfo stAuthInfo = {0};
    rval = get_authorization_info(&stAuthInfo);
    if(!rval)
        return TRUE;
    else
        return FALSE;
}

int aiengine_wakeup(void) 
{
	//struct aiengine *engine = NULL;
	char *engineCfg = NULL;
	//char *engineParam = NULL;
	//char id[64];
	//int wakeup = 0;
	int ret = -1;
	char *err_info = NULL;
    int len = 0;

	engineCfg = "{\
                    \"appKey\":\"15381291924584cd\",\
                    \"secretKey\":\"b04e0978acab6df89f94efd45a8bdf73\",\
                    \"provision\":\"F:\\\\aiengine.provision\",\
                    \"serialNumber\":\"F:\\\\aiengine.serialNumber\",\
                    \"version\":\"aiengine-2.9.4\",\
                    \"native\":{\
                        \"cn.wakeup\":{\
                            \"resBinPath\":\"\"\
                        }\
                    }\
                }";

#if 0
	engineParam =
			"{\
        \"request\":{\
            \"env\":\"words=wo yao pai zhao,zhua pai zhao pian,wo yao lu xiang,da kai ping mu,guan bi ping mu,da kai lu yin,guan bi lu yin,da kai re dian,guan bi re dian,da kai qian lu,xian shi qian lu,da kai hou lu,xian shi hou lu,zhua pai shi ping;thresh=0.26,0.09,0.24,0.22,0.23,0.24,0.17,0.24,0.16,0.24,0.22,0.26,0.27,0.11;\"\
        }\
    }";
#endif

	aiauth* aiauth_s = (aiauth*)malloc(sizeof(aiauth));
	if (aiauth_s) memset(aiauth_s,0,sizeof(aiauth));

	//¨º¨²¨¨¡§?¡è¨º¨¦
	#if 0
	int content_len = 0;
	FST_FILE file_fd = FileSys_OpenFile(AISPEECH_AUTH_PROVISION_PATH, FST_OPEN_READ);
	debug_msg("AISPEECH_AUTH_PROVISION_PATH %s", AISPEECH_AUTH_PROVISION_PATH);
	if(file_fd != NULL) {
		FileSys_SeekFile(file_fd, (UINT64)0, FST_SEEK_END);
		content_len = FileSys_TellFile(file_fd);
		int malloc_len = content_len + 1;
		FileSys_SeekFile(file_fd, (UINT64)0, FST_SEEK_SET);

		aiauth_s->provision = (char *)malloc(sizeof(char) * malloc_len);
		if(aiauth_s->provision != NULL) {
			memset(aiauth_s->provision, 0, malloc_len);
			int readBytes = content_len;
			FileSys_ReadFile(file_fd, (UINT8 *)aiauth_s->provision, (UINT32 *)&readBytes, 0, NULL);
			if(readBytes == content_len) {
				aiauth_s->provision_size = content_len;
			}
		}

		debug_msg("AISPEECH_AUTH_PROVISION_PATH <%d> \n", aiauth_s->provision_size);
	}
    #else 
    aiauth_s->provision = (char *)malloc(sizeof(char) * sizeof(auth_provision));
    if(aiauth_s->provision != NULL) {
    	//aiauth_s->provision = auth_provision;
    	memcpy(aiauth_s->provision, auth_provision, sizeof(auth_provision));
        aiauth_s->provision_size = sizeof(auth_provision);
    }
    #endif

	//¨º¨²¨¨¡§?¨¢1?
	#if 0
	int content_len1 = 0;
	FST_FILE file_fd1 = FileSys_OpenFile(AISPEECH_AUTH_INFO_PATH, FST_OPEN_READ);
	if(file_fd1 != NULL) {
		FileSys_SeekFile(file_fd1, (UINT64)0, FST_SEEK_END);
		content_len1 = FileSys_TellFile(file_fd1);
		int malloc_len = content_len1 + 1;
		FileSys_SeekFile(file_fd1, (UINT64)0, FST_SEEK_SET);

		aiauth_s->serialnumber = (char *)malloc(sizeof(char) * malloc_len);
		if(aiauth_s->serialnumber != NULL) {
			memset(aiauth_s->serialnumber, 0, malloc_len);
			int readBytes = content_len1;
			FileSys_ReadFile(file_fd1, (UINT8 *)aiauth_s->serialnumber, (UINT32 *)&readBytes, 0, NULL);
			if(readBytes == content_len1) {
				aiauth_s->serialnumber_size = content_len1;
			}
		}

		debug_msg("AISPEECH_AUTH_INFO_PATH <%d> \n", aiauth_s->serialnumber_size);
	}
    #else
    aiauth_s->serialnumber = (char *)malloc(sizeof(char) * 256);
    if(aiauth_s->serialnumber != NULL) {
        if (auth_check_info(aiauth_s->serialnumber, &len) == 0) {
            debug_msg("auth_check_info ok\r\n");
            aiauth_s->serialnumber_size = len;
        }
    }
    #endif

	ret = aiengine_check_auth(engineCfg, &err_info, get_device_id, aiauth_s);
	if (ret != 0) {
        debug_msg("check auth error: %s\n", err_info);
        int serialnumber_size = 0;
        char* serialnumber = NULL;
        ret = aiengine_do_auth(engineCfg, &err_info, provision_http_request, get_device_id, NULL, aiauth_s, &serialnumber, &serialnumber_size);
		if (ret != 0) {
			debug_msg("do auth error: %s\n", err_info);
		} else {
            debug_msg("do auth ok\r\n");
            debug_msg("aiengine read size %d\r\n", serialnumber_size);

            if (auth_save_info(serialnumber, serialnumber_size) == 0) {
                debug_msg("auth_save_info ok\r\n");
            }
#if 0
            debug_msg("aiengine serialnumber = %d\r\n", (int)serialnumber);
            //¨¤?¨®?¡¤¦Ì??¦Ì?¨ºy?Y¡Á?¨º¨²¨¨¡§?¨¬2¨¦
            memset(aiauth_s->serialnumber, 0, aiauth_s->serialnumber_size);

            memcpy(aiauth_s->serialnumber, serialnumber, serialnumber_size);
            aiauth_s->serialnumber_size = serialnumber_size;

            ret = aiengine_check_auth(engineCfg, &err_info, get_device_id, aiauth_s);

            if (ret == 0) {
                debug_msg("AAAAAAAAAAAAAAAAAAAAAA do auth ok ==========================!!!\r\n");
            }
#endif
		}
	} else {
		debug_msg("check auth success\r\n");
	}

    if(aiauth_s->provision != NULL) {
        free(aiauth_s->provision);
    }

    if(aiauth_s->serialnumber != NULL) {
        free(aiauth_s->serialnumber);
    }

    if (aiauth_s) {
        free(aiauth_s);
    }

	return ret;
}


int cec_check_auth(void) 
{
	char *engineCfg = NULL;
	int ret = -1;
	char *err_info = NULL;
    int len = 0;

	engineCfg = "{\
                    \"appKey\":\"15381291924584cd\",\
                    \"secretKey\":\"b04e0978acab6df89f94efd45a8bdf73\",\
                    \"provision\":\"F:\\\\aiengine.provision\",\
                    \"serialNumber\":\"F:\\\\aiengine.serialNumber\",\
                    \"version\":\"aiengine-2.9.4\",\
                    \"native\":{\
                        \"cn.wakeup\":{\
                            \"resBinPath\":\"\"\
                        }\
                    }\
                }";


	aiauth* aiauth_s = (aiauth*)malloc(sizeof(aiauth));
	if (aiauth_s) memset(aiauth_s,0,sizeof(aiauth));

	//¨º¨²¨¨¡§?¡è¨º¨¦
    aiauth_s->provision = (char *)malloc(sizeof(char) * sizeof(auth_provision));
    if(aiauth_s->provision != NULL) {
    	//aiauth_s->provision = auth_provision;
    	memcpy(aiauth_s->provision, auth_provision, sizeof(auth_provision));
        aiauth_s->provision_size = sizeof(auth_provision);
    }

	//¨º¨²¨¨¡§?¨¢1?
    aiauth_s->serialnumber = (char *)malloc(sizeof(char) * 256);
    if(aiauth_s->serialnumber != NULL) {
        if (auth_check_info(aiauth_s->serialnumber, &len) == 0) {
            debug_msg("auth_check_info ok\r\n");
            aiauth_s->serialnumber_size = len;
        }
    }

	ret = aiengine_check_auth(engineCfg, &err_info, get_device_id, aiauth_s);
	if (ret != 0) {
        debug_msg("check auth error: %s\n", err_info);
	} else {
		debug_msg("check auth success\r\n");
	}

    if(aiauth_s->provision != NULL) {
        free(aiauth_s->provision);
    }

    if(aiauth_s->serialnumber != NULL) {
        free(aiauth_s->serialnumber);
    }

    if (aiauth_s) {
        free(aiauth_s);
    }

	return ret;
}

void auth_net_ready(void)
{
    set_flg(FLG_ID_AUTH, AUTH_FLAG_NETREADY);
}

void auth_data_ready(void)
{
    set_flg(FLG_ID_AUTH, AUTH_FLAG_DATAREADY);
}

int auth_start(void)
{   
    sta_tsk(AUTHPROC_TSK_ID, 0);
    return 0;
}

void AuthProcTsk(void)
{
    FLGPTN recvFlag;
	kent_tsk();

    debug_msg("########  AuthProcTsk start\r\n");
    clr_flg(FLG_ID_AUTH, AUTH_FLAG_ALL);

    //debug_msg("start time %d\r\n", Perf_GetCurrent());
    if (0 == check_test_file()) {
        wifi_sta_connect(ap_ssid, ap_passwd);

        debug_msg("###1####  recvFlag = %d \r\n", recvFlag);
        wai_flg(&recvFlag, FLG_ID_AUTH, (AUTH_FLAG_NETREADY | AUTH_FLAG_DATAREADY), 
            TWF_ORW|TWF_CLR);
        debug_msg("###2####  recvFlag = %d \r\n", recvFlag);
        if (recvFlag == AUTH_FLAG_NETREADY) {
            aiengine_wakeup();
        }
    }
    
    debug_msg("########  AuthProcTsk end\r\n");
    //debug_msg("end time %d\r\n", Perf_GetCurrent());
	ext_tsk();
}

////////////////////////////////////AIENGINE TEST CODE///////////////////////////////////////////////////////////////

#endif
