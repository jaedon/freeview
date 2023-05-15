/**************************************************************
*	@file	pl_dms_scan_pvr.c
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
#include "pl_common.h"
#include "pl_fs.h"
#include "pl_pvr.h"

#include <clib.h>
#include <dlib.h>
#include <thapi.h>
#include <dapi.h>

#include <mxdlna.h>
#include <mxdlna_dms.h>
#include <mxfile_user.h>
#include <mxmedia.h>

#define ________________Defines____________________________________
#define PL_DMS_PVR_FILE_TS_EXT						".ts"
#define PL_DMS_PVR_FILE_JMETA_EXT					".hjm"

#if defined(CONFIG_HOMMA_CDI)
#define __PL_DMS_SUPPORT_FS_SECURE__			1
#define __PL_DMS_SUPPORT_FS_CHUNK__				1
#else
#define __PL_DMS_SUPPORT_FS_SECURE__			0
#define __PL_DMS_SUPPORT_FS_CHUNK__				1 /* Supported plain chunk-io under non-crypt PVR record */
#endif

#define PL_DMS_PVR_MAX_CONTENT_NUM				20

#define PL_DMS_OBJECT_ITEM_VIDEOITEM		"object.item.videoItem"
#define PL_DMS_OBJECT_ITEM_AUDIOITEM		"object.item.audioItem"
#define PL_DMS_OBJECT_ITEM_IMAGEITEM			"object.item.imageItem"

typedef enum
{
	eSEARCH_CRITERIA_ITEM_NONE		= 0x0000,
	eSEARCH_CRITERIA_ITEM_VIDEO		= 0x0001,
	eSEARCH_CRITERIA_ITEM_AUDIO		= 0x0002,
	eSEARCH_CRITERIA_ITEM_IMAGE		= 0x0004,
	eSEARCH_CRITERIA_ITEM_ALL		= 0x0008
} SEARCH_CRITERIA_ITEM_e;

typedef enum {
	PL_MEDIA_RECORD_IO_TYPE_NA=0,
	PL_MEDIA_RECORD_IO_TYPE_SINGLE,		/* Single record */
	PL_MEDIA_RECORD_IO_TYPE_DELAYED		/* Delayed record(Nexus Chunked IO) */
} PL_DMS_RECORD_IO_TYPE_e;

#define ________________Static_Variables___________________________
static 	PL_DMS_MXDLNA_HANDLE_t	pDmsHandles;

#define ________________Internal_APIs_Prototype____________________



#define ________________Internal_Functions_________________________
static HBOOL _pl_dms_pvr_IsAllowedCasType(DxCasType_e eCasType);

#if defined(PL_DMS_THIRDPARTY_PVR_FEATURE)
/* currently (.hjm) only supported */
static PL_DMS_RECORD_IO_TYPE_e __pl_dms_pvr_CheckRecordType(HCHAR *pRecordPath)
{
	PL_DMS_RECORD_IO_TYPE_e eRecordIoType = PL_MEDIA_RECORD_IO_TYPE_NA;
	HBOOL   bArgPathTS = FALSE; /* Avoiding too many strstr() */
	HBOOL   bArgPathJMeta = FALSE; /* Avoiding too many strstr() */
	HBOOL   bRecordJMetaExisted = FALSE;
	HCHAR   szPairPath[512] = { 0, };
	HCHAR   *pPathTemp = NULL;
	HCHAR   *pPathSrc = NULL; // warning: initialization discards qualifiers from pointer target type
	HINT32  nPathLen = 0;

	PL_FS_FileStat_t	filestat;
	PL_FS_FILE_TYPE_e fileType = PL_FS_FILE_TYPE_FILE;

	HxSTD_MemSet(&filestat, 0x00, sizeof(PL_FS_FileStat_t));

	if( pRecordPath == NULL)
	{
		HxLOG_Print("[ MW DMS ] Error> Invalid NULL arg!\n" );
		goto func_done;
	}

	if (HxSTD_StrStr(pRecordPath, PL_DMS_PVR_FILE_JMETA_EXT) != NULL)
	{
		bArgPathJMeta = TRUE;
		bArgPathTS = FALSE;
	}
	else if (HxSTD_StrStr(pRecordPath, ".ts") != NULL)
	{
		bArgPathTS = TRUE;
		bArgPathJMeta = FALSE;
	}
	else
	{
		goto func_done;
	}

	if (bArgPathTS == TRUE) /* Check .hjm file existing if .ts path(directory or file) */
	{
		/* Check pair file(.hjm) whether this ts is humax record property (single or delayed record) */
		pPathSrc = pRecordPath;
		pPathTemp = HxSTD_StrStr(pRecordPath, ".ts");
		if (pPathTemp != NULL)
		{
			nPathLen = pPathTemp - pPathSrc;
			HxSTD_MemSet(szPairPath, 0x00, sizeof(szPairPath) );
			HxSTD_StrNCat(szPairPath, pPathSrc, nPathLen);
			HxSTD_StrNCat(szPairPath, PL_DMS_PVR_FILE_JMETA_EXT, 4);

			PL_FS_GetFileInfo( szPairPath, PL_FS_INFO_NAMETYPE, &filestat );
			fileType = filestat.fileType;
			if( fileType == PL_FS_FILE_TYPE_FILE )
			{
				bRecordJMetaExisted = TRUE;
			}
			else if ( fileType == PL_FS_FILE_TYPE_DIR )
			{
				bRecordJMetaExisted = FALSE;
			}

			if (bRecordJMetaExisted == TRUE)
			{
				/* Check whether this .ts has single or delayed recoding property */
				PL_FS_GetFileInfo( pRecordPath, PL_FS_INFO_NAMETYPE, &filestat );
				fileType = filestat.fileType;
				if( fileType == PL_FS_FILE_TYPE_DIR )
				{
					eRecordIoType = PL_MEDIA_RECORD_IO_TYPE_DELAYED;
				}
				else if( fileType == PL_FS_FILE_TYPE_FILE )
				{
					eRecordIoType = PL_MEDIA_RECORD_IO_TYPE_SINGLE;
				}
			}
		}
	}
	else if (bArgPathJMeta == TRUE) /* Check .ts file existing if .hjm path */
	{
		/* Check .ts path whether this is single or delayed record */
		bRecordJMetaExisted = TRUE;
		pPathSrc = pRecordPath;
		pPathTemp = HxSTD_StrStr(pRecordPath, PL_DMS_PVR_FILE_JMETA_EXT);
		if (pPathTemp != NULL)
		{
			nPathLen = pPathTemp - pPathSrc;
			HxSTD_MemSet(szPairPath, 0x00, sizeof(szPairPath) );
			HxSTD_StrNCat(szPairPath, pPathSrc, nPathLen);
			HxSTD_StrNCat(szPairPath, PL_DMS_PVR_FILE_TS_EXT, 3);

			PL_FS_GetFileInfo( szPairPath, PL_FS_INFO_NAMETYPE, &filestat );
			fileType = filestat.fileType;
			if( fileType == PL_FS_FILE_TYPE_DIR )
			{
				eRecordIoType = PL_MEDIA_RECORD_IO_TYPE_DELAYED;
			}
			else if( fileType == PL_FS_FILE_TYPE_FILE )
			{
				eRecordIoType = PL_MEDIA_RECORD_IO_TYPE_SINGLE;
			}
		}
	}

	return eRecordIoType;

func_done:
	return PL_MEDIA_RECORD_IO_TYPE_NA;
}

