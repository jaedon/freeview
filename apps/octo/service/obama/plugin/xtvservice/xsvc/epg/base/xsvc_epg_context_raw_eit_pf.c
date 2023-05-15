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

#include <pal_sef.h>
#include "util.h"
#include "util_task.h"
#include "db_svc.h"
#include <db_param.h>
#include <svc_si.h>
#include "linkedlist.h"
#include <pal_pipe.h>
#include <svc_epg.h>

#include "_svc_epg_common.h"
#include "_svc_epg_common_lib.h"
#include "_svc_epg_db_raw_lib.h"
#include "_svc_epg_manager.h"
#include "_svc_epg_context_mgr.h"
#include "_svc_epg_eit_lib.h"

#include "../local_include/_xsvc_epg.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



#define FUNC_ENTER 		HxLOG_Debug("(+)Enter!!\n")
#define FUNC_LEAVE(x)	HxLOG_Debug("(-)Leave!!(0x%X)\n", x)



// For Debugging function
#define DBG_PF_SETBITMASK(p, pos)			(p[(pos>>3)]  |= (0x01 <<(pos&7)))
#define DBG_PF_CLEARBITMASK(p, pos)			(p[(pos>>3)] &= (~(0x01 <<(pos&7))))
#define DBG_PF_CHECKBITMASK(p, pos)			(p[(pos>>3)] & (0x01 <<(pos&7)))
#define DBG_PF_SEGMENTBITMASK(p, pos)		( p[(pos>>3)] &= (  0xff >> (7 - (pos&7))  )  )


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	EPGCNXT_FITER_SLOT_EIT_PF_AC		= 0,	// Actual PF Filter
	EPGCNXT_FITER_SLOT_EIT_PF_OTH,				// Other PF Filer

	EPGCNXT_FITER_SLOT_MAX
}XsvcEpg_ContextEitPfFilterSlot_e;




/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

#if 0//defined(CONFIG_DEBUG) // 새로 구현 필요.
// For Debugging function
STATIC HUINT8 s_aucPfSecBuildMask[32];
STATIC HUINT8 s_aucPfCmpSecBuildMask[32];
STATIC HUINT8 s_aucPfNonRcvSecBuildMask[32];
STATIC HUINT8 s_ucPfLastSecNum;
#endif


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
STATIC HERROR xsvc_epgcontext_PfGetContextInfo_Base(Handle_t hAction, HUINT32 ulIndex, Handle_t *phSvcHandle, HUINT32 *pulOrgDeliType, HUINT8 *pucAntId, EPG_SpecType_e *peSpec, svcEpg_Filter_t *pEpgContextFilterInfo);
STATIC HERROR xsvc_epgcontext_PfSetContextFilterInfo_Base(Handle_t hActon, HUINT32 ulIndex, svcEpg_Filter_t *pEpgContextFilterInfo);
STATIC HERROR xsvc_epgcontext_PfStartNextFilter_Base(svcEpg_Filter_t *pstFilterInfo, Handle_t hAction, Handle_t hService, HUINT8 ucHead, HUINT32 ulDemuxId, HUINT16 usTsUniqueId);
STATIC HERROR xsvc_epgcontext_PfStopFilter_Base(svcEpg_Filter_t *pstFilterInfo);
STATIC HERROR xsvc_epgcontext_PfStartFilter_Base(	svcEpg_Filter_t *pstFilterInfo,
														Handle_t hAction,
														HUINT8 ucHead,
														HUINT32 ulDemuxId,
														HUINT16 usTsUniqueId,
														HINT32 lFilterState);

#if defined(CONFIG_DEBUG)
STATIC void xsvc_epgcontext_PfDebugStatus_Base(void);
STATIC void xsvc_epgcontext_PfDebugFunction_Base(HUINT32 ulHeadInfo, HUINT32 ulCmdFlag);
#endif

#define ____GLOBAL_FUNC____
/*****************************************************
 * Name : xsvc_epgcontext_PfProcess_Base
 *
 *
 *
 *****************************************************/
HERROR xsvc_epgcontext_PfProcess_Base(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3, HUINT32 ulParam4)
{
	svcEpg_Msg_t			*pstMsg = NULL;
	svcEpg_ContextData_t		*pCnxtData = NULL;
	HUINT32 			ulCnxtCommand = 0;
	Handle_t			hAction = 0;
	Handle_t			hService = HANDLE_NULL;
	HERROR				hError = ERR_EPG_FAIL;
	HUINT32 			ulActionId = 0;
	HUINT32 			ulDemuxId = RxRSCID_NULL;
	HUINT16 			usTsUniqueId = 0;
	HUINT16 			usNewTsUniqueId = 0;
	HUINT8				ucHead = ACT_HEAD;
	DbSvc_Info_t			stSvcInfo;
	DbSvc_TsInfo_t		stTsInfo = {0, };
	EPG_SpecType_e		eSpec;

	HxLOG_Debug("(+)Enter!!\n");

	ulCnxtCommand = ulParam1;
	pstMsg = (svcEpg_Msg_t*)ulParam2;
	pCnxtData = (svcEpg_ContextData_t*)ulParam3;
	if(pCnxtData == NULL)
	{
		HxLOG_Print("pCnxtData null!!\n");
		goto EXIT_CNXTRAWPF_FUNC;
	}
	HAPPY(ulParam4);

	if ( pstMsg == NULL)
	{
		HxLOG_Print("param null!!\n");
		goto EXIT_CNXTRAWPF_FUNC;
	}

	switch(ulCnxtCommand)
	{
		case EPG_CMD_START :
			// step 1. local data setting
			hAction = (Handle_t)pstMsg->ulParam1;
			hService = (Handle_t)pstMsg->ulParam2;

			HxLOG_Info("EPG_CMD_START(hSvc:%x)\n", hService);
			/**********************************************************************************
			  *
			  * get tuning info
			  *
			  **********************************************************************************/
			DB_SVC_GetUniqueIdFromSvcHandle(hService, &usNewTsUniqueId);
			DB_SVC_GetUniqueIdFromSvcHandle(pCnxtData->hService, &usTsUniqueId);
			if(usNewTsUniqueId != usTsUniqueId)
			{
				HxLOG_Info("TP Change!!\n");
				hError = xsvc_epgcontext_StopSchFilter_Base(hAction);
				if(hError != ERR_EPG_OK)
				{
					HxLOG_Error("xsvc_epgcontext_StopSchFilter_Base fail!!\n");
				}
			}

#if	defined(CONFIG_MW_EPG_NEXT_EITFILTER_TIMER)
			if (VK_TIMERID_NULL != pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_AC].ulReStartTimerId)
			{
				VK_TIMER_Cancel(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_AC].ulReStartTimerId);
				pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_AC].ulReStartTimerId = VK_TIMERID_NULL;
			}

			if (VK_TIMERID_NULL != pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_OTH].ulReStartTimerId)
			{
				VK_TIMER_Cancel(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_OTH].ulReStartTimerId);
				pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_OTH].ulReStartTimerId = VK_TIMERID_NULL;
			}
