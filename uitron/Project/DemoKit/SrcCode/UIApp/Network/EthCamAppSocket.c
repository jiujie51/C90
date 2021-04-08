#include "PrjCfg.h"

#if (defined(_NVT_ETHREARCAM_TX_) || defined(_NVT_ETHREARCAM_RX_))
#include "NVTToolCommand.h"
//#include "UIAppWiFiCmd.h"
#include "UIInfo.h"
#include "UsockIpcAPI.h"
#include "nvtmpp.h"
#include "SysCfg.h"
#include "WifiAppXML.h"
#include "UIAppPhoto.h"
#include "ImageUnit_VdoDec.h"
#include "ImageUnit_AudDec.h"
#include "UIAppMovie.h"
#include "UIAppNetwork.h"
#include "EthCamAppCmd.h"
#include "EthCamAppSocket.h"
#include "ImageApp_MovieCommon.h"
#include "ImageApp_MovieMulti.h"
#include "SysMain.h"
#include "SysCommon.h"
#include "UIMovieMapping.h"
#include "UIAppMovie.h"
#include "EthCamSocket.h"
#include "ImageUnit_Demux.h"
#include "UIModeUpdFw.h"
#include "movieinterface_def.h"
#include "UIModeWifi.h"
#include "HwClock.h"
#include "EthCamAppNetwork.h"

#define THIS_DBGLVL         2 // 0=FATAL, 1=ERR, 2=WRN, 3=UNIT, 4=FUNC, 5=IND, 6=MSG, 7=VALUE, 8=USER
///////////////////////////////////////////////////////////////////////////////
#define __MODULE__          EthCamAppSocket
#define __DBGLVL__          ((THIS_DBGLVL>=PRJ_DBG_LVL)?THIS_DBGLVL:PRJ_DBG_LVL)
#define __DBGFLT__          "*" //*=All, [mark]=CustomClass
#include "DebugModule.h"
///////////////////////////////////////////////////////////////////////////////

#define TBR_SIZE_RATIO  100//220//240

ETHCAM_SENDCMD_INFO sEthCamSendCmdInfo={0};

#if(defined(_NVT_ETHREARCAM_TX_))
extern int SX_TIMER_ETHCAM_LINKDET_ID;
static NVTMPP_VB_POOL g_SocketData1_Tx_SendPool[1]= {NVTMPP_VB_INVALID_POOL};
static UINT32 g_SocketData1_Tx_SendPoolAddr[1]={0};
static UINT32 g_SocketData1_Tx_SendAddr=0;

static NVTMPP_VB_POOL g_SocketData1_Tx_RawEncodePool[1]= {NVTMPP_VB_INVALID_POOL};
static UINT32 g_SocketData1_Tx_RawEncodePoolAddr[1]={0};
static UINT32 g_SocketData1_Tx_RawEncodeAddr=0;


static UINT32 g_IsSocketCmdOpen=0;

#if 1//(ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
static NVTMPP_VB_POOL g_SocketData2_Tx_SendPool[1]= {NVTMPP_VB_INVALID_POOL};
static UINT32 g_SocketData2_Tx_SendPoolAddr[1]={0};
static UINT32 g_SocketData2_Tx_SendAddr=0;
#endif
BOOL socketEthCmd_IsOpen(void)
{
	return g_IsSocketCmdOpen;
}

UINT32 socketEthData1_GetSendBufAddr(UINT32 blk_size)
{
        NVTMPP_VB_BLK  blk=0;
        CHAR pool_name[20] ={0};

        //DBG_DUMP("socketCli_GetRecvBufAddr blk_size=%d\r\n",blk_size);

        if(g_SocketData1_Tx_SendPool[0]==NVTMPP_VB_INVALID_POOL)  {
            sprintf(pool_name,"EthSocket_SendD1");
            g_SocketData1_Tx_SendPool[0]=nvtmpp_vb_create_pool(pool_name, blk_size , 1, NVTMPP_DDR_1);
            if (NVTMPP_VB_INVALID_POOL == g_SocketData1_Tx_SendPool[0])	{
        		DBG_ERR("create private pool err\r\n");
        		return NVTEVT_CONSUME;
            }

            blk = nvtmpp_vb_get_block(0, g_SocketData1_Tx_SendPool[0], blk_size, NVTMPP_DDR_1);
            if (NVTMPP_VB_INVALID_BLK == blk) {
        		DBG_ERR("get vb block err\r\n");
                	return NVTEVT_CONSUME;
            }
            g_SocketData1_Tx_SendPoolAddr[0]=nvtmpp_vb_block2addr(blk);
        }
        if(g_SocketData1_Tx_SendPoolAddr[0] == 0)
    		DBG_ERR("get buf addr err\r\n");
        return g_SocketData1_Tx_SendPoolAddr[0];
}
void socketEthData1_DestroySendBuff(void)
{
	INT32 i, ret;
	for (i=0;i<1;i++) {
		if(g_SocketData1_Tx_SendPool[i] != NVTMPP_VB_INVALID_POOL){
			ret = nvtmpp_vb_destroy_pool(g_SocketData1_Tx_SendPool[i]);
			if (NVTMPP_ER_OK != ret) {
				DBG_ERR("destory pool error pool 0x%x   ret=%d\r\n", g_SocketData1_Tx_SendPool[i], ret);
				break;
			}
			g_SocketData1_Tx_SendPool[i]=NVTMPP_VB_INVALID_POOL;
			g_SocketData1_Tx_SendPoolAddr[i]=0;
		}
	}
	g_SocketData1_Tx_SendAddr=0;
}
UINT32 socketEthData1_GetRawEncodeBufAddr(UINT32 blk_size)
{
        NVTMPP_VB_BLK  blk=0;
        CHAR pool_name[30] ={0};

        //DBG_DUMP("socketCli_GetRecvBufAddr blk_size=%d\r\n",blk_size);

        if(g_SocketData1_Tx_RawEncodePool[0]==NVTMPP_VB_INVALID_POOL)  {
            sprintf(pool_name,"socketCli_RawEncode");
            g_SocketData1_Tx_RawEncodePool[0]=nvtmpp_vb_create_pool(pool_name, blk_size , 1, NVTMPP_DDR_1);
            if (NVTMPP_VB_INVALID_POOL == g_SocketData1_Tx_RawEncodePool[0])	{
        		DBG_ERR("create private pool err\r\n");
        		return NVTEVT_CONSUME;
            }

            blk = nvtmpp_vb_get_block(0, g_SocketData1_Tx_RawEncodePool[0], blk_size, NVTMPP_DDR_1);
            if (NVTMPP_VB_INVALID_BLK == blk) {
        		DBG_ERR("get vb block err\r\n");
                	return NVTEVT_CONSUME;
            }
            g_SocketData1_Tx_RawEncodePoolAddr[0]=nvtmpp_vb_block2addr(blk);
        }
        if(g_SocketData1_Tx_RawEncodePoolAddr[0] == 0)
    		DBG_ERR("get buf addr err\r\n");
        return g_SocketData1_Tx_RawEncodePoolAddr[0];
}
void socketEthData1_DestroyRawEncodeBuff(void)
{
	INT32 i, ret;
	for (i=0;i<1;i++) {
		if(g_SocketData1_Tx_RawEncodePool[i] != NVTMPP_VB_INVALID_POOL){
			ret = nvtmpp_vb_destroy_pool(g_SocketData1_Tx_RawEncodePool[i]);
			if (NVTMPP_ER_OK != ret) {
				DBG_ERR("destory pool error pool 0x%x   ret=%d\r\n", g_SocketData1_Tx_RawEncodePool[i], ret);
				break;
			}
			g_SocketData1_Tx_RawEncodePool[i]=NVTMPP_VB_INVALID_POOL;
			g_SocketData1_Tx_RawEncodePoolAddr[i]=0;
		}
	}
	g_SocketData1_Tx_RawEncodeAddr=0;
}
UINT32 socketEthData2_GetSendBufAddr(UINT32 blk_size)
{
#if 1//(ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
        NVTMPP_VB_BLK  blk=0;
        CHAR pool_name[20] ={0};

        //DBG_DUMP("socketCli_GetRecvBufAddr blk_size=%d\r\n",blk_size);

        if(g_SocketData2_Tx_SendPool[0]==NVTMPP_VB_INVALID_POOL)  {
            sprintf(pool_name,"EthSocket_SendD2");
            g_SocketData2_Tx_SendPool[0]=nvtmpp_vb_create_pool(pool_name, blk_size , 1, NVTMPP_DDR_1);
            if (NVTMPP_VB_INVALID_POOL == g_SocketData2_Tx_SendPool[0])	{
        		DBG_ERR("create private pool err\r\n");
        		return NVTEVT_CONSUME;
            }

            blk = nvtmpp_vb_get_block(0, g_SocketData2_Tx_SendPool[0], blk_size, NVTMPP_DDR_1);
            if (NVTMPP_VB_INVALID_BLK == blk) {
        		DBG_ERR("get vb block err\r\n");
                	return NVTEVT_CONSUME;
            }
            g_SocketData2_Tx_SendPoolAddr[0]=nvtmpp_vb_block2addr(blk);
        }
        if(g_SocketData2_Tx_SendPoolAddr[0] == 0)
    		DBG_ERR("get buf addr err\r\n");
        return g_SocketData2_Tx_SendPoolAddr[0];
#else
        return 0;
#endif

}
void socketEthData2_DestroySendBuff(void)
{
#if 1//(ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
	INT32 i, ret;
	for (i=0;i<1;i++) {
		if(g_SocketData2_Tx_SendPool[i] != NVTMPP_VB_INVALID_POOL){
			ret = nvtmpp_vb_destroy_pool(g_SocketData2_Tx_SendPool[i]);
			if (NVTMPP_ER_OK != ret) {
				DBG_ERR("destory pool error pool 0x%x   ret=%d\r\n", g_SocketData2_Tx_SendPool[i], ret);
				break;
			}
			g_SocketData2_Tx_SendPool[i]=NVTMPP_VB_INVALID_POOL;
			g_SocketData2_Tx_SendPoolAddr[i]=0;
		}
	}
	g_SocketData2_Tx_SendAddr=0;
#endif
}
void socketEthData_DestroyAllBuff(void)
{
	socketEthData1_DestroySendBuff();
	socketEthData2_DestroySendBuff();
	socketEthData1_DestroyRawEncodeBuff();
}
void socketEthData1_RecvCB(char* addr, int size)
{
}
void socketEthData2_RecvCB(char* addr, int size)
{
}
void socketEthData1_NotifyCB(int status, int parm)
{
    if(status == CYG_ETHSOCKET_STATUS_CLIENT_CONNECT){
        DBG_DUMP("socketEthData1_NotifyCB Connect OK\r\n");
    }
    else if(status == CYG_ETHSOCKET_STATUS_CLIENT_REQUEST){
    }
    else if(status == CYG_ETHSOCKET_STATUS_CLIENT_DISCONNECT){
        DBG_DUMP("socketEthData1_NotifyCB DISConnect !!\r\n");
        SxTimer_SetFuncActive(SX_TIMER_ETHCAM_LINKDET_ID, FALSE);
    }
    else{
        DBG_ERR("^GUnknown status = %d, parm = %d\r\n", status, parm);
    }
}
void socketEthData2_NotifyCB(int status, int parm)
{
    if(status == CYG_ETHSOCKET_STATUS_CLIENT_CONNECT){
        DBG_DUMP("socketEthData2_NotifyCB Connect OK\r\n");
    }
    else if(status == CYG_ETHSOCKET_STATUS_CLIENT_REQUEST){
    }
    else if(status == CYG_ETHSOCKET_STATUS_CLIENT_DISCONNECT){
        DBG_DUMP("socketEthData2_NotifyCB DISConnect !!\r\n");
    }
    else{
        DBG_ERR("^GUnknown status = %d, parm = %d\r\n", status, parm);
    }
}
void socketEthData_Open(ETHSOCKIPC_ID id)
{
	EthCamCmdTsk_Open();
	ETHSOCKIPC_OPEN EthsockIpcOpen[2] = {0};
	if(id< ETHSOCKIPC_ID_0 || id>ETHSOCKIPC_ID_1){
		DBG_ERR("id out of range ,id=%d\r\n", id);
	}

	if(id== ETHSOCKIPC_ID_0){
	    	EthCamSocket_SetDataRecvCB(ETHSOCKIPC_ID_0, (UINT32)&socketEthData1_RecvCB);
	    	EthCamSocket_SetDataNotifyCB(ETHSOCKIPC_ID_0, (UINT32)&socketEthData1_NotifyCB);
		UINT32 JpgCompRatio = 10; // JPEG compression ratio
		UINT32 MaxRawEncodeSize=MovieExe_GetWidth(_CFG_REC_ID_1)*MovieExe_GetHeight(_CFG_REC_ID_1)*3/(2*JpgCompRatio);
		MaxRawEncodeSize*= 13/10;
		DBG_IND("MaxRawEncodeSize=%d\r\n",MaxRawEncodeSize);
		g_SocketData1_Tx_RawEncodeAddr=socketEthData1_GetRawEncodeBufAddr(MaxRawEncodeSize);
		ImageApp_MovieMulti_EthCamTxRecId1_SetRawEncodeBuff(g_SocketData1_Tx_RawEncodeAddr, MaxRawEncodeSize);

		// open
		EthsockIpcOpen[id].sharedSendMemAddr=socketEthData1_GetSendBufAddr((MovieExe_GetTBR(_CFG_REC_ID_1)*100/TBR_SIZE_RATIO));
		EthsockIpcOpen[id].sharedSendMemSize=(MovieExe_GetTBR(_CFG_REC_ID_1)*100/TBR_SIZE_RATIO);
		EthsockIpcOpen[id].sharedRecvMemAddr=OS_GetMempoolAddr(POOL_ID_ETHSOCK_IPC);
		EthsockIpcOpen[id].sharedRecvMemSize=POOL_SIZE_ETHSOCK_IPC;

		DBG_IND("D1 SendMemAddr=0x%x, RecvMemAddr=0x%x\r\n",EthsockIpcOpen[id].sharedSendMemAddr,EthsockIpcOpen[id].sharedRecvMemAddr);
		EthCamSocket_Open(id, &EthsockIpcOpen[id]);
	}else if(id== ETHSOCKIPC_ID_1){
	    	EthCamSocket_SetDataRecvCB(ETHSOCKIPC_ID_1, (UINT32)&socketEthData2_RecvCB);
	    	EthCamSocket_SetDataNotifyCB(ETHSOCKIPC_ID_1, (UINT32)&socketEthData2_NotifyCB);

		// open
		EthsockIpcOpen[id].sharedSendMemAddr=socketEthData2_GetSendBufAddr((MovieExe_GetTBR(_CFG_CLONE_ID_1)*100/TBR_SIZE_RATIO));
		EthsockIpcOpen[id].sharedSendMemSize=(MovieExe_GetTBR(_CFG_CLONE_ID_1)*100/TBR_SIZE_RATIO);
		EthsockIpcOpen[id].sharedRecvMemAddr=OS_GetMempoolAddr(POOL_ID_ETHSOCK_IPC)+POOL_SIZE_ETHSOCK_IPC;
		EthsockIpcOpen[id].sharedRecvMemSize=POOL_SIZE_ETHSOCK_IPC;

		DBG_IND("D2 SendMemAddr=0x%x, RecvMemAddr=0x%x\r\n",EthsockIpcOpen[id].sharedSendMemAddr,EthsockIpcOpen[id].sharedRecvMemAddr);
		EthCamSocket_Open(id, &EthsockIpcOpen[id]);
	}
}
void socketEthCmd_Open(void)
{
	ETHSOCKIPC_OPEN EthsockIpcOpen = {0};

    	EthCamSocket_SetCmdRecvCB((UINT32)&socketEthCmd_RecvCB);
    	EthCamSocket_SetCmdNotifyCB((UINT32)&socketEthCmd_NotifyCB);

	EthsockIpcOpen.sharedSendMemAddr=OS_GetMempoolAddr(POOL_ID_ETHSOCK_IPC)+2*POOL_SIZE_ETHSOCK_IPC;
	EthsockIpcOpen.sharedSendMemSize=POOL_SIZE_ETHSOCK_IPC;

	EthsockIpcOpen.sharedRecvMemAddr=OS_GetMempoolAddr(POOL_ID_ETHSOCK_IPC)+3*POOL_SIZE_ETHSOCK_IPC;
	EthsockIpcOpen.sharedRecvMemSize=POOL_SIZE_ETHSOCK_IPC;

	EthCamSocket_Open(ETHSOCKIPC_ID_2, &EthsockIpcOpen);
	g_IsSocketCmdOpen=1;
}
void socketEth_Close(void)
{
	g_IsSocketCmdOpen=0;
	EthCamSocket_Close();
}

#endif
#if(defined(_NVT_ETHREARCAM_RX_))
extern UINT32 EthCamHB1[ETHCAM_PATH_ID_MAX], EthCamHB2;
extern MOVIE_RECODE_FILE_OPTION gMovie_Rec_Option;
extern int SX_TIMER_ETHCAM_ETHLINKRETRY_ID;

static UINT32 g_SocketCliData1_RecvAddr[ETHCAM_PATH_ID_MAX]={0};
static UINT32 g_SocketCliData1_BsBufTotalAddr[ETHCAM_PATH_ID_MAX]={0}; //tbl Buf+BsFrmBuf
static UINT32 g_SocketCliData1_BsBufTotalSize[ETHCAM_PATH_ID_MAX]={0};
static UINT32 g_SocketCliData1_BsQueueMax[ETHCAM_PATH_ID_MAX]={0};

UINT32 g_SocketCliData1_RawEncodeAddr[ETHCAM_PATH_ID_MAX]={0};

