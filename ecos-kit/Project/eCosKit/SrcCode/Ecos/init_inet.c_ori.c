#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//#include <sys/socket.h>
//#include <sys/ioctl.h>
//#include <sys/errno.h>
//#include <netinet/in.h>
//#include <net/if.h>
//#include <dhcp.h>
#include <network.h>
#include "init_inet.h"
#include <cyg/io/nvtimeth.h>
#include <netinet/if_ether.h>
#include "init_ipc.h"
#include <cyg/nvtipc/NvtIpcAPI.h>
#if (defined(_NVT_ETHREARCAM_TX_) || defined(_NVT_ETHREARCAM_RX_))
#include <cyg/ethsocket/ethsocket.h>
#endif
#include <cyg/io/nvtimeth.h>

#if (defined(_NVT_ETHREARCAM_TX_))
#define NET_IP_ETH0            "192.168.0.12"
#else
#define NET_IP_ETH0            "192.168.0.3"
#endif
#define NET_NETMASK_ETH0       "255.255.255.0"
#define NET_BRAODCAST_ETH0     "192.168.0.255"
#define NET_GATEWAY_ETH0       "192.168.0.3"
#define NET_SRVIP_ETH0         "192.168.0.3"
#define NET_LEASE_START_ETH0   "192.168.0.12"
#define NET_LEASE_END_ETH0     	"192.168.0.13"
#define NET_LEASE_DNS_ETH0     "0.0.0.0"
#define NET_LEASE_WINS_ETH0    "0.0.0.0"

#define UDP_PORT 2222
fix_ip_pair_t eth_ip[1] = {
	{NET_IP_ETH0, NET_NETMASK_ETH0, NET_BRAODCAST_ETH0, NET_GATEWAY_ETH0, NET_SRVIP_ETH0, 0},
};

static cyg_thread thread_s; /* space for thread object */
static char inet_stack[8192]; /* 8K stack space for the thread */
static char eth0_stack[4096]; /* 4K stack space for the thread */
static char eth0_exit_stack[4096]; /* 4K stack space for the thread */
static cyg_handle_t handle_t;  /* now the handles for the thread */
static cyg_thread thread_exit_eth0_s; /* space for thread object */
static cyg_handle_t handle_exit_eth0_t;  /* now the handles for the thread */
static cyg_thread_entry_t thread_inet; /* and now variable (prototype) for the procedure which is the thread itself */
bool gEthCamCpu1IpcReady = false;
#if (ETHCAM_USE_DHCP==ENABLE)
unsigned int gEthCamDhcpSrvIP = 0;
unsigned int gEthCamDhcpCliIP = 0;
char g_chEthCamDhcpCliIP[FIX_IP_MAX_LEN] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

bool gEthCamDhcpCliOpened = false;
bool gEthCamDhcpSrvOpened = false;
char gEthCamNvtNetHostNameCli[CYGNUM_NET_DHCP_OPTION_HOST_NAME_LEN] = "nvtdsc01cli";
char gEthCamNvtNetHostNameSrv[CYGNUM_NET_DHCP_OPTION_HOST_NAME_LEN] = "nvtdsc01srv";
dhcp_assign_ip_info gEthCamDhcpSrvConnIpInfo={0};//[ETHCAM_MAX_TX_NUM] = {0}; //for DHCP lease is 1,keep Mac of IP
bool gEthCamDhcpCliFailIP = false;
unsigned int g_ChipId_h=0;
unsigned int g_ChipId_l=0;
#endif
#if (defined(_NVT_ETHREARCAM_TX_))
char g_udp_cmd[50]={0};
int g_udp_cmdsize=50;
#endif

extern void ethlinkstatusnofity(unsigned int sts);
extern unsigned int ethlinkstatusget(void);
extern void ethlinkstatusnofity_cmd(char* cmd);
extern int eth_get_mac_address( const char *interface, char *mac_address );
extern void eth_config_mac_address(void);

