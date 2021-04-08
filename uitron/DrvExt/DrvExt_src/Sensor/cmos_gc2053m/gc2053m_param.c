/**
    General Sensor API for project layer

    Sample module detailed description.

    @file       gc2053m_param.c
    @ingroup    Predefined_group_name
    @note       Nothing (or anything need to be mentioned).

    Copyright   Novatek Microelectronics Corp. 2011.  All rights reserved.
*/

#include "gc2053m_param_int.h"
#include "sensor_if.h"
#include "csi.h"
#include "string.h"
#define __MODULE__ GC2053M_PARAM
#define __DBGLVL__ 1        //ERROR mode, show err, wrn only
#define __DBGFLT__ "*"      // *=All
#include "DebugModule.h"

#define FOUR_LANE DISABLE

static SENSOR_MULTI_FRM_INFO multi_frame_info_gc2053m[MODE_MAX + 1] = {
    {0, {0, 0}, 0}, // NONE
    {1, {0, 0}, SEN_BIT_OFS_NONE}, // SENSOR_MODE_1            
};

static SENSOR_MODE_MIPI mipi_mode_gc2053m[MODE_MAX + 1] = {
    {0},
    {
        CSI_DATLANE_NUM_2,
        SEN_MIPI_PIXDEPTH_10BIT,
        {{SEN_MIPI_MANUAL_NONE, 0}, {SEN_MIPI_MANUAL_NONE, 0}, {SEN_MIPI_MANUAL_NONE, 0},},
        FALSE
    },            
};

static SENSOR_SIGNAL hd_trans_gc2053m[MODE_MAX + 1] = {
    {0, 0, 0, 0},
    {0, 0, 8, 1920}, 	
};

static SENSOR_SIGNAL vd_trans_gc2053m[MODE_MAX + 1] = {
    {0, 0, 0, 0},
    {0, 0, 2, 1080},	
};

static SENSOR_SIGNAL hd_sen_gc2053m[MODE_MAX + 1] = {
    {0, 0, 0, 0},
    {0, 2200, 0, 1928},//T_row=2200/74.25=29.6us
};

static SENSOR_SIGNAL vd_sen_gc2053m[MODE_MAX + 1] = {
    {0, 0, 0, 0},
    {0, 1125, 0, 1088},// vts=windown_height+vb+20=1088+6+20=1114	
};

static SENSOR_MODE_INFO mode_gc2053m[MODE_MAX + 1] = {
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
        //window_size=1928*1080,mipi 2lane
        SENSOR_MODE_LINEAR,
        SENSOR_STPIX_R,
        SENSOR_FMT_POGRESSIVE,
        1920,
        1080,
        {SEN_RATIO(16, 9), 1000, 1000, 1000, 1000},
        SEN_FPS(30, 1),
        74250000,
        74250000,
        100,
		{1000, 1024000},
        {0, 0, 0,},
        {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        
        &multi_frame_info_gc2053m[SENSOR_MODE_1],
        {&hd_trans_gc2053m[SENSOR_MODE_1], NULL},
        {&vd_trans_gc2053m[SENSOR_MODE_1], NULL},
        {&hd_sen_gc2053m[SENSOR_MODE_1], NULL},
        {&vd_sen_gc2053m[SENSOR_MODE_1], NULL},
        
        NULL,
        &mipi_mode_gc2053m[SENSOR_MODE_1],
        NULL,
        NULL,
    },
};

static SEN_CMD_INFO gc2053m_cmd_info = {
    {
        //{SEN_I2C_W_ADDR_DFT, 0x6E},
		{SEN_I2C_W_ADDR_DFT, 0x6E},	
        {SEN_I2C_W_ADDR_OPTION1, 0xff},
        {SEN_I2C_W_ADDR_OPTION2, 0xff},
        {SEN_I2C_W_ADDR_OPTION3, 0xff},
        {SEN_I2C_W_ADDR_OPTION4, 0xff},
        {SEN_I2C_W_ADDR_OPTION5, 0xff},
    },
};

SEN_CMD_INFO *get_sensor_cmd_info_gc2053m(void)
{
	return &gc2053m_cmd_info;
}

static SEN_IF_INFO gc2053m_if_info = {
    {
        NULL,
        NULL,
    },
};

SEN_IF_INFO *get_sensor_if_info_gc2053m(void)
{
    return &gc2053m_if_info;
}

