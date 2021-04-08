#ifndef	_LINUX_AXP_SPLY_H_
#define	_LINUX_AXP_SPLY_H_

#if defined(__ECOS)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "Type.h"
#include "i2c.h"

#if (defined(_PMU_AXP_))
/*      AXP 216    */
#define AXP_CHARGE_STATUS		AXP_STATUS
#define AXP_IN_CHARGE			(1 << 6)
#define AXP_PDBC			(0x32)
#define AXP_CHARGE_CONTROL1		AXP_CHARGE1
#define AXP_CHARGER_ENABLE		(1 << 7)
#define AXP_CHARGE_CONTROL2		AXP_CHARGE2
#define AXP_CHARGE_VBUS		AXP_IPS_SET
#define AXP_CAP			(0xB9)
#define AXP_BATCAP0			(0xe0)
#define AXP_BATCAP1			(0xe1)
#define AXP_RDC0			(0xba)
#define AXP_RDC1			(0xbb)
#define AXP_WARNING_LEVEL		(0xe6)
#define AXP_ADJUST_PARA		(0xe8)
#define AXP_FAULT_LOG1		AXP_MODE_CHGSTATUS
#define AXP_FAULT_LOG_CHA_CUR_LOW	(1 << 2)
#define AXP_FAULT_LOG_BATINACT	(1 << 3)
#define AXP_FAULT_LOG_OVER_TEMP	(1 << 7)
#define AXP_FAULT_LOG2		AXP_INTSTS2
#define AXP_FAULT_LOG_COLD		(1 << 0)
#define AXP_FINISH_CHARGE		(1 << 2)
#define AXP_COULOMB_CONTROL		AXP_COULOMB_CTL
#define AXP_COULOMB_ENABLE		(1 << 7)
#define AXP_COULOMB_SUSPEND		(1 << 6)
#define AXP_COULOMB_CLEAR		(1 << 5)

#define AXP_ADC_CONTROL				AXP_ADC_EN
#define AXP_ADC_BATVOL_ENABLE				(1 << 7)
#define AXP_ADC_BATCUR_ENABLE				(1 << 6)
#define AXP_ADC_DCINVOL_ENABLE			(1 << 5)
#define AXP_ADC_DCINCUR_ENABLE			(1 << 4)
#define AXP_ADC_USBVOL_ENABLE				(1 << 3)
#define AXP_ADC_USBCUR_ENABLE				(1 << 2)
#define AXP_ADC_APSVOL_ENABLE				(1 << 1)
#define AXP_ADC_TSVOL_ENABLE				(1 << 0)
#define AXP_ADC_INTERTEM_ENABLE			(1 << 7)
#define AXP_ADC_GPIO0_ENABLE				(1 << 3)
#define AXP_ADC_GPIO1_ENABLE				(1 << 2)
#define AXP_ADC_GPIO2_ENABLE				(1 << 1)
#define AXP_ADC_GPIO3_ENABLE				(1 << 0)
#define AXP_ADC_CONTROL3				(0x84)
#define AXP_VBATH_RES					(0x78)
#define AXP_VTS_RES					(0x58)
#define AXP_VBATL_RES					(0x79)
#define AXP_OCVBATH_RES				(0xBC)
#define AXP_OCVBATL_RES				(0xBD)
#define AXP_INTTEMP					(0x56)
#define AXP_DATA_BUFFER0				AXP_BUFFER1
#define AXP_DATA_BUFFER1				AXP_BUFFER2
#define AXP_DATA_BUFFER2				AXP_BUFFER3
#define AXP_DATA_BUFFER3				AXP_BUFFER4
#define AXP_DATA_BUFFER4				AXP_BUFFER5
#define AXP_DATA_BUFFER5				AXP_BUFFER6
#define AXP_DATA_BUFFER6				AXP_BUFFER7
#define AXP_DATA_BUFFER7				AXP_BUFFER8
#define AXP_DATA_BUFFER8				AXP_BUFFER9
#define AXP_DATA_BUFFER9				AXP_BUFFERA
#define AXP_DATA_BUFFERA				AXP_BUFFERB
#define AXP_DATA_BUFFERB				AXP_BUFFERC
#endif

#if (defined(_PMU_IP6303_))
/*    ip6303   */
/*设备地址*/
/*
	一般不改变：
	IP6303: 写地址 ：0x60	读地址：0x61
*/
#define	AXP_DEVICES_ADDR_W	0x60
#define	AXP_DEVICES_ADDR_R	0x61


/*      AXP      */
//IP6303 values
#define AXP_BAT_FULL               (4150)
#define AXP_BAT_NEED_CHG           (4050)

#define AXP_CHG_275MA               (10)
#define AXP_CHG_300MA               (11)
#define AXP_CHG_CURRENT             AXP_CHG_275MA

#define AXP_BAT_EXIST               (0X08)

#define AXP_EN_BATEXT_DT            (0X01)
#define AXP_CHG_EN                  (0X02)
#define AXP_CHG_DIS                 (0X00)

