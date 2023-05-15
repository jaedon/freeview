/**************************************************************
 *	@file		xmgr_setting_base.c
 *
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/02/16
 *	@author			humax
 *	@brief			menu setting change action
 **************************************************************/
#include <hlib.h>

#include <octo_common.h>

#include <db_param.h>

#include <svc_si.h>
#include <bus.h>

#include <mgr_common.h>
#include <mgr_svclist.h>
#include <mgr_action.h>
#include <mgr_live.h>

/**************************************************************
	internal structures and variables
 **************************************************************/
//STATIC HBOOL	s_bRsvLoadingDone = FALSE;

/**************************************************************
	private functions
 **************************************************************/
STATIC HERROR xmgr_setting_UpdateSvcInfoPID_Base(HUINT32 ulActionId, Handle_t hService)
{
	Handle_t			 hAction = SVC_PIPE_GetActionHandle(ulActionId);
	HBOOL			 	 bChanged = FALSE;
	DbSvc_Info_t		*pstOrgSvcInfo = NULL, *pstDstSvcInfo = NULL;
	HUINT16				 usSvcId = 0;
	HERROR				 hErr = ERR_FAIL;

	pstOrgSvcInfo = (DbSvc_Info_t *)OxMGR_Malloc (sizeof(DbSvc_Info_t));
	pstDstSvcInfo = (DbSvc_Info_t *)OxMGR_Malloc (sizeof(DbSvc_Info_t));

	if ((NULL != pstOrgSvcInfo) && (NULL != pstDstSvcInfo))
	{
		// 현재 db에 저장하고 있는 service info를 가져오고
		hErr = DB_SVC_GetServiceInfo(hService, pstOrgSvcInfo);
		if (ERR_FAIL == hErr)
		{
			HxLOG_Error("DB_SVC_GetServiceInfo() failed.. : hService(0x%X)", hService);
			goto EXIT_FUNC;
		}

		// si table에서 svc info 를 가져오기 위해 svc id를 가져오고
		hErr = DB_SVC_GetServiceIdFromSvcHandle (hService, &usSvcId);
		if (ERR_FAIL == hErr)
		{
			HxLOG_Error("DB_SVC_GetServiceIdFromSvcHandle() failed.. : hService(0x%X)", hService);
			goto EXIT_FUNC;
		}

		// si raw에서 svc info를 가져온다
		hErr = MGR_LIVE_UpdateServiceInfo (hAction, (HUINT32)usSvcId, pstOrgSvcInfo, pstDstSvcInfo, &bChanged);
		HxLOG_Info("pstOrgSvcInfo!=pstDstSvcInfo,  bChanged: %d \n", bChanged);
		if (ERR_FAIL == hErr)
		{
			HxLOG_Error("MGR_LIVE_UpdateServiceInfo() failed.. : hService(0x%X)", hService);
			goto EXIT_FUNC;
		}

		// 두 svc info를 비교해서 다르면 db에 업데이트 한다
		if (TRUE == bChanged)
		{
			hErr = DB_SVC_UpdateServiceInfo (hService, pstDstSvcInfo);
			if (ERR_FAIL == hErr)
			{
				HxLOG_Error("MGR_LIVE_UpdateServiceInfo() failed.. : hService(0x%X)", hService);
				goto EXIT_FUNC;
			}
			BUS_PostMessage (NULL, eSEVT_DB_NOTIFY_UPDATED, HANDLE_NULL, 0, 0, 0);
		}
	}

	hErr = ERR_OK;

EXIT_FUNC:

	if (NULL != pstOrgSvcInfo)				{ OxMGR_Free (pstOrgSvcInfo); }
	if (NULL != pstDstSvcInfo)				{ OxMGR_Free (pstDstSvcInfo); }

	return hErr;
}

STATIC HERROR xmgr_setting_UpdateMetaInfoPID_Base(HUINT32 ulActionId, Handle_t hService)
{

#if defined(CONFIG_MW_MM_PVR)
	Handle_t			hAction = SVC_PIPE_GetActionHandle(ulActionId);
	SvcMeta_Service_t	stOrgSvcMetaInfo, stNewSvcMetaInfo;
	HBOOL				bChanged = FALSE;
	HERROR				hErr = ERR_FAIL;
	HUINT32 			ulContentId = 0;

	HxSTD_MemSet(&stOrgSvcMetaInfo, 0, sizeof(SvcMeta_Service_t));
	HxSTD_MemSet(&stNewSvcMetaInfo, 0, sizeof(SvcMeta_Service_t));

	hErr = SVC_META_GetContentIdByhAction(hAction, &ulContentId);
	if (hErr == ERR_OK)
	{
		hErr = SVC_META_GetServiceInfo(ulContentId, &stOrgSvcMetaInfo);
	}
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SVC_META_GetServiceInfo Error (ulContentId : %d )\n", ulContentId);
		return hErr;
	}

	hErr = MGR_PLAYBACK_UpdateServiceInfo(ulActionId, stOrgSvcMetaInfo.usSvcId, &stOrgSvcMetaInfo, &stNewSvcMetaInfo, &bChanged);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("MGR_PLAYBACK_UpdateServiceInfo() failed.. : hService(0x%X)", hService);
		return hErr;
	}

	if (bChanged == TRUE)
	{
		hErr = SVC_META_SetServiceInfo(ulContentId, &stNewSvcMetaInfo, TRUE);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("SVC_META_SetServiceInfo() failed.. : hService(0x%X)", hService);
			return hErr;
		}
	}
