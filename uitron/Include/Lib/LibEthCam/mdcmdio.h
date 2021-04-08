#ifndef _ETHCAM_SMI_H
#define _ETHCAM_SMI_H
#include "Type.h"

void smiInit(UINT32 MDC_Gpio, UINT32 MDIO_Gpio);
INT32 smiWrite(UINT32 phyad, UINT32 regad, UINT32 data);
INT32 smiRead(UINT32 phyad, UINT32 regad, UINT32 * data);
INT32 smiReadBit(UINT32 phyad, UINT32 regad, UINT32 bit, UINT32 * pdata);
INT32 smiWriteBit(UINT32 phyad, UINT32 regad, UINT32 bit, UINT32 data);

#endif //_ETHCAM_SMI_H
