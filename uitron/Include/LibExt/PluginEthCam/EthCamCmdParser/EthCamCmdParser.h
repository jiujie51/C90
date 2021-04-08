#ifndef _ETHCAMCMDPARSER_H_
#define _ETHCAMCMDPARSER_H_

#include "Type.h"
extern void EthCamCmd_InstallID(UINT32 path_cnt) _SECTION(".kercfg_text");


typedef struct _ETHCAM_CMD_ENTRY {
	UINT32                cmd;
	UINT32                event;
	UINT32                usrCB;
	FLGPTN                Waitflag;
	UINT32                UIflag;
}
ETHCAM_CMD_ENTRY;

typedef void EthCamCmd_DefCB(UINT32 cmd, UINT32 ret, UINT32 bufAddr, UINT32 *bufSize, char *mimeType);
typedef void EthCamCmd_EventHandle(UINT32 evt, UINT32 paramNum, ...);
typedef int  EthCamCmd_getCustomData(char *path, char *argument, UINT32 bufAddr, UINT32 *bufSize, char *mimeType, UINT32 segmentCount);
typedef int  EthCamCmd_puttCustomData(char *path, char *argument, UINT32 bufAddr, UINT32 bufSize, UINT32 segmentCount, UINT32 putStatus);
//#NT#2016/03/23#Isiah Chang -begin
//#NT#add new Wi-Fi UI flow.
typedef INT32  EthCamCmd_APPStartupCheck(void);
//#NT#2016/03/23#Isiah Chang -end

#define ETHCAM_CMD_BEGIN(tbl) ETHCAM_CMD_ENTRY (Cmd_##tbl)[]={ ///< begin a command table
#define ETHCAM_CMD_ITEM(cmd,event,func,wait,UIflag)  {(cmd), (event),(func),(wait),(UIflag)}, ///< insert a command item in command table
#define ETHCAM_CMD_END()    {0,0,0,0}}; ///< end a command table

#define CMD_STR "custom=1&cmd="
#define PAR_STR "&par="
#define PARS_STR "&str="
#define HTML_PATH "A:\\test.htm"

#define ETHCAM_CMD_STATE_ERR    (-22)
#define ETHCAM_CMD_TERMINATE    (-255)
#define ETHCAM_CMD_NOT_FOUND    (-256)

#define ETHCAM_CMD_GETDATA_RETURN_ERROR               -1   // has error
#define ETHCAM_CMD_GETDATA_RETURN_OK                  0    // ok get all data
#define ETHCAM_CMD_GETDATA_RETURN_CONTINUE            1    // has more data need to get
#define ETHCAM_CMD_GETDATA_RETURN_WAIT            	2    // wait operation done
#define ETHCAM_CMD_GETDATA_RETURN_CONTI_NEED_ACKDATA 	3    // has more data need to get and need feedback data

#define ETHCAM_CMD_GETDATA_SEGMENT_ERROR_BREAK        0xFFFFFFFF    ///<  hfs error break, the connection may be closed

#define ETHCAM_CMD_UPLOAD_OK                      0    ///<  upload file ok
#define ETHCAM_CMD_UPLOAD_FAIL_FILE_EXIST         -1   ///<  upload file fail because of file exist
#define ETHCAM_CMD_UPLOAD_FAIL_RECEIVE_ERROR      -2   ///<  receive data has some error
#define ETHCAM_CMD_UPLOAD_FAIL_WRITE_ERROR        -3   ///<  write file has some error
#define ETHCAM_CMD_UPLOAD_FAIL_FILENAME_EMPTY     -4   ///<  file name is emtpy

typedef enum _ETHCAM_CMD_PUT_STATUS {
	ETHCAM_CMD_PUT_STATUS_CONTINUE               =   0, ///< still have data need to put
	ETHCAM_CMD_PUT_STATUS_FINISH                 =   1, ///< put data finish
	ETHCAM_CMD_PUT_STATUS_ERR                    =  -1, ///< some error happened
} ETHCAM_CMD_PUT_STATUS;

#define ETHCAM_PAR_STR_LEN        (1024)      ///< max string length of URL cmd

extern void   EthCamCmd_Done(UINT32 path_id, FLGPTN flag, UINT32 result);
extern void   EthCamCmd_SetExecTable(ETHCAM_CMD_ENTRY *pAppCmdTbl);
extern ETHCAM_CMD_ENTRY   *EthCamCmd_GetExecTable(void);
extern INT32  EthCamCmd_GetData(UINT32 path_id, char *path, char *argument, UINT32 bufAddr, UINT32 *bufSize, char *mimeType, UINT32 segmentCount);
extern INT32  EthCamCmd_PutData(UINT32 path_id, char *path, char *argument, UINT32 bufAddr, UINT32 bufSize, UINT32 segmentCount, UINT32 putStatus);
extern void   EthCamCmd_SetDefautCB(UINT32 defaultCB);
extern void   EthCamCmd_SetEventHandle(UINT32 eventHandle);
extern void   EthCamCmd_ReceiveCmd(UINT32 enable);
//#NT#2016/03/23#Isiah Chang -begin
//#NT#add new Wi-Fi UI flow.
extern void   EthCamCmd_SetAppStartupChecker(UINT32 app_startupchecker);
extern void   EthCamCmd_SetAppStartupCmdCode(UINT32 cmd_code);
//#NT#2016/03/23#Isiah Chang -end
extern void   EthCamCmd_UnlockString(UINT32 path_id, char *string);
extern char   *EthCamCmd_LockString(UINT32 path_id);
extern UINT32 EthCamCmd_WaitFinish(UINT32 path_id, FLGPTN waiptn);
extern void   EthCamCmd_ClrFlg(UINT32 path_id, FLGPTN flag);
extern UINT32 EthCamCmd_GetCurCmd(void);
#endif //_ETHCAMCMDPARSER_H_



