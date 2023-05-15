/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_filemgr.c
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
#include	<dapi.h>

#include	<apk.h>
#include	"_apk_int.h"
#include <dlib.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HINT32	nRefer;
	HINT32	nRequestId;
	HUINT32	ulBrowsingMode;
	HBOOL	bDir;

	HCHAR 	*pszKey;		// keyword browser시 필요한 정보
	void		*pvUserData;
	HxList_t	*pstResultList;

	APK_FILEMGR_Notifier_t	fnNotifier;
} AxFileMgrStatus_t;

typedef struct
{
	HUINT32	ulSemId;

	HxCBUF_t	nCBuf;
	HINT32	nStream;
	HINT32	nStreamParam;

	HxHASH_t	*pstRequestTable;
} AxFileMgrInfo_t;
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ***************************/
/*******************************************************************/
static AxFileMgrInfo_t*	apk_filemgr_getInstance(void)
{
	static AxFileMgrInfo_t	*s_pstFileMgrInfo = NULL;

	if (s_pstFileMgrInfo == NULL)
	{
		s_pstFileMgrInfo = (AxFileMgrInfo_t*)APK_Calloc(sizeof(AxFileMgrInfo_t));
		HxSEMT_Create(&s_pstFileMgrInfo->ulSemId, "apkfilemgrsem", 0);
		HxSEMT_Get(s_pstFileMgrInfo->ulSemId);
		s_pstFileMgrInfo->nCBuf   = HLIB_CBUF_New(8192);
		s_pstFileMgrInfo->nStream = HLIB_RWSTREAM_OpenEx(NULL, 1024, APK_Malloc_CB, APK_Free_CB);
		s_pstFileMgrInfo->nStreamParam = HLIB_RWSTREAM_OpenEx(NULL, 1024, APK_Malloc_CB, APK_Free_CB);
		s_pstFileMgrInfo->pstRequestTable = HLIB_HASH_NewFull(HLIB_HASH_GetIntKey, HLIB_HASH_IsIntEqual, NULL, APK_Free_CB);

		UAPI_FM_Init();
	}
	else
	{
		HxSEMT_Get(s_pstFileMgrInfo->ulSemId);
	}

	return s_pstFileMgrInfo;
}

static void				apk_filemgr_releaseInstance(AxFileMgrInfo_t *pstInfo)
{
	HxSEMT_Release(pstInfo->ulSemId);
}

static HINT32	apk_filemgr_GetDummyId(void)
{
	static HINT32	s_nDummyId = 0;

	s_nDummyId = (s_nDummyId + 1) & 0xFFFF;

	return s_nDummyId;
}