#endif

			if(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_AC].eState != eEPG_CNXT_FILTER_EMPTY)
			{
				hError = xsvc_epgcontext_PfStopFilter_Base( &(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_AC]) );
				if(hError != ERR_EPG_OK)
				{
					HxLOG_Print("xsvc_epgcontext_PfStopFilter_Base fail!!\n");
					//goto EXIT_CNXTRAWPF_FUNC;
				}
				pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_AC].ulRequestId = 0;
				HxSTD_memset(&(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_AC].stBaseInfo), 0x00, sizeof(svcEpg_ServiceInfo_t));
			}
			if(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_OTH].eState != eEPG_CNXT_FILTER_EMPTY)
			{
				hError = xsvc_epgcontext_PfStopFilter_Base( &(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_OTH]) );
				if(hError != ERR_EPG_OK)
				{
					HxLOG_Print("xsvc_epgcontext_PfStopFilter_Base fail!!\n");
					//goto EXIT_CNXTRAWPF_FUNC;
				}
				pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_OTH].ulRequestId = 0;
				HxSTD_memset(&(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_OTH].stBaseInfo), 0x00, sizeof(svcEpg_ServiceInfo_t));
			}
			hError = DB_SVC_GetServiceInfo(hService, &stSvcInfo);
			if(hError != ERR_OK)
			{
				VK_TASK_Sleep(100);
				hError = DB_SVC_GetServiceInfo(hService, &stSvcInfo);
				if(hError != ERR_OK)
				{
					HxLOG_Print("DB_SVC_GetServiceInfo fail!!\n");
					goto EXIT_CNXTRAWPF_FUNC;
				}
			}
			hError = DB_SVC_GetTsInfo(DbSvc_GetTsIdx(&stSvcInfo), &stTsInfo);
			if(hError != ERR_OK)
			{
				HxLOG_Print("DB_SVC_GetTsInfo fail!!\n");
				goto EXIT_CNXTRAWPF_FUNC;
			}

			/**********************************************************************************
			  *
			  * set context
			  *
			  **********************************************************************************/

			pCnxtData->hAction = hAction;
			pCnxtData->hService = hService;
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

			ulActionId = PAL_PIPE_GetActionId(hAction);
			if(ulActionId >= NUM_MAX_ACTION)
			{
				HxLOG_Print("GetActionId fail!!\n");
				goto EXIT_CNXTRAWPF_FUNC;
			}
			pCnxtData->ulActionId = ulActionId;

			hError = PAL_PIPE_GetResourceId(ulActionId, eRxRSCTYPE_DEMUX, &ulDemuxId);
			if(hError != ERR_OK)
			{
				HxLOG_Print("GetResourceId fail!!\n");
				goto EXIT_CNXTRAWPF_FUNC;
			}
			pCnxtData->ulDemuxId = ulDemuxId;
			pCnxtData->eState = eEPG_CNXT_STATUS_RUNNING;

			/**********************************************************************************
			  *
			  * resource init
			  *
			  **********************************************************************************/

			DB_SVC_GetUniqueIdFromSvcHandle(pCnxtData->hService, &usTsUniqueId);
			hError = SVC_EPGSECTION_InitSvcSectionManager(hAction, usTsUniqueId, EIT_ACTUAL_TABLE_ID);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Print("EPGRAWDB_InitSvcSectionManager fail!!\n");
				goto EXIT_CNXTRAWPF_FUNC;
			}

			hError = SVC_EPGSECTION_InitSvcSectionManager(hAction, usTsUniqueId, EIT_OTHER_TABLE_ID);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Print("EPGRAWDB_InitSvcSectionManager fail!!\n");
				goto EXIT_CNXTRAWPF_FUNC;
			}

			hError = xsvc_epgcontext_PfStartFilter_Base(	&(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_AC]),
												hAction,
												ACT_HEAD,
												pCnxtData->ulDemuxId,
												usTsUniqueId,
												eEPG_CNXT_FILTER_CONTINUE);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Print("EPGCNXT_FITER_SLOT_EIT_PF_AC fail!!\n");
				goto EXIT_CNXTRAWPF_FUNC;
			}

			hError = xsvc_epgcontext_PfStartFilter_Base(	&(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_OTH]),
												hAction,
												OTH_HEAD,
												pCnxtData->ulDemuxId,
												usTsUniqueId,
												eEPG_CNXT_FILTER_CONTINUE);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Print("EPGCNXT_FITER_SLOT_EIT_PF_OTH fail!!\n");
				goto EXIT_CNXTRAWPF_FUNC;
			}
			pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_AC].ucDemuxId = (HUINT8)ulDemuxId;
			pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_OTH].ucDemuxId = (HUINT8)ulDemuxId;
			break;

		case EPG_CMD_STOP :
			HxLOG_Info("EPG_CMD_STOP\n");
			hAction = (Handle_t)pstMsg->ulParam1;
			pCnxtData->hAction = HANDLE_NULL;
			pCnxtData->hService = HANDLE_NULL;
#if defined(CONFIG_MW_EPG_NEXT_EITFILTER_TIMER)
			if (VK_TIMERID_NULL != pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_AC].ulReStartTimerId)
			{
				VK_TIMER_Cancel(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_AC].ulReStartTimerId);
				pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_AC].ulReStartTimerId = VK_TIMERID_NULL;
			}

			if (VK_TIMERID_NULL != pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_OTH].ulReStartTimerId)
			{
				VK_TIMER_Cancel(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_OTH].ulReStartTimerId);
				pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_OTH].ulReStartTimerId = VK_TIMERID_NULL;
			}
#endif
			hError = xsvc_epgcontext_PfStopFilter_Base( &(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_AC]) );
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Print("xsvc_epgcontext_PfStopFilter_Base fail!!\n");
				//goto EXIT_CNXTRAWPF_FUNC;
			}
			HxSTD_memset(&(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_AC].stBaseInfo), 0x00, sizeof(svcEpg_ServiceInfo_t));
			pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_AC].ulRequestId = 0;

			hError = xsvc_epgcontext_PfStopFilter_Base( &(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_OTH]) );
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Print("xsvc_epgcontext_PfStopFilter_Base fail!!\n");
				//goto EXIT_CNXTRAWPF_FUNC;
			}
			hError = SVC_EPGSECTION_InitSvcSectionManager(hAction, 0, EIT_ACTUAL_TABLE_ID);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Print("EPGRAWDB_InitSvcSectionManager fail!!\n");
				goto EXIT_CNXTRAWPF_FUNC;
			}

			hError = SVC_EPGSECTION_InitSvcSectionManager(hAction, 0, EIT_OTHER_TABLE_ID);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Print("EPGRAWDB_InitSvcSectionManager fail!!\n");
				goto EXIT_CNXTRAWPF_FUNC;
			}

			HxSTD_memset(&(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_OTH].stBaseInfo), 0x00, sizeof(svcEpg_ServiceInfo_t));
			pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_OTH].ulRequestId = 0;
			break;

		case EPG_CMD_NEXT_FILTER_TIMER :
			{
				HUINT32 ulTimerId;

				hAction = (Handle_t)pstMsg->ulParam1;
				ucHead = (HUINT8)pstMsg->ulParam2;
				ulTimerId = (HUINT32)pstMsg->ulParam3;

#if defined(CONFIG_MW_EPG_NEXT_EITFILTER_TIMER)
				if (ulTimerId != pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_AC + ucHead].ulReStartTimerId)
				{
					HxLOG_Error("InTimerId:0x%X != ExistTimerId:0x%X\n",
									ulTimerId, pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_AC + ucHead].ulReStartTimerId);
					pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_AC + ucHead].ulReStartTimerId = VK_TIMERID_NULL;
					break;
				}
				pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_AC + ucHead].ulReStartTimerId = VK_TIMERID_NULL;
#endif
				(void)ulTimerId;
				HxLOG_Info("EPG_CMD_NEXT_FILTER_TIMER (%d)\n", ucHead);
				DB_SVC_GetUniqueIdFromSvcHandle(pCnxtData->hService, &usTsUniqueId);
				hError = SVC_EPGSECTION_InitSvcSectionManager(hAction, usTsUniqueId, EIT_ACTUAL_TABLE_ID + ucHead);
				if(hError != ERR_EPG_OK)
				{
					HxLOG_Print("EPGRAWDB_InitSvcSectionManager fail!!\n");
					goto EXIT_CNXTRAWPF_FUNC;
				}
				hError = xsvc_epgcontext_PfStartFilter_Base(	&(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_AC + ucHead]),
													hAction,
													ucHead,
													pCnxtData->ulDemuxId,
													usTsUniqueId,
													eEPG_CNXT_FILTER_CONTINUE);
				if(hError != ERR_EPG_OK)
				{
					HxLOG_Print("EPG_CMD_NEXT_FILTER_TIMER fail!!\n");
					goto EXIT_CNXTRAWPF_FUNC;
				}
			}
			break;

		case EPG_CMD_STOP_FILTER :
			hAction = (Handle_t)pstMsg->ulParam1;
			ucHead = (HUINT8)pstMsg->ulParam2;

			HxLOG_Info("EPG_CMD_STOP_FILTER(%d)\n", ucHead);
			DB_SVC_GetUniqueIdFromSvcHandle(pCnxtData->hService, &usTsUniqueId);
			hError = xsvc_epgcontext_PfStopFilter_Base( &(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_AC + ucHead]) );
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Print("local_cnxtsch_StopFilter(%d) fail!!\n", ucHead);
				goto EXIT_CNXTRAWPF_FUNC;
			}

