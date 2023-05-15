/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_cas_nagra.h
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

#ifndef __OAPI_CAS_NAGRA_H__
#define __OAPI_CAS_NAGRA_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define RPC_OAPI_CAS_NAGRA_Init						"oapi.cas.nagra.Init"
#define RPC_OBAMA_CAS_NAGRA_onStatusChanged			"obama.cas.nagra.onStatusChanged"
#define RPC_OBAMA_CAS_NAGRA_onSCInserted				"obama.cas.nagra.onSCInserted"
#define RPC_OBAMA_CAS_NAGRA_PINCODE_VERIFY			"obama.cas.nagra.verifypin"
#define RPC_OBAMA_CAS_NAGRA_GetAccessibleState			"obama.cas.nagra.GetAccessibleState"
#define RPC_OBAMA_CAS_NAGRA_VerifyPincodeGetAccessibleState	"obama.cas.nagra.VerifyPincodeGetAccessibleState"
#define RPC_OBAMA_CAS_NAGRA_PINCODE_SET					"obama.cas.nagra.setpin"
#define RPC_OBAMA_CAS_NAGRA_UnlockAV						"obama.cas.nagra.unlockAV"
#define RPC_OBAMA_CAS_NAGRA_PRM_NovaNuid					"obama.cas.nagra.prm.NovaNuid"
#define RPC_OBAMA_CAS_NAGRA_PRM_NovaTransportSessionInfoNotifier	"obama.cas.nagra.prm.NovaTransportSessionInfoNotifier"
#define RPC_OBAMA_CAS_NAGRA_PRM_NovaDrmRelease					"obama.cas.nagra.prm.NovaDrmRelease"
#define RPC_OBAMA_CAS_NAGRA_PRM_NovaDrmSetParameters			"obama.cas.nagra.prm.NovaDrmSetParameters"
#define RPC_OBAMA_CAS_NAGRA_PRM_NovaDrmRelease					"obama.cas.nagra.prm.NovaDrmRelease"
#define RPC_OBAMA_CAS_NAGRA_PRM_NovaLicenseNotifier				"obama.cas.nagra.prm.NovaLicenseNotifier"
#define RPC_OBAMA_CAS_NAGRA_PRM_NovaKeyHandlerNotifier				"obama.cas.nagra.prm.NovaKeyHandlerNotifier"
#define RPC_OBAMA_CAS_NAGRA_PRM_NovaPlatformUsageRulesNotifier		"obama.cas.nagra.prm.NovaPlatformUsageRulesNotifier"
#define RPC_OBAMA_CAS_NAGRA_PRM_NovaServiceUsageRulesNotifier		"obama.cas.nagra.prm.NovaServiceUsageRulesNotifier"
#define RPC_OBAMA_CAS_NAGRA_PRM_NovaGetCaInfo					"obama.cas.nagra.prm.NovaGetCaInfo"
/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/



#define	OAPI_CAS_NAGRA_CLIENT_VERSION_LENG		256
#define	OAPI_CAS_NAGRA_SYSTEM_INFO_COMPONENT_LENG		16
typedef struct
{
	HBOOL						bCWE;
	HBOOL						bJTAG;
	HBOOL						bSBOOT;
	HBOOL						bOTP;

	HUINT8						szNUID[OAPI_CAS_NAGRA_SYSTEM_INFO_COMPONENT_LENG];
	HUINT8						szCASN[OAPI_CAS_NAGRA_SYSTEM_INFO_COMPONENT_LENG];

	HBOOL						bSCRemoved;
} OxCasNagraStatus_t;

typedef struct
{
	HUINT8 		nAccessState;		// eCAS_PIN_STATE
	HUINT32 		ulRemainTime;
	HUINT8 		nRetryCount;
}OxCasNagraPinAccess_RPC_Item_t;

#if defined(CONFIG_SUPPORT_NOVA_APP)
#define MAX_DATA_LENGTH						(1024)
typedef enum
{
	eNOVA_CAS_GET_CAINFO = 0,
	eNOVA_CAS_GET_KEYHANDLER,
	eNOVA_CAS_MAX,
} OxCasNagraPrm_NovaType_e;

typedef struct
{
	OxCasNagraPrm_NovaType_e NovaCasType;
	HUINT8 ucData[MAX_DATA_LENGTH];
} OxCasNagraPrm_Cas_Item_t;

typedef struct
{
	HUINT8 		ucData[1024];
	HUINT32 	ulSize;
} OxCasNagraPrm_Lcm_t;

typedef struct
{
	HUINT32		ulTransportSessionId;
	HUINT32 	ulRecordHandle;
	HUINT8 		ucData[1024];
	HUINT32 	ulSize;
	HBOOL		bRequestLicenseAgain;
} OxCasNagraPrm_NovaLicenseNotifier_t;

typedef struct
{
	HUINT8 		ucData[1024];
	HUINT32 	ulSize;
} OxCasNagraPrm_NovaPlatformUsageRulesNotifier_t;

typedef struct
{
	HUINT32		ulTransportSessionId;
	HUINT8 		ucData[1024];
	HUINT32 	ulSize;
} OxCasNagraPrm_NovaServiceUsageRuleseNotifier_t;
#endif

typedef struct
{
	Handle_t hAction;
	HUINT32 ulTransportSessionId;

	HINT32 usOnId;
	HINT32 usTsId;
	HINT32 usSvcId;

	HUINT16 usAudioPid;
	HUINT16 usVideoPid;	
} OxCasNagraPrm_TransportSessionInfo_t;

