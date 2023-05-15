/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  umma_group.c
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
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/vfs.h>
#include	<dirent.h>

#include	<hlib.h>

#include	<uapi.h>
#include	<umma_int.h>
#include	<umma_fm.h>
/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#if defined(CONFIG_PROD_OS_EMULATOR)
#define stat emu_stat
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HxList_t *pstTokenHndList;
} UMMA_FM_GROUP_INFO_t;

typedef struct
{
	HUINT32 nHndId;
	HINT32 nReqId;
	HINT32 nItemCnt;
	HCHAR szExtFilter[256];
	HxList_t *pstFileList;
	HBOOL	bCheckSize;
	HUINT64 ullTotalSize;
	UAPI_FM_GROUP_OPER_e eOper;
} UMMA_FM_GROUP_HANDLE_t;
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ***************************/
/*******************************************************************/
static HBOOL umma_fm_group_isWildcardPath( const HCHAR *pszPath, HCHAR *pszGetDirPath, HINT32 nBufSize )
{
	HINT32 nLen = 0;
	nLen = HxSTD_StrLen( pszPath );
	if ( 1 < nLen && '*' == pszPath[nLen -1] )
	{
		HxSTD_MemSet(pszGetDirPath, 0x00, nBufSize);
		HxSTD_MemCopy(pszGetDirPath, pszPath, nLen -2 );
		HLIB_PATH_Normalize(pszGetDirPath);

		if (HLIB_DIR_IsDirPath(pszGetDirPath))
			return TRUE;
		return TRUE;
	}
	return FALSE;
}

static UMMA_FM_GROUP_INFO_t*	umma_fm_get_groupInfo(void)
{
	static UMMA_FM_GROUP_INFO_t		*s_pstCopyInfo = NULL;

	if (s_pstCopyInfo == NULL)
	{
		s_pstCopyInfo = (UMMA_FM_GROUP_INFO_t *)UMMA_Calloc(sizeof(UMMA_FM_GROUP_INFO_t));
		HxSTD_MemSet( s_pstCopyInfo, 0x00, sizeof(UMMA_FM_GROUP_INFO_t) );
	}
	return s_pstCopyInfo;
}

static HUINT64 umma_fm_group_getPathSize( const HCHAR *pszPath, const HCHAR *pszExtFilter )
{
#if defined(CONFIG_PROD_OS_EMULATOR)
	//TODO;
	return 0;
#else
	HUINT64	ullFileSize = 0;
	struct stat	fstat;
	HCHAR *pszFullPath = NULL;
	HCHAR szTmpBuf[512] = { 0 };

	if ( 0 != stat(pszPath, &fstat))
		return 0;

	if ( S_ISDIR(fstat.st_mode) ) // folder
	{
		HINT32	nItemCount = 0;
		struct dirent	**ppstItemList = NULL;

		HxLOG_Print( "[UMMA_FM_GROUP] check size... dir path : %s\n",  pszPath );  
		nItemCount = scandir((const char*)pszPath, &ppstItemList, NULL, NULL);
		if ( nItemCount < 0 )
			return 0;

		while (nItemCount--)
		{
			/* Check "." or  ".." explicitly */
			if (!HxSTD_StrCmp(ppstItemList[nItemCount]->d_name, ".") ||  
					!HxSTD_StrCmp(ppstItemList[nItemCount]->d_name, ".."))
			{
				free(ppstItemList[nItemCount]); ppstItemList[nItemCount]=NULL;
				continue;
			}

			pszFullPath = HLIB_PATH_Join(pszPath, ppstItemList[nItemCount]->d_name, szTmpBuf, sizeof(szTmpBuf));
			ullFileSize += umma_fm_group_getPathSize(pszFullPath, pszExtFilter);
			free(ppstItemList[nItemCount]); ppstItemList[nItemCount]=NULL;
		}

		if (ppstItemList != NULL)
		{
			free(ppstItemList);
			ppstItemList = NULL;
		}
	}
	else if ( S_ISREG(fstat.st_mode) ) // file
	{
		HCHAR	*ext = NULL;
		/* Filtering by Extensions **/
		if ( HxSTD_StrEmpty(pszExtFilter) || !HxSTD_StrCmp("*", pszExtFilter ) )
		{
			ullFileSize = fstat.st_size; 
		}
		else 
		{
			ext = HLIB_PATH_Extension( pszPath );
			if ( !HxSTD_StrEmpty(ext) && HLIB_STD_StrCaseStr( pszExtFilter, ext) )
				ullFileSize = fstat.st_size; 
			else
			{
				HxLOG_Print( "[UMMA_FM_GROUP] check size... exclude file : %s\n", pszPath );  
				return 0;
			}
		}
		HxLOG_Print( "[UMMA_FM_GROUP] size : %lld byte,  path : %s\n", ullFileSize, pszPath );  
	}

	return ullFileSize;
#endif /*CONFIG_PROD_OS_EMULATOR*/	
}

