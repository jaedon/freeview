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

#include "linkedlist.h"

#include <db_svc.h>

#include <pal_pipe.h>

#include <svc_epg.h>
#include "./local_include/_svc_epg_common.h"
#include "./local_include/_svc_epg_common_lib.h"
#include "./local_include/_svc_epg_raw_mgr.h"

#include "./local_include/_svc_epg_event.h"
#include "./local_include/_svc_epg_query.h"
#include "./local_include/_svc_epg_manager.h"

#include "./local_include/_svc_epg_context_mgr.h"
#include "./local_include/_svc_epg_db_raw_lib.h"

#include <psvc_epg.h>

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define TASK_EPGMAIN_NAME						"EpgMainTask"
#define MSGQUEUE_EPGMAIN_NAME				"EpgMainQueue"


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eEPG_CB_KIND_InitAction = 0,
	eEPG_CB_KIND_Start,
	eEPG_CB_KIND_Stop,
	eEPG_CB_KIND_LoadData,
	eEPG_CB_KIND_UnLoadData,
	eEPG_CB_KIND_StartContent,
	eEPG_CB_KIND_StopContent,
	eEPG_CB_KIND_Max,
} SvcEpg_CbKind_e;

typedef struct tagEpgCbItem
{
	HBOOL		bAllocated;
	svcEpg_Callback_t	stCallback;
} SvcEpg_CbItem_t;

typedef struct tagEPG_CB_Map_t
{
	SvcEpg_CbItem_t		astCbList[eEPG_KIND_MAX];
} SvcEpg_CbMap_t;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HUINT32 						s_ulEpgmainMsgQueId = 0;
STATIC HUINT32						s_ulEpgmainTaskId = 0;
STATIC SvcEpg_CbMap_t				s_stEpgCbMap;
STATIC PostMsgToMgrCb_t				s_fnSvcEpgPostMsgCb = NULL;
STATIC SvcEpg_SiGetTimeOutCb_t		s_fnSvcEpgTimeoutCb = NULL;

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
#define ____LOCAL_FUNC_PROTOTYPE____

STATIC HERROR				svc_epg_GetMsgQueueIdInternal(HUINT32 *pulMsgQueueId);
STATIC HERROR				svc_epg_SetMsgQueueId(HUINT32 ulMsgQueueId);
STATIC HERROR				svc_epg_SetTaskId(HUINT32 ulTaskId);
STATIC HERROR				svc_epg_InitMain(void);
STATIC void					svc_epg_TaskProc(void *pParam);
STATIC HERROR				svc_epg_ProcessMsgRaw(void);
STATIC HERROR				svc_epg_GetCb(SvcEpg_EpgKind_e eEpgKind, SvcEpg_CbKind_e eEpgCbKind, void **ppfnCallback);

#define ____LOCAL_FUNC____

STATIC HERROR svc_epg_GetCb(SvcEpg_EpgKind_e eEpgKind, SvcEpg_CbKind_e eEpgCbKind, void **ppfnCallback)
{
	HERROR		hErr;

	if (eEPG_KIND_MAX <= eEpgKind)
		return ERR_FAIL;

	if (eEPG_CB_KIND_Max <= eEpgCbKind)
		return ERR_FAIL;

	if (NULL == ppfnCallback)
		return ERR_FAIL;

	hErr = ERR_FAIL;
	if (s_stEpgCbMap.astCbList[eEpgKind].bAllocated)
	{
		svcEpg_Callback_t	*pstCallback = &s_stEpgCbMap.astCbList[eEpgKind].stCallback;

		hErr = ERR_OK;
		switch(eEpgCbKind)
		{
		case eEPG_CB_KIND_InitAction:
			*ppfnCallback = pstCallback->pfnEpgInitAction;
			break;

		case eEPG_CB_KIND_Start:
			*ppfnCallback = pstCallback->pfnEpgStart;
			break;

		case eEPG_CB_KIND_Stop:
			*ppfnCallback = pstCallback->pfnEpgStop;
			break;

		case eEPG_CB_KIND_LoadData:
			*ppfnCallback = pstCallback->pfnEpgLoadData;
			break;

		case eEPG_CB_KIND_UnLoadData:
			*ppfnCallback = pstCallback->pfnEpgUnLoadData;
			break;

		case eEPG_CB_KIND_StartContent:
			*ppfnCallback = pstCallback->pfnEpgStartContent;
			break;

		case eEPG_CB_KIND_StopContent:
			*ppfnCallback = pstCallback->pfnEpgStopContent;
			break;

		default:
			hErr = ERR_FAIL;
			break;
		}
	}

	return hErr;
}


