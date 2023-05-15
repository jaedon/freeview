#ifndef __DRV_WIRELESS_HOTPLUG_H__
#define __DRV_WIRELESS_HOTPLUG_H__

#include "drv_err.h"

#define WIRELESS_DEV_NAME_MAX	(12)

typedef enum
{
	DRV_WIRELESS_Extract,
	DRV_WIRELESS_Insert
}DRV_Wireless_Event_t;

typedef enum
{
	DRV_WIRELESS_PORT_NO_Port=0,
	DRV_WIRELESS_PORT_Num_1,
	DRV_WIRELESS_PORT_Num_2,
	DRV_WIRELESS_PORT_Num_MAX=255
}DRV_Wireless_portnum_t;

typedef struct wireless_info_s
{	
	unsigned long devCount;	
	char devName[WIRELESS_DEV_NAME_MAX];
	DRV_Wireless_portnum_t ePortNum;
}wireless_info_t;


typedef void (*pfnDRV_WIRELESS_NotifyFUNC)(DRV_Wireless_Event_t etWirelessEventType, void *pucWirelessInfo);

DRV_Error DRV_WIRELESS_Hotplug_Init(void);
DRV_Error DRV_WIRELESS_RegisterCallback(pfnDRV_WIRELESS_NotifyFUNC pfnNotify);

#endif

