 /*************************************************************************************************************
	File 		: oplfilemgr.cpp
	author 		: WEB TFT tft_webapp@humaxdigital.com
	comment		:
	date    	: 2012/10/23
	attention 	:

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (¡°Humax¡±) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*************************************************************************************************************/

#include "oplfilemgr.h"
#include <uapi.h>
#include <thapi.h>

#include <iostream>

#include <time.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <dlib.h>
#include <hlib.h>

#include <apk.h>

using namespace std;

#define DOT_OR_DOTDOT(_basename)					\
	((_basename[0] == '.') &&					\
	 (_basename[1] == '\0' || (_basename[1] == '.' && _basename[2] == '\0')))

#define ENTRY					HxLOG_Debug("%s : %s : +++++++++++++++++++++++++++++\n", __FILE__ , __FUNCTION__);
#define LEAVE					HxLOG_Debug("%s : %s : -----------------------------\n", __FILE__ , __FUNCTION__);
#define SAFE_DELETE(p)			{if(p){ HLIB_STD_MemFree(p); (p)=NULL;}}
#define SAFE_DELETE_ARRAY(p) 	{if(p){ HLIB_STD_MemFree[](p); (p)=NULL;}}
#define OP_FASTER				true
#define THUMBNAIL_AUTO_PATH		NULL
#define THUMBNAIL_EXTRACT_TIME	7 /*sec*/
typedef unsigned long 			FILE_TAG;

const FILE_TAG 					FILE_MGR_TAG		= 0xfd00;
const FILE_TAG 					FILE_BRS_TAG		= 0xfd01;
const FILE_TAG					FILE_MNT_TAG		= 0xfd02;
const FILE_TAG					FILE_SCN_TAG		= 0xfd03;

HCHAR							g_szFileExtension[eFILE_MAX_EXT_LEN];
HxList_t 						*g_pstContentList 		= NULL;
OplFileManagerCallback			g_pOperationListener 	= NULL;
OplFileBrowserCallback			g_pBrowseListener	 	= NULL;
//static 	pthread_t				g_pBrowseThread;


typedef		HINT32	( *cmdFunc_t)(void *, void *);

typedef struct
{
	HCHAR szType[16];
	HUINT32 ulValue;
} FILEMGR_SUBTYPE_t;

FILEMGR_SUBTYPE_t s_subtype_table[] = {
	{ "folder",	AxFILEMGR_BROWSE_DIR	},
	{ "file",	AxFILEMGR_BROWSE_FILE	},
	{ "video",	AxFILEMGR_BROWSE_VIDEO	},
	{ "music",	AxFILEMGR_BROWSE_MUSIC	},
	{ "photo",	AxFILEMGR_BROWSE_PHOTO	},
	{ "", 0 }
};


/****************************************************************************************
 * Global Util Function
 ***************************************************************************************/

HBOOL IsExist(const char* path)
{
	HBOOL bRet = FALSE;
	struct stat st;
	int statRet = 0;

	statRet = lstat(path,&st);
	if(statRet < 0)
	{
		if(errno != ENOENT)
		{
			bRet = TRUE;
		}
	}
	else
	{
		bRet = TRUE;
	}

	return bRet;
}

HBOOL isDirectory(const char *path)
{
	ENTRY;
	HBOOL 	bRet = false;
	struct 	stat fstat;

	if(0 != lstat(path, &fstat))
	{
	}
	else
	{
		if (S_ISDIR(fstat.st_mode))
		{
			bRet = true;
		}
	}
	return bRet;
}

HBOOL HasFile(const char *path)
{
	ENTRY;
	HBOOL bRet = false;
	struct dirent *dirent;
	struct 	stat fstat;
	char szBuf[eFILE_MAX_PATH_LEN] = {0};
	DIR *dp = opendir(path);
	int ret;

	HCHAR *pExt = NULL;
	if (NULL != dp)
	{
		while ((dirent = readdir(dp)) != NULL) {
			if (DOT_OR_DOTDOT(dirent->d_name))
			{
				continue;
			}
			HxSTD_snprintf(szBuf, sizeof(szBuf)-1, "%s/%s", path, dirent->d_name);
			ret = lstat( szBuf, &fstat );
			if(0 != ret)
			{
				HxLOG_Error("cannot stat \"%s\": %s (%d)\n", szBuf, strerror(errno), errno);
				continue;
			}
			if(0 < HxSTD_StrLen(dirent->d_name) && !S_ISDIR(fstat.st_mode))
			{
				pExt = HxSTD_StrRChar(szBuf, '.');
				if( NULL == pExt || NULL == pExt+1) continue;
				if( NULL != HLIB_STD_StrCaseStr(g_szFileExtension, pExt+1) )
				{
					bRet = true;
					break;
				}
				pExt = NULL;
			}
		}
		closedir(dp);
	}

	return bRet;
}

/****************************************************************************************
 * OplBrowsedContent
 ***************************************************************************************/

OplBrowsedContent::OplBrowsedContent(OplBrowsedContentHandle Handle)
{
	DxFileInfo_t	*pstFsInfo = (DxFileInfo_t*)Handle;
	ENTRY;

	if(NULL != pstFsInfo)
	{
		init_Content_info();
		set_Common_Info(pstFsInfo);
		set_Content_Type(pstFsInfo);

		switch ( pstFsInfo->eDataType )
		{
			case eDxFILETYPE_MUSIC :
				set_MP3_Info(pstFsInfo);
				break;
			case eDxFILETYPE_EXIF :
				set_JPEG_Info(pstFsInfo);
				break;
			case eDxFILETYPE_PNG :
				set_PNG_Info(pstFsInfo);
				break;
			case eDxFILETYPE_RECORDINGS :
				set_RECORD_Info(pstFsInfo);
				break;
			case eDxFILETYPE_AVSTREAM :
				set_AV_Info(pstFsInfo);
				break;
			case eDxFILETYPE_DIRECTORY :
				set_DIR_Info(pstFsInfo);
				break;
			default :
				HxLOG_Warning("OplBrowsedContent UnKnownType[%d]. path : %s\n",pstFsInfo->eDataType, pstFsInfo->szFileName);
				break;
		}

		set_ThumbnailUrl_Info(m_nContentType);
	}

	print_Content_Info();

	LEAVE;
}

OplBrowsedContent::OplBrowsedContent( const OplBrowsedContent &original )
{
	HxLOG_Debug("OplBrowsedContent( const OplBrowsedContent &original \n");
	m_nContentType 						= original.m_nContentType;
	m_nFileType							= original.m_nFileType;
	m_nDuration							= original.m_nDuration;
	m_nLastViewTime						= original.m_nLastViewTime;
	m_nWatched 							= original.m_nWatched;
	m_nStorageCategory					= original.m_nStorageCategory;
	m_nSize								= original.m_nSize;
	m_bHasSubItem						= original.m_bHasSubItem;
	m_nMP3_Bitrate	 					= original.m_nMP3_Bitrate;
	m_nMP3_SampleRate	 				= original.m_nMP3_SampleRate;

	HLIB_STD_StrNCpySafe( m_szName, 			original.m_szName, 				sizeof(m_szName));
	HLIB_STD_StrNCpySafe( m_szFullPath, 		original.m_szFullPath, 			sizeof(m_szFullPath));
	HLIB_STD_StrNCpySafe( m_szLargeThumbnailUrl,	original.m_szLargeThumbnailUrl,	sizeof(m_szLargeThumbnailUrl));
	HLIB_STD_StrNCpySafe( m_szThumbnailUrl, 	original.m_szThumbnailUrl, 		sizeof(m_szThumbnailUrl));
	HLIB_STD_StrNCpySafe( m_szDate, 			original.m_szDate, 				sizeof(m_szDate));
	HLIB_STD_StrNCpySafe( m_szResolution, 		original.m_szResolution, 		sizeof(m_szResolution));
	HLIB_STD_StrNCpySafe( m_szThumbResolution, original.m_szThumbResolution, 	sizeof(m_szThumbResolution));
	HLIB_STD_StrNCpySafe( m_szPictureDate, 	original.m_szPictureDate, 		sizeof(m_szPictureDate));
	HLIB_STD_StrNCpySafe( m_szAlbum, 			original.m_szAlbum, 			sizeof(m_szAlbum));
	HLIB_STD_StrNCpySafe( m_szArtist, 			original.m_szArtist, 			sizeof(m_szArtist));
	HLIB_STD_StrNCpySafe( m_szTitle, 			original.m_szTitle, 			sizeof(m_szTitle));
	LEAVE;
}
OplBrowsedContent::~OplBrowsedContent()
{
	ENTRY;
	//1 ### release allocated resource

	LEAVE;
}

void OplBrowsedContent::print_Content_Info()
{
	ENTRY;
	HxLOG_Debug("contentType        : %s \n", print_Content_Type(m_nContentType));
	HxLOG_Debug("fileType           : %s \n", print_File_Type(m_nFileType));
	HxLOG_Debug("fullPath           : %s \n", m_szFullPath);
	HxLOG_Debug("name               : %s \n", m_szName);
	HxLOG_Debug("size               : %llu \n", m_nSize);
	HxLOG_Debug("date               : %s \n", m_szDate);
	HxLOG_Debug("duration           : %d \n", m_nDuration);
	HxLOG_Debug("lastViewTime       : %d \n", m_nLastViewTime);
	HxLOG_Debug("largeThumbnailUrl  : %s \n", m_szLargeThumbnailUrl);
	HxLOG_Debug("thumbnailUrl       : %s \n", m_szThumbnailUrl);
	HxLOG_Debug("resolution         : %s \n", m_szResolution);
	HxLOG_Debug("thumbResolution    : %s \n", m_szThumbResolution);
	HxLOG_Debug("pictureDate        : %s \n", m_szPictureDate);
	HxLOG_Debug("title              : %s \n", m_szTitle);
	HxLOG_Debug("genre              : %s \n", m_szGenre);
	HxLOG_Debug("album              : %s \n", m_szAlbum);
	HxLOG_Debug("artist             : %s \n", m_szArtist);
	HxLOG_Debug("watched            : %d \n", m_nWatched);
	HxLOG_Debug("storageCategory    : %d \n", m_nStorageCategory);
	LEAVE;
}

const HCHAR* OplBrowsedContent::print_Content_Type( int status )
{
	switch (status)
	{
		ENUM_TO_STR(eMUSIC_TYPE);
		ENUM_TO_STR(ePHOTO_TYPE);
		ENUM_TO_STR(eVIDEO_TYPE);
		default:
			break;
	}
	return ( "Unknown ContentType" );
}

const HCHAR* OplBrowsedContent::print_File_Type( int status )
{
	switch (status)
	{
		ENUM_TO_STR(eFOLDER_TYPE);
		ENUM_TO_STR(eFILE_TYPE);
		default:
			break;
	}
	return ( "Unknown FileType" );
}

void OplBrowsedContent::init_Content_info()
{
	m_nContentType 						= eUNKNOWN_MEDIA_TYPE;
	m_nFileType							= eUNKNOWN_FILE_TYPE;
	m_nSize								= 0;
	m_nDuration							= 0;
	m_nLastViewTime						= 0;
	m_nWatched 							= eCONTENT_NOT_WATCHED;
	m_nStorageCategory					= eDxINTERFACE_UNKNOWN;
	m_bHasSubItem						= FALSE;
	m_nMP3_Bitrate						= 0;
	m_nMP3_SampleRate					= 0;
	m_bExistThumb						= FALSE;

	HxSTD_MemSet( m_szFullPath, 		0x00, 	sizeof(m_szFullPath));
	HxSTD_MemSet( m_szName, 			0x00, 	sizeof(m_szName));
	HxSTD_MemSet( m_szLargeThumbnailUrl, 	0x00, 	sizeof(m_szLargeThumbnailUrl));
	HxSTD_MemSet( m_szThumbnailUrl, 	0x00, 	sizeof(m_szThumbnailUrl));
	HxSTD_MemSet( m_szDate, 			0x00, 	sizeof(m_szDate));
	HxSTD_MemSet( m_szResolution, 		0x00, 	sizeof(m_szResolution));
	HxSTD_MemSet( m_szThumbResolution,	0x00, 	sizeof(m_szThumbResolution));
	HxSTD_MemSet( m_szPictureDate, 		0x00,	sizeof(m_szPictureDate));
	HxSTD_MemSet( m_szAlbum, 			0x00,	sizeof(m_szAlbum));
	HxSTD_MemSet( m_szGenre, 			0x00, 	sizeof(m_szGenre));
	HxSTD_MemSet( m_szArtist, 			0x00,	sizeof(m_szArtist));
	HxSTD_MemSet( m_szTitle, 			0x00,	sizeof(m_szTitle));

	HxSTD_MemSet( m_szAV_CodecName, 	0x00, 	sizeof(m_szAV_CodecName));
	HxSTD_MemSet( m_szJPG_PictureDate, 	0x00, 	sizeof(m_szJPG_PictureDate));
	HxSTD_MemSet( m_szRecFile_ThumbnailPath, 0x00, sizeof(m_szRecFile_ThumbnailPath));


}
HBOOL OplBrowsedContent::get_SubFile(const char *path, char *pszFile)
{
	ENTRY;
	HBOOL bRet = false;
	struct dirent *dirent;
	struct 	stat fstat;
	char szBuf[eFILE_MAX_PATH_LEN] = {0};
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
			if( NULL == HLIB_STD_StrCaseStr(g_szFileExtension, pExt+1)) continue;

			if(0 != lstat( szBuf, &fstat)) break;
			if(0 < HxSTD_StrLen(dirent->d_name) && !S_ISDIR(fstat.st_mode))
			{
				bRet = true;
				//1 ### assign file to extract thumbnail
				HxSTD_PrintToStrN(pszFile, eFILE_MAX_URL_LEN, "%s", szBuf);
				break;
			}
			pExt = NULL;
		}
		closedir(dp);
	}

	return bRet;
}