static HERROR	apk_filemgr_GetKeywordNoStr(const HCHAR *pszKeyword, HCHAR *pszKeyNoStr)
{

	 //등록된 키워드에 따라서 SQL field 이름을 지정
	if (HxSTD_StrStr(pszKeyword, "Album") != NULL)
	{
		HxSTD_StrNCpy(pszKeyNoStr, "keyword1", 9);
	}
	else if (HxSTD_StrStr(pszKeyword, "Artist") != NULL)
	{
		HxSTD_StrNCpy(pszKeyNoStr, "keyword2", 9);
	}
	else
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HCHAR*	apk_filemgr_MakeExtensionSQL(AxFileMgrInfo_t *pstInfo,  const HCHAR *pszFilter)
{
	HCHAR	*pszExt, *p;
	HINT32	nParamSd = pstInfo->nStreamParam;

	pszExt = HLIB_CBUF_GetBuffer(pstInfo->nCBuf, 1024);
	HLIB_STD_StrNCpySafe(pszExt, pszFilter, 1024);

	HLIB_RWSTREAM_Rewind(nParamSd);
	HLIB_RWSTREAM_Print (nParamSd, " (ext in (");

	p = pszExt;
	while ((p = HxSTD_StrRChar(pszExt, '|')))
	{
		HLIB_RWSTREAM_Print(nParamSd, "\"%s\",", p + 1);
		*p = '\0';
	}
	if (!HxSTD_StrEmpty(pszExt))
		HLIB_RWSTREAM_Print(nParamSd, "\"%s\"", pszExt);

	HLIB_RWSTREAM_Print (nParamSd, "))");

	return HLIB_RWSTREAM_GetBuf(nParamSd);
}

static HCHAR * apk_filemgr_MakeSubTypeSQL( HUINT32 ulMode, HCHAR *pszBuf, HINT32 nBufSize )
{
	HCHAR *pTmp = pszBuf;
	HINT32 remainLen = nBufSize, typeLen = 0;
	HBOOL bNeedOr = FALSE;

	if ( NULL == pszBuf )
		return NULL;

	if ( ulMode & AxFILEMGR_BROWSE_VIDEO )
	{
		HxSTD_snprintf( pTmp, remainLen, "type = %d", eDxFILETYPE_AVSTREAM );
		typeLen = HxSTD_StrLen(pszBuf);
		remainLen = nBufSize - typeLen;
		pTmp = pszBuf + typeLen;
		bNeedOr = TRUE;
	}
	if ( ulMode & AxFILEMGR_BROWSE_MUSIC )
	{
		if ( bNeedOr )
		{
			HxSTD_StrNCat( pTmp, " or ", remainLen );
			typeLen = HxSTD_StrLen(pszBuf);
			remainLen = nBufSize - typeLen;
			pTmp = pszBuf + typeLen;
		}
		HxSTD_snprintf( pTmp, remainLen, "type = %d", eDxFILETYPE_MUSIC );
		bNeedOr = TRUE;
	}
	if ( ulMode & AxFILEMGR_BROWSE_PHOTO )
	{
		if ( bNeedOr )
		{
			HxSTD_StrNCat( pTmp, " or ", remainLen );
			typeLen = HxSTD_StrLen(pszBuf);
			remainLen = nBufSize - typeLen;
			pTmp = pszBuf + typeLen;
		}
		HxSTD_snprintf( pTmp, remainLen, "type = %d", eDxFILETYPE_EXIF );
	}

	return pszBuf;
}

static HCHAR*	apk_filemgr_MakeOneDepthViewSQL(AxFileMgrInfo_t *pstInfo, const HCHAR *pszSrc, const HCHAR *pszFilter, HUINT32 ulMode)
{
	HCHAR	*pszExtensionSQL = NULL;
	HINT32	nStream;

	nStream = pstInfo->nStream;

	HLIB_RWSTREAM_Rewind(nStream);
	if (!HxSTD_StrEmpty(pszFilter))
		pszExtensionSQL = apk_filemgr_MakeExtensionSQL(pstInfo, pszFilter);

	if (ulMode & AxFILEMGR_BROWSE_DIR)
	{
		if (pszExtensionSQL)
			HLIB_RWSTREAM_Print(nStream, "select * from DxFileInfo_t where %s and ( path like \"%s/%%\") group by path ", pszExtensionSQL, pszSrc);
		else
			HLIB_RWSTREAM_Print(nStream, "select * from DxFileInfo_t where ( path like \"%s/%%\" ) group by path", pszSrc);
		HLIB_RWSTREAM_Print(nStream, " order by onedepth  asc");
	}
	else	if (ulMode & AxFILEMGR_BROWSE_FILE)
	{
		if (pszExtensionSQL)
			HLIB_RWSTREAM_Print(nStream, " select * from DxFileInfo_t where path=\"%s\" and %s ", pszSrc, pszExtensionSQL);
		else
			HLIB_RWSTREAM_Print(nStream, " select * from DxFileInfo_t where path=\"%s\" and ( ext like  \"_%%\") ", pszSrc);

		HLIB_RWSTREAM_Print(nStream, " order by name  asc");
	}
	else 	if (ulMode & AxFILEMGR_BROWSE_MASK_SUBTYPE )
	{
		HCHAR szTypeSQL[128] = { 0 };

		apk_filemgr_MakeSubTypeSQL( ulMode, szTypeSQL, sizeof(szTypeSQL) );

		if (pszExtensionSQL)
			HLIB_RWSTREAM_Print(nStream, "select * from DxFileInfo_t where ( %s ) and path like \"%s/%%\" and %s ", szTypeSQL,  pszSrc, pszExtensionSQL);
		else
			HLIB_RWSTREAM_Print(nStream, "select * from DxFileInfo_t where ( %s ) and path like \"%s/%%\"  ", szTypeSQL,  pszSrc);

	}

	return (HCHAR*)HLIB_RWSTREAM_GetBuf(nStream);
}

static HCHAR*	apk_filemgr_MakeFolderViewSQL(AxFileMgrInfo_t *pstInfo, const HCHAR *pszSrc, const HCHAR *pszFilter, HUINT32 ulMode)
{
	HCHAR	*pszExtensionSQL = NULL;
	HINT32	nStream;

	nStream = pstInfo->nStream;

	HLIB_RWSTREAM_Rewind(nStream);
	if (!HxSTD_StrEmpty(pszFilter))
		pszExtensionSQL = apk_filemgr_MakeExtensionSQL(pstInfo, pszFilter);

	if (ulMode & AxFILEMGR_BROWSE_DIR)
	{
		HLIB_RWSTREAM_Print(nStream, "select * from DxFileInfo_t where path = \"%s\" and ( type = %d ) ", pszSrc, eDxFILETYPE_DIRECTORY);
	}
	else 	if (ulMode & AxFILEMGR_BROWSE_FILE)
	{
		if (pszExtensionSQL)
			HLIB_RWSTREAM_Print(nStream, " select * from DxFileInfo_t where path=\"%s\" and  %s ", pszSrc, pszExtensionSQL);
		else
			HLIB_RWSTREAM_Print(nStream, " select * from DxFileInfo_t where path=\"%s\"  and ( ext like  \"_%%\")", pszSrc);
	}
	else 	if (ulMode & AxFILEMGR_BROWSE_MASK_SUBTYPE )
	{
		HCHAR szTypeSQL[128] = { 0 };

		apk_filemgr_MakeSubTypeSQL( ulMode, szTypeSQL, sizeof(szTypeSQL) );

		if (pszExtensionSQL)
			HLIB_RWSTREAM_Print(nStream, "select * from DxFileInfo_t where ( %s ) and path=\"%s\" and %s ", szTypeSQL,  pszSrc, pszExtensionSQL);
		else
			HLIB_RWSTREAM_Print(nStream, "select * from DxFileInfo_t where ( %s ) and path=\"%s\"  ", szTypeSQL,  pszSrc);
	}

	HLIB_RWSTREAM_Print(nStream, " order by name asc ");
	return (HCHAR*)HLIB_RWSTREAM_GetBuf(nStream);
}


static HCHAR*	apk_filemgr_MakeKeywordSQL(AxFileMgrInfo_t *pstInfo, const HCHAR *pszKeyword, const HCHAR *pszPath, const HCHAR *pszFilter, HUINT32 ulMode )
{
	HCHAR	*pszExtensionSQL = NULL;
	HINT32	nStream;
	HCHAR	szKeyNoStr[10] = {0,};
	HERROR	hErr;

	nStream = pstInfo->nStream;

	hErr = apk_filemgr_GetKeywordNoStr(pszKeyword, szKeyNoStr);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("[%s] [%d] Unknown keyword!!!(%s) \n",__FUNCTION__,__LINE__,pszKeyword);
		return NULL;
	}

	HLIB_RWSTREAM_Rewind(nStream);
	if (!HxSTD_StrEmpty(pszFilter))
		pszExtensionSQL = apk_filemgr_MakeExtensionSQL(pstInfo, pszFilter);

	nStream = pstInfo->nStream;
	HLIB_RWSTREAM_Rewind(nStream);

	HLIB_RWSTREAM_Print(nStream," select * from DxFileInfo_t where ");
	if (ulMode & AxFILEMGR_BROWSE_MASK_SUBTYPE )
	{
		HCHAR szTypeSQL[128] = { 0 };
		apk_filemgr_MakeSubTypeSQL( ulMode, szTypeSQL, sizeof(szTypeSQL) );

		HLIB_RWSTREAM_Print(nStream,"%s and ",  szTypeSQL);
	}
	HLIB_RWSTREAM_Print(nStream,"%s=\"%s\"", szKeyNoStr, pszKeyword);

	if (pszPath)
	{
		HLIB_RWSTREAM_Print(nStream," and path like \"%s%%\"", pszPath);
	}

	if (pszExtensionSQL)
	{
		HLIB_RWSTREAM_Print(nStream," and %s", pszExtensionSQL);
	}

	return (HCHAR*)HLIB_RWSTREAM_GetBuf(nStream);
}

