/**
	@file     file_name.c File Name을 적습니다.
	@brief    file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <db_param.h>
#include <svc_si.h>

#include <bus.h>

#include <mgr_action.h>
#include <mgr_live.h>

#include <svc_pipe.h>
#include <svc_sys.h>
#include <svc_epg.h>
#include <svc_fs.h>
#include <svc_resources.h>
#include <mgr_rscmap.h>
#include <mgr_storage.h>

#include <mgr_epg.h>

#include <otl.h>
#include <sapi.h>
#include <hapi.h>

#if	defined(CONFIG_MW_EPG_TVTV)

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

//#define SUPPORT_VFD			// current progress percent is display at vfd

#define TVTVACTION_TIMER_ID					0x03240315
#define TVTVACTION_TIMER_TIME					(80*60*1000)

#define TVTVLOCK_TIMER_ID						0x08310230
#define TVTVLOCK_TIMER_TIME					(10*1000)


#define TVTVHDD_TIMER_ID						0x09090340
#define TVTVHDD_TIMER_TIME					(1*1000)

#define TVTV_TUNE_INFO_FREQ					12603
#define TVTV_TUNE_INFO_SR						22000
#define TVTV_TUNE_INFO_CODERATE				(eDxSAT_CODERATE_AUTO)
#define TVTV_TUNE_INFO_POLARIZATION			(eDxSAT_POLAR_AUTO)
#define TVTV_TUNE_INFO_TRANS					(eDxSAT_TRANS_DVBS)
#define TVTV_TUNE_INFO_PSK					(eDxSAT_PSK_QPSK)
#define TVTV_TUNE_INFO_PILOT					(eDxSAT_PILOT_AUTO)

#define TVTV_SAT_SVC_ID						0x1C2C
#define TVTV_SAT_TS_ID						0x0457
#define TVTV_SAT_ON_ID						0x0001


#define 	DxDB_APPL_TVTV_LASTUPDATETIME				"tvtvlastupdatetime"



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct tagTvtvActannelContent
{
	MgrEpg_CallType_e	eCallType;

	HBOOL				bChLocked;
	HBOOL				bComplete;
	HUINT32				ulActionId;
	Handle_t				hAction;

	Handle_t				hTvtvService;
	DbSvc_TripleId_t		stTvtvTripleId;
	DxTuneParam_t		stTuningInfo;

}TvtvAction_Content_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static TvtvAction_Content_t		s_stTvtvActionContent = {0, };



/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/
STATIC HERROR 					xmgr_tvtvaction_SetLastUpdateTime(void);
STATIC HERROR 					xmgr_tvtvaction_IsConflict(void);
STATIC BUS_Result_t				xmgr_tvtvaction_GetContent(TvtvAction_Content_t **ppContent);
STATIC BUS_Result_t				xmgr_tvtvaction_InitApp(void);
STATIC BUS_Result_t				xmgr_tvtvaction_StartTvtvService(void);
STATIC BUS_Result_t				xmgr_tvtvaction_TuneTvtvService (void);
STATIC BUS_Result_t				xmgr_tvtvaction_DestroyApp(BUS_DestroyReason_t actionReason);
#if defined(SUPPORT_VFD)
STATIC BUS_Result_t				xmgr_tvtvaction_DisplayProgress(HINT32 lParam);
#endif
STATIC BUS_Result_t				xmgr_tvtvaction_ConvertAntennaTunningInfo(DbSvc_AntInfo_t *pAntInfo, DxTuneParam_t *pChTuningParam);

/*******************************************************************/
/********************      API Functions   *************************/
/*******************************************************************/
#if defined (CONFIG_DEBUG)
HUINT8 *xproc_GetTvtvActiionMessage(HINT32 message)
{
	switch(message)
	{
		ENUM_TO_STR(eMEVT_BUS_CREATE);
		ENUM_TO_STR(eSEVT_EPG_TVTV_STOP_ACTION);
		//ENUM_TO_STR(eMEVT_BUS_TIMER);
		ENUM_TO_STR(eSEVT_CH_NO_SIGNAL);
		ENUM_TO_STR(eSEVT_CH_LOCKED);
#if defined(SUPPORT_VFD)
		ENUM_TO_STR(eSEVT_EPG_TVTV_PROGRESS);
#endif
		ENUM_TO_STR(eSEVT_EPG_TVTV_DONE);
		//ENUM_TO_STR(eMEVT_BUS_GO_SHUTDOWN);
		ENUM_TO_STR(eMEVT_BUS_READY_SHUTDOWN);
		ENUM_TO_STR(eMEVT_BUS_GO_OPERATION);
		ENUM_TO_STR(eMEVT_STORAGE_FORMAT_DEVICE);
		ENUM_TO_STR(eMEVT_BUS_DESTROY);
		default:
			return NULL;
	}
}
#endif

/********************************************************************/
/********************	Application Proc.	*************************/
/********************************************************************/

