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
 * @file	  		nx_core_filemgr_file.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#include <nx_core_filemgr_file.h>
#include <nx_core_message.h>

/**************************************************************************************************/
#define ___________________Private_variable________________________________________________
/**************************************************************************************************/
static NX_FileContext_t		*g_FileContext = NULL;

/**************************************************************************************************/
#define ___________________Private_typedef________________________________________________
/**************************************************************************************************/
#define DOT_OR_DOTDOT(_basename)					\
	((_basename[0] == '.') &&					\
	 (_basename[1] == '\0' || (_basename[1] == '.' && _basename[2] == '\0')))

/**************************************************************************************************/
#define ___________________Private_prototype________________________________________________
/**************************************************************************************************/
static NX_FileContext_t *nx_filemgr_file_GetContext(void);
static HCHAR	*nx_filemgr_file_GetThumbnail(HCHAR *szFileUrl, HUINT32 ulExtractPosition, const HCHAR *pszMountPath, HBOOL bRetExpectURL);
static HCHAR	*nx_filemgr_file_ExtractNameFromPath(HCHAR *szPath);
static HERROR	nx_filemgr_file_MakeAvFile(NxFileInfo_t *dstFileInfo, HxAV_SteamInfo_t *avInfo);
static HERROR	nx_filemgr_file_MakePngFile(NxFileInfo_t *dstFileInfo, HxPNG_Info_t *pngInfo);
static HERROR	nx_filemgr_file_MakeDirInfo(NxFileInfo_t *dstFileInfo, HINT32 lSize, HCHAR *pszFilter, DxDirInfo_t *dirInfo);
static HBOOL	nx_filemgr_file_GetSubFile(HCHAR *path, HCHAR *pszFilter);
static HERROR	nx_filemgr_file_SetThumbnail(NxFileInfo_t *pstNxFileInfo, NxFileType_e eType);
static HERROR	nx_filemgr_file_SetFileDate(NxFileInfo_t *pstNxFileInfo, HUINT64 ullTime);
static void		nx_filemgr_file_Umma_callback(int requestId, void *pvUserData, HxList_t *pstResultList, void *pvCbInfo);
static NxFileInfo_t	*nx_filemgr_file_MakeFileInfo(DxFileInfo_t *pstFileInfo, HCHAR *pszFilter);
static HERROR	nx_filemgr_file_CreateFileOperation(HCHAR *pszFilter);
static HERROR	nx_filemgr_file_DestroyFileOperation(void);
static HERROR	nx_filemgr_file_AddPath(HCHAR *pszFilePath);
static HERROR	nx_filemgr_file_RemovePath(HCHAR *pszFilePath);
static HERROR	nx_filemgr_file_RemoveFileGroup(void);
static HERROR	nx_filemgr_file_CopyFileGroup(HCHAR *pszPath);
static HERROR	nx_filemgr_file_MoveFileGroup(HCHAR *pszPath);
static HERROR	nx_filemgr_file_CancelOperation(void);

/**************************************************************************************************/
#define ___________________Private_functions________________________________________________
/**************************************************************************************************/
static NX_FileContext_t *nx_filemgr_file_GetContext(void)
{
	if (g_FileContext == NULL)
	{
		g_FileContext = (NX_FileContext_t *)NX_APP_Calloc(sizeof(NX_FileContext_t));
		g_FileContext->ulOperationId = 0;
		g_FileContext->ulRequestId = 0;
		g_FileContext->ulRequestCount = 0;
		g_FileContext->eFileOperationState = eFileOperation_Idle;
	}
	return g_FileContext;
}

static HCHAR	*nx_filemgr_file_GetThumbnail(HCHAR *szFileUrl, HUINT32 ulExtractPosition, const HCHAR *pszMountPath, HBOOL bRetExpectURL)
{
	HCHAR tempUrl[1024] = {0,};
	HCHAR *pstRet = NULL;
	HCHAR *pThumbnail = NULL;

#if defined(CONFIG_SUPPORT_THMA)
	pThumbnail = (HCHAR*)THAPI_GetThumbnailURL((HCHAR*)szFileUrl, ulExtractPosition, (HCHAR*)pszMountPath, bRetExpectURL);
#endif

	// pThumbnail 는 cbuf 이므로 release mem 필요없음.
	if (pThumbnail)
	{
		HxLOG_Debug("Get Thumbnail (%s)\n", pThumbnail);
		HxSTD_PrintToStrN(tempUrl, 1024, "%s", pThumbnail); // without file:///
		pstRet = HLIB_STD_StrDup(tempUrl);
		return pstRet;
	}

	return NULL;
}

