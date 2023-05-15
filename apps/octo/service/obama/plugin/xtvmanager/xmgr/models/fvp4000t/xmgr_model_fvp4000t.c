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
#include <papi.h>
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

#include <xmgr_live.h>
#include <xmgr_swup.h>
#include <xmgr_module_portingprotocol.h>
#include "../local_include/_xmgr_model.h"

#define ___LOCAL_FUNCTIONS___
#define _____MESSAGE_PROCESS_FUNCTION_____
STATIC BUS_Result_t xmgr_fvp4000t_msgBusCreate (Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	// Default SI Character Type 을 여기서 세팅해준다
	(void) MWC_UTIL_SetSiStringEncodedType (eSITEXT_ENCODED_300468_UK);

	NOT_USED_PROC_ARGUMENT (hHandle, p1, p2, p3);
	return MESSAGE_BREAK;
}

STATIC void xmgr_fvp4000t_OperationFrontControl(void)
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

STATIC void xmgr_fvp4000t_ShutDownFrontControl(void)
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

STATIC void xmgr_fvp4000t_msgBusGoShutdown (Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	(void)PAPI_SetSleepModeOnStandby(ePAMA_POWERSTATE_DEEP_SLEEP);
	(void)PAPI_SetBootModeOnStandby(ePAMA_BOOTMODE_WARM);
}

#define _____PROCEDURE_FUNCTION_____
STATIC BUS_Result_t proc_fvp4000t (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			 eResult = MESSAGE_PASS;
	HINT32					nProgress;
	HINT32					nProgressMax;
	MgrAction_State_e		eActionState;

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
		eResult = xmgr_fvp4000t_msgBusCreate (hHandle, p1, p2, p3);
		break;

	case eMEVT_DN_SWUP_INSTALL_PROGRESS:
		{
			HxLOG_Print("eMEVT_DN_SWUP_INSTALL_PROGRESS - swHandle(0x%X), [ %ld : %ld ]\n\n", (Handle_t)p1, (HUINT32)p2, (HUINT32)p3);

			nProgress	=	((HINT32)p2/2) + ((HINT32)p3/2);
			nProgressMax=	(HINT32)p3;

			if (nProgress != nProgressMax)
			{
				break;
			}

			eActionState = MGR_ACTION_GetState();
			if (eActionState != eAmState_ACTIVE_STANDBY)
			{
				break;
			}

			// reboot를 하는 주체는 progress를 받는 곳에서 해야 하나, standby시에는 progress를 받는 곳이 없으므로 여기서 한다.
			HxLOG_Error("Reboot System by standby OTA. \n");
			MGR_ACTION_RebootSystem();
		}
		break;
	case eMEVT_BUS_READY_SHUTDOWN :
	case eMEVT_BUS_GO_SHUTDOWN:
	{
		xmgr_fvp4000t_ShutDownFrontControl();
		xmgr_fvp4000t_msgBusGoShutdown (hHandle, p1, p2, p3);
		break;
	}
	case eMEVT_BUS_GO_OPERATION:
	{
		xmgr_fvp4000t_OperationFrontControl();
		break;
	}
	default:
		break;
	}

	return (MESSAGE_BREAK == eResult) ? MESSAGE_BREAK : MESSAGE_PASS;
}

#define ___API_FUNCTIONS___
HERROR	xmgr_model_Init_FVP4000T (void)
{
	BUS_MGR_Create (NULL, APP_DEFAULT_PRIORITY, proc_fvp4000t, 0, 0, 0, 0);
	return ERR_OK;
}

#define _____LIVE_SPEC_____
HERROR xmgr_model_GetLiveSpec_FVP4000T (XmgrLive_LiveType_InArg_t *pstIn, XmgrLive_LiveType_t *pstOut)
{
	pstOut->eViewModule	= eLiveModuleType_Base;
	pstOut->eLockModule	= eLiveModuleType_Base;

	return ERR_OK;
}

