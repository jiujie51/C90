//global debug level: PRJ_DBG_LVL
#include "PrjCfg.h"

#include "SysCfg.h"
#include "SysCommon.h"

#include "AppLib.h"

#if (UI_FUNC == ENABLE)
#include "UISetup.h"
#endif

#include "UIFramework.h"
#include "NVTUserCommand.h"
#include "nvtmpp.h"
#include "ProjectInfo.h"
#include "SysMain.h"
#include "FileDB.h"

#if (defined(_MODEL_DVCAM1_EVB_) || defined(_MODEL_DVCAM2_EVB_) || defined(_MODEL_DVCAM_64M_EVB_))
#include "UIAppMovie.h"
#endif

//local debug level: THIS_DBGLVL
#define THIS_DBGLVL         2 // 0=FATAL, 1=ERR, 2=WRN, 3=UNIT, 4=FUNC, 5=IND, 6=MSG, 7=VALUE, 8=USER
///////////////////////////////////////////////////////////////////////////////
#define __MODULE__          SysMainCmd
#define __DBGLVL__          ((THIS_DBGLVL>=PRJ_DBG_LVL)?THIS_DBGLVL:PRJ_DBG_LVL)
#define __DBGFLT__          "*" //*=All, [mark]=CustomClass
#include "DebugModule.h"
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// sys state
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "PStore.h"
#include "FileSysTsk.h"
//#include "DxPower.h"

CHAR sys_usage_str[17] = {0};
CHAR sys_usage_str2[17] = {0};
int sys_dump_cnt = 0;
BOOL sys_keeptop = FALSE;
BOOL sys_dotrace = FALSE;

void sys_usage_dump(BOOL bTrace)
{
	//fix for CID 62610,62611 - begin
	UINT32 dma_usage = 0;
	UINT32 cpu_usage = 0;
	//fix for CID 62610,62611 - end
	UINT32 mem_free, mem_total;
	#if (PST_FUNC == ENABLE)
	UINT32 stg_pst_free, stg_pst_total;
	#endif
	#if (FS_FUNC == ENABLE)
	UINT64 stg_fat_free, stg_fat_total;
	#endif
	//UINT32 pwr_lvl_usage;
	//UINT32 pwr_vot_usage;
	//UINT32 pwr_tmp_usage;
	profile_get(PROFILE_DMA, &dma_usage);
	if (dma_usage > 100) {
		dma_usage = 0;
	}
	profile_get(PROFILE_CPU, &cpu_usage);
	if (cpu_usage > 100) {
		cpu_usage = 0;
	}
	mem_free = SxCmd_GetTempMem(TEMPBUF_GET_MAXSIZE); //not release,it get size
	mem_total = OS_GetMemSize(MEM_TOTAL);
	#if (PST_FUNC == ENABLE)
	stg_pst_free = PStore_GetInfo(PS_INFO_FREE_SPACE);
	stg_pst_total = PStore_GetInfo(PS_INFO_TOT_SIZE);
	#endif
	#if (FS_FUNC == ENABLE)
	stg_fat_free = FileSys_GetDiskInfoEx('A', FST_INFO_FREE_SPACE);
	stg_fat_total = FileSys_GetDiskInfoEx('A', FST_INFO_DISK_SIZE);
	#endif
	//pwr_lvl_usage = DrvPower_GetControl(DRVPWR_CTRL_BATTERY_LEVEL); //level
	//if(pwr_lvl_usage>4)pwr_lvl_usage=0;
	//pwr_vot_usage = DrvPower_GetControl(DRVPWR_CTRL_BATTERY_ADC_VALUE); //votiage
	//pwr_tmp_usage = DrvPower_GetControl(DRVPWR_CTRL_BATTERY_TEMPERATURE); //temparature
	if (!bTrace) {
		profile_msg("resource |  total |  usage\r\n");
		profile_msg("=================================\r\n");
		profile_msg("     DMA |  %5u |  %5u (%%)\r\n", 100, dma_usage);
		profile_msg("     CPU |  %5u |  %5u (%%)\r\n", 100, cpu_usage);
		profile_msg("     MEM |  %5u |  %5u (MB)\r\n", (mem_total) / 1024 / 1024, (mem_total - mem_free) / 1024 / 1024);
		#if (PST_FUNC == ENABLE)
		profile_msg("   INMEM |  %5u |  %5u (MB)\r\n", (stg_pst_total) / 1024 / 1024, (stg_pst_total - stg_pst_free) / 1024 / 1024);
		#endif
		#if (FS_FUNC == ENABLE)
		profile_msg("   EXMEM |  %5u |  %5u (MB)\r\n", (UINT32)((stg_fat_total) / 1024 / 1024), (UINT32)((stg_fat_total - stg_fat_free) / 1024 / 1024));
		#endif
		//profile_msg("     PWR |  %5u |  %5u\r\n", 4, pwr_lvl_usage);
		//profile_msg("PWR_VOT: %u/%u\r\n", pwr_vot_usage, 1000);
		//profile_msg("PWR_TMP: %u/%u\r\n", pwr_tmp_usage, 100);
	} else {
		if (sys_dump_cnt == 0) {
			profile_msg("\r\n");
			profile_msg("             DMA             CPU             MEM           INMEM           EXMEM\r\n");
			profile_msg("================================================================================\r\n");
		} else {
			snprintf(sys_usage_str, sizeof(sys_usage_str), "%u/%u", dma_usage, 100);
			profile_msg("%16s", sys_usage_str);
			snprintf(sys_usage_str, sizeof(sys_usage_str), "%u/%u", cpu_usage, 100);
			profile_msg("%16s", sys_usage_str);
			snprintf(sys_usage_str, sizeof(sys_usage_str), "%u/%u", (mem_total - mem_free) / 1024 / 1024, (mem_total) / 1024 / 1024);
			profile_msg("%16s", sys_usage_str);
			#if (PST_FUNC == ENABLE)
			snprintf(sys_usage_str, sizeof(sys_usage_str), "%u/%u", (stg_pst_total - stg_pst_free) / 1024 / 1024, (stg_pst_total) / 1024 / 1024);
			profile_msg("%16s", sys_usage_str);
			#endif
			#if (FS_FUNC == ENABLE)
			snprintf(sys_usage_str, sizeof(sys_usage_str), "%u/%u", (UINT32)((stg_fat_total - stg_fat_free) / 1024 / 1024), (UINT32)((stg_fat_total) / 1024 / 1024));
			profile_msg("%16s\r\n", sys_usage_str);
			#endif
		}
	}
}

