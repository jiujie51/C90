/*
    Display object object panel configuration parameters for NT53001

    @file      NT53001.c
    @ingroup
    @note       Nothing

    Copyright   Novatek Microelectronics Corp. 2011.  All rights reserved.
*/

#include "display.h"
#include "dispdev_IFParal.h"
#include "dispdev_panlcomn.h"
#include "i2c.h"
#include "Delay.h"
#include "LT8918_Int.h"
#include "dcs.h"

#define IFCMD_CMD		0x01000000
#define IFCMD_DATA		0x02000000

#define LT8918_WRITE_ID		0x40
#define LT8918_READ_ID		0x41

static LT8918_I2C_INFO		g_I2cInfo;
static PI2C_OBJ				g_pI2cObj;


/*Used in RGB 16-bit da mode, we must trans stripe pattern */
const tPANEL_CMD tCmdStandbyParal[] = {
	{IFCMD_CMD, 0x01},
};

const tLCD_PARAM tModeParal[] = {
	/***********       RGB16 DA MODE         *************/
	{
		// tPANEL_PARAM
		{
			/* Old prototype */
			PINMUX_LCDMODE_RGB_PARALL565,//!< LCDMode
			43.0f,                          //!< fDCLK
			522,                           //!< uiHSyncTotalPeriod
			320,                           //!< uiHSyncActivePeriod
			168,                            //!< uiHSyncBackPorch
			1304,                            //!< uiVSyncTotalPeriod
			1280,                            //!< uiVSyncActivePeriod
			10,                             //!< uiVSyncBackPorchOdd
			10,                             //!< uiVSyncBackPorchEven
			320,                            //!< uiBufferWidth
			1280,                            //!< uiBufferHeight
			320,                            //!< uiWindowWidth
			1280,                            //!< uiWindowHeight
			FALSE,                          //!< bYCbCrFormat

			/* New added parameters */
			0x01,                           //!< uiHSyncSYNCwidth
			0x01                            //!< uiVSyncSYNCwidth
		},

		// tIDE_PARAM
		{
			/* Old prototype */
			PINMUX_LCD_SEL_PARALLE_RGB565 | PINMUX_LCD_SEL_DE_ENABLE,//!< pinmux_select_lcd;
			ICST_CCIR601,                   //!< icst;
			{FALSE, FALSE},                 //!< dithering[2];
			DISPALY_DEVICE_PARALLEL,       //!< **DONT-CARE**
			IDE_PDIR_RGB,                   //!< pdir;
			IDE_LCD_R,                      //!< odd;
			IDE_LCD_R,                      //!< even;
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
			0x40,                           //!< cmults;
			FALSE,                          //!< cex;
			FALSE,                          //!< **DONT-CARE**
			FALSE,                          //!< clamp
			TRUE,                           //!< tv_powerdown;
			{0x00, 0x00},                   //!< **DONT-CARE**

			/* New added parameters */
			FALSE,                          //!< YCex
			FALSE,                          //!< HLPF
			{FALSE, FALSE, FALSE},             //!< subpixOdd[3]
			{FALSE, FALSE, FALSE},          //!< subpixEven[3]
			{IDE_DITHER_6BITS, IDE_DITHER_6BITS, IDE_DITHER_6BITS}, //!< DitherBits[3]
			FALSE                           //!< clk1/2
		},

		(tPANEL_CMD *)tCmdStandbyParal,                 //!< pCmdQueue
		sizeof(tCmdStandbyParal) / sizeof(tPANEL_CMD),  //!< nCmd
	},
};

const tLCD_ROT *tRotateParal = NULL;

//@}