HERROR xmgr_model_LiveGetSiSpec_FVP4000T (MgrLive_START_t *pstStart, SvcSi_LiveSpec_t *pstSiSpec)
{
	pstSiSpec->eActionKind	= eSVCSI_ACT_KIND_Live;
	pstSiSpec->ulActionSpec	= eSVCSI_ACT_SPEC_UkDtt;
	pstSiSpec->eVideo		= eSVCSI_ES_SELECT_Ukdtt;
	pstSiSpec->eAudio		= eSVCSI_ES_SELECT_Normal;
	pstSiSpec->eSubtitle	= eSVCSI_ES_SELECT_Normal;
	pstSiSpec->eTeletext	= eSVCSI_ES_SELECT_Normal;
	pstSiSpec->eTimeUpdate	= eSVCSI_TIME_UPDATE_Ukdtt;

	return ERR_OK;
}

#define _____SEARCH_SPEC_____
HERROR xmgr_model_SearchGetSiSpec_FVP4000T (SvcSi_SearchOption_t *pstOption, SvcSi_SearchSpec_t *pstSiSpec)
{
	pstSiSpec->ulActionKind	= eSVCSI_ACT_KIND_Search;
	pstSiSpec->ulActionSpec	= eSVCSI_ACT_SPEC_UkDtt;

	return ERR_OK;
}

#define _____LCN_POLICY_____
HERROR xmgr_model_SearchGetLcnPolicy_FVP4000T (DefaultLcnPolicy_t *peLcnPolicy, PreProgrammedChNumPolicy_t *pePreProgChNumPolicy)
{
	if ((peLcnPolicy == NULL) && (pePreProgChNumPolicy == NULL))		{ return ERR_FAIL; }

	if (peLcnPolicy != NULL)
	{
		*peLcnPolicy = eDEFAULTLCN_UKDTT;
	}

	if (pePreProgChNumPolicy != NULL)
	{
		*pePreProgChNumPolicy = ePreChNum_NONE;
	}

	return ERR_OK;
}

#define _____SOFTWARE_UPDATE_____
HERROR xmgr_model_SwupGetSiSpec_FVP4000T (MgrSwup_InArg_t *pstInArg, SvcSi_SwupCheckSpec_t *pstSiSpec)
{
	pstSiSpec->eActionKind	= eSVCSI_ACT_KIND_SwupCheck;
	pstSiSpec->ulActionSpec	= eSVCSI_ACT_SPEC_DvbSsu;

	return ERR_OK;
}

HERROR	xmgr_model_SwupInitProduct_FVP4000T (void)
{
	MgrSwup_ModuleInfo_t	stModuleInfo;
	MgrSwup_ModuleCb_t	*pstCallback = &stModuleInfo.stCallback;

	HLIB_STD_StrUtf8NCpy(stModuleInfo.szModuleName, "UK_SWUP", SWUPDATE_MODULE_NAME_LEN);

	pstCallback->pfnCreate			= XMGR_SWUP_Create_Dvbssu;
	pstCallback->pfnEnterAction		= XMGR_SWUP_EnterAction_Dvbssu;
	pstCallback->pfnLeaveAction		= XMGR_SWUP_LeaveAction_Dvbssu;
	pstCallback->pfnGetNextState		= XMGR_SWUP_GetNextState_Dvbssu;
	pstCallback->pfnDestroy			= XMGR_SWUP_Destroy_Dvbssu;
	pstCallback->pfnSetManualInfo	= XMGR_SWUP_SetManualInfo_Dvbssu;
	pstCallback->pfnGetHomeTpInfo	= XMGR_SWUP_GetHomeTpInfo_Dvbssu;

	MGR_SWUP_RegisterModuleInfo(&stModuleInfo);

	return ERR_OK;

}

