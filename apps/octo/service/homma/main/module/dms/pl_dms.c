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
#if defined(CONFIG_HOMMA_CDI)
#include "pl_crypt.h"
#endif

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
#include "dlib_recording.h"
#include "homma_system.h"

#if defined(CONFIG_HOMMA_WOON)
#include "pl_woons.h"
#endif

#define PL_DMS_DEFAULT_DB_TYPE				SQLITE

typedef struct
{
	HCHAR *SerialNumber;			// 장치의 시리얼 번호
	HCHAR *SortCapabilities;		// 정렬 가능한 필드들을 CSV 형태로 지정
	HCHAR *SearchCapabilities;		// 검색 가능한 필드들을 CSV 형태로 지정
	HCHAR *SourceProtocolInfo;		// 서버가 지원하는 미디어 타입을 나열함
	HCHAR *FeatureList;				// 서버가 지원하는 Feature들에 대한 리스트를 XML 문서형태로 지정

	HCHAR *manufacturer;			// 제조사
	HCHAR *manufacturerURL;			// 제조사의 설명이 들어 있는 페이지의 URL

	HCHAR *defaultCreator;			// 명시적이지 않은 Creator의 기본 값
	HCHAR *Corporation;				// 회사이름
	//HCHAR *ModelNumber;			// 모델에 대한 넘버
	HCHAR *productCode;				// 제품 코드

	HCHAR *defaultSortCapabilities;	// 기본적인 서버의 소팅 방법
	HCHAR *uploadPath;				// DMS 내부에서 upload할 경로
	HCHAR *tmpUploadPath;			// 20091110 for 임시 파일을 스캔에서 제외하기 위해.
	HCHAR *thumbnailPath;			// DMS 내부에서 thumbnail을 저장할 경로

} PL_DMS_Server_t; /* MXDLNA_DMS_SERVER_INFO 를 재정의 */

static PL_DMS_ICON_t s_stDmsIcon[] =
{
	/*======================================================*/
	/*	{ "mime type", 	width,	height,	depth,	"icon url"} */
	/*======================================================*/
	{ "image/jpeg", 48,		48, 	24,	 	"/usr/dlna/dms_48.jpg" 	},
	{ "image/png",	48, 	48, 	24,	 	"/usr/dlna/dms_48.png" 	},
	{ "image/jpeg",	120, 	120, 	24,  	"/usr/dlna/dms_120.jpg" },
	{ "image/png",	120, 	120, 	24,  	"/usr/dlna/dms_120.png" }
};

typedef struct
{
	HBOOL					is_init;
	HBOOL					is_stopping;
	PL_DMS_Status_e			eStatus;

	MXDLNA_DMS_HANDLE		hDms;
	DMS_DB_HANDLE			hDmsDB;
	MXDLNA_FSS_HANDLE		hFSS;

	PL_DMS_Server_t			stIntConfig;
	PL_DMS_Config_t 		stExtConfig;

	HxList_t				*pChannelGroupList; /* Unused because it is divided to pl_dms_scan_epg */

	HUINT16					usUseTransfer;		/* DMS 에서 Transfer 동작이 진행중이면 */
	PL_DMS_StatusEventCallback	tStatusEvent_cb;
} PL_DMS_Context_t;

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
STATIC PL_DMS_Context_t s_PL_DMS_Context = { 0, };

#define ________________Internal_APIs_Prototype____________________
extern HERROR	PL_DMS_FilsOperation_SetUserCallbacks(void);
extern HERROR	PL_DMS_SetDmsHandles(void);
extern HERROR 	PL_DMS_ScanFilterInit(MXDLNA_FSS_SCANOPTION *stScanOption);
extern void 	PL_DMS_SetupTunerEpg( PL_DMS_EPG_TARGET_e eTarget );
extern void 	PL_DMS_ClearTunerEpg( void );
#if defined(PL_DMS_THIRDPARTY_PVR_FEATURE)
extern void 	PL_DMS_SetupForPVR( void );
#endif

#define ________________Internal_Functions_________________________
PL_DMS_Context_t *__pl_dms_GetContext( void )
{
	return &s_PL_DMS_Context;
}

const HCHAR* __pldms_StatusToStr( MXDLNADMS_CALL_STATUS status )
{
	switch (status)
	{
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SERVERINIT_DLNA_START);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SERVERINIT_DLNA_END);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SERVERINIT_FS_START);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SERVERINIT_FS_END);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SERVERINIT_SFM_START);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SERVERINIT_SFM_END);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SERVERINIT_FAIL);

		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SERVERDEINIT_DLNA_START);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SERVERDEINIT_DLNA_END);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SERVERDEINIT_FS_START);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SERVERDEINIT_FS_END);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SERVERDEINIT_SFM_START);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SERVERDEINIT_SFM_END);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SERVERDEINIT_FAIL);

		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SCAN_DIRECTSCAN_FILE_START);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SCAN_DIRECTSCAN_FILE_END);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SCAN_DIRECTSCAN_FILE_FAIL);

		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SCAN_DIRECTSCAN_FOLDER_START);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SCAN_DIRECTSCAN_FOLDER_END);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SCAN_DIRECTSCAN_FOLDER_STOP);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SCAN_DIRECTSCAN_FOLDER_FAIL);

		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SCAN_DIRECTSCAN_REMOVE_START);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SCAN_DIRECTSCAN_REMOVE_END);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SCAN_DIRECTSCAN_REMOVE_STOP);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SCAN_DIRECTSCAN_REMOVE_FAIL);

		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SCAN_ONESHOT_START);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SCAN_ONESHOT_END);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SCAN_ONESHOT_STOP);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SCAN_ONESHOT_FAIL);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SCAN_ONESHOT_REMOVE_START);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SCAN_ONESHOT_REMOVE_END);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SCAN_ONESHOT_REMOVE_STOP);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SCAN_ONESHOT_REMOVE_FAIL);

		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SCAN_ETERNAL_START);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SCAN_ETERNAL_END);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SCAN_ETERNAL_STOP);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SCAN_ETERNAL_FAIL);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SCAN_ETERNAL_ONECYCLE_START);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SCAN_ETERNAL_ONECYCLE_END);

		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SCAN_ETERNAL_REMOVE_START);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SCAN_ETERNAL_REMOVE_END);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SCAN_ETERNAL_REMOVE_STOP);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SCAN_ETERNAL_REMOVE_FAIL);

		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SCAN_STOP_START);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_SCAN_STOP_END);

		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_REMOVEDIR_START);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_REMOVEDIR_END);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_REMOVEDIR_FAIL);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_REMOVEDIR_ITEMINFO);

		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_DEVICEAUTH_INIT_START);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_DEVICEAUTH_INIT_END);

		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_DEVICEAUTH_INSERT_START);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_DEVICEAUTH_INSERT_END);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_DEVICEAUTH_INSERT_FAIL);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_DEVICEAUTH_INSERT_INFO_ETH);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_DEVICEAUTH_INSERT_INFO_DEVICE_HEADER);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_DEVICEAUTH_INSERT_INFO_DEVICE_UDN);

		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_DEVICEAUTH_STATUS_ACCESS_ALLOW);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_DEVICEAUTH_STATUS_ACCESS_DENY);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_DEVICEAUTH_STATUS_CONNECTIONCHECK_START);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_DEVICEAUTH_STATUS_CONNECTIONCHECK_END);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_DEVICEAUTH_STATUS_CONNECTIONCHECK_PING_START);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_DEVICEAUTH_STATUS_CONNECTIONCHECK_PING_END);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_DEVICEAUTH_STATUS_CONNECTIONCHECK_PING_CONNECT);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_DEVICEAUTH_STATUS_CONNECTIONCHECK_PING_DISCONNECT);

		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_DEVICEAUTH_DELETE_START);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_DEVICEAUTH_DELETE_END);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_DEVICEAUTH_DELETE_FAIL);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_DEVICEAUTH_DELETE_INFO_ETH);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_DEVICEAUTH_DELETE_INFO_DEVICE_HEADER);
		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_DEVICEAUTH_DELETE_INFO_DEVICE_UDN);

		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_STREAMMING_SEND);

		ENUM_TO_STR(MXDLNADMS_CALLSTATUS_UPLOAD_POSTSAVE);

		default:
			break;
	}

	return ( "Unknown status" );
}