static HCHAR*	apk_filemgr_MakeGroupSQL(AxFileMgrInfo_t *pstInfo, const HCHAR *pszKeyword, const HCHAR *pszPath, const HCHAR *pszFilter, HUINT32 ulMode )
{
	HCHAR	*pszExtensionSQL = NULL;
	HINT32	nStream;
	HCHAR	szKeyNoStr[10] = {0,};
	HERROR	hErr;

	nStream = pstInfo->nStream;

	hErr = apk_filemgr_GetKeywordNoStr(pszKeyword, szKeyNoStr);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("[%s] [%d] Unknown keyword!!!(%s) \n",__FUNCTION__,__LINE__,pszKeyword);
		return NULL;
	}

	HLIB_RWSTREAM_Rewind(nStream);
	if (!HxSTD_StrEmpty(pszFilter))
		pszExtensionSQL = apk_filemgr_MakeExtensionSQL(pstInfo, pszFilter);

	nStream = pstInfo->nStream;
	HLIB_RWSTREAM_Rewind(nStream);

	HLIB_RWSTREAM_Print(nStream," select * from DxFileInfo_t where ");
	if (ulMode & AxFILEMGR_BROWSE_MASK_SUBTYPE )
	{
		HCHAR szTypeSQL[128] = { 0 };
		apk_filemgr_MakeSubTypeSQL( ulMode, szTypeSQL, sizeof(szTypeSQL) );

		HLIB_RWSTREAM_Print(nStream,"%s and ",  szTypeSQL);
	}
	HLIB_RWSTREAM_Print(nStream," %s like '%s%%'", szKeyNoStr, pszKeyword);

	if (pszPath)
	{
		HLIB_RWSTREAM_Print(nStream," and path like \"%s%%\"", pszPath);
	}

	if (pszExtensionSQL)
	{
		HLIB_RWSTREAM_Print(nStream," and %s", pszExtensionSQL);
	}

	HLIB_RWSTREAM_Print(nStream," group by %s", szKeyNoStr);

	//HLIB_RWSTREAM_Print(nStream, " order by name  asc");

	return (HCHAR*)HLIB_RWSTREAM_GetBuf(nStream);
}

static HCHAR*	apk_filemgr_MakeSizeofPathSQL(AxFileMgrInfo_t *pstInfo, const HCHAR *pszPath, const HCHAR *pszFilter)
{
	HCHAR	*pszExtensionSQL = NULL;
	HINT32	nStream;
	nStream = pstInfo->nStream;
	HLIB_RWSTREAM_Rewind(nStream);

	if (!HxSTD_StrEmpty(pszFilter))
		pszExtensionSQL = apk_filemgr_MakeExtensionSQL(pstInfo, pszFilter);

	if (pszExtensionSQL)
		HLIB_RWSTREAM_Print(nStream, "select * from DxFileInfo_t where path like \"%s/%%\" and %s ",  pszPath, pszExtensionSQL);
	else
		HLIB_RWSTREAM_Print(nStream, "select * from DxFileInfo_t where path like \"%s/%%\"  ", pszPath);
	return (HCHAR*)HLIB_RWSTREAM_GetBuf(nStream);
}

static void		apk_filemgr_onSizeOfPath(const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	DAxIterator_t		iter;
	DxFileInfo_t		*pstFileInfo = NULL;
	HUINT64 *pulTotalSize = (HUINT64 *)userdata;

	if(pulTotalSize == NULL)
	{
		HxLOG_Error("[%s:%d] pstStatus is NULL\n", __FUNCTION__, __LINE__);
		return ;
	}
	*pulTotalSize = 0;

	DAPI_InitIterator(&iter, data, size, 0);
	while (DAPI_NextIterator(&iter))
	{
		pstFileInfo = (DxFileInfo_t*)DLIB_NewType(iter.name, iter.uid, iter.data, iter.size);
		if ( pstFileInfo )
		{
			*pulTotalSize += pstFileInfo->st_stat.st_size;

			DLIB_FreeType(iter.name, pstFileInfo);
		}
	}
}

static void		apk_filemgr_onDataUpdated(const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	AxFileMgrInfo_t		*pstInfo;
	AxFileMgrStatus_t	*pstStatus;

	DAxIterator_t		iter;
	DxFileInfo_t		*pstFileInfo;
	DxFileInfo_t		*pstNewFileInfo;

	pstStatus = (AxFileMgrStatus_t*)userdata;
	if(pstStatus == NULL)
	{
		HxLOG_Error("[%s:%d] pstStatus is NULL\n", __FUNCTION__, __LINE__);
		return ;
	}
	pstInfo = apk_filemgr_getInstance();

	HxLOG_Debug("[%s:%d] size : [%d] - ulBrowsingMode [%x] nRefer [%d]\n", __FUNCTION__, __LINE__, size, pstStatus->ulBrowsingMode, pstStatus->nRefer);
	DAPI_InitIterator(&iter, data, size, 0);
	while (DAPI_NextIterator(&iter))
	{
		pstFileInfo = (DxFileInfo_t*)DLIB_NewType(iter.name, iter.uid, iter.data, iter.size);
		if (pstFileInfo)
		{
			pstNewFileInfo = APK_MemDup(pstFileInfo, sizeof(DxFileInfo_t) + iter.size - sizeof(DxFileInfo_t));
			HLIB_PATH_Normalize(pstNewFileInfo->szFileName);

			//	force change file to dir for one depth folder view mode.
			if ( (pstStatus->ulBrowsingMode == eAxFILEMGR_MODE_ONEDEPTHVIEW) && (pstStatus->bDir == TRUE) && (pstStatus->nRefer == 2))
			{
				HCHAR	*p;

				p = HxSTD_StrRChar(pstNewFileInfo->szFileName, '/');
				if (p)
					*p = 0;
				pstNewFileInfo->st_stat.st_mode = eDxISDIR;
			}

			HxLOG_Debug("[%s:%d] filename [%s]\n", __FUNCTION__, __LINE__, pstNewFileInfo->szFileName);
			DLIB_FreeType(iter.name, pstFileInfo);

			pstStatus->pstResultList = HLIB_LIST_Append(pstStatus->pstResultList, (void*)pstNewFileInfo);
		}
	}

	if (pstStatus)
	{
		pstStatus->nRefer--;
		if (pstStatus->nRefer < 1)
		{
			APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

			//	수신측에서 pstResultList를 free.
			stApkEvtArgs.pfnFreeFunc = NULL;
			stApkEvtArgs.ulArgCount = 5;
			stApkEvtArgs.apArgV[0] = (void *)pstStatus->fnNotifier;
			stApkEvtArgs.apArgV[1] = (void *)pstStatus->nRequestId;
			stApkEvtArgs.apArgV[2] = (void *)NULL;
			stApkEvtArgs.apArgV[3] = (void *)pstStatus->pstResultList;
			stApkEvtArgs.apArgV[4] = (void *)NULL;
			APK_EVENT_Emit(&stApkEvtArgs);

			if(pstStatus->pszKey)
			{
				APK_Free(pstStatus->pszKey);
			}
			APK_Free(pstStatus);
		}
	}

	apk_filemgr_releaseInstance(pstInfo);
}

