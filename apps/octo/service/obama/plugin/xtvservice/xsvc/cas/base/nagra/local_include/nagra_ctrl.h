/*
@file				nagra_ctrl.h
@brief				nagra_ctrl.h

Description:  \n
				\n
Module: MW/CAS \n
Remarks :\n

Copyright (c) 2011 HUMAX Co., Ltd. \n
All rights reserved. \n
*/

#ifndef NAGRA_CTRL_H
#define NAGRA_CTRL_H


/**************************************************************************************************/
#define ________Nagra_ctrl_Private_Type_Define_____________________________________________
/**************************************************************************************************/
typedef enum tagControlTypeNa
{
	eNaCtrl_RequestInfo = 0,
	eNaCtrl_RegisterSystemInfoCallback,
	eNaCtrl_RegisterLocInfoCallback,
	eNaCtrl_RegisterScAlarmCallback,
	eNaCtrl_GetSystemInfo,
	eNaCtrl_GetChipsetInfo,
#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ALME)
	eNaCtrl_GetCscMaxIndex,
#endif
#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME)
	eNaCtrl_GetNumberOfIcc,
#endif
#if defined(CONFIG_MW_CAS_NAGRA_PRM)
	eNaCtrl_GetDvlVersion,
	eNaCtrl_GetDvlChipsetType,
#endif
	eNaCtrl_GetLocInfo,
	eNaCtrl_CheckPincode,
	eNaCtrl_ChangePincode,

#if defined(CONFIG_OP_CAS_NAGRA_CAKTEST_PPT)
	eNaCtrl_Cak7_EnablingConsumptionForced,
#endif

#if 0
	eNaCtrl_GetProviderId,
	eNaCtrl_GetEcmCaSystemId,
#endif
	eNaCtrl_GetCurrScStatus,
	eNaCtrl_ReportCurrScStatus,
	eNaCtrl_ReportCurrAlarmData,

	eNaCtrl_GetNumberOfSlots,
	eNaCtrl_IsSameCasIdInSc,

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD)
	eNaCtrl_ResetIRDCmdData,

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
	eNaCtrl_IRDCMD_MAIL_CheckUnreadMessage,
	eNaCtrl_IRDCMD_MAIL_ClearMailMessageHeader,
	eNaCtrl_IRDCMD_MAIL_MakeMailMessage,
	eNaCtrl_IRDCMD_MAIL_LoadMailMessages,
	eNaCtrl_IRDCMD_MAIL_GetMailPriority,
	eNaCtrl_IRDCMD_MAIL_GetMailList,
	eNaCtrl_IRDCMD_MAIL_GetMailData,
	eNaCtrl_IRDCMD_MAIL_DeleteMailMessage,
	eNaCtrl_IRDCMD_MAIL_UpdateReadFlag,
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_POPUP)
	eNaCtrl_IRDCMD_POPUP_DisplayPopupMessage,
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
	eNaCtrl_IRDCMD_EMM_GetWakeupTime,
	eNaCtrl_IRDCMD_EMM_InitWakingTable,
	eNaCtrl_IRDCMD_EMM_HasMoreNextWaking,
	eNaCtrl_IRDCMD_EMM_GetWakingTuneInfo,
	eNaCtrl_IRDCMD_EMM_GetWakingTimer,
#endif
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	eNaCtrl_GetZipCode,
	eNaCtrl_GetScExpirationDate,
	eNaCtrl_IsHdPlusSmartCard,
	eNaCtrl_IsHdPlusService,
	eNaCtrl_GetPdExpirationDuration,
//	eNaCtrl_GetSizeOfNaspCaDesc,
//	eNaCtrl_GetContentOfNaspCaDesc,
	eNaCtrl_GetValidityHdProduct,
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
	eNaCtrl_RegisterScCreditCallback,
	eNaCtrl_RegisterProductsLoadingCallback,
	eNaCtrl_RegisterPurchaseHistoryCallback,
	eNaCtrl_RegisterRechargeListCallback,
	eNaCtrl_RegisterOppvPurchaseListCallback,
	eNaCtrl_RegisterRechargeAlarmCallback,
	eNaCtrl_RegisterEventInfoCallback,
	eNaCtrl_RegisterPackageListCallback,
	eNaCtrl_RegisterFullInfoPackageListNotifyCallback,
	eNaCtrl_RegisterRequestPurchaseEventCallback,
	eNaCtrl_RegisterRequestPurchasePackageCallback,
	eNaCtrl_RegisterDgtviScAlarmCallback,

	eNaCtrl_GetScExpireDateForPpv,
	eNaCtrl_GetCurrentDateStr,
	eNaCtrl_GetScCreditStr,
	eNaCtrl_GetScCredit,
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET) || defined(CONFIG_MW_CAS_NAGRA_OP_SPAIN_TDT)
	eNaCtrl_NotifyTPChanged,
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
	eNaCtrl_NotifySvcChanged,
	eNaCtrl_RequestPurchaseHistory,
	eNaCtrl_RequestRechargeList,
	eNaCtrl_CakGetProviderId,
	eNaCtrl_RequestFullInfoPackageList,
	eNaCtrl_RequestPackageList,
	eNaCtrl_RequestEventPurchase,
	eNaCtrl_RequestPackagePurchase,
	eNaCtrl_RequestProductPurchasable,
	eNaCtrl_GetProductGetDescription,
	eNaCtrl_GetProductGetDisclaimer,
	eNaCtrl_GetPackageEventInfo,
	eNaCtrl_GetPurchaseHistoryNum,
	eNaCtrl_DisposeRequest,
	eNaCtrl_TsCheck,
	eNaCtrl_RequestEventInfo,
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_SPAIN_TDT)
	eNaCtrl_GetPPVStateInfo,
	eNaCtrl_GetPackageList,
	eNaCtrl_GetPurchaseHistoryList,
	eNaCtrl_GetProviderName,
	eNaCtrl_ReleaseProductList,
	eNaCtrl_ReleasePurchaseHistoryList,
	eNaCtrl_ReleaseProviderName,
#endif

#if	defined(CONFIG_MW_CAS_NAGRA_OP_RCS)
	eNaCtrl_PPV_SetEpgMode,
	eNaCtrl_PPV_ForceCreatEventInfo,
	eNaCtrl_PPV_GetPpvList,
	eNaCtrl_PPV_GetCredit,
	eNaCtrl_PPV_GetPurchaseEvtInfo,
	eNaCtrl_PPV_RequestEventInfo,
	eNaCtrl_PPV_RequestEventInfoByEpg,
	eNaCtrl_PPV_RequestPurchaseEvent,
	eNaCtrl_PPV_RequestPurchaseEventByEpg,

	eNaCtrl_PPV_RegisterCreditCallback,
	eNaCtrl_PPV_RegisterPpvListCallback,
	eNaCtrl_PPV_RegisterRechargeNotifyCallback,
	eNaCtrl_PPV_RegisterRequestPurchaseEventNotifyCallback,
	eNaCtrl_PPV_RegisterRequestEventNotifyCallback,
	eNaCtrl_PPV_RegisterPurchaseHistoryNotifyCallback,
#endif

#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
	eNaCtrl_GetSlotIDbySessionID,
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_TIVU)
	eNaCtrl_CheckTivuService,
	eNaCtrl_SetTivuService,
	eNaCtrl_CheckTivuCardSystemId,
#endif

#if defined(CONFIG_MW_CAS_NAGRA_PRM)
	eNaCtrl_dvlSystemInformationRequest,
	eNaCtrl_dvlChipsetInformationRequest,
	eNaCtrl_dvlReleaseBuffer,
	eNaCtrl_dvlCredentialsInformationRequest,
	eNaCtrl_dvlStartPlayback,
	eNaCtrl_dvlStopPlayback,
#if defined(CONFIG_MW_CAS_NAGRA_PRM_3_1)
	eNaCtrl_dvlPlaybackOpenSession,
	eNaCtrl_dvlPlaybackSetEntitlements,
	eNaCtrl_dvlPlaybackCloseSession,
	eNaCtrl_dvlPostPrmManagementMessage,
	eNaCtrl_dvlGetAuthenticatedMessage,
	eNaCtrl_dvlOpaqueDataMessageCreate,
	eNaCtrl_dvlOpaqueDataMessageRelease,
	eNaCtrl_dvlOpaqueDataMessageSetApplicationData,
	eNaCtrl_dvlOpaqueDataMessageSetPrmSignalization,
	eNaCtrl_dvlOpaqueDataMessageGet,
	eNaCtrl_AddTransportSessionIdDirect,
	eNaCtrl_RemoveTransportSessionIdDirect,
#endif
	eNaCtrl_dvlUpdateSpecificLcmMetadata,
#if defined(CONFIG_MW_CAS_NAGRA_PRM_VOD_PVR)
	eNaCtrl_dvlCreateRecordSessionParameters,
	eNaCtrl_dvlStartRecordEx,
	eNaCtrl_dvlStopRecord,
#endif
#endif

#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
	eNaCtrl_SetParentalControl,
	eNaCtrl_SetParentalControlLevel,
#endif

	eNaCtrl_FactoryDefault,
#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
	eNaCtrl_GetStandardErrorCode,
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_ZONEID)
	eNaCtrl_GetZoneId,
	eNaCtrl_GetCountryCode,
#endif
	eNaCtrl_MAX,
} ControlTypeNa_t;

#if 0
typedef struct tagXXXParam
{
	XXXInParam_t	*pstInParam;
	XXXOutParam_t	*pstOutParam;
} XXXParam_t;
#endif

// MW_CA_NA_RequestInfo
typedef struct tagNA_RequestInfoInParam_t
{
	TUnsignedInt32 ulReqId;
} NA_RequestInfoInParam_t;
#if 0
typedef struct tagNA_RequestInfoOutParam_t
{
} NA_RequestInfoOutParam_t;
#endif

// MW_CA_NA_RegisterSystemInfoCallback
typedef struct tagNA_RegisterSystemInfoCallbackInParam_t
{
	TSystemInfoCallback callback;
} NA_RegisterSystemInfoCallbackInParam_t;
#if 0
typedef struct tagNA_RegisterSystemInfoCallbackOutParam_t
{
} NA_RegisterSystemInfoCallbackOutParam_t;
#endif

// MW_CA_NA_RegisterLocInfoCallback
typedef struct tagNA_RegisterLocInfoCallbackInParam_t
{
	TLocationInfoCallback		callback;
} NA_RegisterLocInfoCallbackInParam_t;
#if 0
typedef struct tagNA_RegisterLocInfoCallbackOutParam_t
{
} NA_RegisterLocInfoCallbackOutParam_t;
#endif

// MW_CA_NA_RegisterScAlarmCallback
typedef struct tagNA_RegisterScAlarmCallbackInParam_t
{
	TSmartcardAlarmCallback callback;
} NA_RegisterScAlarmCallbackInParam_t;
#if 0
typedef struct tagNA_RegisterScAlarmCallbackOutParam_t
{
} NA_RegisterScAlarmCallbackOutParam_t;
#endif

// MW_CA_NA_GetSystemInfo
typedef struct tagNA_GetSystemInfoInParam_t
{
} NA_GetSystemInfoInParam_t;