HCHAR * OplBrowsedContent::GetThumbnailUrl()
{
	if (!m_bExistThumb && HxSTD_StrLen(m_szThumbnailUrl))
	{
		if (FALSE == HLIB_FILE_Exist((const HUINT8 *)(m_szThumbnailUrl + HxSTD_StrLen("file://"))) )
		{
			return (HCHAR*)"";
		}
		m_bExistThumb = TRUE;
	}
	return m_szThumbnailUrl;
}

void OplBrowsedContent::set_Content_Type(void *pstFsInfo)
{
	DxFileInfo_t	*pstFsInfoTmp = (DxFileInfo_t*)pstFsInfo;
	if(eFILE_TYPE == m_nFileType)
	{
		switch ( pstFsInfoTmp->eDataType )
		{
			case eDxFILETYPE_MUSIC :
				m_nContentType = eMUSIC_TYPE;
				break;
			case eDxFILETYPE_EXIF :
				m_nContentType = ePHOTO_TYPE;
				break;
			case eDxFILETYPE_PNG :
				m_nContentType = ePHOTO_TYPE;
				break;
			case eDxFILETYPE_RECORDINGS :
				m_nContentType = eVIDEO_TYPE;
				break;
			case eDxFILETYPE_AVSTREAM :
				m_nContentType = eVIDEO_TYPE;
				break;
			default :
				m_nContentType = eUNKNOWN_MEDIA_TYPE;
				break;
		}
	}
	else
	{
		m_nContentType = eUNKNOWN_MEDIA_TYPE;
	}
}

void OplBrowsedContent::init_ThumbnailPath(HCHAR *pszPath)
{
	switch(m_nFileType)
	{
		case eFILE_TYPE:
			{
				HxSTD_PrintToStrN(pszPath, eFILE_MAX_PATH_LEN, "%s", m_szFullPath);
			}
			break;
		case eFOLDER_TYPE:
			{
				/* dirItem use temp buffer : m_szLargeThumbnailUrl */
				HxSTD_PrintToStrN(pszPath, eFILE_MAX_PATH_LEN, "%s", m_szLargeThumbnailUrl);
			}
			break;
	}
}

void OplBrowsedContent::set_Common_Info(void *pstFsInfo)
{
	DxFileInfo_t *pstFsInfoTmp = (DxFileInfo_t	*)pstFsInfo;
	time_t	mtime;
	struct tm	*now;

	ENTRY;
	m_nFileType = (pstFsInfoTmp->st_stat.st_mode == eDxISDIR) ? eFOLDER_TYPE : eFILE_TYPE;
	m_nStorageCategory = pstFsInfoTmp->eInterface;

	if ( eFILE_TYPE == m_nFileType )
		m_nSize = pstFsInfoTmp->st_stat.st_size;

	HxSTD_MemSet(m_szFullPath,0x0,eFILE_MAX_PATH_LEN);
	HxSTD_MemSet(m_szName,0x0,eFILE_MAX_PATH_LEN);
	HxSTD_MemSet(m_szDate,0x0,eFILE_MAX_DATE_LEN);

	HLIB_STD_StrNCpySafe(m_szFullPath,	pstFsInfoTmp->szFileName,		sizeof(m_szFullPath));
	HLIB_STD_StrNCpySafe(m_szName,		pstFsInfoTmp->szOnedepth,		sizeof(m_szName));
	{
		mtime = pstFsInfoTmp->st_stat.ullMTime;
		now = localtime(&mtime);
		strftime(m_szDate, eFILE_MAX_DATE_LEN, "%H:%M %d.%m.%Y", now);
	}
//	HLIB_STD_StrNCpySafe(m_szDate,		pstFsInfoTmp->st_stat.szMtime,	sizeof(m_szDate));
}

void OplBrowsedContent::set_AV_Info(void *pstFsInfo)
{
	DxFileInfo_t	*pstFsInfoTmp = (DxFileInfo_t	*)pstFsInfo;
	HxAV_SteamInfo_t *pstAVInfo = (HxAV_SteamInfo_t*)pstFsInfoTmp->data;
	ENTRY;

	if ((NULL != pstAVInfo) && (pstFsInfoTmp->size))
	{
		m_nAV_StreamType		= pstAVInfo->nStreamType;
		HxLOG_Debug("[%s] duration from appkit : %d \n", __FUNCTION__, pstAVInfo->nDuration);
		m_nAV_Duration			= static_cast<HINT32>(pstAVInfo->nDuration);
		m_lAV_FrameCount		= pstAVInfo->llFrameCount;
		m_nAV_Width 			= pstAVInfo->nWidth;
		m_nAV_Height			= pstAVInfo->nHeight;

		HxSTD_MemSet(m_szAV_CodecName, 0x00, 32);
		HxSTD_PrintToStrN(m_szAV_CodecName, 32, "%s", pstAVInfo->szCodecName);

		m_nDuration 	= m_nAV_Duration;
		HxSTD_MemSet(m_szResolution, 0x00, eFILE_MAX_RESOLUTION_LEN);
		HxSTD_PrintToStrN(m_szResolution, eFILE_MAX_RESOLUTION_LEN, "%dx%d", pstAVInfo->nWidth, pstAVInfo->nHeight);
	}
}

void OplBrowsedContent::set_MP3_Info(void *pstFsInfo)
{
	DxFileInfo_t	*pstFsInfoTmp = (DxFileInfo_t	*)pstFsInfo;
	HxID3_MusicInfo_t *pstMusicInfo = (HxID3_MusicInfo_t*)pstFsInfoTmp->data;
	ENTRY;

	if ((NULL != pstMusicInfo) && (pstFsInfoTmp->size > 0))
	{
		m_nDuration 	= static_cast<HINT32>(pstMusicInfo->ulDuration);
		m_nMP3_Bitrate 		= pstMusicInfo->nBitRate;
		m_nMP3_SampleRate 	= pstMusicInfo->nSampleRate;

		HxSTD_PrintToStrN(m_szAlbum, 	sizeof(m_szAlbum), 	"%s", pstMusicInfo->szAlbum);
		HxSTD_PrintToStrN(m_szArtist, 	sizeof(m_szArtist), "%s", pstMusicInfo->szArtist);
		HxSTD_PrintToStrN(m_szTitle, 	sizeof(m_szTitle), 	"%s", pstMusicInfo->szTitle);
		HxSTD_PrintToStrN(m_szGenre, 	sizeof(m_szGenre),  "%s",  pstMusicInfo->szGenre);
		//HxLOG_Debug("==>Music Title : %s, Artist : %s, Album : %s\n", pstMusicInfo->szTitle, pstMusicInfo->szArtist, pstMusicInfo->szAlbum);
	}
}
void OplBrowsedContent::set_PNG_Info(void	*pstFsInfo)
{
	DxFileInfo_t	*pstFsInfoTmp = (DxFileInfo_t	*)pstFsInfo;
	HxPNG_Info_t *pstPNGInfo = (HxPNG_Info_t*)pstFsInfoTmp->data;
	ENTRY;

	if ((NULL != pstPNGInfo) && (pstFsInfoTmp->size > 0))
	{
		(void)pstPNGInfo->ulPitch;
		(void)pstPNGInfo->ucFormat;
		(void)pstPNGInfo->ucBitPerPixel;

		HxSTD_MemSet(m_szResolution, 0x00, sizeof(m_szResolution));
		HxSTD_PrintToStrN(m_szResolution, sizeof(m_szResolution), "%dx%d", pstPNGInfo->nWidth, pstPNGInfo->nHeight);
	}
}
void OplBrowsedContent::set_JPEG_Info(void	*pstFsInfo)
{
	DxFileInfo_t	*pstFsInfoTmp = (DxFileInfo_t	*)pstFsInfo;
	HxEXIF_Info_t 	*pstEXIFInfo = (HxEXIF_Info_t*)pstFsInfoTmp->data;
	ENTRY;

	if ((NULL != pstEXIFInfo) && (pstFsInfoTmp->size > 0))
	{
		HxSTD_MemSet(m_szResolution, 0x00, sizeof(m_szResolution));
		HxSTD_PrintToStrN(m_szResolution, sizeof(m_szResolution), "%dx%d", pstEXIFInfo->nWidth, pstEXIFInfo->nHeight);

		HxSTD_MemSet(m_szPictureDate, 0x00, sizeof(m_szPictureDate));
		HxSTD_PrintToStrN(m_szPictureDate,  sizeof(m_szPictureDate), "%s", pstEXIFInfo->szDate);
	}
}

#if defined(CONFIG_FILEINFO_IFACE_NO_RECORDINGS)
void OplBrowsedContent::set_RECORD_Info(void	*pstFsInfo)
{
	DxFileInfo_t	*pstFsInfoTmp = (DxFileInfo_t	*)pstFsInfo;
	HCHAR			*szUrl = (HCHAR *)pstFsInfoTmp->data;
	//DxRecListData_t *pstRecordInfo = (DxRecListData_t*)pstFsInfoTmp->data;

	ENTRY;

	if ((NULL != szUrl) && (pstFsInfoTmp->size > 0))
	{
		DxRecListData_t		*pstRecordInfo = APK_META_RECORD_GetByUrl(szUrl);

		if (NULL != pstRecordInfo)
		{
			m_nDuration 	= static_cast<HINT32>(pstRecordInfo->ulRecDuration);
			m_nLastViewTime = pstRecordInfo->ulLastViewTime;
			m_nWatched		= pstRecordInfo->bPlayed;
			HxSTD_MemSet(m_szThumbnailUrl, 0x00, sizeof(m_szThumbnailUrl));
			HxSTD_PrintToStrN(m_szThumbnailUrl, sizeof(m_szThumbnailUrl), "%s", pstRecordInfo->szThumbnailPath);

			APK_META_RECORD_Release(pstRecordInfo);
		}
	}
}
#else
void OplBrowsedContent::set_RECORD_Info(void	*pstFsInfo)
{
	DxFileInfo_t	*pstFsInfoTmp = (DxFileInfo_t	*)pstFsInfo;
	DxRecListData_t *pstRecordInfo = (DxRecListData_t*)pstFsInfoTmp->data;
	ENTRY;

	if ((NULL != pstRecordInfo) && (pstFsInfoTmp->size > 0))
	{
		m_nDuration 	= static_cast<HINT32>(pstRecordInfo->ulRecDuration);
		m_nLastViewTime = pstRecordInfo->ulLastViewTime;
		m_nWatched		= pstRecordInfo->bPlayed;
		HxSTD_MemSet(m_szThumbnailUrl, 0x00, sizeof(m_szThumbnailUrl));
		HxSTD_PrintToStrN(m_szThumbnailUrl, sizeof(m_szThumbnailUrl), "%s", pstRecordInfo->szThumbnailPath);
	}
}
#endif

