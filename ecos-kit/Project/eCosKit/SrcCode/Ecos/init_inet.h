#ifndef _INIT_INET_H
#define _INIT_INET_H
#include <net/dhcpelios/dhcpelios.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <net/if.h>

#include "dhcp.h"
#include "Type.h"

#if (defined(_NVT_ETHREARCAM_CAPS_COUNT_2_))
#define ETH_REARCAM_CAPS_COUNT   2
#else
#define ETH_REARCAM_CAPS_COUNT   1
#endif

#if (defined(_NVT_ETHREARCAM_RX_) )
#define ETHCAM_USE_DHCP   ENABLE
#elif (defined(_NVT_ETHREARCAM_TX_) )
#if(ETH_REARCAM_CAPS_COUNT==1)
#define ETHCAM_USE_DHCP   DISABLE
#else
#define ETHCAM_USE_DHCP   ENABLE
#endif
#else
#define ETHCAM_USE_DHCP   DISABLE
#endif


void init_inet(void);
void init_eth0(void);
void exit_eth0(void);
int ethcam_check_path_id(char* ip_addr);
void EthcamBoot_Dhcp(char *cmd);
void EthcamCpu1IpcReady(char *cmd);
#if (defined(_NVT_ETHREARCAM_RX_))
void ethcam_notify_ethhub_conn(char *cmd);
void EthCam_PortReadyCheck(char* cmd);
#endif
#if (defined(_NVT_ETHREARCAM_TX_))
void ethcam_set_tx_ipaddr(char *cmd);
void ethcam_get_chipid(char *cmd);
#endif

#if (defined(_NVT_ETHREARCAM_TX_))
void EthCam_Dhcp_Client_Stop(void);
void EthCam_Dhcp_Client_Start(char *pName, dhcp_cb_func pFunc, bool forceStopIfPrvExist);
void EthCam_Dhcp_Client_Cb(enum dhcp_cb_type cbType, unsigned int p1, unsigned int p2, unsigned int p3);
#endif
#if (defined(_NVT_ETHREARCAM_RX_))
bool EthCam_Dhcp_Set_Interface(char *pIntf);
void EthCam_Dhcp_Server_Start(char *pName);
void EthCam_Dhcp_Server_Stop(bool uninitIP);
#endif

void EthcamIperf_CmdLine(char *szCmd);

#endif