I2C_STS LT8918_I2C_Transmit(UINT32 i2c_w_addr, UINT32 reg_addr, UINT32 value)
{
	I2C_DATA I2cData;
	I2C_BYTE I2cByte[8];
	//I2C_STS ret = I2C_STS_INVALID_PARAM;
	I2C_STS ret;

	I2cData.pByte = I2cByte;
	I2cData.VersionInfo = DRV_VER_96660;
	I2cData.ByteCount = 3;
	I2cData.pByte[0].uiValue = i2c_w_addr & 0xff;
	I2cData.pByte[0].Param = I2C_BYTE_PARAM_START;
	I2cData.pByte[1].uiValue = reg_addr & 0xff;
	I2cData.pByte[1].Param = I2C_BYTE_PARAM_NONE;
	I2cData.pByte[2].uiValue = value & 0xff;
	I2cData.pByte[2].Param = I2C_BYTE_PARAM_STOP;

	if ((ret = g_pI2cObj->transmit(&I2cData)) != I2C_STS_OK) {
		DBG_ERR("fail to send data, w_addr 0x%X, r_addr 0x%X, data 0x%X, sts %d\r\n", i2c_w_addr, reg_addr, value, ret);
	}
	return ret;
}

I2C_STS LT8918_I2C_Receive(UINT32 i2c_w_addr, UINT32 i2c_r_addr, UINT32 reg_addr, UINT32 *value)
{
	I2C_DATA I2cData;
	I2C_BYTE I2cByte[8];
	I2C_STS ret;

	//send i2c address of write
	I2cData.pByte = I2cByte;
	I2cData.VersionInfo = DRV_VER_96660;
	I2cData.ByteCount = 1;
	I2cData.pByte[0].uiValue = i2c_w_addr & 0xff;
	I2cData.pByte[0].Param = I2C_BYTE_PARAM_START;

	if ((ret = g_pI2cObj->transmit(&I2cData)) != I2C_STS_OK) {
		DBG_ERR("fail to write slave address 0x%X, sts %d\r\n", i2c_w_addr, ret);
		return ret;
	}

	//send register address
	I2cData.ByteCount = 1;
	I2cData.pByte[0].uiValue = reg_addr & 0xff;
	I2cData.pByte[0].Param = I2C_BYTE_PARAM_NONE;

	if ((ret = g_pI2cObj->transmit(&I2cData)) != I2C_STS_OK) {
		DBG_ERR("fail to send data 0x%X, sts %d\r\n", reg_addr, ret);
		return ret;
	}

	//send i2c address of read
	I2cData.VersionInfo = DRV_VER_96660;
	I2cData.ByteCount = 1;
	I2cData.pByte[0].uiValue = i2c_r_addr & 0xff;
	I2cData.pByte[0].Param = I2C_BYTE_PARAM_START;

	if ((ret = g_pI2cObj->transmit(&I2cData)) != I2C_STS_OK) {
		DBG_ERR("fail to write slave address 0x%X, sts %d\r\n", i2c_r_addr, ret);
		return ret;
	}

	//receive register value
	I2cData.ByteCount = 1;
	I2cData.pByte[0].uiValue = 0;
	I2cData.pByte[0].Param = I2C_BYTE_PARAM_NACK | I2C_BYTE_PARAM_STOP;

	if ((ret = g_pI2cObj->receive(&I2cData)) != I2C_STS_OK) {
		DBG_ERR("fail to receive data, sts %d\r\n", ret);
		return ret;
	}

	*value = I2cData.pByte[0].uiValue;
	//DBG_DUMP("^C Reg addr 0x%02X, data 0x%02X\r\n", reg_addr, *value);
	return ret;
}

ER LT8918_WriteReg(UINT32 addr, UINT32 data)
{
	ER ret = E_OK;

	g_pI2cObj->lock(g_I2cInfo.I2C_Channel);
	if (LT8918_I2C_Transmit(LT8918_WRITE_ID, addr, data) != I2C_STS_OK) {
		ret = E_SYS;
	}
	g_pI2cObj->unlock(g_I2cInfo.I2C_Channel);
	return ret;
}

ER LT8918_ReadReg(UINT32 addr, UINT32* data)
{
	ER ret = E_OK;

	g_pI2cObj->lock(g_I2cInfo.I2C_Channel);
	if (LT8918_I2C_Receive(LT8918_WRITE_ID, LT8918_READ_ID, addr, data) != I2C_STS_OK) {
		ret = E_SYS;
	}
	g_pI2cObj->unlock(g_I2cInfo.I2C_Channel);

	return ret;
}