STATIC HERROR				svc_epg_GetMsgQueueIdInternal(HUINT32 *pulMsgQueueId)
{
	HERROR				hError = ERR_EPG_FAIL;

	if (pulMsgQueueId == NULL)
	{
		HxLOG_Print("Param null!!\n");
		goto EPGMAIN_FUCN_EXIT;
	}

	*pulMsgQueueId = s_ulEpgmainMsgQueId;

	hError = ERR_EPG_OK;

EPGMAIN_FUCN_EXIT :

	return hError;
}



STATIC HERROR				svc_epg_SetMsgQueueId(HUINT32 ulMsgQueueId)
{
	HERROR			hError = ERR_EPG_FAIL;

	s_ulEpgmainMsgQueId = ulMsgQueueId;

	hError = ERR_EPG_OK;

	return hError;
}



STATIC HERROR				svc_epg_SetTaskId(HUINT32 ulTaskId)
{
	HERROR				hError = ERR_EPG_FAIL;

	s_ulEpgmainTaskId = ulTaskId;

	hError = ERR_EPG_OK;

	return hError;
}

STATIC HERROR				svc_epg_InitMain(void)
{
	HERROR				hError = ERR_EPG_FAIL;
	HINT32				lResult = 0;
	HUINT32				ulMsgQueueId = 0;
	HUINT32				ulTaskId = 0;

	HxLOG_Debug("Enter!!\n");

	HxSTD_memset(&s_stEpgCbMap, 0, sizeof(SvcEpg_CbMap_t));
	hError = SVC_EPG_InitProduct();
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("SVC_EPG_InitProduct fail!!\n");
		goto EPGMAIN_FUCN_EXIT;
	}

	hError = SVC_EPG_InitResource();
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("SVC_EPG_InitResource fail!!\n");
		goto EPGMAIN_FUCN_EXIT;
	}

	lResult = VK_MSG_Create(MW_EPG_MSGQ_SIZE,
							sizeof(svcEpg_Msg_t),
							(const char*)MSGQUEUE_EPGMAIN_NAME,
							(unsigned long*)&ulMsgQueueId,
							VK_SUSPENDTYPE_FIFO);
	if (lResult != VK_OK)
	{
		HxLOG_Print("MSG_Create fail!!\n");
		goto EPGMAIN_FUCN_EXIT;
	}
	hError = svc_epg_SetMsgQueueId(ulMsgQueueId);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("SetMsgQueueId fail!!\n");
		goto EPGMAIN_FUCN_EXIT;
	}

	lResult = VK_TASK_Create(	svc_epg_TaskProc,
								MW_EPG_TASK_PRIORITY,
								MW_EPG_TASK_STACK_SIZE,
								(const char*)TASK_EPGMAIN_NAME,
								NULL,
								(unsigned long*)&ulTaskId,
								0);
	if (lResult != VK_OK)
	{
		HxLOG_Print("MSG_Create fail!!\n");
		goto EPGMAIN_FUCN_EXIT;
	}
	hError = svc_epg_SetTaskId(ulTaskId);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("SetMsgQueueId fail!!\n");
		goto EPGMAIN_FUCN_EXIT;
	}
	lResult = VK_TASK_Start(ulTaskId);
	if (lResult != VK_OK)
	{
		HxLOG_Print("VK_TASK_Start fail!!\n");
		goto EPGMAIN_FUCN_EXIT;
	}

	hError = ERR_EPG_OK;

EPGMAIN_FUCN_EXIT :

	HxLOG_Debug("Leave (retVal:0x%X)!!\n",hError);
	return hError;
}



