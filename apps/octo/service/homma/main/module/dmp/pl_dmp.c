/*************************************************************************************************************
	File            : pl_dmp.c
	author          : IPM tipmsw1@humaxdigital.com
	comment         :
	date            :
	attention       :

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (Humax) and
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

#define	________________Header_Files_______________________________

#include <hlib.h>

#include "mxdlna.h"
#include "mxres.h"
#include "mxfile.h"
#include "mxdlna_mscp.h"
#include "mxmedia_define.h"

#include "common.h"
#include "pl_common.h"
#include "pl_dmp.h"

#define ________________Private_Mecro_and_Defines__________________
#define	__MODULE__	"PL:DMP"

#define PL_DMP_MAX_CDS_BROWSING_ITEM		( 1024 )

#define PL_DMP_ASSERT(x)	\
    if ( x == NULL )	\
    {\
		        HxLOG_Error("[ %s ] : the param is NULL.\n", __MODULE__);\
		        return ERR_FAIL;\
    }\

#define	PL_DMP_SNPRINTF(x, v)			HxSTD_snprintf(x, sizeof(x), "%s", v)

#define ________________Internal_APIs_Prototype____________________
MX_INT32 			PL_USERDATA_Make( MXDLNA_CDSOBJECT *object, void *appData );
MX_INT32 			PL_USERDATA_Free( void* appData );

MXRES_PRE_TABLE 	*PL_DLNA_CbGetPreCmdTable( MX_INT32 media_type );
MXPN_PRI_MAP 		*PL_DLNA_CbGetProfilePriTable( MX_INT32 media_type );
MX_INT32 			PL_DLNA_CbIsAvailablePlay( MX_INT32 format );

#define ________________Internal_Typedef_and_Structure_____________
typedef struct
{
	HBOOL							is_init;

	MXDLNA_USER_DEVICECAP			*stDevCap;

	MXDLNA_MSCP_OBJECTHANDLE 		mscp_handle;
	MSCP_ACTION_HANDLE              mscpa_handle;

	void							*hCDS;

	HxList_t 						*pDmsHList;

	PL_DMP_DeviceCallback 			cbDevice;
	PL_DMP_BrowseResultCallback		cbBrowseResult;
	PL_DMP_DeviceResponseSink		cbDeviceResponse;
	PL_DMP_SearchResultCallback		cbSearchResult;

	HUINT32							nCdsItemAccum;
	HINT32							nTotalItem;

	PL_DMP_DmsInfo_t				stCurrentDMS;
	HCHAR							szCurrentCID[ CID_SIZE ];

	CO_MediaInstence_t				*pItemList;
	PL_DMP_DmsInfo_t  				*pDmsSendList;
}PL_DMP_Context_t;

typedef struct
{
	HBOOL  bAllDMS; /* default false; whether search target is specific UDN or All DMSs */
	HINT32 nReqId;
	HCHAR szUDN[ UDN_MAX ];  ///< CDS UDN
} PL_DMP_CDSReq_t;

#define ________________Private_Static_Variables___________________
static PL_DMP_Context_t s_PL_DMP_Context = { 0, };


#define ________________Internal_Functions_________________________
static inline PL_DMP_Context_t *__dmp_context( void )
{
	return &s_PL_DMP_Context;
}

/*------------------------------------------------------------------
 -	device list
--------------------------------------------------------------------*/
static HBOOL    __dmsinfo_compare(void *pSrc, void *pDest)
{
	PL_DMP_DmsInfo_t *pstSrc  = (PL_DMP_DmsInfo_t*)pSrc;
	PL_DMP_DmsInfo_t *pstDest = (PL_DMP_DmsInfo_t*)pDest;

	if(pSrc == NULL || pDest == NULL)
		return FALSE;

	if (HxSTD_StrNCmp(pstSrc->szUDN, pstDest->szUDN, UDN_LEN) == 0)
		return TRUE;

	return FALSE;
}

static void 	__dms_info_dump(void *data)
{
	PL_DMP_DmsInfo_t *p = (PL_DMP_DmsInfo_t *)data;
	HxLOG_Print("[ %s:%d ] FriendlyName : %s / UDN :%s / IP : %s \n", \
			__MODULE__, __LINE__, p->szFriendlyName, p->szUDN, p->szIp);
}

static HBOOL	__is_current_dms(HCHAR *pUdn)
{
	PL_DMP_Context_t 	*hDLNA = __dmp_context();
	if( HxSTD_StrCmp( pUdn , hDLNA->stCurrentDMS.szUDN ) == 0 )
		return TRUE;

	return FALSE;
}

static HBOOL	__is_listed_dms(HCHAR *pUdn)
{
	PL_DMP_Context_t 	*hDLNA = __dmp_context();
	HUINT32			  	nDmsCount = HLIB_LIST_Length(HLIB_LIST_First(hDLNA->pDmsHList));
	HxList_t			*p;
	PL_DMP_DmsInfo_t	*pDms;
	HINT32				i;

	if(nDmsCount > 0)
	{
		for(i = 0; i<nDmsCount; i++)
		{
			p = HLIB_LIST_GetListItem(HLIB_LIST_First(hDLNA->pDmsHList), i);
			pDms = HLIB_LIST_Data(p);
			if( HxSTD_StrCmp( pUdn , pDms->szUDN ) == 0 )
				return TRUE;
		}
	}

	return FALSE;
}

/*  convert data from Library	 */
static void __convert_mxdevice_to_dmsinfo(MXDLNA_DEVICE *pMxDevice, PL_DMP_DmsInfo_t *pPalDevice )
{
	PL_DMP_SNPRINTF( pPalDevice->szFriendlyName, pMxDevice->FriendlyName );
	PL_DMP_SNPRINTF( pPalDevice->szUDN, pMxDevice->UDN );
	//PL_DMP_SNPRINTF( pPalDevice->szIp, (HCHAR *)mxDlnaMscp_getDeviceIPByUDN(hDLNA->mscp_handle,pMxDevice->UDN ));
}

