/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_sysinfo_stb.c
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
* MS949 encoding ?¬ª ¬ª√ß¬ø√´√á√è¬¥√Ç source file ?√î¬¥√è¬¥√ô.
* MS949 encoding¬™√≤√û√Ö√©√Ñ¬™¬∑¬™√Ü source file¬™√á¬™¬π¬°¬£
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
#define	ENTER_SYSINFO_STB_CACHE		HxSEMT_Get(s_ulSysInfoStbSemId)
#define	LEAVE_SYSINFO_STB_CACHE		HxSEMT_Release(s_ulSysInfoStbSemId)

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC OxSysInfoStb_Info_t		*s_pstStbInfo = NULL;
STATIC OxSysInfoStb_Notifier_t	s_pfnStbStatusChanged = NULL;
STATIC HUINT32					s_ulSysInfoStbSemId = 0;
STATIC HINT32					s_nRpcHandle = 0x0;
STATIC HBOOL					s_bInitializedStbInfo = FALSE;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___


#define _____NOTIFICATION_CALLBACKS_____
//	apArgv[0] == 'b' OxSysInfoStb_Info_t*
STATIC void oapi_sysinfo_stb_onStatusChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *userdata)
{
	OxSysInfoStb_Info_t	*pstStbInfo;

	if (nArgc != 1)
	{
		HxLOG_Error("invalid nArgc = [%d] nRequestId=[%d]\n", nArgc, nRequestId);
		return;
	}

	if (HxOBJECT_BIN_SIZE(apArgv[0]) != sizeof(OxSysInfoStb_Info_t))
	{
		HxLOG_Error("invalid data size [%d : %d]\n", HxOBJECT_BIN_SIZE(apArgv[0]), sizeof(OxSysInfoStb_Info_t));
		return;
	}

	ENTER_SYSINFO_STB_CACHE;
	pstStbInfo = (OxSysInfoStb_Info_t*)HxOBJECT_BIN_DATA(apArgv[0]);
	HxSTD_MemCopy(s_pstStbInfo, pstStbInfo, sizeof(OxSysInfoStb_Info_t));
	s_bInitializedStbInfo = TRUE;
	LEAVE_SYSINFO_STB_CACHE;

	if (s_pfnStbStatusChanged)
		(s_pfnStbStatusChanged)();
}

#define _____RESPONSE_CALLBACKS_____
STATIC void oapi_sysinfo_stb_onStatusChanged_as_response(HERROR hErr, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *userdata)
{
	if(ERR_OK != hErr)
	{
		HxLOG_Error("Error = [%d] nArgc = [%d] nRequestId=[%d]\n", hErr, nArgc,nRequestId);
		return;
	}
	oapi_sysinfo_stb_onStatusChanged(nArgc, apArgv, nRequestId, userdata);
}


STATIC void oapi_sysinfo_stb_GetCPUTemperature_cb(HERROR hErr, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *userdata)
{
	if(ERR_OK != hErr)
	{
		HxLOG_Error("Error = [%d] nArgc = [%d] nRequestId=[%d]\n", hErr, nArgc,nRequestId);
		return;
	}
	s_pstStbInfo->ulCPUTemperature =	HxOBJECT_INT(apArgv[0]);
	s_pstStbInfo->ulMaxCPUTemperature =	HxOBJECT_INT(apArgv[1]);
}



#define ___GLOBAL_FUNCTIONS___

HERROR	OAPI_SYSINFO_STB_Init(OxSysInfoStb_Notifier_t fnNotifier)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;
	
	if (s_bInitializedStbInfo != TRUE)
	{
		HxSEMT_Create(&s_ulSysInfoStbSemId, "sysinfo_stb", HxSEMT_FIFO);
		
		hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

		ENTER_SYSINFO_STB_CACHE;

		s_pstStbInfo = (OxSysInfoStb_Info_t*)OxMEM_Calloc(sizeof(OxSysInfoStb_Info_t));
		HxSTD_MemSet(s_pstStbInfo, 0x0, sizeof(OxSysInfoStb_Info_t));
		s_nRpcHandle = nRpcHandle;
		
		HLIB_RPC_RegisterNotifyCallback (nRpcHandle, RPC_OBAMA_SYSINFO_STB_onStatusChanged, "b", NULL, (HxRPC_Noti_t)oapi_sysinfo_stb_onStatusChanged);
		HLIB_RPC_Call (nRpcHandle, RPC_OAPI_SYSINFO_STB_Init, NULL);
	
		s_pfnStbStatusChanged = fnNotifier;
		/* s_bInitializedStbInfo = TRUE; */

		LEAVE_SYSINFO_STB_CACHE;
	}

	return ERR_OK;
}

