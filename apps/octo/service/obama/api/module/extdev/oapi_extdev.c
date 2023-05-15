/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_exe.c
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
#include <oapi_extdev.h>

#include "../../oapi_mem.h"

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

static	HBOOL	s_bMicStatus = FALSE;
static 	HINT32	s_nMicVolume = 100;	// default : 100

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___

#define _____NOTIFICATION_CALLBACKS_____
STATIC void oapi_extdev_CbOnVolumeChanged(HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	HxLOG_Trace();

	if (argc == 1)
	{
		HINT32 nMicVolume = 0;

		nMicVolume = (HINT32)HxOBJECT_INT(argv[0]);
		HxLOG_Info("Microphone Volume Changed !!! (%d -> %d\n", s_nMicVolume, nMicVolume);
		s_nMicVolume = nMicVolume;
	}
}

#define ___GLOBAL_FUNCTIONS___

HERROR	OAPI_EXTDEV_Init(void)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	if (hErr == ERR_OK)
	{
		HLIB_RPC_RegisterNotifyCallback (nRpcHandle, RPC_OBAMA_EXTDEV_MIC_onVolumeChanged, "i", NULL, (HxRPC_Noti_t)oapi_extdev_CbOnVolumeChanged);
		HLIB_RPC_Call (nRpcHandle, RPC_OAPI_EXTDEV_MIC_REQINIT, NULL);
	}

	return hErr;
}

HERROR	OAPI_EXTDEV_MIC_Start(void)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	HxLOG_Trace();
	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_EXTDEV_MIC_START, NULL);

	// 현재 값으로 Volume init
	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_EXTDEV_MIC_SETVOLUME, "i", s_nMicVolume);

	s_bMicStatus = TRUE;
	return hErr;
}

HERROR	OAPI_EXTDEV_MIC_Stop(void)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	HxLOG_Trace();
	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_EXTDEV_MIC_STOP, NULL);

	s_bMicStatus = FALSE;
	return hErr;
}

HERROR	OAPI_EXTDEV_MIC_SetVolume(HINT32 nValue)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	HxLOG_Trace();

	HxLOG_Debug("nValue : %d\n", nValue);
	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_EXTDEV_MIC_SETVOLUME, "i", nValue);

	//s_nMicVolume = nValue;

	return hErr;
}


HERROR	OAPI_EXTDEV_MIC_GetVolume(HINT32 *pnValue)
{
	if (pnValue)
	{
		*pnValue = s_nMicVolume;
	}
	return ERR_OK;
}

HERROR	OAPI_EXTDEV_MIC_GetStatus(HBOOL *pbMicStatus)
{
	if (pbMicStatus)
	{
		*pbMicStatus = s_bMicStatus;
	}
	return ERR_OK;
}