STATIC void		apk_filemgr_freeOnStatusChange(HINT32 nArgc, void *apArgV[])
{
	void	*pvData;

	HxLOG_Assert(nArgc == 5);

	pvData = apArgV[4];
	if (pvData)
		APK_Free(pvData);
}

static HERROR	apk_filemgr_onStatusChange(void *pvUserData, HINT32 nRequestId, UAPI_STATUS_t *pstUAPIStatus)
{
	AxFileMgrInfo_t		*pstInfo;
	AxFileMgrStatus_t	*pstStatus;

	pstInfo = apk_filemgr_getInstance();

	pstStatus = (AxFileMgrStatus_t*)HLIB_HASH_Lookup(pstInfo->pstRequestTable, (void*)nRequestId);
	if (pstStatus)
	{
		AxFileMgrCbInfo_t	*pstCbInfo;

		pstCbInfo = (AxFileMgrCbInfo_t*)APK_Malloc(sizeof(AxFileMgrCbInfo_t));
		if (pstCbInfo == NULL)
		{
			HxLOG_Error("%s() Out of memory!!\n", __FUNCTION__);
			return ERR_FAIL;
		}

		pstCbInfo->eState = eAxFILEMGR_ERR_OK;

		pstCbInfo->nDoneOperationNum  = pstUAPIStatus->nDoneOperationNum;
		pstCbInfo->nTotalOperationNum = pstUAPIStatus->nTotalOperationNum;

		if (pstUAPIStatus->ulErr & UMERR_ON_GOING)
			pstCbInfo->eState = eAxFILEMGR_ERR_ONGOING;
		if (pstUAPIStatus->ulErr & UMERR_REASON_CANCELED)
			pstCbInfo->eState = eAxFILEMGR_ERR_CANCELED;
		if (pstUAPIStatus->ulErr & UMERR_REASON_FAIL_TO_READ)
			pstCbInfo->eState = eAxFILEMGR_ERR_FAIL_READ;
		if (pstUAPIStatus->ulErr & UMERR_REASON_FAIL_TO_STAT)
			pstCbInfo->eState = eAxFILEMGR_ERR_FAIL_STAT;
		if (pstUAPIStatus->ulErr & UMERR_REASON_FAIL_TO_WRITE)
			pstCbInfo->eState = eAxFILEMGR_ERR_FAIL_WRITE;
		if (pstUAPIStatus->ulErr & UMERR_REASON_INVALID_ARG)
			pstCbInfo->eState = eAxFILEMGR_ERR_INVALID_ARG;
		if (pstUAPIStatus->ulErr & UMERR_REASON_NOT_ENOUGH_SPACE)
			pstCbInfo->eState = eAxFILEMGR_ERR_NOT_ENOUGH_SPACE;
		if (pstUAPIStatus->ulErr & UMERR_REASON_SIZE_LIMIT_4G)
			pstCbInfo->eState = eAxFILEMGR_ERR_4G_FAT;

		apk_filemgr_releaseInstance(pstInfo);

		if (pstStatus->fnNotifier)
		{
			APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

			stApkEvtArgs.pfnFreeFunc = apk_filemgr_freeOnStatusChange;
			stApkEvtArgs.ulArgCount = 5;
			stApkEvtArgs.apArgV[0] = (void *)pstStatus->fnNotifier;
			stApkEvtArgs.apArgV[1] = (void *)nRequestId;
			stApkEvtArgs.apArgV[2] = (void *)pstStatus->pvUserData;
			stApkEvtArgs.apArgV[3] = (void *)NULL;
			stApkEvtArgs.apArgV[4] = (void *)APK_MemDup(pstCbInfo, sizeof(AxFileMgrCbInfo_t));

			APK_EVENT_Emit(&stApkEvtArgs);
		}

		if (pstCbInfo->nDoneOperationNum == pstCbInfo->nTotalOperationNum)
		{
			pstInfo = apk_filemgr_getInstance();
			HLIB_HASH_Remove(pstInfo->pstRequestTable, (void*)nRequestId);
			apk_filemgr_releaseInstance(pstInfo);
		}

		if(pstCbInfo)
			APK_Free(pstCbInfo);

	} else
		apk_filemgr_releaseInstance(pstInfo);

	return ERR_OK;
}