void OplBrowsedContent::set_DIR_Info(void	*pstFsInfo)
{
	DxFileInfo_t	*pstFsInfoTmp = (DxFileInfo_t	*)pstFsInfo;
	DxDirInfo_t		*pstDirInfo = (DxDirInfo_t *)pstFsInfoTmp->data;
	ENTRY;

	if ( 0 < pstFsInfoTmp->size  )
	{
		/* dirItem use temp buffer : m_szLargeThumbnailUrl */
		HxSTD_PrintToStrN(m_szLargeThumbnailUrl, sizeof(m_szLargeThumbnailUrl), "%s", pstDirInfo->szRepFile);
		m_bHasSubItem = pstDirInfo->bHasSubFile ? TRUE : FALSE;
	}
	else
	{
		m_bHasSubItem = get_SubFile(m_szFullPath, m_szLargeThumbnailUrl);
	}
}
void OplBrowsedContent::set_ThumbnailUrl_Info(eOPL_FILE_MEDIATYPE_e type)
{
	HCHAR  szFilePathForThumb[eFILE_MAX_PATH_LEN];
	HINT32 extractPosition = 0;
	HCHAR *p = NULL;
	HCHAR *pLargeThumb = NULL;

	szFilePathForThumb[0] = 0;
	init_ThumbnailPath(szFilePathForThumb);

	switch(type)
	{
		case eVIDEO_TYPE:
			// position -> seconds
			extractPosition = ((0 < m_nDuration && m_nDuration < THUMBNAIL_EXTRACT_TIME) ? m_nDuration : THUMBNAIL_EXTRACT_TIME);
			//extractPosition = (m_nLastViewTime ==0 ||  m_nLastViewTime < 1000 ) ? m_nDuration : (HUINT32)(m_nLastViewTime / 1000);
			HxLOG_Debug("[%s:%d]m_nLastViewTime[%d]\n",__FUNCTION__,__LINE__,m_nLastViewTime);
			break;
		case eMUSIC_TYPE:
		case ePHOTO_TYPE:
			extractPosition = 0;
			break;
		case eUNKNOWN_MEDIA_TYPE:
		default:
			//1 ### folder and unknown file case
			extractPosition = THUMBNAIL_EXTRACT_TIME;
			break;
	}

	HxLOG_Debug("[%s] extractPosition -- %d sec \n", __FUNCTION__, extractPosition);
	p = (HCHAR*)THAPI_GetThumbnailURL((HCHAR*)szFilePathForThumb, extractPosition, (HCHAR*)THUMBNAIL_AUTO_PATH, TRUE);
	if(NULL != p)
	{
		HxSTD_MemSet(m_szThumbnailUrl, 0x00, sizeof(m_szThumbnailUrl));
		HxSTD_PrintToStrN(m_szThumbnailUrl, sizeof(m_szThumbnailUrl), "file://%s", p);

		if (ePHOTO_TYPE == type)
		{
			pLargeThumb = THAPI_GetLargeThumbnailUrl(p, 1024);
			if(NULL != pLargeThumb)
			{
				HxSTD_MemSet(m_szLargeThumbnailUrl, 0x00, sizeof(m_szLargeThumbnailUrl));
				HxSTD_PrintToStrN(m_szLargeThumbnailUrl, sizeof(m_szLargeThumbnailUrl), "%s", pLargeThumb);
			}
		}
	}
}

/****************************************************************************************
 * OplFileBrowser
 ***************************************************************************************/
OplFileBrowserCallback 	OplFileBrowser::m_callback = NULL;
eOplFileBrowserState 	OplFileBrowser::m_nCurrentState;
OplFileBrowser 			*OplFileBrowser::m_oplFileBrowserInstance;
char 					OplFileBrowser::m_szBrowsePath[];
int						OplFileBrowser::m_nBrowseType;
int						OplFileBrowser::m_nReqItemCount;
char 					OplFileBrowser::m_szFileExt[];
int						OplFileBrowser::m_nSortType;
char 					OplFileBrowser::m_szRootPath[];
HUINT32					OplFileBrowser::m_nHandleId;
HINT32					OplFileBrowser::m_nIndex;

OplFileBrowser::OplFileBrowser()
{
	ENTRY;
	m_oplFileBrowserInstance = this;

}
OplFileBrowser::~OplFileBrowser()
{
	ENTRY;
}

void OplFileBrowser::SetListener(OplFileBrowserCallback callback)
{
	ENTRY;
	if ( callback )
	{
		m_callback = callback;
	}
}

eOplFileBrowserState OplFileBrowser::GetState()
{
	return m_nCurrentState;
}

void OplFileBrowser::send_NotifyMessage(eOplFileBrowserState nMode, HUINT32 requestId, HINT32 size, void **pContentList)
{
	ENTRY;
	if(NULL != m_callback)
	{
		m_nCurrentState = nMode;
		m_callback(nMode, requestId, size, pContentList);
	}
	LEAVE;
}

void OplFileBrowser::dama_callback(int requestId, void *pvUserData, HxList_t *pstResultList, void *pvCbInfo)
{
	int size = 0;
	int index = 0;
	int i = 0;
	HxList_t *pstOrgResultList = pstResultList;
	HxList_t *pFound = NULL;
	DxFileInfo_t* pstFsInfo = NULL;
	int contentCount = HLIB_LIST_Length(pstResultList);
	HxLOG_Debug("pstResultList address : %p \n", pstResultList);
	HxLOG_Debug("file count            : %d \n", contentCount);
	HxLOG_Debug("request count         : %d \n", m_nReqItemCount);

	if(0 < contentCount)
	{
		//1  ### event callback upper side
		OplBrowsedContent **pContentList = (OplBrowsedContent **)HLIB_STD_MemAlloc(sizeof(OplBrowsedContentHandle) * contentCount);

		pFound = pstOrgResultList;
		while( pFound )
		{
			pstFsInfo = (DxFileInfo_t*)HLIB_LIST_Data(pFound);
			pContentList[i] = new OplBrowsedContent((OplBrowsedContentHandle)pstFsInfo);
			size++;

			if(m_nReqItemCount <= size)
			{
				send_NotifyMessage(eOPL_FILEBRS_STATE_ADD, requestId, size, (void**)&pContentList[index*m_nReqItemCount]);
				size = 0;
				index++;
			}
			pFound = HLIB_LIST_Next(pFound);
			i++;
		}

		if(0 != size)
		{
			send_NotifyMessage(eOPL_FILEBRS_STATE_ADD, requestId, size, (void**)&pContentList[index*m_nReqItemCount]);
		}
	}
	send_NotifyMessage(eOPL_FILEBRS_STATE_COMPLETE, requestId);
	APK_FILEMGR_FreeResultList(pstOrgResultList);
}

void OplFileBrowser::umma_callback(HUINT32 handleId, HINT32 event, HINT32 result, void *pvData)
{
	APK_FILEMGR_BROWSE_EVT_e eEvt = (APK_FILEMGR_BROWSE_EVT_e)event;
	APK_FILEMGR_BROWSE_RESULT_t *pInfo = (APK_FILEMGR_BROWSE_RESULT_t *)pvData;
	switch( eEvt )
	{
		case eAPK_FILEMGR_BROWSE_EVT_OPEN :
			{
				HxLOG_Print("[OPL_FILE] UMMA Event - OPEN. Total Item(%d)\n", pInfo->nItemCnt );
				if ( ERR_OK == result )
				{
					//APK_FILEMGR_BROWSE_Sort( handleId, 0 );
					//APK_FILEMGR_BROWSE_ScanItems( handleId, m_nReqItemCount, 0 );
				}
				else
				{
					send_NotifyMessage(eOPL_FILEBRS_STATE_ERROR, handleId);
				}
			}
			break;
		case eAPK_FILEMGR_BROWSE_EVT_CLOSE :
			{
				HxLOG_Print("[OPL_FILE] UMMA Event - CLOSE\n");
			}
			break;
		case eAPK_FILEMGR_BROWSE_EVT_SCANITEMS :
			{
				HINT32 nIndex = 0;
				HINT32 i = 0;

				OplBrowsedContent **pContentList = NULL;
				HxList_t *pFound = pInfo->pstFileList;
				DxFileInfo_t* pstFsInfo = NULL;
				HxLOG_Print("[OPL_FILE] UMMA Event - ScanItems. index(%d), Item Count(%d)\n", pInfo->nReqIdx, pInfo->nItemCnt );
				if ( m_nHandleId != handleId )
					return ;
				if( pInfo->nItemCnt )
					pContentList = (OplBrowsedContent **)HLIB_STD_MemAlloc(sizeof(OplBrowsedContentHandle) * pInfo->nItemCnt);

				if (pContentList == NULL)
				{
					if ( nIndex >=  pInfo->nTotalNum )
					{
						send_NotifyMessage(eOPL_FILEBRS_STATE_COMPLETE, handleId);
					}
					return;
				}
				if ( pContentList )
				{
				while ( pFound )
				{
					pstFsInfo = (DxFileInfo_t*)HLIB_LIST_Data(pFound);
					pContentList[i] = new OplBrowsedContent((OplBrowsedContentHandle)pstFsInfo);
					pFound = HLIB_LIST_Next(pFound);
					++i;
				}
				}
				send_NotifyMessage(eOPL_FILEBRS_STATE_ADD, handleId, pInfo->nItemCnt, (void**)pContentList);

				nIndex = pInfo->nReqIdx +  pInfo->nItemCnt;
				if ( nIndex >=  pInfo->nTotalNum )
				{
					send_NotifyMessage(eOPL_FILEBRS_STATE_COMPLETE, handleId);
				}
			}
			break;
		case eAPK_FILEMGR_BROWSE_EVT_GETITEMS :
			{
				HINT32 nIndex = 0;
				HINT32 i = 0;

				OplBrowsedContent **pContentList = NULL;
				HxList_t *pFound = pInfo->pstFileList;
				DxFileInfo_t* pstFsInfo = NULL;
				HxLOG_Print("[OPL_FILE] UMMA Event - GetItems. index(%d), Item Count(%d)\n", pInfo->nReqIdx, pInfo->nItemCnt );
				if ( m_nHandleId != handleId )
					return ;
				if( pInfo->nItemCnt )
				{
					pContentList = (OplBrowsedContent **)HLIB_STD_MemAlloc(sizeof(OplBrowsedContentHandle) * pInfo->nItemCnt);
					if (pContentList == NULL)
						return;
				}
				if ( pContentList )
				{
					while ( pFound )
					{
						pstFsInfo = (DxFileInfo_t*)HLIB_LIST_Data(pFound);
						pContentList[i] = new OplBrowsedContent((OplBrowsedContentHandle)pstFsInfo);
						pFound = HLIB_LIST_Next(pFound);
						++i;
					}
				}
				send_NotifyMessage(eOPL_FILEBRS_STATE_ADD, handleId, pInfo->nItemCnt, (void**)pContentList);

				nIndex = pInfo->nReqIdx +  pInfo->nItemCnt;
				if ( nIndex < pInfo->nTotalNum )
				{
					APK_FILEMGR_BROWSE_GetItems( handleId, nIndex, m_nReqItemCount, 0 );
				}
				else
				{
					send_NotifyMessage(eOPL_FILEBRS_STATE_COMPLETE, handleId);
				}
			}
			break;
		case eAPK_FILEMGR_BROWSE_EVT_SORT :
			{
				APK_FILEMGR_BROWSE_GetItems( handleId, 0, m_nReqItemCount, 0 );
			}
			break;
		default :
			return ;
	}
}

void OplFileBrowser::init_BrowseContext(const char *szBrowsePath, int nBrowsingType, int nReqItemCount, char *szFileExt, int nSortType, const char *szRootPath)
{
	HxSTD_MemSet( m_szBrowsePath, 		0x00, 			sizeof(m_szBrowsePath));
	HxSTD_MemSet( m_szRootPath, 		0x00, 			sizeof(m_szRootPath));
	HxSTD_MemSet( m_szFileExt, 			0x00, 			sizeof(m_szFileExt));
	HxSTD_MemSet( g_szFileExtension, 	0x00, 			sizeof(g_szFileExtension));

	HLIB_STD_StrNCpySafe(m_szBrowsePath, 		szBrowsePath, 	sizeof(m_szBrowsePath));
	HLIB_STD_StrNCpySafe(m_szRootPath, 		szRootPath, 	sizeof(m_szRootPath));
	HLIB_STD_StrNCpySafe(m_szFileExt, 			szFileExt, 		sizeof(m_szFileExt));
	HLIB_STD_StrNCpySafe(g_szFileExtension, 	szFileExt, 		sizeof(g_szFileExtension));

	m_nReqItemCount 				  = nReqItemCount;
	m_nBrowseType 					  = nBrowsingType;
	m_nSortType 					  = nSortType;

	HxLOG_Debug("browsePath : %s \n", m_szBrowsePath);
	HxLOG_Debug("rootPath   : %s \n", m_szRootPath);
	HxLOG_Debug("reqCount   : %d \n", m_nReqItemCount);
	HxLOG_Debug("browseType : %s \n", m_nBrowseType==0 ? "ONEDEPTH_VIEW" : "FOLDER_VIEW");
	HxLOG_Debug("fileExt    : %s \n", m_szFileExt);
	HxLOG_Debug("sortType   : %s \n", m_nSortType == 0 ? "ALPHA_SORT" : "Unknown Sort");

}

