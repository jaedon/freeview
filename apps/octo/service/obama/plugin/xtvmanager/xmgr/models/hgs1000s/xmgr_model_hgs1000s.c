/**
	@file     file_name.c File Name을 적습니다.
	@brief    file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2012 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <isosvc.h>
#include <svc_si.h>
#include <svc_sys.h>
#include <bus.h>
#include <mgr_common.h>

#include <mgr_swup.h>
#include <mgr_pvr_action.h>
#include <mgr_live.h>
#include <mgr_search.h>
#include <mgr_swup.h>
#include <mgr_satip.h>

#include <papi.h>

#include <xsvc_si.h>
#include <xmgr_live.h>
#include <xmgr_swup.h>
#include <xmgr_module_portingprotocol.h>

#include "../local_include/_xmgr_model.h"

#define ___LOCAL_FUNCTIONS___
/*
bNormal : Sometimes Action Manager State is not updated.
TRUE : eAmState_NORMAL
FALSE : eAmState_ACTIVE_STANDBY or not sure
*/
STATIC void xmgr_hgs1000s_UpdateFrontpanel(HBOOL bNormal)
{
	if (MGR_ACTION_GetSATIPCount() > 0)
	{
		SVC_SYS_SetPowerLedState(ePOWER_LED_WATCHING);
	}
	else if (MGR_ACTION_GetRecordCount() > 0)
	{
		SVC_SYS_SetPowerLedState(ePOWER_LED_RECORDING);
	}
	else if (bNormal || MGR_ACTION_GetState() != eAmState_ACTIVE_STANDBY)
	{
		SVC_SYS_SetPowerLedState(ePOWER_LED_OFF);
	}
	else
	{
		SVC_SYS_SetPowerLedState(ePOWER_LED_STANDBY);
	}
}

STATIC void xmgr_hgs1000s_OperationFrontControl(void)
{
	PanelDimmingLevel_t eDimLevel = ePANEL_DIM_Level100;
	HBOOL				bPowerSavingStandby;
	HUINT32				ulActionId = MGR_ACTION_GetMainActionId();
	HERROR				hErr = ERR_FAIL;
	DbSvc_Info_t		svcInfo;

	HxLOG_Trace();
	SVC_SYS_SetPanelTimeDisplay(FALSE);

	/*Channel name display*/
	hErr = MGR_LIVE_GetSvcInfo (ulActionId, eLiveViewPlaySvc_MASTER, &svcInfo);
	if(hErr == ERR_OK)
	{
#if defined (CONFIG_MW_SYS_7SEG)
#else
		SVC_SYS_DisplayPanelString (ePANEL_STR_PRIORITY_VIEW_ACTION, DbSvc_GetName(&svcInfo), eDxPANEL_ALIGN_LEFT);
#endif
		switch(DbSvc_GetSvcType(&svcInfo))
		{
			case eDxSVC_TYPE_TV :				
				SVC_SYS_SetLedOnOff(eDxLED_ID_TV, TRUE);
				SVC_SYS_SetLedOnOff(eDxLED_ID_RADIO, FALSE);
				break;

			case eDxSVC_TYPE_RADIO :
				SVC_SYS_SetLedOnOff(eDxLED_ID_TV, FALSE);
				SVC_SYS_SetLedOnOff(eDxLED_ID_RADIO, TRUE);
				break;

			default :
				HxLOG_Print("\t=> Unknown service type : %d\n", DbSvc_GetSvcType(&svcInfo));
				break;
		}
	}

	SVC_SYS_SetPanelDimLevel(eDimLevel);	//write nvram
	SVC_SYS_SetPanelDim(eDimLevel);			//front dim level setting
#if defined(CONFIG_MW_SYS_VFD)
	SVC_SYS_SetLedVfdDimmLevel(eDimLevel);
#else
	SVC_SYS_SetLedDimmLevel(eDimLevel);	//led dim level setting
#endif

	SVC_SYS_SetColdBootPanelCondition (TRUE, TRUE);

	if(PAPI_GetPowerSavingOnStandby(&bPowerSavingStandby) != ERR_OK)
	{
		bPowerSavingStandby = TRUE;
	}
	if(bPowerSavingStandby == TRUE)
	{
		SVC_SYS_SetPanelPower(TRUE);
	}
#if defined(CONFIG_MW_SYS_ACTIVESTBY_BY_PWRSAVINGONSTBY)
	SVC_SYS_SetPanelTimeDisplay(TRUE); // Active Standby 에서는 무조건 Time Display가 동작 하도록 한다.
#endif

#if defined(CONFIG_MW_MM_PVR)

#if defined(CONFIG_PROD_HGS1000S)
#elif defined(CONFIG_MW_SYS_LED_DIMMING_CTRL_ON_STBY)
	if(MGR_ACTION_GetRecordCount()>0)
	{
		SVC_SYS_SetPowerLedState(ePOWER_LED_RECORDING);
	}
	else
	{
		SVC_SYS_SetPowerLedState(ePOWER_LED_WATCHING);
	}
#else
	if(MGR_ACTION_GetRecordCount()>0)
	{
		SVC_SYS_SetLedOnOff(eDxLED_ID_REC, TRUE);
	}
#if (defined(CONFIG_PROD_ICORDPRO) || defined(CONFIG_PROD_HDNEO)) && defined(CONFIG_MW_SYS_COLOR_LED)
	else
	{
		SVC_SYS_SetPowerLedState(ePOWER_LED_WATCHING);
	}
#else
	SVC_SYS_SetPowerLedState(ePOWER_LED_WATCHING);
#endif
#endif

#else
	SVC_SYS_SetPowerLedState(ePOWER_LED_WATCHING);
#endif
}

