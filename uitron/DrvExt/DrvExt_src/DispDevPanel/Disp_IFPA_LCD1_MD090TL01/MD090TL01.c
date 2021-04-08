/*
    Display object object panel configuration parameters for T30P105

    @file       T30P106.c
    @ingroup
    @note       Nothing

    Copyright   Novatek Microelectronics Corp. 2011.  All rights reserved.
*/

#include "kernel.h"
#include "Debug.h"
#include "display.h"
#include "dispdev_IFParal.h"
#include "dispdev_panlcomn.h"
#include "i2c.h"
#define DRV_IICDATA		0x7fffffff
#define DRV_DELAYMS		0x8fffffff

#define PANEL_WIDTH     1280//360
#define PANEL_HEIGHT    320

#define HSYNCT      64
#define VSYNCT      8


/*
    Panel Parameters for T30P105
*/
const tPANEL_CMD tCmdStandby[] =
{
};


const tPANEL_CMD tCmdModeRgbd320[] =
{
	{DRV_IICDATA,0x00020001},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x00020000},
	//**************************************************-->
	//TC358768XBGPLL,ClockSetting-->
	//**************************************************-->
	{DRV_IICDATA,0x00161095},
	{DRV_IICDATA,0x00180A03},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x00180A13},
	//**************************************************-->
	//TC358768XBGDPIInputControl-->
	//**************************************************-->
	{DRV_IICDATA,0x00060050},
	//**************************************************-->
	//TC358768XBGD-PHYSetting-->
	//**************************************************-->
	{DRV_IICDATA,0x01400000},
	{DRV_IICDATA,0x01420000},
	{DRV_IICDATA,0x01440000},
	{DRV_IICDATA,0x01460000},
	{DRV_IICDATA,0x01480000},
	{DRV_IICDATA,0x014A0000},
	{DRV_IICDATA,0x014C0000},
	{DRV_IICDATA,0x014E0000},
	{DRV_IICDATA,0x01500000},
	{DRV_IICDATA,0x01520000},
	{DRV_IICDATA,0x01000002},
	{DRV_IICDATA,0x01020000},
	{DRV_IICDATA,0x01040002},
	{DRV_IICDATA,0x01060000},
	{DRV_IICDATA,0x01080002},
	{DRV_IICDATA,0x010A0000},
	{DRV_IICDATA,0x010C0002},
	{DRV_IICDATA,0x010E0000},
	{DRV_IICDATA,0x01100002},
	{DRV_IICDATA,0x01120000},
	//**************************************************-->
	//TC358768XBGDSI-TXPPIControl-->
	//**************************************************-->
	{DRV_IICDATA,0x02101644},
	{DRV_IICDATA,0x02120000},
	{DRV_IICDATA,0x02140001},
	{DRV_IICDATA,0x02160000},
	{DRV_IICDATA,0x02182000},
	{DRV_IICDATA,0x021A0000},
	{DRV_IICDATA,0x02200601},
	{DRV_IICDATA,0x02220000},
	{DRV_IICDATA,0x02244268},
	{DRV_IICDATA,0x02260000},
	{DRV_IICDATA,0x022C0000},
	{DRV_IICDATA,0x022E0000},
	{DRV_IICDATA,0x02300005},
	{DRV_IICDATA,0x02320000},
	{DRV_IICDATA,0x0234001F},
	{DRV_IICDATA,0x02360000},
	{DRV_IICDATA,0x02380001},
	{DRV_IICDATA,0x023A0000},
	{DRV_IICDATA,0x023C0001},
	{DRV_IICDATA,0x023E0001},
	{DRV_IICDATA,0x02040001},
	{DRV_IICDATA,0x02060000},
	//**************************************************-->
	//TC358768XBGDSI-TXTimingControl-->
	//**************************************************-->
	{DRV_IICDATA,0x06200001},
	{DRV_IICDATA,0x0622000C},
	{DRV_IICDATA,0x06240004},
	{DRV_IICDATA,0x06260500},
	{DRV_IICDATA,0x062800E9},
	{DRV_IICDATA,0x062A0053},
	{DRV_IICDATA,0x062C0280},
	{DRV_IICDATA,0x05180001},
	{DRV_IICDATA,0x051A0000},
	//**************************************************-->
	//LCDD(Peripheral)Setting-->
	//**************************************************-->
	{DRV_IICDATA,0x06021013},
	{DRV_IICDATA,0x06040000},
	{DRV_IICDATA,0x06100001},
	{DRV_IICDATA,0x06000001},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x06024029},
	{DRV_IICDATA,0x06040003},
	{DRV_IICDATA,0x06105AF0},
	{DRV_IICDATA,0x06120059},
	{DRV_IICDATA,0x06000001},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x06024029},
	{DRV_IICDATA,0x06040003},
	{DRV_IICDATA,0x0610A5F1},
	{DRV_IICDATA,0x061200A6},
	{DRV_IICDATA,0x06000001},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x00080001},
	{DRV_IICDATA,0x00500029},
	{DRV_IICDATA,0x00220017},
	{DRV_IICDATA,0x00E08000},
	{DRV_IICDATA,0x00E800B4},
	{DRV_IICDATA,0x00E80504},
	{DRV_IICDATA,0x00E81E00},
	{DRV_IICDATA,0x00E8011F},
	{DRV_IICDATA,0x00E81110},
	{DRV_IICDATA,0x00E81312},
	{DRV_IICDATA,0x00E80D0C},
	{DRV_IICDATA,0x00E80F0E},
	{DRV_IICDATA,0x00E80303},
	{DRV_IICDATA,0x00E80303},
	{DRV_IICDATA,0x00E80303},
	{DRV_IICDATA,0x00E80003},
	{DRV_IICDATA,0x00E0E000},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x00E02000},
	{DRV_IICDATA,0x00E00000},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x00080001},
	{DRV_IICDATA,0x00500029},
	{DRV_IICDATA,0x00220010},
	{DRV_IICDATA,0x00E08000},
	{DRV_IICDATA,0x00E8C6B0},
	{DRV_IICDATA,0x00E8BBBB},
	{DRV_IICDATA,0x00E833BB},
	{DRV_IICDATA,0x00E83333},
	{DRV_IICDATA,0x00E82433},
	{DRV_IICDATA,0x00E80000},
	{DRV_IICDATA,0x00E8009C},
	{DRV_IICDATA,0x00E88000},
	{DRV_IICDATA,0x00E0E000},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x00E02000},
	{DRV_IICDATA,0x00E00000},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x00080001},
	{DRV_IICDATA,0x00500029},
	{DRV_IICDATA,0x0022000C},
	{DRV_IICDATA,0x00E08000},
	{DRV_IICDATA,0x00E853B1},
	{DRV_IICDATA,0x00E800A0},
	{DRV_IICDATA,0x00E80485},
	{DRV_IICDATA,0x00E80000},
	{DRV_IICDATA,0x00E8007C},
	{DRV_IICDATA,0x00E85F00},
	{DRV_IICDATA,0x00E0E000},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x00E02000},
	{DRV_IICDATA,0x00E00000},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x00080001},
	{DRV_IICDATA,0x00500029},
	{DRV_IICDATA,0x0022000B},
	{DRV_IICDATA,0x00E08000},
	{DRV_IICDATA,0x00E807B2},
	{DRV_IICDATA,0x00E80809},
	{DRV_IICDATA,0x00E8088B},
	{DRV_IICDATA,0x00E82200},
	{DRV_IICDATA,0x00E84400},
	{DRV_IICDATA,0x00E800D9},
	{DRV_IICDATA,0x00E0E000},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x00E02000},
	{DRV_IICDATA,0x00E00000},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x06024029},
	{DRV_IICDATA,0x06040003},
	{DRV_IICDATA,0x06104AB6},
	{DRV_IICDATA,0x0612004A},
	{DRV_IICDATA,0x06000001},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x00080001},
	{DRV_IICDATA,0x00500029},
	{DRV_IICDATA,0x00220013},
	{DRV_IICDATA,0x00E08000},
	{DRV_IICDATA,0x00E801B7},
	{DRV_IICDATA,0x00E80901},
	{DRV_IICDATA,0x00E8110D},
	{DRV_IICDATA,0x00E81D19},
	{DRV_IICDATA,0x00E80015},
	{DRV_IICDATA,0x00E82125},
	{DRV_IICDATA,0x00E80000},
	{DRV_IICDATA,0x00E80000},
	{DRV_IICDATA,0x00E8F702},
	{DRV_IICDATA,0x00E80038},
	{DRV_IICDATA,0x00E0E000},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x00E02000},
	{DRV_IICDATA,0x00E00000},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x06024029},
	{DRV_IICDATA,0x06040005},
	{DRV_IICDATA,0x0610B8B8},
	{DRV_IICDATA,0x06120252},
	{DRV_IICDATA,0x061400CC},
	{DRV_IICDATA,0x06000001},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x06024029},
	{DRV_IICDATA,0x06040003},
	{DRV_IICDATA,0x061027BA},
	{DRV_IICDATA,0x061200D3},
	{DRV_IICDATA,0x06000001},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x06024029},
	{DRV_IICDATA,0x06040008},
	{DRV_IICDATA,0x061043BD},
	{DRV_IICDATA,0x06120E0E},
	{DRV_IICDATA,0x06144B4B},
	{DRV_IICDATA,0x06161432},
	{DRV_IICDATA,0x06000001},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x00080001},
	{DRV_IICDATA,0x00500029},
	{DRV_IICDATA,0x00220009},
	{DRV_IICDATA,0x00E08000},
	{DRV_IICDATA,0x00E800C1},
	{DRV_IICDATA,0x00E80E0F},
	{DRV_IICDATA,0x00E80001},
	{DRV_IICDATA,0x00E83A36},
	{DRV_IICDATA,0x00E80008},
	{DRV_IICDATA,0x00E0E000},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x00E02000},
	{DRV_IICDATA,0x00E00000},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x06024029},
	{DRV_IICDATA,0x06040003},
	{DRV_IICDATA,0x061022C2},
	{DRV_IICDATA,0x06120080},
	{DRV_IICDATA,0x06000001},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x06024029},
	{DRV_IICDATA,0x06040003},
	{DRV_IICDATA,0x061022C3},
	{DRV_IICDATA,0x06120031},
	{DRV_IICDATA,0x06000001},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x00080001},
	{DRV_IICDATA,0x00500029},
	{DRV_IICDATA,0x00220009},
	{DRV_IICDATA,0x00E08000},
	{DRV_IICDATA,0x00E800C6},
	{DRV_IICDATA,0x00E8FF00},
	{DRV_IICDATA,0x00E80000},
	{DRV_IICDATA,0x00E800FF},
	{DRV_IICDATA,0x00E80000},
	{DRV_IICDATA,0x00E0E000},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x00E02000},
	{DRV_IICDATA,0x00E00000},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x00080001},
	{DRV_IICDATA,0x00500029},
	{DRV_IICDATA,0x00220027},
	{DRV_IICDATA,0x00E08000},
	{DRV_IICDATA,0x00E87CC8},
	{DRV_IICDATA,0x00E85C69},
	{DRV_IICDATA,0x00E84E50},
	{DRV_IICDATA,0x00E84540},
	{DRV_IICDATA,0x00E8472F},
	{DRV_IICDATA,0x00E84545},
	{DRV_IICDATA,0x00E85262},
	{DRV_IICDATA,0x00E84E5C},
	{DRV_IICDATA,0x00E83F4D},
	{DRV_IICDATA,0x00E8062D},
	{DRV_IICDATA,0x00E8697C},
	{DRV_IICDATA,0x00E8505C},
	{DRV_IICDATA,0x00E8404E},
	{DRV_IICDATA,0x00E82F45},
	{DRV_IICDATA,0x00E84547},
	{DRV_IICDATA,0x00E86245},
	{DRV_IICDATA,0x00E85C52},
	{DRV_IICDATA,0x00E84D4E},
	{DRV_IICDATA,0x00E82D3F},
	{DRV_IICDATA,0x00E80006},
	{DRV_IICDATA,0x00E80000},
	{DRV_IICDATA,0x00E0E000},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x00E02000},
	{DRV_IICDATA,0x00E00000},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x06024029},
	{DRV_IICDATA,0x06040004},
	{DRV_IICDATA,0x061007D0},
	{DRV_IICDATA,0x0612FFFF},
	{DRV_IICDATA,0x06000001},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x06024029},
	{DRV_IICDATA,0x06040005},
	{DRV_IICDATA,0x061063D2},
	{DRV_IICDATA,0x0612080B},
	{DRV_IICDATA,0x06140088},
	{DRV_IICDATA,0x06000001},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x06024029},
	{DRV_IICDATA,0x06040007},
	{DRV_IICDATA,0x061000D4},
	{DRV_IICDATA,0x06120000},
	{DRV_IICDATA,0x06140432},
	{DRV_IICDATA,0x06160051},
	{DRV_IICDATA,0x06000001},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x06024029},
	{DRV_IICDATA,0x06040003},
	{DRV_IICDATA,0x06105AF1},
	{DRV_IICDATA,0x06120059},
	{DRV_IICDATA,0x06000001},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x06021013},
	{DRV_IICDATA,0x06040000},
	{DRV_IICDATA,0x06100011},
	{DRV_IICDATA,0x06000001},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x06021013},
	{DRV_IICDATA,0x06040000},
	{DRV_IICDATA,0x06100029},
	{DRV_IICDATA,0x06000001},
	{DRV_DELAYMS,1},
	//**************************************************-->
	//SettoHSmode-->
	//**************************************************-->
	{DRV_IICDATA,0x05000086},
	{DRV_IICDATA,0x0502A300},
	{DRV_IICDATA,0x05008000},
	{DRV_IICDATA,0x0502C300},
	//**************************************************-->
	//Host:RGB(DPI)inputstart-->
	//**************************************************-->
	{DRV_IICDATA,0x00080057},
	{DRV_IICDATA,0x0050000E},
	{DRV_IICDATA,0x00320000},
	{DRV_IICDATA,0x00040144},
	//**************************************************-->
	//LCDD(Peripheral)Setting-->
	//**************************************************-->



	/******
	{DRV_IICDATA,0x00020001},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x00020000},
	//**************************************************-->
	//TC358768XBGPLL,ClockSetting-->
	//**************************************************-->
	{DRV_IICDATA,0x00161095},
	{DRV_IICDATA,0x00180A03},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x00180A13},
	//**************************************************-->
	//TC358768XBGDPIInputControl-->
	//**************************************************-->
	{DRV_IICDATA,0x00060050},
	//**************************************************-->
	//TC358768XBGD-PHYSetting-->
	//**************************************************-->
	{DRV_IICDATA,0x01400000},
	{DRV_IICDATA,0x01420000},
	{DRV_IICDATA,0x01440000},
	{DRV_IICDATA,0x01460000},
	{DRV_IICDATA,0x01480000},
	{DRV_IICDATA,0x014A0000},
	{DRV_IICDATA,0x014C0000},
	{DRV_IICDATA,0x014E0000},
	{DRV_IICDATA,0x01500000},
	{DRV_IICDATA,0x01520000},
	{DRV_IICDATA,0x01000002},
	{DRV_IICDATA,0x01020000},
	{DRV_IICDATA,0x01040002},
	{DRV_IICDATA,0x01060000},
	{DRV_IICDATA,0x01080002},
	{DRV_IICDATA,0x010A0000},
	{DRV_IICDATA,0x010C0002},
	{DRV_IICDATA,0x010E0000},
	{DRV_IICDATA,0x01100002},
	{DRV_IICDATA,0x01120000},
	//**************************************************-->
	//TC358768XBGDSI-TXPPIControl-->
	//**************************************************-->
	{DRV_IICDATA,0x02101644},
	{DRV_IICDATA,0x02120000},
	{DRV_IICDATA,0x02140001},
	{DRV_IICDATA,0x02160000},
	{DRV_IICDATA,0x02182000},
	{DRV_IICDATA,0x021A0000},
	{DRV_IICDATA,0x02200601},
	{DRV_IICDATA,0x02220000},
	{DRV_IICDATA,0x02244268},
	{DRV_IICDATA,0x02260000},
	{DRV_IICDATA,0x022C0000},
	{DRV_IICDATA,0x022E0000},
	{DRV_IICDATA,0x02300005},
	{DRV_IICDATA,0x02320000},
	{DRV_IICDATA,0x0234001F},
	{DRV_IICDATA,0x02360000},
	{DRV_IICDATA,0x02380001},
	{DRV_IICDATA,0x023A0000},
	{DRV_IICDATA,0x023C0001},
	{DRV_IICDATA,0x023E0001},
	{DRV_IICDATA,0x02040001},
	{DRV_IICDATA,0x02060000},
	//**************************************************-->
	//TC358768XBGDSI-TXTimingControl-->
	//**************************************************-->
	{DRV_IICDATA,0x06200001},
	{DRV_IICDATA,0x0622000C},
	{DRV_IICDATA,0x06240004},
	{DRV_IICDATA,0x06260500},
	{DRV_IICDATA,0x062800E9},
	{DRV_IICDATA,0x062A0053},
	{DRV_IICDATA,0x062C0280},
	{DRV_IICDATA,0x05180001},
	{DRV_IICDATA,0x051A0000},
	//**************************************************-->
	//LCDD(Peripheral)Setting-->
	//**************************************************-->
	{DRV_IICDATA,0x06021013},
	{DRV_IICDATA,0x06040000},
	{DRV_IICDATA,0x06100001},
	{DRV_IICDATA,0x06000001},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x06024029},
	{DRV_IICDATA,0x06040003},
	{DRV_IICDATA,0x06105AF0},
	{DRV_IICDATA,0x06120059},
	{DRV_IICDATA,0x06000001},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x06024029},
	{DRV_IICDATA,0x06040003},
	{DRV_IICDATA,0x0610A5F1},
	{DRV_IICDATA,0x061200A6},
	{DRV_IICDATA,0x06000001},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x00080001},
	{DRV_IICDATA,0x00500029},
	{DRV_IICDATA,0x00220017},
	{DRV_IICDATA,0x00E08000},
	{DRV_IICDATA,0x00E800B4},
	{DRV_IICDATA,0x00E80504},
	{DRV_IICDATA,0x00E81E00},
	{DRV_IICDATA,0x00E8011F},
	{DRV_IICDATA,0x00E81110},
	{DRV_IICDATA,0x00E81312},
	{DRV_IICDATA,0x00E80D0C},
	{DRV_IICDATA,0x00E80F0E},
	{DRV_IICDATA,0x00E80303},
	{DRV_IICDATA,0x00E80303},
	{DRV_IICDATA,0x00E80303},
	{DRV_IICDATA,0x00E80003},
	{DRV_IICDATA,0x00E0E000},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x00E02000},
	{DRV_IICDATA,0x00E00000},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x00080001},
	{DRV_IICDATA,0x00500029},
	{DRV_IICDATA,0x00220010},
	{DRV_IICDATA,0x00E08000},
	{DRV_IICDATA,0x00E8C6B0},
	{DRV_IICDATA,0x00E8BBBB},
	{DRV_IICDATA,0x00E833BB},
	{DRV_IICDATA,0x00E83333},
	{DRV_IICDATA,0x00E82433},
	{DRV_IICDATA,0x00E80000},
	{DRV_IICDATA,0x00E8009C},
	{DRV_IICDATA,0x00E88000},
	{DRV_IICDATA,0x00E0E000},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x00E02000},
	{DRV_IICDATA,0x00E00000},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x00080001},
	{DRV_IICDATA,0x00500029},
	{DRV_IICDATA,0x0022000C},
	{DRV_IICDATA,0x00E08000},
	{DRV_IICDATA,0x00E853B1},
	{DRV_IICDATA,0x00E800A0},
	{DRV_IICDATA,0x00E80485},
	{DRV_IICDATA,0x00E80000},
	{DRV_IICDATA,0x00E8007C},
	{DRV_IICDATA,0x00E85F00},
	{DRV_IICDATA,0x00E0E000},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x00E02000},
	{DRV_IICDATA,0x00E00000},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x00080001},
	{DRV_IICDATA,0x00500029},
	{DRV_IICDATA,0x0022000B},
	{DRV_IICDATA,0x00E08000},
	{DRV_IICDATA,0x00E807B2},
	{DRV_IICDATA,0x00E80809},
	{DRV_IICDATA,0x00E8088B},
	{DRV_IICDATA,0x00E82200},
	{DRV_IICDATA,0x00E84400},
	{DRV_IICDATA,0x00E800D9},
	{DRV_IICDATA,0x00E0E000},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x00E02000},
	{DRV_IICDATA,0x00E00000},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x06024029},
	{DRV_IICDATA,0x06040003},
	{DRV_IICDATA,0x06104AB6},
	{DRV_IICDATA,0x0612004A},
	{DRV_IICDATA,0x06000001},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x00080001},
	{DRV_IICDATA,0x00500029},
	{DRV_IICDATA,0x00220013},
	{DRV_IICDATA,0x00E08000},
	{DRV_IICDATA,0x00E801B7},
	{DRV_IICDATA,0x00E80901},
	{DRV_IICDATA,0x00E8110D},
	{DRV_IICDATA,0x00E81D19},
	{DRV_IICDATA,0x00E80015},
	{DRV_IICDATA,0x00E82125},
	{DRV_IICDATA,0x00E80000},
	{DRV_IICDATA,0x00E80000},
	{DRV_IICDATA,0x00E8F702},
	{DRV_IICDATA,0x00E80038},
	{DRV_IICDATA,0x00E0E000},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x00E02000},
	{DRV_IICDATA,0x00E00000},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x06024029},
	{DRV_IICDATA,0x06040005},
	{DRV_IICDATA,0x0610B8B8},
	{DRV_IICDATA,0x06120252},
	{DRV_IICDATA,0x061400CC},
	{DRV_IICDATA,0x06000001},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x06024029},
	{DRV_IICDATA,0x06040003},
	{DRV_IICDATA,0x061027BA},
	{DRV_IICDATA,0x061200D3},
	{DRV_IICDATA,0x06000001},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x06024029},
	{DRV_IICDATA,0x06040008},
	{DRV_IICDATA,0x061043BD},
	{DRV_IICDATA,0x06120E0E},
	{DRV_IICDATA,0x06144B4B},
	{DRV_IICDATA,0x06161432},
	{DRV_IICDATA,0x06000001},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x00080001},
	{DRV_IICDATA,0x00500029},
	{DRV_IICDATA,0x00220009},
	{DRV_IICDATA,0x00E08000},
	{DRV_IICDATA,0x00E800C1},
	{DRV_IICDATA,0x00E80E0F},
	{DRV_IICDATA,0x00E80001},
	{DRV_IICDATA,0x00E83A36},
	{DRV_IICDATA,0x00E80008},
	{DRV_IICDATA,0x00E0E000},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x00E02000},
	{DRV_IICDATA,0x00E00000},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x06024029},
	{DRV_IICDATA,0x06040003},
	{DRV_IICDATA,0x061022C2},
	{DRV_IICDATA,0x06120080},
	{DRV_IICDATA,0x06000001},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x06024029},
	{DRV_IICDATA,0x06040003},
	{DRV_IICDATA,0x061022C3},
	{DRV_IICDATA,0x06120031},
	{DRV_IICDATA,0x06000001},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x00080001},
	{DRV_IICDATA,0x00500029},
	{DRV_IICDATA,0x00220009},
	{DRV_IICDATA,0x00E08000},
	{DRV_IICDATA,0x00E800C6},
	{DRV_IICDATA,0x00E8FF00},
	{DRV_IICDATA,0x00E80000},
	{DRV_IICDATA,0x00E800FF},
	{DRV_IICDATA,0x00E80000},
	{DRV_IICDATA,0x00E0E000},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x00E02000},
	{DRV_IICDATA,0x00E00000},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x00080001},
	{DRV_IICDATA,0x00500029},
	{DRV_IICDATA,0x00220027},
	{DRV_IICDATA,0x00E08000},
	{DRV_IICDATA,0x00E87CC8},
	{DRV_IICDATA,0x00E85C69},
	{DRV_IICDATA,0x00E84E50},
	{DRV_IICDATA,0x00E84540},
	{DRV_IICDATA,0x00E8472F},
	{DRV_IICDATA,0x00E84545},
	{DRV_IICDATA,0x00E85262},
	{DRV_IICDATA,0x00E84E5C},
	{DRV_IICDATA,0x00E83F4D},
	{DRV_IICDATA,0x00E8062D},
	{DRV_IICDATA,0x00E8697C},
	{DRV_IICDATA,0x00E8505C},
	{DRV_IICDATA,0x00E8404E},
	{DRV_IICDATA,0x00E82F45},
	{DRV_IICDATA,0x00E84547},
	{DRV_IICDATA,0x00E86245},
	{DRV_IICDATA,0x00E85C52},
	{DRV_IICDATA,0x00E84D4E},
	{DRV_IICDATA,0x00E82D3F},
	{DRV_IICDATA,0x00E80006},
	{DRV_IICDATA,0x00E80000},
	{DRV_IICDATA,0x00E0E000},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x00E02000},
	{DRV_IICDATA,0x00E00000},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x06024029},
	{DRV_IICDATA,0x06040004},
	{DRV_IICDATA,0x061007D0},
	{DRV_IICDATA,0x0612FFFF},
	{DRV_IICDATA,0x06000001},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x06024029},
	{DRV_IICDATA,0x06040005},
	{DRV_IICDATA,0x061063D2},
	{DRV_IICDATA,0x0612080B},
	{DRV_IICDATA,0x06140088},
	{DRV_IICDATA,0x06000001},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x06024029},
	{DRV_IICDATA,0x06040007},
	{DRV_IICDATA,0x061000D4},
	{DRV_IICDATA,0x06120000},
	{DRV_IICDATA,0x06140432},
	{DRV_IICDATA,0x06160051},
	{DRV_IICDATA,0x06000001},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x06024029},
	{DRV_IICDATA,0x06040003},
	{DRV_IICDATA,0x06105AF1},
	{DRV_IICDATA,0x06120059},
	{DRV_IICDATA,0x06000001},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x06021013},
	{DRV_IICDATA,0x06040000},
	{DRV_IICDATA,0x06100011},
	{DRV_IICDATA,0x06000001},
	{DRV_DELAYMS,1},
	{DRV_IICDATA,0x06021013},
	{DRV_IICDATA,0x06040000},
	{DRV_IICDATA,0x06100029},
	{DRV_IICDATA,0x06000001},
	{DRV_DELAYMS,1},
	//**************************************************-->
	//SettoHSmode-->
	//**************************************************-->
	{DRV_IICDATA,0x05000086},
	{DRV_IICDATA,0x0502A300},
	{DRV_IICDATA,0x05008000},
	{DRV_IICDATA,0x0502C300},
	//**************************************************-->
	//Host:RGB(DPI)inputstart-->
	//**************************************************-->
	{DRV_IICDATA,0x00080057},
	{DRV_IICDATA,0x0050000E},
	{DRV_IICDATA,0x00320000},
	{DRV_IICDATA,0x00040144},
	//**************************************************-->
	//LCDD(Peripheral)Setting-->
	//**************************************************-->
		***/
};