static HINT32 __get_itemlist( void )
{
	PL_DMP_Context_t 	*hDLNA = __dmp_context();
	CO_MediaInstence_t	*pList = NULL, *pTemp = NULL;
	HINT32	nItemCount = hDLNA->nTotalItem, i = 0;

	if( nItemCount <= 0 )
		return 0;

	if (hDLNA->hCDS == NULL)
	{
		HxLOG_Error("[PL_DMP] Error> NULL CDS!\n");
		return 0;
	}

	pList = (CO_MediaInstence_t *)HLIB_STD_MemCalloc(sizeof(CO_MediaInstence_t)*nItemCount);
	HxSTD_MemSet(pList, 0x0, sizeof(CO_MediaInstence_t)*nItemCount);

	for( i = 0; i < nItemCount; ++i )
	{
		mxDlnaMscp_getDataFromHandle( hDLNA->hCDS , i , (void*)&pTemp);

		if( pTemp == NULL )
		{
			/* 20개 단위로 Cache 하기 때문에 20개 이상시 기다려야 하는데... */
			HxLOG_Print("[PL_DMP] item(%d) is NULL \n", i);
			nItemCount--;
			continue;
		}

		PL_DMP_SNPRINTF( pList[i].IDofUpper, hDLNA->szCurrentCID );
		HxSTD_MemCopy(&pList[i], pTemp, sizeof(CO_MediaInstence_t));

		PL_USERDATA_Cds_Dump(&pList[i]);
	}

	/* 기존 List 삭제 후 재 생성 */
	if (hDLNA->pItemList)
	{
		HOM_MEMFREE(hDLNA->pItemList);
		hDLNA->pItemList = NULL;
	}

	hDLNA->pItemList = (CO_MediaInstence_t *)HLIB_STD_MemCalloc(sizeof(CO_MediaInstence_t)*nItemCount);
	HxSTD_MemSet(hDLNA->pItemList, 0x0, sizeof(CO_MediaInstence_t)*nItemCount);
	HxSTD_MemCopy( hDLNA->pItemList, pList, sizeof( CO_MediaInstence_t ) * nItemCount );

	HOM_MEMFREE( pList );
	pList = NULL;

	hDLNA->nTotalItem = nItemCount;

	HxLOG_Print( "[ %s:%d ] Item list is created. \n", __MODULE__, __LINE__ );
	return nItemCount;
}

/*------------------------------------------------------------------
 - callback function for Library
--------------------------------------------------------------------*/
static void __browse_cb(
	 MXDLNA_CP_HANDLE CDSCP,
	 MXDLNA_MSCP_OBJECTHANDLE *handle ,
	 void *userObject,
	 HINT32 errorCode,
	 HINT32 NumberReturned,
	 HINT32 TotalNumber
)
{
	HINT32	i = 0;
	PL_DMP_Context_t *hDLNA = __dmp_context();
	HxLOG_Print("[PL_DMP] Callback number = (%d/%d) errorCode = %d \n" , NumberReturned, TotalNumber, errorCode);

	if (!hDLNA->cbBrowseResult)
	{
		HxLOG_Error("[PL_DMP] Error> No browseResult callback!\n");
		return;
	}

	hDLNA->nCdsItemAccum += NumberReturned;

	if (errorCode != 0)
	{
		hDLNA->cbBrowseResult(hDLNA->stCurrentDMS.szUDN, NULL, 0, errorCode);
	}
	else
	{
		/* Cached Complete */
		if(hDLNA->nCdsItemAccum == TotalNumber)
		{
			HINT32 nItemCount = __get_itemlist();
			hDLNA->cbBrowseResult( hDLNA->stCurrentDMS.szUDN,  hDLNA->pItemList, nItemCount, 0 );
			for(i=0; i<nItemCount; i++)
				PL_USERDATA_Cds_Dump(&hDLNA->pItemList[i]);
		}
		else
		{
			void *pItem = NULL;
			/* Not Complete for All files */
			mxDlnaMscp_getDataFromHandle( hDLNA->hCDS , NumberReturned , (void**)&pItem );
		}
	}
	return;
}

static void __search_cb( MXDLNA_CP_HANDLE CDSCP, MXDLNA_MSCP_BROWSEARGS *args, MXDLNA_CDSOBJECT* result, int errorCode, int NumberReturned,int TotalNumber)
{
	PL_DMP_Context_t *hDLNA = __dmp_context();
	PL_DMP_CDSReq_t *pCdsReq = NULL;
	PL_DMP_CDSResultInfo_t *pResultInfo = NULL;
	CO_MediaInstence_t *pItemList = NULL;
	MXDLNA_CDSOBJECT *pObject = result;
	HINT32 i = 0;
	HBOOL bAllDMS;
	HINT32 reqId = 0;
	HINT32 nRealCnt = 0;

	if ((args == NULL) || (result == NULL) || (NumberReturned <= 0))
	{
		HxLOG_Error("[ PL_DMP ] Error> invalid arg(num:%D)!\n", NumberReturned);
		return ;
	}

	pCdsReq = (PL_DMP_CDSReq_t *)args->UserObject;
	if(pCdsReq == NULL)
	{
		HxLOG_Error("[ PL_DMP ] Error> pCdsReq == NULL\n");
		goto END_PROC;
	}

	bAllDMS = pCdsReq->bAllDMS;
	reqId = pCdsReq->nReqId;

	pResultInfo = (PL_DMP_CDSResultInfo_t *)HLIB_STD_MemCalloc(sizeof(PL_DMP_CDSResultInfo_t));
	if(pResultInfo == NULL)
	{
		HxLOG_Error("[ PL_DMP ] Error> pResultInfo == NULL\n");
		goto END_PROC;
	}

	HxSTD_MemSet(pResultInfo, 0x00, sizeof(PL_DMP_CDSResultInfo_t));

	HxSTD_StrNCpy( pResultInfo->szUDN, pCdsReq->szUDN, sizeof(pResultInfo->szUDN)-1 );
	HxSTD_StrNCpy( pResultInfo->szCID, args->ObjectID, sizeof(pResultInfo->szCID)-1 );
	pResultInfo->nTotalMatch 	= TotalNumber;
	pResultInfo->nReturnCnt 	= NumberReturned;
	pResultInfo->bAllDMS 		= bAllDMS;
	pResultInfo->nError 		= errorCode;
	pResultInfo->nReqId 		= reqId;

	HxLOG_Print("[ PL_DMP ] [AllDMS : %d, UDN : %s, ErroCode : %d, reqId : %d, NumberReturned : %d, TotalNumber : %d]\n",
		   	bAllDMS, pResultInfo->szUDN, errorCode,  reqId, NumberReturned, TotalNumber );

	pItemList = (CO_MediaInstence_t *)HLIB_STD_MemCalloc(sizeof(CO_MediaInstence_t)*NumberReturned);
	if(pItemList == NULL)
	{
		HxLOG_Error("[ PL_DMP ] Error> pItemList == NULL\n");
		goto END_PROC;
	}

	HxSTD_MemSet(pItemList, 0x00, sizeof(CO_MediaInstence_t)*NumberReturned);

	for( i = 0; i < NumberReturned; i++ )
	{
		if ( PL_USERDATA_Make( pObject, &pItemList[nRealCnt] ) != ERR_OK )
			continue;

		HxSTD_snprintf( pItemList[nRealCnt].IDofUpper , sizeof(pItemList[nRealCnt].IDofUpper), "%s", pObject->ParentID );

		pItemList[nRealCnt].totalMatches = TotalNumber;
		pObject = pObject->next;
		nRealCnt++;

		if ( NULL == pObject )
			break;
	}

	if ( hDLNA->cbSearchResult != NULL )
		hDLNA->cbSearchResult( NULL, pResultInfo, pItemList, reqId );

END_PROC:
	if ( pCdsReq != NULL )
	{
		HLIB_STD_MemFree( pCdsReq );
		pCdsReq = NULL;
		args->UserObject = NULL;
	}

	if (pResultInfo != NULL) /* CID #143103 (RESOURCE_LEAK) */
	{
		HLIB_STD_MemFree(pResultInfo);
		pResultInfo = NULL;
	}

	if (pItemList != NULL) /* CID #143104 (RESOURCE_LEAK) */
	{
		HLIB_STD_MemFree(pItemList);
		pItemList = NULL;
	}
	return;
}

