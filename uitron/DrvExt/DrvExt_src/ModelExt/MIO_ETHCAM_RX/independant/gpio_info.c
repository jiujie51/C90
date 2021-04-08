#include "modelext_info.h"
#include "gpio_info.h"
#include "IOCfg.h"

GPIO_INIT_OBJ ind_gpio_info_data[] __attribute__((section("modelext_data.gpio_info"))) = {
#if 0
	{  GPIO_LENS_RESET,        GPIO_DIR_OUTPUT,    PAD_NONE,             PAD_PIN_NOT_EXIST   },
	{  GPIO_LENS_ZOOM_INTR,    GPIO_DIR_INPUT,     PAD_PULLDOWN,         PAD_LENS_ZOOM_INTR  },
	{  GPIO_LENS_FOCUS_INTR,   GPIO_DIR_INPUT,     PAD_PULLDOWN,         PAD_LENS_FOCUS_INTR },
	{  GPIO_LENS_IR_CTRL0,     GPIO_DIR_OUTPUT,    PAD_NONE,             PAD_PIN_NOT_EXIST   },
	{  GPIO_LENS_IR_CTRL1,     GPIO_DIR_OUTPUT,    PAD_NONE,             PAD_PIN_NOT_EXIST   },

	{  GPIO_KEY_LEFT,          GPIO_DIR_INPUT,     PAD_PULLUP,           PAD_KEY_LEFT        },
	{  GPIO_KEY_ENTER,         GPIO_DIR_INPUT,     PAD_PULLUP,           PAD_KEY_ENTER       },
	{  GPIO_KEY_UP,            GPIO_DIR_INPUT,     PAD_PULLUP,           PAD_KEY_UP          },
	{  GPIO_KEY_RIGHT,         GPIO_DIR_INPUT,     PAD_PULLUP,           PAD_KEY_RIGHT       },
	{  GPIO_KEY_PLAYBACK,      GPIO_DIR_INPUT,     PAD_PULLUP,           PAD_KEY_PLAYBACK    },
	{  GPIO_KEY_DOWN,          GPIO_DIR_INPUT,     PAD_PULLUP,           PAD_KEY_DOWN        },
	{  GPIO_KEY_MODE,          GPIO_DIR_INPUT,     PAD_PULLUP,           PAD_KEY_MODE        },
#endif
	{  GPIO_KEY_MENU,          GPIO_DIR_INPUT,     PAD_PULLUP,           PAD_KEY_MENU        },
#if defined(_MCU_ENABLE_)
	{  PIR_POWER_ON,           GPIO_DIR_OUTPUT,    PAD_NONE,             PAD_PIN_NOT_EXIST   },
	{  PIR_POWER_OFF,          GPIO_DIR_OUTPUT,    PAD_NONE,             PAD_PIN_NOT_EXIST   },
#endif

// SENSOR 1
    {  GPIO_SENSOR_SIE,		      GPIO_DIR_OUTPUT,    GPIO_SET_OUTPUT_HI,			 PAD_PIN_NOT_EXIST	 },
    {  GPIO_SENSOR_PWR1_8,		   GPIO_DIR_OUTPUT,    GPIO_SET_OUTPUT_LOW,			 PAD_PIN_NOT_EXIST	 },
    {  GPIO_SENSOR_PWR1_2,		   GPIO_DIR_OUTPUT,    GPIO_SET_OUTPUT_LOW,			 PAD_PIN_NOT_EXIST	 },
    {  GPIO_SENSOR_PWR3_3,		   GPIO_DIR_OUTPUT,    GPIO_SET_OUTPUT_LOW,			 PAD_PIN_NOT_EXIST	 },
    {  GPIO_SENSOR_RESET,          GPIO_DIR_OUTPUT,   GPIO_SET_OUTPUT_HI,           PAD_PIN_NOT_EXIST   },

// SENSOR 2
   // {  GPIO_SENSOR2_RESET, 		  GPIO_DIR_OUTPUT,	  GPIO_SET_OUTPUT_HI,			 PAD_PIN_NOT_EXIST	 },
   // {  GPIO_SENSOR2_AVT,		   GPIO_DIR_OUTPUT,    PAD_PULLDOWN, 		 PAD_PIN_NOT_EXIST	 },
   // {  GPIO_SENSOR2_PWR1_2,		   GPIO_DIR_OUTPUT,    GPIO_SET_OUTPUT_HI, 		 PAD_PIN_NOT_EXIST	 },
   // {  GPIO_SENSOR2_PWR3_3,		   GPIO_DIR_OUTPUT,    GPIO_SET_OUTPUT_HI, 		 PAD_PIN_NOT_EXIST	 },
// ETHERNET
  // {  GPIO_ETH_PWR,            GPIO_DIR_OUTPUT,   GPIO_SET_OUTPUT_LOW,           PAD_PIN_NOT_EXIST   },
      {  GPIO_ETH_PWR,            GPIO_DIR_OUTPUT,   GPIO_SET_OUTPUT_HI,           PAD_PIN_NOT_EXIST   },


// LCD
#if (LCD_BACKLIGHT_CTRL == LCD_BACKLIGHT_BY_GPIO)
    {  GPIO_LCD_BLG_PCTL,			GPIO_DIR_OUTPUT,   GPIO_SET_OUTPUT_HI,			 PAD_PIN_NOT_EXIST	 },
#endif
    {  GPIO_LCD_EN,			GPIO_DIR_OUTPUT,   GPIO_SET_OUTPUT_HI,			 PAD_PIN_NOT_EXIST	 },
    {  GPIO_BL_EN,			GPIO_DIR_OUTPUT,   GPIO_SET_OUTPUT_HI,			 PAD_PIN_NOT_EXIST	 },
    {  GPIO_LCD_RESET,      GPIO_DIR_OUTPUT,   GPIO_SET_OUTPUT_HI,			 PAD_PIN_NOT_EXIST},
    {  GPIO_CONVERT_IC_RESET,      GPIO_DIR_OUTPUT,   GPIO_SET_OUTPUT_HI,			 PAD_PIN_NOT_EXIST},
// GSENSOR
    {  GPIO_GSENSOR_RESET,  GPIO_DIR_OUTPUT,   GPIO_SET_OUTPUT_LOW,           PAD_PIN_NOT_EXIST},

// Speaker
    {  GPIO_SPK_PWR,  GPIO_DIR_OUTPUT,   GPIO_SET_OUTPUT_HI,           PAD_PIN_NOT_EXIST},

// Touch Panel
    {  TP_RESET_IO,  GPIO_DIR_OUTPUT,	 GPIO_SET_OUTPUT_LOW,		   PAD_PIN_NOT_EXIST},
    {  TP_PEN_IO,  GPIO_DIR_OUTPUT,   GPIO_SET_OUTPUT_LOW,           PAD_PIN_NOT_EXIST},
	{ TP_3P3V_EN, GPIO_DIR_OUTPUT,   GPIO_SET_OUTPUT_HI,           PAD_PIN_NOT_EXIST},

};

