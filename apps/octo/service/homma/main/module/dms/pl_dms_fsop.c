/**************************************************************
 *	@file	pl_dms.c
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

#define ________________Defines____________________________________
#if defined(CONFIG_HOMMA_CDI)
#define __PL_DMS_SUPPORT_FS_SECURE__				1
#define __PL_DMS_SUPPORT_FS_CHUNK__					1
#else
#define __PL_DMS_SUPPORT_FS_SECURE__				0
#define __PL_DMS_SUPPORT_FS_CHUNK__					1 /* Supported plain chunk-io under non-crypt PVR record */
#endif

#if __PL_DMS_SUPPORT_FS_CHUNK__
#define __PL_DMS_SUPPORT_EMULATE_CHUNKED_FILES__
#endif

/* obama와 공유해야 하는 필드로 public define 필요 */
#define FILEHANDLE_HEAD_DTCP			0x0FFF0000
#define FILEHANDLE_HEAD_LIVESTREAM		0x1FFF0000
#define FILEHANDLE_HEAD_CHUNKEDFILE		0x2FFF0000
#define FILEHANDLE_HEAD_MASK			0xFFFF0000
#define FILEHANDLE_TAIL_MASK			0x0000FFFF

/* Define Protocol Prefix */
#define PL_DMS_PROTOCOL_PREFIX_HTTP				"http://"
#define PL_DMS_PROTOCOL_PREFIX_HTTPS			"https://"
#define PL_DMS_PROTOCOL_PREFIX_MMS				"mms://"
#define PL_DMS_PROTOCOL_PREFIX_RTSP				"rtsp://"

/* Define File Extension */
#define PL_DMS_TSFILE_EXT						".ts"

#define PL_DMS_MAX_MEDIA_DRMINFO_NUM			(2) /* 동기화 필요: PAL_MEDIA_DRMINFO_MAXNUM in pal_media.h */
#define PL_DMA_MAX_USERIO_INST					(255)
#define PL_DMS_MEDIA_FILE_DRMINFO_MAX_LENGTH	(4)

/* DMS userio callback에서 사용하는 error, mxlib error 대체 필요 */
#define _DEF_DMS_FILE_OK					0
#define _DEF_DMS_FILE_ERROR					-1
#define	_DEF_DMS_FILE_ERROR_EOF				-2

#define ________________Typedef____________________________________
typedef enum {
	PL_MEDIA_RECORD_IO_TYPE_NA=0,
	PL_MEDIA_RECORD_IO_TYPE_SINGLE,		/* Single record */
	PL_MEDIA_RECORD_IO_TYPE_DELAYED		/* Delayed record(Nexus Chunked IO) */
} PL_DMS_RECORD_IO_TYPE_e;

typedef enum
{
	MW_MEDIA_USERINSTANCE_TYPE_NONE,
	MW_MEDIA_USERINSTANCE_TYPE_DRM,		// Drm
	MW_MEDIA_USERINSTANCE_TYPE_MP3		// MP3
} PL_DMS_USERINSTANCE_TYPE_e;

typedef struct
{
	HCHAR valid;
	HCHAR *url;
	HCHAR drminfo[PL_DMS_MAX_MEDIA_DRMINFO_NUM];
	HINT32 number;
	PL_DMS_USERINSTANCE_TYPE_e instancetype;
} PL_DMS_UserInstance_t;

#define ________________Static_Variables___________________________
static HUINT16					s_usUseTransfer;		/* DMS 에서 Transfer 동작이 진행중이면 */
static PL_DMS_UserInstance_t	s_tUserioInstance[PL_DMA_MAX_USERIO_INST];

#ifdef	__DMS_DEBUG_DUMP_CHUNK__
FILE			*s_fpDebugChunk = NULL;
int			s_nDebugChunkDumpCount = 0;
char			*s_pDebugChunkDumpPath = NULL;
#endif

#define ________________Internal_Functions_________________________
extern void PL_DMS_GetPracticalPVRPath(const HCHAR *pszVirtualPath, HCHAR *pszPracticalPath, HINT32 nPracticalPathLen);

/**
 * @brief		현재 사용되지 않는 User Instance ID를 얻는다.
 *
 * @param[out]	punUserInstanceId 	User Instance ID
 * @return		HERROR
 * @see
 */
STATIC HERROR __pldms_GetUserInstance(HUINT32 *punUserInstanceId)
{
	HERROR		nRet = ERR_OK;
	HUINT32		i=0;

	for(i = 0;i<PL_DMA_MAX_USERIO_INST;i++)
	{
		if(s_tUserioInstance[i].valid == FALSE)
		{
			*punUserInstanceId = i;
			s_tUserioInstance[i].valid = TRUE;
			break;
		}
	}

	if(i == PL_DMA_MAX_USERIO_INST)
	{
		HxLOG_Print("[%s] User Instance ID is full.\n", __func__);
		nRet = ERR_FAIL;
	}

	return nRet;
}

/**
 * @brief		Url을 통해 User Instance Id가 유효한지 검사하며 Instance Id를 얻는다.
 *
 * @param[in]	pcUrl 				Url
 * @param[out]	pnUserInstanceId 	User Instance ID
 * @return		HERROR
 * @see
 */
STATIC HERROR __pldms_GetUserInstancebyUrl(HUINT8 *pcUrl, HUINT32 *pnUserInstanceId)
{
	HERROR		nRet = ERR_OK;
	HUINT32		i=0;

	for(i = 0;i<PL_DMA_MAX_USERIO_INST;i++)
	{
		if(s_tUserioInstance[i].url != NULL)
		{
			if((HxSTD_StrCmp(pcUrl, s_tUserioInstance[i].url) == 0) && (s_tUserioInstance[i].valid == TRUE))
			{
				*pnUserInstanceId = i;
				break;
			}
		}
	}

	if(i == PL_DMA_MAX_USERIO_INST)
	{
		HxLOG_Print("[%s] Not Found User Instance ID.\n", __func__);
		nRet = ERR_FAIL;
	}

	return nRet;
}

/**
 * @brief		User Instance Id가 유효한지 검사한다.
 *
 * @param[in]	unUserioID 	User Instance ID
 * @return		HERROR
 * @see
 */
STATIC HERROR __pldms_CheckValidUserInstancebyId(HUINT32 unUserioID)
{
	HERROR		nRet = ERR_OK;

	if(unUserioID >= PL_DMA_MAX_USERIO_INST)
	{
		HxLOG_Print("[%s] Invalid User Instance ID.\n", __func__);
		nRet = ERR_FAIL;
	}
	else
	{
		if(s_tUserioInstance[unUserioID].valid != TRUE)
		{
			HxLOG_Print("[%s] Invalid User Instance ID.\n", __func__);
			nRet = ERR_FAIL;
		}
		else
		{
			nRet = ERR_OK;
		}
	}

	return nRet;
}

