/**
    Auto Exposure parameter.

    ae parameter.

    @file       ae_gc2053_param.c
    @ingroup    mISYSAlg
    @note       Nothing (or anything need to be mentioned).

    Copyright   Novatek Microelectronics Corp. 2011.  All rights reserved.
*/
#include "Type.h"
#include "ae_alg.h"
#include "sensor.h"
#include "ae_api.h"
#include "ae_proc_nvt_int.h"
#include "pwm.h"

#define AE_ISO_BASE             100
#define AE_PRV_ISO_MIN          AE_ISO_BASE
#define AE_PRV_ISO_MAX          12800
#define AE_CAP_ISO_MIN          AE_ISO_BASE
#define AE_CAP_ISO_MAX          6400

static AE_EXPECT_LUM expect_lum = {
    452, //mov
    480, //photo
  //{27, 27, 30, 36, 43, 52, 66, 75, 80, 90, 100, 100, 100, 108, 108, 108, 108, 108, 108, 108, 108 },
//    {27, 27, 30, 36, 40, 50, 64, 75, 83, 93, 100, 100, 100, 108, 108, 108, 108, 108, 108, 108, 108 }, //
 //   {23, 24, 27, 33, 37, 46, 63, 75, 82, 90, 97, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 },//0529
    //{27, 30, 32, 38, 42, 50, 66, 75, 82, 90, 97, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 },//0610
    //{30,  33,  36,  41,  46,  56,  70,  78,  85,  92,  97,  100,  100,  100,  100,  100,  100,  100,  100,  100,  100},//0613
  //  {26,  28,  33,  38,  46,  56,  72,  79,  85,  93,  96,   98,   99,  100,  101,  101,  101,  101,  101,  101,  100,},//0620
//    {26,  28,  33,  38,  45,  55,  69,  78,  85,  93,  96,   98,   99,  100,  101,  101,  101,  101,  101,  101,  100,},//0919
  //  {26,  28,  33,  38,  45,  55,  69,  78,  85,  93,  95,   96,   97,  98,  101,  101,  101,  101,  101,  101,  100,},//0924
  //  {26,  28,  33,  38,  45,  55,  69,  78,  85,  93,  95,   96,   97,  98,  99,  100,  101,  101,  101,  101,  100,},//0925
    {26,  28,  33,  37,  45,  55,  69,  78,  85,  93,  95,   96,   96,  97,  97,  97,  102,  102,  102,  101,  100,},//0925
    {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 },
    {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 },
};

