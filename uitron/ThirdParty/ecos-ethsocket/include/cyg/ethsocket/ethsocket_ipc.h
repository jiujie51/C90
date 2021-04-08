#ifndef _ETHSOCKET_IPC_H_
#define _ETHSOCKET_IPC_H_

//=============================================================================
//define ETHSOCKET_SIM for simulation
//#define ETHSOCKET_SIM

#define ETHSOCKET_VER_KEY               20190506

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)  ((unsigned int)(unsigned char)(ch0) | ((unsigned int)(unsigned char)(ch1) << 8) | ((unsigned int)(unsigned char)(ch2) << 16) | ((unsigned int)(unsigned char)(ch3) << 24 ))
#endif
#define ETHSOCKETIPC_BUF_CHK_SIZE       (4)
#define ETHSOCKETIPC_BUF_TAG            MAKEFOURCC('E','T','S','C')
#define ETHSOCKIPC_PARAM_BUF_SIZE       4112  //for ETHSOCKIPC_TRANSFER_BUF_SIZE is 2048
#define ETHSOCKIPC_TRANSFER_BUF_SIZE    ((ETHSOCKIPC_PARAM_BUF_SIZE / 2) - ETHSOCKETIPC_BUF_CHK_SIZE - 4) // 4 is size of ETHSOCKET_TRANSFER_PARAM
#define ETHSOCKIPC_IP_BUF               (16)

//-------------------------------------------------------------------------
//ETHSOCKETECOS API return value
#define ETHSOCKET_RET_OK           0
#define ETHSOCKET_RET_OPENED       1
#define ETHSOCKET_RET_ERR          (-1)
#define ETHSOCKET_RET_NO_FUNC      (-2)
#define ETHSOCKET_RET_FORCE_ACK    (-3)

#define ETHSOCKET_TOKEN_PATH0       "ethsockipc0"
#define ETHSOCKET_TOKEN_PATH1       "ethsockipc1"
#define ETHSOCKET_MAX_PARAM                6
//-------------------------------------------------------------------------
//IPC command id and parameters
typedef enum {
	ETHSOCKET_CMDID_GET_VER_INFO  =   0x00000000,
	ETHSOCKET_CMDID_GET_BUILD_DATE,
	ETHSOCKET_CMDID_TEST,
	ETHSOCKET_CMDID_OPEN,
	ETHSOCKET_CMDID_CLOSE,
	ETHSOCKET_CMDID_SEND,
	ETHSOCKET_CMDID_RCV,
	ETHSOCKET_CMDID_NOTIFY,
	ETHSOCKET_CMDID_UDP_OPEN,
	ETHSOCKET_CMDID_UDP_CLOSE,
	ETHSOCKET_CMDID_UDP_SEND,
	ETHSOCKET_CMDID_UDP_RCV,
	ETHSOCKET_CMDID_UDP_NOTIFY,
	ETHSOCKET_CMDID_SYSREQ_ACK,
	ETHSOCKET_CMDID_UNINIT,
	ETHSOCKET_CMDID_UDP_SENDTO,
	ETHSOCKET_CMDID_MAX,
} ETHSOCKET_CMDID;

typedef struct {
	ETHSOCKET_CMDID CmdId;
	int Arg;
} ETHSOCKET_MSG;

typedef struct {
	int param1;
	int param2;
	int param3;
} ETHSOCKET_PARAM_PARAM;

typedef struct {
	int size;
	char buf[ETHSOCKIPC_TRANSFER_BUF_SIZE];
} ETHSOCKET_TRANSFER_PARAM;

typedef struct {
	char dest_ip[ETHSOCKIPC_IP_BUF];
	int  dest_port;
	int  size;
	char buf[ETHSOCKIPC_TRANSFER_BUF_SIZE];
} ETHSOCKET_SENDTO_PARAM;

//-------------------------------------------------------------------------
//ETHSOCKET definition

typedef struct _ETHSOCKET_OPEN_OBJ {
	unsigned int        RcvParamAddr;
	unsigned int        SndParamAddr;
} ETHSOCKET_OPEN_OBJ;


#ifndef __externC
#ifdef __cplusplus
# define __externC extern "C"
#else
# define __externC extern
#endif
#endif

#if defined(__ECOS)
__externC void ETHSOCKETECOS_CmdLine(char *szCmd);
#else
__externC int ETHSOCKETECOS_CmdLine(char *szCmd, char *szRcvAddr, char *szSndAddr);
#endif

#endif //_ETHSOCKET_IPC_H_

