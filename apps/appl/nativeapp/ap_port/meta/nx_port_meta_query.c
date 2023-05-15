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

/******************************************************************************/
/**
 * @file	  		nx_port_meta_query.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#include <nx_port.h>


#define ________________RELATED_META_QUERY_________________

NX_MetaQHandle_t	NX_PORT_META_QUERY_New (NX_META_QUERY_Target_e eTarget)
{
	APK_META_QUERY_Target_e eApkTarget = (APK_META_QUERY_Target_e)eTarget;
	NX_MetaQHandle_t hNxMetaHandle = (NX_MetaQHandle_t)APK_META_QUERY_New (eApkTarget);

	return hNxMetaHandle;
}

void 	NX_PORT_META_QUERY_Delete(NX_MetaQHandle_t self)
{
	APK_META_QUERY_Delete(self);
}

void	NX_PORT_META_QUERY_SetNotifier (NX_MetaQHandle_t self, NX_PORT_META_QUERY_Notifier_t fnNotifier, void *userdata)
{
	ApkMetaQHandle_t	cvSelf = (ApkMetaQHandle_t)self;
	APK_META_QUERY_Notifier_t cvFnNotifier = (APK_META_QUERY_Notifier_t)fnNotifier;
	APK_META_QUERY_SetNotifier(cvSelf, cvFnNotifier, userdata);
}

NX_QUERY_t	NX_PORT_META_QUERY_NewQuery (const HCHAR *field, const HCHAR *comparison, const HCHAR *value)
{
	NX_QUERY_t retQuery;
	retQuery = APK_META_QUERY_NewQuery (field, comparison, value);
	return retQuery;
}

NX_QUERY_t	NX_PORT_META_QUERY_AND (const NX_QUERY_t q1, const NX_QUERY_t q2)
{
	return APK_META_QUERY_And (q1, q2);
}

NX_QUERY_t  NX_PORT_META_QUERY_OR (const NX_QUERY_t q1, const NX_QUERY_t q2)
{
	return APK_META_QUERY_Or (q1, q2);
}

NX_QUERY_t  NX_PORT_META_QUERY_NOT (const NX_QUERY_t q1)
{
	return APK_META_QUERY_Not (q1);
}

HERROR		NX_PORT_META_QUERY_SetQuery (NX_MetaQHandle_t self, const NX_QUERY_t query)
{
	ApkMetaQHandle_t apkSelf = (ApkMetaQHandle_t)self;
	APK_QUERY_t	apkQuery = (APK_QUERY_t)query;
	return APK_META_QUERY_SetQuery (apkSelf, apkQuery);
}

HERROR		NX_PORT_META_QUERY_Abort (NX_MetaQHandle_t self)
{
	ApkMetaQHandle_t apkSelf = (ApkMetaQHandle_t)self;
	return APK_META_QUERY_Abort(apkSelf);
}

void		NX_PORT_META_QUERY_DeleteQuery (NX_QUERY_t q)
{
	APK_QUERY_t	apkQuery = (APK_QUERY_t)q;
	APK_META_QUERY_DeleteQuery(apkQuery);
}

#ifndef CONFIG_FUNC_EPG_USE_SVCUID
HERROR		NX_PORT_META_QUERY_AddSvcConstraint (NX_MetaQHandle_t self, HINT32 onid, HINT32 tsid, HINT32 svcid)
{
	ApkMetaQHandle_t apkSelf = (ApkMetaQHandle_t)self;
	return APK_META_QUERY_AddSvcConstraint(apkSelf, onid, tsid, svcid);
}
#else
HERROR		NX_PORT_META_QUERY_AddSvcConstraint (NX_MetaQHandle_t self, HINT32 svcuid)
{
	ApkMetaQHandle_t apkSelf = (ApkMetaQHandle_t)self;
	return APK_META_QUERY_AddSvcConstraint(apkSelf, svcuid);
}
#endif

void		NX_PORT_META_QUERY_SetFilter (NX_MetaQHandle_t self, NX_META_QUERY_Filter_e eFilter)
{
	ApkMetaQHandle_t apkSelf = (ApkMetaQHandle_t)self;
	APK_META_QUERY_SetFilter(apkSelf, (APK_META_QUERY_Filter_e)eFilter);
}

HERROR		NX_PORT_META_QUERY_Search (NX_MetaQHandle_t self, HBOOL direct)
{
	ApkMetaQHandle_t apkSelf = (ApkMetaQHandle_t)self;
	return APK_META_QUERY_Search(apkSelf, direct);
}

HxVector_t *NX_PORT_META_QUERY_Result (NX_MetaQHandle_t self)
{
	ApkMetaQHandle_t apkSelf = (ApkMetaQHandle_t)self;
	return APK_META_QUERY_Result(apkSelf);
}

DxEvent_t *	NX_PORT_EVENT_Incref (DxEvent_t *event)
{
	return APK_EVENT_Incref(event);
}

void		NX_PORT_EVENT_Delete (DxEvent_t *event)
{
	APK_EVENT_Delete(event);
}

#ifndef CONFIG_FUNC_EPG_USE_SVCUID
HxVector_t * NX_PORT_META_QUERY_GetPF (HINT32 onid, HINT32 tsid, HINT32 svcid)
{
	return APK_META_QUERY_GetPF (onid, tsid, svcid);
}
#else
HxVector_t * NX_PORT_META_QUERY_GetPF (HINT32 svcuid)
{
	return APK_META_QUERY_GetPF (svcuid);
}

#endif


