/**************************************************************
*	@file	homma_interop.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			HOMMA / interoperation supporting instead
*					of using APK
**************************************************************/

/*
* ⓒ 2011-2012 Humax Co., Ltd.
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
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

/* ------------------------------------------------------------- //
 * Author : tjjang
 * Description : This is for replacing apk methods
 *  - HOMMA_INTEROP prefix is used for external methods
 *  - s_HOMMA_INTEROP prefix is used to represent static methods
 */

#include <homma_interop.h>

#ifdef HOMMA_USE_APPKIT_DIRECTLY
#else
#include <uapi.h>
#endif

/*
 * methods
 *  - static & external
 */

/* ############################################################################
  __  _  _
 / _|(_)| |
| |_  _ | |  ___  _ __ ___    __ _  _ __
|  _|| || | / _ \| '_ ` _ \  / _` || '__|
| |  | || ||  __/| | | | | || (_| || |
|_|  |_||_| \___||_| |_| |_| \__, ||_|
                              __/ |
                             |___/
 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx  filemgr storage  xxxxxxxxx
############################################################################ */

// unwilling to use apps/octo/appkit/
#ifdef HOMMA_USE_APPKIT_DIRECTLY

HERROR HOMMA_INTEROP_FILEMGR_STORAGE_Init(void)
{
	return APK_FILEMGR_STORAGE_Init();
}

const HCHAR* HOMMA_INTEROP_FILEMGR_STORAGE_GetDevTypeString(const DxStorage_DevType_e	eDevType)
{
	return APK_FILEMGR_STORAGE_GetDevTypeString(eDevType);
}

const HCHAR* HOMMA_INTEROP_FILEMGR_STORAGE_GetFsString(const DxStorage_FsType_e	eFsType)
{
	return APK_FILEMGR_STORAGE_GetFsString(eFsType);
}

HERROR HOMMA_INTEROP_FILEMGR_STORAGE_GetList(HUINT32 ulFilter, DxStorage_Info_t **pastInfoList, HUINT32 *pulDevNum)
{
	return APK_FILEMGR_STORAGE_GetList(ulFilter, pastInfoList, pulDevNum);
}

HERROR HOMMA_INTEROP_FILEMGR_STORAGE_FreeList(DxStorage_Info_t *pastInfoList, HUINT32 ulDevNum)
{
	return APK_FILEMGR_STORAGE_FreeList(pastInfoList, ulDevNum);
}

void HOMMA_INTEROP_FILEMGR_STORAGE_RegisterNotifier(DxStorageNotifier_t pfnObjectHandler)
{
	APK_FILEMGR_STORAGE_RegisterNotifier(pfnObjectHandler);
}

#else // #ifdef HOMMA_USE_APPKIT_DIRECTLY else

//APK_FILEMGR_STORAGE_Init();
HERROR HOMMA_INTEROP_FILEMGR_STORAGE_Init(void)
{
	return UAPI_FS_Init();
}

// APK_FILEMGR_STORAGE_GetDevTypeString(pstInfo->eDevType));
const HCHAR* HOMMA_INTEROP_FILEMGR_STORAGE_GetDevTypeString(const DxStorage_DevType_e	eDevType)
{
	return UAPI_FS_GetDevTypeString(eDevType);
}

// APK_FILEMGR_STORAGE_GetFsString(pstInfo->astPartitionInfos[i].eFsType));
const HCHAR* HOMMA_INTEROP_FILEMGR_STORAGE_GetFsString(const DxStorage_FsType_e	eFsType)
{
	return UAPI_FS_GetFsString(eFsType);
}

// APK_FILEMGR_STORAGE_GetList(storageFilter, &devList, &devCnt);
HERROR HOMMA_INTEROP_FILEMGR_STORAGE_GetList(HUINT32 ulFilter, DxStorage_Info_t **pastInfoList, HUINT32 *pulDevNum)
{
	return UAPI_FS_GetList(ulFilter, pastInfoList, pulDevNum);
}

// APK_FILEMGR_STORAGE_FreeList(devList, devCnt);
HERROR HOMMA_INTEROP_FILEMGR_STORAGE_FreeList(DxStorage_Info_t *pastInfoList, HUINT32 ulDevNum)
{
	return UAPI_FS_FreeList(pastInfoList, ulDevNum);
}

// APK_FILEMGR_STORAGE_RegisterNotifier(__system_storage_notifier);
void HOMMA_INTEROP_FILEMGR_STORAGE_RegisterNotifier(DxStorageNotifier_t pfnObjectHandler)
{
	UAPI_FS_SetNotifier(pfnObjectHandler);
}

#endif // #ifdef HOMMA_USE_APPKIT_DIRECTLY endif

