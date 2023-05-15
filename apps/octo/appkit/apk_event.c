/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_event.c
	@brief

	Description:  									\n
	Module: APPKITv2			\n

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

#include <apk.h>
#include <_apk_int.h>

#include <hlib.h>
#define ___INTERNAL_STRUCTURES___

#define	MAX_EVENT_ARG_NUM		10
typedef struct
{
#ifdef	CONFIG_DEBUG
	const HCHAR			*pszFuncName;
	HINT32				nLine;
#endif

	ApkEventFreeFunc_t	pfnFreeFunc;

	HINT32				nArgc;
	void				*apArgV[MAX_EVENT_ARG_NUM];				//	apArgV[0] : callback
} apkEventInfo_t;

typedef struct
{
	HUINT32				ulSemId;
	HxList_t			*pstEventList;
	HINT32				nEventNo;
} apkEvent_t;

typedef HINT32	(*apkEventCallback0_t)		(void);
typedef HINT32	(*apkEventCallback_t)		(void *p1, ...);

#define ___INTERNAL_VARIABLES___

STATIC apkEvent_t	*s_pstApkEventInstance = NULL;

#define ___PRIVATE_FUNCTION___
STATIC apkEvent_t*	apk_event_getInstance(void)
{
	if (s_pstApkEventInstance == NULL)
	{
		HxLOG_Warning("s_pstApkEventInstance [%p]\n", s_pstApkEventInstance);

		s_pstApkEventInstance = (apkEvent_t*)APK_Calloc(sizeof(apkEvent_t));
		HxSEMT_Create(&s_pstApkEventInstance->ulSemId, "apkevtsem", 0);
	}

	HxSEMT_Get(s_pstApkEventInstance->ulSemId);

	return s_pstApkEventInstance;
}

#ifdef	CONFIG_DEBUG
STATIC void			apk_event_checkEventCount(apkEvent_t *event)
{
	if (event->nEventNo > 20)
		HxLOG_Warning("event count is [%d]\n", event->nEventNo);
}
#endif

STATIC void			apk_event_releaseInstance(apkEvent_t *event)
{
	HxSEMT_Release(event->ulSemId);
}

#define ___MEMBER_FUNCTION___

#define ___API_FUNCTION___
HERROR	APK_EVENT_Init(void)
{
	apk_event_releaseInstance(apk_event_getInstance());
	return ERR_OK;
}

HERROR	APK_EVENT_EmitV(const HCHAR *func, HINT32 line, APK_EVENT_EMIT_ARGS *pstArgs)
{
	HINT32		i;
	apkEvent_t		*pstEvent;
	apkEventInfo_t	*pstEventInfo;


	HxLOG_Assert(pstArgs->ulArgCount > 0);

	pstEvent = apk_event_getInstance();

	pstEventInfo = (apkEventInfo_t*)APK_Calloc(sizeof(apkEventInfo_t));
	pstEventInfo->pfnFreeFunc = pstArgs->pfnFreeFunc;
	for (i = 0; i < pstArgs->ulArgCount && i < MAX_EVENT_ARG_NUM; i++)
		pstEventInfo->apArgV[i] = pstArgs->apArgV[i];
	pstEventInfo->nArgc = pstArgs->ulArgCount;

	pstEvent->pstEventList = HLIB_LIST_Append(pstEvent->pstEventList, (void*)pstEventInfo);
	pstEvent->nEventNo++;

#ifdef	CONFIG_DEBUG
	pstEventInfo->pszFuncName = func;
	pstEventInfo->nLine       = line;
	apk_event_checkEventCount(pstEvent);
#else
	(void)func;
	(void)line;
#endif

	apk_event_releaseInstance(pstEvent);

	return ERR_OK;
}

