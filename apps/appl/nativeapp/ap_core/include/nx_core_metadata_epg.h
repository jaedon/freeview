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
 * @file	  		nx_core_metadata_epg.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_CORE_METADATA_EPG_H__
#define __NX_CORE_METADATA_EPG_H__

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
#include <nx_core_message.h>
#include <nx_port.h>
#include <nx_core_metadata_programmes.h>
#include <nx_core_metadata_search.h>

/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/

/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/

/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/

typedef struct
{
	HINT32	nCurProgrammesNum;
	NX_Programme_t *pstProgrammes;
	HxVector_t *resultList;

}NX_Metadata_EPG_Context_t;
/*******************************************************************/
/********************	Private Variables		********************/
/*******************************************************************/


/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/
HERROR 	NX_METADATA_EPG_Init(void);
HERROR	NX_METADATA_EPG_SearchEPG(NX_MetadataSearch_t *pstMetaSearch);
HERROR 	NX_METADATA_EPG_GetResultProgrammeBySvcUid(HUINT32 svcuid, HxVector_t *resultList, HINT32 *nNum);
HERROR	NX_METADATA_EPG_GetResultProgrammeBySvcUidAndEventId(HUINT32 ulSvcUid, HINT32 lEventId, HxVector_t *resultList, HINT32 *nNum);
HERROR	NX_METADATA_EPG_GetResultProgrammeItemBySvcUid(HUINT32 svcuid, HxVector_t *resultList, HINT32 *nNum);
HERROR	NX_METADATA_EPG_GetResultProgrammeBySeriesId(HUINT32 ulSvcUid, HUINT32 ulSeriesId, HUINT32 ulTime, HxVector_t *stResultList, HINT32 *lNum);
HERROR	NX_METADATA_EPG_GetResultProgrammeBySvcUidAndTripleId(HINT32 lEventId, HUINT32 ulSvcUid, HINT32 lSvcId, HINT32 lTsId, HINT32 lOnId, HxVector_t *stResultList, HINT32 *lNum);


#endif // __NX_CORE_METADATA_EPG_H__