#define _____STATIC_FUNCTION_____
STATIC BUS_Result_t xproc_tvtvaction(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3)
{
	BUS_Result_t			lResult = ERR_FAIL;
	HERROR				hError = ERR_FAIL;
	TvtvAction_Content_t	*pContent = NULL;
	HUINT32				ulActionId = SVC_PIPE_GetActionId (hHandle);
	HUINT8				*pszMsg = NULL;

	lResult = xmgr_tvtvaction_GetContent(&pContent);

#if defined (CONFIG_DEBUG)
	pszMsg = xproc_GetTvtvActiionMessage(lMessage);
	if(pszMsg != NULL)
	{
		HLIB_CMD_Printf("********************************************************************************\n");
		HLIB_CMD_Printf("*\n");
		HLIB_CMD_Printf("* [XMGR:TVTV EPG] MESSAGE (%s), cActionHandle(0x%08x), hHandle(0x%08x), ulActionId(%x), p1(%x), p2(%x), p3(%x)\n", pszMsg, pContent->hAction, hHandle, ulActionId, lParam1, lParam2, lParam3);
		HLIB_CMD_Printf("*\n");
		HLIB_CMD_Printf("********************************************************************************\n");
	}
#endif

	if(lResult != ERR_OK || pContent == NULL)
	{
		HxLOG_Print("[xproc_tvtvaction] xmgr_tvtvaction_GetContent fail!!\n");
		return MESSAGE_PASS;
	}

	switch(lMessage)
	{
		case eMEVT_BUS_CREATE :

#if 0 // only test
			xmgr_tvtvaction_SetLastUpdateTime();
			BUS_MGR_DestroyWithReason(0, eDESTROY_BUS_MYSELF, 0);
			break;
#endif

			if(xmgr_tvtvaction_IsConflict() == TRUE)
			{
				BUS_MGR_DestroyWithReason(0, eDESTROY_BUS_MYSELF, 0);
				break;
			}

			if(SVC_EPG_InitAction(eEPG_KIND_TVTV) != ERR_OK)
			{
				HxLOG_Print("[xproc_tvtvaction] SVC_EPG_InitAction(eEPG_KIND_TVTV) fail!!\n");
			}

			HxSTD_memset(pContent, 0x00, sizeof(TvtvAction_Content_t));

			pContent->eCallType = lParam3;

			BUS_SetTimer(TVTVACTION_TIMER_ID, TVTVACTION_TIMER_TIME);
			break;

		case eSEVT_EPG_TVTV_STOP_ACTION :
			BUS_SetTimer(TVTVHDD_TIMER_ID, TVTVHDD_TIMER_TIME);
#if defined(SUPPORT_VFD)
			hError = SVC_SYS_DisplayPanelString (ePANEL_STR_PRIORITY_BG_ACTION, "TVTV SIGNALING", eDxPANEL_ALIGN_CENTER);

			if (hError != ERR_OK)
			{
				HxLOG_Print("[xproc_tvtvaction] SVC_SYS_DisplayPanelString fail!!\n");
			}
#endif
			return MESSAGE_BREAK;

		case eMEVT_BUS_TIMER :
			if(lParam1 == TVTVHDD_TIMER_ID)
			{
				SvcFs_DevStatus_e	 ePvrStatus;
				static HUINT8			ucCount = 0;

				ucCount++;

				ePvrStatus = SVC_FS_GetPvrHddStatus(SVC_FS_GetDefaultPvrIdx());
				HxLOG_Info("TVTVHDD_TIMER_ID eHddStatus(%d)ucCount(%d)\n", ePvrStatus, ucCount);
				if(ePvrStatus != eDevStatus_OK)
				{
					if(ucCount < 15)
					{
						BUS_ResetTimer(TVTVHDD_TIMER_ID);
					}
					else
					{
						HxLOG_Error("TVTV_HDD_TIMER_ID Loading FAIL!!!)\n");
						BUS_KillTimer(TVTVHDD_TIMER_ID);
						ucCount = 0;

						if(pContent->eCallType == eEpgCallType_Shutdown)
						{
							BUS_MGR_Destroy(0);
						}
						else if(pContent->eCallType == eEpgCallType_StandbyAction)
						{
							BUS_MGR_Destroy(0);
						}
						else if(pContent->eCallType == eEpgCallType_NomalAction)
						{
							MGR_ACTION_RequestOperation(eWAKEUP_BY_KEY, eWAKEUP_FOR_UNDECIDED);
						}
					}
					return MESSAGE_BREAK;
				}

				BUS_KillTimer(TVTVHDD_TIMER_ID);

				lResult = xmgr_tvtvaction_InitApp();

				if(lResult != ERR_BUS_NO_ERROR)
				{
					BUS_SendMessage(xproc_tvtvaction, eSEVT_EPG_TVTV_DONE, (Handle_t)0, (HINT32)lResult, 0, 0);
					HxLOG_Error("[xproc_tvtvaction] xmgr_tvtvaction_InitApp fail!!\n");
				}
			}
			else if(lParam1 == TVTVACTION_TIMER_ID)
			{
				HxLOG_Info("[xproc_tvtvaction] TVTVACTION_TIMER_ID\n");
				BUS_KillTimer(TVTVACTION_TIMER_ID);

				if(pContent->eCallType == eEpgCallType_Shutdown)
				{
					BUS_MGR_Destroy(0);
				}
				else if(pContent->eCallType == eEpgCallType_StandbyAction)
				{
					BUS_MGR_Destroy(0);
				}
				else if(pContent->eCallType == eEpgCallType_NomalAction)
				{

				}
			}
			else if(lParam1 == TVTVLOCK_TIMER_ID)
			{
				HxLOG_Info("[xproc_tvtvaction] TVTVLOCK_TIMER_ID\n");
				if(pContent->bChLocked == TRUE)
				{
					break;
				}
				BUS_KillTimer(TVTVACTION_TIMER_ID);
				if(pContent->eCallType == eEpgCallType_Shutdown)
				{
					BUS_MGR_Destroy(0);
				}
				else if(pContent->eCallType == eEpgCallType_StandbyAction)
				{
					BUS_MGR_Destroy(0);
				}
				else if(pContent->eCallType == eEpgCallType_NomalAction)
				{

				}
			}
			break;

		case eSEVT_CH_NO_SIGNAL:
			return MESSAGE_BREAK;

		case eSEVT_CH_LOCKED :
			lResult = xmgr_tvtvaction_GetContent(&pContent);
			if(lResult != ERR_OK)
			{
				HxLOG_Print("[xproc_tvtvaction] xmgr_tvtvaction_GetContent fail!!\n");
				return MESSAGE_BREAK;
			}
			if(pContent->bChLocked == TRUE)
			{
				HxLOG_Info("[xproc_tvtvaction] download already!!\n");
				return MESSAGE_BREAK;
			}
			if (pContent->ulActionId != ulActionId)
			{
				HxLOG_Print("[xproc_tvtvaction] ulActionId fail!!\n");
				return MESSAGE_BREAK;
			}
			pContent->bChLocked = TRUE;
			lResult = xmgr_tvtvaction_StartTvtvService();
			if(lResult != ERR_OK)
			{
				HxLOG_Print("[xproc_tvtvaction] xmgr_tvtvaction_StartTvtvService fail!!\n");
				return MESSAGE_BREAK;
			}
			break;

#if defined(SUPPORT_VFD)
		case eSEVT_EPG_TVTV_PROGRESS :
			lResult = xmgr_tvtvaction_DisplayProgress(lParam1);
			if(lResult != ERR_OK)
			{
				HxLOG_Print("[xproc_tvtvaction] xmgr_tvtvaction_DisplayProgress fail!!\n");
			}
			return MESSAGE_BREAK;
#endif

		case eSEVT_EPG_TVTV_DONE :
			pContent->bComplete = TRUE;

			xmgr_tvtvaction_SetLastUpdateTime();

			if(pContent->eCallType == eEpgCallType_Shutdown)
			{
				HxLOG_Info("[xproc_tvtvaction] eEpgCallType_Shutdown !!\n");
				BUS_MGR_DestroyWithReason(0, (lParam1 == ERR_BUS_ACTION_FAILED)?eDESTROY_BUS_ACTION:eDESTROY_BUS_MYSELF, 0);
			}
			else if(pContent->eCallType == eEpgCallType_StandbyAction)
			{
				HxLOG_Info("[xproc_tvtvaction] eEpgCallType_StandbyAction !!\n");
				BUS_MGR_DestroyWithReason(0, (lParam1 == ERR_BUS_ACTION_FAILED)?eDESTROY_BUS_ACTION:eDESTROY_BUS_MYSELF, 0);
			}
			else if(pContent->eCallType == eEpgCallType_NomalAction)
			{
				HxLOG_Info("[xproc_tvtvaction] eEpgCallType_NomalAction !!\n");
				hError = SVC_EPG_LoadData(eEPG_KIND_TVTV, HANDLE_NULL, HANDLE_NULL);
				if(hError != ERR_OK)
				{
					HxLOG_Error("[xproc_tvtvaction] SVC_EPG_LoadData fail\n");
				}

			}
			return MESSAGE_BREAK;

		case eMEVT_BUS_GO_OPERATION:
			// go operation이 올라오면 tvtv epg를 destory한다.
			BUS_MGR_Destroy(0);
			break;

		case eMEVT_STORAGE_FORMAT_DEVICE:
			lResult = SVC_EPG_InitAction(eEPG_KIND_TVTV);
			if(lResult != ERR_OK)
			{
				HxLOG_Error("[xproc_tvtvaction] eMEVT_STORAGE_FORMAT_DEVICE : SVC_EPG_InitAction(eEPG_KIND_TVTV) failed.\n");
			}
			break;

		case eMEVT_BUS_DESTROY :
			lResult = xmgr_tvtvaction_DestroyApp(lParam1);
			if(lResult != ERR_OK)
			{
				HxLOG_Print("[xproc_tvtvaction] xmgr_tvtvaction_DestroyApp fail!!\n");
				break;
			}
			break;
	}

	return MESSAGE_PASS;
}

