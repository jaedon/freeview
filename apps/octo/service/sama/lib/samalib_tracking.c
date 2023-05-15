/*******************************************************************
	File Description
********************************************************************/
/**
	@file     samalib_trackschedule.c
	@brief    MW와 AP에서 사용할 수 있는 여러가지 공통 유틸리티 API

	Description: MW와 AP가 데이터를 가공하고, 판단하는 알고리즘들을 공통 유틸리티로 제공하여 각 application들이나 MW 모듈들이
				중복 제작하지 않도록 하고 공통된 알고리즘을 사용할 수 있도록 한다.
	Module: MWC/Freeview Playback Series			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <htype.h>
#include <hlib.h>
#include <vkernel.h>
#include <sama_lib.h>
#include "./local_include/sama_lib_int.h"


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#if defined(CONFIG_DEBUG)
#define	TRACKING_GOTO_IF(expr, pos)	do{if(expr){HxLOG_Debug("[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);goto pos;}}while(0)
#else
#define	TRACKING_GOTO_IF(expr, pos)	do{if(expr){goto pos;}}while(0)
#endif


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct tag_TRACKING_EVENT
{
	HINT32		nEventId;
	UNIXTIME	utTransitionTime;
	HUINT8		ucRunningStatus;
	HUINT32		slot;
	HBOOL		isPending;
	TRACKING_FAIL_REASON_e		ePendingFailReason;
	struct tag_TRACKING_EVENT *pNext;
} TRACKING_EVENT;

typedef struct tag_TRACKING_SVC
{
	HINT32		nSvcUid;
	struct tag_TRACKING_SVC *pNext;
	TRACKING_EVENT *pEventList;
} TRACKING_SVC;

typedef struct
{
	HBOOL				bTrackingInited;
	HUINT32				ulTrackingSemId;
	TRACKING_SVC 		*pstTrackingSvcList;
	PostMsgToSamaCb_t	fnTrackingPostMsg;
} TRACKING_INSTANCE;

//------------------------------------------------------------------------


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
enum
{
	RS_UNDEFINED				= 0x00,	/* 0: undefined	*/
	RS_NOT_RUNNING			= 0x01,	/* 1: not running*/
	RS_FEW_SECONDS			= 0x02,	/* 2: starts in a few seconds	*/
	RS_PAUSING					= 0x03,	/* 3: pausing		*/
	RS_RUNNING					= 0x04,	/* 4: running		*/
	RS_SERVICE_OFF_AIR		= 0x05,	/* 5: service off-air */
	RS_RESERVED				= 0x06,	/* 6-7: rsvd for future use	*/
};


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

STATIC TRACKING_INSTANCE		 s_stTrackingInst;

/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
#define ____DEBUG_LOCAL_FUNC____

STATIC void samalib_tracking_PrintTrackingStatus(TRACKING_INSTANCE *pstInst)
{
	TRACKING_SVC *pTrackingSvc;
	TRACKING_EVENT *pTrackingEvent;
	UNIXTIME sysTime;
	HxDATETIME_Time_t stTime;
	HxDATETIME_Date_t stDate;
	HERROR hError;

	hError = VK_CLOCK_GetTime((unsigned long *)&sysTime);
	if(hError != ERR_OK || UNIX_FIRST_DAY >= sysTime)
	{
		HxLOG_Error("GetTime Failed~~~~\n");
	}


	HLIB_DATETIME_ConvertUnixTimeToDate(sysTime, &stDate);
	HLIB_DATETIME_ConvertUnixTimeToTime(sysTime, &stTime);
	HxLOG_Debug("%s()\n", __FUNCTION__);

	for (pTrackingSvc = pstInst->pstTrackingSvcList; pTrackingSvc; pTrackingSvc = pTrackingSvc->pNext)
	{
		pTrackingEvent = pTrackingSvc->pEventList;
		HxLOG_Debug("  SVC %d\n", pTrackingSvc->nSvcUid);
		for (; pTrackingEvent; pTrackingEvent = pTrackingEvent->pNext)
		{
				HxLOG_Debug("    EVENT 0x%x(%d) (run %d)(pending (%d), slot[%d]\n",
						pTrackingEvent->nEventId,
						pTrackingEvent->nEventId,
						pTrackingEvent->ucRunningStatus,
						pTrackingEvent->isPending,
						pTrackingEvent->slot);
		}
	}

	HxLOG_Debug("(at %d/%d/%d %02d:%02d:%02d)\n",
			stDate.usYear, stDate.ucMonth, stDate.ucDay,
			stTime.ucHour, stTime.ucMinute, stTime.ucSecond);
}



