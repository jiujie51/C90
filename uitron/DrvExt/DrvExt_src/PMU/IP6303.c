/*
 * Battery s_stCharger driver for X-Powers AXP
 *
 * Copyright (C) 2013 X-Powers, Ltd.
 *  Zhang Donglu <zhangdonglu@x-powers.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */


#include "axp-sply.h"
#include "axp-cfg.h"
//#include "usart1.h"	//打印函数头文件
#include "Delay.h"
#include "i2c.h"

#define __MODULE__    axpsply
#define __DBGLVL__    1 // 0=OFF, 1=ERROR, 2=TRACE
#define __DBGFLT__    "*" // *=All, [mark1]=CustomClass
#include "DebugModule.h"

#if defined(_PMU_IP6303_)

//VBAT = VBATADC*15.625+500+0.5*15.625 (mV)
//IBAT = (IBATADC*15.625-1100+0.5*15.625)/0.495 (mA)
//ICHG = (ICHGADC*15.625-750+0.5*15.625)/3 (mA)
//VGP1 = GP1ADC *15.625+500+0.5*15.625 (mV)

static struct axp_charger s_stCharger;
static BOOL s_axp_debug_ctr = FALSE;
static BOOL s_iSysshutdownKeepPower = FALSE;

static POWERIC_INFO g_PowerICInfo;

//extern BOOL SysInit_GetMTPW(void);
BOOL PowerIC_I2C_Init(POWERIC_INFO PowerICInfo)
{
    UINT erReturn;
    g_PowerICInfo.I2C_RegBytes = PowerICInfo.I2C_RegBytes;
    g_PowerICInfo.I2C_PinMux = PowerICInfo.I2C_PinMux;
    g_PowerICInfo.I2C_BusClock = PowerICInfo.I2C_BusClock;
    g_PowerICInfo.I2C_Slave_WAddr = PowerICInfo.I2C_Slave_WAddr;
    g_PowerICInfo.I2C_Slave_RAddr = PowerICInfo.I2C_Slave_RAddr;
	if(1)//(SysInit_GetMTPW())
	{
	    if (g_PowerICInfo.I2C_RegBytes > 2)
	    {
	        debug_msg("Error register Byte Count !!\r\n");
	        return FALSE;
	    }

	    if (g_PowerICInfo.I2C_PinMux > I2C_PINMUX_2ND)
	    {
	        debug_msg("Error I2C Pin mux !!\r\n");
	        return FALSE;
	    }

	    if (g_PowerICInfo.I2C_BusClock > I2C_BUS_CLOCK_1MHZ)
	    {
	        debug_msg("Error I2C Bus Clock !!\r\n");
	        return FALSE;
	    }
	    erReturn = i2c3_open(&(g_PowerICInfo.I2C_Channel));
	    if(erReturn != I2C_STS_OK)
	    {
	        debug_msg("Error open I2C driver!!\r\n");
	        return FALSE;
	    }
		DBG_DUMP("^C g_PowerICInfo.I2C_Channel = %d\r\n", g_PowerICInfo.I2C_Channel);
		#if 0
	    i2c_setConfig(g_PowerICInfo.I2C_Channel, I2C_CONFIG_ID_MODE,        I2C_MODE_MASTER);
	    i2c_setConfig(g_PowerICInfo.I2C_Channel, I2C_CONFIG_ID_PINMUX, g_PowerICInfo.I2C_PinMux); //select to control I2C 1st device
	    i2c_setConfig(g_PowerICInfo.I2C_Channel, I2C_CONFIG_ID_BUSCLOCK, g_PowerICInfo.I2C_BusClock); //up to 400KHZ
		#else
		i2c3_setConfig(g_PowerICInfo.I2C_Channel, I2C_CONFIG_ID_MODE,        I2C_MODE_MASTER);
		i2c3_setConfig(g_PowerICInfo.I2C_Channel, I2C_CONFIG_ID_BUSCLOCK,    I2C_BUS_CLOCK_200KHZ);
		#endif
	    return TRUE;
	}
	else
		return FALSE;
}