HERROR	OAPI_SYSINFO_STB_DeInit(void)
{
	ENTER_SYSINFO_STB_CACHE;
	if (s_bInitializedStbInfo == TRUE)
	{
		/* RCP_Close() «œ¡ˆ æ ∞Ì,
		   OAPI ¡æ∑·∞° « ø‰«— Process∞° APK_DeInit() ≈Î«ÿ OAPI RPC/socket/task ¡§∏Æ«—¥Ÿ */
		if (s_pfnStbStatusChanged != NULL)
		{
			s_pfnStbStatusChanged = NULL;
		}
		
		HxSTD_MemSet(s_pstStbInfo, 0x0, sizeof(OxSysInfoStb_Info_t));
		s_bInitializedStbInfo = FALSE;
	}
	LEAVE_SYSINFO_STB_CACHE;
	VK_SEM_Destroy(s_ulSysInfoStbSemId); s_ulSysInfoStbSemId = 0x0;
	return ERR_OK;
}

HERROR	OAPI_SYSINFO_Initialized(void)
{
	if(s_bInitializedStbInfo)
		return ERR_OK;
	else
		return ERR_FAIL;
}
HERROR	OAPI_SYSINFO_STB_Refresh(HBOOL bSync)
{
	HINT32	nRequestId;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	nRequestId = HLIB_RPC_Request (nRpcHandle, (HxRPC_Response_t)oapi_sysinfo_stb_onStatusChanged_as_response, NULL, RPC_OAPI_SYSINFO_STB_Refresh, NULL);

	if (bSync == TRUE)
		HLIB_RPC_Join (nRpcHandle, nRequestId, 1000);

	return ERR_OK;
}

HERROR	OAPI_SYSINFO_STB_GetCPUTemperature(HUINT32 *pulCurTemp, HUINT32 *pulMaxTemp)
{
	HINT32	nRequestId;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	nRequestId = HLIB_RPC_Request (nRpcHandle, (HxRPC_Response_t)oapi_sysinfo_stb_GetCPUTemperature_cb, NULL, RPC_OAPI_SYSINFO_GetCPUTemperature, NULL);
	HLIB_RPC_Join (nRpcHandle, nRequestId, 1000);

	if(pulCurTemp)
	{
		*pulCurTemp = s_pstStbInfo->ulCPUTemperature;
	}

	if(pulMaxTemp)
	{
		*pulMaxTemp = s_pstStbInfo->ulMaxCPUTemperature;
	}

	return ERR_OK;
}

HERROR	OAPI_SYSINFO_STB_GetModelName(HCHAR *pszModelName, HINT32 nMax)
{
	ENTER_SYSINFO_STB_CACHE;

	HLIB_STD_StrNCpySafe(pszModelName, s_pstStbInfo->szModelName, nMax);

	LEAVE_SYSINFO_STB_CACHE;

	return ERR_OK;
}

HERROR	OAPI_SYSINFO_STB_GetUpdatedList(HCHAR *pszUpdatedList, HINT32 nMax)
{
	//HCHAR				*decodeBuffer = NULL;
	//HUINT32 				ulStreamSize = 0;

	ENTER_SYSINFO_STB_CACHE;

#if 1
	HxLOG_Debug("s_pstStbInfo->szUpdatedList: %s \n", s_pstStbInfo->szUpdatedList);
	HLIB_STD_StrNCpySafe(pszUpdatedList, s_pstStbInfo->szUpdatedList, nMax);

#else // for decoding
	ulStreamSize = HxSTD_StrLen(s_pstStbInfo->szUpdatedList);

	decodeBuffer = OxMEM_Calloc(ulStreamSize+1);
	if(decodeBuffer != NULL)
	{
		HLIB_MATH_DecodeBase64(decodeBuffer, s_pstStbInfo->szUpdatedList, ulStreamSize);
	}

	HxLOG_Debug("decodeBuffer: %s \n", decodeBuffer);

	HLIB_STD_StrNCpySafe(pszUpdatedList, decodeBuffer, nMax);
#endif
	LEAVE_SYSINFO_STB_CACHE;

	return ERR_OK;
}

HERROR	OAPI_SYSINFO_STB_GetSoftwareVersion(HCHAR *pszSoftwareVersion, HINT32 nMax)
{
	ENTER_SYSINFO_STB_CACHE;

	HLIB_STD_StrNCpySafe(pszSoftwareVersion, s_pstStbInfo->szSoftwareVersion, nMax);

	LEAVE_SYSINFO_STB_CACHE;

	return ERR_OK;
}

