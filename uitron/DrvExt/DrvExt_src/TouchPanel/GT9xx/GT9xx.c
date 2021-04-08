/**
    Copyright   Novatek Microelectronics Corp. 2009.  All rights reserved.

    @file       Input_Touch.c
    @ingroup    mIPRJAPKeyIO

    @brief      Scan Touch Panel HW level

    @note       Nothing.

    @date       2009/04/22
*/

/** \addtogroup mIPRJAPKeyIO */
//@{

#include "kernel.h"
#include "Debug.h"
#include "DxInput.h"
#include "adc.h"
#include "Utility.h"
#include "i2c.h"
#include "mIType.h"
#include "GT9xx.h"
#include "gpio.h"
///////////////////////////////////////////////////////////////////////////////
#define __MODULE__          DxDrv
#define __DBGLVL__          2 // 0=OFF, 1=ERROR, 2=TRACE
#define __DBGFLT__          "*" //*=All, [mark]=CustomClass
#include "DebugModule.h"
///////////////////////////////////////////////////////////////////////////////

#define TP_PEN_IO          L_GPIO_21
#define PAD_PEN_IO         PAD_PIN_LGPIO21

#define TP_RESET_IO        L_GPIO_20

#define TP_PRESSURE_TH      240     // 255 - 10

#define TP_HW_MAX_X         1920
#define TP_HW_MAX_Y         384
#define TP_HW_MIN_X         0
#define TP_HW_MIN_Y         0

#define TP_PANEL_X1         0
#define TP_PANEL_Y1         0
#define TP_PANEL_X2         1919
#define TP_PANEL_Y2         383

#define TP_PANEL_W          (TP_PANEL_X2-TP_PANEL_X1 + 1)
#define TP_PANEL_H          (TP_PANEL_Y2-TP_PANEL_Y1 + 1)

#define READ_ONE_TP_ONLY   1

static UINT32 uiTouchPoints;
UINT32 uiSystemTouchTimeTick;
UINT32 uiValidTouchTimeTick;
static UINT32 uiTimeTickIntv;
//static UINT32 uiRemainingPoints;


UINT32 point1_x_prev;
UINT32 point1_y_prev;
UINT32 point2_x_prev;
UINT32 point2_y_prev;

BOOL bswap_p1p2 = FALSE;
//////////////////////////////////////////////////////////////////////
//for touch panel
//Touch panel I2C info structure
typedef struct
{
    I2C_SESSION     I2C_Channel;
    I2C_PINMUX      I2C_PinMux;
    I2C_BUS_CLOCK   I2C_BusClock;
    UINT32          I2C_Slave_WAddr;
    UINT32          I2C_Slave_RAddr;
} TP_INFO, *PTP_INFO;

//Touch Panel recieved data structure // 2 points touch
typedef struct
{
    UINT32 P1_TouchStatus;
    UINT32 P1_TouchEventID;
    UINT32 P1_CoordRecvdX;
    UINT32 P1_CoordRecvdY;
    UINT32 P2_TouchStatus;
    UINT32 P2_TouchEventID;
    UINT32 P2_CoordRecvdX;
    UINT32 P2_CoordRecvdY;
} TP_RECV_DATA, *pPT_RECV_DATA;

//////////////////////////////////////////////////////////////////////

static TP_RECV_DATA     g_TPRecvdData;
static pPT_RECV_DATA    pTPRecvdData = &g_TPRecvdData;
static TP_INFO          g_TpInfo;

/*
    0:  Use AUTO Mode with Polling
    1:  Use AUTO Mode with Interrupt (N/A)
    2:  Use AUTO Mode with FIFO Threshold (N/A)
*/
#define GSTP_USED_MODE      0

static BOOL TP_I2C_Init(TP_INFO TouchInfo)
{
    UINT erReturn;
    g_TpInfo.I2C_PinMux = TouchInfo.I2C_PinMux;
    g_TpInfo.I2C_BusClock = TouchInfo.I2C_BusClock;
    g_TpInfo.I2C_Slave_WAddr = TouchInfo.I2C_Slave_WAddr;
    g_TpInfo.I2C_Slave_RAddr = TouchInfo.I2C_Slave_RAddr;

    if (g_TpInfo.I2C_PinMux > I2C_PINMUX_2ND)
    {
        DBG_ERR("I2C3 pin mux err!\r\n");
        return FALSE;
    }

    if (g_TpInfo.I2C_BusClock > I2C_BUS_CLOCK_1MHZ)
    {
        DBG_ERR("I2C3 bus clock err!\r\n");
        return FALSE;
    }

    erReturn = i2c3_open(&(g_TpInfo.I2C_Channel));
    if(erReturn != I2C_STS_OK)
    {
        DBG_ERR("Open I2C3 driver err!\r\n");
        return FALSE;
    }
    return TRUE;
}
I2C_STS TP_I2C_Receive(UINT32 *value, BOOL bNACK, BOOL bStop)
{
    I2C_DATA I2cData;
    I2C_BYTE I2cByte[1];
    I2C_STS ret;

    I2cData.VersionInfo = DRV_VER_96660;
    I2cData.ByteCount = I2C_BYTE_CNT_1;
    I2cData.pByte = I2cByte;
    I2cByte[0].Param = I2C_BYTE_PARAM_NONE;
    if ( bNACK == TRUE )
        I2cByte[0].Param |= I2C_BYTE_PARAM_NACK;
    if ( bStop == TRUE )
        I2cByte[0].Param |= I2C_BYTE_PARAM_STOP;

    ret = i2c3_receive(&I2cData);
    if ( ret != I2C_STS_OK )
    {
        DBG_ERR("i2c3 ret = %02x!!\r\n", ret);
    }

    *value = I2cByte[0].uiValue;

    return ret;
}

