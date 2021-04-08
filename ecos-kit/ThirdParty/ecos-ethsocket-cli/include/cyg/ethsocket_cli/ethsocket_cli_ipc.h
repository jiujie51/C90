#ifndef _ETHSOCKETCLI_IPC_H_
#define _ETHSOCKETCLI_IPC_H_

//=============================================================================
//define ETHSOCKETCLI_SIM for simulation
//#define ETHSOCKETCLI_SIM

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)      ((unsigned int)(unsigned char)(ch0) | ((unsigned int)(unsigned char)(ch1) << 8) | ((unsigned int)(unsigned char)(ch2) << 16) | ((unsigned int)(unsigned char)(ch3) << 24 ))
#endif
#define ETHSOCKETCLIIPC_BUF_CHK_SIZE        (4)
#define ETHSOCKETCLIIPC_BUF_TAG             MAKEFOURCC('E', 'T', 'C', 'I')
//#define ETHSOCKCLIIPC_PARAM_BUF_SIZE        (4*1024+16)//2048
//#define ETHSOCKCLIIPC_PARAM_BUF_SIZE        (8*1024+16)//4K
#define ETHSOCKCLIIPC_PARAM_BUF_SIZE        (96*1024+16)//48K
#define ETHSOCKCLIIPC_TRANSFER_BUF_SIZE     ((ETHSOCKCLIIPC_PARAM_BUF_SIZE / 2) - ETHSOCKETCLIIPC_BUF_CHK_SIZE - 4) // 4 is size of ETHSOCKETCLI_TRANSFER_PARAM

#define ETHSOCKETCLIIPC_SLOW_TAG             MAKEFOURCC('E', 'T', 'S', 'L')

//-------------------------------------------------------------------------
//ETHSOCKETECOS API return value

#define ETHSOCKETCLI_RET_OK           0
#define ETHSOCKETCLI_RET_OPENED       1
#define ETHSOCKETCLI_RET_ERR          (-1)
#define ETHSOCKETCLI_RET_NO_FUNC      (-2)
#define ETHSOCKETCLI_RET_NOT_OPEN     (-3)
#define ETHSOCKETCLI_RET_ERR_PAR      (-4)
#define ETHSOCKETVLI_RET_FORCE_ACK    (-5)

//#define ETHSOCKETCLI_TOKEN_PATH0       "ethsockcliipc0"
//#define ETHSOCKETCLI_TOKEN_PATH1       "ethsockcliipc1"
//#define ETHSOCKETCLI_TOKEN_PATH2       "ethsockcliipc2"
#define ETHSOCKETCLI_TOKEN_scoket0_p0       "ethsockcliipc0_p0"
#define ETHSOCKETCLI_TOKEN_scoket1_p0       "ethsockcliipc1_p0"
#define ETHSOCKETCLI_TOKEN_scoket2_p0       "ethsockcliipc2_p0"
#define ETHSOCKETCLI_TOKEN_scoket0_p1       "ethsockcliipc0_p1"
#define ETHSOCKETCLI_TOKEN_scoket1_p1       "ethsockcliipc1_p1"
#define ETHSOCKETCLI_TOKEN_scoket2_p1       "ethsockcliipc2_p1"

#define ETHSOCKETCLI_MAX_PARAM                6
//-------------------------------------------------------------------------
//IPC command id and parameters
typedef enum{
	ETHSOCKETCLI_CMDID_GET_VER_INFO  =   0x00000000,
	ETHSOCKETCLI_CMDID_GET_BUILD_DATE,
	ETHSOCKETCLI_CMDID_START,
	ETHSOCKETCLI_CMDID_CONNECT,
	ETHSOCKETCLI_CMDID_DISCONNECT,
	ETHSOCKETCLI_CMDID_STOP,
	ETHSOCKETCLI_CMDID_SEND,
	ETHSOCKETCLI_CMDID_RCV,
	ETHSOCKETCLI_CMDID_NOTIFY,
	ETHSOCKETCLI_CMDID_SYSREQ_ACK,
	ETHSOCKETCLI_CMDID_UNINIT,
} ETHSOCKETCLI_CMDID;

typedef struct{
	ETHSOCKETCLI_CMDID CmdId;
	int Arg;
} ETHSOCKETCLI_MSG;

typedef struct{
	int param1;
	int param2;
	int param3;
} ETHSOCKETCLI_PARAM_PARAM;

typedef struct{
	//int obj;
	int size;
	char buf[ETHSOCKCLIIPC_TRANSFER_BUF_SIZE];
} ETHSOCKETCLI_TRANSFER_PARAM;
typedef struct{
	//int obj;
	int size;
	char *buf;
} ETHSOCKETCLI_RECV_PARAM;

//-------------------------------------------------------------------------
//ETHSOCKET definition

typedef struct _ETHSOCKETCLI_OPEN_OBJ{
	unsigned int        RcvParamAddr;
	unsigned int        RcvParamSize;
	unsigned int        SndParamAddr;
	unsigned int        SndParamSize;

	unsigned int        BsBufAddr;
	unsigned int        BsBufSize;

	unsigned int        RawEncodeAddr;
	unsigned int        RawEncodeSize;

	unsigned int        BsQueueMax;
} ETHSOCKETCLI_OPEN_OBJ;


#ifndef __externC
#ifdef __cplusplus
# define __externC extern "C"
#else
# define __externC extern
#endif
#endif

__externC void ETHSOCKETCLIECOS_CmdLine(char *szCmd);

#endif //_ETHSOCKETCLI_IPC_H_

