/**
    AlphaVision Rear Assistance System Library.

    @file       av_lca_lib.h

    include LCA 

    Copyright  ALPHAVISON . 2019.  All rights reserved.
*/
#ifndef _AV_LCA_LIB_H
#define _AV_LCA_LIB_H


#include "photo_task.h"

/**
    
*/
typedef enum 
{
	AVLCA_L_WRN_NONE = 0,          //
	AVLCA_L_WRN_LEVEL_L= 1,       //�������������������1�������ߣ�����Զ
	AVLCA_L_WRN_LEVEL_H= 2,       //�������������������2�������ߣ������
} AVLCA_LEFT_WARNMSG;

typedef enum 
{
	AVLCA_M_WRN_NONE = 0,          //
	AVLCA_M_WRN_LEVEL_L= 1,       //�����м�������������1�������ߣ�����Զ
	AVLCA_M_WRN_LEVEL_H= 2,       //�����м�������������2�������ߣ������
} AVLCA_MIDDLE_WARNMSG;

typedef enum 
{
	AVLCA_R_WRN_NONE = 0,          //
	AVLCA_R_WRN_LEVEL_L= 1,       //�����ұ�������������1�������ߣ�����Զ
	AVLCA_R_WRN_LEVEL_H= 2,       //�����ұ�������������2�������ߣ������
} AVLCA_RIGHT_WARNMSG;


typedef struct _AVLCA_WARNING_RESULT
{
	AVLCA_LEFT_WARNMSG        	LWarnStatus;
	AVLCA_MIDDLE_WARNMSG        MWarnStatus;
	AVLCA_RIGHT_WARNMSG			RWarnStatus;
	 	 
} AVLCA_WARNING_RESULT;


typedef struct _AVLCA_VEHICLE_INFO //��鵽�ĳ�����������
{
	INT32      iVehicleNum;
	IRECT	   iVehicleRect[20]; //���20��������Ϣ
	 	 
} AVLCA_VEHICLE_INFO;

extern INT32 Anfa_LCACopySrcDataInt(IMG_BUF *pInputImg);

extern void AV_LCAInit(MEM_RANGE *buf, MEM_RANGE *cachebuf);

extern VOID AV_LCAProcess(void);

extern UINT32 AV_CalcLCABuffSize(VOID);

extern UINT32 AV_CalcLCACacheBuffSize(VOID);



extern INT32 AV_GetLCAWarnRslt(UINT8* rsltInfo);//��ȡ�������

extern INT32 AV_GetLCAVehicleInfo(AVLCA_VEHICLE_INFO *pVehicleInfo);//��ȡ��⵽�ĳ�����������

extern INT32 AV_GetInstallCoord(IPOINT *pCoord);//��ȡ��װ�豸�Ĳο�����

extern VOID AV_RAS_InstallID(VOID);
extern VOID AV_RASInstallTask(PHOTO_FUNC_INFO* phead);

extern void AV_GetTXRslt(char* inputTXinfo,AVLCA_WARNING_RESULT *LCA_Result);

extern UINT32 AV_GetLCARunMode(void);//get day==1 or night==2 mode

extern UINT32 AV_SetDetTriangleVertex(IRECT point);

extern UINT32 AV_SetWarningLevel(INT32 level1,INT32 level2);

#endif
