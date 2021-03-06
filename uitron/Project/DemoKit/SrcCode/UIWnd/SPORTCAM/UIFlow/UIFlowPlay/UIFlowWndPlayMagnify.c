//This source code is generated by UI Designer Studio.

#include <stdio.h>
#include "UIFramework.h"
#include "UIFrameworkExt.h"
#include "UIGraphics.h"
#include "NVTToolCommand.h"
#include "UIFlowWndPlayMagnifyRes.c"
#include "UIFlowWndPlayMagnify.h"
//#NT#2009/08/04#Chris Chung -begin
#include "PlaybackTsk.h"
#include "Utility.h"
#include "KeyDef.h"
#include "UIFlow.h"
#include "JpgHeader.h"
#include "Exif.h"

#define DEBUG_FLOWZOOM 1
#if (DEBUG_FLOWZOOM)
#define Debug_FlowZoom(msg)              debug_msg msg
#else
#define Debug_FlowZoom(msg)
#endif

#define ZOOM_LOCK_FLOW          ENABLE

#define ZOOM_EXTERNAL_W         64
#define ZOOM_EXTERNAL_H         48

#define ZOOM_MIN_LEVEL          100   //1X
//#define ZOOM_MAX_LEVEL          400 //4X
#define ZOOM_MAX_LEVEL          800 //8X
#define ZOOM_MAX_STEP           16

#define ZOOM_SCALE_UNIT         50

// Magnify mode key mask
#define PLBZOOM_KEY_CONTINUE_MASK   (FLGKEY_KEY_CONT_MASK_DEFAULT|FLGKEY_ZOOMIN|FLGKEY_ZOOMOUT)
#define PLBZOOM_KEY_RELEASE_MASK    0

UINT32   g_UIPlayZoomLvlTbl[ZOOM_MAX_STEP] = {0};
//------------------------------------------------------------
//static POINT LastPoint={0};
static UINT32 g_uiZoomIndex = 0;
static UINT32 g_uiOriImageWidth = 0;
static UINT32 g_uiOriImageHeight = 0;
//static UINT32 g_uiAutoHideID = TIMER_NULL;
static UINT32 g_uiKeyReleaseBackup;
static UINT32 g_uiKeyContinueBackup;
static BOOL   g_bShowOSD = TRUE;
static BOOL   g_bZoomLock = FALSE;

static void UIFlowWndPlayMagnify_DrawNavigatorWindow(void);
static INT32 UIFlowWndPlayMagnify_ZoomNavi(UINT32 key);
static void UIFlowWndPlayMagnify_DrawZoomRatio(void);
//#NT#2009/08/04#Chris Chung -end
void Input_SetKeyMask(KEY_STATUS uiMode, UINT32 uiMask);
UINT32 Input_GetKeyMask(KEY_STATUS uiMode);
//---------------------UIFlowWndPlayMagnifyCtrl Control List---------------------------
CTRL_LIST_BEGIN(UIFlowWndPlayMagnify)
CTRL_LIST_ITEM(UIFlowWndPlayMagnify_StaticTXT_DZoom)
CTRL_LIST_ITEM(UIFlowWndPlayMagnify_StaticIcon_DirUp)
CTRL_LIST_ITEM(UIFlowWndPlayMagnify_StaticIcon_DirDown)
CTRL_LIST_ITEM(UIFlowWndPlayMagnify_StaticIcon_DirRight)
CTRL_LIST_ITEM(UIFlowWndPlayMagnify_StaticIcon_DirLeft)
//#NT#2009/07/24#Chris Chung -begin
//#NT#add specific rectangle for zoom in/out/move
CTRL_LIST_ITEM(ZoomWin)
//#NT#2009/07/24#Chris Chung -end
CTRL_LIST_END

