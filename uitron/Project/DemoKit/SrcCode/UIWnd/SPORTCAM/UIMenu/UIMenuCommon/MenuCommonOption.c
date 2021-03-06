//This source code is generated by UI Designer Studio.


#include "UIFramework.h"
#include "UIFrameworkExt.h"
#include "UIGraphics.h"
#include "NVTToolCommand.h"
#include "MenuCommonOptionRes.c"
#include "MenuCommonOption.h"
#include "PrjCfg.h"
#include "UIFlow.h"

//#NT#2016/09/20#Bob Huang -begin
//#NT#Support HDMI Display with 3DNR Out
#if (_3DNROUT_FUNC == ENABLE)
extern BOOL gb3DNROut;
#endif
//#NT#2016/09/20#Bob Huang -end

//---------------------MenuCommonOptionCtrl Debug Definition -----------------------------
#define _MENUCOMMONOPTION_ERROR_MSG        1
#define _MENUCOMMONOPTION_TRACE_MSG        0

#if (_MENUCOMMONOPTION_ERROR_MSG&(PRJ_DBG_LVL>=PRJ_DBG_LVL_ERR))
#define MenuCommonOptionErrMsg(...)            debug_msg ("^R MenuCommonOption: "__VA_ARGS__)
#else
#define MenuCommonOptionErrMsg(...)
#endif

#if (_MENUCOMMONOPTION_TRACE_MSG&(PRJ_DBG_LVL>=PRJ_DBG_LVL_TRC))
#define MenuCommonOptionTraceMsg(...)          debug_msg ("^B MenuCommonOption: "__VA_ARGS__)
#else
#define MenuCommonOptionTraceMsg(...)
#endif

//---------------------MenuCommonOptionCtrl Global Variables -----------------------------
#define PAGE           4
//---------------------MenuCommonOptionCtrl Prototype Declaration  -----------------------

//---------------------MenuCommonOptionCtrl Public API  ----------------------------------
static UINT32 MenuCommonOption_Menu_CalcShiftFrom1stOption(UINT32 x, UINT32 y, Ux_RECT* MenuRect, UINT32 option_per_page, UINT32 colum_total);

//---------------------MenuCommonOptionCtrl Private API  ---------------------------------
static TM_MENU *g_pOptionMenu = 0;

static void MenuCommonOption_SetCurrentMenu(TM_MENU *pMenu)
{
	g_pOptionMenu = pMenu;
}

static TM_MENU *MenuCommonOption_GetCurrentMenu(void)
{
	return g_pOptionMenu;
}

