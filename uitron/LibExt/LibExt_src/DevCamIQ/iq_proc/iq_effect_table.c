#include "iq_common.h"

                                                    //CbOfs CrOfs YCon CCon IntOfs SatOfs HueRotate
IQS_COLOR_EFFECT_PARAM ColorEffectTable_Normal       = {128, 128,  128, 128,    0,     0,   FALSE };    //SEL_IMAGEEFFECT_OFF = 0,
IQS_COLOR_EFFECT_PARAM ColorEffectTable_BW           = {128, 128,  128,   0,    0,     0,   FALSE };    //SEL_IMAGEEFFECT_BW,
IQS_COLOR_EFFECT_PARAM ColorEffectTable_SEPIA        = {116, 144,  128,   0,    0,     0,   FALSE };    //SEL_IMAGEEFFECT_SEPIA,
IQS_COLOR_EFFECT_PARAM ColorEffectTable_VIVID        = {128, 128,  128,  128,   0,    50,   FALSE };    //SEL_IMAGEEFFECT_VIVID,
IQS_COLOR_EFFECT_PARAM ColorEffectTable_PencilSketch = {128, 128,  224,    0,   0,  -128,   FALSE };    //SEL_IMAGEEFFECT_COLORPENCIL,
IQS_COLOR_EFFECT_PARAM ColorEffectTable_ColorPencil  = {128, 128,  224,  128,   0,     0,   FALSE };    //SEL_IMAGEEFFECT_SKETCH,
IQS_COLOR_EFFECT_PARAM ColorEffectTable_CoolGreen    = {128,  64,  128,  128,   0,     0,   FALSE };    //SEL_IMAGEEFFECT_SKETCH,
IQS_COLOR_EFFECT_PARAM ColorEffectTable_WarmYellow   = {64,  160,  128,  128,   0,     0,   FALSE };    //SEL_IMAGEEFFECT_SKETCH,

//////////////////////////////////
// Color pencil
//////////////////////////////////
IPE_YCCFIX IE_ColorPencil_Param =
{
  //yth1{YTH,  ETH,  HIT1SEL,NHIT1SEL,HIT,NHIT},  yth2{YTH, HIT2SEL,NHIT2SEL,HIT,NHIT}, CC{ETH, YTG, CBTH,  CRTH, HITSEL,NHITSEL,CB_HIT,CB_NHIT,CR_HIT,CR_NHIT}
        {0,     0,   ENABLE,  DISABLE,  128, 0},      {0,    FALSE,  FALSE,    0,   0},   {0,   0,0,  0,0,  0,0,  FALSE,  FALSE,     0,      0,     0,     0},
};

//////////////////////////////////
// Sketch
//////////////////////////////////
IPE_YCCFIX IE_PencilSketch_Param =
{
  //yth1{YTH,  ETH,  HIT1SEL,NHIT1SEL,HIT,NHIT},  yth2{YTH, HIT2SEL,NHIT2SEL,HIT,NHIT}, CC{ETH, YTG, CBTH,  CRTH, HITSEL,NHITSEL,CB_HIT,CB_NHIT,CR_HIT,CR_NHIT}
        {0,    0,   ENABLE,  DISABLE,  128, 0},      {0,    FALSE,  FALSE,    0,   0},   {0,   0,0,  0,0,  0,0,  FALSE,  FALSE,     0,      0,     0,     0},
};

///////////////////////
// IPP Rock
///////////////////////
IPE_YCCFIX IPE_RockEffect_YccFix =
{
	//yth1{YTH,  ETH,  HIT1SEL,NHIT1SEL,HIT,NHIT},  yth2{YTH, HIT2SEL,NHIT2SEL,HIT,NHIT}, CC{ETH, YTG, CBTH,  CRTH, HITSEL,NHITSEL,CB_HIT,CB_NHIT,CR_HIT,CR_NHIT}
	{0,      0,    FALSE,  FALSE,    0,   0},    {128,   TRUE,  TRUE,    112,   0},   {0,  128, 255, 0, 255, 0, 255, TRUE,  TRUE,    165,    128,   235,    128} // Pink
    //{0,      0,    FALSE,  FALSE,    0,   0},    {128,   TRUE,  TRUE,    192,   0},   {0,  128,255,0,255,0,255, TRUE,  TRUE,    64,    128,   160,    128} // Yellow
};

///////////////////////////////
// DCE_Init
///////////////////////////////
UINT32 IQS_DCE_GLUT_Init[65] =
{
	65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,
	65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,
	65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,
	65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,
	65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,
	65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,
	65535, 65535, 65535, 65535, 65535
};

