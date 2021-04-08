/**
    @file       GC2053P.c

    @brief      GC2053P

    @note       Nothing (or anything need to be mentioned).

    Copyright   Novatek Microelectronics Corp. 2010.  All rights reserved.
*/
//----------------------------------------------------------------------------------------------------
#include "Utility.h"
#include "gc2053p_param.h"
#include "sensor_cmd_if.h"
#include "NvtVerInfo.h"

NVTVER_VERSION_ENTRY(CMOS_GC2053P, 1, 00, 000, 00)

#define __MODULE__ GC2053P
#define __DBGLVL__          2 // 0=FATAL, 1=ERR, 2=WRN, 3=UNIT, 4=FUNC, 5=IND, 6=MSG, 7=VALUE, 8=USER
#define __DBGFLT__          "*" // *=All, [mark]=CustomClass
#define __DBGFLT__ "*"      // *=All
#include "DebugModule.h"

#define MIN_EXPOSURE_LINE    4

//Function prototype
#if 0
#endif

static UINT32 sensor_option_init[SENSOR_ID_MAX_NUM] = {0};
static UINT32 sensor_option[SENSOR_ID_MAX_NUM] = {0};
static UINT32 vd_period[SENSOR_ID_MAX_NUM] = {0};
static UINT32 fps_mode = FPS_DEPEND_ON_EXPT;

static ER init_GC2053P(SENSOR_ID id, SEN_DRV_INIT_OBJ *drv_init_obj);
static ER uninit_GC2053P(SENSOR_ID id);
static ER sleep_GC2053P(SENSOR_ID id);
static ER wakeup_GC2053P(SENSOR_ID id);
static ER write_reg_GC2053P(SENSOR_ID id, SENSOR_CMD *cmd);
static ER read_reg_GC2053P(SENSOR_ID id, SENSOR_CMD *cmd);
static ER chg_mode_GC2053P(SENSOR_ID id, SEN_CHGMODE_DATA chg_mode_data, GAIN_SETTING *preset_gain, EXPOSURE_SETTING *preset_expt);
static ER chg_fps_GC2053P(SENSOR_ID id, SEN_CHGFPS_DATA chg_fps_data, GAIN_SETTING *preset_gain, EXPOSURE_SETTING *preset_expt);
static ER set_info_GC2053P(SENSOR_ID id, SEN_SET_DRV_ITEM set_drv_item, void *data, SEN_COM_INFO com_info);
static ER get_info_GC2053P(SENSOR_ID id, SEN_GET_DRV_ITEM get_drv_item, void *data);

//Internal API
static ER set_exptime_GC2053P(SENSOR_ID id, EXPOSURE_SETTING *expt_setting, SENSOR_MODE mode);
static ER set_gain_GC2053P(SENSOR_ID id, GAIN_SETTING *gain, SENSOR_MODE mode);
static ER set_vd_period_register_GC2053P(SENSOR_ID id, SENSOR_MODE mode, UINT32 frame_rate);
static ER set_grs_mode_GC2053P(SENSOR_ID id, void *param);
static ER set_hdr_info_GC2053P(SENSOR_ID id, HDR_SET_INFO *hdr_info);
static ER set_user_option_GC2053P(SENSOR_ID id, UINT32 option);
static ER get_expo_setting_GC2053P(SENSOR_ID id, SENSOR_MODE mode, UINT32 *exp_time, EXPOSURE_SETTING *expt_setting, UINT32 *compensate_ratio);
static ER get_gain_setting_GC2053P(SENSOR_ID id, SENSOR_MODE mode, UINT32 *gain_ratio, GAIN_SETTING *gain);

static SENSOR_DRV_TAB m_p_GC2053P_drv = {
	init_GC2053P,
	uninit_GC2053P,
	sleep_GC2053P,
	wakeup_GC2053P,
	write_reg_GC2053P,
	read_reg_GC2053P,
	chg_mode_GC2053P,
	chg_fps_GC2053P,
	set_info_GC2053P,
	get_info_GC2053P,
	get_sensor_drv_info_GC2053P,
	get_mode_info_GC2053P,
	get_sensor_cmd_info_GC2053P,
	get_sensor_if_info_GC2053P,
};

#if 0
-Intern api
#endif

