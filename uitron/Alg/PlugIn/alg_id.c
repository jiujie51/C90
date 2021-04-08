#include "SysKer.h"
#include "SysCommon.h"
#include "alg_id.h"
#include "CvHost_lib.h"
#include "NvtCv_lib.h"

UINT32 AlgPullIsfDataSemiID[MOVIE_ALG_IPL_MAX];

#if (_ADAS_FUNC_ == ENABLE)
#include "adas_Apps.h"
#endif
#if (_DDD_FUNC_ == ENABLE)
#include "DDD_lib.h"
#endif

#if (_TSR_FUNC_ == ENABLE)
#include "tsr_lib.h"
#endif
#if ((MOVIE_ODT_FUNC == ENABLE) || (IPCAM_ODT_FUNC == ENABLE))
#include "odt_lib.h"
#endif

#if (AV_LCA_FUNC == ENABLE)
#include "av_lca_lib.h"
#endif

#if	(_ALPHAVISION_LICENSE_ == ENABLE)
#include "Anfa_License.h"
#endif

void alg_InstallID(void)
{
#if	(_ALPHAVISION_LICENSE_ == ENABLE)
	//AnfaAlgAuthProc_InstallID();
#endif
#if (_ADAS_FUNC_ == ENABLE)
	ADAS_InstallID();
#endif

#if (_DDD_FUNC_ == ENABLE)
	DDD_InstallID();
#endif

#if (_TSR_FUNC_ == ENABLE)
	TSR_InstallID();
#endif

#if (_ANFA_ADAS_FUNC_ == ENABLE)
	Anfa_ADAS_InstallID();
#endif
#if (AV_LCA_FUNC == ENABLE)
#if(defined(_NVT_ETHREARCAM_RX_))
	AV_RAS_InstallID();
#endif
#endif

#if (_CVHOST_EX_ == ENABLE)
	CvHost_InstallID();
#endif

#if ((MOVIE_ODT_FUNC == ENABLE) || (IPCAM_ODT_FUNC == ENABLE))
	odt_installid();
#endif

#ifndef _DSP1_NONE_
	NvtCv_InstallID();
#endif

	OS_CONFIG_SEMPHORE(AlgPullIsfDataSemiID[MOVIE_ALG_IPL1_3], 0, 1, 1);
	OS_CONFIG_SEMPHORE(AlgPullIsfDataSemiID[MOVIE_ALG_IPL1_4], 0, 1, 1);
	OS_CONFIG_SEMPHORE(AlgPullIsfDataSemiID[MOVIE_ALG_IPL2_3], 0, 1, 1);
	OS_CONFIG_SEMPHORE(AlgPullIsfDataSemiID[MOVIE_ALG_IPL2_4], 0, 1, 1);
}