static UINT32 g_SocketCliData1_BsFrameCnt[ETHCAM_PATH_ID_MAX]={0};
static UINT32 g_SocketCliData1_RecBsFrameCnt[ETHCAM_PATH_ID_MAX]={0};
static UINT32 g_SocketCliData1_eth_i_cnt[ETHCAM_PATH_ID_MAX]={0};
static UINT32 g_SocketCliData1_LongCntTxRxOffset[ETHCAM_PATH_ID_MAX]={0};

static NVTMPP_VB_POOL g_SocketCliData1_RecvPool[ETHCAM_PATH_ID_MAX][1]= {NVTMPP_VB_INVALID_POOL, NVTMPP_VB_INVALID_POOL};
static UINT32 g_SocketCliData1_RecvPoolAddr[ETHCAM_PATH_ID_MAX][1]={0};
static NVTMPP_VB_POOL g_SocketCliData1_RawEncodePool[ETHCAM_PATH_ID_MAX][1]= {NVTMPP_VB_INVALID_POOL, NVTMPP_VB_INVALID_POOL};
static UINT32 g_SocketCliData1_RawEncodePoolAddr[ETHCAM_PATH_ID_MAX][1]={0};
NVTMPP_VB_POOL g_SocketCliData1_BsPool[ETHCAM_PATH_ID_MAX][1]= {NVTMPP_VB_INVALID_POOL, NVTMPP_VB_INVALID_POOL};
static UINT32 _SocketCliData1_BsPoolAddr[ETHCAM_PATH_ID_MAX][1]={0};


#if (ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
static UINT32 g_SocketCliData2_RecvAddr[ETHCAM_PATH_ID_MAX]={0};
static UINT32 g_SocketCliData2_BsBufTotalAddr[ETHCAM_PATH_ID_MAX]={0}; //tbl Buf+BsFrmBuf
static UINT32 g_SocketCliData2_BsBufTotalSize[ETHCAM_PATH_ID_MAX]={0};
static UINT32 g_SocketCliData2_BsQueueMax[ETHCAM_PATH_ID_MAX]={0};

UINT32 g_SocketCliData2_RawEncodeAddr[ETHCAM_PATH_ID_MAX]={0};
static UINT32 g_SocketCliData2_BsFrameCnt[ETHCAM_PATH_ID_MAX]={0};
static UINT32 g_SocketCliData2_eth_i_cnt[ETHCAM_PATH_ID_MAX]={0};
static UINT32 g_SocketCliData2_LongCntTxRxOffset[ETHCAM_PATH_ID_MAX]={0};

static NVTMPP_VB_POOL g_SocketCliData2_RecvPool[ETHCAM_PATH_ID_MAX][1]= {NVTMPP_VB_INVALID_POOL, NVTMPP_VB_INVALID_POOL};
static UINT32 g_SocketCliData2_RecvPoolAddr[ETHCAM_PATH_ID_MAX][1]={0};
NVTMPP_VB_POOL g_SocketCliData2_BsPool[ETHCAM_PATH_ID_MAX][1]= {NVTMPP_VB_INVALID_POOL, NVTMPP_VB_INVALID_POOL};
static UINT32 _SocketCliData2_BsPoolAddr[ETHCAM_PATH_ID_MAX][1]={0};
#endif

static UINT32 g_IsSocketCliData1_RecvData[ETHCAM_PATH_ID_MAX]={0};
static UINT32 g_IsSocketCliData1_Conn[ETHCAM_PATH_ID_MAX]={0};
static UINT32 g_IsSocketCliCmdConn[ETHCAM_PATH_ID_MAX]={0};

static UINT32 g_IsSocketCliData2_RecvData[ETHCAM_PATH_ID_MAX]={0};
static UINT32 g_IsSocketCliData2_Conn[ETHCAM_PATH_ID_MAX]={0};
UINT32  socketCliEthData1_GetBsFrameCnt(ETHCAM_PATH_ID path_id)
{
	return g_SocketCliData1_BsFrameCnt[path_id];
}
void  socketCliEthData1_SetRecv(ETHCAM_PATH_ID path_id, UINT32 bRecvData)
{
	g_IsSocketCliData1_RecvData[path_id]=bRecvData;
}
UINT32  socketCliEthData1_IsRecv(ETHCAM_PATH_ID path_id)
{
	return g_IsSocketCliData1_RecvData[path_id];
}
void  socketCliEthData2_SetRecv(ETHCAM_PATH_ID path_id, UINT32 bRecvData)
{
	g_IsSocketCliData2_RecvData[path_id]=bRecvData;
}
UINT32  socketCliEthData2_IsRecv(ETHCAM_PATH_ID path_id)
{
	return g_IsSocketCliData2_RecvData[path_id];
}


UINT32 socketCliEthData1_GetRecvBufAddr(ETHCAM_PATH_ID path_id, UINT32 blk_size)
{
        NVTMPP_VB_BLK  blk=0;
        CHAR pool_name[20] ={0};

        //DBG_DUMP("socketCli_GetRecvBufAddr blk_size=%d\r\n",blk_size);

        if(g_SocketCliData1_RecvPool[path_id][0]==NVTMPP_VB_INVALID_POOL && !System_IsModeChgClose())  {
            sprintf(pool_name,"socketCli_RecvD1_%d",path_id);
			
            g_SocketCliData1_RecvPool[path_id][0]=nvtmpp_vb_create_pool(pool_name, blk_size , 1, NVTMPP_DDR_1);
            if (NVTMPP_VB_INVALID_POOL == g_SocketCliData1_RecvPool[path_id][0])	{
        		DBG_ERR("create private pool err\r\n");
        		return NVTEVT_CONSUME;
            }

            blk = nvtmpp_vb_get_block(0, g_SocketCliData1_RecvPool[path_id][0], blk_size, NVTMPP_DDR_1);
            if (NVTMPP_VB_INVALID_BLK == blk) {
        		DBG_ERR("get vb block err\r\n");
                	return NVTEVT_CONSUME;
            }
            g_SocketCliData1_RecvPoolAddr[path_id][0]=nvtmpp_vb_block2addr(blk);
        }
        if(g_SocketCliData1_RecvPoolAddr[path_id][0] == 0)
    		DBG_ERR("get buf addr err\r\n");
		//CHKPNT;
		debug_msg("zjf create private pool socketCli_RecvD1_%d successful \r\n",path_id);
        return g_SocketCliData1_RecvPoolAddr[path_id][0];
}
void socketCliEthData1_DestroyRecvBuff(ETHCAM_PATH_ID path_id)
{
	INT32 i, ret;
	for (i=0;i<1;i++) {
		//DBGD(path_id);
		//DBGD(i);
		//DBGD(g_SocketCliData1_RecvPool[path_id][i]);
		if(g_SocketCliData1_RecvPool[path_id][i] != NVTMPP_VB_INVALID_POOL){
			ret = nvtmpp_vb_destroy_pool(g_SocketCliData1_RecvPool[path_id][i]);
			if (NVTMPP_ER_OK != ret) {
				DBG_ERR("destory pool error pool 0x%x   ret=%d\r\n", g_SocketCliData1_RecvPool[path_id][i], ret);
				break;
			}
			//CHKPNT;
			//debug_msg("zjf destroy private pool socketCli_RecvD1_%d successful \r\n",path_id);
			g_SocketCliData1_RecvPool[path_id][i]=NVTMPP_VB_INVALID_POOL;
			g_SocketCliData1_RecvPoolAddr[path_id][i]=0;
		}
	}
	g_SocketCliData1_RecvAddr[path_id]=0;
}
UINT32 socketCliEthData1_GetRawEncodeBufAddr(ETHCAM_PATH_ID path_id, UINT32 blk_size)
{
        NVTMPP_VB_BLK  blk=0;
        CHAR pool_name[30] ={0};

        //DBG_DUMP("socketCli_GetRecvBufAddr blk_size=%d\r\n",blk_size);

        if(g_SocketCliData1_RawEncodePool[path_id][0]==NVTMPP_VB_INVALID_POOL && !System_IsModeChgClose())  {
            sprintf(pool_name,"socketCli_RawEncode_%d",path_id);
			//CHKPNT;
            g_SocketCliData1_RawEncodePool[path_id][0]=nvtmpp_vb_create_pool(pool_name, blk_size , 1, NVTMPP_DDR_1);
            if (NVTMPP_VB_INVALID_POOL == g_SocketCliData1_RawEncodePool[path_id][0])	{
        		DBG_ERR("create private pool err\r\n");
        		return NVTEVT_CONSUME;
            }

            blk = nvtmpp_vb_get_block(0, g_SocketCliData1_RawEncodePool[path_id][0], blk_size, NVTMPP_DDR_1);
            if (NVTMPP_VB_INVALID_BLK == blk) {
        		DBG_ERR("get vb block err\r\n");
                	return NVTEVT_CONSUME;
            }
            g_SocketCliData1_RawEncodePoolAddr[path_id][0]=nvtmpp_vb_block2addr(blk);
        }
        if(g_SocketCliData1_RawEncodePoolAddr[path_id][0] == 0)
    		DBG_ERR("get buf addr err\r\n");
        return g_SocketCliData1_RawEncodePoolAddr[path_id][0];
}
void socketCliEthData1_DestroyRawEncodeBuff(ETHCAM_PATH_ID path_id)
{//CHKPNT;
	INT32 i, ret;
	for (i=0;i<1;i++) {
		if(g_SocketCliData1_RawEncodePool[path_id][i] != NVTMPP_VB_INVALID_POOL){
			//CHKPNT;
			ret = nvtmpp_vb_destroy_pool(g_SocketCliData1_RawEncodePool[path_id][i]);
			if (NVTMPP_ER_OK != ret) {
				DBG_ERR("destory pool error pool 0x%x   ret=%d\r\n", g_SocketCliData1_RawEncodePool[path_id][i], ret);
				break;
			}
			g_SocketCliData1_RawEncodePool[path_id][i]=NVTMPP_VB_INVALID_POOL;
			g_SocketCliData1_RawEncodePoolAddr[path_id][i]=0;
		}
	}
	g_SocketCliData1_RawEncodeAddr[path_id]=0;
}
UINT32 socketCliEthData1_GetBsBufAddr(ETHCAM_PATH_ID path_id, UINT32 blk_size)
{
        NVTMPP_VB_BLK  blk=0;
        CHAR pool_name[20] ={0};

        DBG_IND("eth_GetBsFrmBufAddr blk_size=%d\r\n",blk_size);
        if(g_SocketCliData1_BsPool[path_id][0]==NVTMPP_VB_INVALID_POOL && !System_IsModeChgClose() )  {
            sprintf(pool_name,"socketCli_BsD1_%d",path_id);
			//CHKPNT;
            g_SocketCliData1_BsPool[path_id][0]=nvtmpp_vb_create_pool(pool_name, blk_size , 1, NVTMPP_DDR_1);
            if (NVTMPP_VB_INVALID_POOL == g_SocketCliData1_BsPool[path_id][0])	{
        		DBG_ERR("create private pool err\r\n");
        		return NVTEVT_CONSUME;
            }

            blk = nvtmpp_vb_get_block(0, g_SocketCliData1_BsPool[path_id][0], blk_size, NVTMPP_DDR_1);
            if (NVTMPP_VB_INVALID_BLK == blk) {
        		DBG_ERR("get vb block err\r\n");
                	return NVTEVT_CONSUME;
            }
            _SocketCliData1_BsPoolAddr[path_id][0]=nvtmpp_vb_block2addr(blk);
        }
        if(_SocketCliData1_BsPoolAddr[path_id][0] == 0)
    		DBG_ERR("get buf addr err\r\n");
        return _SocketCliData1_BsPoolAddr[path_id][0];
}
void socketCliEthData1_DestroyBsBuff(ETHCAM_PATH_ID path_id)
{//CHKPNT;
	INT32 i, ret;
	for (i=0;i<1;i++) {
		if(g_SocketCliData1_BsPool[path_id][i] != NVTMPP_VB_INVALID_POOL){
			//CHKPNT;
			ret = nvtmpp_vb_destroy_pool(g_SocketCliData1_BsPool[path_id][i]);
			if (NVTMPP_ER_OK != ret) {
				DBG_ERR("destory pool error pool 0x%x   ret=%d\r\n", g_SocketCliData1_BsPool[path_id][i], ret);
				break;
			}
			g_SocketCliData1_BsPool[path_id][i]=NVTMPP_VB_INVALID_POOL;
			_SocketCliData1_BsPoolAddr[path_id][i]=0;
		}
	}
	g_SocketCliData1_BsBufTotalAddr[path_id]=0;
}
INT32 socketCliEthData1_ConfigRecvBuf(ETHCAM_PATH_ID path_id)
{
#if 0
	if(sEthCamTxDecInfo.Tbr !=0 && sEthCamTxDecInfo.bStarupOK==1){
		g_SocketCliData1_RecvAddr=socketCliEthData1_GetRecvBufAddr((sEthCamTxDecInfo.Tbr*100/285));
	}else{
		g_SocketCliData1_RecvAddr=socketCliEthData1_GetRecvBufAddr(MAX_I_FRAME_SZIE);
	}
#endif
	if(MovieExe_GetCommonMemInitFinish()==0){
		DBG_ERR("Movie Mem not init[%d]\r\n",path_id);
		return -1;
	}
	UINT32 JpgCompRatio = 10; // JPEG compression ratio
	UINT32 MaxRawEncodeSize = sEthCamTxRecInfo[path_id].width * sEthCamTxRecInfo[path_id].height * 3 / (2 * JpgCompRatio);
	//MaxRawEncodeSize*= 13/10;
	MaxRawEncodeSize= (MaxRawEncodeSize*13)/10;
	DBG_IND("MaxRawEncodeSize=%d\r\n",MaxRawEncodeSize);
	if((g_SocketCliData1_RawEncodeAddr[path_id]=socketCliEthData1_GetRawEncodeBufAddr(path_id, MaxRawEncodeSize))==NVTEVT_CONSUME){
		return -1;
	}
	//TBR* (EMR RollbackSec + 3) + MapTbl= FPS* (EMR RollbackSec + 3)
	g_SocketCliData1_BsBufTotalSize[path_id] = (MovieExe_GetEthcamEncBufSec(path_id) + MovieExe_GetEmrRollbackSec()) * sEthCamTxRecInfo[path_id].tbr + sizeof(UINT32) * (MovieExe_GetEthcamEncBufSec(path_id) + MovieExe_GetEmrRollbackSec()) * sEthCamTxRecInfo[path_id].vfr;
	if (g_SocketCliData1_BsBufTotalSize[path_id]==0) {
		DBG_ERR("EthBsFrmBufTotalSize[%d] =0 !!\r\n",path_id);
	}
	if((g_SocketCliData1_BsBufTotalAddr[path_id]=socketCliEthData1_GetBsBufAddr(path_id, g_SocketCliData1_BsBufTotalSize[path_id]))==NVTEVT_CONSUME){
		return -1;
	}
	DBG_IND("g_SocketCliData1_BsBufTotalAddr[%d]=0x%x, g_SocketCliData1_BsBufTotalSize[%d]=%d\r\n",path_id,g_SocketCliData1_BsBufTotalAddr[path_id],path_id,g_SocketCliData1_BsBufTotalSize[path_id]);

#if 0
	g_SocketCliData1_BsBufMapTbl = (UINT32*)g_SocketCliData1_BsBufTotalAddr;
	//DBG_DUMP("g_EthBsFrmBufMapTbl=0x%x, g_EthBsFrmTotalAddr=0x%x, end=0x%x\r\n",g_EthBsFrmBufMapTbl,g_EthBsFrmTotalAddr,g_EthBsFrmTotalAddr+g_EthBsFrmBufTotalSize);

	memset((UINT8*)g_SocketCliData1_BsBufTotalAddr, 0, g_SocketCliData1_BsBufTotalSize);
	g_SocketCliData1_BsAddr = (g_SocketCliData1_BsBufTotalAddr + sizeof(UINT32)*(3+ MovieExe_GetEmrRollbackSec())* MovieExe_GetFps(_CFG_REC_ID_1));
	g_SocketCliData1_BsBufTotalSize-= (sizeof(UINT32)*(3+ MovieExe_GetEmrRollbackSec())* MovieExe_GetFps(_CFG_REC_ID_1));
	//DBG_DUMP("1EthBsFrmAddr=0x%x, EthBsFrmBufTotalSize=0x%x, end=0x%x\r\n",g_EthBsFrmAddr,g_EthBsFrmBufTotalSize,g_EthBsFrmAddr+g_EthBsFrmBufTotalSize);
	while (g_SocketCliData1_BsAddr % 4 != 0)
	{
		g_SocketCliData1_BsAddr += 1;
	}
	g_SocketCliData1_BsBufTotalSize-=4;
	//DBG_DUMP("2EthBsFrmAddr=0x%x, EthBsFrmBufTotalSize=0x%x, end=0x%x\r\n",g_EthBsFrmAddr,g_EthBsFrmBufTotalSize,g_EthBsFrmAddr+g_EthBsFrmBufTotalSize);
#endif
	ETHCAM_SOCKET_BUF_OBJ BufObj={0};
	BufObj.ParamAddr=g_SocketCliData1_BsBufTotalAddr[path_id];
	BufObj.ParamSize=g_SocketCliData1_BsBufTotalSize[path_id];
	EthCamSocketCli_DataSetBsBuff(path_id, ETHSOCKIPCCLI_ID_0, &BufObj);

	BufObj.ParamAddr=g_SocketCliData1_RawEncodeAddr[path_id];
	BufObj.ParamSize=MaxRawEncodeSize;
	EthCamSocketCli_DataSetRawEncodeBuff(path_id, ETHSOCKIPCCLI_ID_0, &BufObj);
	g_SocketCliData1_BsQueueMax[path_id]=((MovieExe_GetEthcamEncBufSec(path_id)+ MovieExe_GetEmrRollbackSec())* sEthCamTxRecInfo[path_id].vfr);
	EthCamSocketCli_DataSetBsQueueMax(path_id, ETHSOCKIPCCLI_ID_0, g_SocketCliData1_BsQueueMax[path_id]);
	return 0;
}