static HCHAR	*nx_filemgr_file_ExtractNameFromPath(HCHAR *szPath)
{
	HCHAR *szNamePointer = NULL;
	HCHAR *szLastPointer = NULL;

	if (szPath == NULL)
	{
		HxLOG_Error("szPath is NULL!\n");
		return NULL;
	}

	szLastPointer = (HCHAR*)strrchr(szPath, '/');

	if (szLastPointer)
	{
		if (HxSTD_StrLen(szLastPointer) > 1)
		{
			szNamePointer = szLastPointer + 1;
			HxLOG_Debug("File Name(%s)\n", szNamePointer);
			return szNamePointer;
		}
	}
	return NULL;
}

static HERROR	nx_filemgr_file_MakeAvFile(NxFileInfo_t *dstFileInfo, HxAV_SteamInfo_t *avInfo)
{
	NxFileAV_t	*dstAvFile = &dstFileInfo->files.av;

	if (avInfo == NULL)
	{
		HxLOG_Error("avInfo is NULL!\n");
		return ERR_FAIL;
	}

	dstAvFile->nStreamType	= avInfo->nStreamType;
	dstAvFile->nDuration	= avInfo->nDuration;
	dstAvFile->lFrameCount	= avInfo->llFrameCount;
	dstAvFile->nWidth		= avInfo->nWidth;
	dstAvFile->nHeight		= avInfo->nHeight;
	HxSTD_PrintToStrN(dstAvFile->szCodecName, 32, "%s", avInfo->szCodecName);

	return ERR_OK;
}

static HERROR	nx_filemgr_file_MakePngFile(NxFileInfo_t *dstFileInfo, HxPNG_Info_t *pngInfo)
{
	NxFilePNG_t	*pdstPngFile = NULL;

	if(dstFileInfo == NULL)
	{
		HxLOG_Error("dstFileInfo is NULL!\n");
		return ERR_FAIL;
	}
	if (pngInfo == NULL)
	{
		HxLOG_Error("pngInfo is NULL!\n");
		return ERR_FAIL;
	}

	pdstPngFile = &dstFileInfo->files.png;

	pdstPngFile->nWidth		= pngInfo->nWidth;
	pdstPngFile->nHeight		= pngInfo->nHeight;
	pdstPngFile->ulPitch		= pngInfo->ulPitch;
	pdstPngFile->ucFormat	= pngInfo->ucFormat;
	pdstPngFile->ucBitPerPixel = pngInfo->ucBitPerPixel;

	return ERR_OK;
}

static HERROR	nx_filemgr_file_MakeDirInfo(NxFileInfo_t *dstFileInfo, HINT32 lSize, HCHAR *pszFilter, DxDirInfo_t *dirInfo)
{
	NxFileDir_t	*pdstDir = NULL;

	if(dstFileInfo == NULL)
	{
		HxLOG_Error("dstFileInfo is NULL!\n");
		return ERR_FAIL;
	}
	if (dirInfo == NULL)
	{
		HxLOG_Error("dirInfo is NULL!\n");
		return ERR_FAIL;
	}

	pdstDir = &dstFileInfo->files.dir;

	if (0 < lSize)
	{
		pdstDir->bHasSubFile = dirInfo->bHasSubFile ? TRUE : FALSE;
	}
	else
	{
		pdstDir->bHasSubFile = nx_filemgr_file_GetSubFile(dstFileInfo->szFullPath, pszFilter);
	}

	return ERR_OK;
}

static HBOOL	nx_filemgr_file_GetSubFile(HCHAR *path, HCHAR *pszFilter)
{
	HBOOL bRet = TRUE;
	/*
	struct dirent *dirent;
	struct 	stat fstat;
	char szBuf[DLIB_FILEINFO_NAME_LEN] = {0};
	DIR *dp = opendir(path);
	HCHAR *pExt = NULL;

	if (NULL != dp)
	{
		while ((dirent = readdir(dp)) != NULL) {
			if (DOT_OR_DOTDOT(dirent->d_name)) continue;

			HxSTD_PrintToStrN(szBuf, sizeof(szBuf), "%s/%s", path, dirent->d_name);
			HxLOG_Debug("get_SubFile's name ------> %s \n", dirent->d_name);

			pExt = HxSTD_StrRChar(szBuf, '.');
			if( NULL == pExt || NULL == pExt+1) continue;
			if( NULL == HLIB_STD_StrCaseStr(pszFilter, pExt+1)) continue;

			if(0 != lstat( szBuf, &fstat)) break;
			if(0 < HxSTD_StrLen(dirent->d_name) && !S_ISDIR(fstat.st_mode))
			{
				bRet = TRUE;
				break;
			}
			pExt = NULL;
		}
		closedir(dp);
	}
	*/
	return bRet;
}

