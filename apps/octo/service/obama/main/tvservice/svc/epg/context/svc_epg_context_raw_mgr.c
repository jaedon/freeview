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

#include <pal_pipe.h>
#include <db_svc.h>
#include <svc_epg.h>
#include <util.h>
#include "_svc_epg_common.h"
#include "_svc_epg_common_lib.h"
#include "_svc_epg_context_mgr.h"
#include <psvc_epg.h>
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef 	HERROR 		(*EIT_PROC)(PalSef_ReceiveMessage_t *, HUINT32)  ;


typedef struct tagEpgRawReceiverMgr
{
	HBOOL		bEnable;
	HUINT32		ucRcvTaskId;
	HUINT32		ulMsgQueueId;
	HUINT32		ulEmergencyMsgQId;
	HUINT32		ulTaskId;
	HUINT32		ulSemId;
	EIT_PROC	pfnActPfProc;
	EIT_PROC	pfnActSchProc;
	EIT_PROC	pfnOthPfProc;
	EIT_PROC	pfnOthSchProc;
}SvcEpg_RawRcvMgr_t;



/*******************************************************************/
/********************      STATIC Variables     ********************/
/*******************************************************************/
STATIC svcEpg_Context_t			s_astEpgRawContext[eEPG_ACTTYPE_MAX][eEPG_PROCESS_MAX] = {{ {0, }, }, { {0, }, }, };
STATIC svcEpg_RawBuffer_t		s_astRawBuffer[eEPG_CNXT_EIT_MAX] = { {0, {NULL, }}, };

STATIC SvcEpg_RawRcvMgr_t		s_stRawReceiverMgr = {0, };

#if defined(CONFIG_MW_EPG_TVTV)
STATIC svcEpg_RawBuffer_t			s_astDsmccBuffer[eEPG_CNXT_DSMCC_MAX] = { {0, {NULL, }}, };
#endif

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
STATIC HERROR			svc_epgcontext_SetContext (Handle_t hActon, HUINT16 usProcessType, svcEpg_Context_t *pContext);
STATIC HERROR			svc_epgcontext_GetContext (Handle_t hActon, HUINT16 usProcessType, svcEpg_Context_t **ppContext);
STATIC HERROR			svc_epgcontext_StartContext(Handle_t hActon, HUINT16 usProcessType,svcEpg_Msg_t *pstMsg);
STATIC HERROR			svc_epgcontext_StopContext(Handle_t hActon, HUINT16 usProcessType, svcEpg_Msg_t *pstMsg);
STATIC HERROR			svc_epgcontext_SetStartContext(Handle_t hActon, HUINT16 usProcessType, svcEpg_Msg_t *pstMsg);
STATIC HERROR			svc_epgcontext_SetStopContext(Handle_t hActon, HUINT16 usProcessType);
STATIC HERROR			svc_epgcontext_DoAction(HUINT32 ulContextCommand, Handle_t hActon, HUINT16 usProcessType, svcEpg_Msg_t *pstMsg);
STATIC HERROR			svc_epgcontext_GetEitSectionBuffer(HUINT32 ulContext, svcEpg_RawBuffer_t **ppBuffer);
#if defined(CONFIG_MW_EPG_TVTV)
STATIC HERROR			svc_epgcontext_GetDsmccSectionBuffer(HUINT32 ulContext, svcEpg_RawBuffer_t **ppBuffer);
#endif

#if defined(CONFIG_DEBUG)
STATIC void				svc_epgcontext_CheckByteRateNSectionCount(PalSef_ReceiveMessage_t  *pstMsg, HUINT8 *ucRawData);
#endif

#define ____GLOBAL_FUNC____
HERROR		SVC_EPGCONTEXT_SetContext (Handle_t hActon, HUINT16 usProcessType, svcEpg_Context_t *pContext)
{
	return svc_epgcontext_SetContext(hActon, usProcessType, pContext);
}



HERROR		SVC_EPGCONTEXT_StartContext(Handle_t hActon, HUINT16 usProcessType,svcEpg_Msg_t *pstMsg)
{

	return svc_epgcontext_StartContext(hActon, usProcessType, pstMsg);
}

HERROR		SVC_EPGCONTEXT_StopContext(Handle_t hActon, HUINT16 usProcessType, svcEpg_Msg_t *pstMsg)
{
	return svc_epgcontext_StopContext(hActon, usProcessType, pstMsg);
}


HERROR		SVC_EPGCONTEXT_GetContext (Handle_t hActon, HUINT16 usProcessType, svcEpg_Context_t **ppContext)
{

	return svc_epgcontext_GetContext (hActon, usProcessType, ppContext);
}



HERROR		SVC_EPGCONTEXT_DoAction(HUINT32 ulContextCommand, Handle_t hActon, HUINT16 usProcessType, svcEpg_Msg_t *pstMsg)
{
	return svc_epgcontext_DoAction(ulContextCommand, hActon, usProcessType, pstMsg);
}



HERROR		SVC_EPGCONTEXT_GetEitSectionBuffer(HUINT32 ulContext, svcEpg_RawBuffer_t **ppBuffer)
{

	return svc_epgcontext_GetEitSectionBuffer(ulContext, ppBuffer);
}


#if defined(CONFIG_MW_EPG_TVTV)
HERROR SVC_EPGCONTEXT_GetDsmccSectionBuffer(HUINT32 ulContext, svcEpg_RawBuffer_t **ppBuffer)
{
	return svc_epgcontext_GetDsmccSectionBuffer(ulContext, ppBuffer);
}
#endif



