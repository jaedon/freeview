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

#include <util_task.h>
#include <linkedlist.h>
#include <db_svc.h>
#include <db_param.h>

#include <pal_sef.h>
#include <pal_pipe.h>

#include <svc_si.h>
#include <svc_epg.h>

#include "_svc_epg_common.h"
#include "_svc_epg_common_lib.h"
#include "_svc_epg_manager.h"
#include "_svc_epg_context_mgr.h"
#include "_svc_epg_db_raw_lib.h"
#include "_svc_epg_eit_lib.h"

#include "util.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
//#define CONFIG_EPG_EITTEST


#define EIT_ACTUAL_SCHEDULE_TABLE_ID_MASK		0x10
#define EIT_OTHER_SCHEDULE_TABLE_ID_MASK		0x20

#define EIT_ACTUAL_SCHEDULE_MSG_MAX				100
#define EIT_OTHER_SCHEDULE_MSG_MAX				25

#if defined(CONFIG_DOUGLAS)
#define EIT_PROC_DURATION							20			// 20 ms
#define EIT_TASKSLEEP_ACT_DURATION				200			// 200 ms
#define EIT_TASKSLEEP_OTH_DURATION				500			// 500 ms
#define EIT_TASK_MAX_WAITTIME						10000		// 10000 ms

#else
#define EIT_PROC_DURATION							20			// 20 ms
#define EIT_TASKSLEEP_ACT_DURATION				200			// 200 ms
#define EIT_TASKSLEEP_OTH_DURATION				200			// 200 ms
#define EIT_TASK_MAX_WAITTIME						10000		// 10000 ms

#endif






#define FUNC_ENTER 		HxLOG_Debug("(+)Enter!!\n")
#define FUNC_LEAVE(x)	HxLOG_Debug("(-)Leave!!(0x%X)\n", x)

// For Debugging function
#define DBG_SC_SETBITMASK(p, pos)			(p[(pos>>3)]  |= (0x01 <<(pos&7)))
#define DBG_SC_CLEARBITMASK(p, pos)			(p[(pos>>3)] &= (~(0x01 <<(pos&7))))
#define DBG_SC_CHECKBITMASK(p, pos)			(p[(pos>>3)] & (0x01 <<(pos&7)))
#define DBG_SC_SEGMENTBITMASK(p, pos)		( p[(pos>>3)] &= (  0xff >> (7 - (pos&7))  )  )


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	EPGCNXT_FITER_SLOT_EIT_SCH_AC		= 0,
	EPGCNXT_FITER_SLOT_EIT_SCH_OTH,

	EPGCNXT_FITER_SLOT_SCH_MAX
}XsvcEpg_ContextEitSchFilterSlot_e;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

#if defined(CONFIG_DEBUG) && 0
// For Debugging function
STATIC HUINT8 s_aaucScSecBuildMask[16][32];
STATIC HUINT8 s_aaucScCmpSecBuildMask[16][32];
STATIC HUINT8 s_aucScNonRcvSecBuildMask[32];
STATIC HUINT8 s_aucScLastSecNum[16];
#endif

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

STATIC HERROR xsvc_epgcontext_SchStartFilter_Base(	svcEpg_Filter_t *pstFilterInfo,
											Handle_t hAction,
											HUINT8 ucHead,
											HUINT32 ulDemuxId,
											HUINT16 usTsUniqueId,
											HINT32 lFilterState);
STATIC HERROR xsvc_epgcontext_SchStartNextFilter_Base(svcEpg_Filter_t *pstFilterInfo, Handle_t hAction, Handle_t hService, HUINT8 ucHead, HUINT32 ulDemuxId, HUINT16 usTsUniqueId);
STATIC HERROR xsvc_epgcontext_SchStopFilter_Base(svcEpg_Filter_t *pstFilterInfo);
STATIC HERROR xsvc_epgcontext_SchGetContextInfo_Base(Handle_t hAction, HUINT32 ulIndex, Handle_t *phSvcHandle, HUINT32 *pulOrgDeliType, HUINT8 *pucAntId, EPG_SpecType_e *peSpec, svcEpg_Filter_t *pEpgContextFilterInfo);
STATIC HERROR xsvc_epgcontext_SchSetContextFilterInfo_Base(Handle_t hAction, HUINT32 ulIndex, svcEpg_Filter_t *pEpgContextFilterInfo);
#if 0 //defined but not used
STATIC HERROR _l_cnxtrawsch_CheckEitVersion(HUINT32 ulParam1, HUINT32 ulParam2);
#endif
#if defined(CONFIG_DEBUG)
STATIC void xsvc_epgcontext_SchDebugStatus_Base(void);
STATIC void xsvc_epgcontext_SchDebugFunction_Base(HUINT32 ulHeadInfo, HUINT32 ulCmdFlag);

#endif

#define ____GLOBAL_FUNC____


/*****************************************************
 * Name : xsvc_epgcontext_SchProcess_Base
 *
 *
 *
 *****************************************************/
HERROR xsvc_epgcontext_SchProcess_Base(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3, HUINT32 ulParam4)
{
	svcEpg_Msg_t			*pstMsg = NULL;
	svcEpg_ContextData_t		*pCnxtData = NULL;
	HUINT32				ulCnxtCommand = 0;
	Handle_t			hAction = 0;
	Handle_t			hService = HANDLE_NULL;
	HERROR				hError = ERR_EPG_FAIL;
	HUINT32				ulActionId = 0;
	HUINT32		ulDemuxId = RxRSCID_NULL;
	HUINT16				usTsUniqueId = 0;
	HUINT8				ucHead = ACT_HEAD;
	HUINT16				usNewTsUniqueId = 0;
	DbSvc_Info_t			stSvcInfo;
	DbSvc_TsInfo_t		stTsInfo = {0,};
	HUINT32				ulMessage = 0;
	HBOOL				bLoadingDone =FALSE;
	EPG_SpecType_e		eSpec;
	HUINT16				usTableId = 0;

	ulCnxtCommand = ulParam1;
	pstMsg = (svcEpg_Msg_t*)ulParam2;
	pCnxtData = (svcEpg_ContextData_t*)ulParam3;
	if(pCnxtData == NULL)
	{
		HxLOG_Error("pCnxtData null!!\n");
		goto EXIT_CNXTRAWSCH_FUNC;
	}
	HAPPY(ulParam4);

	if ( pstMsg == NULL)
	{
		HxLOG_Error("param null!!\n");
		goto EXIT_CNXTRAWSCH_FUNC;
	}

	/**********************************************************************************
	  *
	  * EPG_CMD_START => EPG_CMD_START_FILTER => EPG_CMD_STOP_FILTER => EPG_CMD_NEXT_FILTER
	  * EPG_CMD_STOP_FILTER => EPG_CMD_NEXT_FILTER .... 순환 구조
	  *
	  **********************************************************************************/
	switch(ulCnxtCommand)
	{
		case EPG_CMD_START :
			// step 1. local data setting
			hAction = (Handle_t)pstMsg->ulParam1;
			hService = (Handle_t)pstMsg->ulParam2;

			HxLOG_Print("EPG_CMD_START(hSvc:%x)!!\n", hService);
			/**********************************************************************************
			  *
			  * get tuning info
			  *
			  **********************************************************************************/
			DB_SVC_GetUniqueIdFromSvcHandle(hService, &usNewTsUniqueId);
			DB_SVC_GetUniqueIdFromSvcHandle(pCnxtData->hService, &usTsUniqueId);
#if 0 // This is useless function. it can be make problem.
			if(usTsUniqueId == usNewTsUniqueId)
			{
				HxLOG_Error("Same TS!!\n");
				hError = ERR_EPG_OK;
				goto EXIT_CNXTRAWSCH_FUNC;
			}
#endif
			bLoadingDone = DB_SVC_IsMemoryLoadingDone();
			if(bLoadingDone == FALSE)
			{
				HxLOG_Info("EPG_CMD_START retry!!\n");
				VK_TASK_Sleep(100);
				// retry
				hError = SVC_EPG_MakeEpgMessage(EPG_CMD_START, eEPG_PROCESS_EIT_SCH, &ulMessage);
				if(hError != ERR_EPG_OK)
				{
					HxLOG_Error("MW_EPGMAIN_MakeEpgMessage fail!!\n");
				}
				hError = SVC_EPG_SendMessage(ulMessage, hAction, hService, 0);
				if(hError != ERR_EPG_OK)
				{
					HxLOG_Error("SendMessage fail!!\n");
				}
				break;
			}

#if defined(CONFIG_MW_EPG_NEXT_EITFILTER_TIMER)
			if (VK_TIMERID_NULL != pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_AC].ulReStartTimerId)
			{
				VK_TIMER_Cancel(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_AC].ulReStartTimerId);
				pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_AC].ulReStartTimerId = VK_TIMERID_NULL;
			}

			if (VK_TIMERID_NULL != pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_OTH].ulReStartTimerId)
			{
				VK_TIMER_Cancel(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_OTH].ulReStartTimerId);
				pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_OTH].ulReStartTimerId = VK_TIMERID_NULL;
			}