static void __device_cb(void * CDSCP, MXDLNA_DEVICE *pEventDMS , HINT32 added )
{
	PL_DMP_Context_t 	*hDLNA = __dmp_context();
	PL_DMP_DmsInfo_t 	*pDmsInfo = NULL;

	HBOOL		isCurrent 	= FALSE;
	HBOOL		isAdded		= FALSE;

	isAdded = ( HBOOL )added;

	if(NULL == pEventDMS)
		return;

	HxLOG_Print( "[ %s:%d ] Device Event( %s : %s : %s )\n", __MODULE__, __LINE__, \
			pEventDMS->FriendlyName, pEventDMS->UDN, added? "ADD":"REMOVE");

	pDmsInfo = (PL_DMP_DmsInfo_t *)HLIB_STD_MemCalloc(sizeof(PL_DMP_DmsInfo_t));
	HxSTD_MemSet(pDmsInfo, 0x0, sizeof(PL_DMP_DmsInfo_t));

	__convert_mxdevice_to_dmsinfo(pEventDMS, pDmsInfo);

	/* check if the current server changed or not for app to know */
	if(__is_current_dms(pDmsInfo->szUDN))
		isCurrent = TRUE;

	if( isAdded ) /* DMS Added */
	{
		hDLNA->pDmsHList = HLIB_LIST_Append(hDLNA->pDmsHList, (void *)pDmsInfo);
	}
	else  /* DMS Remove */
	{
		HxList_t *pRemove = HLIB_LIST_FindEx(hDLNA->pDmsHList, (void*)pDmsInfo, __dmsinfo_compare);
		if(pRemove)
		{
			void *p = HLIB_LIST_Data(pRemove);
			hDLNA->pDmsHList = HLIB_LIST_Remove(hDLNA->pDmsHList, (void*)HLIB_LIST_Data(pRemove));
			HOM_MEMFREE(p);
		}
		else
			return;

		/* current dms is removed */
		if(isCurrent)
			HxSTD_MemSet( &hDLNA->stCurrentDMS, 0x00, sizeof( hDLNA->stCurrentDMS ) );
	}

	/* Browse Callback 호출  */
	if ( hDLNA->cbDevice )
		hDLNA->cbDevice( pDmsInfo , isAdded, isCurrent );

	/* Dump DMS List */
#if 0
	HxLOG_Debug("[ %s:%s:%d ] DMS List Count : %d \n", __MODULE__, __FUNCTION__, __LINE__, \
			HLIB_LIST_Length(HLIB_LIST_First(hDLNA->pDmsHList)));
	HLIB_LIST_Foreach(hDLNA->pDmsHList, __dms_info_dump);
#endif
}

/* Direct Browse에 대한 Result Callback */
static void __direct_browse_cb( MXDLNA_CP_HANDLE CDSCP,
		MXDLNA_MSCP_BROWSEARGS *args,
		MXDLNA_CDSOBJECT *result,
		int error, int NumberReturned,int TotalNumber)
{
	PL_DMP_Context_t 	*hDLNA = __dmp_context();
	MXDLNA_CDSOBJECT 	*pObject = result;
	HINT32	i = 0;
	HINT32	nScanCount = 0;
	CO_MediaInstence_t *pTempList = NULL;

	if ((args == NULL) || (result == NULL) || (NumberReturned <= 0))
	{
		HxLOG_Error("[ PL_DMP ] Error> invalid arg(num:%d, total:%d)!\n", NumberReturned, TotalNumber);
		return;
	}

	if(args->BrowseFlag != MXDLNA_MSCP_BROWSEFLAG_CHILDREN) return;

	if (hDLNA->pItemList)
	{
		HOM_MEMFREE( hDLNA->pItemList );
		hDLNA->pItemList = NULL;
	}

	hDLNA->nTotalItem = 0;

	if(error!= 0)
		goto CALL_MW_BROWSE_CALLBACK;


	/* 임시 List 생성, Returned된 모든 List를 DMP에서 처리 하지 않을수 있으므로, 실제로는 더 적을수 있음 */
	pTempList = HLIB_STD_MemCalloc(sizeof(CO_MediaInstence_t)*NumberReturned);
	if (pTempList == NULL)
		goto CALL_MW_BROWSE_CALLBACK;

	HxSTD_MemSet(pTempList, 0x0, sizeof(CO_MediaInstence_t)*NumberReturned);
	for( i = 0; i < NumberReturned; i++ )
	{
		if(PL_USERDATA_Make( pObject, &pTempList[nScanCount]) != ERR_OK) 	continue;

		PL_DMP_SNPRINTF( pTempList[nScanCount].IDofUpper, pObject->ParentID );
		pTempList[nScanCount].totalMatches = TotalNumber;

		pObject = pObject->next;
		nScanCount++;
	}
	hDLNA->pItemList = pTempList;
	hDLNA->nTotalItem = nScanCount;


CALL_MW_BROWSE_CALLBACK:
	if( hDLNA->cbBrowseResult)
	{
		HxLOG_Print( "[ %s:%d ] DirectBrowseResult ::  Returned( %d ), Real_ScanCount( %d ), List( %p )\n", \
			   __MODULE__, __LINE__, NumberReturned, nScanCount, hDLNA->pItemList );
		hDLNA->cbBrowseResult( hDLNA->stCurrentDMS.szUDN,  hDLNA->pItemList, nScanCount, error);
	}

	for(i=0; i<nScanCount; i++)
		PL_USERDATA_Cds_Dump(&hDLNA->pItemList[i]);
}