static AE_LA_CLAMP la_clamp = {
    // For normal mode, LA clamp value
    {1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    // For SHDR mode, ratio value, base on Target Y to calculate clamp boundary.
    { 125,  125,  125,  125,  125,  125,  125,  125,  125,  125,  125,  125,  125,  125,  125,  125,  125,  125,  125,  125,  125},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
};

static AE_HISTO histogram = {
    AE_FUNC_AUTO,
    486,
    {100,  100,  100,  100,  100,  100,  100,  100,   100,   100,    100,  100,   100,  100,  100,  100,  100,  100,  100,  100,  100},
};

static AE_OVER_EXPOSURE over_exposure = {
    ENABLE,
    869,
    {105, 105, 105, 105, 105, 105, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 },
    //{40, 40, 40, 40, 50, 60, 70, 85, 110, 130, 130, 130, 130, 130, 130, 130, 130, 130, 200, 200, 200 },
    //{20, 20, 20, 20, 20, 30, 40, 80, 110, 130, 130, 130, 130, 130, 130, 130, 130, 130, 200, 200, 200 },
	//{20,  20,  20,  20,  20,  30,  40,  70,  90,  110,  132,  145,  145,  145,  150,  160,  180,  190,  200,  200,  200},//0620
	//{20,  20,  20,  20,  20,  30,  40,  70,  90,  115,  140,  160,  160,  160,  160,  160,  180,  190,  200,  200,  200},//0924
	//{20,  20,  20,  20,  20,  30,  40,  70,  90,  115,  140,  160,  175,  175,  175,  175,  180,  190,  200,  200,  200},//0925
	//{20,  20,  20,  20,  20,  30,  40,  70,  90,  115,  140,  160,  175,  180,  180,  185,  190,  200,  200,  200,  200},//1125
	//{20,  20,  20,  20,  20,  30,  40,  70,  90,  115,  140,  160,  175,  175,  175,  180,  190,  200,  200,  200,  200},//1125
	{20,  20,  20,  20,  20,  30,  40,  70,  90,  115,  140,  160,  175,  175,  175,  175,  180,  190,  200,  200,  200},//0925
	{10, 10, 20, 30, 40, 50, 64, 80, 96, 114, 137, 147, 152, 156, 156, 156, 156, 156, 156, 156, 156 },
    {10, 10, 20, 30, 40, 50, 64, 80, 96, 114, 137, 147, 152, 156, 156, 156, 156, 156, 156, 156, 156 },
    {50, 50,  50,  50,  50,  50,  50,  50,  50,   50,   40,   40,   40,   40,   40,   40,   40,   40,   40,   40,   40},
    {17, 17,  17,  17,  17,  17,  17,  17,  17,   17,   13,   13,   13,   13,   13,   13,   13,   13,   13,   13,   13},

};

static AE_CONVERGENCE convergence  = {
    AEALG_SPEED_P1,
    {5, 5},
    1000,    // smooth ratio thr
    ENABLE,  // De-Pulse enable
    150,     // De-Pulse Ratio
    2,       // De-Pulse Frame Number
    0,
    0,      // Slow shoot cyc (Ex. 10 = 1s), auto reset counter when unstable ratio not over thr)
    0,       // Slow shoot thr (Sum of AE unstable ratio during slow shoot cyc)
    0, // reserved
    0 // reserved
};

static AE_IRISINFO iris_info = {
    AE_IRIS_TYPE_FIXED, AE_IRIS_MODE_OPEN, 10, AEALG_IRIS_BIG, {AEALG_IRIS_BIG, AEALG_IRIS_SMALL},
};

static AE_PROC_BOUNDARY proc_boundary = {
    {AE_PRV_ISO_MAX, AE_PRV_ISO_MIN},
    {AE_CAP_ISO_MAX, AE_CAP_ISO_MIN},
};


static AE_CURVE_GEN_PARAM curve_gen_param = {
    FNO_SEL_USER,
    40,
    FNO_F18,
    AE_FLICKER_50HZ,
    {
        {30, 100, 0, 0},
        {10000, 800, 0, 0},
        {20000, 900, 0, 0},
        {30000, 800, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
    },
    4, // node_num
    AE_PRV_ISO_MAX, // IsoMax
    {64, 512},
    DISABLE,
	{{300, 300}, {240, 240}, {200, 200}, {150, 150}, {120, 120}},
    AE_ISOMODE_MOV_PARAM,
    AE_PRV_ISO_MAX,
    AE_FLICKER_ON
};

static UINT32 ae_meter_window[AE_WIN_NUM] = {
/*	1, 1, 1, 1, 1, 1, 1, 1,
	1, 2, 2, 2, 2, 2, 2, 1,
	1, 2, 3, 3, 3, 3, 2, 1,
	1, 2, 3, 4, 4, 3, 2, 1,
	1, 2, 3, 4, 4, 3, 2, 1,
	1, 2, 3, 3, 3, 3, 2, 1,
	1, 2, 2, 2, 2, 2, 2, 1,
	1, 1, 1, 1, 1, 1, 1, 1 */

    2,2,3,3,3,3,2,2,
    2,2,4,5,5,4,2,2,
    1,2,3,4,4,3,2,1,
    1,1,2,3,3,2,1,1,
    1,1,2,2,2,2,1,1,
    1,1,1,0,0,1,1,1,
    1,1,0,0,0,0,1,1,
    0,0,0,0,0,0,0,0
};

static AE_LUM_GAMMA lum_gamma = {
    AE_LUMGAMMA_PREDEF,
    {
	0,29,57,81,106,132,156,180,204,226,
	250,272,293,311,330,349,366,381,398,413,
	428,445,459,473,486,502,518,533,548,564,
	578,592,604,617,628,639,649,658,667,676,
	683,692,699,707,714,721,727,733,739,745,
	750,756,761,767,772,777,782,788,792,797,
	802,807,811,816,820,825,828,833,837,842,
	845,850,853,857,861,865,868,872,875,879,
	882,886,888,892,895,898,901,904,907,910,
	913,916,918,921,924,927,929,932,935,938,
	940,943,946,949,951,955,957,960,963,966,
	968,972,974,978,980,983,986,989,992,995,
    997,1001,1003,1006,1010,1013,1016,1020,1023
    }
};

static AE_SHDR shdr = {
	AE_SHDR_EV_OPT_FIXED,
	486,
	{100,  100,  100,  100,  100,  100,  100,  100,   100,   100,    100,  100,   100,  100,  100,  100,  100,  100,  100,  100,  100},
	{
		1024,
		80,
		{5, 5},
		{4095, 960},
		{1024, 256},
		{100,  100,  100,  100,  100,  100,  100,  100,   100,   100,    100,  100,   100,  100,  100,  100,  100,  100,  100,  100,  100},
		100,
		0,
	},
	{
		256,
		400,
	},
	{  256,  256,  256,  256,  256,  256,  256,  256,  256,  256,  256,  256,  256,  256,  256,  256,  256,  256,  256,  256,  256},
	{ 8192, 8192, 4096, 2048, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024},
	{    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
	{   20,   40,   60,   60,   60,   60,   70,   85,  100,  100,  100,  100,  100,  100,  100,  100,  100,  100,  100,  100,  100},
	0,
	0
};

static AE_IRIS_CFG dciris_cfg = {
    DISABLE,
    0, // exposure min, set 0 for auto select flicker base
    0,
    110,
    0,
    { 3, 1, 1 },
    0,   // PWMID
    200, // drv_max
    500, // exposure freeze_time (ms)
    1000 // exposure unfreeze_time (ms)
};

static AE_CURVE_GEN_PHOTO curve_gen_photo = {
    {
    {250, 100, 0, 0},
    {33333, 800, 0, 0},
    {66666, 1600, 0, 0},
    {100000, 3200, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    },
    4, // node_num
    66666, // max cap expotime
    AE_CAP_ISO_MAX, // max cap iso
    AE_ISOMODE_PHOTO_SPEED,
    AE_CAP_ISO_MAX,
};

AE_PARAM_INFO ae_param_gc2053_evb = {
    AE_PRV_ISO_MIN,
    1000,
    &expect_lum,
    &la_clamp,
    &histogram,
    &over_exposure,
    &convergence,
    &iris_info,
    &proc_boundary,
    &curve_gen_param,
    &ae_meter_window[0],
    &lum_gamma,
    &shdr,
    &dciris_cfg,
    &curve_gen_photo
};

void ae_get_param_gc2053_evb(UINT32 *param);

void ae_get_param_gc2053_evb(UINT32 *param)
{
CHKPNT;
	*param = (UINT32) &ae_param_gc2053_evb;
}