static HINT32 umma_fm_group_getTotalSize( UMMA_FM_GROUP_HANDLE_t *pTokenHnd, HUINT64 *pullSize )
{
	HUINT64	ullFileSize = 0;
	HxList_t *pItem = NULL;
	HCHAR *pPath = NULL;
	HCHAR szTmpPath[512] = { 0 };

	pItem = pTokenHnd->pstFileList;
	while ( pItem )
	{
		pPath = (HCHAR *)HLIB_LIST_Data(pItem);
		//if ( 1 < nLen && '*' == pPath[nLen -1] )
		if ( TRUE == umma_fm_group_isWildcardPath(pPath, szTmpPath, sizeof(szTmpPath)) )
		{
			pPath = szTmpPath;
			HxLOG_Print( "[UMMA_FM_GROUP] getTotal Size. wildcard char '*' : %s\n", pPath );  
		} 
		ullFileSize += umma_fm_group_getPathSize( pPath, pTokenHnd->szExtFilter );
		pItem = HLIB_LIST_Next( pItem );
	}

	*pullSize = ullFileSize;
	pTokenHnd->ullTotalSize = ullFileSize;
	pTokenHnd->bCheckSize = TRUE;
	return UMERR_OK;
}

static HUINT32	umma_fm_group_checkSpace(UMMA_FM_GROUP_HANDLE_t *pTokenHnd, const HCHAR *pszDestName)
{
#define	MARGIN_FREE_SIZE	10240LL		//	10MB
#define	SIZE_4G				(4 * 1024 * 1024)	//	4G
#define	LIMIT_4G_FS			"fat32,fat16,vfat"
	HCHAR	pszMountPath[256], szfsType[16];
	HUINT64	ullFreeSize = 0, ullFileSize = 0;

	if ( ERR_OK != HLIB_DIR_GetMountPath(pszDestName, pszMountPath, sizeof(pszMountPath)))
	{
		return UMERR_REASON_INVALID_ARG;
	}

	if ( TRUE != pTokenHnd->bCheckSize )
	{
		umma_fm_group_getTotalSize( pTokenHnd, &pTokenHnd->ullTotalSize );
	}
	
	HxLOG_Debug("pszDestName [%s], pszMountPath [%s]\n", pszDestName, pszMountPath);
	if ( FALSE ) // Network Storage
	{
		return UMERR_OK;
	}
	if ( ERR_OK != HLIB_DIR_GetMountFstype(pszMountPath, szfsType, 16) )
	{
		return UMERR_REASON_INVALID_ARG;
	}
	if ( ERR_OK != UMMA_FM_TOOLKIT_GetDeviceSizeKB(pszMountPath, NULL, NULL, &ullFreeSize) )
	{
		return UMERR_REASON_INVALID_ARG;
	}

	if ( pTokenHnd->ullTotalSize )
		ullFileSize = (pTokenHnd->ullTotalSize / 1024);

	HxLOG_Debug("ullFreeSize : [%lldKB], ullFileSize : [%lldKB]\n", ullFreeSize, ullFileSize);

	if ( ullFileSize > SIZE_4G && HLIB_STD_StrCaseStr(LIMIT_4G_FS, szfsType) )
	{
		return UMERR_REASON_SIZE_LIMIT_4G;
	}

	ullFileSize += MARGIN_FREE_SIZE;
	if (ullFileSize > ullFreeSize)
	{
		HxLOG_Error("Not enough space(%lldKB), total file Size(%lldKB)\n", ullFreeSize, ullFileSize);
		return UMERR_REASON_NOT_ENOUGH_SPACE;
	}

	return UMERR_OK;
}

static UMMA_FM_GROUP_HANDLE_t * umma_fm_group_findHandle( HUINT32 nHandleId )
{
	UMMA_FM_GROUP_INFO_t	*pstGpInfo = umma_fm_get_groupInfo();
	HxList_t *pItem = NULL;
	UMMA_FM_GROUP_HANDLE_t *pTokenHnd = NULL;

	pItem = pstGpInfo->pstTokenHndList;
	while ( pItem )
	{
		pTokenHnd = (UMMA_FM_GROUP_HANDLE_t*)HLIB_LIST_Data(pItem);
		if( pTokenHnd->nHndId == nHandleId )
		{
			break;
		}

		pItem = HLIB_LIST_Next( pItem );
	}

	if ( !pItem )
	{
		HxLOG_Error("Not find HandleId(%ld)\n", nHandleId);
		return NULL;
	}
	return pTokenHnd;
}