STATIC void				svc_epg_TaskProc(void *pParam)
{
	HERROR				hError = ERR_EPG_FAIL;

	HAPPY(pParam);

	while (1)
	{

		hError = svc_epg_ProcessMsgRaw();
		if(hError != ERR_OK)
		{
			HxLOG_Print("svc_epg_ProcessMsgRaw fail!!\n");
		}

	}

	return;

}


STATIC HERROR				svc_epg_ProcessMsgRaw(void)
{
	HERROR				hError = ERR_EPG_FAIL;
	HINT32				lResult = 0;
	HUINT32				ulMsgQueueId = 0;
	svcEpg_Msg_t				stMsg;
	HUINT16				usCommand = 0;
	HUINT16				usProcess = 0;
	Handle_t				hAction = HANDLE_NULL;

	HxLOG_Debug("Enter!!\n");

	hError = svc_epg_GetMsgQueueIdInternal(&ulMsgQueueId);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("GetMsgQueueId fail!!\n");
		goto EXIT_EPGMAIN_FUNC;
	}

	lResult = VK_MSG_Receive (ulMsgQueueId, &stMsg, sizeof(svcEpg_Msg_t));
	if (lResult != VK_OK)
	{
		HxLOG_Print("GetMsgQueueId fail!!\n");
		goto EXIT_EPGMAIN_FUNC;
	}

	hError = SVC_EPG_GetEpgMessageInfo(stMsg.ulMessage, &usCommand, &usProcess);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("GetEpgMessageInfo fail!!\n");
		goto EXIT_EPGMAIN_FUNC;
	}
	hAction = (Handle_t)stMsg.ulParam1;

	switch(usCommand)
	{
		case EPG_CMD_START :
			hError = SVC_EPGCONTEXT_StartContext(hAction, usProcess, &stMsg);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Print("SVC_EPGCONTEXT_StartContext fail!!\n");
				break;
			}

			hError = SVC_EPGCONTEXT_DoAction(usCommand, hAction, usProcess, &stMsg);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Print("SVC_EPGCONTEXT_DoAction fail!!\n");
				break;
			}
			break;

		case EPG_CMD_STOP :
			hError = SVC_EPGCONTEXT_StopContext(hAction, usProcess, &stMsg);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Print("SVC_EPGCONTEXT_StopContext fail!!\n");
				break;
			}
			hError = SVC_EPGCONTEXT_DoAction(usCommand, hAction, usProcess, &stMsg);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Print("SVC_EPGCONTEXT_DoAction fail!!\n");
				break;
			}
			break;

		case EPG_CMD_START_FILTER:
		case EPG_CMD_STOP_FILTER:
		case EPG_CMD_NEXT_FILTER_TIMER :
		case EPG_CMD_PFACT_1CYCLE :
		case EPG_CMD_NEXT_FILTER:
		case EPG_CMD_PMT_RECEIVED :
		case EPG_CMD_PAT_RECEIVED :
		case EPG_CMD_SDT_RECEIVED :
		case EPG_CMD_DSI_RECEIVED :
		case EPG_CMD_DDB_RECEIVED :
		case EPG_CMD_DSMCC_CHANGE :
		case EPG_CMD_CONTEXT :
#if defined(CONFIG_DEBUG)
		case EPG_CMD_PRINT:
		case EPG_CMD_CHECK_DONE:
		case EPG_CMD_CHECK_STATUS:
#endif
			hError = SVC_EPGCONTEXT_DoAction(usCommand, hAction, usProcess, &stMsg);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Print("SVC_EPGCONTEXT_DoAction fail!!\n");
				break;
			}
			break;
#if defined(CONFIG_MW_EPG_TVTV)
		case EPG_CMD_STOP_TVTV_ACTION :
			hError = SVC_EPGCONTEXT_StartContext(hAction, usProcess, &stMsg);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Print("SVC_EPGCONTEXT_StartContext fail!!\n");
				break;
			}

			hError = SVC_EPGCONTEXT_DoAction(usCommand, hAction, usProcess, &stMsg);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Print("SVC_EPGCONTEXT_DoAction fail!!\n");
				break;
			}
			break;
