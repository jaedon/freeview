/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_cas_nagra.c
	@brief

	Description:  									\n
	Module: OAPI / API					\n

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
#include <vkernel.h>
#include <hlib.h>
#include <oapi.h>
#include <obama_int.h>

#include "../../oapi_mem.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define	ENTER_CAS_NAGRA_CACHE	VK_SEM_Get(s_ulOapiCasNagraSemId)
#define	LEAVE_CAS_NAGRA_CACHE	VK_SEM_Release(s_ulOapiCasNagraSemId)

#define CHECK_INIT	{if(s_bCasNagraInit != TRUE) return ERR_FAIL;}

#if defined(CONFIG_SUPPORT_NOVA_APP)
#define CHECK_NOVA_DRM_INIT	{if(s_bCasNovaDrmInit != TRUE) return ERR_FAIL;}
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HULONG					s_ulOapiCasNagraSemId;
STATIC HBOOL					s_bCasNagraInit;
STATIC OxCasNagraStatus_t		s_stOapiCasNagraStatus;
STATIC OxCasNagra_Notifier_t	s_pfCasNagraApkNotifier;
#if defined(CONFIG_SUPPORT_NOVA_APP)
STATIC HBOOL					s_bCasNovaDrmInit;
STATIC OxCasNagra_NovaTransportSessionInfoNotifier_t	s_stNovaTransportSessionInfoNotifier;
STATIC OxCasNagra_NovaLicenseNotifier_t 				s_stNovaLicenseNotifier;
STATIC OxCasNagra_NovaKeyHandlerNotifier_t 				s_stNovaKeyHandlerNotifier;
STATIC OxCasNagra_NovaPlatformUsageRulesNotifier_t		s_stNovaPlatformUsageRulesNotifier;
STATIC OxCasNagra_NovaServiceUsageRulesNotifier_t		s_stNovaServiceUsageRulesNotifier;
#endif

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___


#define _____NOTIFICATION_CALLBACKS_____

STATIC void oapi_cas_nagra_onInited(HERROR hErr, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	if(argc != 1)
	{
		HxLOG_Error("==========> ERROR !!!! %s() has invalid argc[%d]\n\n", __FUNCTION__, argc);
		return;
	}
	else if(HxOBJECT_BIN_SIZE(argv[0]) != sizeof(OxCasNagraStatus_t))
	{
		HxLOG_Error("==========> ERROR !!!! %s() has invalid struct size [%d]\n\n", __FUNCTION__, HxOBJECT_BIN_SIZE(argv[0]));
		return;
	}
	else
	{
		s_stOapiCasNagraStatus = *(OxCasNagraStatus_t *)HxOBJECT_BIN_DATA(argv[0]);
		HxLOG_Debug("OAPI_CAS_NAGRA status cache refreshed. Good to go ==============>\n");
	}
}

