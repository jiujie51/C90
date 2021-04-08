#ifndef _ETHCAMAPPSOCKET_H_
#define _ETHCAMAPPSOCKET_H_

#include "Type.h"
#include "EthsockIpcAPI.h"
#include "EthsockCliIpcAPI.h"
#include "EthCamSocket.h"

extern BOOL bflag_EthLinkFinish;

#define H264_NALU_TYPE_IDR      5
#define H264_NALU_TYPE_SLICE   1
#define H264_NALU_TYPE_SPS     7
#define H264_NALU_TYPE_PPS     8
#define H264_START_CODE_I	0x88
#define H264_START_CODE_P     	0x9A

#define H265_NALU_TYPE_IDR      19
#define H265_NALU_TYPE_SLICE   1
#define H265_NALU_TYPE_VPS     32
#define H265_NALU_TYPE_SPS     33
#define H265_NALU_TYPE_PPS     34

#define TS_VIDEO_RESERVED_SIZE 24 //align 4

typedef struct {
	ETHCAM_PATH_ID path_id;
	CHAR ParserBuf[2048];
	UINT32 BufSize;
} ETHCAM_SENDCMD_INFO;

typedef struct {
	UINT32           refCnt;
	UINT32           hData;
	MEM_RANGE	 mem;
	UINT32           blk_id;
	void            *pnext_bsdata;
} ETHCAM_BSDATA;

extern ETHCAM_SENDCMD_INFO sEthCamSendCmdInfo;

void  socketCliEthData1_SetRecv(ETHCAM_PATH_ID path_id, UINT32 bRecvData);
UINT32  socketCliEthData1_IsRecv(ETHCAM_PATH_ID path_id);

void  socketCliEthData2_SetRecv(ETHCAM_PATH_ID path_id, UINT32 bRecvData);
UINT32  socketCliEthData2_IsRecv(ETHCAM_PATH_ID path_id);
void  socketCliEthData2_SetResetI(ETHCAM_PATH_ID path_id, UINT32 Data);
void  socketCliEthData2_SetAllowPush(ETHCAM_PATH_ID path_id, UINT32 Data);

#if(defined(_NVT_ETHREARCAM_TX_))
void socketEthData_DestroyAllBuff(void);
void socketEthData_Open(ETHSOCKIPC_ID id);
void socketEthCmd_Open(void);
BOOL socketEthCmd_IsOpen(void);
void socketEth_Close(void);

#elif defined(_NVT_ETHREARCAM_RX_)
UINT32  socketCliEthData1_GetBsFrameCnt(ETHCAM_PATH_ID path_id);
INT32 socketCliEthData1_ConfigRecvBuf(ETHCAM_PATH_ID path_id);
INT32 socketCliEthData2_ConfigRecvBuf(ETHCAM_PATH_ID path_id);
void socketCliEthData_DestroyAllBuff(ETHCAM_PATH_ID path_id);
void socketCliEthData_Open(ETHCAM_PATH_ID path_id, ETHSOCKIPCCLI_ID id);
void socketCliEthCmd_Open(ETHCAM_PATH_ID path_id);
void socketCliEthData1_RecvCB(ETHCAM_PATH_ID path_id, char* addr, int size);
void socketCliEthData2_RecvCB(ETHCAM_PATH_ID path_id, char* addr, int size);
void socketCliEthData1_NotifyCB(ETHCAM_PATH_ID path_id, int status, int parm);
void socketCliEthData2_NotifyCB(ETHCAM_PATH_ID path_id, int status, int parm);
void socketCliEthCmd_NotifyCB(ETHCAM_PATH_ID path_id, int status, int parm);
void socketCliEthData1_RecvResetParam(ETHCAM_PATH_ID path_id);
void socketCliEthData2_RecvResetParam(ETHCAM_PATH_ID path_id);
void socketCliEthCmd_SendSizeCB(int size, int total_size);
BOOL socketCliEthCmd_IsConn(ETHCAM_PATH_ID path_id);
BOOL socketCliEthData1_IsConn(ETHCAM_PATH_ID path_id);
BOOL socketCliEthData2_IsConn(ETHCAM_PATH_ID path_id);
#endif

void socketEthCmd_RecvCB(char* addr, int size);
void socketEthCmd_NotifyCB(int status, int parm);

void socketCliEthCmd_RecvCB(ETHCAM_PATH_ID path_id, char* addr, int size);

#endif //_ETHCAMAPPSOCKET_H_