static void MenuCommonOption_UpdateContent(TM_MENU *pMenu)
{
	TM_PAGE    *pPage;
	TM_ITEM    *pItem;
	TM_OPTION  *pOption;
	UINT32      i;
	//UINT16      startIndex = 0;
	//UINT16      itemIndex = 0;
	UINT16      SelOption = 0;
	pPage = &pMenu->pPages[pMenu->SelPage];
	pItem = &pPage->pItems[pPage->SelItem];
	SelOption = SysGetFlag(pItem->SysFlag);
	TM_CheckOptionStatus(pMenu, &SelOption, TRUE);
	SysSetFlag(pItem->SysFlag, SelOption); //SelOption might change

	pOption = &pItem->pOptions[SelOption];

    UxMenu_SetData(&MenuCommonOption_MenuCtrl, MNU_TOTITM, pItem->Count);
    for (i = 0; i < pItem->Count; i++)
    {
        pOption = &pItem->pOptions[i];
        UxMenu_SetItemData(&MenuCommonOption_MenuCtrl, i, MNUITM_STATUS, STATUS_ENABLE);
        UxMenu_SetItemData(&MenuCommonOption_MenuCtrl, i, MNUITM_STRID,  pOption->TextId);
    }

    UxStatic_SetData(&MenuCommonOption_Static_TitleCtrl, STATIC_VALUE, pItem->TextId);

}
static void MenuCommonOption_UpdatePosition(void)
{
	ISIZE str_size;
	//Ux_RECT MenuRect, NewRect;
    //#MT#lyb -20200317 -begin
	UINT32 strId = 0;//UxMenu_GetItemData(&MenuCommonOption_TitleCtrl, CURITEM_INDEX, MNUITM_STRID);
    //#MT#lyb -20200317 -end

#if (!defined(_NVT_ETHREARCAM_TX_))
	//avoid other tmp UI change font table
	GxGfx_SetTextStroke((const FONT *)gDemoKit_Font, FONTSTYLE_NORMAL, SCALE_1X);
#endif
	if (strId & TEXT_POINTER) {
		strId = (strId) & ~TEXT_POINTER;
		GxGfx_TextPrint(0, 0, 0, "%s", strId);
	} else {
		GxGfx_Text(0, 0, 0, GxGfx_GetStringID(strId));
	}

	str_size = GxGfx_GetTextLastSize();
    //#MT#lyb -20200317 -begin
    /*
	UxCtrl_GetPos(&MenuCommonOption_TitleCtrl, &MenuRect);
	UxCtrl_GetPos(&MenuCommonOption_StatusCtrl, &NewRect);
	NewRect.x1 = MenuRect.x1 + str_size.w + 5   ;
	UxCtrl_SetPos(&MenuCommonOption_StatusCtrl, NewRect);
	*/
    //#MT#lyb -20200317 -end

}

//---------------------MenuCommonOptionCtrl Control List---------------------------
CTRL_LIST_BEGIN(MenuCommonOption)
CTRL_LIST_ITEM(MenuCommonOption_Static_Title)
CTRL_LIST_ITEM(MenuCommonOption_Menu)
CTRL_LIST_ITEM(MenuCommonOption_Panel_Operate)
CTRL_LIST_END

