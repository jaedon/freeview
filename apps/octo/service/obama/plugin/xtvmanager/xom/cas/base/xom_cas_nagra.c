/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xom_cas_nagra.c
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
#if defined (CONFIG_OP_SES)
#include <mgr_pg.h>
#endif
#include "../local_include/_xom_cas.h"

#include <xmgr_swup.h>

#if defined(CONFIG_SUPPORT_NOVA_APP)
#include <om_common.h>
#include <xmgr_novaspread.h>
#endif

/*******************************************************************/
/********************      Defines         *************************/

#define	XOM_CAS_NAGRA_CLIENT_VER_LENG			256
#define	XOM_CAS_NAGRA_SYSTEM_INFO_COMPONENT_LENG		16

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

STATIC OxCasNagraStatus_t			s_stXomCasNagraStatus;
#if defined(CONFIG_SUPPORT_NOVA_APP)
STATIC OxCasNagraPrm_TransportSessionInfo_t			stNovaTransportSessionInfo;
STATIC OxCasNagraPrm_NovaLicenseNotifier_t 			stNovaUpdateLcm;
STATIC OxCasNagraPrm_Cas_Item_t						stNovaUpdateKeyHandler;
STATIC OxCasNagraPrm_NovaPlatformUsageRulesNotifier_t 	stNovaUpdatePlatformUsageRules;
STATIC OxCasNagraPrm_NovaServiceUsageRuleseNotifier_t 	stNovaUpdateServiceUsageRules;
#endif
/*******************************************************************/
/********************     Extern Functions     ********************/
/*******************************************************************/
extern HERROR	xmgr_cas_NaGetSysNUID(HUINT8* pszStr, HINT32 nLength);
extern HERROR	xmgr_cas_NaGetCANumber(HUINT8* paucCaSer);
extern HUINT8	xmgr_cas_NaPincodeCheckSCRemoved(void);
extern HERROR	xmgr_cas_NaCheckSmartCardPin(HUINT8* szPicode);
extern HERROR	xmgr_cas_NaGetAccessibleState(HUINT8 *pnAccessState, HUINT32 *pulRemainTime, HUINT8 *pnRetryCount);
extern HERROR	xmgr_cas_NaVerifyPincodeGetAccessibleState(HUINT8 *pnAccessState, HUINT32 *pulRemainTime, HUINT8 *pnRetryCount, HUINT8* szPincode);
extern HERROR	xmgr_cas_NaChangeSmartCardPin(HUINT8* szOldPincode, HUINT8* szNewPincode);
extern HERROR	xmgr_cas_NaPincodeInit(void);
#if defined(CONFIG_SUPPORT_NOVA_APP)
extern HERROR xmgr_cas_na_prm_NovaCaInfo(HUINT8 *pucData);
#endif

/*******************************************************************/
/********************     Static Functions     ********************/
/*******************************************************************/
STATIC void xom_cas_nagra_CheckStatusChange_BASE(void);
STATIC HERROR xom_cas_nagra_InitRpc_BASE(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData);
STATIC HBOOL xom_cas_nagra_RefreshData_BASE(void);


#define ___LOCAL_FUNCTIONS___

#define _____OAPI_CALLBACK_FUNCTIONS_____
STATIC void xom_cas_nagra_PrintHexDump(HUINT8 *pusTitle, HUINT8 *pusData, HUINT32 ulSize)
{
#if defined(CONFIG_DEBUG)	
	int i;
	if(!pusData || ulSize<=0)
	{
		return;
	}

	HxLOG_Print("\n\t\t\t*  %s Hex Dump(size = %d)\n", pusTitle, ulSize);

	for(i=0;i<ulSize;i++)
	{
		if(i%16 == 0)
			HxLOG_Print ("\n \t\t\t* %08xh: ", i);
		HxLOG_Print ("%02X ", pusData[i]);
	}
	HxLOG_Print ("\n");
#endif	
}

STATIC void xom_cas_nagra_CheckStatusChange_BASE(void)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;
	HBOOL					bChanged = FALSE;

	bChanged = xom_cas_nagra_RefreshData_BASE();
	HxLOG_Info("bChanged: %d \n", bChanged);
	if(bChanged == TRUE)
	{
		hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);

		if (hErr == ERR_OK)
		{
			HLIB_RPC_Notify(nRpcHandle, RPC_OBAMA_CAS_NAGRA_onStatusChanged, "b", &s_stXomCasNagraStatus, sizeof(OxCasNagraStatus_t));
		}
	}

}

#define _____OAPI_COMMAND_FUNCTIONS_____
STATIC HERROR xom_cas_nagra_InitRpc_BASE(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle() : fail \n");
		return ERR_FAIL;
	}

	HxLOG_Debug("\t%s(reqID:0x%x)\n", __FUNCTION__, nRequestId);
	HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_OK, "b", &s_stXomCasNagraStatus, sizeof(OxCasNagraStatus_t));

	return ERR_OK;
}