static void thread_inet(cyg_addrword_t data)
{
    dhcp_set_hostname("nvtdsc01");
    init_all_network_interfaces_statically();
    cyg_thread_exit();
}
#if (defined(_NVT_ETHREARCAM_RX_))
void EthCam_SrvCliConnIPNofity(char* cmd)
{
	char ipccmd[50];
	NVTIPC_SYS_MSG sysMsg;
	NVTIPC_I32 ipcErr = 0;

	snprintf(ipccmd, sizeof(ipccmd) - 1, "%s",cmd);
	sysMsg.sysCmdID = NVTIPC_SYSCMD_SYSCALL_REQ;
	sysMsg.DataAddr = (UINT32)ipccmd;
	sysMsg.DataSize = strlen(ipccmd) + 1;
	if ((ipcErr = NvtIPC_MsgSnd(NVTIPC_SYS_QUEUE_ID, NVTIPC_SENDTO_CORE1, &sysMsg, sizeof(sysMsg))) < 0) {
		printf("Failed to NVTIPC_SYS_QUEUE_ID\r\n");
	}
}
void EthCam_SrvCheckConn(void)
{
	#if (ETH_REARCAM_CAPS_COUNT >=2)//bootup
	if(gEthCamDhcpSrvConnIpInfo.ipaddr==0){
		char udp_cmd[50]={0};
		int udp_cmdsize=50;
		udp_cmdsize=snprintf(udp_cmd, sizeof(udp_cmd) - 1, "cliconncheck");
		printf("udp cmd=%s, sz=%d\r\n",udp_cmd, udp_cmdsize);
		ethsocket_udp_sendto(NET_LEASE_START_ETH0, UDP_PORT, udp_cmd, &udp_cmdsize);
		ethsocket_udp_sendto(NET_LEASE_END_ETH0, UDP_PORT, udp_cmd, &udp_cmdsize);
	}
	#else
	if(gEthCamDhcpSrvConnIpInfo.ipaddr==0){
		char udp_cmd[50]={0};
		int udp_cmdsize=50;
		udp_cmdsize=snprintf(udp_cmd, sizeof(udp_cmd) - 1, "cliconncheck");
		printf("udp cmd=%s, sz=%d\r\n",udp_cmd, udp_cmdsize);
		ethsocket_udp_sendto(NET_LEASE_START_ETH0, UDP_PORT, udp_cmd, &udp_cmdsize);
	}
	#endif
}
#endif
#if (defined(_NVT_ETHREARCAM_RX_) || defined(_NVT_ETHREARCAM_TX_))
int udpsocket_recv(char *addr, int size)
{
	if (addr && (size > 0)) {
		addr[size] = '\0';
		printf("udp=%s ,sz=%d\r\n", addr, size);
#if (defined(_NVT_ETHREARCAM_RX_))
	char cmdname[50];
	unsigned int CliIPAddr=0;
	unsigned int CliMacAddr[2]={0};
	char chCliIPAddr[FIX_IP_MAX_LEN]={0};
	char chCliMacAddr[20]={0};

	sscanf_s(addr, "%s %d %d %d", cmdname, 50, &CliIPAddr, &CliMacAddr[0], &CliMacAddr[1]);
	if(strcmp(cmdname,"cliconnipnotify")==0){
		snprintf(chCliIPAddr, FIX_IP_MAX_LEN, "%d.%d.%d.%d", (CliIPAddr & 0xFF), (CliIPAddr >> 8) & 0xFF, (CliIPAddr >> 16) & 0xFF, (CliIPAddr >> 24) & 0xFF);
		snprintf(chCliMacAddr, 20, "%02x:%02x:%02x:%02x:%02x:%02x", (CliMacAddr[0] & 0xFF), (CliMacAddr[0] >> 8) & 0xFF, (CliMacAddr[0] >> 16) & 0xFF, (CliMacAddr[0] >> 24) & 0xFF , (CliMacAddr[1]) & 0xFF ,(CliMacAddr[1] >> 8) & 0xFF);
		printf("Get=%s ,CliIPAddr=0x%x, %s, CliMac=%s\r\n", cmdname, CliIPAddr,chCliIPAddr, chCliMacAddr);

		strcpy(cmdname, "");
		snprintf(cmdname, sizeof(cmdname) - 1, "cliconnipnotify %d %d %d",CliIPAddr,CliMacAddr[0],CliMacAddr[1]);
		#if(ETH_REARCAM_CAPS_COUNT>=2)
		EthCam_SrvCliConnIPNofity(cmdname);
		#else
		if(gEthCamDhcpSrvConnIpInfo.ipaddr==0){
			EthCam_SrvCliConnIPNofity(cmdname);
			gEthCamDhcpSrvConnIpInfo.ipaddr=CliIPAddr;
		}
		#endif
	}
#endif
#if (defined(_NVT_ETHREARCAM_TX_))
	char cmdname[50];
	sscanf_s(addr, "%s", cmdname, 50);
	if(strcmp(cmdname,"cliconncheck")==0){
		printf("Get=%s\r\n", cmdname);
		if(g_udp_cmdsize){
			printf("udp cmd=%s, sz=%d\r\n",g_udp_cmd, g_udp_cmdsize);
			ethsocket_udp_sendto(NET_SRVIP_ETH0, UDP_PORT, g_udp_cmd, &g_udp_cmdsize);
		}
	}
#endif
	}
	return true;
}
void udpsocket_notify(int status, int parm)
{
	switch (status) {
	case CYG_ETHSOCKET_UDP_STATUS_CLIENT_REQUEST: {
		}
		break;
	}
}
#endif
static void thread_eth0(cyg_addrword_t data)
{
#if (defined(_NVT_ETHREARCAM_RX_))
#if 1//(ETH_REARCAM_CAPS_COUNT>=2)
	printf("thread_eth0 DHCP ip\r\n");

	dhcp_set_fix_ip_pair((pfix_ip_pair_t)eth_ip, 1);
	init_all_network_interfaces_statically();

	//get client static ip
	ethsocket_install_obj  udpsocket_obj = {0};
	printf("open udp usocket\r\n");
	udpsocket_obj.notify = udpsocket_notify;
	udpsocket_obj.recv = udpsocket_recv;
	udpsocket_obj.portNum = UDP_PORT;
	udpsocket_obj.threadPriority = 8;
	udpsocket_obj.sockbufSize = 1024; //default 8192
	ethsocket_udp_install(&udpsocket_obj);
	ethsocket_udp_open();


	EthCam_Dhcp_Set_Interface("eth0");
	EthCam_Dhcp_Server_Start("DhcpSrv01");

	//init_all_network_interfaces();
	gEthCamDhcpSrvIP = dhcp_cli_get_server_ip();
	printf("gEthCamDhcpSrvIP=0x%x, %d.%d.%d.%d\r\n", gEthCamDhcpSrvIP, (gEthCamDhcpSrvIP & 0xFF), (gEthCamDhcpSrvIP >> 8) & 0xFF, (gEthCamDhcpSrvIP >> 16) & 0xFF, (gEthCamDhcpSrvIP >> 24) & 0xFF);
	if(ethlinkstatusget()==NVTIMETH_LINK_UP
		&& gEthCamCpu1IpcReady==true){//already set SrvCheckConn
		EthCam_SrvCheckConn();
	}
#else
	printf("thread_eth0 static ip\r\n");

	dhcp_set_fix_ip_pair((pfix_ip_pair_t)eth_ip, 1);
	init_all_network_interfaces_statically();
#endif
#endif
#if (defined(_NVT_ETHREARCAM_TX_))
#if (ETHCAM_USE_DHCP==DISABLE)
	printf("thread_eth0 static ip\r\n");
	eth_config_mac_address();

	dhcp_set_fix_ip_pair((pfix_ip_pair_t)eth_ip, 1);
	init_all_network_interfaces_statically();

#else
	const cyg_uint32 tick = 1000000/(CYGNUM_HAL_RTC_NUMERATOR/CYGNUM_HAL_RTC_DENOMINATOR);//1ms
	int chk_cnt=0;
	while(g_ChipId_h==0 || g_ChipId_l==0){
		cyg_thread_delay(tick);
		chk_cnt++;
		if((chk_cnt%100)==0){
			printf("delay!! ChipId_h=0x%x, ChipId_l=0x%x\r\n", g_ChipId_h, g_ChipId_l);
		}
	}
	eth_config_mac_address();
	if(gEthCamDhcpCliIP){
		printf("static ip=%d.%d.%d.%d\r\n", (gEthCamDhcpCliIP & 0xFF), (gEthCamDhcpCliIP >> 8) & 0xFF, (gEthCamDhcpCliIP >> 16) & 0xFF, (gEthCamDhcpCliIP >> 24) & 0xFF);
		snprintf(eth_ip[0].ip, FIX_IP_MAX_LEN, "%d.%d.%d.%d", (gEthCamDhcpCliIP & 0xFF), (gEthCamDhcpCliIP >> 8) & 0xFF, (gEthCamDhcpCliIP >> 16) & 0xFF, (gEthCamDhcpCliIP >> 24) & 0xFF);
		dhcp_set_fix_ip_pair((pfix_ip_pair_t)eth_ip, 1);
		init_all_network_interfaces_statically();


		//send static ip info to server
		ethsocket_install_obj  udpsocket_obj = {0};
		printf("open udp socket\r\n");
		udpsocket_obj.notify = udpsocket_notify;
		udpsocket_obj.recv = udpsocket_recv;
		udpsocket_obj.portNum = UDP_PORT;
		udpsocket_obj.threadPriority = 8;
		udpsocket_obj.sockbufSize = 1024; //default 8192
		ethsocket_udp_install(&udpsocket_obj);
		ethsocket_udp_open();

		char udp_cmd[50]={0};
		int udp_cmdsize=50;
		unsigned int macaddr[2]={0};
		char mac_addr[ETHER_ADDR_LEN]={0x00,0x00,0x00,0x00,0x00,0x00};
		eth_get_mac_address("eth0", mac_addr);

		macaddr[0]=(mac_addr[0] & 0xFF)+ ((mac_addr[1] & 0xFF)<<8 )+ ((mac_addr[2] & 0xFF)<<16)+ ((mac_addr[3] & 0xFF)<<24);
		macaddr[1]=(mac_addr[4] & 0xFF)+ ((mac_addr[5]  & 0xFF)<<8 );
		memset(udp_cmd, 0 ,sizeof(udp_cmd));
		udp_cmdsize=snprintf(udp_cmd, sizeof(udp_cmd) - 1, "cliconnipnotify %d %d %d",gEthCamDhcpCliIP,macaddr[0],macaddr[1]);
		printf("udp cmd=%s, sz=%d\r\n",udp_cmd, udp_cmdsize);
		strcpy(g_udp_cmd, udp_cmd);
		g_udp_cmdsize=udp_cmdsize;
		ethsocket_udp_sendto(NET_SRVIP_ETH0, UDP_PORT, udp_cmd, &udp_cmdsize);
	}else{
		printf("DHCP\r\n");
		EthCam_Dhcp_Client_Start("DhcpCli01", EthCam_Dhcp_Client_Cb, 1);
		gEthCamDhcpCliIP = dhcp_cli_get_ip();
	}
	if(gEthCamDhcpCliIP){
		snprintf(g_chEthCamDhcpCliIP, FIX_IP_MAX_LEN, "%d.%d.%d.%d", (gEthCamDhcpCliIP & 0xFF), (gEthCamDhcpCliIP >> 8) & 0xFF, (gEthCamDhcpCliIP >> 16) & 0xFF, (gEthCamDhcpCliIP >> 24) & 0xFF);
		printf("gDhcpCliIP=0x%x, %s\r\n",gEthCamDhcpCliIP,g_chEthCamDhcpCliIP);
		if(ethlinkstatusget()==NVTIMETH_LINK_UP){
			//ethlinkstatusnofity(NVTIMETH_LINK_UP);
			char ipccmd[40]={0};
			snprintf(ipccmd, sizeof(ipccmd) - 1, "ethlinknotify %d %d",gEthCamDhcpCliIP, NVTIMETH_LINK_UP);
			ethlinkstatusnofity_cmd(ipccmd);
		}
	}
#endif
#endif
    cyg_thread_exit();
}
static void thread_exit_eth0(cyg_addrword_t data)
{
#if (ETHCAM_USE_DHCP==ENABLE)//ECOS_DHCP
#if (defined(_NVT_ETHREARCAM_TX_))
    EthCam_Dhcp_Client_Stop();
#endif
#endif
    cyg_thread_exit();
}
void init_inet(void)
{
    cyg_thread_create(4, thread_inet, (cyg_addrword_t) 0,
        "init inet", (void *) inet_stack, sizeof(inet_stack),
        &handle_t, &thread_s);

    cyg_thread_resume(handle_t);
}