STATIC HERROR 					xmgr_tvtvaction_SetLastUpdateTime(void)
{
	HLONG sysTime;
	HxDATETIME_t	 startTime;

	sysTime = HLIB_STD_GetSystemTime();

	HAPI_SetRegistryInt (DxDB_APPL_TVTV_LASTUPDATETIME, (HINT32)sysTime);

	HLIB_DATETIME_ConvertUnixTimeToDateTime(sysTime, &startTime);

	HLIB_CMD_Printf("+\t\t * Set  > ast Updat time : ( %d.%d.%d) - (%d:%d:%d)\n", startTime.stDate.usYear,
															startTime.stDate.ucMonth,
															startTime.stDate.ucDay,
															startTime.stTime.ucHour,
															startTime.stTime.ucMinute,
															startTime.stTime.ucSecond );

	HxLOG_Print("+\t\t * Set  > ast Updat time : ( %d.%d.%d) - (%d:%d:%d)\n", startTime.stDate.usYear,
															startTime.stDate.ucMonth,
															startTime.stDate.ucDay,
															startTime.stTime.ucHour,
															startTime.stTime.ucMinute,
															startTime.stTime.ucSecond );
	return ERR_OK;
}


STATIC HERROR 					xmgr_tvtvaction_IsConflict(void)
{
	HBOOL   bTvtvEnable = FALSE;
	HBOOL   bConflictFlag = FALSE;
	UNIXTIME			ulCurTime;
	HUINT32			ulNumSch = 0;
	DxSchedule_t		*pstSchArray = NULL;
	DxSchedule_FindFilterAttr_t stFilter;

	if(DB_PARAM_GetItem(eDB_PARAM_ITEM_TVTV_ENABLE, (HUINT32 *)&bTvtvEnable, 0) != ERR_OK)
	{
		bConflictFlag = TRUE;
	}
	else
	{
		if (eAmState_ACTIVE_STANDBY != MGR_ACTION_GetState())
		{
			bConflictFlag = TRUE;
		}
		else
		{
			if(bTvtvEnable == FALSE)
			{
				bConflictFlag = TRUE;
			}
			else
			{
				if(MGR_ACTION_GetRecordCount() > 0)
				{
					bConflictFlag = TRUE;
				}
				else
				{
					ulCurTime = HLIB_STD_GetSystemTime();

					SAPI_FindScheduleFilterInit(&stFilter);

					stFilter.stOverlapTimeRange.utOverlapTimeRangeStart = ulCurTime;
					stFilter.stOverlapTimeRange.utOverlapTimeRangeEnd = ulCurTime + 60*60;	// reservation과 tvtv와의 시간 간격을 1시간으로 둔다. 보통 1시간 이전에는 받을거는 예상

					SAPI_FindScheduleUsingFilter(&stFilter, &ulNumSch, &pstSchArray);
					SAPI_FindScheduleUsingFilterFreeList(pstSchArray);

					if (ulNumSch > 0)
					{
						bConflictFlag = TRUE;
					}
				}
			}
		}
	}

	HLIB_CMD_Printf("\[xmgr_tvtvaction_GetContent] bConflictFlag (%d) -- standbyStatus(%d), bTvtvEnable(%d), RecCnt(%d), ulNumSch(%d)\n",
									bConflictFlag, MGR_ACTION_GetState(), bTvtvEnable, MGR_ACTION_GetRecordCount(), ulNumSch);

	return bConflictFlag;
}