STATIC void oapi_cas_nagra_onStatusChanged(HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{	
	if(argc != 1)
	{
		HxLOG_Error("==========> ERROR !!!! %s() has invalid argc[%d]\n\n", __FUNCTION__, argc);
		return;
	}
	else if(HxOBJECT_BIN_SIZE(argv[0]) != sizeof(OxCasNagraStatus_t))
	{
		HxLOG_Error("==========> ERROR !!!! %s() has invalid struct size [%d]\n\n", __FUNCTION__, HxOBJECT_BIN_SIZE(argv[0]));
		return;
	}
	else
	{


		ENTER_CAS_NAGRA_CACHE;
		s_stOapiCasNagraStatus = *(OxCasNagraStatus_t *)HxOBJECT_BIN_DATA(argv[0]);
		LEAVE_CAS_NAGRA_CACHE;

		if(s_pfCasNagraApkNotifier != NULL)
		{
			(s_pfCasNagraApkNotifier)();
		}

	}

}

STATIC void oapi_cas_nagra_pincode_onSCInserted(HERROR hErr, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	HBOOL	*pbSCInserted = (HBOOL*)userdata;

	HxLOG_Debug("*pbSCInserted = %d \n", *pbSCInserted);

	if (hErr == ERR_OK)
	{
		*pbSCInserted = TRUE;
		HxLOG_Debug("*pbSCInserted = %d, ERR_OK \n", *pbSCInserted);
	}
	else
	{
		*pbSCInserted = FALSE;
		HxLOG_Debug("*pbSCInserted = %d, ERR_FAIL \n", *pbSCInserted);
	}

}

STATIC void oapi_cas_nagra_pincode_onVerified(HERROR hErr, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	HBOOL	*pbVerified = (HBOOL*)userdata;

	HxLOG_Debug("*pbVerified = %d \n", *pbVerified);

	if (hErr == ERR_OK)
	{
		*pbVerified = TRUE;
		HxLOG_Debug("*pbVerified = %d, ERR_OK \n", *pbVerified);
	}
	else
	{
		*pbVerified = FALSE;
		HxLOG_Debug("*pbVerified = %d, ERR_FAIL \n", *pbVerified);
	}

}

STATIC void oapi_cas_nagra_onGetAccessibleState(HERROR hErr, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	OxCasNagraPinAccess_RPC_Item_t		*pstItem = (OxCasNagraPinAccess_RPC_Item_t *)userdata;	// return data
	OxCasNagraPinAccess_RPC_Item_t 	*pstInfo = (OxCasNagraPinAccess_RPC_Item_t *)HxOBJECT_BIN_DATA(argv[0]);	// rpc 로 받은 데이터

	if (pstInfo == NULL || pstItem == NULL)
	{
		HxLOG_Error("NULL error \n");
		return;
	}

	HxLOG_Debug("hErr = %d, pstInfo->nAccessState = %d, pstInfo->ulRemainTime = %d, pstInfo->nRetryCount = %d  \n",
				hErr, pstInfo->nAccessState, pstInfo->ulRemainTime, pstInfo->nRetryCount);

	HxSTD_memcpy(pstItem, pstInfo, sizeof(OxCasNagraPinAccess_RPC_Item_t));

	if (pstItem == NULL)
	{
		HxLOG_Error("pstItem == NULL \n");
		return;
	}

	HxLOG_Debug("hErr = %d, pstItem->nAccessState = %d, pstItem->ulRemainTime = %d, pstItem->nRetryCount = %d  \n",
				hErr, pstItem->nAccessState, pstItem->ulRemainTime, pstItem->nRetryCount);

}

STATIC void oapi_cas_nagra_onVerifyPincodeAccessibleState(HERROR hErr, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	OxCasNagraPinAccess_RPC_Item_t		*pstItem = (OxCasNagraPinAccess_RPC_Item_t *)userdata;	// return data
	OxCasNagraPinAccess_RPC_Item_t 	*pstInfo = (OxCasNagraPinAccess_RPC_Item_t *)HxOBJECT_BIN_DATA(argv[0]);	// rpc 로 받은 데이터

	if (pstInfo == NULL || pstItem == NULL)
	{
		HxLOG_Error("NULL error \n");
		return;
	}

	HxSTD_memcpy(pstItem, pstInfo, sizeof(OxCasNagraPinAccess_RPC_Item_t));

	if (pstItem == NULL)
	{
		HxLOG_Error("pstItem == NULL \n");
		return;
	}

	HxLOG_Debug("hErr = %d, pstItem->nAccessState = %d, pstItem->ulRemainTime = %d, pstItem->nRetryCount = %d  \n",
				hErr, pstItem->nAccessState, pstItem->ulRemainTime, pstItem->nRetryCount);

}

STATIC void oapi_cas_nagra_pincode_onSet(HERROR hErr, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	HBOOL	*pbVerified = (HBOOL*)userdata;

	if (hErr == ERR_OK)
	{
		*pbVerified = TRUE;
		HxLOG_Error("*pbVerified = %d, ERR_OK \n", *pbVerified);
	}
	else
	{
		*pbVerified = FALSE;
		HxLOG_Error("*pbVerified = %d, ERR_FAIL \n", *pbVerified);
	}

}

STATIC void oapi_cas_nagra_onUnlockAV(HERROR hErr, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	// not used yet
}

#if defined(CONFIG_SUPPORT_NOVA_APP)
STATIC void oapi_cas_nagra_prm_onNovaSetServiceUsageRulesReceivedListener(HERROR hErr, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	HBOOL	*pbVerified = (HBOOL*)userdata;

	HxLOG_Trace();

	if (hErr == ERR_OK)
	{
		*pbVerified = TRUE;
		HxLOG_Debug("*\t pbVerified = %d, ERR_OK \n", *pbVerified);
	}
	else
	{
		*pbVerified = FALSE;
		HxLOG_Debug("*\t pbVerified = %d, ERR_FAIL \n", *pbVerified);
	}	
}

STATIC void oapi_cas_nagra_prm_onNovaCaInfo(HERROR hErr, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	OxCasNagraPrm_Cas_Item_t		*pstItem = (OxCasNagraPrm_Cas_Item_t *)userdata;
	OxCasNagraPrm_Cas_Item_t 	*pstInfo = (OxCasNagraPrm_Cas_Item_t *)HxOBJECT_BIN_DATA(argv[0]);

	HxLOG_Trace();

	if (pstInfo == NULL || pstItem == NULL)
	{
		HxLOG_Error("NULL error \n");
		return;
	}


	HxSTD_memcpy(pstItem, pstInfo, sizeof(OxCasNagraPrm_Cas_Item_t));

	if (pstItem == NULL)
	{
		HxLOG_Error("pstItem == NULL \n");
		return;
	}
}

STATIC void oapi_cas_nagra_prm_onNovaDrmRelease(HERROR hErr, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	HBOOL	*pbVerified = (HBOOL*)userdata;

	HxLOG_Trace();

	if (hErr == ERR_OK)
	{
		*pbVerified = TRUE;
		HxLOG_Debug("*\t pbVerified = %d, ERR_OK \n", *pbVerified);
	}
	else
	{
		*pbVerified = FALSE;
		HxLOG_Debug("*\t pbVerified = %d, ERR_FAIL \n", *pbVerified);
	}	
}

STATIC void oapi_cas_nagra_onNovaDrmRelease(HERROR hErr, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	HBOOL	*pbVerified = (HBOOL*)userdata;

	if (hErr == ERR_OK)
	{
		*pbVerified = TRUE;
		HxLOG_Error("*pbVerified = %d, ERR_OK \n", *pbVerified);
	}
	else
	{
		*pbVerified = FALSE;
		HxLOG_Error("*pbVerified = %d, ERR_FAIL \n", *pbVerified);
	}

}

STATIC void oapi_cas_nagra_prm_onNovaDrmSetParameters(HERROR hErr, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	HBOOL	*pbVerified = (HBOOL*)userdata;

	HxLOG_Trace();

	if (hErr == ERR_OK)
	{
		*pbVerified = TRUE;
		HxLOG_Debug("*\t pbVerified = %d, ERR_OK \n", *pbVerified);
	}
	else
	{
		*pbVerified = FALSE;
		HxLOG_Debug("*\t pbVerified = %d, ERR_FAIL \n", *pbVerified);
	}	
}

#if 0
STATIC void oapi_cas_nagra_prm_onNovaDrmSetLicenseChangeListener(HERROR hErr, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	HBOOL	*pbVerified = (HBOOL*)userdata;

	HxLOG_Trace();

	if (hErr == ERR_OK)
	{
		*pbVerified = TRUE;
		HxLOG_Debug("*\t pbVerified = %d, ERR_OK \n", *pbVerified);
	}
	else
	{
		*pbVerified = FALSE;
		HxLOG_Debug("*\t pbVerified = %d, ERR_FAIL \n", *pbVerified);
	}	
}
#endif

STATIC void oapi_cas_nagra_prm_onNovaTransportSessionInfoNotifier(HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{	
	OxCasNagraPrm_TransportSessionInfo_t TransportSessionInfo;

	HxLOG_Trace();

	if(argc != 1)
	{
		HxLOG_Error("==========> ERROR !!!! %s() has invalid argc[%d]\n\n", __FUNCTION__, argc);
		return;
	}
	else if(HxOBJECT_BIN_SIZE(argv[0]) != sizeof(OxCasNagraPrm_TransportSessionInfo_t))
	{
		HxLOG_Error("==========> ERROR !!!! %s() has invalid struct size [%d]\n\n", __FUNCTION__, HxOBJECT_BIN_SIZE(argv[0]));
		return;
	}
	else
	{
		HxSTD_memset(&TransportSessionInfo, 0 , sizeof(OxCasNagraPrm_TransportSessionInfo_t));

		//ENTER_CAS_NAGRA_CACHE;
		TransportSessionInfo = *(OxCasNagraPrm_TransportSessionInfo_t *)HxOBJECT_BIN_DATA(argv[0]);
		//LEAVE_CAS_NAGRA_CACHE;

		if(s_stNovaTransportSessionInfoNotifier != NULL)
		{
			s_stNovaTransportSessionInfoNotifier(TransportSessionInfo.hAction
											, TransportSessionInfo.ulTransportSessionId
											, TransportSessionInfo.usOnId
											, TransportSessionInfo.usTsId
											, TransportSessionInfo.usSvcId
											, TransportSessionInfo.usAudioPid
											, TransportSessionInfo.usVideoPid);
		}
		else
		{
			HxLOG_Error("==========> ERROR !!!! %s() oapi_cas_nagra_prm_onNovaTransportSessionInfoNotifier is fail\n\n", __FUNCTION__);
		}
	}	
}

STATIC void oapi_cas_nagra_prm_onNovaLicenseNotifier(HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{	
	OxCasNagraPrm_NovaLicenseNotifier_t License;

	HxLOG_Trace();

	if(argc != 1)
	{
		HxLOG_Error("==========> ERROR !!!! %s() has invalid argc[%d]\n\n", __FUNCTION__, argc);
		return;
	}
	else if(HxOBJECT_BIN_SIZE(argv[0]) != sizeof(OxCasNagraPrm_NovaLicenseNotifier_t))
	{
		HxLOG_Error("==========> ERROR !!!! %s() has invalid struct size [%d]\n\n", __FUNCTION__, HxOBJECT_BIN_SIZE(argv[0]));
		return;
	}
	else
	{
		HxSTD_memset(&License, 0 , sizeof(OxCasNagraPrm_NovaLicenseNotifier_t));

		//ENTER_CAS_NAGRA_CACHE;
		License = *(OxCasNagraPrm_NovaLicenseNotifier_t *)HxOBJECT_BIN_DATA(argv[0]);
		//LEAVE_CAS_NAGRA_CACHE;

		if(s_stNovaLicenseNotifier != NULL)
		{
			HxLOG_Debug("*\t ulTransportSessionId(%d), ulRecordHandle(0x%08x), bRequestLicenseAgain(%d), ulSize(%d), ucData = { %02x, %02x, %02x, ... }\n", 
					License.ulTransportSessionId, License.ulRecordHandle, License.bRequestLicenseAgain, License.ulSize, License.ucData[0], License.ucData[1], License.ucData[2]);

			s_stNovaLicenseNotifier(License.ulTransportSessionId, License.ulRecordHandle, License.ucData, License.ulSize, License.bRequestLicenseAgain);
		}
		else
		{
			HxLOG_Error("==========> ERROR !!!! %s() s_stNovaLicenseNotifier is fail\n\n", __FUNCTION__);
		}
	}	
}

STATIC void oapi_cas_nagra_prm_onNovaKeyHandlerNotifier(HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{	
	OxCasNagraPrm_Cas_Item_t Item;

	HxLOG_Trace();

	if(argc != 1)
	{
		HxLOG_Error("==========> ERROR !!!! %s() has invalid argc[%d]\n\n", __FUNCTION__, argc);
		return;
	}
	else if(HxOBJECT_BIN_SIZE(argv[0]) != sizeof(OxCasNagraPrm_Cas_Item_t))
	{
		HxLOG_Error("==========> ERROR !!!! %s() has invalid struct size [%d]\n\n", __FUNCTION__, HxOBJECT_BIN_SIZE(argv[0]));
		return;
	}
	else
	{
		HxSTD_memset(&Item, 0 , sizeof(OxCasNagraPrm_Cas_Item_t));

		//ENTER_CAS_NAGRA_CACHE;
		Item = *(OxCasNagraPrm_Cas_Item_t *)HxOBJECT_BIN_DATA(argv[0]);

		if(s_stNovaKeyHandlerNotifier != NULL)
		{
			HxLOG_Debug("*\t NovaCasType(%d), ucData = { %02x, %02x, %02x, ... }\n", Item.NovaCasType, Item.ucData[0], Item.ucData[1], Item.ucData[2]);
			if(Item.NovaCasType == eNOVA_CAS_GET_KEYHANDLER)
			{
				s_stNovaKeyHandlerNotifier(&Item.ucData[0]);
			}
			else
			{
				HxLOG_Error("==========> ERROR !!!! %s() NovaCasType(%d) is fail\n\n", __FUNCTION__, Item.NovaCasType);
			}
		}
		else
		{
			HxLOG_Error("==========> ERROR !!!! %s() s_stNovaLicenseNotifier is fail\n\n", __FUNCTION__);
		}
	}	
}

//STATIC void oapi_cas_nagra_prm_onNovaPlatformUsageRulesNotifier(HERROR hErr, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
STATIC void oapi_cas_nagra_prm_onNovaPlatformUsageRulesNotifier(HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	OxCasNagraPrm_NovaPlatformUsageRulesNotifier_t UsageRules;

	HxLOG_Trace();

	if(argc != 1)
	{
		HxLOG_Error("==========> ERROR !!!! %s() has invalid argc[%d]\n\n", __FUNCTION__, argc);
		return;
	}
	else if(HxOBJECT_BIN_SIZE(argv[0]) != sizeof(OxCasNagraPrm_NovaPlatformUsageRulesNotifier_t))
	{
		HxLOG_Error("==========> ERROR !!!! %s() has invalid struct size [%d]\n\n", __FUNCTION__, HxOBJECT_BIN_SIZE(argv[0]));
		return;
	}
	else
	{
		HxSTD_memset(&UsageRules, 0 , sizeof(OxCasNagraPrm_NovaPlatformUsageRulesNotifier_t));

		//ENTER_CAS_NAGRA_CACHE;
		UsageRules = *(OxCasNagraPrm_NovaPlatformUsageRulesNotifier_t *)HxOBJECT_BIN_DATA(argv[0]);
		//LEAVE_CAS_NAGRA_CACHE;

		if(s_stNovaPlatformUsageRulesNotifier != NULL)
		{
			s_stNovaPlatformUsageRulesNotifier(UsageRules.ucData, UsageRules.ulSize);
		}
		else
		{
			HxLOG_Error("==========> ERROR !!!! %s() s_stNovaPlatformUsageRulesNotifier is fail\n\n", __FUNCTION__);
		}		
	}	
}

//STATIC void oapi_cas_nagra_prm_onNovaServiceUsageRulesNotifier(HERROR hErr, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
STATIC void oapi_cas_nagra_prm_onNovaServiceUsageRulesNotifier(HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	OxCasNagraPrm_NovaServiceUsageRuleseNotifier_t UsageRules;

	HxLOG_Trace();

	if(argc != 1)
	{
		HxLOG_Error("==========> ERROR !!!! %s() has invalid argc[%d]\n\n", __FUNCTION__, argc);
		return;
	}
	else if(HxOBJECT_BIN_SIZE(argv[0]) != sizeof(OxCasNagraPrm_NovaServiceUsageRuleseNotifier_t))
	{
		HxLOG_Error("==========> ERROR !!!! %s() has invalid struct size [%d]\n\n", __FUNCTION__, HxOBJECT_BIN_SIZE(argv[0]));
		return;
	}
	else
	{
		HxSTD_memset(&UsageRules, 0 , sizeof(OxCasNagraPrm_NovaServiceUsageRuleseNotifier_t));

		//ENTER_CAS_NAGRA_CACHE;
		UsageRules = *(OxCasNagraPrm_NovaServiceUsageRuleseNotifier_t *)HxOBJECT_BIN_DATA(argv[0]);
		//LEAVE_CAS_NAGRA_CACHE;

		if(s_stNovaServiceUsageRulesNotifier != NULL)
		{
			s_stNovaServiceUsageRulesNotifier(UsageRules.ulTransportSessionId, UsageRules.ucData, UsageRules.ulSize);
		}
		else
		{
			HxLOG_Error("==========> ERROR !!!! %s() s_stNovaServiceUsageRulesNotifier is fail\n\n", __FUNCTION__);
		}		
		
	}	
}
#endif

#define ___GLOBAL_FUNCTIONS___

#if defined(CONFIG_SUPPORT_NOVA_APP)
HERROR OAPI_CAS_NAGRA_NOVA_Init(OxCasNagra_NovaTransportSessionInfoNotifier_t pfNovatTransportSessionInfoNotifier
									, OxCasNagra_NovaLicenseNotifier_t pfNovaLicenseNotifier
									, OxCasNagra_NovaKeyHandlerNotifier_t pfNovaKeyHandlerNotifier
									, OxCasNagra_NovaPlatformUsageRulesNotifier_t pfNovaPlatformUsageRulesNotifier
									, OxCasNagra_NovaServiceUsageRulesNotifier_t pfNovaServiceUsageRulesNotifier)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("-- error! \n");
	}

	HxLOG_Debug("%s()\n", __FUNCTION__);

	if (s_bCasNovaDrmInit != TRUE)
	{
		HLIB_RPC_RegisterNotifyCallback (nRpcHandle, RPC_OBAMA_CAS_NAGRA_PRM_NovaTransportSessionInfoNotifier, "b", NULL, (HxRPC_Noti_t)oapi_cas_nagra_prm_onNovaTransportSessionInfoNotifier);		
		HLIB_RPC_RegisterNotifyCallback (nRpcHandle, RPC_OBAMA_CAS_NAGRA_PRM_NovaLicenseNotifier, "b", NULL, (HxRPC_Noti_t)oapi_cas_nagra_prm_onNovaLicenseNotifier);
		HLIB_RPC_RegisterNotifyCallback (nRpcHandle, RPC_OBAMA_CAS_NAGRA_PRM_NovaKeyHandlerNotifier, "b", NULL, (HxRPC_Noti_t)oapi_cas_nagra_prm_onNovaKeyHandlerNotifier);		
		HLIB_RPC_RegisterNotifyCallback (nRpcHandle, RPC_OBAMA_CAS_NAGRA_PRM_NovaPlatformUsageRulesNotifier, "b", NULL, (HxRPC_Noti_t)oapi_cas_nagra_prm_onNovaPlatformUsageRulesNotifier);
		HLIB_RPC_RegisterNotifyCallback (nRpcHandle, RPC_OBAMA_CAS_NAGRA_PRM_NovaServiceUsageRulesNotifier, "b", NULL, (HxRPC_Noti_t)oapi_cas_nagra_prm_onNovaServiceUsageRulesNotifier);

		s_stNovaTransportSessionInfoNotifier = pfNovatTransportSessionInfoNotifier;
		s_stNovaLicenseNotifier = pfNovaLicenseNotifier;
		s_stNovaKeyHandlerNotifier = pfNovaKeyHandlerNotifier;		
		s_stNovaPlatformUsageRulesNotifier = pfNovaPlatformUsageRulesNotifier;
		s_stNovaServiceUsageRulesNotifier = pfNovaServiceUsageRulesNotifier;

		s_bCasNovaDrmInit = TRUE;
	}
	else
	{
		HxLOG_Error("==========> ERROR !!!! %s() has already been initialized !!!!\n\n", __FUNCTION__);
	}

	return ERR_OK;
}

#endif

HERROR OAPI_CAS_NAGRA_Init(OxCasNagra_Notifier_t pfApkListener)
{
#if defined(CONFIG_MW_CAS_NAGRA)
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("-- error! \n");
	}

	HxLOG_Debug("%s()\n", __FUNCTION__);

	if (s_bCasNagraInit != TRUE)
	{

		if ( VK_SEM_Create(&s_ulOapiCasNagraSemId, "OapiOutputVideoSema", VK_SUSPENDTYPE_PRIORITY) != VK_OK )
		{
			HxLOG_Error("VK_SEM_Create error! \n");
		}

		HLIB_RPC_RegisterNotifyCallback (nRpcHandle, RPC_OBAMA_CAS_NAGRA_onStatusChanged, "b", NULL, (HxRPC_Noti_t)oapi_cas_nagra_onStatusChanged);

		(void)HLIB_RPC_Request (nRpcHandle, (HxRPC_Response_t)oapi_cas_nagra_onInited, NULL, RPC_OAPI_CAS_NAGRA_Init, NULL);

		s_bCasNagraInit = TRUE;

		s_pfCasNagraApkNotifier = pfApkListener;
	}
	else
	{
		HxLOG_Error("==========> ERROR !!!! %s() has already been initialized !!!!\n\n", __FUNCTION__);
	}
#endif
	return ERR_OK;
}


HERROR OAPI_CAS_NAGRA_GetStatus(OxCasNagraStatus_t *pstStatus)
{
	HxLOG_Trace();
	HxLOG_Assert(pstStatus);

	CHECK_INIT;

	ENTER_CAS_NAGRA_CACHE;
	*pstStatus = s_stOapiCasNagraStatus;
	LEAVE_CAS_NAGRA_CACHE;

	return ERR_OK;
}

HERROR	OAPI_CAS_NAGRA_IsSCInserted(HBOOL *pbSCInserted)
{
	HINT32	nReqId;
	HINT32	nRpcHandle = 0;

	HxLOG_Trace();

	if(OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle) != ERR_OK)
	{
		HxLOG_Error("OAPI_RpcProtocol_GetRpcHandle : error\n");
	}

	nReqId = HLIB_RPC_Request (nRpcHandle, (HxRPC_Response_t)oapi_cas_nagra_pincode_onSCInserted, (void*)pbSCInserted, RPC_OBAMA_CAS_NAGRA_onSCInserted, NULL);
	if (HLIB_RPC_Join (nRpcHandle, nReqId, 10000) == ERR_FAIL)
	{
		HxLOG_Error("error to join - \n");
		*pbSCInserted = FALSE;

		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	OAPI_CAS_NAGRA_PINCODE_Verify(const HCHAR *pszPinStr, HBOOL *pbVerified)
{
	HINT32	nReqId;
	HINT32	nRpcHandle = 0;

	HxLOG_Trace();

	if(OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle) != ERR_OK)
	{
		HxLOG_Error("OAPI_RpcProtocol_GetRpcHandle : error\n");
	}


	nReqId = HLIB_RPC_Request (nRpcHandle, (HxRPC_Response_t)oapi_cas_nagra_pincode_onVerified, (void*)pbVerified, RPC_OBAMA_CAS_NAGRA_PINCODE_VERIFY, "s", pszPinStr);
	if (HLIB_RPC_Join (nRpcHandle, nReqId, 10000) == ERR_FAIL)
	{
		HxLOG_Error("error to join - \n");
		*pbVerified = FALSE;

		return ERR_FAIL;
	}
	HxLOG_Debug("[%s] *pbVerified = %d \n", __FUNCTION__, *pbVerified);

	return ERR_OK;
}

HERROR OAPI_CAS_NAGRA_GetAccessibleState(HUINT8 *pnAccessState, HUINT32 *pulRemainTime, HUINT8 *pnRetryCount)
{
	HINT32	nRpcHandle = 0;
	HINT32	nReqId;
	OxCasNagraPinAccess_RPC_Item_t	*pstItem = NULL;

	HxLOG_Trace();

	CHECK_INIT;

	pstItem = (OxCasNagraPinAccess_RPC_Item_t *)HLIB_MEM_Malloc(sizeof(OxCasNagraPinAccess_RPC_Item_t));
	if (pstItem == NULL)
	{
		HxLOG_Error("Alloc fail!!!\n");
		return ERR_FAIL;
	}
	HxSTD_memset(pstItem, 0 , sizeof(OxCasNagraPinAccess_RPC_Item_t));

	if(OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle) != ERR_OK)
	{
		HxLOG_Error("OAPI_RpcProtocol_GetRpcHandle fail!!!\n");
	}

	nReqId = HLIB_RPC_Request (nRpcHandle, (HxRPC_Response_t)oapi_cas_nagra_onGetAccessibleState, (void *)pstItem, RPC_OBAMA_CAS_NAGRA_GetAccessibleState, NULL);
	if (HLIB_RPC_Join (nRpcHandle, nReqId, 10000) == ERR_FAIL)
	{
		HxLOG_Error("error to join - \n");

		if (pstItem)	{	HLIB_MEM_Free(pstItem); }

		return ERR_FAIL;
	}

	*pnAccessState = pstItem->nAccessState;
	*pulRemainTime = pstItem->ulRemainTime;
	*pnRetryCount = pstItem->nRetryCount;

	if (pstItem)	{	HLIB_MEM_Free(pstItem);	}

	return ERR_OK;
}