#define _____PVR_SPEC_____
HERROR xmgr_model_RecordGetSiSpec_FVP4000T (SvcSi_RecSpec_t *pstSiSpec)
{
	pstSiSpec->eActionKind	= eSVCSI_ACT_KIND_Record;
	pstSiSpec->ulActionSpec	= eSVCSI_ACT_SPEC_UkDtt;

	return ERR_OK;
}

HERROR xmgr_model_PlaybackGetSiSpec_FVP4000T (SvcSi_PbSpec_t *pstSiSpec)
{
	pstSiSpec->eActionKind	= eSVCSI_ACT_KIND_Playback;
	pstSiSpec->ulActionSpec	= eSVCSI_ACT_SPEC_UkDtt;

	pstSiSpec->eVideo		= eSVCSI_ES_SELECT_Normal;
	pstSiSpec->eAudio		= eSVCSI_ES_SELECT_Normal;
	pstSiSpec->eSubtitle	= eSVCSI_ES_SELECT_Normal;
	pstSiSpec->eTeletext	= eSVCSI_ES_SELECT_Normal;
	pstSiSpec->eTimeUpdate	= eSVCSI_TIME_UPDATE_Ukdtt;

	return ERR_OK;
}

HERROR xmgr_model_MediaplaybackGetSiSpec_FVP4000T  (SvcSi_MediaPbSpec_t *pstSiSpec)
{
	pstSiSpec->eActionKind	= eSVCSI_ACT_KIND_MediaPlayback;
	pstSiSpec->ulActionSpec	= eSVCSI_ACT_SPEC_UkDtt;

	return ERR_OK;
}

HERROR xmgr_model_RecordGetCasTypes_FVP4000T (HUINT32 *pulCasTypes)
{
	if (NULL == pulCasTypes)
		return ERR_FAIL;

	*pulCasTypes	=	eMgrRec_CasType_None;

	return ERR_OK;
}

HERROR xmgr_model_PlaybackGetCasTypes_FVP4000T (HUINT32 *pulCasTypes)
{
	if (NULL == pulCasTypes)
		return ERR_FAIL;

	*pulCasTypes	=	eMgrPb_CasType_None;

	return ERR_OK;
}

#define _____TIME_UPDATE_SPEC_____
HERROR xmgr_model_TimeupdateGetSiSpec_FVP4000T (SvcSi_TimeUpdateSpec_t *pstSiSpec)
{
	pstSiSpec->eActionKind	= eSVCSI_ACT_KIND_TimeUpdate;
	pstSiSpec->ulActionSpec	= eSVCSI_ACT_SPEC_UkDtt;

	return ERR_OK;
}

#define _____MAKE_SECTION_SPEC_____
HERROR xmgr_model_MakesectionGetSiSpec_FVP4000T (SvcSi_MakeSectionSpec_t *pstSiSpec)
{
	pstSiSpec->eActionKind	= eSVCSI_ACT_KIND_None;		// reverved for future use
	pstSiSpec->ulActionSpec	= eSVCSI_ACT_SPEC_UkDtt;

	return ERR_OK;
}