#define ____DEBUG_GLOBAL_FUNC____



#define ____LOCAL_FUNC____
TRACKING_INSTANCE *samalib_tracking_GetInstance (void)
{
	if (TRUE != s_stTrackingInst.bTrackingInited)
	{
		HxLOG_Debug("Initialize the Instance\n");

		SAMA_MemSet(&s_stTrackingInst, 0, sizeof(TRACKING_INSTANCE));

		(void)HxSEMT_Create(&s_stTrackingInst.ulTrackingSemId,"s_ulMWCTrackingSemId", 0);
		HxSEMT_Get(s_stTrackingInst.ulTrackingSemId);
		s_stTrackingInst.bTrackingInited = TRUE;
	}
	else
	{
		HxSEMT_Get(s_stTrackingInst.ulTrackingSemId);
	}

	return &s_stTrackingInst;
}

void samalib_tracking_ReleaseInstance (TRACKING_INSTANCE *pstInst)
{
	if (pstInst != NULL)
	{
		HxSEMT_Release(pstInst->ulTrackingSemId);
	}
}

#define ____GLOBAL_FUNC____


#define	_____LOCAL_TRACKING_FUNC______

STATIC TRACKING_EVENT *	findTrackingEvtBySlotId (TRACKING_INSTANCE *pstInst, HUINT32 ulSlot)
{
	TRACKING_SVC 	*pTrackingSvc = NULL;
	TRACKING_EVENT 	*pTrackingEvent = NULL;
	TRACKING_EVENT	*pFoundEvent = NULL;

	for (pTrackingSvc = pstInst->pstTrackingSvcList; pTrackingSvc; pTrackingSvc = pTrackingSvc->pNext)
	{
		for (pTrackingEvent = pTrackingSvc->pEventList; pTrackingEvent; pTrackingEvent = pTrackingEvent->pNext)
		{
			if (ulSlot == pTrackingEvent->slot)
			{
				pFoundEvent = pTrackingEvent;
				break;
			}
		}
	}
	return pFoundEvent;
}

STATIC TRACKING_SVC *	findTrackingSvc(TRACKING_SVC *pstTrackingSvcList, HINT32 nSvcUid)
{
	TRACKING_SVC *pTrackingSvc = NULL;

	for (pTrackingSvc = pstTrackingSvcList; pTrackingSvc; pTrackingSvc = pTrackingSvc->pNext)
	{
		if (nSvcUid == pTrackingSvc->nSvcUid)
			break;
	}
	return pTrackingSvc;
}

STATIC TRACKING_SVC *allocTrackingSvc (TRACKING_INSTANCE *pstInst)
{
	TRACKING_SVC *pTrackingSvc = (TRACKING_SVC *)SAMA_Malloc(sizeof(TRACKING_SVC));

	if (pTrackingSvc)
	{
		SAMA_MemSet(pTrackingSvc, 0, sizeof(TRACKING_SVC));
		pTrackingSvc->pNext = pstInst->pstTrackingSvcList;
		pstInst->pstTrackingSvcList = pTrackingSvc;
	}

	return pTrackingSvc;
}

STATIC void freeTrackingSvc(TRACKING_INSTANCE *pstInst, TRACKING_SVC *pToBeRemoved)
{
	TRACKING_SVC *pTrackingSvc;

	pTrackingSvc = pstInst->pstTrackingSvcList;
	if (pTrackingSvc == pToBeRemoved)
	{
		pstInst->pstTrackingSvcList = pTrackingSvc->pNext;
	}
	else
	{
		while (pTrackingSvc != NULL && pTrackingSvc->pNext != pToBeRemoved)
		{
			pTrackingSvc = pTrackingSvc->pNext;
		}

		if (pTrackingSvc == NULL)
			return;

		pTrackingSvc->pNext = pToBeRemoved->pNext;
	}

	SAMA_Free(pToBeRemoved);
}