UINT32 socketCliEthData2_GetRecvBufAddr(ETHCAM_PATH_ID path_id, UINT32 blk_size)
{
#if (ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
        NVTMPP_VB_BLK  blk=0;
        CHAR pool_name[20] ={0};

        //DBG_DUMP("socketCli_GetRecvBufAddr blk_size=%d\r\n",blk_size);

        if(g_SocketCliData2_RecvPool[path_id][0]==NVTMPP_VB_INVALID_POOL && !System_IsModeChgClose())  {
            sprintf(pool_name,"socketCli_RecvD2_%d",path_id);
			//CHKPNT;
            g_SocketCliData2_RecvPool[path_id][0]=nvtmpp_vb_create_pool(pool_name, blk_size , 1, NVTMPP_DDR_1);
            if (NVTMPP_VB_INVALID_POOL == g_SocketCliData2_RecvPool[path_id][0])	{
        		DBG_ERR("create private pool err\r\n");
        		return NVTEVT_CONSUME;
            }

            blk = nvtmpp_vb_get_block(0, g_SocketCliData2_RecvPool[path_id][0], blk_size, NVTMPP_DDR_1);
            if (NVTMPP_VB_INVALID_BLK == blk) {
        		DBG_ERR("get vb block err\r\n");
                	return NVTEVT_CONSUME;
            }
            g_SocketCliData2_RecvPoolAddr[path_id][0]=nvtmpp_vb_block2addr(blk);
        }
        if(g_SocketCliData2_RecvPoolAddr[path_id][0] == 0)
    		DBG_ERR("get buf addr err\r\n");
        return g_SocketCliData2_RecvPoolAddr[path_id][0];
#else
	return 0;
#endif
}
void socketCliEthData2_DestroyRecvBuff(ETHCAM_PATH_ID path_id)
{//CHKPNT;
#if (ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
	INT32 i, ret;
	for (i=0;i<1;i++) {
		if(g_SocketCliData2_RecvPool[path_id][i] != NVTMPP_VB_INVALID_POOL){
			//CHKPNT;
			ret = nvtmpp_vb_destroy_pool(g_SocketCliData2_RecvPool[path_id][i]);
			if (NVTMPP_ER_OK != ret) {
				DBG_ERR("destory pool error pool 0x%x   ret=%d\r\n", g_SocketCliData2_RecvPool[path_id][i], ret);
				break;
			}
			g_SocketCliData2_RecvPool[path_id][i]=NVTMPP_VB_INVALID_POOL;
			g_SocketCliData2_RecvPoolAddr[path_id][i]=0;
		}
	}
	g_SocketCliData2_RecvAddr[path_id]=0;
#endif
}

UINT32 socketCliEthData2_GetBsBufAddr(ETHCAM_PATH_ID path_id, UINT32 blk_size)
{
#if (ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
        NVTMPP_VB_BLK  blk=0;
        CHAR pool_name[20] ={0};

        DBG_IND("eth_GetBsFrmBufAddr blk_size[%d]=%d\r\n",path_id,blk_size);
        if(g_SocketCliData2_BsPool[path_id][0]==NVTMPP_VB_INVALID_POOL  && !System_IsModeChgClose())  {
            sprintf(pool_name,"socketCli_BsD2_%d",path_id);
			//CHKPNT;
            g_SocketCliData2_BsPool[path_id][0]=nvtmpp_vb_create_pool(pool_name, blk_size , 1, NVTMPP_DDR_1);
            if (NVTMPP_VB_INVALID_POOL == g_SocketCliData2_BsPool[path_id][0])	{
        		DBG_ERR("create private pool err\r\n");
        		return NVTEVT_CONSUME;
            }

            blk = nvtmpp_vb_get_block(0, g_SocketCliData2_BsPool[path_id][0], blk_size, NVTMPP_DDR_1);
            if (NVTMPP_VB_INVALID_BLK == blk) {
        		DBG_ERR("get vb block err\r\n");
                	return NVTEVT_CONSUME;
            }
            _SocketCliData2_BsPoolAddr[path_id][0]=nvtmpp_vb_block2addr(blk);
        }
        if(_SocketCliData2_BsPoolAddr[path_id][0] == 0)
    		DBG_ERR("get buf addr err\r\n");
        return _SocketCliData2_BsPoolAddr[path_id][0];
#else
	return 0;
#endif
}
void socketCliEthData2_DestroyBsBuff(ETHCAM_PATH_ID path_id)
{//CHKPNT;
#if (ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
	INT32 i, ret;
	for (i=0;i<1;i++) {
		if(g_SocketCliData2_BsPool[path_id][i] != NVTMPP_VB_INVALID_POOL){
			//CHKPNT;
			ret = nvtmpp_vb_destroy_pool(g_SocketCliData2_BsPool[path_id][i]);
		//CHKPNT;
			if (NVTMPP_ER_OK != ret) {
				DBG_ERR("destory pool error pool 0x%x   ret=%d\r\n", g_SocketCliData2_BsPool[path_id][i], ret);
				break;
			}
			g_SocketCliData2_BsPool[path_id][i]=NVTMPP_VB_INVALID_POOL;
			_SocketCliData2_BsPoolAddr[path_id][i]=0;
		}
	}
	g_SocketCliData2_BsBufTotalAddr[path_id]=0;
#endif
}
INT32 socketCliEthData2_ConfigRecvBuf(ETHCAM_PATH_ID path_id)
{
#if(ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
	if(MovieExe_GetCommonMemInitFinish()==0){
		DBG_ERR("Movie Mem not init[%d]\r\n",path_id);
		return -1;
	}

	if(sEthCamTxDecInfo[path_id].Tbr !=0 && sEthCamTxDecInfo[path_id].bStarupOK==1){
		//CHKPNT;
		if((g_SocketCliData2_RecvAddr[path_id]=socketCliEthData2_GetRecvBufAddr(path_id, (sEthCamTxDecInfo[path_id].Tbr*100/TBR_SIZE_RATIO)))==NVTEVT_CONSUME){
			return -1;
		}
	}else{
		//CHKPNT;
		if((g_SocketCliData2_RecvAddr[path_id]=socketCliEthData2_GetRecvBufAddr(path_id, MAX_I_FRAME_SZIE))==NVTEVT_CONSUME){
			return -1;
		}
	}
	//TBR* (EMR RollbackSec + 3) + MapTbl= FPS* (EMR RollbackSec + 3)
	//g_SocketCliData2_BsBufTotalSize[path_id]  = (MovieExe_GetEthcamEncBufSec(path_id)+ MovieExe_GetEmrRollbackSec())*sEthCamTxDecInfo[path_id].Tbr+ sizeof(UINT32)*(MovieExe_GetEthcamEncBufSec(path_id)+ MovieExe_GetEmrRollbackSec())* sEthCamTxDecInfo[path_id].Fps;
	g_SocketCliData2_BsBufTotalSize[path_id]  = (1)*sEthCamTxDecInfo[path_id].Tbr+ sizeof(UINT32)*(1)* sEthCamTxDecInfo[path_id].Fps;
	if(g_SocketCliData2_BsBufTotalSize[path_id]==0){
		DBG_ERR("EthBsFrmBufTotalSize =0 !!\r\n");
	}
	if((g_SocketCliData2_BsBufTotalAddr[path_id]=socketCliEthData2_GetBsBufAddr(path_id, g_SocketCliData2_BsBufTotalSize[path_id]))==NVTEVT_CONSUME){
		return -1;
	}
#if 0
	g_SocketCliData2_BsBufMapTbl = (UINT32*)g_SocketCliData2_BsBufTotalAddr;
	//DBG_DUMP("g_EthBsFrmBufMapTbl=0x%x, g_EthBsFrmTotalAddr=0x%x, end=0x%x\r\n",g_EthBsFrmBufMapTbl,g_EthBsFrmTotalAddr,g_EthBsFrmTotalAddr+g_EthBsFrmBufTotalSize);

	memset((UINT8*)g_SocketCliData2_BsBufTotalAddr, 0, g_SocketCliData2_BsBufTotalSize);
	g_SocketCliData2_BsAddr = (g_SocketCliData2_BsBufTotalAddr + sizeof(UINT32)*(3+ MovieExe_GetEmrRollbackSec())* MovieExe_GetFps(_CFG_CLONE_ID_1));
	g_SocketCliData2_BsBufTotalSize-= (sizeof(UINT32)*(3+ MovieExe_GetEmrRollbackSec())* MovieExe_GetFps(_CFG_CLONE_ID_1));
	//DBG_DUMP("1EthBsFrmAddr=0x%x, EthBsFrmBufTotalSize=0x%x, end=0x%x\r\n",g_EthBsFrmAddr,g_EthBsFrmBufTotalSize,g_EthBsFrmAddr+g_EthBsFrmBufTotalSize);
	while (g_SocketCliData2_BsAddr % 4 != 0)
	{
		g_SocketCliData2_BsAddr += 1;
	}
	g_SocketCliData2_BsBufTotalSize-=4;
	//DBG_DUMP("2EthBsFrmAddr=0x%x, EthBsFrmBufTotalSize=0x%x, end=0x%x\r\n",g_EthBsFrmAddr,g_EthBsFrmBufTotalSize,g_EthBsFrmAddr+g_EthBsFrmBufTotalSize);
#endif
	ETHCAM_SOCKET_BUF_OBJ BufObj={0};
	BufObj.ParamAddr=g_SocketCliData2_BsBufTotalAddr[path_id];
	BufObj.ParamSize=g_SocketCliData2_BsBufTotalSize[path_id];
	EthCamSocketCli_DataSetBsBuff(path_id, ETHSOCKIPCCLI_ID_1, &BufObj);

	//g_SocketCliData2_BsQueueMax[path_id]=((MovieExe_GetEthcamEncBufSec(path_id)+ MovieExe_GetEmrRollbackSec())* sEthCamTxDecInfo[path_id].Fps);
	g_SocketCliData2_BsQueueMax[path_id]=((1)* sEthCamTxDecInfo[path_id].Fps);
	EthCamSocketCli_DataSetBsQueueMax(path_id, ETHSOCKIPCCLI_ID_1, g_SocketCliData2_BsQueueMax[path_id]);
#endif
	return 0;
}

void socketCliEthData_DestroyAllBuff(ETHCAM_PATH_ID path_id)
{
	//CHKPNT;
	socketCliEthData1_DestroyRecvBuff(path_id);
	//CHKPNT;
	socketCliEthData1_DestroyRawEncodeBuff(path_id);
	socketCliEthData1_DestroyBsBuff(path_id);
#if (ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
	socketCliEthData2_DestroyRecvBuff(path_id);
	socketCliEthData2_DestroyBsBuff(path_id);
	//CHKPNT;
#endif
}

void socketCliEthData_Open(ETHCAM_PATH_ID path_id, ETHSOCKIPCCLI_ID id)
{
	//EthCamCmdTsk_Open();
	ETHSOCKCLIIPC_OPEN EthsockCliIpcOpen[ETHCAM_PATH_ID_MAX][2] = {0};
	if(id< ETHSOCKIPCCLI_ID_0 || id>ETHSOCKIPCCLI_ID_1){
		DBG_ERR("id out of range ,id=%d\r\n", id);
	}
	if(path_id>=ETHCAM_PATH_ID_MAX){
		DBG_ERR("path_id out of range ,path_id=%d\r\n", path_id);
	}

	if(id== ETHSOCKIPCCLI_ID_0){
		//CHKPNT;
		if(socketCliEthData1_ConfigRecvBuf(path_id)==-1){
			return;
		}
	    EthCamSocketCli_SetDataRecvCB(path_id, ETHSOCKIPCCLI_ID_0, (UINT32)&socketCliEthData1_RecvCB);
		EthCamSocketCli_SetDataNotifyCB(path_id, ETHSOCKIPCCLI_ID_0, (UINT32)&socketCliEthData1_NotifyCB);
		socketCliEthData1_RecvResetParam(path_id);
		// open
		EthsockCliIpcOpen[path_id][id].sharedSendMemAddr=OS_GetMempoolAddr(POOL_ID_ETHSOCKCLI_IPC)+(POOL_CNT_ETHSOCKCLI_IPC/ETH_REARCAM_CAPS_COUNT)*path_id*POOL_SIZE_ETHSOCKCLI_IPC  ;
		EthsockCliIpcOpen[path_id][id].sharedSendMemSize=POOL_SIZE_ETHSOCKCLI_IPC;
		if(sEthCamTxRecInfo[path_id].tbr!=0 && sEthCamTxDecInfo[path_id].bStarupOK==1){
			//CHKPNT;
			DBGD(sEthCamTxRecInfo[path_id].tbr);
			g_SocketCliData1_RecvAddr[path_id]=socketCliEthData1_GetRecvBufAddr(path_id, (sEthCamTxRecInfo[path_id].tbr *100/TBR_SIZE_RATIO));
			EthsockCliIpcOpen[path_id][id].sharedRecvMemSize=(sEthCamTxRecInfo[path_id].tbr*100/TBR_SIZE_RATIO);
		}else{
			//CHKPNT;
			g_SocketCliData1_RecvAddr[path_id]=socketCliEthData1_GetRecvBufAddr(path_id, MAX_I_FRAME_SZIE);
			EthsockCliIpcOpen[path_id][id].sharedRecvMemSize=MAX_I_FRAME_SZIE;
		}
		EthsockCliIpcOpen[path_id][id].sharedRecvMemAddr=g_SocketCliData1_RecvAddr[path_id];
		//DBG_DUMP("sEthCamTxRecInfo.tbr=%d, sharedRecvMemSize%d\r\n", sEthCamTxRecInfo.tbr,EthsockCliIpcOpen[id].sharedRecvMemSize );
		DBG_IND("Cli[%d] D1 SendMemAddr=0x%x, RecvMemAddr=0x%x\r\n",path_id,EthsockCliIpcOpen[path_id][id].sharedSendMemAddr,EthsockCliIpcOpen[path_id][id].sharedRecvMemAddr);
		EthCamSocketCli_Open(path_id,  id, &EthsockCliIpcOpen[path_id][id]);
	}else if(id== ETHSOCKIPCCLI_ID_1){
#if (ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
		//CHKPNT;
		if(socketCliEthData2_ConfigRecvBuf(path_id)==-1){
			return;
		}
	    	EthCamSocketCli_SetDataRecvCB(path_id, ETHSOCKIPCCLI_ID_1, (UINT32)&socketCliEthData2_RecvCB);
		EthCamSocketCli_SetDataNotifyCB(path_id, ETHSOCKIPCCLI_ID_1, (UINT32)&socketCliEthData2_NotifyCB);
		socketCliEthData2_RecvResetParam(path_id);
		// open
		EthsockCliIpcOpen[path_id][id].sharedSendMemAddr=OS_GetMempoolAddr(POOL_ID_ETHSOCKCLI_IPC)+(POOL_CNT_ETHSOCKCLI_IPC/ETH_REARCAM_CAPS_COUNT)*path_id*POOL_SIZE_ETHSOCKCLI_IPC +POOL_SIZE_ETHSOCKCLI_IPC;
		EthsockCliIpcOpen[path_id][id].sharedSendMemSize=POOL_SIZE_ETHSOCKCLI_IPC;
		if(sEthCamTxDecInfo[path_id].Tbr !=0 && sEthCamTxDecInfo[path_id].bStarupOK==1){
			//CHKPNT;
			g_SocketCliData2_RecvAddr[path_id]=socketCliEthData2_GetRecvBufAddr(path_id, (sEthCamTxDecInfo[path_id].Tbr*100/TBR_SIZE_RATIO));
			EthsockCliIpcOpen[path_id][id].sharedRecvMemSize=(sEthCamTxDecInfo[path_id].Tbr*100/TBR_SIZE_RATIO);
		}else{
			//CHKPNT;
			g_SocketCliData2_RecvAddr[path_id]=socketCliEthData2_GetRecvBufAddr(path_id, MAX_I_FRAME_SZIE);
			EthsockCliIpcOpen[path_id][id].sharedRecvMemSize=MAX_I_FRAME_SZIE;
		}
		//DBG_DUMP("sEthCamTxDecInfo.Tbr=%d, sharedRecvMemSize%d\r\n", sEthCamTxDecInfo.Tbr,EthsockCliIpcOpen[id].sharedRecvMemSize );
		EthsockCliIpcOpen[path_id][id].sharedRecvMemAddr=g_SocketCliData2_RecvAddr[path_id];
		DBG_IND("Cli[%d] D2 SendMemAddr=0x%x, RecvMemAddr=0x%x\r\n",path_id,EthsockCliIpcOpen[path_id][id].sharedSendMemAddr,EthsockCliIpcOpen[path_id][id].sharedRecvMemAddr);
		EthCamSocketCli_Open(path_id, id, &EthsockCliIpcOpen[path_id][id]);
#endif
	}
}
void socketCliEthCmd_Open(ETHCAM_PATH_ID path_id)
{
	ETHSOCKCLIIPC_OPEN EthsockCliIpcOpen[ETHCAM_PATH_ID_MAX] = {0};

	EthCamSocketCli_SetCmdRecvCB(path_id, (UINT32)&socketCliEthCmd_RecvCB);
	//CHKPNT;
	EthCamSocketCli_SetCmdNotifyCB(path_id, (UINT32)&socketCliEthCmd_NotifyCB);
	EthsockCliIpcOpen[path_id].sharedSendMemAddr=OS_GetMempoolAddr(POOL_ID_ETHSOCKCLI_IPC)+(POOL_CNT_ETHSOCKCLI_IPC/ETH_REARCAM_CAPS_COUNT)*path_id*POOL_SIZE_ETHSOCKCLI_IPC +2*POOL_SIZE_ETHSOCKCLI_IPC;
	EthsockCliIpcOpen[path_id].sharedSendMemSize=POOL_SIZE_ETHSOCKCLI_IPC;

	EthsockCliIpcOpen[path_id].sharedRecvMemAddr=OS_GetMempoolAddr(POOL_ID_ETHSOCKCLI_IPC)+(POOL_CNT_ETHSOCKCLI_IPC/ETH_REARCAM_CAPS_COUNT)*path_id*POOL_SIZE_ETHSOCKCLI_IPC +3*POOL_SIZE_ETHSOCKCLI_IPC;
	EthsockCliIpcOpen[path_id].sharedRecvMemSize=POOL_SIZE_ETHSOCKCLI_IPC;
	DBG_IND("Cli[%d] Cmd SendMemAddr=0x%x, RecvMemAddr=0x%x\r\n",path_id,EthsockCliIpcOpen[path_id].sharedSendMemAddr,EthsockCliIpcOpen[path_id].sharedRecvMemAddr);
	EthCamSocketCli_ReConnect(path_id, ETHSOCKIPCCLI_ID_2, 0);
	EthCamSocketCli_Open(path_id, ETHSOCKIPCCLI_ID_2, &EthsockCliIpcOpen[path_id]);
}