//----------------------MenuCommonOptionCtrl Event---------------------------
INT32 MenuCommonOption_OnOpen(VControl *, UINT32, UINT32 *);
INT32 MenuCommonOption_OnClose(VControl *, UINT32, UINT32 *);
INT32 MenuCommonOption_OnChildClose(VControl *, UINT32, UINT32 *);
INT32 MenuCommonOption_OnStorageChange(VControl *, UINT32, UINT32 *);
EVENT_BEGIN(MenuCommonOption)
EVENT_ITEM(NVTEVT_OPEN_WINDOW, MenuCommonOption_OnOpen)
EVENT_ITEM(NVTEVT_CLOSE_WINDOW, MenuCommonOption_OnClose)
EVENT_ITEM(NVTEVT_CHILD_CLOSE, MenuCommonOption_OnChildClose)
EVENT_ITEM(NVTEVT_STORAGE_CHANGE, MenuCommonOption_OnStorageChange)
EVENT_END
extern BOOL  isCycRec;
INT32 MenuCommonOption_OnOpen(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	TM_MENU    *pMenu;
	TM_PAGE    *pPage;
	TM_ITEM    *pItem;
	TM_OPTION  *pOption;
	UINT16      SelOption = 0;


	//if (paramNum == 1) {
	{

		MenuCommonOption_SetCurrentMenu((TM_MENU *)paramArray[0]);
		pMenu = MenuCommonOption_GetCurrentMenu();
		pMenu->Status = TMS_ON_OPTION;
		pMenu->SelPage = 0;

		pPage = &pMenu->pPages[pMenu->SelPage];
		//pPage->SelItem = 0;
		//check item if disable
		TM_CheckItemStatus(pMenu, &pPage->SelItem, TRUE);
		pItem = &pPage->pItems[pPage->SelItem];
		if (pItem->Count > 0) {
			SelOption = SysGetFlag(pItem->SysFlag);
            TESTLOG("%d", SelOption);
           debug_msg(" --cj SelOption:%d \r\n", SelOption);
			if (isCycRec)
			{
				//SelOption -=1;
				 isCycRec =FALSE;
				 debug_msg(" --cj SelOption:%d \r\n", SelOption);

			}
			//check option if disable
			TM_CheckOptionStatus(pMenu, &SelOption, TRUE);
			SysSetFlag(pItem->SysFlag, SelOption); //SelOption might change
			debug_msg(" --cj SelOption:%d \r\n", SelOption);
			pOption = &pItem->pOptions[SelOption];
		}
		
        UxMenu_SetData(&MenuCommonOption_MenuCtrl, MNU_CURITM, SysGetFlag(pItem->SysFlag));

		MenuCommonOption_UpdateContent(pMenu);
		//MenuCommonOption_UpdatePosition();
		//UI_SetDisplayPalette(LAYER_OSD1, 0, 256, gDemoKit_PaletteOption_Palette);
		Ux_DefaultEvent(pCtrl, NVTEVT_OPEN_WINDOW, paramNum, paramArray);
	}
	return NVTEVT_CONSUME;
}
INT32 MenuCommonOption_OnClose(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	//UI_SetDisplayPalette(LAYER_OSD1, 0, 256, gDemoKit_Palette_Palette);
	Ux_DefaultEvent(pCtrl, NVTEVT_CLOSE_WINDOW, paramNum, paramArray);
	return NVTEVT_CONSUME;
}
INT32 MenuCommonOption_OnChildClose(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	TM_MENU    *pMenu = MenuCommonOption_GetCurrentMenu();
	pMenu->Status = TMS_ON_OPTION;
	MenuCommonOption_UpdateContent(pMenu);
	MenuCommonOption_UpdatePosition();
	Input_SetKeyMask(KEY_RELEASE, FLGKEY_KEY_MASK_DEFAULT);
	Ux_DefaultEvent(pCtrl, NVTEVT_CHILD_CLOSE, paramNum, paramArray);
	return NVTEVT_CONSUME;
}
INT32 MenuCommonOption_OnStorageChange(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
#if (SDHOTPLUG_FUNCTION == ENABLE)
	Ux_PostEvent(NVTEVT_SYSTEM_MODE, 1, System_GetState(SYS_STATE_CURRMODE));
#endif
	return NVTEVT_CONSUME;
}
//#MT#lyb -20200317 -begin
#if 0
//----------------------MenuCommonOption_ListCtrl Event---------------------------
INT32 MenuCommonOption_List_OnKeyNext(VControl *, UINT32, UINT32 *);
INT32 MenuCommonOption_List_OnKeyPrev(VControl *, UINT32, UINT32 *);
INT32 MenuCommonOption_List_OnKeySelect(VControl *, UINT32, UINT32 *);
INT32 MenuCommonOption_List_OnKeyExit(VControl *, UINT32, UINT32 *);
EVENT_BEGIN(MenuCommonOption_List)
EVENT_ITEM(NVTEVT_KEY_NEXT, MenuCommonOption_List_OnKeyNext)
EVENT_ITEM(NVTEVT_KEY_PREV, MenuCommonOption_List_OnKeyPrev)
EVENT_ITEM(NVTEVT_KEY_SELECT, MenuCommonOption_List_OnKeySelect)
EVENT_ITEM(NVTEVT_KEY_SHUTTER2, MenuCommonOption_List_OnKeyExit)

EVENT_END