HERROR	OAPI_SYSINFO_STB_GetPackageVersion(HCHAR *pszPackageVersion, HINT32 nMax)
{
	ENTER_SYSINFO_STB_CACHE;

	HLIB_STD_StrNCpySafe(pszPackageVersion, s_pstStbInfo->szPackageId, nMax);

	LEAVE_SYSINFO_STB_CACHE;

	return ERR_OK;
}

HERROR	OAPI_SYSINFO_STB_GetLoaderName(HCHAR *pszLoaderName, HINT32 nMax)
{
	ENTER_SYSINFO_STB_CACHE;

	HLIB_STD_StrNCpySafe(pszLoaderName, s_pstStbInfo->szLoaderName, nMax);

	LEAVE_SYSINFO_STB_CACHE;

	return ERR_OK;
}

HERROR	OAPI_SYSINFO_STB_GetLoaderVersion(HCHAR *pszLoaderVersion, HINT32 nMax)
{
	ENTER_SYSINFO_STB_CACHE;

	HLIB_STD_StrNCpySafe(pszLoaderVersion, s_pstStbInfo->szLoaderVersion, nMax);

	LEAVE_SYSINFO_STB_CACHE;

	return ERR_OK;
}

HERROR	OAPI_SYSINFO_STB_GetUpgraderVersion(HCHAR *pszVersion, HINT32 nBufSize)
{
	ENTER_SYSINFO_STB_CACHE;

	HLIB_STD_StrNCpySafe(pszVersion, s_pstStbInfo->szUpgraderVersion, nBufSize);

	LEAVE_SYSINFO_STB_CACHE;

	return ERR_OK;
}

HERROR	OAPI_SYSINFO_STB_GetManufacturerName(HCHAR *pszManufacturerName, HINT32 nMax)
{
	ENTER_SYSINFO_STB_CACHE;

	HLIB_STD_StrNCpySafe(pszManufacturerName, s_pstStbInfo->szManufacturerName, nMax);

	LEAVE_SYSINFO_STB_CACHE;

	return ERR_OK;
}

HERROR	OAPI_SYSINFO_STB_GetSystemId(HCHAR *pszSystemId, HINT32 nMax)
{
	ENTER_SYSINFO_STB_CACHE;

	HLIB_STD_StrNCpySafe(pszSystemId, s_pstStbInfo->szSystemId, nMax);

	LEAVE_SYSINFO_STB_CACHE;

	return ERR_OK;
}

HERROR	OAPI_SYSINFO_STB_GetDeviceId(HCHAR *pszDeviceId, HINT32 nMax)
{
	ENTER_SYSINFO_STB_CACHE;

	HLIB_STD_StrNCpySafe(pszDeviceId, s_pstStbInfo->szDeviceId, nMax);

	LEAVE_SYSINFO_STB_CACHE;

	return ERR_OK;
}

HERROR	OAPI_SYSINFO_STB_GetChipId(HCHAR *pszChipId, HINT32 nMax)
{
	ENTER_SYSINFO_STB_CACHE;

	HLIB_STD_StrNCpySafe(pszChipId, s_pstStbInfo->szChipId, nMax);

	LEAVE_SYSINFO_STB_CACHE;

	return ERR_OK;
}

HERROR	OAPI_SYSINFO_STB_GetObamaVersion(HCHAR *pszObamaVersion, HINT32 nMax)
{
	ENTER_SYSINFO_STB_CACHE;

	HLIB_STD_StrNCpySafe(pszObamaVersion, s_pstStbInfo->szObamaVersion, nMax);

	LEAVE_SYSINFO_STB_CACHE;

	return ERR_OK;
}

HERROR	OAPI_SYSINFO_STB_GetMicomVersion(HCHAR *pszMicomVersion, HINT32 nMax)
{
	ENTER_SYSINFO_STB_CACHE;

	HLIB_STD_StrNCpySafe(pszMicomVersion, s_pstStbInfo->szMicomVersion, nMax);

	LEAVE_SYSINFO_STB_CACHE;

	return ERR_OK;
}

HERROR	OAPI_SYSINFO_STB_GetUpdateDate(HCHAR *pszUpdateDate, HINT32 nMax)
{
	ENTER_SYSINFO_STB_CACHE;

	HLIB_STD_StrNCpySafe(pszUpdateDate, s_pstStbInfo->szUpdateDate, nMax);

	LEAVE_SYSINFO_STB_CACHE;

	return ERR_OK;
}

