/*
 * (c) 2011-2012 Humax Co., Ltd.
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
/*******************************************************************
	File Description
********************************************************************/
/**
@file				conax.h
@brief			conax.h

	Description:  conax core 부분의 api 및 필요한 define들 \n
	Module: CONAX CORE \n

	Copyright (c) 2009 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

#ifndef	__CONAX_H__
#define	__CONAX_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <octo_common.h>

#include <hapi.h>
#include <papi.h>

// SvcCas_PmtInfo_t가 정의된 header file
#include <_svc_cas_mgr_common.h>



/*******************************************************************/
/********************      Global Variables 	********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition 	********************/
/*******************************************************************/
#if defined(CONFIG_DEBUG)
#define ENTER_FUNCTION					HxLOG_Print("==============================>>> CONAX (+)\r\n");
#define LEAVE_FUNCTION					HxLOG_Print("==============================>>> CONAX (-)\r\n");
#else
#define ENTER_FUNCTION					HxLOG_NullPrint;
#define LEAVE_FUNCTION					HxLOG_NullPrint;
#endif

#define GET_ACTION_ID_FROM_STBHANDLE(a)		((a >> 24) & 0xFF)
//**** 옵션 설정 ****//

// 동일 ecm을 사용하는 es pid들이 하나의 descrambler slot을 공유하여 사용한다면 아래 define 사용
//#define CX_SUPPORT_SHARED_DESCRAMBLER_SLOT
// PVR 설정
#if !defined(CONFIG_MW_MM_FULLTS_REC)
#define CONAX_PVR_SPEC_ENABLE
#define CONAX_PVR_SPEC_ENABLE_ONLY_CONAX_SERVICE
#endif
//**** 옵션 설정 (끝) ****//


// audio가 여럿이라든가 필요하다면 늘리세요. 단, CX_MAX_NUM_OF_ES보다 크면 안됨
#define CX_MAX_NUM_OF_ESPID					12//8->12...


#define CX_ERR_INVALID_PARAMETER			(-1)

// ui error code
#define CX_ERR_UI_DIFFERENT_NEW_PIN		100
#define CX_ERR_UI_WRONG_PIN				101
#define CX_ERR_UI_PIN_LOCKED			102
#define CX_ERR_UI_PIN_CHANGE_FAIL		103
#define CX_ERR_UI_IDENTICAL_PIN_FAIL	104

#define CX_MAX_USERTEXT_SLOT_NUM		16
#define CX_MAX_USER_TEXT_LEN			256
#define CX_MAX_FINGERPRINT				32
#define CX_MAX_CENTTIME_INDEX			4

#define CX_PID_ADD						TRUE
#define CX_PID_DELETE					FALSE

/* -- Maturity Rating Level Value -- */
#define	CX_MATURITY_RATE_OFF		0x00
#define	CX_MATURITY_RATE_G			0x01
#define	CX_MATURITY_RATE_PG			0x02
#define	CX_MATURITY_RATE_A			0x04
#define	CX_MATURITY_RATE_X			0x08
#define CX_MATURITY_RATE_XXX			0x09

/* -- TOKEN DEBIT INFO -- */
// - Tokens Mode
#define	CX_TOKENS_MODE_EVENT				0
#define	CX_TOKENS_MODE_TIME					1

// - Tokens Insufficient
#define	CX_TOKENS_INSUFFICIENT_ENABLE		0
#define	CX_TOKENS_INSUFFICIENT_UNABLE		1

/* Finger Print */
#define	CX_FINGER_PRINT_IMMEDIATELY_TIME		0xFFFFFF

#define CX_MAX_LABEL_LENGTH			128
#define CX_MAX_ENTITLEMENT_LENGTH	8

#define CXUI_GET_MSG_LEVEL(x)			(x >> 8)

#define CONAX_CA_PIN_SIZE			5

/*******************************************************************/
/********************      Typedef  	   *************************/
/*******************************************************************/

//htype.h 등에 이미 존재한다면 아래 typedef는 삭제하거나 수정하세요.
//typedef HUINT32					HERROR;
//typedef HUINT32					Handle_t;

typedef enum
{
	eCxEsType_None,
	eCxEsType_Video,
	eCxEsType_Audio,
	eCxEsType_PCR,
	eCxEsType_Subtitle,
	eCxEsType_Teletext,
	eCxEsType_ClosedCaption,
	eCxEsType_SuperImpose,
	eCxEsType_Data,
	eCxEsType_Reserved,
	eCxEsType_Unknown,
	eCxEsType_Max
} CxEsType_t;