static SENSOR_CMD set_cmd_info_GC2053P(UINT32 addr, UINT32 data_length, UINT32 data0, UINT32 data1)
{
	SENSOR_CMD cmd;
	cmd.ui_addr = addr;
	cmd.ui_data_len = data_length;
	cmd.ui_data[0] = data0;
	cmd.ui_data[1] = data1;
	return cmd;
}

static UINT32 chkdata1 = 0xFFFF;
static UINT32 chkdata2 = 0xFFFF;
static ER init_GC2053P(SENSOR_ID id, SEN_DRV_INIT_OBJ *drv_init_obj)
{
	sensor_option_init[id] = drv_init_obj->option;
	sensor_option[id] = 0;

	chkdata1 = 0xFFFF;
	chkdata2 = 0xFFFF;

	return E_OK;
}

static ER uninit_GC2053P(SENSOR_ID id)
{
	UINT32 cnt;
	for (cnt = 0; cnt < MODE_MAX; cnt++) {
		set_sensor_chgmode_frame_rate_GC2053P(id, cnt, 0);
	}
	return E_OK;
}

static ER sleep_GC2053P(SENSOR_ID id)
{
	return E_OK;
}

static ER wakeup_GC2053P(SENSOR_ID id)
{
	return E_OK;
}

static ER write_reg_GC2053P(SENSOR_ID id, SENSOR_CMD *cmd)
{
	//Use command interface (sif/i2c/io/...) write register to sensor
	ER rt = E_OK;
	rt = sensor_cmd_get_i2c_ctrl_obj()->write(id, cmd->ui_addr, cmd->ui_data[0], I2CFMT_1B1B);
    //re try
    if (rt != E_OK) {
        rt = sensor_cmd_get_i2c_ctrl_obj()->write(id, cmd->ui_addr, cmd->ui_data[0], I2CFMT_1B1B);
    }

    return rt;
}

static ER read_reg_GC2053P(SENSOR_ID id, SENSOR_CMD *cmd)
{
	//Use command interface (i2c/io/...) read register from sensor
	ER rt = E_OK;
	rt = sensor_cmd_get_i2c_ctrl_obj()->read(id, cmd->ui_addr, &(cmd->ui_data[0]), I2CFMT_1B1B);
	return rt;
}

static ER chg_mode_GC2053P(SENSOR_ID id, SEN_CHGMODE_DATA chg_mode_data, GAIN_SETTING *preset_gain, EXPOSURE_SETTING *preset_expt)
{
	SENSOR_CMD *p_cmd_list = NULL, cmd;
	UINT32 idx, cmd_num = 0;
	ER rt = E_OK;

    debug_msg("chg_mode_GC2053P change to mode: %d\r\n",chg_mode_data.mode);
	//Set sensor frame rate
	set_sensor_chgmode_frame_rate_GC2053P(id, chg_mode_data.mode, chg_mode_data.frame_rate);
	rt |= set_vd_period_register_GC2053P(id, chg_mode_data.mode, chg_mode_data.frame_rate); // set register to change VD period
	if (rt != E_OK) {
		DBG_ERR("set_vd_period_register_GC2053P error %d\r\n", rt);
	}

    CHKPNT;
	//Get & set sensor cmd table
	cmd_num = get_cmd_tab_GC2053P(chg_mode_data.mode, &p_cmd_list);
	
	CHKPNT;
	if (p_cmd_list == NULL) {
		
		CHKPNT;
		DBG_ERR("%s: SenMode(%d) out of range!!!\r\n", __func__, chg_mode_data.mode);
		return E_SYS;
	}
    rt = E_OK;
	for (idx = 0; idx < cmd_num; idx ++) {
		if (p_cmd_list[idx].ui_addr == CMD_DELAY) {
			Delay_DelayMs((p_cmd_list[idx].ui_data[0] & 0xff) | ((p_cmd_list[idx].ui_data[1] << 8) & 0xff00));
		} 
		else 
		{
		    ER t_ret;
			cmd = set_cmd_info_GC2053P(p_cmd_list[idx].ui_addr, p_cmd_list[idx].ui_data_len, p_cmd_list[idx].ui_data[0], p_cmd_list[idx].ui_data[1]);
			t_ret = write_reg_GC2053P(id, &cmd);
			rt |= t_ret;
			//debug_msg("GC2053 Write Reg:(0x%02x, 0x%02x), %d\r\n",p_cmd_list[idx].ui_addr,p_cmd_list[idx].ui_data[0],t_ret);
		}
    }
	
    /*for (idx = 0; idx < cmd_num; idx ++) {
        ER t_ret;
		cmd.ui_addr = p_cmd_list[idx].ui_addr;
        t_ret = read_reg_GC2053P(id, &cmd);
		debug_msg("GC2023 Read Reg:(0x%02x, 0x%02x),%d\r\n",cmd.ui_addr, cmd.ui_data[0],t_ret);
    }*/
    
    if (rt != E_OK) {
		CHKPNT;
        DBG_ERR("write register error %d\r\n", rt);
        return rt;
    }

	//Set user option
	/*rt = set_user_option_GC2053P(id, sensor_option[id]);
	if (rt != E_OK) {
		DBG_ERR("set user option error %d\r\n", rt);
		return rt;
	}*/

	chkdata1 = 0xFFFF;
	chkdata2 = 0xFFFF;

	return E_OK;
}

