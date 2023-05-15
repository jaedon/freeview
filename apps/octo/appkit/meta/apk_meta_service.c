/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_meta_service.c
	@brief

	Description:  									\n
	Module: APPKITv2			\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/
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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hapi.h>
#include <dlib.h>
#include <dapi.h>
#include <oapi.h>
#include <apk.h>
#include <_apk_int.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
//#define apkMetaSvc_DUMP_ON_UPDATE
#define	apkMetaSvc_CHLOGO
#ifdef apkMetaSvc_DUMP_ON_UPDATE
	#define	apkMetaSvc_DUMP_ONCE
#endif

#define	apkMetaSvc_MAX_TABLE_NUM 		6	// DxService_t, DxTransponder_t, DxNetwork_t, DxProvider_t, DxGroup_t, DxBouquet_t

#define 	ENTER_CRITICAL_APKMETASVC		do { HxSEMT_Get(s_ulMetaSvcSemId); } while (0)
#define	LEAVE_CRITICAL_APKMETASVC		do { HxSEMT_Release(s_ulMetaSvcSemId); } while (0)

#define	RETURN_IF_VOID(expr)	do{if(expr){HxLOG_Debug("[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);return;}}while(0)
#define	RETURN_IF(expr, err)		do{if(expr){HxLOG_Debug("[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);return (err);}}while(0)
#define	GOTO_IF(expr, pos)		do{if(expr){HxLOG_Debug("[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);goto pos;}}while(0)

//#define	APKMETASVC_USE_VCLONE


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

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
	DxProvider_t		stProvider;
	DxGroup_t			stGroup;
	DxBouquet_t			stBouquet;
} apkMetaSvc_Type_t;

typedef struct
{
	ApkMetaService_Notifier_t	pfOnUpdate;
	void *				pvUserdata;
} apkMetaSvc_Notifier_t;

typedef struct
{
	HCHAR	*pszName;
	HUINT32	ulSize;
} apkMetaSvc_DxInfo_t;

typedef struct
{
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
} apkMetaSvc_Ctx_t;

typedef struct
{
	HxTREE_t	*pstUpdated;
	HxTREE_t	*pstDeleted;
}apkMetaSvc_DuInfo_t;


typedef struct
{
	HUINT32	nUid;
}apkMetaSvc_TrashInfo_t;


#if defined(APKMETASVC_USE_VCLONE)
typedef struct
{
	apkMetaSvc_Type_t		*pstItem;
	HUINT32				nRefCnt;
}apkMetaSvc_VClone_t;
#endif

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
const STATIC 	apkMetaSvc_DxInfo_t		cs_astDxInfo[apkMetaSvc_MAX_TABLE_NUM] = 	{
																{DxNAMEOF(DxService_t), sizeof(DxService_t)},
																{DxNAMEOF(DxTransponder_t), sizeof(DxTransponder_t)},
																{DxNAMEOF(DxNetwork_t), sizeof(DxNetwork_t)},
																{DxNAMEOF(DxProvider_t), sizeof(DxProvider_t)},
																{DxNAMEOF(DxGroup_t), sizeof(DxGroup_t)},
																{DxNAMEOF(DxBouquet_t), sizeof(DxBouquet_t)}
															};
STATIC HUINT32					s_ulMetaSvcSemId;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

#define	________LOCAL___Prototype__apkmetasvc_____

static HINT32	apk_meta_svc_ApplyUpdate (apkMetaSvc_Ctx_t *pstMgr, HINT32 *nCount, HxTREE_t *pstUpdated, HxTREE_t *pstRemoved, HBOOL bFlag);
static HBOOL	apk_meta_svc_UpdateChecker (apkMetaSvc_Ctx_t *pstMgr, const HCHAR *pszName, const apkMetaSvc_Type_t *pstOld, const apkMetaSvc_Type_t *pstNew);
static void	apk_meta_svc_DumpAll (apkMetaSvc_Ctx_t *pstMgr, HxTREE_t *pstListTree, HxTREE_t *pstUidTree, const HCHAR *pszName);
static void	apk_meta_svc_SetDynamicUpdate (apkMetaSvc_Ctx_t *pstMgr, HBOOL bOnOff);
static void 	apk_meta_svc_LoadUpdatedList(HBOOL bForceNoti);
static void *	apk_meta_svc_ReplaceObject (apkMetaSvc_Ctx_t *pstMgr, const HCHAR *pszType, const void *newObj);
static void *	apk_meta_svc_FindObject (apkMetaSvc_Ctx_t *pstMgr, const HCHAR *type, const void *key);
static HBOOL	apk_meta_svc_RemoveObject (apkMetaSvc_Ctx_t *pstMgr, const HCHAR *type, const void *key);
static HERROR	apk_meta_svc_Reset (HBOOL bResetDb, const HCHAR *pszName);

#if defined(APKMETASVC_USE_VCLONE)
static void		apk_meta_svc_VDeleteObject (void *pvSelf);
static void		apk_meta_svc_DeleteVCloneObject (void *pvSelf);
static HINT32		apk_meta_svc_CompareVCloneObject (const void *pvArg1, const void *pvArg2);
static void	*	apk_meta_svc_VCloneObject (const void *pvSelf);
#endif

#if defined(CONFIG_DEBUG)
static void	apk_meta_svc_InitCommand(void);
#endif

#define	________LOCAL___Body___apkmetasvc____
static apkMetaSvc_Ctx_t *	apk_meta_svc_GetContext (void)
{
	static apkMetaSvc_Ctx_t	s_stManager;
	if (s_stManager.pstListTree == NULL)
	{
		s_stManager.pstListTree = HLIB_TREE_NewFull(
				  (HxCompareDataFunc)HLIB_STD_StrCaseCmp
				, NULL
				, NULL
				, (HxDestroyNotify)HLIB_VECTOR_Delete
			);
		HxLOG_Assert(s_stManager.pstListTree);

		s_stManager.pstTrashTree = HLIB_TREE_NewFull(
				  (HxCompareDataFunc)HLIB_STD_StrCaseCmp
				, NULL
				, NULL
				, (HxDestroyNotify)HLIB_VECTOR_Delete
			);
		HxLOG_Assert(s_stManager.pstTrashTree);

#if defined(APKMETASVC_USE_VCLONE)
		s_stManager.pstVCloneTree = HLIB_TREE_NewFull(
			  (HxCompareDataFunc)HLIB_STD_StrCaseCmp
			, NULL
			, NULL
			, (HxDestroyNotify)HLIB_VECTOR_Delete
		);
		HxLOG_Assert(s_stManager.pstVCloneTree);
#endif
	}
	return &s_stManager;
}

static void *	apk_meta_svc_NewObject (const apkMetaSvc_DxInfo_t *pstDxInfo)
{
	apkMetaSvc_Type_t	*pstSelf;

	pstSelf = (apkMetaSvc_Type_t *)APK_Calloc(pstDxInfo->ulSize);
	if (pstSelf)
	{
		pstSelf->common.typeOf = pstDxInfo->pszName;
	}
	return (void *)pstSelf;
}

static apkMetaSvc_DxInfo_t * apk_meta_svc_GetDxInfo(const HCHAR *pszName)
{
	HUINT32		ulIdx;

	for (ulIdx = 0 ; ulIdx < apkMetaSvc_MAX_TABLE_NUM ; ulIdx++)
	{
		if (HxSTD_StrCmp(pszName, cs_astDxInfo[ulIdx].pszName) == 0)
		{
			return (apkMetaSvc_DxInfo_t *)&cs_astDxInfo[ulIdx];
		}
	}

	return NULL;
}
static void		apk_meta_svc_DeleteObject (void *pvSelf)
{
	apkMetaSvc_Type_t	*pstThiz = (apkMetaSvc_Type_t *)pvSelf;
	if (pstThiz)
	{
		HxLOG_Assert(pstThiz->common.typeOf);
		APK_Free(pstThiz);
	}
}

static void		apk_meta_svc_DeleteTrashObject (void *pvSelf)
{
	apkMetaSvc_TrashInfo_t	*pstThiz = (apkMetaSvc_TrashInfo_t *)pvSelf;
	if (pstThiz)
	{
		HLIB_MEM_Free(pstThiz);
	}
}

static HINT32	apk_meta_svc_CompareVectorObject (const void *pvArg1, const void *pvArg2)
{
	const void *pvObj1 = *(const void **)pvArg1;
	const void *pvObj2 = *(const void **)pvArg2;

	return (HINT32)memcmp(pvObj1, pvObj2, sizeof(HINT32)/* uid field */);
}

//Callback func for DAPI_GetAll() in meta_svc_Load()
static void		apk_meta_svc_OnDataReceived(const HCHAR *pszName, HUID nUid, const void *pvData, HUINT32 ulSize, void **ppvArgs)
{
	HxVector_t		*pstList;
	apkMetaSvc_DxInfo_t *pstDxInfo;
	void			*pvObj;
	DAxIterator_t	pvIter;
	apkMetaSvc_Type_t    *pstThiz;

	RETURN_IF_VOID(ulSize == 0 || pvData == NULL);

	pstDxInfo = apk_meta_svc_GetDxInfo(pszName);
	if (pstDxInfo == NULL)
	{
		HxLOG_Error("[%s] [%d] error \n",__FUNCTION__,__LINE__);
		return;
	}

	pstList  = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))apk_meta_svc_DeleteObject, apk_meta_svc_CompareVectorObject);
	RETURN_IF_VOID(pstList == NULL);

	DAPI_InitIterator(&pvIter, pvData, ulSize, pstDxInfo->ulSize);
	while (DAPI_NextIterator(&pvIter))
	{
		pvObj = apk_meta_svc_NewObject(pstDxInfo);
		if (pvObj == NULL)
			continue;

		memcpy(pvObj, pvIter.data,pstDxInfo->ulSize);
		pstThiz = (apkMetaSvc_Type_t *)pvObj;
		pstThiz->common.typeOf = pstDxInfo->pszName;

		HLIB_VECTOR_Add(pstList, pvObj);
	}

	if (HLIB_VECTOR_Length(pstList) == 0)
	{
		HxLOG_Error("[%s] [%d] empty \n",__FUNCTION__,__LINE__);
		HLIB_VECTOR_Delete(pstList);
		return;
	}
	HLIB_VECTOR_Sort(pstList, NULL);

	HxLOG_Print("[%s] [%d]  \n",__FUNCTION__,__LINE__);

	ppvArgs[0] = (void *)pstList;
	(void)nUid;
}

// Callback func for OAPI_META_SVC_LoadUpdatedList()
STATIC void		apk_meta_svc_freeOnResponse(HINT32 nArgc, void *apArgV[])
{
	void	*pvData;

	HxLOG_Assert(nArgc == 4);

	pvData = apArgV[2];
	if (pvData)
		APK_Free(pvData);
}

static void apk_meta_svc_OnResponse(HxTREE_t	*pstUpdated, HxTREE_t *pstRemoved, HBOOL bFlag)
{
	apkMetaSvc_Ctx_t	*pstMgr;
	ApkMetaService_Notifier_t	pfOnUpdate = NULL;
	void *				pvUserdata = NULL;
	HINT32				nChanged  = 0;
	HINT32				nDynamicUpdated = 0;
	HINT32				nArgs[2];
	APK_EVENT_EMIT_ARGS	stApkEvtArgs;

	ENTER_CRITICAL_APKMETASVC;
	pstMgr = apk_meta_svc_GetContext();

	pfOnUpdate = pstMgr->pfNotifier.pfOnUpdate;
	pvUserdata = pstMgr->pfNotifier.pvUserdata;

	HxLOG_Print("Receive response!!\n");

	/**
	 * nChanged는 변화한 모든 개수 (Remove, Add, Update)
	 * nDynamicUpdated는 실제 다이나믹 업뎃 처리할 아이템 개수
	 */
	nChanged = apk_meta_svc_ApplyUpdate(pstMgr, &nDynamicUpdated, pstUpdated, pstRemoved, bFlag);

	nArgs[0] = pstMgr->nVersion;
	nArgs[1] = (HINT32)FALSE;

	LEAVE_CRITICAL_APKMETASVC;

	if (!pfOnUpdate)
	{
		return;
	}

	if(bFlag)  //변경된것이 없어도 무조건 올려준다. (search 시)
	{
		HxSTD_MemSet(&stApkEvtArgs, 0, sizeof(APK_EVENT_EMIT_ARGS));

		stApkEvtArgs.pfnFreeFunc = apk_meta_svc_freeOnResponse;
		stApkEvtArgs.ulArgCount = 4;
		stApkEvtArgs.apArgV[0] = (void *)pfOnUpdate;
		stApkEvtArgs.apArgV[1] = (void *)1;
		stApkEvtArgs.apArgV[2] = (void *)APK_MemDup(nArgs, sizeof(HINT32) * 2);
		stApkEvtArgs.apArgV[3] = (void *)pvUserdata;

		APK_EVENT_Emit(&stApkEvtArgs);
	}
	else
	{
		if (nChanged > 0)
		{
			HxLOG_Warning("( DYNAMIC UPDATE ) pstUpdated:%d, svc_updated:%d!!\n", pstUpdated, nDynamicUpdated);

			if (nDynamicUpdated > 0)
			{
				HxSTD_MemSet(&stApkEvtArgs, 0, sizeof(APK_EVENT_EMIT_ARGS));

				stApkEvtArgs.pfnFreeFunc = apk_meta_svc_freeOnResponse;
				stApkEvtArgs.ulArgCount = 4;
				stApkEvtArgs.apArgV[0] = (void *)pfOnUpdate;
				stApkEvtArgs.apArgV[1] = (void *)0;
				stApkEvtArgs.apArgV[2] = (void *)APK_MemDup(nArgs, sizeof(HINT32) * 2);
				stApkEvtArgs.apArgV[3] = (void *)pvUserdata;

				APK_EVENT_Emit(&stApkEvtArgs);
			}
		}
	}

}