ER LT8918_Init(void)
{
	ER ret=0;
	UINT32 i, count;
	LT8918_CMD *cmd;

	// I2C init
	g_I2cInfo.I2C_RegBytes		= 1; // 1 byte
	g_I2cInfo.I2C_PinMux		= I2C_PINMUX_1ST; // useless for 67x?
	g_I2cInfo.I2C_BusClock		= I2C_BUS_CLOCK_400KHZ;
	g_I2cInfo.I2C_Slave_WAddr	= LT8918_WRITE_ID;
	g_I2cInfo.I2C_Slave_RAddr	= LT8918_READ_ID;

	g_pI2cObj = i2c_getDrvObject(I2C_ID_I2C3); // 1st I2C   I2C_ID_I2C

	ret = g_pI2cObj->open(&(g_I2cInfo.I2C_Channel));
	if (ret != I2C_STS_OK) {
		DBG_ERR("open I2C failed!\r\n");
		return ret;
	}

	g_pI2cObj->setConfig(g_I2cInfo.I2C_Channel, I2C_CONFIG_ID_MODE, I2C_MODE_MASTER);
	//g_pI2cObj->setConfig(g_I2cInfo.I2C_Channel, I2C_CONFIG_ID_PINMUX, g_I2cInfo.I2C_PinMux); //select to control I2C 1st device
	g_pI2cObj->setConfig(g_I2cInfo.I2C_Channel, I2C_CONFIG_ID_BUSCLOCK, g_I2cInfo.I2C_BusClock); //up to 400KHZ
	g_pI2cObj->setConfig(g_I2cInfo.I2C_Channel, I2C_CONFIG_ID_SCLTIMEOUT_CHECK, DISABLE);

	//DBG_DUMP("^C LT8918_init...\r\n");
	// LT8189 init table
	count = LT8918_GetInitTbl(&cmd);
	//debug_msg("----LT8918--count=%d-----\r\n",count);
	for (i = 0; i < count; i++)	{
		//debug_msg("----LT8918--cmd==%d=0x%x--0x%x---\r\n",i,cmd[i].addr, cmd[i].data);
		if (cmd[i].addr == CMD_DELAY) {
			Delay_DelayMs(cmd[i].data);
		} else {
			LT8918_WriteReg(cmd[i].addr, cmd[i].data);
		}
	}
	//DBG_DUMP("^C LT8918_init...end\r\n");
	return E_OK;
}

ER LT8918_SetRegBank(UINT32 bank)
{
	if (LT8918_WriteReg(0xff, bank) != E_OK) {
		return E_SYS;
	}
	return E_OK;
}

//-----------------------------------------------------------------------------
//hfp,hs,hbp,hact,htotal,vfp,vs,vbp,vact,vtotal,pixclk:70MHz
typedef struct
{
	UINT32 hfp;
	UINT32 hs;
	UINT32 hbp;
	UINT32 hact;
	UINT32 htotal;
	UINT32 vfp;
	UINT32 vs;
	UINT32 vbp;
	UINT32 vact;
	UINT32 vtotal;
} LT8918_VIDEO_TIMING;

static LT8918_VIDEO_TIMING g_VidTiming = {34/*hfp*/,2/*hs*/,166/*hbp*/,320/*hact*/,522/*htotal*/,
                                          14/*vfp*/,2/*vs*/,8/*vbp*/,1280/*vact*/,1304/*vtotal*/};
//static LT8918_VIDEO_TIMING g_VidTiming = {54, 4, 72, 1920, 2050, 44, 4, 52, 480, 580};
//LT8918_VIDEO_TIMING g_VidTiming = {60, 20, 40, 1920, 2040, 10, 4, 6, 480, 500};

#if 0
static void LT8918_I2C_Enable(void)
{
	LT8918_SetRegBank(0x60);
	LT8918_WriteReg(0xee, 0x01); //cfg I2C enable
}

static void LT8918_CheckID(void)
{
	UINT32 id1, id2;

	LT8918_SetRegBank(0x60);
	LT8918_ReadReg(0x00, &id1); //ID1: 0x16
	LT8918_ReadReg(0x01, &id2); //ID2: 0x03
}
#endif

