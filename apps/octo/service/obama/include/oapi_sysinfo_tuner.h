/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_sysinfo_tuner.h
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

#ifndef __OAPI_SYSINFO_TUNER_H__
#define __OAPI_SYSINFO_TUNER_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	RPC_OAPI_SYSINFO_TUNER_Init					"oapi.sysinfo.tuner.Init"
#define	RPC_OAPI_SYSINFO_TUNER_Refresh				"oapi.sysinfo.tuner.Refresh"

#define	RPC_OBAMA_SYSINFO_TUNER_onStatusChanged		"obama.sysinfo.onStatusChanged"

#define	NUM_OAPI_SYSINFO_CONNECTED_SVC				4
#define	NUM_OAPI_TUNER								4
/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HUINT16		usTsId;
	HUINT16		usSvcId;
	HUINT16		usOrgNetId;
} OxSysInfoTuner_TripleId_t;

typedef struct
{
	DxDeliveryType_e	eTunerType;
	HBOOL				bLocked;
	HINT32				nSignalLevel;
	HINT32				nSignalQuality;
	HINT32				nAgc;
	HFLOAT32			fBer;
	HFLOAT32			fSnr;
	HFLOAT32			fInputPower;
	HUINT32				ulLastCheckTick;

	OxSysInfoTuner_TripleId_t		astConnectedService[NUM_OAPI_SYSINFO_CONNECTED_SVC];

	union
	{
		DxSatTuningInfo_t	stSatInfo;
		DxCabTuningInfo_t	stCabInfo;
		DxTerTuningInfo_t	stTerInfo;
	} utInfo;
} OxSysInfoTuner_Status_t;

typedef struct
{
	HUINT32		ulDummy;
} OxSysInfoTuner_AnaTunerStatus_t;

typedef struct
{
	HINT32		nNoOfTunerGroup;
	HINT32		nNoOfRfInputGroup;			//	always set to zero (before supporting analog tuner)

	OxSysInfoTuner_Status_t			astTunerStatus[NUM_OAPI_TUNER];
	OxSysInfoTuner_AnaTunerStatus_t	astAnaTunerStatus[NUM_OAPI_TUNER];
} OxSysInfoTuner_Info_t;

typedef struct
{
	HINT32		nSignalLevel;
	HINT32		nSignalQuality;
	HINT32		nAgc;
	HFLOAT32	fBer;
	HFLOAT32	fSnr;
	HFLOAT32	fInputPower;
} OxSysInfoTuner_SignalInfo_t;

typedef void (*OxSysInfoTuner_Notifier_t)(void);

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
HERROR	OAPI_SYSINFO_TUNER_Init(OxSysInfoTuner_Notifier_t pfnNotifier);
HERROR	OAPI_SYSINFO_TUNER_DeInit(void);
HERROR	OAPI_SYSINFO_TUNER_GetDeviceNum(HUINT32 *pulDevNum);
HERROR	OAPI_SYSINFO_TUNER_GetDevType(HINT32 nIndex, DxDeliveryType_e *peDeliType);
HERROR	OAPI_SYSINFO_TUNER_IsLocked(HINT32 nIndex, HBOOL *pbLocked);
HERROR	OAPI_SYSINFO_TUNER_GetSignalInfo(HINT32 nIndex, OxSysInfoTuner_SignalInfo_t *pstSignalInfo);
HERROR	OAPI_SYSINFO_TUNER_GetTuneParam(HINT32 nIndex, DxTuneParam_t *pstTuneParam);
HERROR	OAPI_SYSINFO_TUNER_GetTunerIdByTripleId(HINT32 nOnId, HINT32 nTsId, HINT32 nSvcId, HINT32 *pnTunerId);

#endif // __OAPI_SYSINFO_TUNER_H__