typedef enum
{
	eCxCaStatus_AllSubscription	= 0x0,
	eCxCaStatus_AllPpvEvent,
	eCxCaStatus_AllPurse,
	eCxCaStatus_Subscription	= 0x5,
	eCxCaStatus_PpvEvent,
	eCxCaStatus_Purse,
	eCxCaStatus_Debit,
	eCxCaStatus_Credit
} CxStatusSelectType_t;

// 높은 숫자가 priority도 높다.
typedef enum
{
	eCxUi_GroupBasic	= 0,
	eCxUi_GroupMessage,
	eCxUi_GroupPpv,
	eCxUi_GroupMr,					// maturity rating
	eCxUi_GroupRc,					// return channel
	eCxUi_GroupMenu,
	eCxUi_GroupSc,
	eCxUi_GroupMax,
	eCxUi_GroupOther,			// 다른 메시지와 동시에 표시될 수 있다. priroity에 영향을 받지 않는다. 별개로 처리한다.
} CxUiPriority_t;

typedef enum
{
	//eCxUi_GroupBasic_None,		// == access granted
	eCxUi_GroupBasic_NoAccess		= eCxUi_GroupBasic * 0x100,
	/*No Access 관련 Dialog는 eCxUi_GroupBasic_NoAccess 값만 사용함.*/
	eCxUi_GroupBasic_NoAccessBlackOut,
	eCxUi_GroupBasic_NoAccessNetwork,
	eCxUi_GroupBasic_NoAccessPullEmm,

	//eCxUi_GroupMessage_None,
	eCxUi_GroupMessage_Display		= eCxUi_GroupMessage * 0x100,

	//eCxUi_GroupPpv_None,
	eCxUi_GroupPpv_OrderInfo		= eCxUi_GroupPpv * 0x100,
	eCxUi_GroupPpv_OrderConnect,
	eCxUi_GroupPpv_OrderFailed,
	eCxUi_GroupPpv_OrderAccepted,
	eCxUi_GroupPpv_OrderNotAccepted,
	eCxUi_GroupPpv_OrderClosed,
	eCxUi_GroupPpv_AcceptViewing,
	eCxUi_GroupPpv_TokenPpv,

	//eCxUi_GroupMr_None,
	eCxUi_GroupMr_Ask				= eCxUi_GroupMr * 0x100,
	eCxUi_GroupMr_WrongPin,
	eCxUi_GroupMr_PinLocked,

	//eCxUi_GroupRc_None,

	//eCxUi_GroupMenu_None,
	eCxUi_GroupMenu_Subscription	= eCxUi_GroupMenu * 0x100,
	eCxUi_GroupMenu_Event,
	eCxUi_GroupMenu_Purse,
	eCxUi_GroupMenu_Debit,
	eCxUi_GroupMenu_Credit,
	eCxUi_GroupMenu_ChangePin,
	eCxUi_GroupMenu_CheckNewPin,
	eCxUi_GroupMenu_WrongPin,
	eCxUi_GroupMenu_PinLocked,
	eCxUi_GroupMenu_Mr,
	eCxUi_GroupMenu_Modem,
	eCxUi_GroupMenu_Message,
	eCxUi_GroupMenu_Info,

	//eCxUi_GroupSc_None,	// == ready
	eCxUi_GroupSc_NoCard			= eCxUi_GroupSc * 0x100,
	eCxUi_GroupSc_Inserted,
	eCxUi_GroupSc_Incorrect,
	eCxUi_GroupSc_IncorrectConax,
	eCxUi_GroupSc_CommFail,

	//popup - priority에 영향을 받지 않고 동시에 표시할 수도 있는 것들..
	eCxUi_GroupOther_Fingerprint	= eCxUi_GroupOther * 0x100,
	eCxUi_GroupOther_CachedPin,
	eCxUi_GroupOther_CachedPinWithErr,
	//		eCxUi_GroupOther_MessageReceived,

	eCxUi_GroupAll_None,
} CxUiMsgType_t;

