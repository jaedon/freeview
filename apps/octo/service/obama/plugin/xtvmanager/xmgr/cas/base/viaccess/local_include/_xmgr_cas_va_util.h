
/**
	@file     _xmgr_cas_va_util.h
	@brief    coordinate repository of conax util

	Description: Viaccess AP Util header file for Ray_k \n
	Module: Application(Layer), AP_CORE(module), VIACCESS \n
	Remarks : \n
	 Date			Modification							Name\n
	-----------	-------------------------------------	------------------\n
	 2009/07/24		init					 				jhlee\n

	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef __XMGR_CAS_VA_UTIL_H__
#define __XMGR_CAS_VA_UTIL_H__


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	AP_CAS_VA_PARAM_UNUSED(x) ((void)x)
#define	AP_CAS_VA_PARAM_WARRING_REMOVE(arg1, arg2, arg3, arg4)		\
	{													\
		(void)arg1;										\
		(void)arg2;										\
		(void)arg3;										\
		(void)arg4;				 						\
	}

#define		TEXT_SIZE64			64
#define		TEXT_SIZE128		128
#define		TEXT_SIZE256		256
#define		TEXT_SIZE512		512


#define	VA_MAX_USER_TEXT_LEN			244
#define	VA_MAX_FINGERPRINT				32

#define VA_MAX_LABEL_LENGTH				128
#define VA_MAX_ENTITLEMENT_LENGTH		8

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*
	LIST OF ITU-T RECOMMENDATION E.164 ASSIGNED COUNTRY CODES  - No. 835 - 1.V.200
	에 정의된 내용 중에 CONAX에서 제공하는 Code만 정의
*/

/*
	Check Pin-code Enter Type
*/

typedef enum {
	eVaCaStatus_AllSubscription = 0x0,
	eVaCaStatus_AllPpvEvent,
	eVaCaStatus_AllPurse,
	eVaCaStatus_Subscription = 0x5,
	eVaCaStatus_PpvEvent,
	eVaCaStatus_Purse,
	eVaCaStatus_Debit,
	eVaCaStatus_Credit
} VaStatusSelectType_t;

typedef enum {
	eVaUi_GroupBasic = 0,
	eVaUi_GroupMessage,
	eVaUi_GroupPpv,
	eVaUi_GroupMr,		// maturity rating
	eVaUi_GroupRc,		// return channel
	eVaUi_GroupMenu,
	eVaUi_GroupSc,
	eVaUi_GroupMax,
	eVaUi_GroupOther,
} VaUiPriority_t;

typedef enum {
		//eCxUi_GroupBasic_None,		// == access granted
		eVaUi_GroupBasic_NoAccess				= eVaUi_GroupBasic * 0x100,
		eVaUi_GroupBasic_NoAccessBlackOut,
		eVaUi_GroupBasic_NoAccessNetwork,

		//eCxUi_GroupMessage_None,
		eVaUi_GroupMessage_Display				= eVaUi_GroupMessage * 0x100,

		//eCxUi_GroupPpv_None,
		eVaUi_GroupPpv_OrderInfo				= eVaUi_GroupPpv * 0x100,
		eVaUi_GroupPpv_OrderConnect,
		eVaUi_GroupPpv_OrderFailed,
		eVaUi_GroupPpv_OrderAccepted,
		eVaUi_GroupPpv_OrderNotAccepted,
		eVaUi_GroupPpv_OrderClosed,
		eVaUi_GroupPpv_AcceptViewing,
		eVaUi_GroupPpv_TokenPpv,

		//eCxUi_GroupMr_None,
		eVaUi_GroupMr_Ask						= eVaUi_GroupMr * 0x100,
		eVaUi_GroupMr_WrongPin,
		eVaUi_GroupMr_PinLocked,

		//eCxUi_GroupRc_None,

		//eCxUi_GroupMenu_None,
		eVaUi_GroupMenu_Subscription			= eVaUi_GroupMenu * 0x100,
		eVaUi_GroupMenu_Event,
		eVaUi_GroupMenu_Purse,
		eVaUi_GroupMenu_Debit,
		eVaUi_GroupMenu_Credit,
		eVaUi_GroupMenu_ChangePin,
		eVaUi_GroupMenu_CheckNewPin,
		eVaUi_GroupMenu_WrongPin,
		eVaUi_GroupMenu_PinLocked,
		eVaUi_GroupMenu_Mr,
		eVaUi_GroupMenu_Modem,
		eVaUi_GroupMenu_Message,
		eVaUi_GroupMenu_Info,

		//eCxUi_GroupSc_None,	// == ready
		eVaUi_GroupSc_NoCard					= eVaUi_GroupSc * 0x100,
		eVaUi_GroupSc_Inserted,
		eVaUi_GroupSc_Incorrect,
		eVaUi_GroupSc_IncorrectConax,
		eVaUi_GroupSc_CommFail,

		//popup - priority에 영향을 받지 않고 동시에 표시할 수도 있는 것들..
		eVaUi_GroupOther_Fingerprint			= eVaUi_GroupOther * 0x100,
		eVaUi_GroupOther_MessageReceived,

		eVaUi_GroupAll_None,
} VaUiMsgType_t;