typedef struct tagNA_GetSystemInfoOutParam_t
{
	TSystemInfoStatus			xStatus;
	TIrdInfo					*pxIrdInfo;
	TUnsignedInt8				xNumberOfSmartcards;
	TSmartcardInfoItem		*pxSmartcardInfoArray;
	NaChipsetInfo_t				*pstChipInfo;
#if defined(CONFIG_MW_CAS_NAGRA_PRM)
	TDvlVersionString			*pxDvlVersion;
	TDvlChipsetTypeString 		*pxDvlChipsetType;
#endif
} NA_GetSystemInfoOutParam_t;

// MW_CA_NA_GetLocInfo
#if 0
typedef struct tagNA_GetLocInfoInParam_t
{
} NA_GetLocInfoInParam_t;
#endif
typedef struct tagNA_GetLocInfoOutParam_t
{
	TLocationInfoStatus		 xStatus;
	TUnsignedInt8			 xNumberOfSmartcards;
	TLocationInfoItem		*pxLocationInfoArray;
} NA_GetLocInfoOutParam_t;

// MW_CA_NA_CheckPincode
typedef struct tagNA_CheckPincodeInParam_t
{
	TPinIndex			xPinIndex;
	TPinCode			xPinCode;
	TUnsignedInt8		*pucPinCode;
} NA_CheckPincodeInParam_t;

typedef struct tagNA_CheckPincodeOutParam_t
{
	TPinCodeStatus		xStatus;
} NA_CheckPincodeOutParam_t;

// MW_CA_NA_ChangePincode
typedef struct tagNA_ChangePincodeInParam_t
{
	TPinIndex				xPinIndex;
	TPinCode				xOldPinCode;
	TPinCode				xNewPinCode;
	TUnsignedInt8			*pucOldPinCode;
	TUnsignedInt8			*pucNewPinCode;
} NA_ChangePincodeInParam_t;
typedef struct tagNA_ChangePincodeOutParam_t
{
	TPinCodeStatus			xStatus;
} NA_ChangePincodeOutParam_t;

#if defined(CONFIG_OP_CAS_NAGRA_CAKTEST_PPT)
// MW_CA_NA_Cak7_EnablingConsumptionForced
typedef struct tagNA_Cak7_EnablingConsumptionForcedInParam_t
{
	TBoolean	bOneShot;
} NA_Cak7_EnablingConsumptionForcedInParam_t;
#if 0
typedef struct tagNA_Cak7_EnablingConsumptionForcedOutParam_t
{
} NA_Cak7_EnablingConsumptionForcedOutParam_t;
#endif
#endif

#if 0
// MW_CA_NA_GetProviderId
typedef struct tagNA_GetProviderIdInParam_t
{
	TUnsignedInt8	ucSlotId;
} NA_GetProviderIdInParam_t;
typedef struct tagNA_GetProviderIdOutParam_t
{
	TPpid			usProviderId;
} NA_GetProviderIdOutParam_t;

// MW_CA_NA_GetEcmCaSystemId
typedef struct tagNA_GetEcmCaSystemIdInParam_t
{
	TUnsignedInt8			ucSlotId;
} NA_GetEcmCaSystemIdInParam_t;
typedef struct tagNA_GetEcmCaSystemIdOutParam_t
{
	TUnsignedInt16			usCaSysId;
} NA_GetEcmCaSystemIdOutParam_t;
#endif

// MW_CA_NA_GetCurrScStatus
typedef struct tagNA_GetCurrScStatusInParam_t
{
	TUnsignedInt32 ulSlotId;
} NA_GetCurrScStatusInParam_t;
typedef struct tagNA_GetCurrScStatusOutParam_t
{
	TSmartcardState 	eScAlarm;
} NA_GetCurrScStatusOutParam_t;

// MW_CA_NA_ReportCurrScStatus
typedef struct tagNA_ReportCurrScStatusInParam_t
{
	TUnsignedInt32		hAction;
} NA_ReportCurrScStatusInParam_t;
#if 0
typedef struct tagNA_ReportCurrScStatusOutParam_t
{
} NA_ReportCurrScStatusOutParam_t;
#endif

// MW_CA_NA_ReportCurrAlarmData
#if 0
typedef struct tagNA_ReportCurrAlarmDataInParam_t
{
} NA_ReportCurrAlarmDataInParam_t;
typedef struct tagNA_ReportCurrAlarmDataOutParam_t
{
} NA_ReportCurrAlarmDataOutParam_t;
#endif

// MW_CA_NA_GetNumberOfSlots
#if 0
typedef struct tagNA_GetNumberOfSlotsInParam_t
{
} NA_GetNumberOfSlotsInParam_t;
#endif
typedef struct tagNA_GetNumberOfSlotsOutParam_t
{
	TUnsignedInt8			ucSlots;
} NA_GetNumberOfSlotsOutParam_t;

// MW_CA_NA_IsSameCasIdInSc
typedef struct tagNA_IsSameCasIdInScInParam_t
{
	TUnsignedInt8		ucSlotId;
	TUnsignedInt16		usCaSysId;
} NA_IsSameCasIdInScInParam_t;
typedef struct tagNA_IsSameCasIdInScOutParam_t
{
	TBoolean			bSame;
} NA_IsSameCasIdInScOutParam_t;

// MW_CA_NA_ResetIRDCmdData
#if 0
typedef struct tagNA_ResetIRDCmdDataInParam_t
{
} NA_ResetIRDCmdDataInParam_t;
typedef struct tagNA_ResetIRDCmdDataOutParam_t
{
} NA_ResetIRDCmdDataOutParam_t;
#endif

// for ird cmd mail
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
// MW_CA_NA_IRDCMD_MAIL_CheckUnreadMessage
#if 0
typedef struct tagNA_IRDCMD_MAIL_CheckUnreadMessageInParam_t
{
} NA_IRDCMD_MAIL_CheckUnreadMessageInParam_t;
#endif
typedef struct tagNA_IRDCMD_MAIL_CheckUnreadMessageOutParam_t
{
	TBoolean bUnread;
} NA_IRDCMD_MAIL_CheckUnreadMessageOutParam_t;


// MW_CA_NA_IRDCMD_MAIL_ClearMailMessageHeader
#if 0
typedef struct tagNA_IRDCMD_MAIL_ClearMailMessageHeaderInParam_t
{
} NA_IRDCMD_MAIL_ClearMailMessageHeaderInParam_t;
typedef struct tagNA_IRDCMD_MAIL_ClearMailMessageHeaderOutParam_t
{
} NA_IRDCMD_MAIL_ClearMailMessageHeaderOutParam_t;
#endif

// MW_CA_NA_IRDCMD_MAIL_MakeMailMessage
typedef struct tagNA_IRDCMD_MAIL_MakeMailMessageInParam_t
{
	TUnsignedInt8		*pIrdCmd;
} NA_IRDCMD_MAIL_MakeMailMessageInParam_t;
typedef struct tagNA_IRDCMD_MAIL_MakeMailMessageOutParam_t
{
	TUnsignedInt16	nMailId;
	TBoolean			fCompleted;
	TUnsignedInt16	nDeletedMailId;
} NA_IRDCMD_MAIL_MakeMailMessageOutParam_t;

// MW_CA_NA_IRDCMD_MAIL_LoadMailMessages
#if 0
typedef struct tagNA_IRDCMD_MAIL_LoadMailMessagesInParam_t
{
} NA_IRDCMD_MAIL_LoadMailMessagesInParam_t;
typedef struct tagNA_IRDCMD_MAIL_LoadMailMessagesOutParam_t
{
} NA_IRDCMD_MAIL_LoadMailMessagesOutParam_t;
#endif

// MW_CA_NA_IRDCMD_MAIL_GetMailPriority
typedef struct tagNA_IRDCMD_MAIL_GetMailPriorityInParam_t
{
	TUnsignedInt16		nMailId;
} NA_IRDCMD_MAIL_GetMailPriorityInParam_t;
typedef struct tagNA_IRDCMD_MAIL_GetMailPriorityOutParam_t
{
	NA_MAIL_PRIORITY	nPriority;
} NA_IRDCMD_MAIL_GetMailPriorityOutParam_t;

#if 0
// MW_CA_NA_IRDCMD_MAIL_GetMailList
typedef struct tagNA_IRDCMD_MAIL_GetMailListInParam_t
{
} NA_IRDCMD_MAIL_GetMailListInParam_t;
#endif
typedef struct tagNA_IRDCMD_MAIL_GetMailListOutParam_t
{
	NA_MAIL_MSG			***pppMailList;
	TUnsignedInt32		nCount;
} NA_IRDCMD_MAIL_GetMailListOutParam_t;

// MW_CA_NA_IRDCMD_MAIL_GetMailData
typedef struct tagNA_IRDCMD_MAIL_GetMailDataInParam_t
{
	TUnsignedInt16		nMailId;
} NA_IRDCMD_MAIL_GetMailDataInParam_t;
typedef struct tagNA_IRDCMD_MAIL_GetMailDataOutParam_t
{
	NA_MAIL_MSG			**ppMailData;
} NA_IRDCMD_MAIL_GetMailDataOutParam_t;

// MW_CA_NA_IRDCMD_MAIL_DeleteMailMessage
typedef struct tagNA_IRDCMD_MAIL_DeleteMailMessageInParam_t
{
	TUnsignedInt16		nMailId;
} NA_IRDCMD_MAIL_DeleteMailMessageInParam_t;
#if 0
typedef struct tagNA_IRDCMD_MAIL_DeleteMailMessageOutParam_t
{
} NA_IRDCMD_MAIL_DeleteMailMessageOutParam_t;
#endif

// MW_CA_NA_IRDCMD_MAIL_UpdateReadFlag
typedef struct tagNA_IRDCMD_MAIL_UpdateReadFlagInParam_t
{
	TUnsignedInt16		nMailId;
	TBoolean			bRead;
} NA_IRDCMD_MAIL_UpdateReadFlagInParam_t;
#if 0
typedef struct tagNA_IRDCMD_MAIL_UpdateReadFlagOutParam_t
{
} NA_IRDCMD_MAIL_UpdateReadFlagOutParam_t;
#endif
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_POPUP)
// for ird cmd popup msg
// MW_CA_NA_IRDCMD_POPUP_DisplayPopupMessage
#if 0
typedef struct tagNA_IRDCMD_POPUP_DisplayPopupMessageInParam_t
{
} NA_IRDCMD_POPUP_DisplayPopupMessageInParam_t;
typedef struct tagNA_IRDCMD_POPUP_DisplayPopupMessageOutParam_t
{
} NA_IRDCMD_POPUP_DisplayPopupMessageOutParam_t;
#endif
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
// for ird cmd emm wake-up
// MW_CA_NA_IRDCMD_EMM_GetWakeupTime
#if 0
typedef struct tagNA_IRDCMD_EMM_GetWakeupTimeInParam_t
{
} NA_IRDCMD_EMM_GetWakeupTimeInParam_t;
#endif
typedef struct tagNA_IRDCMD_EMM_GetWakeupTimeOutParam_t
{
	UNIXTIME		ulTime;
} NA_IRDCMD_EMM_GetWakeupTimeOutParam_t;

// MW_CA_NA_IRDCMD_EMM_InitWakingTable
#if 0
typedef struct tagNA_IRDCMD_EMM_InitWakingTableInParam_t
{
} NA_IRDCMD_EMM_InitWakingTableInParam_t;
typedef struct tagNA_IRDCMD_EMM_InitWakingTableOutParam_t
{
} NA_IRDCMD_EMM_InitWakingTableOutParam_t;
#endif