fmgr_ticket_t OplFileBrowser::browseContent_byOndepth(void *pParam)
{
	HUINT32		ulBrowsingMode;
	fmgr_ticket_t *ticket = (fmgr_ticket_t*)pParam;

	ulBrowsingMode = eAxFILEMGR_MODE_ONEDEPTHVIEW | AxFILEMGR_BROWSE_DIR | AxFILEMGR_BROWSE_FILE;

	if(0 != HxSTD_StrCmp(m_szBrowsePath, m_szRootPath))
	{
		ulBrowsingMode = eAxFILEMGR_MODE_ONEDEPTHVIEW;
	}


	HxLOG_Debug("browseContent_byOndepth [%p]\n", ulBrowsingMode);
	if (ERR_FAIL == APK_FILEMGR_BrowserFiles(m_szBrowsePath, m_szFileExt, NULL, ulBrowsingMode, OplFileBrowser::dama_callback, (HINT32*)ticket))
	{
		send_NotifyMessage(eOPL_FILEBRS_STATE_ERROR, *ticket);
	} else
	{
		send_NotifyMessage(eOPL_FILEBRS_STATE_START, *ticket);
	}

	return *ticket;
}

fmgr_ticket_t OplFileBrowser::browseContent_byFolderView(void *pParam)
{
	HUINT32		ulBrowsingMode;
	fmgr_ticket_t *ticket = (fmgr_ticket_t*)pParam;

	ulBrowsingMode = eAxFILEMGR_MODE_FOLDERVIEW | AxFILEMGR_BROWSE_DIR | AxFILEMGR_BROWSE_FILE;

	if (ERR_FAIL == APK_FILEMGR_BrowserFiles(m_szBrowsePath, m_szFileExt,NULL, ulBrowsingMode, OplFileBrowser::dama_callback, (HINT32*)ticket))

	{
		send_NotifyMessage(eOPL_FILEBRS_STATE_ERROR, *ticket);
	} else
	{
		send_NotifyMessage(eOPL_FILEBRS_STATE_START, *ticket);
	}

	return *ticket;
}

fmgr_ticket_t OplFileBrowser::browseContent_byUmma(void *pParam)
{
	fmgr_ticket_t *ticket = (fmgr_ticket_t*)pParam;
	HUINT32		ulBrowsingMode;
	ulBrowsingMode = AxFILEMGR_BROWSE_DIR | AxFILEMGR_BROWSE_FILE;
	if ( 0 == m_nHandleId )
	{
		APK_FILEMGR_BROWSE_RegisterCallback( OplFileBrowser::umma_callback );
	}
	else
	{
		m_nIndex = 0;
		APK_FILEMGR_BROWSE_Close(m_nHandleId);
	}

	HxLOG_Print("[OPL_FILE] Browse open(%s), reqCount(%d)\n", m_szBrowsePath, m_nReqItemCount );
	m_nHandleId = APK_FILEMGR_BROWSE_Open( m_szBrowsePath, ulBrowsingMode, m_szFileExt, 0 );
	if ( 0 == m_nHandleId )
	{
		return browseContent_byFolderView(pParam);
	}
	else
		*ticket = (fmgr_ticket_t)m_nHandleId;

	if ( 0 != m_nHandleId )
	{
		send_NotifyMessage(eOPL_FILEBRS_STATE_START, *ticket);
		APK_FILEMGR_BROWSE_ScanItems( m_nHandleId, m_nReqItemCount, 0 );
	}
	else
	{
		send_NotifyMessage(eOPL_FILEBRS_STATE_ERROR, *ticket);
	}

	return *ticket;
}

fmgr_ticket_t OplFileBrowser::BrowseContent(const char *szBrowsePath, int nBrowsingType, int nReqItemCount, char *szFileExt, int nSortType, const char *szRootPath)
{
	OplFileBrowseThreadCall threadFunc = NULL;
	fmgr_ticket_t ticket = eOPL_FILEMGR_STATE_UNKNOWN_TICKET;

	init_BrowseContext(szBrowsePath, nBrowsingType, nReqItemCount, szFileExt, nSortType, szRootPath);

	switch(nBrowsingType)
	{
		case eBROWSE_BY_1DEPTH_VIEW:
			threadFunc = browseContent_byOndepth;
			break;
		case eBROWSE_BY_FOLDER_VIEW:
			//threadFunc = browseContent_byFolderView;
			threadFunc = browseContent_byUmma;
			break;
		default:
			return ticket;
	}

	threadFunc(&ticket);
	return ticket;
}

/****************************************************************************************
 * OplFileSearch
 ***************************************************************************************/
OplFileSearchCallback 	OplFileSearch::m_callback = NULL;
eOplFileBrowserState 	OplFileSearch::m_nCurrentState;
OplFileSearch 			*OplFileSearch::m_oplFileSearchInstance;
char 					OplFileSearch::m_szCategory[];
char 					OplFileSearch::m_szKeyword[];
int						OplFileSearch::m_nReqItemCount;
int						OplFileSearch::m_nSortType;
HBOOL					OplFileSearch::m_bReqGroup;

OplFileSearch::OplFileSearch()
{
	ENTRY;
	m_oplFileSearchInstance = this;

}
OplFileSearch::~OplFileSearch()
{
	ENTRY;
}

void OplFileSearch::SetListener(OplFileSearchCallback callback)
{
	ENTRY;
	if ( callback )
	{
		m_callback = callback;
	}
}

eOplFileBrowserState OplFileSearch::GetState()
{
	return m_nCurrentState;
}

void OplFileSearch::send_NotifyMessage(eOplFileBrowserState nMode, HUINT32 requestId, HINT32 size, void **pContentList)
{
	ENTRY;
	if(NULL != m_callback)
	{
		HxLOG_Debug("send Search Results. requestId(%d), size(%d) \n", requestId, size);
		m_nCurrentState = nMode;
		m_callback(nMode, requestId, size, pContentList);
	}
	else
	{
		HxLOG_Error("m_callback is null.\n");
	}
	LEAVE;
}

void OplFileSearch::dama_callback(int requestId, void *pvUserData, HxList_t *pstResultList, void *pvCbInfo)
{
	int size = 0;
	int index = 0;
	int i = 0;
	DxFileInfo_t* pstFsInfo = NULL;
	HxList_t *pstOrgResultList = pstResultList;
	HxList_t *pFound = NULL;
	int contentCount = HLIB_LIST_Length(pstResultList);
	int (*sort_func)(const void *cmp1, const void *cmp2) = sort_asc;
	int compType = 0;
	enum { eCOMP_TILTE = 0, eCOMP_ALBUM, eCOMP_ARTIST };
	HxLOG_Debug("pstResultList address : %p \n", pstResultList);
	HxLOG_Debug("result count            : %d \n", contentCount);
	HxLOG_Debug("request count         : %d \n", m_nReqItemCount);

	if ( m_nSortType )
		sort_func = sort_rasc;
	else
		sort_func = sort_asc;

	if ( !HxSTD_StrCmp( m_szCategory, "albumlist" ) )
		compType = eCOMP_ALBUM;
	else if ( !HxSTD_StrCmp( m_szCategory, "artistlist" ) )
		compType = eCOMP_ARTIST;
	else
		compType = eCOMP_TILTE;

	if(0 < contentCount)
	{
		OplBrowsedContent **pContentList = (OplBrowsedContent **)HLIB_STD_MemAlloc(sizeof(OplBrowsedContentHandle) * contentCount);

		pFound = pstOrgResultList;
		/* convert to content*/
		while( pFound )
		{
			pstFsInfo = (DxFileInfo_t*)HLIB_LIST_Data(pFound);
			pContentList[i] = new OplBrowsedContent((OplBrowsedContentHandle)pstFsInfo);
			if ( m_bReqGroup )
				pContentList[i]->SetFileType(eGROUP_TYPE);

			switch (compType)
			{
				case eCOMP_TILTE :
					pContentList[i]->SetCompare(pContentList[i]->GetTitle());
					break;
				case eCOMP_ALBUM :
					pContentList[i]->SetCompare(pContentList[i]->GetAlbum());
					break;
				case eCOMP_ARTIST :
					pContentList[i]->SetCompare(pContentList[i]->GetArtist());
					break;
				default :
					break;
			}

			pFound = HLIB_LIST_Next(pFound);
			i++;
		}
		/* sorting */
		qsort((void *)pContentList, contentCount, sizeof(OplBrowsedContentHandle), sort_func );

		/* notify Event */
		for (i = 0; i < contentCount; i++)
		{
			size++;
			if(m_nReqItemCount <= size)
			{
				send_NotifyMessage(eOPL_FILEBRS_STATE_ADD, requestId, size, (void**)&pContentList[index*m_nReqItemCount]);
				size = 0;
				index++;
			}
		}
		if(0 != size)
		{
			send_NotifyMessage(eOPL_FILEBRS_STATE_ADD, requestId, size, (void**)&pContentList[index*m_nReqItemCount]);
		}
	}
	send_NotifyMessage(eOPL_FILEBRS_STATE_COMPLETE, requestId);
	APK_FILEMGR_FreeResultList(pstOrgResultList);
}

void OplFileSearch::init_SearchContext(const HCHAR *szCategory, const HCHAR *szKeyword, HINT32 nReqItemCount, HINT32 nSortType)
{
	HxSTD_MemSet( m_szCategory,	0x00, sizeof(m_szCategory));
	HLIB_STD_StrNCpySafe(m_szCategory, szCategory, sizeof(m_szCategory));

	HxSTD_MemSet( m_szKeyword,	0x00, sizeof(m_szKeyword));
	HLIB_STD_StrNCpySafe(m_szKeyword, szKeyword, sizeof(m_szKeyword));

	m_nReqItemCount 				  = nReqItemCount;
	m_nSortType 					  = nSortType;

	HxLOG_Debug("category : %s \n", m_szCategory);
	HxLOG_Debug("keyword  : %s \n", m_szKeyword);
	HxLOG_Debug("reqCount   : %d \n", m_nReqItemCount);
	HxLOG_Debug("sortType   : %s \n", m_nSortType == 0 ? "ALPHA_SORT" : "Unknown Sort");
}

