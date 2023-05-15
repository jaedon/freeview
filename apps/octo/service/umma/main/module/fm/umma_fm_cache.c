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
#include	<hlib.h>
#include	<dlib.h>

#include	<umma_int.h>
#include	<umma_fm.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
//	Policy of umma cache.
//	umma cache will be saved in target device.
#define	UMMA_FM_CACHE_STORAGE	"/mnt/hd2"

#define	UMMA_FM_CACHE_OLD_DIR		".scan"
#define	UMMA_FM_CACHE_DIR			".umma"
#define	UMMA_FM_CACHE_VERSION		"0.9.5"
#define	UMMA_FM_CACHE_VERSION_SIZE	(HxSTD_StrLen(UMMA_FM_CACHE_VERSION) + 1)
#define	UMMA_FM_CACHE_VERFILE		"version"
#define	UMMA_FM_CACHE_MAX_NO		2048		//	1024 files per directory

#if defined(CONFIG_PROD_OS_EMULATOR)
#define stat emu_stat
#endif /* CONFIG_PROD_OS_EMULATOR */
/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HxCBUF_t	nCBuf;

	HBOOL	bChanged;
	HCHAR	szUUID[512];
	HCHAR	szDirectory[1024];
	HCHAR	szCacheName[1024];
	HCHAR	szTempName[512];
	HxHASH_t	*pstDirHash;
} UMMA_FM_CACHE_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ***************************/
/*******************************************************************/
static UMMA_FM_CACHE_t*	umma_fm_cache_getInstance(void)
{
	static UMMA_FM_CACHE_t	*s_pstUmmaCache = NULL;

	if (s_pstUmmaCache == NULL)
	{
		s_pstUmmaCache = (UMMA_FM_CACHE_t*)UMMA_Calloc(sizeof(UMMA_FM_CACHE_t));

		s_pstUmmaCache->nCBuf = HLIB_CBUF_New(8192);
		s_pstUmmaCache->pstDirHash = HLIB_HASH_NewFull(HLIB_HASH_GetStringKey, HLIB_HASH_IsStringEqual, NULL, UMMA_Free_CB);
	}

	return s_pstUmmaCache;
}

static void				umma_fm_cache_releaseInstance(UMMA_FM_CACHE_t *pstCache)
{
	//	TODO: nothing to do.
}

void	UMMA_FM_CACHE_CheckVersion(const HCHAR *pszMountPath)
{
	HCHAR			*pszDirName, *pszVersionNo, *pszFileName;
	HUINT32			ulSize;
	UMMA_FM_CACHE_t	*pstCache;

	pstCache = umma_fm_cache_getInstance();

#if 0
	//	if mountpath is umma cache folder?
	if (HLIB_STD_StrStartWith(pszMountPath, UMMA_FM_CACHE_STORAGE) == TRUE)
	{
#endif
		//	if old cache is existed, remove it.
		pszDirName = HLIB_CBUF_GetBuffer(pstCache->nCBuf, 1024);
		HLIB_PATH_Join(pszMountPath, UMMA_FM_CACHE_OLD_DIR, pszDirName, 1024);
		if (HLIB_DIR_IsDirPath(pszDirName) == TRUE)
			HLIB_DIR_Delete(pszDirName);

		//	if new cache is existed, check version more.
		HLIB_PATH_Join(pszMountPath, UMMA_FM_CACHE_DIR, pszDirName, 1024);
		if (HLIB_DIR_IsDirPath(pszDirName) == TRUE)
		{
			//	check umma version.
			pszFileName = HLIB_CBUF_GetBuffer(pstCache->nCBuf, 1024);
			HLIB_PATH_Join(pszDirName, UMMA_FM_CACHE_VERFILE, pszFileName, 1024);
			if (HLIB_FILE_ReadFile(pszFileName, (void**)&pszVersionNo, &ulSize) == ERR_OK)
			{
				//	if version is not same!!!
				if (HLIB_STD_StrNCaseCmp(pszVersionNo, UMMA_FM_CACHE_VERSION, UMMA_FM_CACHE_VERSION_SIZE))
					HLIB_DIR_Delete(pszDirName);
				HLIB_FILE_FreeBuffer(pszVersionNo);
			} else
			{
				pszVersionNo = HLIB_CBUF_GetBuffer(pstCache->nCBuf, 512);
				HxSTD_StrNCpy(pszVersionNo, UMMA_FM_CACHE_VERSION, UMMA_FM_CACHE_VERSION_SIZE);
				HLIB_FILE_WriteFile(pszFileName, pszVersionNo, UMMA_FM_CACHE_VERSION_SIZE);
			}
		}
#if 0
	}
#endif
	umma_fm_cache_releaseInstance(pstCache);
}

