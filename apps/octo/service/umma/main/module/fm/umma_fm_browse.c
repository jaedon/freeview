/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  umma_cache.c
	@brief

	Description:  									\n
	Module: Umma Toolkit Module						\n

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
#include	<stdlib.h>
#include	<dirent.h>

#include	<vkernel.h>
#include	<hlib.h>
#include	<dlib.h>

#include	<uapi.h>
#include	<thapi.h>
#include	<dapi.h>
#include	<umma_int.h>
#include	<umma_fm.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	UMMA_FM_BROWSE_STORAGE	"/mnt/hd2"

#define	UMMA_FM_BROWSE_DIR			".umma"
#define	UMMA_FM_BROWSE_MAX_NO		2048		//	1024 files per directory

#define UMMA_FM_BROWSE_EXCLUDE_FOLDER1 "."
#define UMMA_FM_BROWSE_EXCLUDE_FOLDER2 ".."

#define UMMA_FM_BROWSE_EXT_MAX 16
#define UMMA_FM_BROWSE_UPDATE_DEFAULT 16

#if defined(CONFIG_PROD_OS_EMULATOR)
#define stat emu_stat
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eDIR_BROWSE_DIR_OPEN = 0x00,
	eDIR_BROWSE_ITEM_SORT,
	eDIR_BROWSE_ITEM_READY,
	eDIR_BROWSE_ITEM_GETTING,
	eDIR_BROWSE_DIR_CLOSE,
} DIR_BROWSE_STATUS_e;

typedef enum
{
	eBROWSE_CMD_OPEN = 0x00,
	eBROWSE_CMD_CLOSE,
	eBROWSE_CMD_GETITEM,
	eBROWSE_CMD_SCANITEM,
	eBROWSE_CMD_SORT,
} UMMA_FM_BROWSE_CMD_e;

typedef struct
{
	HxList_t *pstBrowHndList;
	/* list로 해야 할까?? */
	HULONG ulBrowsingTaskId;
	HULONG ulBrowsingMsgId;
} UMMA_FM_BROWSE_t;

typedef struct
{
	HBOOL bSaved;
	HxList_t *pstNotFindList;
	HxList_t *pFileList;
} DAMA_FILE_CHK_t;

typedef struct
{
	HUINT32 nHndId;
	HINT32	nReqId;
	HUINT32 eSortType;
	HUINT32 eBrowseType;

	HCHAR szPath[512];
	HCHAR szFilter[256];

	/* scan result */
	HUINT32 nTotal;
	HUINT32 nDirCnt;
	struct dirent **ppDirEntry;

	/* processing meta */
	DIR_BROWSE_STATUS_e eStatus;
	HERROR (*cbResultEvt)(void *pvUmma, HINT32 nReqId, void *pstResult );
	void *pvUmma;
	DAMA_FILE_CHK_t stDamaChk;
	HBOOL bHiddenVisible;
	HBOOL bSubScan;
	HBOOL bMetaIgnore;
	HBOOL bThumbWait;
} UMMA_FM_BROWSE_HANDLE_t;

typedef struct
{
	HINT32 eCmd;
	HINT32 nReqId;
	UMMA_FM_BROWSE_HANDLE_t *pstHandle;
	HINT32 nParam1;
	HINT32 nParam2;
	HINT32 nParam3;
} UMMA_FM_BROWSE_MSG_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static UMMA_FM_BROWSE_t s_fm_dirContext = { 0 };
static HUINT32			s_fm_ulSemId =0;

static void umma_fm_browse_sortTask( UMMA_FM_BROWSE_HANDLE_t *pstBrowHnd, HUINT32 eSortType, HINT32 nReqId );
/*******************************************************************/
/********************      Functions     ***************************/
/*******************************************************************/
static UMMA_FM_BROWSE_t * umma_fm_browse_getContext(void)
{
	return &s_fm_dirContext;
}

static UMMA_FM_BROWSE_HANDLE_t * umma_fm_browse_findHanle( UMMA_FM_BROWSE_t *pstCtx, HUINT32 nHandleId )
{
	HxList_t *pList = NULL;
	UMMA_FM_BROWSE_HANDLE_t *pHandle = NULL;

	HxSEMT_Get(s_fm_ulSemId);
	pList = pstCtx->pstBrowHndList;

	while (pList)
	{
		pHandle = (UMMA_FM_BROWSE_HANDLE_t *)HLIB_LIST_Data(pList);
		if ( pHandle->nHndId == nHandleId)
		{
			break;
		}

		pList = HLIB_LIST_Next(pList);
	}
	HxSEMT_Release(s_fm_ulSemId);
	if ( !pList )
		return NULL;

	return pHandle;
}

static inline int umma_fm_browse_notifyResultEvent( UMMA_FM_BROWSE_HANDLE_t *pstBrowHnd, UAPI_FM_BROWSE_EVT_e eEvent, HINT32 nErrCode, HINT32 p1, HINT32 p2, HINT32 p3 )
{
	UAPI_BROWSE_RESULT_t *result;
	HUINT32 size =0,nextdatapos=0;

	if (NULL == pstBrowHnd || NULL ==pstBrowHnd->pvUmma )
	{
		return -1;
	}
	result = (UAPI_BROWSE_RESULT_t*)UMMA_Calloc(sizeof(UAPI_BROWSE_RESULT_t));
	if(result)
	{
		result->nHandleId	= pstBrowHnd->nHndId;
		result->eEvent		= eEvent;
		result->ulErr		= (HUINT32)nErrCode;
		result->nTotalNum	= pstBrowHnd->nTotal;

		if(ERR_OK == nErrCode)
		{
			if( eUAPI_FM_BROWSE_GETITEMS == eEvent || eUAPI_FM_BROWSE_SCANITEMS == eEvent )
			{
				HxList_t *pList 	= (HxList_t*)p3;
				result->nReqIdx		= p1;
				result->nDoneNum	= p2;

				while(pList)
				{
					DxFileInfo_t *pInfo;

					pInfo = HLIB_LIST_Data(pList);
					if(pInfo)
					{
						HCHAR *p;
						size += sizeof(DxFileInfo_t) + pInfo->size;
						result = (UAPI_BROWSE_RESULT_t*)UMMA_Realloc(result,sizeof(UAPI_BROWSE_RESULT_t) +size);
						p = (HCHAR*)result->Itemdata;// 1byte 씩접근
						p +=  nextdatapos;
						HxSTD_MemCopy(p,pInfo,sizeof(DxFileInfo_t) + pInfo->size);

						nextdatapos += sizeof(DxFileInfo_t) + pInfo->size;
						pList = HLIB_LIST_Next(pList);
					}
				}
			}
			result->nItemSize	= size;
		}

		if ( 0 == result->nDoneNum || NULL ==  result->Itemdata)
		{
			DxFileInfo_t stTemp;

			HxSTD_MemSet(&stTemp,0x0,sizeof(DxFileInfo_t));
			result = (UAPI_BROWSE_RESULT_t*)UMMA_Realloc(result,sizeof(UAPI_BROWSE_RESULT_t) +sizeof(DxFileInfo_t));
			HxSTD_MemCopy(result->Itemdata,&stTemp,sizeof(DxFileInfo_t));
			result->nItemSize = sizeof(DxFileInfo_t);
		}
		HxLOG_Print("[UMMA_FM_BROWSE] Send eEvent(%d), hndId(%d), reqId(%d)\n", eEvent, pstBrowHnd->nHndId, pstBrowHnd->nReqId );

		if(pstBrowHnd->cbResultEvt)
		{
			pstBrowHnd->cbResultEvt( pstBrowHnd->pvUmma, pstBrowHnd->nReqId, (void *)result );
		}
		UMMA_Free(result);
	}

	return 0;
}

