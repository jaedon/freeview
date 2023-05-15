/**************************************************************
*	@file	dms.c
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
#include <vkernel.h>
#include <hlib.h>
#include <sapi.h>
#include <sys/time.h>

#include "../include/dms.h"
#include "pl_dms.h"
#include "pl_common.h"
#include "pl_fs.h"
#include "woons.h"
#include "homma_system.h"
#include "../../include/common.h"
#if defined(CONFIG_HOMMA_DMR)
#include "pl_dmr.h"
#endif

#define ________________Defines____________________________________
#define _DEF_DMS_INT_HDD_MEDIA_DEFAULT_PATH "/mnt/hd2/Media/"

/* Detachable의 Recording file을 지원할 것인지 여부 */
#define SUPPORT_DMS_DETACHABLE_RECORDING_SCAN_ON	( 0 )
#define DLNA_DMS_DEFAULT_DETACHABLE_RECPATH			".recordings"

#define ________________Typedef____________________________________
typedef enum
{
	eDMS_STATUS_STOPPED=0,
	eDMS_STATUS_STOPPING,
	eDMS_STATUS_STARTING,
	eDMS_STATUS_STARTED,
} DMS_Status_e;

typedef struct
{
	HBOOL					bInited;
	DMS_Status_e			eStatus;
	HBOOL					bReqStart; /* To remember prev request under start() failure status and retry next time*/

	DMS_ProductInfo_t		stModel;
	DMS_Config_t 			stConfig;

	CO_STORAGE_Info_t 		stStorageInfo;
	CO_NET_Iface_t			stDefIface;
	CO_STORAGE_Mount_t		stDefMount;

	DMS_EVT_HANDLE_Func		cbDmsEvtNotify;

	/* Scanning Path*/
	HBOOL					bScanDetachableHDD;
	HBOOL					bScanUsbHDD;
	HBOOL					bScanUsbMemory;
	HBOOL					bIpAttachOnName; /* HMS scenario */
	HBOOL					bSupportRecordItem;

	/*  for Live Stream  */
	HUINT32					unSyncEventTaskId;
	HUINT32					unSyncEventMsgId;
	HUINT32					unSemId;

	/*  StandbyWait request for PAMA */
	HBOOL					bLooseStandbyWait;
	HUINT32					ulLastStreamingRecvTick;  /* To prevent IPC overhead by too many callback */
	HUINT32					ulLastStandbyWaitSendTick;
	HUINT32					ulLastStandbyWaitIgnoredTick;
} DMS_Context_t;

#define ________________Static_Variables___________________________
static	DMS_Context_t	s_stDmsContext = { 0, };

#define ________________Internal_Functions_________________________
static HBOOL _dms_IsDetachableHDDScanable(void);
static HBOOL _dms_IsUsbHDDScanable(void);
static HBOOL _dms_IsUsbMemoryScanable(void);

STATIC DMS_Context_t * __dms_getContext(void)
{
	return &s_stDmsContext;
}

/* pl dms 모듈에서 올라오는 이벤트 */
STATIC HINT32		__dms_event_callback(HINT32 evt, HINT32 p1, HINT32 p2, HUINT64 p3, void *p4)
{
//	DMS_Context_t *pDmsCtx = __dms_getContext();
//	HUINT32	uTickDiff = 0x0;

	switch(evt)
	{
		case PL_DMS_StatusEvent_Started:
		case PL_DMS_StatusEvent_Stopped:
			break;

		case PL_DMS_StatusEvent_Open:
		case PL_DMS_StatusEvent_Read:
		case PL_DMS_StatusEvent_Seek:
		case PL_DMS_StatusEvent_Tell:
		case PL_DMS_StatusEvent_Close:
			break;

		case PL_DMS_StatusEvent_StackStreamingSend:   /* mxlib's MXDLNADMS_CALLSTATUS_STREAMMING_SEND, correspond to woons's MXRA_EVENT_STREAMMING_SEND */
			break;

		case PL_DMS_StatusEvent_StackScanOneCycleEnd: /* MXDLNADMS_CALLSTATUS_SCAN_ETERNAL_ONECYCLE_END */
			break;

		default:
			break;
	}

// func_done:
	return ERR_OK;
}

static HERROR _dms_start(DMS_Config_t *pstConfig )
{
	DMS_Context_t				*pDmsCtx = __dms_getContext();
	PL_COMMON_ProductInfo_t		product_info;
	PL_DMS_Config_t				dmsConfig;
	CO_NET_Iface_t				*pIface = NULL;

	HxSTD_MemSet( &product_info, 0x00, sizeof(PL_COMMON_ProductInfo_t) );
	PL_COMMON_GetProductInfo(&product_info);

	if ( eDMS_STATUS_STARTED == pDmsCtx->eStatus )
	{
		HxLOG_Error( "[ DMS ] Error> Already Started DMS !!!!\n" );
		return ERR_FAIL;
	}

	pDmsCtx->eStatus 			= eDMS_STATUS_STARTING;
	pDmsCtx->bScanDetachableHDD = _dms_IsDetachableHDDScanable();
	pDmsCtx->bScanUsbHDD 		= _dms_IsUsbHDDScanable();
	pDmsCtx->bScanUsbMemory 	= _dms_IsUsbMemoryScanable();

	/*동일 모델 BOX를 구분하기 위해 IP주소를 FriendlyName에 추가한다.*/
	pDmsCtx->bIpAttachOnName 	= TRUE;

	/* DMS record sharing */
	pDmsCtx->bSupportRecordItem = TRUE;

	HxSTD_MemSet(&dmsConfig, 0x00, sizeof(PL_DMS_Config_t));

	pIface = &pDmsCtx->stDefIface;

	if(pstConfig != NULL)
	{
		HxSTD_MemCopy(&pDmsCtx->stConfig, pstConfig, sizeof(DMS_Config_t) );

		if (HxSTD_StrEmpty(pstConfig->szFriendlyName))
		{
			HxSTD_snprintf( pstConfig->szFriendlyName, sizeof(pstConfig->szFriendlyName)-1, "%s", product_info.szModelName);
		}

		if ( pDmsCtx->bIpAttachOnName )
		{
			HxSTD_snprintf( dmsConfig.szFriendlyName, sizeof(dmsConfig.szFriendlyName)-1, "%s DMS %s", pstConfig->szFriendlyName, pIface->szIpAddr );
		}
		else
		{
			HxSTD_snprintf( dmsConfig.szFriendlyName, sizeof(dmsConfig.szFriendlyName)-1, "%s DMS", pstConfig->szFriendlyName );
		}
	}

	/* Product Config */
	dmsConfig.bSupportRecordItem = pDmsCtx->bSupportRecordItem;

	if (PL_DMS_Init( &dmsConfig ) != ePL_DMS_Result_Ok)
	{
		HxLOG_Error( "[ DMS ] Error> Can't initialize DMS!!!!\n" );
		goto func_error;
	}

	if (PL_DMS_Start() != ERR_OK) /* sometimes, takes too long time because of mxlib delay(network error, ...) */
	{
		HxLOG_Error( "[ DMS ] Error> Can't start DMS\n" );
		goto func_error;
	}

	(void)PL_DMS_RegisterStatusEventCallback(__dms_event_callback);

	pDmsCtx->eStatus = eDMS_STATUS_STARTED;

#if defined(CONFIG_HOMMA_WOON)
	WOONS_Start(pDmsCtx->stConfig.szFriendlyName, pIface->szMacAddr);
#endif

	// http://svn.humaxdigital.com:3000/issues/111152
	PL_DMR_RequestBringInURL();

	return ERR_OK;

func_error:
	/* */
	HxLOG_Error( "[ DMS ] Error> failed to start DMS; TODO) Deallocate resources!\n" );
	pDmsCtx->eStatus = eDMS_STATUS_STOPPED;
	return ERR_FAIL;
}