#define COMBINATION_CHAR "|"
fmgr_ticket_t OplFileSearch::SearchContent(const HCHAR *szSearchPath, const HCHAR *szCategory, const HCHAR *szKeyword, HINT32 nReqItemCount, HINT32 nSortType)
{
	fmgr_ticket_t ticket = eOPL_FILEMGR_STATE_UNKNOWN_TICKET;
	HUINT32 ulSearchMode = 0;
	HCHAR szSearchCmd[256] = { 0 };
	const HCHAR *path = NULL;
	HCHAR *pCombin = NULL;
	HINT32 i = 0;
	HBOOL bMached = TRUE;

	init_SearchContext(szCategory, szKeyword, nReqItemCount, nSortType);

	if (HxSTD_StrStr(szCategory, "albumlist") )
	{
		ulSearchMode = eAxFILEMGR_MODE_GROUP;
		HxSTD_snprintf( szSearchCmd, sizeof(szSearchCmd), "%s", "Album" );
		m_bReqGroup = TRUE;
	}
	else if	(HxSTD_StrStr(szCategory, "artistlist") )
	{
		ulSearchMode = eAxFILEMGR_MODE_GROUP;
		HxSTD_snprintf( szSearchCmd, sizeof(szSearchCmd), "%s", "Artist" );
		m_bReqGroup = TRUE;
	}
	else if (HxSTD_StrStr(szCategory, "album"))
	{
		ulSearchMode = eAxFILEMGR_MODE_KEYWORD;
		if (HxSTD_StrEmpty(szKeyword))
			HxSTD_snprintf( szSearchCmd, sizeof(szSearchCmd), "%s", "Album" );
		else
			HxSTD_snprintf( szSearchCmd, sizeof(szSearchCmd), "%s=%s", "Album", szKeyword );
		m_bReqGroup = FALSE;
	}
	else if (HxSTD_StrStr(szCategory, "artist"))
	{
		ulSearchMode = eAxFILEMGR_MODE_KEYWORD;
		if (HxSTD_StrEmpty(szKeyword))
			HxSTD_snprintf( szSearchCmd, sizeof(szSearchCmd), "%s", "Artist" );
		else
			HxSTD_snprintf( szSearchCmd, sizeof(szSearchCmd), "%s=%s", "Artist", szKeyword );

		m_bReqGroup = FALSE;
	}
	else
	{
		bMached = FALSE;
	}

	if ( !ulSearchMode )
	{
		ulSearchMode = eAxFILEMGR_MODE_ONEDEPTHVIEW;
		m_bReqGroup = FALSE;
	}

	pCombin = (HCHAR*)HxSTD_StrStr( szCategory, COMBINATION_CHAR );
	while ( !HxSTD_StrEmpty(s_subtype_table[i].szType) )
	{
		if ( HxSTD_StrStr( szCategory, s_subtype_table[i].szType ) )
		{
			ulSearchMode |= s_subtype_table[i].ulValue;
			bMached = TRUE;
			if ( NULL == pCombin )
				break;
		}
		i++;
	}

	if ( TRUE != bMached )
	{
		return ticket;
	}

	if ( HxSTD_StrLen(szSearchPath) > 1 )
		path = szSearchPath;

	if (ERR_FAIL == APK_FILEMGR_BrowserFiles((const HCHAR *)path, NULL, szSearchCmd,
							ulSearchMode, OplFileSearch::dama_callback, (HINT32*)&ticket))
	{
		send_NotifyMessage(eOPL_FILEBRS_STATE_ERROR, ticket);
	}
	else
	{
		send_NotifyMessage(eOPL_FILEBRS_STATE_START, ticket);
	}

	return ticket;
}

int OplFileSearch::sort_asc( const void *cmp1, const void *cmp2 )
{
	OplBrowsedContent *pContent1 = *((OplBrowsedContent **)cmp1);
	OplBrowsedContent *pContent2 = *((OplBrowsedContent **)cmp2);
	HCHAR *pComp1 = pContent1->GetCompare();
	HCHAR *pComp2 = pContent2->GetCompare();

	if (HxSTD_StrEmpty(pComp1) && HxSTD_StrEmpty(pComp2))
		return 0;

	if (HxSTD_StrEmpty(pComp1))
		return 1;

	if (HxSTD_StrEmpty(pComp2))
		return -1;

	return  HLIB_STD_StrCaseCmp(pComp1, pComp2);
}

int OplFileSearch::sort_rasc( const void *cmp1, const void *cmp2 )
{
	OplBrowsedContent *pContent1 = *((OplBrowsedContent **)cmp1);
	OplBrowsedContent *pContent2 = *((OplBrowsedContent **)cmp2);
	HCHAR *pComp1 = pContent1->GetCompare();
	HCHAR *pComp2 = pContent2->GetCompare();
	int ret = 0;

	if (HxSTD_StrEmpty(pComp1) && HxSTD_StrEmpty(pComp2))
		return 0;

	if (HxSTD_StrEmpty(pComp1))
		return 1;

	if (HxSTD_StrEmpty(pComp2))
		return -1;

	ret = HLIB_STD_StrCaseCmp(pComp1, pComp2);

	if ( ret > 0 )
		return -1;
	else if ( ret < 0 )
		return  1;

	return 0;
}

/****************************************************************************************
 * OplRecFMTicketInfo
 ***************************************************************************************/
OplRecFMTicketInfo::OplRecFMTicketInfo(const char *id, unsigned int key, bool bDecrypt, eOplOperationCommand cmd)
		: m_key(key), m_state(eOPL_FILEMGR_STATE_IDLE), m_decrypt(bDecrypt), m_command(cmd)
{
	ENTRY;
	if(id)
	{
		m_recordingId = HLIB_STD_StrDup(id);
	}
}

OplRecFMTicketInfo::~OplRecFMTicketInfo()
{
	ENTRY;
	if(m_recordingId){ HLIB_STD_MemFree(m_recordingId); }
	m_reqIdList.clear();
}

OplRecFMReqIdInfo* OplRecFMTicketInfo::getReqIdInfo(unsigned int id)
{
	ENTRY;
	OplRecFMReqIdInfo *reqIdInfo = NULL;
	for(list<OplRecFMReqIdInfo *>::iterator it = m_reqIdList.begin(); it!=m_reqIdList.end(); ++it)
	{
		if((*it)->getReqId() == id)
		{
			reqIdInfo = (*it);
			break;
		}
	}
	return reqIdInfo;
}

void OplRecFMTicketInfo::transitState(eOplFileManagerBaseState base)
{
	ENTRY;
	unsigned int ingReqs = 0, completeReqs = 0, errorReqs = 0;

	for(list<OplRecFMReqIdInfo *>::iterator it = m_reqIdList.begin(); it!=m_reqIdList.end(); ++it)
	{
		switch((*it)->getState())
		{
			case eOPL_ING_INDEX:
				ingReqs+=1;
				break;
			case eOPL_COMPLETE_INDEX:
				completeReqs+=1;
				break;
			case eOPL_ERROR_INDEX:
				errorReqs+=1;
				break;
			default:
				break;
		}
	}

	if(errorReqs > 0)
	{
		m_state = (eOplFileManagerState)(base + eOPL_ERROR_INDEX);
	}
	else
	{
		if(completeReqs == m_reqIdList.size())
		{
			m_state = (eOplFileManagerState)(base + eOPL_COMPLETE_INDEX);
		}
	}
	return;
}

void OplRecFMTicketInfo::stopOperation()
{
	ENTRY;
	unsigned int cancelReqId = 0;
	OplRecFMReqIdInfo *it = NULL;
	HUINT32 ulTargetRequestId = 0;

	if(m_decrypt)
	{
		while (!m_reqIdList.empty())
		{
			it = m_reqIdList.back();
			if(it)
			{
				ulTargetRequestId = it->getReqId();
				HxLOG_Debug("[%s:%d] FS_Cancel %d\n", __FUNCTION__, __LINE__, ulTargetRequestId);
				APK_FILEMGR_Cancel(it->getReqId(), NULL, (HINT32*)&cancelReqId);
//				APK_FS_Cancel(it->getReqId(), &cancelReqId);
			}
			m_reqIdList.pop_back();
		}
	}
	else
	{
		HxLOG_Debug("[%s:%d] FS_Cancel %s\n", __FUNCTION__, __LINE__, m_recordingId);
		APK_META_RECORD_CancelFileOperation(APK_META_RECORD_GetCID(m_recordingId));
	}
	return;
}

/****************************************************************************************
 * OplRecFileManager
 ***************************************************************************************/
list<OplRecFMTicketInfo *>	OplRecFileManager::m_tickets;
OplRecFileManagerCallback			OplRecFileManager::m_callback = NULL;
HUINT32								OplRecFileManager::m_ticketKey = 0x7F000000;
OplRecFileManager				*OplRecFileManager::m_oplRecFileManagerInstance;

OplRecFileManager::OplRecFileManager()
{
	ENTRY;
	m_oplRecFileManagerInstance = this;
}

OplRecFileManager::~OplRecFileManager()
{

}

void OplRecFileManager::SetListener(OplRecFileManagerCallback callback)
{
	ENTRY;
	if(callback)
	{
		m_callback = callback;
	}
}

HBOOL OplRecFileManager::CopyRecording (const HCHAR *id, HUINT32 ulDevId, int *pTicketKey)
{
	HERROR				hErr;
	OplRecFMTicketInfo *recTicketInfo = NULL;
	HCHAR				szDstPath[DxSTORAGE_NAME_LEN] = {0,};
	HBOOL				bDecryptDummy = FALSE;

	hErr = getRecordPath(ulDevId, szDstPath);
	if (hErr != ERR_OK)
	{
		HxLOG_Warning("fail to find record-path. copy to mount-root \n");
		hErr = getMountPath(ulDevId, szDstPath);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("fail to find mount-path. \n");
			return FALSE;
		}
	}

	recTicketInfo = new OplRecFMTicketInfo(id, createTicketKey(), bDecryptDummy, eOPL_CMD_COPY);
	if (recTicketInfo == NULL)
	{
		HxLOG_Error("new failed. \n");
		return FALSE;
	}

	hErr = APK_META_RECORD_Copy(APK_META_RECORD_GetCID(id), szDstPath,recTicketInfo->getDecrypt(), recordings_callback, (void *)recTicketInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("APK_META_RECORD_Copy failed. \n");
		if (recTicketInfo != NULL)
		{
			delete recTicketInfo;
		}
		return FALSE;
	}

	recTicketInfo->setState(eOPL_FILEMGR_STATE_COPYING);
	m_tickets.push_back(recTicketInfo);

	(*pTicketKey) = recTicketInfo->getKey();
	send_NotifyMessage((*pTicketKey), eOPL_FILEMGR_STATE_COPYING);

	return TRUE;
}

HBOOL OplRecFileManager::MoveRecording (const HCHAR *id, HUINT32 ulDevId, int *pTicketKey)
{
	HERROR				hErr;
	OplRecFMTicketInfo *recTicketInfo = NULL;
	HCHAR				szDstPath[DxSTORAGE_NAME_LEN] = {0,};
	HBOOL				bDecryptDummy = FALSE;

	hErr = getRecordPath(ulDevId, szDstPath);
	if (hErr != ERR_OK)
	{
		HxLOG_Warning("fail to find record-path. move to mount-root \n");
		hErr = getMountPath(ulDevId, szDstPath);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("fail to find mount-path. \n");
			return FALSE;
		}
	}

	recTicketInfo = new OplRecFMTicketInfo(id, createTicketKey(), bDecryptDummy, eOPL_CMD_MOVE);
	if (recTicketInfo == NULL)
	{
		HxLOG_Error("new failed. \n");
		return FALSE;
	}

	hErr = APK_META_RECORD_Move(APK_META_RECORD_GetCID(id), szDstPath,recTicketInfo->getDecrypt(), recordings_callback, (void *)recTicketInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("APK_META_RECORD_Move failed. \n");
		if (recTicketInfo != NULL)
		{
			delete recTicketInfo;
		}
		return FALSE;
	}

	recTicketInfo->setState(eOPL_FILEMGR_STATE_MOVING);
	m_tickets.push_back(recTicketInfo);

	(*pTicketKey) = recTicketInfo->getKey();
	send_NotifyMessage((*pTicketKey), eOPL_FILEMGR_STATE_MOVING);

	return TRUE;
}

HBOOL OplRecFileManager::RemoveRecording(const HCHAR *id, const HCHAR *srcPath, const HCHAR *fileName, int *pTicketKey)
{
    // TODO: Implement !!!!!!
    return false;
}

HBOOL OplRecFileManager::stopRecordingFileOp(int ticketKey, int *pTicketKey)
{
	ENTRY;
	HBOOL bRet = FALSE;
	OplRecFMTicketInfo *recTicketInfo = NULL;

	HxLOG_Debug("[%s] ticketKey : %d\n", __FUNCTION__, ticketKey);

	recTicketInfo = getRecTicketInfo((unsigned int)ticketKey);
	if(recTicketInfo == NULL)
	{
		HxLOG_Debug("[%s:%d]\n", __FUNCTION__, __LINE__);
		bRet = TRUE;
	}
	else
	{
		recTicketInfo->stopOperation();
		m_tickets.remove(recTicketInfo);
	}

	return bRet;
}

OplRecFMTicketInfo *OplRecFileManager::getRecTicketInfo(unsigned int key)
{
	ENTRY;
	OplRecFMTicketInfo *ticketInfo = NULL;
	for(list<OplRecFMTicketInfo *>::iterator it = m_tickets.begin(); it!=m_tickets.end(); ++it)
	{
		if((*it)->getKey() == key)
		{
			ticketInfo = (*it);
			break;
		}
	}
	return ticketInfo;
}

unsigned int OplRecFileManager::createTicketKey()
{
	m_ticketKey += 1;
	return m_ticketKey;
}

void OplRecFileManager::send_NotifyMessage(int ticketKey, eOplFileManagerState state)
{
	if(NULL != m_callback)
	{
		HxLOG_Debug("send_NotifyMessage key 0x%x, state 0x%x\n", ticketKey, state);
		m_callback(ticketKey, state);
	}
}

