/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  dama_thread.c
	@brief

	Description:  									\n
	Module: DAMA / DAMA Multi-Thread				\n

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

#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <pthread.h>

#include <vkernel.h>

#include <hlib.h>

#include <hapi.h>
#include <papi.h>
#include <uapi.h>
#include <dlib.h>
#include "dama_int.h"
#include "dama_db.h"
#include "dama_server.h"

#if defined(CONFIG_HUMAX_CRASHREPORT)
#include "hcrlib.h"
#endif
#ifdef	CONFIG_DEBUG
#include "dama_debug.c"
#endif

#ifdef CONFIG_DEBUG
//#define DEBUG_MT_INST_CMD_HISTORY
#endif

#if defined(CONFIG_DEBUG)
#define LOG_INST_POS(pstInst)			do { dama_mt_LogPosition(pstInst, __FUNCTION__, __LINE__); } while (0)
#else
#define LOG_INST_POS(pstInst)
#endif

#define ___INTERNAL_DEFINITIONS___
#define	DAMA_MT_TEMP_BUF					3
#define	DAMA_MT_TEMP_STREAM					3
#define	DAMA_MT_BUFFER_THRESHOLD			(1024 * 1024 * 2)
#define	DAMA_MT_CHECK_TIMER_CYCLE			1000	// ms
#define	DAMA_MT_SYNC_CYCLE					2000	// ms
#define	DAMA_MT_NOTIFICATION_CYCLE			1000	// ms
#define	DAMA_MT_NOTIFICATION_TERM			1000	// ms
#define	DAMA_MT_REST_IN_PEACE				10000	// ms
#define	DAMA_MT_THRESHOLD_FOR_SYNC_CMD		5000	// ms

#define DAMA_MT_EPG_PREFIX					"eDxEPG_TYPE_"

#if defined(CONFIG_DEBUG)
#define DAMA_MT_HISTORY_FOR_QUEUE_INCREASE			128
#endif

#define ___INTERNAL_DATATYPES___
typedef enum
{
	eDAMA_MT_EXTJOB_None			= 0x0000,

	eDAMA_MT_EXTJOB_Sync			= 0x0001,
	eDAMA_MT_EXTJOB_Notify			= 0x0002,
	eDAMA_MT_EXTJOB_CheckThreshold	= 0x0004,
	eDAMA_MT_EXTJOB_Shutdown		= 0x0008,
	eDAMA_MT_EXTJOB_ShutdownDone	= 0x0010,

	eDAMA_MT_EXTJOB_EndOfCase		= 0xFFFF
} dama_mt_ExtraJobType_e;

#if defined(DEBUG_MT_INST_CMD_HISTORY)
typedef enum
{
	eDAMA_MT_HISTORY_None			= 0,

	eDAMA_MT_HISTORY_Push			= 0x01000000,
	eDAMA_MT_HISTORY_Pop			= 0x02000000,

	eDAMA_MT_HISTORY_MASK			= 0xFF000000
} dama_mt_HistoryType_e;

typedef struct
{
	dama_mt_HistoryType_e	 eType;

	DAMA_Cmd_e				 eCmd;
	HUINT32					 ulTick;
	HUINT32					 ulDataSize;
} dama_mt_History_t;
#endif

typedef struct
{
	DAMA_DB_t		*pstDb;
	DAMA_Table_t	*pstTable;

	HxList_t		*pstDataTypeList;

	unsigned long	 ulTaskId;
	unsigned long	 ulTimerId;
	unsigned long	 ulSemId;

	// For Message Queueing:
	HxPriorityQueue_t	*pstMsgQueue;
	pthread_mutex_t		 msgQueueMutex;
    pthread_cond_t       msgQueueNotEmpty;
	HINT32				 nReadCount;

	// For Temp Buffer
	struct
	{
		HINT32	rwstream[DAMA_MT_TEMP_STREAM];
		HINT32	index;
	} tempstream;

	struct
	{
		void *	buf[DAMA_MT_TEMP_BUF];
		HUINT32	size[DAMA_MT_TEMP_BUF];
		HINT32	index;
	} tempbuf;

	// Observer
	HxHASH_t	*observer;

	// Need to access with semaphore
	HBOOL		 bActive;
	HUINT32		 ulMsgNumInQueue;
	HINT32		 nMsgTotalSizeInQueue;
	dama_mt_ExtraJobType_e	 eExtraJob;

	// Used only in the threads:
	HUINT32		 ulContinuousShortSync;
	HUINT32		 ulLastForcedSyncTick;

#if defined(CONFIG_DEBUG)
	// Last Time
	HUINT32		 ulLastReqPushedTick;
	HUINT32		 ulLastReqPoppedTick;
	const HCHAR	*szLastFuncName;
	HUINT32		 ulLastFuncLine;
	HUINT32		 ulLastLogTick;

	// for Burst-time Average:
	HUINT64		 ullBurstReqPushedTick;
	HUINT64		 ullBurstReqPushedNum;
	HUINT64		 ullBurstReqPoppedTick;
	HUINT64		 ullBurstReqPoppedNum;

	// Total Number
	HUINT32		 ulTotalReqPushedNum;
	HUINT32		 ulTotalReqPoppedNum;

	// When the queue is increasing, not decreasing...
#if defined(DEBUG_MT_INST_CMD_HISTORY)
	HBOOL		 bRecordHistory;
	HUINT32		 ulHistoryPos;
	dama_mt_History_t	*pstQueueHistory;
#endif

	// CMD Process Time:
	HUINT64		 ullTotalCmdProcessTick;
	HUINT64		 ullTotalCmdProcessNum;
	DAMA_Cmd_e	 eLastCmd;
	HUINT32		 ulLastCmdDataSize;
	HUINT32		 ulLastCmdProcessTick;
	HUINT32		 ulMaxCmdProcessTick;
	HUINT32		 ulMinCmdProcessTick;
#endif
} dama_mt_Instance_t;

typedef struct
{
	DAMA_Request_t	*pstReq;
	HERROR			 hRetValue;

	HBOOL			 bSentAll;
} dama_mt_ReqItem_t;

typedef struct
{
	dama_mt_Instance_t	*pstInst;

	HUINT32				 ulArg1;
	HUINT32				 ulArg2;
	HUINT32				 ulArg3;
	void				*pvArgPtr1;
	void				*pvArgPtr2;
	void				*pvArgPtr3;
} dama_mt_Check_t;



#define ___INTERNAL_VARIABLES___
STATIC HxList_t		*s_pstDamaMt_InstList = NULL;

#define ___INTERNAL_FUNCTIONS___
STATIC void		dama_mt_processOneItem (dama_mt_Instance_t *pstInst, HINT32 client_handle, HUINT32 request_id, void *data, HUINT32 size, DAMA_Header_t *header);


#define RETURN_ENUM_STR(a)		case a :  return #a;

STATIC const HCHAR *dama_mt_getStringOfCmdEnum (DAMA_Cmd_e eCmd)
{
	switch (eCmd)
	{
		RETURN_ENUM_STR(eDAMA_CMD_SET);
		RETURN_ENUM_STR(eDAMA_CMD_REQUEST);
		RETURN_ENUM_STR(eDAMA_CMD_RESPONSE);
		RETURN_ENUM_STR(eDAMA_CMD_ADD_NOTIFIER);
		RETURN_ENUM_STR(eDAMA_CMD_REMOVE_NOTIFIER);
		RETURN_ENUM_STR(eDAMA_CMD_ALLOW_NOTIFY_MYSELF);
		RETURN_ENUM_STR(eDAMA_CMD_NOTIFICATION);
		RETURN_ENUM_STR(eDAMA_CMD_SYNC);
		RETURN_ENUM_STR(eDAMA_CMD_RESTORE);
		RETURN_ENUM_STR(eDAMA_CMD_BATCH);
		RETURN_ENUM_STR(eDAMA_CMD_FACTORY_RESET);
		RETURN_ENUM_STR(eDAMA_CMD_SERVER_SYNC);
		RETURN_ENUM_STR(eDAMA_CMD_EXIT);
		RETURN_ENUM_STR(eDAMA_CMD_MONITOR);
#if defined(CONFIG_FUNC_USE_DAPI_DIRECT_QUERY)
		RETURN_ENUM_STR(eDAMA_CMD_DATACHANGED);
#endif
#if defined(CONFIG_DAMA_BACKGROUND_SYNC)
		RETURN_ENUM_STR(eDAMA_CMD_SYNCFINISHED);
#endif
#if defined(DAMA_CHECK_STORAGE_STATUS)
		RETURN_ENUM_STR(eDAMA_CMD_STORAGE_INSERTED);
		RETURN_ENUM_STR(eDAMA_CMD_STORAGE_REMOVED);
		RETURN_ENUM_STR(eDAMA_CMD_STORAGE_UNMOUNTED);
#endif
#if defined(CONFIG_DAMA_MULTITHREAD)
		RETURN_ENUM_STR(eDAMA_CMD_NOTIFY_TO_OBSERVERS);
#endif
	default:
		break;
	}

	return "Unknown";
}

STATIC HINT32 dama_mt_cmpRequestPriority (const DAMA_Request_t *lhs, const DAMA_Request_t *rhs)
{
	return lhs->prior - rhs->prior;
}

#define _____MANAGING_INSTANCE_____
STATIC HERROR dama_mt_registerInstance (dama_mt_Instance_t *pstInst)
{
	s_pstDamaMt_InstList = HLIB_LIST_Append(s_pstDamaMt_InstList, (void *)pstInst);
	return ERR_OK;
}

STATIC dama_mt_Instance_t *dama_mt_getInstance (HCHAR *szDataType)
{
	HxList_t		*pstListItem;

	if ((NULL != szDataType) && ('\0' != *szDataType))
	{
		for (pstListItem = s_pstDamaMt_InstList; NULL != pstListItem; pstListItem = HLIB_LIST_Next(pstListItem))
		{
			dama_mt_Instance_t		*pstInst = (dama_mt_Instance_t *)HLIB_LIST_Data(pstListItem);

			if (NULL != pstInst)
			{
				HxList_t		*pstNameList;

				for (pstNameList = pstInst->pstDataTypeList; NULL != pstNameList; pstNameList = HLIB_LIST_Next(pstNameList))
				{
					HCHAR			*pszName = (HCHAR *)HLIB_LIST_Data(pstNameList);

					if ((NULL != pszName) && (HxSTD_StrCmp(szDataType, pszName) == 0))
					{
						return pstInst;
					}
				}
			}
		}
	}

	return NULL;
}

STATIC HCHAR *dama_mt_getInstanceName (dama_mt_Instance_t *pstInst)
{
	if (NULL != pstInst)
	{
		HxList_t		*pstNameList;

		for (pstNameList = pstInst->pstDataTypeList; NULL != pstNameList; pstNameList = HLIB_LIST_Next(pstNameList))
		{
			HCHAR			*pszName = (HCHAR *)HLIB_LIST_Data(pstNameList);

			if (NULL != pszName)
			{
				return pszName;
			}
		}
	}

	return (HCHAR *)"Unknown";
}