static HBOOL _dms_IsDetachableHDDScanable(void)
{
	return TRUE;
}

static HBOOL _dms_IsUsbHDDScanable(void)
{
// 2016/3/11 mhkang: http://svn.humaxdigital.com:3000/issues/111264#note-1
#ifdef CONFIG_PROD_HMS1000S	// HMS-1000S does not support USB-HDD for DMS.
	return FALSE;
#endif

	return TRUE;
}

static HBOOL _dms_IsUsbMemoryScanable(void)
{
	return FALSE;
}

static inline HERROR _dms_stop( DMS_Context_t *pDmsCtx )
{
	if (pDmsCtx->eStatus < eDMS_STATUS_STARTING)
	{
		HxLOG_Error( "[ DMS ] Error> Already stopped DMS status(%d)!\n", pDmsCtx->eStatus);
		return ERR_FAIL;
	}

	if (pDmsCtx->eStatus == eDMS_STATUS_STARTING)
	{
		HxLOG_Error( "[ DMS ] Error> Invalid DMS status(%d)! Waiting prev DMS starting\n", pDmsCtx->eStatus);
		pDmsCtx->eStatus = eDMS_STATUS_STOPPING;
		/* TODO: event wait or waiting loop */
	}
	pDmsCtx->eStatus = eDMS_STATUS_STOPPING;

	// Reset Storage context
	HxSTD_MemSet( &pDmsCtx->stStorageInfo, 0x00, sizeof(CO_STORAGE_Info_t) );
	HxSTD_MemSet( &pDmsCtx->stDefMount, 0x00, sizeof(CO_STORAGE_Mount_t) );

	//(void)PL_HLS_UnregisterEventCallback();
	(void)PL_DMS_UnregisterStatusEventCallback();

#if defined(CONFIG_HOMMA_WOON)
	WOONS_Stop();
#endif

	PL_DMS_Stop(); /* sometimes, takes too long time because of mxlib/DB */
	PL_DMS_DeInit();

	pDmsCtx->eStatus = eDMS_STATUS_STOPPED;

	return ERR_OK;
}

static inline void __dms_add_mountPath( CO_STORAGE_Mount_t *pstMount )
{
	PL_DMS_ScanOpt_t MediaScanOpt = { 0 };
	PL_DMS_ScanOpt_t RecordScanOpt = { 0 };
	HBOOL bScanMediaPath = FALSE;
	HBOOL bScanRecordPath = FALSE;

	if (pstMount == NULL)
	{
		HxLOG_Error( "[ DMS ] Error> NULL mountPath!\n" );
		return;
	}

	if (eSTORAGE_TYPE_HDD != pstMount->eType)
	{
		return;
	}

	if (pstMount->IsScanable == FALSE)
	{
		HxLOG_Info("Not scannale storage(%s))\n", pstMount->szPath);
		return;
	}

	HxLOG_Print(HxANSI_COLOR_YELLOW("MediaPath:%s RecordPath:%s)\n"), pstMount->szMediaPath, pstMount->szRecordPath);

	if ( HxSTD_StrLen(pstMount->szMediaPath) )
	{
		MediaScanOpt.eDepth			= ePL_DMS_ScanDepth_Recursive;
		MediaScanOpt.pszFilter		= PL_DMS_DEFAULT_SCANFILTER_MEDIA;
		MediaScanOpt.pszUserData	= PL_DMS_DEFAULT_CONTENT_CONTAINER_TITLE;// exclude Recording folder. refer to http://svn:3000/issues/111098
		MediaScanOpt.ePath			= ePL_DMS_ScanPath_External_Root;

		bScanMediaPath = TRUE;
	}

	if ( HxSTD_StrLen(pstMount->szRecordPath) > 0)
	{
		RecordScanOpt.eDepth		= ePL_DMS_ScanDepth_OneDepth;
		RecordScanOpt.pszFilter		= NULL;
		RecordScanOpt.pszUserData	= NULL;
		RecordScanOpt.ePath			= ePL_DMS_ScanPath_External_Sub;

		bScanRecordPath = TRUE;
	}

	if (bScanMediaPath == TRUE || bScanRecordPath == TRUE)
	{
		PL_DMS_StopScan();
	}

	if (bScanMediaPath == TRUE)
	{
		PL_DMS_AddScanDirectory( pstMount->szMediaPath, &MediaScanOpt );
	}

	if (bScanRecordPath == TRUE && pstMount->bPvrFlag)
	{
		PL_DMS_AddScanDirectory( pstMount->szRecordPath, &RecordScanOpt );
	}

	if (bScanMediaPath == TRUE || bScanRecordPath == TRUE)
	{
		PL_DMS_StartScan( PL_DMS_DEFAULT_RESCAN_INTERVAL, PL_DMS_DEFAULT_RESCAN_INTERVAL_FILE );
	}
}

