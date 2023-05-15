/**************************************************************
*	@file	pl_dms_scan_media.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			DLNA / DMS
**************************************************************/

/*
* ⓒ 2011-2012 Humax Co., Ltd.
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
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

#define	________________Header_Files_______________________________
#include "pl_dms.h"
#include "pl_fs.h"
#include "pl_common.h"
#include "pl_pvr.h"
#include "homma_system.h"

#include <clib.h>
#include <dlib.h>
#include <thapi.h>
#include <dapi.h>

#include <mxlib_config.h>
#include <mxkernel.h>
#include <mxdlna.h>
#include <mxdlna_dms.h>
#include <mxdlna_diskscan.h>
#include <mxdbc.h>
#include <mxdetect_define.h>
#include <mxmedia.h>
#include <mxmedia_define.h>
#include <mxfile_user.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#if 0
#define __PL_DMS_SUPPORT_LIVE_STREAM__				0 /* apk_v2 작업 이후 지원 */
#endif

#if defined(CONFIG_HOMMA_CDI)
#define __PL_DMS_SUPPORT_FS_SECURE__			1
#define __PL_DMS_SUPPORT_FS_CHUNK__				1
#else
#define __PL_DMS_SUPPORT_FS_SECURE__			0
#define __PL_DMS_SUPPORT_FS_CHUNK__				1 /* Supported plain chunk-io under non-crypt PVR record */
#endif

#define PL_DMS_DEFAULT_SCAN_COUNT_UNLIMITED		( 0 )
#define PL_DMS_DEFAULT_SCAN_RESCAN_INTERVAL		( 4 * 60 * 60 * 1000 ) /* msec; HMS PH#2: Agreed to extend scan intv to 4~8 hours */
#define PL_DMS_DEFAULT_SCAN_ITEM_INTERVAL		( 100 ) /* ms */
#define PL_DMS_DEFAULT_SCAN_BACKUP_PRESERVATION_TIME		( 7 * 24 ) /* hours */

#define PL_DMS_DEFAULT_CREATOR_NAME				"HUMAX"
#define PL_DMS_FILE_TS_EXT						".ts"
#define PL_DMS_FILE_JMETA_EXT					".hjm"
#define PL_DMS_MAX_TITLE_LEN					128

#define PL_DMS_THUMBNAIL_EXTRACT_TIME           7 /*sec*/
#define PL_DMS_DEFAULT_THUMBNAIL_PATH           NULL

typedef enum {
	PL_MEDIA_RECORD_IO_TYPE_NA=0,
	PL_MEDIA_RECORD_IO_TYPE_SINGLE,		/* Single record */
	PL_MEDIA_RECORD_IO_TYPE_DELAYED		/* Delayed record(Nexus Chunked IO) */
} PL_DMS_RECORD_IO_TYPE_e;

typedef enum { /* SDT Service Type in en_300468 */
	PL_MEDIA_RECORD_SVC_TYPE_NA=0,
	PL_MEDIA_RECORD_SVC_TYPE_TV,		/* eDxSVC_TYPE_TV */
	PL_MEDIA_RECORD_SVC_TYPE_RADIO		/* eDxSVC_TYPE_RADIO */
} PL_DMS_RECORD_SVC_TYPE_e;

typedef enum
{
	ePL_DMS_ScanError_None                      = 0x00000000,
	ePL_DMS_ScanError_OpenError                 = 0x00000001,
	ePL_DMS_ScanError_InvalidFile               = 0x00000002,
	ePL_DMS_ScanError_Unsupported               = 0x00000004,
	ePL_DMS_ScanError_InvalidRecordMeta         = 0x00000008,
	ePL_DMS_ScanError_InvalidRecordTitle        = 0x00000010,
	ePL_DMS_ScanError_InvalidRecordStatus       = 0x00000020,
	ePL_DMS_ScanError_UnsupportedRecordStatus   = 0x00000040,
	ePL_DMS_ScanError_UnsupportedCAS            = 0x00000080,
	ePL_DMS_ScanError_UnsupportedBlock          = 0x00000100,
	ePL_DMS_ScanError_UnsupportedLock           = 0x00000200,
	ePL_DMS_ScanError_UnsupportedDRM            = 0x00000400,
	ePL_DMS_ScanError_UnsupportedFileType       = 0x00000800,
	ePL_DMS_ScanError_DelayedDirectory          = 0x00001000,
	ePL_DMS_ScanError_Max                       = 0x10000000
} PL_DMS_ScanError_e;

/* CDS상에 표기할 Default 표기명을 정의할 구조체 */
typedef struct
{
	HCHAR		*pRootFolderTitle;
	HCHAR		*pUnKnown;
	HCHAR		*pUnKnownYear;
	HCHAR		*pUnKnownDate;
	HCHAR		*pUnKnownMonth;
	HCHAR		*pUnKnownDay;
} PL_DMS_SysFolderArg_t;

#define ________________Static_Variables___________________________
static 	PL_DMS_MXDLNA_HANDLE_t	pDmsHandles;

#define ________________Internal_Functions_________________________
static HBOOL	__pl_dms_isRecordingsFolder(HCHAR *pPathName);
static HCHAR*	__pl_dms_media_GetCompatibleLiveTvAppDevName(DxStorage_DevType_e eDevType);


/* ProtocolInfo의 기본값을 세팅 */
void __pl_dms_media_util_SetDefaultProtocolInfo( PL_DMS_PROTOCOLINFO_t *pProtocolInfo )
{
	if( pProtocolInfo == NULL )
	{
		return;
	}

	pProtocolInfo->SupportsTimeBasedSeek = DMS_SUPPROTS_TIME_BASED_SEEK;
	pProtocolInfo->SupportsByteBasedSeek = DMS_SUPPROTS_BYTE_BASED_SEEK;

	pProtocolInfo->Profile = NULL;
	pProtocolInfo->MimeType = NULL;

	pProtocolInfo->SupportedPlaySpeeds = NULL;
	pProtocolInfo->SupportedPlaySpeedsLength = 0;

	pProtocolInfo->IsConvertedContent = 0;
	pProtocolInfo->Protocol = MXDLNA_ProtocolType_HTTP;
	pProtocolInfo->IsPlaySingleUri = 0;

	pProtocolInfo->SenderPaced = 0;
	pProtocolInfo->LimitedOperations_TimeBasedSeek = 0;
	pProtocolInfo->LimitedOperations_ByteBasedSeek = 0;

	pProtocolInfo->DLNAPlayContainer = 0;
	pProtocolInfo->S0_Increasing = 0;
	pProtocolInfo->SN_Increasing = 0;

	pProtocolInfo->RTSP_Pause = DMS_SUPPORTS_RTSP_PAUSE;
	pProtocolInfo->HTTP_Stalling = DMS_SUPPORTS_HTTP_STALLING;

	pProtocolInfo->TM_S = 0;
	pProtocolInfo->TM_I = 0;
	pProtocolInfo->TM_B = DMS_SUPPORTS_HTTP_STALLING;

	pProtocolInfo->MaxSpeed_Major = 0;
	pProtocolInfo->MaxSpeed_Minor = 0;

	pProtocolInfo->LinkProtection = 0;
	pProtocolInfo->ClearText = 0;

	pProtocolInfo->DLNA_Major_Version = DLNA_DMS_MAJOR_VERSION;
	pProtocolInfo->DLNA_Minor_Version = DLNA_DMS_MINOR_VERSION;

	pProtocolInfo->reserved = NULL;
	pProtocolInfo->reserved2 = NULL;
}

/* 이 부분은 아직 Base에 Language Util이 적용되지 않아 문제가 되어 추가한 부분으로 향후 Util이 적용되면 비교해서 삭제해야함 */
HUINT32		__pl_dms_media_UCS2Strlen(const HUINT16 *pusStr)
{
	const HUINT16	*pusIdx = pusStr;

	/* check parameter */
	if((pusStr == NULL) || (*pusStr == 0x0000))		{ return 0; }

	/* following text */
	for (/*none*/; *pusIdx != 0x0000; ++pusIdx)/* nothing */;

	/* buffer의 크기임 */
	return (pusIdx - pusStr);
}

HUINT32	__pl_dms_media_ConvUCS2ToUtf8(const CH_UCS2 strUcs2, const HINT32 nUtf8BufferLen, HINT32 *pnUcs2Utf8Len, CH_UTF8 *pUcs2Utf8)
{
	HUINT16 count = 0;
	HUINT16	ch = strUcs2;

	if (ch < 0x80)
		count = 1;
	else if (ch < 0x800)
		count = 2;
	else //if (ch < 0x10000)
		count = 3;
#if 0
	else
		return ERR_ILLEGAL_SEQ;
#endif

	if (nUtf8BufferLen < count)
		return ERR_BUFFER_TOOSMALL;

	switch (count) { /* note: code falls through cases! */
		case 3: pUcs2Utf8[2] = 0x80 | (ch & 0x3f); ch = ch >> 6; ch |= 0x800;
		case 2: pUcs2Utf8[1] = 0x80 | (ch & 0x3f); ch = ch >> 6; ch |= 0xc0;
		case 1: pUcs2Utf8[0] = ch;
	}

	*pnUcs2Utf8Len = count;

	return ERR_OK;
}


HINT32	__pl_dms_media_ConvertUCS2ToUtf8(const CH_UCS2 *pStrUcs2, const HINT32 nStrUcs2Len, const HINT32 nBufferLen, HINT32 *pnUsedByte, CH_UTF8 *pUtf8Str, HINT32 *pnUtf8StrLen)
{
	HINT32		i = 0;
	HINT32		nConvertedCharacterCount = 0;

	HINT32	resultErr = ERR_FAIL;

	*pnUsedByte = 0;
	*pnUtf8StrLen = 0;

	for(i = 0; i < nStrUcs2Len; i++)
	{
		nConvertedCharacterCount = 0;

		resultErr = __pl_dms_media_ConvUCS2ToUtf8(pStrUcs2[i], nBufferLen-(*pnUtf8StrLen), &nConvertedCharacterCount, pUtf8Str+(*pnUtf8StrLen));
		if(resultErr == ERR_BUFFER_TOOSMALL)
		{
			/* 버퍼가 모자라다, 변환 마침. */
			break;
		}
		else if(resultErr != ERR_OK)
		{
			continue;
		}
		else
		{
			/* nothing to do */
		}

		/* 누적되면 변환된 utf8 스트링 길이 */
		*pnUtf8StrLen += nConvertedCharacterCount;
	}

	*pnUsedByte = i;

	return resultErr;
}


STATIC HINT32 __pl_dms_media_GetFileSize( MXFILE_DESC *pAccDesc, HINT64 *pSize )
{
	HUINT32 nRet = ERR_OK;
	MXFILE_HANDLE hFile = NULL;

	if( mxFile_openFile( pAccDesc, &hFile, NULL) != 0 )
	{
		return ERR_FAIL;
	}

	(void)mxFile_getFilesize( hFile, pSize );
	(void)mxFile_closeFile( hFile );

	return nRet;
}

STATIC HUINT64	__pl_dms_media_GetLocalFileSize( HCHAR *path )
{
	PL_FS_FileStat_t	filestatp;
	HxSTD_MemSet( &filestatp, 0x00, sizeof( PL_FS_FileStat_t ) );
	PL_FS_GetFileInfo( path, PL_FS_FILE_TYPE_FILE, &filestatp );
	return filestatp.ulFileSize;
}


/* Define Detachable-HDD path, title */
#define SVC_DMS_DETACHABLE_MEDIA_PATH			"/media/ehd1"
#define SVC_DMS_DETACHABLE_MEDIA_TITLE			"Detachable Media"
#define SVC_DMS_DETACHABLE_RECORDINGS_PATH		"/media/ehd1/.recordings"
#define SVC_DMS_DETACHABLE_RECORDINGS_TITLE		"Detachable Recordings"

/* Define USB-HDD path, title */
#define SVC_DMS_USB_MEDIA_PATH_PREFIX			"/media/driver"
#define SVC_DMS_USB_MEDIA_TITLE_PREFIX			"USB"
#define SVC_DMS_USB_MEDIA_TITLE					"USB Media"
#define SVC_DMS_USB_RECORDINGS_TITLE			"USB Recordings"

#define SVC_DMS_HIDDEN_RECORDINGS				".recordings"
#define SVC_DMS_RECORDINGS_TITLE				"Recordings"

