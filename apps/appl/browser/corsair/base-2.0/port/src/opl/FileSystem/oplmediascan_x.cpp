/**************************************************************************************/
/**
 * @file oplmediascan.c
 *
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include "oplmediascan.h"

#include "oapi.h"
#include "apk.h"
#include <hlib.h>


/******************************************************************
	Define
******************************************************************/

/******************************************************************
	Static functions
******************************************************************/
static scanEventCallback s_scanListener = NULL;
static scanDirEventCallback s_scanDirListener = NULL;
static fileOpEventCallback s_fileOpListener = NULL;


/******************************************************************
	Interfaces
******************************************************************/

void _OPL_MediaScan_EventHandler( void **id, int ulParam2, unsigned int ulParam3 )
{
	if ( NULL != s_scanListener )
	{
		HxLOG_Print("[_OPL_MediaScan_EventHandler] %d %d\n", ulParam2, ulParam3);
		s_scanListener( id, ulParam2, ulParam3  );
	}

}


void	OPL_HMX_MediaScanningResultListener( scanEventCallback listener )
{
	if(s_scanListener == NULL)
	{
		s_scanListener = listener;
		HxLOG_Print("[OPL_HMX_MediaScanningResultListener] \n");
		MEDIA_FS_RegisterScanEventCallback((MEDIA_FS_Scan_Callback_t)_OPL_MediaScan_EventHandler );
	}
}


int		OPL_HMX_MediaBrowse( void *param, int *retval )
{
	MEDIA_FS_StartScan(param, retval);
	return 0;
}


void _OPL_DirScan_EventHandler( void **id, int ulParam2, unsigned int ulParam3 )
{
	if ( NULL != s_scanDirListener )
	{
		s_scanDirListener( id, ulParam2, ulParam3  );
	}

}


void	OPL_HMX_DirScanningResultListener( scanDirEventCallback listener )
{
	if(s_scanDirListener == NULL)
	{
		s_scanDirListener = listener;
		MEDIA_FS_RegisterDirScanEventCallback((MEDIA_FS_Scan_Callback_t)_OPL_DirScan_EventHandler );
	}
}


int		OPL_HMX_DirBrowse( void *param, int *retval )
{
	MEDIA_FS_StartDirScan(param, retval);
	return 0;
}


void _OPL_FileOp_EventHandler( unsigned int ulParam1, int ulParam2, unsigned int ulParam3 )
{
	if ( NULL != s_fileOpListener )
	{
		//printf("[_OPL_FileOp_EventHandler] %d %d %d\n", ulParam1, ulParam2, ulParam3);
		s_fileOpListener( ulParam1, ulParam2, ulParam3  );
	}

}


void	OPL_HMX_FileOpResultListener( fileOpEventCallback listener )
{
	if(s_fileOpListener == NULL)
	{
		s_fileOpListener = listener;
		//APK_METAMEDIALIST_RegisterEventCallback((fileOpEventCallback)_OPL_FileOp_EventHandler );
		//printf("[OPL_HMX_FileOpResultListener]\n");
		MEDIA_FS_RegisterFileOpEventCallback((MEDIA_FS_fileOpEventCallback)_OPL_FileOp_EventHandler );
	}
}