static HBOOL	apk_meta_svc_RemoveAllObject (apkMetaSvc_Ctx_t *pstMgr, const HCHAR *pszType)
{
	HxVector_t	*pstList;

	pstList = (HxVector_t *)HLIB_TREE_Lookup(pstMgr->pstListTree, pszType);
	RETURN_IF(pstList == NULL, FALSE);

	if (HLIB_VECTOR_RemoveAll(pstList) != ERR_OK)
	{
		return FALSE;
	}

#if defined(APKMETASVC_USE_VCLONE)
	pstList = (HxVector_t *)HLIB_TREE_Lookup(pstMgr->pstVCloneTree, pszType);
	RETURN_IF(pstList == NULL, FALSE);

	HLIB_VECTOR_RemoveAll(pstList);
#endif

	return TRUE;
}


static HINT32	apk_meta_svc_CompareINT32 (const void *pvA , const void *pvB)
{
	return (HINT32)pvA - (HINT32)pvB;
}

static HBOOL	apk_meta_svc_RemoveIf (const apkMetaSvc_Type_t *pstItem, void **ppvArgs)
{
	apkMetaSvc_Ctx_t	*pstMgr  = (apkMetaSvc_Ctx_t *)ppvArgs[0];
	const HCHAR			*pszName = (const HCHAR *)ppvArgs[1];
	HxVector_t 			*pstUids = (HxVector_t *)ppvArgs[2];
	HINT32				*pnCount= (HINT32 *)ppvArgs[3];

	if (pstItem && pstUids && HLIB_VECTOR_IndexOf(pstUids, (void *)pstItem) >= 0)
	{
		if (apk_meta_svc_UpdateChecker(pstMgr, pszName, pstItem, NULL))
		{
			(*pnCount)++;
		}
		return TRUE;
	}
	return FALSE;
}

static HBOOL		apk_meta_svc_IsServiceUpdated (const DxService_t *pstOld, const DxService_t *pstNew)
{
	if (pstOld == NULL)
		return pstNew ? TRUE : FALSE;

#define	INT_UPDATED(prop)	(pstOld->prop != pstNew->prop)
#define	MEM_UPDATED(prop,s)	(memcmp(pstOld->prop, pstNew->prop, s) != 0)
#define STR_UPDATED(prop)	(strcmp(pstOld->prop, pstNew->prop) != 0)

	if (	INT_UPDATED(uid)
		|| INT_UPDATED(tsuid)
		|| INT_UPDATED(prvuid)
		|| INT_UPDATED(antuid)
		|| MEM_UPDATED(grpuids, (sizeof(HINT32) * DxGROUP_LEN))
		|| MEM_UPDATED(bqtuids, (sizeof(HINT32) * DxBOUQUET_LEN))
		|| INT_UPDATED(svcid)
		|| INT_UPDATED(tsid)
		|| INT_UPDATED(onid)
		|| INT_UPDATED(lcn)
		|| INT_UPDATED(locked)
		|| STR_UPDATED(name)
		|| INT_UPDATED(removed)
		|| INT_UPDATED(audioPid)
#if defined(CONFIG_OP_SES)
		|| INT_UPDATED(casType)
#endif
		|| INT_UPDATED(user.subtitle)
#if defined(SVC_SUBTITLE_LANGUAGE_CODE)
		|| STR_UPDATED(user.subtitleLanguage)
		|| INT_UPDATED(user.hardOfHearing)
#else
		|| INT_UPDATED(user.subtitleTrack)
#endif
		|| INT_UPDATED(user.audio)
		|| INT_UPDATED(user.audioTrack)
		|| INT_UPDATED(user.ratingPassedEventId)
		|| INT_UPDATED(visibleFlag)
	) {
		return TRUE;
	}
#undef	INT_UPDATED
#undef	MEM_UPDATED
#undef	STR_UPDATED
	return FALSE;
}

static HBOOL	apk_meta_svc_UpdateChecker (apkMetaSvc_Ctx_t *pstMgr, const HCHAR *pszName, const apkMetaSvc_Type_t *pstOld, const apkMetaSvc_Type_t *pstNew)
{
	HBOOL	bUpdated = FALSE;

	if (pstMgr->pfChecker && pstMgr->pfChecker(pszName, pstOld, pstNew, &bUpdated))
	{
		return bUpdated;
	}

	if (pstOld && pstNew)	// bUpdated
	{
		if (strcmp(pszName, DxNAMEOF(DxService_t)) == 0
			&& apk_meta_svc_IsServiceUpdated(&(pstOld->stService), &(pstNew->stService)))
		{
			return TRUE;
		}
	}
	else if (pstOld)		// removed
	{
		if (strcmp(pszName, DxNAMEOF(DxService_t)) == 0)
			return TRUE;
	}
	else if (pstNew)		// added
	{
		if (strcmp(pszName, DxNAMEOF(DxService_t)) == 0)
			return TRUE;
	}
	else
	{
		HxLOG_Critical("[%s] WTF!!!!!!\n", __FUNCTION__);
		return FALSE;
	}
	return FALSE;
}


// Obama의 dynamic update를 appkit/Dama에 반영
static HINT32		apk_meta_svc_ApplyUpdate (apkMetaSvc_Ctx_t *pstMgr, HINT32 *nCount, HxTREE_t *pstUpdated, HxTREE_t *pstRemoved, HBOOL bFlag)
{
	void		*pvIter;
	const HCHAR *pszName;
	HxVector_t	*pstList;
	HxVector_t	*pstNewList, *pstTList;
	HINT32		nChanged = 0;
	void		*pvArgs[5];
	apkMetaSvc_TrashInfo_t		*pstTrash;

	// 삭제 처리
	if (pstRemoved)
	{
		apkMetaSvc_Type_t *pstItem;

		HxLOG_Warning("[%s:%d] SVCList Removed --> type count [%d]\n", __FUNCTION__, __LINE__, HLIB_TREE_Nnodes(pstRemoved));
		for (pvIter = HLIB_TREE_Iterator(pstRemoved) ; pvIter ; pvIter = HLIB_TREE_NextIterator(pvIter))
		{
			pszName    = (const HCHAR *)HLIB_TREE_IteratorKey(pvIter);
			pstNewList = (HxVector_t  *)HLIB_TREE_IteratorValue(pvIter);
			if (!pszName || !pstNewList)
				continue;

			pstList = (HxVector_t *)HLIB_TREE_Lookup(pstMgr->pstListTree, (void *)pszName);
			if (pstList == NULL)
				continue;

			pvArgs[0] = (void *)pstMgr;
			pvArgs[1] = (void *)pszName;
			pvArgs[2] = (void *)pstNewList;
			pvArgs[3] = (void *)nCount;

			// DU
			if (FALSE == bFlag)
			{
				DAPI_BatchBegin(pszName);
				HxVECTOR_EACH(pstNewList, apkMetaSvc_Type_t *, pstItem, {
					HxLOG_Print("DEL uid : %d\n",pstItem->common.uid);
					DAPI_Reset(pszName, pstItem->common.uid);
				});
				DAPI_BatchEnd(pszName);
				DAPI_LazySync(pszName, 1000);


			}
			else	// Search
			{
				// Trash list에 저장한다.
				pstTList = (HxVector_t *)HLIB_TREE_Lookup(pstMgr->pstTrashTree, (void *)pszName);
				if (pstTList == NULL)
				{
					HxLOG_Debug("Create new trash list (type : %s)\n", pszName);
					pstTList  = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))apk_meta_svc_DeleteTrashObject, apk_meta_svc_CompareVectorObject);
					HLIB_TREE_Insert(pstMgr->pstTrashTree, (void *)pszName, pstTList);
				}
				HxVECTOR_EACH(pstNewList, apkMetaSvc_Type_t *, pstItem, {
					pstTrash = NULL;
					pstTrash = (apkMetaSvc_TrashInfo_t *)HLIB_STD_MemAlloc(sizeof(apkMetaSvc_TrashInfo_t));
					if (pstTrash)
					{
						pstTrash->nUid = pstItem->common.uid;
						HLIB_VECTOR_Add(pstTList, (void *)pstTrash);
						HxLOG_Debug("Add Trash list (Type : %s, UID : %d)  \n", pszName, pstTrash->nUid);
					}
				});
			}
			nChanged += HLIB_VECTOR_RemoveIf(pstList, (HBOOL (*)(void *,void *))apk_meta_svc_RemoveIf, (void *)pvArgs);
			HxLOG_Print("SVCList Removed!(count : %d) \n", nChanged);

		}
		HLIB_TREE_Destroy(pstRemoved);
	}

	if (pstUpdated)
	{
		const HCHAR *pszSteal[10];
		HINT32		nStealLen = 0;
		void		*pvItem, *pvOld;
		HINT32		ulIdx;
		HINT32		i, n;
		apkMetaSvc_DxInfo_t *pstDxInfo;

		HxLOG_Warning("SVCList Updated --> nodes [%d]\n", HLIB_TREE_Nnodes(pstUpdated));
		for (pvIter = HLIB_TREE_Iterator(pstUpdated) ; pvIter ; pvIter = HLIB_TREE_NextIterator(pvIter))
		{
			pszName    = (const HCHAR *)HLIB_TREE_IteratorKey(pvIter);
			pstNewList = (HxVector_t  *)HLIB_TREE_IteratorValue(pvIter);
			if (!pszName || !pstNewList)
				continue;

			pstDxInfo = apk_meta_svc_GetDxInfo(pszName);
			if (pstDxInfo == NULL)
			{
				continue;
			}

			pstList = (HxVector_t *)HLIB_TREE_Lookup(pstMgr->pstListTree, (void *)pszName);
			if (pstList == NULL)
			{
				HxLOG_Warning("!!!!New List (%s)\n", pszName);
				// 비어있다면 그냥 모두 추가한다.
				pszSteal[nStealLen++] = pszName;
				HLIB_TREE_Insert(pstMgr->pstListTree, (void *)pszName, pstNewList);
				nChanged += HLIB_VECTOR_Length(pstNewList);

				// Set New Channel flag
				if(HxSTD_StrCmp(pszName, DxNAMEOF(DxService_t)) == 0)
				{
					DxService_t * pStSvc;
					HxVECTOR_EACH(pstNewList, void *, pvItem, {
						pStSvc = (DxService_t *)pvItem;
						pStSvc->updateSvcFlag = eDxSVC_UPDATE_NEWCHANNEL;
					});
				}

				if (FALSE == bFlag)
				{
					// Added
					HxVECTOR_EACH(pstNewList, void *, pvItem, {
						if (apk_meta_svc_UpdateChecker(pstMgr, pszName, NULL, pvItem))
							(*nCount)++;
						DAPI_LazySet(pszName, *(HINT32 *)pvItem, pvItem, pstDxInfo->ulSize, 100);
					});
					DAPI_LazySync(pszName, 500);
				}
				continue;
			}

			//2 Update
			n = HLIB_VECTOR_Length(pstNewList);
			for (i = 0 ; i < n ; i++)
			{
				pvItem  = HLIB_VECTOR_ItemAt(pstNewList, i);
				if (pvItem == NULL)
					continue;

				ulIdx = HLIB_VECTOR_BSearch(pstList, pvItem, NULL);// uid 기반 search
				if (ulIdx >= 0)
				{
					if (TRUE == bFlag)	// from search
					{
						HxLOG_Print("From search~\n");
						if(HxSTD_StrCmp(pszName, DxNAMEOF(DxService_t)) == 0)
						{
							DxService_t * pStSvc;
							pStSvc = (DxService_t *)pvItem;
							pStSvc->updateSvcFlag = eDxSVC_UPDATE_UPDATECHANNEL;
							pStSvc->removed = FALSE;
						}

						pvOld = HLIB_VECTOR_ItemAt(pstList, ulIdx);
						if (pvOld && memcmp(pvOld, pvItem, pstDxInfo->ulSize) != 0)
						{
							HxLOG_Print("Update Item [UID : %d]\n",((apkMetaSvc_Type_t *)pvItem)->common.uid);
							HLIB_VECTOR_Replace(pstList, ulIdx, pvItem);

							if (apk_meta_svc_UpdateChecker(pstMgr, pszName, pvOld, pvItem))
							{
								(*nCount)++;
							}
						}
					}
					else	// From dynamic update
					{
						HxLOG_Print("From dynamic update. (type : %s)\n", pszName);
						pvOld = HLIB_VECTOR_ItemAt(pstList, ulIdx);
						if (pvOld && memcmp(pvOld, pvItem, pstDxInfo->ulSize) != 0)
						{
							HLIB_VECTOR_Replace(pstList, ulIdx, pvItem);

							if (apk_meta_svc_UpdateChecker(pstMgr, pszName, pvOld, pvItem))
							{
								(*nCount)++;
							}

							HxLOG_Print("dynamic update. (type : %s)\n", pszName);
							if(HxSTD_StrCmp(pszName, DxNAMEOF(DxService_t)) == 0)
							{
								DxService_t * pStSvc;
								pStSvc = (DxService_t *)pvItem;
								pStSvc->updateSvcFlag = eDxSVC_UPDATE_UPDATECHANNEL;
								HxLOG_Print("[DxService_t Update] UID : %d, name : %s, removed : %d, lcn : %d\n", pStSvc->uid, pStSvc->name, pStSvc->removed, pStSvc->lcn);
							}
							DAPI_LazySet(pszName, *(HINT32 *)pvItem, pvItem, pstDxInfo->ulSize, 100);
							DAPI_LazySync(pszName, 500);

							apk_meta_svc_DeleteObject(pvOld);
						}
					}

					HLIB_VECTOR_Replace(pstNewList, i, NULL);
				}
			}

			//2 Add
			n = HLIB_VECTOR_Length(pstNewList);
			for (i = 0 ; i < n ; i++)
			{
				pvItem = HLIB_VECTOR_ItemAt(pstNewList, i);
				if (pvItem)
				{
					if (apk_meta_svc_UpdateChecker(pstMgr, pszName, NULL, pvItem))
					{
						(*nCount)++;
					}

					// Set New Channel flag
					if(HxSTD_StrCmp(pszName, DxNAMEOF(DxService_t)) == 0)
					{
						   DxService_t * pStSvc;
						   pStSvc = (DxService_t *)pvItem;
						   pStSvc->updateSvcFlag = eDxSVC_UPDATE_NEWCHANNEL;
						   HxLOG_Print("[DxService_t Add]!!!! UID : %d, name : %s, removed : %d, lcn : %d\n", pStSvc->uid, pStSvc->name, pStSvc->removed, pStSvc->lcn);
					}

					HLIB_VECTOR_Add(pstList, pvItem);

					if (FALSE == bFlag)
					{
						DAPI_LazySet(pszName, *(HINT32 *)pvItem, pvItem, pstDxInfo->ulSize, 100);
						DAPI_LazySync(pszName, 500);
					}
				}
			}
			HLIB_VECTOR_Sort(pstList, NULL);

			nChanged += HLIB_VECTOR_Length(pstNewList);
			pstNewList->count = 0; // steal all
		}
		while (nStealLen--)
			HLIB_TREE_Steal(pstUpdated, (void *)pszSteal[nStealLen]);
		HLIB_TREE_Destroy(pstUpdated);
	}

