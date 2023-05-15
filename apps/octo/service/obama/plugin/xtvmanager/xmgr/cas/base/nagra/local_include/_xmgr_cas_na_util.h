#ifndef	__xmgr_cas_NaUTIL_H__
#define	__xmgr_cas_NaUTIL_H__


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <bus.h>

#include "ca_cak.h"
#if defined (CONFIG_MW_CAS_NAGRA_PRM)
#include "ca_dvl.h"
#endif

#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PPV)
#include "na_mgr_cak_ppv.h"
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
#include "na_mgr_cak_irdcmd.h"
#endif

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
#define			NA_UI_SC_NUMBER_STRING_LEN				64
#define			NA_UI_CA_NUMBER_STRING_LEN				64
#define			NA_UI_CA_VERSION_STRING_LEN				64
#define			NA_UI_SC_VERSION_STRING_LEN				64
#define 		NA_UI_IRDCMD_MAX_MSG_LEN				50

#define			NA_CAS_APS_MSG_T_BLOCK					0
#define			NA_CAS_APS_MSG_T_DIALOG					1

#if defined(CONFIG_MW_CAS_NAGRA_OP_TIVU)
#define 		NA_CAS_TIVU_PROVIDER_ID					0x5411
#define 		NA_CAS_TIVU_TEST_PROVIDER_ID			0x101

#define 		NA_CAS_MEDIASET_RANGE_FROM			0x1802
#define 		NA_CAS_MEDIASET_RANGE_TO			0x1805
#define 		NA_CAS_TIVU_RANGE_FROM				0x183D
#define 		NA_CAS_TIVU_RANGE_TO				0x183F
#endif

typedef enum
{
	eCAS_PIN_STATE_OK = 0x00,
	eCAS_PIN_STATE_NOACCESS,	// can't access
	eCAS_PIN_STATE_NORIGHT,		// fail to access pin more than three times
	eCAS_PIN_STATE_NOTIME
} eCAS_PIN_STATE;


typedef enum
{
	eCAS_CMD_IRDMSG_UPDATED = 0x00,
	eCAS_CMD_IRDMSG_USERSETTING,
	eCAS_CMD_IRD_EEPROM_CLEAR,
	eCAS_CMD_PIN_EEPROM_CLEAR,
	eCAS_CMD_BDC_CREATE
} eNA_AP_CAS_CMD;


#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
typedef enum
{
	eCAS_NA_UI_APS_UPDATE_OK = 0x00,
	eCAS_NA_UI_APS_UPDATE_PRV
} eCAS_NA_UI_APSUpdate;
#endif


typedef enum
{
	eNaSysInfoItem_Project_Name = 0,
	eNaSysInfoItem_iccVersion,
	eNaSysInfoItem_caVersion,
	eNaSysInfoItem_iccSerial_Number,
#if	defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	eNaSysInfoItem_Smartcard_Credit,				// Remaining time to expired for pre-paid smart card.
#endif
	eNaSysInfoItem_CA_Serial_Number,
	eNaSysInfoItem_Chipset_Nuid,
	eNaSysInfoItem_Chipset_Type,
	eNaSysInfoItem_Chipset_Rev,
#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ALME)
	eNaSysInfoItem_CSC_Index,
#endif
#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
	eNaSysInfoItem_iccVersion_2nd,
	eNaSysInfoItem_iccSerial_Number_2nd,
#endif
#if defined(CONFIG_MW_CAS_NAGRA_PRM)
	eNaSysInfoItem_DVL_Version,
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	eNaSysInfoItem_DVL_ChipsetType,
#endif
#endif
	eNaSysInfoItem_Smartcard_Status,
	eNaSysInfoItem_Item_Max
} XmgrCas_NaSystemInfoItem_e;


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
//using pin-code
HERROR			xmgr_cas_NaPincodeInit(void);
eCAS_PIN_STATE	xmgr_cas_NaGetPincodeAccessibleState(HUINT32 *pulRemainTime, HUINT8 *pnRetryCount);
HERROR			xmgr_cas_NaCheckSmartCardPin(HUINT8* szPicode);
HERROR			xmgr_cas_NaGetAccessibleState(HUINT8 *pnAccessState, HUINT32 *pulRemainTime, HUINT8 *pnRetryCount);
HERROR			xmgr_cas_NaVerifyPincodeGetAccessibleState(HUINT8 *pnAccessState, HUINT32 *pulRemainTime, HUINT8 *pnRetryCount, HUINT8* szPincode);
HERROR			xmgr_cas_NaChangeSmartCardPin(HUINT8* szOldPincode, HUINT8* szNewPincode);
HUINT8			xmgr_cas_NaPincodeCheckSCRemoved(void);

