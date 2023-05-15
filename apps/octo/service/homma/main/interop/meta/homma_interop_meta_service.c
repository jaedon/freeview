/**************************************************************
*	@file	homma_interop.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			HOMMA / interoperation supporting instead
*					of using APK
**************************************************************/

/*
* â“’ 2011-2012 Humax Co., Ltd.
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

/* ------------------------------------------------------------- //
 * Author : tjjang
 * Description : This is for replacing apk methods
 *  - HOMMA_INTEROP prefix is used for external methods
 *  - s_HOMMA_INTEROP prefix is used to represent static methods
 */

#include <homma_interop.h>
#include <clib.h>

/*
 * methods
 *  - static & external
 */

#ifdef HOMMA_USE_APPKIT_DIRECTLY
#else
#define	hintopMetaSvc_MAX_TABLE_NUM 		3	// DxService_t, DxTransponder_t, DxNetwork_t

#define	ENTER_CRITICAL_HINTOPMETASVC		do { HxSEMT_Get(s_ulhintopMetaSvcSemId); } while (0)
#define	LEAVE_CRITICAL_HINTOPMETASVC		do { HxSEMT_Release(s_ulhintopMetaSvcSemId); } while (0)

#define	RETURN_IF_VOID(expr)	do{if(expr)return;}while(0)
#define	RETURN_IF(expr, err)	do{if(expr)return (err);}while(0)
#define	GOTO_IF(expr, pos)		do{if(expr)goto pos;}while(0)

typedef union
{
	struct common{
		HINT32		uid;
		const HCHAR	*typeOf;
		HINT32		version;
	} common;

	DxService_t			stService;
	DxTransponder_t		stTransponder;
	DxNetwork_t			stNetwork;
} hintopMetaSvc_Type_t;

typedef struct
{
	HCHAR	*pszName;
	HUINT32	ulSize;
} hintopMetaSvc_DxInfo_t;

typedef struct
{
#if 0
	HBOOL			bMainApp;
	HxTREE_t			*pstListTree;		//< <HxVector_t *>  cache data
	HxTREE_t			*pstTrashTree;
#if defined(APKMETASVC_USE_VCLONE)
	HxTREE_t			*pstVCloneTree;
#endif
	HINT32				nVersion;		//< last version
	apkMetaSvc_Notifier_t	pfNotifier;
	APK_MetaUpdateChecker	pfChecker;
#ifdef	apkMetaSvc_CHLOGO
	HxTREE_t			*pstChLogoMap;
#endif
#else
	HxVector_t		*pstSvcVector;
	HxVector_t		*pstTpVector;
	HxVector_t		*pstNetVector;
	HxTREE_t		*pstChLogoMap;
	HxList_t		*pstGroupList;

	HUINT32			nSvcNotifierId;
	HUINT32			nTpNotifierId;
	HUINT32			nNetNotifierId;
	HUINT32			nChLogoNotifierId;
	HUINT32			nGroupListNotifierId;
	hintopMetaSvc_Notifier_t	fnNotifier;
#endif
} hintopMetaSvc_Ctx_t;




const STATIC 	hintopMetaSvc_DxInfo_t		cs_astDxInfo[hintopMetaSvc_MAX_TABLE_NUM] = 	{
																{DxNAMEOF(DxService_t), sizeof(DxService_t)},
																{DxNAMEOF(DxTransponder_t), sizeof(DxTransponder_t)},
																{DxNAMEOF(DxNetwork_t), sizeof(DxNetwork_t)}
															};

static HUINT32					s_ulhintopMetaSvcSemId;


static HINT32	homma_interop_meta_svc_CompareVectorObject (const void *pvArg1, const void *pvArg2);
static HERROR	homma_interop_meta_service_Load (void);
static HERROR	homma_interop_meta_transponder_Load (void);
static HERROR	homma_interop_meta_network_Load (void);
#endif

/* ############################################################################
                  _
                 | |
 _ __ ___    ___ | |_  __ _
| '_ ` _ \  / _ \| __|/ _` |
| | | | | ||  __/| |_| (_| |
|_| |_| |_| \___| \__|\__,_|
 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx  meta record & antenna & svn  xxxxxxxxx
############################################################################ */


// unwilling to use apps/octo/appkit/
#ifdef HOMMA_USE_APPKIT_DIRECTLY

HBOOL HOMMA_INTEROP_META_SVC_Init (HBOOL bMainApp)
{
	return APK_META_SVC_Init(bMainApp);
}

HBOOL HOMMA_INTEROP_META_SVC_DeInit (void)
{
	return APK_META_SVC_DeInit();
}

DxTransponder_t * HOMMA_INTEROP_META_SVC_GetTransponder (HINT32 nUid)
{
	return APK_META_SVC_GetTransponder(nUid);
}

DxNetwork_t * HOMMA_INTEROP_META_SVC_GetNetwork (HINT32 nUid)
{
	return APK_META_SVC_GetNetwork(nUid);
}

DxChannelLogo_t * HOMMA_INTEROP_META_SVC_GetLogoUrl(HINT32 nUid, DxChannelLogo_t *pstBuf)
{
	return APK_META_SVC_GetLogoUrl(nUid, pstBuf);
}

HxVector_t * HOMMA_INTEROP_META_SVC_GetServiceList (void)
{
	return APK_META_SVC_GetServiceList();
}

DxService_t * HOMMA_INTEROP_META_SVC_GetService (HINT32 nUid)
{
	return APK_META_SVC_GetService(nUid);
}

HERROR HOMMA_INTEROP_META_SVC_SetListener (HOMMA_INTEROP_MetaService_Notifier_t pfOnUpdate, void *pvUserdata)
{
	return APK_META_SVC_SetListener(pfOnUpdate, pvUserdata);
}

HERROR HOMMA_INTEROP_META_SVC_LoadupdatedList (void)
{
	return APK_META_SVC_LoadupdatedList();
}

void HOMMA_INTEROP_META_SVC_Delete (void *pvObject)
{
	APK_META_SVC_Delete(pvObject);
}