STATIC void __pldms_CbDmsStatus( MXDLNADMS_CALL_STATUS call_status, void * pValue )
{
	PL_DMS_Context_t *pContext = __pl_dms_GetContext();

	HxLOG_Print(HxANSI_COLOR_YELLOW("[PL_DMS] DMS Status ( %s )\n"), __pldms_StatusToStr( call_status ) );
	switch (call_status)
	{
		case MXDLNADMS_CALLSTATUS_SCAN_ONESHOT_START:
		case MXDLNADMS_CALLSTATUS_SCAN_ONESHOT_END:
		case MXDLNADMS_CALLSTATUS_SCAN_ONESHOT_STOP:
		case MXDLNADMS_CALLSTATUS_SCAN_ETERNAL_ONECYCLE_START:
			break;

		/*  아래 두 이벤트는 위로 올라가야 함.  */
		case MXDLNADMS_CALLSTATUS_SCAN_ETERNAL_ONECYCLE_END:
			if (pContext->tStatusEvent_cb) pContext->tStatusEvent_cb(PL_DMS_StatusEvent_StackScanOneCycleEnd, 0x0, 0x0, 0x0, 0x0);
			break;

		case MXDLNADMS_CALLSTATUS_STREAMMING_SEND:
			if (pContext->tStatusEvent_cb) pContext->tStatusEvent_cb(PL_DMS_StatusEvent_StackStreamingSend, 0x0, 0x0, 0x0, 0x0);
			break;

		default:
			break;
	}
}

// 사용자가 MXDLNA_DMS_VIRTUAL_MULTIRESOURCE 구조체에 등록하고, 라이브러리에서는 Browse() or Search()시
// 특정 MXDLNA_CDSOBJECT에 대해서, 사용자가 정의한 가상 리소스로 보여줄지 여부를 판단한다.
// 리턴값이 0이면 생성하지 않는다, 1이면 생성한다.
STATIC HINT32 __pldms_CbIsVirtualRes( MXDLNA_CDSOBJECT *pObject, void *pVMR )
{
	HINT32 nRet = 0;
	MXDLNA_DMS_VIRTUAL_MULTIRESOURCE *pMultiRes = NULL;

	if(pObject == NULL || pVMR == NULL )
		return nRet;

	pMultiRes = ( MXDLNA_DMS_VIRTUAL_MULTIRESOURCE * )pVMR;

	if(pObject->res && pObject->res->Value)
	{
		/* ios bringin issue under NTLS */
		if( HLIB_STD_StrNCaseCmp( pObject->res->Value, pMultiRes->WebDelimiter, pMultiRes->szDelimiter ) == 0 )
			nRet = 1;
	}

	return nRet;
}

// MediaTransport에서 File을 Open하기 위한 Path를 DirectiveObj를 이용하여 생성한다.
STATIC HINT32 __pldms_CbChangeVitualURL( HCHAR *pDirectiveObj, HCHAR **ppNewURL )
{
	HINT32 retCode = 0;

	if( pDirectiveObj && ppNewURL )
	{
		/* ios bringin issue under NTLS */
		if( HLIB_STD_StrNCaseCmp( pDirectiveObj, PL_DMS_DEFAULT_CHANNEL_PREFIX, HxSTD_StrLen( PL_DMS_DEFAULT_CHANNEL_PREFIX ) ) == 0 )
		{
			/* 아래 할당된 ppNewURL은 lib 내부적으로 해제 함 */
			*ppNewURL = HLIB_STD_StrDup( pDirectiveObj + 6 );
			retCode = 1;
		}
	}

	HxLOG_Print( "[ PL_DMS ] CbChangeVirtualURL( %s ) - Ret( %d )\n", pDirectiveObj, retCode );

	if(ppNewURL != NULL)
	{
		if( *ppNewURL )
			HxLOG_Print( "[ PL_DMS ] NewURL( %s )\n", *ppNewURL );
	}

	return retCode;
}

/*-----------------------------------------------------------------------------
 * Media Server가 내부적으로 사용하는 Callback
 * 해당 액션들은 현재 서비스 되지 않음으로, 미구현
 *-----------------------------------------------------------------------------*/
/* UploadObject() /CreateObject() Action에 의해서 아이템이 생성될 때 */
MXDMS_USER_CALLBACK_RET __pldms_CB_FileUploadDone(
		MXDMS_UPLOADTYPE uploadType,
		const HCHAR * tmpFilename,
		const HCHAR * titlePath,
		const HCHAR * parent_phy_location,
		MXDLNA_CDSOBJECT * pObject,
		HCHAR ** savedFilename
		)
{
	HxLOG_Print( "[ PL_DMS ] Called Callback - %s\n", __FUNCTION__ );
	return MXDMS_USER_CALLBACK_SUCCESS;
}

/* DestroyObject() Action에 의해서 아이템이 제거될 때 */
MXDMS_USER_CALLBACK_RET __pldms_CB_FileDestroyDone (
		MXDMS_DESTROYTYPE destroyType,
		const HCHAR * tmpFilename,
		const HCHAR * titlePath,
		const HCHAR * parent_phy_location,
		MXDLNA_CDSOBJECT * pObject,
		HINT32 refCount
		)
{
	HxLOG_Print( "[ PL_DMS ] Called Callback - %s\n", __FUNCTION__ );
	return MXDMS_USER_CALLBACK_SUCCESS;
}

/* MoveObject() Action에 의해서 아이템이 이동될 때 */
MXDMS_USER_CALLBACK_RET __pldms_CB_FileMoveObject (
		const MXDLNA_CDSOBJECT * pCurrObject,
		const MXDLNA_CDSOBJECT * pNewParentObject,
		const HINT32 isFolder,
		const HCHAR * currentCdsTitlePath_APP,
		const HCHAR * currentRealLocation_APP,
		const HCHAR * newParentCdsTitlePath_APP,
		const HCHAR * newParentRealLocation_APP,
		HCHAR ** ppNewRealLocation_UTF8
		)
{
	HxLOG_Print( "[ PL_DMS ] Called Callback - %s\n", __FUNCTION__ );
	return MXDMS_USER_CALLBACK_SUCCESS;
}

/* UpdateObject() Action에 의해서  아이템이 변경될 때 */
MXDMS_USER_CALLBACK_RET __pldms_CB_FileUpdateObject (
		const MXDLNA_CDSOBJECT * pObject,
		const HINT32 isFolder,
		const HCHAR * currentCdsTitlePath_APP,
		const HCHAR * currentRealLocation_APP,
		const HCHAR * currTitle_APP,
		const HCHAR * newTitle_APP,
		HCHAR ** ppNewRealLocation_UTF8
		)
{
	HxLOG_Print( "[ PL_DMS ] Called Callback - %s\n", __FUNCTION__ );
	return MXDMS_USER_CALLBACK_SUCCESS;
}

/** MediaTransport에서 http://ipaddr.port/web/cmd/startTranscoding?id=123 과 같이
 * web/cmd 아래에 들어온 명령을 Application Callback에 전달한다.
 * pInUrlParam 		: callback에서 넘어오는 web/cmd 다음 부터의 값들.
 * pOutHttpRespVal 	: HTTP의 응답값으로서, 200 인 경우, 리턴값을 함께 포함해서 응답. 200 이 아닌경우 *pOutHttpRespVal만 응답에 사용
 * pbFreeReturnVal 	: Return 값을 Lib에서 Free해야 하는지 명시적으로 선언. 1이면 Lib이 Free해야 한다.
 * Return 			: HTTP의 응답에 포함시킬 데이터 */
STATIC HCHAR* __pldms_CbCmdProcess(	HCHAR *pInUrlParam,	HINT32 *pOutHttpRespVal, HINT32 *pbFreeReturnVal )
{
	if( pInUrlParam )
	{
		HxLOG_Print( "[ PL_DMS ] ######################################################\n" );
		HxLOG_Print( "[ PL_DMS ] Media Transport CMD( %s )\n", pInUrlParam );
		HxLOG_Print( "[ PL_DMS ] ######################################################\n" );
	}

	// 메모리 해제할 필요 없다
	*pbFreeReturnVal = 0;

	Sleep(100);
	// 성공 시
	*pOutHttpRespVal = 200;	// 200 OK
	return "OK";

	// 실패 시
	//*pOutHttpRespVal = 500;	// 500 Invalid Request
	//return "Invalid Request";
}

