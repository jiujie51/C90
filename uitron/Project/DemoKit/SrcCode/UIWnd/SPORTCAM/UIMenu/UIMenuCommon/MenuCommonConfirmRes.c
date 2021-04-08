//This source code is generated by UI Designer Studio.

#ifndef MENUCOMMONCONFIRMRES_H
#define MENUCOMMONCONFIRMRES_H

#include "UIFramework.h"
#include "UIFrameworkExt.h"
#include "UIGraphics.h"
#include "NVTToolCommand.h"
#include "UIDisplayUtil.h"
//------------------------------------------------------------

SHOW_ITEM_RECT(CMD_Rectangle, Skin1Rect, 0, 0, 319, 239, 0, 0, 255, 255, 0, 0, 0)
SHOW_MAP_BEGIN(Skin1)
SHOW_MAP_BODY(Skin1Rect)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group, Skin1, 0, 0, 319, 239)

SHOW_ITEM_IMAGE(CMD_Image, Image4, 15, 0, 124, 89, ICON_BTN_EMBOSS, 83886080, 0, 0, 0, 0, 0, 17, 0)
SHOW_ITEM_RECT(CMD_Rectangle, Skin3Rect, 0, 0, 139, 89, 0, 0, 255, 255, 0, 0, 0)
SHOW_MAP_BEGIN(Skin3)
SHOW_MAP_BODY(Skin3Rect)
SHOW_MAP_BODY(Image4)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group, Skin3, 0, 0, 139, 89)

SHOW_ITEM_IMAGE(CMD_Image | CMD_ITEM, Icon5, 27, 13, 112, 78, ICONID_NULL, 83886080, 0, 0, 0, 0, 0, 17, 0)
SHOW_MAP_BEGIN(Normal2)
SHOW_MAP_BODY(Skin3)
SHOW_MAP_BODY(Icon5)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group, Normal2, 20, 70, 159, 159)

SHOW_ITEM_IMAGE(CMD_Image, Image8, 15, 0, 124, 89, ICON_ACT_RELEASE_BTN, 83886080, 0, 0, 0, 0, 0, 17, 0)
SHOW_ITEM_RECT(CMD_Rectangle, Skin7Rect, 0, 0, 139, 89, 0, 0, 255, 255, 0, 0, 0)
SHOW_MAP_BEGIN(Skin7)
SHOW_MAP_BODY(Skin7Rect)
SHOW_MAP_BODY(Image8)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group, Skin7, 0, 0, 139, 89)

SHOW_ITEM_IMAGE(CMD_Image | CMD_ITEM, Icon9, 27, 13, 112, 78, ICONID_NULL, 83886080, 0, 0, 0, 0, 0, 17, 0)
SHOW_MAP_BEGIN(Focused6)
SHOW_MAP_BODY(Skin7)
SHOW_MAP_BODY(Icon9)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group, Focused6, 20, 70, 159, 159)

SHOW_ITEM_IMAGE(CMD_Image, Image12, 15, 0, 124, 89, ICON_BTN_EMBOSS, 83886080, 0, 0, 0, 0, 0, 17, 0)
SHOW_ITEM_RECT(CMD_Rectangle, Skin11Rect, 0, 0, 139, 89, 0, 0, 255, 255, 0, 0, 0)
SHOW_MAP_BEGIN(Skin11)
SHOW_MAP_BODY(Skin11Rect)
SHOW_MAP_BODY(Image12)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group, Skin11, 0, 0, 139, 89)

SHOW_ITEM_IMAGE(CMD_Image | CMD_ITEM, Icon13, 27, 13, 112, 78, ICONID_NULL, 83886080, 0, 0, 0, 0, 0, 17, 0)
SHOW_MAP_BEGIN(Normal_Disable10)
SHOW_MAP_BODY(Skin11)
SHOW_MAP_BODY(Icon13)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group, Normal_Disable10, 20, 70, 159, 159)

SHOW_ITEM_IMAGE(CMD_Image, Image16, 15, 0, 124, 89, ICON_BTN_EMBOSS, 83886080, 0, 0, 0, 0, 0, 17, 0)
SHOW_ITEM_RECT(CMD_Rectangle, Skin15Rect, 0, 0, 139, 89, 0, 0, 255, 255, 0, 0, 0)
SHOW_MAP_BEGIN(Skin15)
SHOW_MAP_BODY(Skin15Rect)
SHOW_MAP_BODY(Image16)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group, Skin15, 0, 0, 139, 89)

SHOW_ITEM_IMAGE(CMD_Image | CMD_ITEM, Icon17, 27, 13, 112, 78, ICONID_NULL, 83886080, 0, 0, 0, 0, 0, 17, 0)
SHOW_MAP_BEGIN(Focused_Disable14)
SHOW_MAP_BODY(Skin15)
SHOW_MAP_BODY(Icon17)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group, Focused_Disable14, 20, 70, 159, 159)

