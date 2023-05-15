/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xom_cas_irdeto.c
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

#include <db_param.h>

#include <svc_pipe.h>
#include <svc_sys.h>
#include <svc_cas.h>

#include <bus.h>

#include <mgr_common.h>
#include <mgr_cas.h>
#include "../local_include/_xom_cas.h"

#include <xmgr_swup.h>


/*******************************************************************/
/********************      Defines         *************************/

#define	XOM_CAS_IRDETO_CLIENT_VER_LENG			256
#define	XOM_CAS_IRDETO_SYSTEM_INFO_COMPONENT_LENG		16

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

STATIC OxCasIrdetoStatus_t			s_stXomCasIrdetoStatus;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
STATIC void xom_cas_irdeto_CheckStatusChange_BASE(void);
STATIC HERROR xom_cas_irdeto_InitRpc_BASE(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData);
STATIC HERROR xom_cas_irdeto_SetVirtualSecureCwMode_BASE(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData);
STATIC HBOOL xom_cas_irdeto_RefreshData_BASE(void);


#define ___LOCAL_FUNCTIONS___

#define _____OAPI_CALLBACK_FUNCTIONS_____
STATIC void xom_cas_irdeto_CheckStatusChange_BASE(void)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;
	HBOOL					bChanged = FALSE;

	bChanged = xom_cas_irdeto_RefreshData_BASE();
	HxLOG_Info("bChanged: %d \n", bChanged);
	if(bChanged == TRUE)
	{
		hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);

		if (hErr == ERR_OK)
		{
			HLIB_RPC_Notify(nRpcHandle, RPC_OBAMA_CAS_IRDETO_onStatusChanged, "b", &s_stXomCasIrdetoStatus, sizeof(OxCasIrdetoStatus_t));
		}
	}

}

#define _____OAPI_COMMAND_FUNCTIONS_____
STATIC HERROR xom_cas_irdeto_InitRpc_BASE(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);

	HxLOG_Debug("\t%s(reqID:0x%x)\n", __FUNCTION__, nRequestId);
	HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_OK, "b", &s_stXomCasIrdetoStatus, sizeof(OxCasIrdetoStatus_t));

	return ERR_OK;
}