//  not defined
//STATIC HERROR			local_epgrawcontext_AllocRcvTask(HUINT16 usTableId, void* pvTaskProc, HUINT8 *pucRcvTaskId);

STATIC HERROR		svc_epgcontext_SectionRcvTask(PalSef_ReceiveMessage_t *pstMsg, HUINT32 ulParam);
STATIC HERROR		svc_epgcontext_GetNextFilterTimeDuration(HUINT16 usTableId, EPG_SpecType_e eSpec, HUINT32 *pulTimeDuration);
STATIC void			svc_epgcontext_ACPFTimerCallback(HULONG ulTimerId, void *pLocalData);
STATIC void			svc_epgcontext_OTHPFTimerCallback(HULONG ulTimerId, void *pLocalData);
STATIC void			svc_epgcontext_ACSCHTimerCallback(HULONG ulTimerId, void *pLocalData);
STATIC void			svc_epgcontext_OTHSCHTimerCallback(HULONG ulTimerId, void *pLocalData);
STATIC HERROR		svc_epgcontext_SetTimerOnce(HUINT16 usTableId, EPG_SpecType_e eSpec, HUINT32 *pulTimerId);



HERROR		SVC_EPGCONTEXT_SetTimerOnce(HUINT16 usTableId, EPG_SpecType_e eSpec, HUINT32 *pulTimerId)
{

	return svc_epgcontext_SetTimerOnce(usTableId, eSpec, pulTimerId);
}

STATIC void	svc_epgcontext_TaskProc(void *pParam)
{
	HERROR				hError = 1;
	HINT32				lResult = 0;
	PalSef_ReceiveMessage_t	stRecvMsg;
	PalSef_SendMessage_t	stEmergencyMsg;

	while (1)
	{
		lResult = VK_MSG_Receive (s_stRawReceiverMgr.ulMsgQueueId, &stRecvMsg, sizeof(PalSef_ReceiveMessage_t));
		if (lResult == VK_OK)
		{
			hError = svc_epgcontext_SectionRcvTask(&stRecvMsg, (HUINT32)0);
			if(hError != ERR_OK)
			{
				HxLOG_Print("pfnTaskProc fail!!\n");
			}
		} else HxLOG_Print("VK_MSG_Receive fail!!\n");

		lResult = VK_MSG_ReceiveTimeout(s_stRawReceiverMgr.ulEmergencyMsgQId, &stEmergencyMsg, sizeof(PalSef_SendMessage_t), 0);
		if (lResult == VK_OK)
		{
			HxLOG_Print("\n\tEMERGENCY!!! %d\n", stEmergencyMsg.eMsgClass);
			//pstEmergencyMsg->eMsgClass = eSiFilterMsg_EmergencyStop;
			//pstEmergencyMsg->ucDemuxId = pstRcvMsg->ucDemuxId;
			//pstEmergencyMsg->msg.stEmergencyStop.ulRequestId = pstRcvMsg->ulRequestId;
		}
	}

	return;
}

HERROR		SVC_EPGCONTEXT_InitContext(void *ActPfProc,void *OthPfProc,void *ActScProc,void *OthScProc)
{
	HERROR		hError = ERR_EPG_FAIL;

	if(!s_stRawReceiverMgr.bEnable)
	{
		s_stRawReceiverMgr.pfnActPfProc = (EIT_PROC)ActPfProc;
		s_stRawReceiverMgr.pfnOthPfProc = (EIT_PROC)OthPfProc;
		s_stRawReceiverMgr.pfnActSchProc = (EIT_PROC)ActScProc;
		s_stRawReceiverMgr.pfnOthSchProc = (EIT_PROC)OthScProc;


		hError = VK_TASK_Create(	(void*)svc_epgcontext_TaskProc,
									(unsigned long)UTIL_TASKUTIL_TASK_PRIORITY,
									(unsigned long)UTIL_TASKUTIL_TASK_STACK_SIZE,
									(const char*)"EpgRawMgrT",
									NULL,
									(unsigned long*)(&s_stRawReceiverMgr.ucRcvTaskId),
									0);


		hError = VK_MSG_Create(UTIL_TASKUTIL_MSGQ_SIZE,
								(unsigned long)sizeof(PalSef_ReceiveMessage_t),
								(const char*)"EpgRawMgrQ",
								(unsigned long*)(&s_stRawReceiverMgr.ulMsgQueueId),
								VK_SUSPENDTYPE_FIFO);

		hError = VK_MSG_Create(UTIL_TASKUTIL_MSGQ_SIZE,
								(unsigned long)sizeof(PalSef_SendMessage_t),
								(const char*)"EpgRawMgrEQ",
								(unsigned long*)(&s_stRawReceiverMgr.ulEmergencyMsgQId),
								VK_SUSPENDTYPE_FIFO);


		hError = VK_SEM_Create((unsigned long*)&s_stRawReceiverMgr.ulSemId, "epgcnxt_sem", VK_SUSPENDTYPE_FIFO);
		if(hError != VK_OK)
		{
			HxLOG_Print("VK_SEM_Create fail!!\n");
			return hError;
		}

		VK_TASK_Start(s_stRawReceiverMgr.ucRcvTaskId);

		s_stRawReceiverMgr.bEnable = TRUE;
	}

	return hError;
}

HERROR SVC_EPGCONTEXT_GetContextTaskId(HUINT32 *ulTaskId)
{
	*ulTaskId = s_stRawReceiverMgr.ucRcvTaskId;
	return ERR_OK;

}