STATIC HERROR xom_cas_nagra_IsSCInserted(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{

	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle() : fail \n");
		return ERR_FAIL;
	}


	if (xmgr_cas_NaPincodeCheckSCRemoved())
	{
		HxLOG_Debug("Smartcard not inserted!>> RPC Response - ERR_FAIL \n");
		HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_FAIL, NULL);
	}
	else
	{
		HxLOG_Debug("Smartcard inserted!>> RPC Response - ERR_OK \n");
		HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_OK, NULL);
	}

	return ERR_OK;
}

STATIC HERROR xom_cas_nagra_pincode_Verify(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{

	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle() : fail \n");
		return ERR_FAIL;
	}

	hErr = xmgr_cas_NaCheckSmartCardPin(HxOBJECT_STR (argv[0]));
	if (hErr == ERR_OK)
	{
		HxLOG_Debug("+++ RPC Response - OK \n");
		HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_OK, NULL);
	}
	else
	{
		HxLOG_Debug("+++ RPC Response - FAIL \n");
		HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_FAIL, NULL);
	}

	return ERR_OK;
}

STATIC HERROR xom_cas_nagra_GetAccessibleState(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;
	HUINT32 	ulRemainTime = 0;
	HUINT8	nRetryCount;
	HUINT8 	nAccessState;
	OxCasNagraPinAccess_RPC_Item_t		stItem;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle() : fail \n");
		return ERR_FAIL;
	}	

	hErr = xmgr_cas_NaGetAccessibleState(&nAccessState, &ulRemainTime, &nRetryCount);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("xmgr_cas_NaGetAccessibleState() fail \n");

		stItem.nAccessState = nAccessState;
		stItem.ulRemainTime = ulRemainTime;
		stItem.nRetryCount = nRetryCount;

		HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_FAIL, "b", (void *)&stItem, sizeof(OxCasNagraPinAccess_RPC_Item_t));
	}
	else
	{
		stItem.nAccessState = nAccessState;
		stItem.ulRemainTime = ulRemainTime;
		stItem.nRetryCount = nRetryCount;

		HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_OK, "b", (void *)&stItem, sizeof(OxCasNagraPinAccess_RPC_Item_t));
	}

	return ERR_OK;
}

STATIC HERROR xom_cas_nagra_VerifyPincodeGetAccessibleState(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;
	HUINT32 	ulRemainTime = 0;
	HUINT8	nRetryCount;
	HUINT8 	nAccessState;
	OxCasNagraPinAccess_RPC_Item_t		stItem;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle() : fail \n");
		return ERR_FAIL;
	}

	hErr = xmgr_cas_NaVerifyPincodeGetAccessibleState(&nAccessState, &ulRemainTime, &nRetryCount, HxOBJECT_STR (argv[0]));
	if (hErr != ERR_OK)
	{
		HxLOG_Error("xmgr_cas_NaVerifySmartCardPin() fail \n");

		stItem.nAccessState = nAccessState;
		stItem.ulRemainTime = ulRemainTime;
		stItem.nRetryCount = nRetryCount;

		HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_FAIL, "b", (void *)&stItem, sizeof(OxCasNagraPinAccess_RPC_Item_t));
	}
	else
	{
		stItem.nAccessState = nAccessState;
		stItem.ulRemainTime = ulRemainTime;
		stItem.nRetryCount = nRetryCount;

		HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_OK, "b", (void *)&stItem, sizeof(OxCasNagraPinAccess_RPC_Item_t));
	}

	return ERR_OK;
}

STATIC HERROR	xom_cas_nagra_pincode_Set(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle() : fail \n");
		return ERR_FAIL;
	}

	hErr = xmgr_cas_NaChangeSmartCardPin(HxOBJECT_STR(argv[0]), HxOBJECT_STR(argv[1]));
	if (hErr != ERR_OK)
	{
		HxLOG_Error("xmgr_cas_NaChangeSmartCardPin() fail \n");
		HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_FAIL, NULL);
	}
	else
	{
		HxLOG_Error("xmgr_cas_NaChangeSmartCardPin() success \n");
		HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_OK, NULL);
	}

	return ERR_OK;
}

#if defined (CONFIG_OP_SES)
STATIC HERROR	xom_cas_nagra_pincode_UnlockAV(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle() : fail \n");
		return ERR_FAIL;
	}

	HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_OK, NULL);
	BUS_PostMessage(NULL, eMEVT_PG_PIN_CHECK_OK, (Handle_t)NULL, 0, 0, 0);

	return ERR_OK;
}
#endif

