#ifndef __DI_USB_H__
#define __DI_USB_H__

#include "htype.h"
#include "di_err.h"

#define USB_PROC_NAME_SIZE		64
#define USB_PROC_VER_SIZE		5

typedef enum
{
	DI_USB_Event_Insert,
	DI_USB_Event_Extract,
	DI_USB_Event_OverCurrent,
	DI_USB_Event_PendingSector,
	DI_USB_Event_MAX
}DI_USB_Event_t;

typedef enum
{
	DI_USB_PORT_NO_Port=0,
	DI_USB_PORT_Num_1,
	DI_USB_PORT_Num_2,
	DI_USB_PORT_Num_3,
	DI_USB_PORT_Num_4,
	DI_USB_PORT_Num_MAX=255
}DI_USB_PORT_Num_t;

typedef enum
{
	DI_USB_GPIO_LOW,
	DI_USB_GPIO_HIGH
} DI_USB_GpioStatus;

typedef enum
{
	DI_USB_PWR_OFF,
	DI_USB_PWR_ON
} DI_USB_PWR_t;

typedef enum
{
	DI_USB_SPEED_1=0,	/* 1.5Mbits/sec */
	DI_USB_SPEED_12,	/* 12Mbits/sec */
	DI_USB_SPEED_480,	/* 480Mbits/sec */
	DI_USB_SPEED_5000,	/* 5.0Gbits/sec */
	DI_USB_SPEED_UNKNOWN,
	DI_USB_SPEED_MAX = 0xff
}DI_USB_SPEED_e, DI_USB_SPEED_t;

#define DI_USB_SPEED_11         DI_USB_SPEED_12
#define DI_USB_SPEED_20         DI_USB_SPEED_480
#define DI_USB_SPEED_30         DI_USB_SPEED_5000

typedef struct usb_info_s
{	
	unsigned long devCount;	
	unsigned long devNameSize;
	char **devName; 			
	DI_USB_PORT_Num_t ePortNum;
	DI_USB_SPEED_t	eSpeed;	
	unsigned char isOverCurrent;
}usb_info_t;

typedef struct usb_state_s
{
	unsigned long devId;	
	usb_info_t usbInfo;
	unsigned char isUsed;
}usb_state_t;

typedef enum
{
	DI_USB_HOST_NO_TYPE = 0,
	DI_USB_HOST_OHCI,
	DI_USB_HOST_EHCI,
	DI_USB_HOST_UHCI,
	DI_USB_HOST_XHCI,
	DI_USB_HOST_MAX
}DI_USB_HOST_TYPE_e;

typedef struct
{	
	HUINT8 aucHostName[USB_PROC_NAME_SIZE];
	DI_USB_HOST_TYPE_e eHostTye;
	HUINT8 aucHostUsbVer[USB_PROC_VER_SIZE];
	HUINT32 unDeviceNumOfEntries;
} DI_USB_HOSTCONTROLLER_INFO_t;

typedef struct
{
	HUINT32 unDevInterfaceNum;
	HUINT8 ucDevInterfaceClass;
	HUINT8 ucDevInterfaceSubClass;
	HUINT8 ucDevInterfaceProtocol;
}DI_USB_DEVICE_INTERFACE_t;

typedef struct
{
	HUINT32 unDevConfigurationNum;
	HUINT32 unDevInterfaceNumOfEntries;
}DI_USB_DEVICE_CONFIGURATION_t;

typedef struct
{	
	HUINT32 unDevLev;
	HUINT32 unDevNum;
	HUINT8 aucDevUsbVer[USB_PROC_VER_SIZE];
	HUINT8 ucDevClass;
	HUINT8 ucDevSubClass;
	HUINT32 unDevVer;
	HUINT8 ucDevProtocol;
	HUINT32 unDevProductId;
	HUINT32 unDevVendorId;
	HUINT8 aucDevManufacturer[USB_PROC_NAME_SIZE];
	HUINT8 aucDevProduct[USB_PROC_NAME_SIZE];
	HUINT8 aucDevSerialNumber[USB_PROC_NAME_SIZE];
	HUINT32 unDevPort;
	DI_USB_SPEED_e eDevSpeed;
	HUINT8 ucDevParent;
	HUINT32 unDevMxch;
	HBOOL bDevIsSuspended;
	HBOOL bDevIsSelfPowered;
	HUINT32 unDevConfigurationNumOfEntries;
} DI_USB_DEVICE_INFO_t;

typedef DI_ERR_CODE	(*pfnDI_USB_NotifyFUNC)(DI_USB_Event_t etUsbEventType, void *pucUsbInfo);
typedef void	(*pfnDI_USB_ErrNotifyFunc)(DI_USB_Event_t etUSBEventType, const char *devname, int devname_len, unsigned long long sector);

void DI_USB_RegisterCallback(pfnDI_USB_NotifyFUNC pfnNotify);
//DI_ERR_CODE DI_USB_GetState(HUINT32 *pUsbDevCount, usb_state_t **ppUsbState);
DI_ERR_CODE DI_USB_PWR_Ctrl(DI_USB_PWR_t OnOff); 
DI_ERR_CODE DI_USB_HOST_CONTROLLER_PWR_Ctrl(DI_USB_PWR_t OnOff); /* HOST CONTROLLER */
DI_ERR_CODE DI_USB_InsertModule(void);
DI_ERR_CODE DI_USB_RemoveModule(void);

DI_ERR_CODE DI_USB_GetUsbHostCount(HUINT32 *pulCount);
DI_ERR_CODE DI_USB_GetUsbHostInfo(HUINT32 ulHostId, DI_USB_HOSTCONTROLLER_INFO_t *pstUsbHostInfo);
DI_ERR_CODE DI_USB_GetUsbDeviceInfo(HUINT32 ulHostId, HUINT32 ulDevId, DI_USB_DEVICE_INFO_t *pstUsbDevInfo);
DI_ERR_CODE DI_USB_GetUsbDevConfigurationsInfo(HUINT32 ulHostId, HUINT32 ulDevId, DI_USB_DEVICE_CONFIGURATION_t *pstUsbDevConfigusInfo);
DI_ERR_CODE DI_USB_GetUsbDevInterfacesInfo(HUINT32 ulHostId, HUINT32 ulDevId, DI_USB_DEVICE_INTERFACE_t *pstUsbDevInterfacesInfo);
DI_ERR_CODE DI_USB_Init(void);

DI_ERR_CODE DI_USB_ConnectUSB2Serial(void);
DI_ERR_CODE DI_USB_DisconnectUSB2Serial(void);
DI_ERR_CODE DI_USB_WriteUSB2Serial(HUINT8 *buffer, HINT32 length);
DI_ERR_CODE DI_USB_ReadUSB2Serial(HUINT8 *buffer, HINT32 length);
DI_ERR_CODE DI_USB_ClearUSB2Serial(void);

#endif