STATIC dama_mt_ExtraJobType_e dama_mt_getExtraJobFlags (dama_mt_Instance_t *pstInst)
{
	dama_mt_ExtraJobType_e		 eExtraJob = eDAMA_MT_EXTJOB_None;

	if (NULL != pstInst)
	{
		VK_SEM_Get(pstInst->ulSemId);
		eExtraJob = pstInst->eExtraJob;
		VK_SEM_Release(pstInst->ulSemId);
	}

	return eExtraJob;
}

STATIC HERROR dama_mt_setExtraJobFlags (dama_mt_Instance_t *pstInst, dama_mt_ExtraJobType_e eFlags)
{
	if (NULL == pstInst)
	{
		return ERR_FAIL;
	}

	VK_SEM_Get(pstInst->ulSemId);
	pstInst->eExtraJob |= eFlags;
	VK_SEM_Release(pstInst->ulSemId);

	return ERR_OK;
}

STATIC HERROR dama_mt_resetExtraJobFlags (dama_mt_Instance_t *pstInst, dama_mt_ExtraJobType_e eFlags)
{
	if (NULL == pstInst)
	{
		return ERR_FAIL;
	}

	VK_SEM_Get(pstInst->ulSemId);
	pstInst->eExtraJob &= ~eFlags;
	VK_SEM_Release(pstInst->ulSemId);

	return ERR_OK;
}

#if defined(CONFIG_DEBUG)
STATIC void dama_mt_LogPosition (dama_mt_Instance_t *pstInst, const HCHAR *pszFuncName, HUINT32 ulFuncLine)
{
	pstInst->szLastFuncName = pszFuncName;
	pstInst->ulLastFuncLine = ulFuncLine;
	pstInst->ulLastLogTick = HLIB_STD_GetSystemTick();
}

STATIC const HCHAR *dama_mt_GetCmdString (DAMA_Cmd_e eCmd)
{
	switch (eCmd)
	{
	case eDAMA_CMD_SET:						return "eDAMA_CMD_SET";
	case eDAMA_CMD_REQUEST:					return "eDAMA_CMD_REQUEST";
	case eDAMA_CMD_RESPONSE:				return "eDAMA_CMD_RESPONSE";
	case eDAMA_CMD_ADD_NOTIFIER:			return "eDAMA_CMD_ADD_NOTIFIER";
	case eDAMA_CMD_REMOVE_NOTIFIER:			return "eDAMA_CMD_REMOVE_NOTIFIER";
	case eDAMA_CMD_ALLOW_NOTIFY_MYSELF:		return "eDAMA_CMD_ALLOW_NOTIFY_MYSELF";
	case eDAMA_CMD_NOTIFICATION:			return "eDAMA_CMD_NOTIFICATION";
	case eDAMA_CMD_SYNC:					return "eDAMA_CMD_SYNC";
	case eDAMA_CMD_RESTORE:					return "eDAMA_CMD_RESTORE";
	case eDAMA_CMD_BATCH:					return "eDAMA_CMD_BATCH";
	case eDAMA_CMD_FACTORY_RESET:			return "eDAMA_CMD_FACTORY_RESET";
	default:
		break;
	}

	return "UNKNOWN";
}

#if defined(DEBUG_MT_INST_CMD_HISTORY)
STATIC void dama_mt_recordQueueHistory (dama_mt_Instance_t *pstInst, dama_mt_HistoryType_e eHistoryType, DAMA_Cmd_e eCmd, HUINT32 ulDataSize)
{
	dama_mt_History_t		*pstHistoryItem;

	if (NULL == pstInst->pstQueueHistory)
	{
		pstInst->pstQueueHistory = (dama_mt_History_t *)HLIB_STD_MemCalloc(sizeof(dama_mt_History_t) * DAMA_MT_HISTORY_FOR_QUEUE_INCREASE);
		if (NULL == pstInst->pstQueueHistory)
		{
			return;
		}

		pstInst->ulHistoryPos = 0;
	}

	pstHistoryItem = &(pstInst->pstQueueHistory[pstInst->ulHistoryPos]);
	pstHistoryItem->eType = eHistoryType;
	pstHistoryItem->eCmd = eCmd;
	pstHistoryItem->ulTick = HLIB_STD_GetSystemTick();
	pstHistoryItem->ulDataSize = ulDataSize;

	pstInst->ulHistoryPos++;
	if (pstInst->ulHistoryPos >= DAMA_MT_HISTORY_FOR_QUEUE_INCREASE)
	{
		pstInst->ulHistoryPos = 0;
	}
}

STATIC void dama_mt_clearQueueHistory (dama_mt_Instance_t *pstInst)
{
	if (NULL != pstInst->pstQueueHistory)
	{
		HLIB_STD_MemFree(pstInst->pstQueueHistory);
		pstInst->pstQueueHistory = NULL;
	}
}
#endif
#endif

#define _____MESSAGE_QUEUE_FUNCTION_____

STATIC HERROR dama_mt_createMsgQueue (dama_mt_Instance_t *pstInst)
{
	HxPriorityQueue_t	*pstQueue = NULL;

	pstQueue = HLIB_PRIORITYQUEUE_New((HxCompareFunc_t)dama_mt_cmpRequestPriority, HLIB_STD_MemFree_CB);
	if (NULL == pstQueue)
	{
		HxLOG_Error("HLIB_PRIORITYQUEUE_New err:\n");
		goto ERROR;
	}

	pstInst->pstMsgQueue = pstQueue;
    pthread_mutex_init(&(pstInst->msgQueueMutex), NULL);
    pthread_cond_init(&(pstInst->msgQueueNotEmpty), NULL);
	pstInst->nReadCount = 0;

	return ERR_OK;

ERROR:
	if (NULL != pstQueue)
	{
		HLIB_PRIORITYQUEUE_Delete(pstQueue);
	}

	return ERR_FAIL;
}

STATIC HERROR dama_mt_destroyMsgQueue (dama_mt_Instance_t *pstInst)
{
	if (NULL != pstInst->pstMsgQueue)
	{
		HLIB_PRIORITYQUEUE_Delete(pstInst->pstMsgQueue);
		pstInst->pstMsgQueue = NULL;
	}

	return ERR_OK;
}

STATIC DAMA_Request_t *dama_mt_duplicateRequest (DAMA_Request_t *pstReq)
{

	if (NULL != pstReq)
	{
		HUINT32				 ulSize = sizeof(DAMA_Request_t) + pstReq->size;
		HUINT8				*pucBuffer;
		DAMA_Request_t		*pstDupReq;

		pucBuffer = (HUINT8 *)HLIB_STD_MemAlloc(ulSize);
		if (NULL != pucBuffer)
		{
			pstDupReq = (DAMA_Request_t *)pucBuffer;

			HxSTD_MemCopy(pucBuffer, (void *)pstReq, ulSize);
#if defined(CONFIG_DEBUG)
			pstDupReq->ulMaskValue = 0x7852;
#endif
			pstDupReq->data  = (void *)(pucBuffer + sizeof(DAMA_Request_t));

			return pstDupReq;
		}
	}

	return NULL;
}

STATIC HERROR dama_mt_pushRequest (dama_mt_Instance_t *pstInst, DAMA_Request_t *pstReq)
{
	DAMA_Request_t		*pstDupReq = NULL;
	HBOOL				 bPush = FALSE;

	if ((NULL == pstInst) || (NULL == pstInst->pstMsgQueue) || (NULL == pstReq))
	{
		return ERR_FAIL;
	}

	pstDupReq = dama_mt_duplicateRequest(pstReq);
	if (NULL == pstDupReq)
	{
		return ERR_FAIL;
	}

#if defined(CONFIG_DEBUG)
{
	HUINT32			 ulCurrTick = HLIB_STD_GetSystemTick();
	HUINT32			 ulDuration = ulCurrTick - pstInst->ulLastReqPushedTick;

	// Burst mode...
	if (ulDuration < 1000)
	{
		pstInst->ullBurstReqPushedTick += (HUINT64)ulDuration;
		pstInst->ullBurstReqPushedNum ++;
	}

#if defined(DEBUG_MT_INST_CMD_HISTORY)
	// If the queue history shall be recorded....
	if (TRUE == pstInst->bRecordHistory)
	{
		HUINT32			 ulDataSize = pstReq->size - sizeof(DAMA_Header_t);
		DAMA_Header_t	*pstHeader = (DAMA_Header_t *)(pstReq->data);

		if (NULL != pstHeader)
		{
			dama_mt_recordQueueHistory(pstInst, eDAMA_MT_HISTORY_Push, pstHeader->cmd, ulDataSize);
		}
	}
#endif

	// Last pushed time, total number
	pstInst->ulLastReqPushedTick = HLIB_STD_GetSystemTick();
	pstInst->ulTotalReqPushedNum ++;
}
#endif

	(void)pthread_mutex_lock(&(pstInst->msgQueueMutex));

	bPush = HLIB_PRIORITYQUEUE_Push(pstInst->pstMsgQueue, pstDupReq);
	if (TRUE == bPush)
	{
#if defined(CONFIG_DEBUG)
		pstDupReq->pvQueuePnt = (void *)pstInst->pstMsgQueue;
		pstDupReq->pvInstPnt = (void *)pstInst;
#endif
		VK_SEM_Get(pstInst->ulSemId);
		pstInst->ulMsgNumInQueue++;
		pstInst->nMsgTotalSizeInQueue += (HINT32)pstDupReq->size;
		VK_SEM_Release(pstInst->ulSemId);

		if (pstInst->nReadCount > 0)
		{
			pthread_cond_signal(&(pstInst->msgQueueNotEmpty));
		}
	}
	else
	{
		HxLOG_Error("HLIB_PRIORITYQUEUE_Push(pstInst->pstMsgQueue, pstDupReq) failed!!!\n");
		HLIB_STD_MemFree(pstDupReq);
	}

	(void)pthread_mutex_unlock(&(pstInst->msgQueueMutex));

	return (TRUE == bPush) ? ERR_OK : ERR_FAIL;
}

