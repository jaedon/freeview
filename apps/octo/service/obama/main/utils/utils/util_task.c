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
#include <stdio.h>
#include <octo_common.h>

#include "vkernel.h"
#include "util_task.h"
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define USE_VK_MSG

#define TASKUTIL_POOL_SIZE								11
#define TASKUTIL_MAX_MSG_LEN							256

#define TASKUTIL_PROC_LOW_DURATION					60			// 40 ms
#define TASKUTIL_PROC_HIGH_DURATION					40			// 60 ms
#define TASKUTIL_TASKSLEEP_LOW_DURATION				800			// 800 ms
#define TASKUTIL_TASKSLEEP_HIGH_DURATION				1000		// 1200 ms
#define TASKUTIL_TASK_MAX_WAITTIME					1000		// 1200 ms

#define MSGQUEUE_TASKUTIL_NAME			"taskutil_q (%d)"
#define TASK_TASKUTIL_NAME				"taskutil_task (%d)"
#define SEM_TASKUTIL_NAME					"taskutil_sem (%d)"








/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	TASKUTIL_STATUS_NONE,
	TASKUTIL_STATUS_NOTUSED,
	TASKUTIL_STATUS_START,
	TASKUTIL_STATUS_COMPLETE,

	TASKUTIL_STATUS_MAX
}TASKUTIL_STATUS_e;


typedef enum{
	MODE_QUEUE_FULL_PASS		= 0x00000001,
	MODE_QUEUE_FULL_STOP		= 0x00000002

}MODE_TASKUTIL_QUEUE;


typedef	HERROR	(*pfnQitemDataFree)(void*);
typedef	HERROR	(*pfnQitemDataCopy)(void* pDest, void* pSrc);


typedef	void* QITEM;

typedef struct tagTaskUtilQueue
{
	HINT32				lQueueSize;
	HUINT32				lQueueMode;
	HUINT32				ulSemId;
	HINT32				lFirst;				// Index of item at front of the queue
	HINT32				lLast;				// Index of item at rear of the queue
	HINT32				lCount;				// Total number of items in the queue
	HINT32				lFocus;				// current focus index
	QITEM 				*aItem;				// Circular array
	pfnQitemDataFree		pfnDataFree;
	pfnQitemDataCopy		pfnDataCopy;
}TASKUTIL_QUEUE_t;




/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
#if defined(CONFIG_UTILTASK_Q)
static HERROR				local_taskutil_CreateQueue(TASKUTIL_QUEUE_t **ppQueue, HINT32 lQueueSize, HUINT32 ulMode);
static HERROR				local_taskutil_DestroyQueue(TASKUTIL_QUEUE_t *pQueue);
static HERROR				local_taskutil_PopQueue(TASKUTIL_QUEUE_t *pQueue, HBOOL bCopy, void** ppData);
static HERROR				local_taskutil_PushQueue(TASKUTIL_QUEUE_t *pQueue, void* pData);
static HERROR				local_taskutil_IsEmptyQueue(TASKUTIL_QUEUE_t *pQueue, HBOOL *pbEmpty);
static HERROR				local_taskutil_IsFullQueue(TASKUTIL_QUEUE_t *pQueue, HBOOL *pbFull);
static HERROR				local_taskutil_GetNextQueueIndex(TASKUTIL_QUEUE_t *pQueue, HINT32 lIndex, HINT32 *plNextIndex);
static HERROR				local_taskutil_MoveNextFocus(TASKUTIL_QUEUE_t *pQueue, void* pData, HBOOL bCopy);
static HERROR				local_taskutil_MovePrevFocus(TASKUTIL_QUEUE_t *pQueue, void* pData, HBOOL bCopy);
static HERROR				local_taskutil_GetValidNextIndex(TASKUTIL_QUEUE_t *pQueue, HINT32 lIndex, HINT32 *plNextIndex);
static HERROR				local_taskutil_GetValidPrevIndex(TASKUTIL_QUEUE_t *pQueue, HINT32 lIndex, HINT32 *plPrevIndex);
#endif


static HERROR				local_taskutil_AllocTaskItem(HUINT32 ulMsgSize, void* pvTaskProc, void* pvMsgFree, HUINT8 *pucTaskSlot);
static HERROR				local_taskutil_CheckMsgNTaskStatus(HUINT8 ucTaskSlot);
static HERROR				local_taskutil_GetTaskItem(HUINT8 ucTaskSlot, TASKUTIL_ITEM_t *pstTaskItem);
static HERROR				local_taskutil_AddTaskItem(TASKUTIL_ITEM_t *pstTaskItem);
static HERROR				local_taskutil_Init(void);
static HERROR				local_taskutil_SendMessage(HUINT8 ucTaskSlot, void *pMsg);
static HERROR				local_taskutil_SendMessageTimeout(HUINT8 ucTaskSlot, HUINT32 ulMsTimeout, void *pMsg);
static HERROR				local_taskutil_GetMessageCount(HUINT8 ucTaskSlot, HUINT32 *pulMsgCount);
static HERROR				local_taskutil_Restart(HUINT8 ucTaskSlot);
static void				local_taskutil_TaskProc0(void *pParam);
static void				local_taskutil_TaskProc1(void *pParam);
static void				local_taskutil_TaskProc2(void *pParam);
static void				local_taskutil_TaskProc3(void *pParam);
static void				local_taskutil_TaskProc4(void *pParam);
static void				local_taskutil_TaskProc5(void *pParam);
static void				local_taskutil_TaskProc6(void *pParam);
static void				local_taskutil_TaskProc7(void *pParam);
static void				local_taskutil_TaskProc8(void *pParam);
static void				local_taskutil_TaskProc9(void *pParam);
static void				local_taskutil_TaskProc10 (void *pParam);

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static TASKUTIL_ITEM_t		s_astTaskUtilPool[TASKUTIL_POOL_SIZE] = {
						{	TASKUTIL_STATUS_NONE,		0,		0,		0,		0,	NULL,	NULL,	local_taskutil_TaskProc0	},
						{	TASKUTIL_STATUS_NONE,		0,		0,		0,		0,	NULL,	NULL,	local_taskutil_TaskProc1	},
						{	TASKUTIL_STATUS_NONE,		0,		0,		0,		0,	NULL,	NULL,	local_taskutil_TaskProc2	},
						{	TASKUTIL_STATUS_NONE,		0,		0,		0,		0,	NULL,	NULL,	local_taskutil_TaskProc3	},
						{	TASKUTIL_STATUS_NONE,		0,		0,		0,		0,	NULL,	NULL,	local_taskutil_TaskProc4	},
						{	TASKUTIL_STATUS_NONE,		0,		0,		0,		0,	NULL,	NULL,	local_taskutil_TaskProc5	},
						{	TASKUTIL_STATUS_NONE,		0,		0,		0,		0,	NULL,	NULL,	local_taskutil_TaskProc6	},
						{	TASKUTIL_STATUS_NONE,		0,		0,		0,		0,	NULL,	NULL,	local_taskutil_TaskProc7	},
						{	TASKUTIL_STATUS_NONE,		0,		0,		0,		0,	NULL,	NULL,	local_taskutil_TaskProc8	},
						{	TASKUTIL_STATUS_NONE,		0,		0,		0,		0,	NULL,	NULL,	local_taskutil_TaskProc9	},
						{	TASKUTIL_STATUS_NONE,		0,		0,		0,		0,	NULL,	NULL,	local_taskutil_TaskProc10	},
};


#define	ENTER_CRITICAL_TASKUTIL(i)								\
{																\
	VK_SEM_Get(s_astTaskUtilPool[i].ulSemId);					\
}

#define	LEAVE_CRITICAL_TASKUTIL(i)								\
{																\
	VK_SEM_Release(s_astTaskUtilPool[i].ulSemId);				\
}




HERROR			UTIL_TASK_AllocTaskItem(HUINT32 ulMsgSize, void* pvTaskProc, void* pvMsgFree, HUINT8 *pucTaskSlot)
{
	return local_taskutil_AllocTaskItem(ulMsgSize, pvTaskProc, pvMsgFree, pucTaskSlot);
}


HERROR			UTIL_TASK_CheckMsgNTaskStatus(HUINT8 ucTaskSlot)
{

	return local_taskutil_CheckMsgNTaskStatus(ucTaskSlot);
}


HERROR			UTIL_TASK_GetTaskItem(HUINT8 ucTaskSlot, TASKUTIL_ITEM_t *pstTaskItem)
{
	return local_taskutil_GetTaskItem(ucTaskSlot, pstTaskItem);
}


HERROR			UTIL_TASK_AddTaskItem(TASKUTIL_ITEM_t *pstTaskItem)
{
	return local_taskutil_AddTaskItem(pstTaskItem);
}


HERROR			UTIL_TASK_Init(void)
{
	return local_taskutil_Init();
}

HERROR			UTIL_TASK_SendMessage(HUINT8 ucTaskSlot, void *pMsg)
{
	return local_taskutil_SendMessage(ucTaskSlot, pMsg);
}

HERROR			UTIL_TASK_SendMessageTimeout(HUINT8 ucTaskSlot, HUINT32 ulMsTimeout, void *pMsg)
{
	return local_taskutil_SendMessageTimeout(ucTaskSlot, ulMsTimeout, pMsg);
}

HERROR			UTIL_TASK_GetMessageCount(HUINT8 ucTaskSlot, HUINT32 *pulMsgCount)
{
	return local_taskutil_GetMessageCount(ucTaskSlot, pulMsgCount);
}



