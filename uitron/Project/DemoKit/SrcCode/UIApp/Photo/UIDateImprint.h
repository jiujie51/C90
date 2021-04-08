#ifndef _UI_DATEIMPRINT_H
#define _UI_DATEIMPRINT_H


#include "imgcapcbmsg.h"
#include "MovieStamp.h"

extern void UiDateImprint_InstallID(void) _SECTION(".kercfg_text");
extern void UiDateImprint_SetEnable(BOOL bEnable);
extern BOOL UiDateImprint_GenData(IMG_CAP_DATASTAMP_INFO *pInfo);
extern void UiDateImprint_InitBuff(void);
extern void UiDateImprint_DestroyBuff(void);
extern void UiDateImprint_WaterLogoConfig(WATERLOGO_BUFFER *pWaterLogoBuf, UINT32 ImgW, UINT32 ImgH, UINT32 CapEvent);
#endif