// 2015/12/04 mhkang: HDD Root 경로 이름을 HDD 종류에 맞게 설정한다(Internal HDD, Detachable HDD, USB HDD, eSATA HDD, ...)
STATIC void __pl_dms_media_RenameSpecialDirectory( HCHAR *pPathName, HCHAR *pszTitle, HINT32 nTitleSize )
{
	HCHAR* szDevType;
	DxStorage_DevType_e eDevType;

	if( pPathName == NULL || pszTitle == NULL || nTitleSize <= 0 )
	{
		return;
	}

	// pPathName이 MountPath와 동일할 때만 HDD DevType String이 넘어온다. 그 외에는 기타 경로이기 때문에 Title을 변경하지 않는다.
	szDevType = HOMMA_SYSTEM_GetDeviceTypeStringWithMounthPath(pPathName, TRUE);
	if (szDevType != NULL)
	{
		HxSTD_snprintf( pszTitle, nTitleSize, "%s", szDevType );
		HLIB_STD_MemFree(szDevType);
		return;
	}

	if ( __pl_dms_isRecordingsFolder(pPathName) == TRUE)
	{
		// 2016/1/24 mhkang:
		eDevType = HOMMA_SYSTEM_GetDeviceTypeWithMounthPath(pPathName, FALSE);
		HxSTD_snprintf( pszTitle, nTitleSize, "%s", __pl_dms_media_GetCompatibleLiveTvAppDevName(eDevType));

		HxLOG_Info(HxANSI_COLOR_YELLOW("[%s:%d] PathName:%s Title:%s\n"), __FUNCTION__, __LINE__, pPathName, pszTitle);
		return;
	}
}

HCHAR* __pl_dms_media_GetCompatibleLiveTvAppDevName(DxStorage_DevType_e eDevType)
{
	switch(eDevType)
	{
	case eDxSTORAGE_DEVTYPE_INTHDD:		return SVC_DMS_RECORDINGS_TITLE;
	case eDxSTORAGE_DEVTYPE_DETACHABLE:	return SVC_DMS_DETACHABLE_RECORDINGS_TITLE;
	case eDxSTORAGE_DEVTYPE_ESATAHDD:	return SVC_DMS_DETACHABLE_RECORDINGS_TITLE;
	case eDxSTORAGE_DEVTYPE_USBHDD:		return SVC_DMS_USB_RECORDINGS_TITLE;
	default: return SVC_DMS_RECORDINGS_TITLE;
	}
}

static HBOOL __pl_dms_isRecordingsFolder(HCHAR *pPathName)
{
	DxStorage_DevType_e eDevType = eDxSTORAGE_DEVTYPE_NONE;

	if( pPathName == NULL) return FALSE;

	eDevType = HOMMA_SYSTEM_GetDeviceTypeWithMounthPath(pPathName, FALSE);
	if (eDevType == eDxSTORAGE_DEVTYPE_NONE)
	{
		return FALSE;
	}
	else if (eDevType == eDxSTORAGE_DEVTYPE_INTHDD)
	{
		// in the case of internall hdd
		if ( strstr(pPathName, SVC_DMS_RECORDINGS_TITLE) != NULL )
		{
			return TRUE;
		}
	}
	else
	{
		if (strstr(pPathName, SVC_DMS_HIDDEN_RECORDINGS) != NULL)
		{
			return TRUE;
		}
	}

	return FALSE;
}

STATIC HINT32 __pl_dms_media_GetFileList( HCHAR *pDirPath, HCHAR *pPattern, HCHAR **ppFileList, HINT32 nBuffCnt )
{
	DIR *pDir 				= NULL;
	struct dirent *pEnt 		= NULL;
	HINT32 i 				= 0;
	HCHAR szFileName[ 2048 ]	= { 0, };

	pDir = opendir( pDirPath );
	if( pDir != NULL )
	{
		while( ( pEnt = readdir( pDir ) ) )
		{
			if( HxSTD_StrStr( pEnt->d_name, pPattern ) != NULL )
			{
				HxSTD_snprintf( szFileName, sizeof( szFileName ), "%s/%s", pDirPath, pEnt->d_name );
				ppFileList[ i++ ] = HLIB_STD_StrDup( szFileName );
				if( i >= nBuffCnt )
				{
					break;
				}
			}
		}

		(void)closedir( pDir );
	}
	else
	{
		/* Couldn't Open the directory. */
	}

	return i;
}

STATIC HINT32 __pl_dms_media_MakeResource( MXDLNA_DMS_ADDITEM_PROP *pAddData, MXMEDIA_INFO *pMediaInfo,
	MXMETA_INFO  * pMetaInfo, MXCODEC_INFO * pCodecInfo, HINT8 *pcLocation, HINT8 *pcLocationTemp,
	HINT8 *pcUserData, HBOOL bUseCodecInfo )
{
	MXDLNA_DMS_RESOURCE *localRes = NULL;
	MXDLNA_PROTOCOLINFO protocolInfo = { 0, };

	HCHAR		*pMimeType = NULL;
	HLONG 		lValue = 0;
	HINT8 		*pPNStr = NULL;

	HINT32 		nMediaType = 0;
	HINT32 		nAudioCnt = -1;
	HINT32 		nVideoCnt = -1;
	HLONG 		lAudioDuration = 0;
	HLONG 		lVideoDuration = 0;

	HxLOG_Print( "[PL_DMS] __pl_dms_media_MakeResource() +++++++++\n" );

	if (pMediaInfo == NULL) /* codecInfo can be have NULL value */
	{
		HxLOG_Error("[PL_DMS] Error> invalid resource arg!\n");
		goto func_error;
	}

	if ((bUseCodecInfo == TRUE) && (pCodecInfo != NULL))
	{
		HxLOG_Print( "[PL_DMS] __pl_dms_media_MakeResource : FileSize = %lld\n", pCodecInfo->filesize );
	}
	else
	{
		HxLOG_Print( "[PL_DMS] __pl_dms_media_MakeResource : Codec is NULL\n" );
	}

	pMimeType = pAddData->mimeType;
	if( HxSTD_StrCmp( pMimeType, "application/octet-stream" ) == 0 )
	{
		pMimeType = "*";
	}

	/* Get PN Info */
	if( mxMedia_getPNInfo( pMediaInfo, pCodecInfo, pMimeType, PL_DMS_DETECT_CMD_MIME, &lValue ) == ERR_OK )
	{
		pPNStr = mxMedia_getPNStr( pMediaInfo->media_format, lValue );
		if( pPNStr != NULL )
		{
			if ( HxSTD_StrCmp( pPNStr ,"MP_UNKNOWN") == 0 || HxSTD_StrCmp( pPNStr ,"ERROR") == 0 )
			{
				pPNStr = NULL;
			}
		}
	}

	/* Protocol Info의 기본값 세팅 */
	__pl_dms_media_util_SetDefaultProtocolInfo( (PL_DMS_PROTOCOLINFO_t *)&protocolInfo );

	localRes = ( MXDLNA_DMS_RESOURCE * )HLIB_STD_MemAlloc( sizeof( MXDLNA_DMS_RESOURCE ) );
	HxSTD_MemSet( localRes, 0x00, sizeof( MXDLNA_DMS_RESOURCE ) );

	/* DTCP/IP related information -> always none for media files */
	protocolInfo.LinkProtection = FALSE;
	protocolInfo.ClearText = FALSE;
	localRes->dtcp_emi = 0;
	localRes->dtcp_pcplength = 0;

	/* ProtocolInfo 세팅 - 필요한것들만... */
	protocolInfo.MimeType = pMimeType;
	protocolInfo.Profile = pPNStr;
	protocolInfo.Protocol = 1;

#if defined(CONFIG_MW_MM_DLNA_DMS_JAPAN)
	pl_dms_MakeResourceForJapan(pcLocation, pAddData, &protocolInfo, localRes);
#endif

	/******************************************************************/
	/* Resource Setting */
	/******************************************************************/
	nMediaType = PL_DMS_MASK_MEDIATYPE( pMediaInfo->media_format );
	if( ( nMediaType == PL_DMS_TYPE_AUDIO ) || ( nMediaType == PL_DMS_TYPE_VIDEO ) )
	{
		/* A/V Setting */
		protocolInfo.TM_S = 1;

		if( pCodecInfo )
		{
			nAudioCnt = pCodecInfo->media.av.audioCnt;
			nVideoCnt = pCodecInfo->media.av.videoCnt;
			/* File Size */
			localRes->size = pCodecInfo->filesize;
		}

		/* Audio Resource Setting */
		if(nAudioCnt > 0)
		{
			if(pCodecInfo->media.av.audio[0].bitrate <= 0)
			{
				localRes->bitrate = -1;
			}
			else
			{
				localRes->bitrate = pCodecInfo->media.av.audio[0].bitrate;
			}

			if(pCodecInfo->media.av.audio[0].bitPerSample <= 0)
			{
				localRes->bitPerSample = -1;
			}
			else
			{
				localRes->bitPerSample = pCodecInfo->media.av.audio[0].bitPerSample;
			}

			/* Workaround : SQE 3Gb가 넘는 mp3 에 대하여 duration 오류 보정 - 1Gb가 넘을경우 처리 */
			if( ( localRes->size > 1024 * 1024 * 1024 ) && ( localRes->bitrate > 0 ) && ( nMediaType == PL_DMS_TYPE_AUDIO ) )
			{
				lAudioDuration = localRes->size * 8 / localRes->bitrate;
			}
			else
			{
				if(pCodecInfo->media.av.audio[0].duration <= 0)
				{
					pCodecInfo->media.av.audio[0].duration = -1;
				}
				else
				{
					lAudioDuration = pCodecInfo->media.av.audio[0].duration; // sec, not msec
				}
			}

			if(pCodecInfo->media.av.audio[0].channel <= 0)
			{
				localRes->nrAudioChannels = -1;
			}
			else
			{
				localRes->nrAudioChannels = pCodecInfo->media.av.audio[0].channel;
			}

			if(pCodecInfo->media.av.audio[0].frequency <= 0)
			{
				localRes->sampleFrequency = -1;
			}
			else
			{
				localRes->sampleFrequency = pCodecInfo->media.av.audio[0].frequency;
			}
		}

		/* Video Resource Setting */
		if ( nVideoCnt > 0 )
		{
			if(pCodecInfo->media.av.video[0].duration <= 0)
			{
				lVideoDuration = -1;
			}
			else
			{
				lVideoDuration = pCodecInfo->media.av.video[0].duration; // sec, not msec
			}

			if(pCodecInfo->media.av.video[0].width > 0 && pCodecInfo->media.av.video[0].height > 0)
			{
				localRes->resolutionX = pCodecInfo->media.av.video[0].width;
				localRes->resolutionY = pCodecInfo->media.av.video[0].height;
			}
			else
			{
				localRes->resolutionX = -1;
				localRes->resolutionY = -1;
			}

			if( pCodecInfo->media.av.video[0].Bitrate <= 0 )
			{
				localRes->bitrate = -1;
			}
			else
			{
				localRes->bitrate = pCodecInfo->media.av.video[0].Bitrate;
			}
		}

		/* Duration Setting */
		/* Caution to time unit: res->duration has sec unit, not msec */
		localRes->duration = ( lAudioDuration > lVideoDuration ) ? lAudioDuration : lVideoDuration;

		HxLOG_Print( "[PL DMS] MAKE_RESOURCE duration (%d)\n", localRes->duration );
	}
	else if( nMediaType == PL_DMS_TYPE_IMAGE )
	{
		/* Image Setting */
		protocolInfo.TM_I = 1;
		if  ( pCodecInfo && pCodecInfo->media.image.width > 0  && pCodecInfo->media.image.height > 0 )
		{
			localRes->resolutionX = pCodecInfo->media.image.width;
			localRes->resolutionY = pCodecInfo->media.image.height;
			localRes->size = pCodecInfo->filesize;
		}
		else
		{
			localRes->resolutionX = -1;
			localRes->resolutionY = -1;
		}
	}

	if ( !pCodecInfo || nAudioCnt < 0 )
	{
		if ( nAudioCnt < 0 )
		{
			localRes->bitrate = -1;
			localRes->bitPerSample = -1;
			localRes->nrAudioChannels = -1;
			localRes->sampleFrequency = -1;
		}

		if ( !localRes->size )
		{
			localRes->size = -1;
		}
	}

#if 1
	/* 이것은 필요한 부분인지..  겹치는 듯 한데 일단 둠 */
	if(localRes->size <= 0)
	{
		MXFILE_DESC desc = { 0, };
		desc.url = pcLocation ;
		desc.linear_access = 0 ;
		desc.cancel_op = NULL ;

		localRes->size = __pl_dms_media_GetLocalFileSize( desc.url );
		if( localRes->size == 0 )
			(void)__pl_dms_media_GetFileSize( &desc, &localRes->size );

		if(localRes->size <= 0)
		{
			localRes->size = -1;
		}
	}
#endif

	if(pcLocationTemp != NULL)
	{
		(void)mxDlnaDms_checkedDuplicateString(&localRes->contentURI, pcLocationTemp);
	}

	if(pMimeType != NULL)
	{
		(void)mxDlnaDms_checkedDuplicateString(&localRes->mimeType, pMimeType);
	}

	if(pcUserData != NULL)
	{
		(void)mxDlnaDms_checkedDuplicateString(&localRes->userData, pcUserData);
	}

	localRes->protocol = mxDlna_protocolinfoSerialize( &protocolInfo );
	localRes->colorDepth = -1;

	(void)mxDlnaDmsDb_addResProp( pAddData , localRes );

#if 0
	{
		HxLOG_Print( "[RES] contentURI : %s\n", localRes->contentURI);
		HxLOG_Print( "[RES] importURI  : %s\n", localRes->importURI);
		HxLOG_Print( "[RES] ifoFileUri  : %s\n", localRes->ifoFileUri);
		HxLOG_Print( "[RES] importIfoFileUri  : %s\n", localRes->importIfoFileUri);
		HxLOG_Print( "[RES] mimeType  : %s\n", localRes->mimeType);
		HxLOG_Print( "[RES] size  : %d\n", localRes->size);
		HxLOG_Print( "[RES] bitrate  : %ld\n", localRes->bitrate);
		HxLOG_Print( "[RES] bitPerSample  : %ld\n", localRes->bitPerSample);
		HxLOG_Print( "[RES] colorDepth  : %ld\n", localRes->colorDepth);
		HxLOG_Print( "[RES] nrAudioChannels  : %ld\n", localRes->nrAudioChannels);
		HxLOG_Print( "[RES] protection  : %s\n", localRes->protection);
		HxLOG_Print( "[RES] resolutionX  : %ld\n", localRes->resolutionX);
		HxLOG_Print( "[RES] resolutionY  : %ld\n", localRes->resolutionY);
		HxLOG_Print( "[RES] sampleFrequency  : %ld\n", localRes->sampleFrequency);
		HxLOG_Print( "[RES] userData  : %s\n", localRes->userData);
		HxLOG_Print( "[RES] dtcp_emi  : %d\n", localRes->dtcp_emi);
		HxLOG_Print( "[RES] dtcp_pcplength  : %ld\n", localRes->dtcp_pcplength);
		HxLOG_Print( "[RES] resAddType  : %d\n", localRes->resAddType);
	}
#endif

	HxLOG_Print( "[PL_DMS] __pl_dms_media_MakeResource() ---------\n" );
	return TRUE;

func_error:
	HxLOG_Print( "[PL_DMS] __pl_dms_media_MakeResource() ---------\n" );
	return FALSE;
}