void sys_profile_process(void)
{
	//fix for CID 62612 - begin
	UINT32 mode = 0;
	//fix for CID 62612 - end
	profile_get(PROFILE_MODE, &mode);
	switch (mode) {
	case 1: { //"sys task" or "sys task top"
			if (sys_keeptop) { //"sys task top"
				profile_msg("\x1B[2J\x1B[H"); //cls
				profile_cmd(PROFILE_TASK, TRUE, 0);
			} else { //"sys task"
				if (sys_dump_cnt > 9) {
					profile_msg("\r\n");
					profile_cmd(PROFILE_TASK, TRUE, 0);
					SxCmd_DoCommand("tprof stop");
				} else {
					profile_cmd(PROFILE_TASK, FALSE, 0);
					profile_msg(".");
				}
			}
		}
		sys_dump_cnt++;
		break;
	case 2: //"sys cpu" or "sys cpu top" or "sys cpu trace"
		if (!sys_dotrace) { //"sys cpu" or "sys cpu top"
			if (sys_keeptop) { //"sys cpu top"
				profile_msg("\x1B[2J\x1B[H"); //cls
				profile_cmd(PROFILE_CPU, TRUE, 0);
			} else { //"sys cpu"
				if (sys_dump_cnt > 9) {
					profile_msg("\r\n");
					profile_cmd(PROFILE_CPU, TRUE, 0);
					SxCmd_DoCommand("tprof stop");
				} else {
					profile_cmd(PROFILE_CPU, FALSE, 0);
					profile_msg(".");
				}
			}
		} else { //"sys cpu trace"
			profile_cmd(PROFILE_CPU, TRUE, 0);
		}
		sys_dump_cnt++;
		break;
	case 3: //"sys usage" or "sys usage top" or "sys usage trace"
		if (!sys_dotrace) { //"sys usage" or "sys usage top"
			if (sys_keeptop) { //"sys cpu top"
				profile_msg("\x1B[2J\x1B[H"); //cls
				sys_usage_dump(FALSE);
			} else { //"sys cpu"
				if (sys_dump_cnt > 0) {
					profile_msg("\r\n");
					sys_usage_dump(FALSE);
					SxCmd_DoCommand("tprof stop");
				} else {
					profile_msg(".");
				}
			}
		} else { //"sys usage trace"
			sys_usage_dump(TRUE);
		}
		sys_dump_cnt++;
		break;
	}
}


