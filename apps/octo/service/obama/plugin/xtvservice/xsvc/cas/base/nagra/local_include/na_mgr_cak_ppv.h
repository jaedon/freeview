/*
@file				na_mgr_cak_ppv.h
@brief				na_mgr_cak_ppv.h (Nagra CAK Service[PPV/IPPV])

Description:  \n
				\n
Module: MW/CAS \n
Remarks :\n

Copyright (c) 2008 HUMAX Co., Ltd. \n
All rights reserved. \n
*/

#ifndef _CA_CAK_PPV_H
#define _CA_CAK_PPV_H

#include "ca_cak.h"

typedef enum
{
	eNA_PPV_State_None						= 0x00000000,
	eNA_PPV_State_Fail							= 0x00000001,
	eNA_PPV_State_Waiting						= 0x00000002,
	eNA_PPV_State_OutOfArea					= 0x00000004,
	eNA_PPV_State_PurchaseHistoryComplete		= 0x00000010,
	eNA_PPV_State_PackageListComplete			= 0x00000020,

	eNA_PPV_State_Max
}NA_PPV_State_t;

#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)  ||  defined(CONFIG_MW_CAS_NAGRA_OP_SPAIN_TDT)

#define NA_MEDIASET_PROVIDER_ID					0x8D11
#define NA_TDT_PROVIDER_ID				0x8000
#define NA_OTHER_PROVIDER_ID					0xFFF0 // 뭐로 하나...임시 임
#define NA_FAKE_PROVIDER_ID						0xFFFF // MHP를 위한 것으로, 이것을 사용 하면 무조건 CAK 요청 한다.

#define MAX_EVENT_ITEM_NAME_LENGTH 				(45+5)	//AP에서는 "42+..."자만 보여 주면 된다...Mediaset Phase3 Spec P36

#define MAX_PPV_PURCHASE_LIST_NAME_LENGTH 	(45+5)		//AP에서는 45자만 보여 주면 된다...Mediaset Phase3 Spec P36
#define MAX_MSET_PPV_DISCLAIMER_LENGTH 			255		//AP에서는 255만 보여 주면 된다.
#define MAX_MSET_PPV_DESCRIPTION_LENGTH 		255		//AP에서는 255만 보여 주면 된다.
#define MAX_PACKAGE_NAME_LENGTH 				45


/* 현재 current TS에 관한 TYPE 정의 */
typedef enum
{
	TS_CLEAR_NA_MEDIASET_PERIOD = 0, // clear Na Mediaset period
	TS_CLEAR_NA_OTHER_PERIOD, 			// claar NA other period
	TS_NA_MEDIASET_PERIOD, 				// scrambled, NA Mediaset
	TS_NA_TDT_PERIOD, 					// scrambled, NA spain tdt
 	TS_NA_OTHER_PERIOD,					// scrambled, NA other provider
 	TS_NOT_NA_PERIOD, 					// NOT NA
	TS_PERIOD_ERROR, 					//ERROR
	TS_PERIOD_Max
}TS_PERIOD_TYPE;

typedef enum
{
	eNAGRA_OP_TYPE_UNDEFINED = 0,
	eNAGRA_OP_TYPE_MEDIASET,
	eNAGRA_OP_TYPE_PANGEA,
	eNAGRA_OP_TYPE_Abertis,
	eNAGRA_OP_TYPE_Max
} TNAGRA_OP_TYPE;

typedef enum
{
	CA_EVENT_SPECIAL_EVENT = 1,
	CA_EVENT_PPT
} TPurchasableEventType;

typedef enum
{
	EVT_PF_PRESENT	=	0x01,
	EVT_PF_FOLLOWING	=	0x02
} EVT_PF_TYPE;

typedef enum
{
	CA_EVENT_SUCCESS,
	CA_EVENT_ERROR,
	CA_EVENT_PURCHASE_PERIOD_OVER,
	CA_EVENT_PURCHASE_PERIOD_BEFORE,
	CA_EVENT_TS_FAIL,
	CA_EVENT_NO_VALID_CREDIT	// Mediaset PPT
} TEventStatus;