#if (LT8918_MODE == LT8918_PATTERN_MODE) //Pattern Mode
static void LT8918_DesscPll_Init(void)
{
	LT8918_SetRegBank(0x70);
	LT8918_WriteReg(0x38, 0x00);
	LT8918_SetRegBank(0x80);
	LT8918_WriteReg(0xa1, 0x00);
	LT8918_WriteReg(0xa5, 0xca);
	LT8918_WriteReg(0xa9, 0x96);
	LT8918_WriteReg(0xaa, 0x66);
	LT8918_WriteReg(0xab, 0x66);
	LT8918_WriteReg(0xac, 0x66);
	LT8918_WriteReg(0xad, 0x00);
	LT8918_WriteReg(0xad, 0x02);
}

static void LT8918_TxPll_Output_Config(void)
{
	UINT32 fm_value;
	UINT32 data;

	LT8918_SetRegBank(0x70);
	LT8918_WriteReg(0x30, 0x00);
	LT8918_WriteReg(0x31, 0x2c);
	LT8918_WriteReg(0x33, 0x14); //tx pll
	LT8918_WriteReg(0x34, 0x02);
	LT8918_WriteReg(0x24, 0x64);

	LT8918_WriteReg(0x23, 0x2f);
	LT8918_WriteReg(0x23, 0xaf);
	LT8918_WriteReg(0x23, 0x9f);

	LT8918_SetRegBank(0x80);
	LT8918_WriteReg(0x33, 0x09);
	Delay_DelayMs(10);
	LT8918_ReadReg(0x30, &data);
	fm_value = (data & 0x0f) << 8;
	LT8918_ReadReg(0x31, &data);
	fm_value += data;
	fm_value <<= 8;
	LT8918_ReadReg(0x32, &data);
	fm_value += data;
	//DBG_DUMP("^C MIPI Byte clock: %d\r\n", fm_value);
}

static void LT8918_TX_DPHY_Logic_Reset(void)
{
	LT8918_SetRegBank(0x60);
	LT8918_WriteReg(0x0d,0x00);
	Delay_DelayMs(5);
	LT8918_WriteReg(0x0d,0xff);
}

static void LT8918_PtnData_Config(void)
{
	LT8918_SetRegBank(0x80); //Tx pattern
	LT8918_WriteReg(0xbe, 0xc0);
	LT8918_WriteReg(0xbf, 0x02); //pattern select
	LT8918_WriteReg(0xc0, 0xff);
	LT8918_WriteReg(0xc1, (g_VidTiming.hs+g_VidTiming.hbp)/256);
	LT8918_WriteReg(0xc2, (g_VidTiming.hs+g_VidTiming.hbp)%256); //de_dly(HS+HBP):42
	LT8918_WriteReg(0xc3, (g_VidTiming.vs+g_VidTiming.vbp)%256); //de_top(VS+VBP):10
	LT8918_WriteReg(0xc4, g_VidTiming.hact/256); //Hactive[15:8]
	LT8918_WriteReg(0xc5, g_VidTiming.hact%256); //Hactive[7:0]--1200
	LT8918_WriteReg(0xc6, g_VidTiming.vact/256); //vact[15:8]
	LT8918_WriteReg(0xc7, g_VidTiming.vact%256); //vact[7:0]--1920
	LT8918_WriteReg(0xc8, g_VidTiming.htotal/256); //h_total[15:8]
	LT8918_WriteReg(0xc9, g_VidTiming.htotal%256); //h_total[7:0]:1274
	LT8918_WriteReg(0xca, g_VidTiming.vtotal/256); //v_total[15:8]
	LT8918_WriteReg(0xcb, g_VidTiming.vtotal%256); //v_total[7:0]:1938
	LT8918_WriteReg(0xcc, 0x00);
	LT8918_WriteReg(0xcd, g_VidTiming.hs%256); //Hsa:12
	LT8918_WriteReg(0xce, g_VidTiming.vs%256); //Vsa:2
}