STATIC TRACKING_EVENT *findTrackingEventBySvcUidAndEvtId(TRACKING_SVC *pstTrackingSvcList, HINT32 nSvcUid, HINT32 nEventId)
{
	TRACKING_SVC	*pTrackingSvc = NULL;
	TRACKING_EVENT	*pTrackingEvent = NULL;
	TRACKING_EVENT	*pFoundEvent = NULL;

	for (pTrackingSvc = pstTrackingSvcList; pTrackingSvc; pTrackingSvc = pTrackingSvc->pNext)
	{
		if (nSvcUid != pTrackingSvc->nSvcUid)
			continue;

		pTrackingEvent = pTrackingSvc->pEventList;
		for (; pTrackingEvent; pTrackingEvent = pTrackingEvent->pNext)
		{
			if (nEventId == pTrackingEvent->nEventId)
			{
				pFoundEvent = pTrackingEvent;
				break;
			}
		}
	}

	return pFoundEvent;
}

STATIC TRACKING_EVENT *findTrackingEvent(TRACKING_SVC *pTrackingSvc, HINT32 nEventId)
{
	TRACKING_EVENT *pTrackingEvent = NULL;

	pTrackingEvent = pTrackingSvc->pEventList;
	for (; pTrackingEvent; pTrackingEvent = pTrackingEvent->pNext) {
		if (nEventId == pTrackingEvent->nEventId)
			break;
	}
	return pTrackingEvent;
}

STATIC TRACKING_EVENT *allocTrackingEvent(TRACKING_SVC *pTrackingSvc)
{
	TRACKING_EVENT *pTrackingEvent = (TRACKING_EVENT *)SAMA_Malloc(sizeof(TRACKING_EVENT));

	if (pTrackingEvent)
	{
		VK_memset(pTrackingEvent, 0, sizeof(TRACKING_EVENT));
		pTrackingEvent->pNext = pTrackingSvc->pEventList;
		pTrackingSvc->pEventList = pTrackingEvent;
	}
	return pTrackingEvent;
}

STATIC void freeTrackingEvent(TRACKING_SVC *pTrackingSvc, TRACKING_EVENT *pToBeRemoved)
{
	TRACKING_EVENT *pTrackingEvent;

	pTrackingEvent = pTrackingSvc->pEventList;
	if (pTrackingEvent == pToBeRemoved)
	{
		pTrackingSvc->pEventList = pTrackingEvent->pNext;
	}
	else
	{
		while (pTrackingEvent != NULL && pTrackingEvent->pNext != pToBeRemoved)
		{
			pTrackingEvent = pTrackingEvent->pNext;
		}

		if (pTrackingEvent == NULL)
			return;

		pTrackingEvent->pNext = pToBeRemoved->pNext;
	}

	SAMA_Free(pToBeRemoved);
}

STATIC void samalib_tracking_PostRunningTransition (PostMsgToSamaCb_t fnTrackingPostMsg, TRACKING_EVENT *pTrackingEvent)
{
	UNIXTIME		utCurrTime;

	if ( pTrackingEvent == NULL || fnTrackingPostMsg == NULL )
	{
		HxLOG_Error("invalid param. evt(0x%x), post(0x%x) \n",pTrackingEvent,fnTrackingPostMsg);
		return;
	}

	utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();
	pTrackingEvent->utTransitionTime = utCurrTime;
	pTrackingEvent->ucRunningStatus = RS_RUNNING;
	(void)(*fnTrackingPostMsg)(pTrackingEvent->slot,eSAPI_TIMER_TYPE_Ready,0);

}