#ifdef apkMetaSvc_DUMP_ON_UPDATE
{
#ifdef apkMetaSvc_DUMP_ONCE
	static int ___dump_once = 0;

	if (___dump_once)
		return nChanged;
#endif

	HxLOG_Debug("[%s:%d] write all meta data to File [%d]\n", __FUNCTION__, __LINE__, HLIB_STD_GetSystemTick());
	apk_meta_svc_DumpAll(pstMgr, NULL, NULL, NULL);
	HxLOG_Debug("[%s:%d] Done [%d]\n", __FUNCTION__, __LINE__, HLIB_STD_GetSystemTick());
#ifdef	apkMetaSvc_DUMP_ONCE
	___dump_once++;
#endif
}
#endif

	return nChanged;		// add, remove, update 개수 모두 리턴

}

static void	*	apk_meta_svc_CloneObject (const void *pvSelf)
{
	apkMetaSvc_Type_t    *pstThiz = (apkMetaSvc_Type_t *)pvSelf;
	apkMetaSvc_DxInfo_t * pstDxInfo;
	apkMetaSvc_Type_t    *pstClone;

	RETURN_IF(pvSelf == NULL, NULL);

	HxLOG_Assert(pstThiz->common.typeOf);

	pstDxInfo = apk_meta_svc_GetDxInfo(pstThiz->common.typeOf);

	pstClone = (apkMetaSvc_Type_t *)APK_Calloc(pstDxInfo->ulSize);
	if (pstClone)
	{
		memcpy(pstClone, pvSelf, pstDxInfo->ulSize);
		pstClone->common.typeOf = pstThiz->common.typeOf;
	}
	return pstClone;
}

STATIC void	apk_meta_svc_EmitEvent(void)
{
	apkMetaSvc_Ctx_t * pstMgr = NULL;
	HINT32				nArgs[2];
	ApkMetaService_Notifier_t	pfOnUpdate = NULL;
	void *				pvUserdata = NULL;

	HxLOG_Trace();

	pstMgr = apk_meta_svc_GetContext();
	if (!pstMgr)
	{
		HxLOG_Error("Error!!\n");
		return ;
	}

	ENTER_CRITICAL_APKMETASVC;
	pfOnUpdate = pstMgr->pfNotifier.pfOnUpdate;
	pvUserdata = pstMgr->pfNotifier.pvUserdata;

	nArgs[0] = pstMgr->nVersion;
	nArgs[1] = (HINT32)FALSE;

	LEAVE_CRITICAL_APKMETASVC;

	if (pfOnUpdate)
	{
		APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

		HxLOG_Debug("Emit Event!!\n");

		stApkEvtArgs.pfnFreeFunc = apk_meta_svc_freeOnResponse;
		stApkEvtArgs.ulArgCount = 4;
		stApkEvtArgs.apArgV[0] = (void *)pfOnUpdate;
		stApkEvtArgs.apArgV[1] = (void *)0;
		stApkEvtArgs.apArgV[2] = (void *)APK_MemDup(nArgs, sizeof(HINT32) * 2);
		stApkEvtArgs.apArgV[3] = (void *)pvUserdata;

		APK_EVENT_Emit(&stApkEvtArgs);
	}
}

STATIC void		apk_meta_svc_DamaCallback (const HCHAR *pszName, HUID hUid, const void *pvData, HUINT32 ulSize, void *pvUser)
{
	HxPair_t				*pstPair = (HxPair_t *)pvUser;
	HUINT32 				 ulUid;
	HINT32				 i,j;
	HUINT32				 ulNumberOfSvc = (HUINT32)pstPair->left;
	HUID				*pulUidlist = (HUID *)pstPair->right;
	DAxIterator_t			 stIter;
	apkMetaSvc_Ctx_t * pstMgr;
	apkMetaSvc_Type_t stKey;

	void *pvItem, *pvNew;
	void *pvSelf;
	HxVector_t		 *pstList;
	HUINT32			ulChanged=0;
	apkMetaSvc_DxInfo_t * pstDxInfo;

	HUINT32	*pulUidCheck;
	HUINT32	ulIdx = 0;

	pstDxInfo = apk_meta_svc_GetDxInfo(pszName);

	HxLOG_Warning("Enter (type : %s, size : %d, size : %d)\n", pszName, pstDxInfo->ulSize, sizeof(DxTransponder_t));

	pulUidCheck = (HUINT32 *)HLIB_MEM_Calloc(sizeof(HUINT32) * ulNumberOfSvc);

#if defined(CONFIG_DEBUG)
	{
		HUID	*pulTemp = pulUidlist;

		for (i = ulNumberOfSvc;  i-- ; pulTemp++)
		{
			ulUid = (HUINT32)*pulTemp;
			HxLOG_Debug("[%s] [%d]svcuid : %d\n",__FUNCTION__,__LINE__, ulUid);
		}
	}
#endif

	DAPI_InitIterator(&stIter, pvData, ulSize, 0);

	pstMgr = apk_meta_svc_GetContext();

	if (DAPI_NextIterator(&stIter) == FALSE)
	{
		//2 Delete svclist
		HxLOG_Trace();
		for (i = ulNumberOfSvc;  i-- ; pulUidlist++)
		{
			HxSTD_memset(&stKey, 0, pstDxInfo->ulSize);
			ulUid = (HUINT32)*pulUidlist;
			stKey.common.uid = ulUid;

			ENTER_CRITICAL_APKMETASVC;
			HxLOG_Debug("Delete(type : %s, ulUid : %d)\n",pszName, ulUid);
			apk_meta_svc_RemoveObject(pstMgr, pszName, (const void *)&stKey);
			ulChanged++;
			LEAVE_CRITICAL_APKMETASVC;
		}
	}
	else
	{
		ENTER_CRITICAL_APKMETASVC;
		do
		{
			HxLOG_Trace();
			pvSelf = NULL;
			pvItem = (void *)stIter.data;
			pvSelf = apk_meta_svc_FindObject(apk_meta_svc_GetContext(), pszName, (const void *)pvItem);
			pulUidCheck[ulIdx++] = (HUINT32)((apkMetaSvc_Type_t *)pvItem)->common.uid;

			if (pvSelf)
			{
				//2 Update
				pvNew = (void *)APK_MemDup((void *)pvItem, pstDxInfo->ulSize);
				if(pvNew != NULL)
				{
					HxLOG_Print("Update(type : %s, uid : %d)\n", pszName, ((apkMetaSvc_Type_t *)pvNew)->common.uid);
					((apkMetaSvc_Type_t *)pvNew)->common.typeOf = pstDxInfo->pszName;
					if (apk_meta_svc_ReplaceObject(pstMgr, pszName, (const void *)pvNew) == NULL)
					{
						HxLOG_Error("[%s] [%d]  \n",__FUNCTION__,__LINE__);
					}
					ulChanged++;

					APK_Free(pvNew);
				}
			}
			else
			{
				//2 Add
				// DAMA 저장
				pstList = (HxVector_t *)HLIB_TREE_Lookup(pstMgr->pstListTree, pszName);
				if (pstList == NULL)
				{
					pstList  = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))apk_meta_svc_DeleteObject, apk_meta_svc_CompareVectorObject);
					HxLOG_Print("!!!!New List (type : %s)", pszName);
					// 비어있다면 그냥 모두 추가한다.
					HLIB_TREE_Insert(pstMgr->pstListTree, (void *)pstDxInfo->pszName, pstList);
				}

				pvNew = (void *)APK_MemDup((void *)pvItem, pstDxInfo->ulSize);
				if (pvNew == NULL)
				{
					HxLOG_Error("[%s] [%d] error. \n",__FUNCTION__,__LINE__);
				}
				else
				{
					HxLOG_Print("Add(type : %s, uid : %d)\n", pszName, ((apkMetaSvc_Type_t *)pvNew)->common.uid);
					((apkMetaSvc_Type_t *)pvNew)->common.typeOf = pstDxInfo->pszName;
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
					apk_meta_svc_RemoveObject(pstMgr, pszName, (const void *)&stKey);
					ulChanged++;
				}
			}
		}

		LEAVE_CRITICAL_APKMETASVC;
	}

	if (ulChanged > 0)
	{
		HxLOG_Debug("type : %s, ulChanged : %d\n", pszName, ulChanged);
		apk_meta_svc_EmitEvent();
	}

	if (pulUidCheck)
	{
		HLIB_STD_MemFree(pulUidCheck);
	}

	HLIB_STD_MemFree(pstPair);
}


STATIC void	apk_meta_svc_OnDamaServiceUpdated(const HCHAR *pszName, HUID hUid, const void *pvData, HUINT32 ulSize, void *pvUser)
{
	HUINT32		 ulNumberOfSvc = 0;
	HUID		*pulUidlist = (HUID *)pvData;
	HUINT32 	 ulUid;
	HxPair_t	*stItem;

	ulNumberOfSvc = (HUINT32)(ulSize / sizeof(HUID));
	HxLOG_Trace();

	// Reset 되어있는지 확인
	if (ulNumberOfSvc == 1)
	{
		ulUid = *pulUidlist;
		if (ulUid == -1)
		{
			// Delete all list
			apk_meta_svc_Reset(FALSE, pszName);

			if (HxSTD_StrNCmp(pszName, DxNAMEOF(DxService_t), HxSTD_StrLen(pszName)) == 0)
			{
				apk_meta_svc_EmitEvent();
			}
			return ;
		}
	}
	stItem= (HxPair_t *)HLIB_STD_MemCalloc(sizeof(HxPair_t));

	stItem->left = (void *)ulNumberOfSvc;
	stItem->right = (void *)pulUidlist;

	HxLOG_Warning("[%s] Service info Changed!! ulNumberOfSvc : %d\n", pszName, ulNumberOfSvc);

	DAPI_GetUIDInfos(pszName, (HUID*)pvData, ulNumberOfSvc, apk_meta_svc_DamaCallback, (void *)stItem);
}