STATIC BUS_Result_t				xmgr_tvtvaction_GetContent(TvtvAction_Content_t **ppContent)
{
	BUS_Result_t					lResult = ERR_FAIL;

	if(ppContent == NULL)
	{
		HxLOG_Print("[xmgr_tvtvaction_GetContent] param fail!!\n");
		goto EXIT_TVTVACT_FUNC;
	}

	*ppContent = &s_stTvtvActionContent;

	lResult = ERR_BUS_NO_ERROR;

EXIT_TVTVACT_FUNC :

	return lResult;

}

STATIC BUS_Result_t				xmgr_tvtvaction_InitApp(void)
{
	BUS_Result_t					lResult = ERR_FAIL;
	HERROR						hError = ERR_FAIL;
	TvtvAction_Content_t			*pContent = NULL;
	Handle_t						hTvtvHandle = HANDLE_NULL;
	DbSvc_TsInfo_t					stTsInfo;
	DxTuneParam_t				stTuningInfo = {0, };
	DbSvc_TripleId_t				stTripleId = {0, };
	HINT32						lTotalAntNum = 0;
	HINT32						lAntCnt = 0;
	HINT32						*plAntIdxArray = NULL;
	HBOOL						bFoundAstra = FALSE;
	DbSvc_AntInfo_t					stAntInfo = {0, };


	HxLOG_Info("[xmgr_tvtvaction_InitApp] +\n");

	HxSTD_memset(&stTsInfo, 0x00, sizeof(DbSvc_TsInfo_t));
	HxSTD_memset(&stTuningInfo, 0x00, sizeof(DxTuneParam_t));


	lResult = xmgr_tvtvaction_GetContent(&pContent);
	if(lResult != ERR_OK)
	{
		lResult = ERR_BUS_NO_OBJECT;
		HxLOG_Error("[xmgr_tvtvaction_InitApp] xmgr_tvtvaction_GetContent fail!!\n");
		goto EXIT_TVTVACT_FUNC;
	}

	pContent->bComplete = FALSE;

	stTripleId.usOnId = TVTV_SAT_ON_ID;
	stTripleId.usTsId = TVTV_SAT_TS_ID;
	stTripleId.usSvcId = TVTV_SAT_SVC_ID;

	hError = DB_SVC_FindServiceHandleByTripleId(eDxDELIVERY_TYPE_SAT, stTripleId, &hTvtvHandle);
	if(hError != ERR_OK || hTvtvHandle == HANDLE_NULL)
	{
		HxLOG_Info("[xmgr_tvtvaction_InitApp] hTvtvHandle == HANDLE_NULL \n");

		hTvtvHandle = HANDLE_NULL;

#if		defined(CONFIG_MW_CH_SATELLITE)
		hError = DB_SVC_FindAllAntIdxList (&lTotalAntNum, &plAntIdxArray);
		if (hError == ERR_OK)
		{
			for(lAntCnt = 0; lAntCnt < lTotalAntNum; lAntCnt++)
			{
				hError = DB_SVC_GetAntInfo((HUINT16)plAntIdxArray[lAntCnt], &stAntInfo);
				if (hError == ERR_OK)
				{
					if (stAntInfo.eSatType == eSatType_ASTRA_1)
					{
						bFoundAstra = TRUE;
						break;
					}
				}
			}
			DB_SVC_FreeAntList (plAntIdxArray);
		}

		if(!bFoundAstra)
		{
			lResult = ERR_BUS_NO_OBJECT;

			HxLOG_Error("[xmgr_tvtvaction_InitApp] !bFoundAstra!!\n");
			goto EXIT_TVTVACT_FUNC;
		}
#endif

		hError = xmgr_tvtvaction_ConvertAntennaTunningInfo(&stAntInfo, &stTuningInfo);
		if(hError != ERR_OK)
		{
			lResult = ERR_BUS_INITIAL_FAILED;

			HxLOG_Error("[xmgr_tvtvaction_InitApp] DB_SVC_GetAntCount fail!!\n");
			goto EXIT_TVTVACT_FUNC;
		}
		HxSTD_memcpy(&pContent->stTuningInfo, &stTuningInfo, sizeof(DxTuneParam_t));
	}
	else
	{
		HxLOG_Info("[xmgr_tvtvaction_InitApp] hTvtvHandle != HANDLE_NULL \n");

		hError = DB_SVC_GetTsInfoByServiceHandle(hTvtvHandle, &stTsInfo);
		if(hError != ERR_OK)
		{
			lResult = ERR_BUS_NO_OBJECT;

			HxLOG_Error("[xmgr_tvtvaction_InitApp] DB_SVC_GetTsInfoByServiceHandle fail!!\n");
			goto EXIT_TVTVACT_FUNC;
		}

		hError = OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&stTsInfo, &stTuningInfo);
		if(hError != ERR_OK)
		{
			lResult = ERR_BUS_NO_OBJECT;
			HxLOG_Error("[xmgr_tvtvaction_InitApp] OTL_CONV_DbTuningInfo2DxTuneParam fail!!\n");
			goto EXIT_TVTVACT_FUNC;
		}
		HxLOG_Info("[xmgr_tvtvaction_InitApp] OTL_CONV_DbTuningInfo2DxTuneParam OK !!\n");

		HxSTD_memcpy(&pContent->stTuningInfo, &stTuningInfo, sizeof(DxTuneParam_t));
	}
	pContent->hTvtvService = hTvtvHandle;

	if(MGR_ACTION_GetEmptyBgActionId (&(pContent->ulActionId)) != ERR_OK)
	{
		HxLOG_Error("[xmgr_tvtvaction_InitApp] MGR_ACTION_SetupTvTv fail!!\n");
		lResult = ERR_BUS_ACTION_FAILED;
		goto EXIT_TVTVACT_FUNC;
	}

	HxSTD_memcpy(&pContent->stTvtvTripleId, &stTripleId, sizeof(DbSvc_TripleId_t));

