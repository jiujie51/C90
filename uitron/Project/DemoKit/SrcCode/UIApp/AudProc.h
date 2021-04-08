#ifndef _AUDPROC_H
#define _AUDPROC_H
#include "PrjCfg.h"
#if (AUDPROC_FUNC == ENABLE)
#include "ImageUnit_UserProc.h"
#include "NvtIpcAPI.h"

typedef enum CMD_AISPEECH_WAKEUP {
    CMD_AISPEECH_TAKE_PHOTO = 0,  //take photo
    CMD_AISPEECH_TAKE_PHOTO2,
    CMD_AISPEECH_VIDEO_REC,
    CMD_AISPEECH_OPEN_SCREEN,
    CMD_AISPEECH_CLOSE_SCREEN,
    CMD_AISPEECH_OPEN_HOTSPOT,
    CMD_AISPEECH_CLOSE_HOTSPOT,
    CMD_AISPEECH_OPEN_AUDIO,
    CMD_AISPEECH_CLOSE_AUDIO,
    CMD_AISPEECH_NUM
} CMD_AISPEECH_WAKEUP;

enum{
    AI_SPEECH_AUTH_OK   = 360,
    AI_SPEECH_AUTH_FAIL,
};

typedef struct {
	NVTIPC_U32 count;
	NVTIPC_U32 addr;
	NVTIPC_U32 size;
} AUDPROC_DATA;

typedef struct{
    unsigned int  iLen;
    char szAuthInfo[256];
    char szAuthMac[8];
    int  iAuthStatus;
}AISpeechAuthInfo;

typedef struct {
	NVTIPC_U32 count;
	NVTIPC_U32 result;
} AUDPROC_RESULT;

extern int get_authorization_info(AISpeechAuthInfo *pAuthInfo);
extern void AudProc_InstallID(void);
extern INT32 AudProc_Init(void);
extern INT32 AudProc_UnInit(void);
extern USERPROC_TRIG_PROC_VAL AudProc_UserProc_PushCB(ISF_PORT *pPort, ISF_DATA *pData);
extern int auth_start(void);
#endif
#endif // #ifndef _AUDPROC_H