static inline void __dms_remove_mountPath( CO_STORAGE_Mount_t *pstMount )
{
	HERROR hErr = ERR_OK;
	HINT32 scanCnt = 0;
	HCHAR *scanPath = NULL;
	HBOOL bStopScan = FALSE;
	HBOOL bInternalHddScan = FALSE;
	HBOOL bDetachableHddScan = FALSE;
	HBOOL bUSBHddScan = FALSE;
	HUINT16 uMatchCnt = 0;
	HINT32 pathLen = 0;
	HxList_t *pathList = NULL;
	HxList_t *pathItem = NULL;
	void *scanData= NULL;
	HINT32 scanOption = ePL_DMS_ScanPath_Normal;

	/* Internal-HDD: Media, Record */
	if ((eSTORAGE_TYPE_HDD == pstMount->eType) &&
		(eSTORAGE_IFACE_SATA == pstMount->eInterface) &&
		(eSTORAGE_LOC_INTERNAL == pstMount->eLocation))
	{
		 bInternalHddScan = TRUE;
	}

	/* Detachable-HDD: Media, Hidden Record(/.recordings) */
	if ((eSTORAGE_TYPE_HDD == pstMount->eType) &&
		(eSTORAGE_IFACE_SATA == pstMount->eInterface) &&
		(eSTORAGE_LOC_EXTERNAL == pstMount->eLocation))
	{
		bDetachableHddScan = TRUE;

		if ( !HxSTD_StrLen(pstMount->szMediaPath) )
		{
			if ( HxSTD_StrLen(pstMount->szPath) )
			{
				/* Use "/" root with Media directory beause detachable-HDD has no "Media" directory.  */
				HxSTD_StrNCpy( pstMount->szMediaPath, pstMount->szPath, sizeof(pstMount->szMediaPath)-1 );
			}
			else
			{
				/* Remove last "/" slash for string matching with CDS/DB scan path */
				HxSTD_StrNCpy( pstMount->szMediaPath, "/media/ehd1", sizeof(pstMount->szMediaPath)-1 );
			}
		}

		if ( !HxSTD_StrLen(pstMount->szRecordPath) )
		{
			/* Remove last "/" slash for string matching with CDS/DB scan path */
			HxSTD_StrNCpy( pstMount->szRecordPath, "/media/ehd1/.recordings", sizeof(pstMount->szRecordPath)-1 );
		}
	}

	/* USB-HDD: Media, Hidden Record(/.recordings) */
	if ((eSTORAGE_TYPE_HDD == pstMount->eType) &&
		(eSTORAGE_IFACE_USB == pstMount->eInterface) &&
		(eSTORAGE_LOC_EXTERNAL == pstMount->eLocation))
	{
		bUSBHddScan = TRUE;

		if ( !HxSTD_StrLen(pstMount->szMediaPath) )
		{
			if ( HxSTD_StrLen(pstMount->szPath) )
			{
				/* Use "/" root with Media directory beause detachable-HDD has no "Media" directory.  */
				HxSTD_StrNCpy( pstMount->szMediaPath, pstMount->szPath, sizeof(pstMount->szMediaPath)-1 );
			}
			else
			{
				/* Remove last "/" slash for string matching with CDS/DB scan path */
				HxSTD_StrNCpy( pstMount->szMediaPath, "/media/drive1", sizeof(pstMount->szMediaPath)-1 );
			}
		}

		if ( !HxSTD_StrLen(pstMount->szRecordPath) )
		{
			/* Remove last "/" slash for string matching with CDS/DB scan path */
			HxSTD_StrNCpy( pstMount->szRecordPath, "/media/drive1/.recordings", sizeof(pstMount->szRecordPath)-1 );
		}
	}

	HxLOG_Debug("[ DMS ][%s] bInternalHddScan (%d), bDetachableHddScan (%d), bUSBHddScan (%d)\n", __FUNCTION__, bInternalHddScan, bDetachableHddScan, bUSBHddScan);

	pathLen = HxSTD_StrLen( pstMount->szPath );
	if ( 0 == pathLen )
	{
		HxLOG_Error( "[ DMS ][%s] Error> can't get scan dir!\n", __FUNCTION__);
		return;
	}

	hErr = PL_DMS_GetScanPathList( &scanCnt, &pathList );
	if ((hErr != ERR_OK) || (!scanCnt))
	{
		HxLOG_Error( "[ DMS ][%s] Error> can't get scan dir(cnt:%d)!\n", __FUNCTION__, scanCnt);
		goto END_PROC;
	}

	pathItem = pathList;

	while ( pathItem )
	{
		scanData = (void *)HLIB_LIST_Data( pathItem );

		if(PL_DMS_GetScanPath( scanData, &scanPath ) != ERR_OK)
		{
			goto NEXT_ITEM;
		}

		if(PL_DMS_GetScanOption( scanData, &scanOption) != ERR_OK)
		{
			goto NEXT_ITEM;
		}

		if(scanOption == ePL_DMS_ScanPath_External_Sub)
		{
			HxLOG_Error("[%s] we don't remove scan directory with scan option [MXDMS_SCAN_PATH_DETACHABLE_SUB]!\n", __FUNCTION__);
			goto NEXT_ITEM;
		}

		if (bDetachableHddScan == TRUE)
		{
			if ( 0 == HxSTD_StrNCmp( pstMount->szMediaPath, scanPath, pathLen) )
			{
				uMatchCnt++;
				PL_DMS_StopScan();
				if (PL_DMS_RemoveScanDirectory(scanPath) != ERR_OK)
				{
					HxLOG_Error( "[ DMS ][%s] Error> failed to remove scan path(%s)!\n", __FUNCTION__, scanPath);
				}
				bStopScan = TRUE;
			}
		}

		if (bUSBHddScan == TRUE)
		{
			if ( 0 == HxSTD_StrNCmp( pstMount->szPath, scanPath, pathLen) )
			{
				uMatchCnt++;
				PL_DMS_StopScan();
				if (PL_DMS_RemoveScanDirectory(scanPath) != ERR_OK)
				{
					HxLOG_Error( "[ DMS ][%s] Error> failed to remove scan path(%s)!\n", __FUNCTION__, scanPath);
				}
				bStopScan = TRUE;
			}
		}
NEXT_ITEM :
		pathItem = pathItem->next;
	}


	if (!uMatchCnt)
	{
		HxLOG_Warning( "[ DMS ][%s] There is no matched scan directories!\n", __FUNCTION__);
	}

	if( bStopScan )
	{
		PL_DMS_StartScan( PL_DMS_DEFAULT_RESCAN_INTERVAL, PL_DMS_DEFAULT_RESCAN_INTERVAL_FILE );
	}

END_PROC :
	(void)PL_DMS_FreeScanPathList( pathList );
}