#endif

			if(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_AC].eState != eEPG_CNXT_FILTER_EMPTY)
			{
				hError = xsvc_epgcontext_SchStopFilter_Base(	&(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_AC]) );
				if(hError != ERR_EPG_OK)
				{
					HxLOG_Error("xsvc_epgcontext_SchStopFilter_Base fail!!\n");
					//goto EXIT_CNXTRAWSCH_FUNC;
				}
				HxSTD_memset(&(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_AC].stBaseInfo), 0x00, sizeof(svcEpg_ServiceInfo_t));
				pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_AC].ulRequestId = 0;
			}
			if(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_OTH].eState != eEPG_CNXT_FILTER_EMPTY)
			{
				hError = xsvc_epgcontext_SchStopFilter_Base(	&(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_OTH]) );
				if(hError != ERR_EPG_OK)
				{
					HxLOG_Error("xsvc_epgcontext_SchStopFilter_Base fail!!\n");
					//goto EXIT_CNXTRAWSCH_FUNC;
				}
				HxSTD_memset(&(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_OTH].stBaseInfo), 0x00, sizeof(svcEpg_ServiceInfo_t));
				pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_OTH].ulRequestId = 0;
			}

			hError = DB_SVC_GetServiceInfo(hService, &stSvcInfo);
			if(hError != ERR_OK)
			{
				VK_TASK_Sleep(100);
				hError = DB_SVC_GetServiceInfo(hService, &stSvcInfo);
				if(hError != ERR_OK)
				{
					HxLOG_Error("DB_SVC_GetServiceInfo fail!!\n");
					goto EXIT_CNXTRAWSCH_FUNC;
				}
			}
			hError = DB_SVC_GetTsInfo(DbSvc_GetTsIdx(&stSvcInfo), &stTsInfo);
			if(hError != ERR_OK)
			{
				HxLOG_Error("DB_SVC_GetTsInfo fail!!\n");
				goto EXIT_CNXTRAWSCH_FUNC;
			}


			/**********************************************************************************
			  *
			  * set context
			  *
			  **********************************************************************************/
			pCnxtData->hAction = hAction;
			pCnxtData->hService = hService;
			HxLOG_Print("EPG_CMD_START(hAction:%x)(hService:0x%X)!!\n", hAction,hService);
			pCnxtData->ulOrgDeliveryType = (HUINT32)DbSvc_GetDeliType(stSvcInfo);
			pCnxtData->ucAntId = 0;
			if(pCnxtData->ulOrgDeliveryType == eDxDELIVERY_TYPE_SAT)
			{
				pCnxtData->ucAntId = (HUINT32)stTsInfo.stTuningParam.sat.ucAntId;
			}
			hError = SVC_EPG_GetSpec(pCnxtData->ulOrgDeliveryType, hService, &eSpec);
			if(hError != ERR_OK)
			{
				HxLOG_Error("DB_SVC_GetTsInfo fail!!\n");
			}
			pCnxtData->eSpec = eSpec;

			HxLOG_Print("ulOrgDeliveryType (%d) ucAntId(%d)!!\n", pCnxtData->ulOrgDeliveryType, pCnxtData->ucAntId);
			pCnxtData->eState = eEPG_CNXT_STATUS_RUNNING;

			ulActionId = PAL_PIPE_GetActionId(hAction);
			if(ulActionId >= NUM_MAX_ACTION)
			{
				HxLOG_Error("GetActionId fail!!\n");
				goto EXIT_CNXTRAWSCH_FUNC;
			}
			pCnxtData->ulActionId = ulActionId;

			hError = PAL_PIPE_GetResourceId(ulActionId, eRxRSCTYPE_DEMUX, &ulDemuxId);
			if(hError != ERR_OK)
			{
				HxLOG_Error("GetResourceId fail!!\n");
				goto EXIT_CNXTRAWSCH_FUNC;
			}
			pCnxtData->ulDemuxId = (HUINT32)ulDemuxId;

			/**********************************************************************************
			  *
			  * resource init
			  *
			  **********************************************************************************/
			DB_SVC_GetUniqueIdFromSvcHandle(pCnxtData->hService, &usTsUniqueId);
			hError = SVC_EPGSECTION_InitSvcSectionManager(hAction, usTsUniqueId, EIT_ACTUAL_SCHEDULE_TABLE_ID_0);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Error("EPGRAWDB_InitSvcSectionManager fail!!\n");
				goto EXIT_CNXTRAWSCH_FUNC;
			}

			hError = SVC_EPGSECTION_InitSvcSectionManager(hAction, usTsUniqueId, EIT_OTHER_SCHEDULE_TABLE_ID_0);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Error("EPGRAWDB_InitSvcSectionManager fail!!\n");
				goto EXIT_CNXTRAWSCH_FUNC;
			}

#if 0 // optimize 필요 없음.
#if defined(YSMOON_NEW_EPG)
			hError = SVC_EPGDB_OptimizeSectionList(eSpec,pCnxtData->ulOrgDeliveryType,&stTsInfo,stSvcInfo.usSvcId);
#else
//			hError = SVC_EPGLIB_OptimizeSectionList(eSpec,pCnxtData->ulOrgDeliveryType,&stTsInfo,stSvcInfo.usSvcId);
#endif
			if(hError != ERR_OK)
			{
				HxLOG_Print("SVC_EPGDB_OptimizeSectionList fail!!\n");
				goto EXIT_CNXTRAWSCH_FUNC;
			}
#endif

			/**********************************************************************************
			  *
			  * send message
			  *
			  **********************************************************************************/
			HxLOG_Info("EPG_CMD_START_FILTER +!!\n");
			hError = xsvc_epgcontext_SchStartFilter_Base(	&(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_AC]),
												hAction,
												ACT_HEAD,
												pCnxtData->ulDemuxId,
												usTsUniqueId,
												eEPG_CNXT_FILTER_CONTINUE);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Error("xsvc_epgcontext_SchStartFilter_Base EPGCNXT_FITER_SLOT_EIT_SCH_AC fail!!\n");
				goto EXIT_CNXTRAWSCH_FUNC;
			}

			hError = xsvc_epgcontext_SchStartFilter_Base(	&(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_OTH]),
												hAction,
												OTH_HEAD,
												pCnxtData->ulDemuxId,
												usTsUniqueId,
												eEPG_CNXT_FILTER_CONTINUE);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Error("xsvc_epgcontext_SchStartFilter_Base EPGCNXT_FITER_SLOT_EIT_SCH_OTH fail!!\n");
				goto EXIT_CNXTRAWSCH_FUNC;
			}
			pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_AC].ucDemuxId = (HUINT8)ulDemuxId;
			pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_OTH].ucDemuxId = (HUINT8)ulDemuxId;
			break;

		case EPG_CMD_STOP :
			HxLOG_Print("EPG_CMD_STOP!!\n");
			hAction = (Handle_t)pstMsg->ulParam1;
			pCnxtData->hAction = HANDLE_NULL;
			pCnxtData->hService = HANDLE_NULL;
#if defined(CONFIG_MW_EPG_NEXT_EITFILTER_TIMER)
			if (VK_TIMERID_NULL != pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_AC].ulReStartTimerId)
			{
				VK_TIMER_Cancel(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_AC].ulReStartTimerId);
				pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_AC].ulReStartTimerId = VK_TIMERID_NULL;
			}

			if (VK_TIMERID_NULL != pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_OTH].ulReStartTimerId)
			{
				VK_TIMER_Cancel(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_OTH].ulReStartTimerId);
				pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_OTH].ulReStartTimerId = VK_TIMERID_NULL;
			}
#endif

			hError = xsvc_epgcontext_SchStopFilter_Base(	&(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_AC]) );
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Error("xsvc_epgcontext_SchStopFilter_Base fail!!\n");
				//goto EXIT_CNXTRAWSCH_FUNC;
			}
			HxSTD_memset(&(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_AC].stBaseInfo), 0x00, sizeof(svcEpg_ServiceInfo_t));
			pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_AC].ulRequestId = 0;

			hError = xsvc_epgcontext_SchStopFilter_Base(	&(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_OTH]) );
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Error("xsvc_epgcontext_SchStopFilter_Base fail!!\n");
				//goto EXIT_CNXTRAWSCH_FUNC;
			}
			HxSTD_memset(&(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_OTH].stBaseInfo), 0x00, sizeof(svcEpg_ServiceInfo_t));
			pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_OTH].ulRequestId = 0;

			hError = SVC_EPGSECTION_InitSvcSectionManager(hAction, 0, EIT_ACTUAL_SCHEDULE_TABLE_ID_0);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Error("EPGRAWDB_InitSvcSectionManager fail!!\n");
				goto EXIT_CNXTRAWSCH_FUNC;
			}

			hError = SVC_EPGSECTION_InitSvcSectionManager(hAction, 0, EIT_OTHER_SCHEDULE_TABLE_ID_0);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Error("EPGRAWDB_InitSvcSectionManager fail!!\n");
				goto EXIT_CNXTRAWSCH_FUNC;
			}
			break;

		case EPG_CMD_NEXT_FILTER_TIMER:
			HxLOG_Print("EPG_CMD_NEXT_FILTER_TIMER!!\n");
			{
				HUINT32 ulTimerId;

				hAction = (Handle_t)pstMsg->ulParam1;
				ucHead = (HUINT8)pstMsg->ulParam2;
				ulTimerId = (HUINT32)pstMsg->ulParam3;

				HxLOG_Info("EPG_CMD_NEXT_FILTER_TIMER (%d)(0x%X)\n", ucHead,ulTimerId);
#if defined(CONFIG_MW_EPG_NEXT_EITFILTER_TIMER)
				if (ulTimerId != pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_AC + ucHead].ulReStartTimerId)
				{
					HxLOG_Error("InTimerId:0x%X != ExistTimerId:0x%X\n",
									ulTimerId, pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_AC + ucHead].ulReStartTimerId);
					pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_AC + ucHead].ulReStartTimerId = VK_TIMERID_NULL;
					break;
				}

				pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_AC + ucHead].ulReStartTimerId = VK_TIMERID_NULL;
