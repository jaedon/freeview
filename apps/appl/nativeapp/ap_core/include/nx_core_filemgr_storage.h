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

/******************************************************************************/
/**
 * @file	  		nx_core_filemgr_storage.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_CORE_FILEMGR_STORAGE_H__
#define __NX_CORE_FILEMGR_STORAGE_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/
#include <ondk.h>
#include <hapi.h>
#include <papi.h>
#include <oapi.h>
#include <dapi.h>
#include <apk.h>

#include <nx_common.h>
#include <nx_port.h>


/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/


/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/


/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/
typedef enum
{
	eStorageInterface_Unknown,
	eStorageInterface_SATA,
	eStorageInterface_USB,
	eStorageInterface_SD,
}NX_StorageInterface_e;

typedef	enum
{
	eStorageInterfaceVersion_Unknown,
	eStorageInterfaceVersion_USB2,
	eStorageInterfaceVersion_USB3,
}NX_StorageInterfaceVersion_e;

typedef enum
{
	eStorageType_Unknown,
	eStorageType_Internal,
	eStorageType_External,
}NX_StorageType_e;

typedef enum
{
	eStorageKind_Unknown,
	eStorageKind_HDD,
	eStorageKind_USBMemory,
	eStorageKind_SDMemory,
}NX_StorageKind_e;

typedef enum
{
	eStorageUsage_Unknown,
	eStorageUsage_PVR,
	eStorageUsage_SubPVR,
	eStorageUsage_Storage,
	eStorageUsage_NeedFormat,
}NX_StorageUsage_e;

typedef enum
{
	eStorageSort_Unknown,
	eStorageSort_InternalPVRHDD,
	eStorageSort_externalPVRHDD,
	eStorageSort_externalSubPVRHDD,
	eStorageSort_externalHDD,
	eStorageSort_UsbMemory,
	eStorageSort_SdMemory
}NX_StorageSort_e;

typedef enum
{
	eStoragePairing_Unknown,
	eStoragePairing_OK,
	eStoragePairing_Fail,
	eStoragePairing_NoInfo,
	eStoragePairing_Now,
}NX_StoragePairing_e;

typedef enum
{
	eNxStorageState_Attach,
	eNxStorageState_Detach,
	eNxStorageState_Update
}NX_StorageState_e;

typedef struct
{
	HUINT32			ulDeviceId;
	NX_StoragePairing_e 			ePairing;
	NX_StorageInterface_e			eInterface;
	NX_StorageInterfaceVersion_e	eInterfaceVersion;
	NX_StorageType_e				eType;
	NX_StorageKind_e				eKind;
	NX_StorageUsage_e				eUsage;
	NX_StorageSort_e				eSort;
	HCHAR			*pszUuid;
	HCHAR			*pszLabel;
	HUINT64			ullAvailableSize;
	HUINT64 		ullAvailablePvrSize;
	HUINT64 		ullUsedSize;
	HUINT64 		ullReservedSize;
	HUINT64 		ullTotalSize;
	HBOOL		   	bIsmount;
    HCHAR			*pszMountpath;
}NX_PhysicalStorageData_t;


/*******************************************************************/
/********************	Private Variables		********************/
/*******************************************************************/


/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/
HERROR	NX_FILEMGR_STORAGE_Init(void);
HERROR	NX_FILEMGR_STORAGE_GetPhysicalStorageListLength(HUINT32	*storageLength);
HERROR	NX_FILEMGR_STORAGE_GetPhysicalStorageDevIdList(HUINT32 **storageDevIdList, HUINT32 *storageLength);
HERROR 	NX_FILEMGR_STORAGE_GetPhysicalStorageInfoWithDevId(HUINT32 storageDevId, NX_PhysicalStorageData_t **physicalStorageInfo);
HERROR  NX_FILEMGR_STORAGE_FormatAll(HUINT32 ulDeviceId);
HERROR	NX_FILEMGR_STORAGE_SafeRemove(HUINT32 ulDeviceId);
HERROR	NX_FILEMGR_STORAGE_Recovery(HUINT32 ulDeviceId);
HERROR	NX_FILEMGR_STORAGE_SetStorageName(HUINT32 ulDeviceId, HCHAR *pszLabel);
HERROR	NX_FILEMGR_STORAGE_Check_HDD_Paired(HBOOL *pbPaired, NX_PhysicalStorageData_t **physicalStorageInfo);
HERROR	NX_FILEMGR_STORAGE_PAIRING_HDD(HUINT32 ulDeviceId, HBOOL bPaired);

#endif // __NX_CORE_FILEMGR_STORAGE_H__


