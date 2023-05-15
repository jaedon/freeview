/**************************************************************************************************/
#define ________Nagra_ctrl_Private_Include__________________________________________________
/**************************************************************************************************/
#include <_svc_cas_mgr_control.h>
#include <namgr_main.h>
#include "nagra_ctrl.h"
#if defined(CONFIG_MW_CAS_NAGRA_PRM)
#include "na_mgr_dvl_mgr.h"
#endif



/**************************************************************************************************/
#define ________Nagra_ctrl_Golbal_Value____________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________Nagra_ctrl_Private_Define___________________________________________________
/**************************************************************************************************/




/**************************************************************************************************/
#define ________Nagra_ctrl_Private_Type_Define_____________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define ________Nagra_ctrl_Private_Static_Value______________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________Nagra_ctrl_Private_Static_Prototype__________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________Nagra_ctrl_Private_Static_Prototype_Body_____________________________________
/**************************************************************************************************/
static HERROR _naCtrl_RequestInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_RegisterSystemInfoCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_RegisterLocInfoCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_RegisterScAlarmCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_GetSystemInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_GetChipsetInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ALME)
static HERROR _naCtrl_GetCscMaxIndex(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#endif
#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME)
static HERROR _naCtrl_GetNumberOfIcc(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#endif
#if defined(CONFIG_MW_CAS_NAGRA_PRM)
static HERROR _naCtrl_GetDvlVersion(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_GetDvlChipsetType(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#endif
static HERROR _naCtrl_GetLocInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_CheckPincode(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_ChangePincode(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#if defined(CONFIG_OP_CAS_NAGRA_CAKTEST_PPT)
static HERROR _naCtrl_Cak7_EnablingConsumptionForced(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#endif
#if 0
static HERROR _naCtrl_GetProviderId(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_GetEcmCaSystemId(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#endif
static HERROR _naCtrl_GetCurrScStatus(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_ReportCurrScStatus(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_ReportCurrAlarmData(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_GetNumberOfSlots(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_IsSameCasIdInSc(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD)
static HERROR _naCtrl_ResetIRDCmdData(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
static HERROR _naCtrl_IRDCMD_MAIL_CheckUnreadMessage(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_IRDCMD_MAIL_ClearMailMessageHeader(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_IRDCMD_MAIL_MakeMailMessage(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_IRDCMD_MAIL_LoadMailMessages(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_IRDCMD_MAIL_GetMailPriority(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_IRDCMD_MAIL_GetMailList(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_IRDCMD_MAIL_GetMailData(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_IRDCMD_MAIL_DeleteMailMessage(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_IRDCMD_MAIL_UpdateReadFlag(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_POPUP)
static HERROR _naCtrl_IRDCMD_POPUP_DisplayPopupMessage(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
static HERROR _naCtrl_IRDCMD_EMM_GetWakeupTime(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_IRDCMD_EMM_InitWakingTable(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_IRDCMD_EMM_HasMoreNextWaking(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_IRDCMD_EMM_GetWakingTuneInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_IRDCMD_EMM_GetWakingTimer(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#endif
#endif
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
static HERROR _naCtrl_GetZipCode(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_GetScExpirationDate(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_IsHdPlusSmartCard(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_IsHdPlusService(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_GetPdExpirationDuration(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_GetValidityHdProduct(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
static HERROR _naCtrl_RegisterScCreditCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_RegisterProductsLoadingCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_RegisterPurchaseHistoryCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_RegisterRechargeListCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_RegisterOppvPurchaseListCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_RegisterRechargeAlarmCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_RegisterEventInfoCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_RegisterPackageListCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_RegisterFullInfoPackageListNotifyCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_RegisterRequestPurchaseEventCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_RegisterRequestPurchasePackageCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_RegisterDgtviScAlarmCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_GetScExpireDateForPpv(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_GetCurrentDateStr(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_GetScCreditStr(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_GetScCredit(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#endif
#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET) || defined(CONFIG_MW_CAS_NAGRA_OP_SPAIN_TDT)
static HERROR _naCtrl_NotifyTPChanged(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#endif
#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
static HERROR _naCtrl_NotifySvcChanged(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_RequestPurchaseHistory(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_RequestRechargeList(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_CakGetProviderId(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_RequestFullInfoPackageList(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_RequestPackageList(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_RequestEventPurchase(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_RequestPackagePurchase(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_RequestProductPurchasable(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_GetProductGetDescription(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_GetProductGetDisclaimer(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_GetPackageEventInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_GetPurchaseHistoryNum(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_DisposeRequest(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_TsCheck(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_RequestEventInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#endif
#if defined(CONFIG_MW_CAS_NAGRA_OP_SPAIN_TDT)
static HERROR _naCtrl_GetPPVStateInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_GetPackageList(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_GetPurchaseHistoryList(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_GetProviderName(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_ReleaseProductList(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_ReleasePurchaseHistoryList(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_ReleaseProviderName(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#endif
#if	defined(CONFIG_MW_CAS_NAGRA_OP_RCS)
static HERROR _naCtrl_PPV_SetEpgMode(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_PPV_ForceCreatEventInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_PPV_GetPpvList(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_PPV_GetCredit(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_PPV_GetPurchaseEvtInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_PPV_RequestEventInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_PPV_RequestEventInfoByEpg(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_PPV_RequestPurchaseEvent(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_PPV_RequestPurchaseEventByEpg(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_PPV_RegisterCreditCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_PPV_RegisterPpvListCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_PPV_RegisterRechargeNotifyCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_PPV_RegisterRequestPurchaseEventNotifyCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_PPV_RegisterRequestEventNotifyCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_PPV_RegisterPurchaseHistoryNotifyCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#endif
#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
static HERROR _naCtrl_GetSlotIDbySessionID(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#endif
#if defined(CONFIG_MW_CAS_NAGRA_OP_TIVU)
static HERROR _naCtrl_CheckTivuService(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_SetTivuService(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_CheckTivuCardSystemId(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_PRM)
static HERROR _naCtrl_dvlSystemInformationRequest(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_dvlChipsetInformationRequest(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_dvlReleaseBuffer(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_dvlCredentialsInformationRequest(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_dvlStartPlayback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_dvlStopPlayback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#if defined(CONFIG_MW_CAS_NAGRA_PRM_3_1)
static HERROR _naCtrl_dvlPlaybackOpenSession(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_dvlPlaybackSetEntitlements(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_dvlPlaybackCloseSession(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_dvlPostPrmManagementMessage(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_dvlGetAuthenticatedMessage(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_dvlOpaqueDataMessageCreate(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_dvlOpaqueDataMessageRelease(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_dvlOpaqueDataMessageSetApplicationData(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_dvlOpaqueDataMessageSetPrmSignalization(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_dvlOpaqueDataMessageGet(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_AddTransportSessionIdDirect(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_RemoveTransportSessionIdDirect(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#endif
static HERROR _naCtrl_dvlUpdateSpecificLcmMetadata(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#if defined(CONFIG_MW_CAS_NAGRA_PRM_VOD_PVR)
static HERROR _naCtrl_dvlCreateRecordSessionParameters(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_dvlStartRecordEx(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_dvlStopRecord(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#endif
#endif

#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
static HERROR _naCtrl_SetParentalControl(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_SetParentalControlLevel(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#endif
static HERROR _naCtrl_FactoryDefault(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
static HERROR _naCtrl_GetStandardErrorCode(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_ZONEID)
static HERROR _naCtrl_GetZoneId(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR _naCtrl_GetCountryCode(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#endif
/**************************************************************************************************/
#define ________Nagra_ctrl_External_Functions_Body__________________________________________
/**************************************************************************************************/

static const SvcCas_Ctrl_t s_astNaCtrlTable[eNaCtrl_MAX] =
{
	{
		eNaCtrl_RequestInfo,
		eCasCtrl_Req,
		NULL,	// Control
		NULL,	// Get
		NULL,	// Set
		_naCtrl_RequestInfo,
		NULL,	// Noti
	},

	{
		eNaCtrl_RegisterSystemInfoCallback,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		_naCtrl_RegisterSystemInfoCallback,
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_RegisterLocInfoCallback,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		_naCtrl_RegisterLocInfoCallback,
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_RegisterScAlarmCallback,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		_naCtrl_RegisterScAlarmCallback,
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_GetSystemInfo,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_GetSystemInfo,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_GetChipsetInfo,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_GetChipsetInfo,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ALME)
	{
		eNaCtrl_GetCscMaxIndex,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_GetCscMaxIndex,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},
#endif

#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME)
	{
		eNaCtrl_GetNumberOfIcc,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_GetNumberOfIcc,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},
#endif

#if defined(CONFIG_MW_CAS_NAGRA_PRM)
	{
		eNaCtrl_GetDvlVersion,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_GetDvlVersion,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_GetDvlChipsetType,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_GetDvlChipsetType,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},
#endif
	{
		eNaCtrl_GetLocInfo,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_GetLocInfo,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_CheckPincode,
		eCasCtrl_Control,
		_naCtrl_CheckPincode,
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_ChangePincode,
		eCasCtrl_Control,
		_naCtrl_ChangePincode,
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

#if defined(CONFIG_OP_CAS_NAGRA_CAKTEST_PPT)
	{
		eNaCtrl_Cak7_EnablingConsumptionForced,
		eCasCtrl_Control,
		_naCtrl_Cak7_EnablingConsumptionForced,
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},
#endif

#if 0
	{
		eNaCtrl_GetProviderId,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_GetProviderId,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_GetEcmCaSystemId,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_GetEcmCaSystemId,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},
#endif

	{
		eNaCtrl_GetCurrScStatus,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_GetCurrScStatus,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_ReportCurrScStatus,
		eCasCtrl_Control,
		_naCtrl_ReportCurrScStatus,
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_ReportCurrAlarmData,
		eCasCtrl_Control,
		_naCtrl_ReportCurrAlarmData,
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_GetNumberOfSlots,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_GetNumberOfSlots,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_IsSameCasIdInSc,
		eCasCtrl_Get,
#if defined (CONFIG_OP_SES)
		NULL,	// Control
#endif
		_naCtrl_IsSameCasIdInSc,
#if defined (CONFIG_OP_SES)
#else
		NULL,	// Get
#endif
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD)
	{
		eNaCtrl_ResetIRDCmdData,
		eCasCtrl_Control,
		_naCtrl_ResetIRDCmdData,
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
	{
		eNaCtrl_IRDCMD_MAIL_CheckUnreadMessage,
		eCasCtrl_Control,
		_naCtrl_IRDCMD_MAIL_CheckUnreadMessage,
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_IRDCMD_MAIL_ClearMailMessageHeader,
		eCasCtrl_Control,
		_naCtrl_IRDCMD_MAIL_ClearMailMessageHeader,
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_IRDCMD_MAIL_MakeMailMessage,
		eCasCtrl_Control,
		_naCtrl_IRDCMD_MAIL_MakeMailMessage,
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_IRDCMD_MAIL_LoadMailMessages,
		eCasCtrl_Control,
		_naCtrl_IRDCMD_MAIL_LoadMailMessages,
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_IRDCMD_MAIL_GetMailPriority,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_IRDCMD_MAIL_GetMailPriority,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_IRDCMD_MAIL_GetMailList,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_IRDCMD_MAIL_GetMailList,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_IRDCMD_MAIL_GetMailData,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_IRDCMD_MAIL_GetMailData,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_IRDCMD_MAIL_DeleteMailMessage,
		eCasCtrl_Control,
		_naCtrl_IRDCMD_MAIL_DeleteMailMessage,
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_IRDCMD_MAIL_UpdateReadFlag,
		eCasCtrl_Control,
		_naCtrl_IRDCMD_MAIL_UpdateReadFlag,
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},
#endif


#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_POPUP)
	{
		eNaCtrl_IRDCMD_POPUP_DisplayPopupMessage,
		eCasCtrl_Control,
		_naCtrl_IRDCMD_POPUP_DisplayPopupMessage,
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
	{
		eNaCtrl_IRDCMD_EMM_GetWakeupTime,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_IRDCMD_EMM_GetWakeupTime,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_IRDCMD_EMM_InitWakingTable,
		eCasCtrl_Control,
		_naCtrl_IRDCMD_EMM_InitWakingTable,
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_IRDCMD_EMM_HasMoreNextWaking,
		eCasCtrl_Control,
		_naCtrl_IRDCMD_EMM_HasMoreNextWaking,
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_IRDCMD_EMM_GetWakingTuneInfo,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_IRDCMD_EMM_GetWakingTuneInfo,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_IRDCMD_EMM_GetWakingTimer,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_IRDCMD_EMM_GetWakingTimer,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},
#endif
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	{
		eNaCtrl_GetZipCode,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_GetZipCode,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_GetScExpirationDate,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_GetScExpirationDate,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_IsHdPlusSmartCard,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_IsHdPlusSmartCard,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_IsHdPlusService,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_IsHdPlusService,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_GetPdExpirationDuration,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_GetPdExpirationDuration,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_GetValidityHdProduct,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_GetValidityHdProduct,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
	{
		eNaCtrl_RegisterScCreditCallback,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		_naCtrl_RegisterScCreditCallback,
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_RegisterProductsLoadingCallback,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		_naCtrl_RegisterProductsLoadingCallback,
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_RegisterPurchaseHistoryCallback,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		_naCtrl_RegisterPurchaseHistoryCallback,
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_RegisterRechargeListCallback,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		_naCtrl_RegisterRechargeListCallback,
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_RegisterOppvPurchaseListCallback,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		_naCtrl_RegisterOppvPurchaseListCallback,
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_RegisterRechargeAlarmCallback,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		_naCtrl_RegisterRechargeAlarmCallback,
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_RegisterEventInfoCallback,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		_naCtrl_RegisterEventInfoCallback,
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_RegisterPackageListCallback,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		_naCtrl_RegisterPackageListCallback,
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_RegisterFullInfoPackageListNotifyCallback,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		_naCtrl_RegisterFullInfoPackageListNotifyCallback,
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_RegisterRequestPurchaseEventCallback,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		_naCtrl_RegisterRequestPurchaseEventCallback,
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_RegisterRequestPurchasePackageCallback,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		_naCtrl_RegisterRequestPurchasePackageCallback,
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_RegisterDgtviScAlarmCallback,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		_naCtrl_RegisterDgtviScAlarmCallback,
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_GetScExpireDateForPpv,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_GetScExpireDateForPpv,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_GetCurrentDateStr,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_GetCurrentDateStr,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_GetScCreditStr,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_GetScCreditStr,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_GetScCredit,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_GetScCredit,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET) || defined(CONFIG_MW_CAS_NAGRA_OP_SPAIN_TDT)
	{
		eNaCtrl_NotifyTPChanged,
		eCasCtrl_Control,
		_naCtrl_NotifyTPChanged,
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
	{
		eNaCtrl_NotifySvcChanged,
		eCasCtrl_Control,
		_naCtrl_NotifySvcChanged,
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_RequestPurchaseHistory,
		eCasCtrl_Req,
		NULL,	// Control
		NULL,	// Get
		NULL,	// Set
		_naCtrl_RequestPurchaseHistory,
		NULL,	// Noti
	},

	{
		eNaCtrl_RequestRechargeList,
		eCasCtrl_Req,
		NULL,	// Control
		NULL,	// Get
		NULL,	// Set
		_naCtrl_RequestRechargeList,
		NULL,	// Noti
	},

	{
		eNaCtrl_CakGetProviderId,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_CakGetProviderId,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_RequestFullInfoPackageList,
		eCasCtrl_Req,
		NULL,	// Control
		NULL,	// Get
		NULL,	// Set
		_naCtrl_RequestFullInfoPackageList,
		NULL,	// Noti
	},

	{
		eNaCtrl_RequestPackageList,
		eCasCtrl_Req,
		NULL,	// Control
		NULL,	// Get
		NULL,	// Set
		_naCtrl_RequestPackageList,
		NULL,	// Noti
	},

	{
		eNaCtrl_RequestEventPurchase,
		eCasCtrl_Control,
		_naCtrl_RequestEventPurchase,
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_RequestPackagePurchase,
		eCasCtrl_Control,
		_naCtrl_RequestPackagePurchase,
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_RequestProductPurchasable,
		eCasCtrl_Control,
		_naCtrl_RequestProductPurchasable,
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_GetProductGetDescription,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_GetProductGetDescription,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_GetProductGetDisclaimer,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_GetProductGetDisclaimer,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_GetPackageEventInfo,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_GetPackageEventInfo,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_GetPurchaseHistoryNum,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_GetPurchaseHistoryNum,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},
	{
		eNaCtrl_DisposeRequest,
		eCasCtrl_Req,
		NULL,	// Control
		NULL,	// Get
		NULL,	// Set
		_naCtrl_DisposeRequest,
		NULL,	// Noti
	},
	{
		eNaCtrl_TsCheck,
		eCasCtrl_Control,
		NULL,	// Control
		NULL,	// Get
		NULL,	// Set
		_naCtrl_TsCheck,	// Req
		NULL,	// Noti
	},
	{
		eNaCtrl_RequestEventInfo,
		eCasCtrl_Req,
		NULL,	// Control
		NULL,	// Get
		NULL,	// Set
		_naCtrl_RequestEventInfo,
		NULL,	// Noti
	},
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_SPAIN_TDT)
	{
		eNaCtrl_GetPPVStateInfo,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_GetPPVStateInfo,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_GetPackageList,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_GetPackageList,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_GetPurchaseHistoryList,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_GetPurchaseHistoryList,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_GetProviderName,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_GetProviderName,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_ReleaseProductList,
		eCasCtrl_Control,
		_naCtrl_ReleaseProductList,
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_ReleasePurchaseHistoryList,
		eCasCtrl_Control,
		_naCtrl_ReleasePurchaseHistoryList,
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_ReleaseProviderName,
		eCasCtrl_Control,
		_naCtrl_ReleaseProviderName,
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},
#endif

#if	defined(CONFIG_MW_CAS_NAGRA_OP_RCS)
	{
		eNaCtrl_PPV_SetEpgMode,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		_naCtrl_PPV_SetEpgMode,
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_PPV_ForceCreatEventInfo,
		eCasCtrl_Control,
		_naCtrl_PPV_ForceCreatEventInfo,
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_PPV_GetPpvList,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_PPV_GetPpvList,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_PPV_GetCredit,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_PPV_GetCredit,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_PPV_GetPurchaseEvtInfo,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_PPV_GetPurchaseEvtInfo,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_PPV_RequestEventInfo,
		eCasCtrl_Control,
		_naCtrl_PPV_RequestEventInfo,
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_PPV_RequestEventInfoByEpg,
		eCasCtrl_Control,
		_naCtrl_PPV_RequestEventInfoByEpg,
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_PPV_RequestPurchaseEvent,
		eCasCtrl_Control,
		_naCtrl_PPV_RequestPurchaseEvent,
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_PPV_RequestPurchaseEventByEpg,
		eCasCtrl_Control,
		_naCtrl_PPV_RequestPurchaseEventByEpg,
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_PPV_RegisterCreditCallback,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		_naCtrl_PPV_RegisterCreditCallback,
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_PPV_RegisterPpvListCallback,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		_naCtrl_PPV_RegisterPpvListCallback,
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_PPV_RegisterRechargeNotifyCallback,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		_naCtrl_PPV_RegisterRechargeNotifyCallback,
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_PPV_RegisterRequestPurchaseEventNotifyCallback,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		_naCtrl_PPV_RegisterRequestPurchaseEventNotifyCallback,
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_PPV_RegisterRequestEventNotifyCallback,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		_naCtrl_PPV_RegisterRequestEventNotifyCallback,
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_PPV_RegisterPurchaseHistoryNotifyCallback,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		_naCtrl_PPV_RegisterPurchaseHistoryNotifyCallback,
		NULL,	// Req
		NULL,	// Noti
	},
#endif

#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
	{
		eNaCtrl_GetSlotIDbySessionID,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_GetSlotIDbySessionID,
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_TIVU)
	{
		eNaCtrl_CheckTivuService,
		eCasCtrl_Control,
		_naCtrl_CheckTivuService,
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_SetTivuService,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		_naCtrl_SetTivuService,
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_CheckTivuCardSystemId,
		eCasCtrl_Control,
		_naCtrl_CheckTivuCardSystemId,
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},
#endif

#if defined(CONFIG_MW_CAS_NAGRA_PRM)
	{
		eNaCtrl_dvlSystemInformationRequest,
		eCasCtrl_Control,
		_naCtrl_dvlSystemInformationRequest,	// Control
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_dvlChipsetInformationRequest,
		eCasCtrl_Control,
		_naCtrl_dvlChipsetInformationRequest,	// Control
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_dvlReleaseBuffer,
		eCasCtrl_Control,
		_naCtrl_dvlReleaseBuffer,	// Control
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_dvlCredentialsInformationRequest,
		eCasCtrl_Req,
		NULL,	// Control
		NULL,	// Get
		NULL,	// Set
		_naCtrl_dvlCredentialsInformationRequest,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_dvlStartPlayback,
		eCasCtrl_Control,
		_naCtrl_dvlStartPlayback,	// Control
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_dvlStopPlayback,
		eCasCtrl_Control,
		_naCtrl_dvlStopPlayback,	// Control
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

#if defined(CONFIG_MW_CAS_NAGRA_PRM_3_1)
	{
		eNaCtrl_dvlPlaybackOpenSession,
		eCasCtrl_Control,
		_naCtrl_dvlPlaybackOpenSession,	// Control
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_dvlPlaybackSetEntitlements,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		_naCtrl_dvlPlaybackSetEntitlements,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_dvlPlaybackCloseSession,
		eCasCtrl_Control,
		_naCtrl_dvlPlaybackCloseSession,	// Control
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_dvlPostPrmManagementMessage,
		eCasCtrl_Control,
		_naCtrl_dvlPostPrmManagementMessage,	// Control
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_dvlGetAuthenticatedMessage,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_dvlGetAuthenticatedMessage,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_dvlOpaqueDataMessageCreate,
		eCasCtrl_Control,
		_naCtrl_dvlOpaqueDataMessageCreate,	// Control
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_dvlOpaqueDataMessageRelease,
		eCasCtrl_Control,
		_naCtrl_dvlOpaqueDataMessageRelease,	// Control
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_dvlOpaqueDataMessageSetApplicationData,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		_naCtrl_dvlOpaqueDataMessageSetApplicationData,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_dvlOpaqueDataMessageSetPrmSignalization,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		_naCtrl_dvlOpaqueDataMessageSetPrmSignalization,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_dvlOpaqueDataMessageGet,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_dvlOpaqueDataMessageGet,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_AddTransportSessionIdDirect,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		_naCtrl_AddTransportSessionIdDirect,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_RemoveTransportSessionIdDirect,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		_naCtrl_RemoveTransportSessionIdDirect,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

#endif

	{
		eNaCtrl_dvlUpdateSpecificLcmMetadata,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		_naCtrl_dvlUpdateSpecificLcmMetadata,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

#if defined(CONFIG_MW_CAS_NAGRA_PRM_VOD_PVR)
	{
		eNaCtrl_dvlCreateRecordSessionParameters,
		eCasCtrl_Control,
		_naCtrl_dvlCreateRecordSessionParameters,	// Control
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_dvlStartRecordEx,
		eCasCtrl_Control,
		_naCtrl_dvlStartRecordEx,	// Control
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_dvlStopRecord,
		eCasCtrl_Control,
		_naCtrl_dvlStopRecord,	// Control
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},
#endif

#endif

#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
	{
		eNaCtrl_SetParentalControl,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		_naCtrl_SetParentalControl,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_SetParentalControlLevel,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		_naCtrl_SetParentalControlLevel,	// Set
		NULL,	// Req
		NULL,	// Noti
	},
#endif

	{
		eNaCtrl_FactoryDefault,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		_naCtrl_FactoryDefault,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
	{
		eNaCtrl_GetStandardErrorCode,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_GetStandardErrorCode,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_ZONEID)
	{
		eNaCtrl_GetZoneId,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_GetZoneId,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eNaCtrl_GetCountryCode,
		eCasCtrl_Get,
		NULL,	// Control
		_naCtrl_GetCountryCode,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	}
#endif
#if 0
	{
		eNaCtrl_MAX,
		eCasCtrl_None,
		NULL,	// Control
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},
#endif
};

HERROR CA_NA_RegisterCtrlList(void)
{
	HERROR	hErr;

	hErr = svc_cas_MgrSubInitControl(eDxCAS_GROUPID_NA);
	if (hErr == ERR_OK)
	{
		hErr = svc_cas_MgrSubRegisterControl(eDxCAS_GROUPID_NA, eNaCtrl_MAX, (SvcCas_Ctrl_t *)&s_astNaCtrlTable[0]);
	}

	return hErr;
}

//inline void *local_getInParam(SvcCas_CtrlParam_t *pstParam)
static inline Na_InParam_t *local_getInParam(SvcCas_CtrlParam_t *pstParam)
{
#if 0
	return (Na_InParam_t *)pstParam->unArg.pstNa->putIn;
#else
	return (Na_InParam_t *)pstParam->pvIn;
#endif
}

//inline void *local_getOutParam(SvcCas_CtrlParam_t *pstParam)
static inline Na_OutParam_t *local_getOutParam(SvcCas_CtrlParam_t *pstParam)
{
#if 0
	return (Na_OutParam_t *)pstParam->unArg.pstNa->putOut;
#else
	return (Na_OutParam_t *)pstParam->pvOut;
#endif
}

#if 0
static HERROR local_XXX(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	XXXInParam_t	*pstInParam;
	XXXOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (XXXInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (XXXOutParam_t *)local_getInParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

#if 0
	hErr = MW_CA_XXX(pstInParam-> , pstOutParam->);
#endif

	return hErr;
}
#endif

// eNaCtrl_RequestInfo,
static HERROR _naCtrl_RequestInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR						hErr;
	NA_RequestInfoInParam_t		*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_RequestInfoInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_RequestInfo(pstInParam->ulReqId);

	return hErr;
}


// eNaCtrl_RegisterSystemInfoCallback,
static HERROR _naCtrl_RegisterSystemInfoCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_RegisterSystemInfoCallbackInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_RegisterSystemInfoCallbackInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_RegisterSystemInfoCallback(pstInParam->callback);

	return hErr;
}

// eNaCtrl_RegisterLocInfoCallback,
static HERROR _naCtrl_RegisterLocInfoCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_RegisterLocInfoCallbackInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_RegisterLocInfoCallbackInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_RegisterLocInfoCallback(pstInParam->callback);

	return hErr;
}

// eNaCtrl_RegisterScAlarmCallback,
static HERROR _naCtrl_RegisterScAlarmCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_RegisterScAlarmCallbackInParam_t		*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_RegisterScAlarmCallbackInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_RegisterScAlarmCallback(pstInParam->callback);

	return hErr;
}

// eNaCtrl_GetSystemInfo,
static HERROR _naCtrl_GetSystemInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_GetSystemInfoOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_GetSystemInfoOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_GetSystemInfo(&pstOutParam->xStatus, &pstOutParam->pxIrdInfo, &pstOutParam->xNumberOfSmartcards, &pstOutParam->pxSmartcardInfoArray);

	return hErr;
}

// eNaCtrl_GetChipsetInfo
static HERROR _naCtrl_GetChipsetInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	NA_GetSystemInfoOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_GetSystemInfoOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	return MW_CA_NA_GetChipsetInfo(&pstOutParam->pstChipInfo);
}

#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ALME)
// eNaCtrl_GetCscMaxIndex
static HERROR _naCtrl_GetCscMaxIndex(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	TUnsignedInt8	*pxOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pxOutParam = (TUnsignedInt8 *)local_getOutParam(pstParam);
	if (pxOutParam == NULL)
	{
		return ERR_FAIL;
	}

	return MW_CA_NA_GetCscMaxIndex(pxOutParam);
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME)
// eNaCtrl_GetNumberOfIcc
static HERROR _naCtrl_GetNumberOfIcc(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	TUnsignedInt8	*pxOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pxOutParam = (TUnsignedInt8 *)local_getOutParam(pstParam);
	if (pxOutParam == NULL)
	{
		return ERR_FAIL;
	}

	return MW_CA_NA_GetNumberOfIcc(pxOutParam);
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_PRM)
// eNaCtrl_GetDvlVersion
static HERROR _naCtrl_GetDvlVersion(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	NA_GetSystemInfoOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_GetSystemInfoOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	return CA_NA_GetDvlVersion((TUnsignedInt8 **)&pstOutParam->pxDvlVersion);
}

// eNaCtrl_GetDvlChipsetType
static HERROR _naCtrl_GetDvlChipsetType(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	NA_GetSystemInfoOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_GetSystemInfoOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	return CA_NA_GetDvlChipsetType((TUnsignedInt8 **)&pstOutParam->pxDvlChipsetType);
}
#endif

// eNaCtrl_GetLocInfo,
static HERROR _naCtrl_GetLocInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_GetLocInfoOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_GetLocInfoOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_GetLocInfo(&pstOutParam->xStatus, &pstOutParam->xNumberOfSmartcards, &pstOutParam->pxLocationInfoArray);

	return hErr;
}

// eNaCtrl_CheckPincode,
static HERROR _naCtrl_CheckPincode(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_CheckPincodeInParam_t	*pstInParam;
	NA_CheckPincodeOutParam_t *pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_CheckPincodeInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_CheckPincodeOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_CheckPincode(pstInParam->xPinIndex, pstInParam->xPinCode, pstInParam->pucPinCode, &pstOutParam->xStatus);

	return hErr;
}

// eNaCtrl_ChangePincode,
static HERROR _naCtrl_ChangePincode(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_ChangePincodeInParam_t	*pstInParam;
	NA_ChangePincodeOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_ChangePincodeInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_ChangePincodeOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_ChangePincode(pstInParam->xPinIndex, pstInParam->xOldPinCode, pstInParam->xNewPinCode, pstInParam->pucOldPinCode, pstInParam->pucNewPinCode, &pstOutParam->xStatus);

	return hErr;
}

#if defined(CONFIG_OP_CAS_NAGRA_CAKTEST_PPT)
// eNaCtrl_Cak7_EnablingConsumptionForced,
static HERROR _naCtrl_Cak7_EnablingConsumptionForced(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_Cak7_EnablingConsumptionForcedInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_Cak7_EnablingConsumptionForcedInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_Cak7_EnablingConsumptionForced(pstInParam->bOneShot);

	return hErr;
}
#endif

#if 0
// eNaCtrl_GetProviderId,
static HERROR _naCtrl_GetProviderId(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_GetProviderIdInParam_t	*pstInParam;
	NA_GetProviderIdOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_GetProviderIdInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_GetProviderIdOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_GetProviderId(pstInParam->ucSlotId, &pstOutParam->usProviderId);

	return hErr;
}

// eNaCtrl_GetEcmCaSystemId,
static HERROR _naCtrl_GetEcmCaSystemId(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_GetEcmCaSystemIdInParam_t	*pstInParam;
	NA_GetEcmCaSystemIdOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_GetEcmCaSystemIdInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_GetEcmCaSystemIdOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_GetEcmCaSystemId(pstInParam->ucSlotId, &pstOutParam->usCaSysId);

	return hErr;
}
#endif

// eNaCtrl_GetCurrScStatus,
static HERROR _naCtrl_GetCurrScStatus(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_GetCurrScStatusInParam_t	*pstInParam;
	NA_GetCurrScStatusOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_GetCurrScStatusInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_GetCurrScStatusOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_GetCurrScStatus(pstInParam->ulSlotId, &pstOutParam->eScAlarm);

	return hErr;
}

// eNaCtrl_ReportCurrScStatus,
/* Using in Sub CAS module */
static HERROR _naCtrl_ReportCurrScStatus(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_ReportCurrScStatusInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_ReportCurrScStatusInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_ReportCurrScStatus(pstInParam->hAction);

	return hErr;
}

// eNaCtrl_ReportCurrAlarmData,
/* Using in Sub CAS module */
static HERROR _naCtrl_ReportCurrAlarmData(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	(void) (pstParam);

	hErr = MW_CA_NA_ReportCurrAlarmData();

	return hErr;
}

// eNaCtrl_GetNumberOfSlots,
static HERROR _naCtrl_GetNumberOfSlots(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_GetNumberOfSlotsOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_GetNumberOfSlotsOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_GetNumberOfSlots(&pstOutParam->ucSlots);

	return hErr;
}

// eNaCtrl_IsSameCasIdInSc,
static HERROR _naCtrl_IsSameCasIdInSc(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_IsSameCasIdInScInParam_t	*pstInParam;
	NA_IsSameCasIdInScOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_IsSameCasIdInScInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_IsSameCasIdInScOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_IsSameCasIdInSc(pstInParam->ucSlotId, pstInParam->usCaSysId);

	if(hErr == ERR_OK)
	{
		pstOutParam->bSame = TRUE;
	}
	else
	{
		pstOutParam->bSame = FALSE;
	}

	return hErr;
}

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD)
// eNaCtrl_ResetIRDCmdData,
static HERROR _naCtrl_ResetIRDCmdData(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	(void) (pstParam);

	hErr = MW_CA_NA_ResetIRDCmdData();

	return hErr;
}

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
// eNaCtrl_IRDCMD_MAIL_CheckUnreadMessage,
static HERROR _naCtrl_IRDCMD_MAIL_CheckUnreadMessage(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_IRDCMD_MAIL_CheckUnreadMessageOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_IRDCMD_MAIL_CheckUnreadMessageOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_IRDCMD_MAIL_CheckUnreadMessage();

	if(hErr == ERR_OK)
	{
		pstOutParam->bUnread = TRUE;
	}
	else
	{
		pstOutParam->bUnread = FALSE;
	}

	return hErr;
}

// eNaCtrl_IRDCMD_MAIL_ClearMailMessageHeader,
static HERROR _naCtrl_IRDCMD_MAIL_ClearMailMessageHeader(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	(void) (pstParam);

	hErr = MW_CA_NA_IRDCMD_MAIL_ClearMailMessageHeader();

	return hErr;
}

// eNaCtrl_IRDCMD_MAIL_MakeMailMessage,
static HERROR _naCtrl_IRDCMD_MAIL_MakeMailMessage(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_IRDCMD_MAIL_MakeMailMessageInParam_t	*pstInParam;
	NA_IRDCMD_MAIL_MakeMailMessageOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_IRDCMD_MAIL_MakeMailMessageInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_IRDCMD_MAIL_MakeMailMessageOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_IRDCMD_MAIL_MakeMailMessage(pstInParam->pIrdCmd, &pstOutParam->nMailId, &pstOutParam->fCompleted, &pstOutParam->nDeletedMailId);

	return hErr;
}

// eNaCtrl_IRDCMD_MAIL_LoadMailMessages,
static HERROR _naCtrl_IRDCMD_MAIL_LoadMailMessages(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;

	(void)(pstParam);

	hErr = MW_CA_NA_IRDCMD_MAIL_LoadMailMessages();

	return hErr;
}

// eNaCtrl_IRDCMD_MAIL_GetMailPriority,
static HERROR _naCtrl_IRDCMD_MAIL_GetMailPriority(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_IRDCMD_MAIL_GetMailPriorityInParam_t	*pstInParam;
	NA_IRDCMD_MAIL_GetMailPriorityOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_IRDCMD_MAIL_GetMailPriorityInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_IRDCMD_MAIL_GetMailPriorityOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_IRDCMD_MAIL_GetMailPriority(pstInParam->nMailId, &pstOutParam->nPriority);

	return hErr;
}

// eNaCtrl_IRDCMD_MAIL_GetMailList,
static HERROR _naCtrl_IRDCMD_MAIL_GetMailList(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_IRDCMD_MAIL_GetMailListOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_IRDCMD_MAIL_GetMailListOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_IRDCMD_MAIL_GetMailList(pstOutParam->pppMailList, &pstOutParam->nCount);

	return hErr;
}

// eNaCtrl_IRDCMD_MAIL_GetMailData,
static HERROR _naCtrl_IRDCMD_MAIL_GetMailData(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_IRDCMD_MAIL_GetMailDataInParam_t	*pstInParam;
	NA_IRDCMD_MAIL_GetMailDataOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_IRDCMD_MAIL_GetMailDataInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_IRDCMD_MAIL_GetMailDataOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_IRDCMD_MAIL_GetMailData(pstInParam->nMailId, pstOutParam->ppMailData);

	return hErr;
}

// eNaCtrl_IRDCMD_MAIL_DeleteMailMessage,
static HERROR _naCtrl_IRDCMD_MAIL_DeleteMailMessage(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_IRDCMD_MAIL_DeleteMailMessageInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_IRDCMD_MAIL_DeleteMailMessageInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_IRDCMD_MAIL_DeleteMailMessage(pstInParam->nMailId);

	return hErr;
}

// eNaCtrl_IRDCMD_MAIL_UpdateReadFlag,
static HERROR _naCtrl_IRDCMD_MAIL_UpdateReadFlag(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_IRDCMD_MAIL_UpdateReadFlagInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_IRDCMD_MAIL_UpdateReadFlagInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_IRDCMD_MAIL_UpdateReadFlag(pstInParam->nMailId, pstInParam->bRead);

	return hErr;
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_POPUP)
// eNaCtrl_IRDCMD_POPUP_DisplayPopupMessage,
static HERROR _naCtrl_IRDCMD_POPUP_DisplayPopupMessage(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	(void)(pstParam);

	hErr = MW_CA_NA_IRDCMD_POPUP_DisplayPopupMessage();

	return hErr;
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
// eNaCtrl_IRDCMD_EMM_GetWakeupTime,
static HERROR _naCtrl_IRDCMD_EMM_GetWakeupTime(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_IRDCMD_EMM_GetWakeupTimeOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_IRDCMD_EMM_GetWakeupTimeOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_IRDCMD_EMM_GetWakeupTime(&pstOutParam->ulTime);

	return hErr;
}

// eNaCtrl_IRDCMD_EMM_InitWakingTable,
static HERROR _naCtrl_IRDCMD_EMM_InitWakingTable(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	(void)(pstParam);

	hErr = MW_CA_NA_IRDCMD_EMM_InitWakingTable();

	return hErr;
}

// eNaCtrl_IRDCMD_EMM_HasMoreNextWaking,
static HERROR _naCtrl_IRDCMD_EMM_HasMoreNextWaking(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_IRDCMD_EMM_HasMoreNextWakingOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_IRDCMD_EMM_HasMoreNextWakingOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_IRDCMD_EMM_HasMoreNextWaking(&pstOutParam->bNext);

	return hErr;
}

// eNaCtrl_IRDCMD_EMM_GetWakingTuneInfo,
static HERROR _naCtrl_IRDCMD_EMM_GetWakingTuneInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_IRDCMD_EMM_GetWakingTuneInfoOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_IRDCMD_EMM_GetWakingTuneInfoOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_IRDCMD_EMM_GetWakingTuneInfo(&pstOutParam->ucNetworkIdFlag, &pstOutParam->usNetworkId,
														&pstOutParam->ucTsIdFlag, &pstOutParam->usTsId,
														&pstOutParam->ucServiceIdFlag, &pstOutParam->usServiceId);

	return hErr;
}

// eNaCtrl_IRDCMD_EMM_GetWakingTimer,
static HERROR _naCtrl_IRDCMD_EMM_GetWakingTimer(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_IRDCMD_EMM_GetWakingTimerOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_IRDCMD_EMM_GetWakingTimerOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_IRDCMD_EMM_GetWakingTimer(&pstOutParam->ulTime);

	return hErr;
}
#endif
#endif //#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD)

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
// eNaCtrl_GetZipCode,
HERROR _naCtrl_GetZipCode(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_GetZipCodeInParam_t *pstInParam;
	NA_GetZipCodeOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_GetZipCodeInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_GetZipCodeOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_GetZipCode(pstInParam->ucSlotId, &pstOutParam->ulZipCode);

	return hErr;
}

// eNaCtrl_GetScExpirationDate,
static HERROR _naCtrl_GetScExpirationDate(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_GetScExpirationDateInParam_t	*pstInParam;
	NA_GetScExpirationDateOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_GetScExpirationDateInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_GetScExpirationDateOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_GetScExpirationDate(pstInParam->ucSlotId, &pstOutParam->bEndOfTime, &pstOutParam->stDataTime);

	return hErr;
}

// eNaCtrl_IsHdPlusSmartCard,
static HERROR _naCtrl_IsHdPlusSmartCard(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_IsHdPlusSmartCardInParam_t	*pstInParam;
	NA_IsHdPlusSmartCardOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_IsHdPlusSmartCardInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_IsHdPlusSmartCardOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_IsHdPlusSmartCard(pstInParam->ucSlotNo, &pstOutParam->bHdPlusSc);

	return hErr;
}

// eNaCtrl_IsHdPlusService,
static HERROR _naCtrl_IsHdPlusService(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_IsHdPlusServiceInParam_t	*pstInParam;
	NA_IsHdPlusServiceOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_IsHdPlusServiceInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_IsHdPlusServiceOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_IsHdPlusService(pstInParam->hAction);
	if(hErr == ERR_OK)
	{
		pstOutParam->bStatus = TRUE;
	}
	else
	{
		pstOutParam->bStatus = FALSE;
	}

	return hErr;
}

// eNaCtrl_GetPdExpirationDuration,
static HERROR _naCtrl_GetPdExpirationDuration(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_GetPdExpirationDurationOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_GetPdExpirationDurationOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_GetPdExpirationDuration(&pstOutParam->expireDuraion);
	return hErr;
}


// eNaCtrl_GetValidityHdProduct,
static HERROR _naCtrl_GetValidityHdProduct(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_GetValidityHdProductInParam_t	*pstInParam;
	NA_GetValidityHdProductOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_GetValidityHdProductInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_GetValidityHdProductOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_GetValidityHdProduct(pstInParam->eType, pstInParam->ucDscLen, pstInParam->pucDesc, &pstOutParam->bExpired, &pstOutParam->ulDuration);

	return hErr;
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
// eNaCtrl_RegisterScCreditCallback,
static HERROR _naCtrl_RegisterScCreditCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_RegisterScCreditCallbackInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_RegisterScCreditCallbackInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_RegisterScCreditCallback(pstInParam->callback);

	return hErr;
}

// eNaCtrl_RegisterProductsLoadingCallback,
static HERROR _naCtrl_RegisterProductsLoadingCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_RegisterProductsLoadingCallbackInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_RegisterProductsLoadingCallbackInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_RegisterProductsLoadingCallback(pstInParam->callback);

	return hErr;
}

// eNaCtrl_RegisterPurchaseHistoryCallback,
static HERROR _naCtrl_RegisterPurchaseHistoryCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_RegisterPurchaseHistoryCallbackInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_RegisterPurchaseHistoryCallbackInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_RegisterPurchaseHistoryCallback(pstInParam->callback);

	return hErr;
}

// eNaCtrl_RegisterRechargeListCallback,
static HERROR _naCtrl_RegisterRechargeListCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_RegisterRechargeListCallbackInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_RegisterRechargeListCallbackInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_RegisterRechargeListCallback(pstInParam->callback);

	return hErr;
}

// eNaCtrl_RegisterOppvPurchaseListCallback,
static HERROR _naCtrl_RegisterOppvPurchaseListCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_RegisterOppvPurchaseListCallbackInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_RegisterOppvPurchaseListCallbackInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_RegisterOppvPurchaseListCallback(pstInParam->callback);

	return hErr;
}

// eNaCtrl_RegisterRechargeAlarmCallback,
static HERROR _naCtrl_RegisterRechargeAlarmCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_RegisterRechargeAlarmCallbackInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_RegisterRechargeAlarmCallbackInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_RegisterRechargeAlarmCallback(pstInParam->callback);

	return hErr;
}

// eNaCtrl_RegisterEventInfoCallback,
static HERROR _naCtrl_RegisterEventInfoCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_RegisterEventInfoCallbackInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_RegisterEventInfoCallbackInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_RegisterEventInfoCallback(pstInParam->callback);

	return hErr;
}

// eNaCtrl_RegisterPackageListCallback,
static HERROR _naCtrl_RegisterPackageListCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_RegisterPackageListCallbackInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_RegisterPackageListCallbackInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_RegisterPackageListCallback(pstInParam->callback);

	return hErr;
}

// eNaCtrl_RegisterFullInfoPackageListNotifyCallback,
static HERROR _naCtrl_RegisterFullInfoPackageListNotifyCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_RegisterFullInfoPackageListNotifyCallbackInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_RegisterFullInfoPackageListNotifyCallbackInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_RegisterFullInfoPackageListNotifyCallback(pstInParam->callback);

	return hErr;
}

// eNaCtrl_RegisterRequestPurchaseEventCallback,
static HERROR _naCtrl_RegisterRequestPurchaseEventCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_RegisterRequestPurchaseEventCallbackInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_RegisterRequestPurchaseEventCallbackInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_RegisterRequestPurchaseEventCallback(pstInParam->callback);

	return hErr;
}

// eNaCtrl_RegisterRequestPurchasePackageCallback,
static HERROR _naCtrl_RegisterRequestPurchasePackageCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_RegisterRequestPurchasePackageCallbackInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_RegisterRequestPurchasePackageCallbackInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_RegisterRequestPurchasePackageCallback(pstInParam->callback);

	return hErr;
}

// eNaCtrl_RegisterDgtviScAlarmCallback,
static HERROR _naCtrl_RegisterDgtviScAlarmCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_RegisterDgtviScAlarmCallbackInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_RegisterDgtviScAlarmCallbackInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_RegisterDgtviScAlarmCallback(pstInParam->callback);

	return hErr;
}

// eNaCtrl_GetScExpireDateForPpv,
static HERROR _naCtrl_GetScExpireDateForPpv(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_GetScExpireDateForPpvOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_GetScExpireDateForPpvOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_GetScExpireDateForPpv(pstOutParam->pszDate, &pstOutParam->bExpired);

	return hErr;
}

// eNaCtrl_GetCurrentDateStr,
static HERROR _naCtrl_GetCurrentDateStr(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_GetCurrentDateStrOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_GetCurrentDateStrOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_GetCurrentDateStr(pstOutParam->pszDate);

	return hErr;
}

// eNaCtrl_GetScCreditStr,
static HERROR _naCtrl_GetScCreditStr(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_GetScCreditStrOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_GetScCreditStrOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_GetScCreditStr(pstOutParam->pszCredit);

	return hErr;
}

// eNaCtrl_GetScCredit,
static HERROR _naCtrl_GetScCredit(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_GetScCreditOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_GetScCreditOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_GetScCredit(&pstOutParam->ulCredit);

	return hErr;
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET) || defined(CONFIG_MW_CAS_NAGRA_OP_SPAIN_TDT)
// eNaCtrl_NotifyTPChanged,
static HERROR _naCtrl_NotifyTPChanged(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_NotifyTPChangedInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_NotifyTPChangedInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_NotifyTPChanged(pstInParam->bTpChanged);

	return hErr;
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
// eNaCtrl_NotifySvcChanged,
static HERROR _naCtrl_NotifySvcChanged(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_NotifySvcChangedInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_NotifySvcChangedInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_NotifySvcChanged(pstInParam->hSvc, pstInParam->hAction);

	return hErr;
}

// eNaCtrl_RequestPurchaseHistory,
static HERROR _naCtrl_RequestPurchaseHistory(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_RequestPurchaseHistoryInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_RequestPurchaseHistoryInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_RequestPurchaseHistory(pstInParam->ulProviderId);

	return hErr;
}

// eNaCtrl_RequestRechargeList,
static HERROR _naCtrl_RequestRechargeList(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	(void)(pstParam);

	hErr = MW_CA_NA_RequestRechargeList();

	return hErr;
}

// eNaCtrl_CakGetProviderId,
static HERROR _naCtrl_CakGetProviderId(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_CakGetProviderIdOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_CakGetProviderIdOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_CakGetProviderId(&pstOutParam->usProviderId);

	return hErr;
}

// eNaCtrl_RequestFullInfoPackageList,
static HERROR _naCtrl_RequestFullInfoPackageList(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_RequestFullInfoPackageListInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_RequestFullInfoPackageListInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_RequestFullInfoPackageList(pstInParam->pProviderId);

	return hErr;
}

// eNaCtrl_RequestPackageList,
static HERROR _naCtrl_RequestPackageList(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_RequestPackageListInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_RequestPackageListInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_RequestPackageList(pstInParam->ulProviderId);

	return hErr;
}

// eNaCtrl_RequestEventPurchase,
static HERROR _naCtrl_RequestEventPurchase(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_RequestEventPurchaseInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_RequestEventPurchaseInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_RequestEventPurchase(pstInParam->xStatus, pstInParam->pRequest, pstInParam->pEvent, pstInParam->pProduct);

	return hErr;
}

// eNaCtrl_RequestPackagePurchase,
static HERROR _naCtrl_RequestPackagePurchase(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_RequestPackagePurchaseInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_RequestPackagePurchaseInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_RequestPackagePurchase(pstInParam->pProduct);

	return hErr;
}

// eNaCtrl_RequestProductPurchasable,
static HERROR _naCtrl_RequestProductPurchasable(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_RequestProductPurchasableInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_RequestProductPurchasableInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_RequestProductPurchasable(pstInParam->pRequest, pstInParam->pProduct);

	return hErr;
}

// eNaCtrl_GetProductGetDescription,
static HERROR _naCtrl_GetProductGetDescription(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_GetProductGetDescriptionInParam_t	*pstInParam;
	NA_GetProductGetDescriptionOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_GetProductGetDescriptionInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_GetProductGetDescriptionOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_GetProductGetDescription(pstInParam->pxProduct, &pstOutParam->tStrLen, &pstOutParam->pBuffer);

	return hErr;
}

// eNaCtrl_GetProductGetDisclaimer,
static HERROR _naCtrl_GetProductGetDisclaimer(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_GetProductGetDisclaimerInParam_t	*pstInParam;
	NA_GetProductGetDisclaimerOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_GetProductGetDisclaimerInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_GetProductGetDisclaimerOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_GetProductGetDisclaimer(pstInParam->pxProduct, &pstOutParam->tStrLen, &pstOutParam->pBuffer);

	return hErr;
}

// eNaCtrl_GetPackageEventInfo,
static HERROR _naCtrl_GetPackageEventInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_GetPackageEventInfoInParam_t	*pstInParam;
	NA_GetPackageEventInfoOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_GetPackageEventInfoInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_GetPackageEventInfoOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_GetPackageEventInfo(pstInParam->pRequest, pstInParam->pPackage, pstOutParam->pPackageInfo);

	return hErr;
}

// eNaCtrl_GetPurchaseHistoryNum,
static HERROR _naCtrl_GetPurchaseHistoryNum(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_GetPurchaseHistoryNumOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_GetPurchaseHistoryNumOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_GetPurchaseHistoryNum(&pstOutParam->ulItems);

	return hErr;
}

// eNaCtrl_DisposeRequest
static HERROR _naCtrl_DisposeRequest(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR						hErr;
	NA_DisposeRequestInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_DisposeRequestInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_DisposeRequest(&pstInParam->pRequest);

	return hErr;
}

// eNaCtrl_TsCheck
static HERROR _naCtrl_TsCheck(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_TsCheckInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_TsCheckInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_TsCheck(pstInParam->hSvcHandle);

	return hErr;
}

// eNaCtrl_RequestEventInfo
static HERROR _naCtrl_RequestEventInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	(void)(pstParam);

	hErr = MW_CA_NA_RequestEventInfo();

	return hErr;
}

#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_SPAIN_TDT)
// eNaCtrl_GetPPVStateInfo,
static HERROR _naCtrl_GetPPVStateInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_GetPPVStateInfoOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_GetPPVStateInfoOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_GetPPVStateInfo(&pstOutParam->eState, &pstOutParam->bNagraCaSys);

	return hErr;
}

// eNaCtrl_GetPackageList,
static HERROR _naCtrl_GetPackageList(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_GetPackageListOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_GetPackageListOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_GetPackageList(&pstOutParam->ulNumOfProducts, pstOutParam->ppPackageList);

	return hErr;
}

// eNaCtrl_GetPurchaseHistoryList,
static HERROR _naCtrl_GetPurchaseHistoryList(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_GetPurchaseHistoryListOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_GetPurchaseHistoryListOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_GetPurchaseHistoryList(&pstOutParam->ulNumOfPurchase, pstOutParam->ppPurchaseList);

	return hErr;
}

// eNaCtrl_GetProviderName,
static HERROR _naCtrl_GetProviderName(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_GetProviderNameOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_GetProviderNameOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_GetProviderName(&pstOutParam->ulSize, &pstOutParam->pucProviderName);

	return hErr;
}

// eNaCtrl_ReleaseProductList,
static HERROR _naCtrl_ReleaseProductList(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_ReleaseProductListInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_ReleaseProductListInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_ReleaseProductList(pstInParam->pPackageList);

	return hErr;
}

// eNaCtrl_ReleasePurchaseHistoryList,
static HERROR _naCtrl_ReleasePurchaseHistoryList(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_ReleasePurchaseHistoryListInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_ReleasePurchaseHistoryListInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_ReleasePurchaseHistoryList(pstInParam->pPurchaseList);

	return hErr;
}

// eNaCtrl_ReleaseProviderName,
static HERROR _naCtrl_ReleaseProviderName(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_ReleaseProviderNameInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_ReleaseProviderNameInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_ReleaseProviderName(pstInParam->pProviderName);

	return hErr;
}
#endif

#if	defined(CONFIG_MW_CAS_NAGRA_OP_RCS)
// eNaCtrl_PPV_SetEpgMode,
static HERROR _naCtrl_PPV_SetEpgMode(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_PPV_SetEpgModeInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_PPV_SetEpgModeInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_PPV_SetEpgMode(pstInParam->xEpgMode);

	return hErr;
}

// eNaCtrl_PPV_ForceCreatEventInfo,
static HERROR _naCtrl_PPV_ForceCreatEventInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_PPV_ForceCreatEventInfoInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_PPV_ForceCreatEventInfoInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_PPV_ForceCreatEventInfo(pstInParam->svcHandle);

	return hErr;
}

// eNaCtrl_PPV_GetPpvList,
static HERROR _naCtrl_PPV_GetPpvList(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_PPV_GetPpvListOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_PPV_GetPpvListOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_PPV_GetPpvList(&pstOutParam->xStatus, &pstOutParam->xNumberOfItems, pstOutParam->ppxEventPurchaseListArray);

	return hErr;
}

// eNaCtrl_PPV_GetCredit,
static HERROR _naCtrl_PPV_GetCredit(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_PPV_GetCreditOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_PPV_GetCreditOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_PPV_GetCredit(&pstOutParam->xStatus, &pstOutParam->xNumberOfItems, pstOutParam->ppxCreditArray);

	return hErr;
}

// eNaCtrl_PPV_GetPurchaseEvtInfo,
static HERROR _naCtrl_PPV_GetPurchaseEvtInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_PPV_GetPurchaseEvtInfoInParam_t	*pstInParam;
	NA_PPV_GetPurchaseEvtInfoOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_PPV_GetPurchaseEvtInfoInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_PPV_GetPurchaseEvtInfoOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_PPV_GetPurchaseEvtInfo(pstInParam->svcHandle, pstInParam->currTime, &pstOutParam->nEventId);

	return hErr;
}

// eNaCtrl_PPV_RequestEventInfo,
static HERROR _naCtrl_PPV_RequestEventInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_PPV_RequestEventInfoInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_PPV_RequestEventInfoInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_PPV_RequestEventInfo(pstInParam->svcHandle, pstInParam->nEvtId);

	return hErr;
}

// eNaCtrl_PPV_RequestEventInfoByEpg,
static HERROR _naCtrl_PPV_RequestEventInfoByEpg(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_PPV_RequestEventInfoByEpgInParam_t	*pstInParam;
	NA_PPV_RequestEventInfoByEpgOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_PPV_RequestEventInfoByEpgInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_PPV_RequestEventInfoByEpgOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_PPV_RequestEventInfoByEpg(pstInParam->svcHandle, pstInParam->nEvtId, &pstOutParam->xStatus, pstOutParam->pRequest, pstOutParam->pEventInfo, pstOutParam->pProduct);

	return hErr;
}

// eNaCtrl_PPV_RequestPurchaseEvent,
static HERROR _naCtrl_PPV_RequestPurchaseEvent(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_PPV_RequestPurchaseEventInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_PPV_RequestPurchaseEventInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_PPV_RequestPurchaseEvent(pstInParam->xType, pstInParam->pRequest, pstInParam->pProduct);

	return hErr;
}

// eNaCtrl_PPV_RequestPurchaseEventByEpg,
static HERROR _naCtrl_PPV_RequestPurchaseEventByEpg(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_PPV_RequestPurchaseEventByEpgInParam_t	*pstInParam;
	NA_PPV_RequestPurchaseEventByEpgOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_PPV_RequestPurchaseEventByEpgInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_PPV_RequestPurchaseEventByEpgOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_PPV_RequestPurchaseEventByEpg(pstInParam->xType, pstInParam->pRequest, pstInParam->pProduct, &pstOutParam->eProStatus);

	return hErr;
}

// eNaCtrl_PPV_RegisterCreditCallback,
static HERROR _naCtrl_PPV_RegisterCreditCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_PPV_RegisterCreditCallbackInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_PPV_RegisterCreditCallbackInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_PPV_RegisterCreditCallback(pstInParam->callback);

	return hErr;
}

// eNaCtrl_PPV_RegisterPpvListCallback,
static HERROR _naCtrl_PPV_RegisterPpvListCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_PPV_RegisterPpvListCallbackInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_PPV_RegisterPpvListCallbackInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_PPV_RegisterPpvListCallback(pstInParam->callback);

	return hErr;
}

// eNaCtrl_PPV_RegisterRechargeNotifyCallback,
static HERROR _naCtrl_PPV_RegisterRechargeNotifyCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_PPV_RegisterRechargeNotifyCallbackInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_PPV_RegisterRechargeNotifyCallbackInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_PPV_RegisterRechargeNotifyCallback(pstInParam->callback);

	return hErr;
}

// eNaCtrl_PPV_RegisterRequestPurchaseEventNotifyCallback,
static HERROR _naCtrl_PPV_RegisterRequestPurchaseEventNotifyCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_PPV_RegisterRequestPurchaseEventNotifyCallbackInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_PPV_RegisterRequestPurchaseEventNotifyCallbackInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_PPV_RegisterRequestPurchaseEventNotifyCallback(pstInParam->callback);

	return hErr;
}

// eNaCtrl_PPV_RegisterRequestEventNotifyCallback,
static HERROR _naCtrl_PPV_RegisterRequestEventNotifyCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_PPV_RegisterRequestEventNotifyCallbackInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_PPV_RegisterRequestEventNotifyCallbackInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_PPV_RegisterRequestEventNotifyCallback(pstInParam->callback);

	return hErr;
}

// eNaCtrl_PPV_RegisterPurchaseHistoryNotifyCallback,
static HERROR _naCtrl_PPV_RegisterPurchaseHistoryNotifyCallback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_PPV_RegisterPurchaseHistoryNotifyCallbackInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_PPV_RegisterPurchaseHistoryNotifyCallbackInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_PPV_RegisterPurchaseHistoryNotifyCallback(pstInParam->callback);

	return hErr;
}
#endif

#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
// eNaCtrl_GetSlotIDbySessionID,
static HERROR _naCtrl_GetSlotIDbySessionID(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_GetSlotIDbySessionIDInParam_t	*pstInParam;
	NA_GetSlotIDbySessionIDOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_GetSlotIDbySessionIDInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_GetSlotIDbySessionIDOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_GetSlotIDbySessionID(pstInParam->iccSessionId, &pstOutParam->ulSlotId);

	return hErr;
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_TIVU)
// eNaCtrl_CheckTivuService,
static HERROR _naCtrl_CheckTivuService(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	NA_CheckTivuServiceOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_CheckTivuServiceOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam->bStatus = MW_CA_NA_CheckTivuService();

	return ERR_OK;
}

// eNaCtrl_SetTivuService,
static HERROR _naCtrl_SetTivuService(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_SetTivuServiceInParam_t	*pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_SetTivuServiceInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = MW_CA_NA_SetTivuService(pstInParam->bTivu);

	return hErr;
}

// eNaCtrl_CheckTivuCardSystemId,
static HERROR _naCtrl_CheckTivuCardSystemId(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	NA_CheckTivuCardSystemIdOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_CheckTivuCardSystemIdOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam->bStatus = MW_CA_NA_CheckTivuCardSystemId();

	return ERR_OK;
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_PRM)
static HERROR _naCtrl_dvlSystemInformationRequest(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	TDvlStatus eStatus;
	NA_dvlSystemInformationRequestOutParam_t *pstOutParam;

	HxLOG_Print("[%s] Enter function\n", __FUNCTION__);

	if (ulControlType >= eNaCtrl_MAX)
	{
		HxLOG_Error("[%s][%d] ulControlType : [%d]\n",  __FUNCTION__, __LINE__, ulControlType);
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	pstOutParam = (NA_dvlSystemInformationRequestOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	eStatus = CA_NA_dvlSystemInformationRequest(pstOutParam->xVersionString, &pstOutParam->xIrdSerialNumberValue, pstOutParam->xIrdSerialNumberString,
												&pstOutParam->xNuidValue, pstOutParam->xNuidString);

	if(eStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("eStatus : [%d]\n", eStatus);
		return (HERROR)eStatus;
	}

	return ERR_OK;
}

static HERROR _naCtrl_dvlChipsetInformationRequest(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	TDvlStatus eStatus;
	NA_dvlChipsetInformationRequestOutParam_t *pstOutParam;

	HxLOG_Print("[%s] Enter function\n", __FUNCTION__);

	if (ulControlType >= eNaCtrl_MAX)
	{
		HxLOG_Error("[%s][%d] ulControlType : [%d]\n",  __FUNCTION__, __LINE__, ulControlType);
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	pstOutParam = (NA_dvlChipsetInformationRequestOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	eStatus = CA_NA_dvlChipsetInformationRequest(pstOutParam->ppxChipsetInformation);

	if(eStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("eStatus : [%d]\n", eStatus);
		return (HERROR)eStatus;
	}

	return ERR_OK;
}

static HERROR _naCtrl_dvlReleaseBuffer(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	TDvlStatus eStatus;
	NA_dvlReleaseBufferInParam_t *pstInParam;

	HxLOG_Print("[%s] Enter function\n", __FUNCTION__);

	if (ulControlType >= eNaCtrl_MAX)
	{
		HxLOG_Error("[%s][%d] ulControlType : [%d]\n",  __FUNCTION__, __LINE__, ulControlType);
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	pstInParam = (NA_dvlReleaseBufferInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	eStatus = CA_NA_dvlReleaseBuffer(pstInParam->pxBuffer);

	if(eStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("eStatus : [%d]\n", eStatus);
		return (HERROR)eStatus;
	}

	return ERR_OK;
}

static HERROR _naCtrl_dvlCredentialsInformationRequest(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	TDvlStatus eStatus;
	NA_dvlCredentialsInformationRequestInParam_t *pstInParam;
	NA_dvlCredentialsInformationRequestOutParam_t *pstOutParam;

	HxLOG_Print("[%s] Enter function\n", __FUNCTION__);

	if (ulControlType >= eNaCtrl_MAX)
	{
		HxLOG_Error("[%s][%d] ulControlType : [%d]\n",  __FUNCTION__, __LINE__, ulControlType);
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	pstInParam = (NA_dvlCredentialsInformationRequestInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	pstOutParam = (NA_dvlCredentialsInformationRequestOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	eStatus = CA_NA_dvlCredentialsInformationRequest( pstInParam->xCredentialsSize, pstInParam->pxCredentials,
											pstOutParam->pxCredentialsType, pstOutParam->ppxCredentialsUnion,
											pstOutParam->pxGenericMetadataSize, pstOutParam->ppxGenericMetadata,
											pstOutParam->pxSpecificMetadataSize, pstOutParam->ppxSpecificMetadata);

	if(eStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("eStatus : [%d]\n", eStatus);
		return (HERROR)eStatus;
	}

	return ERR_OK;
}


static HERROR _naCtrl_dvlStartPlayback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	TDvlStatus eStatus;
	NA_dvlStartPlaybackInParam_t *pstInParam;
	NA_dvlStartPlaybackOutParam_t *pstOutParam;

	HxLOG_Print("[%s] Enter function\n", __FUNCTION__);

	if (ulControlType >= eNaCtrl_MAX)
	{
		HxLOG_Error("[%s][%d] ulControlType : [%d]\n",  __FUNCTION__, __LINE__, ulControlType);
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	pstInParam = (NA_dvlStartPlaybackInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	pstOutParam = (NA_dvlStartPlaybackOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	eStatus = CA_NA_dvlStartPlayback(pstInParam->xTSid, pstInParam->xInputCredentialsSize, pstInParam->pxInputCredentials,
								pstOutParam->xPlaybackErrorCB, pstOutParam->pxOutputCredentialsSize, pstOutParam->ppxOutputCredentials,
								pstOutParam->pxPlaybackStatus,pstOutParam->pxGenericMetadataSize, pstOutParam->ppxGenericMetadata,
								pstOutParam->pxSpecificMetadataSize, pstOutParam->ppxSpecificMetadata, pstOutParam->ppxPlaybackHandle);

	if(eStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("eStatus : [%d]\n", eStatus);
		return (HERROR)eStatus;
	}

	return ERR_OK;
}

static HERROR _naCtrl_dvlStopPlayback(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	TDvlStatus eStatus;
	NA_dvlStopPlaybackInParam_t *pstInParam;

	HxLOG_Print("[%s] Enter function\n", __FUNCTION__);

	if (ulControlType >= eNaCtrl_MAX)
	{
		HxLOG_Error("[%s][%d] ulControlType : [%d]\n",  __FUNCTION__, __LINE__, ulControlType);
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	pstInParam = (NA_dvlStopPlaybackInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	eStatus = CA_NA_dvlStopPlayback(pstInParam->pxPlaybackHandle);

	if(eStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("eStatus : [%d]\n", eStatus);
		return (HERROR)eStatus;
	}

	return ERR_OK;
}

#if defined(CONFIG_MW_CAS_NAGRA_PRM_3_1)
static HERROR _naCtrl_dvlPlaybackOpenSession(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	TDvlStatus eStatus;
	NA_dvlPlaybackOpenSessionInParam_t *pstInParam;
	NA_dvlPlaybackOpenSessionOutParam_t *pstOutParam;

	HxLOG_Print("[%s] Enter function\n", __FUNCTION__);

	if (ulControlType >= eNaCtrl_MAX)
	{
		HxLOG_Error("[%s][%d] ulControlType : [%d]\n",  __FUNCTION__, __LINE__, ulControlType);
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	pstInParam = (NA_dvlPlaybackOpenSessionInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	pstOutParam = (NA_dvlPlaybackOpenSessionOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	eStatus = CA_NA_dvlPlaybackOpenSession(pstInParam->xTSid, pstOutParam->ppxPlaybackHandle);

	if(eStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("eStatus : [%d]\n", eStatus);
		return (HERROR)eStatus;
	}

	return ERR_OK;
}


static HERROR _naCtrl_dvlPlaybackSetEntitlements(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	TDvlStatus eStatus;
	NA_dvlPlaybackSetEntitlementsInParam_t *pstInParam;
	NA_dvlPlaybackSetEntitlementsOutParam_t *pstOutParam;

	HxLOG_Print("[%s] Enter function\n", __FUNCTION__);

	if (ulControlType >= eNaCtrl_MAX)
	{
		HxLOG_Error("[%s][%d] ulControlType : [%d]\n",  __FUNCTION__, __LINE__, ulControlType);
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	pstInParam = (NA_dvlPlaybackSetEntitlementsInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	pstOutParam = (NA_dvlPlaybackSetEntitlementsOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	eStatus = CA_NA_dvlPlaybackSetEntitlements( pstInParam->pxPlaybackHandle, pstInParam->pxEntitlementsParameters,
											pstOutParam->pxPlaybackStatus, pstOutParam->pxOutputCredentialsSize,
											pstOutParam->ppxOutputCredentials, pstOutParam->pxKeyIdSize, pstOutParam->ppxKeyId);

	if(eStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("eStatus : [%d]\n", eStatus);
		return (HERROR)eStatus;
	}

	return ERR_OK;
}

static HERROR _naCtrl_dvlPlaybackCloseSession(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	TDvlStatus eStatus;
	NA_dvlPlaybackCloseSessionInParam_t *pstInParam;

	HxLOG_Print("[%s] Enter function\n", __FUNCTION__);

	if (ulControlType >= eNaCtrl_MAX)
	{
		HxLOG_Error("[%s][%d] ulControlType : [%d]\n",  __FUNCTION__, __LINE__, ulControlType);
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	pstInParam = (NA_dvlPlaybackCloseSessionInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	eStatus = CA_NA_dvlPlaybackCloseSession(pstInParam->pxPlaybackHandle);

	if(eStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("eStatus : [%d]\n", eStatus);
		return (HERROR)eStatus;
	}

	return ERR_OK;
}

static HERROR _naCtrl_dvlPostPrmManagementMessage(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	TDvlStatus eStatus;
	NA_dvlPostPrmManagementMessageInParam_t *pstInParam;

	HxLOG_Print("[%s] Enter function\n", __FUNCTION__);

	if (ulControlType >= eNaCtrl_MAX)
	{
		HxLOG_Error("[%s][%d] ulControlType : [%d]\n",  __FUNCTION__, __LINE__, ulControlType);
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	pstInParam = (NA_dvlPostPrmManagementMessageInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	eStatus = CA_NA_dvlPostPrmManagementMessage(pstInParam->xPrmManagementMessageSize, pstInParam->pxPrmManagementMessage);

	if(eStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("eStatus : [%d]\n", eStatus);
		return (HERROR)eStatus;
	}

	return ERR_OK;
}

static HERROR _naCtrl_dvlGetAuthenticatedMessage(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	TDvlStatus eStatus;
	NA_dvlGetAuthenticatedMessageInParam_t *pstInParam;
	NA_dvlGetAuthenticatedMessageOutParam_t *pstOutParam;

	HxLOG_Print("[%s] Enter function\n", __FUNCTION__);

	if (ulControlType >= eNaCtrl_MAX)
	{
		HxLOG_Error("[%s][%d] ulControlType : [%d]\n",  __FUNCTION__, __LINE__, ulControlType);
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	pstInParam = (NA_dvlGetAuthenticatedMessageInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	pstOutParam = (NA_dvlGetAuthenticatedMessageOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	eStatus = CA_NA_dvlGetAuthenticatedMessage( pstInParam->xCipheredMessageSize, pstInParam->pxCipheredMessage,
											pstOutParam->pxAuthenticatedDataSize, pstOutParam->ppxAuthenticatedData);

	if(eStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("eStatus : [%d]\n", eStatus);
		return (HERROR)eStatus;
	}

	return ERR_OK;
}


static HERROR _naCtrl_dvlOpaqueDataMessageCreate(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	TDvlStatus eStatus;
	NA_dvlOpaqueDataMessageCreateOutParam_t *pstOutParam;

	HxLOG_Print("[%s] Enter function\n", __FUNCTION__);

	if (ulControlType >= eNaCtrl_MAX)
	{
		HxLOG_Error("[%s][%d] ulControlType : [%d]\n",  __FUNCTION__, __LINE__, ulControlType);
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	pstOutParam = (NA_dvlOpaqueDataMessageCreateOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	eStatus = CA_NA_dvlOpaqueDataMessageCreate( pstOutParam->ppxOpaqueDataMessageHandle);

	if(eStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("eStatus : [%d]\n", eStatus);
		return (HERROR)eStatus;
	}

	return ERR_OK;
}

static HERROR _naCtrl_dvlOpaqueDataMessageRelease(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	TDvlStatus eStatus;
	NA_dvlOpaqueDataMessageReleaseInParam_t *pstInParam;

	HxLOG_Print("[%s] Enter function\n", __FUNCTION__);

	if (ulControlType >= eNaCtrl_MAX)
	{
		HxLOG_Error("[%s][%d] ulControlType : [%d]\n",  __FUNCTION__, __LINE__, ulControlType);
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	pstInParam = (NA_dvlOpaqueDataMessageReleaseInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	eStatus = CA_NA_dvlOpaqueDataMessageRelease(pstInParam->pxOpaqueDataMessageHandle);

	if(eStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("eStatus : [%d]\n", eStatus);
		return (HERROR)eStatus;
	}

	return ERR_OK;
}

static HERROR _naCtrl_dvlOpaqueDataMessageSetApplicationData(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	TDvlStatus eStatus;
	NA_dvlOpaqueDataMessageSetApplicationDataInParam_t *pstInParam;

	HxLOG_Print("[%s] Enter function\n", __FUNCTION__);

	if (ulControlType >= eNaCtrl_MAX)
	{
		HxLOG_Error("[%s][%d] ulControlType : [%d]\n",  __FUNCTION__, __LINE__, ulControlType);
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	pstInParam = (NA_dvlOpaqueDataMessageSetApplicationDataInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	eStatus = CA_NA_dvlOpaqueDataMessageSetApplicationData(pstInParam->pxOpaqueDataMessageHandle, pstInParam->xApplicationDataSize, pstInParam->pxApplicationData);

	if(eStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("eStatus : [%d]\n", eStatus);
		return (HERROR)eStatus;
	}

	return ERR_OK;
}

static HERROR _naCtrl_dvlOpaqueDataMessageSetPrmSignalization(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	TDvlStatus eStatus;
	NA_dvlOpaqueDataMessageSetPrmSignalizationInParam_t *pstInParam;

	HxLOG_Print("[%s] Enter function\n", __FUNCTION__);

	if (ulControlType >= eNaCtrl_MAX)
	{
		HxLOG_Error("[%s][%d] ulControlType : [%d]\n",  __FUNCTION__, __LINE__, ulControlType);
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	pstInParam = (NA_dvlOpaqueDataMessageSetPrmSignalizationInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	eStatus = CA_NA_dvlOpaqueDataMessageSetPrmSignalization(pstInParam->pxOpaqueDataMessageHandle, pstInParam->xPrmSignalizationSize, pstInParam->pxPrmSignalization);

	if(eStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("eStatus : [%d]\n", eStatus);
		return (HERROR)eStatus;
	}

	return ERR_OK;
}

static HERROR _naCtrl_dvlOpaqueDataMessageGet(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	TDvlStatus eStatus;
	NA_dvlOpaqueDataMessageGetInParam_t *pstInParam;
	NA_dvlOpaqueDataMessageGetOutParam_t *pstOutParam;

	HxLOG_Print("[%s] Enter function\n", __FUNCTION__);

	if (ulControlType >= eNaCtrl_MAX)
	{
		HxLOG_Error("[%s][%d] ulControlType : [%d]\n",  __FUNCTION__, __LINE__, ulControlType);
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	pstInParam = (NA_dvlOpaqueDataMessageGetInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	pstOutParam = (NA_dvlOpaqueDataMessageGetOutParam_t *)local_getOutParam(pstParam);
	if (pstInParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	eStatus = CA_NA_dvlOpaqueDataMessageGet(pstInParam->pxOpaqueDataMessageHandle, pstOutParam->pxOpaqueDataMessageSize, pstOutParam->ppxOpaqueDataMessage);

	if(eStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("eStatus : [%d]\n", eStatus);
		return (HERROR)eStatus;
	}

	return ERR_OK;
}

static HERROR _naCtrl_dvlUpdateSpecificLcmMetadata(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	TDvlStatus eStatus;
	NA_dvlUpdateSpecificLcmMetadataInParam_t *pstInParam;
	NA_dvlUpdateSpecificLcmMetadataOutParam_t *pstOutParam;

	HxLOG_Print("[%s] Enter function\n", __FUNCTION__);

	if (ulControlType >= eNaCtrl_MAX)
	{
		HxLOG_Error("[%s][%d] ulControlType : [%d]\n",  __FUNCTION__, __LINE__, ulControlType);
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	pstInParam = (NA_dvlUpdateSpecificLcmMetadataInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	pstOutParam = (NA_dvlUpdateSpecificLcmMetadataOutParam_t *)local_getOutParam(pstParam);
	if (pstInParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	eStatus = CA_NA_dvlUpdateSpecificLcmMetadata(pstInParam->xInLcmCredentialsSize, pstInParam->pxInLcmCredentials, pstInParam->xSpecificMetadataSize, pstInParam->pxSpecificMetadata,
							pstOutParam->pxOutLcmCredentialsSize, pstOutParam->ppxOutLcmCredentials);

	if(eStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("eStatus : [%d]\n", eStatus);
		return (HERROR)eStatus;
	}

	return ERR_OK;
}

static HERROR _naCtrl_AddTransportSessionIdDirect(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR hErr;
	NA_AddTransportSessionIdForPrmInParam_t *pstInParam;

	HxLOG_Print("[%s] Enter function\n", __FUNCTION__);

	if (ulControlType >= eNaCtrl_MAX)
	{
		HxLOG_Error("[%s][%d] ulControlType : [%d]\n",  __FUNCTION__, __LINE__, ulControlType);
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	pstInParam = (NA_AddTransportSessionIdForPrmInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	hErr = caSessionMgr_AddTransportSessionIdDirect(pstInParam->xTSid);

	if(hErr != ERR_OK)
	{
		HxLOG_Error("hErr : [%d]\n", hErr);
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR _naCtrl_RemoveTransportSessionIdDirect(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR hErr;
	NA_RemoveTransportSessionIdForPrmInParam_t *pstInParam;

	HxLOG_Print("[%s] Enter function\n", __FUNCTION__);

	if (ulControlType >= eNaCtrl_MAX)
	{
		HxLOG_Error("[%s][%d] ulControlType : [%d]\n",  __FUNCTION__, __LINE__, ulControlType);
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	pstInParam = (NA_RemoveTransportSessionIdForPrmInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	hErr = caSessionMgr_RemoveTransportSessionIdDirect(pstInParam->xTSid);

	if(hErr != ERR_OK)
	{
		HxLOG_Error("hErr : [%d]\n", hErr);
		return ERR_FAIL;
	}

	return ERR_OK;
}

#endif

#if defined(CONFIG_MW_CAS_NAGRA_PRM_VOD_PVR)
static HERROR _naCtrl_dvlCreateRecordSessionParameters(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	TDvlStatus eStatus;
	NA_dvlCreateRecordSessionParametersOutParam_t *pstOutParam;

	HxLOG_Print("[%s] Enter function\n", __FUNCTION__);

	if (ulControlType >= eNaCtrl_MAX)
	{
		HxLOG_Error("[%s][%d] ulControlType : [%d]\n",  __FUNCTION__, __LINE__, ulControlType);
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	pstOutParam = (NA_dvlCreateRecordSessionParametersOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	eStatus = CA_NA_dvlCreateRecordSessionParameters(pstOutParam->ppxRecordParameters);

	if(eStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("eStatus : [%d]\n", eStatus);
		return (HERROR)eStatus;
	}

	return ERR_OK;
}

static HERROR _naCtrl_dvlStartRecordEx(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	TDvlStatus eStatus;
	NA_dvlCreateRecordSessionParametersInParam_t *pstInParam;
	NA_dvlStartRecordExOutParam_t *pstOutParam;

	HxLOG_Print("[%s] Enter function\n", __FUNCTION__);

	if (ulControlType >= eNaCtrl_MAX)
	{
		HxLOG_Error("[%s][%d] ulControlType : [%d]\n",  __FUNCTION__, __LINE__, ulControlType);
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	pstInParam = (NA_dvlCreateRecordSessionParametersInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	pstOutParam = (NA_dvlStartRecordExOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	eStatus = CA_NA_dvlStartRecordEx( 	pstInParam->pxRecordParameters,
								pstOutParam->pxOutputCredentialsSize, pstOutParam->ppxOutputCredentials,
								pstOutParam->pxRecordingStatus, pstOutParam->ppxRecordHandle);

	if(eStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("eStatus : [%d]\n", eStatus);
		return (HERROR)eStatus;
	}

	return ERR_OK;
}

static HERROR _naCtrl_dvlStartRecord(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	TDvlStatus eStatus;
	// 이걸 사용할지에대해 확인해 보자
	return ERR_OK;
}

static HERROR _naCtrl_dvlStopRecord(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	TDvlStatus eStatus;
	NA_dvlStopRecordInParam_t *pstInParam;

	HxLOG_Print("[%s] Enter function\n", __FUNCTION__);

	if (ulControlType >= eNaCtrl_MAX)
	{
		HxLOG_Error("[%s][%d] ulControlType : [%d]\n",  __FUNCTION__, __LINE__, ulControlType);
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	pstInParam = (NA_dvlStopRecordInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		HxLOG_Error("patameter is NULL\n");
		return ERR_FAIL;
	}

	eStatus = CA_NA_dvlStopRecord(pstInParam->pxRecordHandle);

	if(eStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("eStatus : [%d]\n", eStatus);
		return (HERROR)eStatus;
	}

	return ERR_OK;
}

#endif

#endif

#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
// eNaCtrl_SetParentalControl
static HERROR _naCtrl_SetParentalControl(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	NA_SetParentalControlInParam_t *pstInParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_SetParentalControlInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	NAMGR_SetParentalControl(pstInParam->xTsId, pstInParam->bUserPinPass);
	return ERR_OK;
}

// eNaCtrl_SetParentalControlLevel
static HERROR _naCtrl_SetParentalControlLevel(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	NAMGR_SetParentalControlLevel();
	return ERR_OK;
}
#endif

static HERROR _naCtrl_FactoryDefault(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR hErr;

	HxLOG_Print("[%s] Enter function\n", __FUNCTION__);

	if (ulControlType >= eNaCtrl_MAX)
	{
		HxLOG_Error("[%s][%d] ulControlType : [%d]\n",  __FUNCTION__, __LINE__, ulControlType);
		return ERR_FAIL;
	}

	(void) pstParam ;

	hErr = NAMGR_FactoryDefault();

	if(hErr != ERR_OK)
	{
		HxLOG_Error("[%s][%d] Error in NAMGR_FactoryDefault() \n",  __FUNCTION__, __LINE__);
	}

	return ERR_OK;
}

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
static HERROR _naCtrl_GetStandardErrorCode(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_GetStandardErrorCodeInParam_t	*pstInParam;
	NA_GetStandardErrorCodeOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_GetStandardErrorCodeInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_GetStandardErrorCodeOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	
	hErr = MW_CA_NA_GetAccessErrorCode(pstInParam->ulSessionId, &pstOutParam->ulStandardErrorCode);

	return hErr;
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_ZONEID)
static HERROR _naCtrl_GetZoneId(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_GetZoneIdInParam_t	*pstInParam;
	NA_GetZoneIdOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_GetStandardErrorCodeInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_GetStandardErrorCodeOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	
	hErr = MW_CA_NA_GetZoneId(pstOutParam->pZoneId, pstInParam->ulMaxLen);

	return hErr;
}

static HERROR _naCtrl_GetCountryCode(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	NA_GetCountryCodeInParam_t	*pstInParam;
	NA_GetCountryCodeOutParam_t	*pstOutParam;

	if (ulControlType >= eNaCtrl_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (NA_GetStandardErrorCodeInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (NA_GetStandardErrorCodeOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	
	hErr = MW_CA_NA_GetCountryCode(pstOutParam->pCountryCode, pstInParam->ulMaxLen);

	return hErr;
}

#endif