#endif
		default :
			break;
	}

	hError = ERR_EPG_OK;

EXIT_EPGMAIN_FUNC :

	HxLOG_Debug("Leave (retVal:0x%X)!!\n",hError);
	return hError;
}

#define ____MODULE_INTERNAL_API____
PostMsgToMgrCb_t svc_epg_GetPostMsgCb(void)
{
	return s_fnSvcEpgPostMsgCb;
}

HERROR	svc_epg_GetMsgQueueId(HUINT32 *pulMsgQueueId)
{
	return svc_epg_GetMsgQueueIdInternal(pulMsgQueueId);
}

#define ____GLOBAL_FUNC____

HERROR	SVC_EPG_RegisterCallback(SvcEpg_EpgKind_e eEpgKind, svcEpg_Callback_t *pstCallback)
{
	if (eEpgKind >= eEPG_KIND_MAX)
		return ERR_FAIL;

	if (NULL == pstCallback)
		return ERR_FAIL;

	s_stEpgCbMap.astCbList[eEpgKind].bAllocated = TRUE;
	HxSTD_memcpy(&s_stEpgCbMap.astCbList[eEpgKind].stCallback, pstCallback, sizeof(svcEpg_Callback_t));

	return ERR_OK;
}

HERROR	SVC_EPG_InitProduct(void)
{
	return psvc_epg_InitProduct();
}

HERROR	SVC_EPG_InitResource(void)
{
	return psvc_epg_InitResource();
}

HERROR	SVC_EPG_Init(PostMsgToMgrCb_t fnSvcEpgPostMsgCb)
{
	s_fnSvcEpgPostMsgCb = fnSvcEpgPostMsgCb;

	svc_epg_Init();

	return svc_epg_InitMain();
}

HERROR	SVC_EPG_InitAction(SvcEpg_EpgKind_e eEpgKind)
{
	HERROR	hErr = ERR_EPG_NOT_SUPPORTED;
	pfnEpgInitAction_t pfnEpgInitAction = NULL;

	hErr = svc_epg_GetCb(eEpgKind, eEPG_CB_KIND_InitAction, (void **)&pfnEpgInitAction);
	if (ERR_OK != hErr || NULL == pfnEpgInitAction)
	{
		return ERR_FAIL;
	}

	return (* pfnEpgInitAction)(eEpgKind);
}

#ifdef CONFIG_DEBUG
const HINT8* INT_UTIL_GETSTR_SvcEpg_EpgKind_e(SvcEpg_EpgKind_e eEpgKind)
{
	switch (eEpgKind)
	{
	ENUM_TO_STR(eEPG_KIND_DVB);
	ENUM_TO_STR(eEPG_KIND_TVTV);
	ENUM_TO_STR(eEPG_KIND_EIT_PLUS);
	ENUM_TO_STR(eEPG_KIND_FSAT);
	ENUM_TO_STR(eEPG_KIND_HUMAX);
	ENUM_TO_STR(eEPG_KIND_MAX);
	default:
		break;
	}

	return ("Unknown");
}

const HINT8* INT_UTIL_GETSTR_SvcEpg_EventKind_e(SvcEpg_EventKind_e eEvtKind)
{
	switch (eEvtKind)
	{
	ENUM_TO_STR(eEPG_EVT_KIND_ALL);
	ENUM_TO_STR(eEPG_EVT_KIND_PF_ALL);
	ENUM_TO_STR(eEPG_EVT_KIND_PF_ACT);
	ENUM_TO_STR(eEPG_EVT_KIND_PF_OTH);
	ENUM_TO_STR(eEPG_EVT_KIND_SCH_ALL);
	ENUM_TO_STR(eEPG_EVT_KIND_SCH_ACT);
	ENUM_TO_STR(eEPG_EVT_KIND_SCH_OTH);
	ENUM_TO_STR(eEPG_EVT_KIND_PFEXT_OTH);
	ENUM_TO_STR(eEPG_EVT_KIND_SCHEXT_OTH);
	default:
		break;
	}

	return ("Unknown");
}
#endif