static void PowerIC_I2C_WriteReg(UINT32 uiAddr, UINT32 uiValue)
{
	I2C_DATA i2cData;
	I2C_BYTE i2cByte[3];

	i2cData.VersionInfo = DRV_VER_96660;
	i2cData.pByte       = i2cByte;
	i2cData.ByteCount   = 3;
	i2cByte[0].uiValue  = g_PowerICInfo.I2C_Slave_WAddr;    // slave address
	i2cByte[0].Param    = I2C_BYTE_PARAM_START;
	i2cByte[1].uiValue  = uiAddr;
	i2cByte[1].Param    = I2C_BYTE_PARAM_NONE;
	i2cByte[2].uiValue  = uiValue;
	i2cByte[2].Param    = I2C_BYTE_PARAM_STOP;

	// Lock I2C
	i2c3_lock(g_PowerICInfo.I2C_Channel);
	if (i2c3_transmit(&i2cData) != I2C_STS_OK) {
		DBG_ERR("i2c_transmit err\r\n");
	}

	// Unlock I2C
	i2c3_unlock(g_PowerICInfo.I2C_Channel);
}

static UINT32 PowerIC_I2C_ReadReg(UINT32 uiReg)
{
	I2C_DATA i2cData;
	I2C_BYTE i2cByte[3];
	I2C_STS ret;
	static UINT32 ulData;

	i2cData.VersionInfo = DRV_VER_96660;
	i2cData.pByte       = i2cByte;
	i2cData.ByteCount   = 3;
	i2cByte[0].uiValue  = g_PowerICInfo.I2C_Slave_WAddr;    // slave address
	i2cByte[0].Param    = I2C_BYTE_PARAM_START;
	i2cByte[1].uiValue  = uiReg;
	i2cByte[1].Param    = I2C_BYTE_PARAM_NONE;
	i2cByte[2].uiValue  = g_PowerICInfo.I2C_Slave_RAddr;//AXP_DEVICES_ADDR | 0x01;    // slave address with READ
	i2cByte[2].Param    = I2C_BYTE_PARAM_START;
	// Lock I2C
	i2c3_lock(g_PowerICInfo.I2C_Channel);

	do {
		if (i2c3_transmit(&i2cData) != I2C_STS_OK) {
			break;
		}

		i2cData.ByteCount = 1;
		i2cByte[0].Param = I2C_BYTE_PARAM_STOP | I2C_BYTE_PARAM_NACK;
		ret = i2c3_receive(&i2cData);
		if (ret != I2C_STS_OK) {
			DBG_ERR("i2c_receive err\r\n");
		}

	} while (0);
	// Unlock I2C
	i2c3_unlock(g_PowerICInfo.I2C_Channel);
	ulData = i2cByte[0].uiValue;
		return ulData;
}

/*IP6303 读函数配置*/
static uint8_t IP6303_read( uint8_t readAddr, uint8_t byteNum , uint8_t *readData)
{
	uint8_t i;
	if(byteNum<=0)
		return 0;

	/*此处，填写用户定义的I2C读函数*/
		//I2C_Read(AXP_DEVICES_ADDR,readData, readAddr, byteNum);

	for(i=0;i<byteNum;i++)
	{
		readData[i] = PowerIC_I2C_ReadReg(readAddr);
		//debug_msg("AXP216_read Addr=0x%x  : 0x%x,%d\r\n",readAddr,readData[i],i);
		readAddr++;
	}
 	   return byteNum;
}



/*IP6301 写函数配置	*/
static uint8_t IP6303_write(const uint8_t writeAddr, const uint8_t writeData)
{
    /*此处，填写用户定义的I2C 写函数*/
	PowerIC_I2C_WriteReg(writeAddr,writeData);//I2C_Write(AXP_DEVICES_ADDR,writeAddr,writeData);

	return 1;
}


static void IP6303_set_bits(int reg, uint8_t bit_mask)
{
	uint8_t reg_val;

	IP6303_read( reg,1, &reg_val);

	if ((reg_val & bit_mask) != bit_mask) {
		reg_val |= bit_mask;
		IP6303_write(reg, reg_val)	 ;

   		}

}
static void  IP6303_clr_bits(int reg, uint8_t bit_mask)
{
	uint8_t reg_val;

	IP6303_read( reg,1, &reg_val);

	if (reg_val & bit_mask) {
		reg_val &= ~bit_mask;
		IP6303_write(reg, reg_val);
	}

}