STATIC void xmgr_hgs1000s_ShutDownFrontControl(void)
{
	HBOOL				bPowerSavingStandby;
	PanelDimmingLevel_t	eDimLevel = ePANEL_DIM_Level40;
	HUINT16				ulLedCount =0;

	HxLOG_Trace();
	SVC_SYS_ClearAllPanelString();

	SVC_SYS_SetPanelDimLevel(eDimLevel);		//write nvram
	SVC_SYS_SetPanelDim(eDimLevel);
#if defined(CONFIG_MW_SYS_VFD)
	SVC_SYS_SetLedVfdDimmLevel(eDimLevel);
#else
	SVC_SYS_SetLedDimmLevel(eDimLevel);
#endif

	if(PAPI_GetPowerSavingOnStandby(&bPowerSavingStandby) != ERR_OK)
	{
		bPowerSavingStandby = TRUE;
	}

#if defined(CONFIG_MW_SYS_ACTIVESTBY_BY_PWRSAVINGONSTBY)
	SVC_SYS_SetPanelTimeDisplay(TRUE); // Active Standby 에서는 무조건 Time Display가 동작 하도록 한다.
#else
	SVC_SYS_SetPanelTimeDisplay((bPowerSavingStandby) ? FALSE : TRUE);
#endif	

	// Reservation, Recording Icon 제외하고 Off한다.
	for(ulLedCount=0; ulLedCount< NUM_MAX_DxLED; ulLedCount++)
	{
		if(eDxLED_ID_REC != ulLedCount && eDxLED_ID_RESERVE!=ulLedCount)
		{
			SVC_SYS_SetLedOnOff(ulLedCount, FALSE);
		}
	}
#if defined(CONFIG_PROD_HGS1000S)
#elif defined(CONFIG_MW_SYS_LED_DIMMING_CTRL_ON_STBY)
#if defined(CONFIG_MW_MM_PVR)
	if(MGR_ACTION_GetRecordCount()>0)
	{
		SVC_SYS_SetPowerLedState(ePOWER_LED_RECORDING);
	}
	else
	{
		SVC_SYS_SetPowerLedState(ePOWER_LED_STANDBY);
	}
#endif
#else
	SVC_SYS_SetPowerLedState(ePOWER_LED_STANDBY);
#endif

#if defined(CONFIG_MW_MM_PVR)
	SVC_SYS_SetColdBootPanelCondition (FALSE, FALSE);
#endif
	if(bPowerSavingStandby == TRUE)
	{
		SVC_SYS_SetPanelPower(FALSE);
	}
}


#define _____MESSAGE_PROCESS_FUNCTION_____
STATIC BUS_Result_t xmgr_hgs1000s_msgBusCreate (Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	// Default SI Character Type 을 여기서 세팅해준다
	(void) MWC_UTIL_SetSiStringEncodedType (eSITEXT_ENCODED_300468_EUROPE);

	NOT_USED_PROC_ARGUMENT (hHandle, p1, p2, p3);
	return MESSAGE_BREAK;
}