static HINT64	__pl_dms_pvr_GetLocalFileSize( HCHAR *path )
{
	PL_FS_FileStat_t	filestatp;
	HxSTD_MemSet( &filestatp, 0x00, sizeof( PL_FS_FileStat_t ) );
	PL_FS_GetFileInfo( path, PL_FS_FILE_TYPE_FILE, &filestatp );
	return filestatp.ulFileSize;
}

static HERROR __pl_dms_pvr_GetFileSize(DxRecListData_t *pRecListData, HINT64 *plldSize)
{
	MXFILE_DESC 		stFileDesc = { 0, };
	MXFILE_HANDLE		hFile = NULL;
	HINT32				eDlnaRet = 0;
	HINT64				llFileSize = 0;
	HCHAR				szTsFileName[1024] = {0,};
	HCHAR				szMetaFileName[1024] = {0,};
	PL_DMS_RECORD_IO_TYPE_e 		eRecordIoType = PL_MEDIA_RECORD_IO_TYPE_NA; /* JPN향 영향 없음 */
#if __PL_DMS_SUPPORT_FS_CHUNK__
	HOM_Handle_t hRecFile = 0;
#endif
#if __PL_DMS_SUPPORT_FS_SECURE__
	HBOOL bSecure = TRUE;
#else
	HBOOL bSecure = FALSE;
#endif

	HxSTD_snprintf(szTsFileName, 1023, "%s%s.ts", pRecListData->stMeta.szPath, pRecListData->stMeta.szFileName);
	HxSTD_snprintf(szMetaFileName, 1023, "%s%s.hjm", pRecListData->stMeta.szPath, pRecListData->stMeta.szFileName);

	*plldSize = 0;

	if(PL_FS_IsFileExist(szMetaFileName) == FALSE)
	{
		return ERR_FAIL;
	}

	eRecordIoType = __pl_dms_pvr_CheckRecordType( szMetaFileName ); /* .hjm */
	if (eRecordIoType == PL_MEDIA_RECORD_IO_TYPE_DELAYED)
	{
#if __PL_DMS_SUPPORT_FS_CHUNK__
		HERROR hErr = 0;
		hRecFile = PL_FS_CHUNK_Open( szTsFileName, "", bSecure ); /* record path, not location */
		if( hRecFile == HANDLE_NULL )
		{
			HxLOG_Error( "[PL_DMS] Error> Cant open chunk_record(%s)\n", szTsFileName);
			return ERR_FAIL;
		}

		hErr = PL_FS_CHUNK_GetFileSize( hRecFile, &llFileSize );
		if( hErr != ERR_OK )
		{
			HxLOG_Error( "[PL_DMS] Error> Cant get file size(%s)\n", szTsFileName);
			return ERR_FAIL;
		}

		HxLOG_Print( "[PL_DMS] File Size = %lld\n", llFileSize );
		PL_FS_CHUNK_Close( hRecFile );

		*plldSize = llFileSize;
#else
		HxLOG_Error( "[PL_DMS] Error> Unsupported chunk record(%s)\n", szTsFileName);
		return ERR_FAIL;
#endif
	}
	else if (eRecordIoType == PL_MEDIA_RECORD_IO_TYPE_SINGLE)
	{
		stFileDesc.url				= szTsFileName; /* not location; */
		stFileDesc.linear_access	= 0;
		stFileDesc.cancel_op		= NULL;

		eDlnaRet = mxFile_openFile( &stFileDesc, &hFile , NULL);
		if( eDlnaRet < 0 )
		{
			HxLOG_Error( "[PL_DMS] Error> Fail to open(%s)\n", stFileDesc.url );
			return 0;
		}

		llFileSize = __pl_dms_pvr_GetLocalFileSize( szTsFileName );
		HxLOG_Print( "[PL_DMS] File Size = %lld\n", llFileSize );
		if( llFileSize == 0 )
		{
			eDlnaRet = mxFile_getFilesize( hFile, &llFileSize);
			HxLOG_Print( "[PL_DMS] File Size = %lld\n", llFileSize );
		}
		mxFile_closeFile( hFile );
		HxLOG_Print( "[PL_DMS] File Size = %lld\n", llFileSize );

		*plldSize = llFileSize;
	}
	else
	{
		/* Don't add item if {.hjm path and no .ts record} */
		HxLOG_Error( "[PL_DMS] Error> There is no ts record(%s)!\n", szTsFileName);
		return ERR_FAIL;
	}

	return ERR_OK;
}

static void __pl_dms_pvr_SetDefaultProtocolInfo( MXDLNA_PROTOCOLINFO *pProtocolInfo )
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

static void __pl_dms_pvr_InitPvrInfo(MXDLNA_DMS_PVR_CONTENT *pPvrInfo)
{
	HxSTD_MemSet(pPvrInfo, 0x00, sizeof(MXDLNA_DMS_PVR_CONTENT));

	HxSTD_MemSet(pPvrInfo->title, 0x00, sizeof(pPvrInfo->title));								// MXDLNA_DMS_PVR_CONTENT_TITLE_LEN
	HxSTD_MemSet(pPvrInfo->creator, 0x00, sizeof(pPvrInfo->creator));							// MXDLNA_DMS_PVR_CONTENT_CREATOR_LEN
	HxSTD_MemSet(pPvrInfo->albumarturi, 0x00, sizeof(pPvrInfo->albumarturi)); 					// MXDLNA_DMS_PVR_ALBUMARTURI_LEN
	HxSTD_MemSet(pPvrInfo->resource_contentURI, 0x00, sizeof(pPvrInfo->resource_contentURI));	// MXDLNA_DMS_PVR_CONTENT_URI_LEN
	HxSTD_MemSet(pPvrInfo->resource_protocol, 0x00, sizeof(pPvrInfo->resource_protocol));		//MXDLNA_DMS_PVR_CONTENT_RESOURCEPROTOCOL_LEN
	HxSTD_MemSet(pPvrInfo->thumbnail_contentURI, 0x00, sizeof(pPvrInfo->thumbnail_contentURI));	// MXDLNA_DMS_PVR_CONTENT_URI_LEN
	HxSTD_MemSet(pPvrInfo->thumbnail_protocol, 0x00, sizeof(pPvrInfo->thumbnail_protocol));		//MXDLNA_DMS_PVR_CONTENT_RESOURCEPROTOCOL_LEN
	HxSTD_MemSet(pPvrInfo->channelName, 0x00, sizeof(pPvrInfo->channelName));					//MXDLNA_DMS_PVR_CHANNELNAME_LEN
}

