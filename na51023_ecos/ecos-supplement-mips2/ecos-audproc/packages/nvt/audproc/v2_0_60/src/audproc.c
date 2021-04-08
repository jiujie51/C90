#include <stdio.h>
#include <stdlib.h>
#include <cyg/kernel/kapi.h>
//#include <cyg/infra/maincmd.h>
//#include <cyg/infra/mainfunc.h>
//#include <cyg/infra/diag.h>
//#include <cyg/compat/uitron/uit_func.h>
//--------------------------------------------------------------------------
#include <pthread.h>
#include <cyg/audproc/audproc.h>
#include <cyg/audproc/api_wakeup.h>
#include "audproc_int.h"

#define AUDPROC_ECOS_VER                20180921

#define AUDPROC_THREAD_PRIORITY         8
#define AUDPROC_THREAD_STACK_SIZE       8192

#define AISPEECH_AUDPROC_OPEN

static cyg_uint32 is_audproc_opened = 0;
static NVTIPC_I32 msqid = -1;
static cyg_handle_t audproc_thread;
static cyg_thread   audproc_thread_object;
static cyg_uint8 audproc_stacks[AUDPROC_THREAD_STACK_SIZE] = {0};
static AISpeechAuthInfo *pAuthInfo = NULL;

static char *szCmdLine[] = {"wo yao pai zhao", "zhua pai zhao pian", "wo yao lu xiang", "da kai ping mu", 
                            "guan bi ping mu", "da kai re dian",     "guan bi re dian", "da kai lu yin",
                            "guan bi lu yin",  "da kai qian lu",     "xian shi qian lu","da kai hou lu", "xian shi hou lu",  "zhua pai shi ping"};

#ifdef AISPEECH_AUDPROC_OPEN
static char szAlgMem[120*1024] = {0};
static wakeup_t *w = NULL;
#endif

static int wakeup_handler_cb(void *userdata, wakeup_status_t status, char *json, int bytes)
{
    AUDPROC_RESULT ipcMsgSnd;
    NVTIPC_I32 ipcErr;
    int i = 0;

    if(status != WAKEUP_STATUS_WOKEN){
        return 0;
    }
    ipcMsgSnd.result = 0;
    ipcMsgSnd.count  = 0;
    for(i=0; i<CMD_AISPEECH_NUM; i++){
        if(strstr(json, szCmdLine[i])){
            ipcMsgSnd.count = i;
            break;
        }
    }
    if(i == CMD_AISPEECH_NUM){
        return 0;
    }
    if(1)
	{
        ipcErr = NvtIPC_MsgSnd(msqid, NVTIPC_SENDTO_CORE1, &ipcMsgSnd, sizeof(ipcMsgSnd));
    }
    //AUDPROC_DBG("status:%d, str:%s\r\n", status, json);
    return 0;
} 


int AiSpeech_AudProc_Init(void)
{
    static int inited = 0;
    if(inited)return 0;
#ifdef AISPEECH_AUDPROC_OPEN

    char *env = NULL;
    void * baseMem = (void *)szAlgMem;
    w = wakeup_new(baseMem, NULL);
    wakeup_register_handler(w, w, wakeup_handler_cb);
    env = "words=wo yao pai zhao,zhua pai zhao pian,wo yao lu xiang,da kai ping mu,guan bi ping mu,da kai lu yin,guan bi lu yin,da kai re dian,guan bi re dian,da kai qian lu,xian shi qian lu,da kai hou lu,xian shi hou lu,zhua pai shi ping;thresh=0.26,0.09,0.24,0.22,0.23,0.24,0.17,0.24,0.16,0.24,0.22,0.26,0.27,0.11;";
    wakeup_start(w, env, strlen(env));
    inited = 1;
#endif
}