int		OPL_HMX_FileDelete(FILEOpParam_t *param)
{
	HERROR							hErr		= ERR_FAIL;
	APKS_META_P_MEDIALIST_DELETE_t	stDelete	= {0,};
	HUINT32							idx			= 0;

	//printf("[OPL_HMX_FileDelete] begin\n");
	if(param && param->paramList)
	{
		stDelete.ulParamCount	= param->ulParamCount;
		stDelete.paramList		= (APKS_META_P_MEDIALIST_CID_PARAM_t*)HLIB_STD_MemAlloc(sizeof(APKS_META_P_MEDIALIST_CID_PARAM_t) * stDelete.ulParamCount);
		if (stDelete.paramList)
		{
			memset(stDelete.paramList, 0, sizeof(APKS_META_P_MEDIALIST_CID_PARAM_t) * stDelete.ulParamCount);
			for(idx = 0; idx < stDelete.ulParamCount; idx++)
			{
				HxLOG_Print("[%s:%d] filename (%s)\n",__FUNCTION__,__LINE__, param->paramList[idx].szURL);
				HLIB_STD_StrUtf8NCpy(stDelete.paramList[idx].szURL, param->paramList[idx].szURL, APKD_META_MEDIALIST_MAX_URL);
			}
			hErr = MEDIA_FS_RequestDelete(&stDelete, param->IsSyncMode);
			HLIB_STD_MemFree(stDelete.paramList);
			HLIB_STD_MemFree(param->paramList);

			return ERR_OK;
		}
	}

	return ERR_FAIL;
}


int		OPL_HMX_FileRename(FILEOpParam_t *param)
{
	HERROR							hErr		= ERR_FAIL;
	APKS_META_P_MEDIALIST_RENAME_t	stRename	= {0,};

	//printf("[OPL_HMX_FileRename] begin\n");
	if(param && param->paramList)
	{
		stRename.ulParamCount	= 1;
		stRename.paramList		= (APKS_META_P_MEDIALIST_CID_RENAME_PARAM_t*)HLIB_STD_MemAlloc(sizeof(APKS_META_P_MEDIALIST_CID_RENAME_PARAM_t) * stRename.ulParamCount);
		if (stRename.paramList)
		{
			memset(stRename.paramList, 0, sizeof(APKS_META_P_MEDIALIST_CID_RENAME_PARAM_t) * stRename.ulParamCount);
			//printf("[OPL_HMX_FileRename] 1.[%s:%d] filename (%s)\n",__FUNCTION__,__LINE__, param->paramList[0].szURL);
			HxLOG_Print("[%s:%d] filename (%s)\n",__FUNCTION__,__LINE__, param->paramList[0].szURL);
			HLIB_STD_StrUtf8NCpy(stRename.paramList[0].szRename, param->szRename, APKD_META_MEDIALIST_MAX_URL);
			HLIB_STD_StrUtf8NCpy(stRename.paramList[0].szURL, param->paramList[0].szURL, APKD_META_MEDIALIST_MAX_URL);
			hErr = MEDIA_FS_RequestRename(&stRename);
			HLIB_STD_MemFree(stRename.paramList);
			HLIB_STD_MemFree(param->paramList);

			return ERR_OK;
		}
	}

	return ERR_FAIL;
}


 int		OPL_HMX_FileCopy(FILEOpParam_t *param)
{
	HERROR							hErr		= ERR_FAIL;
	APKS_META_P_MEDIALIST_COPY_t		stCopy	= {{} , 0};
	HUINT32							idx			= 0;

	//printf("[OPL_HMX_FileCopy] begin\n");
	if(param && param->paramList)
	{
		stCopy.ulParamCount	= param->ulParamCount;
		stCopy.paramList		= (APKS_META_P_MEDIALIST_CID_PARAM_t*)HLIB_STD_MemAlloc(sizeof(APKS_META_P_MEDIALIST_CID_PARAM_t) * stCopy.ulParamCount);
		if (stCopy.paramList)
		{
			memset(stCopy.paramList, 0, sizeof(APKS_META_P_MEDIALIST_CID_PARAM_t) * stCopy.ulParamCount);
			HLIB_STD_StrUtf8NCpy(stCopy.szDestUrl, param->szDestUrl, APKD_META_MEDIALIST_MAX_URL);
			for(idx = 0; idx < stCopy.ulParamCount; idx++)
			{
				HxLOG_Print("[%s:%d] filename (%s) target dir (%s)\n",__FUNCTION__,__LINE__, param->paramList[idx].szURL, param->szDestUrl);
				HLIB_STD_StrUtf8NCpy(stCopy.paramList[idx].szURL, param->paramList[idx].szURL, APKD_META_MEDIALIST_MAX_URL);
			}
			hErr = MEDIA_FS_RequestCopy(&stCopy);
			HLIB_STD_MemFree(stCopy.paramList);
			HLIB_STD_MemFree(param->paramList);

			return ERR_OK;
		}
	}

	return ERR_FAIL;
}