static HERROR __pl_dms_pvr_CheckValidation(DxRecListData_t *pRecListData, HUINT32 eSearchCriteria)
{
	HERROR		hErr = ERR_FAIL;
	DxCopyrightControl_CasCcType_e	eDxDrmtype = eDxCopyrightControl_CasCcType_None;

	/*
	 * - 5/11/2013: Allowed INCOMPLETED record file sharing for LiveTV bring-in problem
	 * - 3/xx/2013: Prevented INCOMPLETED record file sharing
	 *   + [redmine#66040] For accurate duration calculation, prevent addScan() while recording
	 *     : Check record status whether this is periodic scan() or caused by MSG_APP_RECORD_NOTIFY_STOPPED event
	 *     : This will be added at next scan() if ePVR_REC_STS_RECORDING
	 *   + [redmine#66603] The recording content has INCOMPLETED type will be unsupported (guided by kilee@)
	 */
	if( (pRecListData->eRecStatus != eDxREC_STS_INCOMPLETED) && (pRecListData->eRecStatus != eDxREC_STS_RECORDED) )
	{
		HxLOG_Error("[PL_DMS_PVR] Invalid record status;, Record status (%d)\n", pRecListData->eRecStatus);
		goto func_error;
	}

	/*
	 * Prevent CAS channel record sharing; Currently, only supported FTA type at HMS/DMS
	 * - Don't reference u1cScrambled flag; check DxCasType_e:eDxCAS_TYPE_FTA from DxRecListData_t field.
	 * - or rather than, access CAS info with MW_PvrService_t:SVC_CasType_t field.
	 */
	if (_pl_dms_pvr_IsAllowedCasType(pRecListData->eCasType) == FALSE) /* Also, check pstRecData->ucas */
	{
		HxLOG_Error( "[PL_DMS_PVR] Unsupported non-FTA content(%s); CAS type (%d), \n", pRecListData->szName, pRecListData->eCasType);
		goto func_error;
	}

	/* fileLock in DLib: Use bBlocked flag --> svc lock or pin lock or pr lock ... */
	if (pRecListData->bBlocked != 0) /* stPvrRecInfo.u1cLocked 대체 thru MWC_PVR_ReadRecordInfo() */
	{
		HxLOG_Error( "[PL_DMS_PVR] Unsupported svc/pin/pr  locked record (%d) \n", pRecListData->bBlocked);
		goto func_error;
	}

	if (pRecListData->bUserLock != 0)
	{
		HxLOG_Error( "[PL_DMS_PVR] Unsupported user locked record (%d) \n", pRecListData->bUserLock);
		goto func_error;
	}

	/*
	 * Changed DMR type: DrmModelType_t --> DxDRM_Type_e
	 * - DrmModelType_t
	 *   (stb_params.h, pl_common.h) 값이 CAS 마다 변할 수 있기 때문에,
	 *   CAS 를 통해 얻어야 함(local_casmain_GetDrmType)
	 */
	eDxDrmtype = pRecListData->stMeta.stCcInfo.stCasCcInfo.eCasCcType;
	if (eDxDrmtype != eDxCopyrightControl_CasCcType_None)
	{
		HxLOG_Error( "[PL_DMS_PVR] Error> Unsupported DRM type(%d) \n", eDxDrmtype);
		goto func_error;
	}

	/* check if available search criteria */
	if ( (pRecListData->eSvcType == eDxSVC_TYPE_TV)
		&& ( (eSearchCriteria != eSEARCH_CRITERIA_ITEM_ALL) && ((eSearchCriteria & eSEARCH_CRITERIA_ITEM_VIDEO) != 1) ) )
	{
		HxLOG_Error( "[PL_DMS_PVR] Error> eSvcType (%d), eItem (0x%x) \n", pRecListData->eSvcType, eSearchCriteria);
		goto func_error;
	}
	else if ( (pRecListData->eSvcType == eDxSVC_TYPE_RADIO)
		&& ( (eSearchCriteria != eSEARCH_CRITERIA_ITEM_ALL) && ((eSearchCriteria & eSEARCH_CRITERIA_ITEM_AUDIO) != 1) ) )
	{
		HxLOG_Error( "[PL_DMS_PVR] Error> eSvcType (%d), eItem (0x%x) \n", pRecListData->eSvcType, eSearchCriteria);
		goto func_error;
	}

	HxLOG_Print("[PL_DMS_PVR] Check validation success!! (0x%x)\n", pRecListData);
	hErr = ERR_OK;

func_error:
	return hErr;
}

static HBOOL _pl_dms_pvr_IsAllowedCasType(DxCasType_e eCasType)
{
	if (eCasType == eDxCAS_TYPE_FTA) return TRUE;

	return FALSE;
}

static HERROR __pl_dms_pvr_SetPropertyInfo(MXDLNA_DMS_PVR_CONTENT *pPvrInfo, DxRecListData_t *pRecListData)
{
	/* title of the pvr content */
	HxSTD_snprintf(pPvrInfo->title, sizeof(pPvrInfo->title) - 1, "%s", pRecListData->stMeta.szFileName);

	/* creator (default : HUMAX) */
	HxSTD_snprintf(pPvrInfo->creator, sizeof(pPvrInfo->creator) - 1, "%s", "HUMAX");

	/* content id of the pvr content */
	pPvrInfo->uniqueValue	= pRecListData->ulContentId;

	/* TV or RADIO */
	pPvrInfo->mediaType		= (pRecListData->eSvcType == eDxSVC_TYPE_RADIO) ? MXDLNA_DMS_PVR_CONTENT_RADIO : MXDLNA_DMS_PVR_CONTENT_TV;

	return ERR_OK;
}

static HERROR __pl_dms_pvr_SetResourceProtocolInfo(MXDLNA_DMS_PVR_CONTENT *pPvrInfo, DxRecListData_t *pRecListData)
{
	MXDLNA_PROTOCOLINFO		protocolInfo = {0,};
	HCHAR					*protocolInfoStr = NULL;
	HCHAR					*pszMimeType = NULL;
	HBOOL					isDtcpEnabled = FALSE;

	/* mime type */
	pszMimeType = (pPvrInfo->mediaType == MXDLNA_DMS_PVR_CONTENT_RADIO) ? "audio/mpeg" : "video/mpeg";

	/* resource content URI */
	HxSTD_snprintf( pPvrInfo->resource_contentURI, sizeof(pPvrInfo->resource_contentURI) - 1, "%s%d.ts", PL_DMS_DEFAULT_CONTENT_PREFIX, pPvrInfo->uniqueValue);

	/* resource bitrate, resolution */
	pPvrInfo->resource_bitrate = -1;
	pPvrInfo->resource_resolutionX = -1;
	pPvrInfo->resource_resolutionY = -1;

	/* resource rec duration */
	pPvrInfo->resource_duration = pRecListData->ulRecDuration;

	/* set default protocol info */
	__pl_dms_pvr_SetDefaultProtocolInfo(&protocolInfo);

	protocolInfo.Profile = "MPEG_TS_SD_EU_ISO";		// Profile
	protocolInfo.TM_S = 1;					// if content type is TV or RADIO
	protocolInfo.MimeType = pszMimeType;	// mime type

	/* DTCP/IP related information */
	PL_PVR_CheckDtcpState(pRecListData, &isDtcpEnabled);
	HxLOG_Print("[%s] DTCP - isEnabled(%d)\n", __FUNCTION__, isDtcpEnabled);
	if (isDtcpEnabled)
	{
		// dtcp protocol info
		protocolInfo.LinkProtection = 1;
		protocolInfo.ClearText = 1;

		// dtcp resource info
		pPvrInfo->dtcp_protection = 1;
		pPvrInfo->dtcp_emi = 12;
		pPvrInfo->dtcp_pcplength = 1024 * 1024 * 64;
	}
	else
	{
		pPvrInfo->dtcp_protection = 0;
	}

	/* Serialize */
	protocolInfoStr = mxDlna_protocolinfoSerialize( &protocolInfo );
	if(protocolInfoStr != NULL)
	{
		HxSTD_snprintf(pPvrInfo->resource_protocol, sizeof(pPvrInfo->resource_protocol) - 1, "%s", protocolInfoStr);
		mxDlna_protocolinfoStrFree(protocolInfoStr);
	}

	return ERR_OK;
}