// MW_CA_NA_IRDCMD_EMM_HasMoreNextWaking
#if 0
typedef struct tagNA_IRDCMD_EMM_HasMoreNextWakingInParam_t
{
} NA_IRDCMD_EMM_HasMoreNextWakingInParam_t;
#endif
typedef struct tagNA_IRDCMD_EMM_HasMoreNextWakingOutParam_t
{
	TBoolean		bNext;
} NA_IRDCMD_EMM_HasMoreNextWakingOutParam_t;

// MW_CA_NA_IRDCMD_EMM_GetWakingTuneInfo
#if 0
typedef struct tagNA_IRDCMD_EMM_GetWakingTuneInfoInParam_t
{
} NA_IRDCMD_EMM_GetWakingTuneInfoInParam_t;
#endif
typedef struct tagNA_IRDCMD_EMM_GetWakingTuneInfoOutParam_t
{
	TUnsignedInt8		ucNetworkIdFlag;
	TUnsignedInt16		usNetworkId;
	TUnsignedInt8		ucTsIdFlag;
	TUnsignedInt16		usTsId;
	TUnsignedInt8		ucServiceIdFlag;
	TUnsignedInt16		usServiceId;
} NA_IRDCMD_EMM_GetWakingTuneInfoOutParam_t;

// MW_CA_NA_IRDCMD_EMM_GetWakingTimer
#if 0
typedef struct tagNA_IRDCMD_EMM_GetWakingTimerInParam_t
{
} NA_IRDCMD_EMM_GetWakingTimerInParam_t;
#endif
typedef struct tagNA_IRDCMD_EMM_GetWakingTimerOutParam_t
{
	UNIXTIME		ulTime;
} NA_IRDCMD_EMM_GetWakingTimerOutParam_t;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
// for aps spec
// MW_CA_NA_GetZipCode
typedef struct tagNA_GetZipCodeInParam_t
{
	TUnsignedInt8		ucSlotId;
} NA_GetZipCodeInParam_t;
typedef struct tagNA_GetZipCodeOutParam_t
{
	TUnsignedInt32 		ulZipCode;
} NA_GetZipCodeOutParam_t;

// MW_CA_NA_GetScExpirationDate
typedef struct tagNA_GetScExpirationDateInParam_t
{
	TUnsignedInt8		ucSlotId;
} NA_GetScExpirationDateInParam_t;
typedef struct tagNA_GetScExpirationDateOutParam_t
{
	TBoolean			bEndOfTime;
	TCalendarTime		stDataTime;
} NA_GetScExpirationDateOutParam_t;

// MW_CA_NA_IsHdPlusSmartCard
typedef struct tagNA_IsHdPlusSmartCardInParam_t
{
	TUnsignedInt8		ucSlotNo;
} NA_IsHdPlusSmartCardInParam_t;
typedef struct tagNA_IsHdPlusSmartCardOutParam_t
{
	TBoolean			bHdPlusSc;
} NA_IsHdPlusSmartCardOutParam_t;

// MW_CA_NA_IsHdPlusService
typedef struct tagNA_IsHdPlusServiceInParam_t
{
	TUnsignedInt32		hAction;
} NA_IsHdPlusServiceInParam_t;
typedef struct tagNA_IsHdPlusServiceOutParam_t
{
	TBoolean				bStatus;
} NA_IsHdPlusServiceOutParam_t;

// MW_CA_NA_GetPdExpirationDuration
typedef struct tagNA_GetPdExpirationDurationOutParam_t
{
	TCaDuration 		expireDuraion;
} NA_GetPdExpirationDurationOutParam_t;

// MW_CA_NA_GetSizeOfNaspCaDesc
typedef struct tagNA_GetSizeOfNaspCaDescInParam_t
{
	TUnsignedInt32		hAction;
} NA_GetSizeOfNaspCaDescInParam_t;
typedef struct tagNA_GetSizeOfNaspCaDescOutParam_t
{
	TUnsignedInt8		ucLen;
} NA_GetSizeOfNaspCaDescOutParam_t;

// MW_CA_NA_GetContentOfNaspCaDesc
typedef struct tagNA_GetContentOfNaspCaDescInParam_t
{
	TUnsignedInt32		hAction;
} NA_GetContentOfNaspCaDescInParam_t;
typedef struct tagNA_GetContentOfNaspCaDescOutParam_t
{
	TUnsignedInt8		ucDesc;
} NA_GetContentOfNaspCaDescOutParam_t;

// MW_CA_NA_GetValidityHdProduct
typedef struct tagNA_GetValidityHdProductInParam_t
{
	TRightType		eType;
	TUnsignedInt32	ucDscLen;
	TUnsignedInt8		*pucDesc;
} NA_GetValidityHdProductInParam_t;
typedef struct tagNA_GetValidityHdProductOutParam_t
{
	TBoolean			bExpired;
	TCaDuration			ulDuration;
} NA_GetValidityHdProductOutParam_t;
#endif

// MW_CA_NA_RegisterScCreditCallback
#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
typedef struct tagNA_RegisterScCreditCallbackInParam_t
{
	TCardCreditCallback	callback;
} NA_RegisterScCreditCallbackInParam_t;
#if 0
typedef struct tagNA_RegisterScCreditCallbackOutParam_t
{
} NA_RegisterScCreditCallbackOutParam_t;
#endif

// MW_CA_NA_RegisterProductsLoadingCallback
typedef struct tagNA_RegisterProductsLoadingCallbackInParam_t
{
	TCaListenerArrivedNotifyCallback callback;
} NA_RegisterProductsLoadingCallbackInParam_t;
#if 0
typedef struct tagNA_RegisterProductsLoadingCallbackOutParam_t
{
} NA_RegisterProductsLoadingCallbackOutParam_t;
#endif

// MW_CA_NA_RegisterPurchaseHistoryCallback
typedef struct tagNA_RegisterPurchaseHistoryCallbackInParam_t
{
	TPurchaseHistoryNotifyCallback callback;
} NA_RegisterPurchaseHistoryCallbackInParam_t;
#if 0
typedef struct tagNA_RegisterPurchaseHistoryCallbackOutParam_t
{
} NA_RegisterPurchaseHistoryCallbackOutParam_t;
#endif

// MW_CA_NA_RegisterRechargeListCallback
typedef struct tagNA_RegisterRechargeListCallbackInParam_t
{
	TRechargeListNotifyCallback		callback;
} NA_RegisterRechargeListCallbackInParam_t;
#if 0
typedef struct tagNA_RegisterRechargeListCallbackOutParam_t
{
} NA_RegisterRechargeListCallbackOutParam_t;
#endif

// MW_CA_NA_RegisterOppvPurchaseListCallback
typedef struct tagNA_RegisterOppvPurchaseListCallbackInParam_t
{
	TOPPVPurchaseListNotifyCallback callback;
} NA_RegisterOppvPurchaseListCallbackInParam_t;
#if 0
typedef struct tagNA_RegisterOppvPurchaseListCallbackOutParam_t
{
} NA_RegisterOppvPurchaseListCallbackOutParam_t;
#endif

// MW_CA_NA_RegisterRechargeAlarmCallback
typedef struct tagNA_RegisterRechargeAlarmCallbackInParam_t
{
	TRechargeNotifyCallback callback;
} NA_RegisterRechargeAlarmCallbackInParam_t;
#if 0
typedef struct tagNA_RegisterRechargeAlarmCallbackOutParam_t
{
} NA_RegisterRechargeAlarmCallbackOutParam_t;
#endif

// MW_CA_NA_RegisterEventInfoCallback
typedef struct tagNA_RegisterEventInfoCallbackInParam_t
{
	TRequestEventNotifyCallback callback;
} NA_RegisterEventInfoCallbackInParam_t;
#if 0
typedef struct tagNA_RegisterEventInfoCallbackOutParam_t
{
} NA_RegisterEventInfoCallbackOutParam_t;
#endif

// MW_CA_NA_RegisterPackageListCallback
typedef struct tagNA_RegisterPackageListCallbackInParam_t
{
	TPackageListNotifyCallback callback;
} NA_RegisterPackageListCallbackInParam_t;
#if 0
typedef struct tagNA_RegisterPackageListCallbackOutParam_t
{
} NA_RegisterPackageListCallbackOutParam_t;
#endif

// MW_CA_NA_RegisterFullInfoPackageListNotifyCallback
typedef struct tagNA_RegisterFullInfoPackageListNotifyCallbackInParam_t
{
	TPackageListNotifyCallback callback;
} NA_RegisterFullInfoPackageListNotifyCallbackInParam_t;
#if 0
typedef struct tagNA_RegisterFullInfoPackageListNotifyCallbackOutParam_t
{
} NA_RegisterFullInfoPackageListNotifyCallbackOutParam_t;
#endif

// MW_CA_NA_RegisterRequestPurchaseEventCallback
typedef struct tagNA_RegisterRequestPurchaseEventCallbackInParam_t
{
	TPurchaseEventNotifyCallback callback;
} NA_RegisterRequestPurchaseEventCallbackInParam_t;
#if 0
typedef struct tagNA_RegisterRequestPurchaseEventCallbackOutParam_t
{
} NA_RegisterRequestPurchaseEventCallbackOutParam_t;
#endif

// MW_CA_NA_RegisterRequestPurchasePackageCallback
typedef struct tagNA_RegisterRequestPurchasePackageCallbackInParam_t
{
	TPurchasePackageNotifyCallback callback;
} NA_RegisterRequestPurchasePackageCallbackInParam_t;
#if 0
typedef struct tagNA_RegisterRequestPurchasePackageCallbackOutParam_t
{
} NA_RegisterRequestPurchasePackageCallbackOutParam_t;
#endif

// MW_CA_NA_RegisterDgtviScAlarmCallback
typedef struct tagNA_RegisterDgtviScAlarmCallbackInParam_t
{
	TSmartcardAlarmCallback callback;
} NA_RegisterDgtviScAlarmCallbackInParam_t;
#if 0
typedef struct tagNA_RegisterDgtviScAlarmCallbackOutParam_t
{
} NA_RegisterDgtviScAlarmCallbackOutParam_t;
#endif

// MW_CA_NA_GetScExpireDateForPpv
#if 0
typedef struct tagNA_GetScExpireDateForPpvInParam_t
{
} NA_GetScExpireDateForPpvInParam_t;
#endif
typedef struct tagNA_GetScExpireDateForPpvOutParam_t
{
	TUnsignedInt8		*pszDate;
	TBoolean			bExpired;
} NA_GetScExpireDateForPpvOutParam_t;

// MW_CA_NA_GetCurrentDateStr
#if 0
typedef struct tagNA_GetCurrentDateStrInParam_t
{
} NA_GetCurrentDateStrInParam_t;
#endif
typedef struct tagNA_GetCurrentDateStrOutParam_t
{
	TUnsignedInt8 *pszDate;
} NA_GetCurrentDateStrOutParam_t;

// MW_CA_NA_GetScCreditStr
#if 0
typedef struct tagNA_GetScCreditStrInParam_t
{
} NA_GetScCreditStrInParam_t;
#endif
typedef struct tagNA_GetScCreditStrOutParam_t
{
	TUnsignedInt8		*pszCredit;
} NA_GetScCreditStrOutParam_t;