HERROR xmgr_model_GetSiEsPriorityTable_FVP4000T (SvcSi_EsPriorityTable_t *pstTable)
{
	STATIC SvcSi_EsTypePriority_t s_astAudioPriorityTable_Fvp4000t[] =
	{
		{ eSvcSi_AUD_PRIORITY_LANG_BASE,							eSvcSi_AUD_SEL_TYPE_NUM,							1000	},
		{ eSvcSi_AUD_PRIORITY_LANG_USER,							eSvcSi_AUD_SEL_LANG_USER,							8000	},
		{ eSvcSi_AUD_PRIORITY_LANG_0,								eSvcSi_AUD_SEL_LANG_0,							7000	},
		{ eSvcSi_AUD_PRIORITY_LANG_1,								eSvcSi_AUD_SEL_LANG_1,							0	},
		{ eSvcSi_AUD_PRIORITY_LANG_OSD, 							eSvcSi_AUD_SEL_LANG_OSD,							2000	},
		{ eSvcSi_AUD_PRIORITY_LANG_UNDEFINED,						eSvcSi_AUD_SEL_LANG_UNDEFINED,					5000	},
		{ eSvcSi_AUD_PRIORITY_LANG_ORIGINAL,						eSvcSi_AUD_SEL_LANG_ORIGINAL,						4000	},
		{ eSvcSi_AUD_PRIORITY_LANG_DEFAULT,						eSvcSi_AUD_SEL_LANG_DEFAULT,						3000	},
		{eSvcSi_AUD_PRIORITY_TYPE_UNDEFINED,						eSvcSi_AUD_SEL_TYPE_UNDEFINED,					30000},
		{eSvcSi_AUD_PRIORITY_TYPE_CLEANEFFECT,						eSvcSi_AUD_SEL_TYPE_CLEANEFFECT,					20000},
		{eSvcSi_AUD_PRIORITY_TYPE_HEARING_IMPAIRED,				eSvcSi_AUD_SEL_TYPE_HEARING_IMPAIRED,				0	},
		{eSvcSi_AUD_PRIORITY_TYPE_VISUAL_IMPAIRED,					eSvcSi_AUD_SEL_TYPE_VISUAL_IMPAIRED,				0	},
		{ eSvcSi_AUD_PRIORITY_DESCRIPTION_BROADCASTER_MIX_USER, 	eSvcSi_AUD_SEL_DESCRIPTION_BROADCASTER_MIX_USER,	700000	},
		{ eSvcSi_AUD_PRIORITY_DESCRIPTION_BROADCASTER_MIX_MENU, 	eSvcSi_AUD_SEL_DESCRIPTION_BROADCASTER_MIX_MENU,	600000	},
		{ eSvcSi_AUD_PRIORITY_DESCRIPTION_OFF_BROADCASTER_MIX_ON,	eSvcSi_AUD_SEL_DESCRIPTION_OFF_BROADCASTER_MIX_ON,	200000	},
		{ eSvcSi_AUD_PRIORITY_DESCRIPTION_BROADCASTER_MIX_BASE, 	eSvcSi_AUD_SEL_TYPE_NUM,							100000	},
		{ eSvcSi_AUD_PRIORITY_DESCRIPTION_RECEIVER_MIX_USER,		eSvcSi_AUD_SEL_DESCRIPTION_RECEIVER_MIX_USER,		500000	},
		{ eSvcSi_AUD_PRIORITY_DESCRIPTION_RECEIVER_MIX_MENU,		eSvcSi_AUD_SEL_DESCRIPTION_RECEIVER_MIX_MENU,		400000	},
		{ eSvcSi_AUD_PRIORITY_DESCRIPTION_OFF_RECEIVER_MIX_ON,		eSvcSi_AUD_SEL_DESCRIPTION_OFF_RECEIVER_MIX_ON,		200000	},
		{ eSvcSi_AUD_PRIORITY_DESCRIPTION_RECEIVER_MIX_BASE,		eSvcSi_AUD_SEL_TYPE_NUM,							100000	},
		{ eSvcSi_AUD_PRIORITY_DESCRIPTION_OFF_BROADCASTER_MIX_OFF,	eSvcSi_AUD_SEL_DESCRIPTION_OFF_BROADCASTER_MIX_OFF,	300000	},
		{ eSvcSi_AUD_PRIORITY_DESCRIPTION_OFF_RECEIVER_MIX_OFF,		eSvcSi_AUD_SEL_DESCRIPTION_OFF_RECEIVER_MIX_OFF,	300000	},
		{ eSvcSi_AUD_PRIORITY_DESCRIPTION_ON_BROADCASTER_MIX_OFF,	eSvcSi_AUD_SEL_DESCRIPTION_ON_BROADCASTER_MIX_OFF,	300000	},
		{ eSvcSi_AUD_PRIORITY_DESCRIPTION_ON_RECEIVER_MIX_OFF,		eSvcSi_AUD_SEL_DESCRIPTION_ON_RECEIVER_MIX_OFF,		300000	},
		{ eSvcSi_AUD_PRIORITY_MULTICHANNEL_BASE,					eSvcSi_AUD_SEL_TYPE_NUM,							0		},
		{ eSvcSi_AUD_PRIORITY_MULTICHANNEL_USER,					eSvcSi_AUD_SEL_MULTICHANNEL_USER,					200 	},
		{ eSvcSi_AUD_PRIORITY_MULTICHANNEL_MENU,					eSvcSi_AUD_SEL_MULTICHANNEL_MENU,					100 	},
		{ eSvcSi_AUD_PRIORITY_FIRST_TRACK,							eSvcSi_AUD_SEL_TYPE_NUM, 							1		},
		{ eSvcSi_AUD_PRIORITY_DUAL_MONO,							eSvcSi_AUD_SEL_TYPE_NUM,							10 		},
		{ eSvcSi_AUD_PRIORITY_TYPE_NUM, 							eSvcSi_AUD_SEL_TYPE_NUM,							-1		}
	};

	STATIC SvcSi_EsTypePriority_t s_astSubtitlePriorityTable_Fvp4000t[] =
	{
		{	eSvcSi_SUBTTL_PRIORITY_LANG_BASE,						eSvcSi_SUBTTL_SEL_TYPE_NUM, 						10000	},
		{	eSvcSi_SUBTTL_PRIORITY_LANG_USER,						eSvcSi_SUBTTL_SEL_LANG_USER,						80000	},
		{	eSvcSi_SUBTTL_PRIORITY_LANG_MENU_0, 					eSvcSi_SUBTTL_SEL_LANG_0,							70000	},
		{	eSvcSi_SUBTTL_PRIORITY_LANG_MENU_1, 					eSvcSi_SUBTTL_SEL_LANG_1,							0	},
		{	eSvcSi_SUBTTL_PRIORITY_LANG_OSD,						eSvcSi_SUBTTL_SEL_LANG_OSD, 						20000	},
		{	eSvcSi_SUBTTL_PRIORITY_LANG_UNDEFINED,					eSvcSi_SUBTTL_SEL_LANG_UNDEFINED,					50000	},
		{	eSvcSi_SUBTTL_PRIORITY_LANG_ORIGINAL,					eSvcSi_SUBTTL_SEL_LANG_ORIGINAL,					40000	},
		{	eSvcSi_SUBTTL_PRIORITY_LANG_DEFAULT,					eSvcSi_SUBTTL_SEL_LANG_DEFAULT, 					30000	},
		{	eSvcSi_SUBTTL_PRIORITY_TYPE_BASE,						eSvcSi_SUBTTL_SEL_TYPE_NUM, 						1000	},
		{	eSvcSi_SUBTTL_PRIORITY_TYPE_DVB_MATCHED_USER,			eSvcSi_SUBTTL_SEL_TYPE_DVB_USER,					7000	},
		{	eSvcSi_SUBTTL_PRIORITY_TYPE_DVB_MATCHED_MENU,			eSvcSi_SUBTTL_SEL_TYPE_DVB_MENU,					6000	},
		{	eSvcSi_SUBTTL_PRIORITY_TYPE_DVB_MATCHED_AUTO,			eSvcSi_SUBTTL_SEL_TYPE_DVB_AUTO,					5000	},
		{	eSvcSi_SUBTTL_PRIORITY_TYPE_EBU_MATCHED_USER,			eSvcSi_SUBTTL_SEL_TYPE_EBU_USER,					4000	},
		{	eSvcSi_SUBTTL_PRIORITY_TYPE_EBU_MATCHED_MENU,			eSvcSi_SUBTTL_SEL_TYPE_EBU_MENU,					3000	},
		{	eSvcSi_SUBTTL_PRIORITY_TYPE_EBU_MATCHED_AUTO,			eSvcSi_SUBTTL_SEL_TYPE_EBU_AUTO,					2000	},
		{	eSvcSi_SUBTTL_PRIORITY_HOH_BASE,						eSvcSi_SUBTTL_SEL_TYPE_NUM, 						100000	},
		{	eSvcSi_SUBTTL_PRIORITY_HOH_MATCHED_USER,				eSvcSi_SUBTTL_SEL_HOH_USER, 						400000	},
		{	eSvcSi_SUBTTL_PRIORITY_HOH_MATCHED_MENU,				eSvcSi_SUBTTL_SEL_HOH_MENU, 						300000	},
		{	eSvcSi_SUBTTL_PRIORITY_HOH_OFF_ES_YES,					eSvcSi_SUBTTL_SEL_HOH_OFF_ES_YES,					100000	},
		{	eSvcSi_SUBTTL_PRIORITY_HOH_OFF_ES_NO,					eSvcSi_SUBTTL_SEL_HOH_OFF_ES_NO,					200000	},
		{	eSvcSi_SUBTTL_PRIORITY_HOH_ON_ES_NO,					eSvcSi_SUBTTL_SEL_HOH_ON_ES_NO, 					200000	},
		{	eSvcSi_SUBTTL_PRIORITY_ASPECTRATIO_BASE,				eSvcSi_SUBTTL_SEL_TYPE_NUM, 						10		},
		{	eSvcSi_SUBTTL_PRIORITY_ASPECTRATIO_NA,					eSvcSi_SUBTTL_SEL_ASPECTRATIO_NA,					20		},
		{	eSvcSi_SUBTTL_PRIORITY_ASPECTRATIO_MATCHED_YES, 		eSvcSi_SUBTTL_SEL_ASPECTRATIO_YES,					30		},
		{	eSvcSi_SUBTTL_PRIORITY_HD_BASE, 						eSvcSi_SUBTTL_SEL_TYPE_NUM, 						100 	},
		{	eSvcSi_SUBTTL_PRIORITY_HD_NA,							eSvcSi_SUBTTL_SEL_HD_NA,							300 	},
		{	eSvcSi_SUBTTL_PRIORITY_HD_MATCHED_USER, 				eSvcSi_SUBTTL_SEL_HD_USER,							400 	},
		{	eSvcSi_SUBTTL_PRIORITY_HD_MATCHED_MENU, 				eSvcSi_SUBTTL_SEL_HD_MENU,							500 	},
		{	eSvcSi_SUBTTL_PRIORITY_FIRST_TRACK_BASE,				eSvcSi_SUBTTL_SEL_TYPE_NUM, 						0		},
		{	eSvcSi_SUBTTL_PRIORITY_FIRST_TRACK_YES, 				eSvcSi_SUBTTL_SEL_TRACK,							1		},
		{	eSvcSi_SUBTTL_PRIORITY_FIRST_TRACK_NO,					eSvcSi_SUBTTL_SEL_TYPE_NUM, 						0		},
		{	eSvcSi_SUBTTL_PRIORITY_TYPE_NUM,						-1, 												0		}
	};

	if (NULL == pstTable)
		return ERR_FAIL;

	HxSTD_MemSet(pstTable, 0x00, sizeof(SvcSi_EsPriorityTable_t));

	pstTable->astAudio 			= s_astAudioPriorityTable_Fvp4000t;
	pstTable->ulItemNumAudio	= sizeof(s_astAudioPriorityTable_Fvp4000t) / sizeof(SvcSi_EsTypePriority_t);

	pstTable->astSubtitle		= s_astSubtitlePriorityTable_Fvp4000t;
	pstTable->ulItemNumSubtitle	= sizeof(s_astSubtitlePriorityTable_Fvp4000t) / sizeof(SvcSi_EsTypePriority_t);

	return ERR_OK;
}

/* end of file */