static HBOOL __dms_isScanningNeed( DMS_Context_t *pDmsCtx, CO_STORAGE_Mount_t *pstMount )
{
	/* Model마다 Scan 지원 여부가 달라질 수 있음 */
	if ((NULL == pDmsCtx) || (NULL == pstMount))
	{
		HxLOG_Error("[ DMS ] pstMount is null \n");
		return FALSE;
	}

	if ( eSTORAGE_IFACE_USB == pstMount->eInterface )
	{
		/* 이 부분은 모델마다 다르다 */
		if ( eSTORAGE_TYPE_HDD == pstMount->eType && pDmsCtx->bScanUsbHDD )
			return TRUE;

		if ( eSTORAGE_TYPE_MEM == pstMount->eType && pDmsCtx->bScanUsbMemory )
			return TRUE;

		return FALSE;
	}

	if ( eSTORAGE_LOC_INTERNAL != pstMount->eLocation )
	{
		return TRUE;
	}

	return TRUE;
}

static HBOOL __dms_isValid_scanPath( const HCHAR *pszPath )
{
	HERROR hErr = ERR_OK;
	PL_FS_FileStat_t fileInfo;
	HINT32 nRet = 0;
	HINT32 fileType = 0;
	HINT32 i = 0;
	HINT32 scanCnt = 0;
	HCHAR  *pszComparePath = NULL;
	HBOOL  bMatch = FALSE;
	HxList_t *pathList = NULL;
	HxList_t *pathItem = NULL;
	void *scanData= NULL;

	hErr = PL_DMS_GetScanPathList( &scanCnt, &pathList );
	if (hErr != ERR_OK)
	{
		HxLOG_Error( "[ DMS ] Error> can't get valid scan dir(%s)\n", pszPath);
		return FALSE;
	}

	pathItem = pathList;
	for ( i = 0; i < scanCnt && pathItem; i++  )
	{
		scanData = (void *)HLIB_LIST_Data( pathItem );
		(void)PL_DMS_GetScanPath( scanData, &pszComparePath );
		if ( NULL != HxSTD_StrStr( pszPath, pszComparePath ) )
		{
			bMatch = TRUE;
			break;
		}
		pathItem = pathItem->next;
	}
	(void)PL_DMS_FreeScanPathList( pathList );

	if ( TRUE != bMatch )
	{
		HxLOG_Debug( "[ DMS ] Error> Unmatched (%s) from scan dir\n", pszPath);
		return FALSE;
	}

	HxSTD_MemSet( &fileInfo, 0x00, sizeof(PL_FS_FileStat_t) );
	nRet = PL_FS_GetFileInfo( pszPath, 0, &fileInfo );
#define PL_FS_FILE_NOT_FOUND (-1)
	if ( PL_FS_ERROR_NO_MORE_RESOURCES == nRet )
	{
		fileType = PL_FS_FILE_NOT_FOUND;
	}
	else
	{
		fileType = fileInfo.fileType;
	}

	/* Folder일 경우 바로 스캔해도 되지만, Copy/Remove 완료된 파일이 자막 파일일 경우, 실제
	 * Copy/Remove 진행중인 Video Item도 같이 스캔 해버리기에 확장자 필터링을 한다.
	 * (잘못된 DB가 만들어진다.)
	 */
	if ( PL_FS_FILE_TYPE_FILE == fileType || PL_FS_FILE_NOT_FOUND == fileType )
	{
		/* PAL_DMS_FILEDIR_TYPE_NOT_FOUND_ERROR 일 경우는 eUAPI_DELETE or eUAPI_MOVE가 호출 될 때다.
		 * (Delete/Move 일경우에 pszSource 경로에 이벤트 시점에 파일이 없기에 ERROR가 Return 된다)
		 */
		HCHAR szFileExt[8] = { 0 };
		HCHAR *pExtPos  = NULL;

		pExtPos = HxSTD_StrRChar( pszPath, '.' );
		if ( NULL ==  pExtPos )
		{
			return FALSE;
		}

		HxSTD_StrNCpy( szFileExt, (HCHAR *)(pExtPos+1), sizeof(szFileExt)-1 );
		if( NULL == HxSTD_StrStr( PL_DMS_DEFAULT_SCANFILTER_VIDEO, szFileExt ) &&
				NULL == HxSTD_StrStr( PL_DMS_DEFAULT_SCANFILTER_MUSIC, szFileExt ) &&
				NULL == HxSTD_StrStr( PL_DMS_DEFAULT_SCANFILTER_PHOTO, szFileExt ) &&
				NULL == HxSTD_StrStr( PL_DMS_DEFAULT_SCANFILTER_RECORD, szFileExt ) &&
				NULL == HxSTD_StrStr( "hmx", szFileExt ) )
		{
			HxLOG_Error("[ DMS ] Error> file is filtered by extension name(%s)!\n", szFileExt );
			return FALSE;
		}
	}
	else if ( PL_FS_FILE_TYPE_DIR != fileType )
	{
		return FALSE;
	}

	return TRUE;
}