HERROR	OAPI_SYSINFO_STB_GetSerialNo(HCHAR *pszSerialNo, HINT32 nMax)
{
	ENTER_SYSINFO_STB_CACHE;

	HLIB_STD_StrNCpySafe(pszSerialNo, s_pstStbInfo->szSerialNumber, nMax);

	LEAVE_SYSINFO_STB_CACHE;

	return ERR_OK;
}

HERROR	OAPI_SYSINFO_STB_GetDtcpIpKeySize(HUINT32 * pnSize)
{
	if(pnSize == NULL)
		return ERR_FAIL;
	ENTER_SYSINFO_STB_CACHE;

	*pnSize = s_pstStbInfo->ulDtcpIpKeySize;

	LEAVE_SYSINFO_STB_CACHE;
	return ERR_OK;
}

HERROR	OAPI_SYSINFO_STB_GetDtcpIpKey(HINT8 *pszDtcpIpKey, HUINT32 nMax)
{
	if(pszDtcpIpKey == NULL || nMax == 0 || s_pstStbInfo->ulDtcpIpKeySize == 0)
		return ERR_FAIL;

	ENTER_SYSINFO_STB_CACHE;

	HLIB_STD_MemCpySafe(pszDtcpIpKey, nMax,s_pstStbInfo->szDtcpIpKey, s_pstStbInfo->ulDtcpIpKeySize);

	LEAVE_SYSINFO_STB_CACHE;

	return ERR_OK;
}

HERROR	OAPI_SYSINFO_STB_GetCustomCode(HUINT32 *pulCustomCode)
{
	ENTER_SYSINFO_STB_CACHE;

	*pulCustomCode = s_pstStbInfo->ulRcuCustomCode;

	LEAVE_SYSINFO_STB_CACHE;

	return ERR_OK;
}

HERROR 	OAPI_SYSINFO_STB_GetColdBootDone(HBOOL *pbColdBootDone)
{
	ENTER_SYSINFO_STB_CACHE;

	*pbColdBootDone = s_pstStbInfo->bColdBootDone;

	LEAVE_SYSINFO_STB_CACHE;

	return ERR_OK;
}

HERROR 	OAPI_SYSINFO_STB_SetFirstBootOnProduction(HBOOL bFirstBootOnProduction)
{
	HINT32	nRequestId;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	ENTER_SYSINFO_STB_CACHE;

	s_pstStbInfo->bFirstBootOnProduction = bFirstBootOnProduction;

	LEAVE_SYSINFO_STB_CACHE;
	nRequestId = HLIB_RPC_Request (nRpcHandle, (HxRPC_Response_t)oapi_sysinfo_stb_onStatusChanged_as_response, NULL, RPC_OAPI_SYSINFO_SetFirstBootOnProduction, "i", bFirstBootOnProduction);
	return ERR_OK;
}

HERROR 	OAPI_SYSINFO_STB_GetFirstBootOnProduction(HBOOL *pbFirstBootOnProduction)
{
	ENTER_SYSINFO_STB_CACHE;

	*pbFirstBootOnProduction = s_pstStbInfo->bFirstBootOnProduction;

	LEAVE_SYSINFO_STB_CACHE;

	return ERR_OK;
}

HERROR 	OAPI_SYSINFO_STB_GetLaunchAppMode(HUINT32 *pulLaunchAppMode)
{
	ENTER_SYSINFO_STB_CACHE;

	*pulLaunchAppMode = s_pstStbInfo->ulLaunchAppMode;

	LEAVE_SYSINFO_STB_CACHE;

	return ERR_OK;
}

HERROR 	OAPI_SYSINFO_STB_SetAntennaPower(HBOOL bAntPower)
{
	HINT32	nRequestId;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	ENTER_SYSINFO_STB_CACHE;

	s_pstStbInfo->bAntnnaPower = bAntPower;

	LEAVE_SYSINFO_STB_CACHE;
	nRequestId = HLIB_RPC_Request (nRpcHandle, (HxRPC_Response_t)oapi_sysinfo_stb_onStatusChanged_as_response, NULL, RPC_OAPI_SYSINFO_SetAntennaPower, "i", bAntPower);
	return ERR_OK;
}

HERROR 	OAPI_SYSINFO_STB_GetAntennaPower(HBOOL *pbAntPower)
{
	ENTER_SYSINFO_STB_CACHE;

	*pbAntPower = s_pstStbInfo->bAntnnaPower;

	LEAVE_SYSINFO_STB_CACHE;

	return ERR_OK;
}