HERROR	__pldms_systemfolder_init( void )
{
	PL_DMS_SysFolderArg_t	stSFArgs = { 0 };
	PL_DMS_Context_t 	*pContext = __pl_dms_GetContext();
	SFM_HANDLE	hRootFS = NULL;
	MXDLNA_DMS_SYSTEMFOLDER_PROPERTY	stSFProperty = { 0, };

	stSFArgs.pRootFolderTitle 	= PL_DMS_DEFAULT_SF_ROOTTITLE;
	stSFArgs.pUnKnown			= PL_DMS_DEFAULT_SF_UNKNOWN;
	stSFArgs.pUnKnownYear		= PL_DMS_DEFAULT_SF_UNKNOWN;
	stSFArgs.pUnKnownDate		= PL_DMS_DEFAULT_SF_UNKNOWN;
	stSFArgs.pUnKnownMonth		= PL_DMS_DEFAULT_SF_UNKNOWN;
	stSFArgs.pUnKnownDay		= PL_DMS_DEFAULT_SF_UNKNOWN;

	if( pContext->hDms )
	{
		/* 최상위 Root SystemFolder의 Handle 획득 */
		hRootFS = mxDlnaDmsSfm_getRootSystemFolder( pContext->hDms, 1, 1);

		/* UnKnown에 해당하는 기본값 설정 */
		(void)mxDlnaDmsSfm_setUnknownValues((const HCHAR *)stSFArgs.pUnKnown,
											(const HCHAR *)stSFArgs.pUnKnownYear,
											(const HCHAR *)stSFArgs.pUnKnownDate,
											(const HCHAR *)stSFArgs.pUnKnownMonth,
											(const HCHAR *)stSFArgs.pUnKnownDay);

		/* FolderProperty값을 채워줌 */
		// 다른 DMP 에서의 삭제를 금지, search 허용한다. isRestricted 값이 1 이면 다른 DMP 에서의 삭제 불가
		mxDlnaDmsSfm_setFolderProperty( &stSFProperty, SYSTEMTYPE_REALFOLDER_PARENT, MEDIATYPE_ALL, 1, 1 );

		/* RootFS 하위에 SystemFolder를 생성 */
		(void)mxDlnaDmsSfm_makeSystemFolder( pContext->hDms, hRootFS, stSFArgs.pRootFolderTitle, stSFProperty );

		/* SubSystemFolder에 추가되는 모든 Contents를 포함하는 폴더를 자동으로 생성할지 여부 */
		mxDlnaDmsSfm_setAllFolder( 1, "All Files" );

		mxDlnaDmsSfm_printAllSystemFolder();
	}

	return ERR_FAIL;
}

HINT32		__pldms_resource_config( MXDLNA_DMS_SERVER_FUNC_CONFIG *pstConfig )
{
	PL_DMS_Context_t 			*pContext = __pl_dms_GetContext();
	PL_DMS_Option_t 			*pDmsOption = NULL;
	MXDLNA_DMS_USER_DEVICECAP 	*pUserDevCap = NULL;
	HBOOL bEnabledNTLS = FALSE, bEnabledM3U8 = FALSE;
	HUINT32	ulStreamingFlags = ePL_DMS_StreamingConfig_None; /* PL_DMS_StreamingRes_e */
	HERROR	hErr = ERR_FAIL;
	PL_COMMON_MacInfo_t stMacInfo;
	HCHAR	szPairingId[PL_DMS_UDN_MAX] = {0,};

#if defined(CONFIG_NON_TRANSCODING_STREAMING)
	ulStreamingFlags |= ePL_DMS_StreamingConfig_Tuner_NTLS; // No-meaning for (flags & ePL_DMS_StreamingConfig_Tuner_NTLS)
	bEnabledNTLS = TRUE;
#endif
#if defined(CONFIG_TRANSCODING_STREAMING)
	ulStreamingFlags |= ePL_DMS_StreamingConfig_Tuner_HLS; // No-meaning for (flags & ePL_DMS_StreamingConfig_Tuner_HLS)
	bEnabledM3U8 = TRUE;
#endif

	/* Humax Capability 기술 */
	pUserDevCap = HLIB_STD_MemCalloc(sizeof(MXDLNA_DMS_USER_DEVICECAP));
	if( pUserDevCap )
	{
		pstConfig->pUserDeviceCapability = pUserDevCap;
		pUserDevCap->NamespacePrefix	= "hmx";
		pUserDevCap->Namespace			= "urn:schemas-humax-co-kr:metadata-1-0";
		pUserDevCap->AttributeName		= "X_HMXCAP";	/*  일반적으로 DLNA에서 User 정의에 대한 값들앞에 X_를 붙임. */

		/*
		 * Added DMS's humax capability for WOON usage and 3G restriction and WoonID
		 *   If there is no WOON server                                             : HX_N_WOON
		 *   If there is WOON server (DMS + WOON)
		 *     : If WOON 3G restriction for live channel streaming is existed       : HX_N_LIVE_P (p: play stands)
		 *     : If WOON 3G restriction for PVR record file streaming is existed    : HX_N_REC_P
		 *     : If WOON 3G restriction for general media file streaming is existed : HX_N_GNRL_P (general stands)
		 *     : If WOON 3G restriction for all contents onto the outside network   : HX_N_ALL_P
		 *     : If there is no woonID usage                                        : HX_N_WOONID
		 */
#if defined(CONFIG_PROD_HMS1000T)
		/*
		 * If there is restriction on WOON 3G streaming under DMS+WOON server
		 *   e.g. HMS-1000T is limiting external contents streaming.
		 */
		pUserDevCap->Value				= "TSR,MULTIVALUE,RCUTYPE_LIME,HX_N_ALL_P";
#elif defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
		/*
		 *
		 * If there is no restriction on WOON 3G streaming
		 *   e.g. HMS-1000S/EU allow external contents (Live channel, PVR record, Media file) streaming.
		 */
		pUserDevCap->Value				= "TSR,MULTIVALUE,RCUTYPE_LIME"; /* Default value under WOON usage */
#elif defined(CONFIG_PROD_IR4000HD)
		/*
		 *
		 * no WOON server / no transcoding streaming / no SAT>IP
		 *
		 */
		pUserDevCap->Value				= "TSR,MULTIVALUE,RCUTYPE_LIME,HX_N_WOON,HX_N_TRANS,HX_N_SATIP,HX_TUNER:1";
#elif defined(CONFIG_PROD_IR4100HD)
		/*
		 *
		 * no WOON server / no transcoding streaming / no SAT>IP
		 *
		 */
		pUserDevCap->Value				= "TSR,MULTIVALUE,RCUTYPE_LIME,HX_N_WOON,HX_N_TRANS,HX_N_SATIP,HX_TUNER:1";
#elif defined(CONFIG_PROD_YSR2000)
		/*
		 *
		 * no WOON server / no transcoding streaming / no SAT>IP
		 *
		 */
		pUserDevCap->Value				= "TSR,MULTIVALUE,RCUTYPE_LIME,HX_N_WOON,HX_N_TRANS,HX_N_SATIP,HX_TUNER:1";
#elif defined(CONFIG_PROD_FVP4000T)
		/*
		 *
		 * no WOON server / no transcoding streaming / no SAT>IP / no EPG / Tuner 3개
		 *
		 */
		pUserDevCap->Value				= "TSR,MULTIVALUE,RCUTYPE_LIME,HX_N_WOON,HX_N_TRANS,HX_N_SATIP,HX_TUNER:3";
#elif defined(CONFIG_PROD_ICORDPRO) || defined(CONFIG_PROD_HDNEO)
		/*
		 *
		 * no WOON server / no transcoding streaming
		 *
		 */
		pUserDevCap->Value				= "TSR,MULTIVALUE,RCUTYPE_LIME,HX_N_WOON,HX_N_TRANS,HX_N_LINK_ALL,HX_TUNER:1";
#elif defined(CONFIG_PROD_HGS1000S)
		/*
		 *
		 * no WOON server /
		 *
		 */
		 pUserDevCap->Value				= "TSR,MULTIVALUE,RCUTYPE_LIME,HX_N_WOON,HX_N_LINK_ALL,HX_TUNER:4";
#elif defined(CONFIG_PROD_HGS1000SIT)
		/*
		 *
		 * no WOON server /
		 *
		 */
		 pUserDevCap->Value				= "TSR,MULTIVALUE,RCUTYPE_LIME,HX_N_WOON,HX_N_LINK_ALL,HX_N_TRANS,HX_N_SATIP,HX_TUNER:4";
#else
#if !defined(CONFIG_HOMMA_WOON)
		/*
		 * If there is no WOON server and also LiveTV, MediaPlayer scenario is unavailable,
		 * record WOON 3G streaming restriction("HX_N_ALL_P") on DMS's humax capability
		 */
		pUserDevCap->Value				= "TSR,MULTIVALUE,RCUTYPE_LIME,HX_N_WOON";
#else
		/*
		 * [Default]
		 * If there is no restriction on WOON 3G streaming
		 *   e.g. HMS-1000S/EU
		 */
		pUserDevCap->Value				= "TSR,MULTIVALUE,RCUTYPE_LIME"; /* Default value under WOON usage */
#endif
#endif

/* WOON 기능을 지원하지 않는 DMS이어도 LiveTV app과의 호환성을 위하여 별도로 추가 */
#if !defined(CONFIG_HOMMA_WOON)
		hErr = PL_COMMON_GetEthMac(&stMacInfo);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("[PL_WOONS][%s] Error> Cant get mac addr(err:%d)!\n", __FUNCTION__, hErr);
			HxSTD_PrintToStrN(szPairingId, sizeof(szPairingId), "%s", "hmx_nomac");
			/* return FALSE; */
		}
		else
		{
			HxSTD_PrintToStrN(szPairingId, sizeof(szPairingId), "hmx_base_%s", stMacInfo.szMacAddr);
		}

/* 2016/1/19 mhkang:  http://svn:3000/issues/109992
	HGS1000S에서는 Humax LiveTV앱 Humax MediaPlayer앱 연동하지 않음.
*/
#ifndef CONFIG_PROD_HGS1000S
		pUserDevCap->next = HLIB_STD_MemCalloc(sizeof(MXDLNA_DMS_USER_DEVICECAP));
		pUserDevCap = pUserDevCap->next;
		pUserDevCap->NamespacePrefix	= "icube";
		pUserDevCap->Namespace			= "urn:schemas-icube-co-kr:WoonDevice-1-0";
		pUserDevCap->AttributeName		= "X_WOON_PAIRING";
		pUserDevCap->Value				= (const char *)szPairingId;
#endif

#endif

	}

	/* Multi-Resource 관련 설정 */
	pstConfig->pUserSpecificMultiResource = HLIB_STD_MemCalloc( sizeof( MXDLNA_DMS_VIRTUAL_MULTIRESOURCE ) );
	if( pstConfig->pUserSpecificMultiResource )
	{
		pstConfig->pUserSpecificMultiResource->WebDelimiter 				= PL_DMS_DEFAULT_CHANNEL_PREFIX;
		pstConfig->pUserSpecificMultiResource->szDelimiter 					= HxSTD_StrLen( PL_DMS_DEFAULT_CHANNEL_PREFIX );
		pstConfig->pUserSpecificMultiResource->fnBeNeedVirtualMultiResource = __pldms_CbIsVirtualRes;
		pstConfig->pUserSpecificMultiResource->fnChangeVirtualURL 			= __pldms_CbChangeVitualURL;

		if(pDmsOption)
		{
			pDmsOption->pstMultiRes = ( PL_DMS_VIRTUAL_MULTIRESOURCE_ITEM_t * )HLIB_STD_MemCalloc( sizeof( PL_DMS_VIRTUAL_MULTIRESOURCE_ITEM_t ));
			pstConfig->pUserSpecificMultiResource->pTblOfVirtualMultiResource = ( MXDLNA_DMS_VIRTUAL_MULTIRESOURCE_ITEM * )pDmsOption->pstMultiRes;
		}
	}

	/* Media Transport 관련 설정 */
	pstConfig->pUserMediaTransport = HLIB_STD_MemCalloc( sizeof( MXDLNA_DMS_MEDIATRANSPORT_DATA ) );
	if( pstConfig->pUserMediaTransport )
	{
		pstConfig->pUserMediaTransport->sizeOfMediaTransBuffer 	= PL_DMS_TRANSBUFFER_SIZE_DEFAULT;
		pstConfig->pUserMediaTransport->fnUserCmdProcess 		= __pldms_CbCmdProcess;
	}

	/* HLS 일 경우, 원본 URI를 제거하고, .m3u8만 제공 */
	//pstConfig->nUserM3U8Position = eMP_Remove_Origin;
	/*
#if defined(CONFIG_NON_TRANSCODING_STREAMING)
	pstConfig->nUserM3U8PositionforTuner = eMP_Normal;
#else
	pstConfig->nUserM3U8PositionforTuner = eMP_Remove_Origin;
#endif
	*/