void __device_response_sink_cb(MXDLNA_CP_HANDLE hCp, MXDLNA_ACTION_HANDLE hAction, HINT32 actionCommand, HINT32 error, void *data)
{
	PL_DMP_Context_t *hDLNA = __dmp_context();
	// !! Warning 모든 "void *data" 값은 Call이후에 메모리에서 해제됩니다.
	// 필요한 데이터는 복사하여 사용하시기 바랍니다.

	if(error)
		HxLOG_Print( "[%s:%d] action(%d) Error(%d) \n", __FUNCTION__, __LINE__, actionCommand, error);

	if(hDLNA->cbDeviceResponse)
		hDLNA->cbDeviceResponse(HLIB_STD_StrDup(mxDlnaMscp_getUDN(hAction)), actionCommand, error, data);
}

void __event_cb(MXDLNA_CP_HANDLE cp, const HCHAR* device_udn, HINT32 CMID, HINT32 service, HINT32 flag, void* userData)
{
    switch(service)
    {
        case MXDLNA_MSCP_CDS_EVENT_CONTAINERUPDATEID:
            // HxLOG_Warning( "[ PL_DMP ] _Event Received of CONTAINER UPDATE ID - %s \n\n", (char *)userData );
            break;

        case MXDLNA_MSCP_CDS_EVENT_TRANSFERIDS:
            // HxLOG_Warning( "[ PL_DMP ] _Event Received of MXDLNA_MSCP_CDS_EVENT_TRANSFERIDS\n\n" );
            break;

		default:
			break;
	}
}

static void __init_context( void )
{
	PL_DMP_Context_t *hDLNA = __dmp_context();

	if(hDLNA->pDmsHList)
	{
		HLIB_LIST_RemoveAllFunc(hDLNA->pDmsHList, HLIB_STD_MemFree_CB);
		hDLNA->pDmsHList = NULL;
	}

	if (hDLNA->pDmsSendList)
	{
		HOM_MEMFREE(hDLNA->pDmsSendList);
		hDLNA->pDmsSendList = NULL;
	}

	if (hDLNA->pItemList)
	{
		HOM_MEMFREE( hDLNA->pItemList );
		hDLNA->pItemList = NULL;
	}

	HxSTD_MemSet(hDLNA, 0x0, sizeof(PL_DMP_Context_t));
	{
		hDLNA->stDevCap         = NULL;
		hDLNA->mscp_handle      = NULL;
		hDLNA->mscpa_handle     = NULL;
		hDLNA->hCDS             = NULL;
		hDLNA->pDmsHList         = NULL;
		hDLNA->cbDevice         = NULL;
		hDLNA->cbBrowseResult   = NULL;
		hDLNA->cbDeviceResponse = NULL;
		hDLNA->cbSearchResult   = NULL;
		hDLNA->pItemList        = NULL;
		hDLNA->pDmsSendList     = NULL;
		hDLNA->is_init          = FALSE;
	}
}

/* Init MSCP( Media Server Control Point ) */
static HERROR __init_MSCP( void )
{
	PL_DMP_Context_t 		*hDLNA = __dmp_context();
	MXDLNA_CP_CONFIG		stCpConfig;

	if ( hDLNA->is_init == FALSE )
	{
		HxSTD_MemSet(&stCpConfig, 0x00, sizeof(MXDLNA_CP_CONFIG));

		/* Callback 등 등록 */
		stCpConfig.user_make 				= PL_USERDATA_Make;
		stCpConfig.user_free 				= PL_USERDATA_Free;
		stCpConfig.BrowserResponseCallBack 	= __browse_cb;
		stCpConfig.DeviceAddOrRemove 		= __device_cb;
		stCpConfig.DeviceEvent 				= __event_cb;
		stCpConfig.DeviceResponse 			= __device_response_sink_cb;
		stCpConfig.user_data_size 			= sizeof( CO_MediaInstence_t );
		stCpConfig.pUserDeviceCap 			= hDLNA->stDevCap;

		/* 설정된 Config 적용 */
		mxDlnaMscp_config(&stCpConfig);
		mxDlna_setSystemCharacterSet( 949, NULL , NULL );

		hDLNA->mscp_handle = mxDlnaMscp_createMsCp();
		if( hDLNA->mscp_handle == NULL )
		{
			HxLOG_Print( "[ %s:%d ] mxDlnaMscp_createMsCp() Fail!\n", __MODULE__, __LINE__ );
			return ERR_FAIL;
		}

		hDLNA->is_init = TRUE;
	}
	else
	{
		HxLOG_Print( "[ %s:%d ]Already Init", __MODULE__, __LINE__ );
	}

	return ERR_OK;
}

/* start MSCP( Media Server Control Point ) */
HERROR	PL_DMP_Start( void )