STATIC BUS_Result_t xmgr_hgs1000s_msgBusGoShutdown (Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HBOOL			bPowerSavingStandby = FALSE;

	if(PAPI_GetPowerSavingOnStandby(&bPowerSavingStandby) != ERR_OK)
	{
		HxLOG_Error("[%s] PAPI_GetPowerSavingOnStandby() Error\n",__FUNCTION__);
	}

	if(bPowerSavingStandby == FALSE)
	{
		(void)PAPI_SetSleepModeOnStandby(ePAMA_POWERSTATE_LIGHT_SLEEP);
	}
	else
	{
		(void)PAPI_SetSleepModeOnStandby(ePAMA_POWERSTATE_DEEP_SLEEP);
	}

	(void)PAPI_SetBootModeOnStandby(ePAMA_BOOTMODE_COLD);

	NOT_USED_PROC_ARGUMENT (hHandle, p1, p2, p3);
	return MESSAGE_PASS;
}

#define _____PROCEDURE_FUNCTION_____
STATIC BUS_Result_t proc_hgs1000s (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			 eResult = MESSAGE_PASS;

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
		eResult = xmgr_hgs1000s_msgBusCreate (hHandle, p1, p2, p3);
		xmgr_hgs1000s_UpdateFrontpanel(FALSE);
		break;
	case eMEVT_BUS_READY_SHUTDOWN :
	case eMEVT_BUS_GO_SHUTDOWN:
	{
		eResult = xmgr_hgs1000s_msgBusGoShutdown (hHandle, p1, p2, p3);
		xmgr_hgs1000s_UpdateFrontpanel(FALSE);
		xmgr_hgs1000s_ShutDownFrontControl();
		break;
	}
	case eMEVT_BUS_GO_OPERATION:
	{
		xmgr_hgs1000s_OperationFrontControl();
		xmgr_hgs1000s_UpdateFrontpanel(TRUE);
		break;
	}
	case eMEVT_PVR_NOTIFY_RECORD_STARTED:
	{
		xmgr_hgs1000s_UpdateFrontpanel(FALSE);
		break;
	}
	case eMEVT_PVR_NOTIFY_RECORD_STOPPED:
	{
		xmgr_hgs1000s_UpdateFrontpanel(FALSE);
		break;
	}
	case eMEVT_SATIP_NOTIFY_STARTED:
	{
		xmgr_hgs1000s_UpdateFrontpanel(FALSE);
		break;
	}
	case eMEVT_SATIP_NOTIFY_START_STOPPED:
	{
		xmgr_hgs1000s_UpdateFrontpanel(FALSE);
		break;
	}
	case eMEVT_ACTION_NOTIFY_TAKEN:
	{
		xmgr_hgs1000s_UpdateFrontpanel(FALSE);
		break;
	}
	case eMEVT_ACTION_NOTIFY_RELEASED:
	{
		xmgr_hgs1000s_UpdateFrontpanel(FALSE);
		break;
	}
	default:
		break;
	}

	return (MESSAGE_BREAK == eResult) ? MESSAGE_BREAK : MESSAGE_PASS;
}

#define ___API_FUNCTIONS___
HERROR	xmgr_model_Init_HGS1000S (void)
{
	BUS_MGR_Create (NULL, APP_PORTING_MGR_PRIORITY, proc_hgs1000s, 0, 0, 0, 0);
	return ERR_OK;
}

#define _____LIVE_SPEC_____
HERROR xmgr_model_GetLiveSpec_HGS1000S (XmgrLive_LiveType_InArg_t *pstIn, XmgrLive_LiveType_t *pstOut)
{
	pstOut->eViewModule	= eLiveModuleType_Base;
	pstOut->eLockModule	= eLiveModuleType_Base;

	return ERR_OK;
}

HERROR xmgr_model_LiveGetSiSpec_HGS1000S (MgrLive_START_t *pstStart, SvcSi_LiveSpec_t *pstSiSpec)
{
	pstSiSpec->eActionKind	= eSVCSI_ACT_KIND_Live;
	pstSiSpec->ulActionSpec	= eSVCSI_ACT_SPEC_Ses;
	pstSiSpec->eVideo		= eSVCSI_ES_SELECT_Normal;
	pstSiSpec->eAudio		= eSVCSI_ES_SELECT_Normal;
	pstSiSpec->eSubtitle	= eSVCSI_ES_SELECT_Normal;
	pstSiSpec->eTeletext	= eSVCSI_ES_SELECT_Normal;
	pstSiSpec->eTimeUpdate	= eSVCSI_TIME_UPDATE_Normal;

	return ERR_OK;
}