HERROR			xmgr_cas_NaGetCAPrjName(HUINT8* paucCaPrjName, HINT32 nLength);
HERROR			xmgr_cas_NaGetCANumber(HUINT8* paucCardSer);
HERROR			xmgr_cas_NaGetCardSerialNumber(HUINT8* paucCaSer);
HERROR			xmgr_cas_NaGetCAVersion(HUINT8* pszStr);
HERROR			xmgr_cas_NaGetSmartcardVersion(HUINT8* pszStr, HINT32 nLength);
HERROR			xmgr_cas_NaGetCurrentSCInfo(HUINT8* pszData, HINT32 nLength);
//HERROR			xmgr_cas_NaGetSystemScInfo(TSystemInfoStatus *xStatus, TIrdInfo **pxIrdInfo,
// 											TUnsignedInt8 *xNumberOfSmartcards, TSmartcardInfoItem **pxSmartcardInfoArray);
HERROR			xmgr_cas_NaGetCountrycode(HUINT8* pszCurLang1, HUINT8* pszCurLang2);
#if 1
HERROR			xmgr_cas_NaGetSysNUID(HUINT8* pszStr, HINT32 nLength);
#endif

//using menu
HERROR			xmgr_cas_NaUpdateSystemInfo(void);
HERROR	xmgr_cas_NaGetSystemInfo(XmgrCas_NaSystemInfoItem_e eItem, HUINT8* pszData, HINT32 nLength, HBOOL bCheckCardError);
HUINT8			xmgr_cas_NaCheckCardErrorOnMenu(void);

//callback
HERROR			xmgr_cas_NaRegisterSystemInfoCallback(void);
HERROR			xmgr_cas_NaRegisterSmartcardInfoCallback(void);

#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ALME)
HERROR			xmgr_cas_NaGetCscMaxIndex(TUnsignedInt8 *pxCscMaxIndex);
#endif
#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME)
HUINT8			xmgr_cas_NaUtil_GetNumberOfIcc(void);
#endif

