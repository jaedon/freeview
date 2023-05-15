/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_filemgr_browse.c
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
static HxList_t		*s_pst_fm_browse_cbList = NULL;

/*******************************************************************/
/********************      Functions     ***************************/
/*******************************************************************/

static void apk_filemgr_browse_freeEventCb( HINT32 nArgc, void *apArgv[] )
{
	APK_FILEMGR_BROWSE_EVT_e apkEvent = eAPK_FILEMGR_BROWSE_EVT_UNKNOWN;
	APK_FILEMGR_BROWSE_RESULT_t *pResult = NULL;
	HxLOG_Assert(nArgc == 5);

	pResult =  (APK_FILEMGR_BROWSE_RESULT_t *)apArgv[4];
	apkEvent = (APK_FILEMGR_BROWSE_EVT_e)apArgv[2];
	(void)apkEvent; // compile warning...

	if ( pResult )
	{
		if( pResult->pstFileList )
		{
			HLIB_LIST_RemoveAllFunc(pResult->pstFileList, APK_Free_CB);
		}
		HLIB_STD_MemFree(pResult);
	}
}

static HxList_t *apk_filemgr_browse_getitems(UAPI_BROWSE_RESULT_t *pstInfo)
{
	HINT32 			nextdatapos	 = 0;
	HxList_t 		*pApkResult = NULL;
	DxFileInfo_t 	*pGetData,*pDxNewFile;
	HCHAR 		 	*p;

	//flexible array member 에서 DxFileInfo_t list 를 뽑아오는 과정
	while(pstInfo->nItemSize > nextdatapos)
	{
		p = (HCHAR*)pstInfo->Itemdata; // 1byte 씩접근
		p +=  nextdatapos;
		pGetData = (DxFileInfo_t*)p;
		pDxNewFile = (DxFileInfo_t *)HLIB_STD_MemAlloc(sizeof(DxFileInfo_t) + pGetData->size );
		HxSTD_MemCopy(pDxNewFile,p,sizeof(DxFileInfo_t) + pGetData->size);
		nextdatapos += sizeof(DxFileInfo_t) + pGetData->size ;
		pApkResult = HLIB_LIST_Append(pApkResult, pDxNewFile );
	}
	return pApkResult;
}

static HERROR	apk_filemgr_browse_listener( HUINT32 nHandleId, HINT32 nRequestId, UAPI_BROWSE_RESULT_t *pstInfo )
{
	APK_FILEMGR_BROWSE_Notifier_t	fnNotifier;
	HUINT32 retErr = pstInfo->ulErr;
	APK_FILEMGR_BROWSE_EVT_e apkEvent = eAPK_FILEMGR_BROWSE_EVT_UNKNOWN;
	APK_FILEMGR_BROWSE_RESULT_t apkResult;
	HxList_t	*list;

	HxLOG_Print( "[APK_FM_BROWSE_EVENT] nHandId(%d), nRequestId(%d)\n", nHandleId, nRequestId);
	if ( !s_pst_fm_browse_cbList || 0 == nRequestId )
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet( &apkResult, 0x00, sizeof(APK_FILEMGR_BROWSE_RESULT_t) );
	apkResult.nTotalNum = pstInfo->nTotalNum;

	switch ( pstInfo->eEvent )
	{
		case eUAPI_FM_BROWSE_OPEN :
			apkEvent = eAPK_FILEMGR_BROWSE_EVT_OPEN;
			break;
		case eUAPI_FM_BROWSE_CLOSE :
			apkEvent = eAPK_FILEMGR_BROWSE_EVT_CLOSE;
			break;
		case eUAPI_FM_BROWSE_GETITEMS :
		case eUAPI_FM_BROWSE_SCANITEMS :
		printf("##############################################\n");
		printf("[%s:%d] Received UAPI Message !!! \n", __FUNCTION__, __LINE__);
		printf("##############################################\n");
			apkEvent = ( eUAPI_FM_BROWSE_GETITEMS == pstInfo->eEvent)
						? eAPK_FILEMGR_BROWSE_EVT_GETITEMS : eAPK_FILEMGR_BROWSE_EVT_SCANITEMS;
			apkResult.nReqIdx = pstInfo->nReqIdx;
			apkResult.nItemCnt = pstInfo->nDoneNum;
			apkResult.pstFileList = apk_filemgr_browse_getitems(pstInfo);
			break;
		case eUAPI_FM_BROWSE_SORT :
			apkEvent = eAPK_FILEMGR_BROWSE_EVT_SORT;
			break;
		case eUAPI_FM_BROWSE_THUMB :
			apkEvent = eAPK_FILEMGR_BROWSE_EVT_THUMB;
			break;
		case eUAPI_FM_BROWSE_ADD :
			apkEvent = eAPK_FILEMGR_BROWSE_EVT_ADD;
			break;
		case eUAPI_FM_BROWSE_REMOVE :
			apkEvent = eAPK_FILEMGR_BROWSE_EVT_REMOVE;
			break;
		default :
			return ERR_OK;
	}

	HxLOG_Print( "[APK_FM_BROWSE_EVENT] Event[%d], nHandId(%d), nRequestId(%d)\n", apkEvent , nHandleId, nRequestId);
	list = s_pst_fm_browse_cbList;
	while ( list )
	{
		fnNotifier = (APK_FILEMGR_BROWSE_Notifier_t)HLIB_LIST_Data(list);
		if (fnNotifier)
		{
			APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

			stApkEvtArgs.pfnFreeFunc = apk_filemgr_browse_freeEventCb;
			stApkEvtArgs.ulArgCount = 5;
			stApkEvtArgs.apArgV[0] = (void *)fnNotifier;
			stApkEvtArgs.apArgV[1] = (void *)nHandleId;
			stApkEvtArgs.apArgV[2] = (void *)apkEvent;
			stApkEvtArgs.apArgV[3] = (void *)retErr;
			stApkEvtArgs.apArgV[4] = (void *)APK_MemDup(&apkResult, sizeof(APK_FILEMGR_BROWSE_RESULT_t));

			APK_EVENT_Emit(&stApkEvtArgs);
		}
		list = list->next;
	}
	return ERR_OK;
}