void OplRecFileManager::recordings_callback(HERROR err, void *userdata, ...)
{
	OplRecFMTicketInfo *recTicketInfo = (OplRecFMTicketInfo *)userdata;
	eOplFileManagerState eState;
	HxLOG_Debug("cmd [0x%x]err[0x%x]\n", recTicketInfo->getCMD(),err);

	if(recTicketInfo->getCMD() != eOPL_CMD_COPY && recTicketInfo->getCMD() != eOPL_CMD_MOVE)
		return;

	if(recTicketInfo->getCMD() == eOPL_CMD_COPY)
	{
		if(err != ERR_OK)
		{
			switch(err)
			{
			case UMERR_REASON_NOT_ENOUGH_SPACE:
				eState = eOPL_FILEMGR_STATE_LACK_SPACE;
				break;
			case UMERR_REASON_SIZE_LIMIT_4G:
				eState = eOPL_FILEMGR_STATE_4G_FAT32;
				break;
			case UMERR_REASON_CANCELED:
			case UMERR_REASON_INVALID_ARG:
			case UMERR_REASON_FAIL_TO_WRITE:
			case UMERR_REASON_FAIL_TO_READ:
			case UMERR_REASON_FAIL_TO_STAT:
			default:
			eState = eOPL_FILEMGR_STATE_COPY_ERROR;
				break;
			}
		}
		else
			eState = eOPL_FILEMGR_STATE_COPY_COMPLETE;
	}


	if(recTicketInfo->getCMD() == eOPL_CMD_MOVE)
	{
		if(err != ERR_OK)
		{
			switch(err)
			{
			case UMERR_REASON_NOT_ENOUGH_SPACE:
				eState = eOPL_FILEMGR_STATE_LACK_SPACE;
				break;
			case UMERR_REASON_SIZE_LIMIT_4G:
				eState = eOPL_FILEMGR_STATE_4G_FAT32;
				break;
			case UMERR_REASON_CANCELED:
			case UMERR_REASON_INVALID_ARG:
			case UMERR_REASON_FAIL_TO_WRITE:
			case UMERR_REASON_FAIL_TO_READ:
			case UMERR_REASON_FAIL_TO_STAT:
			default:
			eState = eOPL_FILEMGR_STATE_MOVE_ERROR;
				break;
			}
		}
		else
			eState = eOPL_FILEMGR_STATE_MOVE_COMPLETE;
	}

	recTicketInfo->setState(eState);

	send_NotifyMessage(recTicketInfo->getKey(), recTicketInfo->getState());
	m_tickets.remove(recTicketInfo);

	return;
}


HERROR OplRecFileManager::getMountPath (HUINT32 ulDeviceId, HCHAR *szDstPath)
{
//	HBOOL				bDecryptDummy = FALSE;
	DxStorage_Info_t	*deviceInfo = NULL, *deviceInfoList = NULL;
//	DxStorage_PartitionInfo_t	*partitionInfo = NULL;
	HUINT32				deviceInfoListCount(0);
	HERROR				hErr, hResult = ERR_FAIL;
	HUINT32				devIndex; //, partitionIndex;

	hErr = APK_FILEMGR_STORAGE_GetList(eDxSTORAGE_FILTER_All, &deviceInfoList, &deviceInfoListCount);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("APK_FILEMGR_STORAGE_GetList failed. \n");
		return ERR_FAIL;
	}

	for (devIndex=0; devIndex<deviceInfoListCount; devIndex++)
	{
		deviceInfo = &(deviceInfoList[devIndex]);
		if (deviceInfo == NULL
			||deviceInfo->ulDeviceId != ulDeviceId)
		{
			continue;
		}

		HxLOG_Debug("Found mount path [%s] \n",deviceInfo->szMountPath);
		HxSTD_snprintf(szDstPath,DxSTORAGE_NAME_LEN,"%s",deviceInfo->szMountPath);
		hResult = ERR_OK;
		goto END_FUNC;
	}

END_FUNC:
	APK_FILEMGR_STORAGE_FreeList(deviceInfoList, deviceInfoListCount);

	return hResult;
}

HERROR OplRecFileManager::getRecordPath (HUINT32 ulDeviceId, HCHAR *szDstPath)
{
//	HBOOL				bDecryptDummy = FALSE;
	DxStorage_Info_t	*deviceInfo = NULL, *deviceInfoList = NULL;
	DxStorage_PartitionInfo_t	*partitionInfo = NULL;
	HUINT32				deviceInfoListCount(0);
	HERROR				hErr, hResult = ERR_FAIL;
	HUINT32				devIndex, partitionIndex;

	hErr = APK_FILEMGR_STORAGE_GetList(eDxSTORAGE_FILTER_All, &deviceInfoList, &deviceInfoListCount);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("APK_FILEMGR_STORAGE_GetList failed. \n");
		return ERR_FAIL;
	}

	for (devIndex=0; devIndex<deviceInfoListCount; devIndex++)
	{
		deviceInfo = &(deviceInfoList[devIndex]);
		if (deviceInfo == NULL
			|| deviceInfo->ulDeviceId != ulDeviceId)
		{
			continue;
		}

		for (partitionIndex=0; partitionIndex<deviceInfo->nPartitionNum && partitionIndex<DxSTORAGE_PARTITION_NUM; partitionIndex++)
		{
			partitionInfo = &(deviceInfo->astPartitionInfos[partitionIndex]);
			if (partitionInfo == NULL)
			{
				continue;
			}

			if (partitionInfo->bAvailable
				&& (partitionInfo->nPlaybackIdx != DxSTORAGE_INVALID_PVR_INDEX
					||partitionInfo->nPvrIdx != DxSTORAGE_INVALID_PVR_INDEX)
				&& !HxSTD_StrEmpty(partitionInfo->szRecordPath))
			{
				HxLOG_Debug("Found record path [%s] \n",partitionInfo->szRecordPath);
				HxSTD_snprintf(szDstPath,DxSTORAGE_NAME_LEN,"%s",partitionInfo->szRecordPath);
				hResult = ERR_OK;
				goto END_FUNC;
			}
		}
	}

END_FUNC:
	APK_FILEMGR_STORAGE_FreeList(deviceInfoList, deviceInfoListCount);

	return hResult;
}

/****************************************************************************************
 * OplFileManager
 ***************************************************************************************/
list<OplFMTicketInfo *> OplFileManager::m_tickets;
OplFileManagerCallback 	OplFileManager::m_callback = NULL;
OplFileExtensionMonitorCallback OplFileManager::m_cbExtMonitor = NULL;
OplFileScanSurveyCallback 		OplFileManager::m_cbScanSurvey = NULL;

OplFileManager 			*OplFileManager::m_oplFileManagerInstance;
OplFileManager::OplFileManager()
{
	ENTRY;
	m_oplFileManagerInstance = this;
}

OplFileManager::~OplFileManager()
{
	ENTRY;

}

void OplFileManager::SetListener(OplFileManagerCallback callback)
{
	ENTRY;
	if ( callback )
	{
		m_callback = callback;
	}
}

void OplFileManager::SetExtMonitorListener(OplFileExtensionMonitorCallback callback)
{
	ENTRY;
	if( callback)
	{
		m_cbExtMonitor = callback;
	}
}

void OplFileManager::SetScanSurveryListener(OplFileScanSurveyCallback callback)
{
	ENTRY;
	if(callback)
	{
		m_cbScanSurvey = callback;
	}
}

void OplFileManager::addListenerToUmma(fmgr_ticket_t ticket)
{
	APK_FILEMGR_AddListener(ticket, (void*)&FILE_MGR_TAG, OplFileManager::umma_callback);
//	APK_FS_AddNotifier(ticket, (APKE_FS_InfoCallback_f)OplFileManager::umma_callback, (void*)&FILE_MGR_TAG);
}

void OplFileManager::removeListenerFromUmma(fmgr_ticket_t ticket)
{
	//1 ȣ����ص� appkit �ؿ��� �˾Ƽ� remove ��.
	//APK_FS_RemoveNotifier(ticket, (APKE_FS_InfoCallback_f)OplFileManager::umma_callback);
}
eOplFileManagerBaseState 	OplFileManager::getBaseIndex(eOplOperationCommand eCmd)
{
	//1 ### �̹� ����� �ݿ��� ��Ȳ�̶� �������� ȹ��
	switch(eCmd)
	{
		case eOPL_CMD_COPY: 	return eOPL_FILEMGR_BASE_COPY;
		case eOPL_CMD_REMOVE:	return eOPL_FILEMGR_BASE_REMOVE;
		case eOPL_CMD_MOVE:		return eOPL_FILEMGR_BASE_MOVE;
		case eOPL_CMD_RENAME:	return eOPL_FILEMGR_BASE_RENAME;
		case eOPL_CMD_MKDIR:	return eOPL_FILEMGR_BASE_MKDIR;
		case eOPL_CMD_STOP:		return eOPL_FILEMGR_BASE_STOP;
		case eOPL_CMD_SCAN:		return eOPL_FILEMGR_BASE_SCAN;
		case eOPL_CMD_RMDIR:	return eOPL_FILEMGR_BASE_RMDIR;
		case eOPL_CMD_AUTOSCAN:	return eOPL_FILEMGR_BASE_AUTOSCAN;
		default:				return eOPL_FILEMGR_BASE_UNKNOWN;
	}
}
eOplFileManagerState OplFileManager::makeNotifyStateMsg(eOplOperationCommand eCmd, int index)
{
	eOplFileManagerState eNotifyMsg = eOPL_FILEMGR_STATE_IDLE;
	eNotifyMsg =  (eOplFileManagerState)(0x0000FFFF & ((eOplFileManagerState)(getBaseIndex(eCmd) + index)));
	return eNotifyMsg;
}

eOplFileManagerState 	OplFileManager::addTicketInfo(HERROR eError, fmgr_ticket_t ticket, eOplOperationCommand eCmd )
{
	eOplFileManagerState eNotifyMsg = eOPL_FILEMGR_STATE_IDLE;
	OplFMTicketInfo *info = NULL;

	if( ERR_OK == eError)
	{
		info = new OplFMTicketInfo(ticket, eCmd);

		if ( NULL != info )
		{
			eNotifyMsg = makeNotifyStateMsg(eCmd, eOPL_ING_INDEX);
			info->setState(eNotifyMsg);
			m_tickets.push_back(info);
			send_NotifyMessage(ticket, eNotifyMsg);	// 1st - mandatory call before 2nd called
			addListenerToUmma(ticket);				// 2nd
		}
	}

	if(ERR_FAIL == eError || NULL == info || eOPL_FILEMGR_STATE_UNKNOWN_TICKET == ticket)
	{
		eNotifyMsg = makeNotifyStateMsg(eCmd, eOPL_ERROR_INDEX);
		send_NotifyMessage(ticket, eNotifyMsg);
	}
	return eNotifyMsg;
}

fmgr_ticket_t OplFileManager::Rescan(const char *szScanPath, HBOOL bRecursive, char *szFileExt)
{
	fmgr_ticket_t ticket = eOPL_FILEMGR_STATE_UNKNOWN_TICKET;
	HERROR eRet = ERR_OK;
	if ( !szScanPath) return ticket;
	HxLOG_Debug("[%s] scanPath : %s, ext : %s\n", __FUNCTION__, szScanPath, szFileExt);

	eRet = APK_FILEMGR_ScanDir((const HCHAR*)szScanPath, "", bRecursive, NULL, (HINT32*)&ticket);
//	eRet = APK_FS_Scan((const HCHAR*)szScanPath, bRecursive, &ticket);
	addTicketInfo(eRet, ticket, eOPL_CMD_SCAN);
	return ticket;
}

fmgr_ticket_t OplFileManager::IsScanning(const char *pszPath, HBOOL bOnlyCheck)
{
	HBOOL bRet = FALSE;
	fmgr_ticket_t ticket = eOPL_FILEMGR_STATE_UNKNOWN_TICKET;
	if(TRUE == HxSTD_StrEmpty(pszPath)) return bRet;
	HxLOG_Debug("[%s] bOnlyCheck : %d \n", __FUNCTION__, bOnlyCheck);

	HxLOG_Warning("[%s;%d] ScanSurvey : Doesn't need anymore!!!\n", __FUNCTION__, __LINE__);

	return ticket;
#if	0
	if(ERR_OK == APK_FS_ScanSurvey(pszPath, ((bOnlyCheck == TRUE) ? NULL : (HINT32*)&ticket),
									(APKE_FS_InfoCallback_f)OplFileManager::umma_ScanSurveyCallback, (void*)&FILE_SCN_TAG))
	{

	}

	return bOnlyCheck == TRUE ? eOPL_FILEMGR_STATE_UNKNOWN_TICKET : ticket;
#endif
}

