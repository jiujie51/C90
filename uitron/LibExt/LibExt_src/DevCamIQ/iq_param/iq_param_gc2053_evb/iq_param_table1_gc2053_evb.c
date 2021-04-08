#ifndef _IQ_PARAM_TABLE1_GC2053_EVB_C
#define _IQ_PARAM_TABLE1_GC2053_EVB_C

#include "iq_param_common_gc2053_evb.h"

////////////////////////////////
static ENGINE_CTRL e_ctrl =
{
    {ECS_ON,       NON_SUPPORT},    // 1
    {VIG_ON, 100},        //0~200,default:100
    {GDC_OFF,       200},            //0~200,default:200(100, same with OFF)
    {CAC_OFF,       NON_SUPPORT},
    {CCM_ON,        NON_SUPPORT},    // 5
    {CC3D_OFF,      NON_SUPPORT},
    {CCTRL_ON,      100, 100, 100},  //sat:0~200;con:0~200;bri:0~200,fedault:100
    {HSV_ON,        0},              //H:0~360,default:0
    {GAMMA_ON,      NON_SUPPORT},
    {Y_CURVE_ON, NON_SUPPORT},
    {EDGE_ON,       100},            //0~200,default:100
    {OUTL_ON,       NON_SUPPORT},
    {RANGE_NR_ON,   100},            //0~200,default:100
    {GBAL_ON,       NON_SUPPORT},
    {LPF_ON,        NON_SUPPORT},    // 15
    {LCA_ON,        NON_SUPPORT},
    {LCA_C_ON,      NON_SUPPORT},
    {LCA_Y_ON,      NON_SUPPORT},
    {TMNR_ON,       NON_SUPPORT},
    {TNR_ON,        5},              // 20   //0~10,default:5
    {LCE_ON, 		NON_SUPPORT},
    {DPC_OFF,	    NON_SUPPORT},
    {RESERVED_OFF,	NON_SUPPORT},
    {RESERVED_OFF,  NON_SUPPORT},
    {RESERVED_OFF,  NON_SUPPORT},
    {RESERVED_OFF,  NON_SUPPORT},
    {RESERVED_OFF,  NON_SUPPORT},
    {RESERVED_OFF,  NON_SUPPORT},
    {RESERVED_OFF,  NON_SUPPORT},
    {RESERVED_OFF,  NON_SUPPORT},
    {RESERVED_OFF,  NON_SUPPORT},
};

////////////////////////////////
//             SIE             /
////////////////////////////////

//////////////////////
//**  SIE-OB
//////////////////////
static UINT32 ob[TOTAL_GAIN_NUM][5] =
{
    {256, 256, 256, 256, 256},
    {256, 256, 256, 256, 256},
    {256, 256, 256, 256, 256},
    {256, 256, 256, 256, 256},
    {256, 256, 256, 256, 256},
    {256, 256, 256, 256, 256},
    {256, 256, 256, 256, 256},
    {256, 256, 256, 256, 256},
    {256, 256, 256, 256, 256},
    {256, 256, 256, 256, 256},
    {256, 256, 256, 256, 256},
    {256, 256, 256, 256, 256},
};

////////////////////////////////
//             RHE             /
////////////////////////////////

//////////////////////
//**  RHE-WDR
//////////////////////
static UINT8 wdr_strength[21] = //LV0~20, range: 0 ~ 128, 1x = 64;
{
    0, 0, 0, 5, 7, 9, 10,
    15, 15, 18, 18, 18, 18, 22,
    28, 32, 36, 40, 42, 42, 42
};

static IQS_WDR_LCE_OFFSET wdr_lce_offset =
{
    5,  //for WDR_AUTO Mode
    10, //for WDR_MANUAL Mode
};