static HCHAR*	umma_fm_cache_GetDirName(const HCHAR *pszSourceName, HCHAR *pszDirName, HINT32 n)
{
	HCHAR	*ret;

	if (!(pszDirName && n))
		return NULL;

	HLIB_STD_StrNCpySafe(pszDirName, pszSourceName, n);

	ret = strrchr(pszDirName, '/');
	if (ret)
	{
		*ret = '\0';
	}
	return pszDirName;
}

#define	UMMA_FM_CACHE_MAGICCODE		0x0F0FF0F0
/*
	UMMA_FM_CACHE_STORAGE/UUID/dir_inode.dat

	//	4 bytes : number of items
	//	loop for number of items
	//	4 bytes : file inode
	//	4 bytes : file size
	//	n bytes : DxFileInfo_t
	//	4 bytes : UMMA_FM_CACHE_MAGICCODE
 */
static void		umma_fm_cache_SaveCacheToFile(UMMA_FM_CACHE_t *pstCache, const HCHAR *pszCacheName, HxHASH_t *pstDirHash)
{
	HxFILE_t	hFile;
	HUINT32		ulNumOfItems, ulFileSize;
	HUINT32		ulMagicCode = UMMA_FM_CACHE_MAGICCODE;
	HxHASH_TableIter_t	iter;
	DxFileInfo_t		*pstFileInfo;

	if (!pstCache->bChanged)
	{
		HxLOG_Debug("cache is same with previous one!\n");
		return;
	}

	ulNumOfItems = HLIB_HASH_Size(pstDirHash);
	if (ulNumOfItems > 0)
	{
		HCHAR	*pszDirName;

		pszDirName = 	HLIB_CBUF_GetBuffer(pstCache->nCBuf, 1024);
		umma_fm_cache_GetDirName(pszCacheName, pszDirName, 1024);
		if (HLIB_DIR_IsDirPath(pszDirName) == FALSE)
			HLIB_DIR_Make(pszDirName);

		hFile = HLIB_FILE_Open(pszCacheName, "wb");
		if (hFile == NULL)
		{
			HxLOG_Error("error to write cache (%s)\n", pszCacheName);
			return;
		}

		HLIB_FILE_Write(hFile, &ulNumOfItems, sizeof(HUINT32), 1);

		HLIB_HASH_IterInit(&iter, pstDirHash);
		while (HLIB_HASH_IterNext(&iter, (void**)NULL, (void**)&pstFileInfo))
		{
			ulFileSize = sizeof(DxFileInfo_t) + pstFileInfo->size;
			HLIB_FILE_Write(hFile, &ulFileSize, sizeof(HUINT32), 1);
			HLIB_FILE_Write(hFile, pstFileInfo, ulFileSize, 1);
		}

		HLIB_FILE_Write(hFile, &ulMagicCode, sizeof(HUINT32), 1);
		HLIB_FILE_Close(hFile);
	}
}