INT32 MenuCommonOption_List_OnKeyNext(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	TM_MENU    *pMenu;
	TM_PAGE    *pPage;
	TM_ITEM    *pItem;

	if (paramNum == 1) {
		if (paramArray[0] == NVTEVT_KEY_PRESS) {
			pMenu = MenuCommonOption_GetCurrentMenu();
			pPage = &pMenu->pPages[pMenu->SelPage];
			pItem = &pPage->pItems[pPage->SelItem];

			pPage->SelItem++;
			//check item if disable
			TM_CheckItemStatus(pMenu, &pPage->SelItem, TRUE);

			if (pPage->SelItem == pPage->Count) {
				// Close current UI Window now
				Ux_CloseWindow(&MenuCommonOptionCtrl, 2, pItem->ItemId, 0);
			} else {
				MenuCommonOption_UpdateContent(pMenu);
				MenuCommonOption_UpdatePosition();
				Ux_SendEvent(pCtrl, NVTEVT_NEXT_ITEM, 0);
			}
		}
	}
	return NVTEVT_CONSUME;
}
INT32 MenuCommonOption_List_OnKeyPrev(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	TM_MENU    *pMenu;
	TM_PAGE    *pPage;
	TM_ITEM    *pItem;

	if (paramNum == 1) {
		if (paramArray[0] == NVTEVT_KEY_PRESS) {
			pMenu = MenuCommonOption_GetCurrentMenu();
			pPage = &pMenu->pPages[pMenu->SelPage];
			pItem = &pPage->pItems[pPage->SelItem];

			if (pPage->SelItem == 0) {
				// Close current UI Window now
				Ux_CloseWindow(&MenuCommonOptionCtrl, 2, pItem->ItemId, 0);
			} else {
				pPage->SelItem--;
				//check item if disable
				TM_CheckItemStatus(pMenu, &pPage->SelItem, FALSE);
				if (pPage->SelItem == pPage->Count) {
					// Close current UI Window now
					Ux_CloseWindow(&MenuCommonOptionCtrl, 2, pItem->ItemId, 0);
				} else {
					MenuCommonOption_UpdateContent(pMenu);
					MenuCommonOption_UpdatePosition();
					Ux_SendEvent(pCtrl, NVTEVT_PREVIOUS_ITEM, 0);
				}
			}
		}
	}
	return NVTEVT_CONSUME;
}

INT32 MenuCommonOption_List_OnKeySelect(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	TM_MENU    *pMenu;
	TM_PAGE    *pPage;
	TM_ITEM    *pItem;
	TM_OPTION  *pOption;
	UINT16      SelOption = 0;

	if (paramNum == 1) {
		pMenu = MenuCommonOption_GetCurrentMenu();
		pPage = &pMenu->pPages[pMenu->SelPage];
		pItem = &pPage->pItems[pPage->SelItem];

		if (paramArray[0] == NVTEVT_KEY_PRESS) {
			if (pItem->Count != 0 && pItem->SysFlag != 0) {
				//#NT#2016/09/20#Bob Huang -begin
				//#NT#Support HDMI Display with 3DNR Out
				//Only support FHD p30 size, cannot change size when 3DNR Out enabled
#if (_3DNROUT_FUNC == ENABLE)
				if (pItem->SysFlag == FL_MOVIE_SIZE_MENU && gb3DNROut) {
					Ux_SendEvent(pCtrl, NVTEVT_PRESS, 0);
					return NVTEVT_CONSUME;
				}
#endif
				//#NT#2016/09/20#Bob Huang -end
				SelOption = SysGetFlag(pItem->SysFlag);
				pMenu->Status = TMS_ON_OPTION;
				SelOption++;
				if (SelOption >= pItem->Count) {
					SelOption = 0;
				}
				TM_CheckOptionStatus(pMenu, &SelOption, TRUE);

				SysSetFlag(pItem->SysFlag, SelOption);
				// notify upper layer the Option had been confirmed
				TM_MENU_CALLBACK(pMenu, TMM_CONFIRM_OPTION, MAKE_LONG(pItem->ItemId, SelOption));

				pOption = &pItem->pOptions[SysGetFlag(pItem->SysFlag)];
				if (pOption->TextId & STRID_USER_START) {
					UxMenu_SetItemData(&MenuCommonOption_StatusCtrl, 0, MNUITM_STRID,  Txt_Pointer(UIRes_GetUserString(pOption->TextId)));
				} else {
					UxMenu_SetItemData(&MenuCommonOption_StatusCtrl, 0, MNUITM_STRID,  pOption->TextId);
				}

				if (pItem->ItemId == IDM_LANGUAGE) {
					MenuCommonOption_UpdatePosition();
				}

				Ux_SendEvent(pCtrl, NVTEVT_PRESS, 0);
			} else if (pItem->pOptions != 0) {              // custom process
				pMenu->Status = TMS_ON_CUSTOM;
				TM_ITEM_CALLBACK(pItem, TMM_CONFIRM_OPTION, pItem->ItemId);              // execute custom pPage flow
			}

		} else if (paramArray[0] == NVTEVT_KEY_RELEASE) {
			if (pItem->Count != 0 && pItem->SysFlag != 0) { // standard process
				Ux_SendEvent(pCtrl, NVTEVT_RELEASE, 0);
			}
		}

	}
	return NVTEVT_CONSUME;
}

