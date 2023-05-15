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
 * @file	  		nx_port_filemgr_storage.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#include <nx_port.h>


HERROR NX_PORT_FILEMGR_STORAGE_GetSize(HUINT32 ulDeviceId, HUINT64 *pullTotalKbSize, HUINT64 *pullAvailableKbSize, HUINT64 *pullUsedKbSize, HUINT64 *pullReservedKbSize, HUINT64 *pullAvailablePvrKbSize)
{
	return APK_FILEMGR_STORAGE_GetSize(ulDeviceId, pullTotalKbSize, pullAvailableKbSize, pullUsedKbSize, pullReservedKbSize, pullAvailablePvrKbSize);
}

HERROR NX_PORT_FILEMGR_STORAGE_FormatAll(HUINT32 ulDeviceId)
{
	return APK_FILEMGR_STORAGE_Format(ulDeviceId, "");
}

HERROR NX_PORT_FILEMGR_STORAGE_SafeRemove(HUINT32 ulDeviceId)
{
	return APK_FILEMGR_STORAGE_SafeRemove(ulDeviceId, FALSE);
}

HERROR NX_PORT_FILEMGR_STORAGE_Recovery(HUINT32 ulDeviceId)
{
	return APK_FILEMGR_STORAGE_StartDST(ulDeviceId);
}

HERROR NX_PORT_FILEMGR_STORAGE_SetStorageName(HCHAR *pszUuid, HCHAR *pszLabel)
{
	return APK_FILEMGR_STORAGE_SetName(pszUuid, pszLabel);
}

HERROR NX_PORT_FILEMGR_STORAGE_DisablePVRHDD(HCHAR *pszUuid)
{
	return APK_FILEMGR_STORAGE_DisablePVRHdd(pszUuid);
}

HERROR NX_PORT_FILEMGR_STORAGE_PairingHdd(HCHAR *pszUuid)
{
	return APK_FILEMGR_STORAGE_PairingHdd(pszUuid);
}

HERROR NX_PORT_FILEMGR_STORAGE_UnPairingHdd(HCHAR *pszUuid)
{
	return APK_FILEMGR_STORAGE_UnPairingHdd(pszUuid);
}

HERROR NX_PORT_FILEMGR_STORAGE_GetStorageList(DxStorage_Info_t **ppstInfoList, HUINT32 *pulDevListNum)
{
	return APK_FILEMGR_STORAGE_GetList(eDxSTORAGE_FILTER_All, ppstInfoList, pulDevListNum);
}

HERROR NX_PORT_FILEMGR_STORAGE_FreeStorageList(DxStorage_Info_t *pstInfoList, HUINT32 ulDevListNum)
{
	return APK_FILEMGR_STORAGE_FreeList(pstInfoList, ulDevListNum);
}

void	NX_PORT_FILEMGR_STORAGE_RegisterNotifier(DxStorageNotifier_t pfnObjectHandler)
{
	APK_FILEMGR_STORAGE_RegisterNotifier(pfnObjectHandler);
}

/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