typedef enum
{
	ePACKAGE_PURCHASABLE_NO_ERROR,
	ePACKAGE_ALREADY_PURCHASED,
	ePACKAGE_CARD_EXPIRED,
	ePACKAGE_PERIOD_ENDED,
	ePACKAGE_CREDIT_SHORTAGE,
	ePACKAGE_UNPURCHASABLE_TYPE_Max
} PACKAGE_UNPURCHASABLE_TYPE;

typedef enum
{
	ePACKAGE_SPORT = 1, 			// for Mediaset & Pangea
	ePACKAGE_SPORT_MORE = 2, 		// for Mediaset & Pangea
	ePACKAGE_FICTION = 3, 			// for Mediaset & Pangea
	ePACKAGE_FICTION_MORE= 4,		// for Mediaset & Pangea
	ePACKAGE_OTHERS = 5, 			// for Mediaset & Pangea
	ePACKAGE_OTHERS_MORE = 6, 		// for Mediaset & Pangea
	ePACKAGE_SPECIAL = 7,			// for  Pangea only
	ePACKAGE_HIDDEN = 8, 			// products that should not be proposed in the menu
	ePACKAGE_PAY_PER_TIME = 9, 	// ppt events, not proposed in menu
	ePACKAGE_OPPV_SUBSCRIPTION = 10, // used by mediaset, not proposed in menu
	//11~ 13은 subscription (used by Mediaset, not proposed in menu)
	ePACKAGE_CHILDREN = 14, 		// for  Pangea only
	ePACKAGE_CHILDREN_MORE = 15,	// for  Pangea only
	ePACKAGE_ADULT = 16,			// for  Pangea only
	ePACKAGE_ADULT_MORE = 17		// for  Pangea only
} PURCHASE_PACKAGE_TYPE;

typedef enum
{
	eNA_PPV_PURCHASE_RESULT_TYPE_SUCCESS = 0,
	eNA_PPV_PURCHASE_RESULT_TYPE_MEMORY_FULL,
	eNA_PPV_PURCHASE_RESULT_TYPE_CREDIT_SHORTAGE,
	eNA_PPV_PURCHASE_RESULT_TYPE_SUSPENDED_CARD,
	eNA_PPV_PURCHASE_RESULT_TYPE_TIME_EXPIRED,
	eNA_PPV_PURCHASE_RESULT_TYPE_INVALID_CARD,
	eNA_PPV_PURCHASE_RESULT_TYPE_Max
} PURCHASE_RESULT_TYPE;

typedef struct PROGRAM_INFO_STRUCT
{
	TCaRequest*				pRequest; //program request object
	TCaProgram*				pProgram;//현재 service의 program
	TCaRequest* 			pWatchRequest; //CA_REQUEST_TYPE_WATCH_PPT request
} PROGRAM_INFO;

typedef struct tagPPVEventInfo
{
	TUnsignedInt16			xEventID;
	TPurchasableEventType	xPurchaseType;		// PPV or PPT
	TEventType				xEventType; 		// PPV or IPPV
	UNIXTIME				xEventStartTime;	// start time in EIT
	UNIXTIME				duration;			// duration in EIT
	TCreditAmount			xCost;				// cost
	UNIXTIME				xPurchaseStartTime; // EIT(same as xEventStartTime), PDT(real purchase start time)
	UNIXTIME				xPurchaseEndTime;	// EIT(same as xEventStartTime + duration), PDT(real purchase end time)
	UNIXTIME				xPreviewTime;		// Preview time
	TUnsignedInt16			xSliceTime; 		// PPT (slice time)
	unsigned char			xEvtName[MAX_EVENT_ITEM_NAME_LENGTH+1]; // Event Name(EIT, PDT)
	TCaProductId			xProductId;
	TCaProduct				*pCaProduct;
	TCaRequest				*pCaRequest;
} PPV_EVENT_INFO;

typedef struct tEIT_EVENT_INFO
{
	TUnsignedInt16			wEvtId;
	TUnsignedInt8			ucEventType;
	TCalendarTime			ucStartTime;
	TUnsignedInt8			szName[32];
	UNIXTIME				startTime;
	UNIXTIME				duration;
} EIT_EVENT_INFO;

typedef struct EVENT_ITEM_STRUCT
{
	TUnsignedInt32			EvtNameLength;
	TChar					EvtName[MAX_EVENT_ITEM_NAME_LENGTH+1];
	TCalendarTime 			pStartTime;
}EVENT_ITEM;