#if defined(CONFIG_MW_EPG_NEXT_EITFILTER_TIMER)
		{
			HUINT32 ulTimerId;
			HUINT16	usTableId;

			if (VK_TIMERID_NULL != pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_AC + ucHead].ulReStartTimerId)
			{
				VK_TIMER_Cancel(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_AC + ucHead].ulReStartTimerId);
				pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_AC + ucHead].ulReStartTimerId = VK_TIMERID_NULL;
			}

			usTableId = EIT_ACTUAL_TABLE_ID;
			if(ucHead == OTH_HEAD)
			{
				usTableId = EIT_OTHER_TABLE_ID;
			}
			hError = SVC_EPGCONTEXT_SetTimerOnce(usTableId, pCnxtData->eSpec, &ulTimerId);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Print("SVC_EPGCONTEXT_SetTimerOnce fail!!\n");
				goto EXIT_CNXTRAWPF_FUNC;
			}
			pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_AC + ucHead].ulReStartTimerId = ulTimerId;
		}
#else
			HxLOG_Info("xsvc_epgcontext_PfStartNextFilter_Base\n");
			hError = xsvc_epgcontext_PfStartNextFilter_Base(	&(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_AC + ucHead]),
												hAction,
												pCnxtData->hService,
												ucHead,
												pCnxtData->ulDemuxId,
												usTsUniqueId);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Print("xsvc_epgcontext_PfStartNextFilter_Base fail!!\n");
				goto EXIT_CNXTRAWPF_FUNC;
			}
#endif
			break;


		case EPG_CMD_CONTEXT :
			HxLOG_Print("EPG_CMD_CONTEXT\n");
			break;

#if defined(CONFIG_DEBUG)
		case EPG_CMD_PRINT:
			xsvc_epgcontext_PfDebugFunction_Base(pstMsg->ulParam2,0);
			break;
		case EPG_CMD_CHECK_DONE:
			xsvc_epgcontext_PfDebugFunction_Base(pstMsg->ulParam2,1);
			break;
		case EPG_CMD_CHECK_STATUS:
			xsvc_epgcontext_PfDebugStatus_Base();
			break;
#endif
		default :
			break;


	}

	VK_TASK_Sleep(100);

	hError = ERR_EPG_OK;

EXIT_CNXTRAWPF_FUNC :
	HxLOG_Debug("(-)Enter!! (hError:0x%X)\n",hError);

	return hError;

}




/*****************************************************
 * Name : xsvc_epgcontext_PfProcessEitCallBack_Base
 * Param :
 *             ulParam1 :  PalSef_ReceiveMessage_t
 *             ulParam2 :  ???
 *
 *****************************************************/
HERROR xsvc_epgcontext_PfProcessEitCallBack_Base(PalSef_ReceiveMessage_t *pstMsg, HUINT32 ulParam)
{
	HERROR						hError = ERR_EPG_FAIL;
	DbSvc_TripleId_t				stTripleId;
	svcEpg_ServiceInfo_t			stServiceInfo;
	HBOOL						bFirstSection = FALSE;
	EPG_SpecType_e				eSpec = eEPG_SPEC_BASE;
	Handle_t						hAction = 0;
	Handle_t						hSvcHandle = HANDLE_NULL;
	svcEpg_Filter_t 				stCnxtFilterInfo;
	HUINT32						ulCnxtFilterSlot = 0;
	HUINT32						ulOrgDeliType = 0;
	HUINT8						ucAntId = 0;
	HUINT8						*pucSection = NULL;
	HUINT8						ucHead = ACT_HEAD;
	HBOOL						bSectionConflict = FALSE;
	HBOOL						bCompleteCycle = FALSE;
	HBOOL						bCompleteSection = FALSE;
	HBOOL						bVersionChange = FALSE;
	HBOOL 						pbIsFirstTime = FALSE;
	HUINT16						usTableId = 0;
	UNIXTIME					ulSystemTime = 0;
	HUINT16						usTsUniqueId = 0;
	POINTER_LIST_T				*pSectionList = NULL;
	HUINT32						ulMessage = 0;
	HUINT32						ulSemId;

	SVC_EPGCONTEXT_GetSemId(&ulSemId);
	VK_SEM_Get(ulSemId);

	HxLOG_Debug("(+)Enter!!\n");

	if(pstMsg == NULL)
	{
		HxLOG_Print("param fail!!\n");
		goto  EXIT_CNXTRAWPF_FUNC;
	}

	if (pstMsg->eSecPackageFlag == eSectionPackage_On)
	{
		HUINT32				 ulCnt;
		PalSef_RawData_t	*pSiRawData = (PalSef_RawData_t*)pstMsg->ulDataAddr;

		HxLOG_Error("eSecPackageFlag == eSectionPackage_On hAction[%x]!!\n", hAction);

		if (NULL != pSiRawData)
		{
			for (ulCnt = 0; ulCnt < pSiRawData->num_sec; ulCnt++)
			{
				if (NULL != pSiRawData->buf[ulCnt])
				{
					OxSI_Free(pSiRawData->buf[ulCnt]);
					pSiRawData->buf[ulCnt] = NULL;
				}
			}

			OxMW_Free(pSiRawData);
		}

		pucSection = NULL;
		goto  EXIT_CNXTRAWPF_FUNC;
	}
	
	pucSection = (HUINT8*)pstMsg->ulDataAddr;
	if(pucSection == NULL)
	{
		HxLOG_Print("OxMW_Malloc null!!\n");
		goto  EXIT_CNXTRAWPF_FUNC;
	}

	HxLOG_Debug("err:0x%X SecPackageFlag:0x%X ucDemuxId:%d TableId:0x%X ReqId:0x%X DataAddr:0x%X\n",
		pstMsg->err,pstMsg->eSecPackageFlag,pstMsg->ucDemuxId,
		pstMsg->ucTableId,pstMsg->ulRequestId,pstMsg->ulDataAddr);

	/**********************************************************************************
	  *
	  * 1. get filtering result info
	  *
	  **********************************************************************************/

	usTableId = pstMsg->ucTableId;
	if(usTableId ==EIT_ACTUAL_TABLE_ID)
	{
		ucHead = ACT_HEAD;
		ulCnxtFilterSlot = EPGCNXT_FITER_SLOT_EIT_PF_AC;
	}
	else if(usTableId ==EIT_OTHER_TABLE_ID)
	{
		ucHead = OTH_HEAD;
		ulCnxtFilterSlot = EPGCNXT_FITER_SLOT_EIT_PF_OTH;
	}

	hAction = (Handle_t)pstMsg->ulUserData;

	// find hAction
	hError = xsvc_epgcontext_PfGetContextInfo_Base(hAction, ulCnxtFilterSlot, &hSvcHandle, &ulOrgDeliType, &ucAntId, &eSpec, &stCnxtFilterInfo);

	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("SendMessage fail!! hAction[%x]\n", hAction);
		goto  EXIT_CNXTRAWPF_FUNC;
	}
	if(stCnxtFilterInfo.bTimeOut == TRUE)
	{
		HxLOG_Print("SendMessage fail!! hAction[%x]\n", hAction);
		goto  EXIT_CNXTRAWPF_FUNC;
	}

	if(stCnxtFilterInfo.bComplete)
	{
		HxLOG_Print("bComplete!! hAction[%x]\n", hAction);
		goto EXIT_CNXTRAWPF_FUNC;
	}
	if(pstMsg->ulRequestId != stCnxtFilterInfo.ulRequestId)
	{
		HxLOG_Debug("pstMsg->ulRequestId != stCnxtFilterInfo.ulRequestId!!\n");
		goto  EXIT_CNXTRAWPF_FUNC;
	}

	DB_SVC_GetUniqueIdFromSvcHandle(hSvcHandle, &usTsUniqueId);

	/**********************************************************************************
	  *
	  * 2. send message
	  *
	  **********************************************************************************/
	HxSTD_memset(&stServiceInfo, 0x00, sizeof(svcEpg_ServiceInfo_t));
	hError = SVC_EPGEIT_GetSectionInfo(eSpec, pucSection, &stServiceInfo);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("SVC_EPGEIT_GetSectionInfo fail!! hAction[%x]\n", hAction);
		goto  EXIT_CNXTRAWPF_FUNC;
	}

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
		if (stCnxtFilterInfo.eState == eEPG_CNXT_FILTER_CONTINUE &&bVersionChange == TRUE)
		{
			HxSTD_memcpy(&(stCnxtFilterInfo.stBaseInfo), &stServiceInfo, sizeof(svcEpg_ServiceInfo_t));
			stCnxtFilterInfo.ulBaseTime = ulSystemTime;
			stCnxtFilterInfo.ucCycle++;

			(void)xsvc_epgcontext_PfSetContextFilterInfo_Base(hAction, ulCnxtFilterSlot, &stCnxtFilterInfo);

			bFirstSection = TRUE;
			HxLOG_Info("start(%d) hAction[%x]\n", ulSystemTime, hAction);
		}
		if(hError != ERR_EPG_OK || bSectionConflict == TRUE)
		{
			HxLOG_Debug("EPGRAWDB_CheckSvcSection invalid section hAction[%x]\n", hAction);
			goto EXIT_CNXTRAWPF_FUNC;
		}