static HERROR	apk_filemgr_AddRequestTable(AxFileMgrInfo_t *pstInfo, HINT32 nRequestId, APK_FILEMGR_Notifier_t fnNotifier, void *pvUserData)
{
	AxFileMgrStatus_t	*pstStatus;

	if (HLIB_HASH_Lookup(pstInfo->pstRequestTable, (void*)nRequestId))
	{
		HxLOG_Error("[%s:%d] Duplicate request table!!\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	pstStatus = (AxFileMgrStatus_t*)APK_Calloc(sizeof(AxFileMgrStatus_t));
	pstStatus->nRequestId  = nRequestId;
	pstStatus->fnNotifier = fnNotifier;
	pstStatus->pvUserData  = pvUserData;
	pstStatus->nRefer      = 1;
	HLIB_HASH_Insert(pstInfo->pstRequestTable, (void*)pstStatus->nRequestId, (void*)pstStatus);

	return ERR_OK;
}

static void apk_filemgr_FindSubtitle(AxFileMgrOperation_e eFileOper, const HCHAR *pszSource, const HCHAR *pszDest, const HCHAR *pszExtFilter)
{
	const HCHAR pszSubtitleList[9][64] = {"smi", "sami", "smil", "srt", "sub", "txt", "ass", "ssa", "psb"};
	HCHAR pszSrcPath[AxFILEMGR_PATH_LEN],pszDestPath[AxFILEMGR_PATH_LEN];
	HCHAR *pszExtension=NULL;
	HINT32 i;
	//HxLOG_Debug("Find subtitle[op:%d, src:%s]\n", eFileOper, pszSource);

	if ((pszSource == NULL) || (HLIB_FILE_Exist(pszSource)== FALSE))
		return;

	pszExtension = HxSTD_StrRChar(pszSource, '.');
	if (!pszExtension)
		return;

	pszExtension += 1;
	*pszExtension = '\0';

	for (i=0; i<9; i++)
	{
		HxSTD_memset(pszSrcPath,0x0,AxFILEMGR_PATH_LEN);
		HxSTD_memset(pszDestPath,0x0,AxFILEMGR_PATH_LEN);
		HxSTD_snprintf(pszSrcPath,AxFILEMGR_PATH_LEN,"%s%s",pszSource,pszSubtitleList[i]);

		if (HLIB_FILE_Exist(pszSrcPath)== TRUE)
		{
			if (0)
			{
				if (pszDest)
				{
					pszExtension = HxSTD_StrRChar(pszDest, '.');
					if(pszExtension)
					{
						pszExtension += 1;
						*pszExtension = '\0';
						HxSTD_snprintf(pszDestPath, AxFILEMGR_PATH_LEN, "%s%s", pszDest, pszSubtitleList[i]);
					}
					else
					{
						HxSTD_snprintf(pszDestPath, AxFILEMGR_PATH_LEN, "%s", pszDest);
					}
				}
			}

			if (eFileOper == eAxFILEMGR_OP_REMOVE)
			{
				UAPI_FM_Remove(pszSrcPath, "smi|sami|smil|srt|sub|txt|ass|ssa|psb", NULL, NULL);
			}
			else if (eFileOper == eAxFILEMGR_OP_COPY)
			{
				(void)UAPI_FM_Copy(pszSrcPath, pszDest, "smi|sami|smil|srt|sub|txt|ass|ssa|psb", FALSE, NULL, NULL);
			}
			else if (eFileOper == eAxFILEMGR_OP_MOVE)
			{
				(void)UAPI_FM_Move(pszSrcPath, pszDest, "smi|sami|smil|srt|sub|txt|ass|ssa|psb", FALSE, NULL, NULL);
			}

			/* No need to fnNotifier */
		}
	}

	return;
}

#if defined(CONFIG_DEBUG)
STATIC const HCHAR* s_NullStr = "";

const HCHAR* apk_filemgr_GetDataTypeString(const DxFileType_e	eDataType)
{
	const HCHAR* ret = s_NullStr;
	switch (eDataType)
	{
		case eDxFILETYPE_MUSIC:		ret = "MUSIC";		break;
		case eDxFILETYPE_EXIF:		ret = "EXIF";		break;
		case eDxFILETYPE_PNG:		ret = "PNG";		break;
		case eDxFILETYPE_RECORDINGS:		ret = "RecMeta";		break;
		case eDxFILETYPE_AVSTREAM:			ret = "AVStream";				break;
		case eDxFILETYPE_DIRECTORY: 	ret = "Directory";				break;

		case eDxFILETYPE_UNKNOWN:
		default:
			ret = "Unknown";
			break;
	}
	return ret;
}

static void		apk_filemgr_onAllFileInfoReceived(const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	DAxIterator_t		iter;
	DxFileInfo_t		*pstFileInfo;
	HUINT32			ulTotal = 0, aulCount[eDxFILETYPE_END] = {0,}, i;

	HLIB_CMD_Printf("\n[%s] [%d] \n================================ Scan File List ========================================= \n",__FUNCTION__,__LINE__);
	DAPI_InitIterator(&iter, data, size, 0);
	while (DAPI_NextIterator(&iter))
	{
		pstFileInfo = (DxFileInfo_t*)DLIB_NewType(iter.name, iter.uid, iter.data, iter.size);
		if (pstFileInfo)
		{
			HLIB_CMD_Printf(" uid : 0x%4llx,   eDataType : %8s, Interface : %4d,  Name: %s\n", pstFileInfo->uid, apk_filemgr_GetDataTypeString(pstFileInfo->eDataType), pstFileInfo->eInterface,pstFileInfo->szFileName);
			ulTotal++;
			aulCount[pstFileInfo->eDataType]++;
			DLIB_FreeType(iter.name, pstFileInfo);
		}
	}
	HLIB_CMD_Printf("========================================================================================= \n");
	HLIB_CMD_Printf("= %10s : %d \n", "Total", ulTotal);

	for (i = 0 ; i < eDxFILETYPE_END ; i++)
	{
		HLIB_CMD_Printf("= %-15s : %d\n", apk_filemgr_GetDataTypeString(i), aulCount[i]);
	}
	HLIB_CMD_Printf("========================================================================================= \n");

}

static HINT32	apk_filemgr_ShowFileList(void *pvArg)
{
	DAPI_GetAll(DxNAMEOF(DxFileInfo_t), apk_filemgr_onAllFileInfoReceived, NULL);

	return 0;
}

void apk_filemgr_Testcallback(int requestId, void *pvUserData, HxList_t *pstResultList, void *pvCbInfo)
//void OplFileBrowser::dama_callback(int requestId, HxList_t *pstResultList)//, HCHAR *pszPath)
{
	HxList_t *pstOrgResultList = pstResultList;
	int contentCount = HLIB_LIST_Length(pstResultList);
	int i;

	HLIB_CMD_Printf("===================================\n");
	HLIB_CMD_Printf("pstResultList address : %p \n", pstResultList);
	HLIB_CMD_Printf("file count            : %d \n", contentCount);

	if(0 < contentCount)
	{
		//1  ### event callback upper side
		for (i = 0; i < contentCount; i++)
		{
			HxList_t *pFound = HLIB_LIST_GetListItem(pstResultList, i);
			if(pFound)
			{
				DxFileInfo_t* pstFsInfo = (DxFileInfo_t*)HLIB_LIST_Data(pFound);

				HxID3_MusicInfo_t *pstMusicInfo = (HxID3_MusicInfo_t*)pstFsInfo->data;

				HLIB_CMD_Printf("Name : %s, Album: %s, Artist : %s\n", pstFsInfo->szFileName, pstMusicInfo->szAlbum, pstMusicInfo->szArtist);
			}
		}
	}
	APK_FILEMGR_FreeResultList(pstOrgResultList);
}

static HINT32	apk_filemgr_GroupTest(void *pvArg)
{
	HCHAR   		*aucArg=NULL;
	HCHAR			szStr[128] = {0, };

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&pvArg);
	if( aucArg == NULL ) 	return HxCMD_ERR;
	if( sscanf(aucArg, "%s", (char *)&szStr) == 0 )   	return HxCMD_ERR;

	HLIB_CMD_Printf("[%s] [%d]GroupTest : %s\n",__FUNCTION__,__LINE__, szStr);

	APK_FILEMGR_BrowserFiles(NULL, NULL, szStr,eAxFILEMGR_MODE_GROUP, apk_filemgr_Testcallback, NULL);

	return 0;
}
static HINT32	apk_filemgr_KeywordTest(void *pvArg)
{
	HCHAR   		*aucArg=NULL;
	HCHAR			szStr[128] = {0, };

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&pvArg);
	if( aucArg == NULL ) 	return HxCMD_ERR;
	if( sscanf(aucArg, "%s", (char *)&szStr) == 0 )   	return HxCMD_ERR;

	HLIB_CMD_Printf("[%s] [%d]KeywordTest : %s\n",__FUNCTION__,__LINE__, szStr);

	APK_FILEMGR_BrowserFiles(NULL, NULL, szStr,eAxFILEMGR_MODE_KEYWORD, apk_filemgr_Testcallback, NULL);
	return 0;
}