STATIC void samalib_tracking_PostEndTransition (PostMsgToSamaCb_t fnTrackingPostMsg, TRACKING_EVENT *pTrackingEvent)
{
	UNIXTIME		utCurrTime;
	UNIXTIME		utTransitionTime;

	if ( pTrackingEvent == NULL || fnTrackingPostMsg == NULL )
	{
		HxLOG_Error("invalid param. evt(0x%x), post(0x%x) \n",pTrackingEvent,fnTrackingPostMsg);
		return;
	}

	/* Rec Time이 60초 미만일 경우에는 End Transition을 무시한다.
	EIT Other에서는 Running으로 전환이 되어 녹화를 시작하려 하는데,
	TP Conflict 등에 의하여 TP를 전환하여야 하는 경우 TP를 전환한 곳에서는
	EIT PF의 F에 존재하여 바로 End Transition이 수신될 수 있음 - ITV, Channel 4계열
	SI Broadcasting이 sync가 맞지 않는 경우의 대비 코드 */
	/* DTG SI CMG 테스트 중 1분 단위의 event가 있어서 이에 대한 지원을 위해 TRANSITION TIME을 60초에서 30초로 변경*/
	/* 레코딩 파일이 생성되는걸 보장해주기 위해 TRANSITION TIME을 30초에서 40초로 변경 */

	utCurrTime= (UNIXTIME)HLIB_STD_GetSystemTime();
	utTransitionTime = pTrackingEvent->utTransitionTime;
	if ((utTransitionTime<utCurrTime) && (utCurrTime < utTransitionTime + SAMA_TVA_TRACKING_TRANSITION_TIME) )
	{
		HxLOG_Warning("End Transition happened in %d seconds. Ignore Stop timer. slot=(%d) transition time(%d) curr time(%d) \n"
			,SAMA_TVA_TRACKING_TRANSITION_TIME,pTrackingEvent->slot,utTransitionTime,utCurrTime);
		return;
	}

	pTrackingEvent->ucRunningStatus = RS_NOT_RUNNING;
	(void)(*fnTrackingPostMsg)(pTrackingEvent->slot,eSAPI_TIMER_TYPE_Stop,0);
}

void samalib_tracking_OnEventReceived(const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	DxEvent_t			*pstEvent;
	DAxIterator_t		iter;

	TRACKING_EVENT		*pTrackingEvent;
	HINT32				nEventId = (HINT32)userdata;
	TRACKING_INSTANCE	*pstInst = samalib_tracking_GetInstance();

	DAPI_InitIterator(&iter, data, size, 0);
	while (DAPI_NextIterator(&iter))
	{
		pstEvent = (DxEvent_t *)DLIB_NewType(iter.name, iter.uid, iter.data, iter.size);
		if (pstEvent == NULL)
		{
			HxLOG_Debug("null event.  \n");
			DLIB_FreeType(iter.name, pstEvent);
			continue;
		}

		HxLOG_Debug("Got event. event name=[%s] eventid=(0x%x) starttime=(%d) runStatus=(%d)\n",
			SAMA_EVENT_GETNAME(pstEvent->name),pstEvent->eventId, pstEvent->startTime, pstEvent->runStatus);

		if (pstInst == NULL)
		{
			HxLOG_Debug("Tracking Inst null. \n");
			DLIB_FreeType(iter.name, pstEvent);
			continue;
		}

		pTrackingEvent = findTrackingEventBySvcUidAndEvtId(pstInst->pstTrackingSvcList, pstEvent->svcuid, nEventId);
		if (pTrackingEvent == NULL )
		{
			HxLOG_Error(" Fail to find the tracking event. eventId=(0x%x). \n",nEventId);
			DLIB_FreeType(iter.name, pstEvent);
			break;
		}
		if ( pstEvent->runStatus == RS_RUNNING )
		{
			HxLOG_Debug(" Tracking event started early. Start Tracking. slot=(%d) \n", pTrackingEvent->slot);
			pTrackingEvent->ucRunningStatus = RS_RUNNING;
			samalib_tracking_PostRunningTransition(pstInst->fnTrackingPostMsg, pTrackingEvent);
		}

		DLIB_FreeType(iter.name, pstEvent);
		break;
	}

	samalib_tracking_ReleaseInstance(pstInst);

	return;
}

void samalib_tracking_StartRecIfAlreadyRunning (TRACKING_INSTANCE *pstInst, HINT32 nSvcUid, HINT32 nEventId)
{
	HUID				eventUid = 0;
	HCHAR				query[256] = {0, };
	HUINT32 			id;

#if defined CONFIG_FUNC_EPG_USE_SVCUID
	eventUid = DxEVENT_UID(nSvcUid, nEventId);
#else
	eventUid = 0; // not implemented
#endif
	HxSTD_PrintToStrN(query, 256
				, "SELECT * FROM %s WHERE uid=%lld" , DxNAMEOF(eDxEPG_TYPE_PF),eventUid);
	HxLOG_Debug("==============================================================================\n");
	HxLOG_Debug("query:\n\t%s\n",query);
	HxLOG_Debug("==============================================================================\n");
	id = DAPI_Query(query, (DAPI_Getter)samalib_tracking_OnEventReceived, (void *)nEventId);
	HxLOG_Warning("DAPI_Query() id: %u \n", id);

}