/*--------------------------------
 eMP_Normal: 모델에 따라 m3u8를 앞 또는 뒤에 추가한다.
 eMP_NotUse: m3u8 을 사용하지 않는다.
 eMP_Remove_Origin: ntls를 사용하지 않는다.
 eMP_Apply_Item_ExConfig: 아이템마다 m3u8 또는 ntls를 설정할 수  있다.
---------------------------------*/
	if ((bEnabledNTLS == TRUE) && (bEnabledM3U8 == TRUE))
	{
		pstConfig->nUserM3U8PositionforTuner = eMP_Normal;
		pstConfig->nUserM3U8PositionforMedia = eMP_Normal;
	}
	else if ((bEnabledNTLS == TRUE) && (bEnabledM3U8 == FALSE))
	{
		pstConfig->nUserM3U8PositionforTuner = eMP_NotUse;
		pstConfig->nUserM3U8PositionforMedia = eMP_NotUse;
	}
	else if ((bEnabledNTLS == FALSE) && (bEnabledM3U8 == TRUE))
	{
		pstConfig->nUserM3U8PositionforTuner = eMP_Remove_Origin;
		pstConfig->nUserM3U8PositionforMedia = eMP_Remove_Origin;
	}
	else
	{
		pstConfig->nUserM3U8PositionforTuner = eMP_Apply_Item_ExConfig;
		pstConfig->nUserM3U8PositionforMedia = eMP_Apply_Item_ExConfig;
	}
/* ToDo: 일반적인 설정은 위와 같으나, 모델에 마다 다르게 설정 할 경우 아래에 ifdef로 추가 해준다. */

	mxDlnaDms_configure( pContext->hDms, pstConfig );

	/*  free  */
	if( pstConfig->pUserDeviceCapability )
	{
		MXDLNA_DMS_USER_DEVICECAP * pUserDevCapNext = NULL;
		pUserDevCap = pstConfig->pUserDeviceCapability;
		pUserDevCapNext = pUserDevCap->next;
		HOM_MEMFREE(pUserDevCap);
		while(pUserDevCapNext)
		{
			pUserDevCap = pUserDevCapNext;
			pUserDevCapNext = pUserDevCapNext->next;
			HOM_MEMFREE(pUserDevCap);
		}
	}

	HOM_MEMFREE( pstConfig->pUserSpecificMultiResource );
	HOM_MEMFREE( pstConfig->pUserMediaTransport );

	return 0;
}

HERROR	__pldms_convert_dmsInfoToLib( PL_DMS_Context_t *pstContext, MXDLNA_DMS_SERVER_INFO *pstMxDmsInfo )
{
	PL_DMS_Config_t *pExtConfig = NULL;
	PL_DMS_Server_t *pIntConfig = NULL;

	pExtConfig = &pstContext->stExtConfig;
	pIntConfig = &pstContext->stIntConfig;

	pstMxDmsInfo->FriendlyName 				= pExtConfig->szFriendlyName;
	pstMxDmsInfo->UDN 						= pExtConfig->szUDN;

	pstMxDmsInfo->SerialNumber				= pIntConfig->SerialNumber;
	pstMxDmsInfo->SortCapabilities			= pIntConfig->SortCapabilities;
	pstMxDmsInfo->SearchCapabilities		= pIntConfig->SearchCapabilities;
	pstMxDmsInfo->SourceProtocolInfo		= pIntConfig->SourceProtocolInfo;
	pstMxDmsInfo->FeatureList				= pIntConfig->FeatureList;

	pstMxDmsInfo->manufacturer				= pIntConfig->manufacturer;
	pstMxDmsInfo->manufacturerURL			= pIntConfig->manufacturerURL;
	pstMxDmsInfo->ModelName					= pExtConfig->szModelName;
	pstMxDmsInfo->ModelDescription			= pExtConfig->szModelDescription;
	pstMxDmsInfo->ModelURL					= pExtConfig->szModelURL;

	pstMxDmsInfo->defaultCreator			= pIntConfig->defaultCreator;
	pstMxDmsInfo->Corporation				= pIntConfig->Corporation;
	pstMxDmsInfo->ModelNumber				= pExtConfig->szModelNumber;
	pstMxDmsInfo->productCode				= pIntConfig->productCode;

	pstMxDmsInfo->defaultSortCapabilities	= pIntConfig->defaultSortCapabilities;
	pstMxDmsInfo->uploadPath				= pIntConfig->uploadPath;
	pstMxDmsInfo->tmpUploadPath				= pIntConfig->tmpUploadPath;
	pstMxDmsInfo->thumbnailPath				= pIntConfig->thumbnailPath;

	pstMxDmsInfo->upnp_port					= pExtConfig->upnp_port;
	pstMxDmsInfo->webserver_port			= pExtConfig->webserver_port;
	pstMxDmsInfo->upnp_dtcp_port			= pExtConfig->upnp_dtcp_port;
	pstMxDmsInfo->uploadserver_port			= pExtConfig->uploadserver_port;

	pstMxDmsInfo->fnDMSStatus				= ( MXDLNA_DMSSTATUS_CALLBACK )__pldms_CbDmsStatus;
#if defined(CONFIG_HOMMA_WOON)
	pstMxDmsInfo->Pairing 					= PL_WOONS_GetPairingId();
	HxLOG_Error("[PL_DMS] WOON Pairing [%s]\n", pstMxDmsInfo->Pairing);
#endif

	return ERR_OK;
}