STATIC HINT32 __pl_dms_media_SubtitleCheckNAdd( MXDLNA_DMS_ADDITEM_PROP *pAddData , HINT32 mediaFormat , const HCHAR *location )
{
	HCHAR szFilename[ 1024 ] 		= { 0, };
	HCHAR szTitle[ 1024 ] 			= { 0, };
	HCHAR szFilter[ 1024 + 3 ] 		= { 0, };

	HCHAR *pPathPos 				= NULL;
	HCHAR *pExtPos					= NULL;

	HINT32 nFileListCount 			= 0;
	HCHAR *ppFileListX[ 10 ]		= { 0, };
	HINT32 nBufferCount 			= 10;

	HCHAR *pMime 					= NULL;

	MXDLNA_DMS_RESOURCE *localRes 	= NULL;
	MXFILE_DESC stDesc	= { 0, };

	HINT32 i 						= 0;
	HCHAR *path_UTF8X 				= NULL;

	HINT32 nRet						= 0;

	HxSTD_snprintf( szFilename, sizeof( szFilename ), "%s", location );
	pPathPos = HxSTD_StrRChar( szFilename, '/' );
	if( pPathPos == NULL )
	{
		return ERR_FAIL;
	}

	HxSTD_snprintf( szTitle, sizeof( szTitle ), "%s", pPathPos + 1 );
	pExtPos = HxSTD_StrRChar( szTitle, '.' );
	if( pExtPos )
	{
		*( pExtPos + 1 ) ='\0';
	}

	*pPathPos = '\0';

	HxSTD_snprintf( szFilter, sizeof( szFilter ), "%s", szTitle );
	nFileListCount = __pl_dms_media_GetFileList( szFilename , szFilter, ppFileListX, nBufferCount);
	if( nFileListCount <= 0 )
	{
		return ERR_FAIL;
	}

	// 다른 동영상에 자막 리소스 추가
	if ( mediaFormat == PL_DMS_FORMAT_SYSTEM_SMI )
	{
		pMime = mxMedia_getMimetypeByExtension( pAddData->localUrl );

		localRes = ( MXDLNA_DMS_RESOURCE * )HLIB_STD_MemAlloc( sizeof( MXDLNA_DMS_RESOURCE ) );
		HxSTD_MemSet( localRes, 0x00, sizeof( MXDLNA_DMS_RESOURCE ) );

		localRes->resAddType = ePL_DMS_ResType_All;

		stDesc.url = (char*)location ;
		stDesc.linear_access = 0 ;
		stDesc.cancel_op = NULL ;

		localRes->size = __pl_dms_media_GetLocalFileSize( stDesc.url );
		if( localRes->size == 0 )
			(void)__pl_dms_media_GetFileSize( &stDesc, &localRes->size );

		(void)mxDlnaDms_checkedDuplicateString( &localRes->contentURI, pAddData->localUrl );
		(void)mxDlnaDms_checkedDuplicateString( &localRes->mimeType, pMime );
		(void)mxDlnaDms_checkedDuplicateString( &localRes->protocol, "http-get:*:application/smil:*" );

		for ( i = 0 ; i < nFileListCount ; i ++)
		{
			if( ppFileListX[ i ] && HxSTD_StrCmp( ppFileListX[ i ], location ) != 0 ) // SMI에 중복으로 넣을 필요는 없다.
			{
				path_UTF8X = HLIB_STD_StrDup( ppFileListX[ i ] );

				//(void)PAL_DMS_UTIL_AddResource( localRes, path_UTF8X );
				mxDlnaDmsDb_addResource( pDmsHandles.hDms, pDmsHandles.hDmsDB, localRes, path_UTF8X );
			}
		}

		mxDlnaDmsDb_freeResourceInfo( localRes );

		nRet = ERR_OK;
	}
	else if( pAddData->mediaType == PL_DMS_TYPE_VIDEO )
	{
		// 존재하는 자막파일을 비디오 리소스에 추가
		HCHAR *locationUTF8 = NULL;

		// 2. smi,crt ... file search
		HINT32 find_subTitle = 0;

		for( i = 0 ; i < nFileListCount; i++ )
		{
			MXMEDIA_INFO mediaInfo = { 0, };

			(void)mxMedia_getInfoByExtension( ppFileListX[ i ], &mediaInfo );

			if( mediaInfo.media_format == PL_DMS_FORMAT_SYSTEM_SMI )
			{
				(void)mxDlnaDms_checkedDuplicateString(&pAddData->subTitleUri , ppFileListX[ i ] );
				find_subTitle = 1;
				locationUTF8 = pAddData->subTitleUri;
				break;
			}
		}

		if ( find_subTitle )
		{
			pMime = mxMedia_getMimetypeByExtension( locationUTF8 );

			localRes = ( MXDLNA_DMS_RESOURCE * )HLIB_STD_MemAlloc( sizeof( MXDLNA_DMS_RESOURCE) );
			HxSTD_MemSet( localRes, 0x00, sizeof( MXDLNA_DMS_RESOURCE ) );

			stDesc.url = pAddData->subTitleUri ;
			stDesc.linear_access = 0 ;
			stDesc.cancel_op = NULL ;
			(void)__pl_dms_media_GetFileSize( &stDesc, &localRes->size );

			localRes->resAddType = ePL_DMS_ResType_OnlyPhysical;

			(void)mxDlnaDms_checkedDuplicateString( &localRes->contentURI, locationUTF8 );
			(void)mxDlnaDms_checkedDuplicateString( &localRes->mimeType, pMime );
			(void)mxDlnaDms_checkedDuplicateString( &localRes->protocol, "http-get:*:application/smil:*" );
			(void)mxDlnaDmsDb_addResProp( pAddData, localRes );

			nRet = ERR_OK;
		}
		else
		{
			nRet = ERR_FAIL;
		}
	}


	for( i = 0 ; i < nFileListCount; i++ )
	{
		if( ppFileListX[ i ] != NULL )
		{
			HLIB_STD_MemFree( ppFileListX[ i ] );
		}
	}

	return nRet;
}

STATIC HINT32 __pl_dms_media_MakeThumbnailResource( MXDLNA_DMS_ADDITEM_PROP *pAddData , HCHAR *pThumbnailURL )
{
	MXDLNA_DMS_RESOURCE 		*pRes 		= NULL;
	MXFILE_DESC	stDesc 		= { 0, };
	MXMEDIA_INFO 	stMediaInfo = { 0, };
	MXCODEC_INFO 	stCodecInfo = { 0, };
	MXDLNA_PROTOCOLINFO 	protocolInfo = { 0, };

	HCHAR *pMimeType 					= "image/jpeg";
	HCHAR *pPNStr 						= NULL;
	HLONG lPNValue 						= 0;
//	HLONG eDlnaRet 						= 0;

	if( pAddData == NULL || pThumbnailURL == NULL )
	{
		HxLOG_Error("[PL_DMS] Error> invalid thumb/resource arg!\n");
		return 0;
	}

	/* albumart 관련 동작 검토가 필요하여 일단 막음 */
	if( pAddData->mediaType == PL_DMS_TYPE_AUDIO )
	{
		pAddData->albumarturi = HLIB_STD_StrDup( pThumbnailURL );
	}

	pRes = ( MXDLNA_DMS_RESOURCE * )HLIB_STD_MemAlloc( sizeof( MXDLNA_DMS_RESOURCE ) );
	HxSTD_MemSet( pRes, 0x00, sizeof( MXDLNA_DMS_RESOURCE ) );

	stDesc.url 				= pThumbnailURL;
	stDesc.linear_access	= 0;
	stDesc.cancel_op		= NULL;

	stMediaInfo.media_format = PL_DMS_FORMAT_IMAGE_JPG;

	pRes->resAddType = ePL_DMS_ResType_All;

	/* Get Codec Info */
	(void)mxMedia_getCodecInfo( &stDesc, &stMediaInfo, NULL, PL_DMS_DETECT_CMD_ACCESS, &stCodecInfo );

	/* Get PN Info */
	if( mxMedia_getPNInfo( &stMediaInfo, &stCodecInfo, pMimeType , PL_DMS_DETECT_CMD_MIME, &lPNValue ) >= 0 )
	{
		pPNStr = mxMedia_getPNStr( PL_DMS_FORMAT_IMAGE_JPG, lPNValue );
	}

	if( pPNStr != NULL )
	{
		if( ( HxSTD_StrCmp( pPNStr ,"MP_UNKNOWN" ) == 0 ) || ( HxSTD_StrCmp( pPNStr ,"ERROR" ) == 0 ) )
		{
			pPNStr = NULL;
		}
	}

	/* Protocol Info의 기본값 세팅 */
	// 재정의 필요
	__pl_dms_media_util_SetDefaultProtocolInfo( (PL_DMS_PROTOCOLINFO_t *) &protocolInfo );

	protocolInfo.MimeType = pMimeType;
	protocolInfo.Profile = pPNStr;
	protocolInfo.TM_I = 1;
	protocolInfo.IsConvertedContent = 1;

	// =========================================
	// Resource Setting
	// =========================================
	if( ( stCodecInfo.media.image.width > 0 )  && ( stCodecInfo.media.image.height > 0 ) )
	{
		pRes->resolutionX = stCodecInfo.media.image.width;
		pRes->resolutionY = stCodecInfo.media.image.height;
		pRes->size = stCodecInfo.filesize;
	}
	else
	{
		pRes->resolutionX = -1;
		pRes->resolutionY = -1;
	}

	/* 노출하지 않을 값들은 -1로 설정 */
	pRes->bitrate = -1;
	pRes->bitPerSample = -1;
	pRes->duration = -1;
	pRes->nrAudioChannels = -1;
	pRes->sampleFrequency = -1;
	pRes->colorDepth = -1;

	if( !pRes->size )
	{
		pRes->size = -1;
	}

	if( pRes->size <= 0 )
	{
		MXFILE_HANDLE hFile = NULL;

		if (mxFile_openFile( &stDesc, &hFile, NULL) != 0)
		{
			HxLOG_Error("mxFile_openFile failed\n");
		}

		if ( hFile )
		{
			(void)mxFile_getFilesize( hFile, &pRes->size );
			(void)mxFile_closeFile( hFile );
		}

		//(void)__pl_dms_media_GetFileSize( &stDesc, &pRes->size );

		if(pRes->size <= 0)
		{
			pRes->size = -1;
		}
	}

	(void)mxDlnaDms_checkedDuplicateString( &pRes->contentURI ,pThumbnailURL );
	(void)mxDlnaDms_checkedDuplicateString( &pRes->mimeType ,pMimeType );
	pRes->protocol = mxDlna_protocolinfoSerialize( &protocolInfo );

	(void)mxDlnaDmsDb_addResProp( pAddData, pRes );

	return 0;
}