static ER chg_fps_GC2053P(SENSOR_ID id, SEN_CHGFPS_DATA chg_fps_data, GAIN_SETTING *preset_gain, EXPOSURE_SETTING *preset_expt)
{
	ER rt = E_OK;

	return rt;
}

static ER set_info_GC2053P(SENSOR_ID id, SEN_SET_DRV_ITEM set_drv_item, void *data, SEN_COM_INFO com_info)
{
	ER rt = E_OK;
	SENSOR_MODE_INFO *mode_info;
	GAIN_SETTING *gain_setting;

	if (set_drv_item >= SEN_SET_DRV_MAX_NUM) {
		DBG_ERR("%s: sensor set item(%d) out of range!!!\r\n", __func__, set_drv_item);
		return E_SYS;
	}

	switch (set_drv_item) {
	case SEN_SET_DRV_EXPT:
		rt |= set_exptime_GC2053P(id, (EXPOSURE_SETTING *)(data), com_info.cur_sensor_mode);
		break;
	case SEN_SET_DRV_GAIN:
		gain_setting = (GAIN_SETTING *)data;
		rt |= set_gain_GC2053P(id, gain_setting, com_info.cur_sensor_mode);
		mode_info = get_mode_info_GC2053P(com_info.cur_sensor_mode);
		break;
	case SEN_SET_DRV_USER_OPTION:
		rt |= set_user_option_GC2053P(id, *((UINT32 *)data));
		break;
	case SEN_SET_DRV_GRS:
		rt |= set_grs_mode_GC2053P(id, (void *)(data));
		break;
	case SEN_SET_DRV_HDR_INFO:
		rt |= set_hdr_info_GC2053P(id, (HDR_SET_INFO *)data);
		break;
	case SEN_SET_DRV_TRIGGER_EVENT:
		break;
	default:
		DBG_ERR("sensor set drv item error %d\r\n", set_drv_item);
		rt = E_PAR;
		break;
	}

	if (rt != E_OK) {
		DBG_ERR("sensor set drv item %d information error %d\r\n", set_drv_item, rt);
	}

	return rt;
}