HERROR	__pl_dms_regist_icon(void)
{
	HINT32	nRet = 0;
	PL_DMS_Context_t *pContext = __pl_dms_GetContext();
	HINT32 i = 0, nCnt = sizeof( s_stDmsIcon ) / sizeof( s_stDmsIcon[0] );

	for( i = 0 ; i < nCnt; ++i )
	{
		nRet = mxDlnaDms_addDmsIcon( pContext->hDms, ( MXDLNA_DMS_ICON * )&(s_stDmsIcon[i]) );
		HxLOG_Print( "[ PL_DMS ] Add Icon - %d( ret = %d )\n", i, nRet );
	}

	return ERR_OK;
}

#define ________________Public_Interfaces__________________________

PL_DMS_Result_e		PL_DMS_Init( const PL_DMS_Config_t *pDmsConfig )
{
	HERROR hErr = ERR_FAIL;
	HERROR bAddIP = TRUE; /* This is scenario for HMS based models */
	HERROR bValidIP = FALSE;
	PL_COMMON_ProductInfo_t product_info;
	PL_COMMON_NetworkInfo_t	net_info;
	PL_COMMON_MacInfo_t	mac_info;
	PL_DMS_Context_t *pContext = __pl_dms_GetContext();

	if ((pContext == NULL ) || (pDmsConfig == NULL))
	{
		HxLOG_Error("[PL_DMS] Error> invalid dms ctx or cfg!\n");
		return ePL_DMS_Result_Error;
	}

	if( pContext->is_init == FALSE )
	{
		PL_DMS_Server_t *pIntConfig = &pContext->stIntConfig;
		PL_DMS_Config_t *pExtConfig = &pContext->stExtConfig;

#if defined(CONFIG_HOMMA_CDI)
		if(PL_DTCP_Init() != eHOM_Result_Ok)
		{
			HxLOG_Error("[%s][%d] PL_DTCP_Init() Error!!\n", __FUNCTION__, __LINE__);
		}
#endif
		HxLOG_Debug("[PL_DMS] initialize DMS context!\n");

		HxSTD_MemSet( pContext, 0x00, sizeof( PL_DMS_Context_t ) );
		HxSTD_MemSet( pIntConfig, 0x00, sizeof( PL_DMS_Server_t ) );
		HxSTD_MemSet( pExtConfig, 0x00, sizeof( PL_DMS_Config_t ) );

		HxSTD_MemSet( &net_info, 0x00, sizeof( PL_COMMON_NetworkInfo_t ) );

		/* PL_DMS_Config_t */
		{
			HxSTD_MemSet( &product_info, 0x00, sizeof( PL_COMMON_ProductInfo_t ) );
			hErr = PL_COMMON_GetProductInfo(&product_info);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("[PL_DMS] Error> Cant get productInfo!\n");
			}

			if (HxSTD_StrEmpty(pDmsConfig->szFriendlyName))
			{
				if ((bAddIP == TRUE) && (PL_COMMON_GetActiveNetwork(&net_info) == ERR_OK)) /* ethernet and wifi */
				{
					if ((net_info.ulIpAddr != 0x0) && !HxSTD_StrEmpty(net_info.szIp))
					{
						bValidIP = TRUE;
					}
					else
					{
						hErr = PL_COMMON_GetDefaultNetwork(&net_info);
						if (hErr != ERR_OK)
						{
							HxLOG_Error("[PL_DMS] Error> Cant get default network!\n");
						}

						if ((net_info.ulIpAddr != 0x0) && !HxSTD_StrEmpty(net_info.szIp))
						{
							bValidIP = TRUE;
						}
						else
						{
							HxLOG_Error("[PL_DMS] Error> [devIdx    :%d]\n", net_info.nDevIdx);
							HxLOG_Error("[PL_DMS] Error> [Valid     :%d]\n", net_info.bValid);
							HxLOG_Error("[PL_DMS] Error> [Connected :%d]\n", net_info.bConnected);
							HxLOG_Error("[PL_DMS] Error> [Active    :%d]\n", net_info.bActive);
							HxLOG_Error("[PL_DMS] Error> [IpAddr    :0x%x]\n", net_info.ulIpAddr);
							HxLOG_Error("[PL_DMS] Error> [Ip        :%s]\n", net_info.szIp);
						}
					}
				}
				HxLOG_Print("[PL_DMS] IP:0x%x,%s\n", (unsigned int)net_info.ulIpAddr, net_info.szIp);

				if ((bAddIP == TRUE) && (bValidIP == TRUE))
				{
					HxSTD_snprintf( pExtConfig->szFriendlyName, sizeof(pExtConfig->szFriendlyName)-1, "%s DMS %s", product_info.szModelName, net_info.szIp );
				}
				else
				{
					HxSTD_snprintf( pExtConfig->szFriendlyName, sizeof(pExtConfig->szFriendlyName)-1, "%s DMS", product_info.szModelName);
				}
			}
			else
			{
				HxSTD_snprintf( pExtConfig->szFriendlyName, sizeof(pExtConfig->szFriendlyName)-1, "%s", pDmsConfig->szFriendlyName );
			}
			HxLOG_Error( "[ PL_DMS ] friendly name:%s\n", pExtConfig->szFriendlyName);

			/* UUID */
			if ((net_info.bValid == TRUE) && !HxSTD_StrEmpty(net_info.szMacAddr))
			{
				HxSTD_snprintf( pExtConfig->szUDN, sizeof(pExtConfig->szUDN), "%s%s", PL_DMS_DEFAULT_UDN_PREFIX, net_info.szMacAddr);
			}
			else
			{
				HxSTD_MemSet( &mac_info, 0x0, sizeof(mac_info) );
				hErr = PL_COMMON_GetEthMac(&mac_info);
				if (hErr != ERR_OK)
				{
					HxLOG_Error("[PL_DMS] Critical/Error> Cant get ethernet mac(%s, err:%d)!\n", mac_info.szMacAddr, hErr);
					/* It's situation that should not occur absolutely.  */
					if (1)
					{
						/* Dont use this; It can't be ensured that the "Default" is same with "ethernet". */
						HxSTD_MemSet(&mac_info, 0x0, sizeof(PL_COMMON_MacInfo_t));
						hErr = PL_COMMON_GetDefaultMac(&mac_info);
						if (hErr != ERR_OK)
						{
							HxLOG_Error("[PL_DMS] Error> Cant get default mac(%s)!\n", mac_info.szMacAddr);
						}
					}
				}
				HxSTD_snprintf( pExtConfig->szUDN, sizeof(pExtConfig->szUDN), "%s%s", PL_DMS_DEFAULT_UDN_PREFIX, mac_info.szMacAddr);
			}
			HxLOG_Print( "[ PL_DMS ] UDN:%s\n", pExtConfig->szUDN);
			HxSTD_snprintf( pExtConfig->szModelName, sizeof(pExtConfig->szModelName), "%s", product_info.szModelName);
			HxSTD_snprintf( pExtConfig->szModelDescription, sizeof(pExtConfig->szModelDescription), "%s", product_info.szModelDescription);
			HxSTD_snprintf( pExtConfig->szModelURL, sizeof(pExtConfig->szModelURL), "%s", product_info.szModelUrl );
			HxSTD_snprintf( pExtConfig->szModelNumber, sizeof(pExtConfig->szModelNumber), "%s", product_info.szModelNumber );

			pExtConfig->upnp_port 			= PL_DMS_DEFAULT_PORT_UPNP;
			pExtConfig->webserver_port 		= PL_DMS_DEFAULT_PORT_WEBSERVER;
			pExtConfig->upnp_dtcp_port 		= PL_DMS_DEFAULT_PORT_DTCP;
			pExtConfig->uploadserver_port 	= PL_DMS_DEFAULT_PORT_UPLOAD;
		}

		/* PL_DMS_Server_t */
		{
			pIntConfig->SortCapabilities 	= (HCHAR *)mxDlnaDms_GetSortCapabilities();
			pIntConfig->SearchCapabilities 	= (HCHAR *)mxDlnaDms_GetSearchCapabilities();

			pIntConfig->SourceProtocolInfo 	= NULL;
			pIntConfig->FeatureList = "";
			/*
			 * 서버가 지원하는 Feature들에 대한 리스트를 XML 문서형태로 지정.
			 FeatureList	= "http-get:*:audio/mpeg:*,"
			 "http-get:*:audio/wav:*,"
			 "http-get:*:audio/mp4:*,"
			 "http-get:*:image/jpeg:*,"
			 "http-get:*:video/mpeg:*,"
			 "http-get:*:video/avi:*,"
			 "http-get:*:text/plain:*,"
			 "http-get:*:video/ts:*";
			 */

			pIntConfig->SerialNumber 	= NULL;
			pIntConfig->manufacturer 	= "HUMAX";
			pIntConfig->manufacturerURL = "www.humaxdigital.com";

			pIntConfig->defaultCreator 	= "HUMAX";
			pIntConfig->Corporation 	= "HUMAX";
			pIntConfig->productCode 	= NULL;

			pIntConfig->defaultSortCapabilities = "upnp:class";

			pIntConfig->uploadPath		= PL_DMS_UPLOAD_PATH;
			pIntConfig->tmpUploadPath 	= PL_DMS_UPLOAD_TEMP_PATH;
			pIntConfig->thumbnailPath 	= PL_DMS_DEFAULT_THUMBNAIL_PATH;

#if defined(CONFIG_HOMMA_WOON)
			/* Set pairingID here(DMS initializing), not WOON initializing */
			PL_WOONS_InitPairingId();

			/* Don't call mxRas_getPairingID() before DMS initializing */
			/* PL_WOONS_GetPairingId() is done at __pldms_convert_dmsInfoToLib() */
#endif
		}

		pContext->hDms              = NULL;
		pContext->hDmsDB            = NULL;
		pContext->hFSS              = NULL;
		pContext->usUseTransfer     = 0;

		PL_DMS_ClearTunerEpg();
		pContext->pChannelGroupList = NULL; /* Unused pChannelGroupList var after divided to pl_dms_scan_epg */

		pContext->eStatus           = ePL_DMS_Status_Stopped;
		pContext->is_stopping       = FALSE;
		pContext->is_init           = TRUE;
	}
	else
	{
		HxLOG_Error("[PL_DMS] Already initialized!\n");
	}

	return ePL_DMS_Result_Ok;
}