static HERROR umma_fm_browse_getItemBaseInfo( const HCHAR *pszPath, DxFileInfo_t *pstFileInfo )
{
	struct stat fstat = { 0 };
	HCHAR pTemp[DLIB_FILEINFO_ONEDEPTH_LEN] = { 0 };
	HCHAR *p = NULL;
	if ( 0 != stat(pszPath, &fstat) )
	{
		HxLOG_Error("[UMMA_FM_BROWSE] getItemBaseInfo errno(%d), path(%s)\n", errno, pszPath );
		return ERR_FAIL;
	}

	DLIB_FILEINFO_ConvertStatToDxStat(&fstat, &pstFileInfo->st_stat);

	pstFileInfo->uid = UMMA_FM_IFACE_GetNewUid(&fstat);
	pstFileInfo->eInterface = eDxINTERFACE_UNKNOWN;

	HLIB_STD_StrNCpySafe(pTemp, pszPath, sizeof(pTemp));
	p = HLIB_PATH_BasePath(pTemp);

	HLIB_STD_StrNCpySafe(pstFileInfo->szOnedepth, p, sizeof(pstFileInfo->szOnedepth));
	HLIB_STD_StrNCpySafe(pstFileInfo->szFileName, pszPath, sizeof(pstFileInfo->szFileName));

	return ERR_OK;
}

static int	umma_fm_browse_selectCb( const struct dirent *item, HCHAR *pszFilter, HBOOL bHiddenVisible )
{
	HCHAR	*itemName = NULL;
	HCHAR	*ext = NULL;
	HCHAR	*find = NULL;
	HCHAR	*pos = NULL;
	HINT32	extLen = 0;
	HINT32  cnt = 0, max_cnt = 3;

	itemName = (HCHAR *)item->d_name;

	/* check hidden file or directory  */
	if ( !bHiddenVisible && '.' == itemName[0] && '.' != itemName[1] )
	{
		return FALSE;
	}

	if ( DT_DIR == item->d_type )
	{
		/* check exclude directory */
		if ( !HxSTD_StrCmp( itemName,"." ) || !HxSTD_StrCmp( itemName,".." ) )
		{
			return FALSE;
		}
		else if( '.' == itemName[0] && HxSTD_StrStr(UMMA_FM_EXCLUDE_SYSPATH, itemName ))
		{
			return FALSE;
		}
		else if ( 0 == HxSTD_StrCmp(UMMA_FM_EXCLUDE_RECYCLEBIN, itemName) )
		{
			return FALSE;
		}
		return TRUE;
	}

	if ( !pszFilter )
	{
		return TRUE;
	}

	/* check file Extension */
	ext = HLIB_PATH_Extension( itemName );
	if ( NULL == ext )
	{
		return FALSE;
	}

	extLen = HxSTD_StrLen(ext);
	pos = pszFilter;
	while ( NULL != (find = HLIB_STD_StrCaseStr( pos, ext) ) && cnt < max_cnt )
	{
		pos = find + extLen;
		if ( *pos == '|' || *pos == '\0' )
	{
		return TRUE;
	}
		++cnt;
	}
	return FALSE;
}

static HERROR umma_fm_browse_subDirHasItem( const HCHAR *pszPath, HCHAR *pszMainItem, HINT32 bufSize, const HCHAR *pszFilter,
		HBOOL *pbHasItem, HINT32 nRecusiveDepth )
{
	HINT32 dirCnt = 0;
	DIR *dir = NULL;
	struct dirent *d = NULL;
	char szBuf[512] = {0};
	HCHAR *pFilter = NULL;

	pFilter = HxSTD_StrLen(pszFilter) ? (HCHAR *)pszFilter : NULL;
	if ( NULL == pbHasItem || HxSTD_StrEmpty(pszPath) )
		return ERR_FAIL;

	if(!(dir = opendir(pszPath)))
	{
		HxLOG_Error("opendir is failed(%s)\n", pszPath);
		return ERR_FAIL;
	}
	while( NULL != (d = readdir(dir)) )
	{
		if( TRUE != umma_fm_browse_selectCb(d, pFilter, FALSE) )
		{
			continue;
		}
		if ( DT_REG == d->d_type ) /* finded sub Item*/
		{
			*pbHasItem = TRUE;
			if ( pszMainItem && bufSize )
			{
				HxSTD_snprintf( pszMainItem, bufSize,"%s/%s", pszPath, d->d_name );

			}
			break;
		}
		else if ( DT_DIR == d->d_type )
			++dirCnt;
	}

	rewinddir(dir);
	if ( TRUE != *pbHasItem && dirCnt && 0 < nRecusiveDepth )
	{
		while( NULL != (d = readdir(dir)) )
		{
			if ( DT_DIR != d->d_type )
			{
				continue;
			}

			if ( !HxSTD_StrCmp( d->d_name,"." ) || !HxSTD_StrCmp( d->d_name,".." ) )
			{
				continue;
			}

			HxSTD_snprintf(szBuf, sizeof(szBuf), "%s/%s", pszPath, d->d_name);
			umma_fm_browse_subDirHasItem( szBuf, NULL, 0, pFilter, pbHasItem, nRecusiveDepth-1 );
			if ( TRUE == *pbHasItem )
				break;
		}
	}
	closedir(dir);
	return ERR_OK;
}