I2C_STS TP_I2C_Transmit(UINT32 value, BOOL bStart, BOOL bStop)
{
    I2C_DATA I2cData;
    I2C_BYTE I2cByte[1];
    I2C_STS ret;

    I2cData.VersionInfo = DRV_VER_96660;
    I2cData.ByteCount = I2C_BYTE_CNT_1;
    I2cData.pByte = I2cByte;
    I2cByte[0].uiValue = value & 0xff;
    I2cByte[0].Param = I2C_BYTE_PARAM_NONE;
    if ( bStart == TRUE )
        I2cByte[0].Param |= I2C_BYTE_PARAM_START;
    if ( bStop == TRUE )
        I2cByte[0].Param |= I2C_BYTE_PARAM_STOP;

    DBG_IND("I2cData VersionInfo=0x%x, ByteCount =0x%x, uiValue=0x%x, parm=0x%x\r\n",I2cData.VersionInfo,I2cData.ByteCount,I2cByte[0].uiValue,I2cByte[0].Param);
    ret = i2c3_transmit(&I2cData);
    if ( ret != I2C_STS_OK )
    {
            DBG_ERR("i2c3 ret = %d!!\r\n", ret);
    }
    return ret;
}



static UINT32 gtp_i2c_write_reg(UINT32 uiAddr, UINT32 uiData)
{
    I2C_DATA    I2cData;
    I2C_BYTE    I2cByte[16];
    I2C_STS     ret;
    if (i2c3_lock(g_TpInfo.I2C_Channel) != E_OK)
    {
        DBG_ERR("I2C lock fail!\r\n");
        return 0;
    }
    I2cData.VersionInfo = DRV_VER_96660;
    I2cData.ByteCount = I2C_BYTE_CNT_4;
    I2cData.pByte = I2cByte;
    I2cByte[0].uiValue = g_TpInfo.I2C_Slave_WAddr;
    I2cByte[0].Param = I2C_BYTE_PARAM_START;
    I2cByte[1].uiValue = ((uiAddr & 0x0000ff00) >> 8); //MSB
    I2cByte[1].Param = I2C_BYTE_PARAM_NONE;
	I2cByte[2].uiValue = (uiAddr & 0x000000ff);//LSB
    I2cByte[2].Param = I2C_BYTE_PARAM_NONE;
    I2cByte[3].uiValue = uiData & 0x000000ff;
    I2cByte[3].Param = I2C_BYTE_PARAM_STOP;
    ret = i2c3_transmit(&I2cData);

	// for(i = 0; i < ReadNum; i ++)
    {
      //  debug_msg("0x%02x,",  I2cByte[3].uiValue);
    }
	 
    if (ret != I2C_STS_OK)
    {
        DBG_ERR("(W)i2c3 ret = %02x!!\r\n", ret);
		i2c3_unlock(g_TpInfo.I2C_Channel);
		return -1;
    }
    if (i2c3_unlock(g_TpInfo.I2C_Channel) != E_OK)
    {
        DBG_ERR("I2C3 unlock fail!\r\n");
        return -1;
    }

    return 0;
}