// MW_CA_NA_GetScCredit
#if 0
typedef struct tagNA_GetScCreditInParam_t
{
} NA_GetScCreditInParam_t;
#endif
typedef struct tagNA_GetScCreditOutParam_t
{
	TCreditAmount		ulCredit;
} NA_GetScCreditOutParam_t;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET) || defined(CONFIG_MW_CAS_NAGRA_OP_SPAIN_TDT)
// MW_CA_NA_NotifyTPChanged
typedef struct tagNA_NotifyTPChangedInParam_t
{
	TBoolean	bTpChanged;
} NA_NotifyTPChangedInParam_t;
#if 0
typedef struct tagNA_NotifyTPChangedOutParam_t
{
} NA_NotifyTPChangedOutParam_t;
#endif
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
// MW_CA_NA_NotifySvcChanged
typedef struct tagNA_NotifySvcChangedInParam_t
{
	Handle_t	hSvc;
	Handle_t	hAction;
} NA_NotifySvcChangedInParam_t;
#if 0
typedef struct tagNA_NotifySvcChangedOutParam_t
{
} NA_NotifySvcChangedOutParam_t;
#endif

// MW_CA_NA_RequestPurchaseHistory
typedef struct tagNA_RequestPurchaseHistoryInParam_t
{
	TUnsignedInt32 ulProviderId;
} NA_RequestPurchaseHistoryInParam_t;
#if 0
typedef struct tagNA_RequestPurchaseHistoryOutParam_t
{
} NA_RequestPurchaseHistoryOutParam_t;
#endif

// MW_CA_NA_RequestRechargeList
#if 0
typedef struct tagNA_RequestRechargeListInParam_t
{
} NA_RequestRechargeListInParam_t;
typedef struct tagNA_RequestRechargeListOutParam_t
{
} NA_RequestRechargeListOutParam_t;
#endif

// MW_CA_NA_CakGetProviderId
#if 0
typedef struct tagNA_CakGetProviderIdInParam_t
{
} NA_CakGetProviderIdInParam_t;
#endif
typedef struct tagNA_CakGetProviderIdOutParam_t
{
	TPpid		usProviderId;
} NA_CakGetProviderIdOutParam_t;

// MW_CA_NA_RequestFullInfoPackageList
typedef struct tagNA_RequestFullInfoPackageListInParam_t
{
	TPpid		providerId;
} NA_RequestFullInfoPackageListInParam_t;
#if 0
typedef struct tagNA_RequestFullInfoPackageListOutParam_t
{
} NA_RequestFullInfoPackageListOutParam_t;
#endif

// MW_CA_NA_RequestPackageList
typedef struct tagNA_RequestPackageListInParam_t
{
	TUnsignedInt32	ulProviderId;
} NA_RequestPackageListInParam_t;
#if 0
typedef struct tagNA_RequestPackageListOutParam_t
{
} NA_RequestPackageListOutParam_t;
#endif

// MW_CA_NA_RequestEventPurchase
typedef struct tagNA_RequestEventPurchaseInParam_t
{
	TPurchasableEventType	xStatus;
	TCaRequest				*pRequest;
	TCaEvent				*pEvent;
	TCaProduct				*pProduct;
} NA_RequestEventPurchaseInParam_t;
#if 0
typedef struct tagNA_RequestEventPurchaseOutParam_t
{
} NA_RequestEventPurchaseOutParam_t;
#endif

// MW_CA_NA_RequestPackagePurchase
typedef struct tagNA_RequestPackagePurchaseInParam_t
{
	TCaProduct* pProduct;
} NA_RequestPackagePurchaseInParam_t;
#if 0
typedef struct tagNA_RequestPackagePurchaseOutParam_t
{
} NA_RequestPackagePurchaseOutParam_t;
#endif

// MW_CA_NA_RequestProductPurchasable
typedef struct tagNA_RequestProductPurchasableInParam_t
{
	TCaRequest		*pRequest;
	TCaProduct		*pProduct;
} NA_RequestProductPurchasableInParam_t;
#if 0
typedef struct tagNA_RequestProductPurchasableOutParam_t
{
} NA_RequestProductPurchasableOutParam_t;
#endif

// MW_CA_NA_GetProductGetDescription
typedef struct tagNA_GetProductGetDescriptionInParam_t
{
	TCaProduct			*pxProduct;
} NA_GetProductGetDescriptionInParam_t;
typedef struct tagNA_GetProductGetDescriptionOutParam_t
{
	TSize				tStrLen;
	TUnsignedInt8		*pBuffer;
} NA_GetProductGetDescriptionOutParam_t;

// MW_CA_NA_GetProductGetDisclaimer
typedef struct tagNA_GetProductGetDisclaimerInParam_t
{
	TCaProduct			*pxProduct;
} NA_GetProductGetDisclaimerInParam_t;
typedef struct tagNA_GetProductGetDisclaimerOutParam_t
{
	TSize				tStrLen;
	TUnsignedInt8		*pBuffer;
} NA_GetProductGetDisclaimerOutParam_t;

// MW_CA_NA_GetPackageEventInfo
typedef struct tagNA_GetPackageEventInfoInParam_t
{
	TCaRequest			*pRequest;
	TCaProduct			*pPackage;
} NA_GetPackageEventInfoInParam_t;
typedef struct tagNA_GetPackageEventInfoOutParam_t
{
	PACKAGE_INFO		**pPackageInfo;
} NA_GetPackageEventInfoOutParam_t;

// MW_CA_NA_GetPurchaseHistoryNum
#if 0
typedef struct tagNA_GetPurchaseHistoryNumInParam_t
{
} NA_GetPurchaseHistoryNumInParam_t;
#endif
typedef struct tagNA_GetPurchaseHistoryNumOutParam_t
{
	TUnsignedInt32		ulItems;
} NA_GetPurchaseHistoryNumOutParam_t;

//MW_CA_NA_DisposeRequest
typedef struct tagNA_DisposeRequestInParam_t
{
	TUnsignedInt32* pRequest;
} NA_DisposeRequestInParam_t;
#if 0
typedef struct tagNA_DisposeRequestOutParam_t
{
} NA_DisposeRequestOutParam_t;
#endif

//MW_CA_NA_TsCheck
typedef struct tagNA_TsCheckInParam_t
{
	Handle_t hSvcHandle;
} NA_TsCheckInParam_t;
#if 0
typedef struct tagNA_TsCheckOutParam_t
{
} NA_TsCheckOutParam_t;
#endif

//MW_CA_NA_RequestEventInfo
#if 0
typedef struct tagNA_RequestEventInfoInParam_t
{
} NA_RequestEventInfoInParam_t;
typedef struct tagNA_RequestEventInfoOutParam_t
{
} NA_RequestEventInfoOutParam_t;
#endif

#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_SPAIN_TDT)
// MW_CA_NA_GetPPVStateInfo
#if 0
typedef struct tagNA_GetPPVStateInfoInParam_t
{
} NA_GetPPVStateInfoInParam_t;
#endif
typedef struct tagNA_GetPPVStateInfoOutParam_t
{
	NA_PPV_State_t	eState;
	HBOOL			bNagraCaSys;
} NA_GetPPVStateInfoOutParam_t;

// MW_CA_NA_GetPackageList
#if 0
typedef struct tagNA_GetPackageListInParam_t
{
} NA_GetPackageListInParam_t;
#endif
typedef struct tagNA_GetPackageListOutParam_t
{
	HUINT32						ulNumOfProducts;
	PPV_PACKAGE_LIST_ITEM		**ppPackageList;
} NA_GetPackageListOutParam_t;

// MW_CA_NA_GetPurchaseHistoryList
#if 0
typedef struct tagNA_GetPurchaseHistoryListInParam_t
{
} NA_GetPurchaseHistoryListInParam_t;
#endif
typedef struct tagNA_GetPurchaseHistoryListOutParam_t
{
	HUINT32						ulNumOfPurchase;
	PPV_PURCHASE_LIST_ITEM		**ppPurchaseList;
} NA_GetPurchaseHistoryListOutParam_t;

// MW_CA_NA_GetProviderName
#if 0
typedef struct tagNA_GetProviderNameInParam_t
{
} NA_GetProviderNameInParam_t;
#endif
typedef struct tagNA_GetProviderNameOutParam_t
{
	HUINT32		ulSize;
	HUINT8		*pucProviderName;
} NA_GetProviderNameOutParam_t;

// MW_CA_NA_ReleaseProductList
typedef struct tagNA_ReleaseProductListInParam_t
{
	PPV_PACKAGE_LIST_ITEM		*pPackageList;
} NA_ReleaseProductListInParam_t;
#if 0
typedef struct tagNA_ReleaseProductListOutParam_t
{
} NA_ReleaseProductListOutParam_t;
#endif

// MW_CA_NA_ReleasePurchaseHistoryList
typedef struct tagNA_ReleasePurchaseHistoryListInParam_t
{
	PPV_PURCHASE_LIST_ITEM		*pPurchaseList;
} NA_ReleasePurchaseHistoryListInParam_t;
#if 0
typedef struct tagNA_ReleasePurchaseHistoryListOutParam_t
{
} NA_ReleasePurchaseHistoryListOutParam_t;
#endif

// MW_CA_NA_ReleaseProviderName
typedef struct tagNA_ReleaseProviderNameInParam_t
{
	HUINT8	*pProviderName;
} NA_ReleaseProviderNameInParam_t;
#if 0
typedef struct tagNA_ReleaseProviderNameOutParam_t
{
} NA_ReleaseProviderNameOutParam_t;
#endif
#endif

#if	defined(CONFIG_MW_CAS_NAGRA_OP_RCS)
// MW_CA_NA_PPV_SetEpgMode
typedef struct tagNA_PPV_SetEpgModeInParam_t
{
	TBoolean		xEpgMode;
} NA_PPV_SetEpgModeInParam_t;
#if 0
typedef struct tagNA_PPV_SetEpgModeOutParam_t
{
} NA_PPV_SetEpgModeOutParam_t;
#endif

// MW_CA_NA_PPV_ForceCreatEventInfo
typedef struct tagNA_PPV_ForceCreatEventInfoInParam_t
{
	TUnsignedInt32 svcHandle;
} NA_PPV_ForceCreatEventInfoInParam_t;
#if 0
typedef struct tagNA_PPV_ForceCreatEventInfoOutParam_t
{
} NA_PPV_ForceCreatEventInfoOutParam_t;
#endif

// MW_CA_NA_PPV_GetPpvList
#if 0
typedef struct tagNA_PPV_GetPpvListInParam_t
{
} NA_PPV_GetPpvListInParam_t;
#endif
typedef struct tagNA_PPV_GetPpvListOutParam_t
{
	TEventPurchaseListStatus		xStatus;
	TUnsignedInt8					xNumberOfItems;
	TEventPurchaseListItem			**ppxEventPurchaseListArray;
} NA_PPV_GetPpvListOutParam_t;

// MW_CA_NA_PPV_GetCredit
#if 0
typedef struct tagNA_PPV_GetCreditInParam_t
{
} NA_PPV_GetCreditInParam_t;
#endif
typedef struct tagNA_PPV_GetCreditOutParam_t
{
	TCreditStatus		xStatus;
	TUnsignedInt8		xNumberOfItems;
	TCreditItem			**ppxCreditArray;
} NA_PPV_GetCreditOutParam_t;

// MW_CA_NA_PPV_GetPurchaseEvtInfo
typedef struct tagNA_PPV_GetPurchaseEvtInfoInParam_t
{
	TUnsignedInt32		svcHandle;
	TUnsignedInt32		currTime;
} NA_PPV_GetPurchaseEvtInfoInParam_t;
typedef struct tagNA_PPV_GetPurchaseEvtInfoOutParam_t
{
	TUnsignedInt16		nEventId;
} NA_PPV_GetPurchaseEvtInfoOutParam_t;

