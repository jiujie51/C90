

#include "GSensor.h"
#include "GSensor_Combo.h"
#include "Gsensor_DMA380.h"
#include "SwTimer.h"
#include "Debug.h"
#include <stdlib.h>
#include "ErrorNo.h"
#include "rtc.h"
#include "Delay.h"
#include "stdio.h"

#define _GSENSOR_TEST_        DISABLE
#define __MODULE__          GSensor_DA380
//#define __DBGLVL__ 0        //OFF mode, show nothing
//#define __DBGLVL__ 1        //ERROR mode, show err, wrn only
#define __DBGLVL__ 1        //TRACE mode, show err, wrn, ind, msg and func and ind, msg and func can be filtering by __DBGFLT__ settings
#define __DBGFLT__ "*"      //*=All
#include "DebugModule.h"


static int   DA380_i2c_read_byte_data( unsigned char addr, unsigned char *data);
static int DA380_i2c_write_byte_data( unsigned char addr, unsigned char data);
static int DA380_register_read( unsigned char addr, unsigned char *data);
static int DA380_register_write( unsigned char addr, unsigned char data);
static UINT DA380_register_mask_write(unsigned char addr, unsigned char mask, unsigned char data);
static UINT DA380_read_latch(void);
static UINT DA380_open_interrupt(UINT num);
static UINT DA380_close_interrupt(UINT num);
static UINT  DA380_i2c_read_block_data( unsigned char base_addr, unsigned char count, unsigned char *data);
static UINT DA380_register_read_continuously( unsigned char addr, unsigned char count, unsigned char *data);
static void DA380_InitRdLatch(void);
static BOOL GSensor_DA380_Init(void);


static BOOL  g_bGsensorOpened = FALSE;

static GSENSOR_SENSITIVITY g_GSensorSensitivity = GSENSOR_SENSITIVITY_OFF;

static GSENSOR_OBJ g_GsensorDA380Obj = {
        GSensor_DA380_open, 
        GSensor_DA380_close, 
        GSensor_DA380_GetStatus, 
        GSensor_DA380_ParkingMode, 
        GSensor_DA380_CrashMode,
        GSensor_DA380_SetSensitivity, 
        GSensor_DA380_Get3AxisData,
        GSensor_DA380_ClearIntSts
        };
static BOOL bGsensorError = FALSE;

BOOL gbDA38TurnOnMachineFlag = FALSE;
static UINT32 g_GSensorMovieParkSensitivity;
BOOL	GetGensorInterruptData(UINT32 *uGsensorX, UINT32 *uGsensorY, UINT32 *uGsensorZ);
void GSensor_SetMovieParkSensitivity(UINT32 MovieParkSensitivity);


PGSENSOR_OBJ GSensor_DA380_getGSensorObj(void)
{
//CHKPNT;
    return &g_GsensorDA380Obj;
}

UINT32 uiThreshold;

static UINT32 GSensor_DA380_GetSensitivityLevel(void)
{
//debug_msg("--cj g_GSensorSensitivity:%d \r\n",g_GSensorSensitivity);
    switch(g_GSensorSensitivity)
    {
    case GSENSOR_SENSITIVITY_OFF:
      uiThreshold =5000;
        break;
    case GSENSOR_SENSITIVITY_LOW:
        uiThreshold =200;//45;//25;//600;
        break;
    case GSENSOR_SENSITIVITY_MED:
        uiThreshold = 105;//20;// 400;
        break;
    case GSENSOR_SENSITIVITY_HIGH:
        uiThreshold = 50;//16;//200;
        break;
    default:
        uiThreshold =5000;
        break;
    }
    return uiThreshold;

}
//----------------------------------------------------------------------------------------------
/*return value: 0: is ok    other: is failed*/
int     DA380_i2c_read_byte_data( unsigned char addr, unsigned char *data){
	*data =  (unsigned char)GSensor_I2C_ReadReg(addr);
	if(*data == 0xffff)
	{
	  bGsensorError = TRUE;
	  return -1;
	}
	return 0;
}

/*return value: 0: is ok    other: is failed*/
int DA380_i2c_write_byte_data( unsigned char addr, unsigned char data){
	GSensor_I2C_WriteReg(addr,data);
	return 0;
}

