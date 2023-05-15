/**
	@file     	pal_ext_stor.c
	@brief    	Get SCSI DISK Connection Status

	Description: 	monitor external storage connection.	\n
	Module: 		pal/fs			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <pthread.h>
#include <sys/types.h>

#include "umma_fs_pal_fs.h"
//#include "pal_pvr.h"
#include "umma_fs_pal_usb.h"
#include	"se_uapi.h"

#include "vkernel.h"
#include "htype.h"

#include "di_usb.h"
#include "drv_hotplug.h"

#ifdef CONFIG_DEBUG
#define	_DEBUG_
//#define	_DEBUG_ENTER_EXIT_
#endif

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/




#define SCSI_ON 						0x0001
#define SCSI_OFF 						0x0000
#define SCSI_CONN_CB					0x10
#define SCSI_DISCONN_CB 				0x100

#define SCSI_OCP_MASK					0xFFFF0000
#define SCSI_SPEED_MASK					0x0000FFFF
#define SCSI_OCP_BIT_OFFSET				16

/* platform specific definitions */
#define EXT_SCSI_SECTOR_SIZE			512

#define EXT_SCSI_RETRY_COUNT			10

#define EXT_SCSI_REDUNDANT_CODE			0x20

#define PAL_SCSI_PLUGGING_CHECK_SEC		2

/*******************************************************************/
/********************      Type Definition         *************************/
/*******************************************************************/

// DD -> PAL callback
typedef void (*PAL_FS_USB_Event_CbFnc)(void *pvParam);

/*******************************************************************/
/********************      Static Variables         *************************/
/*******************************************************************/

// DI -> PAL Callback Functions
#if 0
static PAL_FS_USB_Event_CbFnc	s_stEXTDisconnectEventCallback;
static PAL_FS_USB_Event_CbFnc	s_stEXTOverCurrentCallback;
#endif // #if 0

// PAL -> MW Callback Function
static PAL_FS_USB_Callback_t				s_pfPalUsb_NotifierCbFunc;

// static int 					scsi_continue = SCSI_ON;

static unsigned char		scsi_dev_list[ePAL_FS_DEV_MAX];


/*******************************************************************/
/********************      Functions         *************************/
/*******************************************************************/

#define _____INTERNAL_FUNCTIONS_____

#define _____PAL2MW_CALLBACK_RELATED_____

STATIC void umma_fs_pal_usb_ExecuteMwCallback (HUINT32 event, HUINT32 param1, HUINT32 param2, HUINT32 param3)
{
	if (s_pfPalUsb_NotifierCbFunc != NULL)
	{
		(s_pfPalUsb_NotifierCbFunc)(event, param1, param2, param3);
	}
	else
	{
		HxLOG_Critical("\n\n");
	}
}

STATIC HUINT32 umma_fs_pal_usb_getSectorNumer (PAL_FS_StorDevIdx_t eDevIdx)
{
	int			 nFileDsc, nRet;
	HUINT32		 ulSectors = 0;
	char		 szFileName[32], szNumString[16];

	// szFileName : /sys/block/sda/size, /sys/block/sdb/size
	MACRO_GetDevSizeFileName (szFileName, eDevIdx);
	nFileDsc = open (szFileName, O_RDONLY);
	if (nFileDsc >= 0)
	{
		nRet = read (nFileDsc, szNumString, sizeof(char) * 16);
		ulSectors = (nRet > 0) ? atoi (szNumString) : 0;
		close (nFileDsc);
	}

	return ulSectors;
}

#define _____DI2PAL_CALLBACK_RELATED_____