static ER get_info_GC2053P(SENSOR_ID id, SEN_GET_DRV_ITEM get_drv_item, void *data)
{
	ER rt = E_OK;
	EXPOSURE_PARAM *exp_param;
	GAIN_PARAM *gain_param;
	GAIN_INFO *gain_info;
	UINT32 *value;
	CHAR *name;
	FPS_PARAM *fps_param;
	UINT32 buf_cnt = 0;
	SENSOR_MODE_INFO *mode_info;

	switch (get_drv_item) {
	case SEN_GET_DRV_EXPO_SETTING:
		exp_param = (EXPOSURE_PARAM *)data;
		rt |= get_expo_setting_GC2053P(id, exp_param->sensor_mode, &exp_param->exp_time[0], &exp_param->exp_setting, &exp_param->compensate_ratio[0]);
		break;
	case SEN_GET_DRV_GAIN_SETTING:
		gain_param = (GAIN_PARAM *)data;
		gain_param->gain_setting.rev[0] = gain_param->gain_ratio[0];
		gain_param->gain_setting.rev[1] = gain_param->gain_ratio[1];
		rt |= get_gain_setting_GC2053P(id, gain_param->sensor_mode, &gain_param->gain_ratio[0], &gain_param->gain_setting);
		break;
	case SEN_GET_DRV_GAIN_INFO:
		gain_info = (GAIN_INFO *)data;
		gain_info->gain = get_mode_info_GC2053P(gain_info->sensor_mode)->gain;
		break;
	case SEN_GET_DRV_NAME:
		name = (CHAR *)data;
		buf_cnt = snprintf(NULL, 0, "%s", __xstring(__section_name__));
		snprintf((char *)name, buf_cnt + 1, "%s", __xstring(__section_name__));
		break;
	case SEN_GET_DRV_FPS:
		fps_param = (FPS_PARAM *)data;
		mode_info = get_mode_info_GC2053P(fps_param->sensor_mode);
		fps_param->cur_fps = get_sensor_cur_frame_rate_GC2053P(id, fps_param->sensor_mode);
		fps_param->chg_fps = get_sensor_chgmode_frame_rate_GC2053P(id, fps_param->sensor_mode);
		fps_param->dft_fps = mode_info->frame_rate;
		break;
	case SEN_GET_DRV_MAX_MODE:
		value = (UINT32 *)data;
		*value = MODE_MAX;
		break;
	case SEN_GET_DRV_USER_OPTION:
		value = (UINT32 *)data;
		*value = (UINT32)(sensor_option[id]);
		break;
	case SEN_GET_DRV_TEMP:
		DBG_WRN("not suppor get sensor temperature\r\n");
		value = (UINT32 *)data;
		*value = (UINT32)(0);
		break;
	case SEN_GET_DRV_HDR_STATUS:
		DBG_WRN("not suppor get sensor hdr status\r\n");
		value = (UINT32 *)data;
		*value = (UINT32)(0);
		break;
	default:
		DBG_ERR("sensor get drv item error %d\r\n", get_drv_item);
		rt = E_PAR;
		break;
	}

	if (rt != E_OK) {
		DBG_ERR("sensor get drv item %d information error %d\r\n", get_drv_item, rt);
	}

	return rt;
}

/**
 * Sensor internal AE driver API
*/
static UINT32 calc_expo_setting_GC2053P(SENSOR_ID id, UINT32 exp_time, EXPOSURE_SETTING *expt_setting, SENSOR_MODE mode, UINT32 multi_frame_num)
{
	SENSOR_MODE_INFO *mode_info;
	UINT32 exp_line, line_time, compensate_ratio;

	mode_info = get_mode_info_GC2053P(mode);

	//Calculates the exposure setting
	//30000 * 74250000 /100000 / 10/
	exp_line = (exp_time*(mode_info->pclk/100000))/(mode_info->sen_hd[multi_frame_num]->period*10);
   // debug_msg("calc_expo_setting_GC2053P  exp_time: %d,   expline:%d \r\n", exp_time, exp_line);
    //Limit minimun exposure line
    if (exp_line < 2) {
        exp_line = 2;
    }

	//Collect the exposure setting
	expt_setting->ui_frame[0] = 0;
	expt_setting->ui_line[0] = exp_line;
	expt_setting->ui_pixel[0] = 0;

	//Calculates compensation ratio
	exp_line = (expt_setting->ui_frame[0]*mode_info->sen_vd[multi_frame_num]->period)+expt_setting->ui_line[0];
	line_time = exp_line*(mode_info->sen_hd[multi_frame_num]->period*10)/(mode_info->pclk/100000);

	if (line_time < exp_time) {
		compensate_ratio = (exp_time*100)/line_time;
	} else {
		compensate_ratio = 100;
	}
	return compensate_ratio;
}

