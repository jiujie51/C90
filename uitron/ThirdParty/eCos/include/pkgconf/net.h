#ifndef CYGONCE_PKGCONF_NET_H
#define CYGONCE_PKGCONF_NET_H
/*
 * File <pkgconf/net.h>
 *
 * This file is generated automatically by the configuration
 * system. It should not be edited. Any changes to this file
 * may be overwritten.
 */

#include <pkgconf/system.h>
#include CYGDAT_NET_STACK_CFG
#define CYGPKG_NET_DRIVER_FRAMEWORK 1
#define CYGPKG_NET_DRIVER_FRAMEWORK_1
#define CYGPKG_NET_STACK 1
#define CYGPKG_NET_STACK_1
#define CYGPKG_NET_STACK_INET 1
#define CYGPKG_NET_STACK_INET_1
#define CYGPKG_NET_STACK_INET6 1
#define CYGPKG_NET_STACK_INET6_1
#define CYGPKG_NET_STACK_IPSEC 0
#define CYGPKG_NET_STACK_IPSEC_0
#define INET 1
#define CYGPKG_NET_TFTP 1
#define CYGPKG_NET_TFTPD_THREAD_PRIORITY 10
#define CYGPKG_NET_TFTPD_THREAD_PRIORITY_10
#define CYGPKG_NET_TFTPD_THREAD_STACK_SIZE (CYGNUM_HAL_STACK_SIZE_TYPICAL+(8*(SEGSIZE+4)))
#define CYGSEM_NET_TFTPD_MULTITHREADED 1
#define CYGNUM_NET_TFTPD_MULTITHREADED_PORTS 1
#define CYGNUM_NET_TFTPD_MULTITHREADED_PORTS_1
#define CYGPKG_NET_DHCP 1
#define CYGOPT_NET_DHCP_DHCP_THREAD 1
#define CYGOPT_NET_DHCP_DHCP_THREAD_PARAM 0
#define CYGOPT_NET_DHCP_DHCP_THREAD_PARAM_0
#define CYGPKG_NET_DHCP_THREAD_PRIORITY 6
#define CYGPKG_NET_DHCP_THREAD_PRIORITY_6
#define CYGOPT_NET_DHCP_OPTION_HOST_NAME 1
#define CYGNUM_NET_DHCP_OPTION_HOST_NAME_LEN 60
#define CYGNUM_NET_DHCP_OPTION_HOST_NAME_LEN_60
#define CYGNUM_NET_DHCP_MIN_RETRY_TIME 100
#define CYGNUM_NET_DHCP_MIN_RETRY_TIME_100
#define CYGHWR_NET_DRIVER_ETH0 1
#define CYGHWR_NET_DRIVER_ETH0_BOOTP 1
#define CYGHWR_NET_DRIVER_ETH0_DHCP 1
#define CYGHWR_NET_DRIVER_ETH0_BOOTP_SHOW 1
#define CYGHWR_NET_DRIVER_ETH0_ADDRS_IP 192.168.0.2
#define CYGHWR_NET_DRIVER_ETH0_ADDRS_NETMASK 255.255.255.0
#define CYGHWR_NET_DRIVER_ETH0_ADDRS_BROADCAST 192.168.0.255
#define CYGHWR_NET_DRIVER_ETH0_ADDRS_GATEWAY 192.168.0.1
#define CYGHWR_NET_DRIVER_ETH0_ADDRS_SERVER 192.168.0.1
#define CYGHWR_NET_DRIVER_ETH1 1
#define CYGHWR_NET_DRIVER_ETH1_BOOTP 1
#define CYGHWR_NET_DRIVER_ETH1_DHCP 1
#define CYGHWR_NET_DRIVER_ETH1_BOOTP_SHOW 1
#define CYGHWR_NET_DRIVER_ETH1_ADDRS_IP 192.168.1.2
#define CYGHWR_NET_DRIVER_ETH1_ADDRS_NETMASK 255.255.255.0
#define CYGHWR_NET_DRIVER_ETH1_ADDRS_BROADCAST 192.168.1.255
#define CYGHWR_NET_DRIVER_ETH1_ADDRS_GATEWAY 192.168.1.1
#define CYGHWR_NET_DRIVER_ETH1_ADDRS_SERVER 192.168.1.1
#define CYGHWR_NET_DRIVER_WLAN0 1
#define CYGHWR_NET_DRIVER_WLAN0_BOOTP 1
#define CYGHWR_NET_DRIVER_WLAN0_DHCP 1
#define CYGHWR_NET_DRIVER_WLAN0_BOOTP_SHOW 1
#define CYGHWR_NET_DRIVER_WLAN0_ADDRS_IP 192.168.2.2
#define CYGHWR_NET_DRIVER_WLAN0_ADDRS_NETMASK 255.255.255.0
#define CYGHWR_NET_DRIVER_WLAN0_ADDRS_BROADCAST 192.168.2.255
#define CYGHWR_NET_DRIVER_WLAN0_ADDRS_GATEWAY 192.168.2.1
#define CYGHWR_NET_DRIVER_WLAN0_ADDRS_SERVER 192.168.2.101

#endif