void init_eth0(void)
{
    cyg_thread_create(4, thread_eth0, (cyg_addrword_t) 0,
        "init inet", (void *) eth0_stack, sizeof(eth0_stack),
        &handle_t, &thread_s);

    cyg_thread_resume(handle_t);
}
void exit_eth0(void)
{
	printf("exit_eth0\r\n");
    cyg_thread_create(4, thread_exit_eth0, (cyg_addrword_t) 0,
        "exit eth0", (void *) eth0_exit_stack, sizeof(eth0_exit_stack),
        &handle_exit_eth0_t, &thread_exit_eth0_s);

    cyg_thread_resume(handle_exit_eth0_t);
}
int ethcam_check_path_id(char* ip_addr)
{
	int path_id=-1;
	if(strcmp(ip_addr,NET_LEASE_START_ETH0)==0){
		path_id=0;
	}else if(strcmp(ip_addr,NET_LEASE_END_ETH0)==0){
		path_id=1;
	}else{
		printf("Dhcp ip error!! ip=%s\r\n",ip_addr);
	}
	return path_id;
}
#if (defined(_NVT_ETHREARCAM_RX_))
void EthcamCpu1IpcReady(char *cmd)
{
	//printf("cmd=%s\r\n", cmd);
	gEthCamCpu1IpcReady=true;
	if(ethlinkstatusget()==NVTIMETH_LINK_UP){
		EthCam_SrvCheckConn();
		#if (ETH_REARCAM_CAPS_COUNT >=2)//bootup
		ethlinkstatusnofity(ethlinkstatusget());
		#endif
	}
}
#endif