#if 0 // have to check ---> meta schedule
//APK_META_SCHEDULE_AddListener()
void HOMMA_INTEROP_META_SCHEDULE_AddListener (HOMMA_INTEROP_MetaSched_Handle_t hMeta, HOMMA_INTEROP_MetaSched_Listener_t listener, void *userdata)
{
	APK_META_SCHEDULE_AddListener(hMeta, listener, userdata);
}
#endif

#else // #ifdef HOMMA_USE_APPKIT_DIRECTLY else

#define __________________INTERNAL_FUNCTION__________________
static hintopMetaSvc_Ctx_t *	homma_interop_meta_svc_GetContext (void)
{
	static hintopMetaSvc_Ctx_t		s_stManager;

	if (s_stManager.pstSvcVector == NULL)
	{
		s_stManager.pstSvcVector = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))HLIB_STD_MemFree_CB, homma_interop_meta_svc_CompareVectorObject);
		HxLOG_Assert(s_stManager.pstSvcVector != NULL);
	}

	if (s_stManager.pstTpVector == NULL)
	{
		s_stManager.pstTpVector = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))HLIB_STD_MemFree_CB, homma_interop_meta_svc_CompareVectorObject);
		HxLOG_Assert(s_stManager.pstTpVector != NULL);
	}

	if (s_stManager.pstNetVector == NULL)
	{
		s_stManager.pstNetVector = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))HLIB_STD_MemFree_CB, homma_interop_meta_svc_CompareVectorObject);
		HxLOG_Assert(s_stManager.pstNetVector != NULL);
	}

	return &s_stManager;
}

static HxVector_t * homma_interop_meta_svc_GetObject(hintopMetaSvc_Ctx_t *ctx, const HCHAR *pszName)
{
	HxVector_t *pstList = NULL;

	if (HxSTD_StrCmp(pszName, DxNAMEOF(DxService_t)) == 0)
	{
		pstList = ctx->pstSvcVector;
	}
	else if(HxSTD_StrCmp(pszName, DxNAMEOF(DxTransponder_t)) == 0)
	{
		pstList = ctx->pstTpVector;
	}
	else if(HxSTD_StrCmp(pszName, DxNAMEOF(DxNetwork_t)) == 0)
	{
		pstList = ctx->pstNetVector;
	}
	else
	{
		// unknown..
	}

	return pstList;
}

static hintopMetaSvc_DxInfo_t * homma_interop_meta_svc_GetDxInfo(const HCHAR *pszName)
{
	HUINT32		ulIdx;

	for (ulIdx = 0 ; ulIdx < hintopMetaSvc_MAX_TABLE_NUM ; ulIdx++)
	{
		if (HxSTD_StrCmp(pszName, cs_astDxInfo[ulIdx].pszName) == 0)
		{
			return (hintopMetaSvc_DxInfo_t *)&cs_astDxInfo[ulIdx];
		}
	}

	return NULL;
}

static void *	homma_interop_meta_svc_NewObject (const hintopMetaSvc_DxInfo_t *pstDxInfo)
{
	hintopMetaSvc_Type_t	*pstSelf;

	pstSelf = (hintopMetaSvc_Type_t *)HLIB_STD_MemCalloc(pstDxInfo->ulSize);
	if (pstSelf)
	{
		pstSelf->common.typeOf = pstDxInfo->pszName;
	}
	return (void *)pstSelf;
}

static void	*	homma_interop_meta_svc_CloneObject (const void *pvSelf)
{
	hintopMetaSvc_Type_t    *pstThiz = (hintopMetaSvc_Type_t *)pvSelf;
	hintopMetaSvc_DxInfo_t * pstDxInfo;
	hintopMetaSvc_Type_t    *pstClone;

	RETURN_IF(pvSelf == NULL, NULL);

	HxLOG_Assert(pstThiz->common.typeOf);

	pstDxInfo = homma_interop_meta_svc_GetDxInfo(pstThiz->common.typeOf);

	pstClone = (hintopMetaSvc_Type_t *)HLIB_STD_MemCalloc(pstDxInfo->ulSize);
	if (pstClone)
	{
		memcpy(pstClone, pvSelf, pstDxInfo->ulSize);
		pstClone->common.typeOf = pstThiz->common.typeOf;
	}
	return pstClone;
}

static void		homma_interop_meta_svc_DeleteObject (void *pvSelf)
{
	hintopMetaSvc_Type_t	*pstThiz = (hintopMetaSvc_Type_t *)pvSelf;
	if (pstThiz)
	{
		HxLOG_Assert(pstThiz->common.typeOf);
		HLIB_STD_MemFree(pstThiz);
	}
}

static HINT32	homma_interop_meta_svc_CompareVectorObject (const void *pvArg1, const void *pvArg2)
{
	const void *pvObj1 = *(const void **)pvArg1;
	const void *pvObj2 = *(const void **)pvArg2;

	return (HINT32)memcmp(pvObj1, pvObj2, sizeof(HINT32)/* uid field */);
}

static HBOOL	homma_interop_meta_svc_RemoveObject (hintopMetaSvc_Ctx_t *ctx, const HCHAR *type, const void *key)
{
	HxVector_t	*pstList = NULL;

	pstList = homma_interop_meta_svc_GetObject(ctx, type);
	RETURN_IF(pstList == NULL, FALSE);

	if (HLIB_VECTOR_Remove(pstList, HLIB_VECTOR_BSearch(pstList, key, NULL)) != ERR_OK)
	{
		return FALSE;
	}

	return TRUE;
}

static void *	homma_interop_meta_svc_FindObject (hintopMetaSvc_Ctx_t *ctx, const HCHAR *type, const void *key)
{
	HxVector_t	*pstList = NULL;

	pstList = homma_interop_meta_svc_GetObject(ctx, type);
	RETURN_IF(pstList == NULL, NULL);

	return HLIB_VECTOR_ItemAt(pstList, HLIB_VECTOR_BSearch(pstList, key, NULL));
}