// ird cmd mail
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
HERROR 			xmgr_cas_NaIrdCmdMailCheckUnreadMessage(void);
HERROR 			xmgr_cas_NaIrdCmdMailClearMailMessageHeader(void);
HERROR 			xmgr_cas_NaIrdCmdMailMakeMailMessage(HUINT8 *pIrdCmd, HUINT16	*pnMailId, HBOOL *pfCompleted, HUINT16 *pnDeletedMailId);
HERROR 			xmgr_cas_NaIrdCmdMailLoadMailMessages(void);
HERROR 			xmgr_cas_NaIrdCmdMailGetMailPriority(HUINT16 nMailId, NA_MAIL_PRIORITY *pnPriority);
HERROR 			xmgr_cas_NaIrdCmdMailGetMailList(NA_MAIL_MSG ***pppMailList, HUINT32 *pnCount);
HERROR 			xmgr_cas_NaIrdCmdMailGetMailData(HUINT16 nMailId, NA_MAIL_MSG **ppMailData);
HERROR 			xmgr_cas_NaIrdCmdMailDeleteMailMessage(HUINT16 nMailId);
HERROR 			xmgr_cas_NaIrdCmdMailUpdateReadFlag(HUINT16 nMailId, HBOOL bRead);
HERROR 			xmgr_cas_NaIrdCmdMailGetMailTitleAndText(HUINT8 *pszMailMessage, HUINT8 **ppszMailTitle, HUINT32 *pnTitleLength, HUINT8 **ppszMailText);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
HERROR			xmgr_cas_NaRegisterCardCreditCallback_Mediaset(void);
HERROR			xmgr_cas_NaRegisterTsCheckNotifyCallback_Mediaset(void);
HERROR			xmgr_cas_NaRegisterEventInfoNotifyCallback_Mediaset(void);
HERROR			xmgr_cas_NaRegisterPurchaseHistoryNotifyCallback_Mediaset(void);
HERROR			xmgr_cas_NaRegisterRechargeListNotifyCallback_Mediaset(void);
HERROR			xmgr_cas_NaRegisterOppvPurchaseListNotifyCallback_Mediaset(void);
HERROR			xmgr_cas_NaRegisterRechargeAlarmNotifyCallback_Mediaset(void);
HERROR			xmgr_cas_NaRegisterPackageListNotifyCallback_Mediaset(void);
HERROR			xmgr_cas_NaRegisterRequestPurchaseEventNotifyCallback_Mediaset(void);
HERROR			xmgr_cas_NaRegisterRequestPurchasePackageNotifyCallback_Mediaset(void);
HERROR			xmgr_cas_NaRequestEventPurchase_Mediaset(void);
HERROR			xmgr_cas_NaRequestPackagePurchase_Mediaset(HUINT32* pProduct);
HERROR			xmgr_cas_NaRequestPpvGetProductDescription_Mediaset(HUINT32* pStrLen, HUINT8** ppBuffer);
HERROR			xmgr_cas_NaRequestPpvGetProductDisclaimer_Mediaset(HUINT32* pStrLen, HUINT8** ppBuffer);
HBOOL 			xmgr_cas_NaMatchCardStatusWithOpType_Mediaset(HUINT8 eNaOpType);
HBOOL 			xmgr_cas_NaIsPopupReserved_Mediaset(void);
HERROR 			xmgr_cas_NaCheckPauseResume_Mediaset(HBOOL bPause);
HERROR 			xmgr_cas_NaRegisterDgtviSmartcardInfoCallback_Mediaset(void* callback);

