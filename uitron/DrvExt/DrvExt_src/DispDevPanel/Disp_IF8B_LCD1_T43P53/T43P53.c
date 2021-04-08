/*
    Display object object panel configuration parameters for T43P53

    @file       T43P53.c
    @ingroup
    @note       Nothing

    Copyright   Novatek Microelectronics Corp. 2011.  All rights reserved.
*/

#include "kernel.h"
#include "Debug.h"
#include "display.h"
#include "dispdev_IF8bits.h"
#include "dispdev_panlcomn.h"

#define IFCMD_CMD		0x01000000
#define IFCMD_DATA		0x02000000


/*
    Panel Parameters for T30P105
*/
const tPANEL_CMD tCmdStandby[] =
{
  //  {0x2B, 0x00}
  {CMDDELAY_US, 1},
};


const tPANEL_CMD tCmdModeRgbd320[] =
{
	{IFCMD_CMD, 0xc8},
	{IFCMD_DATA,0xFF},
	{IFCMD_DATA,0x93},
	{IFCMD_DATA,0x42},

	{IFCMD_CMD, 0x36},
	//{IFCMD_DATA,0x08},//0xc8 for rotate
	//{IFCMD_CMD, 0x36},
	{IFCMD_DATA,0xC8},//0xc8 for rotate

	{IFCMD_CMD, 0x3A},
	{IFCMD_DATA,0x55},

	{IFCMD_CMD, 0xc0},
	{IFCMD_DATA,0x15},
	{IFCMD_DATA,0x15},

	{IFCMD_CMD, 0xC1},
	{IFCMD_DATA,0x01},

	{IFCMD_CMD, 0xC5},
	{IFCMD_DATA,0xF5},
	{IFCMD_CMD, 0xB1},
	{IFCMD_DATA,0x00},
	{IFCMD_DATA,0x1B},

	{IFCMD_CMD, 0xB4},
	{IFCMD_DATA,0x02},

	{IFCMD_CMD, 0xe0},
	{IFCMD_DATA,0x0F},
	{IFCMD_DATA,0x13},
	{IFCMD_DATA,0x17},
	{IFCMD_DATA,0x04},
	{IFCMD_DATA,0x13},
	{IFCMD_DATA,0x07},
	{IFCMD_DATA,0x40},
	{IFCMD_DATA,0x39},
	{IFCMD_DATA,0x4F},
	{IFCMD_DATA,0x06},
	{IFCMD_DATA,0x0d},
	{IFCMD_DATA,0x0A},
	{IFCMD_DATA,0x1F},
	{IFCMD_DATA,0x22},
	{IFCMD_DATA,0x00},

	{IFCMD_CMD, 0xE1},
	{IFCMD_DATA,0x00},
	{IFCMD_DATA,0x21},
	{IFCMD_DATA,0x24},
	{IFCMD_DATA,0x03},
	{IFCMD_DATA,0x0F},
	{IFCMD_DATA,0x05},
	{IFCMD_DATA,0x38},
	{IFCMD_DATA,0x32},
	{IFCMD_DATA,0x49},
	{IFCMD_DATA,0x00},
	{IFCMD_DATA,0x09},
	{IFCMD_DATA,0x08},
	{IFCMD_DATA,0x32},
	{IFCMD_DATA,0x35},
	{IFCMD_DATA,0x0f},

	{IFCMD_CMD, 0xb0},
	{IFCMD_DATA,0xe0},

	{IFCMD_CMD, 0xf6},
	{IFCMD_DATA,0x01},//0X01
	{IFCMD_DATA,0x00},
	{IFCMD_DATA,0x03},

	{IFCMD_CMD, 0x11}, //Exit Sleep
	{CMDDELAY_MS, 120},

	{IFCMD_CMD, 0x29}, //Display ON
	{IFCMD_CMD, 0x2C}, //Display ON
};


