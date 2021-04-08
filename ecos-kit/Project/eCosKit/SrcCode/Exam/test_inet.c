#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cyg/kernel/kapi.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <net/if.h>
#include <dhcp.h>
#include <network.h>
#include <cyg/nvtipc/NvtIpcAPI.h>
#include "Type.h"
#include <cyg/hfs/hfs.h>
#include <cyg/lviewd/lviewd.h>
#include <cyg/infra/diag.h>

#define TEST_WIFI                  0
#define INIT_INET_WITH_STATIC_IP   0
#define TEST_DUMPNETIF_CMD         0
#define TEST_FTPD                  0

static cyg_thread testinet_thread_s; /* space for thread object */
static char testinet_stack[8192]; /* 8K stack space for the thread */
static cyg_handle_t testinet_thread;  /* now the handles for the thread */

/* and now variable (prototype) for the procedure which is the thread itself */
cyg_thread_entry_t testinet_program;

extern int dump_all_netif_hwaddr(void);
extern int dump_netif_hwaddr(char *ifname);
//aad for compile warning
extern void test_ftpd(void);

#if TEST_DUMPNETIF_CMD
#if 1
#include <cyg/infra/mainfunc.h>
MAINFUNC_ENTRY(dumpnetif,argc,argv)
#else
int main(int argc, char **argv)
#endif
{
    if (argc < 2)
        return dump_all_netif_hwaddr();
    else
        return dump_netif_hwaddr(argv[1]);
}
#endif

void testinet_program(cyg_addrword_t data)
{
#if TEST_WIFI
    extern void test_wifi(void);
    extern int runmain_as_pthread(char *cmdstr);
    test_wifi();

#if 0
    cyg_thread_delay(40000);
    printf("iwconfig eth1 essid dlink\n");
    runmain_as_pthread("iwconfig eth1 essid dlink");
#endif

#endif

   dhcp_set_hostname("nvtdsc01");
#if INIT_INET_WITH_STATIC_IP
   init_all_network_interfaces_statically();
#else
   init_all_network_interfaces();
#endif

#if TEST_DUMPNETIF_CMD
   dump_netif_hwaddr("eth0");
   dump_netif_hwaddr("eth1");
#endif

#if TEST_FTPD
   test_ftpd();
#endif

   cyg_thread_exit();
}

void test_inet(void)
{
    //we have to start a thread to do actual test job and not to do here directly

    cyg_thread_create(4, testinet_program, (cyg_addrword_t) 0,
            "Testinet Thread", (void *) testinet_stack, 8192,
            &testinet_thread, &testinet_thread_s);

    cyg_thread_resume(testinet_thread);

}