static HERROR __dms_renewal_scanPath( DMS_Context_t *pDmsCtx, CO_STORAGE_Info_t *pstStorageInfo )
{
	HERROR hError = ERR_OK;
	CO_STORAGE_Mount_t *pMount = NULL;
	HBOOL bRenewCDS = TRUE;
	HINT32 i = 0;
	HINT32 mntCnt = 0;
	HINT32 scanCnt = 0;
	HCHAR *scanPath = NULL;
	/* HCHAR mediaPath[256] = { 0 }; */
	HxList_t *pathList = NULL;
	HxList_t *pathItem = NULL;
	void *scanData = NULL;

	mntCnt = pstStorageInfo->nMountCnt;

	/*
	 * CDS/DB에 없는 storage는 명시적으로 삭제해줘야 한다.
	 * Detachable/USB-HDD의 경우 (un)mount event 이외에 dms_start() 시점의 storage check 필요함
	 */
	if (bRenewCDS == TRUE)
	{
		/* 저장된 스캔 경로를 읽어온다 */
		hError = PL_DMS_GetScanPathList( &scanCnt, &pathList );
		if (hError != ERR_OK)
		{
			HxLOG_Error("[ DMS ] Error> Cant get scan path list!\n");
		}
		else
		{
			pathItem = pathList;
			/* 이전에 저장된 경로가 실제 존재하는지 확인하고 없으면, 제거한다. */
			while ( pathItem )
			{
				scanData = (void *)HLIB_LIST_Data( pathItem );
				(void)PL_DMS_GetScanPath( scanData, &scanPath );
				if ( TRUE != PL_FS_IsDirExist( scanPath ) )
				{
					HxLOG_Print("[ DMS ] Rmoved path from Library=%s\n", scanPath );
					if (PL_DMS_RemoveScanDirectory(scanPath) != ERR_OK)
					{
						HxLOG_Error( "[ DMS ][%s] Error> failed to remove scan path(%s)!\n", __FUNCTION__, scanPath);
					}
				}
				else
				{
					HxLOG_Print("[ DMS ] Existed path from Library=%s\n", scanPath );
				}
				pathItem = pathItem->next;
			}

			(void)PL_DMS_FreeScanPathList( pathList );
		}
	}

	for( i = 0; i < mntCnt; i++ )
	{
		pMount = (CO_STORAGE_Mount_t *)&pstStorageInfo->pstMounts[i];
		if ( TRUE != __dms_isScanningNeed( pDmsCtx, pMount ) )
		{
			HxLOG_Error("[ DMS ] Error> scan_dir[UUID:%s, %s] is skipped! [s_id:%d, p_id:%d, type:%d, if:%d, loc:%d]\n",
						pMount->szUUID, pMount->szPath,
						pMount->ulStorageId, pMount->ulPartitionId,
						pMount->eType, pMount->eInterface, pMount->eLocation);
			continue;
		}
		__dms_add_mountPath( pMount );
	}

#ifdef CONFIG_DEBUG
	i = 0;
	if(PL_DMS_GetScanPathList( &scanCnt, &pathList ) != ERR_OK)
		return ERR_FAIL;

	/*  if(scanCnt == 0 || pathList == NULL) 	return ERR_FAIL; */

	pathItem = pathList;
	while ( pathItem )
	{
		scanData = (void *)HLIB_LIST_Data( pathItem );
		(void)PL_DMS_GetScanPath( scanData, &scanPath );
		HxLOG_Warning(HxANSI_COLOR_YELLOW("[ DMS ] index : %d,  SCAN_PATH : %s\n"), i, scanPath );
		i++;
		pathItem = pathItem->next;
	}
	(void)PL_DMS_FreeScanPathList( pathList );
#endif

	return ERR_OK;
}

static HERROR __dms_set_scan_path( DMS_Context_t *pDmsCtx, CO_STORAGE_Info_t *pstStorageInfo )
{
	CO_STORAGE_Mount_t *pMount = NULL;
	HINT32 i = 0, mntCnt = 0;

	if ((pDmsCtx == NULL) || (pstStorageInfo == NULL))
	{
		HxLOG_Error( "[ DMS ] Error> invalid scan arg!\n");
		return ERR_FAIL;
	}

	/* CDS/DB 결과가 sub_dir 중복 or sub_dir 미포함인 경우 간헐적으로 발생 */
	mntCnt = pstStorageInfo->nMountCnt;
	for( i = 0; i < mntCnt; i++ )
	{
		pMount = (CO_STORAGE_Mount_t *)&pstStorageInfo->pstMounts[i];
		if ( TRUE != __dms_isScanningNeed( pDmsCtx, pMount ) )
		{
			HxLOG_Error("[ DMS ] Error> scan_dir[UUID:%s, %s] is skipped! [s_id:%d, p_id:%d, type:%d, if:%d, loc:%d]\n",
						pMount->szUUID, pMount->szPath,
						pMount->ulStorageId, pMount->ulPartitionId,
						pMount->eType, pMount->eInterface, pMount->eLocation);
			continue;
		}

		HxLOG_Print( "[ DMS ] [%d/%d] Adding scan_dir[UUID:%s, %s], [s_id:%d, p_id:%d, type:%d, if:%d, loc:%d]\\n",
						i, mntCnt,
						pMount->szUUID, pMount->szPath,
						pMount->ulStorageId, pMount->ulPartitionId,
						pMount->eType, pMount->eInterface, pMount->eLocation);
		__dms_add_mountPath( pMount );
	}

/*
#ifdef CONFIG_DEBUG
	i = 0;
	PL_DMS_GetScanPathList( &scanCnt, &pathList );
	pathItem = pathList;
	while ( pathItem )
	{
		scanPath = (HCHAR *)HLIB_LIST_Data( pathItem );
		HxLOG_Print(HxANSI_COLOR_YELLOW("[ DMS ] index : %d,  SCAN_PATH : %s\n"), i, scanPath );
		i++;
		pathItem = pathItem->next;
	}
	(void)PL_DMS_FreeScanPathList( pathList );
#endif
*/

	return ERR_OK;
}