/**************************************************************************************************
  *
  * Action setup
  *
  *************************************************************************************************/
	hError = MGR_ACTION_SetupTvTv(pContent->ulActionId, pContent->hTvtvService, &pContent->stTuningInfo, eAmPriority_BGJOB);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[xmgr_tvtvaction_InitApp] MGR_ACTION_SetupTvTv fail!!\n");
		lResult = ERR_BUS_ACTION_FAILED;
		goto EXIT_TVTVACT_FUNC;
	}

	HxLOG_Info("[xmgr_tvtvaction_InitApp] MGR_ACTION_SetupTvTv OK!!\n");

	hError = MGR_ACTION_Take(pContent->ulActionId, eBgType_TVTV, BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()));
	if(hError != ERR_OK)
	{
		lResult = ERR_BUS_ACTION_FAILED;
		HxLOG_Error("[xmgr_tvtvaction_InitApp] MGR_ACTION_Take fail!!\n");
		goto EXIT_TVTVACT_FUNC;
	}
	HxLOG_Info("[xmgr_tvtvaction_InitApp] MGR_ACTION_Take OK!!\n");

/**************************************************************************************************
  *
  * Tune home channel
  *
  *************************************************************************************************/
	hError = xmgr_tvtvaction_TuneTvtvService();
	if(hError != ERR_OK)
	{
		lResult = ERR_BUS_INITIAL_FAILED;
		HxLOG_Error("[xmgr_tvtvaction_InitApp] xmgr_tvtvaction_TuneTvtvService fail!!\n");
		goto EXIT_TVTVACT_FUNC;
	}
	HxLOG_Info("[xmgr_tvtvaction_InitApp] xmgr_tvtvaction_TuneTvtvService OK!!\n");

	BUS_SetTimer(TVTVLOCK_TIMER_ID, TVTVLOCK_TIMER_TIME);

	lResult = ERR_BUS_NO_ERROR;
	HxLOG_Info("[xmgr_tvtvaction_InitApp] -\n");