/*scandir 사용하지 않고 직접 구현 */
static HERROR umma_fm_browse_scanning( UMMA_FM_BROWSE_HANDLE_t *pstBrowHnd, HINT32 *pItemCnt, struct dirent ***namelist  )
{
	HINT32	itemCnt = 0;
	HCHAR *pszFilter = NULL;
	HINT32 arraysz = 0;

	DIR *dir = NULL;
	struct dirent *d = NULL, *p = NULL, **items = NULL;

	pszFilter = HxSTD_StrLen(pstBrowHnd->szFilter) ? pstBrowHnd->szFilter : NULL;

	if(!namelist)
		return -1;

	if(!(dir = opendir(pstBrowHnd->szPath)))
	{
		HxLOG_Error("opendir is failed(%s). errno(%d)\n", pstBrowHnd->szPath, errno);
		return errno;
	}

	arraysz = 64;
	items = (struct dirent **)HLIB_MEM_Malloc(arraysz * sizeof(struct dirent *));
	if (items == NULL)
	{
		goto FAIL;
	}

	while( NULL != (d = readdir(dir)) )
	{
		/* filtering */
		if( TRUE != umma_fm_browse_selectCb(d, pszFilter, pstBrowHnd->bHiddenVisible) )
		{
			continue;
		}

		if ( DT_DIR == d->d_type )
			++pstBrowHnd->nDirCnt;

		p = (struct dirent *)malloc( sizeof(struct dirent) );
		if (p == NULL)
		{
			goto FAIL;
		}
		HxSTD_MemCopy( p, d, sizeof(struct dirent) );

		if (itemCnt >= arraysz)
		{
			struct dirent **temp;

			temp = (struct dirent **)HLIB_MEM_Realloc((char *)items,
					(arraysz * 2) * sizeof(struct dirent *));
			if (temp == NULL) {
				HLIB_MEM_Free(p);
				goto FAIL;
			}
			items = temp;
			arraysz *= 2;
		}
		items[itemCnt++] = p;
	}

	closedir(dir);

	pstBrowHnd->nTotal = itemCnt;
	*pItemCnt = itemCnt;
	*namelist = items;
	return ERR_OK;

FAIL:
	/* Memory Free */
	while(itemCnt > 0 )
	{
		HLIB_MEM_Free(items[--itemCnt]);
	}
	HLIB_MEM_Free(items);
	closedir(dir);
	return ERR_FAIL;
}

static int umma_fm_browse_sortByAlpha(const void *param1, const void *param2 )
{
	struct dirent *item1 = *((struct dirent **)param1);
	struct dirent *item2 = *((struct dirent **)param2);

	if ( item1->d_type == item2->d_type )
	{
		return strcasecmp( item1->d_name, item2->d_name );
	}
	else
	{
		return (DT_DIR == item1->d_type) ? -1 : 1;
	}

	return 0;
}

static HERROR umma_fm_browse_releaseHandle( UMMA_FM_BROWSE_HANDLE_t *pstBrowHnd )
{
	HINT32 nItemCount = 0;
	HINT32 i = 0;

	if ( NULL == pstBrowHnd )
		return ERR_FAIL;

	nItemCount = pstBrowHnd->nTotal;
	/* free */
	for ( i = 0; i < nItemCount; i++ )
	{
		free(pstBrowHnd->ppDirEntry[i]);
	}
	if ( pstBrowHnd->ppDirEntry )
		free(pstBrowHnd->ppDirEntry);

	pstBrowHnd->ppDirEntry = NULL;

	free(pstBrowHnd);
	return ERR_OK;
}

HBOOL umma_fm_browse_compCallback(void *userdata, void *data)
{
	HCHAR *user = (HCHAR*)userdata;
	DxFileInfo_t *item = (DxFileInfo_t*)data;

	if(!HxSTD_StrEmpty(user) && !HxSTD_StrEmpty(item->szFileName) && HxSTD_StrCmp(user,item->szFileName) == 0)
	{
		return TRUE;
	}
	return FALSE;
}

static HERROR umma_fm_browse_getDetailInfoFromFile(DxFileInfo_t *pstFileInfo,DAMA_FILE_CHK_t *pstFileCheck)
{
	UMMA_FM_IFACE_t	*pstIFaceInfo = NULL;
	HCHAR szExt[10], *pszExt = NULL;
	HINT32 nSize = 0;
	void *pvData = NULL;
	DxFileInfo_t *pInfo;
	HCHAR		*pszPath;

	pszPath = pstFileInfo->szFileName;
	pszExt = HLIB_PATH_Extension(pszPath);
	if(HxSTD_StrEmpty(pszExt))
	{
		HxLOG_Error("[%s:%d] Extension is NULL! Path[%s])\n", __FUNCTION__, __LINE__,pszPath);
		return ERR_FAIL;
	}
	HxSTD_StrNCpy( szExt, pszExt, sizeof(szExt)-1 );
	HLIB_STD_StrLower(szExt);
	pstIFaceInfo = UMMA_FM_IFACE_GetIfaceInfo(szExt);

	if( NULL==pstIFaceInfo || ERR_OK != pstIFaceInfo->getFileInfo(pszPath, &pvData, &nSize) )
	{
		HxLOG_Error("[%s:%d] pstIFaceInfo[0x%0x], Ext[%s]\n", __FUNCTION__, __LINE__, pstIFaceInfo, pszExt);
		return ERR_FAIL;
	}

	pstFileCheck->pFileList = HLIB_LIST_RemoveIf(pstFileCheck->pFileList,pstFileInfo->szFileName,umma_fm_browse_compCallback,UMMA_Free_CB);

	pInfo = UMMA_Calloc(sizeof(DxFileInfo_t) + nSize);

	pInfo->uid = pstFileInfo->uid;
	HxSTD_MemCopy(&pInfo->st_stat, &pstFileInfo->st_stat, sizeof(DxFileState_t));
	pInfo->eDataType = pstIFaceInfo->eType;

	HLIB_STD_StrNCpySafe(pInfo->szOnedepth, pstFileInfo->szOnedepth, sizeof(pInfo->szOnedepth));
	HLIB_STD_StrNCpySafe(pInfo->szFileName, pstFileInfo->szFileName, sizeof(pInfo->szFileName));

	pInfo->size = nSize;

	HxSTD_MemCopy(pInfo->data, pvData, nSize);
	if (pstIFaceInfo->freeInfo)
		pstIFaceInfo->freeInfo(pvData);

	pstFileCheck->pFileList = HLIB_LIST_Append(pstFileCheck->pFileList,pInfo);

	return ERR_OK;
}

