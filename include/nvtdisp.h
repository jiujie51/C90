#ifndef _NVTDISP_H
#define _NVTDISP_H
/*
    Copyright (c) 2018~  Novatek Microelectronics Corporation

    @file nvtdisp.h

    @version

    @date
*/

#include <nvt_type.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
    Memory structure.
*/
typedef struct _DispMemSetting{
    UINT32  addr;		///< buffer addr
    UINT32  size;		///< buffer size
} DispMemSetting;

/**
    Rectange structure.
*/
typedef struct {
	INT32  w;       ///< Rectangle width
	INT32  h;       ///< Rectangle height
} ISIZE, *PISIZE;


/**
    NVTDISP Layer Buffer Format

    This is used for nvtdisp_setlayerbufferformat to specify the buffer format.
    This feature can be assigned for each display layer with different setting.
*/

typedef enum _NVTDISP_BUFFORMAT {
	NVTDISP_BUFFORMAT_PAL1,                 ///< Buffer uses format 1bit palette.
	NVTDISP_BUFFORMAT_PAL2,                 ///< Buffer uses format 2bit palette.
	NVTDISP_BUFFORMAT_PAL4,                 ///< Buffer uses format 4bit palette.
	NVTDISP_BUFFORMAT_PAL8,                 ///< Buffer uses format 8bit palette.

	NVTDISP_BUFFORMAT_YUV444,               ///< Buffer uses format YUV444.
	NVTDISP_BUFFORMAT_YUV422,               ///< Buffer uses format YUV422.
	NVTDISP_BUFFORMAT_YUV420,               ///< Buffer uses format YUV420.

	NVTDISP_BUFFORMAT_ARGB4565,             ///< Buffer uses format ARGB 4565.
	NVTDISP_BUFFORMAT_ARGB8565,             ///< Buffer uses format ARGB 8565.

	NVTDISP_BUFFORMAT_YUV422PACK,           ///< Buffer uses format YUV422.
	NVTDISP_BUFFORMAT_YUV420PACK,           ///< Buffer uses format YUV420.

	NVTDISP_BUFFORMAT_ARGB8888,             ///< Buffer uses format ARGB 8888.

	NVTDISP_BUFFORMAT_ARGB4444,             ///< Buffer uses format ARGB 4444.
	NVTDISP_BUFFORMAT_ARGB1555,             ///< Buffer uses format ARGB 1555.

	ENUM_DUMMY4WORD(NVTDISP_BUFFORMAT)
} NVTDISP_BUFFORMAT;

/////////////////////////////////////////////////////////////////////////////////

/**
     @name NVTDISP device id
*/
//@{
#define NVTDISP_DOUT1                  0x00 ///< 1st device
#define NVTDISP_DOUT2                  0x10 ///< 2nd device
#if defined(_BSP_NA51000_)
#define NVTDISP_DOUT_NUM               2 	///< total device
#else
#define NVTDISP_DOUT_NUM               1 	///< total device
#endif
//@}

/////////////////////////////////////////////////////////////////////////////////

/**
     @name NVTDISP layer id
*/
//@{
#define NVTDISP_LYR_OSD1             0x00 ///< OSD 1
#define NVTDISP_LYR_VDO1             0x02 ///< video 1
#define NVTDISP_LYR_VDO2             0x03 ///< video 2
#define NVTDISP_LYR_MASK             0x0D
//@}


/**
     @state param for get/set layer
*/
//@{
#define NVTDISP_LYR_ENABLE           0x00 ///< enable/disable layer, value = 1/0
#define NVTDISP_LYR_COMPARE          0x01 ///< color compare,func = NVTDISP_COMPARE_KEY_NEVER/EQUAL/LESS/GREATER
#define NVTDISP_LYR_COLORKEY         0x02 ///< do color key,value = YUV color
#define NVTDISP_LYR_BLEND            0x03 ///< blend ctrl,func = NVTDISP_BLEND_DISABLE/CONSTALPHA/PIXELALPHA/R1
#define NVTDISP_LYR_ALPHA            0x04 ///< const alpha,value = 0x00~0xFF (NVTDISP_BLEND_OPAQUE_000~NVTDISP_BLEND_OPAQUE_100)
#define NVTDISP_LYR_FLIP             0x05 ///< Set Buffer Output Order such as Flip / 180 Rotation
//@}


