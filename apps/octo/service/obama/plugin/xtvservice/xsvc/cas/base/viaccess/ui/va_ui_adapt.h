/*
	Viaccess UI header
		User API Manual ACS 3.0 - Doc. Ref. 2218
		STB Interface Guidelines ACS 3.0 - Doc. Ref. 2219
*/

#ifndef VA_UI_ADAPT_H
#define VA_UI_ADAPT_H

#include "htype.h"
//#include <casmgr_common.h>
//#include "../../../local_include/_xsvc_cas.h"

#define VA_UI_UC_SIMPLE							1
#define VA_UI_UC_OP_MESSAGE						2
#define VA_UI_UC_CAMLOCK							3
#define VA_UI_UC_SECURITY_RESTRICT				4
#define VA_UI_UC_MATURITY_RATING					5
#define VA_UI_UC_PREVIEW							6
#define VA_UI_UC_IPPV								7
#define VA_UI_UC_RIGHTS_CONSULT					9
#define VA_UI_UC_PURCHASE_LIST_CONSULT			10
#define VA_UI_UC_OP_MESSAGE_CONSULT				12
#define VA_UI_UC_TECH_INFO_CONSULT				13
#define VA_UI_UC_PIN_CODE_MODIFY					14
#define VA_UI_UC_MATURITY_RATING_MANAGE			15
#define VA_UI_UC_ACCOUNT_PURCHASE_OPTION		16
#define VA_UI_UC_DELETE_PRESEL_AREA				17
#define VA_UI_UC_ADD_PRESEL_AREA					21
#define VA_UI_UC_NON_ACTIVE_SESSION				18
#define VA_UI_UC_TERMINAL_ACTIVATION				19
#define VA_UI_UC_TRACE								20

typedef enum
{
    VA_UI_DESCRAMBLING_OK                            = 0x8020,
    VA_UI_DESCRAMBLING_OK_RIGHT_OK,
    VA_UI_DESCRAMBLING_OK_PREVIEW,
    VA_UI_DESCRAMBLING_OK_PREVIEW_AND_IPPV_OFFER,

    VA_UI_DESCRAMBLING_KO                            = 0x8040,
    VA_UI_DESCRAMBLING_KO_NO_RIGHT,
    VA_UI_DESCRAMBLING_KO_MATURITY_RATING,
    VA_UI_DESCRAMBLING_KO_GEOGRAPHICAL_BLACKOUT,
    VA_UI_DESCRAMBLING_KO_MISSING_KEY,
    VA_UI_DESCRAMBLING_KO_IPPV_OFFER,
    VA_UI_DESCRAMBLING_KO_IPPV_OFFER_EXPIRED,
    VA_UI_DESCRAMBLING_KO_MATURITY_RATING_AND_IPPV_OFFER,
    VA_UI_DESCRAMBLING_KO_FORBIDDEN_MODE,
    VA_UI_DESCRAMBLING_KO_INSUFFICIENT_CREDIT,
    VA_UI_DESCRAMBLING_KO_RIGHT_EXPIRED,
    VA_UI_DESCRAMBLING_KO_RIGHT_CONSUMED,
    VA_UI_DESCRAMBLING_KO_CW_PROTECTION_MISMATCH,

    VA_UI_FREE_TO_AIR                                = 0x8080,
    VA_UI_VIACCESS_NOT_APPLICABLE,
    VA_UI_VIACCESS_SMARTCARD_NOT_FOUND,
    VA_UI_VIACCESS_TERMINAL_NOT_ACTIVATED,

    VA_UI_SMARTCARD_INSERTED                         = 0x8100,
    VA_UI_SMARTCARD_EXTRACTED,
    VA_UI_SMARTCARD_READY,
    VA_UI_SMARTCARD_NEEDED,
    VA_UI_SMARTCARD_FAILED,
    VA_UI_SMARTCARD_NOT_ACCEPTED,
    VA_UI_SMARTCARD_INSUFFICIENT_MEMORY,
    VA_UI_SMARTCARD_INVALIDATED,

    VA_UI_NEW_OPERATOR_MESSAGE                       = 0x8200,
    VA_UI_NEW_OPERATOR_DATA,
    VA_UI_CAMLOCK_ACTIVATED_OK,
    VA_UI_CAMLOCK_ACTIVATED_KO,
    VA_UI_CAMLOCK_ACTIVATED_NO_CARD,
    VA_UI_CAMLOCK_NOT_ACTIVATED,
    VA_UI_CAMLOCK_DISABLED,
    VA_UI_START_SECURITY_RESTRICTION,
    VA_UI_STOP_SECURITY_RESTRICTION,
    VA_UI_PAIRING_MISMATCH,
    VA_UI_OVERDRAFT_REACHED ,
    VA_UI_THRESHOLD_REACHED,

    VA_UI_RECORD_RIGHT_OK                            = 0x8400,
    VA_UI_TIMESHIFT_RIGHT_OK,
    VA_UI_RECORD_RIGHT_KO,
    VA_UI_TIMESHIFT_RIGHT_KO,
    VA_UI_RECORD_INFO_CHANGE,

    VA_UI_ORM_CONNECTION_OK                          = 0x8800,
    VA_UI_ORM_CONNECTION_KO,
    VA_UI_CW_SERVER_CONNECTION_OK,
    VA_UI_CW_SERVER_CONNECTION_KO,
    VA_UI_KS_SYNCHRO_START,
    VA_UI_KS_SYNCHRO_STOP,

    VA_UI_TERMINAL_NOT_ACTIVATED                     = 0x9000,
    VA_UI_TERMINAL_READY,
    VA_UI_TERMINAL_INSUFFICIENT_MEMORY,
    VA_UI_TERMINAL_CONFIGURATION_ERROR,
    VA_UI_TERMINAL_INVALIDATED,
    VA_UI_TERMINAL_PERSONALIZATION_OK,
    VA_UI_TERMINAL_PERSONALIZATION_KO,

	VA_UI_FREE_TO_AIR_SERVICE,
	VA_UI_DESCRAMBLE_SERVICE,
	VA_UI_OTHER_DESCRAMBLE_SERVICE,

	VA_UI_START_SERVICE,
	VA_UI_STOP_SERVICE,

#ifdef CONFIG_MW_CAS_VIACCESS_FINGERPRINT
	VA_UI_SHOW_FINGERPRINT                           = 0x9100,// fingerprint
	VA_UI_HIDE_FINGERPRINT,						// fingerprint
#endif

#ifdef CONFIG_MW_CAS_VIACCESS_FREECHANNEL_BLOCK
	VA_UI_FREE_CHANNEL_BLOCK,
	VA_UI_FREE_CHANNEL_BLOCK_STOP,
#endif

    VA_UI_UNKNOWN    /* Only use to know the end */

} VA_UI_EVENT_TYPE;