#else
	HERROR				hErr = ERR_FAIL;
#endif

	hErr = ERR_OK;
	return hErr;
}

STATIC BUS_Result_t xmgr_setting_msgMainLanguageChanged_Base(void)
{
	HUINT32				ulActId = MGR_ACTION_GetMainActionId();
	Handle_t			hService;
	MgrAction_Type_e	eActionType;
	HERROR				hErr;

	/* reset user select for audio & subtitle prioritising */
	DB_SVC_ResetAudioUserFlag();
	DB_SVC_ResetSubtitleUserFlag();

	eActionType = MGR_ACTION_GetType(ulActId);
	switch(eActionType)
	{
	case eViewType_WTV:
		MGR_ACTION_GetMasterSvcHandle(ulActId, &hService);

		/* Service Info에 변경 사항을 적용 시킨다. */
		hErr = xmgr_setting_UpdateSvcInfoPID_Base(ulActId, hService);
		break;

	case eViewType_PB:
	case eViewType_TSR_PB:
	case eViewType_CHASE_PB:
		MGR_ACTION_GetMasterSvcHandle(ulActId, &hService);

		/* update meta service info */
		hErr = xmgr_setting_UpdateMetaInfoPID_Base(ulActId, hService);
		break;

	default:
		hErr = ERR_FAIL;
		break;
	}

	BUS_SendMessage(NULL, eMEVT_SVCLIST_SVC_PID_CHANGED, SVC_PIPE_GetActionHandle (ulActId), 0, 0, 0);
	return hErr;
}

STATIC BUS_Result_t xmgr_setting_msgAudioLanguageChanged_Base(void)
{
	HUINT32				ulActId = MGR_ACTION_GetMainActionId();
	Handle_t			hService;
	MgrAction_Type_e	eActionType;
	HERROR				hErr;

	/* reset user select for audio prioritising */
	DB_SVC_ResetAudioUserFlag();

	eActionType = MGR_ACTION_GetType(ulActId);
	switch(eActionType)
	{
	case eViewType_WTV:
		MGR_ACTION_GetMasterSvcHandle(ulActId, &hService);

		/* Service Info에 변경 사항을 적용 시킨다. */
		hErr = xmgr_setting_UpdateSvcInfoPID_Base(ulActId, hService);
		break;

	case eViewType_PB:
	case eViewType_TSR_PB:
	case eViewType_CHASE_PB:
		MGR_ACTION_GetMasterSvcHandle(ulActId, &hService);

		/* update meta service info */
		hErr = xmgr_setting_UpdateMetaInfoPID_Base(ulActId, hService);
		break;

	default:
		hErr = ERR_FAIL;
		break;
	}

	BUS_SendMessage(NULL, eMEVT_SVCLIST_SVC_PID_CHANGED, SVC_PIPE_GetActionHandle (ulActId), 0, 0, 0);
	return hErr;
}

STATIC BUS_Result_t xmgr_setting_msgSubtitleLanguageChanged_Base(void)
{
	HUINT32			ulActId = MGR_ACTION_GetMainActionId();
	Handle_t			hService;
	MgrAction_Type_e	eActionType;
	HERROR			hErr;

	/* reset user select for subtitle prioritising */
	DB_SVC_ResetSubtitleUserFlag();

	eActionType = MGR_ACTION_GetType(ulActId);
	switch(eActionType)
	{
	case eViewType_WTV:
		MGR_ACTION_GetMasterSvcHandle(ulActId, &hService);

		/* Service Info에 변경 사항을 적용 시킨다. */
		hErr = xmgr_setting_UpdateSvcInfoPID_Base(ulActId, hService);
		break;

	case eViewType_PB:
	case eViewType_TSR_PB:
	case eViewType_CHASE_PB:
		MGR_ACTION_GetMasterSvcHandle(ulActId, &hService);

		/* update meta service info */
		hErr = xmgr_setting_UpdateMetaInfoPID_Base(ulActId, hService);
		break;

	default:
		hErr = ERR_FAIL;
		break;
	}

	BUS_SendMessage(NULL, eMEVT_SVCLIST_SVC_PID_CHANGED, SVC_PIPE_GetActionHandle (ulActId), 0, 0, 0);
	return hErr;
}