{
	PL_DMP_Context_t 		*hDLNA = __dmp_context();

	if (hDLNA->is_init == FALSE)
	{
		HxLOG_Error("Error> uninitialized\n");
		return ERR_FAIL;
	}

	if (hDLNA->mscp_handle == NULL)
	{
		HxLOG_Error("Error> invalid mscp handle\n");
		return ERR_FAIL;
	}

	mxDlnaMscp_start( hDLNA->mscp_handle );

	if(hDLNA->mscp_handle)
         hDLNA->mscpa_handle = mxDlnaMscp_createContext(hDLNA->mscp_handle, NULL, 0);

	return ERR_OK;
}


/* CID에 해당하는 Container로 이동 */
static void __move_container( MXDLNA_MSCP_BROWSEARGS stBrowseArgs )
{
	PL_DMP_Context_t 	*hDLNA 	= __dmp_context();
	void 				*hCDS 	= mxDlnaMscp_openObjectHandle( hDLNA->mscp_handle , &stBrowseArgs ,0);

	if( hCDS )
	{
		if ( hDLNA->hCDS )
		{
			mxDlnaMscp_closeObjectHandle( hDLNA->hCDS );
			hDLNA->hCDS = NULL;
		}

		hDLNA->hCDS  = hCDS;

		/* cached data */
		if(mxDlnaMscp_getDataCountFromHandle(hCDS) > 0)
		{
			( void ) __get_itemlist();
			HxLOG_Error( "[ %s:%d ] Item is cached. (%d)\n)", __MODULE__, __LINE__, hDLNA->nTotalItem);

			hDLNA->cbBrowseResult( hDLNA->stCurrentDMS.szUDN, hDLNA->pItemList, hDLNA->nTotalItem, 0 );
		}
	}
	else
		HxLOG_Error( "[ %s:%d ]hCDS is NULL(%p\n)", __MODULE__, __LINE__, hCDS);
}

/* Direct Browse에 대한 Result Callback */
static void __cds_operation( MXDLNA_MSCP_BROWSEARGS  stBrowseArgs )
{
	PL_DMP_Context_t *hDLNA = __dmp_context();

	MSCP_ACTION_HANDLE 		Handle = mxDlnaMscp_createContext(hDLNA->mscp_handle, \
		   													hDLNA->stCurrentDMS.szUDN, \
														   	mxDlnaMscp_getCMID(hDLNA->mscp_handle) );

	if(Handle)
	{
		mxDlnaMscp_requestCdsOperation(Handle, &stBrowseArgs, __direct_browse_cb);
		mxDlnaMscp_destroyContext(Handle);
	}
}

static HERROR __set_current_dms(HCHAR *szUDN)
{
	MXDLNA_DEVICE 		*pDeviceInfo = NULL;
	PL_DMP_Context_t 	*hDLNA = __dmp_context();

	HxLOG_Print( "[ %s:%d ] Set DMS( UDN : %s )\n", __MODULE__, __LINE__, szUDN );
	mxDlnaMscp_setMediaServerByUDN( hDLNA->mscp_handle, szUDN );

	/* Current DMS 정보 갱신 */
	pDeviceInfo = mxDlnaMscp_getDeviceAtUDN( hDLNA->mscp_handle, szUDN );
	if( pDeviceInfo == NULL )
		return ERR_FAIL;

	HxSTD_MemSet(&hDLNA->stCurrentDMS, 0x00, sizeof(PL_DMP_DmsInfo_t));
	__convert_mxdevice_to_dmsinfo(pDeviceInfo, &hDLNA->stCurrentDMS);

	mxDlnaMscp_freeDeviceAtUDN(pDeviceInfo);
	pDeviceInfo = NULL;
	return ERR_OK;
}

#define ________________Public_Functions___________________________
/* Init DMP Context */
HERROR	PL_DMP_Init( void )
{
	PL_DMP_Context_t *hDLNA = __dmp_context();

	if( hDLNA->is_init == FALSE )
		( void )__init_context();

	/* Humax Capability 속성. DMS의 값과 일치 해야 함 */
	PL_DMP_MakeUserDeviceCaps("X_HMXCAP", "urn:schemas-humax-co-kr:metadata-1-0", "hmx");

	return  __init_MSCP();
}

/* Deinit DMP */
HERROR	PL_DMP_DeInit( void )
{
	PL_DMP_Context_t *hDLNA = __dmp_context();

	if ( hDLNA->is_init == TRUE )
	{
		HOM_MEMFREE(hDLNA->stDevCap);
		hDLNA->stDevCap = NULL;

		mxDlnaMscp_end(hDLNA->mscp_handle);
		mxDlnaMscp_destroyMsCp( hDLNA->mscp_handle );
		hDLNA->mscp_handle = NULL;

		mxDlnaMscp_destroyContext( hDLNA->mscpa_handle );
		hDLNA->mscpa_handle = NULL;

		__init_context();
	}
	else
	{
		HxLOG_Print( "[ %s:%d ] Not yet init", __MODULE__, __LINE__);
	}

	return ERR_OK;
}

/* Registration Callback */
HERROR	PL_DMP_RegisterEventCB(	PL_DMP_DeviceCallback cbDevice,
								PL_DMP_BrowseResultCallback cbBrowseResult,
								PL_DMP_SearchResultCallback cbSearchResult )
{
	PL_DMP_Context_t *hDLNA = __dmp_context();

	hDLNA->cbDevice = cbDevice;
	hDLNA->cbBrowseResult = cbBrowseResult;
	hDLNA->cbSearchResult = cbSearchResult;

	return ERR_OK;
}

/* UnRegistration Callback */
HERROR	PL_DMP_UnRegisterEventCB( void )
{
	PL_DMP_Context_t *hDLNA = __dmp_context();

	hDLNA->cbDevice = NULL;
	hDLNA->cbBrowseResult= NULL;

	return ERR_OK;
}

/* DMP Start( M-Search ) */
HERROR	PL_DMP_M_Search( void )
{
	PL_DMP_Context_t *hDLNA = __dmp_context();

	if (hDLNA->is_init == FALSE)
	{
		HxLOG_Error("Error> uninitialized\n");
		return ERR_FAIL;
	}

	if (hDLNA->mscp_handle == NULL)
	{
		HxLOG_Error("Error> invalid mscp handle\n");
		return ERR_FAIL;
	}

	mxDlnaMscp_sendMSearch( hDLNA->mscp_handle );
	return ERR_OK;
}