EXIT_TVTVACT_FUNC :

	return lResult;
}

STATIC BUS_Result_t				xmgr_tvtvaction_StartTvtvService(void)
{
	BUS_Result_t					lResult = ERR_FAIL;
	HERROR						hError = ERR_FAIL;
	TvtvAction_Content_t			*pContent = NULL;

	HxLOG_Info("[xmgr_tvtvaction_StartTvtvService] +\n");
	lResult = xmgr_tvtvaction_GetContent(&pContent);
	if(lResult != ERR_OK)
	{
		HxLOG_Print("[xmgr_tvtvaction_StartTvtvService] xmgr_tvtvaction_GetContent fail!!\n");
		goto EXIT_TVTVACT_FUNC;
	}

	hError = SVC_EPG_Start(eEPG_KIND_TVTV, eEPG_EVT_KIND_ALL, pContent->hAction, HANDLE_NULL, (HUINT32)&pContent->stTvtvTripleId, 0, 0);
	if (hError != ERR_OK)
	{
		HxLOG_Print("[xmgr_tvtvaction_StartTvtvService] SVC_EPG_Start(eEPG_KIND_TVTV) fail!!\n");
		goto EXIT_TVTVACT_FUNC;
	}

	lResult = ERR_BUS_NO_ERROR;
	HxLOG_Info("[xmgr_tvtvaction_StartTvtvService] -\n");
EXIT_TVTVACT_FUNC :

	return lResult;
}

STATIC BUS_Result_t				xmgr_tvtvaction_TuneTvtvService (void)
{
	BUS_Result_t					lResult = ERR_FAIL;
	HERROR						 hError = ERR_FAIL;
	SvcRsc_Info_t					 stRscInfo;
	TvtvAction_Content_t			*pContent = NULL;

	lResult = xmgr_tvtvaction_GetContent(&pContent);
	if(lResult != ERR_OK)
	{
		HxLOG_Print("[xmgr_tvtvaction_TuneTvtvService] xmgr_tvtvaction_GetContent fail!!\n");
		goto EXIT_TVTVACT_FUNC;
	}

	MGR_RSC_GetResourceInfo (pContent->ulActionId, &stRscInfo);


	hError = SVC_CH_StopTune(	SVC_PIPE_GetActionHandle(pContent->ulActionId),
								(HUINT16)-1,
								eSyncMode);
	if (hError != ERR_OK)
	{
		HxLOG_Print("[xmgr_tvtvaction_TuneTvtvService] SVC_CH_StopTune fail!!\n");
		goto EXIT_TVTVACT_FUNC;
	}

	// version up~
	pContent->hAction = SVC_PIPE_IncreaseActionVersion(pContent->ulActionId);
	hError = SVC_CH_StartTune (pContent->hAction, &stRscInfo, (HUINT16)-1, &pContent->stTuningInfo);
	if (hError != ERR_OK)
	{
		HxLOG_Print("[xmgr_tvtvaction_TuneTvtvService] SVC_CH_StartTune fail!!\n");
		goto EXIT_TVTVACT_FUNC;
	}

	// DD에서 Tuning Signal이 올라오지 않아 무한으로 기다리는 것을 막기 위해 여기서 timer를 처리하도록 한다.

	lResult = ERR_BUS_NO_ERROR;

EXIT_TVTVACT_FUNC :

	return lResult;
}

#if defined(SUPPORT_VFD)
STATIC BUS_Result_t				xmgr_tvtvaction_DisplayProgress(HINT32 lParam)
{
	BUS_Result_t					lResult = ERR_FAIL;
	HERROR						 hError = ERR_FAIL;
	HUINT8						aucProgress[32] = {0, };


	snprintf((char*)aucProgress, 32, "PROGRESS %d%%", lParam);
	hError = SVC_SYS_DisplayPanelString (ePANEL_STR_PRIORITY_BG_ACTION, aucProgress, eDxPANEL_ALIGN_CENTER);
	if (hError != ERR_OK)
	{
		HxLOG_Print("[xmgr_tvtvaction_DisplayProgress] SVC_SYS_DisplayPanelString fail!!\n");
		goto EXIT_TVTVACT_FUNC;
	}

	lResult = ERR_BUS_NO_ERROR;

EXIT_TVTVACT_FUNC :

	return lResult;
}
#endif