HUINT32	APK_EVENT_Dispatch(void)
{
	HINT32			nRemained;
	apkEvent_t		*pstEvent;
	apkEventInfo_t	*pstEventInfo = NULL;
#ifdef CONFIG_DEBUG	
	unsigned long last_time, current_time;
#endif

	pstEvent = apk_event_getInstance();

	if (pstEvent->pstEventList == NULL)
	{
		apk_event_releaseInstance(pstEvent);

		return 0;
	}

	pstEventInfo = (apkEventInfo_t*)HLIB_LIST_Data(pstEvent->pstEventList);
	pstEvent->pstEventList = HLIB_LIST_Remove(pstEvent->pstEventList, (void*)pstEventInfo);
	nRemained = --pstEvent->nEventNo;

	apk_event_releaseInstance(pstEvent);

        if (  pstEventInfo  )
        {

        	HxLOG_Assert(pstEventInfo->nArgc <= 10);
        /* see 211 line */
#ifdef CONFIG_DEBUG	
        	last_time = VK_TIMER_GetSystemTick();
#endif
        		
            if (pstEventInfo->nArgc == 1)
        		((apkEventCallback0_t)pstEventInfo->apArgV[0])();
        	else if (pstEventInfo->nArgc == 2)
        		((apkEventCallback_t)pstEventInfo->apArgV[0])(pstEventInfo->apArgV[1]);
        	else if (pstEventInfo->nArgc == 3)
        		((apkEventCallback_t)pstEventInfo->apArgV[0])(pstEventInfo->apArgV[1], pstEventInfo->apArgV[2]);
        	else if (pstEventInfo->nArgc == 4)
        		((apkEventCallback_t)pstEventInfo->apArgV[0])(pstEventInfo->apArgV[1], pstEventInfo->apArgV[2], pstEventInfo->apArgV[3]);
        	else if (pstEventInfo->nArgc == 5)
        		((apkEventCallback_t)pstEventInfo->apArgV[0])(pstEventInfo->apArgV[1], pstEventInfo->apArgV[2], pstEventInfo->apArgV[3], pstEventInfo->apArgV[4]);
        	else if (pstEventInfo->nArgc == 6)
        		((apkEventCallback_t)pstEventInfo->apArgV[0])(pstEventInfo->apArgV[1], pstEventInfo->apArgV[2], pstEventInfo->apArgV[3], pstEventInfo->apArgV[4], pstEventInfo->apArgV[5]);
        	else if (pstEventInfo->nArgc == 7)
        		((apkEventCallback_t)pstEventInfo->apArgV[0])(pstEventInfo->apArgV[1], pstEventInfo->apArgV[2], pstEventInfo->apArgV[3], pstEventInfo->apArgV[4], pstEventInfo->apArgV[5], pstEventInfo->apArgV[6]);
        	else if (pstEventInfo->nArgc == 8)
        		((apkEventCallback_t)pstEventInfo->apArgV[0])(pstEventInfo->apArgV[1], pstEventInfo->apArgV[2], pstEventInfo->apArgV[3], pstEventInfo->apArgV[4], pstEventInfo->apArgV[5], pstEventInfo->apArgV[6], pstEventInfo->apArgV[7]);
        	else if (pstEventInfo->nArgc == 9)
        		((apkEventCallback_t)pstEventInfo->apArgV[0])(pstEventInfo->apArgV[1], pstEventInfo->apArgV[2], pstEventInfo->apArgV[3], pstEventInfo->apArgV[4], pstEventInfo->apArgV[5], pstEventInfo->apArgV[6], pstEventInfo->apArgV[7], pstEventInfo->apArgV[8]);
        	else if (pstEventInfo->nArgc == 10)
        		((apkEventCallback_t)pstEventInfo->apArgV[0])(pstEventInfo->apArgV[1], pstEventInfo->apArgV[2], pstEventInfo->apArgV[3], pstEventInfo->apArgV[4], pstEventInfo->apArgV[5], pstEventInfo->apArgV[6], pstEventInfo->apArgV[7], pstEventInfo->apArgV[8], pstEventInfo->apArgV[9]);

#ifdef CONFIG_DEBUG
            /* this is for the notification and warning */
            current_time = VK_TIMER_GetSystemTick();
            if ( current_time - last_time > 50 ) 
        	{
        	    HxLOG_Critical("#############################################################\n" );
        		HxLOG_Critical("#### WARN!!!!! [%s] function took %d ms, it can make blocking\n", pstEventInfo->pszFuncName, (current_time - last_time) );
        		HxLOG_Critical("#############################################################\n");
            }
#endif

        	if (pstEventInfo->pfnFreeFunc)
        		pstEventInfo->pfnFreeFunc(pstEventInfo->nArgc, pstEventInfo->apArgV);

        	APK_Free(pstEventInfo);
        }

	return nRemained;
}

/* end of file */