HERROR SVC_EPGCONTEXT_GetMsgQId(HUINT32 *ulMsgQId)
{
	*ulMsgQId = s_stRawReceiverMgr.ulMsgQueueId;
	return ERR_OK;
}

HERROR SVC_EPGCONTEXT_GetEmergencyMsgQId(HUINT32 *ulMsgQId)
{
	*ulMsgQId = s_stRawReceiverMgr.ulEmergencyMsgQId;
	return ERR_OK;
}

HERROR SVC_EPGCONTEXT_GetSemId(HUINT32 *ulSemId)
{
	*ulSemId = s_stRawReceiverMgr.ulSemId;
	return ERR_OK;
}


#define ____LOCAL_FUNC____
STATIC INLINE HUINT32		svc_epgcontext_SemGetId (HUINT16 usProcessType, svcEpg_Context_t *pContext)
{
	return (usProcessType <= eEPG_PROCESS_EIT_SCH) ? s_stRawReceiverMgr.ulSemId : pContext->ulSemaphore;
}

STATIC void			svc_epgcontext_SemGet (HUINT16 usProcessType, svcEpg_Context_t *pContext)
{
	HUINT32	ulSemId = svc_epgcontext_SemGetId(usProcessType, pContext);

	if (ulSemId)
		VK_SEM_Get (ulSemId);
}

STATIC void			svc_epgcontext_SemRelease (HUINT16 usProcessType, svcEpg_Context_t *pContext)
{
	HUINT32	ulSemId = svc_epgcontext_SemGetId(usProcessType, pContext);

	if (ulSemId)
		VK_SEM_Release (ulSemId);
}

STATIC HERROR			svc_epgcontext_SetContext (Handle_t hAction, HUINT16 usProcessType, svcEpg_Context_t *pContext)
{
	HERROR			hError = ERR_EPG_FAIL;
	HUINT16			usActionType = eEPG_ACTTYPE_VIEW_0;

	if(pContext == NULL || hAction == HANDLE_NULL)
	{
		HxLOG_Print("param fail!!\n");
		goto EXIT_EPGRAWCONTEXT_FUNC;
	}

	hError = SVC_EPG_GetEpgActionType(hAction, &usActionType);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("SVC_EPG_GetEpgActionType fail!!\n");
		goto EXIT_EPGRAWCONTEXT_FUNC;
	}

	VK_memcpy32(&s_astEpgRawContext[usActionType][usProcessType], pContext, sizeof(svcEpg_Context_t));

	hError = ERR_EPG_OK;

EXIT_EPGRAWCONTEXT_FUNC :

	return hError;
}

STATIC HERROR			svc_epgcontext_GetContext(Handle_t hAction, HUINT16 usProcessType, svcEpg_Context_t **ppContext)
{
	HERROR			hError = ERR_EPG_FAIL;
	HUINT16			usActionType = eEPG_ACTTYPE_VIEW_0;

	if(ppContext == NULL || hAction == HANDLE_NULL)
	{
		HxLOG_Print("param fail!!\n");
		goto EXIT_EPGRAWCONTEXT_FUNC;
	}

	hError = SVC_EPG_GetEpgActionType(hAction, &usActionType);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("SVC_EPG_GetEpgActionType fail!!\n");
		goto EXIT_EPGRAWCONTEXT_FUNC;
	}

	*ppContext = &(s_astEpgRawContext[usActionType][usProcessType]);

	hError = ERR_EPG_OK;

EXIT_EPGRAWCONTEXT_FUNC :

	return hError;
}



STATIC HERROR			svc_epgcontext_StartContext(Handle_t hActon, HUINT16 usProcessType, svcEpg_Msg_t *pstMsg)
{
	HERROR				hError = ERR_EPG_FAIL;

	if(pstMsg == NULL)
	{
		HxLOG_Print("param fail!!\n");
		goto EXIT_EPGRAWCONTEXT_FUNC;
	}

	hError = svc_epgcontext_SetStartContext(hActon, usProcessType, pstMsg);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("svc_epgcontext_SetStartContext fail!!\n");
		switch(usProcessType)
		{
			case eEPG_PROCESS_EIT_PF :
			case eEPG_PROCESS_EIT_SCH :
			case eEPG_PROCESS_TVTV :
			case eEPG_PROCESS_FSAT_EIT_PFA:
			case eEPG_PROCESS_FSAT_EIT_PFO:
			case eEPG_PROCESS_FSAT_EIT_SCO:
			case eEPG_PROCESS_FSAT_EIT_PFOEXT:
			case eEPG_PROCESS_FSAT_EIT_SCOEXT:
				hError = svc_epgcontext_StopContext(hActon, usProcessType, pstMsg);
				if(hError != ERR_EPG_OK)
				{
					HxLOG_Print("svc_epgcontext_StopContext fail!!\n");
					goto EXIT_EPGRAWCONTEXT_FUNC;
				}
				else
				{
					hError = svc_epgcontext_SetStartContext(hActon, usProcessType, pstMsg);
				}
				break;
			default :
				break;

		}
		goto EXIT_EPGRAWCONTEXT_FUNC;
	}

	hError = ERR_EPG_OK;

EXIT_EPGRAWCONTEXT_FUNC :

	return hError;

}

STATIC HERROR			svc_epgcontext_StopContext(Handle_t hActon, HUINT16 usProcessType, svcEpg_Msg_t *pstMsg)
{
	HERROR				hError = ERR_EPG_FAIL;

	HAPPY(pstMsg);
	HxLOG_Debug("Enter!!\n");

	hError = svc_epgcontext_SetStopContext(hActon, usProcessType);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("svc_epgcontext_SetStopContext fail!!\n");
		goto EXIT_EPGRAWCONTEXT_FUNC;
	}

	hError = ERR_EPG_OK;

