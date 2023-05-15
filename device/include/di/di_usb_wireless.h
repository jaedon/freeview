#ifndef __DI_USB_WIRELESS_H__
#define __DI_USB_WIRELESS_H__

#include "di_err.h"

#define DEV_MAX_NAME	(12)

typedef enum
{
	DI_USB_WIRELESS_Extract,
	DI_USB_WIRELESS_Insert
}DI_USB_Wireless_Event_t;

typedef enum
{
	DI_USB_WIRELESS_PORT_NO_Port=0,
	DI_USB_WIRELESS_PORT_Num_1,
	DI_USB_WIRELESS_PORT_Num_2,
	DI_USB_WIRELESS_PORT_Num_MAX=255
}DI_USB_Wireless_portnum_t;

typedef struct usb_wireless_info_s
{	
	unsigned long devCount;	
	char devName[DEV_MAX_NAME];
	DI_USB_Wireless_portnum_t ePortNum;
}usb_wireless_info_t;

typedef DI_ERR_CODE	(*pfnDI_USB_WIRELESS_NotifyFUNC)(DI_USB_Wireless_Event_t etWirelessEventType,void *pucWirelessInfo);
void DI_USB_WIRELESS_RegisterCallback(pfnDI_USB_WIRELESS_NotifyFUNC pfnNotify);
DI_ERR_CODE DI_USB_WIRELESS_Init(void);
#endif
