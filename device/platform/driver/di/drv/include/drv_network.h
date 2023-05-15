#ifndef __DRV_NETWORK_H___
#define __DRV_NETWORK_H___

#include "di_network.h"

typedef struct
{
	HUINT32 ulDevId;
	HCHAR szName[IF_NAME_SIZE];
	DI_NETWORK_DEV_TYPE_e eDevType;
} DRV_NETWORK_DEV_INFO_t;

DRV_Error DRV_NETWORK_Init(void);
DRV_Error DRV_NETWORK_SetMacAddr(HUINT32 ulDevId, HUINT8 *pucMacAddr);
DRV_Error DRV_NETWORK_WriteMacAddrToNvram(HUINT32 ulDevId, HUINT8 *pucMacAddr);
DRV_Error DRV_NETWORK_ReadMacAddrFromNvram(HUINT32 ulDevId, HUINT8 *pucMacAddr);

#endif /* __DRV_NETWORK_H__ */