typedef enum
{
	eCxECode_1	= 1,// no CWPK in the keystore with No Encrypt KeyID
	eCxECode_2,		// a CWPK in the keystore with No Encrypt KeyID
	eCxECode_3,		// both a CWPK and LPK in the keystore with No Encrypt KeyID
	eCxECode_4,		// Access Denied and No CWPK in keystore
	eCxECode_5,		// Access Denied and only CWPK in keystore
	eCxECode_6,		// Access Denied and CWPK and LPK in keystore
	eCxECode_7,		// Only CWPK ith different key_id (matching
	eCxECode_8,		// descrambling fail	-> optional, if convinced that DSCR input is valid.
	eCxECode_9,		// No match ( no key with matching key_type or key_id )
	eCxECode_10,
	eCxECode_11,
	eCxECode_12,
	eCxECode_13,
	eCxECode_14,
	eCxECode_Max
}CxErrorCode_t;

typedef enum
{
	eCxUTFlagFromTS	= 1,
	eCxUTFlagFromFlash,
}CxUTFlagFrom_t;
typedef enum
{
	eCxUTDisplayCntOnce	= 0,
	eCxUTDisplayCntMax,
}CxUTDisplayCount_t;

typedef enum
{
	eScReset_By_ISR,
	eScReset_By_Recovery,
}CxScReset_Reason_t;

typedef struct
{
	HUINT32		ulPidNum;
	CxEsType_t	eType[CX_MAX_NUM_OF_ESPID];
	HUINT16		usPid[CX_MAX_NUM_OF_ESPID];
} CxEsPidInfo_t;

typedef struct
{
	HINT32	nHostVer;
	HINT32	nSessionCnt;
	HINT32	nCountryInd;
	HINT32	nCaSysId;
	HUINT32	ulCardNumber;
	HUINT32	ulChipId;
	HUINT8* pszSwVer;
} CxVersionInfo_t;

typedef struct
{
	HUINT16	usYear;
	HUINT8	ucDay;
	HUINT8	ucMonth;

	HUINT8	ucHour;
	HUINT8	ucMinute;
} CxTime_t;

typedef struct
{
	HUINT8	aucCurrency[4];
	HUINT16	usPriceUnit;
	HUINT8	ucPriceDecimal;
} CxPrice_t;

typedef struct
{
	HUINT16		usSubscriptionRef;
	HUINT8		szLabel[CX_MAX_LABEL_LENGTH];
	CxTime_t	stStartDate[2];
	CxTime_t	stEndDate[2];
	HUINT8		szEntitlemnet[2][CX_MAX_ENTITLEMENT_LENGTH];
} CxSubscriptionStatus_t;

typedef struct
{
	HUINT32		ulEventTag;		// 24 bit
	HUINT8		szLabel[CX_MAX_LABEL_LENGTH];
	CxTime_t	stStartDate;
	CxTime_t	stEndDate;
	HUINT8		ucAuthType;
	HUINT16		usLeft;
} CxEventStatus_t;

typedef struct
{
	HUINT16	usPurseRef;
	HUINT8	szLabel[CX_MAX_LABEL_LENGTH];
	HUINT32	ulBalance;
} CxPurseStatus_t;

typedef struct
{
	HUINT32		ulEventTag;	//24 bit
	HUINT32		ulConsumedTokens;	//24 bit
	CxTime_t	stLastAccessTime;
	HUINT8		szLabel[CX_MAX_LABEL_LENGTH];
} CxDebitStatus_t;

typedef struct
{
	HUINT32	ulCreditTokens;
	HUINT8	szLabel[CX_MAX_LABEL_LENGTH];
} CxCreditStatus_t;

typedef union
{
	CxSubscriptionStatus_t	stSubscription;
	CxEventStatus_t			stEvent;
	CxPurseStatus_t			stPurse;
	CxDebitStatus_t			stDebit;
	CxCreditStatus_t		stCredit;
} CxStatus_t;

typedef struct _CxStatusList_t
{
	CxStatus_t				stStatus;
	struct _CxStatusList_t* pstNext;
} CxStatusList_t;

typedef struct
{
	HUINT8	ucProgram;
	HUINT8	ucSmartcard;
} CxMaturityRatingValue_t;

typedef struct
{
	HUINT32		ulProdRef;
	CxPrice_t	stPrice;
	HUINT8		szLabel[CX_MAX_LABEL_LENGTH];
} CxOrderInfo_t;

typedef struct
{
	HUINT32	ulProdRef;
	HUINT16	usMinuteLeft;
	HUINT8	szLabel[CX_MAX_LABEL_LENGTH];
} CxAcceptViewingInfo_t;

typedef struct
{
	HUINT16	usPurseRef;

	HUINT32	ulEventTag;	// 24 bits
	HUINT8	ucInsufficientTokens;
	HUINT8	ucTokenMode;

	HUINT32	ulEventCost;	// 24 bits
	HUINT8	szLabel[CX_MAX_LABEL_LENGTH];
} CxTokenDebitInfo_t;