static HERROR	nx_filemgr_file_SetThumbnail(NxFileInfo_t *pstNxFileInfo, NxFileType_e eType)
{
	HINT32 extractPosition = 0;

	switch(eType)
	{
	case eFileType_VIDEO:
		{
			HINT32 duration = pstNxFileInfo->files.av.nDuration;
			extractPosition = ((0 < duration && duration < NX_THUMBNAIL_EXTRACT_TIME) ? duration : NX_THUMBNAIL_EXTRACT_TIME);
		}
		break;

	case eFileType_MUSIC:
	case eFileType_PHOTO:
		extractPosition = 0;
		break;

	case eFileType_UNKNOWN:
	default:
		extractPosition = NX_THUMBNAIL_EXTRACT_TIME;
		break;
	}
	return ERR_OK;
}

static HERROR	nx_filemgr_file_SetFileDate(NxFileInfo_t *pstNxFileInfo, HUINT64 ullTime)
{
	HxDATETIME_t	stFileTime;

	HLIB_DATETIME_ConvertUnixTimeToDateTime(ullTime, &stFileTime);

	HxSTD_sprintf((char*)pstNxFileInfo->szDate, "%d:%d %d.%d.%d", stFileTime.stTime.ucHour, stFileTime.stTime.ucMinute,
						stFileTime.stDate.ucDay, stFileTime.stDate.ucMonth, stFileTime.stDate.usYear);

	return ERR_OK;
}

static void nx_filemgr_file_Umma_callback(int requestId, void *pvUserData, HxList_t *pstResultList, void *pvCbInfo)
{
	NX_FileContext_t	*fileContext = nx_filemgr_file_GetContext();
	AxFileMgrCbInfo_t	*pstInfo = (AxFileMgrCbInfo_t*)pvCbInfo;

	if(requestId == 0)
	{
		return;
	}

	HxLOG_Debug("[Umma_callback] requestId(%d), eState(%d), [%d/%d] \n", requestId, pstInfo->eState, pstInfo->nDoneOperationNum, pstInfo->nTotalOperationNum);
	if (pstInfo->nDoneOperationNum < pstInfo->nTotalOperationNum)
	{
		HxLOG_Debug("[Umma_callback] OperToekn Job is running [%d/%d]\n", pstInfo->nDoneOperationNum, pstInfo->nTotalOperationNum);
		return;
	}

	switch(pstInfo->eState)
	{
		case eAxFILEMGR_ERR_OK:
			if ( 0 > pstInfo->nTotalOperationNum)
			{
				/* start operation  : don't send event!! */
				return;
			}
			else
			{
				/* completed operation */
				fileContext->ulRequestCount--;
				if (fileContext->ulRequestCount <= 0)
				{
					fileContext->eFileOperationState = eFileOperation_Idle;
					ONDK_GWM_PostMessage(NULL, MSG_CORE_MEDIA_OPERATION_DONE, 0, 0, 0, 0);
				}
			}
			break;
		case eAxFILEMGR_ERR_ONGOING :
			HxLOG_Debug("[Umma_callback] ERROR_ONGING [%d/%d]\n", pstInfo->nDoneOperationNum, pstInfo->nTotalOperationNum);
			break;
		case eAxFILEMGR_ERR_4G_FAT:
			HxLOG_Debug("[Umma_callback] ERR_4G_FAT [%d/%d]\n", pstInfo->nDoneOperationNum, pstInfo->nTotalOperationNum);
			break;
		case eAxFILEMGR_ERR_NOT_ENOUGH_SPACE:
		case eAxFILEMGR_ERR_FAIL_STAT:
		case eAxFILEMGR_ERR_ERR :
		case eAxFILEMGR_ERR_CANCELED :
		default:
			fileContext->ulRequestCount = 0;
			fileContext->eFileOperationState = eFileOperation_Idle;
			ONDK_GWM_PostMessage(NULL, MSG_CORE_MEDIA_OPERATION_ERROR, 0, 0, 0, 0);
			break;
	}
	// Set State
	// Send Event
}

