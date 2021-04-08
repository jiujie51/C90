#include <stdio.h>
#include <stdlib.h>
#include "Type.h"
#include <cyg/nvtipc/NvtIpcAPI.h>
//#include "Live555Common.h"
//#include "Live555CliCommon.h"
#include <cyg/hfs/hfs.h>
#include <cyg/lviewd/lviewd.h>
#include <cyg/infra/diag.h>
#include <cyg/cmd/CmdSysAPI.h>
#include <cyg/fs/nvtfs.h>
#include <cyg/nvtwifi/nvtwifi.h>
#include <cyg/usocket/usocket_ipc.h>
//#include <cyg/usocket_cli/usocket_cli_ipc.h>
#include <cyg/ethsocket/ethsocket_ipc.h>
#include <cyg/ethsocket_cli/ethsocket_cli_ipc.h>
#include <net/dhcpelios/dhcpelios.h>
//#include <YoukuIpc.h>
//#include "TutkIpcECOS.h"
//#include <axTLS/ssl.h>
//#include <axTLS/crypto.h>
#include "curl/curl_tool_api.h"
//#include <cyg/sntp/sntp.h>
#include <cyg/msdcnvt/msdcnvt.h>
#include <cyg/audcodec/audcodec.h>
#include <cyg/ethcamboot/ethcamboot.h>
#include "init_inet.h"
#if defined(_ASR_ENABLE_)
#include <cyg/asr/asr.h>
#endif
#include <cyg/audproc/audproc.h>

//#NT#2016/04/21#Lincy Lin -begin
//#NT#Support curl (http client)
#if 0 //(_SSLTYPE_==_SSL_OFF_)
static const char * const ssl_unsupported_str = "Error: ssl Feature not supported\n";
EXP_FUNC void STDCALL __attribute__((weak)) MD5_Init(MD5_CTX *ctx)
{
    printf("%s",ssl_unsupported_str);
}

EXP_FUNC void STDCALL __attribute__((weak)) MD5_Update(MD5_CTX *ctx, const uint8_t * msg, int len)
{
    printf("%s",ssl_unsupported_str);
}

EXP_FUNC void STDCALL __attribute__((weak)) MD5_Final(uint8_t *digest, MD5_CTX *ctx)
{
    printf("%s",ssl_unsupported_str);
}
#endif
//#NT#2016/04/21#Lincy Lin -end

#if 1
static SYSCALL_CMD_TBL ipccmd_tbl[]={
#if (!defined(_NVT_ETHREARCAM_TX_))
{"audcodec",(SYSCALL_CMD_FP)AudCodec_CmdLine},
#endif
{"fsipc",(SYSCALL_CMD_FP)FileSysECOS_CmdLine},
#if (!defined(_NVT_SDIO_WIFI_NONE_) || !defined(_NVT_USB_WIFI_NONE_)) && defined(_NETWORK_CPU2_)
{"wifiipc",(SYSCALL_CMD_FP)NvtWifiECOS_CmdLine},
{"lviewd",(SYSCALL_CMD_FP)cyg_lviewd_startup2},
{"hfs",(SYSCALL_CMD_FP)cyg_hfs_open2},
{"usockipc",(SYSCALL_CMD_FP)USOCKETECOS_CmdLine},
#endif
#if (defined(_NVT_ETHREARCAM_TX_))
{"ethsockipc",(SYSCALL_CMD_FP)ETHSOCKETECOS_CmdLine},
#elif (defined(_NVT_ETHREARCAM_RX_))
{"ethsockcliipc",(SYSCALL_CMD_FP)ETHSOCKETCLIECOS_CmdLine},
#endif
#if (!defined(_NVT_SDIO_WIFI_NONE_) || !defined(_NVT_USB_WIFI_NONE_)) && defined(_NETWORK_CPU2_)
{"dhcpipc",(SYSCALL_CMD_FP)dhcp_ipc_cmd},
{"curl",(SYSCALL_CMD_FP)curl_open},
#endif
#if defined(_NETWORK_CPU2_)
{"msdcnvt",(SYSCALL_CMD_FP)msdcnvt_ecos_main},
#endif
#if (defined(_NVT_ETHREARCAM_RX_))
{"ethcamboot_dhcp",(SYSCALL_CMD_FP)EthcamBoot_Dhcp},
{"ethcamboot",(SYSCALL_CMD_FP)EthcamBoot_CmdLine},
{"ethcamipcready",(SYSCALL_CMD_FP)EthcamCpu1IpcReady},
#endif
#if (ETHCAM_USE_DHCP==ENABLE)
#if (defined(_NVT_ETHREARCAM_TX_) && (ETH_REARCAM_CAPS_COUNT>=2))
{"ethcamsettxip",(SYSCALL_CMD_FP)ethcam_set_tx_ipaddr},//set pstore ip
{"chipid",(SYSCALL_CMD_FP)ethcam_get_chipid},
#endif
#if (defined(_NVT_ETHREARCAM_RX_))
{"ethcamnotifyconn",(SYSCALL_CMD_FP)ethcam_notify_ethhub_conn},
{"ethcamcliconncheck",(SYSCALL_CMD_FP)EthCam_PortReadyCheck},
#endif
#endif
#if defined(_ASR_ENABLE_)
{"asr",(SYSCALL_CMD_FP)ASR_CmdLine},
#endif
{"ethcamiperf",(SYSCALL_CMD_FP)EthcamIperf_CmdLine},
{"audproc",(SYSCALL_CMD_FP)AudProc_CmdLine},
{"",NULL}   // this item should be the last item
};
#endif

void init_ipc(void)
{
#if 1
    int     workbuf;
    NVTIPC_OPEN_OBJ   openObj={0};

    workbuf = (unsigned int)malloc(NvtIPC_CalBuffSize());
    if (!workbuf)
    {
        printf("can't alloc workbuf \r\n");
        return;
    }
    openObj.workBuffAddr = workbuf;
    openObj.workBuffSize = NvtIPC_CalBuffSize();
    openObj.SysCmdTbl = ipccmd_tbl;
    openObj.uartCmdFp = runmain_as_pthread;
	printf("IPC haven't open\r\n");
    NvtIPC_Open(&openObj);
	printf("IPC is opened\r\n");
#endif
}
