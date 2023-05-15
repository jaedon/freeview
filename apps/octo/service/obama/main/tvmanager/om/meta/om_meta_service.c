 /*******************************************************************
	 File Description
 ********************************************************************/
 /**
	 @file	   om_meta_service.c
	 @brief	This module for meta service list

	 Description:									 \n
	 Module: APPKITv2			 \n

	 Copyright (c) 2013 HUMAX Co., Ltd. 			 \n
	 All rights reserved.							 \n
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

 #include	<hlib.h>
#include	<dlib.h>
#include	<obama_rpcprotocol.h>
#include	<otl.h>
#include	<oapi.h>

#include	<octo_common.h>
#include 	<db_param.h>
#include	<bus.h>
#include	<svc_si.h>
#include	<mgr_common.h>
#include	<mgr_action.h>
#include 	<mgr_pg.h>
#include	<oapi_meta_service.h>

#include	<om_common.h>

#define	RETURN_IF_VOID(expr)	do{if(expr){HxLOG_Debug("%s failed!!\n", #expr);return;}}while(0)
#define	RETURN_IF(expr, err)	do{if(expr){HxLOG_Debug("%s failed!!\n", #expr);return (err);}}while(0)
#define	GOTO_IF(expr, pos)		do{if(expr){HxLOG_Debug("%s failed!!\n", #expr);goto pos;}}while(0)


#define METASVC_LOAD_TABLE_NUM		6

typedef struct
{
	HINT32	sd;
	HINT32	srzr;
} MgrMetaSvcContext_t;

static MgrMetaSvcContext_t	s_context;
static BUS_Result_t	Proc_om_meta_svc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

// Dynamic update 여부 setting
static HERROR	om_meta_svc_SetDymanicUpdate (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 requestId,  void *userData)
{
	HBOOL bEnable = FALSE;

	bEnable = (HBOOL)HxOBJECT_INT(argv[0]);

	if (bEnable)
	{
		HxLOG_Print("[%s] [%d]Allow dynamic update.\n",__FUNCTION__,__LINE__);
	}
	else
	{
		HxLOG_Print("[%s] [%d]Block dynamic update.\n",__FUNCTION__,__LINE__);
	}

	return ERR_OK;
}


static HERROR	om_meta_svc_LoadPreprogrammedList (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 requestId,  void *userData)
{
	HERROR			hErr;
	HxCountry_e		eCountryId;

	HxLOG_Trace();

//	eCountryId = MWC_PARAM_GetCountryId();
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID, (HUINT32 *)&eCountryId, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID) Error !!\n");
	}
	HxLOG_Info("Success to Read Region DB eCountryIdx(%d)\n", eCountryId);

	DB_SVC_LoadPreProgrammedChannel(eCountryId);

	return ERR_OK;
}

 static HERROR	 om_meta_svc_Commit (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 requestId,  void *userData)
 {
	 HxLOG_Trace();

	DB_SVC_Commit();
	DB_SVC_RemoveSearchFlag();

	/*
		To resolve ir4000hd CMP issue,
		this code is moved into search action proc
		- when: 20150126
		- redmine: http://svn.humaxdigital.com:3000/issues/100008
	*/
	// Dama listener restart
	//HxLOG_Print(HxANSI_COLOR_YELLOW("call DB_SVC_SetNotifier(TRUE)\n"));
	//DB_SVC_SetNotifier(TRUE);

	 return ERR_OK;
 }


 static void*	om_meta_svc_pack_list(_datalist_t *datalist, HUINT32 *countlist, HUINT32 *outsize, HBOOL *pbExist)
{
	HBOOL		bData = FALSE;
	HINT32		ulSrzr, ulStream;
	HUINT32		size;
	void		*data, *conv, *outbuf = NULL;
	HUINT32		i,j, bufsize;
	const HUINT32		tablesize[2][METASVC_LOAD_TABLE_NUM] = {
							{	sizeof(DbSvc_Info_t),
								sizeof(DbSvc_TsInfo_t),
								sizeof(DbSvc_PrvInfo_t),
								sizeof(DbSvc_NetInfo_t),
								sizeof(DbSvc_GroupInfo_t),
								sizeof(DbSvc_BouqInfo_t)},

							{	sizeof(DxService_t),
								sizeof(DxTransponder_t),
								sizeof(DxProvider_t),
								sizeof(DxNetwork_t),
								sizeof(DxGroup_t),
								sizeof(DxBouquet_t)}
													};

	HUINT8 	*item;

	DxService_t		dxSvcInfo;
	DxTransponder_t	dxTsInfo;
	DxProvider_t		dxPrvInfo;
	DxNetwork_t		dxNetInfo;
	DxGroup_t		dxGroupInfo;
	DxBouquet_t		dxBouqInfo;

	if (datalist == NULL || countlist == NULL || outsize == NULL)
	{
		return NULL;
	}


	ulSrzr = HLIB_SERIALIZER_Open();
	ulStream   = HLIB_RWSTREAM_Open(NULL, 0);

	HxSERIALIZER_Array(ulSrzr);

	for (i = 0 ; i < METASVC_LOAD_TABLE_NUM ; i++)
	{
		item = (HUINT8 *)datalist[i].list;
		HxLOG_Print("[%s] [%d]=====[%d] count : %d, size : %d , convsize : %d===== \n",__FUNCTION__,__LINE__, i, datalist[i].count, tablesize[0][i], tablesize[1][i]);

		for(j = 0 ; j < datalist[i].count ; j++, item += tablesize[0][i])
		{
			conv = NULL;

			switch(i)
			{
				case eSvcTable_Service:

					HxLOG_Info("DbSvc_Info_t usSvcIdx:%d, name : %s, usSvcId : %d\n",  DbSvc_GetSvcIdx((const DbSvc_Info_t *)item) , DbSvc_GetName((const DbSvc_Info_t *)item), DbSvc_GetSvcId((const DbSvc_Info_t *)item));
					conv = (void *)OTL_CONV_SVC_Info2DxService(&dxSvcInfo, (const DbSvc_Info_t *)item);
					HxLOG_Print("[%s] [%d]DxService_t usSvcIdx:%d, name : %s, usSvcId : %d\n",__FUNCTION__,__LINE__, ((DxService_t *)conv)->uid,((DxService_t *)conv)->name, ((DxService_t *)conv)->svcid);
					break;

				case eSvcTable_Transponder:
					HxLOG_Print("[%s] [%d]DbSvc_TsInfo_t usTsIdx:%d, usUniqueId : %d, usNetIdx : %d, usTsId: %d, usOnId: %d, ucTunerId: %d eDeliType: %d\n",__FUNCTION__,__LINE__, ((DbSvc_TsInfo_t *)item)->usTsIdx, ((DbSvc_TsInfo_t *)item)->usUniqueId, ((DbSvc_TsInfo_t *)item)->usNetIdx, ((DbSvc_TsInfo_t *)item)->usTsId, ((DbSvc_TsInfo_t *)item)->usOnId, ((DbSvc_TsInfo_t *)item)->ucTunerId, ((DbSvc_TsInfo_t *)item)->eDeliType);
					conv = (void *)OTL_CONV_SVC_TsInfo2DxTransponder(&dxTsInfo, (const DbSvc_TsInfo_t *)item);
					HxLOG_Print("[%s] [%d]DxTransponder_t uid:%d, tsid : %d, netuid : %d\n",__FUNCTION__,__LINE__, ((DxTransponder_t *)conv)->uid, ((DxTransponder_t *)conv)->tsid, ((DxTransponder_t *)conv)->netuid);
					break;

				case eSvcTable_Provider:
					conv = (void *)OTL_CONV_SVC_PrvInfo2DxProvider(&dxPrvInfo, (const DbSvc_PrvInfo_t *)item);
					HxLOG_Print("[%s] [%d]uid:%d, name : %s\n",__FUNCTION__,__LINE__, ((DxProvider_t *)conv)->uid, ((DxProvider_t *)conv)->name);
					break;

				case eSvcTable_Network:
					conv = (void *)OTL_CONV_SVC_NetInfo2DxNetwork(&dxNetInfo, (const DbSvc_NetInfo_t *)item);
					HxLOG_Print("[%s] [%d]uid:%d, name : %s\n",__FUNCTION__,__LINE__, ((DxNetwork_t *)conv)->uid, ((DxNetwork_t *)conv)->name);
					break;

				case eSvcTable_Group:
					conv = (void *)OTL_CONV_SVC_GroupInfo2DxGroup(&dxGroupInfo, (const DbSvc_GroupInfo_t *)item);
					HxLOG_Print("[%s] [%d]uid:%d, name : %s\n",__FUNCTION__,__LINE__, ((DxGroup_t *)conv)->uid, ((DxGroup_t *)conv)->name);
					break;

				case eSvcTable_Bouquet:
					conv = (void *)OTL_CONV_SVC_BouqInfo2DxBouquet(&dxBouqInfo, (const DbSvc_BouqInfo_t *)item);
					HxLOG_Print("[%s] [%d]uid:%d, name : %s\n",__FUNCTION__,__LINE__, ((DxBouquet_t *)conv)->uid, ((DxBouquet_t *)conv)->name);
					break;

				default:
					break;
			}

			HLIB_RWSTREAM_Rewind(ulStream);

			HLIB_RWSTREAM_Write(ulStream, conv, tablesize[1][i]);
			size = HLIB_RWSTREAM_GetOffset(ulStream);
			data = HLIB_RWSTREAM_GetBuf(ulStream);
			HxSERIALIZER_Binary(ulSrzr, data, size);

			if (bData == FALSE)
			{
				bData = TRUE;
			}
		}
		countlist[i] = datalist[i].count;
		//OxDB_Free(datalist[i].list);
	}

	HxSERIALIZER_ArrayEnd(ulSrzr);
	data = HLIB_SERIALIZER_Pack(ulSrzr, &bufsize);

	outbuf = HLIB_STD_MemDup(data, bufsize);

	if (pbExist)
	{
		*pbExist = bData;
	}

	HLIB_RWSTREAM_Close(ulStream);

	HLIB_SERIALIZER_Clear(ulSrzr);
	HLIB_SERIALIZER_Close(ulSrzr);

	if(outsize)
	{
		*outsize = bufsize;
	}

	return outbuf;

}