INT32 MenuCommonOption_List_OnKeyExit(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	Ux_CloseWindow(&MenuCommonItemCtrl, 0);
	return NVTEVT_CONSUME;
}
#endif
//#MT#lyb -20200317 -end


//----------------------MenuCommonOption_Static_TitleCtrl Event---------------------------
EVENT_BEGIN(MenuCommonOption_Static_Title)
EVENT_END


//----------------------MenuCommonOption_MenuCtrl Event---------------------------
INT32 MenuCommonOption_Menu_OnKeyUp(VControl *, UINT32, UINT32 *);
INT32 MenuCommonOption_Menu_OnKeyDown(VControl *, UINT32, UINT32 *);
INT32 MenuCommonOption_Menu_OnKeySelect(VControl *, UINT32, UINT32 *);
INT32 MenuCommonOption_Menu_OnKeyExit(VControl *, UINT32, UINT32 *);
INT32 MenuCommonOption_Menu_OnTouchPanelClick(VControl *, UINT32, UINT32 *);

EVENT_BEGIN(MenuCommonOption_Menu)
EVENT_ITEM(NVTEVT_KEY_UP,MenuCommonOption_Menu_OnKeyUp)
EVENT_ITEM(NVTEVT_KEY_DOWN,MenuCommonOption_Menu_OnKeyDown)
EVENT_ITEM(NVTEVT_KEY_SELECT,MenuCommonOption_Menu_OnKeySelect)
EVENT_ITEM(NVTEVT_KEY_SHUTTER2,MenuCommonOption_Menu_OnKeyExit)
EVENT_ITEM(NVTEVT_CLICK,MenuCommonOption_Menu_OnTouchPanelClick)
EVENT_END

INT32 MenuCommonOption_Menu_OnKeyUp(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UINT32 uiKeyAct;
    uiKeyAct = paramArray[0];
    switch (uiKeyAct)
    {
        case NVTEVT_KEY_PRESS:
            Ux_SendEvent(pCtrl,NVTEVT_PREVIOUS_ITEM,0);    
        break;
    }
    return NVTEVT_CONSUME;
}
INT32 MenuCommonOption_Menu_OnKeyDown(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UINT32 uiKeyAct;
    uiKeyAct = paramArray[0];
    switch (uiKeyAct)
    {
        case NVTEVT_KEY_PRESS:
            Ux_SendEvent(pCtrl,NVTEVT_NEXT_ITEM,0);    
        break;
    }
    return NVTEVT_CONSUME;
}
INT32 MenuCommonOption_Menu_OnKeySelect(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    TM_MENU*    pMenu = TM_GetMenu();
    TM_PAGE*    pPage;
    TM_ITEM*    pItem;
    UINT32      uiSelOption;
    UINT32 uiKeyAct;
    uiKeyAct = paramArray[0];

    switch (uiKeyAct)
    {
      case NVTEVT_KEY_PRESS:
        pPage = &pMenu->pPages[pMenu->SelPage];
        pItem = &pPage->pItems[pPage->SelItem];

        uiSelOption = UxMenu_GetData(&MenuCommonOption_MenuCtrl, MNU_CURITM); // selected option
        
        // set system flag by selected Option
        SysSetFlag(pItem->SysFlag, uiSelOption);
        
        // notify upper layer the Option had been confirmed
        TM_MENU_CALLBACK(pMenu, TMM_CONFIRM_OPTION, MAKE_LONG(pItem->ItemId, uiSelOption));

        // Close current UI Window now
        Ux_CloseWindow(&MenuCommonOptionCtrl, 2,pItem->ItemId,uiSelOption);

    break;
    }
    return NVTEVT_CONSUME;
}