static void *	homma_interop_meta_svc_ReplaceObject (hintopMetaSvc_Ctx_t *ctx, const HCHAR *pszType, const void *newObj)
{
	HxVector_t	*pstList = NULL;
	HINT32		nIdx;
	void		*pvOld, *pvRet;
	hintopMetaSvc_DxInfo_t *pstDxInfo;

	pstList = homma_interop_meta_svc_GetObject(ctx, pszType);
	RETURN_IF(pstList == NULL, NULL);

	pstDxInfo = homma_interop_meta_svc_GetDxInfo(pszType);
	RETURN_IF(pstDxInfo == NULL, NULL);

	nIdx = HLIB_VECTOR_BSearch(pstList, newObj, NULL);
	RETURN_IF(nIdx < 0, NULL);

	pvOld = HLIB_VECTOR_ItemAt(pstList, nIdx);
	RETURN_IF(pvOld == NULL, NULL);
	RETURN_IF(memcmp(pvOld, newObj, pstDxInfo->ulSize) == 0, NULL);

	pvRet= homma_interop_meta_svc_CloneObject(newObj);
	RETURN_IF(pvRet == NULL, NULL);

	pvOld = HLIB_VECTOR_Replace(pstList, nIdx, pvRet);
	if (pvOld)
	{
		homma_interop_meta_svc_DeleteObject(pvOld);
	}
	return pvRet;
}

static HINT32	homma_interop_meta_svc_CompareINT32 (const void *pvA , const void *pvB)
{
	return (HINT32)pvA - (HINT32)pvB;
}

static HERROR	homma_interop_meta_svc_Reset (const HCHAR *pszName)
{
	HxVector_t		*pstList = NULL;
	HERROR	hErr = ERR_FAIL;

	HxLOG_Trace();

	ENTER_CRITICAL_HINTOPMETASVC;
	pstList = homma_interop_meta_svc_GetObject(homma_interop_meta_svc_GetContext(), pszName);
	if (pstList)
	{
		if (HLIB_VECTOR_RemoveAll(pstList) == ERR_OK)
		{
			HxLOG_Warning("[%s] [%d]-------RESET Table : %s -------\n",__FUNCTION__,__LINE__, pszName);
			hErr = ERR_OK;
		}
	}
	LEAVE_CRITICAL_HINTOPMETASVC;

	return hErr;
}

#define __________________SERVICE_METADATA_RECEIVED__________________
static void homma_interop_meta_svc_OnDataReceived(const HCHAR *pszName, HUID nUid, const void *pvData, HUINT32 ulSize, void **ppvArgs)
{
	HxVector_t				*pstVector;
	hintopMetaSvc_DxInfo_t	*pstDxInfo;
	void					*pvObj;
	DAxIterator_t			stIter;
	hintopMetaSvc_Type_t    *pstThiz;

	RETURN_IF_VOID(ulSize == 0 || pvData == NULL);

	pstDxInfo = homma_interop_meta_svc_GetDxInfo(pszName);
	if (pstDxInfo == NULL)
	{
		HxLOG_Error("[%s] [%d] error \n",__FUNCTION__,__LINE__);
		return;
	}

	pstVector  = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))homma_interop_meta_svc_DeleteObject, homma_interop_meta_svc_CompareVectorObject);
	RETURN_IF_VOID(pstVector == NULL);

	DAPI_InitIterator(&stIter, pvData, ulSize, pstDxInfo->ulSize);
	while (DAPI_NextIterator(&stIter))
	{
		pvObj = homma_interop_meta_svc_NewObject(pstDxInfo);
		if (pvObj == NULL)
			continue;

		memcpy(pvObj, stIter.data, pstDxInfo->ulSize);
		pstThiz = (hintopMetaSvc_Type_t *)pvObj;
		pstThiz->common.typeOf = pstDxInfo->pszName;

		HLIB_VECTOR_Add(pstVector, pvObj);
	}

	if (HLIB_VECTOR_Length(pstVector) == 0)
	{
		HxLOG_Error("[%s] [%d] empty \n",__FUNCTION__,__LINE__);
		HLIB_VECTOR_Delete(pstVector);
		return;
	}
	HLIB_VECTOR_Sort(pstVector, NULL);

	HxLOG_Print("[%s] [%d]  \n",__FUNCTION__,__LINE__);

	ppvArgs[0] = (void *)pstVector;
	(void)nUid;
}

static void	homma_interop_meta_svc_OnChLogoReceived(const HCHAR *pszName, HUID nUid, const void *pvData, HUINT32 ulSize, void **ppvArgs)
{
	HxTREE_t 		*pstMap;
	DAxIterator_t pvIter;
	DxChannelLogo_t	*pstDxLogo;
	hintopMetaSvc_Ctx_t * ctx;

	ctx = homma_interop_meta_svc_GetContext();

	RETURN_IF_VOID(ulSize == 0 || pvData == NULL);

	pstMap = HLIB_TREE_NewFull((HxCompareDataFunc)homma_interop_meta_svc_CompareINT32, NULL, NULL, (HxDestroyNotify)HLIB_STD_MemFree_CB);
	RETURN_IF_VOID(pstMap == NULL);

	DAPI_InitIterator(&pvIter, pvData, ulSize, sizeof(DxChannelLogo_t));
	while (DAPI_NextIterator(&pvIter))
	{
		pstDxLogo = (DxChannelLogo_t *)pvIter.data;
		if(pstDxLogo == NULL)
		{
			continue;
		}
		HLIB_TREE_Insert(pstMap, (void *)pstDxLogo->svcuid, HLIB_STD_MemDup(pstDxLogo, sizeof(DxChannelLogo_t)));

	}

	if (HLIB_TREE_Nnodes(pstMap) == 0)
	{
		HxLOG_Error("[%s] [%d] empty \n",__FUNCTION__,__LINE__);
		HLIB_TREE_Destroy(pstMap);
		pstMap = NULL;
	}

#if 0
	ppvArgs[0] = (void*)pstMap;
#else
	ENTER_CRITICAL_HINTOPMETASVC;
	if (ctx->pstChLogoMap)
	{
		HLIB_TREE_Destroy(ctx->pstChLogoMap);
	}

	ctx->pstChLogoMap = pstMap;
	LEAVE_CRITICAL_HINTOPMETASVC;

	HxLOG_Print("[%s] Load DxChannelLogo_t from DAMA success(%d)\n", __FUNCTION__, HLIB_TREE_Nnodes(ctx->pstChLogoMap));
#endif
	(void)nUid;
}