typedef enum
{
	VA_ES_VIDEO = 0,
	VA_ES_AUDIO,
	VA_ES_TELETEXT,
	VA_ES_SUBTITLE,
	VA_ES_OTHER,
	VA_ES_ALL,
	VA_ES_NONE
} VA_ES_TYPE;

#define VA_UI_ERR_OK						 0
#define VA_UI_ERR							-1
#define VA_UI_ERR_CONSULTATION_LOCKED	-2

#ifndef VA_ERR_UI
#define VA_ERR_UI 							(-1000)
#endif

#define VA_ERR_UI_WRONG_PIN_CODE											(VA_ERR_UI - 1)
#define VA_ERR_UI_LOCKED_PIN_CODE											(VA_ERR_UI - 2)
#define VA_ERR_UI_MAX_CHANGES_PIN_CODE									(VA_ERR_UI - 3)
#define VA_ERR_UI_FORBIDDEN_PIN_CODE										(VA_ERR_UI - 4)
#define VA_ERR_UI_NO_PIN_CODE												(VA_ERR_UI - 5)
#define VA_ERR_UI_SMARTCARD_INSUFFICIENT_MEMORY						(VA_ERR_UI - 6)
#define VA_ERR_UI_WRONG_SLOT												(VA_ERR_UI - 7)
#define VA_ERR_UI_SMARTCARD_ERROR										(VA_ERR_UI - 8)
#define VA_ERR_UI_CONSULTATION_LOCKED									(VA_ERR_UI - 9)
#define VA_ERR_UI_INVALID_MATURITY_RATING								(VA_ERR_UI - 10)
#define VA_ERR_UI_OPERATION_NOT_ACCEPTED								(VA_ERR_UI - 11)
#define VA_ERR_UI_PRESELECTION_AREA_FULL									(VA_ERR_UI - 12)
#define VA_ERR_UI_REFERENCE_ALREADY_STORED_IN_PRESELECTION_AREA		(VA_ERR_UI - 13)
#define VA_ERR_UI_REFERENCE_NOT_FOUND_IN_PRESELECTION_AREA			(VA_ERR_UI - 14)
#define VA_ERR_UI_PRESELECTION_MODE_NOT_ACCEPTED						(VA_ERR_UI - 15)
// VA_IPPV_SURVEY
#define VA_ERR_UI_RESOURCE_BUSY											(VA_ERR_UI - 16)
#define VA_ERR_UI_CONNECTION_ERROR										(VA_ERR_UI - 17)
// PVR
#define VA_ERR_UI_WRONG_PVR_NO_RIGHT									(VA_ERR_UI - 18)