// 64+320+80
const tLCD_PARAM tMode[] =
{
    /***********       RGB Dummy-320 MODE         *************/
    {
        #if 1//37.5Mhz
		// tPANEL_PARAM
        {
            /* Old prototype */
            PINMUX_LCDMODE_RGB_PARALL565,//       //!< LCDMode
            37.5f,//37.5f,                         //!< fDCLK 24.54f,
            484,//484,                           //!< uiHSyncTotalPeriod
            PANEL_HEIGHT,//320,                           //!< uiHSyncActivePeriod
            100,                           //!< uiHSyncBackPorch
            32 + PANEL_WIDTH, //1312                         //!< uiVSyncTotalPeriod
            PANEL_WIDTH,//1280                          //!< uiVSyncActivePeriod
            12,//                        //!< uiVSyncBackPorchOdd  29
            12,//                            //!< uiVSyncBackPorchEven 29.5
            PANEL_HEIGHT,                            //!< uiBufferWidth
            PANEL_WIDTH,                            //!< uiWindowHeight
            PANEL_HEIGHT,                            //!< uiWindowWidth
            PANEL_WIDTH,                            //!< uiWindowHeight
            FALSE,                          //!< bYCbCrFormat

            /* New added parameters */
            HSYNCT,                           //!< uiHSyncSYNCwidth
            VSYNCT,                           //!< uiVSyncSYNCwidth
        },
		#else//33.333332Mhz
        // tPANEL_PARAM
        {
            /* Old prototype */
            PINMUX_LCDMODE_RGB_PARALL565,//       //!< LCDMode
            34.0f,//33.0f,                         //!< fDCLK 24.54f,
            (112+PANEL_HEIGHT),//432,                           //!< uiHSyncTotalPeriod
            PANEL_HEIGHT,//320,                           //!< uiHSyncActivePeriod
            48,                           //!< uiHSyncBackPorch
            32 + PANEL_WIDTH, //1312                         //!< uiVSyncTotalPeriod
            PANEL_WIDTH,//1280                          //!< uiVSyncActivePeriod
            12,//                        //!< uiVSyncBackPorchOdd  29
            12,//                            //!< uiVSyncBackPorchEven 29.5
            PANEL_HEIGHT,                            //!< uiBufferWidth
            PANEL_WIDTH,                            //!< uiWindowHeight
            PANEL_HEIGHT,                            //!< uiWindowWidth
            PANEL_WIDTH,                            //!< uiWindowHeight
            FALSE,                          //!< bYCbCrFormat

            /* New added parameters */
            HSYNCT,                           //!< uiHSyncSYNCwidth
            VSYNCT,                           //!< uiVSyncSYNCwidth
        },
        #endif
		
        // tIDE_PARAM
        {
            /* Old prototype */
            PINMUX_LCD_SEL_PARALLE_RGB565|PINMUX_LCD_SEL_DE_ENABLE,//PINMUX_LCD_SEL_SERIAL_RGB_8BITS,//!< pinmux_select_lcd;
            ICST_CCIR601,                   //!< icst;
                  //      {FALSE,FALSE},                  //!< dithering[2];

            {TRUE,FALSE},                  //!< dithering[2];
            DISPLAY_DEVICE_AU,              //!< **DONT-CARE**
            IDE_PDIR_RGB,                   //!< pdir;
            IDE_LCD_G,                      //!< odd;
            IDE_LCD_G,                      //!< even;
            TRUE,                           //!< hsinv;
            TRUE,                           //!< vsinv;
            FALSE,                          //!< hvldinv;
            FALSE,                          //!< vvldinv;
            FALSE,                           //!< clkinv;
            FALSE,                          //!< fieldinv;
            FALSE,                          //!< **DONT-CARE**
            FALSE,                          //!< interlace;
            FALSE,                          //!< **DONT-CARE**
            0x40,                           //!< ctrst;
            0x00,                           //!< brt;
            0x58,                           //!< cmults;
            FALSE,                          //!< cex;
            FALSE,                          //!< **DONT-CARE**
            TRUE,                           //!< **DONT-CARE**
            TRUE,                           //!< tv_powerdown;
            {0x00,0x00},                    //!< **DONT-CARE**

            /* New added parameters */
            FALSE,                          //!< YCex
            FALSE,                          //!< HLPF
            {FALSE,FALSE,FALSE},            //!< subpixOdd[3]
            {FALSE,FALSE,FALSE},            //!< subpixEven[3]
            {IDE_DITHER_6BITS,IDE_DITHER_6BITS,IDE_DITHER_6BITS}//!< DitherBits[3]
        },

        (tPANEL_CMD*)tCmdModeRgbd320,                  //!< pCmdQueue
        sizeof(tCmdModeRgbd320)/sizeof(tPANEL_CMD),    //!< nCmd
    }
};
const tPANEL_CMD tCmdRotate_None[] =
{
};
const tPANEL_CMD tCmdRotate_180[] =
{
};
const tLCD_ROT tRotate[] =
{
    {DISPDEV_LCD_ROTATE_NONE,(tPANEL_CMD*)tCmdRotate_None,0},
    {DISPDEV_LCD_ROTATE_180,(tPANEL_CMD*)tCmdRotate_180,0}
};
//@}

