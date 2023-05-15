/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_download.h
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

#ifndef ___APK_DOWNLOAD_H___
#define ___APK_DOWNLOAD_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <dlib.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef void (*APK_DOWNLOAD_Notifier_t) (
											HUINT32 ulDownloadId,
											DxDownload_Type_e eType,
											DxDownload_Event_e eEvent,
											DxDownload_NotifierData_t *punNotifyData,
											void *pvUserData
											);


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

HERROR 		APK_DOWNLOAD_Init (void);
HERROR 		APK_DOWNLOAD_RegisterNotifier (APK_DOWNLOAD_Notifier_t pfNotifier, void *pvUserData);
HERROR 		APK_DOWNLOAD_UnregisterNotifier (APK_DOWNLOAD_Notifier_t pfNotifier, void *pvUserData);
Handle_t 	APK_DOWNLOAD_Probe (DxDownload_Start_t *punStart);
Handle_t 	APK_DOWNLOAD_Start (Handle_t hDownload, DxDownload_Start_t *punStart);
HERROR 		APK_DOWNLOAD_Stop (Handle_t hDownload);
HERROR 		APK_DOWNLOAD_Pause (Handle_t hDownload);
HERROR 		APK_DOWNLOAD_Resume (Handle_t hDownload);
HERROR 		APK_DOWNLOAD_GetStatus (Handle_t hDownload, DxDownload_Status_t *pstStatus);
HERROR 		APK_DOWNLOAD_GetBufferingChunkInfo (Handle_t hDownload, HUINT32 *pnChunks, HUINT32 **ppulStartOffsets, HUINT32 **ppulEndOffsets);
HERROR 		APK_DOWNLOAD_GetSeekableRangeInfo (Handle_t hDownload, HUINT32 *nChunks, HUINT64 **ppullSeekableStart, HUINT64 **ppullSeekableEnd);
HERROR 		APK_DOWNLOAD_GetRemainBufferingTime (Handle_t hDownload, HUINT32 *punRemainBufferingTime);
HERROR 		APK_DOWNLOAD_GetBufferingPercent (Handle_t hDownload, HUINT32 *pulBufferPercent);
HERROR 		APK_DOWNLOAD_GetBufferedBytes (Handle_t hDownload, HINT64 *pllBufferedBytes);
HERROR 		APK_DOWNLOAD_GetRemainingBytes (Handle_t hDownload, HINT64 *pullRemainingBytes);
HERROR 		APK_DOWNLOAD_GetComponentNum (Handle_t hDownload, OxDownload_Component_e eCompType, HUINT32 *pulCompNum);
HERROR		APK_DOWNLOAD_GetComponent (Handle_t hDownload, OxDownload_Component_e eCompType, HINT32 nCompIndex, OxDownload_Component_t *pstComponet);
HERROR		APK_DOWNLOAD_GetComponentIndex (Handle_t hDownload, OxDownload_Component_e eCompType, HINT32 *pnCompIndex, HINT32 *pnCompSubIndex);
HERROR		APK_DOWNLOAD_GetAvailabilityStartTime (Handle_t hDownload, HUINT32 pnSzBuffer, HUINT8 *pszTimeString);


#endif // ___APK_DOWNLOAD_H___
/* end of file */