static void	homma_interop_meta_svc_OnGroupListReceived(const HCHAR *pszName, HUID nUid, const void *pvData, HUINT32 ulSize, void **ppvArgs)
{
	DxGroupList_t 	*pstDxGroupList = NULL;
	HxVector_t		*pstGroupList = NULL;
	CListItem_t		*pstListItem = NULL;
	HxList_t		*pstLcnList = NULL;
	DAxIterator_t	 pvIter;
	HINT32 i = 0;
	hintopMetaSvc_Ctx_t * ctx;

	ctx = homma_interop_meta_svc_GetContext();

	RETURN_IF_VOID(ulSize == 0 || pvData == NULL);

	DAPI_InitIterator(&pvIter, pvData, ulSize, 0);
	while( DAPI_NextIterator( &pvIter ) )
	{
		pstDxGroupList = (DxGroupList_t *)pvIter.data;
		if(pstDxGroupList == NULL)
		{
			continue;
		}

		pstGroupList = CLIB_DeSerialize( ( CList_t* )pstDxGroupList->pucData );
		if(pstGroupList)
		{
			HxLOG_Print( "\n\n[%s] GROUP # %s #( %d )\n", __FUNCTION__, pstDxGroupList->szGroupName, pstDxGroupList->nSize );
			if( HxSTD_StrCmp( pstDxGroupList->szGroupName, "ALL") == 0 )
			{
				for( i = 0 ; i < HLIB_VECTOR_Length( pstGroupList ); i++ )
				{
					pstListItem = ( CListItem_t* )HLIB_VECTOR_ItemAt( pstGroupList, i );
					if( pstListItem )
					{
						pstLcnList = HLIB_LIST_Append(pstLcnList, HLIB_STD_MemDup(pstListItem, sizeof(CListItem_t)));
						//HxLOG_Print( "[%s] uid( %lld ), bLCNControl( %d ), LCN( %d )\n", __FUNCTION__, pstListItem->uid, pstListItem->bLCNControl, pstListItem->nLogicalNo );
					}
				}
			}

			CLIB_RemoveItemList(pstGroupList);
		}
	}

#if 0
	ppvArgs[0] = (void*)pstLcnList;
#else
	ENTER_CRITICAL_HINTOPMETASVC;
	if (ctx->pstGroupList != NULL)
	{
		HLIB_LIST_RemoveAllFunc(ctx->pstGroupList, HLIB_STD_MemFree_CB);
	}

	ctx->pstGroupList = pstLcnList;
	LEAVE_CRITICAL_HINTOPMETASVC;

	HxLOG_Print("[%s] Load DxGroupList_t from DAMA success(%d)\n", __FUNCTION__, HLIB_LIST_Length(ctx->pstGroupList));
#endif
	(void)nUid;
}

#define __________________SERVICE_METADATA_LOAD__________________
static HERROR	homma_interop_meta_svc_service_Load (void)
{
	HUINT32			reqId;
	void			*pvArgs[2] = {NULL, NULL};
	hintopMetaSvc_Ctx_t * ctx;

	ctx = homma_interop_meta_svc_GetContext();

	reqId = DAPI_GetAll(DxNAMEOF(DxService_t), (DAPI_Getter)homma_interop_meta_svc_OnDataReceived, (void*)pvArgs);
	DAPI_Wait(60000, reqId);
	if (reqId == 0 || pvArgs[0] == NULL)
	{
		HxLOG_Warning("[%s] Cannot load DxService_t from DAMA!\n", __FUNCTION__);
		return ERR_FAIL;
	}

	homma_interop_meta_svc_Reset(DxNAMEOF(DxService_t));

	ENTER_CRITICAL_HINTOPMETASVC;
	ctx->pstSvcVector = (HxVector_t*)pvArgs[0];
	LEAVE_CRITICAL_HINTOPMETASVC;

	HxLOG_Print("[%s] Load DxService_t from DAMA success(%d)\n", __FUNCTION__, HLIB_VECTOR_Length(ctx->pstSvcVector));

	return ERR_OK;
}

static HERROR	homma_interop_meta_svc_transponder_Load (void)
{
	HUINT32			reqId;
	void			*pvArgs[2] = {NULL, NULL};
	hintopMetaSvc_Ctx_t * ctx;

	ctx = homma_interop_meta_svc_GetContext();

	reqId = DAPI_GetAll(DxNAMEOF(DxTransponder_t), (DAPI_Getter)homma_interop_meta_svc_OnDataReceived, (void*)pvArgs);
	DAPI_Wait(6000, reqId);
	if (reqId == 0 || pvArgs[0] == NULL)
	{
		HxLOG_Warning("[%s] Cannot load DxTransponder_t from DAMA!\n", __FUNCTION__);
		return ERR_FAIL;
	}

	homma_interop_meta_svc_Reset(DxNAMEOF(DxTransponder_t));

	ENTER_CRITICAL_HINTOPMETASVC;
	ctx->pstTpVector = (HxVector_t*)pvArgs[0];
	LEAVE_CRITICAL_HINTOPMETASVC;

	HxLOG_Print("[%s] Load DxTransponder_t from DAMA success(%d)\n", __FUNCTION__, HLIB_VECTOR_Length(ctx->pstTpVector));

	return ERR_OK;
}

