/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  om_sysinfo_tuner.c
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
* MS949 encoding \uc744 \uc0ac\uc6a9\ud558\ub294 source file \uc785\ub2c8\ub2e4.
* MS949 encoding\u3092\u4f7f\u7528\u3057\u3066 source file\u3067\u3059\u3002
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
#include <mgr_action.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define TUNER_THRESHOLD_TIME	(1500)	// 1.5sec

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static OxSysInfoTuner_Info_t		*s_pstSysInfoTunerInfo = NULL;
static OxSysInfoTuner_Info_t		*s_pstSysInfoTunerOldInfo = NULL;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
STATIC void		om_sysinfo_tuner_CheckUnlockedChange(HBOOL bChangeCheck)
{
	HINT32			i;
	HINT32			nNumOfTunerGroup;
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;
	OxSysInfoTuner_Info_t	*pstTunerInfo = s_pstSysInfoTunerInfo;

	if (pstTunerInfo == NULL)
	{
		return;
	}

	SVC_CH_GetNumberOfTunerGroup(&nNumOfTunerGroup);
	nNumOfTunerGroup = (nNumOfTunerGroup > NUM_OAPI_TUNER) ? NUM_OAPI_TUNER : nNumOfTunerGroup;

	for (i = 0; i < nNumOfTunerGroup; i++)
	{
		DxDeliveryType_e			eTunerType;
		DxTuneParam_t			utTuneInfo;
		OxSysInfoTuner_Status_t	*pstInfo = &(pstTunerInfo->astTunerStatus[i]);

		// check locked -> unlocked (do not consider unlocked case)
		if (pstInfo->bLocked != TRUE)
		{
			continue;
		}

		SVC_CH_GetTunerType(i, &eTunerType);
		if (SVC_CH_GetLockedInfoByTunerGroupID(i, eTunerType, &utTuneInfo) != ERR_OK)
		{
			// locked -> unlocked
			HxSTD_MemSet(pstInfo, 0, sizeof(OxSysInfoTuner_Status_t));
			HxLOG_Info("[Tuner(%d) locked -> not locked\n",i);
		}
		else
		{
			// locked -> locked
			// do not change anything
			HxLOG_Info("[Tuner(%d) locked -> locked\n",i);
		}

	}
	pstTunerInfo->nNoOfTunerGroup = nNumOfTunerGroup;
	//	TODO: set service triple id for specific tuner group. (JAPAN)

	//	TODO: Implement code for Analog tuner...

	if ((HxSTD_MemCmp(s_pstSysInfoTunerOldInfo, s_pstSysInfoTunerInfo, sizeof(OxSysInfoTuner_Info_t)) == 0) && bChangeCheck)
		return;

	//	Notify event.
	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr == ERR_OK)
	{
		HxSTD_MemCopy(s_pstSysInfoTunerOldInfo, s_pstSysInfoTunerInfo, sizeof(OxSysInfoTuner_Info_t));
		HLIB_RPC_Notify(nRpcHandle, RPC_OBAMA_SYSINFO_TUNER_onStatusChanged, "b", pstTunerInfo, sizeof(OxSysInfoTuner_Info_t));
	}
}

STATIC void		om_sysinfo_tuner_RefreshStatus(HBOOL bChangeCheck)
{
	HINT32	i;
	HINT32	nNumOfTunerGroup;
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;
	OxSysInfoTuner_Info_t	*pstTunerInfo = s_pstSysInfoTunerInfo;
	HUINT32					 ulCurTick;

	SVC_CH_GetNumberOfTunerGroup(&nNumOfTunerGroup);
	nNumOfTunerGroup = (nNumOfTunerGroup > NUM_OAPI_TUNER) ? NUM_OAPI_TUNER : nNumOfTunerGroup;

	for (i = 0; i < nNumOfTunerGroup; i++)
	{
		DxDeliveryType_e		eTunerType;
		DxTuneParam_t			utTuneInfo;
		CH_SignalInfo_t			stSignalInfo;
		OxSysInfoTuner_Status_t	*pstInfo = &(pstTunerInfo->astTunerStatus[i]);

		SVC_CH_GetTunerType(i, &eTunerType);
		//	if not locked ?? how can we do?
		if (SVC_CH_GetLockedInfoByTunerGroupID(i, eTunerType, &utTuneInfo) == ERR_OK)
		{
			ulCurTick = HLIB_STD_GetSystemTick();

			if (bChangeCheck)
			{
				if ((pstInfo->ulLastCheckTick + TUNER_THRESHOLD_TIME) > ulCurTick)
				{
	 				// SVC_CH_GetSignalInfoByTunerGroupID()에서 약 250ms 소요되므로 짧은 시간내에 연속으로 들어오는 요청은 저장된 값을 이용해 보내준다
					continue;
				}
			}

			// clear previous status
			HxSTD_MemSet(pstInfo, 0, sizeof(OxSysInfoTuner_Status_t));
			pstInfo->ulLastCheckTick = ulCurTick;

			SVC_CH_GetSignalInfoByTunerGroupID(i, &eTunerType, &stSignalInfo);

			pstInfo->eTunerType     = eTunerType;
			pstInfo->bLocked        = TRUE;
			pstInfo->nSignalLevel   = stSignalInfo.signalLevel;
			pstInfo->nSignalQuality = stSignalInfo.signalQuality;
			pstInfo->nAgc           = stSignalInfo.ulAgc;
			pstInfo->fBer           = stSignalInfo.fBer;
			pstInfo->fSnr           = stSignalInfo.fSnr;
			pstInfo->fInputPower    = stSignalInfo.fSignalInputPower;

			//	copy utTuneInfo and stSignalInfo to OxSysInfoTuner_Status_t
			switch (eTunerType)
			{
				case eDxDELIVERY_TYPE_SAT:
					HxSTD_MemCopy(&pstInfo->utInfo.stSatInfo, &utTuneInfo.sat, sizeof(DxSatTuningInfo_t));
					break;
				case eDxDELIVERY_TYPE_CAB:
					HxSTD_MemCopy(&pstInfo->utInfo.stCabInfo, &utTuneInfo.cab, sizeof(DxCabTuningInfo_t));
					break;
				case eDxDELIVERY_TYPE_TER:
					HxSTD_MemCopy(&pstInfo->utInfo.stTerInfo, &utTuneInfo.ter, sizeof(DxTerTuningInfo_t));
					break;
				case eDxDELIVERY_TYPE_MULTICAST:
				default:
					HxLOG_Warning("not supported delivery type [%d]\n", eTunerType);
					break;
			}

			HxLOG_Debug("[Tuner(%d) locked - level(%d), quality(%d)\n", i, stSignalInfo.signalLevel, stSignalInfo.signalQuality);
		}
		else
		{
			HxSTD_MemSet(pstInfo, 0, sizeof(OxSysInfoTuner_Status_t));
			HxLOG_Info("[Tuner(%d) not locked\n",i);
		}
	}
	pstTunerInfo->nNoOfTunerGroup = nNumOfTunerGroup;
	//	TODO: set service triple id for specific tuner group. (JAPAN)

	//	TODO: Implement code for Analog tuner...

	if ((HxSTD_MemCmp(s_pstSysInfoTunerOldInfo, s_pstSysInfoTunerInfo, sizeof(OxSysInfoTuner_Info_t)) == 0) && bChangeCheck)
		return;

	//	Notify event.
	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);

	if (hErr == ERR_OK)
	{
		HxSTD_MemCopy(s_pstSysInfoTunerOldInfo, s_pstSysInfoTunerInfo, sizeof(OxSysInfoTuner_Info_t));
		HLIB_RPC_Notify(nRpcHandle, RPC_OBAMA_SYSINFO_TUNER_onStatusChanged, "b", pstTunerInfo, sizeof(OxSysInfoTuner_Info_t));
	}
}