tLCD_ROT* dispdev_getLcdRotateCmdParal(UINT32 *ModeNumber)
{
    if(tRotate != NULL)
    {
        *ModeNumber = sizeof(tRotate)/sizeof(tLCD_ROT);
    }
    else
    {
        *ModeNumber = 0;
    }
    return (tLCD_ROT*)tRotate;
}

tLCD_PARAM* dispdev_getConfigModeParal(UINT32 *ModeNumber)
{
    *ModeNumber = sizeof(tMode)/sizeof(tLCD_PARAM);
    return (tLCD_PARAM*)tMode;
}

tPANEL_CMD* dispdev_getStandbyCmdParal(UINT32 *CmdNumber)
{
    *CmdNumber = sizeof(tCmdStandby)/sizeof(tPANEL_CMD);
    return (tPANEL_CMD*)tCmdStandby;
}

#if 1
void dispdev_writeToLcdSifParal(DISPDEV_IOCTRL pDispDevControl, UINT32 addr, UINT32 value)
{
    UINT32                  uiSIFData;
    DISPDEV_IOCTRL_PARAM    DevIOCtrl;

    uiSIFData = ((((UINT32)addr << 8)|((UINT32)value)) << 16);

    pDispDevControl(DISPDEV_IOCTRL_GET_REG_IF, &DevIOCtrl);
    sif_send(DevIOCtrl.SEL.GET_REG_IF.uiSifCh, uiSIFData, 0, 0);
}