PL_DMS_Result_e		PL_DMS_DeInit( void )
{
	return ePL_DMS_Result_Ok;
}

HERROR	PL_DMS_Start( void )
{
	PL_DMS_Context_t 		*pContext = __pl_dms_GetContext();
	MXDBC_DB_INFO 			*pstDbInfo = NULL;
	MXDLNA_DMS_SERVER_INFO			stMxDmsInfo = { 0 };
	MXDLNA_FSS_SCANOPTION			stScanOption = { 0, };
	MXDLNA_DMS_SERVER_FUNC_CONFIG 	stConfig = { 0 };
	HINT32					nRet = 0;
	unsigned char crashed = 0;
	HxLOG_Error("[ PL_DMS ] (%s)++\n", __FUNCTION__);

	pContext->eStatus           = ePL_DMS_Status_Stopped;

	/* DMS DB 설정 및 생성 */
	pstDbInfo = ( MXDBC_DB_INFO * )HLIB_STD_MemCalloc( sizeof( MXDBC_DB_INFO ) );
	if (pstDbInfo == NULL)
	{
		HxLOG_Error("[PL_DMS] Error> Out of memory\n" );
		return ERR_FAIL;
	}

	HxSTD_snprintf( pstDbInfo->db_path, sizeof( pstDbInfo->db_path ), "%s", PL_DMS_DEFAULT_DB_PATH);
	pstDbInfo->db_tempPath 	= NULL;
	pstDbInfo->type 		= PL_DMS_DEFAULT_DB_TYPE;

	// check if homma crashed
	crashed = HOMMA_CheckPreviouslyCrashed();
	HxLOG_Print("_dms_start() / HOMMA was crashed???%s\n", (crashed ? "Yes" : "No"));
	pstDbInfo->eIntegrityCheck = (crashed ? INTEGRITY_CHECK : NON_CHECK);

	pContext->hDmsDB = mxDlnaDmsDb_createHandle( pstDbInfo );
	HOM_MEMFREE( pstDbInfo );

	if( pContext->hDmsDB == NULL )
	{
		HxLOG_Error("[PL_DMS] Error> Fail to mxDlnaDmsDb_createHandle()..\n" );
		return ERR_FAIL;
	}

	/*  user file operation callback 등록 */
	PL_DMS_FilsOperation_SetUserCallbacks();

	/* Create DMS Object */
	__pldms_convert_dmsInfoToLib( pContext, &stMxDmsInfo );
	pContext->hDms = mxDlnaDms_createDms( &stMxDmsInfo );
	if( pContext->hDms == NULL )
	{
		mxDlnaDmsDb_destroyHandle();
		pContext->hDmsDB = NULL;

		HxLOG_Print( "[ PL_DMS ] Error> Fail to mxDlnaDms_createDms()\n" );
		return ERR_FAIL;

	}

	/* DB 관련 초기화 작업 */
	mxDlnaDmsDb_initialize();

	/* Regist Icon if DMS Support */
	__pl_dms_regist_icon();

	/* Media Server가 내부적으로 사용하는 Callback 설정 */
	nRet = mxDlnaDms_setAppCallback( pContext->hDms,
										__pldms_CB_FileUploadDone,
										__pldms_CB_FileDestroyDone,
										__pldms_CB_FileMoveObject,
										__pldms_CB_FileUpdateObject );

	/* 사용할 Resource 요청/등록 */
	__pldms_resource_config( &stConfig );

	/* setting ScanFilter */
	PL_DMS_ScanFilterInit( &stScanOption );

	nRet = mxDlnaFileScanner_create( &pContext->hFSS, &stScanOption, pContext->hDmsDB, pContext->hDms );
	HxLOG_Print("[mxDlnaFileScanner_create]nRet:%d\n", nRet);
	if (crashed)
	{
		HxLOG_Print("dms_start() / remove crash checker file\n");
		HOMMA_RemoveCrashCheckerFile();
	}

	HxLOG_Print( "[ PL_DMS ] DmsDB(%p) FSS(%p) Handle(%p) ........  ok\n", pContext->hDmsDB, pContext->hFSS, pContext->hDms);
	PL_DMS_SetDmsHandles();

	mxDlnaDms_startDms( pContext->hDms );

	__pldms_systemfolder_init();

#if defined(PL_DMS_LIVETV_APP_SUPPORT)
	PL_DMS_SetupTunerEpg(PL_DMS_EPG_TARGET_BOTH);
#endif

#if defined(PL_DMS_THIRDPARTY_PVR_FEATURE)
	PL_DMS_SetupForPVR();
#endif

	HxLOG_Error("[ PL_DMS ] (%s)--\n", __FUNCTION__);
	return ERR_OK;
}

PL_DMS_Result_e		PL_DMS_Stop( void )
{
	PL_DMS_Context_t 	*pContext = __pl_dms_GetContext();
	HxLOG_Error("[ PL_DMS ] (%s)++\n", __FUNCTION__);

	/* Add lock */
	pContext->is_stopping = TRUE;

	/* Add waiting routine for channel gathering loop */
	/* ... */


	/** Scan 정지 */
	if( pContext->hFSS )
	{
		if(mxDlnaFileScanner_stopScan( pContext->hFSS ))
		{
			HxLOG_Error("Error> Scanner stop fail. \n");
		}
	}

	/** Start시 생성된 DMS Object 제거 */
	if( pContext->hDms )
	{
		mxDlnaDms_destroyDms( pContext->hDms );
		pContext->hDms = NULL;
	}

	/** File Scanner 제거 */
	if( pContext->hFSS )
	{
		mxDlnaFileScanner_destroy( pContext->hFSS );
		pContext->hFSS = NULL;
	}

	/** DB Handle 제거 */
	if( pContext->hDmsDB )
	{
		mxDlnaDmsDb_destroyHandle();
		pContext->hDmsDB = NULL;
	}

	/** 생성된 모든 System Folder 제거 */
	mxDlnaDmsSfm_removeAllSystemFolder();

	/** Clear channel list */
	PL_DMS_ClearTunerEpg();
	if (1)
	{
		if (pContext->pChannelGroupList != NULL)
		{
			HLIB_LIST_RemoveAllFunc( pContext->pChannelGroupList, HLIB_STD_MemFree_CB );
			pContext->pChannelGroupList = NULL;
		}
	}

	HxSTD_MemSet( pContext, 0x00, sizeof( PL_DMS_Context_t ) );
	pContext->is_init = FALSE;
	pContext->is_stopping = FALSE;

	HxLOG_Error("[ PL_DMS ] (%s)--\n", __FUNCTION__);
	return ePL_DMS_Result_Ok;
}

HERROR	PL_DMS_SetDmsName( const HCHAR *pszDmsName )
{
	PL_DMS_Context_t 	*pContext = __pl_dms_GetContext();

	if( pContext->hDms )
		mxDlnaDms_FriendlyRename( pContext->hDms, pszDmsName );

	return ePL_DMS_Result_Ok;
}

HERROR	PL_DMS_GetDmsName( HCHAR *pszDmsName, HINT32 nBufSize )
{
	return ePL_DMS_Result_Ok;
}

HERROR	PL_DMS_RegisterStatusEventCallback(PL_DMS_StatusEventCallback cb)
{
	PL_DMS_Context_t	*pContext = __pl_dms_GetContext();

	if (pContext->tStatusEvent_cb != NULL)
	{
		HxLOG_Error("[PL_DMS] Error> non-NULL run_event_cb\n");
	}
	pContext->tStatusEvent_cb = cb;
	return eHOM_Result_Ok;
}

