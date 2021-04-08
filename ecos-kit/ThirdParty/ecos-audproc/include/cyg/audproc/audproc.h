#ifndef _AUDPROC_H
#define _AUDPROC_H
/*
    Copyright (c) 2014~  Novatek Microelectronics Corporation

    @file audproc.h

    @version

    @date
*/

#include <cyg/nvtipc/NvtIpcAPI.h>

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

typedef struct {
	NVTIPC_U32 count;
	NVTIPC_U32 result;
} AUDPROC_RESULT;

typedef struct{
    unsigned int  iLen;
    char szAuthInfo[256];
    char szAuthMac[8];
    int  iAuthStatus;
}AISpeechAuthInfo;


__externC void AudProc_CmdLine(char *szCmd);

#endif /* _AUDPROC_H  */