// OPL에서 이벤트 받을 listener 거는 함수
static HERROR	apk_meta_svc_SetUpdateListener (apkMetaSvc_Ctx_t *pstMgr, ApkMetaService_Notifier_t pfOnUpdate, void *pvUserdata)
{
	apkMetaSvc_Notifier_t	*pfNotifier;
	HxLOG_Trace();

	pfNotifier = &pstMgr->pfNotifier;

	RETURN_IF(pfNotifier->pfOnUpdate == pfOnUpdate, ERR_FAIL);

	apk_meta_svc_SetDynamicUpdate(pstMgr, pfOnUpdate ? TRUE : FALSE);

	pfNotifier->pfOnUpdate	 = pfOnUpdate;
	pfNotifier->pvUserdata	 = pvUserdata;

	if(TRUE == pstMgr->bMainApp)	// MainApp only
	{
		// Load updated list from obama
		apk_meta_svc_LoadUpdatedList(FALSE);
	}

	return ERR_OK;
}

static void *	apk_meta_svc_ReplaceObject (apkMetaSvc_Ctx_t *pstMgr, const HCHAR *pszType, const void *newObj)
{
	HxVector_t	*pstList;
	HINT32		nIdx;
	void		*pvOld, *pvRet;
	apkMetaSvc_DxInfo_t *pstDxInfo;

	pstList  = (HxVector_t *)HLIB_TREE_Lookup(pstMgr->pstListTree, pszType);
	RETURN_IF(pstList == NULL, NULL);

	pstDxInfo = apk_meta_svc_GetDxInfo(pszType);
	if (pstDxInfo == NULL)
	{
		return NULL;
	}

	nIdx = HLIB_VECTOR_BSearch(pstList, newObj, NULL);
	RETURN_IF(nIdx < 0, NULL);

	pvOld = HLIB_VECTOR_ItemAt(pstList, nIdx);
	RETURN_IF(pvOld == NULL, NULL);
	RETURN_IF(memcmp(pvOld, newObj, pstDxInfo->ulSize) == 0, NULL);

	pvRet= apk_meta_svc_CloneObject(newObj);
	RETURN_IF(pvRet == NULL, NULL);

	pvOld = HLIB_VECTOR_Replace(pstList, nIdx, pvRet);
	if (pvOld)
	{
		apk_meta_svc_DeleteObject(pvOld);
	}
	return pvRet;
}

static HINT32	apk_meta_svc_RemoveServiceWithFlag(apkMetaSvc_Ctx_t *pstMgr, const DxSvcUpdateFlag_e eFlag)
{
	HINT32		i, j;
	HxVector_t	*pstList;

	pstList = (HxVector_t *)HLIB_TREE_Lookup(pstMgr->pstListTree, DxNAMEOF(DxService_t));

	if (pstList == NULL)
	{
		HxLOG_Error("No registered Service...\n");
		return 0;
	}

	i = j = 0;
	while (i < HLIB_VECTOR_Length(pstList))
	{
		DxService_t	*svcInfo;

		svcInfo = (DxService_t*)HLIB_VECTOR_ItemAt(pstList, i);
		if (svcInfo->updateSvcFlag & eFlag)
		{
			HLIB_VECTOR_Remove(pstList, i);
			j++;
			continue;
		}

		i++;
	}
	return j;
}

static HERROR	apk_meta_svc_changeUpdateFlag(apkMetaSvc_Ctx_t *pstMgr, const DxSvcUpdateFlag_e eFromFlag, const DxSvcUpdateFlag_e eToFlag)
{
	HINT32		i;
	HxVector_t	*pstList;

	pstList = (HxVector_t *)HLIB_TREE_Lookup(pstMgr->pstListTree, DxNAMEOF(DxService_t));
	if (pstList == NULL)
	{
		HxLOG_Error("No registered Service...\n");
		return ERR_FAIL;
	}

	i = 0;
	while (i < HLIB_VECTOR_Length(pstList))
	{
		DxService_t	*svcInfo;

		svcInfo = (DxService_t*)HLIB_VECTOR_ItemAt(pstList, i);
		if (svcInfo->updateSvcFlag & eFromFlag)
		{
			svcInfo->updateSvcFlag = eToFlag;
		}

		i ++;
	}
	return ERR_OK;
}

static HBOOL	apk_meta_svc_RemoveObject (apkMetaSvc_Ctx_t *pstMgr, const HCHAR *type, const void *key)
{
	HxVector_t	*pstList;
#if defined(APKMETASVC_USE_VCLONE)
	apkMetaSvc_VClone_t	stTemp;
#endif

	pstList = (HxVector_t *)HLIB_TREE_Lookup(pstMgr->pstListTree, type);
	RETURN_IF(pstList == NULL, FALSE);

	if (HLIB_VECTOR_Remove(pstList, HLIB_VECTOR_BSearch(pstList, key, NULL)) != ERR_OK)
	{
		return FALSE;
	}

#if defined(APKMETASVC_USE_VCLONE)
	pstList = (HxVector_t *)HLIB_TREE_Lookup(pstMgr->pstVCloneTree, type);
	RETURN_IF(pstList == NULL, FALSE);

	stTemp.pstItem = (apkMetaSvc_Type_t *)key;
	HLIB_VECTOR_Remove(pstList, HLIB_VECTOR_BSearch(pstList, &stTemp, NULL));
#endif
	return TRUE;
}

static void *	apk_meta_svc_FindObject (apkMetaSvc_Ctx_t *pstMgr, const HCHAR *type, const void *key)
{
	HxVector_t	*pstList;

	pstList = (HxVector_t *)HLIB_TREE_Lookup(pstMgr->pstListTree, type);
	RETURN_IF(pstList == NULL, NULL);

	return HLIB_VECTOR_ItemAt(pstList, HLIB_VECTOR_BSearch(pstList, key, NULL));
}

 //채널 추가를 위한 uid 발급 함수
static HERROR	apk_meta_svc_GetNextSvcKey(HINT32	*pnKey)
{
	HxVector_t *		pstList;
	apkMetaSvc_Ctx_t * pstMgr;
	HUINT32			ulIdx = 0;
	DxService_t *pstSvcInfo;
	HINT32		nLastKey = 0;

	if(pnKey == NULL)
	{
		HxLOG_Error("pnKey is NULL \n");
		return ERR_FAIL;
	}

	pstMgr  = apk_meta_svc_GetContext();

	pstList = (HxVector_t *)HLIB_TREE_Lookup(pstMgr->pstListTree, (void *)DxNAMEOF(DxService_t));
	if (pstList == NULL)
	{
		HxLOG_Error("[%s] [%d] Error  \n",__FUNCTION__,__LINE__);
		return ERR_FAIL;
	}

	// 가장 높은 숫자의 uid 찾기
	for (ulIdx = 0; ulIdx < HLIB_VECTOR_Length(pstList); ulIdx++)
	{
		pstSvcInfo = (DxService_t*)HLIB_VECTOR_ItemAt(pstList, ulIdx);
		if (nLastKey < pstSvcInfo->uid)
		{
			nLastKey = pstSvcInfo->uid;
		}
	}

	// Key 할당
	*pnKey = ++nLastKey;

	HxLOG_Debug("[%s] [%d] nLastKey : %d, next key : %d\n",__FUNCTION__,__LINE__, nLastKey, *pnKey);

	return ERR_OK;

}

#define _______RPC_related_func_apkmetasvc___________

static void		apk_meta_svc_SetDynamicUpdate (apkMetaSvc_Ctx_t *pstMgr, HBOOL bOnOff)
{
	HxLOG_Trace();

	OAPI_META_SVC_SetDynamicUpdate(bOnOff);
}

void apk_meta_svc_LoadPreprogrammedList(void)
{
	HxLOG_Trace();

	OAPI_META_SVC_LoadPreprogrammedList();
}

// 변경된 service list Load
// bFlag :
//           TRUE - 상위 레이어 Call, 변경된 list 있는지 여부 상관없이 Noti 올려주며 DAMA에 바로 저장하지 않는다.
//           FALSE -내부 Call, 변경된 list가 없다면 상위에 Noti하지 않고 DAMA에는 바로 저장
static void apk_meta_svc_LoadUpdatedList(HBOOL bFlag)
{
	HxLOG_Trace();

	OAPI_META_SVC_LoadUpdatedList(apk_meta_svc_OnResponse, bFlag);
}

// OBAMA event listener
static void	apk_meta_svc_oapi_Listener (void)
{
	HxLOG_Trace();

	// update 된 list 읽어와서 처리한다.
	apk_meta_svc_LoadUpdatedList(FALSE);
}

// Load full list
static HERROR	apk_meta_svc_Load (void)
{
	HxTREE_t		*pstTree;
	void			*pvArgs[2];
	HUINT32		ulIdx;
	apkMetaSvc_Ctx_t *pstMgr = apk_meta_svc_GetContext();

	HxLOG_Trace();

	pstTree = HLIB_TREE_NewFull(
				  (HxCompareDataFunc)HLIB_STD_StrCaseCmp
				, NULL
				, NULL
				, (HxDestroyNotify)HLIB_VECTOR_Delete
			);
	RETURN_IF(pstTree == NULL, ERR_FAIL);

	for(ulIdx = 0; ulIdx < apkMetaSvc_MAX_TABLE_NUM ; ulIdx++)
	{
		pvArgs[0] = (void *)NULL;// result (HxVector_t *)
		{
			HUINT32	reqId;

#if defined(CONFIG_DEBUG) && defined(CONFIG_FUNC_USE_DAPI_DIRECT_QUERY)
			DAPI_PrintMemUsed();
#endif
			reqId = DAPI_GetAll(cs_astDxInfo[ulIdx].pszName, (DAPI_Getter)apk_meta_svc_OnDataReceived, (void*)pvArgs);
			DAPI_Wait(60000, reqId);
			if (reqId == 0 || pvArgs[0] == NULL)
			{
				HxLOG_Warning("[%s] Cannot load %s from DAMA!\n", __FUNCTION__, cs_astDxInfo[ulIdx].pszName);
				continue;
			}
		}
		HxLOG_Info("[%s] Load %s from DAMA success(%d)\n", __FUNCTION__, cs_astDxInfo[ulIdx].pszName, HLIB_VECTOR_Length(pvArgs[0]));
		HLIB_TREE_Insert(pstTree, (void *)cs_astDxInfo[ulIdx].pszName, pvArgs[0]);

	}

	if (HLIB_TREE_Nnodes(pstTree) == 0)
	{
		HLIB_TREE_Destroy(pstTree);
		return ERR_FAIL;
	}

	if (pstMgr->pstListTree)
	{
		HLIB_TREE_Destroy(pstMgr->pstListTree);
	}
	pstMgr->pstListTree = pstTree;

	return ERR_OK;

}

static HERROR apk_meta_svc_SetService(const DxService_t *pstService)
{
	HERROR	hErr;

	hErr = (HERROR)DAPI_LazySet(DxNAMEOF(DxService_t), pstService->uid, pstService, sizeof(DxService_t), 300);
	if (hErr != ERR_OK)
	{
		HxLOG_Warning(" [%s] [%d]DAPI_LazySet() Fail. uid : %d\n",__FUNCTION__,__LINE__,pstService->uid);
	}

	return DAPI_LazySync(DxNAMEOF(DxService_t), 1000);
}

static HERROR apk_meta_svc_RemoveService(const DxService_t *pstService)
{
	HERROR	hErr;

	hErr = (HERROR)DAPI_LazySet(DxNAMEOF(DxService_t), pstService->uid, NULL, 0, 300);
	if (hErr != ERR_OK)
	{
		HxLOG_Warning(" [%s] [%d]DAPI_LazySet() Fail. uid : %d\n",__FUNCTION__,__LINE__,pstService->uid);
	}

	return DAPI_LazySync(DxNAMEOF(DxService_t), 1000);
}