#if 0
static void IP6303_set_gps_ldo_vol(uint8_t onoff)
{
			//0.7~3.3v,100mV/step
//ALDO5 = 3.3V
	if(onoff)
	{
		//LDO VOL
		IP6303_write( AXP_LDO5_VSEL, 0x68);  // 3.3v An=0.7+nx0.025
		//LDO Ctr
	    	IP6303_set_bits(AXP_LDO_EN, 0x40);		//BIT1:ELDO2
	}
	else
	{
		IP6303_clr_bits(AXP_LDO_EN, 0x00);		//BIT1:ELDO2
	}
}
#endif

#if 1
static void IP6303_set_wifi_dcdc_vol(uint8_t onoff)
{
	//0.6~3.6v,12.5mV/step
	//DCDC2 = 3.3V
	if(onoff)
	{
		//DCDC2 VOL
		IP6303_write( AXP_DC2_VSEL, 0xD8);  // 3.3v V=Vset*12.5mv+0.6
		//LDO Ctr
	    	IP6303_set_bits(AXP_DCDC_CTL, 0x02);		//BIT1:DCDC2
	}
	else
	{
		IP6303_clr_bits(AXP_LDO_EN, 0x02);		//BIT1:DCDC2
	}
}
#endif

#if 0
static void IP6303_set_sensor_ldo_vol(void)
{
	//0.7~3.3v,25mV/step
	//LDO3 = 2.8V
	//LDO6 = 1.8V
	//LDO5 = 1.8V
	//LDO VOL
#if 0
	IP6303_write( AXP_LDO3_VSEL, 84);  //LDO3,2.8V(前路2.8V)
	IP6303_write( AXP_LDO6_VSEL, 44);  //LDO6,1.8V(前路1.8V)

	IP6303_write( AXP_LDO5_VSEL, 44);  //LDO5,1.8V(后路1.8V)
	//LDO Ctr
    //IP6303_set_bits(AXP_LDO_EN, 0x48); //BIT6:LDO6  BIT3:LDO3
    IP6303_set_bits(AXP_LDO_EN, 0x68); //BIT6:LDO6 BIT5:LDO5 BIT3:LDO3
#endif
}
#endif

//前路1.8V
void IP6303_set_sensor_power18v(void)
{
	//0.7~3.3v,25mV/step
	//LDO6 = 1.8V

	//LDO VOL
	IP6303_write( AXP_LDO6_VSEL, 44);  //LDO6,1.8V
	//LDO Ctr
    IP6303_set_bits(AXP_LDO_EN, 0x40); //BIT6:LDO6

}
//前路2.8V
void IP6303_set_sensor_power28v(void)
{
	//0.7~3.3v,25mV/step
	//LDO3 = 2.8V

	//LDO VOL
	IP6303_write( AXP_LDO3_VSEL, 84);  //LDO3,2.8V
	//LDO Ctr
    IP6303_set_bits(AXP_LDO_EN, 0x08); //BIT3:LDO3

}
 //后路1.8V
void IP6303_set_sensor2_power18v(void);
void IP6303_set_sensor2_power18v(void)
{
	//0.7~3.3v,25mV/step
	//LDO5 = 1.8V

	//LDO VOL
	IP6303_write( AXP_LDO5_VSEL, 44);  //LDO5,1.8V
	//LDO Ctr
    IP6303_set_bits(AXP_LDO_EN, 0x20); //BIT5:LDO5
}

#if 0 // 190226, dcj shrink here for compile error
static void  axp_clr_bits(int reg, uint8_t bit_mask)
{
	 uint8_t reg_val;

     IP6303_read( reg,1, &reg_val);

	if (reg_val & bit_mask) {
		reg_val &= ~bit_mask;
		IP6301_write(reg, reg_val)	 ;
    	}

}

static void  axp_update(int reg, uint8_t val, uint8_t mask)
{

	uint8_t reg_val;

	IP6303_read(reg, 1, &reg_val);


	if ((reg_val & mask) != val) {
		reg_val = (reg_val & ~mask) | val;
    	IP6301_write(reg, reg_val);
	 }
}
#endif


