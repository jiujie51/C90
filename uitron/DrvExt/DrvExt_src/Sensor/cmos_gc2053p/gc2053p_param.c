/**
    General Sensor API for project layer

    Sample module detailed description.

    @file       GC2053P_param.c
    @ingroup    Predefined_group_name
    @note       Nothing (or anything need to be mentioned).

    Copyright   Novatek Microelectronics Corp. 2011.  All rights reserved.
*/

#include "gc2053p_param.h"
#define __MODULE__ GC2053P_PARAM
#define __DBGLVL__ 1        //ERROR mode, show err, wrn only
#define __DBGFLT__ "*"      // *=All
#include "DebugModule.h"

static SENSOR_MULTI_FRM_INFO multi_frame_info_GC2053P[MODE_MAX + 1] = {
	{0, {0, 0}, 0}, // NONE
	{1, {0, 0}, SEN_BIT_OFS_NONE}, // SENSOR_MODE_1
	{1, {0, 0}, SEN_BIT_OFS_NONE}, // SENSOR_MODE_2
};

static SENSOR_SIGNAL hd_trans_gc2053p[MODE_MAX + 1] = {
    {0, 0, 0, 0},
    {0, 0,  4, 1920},
    {0, 0,  4, 1920},
};

static SENSOR_SIGNAL vd_trans_gc2053p[MODE_MAX + 1] = {
    {0, 0, 0, 0},
    {0, 0,  4, 1080},
    {0, 0,  4, 1080},

};


static SENSOR_SIGNAL hd_sen_gc2053p[MODE_MAX + 1] = {
    {0, 0, 0, 0},
	{0, 2132,  0, 1928},
	{0, 2132,  0, 1928},
};

static SENSOR_SIGNAL vd_sen_gc2053p[MODE_MAX + 1] = {
    {0, 0, 0, 0},
	{0, 1137,  4, 1080},
	{0, 1137,  4, 1080},
};

