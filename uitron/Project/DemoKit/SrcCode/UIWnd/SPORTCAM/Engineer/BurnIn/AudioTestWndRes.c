//This source code is generated by UI Designer Studio.

#ifndef AUDIOTESTWNDRES_H
#define AUDIOTESTWNDRES_H

#include "UIFramework.h"
#include "UIFrameworkExt.h"
#include "UIGraphics.h"
#include "NVTToolCommand.h"
#include "UIDisplayUtil.h"
//------------------------------------------------------------

SHOW_ITEM_RECT(CMD_Rectangle,Skin1Rect,0,0,319,239,0,0,0,0,0,0,0)
SHOW_MAP_BEGIN(Skin1)
SHOW_MAP_BODY(Skin1Rect)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group,Skin1,0,0,319,239)

SHOW_MAP_BEGIN(AudioTestWnd)
SHOW_MAP_BODY(Skin1)
SHOW_MAP_END

DECLARE_EVENT(AudioTestWnd)
DECLARE_CTRL_LIST(AudioTestWnd)
CREATE_CTRL(AudioTestWnd,AudioTestWnd,CTRL_WND,NULL,0 ,0,0,319,239)
#endif