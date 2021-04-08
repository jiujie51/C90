//This source code is generated by UI Designer Studio.

#ifndef UIFLOWWNDWAITMOMENTRES_H
#define UIFLOWWNDWAITMOMENTRES_H

#include "UIFramework.h"
#include "UIFrameworkExt.h"
#include "UIGraphics.h"
#include "NVTToolCommand.h"
#include "UIDisplayUtil.h"
//------------------------------------------------------------

SHOW_ITEM_RECT(CMD_Rectangle,Skin1Rect,0,0,1279,319,0,0,0,0,0,0,0)
SHOW_MAP_BEGIN(Skin1)
SHOW_MAP_BODY(Skin1Rect)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group,Skin1,0,0,1279,319)

SHOW_ITEM_RECT(CMD_Rectangle,Rect5,0,0,689,142,16,1,189,189,0,0,0)
SHOW_ITEM_RECT(CMD_Rectangle,Skin3Rect,0,0,689,143,0,0,189,189,0,0,0)
SHOW_MAP_BEGIN(Skin3)
SHOW_MAP_BODY(Skin3Rect)
SHOW_MAP_BODY(Rect5)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group,Skin3,0,0,689,143)

SHOW_ITEM_TEXT(CMD_Text|CMD_ITEM,Text7,1,1,687,142,STRID_NULL,0,0,0,0,26,1,1,0,0,17,0,0,0,0)
SHOW_MAP_BEGIN(Normal2)
SHOW_MAP_BODY(Skin3)
SHOW_MAP_BODY(Text7)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group,Normal2,283,69,972,212)

CREATE_STATE_ITEM_DATA(STRID_ENTER_PARKING8,STRID_ENTER_PARKING,ICONID_NULL,NULL) 
CREATE_STATE_ITEM_DATA(STRID_PLEASE_WAIT9,STRID_PLEASE_WAIT,ICONID_NULL,NULL) 
CREATE_STATE_ITEM_DATA(STRID_FORMACT_SD10,STRID_FORMACT_SD,ICONID_NULL,NULL) 
CREATE_STATE_ITEM_DATA(STRID_ETHCAM_DETFAIL11,STRID_ETHCAM_DETFAIL,ICONID_NULL,NULL) 
CREATE_STATE_ITEM_DATA(STRID_RESTART_REC12,STRID_RESTART_REC,ICONID_NULL,NULL) 
CREATE_STATE_ITEM_DATA(STRID_STOPREC_WAIT13,STRID_STOPREC_WAIT,ICONID_NULL,NULL) 
CREATE_STATE_ITEM_DATA(STRID_ETHCAM_UDFW_SENDFW14,STRID_ETHCAM_UDFW_SENDFW,ICONID_NULL,NULL) 
CREATE_STATE_ITEM_DATA(STRID_ETHCAM_UDFW_START15,STRID_ETHCAM_UDFW_START,ICONID_NULL,NULL) 
CREATE_STATE_ITEM_DATA(STRID_ETHCAM_UDFW_FINISH16,STRID_ETHCAM_UDFW_FINISH,ICONID_NULL,NULL) 
CREATE_STATE_ITEM_DATA(STRID_MOVIE_BACK_MSG17,STRID_MOVIE_BACK_MSG,ICONID_NULL,NULL) 
CREATE_STATE_ITEM_DATA(STRID_ETHCAM_CHANGE_BEHINDCAM18,STRID_ETHCAM_CHANGE_BEHINDCAM,ICONID_NULL,NULL) 
CREATE_STATE_ITEM_DATA(STRID_ENTER_SHUTDOWN19,STRID_ENTER_SHUTDOWN,ICONID_NULL,NULL) 
CREATE_STATE_ITEM_DATA(STRID_ETHCAM_CHANGE_FRONTCAM20,STRID_ETHCAM_CHANGE_FRONTCAM,ICONID_NULL,NULL) 
CREATE_STATE_ITEM_DATA(STRID_SLOW_CARD21,STRID_SLOW_CARD,ICONID_NULL,NULL) 
CREATE_STATE_ITEM_DATA(STRID_ETHCAM_FW_LOSTED22,STRID_ETHCAM_FW_LOSTED,ICONID_NULL,NULL) 
CREATE_STATE_ITEM_DATA(STRID_ETHCAM_UPDATE_FAILED23,STRID_ETHCAM_UPDATE_FAILED,ICONID_NULL,NULL) 
CREATE_STATE_ITEM_DATA(STRID_ETHCAM_SENDFW_FAILED24,STRID_ETHCAM_SENDFW_FAILED,ICONID_NULL,NULL) 
SHOW_MAP_BEGIN(UIFlowWndWaitMoment_StatusTXT_Msg)
SHOW_MAP_BODY(Normal2)
SHOW_MAP_END

