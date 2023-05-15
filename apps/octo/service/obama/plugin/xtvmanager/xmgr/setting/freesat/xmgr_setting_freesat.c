/**************************************************************
 *	@file		ap_setting_change.c
 *
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/02/16
 *	@author			humax
 *	@brief			menu setting change action
 **************************************************************/
#include	<hlib.h>

#include	<octo_common.h>

#include	<db_param.h>

#include 	<svc_si.h>

#include	<bus.h>
#include	<mgr_common.h>
#include	<mgr_svclist.h>
#include	<mgr_action.h>



/**************************************************************
	internal structures and variables
 **************************************************************/
static HBOOL	s_bRsvLoadingDone = FALSE;

/**************************************************************
	private functions
 **************************************************************/
STATIC HERROR xmgr_setting_UpdateSvcInfoPID_Freesat(HUINT32 ulActionId, Handle_t hService)
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
		// ���� db�� �����ϰ� �ִ� service info�� ��������
		hErr = DB_SVC_GetServiceInfo(hService, pstOrgSvcInfo);
		if (ERR_FAIL == hErr)
		{
			HxLOG_Error("DB_SVC_GetServiceInfo() failed.. : hService(0x%X)", hService);
			goto EXIT_FUNC;
		}

		// si table���� svc info �� �������� ���� svc id�� ��������
		hErr = DB_SVC_GetServiceIdFromSvcHandle (hService, &usSvcId);
		if (ERR_FAIL == hErr)
		{
			HxLOG_Error("DB_SVC_GetServiceIdFromSvcHandle() failed.. : hService(0x%X)", hService);
			goto EXIT_FUNC;
		}

		// si raw���� svc info�� �����´�
		hErr = MGR_LIVE_UpdateServiceInfo (hAction, (HUINT32)usSvcId, pstOrgSvcInfo, pstDstSvcInfo, &bChanged);
		HxLOG_Info("pstOrgSvcInfo!=pstDstSvcInfo,  bChanged: %d \n", bChanged);
		if (ERR_FAIL == hErr)
		{
			HxLOG_Error("MGR_LIVE_UpdateServiceInfo() failed.. : hService(0x%X)", hService);
			goto EXIT_FUNC;
		}

		// �� svc info�� ���ؼ� �ٸ��� db�� ������Ʈ �Ѵ�
		if (TRUE == bChanged)
		{
			hErr = DB_SVC_UpdateServiceInfo (hService, pstDstSvcInfo);
			if (ERR_FAIL == hErr)
			{
				HxLOG_Error("DB_SVC_UpdateServiceInfo() failed.. : hService(0x%X)", hService);
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

BUS_Result_t xmgr_setting_msgMainLanguageChanged_Freesat(void)
{
	HUINT32		ulActId = MGR_ACTION_GetMainActionId();
	Handle_t		hService;

	//SVC_SYS_SetLanguage(sz639Code[0]);

	DB_SVC_ResetAudioUserFlag();
	DB_SVC_ResetSubtitleUserFlag();

	MGR_ACTION_GetMasterSvcHandle(ulActId, &hService);
	/* Service Info�� ���� ������ ���� ��Ų��. */
	xmgr_setting_UpdateSvcInfoPID_Freesat(ulActId, hService);

	BUS_SendMessage(NULL, eMEVT_SVCLIST_SVC_PID_CHANGED, SVC_PIPE_GetActionHandle (ulActId), 0, 0, 0);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t xmgr_setting_msgAudioLanguageChanged_Freesat(void)
{
	HUINT32			ulActId = MGR_ACTION_GetMainActionId();
	Handle_t		hService;
	MgrAction_Type_e	eActionType;
	HERROR			hErr;

	DB_SVC_ResetAudioUserFlag();

	eActionType = MGR_ACTION_GetType(ulActId);
	switch(eActionType)
	{
		case eViewType_WTV:
			MGR_ACTION_GetMasterSvcHandle(ulActId, &hService);

			/* Service Info�� ���� ������ ���� ��Ų��. */
			hErr = xmgr_setting_UpdateSvcInfoPID_Freesat(ulActId, hService);
			break;

#if defined(CONFIG_MW_MM_PVR)
		case eViewType_CHASE_PB:
		case eViewType_TSR_PB:
		case eViewType_PB:
			hErr = xmgr_pvr_UpdatePid(SVC_PIPE_GetActionHandle(ulActId));
			break;
#endif
		default:
			break;
	}

	BUS_SendMessage(NULL, eMEVT_SVCLIST_SVC_PID_CHANGED, SVC_PIPE_GetActionHandle (ulActId), 0, 0, 0);
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t xmgr_setting_msgSubtitleLanguageChanged_Freesat(void)
{
	HUINT32			ulActId = MGR_ACTION_GetMainActionId();
	Handle_t			hService;
	MgrAction_Type_e	eActionType;
	HERROR			hErr;

	DB_SVC_ResetSubtitleUserFlag();

	eActionType = MGR_ACTION_GetType(ulActId);
	switch(eActionType)
	{
		case eViewType_WTV:
			MGR_ACTION_GetMasterSvcHandle(ulActId, &hService);

			/* Service Info�� ���� ������ ���� ��Ų��. */
			hErr = xmgr_setting_UpdateSvcInfoPID_Freesat(ulActId, hService);
			break;

#if defined(CONFIG_MW_MM_PVR)
		case eViewType_CHASE_PB:
		case eViewType_TSR_PB:
		case eViewType_PB:
			hErr = xmgr_pvr_UpdatePid(SVC_PIPE_GetActionHandle(ulActId));
			break;
#endif
		default:
			break;
	}

	BUS_SendMessage(NULL, eMEVT_SVCLIST_SVC_PID_CHANGED, SVC_PIPE_GetActionHandle (ulActId), 0, 0, 0);
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t xmgr_setting_msgAudioDescriptionChanged_Freesat(void)
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

			/* Service Info�� ���� ������ ���� ��Ų��. */
			hErr = xmgr_setting_UpdateSvcInfoPID_Freesat(ulActId, hService);
			break;

#if defined(CONFIG_MW_MM_PVR)
		case eViewType_CHASE_PB:
		case eViewType_TSR_PB:
		case eViewType_PB:
			hErr = xmgr_pvr_UpdatePid(SVC_PIPE_GetActionHandle(ulActId));
			break;
#endif
		default:
			break;
	}

	BUS_SendMessage(NULL, eMEVT_SVCLIST_SVC_PID_CHANGED, SVC_PIPE_GetActionHandle (ulActId), 0, 0, 0);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t xmgr_setting_msgPaddingTimeChanged_Freesat(void)
{
	if(s_bRsvLoadingDone == TRUE)
	{
		MW_RSV_ChangeRsvPaddingTime();
	}

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t xmgr_setting_msgItemChanged_Freesat(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t	eRes = ERR_BUS_NO_ERROR;

	NOT_USED_PARAM(message);
	NOT_USED_PARAM(hAction);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	switch(p1)
	{
		case eDB_PARAM_ITEM_OPERATOR_TYPE:
			break;

		case eDB_PARAM_ITEM_LANG_MAIN:
			eRes = xmgr_setting_msgMainLanguageChanged_Freesat();
			break;

		case eDB_PARAM_ITEM_LANG_AUDIO0:
			eRes = xmgr_setting_msgAudioLanguageChanged_Freesat();
			break;

		case eDB_PARAM_ITEM_LANG_SUBTITLE1:
			eRes = xmgr_setting_msgSubtitleLanguageChanged_Freesat();
			break;

		case eDB_PARAM_ITEM_AUDIO_DESCRIPTION:
			eRes = xmgr_setting_msgAudioDescriptionChanged_Freesat();
			break;

		case eDB_PARAM_ITEM_PADDINGTIME_START:
		case eDB_PARAM_ITEM_PADDINGTIME_STOP:
			eRes = xmgr_setting_msgPaddingTimeChanged_Freesat();
			break;
		case eDB_PARAM_ITEM_HDMI_CEC_ENABLE:
			break;

		case eDB_PARAM_ITEM_POSTCODE:
			break;
	}

	return eRes;
}

STATIC BUS_Result_t	xproc_setting_Freesat(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t	eRes = ERR_BUS_NO_ERROR;

	switch (message)
	{
		case eMEVT_BUS_CREATE:
			break;

		case eMEVT_BUS_DESTROY:
			break;

		case eSEVT_DB_PARAM_UPDATED:
			eRes = xmgr_setting_msgItemChanged_Freesat(message, hAction, p1, p2, p3);
			break;

		case EVT_RSV_DB_LOADING_DONE:
			s_bRsvLoadingDone = TRUE;
			break;

		default:
			break;
	}

	return (eRes == MESSAGE_BREAK) ? MESSAGE_BREAK : MESSAGE_PASS;
}

/**************************************************************
	public functions
 **************************************************************/
HERROR xmgr_setting_Start_Freesat(void)
{
	BUS_Result_t	eRes;

	eRes = BUS_MGR_Create("xproc_Setting", APP_MONITORING_MGR_PRIORITY, xproc_setting_Freesat, 0, 0, 0, 0);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

#include <xmgr_epg.h>

HERROR	xmgr_spec_DefaultEnvironment_Freesat(HBOOL bKeepPrevious)
{
	return XMGR_FSAT_DefaultMetaDataFolder();
}