STATIC HERROR dama_mt_wakeUpThreadFromWaiting (dama_mt_Instance_t *pstInst)
{
	DAMA_Request_t		*pstDupReq = NULL;
	HERROR				 hResult = ERR_FAIL;

	if ((NULL == pstInst) || (NULL == pstInst->pstMsgQueue))
	{
		return ERR_FAIL;
	}

	(void)pthread_mutex_lock(&(pstInst->msgQueueMutex));
	{
		HBOOL			 bThreadWaiting = (pstInst->nReadCount > 0) ? TRUE : FALSE;
		HBOOL			 bQueueEmpty = (HLIB_PRIORITYQUEUE_Length(pstInst->pstMsgQueue) == 0) ? TRUE : FALSE;

		if ((TRUE == bThreadWaiting) || (TRUE == bQueueEmpty))
		{
			DAMA_Request_t		 stDummyReq;

			HxSTD_MemSet(&stDummyReq, 0, sizeof(DAMA_Request_t));
			stDummyReq.forDummy = TRUE;

			pstDupReq = dama_mt_duplicateRequest(&stDummyReq);
			if (NULL != pstDupReq)
			{
				HBOOL				 bPush;

				bPush = HLIB_PRIORITYQUEUE_Push(pstInst->pstMsgQueue, pstDupReq);
				if (TRUE == bPush)
				{
#if defined(CONFIG_DEBUG)
					pstDupReq->pvQueuePnt = (void *)pstInst->pstMsgQueue;
					pstDupReq->pvInstPnt = (void *)pstInst;
#endif
					VK_SEM_Get(pstInst->ulSemId);
					pstInst->ulMsgNumInQueue++;
					VK_SEM_Release(pstInst->ulSemId);

					if (TRUE == bThreadWaiting)
					{
						pthread_cond_signal(&(pstInst->msgQueueNotEmpty));
					}

					pstDupReq = NULL;
					hResult = ERR_OK;
				}
				else
				{
					HxLOG_Error("HLIB_PRIORITYQUEUE_Push(pstInst->pstMsgQueue, pstDupReq) err..\n");
				}
			}
		}
		else
		{
			hResult = ERR_OK;
		}
	}
	(void)pthread_mutex_unlock(&(pstInst->msgQueueMutex));

	if (NULL != pstDupReq)
	{
		HLIB_STD_MemFree(pstDupReq);
	}

	return hResult;
}

STATIC HERROR dama_mt_popRequest (dama_mt_Instance_t *pstInst, DAMA_Request_t **ppstReq)
{
	HERROR				 hErr, hResult = ERR_FAIL;
	DAMA_Request_t		*pstReq;
#if defined(CONFIG_DEBUG)
	HBOOL				 bBurst = TRUE;
#endif

	if ((NULL == pstInst) || (NULL == pstInst->pstMsgQueue) || (NULL == ppstReq))
	{
		return ERR_FAIL;
	}

	(void)pthread_mutex_lock(&(pstInst->msgQueueMutex));

	pstInst->nReadCount++;

	if ((HLIB_PRIORITYQUEUE_Length(pstInst->pstMsgQueue) == 0) || (pstInst->nReadCount > 1))
	{
#if defined(CONFIG_DEBUG)
		bBurst = FALSE;
#endif

		(void)VK_SEM_Get(pstInst->ulSemId);
		pstInst->bActive = FALSE;
		(void)VK_SEM_Release(pstInst->ulSemId);

		hErr = pthread_cond_wait(&(pstInst->msgQueueNotEmpty), &(pstInst->msgQueueMutex));

		(void)VK_SEM_Get(pstInst->ulSemId);
		pstInst->bActive = TRUE;
		(void)VK_SEM_Release(pstInst->ulSemId);

		if (0 != hErr)
		{
			HxLOG_Error("pthread_cond_wait err:\n");
			goto END_FUNC;
		}
	}

	pstReq = (DAMA_Request_t *)HLIB_PRIORITYQUEUE_Pop(pstInst->pstMsgQueue);

	VK_SEM_Get(pstInst->ulSemId);

	pstInst->ulMsgNumInQueue--;
	pstInst->nMsgTotalSizeInQueue -= (HINT32)pstReq->size;

	while ((pstInst->nMsgTotalSizeInQueue > DAMA_CONFIG_Q_THRESHOLD) && (pstReq->prior > DAMA_CONFIG_PRIORITY))
	{
		HLIB_STD_MemFree(pstReq);

		pstReq = (DAMA_Request_t *)HLIB_PRIORITYQUEUE_Pop(pstInst->pstMsgQueue);
		if (NULL == pstReq)
		{
			break;
		}

		pstInst->ulMsgNumInQueue--;
		pstInst->nMsgTotalSizeInQueue -= (HINT32)pstReq->size;
	}

	if (pstInst->nMsgTotalSizeInQueue < 0)
	{
		HxLOG_Warning("pstInst(%s)->nMsgTotalSizeInQueue < 0 ... something wrong !!!\n", dama_mt_getInstanceName(pstInst));
	}
	VK_SEM_Release(pstInst->ulSemId);

	*ppstReq = pstReq;
	hResult = ERR_OK;

END_FUNC:
	pstInst->nReadCount--;

#if defined(CONFIG_DEBUG)
{
	HUINT32			 ulCurrTick = HLIB_STD_GetSystemTick();
	HUINT32			 ulDuration = ulCurrTick - pstInst->ulLastReqPoppedTick;
	DAMA_Request_t	*pstReq = *ppstReq;

	if ((NULL != pstReq) && (TRUE != pstReq->forDummy))
	{
#if defined(DEBUG_MT_INST_CMD_HISTORY)
		HUINT32 		 ulDataSize = pstReq->size - sizeof(DAMA_Header_t);
		DAMA_Header_t	*pstHeader = (DAMA_Header_t *)(pstReq->data);
#endif

		// Burst mode...
		if (TRUE == bBurst)
		{
			pstInst->ullBurstReqPoppedTick += (HUINT64)ulDuration;
			pstInst->ullBurstReqPoppedNum ++;
		}

#if defined(DEBUG_MT_INST_CMD_HISTORY)
		if (NULL != pstHeader)
		{
			if (HLIB_PRIORITYQUEUE_Length(pstInst->pstMsgQueue) > 100)
			{
				if (TRUE != pstInst->bRecordHistory)
				{
					dama_mt_clearQueueHistory(pstInst);
					pstInst->bRecordHistory = TRUE;
				}

				dama_mt_recordQueueHistory(pstInst, eDAMA_MT_HISTORY_Pop, pstHeader->cmd, ulDataSize);
			}
			else
			{
				pstInst->bRecordHistory = FALSE;
			}
		}
#endif

		// Last pushed time, total number
		pstInst->ulLastReqPoppedTick = ulCurrTick;
		pstInst->ulTotalReqPoppedNum ++;
	}
}
#endif

	(void)pthread_mutex_unlock(&(pstInst->msgQueueMutex));

	return hResult;
}

#define _____REQUEST_ITEM_____
STATIC HERROR dama_mt_getArgsFromRequest (DAMA_Request_t *pstReq, HINT32 *pnClientHandle, HUINT32 *pulRequestId, void **ppvData, HUINT32 *pulSize, DAMA_Header_t **ppstHeader)
{
	HUINT32			 ulSize;
	DAMA_Header_t	*pstHeader;

	if (NULL == pstReq)
	{
		HxLOG_Error("pstReq NULL\n");
		return ERR_FAIL;
	}

	if (pstReq->size < sizeof(DAMA_Header_t))
	{
		HxLOG_Error("pstReq sock(%d), prior(%d), reqId(%d), size(%d) sizeof(DAMA_Header_t) = %d\n", pstReq->sock, pstReq->prior, pstReq->reqId, pstReq->size, sizeof(DAMA_Header_t));
		return ERR_FAIL;
	}

	ulSize = pstReq->size - sizeof(DAMA_Header_t);

	pstHeader = (DAMA_Header_t *)pstReq->data;
	if (pstHeader->size != ulSize)
	{
		HxLOG_Error("pstHeader->size(%d), ulSize(%d) : Header Cmd (%s)\n", pstHeader->size, ulSize, dama_mt_getStringOfCmdEnum(pstHeader->cmd));
		return ERR_FAIL;
	}

	if (NULL != pnClientHandle)
	{
		*pnClientHandle = pstReq->sock;
	}

	if (NULL != pulRequestId)
	{
		*pulRequestId = pstReq->reqId;
	}

	if (NULL != ppvData)
	{
		HUINT8			*pucData = (HUINT8 *)pstReq->data;

		*ppvData = (ulSize != 0) ? (void *)(pucData + sizeof(DAMA_Header_t)) : NULL;
	}

	if (NULL != pulSize)
	{
		*pulSize = ulSize;
	}

	if (NULL != ppstHeader)
	{
		*ppstHeader = pstHeader;
	}

	return ERR_OK;
}


#define _____TEMP_BUFFER_____
STATIC void *dama_mt_getTempBuffer (dama_mt_Instance_t *pstInst, HUINT32 size)
{
	HINT32	index;

	index = pstInst->tempbuf.index;
	if (pstInst->tempbuf.size[index] < size || pstInst->tempbuf.size[index] > DAMA_MT_BUFFER_THRESHOLD)
	{
		void *buf = HLIB_STD_MemAlloc(size);
		if (buf == NULL)
			return NULL;
		if (pstInst->tempbuf.buf[index])
			HLIB_STD_MemFree(pstInst->tempbuf.buf[index]);
		pstInst->tempbuf.buf[index]  = buf;
		pstInst->tempbuf.size[index] = size;
	}

	pstInst->tempbuf.index = (index + 1) % DAMA_MT_TEMP_BUF;
	return pstInst->tempbuf.buf[index];
}

STATIC HINT32 dama_mt_getTempStream (dama_mt_Instance_t *pstInst)
{
	HINT32	stream;

	if (pstInst->tempstream.rwstream[pstInst->tempstream.index] == 0)
	{
		pstInst->tempstream.rwstream[pstInst->tempstream.index] = HLIB_RWSTREAM_Open(NULL, 1024);
	}
	stream = pstInst->tempstream.rwstream[pstInst->tempstream.index];
	if (stream)
	{
		if (HLIB_RWSTREAM_GetBufSize(stream) > DAMA_MT_BUFFER_THRESHOLD)
		{
			HLIB_RWSTREAM_Close(stream);
			pstInst->tempstream.rwstream[pstInst->tempstream.index] = HLIB_RWSTREAM_Open(NULL, 1024);
			stream = pstInst->tempstream.rwstream[pstInst->tempstream.index];
		}
		else
		{
			HLIB_RWSTREAM_Rewind(stream);
		}
	}
	pstInst->tempstream.index = (pstInst->tempstream.index + 1) % DAMA_MT_TEMP_STREAM;
	return stream;
}

#define _____OBSERVERS_____

STATIC HINT32 dama_mt_removeObserverIf (dama_mt_Instance_t *pstInst, HBOOL (* _if)(const DAMA_Observer_t *observer, void *userdata), void *userdata)
{
	HxHASH_TableIter_t	iter;
	DAMA_Observer_t 	*observer;
	DAMA_Observer_t 	*garbage;
	DAMA_Observer_t 	*sibling;
	HxVector_t			*list;
	HINT32				removed = 0;
	HINT32				i, n;

	list = HLIB_VECTOR_NewEasy(HLIB_HASH_Size(pstInst->observer), NULL, NULL);
	if (list == NULL)
		return 0;

	HLIB_HASH_IterInit(&iter, pstInst->observer);
	while (HLIB_HASH_IterNext(&iter, NULL, (void **)&observer))
		HLIB_VECTOR_Add(list, observer);

	n = HLIB_VECTOR_Length(list);
	for (i = 0 ; i < n ; i++)
	{
		observer = (DAMA_Observer_t *)HLIB_VECTOR_ItemAt(list, i);
		sibling  = NULL;
		while (observer)
		{
			if (_if(observer, userdata))
			{
				garbage = observer;

				if (sibling)
				{
					observer = observer->next;
					sibling->next = observer;
				}
				else if (observer->next)
				{
					HLIB_HASH_Replace(pstInst->observer, (void *)HLIB_STD_StrDup(observer->header.name), observer->next);
					observer = observer->next;
				}
				else
				{
					HLIB_HASH_Remove(pstInst->observer, observer->header.name);
					observer = NULL;
				}

				if (garbage->updated)
					HLIB_RWSTREAM_Close(garbage->updated);
				HLIB_STD_MemFree(garbage);
				removed++;
			}
			else
			{
				sibling  = observer;
				observer = observer->next;
			}
		}
	}
	HLIB_VECTOR_Delete(list);
	return removed;
}