typedef struct
{
    I2C_SESSION     I2C_Channel;
    I2C_PINMUX      I2C_PinMux;
    I2C_BUS_CLOCK   I2C_BusClock;
    UINT32          I2C_Slave_WAddr;
    UINT32          I2C_Slave_RAddr;
} PA2DSI_INFO, *PPA2DSI_INFO;

static PA2DSI_INFO          g_PA2DSIInfo;

static BOOL PA2DSI_I2C_Init(PA2DSI_INFO I2CInfo)
{
    UINT erReturn;
    g_PA2DSIInfo.I2C_PinMux = I2CInfo.I2C_PinMux;
    g_PA2DSIInfo.I2C_BusClock = I2CInfo.I2C_BusClock;
    g_PA2DSIInfo.I2C_Slave_WAddr = I2CInfo.I2C_Slave_WAddr;
    g_PA2DSIInfo.I2C_Slave_RAddr = I2CInfo.I2C_Slave_RAddr;

    if (g_PA2DSIInfo.I2C_PinMux > I2C_PINMUX_2ND)
    {
        DBG_ERR("I2C3 pin mux err!\r\n");
        return FALSE;
    }

    if (g_PA2DSIInfo.I2C_BusClock > I2C_BUS_CLOCK_1MHZ)
    {
        DBG_ERR("I2C3 bus clock err!\r\n");
        return FALSE;
    }

    erReturn = i2c3_open(&(g_PA2DSIInfo.I2C_Channel));
    if(erReturn != I2C_STS_OK)
    {
        DBG_ERR("Open I2C3 driver err!\r\n");
        return FALSE;
    }
    return TRUE;
}