//static int pre_batt_max_cap=0;
//static void axp_set_charge(void);

void set_axp_IP6303_debug(BOOL flag)
{
	s_axp_debug_ctr = flag;
}

void shutdown_IP6303_no_power(void)
{
	s_iSysshutdownKeepPower = FALSE;
}
void shutdown_IP6303_keep_power(void)
{
	s_iSysshutdownKeepPower = TRUE;
}
int IP6303_get_current_iusb(void)
{
	return s_stCharger.iusb;
}
static void IP6303_set_current_iusb(int value)
{
	s_stCharger.iusb = value;
}
static void IP6303_charger_update_state(struct axp_charger *s_stCharger)
{
    uint8_t val;

    IP6303_read(AXP_CHG_DIG_CTL2,1,&val);
    s_stCharger->bat_det = (val & AXP_BAT_EXIST)?1:0;
	if(1)//pqw//( s_stCharger->bat_det )//不做电池存在检测,有问题
	{
		//VBAT = VBATADC*15.625+500+0.5*15.625 (mV)
		IP6303_read( AXP_VBAT,1, &val);
		
		//s_stCharger->rest_vol	= (int)	(val & 0x7F);
		s_stCharger->rest_vol	= (int)	(val & 0xFF);

	    s_stCharger->ocv_rest_vol = (int)((float)val*15.625+500+0.5*16.625);
		s_stCharger->vbat = s_stCharger->ocv_rest_vol;

		//IBAT = (IBATADC*15.625-1100+0.5*15.625)/0.495 (mA)
		IP6303_read( AXP_IBAT,1, &val);
	    s_stCharger->ibat = (int)(((float)val*15.625-1100+0.5*15.625)/0.495);

		//ICHG = (ICHGADC*15.625-750+0.5*15.625)/3 (mA)
		IP6303_read( AXP_ICHG,1, &val);
	    s_stCharger->iusb = (int)(((float)val*15.625-750+0.5*15.625)/3);
		IP6303_set_current_iusb(s_stCharger->iusb);

		//VGP1 = GP1ADC *15.625+500+0.5*15.625 (mV)
	}
}

int IP6303_get_bat_valtage(void)
{
	uint8_t	val;
	int nRet = 0;

	//if( s_stCharger.bat_det )
	{
		//VBAT = VBATADC*15.625+500+0.5*15.625 (mV)
		IP6303_read( AXP_VBAT,1, &val);
		//debug_msg(" ------------->>val=0x%x    \r\n",val);
		//s_stCharger.rest_vol	= (int)	(val & 0x7F);
		s_stCharger.rest_vol	= (int)	(val & 0xFF);
		//MY_MSG1("val: %d",val);
		//debug_msg("\r\n s_stCharger.rest_vol:%dv\r\n",s_stCharger.rest_vol);

	    //mv
	    s_stCharger.ocv_rest_vol = (int)((float)val*15.625+500+0.5*16.625);
		//MY_MSG1("s_stCharger.ocv_rest_vol: %d",s_stCharger.ocv_rest_vol);
		//debug_msg("\r\n s_stCharger.ocv_rest_vol:%dv\r\n",s_stCharger.ocv_rest_vol);

		s_stCharger.vbat = s_stCharger.ocv_rest_vol;
		//debug_msg("\r\n s_stCharger.vbat:%dv\r\n",s_stCharger.vbat);
		nRet = s_stCharger.vbat;
	}
	return nRet;
}

BOOL IP6303_get_bat_status(void)
{
	BOOL nRet = FALSE;

	if( s_stCharger.bat_det )
	{
	    nRet = TRUE;
	}
	return nRet;
}
//使能 BAT 存在检测
void IP6303_set_batext_check(BOOL nValue)
{
	if(nValue)
	{
		IP6303_set_bits(AXP_PSTATE_CTL3, 0x01); //使能电池检测
	}
	else
	{
		IP6303_clr_bits(AXP_PSTATE_CTL3, 0x01);//关闭电池检测
	}
}