HERROR OAPI_CAS_NAGRA_VerifyPincodeGetAccessibleState(HUINT8 *pnAccessState, HUINT32 *pulRemainTime, HUINT8 *pnRetryCount, const HCHAR *pszPinStr)
{
	HINT32	nRpcHandle = 0;
	HINT32	nReqId;
	OxCasNagraPinAccess_RPC_Item_t	*pstItem = NULL;

	HxLOG_Trace();

	CHECK_INIT;

	pstItem = (OxCasNagraPinAccess_RPC_Item_t *)HLIB_MEM_Malloc(sizeof(OxCasNagraPinAccess_RPC_Item_t));
	if (pstItem == NULL)
	{
		HxLOG_Error("Alloc fail!!!\n");
		return ERR_FAIL;
	}
	HxSTD_memset(pstItem, 0 , sizeof(OxCasNagraPinAccess_RPC_Item_t));

	if(OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle) != ERR_OK)
	{
		HxLOG_Error("OAPI_RpcProtocol_GetRpcHandle fail!!!\n");	
	}

	nReqId = HLIB_RPC_Request (nRpcHandle, (HxRPC_Response_t)oapi_cas_nagra_onVerifyPincodeAccessibleState, (void *)pstItem, RPC_OBAMA_CAS_NAGRA_VerifyPincodeGetAccessibleState, "s", pszPinStr);
	if (HLIB_RPC_Join (nRpcHandle, nReqId, 10000) == ERR_FAIL)
	{
		HxLOG_Error("error to join - \n");

		if (pstItem)	{	HLIB_MEM_Free(pstItem); }

		return ERR_FAIL;
	}

	*pnAccessState = pstItem->nAccessState;
	*pulRemainTime = pstItem->ulRemainTime;
	*pnRetryCount = pstItem->nRetryCount;

	if (pstItem)	{	HLIB_MEM_Free(pstItem);	}

	return ERR_OK;
}