static HERROR	apk_meta_svc_Save(void)
{
	void			*pvObj;
	HxVector_t		*pstList;
	apkMetaSvc_Ctx_t *pstMgr = apk_meta_svc_GetContext();
	HUINT32		ulIdx;
	HxLOG_Trace();

	HxLOG_Warning("[%s] [%d]-------SAVE -------\n",__FUNCTION__,__LINE__);
	OAPI_META_SVC_Commit();

	for(ulIdx = 0; ulIdx < apkMetaSvc_MAX_TABLE_NUM ; ulIdx++)
	{
		//2 Delete
		pstList  = (HxVector_t *)HLIB_TREE_Lookup(pstMgr->pstTrashTree, cs_astDxInfo[ulIdx].pszName);
		if (pstList)
		{
			DAPI_BatchBegin(cs_astDxInfo[ulIdx].pszName);
			HxVECTOR_EACH(pstList, apkMetaSvc_Type_t *, pvObj, {
				HxLOG_Debug("[DEL DAMA DB] Type : %s, uid : %d\n",cs_astDxInfo[ulIdx].pszName, ((apkMetaSvc_TrashInfo_t *)pvObj)->nUid);
				DAPI_Reset(cs_astDxInfo[ulIdx].pszName, ((apkMetaSvc_TrashInfo_t *)pvObj)->nUid);
			});
			DAPI_BatchEnd(cs_astDxInfo[ulIdx].pszName);

			HLIB_VECTOR_RemoveAll(pstList);
		}

		//2 Add, Update
		pstList  = (HxVector_t *)HLIB_TREE_Lookup(pstMgr->pstListTree, cs_astDxInfo[ulIdx].pszName);
		if (pstList == NULL)
		{
			DAPI_ResetTable(cs_astDxInfo[ulIdx].pszName);
			HxLOG_Info("[%s] Reset %s to DAMA success\n", __FUNCTION__, cs_astDxInfo[ulIdx].pszName);
		}
		else
		{
			 //Service의 경우 변경된 것들만 반영한다.
			if(HxSTD_StrCmp(cs_astDxInfo[ulIdx].pszName, DxNAMEOF(DxService_t)) == 0)
			{
				HUINT32	i;
				HUINT32	ulCnt =  HLIB_VECTOR_Length(pstList);
				DxService_t * pstSvc = NULL;

				for (i = 0 ; i < ulCnt ; i++)
				{
					pstSvc = (DxService_t *)HLIB_VECTOR_ItemAt(pstList, i);

					if (pstSvc->updateSvcFlag == eDxSVC_UPDATE_NEWCHANNEL || pstSvc->updateSvcFlag == eDxSVC_UPDATE_UPDATECHANNEL)
					{
						HxLOG_Debug("[ADD DAMA DB] type : %s, uid :%d\n",__FUNCTION__,__LINE__, cs_astDxInfo[ulIdx].pszName, pstSvc->uid);
						pstSvc->updateSvcFlag = eDxSVC_UPDATE_NORMAL;
						DAPI_LazySet(cs_astDxInfo[ulIdx].pszName, *(HINT32 *)pstSvc, (void *)pstSvc, cs_astDxInfo[ulIdx].ulSize, 100);
					}
				}
			}
			else
			{
				DAPI_ResetTable(cs_astDxInfo[ulIdx].pszName);
				DAPI_BatchBegin(cs_astDxInfo[ulIdx].pszName);
				HxVECTOR_EACH(pstList, void*, pvObj, {
				DAPI_Set(cs_astDxInfo[ulIdx].pszName, *(HINT32 *)pvObj, pvObj,cs_astDxInfo[ulIdx].ulSize);
				});
				DAPI_BatchEnd(cs_astDxInfo[ulIdx].pszName);
				HxLOG_Info("[%s] Save %s to DAMA success\n", __FUNCTION__, cs_astDxInfo[ulIdx].pszName);
			}
		}
		DAPI_Sync(cs_astDxInfo[ulIdx].pszName);
	}

	return ERR_OK;

}

static HERROR	apk_meta_svc_Reset (HBOOL bResetDb, const HCHAR *pszName)
{
	HxVector_t		*pstList;
	HERROR	hErr = ERR_FAIL;
	apkMetaSvc_Ctx_t * pstMgr;

	HxLOG_Trace();
	HxLOG_Warning("[%s] [%d]-------RESET Table : %s -------\n",__FUNCTION__,__LINE__, pszName);

	ENTER_CRITICAL_APKMETASVC;
	pstMgr  = apk_meta_svc_GetContext();

	pstList  = (HxVector_t *)HLIB_TREE_Lookup(pstMgr->pstListTree, pszName);
	if (pstList)
	{
		if (apk_meta_svc_RemoveAllObject(pstMgr, pszName))
		{
			if (bResetDb)
			{
				// DAMA Reset
				hErr = DAPI_ResetTable(pszName);
				hErr |= DAPI_Sync(pszName);
				if (hErr != ERR_OK)
				{
					HxLOG_Info("[%s] Reset %s to DAMA FAIL\n", __FUNCTION__, pszName);
				}

				// DAMA 와 sync 문제로 특별히 Obama DB만  Reset 하도록 한다.
				// Remove All Service
				OAPI_META_SVC_Reset((HCHAR *)pszName);
			}
		}
	}
	LEAVE_CRITICAL_APKMETASVC;

	return hErr;
}

#if defined(APKMETASVC_USE_VCLONE)
static void		apk_meta_svc_VDeleteObject (void *pvSelf)
{
	apkMetaSvc_Type_t	*pstThiz = (apkMetaSvc_Type_t *)pvSelf;
	apkMetaSvc_Ctx_t 	*pstMgr = apk_meta_svc_GetContext();
	HxVector_t		*pstVList = NULL;
	apkMetaSvc_VClone_t	*pstVClone;
	apkMetaSvc_VClone_t	stTemp;
	HINT32	nIdx;

	HxLOG_Assert(pstThiz->common.typeOf);

	if (pstThiz)
	{
		HUINT32	idx;
		idx= pstThiz->common.uid;
		if (HxSTD_StrCmp(pstThiz->common.typeOf, DxNAMEOF(DxService_t)) == 0)
		{
			pstVList = HLIB_TREE_Lookup(pstMgr->pstVCloneTree, pstThiz->common.typeOf);
			if (pstVList)
			{
				stTemp.pstItem = pstThiz;
				nIdx =  HLIB_VECTOR_BSearch(pstVList, &stTemp, NULL);
				pstVClone = HLIB_VECTOR_ItemAt(pstVList, nIdx);

				if (pstVClone)
				{
					pstVClone->nRefCnt--;
					if (pstVClone->nRefCnt <= 0)
					{
						HLIB_VECTOR_Remove(pstVList, nIdx);
					}
				}
			}
		}
		else
		{

			HxLOG_Assert(pstThiz->common.typeOf);
			APK_Free(pstThiz);
		}
	}

}


static void		apk_meta_svc_DeleteVCloneObject (void *pvSelf)
{
	apkMetaSvc_VClone_t	*pstThiz = (apkMetaSvc_VClone_t *)pvSelf;

	if (pstThiz)
	{
		if (pstThiz->pstItem) APK_Free(pstThiz->pstItem);
		APK_Free(pstThiz);
	}
}

static HINT32	apk_meta_svc_CompareVCloneObject (const void *pvArg1, const void *pvArg2)
{
	const void *pvObj1 = *(const void **)pvArg1;
	const void *pvObj2 = *(const void **)pvArg2;

	return (HINT32) ( ((apkMetaSvc_VClone_t *)pvObj1)->pstItem->common.uid - ((apkMetaSvc_VClone_t *)pvObj2)->pstItem->common.uid );
}

static void	*	apk_meta_svc_VCloneObject (const void *pvSelf)
{
	apkMetaSvc_Type_t    *pstThiz = (apkMetaSvc_Type_t *)pvSelf;
	apkMetaSvc_DxInfo_t * pstDxInfo;
	apkMetaSvc_Type_t    *pstClone= NULL;
	apkMetaSvc_Ctx_t 	*pstMgr = apk_meta_svc_GetContext();
	HxVector_t		*pstVList = NULL;
	apkMetaSvc_VClone_t *pstVClone = NULL;
	HBOOL		bNew = TRUE;
	HUINT32		idx;
	apkMetaSvc_VClone_t	stTemp;
	RETURN_IF(pvSelf == NULL, NULL);

	HxLOG_Assert(pstThiz->common.typeOf);


	idx= pstThiz->common.uid;
	if (HxSTD_StrCmp(pstThiz->common.typeOf, DxNAMEOF(DxService_t)) == 0)
	{
		pstVList = HLIB_TREE_Lookup(pstMgr->pstVCloneTree, pstThiz->common.typeOf);
		if (pstVList == NULL)
		{
			pstVList  = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))apk_meta_svc_DeleteVCloneObject, apk_meta_svc_CompareVCloneObject);
			HLIB_TREE_Insert(pstMgr->pstVCloneTree, (void *)pstThiz->common.typeOf, pstVList);
		}
		else
		{
			stTemp.pstItem = pstThiz;
			pstVClone = HLIB_VECTOR_ItemAt(pstVList, HLIB_VECTOR_BSearch(pstVList, &stTemp, NULL));

			if (pstVClone)
			{
				pstClone = pstVClone->pstItem;
				pstClone->common.typeOf = pstThiz->common.typeOf;
				bNew = FALSE;
				pstVClone->nRefCnt++;
			}
		}

		if (bNew)
		{
			pstVClone = APK_Calloc(sizeof(apkMetaSvc_VClone_t));

			pstClone = apk_meta_svc_CloneObject(pvSelf);
			pstVClone->pstItem = pstClone;
			pstVClone->pstItem->common.typeOf = pstThiz->common.typeOf;
			HLIB_VECTOR_Add(pstVList, pstVClone);
			pstVClone->nRefCnt++;
			HLIB_VECTOR_Sort(pstVList, NULL);
		}
	}
	else
	{
		pstDxInfo = apk_meta_svc_GetDxInfo(pstThiz->common.typeOf);

		pstClone = (apkMetaSvc_Type_t *)APK_Calloc(pstDxInfo->ulSize);
		if (pstClone)
		{
			memcpy(pstClone, pvSelf, pstDxInfo->ulSize);
			pstClone->common.typeOf = pstThiz->common.typeOf;
		}
	}
	return pstClone;
}

#endif


#ifdef	apkMetaSvc_CHLOGO

static void	apk_meta_svc_dapi_OnChLogoReceived(const HCHAR *pszName, HUID nUid, const void *pvData, HUINT32 ulSize, void *pvUser)
{
	HxTREE_t 		*pstMap;
	DAxIterator_t pvIter;
	DxChannelLogo_t	*pstLogo;

	if (pvData && ulSize > 0)
	{
		pstMap = HLIB_TREE_NewFull((HxCompareDataFunc)apk_meta_svc_CompareINT32, NULL, NULL, (HxDestroyNotify)APK_Free_CB);
		RETURN_IF_VOID(pstMap == NULL);

		DAPI_InitIterator(&pvIter, pvData, ulSize, sizeof(DxChannelLogo_t));
		while (DAPI_NextIterator(&pvIter))
		{
			pstLogo = (DxChannelLogo_t *)APK_MemDup(pvIter.data, sizeof(DxChannelLogo_t));
			if (pstLogo)
			{
				HLIB_TREE_Insert(pstMap, (void *)pstLogo->svcuid, pstLogo);
			}
		}
		if (HLIB_TREE_Nnodes(pstMap) == 0)
		{
			HLIB_TREE_Destroy(pstMap);
			pstMap = NULL;
		}
	}
	else
	{
		pstMap = NULL;
	}
	ENTER_CRITICAL_APKMETASVC;
	{
		apkMetaSvc_Ctx_t *pstMgr = apk_meta_svc_GetContext();
		if (pstMgr->pstChLogoMap)
			HLIB_TREE_Destroy(pstMgr->pstChLogoMap);
		pstMgr->pstChLogoMap = pstMap;

		HxLOG_Debug("%s(%s)!\n", __FUNCTION__, pszName);
	}
	LEAVE_CRITICAL_APKMETASVC;
}

static void	apk_meta_svc_dapi_OnChLogoUpdated(const HCHAR *pszName, HUID nUid, const void *pvData, HUINT32 ulSize, void *pvUser)
{
	(void)nUid;
	(void)pvData;
	(void)ulSize;
	(void)pvUser;

	HxLOG_Trace();
	DAPI_GetAll(pszName, apk_meta_svc_dapi_OnChLogoReceived, NULL);
}
#endif

#define	____________META_SVC_API________________

// bMainApp		TRUE : Channel list 수정가능한 App (ex. Corsair)
//				FALSE : Main App가 아닌 단순 List 참고용 App (ex. homma, ipepg, prism....)
HBOOL	APK_META_SVC_Init (HBOOL bMainApp)
{
	apkMetaSvc_Ctx_t	*pstMgr;
	HUINT32	ulIdx;

	HxLOG_Trace();

	HxSEMT_Create(&s_ulMetaSvcSemId, "sametasvc", HxSEMT_FIFO);

	pstMgr  = apk_meta_svc_GetContext();
	pstMgr->nVersion = 0;
	pstMgr->bMainApp = bMainApp;

	ENTER_CRITICAL_APKMETASVC;

	if (TRUE == bMainApp)
	{
		// Main APP Only
		OAPI_META_SVC_SetListener(apk_meta_svc_oapi_Listener, NULL);
	}
	LEAVE_CRITICAL_APKMETASVC;

	// Load full list from dama
	apk_meta_svc_Load();

#ifdef	apkMetaSvc_CHLOGO
	DAPI_GetAll(DxNAMEOF(DxChannelLogo_t), apk_meta_svc_dapi_OnChLogoReceived, NULL);
	DAPI_AddTableNotifier(DxNAMEOF(DxChannelLogo_t), apk_meta_svc_dapi_OnChLogoUpdated, NULL);
#endif

	if (FALSE == bMainApp)
	{
		for(ulIdx = 0 ; ulIdx < apkMetaSvc_MAX_TABLE_NUM ; ulIdx++)
		{
			DAPI_AddTableNotifier(cs_astDxInfo[ulIdx].pszName, apk_meta_svc_OnDamaServiceUpdated, NULL);
		}
	}

#if defined(CONFIG_DEBUG)
	apk_meta_svc_InitCommand();
#endif

	return TRUE;
}