#define ____LOCAL_FUNC____
static HERROR				local_taskutil_AllocTaskItem(HUINT32 ulMsgSize, void* pvTaskProc, void* pvMsgFree, HUINT8 *pucTaskSlot)
{
	HERROR				hError = ERR_TASKUTIL_FAIL;
	HUINT8				ucIndex = 0;
	HINT32				lResult = 0;
	HUINT8				aucNameBuffer[20];
	HUINT32				ulMsgQueueId = 0;


	if (pvTaskProc == NULL || /*pvMsgFree == NULL ||*/ pucTaskSlot == NULL)
	{
		HxLOG_Print("[local_taskutil_AllocTaskItem] param fail!!\n");
		return hError;
	}


	for(ucIndex = 0; ucIndex < TASKUTIL_POOL_SIZE; ucIndex++)
	{
		if(s_astTaskUtilPool[ucIndex].ulItemStatus == TASKUTIL_STATUS_NOTUSED)
		{
			HxLOG_Print("[local_taskutil_AllocTaskItem] not TASKUTIL_STATUS_USED !!\n");
			break;
		}
	}
	if(ucIndex >= TASKUTIL_POOL_SIZE)
	{
		HxLOG_Print("[local_taskutil_AllocTaskItem] taskutil full !!\n");
		return hError;
	}

	ENTER_CRITICAL_TASKUTIL(ucIndex);

	snprintf((char*)aucNameBuffer, 20, MSGQUEUE_TASKUTIL_NAME, ucIndex);
	aucNameBuffer[19] = '\0';

	lResult = VK_MSG_Create(UTIL_TASKUTIL_MSGQ_SIZE,
							(unsigned long)ulMsgSize,
							(const char*)aucNameBuffer,
							(unsigned long*)&ulMsgQueueId,
							VK_SUSPENDTYPE_PRIORITY);
	if (lResult != VK_OK)
	{
		HxLOG_Print("[local_taskutil_AllocTaskItem] MSG_Create fail!!\n");
		LEAVE_CRITICAL_TASKUTIL(ucIndex);
		goto TASKUTIL_FUNC_EXIT;
	}

	s_astTaskUtilPool[ucIndex].ulItemStatus = TASKUTIL_STATUS_START;
	s_astTaskUtilPool[ucIndex].pfnTaskProc = (TASK_FUNC)pvTaskProc;
	s_astTaskUtilPool[ucIndex].pfnMsgFree= (TASK_FUNC)pvMsgFree;
	s_astTaskUtilPool[ucIndex].ulMsgSize = ulMsgSize;
	s_astTaskUtilPool[ucIndex].ulMessageQId = ulMsgQueueId;

	VK_TASK_Start(s_astTaskUtilPool[ucIndex].ulTaskId);

	*pucTaskSlot = ucIndex;

	LEAVE_CRITICAL_TASKUTIL(ucIndex);

	hError = ERR_TASKUTIL_OK;

TASKUTIL_FUNC_EXIT :

	return hError;

}


static HERROR				local_taskutil_CheckMsgNTaskStatus(HUINT8 ucTaskSlot)
{
#if 0
	HERROR				hError = ERR_TASKUTIL_FAIL;
	HUINT32				ulMsgCount = 0;
	HUINT32				ulQueSize = 0;
	HINT32				lResult = 0;


	ENTER_CRITICAL_TASKUTIL(ucTaskSlot);
	if (ucTaskSlot >= TASKUTIL_POOL_SIZE)
	{
		HxLOG_Print("[local_taskutil_CheckMsgNTaskStatus] param fail!!\n");
		goto TASKUTIL_FUNC_EXIT;
	}

	lResult = VK_MSG_GetStatus((unsigned long)s_astTaskUtilPool[ucTaskSlot].ulMessageQId, (unsigned long*)&ulMsgCount, (unsigned long*)&ulQueSize);
	if(lResult != VK_OK)
	{
		HxLOG_Print("[local_taskutil_CheckMsgNTaskStatus] VK_MSG_GetStatus fail!!\n");
		goto TASKUTIL_FUNC_EXIT;
	}

	if(ulMsgCount != 0)
	{
		HxLOG_Print("[local_taskutil_CheckMsgNTaskStatus] MessageQ not empty!!\n");
		goto TASKUTIL_FUNC_EXIT;
	}

	lResult = VK_MSG_Destroy(s_astTaskUtilPool[ucTaskSlot].ulMessageQId);
	if(lResult != VK_OK)
	{
		HxLOG_Print("[local_taskutil_CheckMsgNTaskStatus] VK_MSG_Destroy fail!!\n");
		goto TASKUTIL_FUNC_EXIT;
	}

	s_astTaskUtilPool[ucTaskSlot].ulMessageQId = 0;
	s_astTaskUtilPool[ucTaskSlot].ulMsgSize= 0;
	s_astTaskUtilPool[ucTaskSlot].ulItemStatus = TASKUTIL_STATUS_NOTUSED;
	s_astTaskUtilPool[ucTaskSlot].pfnTaskProc = NULL;
	s_astTaskUtilPool[ucTaskSlot].pfnMsgFree = NULL;

#if defined(WIN32)
	VK_TASK_Stop(s_astTaskUtilPool[ucTaskSlot].ulTaskId);
#endif
	hError = ERR_TASKUTIL_OK;

TASKUTIL_FUNC_EXIT :
	LEAVE_CRITICAL_TASKUTIL(ucTaskSlot);

	return hError;
#endif

	return ERR_OK;
}


static HERROR				local_taskutil_GetTaskItem(HUINT8 ucTaskSlot, TASKUTIL_ITEM_t *pstTaskItem)
{
	HERROR				hError = ERR_TASKUTIL_FAIL;

	if (pstTaskItem == NULL || ucTaskSlot >= TASKUTIL_POOL_SIZE)
	{
		HxLOG_Print("[local_taskutil_GetTaskItem] param fail!!\n");
		return hError;
	}

	ENTER_CRITICAL_TASKUTIL(ucTaskSlot);

	VK_memcpy32(pstTaskItem, &s_astTaskUtilPool[ucTaskSlot], sizeof(TASKUTIL_ITEM_t));

	hError = ERR_TASKUTIL_OK;

	LEAVE_CRITICAL_TASKUTIL(ucTaskSlot);

	return hError;

}



static HERROR				local_taskutil_SendMessage(HUINT8 ucTaskSlot, void *pMsg)
{
	HERROR				hError = ERR_TASKUTIL_FAIL;
	TASKUTIL_ITEM_t 		stTaskItem;
	HINT32				lResult = 0;

	if(pMsg == NULL)
	{
		HxLOG_Print("[local_taskutil_SendMessage] param fail!!\n");
		goto TASKUTIL_FUNC_EXIT;
	}
	hError = local_taskutil_GetTaskItem(ucTaskSlot, &stTaskItem);
	if(hError != ERR_TASKUTIL_OK)
	{
		HxLOG_Print("[local_taskutil_SendMessage] local_taskutil_GetTaskItem fail!!\n");
		goto TASKUTIL_FUNC_EXIT;
	}

	lResult = VK_MSG_Send(stTaskItem.ulMessageQId, (const void *)pMsg, stTaskItem.ulMsgSize);
	if(lResult != VK_OK)
	{
		HxLOG_Print("[local_taskutil_SendMessage] VK_MSG_Send fail!!\n");
		goto TASKUTIL_FUNC_EXIT;
	}

	hError = local_taskutil_Restart(ucTaskSlot);
	if(hError != ERR_OK)
	{
		HxLOG_Print("[local_taskutil_SendMessage] local_taskutil_Restart fail!!\n");
		goto TASKUTIL_FUNC_EXIT;
	}

	hError = ERR_TASKUTIL_OK;

TASKUTIL_FUNC_EXIT :

	return hError;

}



static HERROR				local_taskutil_SendMessageTimeout(HUINT8 ucTaskSlot, HUINT32 ulMsTimeout, void *pMsg)
{
	HERROR				hError = ERR_TASKUTIL_FAIL;
	TASKUTIL_ITEM_t 		stTaskItem;
	HINT32				lResult = 0;

	if(pMsg == NULL)
	{
		HxLOG_Print("[local_taskutil_SendMessage] param fail!!\n");
		goto TASKUTIL_FUNC_EXIT;
	}
	hError = local_taskutil_GetTaskItem(ucTaskSlot, &stTaskItem);
	if(hError != ERR_TASKUTIL_OK)
	{
		HxLOG_Print("[local_taskutil_SendMessage] local_taskutil_GetTaskItem fail!!\n");
		goto TASKUTIL_FUNC_EXIT;
	}

	lResult = VK_MSG_SendTimeout(stTaskItem.ulMessageQId, (const void *)pMsg, stTaskItem.ulMsgSize, ulMsTimeout);
	if(lResult != VK_OK)
	{
		HxLOG_Print("[local_taskutil_SendMessage] VK_MSG_Send fail!!\n");
		goto TASKUTIL_FUNC_EXIT;
	}

	hError = local_taskutil_Restart(ucTaskSlot);
	if(hError != ERR_OK)
	{
		HxLOG_Print("[local_taskutil_SendMessage] local_taskutil_Restart fail!!\n");
		goto TASKUTIL_FUNC_EXIT;
	}

	hError = ERR_TASKUTIL_OK;

TASKUTIL_FUNC_EXIT :

	return hError;

}