// MW_CA_NA_PPV_RequestEventInfo
typedef struct tagNA_PPV_RequestEventInfoInParam_t
{
	TUnsignedInt32		svcHandle;
	TUnsignedInt16		nEvtId;
} NA_PPV_RequestEventInfoInParam_t;
#if 0
typedef struct tagNA_PPV_RequestEventInfoOutParam_t
{
} NA_PPV_RequestEventInfoOutParam_t;
#endif

// MW_CA_NA_PPV_RequestEventInfoByEpg
typedef struct tagNA_PPV_RequestEventInfoByEpgInParam_t
{
	TUnsignedInt32		svcHandle;
	TUnsignedInt16		nEvtId;
} NA_PPV_RequestEventInfoByEpgInParam_t;
typedef struct tagNA_PPV_RequestEventInfoByEpgOutParam_t
{
	TUnsignedInt32		xStatus;
	TCaRequest			**pRequest;
	PPV_EVENT_INFO		**pEventInfo;
	TCaProduct			**pProduct;
} NA_PPV_RequestEventInfoByEpgOutParam_t;

// MW_CA_NA_PPV_RequestPurchaseEvent
typedef struct tagNA_PPV_RequestPurchaseEventInParam_t
{
	TPurchasableEventType	xType;
	TCaRequest				*pRequest;
	TCaProduct				*pProduct;
} NA_PPV_RequestPurchaseEventInParam_t;
#if 0
typedef struct tagNA_PPV_RequestPurchaseEventOutParam_t
{
} NA_PPV_RequestPurchaseEventOutParam_t;
#endif

// MW_CA_NA_PPV_RequestPurchaseEventByEpg
typedef struct tagNA_PPV_RequestPurchaseEventByEpgInParam_t
{
	TPurchasableEventType	xType;
	TCaRequest				*pRequest;
	TCaProduct				*pProduct;
} NA_PPV_RequestPurchaseEventByEpgInParam_t;
typedef struct tagNA_PPV_RequestPurchaseEventByEpgOutParam_t
{
	TCaProcessingStatus		eProStatus;
} NA_PPV_RequestPurchaseEventByEpgOutParam_t;


// MW_CA_NA_PPV_RegisterCreditCallback
typedef struct tagNA_PPV_RegisterCreditCallbackInParam_t
{
	TCreditCallback callback;
} NA_PPV_RegisterCreditCallbackInParam_t;
#if 0
typedef struct tagNA_PPV_RegisterCreditCallbackOutParam_t
{
} NA_PPV_RegisterCreditCallbackOutParam_t;
#endif

// MW_CA_NA_PPV_RegisterPpvListCallback
typedef struct tagNA_PPV_RegisterPpvListCallbackInParam_t
{
	TEventPurchaseListCallback callback;
} NA_PPV_RegisterPpvListCallbackInParam_t;
#if 0
typedef struct tagNA_PPV_RegisterPpvListCallbackOutParam_t
{
} NA_PPV_RegisterPpvListCallbackOutParam_t;
#endif

// MW_CA_NA_PPV_RegisterRechargeNotifyCallback
typedef struct tagNA_PPV_RegisterRechargeNotifyCallbackInParam_t
{
	TRechargeNotifyCallback callback;
} NA_PPV_RegisterRechargeNotifyCallbackInParam_t;
#if 0
typedef struct tagNA_PPV_RegisterRechargeNotifyCallbackOutParam_t
{
} NA_PPV_RegisterRechargeNotifyCallbackOutParam_t;
#endif

// MW_CA_NA_PPV_RegisterRequestPurchaseEventNotifyCallback
typedef struct tagNA_PPV_RegisterRequestPurchaseEventNotifyCallbackInParam_t
{
	TPurchaseEventNotifyCallback callback;
} NA_PPV_RegisterRequestPurchaseEventNotifyCallbackInParam_t;
#if 0
typedef struct tagNA_PPV_RegisterRequestPurchaseEventNotifyCallbackOutParam_t
{
} NA_PPV_RegisterRequestPurchaseEventNotifyCallbackOutParam_t;
#endif

// MW_CA_NA_PPV_RegisterRequestEventNotifyCallback
typedef struct tagNA_PPV_RegisterRequestEventNotifyCallbackInParam_t
{
	TRequestEventNotifyCallback	callback;
} NA_PPV_RegisterRequestEventNotifyCallbackInParam_t;
#if 0
typedef struct tagNA_PPV_RegisterRequestEventNotifyCallbackOutParam_t
{
} NA_PPV_RegisterRequestEventNotifyCallbackOutParam_t;
#endif

// MW_CA_NA_PPV_RegisterPurchaseHistoryNotifyCallback
typedef struct tagNA_PPV_RegisterPurchaseHistoryNotifyCallbackInParam_t
{
	TPurchaseHistoryNotifyCallback callback;
} NA_PPV_RegisterPurchaseHistoryNotifyCallbackInParam_t;
#if 0
typedef struct tagNA_PPV_RegisterPurchaseHistoryNotifyCallbackOutParam_t
{
} NA_PPV_RegisterPurchaseHistoryNotifyCallbackOutParam_t;
#endif
#endif

#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
// MW_CA_NA_GetSlotIDbySessionID
typedef struct tagNA_GetSlotIDbySessionIDInParam_t
{
	TIccSessionId		iccSessionId;
} NA_GetSlotIDbySessionIDInParam_t;
typedef struct tagNA_GetSlotIDbySessionIDOutParam_t
{
	TUnsignedInt32		ulSlotId;
} NA_GetSlotIDbySessionIDOutParam_t;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_TIVU)
// MW_CA_NA_CheckTivuService
#if 0
typedef struct tagNA_CheckTivuServiceInParam_t
{
} NA_CheckTivuServiceInParam_t;
#endif
typedef struct tagNA_CheckTivuServiceOutParam_t
{
	HBOOL	bStatus;
} NA_CheckTivuServiceOutParam_t;

// MW_CA_NA_SetTivuService
typedef struct tagNA_SetTivuServiceInParam_t
{
	HBOOL	bTivu;
} NA_SetTivuServiceInParam_t;
#if 0
typedef struct tagNA_SetTivuServiceOutParam_t
{
} NA_SetTivuServiceOutParam_t;
#endif

// MW_CA_NA_CheckTivuCardSystemId
#if 0
typedef struct tagNA_CheckTivuCardSystemIdInParam_t
{
} NA_CheckTivuCardSystemIdInParam_t;
#endif
typedef struct tagNA_CheckTivuCardSystemIdOutParam_t
{
	HBOOL	bStatus;
} NA_CheckTivuCardSystemIdOutParam_t;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_PRM)
#if 0
typedef struct tagNA_dvlSystemInformationRequestInParam_t
{

} NA_dvlSystemInformationRequestInParam_t;
#endif

typedef struct tagNA_dvlSystemInformationRequestOutParam_t
{
	TDvlVersionString	xVersionString;
	TUnsignedInt32		xIrdSerialNumberValue;
	TDvlUniqueIdString	xIrdSerialNumberString;
	TUnsignedInt32		xNuidValue;
	TDvlUniqueIdString	xNuidString;
} NA_dvlSystemInformationRequestOutParam_t;

#if 0
typedef struct tagNA_dvlChipsetInformationRequestInParam_t
{

} NA_dvlChipsetInformationRequestInParam_t;
#endif

typedef struct tagNA_dvlChipsetInformationRequestOutParam_t
{
	TDvlChipsetInfo**   ppxChipsetInformation;
} NA_dvlChipsetInformationRequestOutParam_t;

// MW_CA_NA_dvlReleaseBuffer
typedef struct tagNA_dvlReleaseBufferInParam_t
{
	const TUnsignedInt8*	pxBuffer;
} NA_dvlReleaseBufferInParam_t;

#if 0
typedef struct tagNA_dvlReleaseBufferOutParam_t
{
	const TUnsignedInt8*	pxBuffer;
} NA_dvlReleaseBufferOutParam_t;
#endif

// MW_CA_NA_dvlCredentialsInformationRequest
typedef struct tagNA_dvlCredentialsInformationRequestInParam_t
{
		  TSize 				xCredentialsSize;
	const TUnsignedInt8*		pxCredentials;
} NA_dvlCredentialsInformationRequestInParam_t;

typedef struct tagNA_dvlCredentialsInformationRequestOutParam_t
{
		  TCredentialsType*		pxCredentialsType;
	const TCredentialsUnion**	ppxCredentialsUnion;
		  TSize*				pxGenericMetadataSize;
	const TUnsignedInt8**		ppxGenericMetadata;
		  TSize*				pxSpecificMetadataSize;
	const TUnsignedInt8**		ppxSpecificMetadata;
} NA_dvlCredentialsInformationRequestOutParam_t;

typedef struct tagNA_dvlStartPlaybackInParam_t
{
		  TTransportSessionId   xTSid;
	 	  TSize                 xInputCredentialsSize;
	const TUnsignedInt8*       	pxInputCredentials;

} NA_dvlStartPlaybackInParam_t;

typedef struct tagNA_dvlStartPlaybackOutParam_t
{
		  TDvlPlaybackErrorCB   xPlaybackErrorCB;
		  TSize*               pxOutputCredentialsSize;
	const TUnsignedInt8**     ppxOutputCredentials;
	      TDvlAccessStatus*    pxPlaybackStatus;
		  TSize*               pxGenericMetadataSize;
	const TUnsignedInt8**     ppxGenericMetadata;
		  TSize*               pxSpecificMetadataSize;
	const TUnsignedInt8**     ppxSpecificMetadata;
		  TDvlHandle**        ppxPlaybackHandle;
} NA_dvlStartPlaybackOutParam_t;

typedef struct tagNA_dvlStopPlaybackInParam_t
{
	TDvlHandle*  pxPlaybackHandle;
} NA_dvlStopPlaybackInParam_t;

#if 0
typedef struct tagNA_dvlStopPlaybackOutParam_t
{

} NA_dvlStopPlaybackOutParam_t;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_PRM_3_1)

// MW_CA_NA_dvlPlaybackOpenSession
typedef struct tagNA_dvlPlaybackOpenSessionInParam_t
{
	TTransportSessionId   xTSid;
} NA_dvlPlaybackOpenSessionInParam_t;

typedef struct tagNA_dvlPlaybackOpenSessionOutParam_t
{
	TDvlHandle**		ppxPlaybackHandle;
} NA_dvlPlaybackOpenSessionOutParam_t;

// MW_CA_NA_dvlPlaybackSetEntitlements
typedef struct tagNA_dvlPlaybackSetEntitlementsInParam_t
{
	TDvlHandle* 				pxPlaybackHandle;
	TDvlEntitlementsParameters* pxEntitlementsParameters;
} NA_dvlPlaybackSetEntitlementsInParam_t;

typedef struct tagNA_dvlPlaybackSetEntitlementsOutParam_t
{
		  TDvlAccessStatus* 		pxPlaybackStatus;
		  TSize*					pxOutputCredentialsSize;
	const TUnsignedInt8**			ppxOutputCredentials;
		  TSize*					pxKeyIdSize;
	const TUnsignedInt8**			ppxKeyId;
} NA_dvlPlaybackSetEntitlementsOutParam_t;