#if (ETHCAM_USE_DHCP==ENABLE)//ECOS_DHCP
#if (defined(_NVT_ETHREARCAM_RX_))
void ethcam_notify_ethhub_conn(char *cmd)
{
	char cmdname[40];
	unsigned int  path_id;
	unsigned int link_status;
	sscanf_s(cmd, "%s %d %d", cmdname, 40, &path_id,&link_status);
	printf("path_id=%d, link_status=%d\r\n", path_id, link_status);
	//if(link_status==NVTIMETH_LINK_DOWN){
	//	gEthCamDhcpSrvConnIpInfo[path_id].ipaddr=0;
	//}
}
#endif

#if (defined(_NVT_ETHREARCAM_TX_) && (ETH_REARCAM_CAPS_COUNT>=2))
void ethcam_set_tx_ipaddr(char *cmd)//from Rx Init pstore
{
	char cmdname[40];
	//UINT32 ip_addr;
	sscanf_s(cmd, "%s %d", cmdname, 40, &gEthCamDhcpCliIP);
	printf("cmd=%s ,ip_addr=0x%x, %d.%d.%d.%d\r\n", cmdname, gEthCamDhcpCliIP, (gEthCamDhcpCliIP & 0xFF), (gEthCamDhcpCliIP >> 8) & 0xFF, (gEthCamDhcpCliIP >> 16) & 0xFF, (gEthCamDhcpCliIP >> 24) & 0xFF);

	init_eth0();
}
void ethcam_get_chipid(char *cmd)
{
	char cmdname[40];
	sscanf_s(cmd, "%s %d %d", cmdname, 40, &g_ChipId_h,&g_ChipId_l);
	printf("cmd=%s ,ChipId_h=0x%x, ChipId_l=0x%x\r\n", cmdname, g_ChipId_h, g_ChipId_l);
}
#endif