#define _____SEARCH_SPEC_____
HERROR xmgr_model_SearchGetSiSpec_HGS1000S (SvcSi_SearchOption_t *pstOption, SvcSi_SearchSpec_t *pstSiSpec)
{
	pstSiSpec->ulActionKind	= eSVCSI_ACT_KIND_Search;
	pstSiSpec->ulActionSpec	= eSVCSI_ACT_SPEC_Ses;

	return ERR_OK;
}

#define _____LCN_POLICY_____
HERROR xmgr_model_SearchGetLcnPolicy_HGS1000S (DefaultLcnPolicy_t *peLcnPolicy, PreProgrammedChNumPolicy_t *pePreProgChNumPolicy)
{
	if ((peLcnPolicy == NULL) && (pePreProgChNumPolicy == NULL))		{ return ERR_FAIL; }

	if (peLcnPolicy != NULL)
	{
		*peLcnPolicy = eDEFAULTLCN_NORMAL;
	}

	if (pePreProgChNumPolicy != NULL)
	{
		*pePreProgChNumPolicy = ePreChNum_NONE;
	}

	return ERR_OK;
}

#define _____EXT_SEARCH_SPEC_____
HERROR xmgr_model_ExtsearchGetSiSpec_HGS1000S (SvcSi_ExtSearchOption_t *pstOption, SvcSi_ExtSearchSpec_t *pstSiSpec)
{
	pstSiSpec->eActionKind	= eSVCSI_ACT_KIND_ExtSearch;
	pstSiSpec->ulActionSpec	= eSVCSI_ACT_SPEC_Ses;

	return ERR_OK;
}

#define _____SOFTWARE_UPDATE_____
HERROR xmgr_model_SwupGetSiSpec_HGS1000S (MgrSwup_InArg_t *pstInArg, SvcSi_SwupCheckSpec_t *pstSiSpec)
{
	pstSiSpec->eActionKind	= eSVCSI_ACT_KIND_SwupCheck;
	pstSiSpec->ulActionSpec	= eSVCSI_ACT_SPEC_Ses;

	return ERR_OK;
}

HERROR	xmgr_model_SwupInitProduct_HGS1000S (void)
{
	MgrSwup_ModuleInfo_t	 stModuleInfo;
	MgrSwup_ModuleCb_t		*pstCallback = &stModuleInfo.stCallback;

#if 0		// not used
	HERROR					 hErr = ERR_FAIL;
#endif

	HLIB_STD_StrUtf8NCpy(stModuleInfo.szModuleName, "HGS1000S_SWUP", SWUPDATE_MODULE_NAME_LEN);

#if defined (CONFIG_OTA_SES)
	pstCallback->pfnCreate			= XMGR_SWUP_Create_Ses;
	pstCallback->pfnEnterAction		= XMGR_SWUP_EnterAction_Ses;
	pstCallback->pfnLeaveAction		= XMGR_SWUP_LeaveAction_Ses;
	pstCallback->pfnGetNextState	= XMGR_SWUP_GetNextState_Ses;
	pstCallback->pfnDestroy			= XMGR_SWUP_Destroy_Ses;
	pstCallback->pfnSetManualInfo	= XMGR_SWUP_SetManualInfo_Ses;
	pstCallback->pfnGetHomeTpInfo	= XMGR_SWUP_GetHomeTpInfo_Ses;
#elif defined (CONFIG_OTA_ASTRA)
	pstCallback->pfnCreate			= XMGR_SWUP_Create_Astra;
	pstCallback->pfnEnterAction 	= XMGR_SWUP_EnterAction_Astra;
	pstCallback->pfnLeaveAction 	= XMGR_SWUP_LeaveAction_Astra;
	pstCallback->pfnGetNextState	= XMGR_SWUP_GetNextState_Astra;
	pstCallback->pfnDestroy 		= XMGR_SWUP_Destroy_Astra;
	pstCallback->pfnSetManualInfo	= XMGR_SWUP_SetManualInfo_Astra;
	pstCallback->pfnGetHomeTpInfo	= XMGR_SWUP_GetHomeTpInfo_Astra;
#endif
	MGR_SWUP_RegisterModuleInfo(&stModuleInfo);

#if 0
	MGR_SWUP_GetSpec(&eSpec);
	hErr = xsvc_si_SetOtaMasterAction(eSpec);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("xmgr_model_InitOtaMasterAction() failed.. : eSpec(%d)   \n", eSpec);
	}
