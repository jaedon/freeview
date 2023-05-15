/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_sysinfo_tuner.c
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
#define	ENTER_SYSINFO_TUNER_CACHE		HxSEMT_Get(s_ulSysInfoTunerSemId)
#define	LEAVE_SYSINFO_TUNER_CACHE		HxSEMT_Release(s_ulSysInfoTunerSemId)

#define OAPI_TUNER_THRESHOLD_TIME	(2000)	// 1.5sec
/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC OxSysInfoTuner_Info_t		*s_pstTunerInfo = NULL;
STATIC OxSysInfoTuner_Notifier_t	s_pfnTunerStatusChanged = NULL;
STATIC HUINT32					s_ulSysInfoTunerSemId = 0;
STATIC HUINT32 					s_ulLastTick = 0;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___


#define _____NOTIFICATION_CALLBACKS_____

//	apArgv[0] == 'b' OxSysInfoTuner_Info_t*
STATIC void oapi_sysinfo_tuner_onStatusChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *userdata)
{
	OxSysInfoTuner_Info_t	*pstTunerInfo;

	if (nArgc != 1)
	{
		HxLOG_Error("invalid nArgc = [%d]\n", nArgc);
		return;
	}

	if (HxOBJECT_BIN_SIZE(apArgv[0]) != sizeof(OxSysInfoTuner_Info_t))
	{
		HxLOG_Error("invalid data size [%d : %d]\n", HxOBJECT_BIN_SIZE(apArgv[0]), sizeof(OxSysInfoTuner_Info_t));
		return;
	}

	ENTER_SYSINFO_TUNER_CACHE;
	pstTunerInfo = (OxSysInfoTuner_Info_t*)HxOBJECT_BIN_DATA(apArgv[0]);
	HxSTD_MemCopy(s_pstTunerInfo, pstTunerInfo, sizeof(OxSysInfoTuner_Info_t));
	LEAVE_SYSINFO_TUNER_CACHE;

	if (s_pfnTunerStatusChanged)
		(s_pfnTunerStatusChanged)();
}

STATIC void	oapi_sysinfo_tuner_Refresh(void)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;
	HUINT32 ulCurTick = HLIB_STD_GetSystemTick();

	if ((s_ulLastTick + OAPI_TUNER_THRESHOLD_TIME) > ulCurTick)
	{
		// Skip reflesh under 1.5 sec
		return;
	}

	s_ulLastTick = ulCurTick;
	HxLOG_Warning("RPC CAll [RPC_OAPI_SYSINFO_TUNER_Refresh]\n");

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr == ERR_OK)
	{
		HLIB_RPC_Call (nRpcHandle, RPC_OAPI_SYSINFO_TUNER_Refresh, NULL);
	}
}

#if defined(CONFIG_DEBUG)
STATIC HINT32 oapi_sysinfo_tuner_DebugPrint (void *pvArg)
{
	HUINT32 i;

	if (s_pstTunerInfo == NULL)
	{
		return HxCMD_ERR;
	}

	HLIB_CMD_Printf("Print tuner info \n");

	for (i=0; i<s_pstTunerInfo->nNoOfTunerGroup; i++)
	{
		HLIB_CMD_Printf("(%d/%d) tuner locked(%d) \n", i,s_pstTunerInfo->nNoOfTunerGroup,s_pstTunerInfo->astTunerStatus[i].bLocked);
	}

	return HxCMD_OK;
}

STATIC HINT32 oapi_sysinfo_tuner_DebugRefresh (void *pvArg)
{
	HLIB_CMD_Printf("Refresh tuner info \n");

	oapi_sysinfo_tuner_Refresh();

	return HxCMD_OK;
}
#endif

#define ___GLOBAL_FUNCTIONS___

HERROR	OAPI_SYSINFO_TUNER_Init(OxSysInfoTuner_Notifier_t pfnNotifier)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	s_pstTunerInfo = (OxSysInfoTuner_Info_t*)OxMEM_Calloc(sizeof(OxSysInfoTuner_Info_t));

	HxSEMT_Create(&s_ulSysInfoTunerSemId, "sysinfo_tuner", HxSEMT_FIFO);
	HLIB_RPC_RegisterNotifyCallback (nRpcHandle, RPC_OBAMA_SYSINFO_TUNER_onStatusChanged, "b", NULL, (HxRPC_Noti_t)oapi_sysinfo_tuner_onStatusChanged);
	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_SYSINFO_TUNER_Init, NULL);

	s_pfnTunerStatusChanged = pfnNotifier;