static ER get_expo_setting_GC2053P(SENSOR_ID id, SENSOR_MODE mode, UINT32 *exp_time, EXPOSURE_SETTING *expt_setting, UINT32 *compensate_ratio)
{
	SENSOR_MODE_INFO *mode_info;
	UINT32 frame_cnt, total_frame;

	mode_info = get_mode_info_GC2053P(mode);

	//Get total frame number
	if (mode_info->multi_frame_info != NULL) {
		total_frame = mode_info->multi_frame_info->total_frm;
        if (total_frame == 0) {
            DBG_WRN("total_frame = 0, force to 1 \r\n");
            total_frame = 1;
        }
	} else {
		total_frame = 1;
	}

	//Calculates and set the exposure setting
	for (frame_cnt = 0; frame_cnt < total_frame; frame_cnt++) {
		EXPOSURE_SETTING exp_time_set = {0};
		compensate_ratio[frame_cnt] = calc_expo_setting_GC2053P(id, exp_time[frame_cnt], &(exp_time_set), mode, frame_cnt);
		expt_setting->ui_frame[frame_cnt] = exp_time_set.ui_frame[0];
		expt_setting->ui_line[frame_cnt] = exp_time_set.ui_line[0];
		expt_setting->ui_pixel[frame_cnt] = exp_time_set.ui_pixel[0];
		DBG_IND("Time = %d %d %d %d\r\n", exp_time[frame_cnt], expt_setting->ui_frame[frame_cnt], expt_setting->ui_line[frame_cnt], expt_setting->ui_pixel[frame_cnt]);
	}
	return E_OK;
}


UINT8 regValTable[25][4] = {	 
							  {0x00, 0x00,0x01,0x00},
							  {0x00, 0x10,0x01,0x0c},
							  {0x00, 0x20,0x01,0x1b},
							  {0x00, 0x30,0x01,0x2c},
							  {0x00, 0x40,0x01,0x3f},
							  {0x00, 0x50,0x02,0x16},
							  {0x00, 0x60,0x02,0x35},
							  {0x00, 0x70,0x03,0x16},
							  {0x00, 0x80,0x04,0x02},
							  {0x00, 0x90,0x04,0x31},
							  {0x00, 0xa0,0x05,0x32},
							  {0x00, 0xb0,0x06,0x35},
							  {0x00, 0xc0,0x08,0x04},
							  {0x00, 0x5a,0x09,0x19},
							  {0x00, 0x83,0x0b,0x0f},
							  {0x00, 0x93,0x0d,0x12},
							  {0x00, 0x84,0x10,0x00},
							  {0x00, 0x94,0x12,0x3a},
							  {0x01, 0x2c,0x1a,0x02},
							  {0x01, 0x3c,0x1b,0x20},
							  {0x00, 0x8c,0x20,0x0f},
							  {0x00, 0x9c,0x26,0x07},
							  {0x02, 0x64,0x36,0x21},
							  {0x02, 0x74,0x37,0x3a},
							  {0x00, 0xc6,0x3d,0x02},
					  };
UINT32 Analog_Multiple[25]={
				  64,
				  74,
				  89,
				  102,
				  127,
				  147,
				  177,
				  203,
				  260,
				  300,
				  361,
				  415,
				  504,
				  581,
				  722,
				  832,
				  1027,
				  1182,
				  1408,
				  1621,
				  1990,
				  2291,
				  2850,
				  3282,
				  4048,
}; 
 
UINT32 Gain_factor;
UINT32 Analog_Index;

static ER calc_gain_setting_GC2053P(SENSOR_ID id, UINT32 gain_ratio, GAIN_SETTING *gain)
{
/*	UINT32 Digital_Gain;
	UINT32 Decimal;
	

	Analog_Index=0;
	
	Gain_factor= gain_ratio;
	    //upper and lower bound
    if(Gain_factor<1000)
    {
        Gain_factor=1000;
    }
	
	while(Analog_Index<33)
	{
	  if(Gain_factor<Analog_Multiple[Analog_Index]) 
	  {
	    break;
	  }
	  else
	  {
	    Analog_Index++; 
	  }
	}

	Digital_Gain = Gain_factor*1000/Analog_Multiple[Analog_Index-1];
	Decimal=(Digital_Gain*64)/1000;

    gain->data1[0] =Decimal>>6;
    gain->data2[0] =(Decimal<<2)&0xfc;
	Analog_Index =Analog_Index-1;

    //DBG_ERR("Gain->data1 = %x,  Gain->data2 = %x,  Gain->data3= %x..\r\n", Gain->data1,Gain->data2, Gain->data3);
*/


	UINT32 Digital_Gain;
	UINT32 Decimal;
	UINT32 Gain_factor;
    UINT32 Analog_Index;

	Analog_Index=0;
	
	Gain_factor= 64*gain_ratio/1000; //96
	
	    //upper and lower bound
    if(Gain_factor<64)
    {
        Gain_factor=64;
    }
	
	while(Analog_Index<25)
	{
	  if(Gain_factor<Analog_Multiple[Analog_Index]) 
	  {
	    break;
	  }
	  else
	  {
	    Analog_Index++; //  2
	  }
	}

	Digital_Gain = Gain_factor*1000/Analog_Multiple[Analog_Index-1];
	Decimal=(Digital_Gain*64)/1000;

    gain->data1[0] =Decimal>>6;
    gain->data2[0] =(Decimal<<2)&0xfc;
	gain->data3[0] =Analog_Index-1;
    return E_OK;	

}