/*
typedef enum
{
    eVA_UI_SMARTCARD_INSERTED_STATE,
    eVA_UI_SMARTCARD_EXTRACTED_STATE,
    eVA_UI_SMARTCARD_READY_STATE,
    eVA_UI_SMARTCARD_FAILED_STATE,
    eVA_UI_SMARTCARD_NOT_ACCEPTED_STATE,
    eVA_UI_SMARTCARD_INVALIDATED_STATE

} tVA_UI_SmartcardState;
*/

/*
typedef enum
{
    VA_UI_NO_MODE           = 0,
    VA_UI_AUTOPAIRING_MODE  = 1,
    VA_UI_PAIRING_MODE      = 4
} VA_UI_PAIRING_MODE;
*/
#define VA_UI_NO_MODE				0
#define VA_UI_AUTOPAIRING_MODE	1
#define VA_UI_PAIRING_MODE			4

/*
typedef enum
{
    eVA_UI_THEME_LEVEL_REFERENCE =1,
    eVA_UI_CLASS_REFERENCE,
    eVA_UI_PREBOOKED_REFERENCE,
    eVA_UI_IPPVP_REFERENCE,
    eVA_UI_IPPVT_REFERENCE

} tVA_UI_ReferenceType;
*/
#define VA_UI_THEME_LEVEL_REFERENCE		1
#define VA_UI_CLASS_REFERENCE				2
#define VA_UI_PREBOOKED_REFERENCE		3
#define VA_UI_IPPVP_REFERENCE				4
#define VA_UI_IPPVT_REFERENCE				5

#define VA_UI_DELETE		0
#define VA_UI_WRITE			1

#define VA_UI_UA_SIZE				5		//kVA_UI_UA_SIZE
#define VA_UI_MAX_SO_LABLE			16		//kVA_UI_MAX_LABEL_SIZE
#define VA_UI_MAX_CLASS_MARK		32	//kVA_UI_MAX_CLASS_MASK_SIZE
#define VA_UI_OP_MSG_SIZE			256	// kVA_UI_MAX_OPERATOR_ITEM_SIZE
#define VA_UI_OP_DATA_SIZE			256	// kVA_UI_MAX_OPERATOR_ITEM_SIZE
#define VA_UI_CURRENCY_LABEL_SIZE	16	//kVA_UI_MAX_CURRENCY_LABEL_SIZE

typedef struct {
	VA_UI_EVENT_TYPE 	eEventType;
	HUINT32 			ulScSlot;
	HUINT32 			ulAcsId;
	HUINT32 			ulStbStreamHandle;
	void 				*pUserArg;
	HUINT32 			bIsScrambledByProgram;
	HINT32 					nStreamNumber;
	HINT32					nStreamType;
	HUINT32				ulTimerId;
	VA_ES_TYPE			eDscrErrEsType;
} VA_UI_EVENT;

typedef struct {
	VA_UI_EVENT			stLastEvent;
	VA_UI_EVENT_TYPE	eEventOk;
	VA_UI_EVENT_TYPE	eEventKo;
	HINT32				nReceivedEvtNum;
	HUINT32				ulTimerId;
	SvcCas_MsgType_e 		eMsgTypeOk;
	SvcCas_MsgType_e 		eMsgTypeKo;
	SvcCas_ShowType_e		eShowType;
} VA_UI_INFO;

typedef struct {
	HUINT8 aucUa[VA_UI_UA_SIZE*2+1];
} VA_UI_UA;

typedef struct {
	HUINT32 ulSoid;
	HUINT8 szLable[VA_UI_MAX_SO_LABLE+1];
} VA_UI_OPERATOR_LIST;

