#ifndef _ETHCAMAPPNET_H_
#define _ETHCAMAPPNET_H_
#include "EthCamSocket.h"

//#define ETHCAM_LINK_DOWN          (0x01)          //< link down
//#define ETHCAM_LINK_UP           	 (0x02)          //< link up

extern ETHCAM_SOCKET_INFO SocketInfo[];

extern UINT32 EthCamNet_GetEthLinkStatus(ETHCAM_PATH_ID path_id);
extern UINT32 EthCamNet_GetPrevEthLinkStatus(ETHCAM_PATH_ID path_id);
extern void EthCamNet_SetPrevEthLinkStatus(ETHCAM_PATH_ID path_id, UINT32 LinkStatus);
extern void EthCamNet_EthLinkStatusNotify(char *cmd);
extern void EthCamNet_SrvCliConnIPAddrNofity(char *cmd);
extern void EthCamNet_SetTxIPAddr(void);
extern void EthCamNet_SetTxHB(UINT32 value);
extern UINT32 EthCamNet_GetTxHB(void);
extern UINT32 EthCamNet_GetEthHub_LinkStatus(ETHCAM_PATH_ID path_id);
extern void EthCamNet_EthHubLinkDet(void);
extern UINT32 ipstr2int(char* ip_addr);
extern void EthCamNet_Ethboot(void);
extern void EthCamNet_UDPSocketOpen(void);
extern void EthCamNet_UDPSocketClose(void);
extern void EthCamNet_EthLinkRetry(void);
extern UINT32 EthCamNet_GetDhcpSrvConnIpAddr(void);
extern void EthCamNet_SetSysWdtReset(UINT32 uMS);
extern void EthCamNet_CheckVDCount(void);
extern void EthCamNet_IperfTest(void);
extern void EthCamNet_LinkDetStreamRestart(UINT32 pathid);
extern void EthCamNet_EthHubPortIsolate(UINT32 port_id, BOOL port_isolate);
extern void EthCamNet_EthHubChkPortReadyTimerOpen(void);
extern void EthCamNet_EthHubChkPortReadyTimerClose(void);
extern void EthCamNet_EthHubPortReadySendCmdTimerOpen(void);
extern void EthCamNet_EthHubPortReadySendCmdTimerClose(void);
extern void EthCamNet_EthHubChkPortReady(void);
extern void  EthCamNet_DetConnInit(void);
extern BOOL EthCamNet_IsIPConflict(void);
#endif //_ETHCAMAPPNET_H_