#if defined(CONFIG_SUPPORT_NOVA_APP)
STATIC HERROR xom_cas_nagra_prm_NovaTransportSessionInfoNotifier(HUINT32 p1, HUINT32 p2, HUINT32 p3)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	HxLOG_Trace();

	HxSTD_memset(&stNovaTransportSessionInfo, 0, sizeof(xMgrCas_TransportSessionInfo_t));
	HxSTD_memcpy(&stNovaTransportSessionInfo, p3,  sizeof(xMgrCas_TransportSessionInfo_t));


#if defined(CONFIG_DEBUG)
	HxLOG_Debug("*\t hAction(0x%08x)\n", stNovaTransportSessionInfo.hAction);
	HxLOG_Debug("*\t ulTransportSessionId(%d)\n", stNovaTransportSessionInfo.ulTransportSessionId);
	HxLOG_Debug("*\t ulOnId(0x%08x)\n", stNovaTransportSessionInfo.usOnId);
	HxLOG_Debug("*\t ulTsId(0x%08x)\n", stNovaTransportSessionInfo.usTsId);
	HxLOG_Debug("*\t ulSvcId(0x%08x)\n", stNovaTransportSessionInfo.usSvcId);
	HxLOG_Debug("*\t usAudioPid(0x%08x)\n", stNovaTransportSessionInfo.usAudioPid);
	HxLOG_Debug("*\t usVideoPid(0x%08x)\n", stNovaTransportSessionInfo.usVideoPid);	
#endif

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);

	if (hErr == ERR_OK)
	{
		HLIB_RPC_Notify(nRpcHandle, RPC_OBAMA_CAS_NAGRA_PRM_NovaTransportSessionInfoNotifier, "b", &stNovaTransportSessionInfo, sizeof(OxCasNagraPrm_TransportSessionInfo_t));
	}
	else
	{
		HxLOG_Error("[%s:%d] Rpc is fail \n", __FUNCTION__, __LINE__);
	}

	return ERR_OK;
}

STATIC HERROR xom_cas_nagra_prm_NovaLicenseNotifier(HUINT32 p1, HUINT32 p2, HUINT32 p3)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	HxLOG_Trace();

	HxSTD_memset(&stNovaUpdateLcm, 0, sizeof(OxCasNagraPrm_NovaLicenseNotifier_t));

	stNovaUpdateLcm.ulRecordHandle = p1;
	stNovaUpdateLcm.bRequestLicenseAgain = (p2 >> 31) & 0x1;
	stNovaUpdateLcm.ulTransportSessionId = (p2 >> 16) & 0x7fff;
	stNovaUpdateLcm.ulSize = p2 & 0xffff;

	HxSTD_memcpy(&stNovaUpdateLcm.ucData[0], p3, stNovaUpdateLcm.ulSize);
#if defined(CONFIG_DEBUG)
	HxLOG_Debug("*\t ulTransportSessionId = %d\n", stNovaUpdateLcm.ulTransportSessionId);
	HxLOG_Debug("*\t ulRecordHandle = 0x%08x\n", stNovaUpdateLcm.ulRecordHandle);
	HxLOG_Debug("*\t bRequestLicenseAgain = %d\n", stNovaUpdateLcm.bRequestLicenseAgain);
	HxLOG_Debug("*\t ulSize = %d\n", stNovaUpdateLcm.ulSize);
	xom_cas_nagra_PrintHexDump("XOM : License Notifier : ", &stNovaUpdateLcm.ucData[0], stNovaUpdateLcm.ulSize);
#endif

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);

	if (hErr == ERR_OK)
	{
		HLIB_RPC_Notify(nRpcHandle, RPC_OBAMA_CAS_NAGRA_PRM_NovaLicenseNotifier, "b", &stNovaUpdateLcm, sizeof(OxCasNagraPrm_NovaLicenseNotifier_t));
	}
	else
	{
		HxLOG_Error("[%s:%d] Rpc is fail \n", __FUNCTION__, __LINE__);
	}

	return ERR_OK;
}

STATIC HERROR xom_cas_nagra_prm_NovaKeyHandlerNotifier(HUINT32 p1, HUINT32 p2, HUINT32 p3)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	HxLOG_Trace();

	HxSTD_memset(&stNovaUpdateKeyHandler, 0, sizeof(OxCasNagraPrm_Cas_Item_t));

	stNovaUpdateKeyHandler.NovaCasType = eNOVA_CAS_GET_KEYHANDLER;
	HxSTD_memcpy(&stNovaUpdateKeyHandler.ucData[0], p3, p2);

#if defined(CONFIG_DEBUG)
	xom_cas_nagra_PrintHexDump("XOM : Key Handler Notifier : ", &stNovaUpdateKeyHandler.ucData[0], p2);
#endif

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);

	if (hErr == ERR_OK)
	{
		HLIB_RPC_Notify(nRpcHandle, RPC_OBAMA_CAS_NAGRA_PRM_NovaKeyHandlerNotifier, "b", &stNovaUpdateKeyHandler, sizeof(OxCasNagraPrm_Cas_Item_t));
	}
	else
	{
		HxLOG_Error("[%s:%d] Rpc is fail \n", __FUNCTION__, __LINE__);
	}

	return ERR_OK;
}

