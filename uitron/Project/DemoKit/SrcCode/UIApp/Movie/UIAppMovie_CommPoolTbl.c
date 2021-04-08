#include "SysCfg.h" //for memory POOL_ID
#include "nvtmpp.h"

#define TEST_510_64MB       DISABLE
//#define VDODEC_H264_CODEC_SIZE					0x6E0000		///< H264 Media Plug-in internal used memory size
#define VDODEC_H264_CODEC_SIZE					0x240000		///< H264 Media Plug-in internal used memory size
//#define VDODEC_H265_CODEC_SIZE					0x6E0000		///< H265 Media Plug-in internal used memory size
#define VDODEC_H265_CODEC_SIZE					0x240000		///< H265 Media Plug-in internal used memory size

#if MOVIE_COMMPOOL_AUTO_SETTING

NVT_VB_CPOOL_S g_movie_common_pool_tbl[NVTMPP_VB_MAX_COMM_POOLS+1] = {0};

#else

#if (SENSOR_CAPS_COUNT == 1 && _BOARD_DRAM_SIZE_ > 0x04000000 && (ETH_REARCAM_CAPS_COUNT < 2))
NVT_VB_CPOOL_S g_movie_common_pool_tbl[NVTMPP_VB_MAX_COMM_POOLS+1]= {
#if (defined(_NVT_ETHREARCAM_RX_))
	//{ ((1920 * 3 / 4) * 1080 * 3 / 2) + 1024, 4, NVTMPP_DDR_1 }, // record		size = 0x239C80--------0
	{ (1920 * 1080 * 3 / 2) + 1024, 4, NVTMPP_DDR_1 }, // record		size =  0x2F7A00--------0
#else
	{ 1920 * 1080 * 3 / 2 + 1024, 4, NVTMPP_DDR_1 }, // record					size = 0x2F7A00
#endif
#if defined(_Disp_IFPA_LCD1_MD090TL01_)
	{ 1280 *  320 * 3 / 2 + 1024, 5, NVTMPP_DDR_1 }, // display					size = 0x96400--------1
#elif defined(_Disp_IFPA_LCD1_LT8918_WTL096601G03_)
	{  1280 *  320 * 3 / 2 + 1024, 4, NVTMPP_DDR_1 }, // display				size = 0x96400
#else
	{  720 *  576 * 3 / 2 + 1024, 4, NVTMPP_DDR_1 }, // display					size = 0x98200
#endif
#if (defined(_NVT_ETHREARCAM_RX_))
	#if (defined(_Disp_IFPA_LCD1_LT8918_WTL096601G03_) || defined(_Disp_IFPA_LCD1_MD090TL01_))
	{  1280 *  320 * 3 / 2 + 1024, 6, NVTMPP_DDR_1 }, // for ethcam wifi		size = 0x96400 --------2
	#else
	{  720 *  576 * 3 / 2 + 1024, 6, NVTMPP_DDR_1 }, // for ethcam wifi			size = 0X98200
	#endif
	{  ALIGN_CEIL_64(1280) *  ALIGN_CEIL_64(720) * 3 / 2 *4 + VDODEC_H264_CODEC_SIZE+ 1024 , 1, NVTMPP_DDR_1 }, // for VdoDec	size = 0X7E0400------3
#endif
	#if (_ANFA_ADAS_FUNC_ == ENABLE)
	{  640 *  360 * 3 / 2 + 1024, 4, NVTMPP_DDR_1 }, // 2 wifi					size = 0x54A00 
	#else
	{  848 *  480 * 3 / 2 + 1024, 5, NVTMPP_DDR_1 }, // 2 wifi					size = 0x95500 --------4
	#endif
	#if _TSR_FUNC_|| MOVIE_OT_FUNC
	{ 1920 * 1080 * 3 / 2 + 1024, 3, NVTMPP_DDR_1 }, // 2 ALG					size = 0X2F7A00
	#elif(_ANFA_ADAS_FUNC_ == ENABLE)
	{ 1920 * 1080 * 3 / 2 + 1024, 3, NVTMPP_DDR_1 }, // 2 ALG					size = 0X2F7A00 ---------5
	{  320 *  240 + 1024, 3, NVTMPP_DDR_1 }, // 2 ALG
	#else	//kevin_mio  if (AV_LCA_FUNC == ENABLE)
	{  848 *  480 * 3 / 2 + 1024, 3, NVTMPP_DDR_1 }, // 2 ALG					size = 0X95500
	#endif
	#if (_ADAS_FUNC_ == ENABLE)
	{  (720 *  480 + 1024) *2,    4, NVTMPP_DDR_1 }, // ADAS					size = 0XA9400 --------6
	#endif
	// last item should be 0
	{0, 0, NVTMPP_DDR_1}
};