typedef union
{
	CxMaturityRatingValue_t	stMaturityRating;
	CxOrderInfo_t			stOrder;
	CxAcceptViewingInfo_t	stAcceptViewing;
	CxTokenDebitInfo_t		stTokenDebit;
} CxUiInfo_t;


typedef struct
{
	CxUiMsgType_t	eUiMsgType;

	// Chipset Pairning Error Code
	HINT32			cpm_nErrCode;

	// Pull EMM Error Code
	HINT32			pullemm_nErrCode;

	// CxMaturityRatingValue_t
	HUINT8			mr_ucProgram;
	HUINT8			mr_ucSmartcard;

	// CxOrderInfo_t
	HUINT32			or_ulProdRef;
	HUINT8			or_aucCurrency[3];
	HUINT16			or_usPriceUnit;
	HUINT8			or_ucPriceDecimal;
	HUINT8			or_szLabel[CX_MAX_LABEL_LENGTH];

	//CxAcceptViewingInfo_t
	HUINT32			av_ulProdRef;
	HUINT16			av_usMinuteLeft;
	HUINT8			av_szLabel[CX_MAX_LABEL_LENGTH];

	//CxTokenDebitInfo_t
	HUINT16			td_usPurseRef;
	HUINT32			td_ulEventTag;	// 24 bits
	HUINT8			td_ucInsufficientTokens;
	HUINT8			td_ucTokenMode;
	HUINT32			td_ulEventCost;	// 24 bits
	HUINT8			td_szLabel[CX_MAX_LABEL_LENGTH];

	// CxFingerprint_t
	HUINT32			fp_ulTime;			// 0xffffff = immediately
	HUINT8			fp_ucCentTime[CX_MAX_CENTTIME_INDEX];
	HUINT16			fp_usDuration;		// 단위 centi-second (1/100 초)
	HUINT16			fp_usPosX;
	HUINT16			fp_usPosY;
	HUINT16			fp_usHeight;
	HUINT8			fp_szText[CX_MAX_FINGERPRINT + 1];
	HUINT8			fp_ucDisplayed;
	HBOOL			fp_bFPFromProtectedData;

	// CxUserText_t
	HUINT8			ut_szText[CX_MAX_USER_TEXT_LEN + 1];
	HUINT8			ut_ucDisplayed;
	HUINT16			ut_usDuration;		// 이 값이 0이면 메시지를 표시하지는 않고 왔다고만 알려준다 (mailbox message). 단위 second.
	HUINT8			ut_ucSeqNo;
	HUINT8			ut_ucRepeat;		// 없으면 1회만 출력
	HUINT16			ut_usInterval;		// 메시지를 표시하는 간격 (10초단위로 들어오나 *10하여 second단위로 저장하고 있음)
	HxDATETIME_t	ut_stStartDataTime;
	UNIXTIME		ut_ulImmediateStartTime;
} CxUiMsgInfo_t;

// CxUserText_t 구조체에 변동이 생기면 반드시 CX_NVM_USERTEXT_SLOT_SIZE 사이즈도 CxUserText_t 구조체 사이즈에 맞게 수정해야 함!!!
typedef struct
{
	HUINT8			szText[CX_MAX_USER_TEXT_LEN + 1];
	HUINT8			ucDisplayed;
	HUINT16			usDuration;		// 이 값이 0이면 메시지를 표시하지는 않고 왔다고만 알려준다 (mailbox message). 단위 second.
	HUINT8			ucSeqNo;
	HUINT8			ucRepeat;		// 없으면 1회만 출력
	HUINT16			usInterval;		// 메시지를 표시하는 간격 (10초단위로 들어오나 *10하여 second단위로 저장하고 있음)

	HxDATETIME_t	stStartDataTime;
	UNIXTIME		ulImmediateStartTime;
} CxUserText_t;


typedef struct
{
	HUINT32			ulCount;
	CxUserText_t* 	pstUserText;
} CxUserTextList_t;


typedef struct
{
	HUINT32	ulTime;				// 0xffffff = immediately
	HUINT8	ucCentTime[CX_MAX_CENTTIME_INDEX];		// [0]:Hour, [1]:Min, [2]:Sec, [3]:Cent
	HUINT16	usDuration;			// 단위 centi-second (1/100 초)
	HUINT16	usPosX;
	HUINT16	usPosY;
	HUINT16	usHeight;

	HUINT8	szText[CX_MAX_FINGERPRINT + 1];
	HUINT8	ucDisplayed;
	HBOOL	bFPFromProtectedData;
} CxFingerprint_t;