static void	apk_filemgr_InitCommand(void)
{
#define	hCmdHandle		"FILE"
	HLIB_CMD_RegisterWord (hCmdHandle, apk_filemgr_ShowFileList, "showfilelist", "show all scaned file list", "showfilelist");
	HLIB_CMD_RegisterWord (hCmdHandle, apk_filemgr_GroupTest, "grouptest", "show all scaned file list", "grouptest $(group keyword)");
	HLIB_CMD_RegisterWord (hCmdHandle, apk_filemgr_KeywordTest, "keytest", "show all scaned file list", "keytest $(keyword)");
}
#endif


HERROR	APK_FILEMGR_Init(void)
{
	apk_filemgr_releaseInstance(apk_filemgr_getInstance());
#if defined(CONFIG_DEBUG)
	apk_filemgr_InitCommand();
#endif

	return ERR_OK;
}

//	ulBrowingMode : refer AxFileMgrBrowsingMode_e
HERROR	APK_FILEMGR_BrowserFiles(const HCHAR *pszPath, const HCHAR *pszFilter, const HCHAR *pszKeyword, HUINT32 ulBrowsingMode, APK_FILEMGR_Notifier_t fnNotifier, HINT32 *pnTicket)
{
	AxFileMgrInfo_t	*pstInfo;
	AxFileMgrStatus_t	*pstStatus;
	HCHAR	*pszQuery;
	HUINT32	ulFlag = (ulBrowsingMode & 0xffff0000);

	pstStatus = (AxFileMgrStatus_t*)APK_Calloc(sizeof(AxFileMgrStatus_t));
	if(pstStatus == NULL)
	{
		HxLOG_Error("[%s:%d] APK_Calloc error!!\n",__FUNCTION__,__LINE__);
		return ERR_FAIL;
	}

	pstStatus->nRequestId = apk_filemgr_GetDummyId();
	pstStatus->fnNotifier = fnNotifier;
	pstStatus->ulBrowsingMode = ulBrowsingMode;
	pstStatus->nRefer = 0;

	pstInfo = apk_filemgr_getInstance();

	ulBrowsingMode = (ulBrowsingMode & 0x0000ffff);

	if (ulBrowsingMode == eAxFILEMGR_MODE_FOLDERVIEW)
	{
		if (HxSTD_StrEmpty(pszPath))
		{
			HxLOG_Error("[%s:%d] pszPath is Empty!\n", __FUNCTION__, __LINE__);
			goto ERROR_END;
		}

		if (ulFlag & AxFILEMGR_BROWSE_DIR)
		{
			pszQuery = apk_filemgr_MakeFolderViewSQL(pstInfo, pszPath, pszFilter, AxFILEMGR_BROWSE_DIR);
			pstStatus->bDir = TRUE;
			HxLOG_Print("[%s] [%d]pszQuery : %s\n",__FUNCTION__,__LINE__, pszQuery);

			if (DAPI_Query(pszQuery, apk_filemgr_onDataUpdated, (void*)pstStatus) == 0)
			{
				goto ERROR_END;
			}
			pstStatus->nRefer++;
		}

		if (ulFlag & AxFILEMGR_BROWSE_FILE)
		{
			pszQuery = apk_filemgr_MakeFolderViewSQL(pstInfo, pszPath, pszFilter, AxFILEMGR_BROWSE_FILE);
			pstStatus->bDir = FALSE;

			HxLOG_Print("[%s] [%d]pszQuery : %s\n",__FUNCTION__,__LINE__, pszQuery);
			if (DAPI_Query(pszQuery, apk_filemgr_onDataUpdated, (void*)pstStatus) == 0)
			{
				goto ERROR_END;
			}
			pstStatus->nRefer++;
		}
		else if ( ulFlag & AxFILEMGR_BROWSE_MASK_SUBTYPE )
		{
			pszQuery = apk_filemgr_MakeFolderViewSQL(pstInfo, pszPath, pszFilter,
														(ulFlag & AxFILEMGR_BROWSE_MASK_SUBTYPE) );
			pstStatus->bDir = FALSE;

			HxLOG_Print("[%s] [%d]pszQuery : %s\n",__FUNCTION__,__LINE__, pszQuery);
			if (DAPI_Query(pszQuery, apk_filemgr_onDataUpdated, (void*)pstStatus) == 0)
			{
				goto ERROR_END;
			}
			pstStatus->nRefer++;
		}

	}
	else 	if (ulBrowsingMode == eAxFILEMGR_MODE_ONEDEPTHVIEW)
	{
		if (HxSTD_StrEmpty(pszPath))
		{
			HxLOG_Error("[%s:%d] pszPath is Empty!\n", __FUNCTION__, __LINE__);
			goto ERROR_END;
		}

		if (ulFlag & AxFILEMGR_BROWSE_DIR)
		{
			pszQuery = apk_filemgr_MakeOneDepthViewSQL(pstInfo, pszPath, pszFilter, AxFILEMGR_BROWSE_DIR);
			pstStatus->bDir = TRUE;
			HxLOG_Print("[%s] [%d]pszQuery : %s\n",__FUNCTION__,__LINE__, pszQuery);
			if (DAPI_Query(pszQuery, apk_filemgr_onDataUpdated, (void*)pstStatus) == 0)
			{
				goto ERROR_END;
			}

			pstStatus->nRefer++;
		}

		if (ulFlag & AxFILEMGR_BROWSE_FILE)
		{
			pstStatus->bDir = FALSE;
			pszQuery = apk_filemgr_MakeOneDepthViewSQL(pstInfo, pszPath, pszFilter, AxFILEMGR_BROWSE_FILE);
			HxLOG_Print("[%s] [%d]pszQuery : %s\n",__FUNCTION__,__LINE__, pszQuery);

			if (DAPI_Query(pszQuery, apk_filemgr_onDataUpdated, (void*)pstStatus) == 0)
			{
				goto ERROR_END;
			}

			pstStatus->nRefer++;
		}
		else if ( ulFlag & AxFILEMGR_BROWSE_MASK_SUBTYPE )
		{
			pstStatus->bDir = FALSE;
			pszQuery = apk_filemgr_MakeOneDepthViewSQL(pstInfo, pszPath, pszFilter,
														(ulFlag & AxFILEMGR_BROWSE_MASK_SUBTYPE) );
			HxLOG_Print("[%s] [%d]pszQuery : %s\n",__FUNCTION__,__LINE__, pszQuery);

			if (DAPI_Query(pszQuery, apk_filemgr_onDataUpdated, (void*)pstStatus) == 0)
			{
				goto ERROR_END;
			}

			pstStatus->nRefer++;
		}
	}
	else 	if (ulBrowsingMode == eAxFILEMGR_MODE_KEYWORD)
	{
		if (HxSTD_StrEmpty(pszKeyword))
		{
			HxLOG_Error("[%s:%d] pszKeyword is Empty!\n", __FUNCTION__, __LINE__);
			goto ERROR_END;
		}

		pstStatus->pszKey = APK_StrDup(pszKeyword);

		pszQuery = apk_filemgr_MakeKeywordSQL(pstInfo, pszKeyword, pszPath, pszFilter,
												(ulFlag & AxFILEMGR_BROWSE_MASK_SUBTYPE) );
		if (HxSTD_StrEmpty(pszQuery))
		{
			HxLOG_Error("[%s:%d] pszQuery is Empty!\n", __FUNCTION__, __LINE__);
			goto ERROR_END;
		}
		HxLOG_Print("[%s] [%d]pszQuery : %s\n",__FUNCTION__,__LINE__, pszQuery);

		if (DAPI_Query(pszQuery, apk_filemgr_onDataUpdated, (void*)pstStatus) == 0)
		{
			goto ERROR_END;
		}

		pstStatus->nRefer++;
	}
	else 	if (ulBrowsingMode == eAxFILEMGR_MODE_GROUP)
	{
		if (HxSTD_StrEmpty(pszKeyword))
		{
			HxLOG_Error("[%s:%d] pszKeyword is Empty!\n", __FUNCTION__, __LINE__);
			goto ERROR_END;
		}

		pstStatus->pszKey = APK_StrDup(pszKeyword);

		pszQuery = apk_filemgr_MakeGroupSQL(pstInfo, pszKeyword, pszPath, pszFilter,
												(ulFlag & AxFILEMGR_BROWSE_MASK_SUBTYPE) );
		if (HxSTD_StrEmpty(pszQuery))
		{
			HxLOG_Error("[%s:%d] pszQuery is Empty!\n", __FUNCTION__, __LINE__);
			goto ERROR_END;
		}
		HxLOG_Print("[%s] [%d]pszQuery : %s\n",__FUNCTION__,__LINE__, pszQuery);

		if (DAPI_Query(pszQuery, apk_filemgr_onDataUpdated, (void*)pstStatus) == 0)
		{
			goto ERROR_END;
		}

		pstStatus->nRefer++;
	}
	else
	{
		pszQuery = NULL;
	}
	apk_filemgr_releaseInstance(pstInfo);

	if (pnTicket)	{*pnTicket = pstStatus->nRequestId;}

	if(pstStatus && pstStatus->nRefer == 0)
	{
		if(pstStatus->pszKey)
		{
			APK_Free(pstStatus->pszKey);
		}
		APK_Free(pstStatus);
	}

	return ERR_OK;

ERROR_END:

	if(pstStatus)
	{
		if(pstStatus->pszKey)
		{
			APK_Free(pstStatus->pszKey);
		}

		APK_Free(pstStatus);
	}

	apk_filemgr_releaseInstance(pstInfo);
	return ERR_FAIL;
}