typedef void (*OxCasNagra_Notifier_t)(void);
#if defined(CONFIG_SUPPORT_NOVA_APP)
typedef void (*OxCasNagra_NovaTransportSessionInfoNotifier_t) (HUINT32 hAction, HUINT32 ulTransportSessionId, HINT32 usOnId, HINT32 usTsId, HINT32 usSvcId, HUINT16 usAudioPid, HUINT16 usVideoPid);
typedef void (*OxCasNagra_NovaLicenseNotifier_t) (HUINT32 ulTransportSessionId, HUINT32 ulRecordHandle, HUINT8 *pucLicense, HUINT32 ulSize, HBOOL bRequestLicenseAgain);
typedef void (*OxCasNagra_NovaKeyHandlerNotifier_t) (HUINT8 *pucData);
typedef void (*OxCasNagra_NovaPlatformUsageRulesNotifier_t) (HUINT8 *pucUsageRules, HUINT32 ulSize);
typedef void (*OxCasNagra_NovaServiceUsageRulesNotifier_t) (HUINT32 ulTransportSessionId, HUINT8 *pucUsageRules, HUINT32 ulSize);
#endif
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
#if defined(CONFIG_SUPPORT_NOVA_APP)
HERROR OAPI_CAS_NAGRA_NOVA_Init(OxCasNagra_NovaTransportSessionInfoNotifier_t pfNovatTransportSessionInfoNotifier
									, OxCasNagra_NovaLicenseNotifier_t pfNovaLicenseNotifier
									, OxCasNagra_NovaKeyHandlerNotifier_t pfNovaKeyHandlerNotifier
									, OxCasNagra_NovaPlatformUsageRulesNotifier_t pfNovaPlatformUsageRulesNotifier
									, OxCasNagra_NovaServiceUsageRulesNotifier_t pfNovaServiceUsageRulesNotifier);
#endif
HERROR 		OAPI_CAS_NAGRA_Init(OxCasNagra_Notifier_t pfListener);
HERROR 		OAPI_CAS_NAGRA_SetVirtualSecureCwMode(HBOOL bEnable);
HERROR 		OAPI_CAS_NAGRA_GetStatus(OxCasNagraStatus_t *pstStatus);
HERROR		OAPI_CAS_NAGRA_IsSCInserted(HBOOL *pbVerifie);
HERROR		OAPI_CAS_NAGRA_PINCODE_Verify(const HCHAR *pszPinStr, HBOOL *pbVerified);
HERROR 		OAPI_CAS_NAGRA_GetAccessibleState(HUINT8 *pnAccessState, HUINT32 *pulRemainTime, HUINT8 *pnRetryCount);
HERROR 		OAPI_CAS_NAGRA_VerifyPincodeGetAccessibleState(HUINT8 *pnAccessState, HUINT32 *pulRemainTime, HUINT8 *pnRetryCount, const HCHAR *pszPinStr);
HERROR		OAPI_CAS_NAGRA_PINCODE_Set(const HCHAR *pszOldPinStr, const HCHAR *pszNewPinStr);
#if defined (CONFIG_OP_SES)
HERROR		OAPI_CAS_NAGRA_UnlockAV(void);
#endif

#if defined(CONFIG_SUPPORT_NOVA_APP)
#define MAX_CAS_INFO_STRING_LEGTH			(50) // temp
#define 	NOVA_MAX_LICENSE_LENGTH			(1024)
#define 	NOVA_MAX_USAGERULESE_LENGTH		(1024)

typedef struct
{
	HUINT8 	ucChipsetUid[MAX_CAS_INFO_STRING_LEGTH];
	HUINT8 	ucCaVendor[MAX_CAS_INFO_STRING_LEGTH];
	HUINT8 	ucChipsetType[MAX_CAS_INFO_STRING_LEGTH];
	HUINT8 	ucChipsetRevision[MAX_CAS_INFO_STRING_LEGTH];
	HUINT8 	ucCaVersion[MAX_CAS_INFO_STRING_LEGTH];
	HUINT8 	ucCaNumber[MAX_CAS_INFO_STRING_LEGTH];
	HBOOL 	bSmartcardInserted;
	HBOOL 	bSmartcardSuitable;
	HUINT8 	ucSmartcardType[MAX_CAS_INFO_STRING_LEGTH];
	HUINT8 	ucSmartcardNumber[MAX_CAS_INFO_STRING_LEGTH];
	HUINT8 	ucSmartcardStatus;
	HUINT32	ulExpirationDate;
} OxCasNagraPrm_CasInfo_Item_t;

typedef struct
{
  HUINT8 	License[NOVA_MAX_LICENSE_LENGTH];
  HUINT32   	LicenseLength;

} OAPI_NaPrm_NovaspreadTDrmLicense_t;

typedef struct
{
	HUINT32 ulTransportSessionId;
	OAPI_NaPrm_NovaspreadTDrmLicense_t   OldLicense;

	HUINT32       CollectionId;
	HUINT32       Duration;
	HUINT8 	      UsageRules[NOVA_MAX_USAGERULESE_LENGTH];
	HUINT32       UsageRulesLength;	
} OAPI_NaPrm_NovaspreadTDrmLicenseParameters_t;

HERROR OAPI_CAS_NAGRA_PRM_GetCaInfo(HUINT8 *pucData);
HERROR OAPI_CAS_NAGRA_PRM_NovaDrmRelease(HUINT32 ulTransportSessionId, HUINT32 ulRecordHandle);
HERROR OAPI_CAS_NAGRA_PRM_NovaDrmSetParameters(HUINT32 ulTransportSessionId
									, HUINT8 *pOldLicense, HUINT32 ulOldLicenseSize
									, HUINT32 ulCollectionId
									, HUINT32 ulDuration
									, HUINT8 *pUsageRules, HUINT32 ulUsageRulesLength);
#endif

#endif // __OAPI_CAS_NAGRA_H__