//----------------------UIFlowWndPlayMagnifyCtrl Event---------------------------
INT32 UIFlowWndPlayMagnify_OnOpen(VControl *, UINT32, UINT32 *);
INT32 UIFlowWndPlayMagnify_OnClose(VControl *, UINT32, UINT32 *);
INT32 UIFlowWndPlayMagnify_OnKeyZoomin(VControl *, UINT32, UINT32 *);
INT32 UIFlowWndPlayMagnify_OnKeyZoomout(VControl *, UINT32, UINT32 *);
INT32 UIFlowWndPlayMagnify_OnKeyUp(VControl *, UINT32, UINT32 *);
INT32 UIFlowWndPlayMagnify_OnKeyDown(VControl *, UINT32, UINT32 *);
INT32 UIFlowWndPlayMagnify_OnKeyLeft(VControl *, UINT32, UINT32 *);
INT32 UIFlowWndPlayMagnify_OnKeyRight(VControl *, UINT32, UINT32 *);
INT32 UIFlowWndPlayMagnify_OnKeyEnter(VControl *, UINT32, UINT32 *);
INT32 UIFlowWndPlayMagnify_OnKeyMenu(VControl *, UINT32, UINT32 *);
INT32 UIFlowWndPlayMagnify_OnKeyMode(VControl *, UINT32, UINT32 *);
INT32 UIFlowWndPlayMagnify_OnKeyShutter2(VControl *, UINT32, UINT32 *);
EVENT_BEGIN(UIFlowWndPlayMagnify)
EVENT_ITEM(NVTEVT_OPEN_WINDOW, UIFlowWndPlayMagnify_OnOpen)
EVENT_ITEM(NVTEVT_CLOSE_WINDOW, UIFlowWndPlayMagnify_OnClose)
EVENT_ITEM(NVTEVT_KEY_ZOOMIN, UIFlowWndPlayMagnify_OnKeyZoomin)
EVENT_ITEM(NVTEVT_KEY_ZOOMOUT, UIFlowWndPlayMagnify_OnKeyZoomout)
EVENT_ITEM(NVTEVT_KEY_UP, UIFlowWndPlayMagnify_OnKeyUp)
EVENT_ITEM(NVTEVT_KEY_DOWN, UIFlowWndPlayMagnify_OnKeyDown)
EVENT_ITEM(NVTEVT_KEY_LEFT, UIFlowWndPlayMagnify_OnKeyLeft)
EVENT_ITEM(NVTEVT_KEY_RIGHT, UIFlowWndPlayMagnify_OnKeyRight)
EVENT_ITEM(NVTEVT_KEY_ENTER, UIFlowWndPlayMagnify_OnKeyEnter)
EVENT_ITEM(NVTEVT_KEY_MENU, UIFlowWndPlayMagnify_OnKeyMenu)
EVENT_ITEM(NVTEVT_KEY_MODE, UIFlowWndPlayMagnify_OnKeyMode)
EVENT_ITEM(NVTEVT_KEY_SHUTTER2, UIFlowWndPlayMagnify_OnKeyShutter2)
EVENT_END