DECLARE_EVENT(UIFlowWndWaitMoment_StatusTXT_Msg)
STATE_LIST_BEGIN(UIFlowWndWaitMoment_StatusTXT_Msg)
STATE_LIST_ITEM(STRID_ENTER_PARKING8)
STATE_LIST_ITEM(STRID_PLEASE_WAIT9)
STATE_LIST_ITEM(STRID_FORMACT_SD10)
STATE_LIST_ITEM(STRID_ETHCAM_DETFAIL11)
STATE_LIST_ITEM(STRID_RESTART_REC12)
STATE_LIST_ITEM(STRID_STOPREC_WAIT13)
STATE_LIST_ITEM(STRID_ETHCAM_UDFW_SENDFW14)
STATE_LIST_ITEM(STRID_ETHCAM_UDFW_START15)
STATE_LIST_ITEM(STRID_ETHCAM_UDFW_FINISH16)
STATE_LIST_ITEM(STRID_MOVIE_BACK_MSG17)
STATE_LIST_ITEM(STRID_ETHCAM_CHANGE_BEHINDCAM18)
STATE_LIST_ITEM(STRID_ENTER_SHUTDOWN19)
STATE_LIST_ITEM(STRID_ETHCAM_CHANGE_FRONTCAM20)
STATE_LIST_ITEM(STRID_SLOW_CARD21)
STATE_LIST_ITEM(STRID_ETHCAM_FW_LOSTED22)
STATE_LIST_ITEM(STRID_ETHCAM_UPDATE_FAILED23)
STATE_LIST_ITEM(STRID_ETHCAM_SENDFW_FAILED24)
STATE_LIST_END

CREATE_STATE_DATA(UIFlowWndWaitMoment_StatusTXT_Msg,0)
CREATE_STATE_CTRL(UIFlowWndWaitMoment_StatusTXT_Msg,0,283,69,972,212)

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

SHOW_ITEM_RECT(CMD_Rectangle,Skin22Rect,0,0,316,44,0,0,189,189,0,0,0)
SHOW_MAP_BEGIN(Skin22)
SHOW_MAP_BODY(Skin22Rect)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group,Skin22,0,0,316,44)

SHOW_ITEM_TEXT(CMD_Text|CMD_ITEM,Text24,1,0,317,44,STRID_NULL,0,0,0,0,26,1,1,0,0,17,0,0,0,0)
SHOW_MAP_BEGIN(Normal21)
SHOW_MAP_BODY(Skin22)
SHOW_MAP_BODY(Text24)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group,Normal21,283,212,599,256)

SHOW_ITEM_RECT(CMD_Rectangle,Skin26Rect,0,0,316,44,0,0,189,189,0,0,0)
SHOW_MAP_BEGIN(Skin26)
SHOW_MAP_BODY(Skin26Rect)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group,Skin26,0,0,316,44)

SHOW_ITEM_TEXT(CMD_Text|CMD_ITEM,Text28,1,0,317,44,STRID_NULL,0,0,0,0,26,0,0,0,0,17,0,0,0,0)
SHOW_MAP_BEGIN(Focused25)
SHOW_MAP_BODY(Skin26)
SHOW_MAP_BODY(Text28)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group,Focused25,283,212,599,256)

SHOW_MAP_BEGIN(Skin30)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group,Skin30,0,0,316,44)

SHOW_ITEM_TEXT(CMD_Text|CMD_ITEM,Text32,1,0,317,44,STRID_NULL,0,0,0,0,26,1,1,0,0,17,0,0,0,0)
SHOW_MAP_BEGIN(Normal_Disable29)
SHOW_MAP_BODY(Skin30)
SHOW_MAP_BODY(Text32)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group,Normal_Disable29,283,212,599,256)

SHOW_MAP_BEGIN(Skin34)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group,Skin34,0,0,316,44)