///////////////////////////////////////////////////////////////////////////////
// cmd filter
///////////////////////////////////////////////////////////////////////////////

//FILTER for KEY EMULATION
BOOL User_CommandFilter(CHAR *pcCmdStr, int len)
{
	int slen = strlen(pcCmdStr);

	switch (*pcCmdStr) {
	// Key Macro
	case 'k':
		if (!strncmp(pcCmdStr, "koff", 4) && (slen == 4)) {
			strncpy(pcCmdStr, "key power", len);
			pcCmdStr[len] = 0;
		} else if (!strncmp(pcCmdStr, "kdel", 4) && (slen == 4)) {
			strncpy(pcCmdStr, "key delete", len);
			pcCmdStr[len] = 0;
		} else if (!strncmp(pcCmdStr, "km", 2) && (slen == 2)) {
			strncpy(pcCmdStr, "key menu", len);
			pcCmdStr[len] = 0;
		} else if (!strncmp(pcCmdStr, "kmd", 3) && (slen == 3)) {
			strncpy(pcCmdStr, "key mode", len);
			pcCmdStr[len] = 0;
		} else if (!strncmp(pcCmdStr, "kmp", 3) && (slen == 3)) {
			strncpy(pcCmdStr, "key playback", len);
			pcCmdStr[len] = 0;
		} else if (!strncmp(pcCmdStr, "kmv", 3) && (slen == 3)) {
			strncpy(pcCmdStr, "key movie", len);
			pcCmdStr[len] = 0;
		} else if (!strncmp(pcCmdStr, "ku", 2) && (slen == 2)) {
			strncpy(pcCmdStr, "key up", len);
			pcCmdStr[len] = 0;
		} else if (!strncmp(pcCmdStr, "kd", 2) && (slen == 2)) {
			strncpy(pcCmdStr, "key down", len);
			pcCmdStr[len] = 0;
		} else if (!strncmp(pcCmdStr, "kr", 2) && (slen == 2)) {
			strncpy(pcCmdStr, "key right", len);
			pcCmdStr[len] = 0;
		} else if (!strncmp(pcCmdStr, "kl", 2) && (slen == 2)) {
			strncpy(pcCmdStr, "key left", len);
			pcCmdStr[len] = 0;
		} else if (!strncmp(pcCmdStr, "ke", 2) && (slen == 2)) {
			strncpy(pcCmdStr, "key enter", len);
			pcCmdStr[len] = 0;
		} else if (!strncmp(pcCmdStr, "kzi", 3) && (slen == 3)) {
			strncpy(pcCmdStr, "key zoomin", len);
			pcCmdStr[len] = 0;
		} else if (!strncmp(pcCmdStr, "kzo", 3) && (slen == 3)) {
			strncpy(pcCmdStr, "key zoomout", len);
			pcCmdStr[len] = 0;
		} else if (!strncmp(pcCmdStr, "kzr", 3) && (slen == 3)) {
			strncpy(pcCmdStr, "key zoom release", len);
			pcCmdStr[len] = 0;
		} else if (!strncmp(pcCmdStr, "ks1", 3) && (slen == 3)) {
			strncpy(pcCmdStr, "key shutter1", len);
			pcCmdStr[len] = 0;
		} else if (!strncmp(pcCmdStr, "ks2", 3) && (slen == 3)) {
			strncpy(pcCmdStr, "key shutter2", len);
			pcCmdStr[len] = 0;
		} else if (!strncmp(pcCmdStr, "kc1", 3) && (slen == 3)) {
			strncpy(pcCmdStr, "key custom1", len);
			pcCmdStr[len] = 0;
		}
		if (!strncmp(pcCmdStr, "kn", 2) && (slen == 2)) {
			strncpy(pcCmdStr, "key next", len);
			pcCmdStr[len] = 0;
		}
		//#NT#2016/03/16#Niven Cho -begin
		//#NT#add key previous
		else if (!strncmp(pcCmdStr, "kp", 2) && (slen == 2)) {
			strncpy(pcCmdStr, "key prev", len);
			pcCmdStr[len] = 0;
		}
		//#NT#2016/03/16#Niven Cho -end
		else if (!strncmp(pcCmdStr, "ks", 2) && (slen == 2)) {
			strncpy(pcCmdStr, "key select", len);
			pcCmdStr[len] = 0;
		}
		if (!strncmp(pcCmdStr, "ki", 2) && (slen == 2)) {
			strncpy(pcCmdStr, "key i", len);
			pcCmdStr[len] = 0;
		}
		break;
	case 'x':
		if (!strncmp(pcCmdStr, "x", 1) && (slen == 1)) {
			if (sys_keeptop || sys_dotrace) {
				strncpy(pcCmdStr, "tprof stop", len);
				pcCmdStr[len] = 0;
			}
		}
		break;
	}
	return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
// sys cmd
///////////////////////////////////////////////////////////////////////////////

static BOOL Cmd_sys_mem(CHAR *strCmd)
{
#if 1
	char str1[32];

	sscanf_s(strCmd, "%s", str1, sizeof(str1)); //fix for CID 44998

	if (strncmp(str1, "su", 31) == 0) { //sort by unit
		DBG_DUMP("[System]\r\n");
		Install_SystemMEM();
		DBG_DUMP("\r\n");
	} else { //sort by addr
		SxCmd_DoCommand("ker dumpmem");
	}
	{
		nvtmpp_dump_mem_range(debug_msg);
	}

	if (1) {
		DBG_DUMP("[Summary]\r\n");
		DBG_DUMP("=============================================================\r\n");
		DBG_DUMP("^Y   Total     : Range[0x%08X~0x%08X] Size=0x%08X\r\n",
				 OS_GetMemAddr(MEM_TOTAL),
				 OS_GetMemAddr(MEM_TOTAL) + OS_GetMemSize(MEM_TOTAL),
				 OS_GetMemSize(MEM_TOTAL)
				);
	}
#endif
	return TRUE;
}
static BOOL Cmd_sys_boot(CHAR *strCmd)
{
	DBG_DUMP("Boot\r\n");
	//DscMain();
	// Start Dsc System
	//System_PowerOn(SYS_POWERON_NORMAL);
	GxSystem_PowerOn(); //boot start
	return TRUE;
}
static BOOL Cmd_sys_shutdown(CHAR *strCmd)
{
	DBG_DUMP("Shutdown\r\n");
	//System_PowerOff(SYS_POWEROFF_NORMAL);
	Ux_SendEvent(0, NVTEVT_SYSTEM_SHUTDOWN, 1, 0); //shutdown start
	return TRUE;
}
static BOOL Cmd_sys_sleep(CHAR *strCmd)
{
	DBG_DUMP("Sleep\r\n");
	return TRUE;
}
static BOOL Cmd_sys_wakeup(CHAR *strCmd)
{
	DBG_DUMP("Wakeup\r\n");
	return TRUE;
}

extern UINT32 UI_EventTraceLvl;

static BOOL Cmd_sys_event(CHAR *strCmd)
{
	UINT32 lvl = 0;

	sscanf_s(strCmd, "%d", &lvl);
	if (lvl > 5) {
		lvl = 5;
	}

	UI_EventTraceLvl = lvl;
	DBG_DUMP("event trace level = %d!\r\n", UI_EventTraceLvl);
	return TRUE;
}

static BOOL Cmd_sys_tm(CHAR *strCmd)
{
	char str1[32];

	sscanf_s(strCmd, "%s", str1, sizeof(str1)); //fix for CID 45000

	if (strncmp(str1, "boot", 31) == 0) {
		DBG_MSG("Dump TM results of Boot:\r\n");
		TM_Dump("*");
	}
	return TRUE;
}

static BOOL Cmd_sys_usage(CHAR *strCmd)
{
	char str1[32];

	sscanf_s(strCmd, "%s", str1, sizeof(str1));

	sys_keeptop = FALSE;
	sys_dotrace = FALSE;
	sys_dump_cnt = 0;
	if (strncmp(str1, "top", 31) == 0) {
		sys_keeptop = TRUE;
	}
	if (strncmp(str1, "trace", 31) == 0) {
		sys_dotrace = TRUE;
	}
	SxCmd_DoCommand("tprof start 3");
	return TRUE;
}

static BOOL Cmd_sys_cpu(CHAR *strCmd)
{
	char str1[32];

	sscanf_s(strCmd, "%s", str1, sizeof(str1));

	sys_keeptop = FALSE;
	sys_dotrace = FALSE;
	sys_dump_cnt = 0;
	if (strncmp(str1, "top", 31) == 0) {
		sys_keeptop = TRUE;
	}
	if (strncmp(str1, "trace", 31) == 0) {
		//sys_dotrace = TRUE;
		return TRUE; //not support
	}
	SxCmd_DoCommand("tprof start 2");
	return TRUE;
}

static BOOL Cmd_sys_task(CHAR *strCmd)
{
	char str1[32];

	sscanf_s(strCmd, "%s", str1, sizeof(str1));

	sys_keeptop = FALSE;
	sys_dotrace = FALSE;
	sys_dump_cnt = 0;
	if (strncmp(str1, "top", 31) == 0) {
		sys_keeptop = TRUE;
	}
	if (strncmp(str1, "trace", 31) == 0) {
		return TRUE; //not support
	}
	SxCmd_DoCommand("tprof start 1");
	return TRUE;
}
//#NT#2016/06/23#Niven Cho -begin
//#NT#Enter calibration by cgi event or command event
static BOOL Cmd_sys_calibration(CHAR *strCmd)
{
#if (CALIBRATION_FUNC == ENABLE)
	Ux_PostEvent(NVTEVT_KEY_CALIBRATION, 0);
#else
	DBG_ERR("CALIBRATION_FUNC must be ENABLE.\r\n");
#endif
	return TRUE;
}
//#NT#2016/06/23#Niven Cho -end
BOOL Cmd_sys_reset(CHAR *strCmd)
{
#if (UI_FUNC == ENABLE)
	Ux_SendEvent(0, NVTEVT_EXE_SYSRESET, 0);
#endif
	return TRUE;
}

BOOL Cmd_sys_gitver(CHAR *strCmd)
{
	DBG_DUMP("GitVer=%s\r\n",Prj_GetGitRevision());
	return TRUE;
}

static BOOL Cmd_sys_evtfps(CHAR *strCmd)
{
	UINT32 bEn =0;
	UINT32 unit =0;
	sscanf_s(strCmd, "%d %d", &bEn,&unit);

	//consume fps
	UserCalConsumeFps(bEn,unit);
	//post fps
	Ux_CalPostFps(bEn,unit);
	return TRUE;

}

SXCMD_BEGIN(sys, "system command")
SXCMD_ITEM("usage %", Cmd_sys_usage, "usage")
SXCMD_ITEM("cpu %", Cmd_sys_cpu, "cpu")
SXCMD_ITEM("task %", Cmd_sys_task, "task")
SXCMD_ITEM("mem %", Cmd_sys_mem, "mem")
SXCMD_ITEM("boot", Cmd_sys_boot, "boot")
SXCMD_ITEM("shutdown", Cmd_sys_shutdown, "shutdown")
SXCMD_ITEM("sleep", Cmd_sys_sleep, "sleep")
SXCMD_ITEM("wakeup", Cmd_sys_wakeup, "wakeup")
SXCMD_ITEM("reset", Cmd_sys_reset, "reset to default setting")
SXCMD_ITEM("event %", Cmd_sys_event, "event trace level")
SXCMD_ITEM("tm %", Cmd_sys_tm, "time mark")
//#NT#2016/06/23#Niven Cho -begin
//#NT#Enter calibration by cgi event or command event
SXCMD_ITEM("calibration", Cmd_sys_calibration, "enter calibration window")
//#NT#2016/06/23#Niven Cho -end
SXCMD_ITEM("gitver", Cmd_sys_gitver, "get git fw version")
SXCMD_ITEM("evtfps %", Cmd_sys_evtfps, "cal evt consume fps")
SXCMD_END()



///////////////////////////////////////////////////////////////////////////////
// temp memory for user cmd
///////////////////////////////////////////////////////////////////////////////
#define      MAX_BUF_NUM        1

UINT32 gBufNum = 0;

UINT32 User_GetTempBuffer(UINT32 uiSize)
{
	NVTMPP_VB_POOL pool;
	NVTMPP_VB_BLK  blk;
	UINT32         blk_size = uiSize, addr;
    CHAR           func_name[]= "UsrTmp";

	if (uiSize == TEMPBUF_GET_MAXSIZE) {
		return POOL_SIZE_APP;    //return max size
	}
    if(gBufNum>=MAX_BUF_NUM) {
    	DBG_ERR("over MAX num %d,need release buf\r\n",MAX_BUF_NUM);
        return 0;
    }
    pool = nvtmpp_vb_create_pool(func_name, blk_size, 1, NVTMPP_DDR_1);

    if (NVTMPP_VB_INVALID_POOL == pool) {
    	DBG_ERR("create private pool err\r\n");
    	return 0;
    }

    blk = nvtmpp_vb_get_block(0, pool, blk_size, NVTMPP_DDR_1);
	if (NVTMPP_VB_INVALID_BLK == blk) {
		DBG_ERR("get vb block err\r\n");
		return 0;
	}
	addr = nvtmpp_vb_block2addr(blk);
	DBG_IND("%s get noncache buf addr = 0x%08X, size=0x%08X\r\n", func_name, addr, blk_size);

    if(addr) {
        gBufNum++;
    }
	return (UINT32)addr;
}

UINT32 User_RelTempBuffer(UINT32 addr)
{
	NVTMPP_VB_POOL pool;

    pool = nvtmpp_vb_addr2pool(addr);
	if (pool == NVTMPP_VB_INVALID_POOL){
		DBG_ERR("invalid block addr 0x%08X\r\n",addr);
        return FALSE;
	} else {
        gBufNum--;
		return nvtmpp_vb_destroy_pool(pool);
	}

}

//#NT#2016/03/18#Niven Cho -begin
//#NT#improve the using of gprof
#if defined(_GPROF_TOOL_ON_)
_ALIGNED(4) static UINT8 m_gProfBuf[0x100000];
UINT32 System_GetGprofBuffer(UINT32 uiSize)
{
	void *pBuf = 0;
	if (uiSize == TEMPBUF_GET_MAXSIZE) {
		return 0x100000;    //return max size
	}
	if (uiSize <= sizeof(m_gProfBuf)) {
		pBuf = (void *)m_gProfBuf;
	}
	if (pBuf == NULL) {
		DBG_ERR("get gprof buffer fail\r\n");
	}
	return (UINT32)pBuf;
}
#endif
//#NT#2016/03/18#Niven Cho -end


///////////////////////////////////////////////////////////////////////////////
// user cmd
///////////////////////////////////////////////////////////////////////////////

static BOOL Cmd_user_cmd(CHAR *strCmd)
{
	DBG_DUMP("this is sample cmd!\r\n");
	return TRUE;
}

//#NT#2016/03/03#Ben Wang -begin
//#NT#Add debug message for script test.
BOOL Cmd_dbg(CHAR *strCmd)
{
	UINT32 ShowMsg = 0;
	sscanf_s(strCmd, "%d", &ShowMsg);
	g_bDbgTest = ShowMsg;
	return TRUE;
}


static BOOL Cmd_user_movie_mode_d2d_func_set(CHAR *strCmd)
{
#if (defined(_MODEL_DVCAM1_EVB_) || defined(_MODEL_DVCAM2_EVB_) || defined(_MODEL_DVCAM_64M_EVB_))
	UINT32 uiEnable = 0;

	sscanf_s(strCmd, "%d", &uiEnable);

	DBGD(uiEnable);

	MovieExe_SetMovieD2DModeEn(uiEnable);
	Ux_PostEvent(NVTEVT_SYSTEM_MODE, 1, System_GetState(SYS_STATE_CURRMODE)); // Current mode should be Movie or Photo mode.
#endif

	return TRUE;
}

static BOOL Cmd_user_test_write_b(CHAR *strCmd)
{
	static UINT32 SRC[10] = {0,1,2,3,4,5,6,7,8,9};
	static UINT32 DST[10] = {0};

	int i;
    FST_FILE    pFile;
	UINT32 uiReadSize = sizeof(SRC);
    pFile = FileSys_OpenFile("B:\\test", FST_OPEN_WRITE|FST_OPEN_ALWAYS);
	FileSys_WriteFile(pFile, (UINT8 *)SRC, &uiReadSize, 0, NULL);
    FileSys_CloseFile(pFile);


	pFile = FileSys_OpenFile("B:\\test", FST_OPEN_READ);
	FileSys_ReadFile(pFile, (UINT8 *)DST, &uiReadSize, 0, NULL);
    FileSys_CloseFile(pFile);

	for (i = 0; i < 10; i++) {
		debug_msg("%d\r\n",DST[i]);
	}

	return TRUE;
}

#define COPY2_CARD_BUFF_SIZE ((1 * 1024 * 1024) + POOL_SIZE_FILEDB)
#define COPY2_CARD_BLOCK_UNIT (COPY2_CARD_BUFF_SIZE - POOL_SIZE_FILEDB)
static UINT32 g_uiTempBufAddr = 0, g_uiTempBufSize = 0;
static BOOL Cmd_user_CopyEmmcFile2SD(CHAR* dst, CHAR*src)
{
	FST_FILE  filehdlA = NULL, filehdlB = NULL;
	UINT32 len = COPY2_CARD_BLOCK_UNIT;
	

    // 1. create new file
    filehdlA = FileSys_OpenFile(dst, FST_OPEN_ALWAYS|FST_OPEN_WRITE);
    if (!filehdlA)
    {
        DBG_ERR("Open file %s fail\r\n",dst);
        return FALSE;
    }

    filehdlB = FileSys_OpenFile(src, FST_OPEN_READ);
    if (!filehdlB)
    {
        DBG_ERR("Open file %s fail\r\n", src);
        return FALSE;
    }


    do {
        if (FileSys_ReadFile(filehdlB, (UINT8*)g_uiTempBufAddr, &len, 0, NULL)!=FST_STA_OK)
        {
			FileSys_CloseFile(filehdlB);
			FileSys_CloseFile(filehdlA);
			DBG_ERR("ReadFile file fail\r\n");

			return FALSE;
        }
        if (FileSys_WriteFile(filehdlA, (UINT8*)g_uiTempBufAddr, &len, 0, NULL)!= FST_STA_OK)
        {
			FileSys_CloseFile(filehdlB);
			FileSys_CloseFile(filehdlA);
			DBG_ERR("WriteFile file fail\r\n");

			return FALSE;
        }

        // EOF
        if (len != COPY2_CARD_BLOCK_UNIT) {
			DBG_DUMP("copy done!\r\n");
            break;
        }
    } while (1);

    FileSys_CloseFile(filehdlB);
    FileSys_CloseFile(filehdlA);

    return TRUE;
}

BOOL Cmd_user_readDriveBFile(CHAR* strCmd)
{
	CHAR DstFilename[64] = "A:\\test.bin";
	UINT32 i, CurrIndex, FileNumber;
	PFILEDB_FILE_ATTR  pFileAttr;
	FILEDB_HANDLE fileDbHandle;
	UINT32 fileDbbufSize = POOL_SIZE_FILEDB, fileDbbufAddr;

	g_uiTempBufAddr = SxCmd_GetTempMem(COPY2_CARD_BUFF_SIZE);
	g_uiTempBufSize = COPY2_CARD_BUFF_SIZE;

	// reserved POOL_SIZE_FILEDB for FileDB using
#if 1// USE_FILEDB
	g_uiTempBufSize -= fileDbbufSize;
	fileDbbufAddr = g_uiTempBufAddr + g_uiTempBufSize;
#endif

	//  create fileDB for copy
	{
		FILEDB_INIT_OBJ   FilDBInitObj = {0};
		CHAR              rootPath[20] = "B:\\";

		FilDBInitObj.rootPath = rootPath;
		FilDBInitObj.bIsRecursive = TRUE;
		FilDBInitObj.bIsCyclic = TRUE;
		FilDBInitObj.bIsMoveToLastFile = TRUE;
		FilDBInitObj.bIsSupportLongName = TRUE;
		FilDBInitObj.bIsDCFFileOnly = FALSE;
		FilDBInitObj.bIsChkHasFile = FALSE;
		FilDBInitObj.u32MaxFilePathLen = 60;
		FilDBInitObj.u32MaxFileNum = 10000;
		FilDBInitObj.fileFilter = FILEDB_FMT_JPG | FILEDB_FMT_AVI | FILEDB_FMT_MOV | FILEDB_FMT_MP4 | FILEDB_FMT_TS;
		FilDBInitObj.u32MemAddr = fileDbbufAddr;
		FilDBInitObj.u32MemSize = fileDbbufSize;
		fileDbHandle = FileDB_Create(&FilDBInitObj);
	}

	CurrIndex = FileDB_GetCurrFileIndex(fileDbHandle);
	FileNumber = FileDB_GetTotalFileNum(fileDbHandle);
	DBG_DUMP("starting File From Drive B to Drive A .....\r\n");
	DBGD(FileNumber);
	for (i=0; i < FileNumber; i++)
	{
		pFileAttr = FileDB_SearhFile(fileDbHandle, i);
		strcpy(DstFilename,pFileAttr->filePath);
		DstFilename[0]='A';
		if (Cmd_user_CopyEmmcFile2SD(DstFilename, pFileAttr->filePath) == FALSE)
		{
		   DBG_DUMP("copy FileName %s failed.....\r\n", pFileAttr->filePath);
		} else {
			DBG_DUMP("copy FileName %s ok.....\r\n", pFileAttr->filePath);
			#if 1
			if(pFileAttr->attrib & FST_ATTRIB_READONLY) {
				INT32 ret;
				
				DBG_DUMP("RO file\r\n");
				ret = FileSys_SetAttrib(DstFilename, FST_ATTRIB_READONLY, TRUE);
				if (ret != FST_STA_OK) {
					DBG_WRN("set RO fail!\r\n");
				}
			}
			#endif
		}
	}

	FileDB_Release(fileDbHandle);
	SxCmd_RelTempMem(g_uiTempBufAddr);

    return TRUE;
}

#if PMOVIE_BSQINCARD_FUNC
#include "ImageApp_MovieMulti.h"
static BOOL Cmd_CardEMR_cmd(CHAR *strCmd)
{
 UINT32 ui;

    sscanf_s(strCmd, "%d", &ui);
    if (!ui)
        ImageApp_MovieMulti_TrigOnCardEMR(_CFG_REC_ID_1);
    else
        ImageApp_MovieMulti_StopOnCardEMR(_CFG_REC_ID_1);

    return TRUE;
}
#endif
SXCMD_BEGIN(user, "user command")
SXCMD_ITEM("cmd", Cmd_user_cmd, "sample code")
SXCMD_ITEM("dbg %", Cmd_dbg, "show/hide DBG_TEST() message")
SXCMD_ITEM("movie_mode_d2d_func_set %", Cmd_user_movie_mode_d2d_func_set, "enable d2d func")
SXCMD_ITEM("writeb", Cmd_user_test_write_b, "test write B:\\")
SXCMD_ITEM("readb", Cmd_user_readDriveBFile, "read file from Drive B and write it to Drive A")

#if PMOVIE_BSQINCARD_FUNC
SXCMD_ITEM("cardemr %", Cmd_CardEMR_cmd, "enable/disable card emr")
#endif
SXCMD_END()
//#NT#2016/03/03#Ben Wang -end