/**
     @value of NVTDISP_LYR_COMPARE
*/
//@{
#define NVTDISP_COMPARE_KEY_NEVER         0 ///< never key
#define NVTDISP_COMPARE_KEY_LESS          1 ///< key if pixel color is less then color key
#define NVTDISP_COMPARE_KEY_EQUAL         2 ///< key if pixel color is equal to color key
#define NVTDISP_COMPARE_KEY_GREATER       3 ///< key if pixel color is grater then color key
//@}

/**
     @value of NVTDISP_LYR_BLEND
*/
//@{
#define NVTDISP_BLEND_DISABLE          0x00 ///< blending disable
#define NVTDISP_BLEND_CONSTALPHA       0x01 ///< blending by layer const alpha
#define NVTDISP_BLEND_PIXELALPHA       0x02 ///< blending by layer pixel alpha
#define NVTDISP_BLEND_R1               0x03 ///< (reserved)
//@}

/**
     @value of NVTDISP_LYR_ALPHA
*/
//@{
#define NVTDISP_BLEND_OPAQUE_000       0x00 //Output = Under_Layer
#define NVTDISP_BLEND_OPAQUE_012       0x20 //Output = This_Layer * 12% + Under_Layer * 88%
#define NVTDISP_BLEND_OPAQUE_025       0x40 //Output = This_Layer * 25% + Under_Layer * 75%
#define NVTDISP_BLEND_OPAQUE_050       0x80 //Output = This_Layer * 50% + Under_Layer * 50%
#define NVTDISP_BLEND_OPAQUE_075       0xC0 //Output = This_Layer * 75% + Under_Layer * 25%
#define NVTDISP_BLEND_OPAQUE_088       0xE0 //Output = This_Layer * 88% + Under_Layer * 12%
#define NVTDISP_BLEND_OPAQUE_100       0xff //Output = This_Layer
//@}


/**
     @value of NVTDISP_LYR_FLIP
*/
//@{
#define NVTDISP_FLIP_NORMAL       		0x0 ///< Normal output direction
#define NVTDISP_FLIP_HRZ       			0x1 ///< Horizontal flip
#define NVTDISP_FLIP_VTC       			0x2 ///< Vertical flip
#define NVTDISP_FLIP_180       			0x3 ///< Rotate 180 degree
//@}


/**
     Get continue memory from uItron OSD memory pool.
     OSD pool size is config by PrjCfg_xxxx.h
     ex:

#define DISPLAY_OSD_W                   320
#define DISPLAY_OSD_H                   240
#define DISPLAY_OSD_FMT                 DISP_PXLFMT_ARGB8888_PK

     @note Nothing

     @param[out] pDispBufSetting        return buffer addr and size
     @return
         - @b ER_SUCCESS:   Successed
         - @b others:  Failed
*/
extern NVTRET nvtdisp_memalloc(UINT32 uiDevID, UINT32 uiLyrID, DispMemSetting *pDispBufSetting);


/**
     Initialize device.

     Initialize device.
     @note Nothing

     @param[in] id           device id (ex: NVTDISP_DOUT1 or NVTDISP_DOUT2).
     @return
         - @b ER_SUCCESS:   Successed
         - @b others:  Failed
*/
extern INT32  nvtdisp_init(UINT32 id);

/**
     Uninitialize device.

     Uninitialize device.It would disable all layer
     @note Nothing

     @param[in] id           device id (ex: NVTDISP_DOUT1 or NVTDISP_DOUT2).
     @return
         - @b ER_SUCCESS:   Successed
         - @b others:  Failed
*/
extern INT32  nvtdisp_uninit(UINT32 id);

/**
     Get device size of by current opened device mode.

     Get device size of by current opened device mode.
     @note Nothing

     @param[in] id           device id (ex: NVTDISP_DOUT1 or NVTDISP_DOUT2).
     @param[out] pDeviceSize device size.
     @return
         - @b ER_SUCCESS:   Successed
         - @b others:  Failed
*/
extern INT32  nvtdisp_getdevicesize(UINT32 id,ISIZE *pDeviceSize);

