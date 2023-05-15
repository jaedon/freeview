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
#include <bus.h>
#include <mgr_common.h>

#include <mgr_swup.h>
#include <mgr_pvr_action.h>
#include <mgr_live.h>
#include <mgr_search.h>
#include <mgr_swup.h>

#include <xmgr_live.h>
#include <xmgr_swup.h>
#include <xmgr_module_portingprotocol.h>

#include <xsvc_si.h>
#include <svc_cas.h>

#if defined(CONFIG_MW_CAS_IRDETO_UCAS)
// TODO: [JHLEE] IRDETO UC Softcell Multi descramble 이 지원되지 않아서 녹확를 할 수 없다. 나중에 지워야 한다.
#define CONFIG_MW_CAS_IRDETO_UCAS_RECORD_LIMIT
#endif

/*******************************************************************/
/********************      	Static Function 	********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___
STATIC void xmgr_ir4000hd_OperationFrontControl(void)
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

STATIC void xmgr_ir4000hd_ShutDownFrontControl(void)
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

STATIC BUS_Result_t xmgr_ir4000hd_evtCASCmd_IRDETO (Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if defined(CONFIG_MW_CAS_IRDETO_UCAS_RECORD_LIMIT)
	SvcCas_ShowType_e	eCasShowType = eCasShowType_None;

	eCasShowType = (SvcCas_ShowType_e)p3;
	if (eCasShowType == eCasShowType_RecStop)
	{
		BUS_SendMessage (NULL, eMEVT_PVR_RECORD_STOP_TSR, hHandle, 0, 0, 0);
	}
	else
	{
		HxLOG_Error("Error!! It is not eCasShowType_RecStop!! eCasShowType_RecStop:[%d]\n", eCasShowType);
	}
#endif

	NOT_USED_PROC_ARGUMENT (hHandle, p1, p2, p3);

	return MESSAGE_PASS;
}


STATIC BUS_Result_t xmgr_ir4000hd_evtCASCmd (Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t		eResult	= ERR_BUS_NO_ERROR;
	DxCAS_GroupId_e		eCasId = eDxCAS_GROUPID_NONE;
	SvcCas_ShowType_e	eCasShowType = eCasShowType_None;

	eCasId = (DxCAS_GroupId_e)p1;
	switch (eCasId)
	{
		case eDxCAS_GROUPID_IR:
			HxLOG_Debug("[XMGR:IR4000HD(%d)] MESSAGE : eDxCAS_GROUPID_IR\n");
			eResult = xmgr_ir4000hd_evtCASCmd_IRDETO(hHandle, p1, p2, p3);
			break;
		default:
			eResult = MESSAGE_PASS;
			break;
	}

	NOT_USED_PROC_ARGUMENT (hHandle, p1, p2, p3);

	return eResult;
}


#define _____MESSAGE_PROCESS_FUNCTION_____
STATIC BUS_Result_t xmgr_ir4000hd_msgBusCreate (Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	// Default SI Character Type 을 여기서 세팅해준다
	(void) MWC_UTIL_SetSiStringEncodedType (eSITEXT_ENCODED_300468_EUROPE);

	NOT_USED_PROC_ARGUMENT (hHandle, p1, p2, p3);
	return MESSAGE_BREAK;
}

#define _____PROCEDURE_FUNCTION_____
STATIC BUS_Result_t proc_ir4000hd (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			 eResult = MESSAGE_PASS;

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
		eResult = xmgr_ir4000hd_msgBusCreate (hHandle, p1, p2, p3);
		break;
	case eMEVT_BUS_READY_SHUTDOWN :
	case eMEVT_BUS_GO_SHUTDOWN:
	{
		xmgr_ir4000hd_ShutDownFrontControl();
		break;
	}
	case eMEVT_BUS_GO_OPERATION:
	{
		xmgr_ir4000hd_OperationFrontControl();
		break;
	}
	case eSEVT_CAS_CMD:
		HxLOG_Debug("[XMGR:IR4000HD(%d)] MESSAGE : eSEVT_CAS_CMD\n");
		eResult = xmgr_ir4000hd_evtCASCmd(hHandle, p1, p2, p3);
		break;

	default:
		break;
	}

	return (MESSAGE_BREAK == eResult) ? MESSAGE_BREAK : MESSAGE_PASS;
}

#define ___API_FUNCTIONS___
HERROR	xmgr_model_Init_IR4000HD (void)
{
	BUS_MGR_Create (NULL, APP_PORTING_MGR_PRIORITY, proc_ir4000hd, 0, 0, 0, 0);
	return ERR_OK;
}

#define _____LIVE_SPEC_____
HERROR xmgr_model_GetLiveSpec_IR4000HD (XmgrLive_LiveType_InArg_t *pstIn, XmgrLive_LiveType_t *pstOut)
{
	pstOut->eViewModule	= eLiveModuleType_Base;
	pstOut->eLockModule	= eLiveModuleType_Base;

	return ERR_OK;
}

HERROR xmgr_model_LiveGetSiSpec_IR4000HD (MgrLive_START_t *pstStart, SvcSi_LiveSpec_t *pstSiSpec)
{
	pstSiSpec->eActionKind	= eSVCSI_ACT_KIND_Live;
	pstSiSpec->ulActionSpec	= eSVCSI_ACT_SPEC_MeSat;
	pstSiSpec->eVideo		= eSVCSI_ES_SELECT_Normal;
	pstSiSpec->eAudio		= eSVCSI_ES_SELECT_Normal;
	pstSiSpec->eSubtitle	= eSVCSI_ES_SELECT_Normal;
	pstSiSpec->eTeletext	= eSVCSI_ES_SELECT_Normal;
	pstSiSpec->eTimeUpdate	= eSVCSI_TIME_UPDATE_MiddelEast;

	return ERR_OK;
}

#define _____SEARCH_SPEC_____
HERROR xmgr_model_SearchGetSiSpec_IR4000HD (SvcSi_SearchOption_t *pstOption, SvcSi_SearchSpec_t *pstSiSpec)
{
	pstSiSpec->ulActionKind	= eSVCSI_ACT_KIND_Search;
	pstSiSpec->ulActionSpec	= eSVCSI_ACT_SPEC_MeSat;

	return ERR_OK;
}

#define _____LCN_POLICY_____
HERROR xmgr_model_SearchGetLcnPolicy_IR4000HD (DefaultLcnPolicy_t *peLcnPolicy, PreProgrammedChNumPolicy_t *pePreProgChNumPolicy)
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

#define _____SOFTWARE_UPDATE_____
HERROR xmgr_model_SwupGetSiSpec_IR4000HD (MgrSwup_InArg_t *pstInArg, SvcSi_SwupCheckSpec_t *pstSiSpec)
{
	pstSiSpec->eActionKind	= eSVCSI_ACT_KIND_SwupCheck;

	switch (pstInArg->eSwUpdateSpec)
	{
	case eDxSwUpdate_SPEC_IRDETO:
		pstSiSpec->ulActionSpec	= eSVCSI_ACT_SPEC_Irdeto;
		break;

	default:
		pstSiSpec->ulActionSpec	= eSVCSI_ACT_SPEC_Humax;
		break;
	}

	return ERR_OK;
}

HERROR	xmgr_model_SwupInitProduct_IR4000HD (void)
{
	MgrSwup_ModuleInfo_t	stModuleInfo;
	MgrSwup_ModuleCb_t	*pstCallback = &stModuleInfo.stCallback;
	DxSwUpdate_Spec_e	eSwUpdateSpec;

	MGR_SWUP_GetSpec(&eSwUpdateSpec);
	if(eSwUpdateSpec == eDxSwUpdate_SPEC_IRDETO)
	{
		HLIB_STD_StrUtf8NCpy(stModuleInfo.szModuleName, "IRDETO_SWUP", SWUPDATE_MODULE_NAME_LEN);

		pstCallback->pfnCreate			= XMGR_SWUP_Create_Irdeto;
		pstCallback->pfnEnterAction		= XMGR_SWUP_EnterAction_Irdeto;
		pstCallback->pfnLeaveAction		= XMGR_SWUP_LeaveAction_Irdeto;
		pstCallback->pfnGetNextState		= XMGR_SWUP_GetNextState_Irdeto;
		pstCallback->pfnDestroy			= XMGR_SWUP_Destroy_Irdeto;
		pstCallback->pfnSetManualInfo	= XMGR_SWUP_SetManualInfo_Irdeto;
		pstCallback->pfnGetHomeTpInfo	= XMGR_SWUP_GetHomeTpInfo_Irdeto;
	}
	else
	{
		HLIB_STD_StrUtf8NCpy(stModuleInfo.szModuleName, "HUMAX_SWUP", SWUPDATE_MODULE_NAME_LEN);

		pstCallback->pfnCreate			= XMGR_SWUP_Create_Humax;
		pstCallback->pfnEnterAction		= XMGR_SWUP_EnterAction_Humax;
		pstCallback->pfnLeaveAction		= XMGR_SWUP_LeaveAction_Humax;
		pstCallback->pfnGetNextState	= XMGR_SWUP_GetNextState_Humax;
		pstCallback->pfnDestroy			= XMGR_SWUP_Destroy_Humax;
		pstCallback->pfnSetManualInfo	= XMGR_SWUP_SetManualInfo_Humax;
		pstCallback->pfnGetHomeTpInfo	= XMGR_SWUP_GetHomeTpInfo_Humax;
	}
	MGR_SWUP_RegisterModuleInfo(&stModuleInfo);

	return ERR_OK;
}

#define _____PVR_SPEC_____
HERROR xmgr_model_RecordGetSiSpec_IR4000HD (SvcSi_RecSpec_t *pstSiSpec)
{
	pstSiSpec->eActionKind	= eSVCSI_ACT_KIND_Record;
	pstSiSpec->ulActionSpec	= eSVCSI_ACT_SPEC_MeSat;

	return ERR_OK;
}

HERROR xmgr_model_PlaybackGetSiSpec_IR4000HD (SvcSi_PbSpec_t *pstSiSpec)
{
	pstSiSpec->eActionKind	= eSVCSI_ACT_KIND_Playback;
	pstSiSpec->ulActionSpec	= eSVCSI_ACT_SPEC_MeSat;

	pstSiSpec->eVideo		= eSVCSI_ES_SELECT_Normal;
	pstSiSpec->eAudio		= eSVCSI_ES_SELECT_Normal;
	pstSiSpec->eSubtitle	= eSVCSI_ES_SELECT_Normal;
	pstSiSpec->eTeletext	= eSVCSI_ES_SELECT_Normal;
	pstSiSpec->eTimeUpdate	= eSVCSI_TIME_UPDATE_MiddelEast;

	return ERR_OK;
}

HERROR xmgr_model_MediaplaybackGetSiSpec_IR4000HD (SvcSi_MediaPbSpec_t *pstSiSpec)
{
	pstSiSpec->eActionKind	= eSVCSI_ACT_KIND_MediaPlayback;
	pstSiSpec->ulActionSpec	= eSVCSI_ACT_SPEC_MeSat;

	return ERR_OK;
}

HERROR xmgr_model_RecordGetCasTypes_IR4000HD (HUINT32 *pulCasTypes)
{
	if (NULL == pulCasTypes)
		return ERR_FAIL;

	*pulCasTypes	=	eMgrRec_CasType_Irdeto;

	return ERR_OK;
}

HERROR xmgr_model_PlaybackGetCasTypes_IR4000HD (HUINT32 *pulCasTypes)
{
	if (NULL == pulCasTypes)
		return ERR_FAIL;

	*pulCasTypes	=	eMgrPb_CasType_Irdeto;

	return ERR_OK;
}

#define _____TIME_UPDATE_SPEC_____
HERROR xmgr_model_TimeupdateGetSiSpec_IR4000HD (SvcSi_TimeUpdateSpec_t *pstSiSpec)
{
	pstSiSpec->eActionKind	= eSVCSI_ACT_KIND_TimeUpdate;
	pstSiSpec->ulActionSpec	= eSVCSI_ACT_SPEC_MeSat;

	return ERR_OK;
}

#define _____MAKE_SECTION_SPEC_____
HERROR xmgr_model_MakesectionGetSiSpec_IR4000HD (SvcSi_MakeSectionSpec_t *pstSiSpec)
{
	pstSiSpec->eActionKind	= eSVCSI_ACT_KIND_None;		// reverved for future use
	pstSiSpec->ulActionSpec	= eSVCSI_ACT_SPEC_EuSat;

	return ERR_OK;
}

HERROR xmgr_model_GetSiEsPriorityTable_IR4000HD (SvcSi_EsPriorityTable_t *pstTable)
{
	return xmgr_model_GetSiEsPriorityTable_Base(pstTable);
}

/* end of file */