HERROR			xmgr_cas_NaGetScExpireDateStr(HUINT8 *pszDate, HBOOL	 *bExpired);
HERROR			xmgr_cas_NaGetScCreditStr(HUINT8 *pszCredit);
HERROR			xmgr_cas_NaGetScCredit(HUINT32 *pCredit);
HERROR			xmgr_cas_NaGetCurrentDateStr(HUINT8 *pszDate);
HERROR 			xmgr_cas_NaDisposeCaRequest(HUINT32* pRequest);
HERROR			xmgr_cas_NaRequestTsCheck(Handle_t hSvcHandle);
HERROR			xmgr_cas_NaRequestEventInfo(void);
HERROR			xmgr_cas_NaGetPurchaseHistory(HUINT32 ulProviderId);
HERROR			xmgr_cas_NaGetRechargeList(void);
HERROR 			xmgr_cas_NaCakGetProviderId(HUINT16 *pProviderId);
HERROR 			xmgr_cas_NaCakGetFullPackageList(HUINT16 usProviderId);
HERROR			xmgr_cas_NaGetPackageList(HUINT32 ulProviderId);
HERROR			xmgr_cas_NaGetPackageEventInfo(HUINT32* pRequest, HUINT32* pPackage, void **ppPackageInfo);
HERROR			xmgr_cas_NaGetPurchaseHistoryNum(HUINT32* pNbItems);
HERROR			xmgr_cas_NaCheckProductPurchasable(HUINT32* pRequest, HUINT32*  pProduct);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_UPC)
BUS_Result_t	xproc_cas_NaCreate7DayTimer_Upc(HINT32 nBootUiType);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_RCS)
// ppv event
HERROR			xmgr_cas_NaPpvSetEpgMode(HBOOL xEpgMode);
HERROR  		xmgr_cas_NaPpvForceCreatEventInfo(Handle_t hSvcHandle);
HERROR 			xmgr_cas_NaPpvGetPpvList(TEventPurchaseListStatus *xStatus, HUINT8 *xNumberOfItems, TEventPurchaseListItem **pxEventPurchaseListArray);
HERROR  		xmgr_cas_NaPpvGetCredit(TCreditStatus *xStatus, HUINT8 *xNumberOfItems, TCreditItem **pxCreditArray);
HERROR  		xmgr_cas_NaPpvGetCreditSimple(TCreditAmount	*pCreditAmount, TCreditStatus *pxStatus);
HERROR  		xmgr_cas_NaPpvGetCreditString(HUINT8* pStr);
HERROR 			xmgr_cas_NaPpvIsPurchasedEvent(void  *pData, HUINT8 *pEvtName, HBOOL *pbPurchased);
HERROR 			xmgr_cas_NaPpvCheckPPVRCS(void *pData);
HERROR 			xmgr_cas_NaPpvGetPurchaseEvtInfo(Handle_t hSvcHandle, HUINT32 currTime, HINT32 *eventId);
HERROR 			xmgr_cas_NaRequestEventInfo(Handle_t hSvcHandle, HINT32 nEvtId);
HERROR 			xmgr_cas_NaPpvRequestEventInfoByEpg(Handle_t hSvcHandle,  HINT32 nEvtId, TCaRequest **pRequest, PPV_EVENT_INFO **pEventInfo, TCaProduct **pProduct);
HERROR  		xmgr_cas_NaPpvRequestPurchaseEvent(TPurchasableEventType xType, TCaRequest* pRequest, TCaProduct *pProduct);
HERROR  		xmgr_cas_NaPpvRequestPurchaseEventByEpg(TPurchasableEventType xType, TCaRequest* pRequest, TCaProduct *pProduct, TCaProcessingStatus *pProStatus);
// callback
HERROR  		xmgr_cas_NaPpvInitialize(void);
HERROR  		xmgr_cas_NaPpvRegisterCreditCallback(void);
HERROR  		xmgr_cas_NaPpvRegisterPpvListCallback(void);
HERROR  		xmgr_cas_NaPpvRegisterRechargeNotifyCallback(void);
HERROR  		xmgr_cas_NaPpvRegisterRequestPurchaseEventNotifyCallback(void);
HERROR  		xmgr_cas_NaPpvRegisterRequestEventNotifyCallback(void);
HERROR  		xmgr_cas_NaPpvRegisterPurchaseHistoryNotifyCallback(void);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_SPAIN_TDT)
HERROR			xmgr_cas_NaTdtEMMUpdate(void);
#endif

//command tool
void			xmgr_cas_NaCMD(HUINT8 *pszCmd, HINT32 ulParam1, HINT32 ulParam2, HUINT8 *pszmsg);

#if defined(CONFIG_MW_CAS_NAGRA_OP_TIVU)
HBOOL 			xmgr_cas_NaCheckTivuService(void);
HBOOL			xmgr_cas_NaCheckTivuCardSystemId(void);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
HERROR			xmgr_cas_NaGetPdExpirationDuration(TCaDuration *expireDuraion);
HERROR			xmgr_cas_NaGetZipCode(TUnsignedInt8 ucSlotId, TUnsignedInt32 *pulZipCode);
HERROR			xmgr_cas_NaIsHdPlusSmartCard(TUnsignedInt8 ucSlotId, TBoolean *pbHdPlusSc);
HERROR			xmgr_cas_NaIsHdPlusService(TUnsignedInt32 hAction, TBoolean *peStatus);
#endif
#if defined(CONFIG_OP_SES)
HERROR			xmgr_cas_NaIsSameCasIdInSc(TUnsignedInt8 ucSlotId,TUnsignedInt16 usCaSysId, TBoolean *pbSame);
#else
HERROR			xmgr_cas_NaIsSameCasIdInSc(TUnsignedInt32 ulIndex, TBoolean *pbSame);
#endif
HERROR			xmgr_cas_NaGetCurrScStatus(TUnsignedInt32 ulIndex, TSmartcardAlarm *peScAlarm);

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD)
HERROR			xmgr_cas_NaResetIRDCmdData(void);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
HERROR			xmgr_cas_NaIrdCmdEmmGetWakeupTime(UNIXTIME *pulTime);
HERROR			xmgr_cas_NaIrdCmdEmmInitWakingTable(void);
HERROR			xmgr_cas_NaIrdCmdEmmHasMoreNextWaking(TBoolean *pbNext);
HERROR			xmgr_cas_NaIrdCmdEmmGetWakingTuneInfo(TUnsignedInt8 *pucNetworkIdFlag, TUnsignedInt16 *pusNetworkId,
																			TUnsignedInt8 *pucTsIdFlag, TUnsignedInt16 *pusTsId,
																			TUnsignedInt8 *pucServiceIdFlag, TUnsignedInt16 *pusServiceId);