static SENSOR_MODE_INFO mode_GC2053P[MODE_MAX + 1] = {
	{
		//null
		0,
		0,
		0,
		0,
		0,
		{0, 0, 0, 0, 0},
		0,
		0,
		0,
		0,
		{0, 0},
		{0, 0, 0,},

		{{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
		NULL,

		NULL,
		NULL,
		NULL,
		NULL,

		NULL,
		NULL,
		NULL,
		NULL,
	},

	{
		// 1920*1080 p30 linear 2lane
		SENSOR_MODE_LINEAR,
		SENSOR_STPIX_R,
		SENSOR_FMT_POGRESSIVE,		
		1920,
		1080,
		{SEN_RATIO(16, 9), 1000, 1000, 1000, 1000},
		SEN_FPS(30, 1),
		81000000,
		81000000,
		100,
		{1000, 1024000},
		{0, 0, 0,},
		{{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},

		&multi_frame_info_GC2053P[SENSOR_MODE_1],
		{&hd_trans_gc2053p[SENSOR_MODE_1], NULL},
		{&vd_trans_gc2053p[SENSOR_MODE_1], NULL},
		{&hd_sen_gc2053p[SENSOR_MODE_1], NULL},
        {&vd_sen_gc2053p[SENSOR_MODE_1], NULL},
        NULL,
        NULL,
        NULL,
        NULL,
	},

	{
        // 1920*1080 p30 linear 2lane
        SENSOR_MODE_LINEAR,
        SENSOR_STPIX_R,
        SENSOR_FMT_POGRESSIVE,		
        1920,
        1080,
        {SEN_RATIO(16, 9), 1000, 1000, 1000, 1000},
        SEN_FPS(30, 1),
        81000000,
        81000000,
        100,
        {1000, 1024000},
        {0, 0, 0,},
        {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        
        &multi_frame_info_GC2053P[SENSOR_MODE_1],
        {&hd_trans_gc2053p[SENSOR_MODE_1], NULL},
        {&vd_trans_gc2053p[SENSOR_MODE_1], NULL},
        {&hd_sen_gc2053p[SENSOR_MODE_1], NULL},
        {&vd_sen_gc2053p[SENSOR_MODE_1], NULL},
        NULL,
        NULL,
        NULL,
        NULL,
	},    
};

static SEN_CMD_INFO GC2053P_cmd_info = {
	{
		{SEN_I2C_W_ADDR_DFT,     0x6e},
		{SEN_I2C_W_ADDR_OPTION1, 0xff},
		{SEN_I2C_W_ADDR_OPTION2, 0xff},
		{SEN_I2C_W_ADDR_OPTION3, 0xff},
		{SEN_I2C_W_ADDR_OPTION4, 0xff},
		{SEN_I2C_W_ADDR_OPTION5, 0xff},
	},
};

SEN_CMD_INFO *get_sensor_cmd_info_GC2053P(void)
{
	return &GC2053P_cmd_info;
}

static SEN_IF_INFO GC2053P_if_info = {
	{
		NULL,
		NULL,
	},
};

SEN_IF_INFO *get_sensor_if_info_GC2053P(void)
{
	return &GC2053P_if_info;
}

static SENSOR_DRV_INFO g_p_GC2053P_drv_info = {
	SENSOR_TYPE_CMOS,
	SENSOR_SIGNAL_MASTER,
	SENSOR_DATA_PARALLEL,
	{FALSE},
	{SENSOR_ACTIVE_HIGH, SENSOR_ACTIVE_LOW, SENSOR_PHASE_RISING, SENSOR_PHASE_RISING, SENSOR_PHASE_RISING},
	{TRUE},
	{24000000},
	SENSOR_SUPPORT_MIRROR|SENSOR_SUPPORT_FLIP,
	{3000, 3000},
	{1920, 1080},
	0,
	0,
};

//FHDP30
static SENSOR_CMD GC2053P_mode_1[] = {
	/*system*/
	{0xfe,2,{0x80,0x00}},
	{0xfe,2,{0x80,0x00}},
	{0xfe,2,{0x80,0x00}},
	{0xfe,2,{0x00,0x00}},
	{0xf2,2,{0x00,0x00}}, //[1]I2C_open_ena [0]pwd_dn
	{0xf3,2,{0x0f,0x00}}, //00[3]Sdata_pad_io [2:0]Ssync_pad_io
	{0xf4,2,{0x36,0x00}}, //[6:4]pll_ldo_set
	{0xf5,2,{0xc0,0x00}}, //[7]soc_mclk_enable [6]pll_ldo_en [5:4]cp_clk_sel [3:0]cp_clk_div
	{0xf6,2,{0x44,0x00}}, //[7:3]wpllclk_div [2:0]refmp_div
	{0xf7,2,{0x01,0x00}}, //[7]refdiv2d5_en [6]refdiv1d5_en [5:4]scaler_mode [3]refmp_enb [1]div2en [0]pllmp_en
	{0xf8,2,{0x36,0x00}}, //38////38//[7:0]pllmp_div
	{0xf9,2,{0x42,0x00}}, //82//[7:3]rpllclk_div [2:1]pllmp_prediv [0]analog_pwc
	{0xfc,2,{0x8e,0x00}},
	
	{0xfe,2,{0x00,0x00}},
	{0x87,2,{0x18,0x00}}, //[6]aec_delay_mode
	{0xee,2,{0x30,0x00}}, //[5:4]dwen_sramen
	{0xd0,2,{0xb7,0x00}}, //ramp_en
	{0x03,2,{0x04,0x00}},
	{0x04,2,{0x60,0x00}},
	{0x05,2,{0x04,0x00}},//05
	{0x06,2,{0x2a,0x00}}, //28//[11:0]hb
	{0x07,2,{0x00,0x00}},
	{0x08,2,{0x1d,0x00}},
	{0x09,2,{0x00,0x00}}, 
	{0x0a,2,{0x04,0x00}}, //cisctl row start
	{0x0b,2,{0x00,0x00}},
	{0x0c,2,{0x02,0x00}}, //cisctl col start
	
	{0x0d,2,{0x04,0x00}},
	{0x0e,2,{0x40,0x00}},
	{0x12,2,{0xe2,0x00}}, //vsync_ahead_mode
	{0x13,2,{0x16,0x00}},
	{0x19,2,{0x0a,0x00}}, //ad_pipe_num
	{0x21,2,{0x1c,0x00}}, //eqc1fc_eqc2fc_sw
	{0x28,2,{0x0a,0x00}}, //16//eqc2_c2clpen_sw
	{0x29,2,{0x24,0x00}}, //eq_post_width
	{0x2b,2,{0x04,0x00}}, //c2clpen --eqc2
	{0x32,2,{0xd8,0x00}}, //0x32[7]=1,0xd3[7]=1 rsth=vref
	{0x37,2,{0x03,0x00}}, //[3:2]eqc2sel=0
	{0x39,2,{0x15,0x00}},//17 //[3:0]rsgl
	{0x43,2,{0x07,0x00}},
	{0x44,2,{0x40,0x00}}, //0e//post_tx_width
	{0x4b,2,{0x20,0x00}}, //rst_tx_width
	{0x4e,2,{0x08,0x00}}, //12//ramp_t1_width
	{0x55,2,{0x20,0x00}}, //read_tx_width_pp
	{0x66,2,{0x05,0x00}}, //18//stspd_width_r1
	{0x67,2,{0x05,0x00}}, //40//5//stspd_width_r
	{0x77,2,{0x01,0x00}}, //dacin offset x31
	{0x78,2,{0x00,0x00}}, //dacin offset
	{0x7c,2,{0x93,0x00}}, //[1:0] co1comp
	{0x8c,2,{0x12,0x00}}, //12 ramp_t1_ref
	{0x8d,2,{0x92,0x00}},
	{0x90,2,{0x00,0x00}},
	{0x41,2,{0x04,0x00}},
	{0x42,2,{0x71,0x00}}, //0x64
	{0x9d,2,{0x10,0x00}},
	{0xce,2,{0x7c,0x00}},//70//78//[4:2]c1isel
	{0xd2,2,{0x41,0x00}},//[5:3]c2clamp
	{0xd3,2,{0xdc,0x00}},//ec//0x39[7]=0,0xd3[3]=1 rsgh=vref
	{0xe6,2,{0x50,0x00}},//ramps offset
	
	{0xb6,2,{0x80,0x00}},
	{0xb3,2,{0x00,0x00}},
	{0xb4,2,{0x00,0x00}},
	{0xb0,2,{0x5a,0x00}},//58
	
	{0x26,2,{0x30,0x00}},//23 //[4]Ð´0£¬È«n mode
	{0xfe,2,{0x01,0x00}},
	{0x40,2,{0x23,0x00}},
	{0x55,2,{0x07,0x00}},
	{0x60,2,{0x00,0x00}}, //[7:0]WB_offset
	{0xfe,2,{0x04,0x00}},
	{0x14,2,{0x78,0x00}}, //g1 ratio
	{0x15,2,{0x78,0x00}}, //r ratio
	{0x16,2,{0x78,0x00}}, //b ratio
	{0x17,2,{0x78,0x00}}, //g2 ratio
	
	{0xfe,2,{0x01,0x00}},
	{0x94,2,{0x01,0x00}},
	{0x95,2,{0x04,0x00}},
	{0x96,2,{0x40,0x00}},//38 //[10:0]out_height
	{0x97,2,{0x07,0x00}},
	{0x98,2,{0x88,0x00}},//80 //[11:0]out_width
	
	{0xfe,2,{0x01,0x00}},
	{0x02,2,{0x89,0x00}}, //[7:0]BFF_sram_mode
	{0x04,2,{0x01,0x00}}, //[0]DD_en
	{0x50,2,{0x1c,0x00}},
	{0x01,2,{0x05,0x00}},//03//[3]dpc blending mode [2]noise_mode [1:0]center_choose 2b'11:median 2b'10:avg 2'b00:near
	{0x0f,2,{0x00,0x00}},
	{0xfe,2,{0x04,0x00}},
	{0x28,2,{0x68,0x00}},//84
	{0x29,2,{0x68,0x00}},//84
	{0x2a,2,{0x68,0x00}},//84
	{0x2b,2,{0x68,0x00}},//84
	{0x2c,2,{0x68,0x00}},//84
	{0x2d,2,{0x68,0x00}},//84
	{0x2e,2,{0x68,0x00}},//83
	{0x2f,2,{0x68,0x00}},//82
	{0x30,2,{0x68,0x00}},//82
	{0x31,2,{0x67,0x00}},//82
	{0x32,2,{0x66,0x00}},//82
	{0x33,2,{0x66,0x00}},//82
	{0x34,2,{0x66,0x00}},//82
	{0x35,2,{0x66,0x00}},//82
	{0x36,2,{0x66,0x00}},//64
	{0x37,2,{0x65,0x00}},//68
	{0x38,2,{0x62,0x00}},
	{0x39,2,{0x62,0x00}},
	{0x3a,2,{0x62,0x00}},
	{0x3b,2,{0x62,0x00}},
	{0x3c,2,{0x62,0x00}},
	{0x3d,2,{0x62,0x00}},
	{0x3e,2,{0x62,0x00}},
	{0x3f,2,{0x62,0x00}},
	{0xfe,2,{0x01,0x00}},
	{0x89,2,{0x03,0x00}},
	
	{0xfe,2,{0x01,0x00}},
	{0x9a,2,{0x06,0x00}}, //[5]OUT_gate_mode [4]hsync_delay_half_pclk [3]data_delay_half_pclk [2]vsync_polarity [1]hsync_polarity [0]pclk_out_polarity
	{0xfe,2,{0x00,0x00}},
	{0x7b,2,{0x2a,0x00}}, //[7:6]updn [5:4]drv_high_data [3:2]drv_low_data [1:0]drv_pclk
	{0x23,2,{0x2d,0x00}}, //[3]rst_rc [2:1]drv_sync [0]pwd_rc
	{0xfe,2,{0x03,0x00}},
	{0x01,2,{0x20,0x00}}, //27[6:5]clkctr [2]phy-lane1_en [1]phy-lane0_en [0]phy_clk_en
	{0x02,2,{0x56,0x00}}, //[7:6]data1ctr [5:4]data0ctr [3:0]mipi_diff
	{0x03,2,{0xb2,0x00}}, //b6[7]clklane_p2s_sel [6:5]data0hs_ph [4]data0_delay1s [3]clkdelay1s [2]mipi_en [1:0]clkhs_ph
	{0x12,2,{0x88,0x00}},//80
	{0x13,2,{0x07,0x00}}, //LWC
	
	{0xfe,2,{0x00,0x00}},
	{0x3e,2,{0x40,0x00}}, //91[7]lane_ena [6]DVPBUF_ena [5]ULPEna [4]MIPI_ena [3]mipi_set_auto_disable [2]RAW8_mode [1]ine_sync_mode [0]double_lane_en
	//{0x17,2,{0x83,0x00}},
	{0x17,2,{0x80,0x00}},
};

SENSOR_MODE_INFO *get_mode_info_GC2053P(SENSOR_MODE mode)
{
	if (mode >= (MODE_MAX + 1)) {
		DBG_ERR("sensor mode %d > Max(%d), force to mode 0\r\n", mode, MODE_MAX);
		//return NULL;
		mode = 0;
	}
	return &mode_GC2053P[mode];
}

UINT32 get_cmd_tab_GC2053P(SENSOR_MODE mode, SENSOR_CMD **cmd_tab)
{
	switch (mode) {
	case SENSOR_MODE_1:
		*cmd_tab = GC2053P_mode_1;
		return sizeof(GC2053P_mode_1) / sizeof(SENSOR_CMD);

	case SENSOR_MODE_2:
		*cmd_tab = GC2053P_mode_1;
		return sizeof(GC2053P_mode_1) / sizeof(SENSOR_CMD);

	default:
		DBG_ERR("sensor mode %d no cmd table\r\n", mode);
		*cmd_tab = NULL;
		return 0;
	}
}

SENSOR_DRV_INFO *get_sensor_drv_info_GC2053P(void)
{
	return &g_p_GC2053P_drv_info;
}
static UINT32 cur_frame_rate[SENSOR_ID_MAX_NUM][MODE_MAX + 1] = {0};
static UINT32 chgmode_frame_rate[SENSOR_ID_MAX_NUM][MODE_MAX + 1] = {0};
void set_sensor_cur_frame_rate_GC2053P(SENSOR_ID id, SENSOR_MODE mode, UINT32 frame_rate)
{
	cur_frame_rate[id][mode] = frame_rate;
}
UINT32 get_sensor_cur_frame_rate_GC2053P(SENSOR_ID id, SENSOR_MODE mode)
{
	return cur_frame_rate[id][mode];
}
void set_sensor_chgmode_frame_rate_GC2053P(SENSOR_ID id, SENSOR_MODE mode, UINT32 frame_rate)
{
	chgmode_frame_rate[id][mode] = frame_rate;
	cur_frame_rate[id][mode] = frame_rate;
}
UINT32 get_sensor_chgmode_frame_rate_GC2053P(SENSOR_ID id, SENSOR_MODE mode)
{
	return chgmode_frame_rate[id][mode];
}
