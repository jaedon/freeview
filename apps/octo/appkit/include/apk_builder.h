/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_builder.h
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

#ifndef ___APK2_BUILDER_H___
#define ___APK2_BUILDER_H___

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
typedef	struct
{
	DxDeliveryType_e		eDeliveryType;
	union
	{
		DxBuilder_SatelliteInfo_t		satInfo;
	} utInfo;
} ApkBuilder_Action_t;

typedef void (*APK_BUILDER_Notifier_t) (
											HUINT32 ulBuilderId,
											DxBuilder_Event_e eEvent,
											DxBuilder_NotifierData_t *pstNotifyData,
											void *pvUserData
											);


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
HERROR			APK_BUILDER_Init (void);
HERROR			APK_BUILDER_RegisterNotifier (HUINT32 hBuilderHandle, APK_BUILDER_Notifier_t pfNotifier, void *pvUserData);
HERROR			APK_BUILDER_UnregisterNotifier (HUINT32 hBuilderHandle, APK_BUILDER_Notifier_t pfNotifier, void *pvUserData);
HERROR			APK_BUILDER_OpenSession (DxDeliveryType_e eDeliveryType, HCHAR *scanType, ApkBuilder_Action_t *pstInfo, HUINT32 *phBuilderHandle);
HERROR			APK_BUILDER_CloseSession (HUINT32 hBuilderHandle);
HERROR			APK_BUILDER_ClearBuildInfo (HUINT32 hBuilderHandle);
HERROR			APK_BUILDER_AddBuildInfo (HUINT32 hBuilderHandle, DxTuneParam_t *pstInfo);
HERROR			APK_BUILDER_RemoveBuildInfo (HUINT32 hBuilderHandle, DxBuilder_BuildInfo_t *pstParam);
HERROR			APK_BUILDER_GetOption (HUINT32 hBuilderHandle, DxBuilder_Option_t *pstInfo);
HERROR			APK_BUILDER_SetOption (HUINT32 hBuilderHandle, DxBuilder_Option_t *pstInfo);
HERROR			APK_BUILDER_GetBuilderStatus (HUINT32 hBuilderHandle,
															HUINT32 *pulTotalTuningNum,
															HUINT32 *pulCompleteTuningNum,
															HUINT32 *pulSearchedTuningNum,
															HUINT32	*pulSearchState,
															HUINT32 *pulFrequency,
															DxDeliveryType_e *peDeliveryType,
															DxTuneParam_t *pstTuningInfo);
HERROR			APK_BUILDER_GetBuilderSignalStatus(HUINT32 hBuilderHandle, HINT32 *pnSignalStrength, HINT32 *pnSignalQuality);
HERROR 			APK_BUILDER_GetBuilderSvcInfo (HUINT32 hBuilderHandle, HINT32 lStartIndex, HINT32 lCount, HINT32 *plResultNo, DxBuilder_SearchedInfo_t **ppstSvcInfoList);
HERROR 			APK_BUILDER_FreeBuilderSvcInfo (DxBuilder_SearchedInfo_t *pstSvcInfoList);
const HCHAR* 	APK_BUILDER_GetScanType(HUINT32 hBuilderHandle);
HERROR			APK_BUILDER_StartScan (HUINT32 hBuilderHandle);
HERROR			APK_BUILDER_StopScan (HUINT32 hBuilderHandle);
HERROR 			APK_BUILDER_PauseScan (HUINT32 hBuilderHandle);
HERROR 			APK_BUILDER_ResumeScan (HUINT32 hBuilderHandle);
HERROR 			APK_BUILDER_FinishScan (HUINT32 hBuilderHandle);
HERROR			APK_BUILDER_SetAdditionalNumbers(HUINT32 hBuilderHandle);
HERROR 			APK_BUILDER_MotorGotoPosition (HUINT32 hBuilderHandle, HINT32 lPosition);
HERROR 			APK_BUILDER_MotorStorePosition (HUINT32 hBuilderHandle, HINT32 lPosition);
HERROR 			APK_BUILDER_MotorResetPosition (HUINT32 hBuilderHandle);
HERROR 			APK_BUILDER_MotorRecalculate (HUINT32 hBuilderHandle, HINT32 lPosition);
HERROR 			APK_BUILDER_MotorMove (HUINT32 hBuilderHandle, DxSat_MOTOR_DIRECTION_e eDirection, DxSat_MOTOR_DRIVEMODE_e eMode, HINT32 step);
HERROR 			APK_BUILDER_MotorStop (HUINT32 hBuilderHandle);
HERROR 			APK_BUILDER_MotorSetLimit (HUINT32 hBuilderHandle, DxSat_MOTOR_DIRECTION_e eDirection);
HERROR 			APK_BUILDER_MotorEnableLimit (HUINT32 hBuilderHandle);
HERROR 			APK_BUILDER_MotorDisableLimit (HUINT32 hBuilderHandle);
HERROR 			APK_BUILDER_MotorGotoDegree (HUINT32 hBuilderHandle, HINT32 lDegree);
HERROR 			APK_BUILDER_MotorGotoSatellite (HUINT32 hBuilderHandle, HINT32 lSatellite);
HERROR			APK_BUILDER_MotorSetMyLongitude (HINT32 lNewLongitude);
HERROR			APK_BUILDER_MotorSetMyLatitude (HINT32 lNewLatitude);
HERROR			APK_BUILDER_MotorGetMyLongitude (HINT32 *plMyLongitude);
HERROR			APK_BUILDER_MotorGetMyLatitude (HINT32 *plMyLatitude);
HERROR			APK_BUILDER_SetSelectedConflictLcn(HUINT32 hBuilderHandle, DxBuilder_SearchedInfo_t *selectedInfos, int count);

#endif // ___APK2_BUILDER_H___
/* end of file */