static HERROR __pl_dms_pvr_SetThumbnailProtocolInfo(MXDLNA_DMS_PVR_CONTENT *pPvrInfo, DxRecListData_t *pRecListData)
{
	MXDLNA_PROTOCOLINFO		protocolInfo = { 0, };
	HCHAR*					protocolInfoStr = NULL;
	HCHAR					*pszThumbnailPath = NULL;
	HUINT32					ulThumbnailStillTime = 0;
	HINT32					nThumbnailstart = 0;
	HCHAR					*pMimeType = "image/jpeg";
	HCHAR					szMetaFileName[1024] = {0,};

	HxSTD_snprintf(szMetaFileName, 1023, "%s%s.hjm", pRecListData->stMeta.szPath, pRecListData->stMeta.szFileName);

	if( pPvrInfo->mediaType != MXDLNA_DMS_PVR_CONTENT_TV )
	{
		HxLOG_Error("We only support thumbnail for Video Content! (media type : %d) \n", pPvrInfo->mediaType);
		return ERR_FAIL;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	// Adjust Still image time ( please refer opl_recording_GetThumbnail() in oplrecording.c ) //
	/////////////////////////////////////////////////////////////////////////////////////////////
	ulThumbnailStillTime = (pRecListData->ulLastViewTime < 10 ) ? 10 : pRecListData->ulLastViewTime;

	if(ulThumbnailStillTime == 10 && pRecListData->bEventBased == TRUE)
	{
		if(pRecListData->stEvt.ulStartTime == 0)
		{
			if(10 < pRecListData->ulRecDuration)
			{
				nThumbnailstart = 10;
			}
			else
			{
				HxLOG_Error("Adjust Still image time fail! (case 1)\n");
				return ERR_FAIL;
			}
		}
		else
		{
			nThumbnailstart = (pRecListData->stEvt.ulStartTime - pRecListData->ulRecStartTime) + 10;

			if(nThumbnailstart < 10) //방어 코드
			{
				nThumbnailstart = 10;
			}
		}

		if(pRecListData->ulRecDuration < nThumbnailstart)
		{
			if(pRecListData->eRecStatus == eDxREC_STS_RECORDED || pRecListData->eRecStatus == eDxREC_STS_INCOMPLETED)
			{
				// start padding 구간에서 정지된 녹화물
				nThumbnailstart = 10;
			}
			else
			{
				// 아직 이벤트 시간안에 들어오지 않은 녹화 중 아이템 (start padding time) 은 thumbnail 요청 하지 않는다.
				HxLOG_Error("Adjust Still image time fail! (case 2)\n");
				return ERR_FAIL;
			}
		}
		ulThumbnailStillTime = nThumbnailstart;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////

	pszThumbnailPath = (HCHAR*)THAPI_GetThumbnailURL(szMetaFileName, ulThumbnailStillTime, NULL, TRUE);
	if( pszThumbnailPath == NULL)
	{
		HxLOG_Error("THAPI_GetThumbnailURL() Error! \n");
		return ERR_FAIL;
	}

	if( PL_FS_IsFileExist(pszThumbnailPath) == FALSE )
	{
		HxLOG_Error("PL_FS_IsFileExist() Error! Not exist file (%s)\n", pszThumbnailPath);
		return ERR_FAIL;
	}

	/* thumbnail content URI */
	HxSTD_snprintf( pPvrInfo->thumbnail_contentURI, sizeof(pPvrInfo->thumbnail_contentURI) - 1, "%s", pszThumbnailPath);

	__pl_dms_pvr_SetDefaultProtocolInfo( &protocolInfo );

	protocolInfo.MimeType = pMimeType;
	protocolInfo.Profile = "JPEG_SM";
	protocolInfo.TM_I = 1;
	protocolInfo.IsConvertedContent = 1;

	/* Serialize */
	protocolInfoStr = mxDlna_protocolinfoSerialize( &protocolInfo );
	if(protocolInfoStr != NULL)
	{
		HxSTD_snprintf(pPvrInfo->thumbnail_protocol, sizeof(pPvrInfo->thumbnail_protocol) - 1, "%s", protocolInfoStr);
		mxDlna_protocolinfoStrFree(protocolInfoStr);
	}

	return ERR_OK;
}

static HCHAR * __pl_dms_pvr_MakeTimeStringGMT(time_t current_time)
{
	struct tm *gmt;
	HCHAR * szTime = NULL;

	szTime = (HCHAR*)HLIB_STD_MemCalloc(50);
	HxSTD_MemSet(szTime,0,50);

	gmt = gmtime(&current_time );
	{
		HxSTD_sprintf(szTime,"%d-%.2d-%.2dT%.2d:%.2d:%.2d", gmt->tm_year+1900, gmt->tm_mon+1, gmt->tm_mday,gmt->tm_hour,gmt->tm_min, gmt->tm_sec);
	}

	return szTime;
}

static HCHAR * __pl_dms_pvr_MakeTimeStringLocalTime(time_t current_time)
{
	struct tm *gmt;
	HCHAR * szTime = NULL;

	szTime = (HCHAR*)HLIB_STD_MemCalloc(50);
	HxSTD_MemSet(szTime,0,50);

	gmt = localtime(&current_time );
	{
		HxSTD_sprintf(szTime,"%d-%.2d-%.2dT%.2d:%.2d:%.2d", gmt->tm_year+1900, gmt->tm_mon+1, gmt->tm_mday,gmt->tm_hour,gmt->tm_min, gmt->tm_sec);
	}

	return szTime;
}

// #define PL_DMS_PVR_KERNEL_UUID_PATH "/proc/sys/kernel/random/uuid"

static HERROR __pl_dms_pvr_SetContentAdditionalInfo(MXDLNA_DMS_PVR_CONTENT *pPvrInfo, DxRecListData_t *pRecListData)
{
	HERROR hErr = ERR_FAIL;
	HINT32 nMallocSize = 0;
	HINT32 nLenChannelAdditionalInfo = 0;
	HCHAR *szChannelAdditionalInfo = NULL;
	HCHAR *szDuration = NULL;
	HCHAR *szEpisodeNumber = NULL;
	HCHAR *szSeasonNumber = NULL;
	HCHAR *szChannelNumber = NULL;
	HCHAR *szChannelName = NULL;
	HCHAR *szVideoType = NULL;
	HCHAR *szRecStartTime = NULL;
	HCHAR tmpNumberString[100];

	if(pRecListData == NULL)
	{
		szChannelAdditionalInfo = HLIB_STD_StrDup("rec_duration=0,episode_number=-1,season_number=-1,channel_number=-1");
	}
	else
	{
		// rec_duration
		HxSTD_sprintf(tmpNumberString, "%d", pRecListData->ulRecDuration);
		nMallocSize = HxSTD_StrLen("rec_duration=,") + HxSTD_StrLen(tmpNumberString) + 1;
		szDuration = HLIB_STD_MemCalloc(nMallocSize);
		if(szDuration == NULL)
		{
			HxLOG_Error("__pl_dms_pvr_SetContentAdditionalInfo() malloc error!");
			goto func_error;
		}
		HxSTD_sprintf(szDuration, "rec_duration=%s", tmpNumberString);
		nLenChannelAdditionalInfo += nMallocSize;

		// episode_number
		if(pRecListData->stEvt.ulEpisodeNum>0)
			HxSTD_sprintf(tmpNumberString, "%d", pRecListData->stEvt.ulEpisodeNum);
		else
			HxSTD_sprintf(tmpNumberString, "%s", "-1");
		nMallocSize = HxSTD_StrLen("episode_number=,") + HxSTD_StrLen(tmpNumberString) + 1;
		szEpisodeNumber = HLIB_STD_MemCalloc(nMallocSize);
		if(szEpisodeNumber == NULL)
		{
			HxLOG_Error("__pl_dms_pvr_SetContentAdditionalInfo() malloc error!");
			goto func_error;
		}
		HxSTD_sprintf(szEpisodeNumber, "episode_number=%s", tmpNumberString);
		nLenChannelAdditionalInfo += nMallocSize;

		// season_number
		if(pRecListData->stEvt.ulSeason>0)
			HxSTD_sprintf(tmpNumberString, "%d", pRecListData->stEvt.ulSeason);
		else
			HxSTD_sprintf(tmpNumberString, "%s", "-1");
		nMallocSize = HxSTD_StrLen("season_number=,") + HxSTD_StrLen(tmpNumberString) + 1;
		szSeasonNumber = HLIB_STD_MemCalloc(nMallocSize);
		if(szSeasonNumber == NULL)
		{
			HxLOG_Error("__pl_dms_pvr_SetContentAdditionalInfo() malloc error!");
			goto func_error;
		}
		HxSTD_sprintf(szSeasonNumber, "season_number=%s", tmpNumberString);
		nLenChannelAdditionalInfo += nMallocSize;

		// channel_number
		if(pRecListData->nSvcNo>=0)
			HxSTD_sprintf(tmpNumberString, "%d", pRecListData->nSvcNo);
		else
			HxSTD_sprintf(tmpNumberString, "%s", "-1");
		nMallocSize = HxSTD_StrLen("channel_number=,") + HxSTD_StrLen(tmpNumberString) + 1;
		szChannelNumber = HLIB_STD_MemCalloc(nMallocSize);
		if(szChannelNumber == NULL)
		{
			HxLOG_Error("__pl_dms_pvr_SetContentAdditionalInfo() malloc error!");
			goto func_error;
		}
		HxSTD_sprintf(szChannelNumber, "channel_number=%s", tmpNumberString);
		nLenChannelAdditionalInfo += nMallocSize;

		// channel_name
		if(pRecListData->szSvcName!=NULL)
		{
			nMallocSize = HxSTD_StrLen("channel_name=,") + HxSTD_StrLen(pRecListData->szSvcName) + 1;
			szChannelName = HLIB_STD_MemCalloc(nMallocSize);
			if(szChannelName == NULL)
			{
				HxLOG_Error("__pl_dms_pvr_SetContentAdditionalInfo() malloc error!");
				goto func_error;
			}
			HxSTD_sprintf(szChannelName, "channel_name=%s", pRecListData->szSvcName);
			nLenChannelAdditionalInfo += nMallocSize;
		}

		// video_type
		if(pRecListData->eVideoType == eDxVIDEO_TYPE_HD || pRecListData->eVideoType == eDxVIDEO_TYPE_SD)
		{
			nMallocSize = HxSTD_StrLen("video_type=,") + HxSTD_StrLen("HD") + 1;
			szVideoType = HLIB_STD_MemCalloc(nMallocSize);
			if(szVideoType == NULL)
			{
				HxLOG_Error("__pl_dms_pvr_SetContentAdditionalInfo() malloc error!");
				goto func_error;
			}
			if(pRecListData->eVideoType == eDxVIDEO_TYPE_HD)
				HxSTD_sprintf(szVideoType, "video_type=%s", "HD");
			else
				HxSTD_sprintf(szVideoType, "video_type=%s", "SD");
			nLenChannelAdditionalInfo += nMallocSize;
		}

		// rec_starttime
		if(pRecListData->ulRecStartTime>0)
		{
			HCHAR *szStartTimeGMT = NULL;
			{
				printf(HxANSI_COLOR_YELLOW("tzname:[%s], timezone:%d, daylight:%d\n"), tzname, timezone, daylight);
				printf(HxANSI_COLOR_YELLOW("RecStartTime:%u\n"), pRecListData->ulRecStartTime);
			}

			/* 2017/11/02 mhkang: http://alm.humaxdigital.com/cb/issue/1573816
			 * Instead of __pl_dms_pvr_MakeTimeStringGMT, localtime must be used.
			*/
			szStartTimeGMT = __pl_dms_pvr_MakeTimeStringLocalTime((time_t)pRecListData->ulRecStartTime);

			if(szStartTimeGMT != NULL)
			{
				nMallocSize = HxSTD_StrLen("rec_starttime=,") + HxSTD_StrLen(szStartTimeGMT) + 1;
				szRecStartTime = HLIB_STD_MemCalloc(nMallocSize);
				if(szRecStartTime == NULL)
				{
					HxLOG_Error("__pl_dms_pvr_SetContentAdditionalInfo() malloc error!");
					HLIB_STD_MemFree(szStartTimeGMT);
					goto func_error;
				}
				HxSTD_sprintf(szRecStartTime, "rec_starttime=%s", szStartTimeGMT);
				HLIB_STD_MemFree(szStartTimeGMT);
				nLenChannelAdditionalInfo += nMallocSize;
			}
		}

		// Serialization
		nLenChannelAdditionalInfo++;
		szChannelAdditionalInfo = HLIB_STD_MemCalloc(nLenChannelAdditionalInfo);
		if(szChannelAdditionalInfo == NULL)
		{
			HxLOG_Error("__pl_dms_pvr_SetContentAdditionalInfo() malloc error!");
			goto func_error;
		}

		HxSTD_sprintf(szChannelAdditionalInfo, "%s,%s,%s,%s", szDuration, szEpisodeNumber, szSeasonNumber, szChannelNumber);
		if(szChannelName!=NULL)
		{
			HxSTD_StrCat(szChannelAdditionalInfo, ",");
			HxSTD_StrCat(szChannelAdditionalInfo, szChannelName);
		}

		if(szVideoType!=NULL)
		{
			HxSTD_StrCat(szChannelAdditionalInfo, ",");
			HxSTD_StrCat(szChannelAdditionalInfo, szVideoType);
		}

		if(szRecStartTime!=NULL)
		{
			HxSTD_StrCat(szChannelAdditionalInfo, ",");
			HxSTD_StrCat(szChannelAdditionalInfo, szRecStartTime);
		}
	}

	pPvrInfo->pContent_Additional_Info = szChannelAdditionalInfo;
	hErr = ERR_OK;

func_error:
	if(szDuration!= NULL)		HLIB_STD_MemFree(szDuration);
	if(szEpisodeNumber!=NULL)	HLIB_STD_MemFree(szEpisodeNumber);
	if(szSeasonNumber!=NULL)	HLIB_STD_MemFree(szSeasonNumber);
	if(szChannelNumber!=NULL)	HLIB_STD_MemFree(szChannelNumber);
	if(szChannelName!=NULL)		HLIB_STD_MemFree(szChannelName);
	if(szVideoType!=NULL)		HLIB_STD_MemFree(szVideoType);
	if(szRecStartTime!=NULL)	HLIB_STD_MemFree(szRecStartTime);

	return hErr;
}

static HERROR __pl_dms_pvr_SetResourceInfo(MXDLNA_DMS_PVR_CONTENT *pPvrInfo, DxRecListData_t *pRecListData)
{
	HERROR			hErr = ERR_FAIL;

	// [RESOURCE PROTOCOL]
	hErr = __pl_dms_pvr_SetResourceProtocolInfo(pPvrInfo, pRecListData);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("__pl_dms_pvr_SetResourceProtocolInfo() Error! \n");
		return ERR_FAIL;
	}

	// [FILE SIZE]
	hErr = __pl_dms_pvr_GetFileSize(pRecListData, &pPvrInfo->size);		// PVR content size from db (byte) (flexible)
	if(hErr != ERR_OK)
	{
		/* 2016/1/10 mhkang: http://svn.humaxdigital.com:3000/issues/109249#note-16
			HJM 파일은 존재하지만 TS파일이 없는 에러상황이 발생한는데,
			이때 AV Media Controller에서 CDS Browsing하면 무한히 로그를 찍는 경우가 발생한다.
			=> TS파일이 없는 경우 Size=0 인 상태로 CDS를 만들도록 수정함.
		*/
		HxLOG_Error("__pl_dms_pvr_GetFileSize() Error! (size : %lld)\n", pPvrInfo->size);
	}
	else
	{
		/* TS file이 유효하지 않은 경우에는 thumbnail protocol info를 생성할 필요가 없다 */
		// [THUMBNAIL PROTOCOL]
		hErr = __pl_dms_pvr_SetThumbnailProtocolInfo(pPvrInfo, pRecListData);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("__pl_dms_pvr_SetThumbnailProtocolInfo() Error! \n");
		}
	}

	// [ADDITIONAL INFO]
	hErr = __pl_dms_pvr_SetContentAdditionalInfo(pPvrInfo, pRecListData);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("__pl_dms_pvr_SetContentAdditionalInfo() Error! \n");
	}

	// [OTHERS]
	/* channel name, start time, end time*/
	HxSTD_snprintf(pPvrInfo->channelName, sizeof(pPvrInfo->channelName), "%s", pRecListData->szSvcName);
	pPvrInfo->startTime = pRecListData->ulRecStartTime;					// time_t format
	pPvrInfo->endTime = pPvrInfo->startTime + pRecListData->ulRecDuration;	// time_t format

	return ERR_OK;
}