static int copy_str(char *pdest, char *psrc)
{
    int i = 0;
    int max = CYGNUM_NET_DHCP_OPTION_HOST_NAME_LEN - 1;
    if (pdest && psrc)
    {
        i = strlen(psrc);
        i = (i < max) ? i : max;
        memcpy((void *)pdest, (void *)psrc, i);
        *(pdest+i) = 0;
    }
    return i;
}

bool EthCam_Dhcp_Set_Interface(char *pIntf)
{
    return (dhcpd_set_interface(pIntf));
}
/*
void EthCam_SrvCliConnIPNofity(char* cmd)
{
	char ipccmd[40];
	NVTIPC_SYS_MSG sysMsg;
	NVTIPC_I32 ipcErr = 0;

	snprintf(ipccmd, sizeof(ipccmd) - 1, "%s",cmd);
	sysMsg.sysCmdID = NVTIPC_SYSCMD_SYSCALL_REQ;
	sysMsg.DataAddr = (UINT32)ipccmd;
	sysMsg.DataSize = strlen(ipccmd) + 1;
	if ((ipcErr = NvtIPC_MsgSnd(NVTIPC_SYS_QUEUE_ID, NVTIPC_SENDTO_CORE1, &sysMsg, sizeof(sysMsg))) < 0) {
		printf("Failed to NVTIPC_SYS_QUEUE_ID\r\n");
	}
}
*/
#if (defined(_NVT_ETHREARCAM_TX_))
void EthCam_Dhcp_Client_Cb(enum dhcp_cb_type cbType, unsigned int p1, unsigned int p2, unsigned int p3)
{
	//printf("::type=%d,P=%d, %d, %d\r\n", cbType, p1, p2, p3);
	if (DHCP_CB_TYPE_CLI_REQ_CNT == cbType) {
		//post an event to UI-task show (cycle,count) that client request an IP
	} else if (DHCP_CB_TYPE_CLI_REQ_RESULT == cbType) {
		if (p1) {
			printf("DHCP Client IP = 0x%x\r\n", p1);
			gEthCamDhcpCliFailIP=false;
			//post an event to UI-task to call,Dhcp_Client_Start_Ok(p1) for the first time.
			//post an event to UI-task to show Got-IP
			//Ux_PostEvent(NVTEVT_WIFI_AUTHORIZED_OK, 0);

		} else {
			printf("DHCP Client IP Fail\r\n");
			//post an event to UI-task to Close DHCP Client,Dhcp_Client_Stop(), and show Get-IP fail
			gEthCamDhcpCliFailIP=true;
			exit_eth0();
		}
	} else if (DHCP_CB_TYPE_MAX == cbType) {
		//OS_DumpKernelResStatus();
	}
}
void EthCam_Dhcp_Client_Stop(void)
{
    if (gEthCamDhcpCliOpened)
    {
        dhcp_set_hostname(" ");
        dhcp_release_interfaces(3);
        dhcp_reg_cb(0);
        uninit_all_network_interfaces();
        gEthCamDhcpCliOpened = false;
    }
    else
    {
        //printf("Dhcp Client Not Started;Not End it.\r\n");
    }
}