typedef struct {
	HINT32					nHostVer;
	HINT32					nSessionCnt;
	HINT32					nCountryInd;
	HINT32					nCaSysId;
	HUINT32					ulCardNumber;
	HUINT32					ulChipId;
	HUINT8					*pszSwVer;
} VaVersionInfo_t;

typedef struct {
	HUINT16					usYear;
	HUINT8					ucDay;
	HUINT8					ucMonth;
	HUINT8					ucHour;
	HUINT8					ucMinute;
} VaTime_t;

typedef struct {
	HUINT8					aucCurrency[3];
	HUINT16					usPriceUnit;
	HUINT8					ucPriceDecimal;
} VaPrice_t;

typedef struct {
	HUINT16					usSubscriptionRef;
	HUINT8 					szLabel[VA_MAX_LABEL_LENGTH];
	VaTime_t				stStartDate[2];
	VaTime_t				stEndDate[2];
	HUINT8					szEntitlemnet[2][VA_MAX_ENTITLEMENT_LENGTH];
} VaSubscriptionStatus_t;

typedef struct {
	HUINT32					ulEventTag;		// 24 bit
	HUINT8					szLabel[VA_MAX_LABEL_LENGTH];
	VaTime_t				stStartDate;
	VaTime_t				stEndDate;
	HUINT8					ucAuthType;
	HUINT16					usLeft;
} VaEventStatus_t;

typedef struct {
	HUINT16					usPurseRef;
	HUINT8					szLabel[VA_MAX_LABEL_LENGTH];
	HUINT32					ulBalance;
} VaPurseStatus_t;

typedef struct {
	HUINT32					ulEventTag;	//24 bit
	HUINT32					ulConsumedTokens;	//24 bit
	VaTime_t				stLastAccessTime;
	HUINT8					szLabel[VA_MAX_LABEL_LENGTH];
} VaDebitStatus_t;


typedef struct {
	HUINT32					ulCreditTokens;
	HUINT8					szLabel[VA_MAX_LABEL_LENGTH];
} VaCreditStatus_t;


typedef union {
	VaSubscriptionStatus_t 	stSubscription;
	VaEventStatus_t			stEvent;
	VaPurseStatus_t			stPurse;
	VaDebitStatus_t			stDebit;
	VaCreditStatus_t		stCredit;
} VaStatus_t;


typedef struct _VaStatusList_t {
	VaStatus_t				stStatus;
	struct _VaStatusList_t	*pstNext;
} VaStatusList_t;


typedef struct {
	HUINT8					ucProgram;
	HUINT8					ucSmartcard;
} VaMaturityRatingValue_t;


typedef struct {
	HUINT32 				ulProdRef;
	VaPrice_t 				stPrice;
	HUINT8 					szLabel[VA_MAX_LABEL_LENGTH];
} VaOrderInfo_t;