STATIC HBOOL samalib_tracking_IsTrackingBySlotId (TRACKING_INSTANCE *pstInst, HUINT32 ulSlot)
{
	TRACKING_SVC *pTrackingSvc;
	TRACKING_EVENT *pTrackingEvent;

	for (pTrackingSvc = pstInst->pstTrackingSvcList; pTrackingSvc; pTrackingSvc = pTrackingSvc->pNext)
	{
		pTrackingEvent = pTrackingSvc->pEventList;
		for (; pTrackingEvent; pTrackingEvent = pTrackingEvent->pNext)
		{
			if(pTrackingEvent->slot == ulSlot)
				return TRUE;
		}
	}

	return FALSE;
}

STATIC HBOOL samalib_tracking_IsTrackingBySvcUid (TRACKING_INSTANCE *pstInst, HINT32 nSvcUid)
{
	TRACKING_SVC *pTrackingSvc;

	for (pTrackingSvc = pstInst->pstTrackingSvcList; pTrackingSvc; pTrackingSvc = pTrackingSvc->pNext)
	{
		if ( pTrackingSvc->nSvcUid == nSvcUid)
			return TRUE;
	}

	return FALSE;
}

STATIC HERROR		samalib_tracking_StartTracking (TRACKING_INSTANCE *pstInst, HINT32 nSvcUid, HINT32 nEvtId, HUINT32 ulSlot, UNIXTIME utStartTime)
{
	TRACKING_SVC *pTrackingSvc;
	TRACKING_EVENT *pTrackingEvent;
	HBOOL			new_tracking_svc = FALSE;

	pTrackingSvc = findTrackingSvc(pstInst->pstTrackingSvcList, nSvcUid);
	if (pTrackingSvc == NULL)
	{
		pTrackingSvc = allocTrackingSvc(pstInst);
		if (pTrackingSvc == NULL)
		{
			HxLOG_Debug("[%s:%d] cannot alloc tracking service\n", __FUNCTION__, __LINE__);
			return ERR_FAIL;
		}

		pTrackingSvc->nSvcUid = nSvcUid;
		new_tracking_svc = TRUE;
	}

	pTrackingEvent = findTrackingEvent(pTrackingSvc, nEvtId);
	if (NULL == pTrackingEvent)
	{
		pTrackingEvent = allocTrackingEvent(pTrackingSvc);
		if (pTrackingEvent == NULL)
		{
			HxLOG_Debug("[%s:%d] cannot alloc tracking event\n", __FUNCTION__, __LINE__);
			if (new_tracking_svc)
				freeTrackingSvc(pstInst, pTrackingSvc);
			return ERR_FAIL;
		}

		pTrackingEvent->slot = ulSlot;
		pTrackingEvent->nEventId = nEvtId;
		pTrackingEvent->ucRunningStatus = RS_NOT_RUNNING;
	}

	// 바로 recording start 가능한지 pf check.
	samalib_tracking_StartRecIfAlreadyRunning(pstInst, nSvcUid, nEvtId);

	HxLOG_Trace();
	samalib_tracking_PrintTrackingStatus(pstInst);

	return ERR_OK;
}

STATIC HERROR		samalib_tracking_StopTracking (TRACKING_INSTANCE *pstInst, HINT32 nSvcUid, HINT32 nEventId)
{
	TRACKING_SVC *pTrackingSvc;
	TRACKING_EVENT *pTrackingEvent;

	pTrackingSvc = findTrackingSvc(pstInst->pstTrackingSvcList, nSvcUid);
	if (NULL == pTrackingSvc)
		return ERR_FAIL;

	pTrackingEvent = findTrackingEvent(pTrackingSvc, nEventId);
	if (NULL == pTrackingEvent)
		return ERR_FAIL;

	freeTrackingEvent(pTrackingSvc, pTrackingEvent);
	if (pTrackingSvc->pEventList == NULL)
	{
		freeTrackingSvc(pstInst, pTrackingSvc);
	}

	HxLOG_Trace();
	samalib_tracking_PrintTrackingStatus(pstInst);

	return ERR_OK;
}

