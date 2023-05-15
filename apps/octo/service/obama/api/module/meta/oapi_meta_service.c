/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_meta_service.c
	@brief

	Description:  									\n
	Module: OAPI meta service 			\n

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
#include	<hlib.h>
#include	<dlib.h>
#include	<oapi.h>
#include 	<oapi_meta_service.h>

#include "../../oapi_mem.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define OxMETASVC_LOAD_TABLE_NUM		6		// DxService_t, DxTransponder_t, DxNetwork_t, DxProvider_t, DxGroup_t, DxBouquet_t

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	OxMetaSvc_Notifier_t		pfNotifier;
	void						*pvUserdata;
	HUINT32					ulSema;

}OxMetaSvcMgr_t;

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
} oapi_meta_svc_Type_t;

typedef struct
{
	OxMetaSvc_LoadResCb_t OnResponse;
	HBOOL 	bFlag;			// Appkit이 알아야할 정보로, 변경된 list가 있던없던 OPL로 올려주어야 하는지 판단하는 flag, 예를 들어 Search시에는 무조건 true
}OxMetaSvc_LoadInfo_t;



/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/



#define ________META_SERVICE_____________


static OxMetaSvcMgr_t*	oapi_meta_svc_GetManager (void)
{
	static OxMetaSvcMgr_t 	*s_pstMgr;

	HxLOG_Trace();
	if (s_pstMgr == NULL)
	{
		s_pstMgr = (OxMetaSvcMgr_t *)OxMEM_Calloc(sizeof(OxMetaSvcMgr_t));
		HxLOG_Assert(s_pstMgr);
		HxSEMT_Create(&s_pstMgr->ulSema, "OxMetaSvcMgr_t", 0);
	}
	HxSEMT_Get(s_pstMgr->ulSema);

	return s_pstMgr;
}

static void		oapi_meta_svc_ReleaseManager (OxMetaSvcMgr_t *pstMgr)
{
	HxLOG_Trace();
	HxSEMT_Release(pstMgr->ulSema);
}
static void		oapi_meta_svc_OnUpdate (HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserdata)
{
	OxMetaSvcMgr_t *pstMgr;

	HxLOG_Trace();

	pstMgr = oapi_meta_svc_GetManager();

	if (pstMgr->pfNotifier)
	{
		pstMgr->pfNotifier();
	}
	oapi_meta_svc_ReleaseManager(pstMgr);
}


static HINT32	oapi_meta_svc_CompareVectorObject (const void *pvArg1, const void *pvArg2)
{
	const void *pvObj1 = *(const void **)pvArg1;
	const void *pvObj2 = *(const void **)pvArg2;

	HxLOG_Trace();
	return (HINT32)memcmp(pvObj1, pvObj2, sizeof(HINT32)/* uid field */);
}

static void		oapi_meta_svc_DeleteObject (void *pvSelf)
{
	HxLOG_Trace();
	OxMEM_Free(pvSelf);
}