static UINT32 gtp_i2c_read_reg(UINT32 uiAddr, UINT32 *RecvBuf, UINT32 ReadNum)
{
    I2C_DATA    I2cData;
    I2C_BYTE    I2cByte[16];
    I2C_STS     ret;
    UINT32       i;
    if (i2c3_lock(g_TpInfo.I2C_Channel) != E_OK)
    {
        DBG_ERR("I2C lock fail!\r\n");
        return 0;
    }
    I2cData.VersionInfo = DRV_VER_96660;
    I2cData.ByteCount = I2C_BYTE_CNT_4;
    I2cData.pByte = I2cByte;
    I2cByte[0].uiValue = g_TpInfo.I2C_Slave_WAddr;
    I2cByte[0].Param = I2C_BYTE_PARAM_START;
    I2cByte[1].uiValue = ((uiAddr & 0x0000ff00) >> 8); //MSB
    I2cByte[1].Param = I2C_BYTE_PARAM_NONE;
	I2cByte[2].uiValue = (uiAddr & 0x000000ff);//LSB
    I2cByte[2].Param = I2C_BYTE_PARAM_NONE;
    I2cByte[3].uiValue = g_TpInfo.I2C_Slave_RAddr;
    I2cByte[3].Param = I2C_BYTE_PARAM_START;
    ret = i2c3_transmit(&I2cData);
    if (ret != I2C_STS_OK)
    {
        DBG_ERR("(W)i2c3 ret = %02x!!\r\n", ret);
		i2c3_unlock(g_TpInfo.I2C_Channel);
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
    if (i2c3_unlock(g_TpInfo.I2C_Channel) != E_OK)
    {
        DBG_ERR("I2C3 unlock fail!\r\n");
        return -1;
    }

    for(i = 0; i < ReadNum; i ++)
    {
        RecvBuf[i] = I2cByte[i].uiValue;
    }
	
	/*debug_msg("^GLiwk--Read From Reg:");
	//debug_msg("^G CJ--Read From Reg:");
    for(i = 0; i < ReadNum; i ++)
    {
        debug_msg("0x%02x,", RecvBuf[i]);
    }
	//debug_msg("\r\n");
	//*/
    return 0;
}

void DetTP_GetXY(INT32 *pX, INT32 *pY)
{
	UINT32 I2CReadBuffer[16];
	UINT32 ret;
    *pX = -1;
    *pY = -1;

    ret = gtp_i2c_read_reg(GTP_READ_COOR_ADDR, I2CReadBuffer,1);
	if(I2CReadBuffer[0] & 0x80)
	{ 
	   if((I2CReadBuffer[0] & 0x0f) != 0)
	   {
		  ret = gtp_i2c_read_reg(GTP_READ_COOR_ADDR + 1, I2CReadBuffer, 3);
		  *pX = ((I2CReadBuffer[2] << 8) | I2CReadBuffer[1]);
		  if(*pX > 0)
		  {
		     *pX = TP_PANEL_X2 - *pX;
		  }
		  ret = gtp_i2c_read_reg(GTP_READ_COOR_ADDR + 4, I2CReadBuffer, 2);  
		  *pY = ((I2CReadBuffer[1] << 8) | I2CReadBuffer[0]);
		  if(*pY > 0)
		  {
		     *pY = TP_PANEL_Y2 - *pY;
		  }
		  
	   }
	   gtp_i2c_write_reg(GTP_READ_COOR_ADDR, 0x00);
	}

}

void DetTP_GetXY2Point(INT32 *pX, INT32 *pY, INT32 *pX2nd, INT32 *pY2nd)
{
    memset(pTPRecvdData, 0, sizeof(TP_RECV_DATA));

    uiTimeTickIntv = (uiSystemTouchTimeTick - uiValidTouchTimeTick);
	uiValidTouchTimeTick = uiSystemTouchTimeTick;

	if(uiTimeTickIntv <= 3)//nearby two event
	{
	   if((abs(pTPRecvdData->P1_CoordRecvdX - point1_x_prev) > 80 ||\
	   	abs(pTPRecvdData->P1_CoordRecvdY - point1_y_prev) > 60) && point1_x_prev != 319 && point1_y_prev != 0)
	   {
	      bswap_p1p2 = TRUE;
	   }
	   if((abs(pTPRecvdData->P2_CoordRecvdX - point2_x_prev) > 80 || \
	   	 abs(pTPRecvdData->P2_CoordRecvdY - point2_y_prev) > 60) && \
	   	 point2_x_prev != 319 && point2_y_prev != 0)	   	 
	   {
	      bswap_p1p2 = TRUE;
	   }
	}
    point1_x_prev = (pTPRecvdData->P1_CoordRecvdX);
    point1_y_prev = (pTPRecvdData->P1_CoordRecvdY);
    point2_x_prev = (pTPRecvdData->P2_CoordRecvdX);
    point2_y_prev = (pTPRecvdData->P2_CoordRecvdY);
	
	if(uiTimeTickIntv > 3)
	{
	   bswap_p1p2 = FALSE;
	}
	if(bswap_p1p2)
	{
	   
	   *pX = pTPRecvdData->P2_CoordRecvdX;
	   *pY = pTPRecvdData->P2_CoordRecvdY;
	   *pX2nd = pTPRecvdData->P1_CoordRecvdX;
	   *pY2nd = pTPRecvdData->P1_CoordRecvdY;
	}
	else
	{
	   *pX = pTPRecvdData->P1_CoordRecvdX;
	   *pY = pTPRecvdData->P1_CoordRecvdY;
	   *pX2nd = pTPRecvdData->P2_CoordRecvdX;
	   *pY2nd = pTPRecvdData->P2_CoordRecvdY;
	}
	if(uiTouchPoints < 2)
	{
	   if(*pX == 319 && *pY == 0)
	   {
	     *pX = -1;
		 
		 *pY = -1;
	   }
	   if(*pX2nd== 319 && *pY2nd== 0)
	   {
	     *pX2nd= -1;
		 *pY2nd= -1;
	   }
	}
    //debug_msg("Liwk  points %d remain: %d- 1ST Status %d Touch  (%d, %d)\r\n", uiTouchPoints,uiRemainingPoints,touch_status, *pX, *pY);
    //debug_msg("Liwk  points %d remain: %d- 2ND Status %d Touch  (%d, %d)\r\n",uiTouchPoints, uiRemainingPoints,touch_status,*pX2nd, *pY2nd);
}

static void TP_Reset(void)
{
    gpio_setDir(TP_RESET_IO,GPIO_DIR_OUTPUT);
    gpio_clearPin(TP_RESET_IO);
	gpio_setDir(TP_PEN_IO,GPIO_DIR_OUTPUT);
    gpio_clearPin(TP_PEN_IO);
	Delay_DelayMs(1);
    gpio_setPin(TP_RESET_IO);
	gpio_clearPin(TP_PEN_IO);
	Delay_DelayMs(50);
    gpio_setDir(TP_PEN_IO,GPIO_DIR_INPUT);
    pad_setPullUpDown(PAD_PEN_IO, PAD_NONE);

}


/*
static INT32 gtp_i2c_test(void)
{
	UINT32 I2CReadBuffer[16];
	UINT32 ret;
    ret = gtp_i2c_read_reg(GTP_REG_CONFIG_DATA, I2CReadBuffer,4);
	// debug_msg("0x%02x,", RecvBuf[i]);
    ret = gtp_i2c_read_reg(GTP_REG_VERSION, I2CReadBuffer, 4);
    return ret;
}
*/
static UINT8 gtp_reg_table[CTP_CFG_REG_NUM_MAX] = CTP_CFG_GROUP0;
static UINT32 uiTouchPanelInitOK = FALSE;
void DetTP_Init(void)
{
    TP_INFO TPInfo;
    UINT32 i;
	UINT32 reg_cfg = GTP_REG_CONFIG_DATA;
 	//UINT32 I2CReadBuffer[16];
	UINT8 checksum = 0;
	/*
	for(i = 0; i < CTP_CFG_REG_NUM_MAX - 2; i ++)
	{
	   checksum += gtp_reg_table[i];
	}
    if((checksum & (1 << 7)) != 0)
	{
	   checksum = ((~(checksum&0x7f)) + 1);
	}//*/
    gtp_reg_table[CTP_CFG_REG_NUM_MAX - 2] = checksum;	
    TPInfo.I2C_PinMux = I2C_PINMUX_1ST;
    TPInfo.I2C_BusClock = I2C_BUS_CLOCK_200KHZ;
    TPInfo.I2C_Slave_WAddr = 0xba;
    TPInfo.I2C_Slave_RAddr = 0xbb;
    debug_msg("^R Liwk--------TP Init!!\r\n");
    TP_Reset();

    if(TP_I2C_Init(TPInfo) == TRUE)
    {
       debug_msg("^G Liwk--------TP Init!! I2C Init success!!\r\n");
//	   debug_msg("^G Liwk-----tab checksum:%x\r\n",checksum);
	//  gtp_i2c_test();
	   uiTouchPanelInitOK = TRUE;
  
	   for(i = 0; i < CTP_CFG_REG_NUM_MAX; i ++)
	   { 
	      gtp_i2c_write_reg(reg_cfg++, gtp_reg_table[i]);
	   }
	   
	//   debug_msg("---cj TP init success!!!!!!!!!!!"); 

	   
	   /*
	  UINT32 I2CReadBuffer[16];
	  reg_cfg = GTP_REG_CONFIG_DATA;
	   for(i = 0; i < CTP_CFG_REG_NUM_MAX; i ++)
	   {
	      gtp_i2c_read_reg(reg_cfg++,I2CReadBuffer,1);
	   }

	   gtp_i2c_test();
	   //*/
	   
	   
    }
}

static UINT32 system_tick = 0;
BOOL DetTP_IsPenDown(void)
{
    UINT32 uiTPCurLvl;
    uiTPCurLvl = gpio_getPin(TP_PEN_IO);
	system_tick ++;
	if(system_tick >= 50 * 2 && uiTouchPanelInitOK && uiTPCurLvl)
	{
	   	system_tick = 0xffff;
		return TRUE;
	}
	return FALSE;
}
