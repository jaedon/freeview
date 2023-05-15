/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  se_uapi.h
	@brief

	Description:  									\n
	Module: Secure Handler for Umma API Module Header			\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

#ifndef __SE_UAPI_H__
#define __SE_UAPI_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#ifdef	__cplusplus
extern "C" {
#endif

#include	<hlib.h>
#include 	<di_fs.h>
#include 	<di_hdd.h>
#include 	<di_usb.h>
#include 	<di_mmc.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define	RPC_SEUAPI_Init							"seuapi.Init"
#define	RPC_SEUAPI_Mount						"seuapi.Mount"
#define	RPC_SEUAPI_Umount						"seuapi.Umount"
#define	RPC_SEUAPI_MountFtps					"seuapi.MountFtps"
#define	RPC_SEUAPI_MountSmbs					"seuapi.MountSmbs"
#define	RPC_SEUAPI_DoCommand					"seuapi.DoCommand"
#define	RPC_SEUAPI_StartFtps					"seuapi.StartFtps"
#define	RPC_SEUAPI_StartSmbs					"seuapi.StartSmbs"
#define	RPC_SEUAPI_SetPasswordSmbs				"seuapi.SetPasswordSmbs"
#define	RPC_SEUAPI_MountSmbc						"seuapi.MountSmbc"
#define	RPC_SEUAPI_UmountSmbc						"seuapi.UmountSmbc"
#define	RPC_SEUAPI_PopenCmdSmbc						"seuapi.PopenCmdSmbc"
#define	RPC_SEUAPI_Format						"seuapi.Format"
#define	RPC_SEUAPI_OpenDevMbrBuf				"seuapi.OpenDevMbrBuf"
#define	RPC_SEUAPI_DeleteAllPartitionInDevMbrBuf		"seuapi.DeleteAllPartitionInDevMbrBuf"
#define	RPC_SEUAPI_UpdatePartitionInDevMbrBuf				"seuapi.UpdatePartitionInDevMbrBuf"
#define	RPC_SEUAPI_SetPartitionInDevMbrBuf				"seuapi.SetPartitionInDevMbrBuf"
#define	RPC_SEUAPI_SyncDevMbrBuf				"seuapi.SyncDevMbrBuf"
#define	RPC_SEUAPI_CloseDevMbrBuf				"seuapi.CloseDevMbrBuf"
#define	RPC_SEUAPI_GetPartitionFromDeviceMbr				"seuapi.GetPartitionFromDeviceMbr"
#define	RPC_SEUAPI_GetMountedPartitionStatus		"seuapi.GetMountedPartitionStatus"
#define	RPC_SEUAPI_GetDevSize		"seuapi.GetDevSize"
#define	RPC_SEUAPI_GetPathSize		"seuapi.GetPathSize"
#define	RPC_SEUAPI_ExecuteCheckDisk				"seuapi.ExecuteCheckDisk"
#define	RPC_SEUAPI_EncrFormat				"seuapi.EncrFormat"
#define	RPC_SEUAPI_EncrOpen				"seuapi.EncrOpen"
#define	RPC_SEUAPI_EncrClose				"seuapi.EncrClose"
#define	RPC_SEUAPI_EncrMount				"seuapi.EncrMount"
#define	RPC_SEUAPI_EncrUmount				"seuapi.EncrUmount"

#define	RPC_SEUAPI_GetVolumeUuid				"seuapi.GetVolumeUuid"
#define	RPC_SEUAPI_SetVolumeName				"seuapi.SetVolumeName"
#define	RPC_SEUAPI_GetVolumeName				"seuapi.GetVolumeName"
#define	RPC_SEUAPI_GetFsType				"seuapi.GetFsType"
#define	RPC_SEUAPI_StartDst					"seuapi.StartDst"

#define	RPC_SEUAPI_HddInit				"seuapi.HddInit"
#define	RPC_SEUAPI_HddPwCtrl			"seuapi.HddPwCtrl"
#define	RPC_SEUAPI_HddGetEncryptionKey		"seuapi.HddGetEncryptionKey"
#define	RPC_SEUAPI_HddControlPower			"seuapi.HddControlPower"

#define	RPC_SEUAPI_UsbInit				"seuapi.UsbInit"
#define	RPC_SEUAPI_UsbPwCtrl			"seuapi.UsbPwCtrl"
#define	RPC_SEUAPI_UsbIsDeviceRemovable		"seuapi.UsbIsDeviceRemovable"


#define 	RPC_SEUMMA_onFormatProgress		"seumma.onFormatProgress"
#define 	RPC_SEUMMA_onHddNotifier			"seumma.onHddNotifier"
#define 	RPC_SEUMMA_onUsbNotifier			"seumma.onUsbNotifier"
#define 	RPC_SEUMMA_onDstNotifier				"seuapi.onDstNotifier"

#define 	RPC_SEUMMA_onHotplugNotifier				"seuapi.onHotplugNotifier"
#define	RPC_SEUAPI_HotplugInit							"seuapi.HotplugInit"

#define	RPC_SEUAPI_MountFlash                      "seuapi.MountFlash"
#define	RPC_SEUAPI_UnmountFlash                     "seuapi.UnmountFlash"


/* File Name Macros */
#define MACRO_GetDevSizeFileName(szFile, eDevIdx)		snprintf(szFile, 32, "/sys/block/sd%c/size", 'a' + eDevIdx)
#define MACRO_GetDevRmvFileName(szFile, eDevIdx)		snprintf(szFile, 32, "/sys/block/sd%c/removable", 'a' + eDevIdx)
#define MACRO_GetDevVendorFileName(szFile, eDevIdx)		snprintf(szFile, 32, "/sys/block/sd%c/device/vendor", 'a' + eDevIdx)
#define MACRO_GetDevModelFileName(szFile, eDevIdx)		snprintf(szFile, 32, "/sys/block/sd%c/device/model", 'a' + eDevIdx)

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct
{
	HUINT32		ulSemId;
	HINT32		nClientHandle;
	HINT32		nRpcHandle;

	HxHASH_t	*pstCallbackTable;
	HxList_t		*pstMonitorTable;
} SEUAPI_t;

typedef HERROR	(*pfHotplug_NotifyFUNC) (HUINT32 ulSlotIdx, HUINT32 eConnectType, void* stHpInfo); //DxHotplug_Info_t

//typedef HERROR	(*pfHotplug_NotifyFUNC)(HUINT32 eDevType, HINT32 eConnectType, HINT32 nPortNum);


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
HERROR SEUAPI_Init(void);
HERROR SEUAPI_RegisterFormatProgressCallback (DI_FS_Event_Callback_Func pfEvtCallback);
HERROR SEUAPI_FormatLists(DI_FS_FormatList_t *pstFormatList, HUINT32 nonblock);
HERROR SEUAPI_MountPartition(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *szDstMountDir, DI_FileSystemType_t eFsType);
HERROR SEUAPI_UmountPartition(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, DI_FileSystemType_t eFsType);
HERROR SEUAPI_MountPartition_Ftps(HUINT8 *szStorPath, HUINT8 *pBindPath);
HERROR SEUAPI_MountPartition_Smbs(HUINT8 *pszStorPath, HUINT8 *pszBindPath);
HERROR SEUAPI_MountFlashPartition(void);
HERROR SEUAPI_UnmountFlashPartition(void);
HERROR SEUAPI_OpenDevMbrBuf(DI_FS_StorDevIdx_t eDevIdx);
HERROR SEUAPI_DeleteAllPartitionInDevMbrBuf(DI_FS_StorDevIdx_t eDevIdx);
HERROR SEUAPI_UpdatePartitionInDevMbrBuf(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT32 ulStartUnit, HUINT32 ulEndUnit, DI_FileSystemType_t eFsType);
HERROR SEUAPI_SetPartitionInDevMbrBuf(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT64 ullPartSizeKb, DI_FileSystemType_t fsType);
HERROR SEUAPI_SyncDevMbrBuf(DI_FS_StorDevIdx_t eDevIdx);
HERROR SEUAPI_CloseDevMbrBuf(DI_FS_StorDevIdx_t eDevIdx);
HERROR SEUAPI_GetDevSize(HCHAR *pszDevName, HUINT64 *pullTotalKb);
HERROR SEUAPI_GetPathSize(HCHAR *pszPathName, HUINT64 *pullSizeKb);
HERROR SEUAPI_GetPartitionFromDeviceMbr(DI_FS_StorDevIdx_t eDevIdx, DI_FS_PartitionList_t *pstPartList);
HERROR SEUAPI_InitFormatList(DI_FS_FormatList_t *pstList);
HERROR SEUAPI_AddFormatItemToList(DI_FS_FormatList_t *pstList, DI_FS_FormatItem_t *pstItem);
HERROR SEUAPI_ClearFormatList(DI_FS_FormatList_t *pstList);
HERROR SEUAPI_GetMountedPartitionStatus(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, DI_FileSystemType_t eFsType, HUINT64 *pullTotalKbSize, HUINT64 *pullUsedKbSize, HBOOL *pbWritable);
HERROR SEUAPI_ExecuteCheckDisk(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, DI_FileSystemType_t eFsType);

HERROR SEUAPI_GetDeviceFileName (DI_FS_StorDevIdx_t eDevIdx, HUINT8 *szFileName);
HERROR SEUAPI_GetDeviceFileFullName (DI_FS_StorDevIdx_t eDevIdx, HUINT8 *szFileName);
HERROR SEUAPI_GetPartitionFileName (DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartitionIdx, HUINT8 *szFileName, HUINT8 nSizeOfFileName);
HERROR SEUAPI_GetVolumeUuid(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *pBuf, HUINT32 ulBufsize, HUINT32 *pulUuidsize);
HERROR SEUAPI_SetVolumeName(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *pBuf);
HERROR SEUAPI_GetVolumeName(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *pBuf, HUINT32 ulBufsize);
HERROR SEUAPI_GetFsType (DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, DI_FileSystemType_t *peFsType);
HERROR SEUAPI_DoCommand(HUINT8 *pszCmd, HUINT8 *pszBindPath);
HERROR SEUAPI_Start_Ftps(HUINT8 *pszCmdFtpd, HUINT8 *pszConfigFile);
HERROR SEUAPI_Start_Smbs(HUINT8 *pszCmd, HUINT8 *pszConfigFile);
HERROR SEUAPI_SetPassword_Smbs(HUINT8 *pszCmd, HUINT8 *pszNewPasswd);
HERROR SEUAPI_PopenCmd_Smbc(HUINT8 *pszCmd, HUINT8 *pszResultFile);
HERROR SEUAPI_UmountPartition_Smbc(HUINT8 *pszPath);
HERROR SEUAPI_MountPartition_Smbc(HUINT8 *pszIp, HUINT8 *pszSharedName, HUINT8 *pszPath, HUINT8 *pszId, HUINT8 *pszPassword);



#if defined(CONFIG_MW_SECURITY_FILE_SYSTEM)
HERROR SEUAPI_ENCR_Format(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, char *szPassword);
HERROR SEUAPI_ENCR_Open(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, char *szMapperDeviceName, char *szPassword);
HERROR SEUAPI_ENCR_Close(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx);
HERROR SEUAPI_ENCR_Mount(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, char *szMountDir, DI_FileSystemType_t eFsType);
HERROR SEUAPI_ENCR_Umount(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx);
#endif

//HDD
#if defined(CONFIG_PROD_DEVICE_STORAGE_HDD)
HERROR SEUAPI_RegisterDstCallback(pfnDI_HDD_DSTNotify pfnNotify);
HERROR SEUAPI_StartDst(void);
HERROR SEUAPI_HDD_Initialize(void);
HERROR SEUAPI_HDD_RegisterCallback(pfnDI_HDD_NotifyFUNC pfnNotify);
HERROR SEUAPI_HDD_PWR_Ctrl(DI_HDD_PWR OnOff);
HERROR SEUAPI_HDD_GetEncryptionKey(DI_HDD_PORT_NUM_E portnum, char *pKey);
HERROR SEUAPI_HDD_ControlPower(DI_HDD_PORT_NUM_E portnum, DI_HDD_PWR on);
#endif


// USB
#if defined(CONFIG_PROD_DEVICE_STORAGE_USB)
HERROR SEUAPI_USB_Initialize(void);
HERROR SEUAPI_USB_RegisterCallback(pfnDI_USB_NotifyFUNC pfnNotify);
HERROR SEUAPI_USB_PWR_Ctrl(DI_USB_PWR_t OnOff);
HERROR SEUAPI_USB_IsDeviceRemovable(DI_FS_StorDevIdx_t eDevIdx, HBOOL *pbRemovable);
#endif

#ifdef __cplusplus
}
#endif


HERROR SEUAPI_Hotplug_Init(void);
HERROR SEUAPI_Hotplug_RegisterCallback(pfHotplug_NotifyFUNC pfnNotify);

#endif
/* end of file */