HERROR	OAPI_CAS_NAGRA_PINCODE_Set(const HCHAR *pszOldPinStr, const HCHAR *pszNewPinStr)
{
	HINT32	nReqId;
	HERROR	err;
	HINT32	nRpcHandle = 0;

	HxLOG_Trace();

	if(OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle) != ERR_OK)
	{
		HxLOG_Error("OAPI_RpcProtocol_GetRpcHandle error\n");
	}

	nReqId = HLIB_RPC_Request (nRpcHandle, (HxRPC_Response_t)oapi_cas_nagra_pincode_onSet, (void*)&err, RPC_OBAMA_CAS_NAGRA_PINCODE_SET, "ss", pszOldPinStr, pszNewPinStr);
	if (HLIB_RPC_Join (nRpcHandle, nReqId, 10000) == ERR_FAIL)
	{
		HxLOG_Error("error to join - \n");

		return ERR_FAIL;
	}

	if (err == TRUE)
	{
		HxLOG_Error("---err = %d \n", err);
		return ERR_OK;
	}
	else
	{
		HxLOG_Error("---err = %d \n", err);
		return ERR_FAIL;
	}
}

#if defined (CONFIG_OP_SES)
HERROR	OAPI_CAS_NAGRA_UnlockAV()
{
	HINT32	nReqId;
	HERROR	err;
	HINT32	nRpcHandle = 0;

	HxLOG_Trace();

	if(OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle) != ERR_OK)
	{
		HxLOG_Error("OAPI_RpcProtocol_GetRpcHandle error\n");
	}

	nReqId = HLIB_RPC_Request (nRpcHandle, (HxRPC_Response_t)oapi_cas_nagra_onUnlockAV, (void*)&err, RPC_OBAMA_CAS_NAGRA_UnlockAV, NULL);
	if (HLIB_RPC_Join (nRpcHandle, nReqId, 10000) == ERR_FAIL)
	{
		HxLOG_Error("error to join - \n");

		return ERR_FAIL;
	}

	if (err == TRUE)
	{
		HxLOG_Error("---err = %d \n", err);
		return ERR_OK;
	}
	else
	{
		HxLOG_Error("---err = %d \n", err);
		return ERR_FAIL;
	}
}
#endif