#endif

	return ERR_OK;
}

#define _____PVR_SPEC_____
HERROR xmgr_model_RecordGetSiSpec_HGS1000S (SvcSi_RecSpec_t *pstSiSpec)
{
	pstSiSpec->eActionKind	= eSVCSI_ACT_KIND_Record;
	pstSiSpec->ulActionSpec	= eSVCSI_ACT_SPEC_Ses;

	return ERR_OK;
}

HERROR xmgr_model_PlaybackGetSiSpec_HGS1000S (SvcSi_PbSpec_t *pstSiSpec)
{
	pstSiSpec->eActionKind	= eSVCSI_ACT_KIND_Playback;
	pstSiSpec->ulActionSpec	= eSVCSI_ACT_SPEC_Ses;

	pstSiSpec->eVideo		= eSVCSI_ES_SELECT_Normal;
	pstSiSpec->eAudio		= eSVCSI_ES_SELECT_Normal;
	pstSiSpec->eSubtitle	= eSVCSI_ES_SELECT_Normal;
	pstSiSpec->eTeletext	= eSVCSI_ES_SELECT_Normal;
	pstSiSpec->eTimeUpdate	= eSVCSI_TIME_UPDATE_Normal;

	return ERR_OK;
}

HERROR xmgr_model_MediaplaybackGetSiSpec_HGS1000S (SvcSi_MediaPbSpec_t *pstSiSpec)
{
	pstSiSpec->eActionKind	= eSVCSI_ACT_KIND_MediaPlayback;
	pstSiSpec->ulActionSpec	= eSVCSI_ACT_SPEC_Ses;

	return ERR_OK;
}

HERROR xmgr_model_RecordGetCasTypes_HGS1000S (HUINT32 *pulCasTypes)
{
	if (NULL == pulCasTypes)
		return ERR_FAIL;

	*pulCasTypes	=	eMgrRec_CasType_CiPlus | eMgrRec_CasType_NagraSES;

	return ERR_OK;
}

HERROR xmgr_model_PlaybackGetCasTypes_HGS1000S (HUINT32 *pulCasTypes)
{
	if (NULL == pulCasTypes)
		return ERR_FAIL;

	*pulCasTypes	=	eMgrPb_CasType_CiPlus | eMgrPb_CasType_NagraSES;

	return ERR_OK;
}
#define _____SATIP____
HERROR xmgr_model_SatipGetSiSpec_HGS1000S (SvcSi_SatipSpec_t *pstSiSpec)
{
	pstSiSpec->eActionKind	= eSVCSI_ACT_KIND_Satip;
	pstSiSpec->ulActionSpec	= eSVCSI_ACT_SPEC_Ses;

	return ERR_OK;
}

#define _____SAT_RECORD_____
HERROR xmgr_model_SatrecGetSiSpec_HGS1000S (SvcSi_SatRecordSpec_t *pstSiSpec)
{
	pstSiSpec->eActionKind	= eSVCSI_ACT_KIND_SatRecord;
	pstSiSpec->ulActionSpec	= eSVCSI_ACT_SPEC_Ses;

	return ERR_OK;
}

#define _____TIME_UPDATE_SPEC_____
HERROR xmgr_model_TimeupdateGetSiSpec_HGS1000S (SvcSi_TimeUpdateSpec_t *pstSiSpec)
{
	pstSiSpec->eActionKind	= eSVCSI_ACT_KIND_TimeUpdate;
	pstSiSpec->ulActionSpec	= eSVCSI_ACT_SPEC_Ses;

	return ERR_OK;
}

#define _____MAKE_SECTION_SPEC_____
HERROR xmgr_model_MakesectionGetSiSpec_HGS1000S (SvcSi_MakeSectionSpec_t *pstSiSpec)
{
	pstSiSpec->eActionKind	= eSVCSI_ACT_KIND_None;		// reverved for future use
	pstSiSpec->ulActionSpec	= eSVCSI_ACT_SPEC_EuSat;

	return ERR_OK;
}

HERROR xmgr_model_GetSiEsPriorityTable_HGS1000S (SvcSi_EsPriorityTable_t *pstTable)
{
	return xmgr_model_GetSiEsPriorityTable_Base(pstTable);
}

/* end of file */