EXIT_EPGRAWCONTEXT_FUNC :
	HxLOG_Debug("Leave!!\n");

	return hError;
}





STATIC HERROR		svc_epgcontext_InitSemId(HUINT16 usProcessType, HUINT32 *pulSemId)
{
	HERROR		hErr = ERR_FAIL;
	HUINT32		ulSemId = 0;

	if (NULL == pulSemId)
		return ERR_FAIL;

	switch(usProcessType)
	{
	case eEPG_PROCESS_EIT_PF:
	case eEPG_PROCESS_EIT_SCH:
		// USE Base Semaphore
		ulSemId = s_stRawReceiverMgr.ulSemId;
		hErr = ERR_OK;
		break;

	case eEPG_PROCESS_FSAT_EIT_PFA:
	case eEPG_PROCESS_FSAT_EIT_PFO:
	case eEPG_PROCESS_FSAT_EIT_SCO:
	case eEPG_PROCESS_FSAT_EIT_PFOEXT:
	case eEPG_PROCESS_FSAT_EIT_SCOEXT:
#if defined(CONFIG_MW_EPG_TVTV)
	case eEPG_PROCESS_TVTV:
#endif
		if(VK_SEM_Create((unsigned long*)&ulSemId, "epgcnxt_sem", VK_SUSPENDTYPE_PRIORITY) != VK_OK)
		{
			HxLOG_Print("VK_SEM_Create fail!!\n");
			return hErr;
		}
		hErr = ERR_OK;
		break;

	default:
		hErr = ERR_OK;
		break;
	}

	*pulSemId = (ERR_OK == hErr) ? ulSemId : 0;

	return hErr;
}