static ETHCAM_BSDATA     g_ethcam_data1_bsdata[2] = {0};
static ISF_RV _ISF_EthCamData1_LockCB(UINT64 module, UINT32 hData)
{
	UINT32 i = 0;
	ETHCAM_BSDATA *p_ethcam_bsdata = NULL;

	// search BSDMX_BSDATA link list by hData
	for (i = 0; i < 2; i++) {
		p_ethcam_bsdata = &g_ethcam_data1_bsdata[i];

		if (p_ethcam_bsdata != NULL) {
			p_ethcam_bsdata->refCnt++;
			//DBG_DUMP("ph refCnt++, %s %d\r\n", (CHAR *)&module,p_photo_bsdata->refCnt);
		}
	}
	return ISF_OK;
}

static ISF_RV _ISF_EthCamData1_UnLockCB(UINT64 module, UINT32 hData)
{
	UINT32 i = 0;
	ETHCAM_BSDATA *p_ethcam_bsdata = NULL;

	// search BSDMX_BSDATA link list by hData
	for (i = 0; i < 2; i++) {
		p_ethcam_bsdata = &g_ethcam_data1_bsdata[i];

		if (p_ethcam_bsdata != NULL) {
			p_ethcam_bsdata->refCnt--;
			//DBG_DUMP("ph refCnt--, %s %d\r\n", (CHAR *)&module, p_photo_bsdata->refCnt);
		}
	}
	return ISF_OK;
}

ISF_DATA gISF_EthCam_Pull_InData1 = {
	.Sign      = ISF_SIGN_DATA,                 ///< signature, equal to ISF_SIGN_DATA or ISF_SIGN_EVENT
	.hData     = 0,                             ///< handle of real data, it will be "nvtmpp blk_id", or "custom data handle"
	.pLockCB   = _ISF_EthCamData1_LockCB,      ///< CB to lock "custom data handle"
	.pUnlockCB = _ISF_EthCamData1_UnLockCB,    ///< CB to unlock "custom data handle"
	.Event     = 0,                             ///< default 0
	.Serial    = 0,                             ///< serial id
	.TimeStamp = 0,                             ///< time-stamp
};
static ISF_DATA   g_SocketCliData1_InIsfData[ETH_REARCAM_CAPS_COUNT];

static ETHCAM_BSDATA     g_ethcam_data1_audcap_data[2] = {0};
static ISF_RV _ISF_EthCamData1_AudCap_LockCB(UINT64 module, UINT32 hData)
{
	UINT32 i = 0;
	ETHCAM_BSDATA *p_ethcam_bsdata = NULL;

	// search BSDMX_BSDATA link list by hData
	for (i = 0; i < 2; i++) {
		p_ethcam_bsdata = &g_ethcam_data1_audcap_data[i];

		if (p_ethcam_bsdata != NULL) {
			p_ethcam_bsdata->refCnt++;
			//DBG_DUMP("ph refCnt++, %s %d\r\n", (CHAR *)&module,p_photo_bsdata->refCnt);
		}
	}
	return ISF_OK;
}

static ISF_RV _ISF_EthCamData1_AudCap_UnLockCB(UINT64 module, UINT32 hData)
{
	UINT32 i = 0;
	ETHCAM_BSDATA *p_ethcam_bsdata = NULL;

	// search BSDMX_BSDATA link list by hData
	for (i = 0; i < 2; i++) {
		p_ethcam_bsdata = &g_ethcam_data1_audcap_data[i];

		if (p_ethcam_bsdata != NULL) {
			p_ethcam_bsdata->refCnt--;
			//DBG_DUMP("ph refCnt--, %s %d\r\n", (CHAR *)&module, p_photo_bsdata->refCnt);
		}
	}
	return ISF_OK;
}