static SR_WDR_GCE_PARAM gce_strength[TOTAL_GAIN_NUM] = //sth_range: 0 ~ 128, 0 = 1X
{
    //dark, bright
    {3, 0},
    {2, 0},
    {2, 0},
    {2, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
};

//////////////////////
//**  RHE-LCE
//////////////////////
static IQS_LCE lce_strength[TOTAL_GAIN_NUM] =
{
    //level(range: 0 ~ 128, 0 = OFF), clamp (range: 64 ~ 255, 64 = OFF)
    {3,73},
    {3,73},
    {3,73},
    {2,73},
    {0, 73},
    {0, 73},
    {0, 73},
    {0, 73},
    {0, 73},
    {0, 73},
    {0, 73},
    {0, 73},
};

////////////////////////////////
//             IFE             /
////////////////////////////////

//////////////////////
//**  IFE-Outlier
//////////////////////
static UINT32 outl_bri[TOTAL_GAIN_NUM][5] =
{
    {1188, 1723, 2843, 4095, 4095},
    {1188, 1723, 2843, 4095, 4095},
    {1106, 1604, 2647, 3838, 4095},
    {532, 771, 1272, 1844, 3043},
    {532, 771, 1272, 1844, 3043},
    {532, 771, 1272, 1844, 3043},
    {532, 771, 1272, 1844, 3043},
    {532, 894, 1528, 2297, 2817},
    {710, 1036, 1711, 2365, 3905},
    {532, 771, 1272, 1844, 3043},
    {410, 595, 982, 1424, 2350},
    {410, 595, 982, 1424, 2350},
};

static UINT32 outl_dark[TOTAL_GAIN_NUM][5] =
{
    {1100, 1690, 2925, 3885, 3885},
    {1188, 1723, 2843, 4095, 4095},
    {1106, 1604, 2647, 3838, 4095},
    {1024, 1485, 2450, 3553, 4095},
    {942, 1366, 2254, 3268, 4095}, //ISO1600
    {860, 1247, 2058, 2984, 4095},
    {778, 1128, 1861, 2698, 4095},
    {778, 1128, 1861, 3070, 4095},
    {710, 1036, 1711, 2365, 3905},
    {532, 771, 1272, 1844, 3043},
    {410, 595, 982, 1424, 2350},
    {410, 595, 982, 1424, 2350},
};

static UINT32 outl_cnt[TOTAL_GAIN_NUM][2] =
{
    {1, 0},
    {1, 0},
    {1, 0},
    {1, 0},
    {1, 0},
    {1, 0},
    {1, 0},
    {1, 0},
	{1, 0},
    {1, 0},
    {1, 0},
    {1, 0},
};

static IFE_OUTL outl_set[TOTAL_GAIN_NUM] =
{
    //BrightThres,      DarkThres,          OutlCnt,   Weight, DarkOfs, BrightOfs
	{(UINT32*)outl_bri, (UINT32*)outl_dark, (UINT32*)outl_cnt, 0, 0, 0},
    {(UINT32*)outl_bri, (UINT32*)outl_dark, (UINT32*)outl_cnt, 0, 0, 0},
    {(UINT32*)outl_bri, (UINT32*)outl_dark, (UINT32*)outl_cnt, 0, 0, 0},
    {(UINT32*)outl_bri, (UINT32*)outl_dark, (UINT32*)outl_cnt, 0, 0, 0},
    {(UINT32*)outl_bri, (UINT32*)outl_dark, (UINT32*)outl_cnt, 0, 0, 0},
    {(UINT32*)outl_bri, (UINT32*)outl_dark, (UINT32*)outl_cnt, 0, 0, 0},
    {(UINT32*)outl_bri, (UINT32*)outl_dark, (UINT32*)outl_cnt, 0, 0, 0},
    {(UINT32*)outl_bri, (UINT32*)outl_dark, (UINT32*)outl_cnt, 0, 0, 0},
    {(UINT32*)outl_bri, (UINT32*)outl_dark, (UINT32*)outl_cnt, 0, 0, 0},
    {(UINT32*)outl_bri, (UINT32*)outl_dark, (UINT32*)outl_cnt, 0, 0, 0},
    {(UINT32*)outl_bri, (UINT32*)outl_dark, (UINT32*)outl_cnt, 0, 0, 0},
    {(UINT32*)outl_bri, (UINT32*)outl_dark, (UINT32*)outl_cnt, 0, 0, 0},
};

//////////////////////
//**  IFE-Spatial
//////////////////////
static UINT32 spatial_w[TOTAL_GAIN_NUM][6] =
{
	{31, 23, 17, 9, 7, 3},
	{31, 23, 17, 9, 7, 3},
	{31, 23, 17, 9, 7, 3},//ISO400
    {31, 23, 17, 9, 7, 3},
    {31, 23, 17, 9, 7, 3},
    {31, 23, 17, 9, 7, 3},//ISO3200
    {31, 23, 17, 9, 7, 3},
    {31, 23, 17, 9, 7, 3}, //ISO12800
    {31, 23, 17, 9, 7, 3},
    {31, 23, 17, 9, 7, 3},//ISO3200
    {31, 23, 17, 9, 7, 3},
    {31, 23, 17, 9, 7, 3},
};

static Spatial_Filter spatial[TOTAL_GAIN_NUM] =
{
// sOnlyEn, sOnlyFiltLeng, pWeight
    {DISABLE, Filt_3x3, (UINT32*)spatial_w},
    {DISABLE, Filt_3x3, (UINT32*)spatial_w},
    {DISABLE, Filt_3x3, (UINT32*)spatial_w},
    {DISABLE, Filt_3x3, (UINT32*)spatial_w},
    {DISABLE, Filt_3x3, (UINT32*)spatial_w},
    {DISABLE, Filt_3x3, (UINT32*)spatial_w},
    {DISABLE, Filt_3x3, (UINT32*)spatial_w},
    {DISABLE, Filt_3x3, (UINT32*)spatial_w},
	{DISABLE, Filt_3x3, (UINT32*)spatial_w},
    {DISABLE, Filt_3x3, (UINT32*)spatial_w},
    {DISABLE, Filt_3x3, (UINT32*)spatial_w},
    {DISABLE, Filt_3x3, (UINT32*)spatial_w},
};

//////////////////////
//**  IFE-Range
//////////////////////
static UINT32 range_a_thr[TOTAL_GAIN_NUM][6] =
{
    {0, 1, 2, 2, 3, 4},
    {0, 2, 3, 3, 4, 4},
    {0, 2, 4, 4, 5, 6},
    {0, 3, 4, 5, 6, 6},
    {0, 4, 5, 6, 6, 7},
    {0, 5, 6, 7, 8, 9},
    {0, 5, 6, 7, 8, 9},//6400
    {0, 24, 35, 43, 49, 70},
    {0, 31, 44, 55, 63, 89},
    {0, 38, 54, 66, 76, 108},
    {0, 43, 87, 130, 172, 216},
    {0, 102, 207, 309, 411, 516},
};

static UINT32 range_a_lut[TOTAL_GAIN_NUM][17] =
{
    {0, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33},
    {0, 4, 5, 7, 8, 9, 11, 13, 14, 15, 16, 17, 18, 18, 19, 20, 20},
    {0, 5, 9, 11, 12, 14, 16, 18, 20, 22, 25, 26, 27, 28, 29, 30, 32},
    {0, 8, 12, 15, 18, 21, 24, 28, 32, 35, 38, 40, 42, 46, 48, 52, 56},
    {0, 12, 16, 20, 24, 28, 31, 34, 37, 41, 45, 49, 51, 53, 55, 57, 59},
    {0, 20, 30, 38, 44, 52, 56, 60, 64, 68, 72, 76, 78, 81, 84, 88, 92},
    {4, 30, 40, 48, 54, 62, 66, 70, 74, 78, 82, 86, 88, 90, 94, 99, 102},
    {1, 10, 23, 26, 31, 34, 36, 37, 39, 41, 44, 47, 51, 55, 59, 60, 61},
    {0, 17, 24, 30, 35, 39, 43, 46, 49, 52, 55, 58, 60, 63, 65, 68, 70},
    {0, 30, 42, 51, 60, 67, 73, 79, 84, 90, 94, 99, 103, 108, 112, 116, 120},
    {0, 17, 40, 57, 77, 86, 93, 102, 111, 113, 118, 122, 130, 136, 144, 156, 165},
    {0, 39, 96, 135, 183, 204, 222, 243, 264, 270, 279, 291, 309, 324, 342, 372, 393},
};

static UINT32 range_b_thr[TOTAL_GAIN_NUM][6] =
{
    {0, 4, 8, 12, 16, 22},
    {0, 4, 10, 14, 20, 24},
    {0, 6, 12, 16, 22, 28},
    {0, 8, 11, 14, 16, 23},
    {2, 12, 16, 20, 23, 31},
    {0, 35, 49, 61, 70, 99},
    {0, 18, 25, 31, 36, 51},
    {0, 46, 66, 81, 93, 132},
    {0, 63, 89, 110, 126, 179},
    {0, 76, 108, 132, 152, 216},
    {0, 64, 131, 195, 259, 325},
    {0, 153, 312, 465, 618, 774},
};

static UINT32 range_b_lut[TOTAL_GAIN_NUM][17] =
{
    {0, 3, 8, 9, 11, 12, 14, 16, 17, 19, 21, 22, 24, 25, 27, 29, 30},
    {0, 3, 9, 11, 13, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 35},
    {0, 4, 10, 12, 15, 17, 19, 22, 24, 26, 29, 31, 33, 36, 38, 40, 43},
    {0, 6, 9, 11, 12, 14, 15, 16, 18, 19, 20, 21, 22, 23, 23, 24, 25},
    {2, 12, 16, 20, 22, 25, 27, 29, 31, 33, 34, 36, 38, 39, 40, 42, 43},
    {0, 12, 16, 20, 24, 26, 29, 31, 33, 36, 37, 39, 41, 43, 44, 46, 48},
    {0, 37, 52, 64, 74, 83, 91, 98, 105, 111, 117, 123, 128, 134, 139, 144, 148},
    {0, 18, 44, 50, 60, 66, 70, 72, 76, 80, 86, 92, 100, 108, 116, 118, 120},
    {0, 35, 49, 60, 70, 78, 86, 93, 99, 105, 111, 116, 121, 126, 131, 136, 140},
    {0, 60, 84, 103, 120, 134, 146, 158, 169, 180, 189, 198, 207, 216, 224, 232, 240},
    {0, 24, 61, 86, 115, 128, 141, 153, 166, 169, 176, 183, 195, 204, 215, 234, 248},
    {0, 57, 144, 204, 276, 306, 336, 366, 396, 402, 420, 435, 465, 486, 513, 558, 591},
};

static RangeCFGB range_b[TOTAL_GAIN_NUM] =
{
    //pThres,                  pThLut,            BilatThEn, BilatTh1, BilatTh2
    {(UINT32*)range_b_thr, (UINT32*)range_b_lut, {TRUE, 15, 17}},
    {(UINT32*)range_b_thr, (UINT32*)range_b_lut, {TRUE, 22, 24}},
    {(UINT32*)range_b_thr, (UINT32*)range_b_lut, {TRUE, 35, 40}},
    {(UINT32*)range_b_thr, (UINT32*)range_b_lut, {TRUE, 77, 92}},
    {(UINT32*)range_b_thr, (UINT32*)range_b_lut, {TRUE, 64, 128}},
    {(UINT32*)range_b_thr, (UINT32*)range_b_lut, {TRUE, 129, 154}},
    {(UINT32*)range_b_thr, (UINT32*)range_b_lut, {TRUE, 100, 143}},
    {(UINT32*)range_b_thr, (UINT32*)range_b_lut, {TRUE, 243, 281}},
    {(UINT32*)range_b_thr, (UINT32*)range_b_lut, {TRUE, 335, 402}},
    {(UINT32*)range_b_thr, (UINT32*)range_b_lut, {TRUE, 503, 603}},
    {(UINT32*)range_b_thr, (UINT32*)range_b_lut, {TRUE, 1005, 906}},
    {(UINT32*)range_b_thr, (UINT32*)range_b_lut, {TRUE, 1005, 906}},
};

//////////////////////
//**  IFE-CLAMP
//////////////////////
static RANGE_FILT_CLAMP_PARAM filter_clamp[TOTAL_GAIN_NUM] =
{
    //clamp{clamp_Th,Mul,Dlt}, bilat_w, Rth_w, Bin, BitDither}
    {{4, 110, 0}, 0, 0, 0, 0},
    {{6, 110, 0}, 0, 0, 0, 0},
    {{8, 100, 0}, 0, 0, 0, 0},
    {{8, 100, 0}, 0, 0, 0, 0},
    {{6, 84, 0}, 0, 0, 0, 0},
    {{0, 80, 0}, 0, 0, 0, 0},
    {{0, 80, 0}, 0, 0, 0, 0},
    {{0, 35, 0}, 0, 0, 0, 0},
    {{0, 80, 0}, 0, 0, 0, 0},
    {{0, 64, 0}, 0, 0, 0, 0},
    {{0, 64, 0}, 0, 0, 0, 0},
    {{0, 64, 0}, 0, 0, 0, 0},
};

//////////////////////
//**  IFE-Vig
//////////////////////
static IQS_VIG_CENTER_RATIO vig_center =
{
    494, 504
};

static IQS_VIG_SET vig =
{
    VIG_TAB_GAIN_2X, {0, 1, 1, 2, 4, 7, 11, 17, 26, 36, 48, 64, 83, 114, 169, 263, 463}
};

static IQS_VIG_ISO_TH vig_iso_th =
{
    400, 1600
};

//////////////////////
//**  IFE-Gbal
//////////////////////
static UINT8 gbal_s_tab[TOTAL_GAIN_NUM][17] =
{
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

static UINT8 gbal_d_tab[TOTAL_GAIN_NUM][17] =
{
    {8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

static IFE_GBAL gbal[TOTAL_GAIN_NUM] =
{
    //Thres,pGbalStab,            pGbalDtab,    uiGbalSmtThr
    {3, (UINT8*)gbal_s_tab, (UINT8*)gbal_d_tab, 0},
    {3, (UINT8*)gbal_s_tab, (UINT8*)gbal_d_tab, 0},
    {3, (UINT8*)gbal_s_tab, (UINT8*)gbal_d_tab, 0},
    {4, (UINT8*)gbal_s_tab, (UINT8*)gbal_d_tab, 0},
    {8, (UINT8*)gbal_s_tab, (UINT8*)gbal_d_tab, 0},
    {8, (UINT8*)gbal_s_tab, (UINT8*)gbal_d_tab, 0},
    {5, (UINT8*)gbal_s_tab, (UINT8*)gbal_d_tab, 0},
    {5, (UINT8*)gbal_s_tab, (UINT8*)gbal_d_tab, 0},
    {5, (UINT8*)gbal_s_tab, (UINT8*)gbal_d_tab, 0},
    {8, (UINT8*)gbal_s_tab, (UINT8*)gbal_d_tab, 0},
    {8, (UINT8*)gbal_s_tab, (UINT8*)gbal_d_tab, 0},
    {8, (UINT8*)gbal_s_tab, (UINT8*)gbal_d_tab, 0},
};

//////////////////////
//**  IFE-RBFill
//////////////////////
static UINT8 rbfill_luma_tab[TOTAL_GAIN_NUM][17] =
{
    {0, 16, 24, 27, 27, 28, 30, 31, 31, 30, 29, 28, 28, 27, 26, 12, 4},
    {0, 16, 24, 27, 27, 28, 30, 31, 31, 30, 29, 28, 28, 27, 26, 12, 4},
    {0, 16, 24, 27, 27, 28, 30, 31, 31, 30, 29, 28, 28, 27, 26, 12, 4},
    {0, 16, 24, 27, 27, 28, 30, 31, 31, 30, 29, 28, 28, 27, 26, 12, 4},
    {0, 16, 24, 27, 27, 28, 30, 31, 31, 30, 29, 28, 28, 27, 26, 12, 4},
    {0, 16, 24, 27, 27, 28, 30, 31, 31, 30, 29, 28, 28, 27, 26, 12, 4},
    {0, 16, 24, 27, 27, 28, 30, 31, 31, 30, 29, 28, 28, 27, 26, 12, 4},
    {0, 16, 24, 27, 27, 28, 30, 31, 31, 30, 29, 28, 28, 27, 26, 12, 4},
    {0, 16, 24, 27, 27, 28, 30, 31, 31, 30, 29, 28, 28, 27, 26, 12, 4},
    {0, 16, 24, 27, 27, 28, 30, 31, 31, 30, 29, 28, 28, 27, 26, 12, 4},
    {0, 16, 24, 27, 27, 28, 30, 31, 31, 30, 29, 28, 28, 27, 26, 12, 4},
    {0, 16, 24, 27, 27, 28, 30, 31, 31, 30, 29, 28, 28, 27, 26, 12, 4}
};

static UINT8 rbfill_ratio_tab[TOTAL_GAIN_NUM][32] =
{
    {31, 31, 31, 31, 31, 31, 30, 30, 29, 28, 24, 20, 16, 12, 10, 8, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 2, 1, 0},
    {31, 31, 31, 31, 31, 31, 30, 30, 29, 28, 24, 20, 16, 12, 10, 8, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 2, 1, 0},
    {31, 31, 31, 31, 31, 31, 30, 30, 29, 28, 24, 20, 16, 12, 10, 8, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 2, 1, 0},
    {31, 31, 31, 31, 31, 31, 30, 30, 29, 28, 24, 20, 16, 12, 10, 8, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 2, 1, 0},
    {31, 31, 31, 31, 31, 31, 30, 30, 29, 28, 24, 20, 16, 12, 10, 8, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 2, 1, 0},
    {31, 31, 31, 31, 31, 31, 30, 30, 29, 28, 24, 20, 16, 12, 10, 8, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 2, 1, 0},
    {31, 31, 31, 31, 31, 31, 30, 30, 29, 28, 24, 20, 16, 12, 10, 8, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 2, 1, 0},
    {31, 31, 31, 31, 31, 31, 30, 30, 29, 28, 24, 20, 16, 12, 10, 8, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 2, 1, 0},
    {31, 31, 31, 31, 31, 31, 30, 30, 29, 28, 24, 20, 16, 12, 10, 8, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 2, 1, 0},
    {31, 31, 31, 31, 31, 31, 30, 30, 29, 28, 24, 20, 16, 12, 10, 8, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 2, 1, 0},
    {31, 31, 31, 31, 31, 31, 30, 30, 29, 28, 24, 20, 16, 12, 10, 8, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 2, 1, 0},
    {31, 31, 31, 31, 31, 31, 30, 30, 29, 28, 24, 20, 16, 12, 10, 8, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 2, 1, 0}
};

static IFE_RBFILL rbfill[TOTAL_GAIN_NUM] =
{
    {(UINT8*)rbfill_luma_tab, (UINT8*)rbfill_ratio_tab, 1},
    {(UINT8*)rbfill_luma_tab, (UINT8*)rbfill_ratio_tab, 1},
    {(UINT8*)rbfill_luma_tab, (UINT8*)rbfill_ratio_tab, 1},
    {(UINT8*)rbfill_luma_tab, (UINT8*)rbfill_ratio_tab, 1},
    {(UINT8*)rbfill_luma_tab, (UINT8*)rbfill_ratio_tab, 1},
    {(UINT8*)rbfill_luma_tab, (UINT8*)rbfill_ratio_tab, 1},
    {(UINT8*)rbfill_luma_tab, (UINT8*)rbfill_ratio_tab, 1},
    {(UINT8*)rbfill_luma_tab, (UINT8*)rbfill_ratio_tab, 1},
    {(UINT8*)rbfill_luma_tab, (UINT8*)rbfill_ratio_tab, 1},
    {(UINT8*)rbfill_luma_tab, (UINT8*)rbfill_ratio_tab, 1},
    {(UINT8*)rbfill_luma_tab, (UINT8*)rbfill_ratio_tab, 1},
    {(UINT8*)rbfill_luma_tab, (UINT8*)rbfill_ratio_tab, 1}
};

////////////////////////////////
//             IPE             /
////////////////////////////////

//////////////////////
//**  IPE-Edge EXT
//////////////////////
static INT16 edge_kernel[TOTAL_GAIN_NUM][12] =
{
    //eext0 ~ eext9, eextdiv
    {0x0110, 0x002c, 0x03b8, 0x03e1, 0x03da, 0x0004, 0x0014, 0x000f, 0x0006, 0x0001, 0x000a, 0x0005},
    {0x0110, 0x002c, 0x03b8, 0x03e1, 0x03da, 0x0004, 0x0014, 0x000f, 0x0006, 0x0001, 0x000a, 0x0005},
    {0x0110, 0x002c, 0x03b8, 0x03e1, 0x03da, 0x0004, 0x0014, 0x000f, 0x0006, 0x0001, 0x000a, 0x0005},
    {0x0110, 0x002c, 0x03b8, 0x03e1, 0x03da, 0x0004, 0x0014, 0x000f, 0x0006, 0x0001, 0x000a, 0x0005},
    {0x0110, 0x002c, 0x03b8, 0x03e1, 0x03da, 0x0004, 0x0014, 0x000f, 0x0006, 0x0001, 0x000a, 0x0005},
    {0x0110, 0x002c, 0x03b8, 0x03e1, 0x03da, 0x0004, 0x0014, 0x000f, 0x0006, 0x0001, 0x000a, 0x0005},
    {0x0110, 0x002c, 0x03b8, 0x03e1, 0x03da, 0x0004, 0x0014, 0x000f, 0x0006, 0x0001, 0x000a, 0x0005},
    {0x0110, 0x002c, 0x03b8, 0x03e1, 0x03da, 0x0004, 0x0014, 0x000f, 0x0006, 0x0001, 0x000a, 0x0005},
    {0x0110, 0x002c, 0x03b8, 0x03e1, 0x03da, 0x0004, 0x0014, 0x000f, 0x0006, 0x0001, 0x000a, 0x0005},
    {0x0110, 0x002c, 0x03b8, 0x03e1, 0x03da, 0x0004, 0x0014, 0x000f, 0x0006, 0x0001, 0x000a, 0x0005},
    {0x0110, 0x002c, 0x03b8, 0x03e1, 0x03da, 0x0004, 0x0014, 0x000f, 0x0006, 0x0001, 0x000a, 0x0005},
    {0x0110, 0x002c, 0x03b8, 0x03e1, 0x03da, 0x0004, 0x0014, 0x000f, 0x0006, 0x0001, 0x000a, 0x0005},
};

static UINT8 edir_tab[TOTAL_GAIN_NUM][8]=
{
    {8, 8, 8, 8, 8, 8, 8, 8},
    {8, 8, 8, 8, 8, 8, 8, 8},
    {8, 8, 8, 8, 8, 8, 8, 8},
    {8, 8, 8, 8, 8, 8, 8, 8},
    {8, 8, 8, 8, 8, 8, 8, 8},
    {8, 8, 8, 8, 8, 8, 8, 8},
    {8, 8, 8, 8, 8, 8, 8, 8},
    {8, 8, 8, 8, 8, 8, 8, 8},
    {8, 8, 8, 8, 8, 8, 8, 8},
    {8, 8, 8, 8, 8, 8, 8, 8},
    {8, 8, 8, 8, 8, 8, 8, 8},
    {8, 8, 8, 8, 8, 8, 8, 8},
};

static IPE_DEE edir_ext[TOTAL_GAIN_NUM] =
{
    //YDRange,uiYDMaxTh,PndAdjEn,uiPndShft,uiES,EdirSel
    {IPE_YDR_7x7, 22, DISABLE, 3, 2, KER_SEL_A},
    {IPE_YDR_7x7, 22, DISABLE, 3, 2, KER_SEL_A},
    {IPE_YDR_7x7, 22, DISABLE, 3, 2, KER_SEL_A},
    {IPE_YDR_7x7, 22, DISABLE, 3, 2, KER_SEL_A},
    {IPE_YDR_7x7, 22, DISABLE, 3, 2, KER_SEL_A},
    {IPE_YDR_7x7, 22, DISABLE, 3, 2, KER_SEL_A},
    {IPE_YDR_7x7, 22, DISABLE, 3, 2, KER_SEL_A},
    {IPE_YDR_7x7, 22, DISABLE, 3, 2, KER_SEL_A},
    {IPE_YDR_7x7, 22, DISABLE, 3, 2, KER_SEL_A},
    {IPE_YDR_7x7, 22, DISABLE, 3, 2, KER_SEL_A},
    {IPE_YDR_7x7, 22, DISABLE, 3, 2, KER_SEL_A},
    {IPE_YDR_7x7, 22, DISABLE, 3, 2, KER_SEL_A},
};

static IPE_DirScorCal score_tab[TOTAL_GAIN_NUM] =
{
    //iA0; iB0; iC0; uiD0; iA1; iB1; iC1; uiD1; iA2; iB2; iC2; uiD2;
    {   3,  15,  15,    0,   5,   1,   1,    0,   3,   0,  15,   0},
    {   3,  15,  15,    0,   5,   1,   1,    0,   3,   0,  15,   0},
    {   3,  15,  15,    0,   5,   1,   1,    0,   3,   0,  15,   0},
    {   3,  15,  15,    0,   5,   1,   1,    0,   3,   0,  15,   0},
    {   3,  15,  15,    0,   5,   1,   1,    0,   3,   0,  15,   0},
    {   3,  15,  15,    0,   5,   1,   1,    0,   3,   0,  15,   0},
    {   3,  15,  15,    0,   5,   1,   1,    0,   3,   0,  15,   0},
    {   3,  15,  15,    0,   5,   1,   1,    0,   3,   0,  15,   0},
    {   3,  15,  15,    0,   5,   1,   1,    0,   3,   0,  15,   0},
    {   3,  15,  15,    0,   5,   1,   1,    0,   3,   0,  15,   0},
    {   3,  15,  15,    0,   5,   1,   1,    0,   3,   0,  15,   0},
    {   3,  15,  15,    0,   5,   1,   1,    0,   3,   0,  15,   0},
};

static IPE_DirScorCalTh score_th[TOTAL_GAIN_NUM] =
{
    //uiScoreTh0~3
    {7, 28, 3, 21},
    {5, 28, 3, 18},
    {5, 28, 2, 17},
    {5, 28, 3, 16},
    {9, 28, 4, 27},
    {5, 28, 3, 16},
    {6, 14, 5, 51},
    {63, 0, 63, 0},
    {63, 0, 63, 0},
    {63, 0, 63, 0},
    {63, 0, 63, 0},
    {63, 0, 63, 0},
};

static IPE_KAEdgeW ewa[TOTAL_GAIN_NUM] =
{
    //EWKer2DSel, uiAdwt0~5,           iAds0~5
    {KER_SEL_A, 9, 9, 13, 13, 16, 14, 3, 3, 3, 3, 3, 3},
    {KER_SEL_A, 8, 8, 12, 12, 16, 13, 3, 3, 3, 3, 3, 3},
    {KER_SEL_A, 8, 8, 12, 12, 16, 11, 3, 3, 3, 3, 3, 3},
    {KER_SEL_A, 8, 8, 12, 12, 16, 11, 3, 3, 3, 3, 3, 3},
    {KER_SEL_A, 8, 8, 12, 12, 16, 9, 3, 3, 3, 3, 3, 3},
    {KER_SEL_A, 8, 8, 12, 12, 16, 9, 3, 3, 3, 3, 3, 3},
    {KER_SEL_A, 8, 8, 12, 12, 10, 8, 3, 3, 3, 3, 3, 3},
    {KER_SEL_A, 8, 8, 12, 12, 16, 16, 3, 3, 3, 3, 3, 3},
    {KER_SEL_A, 8, 8, 12, 12, 16, 16, 3, 3, 3, 3, 3, 3},
    {KER_SEL_A, 8, 8, 12, 12, 16, 16, 3, 3, 3, 3, 3, 3},
    {KER_SEL_A, 8, 8, 12, 12, 16, 16, 3, 3, 3, 3, 3, 3},
    {KER_SEL_A, 8, 8, 12, 12, 16, 16, 3, 3, 3, 3, 3, 3},
};

static IPE_KBEdgeW ewb[TOTAL_GAIN_NUM] =
{
    //uiBdwt0~1, iBds0~1, EwKerBOutSel
    {1, 1, 2, 2, SEL_AVG_4D},
    {1, 1, 2, 2, SEL_AVG_4D},
    {1, 1, 2, 2, SEL_AVG_4D},
    {1, 1, 2, 2, SEL_AVG_4D},
    {1, 1, 2, 2, SEL_AVG_4D},
    {1, 1, 2, 2, SEL_AVG_4D},
    {1, 1, 2, 2, SEL_AVG_4D},
    {1, 1, 2, 2, SEL_AVG_4D},
    {1, 1, 2, 2, SEL_AVG_4D},
    {1, 1, 2, 2, SEL_AVG_4D},
    {1, 1, 2, 2, SEL_AVG_4D},
    {1, 1, 2, 2, SEL_AVG_4D},
};

static IPE_EdgeW ewparam[TOTAL_GAIN_NUM] =
{
    //uiEWDir0,1,2, DynEMEn
    {9, 8, 9, 0},
    {9, 8, 9, 0},
    {8, 7, 9, 0},
    {10, 8, 10, 0},
    {9, 9, 10, 0},
    {10, 9, 12, 0},
    {14, 14, 14, 0},
    {14, 14, 14, 0},
    {14, 14, 14, 0},
    {14, 14, 14, 0},
    {14, 14, 14, 0},
    {14, 14, 14, 0},
};

//////////////////////
//**  IPE-Edge Enh
//////////////////////
static IPE_EDGEENH edge_enh[TOTAL_GAIN_NUM] =
{
    //EnhP,EnhN, InvP,  InvN
    {110, 116, FALSE, FALSE},
    {108, 112, FALSE, FALSE},
    {88, 90, FALSE, FALSE},
    {68, 70, FALSE, FALSE},
    {60, 60, FALSE, FALSE},
    {46, 46, FALSE, FALSE},
    {12, 5, FALSE, FALSE},
    {64, 64, FALSE, FALSE},
    {64, 64, FALSE, FALSE},
    {60, 60, FALSE, FALSE},
    {60, 60, FALSE, FALSE},
    {60, 60, FALSE, FALSE},
};

//////////////////////
//**  IPE-Edge Map
//////////////////////
static UINT8 ed_tab[TOTAL_GAIN_NUM][16] =
{
    {0, 36, 72, 108, 144, 180, 216, 255, 255, 224, 192, 170, 154, 144, 136, 128},
    {0, 36, 72, 108, 144, 180, 216, 255, 255, 224, 192, 170, 154, 144, 136, 128},
    {0, 36, 72, 108, 144, 180, 216, 255, 255, 224, 192, 170, 154, 144, 136, 128},
    {0, 36, 72, 108, 144, 180, 216, 255, 255, 224, 192, 170, 154, 144, 136, 128},
    {0, 36, 72, 108, 144, 180, 216, 255, 255, 224, 192, 170, 154, 144, 136, 128},
    {0, 36, 72, 108, 144, 180, 216, 255, 255, 224, 192, 170, 154, 144, 136, 128},
    {0, 36, 72, 108, 144, 180, 216, 255, 255, 224, 192, 170, 154, 144, 136, 128},
    {0, 36, 72, 108, 144, 180, 216, 255, 255, 224, 192, 170, 154, 144, 136, 128},
    {0, 36, 72, 108, 144, 180, 216, 255, 255, 224, 192, 170, 154, 144, 136, 128},
    {0, 36, 72, 108, 144, 180, 216, 255, 255, 224, 192, 170, 154, 144, 136, 128},
    {0, 36, 72, 108, 144, 180, 216, 255, 255, 224, 192, 170, 154, 144, 136, 128},
    {0, 36, 72, 108, 144, 180, 216, 255, 255, 224, 192, 170, 154, 144, 136, 128},
};

static EdgeMap ed_map[TOTAL_GAIN_NUM] =
{
    {EMI_E57 , 2, 90, 0, 2, (UINT32)ed_tab},
    {EMI_E57 , 2, 90, 0, 2, (UINT32)ed_tab},
    {EMI_E57 , 2, 90, 1, 2, (UINT32)ed_tab},
    {EMI_E57 , 8, 85, 1, 2, (UINT32)ed_tab},
    {EMI_E57 , 8, 80, 2, 2, (UINT32)ed_tab},
    {EMI_E57 , 8, 80, 2, 3, (UINT32)ed_tab},
    {EMI_E57 , 8, 128, 3, 3, (UINT32)ed_tab},
    {EMI_E57 , 8, 128, 2, 2, (UINT32)ed_tab},
    {EMI_E57 , 0, 128, 2, 2, (UINT32)ed_tab},
    {EMI_E57 , 0, 128, 2, 2, (UINT32)ed_tab},
    {EMI_E57 , 0, 128, 3, 2, (UINT32)ed_tab},
    {EMI_E57 , 0, 128, 3, 2, (UINT32)ed_tab},
};

static UINT8 es_tab[TOTAL_GAIN_NUM][16] =
{
    {48, 56, 56, 60, 64, 64, 64, 64, 64, 64, 64, 58, 52, 46, 40, 36},
    {48, 52, 56, 60, 64, 64, 64, 64, 64, 64, 64, 60, 56, 52, 50, 48},
    {52, 56, 58, 62, 64, 64, 64, 64, 64, 64, 64, 64, 60, 58, 56, 54},
    {52, 56, 58, 62, 64, 64, 64, 64, 64, 64, 64, 64, 60, 58, 56, 54},
    {48, 52, 56, 60, 64, 64, 64, 64, 64, 64, 64, 60, 56, 52, 50, 48},
    {0,  32, 48, 56, 60, 64, 64, 64, 64, 64, 64, 58, 52, 46, 40, 36},
    {0,  32, 48, 56, 60, 64, 64, 64, 64, 64, 64, 52, 44, 38, 32, 32},
    {0, 32, 48, 52, 64, 64, 64, 64, 64, 64, 64, 52, 44, 38, 32, 32},
    {0, 32, 48, 52, 64, 64, 64, 64, 64, 64, 64, 52, 44, 38, 32, 32},
    {0, 32, 48, 52, 64, 64, 64, 64, 64, 64, 64, 52, 44, 38, 32, 32},
    {0, 32, 48, 52, 64, 64, 64, 64, 64, 64, 64, 52, 44, 38, 32, 32},
    {0, 32, 48, 52, 64, 64, 64, 64, 64, 64, 64, 52, 44, 38, 32, 32},
};

static EdgeMapS es_map[TOTAL_GAIN_NUM] =
{
    //EthrL,EthrH, EtabL, EtabH, TAB_TblAddr
    {0, 576, 5, 6, (UINT32)es_tab},
    {0, 576, 5, 6, (UINT32)es_tab},
    {0, 576, 5, 6, (UINT32)es_tab},
    {0, 576, 5, 6, (UINT32)es_tab},
    {0, 576, 5, 6, (UINT32)es_tab},
    {0, 576, 5, 6, (UINT32)es_tab},
    {0, 576, 5, 6, (UINT32)es_tab},
    {0, 576, 5, 6, (UINT32)es_tab},
    {0, 576, 5, 6, (UINT32)es_tab},
    {0, 576, 5, 6, (UINT32)es_tab},
    {0, 576, 5, 6, (UINT32)es_tab},
    {0, 576, 5, 6, (UINT32)es_tab},
};

//////////////////////
//**  IPE-LPF
//////////////////////
static LPF lpf[TOTAL_GAIN_NUM] =
{
    //lpfw,sonlyw,rangeth0,rangeth1,lpfsize
    {4, 0, 4, 8, _5x5},
    {4, 0, 5, 10, _5x5},
    {4, 0, 6, 13, _5x5},
    {4, 0, 8, 16, _5x5},
    {4, 0, 8, 16, _5x5},
    {4, 0, 8, 16, _5x5},
    {4, 0, 10, 22, _5x5},
    {4, 0, 10, 22, _5x5},
    {4, 0, 10, 22, _5x5},
    {4, 0, 10, 22, _5x5},
    {4, 0, 10, 22, _5x5},
    {4, 0, 10, 22, _5x5},
};

static INT8 saturation[TOTAL_GAIN_NUM] =
{//20/06/05 50:125
    30,
    30,
    30,
    15,
    0,
    -15,
    -25,
    -70,
    -70,
    -70,
    -70,
    -70,
};

static INT8 contrast[TOTAL_GAIN_NUM] =
{
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};

static UINT32 ccon_tab[TOTAL_GAIN_NUM][17] =
{
    {128, 132, 136, 142, 140, 132, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128},
    {128, 130, 133, 140, 137, 132, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128},
    {128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128},
    {128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128},
    {128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128},
    {128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128},
    {128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128},
    {128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128},
    {128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128},
    {128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128},
    {128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128},
    {128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128}
};

static IQS_CCTRL_INFO cctrl_info[TOTAL_GAIN_NUM] =
{
    {CCTRL_SEL_E7, 0x28},
    {CCTRL_SEL_E7, 0x28},
    {CCTRL_SEL_E7, 0x30},
    {CCTRL_SEL_E7, 0x40},
    {CCTRL_SEL_E7, 0x40},
    {CCTRL_SEL_E7, 0x40},
    {CCTRL_SEL_E7, 0x40},
    {CCTRL_SEL_E7, 0x40},
    {CCTRL_SEL_E7, 0x40},
    {CCTRL_SEL_E7, 0x40},
    {CCTRL_SEL_E7, 0x40},
    {CCTRL_SEL_E7, 0x40}
};

////////////////////////////////
//             IFE2            /
////////////////////////////////
//////////////////////
//**  IFE2-ReferenceCenter
//////////////////////
static UINT32 ref_center_y_th[TOTAL_GAIN_NUM][3] =
{
    {1, 2, 3},
    {1, 2, 3},
    {2, 4, 6},
    {6, 9, 12},
    {6, 9, 12},
    {15, 22, 29},
    {15, 22, 29},
    {15, 22, 29},
    {22, 33, 44},
    {44, 66, 88},
    {44, 66, 88},
    {44, 66, 88},
};

static UINT32 ref_center_y_wt[TOTAL_GAIN_NUM][4] =
{
    {8, 4, 2, 1},
    {8, 4, 2, 1},
    {8, 6, 4, 2},
    {8, 6, 4, 2},
    {8, 6, 4, 2},
    {8, 6, 4, 2},
    {8, 6, 4, 2},
    {8, 6, 4, 2},
    {8, 6, 4, 2},
    {8, 6, 4, 2},
    {8, 6, 4, 2},
    {8, 6, 4, 2},
};

static IFE2_REFCENT_Set ref_center_y[TOTAL_GAIN_NUM] =
{
    {(UINT32)ref_center_y_th, (UINT32)ref_center_y_wt, 31, 9, 6},
    {(UINT32)ref_center_y_th, (UINT32)ref_center_y_wt, 31, 9, 6},
    {(UINT32)ref_center_y_th, (UINT32)ref_center_y_wt, 31, 9, 6},
    {(UINT32)ref_center_y_th, (UINT32)ref_center_y_wt, 31, 9, 6},
    {(UINT32)ref_center_y_th, (UINT32)ref_center_y_wt, 31, 9, 6},
    {(UINT32)ref_center_y_th, (UINT32)ref_center_y_wt, 31, 9, 6},
    {(UINT32)ref_center_y_th, (UINT32)ref_center_y_wt, 31, 9, 6},
    {(UINT32)ref_center_y_th, (UINT32)ref_center_y_wt, 31, 9, 6},
    {(UINT32)ref_center_y_th, (UINT32)ref_center_y_wt, 31, 9, 6},
    {(UINT32)ref_center_y_th, (UINT32)ref_center_y_wt, 31, 9, 6},
    {(UINT32)ref_center_y_th, (UINT32)ref_center_y_wt, 31, 9, 6},
    {(UINT32)ref_center_y_th, (UINT32)ref_center_y_wt, 31, 9, 6},
};

static UINT32 ref_center_uv_th[TOTAL_GAIN_NUM][3] =
{
    {1, 2, 3},
    {1, 2, 3},
    {2, 4, 6},
    {7, 10, 13},
    {7, 10, 13},
    {10, 15, 18},
    {13, 19, 26},
    {13, 19, 26},
    {13, 19, 26},
    {26, 38, 52},
    {26, 38, 52},
    {26, 38, 52},
};

static UINT32 ref_center_uv_wt[TOTAL_GAIN_NUM][4] =
{
    {8, 4, 2, 1},
    {8, 4, 2, 1},
    {8, 6, 4, 2},
    {8, 6, 4, 2},
    {8, 6, 4, 2},
    {8, 6, 4, 2},
    {8, 6, 4, 2},
    {8, 6, 4, 2},
    {8, 6, 4, 2},
    {8, 6, 4, 2},
    {8, 6, 4, 2},
    {8, 6, 4, 2},
};

static IFE2_REFCENT_Set ref_center_uv[TOTAL_GAIN_NUM] =
{
    {(UINT32)ref_center_uv_th, (UINT32)ref_center_uv_wt, 31, 9, 6},
    {(UINT32)ref_center_uv_th, (UINT32)ref_center_uv_wt, 31, 9, 6},
    {(UINT32)ref_center_uv_th, (UINT32)ref_center_uv_wt, 31, 9, 6},
    {(UINT32)ref_center_uv_th, (UINT32)ref_center_uv_wt, 31, 9, 6},
    {(UINT32)ref_center_uv_th, (UINT32)ref_center_uv_wt, 31, 9, 6},
    {(UINT32)ref_center_uv_th, (UINT32)ref_center_uv_wt, 31, 9, 6},
    {(UINT32)ref_center_uv_th, (UINT32)ref_center_uv_wt, 31, 9, 6},
    {(UINT32)ref_center_uv_th, (UINT32)ref_center_uv_wt, 31, 9, 6},
    {(UINT32)ref_center_uv_th, (UINT32)ref_center_uv_wt, 31, 9, 6},
    {(UINT32)ref_center_uv_th, (UINT32)ref_center_uv_wt, 31, 9, 6},
    {(UINT32)ref_center_uv_th, (UINT32)ref_center_uv_wt, 31, 9, 6},
    {(UINT32)ref_center_uv_th, (UINT32)ref_center_uv_wt, 31, 9, 6},
};

//////////////////////
//**  IFE2-Filter
//////////////////////
static UINT32 ife2filter_y_th[TOTAL_GAIN_NUM][5] =
{
    {1, 2, 3, 4, 5},
    {1, 2, 3, 4, 5},
    {4, 6, 8, 10, 12},
    {8, 11, 14, 17, 20},
    {8, 11, 14, 17, 20},
    {15, 22, 29, 36, 43},
    {15, 22, 29, 36, 43},
    {15, 22, 29, 36, 43},
    {22, 33, 44, 55, 66},
    {44, 66, 88, 110, 132},
    {44, 66, 88, 110, 132},
    {44, 66, 88, 110, 132},
};

static UINT32 ife2filter_y_wt[TOTAL_GAIN_NUM][6] =
{
    {16, 8, 4, 2, 1, 0},
    {16, 8, 4, 2, 1, 0},
    {16, 8, 4, 2, 1, 0},
    {16, 13, 10, 7, 4, 1},
    {16, 13, 10, 7, 4, 1},
    {16, 13, 10, 7, 4, 1},
    {16, 13, 10, 7, 4, 1},
    {16, 13, 10, 7, 4, 1},
    {16, 13, 10, 7, 4, 1},
    {16, 13, 10, 7, 4, 1},
    {16, 13, 10, 7, 4, 1},
    {16, 13, 10, 7, 4, 1},
};

static UINT32 ife2filter_uv_th[TOTAL_GAIN_NUM][5] =
{
    {1, 1, 3, 3, 5},
    {1, 1, 3, 3, 5},
    {6, 8, 10, 12, 14},
    {10, 14, 18, 22, 26},
    {10, 15, 20, 25, 30},
    {10, 14, 18, 21, 25},
    {10, 14, 18, 21, 25},
    {10, 14, 18, 21, 25},
    {13, 19, 26, 32, 39},
    {26, 38, 52, 64, 78},
    {26, 38, 52, 64, 78},
    {26, 38, 52, 64, 78},
};

static UINT32 ife2filter_uv_wt[TOTAL_GAIN_NUM][6] =
{
    {16, 16, 8, 8, 4, 4},
    {16, 16, 8, 8, 4, 4},
    {16, 13, 10, 7, 4, 1},
    {16, 13, 10, 7, 4, 1},
    {16, 13, 10, 7, 4, 1},
    {16, 13, 10, 7, 4, 1},
    {16, 13, 10, 7, 4, 1},
    {16, 13, 10, 7, 4, 1},
    {16, 13, 10, 7, 4, 1},
    {16, 13, 10, 7, 4, 1},
    {16, 13, 10, 7, 4, 1},
    {16, 13, 10, 7, 4, 1},
};

static IQS_IFE2_FILTER ife2filter[TOTAL_GAIN_NUM] =
{
    //RepNum,FiltSize,     {uiEdthHV, uiEdthPN},EdgeKernelSize, FiltSetY,
    {1, {FLTR_SIZE_9x9, {32, 32}, EKNL_SIZE_3x3, {(UINT32)ife2filter_y_th, (UINT32)ife2filter_y_wt},  {(UINT32)ife2filter_uv_th, (UINT32)ife2filter_uv_wt}, {(UINT32)ife2filter_uv_th, (UINT32)ife2filter_uv_wt}}},
    {1, {FLTR_SIZE_9x9, {32, 32}, EKNL_SIZE_3x3, {(UINT32)ife2filter_y_th, (UINT32)ife2filter_y_wt},  {(UINT32)ife2filter_uv_th, (UINT32)ife2filter_uv_wt}, {(UINT32)ife2filter_uv_th, (UINT32)ife2filter_uv_wt}}},
    {1, {FLTR_SIZE_9x9, {18, 18}, EKNL_SIZE_3x3, {(UINT32)ife2filter_y_th, (UINT32)ife2filter_y_wt},  {(UINT32)ife2filter_uv_th, (UINT32)ife2filter_uv_wt}, {(UINT32)ife2filter_uv_th, (UINT32)ife2filter_uv_wt}}},
    {1, {FLTR_SIZE_9x9, {18, 18}, EKNL_SIZE_3x3, {(UINT32)ife2filter_y_th, (UINT32)ife2filter_y_wt},  {(UINT32)ife2filter_uv_th, (UINT32)ife2filter_uv_wt}, {(UINT32)ife2filter_uv_th, (UINT32)ife2filter_uv_wt}}},
    {1, {FLTR_SIZE_9x9, {14, 14}, EKNL_SIZE_3x3, {(UINT32)ife2filter_y_th, (UINT32)ife2filter_y_wt},  {(UINT32)ife2filter_uv_th, (UINT32)ife2filter_uv_wt}, {(UINT32)ife2filter_uv_th, (UINT32)ife2filter_uv_wt}}},
    {1, {FLTR_SIZE_9x9, {18, 18}, EKNL_SIZE_3x3, {(UINT32)ife2filter_y_th, (UINT32)ife2filter_y_wt},  {(UINT32)ife2filter_uv_th, (UINT32)ife2filter_uv_wt}, {(UINT32)ife2filter_uv_th, (UINT32)ife2filter_uv_wt}}},
    {1, {FLTR_SIZE_9x9, {14, 14}, EKNL_SIZE_3x3, {(UINT32)ife2filter_y_th, (UINT32)ife2filter_y_wt},  {(UINT32)ife2filter_uv_th, (UINT32)ife2filter_uv_wt}, {(UINT32)ife2filter_uv_th, (UINT32)ife2filter_uv_wt}}},
    {1, {FLTR_SIZE_9x9, {14, 14}, EKNL_SIZE_3x3, {(UINT32)ife2filter_y_th, (UINT32)ife2filter_y_wt},  {(UINT32)ife2filter_uv_th, (UINT32)ife2filter_uv_wt}, {(UINT32)ife2filter_uv_th, (UINT32)ife2filter_uv_wt}}},
    {1, {FLTR_SIZE_9x9, {14, 14}, EKNL_SIZE_3x3, {(UINT32)ife2filter_y_th, (UINT32)ife2filter_y_wt},  {(UINT32)ife2filter_uv_th, (UINT32)ife2filter_uv_wt}, {(UINT32)ife2filter_uv_th, (UINT32)ife2filter_uv_wt}}},
    {1, {FLTR_SIZE_9x9, {14, 14}, EKNL_SIZE_3x3, {(UINT32)ife2filter_y_th, (UINT32)ife2filter_y_wt},  {(UINT32)ife2filter_uv_th, (UINT32)ife2filter_uv_wt}, {(UINT32)ife2filter_uv_th, (UINT32)ife2filter_uv_wt}}},
    {1, {FLTR_SIZE_9x9, {14, 14}, EKNL_SIZE_3x3, {(UINT32)ife2filter_y_th, (UINT32)ife2filter_y_wt},  {(UINT32)ife2filter_uv_th, (UINT32)ife2filter_uv_wt}, {(UINT32)ife2filter_uv_th, (UINT32)ife2filter_uv_wt}}},
    {1, {FLTR_SIZE_9x9, {14, 14}, EKNL_SIZE_3x3, {(UINT32)ife2filter_y_th, (UINT32)ife2filter_y_wt},  {(UINT32)ife2filter_uv_th, (UINT32)ife2filter_uv_wt}, {(UINT32)ife2filter_uv_th, (UINT32)ife2filter_uv_wt}}},
};

//////////////////////
//**  IFE2-Gray
//////////////////////
static IFE2_GRAY_Statal gray_statical[TOTAL_GAIN_NUM] =
{
    {125, 131, 125, 131},
    {125, 131, 125, 131},
    {125, 131, 125, 131},
    {125, 131, 125, 131},
    {125, 131, 125, 131},
    {125, 131, 125, 131},
    {125, 131, 125, 131},
    {125, 131, 125, 131},
    {125, 131, 125, 131},
    {125, 131, 125, 131},
    {125, 131, 125, 131},
    {125, 131, 125, 131},
};

////////////////////////////////
//             IME             /
////////////////////////////////

//////////////////////
//**  IME-LCA
//////////////////////
static IME_CHRA_ADAP chroma_adapt[TOTAL_GAIN_NUM] =
{
    //LcaRefWt{RefY,RefUV,OutUV},LcaYinfo{LcaY,LcaYTh,LcaYWtPrc,LcaYWtS,LcaYWtE}, LcaUVinfo{UV, UVTh, UVWtPrc, UVWtS, UVWtE}, SubRatio
    {{28, 30, 5}, {RANGE_8, 0, RANGE_8, 8, 0}, {RANGE_8, 0, RANGE_8, 8, 0}, 25},
    {{28, 30, 6}, {RANGE_8, 0, RANGE_8, 8, 0}, {RANGE_8, 0, RANGE_8, 8, 0}, 25},
    {{28, 30, 7}, {RANGE_8, 0, RANGE_8, 8, 0}, {RANGE_8, 0, RANGE_8, 8, 0}, 25},
    {{28, 30, 8}, {RANGE_8, 0, RANGE_8, 8, 0}, {RANGE_8, 0, RANGE_8, 8, 0}, 25},
    {{30, 30, 9}, {RANGE_8, 0, RANGE_8, 8, 0}, {RANGE_8, 0, RANGE_8, 8, 0}, 25},
    {{30, 30, 12}, {RANGE_8, 0, RANGE_8, 8, 0}, {RANGE_8, 0, RANGE_8, 8, 0}, 25},
    {{18, 18, 12}, {RANGE_8, 0, RANGE_8, 8, 5}, {RANGE_16, 0, RANGE_16, 16, 0}, 25},
    {{18, 18, 12}, {RANGE_8, 0, RANGE_8, 8, 5}, {RANGE_16, 0, RANGE_16, 16, 0}, 25},
    {{18, 18, 12}, {RANGE_8, 0, RANGE_8, 8, 5}, {RANGE_16, 0, RANGE_16, 16, 0}, 25},
    {{18, 18, 12}, {RANGE_16, 0, RANGE_16, 16, 12}, {RANGE_32, 0, RANGE_32, 32, 16}, 25},
    {{18, 18, 12}, {RANGE_16, 0, RANGE_16, 16, 12}, {RANGE_32, 0, RANGE_32, 32, 16}, 25},
    {{18, 18, 12}, {RANGE_16, 0, RANGE_16, 16, 12}, {RANGE_32, 0, RANGE_32, 32, 16}, 25},
};

static IME_CHRA_CADAP chroma_adapt_color[TOTAL_GAIN_NUM] =
{
    //LcaCLREn, LcaRefWt{CentU, CentV}, LcaCLRinfo{LumRng,LumTh,LumWtPrc,LumWtS,LumWtE}
    {DISABLE,         {128,     128},          {RANGE_8,   0,   RANGE_8,   8,     0}},
    {DISABLE,         {128,     128},          {RANGE_8,   0,   RANGE_8,   8,     0}},
    {DISABLE,         {128,     128},          {RANGE_8,   0,   RANGE_8,   8,     0}},
    {DISABLE,         {128,     128},          {RANGE_8,   0,   RANGE_8,   8,     0}},
    {DISABLE,         {128,     128},          {RANGE_8,   0,   RANGE_8,   8,     0}},
    {DISABLE,         {128,     128},          {RANGE_8,   0,   RANGE_8,   8,     0}},
    {DISABLE,         {128,     128},          {RANGE_8,   0,   RANGE_8,   8,     0}},
    {DISABLE,         {128,     128},          {RANGE_8,   0,   RANGE_8,   8,     0}},
    {DISABLE,         {128,     128},          {RANGE_8,   0,   RANGE_8,   8,     0}},
    {DISABLE,         {128,     128},          {RANGE_8,   0,   RANGE_8,   8,     0}},
    {DISABLE,         {128,     128},          {RANGE_8,   0,   RANGE_8,   8,     0}},
    {DISABLE,         {128,     128},          {RANGE_8,   0,   RANGE_8,   8,     0}},
};

static IME_CHRA_LADAP chroma_adapt_lum[TOTAL_GAIN_NUM] =
{
    //LcaLumEn, LcaRefWt{RefWt,OutWt}, LcaLuminfo{LumRng, LumTh,LumWtPrc,LumWtS,LumWtE}
    {DISABLE,         {16,     8},          {RANGE_8,   0,   RANGE_8,   0,     8}},
    {DISABLE,         {16,     8},          {RANGE_8,   0,   RANGE_8,   0,     8}},
    {DISABLE,         {16,     8},          {RANGE_8,   0,   RANGE_8,   0,     8}},
    {ENABLE,         {8,     8},          {RANGE_8,   0,   RANGE_8,   0,     8}},
    {ENABLE,         {8,     8},          {RANGE_8,   0,   RANGE_8,   0,     8}},
    {ENABLE,         {8,     8},          {RANGE_8,   0,   RANGE_8,   0,     8}},
    {ENABLE,         {24,     24},          {RANGE_8,   0,   RANGE_8,   0,     6}},
    {ENABLE,         {24,     24},          {RANGE_8,   0,   RANGE_8,   0,     6}},
    {ENABLE,         {24,     24},          {RANGE_8,   0,   RANGE_8,   0,     6}},
    {ENABLE,         {24,     24},          {RANGE_8,   0,   RANGE_8,   0,     6}},
    {ENABLE,         {24,     24},          {RANGE_8,   0,   RANGE_8,   0,     6}},
    {ENABLE,         {24,     24},          {RANGE_8,   0,   RANGE_8,   0,     6}},
};

//////////////////////
//**  IME-ChromaSuppression
//////////////////////
static UINT32 chroma_supp_w_y[TOTAL_GAIN_NUM][16] =
{
    {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
    {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
    {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
    {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
    {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
    {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
    {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
    {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
    {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
    {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
    {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
    {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
};
static UINT32 chroma_supp_w_c[TOTAL_GAIN_NUM][16] =
{
    {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
    {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
    {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
    {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
    {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
    {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
    {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
    {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
    {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
    {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
    {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
    {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
};
static IME_ChromaSupp chroma_supp[TOTAL_GAIN_NUM] =
{
    //ModeSel,      Cetner,     Step,   puiWeightY,                      puiWeightC
    { 0, DBCS_DK_MODE, {128, 128}, {0, 0}, (UINT32*)chroma_supp_w_y, (UINT32*)chroma_supp_w_c},
    { 0, DBCS_DK_MODE, {128, 128}, {0, 0}, (UINT32*)chroma_supp_w_y, (UINT32*)chroma_supp_w_c},
    { 0, DBCS_DK_MODE, {128, 128}, {0, 0}, (UINT32*)chroma_supp_w_y, (UINT32*)chroma_supp_w_c},
    { 0, DBCS_DK_MODE, {128, 128}, {0, 0}, (UINT32*)chroma_supp_w_y, (UINT32*)chroma_supp_w_c},
    { 0, DBCS_DK_MODE, {128, 128}, {0, 0}, (UINT32*)chroma_supp_w_y, (UINT32*)chroma_supp_w_c},
    { 0, DBCS_DK_MODE, {128, 128}, {0, 0}, (UINT32*)chroma_supp_w_y, (UINT32*)chroma_supp_w_c},
    { 0, DBCS_DK_MODE, {128, 128}, {0, 0}, (UINT32*)chroma_supp_w_y, (UINT32*)chroma_supp_w_c},
    { 0, DBCS_DK_MODE, {128, 128}, {0, 0}, (UINT32*)chroma_supp_w_y, (UINT32*)chroma_supp_w_c},
    { 0, DBCS_DK_MODE, {128, 128}, {0, 0}, (UINT32*)chroma_supp_w_y, (UINT32*)chroma_supp_w_c},
    { 0, DBCS_DK_MODE, {128, 128}, {0, 0}, (UINT32*)chroma_supp_w_y, (UINT32*)chroma_supp_w_c},
    { 0, DBCS_DK_MODE, {128, 128}, {0, 0}, (UINT32*)chroma_supp_w_y, (UINT32*)chroma_supp_w_c},
    { 0, DBCS_DK_MODE, {128, 128}, {0, 0}, (UINT32*)chroma_supp_w_y, (UINT32*)chroma_supp_w_c},
};
//////////////////////
//**  IME-TMNR
//////////////////////
static IME_HAL_TMNR_INFO tmnr_info_1x =
{
    0, 1, 1, 1, 1, 1, 40,
    {6, 10, 4, 8},
    2, 2, 8, 8,
    {{{50, 12, 11, 170},{60, 10, 12, 170},{60, 10, 10, 155},{70, 8, 10, 160},{80, 8, 10, 160},{80, 8, 9, 110},{80, 8, 8, 110},{80, 8, 8, 110}},
    {{120, 10},{145, 9},{120, 11},{50, 11},{70, 10},{70, 10},{30, 12},{30, 12}},
    {{120, 10},{145, 9},{120, 11},{50, 11},{70, 10},{70, 10},{30, 12},{30, 12}}},
    {11, 33, 55, 77},
    {13, 32, 50, 67},
    {40, 14, 7, 3},
    {11, 33, 55, 77},
    {40, 14, 7, 3}
};
static IME_HAL_TMNR_INFO tmnr_info_2x =
{
    0, 1, 1, 1, 1, 1, 40,
    {6, 10, 4, 8},
    2, 2, 8, 8,
    {{{75, 12, 10, 100},{75, 12, 11, 120},{75, 12, 11, 120},{75, 10, 11, 150},{80, 10, 11, 180},{90, 10, 10, 150},{100, 10, 9, 130},{100, 10, 9, 130}},
    {{59, 7},{71, 6},{78, 8},{75, 8},{73, 7},{65, 7},{64, 9},{64, 9}},
    {{59, 7},{71, 6},{78, 8},{75, 8},{73, 7},{65, 7},{64, 9},{64, 9}}},
    {13, 32, 50, 67},
    {13, 32, 50, 67},
    {40, 14, 7, 3},
    {13, 32, 50, 67},
    {40, 14, 7, 3}
};
static IME_HAL_TMNR_INFO tmnr_info_4x =
{
    0, 1, 1, 1, 1, 1, 40,
    {6, 10, 4, 8},
    2, 2, 8, 8,
    {{{100, 12, 10, 100},{64, 12, 12, 140},{60, 12, 13, 140},{75, 10, 12, 140},{134, 10, 12, 150},{134, 10, 9, 150},{134, 10, 8, 110},{134, 10, 8, 110}},
    {{76, 6},{76, 6},{76, 6},{76, 6},{76, 6},{76, 6},{76, 6},{76, 6}},
    {{76, 6},{76, 6},{76, 6},{76, 6},{76, 6},{76, 6},{76, 6},{76, 6}}},
    {11, 33, 55, 77},
    {13, 32, 50, 67},
    {40, 14, 7, 3},
    {11, 33, 55, 77},
    {40, 14, 7, 3}
};
static IME_HAL_TMNR_INFO tmnr_info_8x =
{
    0, 1, 1, 1, 1, 1, 40,
    {6, 12, 4, 8},
    3, 1, 8, 15,
    {{{80, 12, 11, 120},{64, 12, 12, 150},{64, 12, 12, 150},{75, 10, 12, 140},{134, 10, 13, 160},{134, 10, 12, 130},{134, 10, 9, 100},{134, 10, 9, 100}},
    {{90, 9},{100, 12},{101, 12},{102, 12},{103, 12},{100, 12},{88, 12},{88, 12}},
    {{90, 9},{100, 12},{101, 12},{102, 12},{103, 12},{100, 12},{88, 12},{88, 12}}},
    {11, 33, 55, 77},
    {13, 32, 50, 67},
    {40, 14, 7, 3},
    {11, 33, 55, 77},
    {40, 14, 7, 3}
};
static IME_HAL_TMNR_INFO tmnr_info_16x =
{
    0, 1, 1, 1, 1, 1, 40,
    {8, 12, 4, 8},
    3, 1, 15, 16,
    {{{100, 12, 12, 180},{100, 12, 13, 190},{134, 12, 13, 160},{134, 10, 13, 160},{134, 10, 13, 160},{134, 10, 10, 160},{134, 10, 10, 140},{134, 10, 10, 120}},
    {{90, 12},{100, 14},{101, 15},{102, 15},{103, 15},{100, 16},{88, 14},{88, 14}},
    {{90, 12},{100, 14},{101, 15},{102, 15},{103, 15},{100, 16},{88, 14},{88, 14}}},
    {11, 33, 55, 77},
    {11, 33, 55, 77},
    {40, 14, 7, 3},
    {11, 33, 55, 77},
    {40, 14, 7, 3}
};
static IME_HAL_TMNR_INFO tmnr_info_32x =
{
    0, 1, 1, 1, 1, 1, 40,
    {8, 12, 4, 8},
    3, 1, 16, 17,
    {{{100, 12, 11, 230},{100, 12, 13, 230},{144, 12, 12, 220},{144, 10, 10, 240},{144, 10, 10, 240},{144, 10, 8, 240},{144, 10, 9, 240},{144, 10, 9, 240}},
    {{110, 18},{110, 18},{110, 18},{110, 18},{110, 18},{110, 18},{110, 18},{110, 18}},
    {{110, 18},{110, 18},{110, 18},{110, 18},{110, 18},{110, 18},{110, 18},{110, 18}}},
    {11, 33, 55, 77},
    {11, 33, 55, 77},
    {56, 19, 9, 4},
    {11, 41, 68, 96},
    {40, 14, 7, 3}
};
static IME_HAL_TMNR_INFO tmnr_info_64x =
{
    0, 1, 1, 1, 1, 1, 40,
    {8, 12, 4, 8},
    2, 1, 16, 18,
    {{{100, 12, 10, 180},{100, 12, 10, 180},{148, 12, 10, 200},{148, 12, 9, 240},{132, 10, 9, 240},{132, 10, 9, 240},{132, 10, 9, 180},{132, 10, 9, 180}},
    {{110, 16},{110, 16},{110, 16},{110, 16},{110, 16},{110, 16},{110, 16},{110, 16}},
    {{110, 16},{110, 16},{110, 16},{110, 16},{110, 16},{110, 16},{110, 16},{110, 16}}},
    {11, 33, 55, 77},
    {11, 33, 55, 77},
    {40, 14, 7, 3},
    {11, 41, 68, 96},
    {40, 14, 7, 3}
};
static IME_HAL_TMNR_INFO tmnr_info_128x =
{
    0, 1, 1, 1, 1, 1, 40,
    {8, 12, 4, 8},
    2, 1, 16, 16,
    {{{164, 17, 13, 160},{164, 17, 13, 200},{164, 17, 13, 300},{164, 15, 13, 400},{148, 15, 12, 420},{148, 15, 11, 480},{148, 15, 11, 400},{148, 15, 11, 300}},
    {{54, 20},{74, 25},{80, 26},{62, 24},{48, 20},{30, 22},{52, 22},{52, 22}},
    {{54, 20},{74, 25},{80, 26},{62, 24},{48, 20},{30, 22},{52, 22},{52, 22}}},
    {11, 33, 55, 77},
    {11, 33, 55, 77},
    {40, 14, 7, 3},
    {11, 33, 55, 77},
    {40, 14, 7, 3}
};
static IME_HAL_TMNR_INFO tmnr_info_256x =
{
    1, 1, 1, 1, 1, 1, 40,
    {8, 12, 4, 8},
    6, 4, 16, 16,
    {{{64, 18, 6, 720},{64, 18, 6, 720},{64, 18, 6, 720},{64, 16, 5, 680},{64, 16, 5, 680},{64, 12, 5, 650},{64, 12, 5, 650},{64, 12, 5, 650}},
    {{66, 23},{98, 30},{105, 33},{78, 29},{57, 22},{64, 25},{64, 25},{64, 25}},
    {{66, 23},{98, 30},{105, 33},{78, 29},{57, 22},{64, 25},{64, 25},{64, 25}}},
    {11, 33, 55, 77},
    {18, 32, 42, 49},
    {40, 14, 7, 3},
    {11, 33, 55, 77},
    {37, 19, 11, 7}
};
static IME_HAL_TMNR_INFO tmnr_info_512x =
{
    1, 1, 1, 1, 1, 1, 40,
    {12, 18, 8, 16},
    6, 4, 16, 16,
    {{{64, 20, 10, 850},{64, 20, 10, 850},{64, 20, 10, 850},{64, 16, 8, 800},{64, 16, 6, 800},{64, 16, 6, 750},{64, 16, 6, 750},{64, 16, 6, 750}},
    {{66, 24},{98, 24},{105, 24},{78, 24},{57, 24},{64, 24},{64, 24},{64, 24}},
    {{66, 24},{98, 24},{105, 24},{78, 24},{57, 24},{64, 24},{64, 24},{64, 24}}},
    {11, 33, 55, 77},
    {18, 32, 42, 49},
    {40, 14, 7, 3},
    {11, 33, 55, 77},
    {37, 19, 11, 7}
};
static IME_HAL_TMNR_INFO tmnr_info_1024x =
{
    1, 1, 1, 1, 1, 1, 40,
    {12, 18, 8, 16},
    6, 6, 16, 16,
    {{{64, 32, 10, 1000},{64, 32, 10, 1000},{64, 32, 10, 1000},{64, 32, 8, 900},{64, 28, 8, 850},{64, 28, 8, 850},{64, 28, 8, 850},{64, 28, 8, 850}},
    {{160, 30},{160, 30},{160, 30},{140, 30},{140, 30},{140, 30},{100, 30},{84, 30}},
    {{160, 30},{160, 30},{160, 30},{140, 30},{140, 30},{140, 30},{100, 30},{84, 30}}},
    {11, 33, 55, 77},
    {18, 32, 42, 49},
    {40, 14, 7, 3},
    {11, 33, 55, 77},
    {37, 19, 11, 7}
};
static IME_HAL_TMNR_INFO tmnr_info_2048x =
{
    1, 1, 1, 1, 1, 1, 40,
    {8, 16, 8, 16},
    10, 12, 16, 16,
    {{{120, 32, 24, 32},{120, 40, 24, 32},{120, 40, 24, 64},{120, 40, 24, 64},{120, 40, 24, 64},{120, 40, 24, 64},{120, 40, 24, 64},{120, 40, 24, 64}},
    {{66, 23},{98, 30},{105, 33},{78, 29},{57, 22},{64, 25},{64, 25},{64, 25}},
    {{66, 23},{98, 30},{105, 33},{78, 29},{57, 22},{64, 25},{64, 25},{64, 25}}},
    {11, 33, 55, 77},
    {18, 32, 42, 49},
    {40, 14, 7, 3},
    {11, 33, 55, 77},
    {37, 19, 11, 7}
};
static IME_HAL_TMNR_INFO* tmnr_info[TOTAL_GAIN_NUM] =
{
    &tmnr_info_1x,
    &tmnr_info_2x,
    &tmnr_info_4x,
    &tmnr_info_8x,
    &tmnr_info_16x,
    &tmnr_info_32x,
    &tmnr_info_64x,
    &tmnr_info_128x,
    &tmnr_info_256x,
    &tmnr_info_512x,
    &tmnr_info_1024x,
    &tmnr_info_2048x
};

////////////////////////////////
//            Codec            /
////////////////////////////////

////////////////////
// SIE-ECS
////////////////////
static SIE_EmbeddedCS ecs = {ECS_D2F8, ENABLE, ENABLE, _BIT_0_1};
static UINT32 ecs_tab[4225] =
{

    0x10840900,0x10E40D00,0x10F41100,0x10E41500,0x10C41900,0x11242100,0x10D41100,0x10C41100,0x10E41500,0x10E41100,0x10D40D00,0x10B41100,0x10941100,0x10C41900,0x10A41500,0x10C41500,0x10A41100,0x10B41500,0x10941100,0x10740900,0x10B41900,0x10941500,0x10641100,0x10841100,0x10841100,0x10740D00,0x10840D00,0x10841500,0x10540900,0x10540900,0x10840D00,0x10740D00,0x10640900,0x10640900,0x10741100,0x10740900,0x10640900,0x10540900,0x10540900,0x10741100,0x10740D00,0x10740900,0x10740900,0x10740D00,0x10840D00,0x10740500,0x10640500,0x10840D00,0x10840900,0x10840900,0x10740D00,0x10740500,0x10840500,0x10740900,0x10A40900,0x10C41100,0x10940100,0x10A40500,0x10A40900,0x10B40500,0x10C40D00,0x10D41100,0x10D41100,0x10D40900,0x10740100,
    0x10A41100,0x10E41500,0x10D41100,0x10D41900,0x10D41100,0x10A40900,0x10B40D00,0x10840D00,0x10B41500,0x10A41500,0x10A41500,0x10840D00,0x10A41100,0x10941100,0x10941500,0x10740D00,0x10640D00,0x10640900,0x10941100,0x10841100,0x10640900,0x10841100,0x10640D00,0x10641100,0x10941500,0x10540D00,0x10640D00,0x10440500,0x10440500,0x10540D00,0x10540900,0x10540D00,0x10640D00,0x10540900,0x10440D00,0x10740D00,0x10340900,0x10640D00,0x10440500,0x10640D00,0x10640900,0x10240100,0x10540D00,0x10440900,0x10540500,0x10440500,0x10440100,0x10540500,0x10540500,0x10640900,0x10540900,0x10740500,0x10740900,0x10640500,0x10840900,0x10840100,0x10640100,0x10840500,0x10940100,0x10840100,0x10A40D00,0x10840100,0x10940101,0x10940101,0x10B40106,
    0x10941100,0x10D41500,0x10A41100,0x10C40D00,0x10B41500,0x10B40D00,0x10940900,0x10B41100,0x10A40900,0x10940D00,0x10840D00,0x10740900,0x10840D00,0x10741100,0x10841500,0x10640D00,0x10740D00,0x10641100,0x10640D00,0x10741100,0x10640900,0x10540900,0x10440100,0x10440900,0x10440900,0x10440500,0x10240100,0x10440D00,0x10540D00,0x10440900,0x10540900,0x10340500,0x10440900,0x10440900,0x10340900,0x10340500,0x10340900,0x10540D00,0x10240900,0x10340500,0x10440D00,0x10240500,0x10240500,0x10240500,0x10240500,0x10340100,0x10440500,0x10240500,0x10440500,0x10340100,0x10440100,0x10640500,0x10540900,0x10540100,0x10640500,0x10740101,0x10640100,0x10640100,0x10640100,0x10940500,0x10840102,0x10940101,0x10840101,0x10940102,0x10A40100,
    0x10C41100,0x10E41500,0x10C40D00,0x10B41100,0x10C41100,0x10840D00,0x10C41100,0x10B41100,0x10B41500,0x10940D00,0x10740D00,0x10841500,0x10740900,0x10840D00,0x10640D00,0x10840D00,0x10741100,0x10540900,0x10540900,0x10540D00,0x10540D00,0x10741100,0x10540D00,0x10540D00,0x10540900,0x10540900,0x10440D00,0x10440D00,0x10240500,0x10540D00,0x10440D00,0x10340900,0x10240500,0x10340500,0x10140500,0x10440900,0x10240500,0x10440900,0x10340500,0x10340500,0x10340500,0x10340500,0x10440500,0x10340500,0x10240100,0x10440100,0x10340500,0x10240100,0x10540900,0x10340100,0x10540100,0x10540100,0x10540100,0x10340100,0x10540101,0x10840D00,0x10640100,0x10740500,0x10740100,0x10640100,0x10A40100,0x10B40102,0x10A40100,0x10A40103,0x10A40106,
    0x10840900,0x10C41100,0x10B40D00,0x10C40900,0x10B41500,0x10C41500,0x10740500,0x10A40D00,0x10941100,0x10941100,0x10740D00,0x10640D00,0x10941100,0x10740D00,0x10840D00,0x10640900,0x10741100,0x10641100,0x10640D00,0x10540D00,0x10540900,0x10440900,0x10440900,0x10640D00,0x10340500,0x10540D00,0x10240500,0x10340500,0x10340500,0x10440500,0x10440900,0x10540900,0x10340500,0x10140500,0x10440900,0x10240900,0x10340900,0x10340900,0x10340500,0x10240100,0x10340900,0x10340500,0x10340500,0x10340900,0x10340500,0x10340100,0x10340500,0x10440500,0x10340100,0x10540100,0x10440100,0x10640500,0x10540500,0x10440100,0x10440100,0x10440100,0x10640100,0x10640100,0x10640100,0x10740101,0x10840100,0x10840104,0x10A40101,0x10840100,0x10B40105,
    0x10C41100,0x10B40D00,0x10B41100,0x10A40D00,0x10C41500,0x10B40D00,0x10940D00,0x10A41100,0x10A41500,0x10841100,0x10640900,0x10941100,0x10740D00,0x10640D00,0x10641100,0x10640D00,0x10740D00,0x10440D00,0x10540D00,0x10540900,0x10440900,0x10640900,0x10340900,0x10440900,0x10340900,0x10340900,0x10340500,0x10340900,0x10240500,0x10540900,0x10240900,0x10340900,0x10540900,0x10240500,0x10540D00,0x10240500,0x10440500,0x10340D00,0x10240100,0x10440900,0x10140500,0x10240100,0x10340500,0x10340500,0x10340900,0x10240500,0x10540500,0x10440100,0x10340100,0x10340100,0x10540500,0x10340100,0x10440100,0x10240100,0x10440100,0x10440100,0x10640100,0x10840100,0x10840101,0x10840100,0x10840101,0x10840101,0x10640100,0x10840103,0x10A40103,
    0x10941100,0x10940D00,0x10A40D00,0x10D41500,0x10A41100,0x10C41500,0x10940D00,0x10840900,0x10A41100,0x10540500,0x10740D00,0x10740D00,0x10740D00,0x10740D00,0x10640D00,0x10540900,0x10540D00,0x10640D00,0x10340900,0x10540D00,0x10340500,0x10540D00,0x10540D00,0x10440900,0x10340900,0x10240500,0x10340500,0x10240900,0x10540900,0x10240900,0x10340900,0x10340500,0x10440900,0x10440D00,0x10240500,0x10340900,0x10240900,0x10340900,0x10440900,0x10340D00,0x10340900,0x10340500,0x10340900,0x10440900,0x10240500,0x10340100,0x10440500,0x10340100,0x10340100,0x10440100,0x10440100,0x10440100,0x10340100,0x10540100,0x10540101,0x10640100,0x10740100,0x10640101,0x10640102,0x10840100,0x10840101,0x10840102,0x10740102,0x10740104,0x10A40103,
    0x10D41900,0x10B40900,0x10A40D00,0x10A40900,0x10C41100,0x10941100,0x10B41500,0x10840900,0x10840D00,0x10741100,0x10640D00,0x10740D00,0x10740D00,0x10540D00,0x10641100,0x10540D00,0x10640D00,0x10340100,0x10740D00,0x10640D00,0x10540D00,0x10540900,0x10540900,0x10540900,0x10340500,0x10340900,0x10240500,0x10540D00,0x10340900,0x10240900,0x10540D00,0x10340900,0x10240500,0x10240900,0x10240500,0x10140100,0x10240500,0x10240100,0x10340500,0x10440900,0x10440900,0x10240500,0x10340900,0x10340500,0x10140100,0x10240100,0x10340100,0x10440500,0x10540D00,0x10240100,0x10440100,0x10440100,0x10440100,0x10440100,0x10540100,0x10440101,0x10540100,0x10640101,0x10540101,0x10640101,0x10740100,0x10640100,0x10840103,0x10940102,0x10A40107,
    0x10A40D00,0x10A40D00,0x10D41500,0x10A40D00,0x10C41100,0x10A41100,0x10A41100,0x10941100,0x10941100,0x10640D00,0x10741100,0x10741100,0x10741100,0x10640900,0x10640D00,0x10740D00,0x10540900,0x10440100,0x10540500,0x10340500,0x10440900,0x10240500,0x10340D00,0x10540D00,0x10140500,0x10240500,0x10340500,0x10540D00,0x10240500,0x10340500,0x10240500,0x10240500,0x10340900,0x10240900,0x10240500,0x10240500,0x10240500,0x10240500,0x10340500,0x10240900,0x10340500,0x10540900,0x10540D00,0x10240100,0x10340500,0x10340100,0x10440900,0x10340500,0x10340500,0x10440100,0x10540900,0x10340100,0x10440100,0x10440100,0x10540100,0x10440100,0x10440100,0x10540101,0x10640101,0x10640100,0x10740101,0x10640100,0x10740101,0x10940101,0x10940105,
    0x10740500,0x10940D00,0x10940900,0x10B41100,0x10A41500,0x10A41500,0x10840900,0x10740D00,0x10941100,0x10640900,0x10740D00,0x10641100,0x10741100,0x10741100,0x10640D00,0x10540D00,0x10540900,0x10540900,0x10340500,0x10340900,0x10240500,0x10340900,0x10340500,0x10440500,0x10440900,0x10240100,0x10340500,0x10340500,0x10340100,0x10140500,0x10440900,0x10140100,0x10440D00,0x10240500,0x10240500,0x10240100,0x10440900,0x10340900,0x10240500,0x10240500,0x10340500,0x10440500,0x10240100,0x10240100,0x10340500,0x10540500,0x10340500,0x10340100,0x10540500,0x10440500,0x10340100,0x10340100,0x10440100,0x10340100,0x10440101,0x10540100,0x10440100,0x10540101,0x10740100,0x10640101,0x10540100,0x10640500,0x10740101,0x10840103,0x10740101,
    0x10840500,0x10940500,0x10C41100,0x10A40D00,0x10940D00,0x10A41500,0x10941500,0x10740900,0x10840D00,0x10741100,0x10941500,0x10740D00,0x10640D00,0x10740D00,0x10741100,0x10540900,0x10640D00,0x10440500,0x10340900,0x10440900,0x10340900,0x10440900,0x10540D00,0x10240500,0x10541100,0x10340900,0x10140100,0x10240500,0x10440D00,0x10240900,0x10440500,0x10140100,0x10240500,0x10240900,0x10540900,0x10240500,0x10240500,0x10340100,0x10240100,0x10340100,0x10240500,0x10240100,0x10340500,0x10340100,0x10240100,0x10340500,0x10240101,0x10340100,0x10140100,0x10340100,0x10240100,0x10340500,0x10340100,0x10340101,0x10540100,0x10340100,0x10740900,0x10340100,0x10540100,0x10640100,0x10540100,0x10640101,0x10940500,0x10940103,0x10740103,
    0x10840900,0x10B40D00,0x10B40D00,0x10A41100,0x10941100,0x10941100,0x10A41500,0x10941100,0x10640900,0x10640900,0x10740900,0x10640D00,0x10540900,0x10440900,0x10540900,0x10440900,0x10540900,0x10540900,0x10540D00,0x10340900,0x10240500,0x10440500,0x10641100,0x10440900,0x10440D00,0x10340900,0x10140500,0x10440D00,0x10340900,0x10240500,0x10240500,0x10140100,0x10340900,0x10240500,0x10240100,0x10140100,0x10340500,0x10340100,0x10340500,0x10240100,0x10240500,0x10240100,0x10340500,0x10340500,0x10440900,0x10440500,0x10140100,0x10340100,0x10340100,0x10240100,0x10240100,0x10440100,0x10340101,0x10440100,0x10440101,0x10440100,0x10540100,0x10440500,0x10540100,0x10640100,0x10540500,0x10640100,0x10640100,0x10640101,0x10840103,
    0x10740D00,0x10A41100,0x10A41100,0x10840D00,0x10740D00,0x10940D00,0x10841100,0x10741100,0x10A41500,0x10741100,0x10740900,0x10640D00,0x10440500,0x10641100,0x10440500,0x10440D00,0x10440900,0x10540900,0x10440900,0x10440900,0x10440D00,0x10440D00,0x10440900,0x10440900,0x10340500,0x10340D00,0x10340500,0x10240100,0x10240500,0x10440D00,0x10240500,0x10240500,0x10240500,0x10140500,0x10140100,0x10240500,0x10240100,0x10340500,0x10240100,0x10240100,0x10240100,0x10340500,0x10140100,0x10140100,0x10140100,0x10240500,0x10240100,0x10240100,0x10240100,0x10240100,0x10240100,0x10340100,0x10240100,0x10240100,0x10240100,0x10140100,0x10340100,0x10440100,0x10440100,0x10640100,0x10640100,0x10640101,0x10640100,0x10640102,0x10840100,
    0x10B41500,0x10941500,0x10B41500,0x10B41500,0x10840900,0x10841100,0x10940D00,0x10941100,0x10540500,0x10640D00,0x10740D00,0x10640D00,0x10540D00,0x10540D00,0x10440900,0x10440900,0x10340500,0x10540D00,0x10340900,0x10340900,0x10440900,0x10440900,0x10540D00,0x10540D00,0x10340900,0x10240500,0x10240500,0x10440900,0x10440900,0x10440500,0x10340900,0x10340500,0x10240500,0x10240100,0x10240900,0x10340500,0x10340500,0x10140100,0x10140100,0x10340500,0x10340900,0x10340900,0x10240500,0x10240500,0x10240100,0x10240100,0x10240100,0x10140100,0x10240100,0x10340100,0x10540500,0x10240100,0x10240100,0x10240100,0x10440102,0x10440101,0x10440100,0x10340100,0x10440100,0x10640101,0x10440101,0x10440100,0x10440100,0x10540101,0x10840104,
    0x10940900,0x10941100,0x10B41100,0x10A41100,0x10840900,0x10941100,0x10640900,0x10640900,0x10740D00,0x10640900,0x10740900,0x10740D00,0x10540900,0x10540D00,0x10540900,0x10540D00,0x10440D00,0x10540D00,0x10440900,0x10240500,0x10440900,0x10240500,0x10240500,0x10240100,0x10340500,0x10340500,0x10340900,0x10240900,0x10340500,0x10240500,0x10440500,0x10240100,0x10240500,0x10240100,0x10240100,0x10240100,0x10240500,0x10140100,0x10240500,0x10140100,0x10340900,0x10240500,0x10140100,0x10240100,0x10140100,0x10140100,0x10140101,0x10140100,0x10240101,0x10240500,0x10140100,0x10240100,0x10340100,0x10340100,0x10440101,0x10340100,0x10540500,0x10340100,0x10440500,0x10440101,0x10540100,0x10640101,0x10740102,0x10740102,0x10840103,
    0x10B40900,0x10940D00,0x10A40D00,0x10941100,0x10940D00,0x10740D00,0x10A41500,0x10640900,0x10841100,0x10740D00,0x10640900,0x10540100,0x10440900,0x10440D00,0x10540900,0x10440500,0x10440D00,0x10340900,0x10340500,0x10440500,0x10340500,0x10440900,0x10440500,0x10340500,0x10340900,0x10240500,0x10240100,0x10340900,0x10140100,0x10240500,0x10340500,0x10240500,0x10240500,0x10040100,0x10140100,0x10140100,0x10240100,0x10140100,0x10240500,0x10140100,0x10140100,0x10140100,0x100400FF,0x10240500,0x10240100,0x10140100,0x10140100,0x10140100,0x10240100,0x10240100,0x10340100,0x10340100,0x10240100,0x10340100,0x10340500,0x10340101,0x10340100,0x10440100,0x10440100,0x10540101,0x10540101,0x10540101,0x10740102,0x10740103,0x10940103,
    0x10840900,0x10940D00,0x10840D00,0x10A41500,0x10C41900,0x10940D00,0x10740D00,0x10840D00,0x10941100,0x10740D00,0x10540900,0x10640D00,0x10640900,0x10540900,0x10340100,0x10340500,0x10540D00,0x10440500,0x10340900,0x10440900,0x10340500,0x10340100,0x10340500,0x10340900,0x10340500,0x10340500,0x10140100,0x10240500,0x10140100,0x10040100,0x10340500,0x10040100,0x10140100,0x10340500,0x10240500,0x10140100,0x10040100,0x10040100,0x10140100,0x10240100,0x10240500,0x10140100,0x10240900,0x10140100,0x10140100,0x10040100,0x10040100,0x10140100,0x10140100,0x10340100,0x10140100,0x10340100,0x10140100,0x10440500,0x10340100,0x10340500,0x10240100,0x10340100,0x10440100,0x10640100,0x10540102,0x10440100,0x10540101,0x10640102,0x10840104,
    0x10740500,0x10740500,0x10C41500,0x10B41500,0x10840D00,0x10940D00,0x10941500,0x10941100,0x10840D00,0x10741100,0x10640D00,0x10640D00,0x10540900,0x10640D00,0x10340900,0x10340500,0x10440900,0x10540900,0x10540500,0x10140100,0x10240500,0x10140100,0x10240900,0x10340100,0x10340500,0x10240500,0x10240500,0x10340100,0x10340500,0x10340500,0x10140100,0x10040100,0x10140100,0x10240500,0x10240100,0x10040100,0x10040100,0x10040100,0x100400FF,0x10140100,0x10240500,0x10140100,0x10240500,0x10140500,0x10140100,0x10140100,0x10140100,0x10240100,0x10140100,0x10140100,0x10140100,0x10240500,0x10340100,0x10240101,0x10140100,0x10340500,0x10340100,0x10340100,0x10440100,0x10640100,0x10540100,0x10540102,0x10540100,0x10640101,0x10940106,
    0x10940D00,0x10940D00,0x10840D00,0x10B41100,0x10840D00,0x10640D00,0x10740D00,0x10841500,0x10540500,0x10941100,0x10640D00,0x10540900,0x10440900,0x10540900,0x10540900,0x10540900,0x10440900,0x10340100,0x10540D00,0x10340100,0x10240100,0x10240500,0x10340100,0x10340100,0x10240100,0x10240100,0x10040100,0x10340500,0x10340500,0x10240500,0x10240500,0x10340500,0x10140100,0x10140500,0x10040100,0x10040100,0x10140100,0x10040100,0x10140100,0x10140100,0x10040100,0x10140100,0x10140100,0x10040100,0x10140100,0x10140100,0x10140100,0x10140100,0x10240100,0x10240100,0x10240100,0x10340100,0x10440100,0x10240100,0x10340100,0x10240100,0x10340101,0x10340101,0x10540100,0x10540101,0x10540102,0x10440101,0x10640102,0x10640103,0x10840104,
    0x10841100,0x10B41100,0x10840900,0x10A40D00,0x10840D00,0x10640900,0x10740900,0x10741100,0x10640D00,0x10640900,0x10640900,0x10540D00,0x10740D00,0x10640D00,0x10640900,0x10440900,0x10640900,0x10440500,0x10340900,0x10340500,0x10340500,0x10340900,0x10340100,0x10440500,0x10240100,0x10140100,0x10240100,0x10240100,0x10140500,0x10140100,0x10240500,0x10140100,0x10240500,0x10140100,0x10040100,0x10140100,0x10140100,0x10040100,0x10240100,0x10040100,0x10140100,0x10240500,0x10240100,0x10040100,0x10040100,0x10240100,0x10240100,0x10140100,0x10240100,0x10140100,0x10240100,0x10440100,0x10240100,0x10240100,0x10340100,0x10240100,0x10440500,0x10340100,0x10440101,0x10440100,0x10540101,0x10640100,0x10640103,0x10840103,0x10840106,
    0x10640D00,0x10840D00,0x10A41100,0x10A41100,0x10941100,0x10940D00,0x10740D00,0x10740D00,0x10740D00,0x10440500,0x10640D00,0x10741100,0x10640900,0x10440500,0x10640D00,0x10440900,0x10440900,0x10440900,0x10240100,0x10340500,0x10140100,0x10340500,0x10340900,0x10340500,0x10140100,0x10140100,0x10240100,0x10040100,0x10040100,0x10140500,0x10340900,0x10240500,0x10140500,0x10140500,0x10040100,0x10140100,0x10140100,0x10040100,0x10140100,0x10040101,0x10040100,0x10040100,0x10140900,0x10240500,0x10140500,0x10140500,0x10040100,0x10240100,0x10140100,0x10240100,0x10040100,0x10240500,0x10340100,0x10240100,0x10340500,0x10240101,0x10440500,0x10340101,0x10440100,0x10440102,0x10440101,0x10640100,0x10440102,0x10640104,0x10940103,
    0x10840900,0x10940900,0x10840D00,0x10A41100,0x10941500,0x10540900,0x10840D00,0x10640500,0x10540D00,0x10640D00,0x10640D00,0x10640D00,0x10440500,0x10640900,0x10340500,0x10440D00,0x10540900,0x10440900,0x10440500,0x10240500,0x10440500,0x10240100,0x10140100,0x10040100,0x10140100,0x10140100,0x10040100,0x10040100,0x10040100,0x10040100,0x10240500,0x10040100,0x10040100,0x10040100,0x10140100,0x100400FF,0x10040100,0x10040100,0x10040100,0x10140101,0x10040100,0x10240100,0x10140100,0x10240100,0x10140100,0x100400FF,0x10140500,0x10140100,0x10140100,0x10040100,0x10140100,0x10340500,0x10240100,0x10140100,0x10340500,0x10240100,0x10440100,0x10340100,0x10340100,0x10440100,0x10440100,0x10540102,0x10640101,0x10640103,0x10740105,
    0x10840900,0x10840900,0x10A41500,0x10940D00,0x10841100,0x10640D00,0x10740D00,0x10940D00,0x10840D00,0x10640900,0x10440500,0x10740D00,0x10740D00,0x10440100,0x10540900,0x10340100,0x10340500,0x10340500,0x10440900,0x10440900,0x10240500,0x10240500,0x10240100,0x10240100,0x10240500,0x10240100,0x10240900,0x10140100,0x10140100,0x10140500,0x10140100,0x10140100,0x10140100,0x10040500,0x10040101,0x10040100,0x10140500,0x10140100,0x10040100,0x10040100,0x10140100,0x10140100,0x10240500,0x10240500,0x10140100,0x10040100,0x10140100,0x10140100,0x10140100,0x10140100,0x10240100,0x10340100,0x10240100,0x10140100,0x10140100,0x10340100,0x10340102,0x10240100,0x10340100,0x10440100,0x10540100,0x10540101,0x10640101,0x10740103,0x10740105,
    0x10940500,0x10840D00,0x10A41500,0x10740900,0x10941100,0x10740D00,0x10640900,0x10741100,0x10740D00,0x10640500,0x10640900,0x10540900,0x10540D00,0x10540900,0x10340500,0x10340100,0x10440500,0x10440900,0x10140500,0x10140100,0x10240500,0x10240100,0x10140100,0x10140100,0x10140100,0x10140100,0x10240100,0x10140100,0x10140100,0x10040100,0x10140500,0x10040100,0x10140100,0x10140100,0x10040100,0x10140100,0x10040100,0x10040100,0x10040100,0x10140100,0x10140100,0x10140100,0x10140100,0x10040100,0x10040100,0x10140101,0x10140100,0x100400FF,0x10140100,0x10040101,0x10240100,0x10040100,0x10240100,0x10340100,0x10240100,0x10140100,0x10340100,0x10240100,0x10340100,0x10540101,0x10440100,0x10540100,0x10640102,0x10540104,0x10840104,
    0x10740100,0x10740100,0x10740D00,0x10941100,0x10840D00,0x10740D00,0x10741100,0x10740D00,0x10741100,0x10640D00,0x10540900,0x10640900,0x10740D00,0x10540500,0x10440900,0x10240500,0x10440900,0x10240500,0x10240100,0x10240100,0x10240500,0x10340900,0x10240500,0x10240100,0x10140100,0x10140100,0x10140100,0x10140100,0x10240100,0x10140100,0x10040100,0x10140100,0x10040100,0x10240500,0x10040100,0x10140500,0x10140100,0x10040100,0x100400FF,0x10040100,0x10140100,0x10040100,0x10040100,0x10140100,0x10040100,0x10040100,0x10040100,0x10040101,0x10240500,0x10140101,0x10140100,0x10140100,0x10140100,0x10040100,0x10240100,0x10240100,0x10240100,0x10340101,0x10440102,0x10340101,0x10440101,0x10640100,0x10540102,0x10640103,0x10540104,
    0x10740900,0x10B40D00,0x10B41100,0x10840D00,0x10941100,0x10740D00,0x10740900,0x10640900,0x10741100,0x10740D00,0x10440900,0x10540900,0x10340500,0x10440500,0x10340900,0x10340500,0x10440D00,0x10140100,0x10140100,0x10240100,0x10440D00,0x10240500,0x10240100,0x10240100,0x10240101,0x10140100,0x10140100,0x10140100,0x10040100,0x10040100,0x10040100,0x10040100,0x10140100,0x10040100,0x10040100,0x10040100,0x10040100,0x10040100,0x10040100,0x10040100,0x10040100,0x10040100,0x10040100,0x10140100,0x10040100,0x10040100,0x10040100,0x10140100,0x10040100,0x10040100,0x10040100,0x10240500,0x10240500,0x10240500,0x10240100,0x10240100,0x10140100,0x10340100,0x10340101,0x10440100,0x10440100,0x10640103,0x10540100,0x10640103,0x10840105,
    0x10840D00,0x10941100,0x10A40D00,0x10740900,0x10841100,0x10540900,0x10940D00,0x10740D00,0x10640900,0x10540900,0x10440500,0x10340500,0x10540900,0x10440500,0x10440900,0x10440D00,0x10240500,0x10340500,0x10340500,0x10140100,0x10240100,0x10240100,0x10240100,0x10340500,0x10240100,0x10040100,0x10240100,0x10240500,0x10240100,0x10140100,0x10040100,0x10240100,0x10140100,0x10240100,0x10140500,0x10040100,0x10040100,0x10040100,0x10040502,0x10040100,0x10040100,0x10040100,0x10140100,0x10140100,0x10040100,0x10040100,0x10040100,0x10040100,0x10140100,0x10140100,0x10140100,0x10140100,0x10340101,0x10140100,0x10240100,0x10240100,0x10340100,0x10240101,0x10440100,0x10440100,0x10340100,0x10440101,0x10540102,0x10540103,0x10440104,
    0x10941100,0x10840900,0x10840D00,0x10941100,0x10740D00,0x10841100,0x10841100,0x10740D00,0x10640D00,0x10640900,0x10540500,0x10240500,0x10340500,0x10340500,0x10340500,0x10340100,0x10240100,0x10140100,0x10240500,0x10140100,0x10240100,0x10140100,0x10340100,0x10140100,0x10140100,0x10040100,0x10140100,0x10240500,0x10140100,0x10040100,0x10040100,0x100400FF,0x10140100,0x10140100,0x10140100,0x10040100,0x10140100,0x10040100,0x10040101,0x10140101,0x10040100,0x10040100,0x10140100,0x10240900,0x10040100,0x10040100,0x10040100,0x10240100,0x10240500,0x10140100,0x10040100,0x10240100,0x10240100,0x10240100,0x10240100,0x10340100,0x10240100,0x10240101,0x10440100,0x10440102,0x10340100,0x10440101,0x10540101,0x10740103,0x10840106,
    0x10840D00,0x10840D00,0x10940D00,0x10940D00,0x10740D00,0x10440500,0x10841100,0x10640900,0x10440900,0x10540900,0x10440900,0x10340500,0x10340500,0x10440900,0x10340500,0x10340100,0x10340500,0x10140100,0x10340500,0x10240500,0x10040100,0x10240100,0x10240100,0x10140100,0x10140100,0x10240100,0x10040100,0x10040100,0x10140101,0x10140100,0x10040100,0x10140100,0x10040100,0x10040500,0x10040100,0x10140100,0x10140100,0x10040100,0x10040101,0x10040101,0x10040100,0x10040100,0x10040100,0x10140100,0x10240100,0x100400FF,0x10240100,0x10140101,0x10140100,0x10040100,0x10040100,0x10140101,0x10140100,0x10140100,0x10240100,0x10240101,0x10240100,0x10340100,0x10240101,0x10340100,0x10440100,0x10540100,0x10640101,0x10640102,0x10740103,
    0x10841100,0x10940D00,0x10A41100,0x10740D00,0x10941500,0x10941500,0x10640D00,0x10640D00,0x10640900,0x10440500,0x10340100,0x10540500,0x10540500,0x10240500,0x10240500,0x10240500,0x10140100,0x10040500,0x10240500,0x10140100,0x10340500,0x10340100,0x10340500,0x10140100,0x10240100,0x10140100,0x10140100,0x10040100,0x10140100,0x10140101,0x100400FF,0x10040100,0x10040100,0x10140100,0x10040100,0x10040100,0x10040100,0x10040100,0x10040100,0x10040101,0x10140101,0x10040100,0x10140100,0x10040100,0x10040100,0x10040100,0x10140101,0x10040100,0x10040100,0x10040100,0x10140100,0x10040100,0x10140100,0x10140100,0x10240100,0x10240100,0x10140100,0x10240100,0x10340100,0x10440101,0x10440100,0x10440102,0x10640101,0x10640104,0x10640104,
    0x10840D00,0x10841100,0x10941100,0x10741100,0x10640900,0x10740D00,0x10540500,0x10540900,0x10640D00,0x10440500,0x10440900,0x10540900,0x10340900,0x10340500,0x10040100,0x10240500,0x10340500,0x10340500,0x10240100,0x10140100,0x10240100,0x10140100,0x10040100,0x10240500,0x10140100,0x10140100,0x10040100,0x10140100,0x10140100,0x10040100,0x10040100,0x10040100,0x10040102,0x10040100,0x10040100,0x10140100,0x10140100,0x10040100,0x10040100,0x10040100,0x10040100,0x10140100,0x10140500,0x10140100,0x10140101,0x10140100,0x10040100,0x10040100,0x10240500,0x10140100,0x10140100,0x10140100,0x10140102,0x10240100,0x10340500,0x10240100,0x10340101,0x10340101,0x10240100,0x10340100,0x10340100,0x10540100,0x10640101,0x10640103,0x10840104,
    0x10841100,0x10740900,0x10740D00,0x10640900,0x10740900,0x10741100,0x10640900,0x10640900,0x10640900,0x10440500,0x10340900,0x10340100,0x10440900,0x10440500,0x10440900,0x10240500,0x10340500,0x10340100,0x10240100,0x10140100,0x10140100,0x10140100,0x10240100,0x10140500,0x10040100,0x10040100,0x10140100,0x10040100,0x10140100,0x10040101,0x10040101,0x10040100,0x10040100,0x10040100,0x10040100,0x10140100,0x10040100,0x10040100,0x10040101,0x10040100,0x10040100,0x10040100,0x10040100,0x10040100,0x10040100,0x10040100,0x10140100,0x10040100,0x10140500,0x10140500,0x10040100,0x10240500,0x10240100,0x10140100,0x10240100,0x10240101,0x10340100,0x10440500,0x10340102,0x10340100,0x10440100,0x10340100,0x10540103,0x10640103,0x10640104,
    0x10440500,0x10740100,0x10841100,0x10540900,0x10740D00,0x10840D00,0x10540500,0x10640900,0x10640900,0x10440900,0x10540500,0x10340500,0x10540900,0x10240500,0x10240900,0x10340900,0x10440900,0x10240100,0x10140100,0x10240100,0x10140100,0x10240100,0x10140100,0x10240100,0x10140100,0x10040101,0x100400FF,0x10140100,0x10040101,0x10040100,0x10040100,0x10040101,0x10040100,0x10140500,0x10140100,0x10040100,0x10040100,0x10040100,0x10040100,0x10140101,0x10040100,0x10040100,0x10040100,0x100400FF,0x10140100,0x10040101,0x10040100,0x10140100,0x10040500,0x10140100,0x10140100,0x10140101,0x10240100,0x10240100,0x10340100,0x10340100,0x10440100,0x10440101,0x10340100,0x10440101,0x10440100,0x10340100,0x10440100,0x10640103,0x10740106,
    0x10740900,0x10A41500,0x10740900,0x10740D00,0x10640D00,0x10741100,0x10740D00,0x10640900,0x10640D00,0x10540500,0x10540500,0x10340500,0x10340500,0x10240100,0x10340100,0x10340500,0x10340100,0x10240100,0x10240100,0x10240100,0x10240100,0x10140100,0x10140100,0x10040100,0x10140100,0x10240101,0x10240100,0x10140500,0x10040100,0x10040100,0x10040100,0x10140100,0x100400FF,0x10040100,0x10040100,0x10040100,0x10140100,0x10040101,0x10040100,0x10040100,0x100400FF,0x10040100,0x10040100,0x10040100,0x10140100,0x10040100,0x10240900,0x10040101,0x10040100,0x10240101,0x10240100,0x10140100,0x10040100,0x10340100,0x10240100,0x10140100,0x10240100,0x10340100,0x10340101,0x10240101,0x10540100,0x10540101,0x10340101,0x10640105,0x10740107,
    0x10640900,0x10640500,0x10841100,0x10640D00,0x10740D00,0x10740D00,0x10540900,0x10740900,0x10740D00,0x10440900,0x10440900,0x10340500,0x10440900,0x10440900,0x10340100,0x10440900,0x10240500,0x10240500,0x10140100,0x10140100,0x10140100,0x10240100,0x10140100,0x10140100,0x10040101,0x10140100,0x10140100,0x10040101,0x10240100,0x10140100,0x10040101,0x10140100,0x10140100,0x10140100,0x10040101,0x10240100,0x10140100,0x10040100,0x10040100,0x10040100,0x10140100,0x10040100,0x10040100,0x10040100,0x10040100,0x10140100,0x10140100,0x10040100,0x10140500,0x10140100,0x10140100,0x10240500,0x10140100,0x10240101,0x10240101,0x10240101,0x10340100,0x10340101,0x10240100,0x10340101,0x10540101,0x10340100,0x10540102,0x10540103,0x10740105,
    0x10440900,0x10740900,0x10841500,0x10640900,0x10640900,0x10740900,0x10540500,0x10640D00,0x10640900,0x10540500,0x10340100,0x10340500,0x10440900,0x10240100,0x10340100,0x10340500,0x10440500,0x10240100,0x10340100,0x10140100,0x10140100,0x10140100,0x10240100,0x10140100,0x10040100,0x10040101,0x10240101,0x10140100,0x10140101,0x10040100,0x10040100,0x10140100,0x10040100,0x10040100,0x10140100,0x10040100,0x10140100,0x10040100,0x10040102,0x10040101,0x10040100,0x10140100,0x100400FF,0x10040100,0x10040100,0x10040100,0x10040101,0x10040100,0x10040101,0x10140100,0x10040101,0x10140100,0x10240100,0x10040100,0x10240100,0x10340100,0x10140500,0x10240100,0x10440100,0x10240100,0x10340100,0x10540101,0x10540103,0x10740105,0x10840105,
    0x10941100,0x10740500,0x10840D00,0x10740D00,0x10440500,0x10640D00,0x10640900,0x10640D00,0x10440900,0x10440900,0x10440500,0x10540D00,0x10440900,0x10240500,0x10440900,0x10340500,0x10240100,0x10140100,0x10140100,0x10140100,0x10240100,0x10140100,0x10140100,0x10140101,0x10140101,0x10040100,0x10140100,0x10140100,0x10140100,0x10140101,0x10240102,0x10040101,0x10140100,0x10040100,0x10040101,0x10040100,0x10140100,0x10140100,0x10040101,0x100400FF,0x10040100,0x10040100,0x10140100,0x10140100,0x10040100,0x10040100,0x10140100,0x10140100,0x10140100,0x10140100,0x10240100,0x10140100,0x10240101,0x10240100,0x10140100,0x10140100,0x10340100,0x10140100,0x10340101,0x10440102,0x10340100,0x10440102,0x10540102,0x10540103,0x10640105,
    0x10841100,0x10840900,0x10A41100,0x10740900,0x10640D00,0x10740500,0x10540900,0x10440500,0x10440500,0x10540900,0x10640900,0x10440500,0x10240100,0x10440500,0x10440900,0x10240100,0x10140100,0x10240100,0x10340500,0x10040100,0x10040100,0x10240100,0x10140100,0x10240500,0x10140101,0x10140100,0x10440500,0x10240100,0x10140100,0x10140101,0x10140100,0x10040100,0x10040100,0x10040100,0x10140100,0x10040100,0x10040100,0x10040101,0x10040101,0x10040100,0x10040100,0x10140500,0x10140100,0x10140500,0x10040100,0x10040100,0x100400FF,0x10140100,0x10140100,0x10040100,0x10140100,0x10240101,0x10140100,0x10240101,0x10140100,0x10140100,0x10340101,0x10240101,0x10340100,0x10440102,0x10440100,0x10440101,0x10640103,0x10540104,0x10740102,
    0x10440900,0x10840D00,0x10941500,0x10741100,0x10740900,0x10741100,0x10540900,0x10640900,0x10640900,0x10440100,0x10440500,0x10440500,0x10340500,0x10440500,0x10240100,0x10240100,0x10140100,0x10340100,0x10240100,0x10040100,0x10240100,0x10040100,0x10140100,0x10140100,0x10240100,0x10140100,0x10140100,0x10340500,0x10140500,0x10140100,0x10140100,0x10040500,0x10040100,0x10040100,0x10040100,0x10040100,0x10140100,0x10040101,0x10040101,0x10140100,0x10140100,0x10040100,0x10140100,0x10140100,0x10140100,0x10040100,0x10140100,0x10040100,0x10140100,0x10140100,0x10140101,0x10140100,0x10240101,0x10140100,0x10140100,0x10140100,0x10240100,0x10340101,0x10340102,0x10440100,0x10340102,0x10440102,0x10340103,0x10640104,0x10540104,
    0x10740900,0x10740900,0x10840D00,0x10841500,0x10640D00,0x10741100,0x10740D00,0x10740900,0x10440900,0x10540500,0x10540900,0x10540500,0x10440500,0x10440900,0x10340500,0x10240100,0x10240100,0x10140100,0x10240100,0x10240100,0x10140100,0x10140100,0x10240100,0x10140100,0x10140100,0x10040100,0x10140100,0x10240100,0x10140100,0x10140100,0x10140100,0x10040100,0x10140100,0x10040100,0x10040100,0x10140100,0x10140101,0x10040100,0x10140100,0x10040100,0x10140100,0x10140100,0x10240500,0x10040100,0x10040100,0x10140100,0x10040100,0x10140100,0x10240500,0x10040100,0x10140100,0x10140100,0x10140100,0x10240101,0x10240101,0x10340100,0x10340100,0x10340101,0x10440101,0x10340100,0x10340100,0x10540103,0x10540101,0x10740103,0x10940104,
    0x10741100,0x10741100,0x10740900,0x10740D00,0x10740D00,0x10741100,0x10640900,0x10640900,0x10440500,0x10340500,0x10340500,0x10440500,0x10340500,0x10440900,0x10340500,0x10340500,0x10240100,0x10340100,0x10040100,0x10340500,0x10040100,0x10240100,0x10240100,0x10140100,0x10140100,0x10140100,0x10240100,0x10140100,0x10140100,0x10040100,0x10140100,0x10140100,0x10140100,0x10140100,0x10140100,0x10140100,0x10040100,0x10140100,0x10040100,0x10040100,0x10040100,0x10140100,0x10040100,0x10040500,0x10040100,0x10240100,0x10140100,0x10040100,0x10140101,0x10140100,0x10140100,0x10240100,0x10240100,0x10140100,0x10140100,0x10340100,0x10240101,0x10340100,0x10240101,0x10240102,0x10440100,0x10440102,0x10540102,0x10640104,0x10640106,
    0x10840D00,0x10A41100,0x10741100,0x10741100,0x10740D00,0x10840900,0x10640900,0x10440500,0x10640500,0x10340500,0x10540500,0x10440500,0x10340500,0x10440900,0x10340100,0x10240100,0x10340500,0x10240100,0x10040100,0x10240100,0x10040100,0x10240100,0x10140100,0x10140100,0x10140100,0x10240100,0x10140100,0x10140100,0x10140102,0x10140100,0x10140100,0x10140100,0x10040100,0x100400FF,0x10140100,0x10040100,0x10040100,0x10040100,0x10140100,0x10040100,0x10040100,0x10040100,0x10140100,0x10040100,0x10040100,0x10140100,0x10040100,0x10040100,0x10140100,0x10040100,0x10040100,0x10040100,0x10040100,0x10140100,0x10340101,0x10140100,0x10140100,0x10240100,0x10340100,0x10340101,0x10340102,0x10540103,0x10540102,0x10640105,0x10540105,
    0x10840D00,0x10640900,0x10841100,0x10740900,0x10740900,0x10540900,0x10640D00,0x10440500,0x10640900,0x10440900,0x10440100,0x10240500,0x10340500,0x10340900,0x10340100,0x10340100,0x10340100,0x10140100,0x10240100,0x10240100,0x10340100,0x10140100,0x10240100,0x10140100,0x10140101,0x10240100,0x10040100,0x10040100,0x10040100,0x10240500,0x10140100,0x10140100,0x10040100,0x10040100,0x10040100,0x10040100,0x10140100,0x10040100,0x10040100,0x10140100,0x10040100,0x10140500,0x10040100,0x10140100,0x10140101,0x10040100,0x10140500,0x10240500,0x10040100,0x10240100,0x10140100,0x10140100,0x10240100,0x10140101,0x10240100,0x10040100,0x10340100,0x10340100,0x10240100,0x10240100,0x10340100,0x10540100,0x10440103,0x10540104,0x10540105,
    0x10841100,0x10840D00,0x10841100,0x10740D00,0x10440900,0x10540D00,0x10440900,0x10540500,0x10640D00,0x10540D00,0x10440500,0x10340500,0x10440500,0x10340100,0x10340500,0x10440100,0x10240100,0x10240100,0x10240100,0x10240100,0x10140100,0x10140100,0x10140100,0x10140100,0x10040100,0x10140100,0x10040100,0x10140100,0x10040101,0x10240100,0x10140101,0x10040100,0x10140100,0x10040100,0x10040100,0x10040100,0x10240100,0x10040101,0x10140102,0x10240500,0x10140100,0x10140100,0x10140100,0x10040100,0x10040100,0x10140101,0x100400FF,0x10240100,0x10140101,0x10040100,0x10140100,0x10140100,0x10140100,0x10140100,0x10240102,0x10140100,0x10240100,0x10240101,0x10440102,0x10340101,0x10440102,0x10440100,0x10540101,0x10640102,0x10840107,
    0x10540100,0x10740D00,0x10840D00,0x10740D00,0x10640D00,0x10640900,0x10741100,0x10440500,0x10640900,0x10540900,0x10340500,0x10440500,0x10440900,0x10440900,0x10340500,0x10140100,0x10140500,0x10240100,0x10240100,0x10340100,0x10140100,0x10240100,0x10240100,0x10140100,0x10240100,0x10140100,0x10040100,0x10040100,0x10140100,0x10140100,0x10240100,0x10040100,0x10140100,0x10040101,0x10040100,0x10040100,0x10040100,0x10040100,0x10140100,0x10040102,0x10140100,0x10140100,0x10140100,0x10040100,0x10240100,0x10240500,0x10040500,0x10040100,0x10140100,0x10040100,0x10040100,0x10140100,0x10140101,0x10240100,0x10040100,0x10140100,0x10140100,0x10240101,0x10440100,0x10340103,0x10340100,0x10440100,0x10340103,0x10640105,0x10640106,
    0x10740900,0x10940900,0x10841100,0x10840D00,0x10841100,0x10740D00,0x10841100,0x10640500,0x10540500,0x10540900,0x10440500,0x10240100,0x10340500,0x10340100,0x10240500,0x10240500,0x10140100,0x10340500,0x10340100,0x10240100,0x10240100,0x10240100,0x10140100,0x10240101,0x10240101,0x10140100,0x10140100,0x10140101,0x10140100,0x10140100,0x10140100,0x10040100,0x10040100,0x10040100,0x10040100,0x10040100,0x10040100,0x10040100,0x10040100,0x10140100,0x10040100,0x10040100,0x100400FF,0x10040100,0x10040101,0x10140500,0x10040100,0x10040100,0x10140100,0x10140100,0x10140100,0x10140100,0x10140100,0x10140100,0x10140100,0x10140100,0x10240101,0x10240100,0x10340100,0x10340100,0x10340101,0x10340101,0x10640103,0x10640105,0x10740107,
    0x10740D00,0x10841100,0x10941100,0x10740500,0x10740900,0x10840900,0x10540900,0x10640900,0x10440500,0x10540500,0x10340100,0x10540900,0x10340100,0x10340100,0x10240100,0x10340100,0x10040100,0x10240100,0x10240100,0x10140100,0x10240100,0x10340100,0x10240100,0x10340100,0x10240100,0x10140100,0x10140100,0x10240100,0x10140101,0x10040101,0x10140100,0x10140500,0x10040100,0x10140100,0x10140100,0x10040100,0x10140100,0x10040100,0x100400FF,0x10140100,0x10240100,0x10040100,0x10040500,0x10140100,0x10040100,0x10140100,0x10140100,0x10040100,0x10040500,0x10040101,0x10140101,0x10040101,0x10040100,0x10140101,0x10140100,0x10140100,0x10140100,0x10140100,0x10340102,0x10340101,0x10440100,0x10440104,0x10440102,0x10440102,0x10740104,
    0x10740D00,0x10940D00,0x10740900,0x10840900,0x10741100,0x10740900,0x10540900,0x10540900,0x10640900,0x10340500,0x10440100,0x10540900,0x10340100,0x10440100,0x10340500,0x10340500,0x10240500,0x10140100,0x10240100,0x10140100,0x10240100,0x10140100,0x10240100,0x10240101,0x10140100,0x10140100,0x10240100,0x10140102,0x10040102,0x10140100,0x10140100,0x10140101,0x10040101,0x10040100,0x10140100,0x10140100,0x10040100,0x10140100,0x10040100,0x10040100,0x10040100,0x10040100,0x10040100,0x10140100,0x10140500,0x10040100,0x10140100,0x10040100,0x10040100,0x10140100,0x10040100,0x10140100,0x10040100,0x10240100,0x10240100,0x10040100,0x10340100,0x10240102,0x10240101,0x10340101,0x10340102,0x10340100,0x10340101,0x10640105,0x10740108,
    0x10840900,0x10640900,0x10740D00,0x10841100,0x10740D00,0x10740900,0x10840D00,0x10640900,0x10740D00,0x10540900,0x10540500,0x10340100,0x10340100,0x10540900,0x10240500,0x10340100,0x10240100,0x10240100,0x10140100,0x10140100,0x10240101,0x10140100,0x10140100,0x10140100,0x10140100,0x10140100,0x10040101,0x10040100,0x10040100,0x10040100,0x10240101,0x10140100,0x10040101,0x10040100,0x10040100,0x10040100,0x10140100,0x10040100,0x10040102,0x10140500,0x100400FF,0x10040100,0x10240100,0x10140100,0x10140100,0x10140100,0x10040100,0x10040100,0x10140100,0x10040100,0x10140100,0x10240101,0x10240100,0x10140101,0x10140100,0x10040100,0x10340100,0x10340100,0x10340101,0x10440101,0x10340102,0x10440102,0x10540101,0x10640105,0x10540106,
    0x10640500,0x10940D00,0x10A41100,0x10841100,0x10840900,0x10640900,0x10741100,0x10640D00,0x10640D00,0x10440900,0x10540900,0x10440900,0x10340100,0x10240100,0x10340500,0x10340500,0x10340500,0x10140100,0x10240100,0x10140100,0x10240100,0x10240100,0x10240101,0x10140101,0x10040100,0x10140100,0x10140100,0x10240100,0x10040101,0x10140100,0x10040100,0x10240500,0x10140100,0x10140100,0x10040100,0x10040100,0x10040100,0x100400FF,0x10040100,0x10140101,0x10140100,0x10040100,0x10040101,0x10040100,0x10040100,0x10040101,0x100400FF,0x10140100,0x10140100,0x10040100,0x10040100,0x10140100,0x10140100,0x10040100,0x10340100,0x10240100,0x10340101,0x10340100,0x10340101,0x10340101,0x10440102,0x10440101,0x10440102,0x10540104,0x10740106,
    0x10840900,0x10740900,0x10941100,0x10840900,0x10640500,0x10840900,0x10440500,0x10640500,0x10540100,0x10540900,0x10440100,0x10340500,0x10440500,0x10440100,0x10240100,0x10440500,0x10140100,0x10240100,0x10140100,0x10140100,0x10240102,0x10140101,0x10140100,0x10140101,0x10140100,0x10140100,0x10040101,0x10240100,0x10140100,0x10040100,0x10140100,0x10040101,0x10140500,0x10040100,0x10140100,0x10140100,0x10140500,0x10140500,0x10040100,0x10040100,0x10040100,0x10040100,0x10140100,0x10040101,0x10240500,0x10040100,0x10040100,0x10040100,0x10140100,0x10040100,0x10140101,0x10140101,0x10140100,0x10140100,0x10140101,0x10240100,0x10240101,0x10240100,0x10440101,0x10340101,0x10440102,0x10440102,0x10340103,0x10540105,0x10A40109,
    0x10740500,0x10840D00,0x10940900,0x10840D00,0x10740D00,0x10740900,0x10540900,0x10740900,0x10440100,0x10440500,0x10340100,0x10340500,0x10440100,0x10440500,0x10340500,0x10240100,0x10240500,0x10340100,0x10240100,0x10140500,0x10140100,0x10140100,0x10240100,0x10240101,0x10240100,0x10140100,0x10240100,0x10140101,0x10140100,0x10040100,0x10140102,0x10040100,0x10040100,0x10040100,0x10040100,0x10140100,0x10140100,0x10140100,0x10140101,0x10040100,0x10040101,0x10040100,0x10140100,0x100400FF,0x10140100,0x10040100,0x10040100,0x10140101,0x10040101,0x10040100,0x10040100,0x10140101,0x10140100,0x10140100,0x10140100,0x10140100,0x10240100,0x10340100,0x10140101,0x10340101,0x10440101,0x10440103,0x10340102,0x10740104,0x10640106,
    0x10A40D00,0x10840900,0x10A41500,0x10941100,0x10740500,0x10640D00,0x10640D00,0x10640D00,0x10540500,0x10540500,0x10540900,0x10440500,0x10440100,0x10440500,0x10340500,0x10340500,0x10540900,0x10340500,0x10240100,0x10140100,0x10240100,0x10140100,0x10140100,0x10140100,0x10240100,0x10240100,0x10140100,0x10140101,0x10140100,0x10040101,0x10240100,0x10240101,0x10140100,0x10040100,0x10040100,0x10140101,0x10240500,0x10040100,0x10040101,0x10040100,0x10140100,0x10040100,0x10140100,0x10140100,0x10040100,0x10140101,0x10140100,0x10040100,0x10140500,0x10040100,0x10140500,0x10040100,0x10140100,0x10140101,0x10140100,0x10040100,0x10340102,0x10340101,0x10240101,0x10340101,0x10340100,0x10440101,0x10540102,0x10740106,0x10640105,
    0x10840D00,0x10940900,0x10840900,0x10941100,0x10740D00,0x10740D00,0x10640900,0x10740D00,0x10640900,0x10240100,0x10540900,0x10340100,0x10340500,0x10440900,0x10340100,0x10340100,0x10240100,0x10240100,0x10040100,0x10040100,0x10140101,0x10240100,0x10340101,0x10140100,0x10240500,0x10140100,0x10140100,0x10040100,0x10040101,0x10140100,0x10040100,0x10140100,0x10040100,0x10140100,0x10140100,0x10040100,0x10140100,0x10040101,0x10040100,0x10040100,0x10040100,0x10040100,0x10040101,0x10040100,0x10140100,0x10040100,0x10040100,0x10040100,0x10040500,0x10040100,0x10340100,0x10240100,0x10240100,0x10040100,0x10040100,0x10140101,0x10240101,0x10240100,0x10340101,0x10440101,0x10340102,0x10540102,0x10540103,0x10640106,0x10940106,
    0x10740900,0x10940D00,0x10740900,0x10841100,0x10740900,0x10740D00,0x10740900,0x10741100,0x10640D00,0x10640900,0x10440500,0x10440100,0x10440100,0x10440900,0x10340100,0x10340500,0x10340100,0x10240100,0x10340500,0x10240100,0x10240100,0x10340500,0x10240100,0x10040100,0x10240100,0x10140100,0x10140101,0x10140100,0x10040100,0x10140100,0x10040100,0x10140100,0x10040100,0x10040100,0x10140100,0x10140500,0x10040100,0x10040100,0x10040100,0x10140100,0x10140100,0x10140500,0x10140100,0x10040101,0x10040500,0x10040100,0x10040100,0x10040100,0x10140100,0x10140100,0x10140500,0x10140100,0x10140100,0x10240100,0x10340100,0x10140101,0x10240100,0x10240102,0x10240101,0x10440102,0x10440102,0x10440102,0x10540103,0x10640105,0x10640106,
    0x10940900,0x10940D00,0x10840500,0x10840D00,0x10640500,0x10640900,0x10740D00,0x10440500,0x10640D00,0x10440100,0x10440500,0x10540900,0x10540100,0x10440500,0x10340500,0x10240100,0x10440500,0x10240500,0x10140100,0x10140100,0x10240100,0x10340500,0x10140100,0x10240100,0x10140100,0x10140100,0x10040100,0x10040100,0x10040100,0x10140100,0x10040101,0x10040100,0x10240500,0x10140100,0x10040100,0x10040100,0x10140100,0x10040100,0x10040100,0x100400FF,0x10040100,0x100400FF,0x10040101,0x10140100,0x10040100,0x10040100,0x10140100,0x10040100,0x10240101,0x10140100,0x10140100,0x10140100,0x10140100,0x10140100,0x10240101,0x10240100,0x10240100,0x10240101,0x10340102,0x10340101,0x10440100,0x10540100,0x10340104,0x10540102,0x10640103,
    0x10740D00,0x10740500,0x10940900,0x10740900,0x10940D00,0x10740900,0x10640900,0x10740D00,0x10640900,0x10640D00,0x10440500,0x10340100,0x10240100,0x10440100,0x10340500,0x10240500,0x10240500,0x10440500,0x10240500,0x10240100,0x10140100,0x10140100,0x10340100,0x10340100,0x10140100,0x10240500,0x10040100,0x10040100,0x10040101,0x10140100,0x10240500,0x10140100,0x10040100,0x10140100,0x10040500,0x10240100,0x10140500,0x10140100,0x10040100,0x10040100,0x10040100,0x10240100,0x10040100,0x10140100,0x10140100,0x10040100,0x10040100,0x10140100,0x10040100,0x10140100,0x10240100,0x10140100,0x10240100,0x10140100,0x10140100,0x10340102,0x10240101,0x10040101,0x10340100,0x10440101,0x10440101,0x10440103,0x10440102,0x10640103,0x10740104,
    0x10740900,0x10740900,0x10940900,0x10940D00,0x10740D00,0x10540D00,0x10540500,0x10640500,0x10640900,0x10640900,0x10440900,0x10440500,0x10640900,0x10540900,0x10340100,0x10540D00,0x10240100,0x10340500,0x10140100,0x10140100,0x10140100,0x10240500,0x10140500,0x10240101,0x10140100,0x10140100,0x10040100,0x10140100,0x10040100,0x10140100,0x10040100,0x10140100,0x10040100,0x10140100,0x10140500,0x10140900,0x10140500,0x10140101,0x10240900,0x10040100,0x10140100,0x10040500,0x10040100,0x10040100,0x10040100,0x10040100,0x10140100,0x10140101,0x10140100,0x10140100,0x10240100,0x10140100,0x10140100,0x10140100,0x10240100,0x10240100,0x10340101,0x10340100,0x10340100,0x10340100,0x10340100,0x10440101,0x10540102,0x10840104,0x10740103,
    0x10740D00,0x10740500,0x10A41900,0x10841100,0x10740900,0x10941100,0x10640900,0x10640900,0x10740D00,0x10440500,0x10540900,0x10440500,0x10340100,0x10340100,0x10440500,0x10540900,0x10240100,0x10240500,0x10240500,0x10240100,0x10140100,0x10140100,0x10140500,0x10040100,0x10040100,0x10240500,0x10040100,0x10140100,0x10040100,0x10040500,0x10140500,0x10140500,0x10140100,0x10040100,0x10140500,0x10040100,0x10040100,0x10240500,0x10040100,0x10040100,0x10040100,0x10240500,0x10140100,0x10040100,0x10040100,0x10040100,0x10140100,0x10140500,0x10040100,0x10140100,0x10040100,0x10140100,0x10140100,0x10240100,0x10240100,0x10340100,0x10140100,0x10340100,0x10240101,0x10340101,0x10540102,0x10540100,0x10440100,0x10640103,0x10940107,
    0x10640100,0x10740500,0x10840900,0x10740900,0x10840D00,0x10640900,0x10640900,0x10841100,0x10640900,0x10740900,0x10540500,0x10440500,0x10640900,0x10540900,0x10440900,0x10340500,0x10340100,0x10240100,0x10340500,0x10240500,0x10340500,0x10040100,0x10140100,0x10140100,0x10240100,0x10040100,0x10040100,0x10140100,0x10140100,0x10140500,0x10040100,0x10140100,0x10140500,0x10040100,0x10040100,0x10040100,0x10040100,0x10040500,0x10040100,0x10040100,0x10240500,0x10140500,0x10140101,0x10040100,0x10040100,0x10040100,0x10040100,0x10040100,0x10040100,0x10140100,0x10240500,0x10140100,0x10140100,0x10140100,0x10140100,0x10240101,0x10240100,0x10240100,0x10340101,0x10340100,0x10440101,0x10540102,0x10440100,0x10640102,0x10640105,
    0x10540500,0x10840500,0x10740900,0x10740900,0x10A40D00,0x10941100,0x10740D00,0x10840D00,0x10540500,0x10540D00,0x10640D00,0x10540900,0x10540D00,0x10540D00,0x10440500,0x10240100,0x10340500,0x10140100,0x10240500,0x10340500,0x10340500,0x10140100,0x10340500,0x10140100,0x10340900,0x10040100,0x10140100,0x10140100,0x10040100,0x10140500,0x10040100,0x10040100,0x10140100,0x10240500,0x10140500,0x10140100,0x10040100,0x10240500,0x10340900,0x10040100,0x10140900,0x10140100,0x10140500,0x10240500,0x10140100,0x10240500,0x10240500,0x10040100,0x10140100,0x10240100,0x10340500,0x10240500,0x10140500,0x10240100,0x10240100,0x10140100,0x10440100,0x10340101,0x10340100,0x10440100,0x10540101,0x10440100,0x10640102,0x10640103,0x10740105,
    0x10841500,0x10740D00,0x10840900,0x10740D00,0x10840D00,0x10941100,0x10640900,0x10640D00,0x10540900,0x10440D00,0x10741100,0x10540D00,0x10440900,0x10540D00,0x10340900,0x10440900,0x10540900,0x10540900,0x10440D00,0x10240500,0x10040500,0x10240500,0x10240500,0x10140500,0x10040500,0x10140100,0x100400FF,0x10140500,0x10240500,0x10240900,0x10140500,0x10140900,0x10140500,0x10140100,0x10140100,0x10040100,0x10040500,0x10240500,0x10040100,0x10140100,0x10140500,0x10040100,0x10140100,0x10140100,0x10040500,0x10040100,0x10140500,0x10040100,0x10240100,0x10040100,0x10140100,0x10140100,0x10240100,0x10140100,0x10240500,0x10140100,0x10240101,0x10340500,0x10340100,0x10340100,0x10340100,0x10640102,0x10540102,0x10740103,0x10740102,
    0x10741100,0x10941500,0x10740900,0x10841100,0x10941500,0x10941500,0x10741100,0x10641100,0x10540D00,0x10741500,0x10641100,0x10641100,0x10641100,0x10440D00,0x10440900,0x10540D00,0x10340500,0x10140500,0x10240900,0x10240900,0x10340D00,0x10140500,0x10040500,0x10040500,0x10040500,0x10140500,0x10140500,0x10040500,0x10040900,0x10040500,0x10140900,0x10240900,0x10140900,0x10040500,0x10140900,0x10040500,0x10040500,0x10040500,0x10040501,0x100408FF,0x10040500,0x10140900,0x10040D00,0x10040900,0x10040500,0x10040500,0x10140500,0x10040900,0x10140900,0x10140500,0x10140500,0x10040100,0x100400FF,0x10140500,0x10440900,0x10140500,0x10340500,0x10340100,0x10340101,0x10340100,0x10340102,0x10640102,0x10440101,0x10640102,0x10640103,
    0x10A41100,0x10B41900,0x10B41900,0x10D41900,0x10C42100,0x10C41D00,0x10941500,0x10A41900,0x10941900,0x10741100,0x10941500,0x10841900,0x10A41900,0x10641500,0x10841500,0x10541500,0x10641500,0x10541100,0x10641500,0x10541100,0x10341100,0x10540D00,0x10341100,0x10541100,0x10441100,0x10140D00,0x10340D00,0x10441100,0x10340D00,0x10341100,0x10440D00,0x10441100,0x10241100,0x10441100,0x10441500,0x10341100,0x10240D00,0x10341100,0x10341100,0x10341100,0x10240D00,0x10340900,0x10441100,0x10440D00,0x10440D00,0x10240900,0x10340D00,0x10541100,0x10240D00,0x10240900,0x10340900,0x10240500,0x10440D00,0x10440900,0x10340500,0x10440D00,0x10340900,0x10440500,0x10440500,0x10440100,0x10440100,0x10540100,0x10540100,0x10740103,0x10540102,
    0x11142100,0x11242900,0x11342D00,0x11443100,0x11442D00,0x11342900,0x11543100,0x10E42500,0x11142900,0x10F42500,0x11042D00,0x11042D00,0x10F42500,0x11142900,0x10D41900,0x10E42500,0x10B42100,0x10E42500,0x11042500,0x10941500,0x10B41D00,0x10E42500,0x10D42500,0x10E42100,0x10C42100,0x10C42100,0x10B41D00,0x10B41D00,0x10D42100,0x10A41D00,0x10C42500,0x10941D00,0x10C42500,0x10A41D00,0x10C42500,0x10C42100,0x10941500,0x10A41900,0x10941D00,0x10841900,0x10C42500,0x10B41D00,0x10C41D00,0x10D42500,0x10B41900,0x10C42100,0x10B41D00,0x10E42500,0x10C42D00,0x10941500,0x10C41900,0x10B41900,0x10A41500,0x10D41900,0x10B41900,0x10B41500,0x10C41900,0x10A41100,0x10B41100,0x10B41100,0x10A40D00,0x10940500,0x10840100,0x10840100,0x10540100
};

////////////////////
// SIE-DPC
////////////////////
static IQS_DPC dpc = {
	_50F00_PERCENT,
	{
		// (x, y)
		// sample (3, 2), (10, 2), (5, 3). (8, 3), (20, 3), (3, 4) --> sort by y index
		// 0x00038002, 0x8003000A, 0x00080005, 0x80040014, 0xffff0003
		// 2byte x, 2byte y; bit15 = 1 means y, bit15 = 0 means x, 0xffff means EOF
		// y cannot be 0 !!!
		0x0000ffff,
	}
};

static H26XEncTnr tnr_param_off =
{   0,                  //enable temporal noise reduction (0: disable, 1:enable)
    0,                  //TNR Mode as OSD enabled: fully TNR (0: fully TNR, 1: P2P only, 2: no TNR)
    1,                  //enable mctf/p2p pixel blending (0: disable, 1:enable)
    3,                  //P2P SAD Mode (0: pixel, 3: SAD + pixel)
    2,                  //MCTF SAD Mode (0: pixel, 1: b8x8, 2: b16x16, 3: auto)
    0,                  //MCTF Bias Mode (0: disable, 1: b8x8, 2: b16x16, 3: auto)
    {3, 7, 12},         //TNR_P2P_SAD_THRESHOLD (0~255)
    {8, 6, 2},          //TNR_P2P_WEIGHT (0~16)
    12,                 //TNR_P2P_BORDER_CHECK_THRES (0~255)
    4,                  //TNR_P2P_BORDER_CHECK_SCALE (0~7)
    0,                  //TNR_P2P_INPUT (0: original source, 1: mctf result)
    16,                 //TNR_P2P_MCTF_MOTION_THRESHOLD (0~255)
    {1, 1, 1, 1},       //TNR_P2P_MCTF_MOTION_WEIGHT (0~3)
    //P2P TNR Config----------------------------------------------------------------------------
    {3, 2, 3},          //TNR_P2P_REF_Y_WEIGHT (0~7)
    {3, 2, 3},          //TNR_P2P_REF_C_WEIGHT (0~7)
    {6, 10, 16},        //TNR_P2P_RANGE_Y_THRESHOLD (0~255)
    {0, 2, 3},          //TNR_P2P_RANGE_C_THRESHOLD (0~255)
    2,                  //TNR_P2P_CLAMP_Y_THRESHOLD (0~255)
    1,                  //TNR_P2P_CLAMP_Y_LSR (0~7)
    2,                  //TNR_P2P_CLAMP_C_THRESHOLD (0~255)
    1,                  //TNR_P2P_CLAMP_C_LSR (0~7)
    //MCTF TNR Config----------------------------------------------------------------------------
    {3, 2, 3},          //TNR_MCTF_REF_Y_WEIGHT (0~7)
    {3, 2, 3},          //TNR_MCTF_REF_C_WEIGHT (0~7)
    {5, 6, 7},          //TNR_MCTF_RANGE_Y_THRESHOLD (0~255)
    {0, 2, 3},          //TNR_MCTF_RANGE_C_THRESHOLD (0~255)
    2,                  //TNR_MCTF_CLAMP_Y_THRESHOLD (0~255)
    1,                  //TNR_MCTF_CLAMP_Y_LSR (0~7)
    2,                  //TNR_MCTF_CLAMP_C_THRESHOLD (0~255)
    1,                  //TNR_MCTF_CLAMP_C_LSR (0~7)
    //ADDITIONAL TNR Config---------------------------------------------------------------------
	0,                  //NR3D_CUR_MOTION_SAD_TH
	4,                  //NR3D_CUR_MOTION_RAT_TH
	{255, 255},         //NR3D_CUR_MOTION_TWR_P2P_TH
	{255, 255},         //NR3D_REF_MOTION_TWR_P2P_TH
	{255, 255},         //NR3D_CUR_MOTION_TWR_MCTF_TH
	{255, 255},         //NR3D_REF_MOTION_TWR_MCTF_TH
	{1, 2, 1},          //NR3D_MCTF_REF_Y_WEIGHT
	{1, 2, 1},          //NR3D_MCTF_REF_C_WEIGHT
	{1, 2, 1},          //NR3D_P2P_REF_Y_WEIGHT
	{1, 2, 1},          //NR3D_P2P_REF_C_WEIGHT
	3,                  //NR3D_P2P_INPUT_WEIGHT
	255,                //NR3D_REF_MOTION_THRESH
	{255, 255},         //NR3D_SAD_TWR_P2P_TH
	{255, 255},         //NR3D_SAD_TWR_MCTF_TH
    0,                  //enable I frame weight refresh (0: disable, 1:enable)
};

static H26XEncTnr tnr_param_lv1 = //ISO100
{   1,          //enable temporal noise reduction (0: disalbe, 1:enable)
    0,          //TNR Mode as OSD enabled: fully TNR (0: fully TNR, 1: P2P only, 2: no TNR)
    1,          //enable mctf/p2p pixel blending (0: disalbe, 1:enable)
    3,          //P2P SAD Mode (0: pixel, 3: SAD + pixel)
    2,          //MCTF SAD Mode (0: pixel, 1: b8x8, 2: b16x16, 3: auto)
    0,          //MCTF Bias Mode (0: disable, 1: b8x8, 2: b16x16, 3: auto)
    {6, 10, 12},        //TNR_P2P_SAD_THRESHOLD (0~255)
    {16, 8, 0},        //TNR_P2P_WEIGHT (0~16)
    5,          //TNR_P2P_BORDER_CHECK_THRES (0~255)
    4,          //TNR_P2P_BORDER_CHECK_SCALE (0~7)
    0,          //TNR_P2P_INPUT (0: original source, 1: mctf result)
    10,          //TNR_P2P_MCTF_MOTION_THRESHOLD (0~255)
    {1, 1, 1, 1},        //TNR_P2P_MCTF_MOTION_WEIGHT (0~3)
    //P2P TNR Config----------------------------------------------------------------------------
    {2, 4, 2},        //TNR_P2P_REF_Y_WEIGHT (0~7)
    {1, 3, 1},        //TNR_P2P_REF_C_WEIGHT (0~7)
    {8, 16, 24},        //TNR_P2P_RANGE_Y_THRESHOLD (0~255)
    {8, 12, 20},        //TNR_P2P_RANGE_C_THRESHOLD (0~255)
    0,          //TNR_P2P_CLAMP_Y_THRESHOLD (0~255)
    1,          //TNR_P2P_CLAMP_Y_LSR (0~7)
    0,          //TNR_P2P_CLAMP_C_THRESHOLD (0~255)
    1,          //TNR_P2P_CLAMP_C_LSR (0~7)
    //MCTF TNR Config----------------------------------------------------------------------------
    {1, 3, 1},        //TNR_MCTF_REF_Y_WEIGHT (0~7)
    {1, 5, 1},        //TNR_MCTF_REF_C_WEIGHT (0~7)
    {8, 12, 18},        //TNR_MCTF_RANGE_Y_THRESHOLD (0~255)
    {8, 12, 20},        //TNR_MCTF_RANGE_C_THRESHOLD (0~255)
    0,          //TNR_MCTF_CLAMP_Y_THRESHOLD (0~255)
    0,          //TNR_MCTF_CLAMP_Y_LSR (0~7)
    0,          //TNR_MCTF_CLAMP_C_THRESHOLD (0~255)
    0,          //TNR_MCTF_CLAMP_C_LSR (0~7)
    //ADDITIONAL TNR Config---------------------------------------------------------------------
    0,          //NR3D_CUR_MOTION_SAD_TH
    4,          //NR3D_CUR_MOTION_RAT_TH
    {255, 255},        //NR3D_CUR_MOTION_TWR_P2P_TH
    {255, 255},        //NR3D_REF_MOTION_TWR_P2P_TH
    {255, 255},        //NR3D_CUR_MOTION_TWR_MCTF_TH
    {255, 255},        //NR3D_REF_MOTION_TWR_MCTF_TH
    {1, 2, 1},        //NR3D_MCTF_REF_Y_WEIGHT
    {1, 2, 1},        //NR3D_MCTF_REF_C_WEIGHT
    {1, 2, 1},        //NR3D_P2P_REF_Y_WEIGHT
    {1, 2, 1},        //NR3D_P2P_REF_C_WEIGHT
    3,          //NR3D_P2P_INPUT_WEIGHT
    255,          //NR3D_REF_MOTION_THRESH
    {255, 255},        //NR3D_SAD_TWR_P2P_TH
    {255, 255},         //NR3D_SAD_TWR_MCTF_TH
    0           //enable I frame weight refresh (0: disable, 1:enable)
};

static H26XEncTnr tnr_param_lv2 = //ISO200
{   1,          //enable temporal noise reduction (0: disalbe, 1:enable)
    0,          //TNR Mode as OSD enabled: fully TNR (0: fully TNR, 1: P2P only, 2: no TNR)
    0,          //enable mctf/p2p pixel blending (0: disalbe, 1:enable)
    0,          //P2P SAD Mode (0: pixel, 3: SAD + pixel)
    0,          //MCTF SAD Mode (0: pixel, 1: b8x8, 2: b16x16, 3: auto)
    0,          //MCTF Bias Mode (0: disable, 1: b8x8, 2: b16x16, 3: auto)
    {6, 10, 12},        //TNR_P2P_SAD_THRESHOLD (0~255)
    {16, 8, 0},        //TNR_P2P_WEIGHT (0~16)
    5,          //TNR_P2P_BORDER_CHECK_THRES (0~255)
    4,          //TNR_P2P_BORDER_CHECK_SCALE (0~7)
    0,          //TNR_P2P_INPUT (0: original source, 1: mctf result)
    10,          //TNR_P2P_MCTF_MOTION_THRESHOLD (0~255)
    {1, 1, 1, 1},        //TNR_P2P_MCTF_MOTION_WEIGHT (0~3)
    //P2P TNR Config----------------------------------------------------------------------------
    {2, 3, 2},        //TNR_P2P_REF_Y_WEIGHT (0~7)
    {1, 3, 1},        //TNR_P2P_REF_C_WEIGHT (0~7)
    {12, 24, 30},        //TNR_P2P_RANGE_Y_THRESHOLD (0~255)
    {8, 15, 20},        //TNR_P2P_RANGE_C_THRESHOLD (0~255)
    1,          //TNR_P2P_CLAMP_Y_THRESHOLD (0~255)
    2,          //TNR_P2P_CLAMP_Y_THRESHOLD (0~255)
    1,          //TNR_P2P_CLAMP_C_THRESHOLD (0~255)
    2,          //TNR_P2P_CLAMP_C_THRESHOLD (0~255)
    //MCTF TNR Config----------------------------------------------------------------------------
    {1, 4, 2},        //TNR_MCTF_REF_Y_WEIGHT (0~7)
    {1, 4, 1},        //TNR_MCTF_REF_C_WEIGHT (0~7)
    {8, 15, 30},        //TNR_MCTF_RANGE_Y_THRESHOLD (0~255)
    {8, 15, 20},        //TNR_MCTF_RANGE_C_THRESHOLD (0~255)
    1,          //TNR_MCTF_CLAMP_Y_THRESHOLD (0~255)
    1,          //TNR_MCTF_CLAMP_Y_LSR (0~7)
    1,          //TNR_MCTF_CLAMP_C_THRESHOLD (0~255)
    1,          //TNR_MCTF_CLAMP_C_LSR (0~7)
    //ADDITIONAL TNR Config---------------------------------------------------------------------
    0,          //NR3D_CUR_MOTION_SAD_TH
    4,          //NR3D_CUR_MOTION_RAT_TH
    {255, 255},        //NR3D_CUR_MOTION_TWR_P2P_TH
    {255, 255},        //NR3D_REF_MOTION_TWR_P2P_TH
    {255, 255},        //NR3D_CUR_MOTION_TWR_MCTF_TH
    {255, 255},        //NR3D_REF_MOTION_TWR_MCTF_TH
    {1, 2, 1},        //NR3D_MCTF_REF_Y_WEIGHT
    {1, 2, 1},        //NR3D_MCTF_REF_C_WEIGHT
    {1, 2, 1},        //NR3D_P2P_REF_Y_WEIGHT
    {1, 2, 1},        //NR3D_P2P_REF_C_WEIGHT
    3,          //NR3D_P2P_INPUT_WEIGHT
    255,          //NR3D_REF_MOTION_THRESH
    {255, 255},        //NR3D_SAD_TWR_P2P_TH
    {255, 255},         //NR3D_SAD_TWR_MCTF_TH
    0           //enable I frame weight refresh (0: disable, 1:enable)
};

static H26XEncTnr tnr_param_lv3 = //ISO400
{   1,          //enable temporal noise reduction (0: disalbe, 1:enable)
    0,          //TNR Mode as OSD enabled: fully TNR (0: fully TNR, 1: P2P only, 2: no TNR)
    0,          //enable mctf/p2p pixel blending (0: disalbe, 1:enable)
    0,          //P2P SAD Mode (0: pixel, 3: SAD + pixel)
    0,          //MCTF SAD Mode (0: pixel, 1: b8x8, 2: b16x16, 3: auto)
    0,          //MCTF Bias Mode (0: disable, 1: b8x8, 2: b16x16, 3: auto)
    {6, 8, 10},        //TNR_P2P_SAD_THRESHOLD (0~255)
    {16, 8, 0},        //TNR_P2P_WEIGHT (0~16)
    5,          //TNR_P2P_BORDER_CHECK_THRES (0~255)
    4,          //TNR_P2P_BORDER_CHECK_SCALE (0~7)
    0,          //TNR_P2P_INPUT (0: original source, 1: mctf result)
    10,          //TNR_P2P_MCTF_MOTION_THRESHOLD (0~255)
    {1, 1, 1, 1},        //TNR_P2P_MCTF_MOTION_WEIGHT (0~3)
    //P2P TNR Config----------------------------------------------------------------------------
    {2, 5, 3},        //TNR_P2P_REF_Y_WEIGHT (0~7)
    {1, 5, 1},        //TNR_P2P_REF_C_WEIGHT (0~7)
    {12, 20, 28},        //TNR_P2P_RANGE_Y_THRESHOLD (0~255)
    {8, 15, 20},        //TNR_P2P_RANGE_C_THRESHOLD (0~255)
    1,          //TNR_P2P_CLAMP_Y_THRESHOLD (0~255)
    2,          //TNR_P2P_CLAMP_Y_LSR (0~7)
    1,          //TNR_P2P_CLAMP_C_THRESHOLD (0~255)
    2,          //TNR_P2P_CLAMP_C_LSR (0~7)
    //MCTF TNR Config----------------------------------------------------------------------------
    {2, 5, 3},        //TNR_MCTF_REF_Y_WEIGHT (0~7)
    {1, 5, 1},        //TNR_MCTF_REF_C_WEIGHT (0~7)
    {8, 15, 22},        //TNR_MCTF_RANGE_Y_THRESHOLD (0~255)
    {8, 15, 20},        //TNR_MCTF_RANGE_C_THRESHOLD (0~255)
    1,          //TNR_MCTF_CLAMP_Y_THRESHOLD (0~255)
    1,          //TNR_MCTF_CLAMP_Y_LSR (0~7)
    1,          //TNR_MCTF_CLAMP_C_THRESHOLD (0~255)
    1,          //TNR_MCTF_CLAMP_C_LSR (0~7)
    //ADDITIONAL TNR Config---------------------------------------------------------------------
    0,          //NR3D_CUR_MOTION_SAD_TH
    4,          //NR3D_CUR_MOTION_RAT_TH
    {255, 255},        //NR3D_CUR_MOTION_TWR_P2P_TH
    {255, 255},        //NR3D_REF_MOTION_TWR_P2P_TH
    {255, 255},        //NR3D_CUR_MOTION_TWR_MCTF_TH
    {255, 255},        //NR3D_REF_MOTION_TWR_MCTF_TH
    {1, 2, 1},        //NR3D_MCTF_REF_Y_WEIGHT
    {1, 2, 1},        //NR3D_MCTF_REF_C_WEIGHT
    {1, 2, 1},        //NR3D_P2P_REF_Y_WEIGHT
    {1, 2, 1},        //NR3D_P2P_REF_C_WEIGHT
    3,          //NR3D_P2P_INPUT_WEIGHT
    255,          //NR3D_REF_MOTION_THRESH
    {255, 255},        //NR3D_SAD_TWR_P2P_TH
    {255, 255},         //NR3D_SAD_TWR_MCTF_TH
    0           //enable I frame weight refresh (0: disable, 1:enable)
};

static H26XEncTnr tnr_param_lv4 = //ISO800
{   1,          //enable temporal noise reduction (0: disalbe, 1:enable)
    0,          //TNR Mode as OSD enabled: fully TNR (0: fully TNR, 1: P2P only, 2: no TNR)
    0,          //enable mctf/p2p pixel blending (0: disalbe, 1:enable)
    3,          //P2P SAD Mode (0: pixel, 3: SAD + pixel)
    0,          //MCTF SAD Mode (0: pixel, 1: b8x8, 2: b16x16, 3: auto)
    0,          //MCTF Bias Mode (0: disable, 1: b8x8, 2: b16x16, 3: auto)
    {6, 8, 12},        //TNR_P2P_SAD_THRESHOLD (0~255)
    {16, 8, 0},        //TNR_P2P_WEIGHT (0~16)
    5,          //TNR_P2P_BORDER_CHECK_THRES (0~255)
    4,          //TNR_P2P_BORDER_CHECK_SCALE (0~7)
    0,          //TNR_P2P_INPUT (0: original source, 1: mctf result)
    10,          //TNR_P2P_MCTF_MOTION_THRESHOLD (0~255)
    {1, 1, 1, 1},        //TNR_P2P_MCTF_MOTION_WEIGHT (0~3)
    //P2P TNR Config----------------------------------------------------------------------------
    {2, 5, 3},        //TNR_P2P_REF_Y_WEIGHT (0~7)
    {1, 5, 1},        //TNR_P2P_REF_C_WEIGHT (0~7)
    {8, 18, 28},        //TNR_P2P_RANGE_Y_THRESHOLD (0~255)
    {8, 15, 20},        //TNR_P2P_RANGE_C_THRESHOLD (0~255)
    0,          //TNR_P2P_CLAMP_Y_THRESHOLD (0~255)
    2,          //TNR_P2P_CLAMP_Y_LSR (0~7)
    0,          //TNR_P2P_CLAMP_C_THRESHOLD (0~255)
    2,          //TNR_P2P_CLAMP_C_LSR (0~7)
    //MCTF TNR Config----------------------------------------------------------------------------
    {2, 5, 3},        //TNR_MCTF_REF_Y_WEIGHT (0~7)
    {1, 5, 1},        //TNR_MCTF_REF_C_WEIGHT (0~7)
    {8, 15, 24},        //TNR_MCTF_RANGE_Y_THRESHOLD (0~255)
    {8, 15, 20},        //TNR_MCTF_RANGE_C_THRESHOLD (0~255)
    0,          //TNR_MCTF_CLAMP_Y_THRESHOLD (0~255)
    0,          //TNR_MCTF_CLAMP_Y_LSR (0~7)
    0,          //TNR_MCTF_CLAMP_C_THRESHOLD (0~255)
    0,          //TNR_MCTF_CLAMP_C_LSR (0~7)
    //ADDITIONAL TNR Config---------------------------------------------------------------------
    0,          //NR3D_CUR_MOTION_SAD_TH
    4,          //NR3D_CUR_MOTION_RAT_TH
    {255, 255},        //NR3D_CUR_MOTION_TWR_P2P_TH
    {255, 255},        //NR3D_REF_MOTION_TWR_P2P_TH
    {255, 255},        //NR3D_CUR_MOTION_TWR_MCTF_TH
    {255, 255},        //NR3D_REF_MOTION_TWR_MCTF_TH
    {1, 2, 1},        //NR3D_MCTF_REF_Y_WEIGHT
    {1, 2, 1},        //NR3D_MCTF_REF_C_WEIGHT
    {1, 2, 1},        //NR3D_P2P_REF_Y_WEIGHT
    {1, 2, 1},        //NR3D_P2P_REF_C_WEIGHT
    3,          //NR3D_P2P_INPUT_WEIGHT
    255,          //NR3D_REF_MOTION_THRESH
    {255, 255},        //NR3D_SAD_TWR_P2P_TH
    {255, 255},         //NR3D_SAD_TWR_MCTF_TH
    0           //enable I frame weight refresh (0: disable, 1:enable)
};

static H26XEncTnr tnr_param_lv5 = //ISO1600
{   1,          //enable temporal noise reduction (0: disalbe, 1:enable)
    0,          //TNR Mode as OSD enabled: fully TNR (0: fully TNR, 1: P2P only, 2: no TNR)
    0,          //enable mctf/p2p pixel blending (0: disalbe, 1:enable)
    3,          //P2P SAD Mode (0: pixel, 3: SAD + pixel)
    0,          //MCTF SAD Mode (0: pixel, 1: b8x8, 2: b16x16, 3: auto)
    0,          //MCTF Bias Mode (0: disable, 1: b8x8, 2: b16x16, 3: auto)
    {2, 6, 10},        //TNR_P2P_SAD_THRESHOLD (0~255)
    {16, 8, 0},        //TNR_P2P_WEIGHT (0~16)
    6,          //TNR_P2P_BORDER_CHECK_THRES (0~255)
    4,          //TNR_P2P_BORDER_CHECK_SCALE (0~7)
    0,          //TNR_P2P_INPUT (0: original source, 1: mctf result)
    10,          //TNR_P2P_MCTF_MOTION_THRESHOLD (0~255)
    {1, 1, 1, 1},        //TNR_P2P_MCTF_MOTION_WEIGHT (0~3)
    //P2P TNR Config----------------------------------------------------------------------------
    {3, 5, 4},        //TNR_P2P_REF_Y_WEIGHT (0~7)
    {1, 5, 1},        //TNR_P2P_REF_C_WEIGHT (0~7)
    {12, 20, 28},        //TNR_P2P_RANGE_Y_THRESHOLD (0~255)
    {8, 16, 24},        //TNR_P2P_RANGE_C_THRESHOLD (0~255)
    0,          //TNR_P2P_CLAMP_Y_THRESHOLD (0~255)
    2,          //TNR_P2P_CLAMP_Y_LSR (0~7)
    0,          //TNR_P2P_CLAMP_C_THRESHOLD (0~255)
    2,          //TNR_P2P_CLAMP_C_LSR (0~7)
    //MCTF TNR Config----------------------------------------------------------------------------
    {2, 5, 3},        //TNR_MCTF_REF_Y_WEIGHT (0~7)
    {1, 5, 1},        //TNR_MCTF_REF_C_WEIGHT (0~7)
    {10, 20, 30},        //TNR_MCTF_RANGE_Y_THRESHOLD (0~255)
    {8, 12, 16},        //TNR_MCTF_RANGE_C_THRESHOLD (0~255)
    0,          //TNR_MCTF_CLAMP_Y_THRESHOLD (0~255)
    0,          //TNR_MCTF_CLAMP_Y_LSR (0~7)
    0,          //TNR_MCTF_CLAMP_C_THRESHOLD (0~255)
    0,          //TNR_MCTF_CLAMP_C_LSR (0~7)
    //ADDITIONAL TNR Config---------------------------------------------------------------------
    0,          //NR3D_CUR_MOTION_SAD_TH
    4,          //NR3D_CUR_MOTION_RAT_TH
    {255, 255},        //NR3D_CUR_MOTION_TWR_P2P_TH
    {255, 255},        //NR3D_REF_MOTION_TWR_P2P_TH
    {255, 255},        //NR3D_CUR_MOTION_TWR_MCTF_TH
    {255, 255},        //NR3D_REF_MOTION_TWR_MCTF_TH
    {1, 2, 1},        //NR3D_MCTF_REF_Y_WEIGHT
    {1, 2, 1},        //NR3D_MCTF_REF_C_WEIGHT
    {1, 2, 1},        //NR3D_P2P_REF_Y_WEIGHT
    {1, 2, 1},        //NR3D_P2P_REF_C_WEIGHT
    3,          //NR3D_P2P_INPUT_WEIGHT
    255,          //NR3D_REF_MOTION_THRESH
    {255, 255},        //NR3D_SAD_TWR_P2P_TH
    {255, 255},         //NR3D_SAD_TWR_MCTF_TH
    0           //enable I frame weight refresh (0: disable, 1:enable)
};

static H26XEncTnr tnr_param_lv6 = //ISO3200
{   1,          //enable temporal noise reduction (0: disalbe, 1:enable)
    0,          //TNR Mode as OSD enabled: fully TNR (0: fully TNR, 1: P2P only, 2: no TNR)
    1,          //enable mctf/p2p pixel blending (0: disalbe, 1:enable)
    3,          //P2P SAD Mode (0: pixel, 3: SAD + pixel)
    0,          //MCTF SAD Mode (0: pixel, 1: b8x8, 2: b16x16, 3: auto)
    0,          //MCTF Bias Mode (0: disable, 1: b8x8, 2: b16x16, 3: auto)
    {8, 10, 12},        //TNR_P2P_SAD_THRESHOLD (0~255)
    {16, 8, 0},        //TNR_P2P_WEIGHT (0~16)
    5,          //TNR_P2P_BORDER_CHECK_THRES (0~255)
    4,          //TNR_P2P_BORDER_CHECK_SCALE (0~7)
    0,          //TNR_P2P_INPUT (0: original source, 1: mctf result)
    10,          //TNR_P2P_MCTF_MOTION_THRESHOLD (0~255)
    {2, 2, 2, 2},        //TNR_P2P_MCTF_MOTION_WEIGHT (0~3)
    //P2P TNR Config----------------------------------------------------------------------------
    {1, 2, 2},        //TNR_P2P_REF_Y_WEIGHT (0~7)
    {1, 5, 2},        //TNR_P2P_REF_C_WEIGHT (0~7)
    {16, 24, 40},        //TNR_P2P_RANGE_Y_THRESHOLD (0~255)
    {10, 18, 28},        //TNR_P2P_RANGE_C_THRESHOLD (0~255)
    0,          //TNR_P2P_CLAMP_Y_THRESHOLD (0~255)
    2,          //TNR_P2P_CLAMP_Y_LSR (0~7)
    0,          //TNR_P2P_CLAMP_C_THRESHOLD (0~255)
    2,          //TNR_P2P_CLAMP_C_LSR (0~7)
    //MCTF TNR Config----------------------------------------------------------------------------
    {2, 5, 4},        //TNR_MCTF_REF_Y_WEIGHT (0~7)
    {1, 5, 1},        //TNR_MCTF_REF_C_WEIGHT (0~7)
    {10, 20, 30},        //TNR_MCTF_RANGE_Y_THRESHOLD (0~255)
    {8, 12, 20},        //TNR_MCTF_RANGE_C_THRESHOLD (0~255)
    0,          //TNR_MCTF_CLAMP_Y_THRESHOLD (0~255)
    0,          //TNR_MCTF_CLAMP_Y_LSR (0~7)
    0,          //TNR_MCTF_CLAMP_C_THRESHOLD (0~255)
    0,          //TNR_MCTF_CLAMP_C_LSR (0~7)
    //ADDITIONAL TNR Config---------------------------------------------------------------------
    0,          //NR3D_CUR_MOTION_SAD_TH
    4,          //NR3D_CUR_MOTION_RAT_TH
    {255, 255},        //NR3D_CUR_MOTION_TWR_P2P_TH
    {255, 255},        //NR3D_REF_MOTION_TWR_P2P_TH
    {255, 255},        //NR3D_CUR_MOTION_TWR_MCTF_TH
    {255, 255},        //NR3D_REF_MOTION_TWR_MCTF_TH
    {1, 2, 1},        //NR3D_MCTF_REF_Y_WEIGHT
    {1, 2, 1},        //NR3D_MCTF_REF_C_WEIGHT
    {1, 2, 1},        //NR3D_P2P_REF_Y_WEIGHT
    {1, 2, 1},        //NR3D_P2P_REF_C_WEIGHT
    3,          //NR3D_P2P_INPUT_WEIGHT
    255,          //NR3D_REF_MOTION_THRESH
    {255, 255},        //NR3D_SAD_TWR_P2P_TH
    {255, 255},         //NR3D_SAD_TWR_MCTF_TH
    0           //enable I frame weight refresh (0: disable, 1:enable)
};

static H26XEncTnr tnr_param_lv7 = //ISO6400
{   1,          //enable temporal noise reduction (0: disalbe, 1:enable)
    0,          //TNR Mode as OSD enabled: fully TNR (0: fully TNR, 1: P2P only, 2: no TNR)
    1,          //enable mctf/p2p pixel blending (0: disalbe, 1:enable)
    3,          //P2P SAD Mode (0: pixel, 3: SAD + pixel)
    0,          //MCTF SAD Mode (0: pixel, 1: b8x8, 2: b16x16, 3: auto)
    0,          //MCTF Bias Mode (0: disable, 1: b8x8, 2: b16x16, 3: auto)
    {6, 12, 18},        //TNR_P2P_SAD_THRESHOLD (0~255)
    {16, 8, 0},        //TNR_P2P_WEIGHT (0~16)
    16,          //TNR_P2P_BORDER_CHECK_THRES (0~255)
    4,          //TNR_P2P_BORDER_CHECK_SCALE (0~7)
    0,          //TNR_P2P_INPUT (0: original source, 1: mctf result)
    10,          //TNR_P2P_MCTF_MOTION_THRESHOLD (0~255)
    {3, 3, 3, 3},        //TNR_P2P_MCTF_MOTION_WEIGHT (0~3)
    //P2P TNR Config----------------------------------------------------------------------------
    {1, 2, 2},        //TNR_P2P_REF_Y_WEIGHT (0~7)
    {1, 2, 1},        //TNR_P2P_REF_C_WEIGHT (0~7)
    {20, 30, 40},        //TNR_P2P_RANGE_Y_THRESHOLD (0~255)
    {10, 20, 30},        //TNR_P2P_RANGE_C_THRESHOLD (0~255)
    0,          //TNR_P2P_CLAMP_Y_THRESHOLD (0~255)
    2,          //TNR_P2P_CLAMP_Y_LSR (0~7)
    0,          //TNR_P2P_CLAMP_C_THRESHOLD (0~255)
    2,          //TNR_P2P_CLAMP_C_LSR (0~7)
    //MCTF TNR Config----------------------------------------------------------------------------
    {2, 5, 4},        //TNR_MCTF_REF_Y_WEIGHT (0~7)
    {1, 5, 1},        //TNR_MCTF_REF_C_WEIGHT (0~7)
    {16, 24, 32},        //TNR_MCTF_RANGE_Y_THRESHOLD (0~255)
    {10, 20, 30},        //TNR_MCTF_RANGE_C_THRESHOLD (0~255)
    0,          //TNR_MCTF_CLAMP_Y_THRESHOLD (0~255)
    0,          //TNR_MCTF_CLAMP_Y_LSR (0~7)
    0,          //TNR_MCTF_CLAMP_C_THRESHOLD (0~255)
    0,          //TNR_MCTF_CLAMP_C_LSR (0~7)
    //ADDITIONAL TNR Config---------------------------------------------------------------------
    0,          //NR3D_CUR_MOTION_SAD_TH
    4,          //NR3D_CUR_MOTION_RAT_TH
    {255, 255},        //NR3D_CUR_MOTION_TWR_P2P_TH
    {255, 255},        //NR3D_REF_MOTION_TWR_P2P_TH
    {255, 255},        //NR3D_CUR_MOTION_TWR_MCTF_TH
    {255, 255},        //NR3D_REF_MOTION_TWR_MCTF_TH
    {1, 2, 1},        //NR3D_MCTF_REF_Y_WEIGHT
    {1, 2, 1},        //NR3D_MCTF_REF_C_WEIGHT
    {1, 2, 1},        //NR3D_P2P_REF_Y_WEIGHT
    {1, 2, 1},        //NR3D_P2P_REF_C_WEIGHT
    3,          //NR3D_P2P_INPUT_WEIGHT
    255,          //NR3D_REF_MOTION_THRESH
    {255, 255},        //NR3D_SAD_TWR_P2P_TH
    {255, 255},         //NR3D_SAD_TWR_MCTF_TH
    0           //enable I frame weight refresh (0: disable, 1:enable)
};

static H26XEncTnr tnr_param_lv8 = //ISO12800
{   1,          //enable temporal noise reduction (0: disalbe, 1:enable)
    0,          //TNR Mode as OSD enabled: fully TNR (0: fully TNR, 1: P2P only, 2: no TNR)
    0,          //enable mctf/p2p pixel blending (0: disalbe, 1:enable)
    3,          //P2P SAD Mode (0: pixel, 3: SAD + pixel)
    0,          //MCTF SAD Mode (0: pixel, 1: b8x8, 2: b16x16, 3: auto)
    0,          //MCTF Bias Mode (0: disable, 1: b8x8, 2: b16x16, 3: auto)
    {6, 8, 12},        //TNR_P2P_SAD_THRESHOLD (0~255)
    {16, 8, 0},        //TNR_P2P_WEIGHT (0~16)
    16,          //TNR_P2P_BORDER_CHECK_THRES (0~255)
    4,          //TNR_P2P_BORDER_CHECK_SCALE (0~7)
    0,          //TNR_P2P_INPUT (0: original source, 1: mctf result)
    10,          //TNR_P2P_MCTF_MOTION_THRESHOLD (0~255)
    {2, 2, 2, 2},        //TNR_P2P_MCTF_MOTION_WEIGHT (0~3)
    //P2P TNR Config----------------------------------------------------------------------------
    {1, 2, 2},        //TNR_P2P_REF_Y_WEIGHT (0~7)
    {1, 2, 2},        //TNR_P2P_REF_C_WEIGHT (0~7)
    {60, 70, 80},        //TNR_P2P_RANGE_Y_THRESHOLD (0~255)
    {30, 50, 60},        //TNR_P2P_RANGE_C_THRESHOLD (0~255)
    0,          //TNR_P2P_CLAMP_Y_THRESHOLD (0~255)
    2,          //TNR_P2P_CLAMP_Y_LSR (0~7)
    0,          //TNR_P2P_CLAMP_C_THRESHOLD (0~255)
    2,          //TNR_P2P_CLAMP_C_LSR (0~7)
    //MCTF TNR Config----------------------------------------------------------------------------
    {1, 7, 1},        //TNR_MCTF_REF_Y_WEIGHT (0~7)
    {1, 7, 1},        //TNR_MCTF_REF_C_WEIGHT (0~7)
    {40, 60, 80},        //TNR_MCTF_RANGE_Y_THRESHOLD (0~255)
    {20, 40, 60},        //TNR_MCTF_RANGE_C_THRESHOLD (0~255)
    0,          //TNR_MCTF_CLAMP_Y_THRESHOLD (0~255)
    0,          //TNR_MCTF_CLAMP_Y_LSR (0~7)
    0,          //TNR_MCTF_CLAMP_C_THRESHOLD (0~255)
    0,          //TNR_MCTF_CLAMP_C_LSR (0~7)
    //ADDITIONAL TNR Config---------------------------------------------------------------------
    0,          //NR3D_CUR_MOTION_SAD_TH
    4,          //NR3D_CUR_MOTION_RAT_TH
    {255, 255},        //NR3D_CUR_MOTION_TWR_P2P_TH
    {255, 255},        //NR3D_REF_MOTION_TWR_P2P_TH
    {255, 255},        //NR3D_CUR_MOTION_TWR_MCTF_TH
    {255, 255},        //NR3D_REF_MOTION_TWR_MCTF_TH
    {1, 2, 1},        //NR3D_MCTF_REF_Y_WEIGHT
    {1, 2, 1},        //NR3D_MCTF_REF_C_WEIGHT
    {1, 2, 1},        //NR3D_P2P_REF_Y_WEIGHT
    {1, 2, 1},        //NR3D_P2P_REF_C_WEIGHT
    3,          //NR3D_P2P_INPUT_WEIGHT
    255,          //NR3D_REF_MOTION_THRESH
    {255, 255},        //NR3D_SAD_TWR_P2P_TH
    {255, 255},         //NR3D_SAD_TWR_MCTF_TH
    0           //enable I frame weight refresh (0: disable, 1:enable)
};

static H26XEncTnr tnr_param_lv9 = //ISO25600
{   1,          //enable temporal noise reduction (0: disalbe, 1:enable)
    0,          //TNR Mode as OSD enabled: fully TNR (0: fully TNR, 1: P2P only, 2: no TNR)
    0,          //enable mctf/p2p pixel blending (0: disalbe, 1:enable)
    3,          //P2P SAD Mode (0: pixel, 3: SAD + pixel)
    0,          //MCTF SAD Mode (0: pixel, 1: b8x8, 2: b16x16, 3: auto)
    0,          //MCTF Bias Mode (0: disable, 1: b8x8, 2: b16x16, 3: auto)
    {12, 15, 20},        //TNR_P2P_SAD_THRESHOLD (0~255)
    {16, 8, 0},        //TNR_P2P_WEIGHT (0~16)
    12,          //TNR_P2P_BORDER_CHECK_THRES (0~255)
    4,          //TNR_P2P_BORDER_CHECK_SCALE (0~7)
    0,          //TNR_P2P_INPUT (0: original source, 1: mctf result)
    6,          //TNR_P2P_MCTF_MOTION_THRESHOLD (0~255)
    {3, 3, 3, 3},        //TNR_P2P_MCTF_MOTION_WEIGHT (0~3)
    //P2P TNR Config----------------------------------------------------------------------------
    {1, 7, 2},        //TNR_P2P_REF_Y_WEIGHT (0~7)
    {1, 7, 2},        //TNR_P2P_REF_C_WEIGHT (0~7)
    {110, 120, 140},        //TNR_P2P_RANGE_Y_THRESHOLD (0~255)
    {20, 40, 50},        //TNR_P2P_RANGE_C_THRESHOLD (0~255)
    0,          //TNR_P2P_CLAMP_Y_THRESHOLD (0~255)
    2,          //TNR_P2P_CLAMP_Y_LSR (0~7)
    0,          //TNR_P2P_CLAMP_C_THRESHOLD (0~255)
    2,          //TNR_P2P_CLAMP_C_LSR (0~7)
    //MCTF TNR Config----------------------------------------------------------------------------
    {1, 7, 1},        //TNR_MCTF_REF_Y_WEIGHT (0~7)
    {1, 7, 1},        //TNR_MCTF_REF_C_WEIGHT (0~7)
    {110, 120, 130},        //TNR_MCTF_RANGE_Y_THRESHOLD (0~255)
    {30, 50, 60},        //TNR_MCTF_RANGE_C_THRESHOLD (0~255)
    0,          //TNR_MCTF_CLAMP_Y_THRESHOLD (0~255)
    0,          //TNR_MCTF_CLAMP_Y_LSR (0~7)
    0,          //TNR_MCTF_CLAMP_C_THRESHOLD (0~255)
    0,          //TNR_MCTF_CLAMP_C_LSR (0~7)
    //ADDITIONAL TNR Config---------------------------------------------------------------------
    0,          //NR3D_CUR_MOTION_SAD_TH
    4,          //NR3D_CUR_MOTION_RAT_TH
    {255, 255},        //NR3D_CUR_MOTION_TWR_P2P_TH
    {255, 255},        //NR3D_REF_MOTION_TWR_P2P_TH
    {255, 255},        //NR3D_CUR_MOTION_TWR_MCTF_TH
    {255, 255},        //NR3D_REF_MOTION_TWR_MCTF_TH
    {1, 2, 1},        //NR3D_MCTF_REF_Y_WEIGHT
    {1, 2, 1},        //NR3D_MCTF_REF_C_WEIGHT
    {1, 2, 1},        //NR3D_P2P_REF_Y_WEIGHT
    {1, 2, 1},        //NR3D_P2P_REF_C_WEIGHT
    3,          //NR3D_P2P_INPUT_WEIGHT
    255,          //NR3D_REF_MOTION_THRESH
    {255, 255},        //NR3D_SAD_TWR_P2P_TH
    {255, 255},         //NR3D_SAD_TWR_MCTF_TH
    0           //enable I frame weight refresh (0: disable, 1:enable)
};

static H26XEncTnr tnr_param_lv10 = //ISO51200
{   1,          //enable temporal noise reduction (0: disalbe, 1:enable)
    0,          //TNR Mode as OSD enabled: fully TNR (0: fully TNR, 1: P2P only, 2: no TNR)
    0,          //enable mctf/p2p pixel blending (0: disalbe, 1:enable)
    3,          //P2P SAD Mode (0: pixel, 3: SAD + pixel)
    0,          //MCTF SAD Mode (0: pixel, 1: b8x8, 2: b16x16, 3: auto)
    0,          //MCTF Bias Mode (0: disable, 1: b8x8, 2: b16x16, 3: auto)
    {12, 15, 20},        //TNR_P2P_SAD_THRESHOLD (0~255)
    {16, 8, 0},        //TNR_P2P_WEIGHT (0~16)
    12,          //TNR_P2P_BORDER_CHECK_THRES (0~255)
    4,          //TNR_P2P_BORDER_CHECK_SCALE (0~7)
    0,          //TNR_P2P_INPUT (0: original source, 1: mctf result)
    6,          //TNR_P2P_MCTF_MOTION_THRESHOLD (0~255)
    {3, 3, 3, 3},        //TNR_P2P_MCTF_MOTION_WEIGHT (0~3)
    //P2P TNR Config----------------------------------------------------------------------------
    {1, 5, 2},        //TNR_P2P_REF_Y_WEIGHT (0~7)
    {1, 5, 2},        //TNR_P2P_REF_C_WEIGHT (0~7)
    {90, 110, 120},        //TNR_P2P_RANGE_Y_THRESHOLD (0~255)
    {40, 60, 80},        //TNR_P2P_RANGE_C_THRESHOLD (0~255)
    0,          //TNR_P2P_CLAMP_Y_THRESHOLD (0~255)
    2,          //TNR_P2P_CLAMP_Y_LSR (0~7)
    0,          //TNR_P2P_CLAMP_C_THRESHOLD (0~255)
    2,          //TNR_P2P_CLAMP_C_LSR (0~7)
    //MCTF TNR Config----------------------------------------------------------------------------
    {1, 7, 1},        //TNR_MCTF_REF_Y_WEIGHT (0~7)
    {1, 7, 1},        //TNR_MCTF_REF_C_WEIGHT (0~7)
    {80, 90, 100},        //TNR_MCTF_RANGE_Y_THRESHOLD (0~255)
    {40, 60, 80},        //TNR_MCTF_RANGE_C_THRESHOLD (0~255)
    0,          //TNR_MCTF_CLAMP_Y_THRESHOLD (0~255)
    0,          //TNR_MCTF_CLAMP_Y_LSR (0~7)
    0,          //TNR_MCTF_CLAMP_C_THRESHOLD (0~255)
    0,          //TNR_MCTF_CLAMP_C_LSR (0~7)
    //ADDITIONAL TNR Config---------------------------------------------------------------------
    0,          //NR3D_CUR_MOTION_SAD_TH
    4,          //NR3D_CUR_MOTION_RAT_TH
    {255, 255},        //NR3D_CUR_MOTION_TWR_P2P_TH
    {255, 255},        //NR3D_REF_MOTION_TWR_P2P_TH
    {255, 255},        //NR3D_CUR_MOTION_TWR_MCTF_TH
    {255, 255},        //NR3D_REF_MOTION_TWR_MCTF_TH
    {1, 2, 1},        //NR3D_MCTF_REF_Y_WEIGHT
    {1, 2, 1},        //NR3D_MCTF_REF_C_WEIGHT
    {1, 2, 1},        //NR3D_P2P_REF_Y_WEIGHT
    {1, 2, 1},        //NR3D_P2P_REF_C_WEIGHT
    3,          //NR3D_P2P_INPUT_WEIGHT
    255,          //NR3D_REF_MOTION_THRESH
    {255, 255},        //NR3D_SAD_TWR_P2P_TH
    {255, 255},         //NR3D_SAD_TWR_MCTF_TH
    0           //enable I frame weight refresh (0: disable, 1:enable)
};

static H26XEncTnr tnr_param_lv11 = //ISO102400
{   1,          //enable temporal noise reduction (0: disalbe, 1:enable)
    0,          //TNR Mode as OSD enabled: fully TNR (0: fully TNR, 1: P2P only, 2: no TNR)
    0,          //enable mctf/p2p pixel blending (0: disalbe, 1:enable)
    3,          //P2P SAD Mode (0: pixel, 3: SAD + pixel)
    0,          //MCTF SAD Mode (0: pixel, 1: b8x8, 2: b16x16, 3: auto)
    0,          //MCTF Bias Mode (0: disable, 1: b8x8, 2: b16x16, 3: auto)
    {20, 25, 28},        //TNR_P2P_SAD_THRESHOLD (0~255)
    {16, 8, 0},        //TNR_P2P_WEIGHT (0~16)
    16,          //TNR_P2P_BORDER_CHECK_THRES (0~255)
    4,          //TNR_P2P_BORDER_CHECK_SCALE (0~7)
    0,          //TNR_P2P_INPUT (0: original source, 1: mctf result)
    5,          //TNR_P2P_MCTF_MOTION_THRESHOLD (0~255)
    {3, 3, 3, 3},        //TNR_P2P_MCTF_MOTION_WEIGHT (0~3)
    //P2P TNR Config----------------------------------------------------------------------------
    {1, 3, 3},        //TNR_P2P_REF_Y_WEIGHT (0~7)
    {1, 4, 2},        //TNR_P2P_REF_C_WEIGHT (0~7)
    {150, 160, 170},        //TNR_P2P_RANGE_Y_THRESHOLD (0~255)
    {40, 60, 80},        //TNR_P2P_RANGE_C_THRESHOLD (0~255)
    0,          //TNR_P2P_CLAMP_Y_THRESHOLD (0~255)
    2,          //TNR_P2P_CLAMP_Y_LSR (0~7)
    0,          //TNR_P2P_CLAMP_C_THRESHOLD (0~255)
    2,          //TNR_P2P_CLAMP_C_LSR (0~7)
    //MCTF TNR Config----------------------------------------------------------------------------
    {1, 7, 1},        //TNR_MCTF_REF_Y_WEIGHT (0~7)
    {1, 7, 1},        //TNR_MCTF_REF_C_WEIGHT (0~7)
    {80, 90, 100},        //TNR_MCTF_RANGE_Y_THRESHOLD (0~255)
    {40, 60, 80},        //TNR_MCTF_RANGE_C_THRESHOLD (0~255)
    0,          //TNR_MCTF_CLAMP_Y_THRESHOLD (0~255)
    0,          //TNR_MCTF_CLAMP_Y_LSR (0~7)
    0,          //TNR_MCTF_CLAMP_C_THRESHOLD (0~255)
    0,          //TNR_MCTF_CLAMP_C_LSR (0~7)
    //ADDITIONAL TNR Config---------------------------------------------------------------------
    0,          //NR3D_CUR_MOTION_SAD_TH
    4,          //NR3D_CUR_MOTION_RAT_TH
    {255, 255},        //NR3D_CUR_MOTION_TWR_P2P_TH
    {255, 255},        //NR3D_REF_MOTION_TWR_P2P_TH
    {255, 255},        //NR3D_CUR_MOTION_TWR_MCTF_TH
    {255, 255},        //NR3D_REF_MOTION_TWR_MCTF_TH
    {1, 2, 1},        //NR3D_MCTF_REF_Y_WEIGHT
    {1, 2, 1},        //NR3D_MCTF_REF_C_WEIGHT
    {1, 2, 1},        //NR3D_P2P_REF_Y_WEIGHT
    {1, 2, 1},        //NR3D_P2P_REF_C_WEIGHT
    3,          //NR3D_P2P_INPUT_WEIGHT
    255,          //NR3D_REF_MOTION_THRESH
    {255, 255},        //NR3D_SAD_TWR_P2P_TH
    {255, 255},         //NR3D_SAD_TWR_MCTF_TH
    0           //enable I frame weight refresh (0: disable, 1:enable)
};

static H26XEncTnr tnr_param_lv12 = //ISO204800
{   1,          //enable temporal noise reduction (0: disalbe, 1:enable)
    0,          //TNR Mode as OSD enabled: fully TNR (0: fully TNR, 1: P2P only, 2: no TNR)
    0,          //enable mctf/p2p pixel blending (0: disalbe, 1:enable)
    3,          //P2P SAD Mode (0: pixel, 3: SAD + pixel)
    0,          //MCTF SAD Mode (0: pixel, 1: b8x8, 2: b16x16, 3: auto)
    0,          //MCTF Bias Mode (0: disable, 1: b8x8, 2: b16x16, 3: auto)
    {20, 25, 28},        //TNR_P2P_SAD_THRESHOLD (0~255)
    {16, 8, 0},        //TNR_P2P_WEIGHT (0~16)
    16,          //TNR_P2P_BORDER_CHECK_THRES (0~255)
    4,          //TNR_P2P_BORDER_CHECK_SCALE (0~7)
    0,          //TNR_P2P_INPUT (0: original source, 1: mctf result)
    5,          //TNR_P2P_MCTF_MOTION_THRESHOLD (0~255)
    {3, 3, 3, 3},        //TNR_P2P_MCTF_MOTION_WEIGHT (0~3)
    //P2P TNR Config----------------------------------------------------------------------------
    {1, 1, 3},        //TNR_P2P_REF_Y_WEIGHT (0~7)
    {1, 2, 2},        //TNR_P2P_REF_C_WEIGHT (0~7)
    {150, 160, 170},        //TNR_P2P_RANGE_Y_THRESHOLD (0~255)
    {40, 60, 80},        //TNR_P2P_RANGE_C_THRESHOLD (0~255)
    0,          //TNR_P2P_CLAMP_Y_THRESHOLD (0~255)
    2,          //TNR_P2P_CLAMP_Y_LSR (0~7)
    0,          //TNR_P2P_CLAMP_C_THRESHOLD (0~255)
    2,          //TNR_P2P_CLAMP_C_LSR (0~7)
    //MCTF TNR Config----------------------------------------------------------------------------
    {1, 3, 1},        //TNR_MCTF_REF_Y_WEIGHT (0~7)
    {1, 3, 1},        //TNR_MCTF_REF_C_WEIGHT (0~7)
    {80, 90, 100},        //TNR_MCTF_RANGE_Y_THRESHOLD (0~255)
    {40, 60, 80},        //TNR_MCTF_RANGE_C_THRESHOLD (0~255)
    0,          //TNR_MCTF_CLAMP_Y_THRESHOLD (0~255)
    0,          //TNR_MCTF_CLAMP_Y_LSR (0~7)
    0,          //TNR_MCTF_CLAMP_C_THRESHOLD (0~255)
    0,          //TNR_MCTF_CLAMP_C_LSR (0~7)
    //ADDITIONAL TNR Config---------------------------------------------------------------------
    0,          //NR3D_CUR_MOTION_SAD_TH
    4,          //NR3D_CUR_MOTION_RAT_TH
    {255, 255},        //NR3D_CUR_MOTION_TWR_P2P_TH
    {255, 255},        //NR3D_REF_MOTION_TWR_P2P_TH
    {255, 255},        //NR3D_CUR_MOTION_TWR_MCTF_TH
    {255, 255},        //NR3D_REF_MOTION_TWR_MCTF_TH
    {1, 2, 1},        //NR3D_MCTF_REF_Y_WEIGHT
    {1, 2, 1},        //NR3D_MCTF_REF_C_WEIGHT
    {1, 2, 1},        //NR3D_P2P_REF_Y_WEIGHT
    {1, 2, 1},        //NR3D_P2P_REF_C_WEIGHT
    3,          //NR3D_P2P_INPUT_WEIGHT
    255,          //NR3D_REF_MOTION_THRESH
    {255, 255},        //NR3D_SAD_TWR_P2P_TH
    {255, 255},         //NR3D_SAD_TWR_MCTF_TH
    0           //enable I frame weight refresh (0: disable, 1:enable)
};
static INT8 tnr_level[TOTAL_GAIN_NUM]=
{
    H264_UI_3DNR_LV1,
    H264_UI_3DNR_LV2,//ISO200
    H264_UI_3DNR_LV3,
    H264_UI_3DNR_LV4,//ISO800
    H264_UI_3DNR_LV5,
    H264_UI_3DNR_LV6,//3200
    H264_UI_3DNR_LV7,
    H264_UI_3DNR_LV8,//12800
    H264_UI_3DNR_LV9,
    H264_UI_3DNR_LV10,
    H264_UI_3DNR_LV11,
    H264_UI_3DNR_LV12,
};

static H26XEncTnr* tnr_param[TOTAL_UI_3DNR_LV]=
{
    &tnr_param_off,
    &tnr_param_lv1,
    &tnr_param_lv2,
    &tnr_param_lv3,
    &tnr_param_lv4,
    &tnr_param_lv5,
    &tnr_param_lv6,
    &tnr_param_lv7,
    &tnr_param_lv8,
    &tnr_param_lv9,
    &tnr_param_lv10,
    &tnr_param_lv11,
    &tnr_param_lv12,
};

static sISP_ISOINDEX gISO_Index[Total_SENIDNum] = {0};
void iq_param_gc2053_evb_table1_init(UINT32 id,UINT32 mode_idx)
{
	//iqs_set_range
	UINT32 *iq_param_addr[IQ_PARAM_MAX];
	static UINT32 num_3DNR_param = TOTAL_UI_3DNR_LV;
	gISO_Index[id].Id = id;
    gISO_Index[id].num = IQS_ISOMAPTAB_NUM - 1;

	iq_param_addr[IQ_ISOIDX] = (UINT32 *)&gISO_Index[id];
    iq_param_addr[IQ_ISOIDX_MAP ] = (UINT32*)&iso_map_tab_gc2053_evb[0];
    iq_param_addr[IQ_ENGINE_CTRL  ] = (UINT32*)&e_ctrl;

	//***********
	//**  SIE  **
	//***********
    iq_param_addr[IQ_OBSUB      ] = (UINT32 *)ob[0];
	iq_param_addr[IQ_ECS_TAB    ] = (UINT32 *)ecs_tab;
	iq_param_addr[IQ_ECS        ] = (UINT32 *)&ecs;
	iq_param_addr[IQ_DPC        ] = (UINT32 *)&dpc;

	//***********
	//**  RHE  **
	//***********
	iq_param_addr[IQ_WDR             ] = (UINT32*) &wdr_strength[0];
    iq_param_addr[IQ_WDR_LCE_OFFSET  ] = (UINT32*) &wdr_lce_offset;
	iq_param_addr[IQ_LCE             ] = (UINT32*) &lce_strength[0];
    iq_param_addr[IQ_GCE             ] = (UINT32*) &gce_strength[0];
	iq_param_addr[IQ_DEFOG_INIT_PARAM] = (UINT32 *)&defog_init_param_gc2053_evb;
	iq_param_addr[IQ_DEFOG_WET       ] = (UINT32 *)&defog_wet_gc2053_evb[0];

	//***********
	//**  IFE  **
	//***********
	iq_param_addr[IQ_OUTL_BRI        ] = (UINT32 *)outl_bri[0];
	iq_param_addr[IQ_OUTL_DARK       ] = (UINT32 *)outl_dark[0];
	iq_param_addr[IQ_OUTL_CNT        ] = (UINT32 *)outl_cnt[0];
	iq_param_addr[IQ_OUTL            ] = (UINT32 *)&outl_set[0];
	iq_param_addr[IQ_SPATIAL_W       ] = (UINT32 *)spatial_w[0];
	iq_param_addr[IQ_SPATAL          ] = (UINT32 *)&spatial[0];
	iq_param_addr[IQ_RANGEA_THR      ] = (UINT32 *)range_a_thr[0];
	iq_param_addr[IQ_RANGEA_LUT      ] = (UINT32 *)range_a_lut[0];
	iq_param_addr[IQ_RANGEB_THR      ] = (UINT32 *)range_b_thr[0];
	iq_param_addr[IQ_RANGEB_LUT      ] = (UINT32 *)range_b_lut[0];
	iq_param_addr[IQ_RANGEB          ] = (UINT32 *)&range_b[0];
	iq_param_addr[IQ_CLAMP           ] = (UINT32 *)&filter_clamp[0];
	iq_param_addr[IQ_VIG_CENTER      ] = (UINT32 *)&vig_center;
	iq_param_addr[IQ_VIG_TAB         ] = (UINT32 *)&vig;
    iq_param_addr[IQ_VIG_ISO_TH 	 ] = (UINT32 *)&vig_iso_th;
	iq_param_addr[IQ_GBAL_STAB       ] = (UINT32 *)gbal_s_tab[0];
	iq_param_addr[IQ_GBAL_DTAB       ] = (UINT32 *)gbal_d_tab[0];
	iq_param_addr[IQ_GBAL            ] = (UINT32 *)&gbal[0];
    iq_param_addr[IQ_RBFILL_LUMA_TAB ] = (UINT32 *)rbfill_luma_tab;
    iq_param_addr[IQ_RBFILL_RATIO_TAB] = (UINT32 *)rbfill_ratio_tab;
    iq_param_addr[IQ_RBFILL          ] = (UINT32 *)&rbfill[0];

	//***********
	//**  DCE  **
	//***********
    iq_param_addr[IQ_DCE_DIST	] = (UINT32*)&dist_gc2053_evb;
    iq_param_addr[IQ_DCE_RADIOUS] = (UINT32*)&radious_gc2053_evb;
    iq_param_addr[IQ_CFAINTER	] = (UINT32*)&cfa_gc2053_evb;
	iq_param_addr[IQ_GDC_FOV	] = (UINT32*)&fov_gc2053_evb;
    iq_param_addr[IQ_GDC_GLUT	] = (UINT32*)ldc_g_lut_gc2053_evb;
    iq_param_addr[IQ_GDC_RLUT	] = (UINT32*)ldc_r_lut_gc2053_evb;
    iq_param_addr[IQ_GDC_BLUT	] = (UINT32*)ldc_b_lut_gc2053_evb;
	iq_param_addr[IQ_DCE_CA	    ] = (UINT32*)&cac_gc2053_evb;

	//***********
	//**  IPE  **
	//***********
	iq_param_addr[IQ_EDGEKER    ] = (UINT32 *)edge_kernel[0];
	iq_param_addr[IQ_DIR_TAB    ] = (UINT32 *)edir_tab[0];
	iq_param_addr[IQ_DIREDGE_EXT] = (UINT32 *)&edir_ext[0];
	iq_param_addr[IQ_DIRSCORETAB] = (UINT32 *)&score_tab[0];
	iq_param_addr[IQ_DIRSCORETH ] = (UINT32 *)&score_th[0];
	iq_param_addr[IQ_EWA        ] = (UINT32 *)&ewa[0];
	iq_param_addr[IQ_EWB        ] = (UINT32 *)&ewb[0];
	iq_param_addr[IQ_EWPARAM    ] = (UINT32 *)&ewparam[0];
	iq_param_addr[IQ_EDGEENH    ] = (UINT32 *)&edge_enh[0];
	iq_param_addr[IQ_EDMAP      ] = (UINT32 *)&ed_map[0];
	iq_param_addr[IQ_EDTAB      ] = (UINT32 *)ed_tab[0];
	iq_param_addr[IQ_ESMAP      ] = (UINT32 *)&es_map[0];
	iq_param_addr[IQ_ESTAB      ] = (UINT32 *)es_tab[0];
	iq_param_addr[IQ_RGBLPF     ] = (UINT32 *)&lpf[0];
    iq_param_addr[IQ_CCM_TH     ] = (UINT32*)&color_temperature_th_gc2053_evb;
	iq_param_addr[IQ_Color_L    ] = (UINT32*)&color_low_gc2053_evb;
	iq_param_addr[IQ_Color_M    ] = (UINT32*)&color_middle_gc2053_evb;
	iq_param_addr[IQ_Color_H    ] = (UINT32*)&color_high_gc2053_evb;
    iq_param_addr[IQ_FSTAB      ] = (UINT32*)fs_tab_gc2053_evb[0];
	iq_param_addr[IQ_FDTAB	    ] = (UINT32*)fd_tab_gc2053_evb[0];
    iq_param_addr[IQ_CC         ] = (UINT32*)&cc_gc2053_evb;
    iq_param_addr[IQ_Y_CURVE    ] = (UINT32*)y_curve_gc2053_evb;
	iq_param_addr[IQ_GAMMA_TH	] = (UINT32*)&gamma_lv_th_gc2053_evb;
    iq_param_addr[IQ_GAMMA_DAY	] = (UINT32*)gamma_day_gc2053_evb;
	iq_param_addr[IQ_GAMMA_NIGHT] = (UINT32*)gamma_night_gc2053_evb;
	iq_param_addr[IQ_3DCC_TAB   ] = (UINT32*)color_3d_lut_gc2053_evb;
    iq_param_addr[IQ_3DCC_ROUND ] = (UINT32*)&color_3d_gc2053_evb;
	iq_param_addr[IQ_EDGTAB     ] = (UINT32*)edg_tab_gc2053_evb;
    iq_param_addr[IQ_DDSTAB     ] = (UINT32*)dds_tab_gc2053_evb;
	iq_param_addr[IQ_SATURATION ] = (UINT32 *)&saturation[0];
	iq_param_addr[IQ_CONTRAST   ] = (UINT32 *)&contrast[0];
    iq_param_addr[IQ_CCON_TAB   ] = (UINT32 *)&ccon_tab[0];
	iq_param_addr[IQ_CCTRL_INFO ] = (UINT32 *)&cctrl_info[0];

	iq_param_addr[IQ_CST_COEF	] = (UINT32*)cst_coef_gc2053_evb;
    iq_param_addr[IQ_CST_PARAM	] = (UINT32*)&cst_gc2053_evb;
	iq_param_addr[IQ_YCCFIX	    ] = (UINT32*)&ycc_fix_gc2053_evb;
    iq_param_addr[IQ_NOISE_PARAM] = (UINT32*)&noise_param_gc2053_evb;
    iq_param_addr[IQ_GAMMA_RAND ] = (UINT32*)&gamma_rand_gc2053_evb;

	//***********
	//** IFE2  **
	//***********
	iq_param_addr[IQ_REFCENTER_YTH  ] = (UINT32 *)ref_center_y_th[0];
	iq_param_addr[IQ_REFCENTER_YWT  ] = (UINT32 *)ref_center_y_wt[0];
	iq_param_addr[IQ_REFCENTER_Y    ] = (UINT32 *)&ref_center_y[0];
	iq_param_addr[IQ_REFCENTER_UVTH ] = (UINT32 *)ref_center_uv_th[0];
	iq_param_addr[IQ_REFCENTER_UVWT ] = (UINT32 *)ref_center_uv_wt[0];
	iq_param_addr[IQ_REFCENTER_UV   ] = (UINT32 *)&ref_center_uv[0];
	iq_param_addr[IQ_IFE2FILTER_YTH ] = (UINT32 *)ife2filter_y_th[0];
	iq_param_addr[IQ_IFE2FILTER_YWT ] = (UINT32 *)ife2filter_y_wt[0];
	iq_param_addr[IQ_IFE2FILTER_UVTH] = (UINT32 *)ife2filter_uv_th[0];
	iq_param_addr[IQ_IFE2FILTER_UVWT] = (UINT32 *)ife2filter_uv_wt[0];
	iq_param_addr[IQ_IFE2FILTER     ] = (UINT32 *)&ife2filter[0];
	iq_param_addr[IQ_GRAY           ] = (UINT32 *)&gray_statical[0];

	//***********
	//**  IME  **
	//***********
	iq_param_addr[IQ_CHROMAADAPT        ] = (UINT32 *)&chroma_adapt[0];
	iq_param_addr[IQ_CHROMAADAPT_COLOR  ] = (UINT32 *)&chroma_adapt_color[0];
	iq_param_addr[IQ_CHROMAADAPT_LUM    ] = (UINT32 *)&chroma_adapt_lum[0];
	iq_param_addr[IQ_CHROMASUPP_WEIGHTY ] = (UINT32 *)chroma_supp_w_y[0];
	iq_param_addr[IQ_CHROMASUPP_WEIGHTC ] = (UINT32 *)chroma_supp_w_c[0];
	iq_param_addr[IQ_CHROMASUPP         ] = (UINT32 *)&chroma_supp[0];
    iq_param_addr[IQ_TMNR               ] = (UINT32 *)&tmnr_info;
    iq_param_addr[IQ_TMNR_DEBUG         ] = (UINT32 *)&tmnr_momap_sel_gc2053_evb;
    iq_param_addr[IQ_TMNR_AUTO          ] = (UINT32 *)&tmnr_auto_info_gc2053_evb;

	//***********
	//** Codec **
	//***********
	iq_param_addr[IQ_NUM_3DNR_PARAM ] = (UINT32 *)&num_3DNR_param;
	iq_param_addr[IQ_3DNR_LEVEL     ] = (UINT32 *)&tnr_level[0];
	iq_param_addr[IQ_3DNR_PARAM     ] = (UINT32 *)&tnr_param;

	//***********
	//** HDR   **
	//***********
	iq_param_addr[IQ_HDR_TH			] = (UINT32 *)&hdr_lv_th_gc2053_evb;
	iq_param_addr[IQ_HDR_HIGH		] = (UINT32 *)&hdr_high_gc2053_evb;
	iq_param_addr[IQ_HDR_MIDDLE		] = (UINT32 *)&hdr_middle_gc2053_evb;
	iq_param_addr[IQ_HDR_LOW		] = (UINT32 *)&hdr_low_gc2053_evb;

	iq_param_addr[IQ_PATHCURVE_TH		] = (UINT32 *)&pathcurve_lv_th_gc2053_evb;
	iq_param_addr[IQ_PATHCURVE_HIGH		] = (UINT32 *)&pathcurve_high_gc2053_evb;
	iq_param_addr[IQ_PATHCURVE_MIDDLE	] = (UINT32 *)&pathcurve_middle_gc2053_evb;
	iq_param_addr[IQ_PATHCURVE_LOW		] = (UINT32 *)&pathcurve_low_gc2053_evb;
	iq_set_param_addr(id,mode_idx, iq_param_addr);
}

#endif