const tLCD_PARAM tMode[] =
{
    /***********       RGB Dummy-320 MODE         *************/
    {
        // tPANEL_PARAM
        {

            /* Old prototype */
			#if 0
            PINMUX_LCDMODE_RGB_THROUGH,		//!< LCDMode
            6.35f,							//!< fDCLK
            20+480,							//!< uiHSyncTotalPeriod
            480,							//!< uiHSyncActivePeriod
            13,								//!< uiHSyncBackPorch  26 32
            288,                            //!< uiVSyncTotalPeriod
            272,                            //!< uiVSyncActivePeriod
            12,								//!< uiVSyncBackPorchOdd
            12,								//!< uiVSyncBackPorchEven
            480,                            //!< uiBufferWidth
            272,                            //!< uiBufferHeight
            480,                            //!< uiWindowWidth
            272,                            //!< uiWindowHeight
            FALSE,                          //!< bYCbCrFormat

            /* New added parameters */
            2,								//!< uiHSyncSYNCwidth
            10,								//!< uiVSyncSYNCwidth
            #else
            PINMUX_LCDMODE_RGB_THROUGH,		//!< LCDMode
            5.6f,							//!< fDCLK
            24+480,							//!< uiHSyncTotalPeriod
            480,							//!< uiHSyncActivePeriod
            13,								//!< uiHSyncBackPorch
            288,							//!< uiVSyncTotalPeriod
            272,							//!< uiVSyncActivePeriod
            12,								//!< uiVSyncBackPorchOdd
            12,								//!< uiVSyncBackPorchEven
            480,							//!< uiBufferWidth
            272,							//!< uiBufferHeight
            480,							//!< uiWindowWidth
            272,							//!< uiWindowHeight
            FALSE,							//!< bYCbCrFormat

            /* New added parameters */
            4,								//!< uiHSyncSYNCwidth
            10,								//!< uiVSyncSYNCwidth
			#endif
        },

        // tIDE_PARAM
        {
            /* Old prototype */
            PINMUX_LCD_SEL_SERIAL_RGB_8BITS,//!< pinmux_select_lcd;
            ICST_CCIR601,                   //!< icst;
            {FALSE,FALSE},                  //!< dithering[2];
            DISPLAY_DEVICE_TOPPOLY,         //!< **DONT-CARE**
            IDE_PDIR_RGB,                   //!< pdir;
            IDE_LCD_B,						//!< odd;
            IDE_LCD_B,						//!< even;
            TRUE,                           //!< hsinv;
            TRUE,                           //!< vsinv;
            FALSE,                          //!< hvldinv;
            FALSE,                          //!< vvldinv;
            TRUE,                           //!< clkinv;
            FALSE,                          //!< fieldinv;
            FALSE,                           //!< rgbdummy
            FALSE,                          //!< interlace;
            FALSE,                          //!< **DONT-CARE**
            0x40,                           //!< ctrst;
            0x00,                           //!< brt;
            0x40,                           //!< cmults;
            FALSE,                          //!< cex;
            FALSE,                          //!< **DONT-CARE**
            TRUE,                           //!< **DONT-CARE**
            TRUE,                           //!< tv_powerdown;
            {0x00,0x00},                    //!< **DONT-CARE**

            /* New added parameters */
            FALSE,                          //!< YCex
            FALSE,                          //!< HLPF
            {TRUE,TRUE,TRUE},               //!< subpixOdd[3]
            {FALSE,FALSE,FALSE},            //!< subpixEven[3]
            {IDE_DITHER_6BITS,IDE_DITHER_6BITS,IDE_DITHER_6BITS}//!< DitherBits[3]
        },
#if 0
        (tPANEL_CMD*)tCmdModeRgbd320,                  //!< pCmdQueue
        sizeof(tCmdModeRgbd320)/sizeof(tPANEL_CMD),    //!< nCmd
#endif
    }
};


#define ROTATE_PARA_COUNT 5
const tPANEL_CMD tCmdRotate_None[] =
{
        {IFCMD_CMD, 0x11}, //Exit Sleep
        {IFCMD_CMD, 0x36},
        {IFCMD_DATA,0x08},//0xc8 for rotate
        {IFCMD_CMD, 0x29}, //Display ON
        {IFCMD_CMD,0x2c}, //memory write
};
const tPANEL_CMD tCmdRotate_180[] =
{
        {IFCMD_CMD, 0x11}, //Exit Sleep
        {IFCMD_CMD, 0x36},
        {IFCMD_DATA,0xC8},//0xc8 for rotate
        {IFCMD_CMD, 0x29}, //Display ON
        {IFCMD_CMD,0x2c}, //memory write
};
const tLCD_ROT tRotate[] =
{
#if 0
    {DISPDEV_LCD_ROTATE_NONE,(tPANEL_CMD*)tCmdRotate_None,ROTATE_PARA_COUNT},
    {DISPDEV_LCD_ROTATE_180,(tPANEL_CMD*)tCmdRotate_180,ROTATE_PARA_COUNT}
#endif
};
//@}

