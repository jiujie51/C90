#ifndef _WAVPLAY_H
#define _WAVPLAY_H

#define WAV_duang_0XG0			   "duang_0XG0.wav"
#define WAV_green_0XG2			   "green_0XG2.wav"

#define WAV_adas_frontstart		   "adas_frontstart.wav"
#define WAV_adas_green			   "adas_green.wav"

#define WAV_adas_20_EN                "adas_20_EN.wav"
#define WAV_adas_30_EN                "adas_30_EN.wav"
#define WAV_adas_40_EN            	   "adas_40_EN.wav"
#define WAV_adas_50_EN                "adas_50_EN.wav"
#define WAV_adas_60_EN                "adas_60_EN.wav"
#define WAV_adas_70_EN                "adas_70_EN.wav"
#define WAV_adas_80_EN                "adas_80_EN.wav"
#define WAV_adas_90_EN                "adas_90_EN.wav"
#define WAV_adas_100_EN			   "adas_100_EN.wav"
#define WAV_adas_110_EN               "adas_110_EN.wav"
#define WAV_adas_120_EN			   "adas_120_EN.wav"
#define WAV_adas_frontspeed_EN		   "adas_frontspeed_EN.wav"


#define WAV_adas_20                "adas_20.wav"
#define WAV_adas_30                "adas_30.wav"
#define WAV_adas_40            	   "adas_40.wav"
#define WAV_adas_50                "adas_50.wav"
#define WAV_adas_60                "adas_60.wav"
#define WAV_adas_70                "adas_70.wav"
#define WAV_adas_80                "adas_80.wav"
#define WAV_adas_90                "adas_90.wav"
#define WAV_adas_100			   "adas_100.wav"
#define WAV_adas_110               "adas_110.wav"
#define WAV_adas_120			   "adas_120.wav"
#define WAV_adas_frontspeed		   "adas_frontspeed.wav"

typedef struct _WAV_MERGE_DATA
{
    char    FileName[40];   ///< original wav file name
    UINT32  uiAddr;         ///< wav data address in PStore section
    UINT32  uiSize;         ///< wav data size
} WAV_MERGE_DATA;

enum  _WAVPLAYCTRL{
	WAVPLAYCTRL_IDEL,
	WAVPLAYCTRL_LOOP_ON,
	WAVPLAYCTRL_LOOP_PAUSE,
	WAVPLAYCTRL_INFINITE_LOOP
	
} ;
#define MAX_WAV_PLAY_BUFFER	 5
#define MAX_WAV_NAME_LEN	 36

typedef struct _WAV_PLAY_CTRL_DATA
{
	char FileName[MAX_WAV_NAME_LEN];
	UINT8 PlayInterval;    // 1lsb 10MS   //use only when  loop play 
	UINT32 PlayTimes;   //   other----> play N time
	
	
}WAV_PLAY_CTRL_DATA;

extern ER WavPlay_TransWavToAAC(char *pFolderPath);
extern ER WavPlay_WriteWavData(void);
extern ER WavPlay_PlayWavData(char *pName);
extern ER WavPlay_PlayAACData(char *pName);
extern ER WavPlay_PlayWavDataFromFile(char *pFileName);
extern ER WavPlay_PlayAACDataFromFile(char *pFileName);


extern UINT32 GetWavPlayCtrlStatus(void);

#endif