static MgrMetaSvcContext_t * om_meta_svc_GetContext(void)
{
	return &s_context;
}

static HERROR	om_meta_svc_LoadUpdatedList (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 requestId,  void *userData)
{

	HUINT32		aulUpdatedCnt[METASVC_LOAD_TABLE_NUM], aulRemovedCnt[METASVC_LOAD_TABLE_NUM];
	_datalist_t 	astUpdated[eSvcTable_MAX];
	_datalist_t 	astRemoved[eSvcTable_MAX];
	HUINT32		i;
	HUINT32		ulUpdatedSize= 0, ulRemovedSize = 0;
	HBOOL		bUpdated = FALSE, bRemoved = FALSE;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;
	void		*pvUpdatedBuf = NULL, *pvRemovedBuf = NULL;
	MgrMetaSvcContext_t *ctx;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle failed. \n");
	}

	ctx = om_meta_svc_GetContext();

	HxLOG_Trace();

	HxSTD_memset(astUpdated, 0, sizeof(_datalist_t) * eSvcTable_MAX);
	HxSTD_memset(astRemoved, 0, sizeof(_datalist_t) * eSvcTable_MAX);

	for (i = 0 ; i < METASVC_LOAD_TABLE_NUM ; i++)
	{
		(void) DB_SVC_GetChanged(i, &astUpdated[i].list, &astUpdated[i].count, &astRemoved[i].list, &astRemoved[i].count);
	}

	//Pack updated & removed list
	pvUpdatedBuf = om_meta_svc_pack_list((_datalist_t *)&astUpdated, aulUpdatedCnt, &ulUpdatedSize, &bUpdated);
	pvRemovedBuf = om_meta_svc_pack_list((_datalist_t *)&astRemoved, aulRemovedCnt, &ulRemovedSize, &bRemoved);

		// Response
	HxLOG_Print("%s] [%d]bUpdated : %d, bRemoved : %d\n",__FUNCTION__,__LINE__, bUpdated, bRemoved);
	HLIB_RPC_Response(nRpcHandle, nClientHandle, requestId, ERR_OK, "bbbbii", pvUpdatedBuf, ulUpdatedSize,					// updated list data
												aulUpdatedCnt, sizeof(HUINT32)*METASVC_LOAD_TABLE_NUM,	// updated list count array
												pvRemovedBuf, ulRemovedSize,					// removed list data
												aulRemovedCnt, sizeof(HUINT32)*METASVC_LOAD_TABLE_NUM,	//removed list count array
												bUpdated, bRemoved);

	for (i = 0 ; i < METASVC_LOAD_TABLE_NUM ; i++)
	{
		if (astUpdated[i].list)	{	OxDB_Free(astUpdated[i].list);	}
		if (astRemoved[i].list)	{	OxDB_Free(astRemoved[i].list);	}
	}

	HLIB_STD_MemFree(pvUpdatedBuf);
	HLIB_STD_MemFree(pvRemovedBuf);

	(void)ctx;

	return ERR_OK;

}