#if !defined(CONFIG_OP_UK_DTT) /* Will be removed after code review */
		if(bVersionChange == TRUE && pbIsFirstTime == FALSE)
		{
			svc_epg_DeleteFromDama (&stServiceInfo, ulOrgDeliType, ucAntId);
		}
#endif
	}

	if(bCompleteCycle == TRUE	// turn around one cycle in continue state
		|| (stCnxtFilterInfo.eState == eEPG_CNXT_FILTER_MONITOR && bVersionChange == TRUE)
		)
	{
		stCnxtFilterInfo.ucCycle++;
		if(bCompleteCycle == TRUE) // Check Real completion.
		{
			hError = SVC_EPGSECTION_CheckSectionComplete(hAction, (HUINT8)usTableId, &bCompleteSection);
			if(hError != ERR_EPG_OK || bCompleteSection == FALSE)
			{
				HxLOG_Print("ucHead(%d) EPGRAWDB_CheckSvcSection not complete hAction[%x]!!\n", ucHead, hAction);
				goto  ENTER_POST_SECTION;
			}
		}

		OxSI_Free(pucSection);
		pucSection = NULL;

		HxLOG_Print("%s-PF complete(%d) Send EPG_CMD_STOP_FILTER Filter(%d) bcomp(%d) version(%d)\n",	((ACT_HEAD == ucHead) ? "ACT_HEAD":"OTH_HEAD"),(HINT32)VK_TIMER_GetSystemTick()/1000,
			stCnxtFilterInfo.eState, bCompleteCycle, bVersionChange);

		// cycle notify: context에서 처리
		// if completetion, Stop the filter, And Set filter as a monitor.
		// if Not Completetion and monitor filter, Change the filter as a continue.
		hError = SVC_EPG_MakeEpgMessage(EPG_CMD_STOP_FILTER,  eEPG_PROCESS_EIT_PF, &ulMessage);
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Print("MW_EPGMAIN_MakeEpgMessage fail!!hAction[%x]\n", hAction);
		}
		hError = SVC_EPG_SendMessage(ulMessage, hAction, ucHead, 0);
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Print("SendMessage fail!!hAction[%x]\n", hAction);
		}

/* DTG DTR의 경우 일부 Stream에서 Schedule Table이 Complete가 되지 않기 때문에 EPG에 모든 Cell이 나타나기 전에 예약을 할 경우
Series Update를 하지 못하는 문제가 발생 될 수 있다... 물론 DTG Test시 EIT 수신을 하여 EPG가 보이고 Reservation을 하도록 하게 되어 있지만...
Live에서는 너무 잦은 Series Set Update검사를 수행하게 되어 성능에 영향을 줄 수 있다. (Service 개수도 많고, Raw data도 양이 많아 연산하는데
성능을 많이 빼앗긴다) */
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME) && 0
		hError = SVC_EPG_MgrPostMessage(eSEVT_EPG_FPB_UPDATE_SCHEDULE_SERIES_SET, 0, ACT_PF_HEAD, 0, 0);
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Error("SVC_EPG_MgrPostMessage fail!!hAction[%x]\n", hAction);
		}
#endif

		stCnxtFilterInfo.bComplete = TRUE;
		hError = xsvc_epgcontext_PfSetContextFilterInfo_Base(hAction, ulCnxtFilterSlot, &stCnxtFilterInfo);
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Print("xsvc_epgcontext_PfSetContextFilterInfo_Base fail!!hAction[%x]\n", hAction);
			goto  EXIT_CNXTRAWPF_FUNC;
		}

		hError = ERR_EPG_OK;
		HxLOG_Print("End Filter(%04d)\n",(HINT32)VK_TIMER_GetSystemTick()/1000);
		goto  EXIT_CNXTRAWPF_FUNC;
	}
	else
	{
		ulSystemTime = VK_TIMER_GetSystemTick()/1000;
		if(stCnxtFilterInfo.stBaseInfo.ulCrc == 0)
		{
			HxSTD_memcpy(&(stCnxtFilterInfo.stBaseInfo), &stServiceInfo, sizeof(svcEpg_ServiceInfo_t));
			stCnxtFilterInfo.ulBaseTime = ulSystemTime;
			stCnxtFilterInfo.ucCycle++;

			hError = xsvc_epgcontext_PfSetContextFilterInfo_Base(hAction, ulCnxtFilterSlot, &stCnxtFilterInfo);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Print("xsvc_epgcontext_PfSetContextFilterInfo_Base fail!!hAction[%x]\n", hAction);
				goto  EXIT_CNXTRAWPF_FUNC;
			}
			bFirstSection = TRUE;
			HxLOG_Info("start(%d) hAction[%x]\n", ulSystemTime, hAction);
		}
	}

	(void)bFirstSection;
ENTER_POST_SECTION :

/**********************************************************************************
  *
  * 1. check valid
  *
  **********************************************************************************/
	HxSTD_memset(&stTripleId, 0x00, sizeof(DbSvc_TripleId_t));
	stTripleId.usOnId = stServiceInfo.usOnId;
	stTripleId.usTsId = stServiceInfo.usTsId;
	stTripleId.usSvcId = stServiceInfo.usSvcId;
	hError = SVC_EPGSERVICE_ValidSvcCheckServiceValidation(ulOrgDeliType, ucAntId, &stTripleId);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Debug("EPGRAWDB_CheckServiceValidation fail!!hAction[%x]\n", hAction);
		goto  EXIT_CNXTRAWPF_FUNC;
	}

/**********************************************************************************
  *
  * 2. Delete old pf data
  *
  **********************************************************************************/
#if defined(CONFIG_OP_UK_DTT) /* This code is required code review */
	if(bVersionChange || pbIsFirstTime)
	{
		svc_epg_DeleteFromDama(&stServiceInfo, ulOrgDeliType, ucAntId);
	}
#endif

/**********************************************************************************
  *
  * 3. register db
  *
  **********************************************************************************/
	pSectionList = NULL;
	pSectionList = UTIL_LINKEDLIST_AppendListItemToPointerList(pSectionList, pucSection);

	hError = SVC_EPGLIB_AddSectionList(	hAction,
										eSpec,
										ulOrgDeliType,
										ucAntId,
										usTableId,
										pSectionList);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Debug("SVC_EPGDB_AddSectionList fail!!hAction[%x]\n", hAction);
		goto  EXIT_CNXTRAWPF_FUNC;
	}
	else
	{
		pucSection = NULL;
	}

	hError = ERR_EPG_OK;

EXIT_CNXTRAWPF_FUNC :
	if(pucSection != NULL)
	{
		OxSI_Free(pucSection);
		pucSection = NULL;
	}
	HxLOG_Debug("(-)Enter!! (hError:0x%X) hAction[%x]\n",hError, hAction);

	VK_SEM_Release(ulSemId);

	return hError;
}




#define ____LOCAL_FUNC____

/*****************************************************
 * Name : xsvc_epgcontext_PfStartNextFilter_Base
 *
 *
 *
 *****************************************************/