STATIC HERROR			svc_epgcontext_GetContextData(HUINT16 usProcessType, HUINT32 ulStatus, svcEpg_CnxtProcCb_t *ppfnContextProc, EPG_SpecType_e *peSpec)
{
	HUINT32		ulCount, ulNumberOfTable;
	svcEpg_RawProcessDataInit_t*	pstEPGProcessInitTable;

	pstEPGProcessInitTable = psvc_epg_GetEpgProcessInitTable(&ulNumberOfTable);
	HxLOG_Info("ulSize: %d \n", ulNumberOfTable);

	for (ulCount=0; ulCount<ulNumberOfTable; ulCount++)
	{
		if ((usProcessType == pstEPGProcessInitTable[ulCount].usProcessType)
			&& (ulStatus == pstEPGProcessInitTable[ulCount].eStatus))
		{
			if (ppfnContextProc)
			{
				*ppfnContextProc = pstEPGProcessInitTable[ulCount].fnContextProc;
			}
			if (peSpec)
			{
				*peSpec = pstEPGProcessInitTable[ulCount].eSpec;
				HxLOG_Info("*peSpec: %d \n", *peSpec);
			}
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

STATIC HERROR			svc_epgcontext_SetStartContext(Handle_t hActon, HUINT16 usProcessType, svcEpg_Msg_t *pstMsg)
{
	HERROR				hError = ERR_EPG_FAIL;
	svcEpg_Context_t		*pContext = NULL;
	svcEpg_ContextData_t		*pData = NULL;
	svcEpg_CnxtProcCb_t		pfnProc = NULL;
	EPG_SpecType_e		 eEpgSpec = eEPG_SPEC_BASE;

	hError = svc_epgcontext_GetContext(hActon, usProcessType, &pContext);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("GetContext fail!!\n");
		return hError;
	}

	if(pContext->eStatus == eEPG_CNXT_EMPTY)
	{
		hError = svc_epgcontext_InitSemId(usProcessType, &pContext->ulSemaphore);
		if (ERR_OK != hError)
		{
			return hError;
		}
	}

	svc_epgcontext_SemGet(usProcessType, pContext);

	hError = svc_epgcontext_GetContextData(usProcessType, pContext->eStatus, &pfnProc, &eEpgSpec);
	if (ERR_OK == hError)
	{
		switch (pContext->eStatus)
		{
		case eEPG_CNXT_USED:
			hError = ERR_EPG_OK;
			goto EXIT_EPGRAWCONTEXT_FUNC;
			break;

		case eEPG_CNXT_EMPTY:
			if (NULL == pfnProc)
			{
				hError = ERR_EPG_FAIL;
				goto EXIT_EPGRAWCONTEXT_FUNC;
			}

			pContext->fnContextProc = pfnProc;
			pData = (svcEpg_ContextData_t*)OxMW_Malloc(sizeof(svcEpg_ContextData_t));
			if(pData == NULL)
			{
				hError = ERR_EPG_FAIL;
				goto EXIT_EPGRAWCONTEXT_FUNC;
			}
			VK_memset32(pData, 0x00, sizeof(svcEpg_ContextData_t));
			pContext->pCnxtData = pData;
			pContext->pCnxtData->hAction	= HANDLE_NULL;
			pContext->pCnxtData->hService	= HANDLE_NULL;
			pContext->pCnxtData->eSpec		= eEpgSpec;
			pContext->eStatus				= eEPG_CNXT_USED;
			break;

		case eEPG_CNXT_NOTUSED:
			pContext->pCnxtData->hAction	= HANDLE_NULL;
			pContext->pCnxtData->hService	= HANDLE_NULL;
			pContext->pCnxtData->eSpec		= eEpgSpec;
			pContext->eStatus				= eEPG_CNXT_USED;
			break;

		default:
			pContext->eStatus = eEPG_CNXT_USED;
			break;
		}
		hError = ERR_EPG_OK;
	}
	else
	{
		goto EXIT_EPGRAWCONTEXT_FUNC;
	}

EXIT_EPGRAWCONTEXT_FUNC :
	svc_epgcontext_SemRelease(usProcessType, pContext);

	return hError;
}

STATIC HERROR			svc_epgcontext_SetStopContext(Handle_t hActon, HUINT16 usProcessType)
{
	HERROR				hError = ERR_EPG_FAIL;
	svcEpg_Context_t		*pContext = NULL;

	hError = svc_epgcontext_GetContext(hActon, usProcessType, &pContext);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("GetContext fail!!\n");
		return hError;
	}
	if(pContext->pCnxtData == NULL)
	{
		HxLOG_Print("pContext->pCnxtData == NULL!!\n");
		return hError;
	}

	svc_epgcontext_SemGet(usProcessType, pContext);

	pContext->eStatus = eEPG_CNXT_NOTUSED;
	hError = ERR_EPG_OK;

	svc_epgcontext_SemRelease(usProcessType, pContext);

	return hError;
}

// only receive task use function
STATIC HERROR			svc_epgcontext_GetEitSectionBuffer(HUINT32 ulContext, svcEpg_RawBuffer_t **ppBuffer)
{
	HERROR				hError = ERR_EPG_FAIL;

	if(ulContext >= eEPG_CNXT_EIT_MAX || ppBuffer == NULL)
	{
		HxLOG_Print("param fail!!\n");
		goto EXIT_EPGRAWCONTEXT_FUNC;
	}

	*ppBuffer = &(s_astRawBuffer[ulContext]);

	hError = ERR_EPG_OK;

EXIT_EPGRAWCONTEXT_FUNC :

	return hError;
}

#if defined(CONFIG_MW_EPG_TVTV)
STATIC HERROR svc_epgcontext_GetDsmccSectionBuffer(HUINT32 ulContext, svcEpg_RawBuffer_t **ppBuffer)
{
	HERROR hError = ERR_EPG_FAIL;

	if(ulContext >= eEPG_CNXT_DSMCC_MAX || ppBuffer == NULL)
	{
		HxLOG_Print("param fail!!\n");
		goto EXIT_EPGRAWCONTEXT_FUNC;
	}

	*ppBuffer = &(s_astDsmccBuffer[ulContext]);

	hError = ERR_EPG_OK;

EXIT_EPGRAWCONTEXT_FUNC :

	return hError;
}
#endif


STATIC HERROR			svc_epgcontext_DoAction(HUINT32 ulContextCommand, Handle_t hActon, HUINT16 usProcessType, svcEpg_Msg_t *pstMsg)
{
	HERROR			hError = ERR_EPG_FAIL;
	svcEpg_Context_t	*pContext = NULL;

	HxLOG_Debug("Enter!!\n");
	if(hActon == HANDLE_NULL)
	{
		HxLOG_Print("param fail!!\n");
		HxLOG_Debug("Leave Error hAction is HANDLE_NULL!!\n");
		return hError;
	}

	hError = svc_epgcontext_GetContext(hActon, usProcessType, &pContext);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("GetContext fail!!\n");
		HxLOG_Debug("Leave Error svc_epgcontext_GetContext return Error!!\n");
		return hError;
	}

	if(pContext->pCnxtData == NULL)
	{
		HxLOG_Print("pContext->pCnxtData == NULL!!\n");
		HxLOG_Debug("Leave Error pContext->pCnxtData is NULL!!\n");
		return hError;
	}

	//if(s_stRawReceiverMgr.ulSemId == 0) // 문제 생기면 여기를 풀어보세요..
	if(svc_epgcontext_SemGetId(usProcessType, pContext) == 0)
	{
		HxLOG_Print("ulSemaphore == 0!!\n");
		HxLOG_Debug("Leave Error s_stRawReceiverMgr.ulSemId is NULL!!\n");
		return hError;
	}

	svc_epgcontext_SemGet(usProcessType, pContext);

	if(pContext->fnContextProc == NULL)
	{
		HxLOG_Print("pfnContextProc Null!!\n");
		goto EXIT_EPGRAWCONTEXT_FUNC;
	}

	if(pContext->pCnxtData == NULL)
	{
		HxLOG_Print("pCnxtData Null!!\n");
		goto EXIT_EPGRAWCONTEXT_FUNC;
	}

	hError = pContext->fnContextProc(ulContextCommand, (HUINT32)pstMsg, (HUINT32)pContext->pCnxtData, 0);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("pfnContextProc fail!!\n");
		goto EXIT_EPGRAWCONTEXT_FUNC;
	}

	hError = ERR_EPG_OK;

EXIT_EPGRAWCONTEXT_FUNC :
	svc_epgcontext_SemRelease(usProcessType, pContext);

	HxLOG_Debug("Leave (hError:0x%X)!!\n",hError);
	return hError;
}


