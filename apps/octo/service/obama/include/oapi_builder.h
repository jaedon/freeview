/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_builder.h
	@brief

	Description:  									\n
	Module: OAPI / API					\n

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

#ifndef __OAPI_BUILDER_H__
#define __OAPI_BUILDER_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define RPC_OAPI_BUILDER_Init					"oapi.builder.Init"
#define RPC_OAPI_BUILDER_Open					"oapi.builder.Open"
#define RPC_OAPI_BUILDER_Close					"oapi.builder.Close"
#define RPC_OAPI_BUILDER_Start					"oapi.builder.Start"
#define RPC_OAPI_BUILDER_Stop					"oapi.builder.Stop"
#define RPC_OAPI_BUILDER_Pause					"oapi.builder.Pause"
#define RPC_OAPI_BUILDER_Resume					"oapi.builder.Resume"
#define RPC_OAPI_BUILDER_Finish					"oapi.builder.Finish"
#define RPC_OAPI_BUILDER_AddBuildInfo			"oapi.builder.AddBuildInfo"
#define RPC_OAPI_BUILDER_SetOption				"oapi.builder.SetOption"
#define RPC_OAPI_BUILDER_MotorGotoPosition		"oapi.builder.MotorGotoPosition"
#define RPC_OAPI_BUILDER_MotorStorePosition		"oapi.builder.MotorStorePosition"
#define RPC_OAPI_BUILDER_MotorResetPosition		"oapi.builder.MotorResetPosition"
#define RPC_OAPI_BUILDER_MotorRecalculate		"oapi.builder.MotorRecalculate"
#define RPC_OAPI_BUILDER_MotorMove				"oapi.builder.MotorMove"
#define RPC_OAPI_BUILDER_MotorStop				"oapi.builder.MotorStop"
#define RPC_OAPI_BUILDER_MotorSetLimit			"oapi.builder.MotorSetLimit"
#define RPC_OAPI_BUILDER_MotorEnableLimit		"oapi.builder.MotorEnableLimit"
#define RPC_OAPI_BUILDER_MotorDisableLimit		"oapi.builder.MotorDisableLimit"
#define RPC_OAPI_BUILDER_MotorGotoDegree		"oapi.builder.MotorGotoDegree"
#define RPC_OAPI_BUILDER_MotorGotoSatellite		"oapi.builder.MotorGotoSatellite"
#define RPC_OAPI_BUILDER_MotorSetMyLongitude	"oapi.builder.MotorSetMyLongitude"
#define RPC_OAPI_BUILDER_MotorSetMyLatitude		"oapi.builder.MotorSetMyLatitude"
#define RPC_OAPI_BUILDER_SetSelectedConflictLcn	"oapi.builder.SetSelectedConflictLcn"

#define RPC_OBAMA_BUILDER_Inited				"obama.builder.Inited"
#define RPC_OBAMA_BUILDER_onStateChanged		"obama.builder.onStateChanged"


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef	struct
{
	HUINT32				ulTotalTuningNum;
	HUINT32				ulCompleteTuningNum;
	HUINT32				ulSearchedSvcNum;
	HUINT32				ulSearchState;
	HUINT32				ulFrequency;
	DxDeliveryType_e 	eDeliveryType;

	HUINT32				ulSignalStrength;
	HUINT32 			ulSignalQuality;

	DxTuneParam_t		stTuneParam;
} OxBuilder_BuilderState_t;

typedef enum
{
	eOxBUILDER_OPTIONTYPE_NONE,

	// base
	eOxBUILDER_OPTIONTYPE_SVCSEARCH,
	eOxBUILDER_OPTIONTYPE_ANTCONNECT,
	eOxBUILDER_OPTIONTYPE_DISEQCDETECT,
	eOxBUILDER_OPTIONTYPE_TUNEONLY,
	eOxBUILDER_OPTIONTYPE_CIPLUS_SEARCH,

	// derivative
	eOxBUILDER_OPTIONTYPE_ERTELECOM_SEARCH,
	eOxBUILDER_OPTIONTYPE_UKDTT_SEARCH,
	eOxBUILDER_OPTIONTYPE_OP_TP_LIST_SEARCH,

	eOxBUILDER_OPTIONTYPE_ENDOFCASE
} OxBuilder_OptionType_e;

typedef struct
{
	DxDeliveryType_e			eDeliveryType;
	OxBuilder_OptionType_e		eOptionType;
	DxBuilder_Option_t			stSvcSrchOpt;

	union
	{
		DxBuilder_SatelliteInfo_t	stSatInfo;
	} unArgs;

} OxBuilder_Option_t;

typedef void (*OxBuilder_Listener_t) (
											HUINT32 ulBuilderId,
											DxBuilder_Event_e eEvent,
											DxBuilder_NotifierData_t *pstNotifyData,
											void *pvUserData
											);