static SENSOR_DRV_INFO g_p_gc2053m_drv_info = {
    SENSOR_TYPE_CMOS,
    SENSOR_SIGNAL_MASTER,
    SENSOR_DATA_MIPI,
    {FALSE},
    {SENSOR_ACTIVE_HIGH, SENSOR_ACTIVE_HIGH, SENSOR_PHASE_RISING, SENSOR_PHASE_RISING, SENSOR_PHASE_RISING},
    {TRUE},
    {27000000},
     SENSOR_SUPPORT_MIRROR|SENSOR_SUPPORT_FLIP,
	{5640, 3216},  //1945*2.9, 1109*2.9		
	{1920, 1080},
	0,
    0,
};

static SENSOR_CMD gc2053m_mode_1[] = {
    {0xfe, 1, {0x80, 0x0}},
    {0xfe, 1, {0x80, 0x0}},
    {0xfe, 1, {0x80, 0x0}},
    {0xfe, 1, {0x00, 0x0}},//P0
    {0xf2, 1, {0x00, 0x0}},//[1]I2C_open_ena [0]pwd_dn
    {0xf3, 1, {0x00, 0x0}},//0f//00[3]Sdata_pad_io [2:0]Ssync_pad_io 
    {0xf4, 1, {0x36, 0x0}},//[6:4]pll_ldo_set
    {0xf5, 1, {0xc0, 0x0}},//[7]soc_mclk_enable [6]pll_ldo_en [5:4]cp_clk_sel [3:0]cp_clk_div
    {0xf6, 1, {0x44, 0x0}},//[7:3]wpllclk_div [2:0]refmp_div
    {0xf7, 1, {0x01, 0x0}},//[7]refdiv2d5_en [6]refdiv1d5_en [5:4]scaler_mode [3]refmp_enb [1]div2en [0]pllmp_en
    {0xf8, 1, {0x2c, 0x0}},//[7:0]pllmp_div
    {0xf9, 1, {0x42, 0x0}},//[7:3]rpllclk_div [2:1]pllmp_prediv [0]analog_pwc
    {0xfc, 1, {0x8e, 0x0}},
    /****CISCTL & ANALOG****/
    {0xfe, 1, {0x00, 0x0}},
    {0x87, 1, {0x18, 0x0}},//[6]aec_delay_mode
    {0xee, 1, {0x30, 0x0}},//[5:4]dwen_sramen
    {0xd0, 1, {0xb7, 0x0}},//ramp_en
    {0x03, 1, {0x04, 0x0}},
    {0x04, 1, {0x60, 0x0}},
    {0x05, 1, {0x04, 0x0}},//05
    {0x06, 1, {0x4c, 0x0}},//60//[11:0]hb
    {0x07, 1, {0x00, 0x0}},
    {0x08, 1, {0x06, 0x0}},
    {0x09, 1, {0x00, 0x0}}, 
    {0x0a, 1, {0x02, 0x0}}, //cisctl row start
    {0x0b, 1, {0x00, 0x0}},
    {0x0c, 1, {0x02, 0x0}}, //cisctl col start
    {0x0d, 1, {0x04, 0x0}},
    {0x0e, 1, {0x40, 0x0}},
    {0x12, 1, {0xe2, 0x0}}, //vsync_ahead_mode
    {0x13, 1, {0x16, 0x0}},
    {0x19, 1, {0x0a, 0x0}}, //ad_pipe_num
    {0x21, 1, {0x1c, 0x0}},//eqc1fc_eqc2fc_sw
    {0x28, 1, {0x0a, 0x0}},//16//eqc2_c2clpen_sw
    {0x29, 1, {0x24, 0x0}},//eq_post_width
    {0x2b, 1, {0x04, 0x0}},//c2clpen --eqc2
    {0x32, 1, {0xd8, 0x0}}, //[5]txh_en ->avdd28
    {0x37, 1, {0x03, 0x0}}, //[3:2]eqc2sel=0
    {0x39, 1, {0x15, 0x0}},//17 //[3:0]rsgl
    {0x43, 1, {0x07, 0x0}},//vclamp
    {0x44, 1, {0x40, 0x0}}, //0e//post_tx_width
    {0x4b, 1, {0x20, 0x0}}, //rst_tx_width
    {0x4e, 1, {0x08, 0x0}}, //12//ramp_t1_width
    {0x55, 1, {0x20, 0x0}}, //read_tx_width_pp
    {0x66, 1, {0x05, 0x0}}, //18//stspd_width_r1
    {0x67, 1, {0x05, 0x0}}, //40//5//stspd_width_r
    {0x77, 1, {0x01, 0x0}}, //dacin offset x31
    {0x78, 1, {0x00, 0x0}}, //dacin offset
    {0x7c, 1, {0x93, 0x0}}, //[1:0] co1comp
    {0x8c, 1, {0x12, 0x0}}, //12 ramp_t1_ref
    {0x8d, 1, {0x92, 0x0}},//90
    {0x90, 1, {0x00, 0x0}},
    {0x41, 1, {0x04, 0x0}},//high byte
    {0x42, 1, {0x5a, 0x0}},//low byte
    {0x9d, 1, {0x10, 0x0}},
    {0xce, 1, {0x7c, 0x0}},//70//78//[4:2]c1isel
    {0xd2, 1, {0x41, 0x0}},//[5:3]c2clamp
    {0xd3, 1, {0xdc, 0x0}},//ec//0x39[7]=0,0xd3[3]=1 rsgh=vref
    {0xe6, 1, {0x50, 0x0}},//ramps offset
    /*gain*/
    {0xb6, 1, {0xc0, 0x0}},
    {0xb0, 1, {0x70, 0x0}},
    /*blk*/
    {0x26, 1, {0x30, 0x0}},//23 //[4]?0¡A¥þn mode
    {0xfe, 1, {0x01, 0x0}},
    {0x40, 1, {0x23, 0x0}},
    {0x55, 1, {0x07, 0x0}},
    {0x60, 1, {0x40, 0x0}}, //[7:0]WB_offset
    {0xfe, 1, {0x04, 0x0}},
    {0x14, 1, {0x78, 0x0}}, //g1 ratio
    {0x15, 1, {0x78, 0x0}}, //r ratio
    {0x16, 1, {0x78, 0x0}}, //b ratio
    {0x17, 1, {0x78, 0x0}}, //g2 ratio
    /*window*/
    {0xfe, 1, {0x01, 0x0}},
    {0x94, 1, {0x01, 0x0}},
    {0x95, 1, {0x04, 0x0}},
    {0x96, 1, {0x40, 0x0}},//[10:0]out_height
    {0x97, 1, {0x07, 0x0}},
    {0x98, 1, {0x88, 0x0}}, //[11:0]out_width
    {0x99, 1, {0x05, 0x0}}, //0x05 mirror¡Bflip auto cfa
    /*ISP*/
    {0xfe, 1, {0x01, 0x0}},  
    {0x01, 1, {0x05, 0x0}}, //03//[3]dpc blending mode [2]noise_mode [1:0]center_choose 2b'11:median 2b'10:avg 2'b00:near
    {0x02, 1, {0x89, 0x0}}, //[7:0]BFF_sram_mode
    {0x04, 1, {0x01, 0x0}}, //[0]DD_en ; 0:off
    {0x07, 1, {0xa6, 0x0}}, //0x76
    {0x08, 1, {0xa9, 0x0}}, //0x69
    {0x09, 1, {0xa8, 0x0}}, //0x58
    {0x0a, 1, {0xa7, 0x0}}, //0x56
    {0x0b, 1, {0xff, 0x0}}, //0x32
    {0x0c, 1, {0xff, 0x0}}, //0x40
    {0x0f, 1, {0x00, 0x0}},
    {0x50, 1, {0x1c, 0x0}},
    {0xfe, 1, {0x04, 0x0}},
    {0x28, 1, {0x86, 0x0}},
    {0x29, 1, {0x86, 0x0}},
    {0x2a, 1, {0x86, 0x0}},
    {0x2b, 1, {0x68, 0x0}},
    {0x2c, 1, {0x68, 0x0}},
    {0x2d, 1, {0x68, 0x0}},
    {0x2e, 1, {0x68, 0x0}},
    {0x2f, 1, {0x68, 0x0}},
    {0x30, 1, {0x4f, 0x0}},
    {0x31, 1, {0x68, 0x0}},
    {0x32, 1, {0x67, 0x0}},
    {0x33, 1, {0x66, 0x0}},
    {0x34, 1, {0x66, 0x0}},
    {0x35, 1, {0x66, 0x0}},
    {0x36, 1, {0x66, 0x0}},
    {0x37, 1, {0x65, 0x0}},
    {0x38, 1, {0x62, 0x0}},
    {0x39, 1, {0x62, 0x0}},
    {0x3a, 1, {0x62, 0x0}},
    {0x3b, 1, {0x62, 0x0}},
    {0x3c, 1, {0x62, 0x0}},
    {0x3d, 1, {0x62, 0x0}},
    {0x3e, 1, {0x62, 0x0}},
    {0x3f, 1, {0x62, 0x0}},
    {0xfe, 1, {0x01, 0x0}},
    {0x89, 1, {0x03, 0x0}},
    /****DVP & MIPI****/
    {0xfe, 1 ,{0x01, 0x0}},
    {0x9a, 1 ,{0x06, 0x0}},//[5]OUT_gate_mode [4]hsync_delay_half_pclk [3]data_delay_half_pclk [2]vsync_polarity [1]hsync_polarity [0]pclk_out_polarity
    {0xfe, 1 ,{0x00, 0x0}},
    {0x7b, 1 ,{0x2a, 0x0}},//[7:6]updn [5:4]drv_high_data [3:2]drv_low_data [1:0]drv_pclk
    {0x23, 1 ,{0x2d, 0x0}},//[3]rst_rc [2:1]drv_sync [0]pwd_rc
    {0xfe, 1 ,{0x03, 0x0}},
    {0x01, 1 ,{0x27, 0x0}},//20//27[6:5]clkctr [2]phy-lane1_en [1]phy-lane0_en [0]phy_clk_en
    {0x02, 1 ,{0x54, 0x0}},//[7:6]data1ctr [5:4]data0ctr [3:0]mipi_diff
    {0x03, 1 ,{0xb6, 0x0}},//b2//b6[7]clklane_p2s_sel [6:5]data0hs_ph [4]data0_delay1s [3]clkdelay1s [2]mipi_en [1:0]clkhs_ph
    {0x12, 1 ,{0x80, 0x0}},
    {0x13, 1 ,{0x07, 0x0}},//LWC
    {0x15, 1 ,{0x10, 0x0}},//[1:0]clk_lane_mode    
	{0x29, 2, {0x02, 0x0}},
    {0xfe, 1 ,{0x00, 0x0}},//P0
    {0x3e, 1 ,{0x91, 0x0}},//40//91[7]lane_ena [6]DVPBUF_ena [5]ULPEna [4]MIPI_ena [3]mipi_set_auto_disable [2]RAW8_mode [1]ine_sync_mode [0]double_lane_en 
}; 

