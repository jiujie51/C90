#include <stdio.h>
#include <string.h>
#include "kernel.h"
#include "SysCfg.h"
#include "Utility.h"
#include "FileSysTsk.h"
#include "SysCommon.h"
#include "SxCmd.h"
#include "WavPlay.h"
#include "AACE_API.h"
#include "nvtmpp.h"
#include "FileDB.h"

#define __MODULE__			WavTrans
#define __DBGLVL__			1 // 0=OFF, 1=ERROR, 2=TRACE
#define __DBGFLT__			"*" //*=All, [mark]=CustomClass
#include "DebugModule.h"

#if (WAV_PLAY_FUNC == ENABLE)

#define WAV_HEADER_SIZE		44			// wav file header size
#define WAV_DATA_OFFSET		40			// wav data size offset
#define AAC_BUF_SIZE		(80*1024)	// 80KB for AAC data
#define AAC_RAW_BLOCK		1024		// 1024 samples per block for AAC encoding
#define AAC_FILE_FOLDER		"AAC\\"		// AAC output file folder name

ER WavPlay_EncodeAAC(UINT32 inAddr, UINT32 outAddr, UINT32 inLen, UINT32 *pOutLen, ST_AUDIO_AACENC_CFG *pAACEConfig)
{
	UINT32	BsLen = 0;
	ST_AUDIO_AACENC_BUFINFO	AACEBuf; // AAC encoder buffer info
	ST_AUDIO_AACENC_RTNINFO	AACERtn; // AAC encoder return info
	EN_AUDIO_ERROR_CODE		ErrorCode = E_OK;	// error code

	if ((ErrorCode = AACE_InitConfig(pAACEConfig)) != EN_AUDIO_ERROR_NONE)
	{
		DBG_ERR("AAC encoder init error %d!\r\n", ErrorCode);
		return E_SYS;
	}

	AACEBuf.u32nSamples = AAC_RAW_BLOCK * pAACEConfig->u32nChannels;

	while (inLen >= AACEBuf.u32nSamples * 2)
	{
		AACEBuf.pu32InBufferAddr	= (unsigned int *)inAddr;
		AACEBuf.pu32OutBufferAddr	= (unsigned int *)outAddr;
		if ((ErrorCode = AACE_EncodeOneFrame(&AACEBuf, &AACERtn)) != EN_AUDIO_ERROR_NONE)
		{
			DBG_ERR("AAC encode error, %d\r\n", ErrorCode);
			return E_SYS;
		}

		BsLen += AACERtn.u32EncodeOutBytes;
		inLen -= AACEBuf.u32nSamples * 2; // 16-bit PCM
		inAddr += AACEBuf.u32nSamples * 2; // 16-bit PCM
		outAddr += AACERtn.u32EncodeOutBytes;
	}

	*pOutLen = BsLen;

    return E_OK;
}