#endif

				usTableId = EIT_ACTUAL_SCHEDULE_TABLE_ID_0;
				if(ucHead == OTH_HEAD)
				{
					usTableId = EIT_OTHER_SCHEDULE_TABLE_ID_0;
				}
				DB_SVC_GetUniqueIdFromSvcHandle(pCnxtData->hService, &usTsUniqueId);
				hError = SVC_EPGSECTION_InitSvcSectionManager(hAction, usTsUniqueId, (HUINT8) usTableId);
				if(hError != ERR_EPG_OK)
				{
					HxLOG_Error("SVC_EPGSECTION_InitSvcSectionManager fail!!\n");
					goto EXIT_CNXTRAWSCH_FUNC;
				}
				hError = xsvc_epgcontext_SchStartFilter_Base(	&(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_AC + ucHead]),
													hAction,
													ucHead,
													pCnxtData->ulDemuxId,
													usTsUniqueId,
													eEPG_CNXT_FILTER_CONTINUE);
				if(hError != ERR_EPG_OK)
				{
					HxLOG_Error("local_cnxtrawpf_StartFilter fail!!\n");
					goto EXIT_CNXTRAWSCH_FUNC;
				}
			}
			break;


		case EPG_CMD_STOP_FILTER:
			HxLOG_Print("EPG_CMD_STOP_FILTER!!\n");
			hAction = (Handle_t)pstMsg->ulParam1;
			ucHead = (HUINT8)pstMsg->ulParam2;

			HxLOG_Info("EPG_CMD_STOP_FILTER ucHead(%02d)\n", ucHead);
			DB_SVC_GetUniqueIdFromSvcHandle(pCnxtData->hService, &usTsUniqueId);
			hError = xsvc_epgcontext_SchStopFilter_Base(	&(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_AC + ucHead]) );
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Error("xsvc_epgcontext_SchStopFilter_Base(%d) fail!!\n", ucHead);
				goto EXIT_CNXTRAWSCH_FUNC;
			}
#if defined(CONFIG_MW_EPG_NEXT_EITFILTER_TIMER)
			{
				HUINT32 ulTimerId;

				if (VK_TIMERID_NULL != pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_AC + ucHead].ulReStartTimerId)
				{
					VK_TIMER_Cancel(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_AC + ucHead].ulReStartTimerId);
					pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_AC + ucHead].ulReStartTimerId = VK_TIMERID_NULL;
				}

				usTableId = EIT_ACTUAL_SCHEDULE_TABLE_ID_0;
				if(ucHead == OTH_HEAD)
				{
					usTableId = EIT_OTHER_SCHEDULE_TABLE_ID_0;
				}
				hError = SVC_EPGCONTEXT_SetTimerOnce(usTableId, pCnxtData->eSpec, &ulTimerId);
				if(hError != ERR_EPG_OK)
				{
					HxLOG_Error("SVC_EPGCONTEXT_SetTimerOnce fail!!\n");
					goto EXIT_CNXTRAWSCH_FUNC;
				}
				pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_AC + ucHead].ulReStartTimerId = ulTimerId;
				HxLOG_Print("Head:%d ulTimerId:0x%X SettedTimerId:0x%X\n",
							ucHead,ulTimerId,pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_AC + ucHead].ulReStartTimerId);
#if 0 // for debugging purpose.
{

				hError = SVC_EPG_MakeEpgMessage(EPG_CMD_CHECK_STATUS, eEPG_PROCESS_EIT_SCH, &ulMessage);
				hError = SVC_EPG_SendMessage(ulMessage, 0, (HUINT32)0, (HUINT32)0);
}
#endif
			}

#else
			HxLOG_Info("EPG_CMD_NEXT_FILTER ucHead(%02d)\n", ucHead);
			hError = xsvc_epgcontext_SchStartNextFilter_Base(	&(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_AC + ucHead]),
													hAction,
													pCnxtData->hService,
													ucHead,
													pCnxtData->ulDemuxId,
													usTsUniqueId);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Error("local_cnxtpf_StartNextFilter fail!!\n");
				goto EXIT_CNXTRAWSCH_FUNC;
			}
#endif
			break;

		case EPG_CMD_CONTEXT :
			HxLOG_Print("EPG_CMD_CONTEXT\n");
			break;
#if defined(CONFIG_DEBUG)
		case EPG_CMD_PRINT:
			xsvc_epgcontext_SchDebugFunction_Base(pstMsg->ulParam2,0);
			break;
		case EPG_CMD_CHECK_DONE:
			xsvc_epgcontext_SchDebugFunction_Base(pstMsg->ulParam2,1);
			break;
		case EPG_CMD_CHECK_STATUS:
			xsvc_epgcontext_SchDebugStatus_Base();
			break;
#endif
		default :
			break;


	}

	VK_TASK_Sleep(100);

	hError = ERR_EPG_OK;

EXIT_CNXTRAWSCH_FUNC :

	return hError;

}



/*****************************************************
 * Name : xsvc_epgcontext_StopSchFilter_Base
 *
 *
 *
 *****************************************************/
HERROR xsvc_epgcontext_StopSchFilter_Base(Handle_t hAction)
{
	HERROR				hError = ERR_EPG_FAIL;
	svcEpg_Context_t 		*pContext = NULL;
	svcEpg_ContextData_t		*pCnxtData = NULL;
	//HUINT32				ulSemId;

	HxLOG_Debug("(+)Enter!!\n");

	// we don't need any more, because PF & SCH module using only 1 semaphore.
	//SVC_EPGCONTEXT_GetSemId(&ulSemId);

	hError = SVC_EPGCONTEXT_GetContext(hAction, eEPG_PROCESS_EIT_SCH, &pContext);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("SVC_EPGCONTEXT_GetContext fail!!\n");
		goto EXIT_CNXTRAWSCH_FUNC_NO_SEM_REL;
	}
	if(pContext->eStatus == eEPG_CNXT_EMPTY)
	{
		HxLOG_Error("SVC_EPGCONTEXT_GetContext fail!!\n");
		goto EXIT_CNXTRAWSCH_FUNC_NO_SEM_REL;
	}

	//VK_SEM_Get( ulSemId);
	if(pContext->eStatus != eEPG_CNXT_USED || pContext->pCnxtData == NULL)
	{
		HxLOG_Error("context fail!!\n");
		hError = ERR_EPG_FAIL;
		goto EXIT_CNXTRAWSCH_FUNC;
	}

	pCnxtData = pContext->pCnxtData;

#if defined(CONFIG_MW_EPG_NEXT_EITFILTER_TIMER)
	if (VK_TIMERID_NULL != pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_AC].ulReStartTimerId)
	{
		VK_TIMER_Cancel(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_AC].ulReStartTimerId);
		pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_AC].ulReStartTimerId = VK_TIMERID_NULL;
	}

	if (VK_TIMERID_NULL != pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_OTH].ulReStartTimerId)
	{
		VK_TIMER_Cancel(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_OTH].ulReStartTimerId);
		pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_OTH].ulReStartTimerId = VK_TIMERID_NULL;
	}
#endif

	if(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_AC].eState != eEPG_CNXT_FILTER_EMPTY)
	{
		hError = xsvc_epgcontext_SchStopFilter_Base(	&(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_AC]) );
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Error("xsvc_epgcontext_SchStopFilter_Base fail!!\n");
			goto EXIT_CNXTRAWSCH_FUNC;
		}
		HxSTD_memset(&(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_AC].stBaseInfo), 0x00, sizeof(svcEpg_ServiceInfo_t));
		pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_AC].ulRequestId = 0;
	}
	if(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_OTH].eState != eEPG_CNXT_FILTER_EMPTY)
	{
		hError = xsvc_epgcontext_SchStopFilter_Base(	&(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_OTH]) );
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Error("xsvc_epgcontext_SchStopFilter_Base fail!!\n");
			goto EXIT_CNXTRAWSCH_FUNC;
		}
		HxSTD_memset(&(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_OTH].stBaseInfo), 0x00, sizeof(svcEpg_ServiceInfo_t));
		pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_OTH].ulRequestId = 0;
	}

	hError = ERR_EPG_OK;

EXIT_CNXTRAWSCH_FUNC :
	//VK_SEM_Release(ulSemId);

EXIT_CNXTRAWSCH_FUNC_NO_SEM_REL:

	HxLOG_Debug("Leave Error!! (hError:0x%X)\n",hError);

	return hError;
}



/*****************************************************
 * Name : xsvc_epgcontext_SchProcessEitCallBack_Base
 *
 *
 *
 *****************************************************/