#if defined (CONFIG_DEBUG)
#define hCmdHandle "oapi"
	HLIB_CMD_RegisterWord(hCmdHandle, oapi_sysinfo_tuner_DebugPrint,
						(HCHAR *)"print_tuner",
						(HCHAR *)"print_tuner",
						(HCHAR *)"print_tuner");

	HLIB_CMD_RegisterWord(hCmdHandle, oapi_sysinfo_tuner_DebugRefresh,
						(HCHAR *)"refresh_tuner",
						(HCHAR *)"refresh_tuner",
						(HCHAR *)"refresh_tuner");
#endif

	return ERR_OK;
}

HERROR	OAPI_SYSINFO_TUNER_DeInit(void)
{
	//	TODO:
	return ERR_OK;
}

HERROR	OAPI_SYSINFO_TUNER_GetDeviceNum(HUINT32 *pulDevNum)
{
	ENTER_SYSINFO_TUNER_CACHE;
	*pulDevNum = s_pstTunerInfo->nNoOfTunerGroup;
	LEAVE_SYSINFO_TUNER_CACHE;

	return ERR_OK;
}

HERROR	OAPI_SYSINFO_TUNER_GetDevType(HINT32 nIndex, DxDeliveryType_e *peDeliType)
{
	ENTER_SYSINFO_TUNER_CACHE;

	HxLOG_Assert(nIndex < s_pstTunerInfo->nNoOfTunerGroup);
	*peDeliType = s_pstTunerInfo->astTunerStatus[nIndex].eTunerType;
	LEAVE_SYSINFO_TUNER_CACHE;

	return ERR_OK;
}

HERROR	OAPI_SYSINFO_TUNER_IsLocked(HINT32 nIndex, HBOOL *pbLocked)
{
	ENTER_SYSINFO_TUNER_CACHE;

	HxLOG_Assert(nIndex < s_pstTunerInfo->nNoOfTunerGroup);
	*pbLocked = s_pstTunerInfo->astTunerStatus[nIndex].bLocked;
	LEAVE_SYSINFO_TUNER_CACHE;

	oapi_sysinfo_tuner_Refresh();

	return ERR_OK;
}

HERROR	OAPI_SYSINFO_TUNER_GetSignalInfo(HINT32 nIndex, OxSysInfoTuner_SignalInfo_t *pstSignalInfo)
{
	ENTER_SYSINFO_TUNER_CACHE;

	HxLOG_Assert(nIndex < s_pstTunerInfo->nNoOfTunerGroup);

	pstSignalInfo->nSignalLevel   = s_pstTunerInfo->astTunerStatus[nIndex].nSignalLevel;
	pstSignalInfo->nSignalQuality = s_pstTunerInfo->astTunerStatus[nIndex].nSignalQuality;
	pstSignalInfo->nAgc           = s_pstTunerInfo->astTunerStatus[nIndex].nAgc;
	pstSignalInfo->fBer           = s_pstTunerInfo->astTunerStatus[nIndex].fBer;
	pstSignalInfo->fSnr           = s_pstTunerInfo->astTunerStatus[nIndex].fSnr;
	pstSignalInfo->fInputPower    = s_pstTunerInfo->astTunerStatus[nIndex].fInputPower;

	LEAVE_SYSINFO_TUNER_CACHE;

	oapi_sysinfo_tuner_Refresh();

	return ERR_OK;
}

HERROR	OAPI_SYSINFO_TUNER_GetTuneParam(HINT32 nIndex, DxTuneParam_t *pstTuneParam)
{
	ENTER_SYSINFO_TUNER_CACHE;

	HxLOG_Assert(nIndex < s_pstTunerInfo->nNoOfTunerGroup);
	switch (s_pstTunerInfo->astTunerStatus[nIndex].eTunerType)
	{
		case eDxDELIVERY_TYPE_SAT:
			HxSTD_MemCopy(&pstTuneParam->sat, &s_pstTunerInfo->astTunerStatus[nIndex].utInfo.stSatInfo, sizeof(DxSatTuningInfo_t));
			break;
		case eDxDELIVERY_TYPE_CAB:
			HxSTD_MemCopy(&pstTuneParam->cab, &s_pstTunerInfo->astTunerStatus[nIndex].utInfo.stCabInfo, sizeof(DxCabTuningInfo_t));
			break;
		case eDxDELIVERY_TYPE_TER:
			HxSTD_MemCopy(&pstTuneParam->ter, &s_pstTunerInfo->astTunerStatus[nIndex].utInfo.stTerInfo, sizeof(DxTerTuningInfo_t));
			break;
		default:
			HxLOG_Error("not supported delivery type!\n");
			break;
	}

	LEAVE_SYSINFO_TUNER_CACHE;
	oapi_sysinfo_tuner_Refresh();

	return ERR_OK;
}

HERROR	OAPI_SYSINFO_TUNER_GetTunerIdByTripleId(HINT32 nOnId, HINT32 nTsId, HINT32 nSvcId, HINT32 *pnTunerId)
{
	HxLOG_Error("not implemented!!\n");

	return ERR_OK;
}