HERROR			xmgr_cas_NaIrdCmdEmmGetWakingTimer(UNIXTIME *pulTime);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_POPUP)
HERROR 			xmgr_cas_NaIrdCmdPopupDisplayPopupMessage(void);
#endif

HERROR			xmgr_cas_NaCheckPincode(TPinIndex xPinIndex, TPinCode xPinCode, TUnsignedInt8 *pucPinCode, TPinCodeStatus *xStatus);
HERROR			xmgr_cas_NaChangePincode(TPinIndex xPinIndex, TPinCode xOldPinCode, TPinCode xNewPinCode, TUnsignedInt8 *pucOldPinCode, TUnsignedInt8 *pucNewPinCode, TPinCodeStatus *xStatus);

void xmgr_cas_NaCmd_DisplayResetPinCodeMessage(void);

#if defined(CONFIG_OP_CAS_NAGRA_CAKTEST_PPT)
HERROR			xmgr_cas_NaCak7EnablingConsumptionForced(HBOOL bFlag);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_PRM)
HERROR xmgr_cas_NadvlSystemInformationRequest
(
	TDvlVersionString   xVersionString,
	TUnsignedInt32*    pxIrdSerialNumberValue,
	TDvlUniqueIdString  xIrdSerialNumberString,
	TUnsignedInt32*    pxNuidValue,
	TDvlUniqueIdString  xNuidString
);
HERROR xmgr_cas_NadvlChipsetInformationRequest(TDvlChipsetInfo**   ppxChipsetInformation);
HERROR xmgr_cas_NadvlReleaseBuffer(const TUnsignedInt8* pxBuffer);
HERROR xmgr_cas_NadvlCredentialsInformationRequest(TSize xCredentialsSize, const TUnsignedInt8* pxCredentials, TCredentialsType* pxCredentialsType, const TCredentialsUnion** ppxCredentialsUnion,
											       TSize* pxGenericMetadataSize, const TUnsignedInt8** ppxGenericMetadata, TSize* pxSpecificMetadataSize, const TUnsignedInt8** ppxSpecificMetadata);
HERROR xmgr_cas_NadvlStartPlayback(TTransportSessionId xTSid, TSize xInputCredentialsSize, const TUnsignedInt8* pxInputCredentials, TDvlPlaybackErrorCB xPlaybackErrorCB,
								   TSize* pxOutputCredentialsSize,const TUnsignedInt8** ppxOutputCredentials, TDvlAccessStatus* pxPlaybackStatus, TSize* pxGenericMetadataSize,
								   const TUnsignedInt8** ppxGenericMetadata, TSize* pxSpecificMetadataSize, const TUnsignedInt8** ppxSpecificMetadata, TDvlHandle** ppxPlaybackHandle);
HERROR xmgr_cas_NadvlStopPlayback(TDvlHandle* pxPlaybackHandle);

#if defined(CONFIG_MW_CAS_NAGRA_PRM_3_1)
HERROR xmgr_cas_NadvlPlaybackOpenSession(TTransportSessionId xTSid, TDvlHandle** ppxPlaybackHandle);
HERROR xmgr_cas_NadvlPlaybackSetEntitlements(TDvlHandle *pxPlaybackHandle,TDvlEntitlementsParameters* pxEntitlementsParameters,TDvlAccessStatus* pxPlaybackStatus,TSize* pxOutputCredentialsSize,
													const TUnsignedInt8** ppxOutputCredentials, TSize* pxKeyIdSize, const TUnsignedInt8** ppxKeyId);