HERROR xsvc_epgcontext_SchProcessEitCallBack_Base(PalSef_ReceiveMessage_t *pstMsg, HUINT32 ulParam)
{
	HERROR						hError = ERR_EPG_FAIL;
	DbSvc_TripleId_t				stTripleId;
	svcEpg_ServiceInfo_t			stServiceInfo;
	EPG_SpecType_e				eSpec = eEPG_SPEC_BASE;
	Handle_t						hAction = 0;
	Handle_t						hSvcHandle = HANDLE_NULL;
	svcEpg_Filter_t 				stCnxtFilterInfo;
	HUINT32						ulCnxtFilterSlot = 0;
	HUINT32						ulOrgDeliType = 0;
	HUINT8						ucAntId = 0;
	HUINT8						ucHead = ACT_HEAD;
	HBOOL						bSectionConflict = FALSE;
	HBOOL						bCompleteCycle = FALSE;
	HBOOL						bCompleteSection = FALSE;
	HBOOL						bVersionChange = FALSE;
	HBOOL						pbIsFirstTime = FALSE;

	HUINT16						usTableId = 0;
	UNIXTIME					ulSystemTime = 0;
	HUINT16						usTsUniqueId = 0;
	POINTER_LIST_T				*pSectionList = NULL;
	HUINT32						ulMessage = 0;
	PalSef_RawData_t			*pSiRawData = NULL;
	HUINT16						usSectionIndex = 0;
	HBOOL						bAddSection = FALSE;
	HUINT32						ulSemId;

	SVC_EPGCONTEXT_GetSemId(&ulSemId);
	VK_SEM_Get(ulSemId);

	if(pstMsg == NULL)
	{
		HxLOG_Error("pstMsg:0x%X\n",pstMsg);
		goto  EXIT_CNXTRAWSCH_FUNC;
	}
	usTableId = pstMsg->ucTableId;

	if(pstMsg->eSecPackageFlag != eSectionPackage_On)
	{
		HUINT8			*pucBuffer= (HUINT8*)pstMsg->ulDataAddr;
		
		HxLOG_Error("eSecPackageFlag != eSectionPackage_On hAction[%x]!!\n", hAction);
		if (NULL != pucBuffer)
		{
			OxSI_Free(pucBuffer);
		}

		pSiRawData = NULL;
		goto EXIT_CNXTRAWSCH_FUNC;
	}

	pSiRawData = (PalSef_RawData_t*)pstMsg->ulDataAddr;
	if(pSiRawData == NULL)
	{
		HxLOG_Error("pSiRawData == NULL!!\n");
		goto  EXIT_CNXTRAWSCH_FUNC;
	}

	ucHead = ACT_HEAD;
	ulCnxtFilterSlot = EPGCNXT_FITER_SLOT_EIT_SCH_AC;
	if(usTableId & EIT_OTHER_SCHEDULE_TABLE_ID_MASK)
	{
		ulCnxtFilterSlot = EPGCNXT_FITER_SLOT_EIT_SCH_OTH;
		ucHead = OTH_HEAD;
	}

#if	defined(CONFIG_DEBUG)
	for(usSectionIndex = 0; usSectionIndex < pSiRawData->num_sec; usSectionIndex++)
	{
		hError = SVC_EPGEIT_GetSectionInfo(eEPG_SPEC_BASE, pSiRawData->buf[usSectionIndex], &stServiceInfo);
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Error("(%d/%d) Error Section!!\n", usSectionIndex, pSiRawData->num_sec);
			continue;
		}
		HxLOG_Debug("\t tick(%d) (%02d/%02d) ucSectionNumber(0x%02X) ucTableId(0x%02X) usSvcId(%05d) usOnId(%04X) usTsId(%05X) ulCrc(%X)\n",
					(HUINT32)(VK_TIMER_GetSystemTick()/1000),
					usSectionIndex,
					pSiRawData->num_sec,
					stServiceInfo.ucSectionNumber,
					stServiceInfo.ucTableId,
					stServiceInfo.usSvcId,
					stServiceInfo.usOnId,
					stServiceInfo.usTsId,
					stServiceInfo.ulCrc);

	}
#endif /* #if		defined(CONFIG_DEBUG) */

	hAction = (Handle_t)pstMsg->ulUserData;

	hError = xsvc_epgcontext_SchGetContextInfo_Base(hAction, ulCnxtFilterSlot, &hSvcHandle, &ulOrgDeliType, &ucAntId, &eSpec, &stCnxtFilterInfo);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("xsvc_epgcontext_SchGetContextInfo_Base ret:0x%X hAction[%x]!!\n",hError, hAction);
		goto  EXIT_CNXTRAWSCH_FUNC;
	}
	if(stCnxtFilterInfo.bTimeOut == TRUE)
	{
		HxLOG_Error("stCnxtFilterInfo.bTimeOut == TRUE hAction[%x]!!\n", hAction);
		goto  EXIT_CNXTRAWSCH_FUNC;
	}
	DB_SVC_GetUniqueIdFromSvcHandle(hSvcHandle, &usTsUniqueId);

	if(stCnxtFilterInfo.bComplete)
	{
		HxLOG_Error("stCnxtFilterInfo.bComplete:%d hAction[%x]!!\n",stCnxtFilterInfo.bComplete,hAction);
		goto EXIT_CNXTRAWSCH_FUNC;
	}
	if(pstMsg->ulRequestId != stCnxtFilterInfo.ulRequestId)
	{
		HxLOG_Error("pstMsg->ulRequestId(0x%X) != stCnxtFilterInfo.ulRequestId(0x%X) hAction[%x]!!\n", pstMsg->ulRequestId,stCnxtFilterInfo.ulRequestId,  hAction);
		goto  EXIT_CNXTRAWSCH_FUNC;
	}

	for(usSectionIndex = 0; usSectionIndex < pSiRawData->num_sec; usSectionIndex++)
	{
		bAddSection = FALSE;
		bCompleteCycle = FALSE;
#if		defined(CONFIG_DEBUG)
		stCnxtFilterInfo.ulSectionNum++;
#endif
		if(stCnxtFilterInfo.bComplete == TRUE)
		{
			HxLOG_Debug("bComplete hAction[%x]!!\n", hAction);
			goto END_SECION_LOOP;
		}

		hError = SVC_EPGEIT_GetSectionInfo(eSpec, pSiRawData->buf[usSectionIndex], &stServiceInfo);
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Error("SVC_EPGEIT_GetSectionInfo fail hAction[%x]!!\n", hAction);
			goto END_SECION_LOOP;
		}

/**********************************************************************************
  *
  * 1. check cycle & add section number
  *
  **********************************************************************************/
		if(	stCnxtFilterInfo.stBaseInfo.ucSectionNumber == stServiceInfo.ucSectionNumber
			&& stCnxtFilterInfo.stBaseInfo.ucTableId == stServiceInfo.ucTableId)
		{
			if(	stCnxtFilterInfo.stBaseInfo.usSvcId == stServiceInfo.usSvcId
				&& stCnxtFilterInfo.stBaseInfo.usOnId == stServiceInfo.usOnId
				&& stCnxtFilterInfo.stBaseInfo.usTsId == stServiceInfo.usTsId
			)
			{
				bCompleteCycle = TRUE;
			}
		}
		if(bCompleteCycle == FALSE)
		{
			hError = SVC_EPGSECTION_CheckSvcSection(hAction, usTsUniqueId, &stServiceInfo, &bSectionConflict, &bVersionChange, &pbIsFirstTime);
			if(hError != ERR_EPG_OK || bSectionConflict == TRUE)
			{
				HxLOG_Debug("EPGRAWDB_CheckSvcSection invalid or conflict(%d) hAction[%x]!!\n",bSectionConflict,  hAction);
				goto END_SECION_LOOP;
			}

			if(bVersionChange == TRUE && pbIsFirstTime == FALSE)
			{
				HxLOG_Info("EPGRAWDB_CheckSvcSection invalid or bVersionChange(%d)  hAction[%x]!!\n",bVersionChange,  hAction);
				svc_epg_DeleteFromDama (&stServiceInfo, ulOrgDeliType, ucAntId);
			}
		}

/**********************************************************************************
  *
  * 2. if cycle true, check section complete
  *
  **********************************************************************************/
		if(bCompleteCycle == TRUE
		|| (stCnxtFilterInfo.eState == eEPG_CNXT_FILTER_MONITOR && bVersionChange == TRUE)
		)
		{
			stCnxtFilterInfo.ucCycle++;
			HxLOG_Print("bCompleteCycle[%d] bVersionChange[%d]\n",bCompleteCycle, bVersionChange);
			/** cycle notify: context에서 처리 */
			if(bCompleteCycle == TRUE)
			{
				hError = SVC_EPGSECTION_CheckSectionComplete(hAction, (HUINT8)usTableId, &bCompleteSection);
				if(hError != ERR_EPG_OK || bCompleteSection == FALSE)
				{
					HxLOG_Info("(%02d/%02d) ucHead(%x)GetSystemTick(%d)ucSectionNumber(%d)ucTableId(0x%02X) not complete hAction[%x]!!\n",
													usSectionIndex,	stCnxtFilterInfo.ulSectionNum, ucHead,	(HUINT32)(VK_TIMER_GetSystemTick()/1000),
													stServiceInfo.ucSectionNumber, stServiceInfo.ucTableId,  hAction);
					goto  END_SECION_LOOP;
				}
			}
			HxLOG_Debug("%s-SCHEDULE complete(%d) Send EPG_CMD_STOP_FILTER hAction[%x]\n",	((ACT_HEAD == ucHead) ? "ACT_HEAD":"OTH_HEAD"),(HINT32)VK_TIMER_GetSystemTick()/1000, hAction);
			hError = SVC_EPG_MakeEpgMessage(EPG_CMD_STOP_FILTER, eEPG_PROCESS_EIT_SCH, &ulMessage);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Error("MW_EPGMAIN_MakeEpgMessage fail hAction[%x]!!\n", hAction);
			}
			hError = SVC_EPG_SendMessage(ulMessage, hAction, ucHead, 0);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Error("SendMessage fail hAction[%x]!!\n", hAction);
			}

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
			if (bCompleteSection == TRUE)
			{
				hError = SVC_EPG_MgrPostMessage(eSEVT_EPG_FPB_UPDATE_SCHEDULE_SERIES_SET, 0, ACT_SC_HEAD, 0, 0);
				if(hError != ERR_EPG_OK)
				{
					HxLOG_Error("SVC_EPG_MgrPostMessage fail hAction[%x]!!\n", hAction);
				}
			}