static void		oapi_meta_svc_unpackList(HxObject_t *pstIn, HUINT32 *ulCnt, HxTREE_t **pstOut)
{
	void			*pvBin, *pvNew;

	HUINT32 	ulSize_bin;
	HUINT32 	i, j, ulLen;
	HUINT32 	ulIdx = 0;
	HxTREE_t		*pstTree;
	//METASVC_Type_t	*item;

	const HCHAR *paszName[] = {DxNAMEOF(DxService_t),
						DxNAMEOF(DxTransponder_t),
						DxNAMEOF(DxProvider_t),
						DxNAMEOF(DxNetwork_t),
						DxNAMEOF(DxGroup_t),
						DxNAMEOF(DxBouquet_t)
						};
	HxVector_t		*pstList;


	// DxXXX_t로 나온 데이터를 Vector에 넣고, 다시 Tree에 넣어준다.

	HxLOG_Trace();

	pstTree = HLIB_TREE_NewFull(
				  (HxCompareDataFunc)HLIB_STD_StrCaseCmp
				, NULL
				, NULL
				, (HxDestroyNotify)HLIB_VECTOR_Delete
			);
	if(pstTree == NULL)
	{
		return;
	}

	ulLen = HxOBJECT_ARR_LEN(pstIn);

	HxLOG_Print("[%s] [%d]Len : %d\n",__FUNCTION__,__LINE__, ulLen);

	for (i = 0, ulIdx = 0 ; i < OxMETASVC_LOAD_TABLE_NUM ; i++)
	{
		HxLOG_Print("[%s] [%d]==========ulCnt[%d] : %d===============\n",__FUNCTION__,__LINE__,i, ulCnt[i]);

		pstList = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))oapi_meta_svc_DeleteObject, oapi_meta_svc_CompareVectorObject);
		if(pstList == NULL)
		{
			HxLOG_Error("HxVector_NewEx() Fail!!\n");

			continue;
		}

		for(j=0 ; j<ulCnt[i] ; j++)
		{
			pvBin = HxOBJECT_BIN_DATA(HxOBJECT_ARR_VAL(pstIn, ulIdx));
			ulSize_bin = HxOBJECT_BIN_SIZE(HxOBJECT_ARR_VAL(pstIn, ulIdx));

#if defined(CONFIG_DEBUG)
			if (i == 0)
			{
				DxService_t * svcinfo = (DxService_t *)pvBin;
				HxLOG_Print("[%s] [%d]uid:%d, name : %s, svcid : %d\n",__FUNCTION__,__LINE__, svcinfo->uid, svcinfo->name, svcinfo->svcid);
			}
			else if (i == 1)
			{
				DxTransponder_t * tsinfo = (DxTransponder_t *)pvBin;
				HxLOG_Print("[%s] [%d]uid:%d, tsid : %d, netuid : %d\n",__FUNCTION__,__LINE__, tsinfo->uid, tsinfo->tsid, tsinfo->netuid);
			}
			else if (i== 2)
			{
				DxProvider_t *prvInfo = (DxProvider_t *)pvBin;
				HxLOG_Print("[%s] [%d]uid : %d, name : %s\n",__FUNCTION__,__LINE__, prvInfo->uid, prvInfo->name);
			}

#endif
			((oapi_meta_svc_Type_t *)pvBin)->common.typeOf = paszName[i];

			pvNew = OxMEM_MemDup(pvBin, ulSize_bin);

			HLIB_VECTOR_Add(pstList, pvNew);

			ulIdx++;
		}

		if (HLIB_VECTOR_Length(pstList) == 0)
		{
			HLIB_VECTOR_Delete(pstList);
			continue;
		}
		HLIB_VECTOR_Sort(pstList, NULL);

		HxLOG_Print("[%s] [%d]insert tree(name : %s)\n",__FUNCTION__,__LINE__, paszName[i]);

		// Add tree
		HLIB_TREE_Insert(pstTree, (void *)paszName[i], pstList);

	}

	*pstOut = pstTree;
}

static void		oapi_meta_svc_OnResponse (HERROR hErr, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserdata)
{
	OxMetaSvc_LoadResCb_t pfCb = NULL;
	HxTREE_t *pstUpdated = NULL, *pstRemoved = NULL;

	HxObject_t	*pstUpdated_obj = NULL, *pstRemoved_obj = NULL;

	HUINT32 	*pulUpdated_cnt, *pulRemoved_cnt;
	HBOOL	bUpdated, bRemoved;

	OxMetaSvc_LoadInfo_t	*pstLoadInfo = (OxMetaSvc_LoadInfo_t *)pvUserdata;

	pfCb = pstLoadInfo->OnResponse;

	HxLOG_Trace();
	HxLOG_Print("[%s] [%d]OAPI_META_SERVICE RECEIVE\n",__FUNCTION__,__LINE__);

	if(argc == 0 )
	{
		HxLOG_Print("[%s] [%d]No data \n",__FUNCTION__,__LINE__);
		return;
	}

	bUpdated = HxOBJECT_INT(argv[4]);
	bRemoved = HxOBJECT_INT(argv[5]);

	HxLOG_Print(" [%s] [%d]bUpdated : %d, bRemoved : %d\n",__FUNCTION__,__LINE__, bUpdated, bRemoved);


	if(pfCb)
	{
		if (bUpdated)
		{
			pstUpdated_obj = HLIB_SERIALIZER_Unpack(HxOBJECT_BIN_DATA(argv[0]), HxOBJECT_BIN_SIZE(argv[0]));
			pulUpdated_cnt = (HUINT32 *)HxOBJECT_BIN_DATA(argv[1]);

			if (pstUpdated_obj != NULL)
			{
				oapi_meta_svc_unpackList(pstUpdated_obj, pulUpdated_cnt, &pstUpdated);
			}
		}

		if (bRemoved)
		{
			pstRemoved_obj = HLIB_SERIALIZER_Unpack(HxOBJECT_BIN_DATA(argv[2]), HxOBJECT_BIN_SIZE(argv[2]));
			pulRemoved_cnt = (HUINT32 *)HxOBJECT_BIN_DATA(argv[3]);

			if (pstRemoved_obj != NULL)
			{
				oapi_meta_svc_unpackList(pstRemoved_obj, pulRemoved_cnt, &pstRemoved);
			}
		}

		if (pstUpdated_obj) 	{	HLIB_SERIALIZER_FreeObject(pstUpdated_obj); }
		if (pstRemoved_obj)	{	HLIB_SERIALIZER_FreeObject(pstRemoved_obj); }

		pfCb(pstUpdated, pstRemoved, pstLoadInfo->bFlag);
	}

	HxLOG_Debug("%s(argc:%d, pvUserdata:0x%X)\n", __FUNCTION__, argc, pvUserdata);
	OxMEM_Free(pstLoadInfo);

}