static ER get_gain_setting_GC2053P(SENSOR_ID id, SENSOR_MODE mode, UINT32 *gain_ratio, GAIN_SETTING *gain)
{
	SENSOR_MODE_INFO *mode_info;
	UINT32 frame_cnt, total_frame;

	mode_info = get_mode_info_GC2053P(mode);

	if (mode_info->multi_frame_info != NULL) {
		total_frame = mode_info->multi_frame_info->total_frm;
	} else {
		total_frame = 1;
	}

	for (frame_cnt = 0; frame_cnt < total_frame; frame_cnt++) {
		GAIN_SETTING analog_gain = {0};
		calc_gain_setting_GC2053P(id, gain_ratio[frame_cnt], &(analog_gain));
		gain->data1[frame_cnt] = analog_gain.data1[0];
		gain->data2[frame_cnt] = analog_gain.data2[0];		
		gain->data3[frame_cnt] = analog_gain.data3[0];
		DBG_IND("gain_ratio = %d 0x%.8x 0x%.8x 0x%.8x\r\n", gain_ratio[frame_cnt], gain->data1[frame_cnt], gain->data2[frame_cnt], gain->data3[frame_cnt]);
	}
	return E_OK;
}

static ER set_exptime_GC2053P(SENSOR_ID id, EXPOSURE_SETTING *expt_setting, SENSOR_MODE mode)
{
	SENSOR_CMD cmd;
	SENSOR_CMD Cmd1, Cmd2;
	SENSOR_DRV_INFO *sensor_drv_info;
	SENSOR_MODE_INFO *mode_info;
	UINT32 exp_time = 0, sen_vd_period = 0, chgmode_fps_src = 30, chgmode_fps_div = 0, cur_fps = 0, new_fps = 0;
	ER rt = E_OK;
	//Get information
	mode_info = get_mode_info_GC2053P(mode);
	if (mode_info == NULL) {
		DBG_ERR("Sensor mode(%d) overflow\r\n", mode);
		return E_SYS;
	}
	sensor_drv_info = get_sensor_drv_info_GC2053P();
	if (sensor_drv_info == NULL) {
		DBG_ERR("get sensor info error\r\n");
		return E_SYS;
	}

	//Set frame rate
	if (get_sensor_chgmode_frame_rate_GC2053P(id, mode) != 0) {
		SEN_FPS_CONV_PARAM(mode_info->sen_vd[0]->period, &sen_vd_period
						   , mode_info->frame_rate, get_sensor_chgmode_frame_rate_GC2053P(id, mode));
	} else {
		sen_vd_period = mode_info->sen_vd[0]->period;
	}
    
	exp_time = (mode_info->sen_hd[0]->period) * (expt_setting->ui_line[0]);
	exp_time = (exp_time * 10) / (mode_info->pclk / 100000);
	if (fps_mode == FPS_MUL_FPS) {
		SEN_FPS_CONV2_DIV(get_sensor_chgmode_frame_rate_GC2053P(id, mode), &chgmode_fps_src, &chgmode_fps_div);
		cur_fps = (get_sensor_chgmode_frame_rate_GC2053P(id, mode) & 0xffff0000) | ((chgmode_fps_div * ((expt_setting->ui_line[0] / sen_vd_period) + 1)) & 0xffff);
	} else if (fps_mode == FPS_DEPEND_ON_EXPT) {
		SEN_FPS_EXPT_CONV2_FMT(exp_time, &new_fps);
		cur_fps = (SEN_FPS_FMT_CONV2_REAL(new_fps) < (SEN_FPS_FMT_CONV2_REAL(get_sensor_chgmode_frame_rate_GC2053P(id, mode)))) ? new_fps : get_sensor_chgmode_frame_rate_GC2053P(id, mode);
	} else {
		DBG_ERR("fps mode %d error\r\n", fps_mode);
		SEN_FPS_CONV2_DIV(get_sensor_chgmode_frame_rate_GC2053P(id, mode), &chgmode_fps_src, &chgmode_fps_div);
		cur_fps = (get_sensor_chgmode_frame_rate_GC2053P(id, mode) & 0xffff0000) | ((chgmode_fps_div * ((expt_setting->ui_line[0] / sen_vd_period) + 1)) & 0xffff);
	}
	set_sensor_cur_frame_rate_GC2053P(id, mode, cur_fps);

    if((expt_setting->ui_line[0]<1)&&(expt_setting->ui_line[0]>=0xFFFF))
        return E_PAR;
	
    Cmd1.ui_data[0] = ((expt_setting->ui_line[0] & 0x0f00)>>8);
    Cmd2.ui_data[0] = ((expt_setting->ui_line[0] & 0xff));
	
	cmd = set_cmd_info_GC2053P(0x03, 1, Cmd1.ui_data[0], 0x0);
	write_reg_GC2053P(id, &cmd);
	cmd = set_cmd_info_GC2053P(0x04, 1, Cmd2.ui_data[0], 0x0);
	write_reg_GC2053P(id, &cmd);
	if (rt != E_OK) {
		DBG_ERR("write register error %d\r\n", rt);
	}
	return rt;
}