static	DxFileInfo_t * umma_fm_browse_findItemByUid( HUINT64 ulUid, HxList_t *pstList)
{
	HxList_t 	 *pList;
	DxFileInfo_t *pInfo;

	pList = pstList;

	while (pList)
	{
		pInfo = (DxFileInfo_t*)HLIB_LIST_Data(pList);
		if(pInfo->uid == ulUid)
		{
			return pInfo;
		}
		pList = HLIB_LIST_Next(pList);
	}
	return NULL;
}

static HBOOL umma_fm_browse_findUidCb( void *userData, void *listData )
{
	HUINT64 *pFindUid = (HUINT64 *)userData;
	HUINT64 *pLIstUid = (HUINT64 *)listData;

	if ( *pFindUid == *pLIstUid )
		return TRUE;
	return FALSE;
}
static  void umma_fm_browse_cbGetMetaInfoList (const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	DAMA_FILE_CHK_t 	*pstDamaChk = (DAMA_FILE_CHK_t *)userdata;
	DAxIterator_t		 stIter;
	DxFileInfo_t		*pstDbItemInfo = NULL;
	DxFileInfo_t		*pstFileInfo = NULL;
	HxList_t 			*pUidList = NULL;
	HxList_t 			*pFindItem = NULL;
	HxList_t			*pstDamaChkList = NULL;
	HUINT64 *pUidTmp = NULL;
	HINT32 	i = 0;
	HINT32 	uidCnt = 0;
	DAPI_InitIterator(&stIter, data, size, 0);

	while (DAPI_NextIterator(&stIter))
	{
		pstDbItemInfo = (DxFileInfo_t*)DLIB_NewType(stIter.name, stIter.uid, stIter.data, stIter.size);
		if ( NULL == pstDbItemInfo)
		{
			continue;
		}

		pstFileInfo = umma_fm_browse_findItemByUid( pstDbItemInfo->uid,	pstDamaChk->pFileList);

		if ( NULL == pstFileInfo )
		{
			DLIB_FreeType(stIter.name, pstDbItemInfo);
			pstDbItemInfo = NULL;
			continue;
		}

		if ( 0 != HxSTD_StrCmp( pstFileInfo->szFileName, pstDbItemInfo->szFileName) ||
			eDxFILETYPE_UNKNOWN == pstDbItemInfo->eDataType )
		{
			DLIB_FreeType(stIter.name, pstDbItemInfo);
			pstDbItemInfo = NULL;
			continue;
		}

		pUidTmp = NULL;
		pstDamaChk->pFileList = HLIB_LIST_Remove(pstDamaChk->pFileList,pstFileInfo);
		UMMA_Free(pstFileInfo);
		pstDamaChk->pFileList = HLIB_LIST_Append(pstDamaChk->pFileList,UMMA_MemDup(pstDbItemInfo,stIter.size));

		pUidTmp = (HUINT64 *)UMMA_Calloc(sizeof(HUINT64));
		*pUidTmp = pstDbItemInfo->uid;
		pUidList = HLIB_LIST_Append( pUidList, (void *)pUidTmp );
		++uidCnt;

		DLIB_FreeType(stIter.name, pstDbItemInfo);
		pstDbItemInfo = NULL;
	}

	pUidTmp = NULL;

	pstDamaChkList = pstDamaChk->pFileList;
	while(pstDamaChkList)
	{
		DxFileInfo_t *pInfo;
		pInfo = (DxFileInfo_t*)HLIB_LIST_Data(pstDamaChkList);
		pFindItem = HLIB_LIST_FindEx(pUidList, (void*)&pInfo->uid, umma_fm_browse_findUidCb);
		if ( !pFindItem )
		{
			pstDamaChk->pstNotFindList = HLIB_LIST_Append( pstDamaChk->pstNotFindList, UMMA_MemDup(pInfo,sizeof(DxFileInfo_t) + pInfo->size));
		}
		else
		{
			pUidTmp = (HUINT64 *)HLIB_LIST_Data(pFindItem);
			if ( pUidList == pFindItem )
				pUidList = HLIB_LIST_Remove( pFindItem, pFindItem->data );
			else
				HLIB_LIST_Remove( pFindItem, pFindItem->data );

			if ( pUidTmp )
			{
				UMMA_Free( pUidTmp );
				pUidTmp = NULL;
			}
		}
		pstDamaChkList = HLIB_LIST_Next(pstDamaChkList);
	}
	if ( pUidList )
	{
		HUINT32 size =  HLIB_LIST_Length( pUidList );
		HxLOG_Error( "[UMMA_FM_BROSE] Not Release Memory of List Instance. size(%ld), i(%d), uidCnt(%d)\n", size, i, uidCnt);
		HLIB_LIST_RemoveAllFunc( pUidList, UMMA_Free_CB );
		pUidList = NULL;
	}
}

static HBOOL umma_fm_browse_getDetailInfoListFromDama( DAMA_FILE_CHK_t *pstDamaChk,HINT32 nSize)
{
	HUINT32 	ulDapiReqId = 0;
	HCHAR		szQuery[1024] = { 0 };
	HCHAR 		szUid[32] = { 0 };
	HERROR		hError = ERR_FAIL;
	HINT32 		i = 0;
	HBOOL		bRet = TRUE;
	HxList_t	*pstDamaChkList = NULL;
	DxFileInfo_t *pInfo;

	if ( 0 == nSize )
	{
		return TRUE;
	}

	pstDamaChkList = pstDamaChk->pFileList;

	/* generate SQL Query */
	HxSTD_snprintf( szQuery, sizeof( szQuery ), "select * from DxFileInfo_t where uid in (" );

	while(pstDamaChkList)
	{
		pInfo = (DxFileInfo_t*)HLIB_LIST_Data(pstDamaChkList);
		if (i++ > 0)
			HxSTD_StrNCat( szQuery, ",", sizeof(szQuery) - HxSTD_StrLen(szQuery) -1 );
		HxSTD_snprintf( szUid, sizeof( szUid ), " %lld",  pInfo->uid );
		HxSTD_StrNCat( szQuery, szUid, sizeof(szQuery) - HxSTD_StrLen(szQuery) -1 );
		pstDamaChkList = HLIB_LIST_Next(pstDamaChkList);
	}
	HxSTD_StrNCat( szQuery, " ) ", sizeof(szQuery) - HxSTD_StrLen(szQuery) -1 );
	HxLOG_Print("[UMMA_FM_BROWSE] Dama Query : %s\n", szQuery );
	ulDapiReqId = DAPI_Query( szQuery, umma_fm_browse_cbGetMetaInfoList, (void *)pstDamaChk);

	hError = DAPI_Wait(1500, ulDapiReqId);
	if ( DERR_TIMEOUT == hError || DERR_FAIL == hError )
	{
		HxLOG_Error("[UMMA_FM_BROWSE] detail Info Error(%d).\n", hError);
		if ( NULL ==  pstDamaChk->pstNotFindList )
		{
			pstDamaChkList = pstDamaChk->pFileList;
			while(pstDamaChkList)
			{
				pInfo = HLIB_LIST_Data(pstDamaChkList);
				pstDamaChk->pstNotFindList = HLIB_LIST_Append( pstDamaChk->pstNotFindList, UMMA_MemDup(pInfo,sizeof(DxFileInfo_t) + pInfo->size));
				pstDamaChkList = HLIB_LIST_Next(pstDamaChkList);
			}
		}
		bRet = FALSE;
	}
	return bRet;
}