HUINT32 __pldms_ConvertFcntlFlags(HUINT32 flags)
{
	switch(flags)
	{
		case O_APPEND: 		return PL_FS_MODE_APPEND;
#if 0
		case O_LARGEFILE: 	return PL_FS_MODE_O_LARGEFILE;
		case O_DIRECT: 		return PL_FS_MODE_O_DIRECT;
#endif
		case O_RDONLY:		return PL_FS_MODE_READ;
		case O_RDWR: 		return PL_FS_MODE_RDWR;
		case O_TRUNC: 		return PL_FS_MODE_TRUNC;
		case O_WRONLY: 		return PL_FS_MODE_WRITE;

		default: 			return 0;
	}
}

/* currently (.hjm) only supported */
static PL_DMS_RECORD_IO_TYPE_e __pldms_CheckRecordType(HCHAR *pRecordPath)
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

	if( pRecordPath == NULL)
	{
		HxLOG_Print("[ PL_DMS ] Error> Invalid NULL arg!\n" );
		goto func_done;
	}

	if (HxSTD_StrStr(pRecordPath, ".hjm") != NULL)
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
			HxSTD_StrNCat(szPairPath, ".hjm", 4);

			HxSTD_MemSet(&filestat, 0x00, sizeof(PL_FS_FileStat_t));
			if (ERR_OK == PL_FS_GetFileInfo( szPairPath, PL_FS_INFO_NAMETYPE, &filestat))
			{
				fileType = filestat.fileType;

				if( fileType == PL_FS_FILE_TYPE_FILE )
				{
					bRecordJMetaExisted = TRUE;
				}
				else if ( fileType == PL_FS_FILE_TYPE_DIR )
				{
					bRecordJMetaExisted = FALSE;
				}
			}

			if (bRecordJMetaExisted == TRUE)
			{
				/* Check whether this .ts has single or delayed recoding property */
				if (ERR_OK == PL_FS_GetFileInfo( pRecordPath, PL_FS_INFO_NAMETYPE, &filestat))
				{
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
	}
	else if (bArgPathJMeta == TRUE) /* Check .ts file existing if .hjm path */
	{
		/* Check .ts path whether this is single or delayed record */
		bRecordJMetaExisted = TRUE;
		pPathSrc = pRecordPath;
		pPathTemp = HxSTD_StrStr(pRecordPath, ".hjm");
		if (pPathTemp != NULL)
		{
			nPathLen = pPathTemp - pPathSrc;
			HxSTD_MemSet(szPairPath, 0x00, sizeof(szPairPath) );
			HxSTD_StrNCat(szPairPath, pPathSrc, nPathLen);
			HxSTD_StrNCat(szPairPath, PL_DMS_TSFILE_EXT, 3);

			if (ERR_OK == PL_FS_GetFileInfo( szPairPath, PL_FS_INFO_NAMETYPE, &filestat))
			{
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

	return eRecordIoType;

func_done:
	return PL_MEDIA_RECORD_IO_TYPE_NA;
}

/* 별도로 처리해야 할 Humax 파일인지 판단( DTCP / Live Streaming ) */
STATIC HLONG __pldms_CB_IsUserFile( const HCHAR *pPathName, HLONG *pIsUserFile )
{
	HLONG	lRet = 0;
	HBOOL	bIsHumaxOperation = FALSE;
	HBOOL	bSupportRecordDMS = FALSE;
	HBOOL	IsChunkedRecord = FALSE;
	PL_DMS_RECORD_IO_TYPE_e eRecordIoType = PL_MEDIA_RECORD_IO_TYPE_NA;
	DxRecListData_t *pstRecData = NULL;

	if( pPathName == NULL || pIsUserFile == NULL )
	{
		HxLOG_Print( "[PL_DMS] Invalid Parameter : pPathName or pIsUserFile\n" );
		return -1;
	}

	*pIsUserFile = FALSE;

#if 0
	/*  for LiveStreaming  */
	if( HxSTD_StrNCmp( pPathName, PL_DMS_LIVESTREAM_PREFIX_CHANNEL, HxSTD_StrLen( PL_DMS_LIVESTREAM_PREFIX_CHANNEL ) ) == 0 )
	{
		HxLOG_Print("[%s:%d] %s bIsHumaxOperation is True\n", __FUNCTION__, __LINE__, (HCHAR *)pPathName );
		bIsHumaxOperation = TRUE;
		goto func_done;
	}
	else
#endif
	{
		HUINT8	*pNewPathName = NULL;
		HCHAR	szPracticalPVRPath[1024] = { 0, };

#if defined(PL_DMS_THIRDPARTY_PVR_FEATURE)
		if (HxSTD_StrStr(pPathName, PL_DMS_DEFAULT_CONTENT_PREFIX) != NULL)
		{
			PL_DMS_GetPracticalPVRPath(pPathName, szPracticalPVRPath, sizeof(szPracticalPVRPath));
			pNewPathName = (HUINT8 *)szPracticalPVRPath;
		}
		else
		{
			pNewPathName = (HUINT8 *)pPathName;
		}
#else
		pNewPathName = (HUINT8 *)pPathName;
#endif
		/* DMS record sharing */
		bSupportRecordDMS = TRUE;

		/********************************************************************************/
		/* Live Streaming가 아닌 일반 파일에 대한 처리 */
		/********************************************************************************/
		/* Ts파일에 대해서 Encryption된것인지 물어보는 부분 */
		if( ( NULL == HxSTD_StrStr( pNewPathName, PL_DMS_PROTOCOL_PREFIX_HTTP ) ) &&
				( NULL == HxSTD_StrStr( pNewPathName, PL_DMS_PROTOCOL_PREFIX_HTTPS ) ) &&
				( NULL == HxSTD_StrStr( pNewPathName, PL_DMS_PROTOCOL_PREFIX_MMS ) ) &&
				( NULL == HxSTD_StrStr( pNewPathName, PL_DMS_PROTOCOL_PREFIX_RTSP ) ) &&
				( NULL == HxSTD_StrStr( pNewPathName, ".hmx" ) )&&
				( NULL != HxSTD_StrStr( pNewPathName, PL_DMS_TSFILE_EXT ) ) )
		{
			HUINT8	*pRequestFile = NULL;
			HUINT8	*pTetemp = NULL;
			HINT32	nSizeOfName = 0;

			nSizeOfName = HxSTD_StrLen( pNewPathName );
			pRequestFile = ( HUINT8 * )HLIB_STD_MemAlloc( nSizeOfName + 4 );
			if( pRequestFile != NULL )
			{

				HxSTD_MemSet( pRequestFile, 0, nSizeOfName + 4 );
				HxSTD_MemCopy( pRequestFile, ( void* )pNewPathName, nSizeOfName );
				pTetemp = HxSTD_StrRChar( pRequestFile, '.' );
				if( pTetemp != NULL )
				{
					*( pTetemp + 1 ) = 'h';
					*( pTetemp + 2 ) = 'j';
					*( pTetemp + 3 ) = 'm';
					*( pTetemp + 4 ) = '\0';
				}

				/* MWC_FS_IsEncrypt() */
				pstRecData = PL_PVR_LoadHJM(( HCHAR * )pRequestFile);
				if ((pstRecData != NULL) && (pstRecData->bEncrypted == TRUE))
				{
					bIsHumaxOperation = TRUE;

					HLIB_STD_MemFree(pstRecData);
					pstRecData = NULL;
				}

				HxLOG_Print("[__pldms_CB_IsUserFile] pPathName=%s, pNewPathName=%s, pRequestFile=%s, IsHmx=%d[1]\n", pPathName, pNewPathName, pRequestFile, bIsHumaxOperation);
				HLIB_STD_MemFree( pRequestFile );
			}

			/* Check chunked record (.ts) if DMS record sharing is supported */
			if (bSupportRecordDMS == TRUE)
			{
#if __PL_DMS_SUPPORT_FS_CHUNK__
				IsChunkedRecord = PL_FS_CHUNK_IsChunkedFile (pNewPathName);
				if (IsChunkedRecord == TRUE)
				{
					bIsHumaxOperation = TRUE;
				}
#endif

				if (IsChunkedRecord != TRUE) /* Check again; Sometimes FS_CHUNK() gave wrong detection result */
				{
					eRecordIoType = __pldms_CheckRecordType(pNewPathName);
					if ((eRecordIoType == PL_MEDIA_RECORD_IO_TYPE_DELAYED) || (eRecordIoType == PL_MEDIA_RECORD_IO_TYPE_SINGLE))
					{
						bIsHumaxOperation = TRUE;
					}
				}
			}
		}
		else if( ( NULL == HxSTD_StrStr( pNewPathName, PL_DMS_PROTOCOL_PREFIX_HTTP ) ) &&
				( NULL == HxSTD_StrStr( pNewPathName, PL_DMS_PROTOCOL_PREFIX_HTTPS ) ) &&
				( NULL == HxSTD_StrStr( pNewPathName, PL_DMS_PROTOCOL_PREFIX_MMS ) ) &&
				( NULL == HxSTD_StrStr( pNewPathName, PL_DMS_PROTOCOL_PREFIX_RTSP ) ) &&
				( pNewPathName[ HxSTD_StrLen( pNewPathName ) - 4 ] == '.' ) &&
				( pNewPathName[ HxSTD_StrLen( pNewPathName ) - 3 ] == 'h' ) &&
				( pNewPathName[ HxSTD_StrLen( pNewPathName ) - 2 ] == 'm' ) &&
				( pNewPathName[ HxSTD_StrLen( pNewPathName ) - 1 ] == 'x' ) )
		{
			/* DRM 정보를 위한 hmx file */
			bIsHumaxOperation = TRUE;
			HxLOG_Print("[__pldms_CB_IsUserFile] pPathName=%s, pNewPathName=%s, IsHmx=%d[2]\n", pPathName, pNewPathName, bIsHumaxOperation);
		}
		else if ((bSupportRecordDMS == TRUE) && (NULL != HxSTD_StrStr(pNewPathName, ".hjm")) )
		{
			/* if path has .hjm under DMS record supporting */
			eRecordIoType = __pldms_CheckRecordType(pNewPathName);

			if ((eRecordIoType == PL_MEDIA_RECORD_IO_TYPE_DELAYED) ||
					(eRecordIoType == PL_MEDIA_RECORD_IO_TYPE_SINGLE))
			{
				bIsHumaxOperation = TRUE;
			}
		}
		else
		{
			bIsHumaxOperation = FALSE;
		}
	}

	*pIsUserFile = ( HLONG )bIsHumaxOperation;

	HxLOG_Print( "[PL_DMS] ###########################################################\n" );
	HxLOG_Print( "[PL_DMS] IsUserFile( %s ) - %d\n", pPathName, *pIsUserFile );
	HxLOG_Print( "[PL_DMS] ###########################################################\n" );

	return lRet;
}

/* isUserFile == 1 일경우의 Open */
STATIC HULONG __pldms_CB_UserOpen( const HCHAR *pPathName, HULONG flags, HULONG *filedes, const HCHAR *clientIP )
{
	HBOOL	bSupportRecordDMS = FALSE;
	PL_DMS_RECORD_IO_TYPE_e eRecordIoType = PL_MEDIA_RECORD_IO_TYPE_NA;
	HBOOL	bArgPathTS = FALSE; /* Avoiding too many strstr() */
	HBOOL	bArgPathJMeta = FALSE; /* Avoiding too many strstr() */
	HULONG	ulRet = -1;
	//HULONG	nRet = -1;  /* for MWC_PVR_ReadContentInfo() */
	HUINT32 unUserioID = 0;
	HUINT8	*pNewPathName = NULL;
	HCHAR	szPracticalPVRPath[1024] = { 0, };
	HCHAR 	szRecordPath[1024] = { 0, };
	HCHAR 	*pPathTemp = NULL;
	HCHAR 	*pPathSrc = NULL;
	HINT32 	nPathLen = 0;
	/* MW_PvrRecord_t stPvrRecInfo; */
	/* MW_PvrService_t stPvrSvcInfo; */
	/* MW_PvrEvent_t   stPvrEvtInfo; */
	DxRecListData_t *pstRecData = NULL;

#if defined(PL_DMS_THIRDPARTY_PVR_FEATURE)
	if (HxSTD_StrStr(pPathName, PL_DMS_DEFAULT_CONTENT_PREFIX) != NULL)
	{
		PL_DMS_GetPracticalPVRPath(pPathName, szPracticalPVRPath, sizeof(szPracticalPVRPath));
		pNewPathName = (HUINT8 *)szPracticalPVRPath;
	}
	else
	{
		pNewPathName = (HUINT8 *)pPathName;
	}
#else
	pNewPathName = (HUINT8 *)pPathName;
#endif

	/* DMS record sharing */
	bSupportRecordDMS = TRUE;

	HxLOG_Print( "[PL_DMS] ###########################################################\n" );
	HxLOG_Print( "[PL_DMS] UserOpen( %s , %s, %s )\n", pPathName,  pNewPathName, clientIP );
	HxLOG_Print( "[PL_DMS] ###########################################################\n" );

	if (bSupportRecordDMS == TRUE)
	{
		if (HxSTD_StrStr(pNewPathName, ".hjm") != NULL) /* For avoiding too many strstr() */
		{
			bArgPathJMeta = TRUE;
			bArgPathTS = FALSE;
		}
		else if (HxSTD_StrStr(pNewPathName, ".ts") != NULL)
		{
			bArgPathTS = TRUE;
			bArgPathJMeta = FALSE;
		}
		/* Another case is unsupported */

		/* if path has .hjm or .ts, check PVR recording sharing(single or delayed) */
		if ((bArgPathJMeta == TRUE) || (bArgPathTS == TRUE))
		{
			eRecordIoType = __pldms_CheckRecordType(pNewPathName);
		}

		/* Change .hjm path to .ts for chunk_io if PVR record */
		if ((eRecordIoType != PL_MEDIA_RECORD_IO_TYPE_NA) && (bArgPathJMeta == TRUE))
		{
			/* file lock 상태가 바뀌는 경우 체크하여 streaming-out 금지 */
			/*
			 * There is only rescan(), not each file scan()
			 * So if Record file propertis(file-lock, user-lock) is changed under DLNA_DMS_RESCAN_TIME or PL_DMSDEFAULT_RESCAN_INTERVAL
			 * don't allow it's streaming-out to external renderer
			 */
			pstRecData = PL_PVR_LoadHJM(pNewPathName);
			if (pstRecData == NULL)
			{
				HxLOG_Print( "[PL_DMS] Error> Cant open record meta:%s\n", pNewPathName);
				return _DEF_DMS_FILE_ERROR;
			}

			if (1)
			{
				HxLOG_Print( "[PL_DMS] Info> record status :%d\n", pstRecData->eRecStatus);
				HxLOG_Print( "[PL_DMS] Info> endCondition  :%d\n", pstRecData->eRecEndReason);
				HxLOG_Print( "[PL_DMS] Info> CAS type      :%d\n", pstRecData->eCasType);
				HxLOG_Print( "[PL_DMS] Info> Svc type      :%d\n", pstRecData->eSvcType);
				HxLOG_Print( "[PL_DMS] Info> user lock     :%d\n", pstRecData->bUserLock);
				HxLOG_Print( "[PL_DMS] Info> DRM type      :%d\n", pstRecData->stMeta.stCcInfo.stCasCcInfo.eCasCcType);
			}

			if (pstRecData->bUserLock != 0)
			{
				HxLOG_Print( "[PL_DMS] Error> Unsupported user locked record(%d) \n", pstRecData->bUserLock);
				if (pstRecData != NULL)
				{
					HLIB_STD_MemFree(pstRecData);
					pstRecData = NULL;
				}
				return _DEF_DMS_FILE_ERROR;
			}

			/* Change the path with .ts for .hjm and check the record property(delayed or single) */
			pPathSrc = pNewPathName;
			pPathTemp = HxSTD_StrStr(pNewPathName, ".hjm");
			if (pPathTemp != NULL)
			{
				nPathLen = pPathTemp - pPathSrc;
				HxSTD_MemSet(szRecordPath, 0x00, sizeof(szRecordPath) );
				HxSTD_StrNCat(szRecordPath, pPathSrc, nPathLen);
				HxSTD_StrNCat(szRecordPath, PL_DMS_TSFILE_EXT, 3);
				pNewPathName = &szRecordPath[0];
			}
		}
	}

	if((NULL == HxSTD_StrStr((char *)pNewPathName, "http://")) &&
	   	(NULL == HxSTD_StrStr((char *)pNewPathName, "https://")) &&
		(NULL == HxSTD_StrStr((char *)pNewPathName, "mms://")) &&
	   	(NULL == HxSTD_StrStr((char *)pNewPathName, "rtsp://")) &&
		(pNewPathName[HxSTD_StrLen(pNewPathName)-4] == '.') &&
		(pNewPathName[HxSTD_StrLen(pNewPathName)-3] == 'h') &&
		(pNewPathName[HxSTD_StrLen(pNewPathName)-2] == 'm') &&
	   	(pNewPathName[HxSTD_StrLen(pNewPathName)-1] == 'x'))
	{
		if(__pldms_GetUserInstancebyUrl((HUINT8 *)pNewPathName, &unUserioID) == ERR_OK)
		{
			HxLOG_Print("[%s] Increase User Instance Id Number\n", __FUNCTION__);
			s_tUserioInstance[unUserioID].number = s_tUserioInstance[unUserioID].number+1;
			*filedes = unUserioID | FILEHANDLE_HEAD_DTCP;
			ulRet = ERR_OK;
		}
		else
		{
			ulRet = __pldms_GetUserInstance(&unUserioID);
			if(ulRet != _DEF_DMS_FILE_OK )
			{
				HxLOG_Print("[%s] Error> Fail local_MEDIA_GetUserOperationInstance\n", __FUNCTION__);
				return _DEF_DMS_FILE_ERROR;
			}
			else
			{
				/* Instance Type을 구별한다. */
				s_tUserioInstance[unUserioID].instancetype = MW_MEDIA_USERINSTANCE_TYPE_DRM;

				s_tUserioInstance[unUserioID].url = HLIB_STD_MemAlloc(HxSTD_StrLen(pNewPathName)+1);
				HxSTD_MemSet(s_tUserioInstance[unUserioID].url, 0x0, HxSTD_StrLen(pNewPathName)+1);
				HxSTD_MemCopy(s_tUserioInstance[unUserioID].url, (char *)pNewPathName, HxSTD_StrLen(pNewPathName));
				s_tUserioInstance[unUserioID].number = s_tUserioInstance[unUserioID].number+1;
				*filedes = unUserioID | FILEHANDLE_HEAD_DTCP;
			}
		}
		HxLOG_Print("[__pldms_CB_UserOpen] *filedes = unUserioID | FILEHANDLE_HEAD_DTCP, %s, %d, 0x%08x\n", pNewPathName, unUserioID, (HUINT32)*filedes);
	}
#if defined(__PL_DMS_SUPPORT_EMULATE_CHUNKED_FILES__)
	else if (PL_FS_CHUNK_IsChunkedFile (pNewPathName) == TRUE)
	{
		HOM_Handle_t			 hChunk;
#if __PL_DMS_SUPPORT_FS_SECURE__
		HBOOL bSecure = TRUE;
#else
		HBOOL bSecure = FALSE;
#endif

		hChunk = PL_FS_CHUNK_Open (pNewPathName, "", bSecure);
		if (HANDLE_NULL == hChunk)
		{
			HxLOG_Error ("Error> CHUNK_Open failed(sec:%d)\n", bSecure);
			return _DEF_DMS_FILE_ERROR;
		}

		*filedes = (HULONG)(hChunk | FILEHANDLE_HEAD_CHUNKEDFILE);
		ulRet = ERR_OK;

#ifdef	__DMS_DEBUG_DUMP_CHUNK__
		if (1)
		{
			if (s_pDebugChunkDumpPath != NULL)
			{
				HxLOG_Error("[__pldms_CB_UserOpen] Error> non-NULL dump path!\n");
				HLIB_STD_MemFree(s_pDebugChunkDumpPath);
				s_pDebugChunkDumpPath = NULL;
			}

			if (s_fpDebugChunk != NULL)
			{
				HxLOG_Error("[__pldms_CB_UserOpen] Error> non-NULL dump fp!\n");
				fclose(s_fpDebugChunk);
				s_fpDebugChunk = NULL;
			}
		}

		s_pDebugChunkDumpPath = HLIB_STD_MemAlloc(512);
		if (s_pDebugChunkDumpPath != NULL)
		{
			HxSTD_MemSet(s_pDebugChunkDumpPath, 0x0, 512);
			HxSTD_sprintf(s_pDebugChunkDumpPath, "/mnt/hd2/Recordings/DMSChunkDump%d.ts", s_nDebugChunkDumpCount);
			s_nDebugChunkDumpCount++;

			s_fpDebugChunk = fopen(s_pDebugChunkDumpPath, "wb");
		}
#endif
	}
	else if ((bSupportRecordDMS == TRUE) && (eRecordIoType == PL_MEDIA_RECORD_IO_TYPE_DELAYED))
	{
		HOM_Handle_t			 hChunk;
#if __PL_DMS_SUPPORT_FS_SECURE__
		HBOOL bSecure = TRUE;
#else
		HBOOL bSecure = FALSE;
#endif

		/* Confirm the path has .ts extension, not .hjm */
		hChunk = PL_FS_CHUNK_Open (pNewPathName, "", bSecure);
		if (HANDLE_NULL == hChunk)
		{
			HxLOG_Error ("Error> CHUNK_Open failed(sec:%d)\n", bSecure);
			return _DEF_DMS_FILE_ERROR;
		}

		*filedes = (HULONG)(hChunk | FILEHANDLE_HEAD_CHUNKEDFILE);
		ulRet = ERR_OK;

#ifdef	__DMS_DEBUG_DUMP_CHUNK__
		if (1)
		{
			if (s_pDebugChunkDumpPath != NULL)
			{
				HxLOG_Error("[__pldms_CB_UserOpen] Error> non-NULL dump path!\n");
				HLIB_STD_MemFree(s_pDebugChunkDumpPath);
				s_pDebugChunkDumpPath = NULL;
			}

			if (s_fpDebugChunk != NULL)
			{
				HxLOG_Error("[__pldms_CB_UserOpen] Error> non-NULL dump fp!\n");
				fclose(s_fpDebugChunk);
				s_fpDebugChunk = NULL;
			}
		}

		s_pDebugChunkDumpPath = HLIB_STD_MemAlloc(512);
		if(s_pDebugChunkDumpPath != NULL)
		{
			HxSTD_MemSet(s_pDebugChunkDumpPath, 0x0, 512);
			HxSTD_sprintf(s_pDebugChunkDumpPath, "/mnt/hd2/Recordings/DMSChunkDump%d.ts", s_nDebugChunkDumpCount);
			s_nDebugChunkDumpCount++;

			s_fpDebugChunk = fopen(s_pDebugChunkDumpPath, "wb");
		}
#endif
	}
#endif
	else
	{
#if __PL_DMS_SUPPORT_FS_SECURE__
		HBOOL bSecure = TRUE;
#else
		HBOOL bSecure = FALSE;
#endif
		HULONG flags2 = __pldms_ConvertFcntlFlags(flags);
		if(flags2 <= 0)
		{
			HxLOG_Print("[%s] Error> Fail PL_FS_(Secure)_Open (sec:%d) %s %x %x \n", __FUNCTION__, bSecure, pNewPathName, flags, flags2);
			return _DEF_DMS_FILE_ERROR;
		}

#if __PL_DMS_SUPPORT_FS_SECURE__
		if (bSecure)
		{
			ulRet = PL_FS_Secure_Open(pNewPathName, flags2, filedes);
		}
		else
#endif
		{
			ulRet = PL_FS_Open(pNewPathName, flags2, filedes);
		}

		if(ulRet != ERR_OK)
		{
			HxLOG_Print("[%s] Error> Fail PL_FS_(Secure)_Open sec:%d, %s, %x\n", __FUNCTION__, bSecure, pNewPathName, flags2);
			return _DEF_DMS_FILE_ERROR;
		}
		s_usUseTransfer ++;
		HxLOG_Print("[__pldms_CB_UserOpen] PL_FS_(Secure)_Open() sec:%d, path=%s, new=%s, 0x%08x, UseTrans=%d\n", bSecure, pPathName, pNewPathName, (HUINT32)*filedes, s_usUseTransfer);

#ifdef	TRANSFER_DATA_DUMP
		s_pDumpPath = HLIB_STD_MemAlloc(512);
		if(s_pDumpPath != NULL)
		{
			HxSTD_MemSet(s_pDumpPath, 0x0, 512);
			HLIB_STD_sprintf(s_pDumpPath, "/media/drive1/Video/DlnaFileDump%d.ts", s_nFileDumpCount);
			s_nFileDumpCount++;

			fp = fopen(s_pDumpPath, "wb");
		}
#endif
	}

	HxLOG_Print( "[PL_DMS] UserOpen Ret( %d )\n", ulRet );

	return ulRet;
}

/* isUserFile == 1 일경우의 Close */
STATIC void __pldms_CB_UserClose( HULONG filedes )
{
	HUINT32	ulRet = 0;
	HUINT32 unUserioID = 0;

	HxLOG_Print( "[PL_DMS] ###########################################################\n" );
	HxLOG_Print( "[PL_DMS] ( 0x%08x )UserClose\n", filedes );
	HxLOG_Print( "[PL_DMS] ###########################################################\n" );

	if((filedes & FILEHANDLE_HEAD_MASK) == FILEHANDLE_HEAD_DTCP)
	{
		unUserioID = filedes & FILEHANDLE_TAIL_MASK;
		ulRet = __pldms_CheckValidUserInstancebyId(unUserioID);
		if(ulRet != _DEF_DMS_FILE_OK )
		{
			HxLOG_Print("[%s] Error> Fail local_MEDIA_CheckValidUserInstancebyId\n", __FUNCTION__);
			return;
		}
		else
		{
			if(s_tUserioInstance[unUserioID].number > 0)
			{
				s_tUserioInstance[unUserioID].number = s_tUserioInstance[unUserioID].number-1;
				if(s_tUserioInstance[unUserioID].number == 0)
				{
					HxLOG_Print("[%s] Remove User Instance Id\n", __FUNCTION__);
					if(s_tUserioInstance[unUserioID].url != NULL)
					{
						HLIB_STD_MemFree(s_tUserioInstance[unUserioID].url);
						s_tUserioInstance[unUserioID].url = NULL;
					}

					s_tUserioInstance[unUserioID].valid = FALSE;
				}
				else
				{
					HxLOG_Print("[%s] Decrease User Instance Id Number\n", __FUNCTION__);
				}
			}
			else
			{
				HxLOG_Print("[%s] Invalid User Instance Id\n", __FUNCTION__);
				return;
			}
		}
	}
#if defined(__PL_DMS_SUPPORT_EMULATE_CHUNKED_FILES__)
	else if ((filedes & FILEHANDLE_HEAD_MASK) == FILEHANDLE_HEAD_CHUNKEDFILE)
	{
		HOM_Handle_t		 hChunk = (filedes & FILEHANDLE_TAIL_MASK);

		PL_FS_CHUNK_Close (hChunk);
	}
#endif
	else
	{
#if __PL_DMS_SUPPORT_FS_SECURE__
		ulRet = PL_FS_Secure_Close(filedes);
#else
		ulRet = PL_FS_Close(filedes);
#endif
		if(ulRet != ERR_OK)
		{
			HxLOG_Print("[%s] Error> Fail PL_FS_(Secure)_Close\n", __FUNCTION__);
			return;
		}
		if(s_usUseTransfer > 0)
			s_usUseTransfer --;

	}

#ifdef	TRANSFER_DATA_DUMP
	if(s_pDumpPath != NULL)
	{
		HLIB_STD_MemFree(s_pDumpPath);
		s_pDumpPath = NULL;
		fclose(fp);
	}
#endif
#ifdef	__DMS_DEBUG_DUMP_CHUNK__
	if (s_pDebugChunkDumpPath != NULL)
	{
		HLIB_STD_MemFree(s_pDebugChunkDumpPath);
		s_pDebugChunkDumpPath = NULL;
	}

	if (s_fpDebugChunk != NULL)
	{
		fclose(s_fpDebugChunk);
		s_fpDebugChunk = NULL;
	}
#endif

	return;
}

/* isUserFile == 1 일경우의 Read */
STATIC HLONG __pldms_CB_UserRead( HULONG filedes, HUINT8 *buf, HULONG size, HULONG *sizep )
{
	HLONG	lRet = 0;
	//HULONG	ulReadSize = 0;
	HUINT32 unUserioID = 0;
	HUINT32 nIsExpired = FALSE;
	HUINT8 *pcPath = NULL;
	HUINT8 *pcTempPath = NULL;
	DxRecListData_t *pstRecData = NULL;

	HxLOG_Print( "[PL_DMS] ( 0x%08x )UserRead - req_size( %ld )\n", filedes, size );

	if ((filedes & FILEHANDLE_HEAD_MASK) == FILEHANDLE_HEAD_DTCP)
	{
		unUserioID = filedes & FILEHANDLE_TAIL_MASK;
		HxLOG_Print("[__pldms_CB_UserRead] if ((filedes & FILEHANDLE_HEAD_MASK) == FILEHANDLE_HEAD_DTCP) %d 0x%08x\n", unUserioID, (HUINT32)filedes);
		lRet = __pldms_CheckValidUserInstancebyId(unUserioID);
		if(lRet != _DEF_DMS_FILE_OK )
		{
			HxLOG_Print("[%s] Error> Fail local_MEDIA_CheckValidUserInstancebyId\n", __FUNCTION__);
			return _DEF_DMS_FILE_ERROR;
		}

		if(s_tUserioInstance[unUserioID].instancetype == MW_MEDIA_USERINSTANCE_TYPE_DRM)
		{
			pcPath = HLIB_STD_MemAlloc(HxSTD_StrLen(s_tUserioInstance[unUserioID].url)+1);
			if(pcPath == NULL)
			{
				if(pcPath)
				{
					HLIB_STD_MemFree(pcPath);
					pcPath = NULL;
				}

				HxLOG_Print("[%s] Error> Fail Mem Alloc\n", __FUNCTION__);
				return _DEF_DMS_FILE_ERROR;
			}

			HxSTD_MemSet(pcPath, 0x0, HxSTD_StrLen(s_tUserioInstance[unUserioID].url)+1);
			HxSTD_snprintf(pcPath, HxSTD_StrLen(s_tUserioInstance[unUserioID].url)+1, "%s", s_tUserioInstance[unUserioID].url);
			if(HxSTD_StrStr((char *)pcPath, ".hmx") == NULL)
			{
				HxLOG_Print("[%s] Not Found .hmx\n", __FUNCTION__);

				if(pcPath)
				{
					HLIB_STD_MemFree(pcPath);
					pcPath = NULL;
				}

				return _DEF_DMS_FILE_ERROR;
			}

			pcTempPath = HxSTD_StrStr((char *)pcPath, PL_DMS_TSFILE_EXT);
			if(pcTempPath != NULL)
			{
				pcTempPath[1] = 'h';
				pcTempPath[2] = 'j';
				pcTempPath[3] = 'm';
				pcTempPath[4] = '\0';

				/*
				 * - svc_pvrmeta_IsExpired() 구현과 동기를 맞추거나 : eDxCopyrightControl_CasCcType_CiPlus 모델만 정의되어 있기 때문에,
				 * - apk_query()로 대체해야 함: apk_query() 이용시 blocking 시간 고려
				 */
				HxLOG_Print("### GetDrmInfo call Path=%s ###\n", pcPath);

				/* MWC_FS_GetDrmInfo() */
				pstRecData = PL_PVR_LoadHJM(( HCHAR * )pcPath);
				if ((pstRecData != NULL) && (pstRecData->stMeta.stCcInfo.stCasCcInfo.eCasCcType == eDxCopyrightControl_CasCcType_CiPlus))
				{
					// expireDuration = pstDrmInfo->info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ulMinutesForRL * 60;
					nIsExpired = FALSE;

					HLIB_STD_MemFree(pstRecData);
					pstRecData = NULL;
				}
				lRet = ERR_OK;
			}
			else
			{
				/* MWC_FS_GetDrmInfo() */
				pstRecData = PL_PVR_LoadHJM(( HCHAR * )pcPath);
				if ((pstRecData != NULL) && (pstRecData->stMeta.stCcInfo.stCasCcInfo.eCasCcType == eDxCopyrightControl_CasCcType_CiPlus))
				{
					// expireDuration = pstDrmInfo->info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ulMinutesForRL * 60;
					nIsExpired = FALSE;

					HLIB_STD_MemFree(pstRecData);
					pstRecData = NULL;
				}
				lRet = ERR_OK;
			}

			if(lRet != ERR_OK)
			{
				HxLOG_Print("[%s] Error> Fail GetDrmInfo\n", __FUNCTION__);

				if(pcPath)
				{
					HLIB_STD_MemFree(pcPath);
					pcPath = NULL;
				}

				return _DEF_DMS_FILE_ERROR;
			}

			buf[2] = ';';
			buf[3] = nIsExpired;

			if(pcPath)
			{
				HLIB_STD_MemFree(pcPath);
				pcPath = NULL;
			}

			*sizep = PL_DMS_MEDIA_FILE_DRMINFO_MAX_LENGTH;
			HxLOG_Print("[__pldms_CB_UserRead] PL_DMS_MEDIA_FILE_DRMINFO_MAX_LENGTH 0x%08x %ld\n", (HUINT32)filedes, *sizep);
		}

	}
#if defined(__PL_DMS_SUPPORT_EMULATE_CHUNKED_FILES__)
	else if ((filedes & FILEHANDLE_HEAD_MASK) == FILEHANDLE_HEAD_CHUNKEDFILE)
	{
		HOM_Handle_t		 hChunk = (filedes & FILEHANDLE_TAIL_MASK);

		lRet = PL_FS_CHUNK_Read (hChunk, buf, size, sizep);
		if(lRet != ERR_OK)
		{
			HxLOG_Error ("Fail PL_FS_CHUNK_Read\n");
			return _DEF_DMS_FILE_ERROR;
		}

#ifdef	__DMS_DEBUG_DUMP_CHUNK__
		if (*sizep > 0)
		{
			fwrite(buf, 1, *sizep, s_fpDebugChunk);
		}
#endif

		HxLOG_Print ("[%s:%d] PL_FS_CHUNK_Read() 0x%08x %ld\n", __FUNCTION__, __LINE__, (HUINT32)filedes, *sizep);
	}
#endif
	else
	{
#if __PL_DMS_SUPPORT_FS_SECURE__
		lRet = PL_FS_Secure_Read(filedes, buf, size, sizep); /* (key, keylen) */
#else
		lRet = PL_FS_Read(filedes, buf, size, sizep);
#endif
		if(lRet != ERR_OK)
		{
			HxLOG_Print("[%s] Error> Fail PL_FS_(Secure)_Read\n", __FUNCTION__);
			return _DEF_DMS_FILE_ERROR;
		}
		HxLOG_Print("[__pldms_CB_UserRead] PL_FS_(Secure)_Read 0x%08x %ld\n", (HUINT32)filedes, *sizep);
#ifdef	TRANSFER_DATA_DUMP
		fwrite(buf, 1, *sizep, fp);
#endif
	}

	HxLOG_Print( "[PL_DMS] \t######## ( 0x%08x )UserRead - %x %ld\n", (HUINT32)filedes, *sizep, *sizep );

	return *sizep;
}



/* isUserFile == 1 일경우의 Seek */
STATIC HINT64 __pldms_CB_UserSeek( HULONG filedes, HINT64 offset, HULONG whence )
{
	HUINT32	ulRet = 0;
	HINT64	curOffset = 0;
	HUINT32 unUserioID = 0;

	HxLOG_Print( "[PL_DMS] ( 0x%08x )UserSeek - offset( %ld ) - whence( %ld )\n", filedes, offset, whence );

	if ((filedes & FILEHANDLE_HEAD_MASK) == FILEHANDLE_HEAD_DTCP)
	{
		unUserioID = filedes & FILEHANDLE_TAIL_MASK;
		ulRet = __pldms_CheckValidUserInstancebyId(unUserioID);
		if(ulRet != _DEF_DMS_FILE_OK )
		{
			HxLOG_Print("[%s] Error> Fail local_MEDIA_CheckValidUserInstancebyId\n", __FUNCTION__);
			return _DEF_DMS_FILE_ERROR;
		}
		else
		{
			/* Not Support Seek */
			HxLOG_Print("[%s] Not Support Seek\n", __FUNCTION__);
		}
		HxLOG_Print("[__pldms_CB_UserSeek] if ((filedes & FILEHANDLE_HEAD_MASK) == FILEHANDLE_HEAD_DTCP) 0x%08x %lld\n", (HUINT32)filedes, curOffset);
	}

#if defined(__PL_DMS_SUPPORT_EMULATE_CHUNKED_FILES__)
	else if ((filedes & FILEHANDLE_HEAD_MASK) == FILEHANDLE_HEAD_CHUNKEDFILE)
	{
		HOM_Handle_t		 hChunk = (filedes & FILEHANDLE_TAIL_MASK);

		ulRet = PL_FS_CHUNK_Seek (hChunk, offset, whence, &curOffset);
		if (ERR_OK != ulRet)
		{
			HxLOG_Error ("PL_FS_CHUNK_Seek Err:\n");
			return _DEF_DMS_FILE_ERROR;
		}
	}
#endif
	else
	{
#if __PL_DMS_SUPPORT_FS_SECURE__
		curOffset = PL_FS_Secure_Seek(filedes, offset, whence);
#else
		curOffset = PL_FS_Seek(filedes, offset, whence);
#endif
		HxLOG_Print("[__pldms_CB_UserSeek] PL_FS_(Secure)_Seek: %x %lld\n", (HUINT32)filedes, curOffset);
	}

	return curOffset;
}



/* isUserFile == 1 일경우의 Tell */
STATIC HLONG __pldms_CB_UserTell( HULONG filedes, HUINT64 *poffset )
{
	HLONG	lRet = 0;
	HUINT32 unUserioID = 0;

	HxLOG_Print( "[PL_DMS] ( 0x%08x  )UserTell\n", filedes );

	if ((filedes & FILEHANDLE_HEAD_MASK) == FILEHANDLE_HEAD_DTCP)
	{
		unUserioID = filedes & FILEHANDLE_TAIL_MASK;
		lRet = __pldms_CheckValidUserInstancebyId(unUserioID);
		if(lRet != _DEF_DMS_FILE_OK )
		{
			HxLOG_Print("[%s] Error> Fail local_MEDIA_CheckValidUserInstancebyId\n", __FUNCTION__);
			return _DEF_DMS_FILE_ERROR;
		}
		else
		{
			/* Not Support Tell */
			HxLOG_Print("[%s] Not Support Tell\n", __FUNCTION__);
		}
		HxLOG_Print("[__pldms_CB_UserTell] if ((filedes & FILEHANDLE_HEAD_MASK) == FILEHANDLE_HEAD_DTCP) 0x%08x %lld\n", (HUINT32)filedes, *poffset);
	}
#if defined(__PL_DMS_SUPPORT_EMULATE_CHUNKED_FILES__)
	else if ((filedes & FILEHANDLE_HEAD_MASK) == FILEHANDLE_HEAD_CHUNKEDFILE)
	{
		HOM_Handle_t		 hChunk = (filedes & FILEHANDLE_TAIL_MASK);
		HERROR			 hErr;

		hErr = PL_FS_CHUNK_Tell (hChunk, poffset);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("PL_FS_CHUNK_Tell Err:\n");
			return _DEF_DMS_FILE_ERROR;
		}

		lRet = ERR_OK;
	}
#endif
	else
	{
#if __PL_DMS_SUPPORT_FS_SECURE__
		lRet = PL_FS_Secure_Tell(filedes, poffset);
#else
		lRet = PL_FS_Tell(filedes, poffset);
#endif
		if(lRet != ERR_OK)
		{
			HxLOG_Print("[%s] Error> Fail PL_FS_(Secure)_Tell\n", __FUNCTION__);
			return _DEF_DMS_FILE_ERROR;
		}
		HxLOG_Print("[__pldms_CB_UserTell] PL_FS_(Secure)_Tell 0x%08x %lld\n", (HUINT32)filedes, *poffset);
	}

	return lRet;
}



/* isUserFile == 1 일경우의 GetFileSize */
STATIC HLONG __pldms_CB_UserGetFileSize( HULONG filedes, HINT64 *pulSize )
{
	HUINT32	ulRet = _DEF_DMS_FILE_OK;
	//HINT64	llFileSize = 0;
	HUINT32 unUserioID = 0;

	HxLOG_Print( "[PL_DMS] ( 0x%08x )UserGetFileSize\n", (HUINT32)filedes );

	if ((filedes & FILEHANDLE_HEAD_MASK) == FILEHANDLE_HEAD_DTCP)
	{
		unUserioID = filedes & FILEHANDLE_TAIL_MASK;
		ulRet = __pldms_CheckValidUserInstancebyId(unUserioID);
		if(ulRet != _DEF_DMS_FILE_OK )
		{
			HxLOG_Print("[%s] Error> Fail local_MEDIA_CheckValidUserInstancebyId\n", __FUNCTION__);
			return _DEF_DMS_FILE_ERROR;
		}
		else
		{
			if(s_tUserioInstance[unUserioID].instancetype == MW_MEDIA_USERINSTANCE_TYPE_DRM)
			{
				*pulSize = PL_DMS_MEDIA_FILE_DRMINFO_MAX_LENGTH;
			}
		}
		HxLOG_Print( "[PL_DMS] if ((filedes & FILEHANDLE_HEAD_MASK) == FILEHANDLE_HEAD_DTCP) 0x%08x GetStreamSize( %lld )\n", (HUINT32)filedes, *pulSize );
	}

#if defined(__PL_DMS_SUPPORT_EMULATE_CHUNKED_FILES__)
	else if ((filedes & FILEHANDLE_HEAD_MASK) == FILEHANDLE_HEAD_CHUNKEDFILE)
	{
		HOM_Handle_t		 hChunk = (filedes & FILEHANDLE_TAIL_MASK);
		HERROR			 hErr;

		hErr = PL_FS_CHUNK_GetFileSize (hChunk, pulSize);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("PL_FS_CHUNK_Tell Err:\n");
			return _DEF_DMS_FILE_ERROR;
		}
	}
#endif
	else
	{
#if __PL_DMS_SUPPORT_FS_SECURE__
		ulRet = PL_FS_Secure_GetFileSize(filedes, pulSize);
#else
		ulRet = PL_FS_GetFileSize(filedes, pulSize);
#endif
		if(ulRet != ERR_OK)
		{
			HxLOG_Print("[%s] Error> Fail PL_FS_(Secure)_GetFileSize\n", __FUNCTION__);
			return _DEF_DMS_FILE_ERROR;
		}
	}

	HxLOG_Print( "[PL_DMS] PL_FS_(Secure)_GetFileSize 0x%08x GetStreamSize( %lld )\n", (HUINT32)filedes, *pulSize );

	return ( HLONG )*pulSize;
}



/* isUserFile == 1 일경우의 IsEOF */
STATIC HLONG __pldms_CB_UserIsEOF( HULONG filedes )
{
	HLONG	lRet = 0;

	HxLOG_Print( "[PL_DMS] ( 0x%08x )UserIsEOF\n", (HUINT32)filedes );

	if (0)				// Config 처리를 위한 문구에 지나지 않음.
	{
		;
	}

#if defined(__PL_DMS_SUPPORT_EMULATE_CHUNKED_FILES__)
	else if ((filedes & FILEHANDLE_HEAD_MASK) == FILEHANDLE_HEAD_CHUNKEDFILE)
	{
		HOM_Handle_t		 hChunk = (filedes & FILEHANDLE_TAIL_MASK);

		lRet = (HLONG)PL_FS_CHUNK_IsEof (hChunk);
		HxLOG_Print( "[PL_DMS] ( 0x%08x )PL_FS_CHUNK_IsEof %ld\n", (HUINT32)filedes, lRet );
	}
#endif
	else
	{
#if __PL_DMS_SUPPORT_FS_SECURE__
		lRet = PL_FS_Secure_IsEOF(filedes);
#else
		lRet = PL_FS_IsEOF(filedes);
#endif
		HxLOG_Print( "[PL_DMS] ( 0x%08x )UserIsEOF %ld\n", (HUINT32)filedes, lRet );
	}

	return lRet;
}


#if 0
/*  Not Used  */
/* Http:Get 요청이 File Tranport가 아닌 Command 일경우에 처리하는 콜백 */
STATIC HCHAR* __pldms_CB_UserProcCMD( HCHAR *pUrlNParam )
{
	HxLOG_Print( "[PL_DMS] __pldms_CB_UserProcCMD( %s )\n", pUrlNParam );
	return NULL;
}
#endif

#define ________________Public_Interfaces__________________________
HERROR	PL_DMS_FilsOperation_SetUserCallbacks(void)
{
	mxFileUser_SetApi(  __pldms_CB_IsUserFile,
						__pldms_CB_UserOpen,
						__pldms_CB_UserClose,
						__pldms_CB_UserRead,
						__pldms_CB_UserSeek,
						__pldms_CB_UserTell,
						__pldms_CB_UserGetFileSize,
						__pldms_CB_UserIsEOF);

	return ERR_OK;
}
