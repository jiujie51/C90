#ifndef _ETHCAM_SOCKET_H
#define _ETHCAM_SOCKET_H
#include "Type.h"
#include "EthsockIpcAPI.h"
#include "EthsockCliIpcAPI.h"

#define ETHCAM_LINK_DOWN          (0x01)          //< link down
#define ETHCAM_LINK_UP           	 (0x02)          //< link up

#define ETHCAM_SOCKETCLI_IP_MAX_LEN              16

#ifndef ETHCAM_SOCKETCLI_MAC_MAX_LEN
#define ETHCAM_SOCKETCLI_MAC_MAX_LEN 		18
#endif

#define HEAD_TYPE_THUMB     		0xff
#define HEAD_TYPE_RAW_ENCODE     	0xfe
#define HEAD_TYPE_LONGCNT_STAMP  	0xfc
#define HEAD_TYPE_AUDCAP     		0xfd

#define LONGCNT_STAMP_OFFSET 16

#define ETHCAM_TS_VIDEO_RESERVED_SIZE 24 //align 4
#define ETHCAM_AUDCAP_FRAME_SIZE (2*1024-ETHCAM_TS_VIDEO_RESERVED_SIZE)
#define ETHCAM_AUDCAP_HEADER_OFFSET (8)


typedef struct _ETHCAM_SOCKET_BUF_OBJ {
	unsigned int        ParamAddr;
	unsigned int        ParamSize;
} ETHCAM_SOCKET_BUF_OBJ ;

typedef struct _ETHCAM_SOCKET_INFO
{
    char ip[ETHCAM_SOCKETCLI_IP_MAX_LEN];         ///< ip of the server
    UINT32 port[3];         ///< port of the server
    char mac[ETHCAM_SOCKETCLI_MAC_MAX_LEN];         ///< mac addr of the server

}ETHCAM_SOCKET_INFO, *pETHCAM_SOCKET_INFO;

typedef enum _ETHSOCK_ID {
	ETHSOCKET_DATA1 ,
	ETHSOCKET_DATA2 ,
	ETHSOCKET_CMD ,

	ETHSOCKETCLI_DATA1 ,
	ETHSOCKETCLI_DATA2 ,
	ETHSOCKETCLI_CMD ,

	ETHSOCKET_MAX_NUM,
	ENUM_DUMMY4WORD(ETHSOCKET_ID)
} ETHSOCKET_ID;

typedef enum _ETHCAM_PORT_TYPE {
	ETHCAM_PORT_DEFAULT,  //for cmd port
	ETHCAM_PORT_DATA1,  	//for write file on card port
	ETHCAM_PORT_DATA2,  	//for display port
	ETHCAM_PORT_DATA_MAX,
	ENUM_DUMMY4WORD(ETHCAM_PORT_TYPE)
} ETHCAM_PORT_TYPE;


typedef enum _ETHCAM_PATH_ID {
	ETHCAM_PATH_ID_1,   //Tx 1
	ETHCAM_PATH_ID_2,   //Tx 2
	ETHCAM_PATH_ID_MAX,
	ENUM_DUMMY4WORD(ETHCAM_PATH_ID)
} ETHCAM_PATH_ID;

void EthCamSocket_Open(ETHSOCKIPC_ID id, ETHSOCKIPC_OPEN *pOpen);
void EthCamSocket_Close(void);
void EthCamSocket_SetDataRecvCB(ETHSOCKIPC_ID id, UINT32 value);
void EthCamSocket_SetCmdRecvCB(UINT32 value);
void EthCamSocket_SetDataNotifyCB(ETHSOCKIPC_ID id, UINT32 value);
void EthCamSocket_SetCmdNotifyCB(UINT32 value);
void EthCamSocket_SetInfo(ETHCAM_SOCKET_INFO* info);

void EthCamSocketCli_InstallID(void) _SECTION(".kercfg_text");
void EthCamSocketCli_Open(ETHCAM_PATH_ID path_id, ETHSOCKIPCCLI_ID id, ETHSOCKCLIIPC_OPEN *pOpen);
void EthCamSocketCli_ReConnect(ETHCAM_PATH_ID path_id ,UINT32 socket_id, UINT32 bStart);
UINT32 EthCamSocketCli_ReConnectIsStart(ETHCAM_PATH_ID path_id);
void EthCamSocketCli_Close(ETHCAM_PATH_ID path_id, ETHSOCKIPCCLI_ID id);

void  EthCamSocketCli_DataSetRawEncodeBuff(ETHCAM_PATH_ID path_id, ETHSOCKIPCCLI_ID id, ETHCAM_SOCKET_BUF_OBJ *BufObj);
ETHCAM_SOCKET_BUF_OBJ  EthCamSocketCli_DataGetRawEncodeBufObj(ETHCAM_PATH_ID path_id, ETHSOCKIPCCLI_ID id);
void  EthCamSocketCli_DataSetBsBuff(ETHCAM_PATH_ID path_id, ETHSOCKIPCCLI_ID id, ETHCAM_SOCKET_BUF_OBJ *BufObj);
ETHCAM_SOCKET_BUF_OBJ  EthCamSocketCli_DataGetBsBufObj(ETHCAM_PATH_ID path_id, ETHSOCKIPCCLI_ID id);
void  EthCamSocketCli_DataSetBsQueueMax(ETHCAM_PATH_ID path_id, ETHSOCKIPCCLI_ID id,UINT32 BsQMax);
UINT32  EthCamSocketCli_DataGetBsQueueMax(ETHCAM_PATH_ID path_id, ETHSOCKIPCCLI_ID id);
void EthCamSocketCli_SetDataRecvCB(ETHCAM_PATH_ID path_id, ETHSOCKIPCCLI_ID id, UINT32 value);
void EthCamSocketCli_SetCmdRecvCB(ETHCAM_PATH_ID path_id, UINT32 value);
void EthCamSocketCli_SetDataNotifyCB(ETHCAM_PATH_ID path_id, ETHSOCKIPCCLI_ID id, UINT32 value);
void EthCamSocketCli_SetCmdNotifyCB(ETHCAM_PATH_ID path_id, UINT32 value);
int EthCamSocket_Send(ETHCAM_PATH_ID path_id, ETHSOCKET_ID id, char* addr, int *size);
void EthCamSocketCli_SetCmdSendSizeCB(ETHCAM_PATH_ID path_id, UINT32 value);
void EthCamSocket_SetEthLinkStatus(ETHCAM_PATH_ID path_id, UINT32 bLink);
UINT32 EthCamSocket_Checksum(UINT8 *buf, UINT32 len);
void EthCamSocketCli_SetSvrInfo(ETHCAM_SOCKET_INFO* info, UINT32 num_info);
void EthCamSocketCli_SetDescSize(ETHCAM_PATH_ID path_id, ETHSOCKIPCCLI_ID id, UINT32 Size);

#endif //_ETHCAM_SOCKET_H