static void		umma_fm_cache_LoadCacheFromFile(const HCHAR *pszCacheName, HxHASH_t *pstDirHash)
{
	HxFILE_t	hFile;
	HUINT32		ulNumOfItems = 0, ulMagicCode = 0, ulDataSize = 0, i =0;
	DxFileInfo_t	*pstFileInfo = NULL;
	HERROR		 hErr = ERR_OK;
	HCHAR       *pszPath;

	hFile = HLIB_FILE_Open(pszCacheName, "rb");
	if (hFile == NULL)
	{
		HxLOG_Debug("fail to load cache file [%s]\n", pszCacheName);
		return;
	}

	//	check magic code : file is finished or not.
	hErr = HLIB_FILE_Seek(hFile, -4, eHxFILE_SEEK_END, NULL);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("error occurred in HLIB_FILE_Seek(%d).\n", hErr);
		return;
	}

	HLIB_FILE_Read(hFile, &ulMagicCode, sizeof(HUINT32), 1);
	if (ulMagicCode != UMMA_FM_CACHE_MAGICCODE)
	{
		HxLOG_Error("Cache file is corrupted! [%s]\n", pszCacheName);
		HLIB_FILE_Close(hFile);
		HLIB_FILE_Delete(pszCacheName);
		return;
	}
	hErr = HLIB_FILE_Seek(hFile,  0, eHxFILE_SEEK_SET, NULL);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("error occurred in HLIB_FILE_Seek(%d).\n", hErr);
		return;
	}

	HLIB_FILE_Read(hFile, &ulNumOfItems, sizeof(HUINT32), 1);
	for (i = 0; i < ulNumOfItems; i++)
	{
		HLIB_FILE_Read(hFile, &ulDataSize, sizeof(HUINT32), 1);
		pstFileInfo = (DxFileInfo_t*)UMMA_Malloc(ulDataSize);
		if ( NULL == pstFileInfo )
		{
			HxLOG_Error("pstFileInfo is NULL. ulDataSize(%d), ulNumOfItems(%d), i(%d) \n", ulDataSize, ulNumOfItems, i);
			break;
		}
		HLIB_FILE_Read(hFile, pstFileInfo, ulDataSize, 1);
		pstFileInfo->szOnedepth[DLIB_FILEINFO_ONEDEPTH_LEN - 1] = '\0';
		pstFileInfo->szFileName[DLIB_FILEINFO_NAME_LEN     - 1] = '\0';
		pszPath = HLIB_PATH_FileName(pstFileInfo->szFileName); // cache load 중 crash.. 방어코드 삽입
		if ( (HxSTD_StrEmpty(pstFileInfo->szFileName) == FALSE) && (HxSTD_StrEmpty(pszPath) == FALSE) )
		{
			HLIB_HASH_Insert(pstDirHash, (void*)pszPath, (void*)pstFileInfo);
			HxLOG_Debug("Cache [%s] - [%s]\n", pszCacheName, pstFileInfo->szFileName);
		}
		else
		{
			UMMA_Free(pstFileInfo);
		}
	}

	HLIB_FILE_Close(hFile);
}

static HxHASH_t*	umma_fm_cache_GetDirHashTable(UMMA_FM_CACHE_t *pstCache, const UMMA_MOUNTINFO_t *pstMountInfo, const HCHAR *pszPathName)
{
	HCHAR	*pszCacheDirCvtName, *p;
	struct stat dstat;

	//HxLOG_Debug("[%s - %s]\n", pstMountInfo->szUUID, pszPathName);
	if ((HxSTD_StrCmp(pstCache->szDirectory, pszPathName) == 0) && (HxSTD_StrCmp(pstCache->szUUID, pstMountInfo->szUUID)== 0))
		return pstCache->pstDirHash;

	if (pstCache->pstDirHash)
	{
		//	Save current hash data.
		if (HLIB_HASH_Size(pstCache->pstDirHash) > UMMA_FM_CACHE_MAX_NO)
		{
			//	Need optimize!
			HxLOG_Warning("Cache data is too big!!! [max : %d]\n", HLIB_HASH_Size(pstCache->pstDirHash));
			//	TODO: Optimize!
			//	TODO: Optimize!
			HxLOG_Warning("Simply remove all data in cache !! --> if needs, please add LRU scheme!\n");
			HLIB_HASH_RemoveAll(pstCache->pstDirHash);
		}
		umma_fm_cache_SaveCacheToFile(pstCache, pstCache->szCacheName, pstCache->pstDirHash);
		HLIB_HASH_RemoveAll(pstCache->pstDirHash);
	}

	if (stat(pszPathName, &dstat))
	{
		pstCache->szUUID[0] = '\0';
		pstCache->szDirectory[0] = '\0';

		return NULL;
	}
	//	load hash table.
	HLIB_STD_StrNCpySafe(pstCache->szUUID,      pstMountInfo->szUUID, 512);
	HLIB_STD_StrNCpySafe(pstCache->szDirectory, pszPathName, 1024);

	pszCacheDirCvtName = (HCHAR*)pszPathName;
	if (HxSTD_StrLen(pszPathName) > 512)
		pszCacheDirCvtName = pszCacheDirCvtName + 512 - HxSTD_StrLen(pszCacheDirCvtName);

	HLIB_STD_StrNCpySafe(pstCache->szTempName, pszCacheDirCvtName, 512);
	p = pstCache->szTempName;
	while (*p)
	{
		if (*p == '/') *p = '_';
		p++;
	}

//	HxSTD_snprintf(pstCache->szCacheName, 1024, "%s/%s/%s/%d.dat", UMMA_FM_CACHE_STORAGE, UMMA_FM_CACHE_DIR, pszUUID, ulDirINode);
	HxSTD_snprintf(pstCache->szCacheName, 1023, "%s/%s/%s.dat", pstMountInfo->szMountPath, UMMA_FM_CACHE_DIR, pszCacheDirCvtName);
	pstCache->szCacheName[1023] = '\0';
	HxLOG_Debug("CacheName : [%s]\n", pstCache->szCacheName);
	umma_fm_cache_LoadCacheFromFile(pstCache->szCacheName, pstCache->pstDirHash);
	pstCache->bChanged = FALSE;

	return pstCache->pstDirHash;
}