STATIC HERROR xom_cas_irdeto_SetVirtualSecureCwMode_BASE(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{

//	MWC_PARAM_SetIrdetoVirtualSecureCwSetting(HxOBJECT_INT(argv[0]));

	DB_PARAM_SetItem(eDB_PARAM_ITEM_VIRTUAL_SECURECW_SETTING, (HUINT32)HxOBJECT_INT(argv[0]), 0);

	xom_cas_irdeto_CheckStatusChange_BASE();

	return ERR_OK;
}

extern HERROR	xsvc_cas_IrGetUiData(HUINT8 *pucUiData, DxCAS_UiDataType_e eType);

STATIC HBOOL xom_cas_irdeto_RefreshData_BASE(void)
{
	HBOOL					bChanged = FALSE;
	HBOOL					hErr = ERR_FAIL;
	Sys_FusingInfo_t 		tInfo;
	HUINT32					ulValue;
	OxCasIrdetoStatus_t		tempDat = {0, };

	HxLOG_Trace();


	HxSTD_MemCopy(&tempDat, &s_stXomCasIrdetoStatus, sizeof(OxCasIrdetoStatus_t));

	// Initialize OAPI cache data
#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
	hErr = SVC_SYS_GetFusingInfo( &tInfo );

	if(hErr == ERR_OK)
	{
		tempDat.bCWE = tInfo.ucCWE;
		tempDat.bJTAG = tInfo.ucJTAG;
		tempDat.bOTP = tInfo.ucOTP;
		tempDat.bSBOOT = tInfo.ucSCS;
	}
	else
	{
		HxLOG_Error("SVC_SYS_GetFusingInfo() error! \n");
	}
#endif

	hErr = xsvc_cas_IrGetUiData(tempDat.szMancode, eDxCAS_UIDATATYPE_IRDETO_MANUFACTURERCODE);
	hErr |= xsvc_cas_IrGetUiData(tempDat.szHWCode, eDxCAS_UIDATATYPE_IRDETO_HWCODE);
	hErr |= xsvc_cas_IrGetUiData(tempDat.szVariant, eDxCAS_UIDATATYPE_IRDETO_VARIANT);
	hErr |= xsvc_cas_IrGetUiData(tempDat.szSysId, eDxCAS_UIDATATYPE_IRDETO_SYSTEMID);
	hErr |= xsvc_cas_IrGetUiData(tempDat.szKeyVer, eDxCAS_UIDATATYPE_IRDETO_KEYVERSION);
	hErr |= xsvc_cas_IrGetUiData(tempDat.szSigVer, eDxCAS_UIDATATYPE_IRDETO_SIGNATUREVERSION);
	hErr |= xsvc_cas_IrGetUiData(tempDat.szClientVer, eDxCAS_UIDATATYPE_IRDETO_CLIENTVERSION);
	hErr |= xsvc_cas_IrGetUiData(tempDat.szPrivateData, eDxCAS_UIDATATYPE_IRDETO_PRIVATEDATA);

	if(hErr != ERR_OK)
	{
		HxLOG_Error("MW_CAS_GetIrdetoLoaderXXX() error! \n");
	}

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_VIRTUAL_SECURECW_SETTING, &ulValue, 0);
	if(hErr == ERR_OK)
	{
		tempDat.bVirtualSecureCW = ulValue;
	}
	else
	{
		HxLOG_Error("DB_PARAM_GetItem(eDB_PARAM_ITEM_VIRTUAL_SECURECW_SETTING) error! \n");
	}

	HxLOG_Print("pszClientVersion (%s)\n", tempDat.szClientVer);
	HxLOG_Print("bCWE (%d), bJTAG (%d), bOTP (%d), bSBOOT (%d) \n",  tempDat.bCWE, tempDat.bJTAG, tempDat.bOTP, tempDat.bSBOOT);
	HxLOG_Print("ManCode (%s), HwCode (%s), Variant (%s) \n", tempDat.szMancode, tempDat.szHWCode, tempDat.szVariant);
	HxLOG_Print("SysId (%s), KeyVer (%s), SigVer (%s)\n", tempDat.szSysId, tempDat.szKeyVer, tempDat.szSigVer);
	HxLOG_Print("Virtual Secure CW mode (%d) \n", tempDat.bVirtualSecureCW);
	HxLOG_Print("Private Data (%s) \n", tempDat.szPrivateData);

	XMGR_SWUP_GetControlByte_Irdeto(&tempDat.ucControlByte);

	//serializer 로 비교 해 버릴 것을.. :)
	if(s_stXomCasIrdetoStatus.bVirtualSecureCW != tempDat.bVirtualSecureCW)	bChanged|=TRUE;
	bChanged |= HxSTD_MemCmp(s_stXomCasIrdetoStatus.szClientVer, tempDat.szClientVer, sizeof(OAPI_CAS_IRDETO_CLIENT_VERSION_LENG)) ? FALSE:TRUE;
	if(s_stXomCasIrdetoStatus.bCWE != tempDat.bCWE)	bChanged|=TRUE;
	if(s_stXomCasIrdetoStatus.bJTAG != tempDat.bJTAG)	bChanged|=TRUE;
	if(s_stXomCasIrdetoStatus.bSBOOT != tempDat.bSBOOT)	bChanged|=TRUE;
	if(s_stXomCasIrdetoStatus.bOTP != tempDat.bOTP)	bChanged|=TRUE;
	bChanged |= HxSTD_MemCmp(s_stXomCasIrdetoStatus.szMancode, tempDat.szMancode, sizeof(OAPI_CAS_IRDETO_SYSTEM_INFO_COMPONENT_LENG)) ? FALSE:TRUE;
	bChanged |= HxSTD_MemCmp(s_stXomCasIrdetoStatus.szHWCode, tempDat.szHWCode, sizeof(OAPI_CAS_IRDETO_SYSTEM_INFO_COMPONENT_LENG)) ? FALSE:TRUE;
	bChanged |= HxSTD_MemCmp(s_stXomCasIrdetoStatus.szVariant, tempDat.szVariant, sizeof(OAPI_CAS_IRDETO_SYSTEM_INFO_COMPONENT_LENG)) ? FALSE:TRUE;
	bChanged |= HxSTD_MemCmp(s_stXomCasIrdetoStatus.szSysId, tempDat.szSysId, sizeof(OAPI_CAS_IRDETO_SYSTEM_INFO_COMPONENT_LENG)) ? FALSE:TRUE;
	bChanged |= HxSTD_MemCmp(s_stXomCasIrdetoStatus.szKeyVer, tempDat.szKeyVer, sizeof(OAPI_CAS_IRDETO_SYSTEM_INFO_COMPONENT_LENG)) ? FALSE:TRUE;
	bChanged |= HxSTD_MemCmp(s_stXomCasIrdetoStatus.szSigVer, tempDat.szSigVer, sizeof(OAPI_CAS_IRDETO_SYSTEM_INFO_COMPONENT_LENG)) ? FALSE:TRUE;
	if(s_stXomCasIrdetoStatus.ucControlByte != tempDat.ucControlByte)	bChanged|=TRUE;
	bChanged |= HxSTD_MemCmp(s_stXomCasIrdetoStatus.szPrivateData, tempDat.szPrivateData, sizeof(OAPI_CAS_IRDETO_CLIENT_VERSION_LENG)) ? FALSE:TRUE;

	HxSTD_MemSet(&s_stXomCasIrdetoStatus, 0x00, sizeof(OxCasIrdetoStatus_t));
	HxSTD_MemCopy(&s_stXomCasIrdetoStatus, &tempDat, sizeof(OxCasIrdetoStatus_t));

	return bChanged;
}

