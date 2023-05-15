/**************************************************************************************/
/**
 * @file oplmediascan.h
 * *
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#ifndef __OPLMEDIASCAN_H__
#define __OPLMEDIASCAN_H__

#include "bpltype.h"

#define OPL_MAX_PATH_STR_LEN	512
#define OPL_MAX_TITLE_STR_LEN	180
#define OPL_MAX_SMALL_STR_LEN	32

typedef struct{
	char	szFullPath[OPL_MAX_PATH_STR_LEN];
	char	szFileFilters[OPL_MAX_TITLE_STR_LEN];
	int		nSortOrder;
	int		nPageNum;
}MediaParam_t;

typedef	struct
{
	char	szURL[OPL_MAX_PATH_STR_LEN];
} URL_PARAM_t;


typedef struct{
	int		IsSyncMode;
	int		ulParamCount;
	URL_PARAM_t *paramList;
	char	szRename[OPL_MAX_PATH_STR_LEN];
	char	szDestUrl[OPL_MAX_PATH_STR_LEN];
} FILEOpParam_t;



typedef struct{
	int		m_nFileType;
	char	m_szFullPath[OPL_MAX_PATH_STR_LEN];
	int		m_nHasSubitem;
	char	m_szSDate[OPL_MAX_SMALL_STR_LEN];
	int		m_nDuration;
	int		m_nPlayPosition;
	char	m_szLargeThumbnailUrl[OPL_MAX_PATH_STR_LEN];
	char	m_szThumbnailUrl[OPL_MAX_PATH_STR_LEN];
	char	m_szResolution[OPL_MAX_SMALL_STR_LEN];
	char	m_szThumbResolution[OPL_MAX_SMALL_STR_LEN];
	char	m_szPictureDate[OPL_MAX_SMALL_STR_LEN];
	char	m_szAlbum[OPL_MAX_TITLE_STR_LEN];
	char	m_szArtist[OPL_MAX_TITLE_STR_LEN];
	int		m_nWatched;
}MediaContent_t;

typedef void (*scanEventCallback)(void **id, int size, unsigned int state);
typedef void (*scanDirEventCallback)(void **id, int size, unsigned int state);
typedef void (*fileOpEventCallback)(unsigned int type, int nDevIndex, unsigned int state);

#if defined(CONFIG_PROD_CPU_BCM_ARM)
void	OPL_HMX_MediaScanningResultListener( scanEventCallback listener );
int		OPL_HMX_MediaBrowse( void *param, int *retval );

void	OPL_HMX_DirScanningResultListener( scanDirEventCallback listener );
int		OPL_HMX_DirBrowse( void *param, int *retval );

int		OPL_HMX_IsFileExist( const char *szURL, int *retval );
int		OPL_HMX_IsDirExist( const char *szURL, int *retval );

int		OPL_HMX_GetVideoThumbnailUrl( const char *szURL, char *s_szVideoInfo);
int		OPL_HMX_GetVideoDuration( const char *szURL, char *szVideoInfo);
int		OPL_HMX_GetVideoPlayPosition( const char *szURL, char *szVideoInfo);
int		OPL_HMX_GetVideoWatched( const char *szURL, char *szVideoInfo);
int		OPL_HMX_GetSpaceInfo( const char *szURL, char *szVideoInfo);
int		OPL_HMX_IsCopying(int *retval);
int		OPL_HMX_IsDeleting(int *retval);

void	OPL_HMX_FileOpResultListener( fileOpEventCallback listener );
int		OPL_HMX_FileDelete(FILEOpParam_t *param);
int		OPL_HMX_FileRename(FILEOpParam_t *param);
int		OPL_HMX_FileCopy(FILEOpParam_t *param);
int		OPL_HMX_FileCancel(FILEOpParam_t *param);
int		OPL_HMX_MakeDir(FILEOpParam_t *param);
int		OPL_HMX_AutoCopy(FILEOpParam_t *param);
#else
void	OPL_HMX_MediaScanningResultListener( scanEventCallback listener ) {}
int		OPL_HMX_MediaBrowse( void *param, int *retval ) { return 0; }

void	OPL_HMX_DirScanningResultListener( scanDirEventCallback listener ) {}
int		OPL_HMX_DirBrowse( void *param, int *retval ) { return 0; }

int		OPL_HMX_IsFileExist( const char *szURL, int *retval ) { return ERR_OK; }
int		OPL_HMX_IsDirExist( const char *szURL, int *retval ) { return ERR_OK; }

int		OPL_HMX_GetVideoThumbnailUrl( const char *szURL, char *s_szVideoInfo) { return ERR_OK; }
int		OPL_HMX_GetVideoDuration( const char *szURL, char *szVideoInfo) { return ERR_OK; }
int		OPL_HMX_GetVideoPlayPosition( const char *szURL, char *szVideoInfo) { return ERR_OK; }
int		OPL_HMX_GetVideoWatched( const char *szURL, char *szVideoInfo) { return ERR_OK; }
int		OPL_HMX_GetSpaceInfo( const char *szURL, char *szVideoInfo) { return ERR_OK; }
int		OPL_HMX_IsCopying(int *retval) { return ERR_OK; }
int		OPL_HMX_IsDeleting(int *retval) { return ERR_OK; }

void	OPL_HMX_FileOpResultListener( fileOpEventCallback listener ) {}
int		OPL_HMX_FileDelete(FILEOpParam_t *param) { return ERR_OK; }
int		OPL_HMX_FileRename(FILEOpParam_t *param) { return ERR_OK; }
int		OPL_HMX_FileCopy(FILEOpParam_t *param) { return ERR_OK; }
int		OPL_HMX_FileCancel(FILEOpParam_t *param) { return ERR_OK; }
int		OPL_HMX_MakeDir(FILEOpParam_t *param) { return ERR_OK; }
int		OPL_HMX_AutoCopy(FILEOpParam_t *param) { return ERR_OK; }
#endif // CONFIG_PROD_CPU_BCM_ARM

#endif // __OPLMEDIASCAN_H__