static UINT32 PA2DSI_i2c_write(UINT32 uiRegData)
{
    I2C_DATA    I2cData;
    I2C_BYTE    I2cByte[16];
    I2C_STS     ret;
	static UINT32 tempData = 0;
    if (i2c3_lock(g_PA2DSIInfo.I2C_Channel) != E_OK)
    {
        DBG_ERR("I2C lock fail!\r\n");
        return 0;
    }
    I2cData.VersionInfo = DRV_VER_96660;
    I2cData.ByteCount = I2C_BYTE_CNT_5;
    I2cData.pByte = I2cByte;
    I2cByte[0].uiValue = g_PA2DSIInfo.I2C_Slave_WAddr;
    I2cByte[0].Param = I2C_BYTE_PARAM_START;
	tempData = uiRegData;
    I2cByte[1].uiValue = ((tempData & 0xff000000) >> 24); //
    I2cByte[1].Param = I2C_BYTE_PARAM_NONE;
	tempData = uiRegData;
    I2cByte[2].uiValue = ((tempData & 0x00ff0000) >> 16); //
	tempData = uiRegData;
    I2cByte[2].Param = I2C_BYTE_PARAM_NONE;
	tempData = uiRegData;
    I2cByte[3].uiValue = ((tempData & 0x0000ff00) >> 8);//tempData & 0x000000ff; //
    I2cByte[3].Param = I2C_BYTE_PARAM_NONE;
	tempData = uiRegData;
    I2cByte[4].uiValue = tempData & 0x000000ff;//((tempData & 0x0000ff00) >> 8);
    I2cByte[4].Param = I2C_BYTE_PARAM_STOP;
    
    
   /* I2cByte[3].uiValue = uiRegData & 0x000000ff;
    I2cByte[3].Param = I2C_BYTE_PARAM_NONE;
	I2cByte[4].uiValue = ((uiRegData & 0x0000ff00) >> 8); //
    I2cByte[4].Param = I2C_BYTE_PARAM_STOP;
    */
    ret = i2c3_transmit(&I2cData);
    if (ret != I2C_STS_OK)
    {
        DBG_ERR("(W)i2c3 ret = %02x!!\r\n", ret);
		i2c3_unlock(g_PA2DSIInfo.I2C_Channel);
		return -1;
    }
    if (i2c3_unlock(g_PA2DSIInfo.I2C_Channel) != E_OK)
    {
        DBG_ERR("I2C3 unlock fail!\r\n");
        return -1;
    }

    return 0;
}

