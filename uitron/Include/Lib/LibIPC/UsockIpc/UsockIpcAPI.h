/**
    User socket export variables and function prototypes.

    UsockIpc is simple socket API.
    Through this API user can send and receive network data.

    @file       UsockIpcAPI.h
    @ingroup    mIUsockIpcAPI

    Copyright   Novatek Microelectronics Corp. 2015.  All rights reserved.
*/
#ifndef _USOCKIPCAPI_H
#define _USOCKIPCAPI_H

#ifdef __ECOS
#define USE_IPC                  0
#else
#define USE_IPC                  1
#endif
#include "Type.h"
/**
    @name Usocket error status.
*/
//@{
#define USOCKET_RET_OK           0          // funcion success
#define USOCKET_RET_OPENED       1          // is opend
#define USOCKET_RET_ERR          (-1)       // function fail
#define USOCKET_RET_NO_FUNC      (-2)       // no command function
#define USOCKET_RET_NOT_OPEN     (-3)       // not open ipc
#define USOCKET_RET_ERR_PAR      (-4)       // error parameter
//@}


/**
     usocket ID
*/
typedef enum _USOCKIPC_ID {
	USOCKIPC_ID_0 = 0,       ///< usocket 0
	USOCKIPC_ID_1 = 1,       ///< usocket 1
	USOCKIPC_MAX_NUM,     ///< Number of usocket
	ENUM_DUMMY4WORD(USOCKIPC_ID)
} USOCKIPC_ID;


typedef struct {
	UINT32                  sharedMemAddr;  // shared memory address
	UINT32                  sharedMemSize;  // shared memory size
} USOCKIPC_OPEN;
#if (!USE_IPC)
#ifdef __ECOS //use thirdParty include file
#include <cyg/usocket/usocket.h>
#else //use tmp usocket.h file for compile
#include "usocket.h"
#endif
#else
/**
    Usocket receive prototype.
    User can register receive callback in UsockIpc_Init().
    It would be a callback function so it would use usocket task.

    @param[out] addr    receive buffer address.
    @param[out] size    receive data size.

    @return user can return result back.
*/
typedef int usocket_recv(char *addr, int size);

/**
    Usocket notify prototype.
    User can register notify callback in UsockIpc_Init().

    @param[out] status    client connet status.
    @param[out] parm      connet parameter.

    @return void.

*/
typedef void usocket_notify(int status, int parm);

/**
    Usocket install structure.
*/
typedef struct {
	usocket_notify         *notify;          ///< notify connect status.
	usocket_recv           *recv;            ///< socket receive data callback.
	int                     portNum;         ///< socket port number.
	int                     threadPriority;  ///< socket thread priority.
	int                     sockbufSize;     ///< socket buffer size.
	int                     client_socket;   ///< client socket identity.
} usocket_install_obj;

/**
    @name Usocket notify status.
*/
//@{
#define CYG_USOCKET_STATUS_CLIENT_CONNECT           0   // client connect.
#define CYG_USOCKET_STATUS_CLIENT_REQUEST           1   // client has request comimg in
#define CYG_USOCKET_STATUS_CLIENT_DISCONNECT        2   // client disconnect.
#define CYG_USOCKET_UDP_STATUS_CLIENT_REQUEST       3   // udp client has request comimg in
//@}
#endif
/**
    Install UsockIpc flag and semaphore id.

    @return void.
*/
extern void UsockIpc_InstallID(void) _SECTION(".kercfg_text");

/**
    Open user socket IPC task.
    This API would open IPC of user socket.
    It would run before UsockIpc_Init();

    @param[in]      id     usocket id
    @param[in]      pOpen  user should assign none-cacha buffer for IPC

    @return
        - @b E_OK if success, others fail
*/
extern ER UsockIpcMulti_Open(USOCKIPC_ID id,USOCKIPC_OPEN *pOpen);

/**
    Close user socket IPC task.
    This API would close IPC of user socket.
    It should run after UsockIpc_Uninit();

    @param[in]      id     usocket id

    @return
        - @b E_OK if success, others fail
*/
extern ER   UsockIpcMulti_Close(USOCKIPC_ID id);

/**
    UsockIpc Init.
    This is user socket initialization function.

    @param[in]  id     usocket id
    @param[out] pObj       init socket parameter.

    @return
        - @b E_OK if success, others fail
*/
extern ER   UsockIpcMulti_Init(USOCKIPC_ID id,usocket_install_obj  *pObj);

/**
    User socket send data.

    @param[in]  id         usocket id

    @param[out] addr       Send buffer address.
                           Buffer address must be 16 bytes alignment.
                           Buffer size also need 16 byte alignment;even data is not alignment.
    @param[in,out] size    Send data size,and would return real send data size.
                           Size address must be 16 bytes alignment.

    @return
        - @b (0)  send success.
        - @b (-1) send fail.

    @return
        - @b E_OK if success, others fail
*/
extern ER   UsockIpcMulti_Send(USOCKIPC_ID id,char *addr, int *size);

/**
    User socket Uninit.
    This API would close user socket.

    @param[in]      id     usocket id

    @return
        - @b E_OK if success, others fail
*/
extern ER   UsockIpcMulti_Uninit(USOCKIPC_ID id);


/**
    UsockIpc UDP Init.
    This is user UDP socket initialization function.

    @param[out] pObj       init UDP socket parameter.

    @return
        - @b E_OK if success, others fail
*/
extern ER   UsockIpc_Udp_Init(usocket_install_obj  *pObj);


/**
    User socket send UDP data back to UDP client which ever send to UDP server.

    @param[out] addr       Send buffer address.

    @param[in,out] size    Send data size,and would return real send data size.

    @return
        - @b (0)  send success.
        - @b (-1) send fail.

    @return
        - @b E_OK if success, others fail
*/
extern ER UsockIpc_Udp_Send(char *addr, int *size);

/**
    User socket UDP sendto.

    @param[in] dest_ip     destination IP addr.

    @param[in] dest_port   destination port.

    @param[out] buf        Send buffer address.

    @param[in,out] size    Send data size,and would return real send data size.


    @return
        - @b (0)  send success.
        - @b (-1) send fail.

    @return
        - @b E_OK if success, others fail
*/
extern ER UsockIpc_Udp_Sendto(char *dest_ip, int dest_port, char *buf, int *size);

/**
    User UDP socket Uninit.
    This API would close user socket.

    @return
        - @b E_OK if success, others fail
*/
extern ER   UsockIpc_Udp_Uninit(void);



#define UsockIpc_Open(pOpen)          UsockIpcMulti_Open(USOCKIPC_ID_0,pOpen)
#define UsockIpc_Close()              UsockIpcMulti_Close(USOCKIPC_ID_0)
#define UsockIpc_Init(pObj)           UsockIpcMulti_Init(USOCKIPC_ID_0,pObj)
#define UsockIpc_Send(addr,size)      UsockIpcMulti_Send(USOCKIPC_ID_0,addr,size)
#define UsockIpc_Uninit()             UsockIpcMulti_Uninit(USOCKIPC_ID_0)

#endif /* _USOCKIPCAPI_H  */