INT32 UIFlowWndPlayMagnify_OnOpen(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	//#NT#2009/07/24#Chris Chung -begin
	UINT32 i, uiZoomMaxLevel = ZOOM_MAX_LEVEL;
	JPGHEAD_DEC_CFG JPGInfo = {0};
	EXIF_GETTAG exifTag;
	BOOL bIsLittleEndian = TRUE;
	UINT32 OriImgWidth;
	PB_GetParam(PBPRMID_INFO_IMG, (UINT32 *)&JPGInfo);
	EXIF_GetInfo(EXIF_HDL_ID_1, EXIFINFO_BYTEORDER, &bIsLittleEndian, sizeof(bIsLittleEndian));
	exifTag.uiTagIfd = EXIF_IFD_EXIF;
	exifTag.uiTagId = TAG_ID_PIXEL_X_DIMENSION;
	if (EXIF_ER_OK == EXIF_GetTag(EXIF_HDL_ID_1, &exifTag)) {
		OriImgWidth = Get32BitsData(exifTag.uiTagDataAddr, bIsLittleEndian);
	} else {
		OriImgWidth = (UINT32)JPGInfo.ori_imagewidth;
	}
	if (OriImgWidth < 1280/*JPGIMG_SIZE[_IMGSize_1M][0]*/) {
		uiZoomMaxLevel = 400; // max x4.0 for image size < 1M pixel
	}

	for (i = 0; i < ZOOM_MAX_STEP; i++) {
		g_UIPlayZoomLvlTbl[i] = ZOOM_MIN_LEVEL + i * (uiZoomMaxLevel - ZOOM_MIN_LEVEL) / (ZOOM_MAX_STEP - 1);
		Debug_FlowZoom(("g_UIPlayZoomLvlTbl[%02d] = %d\r\n", i, g_UIPlayZoomLvlTbl[i]));
	}
	PB_SetParam(PBPRMID_ZOOM_LVL_TBL, (UINT32)g_UIPlayZoomLvlTbl);
	Input_SetKeyMask(KEY_PRESS, FLGKEY_KEY_MASK_NULL);
	Input_SetKeyMask(KEY_RELEASE, FLGKEY_KEY_MASK_NULL);
	PB_PlayZoomMode(PLAYZOOM_IN);
	PB_WaitCommandFinish(PB_WAIT_INFINITE);
	Input_SetKeyMask(KEY_PRESS, FLGKEY_KEY_MASK_DEFAULT);
	Input_SetKeyMask(KEY_RELEASE, FLGKEY_KEY_MASK_DEFAULT);
	PB_GetParam(PBPRMID_ZOOM_INDEX, &g_uiZoomIndex);

	g_uiOriImageWidth  = (UINT32)JPGInfo.imagewidth;
	g_uiOriImageHeight = (UINT32)JPGInfo.imageheight;

#if (ZOOM_LOCK_FLOW == DISABLE)
	g_bZoomLock = TRUE;
	UxCtrl_SetShow(&UIFlowWndPlayMagnify_StaticIcon_DirUpCtrl, TRUE);
	UxCtrl_SetShow(&UIFlowWndPlayMagnify_StaticIcon_DirDownCtrl, TRUE);
	UxCtrl_SetShow(&UIFlowWndPlayMagnify_StaticIcon_DirLeftCtrl, TRUE);
	UxCtrl_SetShow(&UIFlowWndPlayMagnify_StaticIcon_DirRightCtrl, TRUE);
#else
	g_bZoomLock = FALSE;
	UxCtrl_SetShow(&UIFlowWndPlayMagnify_StaticIcon_DirUpCtrl, FALSE);
	UxCtrl_SetShow(&UIFlowWndPlayMagnify_StaticIcon_DirDownCtrl, FALSE);
	UxCtrl_SetShow(&UIFlowWndPlayMagnify_StaticIcon_DirLeftCtrl, FALSE);
	UxCtrl_SetShow(&UIFlowWndPlayMagnify_StaticIcon_DirRightCtrl, FALSE);
#endif

	UIFlowWndPlayMagnify_DrawNavigatorWindow();

	g_uiKeyReleaseBackup = Input_GetKeyMask(KEY_RELEASE);
	g_uiKeyContinueBackup = Input_GetKeyMask(KEY_CONTINUE);
	Input_SetKeyMask(KEY_CONTINUE, PLBZOOM_KEY_CONTINUE_MASK);
	Input_SetKeyMask(KEY_RELEASE, PLBZOOM_KEY_RELEASE_MASK);
	//#NT#2009/07/24#Chris Chung -end
	Ux_DefaultEvent(pCtrl, NVTEVT_OPEN_WINDOW, paramNum, paramArray);
	return NVTEVT_CONSUME;
}
INT32 UIFlowWndPlayMagnify_OnClose(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	Input_SetKeyMask(KEY_CONTINUE, g_uiKeyContinueBackup);
	Input_SetKeyMask(KEY_RELEASE, g_uiKeyReleaseBackup);
	Ux_DefaultEvent(pCtrl, NVTEVT_CLOSE_WINDOW, paramNum, paramArray);
	return NVTEVT_CONSUME;
}
INT32 UIFlowWndPlayMagnify_OnKeyZoomin(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	//#NT#2009/07/24#Chris Chung -begin
	if (g_uiZoomIndex < (ZOOM_MAX_STEP - 1)) {
		PB_PlayZoomMode(PLAYZOOM_IN);
		PB_WaitCommandFinish(PB_WAIT_INFINITE);
		PB_GetParam(PBPRMID_ZOOM_INDEX, &g_uiZoomIndex);
		UIFlowWndPlayMagnify_DrawNavigatorWindow();
	}
	Debug_FlowZoom(("PB_GetCurrZoomIndex=%d\r\n ", g_uiZoomIndex));
	//#NT#2009/07/24#Chris Chung -end
	return NVTEVT_CONSUME;
}
INT32 UIFlowWndPlayMagnify_OnKeyZoomout(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	//#NT#2009/10/05#Chris Chung -begin
	if (g_uiZoomIndex > 1) {
		PB_PlayZoomMode(PLAYZOOM_OUT);
		PB_WaitCommandFinish(PB_WAIT_INFINITE);
		PB_GetParam(PBPRMID_ZOOM_INDEX, &g_uiZoomIndex);
		UIFlowWndPlayMagnify_DrawNavigatorWindow();
	} else {
		Ux_CloseWindow((VControl *)(&UIFlowWndPlayMagnifyCtrl), 0);
	}
	Debug_FlowZoom(("PB_GetCurrZoomIndex=%d\r\n ", g_uiZoomIndex));
	//#NT#2009/10/05#Chris Chung -end
	return NVTEVT_CONSUME;
}
INT32 UIFlowWndPlayMagnify_OnKeyUp(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	if (g_bZoomLock) {
		UIFlowWndPlayMagnify_ZoomNavi(NVTEVT_KEY_UP);
	} else {
		if (g_uiZoomIndex < (ZOOM_MAX_STEP - 1)) {
			PB_PlayZoomMode(PLAYZOOM_IN);
			PB_WaitCommandFinish(PB_WAIT_INFINITE);
			PB_GetParam(PBPRMID_ZOOM_INDEX, &g_uiZoomIndex);
			UIFlowWndPlayMagnify_DrawNavigatorWindow();
		}
		Debug_FlowZoom(("PB_GetCurrZoomIndex=%d\r\n ", g_uiZoomIndex));
	}
	return NVTEVT_CONSUME;
}
INT32 UIFlowWndPlayMagnify_OnKeyDown(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	if (g_bZoomLock) {
		UIFlowWndPlayMagnify_ZoomNavi(NVTEVT_KEY_DOWN);
	} else {
		if (g_uiZoomIndex > 1) {
			PB_PlayZoomMode(PLAYZOOM_OUT);
			PB_WaitCommandFinish(PB_WAIT_INFINITE);

			PB_GetParam(PBPRMID_ZOOM_INDEX, &g_uiZoomIndex);
			UIFlowWndPlayMagnify_DrawNavigatorWindow();
		} else {
			Ux_CloseWindow((VControl *)(&UIFlowWndPlayMagnifyCtrl), 0);
		}
		Debug_FlowZoom(("PB_GetCurrZoomIndex=%d\r\n ", g_uiZoomIndex));
	}
	return NVTEVT_CONSUME;
}
INT32 UIFlowWndPlayMagnify_OnKeyLeft(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	if (g_bZoomLock) {
		UIFlowWndPlayMagnify_ZoomNavi(NVTEVT_KEY_LEFT);
	}
	return NVTEVT_CONSUME;
}
INT32 UIFlowWndPlayMagnify_OnKeyRight(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	if (g_bZoomLock) {
		UIFlowWndPlayMagnify_ZoomNavi(NVTEVT_KEY_RIGHT);
	}
	return NVTEVT_CONSUME;
}
INT32 UIFlowWndPlayMagnify_OnKeyEnter(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
#if (ZOOM_LOCK_FLOW == DISABLE)
	Ux_CloseWindow((VControl *)(&UIFlowWndPlayMagnifyCtrl), 1, NVTRET_MAGNIFY);
#else
	if (g_bZoomLock) {
		UxCtrl_SetShow(&UIFlowWndPlayMagnify_StaticIcon_DirUpCtrl, FALSE);
		UxCtrl_SetShow(&UIFlowWndPlayMagnify_StaticIcon_DirDownCtrl, FALSE);
		UxCtrl_SetShow(&UIFlowWndPlayMagnify_StaticIcon_DirLeftCtrl, FALSE);
		UxCtrl_SetShow(&UIFlowWndPlayMagnify_StaticIcon_DirRightCtrl, FALSE);
		g_bZoomLock = FALSE;
	} else {
		UxCtrl_SetShow(&UIFlowWndPlayMagnify_StaticIcon_DirUpCtrl, TRUE);
		UxCtrl_SetShow(&UIFlowWndPlayMagnify_StaticIcon_DirDownCtrl, TRUE);
		UxCtrl_SetShow(&UIFlowWndPlayMagnify_StaticIcon_DirLeftCtrl, TRUE);
		UxCtrl_SetShow(&UIFlowWndPlayMagnify_StaticIcon_DirRightCtrl, TRUE);
		g_bZoomLock = TRUE;
	}
#endif
	return NVTEVT_CONSUME;
}
INT32 UIFlowWndPlayMagnify_OnKeyMenu(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	Ux_CloseWindow((VControl *)(&UIFlowWndPlayMagnifyCtrl), 1, NVTRET_MAGNIFY);
	return NVTEVT_CONSUME;
}
INT32 UIFlowWndPlayMagnify_OnKeyMode(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	Ux_SendEvent(&UISetupObjCtrl, NVTEVT_EXE_CHANGEDSCMODE, 1, DSCMODE_CHGTO_NEXT);
	return NVTEVT_CONSUME;
}
INT32 UIFlowWndPlayMagnify_OnKeyShutter2(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	Ux_SendEvent(&UISetupObjCtrl, NVTEVT_FORCETO_LIVEVIEW_MODE, 0);
	return NVTEVT_CONSUME;
}

