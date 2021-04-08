/**
    Eth socket client export variables and function prototypes.

    EthsockCliIpc is simple socket client API.
    Through this API user can send and receive network data.

    @file       EthsockCliIpcAPI.h
    @ingroup    mIEthsockCliIpcAPI

    Copyright   Novatek Microelectronics Corp. 2019.  All rights reserved.
*/
#ifndef _ETHSOCKCLIIPCAPI_H
#define _ETHSOCKCLIIPCAPI_H

#include "Type.h"

/**
    @name Ethsocket client error status.
*/
//@{
#define ETHSOCKETCLI_RET_OK           0          // funcion success
#define ETHSOCKETCLI_RET_OPENED       1          // is opend
#define ETHSOCKETCLI_RET_ERR          (-1)       // function fail
#define ETHSOCKETCLI_RET_NO_FUNC      (-2)       // no command function
#define ETHSOCKETCLI_RET_NOT_OPEN     (-3)       // not open ipc
#define ETHSOCKETCLI_RET_ERR_PAR      (-4)       // error parameter
//@}

typedef enum _ETHSOCKIPCCLI_ID {
	ETHSOCKIPCCLI_ID_0 = 0,       ///< ethsocket cli 0
	ETHSOCKIPCCLI_ID_1 = 1,          ///< ethsocket cli 1
	ETHSOCKIPCCLI_ID_2 = 2,          ///< ethsocket cli 2
	ETHSOCKIPCCLI_MAX_NUM,     ///< Number of ethsocket
	ENUM_DUMMY4WORD(ETHSOCKIPCCLI_ID)
} ETHSOCKIPCCLI_ID;


typedef struct
{
	//UINT32                  sharedMemAddr;  // shared memory address
	//UINT32                  sharedMemSize;  // shared memory size

	UINT32                  sharedSendMemAddr;  // shared Send memory address
	UINT32                  sharedSendMemSize;  // shared Send memory size

	UINT32                  sharedRecvMemAddr;  // shared Recv memory address
	UINT32                  sharedRecvMemSize;  // shared Recv memory size
} ETHSOCKCLIIPC_OPEN;


/**
    Ethsocket client receive prototype.
    User can register receive callback in EthsockCliIpc_Connect().
    It would be a callback function so it would use ethcam client task.

    @param[out] addr    receive buffer address.
    @param[out] size    receive data size.

    @return user can return result back.
*/
typedef int ethsocket_cli_recv(int path_id , char* addr, int size);

/**
    Ethsocket client notify prototype.
    User can register notify callback in EthsockCliIpc_Connect().

    @param[out] status    client connet status.
    @param[out] parm      connet parameter.

    @return void.

*/
typedef void ethsocket_cli_notify(int path_id , int status, int parm);

#define ETHSOCKETCLI_IP_LEN          (16)

/**
    Ethsocket install structure.
*/
typedef struct
{
	ethsocket_cli_notify        *notify;                    // notify the status
	ethsocket_cli_recv          *recv;
	char                        svrIP[ETHSOCKETCLI_IP_LEN]; // server IP addr
	int                         portNum;                    // socket port number
	int                         sockbufSize;                // socket buffer size
	int                         timeout;
	int                         connect_socket;
	int                         threadPriority; // socket thread priority
} ethsocketcli_install_obj;

/**
    @name Ethsocket client notify status.
*/
//@{
#define CYG_ETHSOCKETCLI_STATUS_CLIENT_CONNECT           0   // client connect.
#define CYG_ETHSOCKETCLI_STATUS_CLIENT_REQUEST           1   // client has request comimg in
#define CYG_ETHSOCKETCLI_STATUS_CLIENT_DISCONNECT        2   // client disconnect.
#define CYG_ETHSOCKETCLI_STATUS_CLIENT_SOCKET_CLOSE       3   // client socket close.

#define CYG_ETHSOCKETCLI_UDP_STATUS_CLIENT_REQUEST       4   // udp client has request comimg in

#define CYG_ETHSOCKETCLI_STATUS_CLIENT_SLOW       5   //  client slow card.//@}

/**
    Install EthsockCliIpc flag and semaphore id.

    @return void.
*/
extern void EthsockCliIpc_InstallID(UINT32 path_cnt) _SECTION(".kercfg_text");

/**
    Open ethcam socket client IPC task.
    This API would open IPC of ethcam socket.

    @param[in]      pOpen  user should assign none-cacha buffer for IPC

    @return
        - @b E_OK if success, others fail
*/
extern ER EthsockCliIpc_Open(UINT32  path_id, ETHSOCKIPCCLI_ID id,ETHSOCKCLIIPC_OPEN *pOpen);

/**
    Close ethcam socket client IPC task.
    This API would close IPC of user socket client .
    It should run after EthsockCliIpc_Stop();

    @return
        - @b E_OK if success, others fail
*/
extern ER EthsockCliIpc_Stop(UINT32  path_id, ETHSOCKIPCCLI_ID id);

/**
    EthsockCliIpc Start.
    It would start thread to wait receive data

    @return
        - @b E_OK if success, others fail
*/
extern ER  EthsockCliIpc_Start(UINT32  path_id, ETHSOCKIPCCLI_ID id);

/**
    EthsockCliIpc Connect.
    This is ethcam socket client initialization function.
    It would connect to socket server
    @param[out] pObj       init socket client parameter.

    @return
        - @b E_OK if success, others fail
*/
extern INT32 EthsockCliIpc_Connect(UINT32  path_id, ETHSOCKIPCCLI_ID id, ethsocketcli_install_obj*  pObj);

/**
    Ethcam socket client send data.

    @param[in] handle      client socket handle.

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
extern ER   EthsockCliIpc_Send(UINT32  path_id, ETHSOCKIPCCLI_ID id, INT32 handle, char* addr, int* size);

/**
    EthsockCliIpc Disconnect.

    It would disconnect to socket server
    @param[in,out] handle       client socket handle.

    @return
        - @b E_OK if success, others fail
*/
extern ER   EthsockCliIpc_Disconnect(UINT32  path_id, ETHSOCKIPCCLI_ID id, INT32 *handle);

/**
    Ethcam socket client Uninit.
    This API would close user socket.

    @return
        - @b E_OK if success, others fail
*/
extern ER  EthsockCliIpc_Close(UINT32  path_id, ETHSOCKIPCCLI_ID id);

#endif /* _ETHSOCKCLIIPCAPI_H  */
