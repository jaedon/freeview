/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * @file NativeHmxMediaScan.h
 *
 * Copyright (C) 2011 HUMAX. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __NATIVE_HMX_MEDIASCAN_
#define __NATIVE_HMX_MEDIASCAN_

#include "NativeOOIFTypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
   \defgroup Configuration Configuration and Settings
   @{

   This is the native interface for the Configuration and setting APIs
   as defined in DAE 7.3.
 */

#if defined OIPF

typedef void *MediaContentHandle;

typedef void (*scanListener)(void **id, int size, unsigned int state);
typedef void (*scanDirListener)(void **id, int size, unsigned int state);
typedef void (*fileOpListener)(unsigned int type, int nDevIndex, unsigned int state);


/*------------------------------------------
  HMX_MediaScan
  ------------------------------------------*/
OOIFReturnCode native_HMX_MediaScanningResultListener(scanListener listener);
OOIFReturnCode native_HMX_MediaBrowse(const char *szURL, int nSortOrder, int nPageNum, const char *szFileExts, int *retval );

OOIFReturnCode native_HMX_DirScanningResultListener(scanDirListener listener);
OOIFReturnCode native_HMX_DirBrowse(const char *szURL, int nSortOrder, int nPageNum, const char *szFileExts, int *retval );

OOIFReturnCode native_HMX_IsFileExist(const char *szURL, int *retval );
OOIFReturnCode native_HMX_IsDirExist(const char *szURL, int *retval );

OOIFReturnCode native_HMX_GetVideoThumbnailUrl(const char *szURL, const char **retval );
OOIFReturnCode native_HMX_GetVideoDuration(const char *szURL, const char **retval );
OOIFReturnCode native_HMX_GetVideoPlayPosition(const char *szURL, const char **retval );
OOIFReturnCode native_HMX_GetVideoWatched(const char *szURL, const char **retval );
OOIFReturnCode native_HMX_GetSpaceInfo(const char *szURL, const char **retval );

OOIFReturnCode native_HMX_IsCopying(int *retval );
OOIFReturnCode native_HMX_IsDeleting(int *retval );

/*------------------------------------------
  HMX_MediaContent
  ------------------------------------------*/

OOIFReturnCode native_HMX_MediaContentReleaseHandle(MediaContentHandle id);
OOIFReturnCode native_HMX_MediaContentGetFileType(MediaContentHandle identifier, OOIFNumber *retval);
OOIFReturnCode native_HMX_MediaContentGetFullPath(MediaContentHandle identifier, const char **retval);
OOIFReturnCode native_HMX_MediaContentGetHasSubitem(MediaContentHandle identifier, OOIFNumber *retval);
OOIFReturnCode native_HMX_MediaContentGetDate(MediaContentHandle identifier, const char **retval);
OOIFReturnCode native_HMX_MediaContentGetDuration(MediaContentHandle identifier, OOIFNumber *retval);
OOIFReturnCode native_HMX_MediaContentGetPlayPosition(MediaContentHandle identifier, OOIFNumber *retval);
OOIFReturnCode native_HMX_MediaContentGetLargeThumbnailUrl(MediaContentHandle identifier, const char **retval);
OOIFReturnCode native_HMX_MediaContentGetThumbnailUrl(MediaContentHandle identifier, const char **retval);
OOIFReturnCode native_HMX_MediaContentGetPicureResolution(MediaContentHandle identifier, const char **retval);
OOIFReturnCode native_HMX_MediaContentGetThumbResolution(MediaContentHandle identifier, const char **retval);
OOIFReturnCode native_HMX_MediaContentGetPictureDate(MediaContentHandle identifier, const char **retval);
OOIFReturnCode native_HMX_MediaContentGetMusicAlbum(MediaContentHandle identifier, const char **retval);
OOIFReturnCode native_HMX_MediaContentGetMusicArtist(MediaContentHandle identifier, const char **retval);
OOIFReturnCode native_HMX_MediaContentGetWatched(MediaContentHandle identifier, OOIFNumber *retval);


OOIFReturnCode native_HMX_FileOpResultListener(fileOpListener listener);
OOIFReturnCode native_HMX_FileOpDo(int nType, int nIsSyncMode, int nParamCount, const char *szURLs, const char *szRename, const char *szDestUrl, int *retval);



#endif /* OIPF */


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_HMX_MEDIASCAN_ */