typedef struct {
	HUINT32 				ulProdRef;
	HUINT16					usMinuteLeft;
	HUINT8 					szLabel[VA_MAX_LABEL_LENGTH];
} VaAcceptViewingInfo_t;

typedef struct {
	HUINT16 				usPurseRef;
	HUINT32					ulEventTag;	// 24 bits
	HUINT8					ucInsufficientTokens;
	HUINT8					ucTokenMode;
	HUINT32					ulEventCost;	// 24 bits
	HUINT8 					szLabel[VA_MAX_LABEL_LENGTH];
} VaTokenDebitInfo_t;

typedef union {
	VaMaturityRatingValue_t	stMaturityRating;
	VaOrderInfo_t 			stOrder;
	VaAcceptViewingInfo_t	stAcceptViewing;
	VaTokenDebitInfo_t		stTokenDebit;
} VaUiInfo_t;


typedef struct{
	VaUiMsgType_t	eUiMsgType;

	// CxMaturityRatingValue_t
	HUINT8			mr_ucProgram;
	HUINT8			mr_ucSmartcard;

	// CxOrderInfo_t
	HUINT32 		or_ulProdRef;
	HUINT8			or_aucCurrency[3];
	HUINT16			or_usPriceUnit;
	HUINT8			or_ucPriceDecimal;
	HUINT8 			or_szLabel[VA_MAX_LABEL_LENGTH];

	//CxAcceptViewingInfo_t
	HUINT32 		av_ulProdRef;
	HUINT16			av_usMinuteLeft;
	HUINT8 			av_szLabel[VA_MAX_LABEL_LENGTH];

	//CxTokenDebitInfo_t
	HUINT16 		td_usPurseRef;
	HUINT32			td_ulEventTag;	// 24 bits
	HUINT8			td_ucInsufficientTokens;
	HUINT8			td_ucTokenMode;
	HUINT32			td_ulEventCost;	// 24 bits
	HUINT8 			td_szLabel[VA_MAX_LABEL_LENGTH];

	// CxFingerprint_t
	HUINT32			fp_ulTime;			// 0xffffff = immediately
	HUINT16			fp_usDuration;		// 단위 centi-second (1/100 초)
	HUINT16			fp_usPosX;
	HUINT16			fp_usPosY;
	HUINT16			fp_usHeight;
	HUINT8			fp_szText[VA_MAX_FINGERPRINT + 1];
	HUINT8			fp_ucDisplayed;

	// CxUserText_t
	HUINT8			ut_szText[VA_MAX_USER_TEXT_LEN + 1];
	HUINT8			ut_ucDisplayed;
	HUINT16			ut_usDuration;		// 이 값이 0이면 메시지를 표시하지는 않고 왔다고만 알려준다 (mailbox message). 단위 second.
	HUINT8			ut_ucSeqNo;
	HUINT8			ut_ucRepeat;		// 없으면 1회만 출력
	HUINT16			ut_usInterval;		// 메시지를 표시하는 간격 (10초단위로 들어오나 *10하여 second단위로 저장하고 있음)
	//DATE_TIME_T		ut_stStartDataTime;
} VaUiMsgInfo_t;

typedef enum
{
	eVA_OpMsg_Encodeing_Ascii = 0,
	eVA_OpMsg_Encodeing_UTF_8,
	eVA_OpMsg_Encodeing_UTF_16,
	eVA_OpMsg_Encodeing_UTF_32,
	eVA_OpMsg_Encodeing_GB2312_80,
} eCasVa_OpMsg_EncodingType;

/*******************************************************************/
/********************      	Global Prototype	********************/
/*******************************************************************/
extern void			AP_CAS_VA_GetWordFromString(HUINT8 *pszDest, HUINT8 *pszSrc, HUINT8 nIndex);

#ifdef CONFIG_DEBUG
extern HERROR		AP_CAS_VA_CMD(HUINT8 *pszCmd, HUINT8 *pszSubCmd);
#endif

// MMI - MENU

/* Command msg Process */

#endif /* __XMGR_CAS_VA_UTIL_H__ */
/* end of file */