// DI2PAL Callback Function
STATIC HERROR umma_fs_pal_usb_GetDevIndexByDevName (char *szDevName, PAL_FS_StorDevIdx_t *peDevIdx)
{
	HUINT32		 ulIndex;
	HUINT8		 szDevNode[8];

	if (szDevName == NULL || peDevIdx == NULL)			{ return ERR_FAIL; }

	for (ulIndex = ePAL_FS_DEV_sda; ulIndex < ePAL_FS_DEV_MAX; ulIndex++)
	{
		PAL_FS_GetDeviceFileName (ulIndex, szDevNode);
		if (NULL != strstr(szDevName, szDevNode))
		{
			*peDevIdx = (PAL_FS_StorDevIdx_t)ulIndex;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

STATIC HERROR umma_fs_pal_usb_DiEventInsert (usb_info_t *pstDiUsbInfo)
{
	HUINT32				 ulEvent, ulParam1, ulParam2, ulParam3;
	HINT32				 nIndex;
	PAL_FS_StorDevIdx_t	 eDevIdx;
	HERROR				 hErr;

	if (pstDiUsbInfo->devCount > 0)
	{
		for (nIndex = 0; nIndex < (HINT32)pstDiUsbInfo->devCount; nIndex++)
		{
			hErr = umma_fs_pal_usb_GetDevIndexByDevName (pstDiUsbInfo->devName[nIndex], &eDevIdx);
			if (hErr == ERR_OK)
			{
				// Notification : USB device inserted
				ulEvent 	= PAL_FS_EVENT_USB_SCSI_ADDED;
				ulParam1	= (HUINT32)eDevIdx;
				ulParam2	= (HUINT32)pstDiUsbInfo->ePortNum;
				ulParam3	= (HUINT32)((pstDiUsbInfo->isOverCurrent << SCSI_OCP_BIT_OFFSET) | (pstDiUsbInfo->eSpeed & SCSI_SPEED_MASK));
				umma_fs_pal_usb_ExecuteMwCallback (ulEvent, ulParam1, ulParam2, ulParam3);

				scsi_dev_list[eDevIdx] = (unsigned char)eDevIdx;
			}
		}

		// Notification : USB connected
		ulEvent 	= PAL_FS_EVENT_USB_CONNECTED;
		ulParam1	= (HUINT32)0;
		ulParam2	= (HUINT32)pstDiUsbInfo->ePortNum;
		ulParam3	= (HUINT32)0;
		umma_fs_pal_usb_ExecuteMwCallback (ulEvent, ulParam1, ulParam2, ulParam3);
	}

	return ERR_OK;
}

STATIC HERROR umma_fs_pal_usb_DiEventExtract (usb_info_t *pstDiUsbInfo)
{
	HUINT32				 ulEvent, ulParam1, ulParam2, ulParam3;
	HINT32				 nIndex;
	PAL_FS_StorDevIdx_t	 eDevIdx;
	HERROR				 hErr;

	if (pstDiUsbInfo->devCount > 0)
	{
		for (nIndex = (HINT32)pstDiUsbInfo->devCount - 1; nIndex >= 0; nIndex--)
		{
			hErr = umma_fs_pal_usb_GetDevIndexByDevName (pstDiUsbInfo->devName[nIndex], &eDevIdx);
			if (hErr == ERR_OK)
			{
				// Notification : USB device removed
				ulEvent 	= PAL_FS_EVENT_USB_SCSI_REMOVED;
				ulParam1	= (HUINT32)eDevIdx;
				ulParam2	= (HUINT32)pstDiUsbInfo->ePortNum;
				ulParam3	= (HUINT32)0;
				umma_fs_pal_usb_ExecuteMwCallback (ulEvent, ulParam1, ulParam2, ulParam3);

				scsi_dev_list[eDevIdx] = 0xFF;
			}
		}

		// Notification : USB disconnected
		ulEvent 	= PAL_FS_EVENT_USB_DISCONNECTED;
		ulParam1	= (HUINT32)0;
		ulParam2	= (HUINT32)pstDiUsbInfo->ePortNum;
		ulParam3	= (HUINT32)0;
		umma_fs_pal_usb_ExecuteMwCallback (ulEvent, ulParam1, ulParam2, ulParam3);
	}

	return ERR_OK;
}

STATIC HERROR umma_fs_pal_usb_DiEventOverCurrent (usb_info_t *pstDiUsbInfo)
{
	HUINT32				 ulEvent, ulParam1, ulParam2, ulParam3;

	// Notification : USB disconnected
	ulEvent 	= PAL_FS_EVENT_USB_OVERCURRENT;
	ulParam1	= (HUINT32)0;
	ulParam2	= (HUINT32)pstDiUsbInfo->ePortNum;
	ulParam3	= (HUINT32)((pstDiUsbInfo->isOverCurrent << SCSI_OCP_BIT_OFFSET) | (pstDiUsbInfo->eSpeed & SCSI_SPEED_MASK));
	umma_fs_pal_usb_ExecuteMwCallback (ulEvent, ulParam1, ulParam2, ulParam3);

	return ERR_OK;
}

STATIC DI_ERR_CODE umma_fs_pal_usb_EventCallback (DI_USB_Event_t etUsbEventType, void *pvUsbInfo)
{
	usb_info_t			*pstDiUsbInfo = (usb_info_t *)pvUsbInfo;
	HERROR				 hErr;

	if (pstDiUsbInfo == NULL)			{ return DI_ERR_ERROR; }

	// start propagation to notice USB storage connection changes
	switch (etUsbEventType)
	{
	case DI_USB_Event_Insert:
		hErr = umma_fs_pal_usb_DiEventInsert (pstDiUsbInfo);
		break;

	case DI_USB_Event_Extract:
		hErr = umma_fs_pal_usb_DiEventExtract (pstDiUsbInfo);
		break;

	case DI_USB_Event_OverCurrent:
		hErr = umma_fs_pal_usb_DiEventOverCurrent (pstDiUsbInfo);
		break;

	default:
		HxLOG_Critical("\n\n\n");
		hErr = ERR_FAIL;
		break;
	}

	return (hErr == ERR_OK) ? DI_ERR_OK : DI_ERR_ERROR;
}

#define _____GLOBAL_FUNCTIONS_____

HERROR PAL_FS_USB_Initialize (void)
{
	VK_memset (&scsi_dev_list, 0xFF, sizeof(unsigned char) * ePAL_FS_DEV_MAX);

#ifndef WIN32
	SEUAPI_USB_Initialize();
	SEUAPI_USB_RegisterCallback(umma_fs_pal_usb_EventCallback);
	SEUAPI_USB_PWR_Ctrl(DI_USB_PWR_ON);
#endif

	return ERR_OK;
}

void PAL_FS_USB_RegisterNotifier (PAL_FS_USB_Callback_t fsCallback)
{
	if (fsCallback != NULL)
	{
		s_pfPalUsb_NotifierCbFunc = fsCallback;
	}
	else
	{
		HxLOG_Critical("\n\n\n");
	}
}

HBOOL PAL_FS_USB_IsDeviceRemovable (PAL_FS_StorDevIdx_t eDevIdx)
{
	HERROR	 hErr;
	HBOOL	bRemovable = FALSE;

	hErr = SEUAPI_USB_IsDeviceRemovable(eDevIdx, &bRemovable);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error!!\n");
	}
	HxLOG_Debug("bRemovable : %d\n", bRemovable);

	return	bRemovable;
}

HERROR PAL_FS_USB_GetCapacity (PAL_FS_StorDevIdx_t eDevIdx, HUINT64 *pullSize)
{
	HUINT64			 ullSectorNum;

	if (pullSize == NULL)				{ return ERR_FAIL; }

	ullSectorNum = (HUINT64)umma_fs_pal_usb_getSectorNumer (eDevIdx);
	*pullSize = (HUINT64)(ullSectorNum * EXT_SCSI_SECTOR_SIZE);

	return ERR_OK;
}

HERROR	PAL_FS_USB_GetVendorName (PAL_FS_StorDevIdx_t eDevIdx, HUINT8 *szVendor)
{
#ifndef WIN32
	int			 nFileDsc, nRet;
	int			 nCount, nStrLen;
	char		 szFileName[48], szString[PAL_SCSI_VENDOR_NAME_LEN + 1];

	if (szVendor == NULL)			{ return ERR_FAIL; }

	MACRO_GetDevVendorFileName (szFileName, eDevIdx);
	for (nCount = 0; nCount < EXT_SCSI_RETRY_COUNT; nCount++)
	{
		nFileDsc = open (szFileName, O_RDONLY);
		if (nFileDsc != -1)
		{
			VK_memset (szString, 0, PAL_SCSI_VENDOR_NAME_LEN + 1);

			nRet = read (nFileDsc, szString, PAL_SCSI_VENDOR_NAME_LEN);
			if (nRet > 0)
			{
				szString[PAL_SCSI_VENDOR_NAME_LEN] = '\0';
				for (nStrLen = strlen(szString) - 1; nStrLen > 0; nStrLen--)
				{
					if (szString[nStrLen] > EXT_SCSI_REDUNDANT_CODE)			{ break; }
					szString[nStrLen] = '\0';
				}

				strncpy (szVendor, szString, PAL_SCSI_VENDOR_NAME_LEN);
			}

			close (nFileDsc);
			return (szString[0] != '\0') ? ERR_OK : ERR_FAIL;
		}

		VK_TASK_Sleep (10);
	}

	return ERR_FAIL;
#else
	strcpy (szVendor, "Unknown");
	return ERR_OK;
#endif
}

HERROR	PAL_FS_USB_GetModelName (PAL_FS_StorDevIdx_t eDevIdx, HUINT8 *szModel)
{
#ifndef WIN32
	int 		 nFileDsc, nRet;
	int 		 nCount, nStrLen;
	char		 szFileName[48], szString[PAL_SCSI_MODEL_NAME_LEN + 1];

	if (szModel == NULL)			{ return ERR_FAIL; }

	MACRO_GetDevModelFileName (szFileName, eDevIdx);
	for (nCount = 0; nCount < EXT_SCSI_RETRY_COUNT; nCount++)
	{
		nFileDsc = open (szFileName, O_RDONLY);
		if (nFileDsc != -1)
		{
			VK_memset (szString, 0, PAL_SCSI_MODEL_NAME_LEN + 1);

			nRet = read (nFileDsc, szString, PAL_SCSI_MODEL_NAME_LEN);
			if (nRet > 0)
			{
				szString[PAL_SCSI_MODEL_NAME_LEN] = '\0';
				for (nStrLen = strlen(szString) - 1; nStrLen > 0; nStrLen--)
				{
					if (szString[nStrLen] > EXT_SCSI_REDUNDANT_CODE)			{ break; }
					szString[nStrLen] = '\0';
				}

				strncpy (szModel, szString, PAL_SCSI_MODEL_NAME_LEN);
			}

			close (nFileDsc);
			return (szString[0] != '\0') ? ERR_OK : ERR_FAIL;
		}

		VK_TASK_Sleep (10);
	}

	return ERR_FAIL;
#else
	strcpy (szModel, "Storage");
	return ERR_OK;
#endif
}

HERROR PAL_FS_USB_GetSpeedAndOcp (HUINT32 ulAttrib, HUINT32 *pulSpeed, HUINT32 *pulOcp)
{
	if (pulSpeed != NULL)
	{
		switch (ulAttrib & SCSI_SPEED_MASK)
		{
		case DI_USB_SPEED_1:
			*pulSpeed = eSCSI_LowSpeed;
			break;

		case DI_USB_SPEED_12:
			*pulSpeed = eSCSI_HighSpeed;
			break;

		case DI_USB_SPEED_480:
			*pulSpeed = eSCSI_HighSpeed_480M;
			break;

		case DI_USB_SPEED_5000:
			*pulSpeed = eSCSI_HighSpeed_5G;
			break;

		default:
			*pulSpeed = eSCSI_LowSpeed;
			break;
		}
	}

	if (pulOcp != NULL)
	{
		if (((ulAttrib & SCSI_OCP_MASK) >> SCSI_OCP_BIT_OFFSET) == 1)
		{
			*pulOcp = eSCSI_PWR_OverCurrent;
		}
		else
		{
			*pulOcp = eSCSI_PWR_Stable;
		}
	}

	return ERR_OK;
}

HERROR PAL_FS_USB_PowerOn (void)
{
#ifndef WIN32
	DI_ERR_CODE 	 eDiErr;
	eDiErr = SEUAPI_USB_PWR_Ctrl(DI_USB_PWR_ON);
	return (eDiErr == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
#endif
	return ERR_OK;
}

HERROR PAL_FS_USB_PowerOff (void)
{
#ifndef WIN32
	DI_ERR_CODE 	 eDiErr;
	eDiErr = SEUAPI_USB_PWR_Ctrl(DI_USB_PWR_OFF);
	return (eDiErr == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
#endif
	return ERR_OK;
}

