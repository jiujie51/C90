#include <stdio.h>
#include <string.h>
#include "kernel.h"
#include "SysCfg.h"
#include "Utility.h"
#include "FileSysTsk.h"
#include "PStore.h"
#include "SysCommon.h"
#include "GxSound.h"
#include "SxCmd.h"
#include "WavPlay.h"
#include "AACD_API.h"

#define __MODULE__				WavPlay
#define __DBGLVL__				1 // 0=OFF, 1=ERROR, 2=TRACE
#define __DBGFLT__				"*" //*=All, [mark]=CustomClass
#include "DebugModule.h"

#if (WAV_PLAY_FUNC == ENABLE)

#define PS_WAV_MERGE_DATA		"WAVMERGE"	// cannot over 12 characters
#define WAV_DATA_MAX			150			// max 150 wav files
#define AAC_BUF_SIZE			(80*1024)	// 80KB for AAC data
#define WAV_IN_USER_PARTITION	ENABLE

#if (USER_PACK_UPDATE == DISABLE)
#undef WAV_IN_USER_PARTITION
#define WAV_IN_USER_PARTITION	DISABLE
#endif

#if (WAV_IN_USER_PARTITION == DISABLE)
#include "WavData.c"
#endif

static UINT32  uWavPlayCtrlStatus = WAVPLAYCTRL_IDEL;
WAV_PLAY_CTRL_DATA  uWavPlayCtrlData[MAX_WAV_PLAY_BUFFER];

static UINT32		g_WavHeaderAddr = 0;
static SOUND_DATA	g_SoundData; // it needs to use global variable for GxSound

ER WavPlay_DecodeAAC(UINT32 inAddr, UINT32 outAddr, UINT32 inLen, UINT32 *pOutLen, ST_AUDIO_AACDEC_CFG *pAACDConfig)
{
	UINT32		uiBsLen;		// AAC bs length
	UINT32		uiRemainBsLen;	// remaining AAC bs length
	UINT32		uiBsAddr;		// current AAC bs address
	UINT32		uiRawAddr;		// current raw output address
	UINT32		uiAACFrameLen;	// AAC frame length
	UINT32		uiAACFrameNum;	// AAC frame number
	EN_AUDIO_ERROR_CODE		ErrorCode = E_OK;	// error code
	ST_AUDIO_AACD_BUFINFO	AACDBuf;	// AAC decoder buffer info
	ST_AUDIO_AACD_RTNINFO	AACDRtn;	// AAC decoder return info

	if ((ErrorCode = AACD_Init(pAACDConfig)) != EN_AUDIO_ERROR_NONE)
	{
		DBG_ERR("AAC decoder init error %d!\r\n", ErrorCode);
		return E_SYS;
	}

	// init buffer address
	uiBsLen = inLen;
	uiBsAddr = inAddr;
	uiRawAddr = outAddr;
	uiRemainBsLen = uiBsLen;
	uiAACFrameNum = 0;

	while (uiBsAddr < (inAddr + uiBsLen))
	{
		AACDBuf.pu32InBufferAddr	= (unsigned int *)uiBsAddr;
		AACDBuf.pu32OutBufferAddr	= (unsigned int *)uiRawAddr;
		AACDBuf.u32AvailBytes		= uiRemainBsLen;

		if ((ErrorCode = AACD_DecodeOneFrame(&AACDBuf, &AACDRtn)) != EN_AUDIO_ERROR_NONE)
		{
			DBG_ERR("AAC decode error %d!\r\n", ErrorCode);
			return E_SYS;
		}

		uiAACFrameLen = AACDRtn.u32OneFrameConsumeBytes;
		uiBsAddr += uiAACFrameLen;
		uiRawAddr += AACDRtn.u32DecodeOutSamples * AACDRtn.u32nChans * 2; // 2 bytes per sample
		uiRemainBsLen -= uiAACFrameLen;

		uiAACFrameNum++;
	}

	*pOutLen = uiRawAddr - outAddr;

    return E_OK;
}