static HERROR	homma_interop_meta_svc_network_Load (void)
{
	HUINT32			reqId;
	void			*pvArgs[2] = {NULL, NULL};
	hintopMetaSvc_Ctx_t * ctx;

	ctx = homma_interop_meta_svc_GetContext();

	reqId = DAPI_GetAll(DxNAMEOF(DxNetwork_t), (DAPI_Getter)homma_interop_meta_svc_OnDataReceived, (void*)pvArgs);
	DAPI_Wait(6000, reqId);
	if (reqId == 0 || pvArgs[0] == NULL)
	{
		HxLOG_Warning("[%s] Cannot load DxNetwork_t from DAMA!\n", __FUNCTION__);
		return ERR_FAIL;
	}

	homma_interop_meta_svc_Reset(DxNAMEOF(DxNetwork_t));

	ENTER_CRITICAL_HINTOPMETASVC;
	ctx->pstNetVector = (HxVector_t*)pvArgs[0];
	LEAVE_CRITICAL_HINTOPMETASVC;

	HxLOG_Print("[%s] Load DxNetwork_t from DAMA success(%d)\n", __FUNCTION__, HLIB_VECTOR_Length(ctx->pstNetVector));

	return ERR_OK;
}

static HERROR	homma_interop_meta_svc_channellogo_Load (void)
{
	HUINT32			reqId;
//	void			*pvArgs[2] = {NULL, NULL};

	reqId = DAPI_GetAll(DxNAMEOF(DxChannelLogo_t), (DAPI_Getter)homma_interop_meta_svc_OnChLogoReceived, NULL /*(void*)pvArgs*/);
#if 0	// dapi wait removed
	DAPI_Wait(6000, reqId);
	if (reqId == 0 || pvArgs[0] == NULL)
	{
		HxLOG_Warning("[%s] Cannot load DxChannelLogo_t from DAMA!\n", __FUNCTION__);
		return ERR_FAIL;
	}
#else
	HxLOG_Print("[%s] reqId : 0x%x\n", __FUNCTION__, reqId);
#endif

	return ERR_OK;
}

static HERROR	homma_interop_meta_svc_grouplist_Load (void)
{
	HUINT32			reqId;
//	void			*pvArgs[2] = {NULL, NULL};

	reqId = DAPI_GetAll(DxNAMEOF(DxGroupList_t), (DAPI_Getter)homma_interop_meta_svc_OnGroupListReceived, NULL /*(void*)pvArgs*/);
#if 0	// dapi wait removed
	DAPI_Wait(60000, reqId);
	if (reqId == 0 || pvArgs[0] == NULL)
	{
		HxLOG_Warning("[%s] Cannot load DxGroupList_t from DAMA!\n", __FUNCTION__);
		return ERR_FAIL;
	}
#else
	HxLOG_Print("[%s] reqId : 0x%x\n", __FUNCTION__, reqId);
#endif

	return ERR_OK;
}