STATIC BUS_Result_t				xmgr_tvtvaction_DestroyApp(BUS_DestroyReason_t actionReason)
{
	BUS_Result_t				lResult = ERR_FAIL;
	HERROR						hError = ERR_FAIL;
	TvtvAction_Content_t		*pContent = NULL;

	HxLOG_Info("[xmgr_tvtvaction_DestroyApp] +\n");

#if defined(SUPPORT_VFD)
	hError = SVC_SYS_ClearPanelString(ePANEL_STR_PRIORITY_BG_ACTION);
	if (hError != ERR_OK)
	{
		HxLOG_Error("[xmgr_tvtvaction_DestroyApp] SVC_SYS_ClearPanelString fail!!\n");
	}
#endif

	lResult = xmgr_tvtvaction_GetContent(&pContent);
	if(lResult != ERR_OK)
	{
		HxLOG_Error("[xmgr_tvtvaction_DestroyApp] xmgr_tvtvaction_GetContent fail!!\n");
	}

	if(!pContent->bComplete)
	{
		HxLOG_Info("[xmgr_tvtvaction_DestroyApp] MW_EPG_StopTvtv!!\n");
		hError = SVC_EPG_Stop(eEPG_KIND_TVTV, eEPG_EVT_KIND_ALL, SVC_PIPE_GetActionHandle(pContent->ulActionId));
		if (hError != ERR_OK)
		{
			HxLOG_Error("[xmgr_tvtvaction_DestroyApp] SVC_EPG_Stop(eEPG_KIND_TVTV) fail!!\n");
		}
	}

	if(actionReason != eDESTROY_BUS_ACTION)
	{
		hError = SVC_CH_StopTune(pContent->hAction,	(HUINT16)-1, eSyncMode);
		if (hError != ERR_OK)
		{
			HxLOG_Error("[xmgr_tvtvaction_DestroyApp] SVC_CH_StopTune fail!!\n");
		}

		hError = MGR_ACTION_Release(pContent->ulActionId, eBgType_TVTV);
		if(hError != ERR_OK)
		{
			HxLOG_Error("[xmgr_tvtvaction_DestroyApp] MGR_ACTION_Release fail!!\n");
		}
	}

	HxLOG_Info("[xmgr_tvtvaction_DestroyApp] -\n");

	lResult = ERR_BUS_NO_ERROR;

//EXIT_TVTVACT_FUNC :

	BUS_PostMessage(NULL, eMEVT_EPG_PROCESS_DONE, eEpgKind_TVTV, 0, 0, 0);

	return lResult;
}


STATIC BUS_Result_t				xmgr_tvtvaction_ConvertAntennaTunningInfo(DbSvc_AntInfo_t *pAntInfo, DxTuneParam_t *pChTuningParam)
{
	BUS_Result_t						lResult = ERR_FAIL;
	DxSatTuningInfo_t					*pChTuningInfoSat = (DxSatTuningInfo_t*)pChTuningParam;

	if(pAntInfo == NULL || pChTuningParam == NULL)
	{
		HxLOG_Print("[xmgr_tvtvaction_ConvertAntennaTunningInfo] param fail!!\n");
		goto EXIT_TVTVACT_FUNC;
	}

	pChTuningInfoSat->deliveryType = eDxDELIVERY_TYPE_SAT;
	pChTuningInfoSat->tuningInfo.ulFrequency = TVTV_TUNE_INFO_FREQ;
	pChTuningInfoSat->tuningInfo.ulSymbolRate = TVTV_TUNE_INFO_SR;
	pChTuningInfoSat->tuningInfo.eFecCodeRate = TVTV_TUNE_INFO_CODERATE;
	pChTuningInfoSat->tuningInfo.ePolarization = TVTV_TUNE_INFO_POLARIZATION;
	pChTuningInfoSat->tuningInfo.eTransSpec = TVTV_TUNE_INFO_TRANS;
	pChTuningInfoSat->tuningInfo.ePskMod = TVTV_TUNE_INFO_PSK;
	pChTuningInfoSat->tuningInfo.ePilot = TVTV_TUNE_INFO_PILOT;

	pChTuningInfoSat->antennaType = pAntInfo->eAntennaType;

	switch (pAntInfo->eAntennaType)
	{
		case eDxANT_TYPE_LNB_ONLY:
			pChTuningInfoSat->antInfo.info.lnb.ulLnbFreq = pAntInfo->utAntInfo.stLnbAnt.ulLnbFreq;
			pChTuningInfoSat->antInfo.info.lnb.lnbVoltage = pAntInfo->utAntInfo.stLnbAnt.eLnbVoltage;
			break;

		case eDxANT_TYPE_DISEQC:
			pChTuningInfoSat->antInfo.info.diseqc.ulLnbFreq = pAntInfo->utAntInfo.stDiseqcAnt.ulLnbFreq;
			pChTuningInfoSat->antInfo.info.diseqc.lnbVoltage = pAntInfo->utAntInfo.stDiseqcAnt.eLnbVoltage;
			pChTuningInfoSat->antInfo.info.diseqc.b22kTone = pAntInfo->utAntInfo.stDiseqcAnt.b22kTone;

			pChTuningInfoSat->antInfo.info.diseqc.diseqcVersion = pAntInfo->utAntInfo.stDiseqcAnt.eDiseqcVer;
			pChTuningInfoSat->antInfo.info.diseqc.diseqcInput = pAntInfo->utAntInfo.stDiseqcAnt.eDiseqcInput;
			pChTuningInfoSat->antInfo.info.diseqc.toneBurst = pAntInfo->utAntInfo.stDiseqcAnt.eToneBurst;

			pChTuningInfoSat->antInfo.info.diseqc.ucMotorPosition = pAntInfo->utAntInfo.stDiseqcAnt.ucMotorPosition;
			pChTuningInfoSat->antInfo.info.diseqc.sSatLonggitude = pAntInfo->utAntInfo.stDiseqcAnt.sSatLonggitude;
			break;

		case eDxANT_TYPE_SCD:
			pChTuningInfoSat->antInfo.info.scd.ulLnbFreq = pAntInfo->utAntInfo.stScdAnt.ulLnbFreq;
			pChTuningInfoSat->antInfo.info.scd.lnbVoltage = pAntInfo->utAntInfo.stScdAnt.eLnbVoltage;
			pChTuningInfoSat->antInfo.info.scd.b22kTone = pAntInfo->utAntInfo.stScdAnt.b22kTone;

			pChTuningInfoSat->antInfo.info.scd.scdInput = pAntInfo->utAntInfo.stScdAnt.eScdInput;
			pChTuningInfoSat->antInfo.info.scd.ulScdUserBandNum = pAntInfo->utAntInfo.stScdAnt.ulScdUserBandNum;
			pChTuningInfoSat->antInfo.info.scd.ulScdUserBandFreq = pAntInfo->utAntInfo.stScdAnt.ulScdUserBandFreq;
			pChTuningInfoSat->antInfo.info.scd.ulScdUserBandNum1 = pAntInfo->utAntInfo.stScdAnt.ulScdUserBandNum1;
			pChTuningInfoSat->antInfo.info.scd.ulScdUserBandFreq1 = pAntInfo->utAntInfo.stScdAnt.ulScdUserBandFreq1;
			break;

		case eDxANT_TYPE_SMATV:
			break;

		default:
			HxLOG_Print("[xmgr_tvtvaction_ConvertAntennaTunningInfo] default!!\n");
			break;
	}
	lResult = ERR_BUS_NO_ERROR;

EXIT_TVTVACT_FUNC :

	return lResult;
}

