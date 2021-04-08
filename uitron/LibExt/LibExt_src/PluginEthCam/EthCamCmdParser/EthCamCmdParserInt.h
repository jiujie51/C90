#ifndef _ETHCAMCMDPARSERINT_H_
#define _ETHCAMCMDPARSERINT_H_

#include "SysKer.h"
#include "EthCamSocket.h"

///////////////////////////////////////////////////////////////////////////////
#define __MODULE__          EthCamCmdParser
#define __DBGLVL__          2 // 0=FATAL, 1=ERR, 2=WRN, 3=UNIT, 4=FUNC, 5=IND, 6=MSG, 7=VALUE, 8=USER
#define __DBGFLT__          "*" //*=All, [mark]=CustomClass
#include "DebugModule.h"
///////////////////////////////////////////////////////////////////////////////
extern UINT32 _SECTION(".kercfg_data") FLG_ID_ETHCAMCMD[ETHCAM_PATH_ID_MAX];
extern UINT32 _SECTION(".kercfg_data") ETHCAMCMD_SEM_ID[ETHCAM_PATH_ID_MAX];
extern UINT32 _SECTION(".kercfg_data") ETHCAMSTR_SEM_ID[ETHCAM_PATH_ID_MAX];

#endif //_ETHCAMCMDPARSERINT_H_