/**
     Set control to layer.

     Set control to layer.
     @note Nothing

     @param[in] id           device id (ex: NVTDISP_DOUT1 or NVTDISP_DOUT2).
     @param[in] layer        layer id (ex: NVTDISP_LYR_OSD1/OSD2/VDO1/VDO2).
     @param[in] param        control state (ex:NVTDISP_LYR_XXX)
     @param[in] value        control value (depend on control state)
     @return
         - @b ER_SUCCESS:   Successed
         - @b others:  Failed
*/
extern INT32  nvtdisp_setlayer(UINT32 id, UINT32 layer, UINT32 param, UINT32 value);

/**
     Get control to layer.

     Get control to layer.
     @note Nothing

     @param[in] id           device id (ex: NVTDISP_DOUT1 or NVTDISP_DOUT2).
     @param[in] layer        layer id (ex: NVTDISP_LYR_OSD1/OSD2/VDO1/VDO2).
     @param[in] param        control state (ex:NVTDISP_LYR_XXX)
     @param[out] value       control value (depend on control state)
     @return
         - @b ER_SUCCESS:   Successed
         - @b others:  Failed

*/
extern INT32 nvtdisp_getlayer(UINT32 id, UINT32 layer, UINT32 param,UINT32 *value);

/**
     Set destination window of layer.

     Set destination window of display layer.
     @note Destination window is an effective rectangle on device screen.
           User need to call nvtdisp_setlayerbufferaddr to present visual effect on screen.

     @param[in] id           device id (ex: NVTDISP_DOUT1 or NVTDISP_DOUT2).
     @param[in] layer        layer id (ex: NVTDISP_LYR_OSD1/OSD2/VDO1/VDO2).
     @param[in] x            left of window (base on device physical coordinate)
     @param[in] y            top of window (base on device physical coordinate)
     @param[in] w            width of window (base on device physical coordinate)
     @param[in] h            height of window (base on device physical coordinate)
     @return
         - @b ER_SUCCESS:   Successed
         - @b others:  Failed
*/
extern INT32  nvtdisp_setlayerscalewindow(UINT32 id, UINT32 layer, UINT32 x, UINT32 y, UINT32 w, UINT32 h);

/**
     Set buffer format of layer.

     Set buffer format of layer.
     @note User need to call nvtdisp_setlayerbufferaddr() to present visual effect on screen.

     @param[in] id           device id (ex: NVTDISP_DOUT1 or NVTDISP_DOUT2).
     @param[in] layer        layer id (ex: NVTDISP_LYR_OSD1/OSD2/VDO1/VDO2).
     @param[in] format       buffer format(enum of NVTDISP_BUFFORMAT)
     @param[in] w            buffer width
     @param[in] h            buffer height
     @param[in] loff         buffer line offset
     @return
         - @b ER_SUCCESS:   Successed
         - @b others:  Failed
*/
extern INT32  nvtdisp_setlayerbufferformat(UINT32 id, UINT32 layer, NVTDISP_BUFFORMAT format, UINT32 w, UINT32 h, UINT32 loff);

/**
     Set buffer address of layer.

     Set buffer address of layer.
     @note different format,addr would different

     @param[in] id           device id (ex: NVTDISP_DOUT1 or NVTDISP_DOUT2).
     @param[in] layer        layer id (ex: NVTDISP_LYR_OSD1/OSD2/VDO1/VDO2).
     @param[in] addr1        addr1 (ARGB)
     @param[in] addr2        addr2 (Alpha of ARGB8565)
     @param[in] addr3        addr3 (reserved)
     @param[in] wait         wait video sync
     @return
         - @b ER_SUCCESS:   Successed
         - @b others:  Failed
*/
extern INT32  nvtdisp_setlayerbufferaddr(UINT32 id, UINT32 layer, UINT32 addr1, UINT32 addr2,UINT32 addr3, UINT32 wait);


#ifdef __cplusplus
}
#endif
/* ----------------------------------------------------------------- */
#endif /* _NVTDISP_H  */