// MW_CA_NA_dvlPlaybackCloseSession
typedef struct tagNA_dvlPlaybackCloseSessionInParam_t
{
	TDvlHandle*        pxPlaybackHandle;
} NA_dvlPlaybackCloseSessionInParam_t;

#if 0
typedef struct tagNA_dvlPlaybackCloseSessionOutParam_t
{
	TDvlHandle*        pxPlaybackHandle;
} NA_dvlPlaybackCloseSessionOutParam_t;
#endif

// MW_CA_NA_dvlPostPrmManagementMessage
typedef struct tagNA_dvlPostPrmManagementMessageInParam_t
{
		  TSize 			xPrmManagementMessageSize;
	const TUnsignedInt8*	pxPrmManagementMessage;
} NA_dvlPostPrmManagementMessageInParam_t;

#if 0
typedef struct tagNA_dvlPostPrmManagementMessageOutParam_t
{
		  TSize 			xPrmManagementMessageSize;
	const TUnsignedInt8*   pxPrmManagementMessage;
} NA_dvlPostPrmManagementMessageOutParam_t;
#endif

// MW_CA_NA_dvlGetAuthenticatedMessage
typedef struct tagNA_dvlGetAuthenticatedMessageInParam_t
{
		  TSize 			xCipheredMessageSize;
	const TUnsignedInt8* 	pxCipheredMessage;
} NA_dvlGetAuthenticatedMessageInParam_t;


typedef struct tagNA_dvlGetAuthenticatedMessageOutParam_t
{
		  TSize*			pxAuthenticatedDataSize;
	const TUnsignedInt8**	ppxAuthenticatedData;
} NA_dvlGetAuthenticatedMessageOutParam_t;

// MW_CA_NA_dvlOpaqueDataMessageCreate
#if 0
typedef struct tagNA_dvlOpaqueDataMessageCreateInParam_t
{
	TOdmHandle**	ppxOpaqueDataMessageHandle;
} NA_dvlOpaqueDataMessageCreateInParam_t;
#endif

typedef struct tagNA_dvlOpaqueDataMessageCreateOutParam_t
{
	TOdmHandle**	ppxOpaqueDataMessageHandle;
} NA_dvlOpaqueDataMessageCreateOutParam_t;


// MW_CA_NA_dvlOpaqueDataMessageRelease
typedef struct tagNA_dvlOpaqueDataMessageReleaseInParam_t
{
	TOdmHandle*     pxOpaqueDataMessageHandle;
} NA_dvlOpaqueDataMessageReleaseInParam_t;

#if 0
typedef struct tagNA_dvlOpaqueDataMessageReleaseOutParam_t
{
	TOdmHandle*     pxOpaqueDataMessageHandle;
} NA_dvlOpaqueDataMessageReleaseOutParam_t;
#endif

// MW_CA_NA_dvlOpaqueDataMessageSetApplicationData
typedef struct tagNA_dvlOpaqueDataMessageSetApplicationDataInParam_t
{
		  TOdmHandle*		pxOpaqueDataMessageHandle;
		  TSize 			xApplicationDataSize;
	const TUnsignedInt8*	pxApplicationData;
} NA_dvlOpaqueDataMessageSetApplicationDataInParam_t;

#if 0
typedef struct tagNA_dvlOpaqueDataMessageSetApplicationDataOutParam_t
{
		  TOdmHandle*		pxOpaqueDataMessageHandle;
		  TSize 			xApplicationDataSize;
	const TUnsignedInt8*	pxApplicationData;
} NA_dvlOpaqueDataMessageSetApplicationDataOutParam_t;
#endif

// MW_CA_NA_dvlOpaqueDataMessageSetPrmSignalization
typedef struct tagNA_dvlOpaqueDataMessageSetPrmSignalizationInParam_t
{
		  TOdmHandle*		pxOpaqueDataMessageHandle;
		  TSize 			xPrmSignalizationSize;
	const TUnsignedInt8*	pxPrmSignalization;
} NA_dvlOpaqueDataMessageSetPrmSignalizationInParam_t;

#if 0
typedef struct tagNA_dvlOpaqueDataMessageSetPrmSignalizationOutParam_t
{
		  TOdmHandle*		pxOpaqueDataMessageHandle;
		  TSize 			xPrmSignalizationSize;
	const TUnsignedInt8*	pxPrmSignalization;
} NA_dvlOpaqueDataMessageSetPrmSignalizationOutParam_t;
#endif

// MW_CA_NA_dvlOpaqueDataMessageGet
typedef struct tagNA_dvlOpaqueDataMessageGetInParam_t
{
	TOdmHandle* 			pxOpaqueDataMessageHandle;
} NA_dvlOpaqueDataMessageGetInParam_t;

typedef struct tagNA_dvlOpaqueDataMessageGetOutParam_t
{
		  TSize*		 	pxOpaqueDataMessageSize;
	const TUnsignedInt8**	ppxOpaqueDataMessage;
} NA_dvlOpaqueDataMessageGetOutParam_t;

// caSessionMgr_AddTransportSessionIdForPrm
typedef struct tagNA_AddTransportSessionIdForPrmInParam_t
{
	TTransportSessionId   xTSid;
} NA_AddTransportSessionIdForPrmInParam_t;
#if 0
typedef struct tagNA_AddTransportSessionIdForPrmOutParam_t
{
	TTransportSessionId   xTSid;
} NA_AddTransportSessionIdForPrmOutParam_t;
#endif

// caSessionMgr_RemoveTransportSessionIdForPrm
typedef struct tagNA_RemoveTransportSessionIdForPrmInParam_t
{
	TTransportSessionId   xTSid;
} NA_RemoveTransportSessionIdForPrmInParam_t;

#if 0
typedef struct tagNA_RemoveTransportSessionIdForPrmOutParam_t
{
	TTransportSessionId   xTSid;
} NA_RemoveTransportSessionIdForPrmOutParam_t;
#endif

#endif

typedef struct tagNA_dvlUpdateSpecificLcmMetadataInParam_t
{
		TSize  xInLcmCredentialsSize;
	const TUnsignedInt8 *  pxInLcmCredentials;
		TSize  xSpecificMetadataSize;
	const TUnsignedInt8 *  pxSpecificMetadata;
} NA_dvlUpdateSpecificLcmMetadataInParam_t;

typedef struct tagNA_dvlUpdateSpecificLcmMetadataOutParam_t
{
		TSize *  pxOutLcmCredentialsSize;
	const TUnsignedInt8 **  ppxOutLcmCredentials;
} NA_dvlUpdateSpecificLcmMetadataOutParam_t;

#if defined(CONFIG_MW_CAS_NAGRA_PRM_VOD_PVR)
typedef struct tagNA_dvlCreateRecordSessionParametersInParam_t
{
	TDvlRecordSessionParameters*   pxRecordParameters;
} NA_dvlCreateRecordSessionParametersInParam_t;

typedef struct tagNA_dvlCreateRecordSessionParameters_t
{
	TDvlRecordSessionParameters**   ppxRecordParameters;
} NA_dvlCreateRecordSessionParametersOutParam_t;

typedef struct tagNA_dvlStartRecordExOutParam_t
{
	TSize* pxOutputCredentialsSize;
	const TUnsignedInt8** ppxOutputCredentials;
	TDvlRecordStatus* pxRecordingStatus;
	TDvlHandle** ppxRecordHandle;
} NA_dvlStartRecordExOutParam_t;

typedef struct tagNA_dvlStopRecordInParam_t
{
	TDvlHandle*  pxRecordHandle;
} NA_dvlStopRecordInParam_t;

#endif

#endif

#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
typedef struct tagNa_SetParentalControlInParam_t
{
	TTransportSessionId	xTsId;
	TBoolean			bUserPinPass;
} NA_SetParentalControlInParam_t;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
typedef struct tagNa_GetStandardErrorCodeInParam_t
{
	TUnsignedInt32		ulSessionId;
}NA_GetStandardErrorCodeInParam_t;

typedef struct tagNa_GetStandardErrorCodeOutParam_t
{
	TUnsignedInt32		ulStandardErrorCode;
}NA_GetStandardErrorCodeOutParam_t;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_ZONEID)
typedef struct tagNa_GetZoneIdInParam_t
{
	TUnsignedInt32	ulMaxLen;
}NA_GetZoneIdInParam_t;

typedef struct tagNa_GetZoneIdOutParam_t
{
	TUnsignedInt8*	pZoneId;
}NA_GetZoneIdOutParam_t;

typedef struct tagNa_GetCountryCodeInParam_t
{
	TUnsignedInt32	ulMaxLen;
}NA_GetCountryCodeInParam_t;

typedef struct tagNa_GetCountryCodeOutParam_t
{
	TUnsignedInt8*	pCountryCode;
}NA_GetCountryCodeOutParam_t;
#endif

union tagNa_InParam_t
{
	NA_RequestInfoInParam_t										stRequestInfo;
	NA_RegisterSystemInfoCallbackInParam_t						stRegisterSystemInfoCallback;
	NA_RegisterLocInfoCallbackInParam_t							stRegisterLocInfoCallback;
	NA_RegisterScAlarmCallbackInParam_t							stRegisterScAlarmCallback;
	NA_GetSystemInfoInParam_t									stGetSystemInfo;
//	NA_GetLocInfoInParam_t										stGetLocInfo;
	NA_CheckPincodeInParam_t 									stCheckPincode;
	NA_ChangePincodeInParam_t									stChangePincode;
#if defined(CONFIG_OP_CAS_NAGRA_CAKTEST_PPT)
	NA_Cak7_EnablingConsumptionForcedInParam_t					stCak7_EnablingConsumptionForced;
#endif
#if 0
	NA_GetProviderIdInParam_t									stGetProviderId;
	NA_GetEcmCaSystemIdInParam_t 								stGetEcmCaSystemId;
#endif
	NA_GetCurrScStatusInParam_t									stGetCurrScStatus;
	NA_ReportCurrScStatusInParam_t								stReportCurrScStatus;
//	NA_ReportCurrAlarmDataInParam_t								stReportCurrAlarmData;
//	NA_GetNumberOfSlotsInParam_t 								stGetNumberOfSlots;
	NA_IsSameCasIdInScInParam_t									stIsSameCasIdInSc;
//	NA_ResetIRDCmdDataInParam_t									stResetIRDCmdData;
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
//	NA_IRDCMD_MAIL_CheckUnreadMessageInParam_t					stIRDCMD_MAIL_CheckUnreadMessage;
//	NA_IRDCMD_MAIL_ClearMailMessageHeaderInParam_t				stIRDCMD_MAIL_ClearMailMessageHeader;
	NA_IRDCMD_MAIL_MakeMailMessageInParam_t						stIRDCMD_MAIL_MakeMailMessage;
//	NA_IRDCMD_MAIL_LoadMailMessagesInParam_t 					stIRDCMD_MAIL_LoadMailMessages;
	NA_IRDCMD_MAIL_GetMailPriorityInParam_t						stIRDCMD_MAIL_GetMailPriority;
//	NA_IRDCMD_MAIL_GetMailListInParam_t							stIRDCMD_MAIL_GetMailList;
	NA_IRDCMD_MAIL_GetMailDataInParam_t							stIRDCMD_MAIL_GetMailData;
	NA_IRDCMD_MAIL_DeleteMailMessageInParam_t					stIRDCMD_MAIL_DeleteMailMessage;
	NA_IRDCMD_MAIL_UpdateReadFlagInParam_t						stIRDCMD_MAIL_UpdateReadFlag;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_POPUP)