STATIC HERROR xsvc_epgcontext_PfStartNextFilter_Base(svcEpg_Filter_t *pstFilterInfo, Handle_t hAction, Handle_t hService, HUINT8 ucHead, HUINT32 ulDemuxId, HUINT16 usTsUniqueId)
{
	HERROR				hError = ERR_EPG_FAIL;

	HxLOG_Debug("(+)Enter!!\n");

	if(pstFilterInfo == NULL)
	{
		HxLOG_Print("param fail!!\n");
		goto EXIT_CNXTRAWPF_FUNC;
	}
	HxLOG_Print("\tpstFilterInfo->eState:0x%X\n",pstFilterInfo->eState);

	switch(pstFilterInfo->eState)
	{
		case eEPG_CNXT_FILTER_CONTINUE :
		case eEPG_CNXT_FILTER_NEXTCONTINUE :
			HxSTD_memcpy(&(pstFilterInfo->stLastCompleteInfo), &(pstFilterInfo->stBaseInfo),  sizeof(svcEpg_ServiceInfo_t));

			if(pstFilterInfo->stLastCompleteInfo.ucTableId == 0 || pstFilterInfo->bTimeOut == TRUE)
			{
				hError = xsvc_epgcontext_PfStartFilter_Base(	pstFilterInfo,
													hAction,
													ucHead,
													ulDemuxId,
													usTsUniqueId,
													eEPG_CNXT_FILTER_CONTINUE);
				if(hError != ERR_EPG_OK)
				{
					HxLOG_Print("xsvc_epgcontext_PfStartFilter_Base fail!!\n");
					goto EXIT_CNXTRAWPF_FUNC;
				}
				HxLOG_Info("ucHead(%d) filter eEPG_CNXT_FILTER_CONTINUE \n\n\n", ucHead);
			}
			else
			{
				hError = xsvc_epgcontext_PfStartFilter_Base(	pstFilterInfo,
													hAction,
													ucHead,
													ulDemuxId,
													usTsUniqueId,
													eEPG_CNXT_FILTER_MONITOR);
				if(hError != ERR_EPG_OK)
				{
					HxLOG_Print("xsvc_epgcontext_PfStartFilter_Base fail!!\n");
					goto EXIT_CNXTRAWPF_FUNC;
				}
				HxLOG_Info("ucHead(%d) complete eEPG_CNXT_FILTER_MONITOR \n\n\n", ucHead);
			}
			break;

		case eEPG_CNXT_FILTER_MONITOR :
			if(pstFilterInfo->bTimeOut == TRUE)
			{
				hError = xsvc_epgcontext_PfStartFilter_Base(	pstFilterInfo,
													hAction,
													ucHead,
													ulDemuxId,
													usTsUniqueId,
													eEPG_CNXT_FILTER_MONITOR);
				if(hError != ERR_EPG_OK)
				{
					HxLOG_Print("xsvc_epgcontext_PfStartFilter_Base fail!!\n");
					goto EXIT_CNXTRAWPF_FUNC;
				}
				HxLOG_Info("ucHead(%d) timeout!! eEPG_CNXT_FILTER_MONITOR \n\n\n", ucHead);
			}
			else
			{
				hError = SVC_EPGSECTION_InitSvcSectionManager(hAction, usTsUniqueId, pstFilterInfo->stLastCompleteInfo.ucTableId);
				if(hError != ERR_EPG_OK)
				{
					HxLOG_Print("EPGRAWDB_InitSvcSectionManager fail!!\n");
					goto EXIT_CNXTRAWPF_FUNC;
				}
				hError = xsvc_epgcontext_PfStartFilter_Base(	pstFilterInfo,
													hAction,
													ucHead,
													ulDemuxId,
													usTsUniqueId,
													eEPG_CNXT_FILTER_CONTINUE);
				if(hError != ERR_EPG_OK)
				{
					HxLOG_Print("xsvc_epgcontext_PfStartFilter_Base fail!!\n");
					goto EXIT_CNXTRAWPF_FUNC;
				}
				HxLOG_Info("ucHead(%d) version change!! eEPG_CNXT_FILTER_CONTINUE \n\n\n", ucHead);
			}
			break;
	}

	hError = ERR_EPG_OK;

EXIT_CNXTRAWPF_FUNC :
	HxLOG_Debug("(-)Leave!! (hError:0x%X)\n",hError);

	return hError;
}

/*****************************************************
 * Name : xsvc_epgcontext_PfStopFilter_Base
 *
 *
 *
 *****************************************************/
STATIC HERROR xsvc_epgcontext_PfStopFilter_Base(svcEpg_Filter_t *pstFilterInfo)
{
	HERROR				hError = ERR_EPG_FAIL;

	HxLOG_Debug("(+)Enter!!\n");

	if(pstFilterInfo == NULL)
	{
		HxLOG_Print("param fail!!\n");
		goto EXIT_CNXTRAWPF_FUNC;
	}

	if(pstFilterInfo->ulRequestId == 0)
	{
		HxLOG_Print("invalid ulRequestId id!!\n");
		hError = ERR_OK;
		goto EXIT_CNXTRAWPF_FUNC;
	}

	HxLOG_Print("\n\t Stop EIT Raw Filter\n\tDemuxId:%d ReqId:0x%X\n\n",
										pstFilterInfo->ucDemuxId,pstFilterInfo->ulRequestId);

	hError = PAL_SEF_DisableSectionFilter(pstFilterInfo->ucDemuxId, pstFilterInfo->ulRequestId);
	if(hError != ERR_OK)
	{
		HxLOG_Error("PAL_SEF_DisableSectionFilter fail!!\n");
		goto EXIT_CNXTRAWPF_FUNC;
	}
	pstFilterInfo->ulRequestId = 0;

	hError = ERR_EPG_OK;

EXIT_CNXTRAWPF_FUNC :
	HxLOG_Debug("(-)Leave!! (hError:0x%X)\n",hError);

	return hError;
}

/*****************************************************
 * Name : xsvc_epgcontext_PfStartFilter_Base
 *
 *
 *
 *****************************************************/
STATIC HERROR xsvc_epgcontext_PfStartFilter_Base( svcEpg_Filter_t *pstFilterInfo,
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

	HxLOG_Debug("(+)Enter!!\n");

	if(pstFilterInfo == NULL)
	{
		HxLOG_Print("param fail!!\n");
		goto EXIT_CNXTRAWPF_FUNC;
	}

	usTableId = EIT_ACTUAL_TABLE_ID;
	if(ucHead == OTH_HEAD)
	{
		usTableId = EIT_OTHER_TABLE_ID;
	}

	HxLOG_Print("hAction:0x%X,ucHead:0x%X,ulDemuxId:0x%X,usTsUniqueId:0x%X,lFilterState:0x%X\n",
					hAction,ucHead,ulDemuxId,usTsUniqueId,lFilterState);

	SVC_EPGCONTEXT_GetMsgQId(&ulMsgQId);
	SVC_EPGCONTEXT_GetEmergencyMsgQId(&ulEmergencyQId);

	HxSTD_memset(&(pstFilterInfo->stBaseInfo), 0x00, sizeof(svcEpg_ServiceInfo_t));

	aucFilterValue[0] = (HUINT8)usTableId;
	aucFilterMask[0]  = 0xff;

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
										0,
										0,
										eSefFilterLen_4Bytes,
										aucFilterValue,
										aucFilterMask,
										aucFilterMode,
										hAction,
										&ulRequestId,
										TRUE);
	if(hError != ERR_OK)
	{
		HxLOG_Print("PAL_SEF_EnableSectionFilter fail!!\n");
		goto EXIT_CNXTRAWPF_FUNC;
	}

	HxLOG_Print("\n\t Start EIT Raw Filter\n\tDemuxId:%d FilterMode:0x%X ReqId:0x%X\n\t\tFilterValue:[0x%02X][0x%02X][0x%02X][0x%02X][0x%02X]\n\t\tMaskValue:[0x%02X][0x%02X][0x%02X][0x%02X][0x%02X]\n\n",
			ulDemuxId,aucFilterMode[0],ulRequestId,aucFilterValue[0],aucFilterValue[1],aucFilterValue[2],aucFilterValue[3],aucFilterValue[4],
			aucFilterMask[0],aucFilterMask[1],aucFilterMask[2],aucFilterMask[3],aucFilterMask[4]);

	pstFilterInfo->eState = lFilterState;
	pstFilterInfo->usPid = EIT_PID;
	pstFilterInfo->ulTable = EIT_ACTUAL_TABLE_ID;
	if(ucHead == OTH_HEAD)
	{
		pstFilterInfo->ulTable = EIT_OTHER_TABLE_ID;
	}
	pstFilterInfo->ulRequestId = ulRequestId;
	pstFilterInfo->ulBufferSize = 0;
	pstFilterInfo->ucCycle = 0;
	pstFilterInfo->bComplete= FALSE;
	pstFilterInfo->bTimeOut = FALSE;

	hError = ERR_EPG_OK;

	(void)eFilterLen;