HERROR	APK_FILEMGR_ScanDir(const HCHAR *pszSource, const HCHAR *pszExtFilter, HBOOL bRecursive, APK_FILEMGR_Notifier_t fnNotifier, HINT32 *pnRequestId)
{
	AxFileMgrInfo_t		*pstInfo;

	*pnRequestId = UAPI_FM_Scan(pszSource, pszExtFilter, bRecursive, NULL, apk_filemgr_onStatusChange);
	if (fnNotifier)		//	if pfnCallback is not existed, user want to add listener manually.
	{
		pstInfo = apk_filemgr_getInstance();
		apk_filemgr_AddRequestTable(pstInfo, *pnRequestId, fnNotifier, NULL);
		apk_filemgr_releaseInstance(pstInfo);
	}

	return ERR_OK;
}

HERROR	APK_FILEMGR_Remove(const HCHAR *pszSource, const HCHAR *pszExtFilter, HBOOL bRecursive, APK_FILEMGR_Notifier_t fnNotifier, HINT32 *pnRequestId)
{
	AxFileMgrInfo_t		*pstInfo;

	*pnRequestId = UAPI_FM_Remove(pszSource, pszExtFilter, NULL, apk_filemgr_onStatusChange);
	if (fnNotifier)
	{
		pstInfo = apk_filemgr_getInstance();
		apk_filemgr_AddRequestTable(pstInfo, *pnRequestId, fnNotifier, NULL);
		apk_filemgr_releaseInstance(pstInfo);
	}

	return ERR_OK;
}