HERROR			SVC_EPG_Start(SvcEpg_EpgKind_e eEpgKind, SvcEpg_EventKind_e eEvtKind, Handle_t hAction, Handle_t hService, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	HERROR	hErr = ERR_EPG_NOT_SUPPORTED;
	pfnEpgStart_t	pfnEpgStart = NULL;

#ifdef CONFIG_DEBUG
	HxLOG_Print("(%s : %s) hAction(0x%x) hService(%d) ulParam1(%d) ulParam2(%d) ulParam3(%d)\r\n"
				, INT_UTIL_GETSTR_SvcEpg_EpgKind_e(eEpgKind)
				, INT_UTIL_GETSTR_SvcEpg_EventKind_e(eEvtKind)
				, hAction, hService, ulParam1, ulParam2, ulParam3);
#endif

	hErr = svc_epg_GetCb(eEpgKind, eEPG_CB_KIND_Start, (void **)&pfnEpgStart);
	if (ERR_OK != hErr || NULL == pfnEpgStart)
	{
		return ERR_FAIL;
	}

	hErr = (* pfnEpgStart)(eEpgKind, eEvtKind, hAction, hService, ulParam1, ulParam2, ulParam3);

	HxLOG_Print("hErr(%d) (-)\r\n", hErr);

	return hErr;
}

HERROR			SVC_EPG_Stop(SvcEpg_EpgKind_e eEpgKind, SvcEpg_EventKind_e eEvtKind, Handle_t hAction)
{
	HERROR	hErr = ERR_EPG_NOT_SUPPORTED;
	pfnEpgStop_t	pfnEpgStop = NULL;

#ifdef CONFIG_DEBUG
	HxLOG_Print("(%s : %s) hAction(0x%x) \r\n"
				, INT_UTIL_GETSTR_SvcEpg_EpgKind_e(eEpgKind)
				, INT_UTIL_GETSTR_SvcEpg_EventKind_e(eEvtKind)
				, hAction);
#endif

	hErr = svc_epg_GetCb(eEpgKind, eEPG_CB_KIND_Stop, (void **)&pfnEpgStop);
	if (ERR_OK != hErr || NULL == pfnEpgStop)
	{
		return ERR_FAIL;
	}

	return (* pfnEpgStop)(eEpgKind, eEvtKind, hAction);
}

HERROR			SVC_EPG_LoadData(SvcEpg_EpgKind_e eEpgKind, Handle_t hAction, Handle_t hService)
{
	HERROR	hErr = ERR_EPG_NOT_SUPPORTED;
	pfnEpgLoadData_t pfnEpgLoad = NULL;

	hErr = svc_epg_GetCb(eEpgKind, eEPG_CB_KIND_LoadData, (void **)&pfnEpgLoad);
	if (ERR_OK != hErr || NULL == pfnEpgLoad)
	{
		return ERR_FAIL;
	}

	return (* pfnEpgLoad)(eEpgKind, hAction, hService);
}

HERROR			SVC_EPG_UnLoadData(SvcEpg_EpgKind_e eEpgKind, Handle_t hAction)
{
	HERROR	hErr = ERR_EPG_NOT_SUPPORTED;
	pfnEpgUnLoadData_t pfnEpgUnLoad = NULL;

	hErr = svc_epg_GetCb(eEpgKind, eEPG_CB_KIND_UnLoadData, (void **)&pfnEpgUnLoad);
	if (ERR_OK != hErr || NULL == pfnEpgUnLoad)
	{
		return ERR_FAIL;
	}

	return (* pfnEpgUnLoad)(eEpgKind, hAction);
}

HERROR				SVC_EPG_StartContent(SvcEpg_EpgKind_e eEpgKind, Handle_t hAction, Handle_t hService)
{
	HERROR	hErr = ERR_EPG_NOT_SUPPORTED;
	pfnEpgStartContent_t	pfnEpgStartContent = NULL;

	hErr = svc_epg_GetCb(eEpgKind, eEPG_CB_KIND_StartContent, (void **)&pfnEpgStartContent);
	if (ERR_OK != hErr || NULL == pfnEpgStartContent)
	{
		return ERR_FAIL;
	}

	return (* pfnEpgStartContent)(eEpgKind, hAction, hService);
}

