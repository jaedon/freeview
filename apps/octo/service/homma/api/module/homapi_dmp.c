/**************************************************************
*	@file	homapi_dmp.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			DLNA / DMP
**************************************************************/

/*
* ??2011-2012 Humax Co., Ltd.
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
#include 	<homapi.h>

#define	________________Define_____________________________________
#define CHECK_DMP_MODULE_INIT	do { HxLOG_Warning("\n"); if(dlnadmp_IsInitialized() == FALSE) HOMAPI_DMP_Init(); } while (0)

#define ________________Typedef____________________________________
typedef struct
{
	HBOOL		bInit;

	HUINT32		accesscode;
	HUINT32		notifierId;

	HOMAPI_EventCallback cb;
} dmp_context_t;

typedef struct
{
	HINT32				nDmsCount;
	HOMxDMP_DmsInfo_t 	*pList;
} dmp_list_t;

typedef struct
{
	HBOOL	bIsDeliver;
	HBOOL	bIsDubbing;
} dmp_upload_status_t;

#define ________________Static_Variables___________________________
static  HUINT32     _ulModuleSemaphoreId = 0;

#define ________________Internal_Functions_________________________
static dmp_context_t *__ctx( void )
{
	static dmp_context_t s_manager;
	return &s_manager;
}

static HBOOL	dlnadmp_IsInitialized(void)
{
	dmp_context_t 	*mgr = __ctx();

	if(mgr == NULL)
	{
		HxLOG_Error("mgr == NULL\n");
		return FALSE;
	}

	return mgr->bInit;
}

static HERROR 	dlnadmp_DecodeResultEvent ( HUINT32	cmd, void *data, HUINT32 size)
{
	dmp_context_t 	*mgr = __ctx();
	HERROR			hErr = ERR_FAIL;
	void  			*pResultHead = NULL;
	void			*pResultData = NULL;
	HINT32 			nCount = 0;
	HCHAR			*pRecv = data;

	switch(cmd)
	{
		case eHOMAPI_DMP_Event_DmsAdd:
		case eHOMAPI_DMP_Event_DmsRemove:
			{
				pResultData = HLIB_STD_MemDup(data, size);
				nCount = 1;
			}
			break;

		case eHOMAPI_DMP_Event_CdsListAdd:
			{
				pResultData = HLIB_STD_MemDup(data, size);
				nCount = size/sizeof(HOMxComm_CdsInfo_t);
				HxLOG_Debug("count (%d) \n", nCount);
			}
			break;

		case eHOMAPI_DMP_Event_CdsListEnd:
			{
				nCount = 0;
			}
			break;

		case eHOMAPI_DMP_Event_DmsList:
			{
				//dlnadmp_DecodeJsonToServerList( JDMPObject, &nCount, (APKS_DLNA_DMP_ServerDevice_t **)&pResultData );
			}
			break;

		case eHOMAPI_DMP_Event_DmsRecRst:
			{
				pResultData = HLIB_STD_MemDup(data, size);
				nCount = 1;
			}
			break;

		case eHOMAPI_DMP_Event_DmsDiskInfo:
			{
				pResultData = HLIB_STD_MemDup(data, size);
				nCount = 1;
			}
			break;


		case eHOMAPI_DMP_Event_CdsRemoved:
			{
				pResultData = HLIB_STD_MemDup(data, size);
				nCount = 1;
			}
			break;

		case eHOMAPI_DMP_Event_SearchResult:
			{
				pResultHead = HLIB_STD_MemDup(pRecv, sizeof(HOMxDMP_ContentResult_t));
				pResultData = HLIB_STD_MemDup(pRecv+sizeof(HOMxDMP_ContentResult_t), size-sizeof(HOMxDMP_ContentResult_t));
				nCount = (HINT32)pResultHead;
			}
			break;

		case eHOMAPI_DMP_Event_SearchAllResult:
			{
				pResultHead = HLIB_STD_MemDup(pRecv, sizeof(HOMxDMP_ContentResult_t));
				pResultData = HLIB_STD_MemDup(pRecv+sizeof(HOMxDMP_ContentResult_t), size-sizeof(HOMxDMP_ContentResult_t));
				nCount = (HINT32)pResultHead;
			}
			break;
	}

	if (mgr->cb)
	  mgr->cb( (HUINT32)cmd, (HUINT32)pResultData, (HUINT32)nCount ); // mgr->cb is ___event_cb in apk_dlna_dmp.c

	HOMAPI_MEMFREE(pResultData);
	HOMAPI_MEMFREE(pResultHead);

	return hErr;
}

static HBOOL dlnadmp_QueryResultEventHandler( HUINT32 cmd, void *param, HUINT32 size, void *userdata )
{
	return  dlnadmp_DecodeResultEvent( cmd, param, size);
}

static	HUINT32	dlnadmp_Notifier(HUINT32 accesscode, HUINT32 cmd, void *param, HUINT32 size)
{
	HUINT32			ret = 0;
	dmp_context_t 	*mgr = __ctx();

	HOMAPI_SEM_GET;

	if ( NULL == param )
	{
		ret = HOMAPI_Query( eHOMAPI_Service_DMP, NULL, mgr, (HBOOL (*)(HUINT32, void *, HUINT32, void *))dlnadmp_QueryResultEventHandler );
	}
	else
	{
		dlnadmp_DecodeResultEvent(cmd, param, size);
	}

	if(ret == 0)
	{
		HxLOG_Debug("not connected to homma(%d) \n", ret);
		HOMAPI_SEM_RELEASE;
		return 0;
	}

	if ( mgr->accesscode != accesscode )
	{
		HxLOG_Debug("accesscode in mgr (%d) not equal to cur accesscode (%d) \n", mgr->accesscode, accesscode);
	}

	HOMAPI_SEM_RELEASE;

	return 0;
}

static	HERROR	dlnadmp_RegisterNotifier( void )
{
	dmp_context_t *mgr = __ctx();
	HUINT32 	accesscode = 0;

	if(mgr == NULL)
	{
		HxLOG_Error("mgr == NULL\n");
		return ERR_FAIL;
	}

	mgr->bInit = FALSE;
	accesscode = HOMAPI_Query( eHOMAPI_Service_DMP, NULL, NULL,  NULL );

	if( accesscode )
	{
		mgr->accesscode = accesscode;
		mgr->notifierId = HOMAPI_ConnectNotifier( accesscode, dlnadmp_Notifier );
		mgr->bInit = TRUE;

		return ERR_OK;
	}

	return ERR_FAIL;
}

static	void	dlnadmp_UnRegisterNotifier( void )
{
	dmp_context_t *mgr = __ctx();

	if ( mgr )
		mgr->notifierId = HOMAPI_DisconnectNotifier( mgr->accesscode, mgr->notifierId );
}

static HBOOL    __dmp_querydata_decoder (HUINT32 cmd, void *data, HUINT32 size, void *pData)
{
	switch(cmd)
	{
		case eHOMAPI_DMP_Command_DmsList:
			{
				dmp_list_t *pCdsListInfo = (dmp_list_t *)pData;
				HINT32 count = 0;
				HCHAR *pRecv = data;

				HxSTD_MemCopy( &count, pRecv, sizeof(HINT32));
				HxLOG_Debug("### Get DMS List(%d) ### \n", count);
				if ( count )
				{
					pCdsListInfo->pList = HLIB_STD_MemCalloc(sizeof(HOMxDMP_DmsInfo_t) * count);
					HxSTD_MemCopy( pCdsListInfo->pList, pRecv+sizeof(HINT32),
										sizeof(HOMxDMP_DmsInfo_t) * count );
				}
				pCdsListInfo->nDmsCount = count;
			}
			break;

		case eHOMAPI_DMP_Command_CdsRemove:
			{
				HxLOG_Debug("### CDS Removed Result ### \n");
			}
			break;

		case eHOMAPI_DMP_Command_DmsUploadStatus:
			{
				HxLOG_Debug("### Upload Status Result ### \n");
			}
			break;

		default:
			HxLOG_Error("Error> unsupported command(%d)\n", cmd);
			break;
	}

	return TRUE;
}

#define ________________Public_Interfaces__________________________
void	HOMAPI_DMP_Init( void )
{
#if !defined(CONFIG_HOMMA_DMP)
	HxLOG_Error("### (HOMAPI_DMP_Init) Not Support DLNA DMP ###\n");
	return;
#else
	if(_ulModuleSemaphoreId == 0)
	{
		HxSEMT_Create(&_ulModuleSemaphoreId, "sadlnadmp", HxSEMT_FIFO);
	}

	if(dlnadmp_RegisterNotifier() != ERR_OK)
	{
		HxLOG_Error("[HOMAPI_DMP_Init] dlnadmp_RegisterNotifier() fail!\n");
	}
#endif
}

void	HOMAPI_DMP_DeInit( void )
{
#if !defined(CONFIG_HOMMA_DMP)
	HxLOG_Error("### (HOMAPI_DMP_DeInit) Not Support DLNA DMP ###\n");
	return;
#else
	dlnadmp_UnRegisterNotifier();

	if(_ulModuleSemaphoreId != 0)
	{
		HxSEMT_Destroy(_ulModuleSemaphoreId);
		_ulModuleSemaphoreId = 0;
	}
#endif
}

void	HOMAPI_DMP_RegisterNotifier(HOMAPI_EventCallback pfnObjectHandler)
{
#if !defined(CONFIG_HOMMA_DMP)
	HxLOG_Error("### (HOMAPI_DMP_RegisterNotifier) Not Support DLNA DMP ###\n");
	return;
#else
	dmp_context_t *mgr = __ctx();
	mgr->cb = pfnObjectHandler;
#endif
}

void	HOMAPI_DMP_UnRegisterNotifier( void )
{
#if !defined(CONFIG_HOMMA_DMP)
	HxLOG_Error("### (HOMAPI_DMP_UnRegisterNotifier) Not Support DLNA DMP ###\n");
	return;
#else
	dmp_context_t * mgr = __ctx();
	mgr->cb = NULL;
#endif
}

HERROR	HOMAPI_DMP_Start( void )
{
#if !defined(CONFIG_HOMMA_DMP)
	HxLOG_Error("### (HOMAPI_DMP_Start) Not Support DLNA DMP ###\n");
	return ERR_FAIL;
#else
	HOMxCommData_t data;

	CHECK_DMP_MODULE_INIT;

	data.cmd = eHOMAPI_DMP_Command_Start;
	data.data = NULL;
	data.size = 0;

	HOMAPI_Call( eHOMAPI_Service_DMP, &data);
	return ERR_OK;
#endif
}

HERROR	HOMAPI_DMP_Stop( void )
{
#if !defined(CONFIG_HOMMA_DMP)
	HxLOG_Error("### (HOMAPI_DMP_Stop) Not Support DLNA DMP ###\n");
	return ERR_FAIL;
#else
	HOMxCommData_t data;

	CHECK_DMP_MODULE_INIT;

	data.cmd = eHOMAPI_DMP_Command_Stop;
	data.data = NULL;
	data.size = 0;

	HOMAPI_SEM_GET;

	HOMAPI_Call( eHOMAPI_Service_DMP, &data);

	HOMAPI_SEM_RELEASE;

	return ERR_OK;
#endif
}

HERROR	HOMAPI_DMP_getListOfDMS( HUINT32 *pCount, HOMxDMP_DmsInfo_t **pResultData )
{
#if !defined(CONFIG_HOMMA_DMP)
	HxLOG_Error("### (HOMAPI_DMP_getListOfDMS) Not Support DLNA DMP ###\n");
	return ERR_FAIL;
#else
	HUINT32     accessCode = 0;
	dmp_list_t 	tResult = { 0 };
	HOMxCommData_t data;

	CHECK_DMP_MODULE_INIT;

	*pCount = 0;

	data.cmd = eHOMAPI_DMP_Command_DmsList;
	data.data = NULL;
	data.size = 0;

	HOMAPI_SEM_GET;

	accessCode = HOMAPI_Query( eHOMAPI_Service_DMP, (void *)&data , &tResult, __dmp_querydata_decoder);

	if(!accessCode)
	{
		HxLOG_Error("### (getListOfDMS) Error> Cant get DMS list ###\n");
		HOMAPI_SEM_RELEASE;
		return ERR_FAIL;
	}

	*pResultData = (HOMxDMP_DmsInfo_t*)tResult.pList;
	*pCount = tResult.nDmsCount;

	HOMAPI_SEM_RELEASE;

	return ERR_OK;
#endif
}

HERROR	HOMAPI_DMP_BrowsingChildItem( const HCHAR *udn, const HCHAR *cid, const HCHAR *sort, HINT32 startIdx, HINT32 nRequest)
{
#if !defined(CONFIG_HOMMA_DMP)
	HxLOG_Error("### (HOMAPI_DMP_BrowsingChildItem) Not Support DLNA DMP ###\n");
	return ERR_FAIL;
#else
	HOMxCommData_t data;
	HOMxDMP_Args_Browse_t stSearchTerms;

	CHECK_DMP_MODULE_INIT;

	HxSTD_MemSet( &stSearchTerms, 0, sizeof(HOMxDMP_Args_Browse_t) );

	HLIB_MATH_EncodeBase64( stSearchTerms.szCID, cid, HxSTD_StrLen(cid)+1 );
	HLIB_STD_StrUtf8NCpy( stSearchTerms.szUDN, udn, sizeof(stSearchTerms.szUDN) );
	HLIB_STD_StrUtf8NCpy( stSearchTerms.szSort, sort, sizeof(stSearchTerms.szSort) );
	stSearchTerms.nStartIdx= startIdx;
	stSearchTerms.nRequestNum = nRequest;

	data.cmd = eHOMAPI_DMP_Command_Browse;
	data.data = (void *)&stSearchTerms;
	data.size = sizeof(HOMxDMP_Args_Browse_t);

	HOMAPI_SEM_GET;

	HOMAPI_Call( eHOMAPI_Service_DMP, &data);

	HOMAPI_SEM_RELEASE;

	return ERR_OK;
#endif
}

HERROR	HOMAPI_DMP_SearchingItem( const HCHAR *udn, const HCHAR *cid, const HCHAR *category, const HCHAR *keyword, const HCHAR *sort, HINT32 startIdx, HINT32 nRequest, HINT32 reqId )
{
#if !defined(CONFIG_HOMMA_DMP)
	HxLOG_Error("### (HOMAPI_DMP_SearchingItem) Not Support DLNA DMP ###\n");
	return ERR_FAIL;
#else
	HOMxCommData_t data;
	HOMxDMP_Args_Search_t stSearchTerms;

	CHECK_DMP_MODULE_INIT;

	HxSTD_MemSet( &stSearchTerms, 0, sizeof(HOMxDMP_Args_Search_t) );

	HLIB_MATH_EncodeBase64( stSearchTerms.szCID, cid, HxSTD_StrLen(cid)+1 );
	HLIB_STD_StrUtf8NCpy( stSearchTerms.szUDN, udn, sizeof(stSearchTerms.szUDN) );
	HLIB_STD_StrUtf8NCpy( stSearchTerms.szSort, sort, sizeof(stSearchTerms.szSort) );
	HLIB_STD_StrUtf8NCpy( stSearchTerms.szCategory, category, sizeof(stSearchTerms.szCategory) );
	HLIB_STD_StrUtf8NCpy( stSearchTerms.szKeyword, keyword, sizeof(stSearchTerms.szKeyword) );
	stSearchTerms.nStartIdx		= startIdx;
	stSearchTerms.nRequestNum	= nRequest;
	stSearchTerms.nRequestId	= reqId;

	data.cmd = eHOMAPI_DMP_Command_Search;
	data.data = (void *)&stSearchTerms;
	data.size = sizeof(HOMxDMP_Args_Search_t);

	HOMAPI_SEM_GET;

	HOMAPI_Call( eHOMAPI_Service_DMP, &data);

	HOMAPI_SEM_RELEASE;

	return ERR_OK;
#endif
}

HERROR	HOMAPI_DMP_SearchAll(const HCHAR *cid, const HCHAR *category, const HCHAR *keyword, const HCHAR *sort, HINT32 reqId )
{
#if !defined(CONFIG_HOMMA_DMP)
	HxLOG_Error("### (HOMAPI_DMP_SearchAll) Not Support DLNA DMP ###\n");
	return ERR_FAIL;
#else
	HOMxCommData_t data;
	HOMxDMP_Args_SearchAll_t stSearchAll;

	CHECK_DMP_MODULE_INIT;

	HxSTD_MemSet( &stSearchAll, 0, sizeof(HOMxDMP_Args_SearchAll_t) );

	HLIB_MATH_EncodeBase64( stSearchAll.szCID, cid, HxSTD_StrLen(cid)+1 );
	HLIB_STD_StrUtf8NCpy( stSearchAll.szSort, sort, sizeof(stSearchAll.szSort) );
	HLIB_STD_StrUtf8NCpy( stSearchAll.szCategory, category, sizeof(stSearchAll.szCategory) );
	HLIB_STD_StrUtf8NCpy( stSearchAll.szKeyword, keyword, sizeof(stSearchAll.szKeyword) );
	stSearchAll.nRequestId	= reqId;

	data.cmd = eHOMAPI_DMP_Command_SearchAll;
	data.data = (void *)&stSearchAll;
	data.size = sizeof(HOMxDMP_Args_SearchAll_t);

	HOMAPI_SEM_GET;

	HOMAPI_Call( eHOMAPI_Service_DMP, &data);

	HOMAPI_SEM_RELEASE;
	return ERR_OK;
#endif
}

HERROR	HOMAPI_DMP_GetRecordDestinations(const HCHAR *udn)
{
#if !defined(CONFIG_HOMMA_DMP)
	HxLOG_Error("### (HOMAPI_DMP_GetRecordDestinations) Not Support DLNA DMP ###\n");
	return ERR_FAIL;
#else
	HOMxCommData_t data;
	HOMxDMP_Args_DmsRecDst_t	arg;

	CHECK_DMP_MODULE_INIT;

	HxSTD_MemSet(&arg, 0, sizeof(HOMxDMP_Args_DmsRecDst_t));
	HxSTD_snprintf(arg.szUDN, sizeof(arg.szUDN), "%s", udn);

	data.cmd = eHOMAPI_DMP_Command_DmsRecDst;
	data.data = (void *)&arg;
	data.size = sizeof(HOMxDMP_Args_DmsRecDst_t);

	HOMAPI_SEM_GET;

	HOMAPI_Call( eHOMAPI_Service_DMP, &data);

	HOMAPI_SEM_RELEASE;

	return ERR_OK;
#endif
}

HERROR	HOMAPI_DMP_GetRecordDiskInfo( const HCHAR *udn, const HCHAR *id)
{
#if !defined(CONFIG_HOMMA_DMP)
	HxLOG_Error("### (HOMAPI_DMP_GetRecordDiskInfo) Not Support DLNA DMP ###\n");
	return ERR_FAIL;
#else
	HOMxCommData_t data;
	HOMxDMP_Args_DmsDiskInfo_t	arg;

	CHECK_DMP_MODULE_INIT;

	HxSTD_MemSet(&arg, 0, sizeof(HOMxDMP_Args_DmsDiskInfo_t));
	HxSTD_snprintf(arg.szUDN, sizeof(arg.szUDN), "%s", udn);
	HxSTD_snprintf(arg.szID, sizeof(arg.szID), "%s", id);

	data.cmd = eHOMAPI_DMP_Command_DmsDiskInfo;
	data.data = (void *)&arg;
	data.size = sizeof(HOMxDMP_Args_DmsDiskInfo_t);

	HOMAPI_SEM_GET;

	HOMAPI_Call( eHOMAPI_Service_DMP, &data);

	HOMAPI_SEM_RELEASE;

	return ERR_OK;
#endif
}

HERROR	HOMAPI_DMP_RefreshDms( const HCHAR *pUdn )
{
#if !defined(CONFIG_HOMMA_DMP)
	HxLOG_Error("### (HOMAPI_DMP_RefreshDms) Not Support DLNA DMP ###\n");
	return ERR_FAIL;
#else
	HOMxCommData_t data;
	HOMxDMP_Args_DmsRefresh_t	arg;

	CHECK_DMP_MODULE_INIT;

	HxSTD_MemSet(&arg, 0, sizeof(HOMxDMP_Args_DmsRefresh_t));
	HxSTD_snprintf(arg.szUDN, sizeof(arg.szUDN), "%s", pUdn);

	data.cmd = eHOMAPI_DMP_Command_DmsRefresh;
	data.data = (void *)&arg;
	data.size = sizeof(HOMxDMP_Args_DmsRefresh_t);

	HOMAPI_SEM_GET;

	HOMAPI_Call( eHOMAPI_Service_DMP, &data);

	HOMAPI_SEM_RELEASE;

	return ERR_OK;
#endif
}

HERROR HOMAPI_DMP_RemoveItem( const HCHAR *udn, const HCHAR *id, char **retval)
{
#if !defined(CONFIG_HOMMA_DMP)
	HxLOG_Error("### (HOMAPI_DMP_RemoveItem) Not Support DLNA DMP ###\n");
	return ERR_FAIL;
#else
	HUINT32     accessCode = 0;
	HINT32		pszResult = -1;
	HCHAR		szRet[1024] = {0, };

	HOMxCommData_t data;
	HOMxDMP_Args_CdsRemove_t arg;

	CHECK_DMP_MODULE_INIT;

	HxLOG_Assert(retval && udn && id);

	HxSTD_MemSet(&arg, 0, sizeof(HOMxDMP_Args_CdsRemove_t));
	HxSTD_snprintf(arg.szUDN, sizeof(arg.szUDN), "%s", udn);
	HxSTD_snprintf(arg.szCID, sizeof(arg.szCID), "%s", id);

	data.cmd = eHOMAPI_DMP_Command_CdsRemove;
	data.data = (void *)&arg;
	data.size = sizeof(HOMxDMP_Args_CdsRemove_t);

	HOMAPI_SEM_GET;

	accessCode = HOMAPI_Query( eHOMAPI_Service_DMP, (void *)&data, &pszResult, __dmp_querydata_decoder);

	if(!accessCode)
	{
		HOMAPI_SEM_RELEASE;
		return ERR_FAIL;
	}

	if(pszResult != ERR_FAIL)
	{
		HxSTD_snprintf( szRet, sizeof(szRet), "%s&%s", udn,id);
		*retval = HLIB_STD_StrDup(szRet);
	}

	HOMAPI_SEM_RELEASE;

	return ERR_OK;
#endif
}

HERROR	HOMAPI_DMP_GetIsIPDubbing(HBOOL *val)
{
#if !defined(CONFIG_HOMMA_DMP)
	HxLOG_Error("### (HOMAPI_DMP_GetIsIPDubbing) Not Support DLNA DMP ###\n");
	return ERR_FAIL;
#else
	HUINT32     		accessCode = 0;
	dmp_upload_status_t	tResult;
	HOMxCommData_t 		data;

	CHECK_DMP_MODULE_INIT;

	data.cmd = eHOMAPI_DMP_Command_DmsUploadStatus;
	data.data = NULL;
	data.size = 0;

	HOMAPI_SEM_GET;

	accessCode = HOMAPI_Query( eHOMAPI_Service_DMP, (void*)&data, &tResult, __dmp_querydata_decoder);

	if(!accessCode)
	{
		HOMAPI_SEM_RELEASE;
		return ERR_FAIL;
	}

	*val = tResult.bIsDubbing;

	HOMAPI_SEM_RELEASE;

	return ERR_OK;
#endif
}