static NxFileInfo_t	*nx_filemgr_file_MakeFileInfo(DxFileInfo_t *pstFileInfo, HCHAR *pszFilter)
{
	HERROR			hErr = ERR_FAIL;
	NxFileInfo_t	*pstFileData = NULL;
	NxFileType_e 	eFileType = eFileType_UNKNOWN;

	if (pstFileInfo == NULL)
	{
		HxLOG_Error("pstFileInfo is NULL\n");
		return NULL;
	}

	pstFileData = (NxFileInfo_t*)NX_APP_Calloc(sizeof(NxFileInfo_t));
	if (pstFileData == NULL)
	{
		HxLOG_Error("pstFileData is NULL \n");
		return NULL;
	}

	HxSTD_sprintf((char*)pstFileData->szFullPath, "%s", (char*)pstFileInfo->szFileName);
	HxSTD_sprintf((char*)pstFileData->szFileName, "%s", (char*)nx_filemgr_file_ExtractNameFromPath(pstFileData->szFullPath));
	nx_filemgr_file_SetFileDate(pstFileData, pstFileInfo->st_stat.ullMTime);

	switch(pstFileInfo->eDataType)
	{
	case eDxFILETYPE_MUSIC:
		eFileType = eFileType_MUSIC;
		break;

	case eDxFILETYPE_EXIF:
		break;

	case eDxFILETYPE_PNG:
		if (pstFileInfo->st_stat.st_size > 0)
		{
			eFileType = eFileType_PHOTO;
			hErr = nx_filemgr_file_MakePngFile(pstFileData, (HxPNG_Info_t*)pstFileInfo->data);
		}
		break;

	case eDxFILETYPE_RECORDINGS:
		eFileType = eFileType_VIDEO;
		break;

	case eDxFILETYPE_AVSTREAM:
		if (pstFileInfo->st_stat.st_size > 0)
		{
			eFileType = eFileType_VIDEO;
			hErr = nx_filemgr_file_MakeAvFile(pstFileData, (HxAV_SteamInfo_t*)pstFileInfo->data);
		}
		break;

	case eDxFILETYPE_DIRECTORY:
		eFileType = eFileType_DIR;
		hErr = nx_filemgr_file_MakeDirInfo(pstFileData, pstFileInfo->size, pszFilter, (DxDirInfo_t*)pstFileInfo->data);
		break;

	default:
		break;
	}

	nx_filemgr_file_SetThumbnail(pstFileData, eFileType);

	if (hErr == ERR_FAIL)
	{
		if (pstFileData)
		{
			NX_APP_Free(pstFileData);
			pstFileData = NULL;
		}
		HxLOG_Error("Can't Make NxFileInfo.!\n");
		return NULL;
	}

	return pstFileData;
}

