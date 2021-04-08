#ifndef _PIPVIEW_H
#define _PIPVIEW_H

#include "AppDisp_View.h"

extern void               PipView_SetStyle(UINT32 uiStyle);
extern INT32              PipView_GetInfo(APPDISP_VIEW_INFO *info);
extern INT32              PipView_OnDraw(APPDISP_VIEW_DRAW *pDraw);
#if (DUALCAM_PIP_BEHIND_FLIP == ENABLE)
extern UINT32 PipView_BFLIP_GetBufAddr(UINT32 blk_size);
extern void   PipView_BFLIP_DestroyBuff(void);
extern void   PipView_BFLIP_SetBuffer(UINT32 uiAddr, UINT32 uiSize);
#endif

#endif
