////////////////////////////////////////////////////////////////////////////////
#include "SysCommon.h"
//#include "AppCommon.h"
////////////////////////////////////////////////////////////////////////////////
#include "AppLib.h"
#include "UIMode.h"
//UIControl
#include "UIFrameworkExt.h"
#include "DeviceCtrl.h"
#if 1//(UI_STYLE==UI_STYLE_DRIVER)
//#include "UIMenuWndUSB.h"
#include "MenuCommonPlayMenu.h"
#endif

int PRIMARY_MODE_PLAYMENU = -1;

void ModePlayMenu_Open(void);
void ModePlayMenu_Close(void);

void ModePlayMenu_Open(void)
{
#if 1//(UI_STYLE==UI_STYLE_DRIVER)
    Input_ResetMask();
    //Ux_OpenWindow(&UIMenuWndUSBCtrl, 0);
	  Ux_OpenWindow((VControl *)(&MenuCommonPlayMenuCtrl), 0);
#endif
}
void ModePlayMenu_Close(void)
{
#if 1//(UI_STYLE==UI_STYLE_DRIVER)
    //Ux_CloseWindow(&UIMenuWndUSBCtrl, 0);
  Ux_CloseWindow((VControl *)(&MenuCommonPlayMenuCtrl), 0);
#endif
}

SYS_MODE gModePlayMenu =
{
    "PLAYMenu",
    ModePlayMenu_Open,
    ModePlayMenu_Close,
    NULL,
    NULL,
    NULL,
    NULL
};