HERROR	APK_FILEMGR_Copy(const HCHAR *pszSource, const HCHAR *pszDest, const HCHAR *pszExtFilter, HBOOL bDecryption, APK_FILEMGR_Notifier_t fnNotifier, HINT32 *pnRequestId)
{
	AxFileMgrInfo_t		*pstInfo;

	*pnRequestId = UAPI_FM_Copy(pszSource, pszDest, pszExtFilter, bDecryption, NULL, apk_filemgr_onStatusChange);
	if (fnNotifier)
	{
		pstInfo = apk_filemgr_getInstance();
		apk_filemgr_AddRequestTable(pstInfo, *pnRequestId, fnNotifier, NULL);
		apk_filemgr_releaseInstance(pstInfo);
	}

	return ERR_OK;
}

HERROR	APK_FILEMGR_Move(const HCHAR *pszSource, const HCHAR *pszDest, const HCHAR *pszExtFilter, HBOOL bDecryption, APK_FILEMGR_Notifier_t fnNotifier, HINT32 *pnRequestId)
{
	AxFileMgrInfo_t		*pstInfo;

	*pnRequestId = UAPI_FM_Move(pszSource, pszDest, pszExtFilter, bDecryption, NULL, apk_filemgr_onStatusChange);
	if (fnNotifier)
	{
		pstInfo = apk_filemgr_getInstance();
		apk_filemgr_AddRequestTable(pstInfo, *pnRequestId, fnNotifier, NULL);
		apk_filemgr_releaseInstance(pstInfo);
	}

	return ERR_OK;
}

HERROR	APK_FILEMGR_Cancel(HINT32 nRequestId, APK_FILEMGR_Notifier_t fnNotifier, HINT32 *pnRequestId)
{
	AxFileMgrInfo_t		*pstInfo;

	*pnRequestId = UAPI_FM_Cancel(nRequestId, NULL, apk_filemgr_onStatusChange);
	if (fnNotifier)
	{
		pstInfo = apk_filemgr_getInstance();
		apk_filemgr_AddRequestTable(pstInfo, *pnRequestId, fnNotifier, NULL);
		apk_filemgr_releaseInstance(pstInfo);
	}

	return ERR_OK;
}

HERROR	APK_FILEMGR_MakeDir(const HCHAR *pszSource, APK_FILEMGR_Notifier_t fnNotifier, HINT32 *pnRequestId)
{
	AxFileMgrInfo_t		*pstInfo;

	*pnRequestId = UAPI_FM_MkDir(pszSource, NULL, apk_filemgr_onStatusChange);
	if (fnNotifier)
	{
		pstInfo = apk_filemgr_getInstance();
		apk_filemgr_AddRequestTable(pstInfo, *pnRequestId, fnNotifier, NULL);
		apk_filemgr_releaseInstance(pstInfo);
	}


	return ERR_OK;
}

HERROR	APK_FILEMGR_RemoveDir(const HCHAR *pszSource, APK_FILEMGR_Notifier_t fnNotifier, HINT32 *pnRequestId)
{
	AxFileMgrInfo_t		*pstInfo;


	*pnRequestId = UAPI_FM_RmDir(pszSource, NULL, apk_filemgr_onStatusChange);
	if (fnNotifier)
	{
		pstInfo = apk_filemgr_getInstance();
		apk_filemgr_AddRequestTable(pstInfo, *pnRequestId, fnNotifier, NULL);
		apk_filemgr_releaseInstance(pstInfo);
	}

	return ERR_OK;
}

HERROR	APK_FILEMGR_GetSizeOfPath(const HCHAR *pszPath, const HCHAR *pszExtFilter, HUINT64 *pulSize )
{
	HCHAR	*pszQuery;
	AxFileMgrInfo_t		*pstInfo;
	HUINT32 	ulDapiReqId = 0;
	HERROR		hError = ERR_FAIL;

	pstInfo = apk_filemgr_getInstance();

	pszQuery = apk_filemgr_MakeSizeofPathSQL( pstInfo, pszPath, pszExtFilter );
	if (HxSTD_StrEmpty(pszQuery))
	{
		HxLOG_Error("pszQuery is empty\n");
		goto SIZE_ERROR;
	}

	ulDapiReqId = DAPI_Query(pszQuery, apk_filemgr_onSizeOfPath, (void*)pulSize);
	if ( 0 == ulDapiReqId )
	{
		HxLOG_Error("dapi query error. (%s)\n",  pszQuery);
		goto SIZE_ERROR;
	}
	HxLOG_Print("[%s] [%d]pszQuery : %s\n",__FUNCTION__,__LINE__, pszQuery);

	hError = DAPI_Wait(200, ulDapiReqId);
	if ( DERR_TIMEOUT == hError || DERR_FAIL == hError )
	{
		HxLOG_Error("dapi query timeout(%d). path(%s), ext(%s)\n", hError, pszPath, pszExtFilter);
		goto SIZE_ERROR;
	}

	apk_filemgr_releaseInstance(pstInfo);
	return ERR_OK;

SIZE_ERROR :
	apk_filemgr_releaseInstance(pstInfo);
	return ERR_FAIL;
}

HERROR	APK_FILEMGR_AddListener(HINT32 nRequestId, void *pvUser, APK_FILEMGR_Notifier_t fnNotifier)
{
	HERROR	err;
	AxFileMgrInfo_t		*pstInfo;

	err = UAPI_FM_AddListener(nRequestId, pvUser, apk_filemgr_onStatusChange);
	if (fnNotifier)
	{
		pstInfo = apk_filemgr_getInstance();
		apk_filemgr_AddRequestTable(pstInfo, nRequestId, fnNotifier, pvUser);
		apk_filemgr_releaseInstance(pstInfo);
	}

	return err;
}

HERROR	APK_FILEMGR_FreeResultList(HxList_t *pstResultList)
{
	HLIB_LIST_RemoveAllFunc(pstResultList, APK_Free_CB);

	return ERR_OK;
}