//////////////////////////////////
// Fisheye
//////////////////////////////////
UINT32 IE_FISYEYE_LUT[65] =
{
	55705, 55933, 56161, 56390, 56617, 56842, 57065, 57286, 57503, 57717,
	57927, 58133, 58335, 58533, 58726, 58915, 59100, 59281, 59458, 59631,
	59801, 59967, 60130, 60291, 60449, 60605, 60760, 60913, 61064, 61215,
	61365, 61515, 61666, 61816, 61967, 62118, 62270, 62423, 62577, 62732,
	62888, 63045, 63203, 63361, 63520, 63678, 63836, 63993, 64148, 64301,
	64450, 64596, 64737, 64872, 65000, 65119, 65228, 65326, 65411, 65481,
	65534, 65535, 65535, 65535, 65535
};

//////////////////
// IFE-Vignette
//////////////////
IQS_VIG_SET g_vig_init =
{
    VIG_TAB_GAIN_2X, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

///////////////////////
// Selective CC
///////////////////////
INT32 IPE_SelectiveCC_SatTab[15][24] =
{
    //  0                        5                       10                       15                       20
	{   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},          //CCID_NO_EFFECT,
	{  99,  99,  99,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  99},          //CCID_ENHANCE_RED,
	{   0,   0,   0,  99,  99,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},          //CCID_ENHANCE_YELLOW,
	{   0,   0,   0,   0,   0,   0,   0,  99,  99,  99,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},          //CCID_ENHANCE_GREEN,
	{   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  99,  99,  99,   0,   0,   0,   0,   0,   0,   0},          //CCID_ENHANCE_BLUE,
	{   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  99,  99,   0,   0,   0,   0,   0,   0,   0,   0,   0},          //CCID_ENHANCE_CYAN,
	{   0,   0,  99,  99,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},          //CCID_ENHANCE_ORANGE,
	{   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  99,  99,  99,   0},          //CCID_ENHANCE_MAGENTA,
	{   0,   0,   0, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128,   0}, //CCID_REMOVAL_RED,
	{ -128, -128, -128,   0,   0, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128}, //CCID_REMOVAL_YELLOW,
	{ -128, -128, -128, -128, -128, -128, -128,   0,   0,   0, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128}, //CCID_REMOVAL_GREEN,
	{ -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128,   0,   0,   0, -128, -128, -128, -128, -128, -128, -128}, //CCID_REMOVAL_BLUE,
	{ -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128,   0,   0, -128, -128, -128, -128, -128, -128, -128, -128, -128}, //CCID_REMOVAL_CYAN,
	{ -128, -128,   0,   0, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128}, //CCID_REMOVAL_ORANGE,
	{ -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128,   0,   0,   0, -128}, //CCID_REMOVAL_MAGENTA

};

UINT32 IE_Gamma_Negative[129] =
{
    1023 ,1003 ,967 ,932 ,899 ,869 ,835 ,800 ,768 ,728 ,687 ,648 ,612 ,576 ,542 ,510 ,483 ,467 ,452 ,
    437 ,422 ,406 ,385 ,366 ,346 ,327 ,309 ,292 ,278 ,264 ,251 ,237 ,225 ,212 ,200 ,188 ,179 ,170 ,
    162 ,153 ,145 ,137 ,128 ,121 ,113 ,106 ,100 ,93 ,88 ,82 ,76 ,70 ,64 ,59 ,54 ,49 ,44 ,
    38 ,34 ,31 ,29 ,28 ,26 ,25 ,23 ,22 ,20 ,20 ,19 ,18 ,17 ,17 ,16 ,16 ,15 ,14 ,
    14 ,13 ,13 ,12 ,12 ,11 ,11 ,10 ,10 ,9 ,9 ,8 ,8 ,8 ,7 ,7 ,7 ,7 ,6 ,6 ,6 ,6 ,6 ,5 ,5 ,5 ,5 ,5 ,
    4 ,4 ,4 ,4 ,4 ,3 ,3 ,3 ,3 ,3 ,2 ,2 ,2 ,2 ,2 ,2 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,0 ,0
};

IPE_EethParam g_ipe_eth =
{
    SEL_ETH_ORG, ETH_2BITS, 28, 48, 128, 0, 0
};

///////////////////////////////
// RGB-IR parameter
///////////////////////////////
UINT32 ir_level_lut[16] = { 128, 140, 152, 160, 168, 176, 184, 192, 200, 208, 216, 224, 232, 240, 248, 256};
UINT32 ir_sub_b_lut[16] = { 230, 230, 230, 184, 147, 105, 87, 75, 60, 48, 38, 30, 24, 19, 0, 0};
UINT32 ir_sub_rg_lut[16] = { 204, 204, 204, 163, 131, 104, 83, 67, 53, 42, 34, 27, 21, 17, 0, 0};
UINT32 ir_sat_gain_lut[16] = { 256, 312, 512, 485, 423, 382, 353, 332, 316, 304, 294, 287, 281, 276, 256, 256};