SHOW_ITEM_TEXT(CMD_Text|CMD_ITEM,Text36,1,0,317,44,STRID_NULL,0,0,0,0,26,1,1,0,0,17,0,0,0,0)
SHOW_MAP_BEGIN(Focused_Disable33)
SHOW_MAP_BODY(Skin34)
SHOW_MAP_BODY(Text36)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group,Focused_Disable33,283,212,599,256)

SHOW_ITEM_RECT(CMD_Rectangle,Skin38Rect,0,0,316,44,0,0,189,189,0,0,0)
SHOW_MAP_BEGIN(Skin38)
SHOW_MAP_BODY(Skin38Rect)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group,Skin38,0,0,316,44)

SHOW_ITEM_TEXT(CMD_Text|CMD_ITEM,Text40,1,0,317,44,STRID_NULL,0,0,0,0,26,1,1,0,0,17,0,0,0,0)
SHOW_MAP_BEGIN(Pressed37)
SHOW_MAP_BODY(Skin38)
SHOW_MAP_BODY(Text40)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group,Pressed37,283,212,599,256)

CREATE_BUTTON_ITEM_DATA(STRID_NO41,STRID_NO,ICONID_NULL,STATUS_ENABLE,NULL) 
SHOW_MAP_BEGIN(UIFlowWndWaitMomentButtonNo)
SHOW_MAP_BODY(Normal21)
SHOW_MAP_BODY(Focused25)
SHOW_MAP_BODY(Normal_Disable29)
SHOW_MAP_BODY(Focused_Disable33)
SHOW_MAP_BODY(Pressed37)
SHOW_MAP_END

DECLARE_EVENT(UIFlowWndWaitMomentButtonNo)
BUTTON_LIST_BEGIN(UIFlowWndWaitMomentButtonNo)
BUTTON_LIST_ITEM(STRID_NO41)
BUTTON_LIST_END

CREATE_BUTTON_DATA(UIFlowWndWaitMomentButtonNo,BTN_DRAW_IMAGE)

CREATE_BUTTON_CTRL(UIFlowWndWaitMomentButtonNo,UIFlowWndWaitMomentButtonNo,0,283,212,599,256)
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

SHOW_ITEM_RECT(CMD_Rectangle,Skin43Rect,0,0,372,44,0,0,189,189,0,0,0)
SHOW_MAP_BEGIN(Skin43)
SHOW_MAP_BODY(Skin43Rect)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group,Skin43,0,0,372,44)

SHOW_ITEM_TEXT(CMD_Text|CMD_ITEM,Text45,0,0,372,44,STRID_NULL,0,0,0,0,26,1,1,0,0,17,0,0,0,0)
SHOW_MAP_BEGIN(Normal42)
SHOW_MAP_BODY(Skin43)
SHOW_MAP_BODY(Text45)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group,Normal42,600,212,972,256)

SHOW_ITEM_RECT(CMD_Rectangle,Skin47Rect,0,0,372,44,0,0,189,189,0,0,0)
SHOW_MAP_BEGIN(Skin47)
SHOW_MAP_BODY(Skin47Rect)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group,Skin47,0,0,372,44)

SHOW_ITEM_TEXT(CMD_Text|CMD_ITEM,Text49,0,0,372,44,STRID_NULL,0,0,0,0,26,0,0,0,0,17,0,0,0,0)
SHOW_MAP_BEGIN(Focused46)
SHOW_MAP_BODY(Skin47)
SHOW_MAP_BODY(Text49)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group,Focused46,600,212,972,256)

SHOW_MAP_BEGIN(Skin51)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group,Skin51,0,0,372,44)

SHOW_ITEM_TEXT(CMD_Text|CMD_ITEM,Text53,0,0,372,44,STRID_NULL,0,0,0,0,26,1,1,0,0,17,0,0,0,0)
SHOW_MAP_BEGIN(Normal_Disable50)
SHOW_MAP_BODY(Skin51)
SHOW_MAP_BODY(Text53)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group,Normal_Disable50,600,212,972,256)

SHOW_MAP_BEGIN(Skin55)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group,Skin55,0,0,372,44)

SHOW_ITEM_TEXT(CMD_Text|CMD_ITEM,Text57,0,0,372,44,STRID_NULL,0,0,0,0,26,1,1,0,0,17,0,0,0,0)
SHOW_MAP_BEGIN(Focused_Disable54)
SHOW_MAP_BODY(Skin55)
SHOW_MAP_BODY(Text57)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group,Focused_Disable54,600,212,972,256)