#if defined(CONFIG_SUPPORT_NOVA_APP)
HERROR OAPI_CAS_NAGRA_PRM_GetCaInfo(HUINT8 *pucData)
{
	HINT32	nRpcHandle = 0;
	HINT32	nReqId;
	OxCasNagraPrm_Cas_Item_t	*pstItem = NULL;

	HxLOG_Trace();

	if(pucData == NULL)
	{
		HxLOG_Error("pucData fail!!!\n");
		return ERR_FAIL;
	}

	pstItem = (OxCasNagraPrm_Cas_Item_t *)HLIB_MEM_Malloc(sizeof(OxCasNagraPrm_Cas_Item_t));
	if (pstItem == NULL)
	{
		HxLOG_Error("Alloc fail!!!\n");
		return ERR_FAIL;
	}

	HxSTD_memset(pstItem, 0 , sizeof(OxCasNagraPrm_Cas_Item_t));

	(void)OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	nReqId = HLIB_RPC_Request (nRpcHandle, (HxRPC_Response_t)oapi_cas_nagra_prm_onNovaCaInfo, (void *)pstItem, RPC_OBAMA_CAS_NAGRA_PRM_NovaGetCaInfo, NULL);
	if (HLIB_RPC_Join (nRpcHandle, nReqId, 10000) == ERR_FAIL)
	{
		HxLOG_Error("error to join - \n");

		if (pstItem)	{	HLIB_MEM_Free(pstItem); }
		return ERR_FAIL;
	}

	if(pstItem->NovaCasType == eNOVA_CAS_GET_CAINFO)
	{
		HxSTD_memcpy(pucData, &pstItem->ucData[0], sizeof(pstItem->ucData));
	}
	
	if (pstItem)	{	HLIB_MEM_Free(pstItem);	}

	return ERR_OK;
}