EXIT_CNXTRAWPF_FUNC :
	HxLOG_Debug("(-)Leave!! (hError:0x%X)\n",hError);

	return hError;
}

/*****************************************************
 * Name : xsvc_epgcontext_PfGetContextInfo_Base
 *
 *
 *
 *****************************************************/
STATIC HERROR xsvc_epgcontext_PfGetContextInfo_Base(Handle_t hAction, HUINT32 ulIndex, Handle_t *phSvcHandle, HUINT32 *pulOrgDeliType, HUINT8 *pucAntId, EPG_SpecType_e *peSpec, svcEpg_Filter_t *pEpgContextFilterInfo)
{
	HERROR				hError = ERR_EPG_FAIL;
	svcEpg_Context_t 		*pContext = NULL;
	svcEpg_ContextData_t		*pCnxtData = NULL;

	HxLOG_Debug("(+)Enter!!\n");
//	SVC_EPGCONTEXT_GetSemId(&ulSemId);

	if(phSvcHandle == NULL || pEpgContextFilterInfo == NULL)
	{
		HxLOG_Print("param fail!!\n");
		goto EXIT_CNXTRAWPF_FUNC_NO_SEM_REL;
	}

	hError = SVC_EPGCONTEXT_GetContext(hAction, eEPG_PROCESS_EIT_PF, &pContext);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("SVC_EPGCONTEXT_GetContext fail!!\n");
		goto EXIT_CNXTRAWPF_FUNC_NO_SEM_REL;
	}
	if(pContext->eStatus == eEPG_CNXT_EMPTY)
	{
		HxLOG_Print("SVC_EPGCONTEXT_GetContext fail!!\n");
		goto EXIT_CNXTRAWPF_FUNC_NO_SEM_REL;
	}

//	VK_SEM_Get(ulSemId);
	if(pContext->eStatus != eEPG_CNXT_USED || pContext->pCnxtData == NULL)
	{
		HxLOG_Print("context fail!!\n");
		hError = ERR_EPG_FAIL;
		goto EXIT_CNXTRAWPF_FUNC;
	}

	pCnxtData = pContext->pCnxtData;

	*phSvcHandle = pCnxtData->hService;
	*pulOrgDeliType = pCnxtData->ulOrgDeliveryType;
	*peSpec = pCnxtData->eSpec;
	*pucAntId = pCnxtData->ucAntId;
	HxSTD_memcpy(pEpgContextFilterInfo, &(pCnxtData->astFilterTable[ulIndex]), sizeof(svcEpg_Filter_t));

	hError = ERR_EPG_OK;

EXIT_CNXTRAWPF_FUNC :
//	VK_SEM_Release(ulSemId);
EXIT_CNXTRAWPF_FUNC_NO_SEM_REL:

	HxLOG_Debug("(-)Enter!! (hError:0x%X)\n",hError);

	return hError;
}

/*****************************************************
 * Name : xsvc_epgcontext_PfSetContextFilterInfo_Base
 *
 *
 *
 *****************************************************/
STATIC HERROR xsvc_epgcontext_PfSetContextFilterInfo_Base(Handle_t hAction, HUINT32 ulIndex, svcEpg_Filter_t *pEpgContextFilterInfo)
{
	HERROR				hError = ERR_EPG_FAIL;
	svcEpg_Context_t 		*pContext = NULL;
	svcEpg_ContextData_t		*pCnxtData = NULL;

	HxLOG_Debug("(+)Enter!!\n");
//	SVC_EPGCONTEXT_GetSemId(&ulSemId);

	if(pEpgContextFilterInfo == NULL)
	{
		HxLOG_Print("param fail!!\n");
		goto EXIT_CNXTRAWPF_FUNC_NO_SEM_REL;
	}

	hError = SVC_EPGCONTEXT_GetContext(hAction, eEPG_PROCESS_EIT_PF, &pContext);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("SVC_EPGCONTEXT_GetContext fail!!\n");
		goto EXIT_CNXTRAWPF_FUNC_NO_SEM_REL;
	}
	if(pContext->eStatus == eEPG_CNXT_EMPTY)
	{
		HxLOG_Print("SVC_EPGCONTEXT_GetContext fail!!\n");
		goto EXIT_CNXTRAWPF_FUNC_NO_SEM_REL;
	}
//	VK_SEM_Get(ulSemId);
	if(pContext->eStatus != eEPG_CNXT_USED || pContext->pCnxtData == NULL)
	{
		HxLOG_Print("context fail!!\n");
		hError = ERR_EPG_FAIL;
		goto EXIT_CNXTRAWPF_FUNC;
	}

	pCnxtData = pContext->pCnxtData;
	if(pCnxtData->astFilterTable[ulIndex].ulRequestId != pEpgContextFilterInfo->ulRequestId)
	{
		HxLOG_Print("context fail!!\n");
		hError = ERR_EPG_FAIL;
		goto EXIT_CNXTRAWPF_FUNC;
	}
	HxSTD_memcpy(&(pCnxtData->astFilterTable[ulIndex]), pEpgContextFilterInfo, sizeof(svcEpg_Filter_t));

	hError = ERR_EPG_OK;

EXIT_CNXTRAWPF_FUNC:
//	VK_SEM_Release(ulSemId);
EXIT_CNXTRAWPF_FUNC_NO_SEM_REL:

	HxLOG_Debug("(-)Enter!! (hError:0x%X)\n",hError);

	return hError;
}

#if defined(CONFIG_DEBUG)
/*****************************************************
 * Name : xsvc_epgcontext_PfDebugStatus_Base
 *
 *
 *
 *****************************************************/
extern	void SVC_EPGRAWSECTION_PrintContent(HUINT32 ulType);

extern HERROR  MGR_LIVE_GetServiceHandle(HUINT32 ulActionId, HINT32 eSvcType, Handle_t *phSvc);
extern HUINT32 MGR_ACTION_GetMainActionId(void);

STATIC void xsvc_epgcontext_PfDebugStatus_Base(void)
{
	Handle_t			hAction = 0;
	Handle_t			hSvcHandle;
	HUINT32				ulOrgDeliType;
	EPG_SpecType_e		eSpec;
	svcEpg_Filter_t		stEitInfo;
	HERROR				hError;
	DbSvc_Info_t			stSvcInfo;
	svcEpg_Context_t 		*pContext = NULL;
	svcEpg_ContextData_t		*pCnxtData = NULL;

	hError = SVC_EPGCONTEXT_GetContext(hAction, eEPG_PROCESS_EIT_PF, &pContext);
	if(hError != ERR_EPG_OK)
	{
		HLIB_CMD_Printf("[%s] xsvc_epgcontext_SchGetContextInfo_Base return : 0x%X\n",__FUNCTION__,hError);
		return;
	}

	pCnxtData = pContext->pCnxtData;
	ulOrgDeliType = pCnxtData->ulOrgDeliveryType;
	hSvcHandle = pCnxtData->hService;
	eSpec= pCnxtData->eSpec;
	HxSTD_memcpy(&stEitInfo, &(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_AC]), sizeof(svcEpg_Filter_t));

	HLIB_CMD_Printf("\n== RAWEPG ACT SCHEDULE\n");
	HLIB_CMD_Printf("\thSvc:0x%X OrgDelType:0x%X eSpec:0x%X\n",hSvcHandle,ulOrgDeliType,eSpec);
	hError = DB_SVC_GetServiceInfo (hSvcHandle, &stSvcInfo);
	if (ERR_OK == hError)
	{
		HxLOG_Print("\t[%04d] - SvcId:0x%X - %s\n", DbSvc_GetLcn(&stSvcInfo), DbSvc_GetSvcId(&stSvcInfo), DbSvc_GetName(&stSvcInfo));
	}