static HERROR __pl_dms_pvr_MakeContentInfo( MXDLNA_DMS_PVR_CONTENT *pPvrInfo, DxRecListData_t *pRecListData, HUINT32 eSearchCriteria )
{
	HERROR		hErr = ERR_FAIL;

	if(pPvrInfo == NULL || pRecListData == NULL)
	{
		return ERR_FAIL;
	}

	/* 1. set upnp property */
	hErr = __pl_dms_pvr_SetPropertyInfo(pPvrInfo, pRecListData);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("[PL_DMS_PVR] __pl_dms_pvr_SetPropertyInfo Error!! (0x%x)\n", pRecListData);
		return ERR_FAIL;
	}

	/* 2. set upnp resource info included in upnp property */
	hErr = __pl_dms_pvr_SetResourceInfo(pPvrInfo, pRecListData);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("[PL_DMS_PVR] __pl_dms_pvr_SetResourceInfo Error!! (0x%x)\n", pRecListData);
		return ERR_FAIL;
	}

	return ERR_OK;
}

// DAMA CALLBACK
static void __pl_dms_pvr_OnDamaContentCountReceived(const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	DAxIterator_t		iter;
	HUINT32			*count = (HUINT32*)userdata;
	HUINT32			cnt = 0;
	DxRecListData_t		*pPvrContent;

	DAPI_InitIterator(&iter, data, size, 0);
	while (DAPI_NextIterator(&iter))
	{
		pPvrContent = (DxRecListData_t *)DLIB_NewType(iter.name, iter.uid, iter.data, iter.size);
		if(pPvrContent)
		{
			if (__pl_dms_pvr_CheckValidation(pPvrContent, eSEARCH_CRITERIA_ITEM_ALL) == ERR_OK)
			{
				cnt++;
			}

			DLIB_FreeType(iter.name, pPvrContent);
		}
	}

	*count = cnt;

	return;
}