ER WavPlay_TransWavToAAC(char *pFolderPath)
{
	ER					err = E_OK;
	NVTMPP_VB_POOL		pool;
	NVTMPP_VB_BLK		blk;
	UINT32				blk_size = 200 * 1024; // 200KB for FileDB
	UINT32				pool_addr;
    CHAR				pool_name[]= "WavPlay";
	UINT32				i, FileNum;
	UINT32				channel = 1, RawBlockSize;
	UINT8				*pWavSizeOfs;
	UINT32				WavFileBuf, WavFileSize, WavBufSize;
	UINT32				PCMBuf, PCMLen;
	UINT32				AACBuf, AACLen;
	UINT32				PaddingAddr, PaddingSize;
	FILEDB_INIT_OBJ		FDBInitObj = {0};
	FILEDB_HANDLE		FileDBHdl;
	FILEDB_FILE_ATTR	*pFileAttr;
	FST_FILE			pFile;
	CHAR				AACFileName[80];
	ST_AUDIO_AACENC_CFG	AACEConfig;

	// create private pool for FileDB
    pool = nvtmpp_vb_create_pool(pool_name, blk_size, 1, NVTMPP_DDR_1);

    if (NVTMPP_VB_INVALID_POOL == pool)
	{
    	DBG_ERR("create private pool err\r\n");
    	return E_SYS;
    }

    blk = nvtmpp_vb_get_block(0, pool, blk_size, NVTMPP_DDR_1);
	if (NVTMPP_VB_INVALID_BLK == blk)
	{
		DBG_ERR("get vb block err\r\n");
    	return E_SYS;
	}

	pool_addr = nvtmpp_vb_block2addr(blk);
	DBG_IND("%s get noncache buf addr 0x%08X, size 0x%08X\r\n", pool_name, pool_addr, blk_size);

	// create FileDB for searching wav file
	FDBInitObj.rootPath				= pFolderPath;
	FDBInitObj.defaultfolder		= NULL;
	FDBInitObj.filterfolder			= NULL;
	FDBInitObj.bIsRecursive			= FALSE;//TRUE;
	FDBInitObj.bIsCyclic			= TRUE;
	FDBInitObj.bIsSupportLongName	= TRUE;
	FDBInitObj.u32MaxFilePathLen	= 80;
	FDBInitObj.u32MaxFileNum		= 150;
	FDBInitObj.fileFilter			= FILEDB_FMT_WAV;
	FDBInitObj.u32MemAddr			= pool_addr;
	FDBInitObj.u32MemSize			= blk_size;

	FileDBHdl = FileDB_Create(&FDBInitObj);
	if (FILEDB_CREATE_ERROR == FileDBHdl)
	{
		DBG_ERR("FileDB create failed\r\n");
    	return E_SYS;
	}

	FileNum = FileDB_GetTotalFileNum(FileDBHdl);

	WavFileBuf = OS_GetMempoolAddr(POOL_ID_WAV_PLAY_DATA);
	WavBufSize = OS_GetMempoolSize(POOL_ID_WAV_PLAY_DATA) - AAC_BUF_SIZE;
	RawBlockSize = AAC_RAW_BLOCK * channel * 2; // 16-bit PCM
	AACBuf = WavFileBuf + WavBufSize;
	AACLen = AAC_BUF_SIZE;

	for (i = 0; i < FileNum; i++)
	{
		pFileAttr = FileDB_SearhFile(FileDBHdl, i);
		if (pFileAttr)
		{
			// read PCM file
			pFile = FileSys_OpenFile(pFileAttr->filePath, FST_OPEN_READ);
			WavFileSize = WavBufSize; // max reading size
			if (pFile)
			{
				FileSys_ReadFile(pFile, (UINT8 *)WavFileBuf, &WavFileSize, 0, NULL);
				FileSys_CloseFile(pFile);
				DBG_DUMP("File %s, size %d\r\n", pFileAttr->filePath, WavFileSize);

				if (WavFileSize == WavBufSize)
				{
					DBG_ERR("Read buffer is not enough!\r\n");
					err = E_SYS;
					goto REL_BUF;
				}
			}
			else
			{
				DBG_ERR("Open %s for reading failed!\r\n", pFileAttr->filePath);
				err = E_SYS;
				goto REL_BUF;
			}

			// get wav data size
			pWavSizeOfs = (UINT8 *)(WavFileBuf + WAV_DATA_OFFSET);
			PCMLen = *pWavSizeOfs | ((UINT32)*(pWavSizeOfs + 1) << 8) | ((UINT32)*(pWavSizeOfs + 2) << 16) | ((UINT32)*(pWavSizeOfs + 3) << 24);
			//DBG_DUMP("PCM size 0x%08x\r\n", PCMLen);

			// aligned to AAC block size and padding 0
			PCMBuf = WavFileBuf + WAV_HEADER_SIZE;
			PaddingAddr = PCMBuf + PCMLen;
			PaddingSize = RawBlockSize - (PCMLen % RawBlockSize);
			memset((void *)PaddingAddr, 0, PaddingSize);
			PCMLen += PaddingSize;
			DBG_DUMP("PCM buf 0x%08x, size 0x%x, padding addr 0x%08x, size 0x%x\r\n", PCMBuf, PCMLen, PaddingAddr, PaddingSize);

			// encode
			AACEConfig.enSampleRate		= EN_AUDIO_SAMPLING_RATE_16000;
			AACEConfig.u32nChannels		= channel;
			AACEConfig.enBitRate		= EN_AUDIO_BIT_RATE_48k;//EN_AUDIO_BIT_RATE_64k;
			AACEConfig.enCodingType		= EN_AUDIO_CODING_TYPE_ENABLE;
			AACEConfig.enEncStopFreq	= EN_AUDIO_ENC_STOP_FREQ_16K;

			err = WavPlay_EncodeAAC(PCMBuf, AACBuf, PCMLen, &AACLen, &AACEConfig);

			// write to file
			if (err == E_OK)
			{
				UINT32 pos;
				memset(AACFileName, 0, sizeof(AACFileName));
				memcpy(AACFileName, pFolderPath, strlen(pFolderPath));
				pos = (UINT32)AACFileName + strlen(pFolderPath);
				memcpy((void *)pos, AAC_FILE_FOLDER, strlen(AAC_FILE_FOLDER));
				pos += strlen(AAC_FILE_FOLDER);
				memcpy((void *)pos, pFileAttr->filename, strlen(pFileAttr->filename));
				DBG_DUMP("AAC file %s, len %d\r\n\r\n", AACFileName, AACLen);

				pFile = FileSys_OpenFile(AACFileName, FST_CREATE_ALWAYS | FST_OPEN_WRITE);
				if (pFile)
				{
					FileSys_WriteFile(pFile, (UINT8 *)AACBuf, &AACLen, 0, NULL);
				}
				FileSys_CloseFile(pFile);
			}
		}
	}

REL_BUF:

	FileDB_Release(FileDBHdl);
	nvtmpp_vb_destroy_pool(pool);

    return err;
}

#endif