HERROR	PL_DMS_UnregisterStatusEventCallback(void)
{
	PL_DMS_Context_t	*pContext = __pl_dms_GetContext();
	pContext->tStatusEvent_cb = NULL;
	return eHOM_Result_Ok;
}

/*-----------------------------------------------------------------------------
 * Fling 관련
 *-----------------------------------------------------------------------------*/

/* Local Path를 통해 URI 및 Meta 정보를 얻는 함수 */
PL_DMS_Result_e	PL_DMS_GetFlingInfoFromPath( HCHAR *pPath, HCHAR **ppOutUri, HCHAR **ppOutMeta )
{
	PL_DMS_Context_t 	*pContext = __pl_dms_GetContext();
	MXDLNA_CDSOBJECT	*pCdsObject = NULL;
	MXDLNA_CDSRESOURCE	*pBestRes	= NULL;
	MXDLNA_CDSRESOURCE	*pRes	= NULL;
	HCHAR				*pOutMeta = NULL;
	HINT32				nError = 0;
	PL_DMS_Result_e		nRet = ePL_DMS_Result_Ok;

	if (( pContext->hDms == NULL) || (pContext->is_init == FALSE))
	{
		HxLOG_Error( "[ PL_DMS ] %s(%d) Error> Inactive DMS status(%d)\n", __FUNCTION__, __LINE__, pContext->is_init);
		return ePL_DMS_Result_Error;
	}

	pCdsObject = mxDlnaDms_getCdsObjectByPath( pContext->hDms, pPath );
	if( pCdsObject == NULL )
	{
		HxLOG_Error( "[ PL_DMS ] Error> CDS Object is NULL(Path:%s)\n", pPath);
		return ePL_DMS_Result_Error;
	}

	if( pCdsObject->res == NULL )
	{
		HxLOG_Error( "[ PL_DMS ] Not Exist Resource!!!!\n" );
		mxDlnaDms_closeServerObjectList( pContext->hDms, pCdsObject );
		return ePL_DMS_Result_Error;
	}

	/* Resource중, .m3u8이 우선권 */
	pRes = pBestRes = pCdsObject->res;
	while( pRes )
	{
		if( pRes->Value )
		{
			if( HxSTD_StrStr( pRes->Value, ".m3u8" ) != NULL )
			{
				pBestRes = pRes;
				break;
			}
#if defined(CONFIG_NON_TRANSCODING_STREAMING) /* .ts recording file for HDR-3000T */
			else if (HxSTD_StrStr( pRes->Value, ".ts") != NULL )
			{
				pBestRes = pRes;
				break;
			}
#endif
		}
		pRes = pRes->Next;
	}

	if( pBestRes->Value == NULL )
	{
		HxLOG_Error( "[ PL_DMS ] Not Exist Resource URI!!!!\n" );
		mxDlnaDms_closeServerObjectList( pContext->hDms, pCdsObject );
		return ePL_DMS_Result_Error;
	}

	*ppOutUri = HLIB_STD_StrDup( pBestRes->Value );
	HxLOG_Print( "[ PL_DMS ] Path( %s ), URI( %s )\n", pPath, *ppOutUri );

	pOutMeta = mxDlnaDms_makeCurrentURIMetaData( pCdsObject, &nError );
	if(NULL != pOutMeta )
	{
		HxLOG_Print( "[ PL_DMS ] Path( %s ), Metainfo( err = %d )\n", pPath, nError );
		HxLOG_Print( "%s\n", pOutMeta );
		HxLOG_Print( "[ PL_DMS ] ============================================================\n" );
		*ppOutMeta = HLIB_STD_StrDup(pOutMeta);
		MX_FREE(pOutMeta);
	}

	mxDlnaDms_closeServerObjectList( pContext->hDms, pCdsObject );

	return nRet;
}

PL_DMS_Result_e	PL_DMS_GetFlingInfoOfPVRFromPath( HCHAR *pPath, HCHAR **ppOutUri, HCHAR **ppOutMeta )
{
	PL_DMS_Context_t 	*pContext = __pl_dms_GetContext();
	MXDLNA_CDSOBJECT	*pCdsObject = NULL;
	MXDLNA_CDSRESOURCE	*pBestRes	= NULL;
	MXDLNA_CDSRESOURCE	*pRes	= NULL;
	HCHAR				*pOutMeta = NULL;
	HINT32				nError = 0;
	PL_DMS_Result_e		nRet = ePL_DMS_Result_Ok;
	HUID				cid;
	HCHAR*				path_title = NULL;
	HCHAR*				mount_path = NULL;

	HxLOG_Error( "Path:%s (+)\n", pPath);

	if (( pContext->hDms == NULL) || (pContext->is_init == FALSE))
	{
		HxLOG_Error( "[ PL_DMS ] Error> Inactive DMS status(%d)\n",  pContext->is_init);
		return ePL_DMS_Result_Error;
	}

	path_title = HLIB_STD_StrDup(pPath);
	path_title[strlen(path_title) - strlen(DxEXT_META_FILE)] = 0; // remove ".hjm"

	cid = PL_COMMON_GetContentID(path_title);
	HLIB_STD_MemFree(path_title);

	if (cid == -1)
	{
		HxLOG_Error( "PL_COMMON_GetContentID(%s) failed\n", pPath);
		return ePL_DMS_Result_Error;
	}

	mount_path = HOMMA_SYSTEM_GetMountPath(pPath);
	if (mount_path == NULL)
	{
		HxLOG_Error("HOMMA_SYSTEM_GetMountPath(path:%s) failed\n", pPath);
		return ePL_DMS_Result_Error;
	}

	pCdsObject = mxDlnaDms_getCdsObjectByContentID( pContext->hDms, mount_path , cid);

	free(mount_path);
	mount_path = NULL;

	if( pCdsObject == NULL )
	{
		HxLOG_Error( "[ PL_DMS ] Error> CDS Object is NULL(Path:%s)\n", pPath);
		return ePL_DMS_Result_Error;
	}

	if( pCdsObject->res == NULL )
	{
		HxLOG_Error( "[ PL_DMS ] Not Exist Resource!!!!\n" );
		mxDlnaDms_closeServerObjectList( pContext->hDms, pCdsObject );
		return ePL_DMS_Result_Error;
	}

	/* Resource중, .m3u8이 우선권 */
	pRes = pBestRes = pCdsObject->res;
	while( pRes )
	{
		if( pRes->Value )
		{
			if( HxSTD_StrStr( pRes->Value, ".m3u8" ) != NULL )
			{
				pBestRes = pRes;
				break;
			}
#if defined(CONFIG_NON_TRANSCODING_STREAMING) /* .ts recording file for HDR-3000T */
			else if (HxSTD_StrStr( pRes->Value, ".ts") != NULL )
			{
				pBestRes = pRes;
				break;
			}
#endif
		}
		pRes = pRes->Next;
	}

	if( pBestRes->Value == NULL )
	{
		HxLOG_Error( "[ PL_DMS ] Not Exist Resource URI!!!!\n" );
		mxDlnaDms_closeServerObjectList( pContext->hDms, pCdsObject );
		return ePL_DMS_Result_Error;
	}

	*ppOutUri = HLIB_STD_StrDup( pBestRes->Value );
	HxLOG_Print( "[ PL_DMS ] Path( %s ), URI( %s )\n", pPath, *ppOutUri );

	pOutMeta = mxDlnaDms_makeCurrentURIMetaData( pCdsObject, &nError );
	if(NULL != pOutMeta )
	{
		HxLOG_Print( "[ PL_DMS ] Path( %s ), Metainfo( err = %d )\n", pPath, nError );
		HxLOG_Print( "%s\n", pOutMeta );
		HxLOG_Print( "[ PL_DMS ] ============================================================\n" );
		*ppOutMeta = HLIB_STD_StrDup(pOutMeta);
		MX_FREE(pOutMeta);
	}

	mxDlnaDms_closeServerObjectList( pContext->hDms, pCdsObject );

	return nRet;
}