static inline HERROR __pl_dms_media_getFileExtension( const HCHAR *szPath, HCHAR *szExtBuf, HINT32 bufSize )
{
	HCHAR *p = NULL;
	p = HxSTD_StrRChar( szPath, '.' );
	if ( NULL == p )
	{
		return ERR_FAIL;
	}

	HxSTD_StrNCpy( szExtBuf, (p+1), bufSize -1 );

	return ERR_OK;
}

/* DMS의 Category를 만들기 위한 정보 가공 */
PL_DMS_Result_e __pl_dms_media_MakeCategory( MXDLNA_DMS_ADDITEM_PROP *pAddData , MXMEDIA_INFO *pMediaInfo,
								MXMETA_INFO *pMetaInfo, HCHAR *pcShareLocation, HCHAR *pcLocationTemp )
{
	SectionInfo *pSectionInfo 		= NULL;
	HCHAR		*pContentData 		= NULL;
	HINT32 		nRet 				= 0;

	HBOOL		bIsConverted		= FALSE;

	CH_UCS2		data[ 128 ] 		= { 0, };			// 64 is the small ID3tag size
	CH_UTF8		convStr[ 128 ] 		= { 0, };
	HINT32		usedBytes = 0, utf8len = 0;
	HCHAR		*pStr				= NULL;

	HCHAR		*pcRel_Path			= NULL;
	HINT32 		isSharedFolderName 	= 1;

	mxDlnaDmsDb_addCreator( ( MXDLNA_DMS_ADDITEM_PROP * )pAddData, PL_DMS_DEFAULT_CREATOR_NAME );

	pSectionInfo = ( SectionInfo * )pMetaInfo->section;
	while( pSectionInfo )
	{
		bIsConverted = FALSE;

		/* Lib에 UTF8 전달하기 위한 Encoding Process ↓↓↓↓↓↓↓↓↓↓↓*/
		if( (pSectionInfo->TitleCmd == MX_MP3_THUMBNAIL) && (pSectionInfo->TitleCmd == MX_WMA_THUMBNAIL) )
		{
			goto MAKE_CATEGORY_SKIP;
		}
		else
		{
			if( pSectionInfo->EncondingType == MX_ENCODING_UNICODE )
			{
				nRet = mxDlnaDms_isAllSpaceString( pSectionInfo->Content ,pSectionInfo->nContentSize );
				if( nRet == SPACEINFO_NORMALSTR )
				{
					HINT32	nTempSize = 0;

					pContentData = ( HCHAR * )HLIB_STD_MemAlloc( pSectionInfo->nContentSize + 2 );
					if (pContentData == NULL)
					{
						goto MAKE_CATEGORY_SKIP;
					}

					HxSTD_MemSet( pContentData, 0x00, pSectionInfo->nContentSize + 2 );
					HxSTD_MemCopy( pContentData, pSectionInfo->Content, pSectionInfo->nContentSize );
					pContentData[ pSectionInfo->nContentSize ] = '\0';
					pContentData[ pSectionInfo->nContentSize + 1 ] = '\0';

					HxSTD_MemSet( data, 0x00, sizeof( data ) );
					HxSTD_MemSet( convStr, 0x00, sizeof( convStr ) );

					nTempSize = __pl_dms_media_UCS2Strlen( ( HUINT16 * )pContentData ) * sizeof( CH_UCS2 );
					nTempSize = ( sizeof( data ) > nTempSize ) ? nTempSize : sizeof( data );
					HxSTD_MemCopy( data, pContentData, nTempSize );

					__pl_dms_media_ConvertUCS2ToUtf8( data, __pl_dms_media_UCS2Strlen( data ), 128, &usedBytes, convStr, &utf8len );

					bIsConverted = TRUE;
				}
				else
				{
					goto MAKE_CATEGORY_SKIP;
				}

			}
			else if( pSectionInfo->EncondingType == MX_ENCODING_ISO_8859_1 )
			{
				if ( pSectionInfo->nContentSize <= 2 )
				{
					if( pSectionInfo->Content[ 0 ] == '\0' )
					{
						goto MAKE_CATEGORY_SKIP;
					}
				}
				else
				{
					nRet = mxDlnaDms_isAllSpaceString( pSectionInfo->Content ,pSectionInfo->nContentSize );
					if(nRet == SPACEINFO_NORMALSTR)
					{
						pContentData = ( HCHAR * )HLIB_STD_MemAlloc( pSectionInfo->nContentSize + 1 );
						if (pContentData == NULL)
						{
							goto MAKE_CATEGORY_SKIP;
						}

						HxSTD_MemSet( pContentData, 0x00, pSectionInfo->nContentSize + 1 );
						HxSTD_MemCopy( pContentData, pSectionInfo->Content, pSectionInfo->nContentSize );
						pContentData[ pSectionInfo->nContentSize ] = '\0';
					}
					else
					{
						goto MAKE_CATEGORY_SKIP;
					}
				}
			}
			else
			{
				nRet = mxDlnaDms_isAllSpaceString(pSectionInfo->Content, pSectionInfo->nContentSize );
				if( nRet == SPACEINFO_NORMALSTR )
				{
					pContentData = HLIB_STD_StrDup( pSectionInfo->Content );
				}
				else
				{
					goto MAKE_CATEGORY_SKIP;
				}
			}

		}		/* Lib에 UTF8 전달하기 위한 Encoding Process ↑↑↑↑↑↑↑↑↑↑↑*/


		if( bIsConverted )
		{
			pStr = convStr;
		}
		else
		{
			pStr = pContentData;
		}

		if( pMediaInfo->media_format == MF_IMAGE_JPG )
		{
			/* JPG일 경우 EXIF 데이터 추출 */
			switch( pSectionInfo->TitleCmd )
			{
				case MX_IMG_EXIF_CAMERA_DATE:
					{
						mxDlnaDmsDb_setTreeInfo( ( MXDLNA_DMS_ADDITEM_PROP * )pAddData, SYSTEMFILTER_YEAR, pStr );
						break;
					}

				default:
					break;
			}

		}
		else if( pMediaInfo->media_format == MF_AUDIO_MP3 )
		{
			switch( pSectionInfo->TitleCmd )
			{
				/* Some ID3tag data(eg. title, artist, album, genre and date shall be converted to UTF8 to display them properly */

				case MX_MP3_TITLE:
					{ /* 파일 명에서 유추한 타이틀 보다는 속성의 타이틀이 신빙성이 높다 - File Name을 그대로 사용하기 위해 아래 막음 */
						//mxDlnaDmsDb_changeTitle( ( MXDLNA_DMS_ADDITEM_PROP * )pAddData, pStr );
						break;
					}
				case MX_MP3_ARTIST:
					{
						mxDlnaDmsDb_setTreeInfo( ( MXDLNA_DMS_ADDITEM_PROP * )pAddData, SYSTEMFILTER_ARTIST, pStr );
						mxDlnaDmsDb_addCreator( ( MXDLNA_DMS_ADDITEM_PROP * )pAddData, pStr );
						break;
					}
				case MX_MP3_ALBUM:
					{
						mxDlnaDmsDb_setTreeInfo( ( MXDLNA_DMS_ADDITEM_PROP * )pAddData, SYSTEMFILTER_ALBUM, pStr );
						break;
					}
				case MX_MP3_GENRE:
					{
						mxDlnaDmsDb_setTreeInfo( ( MXDLNA_DMS_ADDITEM_PROP * )pAddData, SYSTEMFILTER_GENRE, pStr );
						break;
					}
				case MX_MP3_COMPOSER:
					{
						mxDlnaDmsDb_setTreeInfo( ( MXDLNA_DMS_ADDITEM_PROP * )pAddData, SYSTEMFILTER_COMPOSER, pStr );
						mxDlnaDmsDb_addCreator( ( MXDLNA_DMS_ADDITEM_PROP * )pAddData, pStr );
						break;
					}
				case MX_MP3_PUBLISHER:
					{
						mxDlnaDmsDb_setTreeInfo( ( MXDLNA_DMS_ADDITEM_PROP * )pAddData, SYSTEMFILTER_PUBLISHER, pStr );
						mxDlnaDmsDb_addCreator( ( MXDLNA_DMS_ADDITEM_PROP * )pAddData, pStr );
						break;
					}
				case MX_MP3_TRACK:
					{
						mxDlnaDmsDb_setTreeInfo( ( MXDLNA_DMS_ADDITEM_PROP * )pAddData , SYSTEMFILTER_TRACK, pStr );
						break;
					}
				case MX_MP3_YEAR:
					{
						mxDlnaDmsDb_setTreeInfo( ( MXDLNA_DMS_ADDITEM_PROP * )pAddData , SYSTEMFILTER_YEAR, pStr );
						break;
					}
				default:
					break;
			}
		}
		else if( pMediaInfo->media_format == MF_AUDIO_WMA )
		{
			switch ( pSectionInfo->TitleCmd )
			{
				case MX_WMA_TITLE:
					{ /* 파일 명에서 유추한 타이틀 보다는 속성의 타이틀이 신빙성이 높다 - File Name을 그대로 사용하기 위해 아래 막음 */
						// mxDlnaDmsDb_changeTitle( ( MXDLNA_DMS_ADDITEM_PROP * )pAddData , pStr );
						break;
					}
				case MX_WMA_PROVIDERRATING:
				case MX_WMA_RATING:
					{
						mxDlnaDmsDb_setTreeInfo( ( MXDLNA_DMS_ADDITEM_PROP * )pAddData, SYSTEMFILTER_RATING, pStr );
						break;
					}
				case MX_WMA_YEAR:
					{
						mxDlnaDmsDb_setTreeInfo( ( MXDLNA_DMS_ADDITEM_PROP * )pAddData, SYSTEMFILTER_YEAR,	pStr );
						break;
					}
				case MX_WMA_GENRE:
					{
						mxDlnaDmsDb_setTreeInfo( ( MXDLNA_DMS_ADDITEM_PROP * )pAddData, SYSTEMFILTER_GENRE, pStr );
						break;
					}
				case MX_WMA_COMPOSER:
					{
						mxDlnaDmsDb_setTreeInfo( ( MXDLNA_DMS_ADDITEM_PROP * )pAddData, SYSTEMFILTER_COMPOSER, pStr );
						mxDlnaDmsDb_addCreator( ( MXDLNA_DMS_ADDITEM_PROP * )pAddData, pStr );
						break;
					}
				case MX_WMA_PUBLISHER:
					{
						mxDlnaDmsDb_setTreeInfo( ( MXDLNA_DMS_ADDITEM_PROP * )pAddData, SYSTEMFILTER_PUBLISHER, pStr );
						mxDlnaDmsDb_addCreator( ( MXDLNA_DMS_ADDITEM_PROP * )pAddData, pStr );
						break;
					}
				case MX_WMA_ALBUMTITLE:
					{
						mxDlnaDmsDb_setTreeInfo( ( MXDLNA_DMS_ADDITEM_PROP * )pAddData, SYSTEMFILTER_ALBUM, pStr );
						break;
					}
				case MX_WMA_ALBUMARTIST:
					{
						mxDlnaDmsDb_setTreeInfo( ( MXDLNA_DMS_ADDITEM_PROP * )pAddData, SYSTEMFILTER_ARTIST, pStr );
						mxDlnaDmsDb_addCreator( ( MXDLNA_DMS_ADDITEM_PROP * )pAddData, pStr );
						break;
					}
				default:
					break;
			}
		}

MAKE_CATEGORY_SKIP:

		HOM_MEMFREE( pContentData );

		pSectionInfo = pSectionInfo->pNext;
		bIsConverted = FALSE;
	}

	HxLOG_Print("[%s] bIsConverted (%d)\n", __FUNCTION__, bIsConverted);

	if( mxDlnaDms_makeRelPath( ( HCHAR * )pcShareLocation, ( HCHAR * )pcLocationTemp, ( HCHAR ** )&pcRel_Path, ( HINT32 )isSharedFolderName ) != MXDLNA_ERROR )
	{
		mxDlnaDmsDb_setTreeInfo( ( MXDLNA_DMS_ADDITEM_PROP * )pAddData, SYSTEMFILTER_FOLDER, pcRel_Path );
	}

	if(NULL != pcRel_Path)
	{
		// TODO: library에서 malloc한 것은, library에서 free하도록 수정해야 함
		MX_FREE(pcRel_Path);
		//HOM_MEMFREE( pcRel_Path );
	}

	return ePL_DMS_Result_Ok;
}