#define _____PROCEDURE_____
STATIC BUS_Result_t xmproc_cas_irdeto_BASE(HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{

	switch (message)
	{
		case eMEVT_BUS_CREATE:
			HxLOG_Info("%s created\n", (HINT8*) __FUNCTION__);
			BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
			break;

		case eMEVT_CAS_NOTIFICATION:
			HxLOG_Info("eMEVT_CAS_NOTIFICATION, p1: 0x%x\n", p1);
			if(eDxCAS_GROUPID_IR == p1)
			{
				xom_cas_irdeto_CheckStatusChange_BASE();
			}
			break;

		default:
			break;
	}

	return MESSAGE_PASS;
}

#define ___GLOBAL_FUNCTIONS___
HERROR xom_cas_irdeto_Init_BASE(void)
{
	HERROR					hErr;
	HINT32					nRpcHandle = 0;
	HxLOG_Trace();

	(void)xom_cas_irdeto_RefreshData_BASE();

	// Create event handler proc
	BUS_MGR_Create (NULL, APP_ACTION_PRIORITY, xmproc_cas_irdeto_BASE, 0, 0, 0, 0);
	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if(hErr == ERR_OK)
	{
		//	-------   RPC Methods   -------	//
		hErr = HLIB_RPC_RegisterRequest (nRpcHandle, RPC_OAPI_CAS_IRDETO_Init, NULL, (HxRPC_Func_t)xom_cas_irdeto_InitRpc_BASE, NULL,
								"Activate the cas.irdeto module: (void)");
		if(hErr != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterRequest (nRpcHandle, %s) returned error(%d)\n", RPC_OAPI_CAS_IRDETO_Init, hErr);
		}

		hErr = HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_CAS_IRDETO_SetVirtualSecureCwMode, "i", (HxRPC_Func_t)xom_cas_irdeto_SetVirtualSecureCwMode_BASE, NULL,
								"Set Vitual Secure CW mode");
		if(hErr != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterMethod (nRpcHandle, %s) returned error(%d)\n", RPC_OAPI_CAS_IRDETO_SetVirtualSecureCwMode, hErr);
		}

		//	-------   Notifier  Callbakcs------- //
		hErr = HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_CAS_IRDETO_onStatusChanged, "b",
								"Irdeto Cas parameters and status changed.\n"
								"\t   - (OxCasIrdetoStatus_t *pstCasIrdetoStatus)\n"
								);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterCallbackProtoType (nRpcHandle, %s) returned error(%d)\n", RPC_OBAMA_CAS_IRDETO_onStatusChanged, hErr);
		}

	}
	return ERR_OK;
}