STATIC HBOOL dama_mt_cbCmpObserverByRequestId (const DAMA_Observer_t *observer, void *userdata)
{
	return (observer->requestId == (HUINT32)userdata) ? TRUE : FALSE;
}

STATIC HBOOL dama_mt_cbCmpObserverByClientHandle (const DAMA_Observer_t *observer, void *userdata)
{
	return (observer->clientHandle == (HINT32)userdata) ? TRUE : FALSE;
}

STATIC void dama_mt_addtoNotificationQueue (dama_mt_Instance_t *pstInst, const HCHAR *name, HUID uid, HINT32 client_handle)
{
	DAMA_Observer_t	*observer;
	HUINT32			tick;

	HxLOG_Trace();
	HxLOG_Debug("object[%s] uid["HUID_FMT"]\n", name, uid);

	tick  = HLIB_STD_GetSystemTick();
	observer = (void *)HLIB_HASH_Lookup(pstInst->observer, (void *)name);
	for (; observer ; observer = observer->next)
	{
		if (observer->clientHandle == client_handle)
		{
			if(observer->header.allowNotifyMyself == FALSE)
			{
				continue;
			}
		}

		if (observer->header.uid != uid
			&& observer->header.uid != DAMA_TABLE_NOTIFIER)
			continue;

		if (observer->tickStamp.first == 0)
			observer->tickStamp.first = tick;
		observer->tickStamp.last = tick;
		HLIB_RWSTREAM_Write(observer->updated, (void *)&uid, sizeof(HUID));
	}

	dama_mt_setExtraJobFlags(pstInst, eDAMA_MT_EXTJOB_Notify);
}

STATIC void dama_mt_pushNotification (dama_mt_Instance_t *pstInst)
{
	HUINT32				tick;
	HxHASH_TableIter_t	iter;
	DAMA_Observer_t 	*observer;
	DAMA_Header_t		header;
	HINT32				pushsd;

	HxLOG_Trace();

	pushsd = dama_mt_getTempStream(pstInst);
	tick   = HLIB_STD_GetSystemTick();

	HLIB_HASH_IterInit(&iter, pstInst->observer);
	while (HLIB_HASH_IterNext(&iter, NULL, (void **)&observer))
	{
		for (; observer ; observer = observer->next)
		{
			if (observer->tickStamp.last == 0)
				continue;

			if (observer->pushCycle)
			{
				if (tick - observer->tickStamp.first < observer->pushCycle)
					continue;
			}
			else
			{
				if (tick - observer->tickStamp.last < DAMA_MT_NOTIFICATION_TERM)
					continue;
			}

			memcpy(&header, &observer->header, sizeof(DAMA_Header_t));
			header.cmd  = eDAMA_CMD_NOTIFICATION;
			header.size = HLIB_RWSTREAM_GetOffset(observer->updated);
			header.requestId = observer->header.requestId;

			HLIB_RWSTREAM_Write(pushsd, &header, sizeof(DAMA_Header_t));
			if (header.size > 0)
				HLIB_RWSTREAM_Write(pushsd, HLIB_RWSTREAM_GetBuf(observer->updated), HLIB_RWSTREAM_GetOffset(observer->updated));

			DAMA_SERVER_Response(observer->clientHandle, HLIB_RWSTREAM_GetBuf(pushsd), HLIB_RWSTREAM_GetOffset(pushsd), header.requestId);
			//dama_response(dama, observer->clientHandle, HLIB_RWSTREAM_GetBuf(pushsd), HLIB_RWSTREAM_GetOffset(pushsd), header.requestId);
			// dama_response 함수 내부적으로 hash에 add or delete 하게 되고 hash가 틀어진 상태에서dama_push_notification 의 hash iterator를 돌게 되어 잘못된 주소를 call 하게 되는 문제가 존재.
			HxLOG_Debug("%s:%d :: push notification! (%s("HUID_FMT") size:%u)\n", __FUNCTION__, __LINE__, header.name, header.uid, header.size);

			HLIB_RWSTREAM_Rewind(pushsd);
			HLIB_RWSTREAM_Rewind(observer->updated);
			observer->tickStamp.first = 0;
			observer->tickStamp.last  = 0;
		}
	}

	HLIB_HASH_IterInit(&iter, pstInst->observer);
	while (HLIB_HASH_IterNext(&iter, NULL, (void **)&observer))
	{
		for (; observer ; observer = observer->next)
		{
			if (HLIB_RWSTREAM_GetOffset(observer->updated) > 0)
			{
				dama_mt_setExtraJobFlags(pstInst, eDAMA_MT_EXTJOB_Notify);
				return;
			}
		}
	}

	dama_mt_resetExtraJobFlags(pstInst, eDAMA_MT_EXTJOB_Notify);
}

static HBOOL dama_mt_response (dama_mt_Instance_t *pstInst, HINT32 sock, const void *data, HUINT32 size, HUINT32 requestId)
{
	if (!DAMA_SERVER_Response(sock, data, size, requestId))
	{
		HINT32 removed = dama_mt_removeObserverIf(pstInst, dama_mt_cbCmpObserverByClientHandle, (void *)sock);

		HxLOG_Warning("%s(observer removed:%d)\n", __FUNCTION__, removed);
		return FALSE;
	}

	return TRUE;
}

#define _____INSTANCE_ACTION_FUNCTION_____
STATIC HERROR dama_mt_checkTablesCreated (dama_mt_Instance_t *pstInst)
{
	HxList_t			*pstNameList;

	for (pstNameList = pstInst->pstDataTypeList; NULL != pstNameList; pstNameList = HLIB_LIST_Next(pstNameList))
	{
		HCHAR				*pszDataType = (HCHAR *)HLIB_LIST_Data(pstNameList);

		if (!HxSTD_StrEmpty(pszDataType))
		{
			if (DAMA_DB_GetTable(pstInst->pstDb, pszDataType) == NULL)
			{
				DAMA_DB_CreateTable(pstInst->pstDb, pszDataType);
			}
		}
	}

	return ERR_OK;
}

STATIC HBOOL dama_mt_checkExtraJobNeeded (dama_mt_Instance_t *pstInst)
{
	dama_mt_ExtraJobType_e	 eExtraJob = eDAMA_MT_EXTJOB_None;
	HxList_t				*pstNameList;

	for (pstNameList = pstInst->pstDataTypeList; NULL != pstNameList; pstNameList = HLIB_LIST_Next(pstNameList))
	{
		HCHAR				*pszDataType = (HCHAR *)HLIB_LIST_Data(pstNameList);

		if (!HxSTD_StrEmpty(pszDataType))
		{
			DAMA_Table_t	*pstTable = DAMA_DB_GetTable(pstInst->pstDb, pszDataType);

			if ((NULL != pstTable) && (FALSE != pstTable->bUpdated))
			{
				eExtraJob |= eDAMA_MT_EXTJOB_Sync;
				eExtraJob |= eDAMA_MT_EXTJOB_CheckThreshold;
			}
		}
	}

	if (eDAMA_MT_EXTJOB_None != eExtraJob)
	{
		dama_mt_setExtraJobFlags(pstInst, eExtraJob);
		return TRUE;
	}

	return (dama_mt_getExtraJobFlags(pstInst) != eDAMA_MT_EXTJOB_None) ? TRUE : FALSE;
}

STATIC HERROR dama_mt_syncTables (dama_mt_Instance_t *pstInst)
{
	HxList_t			*pstNameList;

	for (pstNameList = pstInst->pstDataTypeList; NULL != pstNameList; pstNameList = HLIB_LIST_Next(pstNameList))
	{
		HCHAR				*pszDataType = (HCHAR *)HLIB_LIST_Data(pstNameList);

		if (!HxSTD_StrEmpty(pszDataType))
		{
			DAMA_Table_t	*pstTable = DAMA_DB_GetTable(pstInst->pstDb, pszDataType);

			if ((NULL != pstTable) && (FALSE != pstTable->bUpdated))
			{
				// HxLOG_Error("Table [%s] Sync...\n", pszDataType);
				DAMA_DB_ForcedSync(pstInst->pstDb, pszDataType);
			}
		}
	}

	dama_mt_resetExtraJobFlags(pstInst, eDAMA_MT_EXTJOB_Sync);
	return ERR_OK;
}

static void	dama_mt_cbForCheckThreshold (const HCHAR *name, HUINT32 count, void *userdata)
{
	dama_mt_Instance_t		*pstInst = (dama_mt_Instance_t *)userdata;
	HINT32					 nTempStream;
	DAMA_Status_e			 eStatus;
	DAMA_Header_t			 stHeader;

	HxSTD_MemSet(&stHeader, 0, sizeof(DAMA_Header_t));

	stHeader.cmd  = eDAMA_CMD_MONITOR;
	HLIB_STD_StrNCpySafe(stHeader.name, name, DAMA_OBJECTNAME_LEN);
	stHeader.size = sizeof(DAMA_Status_e) + sizeof(HUINT32);
	nTempStream = dama_mt_getTempStream(pstInst);
	eStatus = eDAMA_LIMIT;

	HLIB_RWSTREAM_Write(nTempStream, &stHeader, sizeof(DAMA_Header_t));
	HLIB_RWSTREAM_Write(nTempStream, &eStatus, sizeof(DAMA_Status_e));
	HLIB_RWSTREAM_Write(nTempStream, &count , sizeof(HUINT32));

	(void)DAMA_SERVER_Broadcast (HLIB_RWSTREAM_GetBuf(nTempStream), HLIB_RWSTREAM_GetOffset(nTempStream), 0);
}


STATIC HERROR dama_mt_checkThreshold (dama_mt_Instance_t *pstInst)
{
	if (dama_mt_getExtraJobFlags(pstInst) & eDAMA_MT_EXTJOB_CheckThreshold)
	{
		DAMA_DB_CheckThreshold(pstInst->pstDb, dama_mt_cbForCheckThreshold, (void *)pstInst);
		dama_mt_resetExtraJobFlags(pstInst, eDAMA_MT_EXTJOB_CheckThreshold);
	}

	return ERR_OK;
}

STATIC HERROR dama_mt_processForShutdownRequest (dama_mt_Instance_t *pstInst)
{
	dama_mt_ExtraJobType_e	 eExtraJob = dama_mt_getExtraJobFlags(pstInst);

	// For [Shutdown Done] :
	while (eExtraJob & eDAMA_MT_EXTJOB_ShutdownDone)
	{
		HLIB_STD_TaskSleep(50);
		eExtraJob = dama_mt_getExtraJobFlags(pstInst);
	}

	// For [Shutdown] :
	if (eExtraJob & eDAMA_MT_EXTJOB_Shutdown)
	{
		dama_mt_syncTables(pstInst);

		dama_mt_resetExtraJobFlags(pstInst, eDAMA_MT_EXTJOB_Shutdown);
	}

	return ERR_OK;
}



