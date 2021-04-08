#ifndef _USOCKET_IPC_H_
#define _USOCKET_IPC_H_

//=============================================================================
//define USOCKET_SIM for simulation
//#define USOCKET_SIM

#define USOCKET_VER_KEY              20190312

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3) ((unsigned int)(unsigned char)(ch0) | ((unsigned int)(unsigned char)(ch1) << 8) | ((unsigned int)(unsigned char)(ch2) << 16) | ((unsigned int)(unsigned char)(ch3) << 24 ))
#endif
#define USOCKETIPC_BUF_CHK_SIZE    (4)
#define USOCKETIPC_BUF_TAG         MAKEFOURCC('U','S','O','C')
#define USOCKIPC_PARAM_BUF_SIZE      4112  //for USOCKIPC_TRANSFER_BUF_SIZE is 2048
#define USOCKIPC_TRANSFER_BUF_SIZE   ((USOCKIPC_PARAM_BUF_SIZE/2)-USOCKETIPC_BUF_CHK_SIZE-4) // 4 is size of USOCKET_TRANSFER_PARAM
#define USOCKIPC_IP_BUF            (16)

//-------------------------------------------------------------------------
//USOCKETECOS API return value
#define USOCKET_RET_OK           0
#define USOCKET_RET_OPENED       1
#define USOCKET_RET_ERR          (-1)
#define USOCKET_RET_NO_FUNC      (-2)
#define USOCKET_RET_FORCE_ACK    (-3)

#define USOCKET_TOKEN_PATH0       "usockipc0"
#define USOCKET_TOKEN_PATH1       "usockipc1"
#define USOCKET_MAX_PARAM                6
//-------------------------------------------------------------------------
//IPC command id and parameters
typedef enum{
    USOCKET_CMDID_GET_VER_INFO  =   0x00000000,
    USOCKET_CMDID_GET_BUILD_DATE,
    USOCKET_CMDID_TEST,
    USOCKET_CMDID_OPEN,
    USOCKET_CMDID_CLOSE,
    USOCKET_CMDID_SEND,
    USOCKET_CMDID_RCV,
    USOCKET_CMDID_NOTIFY,
    USOCKET_CMDID_UDP_OPEN,
    USOCKET_CMDID_UDP_CLOSE,
    USOCKET_CMDID_UDP_SEND,
    USOCKET_CMDID_UDP_RCV,
    USOCKET_CMDID_UDP_NOTIFY,
    USOCKET_CMDID_SYSREQ_ACK,
    USOCKET_CMDID_UNINIT,
    USOCKET_CMDID_UDP_SENDTO,
    USOCKET_CMDID_MAX,
}USOCKET_CMDID;

typedef struct{
    USOCKET_CMDID CmdId;
    int Arg;
}USOCKET_MSG;

typedef struct{
    int param1;
    int param2;
    int param3;
}USOCKET_PARAM_PARAM;

typedef struct{
    int size;
    char buf[USOCKIPC_TRANSFER_BUF_SIZE];
}USOCKET_TRANSFER_PARAM;

typedef struct{
    char dest_ip[USOCKIPC_IP_BUF];
    int  dest_port;
    int  size;
    char buf[USOCKIPC_TRANSFER_BUF_SIZE];
}USOCKET_SENDTO_PARAM;

//-------------------------------------------------------------------------
//USOCKET definition

typedef struct _USOCKET_OPEN_OBJ{
    unsigned int        RcvParamAddr;
    unsigned int        SndParamAddr;
}USOCKET_OPEN_OBJ;


#ifndef __externC
#ifdef __cplusplus
# define __externC extern "C"
#else
# define __externC extern
#endif
#endif

#if defined(__ECOS)
__externC void USOCKETECOS_CmdLine(char *szCmd);
#else
__externC int USOCKETECOS_CmdLine(char *szCmd, char *szRcvAddr, char *szSndAddr);
#endif

#endif //_USOCKET_IPC_H_