//#NT#2009/10/07#Chris Chung -begin
static void UIFlowWndPlayMagnify_DrawNavigatorWindow(void)
{
	UINT32  InternalW, InternalH, ExternalW, ExternalH;
	UINT32  uiPanSrcStartX, uiPanSrcStartY, uiPanMaxX, uiPanMaxY;
	Ux_RECT InternalWin = {0, 0, 0, 0};
	Ux_RECT ExternalWin = {0, 0, 0, 0};
	//internal and external win are all relative position to ZoomWin
	PB_GetParam(PBPRMID_PAN_CURX, &uiPanSrcStartX);
	PB_GetParam(PBPRMID_PAN_CURY, &uiPanSrcStartY);
	PB_GetParam(PBPRMID_PAN_MAXX, &uiPanMaxX);
	PB_GetParam(PBPRMID_PAN_MAXY, &uiPanMaxY);
	if (g_uiOriImageWidth > g_uiOriImageHeight) {
		ExternalW = ZOOM_EXTERNAL_W;
		ExternalH = ZOOM_EXTERNAL_H;
	} else {
		ExternalW = ZOOM_EXTERNAL_H;
		ExternalH = ZOOM_EXTERNAL_W;
		ExternalWin.y1 = (ZOOM_EXTERNAL_W - ZOOM_EXTERNAL_H);
	}
	ExternalWin.x2 = ExternalWin.x1 + ExternalW - 1;
	ExternalWin.y2 = ExternalWin.y1 + ExternalH - 1;


	InternalW = ZOOM_EXTERNAL_W * g_UIPlayZoomLvlTbl[0] / g_UIPlayZoomLvlTbl[g_uiZoomIndex];
	InternalH = ZOOM_EXTERNAL_H * g_UIPlayZoomLvlTbl[0] / g_UIPlayZoomLvlTbl[g_uiZoomIndex];
	// draw Internal navigator window
	// only support width > height
	InternalWin.x1 = ExternalWin.x1 + ExternalW * uiPanSrcStartX / g_uiOriImageWidth;
	InternalWin.y1 = ExternalWin.y1 + ExternalH * uiPanSrcStartY / g_uiOriImageHeight;
	if (uiPanMaxX == 0) {
		InternalWin.x1 = 0;
		InternalW = ExternalW;
	} else if (uiPanSrcStartX == uiPanMaxX) {
		InternalWin.x1 = ExternalW - InternalW ;
	}

	if (uiPanMaxY == 0) {
		InternalWin.y1 = 0;
		InternalH = ExternalH;
	} else if (uiPanSrcStartY == uiPanMaxY) {
		InternalWin.y1 = ExternalWin.y1 + ExternalH - InternalH;
	}
	InternalWin.x2 = InternalWin.x1 + InternalW - 2;
	InternalWin.y2 = InternalWin.y1 + InternalH - 2;

	// To avoid overlap with External zoom window
	if (InternalWin.x1 < 1) {
		InternalWin.x1 = 1;
	}
	if (InternalWin.y1 < 1) {
		InternalWin.y1 = 1;
	}
	if (InternalWin.x2 > ZOOM_EXTERNAL_W - 3) {
		InternalWin.x2 = ZOOM_EXTERNAL_W - 3;
	}
	if (InternalWin.y2 > ZOOM_EXTERNAL_H - 3) {
		InternalWin.y2 = ZOOM_EXTERNAL_H - 3;
	}

	Debug_FlowZoom(("PanSrcStart(x,y)=(%d,%d), PanMax(x,y)=(%d,%d)\r\n", uiPanSrcStartX, uiPanSrcStartY, uiPanMaxX, uiPanMaxY));
	Debug_FlowZoom(("Internal(W,H)=(%d,%d) External(W,H)=(%d,%d)\r\n", InternalW, InternalH, ExternalW, ExternalH));
	Debug_FlowZoom(("InternalWin(x,y,x2,y2)=(%d,%d,%d,%d)\r\n", InternalWin.x1, InternalWin.y1, InternalWin.x2, InternalWin.y2));
	Debug_FlowZoom(("ExternalWin(x,y,x2,y2)=(%d,%d,%d,%d)\r\n", ExternalWin.x1, ExternalWin.y1, ExternalWin.x2, ExternalWin.y2));

	UxZoom_SetPosition(&ZoomWinCtrl, ZOOM_INTERNAL_RECT, &InternalWin);
	UxZoom_SetPosition(&ZoomWinCtrl, ZOOM_EXTERNAL_RECT, &ExternalWin);
	UxCtrl_SetShow(&ZoomWinCtrl, g_bShowOSD);
	UxCtrl_SetDirty(&ZoomWinCtrl, TRUE);

	UIFlowWndPlayMagnify_DrawZoomRatio();
}