SHOW_ITEM_IMAGE(CMD_Image, Image20, 21, 6, 130, 95, ICON_ACT_PUSH_BTN, 83886080, 0, 0, 0, 0, 0, 17, 0)
SHOW_ITEM_RECT(CMD_Rectangle, Skin19Rect, 0, 0, 139, 89, 0, 0, 255, 255, 0, 0, 0)
SHOW_MAP_BEGIN(Skin19)
SHOW_MAP_BODY(Skin19Rect)
SHOW_MAP_BODY(Image20)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group, Skin19, 0, 0, 139, 89)

SHOW_ITEM_IMAGE(CMD_Image | CMD_ITEM, Icon21, 33, 19, 118, 84, ICONID_NULL, 83886080, 0, 0, 0, 0, 0, 17, 0)
SHOW_MAP_BEGIN(Pressed18)
SHOW_MAP_BODY(Skin19)
SHOW_MAP_BODY(Icon21)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group, Pressed18, 20, 70, 159, 159)

CREATE_LIST_ITEM_DATA(ICON_OK_M22, STRID_NULL, ICON_OK_M, STATUS_ENABLE)
CREATE_LIST_ITEM_DATA(ICON_NO_M23, STRID_NULL, ICON_NO_M, STATUS_ENABLE)
SHOW_MAP_BEGIN(MenuCommonConfirm_List)
SHOW_MAP_BODY(Normal2)
SHOW_MAP_BODY(Focused6)
SHOW_MAP_BODY(Normal_Disable10)
SHOW_MAP_BODY(Focused_Disable14)
SHOW_MAP_BODY(Pressed18)
SHOW_MAP_END

DECLARE_EVENT(MenuCommonConfirm_List)
LIST_LIST_BEGIN(MenuCommonConfirm_List)
LIST_LIST_ITEM(ICON_OK_M22)
LIST_LIST_ITEM(ICON_NO_M23)
LIST_LIST_END

CREATE_LIST_DATA(MenuCommonConfirm_List, 0, 2, MENU_LAYOUT_HORIZONTAL | MENU_DISABLE_HIDE | MENU_SCROLL_CYCLE | MENU_SCROLL_NEXT_ITEM | MENU_DISABLE_NORMAL | LIST_DRAW_IMAGE, NVTEVT_NULL)

CREATE_LIST_CTRL(MenuCommonConfirm_List, MenuCommonConfirm_List, 0, 20, 70, 159, 159)
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

SHOW_ITEM_RECT(CMD_Rectangle, Skin25Rect, 0, 0, 315, 49, 0, 0, 255, 255, 0, 0, 0)
SHOW_MAP_BEGIN(Skin25)
SHOW_MAP_BODY(Skin25Rect)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group, Skin25, 0, 0, 315, 49)

SHOW_ITEM_TEXT(CMD_Text | CMD_ITEM, Text27, 0, 0, 315, 49, STRID_NULL, 0, 0, 0, 0, 1, 1, 1, 256, 0, 16, 0, 0, 0, 0)
SHOW_MAP_BEGIN(Normal24)
SHOW_MAP_BODY(Skin25)
SHOW_MAP_BODY(Text27)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group, Normal24, 5, 5, 320, 54)

SHOW_MAP_BEGIN(MenuCommonConfirm_Title)
SHOW_MAP_BODY(Normal24)
SHOW_MAP_END

DECLARE_EVENT(MenuCommonConfirm_Title)
CREATE_STATIC_DATA(MenuCommonConfirm_Title, STRID_DELETE_WARNING)
CREATE_STATIC_CTRL(MenuCommonConfirm_Title, 0, 5, 5, 320, 54)

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

SHOW_ITEM_RECT(CMD_Rectangle, Skin29Rect, 0, 0, 320, 29, 0, 0, 255, 255, 0, 0, 0)
SHOW_MAP_BEGIN(Skin29)
SHOW_MAP_BODY(Skin29Rect)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group, Skin29, 0, 0, 320, 29)

SHOW_ITEM_RECT(CMD_Rectangle, Skin31Rect, 0, 0, 125, 29, 0, 0, 255, 255, 0, 0, 0)
SHOW_MAP_BEGIN(Skin31)
SHOW_MAP_BODY(Skin31Rect)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group, Skin31, 0, 0, 125, 29)

SHOW_ITEM_TEXT(CMD_Text | CMD_ITEM, Text33, 0, 0, 124, 29, STRID_NULL, 0, 0, 0, 0, 1, 1, 1, 0, 0, 16, 0, 0, 0, 0)
SHOW_MAP_BEGIN(Normal30)
SHOW_MAP_BODY(Skin31)
SHOW_MAP_BODY(Text33)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group, Normal30, 36, 210, 161, 239)

