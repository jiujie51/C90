/*
    Copyright   Novatek Microelectronics Corp. 2005~2014.  All rights reserved.

    @file       ImageUnit_BsMux.h
    @ingroup    mBsMux

    @note       Nothing.

    @date       2017/10/01
*/

#ifndef IMAGEUNIT_BSMUX_H
#define IMAGEUNIT_BSMUX_H

#include "ImageStream.h"
#include "NvtMediaInterface.h"
#include "NMediaRecBsMuxer.h"
#include "MediaWriteLib.h"

enum {
	BSMUX_PARAM_START    = 0x0001F000,
	BSMUX_PARAM_EVENT_CB = BSMUX_PARAM_START,
	BSMUX_PARAM_GPS_DATA,
	BSMUX_PARAM_FILE_TRIGEMR,
	BSMUX_PARAM_USER_DATA,
	BSMUX_PARAM_CRYPTO_AES,
	BSMUX_PARAM_CRYPTO_POS, //0:iframe 1:container 2:all
	BSMUX_PARAM_CRYPTO_MODE,
	BSMUX_PARAM_CUSTOM_DATA,
	BSMUX_PARAM_BYPASS_MODE,
};

enum {
	BSMUX_FUNC_START = 0,
	BSMUX_FUNC_BYPASS_NORMAL = BSMUX_FUNC_START,
	BSMUX_FUNC_BYPASS_JPG,
	BSMUX_FUNC_BYPASS_NONE,
	BSMUX_FUNC_END,
};

typedef void (IsfBsMuxEventCb)(CHAR *Name, UINT32 event_id, UINT32 value);

extern ISF_UNIT ISF_BsMux;

#endif //IMAGEUNIT_BSMUX_H