static INT32 UIFlowWndPlayMagnify_ZoomNavi(UINT32 key)
{
	UINT32    MoveOffset_X, MoveOffset_Y, SetPlayCommand;
	UINT32    OriStart_X, OriStart_Y, tmpZoomLevel, ZoomIndex;
	INT32     PanLoop;
	UINT16    usImagewidth, usImageheight;
	JPGHEAD_DEC_CFG JPGInfo = {0};
	PB_GetParam(PBPRMID_INFO_IMG, (UINT32 *)&JPGInfo);
	PB_GetParam(PBPRMID_ZOOM_INDEX, &ZoomIndex);
	usImagewidth  = JPGInfo.imagewidth;;
	usImageheight = JPGInfo.imageheight;

	Debug_FlowZoom(("UIFlowWndPlayMagnify_ZoomNavi Key=%d\r\n ", key));

	tmpZoomLevel = g_UIPlayZoomLvlTbl[ZoomIndex];

	OriStart_X = (usImagewidth -
				  (usImagewidth * ZOOM_SCALE_UNIT / tmpZoomLevel) / ZOOM_SCALE_UNIT) >> 1;
	OriStart_Y = (usImageheight -
				  (usImageheight * ZOOM_SCALE_UNIT / tmpZoomLevel) / ZOOM_SCALE_UNIT) >> 1;

	PanLoop = tmpZoomLevel / ZOOM_SCALE_UNIT - 1;
	Debug_FlowZoom(("OriStart_X,Y(%d,%d) PanLoop=%d\r\n ", OriStart_X, OriStart_Y, PanLoop));
	if (PanLoop < 2) {
		PanLoop = 2;
	}
	MoveOffset_X = OriStart_X / PanLoop;
	MoveOffset_Y = OriStart_Y / PanLoop;
	Debug_FlowZoom(("MoveOffset_X,Y(%d,%d) PanLoop=%d\r\n ", MoveOffset_X, MoveOffset_Y, PanLoop));

	if ((key == NVTEVT_KEY_UP) || (key == NVTEVT_KEY_DOWN)) {
		if (key == NVTEVT_KEY_UP) {
			SetPlayCommand = PLAYZOOM_UP;
		} else {
			SetPlayCommand = PLAYZOOM_DOWN;
		}
	} else { //if(Command & (NVTEVT_KEY_LEFT|NVTEVT_KEY_RIGHT))
		if (key == NVTEVT_KEY_LEFT) {
			SetPlayCommand = PLAYZOOM_LEFT;
		} else {
			SetPlayCommand = PLAYZOOM_RIGHT;
		}
	}
	PB_PlayZoomPanMode(SetPlayCommand, MoveOffset_X, MoveOffset_Y);
	PB_WaitCommandFinish(PB_WAIT_INFINITE);
	UIFlowWndPlayMagnify_DrawNavigatorWindow();
	return NVTEVT_CONSUME;
}

