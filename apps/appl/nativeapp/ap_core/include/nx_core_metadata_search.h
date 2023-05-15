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
 * @file	  		nx_core_metadata_search.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_CORE_METADATA_SEARCH_H__
#define __NX_CORE_METADATA_SEARCH_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/
#include <ondk.h>
#include <hapi.h>
#include <papi.h>
#include <oapi.h>
#include <apk.h>
#include <clib.h>

#include <nx_common.h>
#include <nx_port.h>
#include <nx_core_channel.h>

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
	eNX_META_SEARCH_TARGET_SCHEDULED		= 1,
	eNX_META_SEARCH_TARGET_ONDEMAND		= 2,

	eNX_META_SEARCH_TARGET_UNKNOWN,
}NX_MetadataSearchTarget_e;

typedef void 	(* NX_SearchPfMonitor_t) (HUINT32 svcUid, HxVector_t *pfDataList);
typedef void 	(* NX_SearchEpgMonitor_t) (HUINT32 svcUid, HxVector_t *pfDataList);
typedef void 	(* NX_SearchResultListener_t) (HxVector_t *pfDataList);

typedef struct
{
	NX_MetaQHandle_t	context;

	HBOOL				zombie;
	HxTREE_t			*dependency;
	NX_SearchResultListener_t resultListener;

	struct monitor {
		HUINT32				svcuid;
		void					*userdata;
		NX_SearchPfMonitor_t	callback;
	} monitor;

} NX_MetadataSearch_t;

typedef struct
{
	HBOOL		bIsMetadataSearch;
	//NX_MetadataSearch_t		*pstMetaSearch;

} NX_MetadataSearchContext_t;


/*******************************************************************/
/********************	Private Variables		********************/
/*******************************************************************/


/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/
HERROR 	NX_METADATA_SEARCH_Init(void);
NX_MetadataSearch_t* NX_METADATA_SEARCH_New(NX_MetadataSearchTarget_e target);
HERROR	NX_METADATA_SEARCH_SearchPF(NX_MetadataSearch_t *pMetaSearchData, HUINT32 svcuid, NX_SearchPfMonitor_t monitor, void *userdata);
HERROR 	NX_METADATA_SEARCH_SearchEPG(NX_MetadataSearch_t *pMetaSearchData, NX_SearchResultListener_t listener, void *userdata);
HERROR 	NX_METADATA_SEARCH_AddChannelConstraint(NX_MetadataSearch_t *pMetaSearchData, NX_Channel_t *channel);
NX_QUERY_t 	NX_METADATA_SEARCH_CreateQuery(const HCHAR *field, const HCHAR *comparison, const HCHAR *value);

#endif // __NX_CORE_METADATA_SEARCH_H__


