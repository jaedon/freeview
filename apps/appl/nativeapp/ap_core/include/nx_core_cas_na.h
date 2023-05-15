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
 * @file	  		nx_core_cas_na.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_CORE_CAS_NA_H__
#define __NX_CORE_CAS_NA_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/
#include <ondk.h>
#include <hapi.h>
#include <papi.h>
#include <oapi.h>
#include <apk.h>

#include <nx_common.h>
#include <nx_port.h>
#include <nx_core_cas.h>


/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/

/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/
#define	NX_CAS_NA_SYSTEM_INFO_COMPONENT_LEN						16

#define	NX_CAS_NA_BANNER_PARAM_DATA								"BANNER"
#define	NX_CAS_NA_MENU_PARAM_DATA								"MENU"
#define	NX_CAS_NA_EXTENSION_PARAM_DATA							"EXTENSION"


#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
#define	NX_CAS_MENU_SMTCARDINFO_PROJECT_NAME_ID					"STR_PROJECT_NAME_ID"
#define	NX_CAS_MENU_SMTCARDINFO_SMARTCARD_VERSION_ID			"STR_SMARTCARD_VERSION_ID"
#define	NX_CAS_MENU_SMTCARDINFO_CA_VERSION_ID					"STR_CA_VERSION_ID"
#define	NX_CAS_MENU_SMTCARDINFO_SMARTCARD_SERIAL_NUMBER_ID		"STR_SMARTCARD_SERIAL_NUMBER_ID"
#define	NX_CAS_MENU_SMTCARDINFO_CA_SERIAL_NUMBER_ID				"STR_CA_SERIAL_NUMBER_ID"
#define	NX_CAS_MENU_SMTCARDINFO_CHIPSET_NUID_ID					"STR_CHIPSET_NUID_ID"
#define	NX_CAS_MENU_SMTCARDINFO_CHIPSET_TYPE_ID					"STR_CHIPSET_TYPE_ID"
#define	NX_CAS_MENU_SMTCARDINFO_CHIPSET_REV_ID					"STR_CHIPSET_REV_ID"
#define	NX_CAS_MENU_SMTCARDINFO_CARD_CREDIT_ID					"STR_CARD_CREDIT_ID"
#if	defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ME)
#define	NX_CAS_MENU_SMTCARDINFO_CSC_INDEX_ID					"STR_CSC_INDEX_ID",
#endif
#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
#define	NX_CAS_MENU_SMTCARDINFO_SMARTCARD_VERSION_ID			"STR_SMARTCARD_VERSION_ID",
#define	NX_CAS_MENU_SMTCARDINFO_SMARTCARD_SERIAL_NUMBER_ID		"STR_SMARTCARD_SERIAL_NUMBER_ID",
#endif
#if defined(CONFIG_MW_CAS_NAGRA_PRM)
#define	NX_CAS_MENU_SMTCARDINFO_DVL_VERSION_ID					"STR_DVL_VERSION_ID",
#define	NX_CAS_MENU_SMTCARDINFO_DVL_CHIPSETTYPE_ID				"STR_DVL_CHIPSETTYPE_ID",
#endif

#else	// Else of defined(CONFIG_MW_CAS_NAGRA_OP_SES)

#define	NX_CAS_MENU_SMTCARDINFO_PROJECT_NAME_ID					"LOC_PROJECT_NAME_ID"
#define	NX_CAS_MENU_SMTCARDINFO_SMARTCARD_VERSION_ID			"LOC_SMARTCARD_VERSION_ID"
#define	NX_CAS_MENU_SMTCARDINFO_CA_VERSION_ID					"LOC_CA_VERSION_ID"
#define	NX_CAS_MENU_SMTCARDINFO_SMARTCARD_SERIAL_NUMBER_ID		"LOC_SMARTCARD_SERIAL_NUMBER_ID"
#define	NX_CAS_MENU_SMTCARDINFO_CA_SERIAL_NUMBER_ID				"LOC_CA_SERIAL_NUMBER_ID"
#define	NX_CAS_MENU_SMTCARDINFO_CHIPSET_NUID_ID					"LOC_CHIPSET_NUID_ID"
#define	NX_CAS_MENU_SMTCARDINFO_CHIPSET_TYPE_ID					"LOC_CHIPSET_TYPE_ID"
#define	NX_CAS_MENU_SMTCARDINFO_CHIPSET_REV_ID					"LOC_CHIPSET_REV_ID"
#if	defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ME)
#define	NX_CAS_MENU_SMTCARDINFO_CSC_INDEX_ID					"LOC_CSC_INDEX_ID",
#endif
#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
#define	NX_CAS_MENU_SMTCARDINFO_SMARTCARD_VERSION_ID			"LOC_SMARTCARD_VERSION_ID",
#define	NX_CAS_MENU_SMTCARDINFO_SMARTCARD_SERIAL_NUMBER_ID		"LOC_SMARTCARD_SERIAL_NUMBER_ID",
#endif
#if defined(CONFIG_MW_CAS_NAGRA_PRM)
#define	NX_CAS_MENU_SMTCARDINFO_DVL_VERSION_ID					"LOC_DVL_VERSION_ID",
#endif