static HERROR				local_taskutil_GetMessageCount(HUINT8 ucTaskSlot, HUINT32 *pulMsgCount)
{
	HERROR				hError = ERR_TASKUTIL_FAIL;
	TASKUTIL_ITEM_t 		stTaskItem = {0, };
	HINT32				lResult = 0;
	HUINT32				ulMsgCount = 0;
	HUINT32				ulMaxMsg = 0;

	if(pulMsgCount == NULL)
	{
		HxLOG_Print("[local_taskutil_GetMessageCount] param fail!!\n");
		goto TASKUTIL_FUNC_EXIT;
	}
	*pulMsgCount = 0;

	hError = local_taskutil_GetTaskItem(ucTaskSlot, &stTaskItem);
	if(hError != ERR_TASKUTIL_OK)
	{
		HxLOG_Print("[local_taskutil_GetMessageCount] local_taskutil_GetTaskItem fail!!\n");
		goto TASKUTIL_FUNC_EXIT;
	}

	lResult = VK_MSG_GetMessageCount(stTaskItem.ulMessageQId, (unsigned long*)&ulMsgCount,(unsigned long*)&ulMaxMsg);
	if(lResult != VK_OK)
	{
		HxLOG_Print("[local_taskutil_GetMessageCount] VK_MSG_GetMessageCount fail!!\n");
		goto TASKUTIL_FUNC_EXIT;
	}

	*pulMsgCount = ulMsgCount;

	hError = ERR_TASKUTIL_OK;

TASKUTIL_FUNC_EXIT :

	return hError;

}




static HERROR				local_taskutil_Restart(HUINT8 ucTaskSlot)
{
	HERROR				hError = ERR_TASKUTIL_FAIL;

	if(ucTaskSlot >= TASKUTIL_POOL_SIZE)
	{
		HxLOG_Print("[local_taskutil_SetUsed] param fail!!\n");
		return hError;
	}

	ENTER_CRITICAL_TASKUTIL(ucTaskSlot);

	s_astTaskUtilPool[ucTaskSlot].ulItemStatus = TASKUTIL_STATUS_START;

	hError = ERR_TASKUTIL_OK;

	LEAVE_CRITICAL_TASKUTIL(ucTaskSlot);

	return hError;

}



static HERROR				local_taskutil_AddTaskItem(TASKUTIL_ITEM_t *pstTaskItem)
{
	HERROR				hError = ERR_TASKUTIL_FAIL;
	HUINT16				usIndex = 0;

	if (pstTaskItem == NULL)
	{
		HxLOG_Print("[local_taskutil_AddTaskItem] Param null!!\n");
		goto TASKUTIL_FUNC_EXIT;
	}

	for(usIndex = 0; usIndex < TASKUTIL_POOL_SIZE; usIndex++)
	{
		if(s_astTaskUtilPool[usIndex].ulItemStatus == TASKUTIL_STATUS_NONE)
		{
			VK_memcpy32(&s_astTaskUtilPool[usIndex], pstTaskItem, sizeof(TASKUTIL_ITEM_t));
			break;
		}
	}

	hError = ERR_TASKUTIL_OK;
	if(usIndex >= TASKUTIL_POOL_SIZE)
	{
		hError = ERR_TASKUTIL_FAIL;
	}

TASKUTIL_FUNC_EXIT :

	return hError;

}



static HERROR				local_taskutil_Init(void)
{
	HERROR				hError = ERR_TASKUTIL_FAIL;
	HINT32				lResult = 0;
	HUINT32				ulSemId = 0;
	HUINT32				ulTaskId = 0;
	HUINT8				ucIndex = 0;
	HUINT8				aucNameBuffer[20];



	if(s_astTaskUtilPool[0].ulItemStatus != TASKUTIL_STATUS_NONE)
	{
		hError = ERR_TASKUTIL_OK;

		HxLOG_Print("[local_taskutil_init] not TASKUTIL_STATUS_NONE!!\n");
		goto TASKUTIL_FUNC_EXIT;
	}

	for(ucIndex = 0; ucIndex < TASKUTIL_POOL_SIZE; ucIndex++)
	{
		snprintf((char*)aucNameBuffer, 20, SEM_TASKUTIL_NAME, ucIndex);
		aucNameBuffer[19] = '\0';

		lResult = VK_SEM_Create((unsigned long*)(&ulSemId), (const char*)aucNameBuffer, VK_SUSPENDTYPE_PRIORITY);
		if(lResult != VK_OK)
		{
			HxLOG_Print("[local_taskutil_init] VK_SEM_Create fail!!\n");
			goto TASKUTIL_FUNC_EXIT;
		}

		snprintf((char*)aucNameBuffer, 20, TASK_TASKUTIL_NAME, ucIndex);
		lResult = VK_TASK_Create(	s_astTaskUtilPool[ucIndex].pfnTaskStartFunc,
									(unsigned long)UTIL_TASKUTIL_TASK_PRIORITY,
									(unsigned long)UTIL_TASKUTIL_TASK_STACK_SIZE,
									(const char*)aucNameBuffer,
									NULL,
									(unsigned long*)(&ulTaskId),
									0);
		if (lResult != VK_OK)
		{
			HxLOG_Error("[local_taskutil_init] VK_TASK_Create fail!!\n");
			goto TASKUTIL_FUNC_EXIT;
		}

		s_astTaskUtilPool[ucIndex].ulItemStatus = TASKUTIL_STATUS_NOTUSED;
		s_astTaskUtilPool[ucIndex].ulSemId= ulSemId;
		s_astTaskUtilPool[ucIndex].ulTaskId = ulTaskId;
		s_astTaskUtilPool[ucIndex].pfnTaskProc = NULL;
		s_astTaskUtilPool[ucIndex].pfnMsgFree = NULL;

	}

	hError = ERR_TASKUTIL_OK;

TASKUTIL_FUNC_EXIT :


	return hError;


}



static void				local_taskutil_TaskProc0(void *pParam)
{
	HERROR				hError = ERR_TASKUTIL_FAIL;
	HINT32				lResult = 0;
	TASKUTIL_ITEM_t		stTaskInfo;
	HUINT8				aucMsg[TASKUTIL_MAX_MSG_LEN];

#if	defined(CONFIG_ENFORCE_SLEEP)
	static HUINT32		s_ulTask0ProcDuration = 0;
	static HUINT32		s_ulTask0ProcStartTime = 0;
	static HUINT32		s_ulTask0CurrentTick = 0;
#endif

	while (1)
	{
		hError = local_taskutil_GetTaskItem(0, &stTaskInfo);
		if(hError != ERR_TASKUTIL_OK)
		{
			HxLOG_Print("[local_taskutil_TaskProc0] local_taskutil_GetTaskItem fail!!\n");
			goto TASKUTIL_LOOP_EXIT;
		}
		if(stTaskInfo.ulItemStatus == TASKUTIL_STATUS_NOTUSED)
		{
			HxLOG_Print("[local_taskutil_TaskProc0] TASKUTIL_STATUS_NOTUSEDl!!\n");
			goto TASKUTIL_LOOP_EXIT;
		}
		VK_memset32(aucMsg, 0x00, sizeof(HUINT8)*TASKUTIL_MAX_MSG_LEN);
		lResult = VK_MSG_Receive (stTaskInfo.ulMessageQId, aucMsg, stTaskInfo.ulMsgSize);
		if (lResult != VK_OK)
		{
			HxLOG_Print("[local_taskutil_TaskProc0] VK_MSG_Receive fail!!\n");
			goto TASKUTIL_LOOP_EXIT;
		}

#if		defined(CONFIG_ENFORCE_SLEEP)
		s_ulTask0CurrentTick = VK_TIMER_GetSystemTick();
		s_ulTask0ProcDuration = s_ulTask0CurrentTick - s_ulTask0ProcStartTime ;
		if(s_ulTask0ProcDuration > TASKUTIL_PROC_HIGH_DURATION && s_ulTask0ProcDuration <= TASKUTIL_TASK_MAX_WAITTIME)
		{
			VK_TASK_Sleep(TASKUTIL_TASKSLEEP_HIGH_DURATION);
			s_ulTask0ProcStartTime = VK_TIMER_GetSystemTick();
		}
		else if(s_ulTask0ProcDuration > TASKUTIL_TASK_MAX_WAITTIME)
		{
			s_ulTask0ProcStartTime = VK_TIMER_GetSystemTick();
		}
#endif

		switch(stTaskInfo.ulItemStatus)
		{
			case TASKUTIL_STATUS_START :
				if(stTaskInfo.pfnTaskProc == NULL)
				{
					HxLOG_Print("[local_taskutil_TaskProc0] pfnTaskProc NULL!!\n");
					goto TASKUTIL_LOOP_EXIT;
				}
				hError = stTaskInfo.pfnTaskProc((HUINT32)aucMsg, (HUINT32)0);
				if(hError != ERR_OK)
				{
					HxLOG_Print("[local_taskutil_TaskProc0] pfnTaskProc fail!!\n");
					goto TASKUTIL_LOOP_EXIT;
				}
				break;

		}
TASKUTIL_LOOP_EXIT :
		continue;

	}

	return;
}