// task util proc
STATIC HERROR			svc_epgcontext_SectionRcvTask(PalSef_ReceiveMessage_t *pstMsg, HUINT32 ulParam)
{
	HERROR				 hError = ERR_EPG_FAIL;
	HBOOL				 bProcessed = FALSE;
	PalSef_RawData_t	*pSiRawData = NULL;
	HUINT8				*pucSection = NULL;
	HINT32				 lIndex = 0;

	if(NULL == pstMsg)
	{
		HxLOG_Print("param fail!!\n");
		goto  EXIT_EPGRAWCONTEXT_FUNC;
	}
	HAPPY(ulParam);

#if defined(CONFIG_DEBUG)
	svc_epgcontext_CheckByteRateNSectionCount(pstMsg, (HUINT8*)pstMsg->ulDataAddr);
#endif

	if(pstMsg->ucTableId ==EIT_ACTUAL_TABLE_ID)
	{
		if(s_stRawReceiverMgr.pfnActPfProc)
		{
			hError = ERR_EPG_OK;
			s_stRawReceiverMgr.pfnActPfProc(pstMsg, ulParam);
			bProcessed = TRUE;
		}
	}
	else if(pstMsg->ucTableId ==EIT_OTHER_TABLE_ID)
	{
		if(s_stRawReceiverMgr.pfnOthPfProc)
		{
			hError = ERR_EPG_OK;
			s_stRawReceiverMgr.pfnOthPfProc(pstMsg, ulParam);
			bProcessed = TRUE;
		}
	}
	else if(pstMsg->ucTableId >=EIT_ACTUAL_SCHEDULE_TABLE_ID_0 && pstMsg->ucTableId <=EIT_ACTUAL_SCHEDULE_TABLE_ID_15)
	{
		if(s_stRawReceiverMgr.pfnActSchProc)
		{
			hError = ERR_EPG_OK;
			s_stRawReceiverMgr.pfnActSchProc(pstMsg, ulParam);
			bProcessed = TRUE;
		}
	}
	else if(pstMsg->ucTableId >=EIT_OTHER_SCHEDULE_TABLE_ID_0 && pstMsg->ucTableId <=EIT_OTHER_SCHEDULE_TABLE_ID_15)
	{
		if(s_stRawReceiverMgr.pfnOthSchProc)
		{
			hError = ERR_EPG_OK;
			s_stRawReceiverMgr.pfnOthSchProc(pstMsg, ulParam);
			bProcessed = TRUE;
		}
	}

EXIT_EPGRAWCONTEXT_FUNC :
	if(!bProcessed && pstMsg != NULL)
	{
		if(pstMsg->eSecPackageFlag == eSectionPackage_On)
		{
			pSiRawData= (PalSef_RawData_t*)pstMsg->ulDataAddr;
			if(pSiRawData)
			{
				for(lIndex = 0; lIndex < pSiRawData->num_sec; lIndex++)
				{
					if(pSiRawData->buf[lIndex])
					{
						OxSI_Free(pSiRawData->buf[lIndex]);
					}
				}
				OxMW_Free(pSiRawData);
			}
		}
		else
		{
			pucSection = (HUINT8*)pstMsg->ulDataAddr;
			if(pucSection)
			{
				OxSI_Free(pucSection);
			}
		}
	}

	return hError;
}

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
// 5 sec
#define TIMEDURATION_ACPF_BASE		(1000*5)
// 5 sec
#define TIMEDURATION_OTHPF_BASE		(1000*5)
#else
// 10 sec
#define TIMEDURATION_ACPF_BASE		(1000*10)
// 10 sec
#define TIMEDURATION_OTHPF_BASE		(1000*10)
#endif
// 2 minutes.
#define TIMEDURATION_ACSCH_BASE		(1000*60*2)
// 2 minutes.
#define TIMEDURATION_OTHSCH_BASE		(1000*60*2)

STATIC HERROR			svc_epgcontext_GetNextFilterTimeDuration(HUINT16 usTableId, EPG_SpecType_e eSpec, HUINT32 *pulTimeDuration)
{
	HERROR				hError = ERR_EPG_FAIL;

	switch(eSpec)
	{
		case eEPG_SPEC_JP_ARIB :
		case eEPG_SPEC_UKDTT :
		case eEPG_SPEC_BASE :
		case eEPG_SPEC_FSAT :
		case eEPG_SPEC_TVTV :
		case eEPG_SPEC_NORDIG :
		case eEPG_SPEC_NORDIG_HOME :
		case eEPG_SPEC_BOXER_EIT_PLUS :
		default :
			if(usTableId ==EIT_ACTUAL_TABLE_ID)
			{
				*pulTimeDuration = TIMEDURATION_ACPF_BASE;
			}
			else if(usTableId ==EIT_OTHER_TABLE_ID)
			{
				*pulTimeDuration = TIMEDURATION_OTHPF_BASE;
			}
			else if(usTableId >=EIT_ACTUAL_SCHEDULE_TABLE_ID_0 && usTableId <=EIT_ACTUAL_SCHEDULE_TABLE_ID_15)
			{
				*pulTimeDuration = TIMEDURATION_ACSCH_BASE;
			}
			else if(usTableId >=EIT_OTHER_SCHEDULE_TABLE_ID_0 && usTableId <=EIT_OTHER_SCHEDULE_TABLE_ID_15)
			{
				*pulTimeDuration = TIMEDURATION_OTHSCH_BASE;
			}
			break;
	}

	hError = ERR_OK;

	return hError;

}


STATIC void		svc_epgcontext_ACPFTimerCallback(HULONG ulTimerId, void *pLocalData)
{
	HERROR				hError = ERR_EPG_FAIL;
	HUINT32				ulMessage = 0;

	HxLOG_Info("+!!\n");
	hError = SVC_EPG_MakeEpgMessage(EPG_CMD_NEXT_FILTER_TIMER,  eEPG_PROCESS_EIT_PF, &ulMessage);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("MW_EPGMAIN_MakeEpgMessage fail!!\n");
	}
	hError = SVC_EPG_SendMessage(ulMessage, 0, ACT_HEAD, ulTimerId);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("SendMessage fail!!\n");
	}
	return;
}