#define _____THREAD_FUNCTION_____
STATIC void dama_mt_processCmdSet (dama_mt_Instance_t *pstInst, HINT32 client_handle, HUINT32 request_id, void *data, HUINT32 size, DAMA_Header_t *header)
{
#if defined(CONFIG_DAMA_BACKGROUND_SYNC)
//	void			*orgdata = pstReq->data;
//	HUINT32 		 orgsize = pstReq->size;
#endif
	DERROR			 eRet;

	HxLOG_Trace();

	if (header->size > 0)
	{
		eRet = DAMA_DB_Set(pstInst->pstDb, header->name, header->uid, header->size, data, TRUE);
	}
	else
	{
		DAMA_DB_Remove(pstInst->pstDb, header->name, header->uid, TRUE);
		eRet = DERR_DATACHANGED;
	}

	if (DERR_DATACHANGED == eRet)
	{
		dama_mt_addtoNotificationQueue(pstInst, header->name, header->uid, client_handle);
	}

#if defined(CONFIG_DAMA_BACKGROUND_SYNC)
//	(void)DAMA_DB_QueueDataDuringSync(pstInst->pstDb, header->name, orgdata, orgsize);
#endif

	return;
}

STATIC void dama_mt_processCmdRequest (dama_mt_Instance_t *pstInst, HINT32 client_handle, HUINT32 request_id, void *data, HUINT32 size, DAMA_Header_t *header)
{
	HINT32				 temp_sd;
	HINT32				 res_sd;
	HCHAR				*buf;
	DAMA_Result_t		*result;
	const DAMA_Item_t	*item;
	DAMA_Header_t		 resheader;

	HxLOG_Trace();

	if ((NULL == header) || (NULL == data))
	{
		HxLOG_Error("header 0x%08x, data 0x%08x\n", (HUINT32)header, (HUINT32)data);
		return;
	}

	HxSTD_MemCopy(&resheader, header, sizeof(DAMA_Header_t));
	resheader.cmd  = eDAMA_CMD_RESPONSE;
	resheader.size = 0;

	buf = dama_mt_getTempBuffer(pstInst, header->size + 1);
	HxSTD_MemCopy(buf, data, header->size);
	buf[header->size] = '\0';

	result = DAMA_DB_Query(pstInst->pstDb, header->name, buf);
	if (result == NULL)
	{
		HxLOG_Warning("%s(%s) Empty repository!\n", __FUNCTION__, header->name);
		HLIB_DBG_Trace("DaMaQuery", {}, 0, buf);
		goto ERROR;
	}

	temp_sd = dama_mt_getTempStream(pstInst);
	HLIB_DBG_Trace("DaMaQuery", {
		res_sd = dama_mt_getTempStream(pstInst);
		while ((item = DAMA_DB_NextItem(result)) != NULL)
		{
			HLIB_RWSTREAM_Write(res_sd, (void *)&(item->uid) , sizeof(HUID));
			HLIB_RWSTREAM_Write(res_sd, (void *)&(item->size), sizeof(HUINT32));
			// +1 의미는 DAMA_Item_t 만큼 주소 점프 해 확장된 공간을 가르침
			HLIB_RWSTREAM_Write(res_sd, (void *)(item + 1), item->size);
		}

		resheader.size = HLIB_RWSTREAM_GetOffset(res_sd);
		HLIB_RWSTREAM_Write(temp_sd, &resheader, sizeof(DAMA_Header_t));
		if (resheader.size > 0)
			HLIB_RWSTREAM_Write(temp_sd, HLIB_RWSTREAM_GetBuf(res_sd), resheader.size);

		dama_mt_response(pstInst, client_handle, HLIB_RWSTREAM_GetBuf(temp_sd), HLIB_RWSTREAM_GetOffset(temp_sd), resheader.requestId);
	}, HLIB_RWSTREAM_GetOffset(temp_sd), buf);

	return;

ERROR:
	temp_sd = dama_mt_getTempStream(pstInst);
	resheader.size = 0;
	HLIB_RWSTREAM_Write(temp_sd, &resheader, sizeof(DAMA_Header_t));

	dama_mt_response(pstInst, client_handle, HLIB_RWSTREAM_GetBuf(temp_sd), HLIB_RWSTREAM_GetOffset(temp_sd), resheader.requestId);


	//dama_mt_pushRequest(pstInst, pstReq);
	return;
}

STATIC void dama_mt_processAddNotifier (dama_mt_Instance_t *pstInst, HINT32 client_handle, HUINT32 request_id, void *data, HUINT32 size, DAMA_Header_t *header)
{
	DAMA_Observer_t		*observer;
	DAMA_Observer_t		*sibling;

	HxLOG_Trace();

	observer = (DAMA_Observer_t *)HLIB_STD_MemCalloc(sizeof(DAMA_Observer_t));
	if (observer == NULL)
	{
		HxLOG_Error(":: Out of memory!\n");
		return;
	}
	observer->updated = HLIB_RWSTREAM_Open(NULL, 0);
	if (!observer->updated)
	{
		HLIB_STD_MemFree(observer);
		HxLOG_Error(":: Out of memory!\n");
		return;
	}
	HxSTD_MemCopy(&observer->header, header, sizeof(DAMA_Header_t));
	observer->clientHandle = client_handle;
	observer->pushCycle    = DLIB_GetPushCycle(header->name);

#if defined(DAMA_DEBUG_OBSERVER)
	if (size > 0)
	{
		HxSTD_StrNCpy(observer->szObserverProcess, data, 64);
		observer->szObserverProcess[63] = '\0';
	}
#endif

	sibling = (DAMA_Observer_t *)HLIB_HASH_Lookup(pstInst->observer, (void *)header->name);
	if (sibling)
	{
		while (sibling->next)
			sibling = sibling->next;
	}
	if (sibling)
		sibling->next = observer;
	else
		HLIB_HASH_Insert(pstInst->observer, (void *)HLIB_STD_StrDup(header->name), observer);

	HxLOG_Debug(":: The notifier(%s:"HUID_FMT") added!\n", header->name, header->uid);
	return;
}

STATIC void dama_mt_processAllowNotifyMyself (dama_mt_Instance_t *pstInst, HINT32 client_handle, HUINT32 request_id, void *data, HUINT32 size, DAMA_Header_t *header)
{
	DAMA_Observer_t		*observer;

	HxLOG_Trace();

	observer = (void *)HLIB_HASH_Lookup(pstInst->observer, (void *)header->name);
	for (; observer ; observer = observer->next)
	{
		if(observer->header.uid != DAMA_TABLE_NOTIFIER)
		{
			continue;
		}

		if(observer->clientHandle != client_handle)
		{
			continue;
		}

		if(observer->header.requestId != header->requestId)
		{
			continue;
		}

		observer->header.allowNotifyMyself = header->allowNotifyMyself;
	}

	return;
}

STATIC void dama_mt_processRemoveNotifier (dama_mt_Instance_t *pstInst, HINT32 client_handle, HUINT32 request_id, void *data, HUINT32 size, DAMA_Header_t *header)
{
	HxLOG_Trace();

	dama_mt_removeObserverIf(pstInst, dama_mt_cbCmpObserverByRequestId, (void *)header->requestId);
	return;
}

STATIC void dama_mt_processForcedSync (dama_mt_Instance_t *pstInst, HINT32 client_handle, HUINT32 request_id, void *data, HUINT32 size, DAMA_Header_t *header)
{
	HBOOL				 bDontSyncNow = FALSE;
	HUINT32				 ulCurrTick = HLIB_STD_GetSystemTick();

	HxLOG_Trace();

	if ((ulCurrTick - pstInst->ulLastForcedSyncTick) > DAMA_MT_THRESHOLD_FOR_SYNC_CMD)
	{
		pstInst->ulContinuousShortSync = 0;
	}
	else
	{
		pstInst->ulContinuousShortSync++;
		if (pstInst->ulContinuousShortSync >= 3)
		{
			bDontSyncNow = TRUE;
		}
	}

	if (TRUE == bDontSyncNow)
	{
		dama_mt_setExtraJobFlags(pstInst, eDAMA_MT_EXTJOB_Sync);
	}
	else
	{
		DAMA_DB_ForcedSync(pstInst->pstDb, header->name);
	}

	pstInst->ulLastForcedSyncTick = ulCurrTick;
	return;
}

STATIC void dama_mt_processRestore (dama_mt_Instance_t *pstInst, HINT32 client_handle, HUINT32 request_id, void *data, HUINT32 size, DAMA_Header_t *header)
{
	HxLOG_Trace();

	DAMA_DB_Restore(pstInst->pstDb, header->name);
	return;
}

STATIC void dama_mt_processBatch (dama_mt_Instance_t *pstInst, HINT32 client_handle, HUINT32 request_id, void *data, HUINT32 size, DAMA_Header_t *header)
{
	HUINT32				 ulBatchSize = 0;
	HCHAR				*pucBuf;
	DAMA_Header_t		 stItemHeader;

	HxLOG_Trace();

	pucBuf = (HUINT8 *)data;
	while (ulBatchSize < size)
	{
		HxSTD_MemCopy(&stItemHeader, pucBuf, sizeof(DAMA_Header_t));
		pucBuf += sizeof(DAMA_Header_t);
		ulBatchSize += sizeof(DAMA_Header_t);

		dama_mt_processOneItem(pstInst, client_handle, header->requestId, pucBuf, stItemHeader.size, &stItemHeader);

		pucBuf += stItemHeader.size;
		ulBatchSize += stItemHeader.size;
	}
}