static void __pl_dms_pvr_OnDamaContentListReceived(const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	DxRecListData_t		*pPvrContent;
	DAxIterator_t		iter;
	HxPair_t			*pstPair = (HxPair_t*)userdata;
	HUINT32				eSearchCriteria = (HUINT32)pstPair->left;
	HxList_t			**ppPvrContentList = (HxList_t**)pstPair->right;
	HxList_t			*pstList = NULL;

	DAPI_InitIterator(&iter, data, size, 0);
	while (DAPI_NextIterator(&iter))
	{
		HxLOG_Print("\t\t +++++ name : %s, uid : %d, size : %d\n", iter.name, iter.uid, iter.size);

		pPvrContent = (DxRecListData_t *)DLIB_NewType(iter.name, iter.uid, iter.data, iter.size);
		if(pPvrContent)
		{
			DxRecListData_t	*pstNewPvrContent;

			if (__pl_dms_pvr_CheckValidation(pPvrContent, eSearchCriteria) == ERR_OK)
			{
				pstNewPvrContent = (DxRecListData_t *)HLIB_STD_MemDup(pPvrContent, sizeof(DxRecListData_t));
				pstList = HLIB_LIST_Append(pstList, pstNewPvrContent);
			}

			DLIB_FreeType(iter.name, pPvrContent);
		}
	}

	*ppPvrContentList = pstList;

	return;
}

static void __pl_dms_pvr_OnDamaContentInfoReceived(const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	DxRecListData_t				*pPvrContent;
	DAxIterator_t				iter;
	DxRecListData_t		**ppPvrContentInfo = (DxRecListData_t**)userdata;

	DAPI_InitIterator(&iter, data, size, 0);
	while (DAPI_NextIterator(&iter))
	{
		HxLOG_Print("\t\t +++++ name : %s, uid : %d, size : %d\n", iter.name, iter.uid, iter.size);

		pPvrContent = (DxRecListData_t *)DLIB_NewType(iter.name, iter.uid, iter.data, iter.size);
		if(pPvrContent)
		{
			DxRecListData_t	*pstNewPvrContent;

			if (__pl_dms_pvr_CheckValidation(pPvrContent, eSEARCH_CRITERIA_ITEM_ALL) == ERR_OK)
			{
				pstNewPvrContent = (DxRecListData_t *)HLIB_STD_MemDup(pPvrContent, sizeof(DxRecListData_t));
				*ppPvrContentInfo = pstNewPvrContent;
			}

			DLIB_FreeType(iter.name, pPvrContent);
			break;
		}
	}

	return;
}