#define __________________SERVICE_METADATA_UPDATED__________________
static void		homma_interop_meta_svc_DamaCallback (const HCHAR *pszName, HUID hUid, const void *pvData, HUINT32 ulSize, void *pvUser)
{
	HxPair_t				*pstPair = (HxPair_t *)pvUser;
	HUINT32 				ulUid;
	HINT32					i,j;
	HUINT32					ulNumberOfSvc = 0;
	HUID					*pulUidlist = NULL;
	DAxIterator_t			stIter;
	hintopMetaSvc_Ctx_t		*ctx;
	hintopMetaSvc_Type_t	stKey;

	void *pvItem, *pvNew;
	void *pvSelf;
	HxVector_t		 *pstList = NULL;
	HUINT32			ulChanged=0;
	hintopMetaSvc_DxInfo_t *pstDxInfo = NULL;

	HUINT32	*pulUidCheck;
	HUINT32	ulIdx = 0;

	if(pstPair)
	{
		ulNumberOfSvc = (HUINT32)pstPair->left;
		pulUidlist = (HUID *)pstPair->right;
	}
	else
	{
		return;
	}

	if(pstPair->right == NULL)
	{
		HLIB_STD_MemFree(pstPair);
		return;
	}

	if(ulNumberOfSvc == 0)
	{
		HLIB_STD_MemFree(pstPair->right);
		HLIB_STD_MemFree(pstPair);
		return;
	}

	pstDxInfo = homma_interop_meta_svc_GetDxInfo(pszName);

	HxLOG_Warning("Enter (type : %s, size : %d, size : %d)\n", pszName, pstDxInfo->ulSize, sizeof(DxTransponder_t));

	pulUidCheck = (HUINT32 *)HLIB_MEM_Calloc(sizeof(HUINT32) * ulNumberOfSvc);
	if(pulUidCheck == NULL)
	{
		HLIB_STD_MemFree(pstPair->right);
		HLIB_STD_MemFree(pstPair);
		return;
	}

#if defined(CONFIG_DEBUG)
	{
		HUID	*pulTemp = pulUidlist;

		for (i = ulNumberOfSvc;  i-- ; pulTemp++)
		{
			ulUid = (HUINT32)*pulTemp;
			HxLOG_Print("[%s] [%d]svcuid : %d\n",__FUNCTION__,__LINE__, ulUid);
		}
	}
#endif

	DAPI_InitIterator(&stIter, pvData, ulSize, 0);

	ctx = homma_interop_meta_svc_GetContext();

	if (DAPI_NextIterator(&stIter) == FALSE)
	{
		//2 Delete svclist
		HxLOG_Trace();
		for (i = ulNumberOfSvc;  i-- ; pulUidlist++)
		{
			HxSTD_memset(&stKey, 0, pstDxInfo->ulSize);
			ulUid = (HUINT32)*pulUidlist;
			stKey.common.uid = ulUid;

			ENTER_CRITICAL_HINTOPMETASVC;
			HxLOG_Debug("Delete(type : %s, ulUid : %d)\n",pszName, ulUid);
			homma_interop_meta_svc_RemoveObject(ctx, pszName, (const void *)&stKey);
			ulChanged++;
			LEAVE_CRITICAL_HINTOPMETASVC;
		}
	}
	else
	{
		ENTER_CRITICAL_HINTOPMETASVC;
		do
		{
			HxLOG_Trace();
			pvSelf = NULL;
			pvItem = (void *)stIter.data;
			pvSelf = homma_interop_meta_svc_FindObject(homma_interop_meta_svc_GetContext(), pszName, (const void *)pvItem);
			pulUidCheck[ulIdx++] = (HUINT32)((hintopMetaSvc_Type_t *)pvItem)->common.uid;

			if (pvSelf)
			{
				//2 Update
				pvNew = (void*)HLIB_STD_MemDup(pvItem, pstDxInfo->ulSize);
				if (pvNew == NULL)
				{
					HxLOG_Error("[%s] [%d] error. \n",__FUNCTION__,__LINE__);
				}
				else
				{
					HxLOG_Print("Update(type : %s, uid : %d)\n", pszName, ((hintopMetaSvc_Type_t *)pvNew)->common.uid);
					((hintopMetaSvc_Type_t *)pvNew)->common.typeOf = pstDxInfo->pszName;
					if (homma_interop_meta_svc_ReplaceObject(ctx, pszName, (const void *)pvNew) == NULL)
					{
						HxLOG_Error("[%s] [%d]  \n",__FUNCTION__,__LINE__);
					}
					HLIB_STD_MemFree(pvNew);

					ulChanged++;
				}
			}
			else
			{
				//2 Add
				// DAMA 저장
				pstList = homma_interop_meta_svc_GetObject(ctx, pszName);
				if (pstList == NULL)
				{
					pstList  = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))homma_interop_meta_svc_DeleteObject, homma_interop_meta_svc_CompareVectorObject);
					HxLOG_Print("!!!!New List (type : %s)", pszName);
				}

				pvNew = (void*)HLIB_STD_MemDup((void *)pvItem, pstDxInfo->ulSize);
				if (pvNew == NULL)
				{
					HxLOG_Error("[%s] [%d] error. \n",__FUNCTION__,__LINE__);
				}
				else
				{
					HxLOG_Print("Add(type : %s, uid : %d)\n", pszName, ((hintopMetaSvc_Type_t *)pvNew)->common.uid);
					((hintopMetaSvc_Type_t *)pvNew)->common.typeOf = pstDxInfo->pszName;
					HLIB_VECTOR_Add(pstList, (void *)pvNew);
					ulChanged++;
				}

				HLIB_VECTOR_Sort(pstList, NULL);
			}
		}while (DAPI_NextIterator(&stIter));

		//2 Delete svclist
		// Appkit 에서 처리한 service 개수(ulIdx)와 dama로 부터 넘어온 service 개수(ulNumberOfSvc)를 비교
		if (ulIdx != ulNumberOfSvc)
		{
			HxLOG_Debug("The number of remaining services : %d\n", ulNumberOfSvc - ulIdx);
			 //다르면 삭제된 service가 남아있다는 의미
			for (i = ulNumberOfSvc;  i-- ; pulUidlist++)
			{
				 //처리되지 않은 UID를 찾아서 해당 service를 제거한다.
				HxSTD_memset(&stKey, 0, pstDxInfo->ulSize);
				ulUid = (HUINT32)*pulUidlist;
				for (j = 0 ; j < ulIdx; j++)
				{
					HxLOG_Debug("ulUid : %d,  pulUidCheck[%d] : %d\n",ulUid, j, pulUidCheck[j]  );
					if (pulUidCheck[j] == ulUid)
					{
						break;
					}
				}

				 //루프 끝까지 왔으니 처리된 리스트에서 못찾았다는 의미이다
				HxLOG_Debug(" j : %d, ulIdx : %d\n", j, ulIdx);
				if (j == ulIdx)
				{
					stKey.common.uid = ulUid;

					HxLOG_Debug("Delete(type : %s, ulUid : %d)\n",pszName, ulUid);
					homma_interop_meta_svc_RemoveObject(ctx, pszName, (const void *)&stKey);
					ulChanged++;
				}
			}
		}

		LEAVE_CRITICAL_HINTOPMETASVC;
	}

	if (ulChanged > 0)
	{
		HxLOG_Debug("type : %s, ulChanged : %d\n", pszName, ulChanged);
		if(ctx->fnNotifier)		ctx->fnNotifier();
	}

	if (pulUidCheck)
	{
		HLIB_STD_MemFree(pulUidCheck);
	}
	HLIB_STD_MemFree(pstPair->right);
	HLIB_STD_MemFree(pstPair);
}

static void homma_interop_meta_svc_OnDamaServiceUpdated(const HCHAR *pszName, HUID hUid, const void *pvData, HUINT32 ulSize, void *pvUser)
{
	HUINT32		 ulNumberOfSvc = 0;
	HUID		*pulUidlist = (HUID *)pvData;
	HUINT32 	 ulUid;
	HxPair_t	*stItem;
	hintopMetaSvc_Ctx_t * ctx;

	ctx = homma_interop_meta_svc_GetContext();

	ulNumberOfSvc = (HUINT32)(ulSize / sizeof(HUID));

	// Reset µÇ¾îÀÖ´ÂÁö È®ÀÎ
	if (ulNumberOfSvc == 1)
	{
		ulUid = *pulUidlist;
		if (ulUid == -1)
		{
			// Delete all list
			homma_interop_meta_svc_Reset(pszName);

			if (HxSTD_StrNCmp(pszName, DxNAMEOF(DxService_t), HxSTD_StrLen(pszName)) == 0)
			{
				if(ctx->fnNotifier)		ctx->fnNotifier();
			}

			HxLOG_Warning("Remove ALL info\n");
			return;
		}
	}

	stItem= (HxPair_t *)HLIB_STD_MemCalloc(sizeof(HxPair_t));
	if (stItem == NULL)
	{
		HxLOG_Error(" Fail to mem Calloc\n");
		return ;
	}

	stItem->left = (void *)ulNumberOfSvc;
	stItem->right = (void *)HLIB_STD_MemCalloc(ulSize);
	if (stItem->right)
	{
		HxSTD_memcpy(stItem->right, (void *)pulUidlist, ulSize);
	}

	HxLOG_Warning("[%s][%s] Service info Changed!! ulNumberOfSvc : %d\n", __FUNCTION__, pszName, ulNumberOfSvc);

	DAPI_GetUIDInfos(pszName, (HUID*)pvData, ulNumberOfSvc, homma_interop_meta_svc_DamaCallback, (void *)stItem);
}