static ER set_gain_GC2053P(SENSOR_ID id, GAIN_SETTING *gain, SENSOR_MODE mode)
{
	/*SENSOR_CMD cmd;
	ER rt = E_OK;

	cmd = set_cmd_info_GC2053P(0xb4, 1, (regValTable[Analog_Index][0]& 0xff), 0x0);
	write_reg_GC2053P(id, &cmd);
	
	cmd = set_cmd_info_GC2053P(0xb3, 1, (regValTable[Analog_Index][1]& 0xff), 0x0);
	write_reg_GC2053P(id, &cmd);
	
	cmd = set_cmd_info_GC2053P(0xb1, 1, (gain->data1[0] & 0xff), 0x0);
	write_reg_GC2053P(id, &cmd);
	
	cmd = set_cmd_info_GC2053P(0xb2, 1, (gain->data2[0] & 0xff), 0x0);
	write_reg_GC2053P(id, &cmd);

*/
	SENSOR_CMD Cmd;
	ER rt = E_OK;


	UINT8 Temp_Gain_data1,Temp_Gain_data2,Temp_Gain_data3,Temp_Gain_data4,Temp_Gain_data5,Temp_Gain_data6;
 
	
	Temp_Gain_data1 = regValTable[gain->data3[0]][0];
	Temp_Gain_data2 = regValTable[gain->data3[0]][1];
	Temp_Gain_data3 = regValTable[gain->data3[0]][2];
	Temp_Gain_data4 = regValTable[gain->data3[0]][3];
	Temp_Gain_data5 = (gain->data1[0] & 0xff);
	Temp_Gain_data6 = (gain->data2[0] & 0xff);

	Cmd = set_cmd_info_GC2053P(0xb4, 1, Temp_Gain_data1, 0);
	write_reg_GC2053P(id, &Cmd);
	
	Cmd = set_cmd_info_GC2053P(0xb3, 1, Temp_Gain_data2, 0);
	write_reg_GC2053P(id, &Cmd);

	Cmd = set_cmd_info_GC2053P(0xb8, 1, Temp_Gain_data3, 0);
	write_reg_GC2053P(id, &Cmd);

	Cmd = set_cmd_info_GC2053P(0xb9, 1, Temp_Gain_data4, 0);
	write_reg_GC2053P(id, &Cmd);

	Cmd = set_cmd_info_GC2053P(0xb1, 1, Temp_Gain_data5, 0);
	write_reg_GC2053P(id, &Cmd);

	Cmd = set_cmd_info_GC2053P(0xb2, 1, Temp_Gain_data6, 0);
	write_reg_GC2053P(id, &Cmd);


	return rt;
}