#if 0
STATIC HINT32 __pl_dms_media_prob_record( MXFILE_DESC *pstFileDesc, MXMEDIA_INFO *pstMediaInfo, MXMETA_INFO *pstMetaInfo, MXCODEC_INFO *pstCodec )
{

#if 0
/* Changed from MWC_PVR_ReadRecordInfo() for getting cas info */
		nRet = MWC_PVR_ReadContentInfo(location, &stPvrRecInfo, &stPvrSvcInfo, &stPvrEvtInfo);
		if( nRet != ERR_OK )
		{
			HxLOG_Print( "[PL_DMS] Error> Cant open record meta:%s\n", location);
			return 0;
		}
		switch (stPvrSvcInfo.eSvcType) {
			case eDxSVC_TYPE_TV:
				eRecordSvcType = PL_MEDIA_RECORD_SVC_TYPE_TV;
				break;
			case eDxSVC_TYPE_RADIO:
				eRecordSvcType = PL_MEDIA_RECORD_SVC_TYPE_RADIO;
				break;
			default :
				eRecordSvcType = PL_MEDIA_RECORD_SVC_TYPE_NA;
		}
		/*
		 * - 5/11/2013: Allowed INCOMPLETED record file sharing for LiveTV bring-in problem
		 * - 3/xx/2013: Prevented INCOMPLETED record file sharing
		 *   + [redmine#66040] For accurate duration calculation, prevent addScan() while recording
		 *     : Check record status whether this is periodic scan() or caused by MSG_APP_RECORD_NOTIFY_STOPPED event
		 *     : This will be added at next scan() if ePVR_REC_STS_RECORDING
		 *   + [redmine#66603] The recording content has INCOMPLETED type will be unsupported (guided by kilee@)
		 */
		if ((stPvrRecInfo.ucRecordStatus != ePVR_REC_STS_INCOMPLETED) && (stPvrRecInfo.ucRecordStatus != ePVR_REC_STS_RECORDED))
		{
			HxLOG_Print( "[PL_DMS] Error> Unsupported record status(%d)\n", stPvrRecInfo.ucRecordStatus);
			return 0;
		}

		if (0) /* - 5/11/2013: Allowed record file sharing has error condition */
		{
			/* Ux 기획팀 이광일(kilee) 대리 시나리오 협의: recording error 녹화물 DMS 공유 제외 */
			/* Recoding file has conflict or error condition will be excluded */
			if (stPvrRecInfo.ucEndCondition != 0)  /* AP_RecEndReasonType_t != eREC_FAILED_NOFAIL */
			{
				HxLOG_Print( "[PL_DMS] Error> Unsupported non-normal endCondition(%d)\n", stPvrRecInfo.ucEndCondition);
				return 0;
			}
		}

		/*
		 * Prevent CAS channel record sharing; Currently, only supported FTA type at HMS/DMS
		 * - Don't reference u1cScrambled flag; check DxCasType_e:eDxCAS_TYPE_FTA from DxRecListData_t field.
		 * - or rather than, access CAS info with MW_PvrService_t:SVC_CasType_t field.
		 */
		if (stPvrSvcInfo.eCasType != eCasType_Fta)
		{
			HxLOG_Print( "[PL_DMS] Error> Unsupported non-FTA content; CAS type(%d), \n", stPvrSvcInfo.eCasType);
			return 0;
		}

		if (stPvrRecInfo.u1cLocked != 0)
		{
			HxLOG_Print( "[PL_DMS] Error> Unsupported file locked record(%d) \n", stPvrRecInfo.u1cLocked);
			return 0;
		}

		if (stPvrRecInfo.bUserLocked != 0)
		{
			HxLOG_Print( "[PL_DMS] Error> Unsupported user locked record(%d) \n", stPvrRecInfo.bUserLocked);
			return 0;
		}

		if ((DxDRM_Type_e)pstRecData->stMeta.stDrm.ulDrmType != eDrmModelType_None) /* (eDrmModelType_None) */
		{
			HxLOG_Print( "[PL_DMS] Error> Unsupported DRM type(%d) \n", stPvrRecInfo.stDrmInfo.ulDrmType);
			return 0;
		}

		if( ( stPvrRecInfo.ucRecordStatus != ePVR_REC_STS_RECORDED ) &&
				( stPvrRecInfo.ucRecordStatus != ePVR_REC_STS_INCOMPLETED ) )
		{
			if( stPvrRecInfo.ulRecTime > 86400 )
			{
				stPvrRecInfo.ulRecTime = 180000;
			}
			stPvrRecInfo.ulEndTime = stPvrRecInfo.ulStartTime + stPvrRecInfo.ulRecTime;
		}

#endif
	return 0;
}

STATIC HINT32 __pl_dms_media_prob_video( MXFILE_DESC *pstFileDesc, MXMEDIA_INFO *pstMediaInfo, MXMETA_INFO *pstMetaInfo, MXCODEC_INFO *pstCodecInfo )
{
	HLONG eDlnaRet = 0;
	HBOOL bIsCodecInfo = FALSE;

	/*************************************/
	/* Step 2. Meta Detect */
	/*************************************/

	/*************************************/
	/* Step 3. Codec Detect */
	/*************************************/
	switch ( pstMediaInfo->media_format )
	{
		case PL_DMS_FORMAT_VIDEO_MPG: /* PL_DMS_FORMAT_VIDEO_MPEG_TS188, TS192(?) */
		case PL_DMS_FORMAT_VIDEO_WMV:
		case PL_DMS_FORMAT_VIDEO_MP4:
		case PL_DMS_FORMAT_VIDEO_MKV:
		case PL_DMS_FORMAT_VIDEO_AVI:
			eDlnaRet = mxMedia_getCodecInfo( pstFileDesc, pstMediaInfo, NULL, PL_DMS_DETECT_CMD_ACCESS, pstCodecInfo );
			if( eDlnaRet >= 0 )
			{
				bIsCodecInfo = TRUE;
			}
			break;
		default :
			break;

	}

	return bIsCodecInfo;
}

STATIC HINT32 __pl_dms_media_prob_music( MXFILE_DESC *pstFileDesc, MXMEDIA_INFO *pstMediaInfo, MXMETA_INFO *pstMetaInfo, MXCODEC_INFO *pstCodecInfo )
{
	HLONG eDlnaRet = 0;
	HBOOL bIsCodecInfo = FALSE;
	/*************************************/
	/* Step 2. Meta Detect */
	/*************************************/
	switch ( pstMediaInfo->media_format )
	{
		case PL_DMS_FORMAT_AUDIO_MP3:
		case PL_DMS_FORMAT_AUDIO_WAV:
		case PL_DMS_FORMAT_AUDIO_AC3:
		case PL_DMS_FORMAT_AUDIO_AAC:
		case PL_DMS_FORMAT_AUDIO_WMA:
			eDlnaRet = mxMedia_getMetaInfo( pstFileDesc, pstMediaInfo, NULL, 0, pstMetaInfo );
			break;
	}

	/*************************************/
	/* Step 3. Codec Detect */
	/*************************************/
	switch ( pstMediaInfo->media_format )
	{
		case PL_DMS_FORMAT_AUDIO_MP3:
		case PL_DMS_FORMAT_AUDIO_MP4:
		case PL_DMS_FORMAT_AUDIO_WAV:
		case PL_DMS_FORMAT_AUDIO_AC3:
		case PL_DMS_FORMAT_AUDIO_AAC:
		case PL_DMS_FORMAT_AUDIO_WMA:
			eDlnaRet = mxMedia_getCodecInfo( pstFileDesc, pstMediaInfo, NULL, PL_DMS_DETECT_CMD_ACCESS, pstCodecInfo );
			if( eDlnaRet >= 0 )
			{
				bIsCodecInfo = TRUE;
			}
			break;
		default :
			break;
	}

	return bIsCodecInfo;
}

STATIC HINT32 __pl_dms_media_prob_photo( MXFILE_DESC *pstFileDesc, MXMEDIA_INFO *pstMediaInfo, MXMETA_INFO *pstMetaInfo, MXCODEC_INFO *pstCodecInfo )
{
	HLONG eDlnaRet = 0;
	HBOOL bIsCodecInfo = FALSE;
	/*************************************/
	/* Step 2. Meta Detect */
	/*************************************/
	switch ( pstMediaInfo->media_format )
	{
		case PL_DMS_FORMAT_IMAGE_JPG:
		case PL_DMS_FORMAT_IMAGE_PNG:
			eDlnaRet = mxMedia_getMetaInfo( pstFileDesc, pstMediaInfo, NULL, 0, pstMetaInfo );
			break;
	}

	/*************************************/
	/* Step 3. Codec Detect */
	/*************************************/
	switch ( pstMediaInfo->media_format )
	{
		case PL_DMS_FORMAT_IMAGE_JPG:
		case PL_DMS_FORMAT_IMAGE_PNG:
			eDlnaRet = mxMedia_getCodecInfo( pstFileDesc, pstMediaInfo, NULL, PL_DMS_DETECT_CMD_ACCESS, pstCodecInfo );
			if( eDlnaRet >= 0 )
			{
				bIsCodecInfo = TRUE;
			}
			break;
		default :
			break;
	}

	return bIsCodecInfo;
}

STATIC  MXDLNA_DMS_ADDITEM_PROP	*__pl_dms_media_create_containerItem(  HCHAR *location, HCHAR *title )
{
	MXDLNA_DMS_ADDITEM_PROP	*pAddItem = NULL;

#define ITEM_DIRECTORY_SIZE -1
	/* Item 생성*/
	pAddItem = mxDlnaDmsDb_newAddItemProp( location, title, ITEM_DIRECTORY_SIZE );
	if ( NULL == pAddItem )
	{
		return NULL;
	}

	mxDlnaDms_checkedDuplicateString( &pAddItem->mimeType, "application/octet-stream" );
	mxDlnaDms_checkedDuplicateString( &pAddItem->classType, "object.container" );
	pAddItem->mediaType = 0;

	return pAddItem;
}

