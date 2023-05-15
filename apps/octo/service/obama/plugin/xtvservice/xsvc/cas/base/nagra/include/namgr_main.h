/*******************************************************************
	File Description
********************************************************************/
/**
	@file	   \n
	@brief	   \n

	Description:   \n
	Module: MW/CAS \n

	Copyright (c) 2008 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

#ifndef	__NAMGR_MAIN_H__
#define	__NAMGR_MAIN_H__


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "na_glob.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define MAX_BDC_LANGUAGE				15
#define MAX_BDC_STRING_SIZE				500

#if defined(CONFIG_MW_CAS_NAGRA_OP_TIVU)
#define 			NA_CAS_MEDIASET_RANGE_FROM		0x1802
#define 			NA_CAS_MEDIASET_RANGE_TO		0x1805
#define 			NA_CAS_TIVU_RANGE_FROM			0x183D
#define 			NA_CAS_TIVU_RANGE_TO				0x183F
#define 			NA_CAS_TIVU_TEST_CA_SYSTEM_ID	0x1803
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
// Alarm message data전송용 struct (OK, FAIL)
typedef enum tagNaAlarmType
{
	eAlarm_Unknown = 0,
	eAlarm_None,
	eAlarm_Access,
	eAlarm_Smartcard,
	eAlarm_PPV,
	eAlarm_Max
} NaAlarmType_t;

typedef struct tagNaScAlarmItem
{
 	TUnsignedInt32    	ulSlotId;
	TSmartcardState	eAlarm;
   	TBoolean          	bChanged;
} NaScAlarmItem_t;

typedef struct tagNaAccessAlarmItem
{
  	TUnsignedInt32  	ulSlotId;
	TCaAccess		eAlarm;
  	TUnsignedInt16 	usSvcId;
  	TBoolean			bChanged;
	TUnsignedInt8		ucElemType;
} NaAccessAlarmItem_t;

typedef struct tagNaAccessAlarmList
{
	TUnsignedInt32		hAction;
	NaAlarmType_t		eAlarmType;

	TBoolean				bRunStatus;
	TUnsignedInt8			ucNumOfStreams;
	NaAccessAlarmItem_t	stAccessAlarmItems[MAX_TOTAL_NO_STREAM];
} NaAccessAlarmList_t;

typedef struct tagNaAlarmData
{
	TUnsignedInt32		hAction;
	NaAlarmType_t		eAlarmType;

	TUnsignedInt8			ucNumOfScAlarms;
	NaScAlarmItem_t		*pstScAlarmItems;

	TUnsignedInt8			ucNumOfAccessAlarms;
	NaAccessAlarmItem_t	*pstAccessAlarmItems;
} NaAlarmData_t;

// Command message data전송용 struct
typedef enum tagNaCmdType
{
	eCmd_PopUp = 0,
	eCmd_Bdc,
	eCmd_IrdCmd,
	eCmd_Cop,
	eCmd_End,
	eCmd_Max
} NaCmdType_t;

typedef enum tagNaBdcCmd
{
	eBdc_Display,
	eBdc_Remove,
	eBdc_Cmd_Max
} NaBdcCmd_t;

typedef enum tagNaBdcType
{
	eBdc_Always,
	eBdc_User,
	eBdc_Timeout,
	eBdc_Type_Max
} NaBdcType_t;

typedef struct tagNaBdcContent
{
	TChar   			xLang[3];
	TSize			ulNumBytes;
	TUnsignedInt8		ucText[MAX_BDC_STRING_SIZE];
} NaBdcContent_t;

typedef struct tagNaBdcItem
{
	NaBdcCmd_t		eBdcCmd;
	NaBdcType_t 		eBdcType;
	TUnsignedInt8 	ucNumOfLang;
	NaBdcContent_t 	stBdcContent[MAX_BDC_LANGUAGE];
} NaBdcItem_t;

typedef NaPopupUI_t	NaPopupItem_t;

typedef struct tagNaIrdCmdItem
{
	NA_IRDCMD_ID_t			eIrdCmd;
	NA_IRDCMD_OPCODE_t	eIrdOpCode;
	TUnsignedInt32			ulParam1;
	TUnsignedInt32			ulParam2;
} NaIrdCmdItem_t;

typedef struct tagDrmCopItem
{
	TUnsignedInt8		ucByte_A;
	TUnsignedInt8		ucByte_B;
} NaDrmCopItem_t;

typedef struct tagNaCmdData
{
	TUnsignedInt32		hAction;
	NaCmdType_t			eCmdType;
	union
	{
		NaBdcItem_t		*pstBdcItem;
		NaPopupItem_t	*pstPopupItem;
		NaIrdCmdItem_t	*pstIrcCmdItem;
		NaDrmCopItem_t	*pstDrmCopItem;
	} item;
} NaCmdData_t;


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
extern HERROR NAMGR_SetSessionStatus(TUnsignedInt32 hAction, TBoolean bRunStatus);
extern HERROR NAMGR_GetLastAlarmData(TUnsignedInt32 hAction, SvcCas_MsgType_e *peMsgType, DxCAS_GroupId_e *peCasId, void **ppMessage, SvcCas_ShowType_e *peShowType);

extern HERROR MW_CA_NA_RequestInfo(TUnsignedInt32 ulReqId);
extern HERROR MW_CA_NA_RegisterSystemInfoCallback(TSystemInfoCallback callback);
extern HERROR MW_CA_NA_RegisterLocInfoCallback(TLocationInfoCallback callback);
extern HERROR MW_CA_NA_RegisterScAlarmCallback(TSmartcardAlarmCallback callback);
extern HERROR MW_CA_NA_GetSystemInfo(TSystemInfoStatus *xStatus, TIrdInfo **pxIrdInfo,
 											TUnsignedInt8 *xNumberOfSmartcards, TSmartcardInfoItem **pxSmartcardInfoArray);
extern HERROR MW_CA_NA_GetLocInfo(TLocationInfoStatus *xStatus, TUnsignedInt8 *xNumberOfSmartcards, TLocationInfoItem **pxLocationInfoArray);
extern HERROR MW_CA_NA_GetChipsetInfo(NaChipsetInfo_t **pstChipInfo);
#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ALME)
extern HERROR MW_CA_NA_GetCscMaxIndex(TUnsignedInt8 *pxCscMaxIndex);
#endif
#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
HERROR MW_CA_NA_GetAccessErrorCode(TUnsignedInt32 ulCurrentSessionId, TUnsignedInt32* pAccessErrorCode);
#endif
#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME)
extern HERROR MW_CA_NA_GetNumberOfIcc(TUnsignedInt8 *pxNumberOfIcc);
#endif
extern HERROR MW_CA_NA_CheckPincode(TPinIndex xPinIndex, TPinCode xPinCode, TUnsignedInt8 *pucPinCode, TPinCodeStatus *xStatus);
extern HERROR MW_CA_NA_ChangePincode(TPinIndex xPinIndex, TPinCode xOldPinCode, TPinCode xNewPinCode, TUnsignedInt8 *pucOldPinCode, TUnsignedInt8 *pucNewPinCode, TPinCodeStatus *xStatus);

#if defined(CONFIG_OP_CAS_NAGRA_CAKTEST_PPT)
extern HERROR MW_CA_NA_Cak7_EnablingConsumptionForced(TBoolean bOneShot);
#endif

#if 0
extern HERROR MW_CA_NA_GetProviderId(TUnsignedInt8 ucSlotId, TPpid *pProviderId);
extern HERROR MW_CA_NA_GetEcmCaSystemId(TUnsignedInt8 ucSlotId, TUnsignedInt16 *usCaSysId);
#endif
extern HERROR MW_CA_NA_GetCurrScStatus(TUnsignedInt32 ulSlotId, TSmartcardState *peScAlarm);
extern HERROR MW_CA_NA_ReportCurrScStatus(TUnsignedInt32 hAction);
extern HERROR MW_CA_NA_ReportCurrAlarmData(void);

extern HERROR MW_CA_NA_GetNumberOfSlots(TUnsignedInt8 *pucSlots);
extern HERROR MW_CA_NA_IsSameCasIdInSc(TUnsignedInt8 ucSlotId, TUnsignedInt16 usCaSysId);

extern HERROR MW_CA_NA_ResetIRDCmdData(void);

// for ird cmd mail
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
extern HERROR MW_CA_NA_IRDCMD_MAIL_CheckUnreadMessage(void);
extern HERROR MW_CA_NA_IRDCMD_MAIL_ClearMailMessageHeader(void);
extern HERROR MW_CA_NA_IRDCMD_MAIL_MakeMailMessage(TUnsignedInt8 *pIrdCmd, TUnsignedInt16 *pnMailId, TBoolean *pfCompleted, TUnsignedInt16 *pnDeletedMailId);
extern HERROR MW_CA_NA_IRDCMD_MAIL_LoadMailMessages(void);
extern HERROR MW_CA_NA_IRDCMD_MAIL_GetMailPriority(TUnsignedInt16 nMailId, NA_MAIL_PRIORITY *pnPriority);
extern HERROR MW_CA_NA_IRDCMD_MAIL_GetMailList(NA_MAIL_MSG ***pppMailList, TUnsignedInt32 *pnCount);
extern HERROR MW_CA_NA_IRDCMD_MAIL_GetMailData(TUnsignedInt16 nMailId, NA_MAIL_MSG **ppMailData);
extern HERROR MW_CA_NA_IRDCMD_MAIL_DeleteMailMessage(TUnsignedInt16 nMailId);
extern HERROR MW_CA_NA_IRDCMD_MAIL_UpdateReadFlag(TUnsignedInt16 nMailId, TBoolean bRead);
#endif

// for ird cmd popup msg
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_POPUP)
extern HERROR MW_CA_NA_IRDCMD_POPUP_DisplayPopupMessage(void);
#endif

// for ird cmd emm wake-up
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
extern HERROR MW_CA_NA_IRDCMD_EMM_GetWakeupTime(UNIXTIME *pulTime);
extern HERROR MW_CA_NA_IRDCMD_EMM_InitWakingTable(void);
extern HERROR MW_CA_NA_IRDCMD_EMM_HasMoreNextWaking(TBoolean *pbNext);
extern HERROR MW_CA_NA_IRDCMD_EMM_GetWakingTuneInfo(TUnsignedInt8 *pucNetworkIdFlag, TUnsignedInt16 *pusNetworkId,
																	TUnsignedInt8 *pucTsIdFlag, TUnsignedInt16 *pusTsId,
																	TUnsignedInt8 *pucServiceIdFlag, TUnsignedInt16 *pusServiceId);
extern HERROR MW_CA_NA_IRDCMD_EMM_GetWakingTimer(UNIXTIME *pulTime);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_ZONEID)
extern HERROR MW_CA_NA_GetZoneId(TUnsignedInt8* pZoneId, TUnsignedInt32 unMaxLen);
extern HERROR MW_CA_NA_GetCountryCode(TUnsignedInt8* pCountryCode, TUnsignedInt32 unMaxLen);
#endif

// for aps spec
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
extern HERROR MW_CA_NA_GetZipCode(TUnsignedInt8 ucSlotId, TUnsignedInt32 *pulZipCode);
extern HERROR MW_CA_NA_GetScExpirationDate(TUnsignedInt8 ucSlotId, TBoolean *pbEndOfTime, TCalendarTime *pstDataTime);
extern HERROR MW_CA_NA_IsHdPlusSmartCard(TUnsignedInt8 ucSlotNo, TBoolean *pbHdPlusSc);
extern HERROR MW_CA_NA_IsHdPlusService(TUnsignedInt32 hAction);
extern HERROR MW_CA_NA_GetPdExpirationDuration(TCaDuration *expireDuraion);
extern HERROR MW_CA_NA_GetAccessStatus(TCaAccess *accessStatus);
extern HERROR MW_CA_NA_GetSizeOfNaspCaDesc(TUnsignedInt32 hAction, TUnsignedInt8 *pucLen);
extern HERROR MW_CA_NA_GetContentOfNaspCaDesc(TUnsignedInt32 hAction, TUnsignedInt8 *pucDesc);
extern HERROR MW_CA_NA_GetValidityHdProduct(TRightType eType, TUnsignedInt32 ucDscLen, TUnsignedInt8 *pucDesc, TBoolean *pbExpired, TCaDuration *ulDuration);
extern HERROR MW_CA_NA_GetContentOfMacCaDesc(TUnsignedInt32 hAction, TUnsignedInt16 usScCaSysId, TUnsignedInt8 *pucDesc);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET) || defined(CONFIG_MW_CAS_NAGRA_OP_SPAIN_TDT)
extern HERROR MW_CA_NA_RegisterScCreditCallback(TCardCreditCallback callback);
extern HERROR MW_CA_NA_RegisterProductsLoadingCallback(TCaListenerArrivedNotifyCallback callback);
extern HERROR MW_CA_NA_RegisterPurchaseHistoryCallback(TPurchaseHistoryNotifyCallback callback);
extern HERROR MW_CA_NA_RegisterRechargeListCallback(TRechargeListNotifyCallback callback);
extern HERROR MW_CA_NA_RegisterOppvPurchaseListCallback(TOPPVPurchaseListNotifyCallback callback);
extern HERROR MW_CA_NA_RegisterRechargeAlarmCallback(TRechargeNotifyCallback callback);
extern HERROR MW_CA_NA_RegisterEventInfoCallback(TRequestEventNotifyCallback callback);
extern HERROR MW_CA_NA_RegisterPackageListCallback(TPackageListNotifyCallback callback);
extern HERROR MW_CA_NA_RegisterFullInfoPackageListNotifyCallback(TPackageListNotifyCallback callback);
extern HERROR MW_CA_NA_RegisterRequestPurchaseEventCallback(TPurchaseEventNotifyCallback callback);
extern HERROR MW_CA_NA_RegisterRequestPurchasePackageCallback(TPurchasePackageNotifyCallback callback);
extern HERROR MW_CA_NA_RegisterDgtviScAlarmCallback(TSmartcardAlarmCallback callback);

extern HERROR MW_CA_NA_GetScExpireDateForPpv(TUnsignedInt8 *pszDate, TBoolean *bExpired);
extern HERROR MW_CA_NA_GetCurrentDateStr(TUnsignedInt8 *pszDate);
extern HERROR MW_CA_NA_GetScCreditStr(TUnsignedInt8 *pszCredit);
extern HERROR MW_CA_NA_GetScCredit(TCreditAmount *pCredit);
extern HERROR MW_CA_NA_NotifyTPChanged(TBoolean	bTpChanged);
extern HERROR MW_CA_NA_NotifySvcChanged(Handle_t	hSvc, Handle_t hAction);
extern HERROR MW_CA_NA_RequestPurchaseHistory(TUnsignedInt32 ulProviderId);
extern HERROR MW_CA_NA_RequestRechargeList(void);
extern HERROR MW_CA_NA_CakGetProviderId(TPpid *pProviderId);
extern HERROR MW_CA_NA_RequestFullInfoPackageList(TPpid providerId);
extern HERROR MW_CA_NA_RequestPackageList(TUnsignedInt32 ulProviderId);
extern HERROR MW_CA_NA_RequestEventPurchase(TPurchasableEventType xStatus, TCaRequest* pRequest, TCaEvent *pEvent, TCaProduct *pProduct);
extern HERROR MW_CA_NA_RequestPackagePurchase(TCaProduct* pProduct);
extern HERROR MW_CA_NA_RequestProductPurchasable(TCaRequest* pRequest, TCaProduct*  pProduct);
extern HERROR MW_CA_NA_GetProductGetDescription(const TCaProduct* pxProduct, TSize* pStrLen, TUnsignedInt8** ppBuffer);
extern HERROR MW_CA_NA_GetProductGetDisclaimer(const TCaProduct* pxProduct, TSize* pStrLen, TUnsignedInt8** ppBuffer);
extern HERROR MW_CA_NA_GetPackageEventInfo(TCaRequest* pRequest, TCaProduct* pPackage, PACKAGE_INFO **pPackageInfo);
extern HERROR MW_CA_NA_GetPurchaseHistoryNum(TUnsignedInt32* pNbItems);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_SPAIN_TDT)
extern HERROR MW_CA_NA_GetPPVStateInfo(NA_PPV_State_t *peState, HBOOL *pbNagraCaSys);
extern HERROR MW_CA_NA_GetPackageList(HUINT32 *pulNumOfProducts, PPV_PACKAGE_LIST_ITEM **ppPackageList);
extern HERROR MW_CA_NA_GetPurchaseHistoryList(HUINT32 *pulNumOfPurchase, PPV_PURCHASE_LIST_ITEM **ppPurchaseList);
extern HERROR MW_CA_NA_GetProviderName(HUINT32 *pulSize, HUINT8 **ppucProviderName);
extern HERROR MW_CA_NA_ReleaseProductList(PPV_PACKAGE_LIST_ITEM *pPackageList);
extern HERROR MW_CA_NA_ReleasePurchaseHistoryList(PPV_PURCHASE_LIST_ITEM *pPurchaseList);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_RCS)
extern HERROR MW_CA_NA_PPV_SetEpgMode(TBoolean xEpgMode);
extern HERROR MW_CA_NA_PPV_ForceCreatEventInfo(TUnsignedInt32 svcHandle);
extern HERROR MW_CA_NA_PPV_GetPpvList(TEventPurchaseListStatus* xStatus, TUnsignedInt8* xNumberOfItems, TEventPurchaseListItem** pxEventPurchaseListArray);
extern HERROR MW_CA_NA_PPV_GetCredit(TCreditStatus* xStatus, TUnsignedInt8* xNumberOfItems, TCreditItem** pxCreditArray);
extern HERROR MW_CA_NA_PPV_GetPurchaseEvtInfo(TUnsignedInt32 svcHandle, TUnsignedInt32 currTime, TUnsignedInt16 *pusEventId);
extern HERROR MW_CA_NA_PPV_RequestEventInfo(TUnsignedInt32 svcHandle, TUnsignedInt16 nEvtId);
extern HERROR MW_CA_NA_PPV_RequestEventInfoByEpg(TUnsignedInt32 svcHandle, TUnsignedInt16 nEvtId,
															TUnsignedInt32 *xStatus, TCaRequest **pRequest, PPV_EVENT_INFO **pEventInfo, TCaProduct **pProduct);
extern HERROR MW_CA_NA_PPV_RequestPurchaseEvent(TPurchasableEventType xType, TCaRequest* pRequest, TCaProduct *pProduct);
extern HERROR MW_CA_NA_PPV_RequestPurchaseEventByEpg(TPurchasableEventType xType, TCaRequest* pRequest, TCaProduct *pProduct, TCaProcessingStatus *pProStatus);

extern HERROR MW_CA_NA_PPV_RegisterCreditCallback(TCreditCallback callback);
extern HERROR MW_CA_NA_PPV_RegisterPpvListCallback(TEventPurchaseListCallback callback);
extern HERROR MW_CA_NA_PPV_RegisterRechargeNotifyCallback(TRechargeNotifyCallback callback);
extern HERROR MW_CA_NA_PPV_RegisterRequestPurchaseEventNotifyCallback(TPurchaseEventNotifyCallback callback);
extern HERROR MW_CA_NA_PPV_RegisterRequestEventNotifyCallback(TRequestEventNotifyCallback callback);
extern HERROR MW_CA_NA_PPV_RegisterPurchaseHistoryNotifyCallback(TPurchaseHistoryNotifyCallback callback);
#endif

#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
extern HERROR MW_CA_NA_GetSlotIDbySessionID(TIccSessionId iccSessionId, TUnsignedInt32 *pulSlotId);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_TIVU)
extern HBOOL MW_CA_NA_CheckTivuService(void);
extern HERROR MW_CA_NA_SetTivuService(HBOOL bTivu);
extern HBOOL MW_CA_NA_CheckTivuCardSystemId(void);
#endif

#if defined (CONFIG_MW_CAS_NAGRA_PRM_3_1)
HERROR  caSessionMgr_AddTransportSessionIdDirect(TTransportSessionId xTsId);
HERROR	caSessionMgr_RemoveTransportSessionIdDirect(TTransportSessionId xTsId);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
extern void	NAMGR_SetParentalControl(Handle_t hAction, TBoolean bUserPinPass);
extern void NAMGR_SetParentalControlLevel(void);
#endif

extern HUINT8 *NAMGR_GetPairingData(void);
extern HERROR NAMGR_FactoryDefault(void);
extern HERROR NAMGR_SetMetaDescriptor(TTransportSessionId ulSessionId, MetaTagType_t eMetaType, TUnsignedInt8 *pucMeta, TUnsignedInt8 ucMetaSize);

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST) && !defined(CONFIG_DEBUG)
extern void MW_CA_NA_InitTerminalForRelease(void);
extern void MW_CA_NA_PrintForRelease(const HUINT8 *pxString);
#endif

#endif /* !__NAMGR_MAIN_H__ */