tLCD_ROT* dispdev_getLcdRotateCmd(UINT32 *ModeNumber)
{CHKPNT;
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

tLCD_PARAM *dispdev_getConfigMode(UINT32 *ModeNumber)
{
	*ModeNumber = sizeof(tMode) / sizeof(tLCD_PARAM);
	return (tLCD_PARAM *)tMode;
}

tPANEL_CMD *dispdev_getStandbyCmd(UINT32 *CmdNumber)
{
	*CmdNumber = sizeof(tCmdStandby) / sizeof(tPANEL_CMD);
	return (tPANEL_CMD *)tCmdStandby;
}

#if 1
void dispdev_writeToLcdSif(DISPDEV_IOCTRL pDispDevControl, UINT32 addr, UINT32 value)
{
    UINT32                  uiSIFData;
    DISPDEV_IOCTRL_PARAM    DevIOCtrl;

    uiSIFData = ((((UINT32)addr << 8)|((UINT32)value)) << 16);

    pDispDevControl(DISPDEV_IOCTRL_GET_REG_IF, &DevIOCtrl);
    sif_send(DevIOCtrl.SEL.GET_REG_IF.uiSifCh, uiSIFData, 0, 0);
}

static void dispdev_writeToLcdGpio_Cmd(DISPDEV_IOCTRL pDispDevControl, UINT32 addr, UINT32 value)
{CHKPNT;
    UINT32                  uiSIFData,j;
    UINT32                  SifClk,SifSen,SifData;
    DISPDEV_IOCTRL_PARAM    DevIOCtrl;

	//DBG_DUMP("^C dispdev_writeToLcdGpioParal!\r\n");
    pDispDevControl(DISPDEV_IOCTRL_GET_REG_IF, &DevIOCtrl);
    SifClk  = DevIOCtrl.SEL.GET_REG_IF.uiGpioClk;
    SifSen  = DevIOCtrl.SEL.GET_REG_IF.uiGpioSen;
    SifData = DevIOCtrl.SEL.GET_REG_IF.uiGpioData;
	//DBG_DUMP("^C SifClk %d, SifSen %d, SifData %d\r\n", SifClk, SifSen, SifData);

    gpio_setDir(SifSen, GPIO_DIR_OUTPUT);
    gpio_setDir(SifClk, GPIO_DIR_OUTPUT);
    gpio_setDir(SifData, GPIO_DIR_OUTPUT);

    gpio_setPin(SifSen);
    gpio_setPin(SifData);
    gpio_clearPin(SifClk);

    uiSIFData = (UINT32)value;

    Delay_DelayUs(500);

    gpio_clearPin(SifSen);
    for(j=9; j>0; j--)
    {
        if(((uiSIFData >> (j - 1)) & 0x01))
        {
            gpio_setPin(SifData);
        }
        else
        {
            gpio_clearPin(SifData);
        }

        Delay_DelayUs(100);
        gpio_setPin(SifClk);
        Delay_DelayUs(200);
        gpio_clearPin(SifClk);
        Delay_DelayUs(100);
    }
    Delay_DelayUs(500);
    gpio_setPin(SifSen);

}


static void dispdev_writeToLcdGpio_data(DISPDEV_IOCTRL pDispDevControl, UINT32 addr, UINT32 value)
{CHKPNT;
    UINT32                  uiSIFData,j;
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
    gpio_clearPin(SifClk);

    uiSIFData = (UINT32)value;
    uiSIFData |= 0x100;

    Delay_DelayUs(500);

    gpio_clearPin(SifSen);

    for(j=9; j>0; j--)
    {
        if(((uiSIFData >> (j - 1)) & 0x01))
        {
            gpio_setPin(SifData);
        }
        else
        {
            gpio_clearPin(SifData);
        }

        Delay_DelayUs(100);
        gpio_setPin(SifClk);
        Delay_DelayUs(200);
        gpio_clearPin(SifClk);
        Delay_DelayUs(100);
    }
    Delay_DelayUs(500);
    gpio_setPin(SifSen);

}
void dispdev_writeToLcdGpio(DISPDEV_IOCTRL pDispDevControl, UINT32 addr, UINT32 value)
{

    if(addr & IFCMD_DATA)
            dispdev_writeToLcdGpio_data(pDispDevControl,addr,value);
        else
            dispdev_writeToLcdGpio_Cmd(pDispDevControl,addr,value);
}

void dispdev_readFromLcdGpio(DISPDEV_IOCTRL pDispDevControl, UINT32 addr, UINT32 *p_value)
{
	DBG_ERR("Has not implement dispdev_readFromLcdGpio \r\n");
}
#endif