SENSOR_MODE_INFO *get_mode_info_gc2053m(SENSOR_MODE mode)
{
    if (mode >= (MODE_MAX + 1)) {
        DBG_ERR("sensor mode %d > Max(%d), force to mode 0\r\n", mode, MODE_MAX);
        //return NULL;
        mode = 0;
    }
    return &mode_gc2053m[mode];
}

UINT32 get_cmd_tab_gc2053m(SENSOR_MODE mode, SENSOR_CMD **cmd_tab)
{
    switch (mode) {
    case SENSOR_MODE_1:
        *cmd_tab = gc2053m_mode_1;
        return sizeof(gc2053m_mode_1) / sizeof(SENSOR_CMD);
    
    default:
        DBG_ERR("sensor mode %d no cmd table\r\n", mode);
        *cmd_tab = NULL;
        return 0;
    }
}

SENSOR_DRV_INFO *get_sensor_drv_info_gc2053m(void)
{
    return &g_p_gc2053m_drv_info;
}
static UINT32 cur_frame_rate[SENSOR_ID_MAX_NUM][MODE_MAX + 1] = {0};
static UINT32 chgmode_frame_rate[SENSOR_ID_MAX_NUM][MODE_MAX + 1] = {0};
void set_sensor_cur_frame_rate_gc2053m(SENSOR_ID id, SENSOR_MODE mode, UINT32 frame_rate)
{
    cur_frame_rate[id][mode] = frame_rate;
}
UINT32 get_sensor_cur_frame_rate_gc2053m(SENSOR_ID id, SENSOR_MODE mode)
{
    return cur_frame_rate[id][mode];
}
void set_sensor_chgmode_frame_rate_gc2053m(SENSOR_ID id, SENSOR_MODE mode, UINT32 frame_rate)
{
    chgmode_frame_rate[id][mode] = frame_rate;
    cur_frame_rate[id][mode] = frame_rate;
}
UINT32 get_sensor_chgmode_frame_rate_gc2053m(SENSOR_ID id, SENSOR_MODE mode)
{
    return chgmode_frame_rate[id][mode];
}
