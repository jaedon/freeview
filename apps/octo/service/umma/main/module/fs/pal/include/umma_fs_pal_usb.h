/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  	umma_fs_pal_usb.h
	@brief 		driver interfaces for external storage

	Description: 		\n
	Module: pal/include			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef __UMMA_FS_PAL_USB_H__
#define	__UMMA_FS_PAL_USB_H__

#include <htype.h>
#include "umma_fs_pal_fs.h"

//#define ___ENABLE_MULTI_CARD_READER_MONITOR___

typedef enum
{
	eSCSI_LowSpeed		= 0,		/* DI_USB_SPEED_1M */
	eSCSI_HighSpeed,				/* DI_USB_SPEED_12M */
	eSCSI_HighSpeed_480M,				/* DI_USB_SPEED_480M */
	eSCSI_HighSpeed_5G				/* DI_USB_SPEED_5000M */
} PAL_SCSI_DeviceSpeed_e;

enum
{
	eSCSI_PWR_Stable	= 0,		/* normal */
	eSCSI_PWR_OverCurrent,			/* over current state */
} PAL_SCSI_PwrState_e;


#define PAL_SCSI_VENDOR_NAME_LEN			8
#define PAL_SCSI_MODEL_NAME_LEN				16

// PAL -> M/W
typedef void (*PAL_FS_USB_Callback_t) (HUINT32 event, HUINT32 param1, HUINT32 param2, HUINT32 param3);


HERROR	PAL_FS_USB_Initialize (void);
void	PAL_FS_USB_RegisterNotifier(PAL_FS_USB_Callback_t fsCallback);

HBOOL	PAL_FS_USB_IsDeviceRemovable (PAL_FS_StorDevIdx_t eDevIdx);
HERROR	PAL_FS_USB_StartMonitor (void);
HERROR	PAL_FS_USB_StopMonitor (void);
HERROR	PAL_FS_USB_GetCapacity (PAL_FS_StorDevIdx_t eDevIdx, HUINT64 *pullSize);
HERROR	PAL_FS_USB_GetVendorName (PAL_FS_StorDevIdx_t eDevIdx, HUINT8 *szVendor);
HERROR	PAL_FS_USB_GetModelName (PAL_FS_StorDevIdx_t eDevIdx, HUINT8 *szVendor);
HERROR	PAL_FS_USB_GetSpeedAndOcp (HUINT32 ulAttrib, HUINT32 *pulSpeed, HUINT32 *pulOcp);
HERROR	PAL_FS_USB_PowerOn (void);
HERROR	PAL_FS_USB_PowerOff (void);

#endif /* __UMMA_FS_PAL_USB_H__ */