#endif
			stCnxtFilterInfo.bComplete = TRUE;
			hError = xsvc_epgcontext_SchSetContextFilterInfo_Base(hAction, ulCnxtFilterSlot, &stCnxtFilterInfo);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Error("xsvc_epgcontext_SchSetContextFilterInfo_Base fail hAction[%x]!!\n", hAction);
				goto  END_SECION_LOOP;
			}
			goto  END_SECION_LOOP;
		}
/**********************************************************************************
  *
  * 3. first section process
  *
  **********************************************************************************/
		else
		{
			if(stCnxtFilterInfo.stBaseInfo.ulCrc == 0)
			{
				ulSystemTime = VK_TIMER_GetSystemTick()/1000;
				HxSTD_memcpy(&(stCnxtFilterInfo.stBaseInfo), &stServiceInfo, sizeof(svcEpg_ServiceInfo_t));
				stCnxtFilterInfo.ulBaseTime = ulSystemTime;

				hError = xsvc_epgcontext_SchSetContextFilterInfo_Base(hAction, ulCnxtFilterSlot, &stCnxtFilterInfo);
				if(hError != ERR_EPG_OK)
				{
					HxLOG_Error("xsvc_epgcontext_SchSetContextFilterInfo_Base fail hAction[%x]!!\n", hAction);
					HxSTD_memset(&(stCnxtFilterInfo.stBaseInfo), 0x00, sizeof(svcEpg_ServiceInfo_t));
					goto  END_SECION_LOOP;
				}
				HxLOG_Info("\n\nstart(%d) hAction[%x]\n\n", ulSystemTime, hAction);
			}
		}

/**********************************************************************************
  *
  * 4. check valid
  *
  **********************************************************************************/
		HxSTD_memset(&stTripleId, 0x00, sizeof(DbSvc_TripleId_t));
		stTripleId.usOnId = stServiceInfo.usOnId;
		stTripleId.usTsId = stServiceInfo.usTsId;
		stTripleId.usSvcId = stServiceInfo.usSvcId;
		hError = SVC_EPGSERVICE_ValidSvcCheckServiceValidation(ulOrgDeliType, ucAntId, &stTripleId);
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Debug("EPGRAWDB_CheckServiceValidation fail hAction[%x]!!\n", hAction);
			goto  END_SECION_LOOP;
		}

 /**********************************************************************************
  *
  * 5. register db
  *
  **********************************************************************************/
		pSectionList = UTIL_LINKEDLIST_AppendListItemToPointerList(pSectionList, pSiRawData->buf[usSectionIndex]);
		bAddSection = TRUE;

END_SECION_LOOP :
		if(bAddSection == TRUE)
		{
			pSiRawData->buf[usSectionIndex] = NULL;
		}
	}

	if(pSectionList)
	{
		hError = SVC_EPGLIB_AddSectionList(hSvcHandle,			eSpec,
											ulOrgDeliType,		ucAntId,
											usTableId,			pSectionList);
	}

	if(hError != ERR_EPG_OK)
	{
		HxLOG_Debug("SVC_EPGDB_AddSectionList fail!!hAction[%x]\n", hAction);
		goto  EXIT_CNXTRAWSCH_FUNC;
	}

	hError = ERR_EPG_OK;

EXIT_CNXTRAWSCH_FUNC :
	if(pSiRawData != NULL)
	{
		for(usSectionIndex = 0; usSectionIndex < pSiRawData->num_sec; usSectionIndex++)
		{
			if(pSiRawData->buf[usSectionIndex])
			{
				OxSI_Free(pSiRawData->buf[usSectionIndex]);
				pSiRawData->buf[usSectionIndex] = NULL;
			}
		}
		OxMW_Free(pSiRawData);
		pSiRawData = NULL;
	}

	VK_SEM_Release(ulSemId);

	return hError;
}

#define ____LOCAL_FUNC____

/*****************************************************
 * Name : xsvc_epgcontext_SchStartFilter_Base
 *
 *
 *
 *****************************************************/
STATIC HERROR xsvc_epgcontext_SchStartFilter_Base(	svcEpg_Filter_t *pstFilterInfo,
														Handle_t hAction,
														HUINT8 ucHead,
														HUINT32 ulDemuxId,
														HUINT16 usTsUniqueId,
														HINT32 lFilterState)
{
	HERROR				hError = ERR_EPG_FAIL;
	HUINT32				ulRequestId = 0,ulMsgQId, ulEmergencyQId;
	HUINT16				usTableId = 0;
	HUINT8				aucFilterMode[PAL_SEF_MAX_FILTER_LEN] = {0, };
	HUINT8				aucFilterValue[PAL_SEF_MAX_FILTER_LEN] = {0, };	/**< section filter value */
	HUINT8				aucFilterMask[PAL_SEF_MAX_FILTER_LEN] = {0, };	/**< section filter mask */
	HBOOL				bUseExtId = FALSE;
	HBOOL				bUseVerNum = FALSE;
	HUINT16				usExtId = 0;
	HUINT8				ucVerNum = 0;
	PalSef_FilterLength_e		eFilterLen = eSefFilterLen_4Bytes;
	HINT32				lPackageCount = 0;
	HUINT32				ulPackageDuration = 0;


	if(pstFilterInfo == NULL)
	{
		HxLOG_Error("param fail!!\n");
		goto EXIT_CNXTRAWSCH_FUNC;
	}

	usTableId = EIT_ACTUAL_SCHEDULE_TABLE_ID_0;
	lPackageCount = RAW_SECTION_PACKAGE_ACT_COUNT;
	ulPackageDuration = RAW_SECTION_PACKAGE_DURATION;
	if(ucHead == OTH_HEAD)
	{
		usTableId = EIT_OTHER_SCHEDULE_TABLE_ID_0;
		lPackageCount = RAW_SECTION_PACKAGE_OTH_COUNT;
		ulPackageDuration = RAW_SECTION_PACKAGE_DURATION;
	}

	SVC_EPGCONTEXT_GetMsgQId(&ulMsgQId);
	SVC_EPGCONTEXT_GetEmergencyMsgQId(&ulEmergencyQId);

	HxSTD_memset(&(pstFilterInfo->stBaseInfo), 0x00, sizeof(svcEpg_ServiceInfo_t));

	aucFilterValue[0] = (HUINT8)usTableId;
	aucFilterMask[0]  = 0xf0;

	bUseExtId = FALSE;
	bUseVerNum = FALSE;
	if(bUseExtId == TRUE)
	{
		aucFilterValue[1] = (HUINT8)((usExtId >> 8) & 0xff);
		aucFilterValue[2] = (HUINT8)(usExtId & 0xff);
		aucFilterMask[1] = 0xff;
		aucFilterMask[2] = 0xff;
	}
	else
	{
		aucFilterMask[1] = 0;
		aucFilterMask[2] = 0;
	}

	if(bUseVerNum == TRUE)
	{
		aucFilterMask[3] = 0x3E;
		aucFilterValue[3] = (ucVerNum & 0x1f) << 1;
	}
	else
	{
		aucFilterMask[3] = 0;
	}

	if(lFilterState == eEPG_CNXT_FILTER_MONITOR)
	{
		// only section number '0' monitor
		aucFilterMask[4] = 0xff;
		aucFilterValue[4] = 0;
		eFilterLen = eSefFilterLen_8Bytes;
	}

	hError = PAL_SEF_EnableSectionFilter(	(HUINT8)ulDemuxId,
										EIT_PID,
										ulMsgQId,
										ulEmergencyQId,
										lPackageCount,
										ulPackageDuration,
										eFilterLen,
										aucFilterValue,
										aucFilterMask,
										aucFilterMode,
										hAction,
										&ulRequestId,
										TRUE);
	if(hError != ERR_OK)
	{
		HxLOG_Error("PAL_SEF_EnableSectionFilter fail!!\n");
		goto EXIT_CNXTRAWSCH_FUNC;
	}

	HxLOG_Debug("\n\t Start EIT Raw Schedule Filter\n\tDemuxId:%d FilterMode:0x%X ReqId:0x%X lPackageCount:%d ulPackageDuration:%d\n\t\tFilterValue:[0x%02X][0x%02X][0x%02X][0x%02X]\n\t\tMaskValue:[0x%02X][0x%02X][0x%02X][0x%02X]\n\n",
			ulDemuxId,aucFilterMode[0],ulRequestId,lPackageCount,ulPackageDuration,aucFilterValue[0],aucFilterValue[1],aucFilterValue[2],aucFilterValue[3],
			aucFilterMask[0],aucFilterMask[1],aucFilterMask[2],aucFilterMask[3]);

	pstFilterInfo->eState = lFilterState;
	pstFilterInfo->usPid = EIT_PID;
	pstFilterInfo->ulTable = usTableId;
	pstFilterInfo->ulRequestId = ulRequestId;
	pstFilterInfo->ulBufferSize = (HUINT32)lPackageCount;
	pstFilterInfo->ucCycle = 0;
	pstFilterInfo->bComplete= FALSE;
	pstFilterInfo->bTimeOut = FALSE;

	hError = ERR_EPG_OK;

EXIT_CNXTRAWSCH_FUNC :

	return hError;
}

