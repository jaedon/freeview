/**
	@file     xmgr_live_comp_base.c
	@brief    file_name & simple comment.

	Description: Applicatuion for searching Services\n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/**
  * Character encoding.
  *
  * MS949
  *
  * This source file that uses MS949 encoding.
  * MS949 encoding �� ����ϴ� source file �Դϴ�.
  * MS949 encoding�����Ī��� source file�Ǫ���
  * Quelldatei, die MS949-Codierung verwendet.
  *
**/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/


#include <mgr_common.h>
#include <octo_common.h>

#include <db_svc.h>

#include <svc_pipe.h>
#include <svc_cas.h>
#include <svc_si.h>
#if defined(CONFIG_MW_SUBTITLE)
#include <svc_sbtl.h>
#endif
#if defined(CONFIG_MW_TELETEXT)
#include <svc_ttx.h>
#endif

#include <mgr_pvr_action.h>
#include <mgr_live.h>
#include <mgr_action.h>

#if defined(CONFIG_3RD_BML)
#include <mgr_3rdparty.h>
#endif

#include <mgr_svclist.h>
#include <xmgr_live.h>
#include "../local_include/_xmgr_live.h"
#include "../local_include/_xmgr_live_comp.h"

/*******************************************************************/
/********************      Definition    ********************/
/*******************************************************************/
#define _____DEFINITIONS_____

#define CHECK_ACTION_ID()	\
	if(pstContext->ulActionId != ulActionId) \
	{ \
		return MESSAGE_PASS; \
	}

#define CHECK_ACTION_HANDLE() \
	CHECK_ACTION_ID() \
	if(SVC_PIPE_IsActionHandleLatest(hAction) != ERR_OK) \
	{ \
		HxLOG_Print("[AP:LiveComp(%d)] MESSAGE[0x%08X](0x%08x) is expired. Expected 0x%08X\n", ulActionId,message, hAction, pstContext->hAction); \
		return MESSAGE_PASS; \
	}


#define COMP_PARAM_WARNING_REMOVE(arg1, arg2, arg3, arg4, arg5)	\
			{													\
				(void)arg1;										\
				(void)arg2;										\
				(void)arg3;										\
				(void)arg4;				 						\
				(void)arg5;				 						\
			}

#define COMP_POINTER_INVALID_CHECK(pArg, returnValue)			\
			if(pArg == NULL)									\
			{													\
				HxLOG_Critical("\n\n");									\
				return returnValue;								\
			}

#define COMP_FCC_CHECK_MAINACTIONID(hAction)																							\
			if (_mgr_LiveComp_base_CheckMainActionId(hAction) != ERR_OK)																\
			{																															\
				HxLOG_Print("no main action id - curAid(%d) mainAid(%d)\n", SVC_PIPE_GetActionId(hAction), MGR_ACTION_GetMainActionId());	\
				return ERR_OK;																											\
			}


/*******************************************************************/
/********************      Local Variables     ********************/
/*******************************************************************/
#define _____LOCAL_VAR_____

STATIC xMGR_LiveComp_Context_t s_stXmgrLiveComp_Context;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Local Functions     ********************/
/*******************************************************************/

#define _____CONTEXT_FUNCTIONS_____

STATIC HERROR 	_mgr_LiveComp_base_StopMwService(xMGR_LiveComp_Context_t *pstContext, xMGR_Complementary_e eMwSvcType, ApiSyncMode_t eSync);
STATIC void 		_mgr_LiveComp_base_ChangePid_SvcDb(xMGR_LiveComp_Context_t *pstContext);

#if defined(CONFIG_MW_SUBTITLE)
STATIC void 		_mgr_LiveComp_base_CheckSbtlAllowed(xMGR_LiveComp_Context_t *pstContext);
STATIC HERROR	_mgr_LiveComp_base_StartSubtitle(xMGR_LiveComp_Context_t *pstContext);
STATIC HERROR 	_mgr_LiveComp_base_FindSubtitleInfo(xMGR_LiveComp_Context_t *pstContext, SvcSi_SubtitleInfo_t *subtitleInfo);
STATIC void 		_mgr_LiveComp_base_StopSubtitle(xMGR_LiveComp_Context_t *pstContext);
STATIC void 		_mgr_LiveComp_base_ChangeSubtitle(xMGR_LiveComp_Context_t *pstContext);
STATIC void 		_mgr_LiveComp_base_InitSubtitlePrivatePageID(xMGR_LiveComp_Context_t *pstContext);
#endif

STATIC HUINT8*	_mgr_LiveComp_base_GetComplemenaryTypeStr(xMGR_Complementary_e eMwSvcType);


#define _____CONTEXT_FUNCTIONS_____

INLINE STATIC xMGR_LiveComp_Context_t *_mgr_LiveComp_base_getContext (void)
{
	return &s_stXmgrLiveComp_Context;
}

STATIC HERROR _mgr_LiveComp_base_initContext (void)
{
	xMGR_LiveComp_Context_t			*pstContext = _mgr_LiveComp_base_getContext();

	HxSTD_memset (pstContext, 0, sizeof(xMGR_LiveComp_Context_t));

#if defined(CONFIG_MW_SUBTITLE)
	/* Invalid ǥ�ø� ���Ͽ� struct�� ��� 0xff�� �ʱ�ȭ */
	HxSTD_memset(&pstContext->subtitleInfo, 0xff, sizeof(SvcSi_SubtitleInfo_t));
#endif

	return ERR_OK;
}

STATIC HERROR _mgr_LiveComp_base_resetContext (void)
{
	return _mgr_LiveComp_base_initContext();
}

#define _____LOCAL_FUNCTIONS_____
STATIC HERROR _mgr_LiveComp_base_IsValidEpgEvtKind(xMGR_Complementary_e eMwSvcType, SvcEpg_EventKind_e eEvtKind)
{
	HERROR hErr = ERR_FAIL;

	switch(eMwSvcType)
	{
		case eComp_EpgPf:
			if ((eEvtKind == eEPG_EVT_KIND_PF_ALL)
			|| (eEvtKind == eEPG_EVT_KIND_PF_ACT)
			|| (eEvtKind == eEPG_EVT_KIND_PF_OTH))
			{
				hErr = ERR_OK;
			}
			break;

		case eComp_EpgSch:
			if ((eEvtKind == eEPG_EVT_KIND_SCH_ALL)
			|| (eEvtKind == eEPG_EVT_KIND_SCH_ACT)
			|| (eEvtKind == eEPG_EVT_KIND_SCH_OTH))
			{
				hErr = ERR_OK;
			}
			break;

		default:
			HxLOG_Error("invalid eMwSvcType(%s) \n", _mgr_LiveComp_base_GetComplemenaryTypeStr(eMwSvcType));
			hErr = ERR_FAIL;
			break;
	}

	return hErr;
}