STATIC void	dama_mt_processOneItem (dama_mt_Instance_t *pstInst, HINT32 client_handle, HUINT32 request_id, void *data, HUINT32 size, DAMA_Header_t *header)
{
	switch (header->cmd)
	{
	// Consider the object name
	case eDAMA_CMD_SET:
		dama_mt_processCmdSet(pstInst, client_handle, request_id, data, size, header);
		break;

	case eDAMA_CMD_REQUEST:
		dama_mt_processCmdRequest(pstInst, client_handle, request_id, data, size, header);
		break;

	case eDAMA_CMD_ADD_NOTIFIER:
		dama_mt_processAddNotifier(pstInst, client_handle, request_id, data, size, header);
		break;

	case eDAMA_CMD_ALLOW_NOTIFY_MYSELF:
		dama_mt_processAllowNotifyMyself(pstInst, client_handle, request_id, data, size, header);
		break;

	case eDAMA_CMD_REMOVE_NOTIFIER:
		dama_mt_processRemoveNotifier(pstInst, client_handle, request_id, data, size, header);
		break;

	case eDAMA_CMD_SYNC:
		dama_mt_processForcedSync(pstInst, client_handle, request_id, data, size, header);
		break;

	case eDAMA_CMD_RESTORE:
		dama_mt_processRestore(pstInst, client_handle, request_id, data, size, header);
		break;

	case eDAMA_CMD_BATCH:
		dama_mt_processBatch(pstInst, client_handle, request_id, data, size, header);
		break;

	// Not-considering object name...
#if 0
	case eDAMA_CMD_FACTORY_RESET:
		dama_command_factory_reset(dama, client_handle, header);
		break;

	case eDAMA_CMD_SERVER_SYNC:
		dama_command_server_sync(dama, client_handle, header);
		break;

#if defined(CONFIG_DAMA_BACKGROUND_SYNC)
	case eDAMA_CMD_SYNCFINISHED:
		DAMA_DB_FinalizeSync(dama->db, header->name);
		break;
#endif

#if defined(DAMA_CHECK_STORAGE_STATUS)
	case eDAMA_CMD_STORAGE_INSERTED:
	case eDAMA_CMD_STORAGE_REMOVED:
	case eDAMA_CMD_STORAGE_UNMOUNTED:
		DAMA_DB_ExecuteForStorageChange(dama->db, header->cmd, data);
		break;
#endif
#endif
	case eDAMA_CMD_NOTIFY_TO_OBSERVERS:
		break;

	default:
		HxLOG_Error(":: Invalid cmd(%d)\n", header->cmd);
		break;
	}
}





STATIC void	dama_mt_processPacket (dama_mt_Instance_t *pstInst, DAMA_Request_t *pstReq)
{
	HINT32				 client_handle;
	HUINT32				 request_id;
	void				*data;
	HUINT32				 size;
	DAMA_Header_t		*header;
	DERROR				 eRet;

#if defined(CONFIG_DEBUG)
	HUINT32				 ulStartTick, ulDurationTick;
#endif

	if (TRUE == pstReq->forDummy)
	{
		return;
	}

	eRet = dama_mt_getArgsFromRequest(pstReq, &client_handle, &request_id, &data, &size, &header);
	if (ERR_OK != eRet)
	{
		HxLOG_Error("dama_mt_getArgsFromRequest err:\n");
		return;
	}

#if defined(CONFIG_DEBUG)
	ulStartTick = HLIB_STD_GetSystemTick();
#endif

	// HxLOG_Error("[%s:%d] Inst (%s) processes Cmd (%s)\n", __FUNCTION__, __LINE__, pstInst->szInstName, dama_mt_getStringOfCmdEnum(header->cmd));
	dama_mt_processOneItem(pstInst, client_handle, request_id, data, size, header);

#if defined(CONFIG_DEBUG)
	ulDurationTick = HLIB_STD_GetSystemTick() - ulStartTick;

	if (0 == ulDurationTick)
	{
		ulDurationTick = 1;
	}

	pstInst->ulLastCmdProcessTick = ulDurationTick;
	pstInst->eLastCmd = header->cmd;
	pstInst->ulLastCmdDataSize = size;
	pstInst->ullTotalCmdProcessTick += (HUINT64)ulDurationTick;
	pstInst->ullTotalCmdProcessNum ++;

	if ((0 == pstInst->ulMaxCmdProcessTick) || (pstInst->ulMaxCmdProcessTick < ulDurationTick))
	{
		pstInst->ulMaxCmdProcessTick = ulDurationTick;
	}

	if ((0 == pstInst->ulMinCmdProcessTick) || (pstInst->ulMinCmdProcessTick > ulDurationTick))
	{
		pstInst->ulMinCmdProcessTick = ulDurationTick;
	}
#endif
}

STATIC void dama_mt_tableThread (void *pvArg)
{
	dama_mt_Instance_t	*pstInst = (dama_mt_Instance_t *)pvArg;
	DAMA_Request_t		*pstReq;

	if (NULL != pstInst)
	{
		HUINT32			 ulCurrTick;
		HUINT32			 ulPrevPushTick, ulPrevSyncTick;

		ulPrevPushTick = ulPrevSyncTick = HLIB_STD_GetSystemTick();

		while (1)
		{
			LOG_INST_POS(pstInst);
			(void)dama_mt_checkTablesCreated(pstInst);

			pstReq = NULL;
			LOG_INST_POS(pstInst);
			(void)dama_mt_popRequest(pstInst, &pstReq);
			LOG_INST_POS(pstInst);
			if (NULL != pstReq)
			{
				dama_mt_processPacket(pstInst, pstReq);
				HLIB_STD_MemFree(pstReq);
			}

			LOG_INST_POS(pstInst);
			if (pstInst->ulMsgNumInQueue > 1)
			{
				continue;
			}

			LOG_INST_POS(pstInst);
			if (TRUE == dama_mt_checkExtraJobNeeded(pstInst))
			{
				ulCurrTick = HLIB_STD_GetSystemTick();

				if ((ulCurrTick - ulPrevPushTick) > DAMA_MT_NOTIFICATION_CYCLE)
				{
					LOG_INST_POS(pstInst);
					dama_mt_pushNotification(pstInst);
					ulPrevPushTick = ulCurrTick;
				}

				if ((ulCurrTick - ulPrevSyncTick) > DAMA_MT_SYNC_CYCLE)
				{
					LOG_INST_POS(pstInst);
					dama_mt_syncTables(pstInst);
					ulPrevSyncTick = ulCurrTick;
				}

				// if (TRUE == pstInst->bSynchronized)
				//{
				//	DAMA_DB_Vacuum(pstInst->pstDb);
				//}

				LOG_INST_POS(pstInst);
				dama_mt_checkThreshold(pstInst);

				LOG_INST_POS(pstInst);
				dama_mt_processForShutdownRequest(pstInst);
			}
		}
	}
}

STATIC void dama_mt_cbTimerMultithread (unsigned long ulSize, void *pvData)
{
	HBOOL				 bNeedWakeUp = FALSE;
	dama_mt_Instance_t	*pstInst = *((dama_mt_Instance_t **)pvData);

	HxLOG_Trace();

	(void)VK_SEM_Get(pstInst->ulSemId);
	{
		if (eDAMA_MT_EXTJOB_None != pstInst->eExtraJob)
		{
			bNeedWakeUp = TRUE;
		}
	}
	(void)VK_SEM_Release(pstInst->ulSemId);

	if (TRUE == bNeedWakeUp)
	{
		(void)dama_mt_wakeUpThreadFromWaiting(pstInst);
	}
}

STATIC HBOOL dama_mt_initThread (const HCHAR *name, const HCHAR *tags[], void *userdata)
{
	unsigned long		 ulTid = 0, ulTimerId = 0, ulSemId = 0;
	HCHAR				 szThreadName[64];
	HCHAR				*pszName;
	DAMA_DB_t			*pstSrcDb = (DAMA_DB_t *)userdata;
	DAMA_DB_t			*pstInstDb = NULL;
	dama_mt_Instance_t	*pstInst = NULL;
	HxList_t			*pstNameList = NULL, *pstTempList;
	int					 nRet;

	if ((HxSTD_StrEmpty(name)) || (dama_mt_getInstance((HCHAR *)name) != NULL))
	{
		return FALSE;
	}

#if 0
	if (HxSTD_StrNCmp(name, DAMA_MT_EPG_PREFIX, HxSTD_StrLen(DAMA_MT_EPG_PREFIX)) == 0)
	{
		HUINT32				 ulCnt;
		HCHAR				*ppszEpgName[] = {
												"eDxEPG_TYPE_PF",
												"eDxEPG_TYPE_SC",
												"eDxEPG_TYPE_PF_EX",
												"eDxEPG_TYPE_SC_EX",
												"eDxEPG_TYPE_RF",
												"eDxEPG_TYPE_TVTV",
												"eDxEPG_TYPE_IP",

												NULL
											};

		for (ulCnt = 0; ppszEpgName[ulCnt] != NULL; ulCnt++)
		{
			pszName = (HCHAR *)HLIB_STD_MemDup(ppszEpgName[ulCnt], HxSTD_StrLen(ppszEpgName[ulCnt]) + 1);

			if (NULL != pszName)
			{
				pstNameList = HLIB_LIST_Append(pstNameList, (void *)pszName);
			}
			else
			{
				goto ERROR;
			}
		}
	}
	else
#endif
	{
		pszName = (HCHAR *)HLIB_STD_MemDup(name, HxSTD_StrLen(name) + 1);
		if (NULL != pszName)
		{
			pstNameList = HLIB_LIST_Append(pstNameList, (void *)pszName);
		}
	}

	if (NULL == pstNameList)
	{
		goto ERROR;
	}

	pstInst = (dama_mt_Instance_t *)DAMA_Calloc(sizeof(dama_mt_Instance_t));
	if (NULL == pstInst)
	{
		HxLOG_Error("Memory allocation failed: name[%s]\n", name);
		goto ERROR;
	}

	pstInstDb = DAMA_DB_Open(DAMA_DIR_DB, DAMA_DIR_BACKUPDB, DAMA_DIR_TEMP_PATH, FALSE, FALSE);
	if (NULL == pstInstDb)
	{
		HxLOG_Error("DAMA_DB_Open: name[%s]\n", name);
		goto ERROR;
	}

	pstInstDb->pMainDB = pstSrcDb->pMainDB;

	for (pstTempList = pstNameList; NULL != pstTempList; pstTempList = HLIB_LIST_Next(pstTempList))
	{
		pszName = (HCHAR *)HLIB_LIST_Data(pstTempList);
		if (!HxSTD_StrEmpty(pszName))
		{
			DAMA_DB_CreateTable(pstInstDb, pszName);
		}
	}

	pstInst->pstDb = pstInstDb;
	pstInst->pstDataTypeList = pstNameList;
	pstInst->observer = HLIB_HASH_NewFull((_HxHashFunc)HLIB_HASH_GetStringKey, (_HxEqualFunc)HLIB_HASH_GetStringKey
							, (_HxDestroyNotify)HLIB_STD_MemFree_CB
							, NULL);

	// Message Queue (No limitation and priority) for the instance

	nRet = dama_mt_createMsgQueue(pstInst);
	if (ERR_OK != nRet)
	{
		HxLOG_Error("dama_mt_createMsgQueue err: name[%s]\n", name);
		goto ERROR;
	}

	// Semaphore for the instance

	nRet = VK_SEM_Create(&ulSemId, name, VK_SUSPENDTYPE_FIFO);
	if (VK_OK != nRet)
	{
		HxLOG_Error("VK_TIMER_EventEvery err:\n");
		goto ERROR;
	}

	pstInst->ulSemId = ulSemId;

	// Task for the instance

	HxSTD_PrintToStrN(szThreadName, 64, "dama_%s", name);
	szThreadName[63] = '\0';

	nRet = VK_TASK_Create(dama_mt_tableThread, VK_TASK_PRIORITY_MW_REF, SIZE_256K, (const char *)szThreadName, (void*)pstInst, &ulTid, 0);
	if (VK_OK != nRet)
	{
		HxLOG_Error("VK_TASK_Create failed: taskName[%s]\n", szThreadName);
		goto ERROR;
	}

	pstInst->ulTaskId	= ulTid;

	nRet = VK_TASK_Start(ulTid);
	if (VK_OK != nRet)
	{
		HxLOG_Error("VK_TASK_Start failed: taskName[%s]\n", szThreadName);
		goto ERROR;
	}

	// Timer for the instance

	nRet = VK_TIMER_EventEvery(DAMA_MT_CHECK_TIMER_CYCLE, dama_mt_cbTimerMultithread, (void *)&pstInst, sizeof(dama_mt_Instance_t *), &ulTimerId);
	if (VK_OK != nRet)
	{
		HxLOG_Error("VK_TIMER_EventEvery err:\n");
		goto ERROR;
	}

	pstInst->ulTimerId	= ulTimerId;

	dama_mt_registerInstance(pstInst);
	return FALSE;

ERROR:
	if (NULL != pstInst)
	{
		dama_mt_destroyMsgQueue(pstInst);
		DAMA_Free(pstInst);
	}

	if (0 != ulTid)
	{
		VK_TASK_Destroy(ulTid);
	}

	if (NULL != pstNameList)
	{
		HLIB_LIST_RemoveAllFunc(pstNameList, HLIB_STD_MemFree_CB);
	}

	return FALSE;
}

