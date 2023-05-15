/*
 * (c) 2011-2012 Humax Co., Ltd.
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
/**
	@file     va_dscr_adapt.c
	@brief    va_dscr_adapt.c (Viaccess descrambler)

	Description: . descrambler adaptation layer \n
	Module: MW/CAS/ VA \n
	Remarks :\n

	Copyright (c) 2009 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "va_def.h"
#include "vamgr_main.h"

//#include "cxcore_os.h"
#include "va_dscr_adapt.h"
#include "vkernel.h"
//#include "conax.h"

#include "pal_dsc.h"
#include <pal_pipe.h>

#if defined(CONFIG_PROD_CPU_BCM) && defined(CONFIG_MW_CAS_VIACCESS_ENCRYPTED_CW)
#include "va_schip_api.h"
#endif

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


/********************************************************************
*	External variables & function prototypes
********************************************************************/

/********************************************************************
*	Static variables
********************************************************************/

/********************************************************************
*	Static functions Prototypes
*	The following functions will be used as inner functions
********************************************************************/

HERROR VA_DSCR_AdaptInit(void)
{
	HERROR nErr = ERR_OK;

	HxLOG_Print("[Enter]\n");

#if 0 // hmkim : commented out (OBAMA_Init() 에서 이미 호출됨)
	// 1. Descrambler를 init한다.
	nErr = PAL_DSC_Init();
#endif

	HxLOG_Print("[Exit]\n");

	return nErr;
}

HERROR VA_DSCR_AdaptOpen(Handle_t hVaAction, HUINT32 *pulDscrHandle, HUINT16 usPid)
{
	HERROR 			nErr;
	HUINT32	ulDemuxId;
	Handle_t		hAction;

	HxLOG_Print("[Enter][%x]\n", usPid);

	if (pulDscrHandle == NULL)
	{
		HxLOG_Error("pulDscrHandle is NULL\n");
		return ERR_FAIL;
	}

	hAction = VAMGR_GetActionHandle(hVaAction);
	HxLOG_Info("Action[%x], Pid[%x]\n", hAction, usPid);

	nErr = svc_cas_SubSvcRmGetResourceId(svc_cas_SubSvcRmGetActionId(hAction), eRxRSCTYPE_DEMUX, &ulDemuxId);
	if (nErr != ERR_OK)
	{
		HxLOG_Error("svc_cas_SubSvcRmGetResourceId(%x)\n", hAction);
		return nErr;
	}

	// ePAL_DSC_DATATYPE_PSIPES --> 이거 별 의미 없어 보임..
	nErr = PAL_DSC_Open(ulDemuxId, ePAL_DSC_DSCTYPE_DVB, ePAL_DSC_DATATYPE_VIDEOPES, pulDscrHandle);
	if (nErr != ERR_OK)
	{
		HxLOG_Error("svc_cas_DevDscOpen(%x)\n", usPid);
		return ERR_FAIL;
	}

	HxLOG_Print("[Exit][%x]\n", *pulDscrHandle);
	return ERR_OK;
}

HERROR VA_DSCR_AdaptSetPid(HUINT32 ulDscrHandle, HUINT16 usPid)
{
	HERROR nErr;

	HxLOG_Print("[Enter][%x]\n", usPid);

	nErr = PAL_DSC_SetPid(ulDscrHandle, usPid);
	if (nErr != ERR_OK)
	{
		HxLOG_Error("svc_cas_DevDscSetPid(%x)\n", usPid);
		return ERR_FAIL;
	}

	HxLOG_Print("[Exit]\n");
	return ERR_OK;
}