STATIC HERROR _mgr_LiveComp_base_GetMwSvcTypeByEvtKind(SvcEpg_EventKind_e eEvtKind, xMGR_Complementary_e *peMwSvcType)
{
	if (peMwSvcType == NULL)
	{
		HxLOG_Error("invalid service handle !! \n");
		return ERR_FAIL;
	}

	switch(eEvtKind)
	{
		case eEPG_EVT_KIND_ALL:			//<<  ������ stop all �ϴ� ��쿡�� �����
		case eEPG_EVT_KIND_PF_ALL:
		case eEPG_EVT_KIND_PF_ACT:
		case eEPG_EVT_KIND_PF_OTH:
			*peMwSvcType = eComp_EpgPf;
			break;

		case eEPG_EVT_KIND_SCH_ALL:
		case eEPG_EVT_KIND_SCH_ACT:
		case eEPG_EVT_KIND_SCH_OTH:
			*peMwSvcType = eComp_EpgSch;
			break;

		case eEPG_EVT_KIND_PFEXT_OTH:
			*peMwSvcType = eComp_EpgPfExt;
			break;

		case eEPG_EVT_KIND_SCHEXT_OTH:
			*peMwSvcType = eComp_EpgSchExt;
			break;

		default:
			HxLOG_Error("invalid evtKind(%d) !! \n", eEvtKind);
			return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR _mgr_LiveComp_base_StartMwService(xMGR_LiveComp_Context_t *pstContext, xMGR_Complementary_e eMwSvcType)
{
	HERROR						hErr = ERR_FAIL;

	HxLOG_Print("hAction(0x%X) =%s= aeServiceState(%d)\n",
				pstContext->hAction,
				_mgr_LiveComp_base_GetComplemenaryTypeStr(eMwSvcType),
				pstContext->aeServiceState[eMwSvcType]);

	if (eMwSvcType >= eComp_MAX)
	{
		HxLOG_Error("invalid complementary type :0x%X\n", eMwSvcType);
		return ERR_FAIL;
	}

	if ((pstContext->aeServiceState[eMwSvcType] != eLiveMwSvcState_ReqStart)
		&& (pstContext->aeServiceState[eMwSvcType] != eLiveMwSvcState_Started))
	{
		switch (eMwSvcType)
		{
			case eComp_EpgContents:
				/* ���� xsvc_epg_StartContent_Base()�� �������� �ʾ� Ʋ�� ����� ��
				hErr = SVC_EPG_StartContent(pstContext->stEpgParam.eEpgKind, pstContext->hAction, pstContext->hSvc);
				if (hErr != ERR_OK)
				{
					HxLOG_Error ("SVC_EPG_StartContent()  failed.. \n");
				}
				*/
				break;

			case eComp_EpgPf:
			case eComp_EpgSch:
			case eComp_EpgPfExt:
			case eComp_EpgSchExt:
				//HxLOG_Debug("EPG ~\n");
				hErr = SVC_EPG_Start(pstContext->stEpgParam.eEpgKind,
									pstContext->stEpgParam.eEvtKind,
									pstContext->hAction, pstContext->hSvc,
									pstContext->stEpgParam.ulParam1, pstContext->stEpgParam.ulParam2, pstContext->stEpgParam.ulParam3);
				if (hErr != ERR_OK)
				{
					HxLOG_Error ("SVC_EPG_Start()  failed.. \n");
				}
				break;

			case eComp_Subtitle:
#if defined(CONFIG_MW_SUBTITLE)
				hErr = _mgr_LiveComp_base_StartSubtitle(pstContext);
				if (hErr != ERR_OK)
				{
					HxLOG_Error ("_mgr_LiveComp_base_StartSubtitle()  failed.. \n");
				}
#endif
				break;


			case eComp_Teletext:
#if defined(CONFIG_MW_TELETEXT)
				hErr = SVC_CAS_SetPid(pstContext->hAction,
							eCAS_ActionType_Live,
							PID_CURRENT,
							PID_CURRENT,
							PID_CURRENT,
							PID_CURRENT,
							DbSvc_GetTtxPid(&pstContext->stSvcInfo),
							PID_CURRENT);
				if (hErr != ERR_OK)
				{
					HxLOG_Error ("SVC_CAS_SetPid()  failed.. \n");
					break;
				}

				hErr = SVC_TTX_StartService(pstContext->hAction, DbSvc_GetTtxPid(&pstContext->stSvcInfo));
				if (hErr != ERR_OK)
				{
					HxLOG_Error ("SVC_TTX_StartService()  failed.. \n");
				}
#endif
				break;

			case eComp_CloseCaption:
				break;

			case eComp_AribDownload:
				break;

			case eComp_BML:
#if defined(CONFIG_3RD_BML)
				MGR_BML_Start();
#endif
				break;

			default:
				HxLOG_Error("invalid complementary type :0x%X\n", eMwSvcType);
				return ERR_FAIL;
		}

		// start success�� ��쿡�� start�� set state
		if (hErr == ERR_OK)
		{
			pstContext->aeServiceState[eMwSvcType] = eLiveMwSvcState_ReqStart;


			/**
			  *	����ó���� �ʿ��� �͵��� ���⿡��...
			**/
			// ...

		}
	}
	else
	{
		HxLOG_Debug("can't start =%s= \n", _mgr_LiveComp_base_GetComplemenaryTypeStr(eMwSvcType));
	}

	return ERR_OK;
}

STATIC HERROR _mgr_LiveComp_base_StopMwService(xMGR_LiveComp_Context_t *pstContext, xMGR_Complementary_e eMwSvcType, ApiSyncMode_t eSync)
{
	HERROR					hErr = ERR_FAIL;
	XmgrLive_MwSvcState_e	eMwSvcState = eLiveMwSvcState_ReqStop;

	HxLOG_Print ("hAction(0x%X) %s :: state(%d) \n",
				pstContext->hAction,
				_mgr_LiveComp_base_GetComplemenaryTypeStr(eMwSvcType),
				pstContext->aeServiceState[eMwSvcType]);

	if ((eLiveMwSvcState_ReqStart == pstContext->aeServiceState[eMwSvcType])
		||(eLiveMwSvcState_Started == pstContext->aeServiceState[eMwSvcType]))
	{
		switch (eMwSvcType)
		{
			case eComp_EpgContents:
				/* ������� �̰� �θ��� �κ��� �ƹ��� ������ ���߿� �ʿ��� ��츦 ����� ����� ��..
				hErr = SVC_EPG_StopContent(pstContext->stEpgParam.eEpgKind, pstContext->hAction);
				if (hErr != ERR_OK)
				{
					HxLOG_Error ("SVC_EPG_StopContent()  failed.. \n");
				}
				*/
				break;

			case eComp_EpgPf:
			case eComp_EpgSch:
			case eComp_EpgPfExt:
			case eComp_EpgSchExt:
				hErr = SVC_EPG_Stop(pstContext->stEpgParam.eEpgKind,
									pstContext->stEpgParam.eEvtKind,
									pstContext->hAction);
				if (hErr != ERR_OK)
				{
					HxLOG_Error ("SVC_EPG_Stop()  failed.. \n");
				}
				break;

			case eComp_Subtitle:
#if defined(CONFIG_MW_SUBTITLE)
				if (pstContext->subtitleInfo.eType == eSI_SUBTITLE_EBU)
				{
					if (PID_NULL == DbSvc_GetTtxPid(&pstContext->stSvcInfo))
					{
						hErr = SVC_TTX_StopService (pstContext->hAction, eSyncMode);
						if (ERR_OK != hErr)
						{
							HxLOG_Error("Error: \n");
						}
					}
					else
					{
						// TTX_PID�� ������� Stop ���� ���� ��쿡��,
						// hErr = ERR_OK�� �ؾ� �Ʒ����� Status�� ���� �ϰ�, �ٸ� SUBT�� START �� �� �ִ�.
						hErr = ERR_OK;
					}
				}
				else
				{
					hErr = SVC_SBTL_StopService (pstContext->hAction, eSync);
					if (hErr != ERR_OK)
					{
						HxLOG_Error ("SVC_SBTL_StopService()  failed.. \n");
					}
				}
#endif

				HxSTD_memset(&pstContext->subtitleInfo, 0xff, sizeof(SvcSi_SubtitleInfo_t));
				_mgr_LiveComp_base_InitSubtitlePrivatePageID(pstContext);
				break;

			case eComp_Teletext:
#if defined(CONFIG_MW_TELETEXT)
				hErr = SVC_TTX_StopService (pstContext->hAction, eSync);
				if (hErr != ERR_OK)
				{
					HxLOG_Error ("SVC_TTX_StopService()  failed.. \n");
				}
#endif
				break;

			case eComp_CloseCaption:
				break;


			case eComp_AribDownload:
				break;

			case eComp_BML:
#if defined(CONFIG_3RD_BML)
				MGR_BML_Stop();
#endif
				break;

			default:
				HxLOG_Error ("invalid eMwSvcType(%d)\n", eMwSvcType);
				break;
		}

		if (ERR_OK == hErr)
		{
			eMwSvcState = (eSyncMode == eSync) ? eLiveMwSvcState_Stopped : eLiveMwSvcState_ReqStop;
			pstContext->aeServiceState[eMwSvcType] = eMwSvcState;

			/**
			  *	����ó���� �ʿ��� �͵��� ���⿡��...
			**/
			switch(eMwSvcType)
			{
				// eEPG_EVT_KIND_ALL�� ��� ����ó��
				case eComp_EpgPf:
				case eComp_EpgSch:
				case eComp_EpgPfExt:
				case eComp_EpgSchExt:
					if (pstContext->stEpgParam.eEvtKind == eEPG_EVT_KIND_ALL)
					{
						pstContext->aeServiceState[eComp_EpgPf] 		= eMwSvcState;
						pstContext->aeServiceState[eComp_EpgSch] 		= eMwSvcState;
						pstContext->aeServiceState[eComp_EpgPfExt] 		= eMwSvcState;
						pstContext->aeServiceState[eComp_EpgSchExt] 	= eMwSvcState;
					}
					break;

				default:	/* nothing to do */
					break;
			}
		}
	}
	else
	{
		HxLOG_Debug("already stopped =%s= \n", _mgr_LiveComp_base_GetComplemenaryTypeStr(eMwSvcType));
	}

	return ERR_OK;
}

STATIC void _mgr_LiveComp_base_ChangePid_SvcDb(xMGR_LiveComp_Context_t *pstContext)
{
	HUINT32			ulActionId = pstContext->ulActionId;
	DbSvc_Info_t		newSvcInfo;
	HERROR			hErr = ERR_FAIL;

	HxLOG_Info ("[AP:LiveComp(%d)] _mgr_LiveComp_base_ChangePid_SvcDb()\n",ulActionId);

	/* Service info�� �ٽ� ã�� ��. PID�����̹Ƿ� actual service���. */
	HxSTD_memset(&newSvcInfo, 0, sizeof(DbSvc_Info_t));
	hErr = DB_SVC_GetServiceInfo(pstContext->hSvc, &newSvcInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("DB_SVC_GetServiceInfo error Handle(0x%x)\r\n", pstContext->hSvc);
	}

#if defined(CONFIG_MW_TELETEXT)
	/********************* Teletext PID ���� �˻� *********************/
	if(DbSvc_GetTtxPid(&pstContext->stSvcInfo) != DbSvc_GetTtxPid(&newSvcInfo))
	{
		DbSvc_SetTtxPid(&pstContext->stSvcInfo, DbSvc_GetTtxPid(&newSvcInfo));

		/* CAS�� PID�� �ٽ� �����ؾ� �Ѵ�. */
		hErr = SVC_CAS_SetPid(pstContext->hAction,
							eCAS_ActionType_Live,
							PID_CURRENT,
							PID_CURRENT,
							PID_CURRENT,
							PID_CURRENT,
							DbSvc_GetTtxPid(&newSvcInfo),
							PID_CURRENT);
		if (hErr)
		{
			HxLOG_Error("SVC_CAS_SetPid ret :0x%X\n",hErr);
		}


		hErr = SVC_TTX_StartService(pstContext->hAction, DbSvc_GetTtxPid(&newSvcInfo));
		if (hErr)
		{
			HxLOG_Error("SVC_TTX_StartService ret :0x%X\n",hErr);
		}

#if 0
		if(ulActionId == MGR_ACTION_GetMainActionId())
		{ /* Main action�� ��� teletext application�� ���ֳ� �˾ƺ��� �������� �׿��� ����� reset��Ų��. */
			if( BUS_MGR_Get(PROC_Teletext) != NULL )
			{
				BUS_MGR_Destroy(PROC_Teletext); /* Teletext appl�� �׿��ٰ� �ٽ� �����Ѵ�. */
				BUS_MGR_Create(NULL, APP_TELETEXT_PRIOTITY, PROC_Teletext, pstContext->hAction, 0, 0, 0);
			}
		}
#endif
	}
#endif

#if defined(CONFIG_MW_SUBTITLE)
	/********************* Subtitle Index ���� �˻� *********************/
	_mgr_LiveComp_base_CheckSbtlAllowed(pstContext);
	_mgr_LiveComp_base_ChangeSubtitle(pstContext);
#endif

	return;
}

#if defined(CONFIG_MW_SUBTITLE)
STATIC void _mgr_LiveComp_base_CheckSbtlAllowed(xMGR_LiveComp_Context_t *pstContext)
{
	HUINT32		ulActionId = pstContext->ulActionId;

	HxLOG_Info("[AP:LiveComp(%d)] _mgr_LiveComp_base_CheckSbtlAllowed()\n", ulActionId);


	if(pstContext->bSubtitleAllowed == TRUE)
	{
		HxLOG_Print("[AP:LiveComp(%d)] \t=> Subtitle already allowed.\n", ulActionId);
		return;
	}

	/* �ó����� 2 : service info���� subtitle index �� valid�ϸ� ����. */
	pstContext->bSubtitleAllowed = TRUE; /* DB������ dependent�ϹǷ� flag�� �׻� TRUE�� �ϸ� ��. */

	return;
}

HERROR _mgr_LiveComp_base_FindSubtitleInfo(xMGR_LiveComp_Context_t *pstContext, SvcSi_SubtitleInfo_t *subtitleInfo)
{
	HUINT32				ulActionId = pstContext->ulActionId;
	HUINT32				ulIndex = (HUINT32)DbSvc_GetSubttlIdx(&pstContext->stSvcInfo);
	HERROR				hErr;
	HUINT32				ulSubtitleNum = 0;

	SvcSi_SubtitleList_t stSubtitleList;

	HxLOG_Info("_mgr_LiveComp_base_FindSubtitleInfo() ++\n");

	HxSTD_MemSet(&stSubtitleList, 0x00, sizeof(SvcSi_SubtitleList_t));

#if 0 // �Ʒ����� ȣ���ϴ� SVC_SI_GetSubtitleList()���� pmt�� ����ϹǷ� ���� ���⼭ �̸� üũ�� �ʿ� ����..
	if(pstContext->bPmtValid != TRUE)
	{ /* PMT ���� ���� ��. */
		/* PMT�� �޾ƾ� subtitle information�� �� �� �ִ�. */
		HxLOG_Print("[AP:LiveComp(%d)] \t=> PMT is not arrived. Can't find subtitle list.\n", ulActionId);
		hErr = ERR_FAIL;
		goto END_FUNC;
	}
#endif

	/* SVC DB�� index validity �˻�. */
	if(ulIndex >= SvcSi_MAX_SUBTITLE_NUM)
	{
		HxLOG_Print("[AP:LiveComp(%d)] \t=> Invalid subtitle index(%d). No subtitle in SVC DB.\n", ulActionId, ulIndex);
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	/* SI�κ��� subtitle list�� ��´�. */
	hErr = SVC_SI_GetSubtitleList (ulActionId, DbSvc_GetSvcId(&pstContext->stSvcInfo), &stSubtitleList);
	ulSubtitleNum = stSubtitleList.ulSubtitleNum;
	if( (hErr == ERR_OK) && (ulSubtitleNum != 0) )
	{
		if(ulIndex > ulSubtitleNum)
		{
			/* �̷��� ����ѵ�... SVC DB�� �� PMT ������ ���̰� ����? */
			HxLOG_Print("[AP:LiveComp(%d)] \t=> Error ! PMT says # of subtitle info(%d), DB says index(%d)\n", ulActionId, ulSubtitleNum, ulIndex);
			hErr = ERR_FAIL;
			goto END_FUNC;
		}
		else
		{
			HxLOG_Print("[AP:LiveComp(%d)] \t=> Select [%d] element in %d subtitle lists.\n" , ulActionId,ulIndex, ulSubtitleNum);

			*subtitleInfo = stSubtitleList.pstSubtitleInfo[ulIndex];

			hErr = ERR_OK;
			goto END_FUNC;
		}
	}
	else
	{
		HxLOG_Print ("[AP:LiveComp(%d)] \t=> No subtitle information.\n", ulActionId);
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

END_FUNC:

	SVC_SI_FreeSubtitleList(&stSubtitleList);

	return hErr;
}

HERROR _mgr_LiveComp_base_StartSubtitle(xMGR_LiveComp_Context_t *pstContext)
{
	HUINT32				ulActionId = pstContext->ulActionId;
	SvcSi_SubtitleInfo_t		subtitleInfo;
	HERROR				hErr = ERR_FAIL;

	HxLOG_Info("[AP:LiveComp(%d)] StartSubtitle: SubtitleAllowed(%d), TtxPageEnable(%d) bStartSubtitle(%d) \n\n",ulActionId,
			pstContext->bSubtitleAllowed, pstContext->bTtxPageEnabled, pstContext->bStartSubtitle);

	if((pstContext->bSubtitleAllowed == TRUE)
		&& (pstContext->bTtxPageEnabled == FALSE)
		&& (pstContext->bStartSubtitle == TRUE))
	{
		/* Subtitle �㰡�ǰ� teletext ���°� �ƴϸ� subtitle activate�Ѵ�. */
		hErr = _mgr_LiveComp_base_FindSubtitleInfo(pstContext, &subtitleInfo);
		if(hErr == ERR_OK)
		{
			pstContext->subtitleInfo = subtitleInfo; /* Valid�� ������ �־ subtitle �����Ѵٰ� ǥ����... */

			if(subtitleInfo.eType == eSI_SUBTITLE_DVB)
			{ /* Subtitle DVB case */
				HxLOG_Print("[AP:LiveComp(%d)] \t=> Starting subtitle in DVB mode(PID:0x%X).\n", ulActionId, subtitleInfo.usPid);

				hErr = SVC_CAS_SetPid(pstContext->hAction,
									eCAS_ActionType_Live,
									PID_CURRENT,
									PID_CURRENT,
									PID_CURRENT,
									PID_CURRENT,
									PID_CURRENT,
									subtitleInfo.usPid);
				if (hErr)
				{
					HxLOG_Error("SVC_CAS_SetPid ret :0x%X\n",hErr);
				}


				if(pstContext->nDVBSbtCPageID != 0 || pstContext->nDVBSbtAPageID != 0)
				{
					hErr = SVC_SBTL_StartService(pstContext->hAction,
										subtitleInfo.usPid,
										pstContext->nDVBSbtCPageID,
										pstContext->nDVBSbtAPageID);
				}
				else
				{
					hErr = SVC_SBTL_StartService(pstContext->hAction,
										subtitleInfo.usPid,
										subtitleInfo.utArg.stDvbInfo.usCompositionPageId,
										subtitleInfo.utArg.stDvbInfo.usAncillaryPageId);
				}

				if (hErr)
				{
					HxLOG_Error("SVC_SBTL_StartService ret :0x%X\n",hErr);
				}

			}
			else if(subtitleInfo.eType == eSI_SUBTITLE_EBU)
			{ /* Subtitle EBU case */

				/*
				 * Subtitle index �� ����� ���,
				 * EBU Subtitle�� TTX PID �� �ɾ� �־�� ��.
				 *	 (TTX Page �� ���� ��� ���)
				 *	 (DB�� Update ���� �ʰ� �����, TTX���� ������ ����)
				 */
				hErr = SVC_CAS_SetPid(pstContext->hAction,
									eCAS_ActionType_Live,
									PID_CURRENT,
									PID_CURRENT,
									PID_CURRENT,
									PID_CURRENT,
									PID_CURRENT,
									subtitleInfo.usPid);
				if (hErr)
				{
					HxLOG_Error("SVC_CAS_SetPid ret :0x%X\n",hErr);
				}
#if defined(CONFIG_MW_TELETEXT)
				if (PID_NULL != subtitleInfo.usPid)
				{
					hErr = SVC_TTX_StartService (pstContext->hAction, subtitleInfo.usPid);
					if (hErr != ERR_OK)
					{
						HxLOG_Error ("SVC_TTX_StartService()  failed.. \n");
					}
				}
#endif
			}
			else
			{ /* ���� ������ �ȵǴµ�... */
				HxLOG_Critical("\n\n");
			}
		}
		else
		{
			HxSTD_memset(&pstContext->subtitleInfo, 0xff, sizeof(SvcSi_SubtitleInfo_t)); /* Invalid ǥ�ø� ���Ͽ� struct�� ��� 0xff�� �ʱ�ȭ */
		}
	}
	else
	{
		HxLOG_Print ("[AP:LiveComp(%d)] \t=> Subtitle is not allowed.\n", ulActionId);
	}

	return hErr;
}

void _mgr_LiveComp_base_StopSubtitle(xMGR_LiveComp_Context_t *pstContext)
{
	HERROR hErr;
	HUINT32				ulActionId = pstContext->ulActionId;

	HxLOG_Info("[AP:LiveComp(%d)] _mgr_LiveComp_base_StopSubtitle()\n", ulActionId);

	if((pstContext->bSubtitleAllowed == TRUE)
		&& (pstContext->bTtxPageEnabled == FALSE)
		&& (pstContext->bStartSubtitle == TRUE))
	{
		/* AV state �̸� Subtitle�� stop�Ѵ�. */
		if(pstContext->subtitleInfo.eType == eSI_SUBTITLE_DVB)
		{
			/* DVB mode ���� ���̸� stop��Ű�� state �ʱ�ȭ�Ѵ�. */
			_mgr_LiveComp_base_StopMwService(pstContext, eComp_Subtitle, eSyncMode);
		}
#if defined(CONFIG_MW_TELETEXT)
		else if(pstContext->subtitleInfo.eType == eSI_SUBTITLE_EBU)
		{
			if (PID_NULL == DbSvc_GetTtxPid(&pstContext->stSvcInfo))
			{
				hErr = SVC_TTX_StopService (pstContext->hAction, eSyncMode);
				if (ERR_OK != hErr)
					HxLOG_Error("Error: \n");
			}
		}
#endif
		else
		{
			/* NONE�̸� �׳� ������... */
		}

		/* stop�� �� ��� ���� subtitle ������ �����. */
		HxSTD_memset(&pstContext->subtitleInfo, 0xff, sizeof(SvcSi_SubtitleInfo_t));
		_mgr_LiveComp_base_InitSubtitlePrivatePageID(pstContext);
	}
}

void _mgr_LiveComp_base_ChangeSubtitle(xMGR_LiveComp_Context_t *pstContext)
{
	HUINT32				ulActionId = pstContext->ulActionId;
	DbSvc_Info_t			newSvcInfo;
	SvcSi_SubtitleInfo_t		newSubtitleInfo;
	HERROR				hErr;

	HxLOG_Info("[AP:LiveComp(%d)] _mgr_LiveComp_base_ChangeSubtitle()\n", ulActionId);

	/* New index�� �˾ƺ��� ���� Service info�� �ٽ� ã�� ��. */
	HxSTD_memset(&newSvcInfo, 0, sizeof(DbSvc_Info_t));
	hErr = DB_SVC_GetServiceInfo(pstContext->hSvc, &newSvcInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("DB_SVC_GetServiceInfo error Handle(0x%x)\r\n", pstContext->hSvc);
	}

	DbSvc_SetSubttlIdx(&pstContext->stSvcInfo, DbSvc_GetSubttlIdx(&newSvcInfo));

	if((pstContext->bSubtitleAllowed == TRUE)
		&& (pstContext->bTtxPageEnabled == FALSE)
		&& (pstContext->bStartSubtitle == TRUE))
	{
		/* New index�� new subtitle info�� �ٽ� ã�� ��. */
		hErr = _mgr_LiveComp_base_FindSubtitleInfo(pstContext, &newSubtitleInfo);

		if(hErr != ERR_OK)
		{
			/* Invalid ǥ�ø� ���Ͽ� struct�� ��� 0xff�� �ʱ�ȭ */
			HxSTD_memset(&newSubtitleInfo, 0xff, sizeof(SvcSi_SubtitleInfo_t));
		}

		if(HxSTD_memcmp(&newSubtitleInfo, &pstContext->subtitleInfo, sizeof(SvcSi_SubtitleInfo_t)) != 0)
		{
			//_mgr_LiveComp_base_StopSubtitle(pstContext); /* ���� �� stop ... */
			_mgr_LiveComp_base_StopMwService(pstContext, eComp_Subtitle, eSyncMode); /* ServiceState�� �ٲپ� �ֱ� ���� */

			_mgr_LiveComp_base_StartMwService(pstContext, eComp_Subtitle); /* ServiceState�� �ٲپ� �ֱ� ���� StartSubtitle()���� �ٲ�. */
		}
		else
		{
			if(pstContext->aeServiceState[eComp_Subtitle] == eLiveMwSvcState_ReqStart)
			{
				_mgr_LiveComp_base_StopSubtitle(pstContext); /* ���� �� stop ... */
				_mgr_LiveComp_base_StartMwService(pstContext, eComp_Subtitle); /* ServiceState�� �ٲپ� �ֱ� ���� StartSubtitle()���� �ٲ�. */
			}
		}
	}
}

STATIC void _mgr_LiveComp_base_InitSubtitlePrivatePageID(xMGR_LiveComp_Context_t *pstContext)
{
	pstContext->nEBUSbtMagNum = 0;
	pstContext->nEBUSbtPageNum = 0;
	pstContext->nDVBSbtCPageID = 0;
	pstContext->nDVBSbtAPageID = 0;
}

//eSEVT_SBTL_STOPPED
STATIC BUS_Result_t _mgr_LiveComp_base_EvtSbtlStopped(xMGR_LiveComp_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	COMP_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++


	HxLOG_Print("[AP:LiveComp(%d)] eSEVT_SBTL_STOPPED \n", ulActionId);

	//--------------------------------------------------------
 	COMP_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);

	return MESSAGE_BREAK;
}
#endif

#if defined(CONFIG_MW_TELETEXT)
//eSEVT_TTX_STOPPED
STATIC BUS_Result_t _mgr_LiveComp_base_EvtTtxStopped(xMGR_LiveComp_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	COMP_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/* nothing to do now */

	//--------------------------------------------------------
 	COMP_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);

	return MESSAGE_BREAK;
}
#endif

STATIC HERROR _mgr_LiveComp_base_CheckMainActionId(Handle_t hAction)
{
#if defined(CONFIG_SUPPORT_FCC)
	HUINT32		ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32		ulMainActionId = MGR_ACTION_GetMainActionId();

	if (ulActionId == ulMainActionId)
	{
		return ERR_OK;
	}

	return ERR_FAIL;
#else
	return ERR_OK;
#endif
}

/*******************************************************************/
/********************      Proc    ********************/
/*******************************************************************/
#define _____MSG_EVT_HANDLER_____

STATIC BUS_Result_t _mgr_LiveComp_base_MsgBusCreate(xMGR_LiveComp_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	MgrLive_START_t	*pstStart = (MgrLive_START_t *)p1;

	if (pstStart->eLiveMode != eMgrLiveMode_MainLive)
	{
		HxLOG_Debug("Support Livemode only!! cur(%d)\n", pstStart->eLiveMode);
		BUS_MGR_Destroy(0);
		return ERR_BUS_NO_ERROR;
	}

	COMP_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);

	// reset context
	_mgr_LiveComp_base_resetContext();

	// update context
	pstContext->ulActionId = SVC_PIPE_GetActionId(hAction);
	pstContext->hAction = hAction;
	pstContext->hSvc = pstStart->hService;

	COMP_PARAM_WARNING_REMOVE(pstContext, hAction, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t _mgr_LiveComp_base_MsgLiveStartSvc(xMGR_LiveComp_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	MgrLive_START_t *pstStart = (MgrLive_START_t *)p1;
        HERROR			 hErr = ERR_FAIL;

	if (pstStart->eLiveMode != eMgrLiveMode_MainLive)
	{
		HxLOG_Debug("Support Livemode only!! cur(%d)\n", pstStart->eLiveMode);
		BUS_MGR_Destroy(0);
		return ERR_BUS_NO_ERROR;
	}

	COMP_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);

	// update context
	pstContext->ulActionId = SVC_PIPE_GetActionId(hAction);
	pstContext->hAction = hAction;
	pstContext->hSvc = pstStart->hService;
	pstContext->bPmtValid = FALSE;

	HxSTD_memset(&pstContext->stSvcInfo, 0, sizeof(DbSvc_Info_t));
	hErr = DB_SVC_GetServiceInfo(pstContext->hSvc, &pstContext->stSvcInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("DB_SVC_GetServiceInfo error Handle(0x%x)\r\n", pstContext->hSvc);
	}

	COMP_PARAM_WARNING_REMOVE(pstContext, hAction, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t _mgr_LiveComp_base_EvtSiPmt(xMGR_LiveComp_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR		hErr = ERR_FAIL;

	if(MGR_ACTION_GetType(ulActionId) != eViewType_WTV)
		return MESSAGE_PASS;

	COMP_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	if(pstContext->bPmtValid != TRUE)
	{
		/* ù ��° PMT������ ��� ó��... */
		/* ���� PMT valid ǥ�ø� �ؾ� subtitle�� ������ �� �ִ�. */
		pstContext->bPmtValid = TRUE;

#if defined(CONFIG_MW_SUBTITLE)
		/* ó�� ���� PMT�� ��� subtitle�� start�Ѵ�. */
		_mgr_LiveComp_base_CheckSbtlAllowed(pstContext);
#endif
	}
	else
	{
	}

	// PMT�� �޾����� pid �˻縦 ����  - view�� curAppState�� ���鼭 ������ ���⿡�� ������ �� �Ѵ�
	/* AV/Teletext PID ���� �˻�. pstContext->svcInfo�� ���� update�ϸ� �ȵ�. �׷��� ���� Ȯ���� �ȵǴ� ���� Ȯ�� �� update�ϵ��� �ؾ� ��. */
	/* DB�� AV PID ������ �޽����� ó����. */
	_mgr_LiveComp_base_ChangePid_SvcDb(pstContext);
	hErr = DB_SVC_GetServiceInfo(pstContext->hSvc, &pstContext->stSvcInfo); /* New PMT �ݿ��� service info �ٽ� ����. */
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("DB_SVC_GetServiceInfo error Handle(0x%x)\r\n", pstContext->hSvc);
	}

	//--------------------------------------------------------
 	COMP_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t _mgr_LiveComp_base_MsgLiveStartComplementary(xMGR_LiveComp_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR					hErr = ERR_FAIL;
	xMGR_Complementary_e 	eMwSvcType = eComp_MAX;
	Handle_t 					hSvc = HANDLE_NULL;
	HUINT16					usTtxPid = 0;
	xMGR_Comp_EpgParam_T	*pstEpgParam = NULL;

	COMP_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);

	eMwSvcType = (xMGR_Complementary_e)p1;
	//HxLOG_Debug("Start : %s \n", _mgr_LiveComp_base_GetComplemenaryTypeStr(eMwSvcType));

	/**
	  *	complementary start�� �ʿ��� �������� context�� ������Ʈ �Ѵ�
	  *	������ start�� _mgr_LiveComp_base_StartMwService()���� �Ѵ�
	**/

	switch(eMwSvcType)
	{
		case eComp_EpgContents:
			pstEpgParam = (xMGR_Comp_EpgParam_T*)p2;
			if (pstEpgParam == NULL)
			{
				HxLOG_Error("ERROR ! epgParam is NULL  \n");
				return ERR_BUS_NO_ERROR;
			}

			if ((pstEpgParam->hSvc == HANDLE_NULL)
				|| (pstEpgParam->hSvc != pstContext->hSvc))
			{
				HxLOG_Error("ERROR ! different svcHandle (0x%x : 0x%X)..   \n", pstContext->hSvc, pstEpgParam->hSvc);
				return ERR_BUS_NO_ERROR;
			}

			HxSTD_memcpy(&pstContext->stEpgParam, pstEpgParam, sizeof(xMGR_Comp_EpgParam_T));
			break;

		case eComp_EpgPf:
		case eComp_EpgSch:
		case eComp_EpgPfExt:
		case eComp_EpgSchExt:
			pstEpgParam = (xMGR_Comp_EpgParam_T*)p2;
			if (pstEpgParam == NULL)
			{
				HxLOG_Error("ERROR ! epgParam is NULL  \n");
				return ERR_BUS_NO_ERROR;
			}

			// check epg kind
			if ((eMwSvcType == eComp_EpgPf)
				|| (eMwSvcType == eComp_EpgSch))
			{
				if (_mgr_LiveComp_base_IsValidEpgEvtKind(eMwSvcType, pstEpgParam->eEvtKind) != ERR_OK)
				{
					HxLOG_Error("Invalid evt kind - (%s : %d) \n",
								_mgr_LiveComp_base_GetComplemenaryTypeStr(eMwSvcType),
								pstEpgParam->eEvtKind);
					return ERR_BUS_NO_ERROR;
				}
			}

			// update epg params
			if ((pstEpgParam->hSvc == HANDLE_NULL)
				|| (pstEpgParam->hSvc != pstContext->hSvc))
			{
				HxLOG_Error("ERROR ! different svcHandle (0x%x : 0x%X)..   \n", pstContext->hSvc, pstEpgParam->hSvc);
				return ERR_BUS_NO_ERROR;
			}

			HxSTD_memcpy(&pstContext->stEpgParam, pstEpgParam, sizeof(xMGR_Comp_EpgParam_T));
			break;

		case eComp_Subtitle:
			pstContext->bStartSubtitle = TRUE;
			break;

		case eComp_Teletext:
			usTtxPid = (HUINT16)p2;
			if ((usTtxPid == PID_NULL)
				|| (usTtxPid != DbSvc_GetTtxPid(&pstContext->stSvcInfo)))
			{
				HxLOG_Error("ERROR ! different ttx pid (0x%x : 0x%X)..   \n", DbSvc_GetTtxPid(&pstContext->stSvcInfo), usTtxPid);
				return ERR_BUS_NO_ERROR;
			}
			break;

		case eComp_AribDownload:
			hSvc = (Handle_t)p2;
			if ((hSvc == HANDLE_NULL)
				|| (hSvc != pstContext->hSvc))
			{
				HxLOG_Error("ERROR ! different svcHandle (0x%x : 0x%X)..   \n", pstContext->hSvc, hSvc);
				return ERR_BUS_NO_ERROR;
			}
			break;

		/* Ư���� �� �� ���� ���� ����� ������ */
		case eComp_CloseCaption:
		case eComp_BML:
			/* nothing to do */
			break;

		default:
			HxLOG_Error("invalid complementary type (%s) \n", _mgr_LiveComp_base_GetComplemenaryTypeStr(eMwSvcType));
			return ERR_BUS_NO_ERROR;
	}

	hErr = _mgr_LiveComp_base_StartMwService(pstContext, eMwSvcType);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("_mgr_LiveComp_base_StartMwService() failed.. 0x%X\n", eMwSvcType , hErr);
	}

 	COMP_PARAM_WARNING_REMOVE(pstContext, hAction, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t _mgr_LiveComp_base_MsgLiveStopComplementary(xMGR_LiveComp_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR					hErr = ERR_FAIL;
	xMGR_Complementary_e 	eMwSvcType = eComp_MAX;
	ApiSyncMode_t 			eSync = eSyncMode;
	xMGR_Comp_EpgParam_T	*pstEpgParam = NULL;

	COMP_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);

	eMwSvcType = (xMGR_Complementary_e)p1;
	eSync = (ApiSyncMode_t)p2;
	HxLOG_Debug("Stop (%s)... sync(%d) \n", _mgr_LiveComp_base_GetComplemenaryTypeStr(eMwSvcType), eSync);

	/**
	  *	complementary stop�� �ʿ��� �������� context�� ������Ʈ �Ѵ�
	  *	������ stop�� _mgr_LiveComp_base_StopMwService()���� �Ѵ�
	**/

	switch(eMwSvcType)
	{
		case eComp_EpgContents:
			pstEpgParam = (xMGR_Comp_EpgParam_T*)p2;
			if (pstEpgParam == NULL)
			{
				HxLOG_Error("ERROR ! epgParam is NULL  \n");
				return ERR_BUS_NO_ERROR;
			}

			HxSTD_memcpy(&pstContext->stEpgParam, pstEpgParam, sizeof(xMGR_Comp_EpgParam_T));
			break;

		case eComp_EpgPf:
		case eComp_EpgSch:
		case eComp_EpgPfExt:
		case eComp_EpgSchExt:
			pstEpgParam = (xMGR_Comp_EpgParam_T *)p3;
			if (pstEpgParam == NULL)
			{
				HxLOG_Error("ERROR ! epgParam is NULL  \n");
				return ERR_BUS_NO_ERROR;
			}

			// check epg kind
			if ((eMwSvcType == eComp_EpgPf)
				|| (eMwSvcType == eComp_EpgSch))
			{
				if ((pstEpgParam->eEvtKind != eEPG_EVT_KIND_ALL) 		// kind all�� check�� �ʿ� ����
					&& (_mgr_LiveComp_base_IsValidEpgEvtKind(eMwSvcType, pstEpgParam->eEvtKind) != ERR_OK))
				{
					HxLOG_Error("Invalid evt kind - (%s : %d) \n",
								_mgr_LiveComp_base_GetComplemenaryTypeStr(eMwSvcType),
								pstEpgParam->eEvtKind);
					return ERR_BUS_NO_ERROR;
				}
			}

			// update epg params
			HxSTD_memcpy(&pstContext->stEpgParam, pstEpgParam, sizeof(xMGR_Comp_EpgParam_T));
			break;

		case eComp_Subtitle:
			pstContext->bStartSubtitle = FALSE;
			break;

		/* Ư���� �� �� ���� ���� ����� ������ */
		case eComp_Teletext:
		case eComp_CloseCaption:
		case eComp_AribDownload:
		case eComp_BML:
			/* nothing to do */
			break;

		default:
			HxLOG_Error("invalid complementary type (%s) \n", _mgr_LiveComp_base_GetComplemenaryTypeStr(eMwSvcType));
			return ERR_BUS_NO_ERROR;
	}

	hErr = _mgr_LiveComp_base_StopMwService(pstContext, eMwSvcType, eSync);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("_mgr_LiveComp_base_StopMwService() failed.. 0x%X\n", eMwSvcType , hErr);
	}

 	COMP_PARAM_WARNING_REMOVE(pstContext, hAction, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t _mgr_LiveComp_base_MsgMgrSvcPidChanged(xMGR_LiveComp_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(MGR_ACTION_GetType(ulActionId) != eViewType_WTV)
		return MESSAGE_PASS;

	COMP_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	_mgr_LiveComp_base_ChangePid_SvcDb(pstContext);

	//--------------------------------------------------------
 	COMP_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

#define _____PROTOTYPE_PROCEDURE_____
BUS_Result_t XMGR_LiveComp_BASE_ProtoProc (xMGR_LiveComp_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t		eBusResult = ERR_BUS_NO_ERROR;
	HUINT32				ulActionId = SVC_PIPE_GetActionId(hAction);

	switch (message)
	{
		case eMEVT_BUS_CREATE :
			HxLOG_Debug("[AP:LiveComp(%d)] MESSAGE : eMEVT_BUS_CREATE \n", ulActionId);
			_mgr_LiveComp_base_MsgBusCreate(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_LIVE_START_SVC:
			HxLOG_Debug("[AP:LiveComp(%d)] MESSAGE : eMEVT_LIVE_START_SVC \n", ulActionId);
			_mgr_LiveComp_base_MsgLiveStartSvc(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_LIVE_NOTIFY_SVC_CHANGED:
			if (ulActionId == pstContext->ulActionId)
			{
				// view proc�� ���۵Ǹ鼭 action session version�� ���������Ƿ� ���⼭ ������Ʈ ���ش�
				pstContext->hAction = hAction;
			}
			break;

		case eMEVT_LIVE_START_COMPLEMENTARY:
			CHECK_ACTION_ID();
			//HxLOG_Debug("[AP:LiveComp(%d)] MESSAGE : eMEVT_LIVE_START_COMPLEMENTARY \n", ulActionId);
			eBusResult = _mgr_LiveComp_base_MsgLiveStartComplementary(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_LIVE_STOP_COMPLEMENTARY:
			CHECK_ACTION_ID();
			//HxLOG_Debug("[AP:LiveComp(%d)] MESSAGE : eMEVT_LIVE_START_COMPLEMENTARY \n", ulActionId);
			eBusResult = _mgr_LiveComp_base_MsgLiveStopComplementary(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_PMT :
			CHECK_ACTION_HANDLE();
			HxLOG_Debug("[AP:LiveComp(%d)] MESSAGE : eSEVT_SI_PMT\n", ulActionId);
			eBusResult = _mgr_LiveComp_base_EvtSiPmt(pstContext, ulActionId, p1, p2, p3);
			break;

#if defined(CONFIG_MW_SUBTITLE)
		/**********************************************************************
			Subtitle ���� �޶�� ��û �̺�Ʈ.
		***********************************************************************/
		case eSEVT_SBTL_STOPPED:
			CHECK_ACTION_HANDLE();
			HxLOG_Debug("[AP:LiveComp(%d)] MESSAGE : eSEVT_SBTL_STOPPED\n", ulActionId);
			eBusResult = _mgr_LiveComp_base_EvtSbtlStopped(pstContext, ulActionId, p1, p2, p3);
			break;
#endif

#if defined(CONFIG_MW_TELETEXT)
		case eSEVT_TTX_STOPPED:
			CHECK_ACTION_HANDLE();
			HxLOG_Debug("[AP:LiveComp(%d)] MESSAGE : eSEVT_TTX_STOPPED\n", ulActionId);
			eBusResult = _mgr_LiveComp_base_EvtTtxStopped(pstContext, ulActionId, p1, p2, p3);
			break;
#endif /* #if defined(CONFIG_MW_TELETEXT) */

		case eMEVT_SVCLIST_SVC_PID_CHANGED :
			CHECK_ACTION_ID();
			HxLOG_Debug ("[AP:LiveComp(%d)] MESSAGE : eMEVT_SVCLIST_SVC_PID_CHANGED\n", ulActionId);
			eBusResult = _mgr_LiveComp_base_MsgMgrSvcPidChanged(pstContext, ulActionId, p1, p2, p3);
			break;


		case eMEVT_BUS_DESTROY :
			HxLOG_Debug("[AP:LiveComp(%d)] MESSAGE : eMEVT_BUS_DESTROY\n",pstContext->ulActionId);
#if defined(CONFIG_MW_SUBTITLE)
			_mgr_LiveComp_base_MsgLiveStopComplementary(pstContext, hAction, eComp_Subtitle, eSyncMode, p3);
#endif
			break;
	}

	if( (eBusResult == MESSAGE_BREAK) || (eBusResult == MESSAGE_PASS) )
	{
		return eBusResult;
	}
	else
	{
		return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
	}
}

STATIC BUS_Result_t  _mgr_LiveComp_base_proc (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xMGR_LiveComp_Context_t			*pstContext = _mgr_LiveComp_base_getContext();

	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
	}

	return XMGR_LiveComp_BASE_ProtoProc (pstContext, message, hHandle, p1, p2, p3);
}

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
#define _____GLOBAL_FUNCTIONS_____

BUS_Callback_t XMGR_LiveComp_BASE_GetProc(void)
{
	BUS_Callback_t			 pfLiveProc = NULL;

	pfLiveProc = _mgr_LiveComp_base_proc;

	return pfLiveProc;
}

HERROR XMGR_LiveComp_BASE_Init (void)
{
	return _mgr_LiveComp_base_initContext();
}

HERROR XMGR_LiveComp_BASE_StartEpgContent(SvcEpg_EpgKind_e eEpgKind, Handle_t hAction, Handle_t hService)
{
#if 0	// ���� epg context mgr�ʿ� epg contents ������ �����Ǿ� ���� �ʾ� ����
	xMGR_Comp_EpgParam_T		stEpgParam;

	if (hService == HANDLE_NULL)
	{
		HxLOG_Error("invalid service handle !! \n");
		return ERR_FAIL;
	}

	HxSTD_memset(&stEpgParam, 0x00, sizeof(xMGR_Comp_EpgParam_T));
	stEpgParam.hSvc = hService;
	stEpgParam.eEpgKind = eEpgKind;

	BUS_SendMessage(NULL, eMEVT_LIVE_START_COMPLEMENTARY, hAction, (HINT32)eComp_EpgContents, (HINT32)&stEpgParam, 0);
#endif

	return ERR_OK;
}

HERROR XMGR_LiveComp_BASE_StartEpg(SvcEpg_EpgKind_e eEpgKind, SvcEpg_EventKind_e eEvtKind, Handle_t hAction, Handle_t hService, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	xMGR_Comp_EpgParam_T		stEpgParam;
	xMGR_Complementary_e 		eMwSvcType = eComp_MAX;

	COMP_FCC_CHECK_MAINACTIONID(hAction);

	if (hService == HANDLE_NULL)
	{
		HxLOG_Error("invalid service handle !! \n");
		return ERR_FAIL;
	}

	HxSTD_memset(&stEpgParam, 0x00, sizeof(xMGR_Comp_EpgParam_T));
	stEpgParam.hSvc 		= hService;
	stEpgParam.eEpgKind 	= eEpgKind;
	stEpgParam.eEvtKind 	= eEvtKind;
	stEpgParam.ulParam1 	= ulParam1;
	stEpgParam.ulParam2 	= ulParam2;
	stEpgParam.ulParam3 	= ulParam3;

	if (_mgr_LiveComp_base_GetMwSvcTypeByEvtKind(eEvtKind, &eMwSvcType) != ERR_OK)
	{
		HxLOG_Error("invalid eEvtKind(%d) !! \n", eEvtKind);
		return ERR_FAIL;
	}

	BUS_SendMessage(NULL, eMEVT_LIVE_START_COMPLEMENTARY, hAction, (HINT32)eMwSvcType, (HINT32)&stEpgParam, 0);
	return ERR_OK;
}

HERROR XMGR_LiveComp_BASE_StopEpg(SvcEpg_EpgKind_e eEpgKind, SvcEpg_EventKind_e eEvtKind, Handle_t hAction)
{
	xMGR_Comp_EpgParam_T		stEpgParam;
	xMGR_Complementary_e 		eMwSvcType = eComp_MAX;

	COMP_FCC_CHECK_MAINACTIONID(hAction);

	HxSTD_memset(&stEpgParam, 0x00, sizeof(xMGR_Comp_EpgParam_T));
	stEpgParam.eEpgKind = eEpgKind;
	stEpgParam.eEvtKind = eEvtKind;

	if (_mgr_LiveComp_base_GetMwSvcTypeByEvtKind(eEvtKind, &eMwSvcType) != ERR_OK)
	{
		HxLOG_Error("_mgr_LiveComp_base_GetMwSvcTypeByEvtKind() failed.. \n");
		return ERR_FAIL;
	}

	BUS_SendMessage(NULL, eMEVT_LIVE_STOP_COMPLEMENTARY, hAction, (HINT32)eMwSvcType, eAsyncMode, (HINT32)&stEpgParam);
	return ERR_OK;
}

HERROR XMGR_LiveComp_BASE_StartSubtitle(Handle_t hAction)
{
	HERROR hErr = ERR_FAIL;

	COMP_FCC_CHECK_MAINACTIONID(hAction);

#if defined(CONFIG_MW_SUBTITLE)
	hErr = BUS_SendMessage(NULL, eMEVT_LIVE_START_COMPLEMENTARY, hAction, (HINT32)eComp_Subtitle, 0, 0);
#endif

	return hErr;
}

HERROR XMGR_LiveComp_BASE_StopSubtitle(Handle_t hAction, ApiSyncMode_t eSync)
{
	HERROR hErr = ERR_FAIL;

	COMP_FCC_CHECK_MAINACTIONID(hAction);

#if defined(CONFIG_MW_SUBTITLE)
	hErr = BUS_SendMessage(NULL, eMEVT_LIVE_STOP_COMPLEMENTARY, hAction, (HINT32)eComp_Subtitle, (HINT32)eSync, 0);
#endif

	return hErr;
}

HERROR XMGR_LiveComp_BASE_StartCloseCaption(Handle_t hAction)
{
	HERROR hErr = ERR_FAIL;

	return hErr;
}

HERROR XMGR_LiveComp_BASE_StopCloseCaption(Handle_t hAction, ApiSyncMode_t eSync)
{
	HERROR hErr = ERR_FAIL;

	return hErr;
}

HERROR XMGR_LiveComp_BASE_StartTeletext(Handle_t hAction, HUINT16 usTtxPid)
{
	HERROR hErr = ERR_FAIL;

	COMP_FCC_CHECK_MAINACTIONID(hAction);

#if defined(CONFIG_MW_TELETEXT)
	hErr = BUS_SendMessage(NULL, eMEVT_LIVE_START_COMPLEMENTARY, hAction, (HINT32)eComp_Teletext, (HINT32)usTtxPid, 0);
#endif

	return hErr;
}

HERROR XMGR_LiveComp_BASE_StopTeletext(Handle_t hAction, ApiSyncMode_t eSync)
{
	HERROR hErr = ERR_FAIL;

	COMP_FCC_CHECK_MAINACTIONID(hAction);

#if defined(CONFIG_MW_TELETEXT)
	hErr = BUS_SendMessage(NULL, eMEVT_LIVE_STOP_COMPLEMENTARY, hAction, (HINT32)eComp_Teletext, (HINT32)eSync, 0);
#endif

	return hErr;
}

HERROR XMGR_LiveComp_BASE_StartAribDownload(Handle_t hAction, Handle_t hSvc)
{
	HERROR hErr = ERR_FAIL;

	return hErr;
}

HERROR XMGR_LiveComp_BASE_StopAribDownload(Handle_t hAction, ApiSyncMode_t eSync)
{
	HERROR hErr = ERR_FAIL;

	return hErr;
}


HERROR XMGR_LiveComp_BASE_StartBML(Handle_t hAction)
{
	HERROR hErr = ERR_FAIL;

	COMP_FCC_CHECK_MAINACTIONID(hAction);

#if defined(CONFIG_3RD_BML)
	hErr = BUS_SendMessage(NULL, eMEVT_LIVE_START_COMPLEMENTARY, hAction, (HINT32)eComp_BML, 0, 0);
#endif

	return hErr;
}

HERROR XMGR_LiveComp_BASE_StopBML(Handle_t hAction, ApiSyncMode_t eSync)
{
	HERROR hErr = ERR_FAIL;

	COMP_FCC_CHECK_MAINACTIONID(hAction);

#if defined(CONFIG_3RD_BML)
	hErr = BUS_SendMessage(NULL, eMEVT_LIVE_STOP_COMPLEMENTARY, hAction, (HINT32)eComp_BML, (HINT32)eSync, 0);
#endif

	return hErr;
}


/*******************************************************************/
/********************      Debug Functions     ********************/
/*******************************************************************/
#define _____DEBUG_FUNCTIONS_____

STATIC HUINT8 *_mgr_LiveComp_base_GetComplemenaryTypeStr(xMGR_Complementary_e eMwSvcType)
{
	switch(eMwSvcType)
	{
		ENUM_TO_STR(eComp_EpgContents)
		ENUM_TO_STR(eComp_EpgPf)
		ENUM_TO_STR(eComp_EpgSch)
		ENUM_TO_STR(eComp_Subtitle)
		ENUM_TO_STR(eComp_Teletext)
		ENUM_TO_STR(eComp_CloseCaption)
		ENUM_TO_STR(eComp_AribDownload)
		ENUM_TO_STR(eComp_BML)
		default:			break;
	}

	HxLOG_Print("Unkown mw service type [%d]\n", eMwSvcType);

	return "UNKNOWN";
}


/* End Of File */
