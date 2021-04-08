//
//Anfa_License.h
//
//



#ifndef _ANFA_LICENSE_H_
#define _ANFA_LICENSE_H_



typedef enum
{
    USE_PSTORE=0,
    USE_EMBTYPE_USER0,
    ENUM_DUMMY4WORD(SN_SAVE_AREA)
}SN_SAVE_AREA;



extern void AnfaAlgAuthProc_InstallID(void);

//extern void App_cJSON_Init(void);

extern INT32 AnfaAlgAuthProc_Init(void);

extern INT32 AnfaAlgAuthProc_UnInit(void);

extern void AnfaAlgAuth_NetReady(void);

extern void AnfaAlgAuth_MacAddrReady(void);

extern BOOL AnfaAlgAuth_IsOK(void);

extern void AV_SetUserEMBMEMType(UINT32 tpye);

extern void AV_SelectSNSaveArea(SN_SAVE_AREA value);

extern INT32 AVLic_SetUserID(CHAR *ID);//id name 限制8个字符内

extern void AV_SetUIDFormat(UINT32 value);//设置uid 的格式，0 mac地址，1 chipid

#endif