HERROR xmgr_cas_NadvlPlaybackCloseSession(TDvlHandle* pxPlaybackHandle);
HERROR xmgr_cas_NadvlPostPrmManagementMessage(TSize xPrmManagementMessageSize, const TUnsignedInt8* pxPrmManagementMessage);
HERROR xmgr_cas_NadvlGetAuthenticatedMessage(TSize xCipheredMessageSize,const TUnsignedInt8* pxCipheredMessage,TSize *pxAuthenticatedDataSize,const TUnsignedInt8** ppxAuthenticatedData);
HERROR xmgr_cas_NadvlOpaqueDataMessageCreate(TOdmHandle** ppxOpaqueDataMessageHandle);
HERROR xmgr_cas_NadvlOpaqueDataMessageRelease(TOdmHandle* pxOpaqueDataMessageHandle);
HERROR xmgr_cas_NadvlOpaqueDataMessageSetApplicationData(TOdmHandle *pxOpaqueDataMessageHandle,TSize xApplicationDataSize,const TUnsignedInt8 *pxApplicationData);
HERROR xmgr_cas_NadvlOpaqueDataMessageSetPrmSignalization(TOdmHandle *pxOpaqueDataMessageHandle,TSize xPrmSignalizationSize,const TUnsignedInt8* pxPrmSignalization);
HERROR xmgr_cas_NadvlOpaqueDataMessageGet(TOdmHandle *pxOpaqueDataMessageHandle, TSize *pxOpaqueDataMessageSize, const TUnsignedInt8** ppxOpaqueDataMessage);
HERROR xmgr_cas_AddTransportSessionIdDirect(TTransportSessionId xTsId);
HERROR xmgr_cas_RemoveTransportSessionIdDirect(TTransportSessionId xTsId);
#endif
HERROR xmgr_cas_NadvlUpdateSpecificLcmMetadata(TSize  xInLcmCredentialsSize, const TUnsignedInt8 *  pxInLcmCredentials, TSize  xSpecificMetadataSize, const TUnsignedInt8 *  pxSpecificMetadata,
									TSize *  pxOutLcmCredentialsSize, const TUnsignedInt8 **  ppxOutLcmCredentials);
#if defined(CONFIG_MW_CAS_NAGRA_PRM_VOD_PVR)
HERROR xmgr_cas_NadvlCreateRecordSessionParameters(TDvlRecordSessionParameters** ppxRecordParameters);
HERROR xmgr_cas_NadvlStartRecordEx(TDvlRecordSessionParameters* pxRecordParameters, TSize* pxOutputCredentialsSize,const TUnsignedInt8** ppxOutputCredentials,
										TDvlRecordStatus* pxRecordingStatus, TDvlHandle** ppxRecordHandle);
HERROR xmgr_cas_NadvlStopRecord(TDvlHandle* pxRecordHandle);
HERROR xmgr_cas_NadvlStopPlayback(TDvlHandle* pxPlaybackHandle);
#endif
#endif	//#if defined(CONFIG_MW_CAS_NAGRA_PRM)

HCHAR	*xmgr_cas_NaUtil_EncodeString(HCHAR *string);
void	xmgr_cas_NaUtil_ConvertLanguageCode(HUINT8 *pszMessage);
HERROR xmgr_cas_NaUtil_PostMessageSessionEventToOM(DxCAS_MmiSessionType_e eSessionType, BUS_Callback_t ulUiHandle, Handle_t hSession, HUINT32 ulSlotNumber, HINT32 eEvent);

HERROR	xmgr_cas_NaUtil_FactoryDefault(void);

HUINT8 xmgr_cas_NaGetSCRemoved(void );
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
HERROR xmgr_cas_NaGetExpirationDate(HUINT32 *pUnixTime);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
HERROR	xmgr_cas_NaGetStandardErrorCode(TUnsignedInt32 ulSessionId, TUnsignedInt32* pStandardErrorCode);
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_ZONEID)
HERROR	xmgr_cas_NaGetZoneId(TUnsignedInt8* pZoneId, TUnsignedInt32 ulMaxLen);
HERROR	xmgr_cas_NaGetCountryCode(TUnsignedInt8* pCountryCode, TUnsignedInt32 ulMaxLen);
#endif

#endif /* !__xmgr_cas_NaUTIL_H__ */