#ifdef CONFIG_DEBUG
STATIC HCHAR*	om_sysinfo_tuner_PrintChEvt(HINT32 nChEvt)
{
	switch(nChEvt)
	{
	ENUM_TO_STR(eSEVT_CH_LOCKED);
	ENUM_TO_STR(eSEVT_CH_NO_SIGNAL);
	ENUM_TO_STR(eSEVT_CH_MOVING_MOTOR);
	ENUM_TO_STR(eSEVT_CH_ANTENNA_OK);
	ENUM_TO_STR(eSEVT_CH_ANTENNA_FAULT);
	default:
		return "Unknown";
	}
}
#endif

STATIC BUS_Result_t		proc_om_sysinfo_tuner(HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch (message)
	{
		case eMEVT_BUS_CREATE:
			BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
			break;
		case eMEVT_ACTION_NOTIFY_RELEASED:
			om_sysinfo_tuner_CheckUnlockedChange(TRUE);
			break;
		case eSEVT_CH_STOPPED:
		case eSEVT_CH_LOCKED:
		case eSEVT_CH_NO_SIGNAL:
		case eSEVT_CH_MOVING_MOTOR:
		case eSEVT_CH_ANTENNA_OK:
		case eSEVT_CH_ANTENNA_FAULT:
#ifdef CONFIG_DEBUG
			HxLOG_Print(HxANSI_COLOR_YELLOW("%s, call om_sysinfo_tuner_RefreshStatus()\n"), om_sysinfo_tuner_PrintChEvt(message));
#endif
			om_sysinfo_tuner_RefreshStatus(FALSE);
			break;
		default:
			break;
	}

	return MESSAGE_PASS;
}


//	nArgc == 0
STATIC HERROR om_sysinfo_tuner_Init(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HxLOG_Assert(nArgc == 0);

	om_sysinfo_tuner_RefreshStatus(FALSE);

	return ERR_OK;
}

STATIC HERROR om_sysinfo_tuner_Refresh(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	om_sysinfo_tuner_RefreshStatus(TRUE);
	return ERR_OK;
}


HERROR	OM_SYSINFO_TUNER_Init(void)
{
	HERROR	err;
	HINT32	nRpcHandle = 0;

	s_pstSysInfoTunerInfo = (OxSysInfoTuner_Info_t*)OxMGR_Calloc(sizeof(OxSysInfoTuner_Info_t));
	s_pstSysInfoTunerOldInfo = (OxSysInfoTuner_Info_t*)OxMGR_Calloc(sizeof(OxSysInfoTuner_Info_t));

	BUS_MGR_Create(NULL, APP_ACTION_PRIORITY, proc_om_sysinfo_tuner, 0, 0, 0, 0);

	err = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if(err == ERR_OK)
	{
		err |= HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_SYSINFO_TUNER_Init, NULL, (HxRPC_Func_t)om_sysinfo_tuner_Init, NULL,
								"Activate the sysinfo.tuner: (void)");
		err |= HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_SYSINFO_TUNER_Refresh, NULL, (HxRPC_Func_t)om_sysinfo_tuner_Refresh, NULL,
								"Refresh sysinfo.tuner : (void)");

		err |= HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_SYSINFO_TUNER_onStatusChanged, "b",
				"notifying when Tuner information is changed : (OxSysInfoTuner_Info_t *pstTunerInfos)");

	}


	return err;
}