static void LT8918_TxDPhy_Config(void)
{
	LT8918_SetRegBank(0x83); //MIPI TX PHY
	LT8918_WriteReg(0x40,0x00);

//	LT8918_WriteReg(0x41,0x08);
//	LT8918_WriteReg(0x42,0x08);
//	LT8918_WriteReg(0x43,0x08);
//	LT8918_WriteReg(0x44,0x08);

	LT8918_WriteReg(0x46,0x08);
	LT8918_WriteReg(0x47,0x08);
	LT8918_WriteReg(0x48,0x08);
	LT8918_WriteReg(0x4a,0x25);
}

static void LT8918_TxPrtcl_Config(void)
{
	LT8918_SetRegBank(0x83);
	LT8918_WriteReg(0x12, 0x01); //rd_dly[15:8]
	LT8918_WriteReg(0x13, 0x40); //rd_dly[7:0]FIFO(byte_clk)
	LT8918_WriteReg(0x14, g_VidTiming.vs%256); //vsa--2
	LT8918_WriteReg(0x15, g_VidTiming.vbp%256); //vbp--8
	LT8918_WriteReg(0x16, g_VidTiming.vact/256); //vact[15:8]
	LT8918_WriteReg(0x17, g_VidTiming.vact%256); //vact[7:0]--1920
	LT8918_WriteReg(0x18, g_VidTiming.vfp%256); //vfp--08

	LT8918_WriteReg(0x19, 0x1e);
	LT8918_WriteReg(0x1a, 0x1e);
	LT8918_WriteReg(0x1b, 0x1e);
	LT8918_WriteReg(0x1c, g_VidTiming.hact/256); //Hactive[15:8]
	LT8918_WriteReg(0x1d, g_VidTiming.hact%256); //Hactive[7:0]--1200
	LT8918_WriteReg(0x10, 0x18);
	LT8918_WriteReg(0x1f, 0x00);
	LT8918_WriteReg(0x11, 0x0c);

	//DBG_DUMP("^C MIPI video setting:\r\n");
	//DBG_DUMP("^C hfp %d, hs %d, hbp %d, hact %d, htotal %d\r\n",
	//	g_VidTiming.hfp, g_VidTiming.hs, g_VidTiming.hbp, g_VidTiming.hact, g_VidTiming.htotal);
	//DBG_DUMP("^C vfp %d, vs %d, vbp %d, vact %d, vtotal %d\r\n",
	//	g_VidTiming.vfp, g_VidTiming.vs, g_VidTiming.vbp, g_VidTiming.vact, g_VidTiming.vtotal);
}

static void LT8918_Pattern_Mode(void)
{
	LT8918_DesscPll_Init();
	LT8918_TxPll_Output_Config();
    LT8918_TX_DPHY_Logic_Reset();
	LT8918_PtnData_Config();
	LT8918_TxDPhy_Config();
	LT8918_TxPrtcl_Config();
}

#define  pat_num  64
const UINT8 pat_select[pat_num]={
0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,
0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f};

void LT8918_Pattern_cycle_test(void)
{
    UINT8 pnum=0, i;

	LT8918_SetRegBank(0x80);      //Tx pattern
	for (i=0; i<1; i++){
		for(pnum=0;pnum<=pat_num;pnum++){
			LT8918_WriteReg(0xbf,pat_select[pnum]); //pattern select
			Delay_DelayMs(1000);
		}
	}

}

#else // Normal Mode
static void vdLT8918L_MLRX_Init(void)
{
	LT8918_SetRegBank(0x70);//RX PHY
	LT8918_WriteReg(0x01,0x80);
	LT8918_WriteReg(0x03,0x48);
	LT8918_WriteReg(0x04,0xa2);
	LT8918_WriteReg(0x0c,0x80);
	LT8918_WriteReg(0x13,0x80);
	LT8918_WriteReg(0x18,0x50);
	LT8918_WriteReg(0x38,0xb0);
	LT8918_SetRegBank(0x80);
	LT8918_WriteReg(0x33,0x0C);
    LT8918_WriteReg(0x38,0x16);
}

static void vdLT8918L_Input_Config(void)
{
	LT8918_SetRegBank(0x80);
	LT8918_WriteReg(0x02,0x00);
}