/*****************************************************
 * Name : xsvc_epgcontext_SchStartNextFilter_Base
 *
 *
 *
 *****************************************************/
STATIC HERROR				xsvc_epgcontext_SchStartNextFilter_Base(svcEpg_Filter_t *pstFilterInfo, Handle_t hAction, Handle_t hService, HUINT8 ucHead, HUINT32 ulDemuxId, HUINT16 usTsUniqueId)
{
	HERROR				hError = ERR_EPG_FAIL;

	if(pstFilterInfo == NULL)
	{
		HxLOG_Error(" param fail!!\n");
		goto EXIT_CNXTRAWSCH_FUNC;
	}

	switch(pstFilterInfo->eState)
	{
		case eEPG_CNXT_FILTER_CONTINUE :
		case eEPG_CNXT_FILTER_NEXTCONTINUE :
			if(pstFilterInfo->bComplete)
			{
				HxSTD_memcpy(&(pstFilterInfo->stLastCompleteInfo), &(pstFilterInfo->stBaseInfo),  sizeof(svcEpg_ServiceInfo_t));
			}
			/* 단 하나의 section도 들어오지 않은 상태라면 */
			if(pstFilterInfo->stBaseInfo.ucTableId == 0)
			{
				hError = xsvc_epgcontext_SchStartFilter_Base(	pstFilterInfo,
													hAction,
													ucHead,
													ulDemuxId,
													usTsUniqueId,
													eEPG_CNXT_FILTER_CONTINUE);
				if(hError != ERR_EPG_OK)
				{
					HxLOG_Error(" xsvc_epgcontext_SchStartFilter_Base fail!!\n");
					goto EXIT_CNXTRAWSCH_FUNC;
				}
				HxLOG_Info(" ucHead(%d) filter eEPG_CNXT_FILTER_CONTINUE \n\n\n", ucHead);
			}
			/** 1.  complete이면 monitor mode로 전환 */
			/** 2.  section을 받는중 time out이라면 monitor mode로*/
			else
			{
				hError = xsvc_epgcontext_SchStartFilter_Base(	pstFilterInfo,
													hAction,
													ucHead,
													ulDemuxId,
													usTsUniqueId,
													eEPG_CNXT_FILTER_MONITOR);
				if(hError != ERR_EPG_OK)
				{
					HxLOG_Error(" xsvc_epgcontext_SchStartFilter_Base fail!!\n");
					goto EXIT_CNXTRAWSCH_FUNC;
				}
				HxLOG_Info(" ucHead(%d) complete eEPG_CNXT_FILTER_MONITOR \n\n\n", ucHead);
			}
			break;

		case eEPG_CNXT_FILTER_MONITOR :
			/* 모니터링중 timeout 이면 계속해서 모니터링하도록 한다 */
			if(pstFilterInfo->bTimeOut == TRUE)
			{
				hError = xsvc_epgcontext_SchStartFilter_Base(	pstFilterInfo,
													hAction,
													ucHead,
													ulDemuxId,
													usTsUniqueId,
													eEPG_CNXT_FILTER_MONITOR);
				if(hError != ERR_EPG_OK)
				{
					HxLOG_Error(" xsvc_epgcontext_SchStartFilter_Base fail!!\n");
					goto EXIT_CNXTRAWSCH_FUNC;
				}
				HxLOG_Info(" ucHead(%d) timeout!! eEPG_CNXT_FILTER_MONITOR \n\n\n", ucHead);
			}
			else
			{
				hError = SVC_EPGSECTION_InitSvcSectionManager(hAction, usTsUniqueId, pstFilterInfo->stLastCompleteInfo.ucTableId);
				if(hError != ERR_EPG_OK)
				{
					HxLOG_Error(" EPGRAWDB_InitSvcSectionManager fail!!\n");
					goto EXIT_CNXTRAWSCH_FUNC;
				}
				hError = xsvc_epgcontext_SchStartFilter_Base(	pstFilterInfo,
													hAction,
													ucHead,
													ulDemuxId,
													usTsUniqueId,
													eEPG_CNXT_FILTER_NEXTCONTINUE);
				if(hError != ERR_EPG_OK)
				{
					HxLOG_Error(" xsvc_epgcontext_SchStartFilter_Base fail!!\n");
					goto EXIT_CNXTRAWSCH_FUNC;
				}
				HxLOG_Info(" ucHead(%d) version change!! eEPG_CNXT_FILTER_CONTINUE \n\n\n", ucHead);
			}
			break;
	}

	hError = ERR_EPG_OK;

EXIT_CNXTRAWSCH_FUNC :

	return hError;
}

/*****************************************************
 * Name : xsvc_epgcontext_SchStopFilter_Base
 *
 *
 *
 *****************************************************/
STATIC HERROR xsvc_epgcontext_SchStopFilter_Base(svcEpg_Filter_t *pstFilterInfo)
{
	HERROR				hError = ERR_EPG_FAIL;

	if(pstFilterInfo == NULL)
	{
		HxLOG_Error("param fail!!\n");
		goto EXIT_CNXTRAWSCH_FUNC;
	}

	if(pstFilterInfo->ulRequestId == 0)
	{
		HxLOG_Error("invalid ulRequestId id!!\n");
		hError = ERR_OK;
		goto EXIT_CNXTRAWSCH_FUNC;
	}

	HxLOG_Print("\n\t Stop EIT Raw Schedule Filter\n\tDemuxId:%d ReqId:0x%X\n\n",
			pstFilterInfo->ucDemuxId,pstFilterInfo->ulRequestId);

	hError = PAL_SEF_DisableSectionFilter(pstFilterInfo->ucDemuxId, pstFilterInfo->ulRequestId);
	if(hError != ERR_OK)
	{
		HxLOG_Error("PAL_SEF_DisableSectionFilter fail!!\n");
		goto EXIT_CNXTRAWSCH_FUNC;
	}
	pstFilterInfo->ulRequestId = 0;

	hError = ERR_EPG_OK;

EXIT_CNXTRAWSCH_FUNC :

	return hError;
}

/*****************************************************
 * Name : xsvc_epgcontext_SchGetContextInfo_Base
 *
 *
 *
 *****************************************************/
STATIC HERROR				xsvc_epgcontext_SchGetContextInfo_Base(Handle_t hAction, HUINT32 ulIndex, Handle_t *phSvcHandle, HUINT32 *pulOrgDeliType, HUINT8 *pucAntId, EPG_SpecType_e *peSpec, svcEpg_Filter_t *pEpgContextFilterInfo)
{
	HERROR				hError = ERR_EPG_FAIL;
	svcEpg_Context_t 		*pContext = NULL;
	svcEpg_ContextData_t		*pCnxtData = NULL;

	HxLOG_Debug("(+)Enter!!\n");

//	SVC_EPGCONTEXT_GetSemId(&ulSemId);

	if(phSvcHandle == NULL || pEpgContextFilterInfo == NULL )
	{
		HxLOG_Error("param fail!!\n");
		return hError;
	}

	hError = SVC_EPGCONTEXT_GetContext(hAction, eEPG_PROCESS_EIT_SCH, &pContext);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("SVC_EPGCONTEXT_GetContext fail!!\n");
		return hError;
	}

	HxLOG_Debug("pContext->eStatus:0x%X,pContext->pCnxtData:0x%X\n",pContext->eStatus,pContext->pCnxtData);

	if(pContext->eStatus == eEPG_CNXT_EMPTY)
	{
		HxLOG_Error("SVC_EPGCONTEXT_GetContext fail!!\n");
		return hError;
	}

//	VK_SEM_Get(ulSemId);
	if(pContext->eStatus != eEPG_CNXT_USED || pContext->pCnxtData == NULL)
	{
		HxLOG_Error("context fail!!\n");
		hError = ERR_EPG_FAIL;
		goto EXIT_CNXTRAWSCH_FUNC;
	}

	pCnxtData = pContext->pCnxtData;
	*pulOrgDeliType = pCnxtData->ulOrgDeliveryType;
	*pucAntId = pCnxtData->ucAntId;
	*phSvcHandle = pCnxtData->hService;
	*peSpec = pCnxtData->eSpec;
	HxSTD_memcpy(pEpgContextFilterInfo, &(pCnxtData->astFilterTable[ulIndex]), sizeof(svcEpg_Filter_t));

	hError = ERR_EPG_OK;

EXIT_CNXTRAWSCH_FUNC :
//	VK_SEM_Release(ulSemId);

	return hError;
}

/*****************************************************
 * Name : xsvc_epgcontext_SchSetContextFilterInfo_Base
 *
 *
 *
 *****************************************************/