ISF_DATA gISF_EthCam_Pull_AudCapInData1 = {
	.Sign      = ISF_SIGN_DATA,                 ///< signature, equal to ISF_SIGN_DATA or ISF_SIGN_EVENT
	.hData     = 0,                             ///< handle of real data, it will be "nvtmpp blk_id", or "custom data handle"
	.pLockCB   = _ISF_EthCamData1_AudCap_LockCB,      ///< CB to lock "custom data handle"
	.pUnlockCB = _ISF_EthCamData1_AudCap_UnLockCB,    ///< CB to unlock "custom data handle"
	.Event     = 0,                             ///< default 0
	.Serial    = 0,                             ///< serial id
	.TimeStamp = 0,                             ///< time-stamp
};
static ISF_DATA   g_SocketCliData1_AudCapInIsfData[ETH_REARCAM_CAPS_COUNT];
void socketCliEthData1_RecvCB(ETHCAM_PATH_ID path_id, char* addr, int size)
{
	//static UINT32 eth_i_cnt = 0;
	UINT32 eth_is_i_frame = 0;
	UINT16 bPushData=0;
	UINT32 DescSize=0;
	char* addrAudCap=0;
	UINT32 AudCapEn=0;
	UINT32 AudCapTotalSize=0;
	UINT32 AudCapCopyCount=0;
	UINT16 i;

	if(sEthCamTxRecInfo[path_id].codec == MEDIAVIDENC_H264){
		DescSize=sEthCamTxRecInfo[path_id].DescSize;
	}

	//static BOOL bFirstErr=0;
	//DBG_DUMP("D1 size=%d, 0x%x\r\n",  size, addr);
	//if(addr % 4){
	//	DBG_DUMP("addr%4!=0\r\n");
	//}
	if(addr[0] ==0 && addr[1] ==0 && addr[2] ==0 && addr[3] ==1){
		if(((addr[4])& 0x1F) == H264_NALU_TYPE_IDR  && addr[5] == H264_START_CODE_I){
			//DBG_DUMP("D1_I[%d] OK\r\n",path_id);
			//DBG_DUMP("D1_I[%d]=%d,%d\r\n",path_id,g_SocketCliData1_BsFrameCnt[path_id],size);
			g_SocketCliData1_BsFrameCnt[path_id]++;
			eth_is_i_frame = 1;
			g_SocketCliData1_eth_i_cnt[path_id] ++;
			bPushData=1;
		}else if(((addr[4])& 0x1F) == H264_NALU_TYPE_SLICE && addr[5] == H264_START_CODE_P){
			//DBG_DUMP("D1_P[%d] OK\r\n",path_id);
			//DBG_DUMP("D1_P[%d]=%d,%d\r\n",path_id,g_SocketCliData1_BsFrameCnt[path_id],size);
			g_SocketCliData1_BsFrameCnt[path_id]++;
			eth_is_i_frame = 0;
			bPushData=1;
		}else if((((addr[4])>>1)&0x3F) == H265_NALU_TYPE_VPS){
			//DBG_DUMP("D1_I[%d] OK\r\n",path_id);
			//DBG_DUMP("D1_I[%d]=%d,%d\r\n",path_id,g_SocketCliData1_BsFrameCnt[path_id],size);
			g_SocketCliData1_BsFrameCnt[path_id]++;
			eth_is_i_frame = 1;
			g_SocketCliData1_eth_i_cnt[path_id] ++;
			bPushData=1;
		}else if((((addr[4])>>1)&0x3F) == H265_NALU_TYPE_SLICE){
			//DBG_DUMP("D1_P[%d] OK\r\n",path_id);
			//DBG_DUMP("D1_P[%d]=%d,%d\r\n",path_id,g_SocketCliData1_BsFrameCnt[path_id],size);
			g_SocketCliData1_BsFrameCnt[path_id]++;
			eth_is_i_frame = 0;
			bPushData=1;

		}else if(((addr[4])==HEAD_TYPE_THUMB)){
			DBG_DUMP("D1_RecvCB Thumb OK, FRcnt[%d]=%d, sz=%d\r\n",path_id,g_SocketCliData1_BsFrameCnt[path_id],size);
			//g_testData1_RawEncodeAddr=(UINT32)addr;
			//g_testData1_RawEncodeSize=(UINT32)size;
			//set_flg(ETHCAM_CMD_RCV_FLG_ID, FLG_ETHCAM_CMD_RCV);
			bPushData = 2;
			//DBG_DUMP("data[4]=0x%x, %x, %x, %x, %x, %x, %x, %x\r\n",addr[4],addr[5],addr[6],addr[7],addr[8],addr[size-3],addr[size-2],addr[size-1]);
		}else if(((addr[4])==HEAD_TYPE_RAW_ENCODE)){
			DBG_DUMP("D1_RecvCB PIM OK, FRcnt[%d]=%d\r\n",path_id,g_SocketCliData1_BsFrameCnt[path_id]);
			bPushData = 3;
			//DBG_DUMP("data[4]=0x%x, %x, %x, %x, %x, %x, %x, %x\r\n",addr[4],addr[5],addr[6],addr[7],addr[8],addr[size-3],addr[size-2],addr[size-1]);
			BKG_PostEvent(NVTEVT_BKW_ETHCAM_RAW_ENCODE_RESULT);
		}else{
			//DBG_DUMP("Check FAIL\r\n");
		}
	}else if(addr[0+DescSize] ==0 && addr[1+DescSize] ==0 && addr[2+DescSize] ==0 && addr[3+DescSize] ==1){
		if(((addr[4+DescSize])& 0x1F) == H264_NALU_TYPE_IDR && addr[5+DescSize] == H264_START_CODE_I){
			g_SocketCliData1_BsFrameCnt[path_id]++;
			eth_is_i_frame = 1;
			g_SocketCliData1_eth_i_cnt[path_id] ++;
			bPushData=1;
		}
	}else{
#if 0//debug, write to card
		bFirstErr=1;
		g_testData1_Addr=(UINT32)addr;
		g_testData1_Size=(UINT32)size;
		//set_flg(ETHCAM_CMD_RCV_FLG_ID, FLG_ETHCAM_CMD_RCV);
				char path[30];
				sprintf(path, "A:\\RxData.bin");
				//DBG_DUMP("Error! RxData: %s\r\n", path);
			//DBG_DUMP("Err [0]=0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\r\n",addr[0],addr[1],addr[2],addr[3],addr[4],addr[5],addr[6]);

				static  FST_FILE fhdl = 0;
				UINT32      fileSize = 0;
				static UINT32 uiRecvSize=0;
				UINT32      TotalfileSize = 1*1024*1024;
				if(fhdl==0 && (uiRecvSize < TotalfileSize)){
					fhdl = FileSys_OpenFile(path, FST_CREATE_ALWAYS | FST_OPEN_WRITE);
					FileSys_SeekFile(fhdl, 0, FST_SEEK_SET);
				}else if(fhdl && (uiRecvSize < TotalfileSize)){
					FileSys_SeekFile(fhdl, 0, FST_SEEK_END);
				}
				fileSize = g_testData1_Size;
				if(fhdl && (uiRecvSize < TotalfileSize)){
					FileSys_WriteFile(fhdl, (UINT8 *)g_testData1_Addr, &fileSize, 0, NULL);
					uiRecvSize+=g_testData1_Size;
				}
				if(fhdl && (uiRecvSize>=TotalfileSize)){
					DBG_DUMP("Write Finish!\r\n");
					FileSys_CloseFile(fhdl);
					fhdl = 0;
				}
//#else
				static UINT32 uiRecvSize=0;
				UINT32      TotalfileSize = 8*1024*1024;

				hwmem_open();
				hwmem_memcpy((UINT32)(g_Test_Addr+uiRecvSize), (UINT32)addr, (UINT32)size);
				hwmem_close();
				if((uiRecvSize + size) < TotalfileSize){
					uiRecvSize+=size;
				}else{
					FST_FILE fhdl = 0;
					char path[30];
					sprintf(path, "A:\\RxData.bin");
					fhdl = FileSys_OpenFile(path, FST_CREATE_ALWAYS | FST_OPEN_WRITE);
					FileSys_SeekFile(fhdl, 0, FST_SEEK_SET);
					FileSys_WriteFile(fhdl, (UINT8 *)g_Test_Addr, &uiRecvSize, 0, NULL);
					FileSys_CloseFile(fhdl);
					DBG_DUMP("Write Finish!\r\n");
				}
#endif
	}
	if(bPushData==1 ){
		//push data to VdoDec
	#if 1
		//extern UINT8 SPS_Addr[];

		ISF_PORT         *pDist;
		ISF_VIDEO_STREAM_BUF    *pVidStrmBuf;

		UINT32 LongCounterSizeOffset=LONGCNT_STAMP_OFFSET;
		UINT32 StreamSize = size-LONGCNT_STAMP_OFFSET;
		UINT32 LongConterLow[ETHCAM_PATH_ID_MAX]={0};
		UINT32 LongConterHigh[ETHCAM_PATH_ID_MAX]={0};
		UINT64 LongConter[ETHCAM_PATH_ID_MAX]={0};
		static UINT32 LongCntTxRxOffset[ETHCAM_PATH_ID_MAX]={0};
		//DBG_DUMP("D1longcnt=0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\r\n",addr[StreamSize+0],addr[StreamSize+1],addr[StreamSize+2],addr[StreamSize+3],addr[StreamSize+4],addr[StreamSize+5],addr[StreamSize+6],addr[StreamSize+7],addr[StreamSize+8],addr[StreamSize+9],addr[StreamSize+10],addr[StreamSize+11],addr[StreamSize+12]);
		if(addr[StreamSize+0] ==0 && addr[StreamSize+1] ==0 && addr[StreamSize+2] ==0 && addr[StreamSize+3] ==1 && addr[StreamSize+4]==HEAD_TYPE_LONGCNT_STAMP){
			LongConterHigh[path_id]=(addr[StreamSize+5]<<24)+ (addr[StreamSize+6]<<16)+ (addr[StreamSize+7]<<8) + (addr[StreamSize+8]);
			LongConterLow[path_id]=(addr[StreamSize+9]<<24)+ (addr[StreamSize+10]<<16)+ (addr[StreamSize+11]<<8) + (addr[StreamSize+12]);
			LongConter[path_id]=(((UINT64)LongConterHigh[path_id]<<32) | LongConterLow[path_id]);
			if(g_SocketCliData1_BsFrameCnt[path_id]> 10 && g_SocketCliData1_BsFrameCnt[path_id] <= 60){
				LongCntTxRxOffset[path_id]+=HwClock_DiffLongCounter(LongConter[path_id], HwClock_GetLongCounter());
				//DBG_DUMP("11D1_LongCntTxRxOffset[%d]=%d\r\n",path_id,LongCntTxRxOffset[path_id]);

				if(g_SocketCliData1_BsFrameCnt[path_id] == 60){
					//DBG_DUMP("22D1_LongCntTxRxOffset[%d]=%d\r\n",path_id,LongCntTxRxOffset[path_id]);
					g_SocketCliData1_LongCntTxRxOffset[path_id]=LongCntTxRxOffset[path_id]/(60-10);
				}
			}else{
				LongCntTxRxOffset[path_id] =0;
			}
			//DBG_DUMP("D1_LongCntHigh[%d]=(%d,%d), %d, (%d,%d), diff=%d, %d\r\n",path_id,LongConterHigh[path_id],LongConterLow[path_id],(UINT32)LongConter[path_id],(UINT32)((HwClock_GetLongCounter() >> 32) & 0xFFFFFFFF),(UINT32)(HwClock_GetLongCounter() & 0xFFFFFFFF), (UINT32)HwClock_DiffLongCounter(LongConter[path_id], HwClock_GetLongCounter())/1000,(UINT32)g_SocketCliData1_LongCntTxRxOffset[path_id]/1000);
		}else{
			LongCounterSizeOffset=0;
		}
		if(size>=(ETHCAM_AUDCAP_FRAME_SIZE+LONGCNT_STAMP_OFFSET)){
			addrAudCap=addr+ size -ETHCAM_AUDCAP_FRAME_SIZE-LONGCNT_STAMP_OFFSET;
			//DBG_DUMP("0x%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x\r\n",addr_tmp[0],addr_tmp[1],addr_tmp[2],addr_tmp[3],addr_tmp[4],addr_tmp[5],addr_tmp[6],addr_tmp[7],addr_tmp[8],addr_tmp[9],addr_tmp[10],addr_tmp[11],addr_tmp[12],addr_tmp[13]);

			if((addrAudCap[0]>0) && ((addrAudCap[1] + (addrAudCap[2]<<8))>0) && addrAudCap[3+0]==0 && addrAudCap[3+1]==0 && addrAudCap[3+2]==0 && addrAudCap[3+3]==1
				&& addrAudCap[3+4]==HEAD_TYPE_AUDCAP && addrAudCap[3+5]==0xff){
				AudCapEn=1;

				AudCapTotalSize=addrAudCap[1] + (addrAudCap[2]<<8);
				AudCapCopyCount=addrAudCap [0];
				//DBG_DUMP("sz=%d, Cnt=%d, 0x%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x\r\n",AudCapTotalSize,AudCapCopyCount,addr_tmp[0],addr_tmp[1],addr_tmp[2],addr_tmp[3],addr_tmp[4],addr_tmp[5],addr_tmp[6],addr_tmp[7],addr_tmp[8],addr_tmp[9],addr_tmp[10],addr_tmp[11],addr_tmp[12],addr_tmp[13]);
				size-=ETHCAM_AUDCAP_FRAME_SIZE;
			}else{
				AudCapEn=0;
			}
		}else{
			AudCapEn=0;
		}
		g_SocketCliData1_AudCapInIsfData[path_id] = gISF_EthCam_Pull_AudCapInData1;

		g_SocketCliData1_InIsfData[path_id] = gISF_EthCam_Pull_InData1;

#if 0
		if(socketCliEthData1_IsRecv(path_id)==0)
		{
			socketCliEthData1_SetRecv(path_id, 1);
#if(ETH_REARCAM_CLONE_FOR_DISPLAY == DISABLE)
			#if(ETH_REARCAM_CAPS_COUNT >=2)
			UI_SetData(FL_DUAL_CAM, DUALCAM_BOTH);
			#else
			UI_SetData(FL_DUAL_CAM, DUALCAM_BEHIND);
			#endif
			MovieExe_UserProc_ChgCB(UI_GetData(FL_DUAL_CAM));
#endif
		}
#endif

		pVidStrmBuf = (ISF_VIDEO_STREAM_BUF *)&g_SocketCliData1_InIsfData[path_id].Desc;
		pVidStrmBuf->flag     = MAKEFOURCC('V', 'S', 'T', 'M');
		pVidStrmBuf->DataAddr = (UINT32)addr;//uiBsFrmBufAddr;//g_EthBsFrmAddr[g_queue_idx-1];//pVdoBs->BsAddr;
		pVidStrmBuf->DataSize = (UINT32)(size-LongCounterSizeOffset);//g_SocketCliData1_BsBufMapTbl[g_SocketCliData1_BsQueueIdx];//g_CliSocket_size_frm[g_queue_idx-1];//pVdoBs->BsSize;
		//pVidStrmBuf->CodecType = MEDIAVIDENC_H264;                                          //codec type
		pVidStrmBuf->CodecType = sEthCamTxRecInfo[path_id].codec;                                          //codec type
		//pVidStrmBuf->Resv[0]  = (UINT32)&(SPS_Addr[0]);                                     //sps addr
		if(sEthCamTxRecInfo[path_id].codec == MEDIAVIDENC_H264){
		pVidStrmBuf->Resv[0]  = (UINT32)&(sEthCamTxRecInfo[path_id].Desc[0]);         //sps addr
		//pVidStrmBuf->Resv[1]  = 28;                                                         //sps size
		pVidStrmBuf->Resv[1]  = sEthCamTxRecInfo[path_id].SPSSize;                         //sps size
		//pVidStrmBuf->Resv[2]  = (UINT32)&(SPS_Addr[28]);                                    //pps addr
		pVidStrmBuf->Resv[2]  = (UINT32)&(sEthCamTxRecInfo[path_id].Desc[sEthCamTxRecInfo[path_id].SPSSize]);                                    //pps addr
		//pVidStrmBuf->Resv[3]  = 8;
		pVidStrmBuf->Resv[3]  = sEthCamTxRecInfo[path_id].PPSSize;                                                          //pps size
		pVidStrmBuf->Resv[4]  = 0;                                                          //vps addr
		pVidStrmBuf->Resv[5]  = 0;                                                          //vps size
		}else{
			pVidStrmBuf->Resv[0]  = (UINT32)&(sEthCamTxRecInfo[path_id].Desc[sEthCamTxRecInfo[path_id].VPSSize]);         //sps addr
			//pVidStrmBuf->Resv[1]  = 28;                                                         //sps size
			pVidStrmBuf->Resv[1]  = sEthCamTxRecInfo[path_id].SPSSize;                         //sps size
			//pVidStrmBuf->Resv[2]  = (UINT32)&(SPS_Addr[28]);                                    //pps addr
			pVidStrmBuf->Resv[2]  = (UINT32)&(sEthCamTxRecInfo[path_id].Desc[sEthCamTxRecInfo[path_id].VPSSize+sEthCamTxRecInfo[path_id].SPSSize]);  //pps addr
			//pVidStrmBuf->Resv[3]  = 8;
			pVidStrmBuf->Resv[3]  = sEthCamTxRecInfo[path_id].PPSSize;                                                          //pps size
			pVidStrmBuf->Resv[4]  = (UINT32)&(sEthCamTxRecInfo[path_id].Desc[0]);                                         //vps addr
			pVidStrmBuf->Resv[5]  = sEthCamTxRecInfo[path_id].VPSSize;                                                          //vps size
		}

		UINT32 i_per_sec = sEthCamTxRecInfo[path_id].vfr / sEthCamTxRecInfo[path_id].gop;
		if (!i_per_sec) {
			i_per_sec = 1;
		}
		pVidStrmBuf->Resv[6]  = (eth_is_i_frame) ? 3 : 0;                                   //FrameType (IDR = 3, P = 0)
		pVidStrmBuf->Resv[7]  = (((g_SocketCliData1_eth_i_cnt[path_id] - 1) % i_per_sec) == 0 && eth_is_i_frame) ? TRUE : FALSE;    //IsIDR2Cut
		pVidStrmBuf->Resv[8]  = 0;                                                          //SVC size
		pVidStrmBuf->Resv[9]  = 0;                                                          //Temporal Id
		pVidStrmBuf->Resv[10] = (eth_is_i_frame) ? TRUE : FALSE;                            //IsKey
		pVidStrmBuf->Resv[12] = g_SocketCliData1_BsFrameCnt[path_id]-1;//pVdoBs->FrmIdx;
		pVidStrmBuf->Resv[13] = 0;//pVdoBs->bIsEOF;  //have next I Frame ?
		g_SocketCliData1_InIsfData[path_id].TimeStamp = HwClock_GetLongCounter();

		#if (ETH_REARCAM_CLONE_FOR_DISPLAY == DISABLE)
		pDist = ImageUnit_In(&ISF_VdoDec, ImageApp_MovieMulti_GetEthCamVdoDecInPort(_CFG_ETHCAM_ID_1 + path_id));
		if (ImageUnit_IsAllowPush(pDist)) {
			if(sEthCamTxRecInfo[path_id].codec == MEDIAVIDENC_H265 && eth_is_i_frame){
				pVidStrmBuf->DataAddr = (UINT32)(addr+sEthCamTxRecInfo[path_id].VPSSize+sEthCamTxRecInfo[path_id].SPSSize+sEthCamTxRecInfo[path_id].PPSSize);//uiBsFrmBufAddr;//g_EthBsFrmAddr[g_queue_idx-1];//pVdoBs->BsAddr;
				pVidStrmBuf->DataSize = (UINT32)(size-LongCounterSizeOffset-(sEthCamTxRecInfo[path_id].VPSSize+sEthCamTxRecInfo[path_id].SPSSize+sEthCamTxRecInfo[path_id].PPSSize));//g_SocketCliData1_BsBufMapTbl[g_SocketCliData1_BsQueueIdx];//g_CliSocket_size_frm[g_queue_idx-1];//pVdoBs->BsSize;
			}else if(sEthCamTxRecInfo[path_id].codec == MEDIAVIDENC_H264 && eth_is_i_frame){
				pVidStrmBuf->DataAddr = (UINT32)(addr+sEthCamTxRecInfo[path_id].SPSSize+sEthCamTxRecInfo[path_id].PPSSize);//uiBsFrmBufAddr;//g_EthBsFrmAddr[g_queue_idx-1];//pVdoBs->BsAddr;
				pVidStrmBuf->DataSize = (UINT32)(size-LongCounterSizeOffset-(sEthCamTxRecInfo[path_id].SPSSize+sEthCamTxRecInfo[path_id].PPSSize));//g_SocketCliData1_BsBufMapTbl[g_SocketCliData1_BsQueueIdx];//g_CliSocket_size_frm[g_queue_idx-1];//pVdoBs->BsSize;
			}
			ImageUnit_PushData(pDist, &(g_SocketCliData1_InIsfData[path_id]), 0);
		}
		#endif
		pDist = ImageUnit_In(&ISF_Demux, ImageApp_MovieMulti_GetEthCamDemuxInPort(_CFG_ETHCAM_ID_1 + path_id));
		if (ImageUnit_IsAllowPush(pDist)) {
			if(sEthCamTxRecInfo[path_id].codec == MEDIAVIDENC_H265 && eth_is_i_frame){
				pVidStrmBuf->DataAddr = (UINT32)addr;
				pVidStrmBuf->DataSize = (UINT32)(size-LongCounterSizeOffset);
			}else if(sEthCamTxRecInfo[path_id].codec == MEDIAVIDENC_H264 && eth_is_i_frame){

				if(sEthCamTxRecInfo[path_id].rec_format == _CFG_FILE_FORMAT_TS){
					memcpy(addr, sEthCamTxRecInfo[path_id].Desc, sEthCamTxRecInfo[path_id].DescSize);
					pVidStrmBuf->DataAddr = (UINT32)addr;
					pVidStrmBuf->DataSize = (UINT32)(size-LongCounterSizeOffset);
				}else{
					pVidStrmBuf->DataAddr = (UINT32)(addr+sEthCamTxRecInfo[path_id].SPSSize+sEthCamTxRecInfo[path_id].PPSSize);
					pVidStrmBuf->DataSize = (UINT32)(size-LongCounterSizeOffset-(sEthCamTxRecInfo[path_id].SPSSize+sEthCamTxRecInfo[path_id].PPSSize));
				}
			}
			ImageUnit_PushData(pDist, &(g_SocketCliData1_InIsfData[path_id]), 0);
		}
		#if 1
		if(AudCapEn){
			ISF_AUDIO_STREAM_BUF *pAudBuf = (ISF_AUDIO_STREAM_BUF *)&g_SocketCliData1_AudCapInIsfData[path_id].Desc;
			pAudBuf->CodecType=AUDDEC_DECODER_AAC;
			pAudBuf->flag     = MAKEFOURCC('A', 'S', 'T', 'M');
			pAudBuf->DataAddr = (UINT32)(addrAudCap+ETHCAM_AUDCAP_HEADER_OFFSET);
			pAudBuf->DataSize = (UINT32)(AudCapTotalSize-ETHCAM_AUDCAP_HEADER_OFFSET);
			g_SocketCliData1_AudCapInIsfData[path_id].TimeStamp = HwClock_GetLongCounter();
			for(i=0;i<2;i++){
				pDist = ImageUnit_In(&ISF_BsMux, (ImageApp_MovieMulti_GetEthCamAudBsmuxInPort(_CFG_ETHCAM_ID_1 + path_id)+i));
				if (ImageUnit_IsAllowPush(pDist)) {
					ImageUnit_PushData(pDist, &(g_SocketCliData1_AudCapInIsfData[path_id]), 0);
				}
			}
		}
		#endif
	#endif

		if(socketCliEthData1_IsRecv(path_id)==0)
		{
			socketCliEthData1_SetRecv(path_id, 1);
#if(ETH_REARCAM_CLONE_FOR_DISPLAY == DISABLE)
			#if(ETH_REARCAM_CAPS_COUNT >=2)
			UI_SetData(FL_DUAL_CAM, DUALCAM_BOTH);
			#else
			UI_SetData(FL_DUAL_CAM, DUALCAM_BEHIND);
			#endif
			MovieExe_EthCam_ChgDispCB(UI_GetData(FL_DUAL_CAM));
#endif
		}
		EthCamHB1[path_id] = 0;
	} else if (bPushData == 2 || bPushData == 3) {
	#if 1
		ISF_PORT *pDist;
		ISF_VIDEO_STREAM_BUF *pVidStrmBuf;
		g_SocketCliData1_InIsfData[path_id] = gISF_EthCam_Pull_InData1;

		pVidStrmBuf = (ISF_VIDEO_STREAM_BUF *)&g_SocketCliData1_InIsfData[path_id].Desc;
		if (bPushData == 2) {
			pVidStrmBuf->flag = MAKEFOURCC('T', 'H', 'U', 'M');
		} else {
			pVidStrmBuf->flag = MAKEFOURCC('J', 'S', 'T', 'M');
		}
		pVidStrmBuf->DataAddr = (UINT32)&addr[5];
		pVidStrmBuf->DataSize = (UINT32)(size-5);
		if (bPushData == 2) {
			pVidStrmBuf->Resv[0]  = (path_id==0)?AppBKW_GetData(BKW_ETHCAM_TRIGGER_THUMB_PATHID_P0):AppBKW_GetData(BKW_ETHCAM_TRIGGER_THUMB_PATHID_P1);//ImageApp_MovieMulti_Recid2BsPort(_CFG_ETHCAM_ID_1 + path_id);                //user_id
		}else{
			pVidStrmBuf->Resv[0]  =ImageApp_MovieMulti_Recid2BsPort(_CFG_ETHCAM_ID_1 + path_id);
		}
		//DBG_DUMP("JPG push port=%d\r\n",pVidStrmBuf->Resv[0]);
		(path_id==0)?AppBKW_SetData(BKW_ETHCAM_TRIGGER_THUMB_PATHID_P0, 0):AppBKW_SetData(BKW_ETHCAM_TRIGGER_THUMB_PATHID_P1, 0);
		g_SocketCliData1_InIsfData[path_id].TimeStamp = HwClock_GetLongCounter();

		#if 0//(ETH_REARCAM_CLONE_FOR_DISPLAY == DISABLE)
		pDist = ImageUnit_In(&ISF_VdoDec, ImageApp_MovieMulti_GetEthCamVdoDecInPort(_CFG_ETHCAM_ID_1 + path_id));
		if (ImageUnit_IsAllowPush(pDist)) {
			//DBG_DUMP("Push\r\n");
			ImageUnit_PushData(pDist, &(g_SocketCliData1_InIsfData[path_id]), 0);
		}
		#endif
		pDist = ImageUnit_In(&ISF_Demux, ImageApp_MovieMulti_GetEthCamDemuxInPort(_CFG_ETHCAM_ID_1 + path_id));
		if (ImageUnit_IsAllowPush(pDist)) {
			//DBG_DUMP("Push thumb\r\n");
			ImageUnit_PushData(pDist, &(g_SocketCliData1_InIsfData[path_id]), 0);
		}
	#endif
	}
}
BOOL bflag_EthLinkFinish = FALSE;
void socketCliEthData1_NotifyCB(ETHCAM_PATH_ID path_id, int status, int parm)
{
	switch (status) {
	case CYG_ETHSOCKETCLI_STATUS_CLIENT_REQUEST: {
			//DBG_DUMP("Notify EthCli REQUEST\r\n");
			break;
		}
	case CYG_ETHSOCKETCLI_STATUS_CLIENT_CONNECT: {
			DBG_DUMP("[%d]socketCliEthData1_NotifyCB connect OK\r\n",path_id);
			g_IsSocketCliData1_Conn[path_id]=1;
#if((ETH_REARCAM_CLONE_FOR_DISPLAY == DISABLE) && (ETH_REARCAM_CAPS_COUNT==1))
			ImageApp_MovieMulti_EthCamLinkForDisp(_CFG_ETHCAM_ID_1+path_id, ENABLE, TRUE);
			EthCam_SendXMLCmd(path_id, ETHCAM_PORT_DATA1 ,ETHCAM_CMD_TX_STREAM_START, 0);
#endif

			if (System_IsModeChgClose()==0 && System_GetState(SYS_STATE_CURRMODE) == PRIMARY_MODE_MOVIE) {
			//if (System_GetState(SYS_STATE_CURRMODE) == PRIMARY_MODE_MOVIE) {
				Ux_PostEvent(NVTEVT_EXE_MOVIE_ETHCAMHOTPLUG, 1, path_id);
			}
			//CHKPNT;
            bflag_EthLinkFinish = TRUE;
			DBGD(bflag_EthLinkFinish);
            //Ux_SendEvent(0, NVTEVT_SYSTEM_MODE, 1, System_GetState(SYS_STATE_CURRMODE));
			break;
		}
	case CYG_ETHSOCKETCLI_STATUS_CLIENT_DISCONNECT: {
			DBG_ERR("[%d]disconnect!!!\r\n",path_id);
			g_IsSocketCliData1_Conn[path_id]=0;
            bflag_EthLinkFinish = FALSE;
			break;
		}
	case CYG_ETHSOCKETCLI_STATUS_CLIENT_SLOW: {
			DBG_ERR("[%d]Error!!!parm=%d\r\n",path_id,parm);
			if(ImageApp_MovieMulti_IsStreamRunning(_CFG_ETHCAM_ID_1)||
				ImageApp_MovieMulti_IsStreamRunning(_CFG_ETHCAM_ID_2)){
				DBG_ERR("[%d]STOPREC!!!parm=%d\r\n",path_id,parm);
				//Ux_PostEvent(NVTEVT_CB_MOVIE_SLOW, 0);
				BKG_PostEvent(NVTEVT_BKW_STOPREC_PROCESS);
			}
			if(path_id>=ETHCAM_PATH_ID_1 && path_id< ETHCAM_PATH_ID_MAX){
				AppBKW_SetData(BKW_ETHCAM_DEC_ERR_PATHID, path_id);
				BKG_PostEvent(NVTEVT_BKW_ETHCAM_DEC_ERR);

				EthCam_SendXMLCmd(path_id, ETHCAM_PORT_DEFAULT ,ETHCAM_CMD_DUMP_TX_BS_INFO, 0);
			}
			break;
		}
	}
}
void socketCliEthData1_RecvResetParam(ETHCAM_PATH_ID path_id)
{
	g_SocketCliData1_BsFrameCnt[path_id]=0;
	g_SocketCliData1_RecBsFrameCnt[path_id]=0;
	g_SocketCliData1_eth_i_cnt[path_id]=0;
	g_SocketCliData1_LongCntTxRxOffset[path_id]=0;
}