STATIC HERROR xom_cas_nagra_prm_NovaPlatformUsageRulesNotifier(HUINT32 p1, HUINT32 p2, HUINT32 p3)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	HxLOG_Trace();

	HxSTD_memset(&stNovaUpdatePlatformUsageRules, 0, sizeof(OxCasNagraPrm_NovaPlatformUsageRulesNotifier_t));
	stNovaUpdatePlatformUsageRules.ulSize = p1;
	HxSTD_memcpy(&stNovaUpdatePlatformUsageRules.ucData, p3, stNovaUpdatePlatformUsageRules.ulSize);

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);

	if (hErr == ERR_OK)
	{
		HLIB_RPC_Notify(nRpcHandle, RPC_OBAMA_CAS_NAGRA_PRM_NovaPlatformUsageRulesNotifier, "b", &stNovaUpdatePlatformUsageRules, sizeof(OxCasNagraPrm_NovaPlatformUsageRulesNotifier_t));
	}
	else
	{
		HxLOG_Error("[%s:%d] Rpc is fail \n", __FUNCTION__, __LINE__);
	}

	return ERR_OK;
}

STATIC HERROR xom_cas_nagra_prm_NovaServiceUsageRulesNotifier(HUINT32 p1, HUINT32 p2, HUINT32 p3)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	HxLOG_Trace();

	HxSTD_memset(&stNovaUpdateServiceUsageRules, 0, sizeof(OxCasNagraPrm_NovaServiceUsageRuleseNotifier_t));
	stNovaUpdateServiceUsageRules.ulTransportSessionId = p1;
	stNovaUpdateServiceUsageRules.ulSize = p2;
	HxSTD_memcpy(&stNovaUpdateServiceUsageRules.ucData, p3, stNovaUpdateServiceUsageRules.ulSize);

	HxLOG_Debug("*\t ulTransportSessionId(0x%08x)\n", stNovaUpdateServiceUsageRules.ulTransportSessionId);
	xom_cas_nagra_PrintHexDump("XOM UsageRules : ", &stNovaUpdateServiceUsageRules.ucData[0], stNovaUpdateServiceUsageRules.ulSize);

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);

	if (hErr == ERR_OK)
	{
		HLIB_RPC_Notify(nRpcHandle, RPC_OBAMA_CAS_NAGRA_PRM_NovaServiceUsageRulesNotifier, "b", &stNovaUpdateServiceUsageRules, sizeof(OxCasNagraPrm_NovaServiceUsageRuleseNotifier_t));
	}
	else
	{
		HxLOG_Error("[%s:%d] Rpc is fail \n", __FUNCTION__, __LINE__);
	}
	
	return ERR_OK;
}

STATIC HERROR xom_cas_nagra_NovaCaInfo(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;
	OxCasNagraPrm_Cas_Item_t		stItem;

	HxLOG_Trace();

	HxSTD_MemSet(&stItem, 0, sizeof(OxCasNagraPrm_Cas_Item_t));

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle() : fail \n");
		return ERR_FAIL;
	}

	stItem.NovaCasType = eNOVA_CAS_GET_CAINFO;

	hErr = xmgr_cas_na_prm_NovaCaInfo(&stItem.ucData[0]);

	HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, hErr, "b", (void *)&stItem, sizeof(OxCasNagraPrm_Cas_Item_t));

	return ERR_OK;
}

STATIC HERROR xom_cas_nagra_prm_NovaDrmRelease(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	HUINT32 ulTransportSessionId = 0;
	HUINT32 ulRecordHandle = 0;

	HxLOG_Trace();

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle() : fail \n");
		return ERR_FAIL;
	}

	if (hErr == ERR_OK)
	{
		HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_OK, NULL);
	}
	else
	{
		HxLOG_Error("[%s:%d] Rpc is fail \n", __FUNCTION__, __LINE__);
	}

	ulTransportSessionId = (HUINT32)HxOBJECT_INT(argv[0]);
	ulRecordHandle = (HUINT32)HxOBJECT_STR(argv[1]);

#if defined(CONFIG_DEBUG)
	HxLOG_Debug("*\t ulTransportSessionId(%d)\n", ulTransportSessionId);
	HxLOG_Debug("*\t ulRecordHandle(0x%08x)\n", ulRecordHandle);
#endif

	(void)BUS_PostData(NULL, eOEVT_CAS_NA_NOVA_DRMRELEASE, 0, ulTransportSessionId, ulRecordHandle, NULL, 0);

	return ERR_OK;
}