fmgr_ticket_t OplFileManager::StartAutoScan( HUINT32 ulCycleTime )
{
	fmgr_ticket_t ticket = eOPL_FILEMGR_STATE_UNKNOWN_TICKET;

#if	0
	HERROR eRet = ERR_OK;
	if(0 >= ulCycleTime) return ticket;
	HxLOG_Debug("[%s] Cycle Duration : %d \n", __FUNCTION__, ulCycleTime);

	HxLOG_Warning("[%s;%d] StartAutoScan : Doesn't need anymore!!!\n", __FUNCTION__, __LINE__);
#endif

	return ticket;
#if	0
	eRet = APK_FS_AutoScanEnable(ulCycleTime, &ticket);
	addTicketInfo(eRet, ticket, eOPL_CMD_AUTOSCAN);
	return ticket;
#endif
}

HBOOL OplFileManager::StopAutoScan( void )
{
	HxLOG_Warning("[%s;%d] StopAutoScan : Doesn't need anymore!!!\n", __FUNCTION__, __LINE__);
	return TRUE;

#if	0
	HBOOL bRet = FALSE;
	HxLOG_Debug("[%s] StopAutoScan\n", __FUNCTION__);

	if(ERR_OK == APK_FS_AutoScanDisable())
	{
		bRet= TRUE;
	}
	return bRet;
#endif
}

fmgr_ticket_t OplFileManager::Copy(const char *src, const char *dst,  HBOOL bRecursive, char *ext, HBOOL decrypt)
{
	ENTRY;
	HERROR eRet = ERR_OK;
	fmgr_ticket_t ticket = eOPL_FILEMGR_STATE_UNKNOWN_TICKET;
	if ( !src || !dst ) return ticket;
	HxLOG_Debug("[%s] src : %s, dst : %s, ext : %s\n", __FUNCTION__, src, dst, ext);

	eRet = APK_FILEMGR_Copy(src, dst, ext, decrypt, NULL, (HINT32*)&ticket);
	if ( !HxSTD_StrEmpty(dst) )
	addTicketInfo(eRet, ticket, eOPL_CMD_COPY);
	else
		send_NotifyMessage(ticket, eOPL_FILEMGR_STATE_DEVICE_INFO_ERROR);
	return ticket;
}

fmgr_ticket_t OplFileManager::Copy(const char *src[], int n, const char *dst_dir,  HBOOL bRecursive, char *ext, HBOOL decrypt)
{
	ENTRY;
	HERROR eRet = ERR_OK;
	fmgr_ticket_t ticket = eOPL_FILEMGR_STATE_UNKNOWN_TICKET;
	if ( n <= 0 || !isDirectory(dst_dir) ) return ticket;

	for ( int i=0; i<n; ++i )
	{
		if ( !src[i] ) continue;
		eRet = APK_FILEMGR_Copy((const char*)src[i], (const char*)dst_dir[i], ext, decrypt, NULL, (HINT32*)&ticket);
		addTicketInfo(eRet, ticket, eOPL_CMD_COPY);
	}
	return ticket;
}

fmgr_ticket_t OplFileManager::Move(const char *src, const char *dst,  HBOOL bRecursive, char *ext, HBOOL decrypt)
{
	ENTRY;
	HERROR eRet = ERR_OK;
	fmgr_ticket_t ticket = eOPL_FILEMGR_STATE_UNKNOWN_TICKET;
	if ( !src || !dst ) return ticket;
	HxLOG_Debug("[%s] src : %s, dst : %s, ext : %s\n", __FUNCTION__, src, dst, ext);

	eRet = APK_FILEMGR_Move(src, dst, ext, decrypt, NULL, (HINT32*)&ticket);
	if ( !HxSTD_StrEmpty(dst) )
	addTicketInfo(eRet, ticket, eOPL_CMD_MOVE);
	else
		send_NotifyMessage(ticket, eOPL_FILEMGR_STATE_DEVICE_INFO_ERROR);
	return ticket;
}

fmgr_ticket_t OplFileManager::Move(const char *src[], int n, const char *dst_dir,  HBOOL bRecursive, char *ext, HBOOL decrypt)
{
	ENTRY;
	HERROR eRet = ERR_OK;
	fmgr_ticket_t ticket = eOPL_FILEMGR_STATE_UNKNOWN_TICKET;
	if ( n <= 0 || !isDirectory(dst_dir) ) return ticket;

	for ( int i=0; i<n; ++i )
	{
		if ( !src[i] ) continue;
		eRet = APK_FILEMGR_Move(src[i], dst_dir, ext, FALSE, NULL, (HINT32*)&ticket);
//		eRet = APK_FS_Move((HCHAR*)src[i], (HCHAR*)dst_dir, (HCHAR*)ext, bRecursive, OP_FASTER,FALSE, &ticket);
		addTicketInfo(eRet, ticket, eOPL_CMD_MOVE);
	}
	return ticket;
}

fmgr_ticket_t OplFileManager::Remove(const char *path,  HBOOL bRecursive, char *ext)
{
	ENTRY;
	HERROR eRet = ERR_OK;
	fmgr_ticket_t ticket = eOPL_FILEMGR_STATE_UNKNOWN_TICKET;
	if ( !path ) return ticket;
	HxLOG_Debug("[%s] path : %s, ext : %s\n", __FUNCTION__, path, ext);

	eRet = APK_FILEMGR_Remove(path, ext, bRecursive, NULL, (HINT32*)&ticket);
//	eRet = APK_FS_Remove((HCHAR*)path, (HCHAR*)ext, bRecursive, OP_FASTER, &ticket);
	addTicketInfo(eRet, ticket, eOPL_CMD_REMOVE);

	return ticket;
}

fmgr_ticket_t OplFileManager::Remove(const char *path[], int n,  HBOOL bRecursive, char *ext)
{
	ENTRY;
	HERROR eRet = ERR_OK;
	fmgr_ticket_t ticket = eOPL_FILEMGR_STATE_UNKNOWN_TICKET;
	if ( n <= 0 ) return ticket;

	for ( int i=0; i<n; ++i )
	{
		if ( !path[i] ) continue;
		eRet = APK_FILEMGR_Remove(path[i], ext, bRecursive, NULL, (HINT32*)&ticket);
//		eRet = APK_FS_Remove((HCHAR*)path[i], (HCHAR*)ext, bRecursive, OP_FASTER, &ticket);
		addTicketInfo(eRet, ticket, eOPL_CMD_REMOVE);
	}
	return ticket;
}

fmgr_ticket_t OplFileManager::Rename(const char *from, const char *to, char *ext)
{
	ENTRY;
	HERROR eRet = ERR_OK;
	fmgr_ticket_t ticket = eOPL_FILEMGR_STATE_UNKNOWN_TICKET;
	if ( !from || !to ) return ticket;
	HxLOG_Debug("[%s] from : %s, to : %s, ext : %s\n", __FUNCTION__, from, to, ext);

	eRet = APK_FILEMGR_Move(from, to, ext, FALSE, NULL, (HINT32*)&ticket);
//	eRet = APK_FS_Move((HCHAR*)from, (HCHAR*)to, (HCHAR*)ext, false, OP_FASTER,FALSE, &ticket);
	addTicketInfo(eRet, ticket, eOPL_CMD_RENAME);
	return ticket;
}

fmgr_ticket_t OplFileManager::Stop(fmgr_ticket_t reqTicket)
{
	ENTRY;
	HERROR eRet = ERR_OK;
	fmgr_ticket_t ticket = eOPL_FILEMGR_STATE_UNKNOWN_TICKET;

	eRet = APK_FILEMGR_Cancel(reqTicket, NULL, (HINT32*)&ticket);
//	eRet = APK_FS_Cancel(reqTicket, &ticket);
	addTicketInfo(eRet, ticket, eOPL_CMD_STOP);
	return ticket;
}

fmgr_ticket_t OplFileManager::Rmdir(const char *dir)
{
	ENTRY;
	HERROR eRet = ERR_OK;
	fmgr_ticket_t ticket = eOPL_FILEMGR_STATE_UNKNOWN_TICKET;
	if ( !dir ) return ticket ;
	HxLOG_Debug("[%s] dir : %s\n", __FUNCTION__, dir);

	eRet = APK_FILEMGR_RemoveDir(dir, NULL, (HINT32*)&ticket);
//	eRet = APK_FS_Rmdir(dir, &ticket);
	addTicketInfo(eRet, ticket, eOPL_CMD_RMDIR);

	return ticket;
}

fmgr_ticket_t OplFileManager::Mkdir(const char *dir)
{
	ENTRY;
	HERROR eRet = ERR_OK;
	fmgr_ticket_t ticket = eOPL_FILEMGR_STATE_UNKNOWN_TICKET;
	if ( !dir ) return ticket;
	HxLOG_Debug("[%s] dir : %s\n", __FUNCTION__, dir);

	eRet = APK_FILEMGR_MakeDir(dir, NULL, (HINT32*)ticket);
//	eRet = APK_FS_Mkdir(dir, &ticket);
	addTicketInfo(eRet, ticket, eOPL_CMD_MKDIR);

	return ticket;
}

HUINT64  OplFileManager::GetSize(const HCHAR *path, const HCHAR *pExt)
{
	HUINT64 size = 0;
	HERROR eRet = ERR_OK;
	HxLOG_Print ( "GetSize() path : %s, ext : %s \n", path, pExt );
	eRet = APK_FILEMGR_GetSizeOfPath( path, pExt, &size );
	return size;
}

HBOOL OplFileManager::ExtenstionMonitor(const char *pszExt)
{
	HxLOG_Warning("[%s;%d] not supported!!\n", __FUNCTION__, __LINE__);

	return FALSE;
#if	0
	HBOOL bRet = TRUE;
	HxLOG_Debug("[%s] pszExt = [%s][userdata:%p]\n", __FUNCTION__, pszExt, &FILE_MNT_TAG);
	if(ERR_OK != APK_FS_ExtensionMonitor(pszExt, (APKE_FS_InfoCallback_f)OplFileManager::umma_ExtensionMonitorCallback, (void*)&FILE_MNT_TAG))
	{
		bRet = FALSE;
	}
	return bRet;
#endif
}

eOplFileManagerState OplFileManager::GetState(fmgr_ticket_t ticket)
{
	ENTRY;
	OplFMTicketInfo *info = getTicketInfo(ticket);
	if ( !info )
	{
		OplFileOperToken * pOperToken = OplFileOperToken::getInstance((HINT32)ticket);
		if ( pOperToken )
			return pOperToken->getState();
		else
			return eOPL_FILEMGR_STATE_IDLE;
	}
	return info->getState();
}

/****************************************************************************************
 * private functions
 ***************************************************************************************/
OplFMTicketInfo *OplFileManager::getTicketInfo(fmgr_ticket_t ticket)
{
	ENTRY;
	for ( list<OplFMTicketInfo *>::iterator it=m_tickets.begin(); it!=m_tickets.end(); ++it )
	{
		if ( (*it)->getTicket() == ticket ) return (*it);
	}
	return NULL;
}

void OplFileManager::send_NotifyMessage(int ticket, eOplFileManagerState state)
{
	if(NULL != m_callback)
	{
		m_callback(ticket, state);
	}
}

#if	0
void OplFileManager::umma_ScanSurveyCallback(void *pstInfo, void *userdata)
{
	ENTRY;
#if 1
	HUINT32	state = eFS_RET_UNKNOWN_RETURN;
	FILE_TAG *filetag = (FILE_TAG*)userdata;

	if(NULL == filetag) return;
	if(FILE_SCN_TAG != *filetag) return;

	APKS_FS_Notiinfo_t *info 	= (APKS_FS_Notiinfo_t*)pstInfo;
	if(NULL == info) return;

	if(NULL != m_cbScanSurvey)
	{
		switch(info->eMSG)
		{
			case eFS_RET_SCAN_START:
				state = eOPL_RET_SCANNING;
				break;
			case eFS_RET_SCAN_END:
				state = eOPL_RET_SCAN_COMPLETE;
				break;
			default:
				return;
		}
		m_cbScanSurvey(state, (void*)info->pszSource, 0 == info->ulRequestId ? eOPL_FILEMGR_STATE_UNKNOWN_TICKET : info->ulRequestId);
		HxLOG_Debug("state : %d, source : %s, requestId : %d \n", state, info->pszSource, info->ulRequestId);
	}
#endif
}
#endif