static HERROR	nx_filemgr_file_CreateFileOperation(HCHAR *pszFilter)
{
	NX_FileContext_t *fileContext = nx_filemgr_file_GetContext();

	if (fileContext->ulOperationId != 0)
	{
		APK_FILEMGR_GROUP_Release(fileContext->ulOperationId);
	}

	fileContext->ulOperationId = APK_FILEMGR_GROUP_Create(pszFilter);

	if (fileContext->ulOperationId == 0)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR	nx_filemgr_file_DestroyFileOperation(void)
{
	NX_FileContext_t *fileContext = nx_filemgr_file_GetContext();

	if (fileContext->ulOperationId != 0)
	{
		APK_FILEMGR_GROUP_Release(fileContext->ulOperationId);
	}
	fileContext->ulOperationId = 0;

	return ERR_OK;
}

static HERROR	nx_filemgr_file_AddPath(HCHAR *pszFilePath)
{
	NX_FileContext_t *fileContext = nx_filemgr_file_GetContext();

	return APK_FILEMGR_GROUP_AddPath(fileContext->ulOperationId, pszFilePath);
}

static HERROR	nx_filemgr_file_RemovePath(HCHAR *pszFilePath)
{
	NX_FileContext_t *fileContext = nx_filemgr_file_GetContext();

	return APK_FILEMGR_GROUP_RemovePath(fileContext->ulOperationId, pszFilePath);
}

static HERROR	nx_filemgr_file_RemoveFileGroup(void)
{
	NX_FileContext_t 	*fileContext = nx_filemgr_file_GetContext();
	HERROR 				hErr = ERR_FAIL;

	hErr = APK_FILEMGR_GROUP_Remove(fileContext->ulOperationId, &fileContext->ulRequestId);
	if (ERR_OK == hErr)
	{
		fileContext->ulRequestCount++;
		fileContext->eFileOperationState = eFileOperation_Deleting;
		APK_FILEMGR_AddListener(fileContext->ulRequestId, NULL, nx_filemgr_file_Umma_callback);
	}

	return ERR_OK;
}

static HERROR	nx_filemgr_file_CopyFileGroup(HCHAR *pszPath)
{
	NX_FileContext_t 	*fileContext = nx_filemgr_file_GetContext();
	HERROR 				hErr = ERR_FAIL;

	hErr = APK_FILEMGR_GROUP_Copy(fileContext->ulOperationId, pszPath, &fileContext->ulRequestId);
	if ( ERR_OK == hErr )
	{
		fileContext->ulRequestCount++;
		fileContext->eFileOperationState = eFileOperation_Copying;
		APK_FILEMGR_AddListener(fileContext->ulRequestId, NULL, nx_filemgr_file_Umma_callback);
	}

	return ERR_OK;
}

static HERROR	nx_filemgr_file_MoveFileGroup(HCHAR *pszPath)
{
	NX_FileContext_t 	*fileContext = nx_filemgr_file_GetContext();
	HERROR 				hErr = ERR_FAIL;

	hErr = APK_FILEMGR_GROUP_Move(fileContext->ulOperationId, pszPath, &fileContext->ulRequestId);
	if ( ERR_OK == hErr )
	{
		fileContext->ulRequestCount++;
		fileContext->eFileOperationState = eFileOperation_Moving;
		APK_FILEMGR_AddListener(fileContext->ulRequestId, NULL, nx_filemgr_file_Umma_callback);
	}

	return ERR_OK;
}

static HERROR	nx_filemgr_file_CancelOperation(void)
{
	NX_FileContext_t 	*fileContext = nx_filemgr_file_GetContext();
	HERROR 				hErr = ERR_FAIL;

	hErr = APK_FILEMGR_GROUP_Cancel(fileContext->ulOperationId, &fileContext->ulRequestId);
	if ( ERR_OK == hErr )
	{
		fileContext->ulRequestCount++;
		APK_FILEMGR_AddListener(fileContext->ulRequestId, NULL, nx_filemgr_file_Umma_callback);
	}

	return ERR_OK;
}

/**************************************************************************************************/
#define ___________________Public_functions________________________________________________
/**************************************************************************************************/
NxFileInfo_t	*NX_FILEMGR_FILE_MakeNxFileInfo(DxFileInfo_t *pstFileInfo, HCHAR *pszFilter)
{
	return nx_filemgr_file_MakeFileInfo(pstFileInfo, pszFilter);
}

HCHAR *NX_FILEMGR_FILE_ExtractNameFromPath(HCHAR *szPath)
{
	return nx_filemgr_file_ExtractNameFromPath(szPath);
}

HCHAR *NX_FILEMGR_FILE_GetThumbnail(HCHAR *szUrl, HUINT32 ulExtractPosition, const HCHAR *pszMountPath, HBOOL bRetExpectURL)
{
	return nx_filemgr_file_GetThumbnail(szUrl, ulExtractPosition, pszMountPath, bRetExpectURL);
}

NxFileOperation_e	NX_FILEMGR_FILE_GetFileOperationStatus(void)
{
	NX_FileContext_t 	*fileContext = nx_filemgr_file_GetContext();

	return fileContext->eFileOperationState;
}

HERROR	NX_FILEMGR_FILE_CreateFileOperation(HCHAR *pszFilter)
{
	return nx_filemgr_file_CreateFileOperation(pszFilter);
}

HERROR	NX_FILEMGR_FILE_AddPath(HCHAR *pszFilePath)
{
	return nx_filemgr_file_AddPath(pszFilePath);
}

HERROR	NX_FILEMGR_FILE_RemovePath(HCHAR *pszFilePath)
{
	return nx_filemgr_file_RemovePath(pszFilePath);
}

HERROR	NX_FILEMGR_FILE_RemoveFileGroup(void)
{
	return nx_filemgr_file_RemoveFileGroup();
}

HERROR	NX_FILEMGR_FILE_CopyFileGroup(HCHAR *pszPath)
{
	return nx_filemgr_file_CopyFileGroup(pszPath);
}

HERROR	NX_FILEMGR_FILE_MoveFileGroup(HCHAR *pszPath)
{
	return nx_filemgr_file_MoveFileGroup(pszPath);
}

HERROR	NX_FILEMGR_FILE_CancelOperation(void)
{
	return nx_filemgr_file_CancelOperation();
}