#define _____ALL_INSTANCE_EXECUTE_____

STATIC HBOOL dama_mt_cbPushRequestToThread (dama_mt_Instance_t *pstInst, void *userdata)
{
	dama_mt_ReqItem_t	*pstMtReqItem = (dama_mt_ReqItem_t *)userdata;
	DAMA_Request_t		*pstReq = pstMtReqItem->pstReq;
	DAMA_Header_t		*pstHeader = NULL;
	HERROR				 hErr, hResult = ERR_FAIL;

	hErr = dama_mt_getArgsFromRequest(pstReq, NULL, NULL, NULL, NULL, &pstHeader);
	if ((ERR_OK != hErr) || (NULL == pstHeader))
	{
		HxLOG_Error("dama_mt_getArgsFromRequest err: instName[%s]\n", dama_mt_getInstanceName(pstInst));
		goto END_FUNC;
	}

	HxSTD_StrNCpy(pstHeader->name, dama_mt_getInstanceName(pstInst), DAMA_OBJECTNAME_LEN);
	pstHeader->name[DAMA_OBJECTNAME_LEN - 1] = '\0';

	hErr = dama_mt_pushRequest(pstInst, pstReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("dama_mt_pushRequest err: instName[%s]\n", dama_mt_getInstanceName(pstInst));
		goto END_FUNC;
	}

	hResult = ERR_OK;

END_FUNC:
	pstMtReqItem->hRetValue = hResult;
	return FALSE;
}

STATIC HBOOL dama_mt_cbGetShortestRequestingThread (dama_mt_Instance_t *pstInst, void *userdata)
{
	dama_mt_Check_t			*pstCheck = (dama_mt_Check_t *)userdata;

	(void)VK_SEM_Get(pstInst->ulSemId);

	if ((NULL == pstCheck->pstInst) ||
		(pstInst->ulMsgNumInQueue < pstCheck->ulArg1))
	{
		pstCheck->pstInst = pstInst;
		pstCheck->ulArg1 = pstInst->ulMsgNumInQueue;
	}

	(void)VK_SEM_Release(pstInst->ulSemId);
	return FALSE;
}

STATIC HBOOL dama_mt_cbCheckThreadRunning (dama_mt_Instance_t *pstInst, void *userdata)
{
	dama_mt_Check_t			*pstCheck = (dama_mt_Check_t *)userdata;

	(void)VK_SEM_Get(pstInst->ulSemId);

	if ((pstInst->ulMsgNumInQueue > 0) ||
		(TRUE == pstInst->bActive))
	{
		pstCheck->ulArg1++;
	}

	(void)VK_SEM_Release(pstInst->ulSemId);
	return FALSE;
}