#define AXP_VBAT_ADC_EN             (0X01)
#define AXP_IBAT_ADC_EN             (0X02)
#define AXP_ICHG_ADC_EN             (0X04)
#define AXP_GP1_ADC_EN              (0X08)
#define AXP_GP2_ADC_EN              (0X10)


#define AXP_EN_ADC_ALL              (AXP_VBAT_ADC_EN | AXP_IBAT_ADC_EN | AXP_ICHG_ADC_EN | AXP_GP1_ADC_EN|AXP_GP2_ADC_EN)

//////////////////////////////////////////////////////
//IP6303 registers 
#define AXP_VBAT					(0x64)
#define AXP_IBAT					(0x65)
#define AXP_ICHG					(0x66)
#define AXP_PSTATE_CTL2				(0x02)
#define AXP_PSTATE_CTL3             (0x03)
#define AXP_CHG_DIG_CTL2            (0x55)
#define AXP_ADC_ANA_CTL0			(0x60)
#define AXP_CHG_DIG_CTL0            (0x53)
#define AXP_CHG_DIG_CTL3            (0x58)
#define AXP_CHG_ANA_CTL0			(0x50)

#define AXP_PROTECT_CTL5			(0x97)

#define AXP_LDO_EN         (0X40)
#define AXP_LDOSW_EN         (0X41)

#define AXP_LDO2_VSEL        (0X42)
#define AXP_LDO3_VSEL        (0X43)
#define AXP_LDO4_VSEL        (0X44)
#define AXP_LDO5_VSEL        (0X45)
#define AXP_LDO6_VSEL        (0X46)
#define AXP_LDO7_VSEL        (0X47)

#define AXP_LDO_CTL0          (0X48)
#define AXP_LDO_CTL1          (0X49)
#define AXP_LDO_CTL2          (0X4A)
#define AXP_SLDO_VSEL          (0X4D)

#define AXP_DCDC_CTL	      (0X20)
#define AXP_DC1_VSEL        (0X21)
#define AXP_DC2_VSEL        (0X26)
#define AXP_DC3_VSEL        (0X2B)


//for detect battery, set bit7 to 1, keep the other bit unchanged
#define AXP_RESERVE_000             (0x4c)
#endif

#define uint8_t         UINT8
#define uint16_t        UINT16

struct axp_adc_res {//struct change
	uint16_t vbat_res;
	uint16_t ocvbat_res;
	uint16_t ibat_res;
	uint16_t ichar_res;
	uint16_t idischar_res;
	uint16_t vac_res;
	uint16_t iac_res;
	uint16_t vusb_res;
	uint16_t iusb_res;
	uint16_t ts_res;
};



struct axp_charger {

	/* adc */
	struct axp_adc_res *adc;   //ADC数据


	/* charger status */
	uint8_t bat_det;		 //电池存在
	uint8_t is_on;		 //充电指示
	uint8_t ac_det;		 //AC存在
	uint8_t usb_det;		  //USB存在
	uint8_t ac_valid;	   //AC可用
	uint8_t usb_valid;	   //usb可用
	uint8_t ext_valid;	  //外部电源存在
	uint8_t bat_current_direction; //电池电流方向
	uint8_t in_short;			 //AC&USB是否短接
	uint8_t batery_active;
	uint8_t int_over_temp;		//芯片是否过温
	uint8_t fault;
	int charge_on;

	int vbat;					//电池电压
	int ibat;				    //电池电流
	int vac;
	int iac;
	int vusb;
	int iusb;
	int ocv;				  //电池开路电压

	int disvbat;
	int disibat;

	/*rest time*/
	int rest_vol;				  //电池电量
	int ocv_rest_vol;		      //OCV电量
	int base_restvol;
	int rest_time;

	/*ic temperature*/
	int ic_temp;				  //电池温度
};

#if (defined(_PMU_AXP_))
/*AXP 216*/
extern void AXP216_init(void);
extern struct  axp_charger axp_charging_monitor(void);
extern void set_gps_ldo_vol(uint8_t onoff);
extern void set_axp_pmu_poweroff(void);

extern void shutdown_pmu_no_power(void);
extern void shutdown_pmu_keep_power(void);
extern void set_axp_pmu_debug(BOOL flag);
#endif

#if (defined(_PMU_IP6303_))
/*PIP6303*/
extern void IP6303_init(void);
extern void IP6303_poweroff(void);
extern void shutdown_IP6303_no_power(void);
extern void shutdown_IP6303_keep_power(void);
extern void set_axp_IP6303_debug(BOOL flag);
extern void IP6303_test(void);
extern struct axp_charger IP6303_charging_monitor(void);
extern int IP6303_get_bat_valtage(void);
extern void IP6303_set_charge(BOOL nValue);
extern int IP6303_get_current_iusb(void);
extern BOOL IP6303_get_bat_status(void);
extern void IP6303_set_batext_check(BOOL nValue);
extern void IP6303_set_sensor_power18v(void);
extern void IP6303_set_sensor_power28v(void);
#endif

#endif

#endif