HERROR 	OAPI_SYSINFO_STB_SetServiceUpdateFlag(HBOOL bOnOff)
{
	HINT32	nRequestId;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	ENTER_SYSINFO_STB_CACHE;

	s_pstStbInfo->bSvcUpdateFlag = bOnOff;

	LEAVE_SYSINFO_STB_CACHE;
	nRequestId = HLIB_RPC_Request (nRpcHandle, (HxRPC_Response_t)oapi_sysinfo_stb_onStatusChanged_as_response, NULL, RPC_OAPI_SYSINFO_SetSeriveUpdateFlag, "i", bOnOff);
	return ERR_OK;
}

HERROR 	OAPI_SYSINFO_STB_GetServiceUpdateFlag(HBOOL *pbOnOff)
{
	ENTER_SYSINFO_STB_CACHE;

	*pbOnOff = s_pstStbInfo->bSvcUpdateFlag;

	LEAVE_SYSINFO_STB_CACHE;

	return ERR_OK;
}

HERROR 	OAPI_SYSINFO_STB_GetNandBadBlocks(HUINT32 *pulCntLdr, HUINT32 *pulCntUBI, HUINT32 *pulCntUBIFS)
{
	ENTER_SYSINFO_STB_CACHE;

	*pulCntLdr = s_pstStbInfo->ulNandBadBlockCntLdr;
	*pulCntUBI = s_pstStbInfo->ulNandBadBlockCntUBI;
	*pulCntUBIFS = s_pstStbInfo->ulNandBadBlockCntUBIFS;

	LEAVE_SYSINFO_STB_CACHE;

	return ERR_OK;
}

HERROR 	OAPI_SYSINFO_STB_GetFusingInfo(HUINT8 *pucSCS, HUINT8 *pucJTAG, HUINT8 *pucOTP, HUINT8 *pucCWE, HUINT32 *pulMarketID)
{
	ENTER_SYSINFO_STB_CACHE;

	*pucSCS = s_pstStbInfo->ucSCS;
	*pucJTAG = s_pstStbInfo->ucJTAG;
	*pucOTP = s_pstStbInfo->ucOTP;
	*pucCWE = s_pstStbInfo->ucCWE;
	*pulMarketID = s_pstStbInfo->ulMarketID;

	LEAVE_SYSINFO_STB_CACHE;

	return ERR_OK;
}

HERROR 	OAPI_SYSINFO_STB_GetFusingInfoExt(HUINT8 *pucSCS, HUINT8 *pucJTAG, HUINT8 *pucOTP, HUINT8 *pucCWE, HUINT32 *pulOTPField1, HUINT32 *pulOTPField2)
{
	ENTER_SYSINFO_STB_CACHE;

	*pucSCS = s_pstStbInfo->ucSCS;
	*pucJTAG = s_pstStbInfo->ucJTAG;
	*pucOTP = s_pstStbInfo->ucOTP;
	*pucCWE = s_pstStbInfo->ucCWE;
	*pulOTPField1 = s_pstStbInfo->ulOTPField1;
	*pulOTPField2 = s_pstStbInfo->ulOTPField2;

	LEAVE_SYSINFO_STB_CACHE;

	return ERR_OK;
}

#if defined(CONFIG_NETFLIX_APP)
// for Netflix
HERROR	OAPI_SYSINFO_STB_SetNetflixESN(HCHAR *pszESN)
{
    HINT32  nRequestId;
    HERROR  hErr = ERR_FAIL;
    HINT32  nRpcHandle = 0;

    hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

    ENTER_SYSINFO_STB_CACHE;

    HLIB_STD_StrNCpySafe(s_pstStbInfo->szNetflixESN, pszESN, OAPI_SYSINFO_ESN_LEN);

    LEAVE_SYSINFO_STB_CACHE;

    nRequestId = HLIB_RPC_Request (nRpcHandle, (HxRPC_Response_t)oapi_sysinfo_stb_onStatusChanged_as_response, NULL, RPC_OAPI_SYSINFO_SetNetflixESN, "s", pszESN);

    return ERR_OK;
}

// for Netflix
HERROR	OAPI_SYSINFO_STB_GetNetflixESN(HCHAR *pszESN, HINT32 nMax)
{
    ENTER_SYSINFO_STB_CACHE;

	HLIB_STD_StrNCpySafe(pszESN, s_pstStbInfo->szNetflixESN, nMax);

	LEAVE_SYSINFO_STB_CACHE;

	return ERR_OK;
}
#endif


/* end of file */
