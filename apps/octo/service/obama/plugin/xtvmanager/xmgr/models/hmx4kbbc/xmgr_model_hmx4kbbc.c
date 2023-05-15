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

#include "../local_include/_xmgr_model.h"

#define ___LOCAL_FUNCTIONS___
STATIC void xmgr_HMX4KBBC_OperationFrontControl(void)
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

STATIC void xmgr_HMX4KBBC_ShutDownFrontControl(void)
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
STATIC BUS_Result_t xmgr_hms1000s_msgBusCreate (Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	// Default SI Character Type 을 여기서 세팅해준다
	(void) MWC_UTIL_SetSiStringEncodedType (eSITEXT_ENCODED_300468_EUROPE);

	NOT_USED_PROC_ARGUMENT (hHandle, p1, p2, p3);
	return MESSAGE_BREAK;
}

#define _____PROCEDURE_FUNCTION_____
STATIC BUS_Result_t proc_hms1000s (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			 eResult = MESSAGE_PASS;

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HUINT8*)__FUNCTION__);
		eResult = xmgr_hms1000s_msgBusCreate (hHandle, p1, p2, p3);
		break;
	case eMEVT_BUS_READY_SHUTDOWN :
	case eMEVT_BUS_GO_SHUTDOWN:
	{
		xmgr_HMX4KBBC_ShutDownFrontControl();
		break;
	}
	case eMEVT_BUS_GO_OPERATION:
	{
		xmgr_HMX4KBBC_OperationFrontControl();
		break;
	}
	default:
		break;
	}

	return (MESSAGE_BREAK == eResult) ? MESSAGE_BREAK : MESSAGE_PASS;
}

#define ___API_FUNCTIONS___
HERROR	xmgr_model_Init_HMX4KBBC (void)
{
	BUS_MGR_Create (NULL, APP_PORTING_MGR_PRIORITY, proc_hms1000s, 0, 0, 0, 0);
	return ERR_OK;
}

#define _____LIVE_SPEC_____
HERROR xmgr_model_GetLiveSpec_HMX4KBBC (XmgrLive_LiveType_InArg_t *pstIn, XmgrLive_LiveType_t *pstOut)
{
	pstOut->eViewModule	= eLiveModuleType_Base;
	pstOut->eLockModule	= eLiveModuleType_Base;

	return ERR_OK;
}

HERROR xmgr_model_LiveGetSiSpec_HMX4KBBC (MgrLive_START_t *pstStart, SvcSi_LiveSpec_t *pstSiSpec)
{
	pstSiSpec->eActionKind	= eSVCSI_ACT_KIND_Live;
	pstSiSpec->ulActionSpec	= eSVCSI_ACT_SPEC_EuSat;
	pstSiSpec->eVideo		= eSVCSI_ES_SELECT_Normal;
	pstSiSpec->eAudio		= eSVCSI_ES_SELECT_Normal;
	pstSiSpec->eSubtitle	= eSVCSI_ES_SELECT_Normal;
	pstSiSpec->eTeletext	= eSVCSI_ES_SELECT_Normal;
	pstSiSpec->eTimeUpdate	= eSVCSI_TIME_UPDATE_EU;

	return ERR_OK;
}

#define _____SEARCH_SPEC_____
HERROR xmgr_model_SearchGetSiSpec_HMX4KBBC (SvcSi_SearchOption_t *pstOption, SvcSi_SearchSpec_t *pstSiSpec)
{
	pstSiSpec->ulActionKind	= eSVCSI_ACT_KIND_Search;
	pstSiSpec->ulActionSpec	= eSVCSI_ACT_SPEC_EuSat;

	return ERR_OK;
}

#define _____LCN_POLICY_____
HERROR xmgr_model_SearchGetLcnPolicy_HMX4KBBC (DefaultLcnPolicy_t *peLcnPolicy, PreProgrammedChNumPolicy_t *pePreProgChNumPolicy)
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
HERROR xmgr_model_SwupGetSiSpec_HMX4KBBC (MgrSwup_InArg_t *pstInArg, SvcSi_SwupCheckSpec_t *pstSiSpec)
{
	pstSiSpec->eActionKind	= eSVCSI_ACT_KIND_SwupCheck;
	pstSiSpec->ulActionSpec	= eSVCSI_ACT_SPEC_Astra;

	return ERR_OK;
}