static HUINT32 __pl_dms_pvr_ParseSearchCriteria(HCHAR *search_criteria)
{
	HUINT32 eSearchCriteria = eSEARCH_CRITERIA_ITEM_NONE;

	HxLOG_Print("search_criteria (%s) --> action (%s)\n", search_criteria, (search_criteria == NULL) ? "BROWSE" : "SEARCH");
	if(search_criteria == NULL)
	{
		// CDS::browse() action
		eSearchCriteria = eSEARCH_CRITERIA_ITEM_ALL;
	}
	else
	{
		// CDS::search() action
		if(HxSTD_StrStr(search_criteria, PL_DMS_OBJECT_ITEM_VIDEOITEM) != NULL)
		{
			eSearchCriteria |= eSEARCH_CRITERIA_ITEM_VIDEO;
		}

		if(HxSTD_StrStr(search_criteria, PL_DMS_OBJECT_ITEM_AUDIOITEM) != NULL)
		{
			eSearchCriteria |= eSEARCH_CRITERIA_ITEM_AUDIO;
		}

		if(HxSTD_StrStr(search_criteria, PL_DMS_OBJECT_ITEM_IMAGEITEM) != NULL)
		{
			eSearchCriteria |= eSEARCH_CRITERIA_ITEM_IMAGE;
		}

		HxLOG_Print("[Before] eSearchCriteria : 0x%08x\n", eSearchCriteria);

		if(eSearchCriteria == eSEARCH_CRITERIA_ITEM_IMAGE)
		{
			eSearchCriteria = eSEARCH_CRITERIA_ITEM_NONE;
		}
		else if(eSearchCriteria == eSEARCH_CRITERIA_ITEM_NONE)
		{
			eSearchCriteria = eSEARCH_CRITERIA_ITEM_ALL;
		}

		HxLOG_Print("[After] eSearchCriteria : 0x%08x\n", eSearchCriteria);
	}

	HxLOG_Print("[Result] eSearchCriteria : 0x%08x\n", eSearchCriteria);

	return eSearchCriteria;
}

// MXLIB PVR CALLBACK
// get PVR Content count
static long __cb_pvr_getContentCount(void)
{
	int count = 0;
	HCHAR		query[1024] = {0,};
	HUINT32		id;
	DERROR		dErr;

	HxSTD_snprintf(query, 1024, "select * from DxRecListData_t");

	// 1. get PVR Content count
	id = DAPI_Query(query, __pl_dms_pvr_OnDamaContentCountReceived, (void*)&count);
	dErr = DAPI_Wait(10000, id);
	if (id == 0 || dErr != DERR_OK)
	{
		HxLOG_Error("Query FAIL \n");
		return ERR_FAIL;
	}

	HxLOG_Print("\t\t+++++ count : %d\n", count);

	// 2. return count

	return count;
}

// get all PVR Content list
// PVR Content list must be read from zodiac database. range : startIdx ~ startIdx + requestedCount
/* In case of Browse action:
   @search_criteria : always null.  (syntax : refer to ContentDirectory Service doc / 2.5.5. A_ARG_TYPE_SearchCriteria)
   @sort_criteria	: (default)"upnp:class",  ex)  +dc:title, -dc:title, +upnp:channelNr,...
   @startIdx		: StartIndex of Channel
   @requestedCount	: Requested Channel Count by DMC
*/
static MXDLNA_DMS_PVR_CONTENT* __cb_pvr_getContentList( char *search_criteria, char *sort_criteria ,int startIdx, int requestedCount )
{
	HERROR					hErr = ERR_FAIL;
	HUINT32					eSearchCriteria = eSEARCH_CRITERIA_ITEM_NONE;
	MXDLNA_DMS_PVR_CONTENT *pPvrInfo = NULL;
	MXDLNA_DMS_PVR_CONTENT *pPvrList = NULL, *pTempPvrList = NULL;
	HUINT32					ulAddChCount = 0;
	HUINT32					i;
	HxPair_t				*pstPair = NULL;
	HxList_t				*pPvrContentList = NULL;
	HUINT32					ulPvrContentListLength = 0;
	DxRecListData_t			*pRecListData = NULL;
	HCHAR		query[1024] = {0,};
	HUINT32		id;
	DERROR		dErr;

	int nPvrContentCount = __cb_pvr_getContentCount();		// PVR db count

	// 1. get PVR Content List
	if (startIdx > nPvrContentCount)
	{
		// ERROR
		return NULL;
	}

	// 2. adjustment request count max value to 20
	if (requestedCount > PL_DMS_PVR_MAX_CONTENT_NUM)
	{
		requestedCount = PL_DMS_PVR_MAX_CONTENT_NUM;
	}

	eSearchCriteria = __pl_dms_pvr_ParseSearchCriteria(search_criteria);
	if(eSearchCriteria == eSEARCH_CRITERIA_ITEM_NONE)
	{
		HxLOG_Error("[ PL_DMS_PVR ] eItem (eSEARCH_CRITERIA_ITEM_NONE) ---> return NULL!!\n");
		return NULL;
	}

	pstPair = (HxPair_t *)HLIB_STD_MemCalloc(sizeof(HxPair_t));
	if(pstPair == NULL)
	{
		HxLOG_Error("[ PL_DMS_PVR ] Not enough memory!!\n");
		return NULL;
	}

	pstPair->left = (void*)eSearchCriteria;		// input
	pstPair->right = (void*)&pPvrContentList;	// output

	HxSTD_snprintf(query, sizeof(query) - 1, "select * from DxRecListData_t order by starttime DESC limit %d offset %d", requestedCount, startIdx);

	id = DAPI_Query(query, __pl_dms_pvr_OnDamaContentListReceived, (void*)pstPair);
	dErr = DAPI_Wait(10000, id);
	if (id == 0 || dErr != DERR_OK)
	{
		HLIB_STD_MemFree(pstPair);
		HxLOG_Error("Query FAIL \n");
		return NULL;
	}

	// 3. select the item with index
	// 4. fill the PVR Content information
	ulPvrContentListLength = HLIB_LIST_Length(pPvrContentList);

	for(i = 0; i < ulPvrContentListLength; i++)
	{
		HxList_t *tmp = HLIB_LIST_GetListItem(HLIB_LIST_First(pPvrContentList), i);

		pRecListData = (DxRecListData_t*)HLIB_LIST_Data(tmp);
		if(pRecListData == NULL)
		{
			HxLOG_Error("[ PL_DMS_PVR ][idx : %d] pRecListData == NULL!!\n", i);
			continue;
		}

		HxLOG_Print("content id : %d, name : %s, starttime : %d, duration : %d\n",
			pRecListData->ulContentId, pRecListData->szName, pRecListData->ulRecStartTime, pRecListData->ulRecDuration);

		pPvrInfo = (MXDLNA_DMS_PVR_CONTENT*)HLIB_STD_MemAlloc(sizeof(MXDLNA_DMS_PVR_CONTENT));
		if(pPvrInfo == NULL)
		{
			HxLOG_Error("[ PL_DMS_PVR ] malloc error!!\n");
			continue;
		}

		// initialize PVR info
		__pl_dms_pvr_InitPvrInfo(pPvrInfo);

		hErr = __pl_dms_pvr_MakeContentInfo(pPvrInfo, pRecListData, eSearchCriteria);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("[ PL_DMS_PVR ] __pl_dms_pvr_MakeContentInfo() error!!\n");
			HLIB_STD_MemFree(pPvrInfo);
			continue;
		}

		if(ulAddChCount == 0)
		{
			pPvrList = pPvrInfo;
			pTempPvrList = pPvrList;
		}
		else
		{
			pTempPvrList->pNextContent = pPvrInfo;
			pTempPvrList = pTempPvrList->pNextContent;
		}

		ulAddChCount++;
	}

	HLIB_LIST_RemoveAllFunc(pPvrContentList, HLIB_STD_MemFree_CB);
	HLIB_STD_MemFree(pstPair);

	HxLOG_Print( "[ PL_DMS_PVR ] PVR Content Add Count( %d )\n", ulAddChCount );

	// 5. return pvr content list
	return pPvrList;		// this value will be freed in our library
}