int		OPL_HMX_FileCancel(FILEOpParam_t *param)
{
	//printf("[OPL_HMX_FileCancel] begin\n");
	if(param)
	{
		MEDIA_FS_RequestCancel();
		return ERR_OK;
	}

	return ERR_FAIL;
}


int		OPL_HMX_MakeDir(FILEOpParam_t *param)
{
	//printf("[OPL_HMX_MakeDir] begin\n");
	if(param)
	{
		return MEDIA_FS_MakeDir(param->szDestUrl);
	}

	return ERR_FAIL;
}


int		OPL_HMX_AutoCopy(FILEOpParam_t *param)
{
	HERROR							hErr		= ERR_FAIL;
	int								retval		= 0;

	//printf("[OPL_HMX_AutoCopy] begin\n");
	if(param && param->paramList)
	{
		hErr = MEDIA_FS_StartAutoCopy(param, &retval);
		HLIB_STD_MemFree(param->paramList);

		return ERR_OK;
	}

	return ERR_FAIL;
}


int		OPL_HMX_IsFileExist( const char *szURL, int *retval )
{
	if(szURL == NULL || retval == NULL)
	{
		return ERR_FAIL;
	}
	*retval = MEDIA_FS_IsFileExist(szURL);

	return ERR_OK;
}


int		OPL_HMX_IsDirExist( const char *szURL, int *retval )
{
	if(szURL == NULL || retval == NULL)
	{
		return ERR_FAIL;
	}
	*retval = MEDIA_FS_IsDirExist(szURL);

	return ERR_OK;
}


int		OPL_HMX_GetVideoThumbnailUrl( const char *szURL, char *szVideoInfo)
{
	if(szURL == NULL || szVideoInfo == NULL)
	{
		return ERR_FAIL;
	}
	MEDIA_FS_GetVideoThumbnailUrl(szURL, szVideoInfo);

	return ERR_OK;
}


int		OPL_HMX_GetVideoDuration( const char *szURL, char *szVideoInfo)
{
	if(szURL == NULL || szVideoInfo == NULL)
	{
		return ERR_FAIL;
	}
	MEDIA_FS_GetVideoDuration(szURL, szVideoInfo);

	return ERR_OK;
}


int		OPL_HMX_GetVideoPlayPosition( const char *szURL, char *szVideoInfo)
{
	if(szURL == NULL || szVideoInfo == NULL)
	{
		return ERR_FAIL;
	}
	MEDIA_FS_GetVideoPlayPosition(szURL, szVideoInfo);

	return ERR_OK;
}


int		OPL_HMX_GetVideoWatched( const char *szURL, char *szVideoInfo)
{
	if(szURL == NULL || szVideoInfo == NULL)
	{
		return ERR_FAIL;
	}
	MEDIA_FS_GetVideoWatched(szURL, szVideoInfo);

	return ERR_OK;
}


int		OPL_HMX_GetSpaceInfo( const char *szURL, char *szVideoInfo)
{
	if(szURL == NULL || szVideoInfo == NULL)
	{
		return ERR_FAIL;
	}
	MEDIA_FS_EstimateSpace(szURL, szVideoInfo);

	return ERR_OK;
}


int		OPL_HMX_IsCopying(int *retval)
{
	int bCopying = 0;

	if(retval == NULL)
	{
		return ERR_FAIL;
	}

	bCopying = MEDIA_FS_IsCopying();
	*retval = bCopying;

	return ERR_OK;
}


int		OPL_HMX_IsDeleting(int *retval)
{
	int bDeleting = 0;

	if(retval == NULL)
	{
		return ERR_FAIL;
	}

	bDeleting = MEDIA_FS_IsDeleting();
	*retval = bDeleting;

	return ERR_OK;
}