typedef struct {
	HUINT8 ucSize;
	HUINT8 aucClassMark[VA_UI_MAX_CLASS_MARK];
	HUINT8 szStartDate[11];	// 3+3+4+1 = 11
	HUINT8 szEndDate[11];
} VA_UI_CLASS_LIST;

typedef struct {
	HUINT8 ucTheme;
	HUINT8 ucLevel;
	HUINT8 szStartDate[11];	// 3+3+4+1 = 11
	HUINT8 szEndDate[11];
} VA_UI_THEME_LEVEL_LIST;

typedef struct {
	HUINT32 ulMin;
	HUINT32 ulMax;
} VA_UI_PREBOOKED_LIST;

typedef struct {
	HUINT32 ulSoid;
	HUINT32 ulEncode;
	HUINT32 ulCount;
	HUINT8 szMsg[VA_UI_OP_MSG_SIZE + 1];
} VA_UI_OP_MESSAGE;

typedef struct {
	HUINT32 ulSoid;
	HUINT32 ulCount;
	HUINT8 aucData[VA_UI_OP_DATA_SIZE + 1];
} VA_UI_OP_DATA;

#ifdef CONFIG_MW_CAS_VIACCESS_IPPV
typedef struct {
	HINT32 ulInteger;
	HUINT16 usFraction;
} VA_UI_PRICE;

typedef struct {
	HUINT32 ulProgNum;
	VA_UI_PRICE stCost;
} VA_UI_IPPV_ITEM;

typedef struct {
	HUINT32 ulAcsId;
	HUINT32 ulStbStreamHandle;
	HUINT32 ulSoid;
	HUINT32 ulPaymentMode;
	HUINT8 szCurrency[VA_UI_CURRENCY_LABEL_SIZE + 1];
	VA_UI_PRICE stRemainingCredit;
	HBOOL bIsIPPVPAgreementNeeded;
	HBOOL bIsIPPVPPresent;
	HUINT32 ulProgNumIPPVP;
	VA_UI_PRICE stCostIPPVP;
	HBOOL bIsIPPVTAgreementNeeded;
	HBOOL bIsIPPVTPresent;
	HUINT32 ulProgNumIPPVT;
	VA_UI_PRICE stCostIPPVT;
	VA_UI_PRICE stCeiling;
} VA_UI_IPPV_INFO;
#endif

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
typedef struct {
	HUINT32 ulPlaybackDuration;
	HUINT32 ulMaxNumberOfPlayback;
	HUINT8 szExpireDate[11];	// 3+3+4+1 = 11
} VA_UI_RECORD_INFO;

typedef struct {
	HUINT32 ulPlaybackCounter;
	HUINT8 szRecordDate[20];	// 3+3+4+3+3+3+1 = 20
	HUINT8 szFirstPlaybackDate[20];	// 3+3+4+3+3+3+1 = 20
} VA_UI_PLAYBACK_INFO;
#endif

void VA_UI_Adapt_Init();
void VA_UI_Adapt_Exit();
void VA_UI_Adapt_SetMsgQid(HUINT32 ulMsgQId);
void VA_UI_Adapt_SendEvent(VA_UI_EVENT *pEvent, SvcCas_MsgType_e eMsgType, SvcCas_ShowType_e eShowType);
void VA_UI_Adapt_SendLastEvent(int nAcsId);

#ifdef CONFIG_MW_CAS_VIACCESS_FINGERPRINT
void VA_UI_Adapt_ShowFingerprint(HUINT8 *pszFingerprintStr);
void VA_UI_Adapt_HideFingerprint();
#endif

#ifdef CONFIG_MW_CAS_VIACCESS_FREECHANNEL_BLOCK
void VA_UI_Adapt_FreeChannelBlock(int fBlock);
#endif