STATIC  MXDLNA_DMS_ADDITEM_PROP	*__pl_dms_media_create_mediaItem( HCHAR *location, HCHAR *title,
								PL_FS_FileStat_t *pFileStat , MXMEDIA_INFO *pstMediaInfo, MXMETA_INFO *pstMetaInfo,
								MXCODEC_INFO *pstCodecInfo, HBOOL *pbIsCodecInfo )
{
	HINT32 mediaType = 0;

	MXFILE_DESC 	stFileDesc = { 0 };
	HINT64		llFileSize = 0;
	HINT32		retlib = 0;
	HBOOL		bIsCodecInfo = FALSE;

	MXDLNA_DMS_ADDITEM_PROP	*pAddItem = NULL;

	if ( NULL == pFileStat || NULL == pbIsCodecInfo )
	{
		return NULL;
	}

	stFileDesc.url = location;
	stFileDesc.linear_access	= 0;
	stFileDesc.cancel_op		= NULL;

	retlib = mxMedia_getInfoByExtension( location, pstMediaInfo );
	if ( 0 > retlib )
	{
		return NULL;
	}

	mediaType =  PL_DMS_MASK_MEDIATYPE( pstMediaInfo->media_format );

	switch ( mediaType )
	{
		case PL_DMS_TYPE_VIDEO :
			bIsCodecInfo = __pl_dms_media_prob_video( &stFileDesc, pstMediaInfo, pstMetaInfo, pstCodecInfo );
			break;
		case PL_DMS_TYPE_AUDIO :
			bIsCodecInfo = __pl_dms_media_prob_music( &stFileDesc, pstMediaInfo, pstMetaInfo, pstCodecInfo );
			break;
		case PL_DMS_TYPE_IMAGE :
			bIsCodecInfo = __pl_dms_media_prob_photo( &stFileDesc, pstMediaInfo, pstMetaInfo, pstCodecInfo );
			break;
		/* 아래 타입은 지원할 계획이 없음. */
		case PL_DMS_TYPE_TEXT :
		case PL_DMS_TYPE_SYSTEM :
		default :
			return NULL;
	}

	llFileSize = pFileStat->ulFileSize;
	pAddItem = mxDlnaDmsDb_newAddItemProp( location, title, llFileSize );

	/* AUDIO/VIDEO mp4에 대한 type 보정 */
	if( bIsCodecInfo )
	{
		if( ( pstMediaInfo->media_format == PL_DMS_FORMAT_AUDIO_MP4 ) && ( pstCodecInfo->media.av.videoCnt > 0 ) )
		{
			mxDlnaDms_checkedDuplicateString( &pAddItem->mimeType, "video/mp4" );

			pstMediaInfo->media_format = PL_DMS_FORMAT_VIDEO_MP4;
			pAddItem->mediaType = PL_DMS_MASK_MEDIATYPE( pstMediaInfo->media_format );
		}
		else if( ( pstMediaInfo->media_format == PL_DMS_FORMAT_VIDEO_MP4 ) && ( pstCodecInfo->media.av.videoCnt <= 0 ) )
		{
			mxDlnaDms_checkedDuplicateString( &pAddItem->mimeType, "audio/mp4" );

			pstMediaInfo->media_format = PL_DMS_FORMAT_AUDIO_MP4;
			pAddItem->mediaType = PL_DMS_MASK_MEDIATYPE( pstMediaInfo->media_format );
		}
	}

	pAddItem->mediaType = PL_DMS_MASK_MEDIATYPE( pstMediaInfo->media_format );//mediaType;

	*pbIsCodecInfo = bIsCodecInfo;

	return pAddItem;
}

STATIC  MXDLNA_DMS_ADDITEM_PROP	*__pl_dms_media_create_chunkRecordItem(  HCHAR *location, HCHAR *title,
								MXMEDIA_INFO *pstMediaInfo, MXMETA_INFO *pstMetaInfo,
								MXCODEC_INFO *pstCodecInfo, HBOOL *pbIsCodecInfo )
{
	MXDLNA_DMS_ADDITEM_PROP	*pAddItem = NULL;
	PL_FS_FileStat_t	filestat;

	HCHAR recordHjmPath[512] = { 0 };
	HCHAR	fileExt[8] = { 0 };
	HCHAR *p =  NULL;

	HINT32 pathLen = 0;
	HBOOL		bIsCodecInfo = FALSE;

	p = HxSTD_StrRChar( location, '.' );
	if( NULL == p )
	{
		return NULL;
	}

	pathLen = (HINT32)(p - location);

	HxSTD_StrNCpy( recordHjmPath, location, sizeof(recordHjmPath) );
	recordHjmPath[pathLen] = '\0';
	HxSTD_StrNCat( recordHjmPath, PL_DMS_FILE_JMETA_EXT, sizeof(recordHjmPath)-HxSTD_StrLen(recordHjmPath)-1 );

	HxSTD_MemSet(&filestat, 0x00, sizeof(PL_FS_FileStat_t));
	PL_FS_GetFileInfo( recordHjmPath, PL_FS_INFO_NAMETYPE, &filestat );

	if ( PL_FS_FILE_TYPE_FILE !=filestat.fileType )
	{
		return NULL;
	}

#if 0
	{
		Handle_t hRecFile = 0;
		HINT64 llSize = 0;
		HxLOG_Print( "[ DEBUG ] record path = %s\n", location );
		hRecFile = PL_FS_CHUNK_Open( location, "" ); /* <-- Define secure mode(TRUE or FALSE) */
		if( hRecFile == HANDLE_NULL )
		{
			HxLOG_Print( "[PL_DMS] Error> Cant open %s\n", location);
			return 0;
		}

		PL_FS_CHUNK_GetFileSize( hRecFile, &llSize );
		HxLOG_Print( "[ DEBUG ] File Size = %lld\n", llSize );
		PL_FS_CHUNK_Close( hRecFile );

		bIsCodecInfo = __pl_dms_media_prob_record( &stFileDesc, pstMediaInfo, pstMetaInfo, pstCodecInfo );


	}
#endif
	return pAddItem;
}

STATIC  MXDLNA_DMS_ADDITEM_PROP	*__pl_dms_media_create_singleRecordItem( HCHAR *location, HCHAR *title,
								PL_FS_FileStat_t *pFileStat , MXMEDIA_INFO *pstMediaInfo, MXMETA_INFO *pstMetaInfo,
								MXCODEC_INFO *pstCodecInfo, HBOOL *pbIsCodecInfo )
{
	MXDLNA_DMS_ADDITEM_PROP	*pAddItem = NULL;
	HBOOL		bIsCodecInfo = FALSE;

#if 0
	{
		HINT64		llFileSize = 0;
		MXFILE_DESC 	stFileDesc = { 0 };
		MXFILE_HANDLE		hFile = NULL;

		stFileDesc.url				= &szRecordPath[0]; /* not location; */
		stFileDesc.linear_access	= 0;
		stFileDesc.cancel_op		= NULL;

		eDlnaRet = mxFile_openFile( &stFileDesc, &hFile , NULL); // --> mxFile_openFile --> __pl_dms_media_CB_IsUserFile --> HLIB_DIR_Open
		if( eDlnaRet < 0 )
		{
			HxLOG_Print( "[PL_DMS] Error> Fail to FileOpen( %s )\n", stFileDesc.url );
			return 0;
		}

		llFileSize = pFileStat->ulFileSize;
		if( llFileSize == 0 )
		{
			eDlnaRet = mxFile_getFilesize( hFile, &llFileSize);
		}
		mxFile_closeFile( hFile );
		HxLOG_Print( "[ DEBUG ] File Size = %lld\n", llSize );

		bIsCodecInfo = __pl_dms_media_prob_record( &stFileDesc, pstMediaInfo, pstMetaInfo, pstCodecInfo );
	}
#endif
	return pAddItem;
}

STATIC HLONG __pl_dms_media_CbAddScanItem( MXDLNA_DMS_HANDLE  dmsHandle, DMS_DB_HANDLE db_h,
										HCHAR *location, HCHAR *filename , HCHAR *scanningPath )
{
	MXDLNA_DMS_ADDITEM_PROP 	*pAddItem = NULL;
	MXMEDIA_INFO	stMediaInfo = { 0 };
	MXMETA_INFO 	stMetaInfo = { 0 };
	MXCODEC_INFO	stCodecInfo = { 0 };

	PL_FS_FileStat_t	filestat = { 0 };
	PL_FS_FILE_TYPE_e fileType = PL_FS_FILE_TYPE_FILE;

	HBOOL		bSupportRecordDMS = FALSE;
	HBOOL		bDelayedRecordExisted = FALSE;

	HBOOL		bIsCodecInfo = FALSE;
	HINT32		nAddedMediaID = 0;
	HINT32		retlib = 0;

	HCHAR		szTitle[ 256 ] = { 0 };
	HCHAR		*pUserData = NULL;
	HCHAR		fileExt[8] = { 0 };
	HBOOL 		isRecord = FALSE;

	PL_DMS_RECORD_IO_TYPE_e eTsType = PL_MEDIA_RECORD_IO_TYPE_NA;

	HxLOG_Debug ( "[%s:%s:%d] location - %s / name - %s / path - %s \n", __FILE__, __FUNCTION__, __LINE__, location, filename, scanningPath );

	/* product config */
	//bSupportRecordDMS = pContext->stExtConfig.bSupportRecordItem;

	/* Chekc ItemType : File or Directory */
	PL_FS_GetFileInfo( location, PL_FS_INFO_NAMETYPE, &filestat );
	fileType = filestat.fileType;

	if ( filename )
	{
		HxSTD_StrNCpy( szTitle, filename, sizeof( szTitle )-1 );
	}

	/** 1. file Type에 따른 AddItem 생성 */
	switch ( fileType )
	{
		case PL_FS_FILE_TYPE_DIR :
			/* chunk or delayed recoding을 위한 폴더 일 수 있다.  */
			if ( __pl_dms_media_isChunkFolder( location ) )
			{
				HxLOG_Print( "[PL_DMS] Scan Item  : Chunk Directory( %s ).\n", location );
				/* Chunk Folder는 나중 hjm 파일 스캔시 하나의 Item으로 넣도록 하자 */
				return 0;
			}
			else
			{
				HxLOG_Print( "[PL_DMS] Scan Item  : directory( %s ).\n", location );
				/* Rename이 필요한 특수한 Directory에 대한 처리 */
				__pl_dms_media_RenameSpecialDirectory( location, szTitle, sizeof( szTitle ) );

				pAddItem = __pl_dms_media_create_containerItem( location, szTitle );
			}

			break;

		case PL_FS_FILE_TYPE_FILE :
			/* 여기서 Recording 파일인지 구분을 하자. Recording 파일이 아니면, Media 파일!!! */
			HxLOG_Print( "[PL_DMS] Scan Item  : file( %s ).\n", location );
			__pl_dms_media_getFileExtension( location, fileExt, sizeof(fileExt) );

			if ( NULL == HxSTD_StrStr( "ts", fileExt ) )
			{
				/* ts 은 Vidoe 폴더의 필터링 값이다. hjm은 해당 필터링에 없다. */
				/* ts확장자의 경우 Record 일 수도 있고, Media 파일일수 있다 */
				eTsType = __pl_dms_media_CheckRecordType( location );
				if ( PL_MEDIA_RECORD_IO_TYPE_NA != eTsType )
				{
					/* TS가 Recording 파일이다 */
					//isRecord = TRUE;
					/* Video에 Recording 파일이 있을 경우 보여주지 않는 방식 */
					return 0;
				}
				/*TS가 Recoding 파일이 아니면, mediaItem을 통해 생성된다 */
			}
			else if ( NULL == HxSTD_StrStr( "hjm", fileExt ) )
			{
				/* hjm 은 .Recoding 폴더의 필터링 값이다 */
				/* hjm 존재 유무에 따라 Recording 구분을 한다 */
				eTsType = __pl_dms_media_CheckRecordType( location );
				if ( PL_MEDIA_RECORD_IO_TYPE_NA != eTsType )
				{
					isRecord = TRUE;
				}
				else
				{
					/* hjm과 매핑되는 ts 타겟이 Reocording이 아니면 그냥 무시*/
					return 0;
				}

				if ( isRecord && (FALSE == bSupportRecordDMS) )
				{
					return 0;
				}
			}

			/** AddItem 생성 **/
			if ( isRecord )
			{
				if ( PL_MEDIA_RECORD_IO_TYPE_DELAYED == eTsType )
				{
					pAddItem = __pl_dms_media_create_chunkRecordItem( location, szTitle,
							&stMediaInfo, &stMetaInfo, &stCodecInfo, &bIsCodecInfo );

				}
				else if ( PL_MEDIA_RECORD_IO_TYPE_SINGLE == eTsType )
				{
					pAddItem = __pl_dms_media_create_singleRecordItem( location, szTitle, &filestat,
							&stMediaInfo, &stMetaInfo, &stCodecInfo, &bIsCodecInfo );
				}
				else
				{
					pAddItem = NULL;
				}

			}
			else
			{
				pAddItem = __pl_dms_media_create_mediaItem( location, szTitle, &filestat,
						&stMediaInfo, &stMetaInfo, &stCodecInfo, &bIsCodecInfo );
			}

			break;

		default :
			HxLOG_Print( "[PL_DMS] Scan Item : Unknown file( %s ).\n", location );
			return 0;
	}

	if ( NULL == pAddItem )
	{
		HxLOG_Print( "[PL_DMS] does not create a Cds item.\n" );
		return 0;
	}

	/* common item field */
	mxDlnaDms_checkedDuplicateString( &pAddItem->searchRootPath, scanningPath );

	/** 2. Make Category */
	__pl_dms_media_MakeCategory( pAddItem, &stMediaInfo, &stMetaInfo, scanningPath, location );

	/* Directory의 경우 Resource 및 Thumbnail이 필요없다. */
	if (  PL_FS_FILE_TYPE_FILE == fileType  )
	{
		//PAL_MEDIA_INFO_t		tMediaInfo = { 0 };
		HCHAR 	*pThumbPath = NULL;
		HINT32	nThumbStilTime = 0;

		/** 3. Make ItemInfo Resource */
		if (__pl_dms_media_MakeResource(pAddItem , &stMediaInfo, &stMetaInfo, &stCodecInfo, location, location, pUserData, bIsCodecInfo) != TRUE)
		{
			HxLOG_Error( "[PL_DMS] Error> makeResource() failed!\n" );
		}

		/** 4. Make Thumbnail Resource */
#if !defined(CONFIG_MW_MM_DLNA_DMS_JAPAN)
		if (( pAddItem->mediaType == PL_DMS_TYPE_VIDEO ) && ( stMediaInfo.duration> 0 ))
		{
			if (0)
				nThumbStilTime = stMediaInfo.duration / 2;
			else
				nThumbStilTime = ((0 < stMediaInfo.duration && stMediaInfo.duration < PL_DMS_THUMBNAIL_EXTRACT_TIME) ? stMediaInfo.duration : PL_DMS_THUMBNAIL_EXTRACT_TIME);
		}

		/* Thumbnail 추출 - HMS에서는 THMA를 이용 */
		pThumbPath = THAPI_GetThumbnailURL( location, nThumbStilTime, NULL, TRUE);
		if( pThumbPath )
		{
			__pl_dms_media_MakeThumbnailResource( pAddItem, pThumbPath );
		}
#else
		if( ( stMediaInfo.media_format == PL_DMS_FORMAT_AUDIO_MP3 ) ||
				( stMediaInfo.media_format == PL_DMS_FORMAT_AUDIO_WMA ) ||
				( stMediaInfo.media_format == PL_DMS_FORMAT_IMAGE_JPG ) )
		{
			__pl_dms_media_MakeThumb( pAddItem, &stMetaInfo, stMediaInfo.media_format );
		}
#endif

	}

	/* 여기서 위에서 만들어진 아이템 및 리소스/썸네일 정보가 DB에 들어감!!!!. */
	nAddedMediaID = mxDlnaDmsDb_addCdsItem( dmsHandle, db_h, pAddItem, NULL, SYSTEM_CONTAINER );

	/* free */
	mxMedia_freeMetaInfo( &stMetaInfo );
	mxDlnaDmsDb_freeAddItemProp( pAddItem );

	return nAddedMediaID;
}
#endif