#if WAV_IN_USER_PARTITION
ER WavPlay_WriteWavData(void)
{
	#define WAV_READ_BUF_SIZE   0x400000 // 4MB

	FST_FILE	pFile;
	char		*pFileName = "A:\\WavMerge.bin";
	UINT32		uiFileSize;
	UINT32		pBuf;

	pBuf = SxCmd_GetTempMem(WAV_READ_BUF_SIZE);

	if (pBuf == 0)
	{
		DBG_ERR("No enough buffer for sound data reading!\r\n");
		return E_SYS;
	}

	// open wav merge file (WavMerge.bin)
	pFile = FileSys_OpenFile(pFileName, FST_OPEN_READ);
	uiFileSize = WAV_READ_BUF_SIZE; // max reading size
	if (pFile)
	{
		FileSys_ReadFile(pFile, (UINT8 *)pBuf, &uiFileSize, 0, NULL);
		FileSys_CloseFile(pFile);
		DBG_MSG("File %s, size %d\r\n", pFileName, uiFileSize);

		if (uiFileSize == WAV_READ_BUF_SIZE)
		{
			DBG_ERR("Read buffer is not enough!\r\n");
			SxCmd_RelTempMem(pBuf);
			return E_SYS;
		}
	}
	else
	{
		DBG_ERR("Open %s for reading failed!\r\n", pFileName);
		SxCmd_RelTempMem(pBuf);
		return E_SYS;
	}

	// write merged wav data to user partition
	if (UserPartition_Write((INT8 *)pBuf, 0, uiFileSize, USR_PARTI_WAV_DATA) != E_OK)
	{
		DBG_ERR("write wav data to USR_PARTI_WAV_DATA failed!\r\n");
		SxCmd_RelTempMem(pBuf);
		return E_SYS;
	}

	SxCmd_RelTempMem(pBuf);

	return E_OK;
}

ER WavPlay_Init(void)
{
	UINT32	uiSize;
	UINT32	uiPoolSize;
	static BOOL	g_WavPlayInit = FALSE;

	if (g_WavPlayInit == FALSE)
	{
		g_WavHeaderAddr = OS_GetMempoolAddr(POOL_ID_WAV_PLAY_HEADER);
		uiPoolSize = OS_GetMempoolSize(POOL_ID_WAV_PLAY_HEADER);

	    uiSize = sizeof(WAV_MERGE_DATA) * WAV_DATA_MAX;
	    DBG_IND("wav header size = %d\r\n", uiSize);

		if (uiSize > uiPoolSize)
		{
			DBG_ERR("read wav header error, buffer not enough!\r\n");
			return E_SYS;
		}

		if (UserPartition_Read((INT8 *)g_WavHeaderAddr, 0, uiSize, USR_PARTI_WAV_DATA) != E_OK)
		{
			DBG_ERR("read wav header from USR_PARTI_WAV_DATA failed!\r\n");
			return E_SYS;
		}

		g_WavPlayInit = TRUE;
	}

	return E_OK;
}
#endif

ER WavPlay_PlayData(char *pName, UINT32 isWav)
{
	UINT32			i;
	UINT32			pBuf;
	WAV_MERGE_DATA	*pWavData;

	if (GxSound_IsPlaying())
	{
		DBG_ERR("sound is playing, cannot play now!\r\n");
		return E_SYS;
	}

#if WAV_IN_USER_PARTITION

	if (WavPlay_Init() != E_OK)
	{
		return E_SYS;
	}

#else

	if (isWav == FALSE) // AAC data
	{
		g_WavHeaderAddr = (UINT32)uiAACData;
	}
	else
	{
		g_WavHeaderAddr = (UINT32)uiWavData;
	}

#endif

	// compare wav file name
	i = 0;
	while (i < WAV_DATA_MAX)
	{
		pWavData = (WAV_MERGE_DATA *)(g_WavHeaderAddr + sizeof(WAV_MERGE_DATA) * i);
		if (strcmp(pWavData->FileName, pName) == 0)
		{
			DBG_IND("Find name %s, addr 0x%x, size 0x%x\r\n", pWavData->FileName, pWavData->uiAddr, pWavData->uiSize);
			break;
		}
		i++;
	}

	if (i == WAV_DATA_MAX)
	{
		DBG_ERR("No %s found!\r\n", pName);
		return E_SYS;
	}

#if WAV_IN_USER_PARTITION

	UINT32	uiPoolSize;

	pBuf = OS_GetMempoolAddr(POOL_ID_WAV_PLAY_DATA);
	uiPoolSize = OS_GetMempoolSize(POOL_ID_WAV_PLAY_DATA);

	if (pWavData->uiSize > uiPoolSize)
	{
		DBG_ERR("read wav data error, buffer not enough!\r\n");
		return E_SYS;
	}

	if (isWav == FALSE) // AAC data
	{
		pBuf = pBuf + uiPoolSize - AAC_BUF_SIZE;
	}

	if (UserPartition_Read((INT8 *)pBuf, pWavData->uiAddr, pWavData->uiSize, USR_PARTI_WAV_DATA) != E_OK)
	{
		DBG_ERR("read wav data from USR_PARTI_WAV_DATA failed!\r\n");
		return E_SYS;
	}

#else

	pBuf = g_WavHeaderAddr + pWavData->uiAddr;

#endif

	if (isWav == FALSE) // AAC data
	{
		ER		err;
		UINT32	PCMBuf;
		UINT32	PCMLen;
		ST_AUDIO_AACDEC_CFG 	AACDConfig; // AAC decoder config

		AACDConfig.enSampleRate = EN_AUDIO_SAMPLING_RATE_16000;
		AACDConfig.u32nChannels = 1;
		AACDConfig.enCodingType = EN_AUDIO_CODING_TYPE_ENABLE;

		PCMBuf = OS_GetMempoolAddr(POOL_ID_WAV_PLAY_DATA);
		err = WavPlay_DecodeAAC(pBuf, PCMBuf, pWavData->uiSize, &PCMLen, &AACDConfig);

		if (err == E_OK)
		{
			// play wav data
			g_SoundData.puiData = (const UINT8 *)PCMBuf;
			g_SoundData.uiSize = PCMLen;
			g_SoundData.sampleRate = 16000;
			g_SoundData.isMono = TRUE;
			g_SoundData.soundId = 0xFFFFFFFF;
			err = GxSound_ActOnSndNotInTbl(SOUND_PLAY_START, &g_SoundData, TRUE);
		}

		return err;
	}
	else
	{
		// play wav data
		g_SoundData.puiData = (const UINT8 *)pBuf;
		GxSound_ActOnSndNotInTbl(SOUND_PLAY_START, &g_SoundData, FALSE);
	}

	return E_OK;
}


