/**
    AlphaVision ADAS library.

    @file       anfa_adas_lib.h

    @ include SNG TLD TSR FCW LDW 

    Copyright  AlphaVision . 2018.  All rights reserved.
*/
#ifndef _ANFA_ADAS_LIB_H
#define _ANFA_ADAS_LIB_H


#include "photo_task.h"
#include "tsr_lib.h"


/**
    
*/
typedef enum
{
    ANFA_SNG_FAILURE_TRUE   = 1,      ///<  in failure mode
    ANFA_SNG_FAILURE_FALSE  = 0        ///<  Not in failure mode
} ANFA_SNG_FAILURE_STATUS;


/**
    
*/
typedef enum 
{
	ANFA_SNG_NONE = 0,          //
	ANFA_SNG_GO = 1,            //

} ANFA_SNG_WARNMSG;

/**
    
*/
typedef enum
{
    ANFA_TLD_FAILURE_TRUE   = 1,      ///<  in failure mode
    ANFA_TLD_FAILURE_FALSE  = 0        ///<  Not in failure mode
} ANFA_TLD_FAILURE_STATUS;


/**
    
*/
typedef enum 
{
	ANFA_TLD_WRN_NONE = 0,          ///<
	ANFA_TLD_WRNING = 1,            ///<

} ANFA_TLD_WARNMSG;


/**
    
*/
typedef enum
{
    ANFA_FCW_FAILURE_TRUE   = 1,      ///<  in failure mode
    ANFA_FCW_FAILURE_FALSE  = 0        ///<  Not in failure mode
} ANFA_FCW_FAILURE_STATUS;


/**
    
*/
typedef enum 
{
	ANFA_FCW_WRN_NONE = 0,          ///<
	ANFA_FCW_WRNING = 1,            ///<

} ANFA_FCW_WARNMSG;


/**
    
*/
typedef enum
{
    ANFA_LDW_FAILURE_TRUE   = 1,      ///<  in failure mode
    ANFA_LDW_FAILURE_FALSE  = 0        ///<  Not in failure mode
} ANFA_LDW_FAILURE_STATUS;


/**
    
*/
typedef enum 
{
	ANFA_LDW_WRN_NONE = 0,          ///<
	ANFA_LDW_WRNING = 1,            ///<

} ANFA_LDW_WARNMSG;


//@{
/**
    SNG Result
*/
typedef struct _ANFA_SNG_RESULT_INFO
{
	ANFA_SNG_FAILURE_STATUS    Failure;           ///<  Failure mode
    ANFA_SNG_WARNMSG           SNGWarnStatus;    ///<  sound warning flag 
 	 
} ANFA_SNG_RESULT_INFO;
/**
    TLD Result
*/
typedef struct _ANFA_TLD_RESULT_INFO
{
	ANFA_TLD_FAILURE_STATUS    Failure;           ///<  Failure mode
    ANFA_TLD_WARNMSG           TLDWarnStatus;    ///<  sound warning flag 
 	 
} ANFA_TLD_RESULT_INFO;


/**
    FCWS Result
*/
typedef struct _ANFA_FCW_RESULT_INFO
{
	ANFA_FCW_FAILURE_STATUS    Failure;           ///<  Failure mode
    ANFA_FCW_WARNMSG           FCWWarnStatus;    ///<  sound warning flag 
 	 
} ANFA_FCW_RESULT_INFO;


/**
    LDWS Result
*/
typedef struct _ANFA_LDW_RESULT_INFO
{
	ANFA_LDW_FAILURE_STATUS    Failure;           ///<  Failure mode
    ANFA_LDW_WARNMSG           LDWWarnStatus;    ///<  sound warning flag 
 	 
}ANFA_LDW_RESULT_INFO; 

/**




/**
    Init ADAS
*/
extern VOID Anfa_ADAS_InstallID(VOID);

extern VOID Anfa_AdasInit(MEM_RANGE *buf, MEM_RANGE *cachebuf);
/**
	Anfa Process    
*/
INT32 Anfa_SetADASProcess(INT32 GpsSpeed,BOOL GpsEN,INT32 GsensorFlag,BOOL GsEN);

/**
	buffer size   
*/
extern UINT32 Anfa_AdasCalcBuffSize(VOID); 

/**
	cache buffer size   
*/
extern UINT32 Anfa_AdasCalcCacheBuffSize(VOID);

/**
   get result once ,and then wile reset warnning
*/
extern INT32 Anfa_GetSngRslt(ANFA_SNG_RESULT_INFO *pAnfaSngResult);

extern INT32 Anfa_GetTldRslt(ANFA_TLD_RESULT_INFO *pAnfaTldResult);

extern INT32 Anfa_GetFCWRslt(ANFA_FCW_RESULT_INFO *pAnfaFCWResult);

extern INT32 Anfa_GetLDWRslt(ANFA_LDW_RESULT_INFO *pAnfaLDWResult);

/**
  TSR
*/
extern TSR_SPEED Anfa_GetTsrMaxRslt(VOID);

/**
 Anfa adas  task function 
*/
extern VOID Anfa_ADAS_InstallTask(PHOTO_FUNC_INFO* phead);


extern INT32 Anfa_SetSngMdEnable(BOOL bkey);
extern INT32 Anfa_SetTldEnable(BOOL bkey);
extern INT32 Anfa_SetTsrEnable(BOOL bkey);
extern INT32 Anfa_SetFcwEnable(BOOL bkey);
extern INT32 Anfa_SetLdwEnable(BOOL bkey);


extern INT32 Anfa_SetSngMdProEnable(BOOL bkey);
extern INT32 Anfa_SetTldProEnable(BOOL bkey);
extern INT32 Anfa_SetTsrProEnable(BOOL bkey);



extern BOOL Anfa_GetTsrEnable(VOID);
extern BOOL Anfa_GetTldEnable(VOID);
extern BOOL Anfa_GetSngMdEnable(VOID);
extern BOOL Anfa_GetFcwEnable(VOID);
extern BOOL Anfa_GetLdwEnable(VOID);


extern BOOL AnfaTSR_ValidForProcess(VOID);

//
extern int MD_Gsensor(int accX, int accY, int accZ,int mdThreshold);

//debug
extern VOID Anfa_SetDebugEN(UINT32 SngDebugEn,UINT32 TldDebugEn,UINT32 TsrDebugEn);

extern UINT32 Anfa_AdasOsdDebugDraw(UINT32 *paramArray);

#endif
