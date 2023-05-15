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
 * @file	  		nx_core_filemgr_file.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_CORE_FILEMGR_FILE_H__
#define __NX_CORE_FILEMGR_FILE_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/
#include <ondk.h>
#include <hapi.h>
#include <papi.h>
#include <thapi.h>
#include <oapi.h>
#include <apk.h>

#include <nx_common.h>
#include <nx_port.h>

/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/

/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/
#define NX_THUMBNAIL_EXTRACT_TIME	7 /*sec*/
/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/
typedef enum
{
	eFileType_VIDEO,
	eFileType_MUSIC,
	eFileType_PHOTO,
	eFileType_DIR,
	eFileType_UNKNOWN,
}NxFileType_e;

typedef enum
{
	eFileOperation_Idle,
	eFileOperation_Deleting,
	eFileOperation_Copying,
	eFileOperation_Moving,
	eFileOperation_Stopping
}NxFileOperation_e;

typedef struct
{
	HINT32		nStreamType;
	HINT32		nDuration;
	HINT32		nWidth;
	HINT32		nHeight;
	HINT64		lFrameCount;
	HCHAR		szCodecName[32];
}NxFileAV_t;

typedef struct
{
	HINT32		nWidth;
	HINT32		nHeight;
	HUINT32		ulPitch;
	HUINT8		ucFormat;
	HUINT8		ucBitPerPixel;
}NxFilePNG_t;

typedef struct
{
	HCHAR	szRepFile[DLIB_FILEINFO_REPNAME_LEN]; /* not path, file name */
	HBOOL	bHasSubFile;
}NxFileDir_t;

typedef struct
{
	HCHAR	szFullPath[DLIB_FILEINFO_NAME_LEN];
	HCHAR	szFileName[DLIB_FILEINFO_ONEDEPTH_LEN];
	HCHAR	szDate[32];
	HCHAR	szThumbnailUrl[DLIB_FILEINFO_REPNAME_LEN];
	HCHAR	szLargeThumbnailUrl[DLIB_FILEINFO_REPNAME_LEN];

	union
	{
		NxFileAV_t		av;
		NxFilePNG_t		png;
		NxFileDir_t		dir;
	}files;

}NxFileInfo_t;

typedef struct
{
	HUINT32		ulOperationId;
	HUINT32		ulRequestId;
	HUINT32		ulRequestCount;
	NxFileOperation_e	eFileOperationState;
}NX_FileContext_t;

/*******************************************************************/
/********************	Private Variables		********************/
/*******************************************************************/

/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/
NxFileInfo_t *NX_FILEMGR_FILE_MakeNxFileInfo(DxFileInfo_t *pstFileInfo, HCHAR *pszFilter);
HCHAR 	*NX_FILEMGR_FILE_ExtractNameFromPath(HCHAR *szPath);
HCHAR 	*NX_FILEMGR_FILE_GetThumbnail(HCHAR *szUrl, HUINT32 ulExtractPosition, const HCHAR *pszMountPath, HBOOL bRetExpectURL);
NxFileOperation_e	NX_FILEMGR_FILE_GetFileOperationStatus(void);
HERROR	NX_FILEMGR_FILE_CreateFileOperation(HCHAR *pszFilter);
HERROR	NX_FILEMGR_FILE_AddPath(HCHAR *pszFilePath);
HERROR	NX_FILEMGR_FILE_RemovePath(HCHAR *pszFilePath);
HERROR	NX_FILEMGR_FILE_RemoveFileGroup(void);
HERROR	NX_FILEMGR_FILE_CopyFileGroup(HCHAR *pszPath);
HERROR	NX_FILEMGR_FILE_MoveFileGroup(HCHAR *pszPath);
HERROR	NX_FILEMGR_FILE_CancelOperation(void);

#endif // __NX_CORE_FILEMGR_FILE_H__