#if (ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
BOOL g_bsocketCliEthData2_AllowPush[ETHCAM_PATH_ID_MAX]={1, 1};
BOOL g_bsocketCliEthData2_ResetI[ETHCAM_PATH_ID_MAX]={0};
void  socketCliEthData2_SetResetI(ETHCAM_PATH_ID path_id, UINT32 Data)
{
	g_bsocketCliEthData2_ResetI[path_id]=Data;
}
void  socketCliEthData2_SetAllowPush(ETHCAM_PATH_ID path_id, UINT32 Data)
{
	g_bsocketCliEthData2_AllowPush[path_id]=Data;
}

static ETHCAM_BSDATA     g_ethcam_data2_bsdata[2] = {0};
static ISF_RV _ISF_EthCamData2_LockCB(UINT64 module, UINT32 hData)
{
	UINT32 i = 0;
	ETHCAM_BSDATA *p_ethcam_bsdata = NULL;

	// search BSDMX_BSDATA link list by hData
	for (i = 0; i < 2; i++) {
		p_ethcam_bsdata = &g_ethcam_data2_bsdata[i];

		if (p_ethcam_bsdata != NULL) {
			p_ethcam_bsdata->refCnt++;
			//DBG_DUMP("ph refCnt++, %s %d\r\n", (CHAR *)&module,p_photo_bsdata->refCnt);
		}
	}
	return ISF_OK;
}

static ISF_RV _ISF_EthCamData2_UnLockCB(UINT64 module, UINT32 hData)
{
	UINT32 i = 0;
	ETHCAM_BSDATA *p_ethcam_bsdata = NULL;

	// search BSDMX_BSDATA link list by hData
	for (i = 0; i < 2; i++) {
		p_ethcam_bsdata = &g_ethcam_data2_bsdata[i];

		if (p_ethcam_bsdata != NULL) {
			p_ethcam_bsdata->refCnt--;
			//DBG_DUMP("ph refCnt--, %s %d\r\n", (CHAR *)&module, p_photo_bsdata->refCnt);
		}
	}
	return ISF_OK;
}

ISF_DATA gISF_EthCam_Pull_InData2 = {
	.Sign      = ISF_SIGN_DATA,                 ///< signature, equal to ISF_SIGN_DATA or ISF_SIGN_EVENT
	.hData     = 0,                             ///< handle of real data, it will be "nvtmpp blk_id", or "custom data handle"
	.pLockCB   = _ISF_EthCamData2_LockCB,      ///< CB to lock "custom data handle"
	.pUnlockCB = _ISF_EthCamData2_UnLockCB,    ///< CB to unlock "custom data handle"
	.Event     = 0,                             ///< default 0
	.Serial    = 0,                             ///< serial id
	.TimeStamp = 0,                             ///< time-stamp
};
static ISF_DATA   g_SocketCliData2_InIsfData;
#endif
void socketCliEthData2_RecvCB(ETHCAM_PATH_ID path_id, char* addr, int size)
{
#if (ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
	//static UINT32 eth_i_cnt = 0;
	UINT32 eth_is_i_frame = 0;
	UINT16 bPushData=0;
	UINT32 DescSize=0;
	if(sEthCamTxDecInfo[path_id].Codec == MEDIAVIDENC_H264){
		DescSize=sEthCamTxDecInfo[path_id].DescSize;
	}
	//extern UINT8 SPS_Addr[];

	//DBG_DUMP("D2 size=%d, 0x%x\r\n",  size, addr);
	//if(addr % 4){
	//	DBG_DUMP("addr%4!=0\r\n");
	//}
	if((addr[0] ==0 && addr[1] ==0 && addr[2] ==0 && addr[3] ==1)){
		if(((addr[4])& 0x1F) == H264_NALU_TYPE_IDR  && addr[5] == H264_START_CODE_I){
			//DBG_DUMP("D2_I[%d]=%d,%d\r\n",path_id,g_SocketCliData2_BsFrameCnt[path_id],size);
			if(g_bsocketCliEthData2_AllowPush[path_id]==1){
				g_bsocketCliEthData2_ResetI[path_id]=0;
			}
			g_SocketCliData2_BsFrameCnt[path_id]++;
			eth_is_i_frame = 1;
			g_SocketCliData2_eth_i_cnt[path_id] ++;
			bPushData=1;
			//g_TestH264Addr=(UINT32)addr;
			//g_TestH264Size=(UINT32)size;
			//EthCamCmdRcv_SetFlag();

		}else if(((addr[4])& 0x1F) == H264_NALU_TYPE_SLICE && addr[5] == H264_START_CODE_P){
			//DBG_DUMP("D2_P[%d]=%d,%d\r\n",path_id,g_SocketCliData2_BsFrameCnt[path_id],size);
			if(g_bsocketCliEthData2_ResetI[path_id]){
				return;
			}
			g_SocketCliData2_BsFrameCnt[path_id]++;
			eth_is_i_frame = 0;
			bPushData=1;
		}else if((((addr[4])>>1)&0x3F) == H265_NALU_TYPE_VPS){
			//DBG_DUMP("65D2_I[%d]=%d,%d\r\n",path_id,g_SocketCliData2_BsFrameCnt[path_id],size);
			if(g_bsocketCliEthData2_AllowPush[path_id]==1){
				g_bsocketCliEthData2_ResetI[path_id]=0;
			}
			g_SocketCliData2_BsFrameCnt[path_id]++;
			eth_is_i_frame = 1;
			g_SocketCliData2_eth_i_cnt[path_id] ++;
			bPushData=1;
			//g_TestH264Addr=(UINT32)addr;
			//g_TestH264Size=(UINT32)size;
			//EthCamCmdRcv_SetFlag();

		}else if((((addr[4])>>1)&0x3F) == H265_NALU_TYPE_SLICE){
			//DBG_DUMP("65D2_P[%d]=%d,%d\r\n",path_id,g_SocketCliData2_BsFrameCnt[path_id],size);
			if(g_bsocketCliEthData2_ResetI[path_id]){
				return;
			}
			g_SocketCliData2_BsFrameCnt[path_id]++;
			eth_is_i_frame = 0;
			bPushData=1;

		}else if(((addr[4])==0xFF)){
			//DBG_DUMP("Thumb OK\r\n");
			bPushData = 2;
			//DBG_DUMP("data[5]=0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\r\n",addr[5],addr[6],addr[size-5],addr[size-4],addr[size-3],addr[size-2],addr[size-1],addr[size]);
		}else if(((addr[4])==0xFE)){
			//DBG_DUMP("PIM OK\r\n");
			bPushData = 3;
			//DBG_DUMP("data[5]=0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\r\n",addr[5],addr[6],addr[size-5],addr[size-4],addr[size-3],addr[size-2],addr[size-1],addr[size]);
		}else{
			//DBG_DUMP("Check FAIL\r\n");
		}
	}else if(addr[0+DescSize] ==0 && addr[1+DescSize] ==0 && addr[2+DescSize] ==0 && addr[3+DescSize] ==1){
		if(((addr[4+DescSize])& 0x1F) == H264_NALU_TYPE_IDR && addr[5+DescSize] == H264_START_CODE_I){
			if(g_bsocketCliEthData2_AllowPush[path_id]==1){
				g_bsocketCliEthData2_ResetI[path_id]=0;
			}
			g_SocketCliData2_BsFrameCnt[path_id]++;
			eth_is_i_frame = 1;
			g_SocketCliData2_eth_i_cnt[path_id] ++;
			bPushData=1;
		}
	}
	//return;


	if(bPushData==1 && g_bsocketCliEthData2_AllowPush[path_id]==1){
		if(socketCliEthData2_IsRecv(path_id)==0)
		{
			socketCliEthData2_SetRecv(path_id, 1);
#if(ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
			#if((ETH_REARCAM_CAPS_COUNT+SENSOR_CAPS_COUNT)  >2)
			UI_SetData(FL_DUAL_CAM, DUALCAM_BOTH);
			#else
			UI_SetData(FL_DUAL_CAM, DUALCAM_BEHIND);
			#endif
			//CHKPNT;
			MovieExe_EthCam_ChgDispCB(UI_GetData(FL_DUAL_CAM));
#endif
		}

		//push data to VdoDec
	#if 1
		ISF_PORT         *pDist;
		ISF_VIDEO_STREAM_BUF    *pVidStrmBuf;

		UINT32 LongCounterSizeOffset=LONGCNT_STAMP_OFFSET;
		UINT32 StreamSize = size-LONGCNT_STAMP_OFFSET;
		UINT32 LongConterLow[ETHCAM_PATH_ID_MAX]={0};
		UINT32 LongConterHigh[ETHCAM_PATH_ID_MAX]={0};
		UINT64 LongConter[ETHCAM_PATH_ID_MAX]={0};
		static UINT32 LongCntTxRxOffset[ETHCAM_PATH_ID_MAX]={0};
		//DBG_DUMP("D2longcnt=0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\r\n",addr[StreamSize+0],addr[StreamSize+1],addr[StreamSize+2],addr[StreamSize+3],addr[StreamSize+4],addr[StreamSize+5],addr[StreamSize+6],addr[StreamSize+7],addr[StreamSize+8],addr[StreamSize+9],addr[StreamSize+10],addr[StreamSize+11],addr[StreamSize+12]);
		if(addr[StreamSize+0] ==0 && addr[StreamSize+1] ==0 && addr[StreamSize+2] ==0 && addr[StreamSize+3] ==1 && addr[StreamSize+4]==HEAD_TYPE_LONGCNT_STAMP){
			LongConterHigh[path_id]=(addr[StreamSize+5]<<24)+ (addr[StreamSize+6]<<16)+ (addr[StreamSize+7]<<8) + (addr[StreamSize+8]);
			LongConterLow[path_id]=(addr[StreamSize+9]<<24)+ (addr[StreamSize+10]<<16)+ (addr[StreamSize+11]<<8) + (addr[StreamSize+12]);
			LongConter[path_id]=(((UINT64)LongConterHigh[path_id]<<32) | LongConterLow[path_id]);
			if(g_SocketCliData2_BsFrameCnt[path_id]> 10 && g_SocketCliData2_BsFrameCnt[path_id] <= 60){
				LongCntTxRxOffset[path_id]+=HwClock_DiffLongCounter(LongConter[path_id], HwClock_GetLongCounter());
				if(g_SocketCliData2_BsFrameCnt[path_id] == 60){
					g_SocketCliData2_LongCntTxRxOffset[path_id]=LongCntTxRxOffset[path_id]/(60-10);
				}
			}else{
				LongCntTxRxOffset[path_id] =0;
			}
			//DBG_DUMP("D2_LongCntHigh[%d]=(%d,%d), %d, (%d,%d), diff=%d, %d\r\n",path_id,LongConterHigh[path_id],LongConterLow[path_id],(UINT32)LongConter[path_id],(UINT32)((HwClock_GetLongCounter() >> 32) & 0xFFFFFFFF),(UINT32)(HwClock_GetLongCounter() & 0xFFFFFFFF), (UINT32)HwClock_DiffLongCounter(LongConter[path_id], HwClock_GetLongCounter())/1000,(UINT32)g_SocketCliData2_LongCntTxRxOffset[path_id]/1000);
		}else{
			LongCounterSizeOffset=0;
		}

		g_SocketCliData2_InIsfData = gISF_EthCam_Pull_InData2;

		pVidStrmBuf = (ISF_VIDEO_STREAM_BUF *)&g_SocketCliData2_InIsfData.Desc;
		pVidStrmBuf->flag     = MAKEFOURCC('V', 'S', 'T', 'M');
		//pVidStrmBuf->DataAddr = (UINT32)pCliSocket_H264_data[g_queue_idx-1];//pVdoBs->BsAddr;
		if(sEthCamTxDecInfo[path_id].Codec == MEDIAVIDENC_H265 && eth_is_i_frame){
			pVidStrmBuf->DataAddr = (UINT32)(addr+sEthCamTxDecInfo[path_id].VPSSize+sEthCamTxDecInfo[path_id].SPSSize+sEthCamTxDecInfo[path_id].PPSSize);//uiBsFrmBufAddr;//g_EthBsFrmAddr[g_queue_idx-1];//pVdoBs->BsAddr;
			pVidStrmBuf->DataSize = (UINT32)(size-LongCounterSizeOffset-(sEthCamTxDecInfo[path_id].VPSSize+sEthCamTxDecInfo[path_id].SPSSize+sEthCamTxDecInfo[path_id].PPSSize));//g_SocketCliData1_BsBufMapTbl[g_SocketCliData1_BsQueueIdx];//g_CliSocket_size_frm[g_queue_idx-1];//pVdoBs->BsSize;
		}else if (sEthCamTxDecInfo[path_id].Codec == MEDIAVIDENC_H264 && eth_is_i_frame){
			pVidStrmBuf->DataAddr = (UINT32)(addr+sEthCamTxDecInfo[path_id].SPSSize+sEthCamTxDecInfo[path_id].PPSSize);//uiBsFrmBufAddr;//g_EthBsFrmAddr[g_queue_idx-1];//pVdoBs->BsAddr;
			pVidStrmBuf->DataSize = (UINT32)(size-LongCounterSizeOffset-(sEthCamTxDecInfo[path_id].SPSSize+sEthCamTxDecInfo[path_id].PPSSize));//g_SocketCliData1_BsBufMapTbl[g_SocketCliData1_BsQueueIdx];//g_CliSocket_size_frm[g_queue_idx-1];//pVdoBs->BsSize;
		}else{
		pVidStrmBuf->DataAddr = (UINT32)addr;//uiBsFrmBufAddr;//g_EthBsFrmAddr[g_queue_idx-1];//pVdoBs->BsAddr;
		pVidStrmBuf->DataSize = (UINT32)(size-LongCounterSizeOffset);//g_SocketCliData1_BsBufMapTbl[g_SocketCliData1_BsQueueIdx];//g_CliSocket_size_frm[g_queue_idx-1];//pVdoBs->BsSize;
		}
		//pVidStrmBuf->CodecType = MEDIAVIDENC_H264;                                          //codec type
		pVidStrmBuf->CodecType = sEthCamTxDecInfo[path_id].Codec;                                          //codec type
		//pVidStrmBuf->Resv[0]  = (UINT32)&(SPS_Addr[0]);                                     //sps addr
		if(sEthCamTxDecInfo[path_id].Codec == MEDIAVIDENC_H264){
		pVidStrmBuf->Resv[0]  = (UINT32)&(sEthCamTxDecInfo[path_id].Desc[0]);         //sps addr
		//pVidStrmBuf->Resv[1]  = 28;                                                         //sps size
		pVidStrmBuf->Resv[1]  = sEthCamTxDecInfo[path_id].SPSSize;                         //sps size
		//pVidStrmBuf->Resv[2]  = (UINT32)&(SPS_Addr[28]);                                    //pps addr
		pVidStrmBuf->Resv[2]  = (UINT32)&(sEthCamTxDecInfo[path_id].Desc[sEthCamTxDecInfo[path_id].SPSSize]);                                    //pps addr
		//pVidStrmBuf->Resv[3]  = 8;
		pVidStrmBuf->Resv[3]  = sEthCamTxDecInfo[path_id].PPSSize;                                                          //pps size
		pVidStrmBuf->Resv[4]  = 0;                                                          //vps addr
		pVidStrmBuf->Resv[5]  = 0;                                                          //vps size
		}else{
			pVidStrmBuf->Resv[0]  = (UINT32)&(sEthCamTxDecInfo[path_id].Desc[sEthCamTxDecInfo[path_id].VPSSize]);         //sps addr
			//pVidStrmBuf->Resv[1]  = 28;                                                         //sps size
			pVidStrmBuf->Resv[1]  = sEthCamTxDecInfo[path_id].SPSSize;                         //sps size
			//pVidStrmBuf->Resv[2]  = (UINT32)&(SPS_Addr[28]);                                    //pps addr
			pVidStrmBuf->Resv[2]  = (UINT32)&(sEthCamTxDecInfo[path_id].Desc[sEthCamTxDecInfo[path_id].VPSSize+sEthCamTxDecInfo[path_id].SPSSize]);  //pps addr
			//pVidStrmBuf->Resv[3]  = 8;
			pVidStrmBuf->Resv[3]  = sEthCamTxDecInfo[path_id].PPSSize;                                                          //pps size
			pVidStrmBuf->Resv[4]  = (UINT32)&(sEthCamTxDecInfo[path_id].Desc[0]);                                                          //vps addr
			pVidStrmBuf->Resv[5]  = sEthCamTxDecInfo[path_id].VPSSize;                                                          //vps size
		}
		UINT32 i_per_sec = sEthCamTxDecInfo[path_id].Fps/ sEthCamTxDecInfo[path_id].Gop;
		if (!i_per_sec) {
			i_per_sec = 1;
		}
		pVidStrmBuf->Resv[6]  = (eth_is_i_frame) ? 3 : 0;                                   //FrameType (IDR = 3, P = 0)
		pVidStrmBuf->Resv[7]  = (((g_SocketCliData2_eth_i_cnt[path_id] - 1) % i_per_sec) == 0 && eth_is_i_frame) ? TRUE : FALSE;    //IsIDR2Cut
		pVidStrmBuf->Resv[8]  = 0;                                                          //SVC size
		pVidStrmBuf->Resv[9]  = 0;                                                          //Temporal Id
		pVidStrmBuf->Resv[10] = (eth_is_i_frame) ? TRUE : FALSE;                            //IsKey
		pVidStrmBuf->Resv[12] = g_SocketCliData2_BsFrameCnt[path_id]-1;//pVdoBs->FrmIdx;
		pVidStrmBuf->Resv[13] = 0;//pVdoBs->bIsEOF;  //have next I Frame ?
		g_SocketCliData2_InIsfData.TimeStamp = HwClock_GetLongCounter();
		pDist = ImageUnit_In(&ISF_VdoDec, ImageApp_MovieMulti_GetEthCamVdoDecInPort(_CFG_ETHCAM_ID_1 + path_id));
		if (ImageUnit_IsAllowPush(pDist)) {
			//DBG_DUMP("Push\r\n");
			ImageUnit_PushData(pDist, &g_SocketCliData2_InIsfData, 0);
		}
	#endif
		EthCamHB2 = 0;
	} else if (bPushData == 2 || bPushData == 3) {
	#if 0
		ISF_PORT *pDist;
		ISF_VIDEO_STREAM_BUF *pVidStrmBuf;
		g_SocketCliData2_InIsfData = gISF_EthCam_Pull_InData1;

		pVidStrmBuf = (ISF_VIDEO_STREAM_BUF *)&g_SocketCliData2_InIsfData.Desc;
		if (bPushData == 2) {
			pVidStrmBuf->flag = MAKEFOURCC('T', 'H', 'U', 'M');
		} else {
			pVidStrmBuf->flag = MAKEFOURCC('J', 'S', 'T', 'M');
		}
		pVidStrmBuf->DataAddr = (UINT32)addr;//g_SocketCliData1_RawEncodeAddr;
		pVidStrmBuf->DataSize = (UINT32)size;//g_SocketCliData1_RawEncodeSize;
		pVidStrmBuf->Resv[0]  = 4;                                     //user_id
		g_SocketCliData2_InIsfData.TimeStamp = HwClock_GetLongCounter();
		pDist = ImageUnit_In(&ISF_Demux, ImageApp_MovieMulti_GetEthCamDemuxInPort(_CFG_ETHCAM_ID_1 + path_id));
		if (ImageUnit_IsAllowPush(pDist)) {
			//DBG_DUMP("Push thumb\r\n");
			ImageUnit_PushData(pDist, &g_SocketCliData2_InIsfData, 0);
		}
	#endif
	}
#endif
}
void socketCliEthData2_NotifyCB(ETHCAM_PATH_ID path_id, int status, int parm)
{
	switch (status) {
	case CYG_ETHSOCKETCLI_STATUS_CLIENT_REQUEST: {
			//DBG_DUMP("Notify EthCli REQUEST\r\n");
			break;
		}
	case CYG_ETHSOCKETCLI_STATUS_CLIENT_CONNECT: {
			DBG_DUMP("[%d]Notify EthCamSocketCli_Data2 connect OK\r\n",path_id);
			g_IsSocketCliData2_Conn[path_id]=1;
#if(ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE && ETH_REARCAM_CAPS_COUNT ==1)
			ImageApp_MovieMulti_EthCamLinkForDisp(_CFG_ETHCAM_ID_1+path_id, ENABLE, TRUE);
			EthCam_SendXMLCmd(path_id, ETHCAM_PORT_DATA2 ,ETHCAM_CMD_TX_STREAM_START, 0);	
#endif
			 bflag_EthLinkFinish = TRUE; // cj 20200511
			 //CHKPNT;
			break;
		}
	case CYG_ETHSOCKETCLI_STATUS_CLIENT_DISCONNECT: {
			DBG_ERR("[%d]disconnect!!!\r\n",path_id);
			g_IsSocketCliData2_Conn[path_id]=0;

			 bflag_EthLinkFinish = FALSE; // cj 20200511
			break;
		}
	case CYG_ETHSOCKETCLI_STATUS_CLIENT_SLOW: {
			DBG_ERR("[%d]Error!!!parm=%d\r\n",path_id,parm);
			if(path_id>=ETHCAM_PATH_ID_1 && path_id< ETHCAM_PATH_ID_MAX){
				EthCam_SendXMLCmd(path_id, ETHCAM_PORT_DEFAULT ,ETHCAM_CMD_DUMP_TX_BS_INFO, 0);
			}
			bflag_EthLinkFinish = FALSE; //zjf 21-1-26
			break;
		}
	}
}
void socketCliEthData2_RecvResetParam(ETHCAM_PATH_ID path_id)
{
#if (ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
	g_SocketCliData2_BsFrameCnt[path_id]=0;
	g_SocketCliData2_eth_i_cnt[path_id]=0;
	g_bsocketCliEthData2_AllowPush[path_id]=1;
	g_bsocketCliEthData2_ResetI[path_id]=0;
	g_SocketCliData2_LongCntTxRxOffset[path_id]=0;
#endif
}