/* 검색된 전체 DMS의 List를 얻음 */
HERROR	PL_DMP_GetDmsList( HUINT32 *pnDmsCount, PL_DMP_DmsInfo_t **pDmsHList )
{
	HxList_t			*pDms = NULL;
	PL_DMP_Context_t 	*hDLNA = __dmp_context();
	HUINT32			  	nDmsCount = HLIB_LIST_Length(HLIB_LIST_First(hDLNA->pDmsHList));
	HINT32				i;

	*pnDmsCount = 0;
	if (hDLNA->pDmsSendList)
	{
		HOM_MEMFREE(hDLNA->pDmsSendList);
		hDLNA->pDmsSendList = NULL;
	}

	if(nDmsCount > 0)
	{
		hDLNA->pDmsSendList = ( PL_DMP_DmsInfo_t* )HLIB_STD_MemCalloc(sizeof(PL_DMP_DmsInfo_t)*nDmsCount);
		HxSTD_MemSet(hDLNA->pDmsSendList, 0x0, sizeof(PL_DMP_DmsInfo_t)*nDmsCount);
		for(i = 0; i<nDmsCount; i++)
		{
			pDms = HLIB_LIST_GetListItem(HLIB_LIST_First(hDLNA->pDmsHList), i);
			HxSTD_MemCopy(&hDLNA->pDmsSendList[i], HLIB_LIST_Data(pDms), sizeof(PL_DMP_DmsInfo_t));
			__dms_info_dump((void *)&hDLNA->pDmsSendList[i]);
		}
	}

	*pDmsHList = hDLNA->pDmsSendList;
	*pnDmsCount = nDmsCount;
	HxLOG_Debug("DMS list count(%d)\n", nDmsCount);
	return ERR_OK;
}

/* DMS List를 새로고침 - 제거된 것으로 판단되는 DMS는 2-30초 내에 Remove Event 발생 */
HERROR	PL_DMP_RefreshDms( HCHAR *pUdn )
{
	PL_DMP_Context_t 		*hDLNA = __dmp_context();
	MXDLNA_USER_DEVICE_LIST *pList = mxDlnaMscp_getMediaServerList( hDLNA->mscp_handle );

	if (pUdn != NULL)
	{
		mxDlnaMscp_isAliveDevice( hDLNA->mscp_handle , pUdn );
	}
	else
	{
		while( pList )
		{
			mxDlnaMscp_isAliveDevice( hDLNA->mscp_handle , pList->szUDN );
			pList = pList->pNext;
		}
	}

	return ERR_OK;
}

HBOOL	PL_DMP_IsDMPStarted( void )
{
	PL_DMP_Context_t *hDLNA = __dmp_context();

	return ( (hDLNA->mscp_handle != NULL) && hDLNA->is_init ) ? TRUE : FALSE;
}

HERROR    PL_DMP_Browse(PL_DMP_BrowseMode_t nMode,
	   					HCHAR *szUDN,
	   					HCHAR *szCID,
	   					PL_DMP_BrowseFlag_t Browse_Mode,
						HUINT32 nSortOrder,
	   					HUINT32 nStartIdx,
						HUINT32 nRequestedNum )
{
	PL_DMP_Context_t		*hDLNA = __dmp_context();
	MXDLNA_SORT_ORDER		_eSortOrder[4] = {nSortOrder, ASC_TITLE, ASC_SORT_CLASS, NONE_ORD};
	MXDLNA_MSCP_BROWSEARGS 	stBrowseArgs = { 0, };
	HINT32					i, j;
	HxLOG_Print( "[PL_DMP] Browse (UDN:%s, CID:%s, start:%d, req:%d)\n", szUDN, szCID, nStartIdx, nRequestedNum);

	hDLNA->nCdsItemAccum = 0;

	if(__is_listed_dms(szUDN) == FALSE)
	{
	    HxLOG_Error("Error> UDN:%s is not connected dms.\n", szUDN);
		return ERR_FAIL;
	}

	if(Browse_Mode == Dmp_BrowseFlag_Children)
	{
		if( __is_current_dms(szUDN) || HLIB_STD_StrCaseCmp( szCID, "0" ) == 0)
		{
			if( __set_current_dms(szUDN) != ERR_OK)
				return ERR_FAIL;
		}
	}

	for(i=0; i<4; i++)
	{
		/* check support sort order */
		if( mxDlnaMscp_isSupportSortOrder( hDLNA->mscp_handle, _eSortOrder[i]) != 0 )
			_eSortOrder[i] = NONE_ORD;
		else
		{
			for(j=0; j<i; j++)
			{
				if(_eSortOrder[j] == _eSortOrder[i])
				   	_eSortOrder[i] = NONE_ORD; /* 중복 */

				if(_eSortOrder[j] == NONE_ORD)
				{
					_eSortOrder[j] = _eSortOrder[i]; _eSortOrder[i] = NONE_ORD; /* 앞으로 */
				}
			}
		}
	}

	stBrowseArgs.BrowseFlag = Browse_Mode;
	stBrowseArgs.ObjectID = szCID;
	stBrowseArgs.UserObject = NULL;
	stBrowseArgs.Filter = "*";
	stBrowseArgs.StartingIndex = nStartIdx;
	stBrowseArgs.RequestedCount = nRequestedNum;
	stBrowseArgs.SortCriteria = mxDlnaMscp_setOrder( _eSortOrder[0], _eSortOrder[1],  _eSortOrder[2] , _eSortOrder[3] );

	PL_DMP_SNPRINTF( hDLNA->szCurrentCID, szCID );

	if(nMode == 1)
		__cds_operation( stBrowseArgs );
	else
		__move_container( stBrowseArgs );

	return ERR_OK;
}