HBOOL	APK_META_SVC_DeInit (void)
{
	apkMetaSvc_Ctx_t	*pstMgr;

	pstMgr  = apk_meta_svc_GetContext();

	ENTER_CRITICAL_APKMETASVC;
	OAPI_META_SVC_SetListener(NULL, NULL);

	if (pstMgr->pstListTree)
		HLIB_TREE_Destroy(pstMgr->pstListTree);

	LEAVE_CRITICAL_APKMETASVC;
	HxSEMT_Destroy(s_ulMetaSvcSemId);

	return TRUE;
}

HBOOL	APK_META_SVC_AllowTableNotifyMyself(HCHAR *pTableName, HBOOL bAllow)
{
	return DAPI_AllowTableNotifyMyself(pTableName, bAllow);
}

//nOnid : if -1, it means all Onid
//nTsid : if -1, it means all Tsid
//nSvcid : if -1, it means all Svcid
HxVector_t *	APK_META_SVC_FindServiceByTriplet (HINT32 nOnid, HINT32 nTsid, HINT32 nSvcid)
{
	DxService_t	*pstService;
	HxVector_t	*pstList;
	HxVector_t	*pstResult;
	apkMetaSvc_Ctx_t *pstMgr;

	HBOOL	bOnid, bTsid, bSvcid;

	pstResult = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))apk_meta_svc_DeleteObject, apk_meta_svc_CompareVectorObject);
	RETURN_IF(pstResult == NULL, NULL);

	ENTER_CRITICAL_APKMETASVC;
	pstMgr  = apk_meta_svc_GetContext();
	pstList = (HxVector_t *)HLIB_TREE_Lookup(pstMgr->pstListTree, DxNAMEOF(DxService_t));
	if (pstList == NULL)
	{
		LEAVE_CRITICAL_APKMETASVC;
		HLIB_VECTOR_Delete(pstResult);
		return NULL;
	}

	HxVECTOR_EACH(pstList, DxService_t *, pstService, {
		if (pstService == NULL)
			continue;

		bOnid = bTsid = bSvcid = TRUE;

		if (nOnid != -1) 	{	if (pstService->onid != nOnid)	bOnid = FALSE;	}
		if (nTsid != -1) 	{	if (pstService->tsid != nTsid)		bTsid = FALSE;		}
		if (nSvcid != -1)	{	if (pstService->svcid != nSvcid)	bSvcid = FALSE;	}

		if (bOnid != TRUE || bTsid != TRUE ||bSvcid != TRUE )
			continue;

		pstService = (DxService_t *)APK_MemDup((void *)pstService, sizeof(DxService_t));
		if (pstService == NULL)
			continue;
		HLIB_VECTOR_Add(pstResult, pstService);
	});
	LEAVE_CRITICAL_APKMETASVC;

	if (HLIB_VECTOR_Length(pstResult) == 0)
	{
		HLIB_VECTOR_Delete(pstResult);
		return NULL;
	}
	return pstResult;
}

HxVector_t *	APK_META_SVC_FindServiceBySvcId (HINT32 nSvcid)
{
	DxService_t	*pstService;
	HxVector_t	*pstList;
	HxVector_t	*pstResult;
	apkMetaSvc_Ctx_t *pstMgr;

	HBOOL	bOnid, bTsid, bSvcid;

	pstResult = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))apk_meta_svc_DeleteObject, apk_meta_svc_CompareVectorObject);
	RETURN_IF(pstResult == NULL, NULL);

	ENTER_CRITICAL_APKMETASVC;
	pstMgr  = apk_meta_svc_GetContext();
	pstList = (HxVector_t *)HLIB_TREE_Lookup(pstMgr->pstListTree, DxNAMEOF(DxService_t));
	if (pstList == NULL)
	{
		LEAVE_CRITICAL_APKMETASVC;
		HLIB_VECTOR_Delete(pstResult);
		return NULL;
	}

	HxVECTOR_EACH(pstList, DxService_t *, pstService, {
		if (pstService == NULL)
			continue;

		bOnid = bTsid = bSvcid = TRUE;

		if (nSvcid != -1)	{	if (pstService->svcid != nSvcid)	bSvcid = FALSE;	}

		if (bOnid != TRUE || bTsid != TRUE ||bSvcid != TRUE )
			continue;

		pstService = (DxService_t *)APK_MemDup((void *)pstService, sizeof(DxService_t));
		if (pstService == NULL)
			continue;
		HLIB_VECTOR_Add(pstResult, pstService);
	});
	LEAVE_CRITICAL_APKMETASVC;

	if (HLIB_VECTOR_Length(pstResult) == 0)
	{
		HLIB_VECTOR_Delete(pstResult);
		return NULL;
	}
	return pstResult;
}



HxVector_t *	APK_META_SVC_FindServiceByNumber (HINT32 nNumber)
{
	DxService_t	*pstService;
	HxVector_t	*pstList;
	HxVector_t	*pstResult;
	apkMetaSvc_Ctx_t *pstMgr;

	pstResult = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))apk_meta_svc_DeleteObject, apk_meta_svc_CompareVectorObject);
	RETURN_IF(pstResult == NULL, NULL);

	ENTER_CRITICAL_APKMETASVC;
	pstMgr  = apk_meta_svc_GetContext();
	pstList = (HxVector_t *)HLIB_TREE_Lookup(pstMgr->pstListTree, DxNAMEOF(DxService_t));
	if (pstList == NULL)
	{
		LEAVE_CRITICAL_APKMETASVC;
		HLIB_VECTOR_Delete(pstResult);
		return NULL;
	}

	HxVECTOR_EACH(pstList, DxService_t *, pstService, {
		if (pstService == NULL)
			continue;

		if (pstService->lcn != nNumber)
			continue;

		pstService = (DxService_t *)APK_MemDup((void *)pstService, sizeof(DxService_t));
		if (pstService == NULL)
			continue;
		HLIB_VECTOR_Add(pstResult, pstService);
	});
	LEAVE_CRITICAL_APKMETASVC;

	if (HLIB_VECTOR_Length(pstResult) == 0)
	{
		HLIB_VECTOR_Delete(pstResult);
		return NULL;
	}
	return pstResult;
}

void	APK_META_SVC_Delete (void *pvObject)
{
#if defined(APKMETASVC_USE_VCLONE)
	apk_meta_svc_VDeleteObject((void *)pvObject);
#else
	apk_meta_svc_DeleteObject((void *)pvObject);
#endif
}

void *	APK_META_SVC_Clone (const void *pvObject)
{
#if defined(APKMETASVC_USE_VCLONE)
	return apk_meta_svc_VCloneObject(pvObject);
#else
	return apk_meta_svc_CloneObject(pvObject);
#endif
}


DxService_t *	APK_META_SVC_GetService (HINT32 nUid)
{
	DxService_t	stKey;
	void *pvSelf;

	HxLOG_Trace();

	ENTER_CRITICAL_APKMETASVC;
	stKey.uid = nUid;
	pvSelf = apk_meta_svc_FindObject(apk_meta_svc_GetContext(), DxNAMEOF(DxService_t), &stKey);
	pvSelf = apk_meta_svc_CloneObject(pvSelf);
	LEAVE_CRITICAL_APKMETASVC;

	return (DxService_t *)pvSelf;
}

HBOOL			APK_META_SVC_GetServiceTriplet (HINT32 nUid, HINT32 *pnTsid, HINT32 *pnOnid, HINT32 *pnSvcid)
{
	DxService_t	stKey;
	DxService_t *pstRet;

	HxLOG_Trace();

	ENTER_CRITICAL_APKMETASVC;
	stKey.uid = nUid;
	pstRet = (DxService_t *)apk_meta_svc_FindObject(apk_meta_svc_GetContext(), DxNAMEOF(DxService_t), &stKey);
	if (pstRet)
	{
		if (pnTsid)
			*pnTsid = pstRet->tsid;
		if (pnOnid)
			*pnOnid = pstRet->onid;
		if (pnSvcid)
			*pnSvcid = pstRet->svcid;
	}
	LEAVE_CRITICAL_APKMETASVC;

	return pstRet ? TRUE : FALSE;
}

DxNetwork_t *	APK_META_SVC_GetNetwork (HINT32 nUid)
{
	DxNetwork_t	stKey;
	void *pvSelf;

	HxLOG_Trace();

	ENTER_CRITICAL_APKMETASVC;
	stKey.uid = nUid;
	pvSelf = apk_meta_svc_FindObject(apk_meta_svc_GetContext(), DxNAMEOF(DxNetwork_t), &stKey);
	pvSelf = apk_meta_svc_CloneObject(pvSelf);
	LEAVE_CRITICAL_APKMETASVC;

	return (DxNetwork_t *)pvSelf;
}

DxTransponder_t *	APK_META_SVC_GetTransponder (HINT32 nUid)
{
	DxTransponder_t	stKey;
	void *pvSelf;

	HxLOG_Trace();

	ENTER_CRITICAL_APKMETASVC;
	stKey.uid = nUid;
	pvSelf = apk_meta_svc_FindObject(apk_meta_svc_GetContext(), DxNAMEOF(DxTransponder_t), &stKey);
	pvSelf = apk_meta_svc_CloneObject(pvSelf);
	LEAVE_CRITICAL_APKMETASVC;

	return (DxTransponder_t *)pvSelf;
}


DxProvider_t *	APK_META_SVC_GetProvider (HINT32 nUid)
{
	DxProvider_t	stKey;
	void *pvSelf;

	HxLOG_Trace();

	ENTER_CRITICAL_APKMETASVC;
	stKey.uid = nUid;
	pvSelf = apk_meta_svc_FindObject(apk_meta_svc_GetContext(), DxNAMEOF(DxProvider_t), &stKey);
	pvSelf = apk_meta_svc_CloneObject(pvSelf);
	LEAVE_CRITICAL_APKMETASVC;

	return (DxProvider_t *)pvSelf;
}

DxGroup_t *	APK_META_SVC_GetGroup (HINT32 nUid)
{
	DxGroup_t	stKey;
	void *pvSelf;

	//HxLOG_Trace();

	ENTER_CRITICAL_APKMETASVC;
	stKey.uid = nUid;
	pvSelf = apk_meta_svc_FindObject(apk_meta_svc_GetContext(), DxNAMEOF(DxGroup_t), &stKey);
	pvSelf = apk_meta_svc_CloneObject(pvSelf);
	LEAVE_CRITICAL_APKMETASVC;

	return (DxGroup_t *)pvSelf;
}

DxBouquet_t*	APK_META_SVC_GetBouquet (HINT32 nUid)
{
	DxBouquet_t	stKey;
	void *pvSelf;

	HxLOG_Trace();

	ENTER_CRITICAL_APKMETASVC;
	stKey.uid = nUid;
	pvSelf = apk_meta_svc_FindObject(apk_meta_svc_GetContext(), DxNAMEOF(DxBouquet_t), &stKey);
	pvSelf = apk_meta_svc_CloneObject(pvSelf);
	LEAVE_CRITICAL_APKMETASVC;

	return (DxBouquet_t *)pvSelf;
}