static HERROR umma_fm_browse_setDetailInfoToDama( DxFileInfo_t *pstFileInfo )
{
	DAPI_LazySet(DxNAMEOF(DxFileInfo_t), pstFileInfo->uid, pstFileInfo, sizeof(DxFileInfo_t) + pstFileInfo->size, 100);
	return ERR_OK;
}

static void umma_fm_browse_openTask( UMMA_FM_BROWSE_HANDLE_t *pstBrowHnd, HINT32 nReqId )
{
	HINT32 itemCnt = 0;
	HERROR err = ERR_FAIL;

	pstBrowHnd->eStatus = eDIR_BROWSE_DIR_OPEN;

	/* 1. dir_entry Access and Filtering */
	HxLOG_Print( "[UMMA_FM_BROWSE_OPEN] Folder : %s\n", pstBrowHnd->szPath );
	err = umma_fm_browse_scanning( pstBrowHnd, &itemCnt, &pstBrowHnd->ppDirEntry );
	if ( ERR_OK != err )
	{
		umma_fm_browse_notifyResultEvent( pstBrowHnd, eUAPI_FM_BROWSE_OPEN, ERR_FAIL, 0, 0, 0 );
		return ;
	}

	/* 2. Notify Item Count */
	HxLOG_Print( "[UMMA_FM_BROWSE_OPEN] Filtered Item : %d \n", itemCnt );
	umma_fm_browse_notifyResultEvent( pstBrowHnd, eUAPI_FM_BROWSE_OPEN, ERR_OK, 0, 0, 0 );

}

static void umma_fm_browse_getItemsTask( UMMA_FM_BROWSE_HANDLE_t *pstBrowHnd, HINT32 nReqIndex, HINT32 nReqCount, HINT32 nReqId )
{
	struct dirent	**ppstItemList = NULL;
	HINT32 itemCnt = 0;
	HINT32 subIdx = 0;
	HINT32 i = 0;
	HINT32 reqIdx = 0, reqCnt = 0, overIdx = 0;
	HBOOL isDir = FALSE;
	HCHAR fullPath[1024] = { 0 };
	UAPI_FM_BROWSE_EVT_e eEvent = eUAPI_FM_BROWSE_GETITEMS;
	HxList_t 		*pNotFindList = NULL;
	HxList_t 		*pItemList = NULL;
	DxFileInfo_t 	*pFileInfo = NULL;
	DxFileInfo_t 	*pDamaFileInfo = NULL;

	if ( 0 == nReqId )
		eEvent = eUAPI_FM_BROWSE_SCANITEMS;

	reqIdx = nReqIndex;
	reqCnt = nReqCount;
	if( reqIdx < 0  || reqCnt <= 0 )
	{
		umma_fm_browse_notifyResultEvent( pstBrowHnd, eEvent, ERR_FAIL, 0, 0, 0 );
		return ;
	}

	overIdx = reqIdx + reqCnt;
	itemCnt = pstBrowHnd->nTotal;
	if ( 0 == itemCnt )
	{
		umma_fm_browse_notifyResultEvent( pstBrowHnd, eEvent, ERR_OK, 0, 0, 0 );
		return ;
	}

	if ( overIdx >= itemCnt )
	{
		overIdx = itemCnt;
		reqCnt = overIdx - reqIdx;
	}

	ppstItemList = pstBrowHnd->ppDirEntry;

	pstBrowHnd->eStatus = eDIR_BROWSE_ITEM_GETTING;

	HxSTD_MemSet( &pstBrowHnd->stDamaChk, 0x00, sizeof(DAMA_FILE_CHK_t) );
	for( i = reqIdx; i < overIdx; ++i, ++subIdx )
	{
		pFileInfo = (DxFileInfo_t *)UMMA_Calloc(sizeof(DxFileInfo_t));

		HxSTD_snprintf(fullPath, sizeof(fullPath), "%s/%s", pstBrowHnd->szPath, ppstItemList[i]->d_name);
		HxLOG_Print( "[UMMA_FM_GetItems] item(%d) name : %s\n", i,  ppstItemList[i]->d_name );
		umma_fm_browse_getItemBaseInfo( fullPath, pFileInfo );
		pstBrowHnd->stDamaChk.pFileList = HLIB_LIST_Append(pstBrowHnd->stDamaChk.pFileList,pFileInfo);

	}

	umma_fm_browse_getDetailInfoListFromDama( &pstBrowHnd->stDamaChk, subIdx);

	pNotFindList = pstBrowHnd->stDamaChk.pstNotFindList;
	if ( pNotFindList )
	{
		while ( pNotFindList )
		{
			pFileInfo = (DxFileInfo_t *)HLIB_LIST_Data(pNotFindList);

			isDir = (eDxISDIR == pFileInfo->st_stat.st_mode) ? TRUE : FALSE;
			HxLOG_Print( "[UMMA_FM_GetItems] detected new item name : %s\n", pFileInfo->szFileName );

			if ( isDir )
			{
				pFileInfo->eDataType = eDxFILETYPE_DIRECTORY;
			}
			else
			{
				if ( ERR_OK == umma_fm_browse_getDetailInfoFromFile(pFileInfo, &pstBrowHnd->stDamaChk) )
				{
					/* Not Need */
					//UMMA_FM_CACHE_AddFileInfo( pFileInfo, sizeof(DxFileInfo_t) + pFileInfo->size);
					pDamaFileInfo = umma_fm_browse_findItemByUid(pFileInfo->uid, pstBrowHnd->stDamaChk.pFileList);
					if (pDamaFileInfo)
						UMMA_FM_IFACE_AddFileInfoDirect( (void *)pDamaFileInfo );
				}
			}
			pNotFindList = HLIB_LIST_Next(pNotFindList);
		}
		pstBrowHnd->stDamaChk.pstNotFindList = HLIB_LIST_RemoveAllFunc(pstBrowHnd->stDamaChk.pstNotFindList,UMMA_Free_CB);
	}

	pItemList = pstBrowHnd->stDamaChk.pFileList = HLIB_LIST_First(pItemList = pstBrowHnd->stDamaChk.pFileList);
	pItemList = pstBrowHnd->stDamaChk.pFileList;
	while(pItemList)
	{
		pFileInfo = (DxFileInfo_t*)HLIB_LIST_Data(pItemList);
		switch (pFileInfo->eDataType)
		{
			case	eDxFILETYPE_MUSIC:
			case	eDxFILETYPE_EXIF:
			case	eDxFILETYPE_PNG:
				THAPI_AddThumbnailCache((HCHAR*)pFileInfo->szFileName, 0, NULL, TRUE);
				break;
			case eDxFILETYPE_AVSTREAM:
				{
					HUINT32 ulTime;
					HxAV_SteamInfo_t *pstAVInfo = (HxAV_SteamInfo_t*)pFileInfo->data;

					ulTime = (0 < pstAVInfo->nDuration && pstAVInfo->nDuration < UMMA_FM_THUMBNAIL_EXTRACT_TIME) ? pstAVInfo->nDuration : UMMA_FM_THUMBNAIL_EXTRACT_TIME;
					THAPI_AddThumbnailCache((HCHAR*)pFileInfo->szFileName, ulTime, NULL, TRUE);
				}
			default :
				break;
		}
		pItemList = HLIB_LIST_Next(pItemList);
	}
	/* 6. notify itemList */
	umma_fm_browse_notifyResultEvent( pstBrowHnd, eEvent, ERR_OK, reqIdx, reqCnt, (HINT32)pstBrowHnd->stDamaChk.pFileList);
	HxLOG_Print( "[UMMA_FM_GetItems] notify getItems(reqIdx:%d/reqCnt:%d)\n", reqIdx, reqCnt );

	if (pstBrowHnd->stDamaChk.pFileList)
		pstBrowHnd->stDamaChk.pFileList = HLIB_LIST_RemoveAllFunc(pstBrowHnd->stDamaChk.pFileList,UMMA_Free_CB);
	pstBrowHnd->eStatus = eDIR_BROWSE_ITEM_READY;
}