static void				local_taskutil_TaskProc1(void *pParam)
{
	HERROR				hError = ERR_TASKUTIL_FAIL;
	HINT32				lResult = 0;
	TASKUTIL_ITEM_t		stTaskInfo;
	HUINT8				aucMsg[TASKUTIL_MAX_MSG_LEN];

#if	defined(CONFIG_ENFORCE_SLEEP)
	static HUINT32		s_ulTask1ProcDuration = 0;
	static HUINT32		s_ulTask1ProcStartTime = 0;
	static HUINT32		s_ulTask1CurrentTick = 0;
#endif

	while (1)
	{
		hError = local_taskutil_GetTaskItem(1, &stTaskInfo);
		if(hError != ERR_TASKUTIL_OK)
		{
			HxLOG_Print("[local_taskutil_TaskProc1] local_taskutil_GetTaskItem fail!!\n");
			goto TASKUTIL_LOOP_EXIT;
		}
		if(stTaskInfo.ulItemStatus == TASKUTIL_STATUS_NOTUSED)
		{
			HxLOG_Print("[local_taskutil_TaskProc1] TASKUTIL_STATUS_NOTUSEDl!!\n");
			goto TASKUTIL_LOOP_EXIT;
		}
		VK_memset32(aucMsg, 0x00, sizeof(HUINT8)*TASKUTIL_MAX_MSG_LEN);
		lResult = VK_MSG_Receive (stTaskInfo.ulMessageQId, aucMsg, stTaskInfo.ulMsgSize);
		if (lResult != VK_OK)
		{
			HxLOG_Print("[local_taskutil_TaskProc1] VK_MSG_Receive fail!!\n");
			goto TASKUTIL_LOOP_EXIT;
		}

#if		defined(CONFIG_ENFORCE_SLEEP)
		s_ulTask1CurrentTick = VK_TIMER_GetSystemTick();
		s_ulTask1ProcDuration = s_ulTask1CurrentTick - s_ulTask1ProcStartTime ;
		if(s_ulTask1ProcDuration > TASKUTIL_PROC_HIGH_DURATION && s_ulTask1ProcDuration <= TASKUTIL_TASK_MAX_WAITTIME)
		{
			VK_TASK_Sleep(TASKUTIL_TASKSLEEP_HIGH_DURATION);
			s_ulTask1ProcStartTime = VK_TIMER_GetSystemTick();
		}
		else if(s_ulTask1ProcDuration > TASKUTIL_TASK_MAX_WAITTIME)
		{
			s_ulTask1ProcStartTime = VK_TIMER_GetSystemTick();
		}
#endif

		switch(stTaskInfo.ulItemStatus)
		{
			case TASKUTIL_STATUS_START :
				if(stTaskInfo.pfnTaskProc == NULL)
				{
					HxLOG_Print("[local_taskutil_TaskProc1] pfnTaskProc NULL!!\n");
					goto TASKUTIL_LOOP_EXIT;
				}
				hError = stTaskInfo.pfnTaskProc((HUINT32)aucMsg, (HUINT32)0);
				if(hError != ERR_OK)
				{
					HxLOG_Print("[local_taskutil_TaskProc1] pfnTaskProc fail!!\n");
					goto TASKUTIL_LOOP_EXIT;
				}
				break;
		}

TASKUTIL_LOOP_EXIT :
		continue;

	}


	return;

}





static void				local_taskutil_TaskProc2(void *pParam)
{
	HERROR				hError = ERR_TASKUTIL_FAIL;
	HINT32				lResult = 0;
	TASKUTIL_ITEM_t		stTaskInfo;
	HUINT8				aucMsg[TASKUTIL_MAX_MSG_LEN];

#if	defined(CONFIG_ENFORCE_SLEEP)
	static HUINT32		s_ulTask2ProcDuration = 0;
	static HUINT32		s_ulTask2ProcStartTime = 0;
	static HUINT32		s_ulTask2CurrentTick = 0;
#endif

	while (1)
	{

		hError = local_taskutil_GetTaskItem(2, &stTaskInfo);
		if(hError != ERR_TASKUTIL_OK)
		{
			HxLOG_Print("[local_taskutil_TaskProc2] local_taskutil_GetTaskItem fail!!\n");
			goto TASKUTIL_LOOP_EXIT;
		}

		if(stTaskInfo.ulItemStatus == TASKUTIL_STATUS_NOTUSED)
		{
			HxLOG_Print("[local_taskutil_TaskProc2] TASKUTIL_STATUS_NOTUSEDl!!\n");
			goto TASKUTIL_LOOP_EXIT;
		}

		VK_memset32(aucMsg, 0x00, sizeof(HUINT8)*TASKUTIL_MAX_MSG_LEN);
//		HxLOG_Print("[local_taskutil_TaskProc2] VK_MSG_Receive + (%d)\n", VK_TIMER_GetSystemTick()%10000);
		lResult = VK_MSG_Receive (stTaskInfo.ulMessageQId, aucMsg, stTaskInfo.ulMsgSize);
		if (lResult != VK_OK)
		{
			HxLOG_Print("[local_taskutil_TaskProc2] VK_MSG_Receive fail!!\n");
			goto TASKUTIL_LOOP_EXIT;
		}
//		HxLOG_Print("[local_taskutil_TaskProc2] VK_MSG_Receive - (%d)\n", VK_TIMER_GetSystemTick()%10000);

#if		defined(CONFIG_ENFORCE_SLEEP)
		s_ulTask2CurrentTick = VK_TIMER_GetSystemTick();
		s_ulTask2ProcDuration = s_ulTask2CurrentTick - s_ulTask2ProcStartTime ;
		if(s_ulTask2ProcDuration > TASKUTIL_PROC_HIGH_DURATION  && s_ulTask2ProcDuration <= TASKUTIL_TASK_MAX_WAITTIME)
		{
			VK_TASK_Sleep(TASKUTIL_TASKSLEEP_HIGH_DURATION);
			s_ulTask2ProcStartTime = VK_TIMER_GetSystemTick();
		}
		else if(s_ulTask2ProcDuration > TASKUTIL_TASK_MAX_WAITTIME)
		{
			s_ulTask2ProcStartTime = VK_TIMER_GetSystemTick();
		}
#endif

		switch(stTaskInfo.ulItemStatus)
		{
			case TASKUTIL_STATUS_START :
				if(stTaskInfo.pfnTaskProc == NULL)
				{
					HxLOG_Print("[local_taskutil_TaskProc2] pfnTaskProc NULL!!\n");
					goto TASKUTIL_LOOP_EXIT;
				}
				hError = stTaskInfo.pfnTaskProc((HUINT32)aucMsg, (HUINT32)0);
				if(hError != ERR_OK)
				{
					HxLOG_Print("[local_taskutil_TaskProc2] pfnTaskProc fail!!\n");
					goto TASKUTIL_LOOP_EXIT;
				}
				break;

		}

TASKUTIL_LOOP_EXIT :
		continue;


	}

	return;

}


static void				local_taskutil_TaskProc3(void *pParam)
{
	HERROR				hError = ERR_TASKUTIL_FAIL;
	HINT32				lResult = 0;
	TASKUTIL_ITEM_t		stTaskInfo;
	HUINT8				aucMsg[TASKUTIL_MAX_MSG_LEN];

#if	defined(CONFIG_ENFORCE_SLEEP)
	static HUINT32		s_ulTask3ProcDuration = 0;
	static HUINT32		s_ulTask3ProcStartTime = 0;
	static HUINT32		s_ulTask3CurrentTick = 0;
#endif

	while (1)
	{

		hError = local_taskutil_GetTaskItem(3, &stTaskInfo);
		if(hError != ERR_TASKUTIL_OK)
		{
			HxLOG_Print("[local_taskutil_TaskProc3] local_taskutil_GetTaskItem fail!!\n");
			goto TASKUTIL_LOOP_EXIT;
		}

		if(stTaskInfo.ulItemStatus == TASKUTIL_STATUS_NOTUSED)
		{
			HxLOG_Print("[local_taskutil_TaskProc3] TASKUTIL_STATUS_NOTUSEDl!!\n");
			goto TASKUTIL_LOOP_EXIT;
		}

		VK_memset32(aucMsg, 0x00, sizeof(HUINT8)*TASKUTIL_MAX_MSG_LEN);
//		HxLOG_Print("[local_taskutil_TaskProc3] VK_MSG_Receive + (%d)\n", VK_TIMER_GetSystemTick()%10000);
		lResult = VK_MSG_Receive (stTaskInfo.ulMessageQId, aucMsg, stTaskInfo.ulMsgSize);
		if (lResult != VK_OK)
		{
			HxLOG_Print("[local_taskutil_TaskProc3] VK_MSG_Receive fail!!\n");
			goto TASKUTIL_LOOP_EXIT;
		}
//		HxLOG_Print("[local_taskutil_TaskProc3] VK_MSG_Receive - (%d)\n", VK_TIMER_GetSystemTick()%10000);

#if		defined(CONFIG_ENFORCE_SLEEP)
		s_ulTask3CurrentTick = VK_TIMER_GetSystemTick();
		s_ulTask3ProcDuration = s_ulTask3CurrentTick - s_ulTask3ProcStartTime ;
		if(s_ulTask3ProcDuration > TASKUTIL_PROC_HIGH_DURATION   && s_ulTask3ProcDuration <= TASKUTIL_TASK_MAX_WAITTIME)
		{
			VK_TASK_Sleep(TASKUTIL_TASKSLEEP_HIGH_DURATION);
			s_ulTask3ProcStartTime = VK_TIMER_GetSystemTick();
		}
		else if(s_ulTask3ProcDuration > TASKUTIL_TASK_MAX_WAITTIME)
		{
			s_ulTask3ProcStartTime = VK_TIMER_GetSystemTick();
		}
#endif

		switch(stTaskInfo.ulItemStatus)
		{
			case TASKUTIL_STATUS_START :
				if(stTaskInfo.pfnTaskProc == NULL)
				{
					HxLOG_Print("[local_taskutil_TaskProc3] pfnTaskProc NULL!!\n");
					goto TASKUTIL_LOOP_EXIT;
				}
//				HxLOG_Print("[local_taskutil_TaskProc3] pfnTaskProc + (%d)\n", VK_TIMER_GetSystemTick()%10000);
				hError = stTaskInfo.pfnTaskProc((HUINT32)aucMsg, (HUINT32)0);
				if(hError != ERR_OK)
				{
					HxLOG_Print("[local_taskutil_TaskProc3] pfnTaskProc fail!!\n");
					goto TASKUTIL_LOOP_EXIT;
				}
//				HxLOG_Print("[local_taskutil_TaskProc3] pfnTaskProc - (%d)\n", VK_TIMER_GetSystemTick()%10000);
				break;
		}

TASKUTIL_LOOP_EXIT :
		continue;


	}


	return;

}