#if 1
static UINT32 PA2DSI_i2c_read_reg(UINT32 uiAddr, UINT32 *RecvBuf, UINT32 ReadNum)
{
    I2C_DATA    I2cData;
    I2C_BYTE    I2cByte[16];
    I2C_STS     ret;
    UINT32       i;
    if (i2c3_lock(g_PA2DSIInfo.I2C_Channel) != E_OK)
    {
        DBG_ERR("I2C lock fail!\r\n");
        return 0;
    }
    I2cData.VersionInfo = DRV_VER_96660;
    I2cData.ByteCount = I2C_BYTE_CNT_4;
    I2cData.pByte = I2cByte;
    I2cByte[0].uiValue = g_PA2DSIInfo.I2C_Slave_WAddr;
    I2cByte[0].Param = I2C_BYTE_PARAM_START;
    I2cByte[1].uiValue = ((uiAddr & 0x0000ff00) >> 8); //MSB
    I2cByte[1].Param = I2C_BYTE_PARAM_NONE;
	I2cByte[2].uiValue = (uiAddr & 0x000000ff);//LSB
    I2cByte[2].Param = I2C_BYTE_PARAM_NONE;
    I2cByte[3].uiValue = g_PA2DSIInfo.I2C_Slave_RAddr;
    I2cByte[3].Param = I2C_BYTE_PARAM_START;
    ret = i2c3_transmit(&I2cData);
    if (ret != I2C_STS_OK)
    {
        DBG_ERR("(W)i2c3 ret = %02x!!\r\n", ret);
		i2c3_unlock(g_PA2DSIInfo.I2C_Channel);
		return -1;
    }
    I2cData.VersionInfo = DRV_VER_96660;
    I2cData.ByteCount = ReadNum;
    I2cData.pByte = I2cByte;
	if(ReadNum == 1)
	{
      I2cByte[0].Param = I2C_BYTE_PARAM_NONE | I2C_BYTE_PARAM_NACK | I2C_BYTE_PARAM_STOP;
	}
	else if(ReadNum > 1)
	{
       for(i = 0; i < (ReadNum - 1); i ++)
       {
		  I2cByte[i].Param = I2C_BYTE_PARAM_NONE | I2C_BYTE_PARAM_ACK;
       }
	   I2cByte[ReadNum - 1].Param = I2C_BYTE_PARAM_NONE | I2C_BYTE_PARAM_NACK | I2C_BYTE_PARAM_STOP;
	}
	else
	{
        DBG_ERR("ReadNum should > 0!!\r\n");
	}
	ret = i2c3_receive(&I2cData);
	if ( ret != I2C_STS_OK )
    {
        DBG_ERR("(R)i2c3 ret = %02x!!\r\n", ret);
    }
    if (i2c3_unlock(g_PA2DSIInfo.I2C_Channel) != E_OK)
    {
        DBG_ERR("I2C3 unlock fail!\r\n");
        return -1;
    }

    for(i = 0; i < ReadNum; i ++)
    {
        RecvBuf[i] = I2cByte[i].uiValue;
    }
	/*
	debug_msg("^GLiwk--Read From Reg %02x:",uiAddr);
    for(i = 0; i < ReadNum; i ++)
    {
        debug_msg("%02x ", RecvBuf[i]);
    }
	debug_msg("\r\n");
    *pX = ((I2CReadBuffer[2] << 8) | I2CReadBuffer[1]);*/
    return 0;
}
#endif