STATIC HERROR		samalib_tracking_TrackEvent (TRACKING_INSTANCE *pstInst, DxEvent_t *pstEvent)
{
	TRACKING_SVC		*pTrackingSvc;
	TRACKING_EVENT		*pTrackingEvent;

	pTrackingSvc = findTrackingSvc(pstInst->pstTrackingSvcList, pstEvent->svcuid);
	if (pTrackingSvc == NULL)
	{
		HxLOG_Debug("Fail to find tracking svc. received svcuid=(%d) name=[%s] evtid=(0x%x)\n",pstEvent->svcuid,SAMA_EVENT_GETNAME(pstEvent->name), pstEvent->eventId);
		return ERR_FAIL;
	}

	/* check if currently monitoring event is over. */
	pTrackingEvent = pTrackingSvc->pEventList;
	for (; pTrackingEvent; pTrackingEvent = pTrackingEvent->pNext)
	{
 		if ( pTrackingEvent->nEventId != pstEvent->eventId
 			&& pstEvent->runStatus == RS_RUNNING
 			&& pTrackingEvent->ucRunningStatus == RS_RUNNING)
		{
			HxLOG_Debug("Tracking Event's NEXT event started. Stop tracking. slot=(%d) \n", pTrackingEvent->slot);
			samalib_tracking_PostEndTransition(pstInst->fnTrackingPostMsg, pTrackingEvent);
		}
	}

	pTrackingEvent = findTrackingEvent(pTrackingSvc, pstEvent->eventId);
	if (pTrackingEvent == NULL)
	{
		HxLOG_Debug("Not the tracking evt. evtid=(0x%x) name=[%s] runStatus=(%d) \n",pstEvent->eventId, SAMA_EVENT_GETNAME(pstEvent->name), pstEvent->runStatus);
		return ERR_FAIL;
	}

	if (pTrackingEvent->ucRunningStatus != pstEvent->runStatus)
	{
		if (pstEvent->runStatus == RS_NOT_RUNNING )
		{
			HxLOG_Debug("Tracking event stopped. Stop tracking. slot=(%d) \n", pTrackingEvent->slot);
			samalib_tracking_PostEndTransition(pstInst->fnTrackingPostMsg, pTrackingEvent);
		}
		else if (pstEvent->runStatus == RS_RUNNING)
		{
			HxLOG_Debug("Tracking event started. Start Tracking. slot=(%d) \n",pTrackingEvent->slot);
			samalib_tracking_PostRunningTransition(pstInst->fnTrackingPostMsg, pTrackingEvent);
		}

	}

	return ERR_OK;
}

STATIC HERROR samalib_tracking_RestartPendingSchedules (TRACKING_INSTANCE *pstInst)
{
	TRACKING_SVC	*pTrackingSvc = NULL;
	TRACKING_EVENT	*pTrackingEvent = NULL;

	for (pTrackingSvc = pstInst->pstTrackingSvcList; pTrackingSvc; pTrackingSvc = pTrackingSvc->pNext)
	{
		pTrackingEvent = pTrackingSvc->pEventList;
		for (; pTrackingEvent; pTrackingEvent = pTrackingEvent->pNext)
		{
			if (pTrackingEvent->isPending == TRUE)
			{
				HxLOG_Debug("Restart Pending schedule. slot=(%d) \n", pTrackingEvent->slot);
				pTrackingEvent->isPending = FALSE;
				samalib_tracking_StartRecIfAlreadyRunning(pstInst, pTrackingSvc->nSvcUid, pTrackingEvent->nEventId);
			}
		}
	}

	return ERR_OK;
}

#define	_____GLOBAL_TRACKING_FUNC______

void SAMALIB_TRACKING_PrintTrackingStatus(void)
{
	TRACKING_INSTANCE	*pstInst = samalib_tracking_GetInstance();

	TRACKING_GOTO_IF(pstInst == NULL, END_FUNC);

	samalib_tracking_PrintTrackingStatus(pstInst);

END_FUNC:
	samalib_tracking_ReleaseInstance(pstInst);
}