ER WavPlay_PlayWavData(char *pName)
{
	return WavPlay_PlayData(pName, TRUE);
}

ER WavPlay_PlayAACData(char *pName)
{
	return WavPlay_PlayData(pName, FALSE);
}

ER WavPlay_PlayWavDataFromFile(char *pFileName)
{
	FST_FILE	pFile;
	UINT32		uiFileSize;
	UINT32		pBuf;
	UINT32		uiPoolSize;

	pBuf = OS_GetMempoolAddr(POOL_ID_WAV_PLAY_DATA);
	uiPoolSize = OS_GetMempoolSize(POOL_ID_WAV_PLAY_DATA);

	// open wav file
	pFile = FileSys_OpenFile(pFileName, FST_OPEN_READ);
	uiFileSize = uiPoolSize; // max reading size
	if (pFile)
	{
		FileSys_ReadFile(pFile, (UINT8 *)pBuf, &uiFileSize, 0, NULL);
		FileSys_CloseFile(pFile);
		DBG_DUMP("File %s, size %d\r\n", pFileName, uiFileSize);

		if (uiFileSize == uiPoolSize)
		{
			DBG_ERR("Read buffer is not enough!\r\n");
			return E_SYS;
		}
	}
	else
	{
		DBG_ERR("Open %s for reading failed!\r\n", pFileName);
		return E_SYS;
	}

	if (GxSound_IsPlaying())
	{
		DBG_ERR("sound is playing, cannot play now!\r\n");
		return E_SYS;
	}

	// play wav data
	g_SoundData.puiData = (const UINT8 *)pBuf;
	GxSound_ActOnSndNotInTbl(SOUND_PLAY_START, &g_SoundData, FALSE);

    return E_OK;
}