STATIC HERROR xom_cas_nagra_prm_NovaDrmSetParameter(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;
	xMgrCas_NovaDrmSetParameters_t	stDrmLicense;

	HUINT32 ulTransportSessionId = 0;
	HUINT8 * pOldLicense = NULL;
	HUINT32 ulOldLicenseSize = 0;
	HUINT32 ulCollectionId = 0;
	HUINT32 ulDuration = 0;
	HUINT8 * pUsageRules = NULL;
	HUINT32 ulUsageRulesSize = 0;
									

	HxLOG_Trace();

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle() : fail \n");
		return ERR_FAIL;
	}

	if (hErr == ERR_OK)
	{
		HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_OK, NULL);
	}
	else
	{
		HxLOG_Error("[%s:%d] Rpc is fail \n", __FUNCTION__, __LINE__);
	}

	ulTransportSessionId = (HUINT32)HxOBJECT_INT(argv[0]);
	pOldLicense = (HUINT8 *)HxOBJECT_STR(argv[1]);
	ulOldLicenseSize = (HUINT32)HxOBJECT_INT(argv[2]);
	ulCollectionId = (HUINT32)HxOBJECT_INT(argv[3]);
	ulDuration = (HUINT32)HxOBJECT_INT(argv[4]);
	pUsageRules = (HUINT8 *)HxOBJECT_STR(argv[5]);
	ulUsageRulesSize = (HUINT32)HxOBJECT_INT(argv[6]);

	HxSTD_MemSet(&stDrmLicense, 0x00, sizeof(xMgrCas_NovaDrmSetParameters_t));
	
	stDrmLicense.ulTransportSessionId = ulTransportSessionId;
	HxSTD_MemCopy(&stDrmLicense.stOldLicense.usData[0], pOldLicense, sizeof(HUINT8)*ulOldLicenseSize);
	stDrmLicense.stOldLicense.ulSize = ulOldLicenseSize;
	stDrmLicense.ulCollectionId = ulCollectionId;
	stDrmLicense.ulDuration = ulDuration;
	HxSTD_MemCopy(&stDrmLicense.stUsageRules.usData[0], pUsageRules, sizeof(HUINT8)*ulUsageRulesSize);
	stDrmLicense.stUsageRules.ulSize = ulUsageRulesSize;

#if defined(CONFIG_DEBUG)
	HxLOG_Debug("*\t ulTransportSessionId(%d)\n", stDrmLicense.ulTransportSessionId);
	xom_cas_nagra_PrintHexDump("XOM : OldLicense : ", &stDrmLicense.stOldLicense.usData[0], stDrmLicense.stOldLicense.ulSize);
	HxLOG_Debug("*\t ulCollectionId(%d)\n", stDrmLicense.ulCollectionId);
	HxLOG_Debug("*\t ulDuration(%d)\n", stDrmLicense.ulDuration);
	xom_cas_nagra_PrintHexDump("XOM UsageRules : ", &stDrmLicense.stUsageRules.usData[0], stDrmLicense.stUsageRules.ulSize);	
#endif

	(void)BUS_PostData(NULL, eOEVT_CAS_NA_NOVA_DRMSETPARAMETER, 0, 0, 0, (void*) &stDrmLicense, sizeof(xMgrCas_NovaDrmSetParameters_t));

	return ERR_OK;
}
#endif


STATIC void xom_cas_nagra_RemoveSpace(HUINT8 *pszStr)
{
	HCHAR	*pszWrite = pszStr, *pszRead = pszStr;

	do{

		if(*pszRead != ' ')
			*pszWrite++ = *pszRead;

	}while(*pszRead++);
}