HUINT8 *VA_UI_Adapt_GetStbId();
HUINT8 *VA_UI_Adapt_GetUA(HINT32 nScSlot);
HUINT8 *VA_UI_Adapt_GetVersionName();
HUINT8 *VA_UI_Adapt_GetCamlockStr();
HINT32 VA_UI_Adapt_GetPairingMode();
HINT32 VA_UI_Adapt_GetPairingSC();
VA_UI_UA *VA_UI_Adapt_GetPairingUaList(HUINT32 *pulNumUa);
HINT32 VA_UI_Adapt_GetLabelFromSoid(HINT32 nScSlot, HUINT32 ulSoid, HUINT8 *pszLabel);
VA_UI_OPERATOR_LIST *VA_UI_Adapt_GetSoidList(HINT32 nScSlot, HUINT32 *pulNumSoid);
VA_UI_CLASS_LIST *VA_UI_Adapt_GetClassList(HINT32 nScSlot, HUINT32 ulSoid, HUINT32 *pulNumList, HINT32 *pnReturnValue);
VA_UI_THEME_LEVEL_LIST *VA_UI_Adapt_GetThemeLevelList(HINT32 nScSlot, HUINT32 ulSoid, HUINT32 *pulNumList, HINT32 *pnReturnValue);
VA_UI_PREBOOKED_LIST *VA_UI_Adapt_GetPrebookedList(HINT32 nScSlot, HUINT32 ulSoid, HUINT32 *pulNumList, HINT32 *pnReturnValue);
HINT32 VA_UI_Adapt_UnlockConsultation(HINT32 nScSlot, HUINT8 *pPinCode);
HINT32 VA_UI_Adapt_GetPinCodeAttempts(HINT32 nScSlot, HUINT8 *pucRemainingCount, HUINT8 *pucMaxCount);
HINT32 VA_UI_Adapt_ModifyPinCode(HINT32 nScSlot, HUINT8 *pOldPin, HUINT8 *pNewPin);
HINT32 VA_UI_Adapt_GetMaturityRating(HINT32 nScSlot, HUINT8 *pucMaturityRating, HUINT8 *pucMaxMaturityRating);
HINT32 VA_UI_Adapt_ModifyMaturityRating(HINT32 nScSlot, HUINT8 *pPinCode, HUINT8 ucMaturityRating);
HINT32 VA_UI_Adapt_AcceptMaturityRating(HINT32 nAcsId, HINT32 nScSlot, HUINT8 *pPinCode);
void *VA_UI_Adapt_GetPreselection(HINT32 nScSlot, HINT32 *ulCount, HINT32 *pnReturnValue);
void *VA_UI_Adapt_GetOpPreselection(HINT32 nScSlot, HUINT32 ulSoid,  HINT32 *pulCount, HINT32 *pnReturnValue);
void VA_UI_Adapt_GetPreselectionItemData(void *pPreselection,
												HINT32 nIndex,
												HUINT32 *pulType,
												HUINT32 *pulSoid,
												HUINT32 *pulValue1,
												HUINT32 *pulValue2,
												HUINT8 *pszStr);
HINT32 VA_UI_Adapt_DeletePreselection(HINT32 nScSlot, HUINT8 *pPinCode, void *pInfo, HINT32 nIndex);
HINT32 VA_UI_Adapt_AddPreselection(HINT32 nScSlot,
							HUINT8 *pPinCode,
							HUINT32 ulType,
							HUINT32 ulSoid,
							HUINT32 ulValue1,
							HINT32 lValue2,
							HUINT16 usValue3,
							HUINT16 usValue4,
							HUINT16 usValue5);

HINT32 VA_UI_Adapt_GetSecurityState(HINT32 nScSlot);
HINT32 VA_UI_Adapt_GetCamlockLevel(HINT32 nScSlot);
HINT32 VA_UI_Adapt_GetCamlockState(HINT32 nScSlot);
HINT32 VA_UI_Adapt_GetSmartcardState(HINT32 nScSlot);

VA_UI_OP_MESSAGE *VA_UI_Adapt_GetOpMessage(HINT32 nScSlot);
VA_UI_OP_MESSAGE *VA_UI_Adapt_GetLastOpMessage();
VA_UI_OP_DATA *VA_UI_Adapt_GetOpData(HINT32 nScSlot);

HINT32 VA_UI_Adapt_GetManufacturerId();
HINT32 VA_UI_Adapt_GetModelId();
HUINT8 *VA_UI_Adapt_GetStbSerialStr();
HUINT8 *VA_UI_Adapt_GetHwVersionStr();
HINT32 VA_UI_Adapt_GetMaturityRatingValue(HUINT32 ulAcsId, HUINT32 ulStbStreamHandle, HUINT8 *pucMaturityRatingValue);
HINT32 VA_UI_Adapt_GetPreviewInfo(HUINT32 ulAcsId,
								HUINT32 ulStbStreamHandle,
								HUINT8* pucUsedCwNumber,
								HUINT8* pucMaxCwNumber);

