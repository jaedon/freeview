/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_filemgr_group.c
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
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include	<hlib.h>
#include	<uapi.h>

#include	<apk.h>
#include	"_apk_int.h"

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
/********************      Static Variables     ********************/
/*******************************************************************/
static HxList_t		*s_pst_fm_group_cbList = NULL;

/*******************************************************************/
/********************      Functions     ***************************/
/*******************************************************************/

static void apk_filemgr_group_freeEventCb( HINT32 nArgc, void *apArgv[] )
{

}

static HERROR	apk_filemgr_group_listener( HUINT32 nHandleId, HINT32 nRequestId, UAPI_GROUP_RESULT_t *pstInfo )
{

	return ERR_OK;
}

HERROR	APK_FILEMGR_GROUP_RegisterCallback(APK_FILEMGR_GROUP_Notifier_t fnNotifier)
{
	HxList_t *pItem = NULL;
	if ( s_pst_fm_group_cbList )
	{
		pItem = HLIB_LIST_Find( s_pst_fm_group_cbList, (void *)fnNotifier );
	}

	if ( pItem )
		return ERR_FAIL;

	s_pst_fm_group_cbList = HLIB_LIST_Append( s_pst_fm_group_cbList, (void *)fnNotifier );
	return ERR_OK;
}

HERROR	APK_FILEMGR_GROUP_UnregisterCallback(APK_FILEMGR_GROUP_Notifier_t fnNotifier)
{
	s_pst_fm_group_cbList = HLIB_LIST_Remove( s_pst_fm_group_cbList, (void *)fnNotifier );
	return ERR_OK;
}

HUINT32	APK_FILEMGR_GROUP_Create( HCHAR *pszExtFilter )
{
	HUINT32 nHandId = 0;
	nHandId = UAPI_FM_GROUP_Create( pszExtFilter, apk_filemgr_group_listener);

	HxLOG_Print( "[APK_GROUP_OPEN] nHandleId(%ld)\n", nHandId );
	return nHandId;
}

HERROR	APK_FILEMGR_GROUP_Release( HUINT32 nHandleId )
{
	HxLOG_Print( "[APK_GROUP_Close] nHandleId(%d)\n", nHandleId);
	return UAPI_FM_GROUP_Release( nHandleId );
}

HERROR	APK_FILEMGR_GROUP_AddPath( HUINT32 nHandleId, const HCHAR *pszPath )
{
	HxLOG_Print( "[APK_GROUP_AddPath] nHandleId(%d), pszPath(%s)\n", nHandleId, pszPath );
	return UAPI_FM_GROUP_SetPath( nHandleId, eUAPI_FM_GROUP_PATH_ADD, pszPath );
}

HERROR	APK_FILEMGR_GROUP_RemovePath( HUINT32 nHandleId, const HCHAR *pszPath )
{
	HxLOG_Print( "[APK_GROUP_RemovePath] nHandleId(%d), pszPath(%s)\n", nHandleId, pszPath );
	return UAPI_FM_GROUP_SetPath( nHandleId, eUAPI_FM_GROUP_PATH_REMOVE, pszPath);
}

HERROR	APK_FILEMGR_GROUP_ExcludePath( HUINT32 nHandleId, const HCHAR *pszPath )
{
	HxLOG_Print( "[APK_GROUP_ExcludePath] nHandleId(%d), pszPath(%s)\n", nHandleId, pszPath );
	return UAPI_FM_GROUP_SetPath( nHandleId, eUAPI_FM_GROUP_PATH_EXCLUDE, pszPath);
}

HERROR	APK_FILEMGR_GROUP_Copy( HUINT32 nHandleId, const HCHAR *pszPath, HINT32 *pnReqId )
{
	HINT32	nReqId = 0;
	if (!pnReqId)
		return ERR_FAIL;
	//HxLOG_Print( "[APK_GROUP_Copy] nHandleId(%d)\n", nHandleId);
	printf( "[APK_GROUP_Copy] nHandleId(%d)\n", nHandleId);
	nReqId = UAPI_FM_GROUP_Operation( nHandleId, eUAPI_FM_GROUP_OPER_COPY, pszPath );
	if ( 0 >=  nReqId )
		return ERR_FAIL;

	*pnReqId = nReqId;
	return ERR_OK;
}

HERROR	APK_FILEMGR_GROUP_Move( HUINT32 nHandleId, const HCHAR *pszPath, HINT32 *pnReqId )
{
	HINT32	nReqId = 0;
	if (!pnReqId)
		return ERR_FAIL;
	//HxLOG_Print( "[APK_GROUP_Move] nHandleId(%d)\n", nHandleId);
	printf( "[APK_GROUP_Move] nHandleId(%d)\n", nHandleId);
	nReqId = UAPI_FM_GROUP_Operation( nHandleId, eUAPI_FM_GROUP_OPER_MOVE, pszPath );
	if ( 0 >=  nReqId )
		return ERR_FAIL;

	*pnReqId = nReqId;
	return ERR_OK;
}

HERROR	APK_FILEMGR_GROUP_Remove( HUINT32 nHandleId, HINT32 *pnReqId )
{
	HINT32	nReqId = 0;
	if (!pnReqId)
		return ERR_FAIL;
	//HxLOG_Print( "[APK_GROUP_Remove] nHandleId(%d)\n", nHandleId);
	printf( "[APK_GROUP_Remove] nHandleId(%d)\n", nHandleId);
	nReqId = UAPI_FM_GROUP_Operation( nHandleId, eUAPI_FM_GROUP_OPER_REMOVE, NULL );
	if ( 0 >=  nReqId )
		return ERR_FAIL;

	*pnReqId = nReqId;
	return ERR_OK;
}

HERROR	APK_FILEMGR_GROUP_Cancel( HUINT32 nHandleId, HINT32 *pnReqId )
{
	HINT32	nReqId = 0;
	if (!pnReqId)
		return ERR_FAIL;
	HxLOG_Print( "[APK_GROUP_Cancel] nHandleId(%d)\n", nHandleId );
	nReqId = UAPI_FM_GROUP_Operation( nHandleId, eUAPI_FM_GROUP_OPER_CANCEL, NULL );
	if ( 0 >=  nReqId )
		return ERR_FAIL;

	*pnReqId = nReqId;
	return ERR_OK;
}