typedef struct PACKAGE_INFO_STRUCT
{
	TCaRequest*				pRequest; 			//package request object
	TCaProduct*				pProduct;			//원하는 product
	TCaService*				pServiceFilter; 	//NULL이면 filter를 setting 하지 않고 모든 event return
	TSize					nDesSize;
	TUnsignedInt8			aucDescription[MAX_MSET_PPV_DESCRIPTION_LENGTH+1];
	TUnsignedInt32			ulNbEvents;
	EVENT_ITEM*				pEvents;
	TSize					nDisclaimerSize;
	TUnsignedInt8			aucDisclaimer[MAX_MSET_PPV_DISCLAIMER_LENGTH+1];
}PACKAGE_INFO;

typedef struct PPV_PURCHASE_LIST_ITEM_STRUCT
{
	TSize					EvtNameLength;
	TChar					EvtName[MAX_PPV_PURCHASE_LIST_NAME_LENGTH+1];
	TCalendarTime			PurchaseTime;
	TCreditAmount			Cost;
	TPurchaseFlags			PurchaseFlag;
	TCalendarTime			ExpirationDate;
	TCaProductId			ProductID;
} PPV_PURCHASE_LIST_ITEM;

typedef struct PPV_PURCHASE_LIST_STRUCT
{
	TPpid					providerID;
	TChar					providerName[MAX_PPV_PURCHASE_LIST_NAME_LENGTH+1];
	TUnsignedInt32			xNbItems;
	PPV_PURCHASE_LIST_ITEM* pList;
} PPV_PURCHASE_LIST;

typedef struct tagRechargeListItem
{
	TCaRechargeReason		Reason;
	TCalendarTime			Date;
	TCreditAmount			Amount;
} RECHARGE_LIST_ITEM;

typedef struct tagRechargeHistory
{
	TCaRequest*				pRequest;
	TUnsignedInt32			xNbItems;
	RECHARGE_LIST_ITEM*		pList;
} RECHARGE_LIST;

//curOPPVPurchaseInfo; 최신 전달 된 OPPV info 저장 . sNewPurchaseListenerCallBack
typedef struct OPPV_PURCHASE_INFO_ITEM_STRUCT
{
	TUnsignedInt8			*pName;
	TSize					nameLength;
	struct OPPV_PURCHASE_INFO_ITEM	*pNext;
}OPPV_PURCHASE_INFO_ITEM;

typedef struct OPPV_PURCHASE_INFO_STRUCT
{
	TUnsignedInt32			xNbItems;
	struct OPPV_PURCHASE_INFO_ITEM	*pOPPVItem;
} OPPV_PURCHASE_INFO;

typedef struct PACKAGE_LIST_ITEM_STRUCT
{
	TUnsignedInt32			PackageNameLength;
	TCaProduct*				pProduct;			//각 Product Handler
	TCaRequest*				pRequest; 			//package request object
	TCaProductId			ProductID;			// product ID
	TChar					PackageName[MAX_PACKAGE_NAME_LENGTH+1]; //AP에서는 30자만 보여 주면 된다. => Phase3에서는 45 자로 변경
	TCreditAmount			Cost;
	TUnsignedInt32			BusinessType;
	TCalendarTime			PurchaseStartTime;
	TCalendarTime			PurchaseEndTime;
	TProductFlags			PurchasableFlag;
	TUnsignedInt32			NumOfServices;
	TDvbTriplet*			pServices;
	PACKAGE_INFO 			PackageInfo;
	struct PPV_PACKAGE_LIST_ITEM* pNext;
} PPV_PACKAGE_LIST_ITEM;

typedef struct PACKAGE_LIST_STRUCT
{
	TCaRequest*				pRequest; //package request object
	TUnsignedInt32			xNbItems;
	PPV_PACKAGE_LIST_ITEM*	pList;
} PPV_PACKAGE_LIST;
#endif

#if	defined(CONFIG_MW_CAS_NAGRA_OP_TIVU)
typedef enum
{
	CA_EVENT_SUCCESS,
	CA_EVENT_ERROR,
	CA_EVENT_PURCHASE_PERIOD_OVER,
	CA_EVENT_PURCHASE_PERIOD_BEFORE,
	CA_EVENT_TS_FAIL,
	CA_EVENT_NO_VALID_CREDIT
} TEventStatus;
#endif

