#ifndef CYGONCE_NET_ETHSOCKET_H
#define CYGONCE_NET_ETHSOCKET_H
/* =================================================================
 *
 *      ethsocket.h
 *
 *      A simple user socket server
 *
 * =================================================================
 */

#if defined(__ECOS)
#include <pkgconf/system.h>
#include <pkgconf/isoinfra.h>
#include <cyg/hal/hal_tables.h>
#endif
#include <stdio.h>

#define CYG_ETHSOCKET_STATUS_CLIENT_CONNECT           0   // client connect.
#define CYG_ETHSOCKET_STATUS_CLIENT_REQUEST           1   // client has request comimg in
#define CYG_ETHSOCKET_STATUS_CLIENT_DISCONNECT        2   // client disconnect.
#define CYG_ETHSOCKET_UDP_STATUS_CLIENT_REQUEST       3   // udp client has request comimg in

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

typedef int ethsocket_recv(char *addr, int size);
typedef void ethsocket_notify(int status, int parm);
typedef int ethsocket_recv_multi(char *addr, int size, int obj_index);
typedef void ethsocket_notify_multi(int status, int parm, int obj_index);

typedef struct {
	ethsocket_notify        *notify;        // notify the status
	ethsocket_recv          *recv;
	int                     portNum;        // socket port number
	int                     threadPriority; // socket thread priority
	int                     sockbufSize;    // socket buffer size
	int                     client_socket;
} ethsocket_install_obj;


typedef struct {
	ethsocket_notify_multi  *notify;        // notify the status
	ethsocket_recv_multi    *recv;
	int                     portNum;        // socket port number
	int                     threadPriority; // socket thread priority
	int                     sockbufSize;    // socket buffer size
	int                     client_socket;
	int                     occupy;         // 0:not using ,1:using
} ethsocket_install_obj_multi;

/////////////////////ethscoket multi//////////////////////////////////

__externC int ethsocket_install_multi(ethsocket_install_obj_multi  *pObj);  ///return obj_index, if return -1 means error.

__externC void ethsocket_uninstall_multi(int obj_index);

__externC void ethsocket_open_multi(int obj_index);

__externC void ethsocket_close_multi(int obj_index);

__externC int ethsocket_send_multi(char *buf, int *size, int obj_index);

__externC int ethsocket_get_obj_index(int port);

////////////////////////ethsocket/////////////////////////////////////

__externC void ethsocket_install(ethsocket_install_obj  *pObj);   // install some callback function & config some settings

__externC void ethsocket_uninstall(void);

__externC void ethsocket_open(void);     // open ethsocket

__externC void ethsocket_close(void);    // close ethsocket

__externC int ethsocket_send(char *addr, int *size);  // install some callback function & config some settings

///////////////////////////UDP////////////////////////////

__externC void ethsocket_udp_install(ethsocket_install_obj  *pObj);   // install some callback function & config some settings

__externC void ethsocket_udp_uninstall(void);

__externC void ethsocket_udp_open(void);     // open ethsocket

__externC void ethsocket_udp_close(void);    // close ethsocket

__externC int ethsocket_udp_send(char *addr, int *size);  // install some callback function & config some settings

__externC int ethsocket_udp_sendto(char *dest_ip, int dest_port, char *buf, int *size);

/* ----------------------------------------------------------------- */
#endif /* CYGONCE_NET_ETHSOCKET_H                                    */
/* end of ethsocket.h                                                */