static void	homma_interop_meta_svc_OnDamaChLogoUpdated(const HCHAR *pszName, HUID nUid, const void *pvData, HUINT32 ulSize, void *pvUser)
{
	(void)nUid;
	(void)pvData;
	(void)ulSize;
	(void)pvUser;

	HxLOG_Trace();
	(void)homma_interop_meta_svc_channellogo_Load();
}

static void	homma_interop_meta_svc_OnDamaGroupListUpdated(const HCHAR *pszName, HUID nUid, const void *pvData, HUINT32 ulSize, void *pvUser)
{
	(void)nUid;
	(void)pvData;
	(void)ulSize;
	(void)pvUser;

	HxLOG_Trace();
	(void)homma_interop_meta_svc_grouplist_Load();
}

#define __________________EXTERNAL_FUNCTION__________________
//APK_META_SVC_Init(FALSE);
HBOOL HOMMA_INTEROP_META_SVC_Init (void)
{
	hintopMetaSvc_Ctx_t * ctx;

	ctx = homma_interop_meta_svc_GetContext();
	HxSTD_MemSet(ctx, 0x00, sizeof(hintopMetaSvc_Ctx_t));

	HxSEMT_Create(&s_ulhintopMetaSvcSemId, "hintopmetasvc", HxSEMT_FIFO);

	// Load full list from dama
	homma_interop_meta_svc_channellogo_Load();
	homma_interop_meta_svc_grouplist_Load();
	homma_interop_meta_svc_transponder_Load();
	homma_interop_meta_svc_network_Load();
	homma_interop_meta_svc_service_Load();

	ctx->nChLogoNotifierId = DAPI_AddTableNotifier(DxNAMEOF(DxChannelLogo_t), homma_interop_meta_svc_OnDamaChLogoUpdated, NULL);
	ctx->nGroupListNotifierId = DAPI_AddTableNotifier(DxNAMEOF(DxGroupList_t), homma_interop_meta_svc_OnDamaGroupListUpdated, NULL);
	ctx->nTpNotifierId = DAPI_AddTableNotifier(DxNAMEOF(DxTransponder_t), homma_interop_meta_svc_OnDamaServiceUpdated, NULL);
	ctx->nNetNotifierId = DAPI_AddTableNotifier(DxNAMEOF(DxNetwork_t), homma_interop_meta_svc_OnDamaServiceUpdated, NULL);
	ctx->nSvcNotifierId = DAPI_AddTableNotifier(DxNAMEOF(DxService_t), homma_interop_meta_svc_OnDamaServiceUpdated, NULL);

	return TRUE;
}

HBOOL HOMMA_INTEROP_META_SVC_DeInit (void)
{
	hintopMetaSvc_Ctx_t * ctx;

	ctx = homma_interop_meta_svc_GetContext();

	// clear vector
	(void)homma_interop_meta_svc_Reset(DxNAMEOF(DxService_t));
	(void)homma_interop_meta_svc_Reset(DxNAMEOF(DxTransponder_t));
	(void)homma_interop_meta_svc_Reset(DxNAMEOF(DxNetwork_t));

	ENTER_CRITICAL_HINTOPMETASVC;
	if (ctx->pstChLogoMap != NULL)
	{
		HLIB_TREE_Destroy(ctx->pstChLogoMap);
	}
	if (ctx->pstGroupList != NULL)
	{
		HLIB_LIST_RemoveAllFunc(ctx->pstGroupList, HLIB_STD_MemFree_CB);
	}
	LEAVE_CRITICAL_HINTOPMETASVC;

	// clear notifier
	if(ctx->nSvcNotifierId)
	{
		DAPI_RemoveNotifier(ctx->nSvcNotifierId);
		ctx->nSvcNotifierId = 0;
	}
	if(ctx->nTpNotifierId)
	{
		DAPI_RemoveNotifier(ctx->nTpNotifierId);
		ctx->nTpNotifierId = 0;
	}
	if(ctx->nNetNotifierId)
	{
		DAPI_RemoveNotifier(ctx->nNetNotifierId);
		ctx->nNetNotifierId = 0;
	}
	if(ctx->nChLogoNotifierId)
	{
		DAPI_RemoveNotifier(ctx->nChLogoNotifierId);
		ctx->nChLogoNotifierId = 0;
	}
	if(ctx->nGroupListNotifierId)
	{
		DAPI_RemoveNotifier(ctx->nGroupListNotifierId);
		ctx->nGroupListNotifierId = 0;
	}

	// remove semaphore
	HxSEMT_Destroy(s_ulhintopMetaSvcSemId);

	return TRUE;
}

//APK_META_SVC_GetTransponder(item->tsuid);
DxTransponder_t * HOMMA_INTEROP_META_SVC_GetTransponder (HINT32 nUid)
{
	DxTransponder_t	stKey;
	void *pvSelf;

	HxLOG_Trace();

	ENTER_CRITICAL_HINTOPMETASVC;

	stKey.uid = nUid;
	pvSelf = homma_interop_meta_svc_FindObject(homma_interop_meta_svc_GetContext(), DxNAMEOF(DxTransponder_t), &stKey);
	pvSelf = homma_interop_meta_svc_CloneObject(pvSelf);
	LEAVE_CRITICAL_HINTOPMETASVC;

	return (DxTransponder_t *)pvSelf;
}