static void				local_taskutil_TaskProc4(void *pParam)
{
	HERROR				hError = ERR_TASKUTIL_FAIL;
	HINT32				lResult = 0;
	TASKUTIL_ITEM_t		stTaskInfo;
	HUINT8				aucMsg[TASKUTIL_MAX_MSG_LEN];

#if	defined(CONFIG_ENFORCE_SLEEP)
	static HUINT32		s_ulTask4ProcDuration = 0;
	static HUINT32		s_ulTask4ProcStartTime = 0;
	static HUINT32		s_ulTask4CurrentTick = 0;
#endif

	while (1)
	{

		hError = local_taskutil_GetTaskItem(4, &stTaskInfo);
		if(hError != ERR_TASKUTIL_OK)
		{
			HxLOG_Print("[local_taskutil_TaskProc4] local_taskutil_GetTaskItem fail!!\n");
			goto TASKUTIL_LOOP_EXIT;
		}

		if(stTaskInfo.ulItemStatus == TASKUTIL_STATUS_NOTUSED)
		{
			HxLOG_Print("[local_taskutil_TaskProc4] TASKUTIL_STATUS_NOTUSEDl!!\n");
			goto TASKUTIL_LOOP_EXIT;
		}

		VK_memset32(aucMsg, 0x00, sizeof(HUINT8)*TASKUTIL_MAX_MSG_LEN);
		lResult = VK_MSG_Receive (stTaskInfo.ulMessageQId, aucMsg, stTaskInfo.ulMsgSize);
		if (lResult != VK_OK)
		{
			HxLOG_Print("[local_taskutil_TaskProc4] VK_MSG_Receive fail!!\n");
			goto TASKUTIL_LOOP_EXIT;
		}

#if		defined(CONFIG_ENFORCE_SLEEP)
		s_ulTask4CurrentTick = VK_TIMER_GetSystemTick();
		s_ulTask4ProcDuration = s_ulTask4CurrentTick - s_ulTask4ProcStartTime ;
		if(s_ulTask4ProcDuration > TASKUTIL_PROC_HIGH_DURATION  && s_ulTask4ProcDuration <= TASKUTIL_TASK_MAX_WAITTIME)
		{
			VK_TASK_Sleep(TASKUTIL_TASKSLEEP_HIGH_DURATION);
			s_ulTask4ProcStartTime = VK_TIMER_GetSystemTick();
		}
		else if(s_ulTask4ProcDuration > TASKUTIL_TASK_MAX_WAITTIME)
		{
			s_ulTask4ProcStartTime = VK_TIMER_GetSystemTick();
		}
#endif

		switch(stTaskInfo.ulItemStatus)
		{
			case TASKUTIL_STATUS_START :
				if(stTaskInfo.pfnTaskProc == NULL)
				{
					HxLOG_Print("[local_taskutil_TaskProc4] pfnTaskProc NULL!!\n");
					goto TASKUTIL_LOOP_EXIT;
				}
				hError = stTaskInfo.pfnTaskProc((HUINT32)aucMsg, (HUINT32)0);
				if(hError != ERR_OK)
				{
					HxLOG_Print("[local_taskutil_TaskProc4] pfnTaskProc fail!!\n");
					goto TASKUTIL_LOOP_EXIT;
				}
				break;
		}


TASKUTIL_LOOP_EXIT :
		continue;

	}

	return;

}


static void				local_taskutil_TaskProc5(void *pParam)
{
	HERROR				hError = ERR_TASKUTIL_FAIL;
	HINT32				lResult = 0;
	TASKUTIL_ITEM_t		stTaskInfo;
	HUINT8				*pucMsg;

#if	defined(CONFIG_ENFORCE_SLEEP)
	static HUINT32		s_ulTask5ProcDuration = 0;
	static HUINT32		s_ulTask5ProcStartTime = 0;
	static HUINT32		s_ulTask5CurrentTick = 0;
#endif

	while (1)
	{
		pucMsg = NULL;
		hError = local_taskutil_GetTaskItem(5, &stTaskInfo);
		if(hError != ERR_TASKUTIL_OK)
		{
			HxLOG_Print("[local_taskutil_TaskProc5] local_taskutil_GetTaskItem fail!!\n");
			goto TASKUTIL_LOOP_EXIT;
		}

		if(stTaskInfo.ulItemStatus == TASKUTIL_STATUS_NOTUSED)
		{
			HxLOG_Print("[local_taskutil_TaskProc5] TASKUTIL_STATUS_NOTUSEDl!!\n");
			goto TASKUTIL_LOOP_EXIT;
		}

		pucMsg = (HUINT8 *)OxMW_Malloc(stTaskInfo.ulMsgSize);
		VK_memset32(pucMsg, 0x00, stTaskInfo.ulMsgSize);
		lResult = VK_MSG_Receive (stTaskInfo.ulMessageQId, pucMsg, stTaskInfo.ulMsgSize);
		if (lResult != VK_OK)
		{
			HxLOG_Print("[local_taskutil_TaskProc5] VK_MSG_Receive fail!!\n");
			goto TASKUTIL_LOOP_EXIT;
		}

#if		defined(CONFIG_ENFORCE_SLEEP)
		s_ulTask5CurrentTick = VK_TIMER_GetSystemTick();
		s_ulTask5ProcDuration = s_ulTask5CurrentTick - s_ulTask5ProcStartTime ;
		if(s_ulTask5ProcDuration > TASKUTIL_PROC_HIGH_DURATION  && s_ulTask5ProcDuration <= TASKUTIL_TASK_MAX_WAITTIME)
		{
			VK_TASK_Sleep(TASKUTIL_TASKSLEEP_HIGH_DURATION);
			s_ulTask5ProcStartTime = VK_TIMER_GetSystemTick();
		}
		else if(s_ulTask5ProcDuration > TASKUTIL_TASK_MAX_WAITTIME)
		{
			s_ulTask5ProcStartTime = VK_TIMER_GetSystemTick();
		}
#endif

		switch(stTaskInfo.ulItemStatus)
		{
			case TASKUTIL_STATUS_START :
				if(stTaskInfo.pfnTaskProc == NULL)
				{
					HxLOG_Print("[local_taskutil_TaskProc5] pfnTaskProc NULL!!\n");
					goto TASKUTIL_LOOP_EXIT;
				}
				hError = stTaskInfo.pfnTaskProc((HUINT32)pucMsg, (HUINT32)0);
				if(hError != ERR_OK)
				{
					HxLOG_Print("[local_taskutil_TaskProc5] pfnTaskProc fail!!\n");
					goto TASKUTIL_LOOP_EXIT;
				}
				break;
		}


TASKUTIL_LOOP_EXIT :
		if(pucMsg)
			OxMW_Free(pucMsg);
		continue;

	}

	return;

}




static void				local_taskutil_TaskProc6(void *pParam)
{
	HERROR				hError = ERR_TASKUTIL_FAIL;
	HINT32				lResult = 0;
	TASKUTIL_ITEM_t		stTaskInfo;
	HUINT8				*pucMsg;

#if	defined(CONFIG_ENFORCE_SLEEP)
	static HUINT32		s_ulTask6ProcDuration = 0;
	static HUINT32		s_ulTask6ProcStartTime = 0;
	static HUINT32		s_ulTask6CurrentTick = 0;
#endif

	while (1)
	{
		pucMsg = NULL;
		hError = local_taskutil_GetTaskItem(6, &stTaskInfo);
		if(hError != ERR_TASKUTIL_OK)
		{
			HxLOG_Print("[local_taskutil_TaskProc6] local_taskutil_GetTaskItem fail!!\n");
			goto TASKUTIL_LOOP_EXIT;
		}

		if(stTaskInfo.ulItemStatus == TASKUTIL_STATUS_NOTUSED)
		{
			HxLOG_Print("[local_taskutil_TaskProc6] TASKUTIL_STATUS_NOTUSEDl!!\n");
			goto TASKUTIL_LOOP_EXIT;
		}

		pucMsg = (HUINT8 *)OxMW_Malloc(stTaskInfo.ulMsgSize);
		VK_memset32(pucMsg, 0x00, stTaskInfo.ulMsgSize);
		lResult = VK_MSG_Receive (stTaskInfo.ulMessageQId, pucMsg, stTaskInfo.ulMsgSize);
		if (lResult != VK_OK)
		{
			HxLOG_Print("[local_taskutil_TaskProc6] VK_MSG_Receive fail!!\n");
			goto TASKUTIL_LOOP_EXIT;
		}

#if		defined(CONFIG_ENFORCE_SLEEP)
		s_ulTask6CurrentTick = VK_TIMER_GetSystemTick();
		s_ulTask6ProcDuration = s_ulTask6CurrentTick - s_ulTask6ProcStartTime ;
		if(s_ulTask6ProcDuration > TASKUTIL_PROC_HIGH_DURATION  && s_ulTask6ProcDuration <= TASKUTIL_TASK_MAX_WAITTIME)
		{
			VK_TASK_Sleep(TASKUTIL_TASKSLEEP_HIGH_DURATION);
			s_ulTask6ProcStartTime = VK_TIMER_GetSystemTick();
		}
		else if(s_ulTask6ProcDuration > TASKUTIL_TASK_MAX_WAITTIME)
		{
			s_ulTask6ProcStartTime = VK_TIMER_GetSystemTick();
		}
#endif

		switch(stTaskInfo.ulItemStatus)
		{
			case TASKUTIL_STATUS_START :
				if(stTaskInfo.pfnTaskProc == NULL)
				{
					HxLOG_Print("[local_taskutil_TaskProc6] pfnTaskProc NULL!!\n");
					goto TASKUTIL_LOOP_EXIT;
				}
				hError = stTaskInfo.pfnTaskProc((HUINT32)pucMsg, (HUINT32)0);
				if(hError != ERR_OK)
				{
					HxLOG_Print("[local_taskutil_TaskProc6] pfnTaskProc fail!!\n");
					goto TASKUTIL_LOOP_EXIT;
				}
				break;
		}


TASKUTIL_LOOP_EXIT :
		if(pucMsg)
			OxMW_Free(pucMsg);
		continue;

	}

	return;

}