void EthCam_Dhcp_Client_Start(char *pName, dhcp_cb_func pFunc, bool forceStopIfPrvExist)
{
    printf(" dhcpc=%d,dhcps=%d,force=%d, func=0x%x +\r\n",gEthCamDhcpCliOpened,gEthCamDhcpSrvOpened,forceStopIfPrvExist,pFunc);

    if (gEthCamDhcpSrvOpened)
    {
        printf("DhcpSrv Started;Can Not Be Dhcp Client at the same time\r\n");
    }
    else if (forceStopIfPrvExist)
    {
        //printf("Dhcp Client Already Started !! Stop First, then Restart is again:%d\r\n",forceStopIfPrvExist);
        EthCam_Dhcp_Client_Stop();
    }
    else if (gEthCamDhcpCliOpened)
    {
        printf("Dhcp Client Already Started !! Not Start it again:%d\r\n",forceStopIfPrvExist);
        return;
    }
    copy_str(gEthCamNvtNetHostNameCli, pName);
    dhcp_set_hostname(gEthCamNvtNetHostNameCli);
    gEthCamDhcpCliOpened = true;
    dhcp_reg_cb((dhcp_cb_func)pFunc);
    init_all_network_interfaces();
}
#endif
#if (defined(_NVT_ETHREARCAM_RX_))
static void EthCam_DhcpSrvCBCliConn(DHCPD_CLI_STS cliSts, dhcp_assign_ip_info *ipInfo)
{
	char ch_ipaddr[FIX_IP_MAX_LEN];
	#if(ETH_REARCAM_CAPS_COUNT==1)
	unsigned int ethcam_tx_ipaddr=gEthCamDhcpSrvConnIpInfo.ipaddr;
	#endif

	printf("::cliSts=%d,ip=%d.%d.%d.%d ,mac=%02x:%02x:%02x:%02x:%02x:%02x\r\n", cliSts,(ipInfo->ipaddr & 0xFF), (ipInfo->ipaddr >> 8) & 0xFF, (ipInfo->ipaddr >> 16) & 0xFF, (ipInfo->ipaddr >> 24) & 0xFF, ipInfo->macaddr[0], ipInfo->macaddr[1], ipInfo->macaddr[2], ipInfo->macaddr[3], ipInfo->macaddr[4], ipInfo->macaddr[5]);
	snprintf(ch_ipaddr, FIX_IP_MAX_LEN, "%d.%d.%d.%d", (ipInfo->ipaddr & 0xFF), (ipInfo->ipaddr >> 8) & 0xFF, (ipInfo->ipaddr >> 16) & 0xFF, (ipInfo->ipaddr >> 24) & 0xFF);
	if(cliSts==DHCPD_CLI_STS_REQUEST_IP && ethcam_check_path_id(ch_ipaddr)>=0){
		memcpy(&gEthCamDhcpSrvConnIpInfo, ipInfo, sizeof(dhcp_assign_ip_info));
		char ipccmd[50]={0};
		unsigned int macaddr[2]={0};

		macaddr[0]=(ipInfo->macaddr[0] & 0xFF)+ ((ipInfo->macaddr[1] & 0xFF)<<8 )+ ((ipInfo->macaddr[2] & 0xFF)<<16)+ ((ipInfo->macaddr[3] & 0xFF)<<24);
		macaddr[1]=(ipInfo->macaddr[4] & 0xFF)+ ((ipInfo->macaddr[5]  & 0xFF)<<8 );
		snprintf(ipccmd, sizeof(ipccmd) - 1, "cliconnipnotify %d %d %d",ipInfo->ipaddr,macaddr[0],macaddr[1]);
		#if(ETH_REARCAM_CAPS_COUNT>=2)
			EthCam_SrvCliConnIPNofity(ipccmd);
		#else
		if(ethcam_tx_ipaddr==0){
			EthCam_SrvCliConnIPNofity(ipccmd);
		}
		#endif
		//if(ethlinkstatusget()==NVTIMETH_LINK_UP){
		//	ethlinkstatusnofity(NVTIMETH_LINK_UP);
		//}
	}
	//Ux_PostEvent(NVTEVT_WIFI_DHCP_REQ, 1, (UINT32)CurConnectedIpInfo.macaddr);
}
bool EthCam_SetFixLeaseInfo(unsigned int sec, unsigned int cnt)
{
	dhcp_lease_conf leaseInfo;

	memset((void *)&leaseInfo, 0, sizeof(dhcp_lease_conf));//
	memcpy((void *)leaseInfo.ip_pool_start, NET_LEASE_START_ETH0, strlen(NET_LEASE_START_ETH0));
	memcpy((void *)leaseInfo.ip_pool_end, NET_LEASE_END_ETH0, strlen(NET_LEASE_END_ETH0));
	memcpy((void *)leaseInfo.ip_dns, NET_LEASE_DNS_ETH0, strlen(NET_LEASE_DNS_ETH0));
	memcpy((void *)leaseInfo.ip_wins, NET_LEASE_WINS_ETH0, strlen(NET_LEASE_WINS_ETH0));
	memcpy((void *)leaseInfo.ip_server, NET_SRVIP_ETH0, strlen(NET_SRVIP_ETH0));
	memcpy((void *)leaseInfo.ip_gateway, NET_GATEWAY_ETH0, strlen(NET_GATEWAY_ETH0));
	memcpy((void *)leaseInfo.ip_subnetmask, NET_NETMASK_ETH0, strlen(NET_NETMASK_ETH0));
	memcpy((void *)leaseInfo.ip_broadcast, NET_BRAODCAST_ETH0, strlen(NET_BRAODCAST_ETH0));
	leaseInfo.lease_time = (unsigned int)sec;
	leaseInfo.max_lease_cnt = (unsigned int)cnt;
	dhcpd_set_lease_conf((dhcp_lease_conf *)&leaseInfo);
	return 1;
}