#endif


#if (defined(_NVT_ETHREARCAM_RX_) && (_BOARD_DRAM_SIZE_ > 0x04000000) && (ETH_REARCAM_CAPS_COUNT >= 2))
NVT_VB_CPOOL_S g_movie_common_pool_tbl[NVTMPP_VB_MAX_COMM_POOLS+1]= {
#if !defined(_SENSOR1_CMOS_OFF_)
	{ 1920 * 1080 * 3 / 2 + 1024, 2, NVTMPP_DDR_1 }, // record
#endif
#if defined(_Disp_IFPA_LCD1_LT8918_WTL096601G03_)
	{ 1280 * 720 * 3 / 2 + 1024, 3, NVTMPP_DDR_1 }, // for crop
	{  1280 *  320 * 3 / 2 + 1024, 10, NVTMPP_DDR_1 }, // display
#else
	{  960 *  240 * 3 / 2 + 1024, 10, NVTMPP_DDR_1 }, // display
#endif
	{  848 *  480 * 3 / 2 + 1024, 5, NVTMPP_DDR_1 }, // 2 wifi

	{  ALIGN_CEIL_64(1920) *  ALIGN_CEIL_64(1080) * 3 / 2 *4 + VDODEC_H264_CODEC_SIZE+ 1024 , 1, NVTMPP_DDR_1 }, // for VdoDec

	{  ALIGN_CEIL_64(1920) *  ALIGN_CEIL_64(1080) * 3 / 2 *4 + VDODEC_H264_CODEC_SIZE+ 1024 , 1, NVTMPP_DDR_1 }, // for VdoDec

	#if _TSR_FUNC_|| MOVIE_OT_FUNC
	{ 1920 * 1080 * 3 / 2 + 1024, 3, NVTMPP_DDR_1 }, // 2 ALG
	#else
	{  320 *  240 * 3 / 2 + 1024, 3, NVTMPP_DDR_1 }, // 2 ALG
	#endif
	#if (_ADAS_FUNC_ == ENABLE)
	//{  (720 *  480 + 1024) *2,    4, NVTMPP_DDR_1 }, // ADAS
	#endif
	// last item should be 0
	{0, 0, NVTMPP_DDR_1}
};

#endif

