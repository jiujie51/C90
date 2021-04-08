//This source code is generated by UI Designer Studio.

#include "UIFramework.h"
#include "UIFrameworkExt.h"
#include "UIGraphics.h"
#include "NVTToolCommand.h"
#include "LCDTestWndRes.c"
#include "LCDTestWnd.h"
#include "PrjCfg.h"
#include "GxTimer.h"
#include "UIFlow.h"
#include "BurnInWnd.h"

#define __MODULE__          LCDTest
#define __DBGLVL__          1 // 0=OFF, 1=ERROR, 2=TRACE
#define __DBGFLT__          "*" //*=All, [mark]=CustomClass
#include "DebugModule.h"

#define LCD_TEST_STATE_MAX  5

//---------------------LCDTestWndCtrl Debug Definition -----------------------------
#define _LCDTESTWND_ERROR_MSG        1
#define _LCDTESTWND_TRACE_MSG        0

#if (_LCDTESTWND_ERROR_MSG&(PRJ_DBG_LVL>=PRJ_DBG_LVL_ERR))
#define LCDTestWndErrMsg(...)            debug_msg ("^R LCDTestWnd: "__VA_ARGS__)
#else
#define LCDTestWndErrMsg(...)
#endif

#if (_LCDTESTWND_TRACE_MSG&(PRJ_DBG_LVL>=PRJ_DBG_LVL_TRC))
#define LCDTestWndTraceMsg(...)          debug_msg ("^B LCDTestWnd: "__VA_ARGS__)
#else
#define LCDTestWndTraceMsg(...)
#endif

//---------------------LCDTestWndCtrl Global Variables -----------------------------
static UINT32   g_uiLCDTestState = 0;
static UINT32   g_uiLCDTestTimer = NULL_TIMER;

//---------------------LCDTestWndCtrl Prototype Declaration  -----------------------

//---------------------LCDTestWndCtrl Public API  ----------------------------------

//---------------------LCDTestWndCtrl Private API  ---------------------------------
static void LCDTestWnd_DrawTestPattern(UINT32 uiState)
{
	debug_msg("^C(10) LCDTestWnd_DrawTestPattern\r\n");
    switch (uiState)
    {
    case 1:
        UI_ClearOSD(CLRID_IDX_BLACK);
        break;

    case 2:
        UI_ClearOSD(CLRID_IDX_RED);
        break;

    case 3:
        UI_ClearOSD(CLRID_IDX_GREEN);
        break;

    case 4:
        UI_ClearOSD(CLRID_IDX_BLUE);
        break;

    case 5:
        UxCtrl_SetShow(&LCDTestWnd_Panel1Ctrl, TRUE);
        Ux_Redraw();
        break;
    }
}

//---------------------LCDTestWndCtrl Control List---------------------------
CTRL_LIST_BEGIN(LCDTestWnd)
CTRL_LIST_ITEM(LCDTestWnd_Panel1)
CTRL_LIST_END

//----------------------LCDTestWndCtrl Event---------------------------
INT32 LCDTestWnd_OnOpen(VControl *, UINT32, UINT32 *);
INT32 LCDTestWnd_OnClose(VControl *, UINT32, UINT32 *);
INT32 LCDTestWnd_OnTimer(VControl *, UINT32, UINT32 *);
EVENT_BEGIN(LCDTestWnd)
EVENT_ITEM(NVTEVT_OPEN_WINDOW,LCDTestWnd_OnOpen)
EVENT_ITEM(NVTEVT_CLOSE_WINDOW,LCDTestWnd_OnClose)
EVENT_ITEM(NVTEVT_TIMER,LCDTestWnd_OnTimer)
EVENT_END

INT32 LCDTestWnd_OnOpen(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    g_uiLCDTestState = 0;

    if (g_uiLCDTestTimer == NULL_TIMER)
    {
        g_uiLCDTestTimer = GxTimer_StartTimer(TIMER_ONE_SEC, NVTEVT_1SEC_TIMER, CONTINUE);
    }

    UxCtrl_SetShow(&LCDTestWnd_Panel1Ctrl, FALSE);
    Ux_FlushEventByRange(NVTEVT_KEY_EVT_START,NVTEVT_KEY_EVT_END);
    Ux_DefaultEvent(pCtrl,NVTEVT_OPEN_WINDOW,paramNum,paramArray);

    Ux_Redraw();
    BurnIn_DrawLoopCount(FALSE);
    BurnIn_DrawStatus("LCD Test", CLRID_IDX_WHITE, FALSE);

    return NVTEVT_CONSUME;
}

INT32 LCDTestWnd_OnClose(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    if (g_uiLCDTestTimer != NULL_TIMER)
    {
        GxTimer_StopTimer(&g_uiLCDTestTimer);
    }

    Ux_DefaultEvent(pCtrl,NVTEVT_CLOSE_WINDOW,paramNum,paramArray);
    return NVTEVT_CONSUME;
}

INT32 LCDTestWnd_OnTimer(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UINT32  uiEvent;

    uiEvent = paramNum ? paramArray[0] : 0;

    switch(uiEvent)
    {
    case NVTEVT_1SEC_TIMER:
        g_uiLCDTestState++;
        if (g_uiLCDTestState > LCD_TEST_STATE_MAX)
        {
            Ux_CloseWindow((VControl *)(&LCDTestWndCtrl), 0);
        }
        else
        {
            LCDTestWnd_DrawTestPattern(g_uiLCDTestState);
            BurnIn_DrawLoopCount(FALSE);
            BurnIn_DrawStatus("LCD Test", CLRID_IDX_WHITE, FALSE);
        }
        break;
    }

    return NVTEVT_CONSUME;
}

//---------------------LCDTestWnd_Panel1Ctrl Control List---------------------------
CTRL_LIST_BEGIN(LCDTestWnd_Panel1)
CTRL_LIST_END

//----------------------LCDTestWnd_Panel1Ctrl Event---------------------------
EVENT_BEGIN(LCDTestWnd_Panel1)
EVENT_END
