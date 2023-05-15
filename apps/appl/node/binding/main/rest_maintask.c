/*******************************************************************
	File Description
********************************************************************/
/**
	@file	 rest_maintask.c
	@brief

	Description:  									\n
	Module: node/binding/datatype		\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
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
#include <hlib.h>
#include <dlib.h>
#include <vkernel.h>
#include <apk.h>
#include <rest_maintask.h>
#include <rest_datatype.h>

/*******************************************************************/
/********************      Local Definition         *************************/
/*******************************************************************/
#define REST_MSGQ_SIZE		64

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	void		*pfExecFunc;		// REST_CallBack_t
	void		*pvArgValue;
} REST_MSG_t;

typedef HERROR (*REST_CallBack_t) (void *pvArg);


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HULONG			 s_ulRestTask_MsgQid;
STATIC HUINT32			 s_ulRestTask_SyncSemId;
STATIC HxList_t			*s_pstRestTask_InstLink = NULL;

#define ___LOCAL_FUNCTIONS___
STATIC void rest_task_MainTask (void *pvArg)
{
	REST_MSG_t		 stMsg;
	HERROR			 hErr;

	while (1)
	{
		// APPKIT Dispatch
		while (APK_EVENT_Dispatch());

		HxSTD_MemSet(&stMsg, 0, sizeof(REST_MSG_t));

		// Command Dispatch
		hErr = HLIB_MSG_ReceiveTimeout(s_ulRestTask_MsgQid, &stMsg, sizeof(REST_MSG_t), 50);
		if ((ERR_OK == hErr) && (NULL != stMsg.pfExecFunc))
		{
			REST_CallBack_t		 pfExecFunc = (REST_CallBack_t)stMsg.pfExecFunc;

			hErr = pfExecFunc(stMsg.pvArgValue);
		}
	}
}

STATIC RestInstance_t *rest_task_FindInstance (HCHAR *szSessionId)
{
	HxList_t		*pstItem;

	for (pstItem = s_pstRestTask_InstLink; NULL != pstItem; pstItem = pstItem->next)
	{
		RestInstance_t		*pstInst = (RestInstance_t *)HLIB_LIST_Data(pstItem);

		if ((NULL != pstInst) && (HxSTD_StrNCmp(pstInst->szSessionId, szSessionId, REST_SESSION_ID_LENGTH) == 0))
		{
			return pstInst;
		}
	}

	return NULL;
}

STATIC RestInstance_t *rest_task_MakeInstance (HCHAR *szSessionId)
{
	RestInstance_t	*pstInst;

	pstInst = (RestInstance_t *)HLIB_MEM_Calloc(sizeof(RestInstance_t));
	if (NULL == pstInst)
	{
		HxLOG_Critical("Memory allocation failed:\n");
		return NULL;
	}

	HxSTD_StrNCpy(pstInst->szSessionId, szSessionId, REST_SESSION_ID_LENGTH);
	s_pstRestTask_InstLink = HLIB_LIST_Append(s_pstRestTask_InstLink, (void *)pstInst);

	return pstInst;
}

#define ___MEMBER_FUNCTIONS___
HERROR rest_task_Init (void)
{
	unsigned long	 ulTaskId;
	HERROR			 hErr;

	// Create the message queue
	VK_Init();

	hErr = HLIB_MSG_Create(REST_MSGQ_SIZE, sizeof(REST_MSG_t), "restQ", &s_ulRestTask_MsgQid, eHLIB_SUSPENDTYPE_FIFO);
	if (ERR_OK != hErr)
	{
		HxLOG_Critical("!!! HLIB_MSG_Create failed !!!\n");
		return ERR_FAIL;
	}

	// Create the main task
	hErr = VK_TASK_Create(rest_task_MainTask, 40, (64 * 1024), "rest_task", NULL, &ulTaskId, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Critical("!!! VK_TASK_Create failed !!!\n");
		return ERR_FAIL;
	}

	// Start the task
	hErr = VK_TASK_Start(ulTaskId);
	if (ERR_OK != hErr)
	{
		HxLOG_Critical("!!! VK_TASK_Start failed !!!\n");
		return ERR_FAIL;
	}

	// Make the semaphore for the sync functions:
	hErr = HxSEMT_Create(&s_ulRestTask_SyncSemId, "rest_sync", HxSEMT_FIFO);
	if (ERR_OK != hErr)
	{
		HxLOG_Critical("!!! HxSEMT_Create failed !!!\n");
		return ERR_FAIL;
	}

	HxSEMT_Get(s_ulRestTask_SyncSemId);

	return ERR_OK;
}

HERROR rest_task_RequestToExecute (HERROR (*pfExecFunc)(void *), void *pvArg)
{
	REST_MSG_t		 stMsg;
	HERROR			 hErr;

	stMsg.pfExecFunc = pfExecFunc;
	stMsg.pvArgValue = pvArg;

	hErr = HLIB_MSG_SendTimeout(s_ulRestTask_MsgQid, (const void *)&stMsg, (HULONG)sizeof(REST_MSG_t), 10);
	return (ERR_OK == hErr) ? ERR_OK : ERR_FAIL;
}

HERROR rest_task_SyncIn (void)
{
	HxSEMT_Get(s_ulRestTask_SyncSemId);
	return ERR_OK;
}

HERROR rest_task_SyncOut (void)
{
	HxSEMT_Release(s_ulRestTask_SyncSemId);
	return ERR_OK;
}

RestInstance_t *rest_task_GetInstance (HCHAR *szSessionId)
{
	RestInstance_t	*pstInst;

	if ((NULL == szSessionId) || ('\0' == szSessionId[0]))
	{
		return NULL;
	}

	pstInst = rest_task_FindInstance(szSessionId);
	if (NULL == pstInst)
	{
		pstInst = rest_task_MakeInstance(szSessionId);
	}

	return pstInst;
}

HERROR rest_task_CreateInstance (HCHAR *szSessionId)
{
	RestInstance_t	*pstInst;

	if ((NULL == szSessionId) || ('\0' == szSessionId[0]))
	{
		return ERR_FAIL;
	}

	pstInst = rest_task_FindInstance(szSessionId);
	if (NULL != pstInst)
	{
		HxLOG_Error("Already the session exists:\n");
		return ERR_FAIL;
	}

	pstInst = rest_task_MakeInstance(szSessionId);
	if (NULL == pstInst)
	{
		HxLOG_Error("rest_task_MakeInstance err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}