INT32 MenuCommonOption_Menu_OnKeyExit(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
   TM_MENU*    pMenu = TM_GetMenu();
    TM_PAGE*    pPage;
    TM_ITEM*    pItem;
    UINT32      uiSelOption;
    UINT32 uiKeyAct;

    uiKeyAct = paramArray[0];
    switch (uiKeyAct)
    {
      case NVTEVT_KEY_PRESS:        
        pPage = &pMenu->pPages[pMenu->SelPage];
        pItem = &pPage->pItems[pPage->SelItem];

        uiSelOption = UxMenu_GetData(&MenuCommonOption_MenuCtrl, MNU_CURITM); // selected option

        // Close current UI Window now
        Ux_CloseWindow(&MenuCommonOptionCtrl, 2, pItem->ItemId, uiSelOption);
        break;
    }
    return NVTEVT_CONSUME;
}




INT32 MenuCommonOption_Menu_OnTouchPanelClick(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    TM_MENU    *pMenu;
    TM_PAGE    *pPage;
    TM_ITEM    *pItem;
    UINT32 curSelOption = 0;
    UINT32 prevSelOption = 0;
    Ux_RECT MenuRect = {0,0,0,0};


    MenuCommonOption_Menu_CalcShiftFrom1stOption(paramArray[0], paramArray[1], &MenuRect, 4, 1);

    pMenu = MenuCommonOption_GetCurrentMenu();
    pPage = &pMenu->pPages[pMenu->SelPage];
    pItem = &pPage->pItems[pPage->SelItem];

    prevSelOption = SysGetFlag(pItem->SysFlag);
    curSelOption =   UxMenu_GetData(&MenuCommonOption_MenuCtrl, MNU_CURITM);
    TESTLOG("%d, %d", prevSelOption, curSelOption); // 2 ; 0
    if(prevSelOption != curSelOption)
    {
        SysSetFlag(pItem->SysFlag, curSelOption);

        MenuCommonOption_UpdateContent(pMenu);

        Ux_PostEvent(NVTEVT_KEY_SELECT, 1, NVTEVT_KEY_PRESS);
    }
    else
        Ux_PostEvent(NVTEVT_KEY_SELECT, 1, NVTEVT_KEY_PRESS);
    return NVTEVT_CONSUME;
}
static UINT32 MenuCommonOption_Menu_CalcShiftFrom1stOption(UINT32 x, UINT32 y, Ux_RECT* MenuRect, UINT32 option_per_page, UINT32 colum_total)
{
    UINT32 shiftNum = 0;
    UINT32 shiftNum_x, shiftNum_y = 0;
    UINT32 OptionWidth, OptionHeight = 0;
    OptionHeight = (MenuRect->y2 - MenuRect->y1)/option_per_page;
    OptionWidth = (MenuRect->x2 - MenuRect->x1)/colum_total;

    if(y < (UINT32)(MenuRect->y1) || y > (UINT32)(MenuRect->y2) || x < (UINT32)(MenuRect->x1) || x > (UINT32)(MenuRect->x2))
        return 0xffff;
    shiftNum_x = (x - MenuRect->x1)/OptionWidth;
    shiftNum_y = (y - MenuRect->y1)/OptionHeight;
    shiftNum = (shiftNum_x + shiftNum_y * option_per_page);
    if(UxMenu_GetItemData(&MenuCommonOption_MenuCtrl, shiftNum, LSTITM_STATUS) == STATUS_ENABLE)
        return shiftNum;
    return 0xffff;
}