void EthCam_Dhcp_Server_Start(char *pName)
{
    if (false == gEthCamDhcpSrvOpened)
    {
	gEthCamDhcpSrvOpened = true;
	copy_str(gEthCamNvtNetHostNameSrv, pName);
	dhcp_set_hostname(gEthCamNvtNetHostNameSrv);
	dhcpd_reg_cli_attach_cb_func((dhcpd_cli_attach_cb_func)EthCam_DhcpSrvCBCliConn);
	EthCam_SetFixLeaseInfo(30, 2);
	dhcpd_startup();
    }
    else
    {
        printf("Dhcp Server Already Started !!\r\n");
    }
}

void EthCam_Dhcp_Server_Stop(bool uninitIP)
{
    if (true == gEthCamDhcpSrvOpened)
    {
        dhcp_set_hostname(" ");
        dhcpd_exit();
        gEthCamDhcpSrvOpened = false;
        if ((true == uninitIP) && (true == gEthCamDhcpSrvOpened))
        {
            printf("Dhcp Server Running.Please Stop it first.\r\n");
        }
    }
    else
    {
        printf("Dhcp Server Not Started;Not End it.\r\n");
    }
}
void EthcamBoot_Dhcp(char *cmd)
{
	printf("cmd=%s\r\n", cmd);
	EthCam_Dhcp_Set_Interface("eth0");
	EthCam_Dhcp_Server_Start("ethcamsrv");
}
#endif
#endif
