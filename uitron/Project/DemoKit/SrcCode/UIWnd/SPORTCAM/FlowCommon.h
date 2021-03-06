//This source code is generated by UI Designer Studio.

#ifndef FLOWCOMMON_H
#define FLOWCOMMON_H

#include "UIFramework.h"

extern VControl FlowCommonCtrl;//Panel type

extern VControl GridCommCtrl;//Panel type

extern VControl WideBlack32CommCtrl;//Panel type

extern VControl WideBlack32UpCtrl;//Panel type

extern VControl WideBlack32DownCtrl;//Panel type

extern VControl WideBlack169CommCtrl;//Panel type

extern VControl WideBlack169UpCtrl;//Panel type

extern VControl WideBlack169DownCtrl;//Panel type

extern VControl ZoomCommCtrl;//Static type

extern VControl ModeCommCtrl;//Static type

extern VControl AFWindowCommCtrl;//Status type

typedef enum {
	AFWindowComm_ICONID_AF_1,
	AFWindowComm_ICONID_AF_2,
	AFWindowComm_MAX
} AFWindowComm_Setting;

extern VControl ContAFCommCtrl;//Static type

extern VControl AEModeCommCtrl;//Status type

typedef enum {
	AEModeComm_ICONID_AE_1,
	AEModeComm_ICONID_AE_2,
	AEModeComm_ICONID_AE_3,
	AEModeComm_MAX
} AEModeComm_Setting;

extern VControl FlowerCommCtrl;//Static type

extern VControl ZoomBarPanelCommCtrl;//Panel type

extern VControl ZoomBarBgStatusCommCtrl;//Status type

typedef enum {
	ZoomBarBgStatusComm_ICONID_WT_BAR,
	ZoomBarBgStatusComm_ICONID_WT_BAR_DGO,
	ZoomBarBgStatusComm_ICONID_WT_BAR_OPO,
	ZoomBarBgStatusComm_ICONID_WT_BAR_UN,
	ZoomBarBgStatusComm_MAX
} ZoomBarBgStatusComm_Setting;

extern VControl ZoomOpticalSliderBarCommCtrl;//SliderBar type

extern VControl ZoomDigitalSliderBarCommCtrl;//SliderBar type

extern VControl ZoomTempSliderBarCommCtrl;//SliderBar type

extern VControl HistogramCommCtrl;//Panel type

extern VControl EvIconCommCtrl;//Static type

extern VControl EvValueCommCtrl;//Static type

extern VControl EvValueIconCommCtrl;//Static type

extern VControl IsoValueCommCtrl;//Static type

extern VControl IsoValueIconCommCtrl;//Static type

extern VControl StatusBatteryCommCtrl;//Status type

typedef enum {
	StatusBatteryComm_ICONID_BATTERY_1,
	StatusBatteryComm_ICONID_BATTERY_2,
	StatusBatteryComm_ICONID_BATTERY_3,
	StatusBatteryComm_ICONID_BATTERY_6,
	StatusBatteryComm_ICONID_BATTERY_4,
	StatusBatteryComm_ICONID_BATTERY_5,
	StatusBatteryComm_MAX
} StatusBatteryComm_Setting;

extern VControl StatusStorageCommCtrl;//Status type

typedef enum {
	StatusStorageComm_ICONID_MEMORY_IN,
	StatusStorageComm_ICONID_MEMORY_SD,
	StatusStorageComm_MAX
} StatusStorageComm_Setting;

extern VControl AFWindowActCommCtrl;//Static type

extern VControl AEModeActCommCtrl;//Static type

extern VControl FlowerActCommCtrl;//Static type

extern void ZoomBarPanelComm_Shown(void);
extern void ZoomBarPanelComm_OffShown(void);
//#NT#2011/03/22#Lincy Lin -begin
//#NT#
extern void ZoomBarPanelComm_UpdateStatus(void);
//#NT#2011/03/22#Lincy Lin -end
extern void DrawBatteryComm(BOOL bDraw);
extern void DrawStorageComm(BOOL bDraw);

//#NT#2009/12/17#Ben Wang -begin
//#NT#Add common utility
extern INT32 UIFunc_Null(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray);
extern INT32 WndCom_OnKeyPlayback(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray);
extern INT32 WndCom_OnKeyShutter1Press(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray);
extern INT32 WndCom_OnKeyShutter2(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray);
extern INT32 WndCom_OnKeyMenu(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray);
//#NT#2010/02/01#Ben Wang -begin
extern INT32 WndCom_OnKeyMode(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray);
//#NT#2010/02/01#Ben Wang -end
//#NT#2010/09/29#Photon Lin -begin
//#Add event for movie key
extern INT32 WndCom_OnKeyMovie(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray);
extern INT32 WndCom_OnKeyI(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray);
//#NT#2010/09/29#Photon Lin -end
extern INT32 WndCom_OnKeyLeft(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray);
extern INT32 WndCom_OnCloseToRoot(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray);
extern void  Ux_CloseToRootWindow(VControl *pCtrl);
//#NT#2009/12/17#Ben Wang -end
//#NT#2009/12/30#Ben Wang -begin
//#NT#Add check for folder attribute
//#NT#2010/05/19#Lincy Lin -begin
//#NT#Move to UIStorageCheck.h
//extern BOOL check_DCIM_writable(void);
//extern BOOL check_CurrDir_writable(void);
//#NT#2010/05/19#Lincy Lin -end
//#NT#2009/12/30#Ben Wang -end
#endif