static void				local_taskutil_TaskProc7(void *pParam)
{
	HERROR				hError = ERR_TASKUTIL_FAIL;
	HINT32				lResult = 0;
	TASKUTIL_ITEM_t		stTaskInfo;
	HUINT8				*pucMsg;

#if	defined(CONFIG_ENFORCE_SLEEP)
	static HUINT32		s_ulTask7ProcDuration = 0;
	static HUINT32		s_ulTask7ProcStartTime = 0;
	static HUINT32		s_ulTask7CurrentTick = 0;
#endif

	while (1)
	{
		pucMsg = NULL;
		hError = local_taskutil_GetTaskItem(7, &stTaskInfo);
		if(hError != ERR_TASKUTIL_OK)
		{
			HxLOG_Print("[local_taskutil_TaskProc7] local_taskutil_GetTaskItem fail!!\n");
			goto TASKUTIL_LOOP_EXIT;
		}

		if(stTaskInfo.ulItemStatus == TASKUTIL_STATUS_NOTUSED)
		{
			HxLOG_Print("[local_taskutil_TaskProc7] TASKUTIL_STATUS_NOTUSEDl!!\n");
			goto TASKUTIL_LOOP_EXIT;
		}

		pucMsg = (HUINT8 *)OxMW_Malloc(stTaskInfo.ulMsgSize);
		VK_memset32(pucMsg, 0x00, stTaskInfo.ulMsgSize);
		lResult = VK_MSG_Receive (stTaskInfo.ulMessageQId, pucMsg, stTaskInfo.ulMsgSize);
		if (lResult != VK_OK)
		{
			HxLOG_Print("[local_taskutil_TaskProc7] VK_MSG_Receive fail!!\n");
			goto TASKUTIL_LOOP_EXIT;
		}

#if		defined(CONFIG_ENFORCE_SLEEP)
		s_ulTask7CurrentTick = VK_TIMER_GetSystemTick();
		s_ulTask7ProcDuration = s_ulTask7CurrentTick - s_ulTask7ProcStartTime ;
		if(s_ulTask7ProcDuration > TASKUTIL_PROC_HIGH_DURATION  && s_ulTask7ProcDuration <= TASKUTIL_TASK_MAX_WAITTIME)
		{
			VK_TASK_Sleep(TASKUTIL_TASKSLEEP_HIGH_DURATION);
			s_ulTask7ProcStartTime = VK_TIMER_GetSystemTick();
		}
		else if(s_ulTask7ProcDuration > TASKUTIL_TASK_MAX_WAITTIME)
		{
			s_ulTask7ProcStartTime = VK_TIMER_GetSystemTick();
		}
#endif

		switch(stTaskInfo.ulItemStatus)
		{
			case TASKUTIL_STATUS_START :
				if(stTaskInfo.pfnTaskProc == NULL)
				{
					HxLOG_Print("[local_taskutil_TaskProc7] pfnTaskProc NULL!!\n");
					goto TASKUTIL_LOOP_EXIT;
				}
				hError = stTaskInfo.pfnTaskProc((HUINT32)pucMsg, (HUINT32)0);
				if(hError != ERR_OK)
				{
					HxLOG_Print("[local_taskutil_TaskProc7] pfnTaskProc fail!!\n");
					goto TASKUTIL_LOOP_EXIT;
				}
				break;
		}


TASKUTIL_LOOP_EXIT :
		if(pucMsg)
			OxMW_Free(pucMsg);
		continue;

	}


	return;

}




static void				local_taskutil_TaskProc8(void *pParam)
{
	HERROR				hError = ERR_TASKUTIL_FAIL;
	HINT32				lResult = 0;
	TASKUTIL_ITEM_t		stTaskInfo;
	HUINT8				*pucMsg;

#if	defined(CONFIG_ENFORCE_SLEEP)
	static HUINT32		s_ulTask8ProcDuration = 0;
	static HUINT32		s_ulTask8ProcStartTime = 0;
	static HUINT32		s_ulTask8CurrentTick = 0;
#endif

	while (1)
	{
		pucMsg = NULL;
		hError = local_taskutil_GetTaskItem(8, &stTaskInfo);
		if(hError != ERR_TASKUTIL_OK)
		{
			HxLOG_Print("[local_taskutil_TaskProc8] local_taskutil_GetTaskItem fail!!\n");
			goto TASKUTIL_LOOP_EXIT;
		}

		if(stTaskInfo.ulItemStatus == TASKUTIL_STATUS_NOTUSED)
		{
			HxLOG_Print("[local_taskutil_TaskProc8] TASKUTIL_STATUS_NOTUSEDl!!\n");
			goto TASKUTIL_LOOP_EXIT;
		}

		pucMsg = (HUINT8 *)OxMW_Malloc(stTaskInfo.ulMsgSize);
		VK_memset32(pucMsg, 0x00, stTaskInfo.ulMsgSize);
		lResult = VK_MSG_Receive (stTaskInfo.ulMessageQId, pucMsg, stTaskInfo.ulMsgSize);
		if (lResult != VK_OK)
		{
			HxLOG_Print("[local_taskutil_TaskProc8] VK_MSG_Receive fail!!\n");
			goto TASKUTIL_LOOP_EXIT;
		}

#if		defined(CONFIG_ENFORCE_SLEEP)
		s_ulTask8CurrentTick = VK_TIMER_GetSystemTick();
		s_ulTask8ProcDuration = s_ulTask8CurrentTick - s_ulTask8ProcStartTime ;
		if(s_ulTask8ProcDuration > TASKUTIL_PROC_HIGH_DURATION  && s_ulTask8ProcDuration <= TASKUTIL_TASK_MAX_WAITTIME)
		{
			VK_TASK_Sleep(TASKUTIL_TASKSLEEP_HIGH_DURATION);
			s_ulTask8ProcStartTime = VK_TIMER_GetSystemTick();
		}
		else if(s_ulTask8ProcDuration > TASKUTIL_TASK_MAX_WAITTIME)
		{
			s_ulTask8ProcStartTime = VK_TIMER_GetSystemTick();
		}
#endif

		switch(stTaskInfo.ulItemStatus)
		{
			case TASKUTIL_STATUS_START :
				if(stTaskInfo.pfnTaskProc == NULL)
				{
					HxLOG_Print("[local_taskutil_TaskProc8] pfnTaskProc NULL!!\n");
					goto TASKUTIL_LOOP_EXIT;
				}
				hError = stTaskInfo.pfnTaskProc((HUINT32)pucMsg, (HUINT32)0);
				if(hError != ERR_OK)
				{
					HxLOG_Print("[local_taskutil_TaskProc8] pfnTaskProc fail!!\n");
					goto TASKUTIL_LOOP_EXIT;
				}
				break;
		}


TASKUTIL_LOOP_EXIT :
		if(pucMsg)
			OxMW_Free(pucMsg);
		continue;

	}

	return;

}



