/* $Header: $ */

#ifndef DLIB_HOTPLUG_H__
#define DLIB_HOTPLUG_H__

#ifdef __cplusplus
extern "C" {
#endif

#define	DxHOTPLUG_NAME_LEN		64
#define	DxHOTPLUG_PARTITION_NUM	8
#define	DxHOTPLUG_LABEL_SIZE		16+1

typedef enum
{
	eDxHOTPLUG_NOTI_Unknown = 0,
	eDxHOTPLUG_NOTI_DevChange,	// Connect or Disconnect device
} DxHotplug_NotiType_e;

typedef enum
{
	eDxHOTPLUG_DEVTYPE_NONE	= 0,
	eDxHOTPLUG_DEVTYPE_HDD,
	eDxHOTPLUG_DEVTYPE_USB,
	eDxHOTPLUG_DEVTYPE_USB_STOR,
	eDxHOTPLUG_DEVTYPE_USB_INPUT,
	eDxHOTPLUG_DEVTYPE_USB_WIRELESS,
	eDxHOTPLUG_DEVTYPE_SD,
	eDxHOTPLUG_DEVTYPE_MIC,
	eDxHOTPLUG_DEVTYPE_HID,

	eDxHOTPLUG_DEVTYPE_ALL			= 0xFF
} DxHotplug_DevType_e;

typedef enum {
	eDxHOTPLUG_CONN=0,
	eDxHOTPLUG_DISCONN,
	eDxHOTPLUG_CONN_BY_PWR,
	eDxHOTPLUG_DISCONN_BY_PWR,

	eDxHOTPLUG_CONN_ALL	= 0xFF
} DxHotplug_ConnectType_e;


typedef void (*DxHotplugNotifier_t)(DxHotplug_NotiType_e eType, HINT32	p1, HINT32 p2, HINT32 p3);

typedef struct
{
	HUINT32		ulDeviceId;
	DxHotplug_DevType_e			eDevType;
//	DxHotplug_ConnectType_e		eConnectType;
	HINT32		nPortNum;
} DxHotplug_Info_t;

#ifdef __cplusplus
}
#endif

#endif // DLIB_HOTPLUG_H__