#if	0
void OplFileManager::umma_ExtensionMonitorCallback(void *pstInfo, void *userdata)
{
	ENTRY;
#if 1
	HUINT32	state = eFS_RET_UNKNOWN_RETURN;
	FILE_TAG *filetag = (FILE_TAG*)userdata;

	if(NULL == filetag) return;
	if(FILE_MNT_TAG != *filetag) return;

	APKS_FS_Notiinfo_t *info 	= (APKS_FS_Notiinfo_t*)pstInfo;
	if(NULL == info) return;

	if(NULL != m_cbExtMonitor)
	{
		switch(info->eMSG)
		{
			case eFS_RET_ADD:
				state = eOPL_RET_ADD;
				break;
			case eFS_RET_REMOVE:
				state = eOPL_RET_REMOVE;
				break;
			case eFS_RET_UPDATE:
				state = eOPL_RET_UPDATE;
				break;
			default:
				return;
		}
		m_cbExtMonitor(state, (void*)info->pszSource);
		HxLOG_Debug("state : %d, source : %s\n", state, info->pszSource);
	}
#endif
}
#endif

void OplFileManager::umma_callback(HINT32 nRequestId, void *pvUserData, HxList_t *pstResultList, void *pvCbInfo)
//void OplFileManager::umma_callback(void *pstInfo, void *userdata)
{
	ENTRY;
	FILE_TAG *filetag = (FILE_TAG*)pvUserData;
	AxFileMgrCbInfo_t	*pstInfo = (AxFileMgrCbInfo_t*)pvCbInfo;

	if(NULL == filetag) return;
	if(FILE_MGR_TAG != *filetag) return;

	eOplFileManagerState state 	= eOPL_FILEMGR_STATE_IDLE;
	fmgr_ticket_t ticket 		= static_cast<fmgr_ticket_t>(nRequestId);
	OplFMTicketInfo *tInfo 		= getTicketInfo(ticket);

	if ( NULL == tInfo ) return;

	eOplOperationCommand eCmd = tInfo->getCommand();

	if (pstInfo->nDoneOperationNum < pstInfo->nTotalOperationNum)
	{
		HxLOG_Debug("[%s:%d] Job is running [%d/%d]\n", __FUNCTION__, __LINE__, pstInfo->nDoneOperationNum, pstInfo->nTotalOperationNum);
		return;
	}

	switch(eCmd)
	{
		case eOPL_CMD_COPY:
		case eOPL_CMD_MOVE:
		case eOPL_CMD_REMOVE:
		case eOPL_CMD_RENAME:
		case eOPL_CMD_MKDIR:
		case eOPL_CMD_STOP:
		case eOPL_CMD_SCAN:
		case eOPL_CMD_RMDIR:
		case eOPL_CMD_AUTOSCAN:
			state = makeNotifyStateMsg(eCmd, (pstInfo->eState == eAxFILEMGR_ERR_OK) ? eOPL_COMPLETE_INDEX : eOPL_ERROR_INDEX);
			break;
		default:
			state  = eOPL_FILEMGR_STATE_UNKNOWN_TICKET;
			break;
	}

	if(eOPL_CMD_COPY == eCmd || eOPL_CMD_MOVE == eCmd)
	{
		switch(pstInfo->eState)
		{
			case eAxFILEMGR_ERR_NOT_ENOUGH_SPACE:
				state = eOPL_FILEMGR_STATE_LACK_SPACE;
				break;
			case eAxFILEMGR_ERR_4G_FAT:
				state = eOPL_FILEMGR_STATE_4G_FAT32;
				break;
			case eAxFILEMGR_ERR_FAIL_STAT:
				state = eOPL_FILEMGR_STATE_DEVICE_INFO_ERROR;
				break;
			default:
				break;
		}
	}
	HxLOG_Debug("command (0x%08X) result callback, state(%d) \n", eCmd, state);
	m_tickets.remove(tInfo);
	send_NotifyMessage(ticket, state);
}

/****************************************************************************************
 * OplFileOperToken
 ***************************************************************************************/
std::list<OplFileOperToken *>	OplFileOperToken::m_instanceList;

OplFileOperToken::OplFileOperToken()
{
	m_ulHndId = 0;
	m_nReqId = 0;
	m_eState = eOPL_FILEMGR_STATE_UNKNOWN_TICKET;
	HxSTD_MemSet( m_szFilter, 0x00, sizeof(m_szFilter) );
	m_instanceList.push_back(this);
}

OplFileOperToken::~OplFileOperToken()
{
	if ( m_ulHndId )
		APK_FILEMGR_GROUP_Release( m_ulHndId );
	m_instanceList.remove(this);
}

HERROR OplFileOperToken::init(const HCHAR *pszExtFilter)
{
	if (pszExtFilter)
		HxSTD_StrNCpy( m_szFilter, pszExtFilter, sizeof(m_szFilter)-1);

	if (m_ulHndId)
		APK_FILEMGR_GROUP_Release( m_ulHndId );

	m_ulHndId = APK_FILEMGR_GROUP_Create( m_szFilter );
	m_nReqId = 0;
	if ( 0 == m_ulHndId )
		return ERR_FAIL;

	return ERR_OK;
}

HERROR OplFileOperToken::addPath(const HCHAR *pszPath)
{
	return APK_FILEMGR_GROUP_AddPath( m_ulHndId, pszPath );
}

HERROR OplFileOperToken::removePath(const HCHAR *pszPath)
{
	return APK_FILEMGR_GROUP_RemovePath( m_ulHndId, pszPath );
}

HERROR OplFileOperToken::copy(const HCHAR *pszPath)
{
	HERROR hErr = ERR_FAIL;
	m_eOperCmd = eOPL_CMD_COPY;
	hErr = APK_FILEMGR_GROUP_Copy( m_ulHndId, pszPath, &m_nReqId );
	if ( ERR_OK == hErr )
	{
		m_eState = (eOplFileManagerState)(0x0000FFFF & (eOplFileManagerState)(OplFileManager::getBaseIndex(m_eOperCmd)));
		OplFileManager::send_NotifyMessage(m_nReqId, m_eState);
		APK_FILEMGR_AddListener(m_nReqId, NULL, OplFileOperToken::umma_callback);
	}
	return m_nReqId;
}

HERROR OplFileOperToken::move(const HCHAR *pszPath)
{
	HERROR hErr = ERR_FAIL;
	m_eOperCmd = eOPL_CMD_MOVE;
	hErr = APK_FILEMGR_GROUP_Move( m_ulHndId, pszPath, &m_nReqId );
	if ( ERR_OK == hErr )
	{
		m_eState = (eOplFileManagerState)(0x0000FFFF & (eOplFileManagerState)(OplFileManager::getBaseIndex(m_eOperCmd)));
		OplFileManager::send_NotifyMessage(m_nReqId, m_eState);
		APK_FILEMGR_AddListener(m_nReqId, NULL, OplFileOperToken::umma_callback);
	}
	return m_nReqId;
}

HERROR OplFileOperToken::remove(void)
{
	HERROR hErr = ERR_FAIL;
	m_eOperCmd = eOPL_CMD_REMOVE;
	hErr = APK_FILEMGR_GROUP_Remove( m_ulHndId, &m_nReqId );
	if ( ERR_OK == hErr )
	{
		m_eState = (eOplFileManagerState)(0x0000FFFF & (eOplFileManagerState)(OplFileManager::getBaseIndex(m_eOperCmd)));
		OplFileManager::send_NotifyMessage(m_nReqId, m_eState);
		APK_FILEMGR_AddListener(m_nReqId, NULL, OplFileOperToken::umma_callback);
	}
	return m_nReqId;
}

HERROR OplFileOperToken::cancel(void)
{
	HERROR hErr = ERR_FAIL;
	m_eOperCmd = eOPL_CMD_STOP;
	hErr = APK_FILEMGR_GROUP_Cancel( m_ulHndId, &m_nReqId );
	if ( ERR_OK == hErr )
	{
		m_eState = (eOplFileManagerState)(0x0000FFFF & (eOplFileManagerState)(OplFileManager::getBaseIndex(m_eOperCmd)));
		APK_FILEMGR_AddListener(m_nReqId, NULL, OplFileOperToken::umma_callback);
	}
	return m_nReqId;
}

OplFileOperToken *OplFileOperToken::getInstance( HINT32 reqId )
{
	std::list<OplFileOperToken *>::iterator it;
	OplFileOperToken *pOperToken = NULL;
	for ( it = m_instanceList.begin(); it != m_instanceList.end(); it++ )
	{
		if ( reqId == (*it)->getRequestId() )
		{
			return pOperToken = *it;
		}
	}
	return NULL;
}

void OplFileOperToken::onCallback(HINT32 nHandleId, HINT32 eEvent, HINT32 nError, void *pvData )
{

}

void OplFileOperToken::umma_callback(HINT32 nRequestId, void *pvUserData, HxList_t *pstResultList, void *pvCbInfo)
{
	AxFileMgrCbInfo_t	*pstInfo = (AxFileMgrCbInfo_t*)pvCbInfo;
	eOplOperationCommand eCmd;
	ENTRY;

	if( 0 == nRequestId )
		return ;

	HxLOG_Debug ( "[OPL_FILEOPER_TOKEN] nReqId(%d), eState(%d), [%d/%d] \n", nRequestId, pstInfo->eState, pstInfo->nDoneOperationNum, pstInfo->nTotalOperationNum);
	if (pstInfo->nDoneOperationNum < pstInfo->nTotalOperationNum)
	{
		HxLOG_Debug("[OPL_FILE_OPERTOKEN] OperToekn Job is running [%d/%d]\n",
						pstInfo->nDoneOperationNum, pstInfo->nTotalOperationNum);
		return;
	}

	OplFileOperToken *pOperToken = OplFileOperToken::getInstance( nRequestId );
	if ( NULL == pOperToken )
		return ;

	eCmd = pOperToken->getCommand();

	eOplFileManagerState state 	= eOPL_FILEMGR_STATE_IDLE;
	switch(pstInfo->eState)
	{
		case eAxFILEMGR_ERR_OK :
			if ( 0 > pstInfo->nTotalOperationNum )
			{
				state = (eOplFileManagerState)(0x0000FFFF & (eOplFileManagerState)(OplFileManager::getBaseIndex(eCmd)));
				/* start operation  : don't send event!! */
				return;
			}
			else
			{
				/* completed operation */
				state = (eOplFileManagerState)(0x0000FFFF & ((eOplFileManagerState)(OplFileManager::getBaseIndex(eCmd) + eOPL_COMPLETE_INDEX)));
			}
			break;
		case eAxFILEMGR_ERR_NOT_ENOUGH_SPACE:
			state = eOPL_FILEMGR_STATE_LACK_SPACE;
			break;
		case eAxFILEMGR_ERR_4G_FAT:
			state = eOPL_FILEMGR_STATE_4G_FAT32;
			break;
		case eAxFILEMGR_ERR_FAIL_STAT:
			state = eOPL_FILEMGR_STATE_DEVICE_INFO_ERROR;
			break;
		case eAxFILEMGR_ERR_ONGOING :
			HxLOG_Debug("[OPL_FILE_OPERTOKEN] ERROR_ONGING [%d/%d]\n",
					pstInfo->nDoneOperationNum, pstInfo->nTotalOperationNum);
			state = (eOplFileManagerState)(0x0000FFFF & ((eOplFileManagerState)(OplFileManager::getBaseIndex(eCmd) + eOPL_ERROR_INDEX)));
			break;
		case eAxFILEMGR_ERR_ERR :
		case eAxFILEMGR_ERR_CANCELED :
		default:
			state = (eOplFileManagerState)(0x0000FFFF & ((eOplFileManagerState)(OplFileManager::getBaseIndex(eCmd) + eOPL_ERROR_INDEX)));
			break;
	}
	HxLOG_Print ( "[OPL_FILEOPER_TOKEN] nReqId(%d), eCmd(%d), state[%d] \n", nRequestId, eCmd, state );
	pOperToken->setState( eOPL_FILEMGR_STATE_IDLE );
	OplFileManager::send_NotifyMessage(nRequestId, state);
}



/* EOF */