static void				local_taskutil_TaskProc9(void *pParam)
{
	HERROR				hError = ERR_TASKUTIL_FAIL;
	HINT32				lResult = 0;
	TASKUTIL_ITEM_t		stTaskInfo;
	HUINT8				*pucMsg;

#if	defined(CONFIG_ENFORCE_SLEEP)
	static HUINT32		s_ulTask9ProcDuration = 0;
	static HUINT32		s_ulTask9ProcStartTime = 0;
	static HUINT32		s_ulTask9CurrentTick = 0;
#endif

	while (1)
	{
		pucMsg = NULL;
		hError = local_taskutil_GetTaskItem(9, &stTaskInfo);
		if(hError != ERR_TASKUTIL_OK)
		{
			HxLOG_Print("[local_taskutil_TaskProc9] local_taskutil_GetTaskItem fail!!\n");
			goto TASKUTIL_LOOP_EXIT;
		}

		if(stTaskInfo.ulItemStatus == TASKUTIL_STATUS_NOTUSED)
		{
			HxLOG_Print("[local_taskutil_TaskProc9] TASKUTIL_STATUS_NOTUSEDl!!\n");
			goto TASKUTIL_LOOP_EXIT;
		}

		pucMsg = (HUINT8 *)OxMW_Malloc(stTaskInfo.ulMsgSize);
		VK_memset32(pucMsg, 0x00, stTaskInfo.ulMsgSize);
		lResult = VK_MSG_Receive (stTaskInfo.ulMessageQId, pucMsg, stTaskInfo.ulMsgSize);
		if (lResult != VK_OK)
		{
			HxLOG_Print("[local_taskutil_TaskProc9] VK_MSG_Receive fail!!\n");
			goto TASKUTIL_LOOP_EXIT;
		}

#if		defined(CONFIG_ENFORCE_SLEEP)
		s_ulTask9CurrentTick = VK_TIMER_GetSystemTick();
		s_ulTask9ProcDuration = s_ulTask9CurrentTick - s_ulTask9ProcStartTime ;
		if(s_ulTask9ProcDuration > TASKUTIL_PROC_HIGH_DURATION  && s_ulTask9ProcDuration <= TASKUTIL_TASK_MAX_WAITTIME)
		{
			VK_TASK_Sleep(TASKUTIL_TASKSLEEP_HIGH_DURATION);
			s_ulTask9ProcStartTime = VK_TIMER_GetSystemTick();
		}
		else if(s_ulTask9ProcDuration > TASKUTIL_TASK_MAX_WAITTIME)
		{
			s_ulTask9ProcStartTime = VK_TIMER_GetSystemTick();
		}
#endif

		switch(stTaskInfo.ulItemStatus)
		{
			case TASKUTIL_STATUS_START :
				if(stTaskInfo.pfnTaskProc == NULL)
				{
					HxLOG_Print("[local_taskutil_TaskProc9] pfnTaskProc NULL!!\n");
					goto TASKUTIL_LOOP_EXIT;
				}
				hError = stTaskInfo.pfnTaskProc((HUINT32)pucMsg, (HUINT32)0);
				if(hError != ERR_OK)
				{
					HxLOG_Print("[local_taskutil_TaskProc9] pfnTaskProc fail!!\n");
					goto TASKUTIL_LOOP_EXIT;
				}
				break;
		}


TASKUTIL_LOOP_EXIT :
		if(pucMsg)
			OxMW_Free(pucMsg);
		continue;

	}

	return;

}

static void				local_taskutil_TaskProc10 (void *pParam)
{
	HERROR				hError = ERR_TASKUTIL_FAIL;
	HINT32				lResult = 0;
	TASKUTIL_ITEM_t		stTaskInfo;
	HUINT8				*pucMsg;

#if	defined(CONFIG_ENFORCE_SLEEP)
	static HUINT32		s_ulTask10ProcDuration = 0;
	static HUINT32		s_ulTask10ProcStartTime = 0;
	static HUINT32		s_ulTask10CurrentTick = 0;
#endif

	while (1)
	{
		pucMsg = NULL;
		hError = local_taskutil_GetTaskItem(9, &stTaskInfo);
		if(hError != ERR_TASKUTIL_OK)
		{
			HxLOG_Print("[local_taskutil_TaskProc10] local_taskutil_GetTaskItem fail!!\n");
			goto TASKUTIL_LOOP_EXIT;
		}

		if(stTaskInfo.ulItemStatus == TASKUTIL_STATUS_NOTUSED)
		{
			HxLOG_Print("[local_taskutil_TaskProc10] TASKUTIL_STATUS_NOTUSEDl!!\n");
			goto TASKUTIL_LOOP_EXIT;
		}

		pucMsg = (HUINT8 *)OxMW_Malloc(stTaskInfo.ulMsgSize);
		VK_memset32(pucMsg, 0x00, stTaskInfo.ulMsgSize);
		lResult = VK_MSG_Receive (stTaskInfo.ulMessageQId, pucMsg, stTaskInfo.ulMsgSize);
		if (lResult != VK_OK)
		{
			HxLOG_Print("[local_taskutil_TaskProc10] VK_MSG_Receive fail!!\n");
			goto TASKUTIL_LOOP_EXIT;
		}

#if		defined(CONFIG_ENFORCE_SLEEP)
		s_ulTask10CurrentTick = VK_TIMER_GetSystemTick();
		s_ulTask10ProcDuration = s_ulTask10CurrentTick - s_ulTask10ProcStartTime ;
		if(s_ulTask10ProcDuration > TASKUTIL_PROC_HIGH_DURATION  && s_ulTask10ProcDuration <= TASKUTIL_TASK_MAX_WAITTIME)
		{
			VK_TASK_Sleep(TASKUTIL_TASKSLEEP_HIGH_DURATION);
			s_ulTask10ProcStartTime = VK_TIMER_GetSystemTick();
		}
		else if(s_ulTask10ProcDuration > TASKUTIL_TASK_MAX_WAITTIME)
		{
			s_ulTask10ProcStartTime = VK_TIMER_GetSystemTick();
		}
#endif

		switch(stTaskInfo.ulItemStatus)
		{
			case TASKUTIL_STATUS_START :
				if(stTaskInfo.pfnTaskProc == NULL)
				{
					HxLOG_Print("[local_taskutil_TaskProc10] pfnTaskProc NULL!!\n");
					goto TASKUTIL_LOOP_EXIT;
				}
				hError = stTaskInfo.pfnTaskProc((HUINT32)pucMsg, (HUINT32)0);
				if(hError != ERR_OK)
				{
					HxLOG_Print("[local_taskutil_TaskProc10] pfnTaskProc fail!!\n");
					goto TASKUTIL_LOOP_EXIT;
				}
				break;
		}


TASKUTIL_LOOP_EXIT :
		if(pucMsg)
			OxMW_Free(pucMsg);
		continue;

	}

	return;

}



#if defined(CONFIG_UTILTASK_Q)
#define ____QUEUE_FUNC____

static HERROR				local_taskutil_CreateQueue(TASKUTIL_QUEUE_t **ppQueue, HINT32 lQueueSize, HUINT32 ulMode)
{
	HERROR				hError = ERR_TASKUTIL_FAIL;
	TASKUTIL_QUEUE_t	*pTempQ = NULL;
	unsigned long			ulSemId = 0;


	if(ppQueue == NULL)
	{
		HxLOG_Print("[local_taskutil_CreateQueue] param fail!!\n");
		goto TASKUTIL_FUNC_EXIT;
	}

	pTempQ = (TASKUTIL_QUEUE_t*)OxMW_Malloc(sizeof(TASKUTIL_QUEUE_t));
	if(pTempQ == NULL)
	{
		HxLOG_Print("[local_taskutil_CreateQueue] OxMW_Malloc fail!!\n");
		goto TASKUTIL_FUNC_EXIT;
	}

	VK_memset32(pTempQ, 0x00, sizeof(TASKUTIL_QUEUE_t));
	pTempQ->aItem = (QITEM*)OxMW_Malloc(sizeof(QITEM)*lQueueSize);
	if(pTempQ->aItem == NULL)
	{
		HxLOG_Print("[local_taskutil_CreateQueue] OxMW_Malloc fail!!\n");
		goto TASKUTIL_FUNC_EXIT;
	}
	VK_memset32(pTempQ->aItem, 0x00, sizeof(QITEM)*lQueueSize);

	hError = VK_SEM_Create((unsigned long*)(&ulSemId), (const char*)"taskutil_queue_sem", VK_SUSPENDTYPE_PRIORITY);
	if(hError != VK_OK)
	{
		HxLOG_Print("[local_taskutil_CreateQueue] VK_SEM_Create fail!!\n");
		goto TASKUTIL_FUNC_EXIT;
	}

	pTempQ->ulSemId = ulSemId;
	pTempQ->lLast = lQueueSize-1;
	pTempQ->lQueueSize = lQueueSize;
	pTempQ->lQueueMode = ulMode;
	pTempQ->pfnDataFree = NULL;
	pTempQ->pfnDataCopy = NULL;

	*ppQueue = pTempQ;

	hError = ERR_TASKUTIL_OK;

TASKUTIL_FUNC_EXIT :

	return hError;
}


static HERROR				local_taskutil_DestroyQueue(TASKUTIL_QUEUE_t *pQueue)
{
	HERROR				hError = ERR_TASKUTIL_FAIL;
	HINT16				sIndex = 0;
	QITEM				pData = NULL;

	if(pQueue == NULL)
	{
		HxLOG_Print("[local_taskutil_DestroyQueue] OxMW_Malloc fail!!\n");
		goto TASKUTIL_FUNC_EXIT;
	}

	VK_SEM_Destroy(pQueue->ulSemId);

	if(pQueue->aItem)
	{
		for(sIndex = 0; sIndex < pQueue->lQueueSize; sIndex++ )
		{
			pData = NULL;
			pData = pQueue->aItem[sIndex];
			if(pData)
			{
				if(pQueue->pfnDataFree)
				{
					pQueue->pfnDataFree(pData);
				}
				else
				{
					OxMW_Free(pData);
				}
			}
		}
		OxMW_Free(pQueue->aItem);
	}

	OxMW_Free(pQueue);

	hError = ERR_TASKUTIL_OK;

TASKUTIL_FUNC_EXIT :

	return hError;

}