int DA380_register_read( unsigned char addr, unsigned char *data){
    int     res = 0;

    res = DA380_i2c_read_byte_data(addr, data);
    if(res != 0) {
          return res;
    }

    return res;
}

int DA380_register_write( unsigned char addr, unsigned char data){
    int     res = 0;
//debug_err(("mir3da_register_write  is addr==0x%x,data==0x%x\r\n",addr,data));
    res =DA380_i2c_write_byte_data(addr, data);
    if(res != 0) {
         return res;
    }
    return res;
}

UINT DA380_register_mask_write(unsigned char addr, unsigned char mask, unsigned char data){
    UINT     res = 0;
    unsigned char      tmp_data;

    res = DA380_register_read(addr, &tmp_data);
    if(res) {
        return res;
    }

    tmp_data &= ~mask; 
    tmp_data |= data & mask;
    res = DA380_register_write(addr, tmp_data);
    return res;
}

UINT DA380_read_latch(void)
{
    unsigned char data = 0;
    unsigned char data2 = 0;
    DA380_register_read( 0x09, &data);
    DA380_register_read( 0x0B, &data2);
    if(data&0x04)
        return 1;

    return 0;
}

void GSensor_SetMovieParkSensitivity(UINT32 MovieParkSensitivity)
{
    g_GSensorMovieParkSensitivity = MovieParkSensitivity;
}

UINT DA380_open_interrupt(UINT num){
    UINT   res = 0;

    res = DA380_register_write(NSA_REG_INTERRUPT_SETTINGS1,0x03);
    res = DA380_register_write(NSA_REG_ACTIVE_DURATION,0x02 );
    
    DBGD(g_GSensorMovieParkSensitivity);

    switch(g_GSensorMovieParkSensitivity)
    {
        case 1://Low
            debug_msg("mmmmm set park s1\r\n");
            res = DA380_register_write(NSA_REG_ACTIVE_THRESHOLD,0x16 );
            break;
        
        case 2://Med
            debug_msg("mmmmm set park s2\r\n");
            res = DA380_register_write(NSA_REG_ACTIVE_THRESHOLD,0x10 );
            break;
        
        case 3://High
        default:
            debug_msg("mmmmm set park s3\r\n");
            res = DA380_register_write(NSA_REG_ACTIVE_THRESHOLD,0x08 );
        break;
    }
    switch(num)
    {
        case 0:
            res = DA380_register_write(NSA_REG_INTERRUPT_MAPPING1,0x04 );
            break;
        case 1:
            res = DA380_register_write(NSA_REG_INTERRUPT_MAPPING3,0x04 );
            break;
    }

    return res;
}


UINT DA380_close_interrupt(UINT num){
	UINT   res = 0;
    TESTLOG("");
	res = DA380_register_write(NSA_REG_INTERRUPT_SETTINGS1,0x00 );
			
	switch(num){

		case 0:
			res = DA380_register_write(NSA_REG_INTERRUPT_MAPPING1,0x00 );
			break;

		case 1:
			res = DA380_register_write(NSA_REG_INTERRUPT_MAPPING3,0x00 );
			break;
	}

	return res;
}


/*return value: 0: is count    other: is failed*/
UINT     DA380_i2c_read_block_data( unsigned char base_addr, unsigned char count, unsigned char *data)
{
	UINT i = 0;		
	for(i = 0; i < count;i++)
	{
		 DA380_register_read(base_addr+i, data);
               data ++;
	}		
	return count;
}


UINT DA380_register_read_continuously( unsigned char addr, unsigned char count, unsigned char *data)
{
    UINT     res = 0;

    res = (count==DA380_i2c_read_block_data(addr, count, data)) ? 0 : 1;
    if(res != 0) {
         return res;
    }

    return res;
}

 UINT32 g_testGsensor[12];
UINT DA380_read_data(short *x, short *y, short *z)
{
    unsigned char    tmp_data[6] = {0};

    if (DA380_register_read_continuously(NSA_REG_ACC_X_LSB, 6, tmp_data) != 0) {
        return -1;
    }

	g_testGsensor[0] = (UINT32)tmp_data[1];
	g_testGsensor[1] = (UINT32)tmp_data[0];
	
	g_testGsensor[2] = (UINT32)tmp_data[3];
	g_testGsensor[3] = (UINT32)tmp_data[2];
	
	g_testGsensor[4] = (UINT32)tmp_data[5];
	g_testGsensor[5] = (UINT32)tmp_data[4];
	
	g_testGsensor[6] = 0;
	g_testGsensor[7] = 0;
	g_testGsensor[8] = 0;
	g_testGsensor[9] = 0;
	g_testGsensor[10] = 0;
	g_testGsensor[11] = 0;


    *x = ((short)(tmp_data[1] << 8 | tmp_data[0]))>> 4;
    *y = ((short)(tmp_data[3] << 8 | tmp_data[2]))>> 4;
    *z = ((short)(tmp_data[5] << 8 | tmp_data[4]))>> 4;

    return 0;
}