HERROR				SVC_EPG_StopContent(SvcEpg_EpgKind_e eEpgKind, Handle_t hAction)
{
	HERROR	hErr = ERR_EPG_NOT_SUPPORTED;
	pfnEpgStopContent_t	pfnEpgStopContent = NULL;

	hErr = svc_epg_GetCb(eEpgKind, eEPG_CB_KIND_StopContent, (void **)&pfnEpgStopContent);
	if (ERR_OK != hErr || NULL == pfnEpgStopContent)
	{
		return ERR_FAIL;
	}

	return (* pfnEpgStopContent)(eEpgKind, hAction);
}


HERROR SVC_EPG_CreateCheckServiceList(void)
{
	return psvc_epg_CreateCheckServiceList();
}

// --> ip epg
void			SVC_EPG_SetServiceConstraint(SvcEpg_Type_b bwType, const HxVector_t *svcuidList)
{
	svc_epg_SetServiceConstraint(bwType, svcuidList);
}
// --> ip epg

#define _____OLD_SOURCE______

//////// old epg


// --> Freesat
void		SVC_EPG_StopSerialization (void)
{
	// TODO: 새로 구현해야 됩니다..
	//svc_epg_StopSerialization ();
}

#if 0
DesHeader_t * SVC_EPG_GetFirstCachedDescriptorWithLanguage (SvcEpg_EventInfo_t *self, HUINT8 tag, HxLANG_Id_e language)
{
	// TODO: 새로 구현해야 됩니다..
	//return svc_epgeventdvb_GetFirstCachedDescriptorWithLanguage (self, tag, language);

	return NULL;

}
#endif
//<-- Freesat



#if defined(CONFIG_MW_EPG_TVTV)
HERROR SVC_EPG_CmdCreateBuffer_Tvtv(void)
{
	return psvc_epg_CmdCreateBuffer_Tvtv();
}

HERROR SVC_EPG_CmdCreateTvstInfo_Tvtv(void)
{
	return psvc_epg_CmdCreateTvstInfo_Tvtv();
}

HERROR SVC_EPG_CmdCreateDgktInfo_Tvtv(HBOOL bNeedConverting)
{
	return psvc_epg_CmdCreateDgktInfo_Tvtv(bNeedConverting);
}

HERROR	SVC_EPG_CmdCreateTvtvSystemInfo_Tvtv(HBOOL bNeedConverting)
{
	return psvc_epg_CmdCreateTvtvSystemInfo_Tvtv(bNeedConverting);
}

HERROR	SVC_EPG_CmdCreateTsmtInfo_Tvtv(HBOOL bNeedConverting)
{
	return psvc_epg_CmdCreateTsmtInfo_Tvtv(bNeedConverting);
}

HERROR	SVC_EPG_CmdCreateLtotInfo_Tvtv(HBOOL bNeedConverting)
{
	return psvc_epg_CmdCreateLtotInfo_Tvtv(bNeedConverting);
}

HERROR	SVC_EPG_CmdCreateEpgInfo_Tvtv(HBOOL bNeedConverting)
{
	return psvc_epg_CmdCreateEpgInfo_Tvtv(bNeedConverting);
}

void SVC_EPG_CmdInitBuffer_Tvtv(void)
{
	psvc_epg_CmdInitBuffer_Tvtv();
}
#endif

HERROR SVC_EPG_SiRegisterTimeoutCallback(SvcEpg_SiGetTimeOutCb_t fnSiTimeout)
{
	s_fnSvcEpgTimeoutCb = fnSiTimeout;

	return ERR_OK;
}

HERROR svc_epg_SiGetTableTimeOut(Handle_t hAction, SiTableType_e eTableType, HUINT32 *pulTimeout)
{
	if (NULL == pulTimeout)
		return ERR_FAIL;

	if (s_fnSvcEpgTimeoutCb)
	{
		return (* s_fnSvcEpgTimeoutCb)(hAction, eTableType, pulTimeout);
	}

	return ERR_FAIL;
}



/*********************** End of File ******************************/