STATIC HBOOL dama_mt_cbFindInstanceWithQuery (dama_mt_Instance_t *pstInst, void *userdata)
{
	HxList_t				*pstNameList;
	dama_mt_Check_t			*pstCheck = (dama_mt_Check_t *)userdata;
	HCHAR					*pszQuery = NULL;

	if (NULL != pstCheck)
	{
		pszQuery = (HCHAR *)pstCheck->pvArgPtr1;
	}

	if (NULL != pszQuery)
	{
		for (pstNameList = pstInst->pstDataTypeList; NULL != pstNameList; pstNameList = HLIB_LIST_Next(pstNameList))
		{
			HCHAR					*pszDataType = (HCHAR *)HLIB_LIST_Data(pstNameList);

			if (!(HxSTD_StrEmpty(pszDataType)))
			{
				if (HxSTD_StrStr(pszQuery, pszDataType) != NULL)
				{
					pstCheck->pstInst = pstInst;
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

STATIC HBOOL dama_mt_cbSetShutdown (dama_mt_Instance_t *pstInst, void *userdata)
{
	HBOOL					*pbShutdown = (HBOOL *)userdata;

	if (TRUE == *pbShutdown)
	{
		dama_mt_setExtraJobFlags(pstInst, eDAMA_MT_EXTJOB_Shutdown);
	}
	else
	{
		dama_mt_resetExtraJobFlags(pstInst, eDAMA_MT_EXTJOB_Shutdown);
	}

	return FALSE;
}

STATIC HBOOL dama_mt_cbSetShutdownDone (dama_mt_Instance_t *pstInst, void *userdata)
{
	HBOOL					*pbShutdownDone = (HBOOL *)userdata;

	if (TRUE == *pbShutdownDone)
	{
		dama_mt_setExtraJobFlags(pstInst, eDAMA_MT_EXTJOB_ShutdownDone);
	}
	else
	{
		dama_mt_resetExtraJobFlags(pstInst, eDAMA_MT_EXTJOB_ShutdownDone);
	}

	return FALSE;
}

STATIC void dama_mt_forEachInstance (HBOOL (*pfFunc)(dama_mt_Instance_t *, void *), void *pvUserData)
{
	if (NULL != pfFunc)
	{
		HxList_t			*pstListItem;

		for (pstListItem = s_pstDamaMt_InstList; NULL != pstListItem; pstListItem = HLIB_LIST_Next(pstListItem))
		{
			HBOOL					 bRet;
			dama_mt_Instance_t		*pstInst = (dama_mt_Instance_t *)HLIB_LIST_Data(pstListItem);

			if (NULL != pstInst)
			{
				bRet = pfFunc(pstInst, pvUserData);
				if (TRUE == bRet)
				{
					return;
				}
			}
		}
	}
}

#if defined(CONFIG_DEBUG)
STATIC void dama_mt_PrintQueueHistory (dama_mt_Instance_t *pstInst)
{
#if defined(DEBUG_MT_INST_CMD_HISTORY)
	if (NULL != pstInst->pstQueueHistory)
	{
		HUINT32				 ulPos = pstInst->ulHistoryPos;

		HLIB_CMD_Printf("              100-over Queue History....\n");

		while (1)
		{
			dama_mt_History_t	*pstHistoryItem = &(pstInst->pstQueueHistory[ulPos]);

			if (pstHistoryItem->eType == eDAMA_MT_HISTORY_Push)
			{
				HLIB_CMD_Printf("			     [PUSH] [%s] DataSize = %d, Tick = %dms\n", dama_mt_GetCmdString(pstHistoryItem->eCmd), pstHistoryItem->ulDataSize, pstHistoryItem->ulTick);
			}
			else if (pstHistoryItem->eType == eDAMA_MT_HISTORY_Pop)
			{
				HLIB_CMD_Printf("			     [POP] [%s] DataSize = %d, Tick = %dms\n", dama_mt_GetCmdString(pstHistoryItem->eCmd), pstHistoryItem->ulDataSize, pstHistoryItem->ulTick);
			}

			ulPos++;
			if (ulPos >= DAMA_MT_HISTORY_FOR_QUEUE_INCREASE)
			{
				ulPos = 0;
			}

			if (ulPos == pstInst->ulHistoryPos)
			{
				break;
			}
		}
	}
#else
	HLIB_CMD_Printf("			   History Info : 'DEBUG_MT_INST_CMD_HISTORY' not enabled.\n");
#endif
}

static int dama_mt_cmdRequestObj (void *szArgList)
{
	HCHAR				*szArg = NULL;
	unsigned int		result = HxCMD_ERR;
	HUINT32				ulPtrValue;
	HBOOL				bFoundInst = FALSE, bFoundQueue = FALSE;

	if(szArgList != NULL)
	{
		DAMA_Request_t		*pstReq;
		dama_mt_Instance_t	*pstInst;
		HxPriorityQueue_t	*pstMsgQueue;
		HxList_t			*pstListItem;

		// get 1st argument
		szArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if (szArg != NULL)
		{
			ulPtrValue = HLIB_STD_StrToUINT32((const HCHAR *)szArg);
			if (0 == ulPtrValue)
			{
				return HxCMD_ERR;
			}

			pstReq = (DAMA_Request_t *)ulPtrValue;
			if (pstReq->ulMaskValue != 0x7852)
			{
				HLIB_CMD_Printf("ptr 0x%08x is not DAMA_Request_t...\n", ulPtrValue);
				return HxCMD_ERR;
			}

			pstInst = (dama_mt_Instance_t *)pstReq->pvInstPnt;
			pstMsgQueue = (HxPriorityQueue_t *)pstReq->pvQueuePnt;

			for (pstListItem = s_pstDamaMt_InstList; NULL != pstListItem; pstListItem = HLIB_LIST_Next(pstListItem))
			{
				dama_mt_Instance_t	*pstSrcInst = (dama_mt_Instance_t *)HLIB_LIST_Data(pstListItem);

				if (NULL != pstSrcInst)
				{
					if (pstInst == pstSrcInst)
					{
						bFoundInst = TRUE;
						HLIB_CMD_Printf("Request were in the inst [%s]\n", dama_mt_getInstanceName(pstSrcInst));
						HLIB_CMD_Printf("     - Extra Job Flag = 0x%08x\n", pstSrcInst->eExtraJob);
						HLIB_CMD_Printf("     - MSG Num = %d\n", pstSrcInst->ulMsgNumInQueue);
					}

					if (pstMsgQueue == pstSrcInst->pstMsgQueue)
					{
						bFoundQueue = TRUE;
						HLIB_CMD_Printf("Request were in the Queue [%s]\n", dama_mt_getInstanceName(pstSrcInst));
						HLIB_CMD_Printf("     - MSG Num = %d\n", HLIB_PRIORITYQUEUE_Length(pstMsgQueue));
					}
				}
			}

			if (TRUE != bFoundInst)
			{
				HLIB_CMD_Printf("Request lost inst...\n");
			}

			if (TRUE != bFoundQueue)
			{
				HLIB_CMD_Printf("Request lost queue...\n");
			}
		}

		result = HxCMD_OK;
	}

	return result;
}

static int dama_mt_cmdListInsts (void *szArgList)
{
	HxList_t			*pstListItem;
	HUINT32				 ulAverageDelay;
	float				 fShotRate;

	for (pstListItem = s_pstDamaMt_InstList; NULL != pstListItem; pstListItem = HLIB_LIST_Next(pstListItem))
	{
		dama_mt_Instance_t	*pstSrcInst = (dama_mt_Instance_t *)HLIB_LIST_Data(pstListItem);

		if (NULL != pstSrcInst)
		{
			HLIB_CMD_Printf("- DAMA INST [%s] : \n", dama_mt_getInstanceName(pstSrcInst));
			HLIB_CMD_Printf("      Current Status : %s\n", (pstSrcInst->nReadCount > 0) ? "SLEEP" : "RUNNING");
			HLIB_CMD_Printf("	   Extra Job Flag = 0x%08x\n", pstSrcInst->eExtraJob);
			HLIB_CMD_Printf("	   Request Number = %d\n", pstSrcInst->ulMsgNumInQueue);
			if (!HxSTD_StrEmpty(pstSrcInst->szLastFuncName))
			{
				HLIB_CMD_Printf("	   Last LOG : [%s:%d] Tick = %d\n", pstSrcInst->szLastFuncName, pstSrcInst->ulLastFuncLine, pstSrcInst->ulLastLogTick);
			}
			HLIB_CMD_Printf("	   Last Popped Tick = %d\n", pstSrcInst->ulLastReqPoppedTick);
			HLIB_CMD_Printf("	   Last Pushed Tick = %d\n", pstSrcInst->ulLastReqPushedTick);
			HLIB_CMD_Printf("	   Current Tick     = %d\n", HLIB_STD_GetSystemTick());

			HLIB_CMD_Printf("	   Total Pushed Num = %d\n", pstSrcInst->ulTotalReqPushedNum);
			HLIB_CMD_Printf("	   Total Popped Num = %d\n", pstSrcInst->ulTotalReqPoppedNum);

			fShotRate = (0LL != pstSrcInst->ullBurstReqPushedTick) ? (float)pstSrcInst->ullBurstReqPushedNum * (float)1000 / (float)pstSrcInst->ullBurstReqPushedTick : (float)0;
			ulAverageDelay = (0LL != pstSrcInst->ullBurstReqPushedNum) ? (HUINT32)(pstSrcInst->ullBurstReqPushedTick / pstSrcInst->ullBurstReqPushedNum) : 0;
			HLIB_CMD_Printf("	   Push Delay AVG (Burst) = %d ms\n", ulAverageDelay);
			HLIB_CMD_Printf("	   Push Rate (Burst)      = %f shot/sec\n", fShotRate);

			fShotRate = (0LL != pstSrcInst->ullBurstReqPoppedTick) ? (float)pstSrcInst->ullBurstReqPoppedNum * (float)1000 / (float)pstSrcInst->ullBurstReqPoppedTick : (float)0;
			ulAverageDelay = (0LL != pstSrcInst->ullBurstReqPoppedNum) ? (HUINT32)(pstSrcInst->ullBurstReqPoppedTick / pstSrcInst->ullBurstReqPoppedNum) : 0;
			HLIB_CMD_Printf("	   Pop Delay AVG (Burst)  = %d ms\n", ulAverageDelay);
			HLIB_CMD_Printf("	   Pop Rate (Burst)       = %f shot/sec\n", fShotRate);

			ulAverageDelay = (0LL != pstSrcInst->ullTotalCmdProcessNum) ? (HUINT32)(pstSrcInst->ullTotalCmdProcessTick / pstSrcInst->ullTotalCmdProcessNum) : 0;
			HLIB_CMD_Printf("	   CMD Process = (%dms ~ %dms), Average (%dms), executed processes (%d)\n", pstSrcInst->ulMinCmdProcessTick, pstSrcInst->ulMaxCmdProcessTick, ulAverageDelay, (HUINT32)pstSrcInst->ullTotalCmdProcessNum);
			HLIB_CMD_Printf("	                 Last Cmd Type(%s), Data Size (%d) Execution Time (%dms)\n", dama_mt_GetCmdString(pstSrcInst->eLastCmd), pstSrcInst->ulLastCmdDataSize, pstSrcInst->ulLastCmdProcessTick);

			dama_mt_PrintQueueHistory(pstSrcInst);

#if defined(DAMA_DEBUG_OBSERVER)
			HLIB_CMD_Printf("      Observers : \n");
			{
				HxHASH_TableIter_t		 iter;
				DAMA_Observer_t 		*observer = NULL;

				HLIB_HASH_IterInit(&iter, pstSrcInst->observer);
				while (HLIB_HASH_IterNext(&iter, NULL, (void **)&observer))
				{
					for (; observer ; observer = observer->next)
					{
						HLIB_CMD_Printf("	               [%s] [%s]\n", observer->header.name, observer->szObserverProcess);
					}
				}
			}
#endif
			HLIB_CMD_Printf("\n");
		}
	}

	return HxCMD_OK;
}

/**********************************************
	Command 수행 함수를 등록하는 통합 등록 함수.
***********************************************/
#define	hCmdHandle		"multi"
void CMD_DAMA_MULTITHREAD_Init (void)
{
	HLIB_CMD_RegisterWord(hCmdHandle, dama_mt_cmdRequestObj,
			"request_obj",	/* keyword */
			"request_obj",	/*	 help  */
			"request_obj [address_of_request_object]");	/*	usage  */

	HLIB_CMD_RegisterWord(hCmdHandle, dama_mt_cmdListInsts,
			"list_insts",	/* keyword */
			"list_insts",	/*	 help  */
			"list_insts");	/*	usage  */
}
#endif

#define ___GLOBAL_FUNCTIONS___
HERROR DAMA_MULTITHREAD_Init (DAMA_DB_t	*pstDb)
{
	DAMA_DB_ForEachObject(dama_mt_initThread, (void *)pstDb);
#if defined(CONFIG_DEBUG)
	CMD_DAMA_MULTITHREAD_Init();
#endif

#if 0//defined(CONFIG_DEBUG)
{
	HxList_t		*pstListItem;

	HxLOG_Error("--------------------------------------------------------------------------------\n");
	HxLOG_Error("Created number of threads = %d\n", HLIB_LIST_Length(s_pstDamaMt_InstList));
	for (pstListItem = s_pstDamaMt_InstList; NULL != pstListItem; pstListItem = HLIB_LIST_Next(pstListItem))
	{
		dama_mt_Instance_t	*pstInst = HLIB_LIST_Data(pstListItem);

		if (NULL != pstInst)
		{
			HxList_t			*pstNameList;

			for (pstNameList = pstInst->pstDataTypeList; NULL != pstNameList; pstNameList = HLIB_LIST_Next(pstNameList))
			{
				HCHAR				*pszName = (HCHAR *)HLIB_LIST_Data(pstNameList);

				if (!HxSTD_StrEmpty(pszName))
				{
					HxLOG_Error("	- DataType(%s), TaskID(%d), TimerId(%d)\n", pszName, pstInst->ulTaskId, pstInst->ulTimerId);
				}
			}
		}
	}

	HxLOG_Error("--------------------------------------------------------------------------------\n");
}
#endif

	return ERR_OK;
}

HERROR DAMA_MULTITHREAD_PushRequest (const HCHAR *name, DAMA_Request_t *pstReq)
{
	dama_mt_Instance_t	*pstInst = dama_mt_getInstance((HCHAR *)name);
	HERROR				 hErr;

	if (NULL == pstInst)
	{
		HxLOG_Error("dama_mt_getInstance err: name(%s)\n", name);
		return ERR_FAIL;
	}

	hErr = dama_mt_pushRequest(pstInst, pstReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("dama_mt_pushRequest err: name(%s)\n", name);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR DAMA_MULTITHREAD_PushRequestToAllTable (DAMA_Request_t *pstReq)
{
	dama_mt_ReqItem_t	 stReqItem;
	DAMA_Request_t		*pstDupReq = dama_mt_duplicateRequest(pstReq);

	if (NULL == pstDupReq)
	{
		HxLOG_Error("DAMA_MULTITHREAD_PushRequestToAllTable err:\n");
		return ERR_FAIL;
	}

	stReqItem.pstReq	= pstDupReq;
	stReqItem.hRetValue	= ERR_FAIL;

	dama_mt_forEachInstance(dama_mt_cbPushRequestToThread, (void *)&stReqItem);

	HLIB_STD_MemFree(pstDupReq);
	return ERR_OK;
}

HERROR DAMA_MULTITHREAD_PushRequestToShortestQueue (DAMA_Request_t *pstReq)
{
	dama_mt_Check_t		 stCheck;

	HxSTD_MemSet(&stCheck, 0, sizeof(dama_mt_Check_t));

	dama_mt_forEachInstance(dama_mt_cbGetShortestRequestingThread, (void *)&stCheck);
	if (NULL == stCheck.pstInst)
	{
		HxLOG_Error("DAMA_DB_ForEachObject(dama_mt_cbGetShortestRequestingThread) failed:\n");
		return ERR_FAIL;
	}

	return DAMA_MULTITHREAD_PushRequest((const HCHAR *)dama_mt_getInstanceName(stCheck.pstInst), pstReq);
}

HERROR DAMA_MULTITHREAD_PushRequestByQuery (const HCHAR *pszQuery, DAMA_Request_t *pstReq)
{
	dama_mt_Check_t		 stCheck;
	HERROR				 hErr;

	HxSTD_MemSet(&stCheck, 0, sizeof(dama_mt_Check_t));
	stCheck.pvArgPtr1 = (void *)pszQuery;

	dama_mt_forEachInstance(dama_mt_cbFindInstanceWithQuery, (void *)&stCheck);

	if (NULL == stCheck.pstInst)
	{
		HxLOG_Error("DAMA_DB_ForEachObject(dama_mt_cbFindInstanceWithQuery) failed:\n");
		return ERR_FAIL;
	}

	hErr = dama_mt_pushRequest(stCheck.pstInst, pstReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("dama_mt_pushRequest err: name(%s)\n", dama_mt_getInstanceName(stCheck.pstInst));
		return ERR_FAIL;
	}

	return ERR_OK;
}

HBOOL DAMA_MULTITHREAD_CheckMultithreadsRunning (void)
{
	dama_mt_Check_t		 stCheck;

	HxSTD_MemSet(&stCheck, 0, sizeof(dama_mt_Check_t));

	dama_mt_forEachInstance(dama_mt_cbCheckThreadRunning, (void *)&stCheck);

	return (stCheck.ulArg1 > 0) ? TRUE : FALSE;
}

HBOOL DAMA_MULTITHREAD_IsMultithreaded (HCHAR *szDataType)
{
	return (dama_mt_getInstance(szDataType) != NULL) ? TRUE : FALSE;
}

HERROR DAMA_MULTITHREAD_SetShutdown (HBOOL bShutdown)
{
	dama_mt_forEachInstance(dama_mt_cbSetShutdown, (void *)&bShutdown);
	return ERR_OK;
}

HERROR DAMA_MULTITHREAD_SetShutdownDone (HBOOL bShutdownDone)
{
	dama_mt_forEachInstance(dama_mt_cbSetShutdownDone, (void *)&bShutdownDone);
	return ERR_OK;
}