SHOW_MAP_BEGIN(MenuCommonConfirm_Left)
SHOW_MAP_BODY(Normal30)
SHOW_MAP_END

DECLARE_EVENT(MenuCommonConfirm_Left)
CREATE_STATIC_DATA(MenuCommonConfirm_Left, STRID_SELECT)
CREATE_STATIC_CTRL(MenuCommonConfirm_Left, 0, 36, -1, 161, 28)

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

SHOW_ITEM_RECT(CMD_Rectangle, Skin36Rect, 0, 0, 123, 29, 0, 0, 255, 255, 0, 0, 0)
SHOW_MAP_BEGIN(Skin36)
SHOW_MAP_BODY(Skin36Rect)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group, Skin36, 0, 0, 123, 29)

SHOW_ITEM_TEXT(CMD_Text | CMD_ITEM, Text38, 0, 0, 123, 29, STRID_NULL, 0, 0, 0, 0, 1, 1, 1, 0, 0, 18, 0, 0, 0, 0)
SHOW_MAP_BEGIN(Normal35)
SHOW_MAP_BODY(Skin36)
SHOW_MAP_BODY(Text38)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group, Normal35, 160, 211, 283, 240)

SHOW_MAP_BEGIN(MenuCommonConfirm_Right)
SHOW_MAP_BODY(Normal35)
SHOW_MAP_END

DECLARE_EVENT(MenuCommonConfirm_Right)
CREATE_STATIC_DATA(MenuCommonConfirm_Right, STRID_NEXT)
CREATE_STATIC_CTRL(MenuCommonConfirm_Right, 0, 160, 0, 283, 29)

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

SHOW_MAP_BEGIN(Skin41)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group, Skin41, 0, 0, 27, 27)

SHOW_ITEM_IMAGE(CMD_Image | CMD_ITEM, Icon42, 0, 0, 27, 27, ICONID_NULL, 83886080, 0, 0, 0, 0, 0, 0, 0)
SHOW_MAP_BEGIN(Normal40)
SHOW_MAP_BODY(Skin41)
SHOW_MAP_BODY(Icon42)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group, Normal40, 5, 211, 32, 238)

SHOW_MAP_BEGIN(MenuCommonConfirm_LeftIcon)
SHOW_MAP_BODY(Normal40)
SHOW_MAP_END

DECLARE_EVENT(MenuCommonConfirm_LeftIcon)
CREATE_STATIC_DATA(MenuCommonConfirm_LeftIcon, ICON_DIR_LEFT)
CREATE_STATIC_CTRL(MenuCommonConfirm_LeftIcon, 0, 5, 0, 32, 27)

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

SHOW_MAP_BEGIN(Skin45)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group, Skin45, 0, 0, 27, 27)

SHOW_ITEM_IMAGE(CMD_Image | CMD_ITEM, Icon46, 0, 0, 27, 27, ICONID_NULL, 83886080, 0, 0, 0, 0, 0, 0, 0)
SHOW_MAP_BEGIN(Normal44)
SHOW_MAP_BODY(Skin45)
SHOW_MAP_BODY(Icon46)
SHOW_MAP_END

SHOW_ITEM_GROUP(CMD_Group, Normal44, 288, 211, 315, 238)

SHOW_MAP_BEGIN(MenuCommonConfirm_RightIcon)
SHOW_MAP_BODY(Normal44)
SHOW_MAP_END

DECLARE_EVENT(MenuCommonConfirm_RightIcon)
CREATE_STATIC_DATA(MenuCommonConfirm_RightIcon, ICON_DIR_RIGHT)
CREATE_STATIC_CTRL(MenuCommonConfirm_RightIcon, 0, 288, 0, 315, 27)

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

SHOW_MAP_BEGIN(MenuCommConfirm_Panel)
SHOW_MAP_BODY(Skin29)
SHOW_MAP_END

DECLARE_EVENT(MenuCommConfirm_Panel)
DECLARE_CTRL_LIST(MenuCommConfirm_Panel)
CREATE_CTRL(MenuCommConfirm_Panel, MenuCommConfirm_Panel, CTRL_WND, NULL, 0, 0, 211, 320, 240)
SHOW_MAP_BEGIN(MenuCommonConfirm)
SHOW_MAP_BODY(Skin1)
SHOW_MAP_END

DECLARE_EVENT(MenuCommonConfirm)
DECLARE_CTRL_LIST(MenuCommonConfirm)
CREATE_CTRL(MenuCommonConfirm, MenuCommonConfirm, CTRL_WND, NULL, 0, 0, 0, 319, 239)
#endif