static HERROR 	__dms_handle_fileOper( DMS_Context_t *pDmsCtx, HOMMA_BUS_Event_e eEvent, HCHAR *pszPath )
{
	PL_DMS_FILEMGR_TYPE_e eMgrType = PL_DMS_FILEMGR_ADD;

	if ( eDMS_STATUS_STARTED != pDmsCtx->eStatus )
	{
		HxLOG_Error( "[ DMS ][%s] Error> Inactive DMS(%d)!\n", __FUNCTION__, pDmsCtx->eStatus);
		return ERR_FAIL;
	}

	switch ( eEvent )
	{
		case eBUS_EVENT_SYS_FILE_COPY_DONE :
			HxLOG_Print( "[%s:%d] FILE COPY ( %s ) \n", __FUNCTION__, __LINE__, pszPath );
			eMgrType = PL_DMS_FILEMGR_ADD;
			break;
		case eBUS_EVENT_SYS_FILE_DELETE_DONE :
			HxLOG_Print( "[%s:%d] FILE DELETE ( %s ) \n", __FUNCTION__, __LINE__, pszPath );
			eMgrType = PL_DMS_FILEMGR_REMOVE;
			break;
		case eBUS_EVENT_SYS_FILE_MOVE_DONE :
			HxLOG_Print( "[%s:%d] FILE MOVE ( %s ) \n", __FUNCTION__, __LINE__, pszPath );
			break;
		case eBUS_EVENT_SYS_FILE_CREATE_DONE :
			HxLOG_Print( "[%s:%d] FILE CREATE ( %s ) \n", __FUNCTION__, __LINE__, pszPath );
			eMgrType = PL_DMS_FILEMGR_ADD;
			break;
		case eBUS_EVENT_SYS_REC_ADDED:
			HxLOG_Print( "[%s:%d] REC ADDED ( %s ) \n", __FUNCTION__, __LINE__, pszPath );
			eMgrType = PL_DMS_FILEMGR_ADD;
			break;
		case eBUS_EVENT_SYS_REC_DELETED: /* record file will be process by FILE_DELETE_DONE */
			HxLOG_Print( "[%s:%d] REC DELETED ( %s ) \n", __FUNCTION__, __LINE__, pszPath );
			eMgrType = PL_DMS_FILEMGR_ADD;
			break;
		case eBUS_EVENT_SYS_REC_UPDATED:
		default :
			return ERR_FAIL;
	}

	HOM_ASSERT(pszPath);

	/*  TODO@20130914 */
	if ( TRUE != __dms_isValid_scanPath( pszPath ) )
	{
		HxLOG_Print( "[ DMS ] Error> %s File is skipped.\n", pszPath );
		return ERR_FAIL;
	}

	if (0)
	{
		PL_DMS_ScanDirect( pszPath, eMgrType );
	}
	else
	{
		PL_DMS_RescanFSS();
	}

	return ERR_OK;
}

static HERROR	__dms_handle_mountOper( DMS_Context_t *pDmsCtx, HOMMA_BUS_Event_e eEvent, CO_STORAGE_Mount_t * pstMountInfo )
{
	HOM_ASSERT(pstMountInfo);

	if ( eDMS_STATUS_STARTED != pDmsCtx->eStatus )
	{
		HxLOG_Error( "[ DMS ][%s] Error> MountOper() Failed. -  Invalid DMS status(%d)!\n", __FUNCTION__,  pDmsCtx->eStatus);
		return ERR_FAIL;
	}

	HxLOG_Print( "[ DMS ] Mount Path ( %s ).\n", pstMountInfo->szPath );

	if ( TRUE != __dms_isScanningNeed( pDmsCtx, pstMountInfo ) )
	{
		HxLOG_Error( "[ DMS ] Error> need not to scan Mount Path for DMS.\n");
		return ERR_FAIL;
	}

	switch ( eEvent )
	{
		case eBUS_EVENT_SYS_FS_MOUNTED :
			HxLOG_Print("[ DMS ] == ADD Mount SCAN_PATH( %s )\n", pstMountInfo->szPath );
			__dms_add_mountPath( pstMountInfo );
			break;
		case eBUS_EVENT_SYS_FS_UNMOUNTED :
			HxLOG_Print("[ DMS ] == REMOVE Mount SCAN_PATH( %s )\n", pstMountInfo->szPath );
			__dms_remove_mountPath( pstMountInfo );
			break;
		default :
			return ERR_FAIL;
	}

	return ERR_OK;
}

#define ________________Public_Interfaces__________________________

void 	DMS_Initialize( void ) /* Called once after spawning homma process */
{
	DMS_Context_t 	*pDmsCtx = __dms_getContext();
	HOM_MEMSET( pDmsCtx, 0x00, sizeof( DMS_Context_t ) );
	{
		pDmsCtx->eStatus = eDMS_STATUS_STOPPED;
		pDmsCtx->bReqStart = FALSE;
		pDmsCtx->cbDmsEvtNotify = NULL;
		pDmsCtx->unSyncEventTaskId = 0x0;
		pDmsCtx->unSyncEventMsgId  = 0x0;
		pDmsCtx->unSemId           = 0x0;
		pDmsCtx->bLooseStandbyWait            = TRUE;
		pDmsCtx->ulLastStreamingRecvTick      = 0x0;
		pDmsCtx->ulLastStandbyWaitSendTick    = 0x0;
		pDmsCtx->ulLastStandbyWaitIgnoredTick = 0x0;
		pDmsCtx->bInited = TRUE;
	}
}