DxChannelLogo_t *	APK_META_SVC_GetLogoUrl(HINT32 nUid, DxChannelLogo_t *pstBuf)
{
#ifdef	apkMetaSvc_CHLOGO
	apkMetaSvc_Ctx_t	*pstMgr = NULL;
	DxChannelLogo_t		*pstLogo = NULL;

	ENTER_CRITICAL_APKMETASVC;
	pstMgr  = apk_meta_svc_GetContext();

	if (pstMgr != NULL && pstMgr->pstChLogoMap)
	{
		pstLogo = (DxChannelLogo_t *)HLIB_TREE_Lookup(pstMgr->pstChLogoMap, (void *)nUid);
		if (pstLogo)
		{
			if (pstBuf)
			{
				HxSTD_MemCopy(pstBuf, pstLogo, sizeof(DxChannelLogo_t));
				pstLogo = pstBuf;
			}
			else
			{
				pstLogo = (DxChannelLogo_t *)APK_MemDup(pstLogo, sizeof(DxChannelLogo_t));
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

	LEAVE_CRITICAL_APKMETASVC;
	return pstLogo;
#endif
}


HxVector_t *	APK_META_SVC_GetServiceList (void)
{
	HxVector_t	*pstList;
	HxVector_t	*pstResult;
	apkMetaSvc_Ctx_t *pstMgr;

	ENTER_CRITICAL_APKMETASVC;
	pstMgr  = apk_meta_svc_GetContext();
	pstList = (HxVector_t *)HLIB_TREE_Lookup(pstMgr->pstListTree, DxNAMEOF(DxService_t));
	if (pstList)
	{
		pstResult = HLIB_VECTOR_CloneEx(pstList, (void *(*)(void *))apk_meta_svc_CloneObject);
	}
	else
	{
		pstResult = NULL;
	}
	LEAVE_CRITICAL_APKMETASVC;

	RETURN_IF(pstResult == NULL, NULL);
	if (HLIB_VECTOR_Length(pstResult) == 0)
	{
		HLIB_VECTOR_Delete(pstResult);
		return NULL;
	}
	return pstResult;
}

HxVector_t *	APK_META_SVC_GetTransponderList (void)
{
	HxVector_t	*pstList;
	HxVector_t	*pstResult;
	apkMetaSvc_Ctx_t *pstMgr;

	ENTER_CRITICAL_APKMETASVC;
	pstMgr  = apk_meta_svc_GetContext();
	pstList = (HxVector_t *)HLIB_TREE_Lookup(pstMgr->pstListTree, DxNAMEOF(DxTransponder_t));
	if (pstList)
	{
		pstResult = HLIB_VECTOR_CloneEx(pstList, (void *(*)(void *))apk_meta_svc_CloneObject);
	}
	else
	{
		pstResult = NULL;
	}
	LEAVE_CRITICAL_APKMETASVC;

	RETURN_IF(pstResult == NULL, NULL);
	if (HLIB_VECTOR_Length(pstResult) == 0)
	{
		HLIB_VECTOR_Delete(pstResult);
		return NULL;
	}
	return pstResult;
}

HxVector_t *	APK_META_SVC_GetNetworkList (void)
{
	HxVector_t	*pstList;
	HxVector_t	*pstResult;
	apkMetaSvc_Ctx_t *pstMgr;

	ENTER_CRITICAL_APKMETASVC;
	pstMgr  = apk_meta_svc_GetContext();
	pstList = (HxVector_t *)HLIB_TREE_Lookup(pstMgr->pstListTree, DxNAMEOF(DxNetwork_t));
	if (pstList)
	{
		pstResult = HLIB_VECTOR_CloneEx(pstList, (void *(*)(void *))apk_meta_svc_CloneObject);
	}
	else
	{
		pstResult = NULL;
	}
	LEAVE_CRITICAL_APKMETASVC;

	RETURN_IF(pstResult == NULL, NULL);
	if (HLIB_VECTOR_Length(pstResult) == 0)
	{
		HLIB_VECTOR_Delete(pstResult);
		return NULL;
	}
	return pstResult;
}

HxVector_t *	APK_META_SVC_GetGroupList (void)
{
	HxVector_t	*pstList;
	HxVector_t	*pstResult;
	apkMetaSvc_Ctx_t *pstMgr;

	ENTER_CRITICAL_APKMETASVC;
	pstMgr  = apk_meta_svc_GetContext();
	pstList = (HxVector_t *)HLIB_TREE_Lookup(pstMgr->pstListTree, DxNAMEOF(DxGroup_t));
	if (pstList)
	{
		pstResult = HLIB_VECTOR_CloneEx(pstList, (void *(*)(void *))apk_meta_svc_CloneObject);
	}
	else
	{
		pstResult = NULL;
	}
	LEAVE_CRITICAL_APKMETASVC;

	RETURN_IF(pstResult == NULL, NULL);
	if (HLIB_VECTOR_Length(pstResult) == 0)
	{
		HLIB_VECTOR_Delete(pstResult);
		return NULL;
	}
	return pstResult;
}


HERROR		APK_META_SVC_SetListener (ApkMetaService_Notifier_t pfOnUpdate, void *pvUserdata)
{
	HERROR	hErr;

	HxLOG_Trace();

	ENTER_CRITICAL_APKMETASVC;
	hErr = apk_meta_svc_SetUpdateListener(apk_meta_svc_GetContext(), pfOnUpdate, pvUserdata);
	LEAVE_CRITICAL_APKMETASVC;

	return hErr;
}

void		APK_META_SVC_SetUpdateChecker (APK_MetaUpdateChecker pfChecker)
{
	apkMetaSvc_Ctx_t * pstMgr;

	HxLOG_Trace();

	ENTER_CRITICAL_APKMETASVC;
	pstMgr = apk_meta_svc_GetContext();
	pstMgr->pfChecker = pfChecker;
	LEAVE_CRITICAL_APKMETASVC;
}


HERROR	APK_META_SVC_ChangeUpdateFlag(DxSvcUpdateFlag_e eFromFlag, DxSvcUpdateFlag_e eToFlag)
{
	HERROR	hErr;
	HxLOG_Trace();

	ENTER_CRITICAL_APKMETASVC;
	hErr = apk_meta_svc_changeUpdateFlag(apk_meta_svc_GetContext(), eFromFlag, eToFlag);
	LEAVE_CRITICAL_APKMETASVC;

	return hErr;
}

HERROR	APK_META_SVC_SetService (const DxService_t *pstService)
{
	HERROR	hErr;
	apkMetaSvc_Ctx_t * pstMgr;

	HxLOG_Trace();
	HxLOG_Assert(pstService);

	ENTER_CRITICAL_APKMETASVC;
	pstMgr  = apk_meta_svc_GetContext();
	if (apk_meta_svc_ReplaceObject(pstMgr, DxNAMEOF(DxService_t), (const void *)pstService) == NULL)
	{
		LEAVE_CRITICAL_APKMETASVC;
		return ERR_FAIL;
	}
	hErr = apk_meta_svc_SetService(pstService);
	LEAVE_CRITICAL_APKMETASVC;

	return hErr;
}


HERROR	APK_META_SVC_AddService (DxService_t *pstService)
{
	HERROR	hErr;
	apkMetaSvc_Ctx_t * pstMgr;
	HxVector_t *		pstList;
	HINT32			nKey = 0;
	DxService_t		*pstNewSvc = NULL;

	HxLOG_Trace();
	HxLOG_Assert(pstService);

	// uid 발급
	hErr = apk_meta_svc_GetNextSvcKey(&nKey);

	pstService->uid = nKey;

	ENTER_CRITICAL_APKMETASVC;
	pstMgr  = apk_meta_svc_GetContext();

	// DAMA 저장
	pstList = (HxVector_t *)HLIB_TREE_Lookup(pstMgr->pstListTree, DxNAMEOF(DxService_t));
	if (pstList != NULL)
	{
		pstNewSvc = (DxService_t *)APK_MemDup((void *)pstService, sizeof(DxService_t));
		if (pstNewSvc == NULL)
		{
			HxLOG_Error("[%s] [%d] error. \n",__FUNCTION__,__LINE__);
			return ERR_FAIL;
		}

		HLIB_VECTOR_Add(pstList, (void *)pstNewSvc);

		hErr = apk_meta_svc_SetService((const DxService_t *)pstNewSvc);

		HLIB_VECTOR_Sort(pstList, NULL);
	}

	LEAVE_CRITICAL_APKMETASVC;

	return hErr;
}

HERROR	APK_META_SVC_RemoveServiceWithFlag(DxSvcUpdateFlag_e eFlag)
{
	apkMetaSvc_Ctx_t * pstMgr;
	ApkMetaService_Notifier_t pfOnUpdate = NULL;
	void *				pvUserdata = NULL;
	HINT32				nArgs[2];

	HxLOG_Trace();

	ENTER_CRITICAL_APKMETASVC;
	pstMgr = apk_meta_svc_GetContext();
	if (apk_meta_svc_RemoveServiceWithFlag(pstMgr, eFlag) > 0)
	{
		pfOnUpdate = pstMgr->pfNotifier.pfOnUpdate;
		pvUserdata = pstMgr->pfNotifier.pvUserdata;
		nArgs[0] = pstMgr->nVersion;
		nArgs[1] = (HINT32)TRUE;
	}
	LEAVE_CRITICAL_APKMETASVC;

	// emit 'updated'
	if (pfOnUpdate)
	{
		pfOnUpdate(0, nArgs, pvUserdata);
	}
	return ERR_OK;
}

HERROR		APK_META_SVC_RemoveService (const DxService_t *pstService)
{
	HERROR	hErr = ERR_FAIL;
	apkMetaSvc_Ctx_t * pstMgr;

	HxLOG_Trace();
	HxLOG_Assert(pstService);

	ENTER_CRITICAL_APKMETASVC;
	pstMgr  = apk_meta_svc_GetContext();
	if (apk_meta_svc_RemoveObject(pstMgr, DxNAMEOF(DxService_t), (const void *)pstService) == FALSE)
	{
		LEAVE_CRITICAL_APKMETASVC;
		return ERR_FAIL;
	}
	hErr = apk_meta_svc_RemoveService(pstService);
	LEAVE_CRITICAL_APKMETASVC;

	return hErr;
}

HERROR	APK_META_SVC_Load (void)
{
	HxLOG_Trace();

	return apk_meta_svc_Load();
}

HERROR	APK_META_SVC_Save (void)
{
	HxLOG_Trace();

	return apk_meta_svc_Save();
}

HERROR	APK_META_SVC_Reset (void)
{
	HUINT32	ulIdx;
	HxLOG_Trace();

	for(ulIdx = 0; ulIdx < apkMetaSvc_MAX_TABLE_NUM ; ulIdx++)
	{
		apk_meta_svc_Reset(TRUE, cs_astDxInfo[ulIdx].pszName);
	}

	return ERR_OK;
}

HERROR	APK_META_SVC_LoadupdatedList (void)
{
	HxLOG_Trace();
	apk_meta_svc_LoadUpdatedList(TRUE);

	return ERR_OK;
}

HERROR	APK_META_SVC_LoadPreferredList (void)
{
	HxLOG_Trace();

	apk_meta_svc_LoadPreprogrammedList();

	return ERR_OK;
}

#define _______________DEBUG__________________

#ifdef apkMetaSvc_DUMP_ON_UPDATE
static void			apk_meta_svc_DumpAll (apkMetaSvc_Ctx_t *pstMgr, HxTREE_t *pstListTree, HxTREE_t *pstUidTree, const HCHAR *pszName)
{
#if defined(CONFIG_USES_NFSROOT)
	FILE	*fp;
	void	*pvIter;
	HCHAR	szFilename[256];
	void	*pvObj;
	HxVector_t		*pstList;
	apkMetaSvc_Type_t *pstItem;

	if (pstListTree == NULL)
		pstListTree = pstMgr->pstListTree;

	HxSTD_PrintToStrN(szFilename, 256, "%04d.%s.dump", pstMgr->nVersion, pszName ? pszName : "metasvc");

	fp = fopen(szFilename, "w");
	RETURN_IF_VOID(fp == NULL);

	if (pstUidTree)
	{
		fprintf(fp, "\nUID pstTree\n");
		for ( pvIter = HLIB_TREE_Iterator(pstUidTree)
			; pvIter
			; pvIter = HLIB_TREE_NextIterator(pvIter) )
		{
			pstList  = (HxVector_t *)HLIB_TREE_IteratorValue(pvIter);
			if (pstList == NULL)
				continue;

			fprintf(fp, "\n%s\n",(HCHAR *)HLIB_TREE_IteratorKey(pvIter));

			HxVECTOR_EACH(pstList, apkMetaSvc_Type_t *, pstItem, {
				fprintf(fp, "\t%12d\n", pstItem->common.uid);
			});
		}
	}

	for ( pvIter = HLIB_TREE_Iterator(pstListTree)
		; pvIter
		; pvIter = HLIB_TREE_NextIterator(pvIter) )
	{
		pstList  = (HxVector_t *)HLIB_TREE_IteratorValue(pvIter);
		if (pstList == NULL)
			continue;

		fprintf(fp, "\n%s\n", (HCHAR *)HLIB_TREE_IteratorKey(pvIter));

		HxVECTOR_EACH(pstList, void *, pvObj, {
			DLIB_Print((HCHAR *)HLIB_TREE_IteratorKey(pvIter), fp, pvObj);
		});
	}
	fclose(fp);
#else
	(void)pstMgr;
	(void)pstListTree;
	(void)pszName;
#endif
}
#endif


#if defined(CONFIG_DEBUG)
static HINT32	apk_meta_svc_command_load_from_dama(void *pvArg)
{
	APK_META_SVC_Load();

	return 0;
}

static HINT32	apk_meta_svc_command_save_to_dama(void *pvArg)
{
	APK_META_SVC_Save();

	return 0;
}

static void apk_meta_svc_GetSvcType(DxSvcType_e eType, HCHAR *pszTypeName)
{
	switch(eType)
	{
	case eDxSVC_TYPE_TV:
		HxSTD_StrNCpy(pszTypeName,"TV", 2);
		break;

	case eDxSVC_TYPE_RADIO:
		HxSTD_StrNCpy(pszTypeName,"RADIO", 5);
		break;

	case eDxSVC_TYPE_DATA:
		HxSTD_StrNCpy(pszTypeName,"DATA", 4);
		break;

	case eDxSVC_TYPE_SUBFEED:
		HxSTD_StrNCpy(pszTypeName,"SUBFEED", 7);
		break;

	case eDxSVC_TYPE_REPLACEMENT:
		HxSTD_StrNCpy(pszTypeName,"REPLACEMENT", 11);
		break;

	case eDxSVC_TYPE_HIDDEN:
		HxSTD_StrNCpy(pszTypeName,"HIDDEN", 6);
		break;

	case eDxSVC_TYPE_MHPTV:
		HxSTD_StrNCpy(pszTypeName,"MHPTV", 5);
		break;

	case eDxSVC_TYPE_ENGINEERING:
		HxSTD_StrNCpy(pszTypeName,"ENGINEERING", 11);
		break;

	case eDxSVC_TYPE_RFVOD:
		HxSTD_StrNCpy(pszTypeName,"RFVOD", 5);
		break;

	default :
		break;
	}

}

static HINT32	apk_meta_svc_command_show_list(void *pvArg)
{
	HxVector_t		*pstList;
	apkMetaSvc_Ctx_t *pstMgr = apk_meta_svc_GetContext();
	apkMetaSvc_Type_t *pstItem;
	HUINT32			i;
	HCHAR	szTypeName[16];
	HUINT32	 aulNumSvcType[9];

	HxSTD_MemSet(&aulNumSvcType, 0, sizeof(HUINT32) * 9);

	HxLOG_Trace();

	ENTER_CRITICAL_APKMETASVC;

	HLIB_CMD_Printf("\n========= Total Count info  =============\n");
	for (i=0 ; i< apkMetaSvc_MAX_TABLE_NUM ; i++)
	{
		pstList  = (HxVector_t *)HLIB_TREE_Lookup(pstMgr->pstListTree, cs_astDxInfo[i].pszName);
		if (pstList)
		{
			HLIB_CMD_Printf("%s : %d\n",cs_astDxInfo[i].pszName, HLIB_VECTOR_Length(pstList));
		}
	}

	HLIB_CMD_Printf("================ Service ===============\n");

	pstList  = (HxVector_t *)HLIB_TREE_Lookup(pstMgr->pstListTree, DxNAMEOF(DxService_t));
	if (pstList)
	{
		HxVECTOR_EACH(pstList, apkMetaSvc_Type_t *, pstItem, {
			switch(pstItem->stService.svcType)
			{
				case eDxSVC_TYPE_TV:				aulNumSvcType[0]++;			break;
				case eDxSVC_TYPE_RADIO:			aulNumSvcType[1]++;			break;
				case eDxSVC_TYPE_DATA:			aulNumSvcType[2]++;			break;
				case eDxSVC_TYPE_SUBFEED:			aulNumSvcType[3]++;			break;
				case eDxSVC_TYPE_REPLACEMENT:		aulNumSvcType[4]++;		break;
				case eDxSVC_TYPE_HIDDEN:			aulNumSvcType[5]++;			break;
				case eDxSVC_TYPE_MHPTV:			aulNumSvcType[6]++;			break;
				case eDxSVC_TYPE_ENGINEERING:			aulNumSvcType[7]++;		break;
				case eDxSVC_TYPE_RFVOD:			aulNumSvcType[8]++;			break;
				default :
					break;
			}
		});
		if (aulNumSvcType[0] > 0 || aulNumSvcType[1] > 0)
		{
			HLIB_CMD_Printf("  TV : %d\t RADIO : %d\n", aulNumSvcType[0], aulNumSvcType[1]);
		}
		if (aulNumSvcType[2] > 0 || aulNumSvcType[3] > 0)
		{
			HLIB_CMD_Printf("  DATA : %d\t SUBFEED : %d\n", aulNumSvcType[2], aulNumSvcType[3]);
		}
		if (aulNumSvcType[4] > 0 || aulNumSvcType[5] > 0)
		{
			HLIB_CMD_Printf("  REPLACEMENT : %d\t HIDDEN : %d\n", aulNumSvcType[4], aulNumSvcType[5]);
		}
		if (aulNumSvcType[6] > 0 || aulNumSvcType[7] > 0)
		{
			HLIB_CMD_Printf("  MHPTV : %d\t ENGINEERING : %d\n", aulNumSvcType[6], aulNumSvcType[7]);
		}
		if (aulNumSvcType[8] > 0)
		{
			HLIB_CMD_Printf("  RFVOD : %d\n", aulNumSvcType[8]);
		}
		HLIB_CMD_Printf("\n");
	}

	pstList  = (HxVector_t *)HLIB_TREE_Lookup(pstMgr->pstListTree, DxNAMEOF(DxService_t));
	if (pstList)
	{
		HxVECTOR_EACH(pstList, apkMetaSvc_Type_t *, pstItem, {
			HxSTD_MemSet(&szTypeName, 0, 16);
			apk_meta_svc_GetSvcType(pstItem->stService.svcType, szTypeName);
			HLIB_CMD_Printf("= [UID:%d][%-5s, %-10s\t: SVC_ID:0x%04X, TS_ID:0x%04x, ON_ID:0x%04x, LCN: %d, updateSvcFlag : %d, visibleFlag : %d] %s\n",
			pstItem->common.uid, szTypeName, pstItem->stService.name, pstItem->stService.svcid, pstItem->stService.tsid, pstItem->stService.onid, pstItem->stService.lcn,
			pstItem->stService.updateSvcFlag, pstItem->stService.visibleFlag, (pstItem->stService.removed)==TRUE?" --> removed":"");
		});
	}

	HLIB_CMD_Printf("\n========== Transponder =============\n");
	pstList  = (HxVector_t *)HLIB_TREE_Lookup(pstMgr->pstListTree, DxNAMEOF(DxTransponder_t));
	if (pstList)
	{
		HxVECTOR_EACH(pstList, apkMetaSvc_Type_t *, pstItem, {
			HLIB_CMD_Printf("= [UID:%d][TS_ID:0x%04x, ON_ID:0x%04x ]\n",
					pstItem->common.uid, pstItem->stTransponder.tsid, pstItem->stTransponder.onid);
		});
	}

	HLIB_CMD_Printf("\n========== Network =============\n");
	pstList  = (HxVector_t *)HLIB_TREE_Lookup(pstMgr->pstListTree, DxNAMEOF(DxNetwork_t));
	if (pstList)
	{
		HxVECTOR_EACH(pstList, apkMetaSvc_Type_t *, pstItem, {
			HLIB_CMD_Printf("= [UID:%d][NAME : %s ]\n",
					pstItem->common.uid, pstItem->stNetwork.name);
		});
	}

	HLIB_CMD_Printf("\n========== Provider =============\n");
	pstList  = (HxVector_t *)HLIB_TREE_Lookup(pstMgr->pstListTree, DxNAMEOF(DxProvider_t));
	if (pstList)
	{
		HxVECTOR_EACH(pstList, apkMetaSvc_Type_t *, pstItem, {
			HLIB_CMD_Printf("= [UID:%d][NAME : %s ]\n",
					pstItem->common.uid, pstItem->stProvider.name);
		});
	}

	HLIB_CMD_Printf("\n========== Group =============\n");
	pstList  = (HxVector_t *)HLIB_TREE_Lookup(pstMgr->pstListTree, DxNAMEOF(DxGroup_t));
	if (pstList)
	{
		HxVECTOR_EACH(pstList, apkMetaSvc_Type_t *, pstItem, {
			HLIB_CMD_Printf("= [UID:%d][NAME : %s ]\n",
					pstItem->common.uid, pstItem->stGroup.name);
		});
	}

	HLIB_CMD_Printf("\n========== Bouquet =============\n");
	pstList  = (HxVector_t *)HLIB_TREE_Lookup(pstMgr->pstListTree, DxNAMEOF(DxBouquet_t));
	if (pstList)
	{
		HxVECTOR_EACH(pstList, apkMetaSvc_Type_t *, pstItem, {
			HLIB_CMD_Printf("= [UID:%d][NAME : %s ]\n",
					pstItem->common.uid, pstItem->stBouquet.name);
		});
	}

	LEAVE_CRITICAL_APKMETASVC;

	return 0;
}

static HINT32	apk_meta_svc_command_delete_service(void *pvArg)
{
	HCHAR	*aucArg=NULL;
	HUINT32	uid;
	DxService_t 			*pstSvcInfo = NULL;
	HERROR	hErr = ERR_FAIL;

	/* 1st argument */
	aucArg = (HCHAR *)HLIB_CMD_GetNextArg((HCHAR**)&pvArg);
	if( aucArg == NULL )
		return HxCMD_ERR;
	if( sscanf(aucArg, "%d", &uid) == 0 )
		return HxCMD_ERR;

	pstSvcInfo = APK_META_SVC_GetService(uid);
	if(pstSvcInfo == NULL)
	{
		HxLOG_Error("Service not found:\n");
		return HxCMD_ERR;
	}

	hErr = APK_META_SVC_RemoveService(pstSvcInfo);
	if(hErr == ERR_FAIL)
	{
		HxLOG_Error("APK_META_SVC_RemoveService() fail! \n");
		return HxCMD_ERR;
	}

	return HxCMD_OK;

}

#if defined(APKMETASVC_USE_VCLONE)

static HINT32	apk_meta_svc_command_show_vc_info(void *pvArg)
{
	HxVector_t		*pstList;
	HUINT32			i;
	apkMetaSvc_Ctx_t *pstMgr = apk_meta_svc_GetContext();
	apkMetaSvc_VClone_t * pstVItem;
	ENTER_CRITICAL_APKMETASVC;

	HLIB_CMD_Printf("\n========= Total Count info  =============\n");
	for (i=0 ; i< apkMetaSvc_MAX_TABLE_NUM ; i++)
	{
		pstList  = (HxVector_t *)HLIB_TREE_Lookup(pstMgr->pstListTree, cs_astDxInfo[i].pszName);
		if (pstList)
		{
			HLIB_CMD_Printf("%s : %d\n",cs_astDxInfo[i].pszName, HLIB_VECTOR_Length(pstList));
		}
	}

	HLIB_CMD_Printf("\n========= size info=============\n");
	for(i=0; i< apkMetaSvc_MAX_TABLE_NUM ; i++)
	{
		HLIB_CMD_Printf("%s : %d Byte\n", cs_astDxInfo[i].pszName, cs_astDxInfo[i].ulSize);
	}

	HLIB_CMD_Printf("\n========= VClone Ref cnt (Until 200)===========\n");
	pstList  = (HxVector_t *)HLIB_TREE_Lookup(pstMgr->pstVCloneTree, DxNAMEOF(DxService_t));
	if (pstList)
	{
		HLIB_CMD_Printf("Total cnt :%d \n", HLIB_VECTOR_Length(pstList));
		HxVECTOR_EACH(pstList, apkMetaSvc_VClone_t*, pstVItem, {
			HLIB_CMD_Printf("UID : %d, ref : %d\t", pstVItem->pstItem->common.uid, pstVItem->nRefCnt );
			if (__i>0 && __i %8 == 0) HLIB_CMD_Printf("\n");
			if (__i>0 && __i %200 == 0) break;
		});
	}

	LEAVE_CRITICAL_APKMETASVC;

	return 0;
}
#endif
static void	apk_meta_svc_InitCommand(void)
{
#define	hCmdHandle		"apksvc"
	HLIB_CMD_RegisterWord (hCmdHandle, apk_meta_svc_command_load_from_dama, "loadsvclist", "load data from dama", "loadsvclist");
	HLIB_CMD_RegisterWord (hCmdHandle, apk_meta_svc_command_save_to_dama,   "savesvclist", "save data to dama",   "savesvclist");
	HLIB_CMD_RegisterWord (hCmdHandle, apk_meta_svc_command_show_list,   "showsvclist", "show svc list",   "showsvclist");
#if defined(APKMETASVC_USE_VCLONE)
	HLIB_CMD_RegisterWord (hCmdHandle, apk_meta_svc_command_show_vc_info,   "showvinfo", "show virtual clone info",   "showvinfo");
#endif
	HLIB_CMD_RegisterWord (hCmdHandle, apk_meta_svc_command_delete_service,	 "del", "delete svcuid",   "delete svcuid");
}

void	APK_META_SVC_InitCommand(void)
{
	apk_meta_svc_InitCommand();
}
#endif

/* end of file */
