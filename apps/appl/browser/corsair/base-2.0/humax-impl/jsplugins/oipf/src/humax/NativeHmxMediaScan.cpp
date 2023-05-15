
/**************************************************************************************/
/**
 * @file NativeHmxMediaScan.cpp
 *
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NativeHmxMediaScan.h"
#include "macros.h"
#include "oplmediascan.h"

#ifdef OIPF

static	scanListener		s_tMediaScanEventCallback = NULL;
static	scanDirListener		s_tMediaScanDirEventCallback = NULL;
static	fileOpListener		s_tMediaFileOpEventCallback = NULL;

static char s_szVideoInfo[OPL_MAX_PATH_STR_LEN];

void	_MediaScanCallback(void **contentList, int param1, unsigned int param2)
{
	if(s_tMediaScanEventCallback)
	{
		OOIF_LOG_DEBUG( "[_MediaScanCallback] %d %d\n", param1, param2);
		s_tMediaScanEventCallback(contentList, param1, param2);
	}
}
//
// MediaScan
//
OOIFReturnCode native_HMX_MediaScanningResultListener(scanListener listener)
{
	if( s_tMediaScanEventCallback == NULL )
	{
		s_tMediaScanEventCallback = listener;
		OPL_HMX_MediaScanningResultListener( _MediaScanCallback );
	}
	return OOIF_RETURN_OK;
}


void	_DirScanCallback(void **contentList, int param1, unsigned int param2)
{
	if(s_tMediaScanDirEventCallback)
	{
		OOIF_LOG_DEBUG( "[_DirScanCallback] %d %d\n", param1, param2);
		s_tMediaScanDirEventCallback(contentList, param1, param2);
	}
}


OOIFReturnCode native_HMX_DirScanningResultListener(scanDirListener listener)
{
	if( s_tMediaScanDirEventCallback == NULL )
	{
		s_tMediaScanDirEventCallback = listener;
		OPL_HMX_DirScanningResultListener( _DirScanCallback );
	}
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_HMX_MediaBrowse(const char *szURL, int nSortOrder, int nPageNum, const char *szFileExts, int *retval )
{
	static MediaParam_t param;
	snprintf(param.szFullPath, OPL_MAX_PATH_STR_LEN, "%s", szURL);
	snprintf(param.szFileFilters, OPL_MAX_TITLE_STR_LEN, "%s", szFileExts);
	param.nSortOrder = nSortOrder;
	param.nPageNum = nPageNum;
	OOIF_LOG_DEBUG("debug  ", "\n[%s,%d] %s %s %d ################", __FUNCTION__, __LINE__ , param.szFullPath, param.szFileFilters, param.nPageNum);

	OPL_HMX_MediaBrowse( (void *)&param, retval );

	return OOIF_RETURN_OK;
}


OOIFReturnCode native_HMX_DirBrowse(const char *szURL, int nSortOrder, int nPageNum, const char *szFileExts, int *retval )
{
	static MediaParam_t param;
	snprintf(param.szFullPath, OPL_MAX_PATH_STR_LEN, "%s", szURL);
	snprintf(param.szFileFilters, OPL_MAX_TITLE_STR_LEN, "%s", szFileExts);
	param.nSortOrder = nSortOrder;
	param.nPageNum = nPageNum;
	OOIF_LOG_DEBUG("debug  ", "\n[%s,%d] %s %s %d ################", __FUNCTION__, __LINE__ , param.szFullPath, param.szFileFilters, param.nPageNum);

	OPL_HMX_DirBrowse( (void *)&param, retval );

	return OOIF_RETURN_OK;
}


void	_FileOpCallback(unsigned int param1, int param2, unsigned int param3)
{
	if(s_tMediaFileOpEventCallback)
	{
		OOIF_LOG_DEBUG( "[_DirScanCallback] %d %d %d\n", param1, param2, param3);
		s_tMediaFileOpEventCallback(param1, param2, param3);
	}
}


OOIFReturnCode native_HMX_FileOpResultListener(fileOpListener listener)
{
	if(s_tMediaFileOpEventCallback == NULL)
	{
		s_tMediaFileOpEventCallback = listener;
		OPL_HMX_FileOpResultListener( _FileOpCallback );
	}
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_HMX_FileOpDo(int nType, int nIsSyncMode, int nParamCount, const char *szURLs, const char *szRename, const char *szDestUrl, int *retval)
{
	static FILEOpParam_t stFileParam;
	int		idx = 0;

	memset(&stFileParam, 0, sizeof(FILEOpParam_t));
	stFileParam.IsSyncMode = nIsSyncMode;
	stFileParam.ulParamCount = nParamCount;

	switch (nType)
	{
		case 1 :	// rename
			if(nParamCount < 1)
			{
				OOIF_LOG_DEBUG( "[native_HMX_FileOpDo] %d	nParamCount more than 1.\n", nParamCount);
				return OOIF_RETURN_ERROR;
			}
			OOIF_LOG_DEBUG( "[native_HMX_FileOpDo] RENAME	ORG=%s REN=%s\n", szURLs, szRename);
			stFileParam.ulParamCount = 1;
			snprintf(stFileParam.szRename, OPL_MAX_PATH_STR_LEN, "%s", szRename);
			if(strlen(stFileParam.szRename) < 1)
			{
				OOIF_LOG_DEBUG( "[native_HMX_FileOpDo] ERR RENAME 1 more than character size %s\n", stFileParam.szRename);
				return OOIF_RETURN_ERROR;
			}
			stFileParam.paramList		= (URL_PARAM_t*)HLIB_STD_MemAlloc(sizeof(URL_PARAM_t) * stFileParam.ulParamCount);
			memset(stFileParam.paramList, 0, sizeof(URL_PARAM_t) * stFileParam.ulParamCount);
			snprintf(stFileParam.paramList[0].szURL, OPL_MAX_PATH_STR_LEN, "%s", szURLs);
			if(strstr(stFileParam.paramList[0].szURL, "/m") == NULL)
			{
				OOIF_LOG_DEBUG( "[native_HMX_FileOpDo] ERR RENAM filepath %s\n", stFileParam.paramList[0].szURL);
				HLIB_STD_MemFree(stFileParam.paramList);
				return OOIF_RETURN_ERROR;
			}
			OPL_HMX_FileRename(&stFileParam);
			break;

		case 2 :	// copy
			if(nParamCount < 1)
			{
				OOIF_LOG_DEBUG("debug  ", "[native_HMX_FileOpDo] %d	nParamCount more than 1.\n", nParamCount);
				return OOIF_RETURN_ERROR;
			}
			snprintf(stFileParam.szDestUrl, OPL_MAX_PATH_STR_LEN, "%s", szDestUrl);
			if(strstr(stFileParam.szDestUrl, "/m") == NULL)
			{
				OOIF_LOG_DEBUG("debug  ", "[native_HMX_FileOpDo] ERR COPY filepath %s\n", stFileParam.szDestUrl);
				return OOIF_RETURN_ERROR;
			}
			stFileParam.paramList		= (URL_PARAM_t*)HLIB_STD_MemAlloc(sizeof(URL_PARAM_t) * stFileParam.ulParamCount);
			memset(stFileParam.paramList, 0, sizeof(URL_PARAM_t) * stFileParam.ulParamCount);
			if(nParamCount > 1)
			{
				int len = 0;
				char *pTemp = NULL;
				char *p = (char *)szURLs;

				for(idx = 0; idx < stFileParam.ulParamCount; idx++)
				{
					if(p)
					{
						pTemp = strchr(p, '<');
						if(pTemp)
						{
							len = pTemp - p;
							strncpy(stFileParam.paramList[idx].szURL, p, len);
							stFileParam.paramList[idx].szURL[len] = '\0';
							p = pTemp + 1;
							OOIF_LOG_DEBUG("debug  ", "[native_HMX_FileOpDo] COPY	%dst url=%s TargetDir=%s\n", idx, stFileParam.paramList[idx].szURL, stFileParam.szDestUrl);
						}
						else if((idx + 1) == stFileParam.ulParamCount)
						{
							len = strlen(p);
							strncpy(stFileParam.paramList[idx].szURL, p, len);
							stFileParam.paramList[idx].szURL[len] = '\0';
							OOIF_LOG_DEBUG("debug  ", "[native_HMX_FileOpDo] COPY	%dst url=%s TargetDir=%s\n", idx, stFileParam.paramList[idx].szURL, stFileParam.szDestUrl);
						}
					}
				}
			}
			else
			{
				snprintf(stFileParam.paramList[0].szURL, OPL_MAX_PATH_STR_LEN, "%s", szURLs);
				OOIF_LOG_DEBUG("debug  ", "[native_HMX_FileOpDo] COPY	1st url=%s TargetDir=%s\n", stFileParam.paramList[0].szURL, stFileParam.szDestUrl);
			}
			if(strstr(stFileParam.paramList[0].szURL, "/m") == NULL)
			{
				OOIF_LOG_DEBUG("debug  ", "[native_HMX_FileOpDo] ERR COPY filepath %s\n", stFileParam.paramList[0].szURL);
				HLIB_STD_MemFree(stFileParam.paramList);
				return OOIF_RETURN_ERROR;
			}
			OPL_HMX_FileCopy(&stFileParam);
			break;

		case 0 :	// delete
			if(nParamCount < 1)
			{
				OOIF_LOG_DEBUG("debug  ", "[native_HMX_FileOpDo] %d	nParamCount more than 1.\n", nParamCount);
				return OOIF_RETURN_ERROR;
			}
			stFileParam.paramList		= (URL_PARAM_t*)HLIB_STD_MemAlloc(sizeof(URL_PARAM_t) * stFileParam.ulParamCount);
			memset(stFileParam.paramList, 0, sizeof(URL_PARAM_t) * stFileParam.ulParamCount);
			if(nParamCount > 1)
			{
				int len = 0;
				char *pTemp = NULL;
				char *p = (char *)szURLs;

				for(idx = 0; idx < stFileParam.ulParamCount; idx++)
				{
					if(p)
					{
						pTemp = strchr(p, '<');
						if(pTemp)
						{
							len = pTemp - p;
							strncpy(stFileParam.paramList[idx].szURL, p, len);
							stFileParam.paramList[idx].szURL[len] = '\0';
							p = pTemp + 1;
							OOIF_LOG_DEBUG("debug  ", "[native_HMX_FileOpDo] DELETE	%dst url=%s\n", idx, stFileParam.paramList[idx].szURL);
						}
						else if((idx + 1) == stFileParam.ulParamCount)
						{
							len = strlen(p);
							strncpy(stFileParam.paramList[idx].szURL, p, len);
							stFileParam.paramList[idx].szURL[len] = '\0';
							OOIF_LOG_DEBUG("debug  ", "[native_HMX_FileOpDo] DELETE	%dst url=%s\n", idx, stFileParam.paramList[idx].szURL);
						}
					}
				}
			}
			else
			{
				snprintf(stFileParam.paramList[0].szURL, OPL_MAX_PATH_STR_LEN, "%s", szURLs);
				OOIF_LOG_DEBUG("debug  ", "[native_HMX_FileOpDo] DELETE	1st url=%s\n", stFileParam.paramList[0].szURL);
			}
			if(strstr(stFileParam.paramList[0].szURL, "/m") == NULL)
			{
				OOIF_LOG_DEBUG("debug  ", "[native_HMX_FileOpDo] ERR DELETE filepath %s\n", stFileParam.paramList[0].szURL);
				HLIB_STD_MemFree(stFileParam.paramList);
				return OOIF_RETURN_ERROR;
			}
			OPL_HMX_FileDelete(&stFileParam);
			break;

		case 3 :	// Make Directory
			snprintf(stFileParam.szDestUrl, OPL_MAX_PATH_STR_LEN, "%s", szDestUrl);
			if(strstr(stFileParam.szDestUrl, "/m") == NULL)
			{
				OOIF_LOG_DEBUG( "[native_HMX_FileOpDo] ERR MakeDir filepath %s\n", stFileParam.szDestUrl);
				return OOIF_RETURN_ERROR;
			}
			OPL_HMX_MakeDir(&stFileParam);
			break;

		case 4 :	// Cancel Copy/Delete
			OPL_HMX_FileCancel(&stFileParam);
			break;

		case 5 :	// USB AutoCopy
			if(nParamCount < 1 || nParamCount > 4)
			{
				OOIF_LOG_DEBUG("debug  ",  "[native_HMX_FileOpDo] %d	nParamCount more than 1.\n", nParamCount);
				return OOIF_RETURN_ERROR;
			}
			snprintf(stFileParam.szDestUrl, OPL_MAX_PATH_STR_LEN, "%s", szDestUrl);
			if(strstr(stFileParam.szDestUrl, "/m") == NULL)
			{
				OOIF_LOG_DEBUG("debug  ", "[native_HMX_FileOpDo] ERR AUTOCOPY filepath %s\n", stFileParam.szDestUrl);
				return OOIF_RETURN_ERROR;
			}
			stFileParam.paramList		= (URL_PARAM_t*)HLIB_STD_MemAlloc(sizeof(URL_PARAM_t) * stFileParam.ulParamCount);
			memset(stFileParam.paramList, 0, sizeof(URL_PARAM_t) * stFileParam.ulParamCount);
			if(nParamCount > 1)
			{
				int len = 0;
				char *pTemp = NULL;
				char *p = (char *)szURLs;

				for(idx = 0; idx < stFileParam.ulParamCount; idx++)
				{
					if(p)
					{
						pTemp = strchr(p, '<');
						if(pTemp)
						{
							len = pTemp - p;
							strncpy(stFileParam.paramList[idx].szURL, p, len);
							stFileParam.paramList[idx].szURL[len] = '\0';
							p = pTemp + 1;
							OOIF_LOG_DEBUG("debug  ", "[native_HMX_FileOpDo] AUTOCOPY	%dst url=%s TargetDir=%s\n", idx, stFileParam.paramList[idx].szURL, stFileParam.szDestUrl);
						}
						else if((idx + 1) == stFileParam.ulParamCount)
						{
							len = strlen(p);
							strncpy(stFileParam.paramList[idx].szURL, p, len);
							stFileParam.paramList[idx].szURL[len] = '\0';
							OOIF_LOG_DEBUG("debug  ", "[native_HMX_FileOpDo] AUTOCOPY	%dst url=%s TargetDir=%s\n", idx, stFileParam.paramList[idx].szURL, stFileParam.szDestUrl);
						}
					}
				}
			}
			else
			{
				snprintf(stFileParam.paramList[0].szURL, OPL_MAX_PATH_STR_LEN, "%s", szURLs);
				OOIF_LOG_DEBUG("debug  ", "[native_HMX_FileOpDo] AUTOCOPY	1st url=%s TargetDir=%s\n", stFileParam.paramList[0].szURL, stFileParam.szDestUrl);
			}
			OPL_HMX_AutoCopy(&stFileParam);
			break;

		default:
			return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}


OOIFReturnCode native_HMX_IsFileExist(const char *szURL, int *retval )
{
	OPL_HMX_IsFileExist(szURL, retval);
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_HMX_IsDirExist(const char *szURL, int *retval )
{
	OPL_HMX_IsDirExist(szURL, retval);
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_HMX_GetVideoThumbnailUrl(const char *szURL, const char **retval )
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}
	memset(s_szVideoInfo, 0, OPL_MAX_PATH_STR_LEN);
	OPL_HMX_GetVideoThumbnailUrl(szURL, s_szVideoInfo);
	*retval = s_szVideoInfo;

	return OOIF_RETURN_OK;
}


OOIFReturnCode native_HMX_GetVideoDuration(const char *szURL, const char **retval )
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}
	memset(s_szVideoInfo, 0, OPL_MAX_PATH_STR_LEN);
	OPL_HMX_GetVideoDuration(szURL, s_szVideoInfo);
	*retval = s_szVideoInfo;

	return OOIF_RETURN_OK;
}


OOIFReturnCode native_HMX_GetVideoPlayPosition(const char *szURL, const char **retval )
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}
	memset(s_szVideoInfo, 0, OPL_MAX_PATH_STR_LEN);
	OPL_HMX_GetVideoPlayPosition(szURL, s_szVideoInfo);
	*retval = s_szVideoInfo;

	return OOIF_RETURN_OK;
}


OOIFReturnCode native_HMX_GetVideoWatched(const char *szURL, const char **retval )
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}
	memset(s_szVideoInfo, 0, OPL_MAX_PATH_STR_LEN);
	OPL_HMX_GetVideoWatched(szURL, s_szVideoInfo);
	*retval = s_szVideoInfo;

	return OOIF_RETURN_OK;
}


OOIFReturnCode native_HMX_GetSpaceInfo(const char *szURL, const char **retval )
{
	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}
	memset(s_szVideoInfo, 0, OPL_MAX_PATH_STR_LEN);
	OPL_HMX_GetSpaceInfo(szURL, s_szVideoInfo);
	*retval = s_szVideoInfo;

	return OOIF_RETURN_OK;
}


OOIFReturnCode native_HMX_IsCopying(int *retval )
{
	OPL_HMX_IsCopying(retval);
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_HMX_IsDeleting(int *retval )
{
	OPL_HMX_IsDeleting(retval);
	return OOIF_RETURN_OK;
}


//
// MediaContent
//
OOIFReturnCode native_HMX_MediaContentReleaseHandle(MediaContentHandle id)
{
	MediaContent_t *content = (MediaContent_t *)id;
	if ( NULL != id )
	{
		delete content;
		content = NULL;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HMX_MediaContentGetFileType(MediaContentHandle identifier, OOIFNumber *retval)
{
	MediaContent_t	*ApInfo =  (MediaContent_t *)identifier;
	*retval = ApInfo->m_nFileType;
	OOIF_LOG_DEBUG( "[native_HMX_MediaContentGetFileType]%d\n", ApInfo->m_nFileType);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HMX_MediaContentGetFullPath(MediaContentHandle identifier, const char **retval)
{
	MediaContent_t	*ApInfo = (MediaContent_t *)identifier;
	*retval = ApInfo->m_szFullPath;
	OOIF_LOG_DEBUG( "[native_HMX_MediaContentGetFullPath]%s %s\n", ApInfo->m_szFullPath, *retval);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HMX_MediaContentGetHasSubitem(MediaContentHandle identifier, OOIFNumber *retval)
{
	MediaContent_t	*ApInfo = (MediaContent_t *)identifier;
	*retval = ApInfo->m_nHasSubitem;
	OOIF_LOG_DEBUG( "[native_HMX_MediaContentGetHasSubitem]%d\n", ApInfo->m_nHasSubitem);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HMX_MediaContentGetDate(MediaContentHandle identifier, const char **retval)
{
	MediaContent_t	*ApInfo = (MediaContent_t *)identifier;
	*retval = ApInfo->m_szSDate;
	OOIF_LOG_DEBUG( "[native_HMX_MediaContentGetDate]%s %s\n", ApInfo->m_szSDate, *retval);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HMX_MediaContentGetDuration(MediaContentHandle identifier, OOIFNumber *retval)
{
	MediaContent_t	*ApInfo = (MediaContent_t *)identifier;
	*retval = ApInfo->m_nDuration;
	OOIF_LOG_DEBUG( "[native_HMX_MediaContentGetDuration]%d\n", ApInfo->m_nDuration);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HMX_MediaContentGetPlayPosition(MediaContentHandle identifier, OOIFNumber *retval)
{
	MediaContent_t	*ApInfo = (MediaContent_t *)identifier;
	*retval = ApInfo->m_nPlayPosition;
	OOIF_LOG_DEBUG( "[native_HMX_MediaContentGetPlayPosition]%d\n", ApInfo->m_nPlayPosition);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HMX_MediaContentGetLargeThumbnailUrl(MediaContentHandle identifier, const char **retval)
{
	MediaContent_t	*ApInfo = (MediaContent_t *)identifier;
	*retval = ApInfo->m_szLargeThumbnailUrl;
	OOIF_LOG_DEBUG( "[native_HMX_MediaContentGetLargeThumbnailUrl]%s %s\n", ApInfo->m_szLargeThumbnailUrl, *retval);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HMX_MediaContentGetThumbnailUrl(MediaContentHandle identifier, const char **retval)
{
	MediaContent_t	*ApInfo = (MediaContent_t *)identifier;
	*retval = ApInfo->m_szThumbnailUrl;
	OOIF_LOG_DEBUG( "[native_HMX_MediaContentGetThumbnailUrl]%s %s\n", ApInfo->m_szThumbnailUrl, *retval);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HMX_MediaContentGetPicureResolution(MediaContentHandle identifier, const char **retval)
{
	MediaContent_t	*ApInfo = (MediaContent_t *)identifier;
	*retval = ApInfo->m_szResolution;
	OOIF_LOG_DEBUG( "[native_HMX_MediaContentGetPicureResolution]%s %s\n", ApInfo->m_szResolution, *retval);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HMX_MediaContentGetThumbResolution(MediaContentHandle identifier, const char **retval)
{
	MediaContent_t	*ApInfo = (MediaContent_t *)identifier;
	*retval = ApInfo->m_szThumbResolution;
	OOIF_LOG_DEBUG( "[native_HMX_MediaContentGetThumbResolution]%s %s\n", ApInfo->m_szThumbResolution, *retval);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HMX_MediaContentGetPictureDate(MediaContentHandle identifier, const char **retval)
{
	MediaContent_t	*ApInfo = (MediaContent_t *)identifier;
	*retval = ApInfo->m_szPictureDate;
	OOIF_LOG_DEBUG( "[native_HMX_MediaContentGetPictureDate]%s %s\n", ApInfo->m_szPictureDate, *retval);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HMX_MediaContentGetMusicAlbum(MediaContentHandle identifier, const char **retval)
{
	MediaContent_t	*ApInfo = (MediaContent_t *)identifier;
	*retval = ApInfo->m_szAlbum;
	OOIF_LOG_DEBUG( "[native_HMX_MediaContentGetMusicAlbum]%s %s\n", ApInfo->m_szAlbum, *retval);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HMX_MediaContentGetMusicArtist(MediaContentHandle identifier, const char **retval)
{
	MediaContent_t	*ApInfo = (MediaContent_t *)identifier;
	*retval = ApInfo->m_szArtist;
	OOIF_LOG_DEBUG( "[native_HMX_MediaContentGetMusicArtist]%s %s\n", ApInfo->m_szArtist, *retval);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HMX_MediaContentGetWatched(MediaContentHandle identifier, OOIFNumber *retval)
{
	MediaContent_t	*ApInfo = (MediaContent_t *)identifier;
	*retval = ApInfo->m_nWatched;
	OOIF_LOG_DEBUG( "[native_HMX_MediaContentGetWatched]%d\n", ApInfo->m_nWatched);
	return OOIF_RETURN_OK;
}

#endif // OIPF