static BOOL g_bPa2DsiI2cInit = FALSE;

void dispdev_writeToLcdGpioParal(DISPDEV_IOCTRL pDispDevControl, UINT32 addr, UINT32 value)
{
    #if 0  // GPIO ?¡ê?aSPI?¨®?¨²
    //UINT32                  uiSIFData,j,i;
    UINT32                  j;
    UINT32                  SifClk,SifSen,SifData;
    DISPDEV_IOCTRL_PARAM    DevIOCtrl;

    pDispDevControl(DISPDEV_IOCTRL_GET_REG_IF, &DevIOCtrl);
    SifClk  = DevIOCtrl.SEL.GET_REG_IF.uiGpioClk;
    SifSen  = DevIOCtrl.SEL.GET_REG_IF.uiGpioSen;
    SifData = DevIOCtrl.SEL.GET_REG_IF.uiGpioData;

    gpio_setDir(SifSen, GPIO_DIR_OUTPUT);
    gpio_setDir(SifClk, GPIO_DIR_OUTPUT);
    gpio_setDir(SifData, GPIO_DIR_OUTPUT);

    gpio_setPin(SifSen);
    gpio_setPin(SifData);
    gpio_setPin(SifClk);

    //uiSIFData = (UINT32)addr << 8 | (UINT32)value;
    //debug_msg("Has not implement dispdev_readFromLcdGpio%x %x  %x\r\n",uiSIFData,addr,value);
    Delay_DelayUs(500);

    gpio_clearPin(SifSen);

	if (addr == IFCMD_CMD) {
        value = (value & 0x00ff);
		for (j = 9; j > 0; j--) {
			if (((value >> (j - 1)) & 0x01)) {
				gpio_setPin(SifData);
			} else {
				gpio_clearPin(SifData);
			}

			Delay_DelayUs(100);
			gpio_clearPin(SifClk);
			Delay_DelayUs(200);
			gpio_setPin(SifClk);
			Delay_DelayUs(100);
		}
	} else if (addr == IFCMD_DATA) {
		value = ((value & 0x00ff) | 0x100);
		for (j = 9; j > 0; j--) {
			if (((value >> (j - 1)) & 0x01)) {
				gpio_setPin(SifData);
			} else {
				gpio_clearPin(SifData);
			}

			Delay_DelayUs(100);
			gpio_clearPin(SifClk);
			Delay_DelayUs(200);
			gpio_setPin(SifClk);
			Delay_DelayUs(100);
		}
	}

    Delay_DelayUs(500);
    gpio_setPin(SifSen);
   #else
   if(g_bPa2DsiI2cInit == FALSE)
   {
	   gpio_clearPin(P_GPIO_9);
	   gpio_clearPin(P_GPIO_17);
	   Delay_DelayMs(5);
	   gpio_setPin(P_GPIO_9);
	   gpio_setPin(P_GPIO_17);
	   Delay_DelayMs(1);
     PA2DSI_INFO PA2DSIInfo;
     PA2DSIInfo.I2C_PinMux = I2C_PINMUX_1ST;
     PA2DSIInfo.I2C_BusClock = I2C_BUS_CLOCK_200KHZ;
     PA2DSIInfo.I2C_Slave_WAddr = 0x1C;
     PA2DSIInfo.I2C_Slave_RAddr = 0x1D;
     if(PA2DSI_I2C_Init(PA2DSIInfo))
     {
        debug_msg("^GLiwk--------- PA2DSI Initial Success!!\r\n");
        g_bPa2DsiI2cInit = TRUE;
		gpio_clearPin(P_GPIO_9);
		gpio_clearPin(P_GPIO_17);
		Delay_DelayMs(5);
		gpio_setPin(P_GPIO_9);
		gpio_setPin(P_GPIO_17);
		Delay_DelayMs(1);
     }
   }
   if(g_bPa2DsiI2cInit)
   {
     if(addr == DRV_IICDATA)
     {
        PA2DSI_i2c_write(value);
     }
     else if(addr == DRV_DELAYMS)
     {
        Delay_DelayMs(value);
     }
	 if(value == 0x00040144)
	 {
	   UINT32 readBuffer[4];
	   PA2DSI_i2c_read_reg(0x0004,&readBuffer[0],2);	   
	 }
   }   
   #endif
}


void dispdev_readFromLcdGpioParal(DISPDEV_IOCTRL pDispDevControl, UINT32 addr, UINT32* p_value)
{
    //debug_err(("Has not implement dispdev_readFromLcdGpio \r\n"));
}
#endif