#if (SENSOR_CAPS_COUNT == 2 && _BOARD_DRAM_SIZE_ > 0x04000000)
NVT_VB_CPOOL_S g_movie_common_pool_tbl[NVTMPP_VB_MAX_COMM_POOLS+1]= {
	{ 1920 * 1080 * 3 / 2 + 1024, 7, NVTMPP_DDR_1 }, // record
	{  720 *  576 * 3 / 2 + 1024, 4, NVTMPP_DDR_1 }, // display
	{  848 *  480 * 3 / 2 + 1024, 4, NVTMPP_DDR_1 }, // 2 wifi

#if (defined(_NVT_ETHREARCAM_RX_))
	#if defined(_Disp_IFPA_LCD1_LT8918_WTL096601G03_)
	{  1280 *  320 * 3 / 2 + 1024, 6, NVTMPP_DDR_1 }, // for ethcam wifi
	#else
	{  720 *  576 * 3 / 2 + 1024, 6, NVTMPP_DDR_1 }, // for ethcam wifi
	#endif

	{  ALIGN_CEIL_64(1920) *  ALIGN_CEIL_64(1080) * 3 / 2 *4 + VDODEC_H264_CODEC_SIZE+ 1024 , 1, NVTMPP_DDR_1 }, // for VdoDec
#endif

	// last item should be 0
	{0, 0, NVTMPP_DDR_1}
};
#endif
#if (TEST_510_64MB == ENABLE)
NVT_VB_CPOOL_S g_movie_common_pool_tbl[NVTMPP_VB_MAX_COMM_POOLS+1]= {
	{ 1920 * 1080 * 3 / 2 + 1024, 4, NVTMPP_DDR_1 },
	{  720 *  576 * 3 / 2 + 1024, 4, NVTMPP_DDR_1 },
	{  640 *  480 * 3 / 2 + 1024, 3, NVTMPP_DDR_1 },
	// last item should be 0
	{0, 0, NVTMPP_DDR_1}
};
#endif

#if (_BOARD_DRAM_SIZE_ == 0x04000000)
#if(defined(_NVT_ETHREARCAM_TX_))
#if defined(_MODEL_MIO_ETHCAM_TX_)
NVT_VB_CPOOL_S g_movie_common_pool_tbl[NVTMPP_VB_MAX_COMM_POOLS+1]= {
	#if (ETHCAM_TX_DISPLAY_FLIP == ENABLE)
	{ 1920 * 1080 * 3 / 2 + 1024, 5, NVTMPP_DDR_1 }, // record
	#else
	{ ((1920 * 3 / 4) * 1080 * 3 / 2) + 1024, 4, NVTMPP_DDR_1 }, // record
	#endif
	{  848 *  480 * 3 / 2 + 1024, 4, NVTMPP_DDR_1 }, // display
	{ 1920 *  384 * 3 / 2 + 1024, 4, NVTMPP_DDR_1 }, // clone
	#if (AV_LCA_FUNC == ENABLE)
	{ 1280 *  720 * 3 / 2 + 1024, 3, NVTMPP_DDR_1 }, // ALG
	#endif
	// last item should be 0
	{0, 0, NVTMPP_DDR_1}
};
#else
NVT_VB_CPOOL_S g_movie_common_pool_tbl[NVTMPP_VB_MAX_COMM_POOLS+1]= {
	#if (ETHCAM_TX_DISPLAY_FLIP == ENABLE)
	{ 1920 * 1080 * 3 / 2 + 1024, 5, NVTMPP_DDR_1 }, // record
	#else
	{ ((1920* 3 /4) * 1080 * 3 / 2) + 1024, 4, NVTMPP_DDR_1 }, // record
	#endif
	{  960 *  180 * 3 / 2 + 1024, 6, NVTMPP_DDR_1 }, // display
	{ 1280 *  720 * 3 / 2 + 1024, 4, NVTMPP_DDR_1 }, // clone
	#if (AV_LCA_FUNC == ENABLE)
	{ 1280 *  720 * 3 / 2 + 1024, 3, NVTMPP_DDR_1 }, // ALG
	#endif
	// last item should be 0
	{0, 0, NVTMPP_DDR_1}
};
#endif
#else
NVT_VB_CPOOL_S g_movie_common_pool_tbl[NVTMPP_VB_MAX_COMM_POOLS+1]= {
	{ ((1920* 3 /4) * 1080 * 3 / 2) + 1024, 4, NVTMPP_DDR_1 }, // record
	{  960 *  180 * 3 / 2 + 1024, 6, NVTMPP_DDR_1 }, // display
	{  848 *  480 * 3 / 2 + 1024, 5, NVTMPP_DDR_1 }, // wifi
	// last item should be 0
	{0, 0, NVTMPP_DDR_1}
};
#endif
#endif

#endif // MOVIE_COMMPOOL_AUTO_SETTING