HERROR    PL_DMP_Search( PL_DMP_SearchMode_e eMode,
					  	HCHAR *szUDN,
						HCHAR *szCID,
						HCHAR *szKeyword,
						HUINT32 nSortOrder,
						HUINT32 nStartIdx,
						HUINT32 nRequestedNum,
						HINT32 nReqId )
{
	PL_DMP_Context_t		*hDLNA = __dmp_context();

	MXDLNA_MSCP_BROWSEARGS stBrowseArgs = { 0 };
	MXDLNA_SORT_ORDER   eFirstSortOrder     = ASC_TITLE;
	MXDLNA_SORT_ORDER   eSecondSortOrder    = ASC_SORT_CLASS;
	PL_DMP_CDSReq_t *pCdsReq = NULL;

	HCHAR *pszSearchCriteria = NULL;
	HINT32 nCriteriaLen = 0;
	//mxDlnaMscp_isSearchAction();

	nCriteriaLen = mxDlnaMscp_makeSearchCriteria( &pszSearchCriteria, MXDLNA_SC_ITEM_TITLE_CONTAIN, szKeyword, MXDLNA_SC_NONE );
	if ( 0 == nCriteriaLen )
	{
		hDLNA->cbSearchResult( szUDN, (PL_DMP_CDSResultInfo_t *)NULL, (CO_MediaInstence_t *)NULL, nReqId );
		return ERR_FAIL;
	}

	/* Class sorting을 지원하는지 여부 - 디렉토리 우선으로 받기 위함 */
	if( MXDLNA_OK != mxDlnaMscp_supportSortOrder( hDLNA->mscp_handle, szUDN, ASC_TITLE ) )
	{
		eFirstSortOrder = NONE_ORD;
	}

	/* Title sorting을 지원하는지 여부 - 알파벳 정렬을 하기 위함 */
	if( MXDLNA_OK != mxDlnaMscp_supportSortOrder( hDLNA->mscp_handle, szUDN, ASC_SORT_CLASS ) )
	{
		eSecondSortOrder = NONE_ORD;
	}

	if( eFirstSortOrder > NONE_ORD )
	{
		stBrowseArgs.SortCriteria = mxDlnaMscp_setOrder( eFirstSortOrder, eSecondSortOrder, NONE_ORD, NONE_ORD );
	}
	else
	{
		if( eSecondSortOrder > NONE_ORD )
		{
			stBrowseArgs.SortCriteria = mxDlnaMscp_setOrder( eSecondSortOrder, NONE_ORD, NONE_ORD, NONE_ORD );
		}
	}

	pCdsReq = (PL_DMP_CDSReq_t *)HLIB_STD_MemCalloc(sizeof(PL_DMP_CDSReq_t));
	HxSTD_MemSet(pCdsReq, 0x00, sizeof(PL_DMP_CDSReq_t));
	pCdsReq->bAllDMS = FALSE;
	pCdsReq->nReqId = nReqId;
	HxSTD_StrNCpy( pCdsReq->szUDN, szUDN, sizeof(pCdsReq->szUDN)-1 );

	stBrowseArgs.BrowseFlag = MXDLNA_MSCP_BROWSEFLAG_CHILDREN;
	stBrowseArgs.ObjectID = szCID;
	stBrowseArgs.SearchCriteria = pszSearchCriteria;
	stBrowseArgs.UserObject = pCdsReq;
	stBrowseArgs.Filter = "*";
	stBrowseArgs.StartingIndex = nStartIdx;
	stBrowseArgs.RequestedCount = nRequestedNum;

	HxLOG_Print( "[PL_DMP] Search REQ(%d), CDS(%s), CID(%s), Keyworkd(%s), SeachCriteria(%s)\n", nReqId, szUDN, stBrowseArgs.ObjectID, szKeyword, stBrowseArgs.SearchCriteria );

	mxDlnaMscp_requestOperation( hDLNA->mscp_handle, szUDN, &stBrowseArgs, __search_cb );

	if ( pszSearchCriteria )
	{
		HLIB_STD_MemFree(pszSearchCriteria);
		pszSearchCriteria = NULL;
	}

	return ERR_OK;
}

HERROR    PL_DMP_SearchAll( PL_DMP_SearchMode_e eMode,
					  	HCHAR *szCID,
						HCHAR *szKeyword,
						HUINT32 nSortOrder,
						HINT32 nReqId ) /* No UDN and startIdx and requestedNum */
{
	PL_DMP_Context_t		*hDLNA = __dmp_context();
	MXDLNA_MSCP_BROWSEARGS stBrowseArgs = { 0 };
	MXDLNA_SORT_ORDER   eFirstSortOrder     = ASC_TITLE;
	MXDLNA_SORT_ORDER   eSecondSortOrder    = ASC_SORT_CLASS;
	PL_DMP_CDSReq_t *pCdsReq = NULL;
	HCHAR *pszSearchCriteria = NULL;
	HINT32 nCriteriaLen   = 0;
	HUINT32 nStartIdx     = 0;
	HUINT32 nRequestedNum = 100;

	HxList_t			*pDmsItem = NULL;
	HUINT32			  	nDmsCount = 0;
	HINT32				i;

	if (hDLNA->pDmsHList)
	{
		nDmsCount = HLIB_LIST_Length(HLIB_LIST_First(hDLNA->pDmsHList));
	}

	HxLOG_Debug("DMS list count(%d)\n", nDmsCount);
	if (nDmsCount <= 0)
	{
		HxLOG_Error("Error> invalid DMS count(%d)\n", nDmsCount);
		return ERR_FAIL;
	}

	nCriteriaLen = mxDlnaMscp_makeSearchCriteria( &pszSearchCriteria, MXDLNA_SC_ITEM_TITLE_CONTAIN, szKeyword, MXDLNA_SC_NONE );
	if ( 0 == nCriteriaLen )
	{
		HxLOG_Error("Error> invalid DMS count(reqId:%d, dmsCnt:%d)\n", nReqId, nDmsCount);
		return ERR_FAIL;
	}

	for (i = 0; i<nDmsCount; i++)
	{
		PL_DMP_DmsInfo_t *pDmsInfo = NULL;
		pDmsItem = HLIB_LIST_GetListItem(HLIB_LIST_First(hDLNA->pDmsHList), i);
		if (pDmsItem != NULL)
		{
			pDmsInfo = (PL_DMP_DmsInfo_t *)HLIB_LIST_Data(pDmsItem);

			__dms_info_dump(pDmsInfo);

			/* Class sorting을 지원하는지 여부 - 디렉토리 우선으로 받기 위함 */
			if( MXDLNA_OK != mxDlnaMscp_supportSortOrder( hDLNA->mscp_handle, pDmsInfo->szUDN, ASC_TITLE ) )
			{
				eFirstSortOrder = NONE_ORD;
			}

			/* Title sorting을 지원하는지 여부 - 알파벳 정렬을 하기 위함 */
			if( MXDLNA_OK != mxDlnaMscp_supportSortOrder( hDLNA->mscp_handle, pDmsInfo->szUDN, ASC_SORT_CLASS ) )
			{
				eSecondSortOrder = NONE_ORD;
			}

			pCdsReq = (PL_DMP_CDSReq_t *)HLIB_STD_MemCalloc(sizeof(PL_DMP_CDSReq_t));
			HxSTD_MemSet(pCdsReq, 0x00, sizeof(PL_DMP_CDSReq_t));
			pCdsReq->bAllDMS = TRUE;
			pCdsReq->nReqId = nReqId;
			HxSTD_StrNCpy( pCdsReq->szUDN, pDmsInfo->szUDN, sizeof(pCdsReq->szUDN)-1 );

			stBrowseArgs.BrowseFlag = MXDLNA_MSCP_BROWSEFLAG_CHILDREN;
			stBrowseArgs.ObjectID = szCID;
			stBrowseArgs.SearchCriteria = pszSearchCriteria;
			stBrowseArgs.UserObject = pCdsReq;
			stBrowseArgs.Filter = "*";
			stBrowseArgs.StartingIndex = nStartIdx;
			stBrowseArgs.RequestedCount = nRequestedNum;

			if (eFirstSortOrder > NONE_ORD)
			{
				stBrowseArgs.SortCriteria = mxDlnaMscp_setOrder( eFirstSortOrder, eSecondSortOrder, NONE_ORD, NONE_ORD );
			}
			else
			{
				if (eSecondSortOrder > NONE_ORD)
				{
					stBrowseArgs.SortCriteria = mxDlnaMscp_setOrder( eSecondSortOrder, NONE_ORD, NONE_ORD, NONE_ORD );
				}
			}

			HxLOG_Print( "[PL_DMP] SearchAll CDS( %s ), CID( %s ), Keyworkd( %s ), SeachCriteria( %s )\n", pDmsInfo->szUDN, stBrowseArgs.ObjectID, szKeyword, stBrowseArgs.SearchCriteria );
			mxDlnaMscp_requestOperation( hDLNA->mscp_handle, pDmsInfo->szUDN, &stBrowseArgs, __search_cb );
		}
	}

	if ( pszSearchCriteria )
	{
		HLIB_STD_MemFree(pszSearchCriteria);
		pszSearchCriteria = NULL;
	}
	return ERR_OK;
}