MODELEXT_HEADER ind_gpio_info_header __attribute__((section("modelext_header.gpio_info"))) = {
	.size = sizeof(ind_gpio_info_data) + sizeof(MODELEXT_HEADER),
	.type = MODELEXT_TYPE_GPIO_INFO,
	.number = sizeof(ind_gpio_info_data) / sizeof(GPIO_INIT_OBJ),
	.version = GPIO_INFO_VER,
};

void SetEthPowerOn(void);
void SetEthPowerOn(void)
{
	gpio_setDir(GPIO_ETH_PWR, GPIO_DIR_OUTPUT);
	gpio_setPin(GPIO_ETH_PWR);			
}
void SetEthPowerOff(void);
void SetEthPowerOff(void)
{
	gpio_setDir(GPIO_ETH_PWR, GPIO_DIR_OUTPUT);
	gpio_clearPin(GPIO_ETH_PWR);		
}


void SetBLStatus(BOOL status );

void SetBLStatus(BOOL status )
{
	if ( status )
	{
		gpio_setPin(GPIO_PWM_LCD_BLG_PCTL);	
		gpio_setPin(GPIO_LCD_EN);
		
	}else{
	
		gpio_clearPin(GPIO_PWM_LCD_BLG_PCTL);
		gpio_clearPin(GPIO_LCD_EN);
	}
	
}