static void	om_meta_svc_EmitUpdated (void)
{

	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	HxLOG_Trace();

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr == ERR_OK)
	{
		HLIB_RPC_Notify(nRpcHandle, RPC_OBAMA_META_SERVICE_OnUpdate, "s","on_update");
	}
}

// Obama DB Reset 함수 추가.
 static HERROR	 om_meta_svc_Reset (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 requestId,  void *userData)
 {
	HCHAR	*pszName; 
	
	 pszName = (HCHAR *)HxOBJECT_STR(argv[0]);

	HxLOG_Debug("pszName (%s) \n", pszName);
		 
	if(HxSTD_StrCmp(pszName, DxNAMEOF(DxService_t)) == 0)
	{
		DB_SVC_DeleteAllServiceInfo();
	}
	else if(HxSTD_StrCmp(pszName, DxNAMEOF(DxTransponder_t))== 0)
	{
		DB_SVC_DeleteAllTsInfo();
	}
	else if(HxSTD_StrCmp(pszName, DxNAMEOF(DxProvider_t)) == 0)
	{
		DB_SVC_DeleteAllProviderInfo();
	}		
	else if(HxSTD_StrCmp(pszName, DxNAMEOF(DxNetwork_t)) == 0)
	{
		DB_SVC_DeleteAllNetworkInfo();
	}
	else if(HxSTD_StrCmp(pszName, DxNAMEOF(DxGroup_t)) == 0)
	{
		DB_SVC_DeleteAllProviderInfo();
	}
	else if(HxSTD_StrCmp(pszName, DxNAMEOF(DxBouquet_t)) == 0)
	{
		DB_SVC_DeleteAllBouquetInfo();
	}
	else
	{
		return ERR_FAIL;
	}
 
	 return ERR_OK;
 }

 static void	 om_meta_svc_InitRpcFunctions (void)
 {
 	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

//	MgrMetaSvcContext_t *ctx = om_meta_svc_GetContext();
	HxLOG_Trace();

	 // INIT context
//	 ctx->sd   = HLIB_RWSTREAM_Open(NULL, 0);
//	 ctx->srzr = HLIB_SERIALIZER_Open();

	 hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	 if(hErr == ERR_OK)
	 {
		 // REGISTER RPC Method & Callback
		 hErr = HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_META_SERVICE_SetDynamicUpdate, "i", (HxRPC_Func_t)om_meta_svc_SetDymanicUpdate, NULL
			 , "Set dynamic update : (HBOOL bEnable)"
		 );
		 if(hErr != ERR_OK)
		 {
			HxLOG_Error("HLIB_RPC_RegisterCall() fail! %s \n", RPC_OAPI_META_SERVICE_SetDynamicUpdate);
		 }

		 hErr = HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_META_SERVICE_LoadPreprogrammedList, NULL, (HxRPC_Func_t)om_meta_svc_LoadPreprogrammedList, NULL
			 , "Load preprogrammed list: (void)"
		 );
		 if(hErr != ERR_OK)
		 {
			HxLOG_Error("HLIB_RPC_RegisterCall() fail! %s \n", RPC_OAPI_META_SERVICE_LoadPreprogrammedList);
		 }

		 hErr = HLIB_RPC_RegisterRequest (nRpcHandle, RPC_OAPI_META_SERVICE_LoadUpdatedList, NULL, (HxRPC_Func_t)om_meta_svc_LoadUpdatedList, NULL
			 , "Load updated list: (void)"
		 );
		 if(hErr != ERR_OK)
		 {
			HxLOG_Error("HLIB_RPC_RegisterRequest() fail! %s \n", RPC_OAPI_META_SERVICE_LoadUpdatedList);
		 }

		 hErr = HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_META_SERVICE_Commit, NULL, (HxRPC_Func_t)om_meta_svc_Commit, NULL
			 , "Commit list: (void)"
		 );
		 if(hErr != ERR_OK)
		 {
			HxLOG_Error("HLIB_RPC_RegisterCall() fail! %s \n", RPC_OAPI_META_SERVICE_Commit);
		 }

		 hErr = HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_META_SERVICE_OnUpdate, "s",
			 "Trigger when the service list be updated!\n"
		 );
		 if(hErr != ERR_OK)
		 {
			HxLOG_Error("HLIB_RPC_CreateNotification() fail! %s \n", RPC_OBAMA_META_SERVICE_OnUpdate);
		 }
		 
		 hErr = HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_META_SERVICE_Reset, "s", (HxRPC_Func_t)om_meta_svc_Reset, NULL
			 , "reset Obama Db : ( HCHAR *pszName)"
		 );
		 if(hErr != ERR_OK)
		 {
			HxLOG_Error("HLIB_RPC_RegisterCall() fail! %s \n", RPC_OAPI_META_SERVICE_Reset);
		 }

	 }
	 else
	 {
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle() fail! \n");
	 }

 }