static void vdLT8918L_RX_Reset(void)
{
	LT8918_SetRegBank(0x60);
	LT8918_WriteReg(0x10,0x00);
	Delay_DelayMs(5);
	LT8918_WriteReg(0x10,0xff);
}

static void vdLT8918L_TX_DPHY_Logic_Reset(void)
{
	LT8918_SetRegBank(0x60);
	LT8918_WriteReg(0x0d,0x00);
	Delay_DelayMs(5);
	LT8918_WriteReg(0x0d,0xff);
}

static void vdLT8918L_Video_check(void)
{
	UINT32 fm_value = 0, data;
	UINT16 htotal,vtotal;
	UINT16 hactive,vactive;
	UINT16 hsa,hbp,hfp;
	UINT8 vsa,vbp,vfp;
	UINT8 sync_polarity;

	//DBG_DUMP("vdLT8918L_Video_check: Begin\r\n");

	LT8918_SetRegBank(0x80);

	LT8918_WriteReg(0x33,0x0C);
    LT8918_WriteReg(0x38,0x16);
	Delay_DelayMs(10);
	LT8918_ReadReg(0x30, &data);
	fm_value = (data & 0x0f);
	fm_value = (fm_value<<8) ;
	LT8918_ReadReg(0x31, &data);
	fm_value = fm_value + data;
	fm_value = (fm_value<<8);
	LT8918_ReadReg(0x32, &data);
	fm_value = fm_value + data;

	DBG_DUMP("TTL pixel clock: fm_value = %d\r\n", fm_value);

	LT8918_ReadReg(0x42, &data);
	sync_polarity = data;

    LT8918_ReadReg(0x4e, &data);
	htotal = data & 0x0F;
	htotal <<= 8;
	LT8918_ReadReg(0x4f, &data);
	htotal |= data;

    LT8918_ReadReg(0x4c, &data);
	vtotal = data & 0x0F;
	vtotal <<= 8;
	LT8918_ReadReg(0x4d, &data);
	vtotal |= data;

    LT8918_ReadReg(0x52, &data);
	hactive = data & 0x0F;
	hactive <<= 8;
	LT8918_ReadReg(0x53, &data);
	hactive |= data;

    LT8918_ReadReg(0x50, &data);
	vactive = data & 0x0F;
	vactive <<= 8;
	LT8918_ReadReg(0x51, &data);
	vactive |= data;

    LT8918_ReadReg(0x44, &data);
	hsa = data & 0x0F;
	hsa <<= 8;
	LT8918_ReadReg(0x45, &data);
	hsa |= data;

    LT8918_ReadReg(0x48, &data);
	hbp = data & 0x0F;
	hbp <<= 8;
	LT8918_ReadReg(0x49, &data);
	hbp |= data;

    LT8918_ReadReg(0x4a, &data);
	hfp = data & 0x0F;
	hfp <<= 8;
    LT8918_ReadReg(0x4b, &data);
	hfp |= data;

    LT8918_ReadReg(0x43, &data);
	vsa = data;
	LT8918_ReadReg(0x46, &data);
	vbp = data;
	LT8918_ReadReg(0x47, &data);
	vfp = data;

	DBG_DUMP("^Cnsync_polarity = 0x%x\r\n", sync_polarity);

	DBG_DUMP("^Chfp = %d, hs = %d, hbp = %d, hact = %d, htotal = %d\r\n", hfp, hsa, hbp, hactive, htotal);

	DBG_DUMP("^Cvfp = %d, vs = %d, vbp = %d, vact = %d, vtotal = %d\r\n", vfp, vsa, vbp, vactive, vtotal);

	DBG_DUMP("vdLT8918L_Video_check: End\r\n");
}