void IP6303_set_charge(BOOL nValue)
{
	uint8_t val=0x00;
	//uint8_t tmp=0x00;
	//uint8_t CHG_EN;
	//int chgpretime,chgcsttime,chgcur ;
    if( nValue )
    {
		IP6303_read(AXP_CHG_DIG_CTL3,1,&val);
		debug_msg("---VA: 0x%x---\r\n",val);	
		IP6303_set_bits(AXP_CHG_DIG_CTL3, AXP_CHG_EN); //bit2=1,充电使能打开
		IP6303_read(AXP_CHG_DIG_CTL3,1,&val);
		debug_msg("AXP_CHG_EN--->>IP6303 Reg 0x58: 0x%x---\r\n",val);	
    }
	else
	{
		IP6303_read(AXP_CHG_DIG_CTL3,1,&val);
		debug_msg("---VA: 0x%x---\r\n",val);	
		IP6303_clr_bits(AXP_CHG_DIG_CTL3, AXP_CHG_EN); //bit2=0,充电使能关掉
		IP6303_read(AXP_CHG_DIG_CTL3,1,&val);
		debug_msg("AXP_CHG_DIS--->>IP6303 Reg 0x58: 0x%x---\r\n",val);	
	}

}

void IP6303_init(void)
{
	uint8_t	val;
    POWERIC_INFO PowerICInfo;

    PowerICInfo.I2C_RegBytes = POWERIC_I2C_REGISTER_1BYTE;
    PowerICInfo.I2C_PinMux = I2C_PINMUX_2ND;
    PowerICInfo.I2C_BusClock = I2C_BUS_CLOCK_200KHZ;//I2C_BUS_CLOCK_400KHZ;
    // PowerIC I2C slave addresses
    PowerICInfo.I2C_Slave_WAddr = AXP_DEVICES_ADDR_W;
    PowerICInfo.I2C_Slave_RAddr = (AXP_DEVICES_ADDR_R);
    if(PowerIC_I2C_Init(PowerICInfo) == FALSE)
    {
        debug_msg("PowerIC IIC initial Fail!!!\r\n");
    }

	//IP6303_set_sensor_ldo_vol();
	IP6303_set_sensor2_power18v();
	IP6303_set_wifi_dcdc_vol(TRUE);

	IP6303_write(0x00, 0x50);
	IP6303_read(0x00,1,&val);
	debug_msg("\r\nIP6303_init---0x00=0x%x---\r\n",val);

	IP6303_set_bits(0x01, 0x20);  // WKIRQ_POL 上升沿触发
	//IP6303_read(0x10,1,&val);
	//debug_msg(" IP6303_init------------->>0x10=0x%x    \r\n",val);

	//IP6303_read(0x11,1,&val);
	//debug_msg(" IP6303_init------------->>0x11=0x%x    \r\n",val);
	IP6303_read(AXP_RESERVE_000,1,&val);
	//IP6303_write(AXP_RESERVE_000, (val | 0x80) ); //
	IP6303_clr_bits(AXP_RESERVE_000, 0x80);//0x4c[7] = 0
	IP6303_read(AXP_RESERVE_000,1,&val);

	//IP6303_read(0x09,1,&val);
	//debug_msg("IP6303_init--->>Reg:0x09=0x%x---\r\n",val);

       //ENALBE battery detection
	//IP6303_read(AXP_PSTATE_CTL3,1,&val);
	//IP6303_write(AXP_PSTATE_CTL3, AXP_EN_BATEXT_DT); //
	//IP6303_read(AXP_PSTATE_CTL3,1,&val);

       //ENALBE all adc
	IP6303_read(AXP_ADC_ANA_CTL0,1,&val);
	IP6303_write(AXP_ADC_ANA_CTL0, 0x5f); //enable battery exist detetion
	IP6303_read(AXP_ADC_ANA_CTL0,1,&val);

	//CHG_ANA_CTL0(0x50):关闭过压保护
	IP6303_write(AXP_CHG_ANA_CTL0, 0x25);

       //set charge current
	IP6303_read(AXP_CHG_DIG_CTL0,1,&val);
	//debug_msg("\r\n---VA: %x---\r\n",val);
	IP6303_write(AXP_CHG_DIG_CTL0, ((val & 0xF0) | AXP_CHG_CURRENT)); //
	IP6303_read(AXP_CHG_DIG_CTL0,1,&val);
	//debug_msg("\r\n---Reg 0x53--->>VA: %x---\r\n",val);

	//set AXP_PROTECT_CTL5
	IP6303_read(AXP_PROTECT_CTL5,1,&val);
	//debug_msg("\r\n\r\n---Reg 0x97:0X%x---\r\n",val);
	IP6303_set_bits(AXP_PROTECT_CTL5, 0x04);//设置低电保护
	IP6303_read(AXP_PROTECT_CTL5,1,&val);
	//debug_msg("\r\n---Reg 0x97 :0X%x---\r\n\r\n",val);

	//set BATOK
	IP6303_read(AXP_PSTATE_CTL2,1,&val);
	//debug_msg("\r\n\r\n---AXP_PSTATE_CTL2_a:%x---\r\n",val);
	IP6303_write(AXP_PSTATE_CTL2, (val | 0x0C)); //
	IP6303_read(AXP_PSTATE_CTL2,1,&val);
	//debug_msg("\r\n---AXP_PSTATE_CTL2_b:%x---\r\n\r\n",val);
	IP6303_set_batext_check(TRUE);
    Delay_DelayMs(50);
	IP6303_set_charge(TRUE); // 充电使能
	IP6303_write(0x56,0x01); // NTC使能,电池温度检测使能
	IP6303_write(0x57,0x02); //
	IP6303_charger_update_state(&s_stCharger);
}