#if	defined(CONFIG_MW_CAS_NAGRA_OP_RCS)
#define MAX_EVENT_ITEM_NAME_LENGTH 				35
#define MAX_PPV_PURCHASE_LIST_NAME_LENGTH 		35

#define EXPIRED_EVENT_TIME						(1 * 60)

typedef enum
{
	CA_EVENT_SUCCESS,
	CA_EVENT_ERROR,
	CA_EVENT_PURCHASE_PERIOD_OVER,
	CA_EVENT_PURCHASE_PERIOD_BEFORE,
	CA_EVENT_TS_FAIL,
	CA_EVENT_NO_VALID_CREDIT
} TEventStatus;

typedef enum
{
	CA_EVENT_SPECIAL_EVENT = 1,
	CA_EVENT_PPT
} TPurchasableEventType;

typedef struct PPV_PURCHASE_LIST_ITEM_STRUCT
{
	TSize					EvtNameLength;
	TChar					EvtName[MAX_PPV_PURCHASE_LIST_NAME_LENGTH+1];
	TCalendarTime			PurchaseTime;
	TCreditAmount			Cost;
	TPurchaseFlags			PurchaseFlag;
	TCalendarTime			ExpirationDate;
	TCaProductId			ProductID;
} PPV_PURCHASE_LIST_ITEM;

typedef struct PPV_PURCHASE_LIST_STRUCT
{
	TPpid					providerID;
	TUnsignedInt32			xNbItems;
	PPV_PURCHASE_LIST_ITEM	*pList;
} PPV_PURCHASE_LIST;

typedef struct tagPPVEventInfo
{
	TUnsignedInt16			xEventID;
	TPurchasableEventType	xPurchaseType;		// PPV or PPT
	TEventType				xEventType; 		// PPV or IPPV
	UNIXTIME				xEventStartTime;	// start time in EIT
	UNIXTIME				duration;			// duration in EIT
	TCreditAmount			xCost;				// cost
	UNIXTIME				xPurchaseStartTime; // EIT(same as xEventStartTime), PDT(real purchase start time)
	UNIXTIME				xPurchaseEndTime;	// EIT(same as xEventStartTime + duration), PDT(real purchase end time)
	UNIXTIME				xPreviewTime;		// Preview time
	TUnsignedInt16			xSliceTime; 		// PPT (slice time)
	TUnsignedInt8			xEvtName[MAX_EVENT_ITEM_NAME_LENGTH+1]; // Event Name(EIT, PDT)
	TCaProductId			xProductId;
	TCaProduct				*pCaProduct;
	TCaRequest				*pCaRequest;
} PPV_EVENT_INFO;

typedef struct tEIT_EVENT_INFO
{
	TUnsignedInt16			wEvtId;
	TUnsignedInt8			ucEventType;
	TCalendarTime			ucStartTime;
	TUnsignedInt8			ucRunStatus;
	TUnsignedInt8			szName[32];
	UNIXTIME				startTime;
	UNIXTIME				duration;
} EIT_EVENT_INFO;
#endif


