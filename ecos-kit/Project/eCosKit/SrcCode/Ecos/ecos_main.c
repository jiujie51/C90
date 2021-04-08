#include <stdio.h>
#include <stdlib.h>
#include "Type.h"
#include "test_main.h"
#include "init_ipc.h"
#include "init_inet.h"
#include "init_cmd.h"
#include <cyg/strmsnd/nvtstreamsender_protected.h>
//#include <cyg/io/sdio_linux.h>

#include <cyg/nvtipc/NvtIpcAPI.h>
#include <cyg/io/nvtimeth.h>

#include <network.h>
#include <arpa/inet.h>
//for nvtimeth_set_mac_address
#include <netinet/if_ether.h>
#include "unistd.h"

//missing part when linking with libstdc++.a
#include <sys/reent.h>
#ifndef __ATTRIBUTE_IMPURE_PTR__
#define __ATTRIBUTE_IMPURE_PTR__
#endif
#ifndef __ATTRIBUTE_IMPURE_DATA__
#define __ATTRIBUTE_IMPURE_DATA__
#endif
struct _reent __ATTRIBUTE_IMPURE_DATA__ _impure_data = _REENT_INIT(_impure_data);
struct _reent *__ATTRIBUTE_IMPURE_PTR__ _impure_ptr = &_impure_data;

#define PAUSE_FOR__DEBUG        0
#define REG_SYS_EXCEPTION       1
#define SUPPORT_IPC             1
#define SUPPORT_INET            0
//Start dhcp server or client from uIRON rather than start network at ecos initialization.
#define RUN_DHCP_IPC            0
#define RUN_TEST_MAIN           0
#define TIMER0_COUNTER_REG          0xC0040108
#define ipc_getMmioAddr(addr)       (addr)

//for 67x evb
#if (defined(_NVT_ETHREARCAM_TX_) || defined(_NVT_ETHREARCAM_RX_))
char Tx_mac_addr[ETHER_ADDR_LEN]={0x00,0xff,0x3e,0x26,0x0a,0x6b};
//char Tx_mac_addr[ETHER_ADDR_LEN]={0x00,0xff,0x3e,0x26,0x0a,0x7b};
static char ipc_chk_stack[4096]; /* 4K stack space for the thread */
static cyg_handle_t ipc_chk_handle_t;  /* now the handles for the thread */
static cyg_thread ipc_chk_thread_s; /* space for thread object */
#endif
typedef void (*os_exception_handler)(
    UINT32 data,                // user supplied data
    UINT32 exception_number,    // exception being raised
    UINT32 *exception_info      // any exception specific info
);
static unsigned int gbEthCamEthLinkStatus=NVTIMETH_LINK_DOWN;
extern bool gEthCamDhcpCliOpened;
extern unsigned int gEthCamDhcpCliIP;
extern dhcp_assign_ip_info gEthCamDhcpSrvConnIpInfo;
extern bool gEthCamDhcpCliFailIP;
extern bool gEthCamDhcpSrvOpened;

extern INT32 OS_RegSysException(UINT32 os_exception_number, os_exception_handler handler, UINT32 u32Data);
extern void ethlinkstatusnofity(unsigned int sts);
extern bool gEthCamCpu1IpcReady;
extern unsigned int g_ChipId_h;
extern unsigned int g_ChipId_l;
void ethlinkstatusnofity_cmd(char* cmd)
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
unsigned int ethlinkstatusget(void)
{
	return gbEthCamEthLinkStatus;
}
void ethlinkstatusset(unsigned int LinkStatus)
{
	gbEthCamEthLinkStatus=LinkStatus;
}
#if (defined(_NVT_ETHREARCAM_TX_) || defined(_NVT_ETHREARCAM_RX_))
static void thread_ipc_chk(cyg_addrword_t data)
{
	const cyg_uint32 tick = 50000000/(CYGNUM_HAL_RTC_NUMERATOR/CYGNUM_HAL_RTC_DENOMINATOR);//50ms
	if(NvtIPC_ChkIfCoreReady(NVTIPC_CORE2)==0){
		cyg_thread_delay(tick);
	}
	else{
		if(ethlinkstatusget()==NVTIMETH_LINK_UP){
			ethlinkstatusnofity(ethlinkstatusget());
		}
		cyg_thread_exit();
	}
}