HERROR 	DMS_OnSystemEvent( HOMMA_BUS_Event_e eEvent, void *eDataType, void *pvData  )
{
	DMS_Context_t *pDmsCtx = __dms_getContext();
	CO_STORAGE_Mount_t *pMountInfo = NULL;
	CO_NET_Iface_t *pIfaceInfo = NULL;
	HERROR hErr;
	HUINT32	DeviceID;

	HxLOG_Error( "[ DMS ]++[system event:%d]\n", eEvent);

	switch ( eEvent )
	{
		case eBUS_EVENT_SYS_FS_MOUNTED :
			if ( (eDMS_STATUS_STARTED != pDmsCtx->eStatus) && pDmsCtx->bReqStart )
			{
				HxLOG_Error( "[ DMS ][%s] DMS Start(status:%d, req:%d) by Mount Event!\n", __FUNCTION__, pDmsCtx->eStatus, pDmsCtx->bReqStart);
				_dms_start(&pDmsCtx->stConfig );
			}
			/* 아래 수행을 위해 break가 없다 */

		case eBUS_EVENT_SYS_FS_UNMOUNTED :
			pMountInfo = (CO_STORAGE_Mount_t *)pvData;
			if ( pMountInfo )
			{
				HxLOG_Print( "[ DMS ] Mount Event  PATH : %s\n", pMountInfo->szPath );
			}
			__dms_handle_mountOper( pDmsCtx, eEvent, pMountInfo );
			break;

		case eBUS_EVENT_SYS_NET_IF_ONLINE :
			pIfaceInfo = (CO_NET_Iface_t *)pvData;
			if (pIfaceInfo)
			{
				HxSTD_MemCopy( &pDmsCtx->stDefIface, pIfaceInfo, sizeof(CO_NET_Iface_t) );
				if(eDMS_STATUS_STARTED != pDmsCtx->eStatus && pDmsCtx->bReqStart )
				{
					_dms_start(&pDmsCtx->stConfig ); //FriendlyName is already set in this function
					HxLOG_Error( "[ DMS ] DMS Start() by ONLINE Event : %s\n", pIfaceInfo->szName );
				}
				else
				{
					DMS_SetName( pDmsCtx->stConfig.szFriendlyName );
					HxLOG_Error( "[ DMS ] Update FriendlyName\n");
				}
			}
			break;

		case eBUS_EVENT_SYS_NET_IF_OFFLINE :
			if (0)
			{
				/* redmine #78981: Dont DMS stop() because it is blurred distinction between NET_IF_OFFLINE and NET_IF_SWITCHING */
				pIfaceInfo = (CO_NET_Iface_t *)pvData;
				if ( pIfaceInfo )
				{
					HxLOG_Error( "[ DMS ] OFFLINE Event : %s\n", pIfaceInfo->szName );
					DMS_Stop();
				}
			}
			else
			{
				HxLOG_Error( "[ DMS ] Error> Unsupported OFFLINE event\n");
			}
			break;

		case eBUS_EVENT_SYS_NET_IF_SWITCHING :
			pIfaceInfo = (CO_NET_Iface_t *)pvData;
			if ( pIfaceInfo )
			{
				HxLOG_Error( "[ DMS ] NET_IF_SWITCHING Event : %s\n", pIfaceInfo->szName );
				HxSTD_MemCopy( &pDmsCtx->stDefIface, pIfaceInfo, sizeof(CO_NET_Iface_t) );
				DMS_SetName( pDmsCtx->stConfig.szFriendlyName );
			}
			break;

		case eBUS_EVENT_SYS_POWER_REBOOT:
			DMS_Stop();
			break;

		case eBUS_EVENT_SYS_MS_FORMAT_TRY:
			{
				DeviceID = (HUINT32)pvData;
				HxLOG_Info(HxANSI_COLOR_YELLOW("RECEIVE(DeviceID:%X) eBUS_EVENT_SYS_MS_FORMAT_TRY\n"), DeviceID);
				HOMMA_SYSTEM_RemoveDevice(DeviceID);
			}
			break;

		case eBUS_EVENT_SYS_MS_FORMAT_DONE:
			{
				DeviceID = (HUINT32)pvData;
				HxLOG_Info(HxANSI_COLOR_YELLOW("RECEIVE(DeviceID:%X) eBUS_EVENT_SYS_MS_FORMAT_DONE\n"), DeviceID);
				HOMMA_SYSTEM_AddDevice(DeviceID);
			}

			break;

		case eBUS_EVENT_SYS_PW_FACTORYDEFAULT:
			{
				HxLOG_Error( "[ DMS ] System Event(eBUS_EVENT_SYS_PW_FACTORYDEFAULT) - DMS Stop() \n");
				DMS_Stop(); /* dont use _dms_stop() for clearing prev failure, ... for format */

				HxLOG_Info( "[ DMS ] delete Main DB(path:%s)\n" , PL_DMS_DEFAULT_DB_PATH);
				hErr = HLIB_FILE_Delete(PL_DMS_DEFAULT_DB_PATH);
				if (hErr != ERR_OK)
				{
					HxLOG_Error( "[ DMS ] Cannot delete Main DB(errno:%d)\n", errno);
				}
			}
			break;

		case eBUS_EVENT_SYS_FILE_COPY_DONE :
		case eBUS_EVENT_SYS_FILE_DELETE_DONE :
		case eBUS_EVENT_SYS_FILE_MOVE_DONE :
		case eBUS_EVENT_SYS_FILE_CREATE_DONE :
		case eBUS_EVENT_SYS_REC_ADDED :
		case eBUS_EVENT_SYS_REC_DELETED : /* record file will be process by FILE_DELETE_DONE */
			/* 2015/12/03 mhkang:
				DMS의 PVR Content Browse방식이 Callback방식으로 변경되었다(기존은 DMS DB로 관리).
				따라서 더 이상 Umma로부터 파일 Add/Modify/Remove event를 받아서 처리할 필요가 없다.
			*/
			if (0)
			{
				CO_FileOperation_Event_t *evt = (CO_FileOperation_Event_t *)pvData;
				HxLOG_Print( "[ DMS ] FILE OPER Event(evt:%d, path:%s]\n", eEvent, (HCHAR *)evt->szPath);
				__dms_handle_fileOper( pDmsCtx, eEvent, (HCHAR *)evt->szPath);
			}
			break;

		case eBUS_EVENT_SYS_REC_UPDATED :
		default :
			return ERR_OK;
	}

	return ERR_OK;
}

void 	DMS_SetEvtListener( DMS_EVT_HANDLE_Func cbNotify )
{
	DMS_Context_t *pDmsCtx = __dms_getContext();
	pDmsCtx->cbDmsEvtNotify = cbNotify;
}

HERROR	DMS_SetNetworkInfo( const CO_NET_Info_t *pstNetInfo )
{
	DMS_Context_t *pDmsCtx = __dms_getContext();
	HINT32	defIdx = 0;
	if ( !pstNetInfo )
		return ERR_FAIL;

	defIdx = pstNetInfo->nDefIdx;

	if ( pstNetInfo->pstIfaces )
	{
		HxSTD_MemCopy( &pDmsCtx->stDefIface, &pstNetInfo->pstIfaces[defIdx], sizeof(CO_NET_Iface_t) );
	}

	return ERR_OK;
}

HERROR	DMS_SetStorageInfo( const CO_STORAGE_Info_t *pstStoInfo )
{
	DMS_Context_t *pDmsCtx = __dms_getContext();
	HINT32 defIdx = 0, mntCnt = 0;

	HOM_ASSERT(pstStoInfo);

	if ((pstStoInfo->pstMounts == NULL) || (pstStoInfo->nMountCnt == 0))
	{
		HxLOG_Error( "[ DMS ] Error> invalid storage mount infos(cnt:%d)!\n", pstStoInfo->nMountCnt);
		return ERR_FAIL;
	}
	defIdx = pstStoInfo->nDefaultIdx;
	mntCnt = pstStoInfo->nMountCnt;

	HxSTD_MemSet( &pDmsCtx->stStorageInfo, 0x00, sizeof(CO_STORAGE_Info_t) );
	HxSTD_MemCopy( &pDmsCtx->stStorageInfo, pstStoInfo, sizeof(CO_STORAGE_Info_t) );

	HxLOG_Print("[ DMS ] == SetStorageInfo defIdx : %d, MountCount : %d\n", defIdx, mntCnt );

	if ( pstStoInfo->pstMounts )
		HxSTD_MemCopy( &pDmsCtx->stDefMount, &pstStoInfo->pstMounts[defIdx], sizeof(CO_STORAGE_Mount_t) );

	return ERR_OK;
}