STATIC HBOOL xom_cas_nagra_RefreshData_BASE(void)
{
	HBOOL					bChanged = FALSE;
	HBOOL					hErr = ERR_FAIL;
	Sys_FusingInfo_t 	tInfo;
#if 0		// not used
	HUINT32					ulValue;
#endif
	OxCasNagraStatus_t		tempDat = {0, };

	HxLOG_Trace();

	HxSTD_MemCopy(&tempDat, &s_stXomCasNagraStatus, sizeof(OxCasNagraStatus_t));

	// Initialize OAPI cache data
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


	hErr = xmgr_cas_NaGetSysNUID(tempDat.szNUID, OAPI_CAS_NAGRA_SYSTEM_INFO_COMPONENT_LENG);
	hErr |= xmgr_cas_NaGetCANumber(tempDat.szCASN);
	xom_cas_nagra_RemoveSpace(tempDat.szCASN);

	if(hErr != ERR_OK)
	{
		HxLOG_Error("xmgr_cas_NaGetXXX() error! \n");
	}

	HxLOG_Print("bCWE (%d), bJTAG (%d), bOTP (%d), bSBOOT (%d) \n",  tempDat.bCWE, tempDat.bJTAG, tempDat.bOTP, tempDat.bSBOOT);
	HxLOG_Print("NUID (%s), CASN (%s) \n", tempDat.szNUID, tempDat.szCASN);

	if (xmgr_cas_NaPincodeCheckSCRemoved())
	{
		HxLOG_Debug("Smartcard not inserted!\n");
		tempDat.bSCRemoved = TRUE;
	}
	else
	{
		HxLOG_Debug("Smartcard inserted! \n");
		tempDat.bSCRemoved = FALSE;
	}
	HxLOG_Error("s_stXomCasNagraStatus.bSCRemoved (%d), tempDat.bSCRemoved (%d) \n", s_stXomCasNagraStatus.bSCRemoved, tempDat.bSCRemoved);

	//serializer 로 비교 해 버릴 것을.. :)
	if(s_stXomCasNagraStatus.bCWE != tempDat.bCWE)	bChanged|=TRUE;
	if(s_stXomCasNagraStatus.bJTAG != tempDat.bJTAG)	bChanged|=TRUE;
	if(s_stXomCasNagraStatus.bSBOOT != tempDat.bSBOOT)	bChanged|=TRUE;
	if(s_stXomCasNagraStatus.bOTP != tempDat.bOTP)	bChanged|=TRUE;

	if(s_stXomCasNagraStatus.bSCRemoved != tempDat.bSCRemoved)	bChanged|=TRUE;

	bChanged |= HxSTD_MemCmp(s_stXomCasNagraStatus.szNUID, tempDat.szNUID, sizeof(OAPI_CAS_NAGRA_SYSTEM_INFO_COMPONENT_LENG)) ? FALSE:TRUE;
	bChanged |= HxSTD_MemCmp(s_stXomCasNagraStatus.szCASN, tempDat.szCASN, sizeof(OAPI_CAS_NAGRA_SYSTEM_INFO_COMPONENT_LENG)) ? FALSE:TRUE;

	HxSTD_MemSet(&s_stXomCasNagraStatus, 0x00, sizeof(OxCasNagraStatus_t));
	HxSTD_MemCopy(&s_stXomCasNagraStatus, &tempDat, sizeof(OxCasNagraStatus_t));

	return bChanged;
}


#define _____PROCEDURE_____