#ifdef CONFIG_MW_CAS_VIACCESS_IPPV
VA_UI_IPPV_ITEM *VA_UI_Adapt_GetIPPVTList(HINT32 nScSlot, HUINT32 ulSoid, HUINT32 *pulNumList, HINT32 *pnReturnValue);
VA_UI_IPPV_ITEM *VA_UI_Adapt_GetIPPVPList(HINT32 nScSlot, HUINT32 ulSoid, HUINT32 *pulNumList, HINT32 *pnReturnValue);
HUINT8 *VA_UI_Adapt_GetCurrencyStr();
int VA_UI_Adapt_GetCreditInfo(HINT32 nScSlot, HUINT32 ulSoid, HUINT8 *pszCurrency, VA_UI_PRICE *pstPrice);
int VA_UI_Adapt_GetIPPVInfo(HINT32 nAcsId, HINT32 nScSlot, HUINT32 ulStbStreamHandle, VA_UI_IPPV_INFO *pInfo);
int VA_UI_Adapt_AcceptIPPVP(HINT32 nAcsId, HINT32 nScSlot, HUINT32 ulStbStreamHandle, HUINT8 *pPinCode, HUINT32 ulProgNum);
int VA_UI_Adapt_AcceptIPPVT(HINT32 nAcsId, HINT32 nScSlot, HUINT32 ulStbStreamHandle, HUINT8 *pPinCode, HUINT32 ulProgNum, VA_UI_PRICE *pstCeiling);
int VA_UI_Adapt_GetThreshold(HINT32 nScSlot, VA_UI_PRICE *pstThreshold);
int VA_UI_Adapt_ModifyThreshold(HINT32 nScSlot, HUINT8 *pPinCode,  VA_UI_PRICE *pstThreshold);

#ifdef CONFIG_MW_CAS_VIACCESS_IPPV_SURVEY
int VA_UI_Adapt_StartIPPVSurvey(HINT32 nScSlot, HUINT32 ulSoid);
int VA_UI_Adapt_CancelIPPVSurvey();
#endif	// CONFIG_MW_CAS_VIACCESS_IPPV_SURVEY
#endif	// CONFIG_MW_CAS_VIACCESS_IPPV

#if 1 // for camlock control

#define VA_CAMLOCK_CONTROL_NO		0 // 현재 코드 구현상 편의를 위해 추가한 값이지 실제 VA 모델에서 쓰일 수 없음.
#define VA_CAMLOCK_CONTROL_LOW		1 // CAMLOCK_ACTIVATED_KO 상황에서 black screen + not compliant 배너 표시 + 채널 전환 가능 (단, 스크램블 채널인 경우에 한해 배너는 계속 표시) + FTA 시청 가능 + 배너 닫고 메뉴 진입 가능 (BXR-HD)
#define VA_CAMLOCK_CONTROL_MEDIUM	2 // CAMLOCK_ACTIVATED_KO 상황에서 black screen + not compliant 배너 표시 + 채널 전환 가능 (단, 스크램블 채널 상관 없이 배너는 계속 표시) + FTA 시청 불가(?) + 배너 닫고 메뉴 진입 가능 (현재로선 해당 모델 없음)
#define VA_CAMLOCK_CONTROL_HIGH		3 // CAMLOCK_ACTIVATED_KO 상황에서 black screen + not compliant 배너 표시 + 채널 전환 안됨 + FTA 시청 불가 + 메뉴 진입 안됨 (VA-5SD, VHDR-3000S)

int VA_GetCamLockControlLevel(void);
void VA_SetCamLockControlLevel(int); // for test only

#endif

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
int VA_UI_Adapt_GetRecordInfo(HINT32 nAcsId, HUINT32 ulStbStreamHandle, VA_UI_RECORD_INFO *pstInfo);
int VA_UI_Adapt_GetPlaybackInfoNow(VA_UI_PLAYBACK_INFO *pstInfo);
int VA_UI_Adapt_GetPlaybackInfo(HINT32 nAcsId, VA_UI_PLAYBACK_INFO *pstInfo);
int VA_UI_Adapt_IncrementPlaybackCount(HINT32 nAcsId);
void VA_UI_Adapt_AutoPinForRecord(HUINT32 ulAcsId, HUINT32 ulScSlot);
#endif //CONFIG_VA_PVR

#endif //VA_UI_ADAPT_H