ER WavPlay_PlayAACDataFromFile(char *pFileName)
{
	ER			err;
	FST_FILE	pFile;
	UINT32		uiFileSize;
	UINT32		uiPoolSize;
	UINT32		PCMBuf, AACBuf;
	UINT32		PCMLen;
	ST_AUDIO_AACDEC_CFG		AACDConfig; // AAC decoder config

	PCMBuf = OS_GetMempoolAddr(POOL_ID_WAV_PLAY_DATA);
	AACBuf = PCMBuf + OS_GetMempoolSize(POOL_ID_WAV_PLAY_DATA) - AAC_BUF_SIZE;
	uiPoolSize = AAC_BUF_SIZE;

	// open wav file
	pFile = FileSys_OpenFile(pFileName, FST_OPEN_READ);
	uiFileSize = uiPoolSize; // max reading size
	if (pFile)
	{
		FileSys_ReadFile(pFile, (UINT8 *)AACBuf, &uiFileSize, 0, NULL);
		FileSys_CloseFile(pFile);
		DBG_DUMP("File %s, size %d\r\n", pFileName, uiFileSize);

		if (uiFileSize == uiPoolSize)
		{
			DBG_ERR("Read buffer is not enough!\r\n");
			return E_SYS;
		}
	}
	else
	{
		DBG_ERR("Open %s for reading failed!\r\n", pFileName);
		return E_SYS;
	}

	// decode AAC
	AACDConfig.enSampleRate = EN_AUDIO_SAMPLING_RATE_16000;
	AACDConfig.u32nChannels = 1;
	AACDConfig.enCodingType = EN_AUDIO_CODING_TYPE_ENABLE;

	err = WavPlay_DecodeAAC(AACBuf, PCMBuf, uiFileSize, &PCMLen, &AACDConfig);

	if (err == E_OK)
	{
		if (GxSound_IsPlaying())
		{
			DBG_ERR("sound is playing, cannot play now!\r\n");
			return E_SYS;
		}

		// play wav data
		g_SoundData.puiData = (const UINT8 *)PCMBuf;
		g_SoundData.uiSize = PCMLen;
		g_SoundData.sampleRate = 16000;
		g_SoundData.isMono = TRUE;
		g_SoundData.soundId = 0xFFFFFFFF;
		err = GxSound_ActOnSndNotInTbl(SOUND_PLAY_START, &g_SoundData, TRUE);
	}

    return err;
}
UINT32 GetWavPlayCtrlStatus(void)
{
	return uWavPlayCtrlStatus;
}

void  SetWavPlayCtrlStatus(UINT32  mData)
{
	 uWavPlayCtrlStatus = mData;
}





void StopCurrWavPlay(void)
{
	if (GxSound_IsPlaying())
			GxSound_WaitStop();
	uWavPlayCtrlStatus = WAVPLAYCTRL_IDEL;

}

void StopAndClearAllWavPlay(void)
{

		if (GxSound_IsPlaying())
		{
			GxSound_Stop();
			//debug_msg("mmmm dx stop wav\r\n");
		}
		ClearWavQueue();
		uWavPlayCtrlStatus = WAVPLAYCTRL_IDEL;

}


void UserPlayWav(char *pFileName, UINT32  uPlayTimes,UINT8 uPlayInterval)
{
	UINT32 Cnt,Len;
	Len =  sizeof(uWavPlayCtrlData[Cnt].FileName);
	for(Cnt = 0;Cnt<MAX_WAV_PLAY_BUFFER-1;Cnt++)
	{
		strncpy(uWavPlayCtrlData[Cnt].FileName,  uWavPlayCtrlData[Cnt+1].FileName, Len);
		uWavPlayCtrlData[Cnt].PlayTimes = uWavPlayCtrlData[Cnt+1].PlayTimes ;
		uWavPlayCtrlData[Cnt].PlayInterval    = uWavPlayCtrlData[Cnt+1].PlayInterval;
	}

	
	strncpy(uWavPlayCtrlData[MAX_WAV_PLAY_BUFFER-1].FileName,  pFileName, Len);
	uWavPlayCtrlData[MAX_WAV_PLAY_BUFFER-1].PlayTimes = uPlayTimes ;
	uWavPlayCtrlData[MAX_WAV_PLAY_BUFFER-1].PlayInterval = uPlayInterval ;
}

void ClearWavQueue(void)
{

	UINT32 Cnt,Len;
	Len =  sizeof(uWavPlayCtrlData[Cnt].FileName);
	
	for(Cnt = 0;Cnt<MAX_WAV_PLAY_BUFFER;Cnt++)
	{
		memset(uWavPlayCtrlData[Cnt].FileName, 0, Len);
		uWavPlayCtrlData[Cnt].PlayTimes = 1 ;
		uWavPlayCtrlData[Cnt].PlayInterval = 0 ;
	}
	
}

void WavPlayCtrl_GetWavToPlay(char *pFileName, UINT32  *uPlayTimes,UINT8 *uPlayInterval)
{

	UINT32 Cnt,Len;
	//pFileName = NULL;
	pFileName[0] = 0;
	
	for(Cnt = 0;Cnt<MAX_WAV_PLAY_BUFFER;Cnt++)
	{
		if( uWavPlayCtrlData[Cnt].FileName[0] !=0)
		{
	             Len =  sizeof(uWavPlayCtrlData[Cnt].FileName);
			strncpy(pFileName,uWavPlayCtrlData[Cnt].FileName , Len);
			*uPlayTimes =  uWavPlayCtrlData[Cnt].PlayTimes;
			*uPlayInterval =  uWavPlayCtrlData[Cnt].PlayInterval;
			memset(uWavPlayCtrlData[Cnt].FileName, 0, Len);  //clear it
			break;
		}
	}
}


#endif