#define _____GLOBAL_FUNCTION_____

HERROR			xmgr_epg_TvtvStart_Base(MgrEpg_ProcessType_e eProcessType, MgrEpg_CallType_e eActionType)
{
	BUS_Result_t	eRes;

	eRes = BUS_MGR_Create("proc_tvtvaction", APP_ACTION_PRIORITY, xproc_tvtvaction, HANDLE_NULL, 0, 0, eActionType);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

HERROR			xmgr_epg_TvtvStop_Base(void)
{
	BUS_Result_t	eRes;
	eRes = BUS_MGR_Destroy(xproc_tvtvaction);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

BUS_Callback_t	XMGR_EPG_TVTV_GetProc(void)
{
	return xproc_tvtvaction;
}

BUS_Callback_t			XMGR_EPG_InitBatchTvtv_Base(void **ppvUserData, HUINT32 *pulUserDataLength)
{
	MGR_EPG_Start(eEpgKind_TVTV, eEpgStartType_Normal, eEpgCallType_Shutdown);

	return XMGR_EPG_TVTV_GetProc();

}

HERROR			XMGR_EPG_DeinitBatchTvtv_Base(void *pvUserData, HUINT32 ulUserDataLength)
{
	return MGR_EPG_Stop(eEpgKind_TVTV);
}

HERROR			XMGR_EPG_RescheduleBatchTvtv_Base(void)
{
	return ERR_OK;
}

HERROR			XMGR_EPG_GetBatchEpgCBListTvtv_Base(MgrAction_BatchProcCBList_t * fnBatchProcCBList)
{
	HERROR			hError = ERR_FAIL;
	HBOOL			bTvtvEnable = FALSE;
	HBOOL			bUpdateTime = FALSE;

	HxLOG_Trace();

	fnBatchProcCBList->fnAppInit = NULL;
	fnBatchProcCBList->fnAppDeinit = NULL;
	fnBatchProcCBList->fnAppReSchedule = NULL;


	hError = DB_PARAM_GetItem(eDB_PARAM_ITEM_TVTV_ENABLE, (HUINT32 *)&bTvtvEnable, 0);

	if(hError != ERR_OK )
	{
		HxLOG_Error("eDB_PARAM_ITEM_TVTV_ENABLE fail\n");
		return ERR_FAIL;
	}

	if(bTvtvEnable == FALSE)
	{
		HxLOG_Info("bTvtvEnable == FALSE \n");
		return ERR_FAIL;
	}

	hError = DB_PARAM_GetItem(eDB_PARAM_ITEM_TVTV_DOWNTYPE, (HUINT32 *)&bUpdateTime, 0);
	if(hError != ERR_OK )
	{
		HxLOG_Error("eDB_PARAM_ITEM_TVTV_DOWNTYPE fail\n");
		return ERR_FAIL;
	}

	if(bUpdateTime == TRUE)
	{
		HxLOG_Info("bUpdateTime == TRUE \n");
		return ERR_FAIL;
	}



	fnBatchProcCBList->fnAppInit = MGR_EPG_InitBatch;
	fnBatchProcCBList->fnAppDeinit = MGR_EPG_DeinitBatch;
	fnBatchProcCBList->fnAppReSchedule = MGR_EPG_RescheduleBatch;

	return ERR_OK;
}

#endif

/*********************** End of File ******************************/