//---------------------MenuCommonOption_Panel_OperateCtrl Control List---------------------------
CTRL_LIST_BEGIN(MenuCommonOption_Panel_Operate)
CTRL_LIST_ITEM(MenuCommonOption_BTN_Return)
CTRL_LIST_ITEM(MenuCommonOption_BTN_Exit)
CTRL_LIST_END

//----------------------MenuCommonOption_Panel_OperateCtrl Event---------------------------
EVENT_BEGIN(MenuCommonOption_Panel_Operate)
EVENT_END

//----------------------MenuCommonOption_BTN_ReturnCtrl Event---------------------------
INT32 MenuCommonOption_BTN_Return_OnTouchPanelPress(VControl *, UINT32, UINT32 *);
INT32 MenuCommonOption_BTN_Return_OnTouchPanelRelease(VControl *, UINT32, UINT32 *);
INT32 MenuCommonOption_BTN_Return_OnTouchPanelClick(VControl *, UINT32, UINT32 *);
EVENT_BEGIN(MenuCommonOption_BTN_Return)
EVENT_ITEM(NVTEVT_PRESS,MenuCommonOption_BTN_Return_OnTouchPanelPress)
EVENT_ITEM(NVTEVT_USER_RELEASE,MenuCommonOption_BTN_Return_OnTouchPanelRelease)
EVENT_ITEM(NVTEVT_CLICK,MenuCommonOption_BTN_Return_OnTouchPanelClick)
EVENT_END

INT32 MenuCommonOption_BTN_Return_OnTouchPanelPress(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UxButton_SetItemData(&MenuCommonOption_BTN_ReturnCtrl, 0, BTNITM_ICONID, ICON_OPERATE_RETURN_F);
    return NVTEVT_CONSUME;
}

INT32 MenuCommonOption_BTN_Return_OnTouchPanelRelease(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UxButton_SetItemData(&MenuCommonOption_BTN_ReturnCtrl, 0, BTNITM_ICONID, ICON_OPERATE_RETURN);
    return NVTEVT_CONSUME;
}

INT32 MenuCommonOption_BTN_Return_OnTouchPanelClick(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    Ux_CloseWindow(&MenuCommonOptionCtrl, 0);
    return NVTEVT_CONSUME;
}
//----------------------MenuCommonOption_BTN_ExitCtrl Event---------------------------
INT32 MenuCommonOption_BTN_Exit_OnTouchPanelPress(VControl *, UINT32, UINT32 *);
INT32 MenuCommonOption_BTN_Exit_OnTouchPanelRelease(VControl *, UINT32, UINT32 *);
INT32 MenuCommonOption_BTN_Exit_OnTouchPanelClick(VControl *, UINT32, UINT32 *);
EVENT_BEGIN(MenuCommonOption_BTN_Exit)
EVENT_ITEM(NVTEVT_PRESS,MenuCommonOption_BTN_Exit_OnTouchPanelPress)
EVENT_ITEM(NVTEVT_USER_RELEASE,MenuCommonOption_BTN_Exit_OnTouchPanelRelease)
EVENT_ITEM(NVTEVT_CLICK,MenuCommonOption_BTN_Exit_OnTouchPanelClick)
EVENT_END

INT32 MenuCommonOption_BTN_Exit_OnTouchPanelPress(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UxButton_SetItemData(&MenuCommonOption_BTN_ExitCtrl, 0, BTNITM_ICONID, ICON_OPERATE_EXIT_F);
    return NVTEVT_CONSUME;
}

INT32 MenuCommonOption_BTN_Exit_OnTouchPanelRelease(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UxButton_SetItemData(&MenuCommonOption_BTN_ExitCtrl, 0, BTNITM_ICONID, ICON_OPERATE_EXIT);
    return NVTEVT_CONSUME;
}

INT32 MenuCommonOption_BTN_Exit_OnTouchPanelClick(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    Ux_CloseWindow(&MenuCommonOptionCtrl, 1, 0);

    return NVTEVT_CONSUME;
}