HERROR 	PL_DMP_GetCurrentItemList(HUINT32 *pCount, CO_MediaInstence_t **pList)
{
	PL_DMP_Context_t 	*hDLNA = __dmp_context();

	PL_DMP_ASSERT(pCount);
	PL_DMP_ASSERT(pList);

	*pCount = hDLNA->nTotalItem;
	*pList = hDLNA->pItemList;

	return ERR_OK;
}

HERROR	PL_DMP_GetCurrentDms(HCHAR **pUdn)
{
	PL_DMP_Context_t 	*hDLNA = __dmp_context();
	*pUdn = hDLNA->stCurrentDMS.szUDN;

	HxLOG_Print( "[PL_DMP] Current UDN(%s)\n", hDLNA->stCurrentDMS.szUDN);
	return ERR_OK;
}

/*  remove object(file)  */
HERROR  PL_DMP_HDRL_DestroyObject(HCHAR *pUdn, HCHAR *pObjectId)
{
	PL_DMP_Context_t 	*hDLNA = __dmp_context();

    if(hDLNA->mscpa_handle == NULL)
    {
        return ERR_FAIL;
    }
    else
    {
        mxDlnaMscp_setUDN(hDLNA->mscpa_handle, pUdn);
        mxDlnaMscp_setCMID(hDLNA->mscpa_handle, 0);
    }

    mxDlnaMscp_destroyObject(hDLNA->mscpa_handle, pObjectId);

    return ERR_OK;
}

HERROR	PL_DMP_MakeUserDeviceCaps(HCHAR *attribute, HCHAR *name, HCHAR *prefix)
{
	PL_DMP_Context_t 		*hDLNA = __dmp_context();

	PL_DMP_ASSERT(attribute);
	PL_DMP_ASSERT(name);
	PL_DMP_ASSERT(prefix);

	HOM_MEMFREE(hDLNA->stDevCap);
	hDLNA->stDevCap = NULL;

	hDLNA->stDevCap = (MXDLNA_USER_DEVICECAP *)HLIB_STD_MemCalloc(sizeof(MXDLNA_USER_DEVICECAP));
	HxSTD_MemSet(hDLNA->stDevCap, 0x0, sizeof(MXDLNA_USER_DEVICECAP));

	/* TODO:NGS - next cap 을 추가 할 수 있도록 lib 변경 예정 */
	/*  mxlib ngs 버전은 추가 되어 있음.  */

	hDLNA->stDevCap->AttributeName		= attribute;
	hDLNA->stDevCap->szAttributeName	= HxSTD_StrLen(attribute);
	hDLNA->stDevCap->Namespace			= name;
	hDLNA->stDevCap->szNamespace		= HxSTD_StrLen(name);
	hDLNA->stDevCap->NamespacePrefix	= prefix;
	hDLNA->stDevCap->szNamespacePrefix	= HxSTD_StrLen(prefix);

	return ERR_OK;
}

HERROR	PL_DMP_RegisterUserDeviceEventResponseSink(PL_DMP_DeviceResponseSink cb)
{
	PL_DMP_Context_t 	*hDLNA = __dmp_context();

	PL_DMP_ASSERT(cb);

	hDLNA->cbDeviceResponse = cb;

	return ERR_OK;
}

void*	PL_DMP_GetMSCPHandle(void)
{
	PL_DMP_Context_t 	*hDLNA = __dmp_context();

    if(hDLNA)
        return hDLNA->mscp_handle;

    return NULL;
}

void*	PL_DMP_GetMSCPAHandle(void)
{
	PL_DMP_Context_t 	*hDLNA = __dmp_context();

    if(hDLNA)
        return hDLNA->mscpa_handle;

    return NULL;
}


/* end od file */