HERROR	xmgr_model_SwupInitProduct_HMX4KBBC (void)
{
	MgrSwup_ModuleInfo_t	stModuleInfo;
	MgrSwup_ModuleCb_t	*pstCallback = &stModuleInfo.stCallback;

	HLIB_STD_StrUtf8NCpy(stModuleInfo.szModuleName, "HMS_SWUP", SWUPDATE_MODULE_NAME_LEN);

	pstCallback->pfnCreate			= NULL;
	pstCallback->pfnEnterAction		= NULL;
	pstCallback->pfnLeaveAction		= NULL;
	pstCallback->pfnGetNextState	= NULL;
	pstCallback->pfnDestroy			= NULL;
	pstCallback->pfnSetManualInfo	= NULL;
	pstCallback->pfnGetHomeTpInfo	= NULL;

	MGR_SWUP_RegisterModuleInfo(&stModuleInfo);

	return ERR_OK;

}

#define _____PVR_SPEC_____
HERROR xmgr_model_RecordGetSiSpec_HMX4KBBC (SvcSi_RecSpec_t *pstSiSpec)
{
	pstSiSpec->eActionKind	= eSVCSI_ACT_KIND_Record;
	pstSiSpec->ulActionSpec	= eSVCSI_ACT_SPEC_EuSat;


	return ERR_OK;
}

HERROR xmgr_model_PlaybackGetSiSpec_HMX4KBBC (SvcSi_PbSpec_t *pstSiSpec)
{
	pstSiSpec->eActionKind	= eSVCSI_ACT_KIND_Playback;
	pstSiSpec->ulActionSpec	= eSVCSI_ACT_SPEC_EuSat;

	pstSiSpec->eVideo		= eSVCSI_ES_SELECT_Normal;
	pstSiSpec->eAudio		= eSVCSI_ES_SELECT_Normal;
	pstSiSpec->eSubtitle	= eSVCSI_ES_SELECT_Normal;
	pstSiSpec->eTeletext	= eSVCSI_ES_SELECT_Normal;
	pstSiSpec->eTimeUpdate	= eSVCSI_TIME_UPDATE_EU;

	return ERR_OK;
}

HERROR xmgr_model_MediaplaybackGetSiSpec_HMX4KBBC (SvcSi_MediaPbSpec_t *pstSiSpec)
{
	pstSiSpec->eActionKind	= eSVCSI_ACT_KIND_MediaPlayback;
	pstSiSpec->ulActionSpec	= eSVCSI_ACT_SPEC_EuSat;

	return ERR_OK;
}

HERROR xmgr_model_RecordGetCasTypes_HMX4KBBC (HUINT32 *pulCasTypes)
{
	if (NULL == pulCasTypes)
		return ERR_FAIL;

	*pulCasTypes	=	eMgrRec_CasType_CiPlus;

	return ERR_OK;
}

HERROR xmgr_model_PlaybackGetCasTypes_HMX4KBBC (HUINT32 *pulCasTypes)
{
	if (NULL == pulCasTypes)
		return ERR_FAIL;

	*pulCasTypes	=	eMgrRec_CasType_CiPlus;

	return ERR_OK;
}

#define _____TIME_UPDATE_SPEC_____
HERROR xmgr_model_TimeupdateGetSiSpec_HMX4KBBC (SvcSi_TimeUpdateSpec_t *pstSiSpec)
{
	pstSiSpec->eActionKind	= eSVCSI_ACT_KIND_TimeUpdate;
	pstSiSpec->ulActionSpec	= eSVCSI_ACT_SPEC_EuSat;

	return ERR_OK;
}

#define _____MAKE_SECTION_SPEC_____
HERROR xmgr_model_MakesectionGetSiSpec_HMX4KBBC (SvcSi_MakeSectionSpec_t *pstSiSpec)
{
	pstSiSpec->eActionKind	= eSVCSI_ACT_KIND_None;		// reverved for future use
	pstSiSpec->ulActionSpec	= eSVCSI_ACT_SPEC_EuSat;

	return ERR_OK;
}

HERROR xmgr_model_GetSiEsPriorityTable_HMX4KBBC (SvcSi_EsPriorityTable_t *pstTable)
{
	return xmgr_model_GetSiEsPriorityTable_Base(pstTable);
}


/* end of file */