//	NA_IRDCMD_POPUP_DisplayPopupMessageInParam_t 				stIRDCMD_POPUP_DisplayPopupMessage;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
//	NA_IRDCMD_EMM_GetWakeupTimeInParam_t 						stIRDCMD_EMM_GetWakeupTime;
//	NA_IRDCMD_EMM_InitWakingTableInParam_t						stIRDCMD_EMM_InitWakingTable;
//	NA_IRDCMD_EMM_HasMoreNextWakingInParam_t 					stIRDCMD_EMM_HasMoreNextWaking;
//	NA_IRDCMD_EMM_GetWakingTuneInfoInParam_t 					stIRDCMD_EMM_GetWakingTuneInfo;
//	NA_IRDCMD_EMM_GetWakingTimerInParam_t						stIRDCMD_EMM_GetWakingTimer;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	NA_GetScExpirationDateInParam_t								stGetScExpirationDate;
	NA_IsHdPlusSmartCardInParam_t								stIsHdPlusSmartCard;
	NA_IsHdPlusServiceInParam_t									stIsHdPlusService;
	NA_GetSizeOfNaspCaDescInParam_t								stGetSizeOfNaspCaDesc;
	NA_GetContentOfNaspCaDescInParam_t							stGetContentOfNaspCaDesc;
	NA_GetValidityHdProductInParam_t 							stGetValidityHdProduct;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
	NA_RegisterScCreditCallbackInParam_t 						stRegisterScCreditCallback;
	NA_RegisterProductsLoadingCallbackInParam_t					stRegisterProductsLoadingCallback;
	NA_RegisterPurchaseHistoryCallbackInParam_t					stRegisterPurchaseHistoryCallback;
	NA_RegisterRechargeListCallbackInParam_t 					stRegisterRechargeListCallback;
	NA_RegisterOppvPurchaseListCallbackInParam_t 				stRegisterOppvPurchaseListCallback;
	NA_RegisterRechargeAlarmCallbackInParam_t					stRegisterRechargeAlarmCallback;
	NA_RegisterEventInfoCallbackInParam_t						stRegisterEventInfoCallback;
	NA_RegisterPackageListCallbackInParam_t						stRegisterPackageListCallback;
	NA_RegisterFullInfoPackageListNotifyCallbackInParam_t		stRegisterFullInfoPackageListNotifyCallback;
	NA_RegisterRequestPurchaseEventCallbackInParam_t 			stRegisterRequestPurchaseEventCallback;
	NA_RegisterRequestPurchasePackageCallbackInParam_t			stRegisterRequestPurchasePackageCallback;
	NA_RegisterDgtviScAlarmCallbackInParam_t 					stRegisterDgtviScAlarmCallback;
//	NA_GetScExpireDateForPpvInParam_t							stGetScExpireDateForPpv;
//	NA_GetCurrentDateStrInParam_t								stGetCurrentDateStr;
//	NA_GetScCreditStrInParam_t									stGetScCreditStr;
//	NA_GetScCreditInParam_t										stGetScCredit;
#endif
#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET) || defined(CONFIG_MW_CAS_NAGRA_OP_SPAIN_TDT)
	NA_NotifyTPChangedInParam_t									stNotifyTPChanged;
#endif
#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
	NA_NotifySvcChangedInParam_t 								stNotifySvcChanged;
	NA_RequestPurchaseHistoryInParam_t							stRequestPurchaseHistory;
//	NA_RequestRechargeListInParam_t								stRequestRechargeList;
//	NA_CakGetProviderIdInParam_t 								stCakGetProviderId;
	NA_RequestFullInfoPackageListInParam_t						stRequestFullInfoPackageList;
	NA_RequestPackageListInParam_t								stRequestPackageList;
	NA_RequestEventPurchaseInParam_t 							stRequestEventPurchase;
	NA_RequestPackagePurchaseInParam_t							stRequestPackagePurchase;
	NA_RequestProductPurchasableInParam_t						stRequestProductPurchasable;
	NA_GetProductGetDescriptionInParam_t 						stGetProductGetDescription;
	NA_GetProductGetDisclaimerInParam_t							stGetProductGetDisclaimer;
	NA_GetPackageEventInfoInParam_t								stGetPackageEventInfo;
//	NA_GetPurchaseHistoryNumInParam_t							stGetPurchaseHistoryNum;
	NA_DisposeRequestInParam_t								stDisposeRequest;
	NA_TsCheckInParam_t										stTsCheck;
//	NA_RequestEventInfoInParam_t								stRequestEventInfoIn;
#endif
#if defined(CONFIG_MW_CAS_NAGRA_OP_SPAIN_TDT)
//	NA_GetPPVStateInfoInParam_t									stGetPPVStateInfo;
//	NA_GetPackageListInParam_t									stGetPackageList;
//	NA_GetPurchaseHistoryListInParam_t							stGetPurchaseHistoryList;
//	NA_GetProviderNameInParam_t									stGetProviderName;
	NA_ReleaseProductListInParam_t								stReleaseProductList;
	NA_ReleasePurchaseHistoryListInParam_t						stReleasePurchaseHistoryList;
	NA_ReleaseProviderNameInParam_t						stReleaseProviderNameList;
#endif
#if	defined(CONFIG_MW_CAS_NAGRA_OP_RCS)
	NA_PPV_SetEpgModeInParam_t									stPPV_SetEpgMode;
	NA_PPV_ForceCreatEventInfoInParam_t							stPPV_ForceCreatEventInfo;
//	NA_PPV_GetPpvListInParam_t									stPPV_GetPpvList;
//	NA_PPV_GetCreditInParam_t									stPPV_GetCredit;
	NA_PPV_GetPurchaseEvtInfoInParam_t							stPPV_GetPurchaseEvtInfo;
	NA_PPV_RequestEventInfoInParam_t 							stPPV_RequestEventInfo;
	NA_PPV_RequestEventInfoByEpgInParam_t						stPPV_RequestEventInfoByEpg;
	NA_PPV_RequestPurchaseEventInParam_t 						stPPV_RequestPurchaseEvent;
	NA_PPV_RequestPurchaseEventByEpgInParam_t					stPPV_RequestPurchaseEventByEpg;
	NA_PPV_RegisterCreditCallbackInParam_t						stPPV_RegisterCreditCallback;
	NA_PPV_RegisterPpvListCallbackInParam_t						stPPV_RegisterPpvListCallback;
	NA_PPV_RegisterRechargeNotifyCallbackInParam_t				stPPV_RegisterRechargeNotifyCallback;
	NA_PPV_RegisterRequestPurchaseEventNotifyCallbackInParam_t	stPPV_RegisterRequestPurchaseEventNotifyCallback;
	NA_PPV_RegisterRequestEventNotifyCallbackInParam_t			stPPV_RegisterRequestEventNotifyCallback;
	NA_PPV_RegisterPurchaseHistoryNotifyCallbackInParam_t		stPPV_RegisterPurchaseHistoryNotifyCallback;
#endif
#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
	NA_GetSlotIDbySessionIDInParam_t 							stGetSlotIDbySessionID;
#endif
#if defined(CONFIG_MW_CAS_NAGRA_OP_TIVU)
//	NA_CheckTivuServiceInParam_t 								stCheckTivuService;
	NA_SetTivuServiceInParam_t									stSetTivuService;
//	NA_CheckTivuCardSystemIdInParam_t							stCheckTivuCardSystemId;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_PRM)
//	NA_dvlSystemInformationRequestInParam_t						stdvlSystemInformationRequestIn;
//	NA_dvlChipsetInformationRequestInParam_t					stdvlChipsetInformationRequestIn;
	NA_dvlReleaseBufferInParam_t								stdvlReleaseBufferIn;
	NA_dvlCredentialsInformationRequestInParam_t				stdvlCredentialsInformationRequestIn;
	NA_dvlStartPlaybackInParam_t								stdvlStartPlaybackIn;
	NA_dvlStopPlaybackInParam_t									stdvlStopPlaybackIn;
#if defined(CONFIG_MW_CAS_NAGRA_PRM_3_1)
	NA_dvlPlaybackOpenSessionInParam_t							stdlvPlaybackOpenSessionIn;
	NA_dvlPlaybackSetEntitlementsInParam_t						stdvlPlaybackSetEntitlementsIn;
	NA_dvlPlaybackCloseSessionInParam_t							stdvlPlaybackCloseSessionIn;
	NA_dvlPostPrmManagementMessageInParam_t						stdvlPostPrmManagementMessageIn;
	NA_dvlGetAuthenticatedMessageInParam_t						stdvlGetAuthenticatedMessageIn;
	//NA_dvlOpaqueDataMessageCreateInParam_t						stdvlOpaqueDataMessageCreateIn;
	NA_dvlOpaqueDataMessageReleaseInParam_t						stdvlOpaqueDataMessageReleaseIn;
	NA_dvlOpaqueDataMessageSetApplicationDataInParam_t			stdvlOpaqueDataMessageSetApplicationDataIn;
	NA_dvlOpaqueDataMessageSetPrmSignalizationInParam_t			stdvlOpaqueDataMessageSetPrmSignalizationIn;
	NA_dvlOpaqueDataMessageGetInParam_t							stdvlOpaqueDataMessageGetIn;
	NA_AddTransportSessionIdForPrmInParam_t						stAddTransportSessionIdForPrmIn;
	NA_RemoveTransportSessionIdForPrmInParam_t					stRemoveTransportSessionIdForPrmIn;
#endif

#endif

#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
	NA_SetParentalControlInParam_t								stSetParentalControlIn;
#endif
#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
	NA_GetStandardErrorCodeInParam_t							stGetStancardErrorCodeIn;
#endif

};

union tagNa_OutParam_t
{
//	NA_RequestInfoOutParam_t									stRequestInfo;
//	NA_RegisterSystemInfoCallbackOutParam_t						stRegisterSystemInfoCallback;
//	NA_RegisterLocInfoCallbackOutParam_t						stRegisterLocInfoCallback;
//	NA_RegisterScAlarmCallbackOutParam_t						stRegisterScAlarmCallback;
	NA_GetSystemInfoOutParam_t									stGetSystemInfo;
	NA_GetLocInfoOutParam_t										stGetLocInfo;
	NA_CheckPincodeOutParam_t 									stCheckPincode;
	NA_ChangePincodeOutParam_t									stChangePincode;
//	NA_Cak7_EnablingConsumptionForcedOutParam_t					stCak7_EnablingConsumptionForced;
#if 0
	NA_GetProviderIdOutParam_t									stGetProviderId;
	NA_GetEcmCaSystemIdOutParam_t 								stGetEcmCaSystemId;
#endif
	NA_GetCurrScStatusOutParam_t								stGetCurrScStatus;
//	NA_ReportCurrScStatusOutParam_t								stReportCurrScStatus;
//	NA_ReportCurrAlarmDataOutParam_t							stReportCurrAlarmData;
	NA_GetNumberOfSlotsOutParam_t 								stGetNumberOfSlots;
	NA_IsSameCasIdInScOutParam_t								stIsSameCasIdInSc;
//	NA_ResetIRDCmdDataOutParam_t								stResetIRDCmdData;
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
	NA_IRDCMD_MAIL_CheckUnreadMessageOutParam_t					stIRDCMD_MAIL_CheckUnreadMessage;
//	NA_IRDCMD_MAIL_ClearMailMessageHeaderOutParam_t				stIRDCMD_MAIL_ClearMailMessageHeader;
	NA_IRDCMD_MAIL_MakeMailMessageOutParam_t					stIRDCMD_MAIL_MakeMailMessage;
//	NA_IRDCMD_MAIL_LoadMailMessagesOutParam_t 					stIRDCMD_MAIL_LoadMailMessages;
	NA_IRDCMD_MAIL_GetMailPriorityOutParam_t					stIRDCMD_MAIL_GetMailPriority;
	NA_IRDCMD_MAIL_GetMailListOutParam_t						stIRDCMD_MAIL_GetMailList;
	NA_IRDCMD_MAIL_GetMailDataOutParam_t						stIRDCMD_MAIL_GetMailData;
//	NA_IRDCMD_MAIL_DeleteMailMessageOutParam_t					stIRDCMD_MAIL_DeleteMailMessage;
//	NA_IRDCMD_MAIL_UpdateReadFlagOutParam_t						stIRDCMD_MAIL_UpdateReadFlag;
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_POPUP)
//	NA_IRDCMD_POPUP_DisplayPopupMessageOutParam_t 				stIRDCMD_POPUP_DisplayPopupMessage;
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
	NA_IRDCMD_EMM_GetWakeupTimeOutParam_t 						stIRDCMD_EMM_GetWakeupTime;