SHOW_ITEM_RECT(CMD_Rectangle,Skin59Rect,0,0,372,44,0,0,189,189,0,0,0)
SHOW_MAP_BEGIN(Skin59)
SHOW_MAP_BODY(Skin59Rect)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group,Skin59,0,0,372,44)

SHOW_ITEM_TEXT(CMD_Text|CMD_ITEM,Text61,0,0,372,44,STRID_NULL,0,0,0,0,26,1,1,0,0,17,0,0,0,0)
SHOW_MAP_BEGIN(Pressed58)
SHOW_MAP_BODY(Skin59)
SHOW_MAP_BODY(Text61)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group,Pressed58,600,212,972,256)

CREATE_BUTTON_ITEM_DATA(STRID_YES62,STRID_YES,ICONID_NULL,STATUS_ENABLE,NULL) 
SHOW_MAP_BEGIN(UIFlowWndWaitMomentButtonYes)
SHOW_MAP_BODY(Normal42)
SHOW_MAP_BODY(Focused46)
SHOW_MAP_BODY(Normal_Disable50)
SHOW_MAP_BODY(Focused_Disable54)
SHOW_MAP_BODY(Pressed58)
SHOW_MAP_END

DECLARE_EVENT(UIFlowWndWaitMomentButtonYes)
BUTTON_LIST_BEGIN(UIFlowWndWaitMomentButtonYes)
BUTTON_LIST_ITEM(STRID_YES62)
BUTTON_LIST_END

CREATE_BUTTON_DATA(UIFlowWndWaitMomentButtonYes,BTN_DRAW_IMAGE)

CREATE_BUTTON_CTRL(UIFlowWndWaitMomentButtonYes,UIFlowWndWaitMomentButtonYes,0,600,212,972,256)
SHOW_ITEM_ROUNDRECT(CMD_RoundRect,RoundRect66,0,0,689,24,16,2,0,253,0,0,0,16,14)
SHOW_ITEM_RECT(CMD_Rectangle,Skin64Rect,0,0,687,24,0,0,253,253,0,0,0)
SHOW_MAP_BEGIN(Skin64)
SHOW_MAP_BODY(Skin64Rect)
SHOW_MAP_BODY(RoundRect66)
SHOW_MAP_END
SHOW_ITEM_GROUP(CMD_Group,Skin64,0,0,687,24)
SHOW_ITEM_ROUNDRECT(CMD_RoundRect,RoundRect69,0,0,688,24,16,2,0,234,0,0,0,16,20)
SHOW_ITEM_RECT(CMD_Rectangle,Thumb67Rect,0,0,689,34,0,0,0,0,0,0,0)
SHOW_MAP_BEGIN(Thumb67)
SHOW_MAP_BODY(Thumb67Rect)
SHOW_MAP_BODY(RoundRect69)
SHOW_MAP_END
SHOW_ITEM_GROUP(CMD_Group,Thumb67,0,0,689,34)
SHOW_MAP_BEGIN(Normal63)
SHOW_MAP_BODY(Skin64)
SHOW_MAP_BODY(Thumb67)
SHOW_MAP_END
SHOW_ITEM_GROUP(CMD_Group,Normal63,283,212,970,236)
SHOW_MAP_BEGIN(UIFlowWndWaitMoment_ProgressBar)
SHOW_MAP_BODY(Normal63)
SHOW_MAP_END
DECLARE_EVENT(UIFlowWndWaitMoment_ProgressBar)
CTRL_LIST_BEGIN(UIFlowWndWaitMoment_ProgressBar)
CTRL_LIST_END
CREATE_PROBAR_DATA(UIFlowWndWaitMoment_ProgressBar,PROBAR_HORIZONTAL|PROBAR_NORMAL,0,100)
CREATE_PROBAR_CTRL(UIFlowWndWaitMoment_ProgressBar,UIFlowWndWaitMoment_ProgressBar,0,283,212,970,236)
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

SHOW_MAP_BEGIN(UIFlowWndWaitMoment)
SHOW_MAP_BODY(Skin1)
SHOW_MAP_END

DECLARE_EVENT(UIFlowWndWaitMoment)
DECLARE_CTRL_LIST(UIFlowWndWaitMoment)
CREATE_CTRL(UIFlowWndWaitMoment,UIFlowWndWaitMoment,CTRL_WND,NULL,0 ,0,0,1279,319)
#endif