static void umma_fm_browse_scanItemsTask( UMMA_FM_BROWSE_HANDLE_t *pstBrowHnd, HINT32 nReqIndex, HINT32 nReqCount, HINT32 nReqId )
{
	HINT32 reqIdx = 0, dirCnt = 0;

	reqIdx = nReqIndex;
	if ( 0 == nReqIndex && eDIR_BROWSE_DIR_OPEN == pstBrowHnd->eStatus )
	{
		umma_fm_browse_sortTask( pstBrowHnd, eUAPI_FM_BROWSE_SORT_NAME_ALPHA, 0 );

		if ( pstBrowHnd->nDirCnt )
		{
			struct dirent	**ppstItemList = NULL;
			HINT32 k = 0, i = 0;
			HINT32 reqCnt = 0, subIdx = 0;
			HxList_t	 *pItemList = NULL;
			DxFileInfo_t *pFileInfo = NULL;
			HCHAR fullPath[DLIB_FILEINFO_NAME_LEN] = { 0 };
			HCHAR mainItem[DLIB_FILEINFO_NAME_LEN] = { 0 };
			HBOOL bHasItem = FALSE;
			DxDirInfo_t stDirInfo;

			dirCnt = pstBrowHnd->nDirCnt;

			ppstItemList = pstBrowHnd->ppDirEntry;
			for ( k = 0 ; k < dirCnt; k += nReqCount )
			{
				reqCnt = ( (k + nReqCount) <= dirCnt) ? nReqCount : (dirCnt - k) ;

				for( i = 0; i < reqCnt; ++i, ++subIdx )
				{
					HxSTD_MemSet( &stDirInfo, 0x00, sizeof(DxDirInfo_t) );
					pFileInfo = (DxFileInfo_t*)UMMA_Calloc(sizeof(DxFileInfo_t) + sizeof(DxDirInfo_t));
					if(pFileInfo)
					{
						HxSTD_snprintf(fullPath, sizeof(fullPath), "%s/%s", pstBrowHnd->szPath, ppstItemList[subIdx]->d_name);
						umma_fm_browse_getItemBaseInfo( fullPath, pFileInfo );
						pFileInfo->eDataType = eDxFILETYPE_DIRECTORY;

						umma_fm_browse_subDirHasItem( fullPath, mainItem, sizeof(mainItem),
								pstBrowHnd->szFilter, &bHasItem, (2-1) ); // 2depth
						HxSTD_StrNCpy(stDirInfo.szRepFile, mainItem, sizeof(stDirInfo.szRepFile)-1 );
						stDirInfo.bHasSubFile = bHasItem;// ? TRUE : FALSE;
						pFileInfo->size = sizeof(DxDirInfo_t);
						HxSTD_MemCopy(pFileInfo->data,&stDirInfo, sizeof(DxDirInfo_t) );
						HxSTD_MemSet( mainItem, 0x00, sizeof(mainItem) );
						bHasItem = FALSE;
						pItemList = HLIB_LIST_Append(pItemList,pFileInfo);
					}
				}
				umma_fm_browse_notifyResultEvent( pstBrowHnd, eUAPI_FM_BROWSE_SCANITEMS, ERR_OK, k, reqCnt, (HINT32)pItemList);
				pItemList = HLIB_LIST_RemoveAllFunc(pItemList,UMMA_Free_CB);
			}

			reqIdx = pstBrowHnd->nDirCnt;
		}
	}

	umma_fm_browse_getItemsTask( pstBrowHnd, reqIdx, nReqCount, 0 );

	if ( reqIdx + nReqCount < pstBrowHnd->nTotal )
	{
		UMMA_FM_BROWSE_t *pBrowseCtx = umma_fm_browse_getContext();
		UMMA_FM_BROWSE_MSG_t stMsg = { 0 };
		stMsg.eCmd = eBROWSE_CMD_SCANITEM;
		stMsg.pstHandle =  pstBrowHnd;
		stMsg.nParam1 = (reqIdx + nReqCount);
		stMsg.nParam2 = nReqCount;
		stMsg.nParam3 = 0;
		stMsg.nReqId  = 0;

		HLIB_MSG_Send(pBrowseCtx->ulBrowsingMsgId, &stMsg, sizeof(UMMA_FM_BROWSE_MSG_t));
	}
}