HERROR		SAMALIB_TRACKING_RegisterNotifier (PostMsgToSamaCb_t fnPostFunc)
{
	TRACKING_INSTANCE	*pstInst = samalib_tracking_GetInstance();

	TRACKING_GOTO_IF(pstInst == NULL, END_FUNC);

	pstInst->fnTrackingPostMsg = fnPostFunc;

END_FUNC:
	samalib_tracking_ReleaseInstance(pstInst);

	return ERR_OK;
}

HERROR		SAMALIB_TRACKING_StartTracking(HINT32 nSvcUid, HINT32 nEventId, HUINT32 ulSlot,UNIXTIME	utStartTime)
{
	HERROR				hError = ERR_FAIL;
	TRACKING_INSTANCE	*pstInst = samalib_tracking_GetInstance();

	TRACKING_GOTO_IF(pstInst == NULL, END_FUNC);

	hError = samalib_tracking_StartTracking(pstInst,nSvcUid, nEventId, ulSlot, utStartTime);

END_FUNC:
	samalib_tracking_ReleaseInstance(pstInst);

	return hError;
}

HERROR		SAMALIB_TRACKING_StopTracking(HINT32 nSvcUid, HINT32 nEventId)
{
	HERROR				hError = ERR_FAIL;
	TRACKING_INSTANCE	*pstInst = samalib_tracking_GetInstance();

	TRACKING_GOTO_IF(pstInst == NULL, END_FUNC);

	hError = samalib_tracking_StopTracking(pstInst,nSvcUid, nEventId);

END_FUNC:
	samalib_tracking_ReleaseInstance(pstInst);

	return hError;
}

HERROR		SAMALIB_TRACKING_TrackEvent (DxEvent_t	*pstEvent)
{
	HERROR				hError = ERR_FAIL;
	TRACKING_INSTANCE	*pstInst = samalib_tracking_GetInstance();

	TRACKING_GOTO_IF(pstInst == NULL, END_FUNC);

	hError = samalib_tracking_TrackEvent(pstInst,pstEvent);

END_FUNC:
	samalib_tracking_ReleaseInstance(pstInst);

	return hError;
}

HBOOL		SAMALIB_TRACKING_IsTrackingBySlotId(HUINT32 ulSlot)
{
	HERROR				hError = ERR_FAIL;
	TRACKING_INSTANCE	*pstInst = samalib_tracking_GetInstance();

	TRACKING_GOTO_IF(pstInst == NULL, END_FUNC);

	hError = samalib_tracking_IsTrackingBySlotId(pstInst,ulSlot);

END_FUNC:
	samalib_tracking_ReleaseInstance(pstInst);

	return hError;
}

HBOOL	SAMALIB_TRACKING_IsTrackingBySvcUid (HINT32 nSvcUid)
{
	HERROR				hError = ERR_FAIL;
	TRACKING_INSTANCE	*pstInst = samalib_tracking_GetInstance();

	TRACKING_GOTO_IF(pstInst == NULL, END_FUNC);

	hError = samalib_tracking_IsTrackingBySvcUid(pstInst,nSvcUid);

END_FUNC:
	samalib_tracking_ReleaseInstance(pstInst);

	return hError;
}

UNIXTIME		SAMALIB_TRACKING_GetTransitionTimeBySlotId (HUINT32 ulSlot)
{
	TRACKING_EVENT 		*pTrackingEvent = NULL;
	UNIXTIME			utTransitionTime = 0;
	TRACKING_INSTANCE	*pstInst = samalib_tracking_GetInstance();

	TRACKING_GOTO_IF(pstInst == NULL, END_FUNC);

	pTrackingEvent = findTrackingEvtBySlotId(pstInst,ulSlot);
	if (pTrackingEvent == NULL )
	{
		HxLOG_Debug("Fail to get tracking event. slot=(%d)\n");
		utTransitionTime = 0;
	}
	else
	{
		utTransitionTime = pTrackingEvent->utTransitionTime;
	}

END_FUNC:
	samalib_tracking_ReleaseInstance(pstInst);

	return utTransitionTime;
}