#endif


static  CHAR g_SocketCmd_XMLCmdAddr[ETHCAM_PATH_ID_MAX][2048];
#if (defined(_NVT_ETHREARCAM_RX_))
static  CHAR g_SocketCmd_ParserBuf[ETHCAM_PATH_ID_MAX][2048] = {0};
static  UINT32 g_SocketCmd_ResultType[ETHCAM_PATH_ID_MAX]={0};
static  INT32 g_SocketCmd_CmdRet[ETHCAM_PATH_ID_MAX]={0};
static  UINT32 g_SocketCmd_RecvSize[ETHCAM_PATH_ID_MAX]={0};
static  INT32 g_SocketCmd_Cmd[ETHCAM_PATH_ID_MAX]={0};
static  ETHCAM_XML_CB_REGISTER g_SocketCmd_OutputCb[ETHCAM_PATH_ID_MAX]={0};
extern INT32 g_SocketCmd_Status[ETHCAM_PATH_ID_MAX];
#endif
#if (defined(_NVT_ETHREARCAM_TX_))
static  INT32 g_SocketCmd_CmdRet=0 ;
#endif
void socketEthCmd_NotifyCB(int status, int parm)
{
    if(status == CYG_ETHSOCKET_STATUS_CLIENT_CONNECT){
        DBG_DUMP("socketEthCmd_NotifyCB Connect OK\r\n");
	}
    else if(status == CYG_ETHSOCKET_STATUS_CLIENT_REQUEST){
    }
    else if(status == CYG_ETHSOCKET_STATUS_CLIENT_DISCONNECT){
        DBG_DUMP("socketEthCmd_NotifyCB DISConnect !!\r\n");
#if(defined(_NVT_ETHREARCAM_TX_))
        g_IsSocketCmdOpen=0;
#endif
	}
    else{
        DBG_ERR("^GUnknown status = %d, parm = %d\r\n", status, parm);
    }
}
void socketEthCmd_RecvCB(char* addr, int size)
{
	socketCliEthCmd_RecvCB(0, addr, size);
}
void socketCliEthCmd_RecvCB(ETHCAM_PATH_ID path_id, char* addr, int size)
{//CHKPNT;
#if (defined(_NVT_ETHREARCAM_RX_))
	if(addr && (size>0))
	{
		addr[size]='\0';
		//DBG_DUMP("path_id=%d, size=%d, addr[0]=%s\r\n", path_id, size, addr);
#if 1
		char   *pch;
		//UINT32 cmd = 0;
		char mimeType[64];//just a dummy buffer for using EthCamCmd_GetData
		//INT32 EthCamCmdRet;
		UINT32 segmentCount = 0;
		UINT32 uiSendBufSize;
		static BOOL bFirst=1;

		char dest_ip[16]={0};
		UINT32 dest_port = 80;
		char cdest_port[5]={0};
		UINT32 CmdBufShiftSize;
		UINT32 i=0;
		if(g_SocketCmd_ResultType[path_id]==ETHCAM_XML_RESULT_TYPE_LIST && g_SocketCmd_CmdRet[path_id]==ETHCAM_RET_CONTINUE){
			if(memcmp((addr+size-1-strlen("</LIST>")), "</LIST>",strlen("</LIST>"))==0){
				memcpy(g_SocketCmd_XMLCmdAddr[path_id]+g_SocketCmd_RecvSize[path_id], addr, size);
				//DBG_DUMP(" size=%d, XMLCmdAddr=%s\r\n",size,g_SocketCmd_XMLCmdAddr[path_id]+g_SocketCmd_RecvSize[path_id]);

				g_SocketCmd_CmdRet[path_id]=EthCamCmdXML_parser(g_SocketCmd_Cmd[path_id],g_SocketCmd_XMLCmdAddr[path_id],&g_SocketCmd_OutputCb[path_id]);
				if(g_SocketCmd_CmdRet[path_id]==0){
					EthCamCmd_Done(path_id, ETHCAM_CMD_DONE, ETHCAM_RET_OK);
				}else{
					DBG_ERR("EthCam Result error, EthCamCmdRet[%d]=%d\r\n",path_id,g_SocketCmd_CmdRet[path_id]);
					EthCamCmd_Done(path_id, ETHCAM_CMD_DONE, ETHCAM_RET_ERR);
				}
			}else{
				DBG_ERR("cmd[%d]=%d  list result error!\r\n",path_id, g_SocketCmd_Cmd[path_id]);
				EthCamCmd_Done(path_id, ETHCAM_CMD_DONE, ETHCAM_RET_ERR);
			}
			return;
		}
		//memcpy(g_SocketCmd_RecvAddr, addr, size);
		//192.168.0.12:8899<?xml version="1.0" encoding="UTF-8" ?>
		//sscanf(addr, "%15[^:]:%99d<%2046[^]", dest_ip, &dest_port, (g_SocketCmd_XMLCmdAddr+1));
		//g_SocketCmd_XMLCmdAddr[0]='<';
		sscanf(addr, "%15[^:]:%99d", dest_ip, &dest_port);
		//DBG_DUMP("addr=%s\r\n",  addr);
		if(dest_port !=SocketInfo[0].port[0] && dest_port !=SocketInfo[0].port[1] && dest_port !=SocketInfo[0].port[2]){
			DBG_ERR("[%d]port error! %d, sz=%d, %s\r\n", path_id, cdest_port, size, addr);
			return;
		}

		sprintf(cdest_port,"%d",dest_port);
		//DBG_DUMP("cdest_port=%s\r\n",  cdest_port);
		//DBG_DUMP("ip=%s, strlen(ip)=%d, port=%s\r\n",  dest_ip,strlen(dest_ip), cdest_port);
		CmdBufShiftSize=strlen(dest_ip)+ strlen(cdest_port)+1; //+1 for :
		//DBG_DUMP("CmdBufShiftSize=%d\r\n",  CmdBufShiftSize);
		memcpy(g_SocketCmd_XMLCmdAddr[path_id], addr+CmdBufShiftSize, size);
		//DBG_DUMP("dest_ip=%s, dest_port=%d, XMLcmd=%s\r\n",  dest_ip, dest_port, g_SocketCmd_XMLCmdAddr);
		//DBG_ERR("addr=%s, size=%d\r\n",  addr, size);

		//char pbuf_size[] = "TEST cmd ack";
		//UINT32 bufSize=strlen(pbuf_size)+1;
		//UserSocketCliEthCmd_Send(pbuf_size, (int *)&bufSize);
		if(bFirst){
			EthCamCmd_Init();
			bFirst=0;
		}

		//Get XML Cmd
		if (*g_SocketCmd_XMLCmdAddr[path_id] == ETHCAM_CMD_ROOT) {
			//CHKPNT;
			DBG_IND("cmd (%s)\r\n", g_SocketCmd_XMLCmdAddr[path_id]);
			pch = strchr((char *)g_SocketCmd_XMLCmdAddr[path_id], ETHCAM_CMD_CUSTOM_TAG);
			if (pch) {
				if (strncmp(pch + 1, CMD_STR, strlen(CMD_STR)) == 0) {
					sscanf_s(pch + 1 + strlen(CMD_STR), "%d", &g_SocketCmd_Cmd[path_id]);
					DBG_IND("Recv cmd=%d\r\n",g_SocketCmd_Cmd[path_id]);
					//to do
					//specail handle

				}

				*pch = 0;
				uiSendBufSize= sizeof(g_SocketCmd_ParserBuf[path_id]);
				g_SocketCmd_CmdRet[path_id] = EthCamCmd_GetData(path_id, (char *)g_SocketCmd_XMLCmdAddr[path_id], pch + 1, (UINT32)g_SocketCmd_ParserBuf[path_id], &uiSendBufSize, mimeType, segmentCount);
				DBG_IND("EthCamCmdRet[%d]=%d, len=%d(0x%X)\r\n", path_id,g_SocketCmd_CmdRet[path_id], uiSendBufSize, uiSendBufSize);
				if (ETHCAM_CMD_GETDATA_RETURN_OK == g_SocketCmd_CmdRet[path_id]) {
					DBG_IND("XML Return:\r\n%s\r\n", g_SocketCmd_ParserBuf[path_id]);
					//UserSocketCliEthCmd_Send(g_peth_cam_buf, &uiSendBufSize);
				}
			}
		}else{//Get XML Result
		//CHKPNT;
			g_SocketCmd_Cmd[path_id] = (INT32)EthCamCmdXML_GetCmdId(g_SocketCmd_XMLCmdAddr[path_id]);
			//DBG_DUMP("\r\ncmd=%d\r\n", g_SocketCmd_Cmd[path_id]);

			if(g_SocketCmd_Cmd[path_id]>0){
				//ETHCAM_XML_CB_REGISTER output_cb={0};
				memset(&g_SocketCmd_OutputCb[path_id],0,sizeof(ETHCAM_XML_CB_REGISTER));
				//UINT32 result_type=0;
				ETHCAM_XML_RESULT *EthCamXMLResultTbl=EthCamCmd_GetResultTable();
				EthCam_GetDest(addr, &g_SocketCmd_OutputCb[path_id].path_id, &g_SocketCmd_OutputCb[path_id].port_type);
				//DBG_DUMP("path_id=%d, port_type=%d\r\n", g_SocketCmd_OutputCb[path_id].path_id, g_SocketCmd_OutputCb[path_id].port_type);
				//debug_msg("--cj Get XML Result path_id=%d, port_type=%d Cmd_id = %d\r\n", g_SocketCmd_OutputCb[path_id].path_id, g_SocketCmd_OutputCb[path_id].port_type);

				while (EthCamXMLResultTbl[i].cmd != 0) {
					if (g_SocketCmd_Cmd[path_id] == EthCamXMLResultTbl[i].cmd) {
						g_SocketCmd_OutputCb[path_id].EthCamXML_data_CB=EthCamXMLResultTbl[i].output_cb.EthCamXML_data_CB;
						g_SocketCmd_ResultType[path_id]=EthCamXMLResultTbl[i].result_type;
						break;
					}
					i++;
				}
				if(g_SocketCmd_ResultType[path_id]==0){
					DBG_ERR("result_type error\r\n");
				}
				//debug_msg("--cj Get XML Result result_type%d ", g_SocketCmd_ResultType[path_id]);
				DBG_IND("result_type[%d]=%d\r\n",path_id,g_SocketCmd_ResultType[path_id]);
				if(g_SocketCmd_ResultType[path_id]==ETHCAM_XML_RESULT_TYPE_LIST){
					if(memcmp((g_SocketCmd_XMLCmdAddr[path_id]+size-1-CmdBufShiftSize-strlen("</LIST>")), "</LIST>",strlen("</LIST>"))!=0){//list data not finish
						g_SocketCmd_RecvSize[path_id]=size-1-CmdBufShiftSize;
						g_SocketCmd_CmdRet[path_id]=ETHCAM_RET_CONTINUE;
						//EthCamCmd_Done(path_id, ETHCAM_CMD_DONE, ETHCAM_RET_CONTINUE);
						return;
					}
				}
				g_SocketCmd_Status[path_id]=0xff;
				g_SocketCmd_CmdRet[path_id]=EthCamCmdXML_parser(g_SocketCmd_Cmd[path_id],g_SocketCmd_XMLCmdAddr[path_id],&g_SocketCmd_OutputCb[path_id]);
				//DBG_DUMP("cmdret = %d \r\n",g_SocketCmd_CmdRet[path_id]);
				//DBG_DUMP("g_SocketCmd_Status = %d \r\n",g_SocketCmd_Status[path_id]);
				if(g_SocketCmd_CmdRet[path_id]==0){
					EthCamCmd_Done(path_id, ETHCAM_CMD_DONE, ETHCAM_RET_OK);
					if(//g_SocketCmd_Status[path_id]==ETHCAM_RET_OK &&
						(g_SocketCmd_ResultType[path_id] == ETHCAM_XML_RESULT_TYPE_VALUE_RESULT || g_SocketCmd_ResultType[path_id] == ETHCAM_XML_RESULT_TYPE_STRING_RESULT)){
						sEthCamSendCmdInfo.BufSize= sizeof(sEthCamSendCmdInfo.ParserBuf)- CmdBufShiftSize;
						XML_DefaultFormat(g_SocketCmd_Cmd[path_id], ETHCAM_RET_ACK, (UINT32)sEthCamSendCmdInfo.ParserBuf+CmdBufShiftSize, &sEthCamSendCmdInfo.BufSize, mimeType);
						sEthCamSendCmdInfo.path_id=path_id;
						for(i=0;i<strlen(dest_ip);i++){
							sEthCamSendCmdInfo.ParserBuf[i]=dest_ip[i];
						}
						sEthCamSendCmdInfo.ParserBuf[strlen(dest_ip)]=':';
						for(i=(strlen(dest_ip)+1);i<(strlen(dest_ip)+1+strlen(cdest_port));i++){
							sEthCamSendCmdInfo.ParserBuf[i]=cdest_port[i-(strlen(dest_ip)+1)];
						}
						sEthCamSendCmdInfo.BufSize += CmdBufShiftSize;
						//EthCamCmd_Send(path_id, (char*)sEthCamSendCmdInfo.ParserBuf, (int*)&sEthCamSendCmdInfo.BufSize);
						set_flg(ETHCAM_CMD_SND_FLG_ID,FLG_ETHCAM_CMD_SND);
					}
				}else if(g_SocketCmd_CmdRet[path_id]==ETHCAM_RET_CONTINUE){
					EthCamCmd_Done(path_id, ETHCAM_CMD_DONE, ETHCAM_RET_CONTINUE);
				}else if(g_SocketCmd_CmdRet[path_id]==ETHCAM_RET_CMD_NOT_FOUND){
					EthCamCmd_Done(path_id, ETHCAM_CMD_DONE, ETHCAM_RET_CMD_NOT_FOUND);
				}else{
					EthCamCmd_Done(path_id, ETHCAM_CMD_DONE, ETHCAM_RET_ERR);
					DBG_ERR("EthCam Result error, EthCamCmdRet[%d]=%d\r\n",path_id,g_SocketCmd_CmdRet[path_id]);
				}
			}
		}

#endif
	}
#else //===================== TX Start =====================

	if (addr && (size > 0)) {
	//return TRUE;

		addr[size] = '\0';
		//DBG_DUMP("size=%d, addr[0]=%s\r\n", size, addr);

#if 0
		DBG_DUMP("size=%d, addr[0]=0x%x, addr[1]=0x%x\r\n", size, addr[0], addr[1]);
		EthCamCmd_Rcv((ETHCAM_CMD_FMT*)addr);
#endif

		static char   *pch=0;
		static UINT32 cmd = 0;
		static ETHCAM_PATH_ID path_id;
		static ETHCAM_PORT_TYPE port_type;
		char mimeType[64] = {0};
		sprintf(mimeType, "text/xml");

		UINT32 segmentCount = 0;
		//UINT32 uiSendBufSize;
		static BOOL bFirst=1;
		char dest_ip[16]={0};
		UINT32 dest_port = 80;
		char cdest_port[5]={0};
		static UINT32 CmdBufShiftSize=0;
		UINT32 i=0;

		//memcpy(g_SocketCmd_RecvAddr, addr, size);
		if(ETHCAM_CMD_GETDATA_RETURN_CONTINUE == g_SocketCmd_CmdRet
			|| ETHCAM_CMD_GETDATA_RETURN_CONTI_NEED_ACKDATA== g_SocketCmd_CmdRet) {
			if (strncmp(addr, "192.168.", strlen("192.168.")) == 0) {
				//DBG_WRN("CONTINUE cmd=%d, size=%d, addr=%s\r\n",  cmd, size, addr);
				DBG_WRN("CONTINUE cmd=%d, size=%d\r\n",  cmd, size);
				return;
			}

			DBG_IND("CONTINUE cmd=%d, size=%d\r\n",  cmd, size);
			segmentCount=1;
			if(ETHCAM_CMD_GETDATA_RETURN_CONTINUE == g_SocketCmd_CmdRet){
				g_SocketCmd_CmdRet = EthCamCmd_GetData(path_id, (char *)addr, pch + 1, (UINT32)(addr), (UINT32*)&size, mimeType, segmentCount);
			}else{
				memcpy(&sEthCamSendCmdInfo.ParserBuf[CmdBufShiftSize], addr, size);
				sEthCamSendCmdInfo.BufSize= sizeof(sEthCamSendCmdInfo.ParserBuf)- CmdBufShiftSize;
				g_SocketCmd_CmdRet = EthCamCmd_GetData(path_id, (char *)addr, pch + 1, (UINT32)(sEthCamSendCmdInfo.ParserBuf+CmdBufShiftSize), &sEthCamSendCmdInfo.BufSize, mimeType, segmentCount);
			}
			if(g_SocketCmd_CmdRet==ETHCAM_CMD_GETDATA_RETURN_OK){
				if(ETHCAM_CMD_GETDATA_RETURN_CONTINUE == g_SocketCmd_CmdRet){
					EthCam_SendXMLStatusCB(path_id,port_type,cmd, ETHCAM_RET_OK);
				}else{
					sEthCamSendCmdInfo.BufSize+=CmdBufShiftSize;
					DBG_IND("XML Return:\r\n%s, size=%d,%d\r\n", sEthCamSendCmdInfo.ParserBuf, sEthCamSendCmdInfo.BufSize, strlen(sEthCamSendCmdInfo.ParserBuf));
					//EthCamCmd_Send(sEthCamSendCmdInfo.ParserBuf, (int *)&sEthCamSendCmdInfo.BufSize);
					set_flg(ETHCAM_CMD_SND_FLG_ID,FLG_ETHCAM_CMD_SND);
				}
			}else if(g_SocketCmd_CmdRet==ETHCAM_CMD_GETDATA_RETURN_ERROR){
				EthCam_SendXMLStatusCB(path_id,port_type,cmd, ETHCAM_RET_ERR);
			}
			return;
		}
		sscanf(addr, "%15[^:]:%99d", dest_ip, &dest_port);
		sprintf(cdest_port,"%d",dest_port);
		CmdBufShiftSize=strlen(dest_ip)+ strlen(cdest_port)+1; //+1 for :
		memcpy(g_SocketCmd_XMLCmdAddr[0], addr+CmdBufShiftSize, size);
		DBG_IND("dest_ip=%s, dest_port=%d, XMLcmd=%s\r\n",  dest_ip, dest_port, g_SocketCmd_XMLCmdAddr);

		DBG_IND("CmdBufShiftSize=%d\r\n",CmdBufShiftSize);
		EthCam_GetDest(addr, &path_id, &port_type);

		if(bFirst){
			EthCamCmd_Init();
			bFirst=0;
		}

		//Get XML Cmd
		if (*g_SocketCmd_XMLCmdAddr[0] == ETHCAM_CMD_ROOT) {
			//DBG_DUMP("XML cmd=%s\r\n", g_SocketCmd_XMLCmdAddr);
			pch = strchr((char *)g_SocketCmd_XMLCmdAddr[0], ETHCAM_CMD_CUSTOM_TAG);
			if (pch) {
				if (strncmp(pch + 1, CMD_STR, strlen(CMD_STR)) == 0) {
					sscanf_s(pch + 1 + strlen(CMD_STR), "%d", &cmd);
					if(cmd!=ETHCAM_CMD_GET_FRAME){
						DBG_DUMP("cmd Id=%d\r\n",cmd);
					}
					//to do
					//specail handle

				}
				//pch=?custom=1&cmd=9008&par=0
				*pch = 0;

				sEthCamSendCmdInfo.BufSize= sizeof(sEthCamSendCmdInfo.ParserBuf)- CmdBufShiftSize;
				DBG_IND("uiSendBufSize=(%d)\r\n", sEthCamSendCmdInfo.BufSize);
				DBG_IND("strlen(dest_ip)=(%d), strlen(cdest_port)=%d\r\n", strlen(dest_ip),strlen(cdest_port));

				g_SocketCmd_CmdRet = EthCamCmd_GetData(path_id, (char *)addr, pch + 1, (UINT32)(sEthCamSendCmdInfo.ParserBuf+CmdBufShiftSize), &sEthCamSendCmdInfo.BufSize, mimeType, segmentCount);
				DBG_IND("EthCamCmdRet=%d, len=%d\r\n", g_SocketCmd_CmdRet, sEthCamSendCmdInfo.BufSize);
				if (ETHCAM_CMD_GETDATA_RETURN_OK == g_SocketCmd_CmdRet
					|| ETHCAM_CMD_GETDATA_RETURN_CONTINUE == g_SocketCmd_CmdRet
					|| ETHCAM_CMD_GETDATA_RETURN_CONTI_NEED_ACKDATA== g_SocketCmd_CmdRet) {
					for(i=0;i<strlen(dest_ip);i++){
						sEthCamSendCmdInfo.ParserBuf[i]=dest_ip[i];
					}
					sEthCamSendCmdInfo.ParserBuf[strlen(dest_ip)]=':';
					for(i=(strlen(dest_ip)+1);i<(strlen(dest_ip)+1+strlen(cdest_port));i++){
						sEthCamSendCmdInfo.ParserBuf[i]=cdest_port[i-(strlen(dest_ip)+1)];
					//DBG_DUMP("cdest_port[%d]=%x\r\n", i,cdest_port[i]);
					}
					sEthCamSendCmdInfo.BufSize += CmdBufShiftSize;
					DBG_IND("XML Return:\r\n%s\r\n", sEthCamSendCmdInfo.ParserBuf);
					//EthCamCmd_Send(sEthCamSendCmdInfo.ParserBuf, (int *)&sEthCamSendCmdInfo.BufSize);
					set_flg(ETHCAM_CMD_SND_FLG_ID,FLG_ETHCAM_CMD_SND);
				}
			}
		}else{//Get XML Result
			EthCamCmd_Done(path_id, ETHCAM_CMD_DONE, ETHCAM_RET_OK);
		}
	}
#endif//===================== TX End =====================
}
void socketCliEthCmd_NotifyCB(ETHCAM_PATH_ID path_id, int status, int parm)
{
	switch (status) {
	case CYG_ETHSOCKETCLI_STATUS_CLIENT_SOCKET_CLOSE: {
			//DBG_ERR("[%d] close!!!\r\n",path_id);
			break;
		}
	case CYG_ETHSOCKETCLI_STATUS_CLIENT_REQUEST: {
			//DBG_DUMP("Notify EthCli REQUEST\r\n");
			break;
		}
	case CYG_ETHSOCKETCLI_STATUS_CLIENT_CONNECT: {
			DBG_DUMP("socketCliEthCmd_NotifyCB[%d] connect OK\r\n",path_id);
#if(defined(_NVT_ETHREARCAM_RX_))
			UINT32 AllPathLinkStatus=0;
			UINT16 i=0;

			EthCamSocketCli_ReConnect(path_id, 0, 0);

			for(i=0;i<ETH_REARCAM_CAPS_COUNT;i++){
				if(EthCamNet_GetEthLinkStatus(i)==ETHCAM_LINK_UP){
					AllPathLinkStatus++;
				}
			}
			//DBG_DUMP("AllPathLinkStatus=%d\r\n",AllPathLinkStatus);

			if(AllPathLinkStatus==ETH_REARCAM_CAPS_COUNT){
				SxTimer_SetFuncActive(SX_TIMER_ETHCAM_ETHLINKRETRY_ID, FALSE);
				//EthCamNet_UDPSocketClose();
			}

			g_IsSocketCliCmdConn[path_id]=1;

			if(EthCamNet_IsIPConflict()==1){
				DBG_WRN("IPConflict[%d] Reset IP\r\n",path_id);
				EthCam_SendXMLCmd(path_id, ETHCAM_PORT_DEFAULT ,ETHCAM_CMD_SET_TX_IP_RESET, 0);
				EthCam_SendXMLCmd(path_id, ETHCAM_PORT_DEFAULT ,ETHCAM_CMD_TX_POWEROFF, 0);
				break;
			}
			#if (ETH_REARCAM_CAPS_COUNT>=2)
			for (i=0; i<ETH_REARCAM_CAPS_COUNT; i++){
					sEthCamTxSysInfo[i].PullModeEn=1;
			}
			#else
			sEthCamTxSysInfo[0].PullModeEn=0;
			#endif
			#if(ETH_REARCAM_CLONE_FOR_DISPLAY == ENABLE)
			sEthCamTxSysInfo[path_id].CloneDisplayPathEn=1;
			#else
			sEthCamTxSysInfo[path_id].CloneDisplayPathEn=0;
			#endif
			sEthCamCodecSrctype[path_id].VCodec=UI_GetData(FL_MOVIE_CODEC);
			sEthCamCodecSrctype[path_id].Srctype=ETHCAM_TX_SYS_SRCTYPE_67;
			sEthCamCodecSrctype[path_id].bCmdOK=0;
			DBG_DUMP("[%d]PullModeEn=%d, CloneDisplayPathEn=%d, VCode=%d, Srctype=%d\r\n",path_id, sEthCamTxSysInfo[path_id].PullModeEn,sEthCamTxSysInfo[path_id].CloneDisplayPathEn,sEthCamCodecSrctype[path_id].VCodec,sEthCamCodecSrctype[path_id].Srctype);

#endif
			EthCamCmdTsk_Open();
			#if 0
			BKG_PostEvent(NVTEVT_BKW_ETHCAM_IPERF_TEST);
			#else
			if ((System_IsModeChgClose()==0 && System_GetState(SYS_STATE_CURRMODE) == PRIMARY_MODE_MOVIE)|| System_GetState(SYS_STATE_NEXTMODE) == PRIMARY_MODE_MOVIE) {
					BKG_PostEvent(NVTEVT_BKW_ETHCAM_SET_TX_SYSINFO);

					#if (ETH_REARCAM_CAPS_COUNT>=2)
					//CHKPNT;
					BKG_PostEvent(NVTEVT_BKW_GET_ETHCAM_TX_INFO);
					//CHKPNT;
					BKG_PostEvent(NVTEVT_BKW_ETHCAM_SOCKETCLI_DATA_OPEN);
					BKG_PostEvent(NVTEVT_BKW_TX_STREAM_START);
					#else // here
					//CHKPNT;
					BKG_PostEvent(NVTEVT_BKW_GET_ETHCAM_TX_RECINFO);
					BKG_PostEvent(NVTEVT_BKW_GET_ETHCAM_TX_DECINFO);
					//BKG_PostEvent(NVTEVT_BKW_ETHCAM_SOCKETCLI_DISP_DATA_OPEN_START);
					BKG_PostEvent(NVTEVT_BKW_ETHCAM_SOCKETCLI_DISP_DATA_OPEN);
					//BKG_PostEvent(NVTEVT_BKW_ETHCAM_SYNC_TIME);
					//BKG_PostEvent(NVTEVT_BKW_ETHCAM_SOCKETCLI_REC_DATA_OPEN_START);
					#endif
			}
			#endif
			break;
		}
	case CYG_ETHSOCKETCLI_STATUS_CLIENT_DISCONNECT: {
			DBG_ERR("[%d] disconnect!!!\r\n",path_id);
#if(defined(_NVT_ETHREARCAM_RX_))
			g_IsSocketCliCmdConn[path_id]=0;
#endif
			break;
		}
	}
}
#if(defined(_NVT_ETHREARCAM_RX_))
#include "UIFlow.h"
INT8 ethsocket_sendsize;
void socketCliEthCmd_SendSizeCB(int size, int total_size)
{
	ethsocket_sendsize = size*100/total_size;
	DBG_IND("%d, %d, Send %d Percent\r\n",size ,total_size ,size*100/total_size);
	if (System_GetState(SYS_STATE_NEXTSUBMODE) == SYS_SUBMODE_UPDFW
		&& System_GetState(SYS_STATE_CURRSUBMODE)  == SYS_SUBMODE_UPDFW) {
		static char g_StringTmpBuf[64] = {0};
	    	snprintf(g_StringTmpBuf, sizeof(g_StringTmpBuf), "Send FW to Tx: %d",size*100/total_size);
			//---zjf cancel
			//CHKPNT;
	    	//UxState_SetItemData(&UIFlowWndWaitMoment_StatusTXT_MsgCtrl, 0, STATE_ITEM_STRID,  Txt_Pointer(g_StringTmpBuf));
			//UxCtrl_SetShow(&UIFlowWndWaitMoment_StatusTXT_MsgCtrl, TRUE);
		}
}
BOOL socketCliEthCmd_IsConn(ETHCAM_PATH_ID path_id)
{
	return g_IsSocketCliCmdConn[path_id];
}
BOOL socketCliEthData1_IsConn(ETHCAM_PATH_ID path_id)
{
	return g_IsSocketCliData1_Conn[path_id];
}
BOOL socketCliEthData2_IsConn(ETHCAM_PATH_ID path_id)
{
	return g_IsSocketCliData2_Conn[path_id];
}
#if (ETH_REARCAM_CAPS_COUNT< 2)
static SWTIMER_ID  g_socketCliEthDecErrTimeID=SWTIMER_NUM;
static UINT32  g_issocketCliEthDecErrTimerOpen=0;
static void socketCliEth_DecErrTimer_TimeOutCB(UINT32 uiEvent)
{
	//DBG_DUMP("DecErrTimer_TimeOut, VdoDec_ChkTskReady=%d\r\n",NMP_VdoDec_ChkTskReady());
	if(NMP_VdoDec_ChkTskReady()){
		BKG_PostEvent(NVTEVT_BKW_ETHCAM_DEC_ERR_RESTART);
	}
}
#endif
void socketCliEth_DecErrTimerOpen(void)
{
#if (ETH_REARCAM_CAPS_COUNT< 2)
	SWTIMER_CB EventHandler;
	if(g_issocketCliEthDecErrTimerOpen){
		DBG_WRN("timer already open\r\n");
		return;
	}
	EventHandler=(SWTIMER_CB)socketCliEth_DecErrTimer_TimeOutCB;
	if (SwTimer_Open(&g_socketCliEthDecErrTimeID, EventHandler) != E_OK) {
		DBG_ERR("open timer fail\r\n");
	} else {
		SwTimer_Cfg(g_socketCliEthDecErrTimeID, 300 /*ms*/, SWTIMER_MODE_ONE_SHOT);
		SwTimer_Start(g_socketCliEthDecErrTimeID);
		g_issocketCliEthDecErrTimerOpen=1;
	}
#endif
}
void socketCliEth_DecErrTimerClose(void)
{
#if (ETH_REARCAM_CAPS_COUNT< 2)
	if(g_issocketCliEthDecErrTimerOpen==0){
		return;
	}
	if(g_socketCliEthDecErrTimeID !=SWTIMER_NUM){
		DBG_DUMP("DecErr close timer\r\n");

		SwTimer_Stop(g_socketCliEthDecErrTimeID);
		SwTimer_Close(g_socketCliEthDecErrTimeID);
	}
	g_socketCliEthDecErrTimeID=SWTIMER_NUM;
	g_issocketCliEthDecErrTimerOpen=0;
#endif
}
#endif
#endif