static void GSensor_DMA380_OpenInterface(void)
{
	
}

static void GSensor_DMA380_CloseInterface(void)
{

}

extern void System_SetGsensorPwrOn(BOOL bVaule);
void DA380_InitRdLatch(void)
{
    if( 1 == DA380_read_latch() || rtc_getPWROnSource() == RTC_PWRON_SRC_PWR_SW4)
    {
        debug_msg("Liwk ------------ DA380 Power On from Gsensor!!!\r\n");
        gbDA38TurnOnMachineFlag = TRUE;     //to stop record
        System_SetGsensorPwrOn(TRUE);
		//CHKPNT;
    }else if( 0 == DA380_read_latch() )
    {
        gbDA38TurnOnMachineFlag = FALSE;
        System_SetGsensorPwrOn(FALSE);
    }
}

//------------------------------------------------------------------------------------------------
BOOL GSensor_DA380_Init(void)
{
    GSENSOR_INFO GsensorInfo;

    GsensorInfo.I2C_RegBytes = GSENSOR_I2C_REGISTER_1BYTE;
    GsensorInfo.I2C_PinMux = I2C_PINMUX_1ST;
    GsensorInfo.I2C_BusClock = I2C_BUS_CLOCK_200KHZ;
    // DMARD07 GSensor I2C slave addresses
    GsensorInfo.I2C_Slave_WAddr = DA380_WRITE_ID;
    GsensorInfo.I2C_Slave_RAddr = DA380_READ_ID;
    
    //TESTLOG("");

    if (GSensor_I2C_Init(GsensorInfo) == TRUE)
    {
      UINT			 res = 0;
      unsigned char data=0;
      DA380_register_read(NSA_REG_WHO_AM_I,&data);     
      if(data != 0x13){
	  	debug_msg("mmmmm is not da380\r\n");
	  	   return FALSE;
      }
      DA380_InitRdLatch();
      Delay_DelayMs(5);
      
      res =  DA380_register_mask_write(NSA_REG_SPI_I2C, 0x24, 0x24);
      
      Delay_DelayMs(5);
      
      res |= DA380_register_mask_write(NSA_REG_G_RANGE, 0xff, 0x02);
      res |= DA380_register_mask_write(NSA_REG_POWERMODE_BW, 0xFF, 0x34);
      res |= DA380_register_mask_write(NSA_REG_ODR_AXIS_DISABLE, 0xFF, 0x07);
      
      res |= DA380_register_mask_write(NSA_REG_INT_PIN_CONFIG, 0xFF, 0x01);//set int_pin level
      res |= DA380_register_mask_write(NSA_REG_INT_LATCH, 0x8F, 0x85);//clear latch and set latch mode
      
    //  res |= DA380_register_mask_write(NSA_REG_INT_LATCH, 0x8F, 0x85);//clear latch and set latch mode
      //res |= DA380_register_mask_write(NSA_REG_INT_LATCH, 0x8F, 0x8d);//clear latch and set latch mode  (50ms)


      res |= DA380_register_mask_write(NSA_REG_ENGINEERING_MODE, 0xFF, 0x83);
      res |= DA380_register_mask_write(NSA_REG_ENGINEERING_MODE, 0xFF, 0x69);
      res |= DA380_register_mask_write(NSA_REG_ENGINEERING_MODE, 0xFF, 0xBD);
      
        debug_msg("mmmmm  da380 open!\r\n");

        return TRUE;
    } else {
        debug_msg("G Sensor DA380 Init failed !!\r\n");
        return FALSE;
    }
}