STATIC HERROR				xsvc_epgcontext_SchSetContextFilterInfo_Base(Handle_t hAction, HUINT32 ulIndex, svcEpg_Filter_t *pEpgContextFilterInfo)
{
	HERROR				hError = ERR_EPG_FAIL;
	svcEpg_Context_t 		*pContext = NULL;
	svcEpg_ContextData_t		*pCnxtData = NULL;

	HxLOG_Debug("(+)Enter!!\n");

//	SVC_EPGCONTEXT_GetSemId(&ulSemId);

	if(pEpgContextFilterInfo == NULL)
	{
		HxLOG_Error("param fail!!\n");
		return hError;
	}

	hError = SVC_EPGCONTEXT_GetContext(hAction, eEPG_PROCESS_EIT_SCH, &pContext);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("SVC_EPGCONTEXT_GetContext fail!!\n");
		return hError;
	}
	if(pContext->eStatus == eEPG_CNXT_EMPTY)
	{
		HxLOG_Error("SVC_EPGCONTEXT_GetContext fail!!\n");
		return hError;
	}
//	VK_SEM_Get(ulSemId);
	if(pContext->eStatus != eEPG_CNXT_USED || pContext->pCnxtData == NULL)
	{
		HxLOG_Error("context fail!!\n");
		hError = ERR_EPG_FAIL;
		goto EXIT_CNXTRAWSCH_FUNC;
	}

	pCnxtData = pContext->pCnxtData;
	if(pCnxtData->astFilterTable[ulIndex].usFilterId != pEpgContextFilterInfo->usFilterId)
	{
		HxLOG_Error("pCnxtData->astFilterTable[ulIndex].usFilterId != pEpgContextFilterInfo->usFilterId!!\n");
		hError = ERR_EPG_FAIL;
		goto EXIT_CNXTRAWSCH_FUNC;
	}

	HxSTD_memcpy(&(pCnxtData->astFilterTable[ulIndex]), pEpgContextFilterInfo, sizeof(svcEpg_Filter_t));
	hError = ERR_EPG_OK;

EXIT_CNXTRAWSCH_FUNC :
//	VK_SEM_Release(ulSemId);

	return hError;
}

/*****************************************************
 * Name : _l_cnxtrawsch_CheckEitVersion
 *
 *
 *
 *****************************************************/
// receive task

#if defined(CONFIG_DEBUG)
extern	void SVC_EPGRAWSECTION_PrintContent(HUINT32 ulType);
//	Debug니까 그냥 호출해서 쓸수 있도록만??

extern HERROR 	MGR_LIVE_GetServiceHandle(HUINT32 ulActionId, HINT32 eSvcType, Handle_t *phSvc);
extern HUINT32 MGR_ACTION_GetMainActionId(void);

/*****************************************************
 * Name : xsvc_epgcontext_SchDebugStatus_Base
 *
 *
 *
 *****************************************************/
STATIC void xsvc_epgcontext_SchDebugStatus_Base(void)
{
	Handle_t				hAction = 0;
	Handle_t				hSvcHandle;
	HUINT32				ulOrgDeliType;
	EPG_SpecType_e		eSpec;
	svcEpg_Filter_t		stEitInfo;
	HERROR				hError;
	DbSvc_Info_t			stSvcInfo;
	svcEpg_Context_t 		*pContext = NULL;
	svcEpg_ContextData_t		*pCnxtData = NULL;

	hError = SVC_EPGCONTEXT_GetContext(hAction, eEPG_PROCESS_EIT_SCH, &pContext);
	if(hError != ERR_EPG_OK)
	{
		HLIB_CMD_Printf("[%s] xsvc_epgcontext_SchGetContextInfo_Base return : 0x%X\n",__FUNCTION__,hError);
		return;
	}

	pCnxtData = pContext->pCnxtData;
	ulOrgDeliType = pCnxtData->ulOrgDeliveryType;
	hSvcHandle = pCnxtData->hService;
	eSpec= pCnxtData->eSpec;
	HxSTD_memcpy(&stEitInfo, &(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_AC]), sizeof(svcEpg_Filter_t));

	HLIB_CMD_Printf("\n== RAWEPG ACT SCHEDULE\n");
	HLIB_CMD_Printf("\thSvc:0x%X OrgDelType:0x%X eSpec:0x%X\n",hSvcHandle,ulOrgDeliType,eSpec);
	hError = DB_SVC_GetServiceInfo (hSvcHandle, &stSvcInfo);
	if (ERR_OK == hError)
	{
		HLIB_CMD_Printf("\t[%04d] - SvcId:0x%X - %s\n", DbSvc_GetLcn(&stSvcInfo), DbSvc_GetSvcId(&stSvcInfo), DbSvc_GetName(&stSvcInfo));
	}
#if defined(CONFIG_MW_EPG_NEXT_EITFILTER_TIMER)
	HLIB_CMD_Printf("\tState:0x%X Tbl:0x%X bComplete:%d ReStartTimeId:0x%X DmxId:%d FilterReqId:0x%X\n",
		stEitInfo.eState,stEitInfo.ulTable,stEitInfo.bComplete,stEitInfo.ulReStartTimerId,stEitInfo.ucDemuxId,stEitInfo.ulRequestId);
#else
	HLIB_CMD_Printf("\tState:0x%X Tbl:0x%X bComplete:%d DmxId:%d FilterReqId:0x%X\n",
		stEitInfo.eState,stEitInfo.ulTable,stEitInfo.bComplete, stEitInfo.ucDemuxId,stEitInfo.ulRequestId);
#endif
	HxSTD_memcpy(&stEitInfo, &(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_SCH_OTH]), sizeof(svcEpg_Filter_t));

	HLIB_CMD_Printf("\n== RAWEPG OTH SCHEDULE\n");
	HLIB_CMD_Printf("\thSvc:0x%X OrgDelType:0x%X eSpec:0x%X\n",hSvcHandle,ulOrgDeliType,eSpec);
//	hError = DB_SVC_GetServiceInfo (hSvcHandle, &stSvcInfo);
	HLIB_CMD_Printf("\t[%04d] - SvcId:0x%X - %s\n", DbSvc_GetLcn(&stSvcInfo), DbSvc_GetSvcId(&stSvcInfo), DbSvc_GetName(&stSvcInfo));
#if defined(CONFIG_MW_EPG_NEXT_EITFILTER_TIMER)
	HLIB_CMD_Printf("\tState:0x%X Tbl:0x%X bComplete:%d ReStartTimeId:0x%X DmxId:%d FilterReqId:0x%X\n",
		stEitInfo.eState,stEitInfo.ulTable,stEitInfo.bComplete,stEitInfo.ulReStartTimerId,stEitInfo.ucDemuxId,stEitInfo.ulRequestId);
#else
	HLIB_CMD_Printf("\tState:0x%X Tbl:0x%X bComplete:%d DmxId:%d FilterReqId:0x%X\n",
		stEitInfo.eState,stEitInfo.ulTable,stEitInfo.bComplete,stEitInfo.ucDemuxId,stEitInfo.ulRequestId);
#endif

	SVC_EPGRAWSECTION_PrintContent(1);
}