//APK_META_SVC_GetNetwork(pChannel->ts->netuid);
DxNetwork_t * HOMMA_INTEROP_META_SVC_GetNetwork (HINT32 nUid)
{
	DxNetwork_t	stKey;
	void *pvSelf;

	HxLOG_Trace();

	ENTER_CRITICAL_HINTOPMETASVC;

	stKey.uid = nUid;
	pvSelf = homma_interop_meta_svc_FindObject(homma_interop_meta_svc_GetContext(), DxNAMEOF(DxNetwork_t), &stKey);
	pvSelf = homma_interop_meta_svc_CloneObject(pvSelf);
	LEAVE_CRITICAL_HINTOPMETASVC;

	return (DxNetwork_t *)pvSelf;
}

//APK_META_SVC_GetLogoUrl(item->uid, pLogo);
DxChannelLogo_t * HOMMA_INTEROP_META_SVC_GetLogoUrl(HINT32 nUid, DxChannelLogo_t *pstBuf)
{
	hintopMetaSvc_Ctx_t	*ctx = NULL;
	DxChannelLogo_t		*pstLogo = NULL;

	ENTER_CRITICAL_HINTOPMETASVC;
	ctx = homma_interop_meta_svc_GetContext();

	if (ctx != NULL && ctx->pstChLogoMap)
	{
		pstLogo = (DxChannelLogo_t *)HLIB_TREE_Lookup(ctx->pstChLogoMap, (void *)nUid);
		if (pstLogo)
		{
			if (pstBuf)
			{
				HxSTD_MemCopy(pstBuf, pstLogo, sizeof(DxChannelLogo_t));
				pstLogo = pstBuf;
			}
			else
			{
				pstLogo = (DxChannelLogo_t *)HLIB_STD_MemDup(pstLogo, sizeof(DxChannelLogo_t));
			}
		}
		else
		{
			if (pstBuf) HxSTD_MemSet(pstBuf, 0, sizeof(DxChannelLogo_t));
		}
	}
	else
	{
		if (pstBuf)HxSTD_MemSet(pstBuf, 0, sizeof(DxChannelLogo_t));
	}

	LEAVE_CRITICAL_HINTOPMETASVC;
	return pstLogo;
}

//APK_META_SVC_GetServiceList();
HxVector_t * HOMMA_INTEROP_META_SVC_GetServiceList (void)
{
	HxVector_t	*pstList;
	HxVector_t	*pstResult;

	ENTER_CRITICAL_HINTOPMETASVC;
	pstList = homma_interop_meta_svc_GetObject(homma_interop_meta_svc_GetContext(), DxNAMEOF(DxService_t));
	if (pstList)
	{
		pstResult = HLIB_VECTOR_CloneEx(pstList, (void *(*)(void *))homma_interop_meta_svc_CloneObject);
	}
	else
	{
		pstResult = NULL;
	}
	LEAVE_CRITICAL_HINTOPMETASVC;

	RETURN_IF(pstResult == NULL, NULL);
	if (HLIB_VECTOR_Length(pstResult) == 0)
	{
		HLIB_VECTOR_Delete(pstResult);
		return NULL;
	}
	return pstResult;
}

//APK_META_SVC_GetService(ulSvcUID);
DxService_t * HOMMA_INTEROP_META_SVC_GetService (HINT32 nUid)
{
	DxService_t	stKey;
	void *pvSelf;

	HxLOG_Trace();

	ENTER_CRITICAL_HINTOPMETASVC;
	stKey.uid = nUid;
	pvSelf = homma_interop_meta_svc_FindObject(homma_interop_meta_svc_GetContext(), DxNAMEOF(DxService_t), &stKey);
	pvSelf = homma_interop_meta_svc_CloneObject(pvSelf);
	LEAVE_CRITICAL_HINTOPMETASVC;

	return (DxService_t *)pvSelf;
}

HINT32 HOMMA_INTEROP_META_SVC_GetChannelNumber (HINT32 nSvcUid)
{
	hintopMetaSvc_Ctx_t	*ctx = NULL;
	HINT32					nLogicalNumber = -1;
	HxList_t				*pTmpList = NULL;
	CListItem_t				*pCListItem = NULL;

	ctx = homma_interop_meta_svc_GetContext();

	pTmpList = ctx->pstGroupList;
	if(pTmpList == NULL)
		HxLOG_Debug("[%s:%d] Error> channel group is not existed! \n", __func__, __LINE__);

	while( pTmpList != NULL )
	{
		pCListItem = ( CListItem_t * )HLIB_LIST_Data( pTmpList );

		if(pCListItem != NULL)
		{
			if( pCListItem->uid == (HUID)nSvcUid )
			{
				nLogicalNumber = pCListItem->nLogicalNo;
				break;
			}

			pTmpList = pTmpList->next;
		}
		else
			break;
	}

	return nLogicalNumber;
}

//APK_META_SVC_SetListener(_plChannelList_EventListener, (void *)NULL);
HERROR HOMMA_INTEROP_META_SVC_SetListener (hintopMetaSvc_Notifier_t pfOnUpdate)
{
	hintopMetaSvc_Ctx_t *ctx;

	ctx = homma_interop_meta_svc_GetContext();

	if(pfOnUpdate)
	{
		ctx->fnNotifier = pfOnUpdate;
	}

	return ERR_OK;
}

//APK_META_SVC_LoadupdatedList();
HERROR HOMMA_INTEROP_META_SVC_LoadupdatedList (void)
{
	// TODO:
	return ERR_OK;
}

//APK_META_SVC_Delete(pstService);
void HOMMA_INTEROP_META_SVC_Delete (void *pvObject)
{
	if(pvObject)
	{
		HLIB_STD_MemFree(pvObject);
	}
}

#if 0 // have to check ---> meta schedule
//APK_META_SCHEDULE_AddListener()
void HOMMA_INTEROP_META_SCHEDULE_AddListener (HOMMA_INTEROP_MetaSched_Handle_t hMeta, HOMMA_INTEROP_MetaSched_Listener_t listener, void *userdata)
{
	// TODO:
}
#endif

#endif // #ifdef HOMMA_USE_APPKIT_DIRECTLY endif