/* location			: full path (file name + path)
	filename		: only file name (extension included)
	scanningPath	: only file path */
STATIC HLONG __pl_dms_media_CbAddScanFilter( MXDLNA_DMS_HANDLE  dmsHandle, DMS_DB_HANDLE db_h,
										HCHAR *location, HCHAR *filename , HCHAR *scanningPath , char *uuid)
{
	HBOOL						bIsDir = FALSE;
	MXDLNA_DMS_ADDITEM_PROP		*pAddItem = NULL;
	MXFILE_DESC					stFileDesc = {0,};
	MXMEDIA_INFO				stMediaInfo = {0,};
	MXMETA_INFO					stMetaInfo = {0,};
	MXCODEC_INFO				stCodecInfo = {0,};
	HBOOL						bIsCodecInfo = FALSE;
	PL_FS_FileStat_t			filestatp;
	HINT32						nAddedMediaID = 0;	// MXDMS_SCANFILTER_INIT
	HINT64						llFileSize;
	HLONG						eDlnaRet = 0;
	HCHAR						szTitle[256] = {0,};
	HCHAR						*pToken = NULL;

	if ((dmsHandle == NULL) || (db_h == NULL) || (location == NULL))
	{
		HxLOG_Error( "[PL_DMS] Error> Invalid arg or Unknown file!\n" );
		return 0;
	}


	///////////////////////////////
	// [1 step] --> prepare data //
	///////////////////////////////

	/* Set title */
	HxSTD_MemSet(szTitle, 0x0, sizeof(szTitle));
	HxSTD_snprintf(szTitle, sizeof( szTitle ), "%s", filename);

	// fill the desc for file related processing
	stFileDesc.url = location;
	stFileDesc.linear_access = 0;
	stFileDesc.cancel_op = NULL;

	// mxUtil_getFileDirType ---> stat()
	if ( mxUtil_getFileDirType(location) == MXDLNA_FILEDIR_TYPE_DIRECTORY )
	{
		bIsDir = TRUE;
		llFileSize = -1;
	}
	else
	{
		// remove extension from filename
		pToken = HxSTD_StrRChar(szTitle, '.');
		if( pToken )
		{
			*pToken = '\0';
		}

		// get file size
		HxSTD_MemSet(&filestatp, 0x00, sizeof(PL_FS_FileStat_t));
		PL_FS_GetFileInfo( location, PL_FS_FILE_TYPE_FILE, &filestatp );
		llFileSize = filestatp.ulFileSize;
		if( llFileSize == 0 )
		{
			__pl_dms_media_GetFileSize( &stFileDesc, &llFileSize );
		}

		/* get media format from location value (media_format defined in "mxmedia_define.h")
		MF_VIDEO_AVI, MF_VIDEO_MP4, MF_AUDIO_WMA, MF_AUDIO_MP3, MF_IMAGE_JPG ... */
		if( HxSTD_StrStr( location, "HTTP" ) != NULL || HxSTD_StrStr( location, "http" ) )
		{
			eDlnaRet = mxMedia_getInfoByHttp( &stFileDesc, &stMediaInfo );
			if( eDlnaRet < 0 )
			{
				eDlnaRet = mxMedia_getInfoByExtension( location, &stMediaInfo );
			}
		}
		else
		{
			eDlnaRet = mxMedia_getInfoByExtension( location, &stMediaInfo );
		}

		// Meta Info Detect
		switch( stMediaInfo.media_format )
		{
			case PL_DMS_FORMAT_AUDIO_MP3:
			case PL_DMS_FORMAT_AUDIO_WAV:
			case PL_DMS_FORMAT_AUDIO_AC3:
			case PL_DMS_FORMAT_AUDIO_AAC:
			case PL_DMS_FORMAT_AUDIO_WMA:
			case PL_DMS_FORMAT_IMAGE_JPG:
			case PL_DMS_FORMAT_IMAGE_PNG:
				/*************************************/
				/* Step 2. Meta Detect */
				/*************************************/
				//2 Free 해야 하는 값!! -> mxMedia_freeMetaInfo()
				eDlnaRet = mxMedia_getMetaInfo( &stFileDesc, &stMediaInfo, NULL, 0, &stMetaInfo );
				break;

			default:
				break;
		}
	}

	if( bIsDir == TRUE )
	{
		/* Rename이 필요한 특수한 Directory에 대한 처리 */
		/*
		 * AddScanDirectory 시 path 주의; 필요에 따라 rename 수행해야 함
		 * 현재 HMS1000S PH#1, PH#2 base 기준으로 "My Contents" 이하 CDS/DB 구성은 다음과 같음
		 * - "My Contents"
		 *     + Media
		 *       + Media
		 *         + Video
		 *         + Music
		 *         + Photo
		 *         + Download
		 *       + Recordings
		 *     + "Detachable Media"
		 *     + "Detachable Record"
		 *     + "USB Media" if it is supported
		 */
		__pl_dms_media_RenameSpecialDirectory( location, szTitle, sizeof( szTitle ) );
	}


	/////////////////////////////////////
	// [2 step] --> make add item info //
	/////////////////////////////////////


	// Add Media data start.....
	/* CDS에 Object를 생성하기 위한 구조체 생성 -> mxDlnaDmsDb_freeAddItemProp() 로 해제
		-> title, local url, mime type, size will be set to the pAddData */
	//2 Free 해야 하는 값!! -> mxDlnaDmsDb_freeAddItemProp()
	pAddItem = mxDlnaDmsDb_newAddItemProp( location, szTitle, llFileSize);
	if( pAddItem == NULL )
	{
		HxLOG_Print( "[PL_DMS] Error> Fail to mxDlnaDmsDb_newAddItemProp()\n" );

		if( bIsDir == FALSE )
		{
			/* MetaInfo 해제 */
			mxMedia_freeMetaInfo( &stMetaInfo );
		}
		goto func_error;
	}

	/************************************************************************/
	/* pAddItem을 생성했으니 여기서 부터 채워 나가자 ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓*/
	/************************************************************************/
	if( bIsDir == TRUE )
	{
		mxDlnaDms_checkedDuplicateString( &pAddItem->mimeType, "application/octet-stream" );
		mxDlnaDms_checkedDuplicateString( &pAddItem->classType, "object.container" );
		pAddItem->mediaType = 0;

/* 2015/12/18 mhkang: http://svn.humaxdigital.com:3000/issues/109277 */
		pAddItem->isRecordingsFolder = __pl_dms_isRecordingsFolder(location);
	}
	else
	{
		pAddItem->mediaType = PL_DMS_MASK_MEDIATYPE( stMediaInfo.media_format );
	}

	pAddItem->uuid = uuid;

	// Codec Info Detect
	{
		int ret = 0;
		HxSTD_MemSet( (void *)&stCodecInfo , 0 , sizeof (MXCODEC_INFO) );

		switch( stMediaInfo.media_format )
		{
			case PL_DMS_FORMAT_VIDEO_MPG: /* PL_DMS_FORMAT_VIDEO_MPEG_TS188, TS192(?) */
			case PL_DMS_FORMAT_VIDEO_WMV:
			case PL_DMS_FORMAT_VIDEO_MP4:
			case PL_DMS_FORMAT_VIDEO_MKV:
			case PL_DMS_FORMAT_VIDEO_AVI:
			case PL_DMS_FORMAT_AUDIO_MP3:
			case PL_DMS_FORMAT_AUDIO_MP4:
			case PL_DMS_FORMAT_AUDIO_WAV:
			case PL_DMS_FORMAT_AUDIO_AC3:
			case PL_DMS_FORMAT_AUDIO_AAC:
			case PL_DMS_FORMAT_AUDIO_WMA:
			case PL_DMS_FORMAT_IMAGE_JPG:
			case PL_DMS_FORMAT_IMAGE_PNG:
				/*************************************/
				/* Step 3. Codec Detect */
				/*************************************/
				ret = mxMedia_getCodecInfo( &stFileDesc , &stMediaInfo, NULL , MXDLNA_DETECT_CMD_ACCESS , &stCodecInfo );
				if ( ret >= MXMEDIA_OK )
				{
					bIsCodecInfo = TRUE;
				}
				break;

			/* 자막 파일을 CDS Object로 포함하지 않코자 하는경우 */
			case PL_DMS_FORMAT_SYSTEM_SMI:
				pAddItem->objectNotCreate = 1;
				break;

			default :
				break;
		}
	}

	// AUDIO/VIDEO mp4에 대한 type 보정
	if( bIsCodecInfo )
	{
		if( ( stMediaInfo.media_format == PL_DMS_FORMAT_AUDIO_MP4 ) && ( stCodecInfo.media.av.videoCnt > 0 ) )
		{
			mxDlnaDms_checkedDuplicateString( &pAddItem->mimeType, "video/mp4" );

			stMediaInfo.media_format = PL_DMS_FORMAT_VIDEO_MP4;
			pAddItem->mediaType = PL_DMS_MASK_MEDIATYPE( stMediaInfo.media_format );
		}
		else if( ( stMediaInfo.media_format == PL_DMS_FORMAT_VIDEO_MP4 ) && ( stCodecInfo.media.av.videoCnt <= 0 ) )
		{
			mxDlnaDms_checkedDuplicateString( &pAddItem->mimeType, "audio/mp4" );

			stMediaInfo.media_format = PL_DMS_FORMAT_AUDIO_MP4;
			pAddItem->mediaType = PL_DMS_MASK_MEDIATYPE( stMediaInfo.media_format );
		}
	}

	// search Root
	mxDlnaDms_checkedDuplicateString(&pAddItem->searchRootPath , scanningPath);

#if defined(CONFIG_TRANSCODING_STREAMING)
	// ex config
	pAddItem->nExConfig = PL_DMS_M3U8RES_USE | PL_DMS_M3U8RES_POS_BASIC;
#endif


	////////////////////////////////////////////////
	// [3 step] --> make categoray, make resource //
	////////////////////////////////////////////////


	//treeInfo.
	__pl_dms_media_MakeCategory( pAddItem, &stMediaInfo, &stMetaInfo, scanningPath, location );
	if ( bIsDir == FALSE )
	{
		if( bIsCodecInfo )	// 반드시 이렇게 해줘야 함
		{
			if (__pl_dms_media_MakeResource(pAddItem , &stMediaInfo, &stMetaInfo, &stCodecInfo, location, location, NULL, TRUE) != TRUE)
			{
				HxLOG_Error( "[PL_DMS][%d] Error> makeResource() failed!\n", __LINE__);
				mxMedia_freeMetaInfo(&stMetaInfo);
				if (pAddItem != NULL)
				{
					mxDlnaDmsDb_freeAddItemProp(pAddItem);
					pAddItem = NULL;
				}
				goto func_error;
			}
		}
		else
		{
			if (__pl_dms_media_MakeResource( pAddItem , &stMediaInfo, &stMetaInfo, NULL, location, location, NULL, FALSE) != TRUE)
			{
				HxLOG_Error( "[PL_DMS][%d] Error> makeResource() failed!\n", __LINE__);
				mxMedia_freeMetaInfo(&stMetaInfo);
				if (pAddItem != NULL)
				{
					mxDlnaDmsDb_freeAddItemProp(pAddItem);
					pAddItem = NULL;
				}
				goto func_error;
			}
		}

		/* Commented because of being unused */
		/*
		if( ( stMediaInfo.media_format != PL_DMS_FORMAT_SYSTEM_SMI ) &&
			( stMediaInfo.media_format != PL_DMS_FORMAT_UNKONWN ) )
		{
			__pldms_SubtitleCheckNAdd( pAddItem, stMediaInfo.media_format, location );
		}
		*/

		/* Thumbnail 관련 추출 - HMS에서는 THMA를 이용 */
		{
			HCHAR 	*pThumbPath = NULL;
			HUINT32 nThumbStilTime = 0;

			if( ( pAddItem->mediaType == PL_DMS_TYPE_VIDEO ) && ( stMediaInfo.duration> 0 ) )
			{
				nThumbStilTime = (stMediaInfo.duration / 2); /* Still 이미지 추정 */
			}

			pThumbPath = (HCHAR*)THAPI_GetThumbnailURL(location, nThumbStilTime, NULL, TRUE);
			if( pThumbPath )
			{
				/* albumart(pAddItem->albumarturi) will be copied at MakeThumbnailResource() */
				__pl_dms_media_MakeThumbnailResource( pAddItem, pThumbPath );
			}
			else
			{
				HxLOG_Error("[PL_DMS] Error> failed to make thumbnail!\n");
			}
		}
	}


	////////////////////////////////////////////////
	// [4 step] --> add cds item, subtitle check, free //
	////////////////////////////////////////////////


	// 여기서 위에서 만들어진 아이템 및 리소스/썸네일 정보가 DB에 들어감!!!!
	nAddedMediaID = mxDlnaDmsDb_addCdsItem( dmsHandle, db_h, pAddItem, NULL, SYSTEM_CONTAINER );

	if (bIsDir == FALSE)
	{
		char ContainerID[100]={0};
		char artist[1024] ={0};
		char genre[1024] ={0};
		char album[1024] ={0};
		char* name;

		name = mxDlnaDmsDb_getTreeInfo( pAddItem,  SYSTEMFILTER_ARTIST);
		sprintf(artist, "%s", (name==NULL ? "Uknown artist" : name));

		name  = mxDlnaDmsDb_getTreeInfo( pAddItem,  SYSTEMFILTER_GENRE);
		sprintf(genre, "%s", (name==NULL ? "Uknown genre" : name));

		name  = mxDlnaDmsDb_getTreeInfo( pAddItem,  SYSTEMFILTER_ALBUM);
		sprintf(album, "%s", (name==NULL ? "Uknown album" : name));

		// Music/Artist
		if ( mxDlnaDms_getContainerID(db_h, artist, "object.container.person.musicArtist", ContainerID) == MX_FALSE)
		{
			if (mxDlnaDms_getNewContainerID(db_h, CONTAINER_COND_MUSIC_ARTIST, ContainerID) == MX_FALSE)
			{
				HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] mxDlnaDms_getNewContainerID failed\n"), __FUNCTION__, __LINE__);
				goto EXIT_ARTIST;
			}

			if (mxDlnaDms_createContainer(db_h, ContainerID, artist, "object.container.person.musicArtist", MX_TRUE, MX_TRUE) != MXDLNA_OK)
			{
				HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] mxDlnaDms_createContainer failed\n"), __FUNCTION__, __LINE__);
				goto EXIT_ARTIST;
			}
		}

		if (strlen(ContainerID) > 0)
		{
			if (mxDlnaDms_createReferenceItem(db_h, pAddItem->localUrl, ContainerID) != MXDLNA_OK)
			{
				HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] mxDlnaDms_createReferenceItem(localUrl:%s, ContainerID:%s) failed\n"), __FUNCTION__, __LINE__, pAddItem->localUrl, ContainerID);
			}
		}