STATIC void xsvc_epgcontext_SchDebugFunction_Base(HUINT32 ulHeadInfo, HUINT32 ulCmdFlag)
{
	// TODO: 새로 구현 필요..
#if 0 // 새로 구현하세요..
	HERROR						hError = ERR_EPG_FAIL;
	HTREEMGR					hTree = NULL;
	HTREEITEM					hDeliItem = NULL;
	HTREEITEM					hTsItem = NULL;
	HTREEITEM					hTsFirstChild = NULL,hSvcFirstChild = NULL;
	HTREEITEM					hTsTempItem = NULL,hSvcTempItem=NULL;
	HUINT16						usTempSvcId = 0,i;
	SvcEpg_ServiceTsInfoRaw_t	stEpgTsInfo;
	Handle_t					hSvc;
	DbSvc_TsInfo_t				stTsInfo;
//	DbSvc_Info_t					stSvcInfo;
	void						*pData = NULL;
	SvcEpg_SvcEventListRaw_t		*pSvcEventList;
	POINTER_LIST_T				*pCurrent = NULL,*pRoot = NULL;
	HUINT8						*pucRawSec;

	HxLOG_Print("\n============ SCHEDULE EIT PRINT ============\n");

	hError = MGR_LIVE_GetServiceHandle (MGR_ACTION_GetMainActionId(), 0, &hSvc);
	if (ERR_OK != hError)
		return;

	hError = DB_SVC_GetTsInfoByServiceHandle(hSvc,&stTsInfo);
	if (ERR_OK != hError)
		return;

//	hError = DB_SVC_GetServiceInfo(hSvc,&stSvcInfo);

	HxLOG_Print("\tHead : %s\n",(0 == ulHeadInfo ? "ACT_HEAD" : "OTH_HEAD"));
	HxLOG_Print("\tCurrent Ts -- OnId:0x%X TsId:0x%X\n",stTsInfo.usOnId,stTsInfo.usTsId);
	HxLOG_Print("============================================\n");

	ENTER_CRITICAL_EPGRAWDB;
	hError = SVC_EPGDB_GetTreeMgr(&hTree);
	if(hError != ERR_TREEUTIL_OK || hTree == NULL)
	{
		HxLOG_Print("local_epgdb_ukdtt_GetTreeMgr fail!!\n");
		goto EXIT_CNXTRAWSC_FUNC;
	}

#if	defined(CONFIG_MW_CH_SATELLITE)
	hError = SVC_EPGDB_FindDelieveryItem(hTree, eDxDELIVERY_TYPE_SAT, &hDeliItem);
#elif defined(CONFIG_MW_CH_CABLE)
	hError = SVC_EPGDB_FindDelieveryItem(hTree, eDxDELIVERY_TYPE_CAB, &hDeliItem);
#else
	hError = SVC_EPGDB_FindDelieveryItem(hTree, eDxDELIVERY_TYPE_TER, &hDeliItem);
#endif
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("EPGDB_FindDelieveryItem fail!!\n");
		goto EXIT_CNXTRAWSC_FUNC;
	}

	hError = UTIL_TREEUTIL_GetChildItem(hDeliItem, &hTsFirstChild);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("GetChildItem fail!!\n");
		goto EXIT_CNXTRAWSC_FUNC;
	}

	hTsTempItem = hTsFirstChild;
	while(hTsTempItem)
	{
		hError = UTIL_TREEUTIL_GetItemUserInfo(hTsTempItem, sizeof(SvcEpg_ServiceTsInfoRaw_t), (void*)&stEpgTsInfo);
		if(hError != ERR_OK)
		{
			HxLOG_Print("GetItemUserInfo fail!!\n");
			goto GOTO_NEXT_TS_ITEM;
		}
		if ((ulHeadInfo != ACT_HEAD) && ( stEpgTsInfo.usTsId == stTsInfo.usTsId && stEpgTsInfo.usOnId == stTsInfo.usOnId))
			goto GOTO_NEXT_TS_ITEM;

		if ((ulHeadInfo == ACT_HEAD) && ( stEpgTsInfo.usTsId != stTsInfo.usTsId || stEpgTsInfo.usOnId != stTsInfo.usOnId))
			goto GOTO_NEXT_TS_ITEM;

		{
			hTsItem = hTsTempItem;
			hError = UTIL_TREEUTIL_GetChildItem(hTsItem, &hSvcFirstChild);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Print("GetChildItem fail!!\n");
				goto GOTO_NEXT_TS_ITEM;
			}

			hSvcTempItem = hSvcFirstChild;
			while(hSvcTempItem)
			{
				hError = UTIL_TREEUTIL_GetItemUserInfo(hSvcTempItem, sizeof(HUINT16), (void*)&usTempSvcId);
				if(hError != ERR_OK)
				{
					HxLOG_Print("GetItemUserInfo fail!!\n");
					goto GOTO_NEXT_SVC_ITEM;
				}

				HxLOG_Print("\tSvcId:0x%X\n",usTempSvcId);
				hError = UTIL_TREEUTIL_GetItemData(hSvcTempItem, &pData);
				if(hError != ERR_TREEUTIL_OK|| pData == NULL)
				{
					HxLOG_Print("UTIL_TREEUTIL_GetItemData fail!!\n");
					goto GOTO_NEXT_SVC_ITEM;
				}
				pSvcEventList = (SvcEpg_SvcEventListRaw_t*)pData;

				pCurrent = pRoot = pSvcEventList->pSchList;
				if (NULL == pCurrent)
				{
					HxLOG_Print("\t\tNULL\n");
					goto GOTO_NEXT_SVC_ITEM;
				}

				if (0 == ulCmdFlag)
				{
					do
					{
						pucRawSec = (HUINT8 *)pCurrent->pvContents; // This is Raw Section.
						HxLOG_Print("\t\tSvcId:0x%X TableId:0x%X SecNum:%d LastSecNum:%d SegLastSecNum:%d LastTableId:0x%X Ver:0x%X\n",
							get16bit(pucRawSec+3),
							get8bit(pucRawSec),get8bit(pucRawSec+6),
							get8bit(pucRawSec+7),get8bit(pucRawSec+12),
							get8bit(pucRawSec+13),(((*(pucRawSec+5)) >> 1) & 0x1F));
						pCurrent = pCurrent->pNext ;
						if ( pCurrent == pRoot )
						{
							break;
						}
					} while ( pCurrent && pCurrent != pRoot ) ;
				}
				else if (1 == ulCmdFlag)
				{
					VK_memset32(s_aaucScSecBuildMask,0xff,(sizeof(HUINT8) * 16 * 32));
					VK_memset32(s_aaucScCmpSecBuildMask,0,(sizeof(HUINT8) * 16 * 32));
					VK_memset32(s_aucScLastSecNum,0,(sizeof(HUINT8) * 16));
					VK_memset32(s_aucScNonRcvSecBuildMask,0xff,(sizeof(HUINT8) * 32));
					{
						HUINT8 ucTableId,ucMaskIdx,ucSecNum,ucLastSecNum,ucSegLastSecNum;

						do
						{
							pucRawSec = (HUINT8 *)pCurrent->pvContents; // This is Raw Section.
							ucTableId = get8bit(pucRawSec);
							ucMaskIdx = (ucTableId & 0x0F);
							ucSecNum = get8bit(pucRawSec+6);
							ucLastSecNum = get8bit(pucRawSec+7);
							ucSegLastSecNum = get8bit(pucRawSec+12);

							DBG_SC_SEGMENTBITMASK(s_aaucScSecBuildMask[ucMaskIdx], ucLastSecNum);
							DBG_SC_SEGMENTBITMASK(s_aaucScSecBuildMask[ucMaskIdx], ucSegLastSecNum);
							DBG_SC_CLEARBITMASK(s_aaucScSecBuildMask[ucMaskIdx], ucSecNum);
							if (s_aucScLastSecNum[ucMaskIdx] < ucLastSecNum)
								s_aucScLastSecNum[ucMaskIdx] = ucLastSecNum;
							pCurrent = pCurrent->pNext ;
							if ( pCurrent == pRoot )
							{
								break;
							}
						} while ( pCurrent && pCurrent != pRoot ) ;

						for (i=0; i < 16; i++)
						{
							if( memcmp((char *)s_aaucScSecBuildMask[i], (char *)s_aaucScCmpSecBuildMask[i], (sizeof(HUINT8)*((s_aucScLastSecNum[i] >> 3) + 1))) == 0)
							{
								HxLOG_Print("\t\tTableId:0x%X is complete\n",((ulHeadInfo == ACT_HEAD) ? i+0x50 : i+0x60));
							}
							else if ( memcmp((char *)s_aaucScSecBuildMask[i], (char *)s_aucScNonRcvSecBuildMask, 32) == 0)
							{
								// SKIP
							}
							else
							{
								HxLOG_Print("\t\tTableId:0x%X is not complete\n", ((ulHeadInfo == ACT_HEAD) ? i+0x50 : i+0x60));
								HxLOG_Print("\t\t\t[0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X]\n",
										s_aaucScSecBuildMask[i][0],s_aaucScSecBuildMask[i][1],s_aaucScSecBuildMask[i][2],s_aaucScSecBuildMask[i][3],
										s_aaucScSecBuildMask[i][4],s_aaucScSecBuildMask[i][5],s_aaucScSecBuildMask[i][6],s_aaucScSecBuildMask[i][7],
										s_aaucScSecBuildMask[i][8],s_aaucScSecBuildMask[i][9],s_aaucScSecBuildMask[i][10],s_aaucScSecBuildMask[i][11],
										s_aaucScSecBuildMask[i][12],s_aaucScSecBuildMask[i][13],s_aaucScSecBuildMask[i][14],s_aaucScSecBuildMask[i][15]);
								HxLOG_Print("\t\t\t[0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X]\n",
										s_aaucScSecBuildMask[i][16],s_aaucScSecBuildMask[i][17],s_aaucScSecBuildMask[i][18],s_aaucScSecBuildMask[i][19],
										s_aaucScSecBuildMask[i][20],s_aaucScSecBuildMask[i][21],s_aaucScSecBuildMask[i][22],s_aaucScSecBuildMask[i][23],
										s_aaucScSecBuildMask[i][24],s_aaucScSecBuildMask[i][25],s_aaucScSecBuildMask[i][26],s_aaucScSecBuildMask[i][27],
										s_aaucScSecBuildMask[i][28],s_aaucScSecBuildMask[i][29],s_aaucScSecBuildMask[i][30],s_aaucScSecBuildMask[i][31]);
							}
						}
					}
				}
GOTO_NEXT_SVC_ITEM:
				hError = UTIL_TREEUTIL_GetNextItem(hSvcTempItem, &hSvcTempItem);
				if(hError != ERR_OK)
				{
					HxLOG_Print("GetNextItem fail!!\n");
					goto EXIT_CNXTRAWSC_FUNC;
				}
			}
		}

GOTO_NEXT_TS_ITEM:
		hError = UTIL_TREEUTIL_GetNextItem(hTsTempItem, &hTsTempItem);
		if(hError != ERR_OK)
		{
			HxLOG_Print("GetNextItem fail!!\n");
			goto EXIT_CNXTRAWSC_FUNC;
		}
	}

EXIT_CNXTRAWSC_FUNC :
	LEAVE_CRITICAL_EPGRAWDB;
	HxLOG_Print("============================================\n");
#endif
}
#endif //#if defined(CONFIG_DEBUG)


/* end of file */
