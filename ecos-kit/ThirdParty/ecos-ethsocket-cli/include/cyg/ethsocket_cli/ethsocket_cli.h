#ifndef CYGONCE_NET_ETHSOCKETCLI_H
#define CYGONCE_NET_ETHSOCKETCLI_H
/* =================================================================
 *
 *      ethsocket_cli.h
 *
 *      A simple ethcan socket client
 *
 * =================================================================
 */

#if defined(__ECOS)
#include <pkgconf/system.h>
#include <pkgconf/isoinfra.h>
#include <cyg/hal/hal_tables.h>
#endif
#include <stdio.h>

#define CYG_ETHSOCKETCLI_STATUS_CLIENT_CONNECT           0   // client connect.
#define CYG_ETHSOCKETCLI_STATUS_CLIENT_REQUEST           1   // client has request comimg in
#define CYG_ETHSOCKETCLI_STATUS_CLIENT_DISCONNECT        2   // client disconnect.
#define CYG_ETHSOCKETCLI_STATUS_CLIENT_SOCKET_CLOSE       3   // client socket close.

#define CYG_ETHSOCKETCLI_UDP_STATUS_CLIENT_REQUEST       4   // udp client has request comimg in

#define CYG_ETHSOCKETCLI_STATUS_CLIENT_SLOW       5   //  client slow card.

#if defined(__ECOS)
#define DUMMY_NULL
#else
#ifndef __externC
#ifdef __cplusplus
# define __externC extern "C"
#else
# define __externC extern
#endif
#endif

#define bool int
#define false 0
#define true 1

#define cyg_bool int
#define cyg_addrword_t long*
#define CYG_ASSERT(cond,text) if(!(cond)){ \
                                  printf("%s\n", text);\
                                  exit(-1);\
                              }
#define cyg_flag_init(pflag) {*(pflag) = 0;}
#define cyg_flag_maskbits(pflag,bit) {*(pflag) &= (bit);}
#define cyg_flag_setbits(pflag,bit) {*(pflag) |= (bit);}
#define cyg_flag_wait(pflag,bit,mode) while(!(*(pflag) & (bit))){sleep(1);}
#define cyg_flag_destroy(pflag) {*(pflag) = 0;}
#define DUMMY_NULL NULL
#endif

typedef void ethsocket_cli_recv(int obj_index , char* addr, int size);
typedef void ethsocket_cli_notify(int obj_index ,int status, int parm);

#define ETHSOCKETCLI_IP_LEN          (16)

typedef struct
{
	ethsocket_cli_notify       *notify;        // notify the status
	ethsocket_cli_recv         *recv;
	char                       svrIP[ETHSOCKETCLI_IP_LEN];   // server IP addr
	int                        portNum;        // socket port number
	int                        sockbufSize;    // socket buffer size
	int                        timeout;
	int                        connect_socket;
        int                        threadPriority; // socket thread priority
} ethsocket_cli_obj;

typedef struct
{
	int                        path_id;
	int                        socket_id;
} ethsocket_cli_id_mapping;

__externC void ethsocket_cli_install(ethsocket_cli_obj*  pethsocket_cli_obj, ethsocket_cli_obj*  pObj); // install some callback function & config some settings

__externC void ethsocket_cli_uninstall(int path_id, int obj_index, ethsocket_cli_obj*  pethsocket_cli_obj);

__externC void ethsocket_cli_start(int path_id, int obj_index);     // open ethsocket

__externC void ethsocket_cli_stop(int path_id, int obj_index);    // close ethsocket

__externC int ethsocket_cli_send(int path_id, int obj_index, char* addr, int* size);  // install some callback function & config some settings

__externC ethsocket_cli_obj *ethsocket_cli_get_newobj(int path_id, int obj_index);

__externC int ethsocket_cli_connect(int obj_index, ethsocket_cli_obj *pethsocket_cli_obj);

__externC void ethsocket_cli_disconnect(ethsocket_cli_obj *pethsocket_cli_obj);


/* ----------------------------------------------------------------- */
#endif /* CYGONCE_NET_ETHSOCKETCLI_H                                 */
/* end of ethsocket_cli.h                                            */