STATIC void		svc_epgcontext_OTHPFTimerCallback(HULONG ulTimerId, void *pLocalData)
{
	HERROR				hError = ERR_EPG_FAIL;
	HUINT32				ulMessage = 0;

	HxLOG_Info("+!!\n");
	hError = SVC_EPG_MakeEpgMessage(EPG_CMD_NEXT_FILTER_TIMER,  eEPG_PROCESS_EIT_PF, &ulMessage);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("MW_EPGMAIN_MakeEpgMessage fail!!\n");
	}
	hError = SVC_EPG_SendMessage(ulMessage, 0, OTH_HEAD, ulTimerId);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("SendMessage fail!!\n");
	}

	return;
}


STATIC void		svc_epgcontext_ACSCHTimerCallback(HULONG ulTimerId, void *pLocalData)
{
	HERROR				hError = ERR_EPG_FAIL;
	HUINT32				ulMessage = 0;

	HxLOG_Info("+!!\n");
	hError = SVC_EPG_MakeEpgMessage(EPG_CMD_NEXT_FILTER_TIMER,  eEPG_PROCESS_EIT_SCH, &ulMessage);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("MW_EPGMAIN_MakeEpgMessage fail!!\n");
	}
	hError = SVC_EPG_SendMessage(ulMessage, 0, ACT_HEAD, ulTimerId);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("SendMessage fail!!\n");
	}

	return;
}

STATIC void		svc_epgcontext_OTHSCHTimerCallback(HULONG ulTimerId, void *pLocalData)
{
	HERROR				hError = ERR_EPG_FAIL;
	HUINT32				ulMessage = 0;

	HxLOG_Info("+!!\n");
	hError = SVC_EPG_MakeEpgMessage(EPG_CMD_NEXT_FILTER_TIMER,  eEPG_PROCESS_EIT_SCH, &ulMessage);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("MW_EPGMAIN_MakeEpgMessage fail!!\n");
	}
	hError = SVC_EPG_SendMessage(ulMessage, 0, OTH_HEAD, ulTimerId);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("SendMessage fail!!\n");
	}
	return;
}

STATIC HERROR			svc_epgcontext_SetTimerOnce(HUINT16 usTableId, EPG_SpecType_e eSpec, HUINT32 *pulTimerId)
{
	HERROR				hError = ERR_EPG_FAIL;
	HUINT32				ulTimeDuration = 0;
	HUINT32				ulTimerId = 0;

	*pulTimerId = 0;

	HxLOG_Info("+(%x, %d)!!\n", usTableId, eSpec);
	hError = svc_epgcontext_GetNextFilterTimeDuration(usTableId, eSpec, &ulTimeDuration);
	if(hError != ERR_OK)
	{
		HxLOG_Print("svc_epgcontext_GetNextFilterTimeDuration fail!!\n");
		goto EXIT_EPGRAWCONTEXT_FUNC;
	}
	HxLOG_Info("ulTimeDuration (%d)sec!!\n", ulTimeDuration/1000);

	if(usTableId ==EIT_ACTUAL_TABLE_ID)
	{
		VK_TIMER_EventAfter(ulTimeDuration, svc_epgcontext_ACPFTimerCallback, NULL, 0, (unsigned long*)&ulTimerId);
	}
	else if(usTableId ==EIT_OTHER_TABLE_ID)
	{
		VK_TIMER_EventAfter(ulTimeDuration, svc_epgcontext_OTHPFTimerCallback, NULL, 0, (unsigned long*)&ulTimerId);
	}
	else if(usTableId >=EIT_ACTUAL_SCHEDULE_TABLE_ID_0 && usTableId <=EIT_ACTUAL_SCHEDULE_TABLE_ID_15)
	{
		VK_TIMER_EventAfter(ulTimeDuration, svc_epgcontext_ACSCHTimerCallback, NULL, 0, (unsigned long*)&ulTimerId);
	}
	else if(usTableId >=EIT_OTHER_SCHEDULE_TABLE_ID_0 && usTableId <=EIT_OTHER_SCHEDULE_TABLE_ID_15)
	{
		VK_TIMER_EventAfter(ulTimeDuration, svc_epgcontext_OTHSCHTimerCallback, NULL, 0, (unsigned long*)&ulTimerId);
	}

	*pulTimerId = ulTimerId;

	hError = ERR_EPG_OK;

EXIT_EPGRAWCONTEXT_FUNC :

	return hError;
}

#if defined(CONFIG_DEBUG)
typedef enum tag_PrintBitrateStatus
{
	ePrintBitrateStatus_None 		= 0x0000,

	ePrintBitrateStatus_Bitrate 	= 0x0001,
	ePrintBitrateStatus_SectionCount= 0x0002,

	ePrintBitrateStatus_Max			= 0xFFFF
} svcEpg_PrintBitrateStatus_b;

STATIC svcEpg_PrintBitrateStatus_b s_bwPrintBitrateInfo = ePrintBitrateStatus_None;