/*******************************************************************/
/********************      Public Functions 	********************/
/*******************************************************************/

/////////////////////////////////////////////////////////////////////
// provided by core
/////////////////////////////////////////////////////////////////////

// main
HERROR CXAPI_Initialize(void);
HERROR CXAPI_GoOperation(void);
HERROR CXAPI_WakeUpStandby(HBOOL bPowerSavingStandby);

// session
HERROR CXAPI_StartSvc(Handle_t hStbHandle);
HERROR CXAPI_StopSvc(Handle_t hStbHandle, HBOOL bNextSvcExist);
HERROR CXAPI_SetPid(Handle_t hStbHandle, CxEsPidInfo_t* pstCaDscInfo);
//HERROR CXAPI_SetPid(Handle_t hStbHandle, HUINT16 usPid, HBOOL bAdd);
HERROR CXAPI_UpdatePmt(Handle_t hStbHandle, SvcCas_PmtInfo_t* pstPmtInfo);
HERROR CXAPI_UpdateCat(Handle_t hStbHandle, HUINT16 usUniqueTsId, HUINT8* pucCat);
HERROR CXAPI_Select(Handle_t hStbHandle, HBOOL bSelect);
HBOOL CXAPI_IsLiveSessionOverrided(Handle_t hCurSvcHandle);
HERROR CXAPI_CheckCaPinOverride(Handle_t hStbHandle);

#if defined(CONFIG_MW_MM_DELAYED_REC)
HERROR CXAPI_MoveSvc(Handle_t hStbHandle);
#endif

#if defined(CONFIG_SUPPORT_FCC)
HERROR CXAPI_SetMainHandle(Handle_t hMainStbHandle, Handle_t hPrevStbHandle);
#endif

// sc
HERROR CXAPI_ScGetResetReason(HINT32 nScSlot, CxScReset_Reason_t *peReason);

// ui
HERROR CXAPI_UiClosed(CxUiMsgType_t eMsg);

// ui - ca pin check
HERROR CXAPI_UiEnterCaPin(HINT32 nScSlot, HUINT8* pucPin);

// ui - message (usertext) / fingerprint
//HERROR CXAPI_UiDisplayUserText(CxUserText_t *pstText);
HERROR CXAPI_UiUserTextDisplayed(HUINT8 ucSeqNo, CxUTDisplayCount_t ulOpt);
HERROR CXAPI_UiFingerprintDisplayed(void);
HERROR CXAPI_UiResetCompareFPTime(void);
HERROR CXUI_Trick_UserTextReceived(void);
HERROR CXUI_Trick_FingerprintReceived(void);

// ui - menu
HERROR CXAPI_UiGetSCState(HINT32 nScSlot);
CxStatusList_t* CXAPI_UiGetStatusInfo(HINT32 nScSlot, CxStatusSelectType_t eType, HUINT32 ulRef);
HERROR CXAPI_UiChangePin(HINT32 nScSlot, HUINT8* pucOldPin, HUINT8* pucNewPin, HUINT8* pucNewPin2);
HERROR CXAPI_UiGetMaturityRatingLevel(HINT32 nScSlot, HUINT8* pucMaturityRating);
HERROR CXAPI_UiChangeMaturityRating(HINT32 nScSlot, HUINT8 ucMaturityRating, HUINT8* pucPin);
HERROR CXAPI_UiGetUserText(HUINT32* pulNumMsg, CxUserTextList_t* pstList);
HERROR CXAPI_UiGetVersionInfo(HINT32 nScSlot, CxVersionInfo_t* pstInfo);
HERROR CXAPI_UiGetCountryIndicator(HINT32 nScSlot, HINT32* nCountryIdx);
void CXUI_SetUpdateSITimeStamp(void);
HERROR CXUI_SetRepeatImmediateMsgTime(CxUserText_t* pstText, UNIXTIME* unixtime);
HERROR CXAPI_UiCxMenuDisplayed(HBOOL bDisplayed);
HERROR CXAPI_UiPinMmiDisplayed(HBOOL bDisplayed);

// ui - maturity rating
HERROR CXAPI_UiAcceptMaturityRatingOverride(Handle_t hSession, HUINT8* pucPin);