STATIC BUS_Result_t xmgr_setting_msgAudioDescriptionChanged_Base(void)
{
	HUINT32			ulActId = MGR_ACTION_GetMainActionId();
	Handle_t			hService;
	MgrAction_Type_e	eActionType;
	HERROR			hErr;

	DB_SVC_ResetAudioUserFlag();

	eActionType = MGR_ACTION_GetType(ulActId);
	switch(eActionType)
	{
	case eViewType_WTV:
		MGR_ACTION_GetMasterSvcHandle(ulActId, &hService);

		/* Service Info에 변경 사항을 적용 시킨다. */
		hErr = xmgr_setting_UpdateSvcInfoPID_Base(ulActId, hService);
		break;

	case eViewType_PB:
	case eViewType_TSR_PB:
	case eViewType_CHASE_PB:
		MGR_ACTION_GetMasterSvcHandle(ulActId, &hService);

		/* update meta service info */
		hErr = xmgr_setting_UpdateMetaInfoPID_Base(ulActId, hService);
		break;

	default:
		hErr = ERR_FAIL;
		break;
	}

	BUS_SendMessage(NULL, eMEVT_SVCLIST_SVC_PID_CHANGED, SVC_PIPE_GetActionHandle (ulActId), 0, 0, 0);

	return hErr;
}

STATIC BUS_Result_t xmgr_setting_msgHardOfHearingChanged_Base(void)
{
	HUINT32			ulActId = MGR_ACTION_GetMainActionId();
	Handle_t			hService;
	MgrAction_Type_e	eActionType;
	HERROR			hErr;

	eActionType = MGR_ACTION_GetType(ulActId);
	switch(eActionType)
	{
	case eViewType_WTV:
		MGR_ACTION_GetMasterSvcHandle(ulActId, &hService);

		/* Service Info에 변경 사항을 적용 시킨다. */
		hErr = xmgr_setting_UpdateSvcInfoPID_Base(ulActId, hService);
		break;

	case eViewType_PB:
	case eViewType_TSR_PB:
	case eViewType_CHASE_PB:
		MGR_ACTION_GetMasterSvcHandle(ulActId, &hService);

		/* update meta service info */
		hErr = xmgr_setting_UpdateMetaInfoPID_Base(ulActId, hService);
		break;

	default:
		hErr = ERR_FAIL;
		break;
	}

	BUS_SendMessage(NULL, eMEVT_SVCLIST_SVC_PID_CHANGED, SVC_PIPE_GetActionHandle (ulActId), 0, 0, 0);

	return hErr;
}

STATIC BUS_Result_t xmgr_setting_msgItemChanged_Base(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t	eRes = ERR_BUS_NO_ERROR;

	NOT_USED_PARAM(message);
	NOT_USED_PARAM(hAction);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	switch(p1)
	{
	case eDB_PARAM_ITEM_OPERATOR_TYPE:
		HxLOG_Print("eDB_PARAM_ITEM_OPERATOR_TYPE \n");
		break;

	case eDB_PARAM_ITEM_LANG_MAIN:
		HxLOG_Print("eDB_PARAM_ITEM_LANG_MAIN \n");
		eRes = xmgr_setting_msgMainLanguageChanged_Base();
		break;

	case eDB_PARAM_ITEM_LANG_AUDIO0:
	case eDB_PARAM_ITEM_LANG_AUDIO1:
		HxLOG_Print("eDB_PARAM_ITEM_LANG_AUDIO \n");
		eRes = xmgr_setting_msgAudioLanguageChanged_Base();
		break;

	case eDB_PARAM_ITEM_LANG_SUBTITLE0:
	case eDB_PARAM_ITEM_LANG_SUBTITLE1:
		HxLOG_Print("eDB_PARAM_ITEM_LANG_SUBTITLE \n");
		eRes = xmgr_setting_msgSubtitleLanguageChanged_Base();
		break;

	case eDB_PARAM_ITEM_AUDIO_DESCRIPTION:
		HxLOG_Print("eDB_PARAM_ITEM_AUDIO_DESCRIPTION \n");
		eRes = xmgr_setting_msgAudioDescriptionChanged_Base();
		break;

	case eDB_PARAM_ITEM_HARDOFHEARING:
		HxLOG_Print("eDB_PARAM_ITEM_HARDOFHEARING \n");
		eRes = xmgr_setting_msgHardOfHearingChanged_Base();
		break;

	case eDB_PARAM_ITEM_PADDINGTIME_START:
	case eDB_PARAM_ITEM_PADDINGTIME_STOP:
		// implemented at xmgr_schedule_base.c.
		break;

	case eDB_PARAM_ITEM_HDMI_CEC_ENABLE:
		break;
	}

	return eRes;
}

STATIC BUS_Result_t	xproc_setting_Base(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t	eRes = ERR_BUS_NO_ERROR;

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		break;

	case eMEVT_BUS_DESTROY:
		break;

	case eSEVT_DB_PARAM_UPDATED:
		eRes = xmgr_setting_msgItemChanged_Base(message, hAction, p1, p2, p3);
		break;

	default:
		break;
	}

	return (eRes == MESSAGE_BREAK) ? MESSAGE_BREAK : MESSAGE_PASS;
}

/**************************************************************
	public functions
 **************************************************************/
HERROR xmgr_setting_Start_Base(void)
{
	BUS_Result_t	eRes;

	eRes = BUS_MGR_Create("xproc_Setting", APP_MONITORING_MGR_PRIORITY, xproc_setting_Base, 0, 0, 0, 0);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