#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET) || defined(CONFIG_MW_CAS_NAGRA_OP_SPAIN_TDT)
typedef void (*TCardCreditCallback)(TUnsignedInt8 ucSlotId, TUnsignedInt8 eStatus, TCreditAmount nCredit);
typedef TUnsignedInt32 (*TPackageListNotifyCallback)(TUnsignedInt32 status, const TCaRequest* pRequest, TUnsignedInt32 nNumber, const void *aPackageList);
typedef TUnsignedInt32 (*TPackageInfoNotifyCallback)(TCaRequestStatus reqStatus, PACKAGE_INFO *aPackageInfo);
typedef TUnsignedInt32 (*TPurchasePackageNotifyCallback)(TCaProcessingStatus reqStatus);
typedef TUnsignedInt32 (*TFullInfoPackageListNotifyCallback)(TUnsignedInt32 status, const TCaRequest* pRequest, TUnsignedInt32 nNumber, const void *aPackageList);
typedef TUnsignedInt32 (*TCaListenerArrivedNotifyCallback)(TCaListenerType listenerType);
typedef TUnsignedInt32 (*TTSCheckNotifyCallback)(Handle_t svcHandle, TS_PERIOD_TYPE tsType, const TBoolean isSCMatchedWithPMT);
typedef TUnsignedInt32 (*TRequestEventNotifyCallback)(TEventStatus xStatus, const TCaEvent *pEvent, const PPV_EVENT_INFO *pEventInfo);
typedef TUnsignedInt32 (*TPurchaseHistoryNotifyCallback)(TUnsignedInt32 status, TUnsignedInt32 xNumberOfItems, const PPV_PURCHASE_LIST_ITEM* pxPurchaseList);
typedef TUnsignedInt32 (*TRechargeListNotifyCallback)(TUnsignedInt32 status, TUnsignedInt32 xNumberOfItems, const RECHARGE_LIST_ITEM* pxRechargeList);
typedef TUnsignedInt32 (*TOPPVPurchaseListNotifyCallback)(TCaObjectStatus objStatus, const OPPV_PURCHASE_INFO *pxOPPVPurchaseInfo);
typedef TUnsignedInt32 (*TRechargeNotifyCallback)(TUnsignedInt32 status, TUnsignedInt32 wParam, TUnsignedInt32 lParam);
typedef TUnsignedInt32 (*TPurchaseEventNotifyCallback)(TUnsignedInt32 xStatus, TUnsignedInt32 xPurchaseType);
#endif


#if defined(CONFIG_MW_CAS_NAGRA_OP_RCS)
typedef TCaStatus (*TRechargeNotifyCallback)(TUnsignedInt32 xStatus, TUnsignedInt32 wParam, TUnsignedInt32 lParam);
typedef TCaStatus (*TRequestEventNotifyCallback)(TUnsignedInt32 xStatus, TUnsignedInt32 hSvcHandle, TCaRequest *pRequest, PPV_EVENT_INFO *pEventInfo, TCaProduct *pProduct);
typedef TCaStatus (*TPurchaseEventNotifyCallback)(TUnsignedInt32 xStatus, TUnsignedInt32 xPurchaseType);
typedef TCaStatus (*TPurchaseHistoryNotifyCallback)(TUnsignedInt32 xStatus, TUnsignedInt32 xNumberOfItems, PPV_PURCHASE_LIST *pxPurchaseList);
#endif


#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
extern HERROR 		CA_NA_CakGetEcmCaSystemId(TUnsignedInt8 ucSlotId, TUnsignedInt16 *usCaSysId);
extern HERROR 		CA_NA_PpvGetProviderId(TPpid *pProviderId);
extern HERROR 		CA_NA_CakGetProviderId(TUnsignedInt8 ucSlotId, TPpid *pProviderId);
extern HERROR 		CA_NA_PpvRequestFullInfoPackageList(TPpid providerID);

extern HERROR 		CA_NA_PpvStartup(void);
extern TCaStatus 	CA_NA_PpvRequestCardCredit(void);
extern HERROR 		CA_NA_PpvGetScCredit(TCreditAmount  *pCredit);
extern void 		CA_NA_PpvSetScCredit(TCreditAmount  pCredit);
extern HERROR 		CA_NA_PpvRegisterScCreditCallback(TCardCreditCallback callback);
extern TCaStatus 	CA_NA_PpvProductLoadingListener(void);
extern HERROR 		CA_NA_PpvRegisterProductsLoadingCallback(TCaListenerArrivedNotifyCallback callback);
extern HERROR 		CA_NA_PpvRegisterTSCheckNotifyCallback(TTSCheckNotifyCallback callback);
extern HERROR 		CA_NA_PpvRegisterEventInfoNotifyCallback(TRequestEventNotifyCallback callback);
extern HERROR 		CA_NA_PpvRegisterRechargeListNotifyCallback(TRechargeListNotifyCallback callback);
extern HERROR 		CA_NA_PpvRegisterPackageListNotifyCallback(TPackageInfoNotifyCallback callback);
extern HERROR 		CA_NA_PpvRegisterFullInfoPackageListNotifyCallback(TPackageInfoNotifyCallback callback);
extern HERROR 		CA_NA_PpvRegisterPurchaseHistoryNotifyCallback(TPurchaseHistoryNotifyCallback callback);
extern HERROR 		CA_NA_PpvRegisterOppvPurchaseListNotifyCallback(TOPPVPurchaseListNotifyCallback callback);
extern HERROR 		CA_NA_PpvRegisterRechargeAlarmNotifyCallback(TRechargeNotifyCallback callback);
extern HERROR 		CA_NA_PpvRegisterRequestPurchaseEventNotifyCallback(TPurchaseEventNotifyCallback callback);
extern HERROR 		CA_NA_PpvRegisterRequestPurchasePackageNotifyCallback(TPurchasePackageNotifyCallback callback);

