/**
    AWB parameter

    @file       awb_gc2053_param_evb
    @ingroup    LibExt\AWB
    @note       Nothing (or anything need to be mentioned).

    Copyright   Novatek Microelectronics Corp. 2017.  All rights reserved.
*/
#include "Type.h"
#include "awb_alg.h"
#include "awb_api.h"
#include "awb_proc_nvt_int.h"
#include "pq_obj_drv.h"
/******************************/
/**
    AWB CA infomation definition
*/
static awb_ca_info awb_param_ca_info = {
    AWB_WIN, AWB_WIN,
    4095,       //UINT32 RGu; (R/G)<<10
    0,          //UINT32 RGl;
    4095,       //UINT32 GGu; G (12Bits)
    0,          //UINT32 GGl;
    4095,       //UINT32 BGu; (B/G)<<10
    0,          //UINT32 BGl;
    4095,       //UINT32 RBGu;((R+B)/G)<<9
    0           //UINT32 RBGl;
};

/******************************/
/**
    CT gain
*/
#define AWB_CT_2300K_RGAIN 219
#define AWB_CT_2300K_BGAIN 803

#define AWB_CT_2800K_RGAIN 248
#define AWB_CT_2800K_BGAIN 694

#define AWB_CT_3700K_RGAIN 332
#define AWB_CT_3700K_BGAIN 611

#define AWB_CT_4700K_RGAIN 397
#define AWB_CT_4700K_BGAIN 548

#define AWB_CT_6500K_RGAIN 425
#define AWB_CT_6500K_BGAIN 436

#define AWB_CT_11000K_RGAIN 525
#define AWB_CT_11000K_BGAIN 383



/******************************/
/**
    AWB Method1 table
*/
	/*awb_th_y=5,245
	awb_th_rpb=150,358
	awb_th_rsb=-37,161
	awb_th_r2g=94,230
	awb_th_b2g=56,128
	awb_th_rmb=79,109*/

/*awb_th_y=10,255
awb_th_rpb=150,358
awb_th_rsb=-37,161
awb_th_r2g=84,230
awb_th_b2g=56,120
awb_th_rmb=62,114
*/

static awb_th awb_param_th = {
    5, //YL
    235, //YU
    86, //RpBL
    267, //RpBU
    -30, //RsBL
    140, //RsBU
    56, //R2GL
    182, //R2GU
    30, //B2GL
    84, //B2GU
    28, //RmBL
    55, //RmBU
};

/******************************/
/**
    LV check
*/
static awb_lv_check awb_param_lv_check[AWB_TUNING_CHECK_LV_MAX] = {
    { 400, 100}, //LV4~LV1
    { 1000, 600}, //LV10~LV6
    { 2600, 1200}, //LV26~LV12
};

/******************************/
/**
    CT Weight
*/
static ctmp_lut ct_map[AWB_TUNING_CT_WEIGHT_MAX] = {
     {776, 2300},
     {820, 2800},
     {910, 3700},
     {966, 4700},
     {1020, 6500},
     {1083, 11000},
};
static cwgt_lut out_cwgt_lut[AWB_TUNING_CT_WEIGHT_MAX] = {
    { 2300, 1},
    { 2800, 2},
    { 3700, 10},
    { 4700, 12},
    { 6500, 5},
    {11000, 1},
};
static cwgt_lut in_cwgt_lut[AWB_TUNING_CT_WEIGHT_MAX] = {
    { 2300, 1},
    { 2800, 1},
    { 3700, 1},
    { 4700, 1},
    { 6500, 1},
    {11000, 1},
};
static cwgt_lut night_cwgt_lut[AWB_TUNING_CT_WEIGHT_MAX] = {
    { 2300, 1},
    { 2800, 1},
    { 3700, 1},
    { 4700, 1},
    { 6500, 1},
    {11000, 1},
};

static awb_ct_weight awb_param_ct_weight = {
    ct_map,
    out_cwgt_lut,
    in_cwgt_lut,
    night_cwgt_lut,
};

