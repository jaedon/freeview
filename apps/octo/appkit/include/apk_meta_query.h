/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_meta_query.h
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

#ifndef ___APK_META_QUERY_H___
#define ___APK_META_QUERY_H___
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


typedef enum
{
	eQUERY_TARGET_EPG,
	eQUERY_TARGET_VOD,

	eQUERY_TARGET_UNKNOWN
} APK_META_QUERY_Target_e;

typedef enum
{
	eQUERY_EVENT_NEW_VERSION,
	eQUERY_EVENT_MORE_DATA,
	eQUERY_EVENT_FINISHED,
	eQUERY_EVENT_INVALID,

	eQUERY_EVENT_UNKNOWN
} APK_META_QUERY_Event_e;

typedef enum
{
	eQUERY_FILTER_ALL,
	eQUERY_FILTER_PF,
	eQUERY_FILTER_SCH,
	eQUERY_FILTER_RF,
	eQUERY_FILTER_TVTV,
	eQUERY_FILTER_IP
} APK_META_QUERY_Filter_e;

typedef void *	APK_QUERY_t;
typedef struct _QSearch_t	* ApkMetaQHandle_t;

typedef void	(*APK_META_QUERY_Notifier_t) (ApkMetaQHandle_t self, HINT32 event, const HINT32 *args, void *userdata);
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

void	APK_META_QUERY_Init(HBOOL	bEnableNotifier);
void	APK_META_QUERY_DeInit(void);

void			APK_EVENT_Delete (DxEvent_t *event);
DxEvent_t *	APK_EVENT_Clone (DxEvent_t *event);
DxEvent_t *	APK_EVENT_Incref (DxEvent_t *event);

APK_QUERY_t		APK_META_QUERY_NewQuery (const HCHAR *field, const HCHAR *comparision, const HCHAR *value);
void				APK_META_QUERY_DeleteQuery (APK_QUERY_t q);
APK_QUERY_t		APK_META_QUERY_CloneQuery (const APK_QUERY_t q);

APK_QUERY_t		APK_META_QUERY_And (const APK_QUERY_t q1, const APK_QUERY_t q2);
APK_QUERY_t		APK_META_QUERY_Or  (const APK_QUERY_t q1, const APK_QUERY_t q2);
APK_QUERY_t		APK_META_QUERY_Not (const APK_QUERY_t q1);

ApkMetaQHandle_t		APK_META_QUERY_New (APK_META_QUERY_Target_e eTarget);
void				APK_META_QUERY_Delete (ApkMetaQHandle_t self);
HERROR			APK_META_QUERY_SetQuery (ApkMetaQHandle_t self, const APK_QUERY_t query);
APK_QUERY_t		APK_META_QUERY_GetQuery (ApkMetaQHandle_t self);
#ifdef CONFIG_FUNC_EPG_USE_SVCUID
HERROR			APK_META_QUERY_AddSvcConstraint (ApkMetaQHandle_t self, HINT32 svcuid);
#else
HERROR			APK_META_QUERY_AddSvcConstraint (ApkMetaQHandle_t self, HINT32 onid, HINT32 tsid, HINT32 svcid);
#endif
void			APK_META_QUERY_ClearSvcConstraint (ApkMetaQHandle_t self);
void			APK_META_QUERY_OrderBy (ApkMetaQHandle_t self, const HCHAR *field, HBOOL ascending);
void			APK_META_QUERY_SetFilter (ApkMetaQHandle_t self, APK_META_QUERY_Filter_e eFilter);


HERROR			APK_META_QUERY_Search (ApkMetaQHandle_t self, HBOOL direct);
HERROR			APK_META_QUERY_Abort (ApkMetaQHandle_t self);
HxVector_t *	APK_META_QUERY_Result (ApkMetaQHandle_t self);
void			APK_META_QUERY_SetNotifier (ApkMetaQHandle_t self, APK_META_QUERY_Notifier_t fnNotifier, void *userdata);
#ifdef CONFIG_FUNC_EPG_USE_SVCUID
HERROR			APK_META_QUERY_FindEventName(HUINT32 svcuid, HUINT32 startTime, HUINT32 endTime, HCHAR *name, HINT32 eventId);
DxEvent_t *		APK_META_QUERY_GetEvent (DxEPG_Type_e types, HUID uid, HxReturnFunc_t ret, void *userdata);
DxEvent_t *		APK_META_QUERY_GetEventWithIds (HINT32 svcuid, HINT32 eventid);
DxEvent_t *		APK_META_QUERY_GetIpEvent (HINT32 svcuid, HINT32 eventid);
HxVector_t * 	APK_META_QUERY_GetPF (HINT32 svcuid);
#else
DxEvent_t *		APK_META_QUERY_GetEvent (DxEPG_Type_e types, HINT32 onid, HINT32 tsid, HINT32 svcid, HINT32 eventid, HxReturnFunc_t ret, void *userdata);
DxEvent_t *		APK_META_QUERY_GetEventWithIds (HINT32 onid, HINT32 tsid, HINT32 svcid, HINT32 eventid);
DxEvent_t *		APK_META_QUERY_GetIpEvent (HINT32 onid, HINT32 tsid, HINT32 svcid, HINT32 eventid);
HxVector_t * 	APK_META_QUERY_GetPF (HINT32 onid, HINT32 tsid, HINT32 svcid);
#endif

// IP EPG Set Function	===> Only Use In IPEPG
HERROR 			APK_META_QUERY_SetIpEvent (HUID uid, void * data, HINT32 size);
HERROR 			APK_META_QUERY_ResetIpEvent (HUID uid);
HERROR 			APK_META_QUERY_ResetIpEventTable (void);
HERROR 			APK_META_QUERY_DeletIpEventByEndTime(HUINT32 endTime);
HERROR 			APK_META_QUERY_SetIpSvc (HUID uid, void * data, HINT32 size);
HERROR 			APK_META_QUERY_ResetIpSvc (HUID uid);
HERROR 			APK_META_QUERY_ResetIpSvcTable (void);
HERROR 			APK_META_QUERY_SetIpLogo (HUID uid, void * data, HINT32 size);
HERROR 			APK_META_QUERY_ResetIpLogo (HUID uid);
HERROR 			APK_META_QUERY_ResetIpLogoTable (void);
HERROR 			APK_META_QUERY_SyncIpEventTable (void);
HERROR 			APK_META_QUERY_SyncIpSvcTable (void);
HERROR 			APK_META_QUERY_SyncIpLogoTable (void);

HERROR			APK_META_QUERY_ResetTVTVEventTable (void);
HERROR			APK_META_QUERY_ResetTVTVSvcTable (void);
HERROR			APK_META_QUERY_SyncTVTVSvcTable (void);
HERROR			APK_META_QUERY_ResetFvcExpiryTable (void);
HERROR			APK_META_QUERY_SyncFvcExpiryTable (void);

#endif

/* end of file */