BOOL GSensor_DA380_open(void)
{
   BOOL bResult;
   if (g_bGsensorOpened == TRUE)
   {
       debug_msg("Gsensor open already\r\n");
       return TRUE;
   }

   g_bGsensorOpened = TRUE;

   // open Gsensor interface
   GSensor_DMA380_OpenInterface();

   // Gsensor init
   bResult = GSensor_DA380_Init();

   return bResult;
}

BOOL GSensor_DA380_close(void)
{

    if (g_bGsensorOpened==FALSE)
    {
        debug_msg("I2C close already\r\n");
        return TRUE;
    }

    // close Gsensor interface
    GSensor_DMA380_CloseInterface();

    g_bGsensorOpened = FALSE;

    return TRUE;
}

BOOL	GetGensorInterruptData(UINT32 *uGsensorX, UINT32 *uGsensorY, UINT32 *uGsensorZ);



BOOL GSensor_DA380_GetStatus(Gsensor_Data *GS_Data)
{
   // debug_msg("DA380 GSensor_GetStatus..........\n\r");
    return GetGensorInterruptData(&(GS_Data->Axis.Xacc), &(GS_Data->Axis.Yacc), &(GS_Data->Axis.Zacc));
}

BOOL GSensor_DA380_ParkingMode(void)
{
    if(g_GSensorMovieParkSensitivity != 0)
        DA380_open_interrupt(0);
    else
        DA380_close_interrupt(0);
    return TRUE;
}


BOOL GSensor_DA380_CrashMode(void)
{
    return FALSE;
}

void GSensor_DA380_SetSensitivity(GSENSOR_SENSITIVITY GSensorSensitivity)
{
    g_GSensorSensitivity = GSensorSensitivity;
}



void  GSensor_DA380_Get3AxisData(INT32 *pX, INT32 *pY, INT32 *pZ)
{
   DA380_read_data((short *)pX,(short *)pY,(short *)pZ);
}

void  GSensor_DA380_ClearIntSts(void)
{
   DA380_read_latch();
}


//*****************************************************************************//
BOOL  GetGensorInterruptData(UINT32 *uGsensorX, UINT32 *uGsensorY, UINT32 *uGsensorZ)
{
    short ucGx = 0;short ucGY = 0;short ucGZ = 0;
    static short prev_x = 0, prev_y = 0, prev_z = 0;
    static char	b_is_first = 0;
    short  thre = 0; 
  
    UINT res;
    UINT32  GSState = 0;

   res = DA380_read_data(&ucGx,&ucGY,&ucGZ);

    if(res == 0 && !bGsensorError)
    {
        static UINT32 ShowCnt;
        if ( (ShowCnt%10) == 0 && ShowCnt >= 5)
        {
            char pString[512];
            //SenFP_ClearOSD();
            if(abs(ucGx) < 1000 && abs(ucGY) < 1000 && abs(ucGZ) < 1000)
            {
                snprintf(pString, 512,"Gsensor:OK!!");
            }else{
                snprintf(pString, 512,"Gsensor:Data ERROR!!");
            }
        }
        ShowCnt++;
    }else{
        static UINT32 ShowCnt;
        if ( (ShowCnt%10) == 0 && ShowCnt >= 5)
        {
            char pString[512];			

            //SenFP_ClearOSD();
            snprintf(pString, 512, "Gsensor: HW ERROR!!");
        }
        ShowCnt++; 
    }
   //debug_msg("mmmm GS DATA(%d, %d, %d)\r\n", ucGx, ucGY, ucGZ);   
    *uGsensorX = ucGx;
    *uGsensorY = ucGY;
    *uGsensorZ = ucGZ;
   
    if(!b_is_first)
    {
        prev_x = ucGx;
        prev_y = ucGY;
        prev_z = ucGZ;

        b_is_first = 1;
    }

    thre = GSensor_DA380_GetSensitivityLevel();

    if( abs(ucGx - prev_x) > thre
        ||abs(ucGY- prev_y) > thre
        ||abs(ucGZ- prev_z) > thre)
    	{
        	GSState = 1;
    		prev_x = ucGx;
    		prev_y = ucGY;
    		prev_z = ucGZ;
    	}
	//   debug_msg("mmmm GS DATA(%d, %d, %d)\r\n", abs(ucGx - prev_x), abs(ucGY- prev_y), abs(ucGZ- prev_z));   

    if(GSState == 1)
        debug_msg(" Status: %d, THRE %d\r\n",GSState,thre);
    return(GSState);
}