HERROR VA_DSCR_AdaptResetPid(HUINT32 ulDscrHandle, HUINT16 usPid, HBOOL bClearKey)
{
	HERROR nErr;

	HxLOG_Print("[Enter][%x]\n", usPid);

	if (bClearKey) // see VA_DSCR_AdaptClose()
	{
		HUINT8 aucNUllCW[8];

		VA_memset(aucNUllCW, 0, 8);

#if defined(CONFIG_PROD_CPU_BCM)&& defined(CONFIG_MW_CAS_VIACCESS_ENCRYPTED_CW)
	 	VA_SCHIP_SetProtectKey(ulDscrHandle);
#endif

		nErr = PAL_DSC_SetKey(ulDscrHandle, ePAL_DSC_KEYPARITY_EVEN, aucNUllCW);
		if (nErr != ERR_OK)
		{
			HxLOG_Error("svc_cas_DevDscSetKey(%x,%x,even)\n", ulDscrHandle, usPid);
			return ERR_FAIL;
		}
		nErr = PAL_DSC_SetKey(ulDscrHandle, ePAL_DSC_KEYPARITY_ODD, aucNUllCW);
		if (nErr != ERR_OK)
		{
			HxLOG_Error("svc_cas_DevDscSetKey(%x,%x,odd)\n", ulDscrHandle, usPid);
			return ERR_FAIL;
		}
	}

	nErr = PAL_DSC_ResetPid(ulDscrHandle, usPid);
	if (nErr != ERR_OK)
	{
		HxLOG_Error("svc_cas_DevDscResetPid(%x)\n", usPid);
		return ERR_FAIL;
	}

	HxLOG_Print("[Exit]\n");
	return ERR_OK;
}

HERROR VA_DSCR_AdaptClose(HUINT32 ulDscrHandle, HUINT16 usPid)
{
	HERROR 	nErr;
	HUINT8 aucNUllCW[8];

	HxLOG_Print("[Enter][%x,%x]\n", ulDscrHandle, usPid);

	VA_memset(aucNUllCW, 0, 8);

#if defined(CONFIG_PROD_CPU_BCM) && defined(CONFIG_MW_CAS_VIACCESS_ENCRYPTED_CW)
 	VA_SCHIP_SetProtectKey(ulDscrHandle);
#endif

	// Maturity rating을 설정한 channel에서 pincode입력을 해서
	//				CW를 얻어 설정한 상태에서 다른 channel에 갔다가 돌아왔을 때 아래 code가
	//				없으면 이전에 설정된 CW로 인해 descramble이됨.
	// =-> decrambler link를 끊는 경우 Key 값도 초기화 해주어야 함.
	nErr = PAL_DSC_SetKey(ulDscrHandle, ePAL_DSC_KEYPARITY_EVEN, aucNUllCW);
	if (nErr != ERR_OK)
	{
		HxLOG_Error("svc_cas_DevDscSetKey(%x,%x,even)\n", ulDscrHandle, usPid);
		return ERR_FAIL;
	}
	nErr = PAL_DSC_SetKey(ulDscrHandle, ePAL_DSC_KEYPARITY_ODD, aucNUllCW);
	if (nErr != ERR_OK)
	{
		HxLOG_Error("svc_cas_DevDscSetKey(%x,%x,odd)\n", ulDscrHandle, usPid);
		return ERR_FAIL;
	}

	nErr = PAL_DSC_Close(ulDscrHandle);
	if (nErr != ERR_OK)
	{
		HxLOG_Error("svc_cas_DevDscClose(%x,%x)\n", ulDscrHandle, usPid);
		return ERR_FAIL;
	}

	HxLOG_Print("[Exit]\n");
	return ERR_OK;
}

HERROR VA_DSCR_AdaptSetKey(HUINT32 ulDscrHandle, HUINT8 ucType, HUINT8 *pucCW)
{
	HERROR			 	nErr;
	PalDsc_KeyParity_e	eParity;

	HxLOG_Print("[Enter][%x]\n", ulDscrHandle);

	if (ucType == 1)
	{
		eParity = ePAL_DSC_KEYPARITY_ODD;
	}
	else
	{
		eParity = ePAL_DSC_KEYPARITY_EVEN;
	}

	HxLOG_Print("SetKey(%d), %02X %02X %02X %02X %02X %02X %02X %02X\n", eParity, pucCW[0], pucCW[1], pucCW[2], pucCW[3], pucCW[4], pucCW[5], pucCW[6], pucCW[7]);

	nErr = PAL_DSC_SetKey(ulDscrHandle, eParity, pucCW);
	if (nErr != ERR_OK)
	{
		HxLOG_Error("svc_cas_DevDscSetKey(%x,%d)\n", ulDscrHandle, eParity);
		return ERR_FAIL;
	}

	HxLOG_Print("[Exit]\n");
	return ERR_OK;
}

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
HBOOL VA_DSCR_Adapt_IsPlayback(Handle_t hVaAction)
{
	SvcCas_ActionType_e eType;

	eType = VAMGR_GetActType(hVaAction);
	if (eType == eCAS_ActionType_Playback || eType == eCAS_ActionType_Tsr_Play)
		return TRUE;

	return FALSE;
}
#endif

