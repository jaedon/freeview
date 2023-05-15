/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name을 적습니다.
	@brief	  file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__UTIL_TASK_H__
#define	__UTIL_TASK_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef	enum
{
	ERR_TASKUTIL_OK 					= 0,
	ERR_TASKUTIL_FAIL,
	ERR_TASKUTIL_Q_FULL,
	ERR_TASKUTIL_Q_EMPTY


}TASKUTIL_ERR;


typedef void			(*TASK_START_FUNC)(void*);
typedef HERROR		(*TASK_FUNC)(HUINT32 ulParam1, HUINT32 ulParam2);


typedef struct tagTaskUtilItem
{
	HUINT32				ulItemStatus;
	HUINT32				ulTaskId;
	HUINT32				ulMessageQId;
	HUINT32				ulMsgSize;
	HUINT32				ulSemId;
	TASK_FUNC			pfnTaskProc;
	TASK_FUNC			pfnMsgFree;
	TASK_START_FUNC	pfnTaskStartFunc;
}TASKUTIL_ITEM_t;



/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
HERROR			UTIL_TASK_Init(void);
HERROR			UTIL_TASK_AllocTaskItem(HUINT32 ulMsgSize, void* pvTaskProc, void* pvMsgFree, HUINT8 *pucTaskSlot);
HERROR			UTIL_TASK_SendMessage(HUINT8 ucTaskSlot, void *pMsg);
HERROR			UTIL_TASK_SendMessageTimeout(HUINT8 ucTaskSlot, HUINT32 ulMsTimeout, void *pMsg);
HERROR			UTIL_TASK_GetMessageCount(HUINT8 ucTaskSlot, HUINT32 *pulMsgCount);

/* not used at this time */
HERROR			UTIL_TASK_GetTaskItem(HUINT8 ucTaskSlot, TASKUTIL_ITEM_t *pstTaskItem);
HERROR			UTIL_TASK_AddTaskItem(TASKUTIL_ITEM_t *pstTaskItem);
HERROR			UTIL_TASK_CheckMsgNTaskStatus(HUINT8 ucTaskSlot);

#endif /* !__UTIL_TASK_H__ */