#endif	// Else of defined(CONFIG_MW_CAS_NAGRA_OP_SES)


/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/
typedef enum
{
	eNxCasNAHandleKey_None = 0,
	eNxCasNAHandleKey_Banner,
	eNxCasNAHandleKey_Extension,
	eNxCasNAHandleKey_SmartCardInfo

} NX_CasNaHandleKey_t;


typedef enum
{
	eNxCasNaMessage_Type_None = 0,

	//reated by smartcard
	eNxCasNaMessage_SMARTCARD_OK = 0x1000,
	eNxCasNaMessage_SMARTCARD_COM_ERROR,
	eNxCasNaMessage_SMARTCARD_MUTE,
	eNxCasNaMessage_SMARTCARD_INVALID,
	eNxCasNaMessage_SMARTCARD_BLACKLISTED,
	eNxCasNaMessage_SMARTCARD_SUSPENDED,
	eNxCasNaMessage_SMARTCARD_NEVER_PAIRED,
	eNxCasNaMessage_SMARTCARD_NOT_PAIRED,
	eNxCasNaMessage_SMARTCARD_EXPIRED,
	eNxCasNaMessage_SMARTCARD_NOT_CERTIFIED,
	eNxCasNaMessage_SMARTCARD_REMOVED,

	//reated by access
	eNxCasNaMessage_ACCESS_FREE,
	eNxCasNaMessage_ACCESS_GRANTED,
	eNxCasNaMessage_ACCESS_GRANTED_FOR_PREVIEW,
	eNxCasNaMessage_ACCESS_GRANTED_BUT_WARNING_PREVIEW,
	eNxCasNaMessage_ACCESS_GRANTED_PPT,
	eNxCasNaMessage_ACCESS_DENIED,
	eNxCasNaMessage_ACCESS_NO_VALID_SMARTCARD,
	eNxCasNaMessage_ACCESS_DENIED_BUT_PREVIEW,
	eNxCasNaMessage_ACCESS_BLACKOUT,
	eNxCasNaMessage_ACCESS_DENIED_BUT_PPT,
	eNxCasNaMessage_ACCESS_DENIED_NO_VALID_CREDIT,
	eNxCasNaMessage_ACCESS_DENIED_FOR_PARENTAL_RATING,
	eNxCasNaMessage_ACCESS_DENIED_COPY_PROTECTED,
	eNxCasNaMessage_ACCESS_DENIED_PARENTAL_CONTROL,
	eNxCasNaMessage_ACCESS_DENIED_DIALOG_REQUIRED,
	eNxCasNaMessage_ACCESS_DENIED_PAIRING_REQUIRED,
	eNxCasNaMessage_ACCESS_DENIED_CS_PAIRING_REQUIRED
} NX_CasNaMessage_Type;


/*******************************************************************/
/********************	Private Variables		********************/
/*******************************************************************/

/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/

HERROR	NX_CAS_NAGRA_Init(void);
HERROR	NX_CAS_NAGRA_RegisterNotifier(APK_CAS_NAGRA_Notifier_t fnNotifier);
HERROR	NX_CAS_NAGRA_UnregisterNotifier(void);
HERROR	NX_CAS_NAGRA_IsCWE(HBOOL *pbCWE);
HERROR	NX_CAS_NAGRA_IsJTAG(HBOOL *pbJTAG);
HERROR	NX_CAS_NAGRA_IsSBOOT(HBOOL *pbSBOOT);
HERROR	NX_CAS_NAGRA_IsOTP(HBOOL *pbOTP);
HERROR	NX_CAS_NAGRA_GetNUID(HUINT8 *pszNUID);
HERROR	NX_CAS_NAGRA_GetCASN(HUINT8 *pszCASN);
HERROR	NX_CAS_NAGRA_GetFusingInfo(HUINT8 *pucSCS, HUINT8 *pucJTAG, HUINT8 *pucOTP, HUINT8 *pucCWE);
HBOOL	NX_CAS_NAGRA_IsSCInserted(void);
HBOOL	NX_CAS_NAGRA_PINCODE_Verify(HUINT8 *pszPinStr);
HBOOL	NX_CAS_NAGRA_GetAccessibleState(HUINT8 *pnAccessState, HUINT32 *pulRemainTime, HUINT8 *pnRetryCount);
HBOOL	NX_CAS_NAGRA_VerifyPincodeGetAccessibleState(HUINT8 *pnAccessState, HUINT32 *pulRemainTime, HUINT8 *pnRetryCount, HUINT8 *pszPinStr);
HBOOL	NX_CAS_NAGRA_IsAccessibleState(HUINT8 *pnAccessState);
HBOOL	NX_CAS_NAGRA_PINCODE_Set(HUINT8 *pszOldPinStr, HUINT8 *pszNewPinStr);
HERROR	NX_CAS_NAGRA_GetErrorMessage(HCHAR *pszItemIdStrId, NX_CasNaMessage_Type *eCasMessageType);

#endif // __NX_CORE_CAS_NA_H__