//	return DxFileInfo_t
void*	UMMA_FM_CACHE_GetFileInfo(UMMA_MOUNTINFO_t *pstMountInfo, const HCHAR *pszSourceName, struct stat *fstat)
{
	UMMA_FM_CACHE_t	*pstCache;
	HCHAR	*pszPathName;
	DxFileInfo_t	*pstFileInfo;
	HxHASH_t	*pstDirHash;

	pstCache = umma_fm_cache_getInstance();

	pszPathName = HLIB_CBUF_GetBuffer(pstCache->nCBuf, 1024);
	umma_fm_cache_GetDirName(pszSourceName, pszPathName, 1024);

	pstDirHash = umma_fm_cache_GetDirHashTable(pstCache, pstMountInfo, pszPathName);
	if (pstDirHash)
	{
		pstFileInfo = (DxFileInfo_t*)HLIB_HASH_Lookup(pstDirHash, (void*)HLIB_PATH_FileName(pszSourceName));
		if (pstFileInfo)
		{
			HxLOG_Debug("hit : [%s]\n", HLIB_PATH_FileName(pszSourceName));
			//	sometimes, inode value is changed --> Cache is based on filename. so, just fix inode value in return.

			pstFileInfo->uid = UMMA_FM_IFACE_GetNewUid(fstat);
			umma_fm_cache_releaseInstance(pstCache);
			return (void*)pstFileInfo;
		}
	}

	umma_fm_cache_releaseInstance(pstCache);

	return NULL;
}

void	UMMA_FM_CACHE_AddFileInfo(void *pvFileInfo, HINT32 nSize)
{
	UMMA_FM_CACHE_t	*pstCache;
	HxHASH_t	*pstDirHash;
	DxFileInfo_t	*pstOldInfo, *pstNewInfo;

	pstNewInfo = (DxFileInfo_t*)pvFileInfo;

	pstCache = umma_fm_cache_getInstance();

	pstDirHash = pstCache->pstDirHash;
	if (pstDirHash)
	{
		pstOldInfo = (DxFileInfo_t*)HLIB_HASH_Lookup(pstDirHash, (void*)pstNewInfo->szFileName);
		if (pstOldInfo)
		{
			if (pstOldInfo->st_stat.ullMTime == pstNewInfo->st_stat.ullMTime)
			{
				umma_fm_cache_releaseInstance(pstCache);

				return;
			} else
				HLIB_HASH_Remove(pstDirHash, (void*)pstNewInfo->szFileName);
		}

		pstNewInfo = (DxFileInfo_t*)UMMA_MemDup(pstNewInfo, nSize);
		HxLOG_Debug("Add [%s]\n", pstNewInfo->szFileName);
		HLIB_HASH_Insert(pstDirHash, (void*)pstNewInfo->szFileName, pstNewInfo);
		pstCache->bChanged = TRUE;
	}

	umma_fm_cache_releaseInstance(pstCache);
}