void init_ipc_chk(void)
{
    cyg_thread_create(4, thread_ipc_chk, (cyg_addrword_t) 0,
        "init_ipc_chk", (void *) ipc_chk_stack, sizeof(ipc_chk_stack),
        &ipc_chk_handle_t, &ipc_chk_thread_s);

    cyg_thread_resume(ipc_chk_handle_t);
}
#endif


void ethlinkstatusnofity(unsigned int sts)
{
	NVTIPC_I32 ipcErr = 0;
	NVTIPC_SYS_MSG sysMsg;
	char ipccmd[40];
	unsigned int ethcam_tx_ipaddr=0;
	printf("ethlinkstatusnofity, sts=%d\r\n",sts);
	ethlinkstatusset(sts);
	#if (defined(_NVT_ETHREARCAM_TX_))
	#if (ETHCAM_USE_DHCP==ENABLE)
		if(sts==NVTIMETH_LINK_UP){
			if(gEthCamDhcpCliOpened && gEthCamDhcpCliIP==0){//DHCP
				printf("Dhcp start but NO IP Get!\r\n"); //wait ethcamsettxip cmd
				return;
			}else if(gEthCamDhcpCliOpened==0 && gEthCamDhcpCliIP==0){//static
				if(gEthCamDhcpCliFailIP==true){
					init_eth0();//retry
				}else{
					printf("NO IP Get!\r\n"); //wait ethcamsettxip cmd
				}
				return;
			}else{
				if(gEthCamDhcpCliOpened==0){
					//init_eth0();
					//return;
				}
			}
		}else{
			if(gEthCamDhcpCliOpened){
				//exit_eth0();
			}
		}
		ethcam_tx_ipaddr=gEthCamDhcpCliIP;
	#else
		ethcam_tx_ipaddr=0xc00a8c0;//192.168.0.12
		if(NvtIPC_ChkIfCoreReady(NVTIPC_CORE2)==0){
			printf("EthCamIpc NOT Ready!!\r\n");
			init_ipc_chk();
			return;
		}
	#endif
	#endif
	#if (defined(_NVT_ETHREARCAM_RX_))
	#if(ETH_REARCAM_CAPS_COUNT>=2)

		if(sts==NVTIMETH_LINK_UP){
			if(gEthCamDhcpSrvConnIpInfo.ipaddr==0){
				printf("Cli NO IP Get!\r\n");
				//return;
				if(NvtIPC_ChkIfCoreReady(NVTIPC_CORE2)==0){
					printf("EthCamIpc NOT Ready!!\r\n");
					init_ipc_chk();
					return;
				}
			}
			ethcam_tx_ipaddr=0;//gEthCamDhcpSrvConnIpInfo.ipaddr;
		}
	#else
	if(gEthCamDhcpSrvOpened){
		ethcam_tx_ipaddr=gEthCamDhcpSrvConnIpInfo.ipaddr;
	}else{
		ethcam_tx_ipaddr=0xc00a8c0;//192.168.0.12
	}
	if(gEthCamCpu1IpcReady==0){
		printf("EthCamIpc NOT Ready!!\r\n");
		return;
	}
	#endif
	#endif

	snprintf(ipccmd, sizeof(ipccmd) - 1, "ethlinknotify %d %d",ethcam_tx_ipaddr, sts);
	sysMsg.sysCmdID = NVTIPC_SYSCMD_SYSCALL_REQ;
	sysMsg.DataAddr = (UINT32)ipccmd;
	sysMsg.DataSize = strlen(ipccmd) + 1;
	if ((ipcErr = NvtIPC_MsgSnd(NVTIPC_SYS_QUEUE_ID, NVTIPC_SENDTO_CORE1, &sysMsg, sizeof(sysMsg))) < 0) {
		printf("Failed to NVTIPC_SYS_QUEUE_ID\r\n");
	}
}
int eth_set_mac_address( const char *interface, char *mac_address )
{
    int s, i;
    struct ifreq ifr;

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        diag_printf("socket\n");
        return false;
    }

    diag_printf( "%s socket is %d:\n", interface, s );

    strcpy(ifr.ifr_name, interface);

    for ( i = 0; i < ETHER_ADDR_LEN; i++ )
        ifr.ifr_hwaddr.sa_data[i] = mac_address[i];

    diag_printf( "Mac addr %02x:%02x:%02x:%02x:%02x:%02x\n",
                 ifr.ifr_hwaddr.sa_data[0],
                 ifr.ifr_hwaddr.sa_data[1],
                 ifr.ifr_hwaddr.sa_data[2],
                 ifr.ifr_hwaddr.sa_data[3],
                 ifr.ifr_hwaddr.sa_data[4],
                 ifr.ifr_hwaddr.sa_data[5] );

    if (ioctl(s, SIOCSIFHWADDR, &ifr)) {
        diag_printf("SIOCSIFHWADDR\n");
        close( s );
        return false;
    }

    diag_printf( "%s ioctl(SIOCSIFHWADDR) succeeded\n", interface );

    close( s );

    return true;
}
int eth_get_mac_address( const char *interface, char *mac_address )
{
    int s, i;
    struct ifreq ifr;

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        diag_printf("socket\n");
        return false;
    }

    diag_printf( "%s socket is %d:\n", interface, s );

    strcpy(ifr.ifr_name, interface);



    if (ioctl(s, SIOCGIFHWADDR, &ifr)) {
        diag_printf("SIOCGIFHWADDR\n");
        close( s );
        return false;
    }
    printf( "Mac addr %02x:%02x:%02x:%02x:%02x:%02x\n",
                 ifr.ifr_hwaddr.sa_data[0],
                 ifr.ifr_hwaddr.sa_data[1],
                 ifr.ifr_hwaddr.sa_data[2],
                 ifr.ifr_hwaddr.sa_data[3],
                 ifr.ifr_hwaddr.sa_data[4],
                 ifr.ifr_hwaddr.sa_data[5] );

    diag_printf( "%s ioctl(SIOCGIFHWADDR) succeeded\n", interface );

    for ( i = 0; i < ETHER_ADDR_LEN; i++ )
        mac_address[i]=ifr.ifr_hwaddr.sa_data[i];

    close( s );

    return true;
}
int eth_gen_mac_address(char *mac_address)
{
#if(defined(_NVT_ETHREARCAM_TX_) && ETH_REARCAM_CAPS_COUNT>=2)

	int i, j=0, k=0;
	for ( i = 0; i < (ETHER_ADDR_LEN); i++ ){
		if(i<2){
			k++;
			if(i==0){
				mac_address[i]= (g_ChipId_h>>((2-k)*8)) & 0xFE;
			}else{
				mac_address[i]= (g_ChipId_h>>((2-k)*8)) & 0xFF;
			}
		}else{
			j++;
			mac_address[i]=(g_ChipId_l>>((4-j)*8)) & 0xFF;
		}
	}
#endif
	return true;
}
void eth_config_mac_address(void)
{
#if (defined(_NVT_ETHREARCAM_TX_) || defined(_NVT_ETHREARCAM_RX_))
	eth_gen_mac_address(Tx_mac_addr);
	eth_set_mac_address("eth0", Tx_mac_addr);
#endif  
}
//
//cyg_user_start() is appliction main entry for eCos platform
//
void cyg_user_start(void)
{
#if REG_SYS_EXCEPTION
    {
        unsigned int uiIdx;
        //Install default exception handler
        for (uiIdx=0; uiIdx<15; uiIdx++)
        {
            OS_RegSysException(uiIdx, NULL, 0);
        }
    }
#endif

#if PAUSE_FOR__DEBUG
    static int a=0;

    while(!a)
    {
        a=a+a;
    }
#endif
    printf("Hello, eCos world!!!!!!!!!!\n");

#if SUPPORT_IPC
    init_ipc();
#endif

	//init stream sender after ipc init
	ssender_fakeio_module_init();
#if defined(_NVT_ETHERNET_EQOS_)
	//init eth0
#if (ETHCAM_USE_DHCP==ENABLE)
#if (defined(_NVT_ETHREARCAM_RX_))
	init_eth0();
#endif
#else
	init_eth0();
#endif
#if (defined(_NVT_ETHREARCAM_TX_) || defined(_NVT_ETHREARCAM_RX_))
	nvtimeth_register_link_cb(ethlinkstatusnofity);
#endif
#endif

#if (SUPPORT_INET && (0 == RUN_DHCP_IPC))
    printf("Start init_inet for ftpd\n");
    init_inet();
#else
    //Start dhcp server or client from uIRON rather than start network at ecos initialization.
    printf("==== NOT Start init_inet for ftpd ===\n");
#endif

    //command system
    init_cmd();

#if RUN_TEST_MAIN
    test_main();
#endif


    printf("Ready for eCos scheduling...\n");
}