static HCHAR * umma_fm_group_findPath( UMMA_FM_GROUP_HANDLE_t *pTokenHnd, HCHAR *pszPath )
{
	HxList_t *pItem = NULL;
	HCHAR *pPath = NULL;

	pItem = pTokenHnd->pstFileList;
	while ( pItem )
	{
		pPath = (HCHAR *)HLIB_LIST_Data(pItem);
		if ( !HxSTD_StrCmp( pPath, pszPath ))
		{
			return pPath;
		}

		pItem = HLIB_LIST_Next( pItem );
	}
	return NULL;
}

HUINT32 UMMA_FM_GROUP_CreateHandle( const HCHAR *pszExtFilter )
{
	UMMA_FM_GROUP_INFO_t	*pstGpInfo = umma_fm_get_groupInfo();
	UMMA_FM_GROUP_HANDLE_t *pTokenHnd = NULL;
	pTokenHnd = (UMMA_FM_GROUP_HANDLE_t *)UMMA_Calloc(sizeof(UMMA_FM_GROUP_HANDLE_t));
	if ( !pTokenHnd )
		return 0;
	HxSTD_MemSet( pTokenHnd, 0x00, sizeof(UMMA_FM_GROUP_HANDLE_t) );
	pTokenHnd->nHndId = (HUINT32)pTokenHnd;
	HxSTD_StrNCpy( pTokenHnd->szExtFilter, pszExtFilter, sizeof(pTokenHnd->szExtFilter)-1);
	pstGpInfo->pstTokenHndList = HLIB_LIST_Append( pstGpInfo->pstTokenHndList, pTokenHnd );
	pTokenHnd->eOper = eUAPI_FM_GROUP_OPER_UNKNOWN;

	HxLOG_Print("[UMMA] Create nHandleId: %ld, extFilter : %s\n", (HUINT32)pTokenHnd, pszExtFilter );
	return (HUINT32)pTokenHnd;
}

HINT32 UMMA_FM_GROUP_ReleaseHandle( HUINT32 nHandleId )
{
	UMMA_FM_GROUP_INFO_t	*pstGpInfo = umma_fm_get_groupInfo();
	UMMA_FM_GROUP_HANDLE_t *pTokenHnd = NULL;

	HxLOG_Print("[UMMA] Relese nHandleId: %ld\n", nHandleId );
	pTokenHnd = umma_fm_group_findHandle( nHandleId );
	if ( !pTokenHnd )
		return UMERR_ERR;

	pstGpInfo->pstTokenHndList = HLIB_LIST_Remove( pstGpInfo->pstTokenHndList, pTokenHnd );
	return UMERR_OK;
}

HINT32 UMMA_FM_GROUP_AddPath( HUINT32 nHandleId, HCHAR *pszPath )
{
	UMMA_FM_GROUP_HANDLE_t *pTokenHnd = NULL;
	struct stat	fstat;
	HCHAR *pPath = NULL;
	HINT32 pathLen = 0;
	HCHAR szTmpPath[512] = { 0 };

	pTokenHnd = umma_fm_group_findHandle( nHandleId );
	if ( !pTokenHnd )
		return UMERR_ERR;

	if ( HxSTD_StrEmpty(pszPath) )
		return UMERR_ERR;

	HxLOG_Print("[UMMA_FM_GROUP] nHandleId(%ld), try add path : %s\n", nHandleId, pszPath ); 
	if ( 0 == stat(pszPath, &fstat))
	{
		if ( S_ISREG(fstat.st_mode) ) // file
		{
			HCHAR	*ext = NULL;
			ext = HLIB_PATH_Extension( pszPath );
			if ( !HxSTD_StrEmpty(ext) && !HLIB_STD_StrCaseStr( pTokenHnd->szExtFilter, ext) )
			{
				HxLOG_Print("[UMMA_FM_GROUP] nHandleId(%ld), Exclude file : %s, Ext Filter : %s\n", 
						nHandleId, pszPath, pTokenHnd->szExtFilter ); 
				return UMERR_ERR;
			}
		}
	}
	else if ( TRUE == umma_fm_group_isWildcardPath( pszPath, szTmpPath, sizeof(szTmpPath)) )
	{
		HxLOG_Print( "[UMMA_FM_GROUP] AddPath. wildcard char '*' : %s\n", szTmpPath );  
	}
	else
	{
		HxLOG_Error( "[UMMA_FM_GROUP] Error!!!  Add Path : %s\n", szTmpPath );  
		return UMERR_ERR;
	}

	pathLen = HxSTD_StrLen( pszPath );
	pPath = (HCHAR *)UMMA_Calloc(pathLen + 1); 
	HxSTD_StrNCpy( pPath, pszPath, pathLen );
	pTokenHnd->pstFileList = HLIB_LIST_Append( pTokenHnd->pstFileList, pPath );
	pTokenHnd->bCheckSize = FALSE;

	++pTokenHnd->nItemCnt;
	return UMERR_OK;
}

