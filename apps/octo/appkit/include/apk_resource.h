/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_resource.h
	@brief

	Description:  									\n
	Module: appkit/resource						\n

	Copyright (c) 2014 HUMAX Co., Ltd.				\n
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
#ifndef ____APPKIT_RESOURCE__H___
#define ____APPKIT_RESOURCE__H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <oapi.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Member Functions     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      API Functions     ********************/
/*******************************************************************/
extern HERROR	APK_RSC_Init (void);
extern HERROR	APK_RSC_SetListener (OxRsc_Notifier_t pfNotifier, void *pvUserData);
extern HERROR	APK_RSC_ResetListener (OxRsc_Notifier_t pfNotifier, void *pvUserData);
extern HERROR	APK_RSC_GetInitialInfo (OxRsc_InitialInfo_t *pstInitInfo);
extern HERROR	APK_RSC_GetResourceId (HUINT32 ulActionId, RxRscType_e eRscType, HUINT32 *pulRscId);
extern HERROR   APK_RSC_GetResourceNumber( RxRscType_e eRscType, HUINT32 *pulRscNum);
extern Handle_t	APK_RSC_MakeResourceRequestHandle (HUINT32 ulActionId, HUINT32 ulActionPriority, HBOOL bUseVirtualHandle);
extern Handle_t	APK_RSC_DuplicateRequestHandle (RxRscReqHandle_t hRscReq);
extern HERROR	APK_RSC_AddRemovableActionIdToRequestHandle (Handle_t hRscReq, HUINT32 ulActionId, HUINT32 ulActionPriority);
extern HERROR	APK_RSC_AddIgnorableActionIdToRequestHandle (Handle_t hRscReq, HUINT32 ulActionId);
extern HERROR	APK_RSC_AddRequiredResourceToRequestHandle (Handle_t hRscReq, RxRscType_e eRscType, HUINT32 ulResourceId, HBOOL bSharable, RxUsageType_e eUsageType);
extern HERROR	APK_RSC_AddArgumentToRequestHandle (Handle_t hRscReq, RxTrialArg_t *punArg);
extern HERROR	APK_RSC_TryResourceRequest (Handle_t hRscReq);
extern RxRscTrialResult_t*	APK_RSC_GetRequestTrialResult (Handle_t hRscReq);
extern HERROR	APK_RSC_DestroyResourceRequestHandle (RxRscReqHandle_t hRscReq);
extern HERROR	APK_RSC_SetResourcesByTrialResults (RxRscReqHandle_t hRscReq);
extern HERROR	APK_RSC_GetEmptyRecordAndBgActionIdArray (HUINT32 *pulActionIdArray, HUINT32 ulNum);
extern HERROR	APK_RSC_GetEmptyRecordActionIdArray (HUINT32 *pulActionIdArray, HUINT32 ulNum);
extern HERROR	APK_RSC_GetEmptyRecordActionId (HUINT32 *pulActionId);
extern HERROR	APK_RSC_GetEmptyBgActionId (HUINT32 *pulActionId);
extern HERROR	APK_RSC_GetActionPriority (HUINT32 ulActionId, HUINT32 *pulPriority);

#endif	// ____APPKIT_RESOURCE__H___

