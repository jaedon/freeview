/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xom_cas_verimatrix.c
	@brief

	Description:  									\n
	Module: TVMANAGER / OAPI Manager				\n

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
#include <hlib.h>
#include <obama_rpcprotocol.h>
#include <oapi.h>

#include <octo_common.h>

#include <bus.h>

#include <mgr_common.h>
#include <mgr_cas.h>
#include "../local_include/_xom_cas.h"



/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

STATIC OxCasVerimatrixStatus_t			s_stXomCasVerimatrixStatus;

/*******************************************************************/
/********************      External Function    ********************/
/*******************************************************************/
extern HCHAR* xmgr_cas_VmxUtil_GetIptvHybridVersionString(void);
extern HCHAR* xmgr_cas_VmxUtil_GetWebVersionString(void);
extern HCHAR* xmgr_cas_VmxUtil_GetGatewayVersionString(void);

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
STATIC void 	xom_cas_verimatrix_CheckStatusChange_BASE(void);
STATIC HERROR 	xom_cas_verimatrix_InitRpc_BASE(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData);
STATIC HBOOL 	xom_cas_verimatrix_RefreshData_BASE(void);


#define ___LOCAL_FUNCTIONS___

#define _____OAPI_CALLBACK_FUNCTIONS_____
STATIC void xom_cas_verimatrix_CheckStatusChange_BASE(void)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;
	HBOOL					bChanged = FALSE;

	bChanged = xom_cas_verimatrix_RefreshData_BASE();
	HxLOG_Info("bChanged: %d \n", bChanged);
	if(bChanged == TRUE)
	{
		hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);

		if (hErr == ERR_OK)
		{
			HLIB_RPC_Notify(nRpcHandle, RPC_OBAMA_CAS_VERIMATRIX_onStatusChanged, "b", &s_stXomCasVerimatrixStatus, sizeof(OxCasVerimatrixStatus_t));
		}
	}

}

#define _____OAPI_COMMAND_FUNCTIONS_____
STATIC HERROR xom_cas_verimatrix_InitRpc_BASE(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle() failed..! \n");
	}

	HxLOG_Debug("\t%s(reqID:0x%x)\n", __FUNCTION__, nRequestId);
	HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_OK, "b", &s_stXomCasVerimatrixStatus, sizeof(OxCasVerimatrixStatus_t));

	return ERR_OK;
}

//extern HERROR	xsvc_cas_IrGetUiData(HUINT8 *pucUiData, DxCAS_UiDataType_e eType);

STATIC HBOOL xom_cas_verimatrix_RefreshData_BASE(void)
{
	HBOOL			bChanged = FALSE;
	HCHAR			*pszVersion = NULL;

	HxLOG_Trace();

	// structure안에서 동적할당을 했다면 별도로 clear해주는 함수를 만들어 호출해 주어야 함
	HxSTD_MemSet(&s_stXomCasVerimatrixStatus, 0x00, sizeof(s_stXomCasVerimatrixStatus));

	// ViewRight Iptv hybrid
	pszVersion = (HCHAR *)xmgr_cas_VmxUtil_GetIptvHybridVersionString();
	if (pszVersion != NULL)
	{
		snprintf(s_stXomCasVerimatrixStatus.szIptvHybridVersion, sizeof(s_stXomCasVerimatrixStatus.szIptvHybridVersion), "%s", pszVersion);
		HLIB_STD_MemFree(pszVersion);
		pszVersion = NULL;
	}

	// ViewRight WEB
	pszVersion = (HCHAR *)xmgr_cas_VmxUtil_GetWebVersionString();
	if (pszVersion != NULL)
	{
		snprintf(s_stXomCasVerimatrixStatus.szWebVersion, sizeof(s_stXomCasVerimatrixStatus.szWebVersion), "%s", pszVersion);
		HLIB_STD_MemFree(pszVersion);
		pszVersion = NULL;
	}

	// ViewRight Gateway
	pszVersion = (HCHAR *)xmgr_cas_VmxUtil_GetGatewayVersionString();
	if (pszVersion != NULL)
	{
		snprintf(s_stXomCasVerimatrixStatus.szGatewayVersion, sizeof(s_stXomCasVerimatrixStatus.szGatewayVersion), "%s", pszVersion);
		HLIB_STD_MemFree(pszVersion);
		pszVersion = NULL;
	}

	// 필요시 기존 cached data와 새로 받은 data를 비교해서 bChange를 제대로 설정해 주도록 하자
	bChanged = TRUE;

	return bChanged;
}


#define _____PROCEDURE_____

STATIC BUS_Result_t xmproc_cas_verimatrix_BASE(HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{

	switch (message)
	{
		case eMEVT_BUS_CREATE:
			HxLOG_Info("%s created\n", (HINT8*) __FUNCTION__);
			BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
			break;


		case eMEVT_CAS_NOTIFICATION:
			HxLOG_Info("eMEVT_CAS_NOTIFICATION, p1: 0x%x\n", p1);
			if(eDxCAS_GROUPID_VX == p1)
			{
				xom_cas_verimatrix_CheckStatusChange_BASE();
			}
			break;



		default:
			break;
	}

	return MESSAGE_PASS;

}



#define ___GLOBAL_FUNCTIONS___
HERROR xom_cas_verimatrix_Init_BASE(void)
{
	HERROR					hErr;
	HINT32					nRpcHandle = 0;
	HxLOG_Trace();

	(void)xom_cas_verimatrix_RefreshData_BASE();

	// Create event handler proc
	BUS_MGR_Create (NULL, APP_ACTION_PRIORITY, xmproc_cas_verimatrix_BASE, 0, 0, 0, 0);
	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if(hErr == ERR_OK)
	{
		//	-------   RPC Methods   -------	//
		hErr = HLIB_RPC_RegisterRequest (nRpcHandle, RPC_OAPI_CAS_VERIMATRIX_Init, NULL, (HxRPC_Func_t)xom_cas_verimatrix_InitRpc_BASE, NULL,
								"Activate the cas.verimatrix module: (void)");
		if(hErr != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterRequest (nRpcHandle, %s) returned error(%d)\n", RPC_OAPI_CAS_VERIMATRIX_Init, hErr);
		}

		//	-------   Notifier  Callbakcs------- //
		hErr = HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_CAS_VERIMATRIX_onStatusChanged, "b",
								"Verimatrix Cas parameters and status changed.\n"
								"\t   - (OxCasVerimatrixStatus_t *pstXomCasVerimatrixStatus)\n"
								);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterCallbackProtoType (nRpcHandle, %s) returned error(%d)\n", RPC_OBAMA_CAS_VERIMATRIX_onStatusChanged, hErr);
		}

	}
	return ERR_OK;
}