STATIC BUS_Result_t xmproc_cas_nagra_BASE(HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{

	switch (message)
	{
		case eMEVT_BUS_CREATE:
			HxLOG_Info("%s created\n", (HINT8*) __FUNCTION__);
			BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
			break;

		case eMEVT_CAS_SYSINFO_UPDATED:
			HxLOG_Info("eMEVT_CAS_SYSINFO_UPDATED, p1: 0x%x\n", p1);
			if(eDxCAS_GROUPID_NA == p1)
			{
				xom_cas_nagra_CheckStatusChange_BASE();
			}
			break;

		case eSEVT_CAS_SC_REMOVED:
			HxLOG_Info("eMEVT_CAS_SYSINFO_UPDATED, p1: 0x%x\n", p1);
			xom_cas_nagra_CheckStatusChange_BASE();
			break;

#if defined(CONFIG_SUPPORT_NOVA_APP)
		case eMEVT_CASPRM_NOVA_TRANSPORTSESSIONINFO_NOTIFIER:
			HxLOG_Info("eMEVT_CASPRM_NOVA_TRANSPORTSESSIONINFO_NOTIFIER, p1: 0x%x\n", p1);
			if(xom_cas_nagra_prm_NovaTransportSessionInfoNotifier(p1, p2, p3) != ERR_OK)
			{
				HxLOG_Error("\txom_cas_nagra_prm_NovaUpdateLcmData : error\n");
			}
			break;
		case eMEVT_CASPRM_NOVA_LICENSE_NOTIFIER:
			HxLOG_Info("eMEVT_CASPRM_NOVA_LICENSE_NOTIFIER, p1: 0x%x\n", p1);
			if(xom_cas_nagra_prm_NovaLicenseNotifier(p1, p2, p3) != ERR_OK)
			{
				HxLOG_Error("\txom_cas_nagra_prm_NovaUpdateLcmData : error\n");
			}
			break;
		case eMEVT_CASPRM_NOVA_KEYHANDLER_NOTIFIER:
			HxLOG_Info("eMEVT_CASPRM_NOVA_KEYHANDLER_NOTIFIER, p1: 0x%x\n", p1);
			if(xom_cas_nagra_prm_NovaKeyHandlerNotifier(p1, p2, p3) != ERR_OK)
			{
				HxLOG_Error("\txom_cas_nagra_prm_NovaUpdateLcmData : error\n");
			}
			break;			
		case eMEVT_CASPRM_NOVA_PLATFORM_USAGERULES_NOTIFIER:
			HxLOG_Info("eMEVT_CASPRM_NOVA_PLATFORM_USAGERULES_NOTIFIER, p1: 0x%x\n", p1);
			if(xom_cas_nagra_prm_NovaPlatformUsageRulesNotifier(p1, p2, p3) != ERR_OK)
			{
				HxLOG_Error("\tom_cas_nagra_prm_NovaPlatformUsageRulesNotifier : error\n");
			}
			break;
		case eMEVT_CASPRM_NOVA_SERVICE_USAGERULES_NOTIFIER:
			HxLOG_Info("eMEVT_CASPRM_NOVA_SERVICE_USAGERULES_NOTIFIER, p1: 0x%x\n", p1);
			if(xom_cas_nagra_prm_NovaServiceUsageRulesNotifier(p1, p2, p3) != ERR_OK)
			{
				HxLOG_Error("\txom_cas_nagra_prm_NovaServiceUsageRulesNotifier : error\n");
			}
			break;
#endif

		default:
			break;
	}

	return MESSAGE_PASS;

}



#define ___GLOBAL_FUNCTIONS___
HERROR xom_cas_nagra_Init_BASE(void)
{
	HERROR					hErr;
	HINT32					nRpcHandle = 0;
	HxLOG_Trace();

	(void)xom_cas_nagra_RefreshData_BASE();

	(void)xmgr_cas_NaPincodeInit();

	// Create event handler proc
	BUS_MGR_Create (NULL, APP_ACTION_PRIORITY, xmproc_cas_nagra_BASE, 0, 0, 0, 0);
	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if(hErr == ERR_OK)
	{
		//	-------   RPC Methods   -------	//
		hErr = HLIB_RPC_RegisterRequest (nRpcHandle, RPC_OAPI_CAS_NAGRA_Init, NULL, (HxRPC_Func_t)xom_cas_nagra_InitRpc_BASE, NULL,
								"Activate the cas.nagra module: (void)");
		if(hErr != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterRequest (nRpcHandle, %s) returned error(%d)\n", RPC_OAPI_CAS_NAGRA_Init, hErr);
		}

		hErr = HLIB_RPC_RegisterRequest (nRpcHandle, RPC_OBAMA_CAS_NAGRA_onSCInserted, NULL, (HxRPC_Func_t)xom_cas_nagra_IsSCInserted, NULL,
								"Check CAS NAGRA SC inserted (void)");
		if(hErr != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterRequest (nRpcHandle, %s) returned error(%d)\n", RPC_OBAMA_CAS_NAGRA_onSCInserted, hErr);
		}

		hErr = HLIB_RPC_RegisterRequest (nRpcHandle, RPC_OBAMA_CAS_NAGRA_PINCODE_VERIFY, "s", (HxRPC_Func_t)xom_cas_nagra_pincode_Verify, NULL,
								"Verify CAS NAGRA PINCODE (const HCHAR *pszPinCode)");
		if(hErr != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterRequest (nRpcHandle, %s) returned error(%d)\n", RPC_OBAMA_CAS_NAGRA_PINCODE_VERIFY, hErr);
		}

		hErr = HLIB_RPC_RegisterRequest (nRpcHandle, RPC_OBAMA_CAS_NAGRA_GetAccessibleState, NULL, (HxRPC_Func_t)xom_cas_nagra_GetAccessibleState, NULL,
								"Get CAS NAGRA Pincode Accessible module: (void)");
		if(hErr != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterRequest (nRpcHandle, %s) returned error(%d)\n", RPC_OBAMA_CAS_NAGRA_VerifyPincodeGetAccessibleState, hErr);
		}

		hErr = HLIB_RPC_RegisterRequest (nRpcHandle, RPC_OBAMA_CAS_NAGRA_VerifyPincodeGetAccessibleState, "s", (HxRPC_Func_t)xom_cas_nagra_VerifyPincodeGetAccessibleState, NULL,
								"Get CAS NAGRA Pincode Accessible module: (const HCHAR *pszPinCode)");
		if(hErr != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterRequest (nRpcHandle, %s) returned error(%d)\n", RPC_OBAMA_CAS_NAGRA_VerifyPincodeGetAccessibleState, hErr);
		}

		hErr = HLIB_RPC_RegisterRequest (nRpcHandle, RPC_OBAMA_CAS_NAGRA_PINCODE_SET, "ss", (HxRPC_Func_t)xom_cas_nagra_pincode_Set, NULL,
								"Set CAS NAGRA Pincode (const HCHAR *pszOldPin, const HCHAR *pszNewPin)");
		if(hErr != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterRequest (nRpcHandle, %s) returned error(%d)\n", RPC_OBAMA_CAS_NAGRA_PINCODE_SET, hErr);
		}
#if defined (CONFIG_OP_SES)
		hErr = HLIB_RPC_RegisterRequest (nRpcHandle, RPC_OBAMA_CAS_NAGRA_UnlockAV, NULL, (HxRPC_Func_t)xom_cas_nagra_pincode_UnlockAV, NULL,
								"Unlock AV (void)");
		if(hErr != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterRequest (nRpcHandle, %s) returned error(%d)\n", RPC_OBAMA_CAS_NAGRA_UnlockAV, hErr);
		}
#endif
#if defined(CONFIG_SUPPORT_NOVA_APP)
		hErr = HLIB_RPC_RegisterRequest (nRpcHandle, RPC_OBAMA_CAS_NAGRA_PRM_NovaGetCaInfo, NULL, (HxRPC_Func_t)xom_cas_nagra_NovaCaInfo, NULL,
								"NovaGetCaInfo: (void)");		
		if(hErr != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterRequest (nRpcHandle, %s) returned error(%d)\n", RPC_OBAMA_CAS_NAGRA_PRM_NovaGetCaInfo, hErr);
		}

		hErr = HLIB_RPC_RegisterRequest (nRpcHandle, RPC_OBAMA_CAS_NAGRA_PRM_NovaDrmRelease, "ii", (HxRPC_Func_t)xom_cas_nagra_prm_NovaDrmRelease, NULL,
								"NovaDrmRelease ((HUINT32 ulTransportSessionId, HUINT32 ulRecordHandle)");
		if(hErr != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterRequest (nRpcHandle, %s) returned error(%d)\n", RPC_OBAMA_CAS_NAGRA_PRM_NovaDrmRelease, hErr);
		}

		hErr = HLIB_RPC_RegisterRequest (nRpcHandle, RPC_OBAMA_CAS_NAGRA_PRM_NovaDrmSetParameters, "isiiisi", (HxRPC_Func_t)xom_cas_nagra_prm_NovaDrmSetParameter, NULL,
								"NovaDrmSetParameters ((HUINT32 ulTransportSessionId, HUINT8 *pOldLicense, HUINT32 ulOldLicenseSize, HUINT32 ulCollectionId, HUINT32 ulDuration, HUINT8 *pUsageRules, HUINT32 ulUsageRulesLength)");
		if(hErr != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterRequest (nRpcHandle, %s) returned error(%d)\n", RPC_OBAMA_CAS_NAGRA_PRM_NovaDrmSetParameters, hErr);
		}
#endif
		//	-------   Notifier  Callbakcs------- //
		hErr = HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_CAS_NAGRA_onStatusChanged, "b",
								"Nagra Cas parameters and status changed.\n"
								"\t   - (OxCasNagraStatus_t *pstCasNagraStatus)\n"
								);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterCallbackProtoType (nRpcHandle, %s) returned error(%d)\n", RPC_OBAMA_CAS_NAGRA_onStatusChanged, hErr);
		}

#if defined(CONFIG_SUPPORT_NOVA_APP)
		hErr = HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_CAS_NAGRA_PRM_NovaTransportSessionInfoNotifier, "b",
								"Nova transportSessionInfo notifier.\n"
								"\t   - (OxCasNagraPrm_NovaTransportSessionInfoNotifier_t *transportSessionInfo)\n"
								);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterCallbackProtoType (nRpcHandle, %s) returned error(%d)\n", RPC_OBAMA_CAS_NAGRA_PRM_NovaTransportSessionInfoNotifier, hErr);
		}

		hErr = HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_CAS_NAGRA_PRM_NovaLicenseNotifier, "b",
								"Nova license notifier.\n"
								"\t   - (OxCasNagraPrm_NovaLicenseNotifier_t *License)\n"
								);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterCallbackProtoType (nRpcHandle, %s) returned error(%d)\n", RPC_OBAMA_CAS_NAGRA_PRM_NovaLicenseNotifier, hErr);
		}

		hErr = HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_CAS_NAGRA_PRM_NovaKeyHandlerNotifier, "b",
								"Nova KeyHandler notifier.\n"
								"\t   - (OxCasNagraPrm_NovaKeyHandlerNotifier_t *KeyHandler)\n"
								);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterCallbackProtoType (nRpcHandle, %s) returned error(%d)\n", RPC_OBAMA_CAS_NAGRA_PRM_NovaKeyHandlerNotifier, hErr);
		}		

		hErr = HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_CAS_NAGRA_PRM_NovaPlatformUsageRulesNotifier, "b",
								"Nova license notifier.\n"
								"\t   - (OxCasNagraPrm_NovaPlatformUsageRulesNotifier_t *PlatformUsageRules)\n"
								);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterCallbackProtoType (nRpcHandle, %s) returned error(%d)\n", RPC_OBAMA_CAS_NAGRA_PRM_NovaPlatformUsageRulesNotifier, hErr);
		}

		hErr = HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_CAS_NAGRA_PRM_NovaServiceUsageRulesNotifier, "b",
								"Nova license notifier.\n"
								"\t   - (OxCasNagraPrm_NovaServiceUsageRuleseNotifier_t *ServiceUsageRules)\n"
								);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterCallbackProtoType (nRpcHandle, %s) returned error(%d)\n", RPC_OBAMA_CAS_NAGRA_PRM_NovaServiceUsageRulesNotifier, hErr);
		}
#endif
	}
	return ERR_OK;
}



