/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_filemgr_storage.h
	@brief

	Description:  									\n
	Module: APPKITv2			\n

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

#ifndef __APK_FILEMGR_STORAGE_H__
#define __APK_FILEMGR_STORAGE_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define APPKIT_V2_STORAGE 1

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

HERROR	APK_FILEMGR_STORAGE_Init(void);
void	APK_FILEMGR_STORAGE_RegisterNotifier(DxStorageNotifier_t pfnObjectHandler);
HERROR	APK_FILEMGR_STORAGE_Format(HUINT32 ulDeviceId, const HCHAR *pszVolume);
HERROR	APK_FILEMGR_STORAGE_StartDST(HUINT32 ulDeviceId);
HERROR	APK_FILEMGR_STORAGE_SetDefaultHdd(const HCHAR *pszStorageId);
HERROR	APK_FILEMGR_STORAGE_DisablePVRHdd(const HCHAR *pszStorageId);
HERROR	APK_FILEMGR_STORAGE_PairingHdd(const HCHAR *pszStorageId);
HERROR	APK_FILEMGR_STORAGE_UnPairingHdd(const HCHAR *pszStorageId);
HERROR	APK_FILEMGR_STORAGE_SetName(const HCHAR *pszStorageId, const HCHAR *pszName);
HERROR	APK_FILEMGR_STORAGE_SafeRemove(HUINT32 ulDeviceId, HBOOL bForced);
HERROR	APK_FILEMGR_STORAGE_GetList(HUINT32 ulFilter, DxStorage_Info_t **pastInfoList, HUINT32 *pulDevNum);
HERROR	APK_FILEMGR_STORAGE_FreeList(DxStorage_Info_t *pastInfoList, HUINT32 ulDevNum);
HERROR	APK_FILEMGR_STORAGE_GetSize(HUINT32 ulDeviceId, HUINT64 *pullTotalKbSize, HUINT64 *pullAvailableKbSize, HUINT64 *pullUsedKbSize, HUINT64 *pullReservedKbSize, HUINT64 *pullAvailablePvrKbSize);
HERROR APK_FILEMGR_STORAGE_GetPvrReservedSize(HUINT32 ulDeviceId, HUINT64 *pullReservedKbSize);
HERROR	APK_FILEMGR_STORAGE_CheckUsbSpeed(const HCHAR *pszStorageId);

const HCHAR* APK_FILEMGR_STORAGE_GetDevTypeString(const DxStorage_DevType_e	eDevType);
const HCHAR* APK_FILEMGR_STORAGE_GetFsString(const DxStorage_FsType_e	eFsType);

#endif
/* end of file */
