#include <stdio.h>
#include "UIFlow.h"


#define CUSTOM_PHOTO_MENU     0   // set 1 if you want to use custom menu

// --------------------------------------------------------------------------
// OPTIONS
// --------------------------------------------------------------------------
// Photo capture Options
TMDEF_BEGIN_OPTIONS(SELFTIMER)
TMDEF_OPTION_TEXT_S(SELFTIMER_OFF, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(SELFTIMER_2SEC, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(SELFTIMER_5SEC, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(SELFTIMER_10SEC, TM_OPTION_ENABLE)
TMDEF_END_OPTIONS()

// Photo Size Options
TMDEF_BEGIN_OPTIONS(PHOTO_SIZE)
#if (SENSOR_CAPS_COUNT >= 2)
TMDEF_OPTION_TEXT_S(PHOTO_SIZE_12M, TM_OPTION_DISABLE)
#else
TMDEF_OPTION_TEXT_S(PHOTO_SIZE_12M, TM_OPTION_ENABLE)
#endif
TMDEF_OPTION_TEXT_S(PHOTO_SIZE_10M, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(PHOTO_SIZE_8M, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(PHOTO_SIZE_5M, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(PHOTO_SIZE_3M, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(PHOTO_SIZE_VGA, TM_OPTION_ENABLE)
TMDEF_END_OPTIONS()

// Continue shot Options
TMDEF_BEGIN_OPTIONS(CONTINUE_SHOT)
TMDEF_OPTION_TEXT_S(CONTINUE_SHOT_OFF, TM_OPTION_ENABLE)
#if (_BOARD_DRAM_SIZE_ == 0x04000000)
TMDEF_OPTION_TEXT_S(CONTINUE_SHOT_3, TM_OPTION_NOT_SUPPORT)
TMDEF_OPTION_TEXT_S(CONTINUE_SHOT_30, TM_OPTION_NOT_SUPPORT)
TMDEF_OPTION_TEXT_S(CONTINUE_SHOT_ON, TM_OPTION_NOT_SUPPORT)
TMDEF_OPTION_TEXT_S(CONTINUE_SHOT_SIDE, TM_OPTION_NOT_SUPPORT)
#else
TMDEF_OPTION_TEXT_S(CONTINUE_SHOT_3, TM_OPTION_ENABLE)
#if defined(_BSP_NA51000_)
TMDEF_OPTION_TEXT_S(CONTINUE_SHOT_30, TM_OPTION_ENABLE)
#endif
TMDEF_OPTION_TEXT_S(CONTINUE_SHOT_ON, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(CONTINUE_SHOT_SIDE, TM_OPTION_ENABLE)
#endif
TMDEF_END_OPTIONS()


// Photo Quality Options
TMDEF_BEGIN_OPTIONS(QUALITY)
TMDEF_OPTION_TEXT_S(QUALITY_FINE, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(QUALITY_NORMAL, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(QUALITY_ECONOMY, TM_OPTION_ENABLE)
TMDEF_END_OPTIONS()

// Sharpness Options
TMDEF_BEGIN_OPTIONS(SHARPNESS)
TMDEF_OPTION_TEXT_S(SHARP_STRONG, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(SHARP_NORMAL, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(SHARP_SOFT, TM_OPTION_ENABLE)
TMDEF_END_OPTIONS()

// WB Options
TMDEF_BEGIN_OPTIONS(WB)
TMDEF_OPTION_TEXT_S(WB_AUTO, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(WB_DAYLIGHT, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(WB_CLOUDY, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(WB_TUNGSTEN, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(WB_FLUORESCENT, TM_OPTION_ENABLE)
TMDEF_END_OPTIONS()

// Anti-shaking Options
TMDEF_BEGIN_OPTIONS(ANTISHAKE)
TMDEF_OPTION_TEXT_S(ANTISHAKE_OFF, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(ANTISHAKE_ON, TM_OPTION_ENABLE)
TMDEF_END_OPTIONS()

// Quick Review Options
TMDEF_BEGIN_OPTIONS(QUICK_REVIEW)
TMDEF_OPTION_TEXT_S(QV_OFF, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(QV_2SEC, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(QV_5SEC, TM_OPTION_ENABLE)
TMDEF_END_OPTIONS()


// Date Stamp Options
TMDEF_BEGIN_OPTIONS(DATE_STAMP)
TMDEF_OPTION_TEXT_S(DATE_STAMP_OFF, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(DATE_STAMP_DATE, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(DATE_STAMP_DATE_TIME, TM_OPTION_ENABLE)
TMDEF_END_OPTIONS()

// ISO Options
TMDEF_BEGIN_OPTIONS(ISO)
TMDEF_OPTION_TEXT_S(ISO_AUTO, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(ISO_100, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(ISO_200, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(ISO_400, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(ISO_800, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(ISO_1600, TM_OPTION_ENABLE)
TMDEF_END_OPTIONS()

// EV Options
TMDEF_BEGIN_OPTIONS(EV)
TMDEF_OPTION_TEXT_S(EV_P20, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(EV_P16, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(EV_P13, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(EV_P10, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(EV_P06, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(EV_P03, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(EV_00, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(EV_N03, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(EV_N06, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(EV_N10, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(EV_N13, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(EV_N16, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(EV_N20, TM_OPTION_ENABLE)
TMDEF_END_OPTIONS()

// Face Detection Options
TMDEF_BEGIN_OPTIONS(FD)
TMDEF_OPTION_TEXT_S(FD_OFF, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(FD_ON, TM_OPTION_ENABLE)
TMDEF_END_OPTIONS()

TMDEF_BEGIN_OPTIONS(FLASH)
TMDEF_OPTION_TEXT_S(FLASH_AUTO, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(FLASH_ON, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(FLASH_OFF, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(FLASH_RED_EYE, TM_OPTION_ENABLE)
TMDEF_END_OPTIONS()

TMDEF_BEGIN_OPTIONS(RSC)
TMDEF_OPTION_TEXT_S(RSC_OFF, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(RSC_ON, TM_OPTION_ENABLE)
TMDEF_END_OPTIONS()

TMDEF_BEGIN_OPTIONS(HDR)
TMDEF_OPTION_TEXT_S(HDR_OFF, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(HDR_ON, TM_OPTION_ENABLE)
TMDEF_END_OPTIONS()

TMDEF_BEGIN_OPTIONS(WDR)
TMDEF_OPTION_TEXT_S(WDR_OFF, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(WDR_ON, TM_OPTION_ENABLE)
TMDEF_END_OPTIONS()

TMDEF_BEGIN_OPTIONS(METERING)
TMDEF_OPTION_TEXT_S(AVERAGE, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(CENTER, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(SPOT, TM_OPTION_ENABLE)
TMDEF_END_OPTIONS()


TMDEF_BEGIN_OPTIONS(SCENE)
TMDEF_OPTION_TEXT_S(SCENE_AUTO, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(SCENE_PORTRAIT, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(SCENE_LANDSCAPE, TM_OPTION_ENABLE)
TMDEF_END_OPTIONS()

TMDEF_BEGIN_OPTIONS(DUAL_CAM)
TMDEF_OPTION_TEXT_S(DUAL_CAM_FRONT, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(DUAL_CAM_BEHIND, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(DUAL_CAM_BOTH, TM_OPTION_ENABLE)
TMDEF_OPTION_TEXT_S(DUAL_CAM_BOTH2, TM_OPTION_ENABLE)
TMDEF_END_OPTIONS()

// --------------------------------------------------------------------------
// ITEMS
// --------------------------------------------------------------------------
#if CUSTOM_PHOTO_MENU
static int MenuCustom_PhotoEV(UINT32 uiMessage, UINT32 uiParam)
{
	Ux_OpenWindow(&UIMenuWndPhotoExposureCtrl, 0);
	return TMF_PROCESSED;
}

static int MenuCustom_PhotoWB(UINT32 uiMessage, UINT32 uiParam)
{
	Ux_OpenWindow(&UIMenuWndPhotoWBCtrl, 0);
	return TMF_PROCESSED;
}

static int MenuCustom_PhotoColor(UINT32 uiMessage, UINT32 uiParam)
{
	Ux_OpenWindow(&UIMenuWndPhotoColorCtrl, 0);
	return TMF_PROCESSED;
}
#endif

// Photo Menu Items
TMDEF_BEGIN_ITEMS(PHOTO)
TMDEF_ITEM_TEXTID(SELFTIMER)
TMDEF_ITEM_TEXTID(PHOTO_SIZE)
TMDEF_ITEM_TEXTID(CONTINUE_SHOT)
TMDEF_ITEM_TEXTID(QUALITY)
TMDEF_ITEM_TEXTID(SHARPNESS)
TMDEF_ITEM_TEXTID(WB)
TMDEF_ITEM_TEXTID(ANTISHAKE)
TMDEF_ITEM_TEXTID(QUICK_REVIEW)
TMDEF_ITEM_TEXTID(DATE_STAMP)
TMDEF_ITEM_TEXTID(ISO)
TMDEF_ITEM_TEXTID(EV)
TMDEF_ITEM_TEXTID(FD)
TMDEF_ITEM_TEXTID(FLASH)
#if (RSC_FUNC == ENABLE)
TMDEF_ITEM_TEXTID(RSC)
#endif
TMDEF_ITEM_TEXTID(HDR)
TMDEF_ITEM_TEXTID(WDR)
TMDEF_ITEM_TEXTID(METERING)
TMDEF_ITEM_TEXTID(SCENE)
#if (SENSOR_CAPS_COUNT > 1)
TMDEF_ITEM_TEXTID(DUAL_CAM)
#endif
TMDEF_END_ITEMS()

// --------------------------------------------------------------------------
// PAGES
// --------------------------------------------------------------------------
// Photo Menu Pages
TMDEF_BEGIN_PAGES(PHOTO)
TMDEF_PAGE_TEXT_ICON(PHOTO)
TMDEF_END_PAGES()

TMDEF_EMNU(gPhotoMenu, PHOTO, Photo_MenuCallback)

// --------------------------------------------------------------------------
// Menu Callback
// --------------------------------------------------------------------------
int Photo_MenuCallback(UINT32 uiMessage, UINT32 uiParam)
{
	UINT16  uwItemId;
	UINT16  uwOption;

	if (uiMessage == TMM_CONFIRM_OPTION) {
		uwItemId = LO_WORD(uiParam);
		uwOption = HI_WORD(uiParam);

		switch (uwItemId) {
#if (PHOTO_MODE==ENABLE)
		case IDM_SELFTIMER:
			Ux_SendEvent(&CustomPhotoObjCtrl, NVTEVT_EXE_SELFTIMER, 1, uwOption);
			break;

		case IDM_PHOTO_SIZE:
			if (System_GetState(SYS_STATE_CARD)  == CARD_REMOVED) {
				Ux_CloseWindow(&MenuCommonItemCtrl, 0);
				Ux_SendEvent(&CustomPhotoObjCtrl, NVTEVT_EXE_CAPTURE_SIZE, 1, PHOTO_SIZE_3M);
				Ux_OpenWindow(&UIFlowWndWrnMsgCtrl, 2, UIFlowWndWrnMsg_StatusTXT_Msg_STRID_NO_CARD, FLOWWRNMSG_TIMER_2SEC);
				Delay_DelayMs(1000);
				break;
			}
			Ux_SendEvent(&CustomPhotoObjCtrl, NVTEVT_EXE_CAPTURE_SIZE, 1, uwOption);
			break;

		case IDM_CONTINUE_SHOT:
			Ux_SendEvent(&CustomPhotoObjCtrl, NVTEVT_EXE_CONTSHOT, 1, uwOption);
			break;

		case IDM_QUALITY:
			Ux_SendEvent(&CustomPhotoObjCtrl, NVTEVT_EXE_QUALITY, 1, uwOption);
			break;
		case IDM_SHARPNESS:
			Ux_SendEvent(&CustomPhotoObjCtrl, NVTEVT_EXE_SHARPNESS, 1, uwOption);
			break;
		case IDM_WB:
			Ux_SendEvent(&CustomPhotoObjCtrl, NVTEVT_EXE_WB, 1, uwOption);
			break;
		case IDM_ANTISHAKE:
			Ux_SendEvent(&CustomPhotoObjCtrl, NVTEVT_EXE_ANTISHAKING, 1, uwOption);
			break;
		case IDM_QUICK_REVIEW:
			Ux_SendEvent(&CustomPhotoObjCtrl, NVTEVT_EXE_PREVIEW, 1, uwOption);
			break;
		case IDM_DATE_STAMP:
			Ux_SendEvent(&CustomPhotoObjCtrl, NVTEVT_EXE_DATE_PRINT, 1, uwOption);
			break;
		case IDM_ISO:
			Ux_SendEvent(&CustomPhotoObjCtrl, NVTEVT_EXE_ISO, 1, uwOption);
			break;
		case IDM_EV:
			Ux_SendEvent(&CustomPhotoObjCtrl, NVTEVT_EXE_EV, 1, uwOption);
			break;
		case IDM_FD:
			Ux_SendEvent(&CustomPhotoObjCtrl, NVTEVT_EXE_FD, 1, uwOption);
			break;
		case IDM_FLASH:
			Ux_SendEvent(&CustomPhotoObjCtrl, NVTEVT_EXE_FLASH, 1, uwOption);
			break;
		//#NT#2016/08/19#Lincy Lin#[0106935] -begin
		//#NT# Support change WDR, SHDR, RSC setting will change mode after exit menu
		case IDM_RSC:
			//Ux_SendEvent(&CustomPhotoObjCtrl, NVTEVT_EXE_RSC, 1, uwOption);
			break;
		case IDM_HDR:
			//Ux_SendEvent(&CustomPhotoObjCtrl, NVTEVT_EXE_SHDR, 1, uwOption);
			break;
		case IDM_WDR:
			//Ux_SendEvent(&CustomPhotoObjCtrl, NVTEVT_EXE_WDR, 1, uwOption);
			break;
		//#NT#2016/08/19#Lincy Lin -end
		case IDM_METERING:
			Ux_SendEvent(&CustomPhotoObjCtrl, NVTEVT_EXE_METERING, 1, uwOption);
			break;
		case IDM_SCENE:
			Ux_SendEvent(&CustomPhotoObjCtrl, NVTEVT_EXE_SCENEMODE, 1, uwOption);
			break;
		case IDM_DUAL_CAM:
			break;
#endif
		default:
			Setup_MenuCallback(uiMessage, uiParam);
			break;
		}
	}

	return TMF_PROCESSED;
}
