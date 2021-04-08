#ifndef _IQ_PARAM_COMMON_GC2053_EVB_H_
#define _IQ_PARAM_COMMON_GC2053_EVB_H_

#include "iq_common.h"

extern ISOMAP iso_map_tab_gc2053_evb[IQS_ISOMAPTAB_NUM];
extern DIST dist_gc2053_evb;
extern Fact_Norm   radious_gc2053_evb;
extern DCE_CFAPARAM cfa_gc2053_evb;
extern FOV fov_gc2053_evb;
extern UINT32 ldc_g_lut_gc2053_evb[65];
extern INT32 ldc_r_lut_gc2053_evb[65];
extern INT32 ldc_b_lut_gc2053_evb[65];
extern Aberation cac_gc2053_evb;
extern IQS_COLOR_TEMPERATURE_TH color_temperature_th_gc2053_evb;
extern IQS_COLOR_TEMPERATURE_PARAM color_low_gc2053_evb;
extern IQS_COLOR_TEMPERATURE_PARAM color_middle_gc2053_evb;
extern IQS_COLOR_TEMPERATURE_PARAM color_high_gc2053_evb;
extern UINT8 fs_tab_gc2053_evb[TOTAL_GAIN_NUM][16];
extern UINT8 fd_tab_gc2053_evb[TOTAL_GAIN_NUM][16];
extern IPE_CC cc_gc2053_evb;
extern UINT32 y_curve_gc2053_evb[129];
extern IQS_GAMMA_LV_TH gamma_lv_th_gc2053_evb;
extern UINT32 gamma_day_gc2053_evb[129];
extern UINT32 gamma_night_gc2053_evb[129];
extern UINT8 edg_tab_gc2053_evb[24];
extern UINT8 dds_tab_gc2053_evb[8];
extern INT16 cst_coef_gc2053_evb[9];
extern IPE_CST cst_gc2053_evb;
extern IPE_YCCFIX ycc_fix_gc2053_evb;
extern IPE_RAND_NR noise_param_gc2053_evb;
extern IPE_GamRand gamma_rand_gc2053_evb ;
extern UINT32 color_3d_lut_gc2053_evb[900];
extern IPE_3DCCRound color_3d_gc2053_evb;
extern IQS_SHDR_EVRATIO_TH hdr_lv_th_gc2053_evb;
extern SHDR_IQ_PARA hdr_high_gc2053_evb;
extern SHDR_IQ_PARA hdr_middle_gc2053_evb;
extern SHDR_IQ_PARA hdr_low_gc2053_evb;
extern IQS_PATHCURVE_LV_TH pathcurve_lv_th_gc2053_evb;
extern UINT32 pathcurve_high_gc2053_evb[33];
extern UINT32 pathcurve_middle_gc2053_evb[33];
extern UINT32 pathcurve_low_gc2053_evb[33];
extern S_DEFOG_LIB_INIT_PARA defog_init_param_gc2053_evb;
extern UINT32 defog_wet_gc2053_evb[17];
extern IME_TMNR_DISPMOMAP tmnr_momap_sel_gc2053_evb;
extern TMNR_AUTO_INFO tmnr_auto_info_gc2053_evb;

extern void iq_param_gc2053_evb_table1_init(UINT32 id,UINT32 mode_idx);

#endif