//	NA_IRDCMD_EMM_InitWakingTableOutParam_t						stIRDCMD_EMM_InitWakingTable;
	NA_IRDCMD_EMM_HasMoreNextWakingOutParam_t 					stIRDCMD_EMM_HasMoreNextWaking;
	NA_IRDCMD_EMM_GetWakingTuneInfoOutParam_t 					stIRDCMD_EMM_GetWakingTuneInfo;
	NA_IRDCMD_EMM_GetWakingTimerOutParam_t						stIRDCMD_EMM_GetWakingTimer;
#endif
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	NA_GetScExpirationDateOutParam_t							stGetScExpirationDate;
	NA_IsHdPlusSmartCardOutParam_t								stIsHdPlusSmartCard;
	NA_IsHdPlusServiceOutParam_t								stIsHdPlusService;
	NA_GetSizeOfNaspCaDescOutParam_t							stGetSizeOfNaspCaDesc;
	NA_GetContentOfNaspCaDescOutParam_t							stGetContentOfNaspCaDesc;
	NA_GetValidityHdProductOutParam_t 							stGetValidityHdProduct;
#endif
#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
//	NA_RegisterScCreditCallbackOutParam_t 						stRegisterScCreditCallback;
//	NA_RegisterProductsLoadingCallbackOutParam_t				stRegisterProductsLoadingCallback;
//	NA_RegisterPurchaseHistoryCallbackOutParam_t				stRegisterPurchaseHistoryCallback;
//	NA_RegisterRechargeListCallbackOutParam_t 					stRegisterRechargeListCallback;
//	NA_RegisterOppvPurchaseListCallbackOutParam_t 				stRegisterOppvPurchaseListCallback;
//	NA_RegisterRechargeAlarmCallbackOutParam_t					stRegisterRechargeAlarmCallback;
//	NA_RegisterEventInfoCallbackOutParam_t						stRegisterEventInfoCallback;
//	NA_RegisterPackageListCallbackOutParam_t					stRegisterPackageListCallback;
//	NA_RegisterFullInfoPackageListNotifyCallbackOutParam_t		stRegisterFullInfoPackageListNotifyCallback;
//	NA_RegisterRequestPurchaseEventCallbackOutParam_t 			stRegisterRequestPurchaseEventCallback;
//	NA_RegisterRequestPurchasePackageCallbackOutParam_t			stRegisterRequestPurchasePackageCallback;
//	NA_RegisterDgtviScAlarmCallbackOutParam_t 					stRegisterDgtviScAlarmCallback;
	NA_GetScExpireDateForPpvOutParam_t							stGetScExpireDateForPpv;
	NA_GetCurrentDateStrOutParam_t								stGetCurrentDateStr;
	NA_GetScCreditStrOutParam_t									stGetScCreditStr;
	NA_GetScCreditOutParam_t									stGetScCredit;
#endif
#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET) || defined(CONFIG_MW_CAS_NAGRA_OP_SPAIN_TDT)
//	NA_NotifyTPChangedOutParam_t								stNotifyTPChanged;
#endif
#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
//	NA_NotifySvcChangedOutParam_t 								stNotifySvcChanged;
//	NA_RequestPurchaseHistoryOutParam_t							stRequestPurchaseHistory;
//	NA_RequestRechargeListOutParam_t							stRequestRechargeList;
	NA_CakGetProviderIdOutParam_t 								stCakGetProviderId;
//	NA_RequestFullInfoPackageListOutParam_t						stRequestFullInfoPackageList;
//	NA_RequestPackageListOutParam_t								stRequestPackageList;
//	NA_RequestEventPurchaseOutParam_t 							stRequestEventPurchase;
//	NA_RequestPackagePurchaseOutParam_t							stRequestPackagePurchase;
//	NA_RequestProductPurchasableOutParam_t						stRequestProductPurchasable;
	NA_GetProductGetDescriptionOutParam_t 						stGetProductGetDescription;
	NA_GetProductGetDisclaimerOutParam_t						stGetProductGetDisclaimer;
	NA_GetPackageEventInfoOutParam_t							stGetPackageEventInfo;
	NA_GetPurchaseHistoryNumOutParam_t							stGetPurchaseHistoryNum;
//	NA_DisposeRequestOutParam_t								stDisposeRequest
//	NA_TsCheckOutParam_t									stTsCheck;
//	NA_RequestEventInfoOutParam_t								stRequestEventInfo;
#endif
#if defined(CONFIG_MW_CAS_NAGRA_OP_SPAIN_TDT)
	NA_GetPPVStateInfoOutParam_t								stGetPPVStateInfo;
	NA_GetPackageListOutParam_t									stGetPackageList;
	NA_GetPurchaseHistoryListOutParam_t							stGetPurchaseHistoryList;
	NA_GetProviderNameOutParam_t								stGetProviderName;
//	NA_ReleaseProductListOutParam_t								stReleaseProductList;
//	NA_ReleasePurchaseHistoryListOutParam_t						stReleasePurchaseHistoryList;
//	NA_ReleaseProviderNameOutParam_t							stReleaseProviderNameList;
#endif
#if	defined(CONFIG_MW_CAS_NAGRA_OP_RCS)
//	NA_PPV_SetEpgModeOutParam_t									stPPV_SetEpgMode;
//	NA_PPV_ForceCreatEventInfoOutParam_t						stPPV_ForceCreatEventInfo;
	NA_PPV_GetPpvListOutParam_t									stPPV_GetPpvList;
	NA_PPV_GetCreditOutParam_t									stPPV_GetCredit;
	NA_PPV_GetPurchaseEvtInfoOutParam_t							stPPV_GetPurchaseEvtInfo;
//	NA_PPV_RequestEventInfoOutParam_t 							stPPV_RequestEventInfo;
	NA_PPV_RequestEventInfoByEpgOutParam_t						stPPV_RequestEventInfoByEpg;
//	NA_PPV_RequestPurchaseEventOutParam_t 						stPPV_RequestPurchaseEvent;
	NA_PPV_RequestPurchaseEventByEpgOutParam_t					stPPV_RequestPurchaseEventByEpg;
//	NA_PPV_RegisterCreditCallbackOutParam_t						stPPV_RegisterCreditCallback;
//	NA_PPV_RegisterPpvListCallbackOutParam_t					stPPV_RegisterPpvListCallback;
//	NA_PPV_RegisterRechargeNotifyCallbackOutParam_t				stPPV_RegisterRechargeNotifyCallback;
//	NA_PPV_RegisterRequestPurchaseEventNotifyCallbackOutParam_t	stPPV_RegisterRequestPurchaseEventNotifyCallback;
//	NA_PPV_RegisterRequestEventNotifyCallbackOutParam_t			stPPV_RegisterRequestEventNotifyCallback;
//	NA_PPV_RegisterPurchaseHistoryNotifyCallbackOutParam_t		stPPV_RegisterPurchaseHistoryNotifyCallback;
#endif
#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
	NA_GetSlotIDbySessionIDOutParam_t 							stGetSlotIDbySessionID;
#endif
#if defined(CONFIG_MW_CAS_NAGRA_OP_TIVU)
	NA_CheckTivuServiceOutParam_t 								stCheckTivuService;
//	NA_SetTivuServiceOutParam_t									stSetTivuService;
	NA_CheckTivuCardSystemIdOutParam_t							stCheckTivuCardSystemId;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_PRM)
	NA_dvlSystemInformationRequestOutParam_t					stdvlSystemInformationRequestOut;
	NA_dvlChipsetInformationRequestOutParam_t					stdvlChipsetInformationRequestOut;
	//NA_dvlReleaseBufferOutParam_t								stdvlReleaseBufferOut;
	NA_dvlCredentialsInformationRequestOutParam_t				stdvlCredentialsInformationRequestOut;
	NA_dvlStartPlaybackOutParam_t								stdvlStartPlaybackOut;
	//NA_dvlStopPlaybackOutParam_t								stdvlStopPlaybackOut;
#if defined(CONFIG_MW_CAS_NAGRA_PRM_3_1)
	NA_dvlPlaybackOpenSessionOutParam_t							stdlvPlaybackOpenSessionOut;
	NA_dvlPlaybackSetEntitlementsOutParam_t						stdvlPlaybackSetEntitlementsOut;
	//NA_dvlPlaybackCloseSessionOutParam_t							stdvlPlaybackCloseSessionOut;
	//NA_dvlPostPrmManagementMessageOutParam_t						stdvlPostPrmManagementMessageOut;
	NA_dvlGetAuthenticatedMessageOutParam_t						stdvlGetAuthenticatedMessageOut;
	NA_dvlOpaqueDataMessageCreateOutParam_t						stdvlOpaqueDataMessageCreateOut;
	//NA_dvlOpaqueDataMessageReleaseOutParam_t						stdvlOpaqueDataMessageReleaseOut;
	//NA_dvlOpaqueDataMessageSetApplicationDataOutParam_t			stdvlOpaqueDataMessageSetApplicationDataOut;
	//NA_dvlOpaqueDataMessageSetPrmSignalizationOutParam_t			stdvlOpaqueDataMessageSetPrmSignalizationOut;
	NA_dvlOpaqueDataMessageGetOutParam_t							stdvlOpaqueDataMessageGetOut;
	//NA_AddTransportSessionIdForPrmOutParam_t						stAddTransportSessionIdForPrmOut;
	//NA_RemoveTransportSessionIdForPrmOutParam_t					stRemoveTransportSessionIdForPrmOut;
#endif

#endif
#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
	NA_GetStandardErrorCodeOutParam_t							stGetStancardErrorCodeOut;
#endif
};

typedef union tagNa_InParam_t	Na_InParam_t;
typedef union tagNa_OutParam_t	Na_OutParam_t;

#if 0
#define NA_Param_t		CACTRL_NA_Param_t
struct NA_Param_t
{
	Na_InParam_t		*putIn;
	Na_OutParam_t		*putOut;
};
#endif

/********************************************************************
  Extern Function declaration.
 ********************************************************************/
extern	HERROR CA_NA_RegisterCtrlList(void);

#endif /* NAGRA_CTRL_H */