/**
 * sensor internal API
*/
static ER set_vd_period_register_GC2053P(SENSOR_ID id, SENSOR_MODE mode, UINT32 frame_rate)
{
	ER rt = E_OK;
	SENSOR_MODE_INFO *mode_info;
	SENSOR_DRV_INFO *sensor_drv_info;
	UINT32 new_vd_period, update_input_frame_rate, target_fps_src = 30, target_fps_div = 1, ori_fps_src = 30, ori_fps_div = 1;

	sensor_drv_info = get_sensor_drv_info_GC2053P();
	if (!sensor_drv_info->fps_info.support_chg) {
		DBG_ERR("sensor driver not support frame rate adjust\r\n");
		return E_SYS;
	}

	//Get previous information
	mode_info = get_mode_info_GC2053P(mode);

	//Convert frame rate format
	SEN_FPS_CONV2_DIV(frame_rate, &target_fps_src, &target_fps_div);
	SEN_FPS_CONV2_DIV(mode_info->frame_rate, &ori_fps_src, &ori_fps_div);
	SEN_FPS_CONV_PARAM(mode_info->sen_vd[0]->period, &new_vd_period, mode_info->frame_rate, frame_rate);

	if (new_vd_period > 0xFFFF) {
		new_vd_period = 0xFFFF;
		update_input_frame_rate = (mode_info->sen_vd[0]->period * (ori_fps_src * target_fps_div)) / new_vd_period;
		DBG_ERR("frame rate %d/%d out of sensor driver range, force to sensor driver min support fps %d\r\n"
				, target_fps_src, target_fps_div, update_input_frame_rate);
	}
	if (new_vd_period < 0x0002) {
		new_vd_period = 0x0002;
		update_input_frame_rate = (mode_info->sen_vd[0]->period * (ori_fps_src * target_fps_div)) / new_vd_period;
		DBG_ERR("frame rate %d/%d out of sensor driver range, force to sensor driver max support fps %d\r\n"
				, target_fps_src, target_fps_div, update_input_frame_rate);
	}

	//Set new period
	vd_period[id] = new_vd_period;

	return rt;
}

static ER set_grs_mode_GC2053P(SENSOR_ID id, void *param)
{
	return E_OK;
}

static ER set_hdr_info_GC2053P(SENSOR_ID id, HDR_SET_INFO *hdr_info)
{
	return E_OK;
}

static ER set_user_option_GC2053P(SENSOR_ID id, UINT32 option)
{
	ER rt = E_OK;
	SENSOR_CMD cmd;
	UINT32 cmd_value, init_option = sensor_option_init[id];

	//Set sensor option
	if (option & SENSOR_OPTION_MIRROR) {
		sensor_option[id] |= SENSOR_OPTION_MIRROR;
	} else {
		sensor_option[id] &= ~SENSOR_OPTION_MIRROR;
	}
    
	if (option & SENSOR_OPTION_FLIP) {
		sensor_option[id] |= SENSOR_OPTION_FLIP;
	} else {
		sensor_option[id] &= ~SENSOR_OPTION_FLIP;
	}

	//Set sensor register
	cmd = set_cmd_info_GC2053P(0x3007, 1, 0x0, 0x0);
	rt |= read_reg_GC2053P(id, &cmd);
	cmd_value = cmd.ui_data[0];
    
	if ((option & SENSOR_OPTION_MIRROR) != (init_option & SENSOR_OPTION_MIRROR)) {
		cmd_value = cmd_value | 0x02;
	} else {
		cmd_value = cmd_value & (~0x02);
	}
    
	if ((option & SENSOR_OPTION_FLIP) != (init_option & SENSOR_OPTION_FLIP)) {
		cmd_value = cmd_value | 0x01;
	} else {
		cmd_value = cmd_value & (~0x01);
	}
    
	cmd = set_cmd_info_GC2053P(0x3007, 1, cmd_value, 0x0);
	rt |= write_reg_GC2053P(id, &cmd);

	if (rt != E_OK) {
		DBG_ERR("set sensor option error %d\r\n", rt);
	}

	return E_OK;
}

#if 0
-Extern api
#endif
SENSOR_DRV_TAB *sensor_get_drv_tab_GC2053P(void)
{
	return &m_p_GC2053P_drv;
}
//@@ EOF