HERROR OAPI_CAS_NAGRA_PRM_NovaDrmRelease(HUINT32 ulTransportSessionId, HUINT32 ulRecordHandle)
{
	HINT32	nReqId;
	HERROR	err;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	HxLOG_Trace();

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	nReqId = HLIB_RPC_Request (nRpcHandle
									, (HxRPC_Response_t)oapi_cas_nagra_onNovaDrmRelease
									, (void*)&err
									, RPC_OBAMA_CAS_NAGRA_PRM_NovaDrmRelease
									, "ii"
									, ulTransportSessionId
									, ulRecordHandle);

	if (HLIB_RPC_Join (nRpcHandle, nReqId, 10000) == ERR_FAIL)
	{
		HxLOG_Error("[%s:%d] - HLIB_RPC_Join : error to join  = %d \n", __FUNCTION__, __LINE__, err);
		return ERR_FAIL;
	}

	if (err == TRUE)
	{
		return ERR_OK;
	}
	else
	{
		HxLOG_Error("[%s:%d] --- fail = %d \n", __FUNCTION__, __LINE__, err);
		return ERR_FAIL;
	}

	return hErr;
}

HERROR OAPI_CAS_NAGRA_PRM_NovaDrmSetParameters(HUINT32 ulTransportSessionId
									, HUINT8 *pOldLicense, HUINT32 ulOldLicenseSize
									, HUINT32 ulCollectionId
									, HUINT32 ulDuration
									, HUINT8 *pUsageRules, HUINT32 ulUsageRulesLength)
{
	HINT32	nReqId;
	HERROR	err;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	HxLOG_Trace();

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	nReqId = HLIB_RPC_Request (nRpcHandle
									, (HxRPC_Response_t)oapi_cas_nagra_prm_onNovaDrmSetParameters
									, (void*)&err
									, RPC_OBAMA_CAS_NAGRA_PRM_NovaDrmSetParameters
									, "isiiisi"
									, ulTransportSessionId
									, pOldLicense
									, ulOldLicenseSize
									, ulCollectionId
									, ulDuration
									, pUsageRules
									, ulUsageRulesLength);

	if (HLIB_RPC_Join (nRpcHandle, nReqId, 10000) == ERR_FAIL)
	{
		HxLOG_Error("[%s:%d] - HLIB_RPC_Join : error to join  = %d \n", __FUNCTION__, __LINE__, err);
		return ERR_FAIL;
	}

	if (err == TRUE)
	{
		return ERR_OK;
	}
	else
	{
		HxLOG_Error("[%s:%d] --- fail = %d \n", __FUNCTION__, __LINE__, err);
		return ERR_FAIL;
	}

	return hErr;
}
#endif