static void audproc_process(cyg_addrword_t arg)
{
    NVTIPC_I32 ipcErr;
    AUDPROC_DATA ipcMsgRcv;
	NVTIPC_U32 addr, size;
    int ret = -1;

    //AUDPROC_RESULT ipcMsgSnd;
	while (1) {
		ipcErr = NvtIPC_MsgRcv(msqid, &ipcMsgRcv, sizeof(ipcMsgRcv));
        addr = NvtIPC_GetCacheAddr(ipcMsgRcv.addr);
		size = ipcMsgRcv.size;
		NvtIPC_FlushCache(addr, ipcMsgRcv.size);
		//if((seq++%10) == 0)AUDPROC_DBG("Get AudOut Sample %x(%x)/%d (%d)<%p:%u>\r\n", ipcMsgRcv.addr, addr, ipcMsgRcv.size, ipcMsgRcv.count, pMemPool, uiMemPoolSize);

#ifdef AISPEECH_AUDPROC_OPEN
        ret = wakeup_feed(w, (char *)addr, size);
        if (ret == WAKEUP_STATUS_ERROR || ret == WAKEUP_STATUS_WOKEN_BOUNDARY)
        {
            AUDPROC_DBG("%s: ret=%d,size = %d,index=%d\n", __FUNCTION__, ret,size,index);
            continue;
        }
#else
        ////// add audio data processing related code here!!!
        //ipcMsgSnd.result = aiengine_feed(engine, addr, size);
        //ipcMsgSnd.count = 100;
        //ipcMsgSnd.result= 0;

        //if((seq++%10) == 0){
        //    ipcErr = NvtIPC_MsgSnd(msqid, NVTIPC_SENDTO_CORE1, &ipcMsgSnd, sizeof(ipcMsgSnd));
        //}
#endif

	}
//#ifdef AISPEECH_AUDPROC_OPEN
//    wakeup_end(w);
//    wakeup_delete(w);
//#endif
}

static int AudProc_Open(void)
{
	AUDPROC_PRINT("Open AudProc\n");

	if (is_audproc_opened) {
		AUDPROC_ERR("AudProc is already opened!\n");
		return -1;
	}

	if (msqid < 0) {
		msqid = NvtIPC_MsgGet(NvtIPC_Ftok("audproc"));
	}
    AiSpeech_AudProc_Init();
	cyg_thread_create(AUDPROC_THREAD_PRIORITY,
					audproc_process,
					0,
					"audproc_process",
					&audproc_stacks[0],
					sizeof(audproc_stacks),
					&audproc_thread,
					&audproc_thread_object
				);
	cyg_thread_resume(audproc_thread);
	is_audproc_opened = 1;

	return 0;
}

static int AudProc_Close(void)
{
	AUDPROC_PRINT("Close AudProc\n");

	if (is_audproc_opened == 0) {
		AUDPROC_ERR("AudProc is already closed!\n");
		return -1;
	}

	cyg_thread_suspend(audproc_thread);
	cyg_thread_delete(audproc_thread);

	if (msqid >= 0) {
		NvtIPC_MsgRel(msqid);
		msqid = -1;
	}
	is_audproc_opened = 0;

	return 0;
}

void AudProc_CmdLine(char *szCmd)
{
	char *delim = " ";
	char *pToken;
    char *ptr;
    unsigned int uiAuthInfo = 0;

	AUDPROC_PRINT("szCmd = %s\n", szCmd);

	pToken = strtok(szCmd, delim);
	while (pToken != NULL) {
		if (!strcmp(pToken, "-open")) {
			//get version number
			pToken = strtok(NULL, delim);
			if (NULL == pToken) {
				AUDPROC_ERR("get version number fail\n");
				break;
			}
			if (atoi(pToken) != AUDPROC_ECOS_VER) {
				AUDPROC_ERR("ver key mismatch %d %d\n", AUDPROC_ECOS_VER, atoi(pToken));
				break;
			}
            //get auth info 
            pToken = strtok(NULL, delim);
			if (NULL == pToken) {
				AUDPROC_ERR("get version number fail\n");
				break;
			}
            uiAuthInfo = atoi(pToken);
			if (uiAuthInfo == 0) {
				AUDPROC_ERR("the config info mismatch %d %d\n", AUDPROC_ECOS_VER, atoi(pToken));
				break;
			}
            pAuthInfo = (AISpeechAuthInfo *)uiAuthInfo;
			//open AudProc
            AudProc_Open();
			break;
		} else if (!strcmp(pToken, "-close")) {
			//close AudProc
            AudProc_Close();
			break;
		}
		pToken = strtok(NULL, delim);
	}
}