#if defined(CONFIG_MW_EPG_NEXT_EITFILTER_TIMER)
	HLIB_CMD_Printf("\tState:0x%X Tbl:0x%X bComplete:%d ReStartTimeId:0x%X DmxId:%d FilterReqId:0x%X\n",
		stEitInfo.eState,stEitInfo.ulTable,stEitInfo.bComplete,stEitInfo.ulReStartTimerId,stEitInfo.ucDemuxId,stEitInfo.ulRequestId);
#else
	HLIB_CMD_Printf("\tState:0x%X Tbl:0x%X bComplete:%d DmxId:%d FilterReqId:0x%X\n",
		stEitInfo.eState,stEitInfo.ulTable,stEitInfo.bComplete,stEitInfo.ucDemuxId,stEitInfo.ulRequestId);
#endif
	HxSTD_memcpy(&stEitInfo, &(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_EIT_PF_OTH]), sizeof(svcEpg_Filter_t));

	HLIB_CMD_Printf("\n== RAWEPG OTH SCHEDULE\n");
	HLIB_CMD_Printf("\thSvc:0x%X OrgDelType:0x%X eSpec:0x%X\n",hSvcHandle,ulOrgDeliType,eSpec);
//	hError = DB_SVC_GetServiceInfo (hSvcHandle, &stSvcInfo);
//	if (ERR_OK == hError)
//	{
		HLIB_CMD_Printf("\t[%04d] - SvcId:0x%X - %s\n", DbSvc_GetLcn(&stSvcInfo), DbSvc_GetSvcId(&stSvcInfo), DbSvc_GetName(&stSvcInfo));
//	}
#if defined(CONFIG_MW_EPG_NEXT_EITFILTER_TIMER)
	HLIB_CMD_Printf("\tState:0x%X Tbl:0x%X bComplete:%d ReStartTimeId:0x%X DmxId:%d FilterReqId:0x%X\n",
		stEitInfo.eState,stEitInfo.ulTable,stEitInfo.bComplete,stEitInfo.ulReStartTimerId,stEitInfo.ucDemuxId,stEitInfo.ulRequestId);
#else
	HLIB_CMD_Printf("\tState:0x%X Tbl:0x%X bComplete:%d DmxId:%d FilterReqId:0x%X\n",
		stEitInfo.eState,stEitInfo.ulTable,stEitInfo.bComplete,stEitInfo.ucDemuxId,stEitInfo.ulRequestId);
#endif

	SVC_EPGRAWSECTION_PrintContent(0);
}

/*****************************************************
 * Name : xsvc_epgcontext_PfDebugFunction_Base
 *
 *
 *
 *****************************************************/