/* SVC Uid 을 통해 URI및 Meta 정보를 얻는 함수 */
PL_DMS_Result_e	PL_DMS_GetFlingInfoFromSvcUid( HINT32 hSvc, HCHAR **ppOutUri, HCHAR **ppOutMeta )
{
	PL_DMS_Context_t	*pContext = __pl_dms_GetContext();
	MXDLNA_CDSOBJECT	*pCdsObject = NULL;
	HCHAR				*pOutMeta = NULL;
	HINT32				nError = 0;
	MXDLNA_CDSRESOURCE  *pRes = NULL;
	const char          *pURL = NULL;
	PL_DMS_Result_e	nRet = ePL_DMS_Result_Ok;

	if (( pContext->hDms == NULL) || (pContext->is_init == FALSE))
	{
		HxLOG_Error( "[ PL_DMS ] %s(%d) Error> Inactive DMS status(%d)\n", __FUNCTION__, __LINE__, pContext->is_init);
		return ePL_DMS_Result_Error;
	}

	pCdsObject = mxDlnaDms_getCdsObjectByUniqValue( pContext->hDms, hSvc );
	if( pCdsObject == NULL )
	{
		HxLOG_Error( "[ PL_DMS ] %s(%d) Error> CDS Object is NULL!!!!\n", __FUNCTION__, __LINE__ );
		return ePL_DMS_Result_Error;
	}

	if( pCdsObject->res == NULL )
	{
		HxLOG_Error( "[ PL_DMS ] %s(%d) Error> Not Exist Resource!!!!\n", __FUNCTION__, __LINE__);
		mxDlnaDms_closeServerObjectList( pContext->hDms, pCdsObject );
		return ePL_DMS_Result_Error;
	}

	pRes = pCdsObject->res;
	while(pRes != NULL)
	{
		if (pRes->ProtocolInfo != NULL)
	   {
		   if (HxSTD_StrStr(pRes->ProtocolInfo, "x-mpegURL") != NULL)  // select m3u8 url
		   {
			   /* <res protocolInfo="http-get:*:video/x-mpegURL:DLNA.ORG_CI=1" dlna:resumeUpload="0">http://192.168.1.13:9000/web/transcoding/HCHANNEL:1.m3u8</res> */
			   pURL = pRes->Value;
			   break;
		   }
#if defined(CONFIG_NON_TRANSCODING_STREAMING)
		   else if (HxSTD_StrStr(pRes->ProtocolInfo, "video/mpeg") != NULL)  // select .ts NTLS url for TV
		   {
			   /* <res protocolInfo="http-get:*:video/mpeg:DLNA.ORG_CI=0" dlna:resumeUpload="0">http://192.168.1.11:9000/web/tuner/HCHANNEL:1.ts</res> */
			   pURL = pRes->Value;
			   break;
		   }
		   else if (HxSTD_StrStr(pRes->ProtocolInfo, "audio/mpeg") != NULL)  // select .ts NTLS url for RADIO
		   {
			   /* <res protocolInfo="http-get:*:audio/mpeg:DLNA.ORG_CI=0" dlna:resumeUpload="0">http://192.168.1.11:9000/web/tuner/HCHANNEL:1.ts</res> */
			   pURL = pRes->Value;
			   break;
		   }
#endif
	   }

		pRes = pRes->Next;
	}

	if( pURL == NULL )
	{
		HxLOG_Error( "[ PL_DMS ] %s(%d) Error> Not Exist Resource URI!!!!\n", __FUNCTION__, __LINE__);
		mxDlnaDms_closeServerObjectList( pContext->hDms, pCdsObject );
		return ePL_DMS_Result_Error;
	}

	*ppOutUri = HLIB_STD_StrDup( pURL );
	HxLOG_Print( "[ PL_DMS ] SVC Uid( %d ), URI( %s )\n", hSvc, *ppOutUri );

	pOutMeta = mxDlnaDms_makeCurrentURIMetaData( pCdsObject, &nError );
	if(NULL != pOutMeta )
	{
		HxLOG_Print( "[ PL_DMS ] %s(%d) Error> SVC Uid( %d ), Metainfo( err = %d )\n", __FUNCTION__, __LINE__, hSvc, nError );
		HxLOG_Print( "%s\n", *ppOutMeta );
		HxLOG_Print( "[ PL_DMS ] ============================================================\n" );
		*ppOutMeta = HLIB_STD_StrDup(pOutMeta);
		MX_FREE(pOutMeta);
	}

	mxDlnaDms_closeServerObjectList( pContext->hDms, pCdsObject );

	return nRet;
}

PL_DMS_Result_e	PL_DMS_GetLiveStreamInfo( HINT32 hSvc, HUINT32 *pnCount, PL_DMS_LiveStreamInfo_t **pList )
{
	PL_DMS_Context_t	*pContext = __pl_dms_GetContext();
	MXDLNA_CDSOBJECT	*pCdsObject = NULL;
	MXDLNA_CDSRESOURCE  *pRes = NULL;
	PL_DMS_Result_e	nRet = ePL_DMS_Result_Ok;
	PL_DMS_LiveStreamInfo_t *pTemp = NULL;
	HUINT32			  	nCount = 0;

	if(pnCount == NULL || pList == NULL) {
		HxLOG_Error( "[ PL_DMS ] %s(%d) Error> invalid Value\n", __FUNCTION__, __LINE__);
		return ePL_DMS_Result_Error;
	}

	*pnCount = 0;
	*pList = NULL;

	if (( pContext->hDms == NULL) || (pContext->is_init == FALSE))
	{
		HxLOG_Error( "[ PL_DMS ] %s(%d) Error> Inactive DMS status(%d)\n", __FUNCTION__, __LINE__, pContext->is_init);
		return ePL_DMS_Result_Error;
	}

	pCdsObject = mxDlnaDms_getCdsObjectByUniqValue( pContext->hDms, hSvc );
	if( pCdsObject == NULL )
	{
		HxLOG_Error( "[ PL_DMS ] %s(%d) Error> CDS Object is NULL!!!!\n", __FUNCTION__, __LINE__ );
		return ePL_DMS_Result_Error;
	}

	if( pCdsObject->res == NULL )
	{
		HxLOG_Error( "[ PL_DMS ] %s(%d) Error> Not Exist Resource!!!!\n", __FUNCTION__, __LINE__);
		mxDlnaDms_closeServerObjectList( pContext->hDms, pCdsObject );
		return ePL_DMS_Result_Error;
	}

	pRes = pCdsObject->res;

	while(pRes != NULL)
	{
		if(pRes->Value != NULL) {
			nCount++;
		}

		pRes = pRes->Next;
	}

	if(nCount > 0)
	{
		HINT32				i;
		HCHAR				*mime;

		pTemp = ( PL_DMS_LiveStreamInfo_t* )HLIB_STD_MemCalloc(sizeof(PL_DMS_LiveStreamInfo_t)*nCount);
		HxSTD_MemSet(pTemp, 0x0, sizeof(PL_DMS_LiveStreamInfo_t)*nCount);

		pRes = pCdsObject->res;

		for(i = 0; i<nCount; i++)
		{
			if(pRes->Value != NULL)
			{
				HxSTD_StrNCpy(pTemp[i].szURI, pRes->Value, PL_DMS_LIVE_STREAM_URL_MAX);
				mime = mxMedia_getMimeTypeByProtocolInfo(pRes->ProtocolInfo);

				if(mime != NULL) {
					HxSTD_StrNCpy(pTemp[i].szMIME, mime, PL_DMS_LIVE_STREAM_MIME_MAX);
					HLIB_STD_MemFree(mime);
				}
			}

			pRes = pRes->Next;
		}
	}

	mxDlnaDms_closeServerObjectList( pContext->hDms, pCdsObject );

	*pList = pTemp;
	*pnCount = nCount;

	return nRet;
}

/*-----------------------------------------------------------------------------
 *  Woon Server 를 위해
 *-----------------------------------------------------------------------------*/
HERROR	PL_DMS_Get_MxDlna_Res(PL_DMS_MXDLNA_HANDLE_t *pHandle)
{
	PL_DMS_Context_t 	*pContext = __pl_dms_GetContext();

	if(pContext->is_init == FALSE)
	{
		HxLOG_Error("Error> DMS is NOT init. \n");
		return ERR_FAIL;
	}

	HxLOG_Print("[%s:%d] DmsDB(%p) FSS(%p) Handle(%p) ........  ok\n", __FUNCTION__, __LINE__,  pContext->hDmsDB, pContext->hFSS, pContext->hDms);
	if((pContext->hDms == NULL) || (pContext->hDmsDB == NULL) || (pContext->hFSS == NULL))
	{
		HxLOG_Error("Error> DMS Handle is NULL. \n");
		return ERR_FAIL;
	}

	pHandle->hDms = pContext->hDms;
	pHandle->hDmsDB = pContext->hDmsDB;
	pHandle->hFSS = pContext->hFSS;

	return ERR_OK;
}

PL_DMS_Status_e	PL_DMS_GetStatus(void)
{
	PL_DMS_Context_t 	*pContext = __pl_dms_GetContext();
	return pContext->eStatus;
}

HBOOL	PL_DMS_IsStopping(void)
{
	PL_DMS_Context_t 	*pContext = __pl_dms_GetContext();
	return pContext->is_stopping;
}

HBOOL	PL_DMS_IsStreaming(void)
{
	HINT32 cnt = 0;

	cnt = mxDlnaDms_getCurrnetStreamingCnt();

	return (cnt > 0) ? TRUE : FALSE;
}