static void umma_fm_browse_closeTask( UMMA_FM_BROWSE_HANDLE_t *pstBrowHnd, HINT32 nReqId )
{
	UMMA_FM_BROWSE_t *pBrowseCtx = umma_fm_browse_getContext();

	HxSEMT_Get(s_fm_ulSemId);

	pstBrowHnd->eStatus = eDIR_BROWSE_DIR_CLOSE;
	umma_fm_browse_notifyResultEvent( pstBrowHnd, eUAPI_FM_BROWSE_CLOSE, ERR_OK, 0, 0, 0 );

	pBrowseCtx->pstBrowHndList = HLIB_LIST_Remove( pBrowseCtx->pstBrowHndList, pstBrowHnd  );

	umma_fm_browse_releaseHandle( pstBrowHnd );
	HxSEMT_Release(s_fm_ulSemId);
}

static void umma_fm_browse_sortTask( UMMA_FM_BROWSE_HANDLE_t *pstBrowHnd, HUINT32 eSortType, HINT32 nReqId )
{
	HxLOG_Print("[UMMA_FM_BROWSE] Sort(%d) Task()...... requestId(%d)\n", pstBrowHnd->nHndId, nReqId );
	if (  0 != nReqId && eDIR_BROWSE_DIR_CLOSE == pstBrowHnd->eStatus )
	{
		umma_fm_browse_notifyResultEvent( pstBrowHnd, eUAPI_FM_BROWSE_SORT, ERR_FAIL, 0, 0, 0 );
		return ;
	}

	pstBrowHnd->eSortType = eSortType;

	switch ( eSortType )
	{
		default :
			qsort( pstBrowHnd->ppDirEntry, pstBrowHnd->nTotal, sizeof(struct dirent *), umma_fm_browse_sortByAlpha );
			break;
	}
	if ( 0 != nReqId )
		umma_fm_browse_notifyResultEvent( pstBrowHnd, eUAPI_FM_BROWSE_SORT, ERR_OK, 0, 0, 0 );
}

static void umma_fm_browse_mainTask( void *param )
{
	UMMA_FM_BROWSE_t *pBrowseCtx = (UMMA_FM_BROWSE_t *)param;
	UMMA_FM_BROWSE_MSG_t stMsg = { 0 };

	while ( 1 )
	{
		HxSTD_MemSet( &stMsg, 0x00, sizeof(UMMA_FM_BROWSE_MSG_t));
		if ( VK_OK != HLIB_MSG_Receive(pBrowseCtx->ulBrowsingMsgId, &stMsg, sizeof(UMMA_FM_BROWSE_MSG_t)) )
		{
			VK_TASK_Sleep(100);
			continue;
		}
		switch ( stMsg.eCmd )
		{
			case eBROWSE_CMD_OPEN :
				umma_fm_browse_openTask( stMsg.pstHandle, stMsg.nReqId );
				break;
			case eBROWSE_CMD_CLOSE :
				umma_fm_browse_closeTask( stMsg.pstHandle, stMsg.nReqId );
				break;
			case eBROWSE_CMD_GETITEM :
				if ( umma_fm_browse_findHanle( pBrowseCtx, (HUINT32)stMsg.pstHandle) )
					umma_fm_browse_getItemsTask( stMsg.pstHandle, stMsg.nParam1, stMsg.nParam2, stMsg.nReqId );
				break;
			case eBROWSE_CMD_SCANITEM :
				if ( umma_fm_browse_findHanle( pBrowseCtx, (HUINT32)stMsg.pstHandle) )
					umma_fm_browse_scanItemsTask( stMsg.pstHandle, stMsg.nParam1, stMsg.nParam2, stMsg.nReqId );
				break;
			case eBROWSE_CMD_SORT :
				umma_fm_browse_sortTask( stMsg.pstHandle, stMsg.nParam1, stMsg.nReqId );
				break;
			default :
				break;
		}
	}
}

static UMMA_FM_BROWSE_HANDLE_t * umma_fm_browse_createHandle(void)
{
	UMMA_FM_BROWSE_HANDLE_t *pHandle = NULL;
	UMMA_FM_BROWSE_HANDLE_t *pTmp = NULL;
	pTmp = (UMMA_FM_BROWSE_HANDLE_t *)UMMA_Malloc(sizeof(UMMA_FM_BROWSE_HANDLE_t));
	pHandle = (UMMA_FM_BROWSE_HANDLE_t *)UMMA_Malloc(sizeof(UMMA_FM_BROWSE_HANDLE_t));
	HxSTD_MemSet( pHandle, 0x00, sizeof(UMMA_FM_BROWSE_HANDLE_t) );
	if (pTmp)
		UMMA_Free(pTmp);
	return pHandle;
}

