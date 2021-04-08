#ifndef _AUDPROC_INT_H
#define _AUDPROC_INT_H
/*
    Copyright (c) 2014~  Novatek Microelectronics Corporation

    @file audproc.h

    @version

    @date
*/
#include <cyg/infra/diag.h>

#define AUDPROC_DEBUG_MSG      0

#if (AUDPROC_DEBUG_MSG == 1)
#define AUDPROC_DBG(fmtstr, args...) diag_printf("eCooooos: %s() " fmtstr, __func__, ##args)
#else //release
#define AUDPROC_DBG(...)
#endif

#define AUDPROC_PRINT(fmtstr, args...) diag_printf("eCooooos: %s() " fmtstr, __func__, ##args)
#define AUDPROC_ERR(fmtstr, args...) diag_printf("eCooooos: ERR %s() " fmtstr, __func__, ##args)

#endif /* _AUDPROC_INT_H  */

