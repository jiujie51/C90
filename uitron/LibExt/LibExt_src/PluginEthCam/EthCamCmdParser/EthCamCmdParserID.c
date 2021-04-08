#include "SysKer.h"
#include "EthCamCmdParser.h"
#include "EthCamCmdParserInt.h"

UINT32 FLG_ID_ETHCAMCMD[ETHCAM_PATH_ID_MAX] = {0};
UINT32 ETHCAMCMD_SEM_ID[ETHCAM_PATH_ID_MAX] = {0};
UINT32 ETHCAMSTR_SEM_ID[ETHCAM_PATH_ID_MAX] = {0};

void EthCamCmd_InstallID(UINT32 path_cnt)
{
	UINT32 i;
	for(i=0;i<path_cnt;i++){
		OS_CONFIG_FLAG(FLG_ID_ETHCAMCMD[i]);
		OS_CONFIG_SEMPHORE(ETHCAMCMD_SEM_ID[i], 0, 1, 1);
		OS_CONFIG_SEMPHORE(ETHCAMSTR_SEM_ID[i], 0, 1, 1);
	}
}