#define ____________NEW________________________________________
void OAPI_META_SVC_SetListener(OxMetaSvc_Notifier_t pfNotifier, void *pvUserdata)
{
	OxMetaSvcMgr_t *pstMgr;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	HxLOG_Trace();

	pstMgr = oapi_meta_svc_GetManager();
	if (pfNotifier)
	{
		if (pstMgr->pfNotifier == NULL)
		{
			pstMgr->pfNotifier = pfNotifier;
			pstMgr->pvUserdata = pvUserdata;

			HLIB_RPC_RegisterNotifyCallback (nRpcHandle, RPC_OBAMA_META_SERVICE_OnUpdate, "s", NULL, (HxRPC_Noti_t)oapi_meta_svc_OnUpdate);
		}
	}
	else
	{
		if (pstMgr->pfNotifier)
		{
			pstMgr->pfNotifier = NULL;
			pstMgr->pvUserdata = NULL;

			// TODO:
			//OAPI_UnregisterCallback(RPC_OBAMA_META_SERVICE_OnUpdate, oapi_meta_svc_OnUpdate);
		}
	}
	oapi_meta_svc_ReleaseManager(pstMgr);
}

void OAPI_META_SVC_SetDynamicUpdate(HBOOL bEnable)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	HxLOG_Trace();
	HxLOG_Debug("%s(bEnable:%d)\n", __FUNCTION__, bEnable);

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_META_SERVICE_SetDynamicUpdate, "i", bEnable);
}

void OAPI_META_SVC_LoadPreprogrammedList(void)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	HxLOG_Trace();

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_META_SERVICE_LoadPreprogrammedList, NULL);
}

void OAPI_META_SVC_Commit(void)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	HxLOG_Trace();
	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_META_SERVICE_Commit, NULL);
}

void OAPI_META_SVC_LoadUpdatedList(OxMetaSvc_LoadResCb_t OnResponse, HBOOL bFlag)
{
	OxMetaSvc_LoadInfo_t *pstInfo = NULL;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	pstInfo = (OxMetaSvc_LoadInfo_t *)OxMEM_Malloc(sizeof(OxMetaSvc_LoadInfo_t));

	HxLOG_Trace();
	if(pstInfo)
	{
		pstInfo->OnResponse = OnResponse;
		pstInfo->bFlag = bFlag;

		HLIB_RPC_Request (nRpcHandle, (HxRPC_Response_t)oapi_meta_svc_OnResponse, (void *)pstInfo, RPC_OAPI_META_SERVICE_LoadUpdatedList, NULL);
	}
	else
	{
		HxLOG_Critical("MEM alloc fail\n");
	}
}

void OAPI_META_SVC_Reset(HCHAR *pszName)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	HxLOG_Debug("%s(pszName:%s)\n", __FUNCTION__, pszName);

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_META_SERVICE_Reset, "s", pszName);
}