static HERROR				local_taskutil_PopQueue(TASKUTIL_QUEUE_t *pQueue, HBOOL bCopy, void** ppData)
{
	HERROR				hError = ERR_TASKUTIL_FAIL;
	HBOOL				bEmpty = FALSE;
	HINT32				lNextIndex = 0;

	if(pQueue == NULL || ppData == NULL)
	{
		HxLOG_Print("[local_taskutil_PopQueue] param fail!!\n");
		return hError;
	}
	*ppData = NULL;

	VK_SEM_Get(pQueue->ulSemId);

	hError = local_taskutil_IsEmptyQueue(pQueue, &bEmpty);
	if(hError != ERR_OK || bEmpty == TRUE)
	{
		HxLOG_Print("[local_taskutil_PopQueue] local_taskutil_IsEmptyQueue fail!!\n");
		hError = ERR_TASKUTIL_Q_EMPTY;
		goto TASKUTIL_FUNC_EXIT;
	}

	if(pQueue->aItem[pQueue->lFirst])
	{
		if(pQueue->pfnDataCopy && bCopy)
		{
			pQueue->pfnDataCopy(*ppData, pQueue->aItem[pQueue->lFirst]);
		}
		else
		{
			*ppData =pQueue->aItem[pQueue->lFirst];
		}

		if(pQueue->pfnDataFree)
		{
			pQueue->pfnDataFree(pQueue->aItem[pQueue->lFirst]);
		}
		pQueue->aItem[pQueue->lFirst] = NULL;
	}

	hError = local_taskutil_GetNextQueueIndex(pQueue, pQueue->lFirst, &lNextIndex);
	if(hError != ERR_OK)
	{
		HxLOG_Print("[local_taskutil_PopQueue] local_taskutil_GetNextQueueIndex lFirst(%d)lNextIndex(%d) fail!!\n", pQueue->lFirst, lNextIndex);
	}

	pQueue->lFirst = lNextIndex;
	pQueue->lCount--;

	hError = ERR_TASKUTIL_OK;

TASKUTIL_FUNC_EXIT :
	VK_SEM_Release(pQueue->ulSemId);


	return hError;

}

static HERROR				local_taskutil_PushQueue(TASKUTIL_QUEUE_t *pQueue, void* pData)
{
	HERROR				hError = ERR_TASKUTIL_FAIL;
//	HBOOL				bEmpty = FALSE;
	HBOOL				bFull = FALSE;
	HINT32				lNextIndex = 0;
	void*				pTempData = NULL;


	if(pQueue == NULL || pData == NULL)
	{
		HxLOG_Print("[local_taskutil_PushQueue] param fail!!\n");
		return hError;
	}

	VK_SEM_Get(pQueue->ulSemId);

	hError = local_taskutil_IsFullQueue(pQueue, &bFull);
	if(bFull == TRUE)
	{
		HxLOG_Print("[local_taskutil_PushQueue] Queue Full!!\n");

		if(pQueue->lQueueMode & MODE_QUEUE_FULL_PASS)
		{
			// TODO: PASS MODE 경우 FREE CALLBACK을 반드시 등록해야 한다 !!
			local_taskutil_PopQueue(pQueue, FALSE, &pTempData);
		}
		else if(pQueue->lQueueMode & MODE_QUEUE_FULL_STOP)
		{
			hError = ERR_TASKUTIL_Q_FULL;
			goto TASKUTIL_FUNC_EXIT;
		}
	}

	hError = local_taskutil_GetNextQueueIndex(pQueue, pQueue->lLast, &lNextIndex);
	if(hError != ERR_OK)
	{
		HxLOG_Print("[local_taskutil_PushQueue] local_taskutil_IsEmptyQueue fail!!\n");
	}

	pQueue->lLast = lNextIndex;
	pQueue->aItem[lNextIndex] = (QITEM)pData;
	pQueue->lCount++;

	hError = ERR_TASKUTIL_OK;

TASKUTIL_FUNC_EXIT :
	VK_SEM_Release(pQueue->ulSemId);

	return hError;

}

static HERROR				local_taskutil_IsEmptyQueue(TASKUTIL_QUEUE_t *pQueue, HBOOL *pbEmpty)
{
	HERROR				hError = ERR_TASKUTIL_FAIL;

	if(pQueue == NULL ||pbEmpty == NULL)
	{
		HxLOG_Print("[local_taskutil_IsEmptyQueue] param fail!!\n");
		goto TASKUTIL_FUNC_EXIT;
	}

	if(pQueue->lCount == 0)
	{
		*pbEmpty = TRUE;
	}
	else
	{
		*pbEmpty = FALSE;
	}

	hError = ERR_TASKUTIL_OK;

TASKUTIL_FUNC_EXIT :

	return hError;

}


static HERROR				local_taskutil_IsFullQueue(TASKUTIL_QUEUE_t *pQueue, HBOOL *pbFull)
{
	HERROR				hError = ERR_TASKUTIL_FAIL;

	if(pQueue == NULL || pbFull == NULL)
	{
		HxLOG_Print("[local_taskutil_IsFullQueue] param fail!!\n");
		goto TASKUTIL_FUNC_EXIT;
	}


	if(pQueue->lCount == pQueue->lQueueSize)
	{
		*pbFull = TRUE;
	}
	else
	{
		*pbFull = FALSE;
	}

	hError = ERR_TASKUTIL_OK;

TASKUTIL_FUNC_EXIT :

	return hError;

}


static HERROR				local_taskutil_GetNextQueueIndex(TASKUTIL_QUEUE_t *pQueue, HINT32 lIndex, HINT32 *plNextIndex)
{
	HERROR				hError = ERR_TASKUTIL_FAIL;

	if(pQueue == NULL || plNextIndex == NULL)
	{
		HxLOG_Print("[local_taskutil_GetNextQueueIndex] param fail!!\n");
		goto TASKUTIL_FUNC_EXIT;
	}
	*plNextIndex = 0;

	*plNextIndex = ((lIndex + 1)%pQueue->lQueueSize);

	hError = ERR_TASKUTIL_OK;

TASKUTIL_FUNC_EXIT :

	return hError;
}


static HERROR				local_taskutil_MoveNextFocus(TASKUTIL_QUEUE_t *pQueue, void* pData, HBOOL bCopy)
{
	HERROR				hError = ERR_TASKUTIL_FAIL;
	HINT32				lNextIndex =0;
	HBOOL				bEmpty = 0;

	if(pQueue == NULL)
	{
		HxLOG_Print("[local_taskutil_MoveNextFocus] param fail!!\n");
		goto TASKUTIL_FUNC_EXIT;
	}

	hError = local_taskutil_IsEmptyQueue(pQueue, &bEmpty);
	if(hError != ERR_OK || bEmpty == TRUE)
	{
		HxLOG_Print("[local_taskutil_MoveNextFocus] local_taskutil_IsEmptyQueue fail!!\n");
		goto TASKUTIL_FUNC_EXIT;
	}

	local_taskutil_GetValidNextIndex(pQueue, pQueue->lFocus, &lNextIndex);
	pQueue->lFocus = lNextIndex;
	if(pQueue->pfnDataCopy && bCopy)
	{
		pQueue->pfnDataCopy(pData, pQueue->aItem[pQueue->lFocus]);
	}

	hError = ERR_TASKUTIL_OK;

TASKUTIL_FUNC_EXIT :

	return hError;
}


static HERROR				local_taskutil_MovePrevFocus(TASKUTIL_QUEUE_t *pQueue, void* pData, HBOOL bCopy)
{
	HERROR				hError = ERR_TASKUTIL_FAIL;
	HINT32				lPrevIndex =0;
	HBOOL				bEmpty = 0;

	if(pQueue == NULL)
	{
		HxLOG_Print("[local_taskutil_MovePrevFocus] param fail!!\n");
		goto TASKUTIL_FUNC_EXIT;
	}

	hError = local_taskutil_IsEmptyQueue(pQueue, &bEmpty);
	if(hError != ERR_OK || bEmpty == TRUE)
	{
		HxLOG_Print("[local_taskutil_MovePrevFocus] local_taskutil_IsEmptyQueue fail!!\n");
		goto TASKUTIL_FUNC_EXIT;
	}

	local_taskutil_GetValidPrevIndex(pQueue, pQueue->lFocus, &lPrevIndex);
	pQueue->lFocus = lPrevIndex;
	if(pQueue->pfnDataCopy && bCopy)
	{
		pQueue->pfnDataCopy(pData, pQueue->aItem[pQueue->lFocus]);
	}

	hError = ERR_TASKUTIL_OK;

TASKUTIL_FUNC_EXIT :

	return hError;

}




static HERROR				local_taskutil_GetValidNextIndex(TASKUTIL_QUEUE_t *pQueue, HINT32 lIndex, HINT32 *plNextIndex)
{
	HERROR				hError = ERR_TASKUTIL_FAIL;
	HINT32				lTempIndex = 0;

	if(pQueue == NULL || plNextIndex == NULL)
	{
		HxLOG_Print("[local_taskutil_GetValidNextIndex] param fail!!\n");
		goto TASKUTIL_FUNC_EXIT;
	}

	lTempIndex = ((lIndex + 1)%pQueue->lQueueSize);
	if(pQueue->aItem[lTempIndex] == NULL)
	{
		lTempIndex = pQueue->lFirst;
	}

	*plNextIndex = lTempIndex;

	hError = ERR_TASKUTIL_OK;

TASKUTIL_FUNC_EXIT :

	return hError;
}


static HERROR				local_taskutil_GetValidPrevIndex(TASKUTIL_QUEUE_t *pQueue, HINT32 lIndex, HINT32 *plPrevIndex)
{
	HERROR				hError = ERR_TASKUTIL_FAIL;
	HINT32				lTempIndex = 0;

	if(pQueue == NULL || plPrevIndex == NULL)
	{
		HxLOG_Print("[local_taskutil_GetValidPrevIndex] param fail!!\n");
		goto TASKUTIL_FUNC_EXIT;
	}

	lTempIndex = ((lIndex + 1)%pQueue->lQueueSize);

	if(pQueue->aItem[lTempIndex] == NULL)
	{
		lTempIndex = pQueue->lLast;
	}

	*plPrevIndex = lTempIndex;

	hError = ERR_TASKUTIL_OK;

TASKUTIL_FUNC_EXIT :

	return hError;
}
#endif


/*********************** End of File ******************************/