HINT32 UMMA_FM_GROUP_RemovePath( HUINT32 nHandleId, HCHAR *pszPath )
{
	UMMA_FM_GROUP_HANDLE_t *pTokenHnd = NULL;
	HCHAR *pPath= NULL;
	pTokenHnd = umma_fm_group_findHandle( nHandleId );
	if ( !pTokenHnd )
		return UMERR_ERR;

	pPath = umma_fm_group_findPath( pTokenHnd, pszPath );
	if ( !pPath )
		return UMERR_ERR;

	pTokenHnd->pstFileList = HLIB_LIST_Remove( pTokenHnd->pstFileList, pPath );
	UMMA_Free( pPath );
	
	--pTokenHnd->nItemCnt;
	return UMERR_OK;
}

HxList_t * UMMA_FM_GROUP_GetList( HUINT32 nHandleId )
{
	UMMA_FM_GROUP_HANDLE_t *pTokenHnd = NULL;
	pTokenHnd = umma_fm_group_findHandle( nHandleId );
	if ( !pTokenHnd )
		return NULL;

	return pTokenHnd->pstFileList;
}

HINT32 UMMA_FM_GROUP_GetItemCount( HUINT32 nHandleId, HINT32 *pnCount )
{
	UMMA_FM_GROUP_HANDLE_t *pTokenHnd = NULL;
	pTokenHnd = umma_fm_group_findHandle( nHandleId );

	if ( !pTokenHnd || !pnCount )
		return UMERR_ERR;

	*pnCount = pTokenHnd->nItemCnt;
	return UMERR_OK;
}

HINT32 UMMA_FM_GROUP_GetTotalSize( HUINT32 nHandleId, HUINT64 *pullSize )
{
	UMMA_FM_GROUP_HANDLE_t *pTokenHnd = NULL;
	pTokenHnd = umma_fm_group_findHandle( nHandleId );

	if ( !pullSize )
		return UMERR_ERR;

	if ( !pTokenHnd )
		return UMERR_ERR;

	return umma_fm_group_getTotalSize( pTokenHnd, pullSize );
}

HUINT32 UMMA_FM_GROUP_CheckFreeSpace( HUINT32 nHandleId, const HCHAR *pszDestPath )
{
	UMMA_FM_GROUP_HANDLE_t *pTokenHnd = NULL;
	pTokenHnd = umma_fm_group_findHandle( nHandleId );

	if ( HxSTD_StrEmpty(pszDestPath) )
		return UMERR_ERR;

	return umma_fm_group_checkSpace( pTokenHnd, pszDestPath ) ;
}

HINT32 UMMA_FM_GROUP_SetOperation( HUINT32 nHandleId, HINT32 eOper, HINT32 nReqId )
{
	UMMA_FM_GROUP_HANDLE_t *pTokenHnd = NULL;
	pTokenHnd = umma_fm_group_findHandle( nHandleId );
	if ( !pTokenHnd )
		return UMERR_ERR;

	HxLOG_Print( "[UMMA_FM_GROUP] nHandleId(%d), Set Operation[%d]\n", nHandleId, eOper );
	
	if ( eUAPI_FM_GROUP_OPER_CANCEL == eOper )
		pTokenHnd->eOper = eUAPI_FM_GROUP_OPER_UNKNOWN;	
	else
		pTokenHnd->eOper = (UAPI_FM_GROUP_OPER_e)eOper;	
	pTokenHnd->nReqId = nReqId;	
	return UMERR_OK;
}

HINT32 UMMA_FM_GROUP_GetOperation( HUINT32 nHandleId, HINT32 *peOper, HINT32 *pnReqId )
{
	UMMA_FM_GROUP_HANDLE_t *pTokenHnd = NULL;
	pTokenHnd = umma_fm_group_findHandle( nHandleId );
	if ( !pTokenHnd )
		return UMERR_ERR;

	*peOper = (HINT32)pTokenHnd->eOper;	
	*pnReqId = pTokenHnd->nReqId;	
	return UMERR_OK;
}

HBOOL UMMA_FM_GROUP_IsWildcardPath( const HCHAR *pszPath, HCHAR *pszGetDirPath, HINT32 nBufSize )
{
	return umma_fm_group_isWildcardPath( pszPath, pszGetDirPath, nBufSize );
}

const HCHAR * UMMA_FM_GROUP_GetExtFilter( HUINT32 nHandleId )
{
	UMMA_FM_GROUP_HANDLE_t *pTokenHnd = NULL;
	pTokenHnd = umma_fm_group_findHandle( nHandleId );
	if ( pTokenHnd )
		return pTokenHnd->szExtFilter;

	return NULL;
}