#define EPG_RAW_MAX_TABLEID_CNT	36
STATIC void  svc_epgcontext_CheckByteRateNSectionCount(PalSef_ReceiveMessage_t	 *pstMsg, HUINT8 *ucRawData)
{
	STATIC HUINT32 s_ulEitCnt = 0;
	STATIC HUINT32 s_ulStartTick = 0;
	STATIC HUINT32 s_ulArrTableIdCnt[EPG_RAW_MAX_TABLEID_CNT] = {0,};

	STATIC HUINT32 s_ulEitByte = 0;
	STATIC HUINT32 s_ulArrTableByteCnt[EPG_RAW_MAX_TABLEID_CNT] = {0,};

	PalSef_RawData_t *pSiRawData = NULL;
	HINT32 	nLoop = 0;
	HINT32 	nTickTime = 0;

	//count sec
	if(ucRawData == NULL)
	{
		return;
	}

	if(s_ulEitCnt == 0)
	{
		s_ulStartTick = VK_TIMER_GetSystemTick() / 1000;
	}

	if(pstMsg->eSecPackageFlag != eSectionPackage_On)
	{
		s_ulArrTableIdCnt[pstMsg->ucTableId - EIT_ACTUAL_TABLE_ID]++;
		s_ulEitCnt++;

		s_ulEitByte += get12bit(&(ucRawData[1]));
		s_ulArrTableByteCnt[pstMsg->ucTableId - EIT_ACTUAL_TABLE_ID] += (get12bit(&(ucRawData[1])) + 4);
	}
	else
	{
		pSiRawData = (PalSef_RawData_t*)ucRawData;
		s_ulArrTableIdCnt[pstMsg->ucTableId - EIT_ACTUAL_TABLE_ID]+= pSiRawData->num_sec;
		s_ulEitCnt+=pSiRawData->num_sec;

		for(nLoop = 0; nLoop < pSiRawData->num_sec; nLoop++)
		{
			s_ulEitByte += get12bit(&(pSiRawData->buf[nLoop][1]));
			s_ulArrTableByteCnt[pstMsg->ucTableId - EIT_ACTUAL_TABLE_ID] += (get12bit(&(pSiRawData->buf[nLoop][1])) + 4);
		}
	}

	if(s_ulEitCnt < 10000)
	{
		return;
	}

	if(s_bwPrintBitrateInfo == 0)
	{
		s_ulEitCnt = 0;
		s_ulEitByte = 0;
		HxSTD_memset(s_ulArrTableIdCnt, 0, sizeof(HUINT32) * EPG_RAW_MAX_TABLEID_CNT);
		HxSTD_memset(s_ulArrTableByteCnt, 0, sizeof(HUINT32) * EPG_RAW_MAX_TABLEID_CNT);
		return;
	}

	nTickTime = ((VK_TIMER_GetSystemTick() / 1000)-s_ulStartTick);
	HxLOG_Print("\t### Tick : %d\n", nTickTime);
	if(nTickTime == 0)
	{
		s_ulEitCnt = 0;
		s_ulEitByte = 0;
		HxSTD_memset(s_ulArrTableIdCnt, 0, sizeof(HUINT32) * EPG_RAW_MAX_TABLEID_CNT);
		HxSTD_memset(s_ulArrTableByteCnt, 0, sizeof(HUINT32) * EPG_RAW_MAX_TABLEID_CNT);
		return;
	}

	if(s_bwPrintBitrateInfo & ePrintBitrateStatus_Bitrate)
	{
		HxLOG_Print("\t### EPG EIT Count : (%d / %d) = %d SectionCount/Second\n", s_ulEitCnt, nTickTime, s_ulEitCnt/nTickTime);

		HxLOG_Print("\t\t ====== Count Status ======\n");
		HxLOG_Print("\t\t  table id       SectionCount/Second   \n");
		for(nLoop = 0; nLoop < EPG_RAW_MAX_TABLEID_CNT; nLoop++)
		{
			if(s_ulArrTableIdCnt[nLoop] > 0)
			{
				HxLOG_Print("\t\t    0x%02X | %8d	 \n", EIT_ACTUAL_TABLE_ID + nLoop, s_ulArrTableIdCnt[nLoop]/nTickTime);
			}
		}
		HxSTD_memset(s_ulArrTableIdCnt, 0, sizeof(HUINT32) * EPG_RAW_MAX_TABLEID_CNT);
		s_ulEitCnt = 0;
	}


	if(s_bwPrintBitrateInfo & ePrintBitrateStatus_SectionCount)
	{
		HxLOG_Print("\t### EPG Byte rate : (%d / %d) = %d Byte/Second\n", s_ulEitByte, nTickTime, s_ulEitByte/nTickTime);

		HxLOG_Print("\t\t ====== Byte Status ======\n");
		HxLOG_Print("\t\t  table id       Byte/Second   \n");
		for(nLoop = 0; nLoop < EPG_RAW_MAX_TABLEID_CNT; nLoop++)
		{
			if(s_ulArrTableByteCnt[nLoop] > 0)
			{
				HxLOG_Print("\t\t    0x%02X | %8d	 \n", EIT_ACTUAL_TABLE_ID + nLoop, s_ulArrTableByteCnt[nLoop]/nTickTime);
			}
		}
		HxSTD_memset(s_ulArrTableByteCnt, 0, sizeof(HUINT32) * EPG_RAW_MAX_TABLEID_CNT);
		s_ulEitByte = 0;
	}
}

STATIC void svc_epgcontext_EnableCheckByteRateNSectionCount(void)
{
	s_bwPrintBitrateInfo = ePrintBitrateStatus_Bitrate | ePrintBitrateStatus_SectionCount;
}

void SVC_EPGCONTEXT_EnableCheckByteRateNSectionCount(void)
{
	svc_epgcontext_EnableCheckByteRateNSectionCount();
}

#endif /* #if defined(CONFIG_DEBUG) */







/*********************** End of File ******************************/