extern TCaRequestStatus  CA_NA_PpvCreateRequest(TCaRequestType type, TCaRequest** pRequest);
extern HERROR 		CA_NA_PpvDisposeRequest(TCaRequest* pRequest);
extern TCaStatus 	CA_NA_PpvRequestTsCheck(Handle_t hSvcHandle);
extern HERROR 		CA_NA_PpvRequestEventInfo(void);
extern TCaStatus 	CA_NA_PpvProductLoadedListener(void);
extern HERROR 		CA_NA_PpvNotifyTPChanged(TBoolean bTpChanged, Handle_t hAction);
extern HERROR 		CA_NA_PpvNotifySvcChanged(Handle_t hSvc, Handle_t hAction);
extern void 		CA_NA_CakReceivedNit(TUnsignedInt32 hAction, TUnsignedInt16 usUniqueId, TUnsignedInt8 *pucNit);
extern void  		CA_NA_CakReceivedTDT(void);
extern HERROR 		CA_NA_PpvRequestPurchaseHistory(TUnsignedInt32 ulProviderId);
extern HERROR 		CA_NA_PpvRequestRechargeList(void);
extern HERROR 		CA_NA_PpvRequestNewRecharge(void);
extern HERROR 		CA_NA_PpvRequestNewPurchase(TCaExportation* pxExportation);
extern HERROR 		CA_NA_PpvRequestPackageList(TUnsignedInt32 ulProviderId);
extern HERROR 		CA_NA_PpvRequestEventPurchase(TPurchasableEventType xStatus, TCaRequest* pRequest, TCaEvent *pEvent, TCaProduct *pProduct);
extern HERROR 		CA_NA_PpvRequestPackagePurchase(TCaProduct* pProduct);
extern HERROR 		CA_NA_PpvRequestProductPurchasable(TCaRequest* pRequest, TCaProduct*  pProduct);
extern HERROR 		CA_NA_PpvGetProductGetDescription(const TCaProduct* pxProduct, TSize* pStrLen, TUnsignedInt8** ppBuffer);
extern HERROR 		CA_NA_PpvGetProductGetDisclaimer(const TCaProduct* pxProduct, TSize* pStrLen, TUnsignedInt8** ppBuffer);
extern HERROR 		CA_NA_PpvGetPackageEventInfo(TCaRequest* pRequest, TCaProduct* pPackage, PACKAGE_INFO **pPackageInfo);
extern HERROR 		CA_NA_PpvGetPurchaseHistoryNum(TUnsignedInt32* pNbItems);
#endif
#if defined(CONFIG_MW_CAS_NAGRA_OP_SPAIN_TDT)
extern TUnsignedInt32	CA_NA_GetTdtPpvMsgQueueId(void);

extern HERROR		CA_NA_PpvNotifyTPChanged(TBoolean bTpChanged, Handle_t hAction);
extern void			CA_NA_CakReceivedNit(TUnsignedInt32 hAction, TUnsignedInt16 usUniqueId, TUnsignedInt8 *pucNit);
extern void			CA_NA_CakReceivedEit(TUnsignedInt32 hAction, TUnsignedInt16 usUniqueId, TUnsignedInt8 *pucEitP, TUnsignedInt8 *pucEitF);
extern void			CA_NA_CakReceivedTDT(void);
extern HERROR		CA_NA_PpvStartup(void);
extern TCaStatus	 	CA_NA_PpvRequestCardCredit(void);
extern TCaStatus		CA_NA_PpvProductLoadingListener(void);
extern TCaStatus		CA_NA_PpvProductLoadedListener(void);
extern HERROR		CA_NA_PpvRequestPurchaseHistory(TUnsignedInt32 ulProviderId);
extern HERROR		CA_NA_PpvRequestPackageList(TUnsignedInt32 ulProviderId);
extern HERROR		CA_NA_GetPPVStateInfo(NA_PPV_State_t *peState, HBOOL *pbNagraCaSys);
extern HERROR		CA_NA_GetPackageList(HUINT32 *pulNumOfProducts, PPV_PACKAGE_LIST_ITEM **ppPackageList);
extern HERROR		CA_NA_GetPurchaseHistoryList(HUINT32 *pulNumOfPurchase, PPV_PURCHASE_LIST_ITEM **ppPurchaseList);
extern HERROR		CA_NA_GetProviderName(HUINT32 *pulSize,HUINT8 **ppucProviderName);
extern HERROR		CA_NA_ReleaseProductList(PPV_PACKAGE_LIST_ITEM *pPackageList);
extern HERROR		CA_NA_ReleasePurchaseHistoryList(PPV_PURCHASE_LIST_ITEM *pPurchaseList);