static void UIFlowWndPlayMagnify_DrawZoomRatio(void)
{
	static char item1_Buf[32] = "x1.0";
	UINT32 temp1, temp2;

	temp1 = g_UIPlayZoomLvlTbl[g_uiZoomIndex] / 100;
	temp2 = (g_UIPlayZoomLvlTbl[g_uiZoomIndex] % 100) / 10;

	snprintf(item1_Buf, 32, "x%ld.%ld", temp1, temp2);

	UxStatic_SetData(&UIFlowWndPlayMagnify_StaticTXT_DZoomCtrl, STATIC_VALUE, Txt_Pointer(item1_Buf));

	UxCtrl_SetShow(&UIFlowWndPlayMagnify_StaticTXT_DZoomCtrl, TRUE);
}
//#NT#2009/10/07#Chris Chung -end

//----------------------UIFlowWndPlayMagnify_StaticTXT_DZoomCtrl Event---------------------------
EVENT_BEGIN(UIFlowWndPlayMagnify_StaticTXT_DZoom)
EVENT_END

//----------------------UIFlowWndPlayMagnify_StaticIcon_DirUpCtrl Event---------------------------
EVENT_BEGIN(UIFlowWndPlayMagnify_StaticIcon_DirUp)
EVENT_END

//----------------------UIFlowWndPlayMagnify_StaticIcon_DirDownCtrl Event---------------------------
EVENT_BEGIN(UIFlowWndPlayMagnify_StaticIcon_DirDown)
EVENT_END

//----------------------UIFlowWndPlayMagnify_StaticIcon_DirRightCtrl Event---------------------------
EVENT_BEGIN(UIFlowWndPlayMagnify_StaticIcon_DirRight)
EVENT_END

//----------------------UIFlowWndPlayMagnify_StaticIcon_DirLeftCtrl Event---------------------------
EVENT_BEGIN(UIFlowWndPlayMagnify_StaticIcon_DirLeft)
EVENT_END

//#NT#2009/07/24#Chris Chung -begin
//#NT#add specific rectangle for zoom in/out/move
//----------------------ZoomWin Event---------------------------
EVENT_BEGIN(ZoomWin)
EVENT_END
//#NT#2009/07/24#Chris Chung -end