HERROR		SAMALIB_TRACKING_GetRunningStatus (HUINT32 ulSlot, HUINT8 *pucRunningStatus)
{
	TRACKING_EVENT 		*pTrackingEvent = NULL;
	HERROR				hResult = ERR_FAIL;
	TRACKING_INSTANCE	*pstInst = samalib_tracking_GetInstance();

	TRACKING_GOTO_IF(pstInst == NULL, END_FUNC);
	pTrackingEvent = findTrackingEvtBySlotId(pstInst,ulSlot);
	if (pTrackingEvent == NULL )
	{
		HxLOG_Debug("Fail to get tracking event. slot=(%d)\n");
		hResult = ERR_FAIL;
	}
	else
	{
		*pucRunningStatus = pTrackingEvent->ucRunningStatus;
		hResult = ERR_OK;

	}

END_FUNC:
	samalib_tracking_ReleaseInstance(pstInst);

	return hResult;
}

HERROR		SAMALIB_TRACKING_GetPendingReason (HUINT32 ulSlot, TRACKING_FAIL_REASON_e *pePendingReason)
{
	TRACKING_EVENT 		*pTrackingEvent = NULL;
	HERROR				hErr = ERR_FAIL;
	TRACKING_INSTANCE	*pstInst = samalib_tracking_GetInstance();

	TRACKING_GOTO_IF(pstInst == NULL, END_FUNC);

	pTrackingEvent = findTrackingEvtBySlotId(pstInst,ulSlot);
	if (pTrackingEvent == NULL )
	{
		HxLOG_Debug("Fail to get tracking event. slot=(%d)\n");
		hErr = ERR_FAIL;
	}
	else
	{
		*pePendingReason = pTrackingEvent->ePendingFailReason;
		hErr = ERR_OK;
	}

END_FUNC:
	samalib_tracking_ReleaseInstance(pstInst);

	return hErr;
}

HERROR		SAMALIB_TRACKING_GetPending (HUINT32 ulSlot, HBOOL *isPending)
{
	TRACKING_EVENT 		*pTrackingEvent = NULL;
	HERROR				hErr = ERR_FAIL;
	TRACKING_INSTANCE	*pstInst = samalib_tracking_GetInstance();

	TRACKING_GOTO_IF(pstInst == NULL, END_FUNC);

	pTrackingEvent = findTrackingEvtBySlotId(pstInst,ulSlot);
	if (pTrackingEvent == NULL )
	{
		HxLOG_Debug("Fail to get tracking event. slot=(%d)\n");
		hErr = ERR_FAIL;
	}
	else
	{
		*isPending = pTrackingEvent->isPending;
		hErr = ERR_OK;
	}

END_FUNC:
	samalib_tracking_ReleaseInstance(pstInst);

	return hErr;
}

HERROR		SAMALIB_TRACKING_SetPending (HUINT32 ulSlot, HBOOL isPending, TRACKING_FAIL_REASON_e eFailReason)
{
	TRACKING_EVENT 		*pTrackingEvent = NULL;
	HERROR				hErr = ERR_FAIL;
	TRACKING_INSTANCE	*pstInst = samalib_tracking_GetInstance();

	TRACKING_GOTO_IF(pstInst == NULL, END_FUNC);

	pTrackingEvent = findTrackingEvtBySlotId(pstInst,ulSlot);
	if (pTrackingEvent == NULL )
	{
		HxLOG_Debug("Fail to get tracking event. slot=(%d)\n");
		hErr = ERR_FAIL;
	}
	else
	{
		pTrackingEvent->isPending = isPending;
		pTrackingEvent->ePendingFailReason = eFailReason;
		hErr = ERR_OK;
	}

END_FUNC:
	samalib_tracking_ReleaseInstance(pstInst);

	return hErr;
}

HERROR		SAMALIB_TRACKING_RestartPendingSchedules (void)
{
	HERROR				hErr = ERR_FAIL;
	TRACKING_INSTANCE	*pstInst = samalib_tracking_GetInstance();

	TRACKING_GOTO_IF(pstInst == NULL, END_FUNC);

	hErr = samalib_tracking_RestartPendingSchedules(pstInst);
	if ( hErr != ERR_OK )
	{
		HxLOG_Debug("restart pending schedules failed. \n");
	}

END_FUNC:
	samalib_tracking_ReleaseInstance(pstInst);

	return hErr;
}

/* end of file */