HERROR UMMA_FM_BROWSE_Init(void)
{
	UMMA_FM_BROWSE_t *pBrowseCtx = umma_fm_browse_getContext();
	HUINT32	ulTaskId = 0;
	HINT32 err = 0;

	HxSEMT_Create(&s_fm_ulSemId, "umma_fm_browse_sem", HxSEMT_FIFO);
	HLIB_MSG_Create(128, sizeof(UMMA_FM_BROWSE_MSG_t), "ummafmBrowseEvt", (unsigned long*)&pBrowseCtx->ulBrowsingMsgId, eHLIB_SUSPENDTYPE_FIFO);
	err |= VK_TASK_Create( umma_fm_browse_mainTask, 40, SIZE_64K, "ummaFmBrowseMainTask", (void *)pBrowseCtx, (unsigned long *)&ulTaskId, FALSE );

	err |= VK_TASK_Start(ulTaskId);
	if ( VK_OK != err )
	{
		HxLOG_Error("[%s:%d] BROWSE Init Failed.\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}
	pBrowseCtx->ulBrowsingTaskId = ulTaskId;
	return ERR_OK;
}

HUINT32 UMMA_FM_BROWSE_Open( const HCHAR *pszPath, UMMA_FM_BROWSE_OPEN_t *pstOpt, HINT32 nReqId )
{
	UMMA_FM_BROWSE_t *pBrowseCtx = umma_fm_browse_getContext();
	UMMA_FM_BROWSE_HANDLE_t *pstBrowHnd = NULL;
	UMMA_FM_BROWSE_MSG_t stMsg = { 0 };

	pstBrowHnd = umma_fm_browse_createHandle();
	if ( NULL == pstBrowHnd )
		return 0;

	if ( NULL == pszPath || NULL == pstOpt )
	{
		HxLOG_Error("[UMMA_FM_BROWSE] pszPath or stOpt is NULL!!!!\n");
		umma_fm_browse_releaseHandle(pstBrowHnd);
		return 0;
	}
	HxSEMT_Get(s_fm_ulSemId);
	/* Hanlde Init(+) */
	HLIB_STD_StrNCpySafe(pstBrowHnd->szPath, pszPath, sizeof(pstBrowHnd->szPath));
	HLIB_STD_StrNCpySafe(pstBrowHnd->szFilter, pstOpt->szFilter, sizeof(pstBrowHnd->szFilter));
	pstBrowHnd->nHndId = (HINT32)pstBrowHnd;
	pstBrowHnd->pvUmma = pstOpt->pvUmma;
	pstBrowHnd->eBrowseType = pstOpt->eBrowseType;
	pstBrowHnd->cbResultEvt = pstOpt->cbResult;
	pstBrowHnd->nReqId = nReqId;
	/* Handle Init(-) */
	pBrowseCtx->pstBrowHndList = HLIB_LIST_Append( pBrowseCtx->pstBrowHndList, pstBrowHnd );

	stMsg.eCmd = eBROWSE_CMD_OPEN;
	stMsg.pstHandle =  pstBrowHnd ;
	stMsg.nReqId  = nReqId;
	HLIB_MSG_Send(pBrowseCtx->ulBrowsingMsgId, &stMsg, sizeof(UMMA_FM_BROWSE_MSG_t));

	HxSEMT_Release(s_fm_ulSemId);

	HxLOG_Print("[UMMA_FM_BROWSE] Open [%s], hndId : %d\n", pszPath, pstBrowHnd->nHndId);
	return pstBrowHnd->nHndId;
}

HERROR UMMA_FM_BROWSE_Close( HUINT32 nHandleId )
{
	UMMA_FM_BROWSE_t *pBrowseCtx = umma_fm_browse_getContext();
	UMMA_FM_BROWSE_HANDLE_t *pstBrowHnd = NULL;
	UMMA_FM_BROWSE_MSG_t stMsg = { 0 };

	pstBrowHnd = umma_fm_browse_findHanle( pBrowseCtx, nHandleId );

	if ( !pstBrowHnd )
	{
		return ERR_FAIL;
	}
	HxLOG_Print("BROWSE Close [%s]\n", pstBrowHnd->szPath);

	stMsg.eCmd = eBROWSE_CMD_CLOSE;
	stMsg.pstHandle =  pstBrowHnd;
	stMsg.nReqId  = pstBrowHnd->nReqId;

	HLIB_MSG_SendUrgent(pBrowseCtx->ulBrowsingMsgId, &stMsg, sizeof(UMMA_FM_BROWSE_MSG_t));

	return ERR_OK;
}

HERROR UMMA_FM_BROWSE_GetItems( HUINT32 nHandleId, HUINT32 nIdx, HUINT32 nReqCnt, HINT32 nOption )
{
	UMMA_FM_BROWSE_t *pBrowseCtx = umma_fm_browse_getContext();
	UMMA_FM_BROWSE_MSG_t stMsg = { 0 };
	UMMA_FM_BROWSE_HANDLE_t *pstBrowHnd = NULL;

	pstBrowHnd = umma_fm_browse_findHanle( pBrowseCtx, nHandleId );
	if ( !pstBrowHnd )
	{
		return ERR_FAIL;
	}

	stMsg.eCmd = eBROWSE_CMD_GETITEM;
	stMsg.pstHandle =  pstBrowHnd;
	stMsg.nParam1 = nIdx;
	stMsg.nParam2 = nReqCnt;
	stMsg.nParam3 = nOption;
	stMsg.nReqId  = pstBrowHnd->nReqId;

	HLIB_MSG_Send(pBrowseCtx->ulBrowsingMsgId, &stMsg, sizeof(UMMA_FM_BROWSE_MSG_t));
	return ERR_OK;
}

HERROR UMMA_FM_BROWSE_ScanItems( HUINT32 nHandleId, HUINT32 nReqCnt, HINT32 nOption )
{
	UMMA_FM_BROWSE_t *pBrowseCtx = umma_fm_browse_getContext();
	UMMA_FM_BROWSE_MSG_t stMsg = { 0 };
	UMMA_FM_BROWSE_HANDLE_t *pstBrowHnd = NULL;

	pstBrowHnd = umma_fm_browse_findHanle( pBrowseCtx, nHandleId );
	if ( !pstBrowHnd )
	{
		return ERR_FAIL;
	}

	stMsg.eCmd = eBROWSE_CMD_SCANITEM;
	stMsg.pstHandle =  pstBrowHnd;
	stMsg.nParam1 = 0;
	stMsg.nParam2 = nReqCnt;
	stMsg.nParam3 = nOption;
	stMsg.nReqId  = pstBrowHnd->nReqId;

	HLIB_MSG_Send(pBrowseCtx->ulBrowsingMsgId, &stMsg, sizeof(UMMA_FM_BROWSE_MSG_t));
	return ERR_OK;
}

HERROR UMMA_FM_BROWSE_Sort( HUINT32 nHandleId, HUINT32 eSortType )
{
	UMMA_FM_BROWSE_t *pBrowseCtx = umma_fm_browse_getContext();
	UMMA_FM_BROWSE_MSG_t stMsg = { 0 };
	UMMA_FM_BROWSE_HANDLE_t *pstBrowHnd = NULL;

	pstBrowHnd = umma_fm_browse_findHanle( pBrowseCtx, nHandleId );
	if ( !pstBrowHnd )
	{
		return ERR_FAIL;
	}
	stMsg.eCmd = eBROWSE_CMD_SORT;
	stMsg.pstHandle =  pstBrowHnd;
	stMsg.nParam1 = eSortType;
	stMsg.nReqId  = pstBrowHnd->nReqId;

	HLIB_MSG_Send(pBrowseCtx->ulBrowsingMsgId, &stMsg, sizeof(UMMA_FM_BROWSE_MSG_t));
	return ERR_OK;
}


