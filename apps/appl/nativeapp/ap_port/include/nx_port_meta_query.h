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
 * @file	  		nx_port_meta_query.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_PORT_META_QUERY_H__
#define __NX_PORT_META_QUERY_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/

#include <ondk.h>
#include <hapi.h>
#include <papi.h>
#include <hlib.h>
#include <dlib.h>
#include <apk.h>

#include <nx_port_type.h>

/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/


/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/


/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/
typedef enum
{
	eNxQUERY_TARGET_EPG,
	eNxQUERY_TARGET_VOD,

	eNxQUERY_TARGET_UNKNOWN,
}NX_META_QUERY_Target_e;

typedef enum
{
	eNxQUERY_EVENT_NEW_VERSION,
	eNxQUERY_EVENT_MORE_DATA,
	eNxQUERY_EVENT_FINISHED,
	eNxQUERY_EVENT_INVALID,

	eNxQUERY_EVENT_UNKNOWN
} NX_META_QUERY_Event_e;

typedef enum
{
	eNxQUERY_FILTER_ALL,
	eNxQUERY_FILTER_PF,
	eNxQUERY_FILTER_SCH,
	eNxQUERY_FILTER_RF,
	eNxQUERY_FILTER_TVTV,
	eNxQUERY_FILTER_IP
} NX_META_QUERY_Filter_e;


typedef void *	NX_QUERY_t;
typedef struct _QSearch_t	* NX_MetaQHandle_t;

typedef void	(*NX_PORT_META_QUERY_Notifier_t) (NX_MetaQHandle_t self, HINT32 event, const HINT32 *args, void *userdata);


/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/


/*******************************************************************/
/********************	global function prototype   ****************/
/*******************************************************************/
NX_MetaQHandle_t	NX_PORT_META_QUERY_New (NX_META_QUERY_Target_e eTarget);
void 	NX_PORT_META_QUERY_Delete(NX_MetaQHandle_t self);
void	NX_PORT_META_QUERY_SetNotifier (NX_MetaQHandle_t self, NX_PORT_META_QUERY_Notifier_t fnNotifier, void *userdata);
NX_QUERY_t	NX_PORT_META_QUERY_NewQuery (const HCHAR *field, const HCHAR *comparison, const HCHAR *value);
NX_QUERY_t	NX_PORT_META_QUERY_AND (const NX_QUERY_t q1, const NX_QUERY_t q2);
NX_QUERY_t  NX_PORT_META_QUERY_OR (const NX_QUERY_t q1, const NX_QUERY_t q2);
NX_QUERY_t  NX_PORT_META_QUERY_NOT (const NX_QUERY_t q1);
HERROR		NX_PORT_META_QUERY_SetQuery (NX_MetaQHandle_t self, const NX_QUERY_t query);
HERROR		NX_PORT_META_QUERY_Abort (NX_MetaQHandle_t self);
void		NX_PORT_META_QUERY_DeleteQuery (NX_QUERY_t q);
#ifndef CONFIG_FUNC_EPG_USE_SVCUID
HERROR		NX_PORT_META_QUERY_AddSvcConstraint (NX_MetaQHandle_t self, HINT32 onid, HINT32 tsid, HINT32 svcid);
#else
HERROR		NX_PORT_META_QUERY_AddSvcConstraint (NX_MetaQHandle_t self, HINT32 svcuid);
#endif
void		NX_PORT_META_QUERY_SetFilter (NX_MetaQHandle_t self, NX_META_QUERY_Filter_e eFilter);
HERROR		NX_PORT_META_QUERY_Search (NX_MetaQHandle_t self, HBOOL direct);
HxVector_t *NX_PORT_META_QUERY_Result (NX_MetaQHandle_t self);
DxEvent_t *	NX_PORT_EVENT_Incref (DxEvent_t *event);
void		NX_PORT_EVENT_Delete (DxEvent_t *event);

#ifndef CONFIG_FUNC_EPG_USE_SVCUID
HxVector_t * NX_PORT_META_QUERY_GetPF (HINT32 onid, HINT32 tsid, HINT32 svcid);
#else
HxVector_t * NX_PORT_META_QUERY_GetPF (HINT32 svcuid);

#endif


#endif // __NX_PORT_META_QUERY_H__

