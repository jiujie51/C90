/**
    Ethcam socket export variables and function prototypes.

    EthsockIpc is simple socket API.
    Through this API user can send and receive network data.

    @file       EthsockIpcAPI.h
    @ingroup    mIEthsockIpcAPI

    Copyright   Novatek Microelectronics Corp. 2019.  All rights reserved.
*/
#ifndef _ETHSOCKIPCAPI_H
#define _ETHSOCKIPCAPI_H

#ifdef __ECOS
#define USE_IPC                  0
#else
#define USE_IPC                  1
#endif
#include "Type.h"
/**
    @name Ethsocket error status.
*/
//@{
#define ETHSOCKET_RET_OK           0          // funcion success
#define ETHSOCKET_RET_OPENED       1          // is opend
#define ETHSOCKET_RET_ERR          (-1)       // function fail
#define ETHSOCKET_RET_NO_FUNC      (-2)       // no command function
#define ETHSOCKET_RET_NOT_OPEN     (-3)       // not open ipc
#define ETHSOCKET_RET_ERR_PAR      (-4)       // error parameter
//@}


/**
     ethsocket ID
*/
typedef enum _ETHSOCKIPC_ID {
	ETHSOCKIPC_ID_0 = 0,       ///< ethsocket 0
	ETHSOCKIPC_ID_1 = 1,       ///< ethsocket 1
	ETHSOCKIPC_ID_2 = 2,       ///< ethsocket 2
	ETHSOCKIPC_MAX_NUM,     ///< Number of ethsocket
	ENUM_DUMMY4WORD(ETHSOCKIPC_ID)
} ETHSOCKIPC_ID;

typedef struct {
	//UINT32                  sharedMemAddr;  // shared memory address
	//UINT32                  sharedMemSize;  // shared memory size

	UINT32                  sharedSendMemAddr;  // shared Send memory address
	UINT32                  sharedSendMemSize;  // shared Send memory size

	UINT32                  sharedRecvMemAddr;  // shared Recv memory address
	UINT32                  sharedRecvMemSize;  // shared Recv memory size
} ETHSOCKIPC_OPEN;
#if (!USE_IPC)
#ifdef __ECOS //use thirdParty include file
#include <cyg/ethsocket/ethsocket.h>
#else //use tmp ethsocket.h file for compile
#include "ethsocket.h"
#endif
#else
/**
    Ethsocket receive prototype.
    User can register receive callback in EthsockIpc_Init().
    It would be a callback function so it would use ethsocket task.

    @param[out] addr    receive buffer address.
    @param[out] size    receive data size.

    @return user can return result back.
*/
typedef int ethsocket_recv(char *addr, int size);

/**
    Ethsocket notify prototype.
    User can register notify callback in EthsockIpc_Init().

    @param[out] status    client connet status.
    @param[out] parm      connet parameter.

    @return void.

*/
typedef void ethsocket_notify(int status, int parm);

/**
    Ethsocket install structure.
*/
typedef struct {
	ethsocket_notify        *notify;          ///< notify connect status.
	ethsocket_recv          *recv;            ///< socket receive data callback.
	int                     portNum;          ///< socket port number.
	int                     threadPriority;   ///< socket thread priority.
	int                     sockbufSize;      ///< socket buffer size.
	int                     client_socket;    ///< client socket identity.
} ethsocket_install_obj;

/**
    @name Ethsocket notify status.
*/
//@{
#define CYG_ETHSOCKET_STATUS_CLIENT_CONNECT           0   // client connect.
#define CYG_ETHSOCKET_STATUS_CLIENT_REQUEST           1   // client has request comimg in
#define CYG_ETHSOCKET_STATUS_CLIENT_DISCONNECT        2   // client disconnect.
#define CYG_ETHSOCKET_UDP_STATUS_CLIENT_REQUEST       3   // udp client has request comimg in
//@}
#endif
/**
    Install EthsockIpc flag and semaphore id.

    @return void.
*/
extern void EthsockIpc_InstallID(void) _SECTION(".kercfg_text");

/**
    Open ethcam socket IPC task.
    This API would open IPC of ethcam socket.
    It would run before EthsockIpc_Init();

    @param[in]      id     ethsocket id
    @param[in]      pOpen  user should assign none-cacha buffer for IPC

    @return
        - @b E_OK if success, others fail
*/
extern ER EthsockIpcMulti_Open(ETHSOCKIPC_ID id, ETHSOCKIPC_OPEN *pOpen);

/**
    Close ethcam socket IPC task.
    This API would close IPC of ethcam socket.
    It should run after EthsockIpc_Uninit();

    @param[in]      id     ethsocket id

    @return
        - @b E_OK if success, others fail
*/
extern ER   EthsockIpcMulti_Close(ETHSOCKIPC_ID id);

/**
    EthsockIpc Init.
    This is user socket initialization function.

    @param[in]  id     ethsocket id
    @param[out] pObj       init socket parameter.

    @return
        - @b E_OK if success, others fail
*/
extern ER   EthsockIpcMulti_Init(ETHSOCKIPC_ID id, ethsocket_install_obj  *pObj);

/**
    Ethcam socket send data.

    @param[in]  id         ethsocket id

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
extern ER   EthsockIpcMulti_Send(ETHSOCKIPC_ID id, char *addr, int *size);

/**
    Ethcam socket Uninit.
    This API would close ethcam socket.

    @param[in]      id     ethsocket id

    @return
        - @b E_OK if success, others fail
*/
extern ER   EthsockIpcMulti_Uninit(ETHSOCKIPC_ID id);


/**
    EthsockIpc UDP Init.
    This is user UDP socket initialization function.

    @param[out] pObj       init UDP socket parameter.

    @return
        - @b E_OK if success, others fail
*/
extern ER   EthsockIpc_Udp_Init(ethsocket_install_obj  *pObj);


/**
    Ethcam socket send UDP data back to UDP client which ever send to UDP server.

    @param[out] addr       Send buffer address.

    @param[in,out] size    Send data size,and would return real send data size.

    @return
        - @b (0)  send success.
        - @b (-1) send fail.

    @return
        - @b E_OK if success, others fail
*/
extern ER EthsockIpc_Udp_Send(char *addr, int *size);

/**
    Ethcam socket UDP sendto.

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
extern ER EthsockIpc_Udp_Sendto(char *dest_ip, int dest_port, char *buf, int *size);

/**
    Ethcam UDP socket Uninit.
    This API would close user socket.

    @return
        - @b E_OK if success, others fail
*/
extern ER   EthsockIpc_Udp_Uninit(void);

/**
    Install EthsockUdpIpc flag and semaphore id.

    @return void.
*/
extern void EthsockUdpIpc_InstallID(void) _SECTION(".kercfg_text");


#define EthsockIpc_Open(pOpen)          EthsockIpcMulti_Open(ETHSOCKIPC_ID_0,pOpen)
#define EthsockIpc_Close()              EthsockIpcMulti_Close(ETHSOCKIPC_ID_0)
#define EthsockIpc_Init(pObj)           EthsockIpcMulti_Init(ETHSOCKIPC_ID_0,pObj)
#define EthsockIpc_Send(addr,size)      EthsockIpcMulti_Send(ETHSOCKIPC_ID_0,addr,size)
#define EthsockIpc_Uninit()             EthsockIpcMulti_Uninit(ETHSOCKIPC_ID_0)

#endif /* _ETHSOCKIPCAPI_H  */