// ui - ppv
HERROR CXAPI_UiStartOrderingPPV(Handle_t hSession, HUINT8* pucPin, HUINT32 ulProdId);
HERROR CXAPI_UiStartViewingPPV(Handle_t hSession, HUINT8* pucPin);
HERROR CXAPI_UiStartViewingTokenPPV(Handle_t hSession, HUINT8* pucPin);

CxStatusList_t* CXAPI_UiGetPurseBalance(Handle_t hSession, HUINT32 ulRef);
HERROR CXAPI_SetScPinForTokenPPV(Handle_t hSession, HUINT8* pucPin, CxUiMsgType_t* eReturnMsg);
HERROR CXAPI_SetScPin(Handle_t hAction, HUINT8* pucPin, CxUiMsgType_t eMsgType, CxUiMsgType_t* eReturnMsg);

// ui - return channel
// not yet

// pvr
HERROR CXAPI_PVR_GetMaturityRateValue(Handle_t hAction, HUINT8 *pucMaturityLevel);
HERROR CXAPI_PVR_GetMaturityRateValueFromURI(Handle_t hAction, HUINT32 ulCurUriIdx, HUINT8* pucMaturityRate);

#if defined(CONFIG_MW_CAS_CONAX_URI)
HERROR CXUI_UpdateUIForPlayback(Handle_t hAction, HBOOL bCardDisable, CxUiMsgInfo_t* s_stCxUiMsgInfo);
HERROR CXAPI_PVR_GetNextUriTimeFromMetaFile(Handle_t hAction, UNIXTIME* pulNextUriTime);
HERROR CXAPI_PVR_Pb_CheckAllUriRL(Handle_t hAction, UNIXTIME ulStartTime, HBOOL* pbExpiredAllUri);
#endif

HERROR CXAPI_PVR_Pb_Open(Handle_t hAction, HUINT32 ulContentsId, Handle_t hSvc);
HERROR CXAPI_PVR_Pb_GetSessionKey(Handle_t hAction, HUINT8* pucPbKey);
HERROR CXAPI_PVR_Pb_ChangeTimecode(Handle_t hAction, HUINT32 ulTimecode, HBOOL* pbNeedNewKey);
HERROR CXAPI_PVR_Pb_Close(Handle_t hAction);

HERROR CXAPI_PVR_Rec_Open(Handle_t hAction, HUINT32 ulContentsId, Handle_t hSvc, SvcCas_ActionType_e eActionType);
HERROR CXAPI_PVR_Rec_GetSessionKey(Handle_t hAction, HUINT8* pucPbKey);
HERROR CXAPI_PVR_Rec_UpdateSessionKeyToMetaFile(Handle_t hAction, HUINT32 ulTimecode, HUINT8* pucRecKey);
HERROR CXAPI_PVR_Rec_ChangeTimecode(Handle_t hAction, HUINT32 ulTimecode, HBOOL* pbNeedNewKey);
HERROR CXAPI_PVR_Rec_Close(Handle_t hAction);

#if defined(CONFIG_MW_MM_DELAYED_REC)
HERROR CXAPI_PVR_Rec_StartDelayed(Handle_t hAction, HUINT32 ulContentId, HUINT8 *pucOldPath);
#endif

HERROR CXAPI_PVR_GetCaPin(HUINT8* szCxPinCode);
HERROR CXAPI_PVR_SetCaPin(HUINT8* szCAPin);

#if defined(CONFIG_MW_MM_DLNA) && defined(CONFIG_MW_MM_DLNA_DMS)
HERROR CXAPI_PVR_DLNA_Pb_OpenMetaFile(const char* pathname);
HERROR CXAPI_PVR_DLNA_Pb_ChangeTimecode(HUINT32 ulHandle, HUINT32 ulTimecode, HBOOL* pbNeedNewKey);
HERROR CXAPI_PVR_DLNA_Pb_CloseMetaFile(HUINT32 ulHandle);
HERROR CXAPI_PVR_DLNA_Pb_GetSessionKeyByPath(const char* pathname, HUINT8* pucPbKey);
HERROR CXAPI_PVR_DLNA_Pb_GetSessionKey(HUINT32 ulHandle, HUINT8* pucPbKey);
HERROR CXAPI_PVR_DLNA_Pb_SetSessionKeyHandle(const char* pathname, HUINT32 ulHandle);
#endif
void CXAPI_NVM_ClearConaxUserData(void);
void xsvc_cas_CxCmdInit(HxCMD_Handle_t hCmdHandle);
HINT32 CXCMD_Execute(void *szArgList);
#endif /* !__CONAX_H__ */