static BUS_Result_t	Proc_om_meta_svc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t eRet = MESSAGE_PASS;

#ifdef META_SVC_SAVE_FROM_OCTO
	MgrAction_State_e amState = MGR_ACTION_GetState();

	if(amState == eAmState_ACTIVE_STANDBY)
	{
//		om_meta_svc_ShutdownProc(msg, action, p1, p2, p3);
		return MESSAGE_PASS;
	}
#endif

	switch (message)
	{
		case eMEVT_BUS_CREATE:
			HxLOG_Debug("\t: eMEVT_BUS_CREATE\n");
			BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
			om_meta_svc_InitRpcFunctions();
			break;

		case eSEVT_DB_SVCLIST_CHECK_LOCK:
			HxLOG_Debug("\t: eSEVT_DB_SVCLIST_CHECK_LOCK\n");
			BUS_PostMessage(NULL, eOEVT_PG_CHECK_LOCK, (Handle_t)NULL, 0, 0, 0);
			break;

		case eSEVT_DB_CHECK_SVC_DB:
			HxLOG_Debug("\t: eSEVT_DB_CHECK_SVC_DB\n");
			(void)om_meta_svc_EmitUpdated();
			break;

		case eSEVT_DB_NOTIFY_UPDATED:
			HxLOG_Debug("\t: eSEVT_DB_NOTIFY_UPDATED (p1:0x%X, p2:0x%X, p3:0x%X)\n", p1, p2, p3);
			(void)om_meta_svc_EmitUpdated();
			break;

		default:
			break;
	}

	(void)p1;
	(void)p2;
	(void)p3;
	return eRet;
}

void	OM_META_SVC_Init (void)
{
	HxLOG_Trace();

	BUS_MGR_Create(NULL, APP_ACTION_PRIORITY, Proc_om_meta_svc, 0, 0, 0, 0);
}