// get specific pvr content information with unique value
static MXDLNA_DMS_PVR_CONTENT* __cb_pvr_getContentInfo( int uniqValue )
{
	HERROR		hErr = ERR_FAIL;
	MXDLNA_DMS_PVR_CONTENT *pPvrInfo = NULL;
	DxRecListData_t			*pRecListData = NULL;
	HCHAR		query[1024] = {0,};
	HUINT32		id;
	DERROR		dErr;

	HxSTD_snprintf(query, sizeof(query) - 1, "select * from DxRecListData_t where uid=%d", uniqValue);

	id = DAPI_Query(query, __pl_dms_pvr_OnDamaContentInfoReceived, (void*)&pRecListData);
	dErr = DAPI_Wait(10000, id);
	if (id == 0 || dErr != DERR_OK)
	{
		HxLOG_Error("Query FAIL \n");
		return NULL;
	}

	if(pRecListData == NULL)
	{
		HxLOG_Error("There is no matched data!\n");
		return NULL;
	}

	if(pRecListData->ulContentId == uniqValue)
	{
		HxLOG_Print("\t\t content id : %d, name : %s, starttime : %d, duration : %d\n",
			pRecListData->ulContentId, pRecListData->szName, pRecListData->ulRecStartTime, pRecListData->ulRecDuration);

		pPvrInfo = (MXDLNA_DMS_PVR_CONTENT*)HLIB_STD_MemAlloc(sizeof(MXDLNA_DMS_PVR_CONTENT));
		if(pPvrInfo == NULL)
		{
			return NULL;
		}

		// initialize PVR info
		__pl_dms_pvr_InitPvrInfo(pPvrInfo);

		hErr = __pl_dms_pvr_MakeContentInfo(pPvrInfo, pRecListData, eSEARCH_CRITERIA_ITEM_ALL);
		if(hErr != ERR_OK)
		{
			HLIB_STD_MemFree(pPvrInfo);
			return NULL;
		}
	}
	else
	{
		HxLOG_Error("Value different!! pRecListData->ulContentId (%d) uniqValue (%d)\n", pRecListData->ulContentId, uniqValue);
	}

	HLIB_STD_MemFree(pRecListData);

	return pPvrInfo;		// this value will be freed in our library
}

#define ________________Public_Interfaces__________________________


/* PVR Container를 만들고, 관련 Callback을 채워주는 함수. */
void PL_DMS_SetupForPVR( void )
{
	MXDLNA_DMS_PVR_CALLBACK		stPvrCallback = {0};
//	HINT32						nRet = 0;

	if (PL_DMS_Get_MxDlna_Res(&pDmsHandles) != ERR_OK)
	{
		HxLOG_Error("Error> Cant get resHandle!\n");
		return;
	}

	stPvrCallback.fnGetContentCount = __cb_pvr_getContentCount;
	stPvrCallback.fnSearchContent	= __cb_pvr_getContentList;
	stPvrCallback.fnGetContent		= __cb_pvr_getContentInfo;

	mxDlnaDmsPvr_setCallback(&stPvrCallback);
#if 0	// 2015/12/16 mhkang:  http://svn.humaxdigital.com:3000/issues/109277#note-13
	nRet = mxDlnaDmsPvr_createRootContainer(pDmsHandles.hDms, PL_DMS_DEFAULT_CONTENT_CONTAINER_TITLE);

	HxLOG_Print( "[ PL_DMS ] mxDlnaDmsPvr_createRootContainer( ret = %d )\n", nRet );
#endif
	return;
}

void PL_DMS_GetPracticalPVRPath(const HCHAR *pszVirtualPath, HCHAR *pszPracticalPath, HINT32 nPracticalPathLen)
{
	/*
	 1. virtual path로 부터 uid를 얻어옴
	 2. uid를 조건으로 하여 dapi_query 수행
	 3. 결과에서 path를 얻어서 return
	*/
	HCHAR				tmpDirectiveObj[256] = {0,};
	HCHAR				*extensionLocation = NULL;
	int					pvrContentID = 0;

	DxRecListData_t		*pRecListData = NULL;
	HCHAR				query[1024] = {0,};
	HCHAR				szMetaFileName[1024] = {0,};
	HUINT32				id;
	DERROR				dErr;

	HxSTD_snprintf(tmpDirectiveObj, sizeof(tmpDirectiveObj) - 1, pszVirtualPath + 14);		/* 14 ---> except for prefix "/pvr/HCONTENT:" */
	HxLOG_Print("tmpDirectiveObj : %s\n", tmpDirectiveObj);

	extensionLocation = HxSTD_StrRChar( tmpDirectiveObj, '.');
	if ( extensionLocation )
	{
		*extensionLocation = '\0';
		pvrContentID = atoi(tmpDirectiveObj);
	}

	if ( pvrContentID > 0 )
	{
		HxSTD_snprintf(query, sizeof(query) - 1, "select * from DxRecListData_t where uid=%d", pvrContentID);

		id = DAPI_Query(query, __pl_dms_pvr_OnDamaContentInfoReceived, (void*)&pRecListData);
		dErr = DAPI_Wait(10000, id);
		if (id == 0 || dErr != DERR_OK)
		{
			HxLOG_Error("Query FAIL!! \n");
			return;
		}

		if(pRecListData == NULL)
		{
			HxLOG_Error("There is no matched data!\n");
			return;
		}

		HxSTD_snprintf(szMetaFileName, sizeof(szMetaFileName) - 1, "%s%s.hjm", pRecListData->stMeta.szPath, pRecListData->stMeta.szFileName);

		HxSTD_MemCopy(pszPracticalPath, szMetaFileName, nPracticalPathLen - 1);
	}
}
#endif