STATIC void xsvc_epgcontext_PfDebugFunction_Base(HUINT32 ulHeadInfo, HUINT32 ulCmdFlag)
{
	// TODO: 새로 구현 필요..
#if 0 // 새로 구현하세요..
	HERROR						hError = ERR_EPG_FAIL;
	HTREEMGR					hTree = NULL;
	HTREEITEM					hDeliItem = NULL;
	HTREEITEM					hTsItem = NULL;
	HTREEITEM					hTsFirstChild = NULL,hSvcFirstChild = NULL;
	HTREEITEM					hTsTempItem = NULL,hSvcTempItem=NULL;
	HUINT16						usTempSvcId = 0;
	SvcEpg_ServiceTsInfoRaw_t	stEpgTsInfo;
	Handle_t					hSvc;
	DbSvc_TsInfo_t				stTsInfo;
	DbSvc_Info_t					stSvcInfo;
	void						*pData = NULL;
	SvcEpg_SvcEventListRaw_t		*pSvcEventList;
	POINTER_LIST_T				*pCurrent = NULL, *pRoot = NULL;
	HUINT8						*pucRawSec;

	HxLOG_Print("\n============ P/F EIT PRINT ============\n");

	hError = MGR_LIVE_GetServiceHandle (MGR_ACTION_GetMainActionId(), 0, &hSvc);
	if (ERR_OK == hError)
	{
		hError = DB_SVC_GetTsInfoByServiceHandle(hSvc, &stTsInfo);
		if (ERR_OK == hError)
		{
			hError = DB_SVC_GetServiceInfo(hSvc, &stSvcInfo);
		}
	}

	HxLOG_Print("\tHead : %s\n",((0 == ulHeadInfo) ? "ACT_HEAD" : "OTH_HEAD"));

	if (ERR_OK != hError)
		return;

	HxLOG_Print("\tCurrent Ts -- OnId:0x%X TsId:0x%X\n",stTsInfo.usOnId,stTsInfo.usTsId);

	HxLOG_Print("\n============================================\n");

	ENTER_CRITICAL_EPGRAWDB;
	hError = SVC_EPGDB_GetTreeMgr(&hTree);
	if(hError != ERR_TREEUTIL_OK || hTree == NULL)
	{
		HxLOG_Print("[xsvc_epgcontext_PfDebugFunction_Base] local_epgdb_ukdtt_GetTreeMgr fail!!\n");
		goto EXIT_CNXTRAWPF_FUNC;
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
		HxLOG_Print("[xsvc_epgcontext_PfDebugFunction_Base] EPGDB_FindDelieveryItem fail!!\n");
		goto EXIT_CNXTRAWPF_FUNC;
	}

///////////////////////////////////////////////////

	hError = UTIL_TREEUTIL_GetChildItem(hDeliItem, &hTsFirstChild);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("[xsvc_epgcontext_PfDebugFunction_Base] GetChildItem fail!!\n");
		goto EXIT_CNXTRAWPF_FUNC;
	}

	hTsTempItem = hTsFirstChild;
	while(hTsTempItem)
	{
		hError = UTIL_TREEUTIL_GetItemUserInfo(hTsTempItem, sizeof(SvcEpg_ServiceTsInfoRaw_t), (void*)&stEpgTsInfo);
		if(hError != ERR_OK)
		{
			HxLOG_Print("[xsvc_epgcontext_PfDebugFunction_Base] GetItemUserInfo fail!!\n");
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
				HxLOG_Print("[xsvc_epgcontext_PfDebugFunction_Base] GetChildItem fail!!\n");
				goto EXIT_CNXTRAWPF_FUNC;
			}

			hSvcTempItem = hSvcFirstChild;
			while(hSvcTempItem)
			{
				hError = UTIL_TREEUTIL_GetItemUserInfo(hSvcTempItem, sizeof(HUINT16), (void*)&usTempSvcId);
				if(hError != ERR_OK)
				{
					HxLOG_Print("[xsvc_epgcontext_PfDebugFunction_Base] GetItemUserInfo fail!!\n");
					goto GOTO_NEXT_SVC_ITEM;
				}

				HxLOG_Print("\tSvcId:0x%X\n",usTempSvcId);
				hError = UTIL_TREEUTIL_GetItemData(hSvcTempItem, &pData);
				if(hError != ERR_TREEUTIL_OK|| pData == NULL)
				{
					HxLOG_Print("[xsvc_epgcontext_PfDebugFunction_Base] UTIL_TREEUTIL_GetItemData fail!!\n");
					goto GOTO_NEXT_SVC_ITEM;
				}
				pSvcEventList = (SvcEpg_SvcEventListRaw_t*)pData;

				pCurrent = pRoot = pSvcEventList->pPfList;
				if (NULL == pCurrent)
				{
					HxLOG_Print("\t\tNULL\n");
					goto GOTO_NEXT_SVC_ITEM;
				}
				if (0 == ulCmdFlag)
				{
					HUINT16			usSecLen,usTmp;
					unsigned char		*pucEventLoof;
					unsigned long		ulEvtCnt;
					HUINT8			ucDesLen,ucTmp;

					do
					{
						pucRawSec = (HUINT8 *)pCurrent->pvContents; // This is Raw Section.
						usSecLen = (pucRawSec[1] & 0x0f); usSecLen = usSecLen << 8;
						usSecLen |= (pucRawSec[2] & 0xff);

						HxLOG_Print("\t    TableId:0x%X SecNum:%d LastSecNum:%d SegLastSecNum:%d LastTableId:0x%X Ver:0x%X usSecLen:%d\n",
							get8bit(pucRawSec),get8bit(pucRawSec+6),
							get8bit(pucRawSec+7),get8bit(pucRawSec+12),
							get8bit(pucRawSec+13),(((*(pucRawSec+5)) >> 1) & 0x1F),
							usSecLen);
						usSecLen = usSecLen - 11 - 4;
						pucEventLoof = pucRawSec + (11+3);
						ulEvtCnt = 0;
						while(usSecLen > 0)
						{
							UNIXTIME ulUnixTime;
							HxDATETIME_t stDateTime;
							unsigned char *pucDesLoof;
							unsigned char ucDesTag,ucEachDesLen;
							int 			nTmpDesLen;

							usTmp = (pucEventLoof[0] & 0xff); usTmp = usTmp << 8;
							usTmp |= (pucEventLoof[1] & 0xff);
							HxLOG_Print("\t\t%dth Event Id : 0x%X\n", ulEvtCnt,usTmp);

							HxLOG_Print("\t\t\tStart Time : 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",
								pucEventLoof[2],pucEventLoof[3],pucEventLoof[4],pucEventLoof[5],pucEventLoof[6]);

							HLIB_DATETIME_ConvertMJDBcdTimeToUnixTime(&pucEventLoof[2], &ulUnixTime);
							HLIB_DATETIME_ConvertUnixTimeToDateTime(ulUnixTime, &stDateTime);


							HxLOG_Print("\t\t\t\t[%d-%d-%d][%d-%d-%d-%d]\n",
								stDateTime.stDate.usYear,stDateTime.stDate.ucMonth,stDateTime.stDate.ucDay,
								stDateTime.stTime.ucHour,stDateTime.stTime.ucMinute,stDateTime.stTime.ucSecond,stDateTime.stTime.usMillisecond);

							HxLOG_Print("\t\t\tDuration : 0x%02X 0x%02X 0x%02X[%02d-%02d-%02d]\n",
								pucEventLoof[7],pucEventLoof[8],pucEventLoof[9],pucEventLoof[7],pucEventLoof[8],pucEventLoof[9]);

							ucTmp = pucEventLoof[10];
							HxLOG_Print("\t\t\trunning status:0x%X\n",((ucTmp >> 5) & 7));
							HxLOG_Print("\t\t\tfree CA mode:%d\n",((ucTmp >> 4) & 1));
							nTmpDesLen = ucDesLen = get12bit(&pucEventLoof[10]);
							HxLOG_Print("\t\t\tdescriptor loop length:%d\n",ucDesLen);
#if 1
							pucDesLoof = &pucEventLoof[12];
							while(nTmpDesLen >= 0)
							{
								ucDesTag = pucDesLoof[0];
								ucEachDesLen = pucDesLoof[1];
								HxLOG_Print("\t\t\t\t Event DesTag:0x%X Len:%d\n",ucDesTag,ucEachDesLen);
								if (0x7E == ucDesTag)
								{
									//Util_Print_Dump ((unsigned char *)(pucDesLoof+2),ucEachDesLen, 0,1);
								}
								nTmpDesLen -= (ucEachDesLen + 2);
								pucDesLoof += (ucEachDesLen + 2);
							}
#endif
							pucEventLoof += ucDesLen + 12;
							usSecLen -= (ucDesLen + 12);
							ulEvtCnt++;
						}

						pCurrent = pCurrent->pNext ;
						if ( pCurrent == pRoot )
						{
							break;
						}
					} while ( pCurrent && pCurrent != pRoot ) ;
				}
				else if (1 == ulCmdFlag)
				{
					VK_memset32(s_aucPfSecBuildMask,0xff,(sizeof(HUINT8) * 32));
					VK_memset32(s_aucPfCmpSecBuildMask,0,(sizeof(HUINT8) * 32));
					s_ucPfLastSecNum = 0;
					VK_memset32(s_aucPfNonRcvSecBuildMask,0xff,(sizeof(HUINT8) * 32));
					{
						HUINT8 ucTableId,ucSecNum,ucLastSecNum,ucSegLastSecNum;

						do
						{
							pucRawSec = (HUINT8 *)pCurrent->pvContents; // This is Raw Section.
							ucTableId = get8bit(pucRawSec);
							ucSecNum = get8bit(pucRawSec+6);
							ucLastSecNum = get8bit(pucRawSec+7);
							ucSegLastSecNum = get8bit(pucRawSec+12);

							DBG_PF_SEGMENTBITMASK(s_aucPfSecBuildMask, ucLastSecNum);
							DBG_PF_SEGMENTBITMASK(s_aucPfSecBuildMask, ucSegLastSecNum);
							DBG_PF_CLEARBITMASK(s_aucPfSecBuildMask, ucSecNum);
							if (s_ucPfLastSecNum < ucLastSecNum)
								s_ucPfLastSecNum = ucLastSecNum;
							pCurrent = pCurrent->pNext ;
							if ( pCurrent == pRoot )
							{
								break;
							}
						} while ( pCurrent && pCurrent != pRoot ) ;

						if( memcmp((char *)s_aucPfSecBuildMask, (char *)s_aucPfCmpSecBuildMask, (sizeof(HUINT8)*((s_ucPfLastSecNum >> 3) + 1))) == 0)
						{
							HxLOG_Print("\t\tTableId:0x%X is complete\n",((ulHeadInfo == ACT_HEAD) ? 0x4E : 0x4F));
						}
						else if ( memcmp((char *)s_aucPfSecBuildMask, (char *)s_aucPfNonRcvSecBuildMask, 32) == 0)
						{
							// SKIP
						}
						else
						{
							HxLOG_Print("\t\tTableId:0x%X is not complete\n", ((ulHeadInfo == ACT_HEAD) ? 0x4E : 0x4F));
							HxLOG_Print("\t\t\t[0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X]\n",
									s_aucPfSecBuildMask[0],s_aucPfSecBuildMask[1],s_aucPfSecBuildMask[2],s_aucPfSecBuildMask[3],
									s_aucPfSecBuildMask[4],s_aucPfSecBuildMask[5],s_aucPfSecBuildMask[6],s_aucPfSecBuildMask[7],
									s_aucPfSecBuildMask[8],s_aucPfSecBuildMask[9],s_aucPfSecBuildMask[10],s_aucPfSecBuildMask[11],
									s_aucPfSecBuildMask[12],s_aucPfSecBuildMask[13],s_aucPfSecBuildMask[14],s_aucPfSecBuildMask[15]);
							HxLOG_Print("\t\t\t[0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X]\n",
									s_aucPfSecBuildMask[16],s_aucPfSecBuildMask[17],s_aucPfSecBuildMask[18],s_aucPfSecBuildMask[19],
									s_aucPfSecBuildMask[20],s_aucPfSecBuildMask[21],s_aucPfSecBuildMask[22],s_aucPfSecBuildMask[23],
									s_aucPfSecBuildMask[24],s_aucPfSecBuildMask[25],s_aucPfSecBuildMask[26],s_aucPfSecBuildMask[27],
									s_aucPfSecBuildMask[28],s_aucPfSecBuildMask[29],s_aucPfSecBuildMask[30],s_aucPfSecBuildMask[31]);
						}
					}
				}
				else if (2 == ulCmdFlag)
				{
					do
					{
						pucRawSec = (HUINT8 *)pCurrent->pvContents; // This is Raw Section.
						HxLOG_Print("\t\tTableId:0x%X SecNum:%d LastSecNum:%d SegLastSecNum:%d LastTableId:0x%X Ver:0x%X\n",
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

GOTO_NEXT_SVC_ITEM:
				hError = UTIL_TREEUTIL_GetNextItem(hSvcTempItem, &hSvcTempItem);
				if(hError != ERR_OK)
				{
					HxLOG_Print("[xsvc_epgcontext_PfDebugFunction_Base] GetNextItem fail!!\n");
					goto EXIT_CNXTRAWPF_FUNC;
				}
			}
		}

GOTO_NEXT_TS_ITEM:
		hError = UTIL_TREEUTIL_GetNextItem(hTsTempItem, &hTsTempItem);
		if(hError != ERR_OK)
		{
			HxLOG_Print("[xsvc_epgcontext_PfDebugFunction_Base] GetNextItem fail!!\n");
			goto EXIT_CNXTRAWPF_FUNC;
		}
	}

EXIT_CNXTRAWPF_FUNC :
	LEAVE_CRITICAL_EPGRAWDB;
	HxLOG_Print("============================================\n");
#endif
}

#endif // #if defined(CONFIG_DEBUG)


/* end of file */