EXIT_ARTIST:;

		// Music/Genre
		if ( mxDlnaDms_getContainerID(db_h, genre, "object.container.genre.musicGenre", ContainerID) == MX_FALSE)
		{
			if (mxDlnaDms_getNewContainerID(db_h, CONTAINER_COND_MUSIC_GENRE, ContainerID) == MX_FALSE)
			{
				HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] mxDlnaDms_getNewContainerID failed\n"), __FUNCTION__, __LINE__);
				goto EXIT_GENRE;
			}

			if (mxDlnaDms_createContainer(db_h, ContainerID, genre, "object.container.genre.musicGenre", MX_TRUE, MX_TRUE) != MXDLNA_OK)
			{
				HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] mxDlnaDms_createContainer failed\n"), __FUNCTION__, __LINE__);
				goto EXIT_GENRE;
			}
		}

		if (strlen(ContainerID) > 0)
		{
			if (mxDlnaDms_createReferenceItem(db_h, pAddItem->localUrl, ContainerID) != MXDLNA_OK)
			{
				HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] mxDlnaDms_createReferenceItem(localUrl:%s, ContainerID:%s) failed\n"), __FUNCTION__, __LINE__, pAddItem->localUrl, ContainerID);
			}
		}
EXIT_GENRE:;

		// Music/Album
		if ( mxDlnaDms_getContainerID(db_h, album, "object.container.album.musicAlbum", ContainerID) == MX_FALSE)
		{
			if (mxDlnaDms_getNewContainerID(db_h, CONTAINER_COND_MUSIC_ALBUM, ContainerID) == MX_FALSE)
			{
				HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] mxDlnaDms_getNewContainerID failed\n"), __FUNCTION__, __LINE__);
				goto EXIT_ALBUM;
			}

			if (mxDlnaDms_createContainer(db_h, ContainerID, album, "object.container.album.musicAlbum", MX_TRUE, MX_TRUE) != MXDLNA_OK)
			{
				HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] mxDlnaDms_createContainer failed\n"), __FUNCTION__, __LINE__);
				goto EXIT_ALBUM;
			}
		}

		if (strlen(ContainerID) > 0)
		{
			if (mxDlnaDms_createReferenceItem(db_h, pAddItem->localUrl, ContainerID) != MXDLNA_OK)
			{
				HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] mxDlnaDms_createReferenceItem(localUrl:%s, ContainerID:%s) failed\n"), __FUNCTION__, __LINE__, pAddItem->localUrl, ContainerID);
			}
		}
EXIT_ALBUM:;

	}

	// subtitle check & andd
	if( stMediaInfo.media_format == PL_DMS_FORMAT_SYSTEM_SMI )
	{
		(void)__pl_dms_media_SubtitleCheckNAdd( pAddItem, stMediaInfo.media_format, location );
	}

	// ######################################
	// FREE
	// ######################################
	mxMedia_freeMetaInfo( &stMetaInfo );
	if (pAddItem != NULL)
	{
		mxDlnaDmsDb_freeAddItemProp( pAddItem );
		pAddItem = NULL;
	}
	// ######################################

	return nAddedMediaID;

func_error:
	HxLOG_Error("[PL_DMS] Error> failed to addScanFilter(loc:%s, file:%s)!\n", location, filename);

	return 0; /* (-1) */
}

STATIC HLONG __pl_dms_media_CbRemoveScanFilter ( MXDLNA_DMS_HANDLE dmsHandle, DMS_DB_HANDLE h_db,	HCHAR *location )
{
	MXFILE_DESC 	stFileDesc = { 0, };
	MXMEDIA_INFO	stMediaInfo = { 0, };
	MXDLNA_DMS_REMOVE_PROP *pRemoveData = NULL;
	HLONG 					lRemovedMediaID = 0;
	HBOOL					bIsDir = FALSE;
	HLONG					eDlnaRet = 0;

	stFileDesc.url = location ;
	stFileDesc.linear_access = 0 ;
	stFileDesc.cancel_op = NULL ;

	if( mxUtil_getFileDirType( location ) == MXDLNA_FILEDIR_TYPE_DIRECTORY )
	{
		bIsDir = TRUE;
	}
	else
	{
		/* location protocol이 HTTP이가 */
		if( ( HxSTD_StrStr( location, "HTTP" ) != NULL ) || ( HxSTD_StrStr( location, "http" ) != NULL ) )
		{
			eDlnaRet = mxMedia_getInfoByHttp( &stFileDesc, &stMediaInfo);
			if( eDlnaRet < 0 )
			{
				(void)mxMedia_getInfoByExtension(location , &stMediaInfo);
			}
		}
		else
		{
			(void)mxMedia_getInfoByExtension(location , &stMediaInfo);

		}
	}

	pRemoveData = mxDlnaDmsDb_newRemoveProp( location );

	if( pRemoveData == NULL )
	{
		return	0;
	}

	lRemovedMediaID = mxDlnaDmsDb_removeCdsItem( dmsHandle, h_db, pRemoveData, NULL, SYSTEM_CONTAINER );

	HxLOG_Print("[%s] bIsDir (%d) lRemovedMediaID (%d)\n", __FUNCTION__, bIsDir, lRemovedMediaID);

	(void)mxDlnaDmsDb_freeRemoveProp(pRemoveData);

	return lRemovedMediaID;
}

MX_INT32 __pl_dms_media_cbAddDetachableStorage(DMS_DB_HANDLE hDB,	const char* path, const char* parentID)
{
	char objectID[1024] ={0};

	HxLOG_Info(HxANSI_COLOR_YELLOW("[%s:%d] storagePath:%s parentID:%s\n"), __FUNCTION__, __LINE__, path, parentID);

	sprintf(objectID, "%s\\music", parentID);
	mxDlnaDms_createContainer(hDB, objectID, "Music", "object.container", MX_TRUE, MX_TRUE);

	sprintf(objectID, "%s\\music\\artist", parentID);
	mxDlnaDms_createContainer(hDB, objectID, "Artist", "object.container", MX_TRUE, MX_TRUE);

	sprintf(objectID, "%s\\music\\genre", parentID);
	mxDlnaDms_createContainer(hDB, objectID, "Genre", "object.container", MX_TRUE, MX_TRUE);

	sprintf(objectID, "%s\\music\\album", parentID);
	mxDlnaDms_createContainer(hDB, objectID, "Album", "object.container", MX_TRUE, MX_TRUE);

	return 0;
}

#define ________________Public_Interfaces__________________________
HERROR PL_DMS_ScanFilterInit(MXDLNA_FSS_SCANOPTION *stScanOption)
{
	stScanOption->rescan_interval_ms 		= PL_DMS_DEFAULT_SCAN_RESCAN_INTERVAL;
	stScanOption->per_interval_ms 			= PL_DMS_DEFAULT_SCAN_ITEM_INTERVAL;
	stScanOption->scan_maxcount 			= PL_DMS_DEFAULT_SCAN_COUNT_UNLIMITED;

	stScanOption->fnScanFilter_addEvent 	= ( MXDLNA_DMS_ADD_SCANFILTER )__pl_dms_media_CbAddScanFilter;
	stScanOption->fnScanFilter_removeEvent 	= ( MXDLNA_DMS_REMOVE_SCANFILTER )__pl_dms_media_CbRemoveScanFilter;
	stScanOption->fnScanFilter_addDetachableStorage = (MXDLNA_DMS_ADD_DETACHABLE_STORAGE)__pl_dms_media_cbAddDetachableStorage;

	stScanOption->bScanningTimePrint		= FALSE;

	return ERR_OK;
}

