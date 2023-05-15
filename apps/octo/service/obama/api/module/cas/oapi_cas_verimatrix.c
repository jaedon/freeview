/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_cas_verimatrix.c
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
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
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

#define	ENTER_CAS_CACHE	VK_SEM_Get(s_ulOapiCasSemId)
#define	LEAVE_CAS_CACHE	VK_SEM_Release(s_ulOapiCasSemId)

#define CHECK_INIT	{if(s_bCasVerimatrixInit != TRUE) return ERR_FAIL;}

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HULONG						s_ulOapiCasSemId;
STATIC HBOOL						s_bCasVerimatrixInit;
STATIC OxCasVerimatrixStatus_t		s_stOapiCasVerimatrixStatus;
STATIC OxCasVerimatrix_Notifier_t	s_pfCasVerimatrixApkNotifier;



/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___


#define _____NOTIFICATION_CALLBACKS_____

STATIC void oapi_cas_verimatrix_onInited(HERROR hErr, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	if(argc != 1)
	{
		HxLOG_Error("==========> ERROR !!!! %s() has invalid argc[%d]\n\n", __FUNCTION__, argc);
		return;
	}
	else if(HxOBJECT_BIN_SIZE(argv[0]) != sizeof(OxCasVerimatrixStatus_t))
	{
		HxLOG_Error("==========> ERROR !!!! %s() has invalid struct size [%d]\n\n", __FUNCTION__, HxOBJECT_BIN_SIZE(argv[0]));
		return;
	}
	else
	{
		s_stOapiCasVerimatrixStatus = *(OxCasVerimatrixStatus_t *)HxOBJECT_BIN_DATA(argv[0]);
		HxLOG_Debug("OAPI_CAS_VERIMATRIX status cache refreshed. Good to go ==============>\n");
	}
}
STATIC void oapi_cas_verimatrix_onStatusChanged(HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	if(argc != 1)
	{
		HxLOG_Error("==========> ERROR !!!! %s() has invalid argc[%d]\n\n", __FUNCTION__, argc);
		return;
	}
	else if(HxOBJECT_BIN_SIZE(argv[0]) != sizeof(OxCasVerimatrixStatus_t))
	{
		HxLOG_Error("==========> ERROR !!!! %s() has invalid struct size [%d]\n\n", __FUNCTION__, HxOBJECT_BIN_SIZE(argv[0]));
		return;
	}
	else
	{
		ENTER_CAS_CACHE;
		s_stOapiCasVerimatrixStatus = *(OxCasVerimatrixStatus_t *)HxOBJECT_BIN_DATA(argv[0]);
		LEAVE_CAS_CACHE;

		if(s_pfCasVerimatrixApkNotifier != NULL)
		{
			(s_pfCasVerimatrixApkNotifier)();
		}
	}

	return;
}

STATIC void oapi_cas_verimatrix_onWebVersionChanged(HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	if(argc != 1)
	{
		HxLOG_Error("==========> ERROR !!!! %s() has invalid argc[%d]\n\n", __FUNCTION__, argc);
		return;
	}
	else
	{
		ENTER_CAS_CACHE;
		strncpy(s_stOapiCasVerimatrixStatus.szWebVersion,(char*)HxOBJECT_STR(argv[0]), OAPI_CAS_VERIMATRIX_VERSION_LENGTH);
		LEAVE_CAS_CACHE;
	}
}


#define ___GLOBAL_FUNCTIONS___

HERROR OAPI_CAS_VERIMATRIX_Init(OxCasVerimatrix_Notifier_t pfApkListener)
{
#if defined(CONFIG_MW_CAS_VERIMATRIX)
	HINT32	nReqId;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_RpcProtocol_GetRpcHandle() failed..! \n");
	}

	HxLOG_Debug("%s()\n", __FUNCTION__);

	if (s_bCasVerimatrixInit != TRUE)
	{
		if ( VK_SEM_Create(&s_ulOapiCasSemId, "s_ulOapiCasSemId", VK_SUSPENDTYPE_PRIORITY) != VK_OK )
		{
			HxLOG_Error("VK_SEM_Create error! \n");
		}

		HLIB_RPC_RegisterNotifyCallback (nRpcHandle, RPC_OBAMA_CAS_VERIMATRIX_onStatusChanged, "b", NULL, (HxRPC_Noti_t)oapi_cas_verimatrix_onStatusChanged);
		HLIB_RPC_RegisterNotifyCallback (nRpcHandle, RPC_OBAMA_CAS_VERIMATRIX_onWebVersionChanged ,"s",	NULL, oapi_cas_verimatrix_onWebVersionChanged);
		nReqId = HLIB_RPC_Request (nRpcHandle, (HxRPC_Response_t)oapi_cas_verimatrix_onInited, NULL, RPC_OAPI_CAS_VERIMATRIX_Init, NULL);
		HxLOG_Print("nReqId(%d)", nReqId);

		HLIB_RPC_Call(nRpcHandle, RPC_OAPI_CAS_VERIMATRIX_RequestWebVersion,NULL);

		s_bCasVerimatrixInit = TRUE;
		s_pfCasVerimatrixApkNotifier = pfApkListener;
	}
	else
	{
		HxLOG_Error("==========> ERROR !!!! %s() has already been initialized !!!!\n\n", __FUNCTION__);
	}
#endif

	return ERR_OK;
}




HERROR OAPI_CAS_VERIMATRIX_GetStatus(OxCasVerimatrixStatus_t *pstStatus)
{
	HxLOG_Trace();
	HxLOG_Assert(pstStatus);

	CHECK_INIT;

	ENTER_CAS_CACHE;
	*pstStatus = s_stOapiCasVerimatrixStatus;
	LEAVE_CAS_CACHE;

	return ERR_OK;
}


/* end of file */
