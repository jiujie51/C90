#ifndef _SENSOR_DRV_H
#define _SENSOR_DRV_H

#include "Type.h"
#include "sensor.h"

extern SENSOR_DRV_TAB *sensor_get_drv_tab_ar0237irp(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_ar0237l(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_ar0237p(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_ar0330m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_ar0330p(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_ar0521m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_hm2140m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_imx225p(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_imx290m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_imx291m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_imx307m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_imx307l(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_imx317m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_imx415m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_imx323p(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_imx335m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_imx377m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_os02k10m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_os05a10m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_os04b10m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_os08a10m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_ov2715m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_ov2732m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_ov2735m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_ov4689m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_ov4689m_r2a(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_ov5658m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_ps5250m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_f23m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_jxk02m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_jxk03m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_sc2232m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_sc2235p(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_sc2310m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_sc2315p(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_sc3235m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_sc4210m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_sc4236m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_sc4236m_24m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_sc4238m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_sc4335m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_sc5035m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_sc5235m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_sc5236m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_sc5238m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_sc5335m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_gc2053m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_gc2053p(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_gc4623m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_siepatgen(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_tp2825(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_tp2827p(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_pr2020k(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_nvp6124b(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_f37p(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_f37m(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_sp2307lqj(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_gm7150(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_tvp5150(void);
extern SENSOR_DRV_TAB *sensor_get_drv_tab_ad_std_drv_n4(void); //use sensor_get_drv_tab_ad_std_drv_xxx (xxx is the MODEL_NAME in the Makefile of sensor driver)
extern SENSOR_DRV_TAB *sensor_get_drv_tab_ad_std_drv_pr2020k(void); //use sensor_get_drv_tab_ad_std_drv_xxx (xxx is the MODEL_NAME in the Makefile of sensor driver)
extern SENSOR_DRV_TAB *sensor_get_drv_tab_ad_std_drv_pr2000(void); //use sensor_get_drv_tab_ad_std_drv_xxx (xxx is the MODEL_NAME in the Makefile of sensor driver)
#endif //_SENSOR_DRV_H