void IP6303_poweroff(void)
{
	CHKPNT;
	uint8_t	val;

	IP6303_clr_bits(0x00,0x08); //bit 3 写0
	IP6303_read(0x00,1,&val);
	debug_msg("\r\n---0x00val1:%x---\r\n",val);
	IP6303_read(0x04,1,&val);
	debug_msg("\r\n---0x04val1:%x---\r\n\r\n",val);

	IP6303_set_bits(0x04,0x80); 	//bit7,power off delay(delay 8ms), 1： 延时 8ms 后，才开始掉电
	//dcj shrink here 190313, for can not open device by power key after power off
	//IP6301_set_bits(0x00,0x01); 	//bit0=1,pmu power off enable(POFF_EN=1), 1: 自动进 S2|S3,
	IP6303_read(0x00,1,&val);
	debug_msg("\r\n---0x00val2:0x%x---\r\n",val);
	IP6303_read(0x04,1,&val);
	debug_msg("\r\n---0x04val2:0x%x---\r\n",val);
	if(!s_iSysshutdownKeepPower)
	{
		debug_msg(" \r\n ======IP6303_poweroff !!!=====  \r\n");
		IP6303_set_bits(0x00,0x01); //软件写PMU关机
	}
}

void IP6303_test(void)
{
	uint8_t	val =0;
	IP6303_read(0x00,1,&val);
	debug_msg("\r\n---0x00val:%x---\r\n",val);

	IP6303_write(0x00, 0x00);
	IP6303_read(0x00,1,&val);
	debug_msg("\r\n---0x00val2:%x---\r\n",val);

	IP6303_write(0x00, 0x18);
	IP6303_read(0x00,1,&val);
	debug_msg("\r\n---0x00val3:%x---\r\n",val);

	//IP6303_read(0x01,1,&val);
	//debug_msg("\r\n---0x01val:%x---\r\n",val);
	//IP6303_read(0x02,1,&val);
	//debug_msg("\r\n---0x02val:%x---\r\n",val);
	//IP6303_read(0x03,1,&val);
	//debug_msg("\r\n---0x03val:%x---\r\n",val);
	//IP6303_read(0x04,1,&val);
	//debug_msg("\r\n---0x04val:%x---\r\n",val);
}

struct axp_charger  IP6303_charging_monitor(void)
{
	//uint8_t	val;//,temp_val[4];

	//uint8_t	batt_max_cap_val[3];
	//int	batt_max_cap,coulumb_counter;

	IP6303_charger_update_state(&s_stCharger);
    	IP6303_get_bat_valtage();

	return  s_stCharger;
}

#endif

