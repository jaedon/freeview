/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_meta_search.h
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

#ifndef ___APK_META_SEARCH_H___
#define ___APK_META_SEARCH_H___


#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <dlib.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/



/**
 * search asynchronous getter
 *
 * @param	nId             search id
 * @param	nRemainingJob   remaining job (if 0, search finished)
 * @param	szTypeName      type name (same as DB table name)
 * @param	pList           result vector(array), if NULL result failed, length always greater then 0
 * @param	pvUserData      'userdata' passed into APK_METASEARCH_Exec
 * @return	TRUE 'STOP SEARCH', FALSE 'KEEP GOING'
 */
typedef HBOOL (* APK_META_Getter_t) (
					  HINT32 nId
					, HINT32 nRemainingJob
					, const HCHAR *szTypeName
					, HxVector_t *pList
					, void *pvUserData
				);

#ifdef	METASEARCH_GETTER_EXAMPLE
static HBOOL metasearch_Getter (HINT32 nId, HINT32 nRemainingJob, const HCHAR *szTypeName, HxVector_t *pList, void *pvUserData)
{
	if (pList == NULL)	// if pList == NULL, 'SEARCH FAILED' for 'szTypeName'
	{
		return FALSE;
	}

	if (strcmp(DxNAMEOF(DxEvent_t), szTypeName) == 0)
	{
		DxEvent_t *event;
		HxVECTOR_EACH(pList, DxEvent_t *, event, {
			// TODO: do something...
		});
	}
	else if (strcmp(DxNAMEOF(DxService_t), szTypeName) == 0)
	{
		// like this yo~!
	}
	// ...
	else if (strcmp("FOO", szTypeName) == 0) {}
	else if (strcmp("BAR", szTypeName) == 0) {}
	// ...
	else
	{
		HxLOG_Assert(0);
	}

	if (nRemainingJob == 0)	// search finished
	{
		return TRUE; // if return TRUE, 'STOP SEARCH'
	}
	return FALSE; // if return FALSE, 'KEEP GOING'
}
#endif

/**
 * DB로부터 데이터를 검색
 * - szQuery는 FULL SQL
 * - DaMa의 DB Table을 검색하며, 결과는 async로 getter로 들어감
 * - $humaxcmd dama schema 를 입력하여 DB Table 스키마를 확인하고 쿼리를 구성하면 됨?
 *
 * @param	szQuery
 * @param	pfnGetter
 * @param	pvUserData
 * @return	Search id
 */
HINT32	APK_META_SEARCH_Exec (const HCHAR *szQuery, APK_META_Getter_t pfnGetter, void *pvUserData);

/**
 * 검색이 끝나지 않은 상태에서 검색을 무시하고 싶을 경우 사용
 *
 * @param	Search id
 */
void	APK_META_SEARCH_Abort (HINT32 nId);


// for FUTURE (sync)
HBOOL	APK_META_SEARCH_Join (HINT32 nId, HUINT32 timeout);


#ifdef __cplusplus
}
#endif

#endif

/* end of file */