typedef void (*OxBuilder_Activated_t) (void);

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
extern HERROR	OAPI_BUILDER_Init(OxBuilder_Activated_t pfActivated);
extern HERROR	OAPI_BUILDER_SetListener(OxBuilder_Listener_t pfListener, void *pvUserData);
extern HERROR	OAPI_BUILDER_ResetListener(OxBuilder_Listener_t pfListener, void *pvUserData);
extern HERROR	OAPI_BUILDER_Open(HUINT32 *pulBuilderId);
extern HERROR	OAPI_BUILDER_Close(HUINT32 ulBuilderId);
extern HERROR	OAPI_BUILDER_ClearBuildInfo(HUINT32 ulBuilderId);
extern HERROR	OAPI_BUILDER_AddBuildInfo(HUINT32 ulBuilderId, DxBuilder_BuildInfo_t *pstParam);
extern HERROR	OAPI_BUILDER_RemoveBuildInfo(HUINT32 ulBuilderId, DxBuilder_BuildInfo_t *pstParam);
extern HERROR	OAPI_BUILDER_GetOption(HUINT32 ulBuilderId, OxBuilder_Option_t *pstOption);
extern HERROR	OAPI_BUILDER_SetOption(HUINT32 ulBuilderId, OxBuilder_Option_t *pstOption);
extern HERROR 	OAPI_BUILDER_GetBuilderStatus(HUINT32 ulBuilderId, OxBuilder_BuilderState_t *pstBuilderStatus);
extern HERROR 	OAPI_BUILDER_GetBuilderSvcInfo(HUINT32 ulBuilderId, HINT32 lStartIndex, HINT32 lCount, HINT32 *plResultNo, DxBuilder_SearchedInfo_t **ppstSvcInfoList);
extern HERROR 	OAPI_BUILDER_FreeBuilderSvcInfo(DxBuilder_SearchedInfo_t *pstSvcInfoList);
extern HERROR	OAPI_BUILDER_Start(HUINT32 ulBuilderId);
extern HERROR	OAPI_BUILDER_Stop(HUINT32 ulBuilderId);
extern HERROR 	OAPI_BUILDER_Pause(HUINT32 ulBuilderId);
extern HERROR 	OAPI_BUILDER_Resume(HUINT32 ulBuilderId);
extern HERROR 	OAPI_BUILDER_Finish(HUINT32 ulBuilderId);
extern HERROR 	OAPI_BUILDER_MotorGotoPosition(HUINT32 ulBuilderId, HINT32 lPosition);
extern HERROR 	OAPI_BUILDER_MotorStorePosition(HUINT32 ulBuilderId, HINT32 lPosition);
extern HERROR 	OAPI_BUILDER_MotorResetPosition(HUINT32 ulBuilderId);
extern HERROR 	OAPI_BUILDER_MotorRecalculate(HUINT32 ulBuilderId, HINT32 lPosition);
extern HERROR 	OAPI_BUILDER_MotorMove(HUINT32 ulBuilderId, DxSat_MOTOR_DIRECTION_e eDirection, DxSat_MOTOR_DRIVEMODE_e eDriveMode, HINT32 lStep);
extern HERROR 	OAPI_BUILDER_MotorStop(HUINT32 ulBuilderId);
extern HERROR 	OAPI_BUILDER_MotorSetLimit(HUINT32 ulBuilderId, DxSat_MOTOR_DIRECTION_e eDirection);
extern HERROR 	OAPI_BUILDER_MotorEnableLimit(HUINT32 ulBuilderId);
extern HERROR 	OAPI_BUILDER_MotorDisableLimit(HUINT32 ulBuilderId);
extern HERROR 	OAPI_BUILDER_MotorGotoDegree(HUINT32 ulBuilderId, HINT32 lDegree);
extern HERROR 	OAPI_BUILDER_MotorGotoSatellite(HUINT32 ulBuilderId, HINT32 lSatellite);
extern HERROR 	OAPI_BUILDER_MotorSetMyLongitude(HINT32 lNewLongitude);
extern HERROR 	OAPI_BUILDER_MotorSetMyLatitude(HINT32 lNewLatitude);
extern HERROR 	OAPI_BUILDER_MotorGetMyLongitude(HINT32 *plMyLongitude);
extern HERROR 	OAPI_BUILDER_MotorGetMyLatitude(HINT32 *plMyLatitude);
extern HERROR OAPI_BUILDER_SetSelectedConflictLcnSvcInfo (HUINT32 ulBuilderId, DxBuilder_SearchedInfo_t *pstSvcInfoList, HUINT32 ulCount);

#endif // __OAPI_BUILDER_H__