HERROR	DMS_Start( DMS_Config_t *pstConfig )
{
	DMS_Context_t *pDmsCtx = __dms_getContext();
	HERROR retErr = ERR_FAIL;
	HxLOG_Error("[ DMS ] (%s)++\n", __FUNCTION__);

	HOM_ASSERT(pstConfig);

	pDmsCtx->bReqStart = TRUE;

	retErr = _dms_start(pstConfig );
	if ( retErr != ERR_OK)
	{
		HxLOG_Error( "[ DMS ] Error> Fail Dms Start !!!!\n" );
		return ERR_FAIL;
	}

	/* For CDS/DB renewal */
	/* PL_DMS_StopScan(); */
	if (0)
	{
		__dms_set_scan_path( pDmsCtx, &pDmsCtx->stStorageInfo );
	}
	else
	{
		/*
		 * 부팅이나 시작시 이전과  마운트 상태가 다를 수 있기에 갱신한다.
		 * renewal하지 않으면 detach-HDD CDS/DB 엉킴 문제 발생!
		 */
		__dms_renewal_scanPath( pDmsCtx, &pDmsCtx->stStorageInfo );
	}

	PL_DMS_StartScan( PL_DMS_DEFAULT_RESCAN_INTERVAL, PL_DMS_DEFAULT_RESCAN_INTERVAL_FILE );

	if (pDmsCtx->cbDmsEvtNotify != NULL)
		pDmsCtx->cbDmsEvtNotify(eDMS_EVT_START, 0, 0, 0);

	HxLOG_Error("[ DMS ] (%s)--\n", __FUNCTION__);
	return ERR_OK;
}

HERROR	DMS_Stop( void )
{
	DMS_Context_t *pDmsCtx = __dms_getContext();
	HERROR retErr = ERR_FAIL;
	HxLOG_Info("[ DMS ] (%s)++\n", __FUNCTION__);

	retErr = _dms_stop( pDmsCtx );

	pDmsCtx->bReqStart = FALSE;

	if (pDmsCtx->cbDmsEvtNotify != NULL)
		pDmsCtx->cbDmsEvtNotify(eDMS_EVT_STOP, 0, 0, 0);

	HxLOG_Info("[ DMS ] (%s)--\n", __FUNCTION__);
	return retErr;
}

HBOOL	DMS_IsStart(void)
{
	DMS_Context_t *pDmsCtx = __dms_getContext();

	if ( eDMS_STATUS_STARTED != pDmsCtx->eStatus )
		return FALSE;

	return TRUE;
}


HERROR	DMS_SetName( const HCHAR *pszFriendlyName )
{
	DMS_Context_t *pDmsCtx = __dms_getContext();
	HCHAR namebuf[256] = { 0 };

	if (pszFriendlyName == NULL)
	{
		HxLOG_Error( "[ DMS:%d ] Error> invalid friendlyName\n", __LINE__);
		return ERR_FAIL;
	}

	if ( eDMS_STATUS_STARTED != pDmsCtx->eStatus )
	{
		HxLOG_Error( "[ DMS ] SetName() Failed. -  Already Stopped DMS !!!!\n" );
		return ERR_FAIL;
	}

	/* "NAME+IP" 형태로 가공 : HMS-1000S 192.168.10.32  */
	HxSTD_StrNCpy( pDmsCtx->stConfig.szFriendlyName, pszFriendlyName, sizeof(pDmsCtx->stConfig.szFriendlyName)-1 );
	if ( pDmsCtx->bIpAttachOnName )
	{
		CO_NET_Iface_t *pIface = &pDmsCtx->stDefIface;
		HxSTD_snprintf( namebuf, sizeof(pDmsCtx->stConfig.szFriendlyName), "%s DMS %s", pszFriendlyName, pIface->szIpAddr );
	}
	else
		HxSTD_StrNCpy( namebuf, pszFriendlyName, sizeof(namebuf)-1 );

	PL_DMS_SetDmsName( namebuf );

	return ERR_OK;
}

HERROR	DMS_GetConfInfo( void * pvData )
{
	return ERR_OK;
}

HERROR	DMS_UpdateSyncEvent(HUINT32 unMsgId, void *pData)
{
	//__dms_SetSyncEventData(unMsgId, pData);
	return ERR_OK;
}

/*-----------------------------------------------------------------------------
 *  Fling
 *-----------------------------------------------------------------------------*/
/* Local Path를 통해 URI 및 Meta 정보를 얻는 함수
  Path sample: "/mnt/hd2/Recordings/Death in Paradise_20160204_2112.hjm"
*/
HERROR	DMS_GetFlingInfoFromPath( HCHAR *pPath, HCHAR **ppOutUri, HCHAR **ppOutMeta )
{
	return PL_DMS_GetFlingInfoFromPath( pPath, ppOutUri, ppOutMeta );
}

HERROR	DMS_GetFlingInfoOfPVRFromPath( HCHAR *pPath, HCHAR **ppOutUri, HCHAR **ppOutMeta )
{
	return PL_DMS_GetFlingInfoOfPVRFromPath( pPath, ppOutUri, ppOutMeta );
}

/* SVC Uid 을 통해 URI및 Meta 정보를 얻는 함수 */
HERROR	DMS_GetFlingInfoFromSvcUid( HINT32 hSvc, HCHAR **ppOutUri, HCHAR **ppOutMeta )
{
	return PL_DMS_GetFlingInfoFromSvcUid( hSvc, ppOutUri, ppOutMeta );
}

/* SVC Uid 을 통해 URI 정보들을 얻는 함수 */
HERROR	DMS_GetLiveStreamInfo( HINT32 hSvc, HUINT32 *pnCount, PL_DMS_LiveStreamInfo_t **pList )
{
	if(PL_DMS_GetLiveStreamInfo( hSvc, pnCount, pList ) != ePL_DMS_Result_Ok)
		return ERR_FAIL;

	return ERR_OK;
}