static void vdLT8918L_TxPll_Output_Config(void)
{
	UINT32 fm_value;
	UINT32 data;

	LT8918_SetRegBank(0x70);
	LT8918_WriteReg(0x30, 0x00);
	LT8918_WriteReg(0x31, 0x2c);
	LT8918_WriteReg(0x33, 0x14); //tx pll
	LT8918_WriteReg(0x34, 0x02);
	LT8918_WriteReg(0x24, 0x64);

	LT8918_WriteReg(0x23, 0x2f);
	LT8918_WriteReg(0x23, 0xaf);
	LT8918_WriteReg(0x23, 0x9f);

	LT8918_SetRegBank(0x80);
	LT8918_WriteReg(0x33, 0x09);
	Delay_DelayMs(10);
	LT8918_ReadReg(0x30, &data);
	fm_value = (data & 0x0f) << 8;
	LT8918_ReadReg(0x31, &data);
	fm_value += data;
	fm_value <<= 8;
	LT8918_ReadReg(0x32, &data);
	fm_value += data;
	//DBG_DUMP("^C MIPI Byte clock: %d\r\n", fm_value);
}

static void vdLT8918L_TxDPhy_Config(void)
{
	UINT32 fm_value, data;
	UINT8 rdbyteclk ,wrhslpx ,wrhsprep ,wrhszero ,wrhstrail;
	UINT8 wrclkpost ,wrclktrail ,wrclkprpr, wrclkzero;

	LT8918_SetRegBank(0x80);
	LT8918_WriteReg(0x33,0x09);
	Delay_DelayMs(10);
	LT8918_ReadReg(0x30, &data);
	fm_value = (data & 0x0f);
	fm_value = (fm_value<<8);
	LT8918_ReadReg(0x31, &data);
	fm_value = fm_value + data;
	fm_value = (fm_value<<8) ;
	LT8918_ReadReg(0x32, &data);
	fm_value = fm_value + data;
	//DBG_DUMP("vdLT8918L_TxDPhy_Config, MIPI Byte clock: %d\r\n", fm_value);

	rdbyteclk = fm_value/1000;

	wrhslpx = rdbyteclk/20  ; // hs lpx > 50ns
	wrhsprep = rdbyteclk/18 + 2 ; //hs prep : (40ns + 4*UI)~(85ns + 6*UI)
	wrhszero = rdbyteclk/6+1; //hs zero > 145ns + 10UI - hsprep
	wrhstrail = rdbyteclk/10+8;

	wrclkpost = rdbyteclk/12 + 8;
	wrclktrail = rdbyteclk/12+1;
	wrclkprpr = rdbyteclk/25;
	wrclkzero = rdbyteclk/5;

	LT8918_SetRegBank(0x83); //MIPI TX PHY
	LT8918_WriteReg(0x40,0x00);

	LT8918_WriteReg(0x41,wrclkpost);//
	LT8918_WriteReg(0x42,wrclktrail);
	LT8918_WriteReg(0x43,wrclkprpr);
	LT8918_WriteReg(0x44,wrclkzero);

	LT8918_WriteReg(0x46,wrhslpx);
	LT8918_WriteReg(0x47,wrhsprep);
	LT8918_WriteReg(0x48,wrhstrail);
	LT8918_WriteReg(0x4a,wrhslpx + wrhsprep + wrhszero);
}

static  void vdLT8918L_TxPrtcl_Config(void)
{
	LT8918_SetRegBank(0x83);
	LT8918_WriteReg(0x12,0x00);  //rd_dly[15:8]
	LT8918_WriteReg(0x13,0xEA);  //rd_dly[7:0]FIFO(byte_clk)
	LT8918_WriteReg(0x14,g_VidTiming.vs%256);  //vsa
	LT8918_WriteReg(0x15,g_VidTiming.vbp%256); //vbp
	LT8918_WriteReg(0x16,g_VidTiming.vact/256); //vact[15:8]
	LT8918_WriteReg(0x17,g_VidTiming.vact%256); //vact[7:0]
	LT8918_WriteReg(0x18,g_VidTiming.vfp%256); //vfp
	LT8918_WriteReg(0x19,0x1e);
	LT8918_WriteReg(0x1a,0x1e);
	LT8918_WriteReg(0x1b,0x1e);
	LT8918_WriteReg(0x1c,g_VidTiming.hact/256);  //Hactive[15:8]
	LT8918_WriteReg(0x1d,g_VidTiming.hact%256);  //Hactive[7:0]
	LT8918_WriteReg(0x10,0x18);
	LT8918_WriteReg(0x1f,0x00);
	LT8918_WriteReg(0x11,0x0C);

	DBG_DUMP("vdLT8918L_TxPrtcl_Config: Begin\r\n");
	DBG_DUMP("^Cmipi video setting\n\r");
	DBG_DUMP("^Chfp = %d, hs = %d, hbp = %d, hact = %d, htotal = %d\r\n",
		       g_VidTiming.hfp, g_VidTiming.hs, g_VidTiming.hbp, g_VidTiming.hact, g_VidTiming.htotal);
	DBG_DUMP("^Cvfp = %d, vs = %d, vbp = %d, vact = %d, vtotal = %d\r\n",
		       g_VidTiming.vfp, g_VidTiming.vs, g_VidTiming.vbp, g_VidTiming.vact, g_VidTiming.vtotal);
	DBG_DUMP("vdLT8918L_TxPrtcl_Config: end\r\n");
}