HERROR	APK_FILEMGR_BROWSE_RegisterCallback(APK_FILEMGR_BROWSE_Notifier_t fnNotifier)
{
	HxList_t *pItem = NULL;
	if ( s_pst_fm_browse_cbList )
	{
		pItem = HLIB_LIST_Find( s_pst_fm_browse_cbList, (void *)fnNotifier );
	}

	if ( pItem )
		return ERR_FAIL;

	s_pst_fm_browse_cbList = HLIB_LIST_Append( s_pst_fm_browse_cbList, (void *)fnNotifier );
	return ERR_OK;
}

HERROR	APK_FILEMGR_BROWSE_UnregisterCallback(APK_FILEMGR_BROWSE_Notifier_t fnNotifier)
{
	s_pst_fm_browse_cbList = HLIB_LIST_Remove( s_pst_fm_browse_cbList, (void *)fnNotifier );
	return ERR_OK;
}

HUINT32	APK_FILEMGR_BROWSE_Open( HCHAR *pszPath, HUINT32 nCategory, HCHAR *pszExtFilter, HUINT32 nOptFlags )
{
	HUINT32 nHandId = 0;
	nHandId = UAPI_FM_BROWSE_Open( pszPath, nCategory, pszExtFilter, nOptFlags, apk_filemgr_browse_listener);

	HxLOG_Print( "[APK_BROWSE_OPEN] nHandleId(%d)\n", nHandId );
	return nHandId;
}

HERROR	APK_FILEMGR_BROWSE_Close( HUINT32 nHandleId )
{
	HxLOG_Print( "[APK_BROWSE_Close] nHandleId(%d)\n", nHandleId);
	return UAPI_FM_BROWSE_Close( nHandleId );
}

HERROR	APK_FILEMGR_BROWSE_GetItems( HUINT32 nHandleId, HUINT32 nIndex, HUINT32 nReqCount, HINT32 nOption )
{
	HxLOG_Print( "[APK_BROWSE_GetItems] nHandleId(%d), nIndex(%d), nReqCount(%d)\n", nHandleId, nIndex, nReqCount);
	return UAPI_FM_BROWSE_GetItems( nHandleId, nIndex, nReqCount, nOption );
}

HERROR	APK_FILEMGR_BROWSE_ScanItems( HUINT32 nHandleId, HUINT32 nReqCount, HINT32 nOption )
{
	HxLOG_Print( "[APK_BROWSE_ScanItems] nHandleId(%d), nReqCount(%d)\n", nHandleId, nReqCount);
	return UAPI_FM_BROWSE_ScanItems( nHandleId, nReqCount, nOption );
}

HERROR	APK_FILEMGR_BROWSE_Sort( HUINT32 nHandleId, HUINT32 nSort )
{
	HxLOG_Print( "[APK_BROWSE_Sort] nHandleId(%d) nSort(%d)\n", nHandleId, nSort);
	return UAPI_FM_BROWSE_Sort( nHandleId, 0 );
}