/******************************/
/**
    AWB target
*/
static awb_target awb_param_target[AWB_TUNING_TARGET_MAX] = {
    //cx rgratio bgratio
    { 820, 1024, 990}, //L-CT
    { 966, 1024, 1044}, //M-CT
    { 1020, 1024, 1044}, //H-CT
};

/******************************/
/**
    AWB CT table
    The color temperature must be steeled sort
*/
static awb_ct_info awb_param_ct_table[AWB_TUNING_CT_MAX] = {
     {2300, { AWB_CT_2300K_RGAIN,  256, AWB_CT_2300K_BGAIN}},
     {2800, { AWB_CT_2800K_RGAIN,  256, AWB_CT_2800K_BGAIN}},
     {3700, { AWB_CT_3700K_RGAIN,  256, AWB_CT_3700K_BGAIN}},
     {4700, { AWB_CT_4700K_RGAIN,  256, AWB_CT_4700K_BGAIN}},
     {6500, { AWB_CT_6500K_RGAIN,  256, AWB_CT_6500K_BGAIN}},
     {11000, { AWB_CT_11000K_RGAIN,  256, AWB_CT_11000K_BGAIN}},
};

/******************************/
/**
    AWB Manual table
*/
static awb_mwb_gain awb_param_mwb_tab[AWB_TUNING_MWB_MAX] = {
    { 256, 256, 256},
    { AWB_CT_6500K_RGAIN, 256, AWB_CT_6500K_BGAIN * 0.85}, //AWB_MODE_CLOUDY
    { AWB_CT_6500K_RGAIN, 256, AWB_CT_6500K_BGAIN}, //AWB_MODE_DAYLIGHT
    { AWB_CT_2800K_RGAIN, 256, AWB_CT_2800K_BGAIN}, //AWB_MODE_SUNSET
    { AWB_CT_2300K_RGAIN, 256, AWB_CT_2300K_BGAIN}, //AWB_MODE_TUNGSTEN
    { AWB_CT_4700K_RGAIN, 256, AWB_CT_4700K_BGAIN}, //AWB_MODE_FLUORESCENT1
    { 256, 256, 256},
    { 256, 256, 256},
    { 256, 256, 256},
    { 256, 256, 256},
    { 256, 256, 256},
    { 256, 256, 256}
};

static awb_converge awb_converge_par = {
     4, //skip frame
     12, //speed
     259, //tolerance 259/256=1%
};

/*
120,   //RpBL
192, //RpBU
*/
/*
4,	 //YL
255, //YU
120,   //RpBL
192, //RpBU
-37,//RsBL
101, //RsBU
51,   //R2GL
138, //R2GU
30,   //B2GL
80, //B2GU
21,   //RmBL
72, //RmBU

*/
/*
typedef struct _awb_expand_block {
	UINT32 mode; //0:disable, 1:add, 2:remove
	UINT32 lv_l;
	UINT32 lv_h;
	INT32 y_l;
	INT32 y_u;
	INT32 rpb_l;
	INT32 rpb_u;
	INT32 rsb_l;
	INT32 rsb_u;
} awb_expand_block;

*/

static awb_expand_block awb_expand_block_par[AWB_TUNING_BLOCK_MAX] = {
    {1, 1, 900, 5, 245, 106, 141, -4, 52},
    {0, 600, 1000, 5, 245, 105, 122, 56, 74},
    {0, 600, 1000, 5, 245, 91, 105, 18, 32},
    {0, 600, 1000, 5, 245, 114, 128, -67, -53},
};

awb_param awb_param_gc2053_evb = {
    &awb_param_ca_info,
    &awb_param_th,
    &awb_param_lv_check[0],
    &awb_param_ct_weight,
    &awb_param_target[0],
    &awb_param_ct_table[0],
    &awb_param_mwb_tab[0],
    &awb_converge_par,
    &awb_expand_block_par[0],
};
    
void awb_get_param_gc2053_evb(UINT32* param);
    
void awb_get_param_gc2053_evb(UINT32* param)
{
	*param =  (UINT32)(&awb_param_gc2053_evb);
		//DBG_ERR("%x \r\n", *param);
}