void vdLT8918L_Normal_Mode(void)
{
	vdLT8918L_MLRX_Init();
	vdLT8918L_Input_Config();
	vdLT8918L_RX_Reset();
    Delay_DelayMs(200);
    vdLT8918L_Video_check();
	vdLT8918L_TX_DPHY_Logic_Reset();
	vdLT8918L_TxPll_Output_Config();
	vdLT8918L_TxDPhy_Config();
	vdLT8918L_TxPrtcl_Config();
}
#endif

tLCD_ROT *dispdev_getLcdRotateCmdParal(UINT32 *ModeNumber)
{
	if (tRotateParal != NULL) {
		*ModeNumber = sizeof(*tRotateParal) / sizeof(tLCD_ROT);
	} else {
		*ModeNumber = 0;
	}
	return (tLCD_ROT *)tRotateParal;
}

tLCD_PARAM *dispdev_getConfigModeParal(UINT32 *ModeNumber)
{
	*ModeNumber = sizeof(tModeParal) / sizeof(tLCD_PARAM);
	return (tLCD_PARAM *)tModeParal;
}

tPANEL_CMD *dispdev_getStandbyCmdParal(UINT32 *CmdNumber)
{
	*CmdNumber = sizeof(tCmdStandbyParal) / sizeof(tPANEL_CMD);
	return (tPANEL_CMD *)tCmdStandbyParal;
}

#if 1
void dispdev_writeToLcdSifParal(DISPDEV_IOCTRL pDispDevControl, UINT32 addr, UINT32 value)
{
	UINT32                  uiSIFData;
	DISPDEV_IOCTRL_PARAM    DevIOCtrl;

	uiSIFData = ((((UINT32)addr << 9) | (UINT32)value) << 16);

	pDispDevControl(DISPDEV_IOCTRL_GET_REG_IF, &DevIOCtrl);
	sif_send(DevIOCtrl.SEL.GET_REG_IF.uiSifCh, uiSIFData, 0, 0);

}


void dispdev_writeToLcdGpioParal(DISPDEV_IOCTRL pDispDevControl, UINT32 addr, UINT32 value)
{
#if 1
	static BOOL bFirstInit = TRUE;
	if (bFirstInit)
	{
	      gpio_setDir(P_GPIO_12, GPIO_DIR_OUTPUT);
		gpio_clearPin(P_GPIO_12); // for LCD reset. L --> reset
		Delay_DelayMs(5);
		gpio_setPin(P_GPIO_12);
		Delay_DelayMs(5);
	
        #if (LT8918_MODE == LT8918_PATTERN_MODE) //Pattern Mode
        LT8918_Init(); // only i2c enable
        Panel_Init();
        LT8918_Pattern_Mode();
        #else
		LT8918_Init(); // only i2c enable
		vdLT8918L_Normal_Mode();
        #endif

		bFirstInit = FALSE;
	}
#endif
}

void dispdev_readFromLcdGpioParal(DISPDEV_IOCTRL pDispDevControl, UINT32 addr, UINT32 *p_value)
{
	DBG_ERR("Has not implement dispdev_readFromLcdGpioParal \r\n");
}
#endif