#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_RCS)
extern HERROR		CA_NA_PPV_Startup(void);
extern HERROR 		CA_NA_PPV_Request(TUnsignedInt32 ulReqId);

extern HERROR		CA_NA_PPV_SetEpgMode(TBoolean xEpgMode);
extern HERROR		CA_NA_PPV_ForceCreatEventInfo(TUnsignedInt32 svcHandle);
extern HERROR		CA_NA_PPV_GetPpvList(TEventPurchaseListStatus* xStatus, TUnsignedInt8* xNumberOfItems, TEventPurchaseListItem** pxEventPurchaseListArray);
extern HERROR		CA_NA_PPV_GetCredit(TCreditStatus* xStatus, TUnsignedInt8* xNumberOfItems, TCreditItem** pxCreditArray);
extern HERROR		CA_NA_PPV_GetPurchaseEvtInfo(TUnsignedInt32 svcHandle, TUnsignedInt32 currTime, TUnsignedInt16 *eventId);
extern HERROR		CA_NA_PPV_RequestEventInfo(TUnsignedInt32 svcHandle, TUnsignedInt16 usEvtId);
extern HERROR		CA_NA_PPV_RequestEventInfoByEpg(TUnsignedInt32 svcHandle, TUnsignedInt16 usEvtId,
																TUnsignedInt32 *xStatus, TCaRequest **pRequest, PPV_EVENT_INFO **pEventInfo, TCaProduct **pProduct);
extern HERROR		CA_NA_PPV_RequestPurchaseEvent(TPurchasableEventType xType, TCaRequest* pRequest, TCaProduct *pProduct);
extern HERROR		CA_NA_PPV_RequestPurchaseEventByEpg(TPurchasableEventType xType, TCaRequest* pRequest, TCaProduct *pProduct, TCaProcessingStatus *pProStatus);
extern HERROR		CA_NA_PPV_CancelPurchaseEvent(TCaRequest* pRequest);
extern HERROR		CA_NA_PPV_RequestPurchaseHistory(void);
//------------------------------- NOTIFICATION ---------------------------------
extern void 		CA_NA_PPV_NotifyTPChanged(void);
extern void 		CA_NA_PPV_NotifyServiceChanged(TUnsignedInt32 hSvcHandle);
extern void 		CA_NA_PPV_NotifyEITpfArrived(TUnsignedInt32 hSvchandle);
extern void 		CA_NA_PPV_NotifyTDTArrived(TUnsignedInt32 hSvcHandle);
extern HERROR		CA_NA_PPV_RegisterCreditCallback(TCreditCallback callback);
extern HERROR		CA_NA_PPV_RegisterPpvListCallback(TEventPurchaseListCallback callback);
extern HERROR		CA_NA_PPV_RegisterRechargeNotifyCallback(TRechargeNotifyCallback callback);
extern HERROR		CA_NA_PPV_RegisterRequestPurchaseEventNotifyCallback(TPurchaseEventNotifyCallback callback);
extern HERROR		CA_NA_PPV_RegisterRequestEventNotifyCallback(TRequestEventNotifyCallback callback);
extern HERROR		CA_NA_PPV_RegisterPurchaseHistoryNotifyCallback(TPurchaseHistoryNotifyCallback callback);
extern TCaStatus	CA_NA_PPV_PpvListNotification(void);
extern TCaStatus	CA_NA_PPV_CreditNotification(void);

//Command 함수용.
extern void			CA_NA_PPV_PrintPfEvent(void);
#endif

#endif /* _CA_CAK_PPV_H */

