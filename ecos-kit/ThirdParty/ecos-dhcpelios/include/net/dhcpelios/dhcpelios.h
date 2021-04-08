#ifndef CYGONCE_NET_DHCPD_H
#define CYGONCE_NET_DHCPD_H

#include <pkgconf/eliosdhcp.h>

#define __USE_IPC              1


#define NET_IP_MAX_LEN              16
#define NET_INTERFACE_MAX_LEN       16   //"etho","eth1","wlan0"
#define NET_LEASE_SIZE              24

typedef struct _dhcp_lease_conf
{
    char ip_pool_start[NET_IP_MAX_LEN];
    char ip_pool_end[NET_IP_MAX_LEN];
    unsigned int lease_time;
    unsigned int max_lease_cnt;
    char ip_server[NET_IP_MAX_LEN];
    char ip_gateway[NET_IP_MAX_LEN];
    char ip_subnetmask[NET_IP_MAX_LEN];
    char ip_broadcast[NET_IP_MAX_LEN];
    char ip_dns[NET_IP_MAX_LEN];
    char ip_wins[NET_IP_MAX_LEN];
}dhcp_lease_conf;

typedef struct _dhcp_assign_ip_info
{
    unsigned char  macaddr[NET_IP_MAX_LEN];    // MAC adress
    unsigned int ipaddr;                     // IP
}dhcp_assign_ip_info;

typedef struct _dhcpd_lease_tbl_loc
{
    unsigned char *pStartAddr;
    unsigned int size;
}dhcpd_lease_tbl_loc;


#if 1
#define TABLELEASESIZE 16

// Structure to store a lease
struct dhcpOfferedAddr
{
    unsigned char chaddr[TABLELEASESIZE];    // MAC adress !
    unsigned int yiaddr;   // IP network order
    unsigned int expires;  // host order
};//DHCP_LEASE_TBL, *PDHCP_LEASE_TBL; //24byte
#endif

typedef enum{
    DHCPD_CLI_STS_RELEASE_IP = 0,          //release an ip
    DHCPD_CLI_STS_REQUEST_IP,              //request an ip
    DHCPD_CLI_STS_DECLINE_IP,              //decline an ip
    DHCPD_CLI_STS_SRVSTART,                //dhcp server open
    DHCPD_CLI_STS_SRVREADY,                //dhcp server is ready
    DHCPD_CLI_STS_MAX
}DHCPD_CLI_STS;

/**
     Prototype of registering a callback function to notify that a dhcp client connected.

     @param[in] cliSts          0=relese ip,1=request ip,2=decline ip
     @param[in] ipInfo          point to a space with structure,dhcp_assign_ip_info.
*/
typedef void (*dhcpd_cli_attach_cb_func)(DHCPD_CLI_STS cliSts, dhcp_assign_ip_info *ipInfo, dhcpd_lease_tbl_loc *pTbl);

/**
     Register a dhcp client attached notification function.

     Callback if there is a client connected to this dhcp server.

     @param[in] pFunc       points to a function with type, dhcpd_cli_attach_cb_func.
     @return
         - @b 1:   pFunc is not NULL
         - @b 0:   pFunc is NULL
*/
__externC int dhcpd_reg_cli_attach_cb_func(dhcpd_cli_attach_cb_func pFunc);

__externC int dhcpd_startup(void);
__externC int dhcpd_stop(void);
__externC void dhcpd_suspend(void);
__externC void dhcpd_exit(void);
__externC void dhcpd_set_lease_conf(dhcp_lease_conf *pdhcpLeaseConf);
__externC bool dhcpd_set_interface(char *pIntf);
__externC unsigned int dhcpd_get_assigned_ip_info(dhcp_assign_ip_info * pIpInfoAry, unsigned int aryCnt);

//__externC char* dhcpd_GetVerInfo(void);   // get version info
//__externC char* dhcpd_GetBuildDate(void); // get build date

//for 660 IPC
#if __USE_IPC

__externC void dhcp_ipc_cmd(char *pCmd);


#define DHCP_HOST_NAME_MAX_LEN                          60      //CYGNUM_NET_DHCP_OPTION_HOST_NAME_LEN

#define DHCP_IPC_TOKEN_PATH    "dhcpipc"

#define IPC_MSGGET() NvtIPC_MsgGet(NvtIPC_Ftok(DHCP_IPC_TOKEN_PATH))
#define IPC_MSGREL(msqid) NvtIPC_MsgRel(msqid)
#define IPC_MSGSND(msqid,pMsg,msgsz) NvtIPC_MsgSnd(msqid,NVTIPC_SENDTO_CORE1,pMsg,msgsz)
#define IPC_MSGRCV(msqid,pMsg,msgsz) NvtIPC_MsgRcv(msqid,pMsg,msgsz)

// IPC message type;Identify the msg-que is used by uITRON(Client) or Linux(Server) -running CPU.
// But Nvt-IPC library assign two different queue for each CPU.
typedef enum _DHCP_IPC_MSG_TYPE{
    DHCP_IPC_MSG_TYPE_S2C = 1,
    DHCP_IPC_MSG_TYPE_C2S = 2,
}DHCP_IPC_MSG_TYPE;

typedef struct _DHCP_IPC_MSG
{
    long            mtype;
    unsigned int   uiIPC;
    unsigned int   parm1;
    unsigned int   parm2;
    //unsigned int   parm3;
}DHCP_IPC_MSG, *PDHCP_IPC_MSG;

//DHCPD_CLI_STS cliSts, unsigned int yiaddr,
typedef struct _DHCPD_IPC_CLI_ATTA_CB_MSG
{
    long            mtype;
    unsigned int    uiIPC;
    unsigned int    cbFuncAddr;
    DHCPD_CLI_STS   cliSts;
    dhcp_assign_ip_info   *pIPInfo;
    dhcpd_lease_tbl_loc   *pLeaseTblLoc;
}DHCPD_IPC_CLI_ATTA_CB_MSG;

#define DHCP_IPC_MSGSZ          (sizeof(DHCP_IPC_MSG))

typedef struct _DHCP_IPC_STRING
{
    char *pStr;
    unsigned int strSize;
}DHCP_IPC_STRING;

#define DHCP_IPC_FIX_IP_PAIR_MAX_CNT           3
typedef struct _DHCP_IPC_FIX_IP_PAIR
{
    char ip[NET_IP_MAX_LEN];
    char netmask_ip[NET_IP_MAX_LEN];
    char broadcast_ip[NET_IP_MAX_LEN];
    char gateway_ip[NET_IP_MAX_LEN];
    char server_ip[NET_IP_MAX_LEN];
    unsigned int intrfIdx;
}DHCP_IPC_FIX_IP_PAIR;

#endif


#endif
