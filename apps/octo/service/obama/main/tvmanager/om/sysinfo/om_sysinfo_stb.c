/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  om_sysinfo_stb.c
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
#include <papi.h>
#include <octo_common.h>

#include <bus.h>
#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_coldboot.h>
#include <mgr_dynamicupdate.h>
#include <mgr_param.h>

#include <svc_sys.h>
#ifdef CONFIG_MW_MM_MEDIA
#include <svc_mplayercso.h>
#endif
/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define SYSINFO_CPUTEMP_TIMER_ID	0x1001
#define SYSINFO_CPUTEMP_TIMEOUT		(60*1000) // 1 minute


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static OxSysInfoStb_Info_t		*s_pstSysInfoStbInfo = NULL;
static OxSysInfoStb_Info_t		*s_pstSysInfoStbOldInfo = NULL;


/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
STATIC inline void om_sysinfo_stb_GetSerialNumber(HCHAR *pszString)
{
	HERROR  eRet = ERR_OK;
	HUINT32 i = 0;
	HUINT8  temp[32] = {0,};
	HUINT8  number_buf[4] = {0,};

	eRet = SVC_SYS_GetSystemSerialNumber(temp, sizeof(temp));
	if ( eRet != ERR_OK )
	{
		return;
	}


	for ( i=0; i<14; ++i )
	{
#if defined(CONFIG_OP_ERTELECOM) || defined(CONFIG_OP_TDC)
		HxSTD_snprintf(number_buf, 4, "%C", temp[i]);
#else
		HxSTD_snprintf(number_buf, 4, "%X", temp[i]);
#endif
		HxSTD_StrNCat(pszString, number_buf, HxSTD_StrLen(number_buf)); /* fix for prevent #135092 */
	}

	return;
}

STATIC HERROR om_sysinfo_stb_GetDtcpIpKey(HINT8 *szDtcpIpKey, HUINT32 * ulSize)
{
#if defined(CONFIG_MW_MM_MEDIA) && (CONFIG_SUPPORT_IMMA)
	HINT8 *pszTempString = NULL;
	HINT32 ulTempSize = 0;
	if(SVC_CSO_AcquireDtcpIpKey((HINT8**)&pszTempString, (HINT32 *)&ulTempSize) == ERR_OK)
	{
		if(ulTempSize <= 0 || ulTempSize > OAPI_SYSINFO_DTCP_IP_KEY_LEN || pszTempString == NULL)
			return ERR_FAIL;

		HxSTD_MemSet(szDtcpIpKey, 0x00, OAPI_SYSINFO_DTCP_IP_KEY_LEN);
		HLIB_STD_MemCpySafe(szDtcpIpKey, OAPI_SYSINFO_DTCP_IP_KEY_LEN, pszTempString, ulTempSize);
		*ulSize = ulTempSize;
		SVC_CSO_ReleaseDtcpIpKey(pszTempString);
		return ERR_OK;
	}
#endif
	return ERR_FAIL;
}


STATIC HERROR om_sysinfo_stb_GetAntennaPower (HBOOL *pbOnOff)
{
	HERROR	hError;
	HUINT32	ulValue;

	if (pbOnOff == NULL)
		return ERR_FAIL;

	*pbOnOff = FALSE;

	hError = DB_PARAM_GetItem(eDB_PARAM_ITEM_ANTENNA_POWER, &ulValue, 0);
	if (hError == ERR_OK)
	{
		HBOOL bAntPower;
		*pbOnOff = (HBOOL)ulValue;

		HxLOG_Print("[om_sysinfo_stb_GetAntennaPower] DB_PARAM_GetItem() result [%x], ulValue [%d]\n", hError, ulValue);
		hError = SVC_CH_GetTERAntennaPower(&bAntPower);
		if ( (hError == ERR_OK) && (bAntPower != *pbOnOff) )
		{
			SVC_CH_SetTERAntennaPower(*pbOnOff);
		}
	}

	return hError;
}

STATIC HERROR om_sysinfo_stb_GetServiceUpdateBlockFlag (HBOOL *pbOnOff)
{
	HERROR	hError;
	HUINT32	ulValue;

	if (pbOnOff == NULL)
		return ERR_FAIL;

	*pbOnOff = FALSE;

	hError = DB_PARAM_GetItem(eDB_PARAM_ITEM_DYNAMIC_UPDATE_SETTING, &ulValue, 0);
	if (hError == ERR_OK)
	{
		*pbOnOff = (HBOOL)ulValue;

		HxLOG_Print(HxANSI_COLOR_YELLOW("eDB_PARAM_ITEM_DYNAMIC_UPDATE_SETTING : %d\n"), ulValue);
#if 0
		if((DbSvc_DynamicUpdate_e)ulValue == eDymicUpdate_Enable)
		{
			MGR_DYNAMICUPDATE_Start();
		}
#endif
	}

	return hError;
}

STATIC void		om_sysinfo_stb_CheckStatusChange(HBOOL bChangeCheck)
{
	HUINT8		aucStbId[6];
//	HINT32		nCallbackId;
	HUINT32		ulTemp;
	HUINT16		usTemp;
	HBOOL		ubTemp;
	LaunchAppMode_t eLaunchAppMode = eLAUNCH_APP_MAIN;
	Sys_FusingInfo_t	stFusingInfo;
	OxSysInfoStb_Info_t	*pstStbInfo = s_pstSysInfoStbInfo;

#if defined(CONFIG_MW_SYS_SUPPORT_CHIPID_X64)
	HUINT32		ulTemp2;
#endif

	SVC_SYS_GetModelName(pstStbInfo->szModelName, OAPI_SYSINFO_NAME_LEN, FALSE);

#if defined(CONFIG_OTA_USE_PKG_VERSION)
	SVC_SYS_GetSystemVersion(eVERSION_TYPE_APPL_UI, &ulTemp);
	SVC_SYS_MakeSystemVerString(eVERSION_TYPE_APPL, ulTemp, pstStbInfo->szSoftwareVersion);

	SVC_SYS_GetSystemVersion(eVERSION_TYPE_PACKAGE, &ulTemp);
	SVC_SYS_MakeSystemVerString(eVERSION_TYPE_PACKAGE, ulTemp, pstStbInfo->szPackageId);
#else
	SVC_SYS_GetSystemVersion(eVERSION_TYPE_APPL, &ulTemp);
	SVC_SYS_MakeSystemVerString(eVERSION_TYPE_APPL, ulTemp, pstStbInfo->szSoftwareVersion);
#endif

	HLIB_STD_StrUtf8NCpy(pstStbInfo->szLoaderVersion, "humax loader", OAPI_SYSINFO_NAME_LEN);
	if (SVC_SYS_GetSystemVersion(eVERSION_TYPE_LOADER, &ulTemp) == SYS_ERR_OK)
	{
		SVC_SYS_MakeSystemVerString(eVERSION_TYPE_LOADER, ulTemp, pstStbInfo->szLoaderVersion);
	} else
	{
		HxSTD_StrNCpy(pstStbInfo->szLoaderVersion, "Invalid", OAPI_SYSINFO_NAME_LEN);
	}

	HLIB_STD_StrUtf8NCpy(pstStbInfo->szUpgraderVersion, "humax upgrader", OAPI_SYSINFO_NAME_LEN);
	if (SVC_SYS_GetSystemVersion(eVERSION_TYPE_UPGRADER, &ulTemp) == SYS_ERR_OK)
	{
		SVC_SYS_MakeSystemVerString(eVERSION_TYPE_UPGRADER, ulTemp, pstStbInfo->szUpgraderVersion);
	} else
	{
		HxSTD_StrNCpy(pstStbInfo->szUpgraderVersion, "Invalid", OAPI_SYSINFO_NAME_LEN);
	}
	HxLOG_Print("[eVERSION_TYPE_UPGRADER] pstStbInfo->szUpgraderVersion = %s\n", pstStbInfo->szUpgraderVersion);

	HLIB_STD_StrUtf8NCpy(pstStbInfo->szManufacturerName, "HUMAX", OAPI_SYSINFO_NAME_LEN);

	SVC_SYS_GetDeviceId(pstStbInfo->szDeviceId, OAPI_SYSINFO_NAME_LEN);

	SVC_SYS_GetSystemId(&ulTemp);
	HxSTD_snprintf(pstStbInfo->szSystemId, OAPI_SYSINFO_NAME_LEN, "%04X.%04X", ((ulTemp & 0xFFFF0000) >> 16), ulTemp & 0x0000FFFF);

	SVC_SYS_GetSystemVersion(eVERSION_TYPE_APPL, &ulTemp);
	SVC_SYS_MakeSystemVerString(eVERSION_TYPE_APPL, ulTemp, pstStbInfo->szObamaVersion);

	SVC_SYS_GetMicomVersionString(pstStbInfo->szMicomVersion);

	SVC_SYS_GetSystemUpdateDate(pstStbInfo->szUpdateDate);
	MGR_PARAM_GetUpdatedList(pstStbInfo->szUpdatedList);

	HxSTD_MemSet(pstStbInfo->szSerialNumber, 0x00, OAPI_SYSINFO_NAME_LEN);
	om_sysinfo_stb_GetSerialNumber(pstStbInfo->szSerialNumber);

#if defined(CONFIG_MW_SYS_SUPPORT_CHIPID_X64)
	// 8 byte chip id (16 digit)
	SVC_SYS_GetChipId_x64(&ulTemp, &ulTemp2);
	HxSTD_snprintf(pstStbInfo->szChipId, OAPI_SYSINFO_NAME_LEN, "%08X%08X", ulTemp, ulTemp2);
#else
	// 4 byte chip id (8 digit)
	SVC_SYS_GetChipId(&ulTemp);
	HxSTD_snprintf(pstStbInfo->szChipId, OAPI_SYSINFO_NAME_LEN, "%X", ulTemp);
#endif

	SVC_SYS_GetCurrentCustomCode(&usTemp);
	pstStbInfo->ulRcuCustomCode = usTemp;

	SVC_SYS_GetFirstBoot(&ubTemp);
	pstStbInfo->bFirstBootOnProduction = ubTemp;

	SVC_SYS_GetLaunchAppMode(&eLaunchAppMode);
	pstStbInfo->ulLaunchAppMode = eLaunchAppMode;

	om_sysinfo_stb_GetDtcpIpKey(pstStbInfo->szDtcpIpKey, &pstStbInfo->ulDtcpIpKeySize);

	om_sysinfo_stb_GetAntennaPower(&pstStbInfo->bAntnnaPower);

	om_sysinfo_stb_GetServiceUpdateBlockFlag(&pstStbInfo->bSvcUpdateFlag);

	SVC_SYS_GetNandBadBlocks(&pstStbInfo->ulNandBadBlockCntLdr, &pstStbInfo->ulNandBadBlockCntUBI, &pstStbInfo->ulNandBadBlockCntUBIFS);

#if defined(CONFIG_NETFLIX_APP)
    SVC_SYS_GetNetflixESN(pstStbInfo->szNetflixESN, OAPI_SYSINFO_ESN_LEN);
#endif

	HxSTD_MemSet(&stFusingInfo, 0x00, sizeof(Sys_FusingInfo_t));
	SVC_SYS_GetFusingInfo(&stFusingInfo);
	pstStbInfo->ucSCS = stFusingInfo.ucSCS;
	pstStbInfo->ucJTAG = stFusingInfo.ucJTAG;
	pstStbInfo->ucOTP = stFusingInfo.ucOTP;
	pstStbInfo->ucCWE = stFusingInfo.ucCWE;
	pstStbInfo->ulMarketID = stFusingInfo.ulMarketID;
	pstStbInfo->ulOTPField1 = stFusingInfo.ulOTPField1;
	pstStbInfo->ulOTPField2 = stFusingInfo.ulOTPField2;

	if ((HxSTD_MemCmp(s_pstSysInfoStbOldInfo, s_pstSysInfoStbInfo, sizeof(OxSysInfoStb_Info_t)) == 0) && bChangeCheck)
		return;

	HxSTD_MemCopy(s_pstSysInfoStbOldInfo, s_pstSysInfoStbInfo, sizeof(OxSysInfoStb_Info_t));

	(void)(aucStbId);
}

STATIC void om_sysinfo_stb_CheckCPUTemperature(void)
{
	HERROR	hErr = ERR_FAIL;
	HUINT32 ulTemp = 0;

	// Check CPU temperature every 1 minute
	// For TR-069
	// Device.DeviceInfo.TemperatureStatus.TemperatureSensor.{i}.MaxValue
	hErr = SVC_SYS_GetCPUTemperature(&ulTemp);
	if (hErr == ERR_OK)
	{
		if (s_pstSysInfoStbInfo->ulMaxCPUTemperature < ulTemp)
		{
			s_pstSysInfoStbInfo->ulMaxCPUTemperature = ulTemp;
		}
	}
}

STATIC BUS_Result_t om_sysinfo_stb_MsgTimer(HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(p1 == SYSINFO_CPUTEMP_TIMER_ID)
	{
		om_sysinfo_stb_CheckCPUTemperature();
	}

	return MESSAGE_BREAK;
}

STATIC BUS_Result_t om_sysinfo_stb_onSignalBusProcess(HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	PxSTATUS_e	ePapiStatus = nParam1;
	if(ePapiStatus == ePAMA_STATUS_Operation)
	{
		// Reset the maximum value
		HxLOG_Warning("Reset maximum CUP Temperature!!\n");
		s_pstSysInfoStbInfo->ulMaxCPUTemperature = 0;
		om_sysinfo_stb_CheckCPUTemperature();
	}

	return MESSAGE_PASS;
}

STATIC BUS_Result_t		proc_om_sysinfo_stb(HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t eGwmResult = MESSAGE_PASS;

	switch (message)
	{
		case eMEVT_BUS_CREATE:
			BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);

			// Set timer for monitoring maximum CPU temperature
			om_sysinfo_stb_CheckCPUTemperature();
			BUS_SetTimer(SYSINFO_CPUTEMP_TIMER_ID, SYSINFO_CPUTEMP_TIMEOUT);
			break;

		case eMEVT_COLDBOOT_NOTIFY_DONE:
			{
				OxSysInfoStb_Info_t	*pstStbInfo = s_pstSysInfoStbInfo;
				HxLOG_Debug("\n[%s.%d] Received eMEVT_COLDBOOT_NOTIFY_DONE. Cold Boot finish! \n", __FUNCTION__,__LINE__);
				pstStbInfo->bColdBootDone = TRUE;
				om_sysinfo_stb_CheckStatusChange(TRUE);
			}
			break;

		case eMEVT_BUS_TIMER:
			eGwmResult = om_sysinfo_stb_MsgTimer(p1, p2, p3);
			break;

		case eMEVT_BUS_SIGNAL:
			eGwmResult = om_sysinfo_stb_onSignalBusProcess(p1, p2, p3);
			break;

//		TODO: add event for notifying changed sysinfo stb.
//		case EVT_SYSINFO_IS_UPDATED:
//			om_sysinfo_stb_CheckStatusChange(TRUE);
//			break;
		default:
			break;
	}

	return eGwmResult;
}
//	nArgc == 0

STATIC HERROR om_sysinfo_stb_Init(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	HxLOG_Assert(nArgc == 0);

	om_sysinfo_stb_CheckStatusChange(FALSE);


	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);

	if (hErr == ERR_OK)
	{
		HLIB_RPC_Notify(nRpcHandle, RPC_OBAMA_SYSINFO_STB_onStatusChanged, "b", s_pstSysInfoStbInfo, sizeof(OxSysInfoStb_Info_t));
	}

	return ERR_OK;
}

STATIC	HERROR	om_sysinfo_stb_Refresh(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;


	om_sysinfo_stb_CheckStatusChange(FALSE);

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr == ERR_OK)
	{
		HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_OK, "b", s_pstSysInfoStbInfo, sizeof(OxSysInfoStb_Info_t));
	}

	return ERR_OK;
}

STATIC	HERROR	om_sysinfo_stb_GetCPUTemperature(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	HUINT32 ulTemp = 0;

	hErr = SVC_SYS_GetCPUTemperature(&ulTemp);
	hErr |= OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr == ERR_OK)
	{
		// Re-check with latest value
		om_sysinfo_stb_CheckCPUTemperature();
		HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_OK, "ii", (ulTemp/1000), (s_pstSysInfoStbInfo->ulMaxCPUTemperature/1000));
	}

	return ERR_OK;
}


STATIC	HERROR	om_sysinfo_stb_SetFirstBootOnProduction(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HBOOL	bFirstBootOnProduction = (HUINT32)HxOBJECT_INT(apArgv[0]);
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;


	HxLOG_Debug("bFirstBootOnProduction: %d \n", bFirstBootOnProduction);

	SVC_SYS_SetFirstBoot(bFirstBootOnProduction);

	om_sysinfo_stb_CheckStatusChange(FALSE);

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr == ERR_OK)
	{
		HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_OK, "b", s_pstSysInfoStbInfo, sizeof(OxSysInfoStb_Info_t));
	}
	return ERR_OK;
}

STATIC HERROR om_sysinfo_stb_SetAntennaPower(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HBOOL bAntPower = (HUINT32)HxOBJECT_INT(apArgv[0]);
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	HxLOG_Debug("om_sysinfo_stb_SetAntennaPower: %d \n", bAntPower);

	hErr = DB_PARAM_SetItem(eDB_PARAM_ITEM_ANTENNA_POWER, (HUINT32)bAntPower, 0);
	HxLOG_Print("[om_sysinfo_stb_SetAntennaPower] DB_PARAM_SetItem() result [%x], ulValue [%d]\n", hErr, bAntPower);

	if (hErr == ERR_OK)
	{
	    SVC_CH_SetTERAntennaPower(bAntPower);

	    om_sysinfo_stb_CheckStatusChange(FALSE);

		hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
		if(hErr == ERR_OK)
		{
		    HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_OK, "b", s_pstSysInfoStbInfo, sizeof(OxSysInfoStb_Info_t));
		}
	}

	return hErr;
}

STATIC HERROR om_sysinfo_stb_SetServiceUpdateBlockFlag(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32	ulValue =(HUINT32)HxOBJECT_INT(apArgv[0]);
	HERROR	hErr;
	HINT32	nRpcHandle = 0;

	HxLOG_Debug("om_sysinfo_stb_SetServiceUpdateBlockFlag: %d \n", ulValue);

	hErr = DB_PARAM_SetItem(eDB_PARAM_ITEM_DYNAMIC_UPDATE_SETTING, ulValue, 0);
	HxLOG_Print("[om_sysinfo_stb_SetServiceUpdateBlockFlag] DB_PARAM_SetItem() result [%x], ulValue [%d]\n", hErr, ulValue);

	if (hErr == ERR_OK)
	{
		om_sysinfo_stb_CheckStatusChange(FALSE);

		hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
		if(hErr == ERR_OK)
		{
			HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_OK, "b", s_pstSysInfoStbInfo, sizeof(OxSysInfoStb_Info_t));
		}
	}

	return hErr;
}


#if defined(CONFIG_NETFLIX_APP)
STATIC  HERROR  om_sysinfo_stb_SetNetflixESN(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
    HUINT8 *pszESN = (HUINT8 *)HxOBJECT_STR(apArgv[0]);
	HERROR	hErr;
	HINT32	nRpcHandle = 0;

	HxLOG_Debug("szNetflixESN: %s \n", pszESN);

    hErr = SVC_SYS_SetNetflixESN(pszESN, OAPI_SYSINFO_ESN_LEN);

	if (hErr == ERR_OK)
	{
		om_sysinfo_stb_CheckStatusChange(FALSE);

        hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
        if(hErr == ERR_OK)
    	{
    		HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_OK, "b", s_pstSysInfoStbInfo, sizeof(OxSysInfoStb_Info_t));
        }
	}

	return hErr;
}
#endif



HERROR	OM_SYSINFO_STB_Init(void)
{
	HERROR	err;
	HINT32	nRpcHandle = 0;

	HxLOG_Trace();

	s_pstSysInfoStbInfo = (OxSysInfoStb_Info_t*)OxMGR_Calloc(sizeof(OxSysInfoStb_Info_t));
	s_pstSysInfoStbOldInfo = (OxSysInfoStb_Info_t*)OxMGR_Calloc(sizeof(OxSysInfoStb_Info_t));

	BUS_MGR_Create(NULL, APP_ACTION_PRIORITY, proc_om_sysinfo_stb, 0, 0, 0, 0);

	err = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if(err == ERR_OK)
	{

		err = HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_SYSINFO_STB_Init, NULL, (HxRPC_Func_t)om_sysinfo_stb_Init, NULL,
								"Activate the sysinfo.stb: (void)");
		if(err != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterMethod (nRpcHandle, %s) returned error(%d)\n", RPC_OAPI_SYSINFO_STB_Init, err);
		}


		err = HLIB_RPC_RegisterRequest (nRpcHandle, RPC_OAPI_SYSINFO_STB_Refresh, NULL, (HxRPC_Func_t)om_sysinfo_stb_Refresh, NULL,
								"Refresh System Info : (void)");
		if(err != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterRequest (nRpcHandle, %s) returned error(%d)\n", RPC_OAPI_SYSINFO_STB_Refresh, err);
		}

		err = HLIB_RPC_RegisterRequest (nRpcHandle, RPC_OAPI_SYSINFO_GetCPUTemperature, NULL, (HxRPC_Func_t)om_sysinfo_stb_GetCPUTemperature, NULL,
								"Get CPU Temperature : (void)");
		if(err != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterRequest (nRpcHandle, %s) returned error(%d)\n", RPC_OAPI_SYSINFO_GetCPUTemperature, err);
		}

		err = HLIB_RPC_RegisterRequest (nRpcHandle, RPC_OAPI_SYSINFO_SetFirstBootOnProduction, (HCHAR *)"i", (HxRPC_Func_t)om_sysinfo_stb_SetFirstBootOnProduction, NULL,
								"Set First Boot On Production");
		if(err != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterRequest (nRpcHandle, %s) returned error(%d)\n", RPC_OAPI_SYSINFO_SetFirstBootOnProduction, err);
		}


		err = HLIB_RPC_RegisterRequest (nRpcHandle, RPC_OAPI_SYSINFO_SetAntennaPower, (HCHAR *)"i", (HxRPC_Func_t)om_sysinfo_stb_SetAntennaPower, NULL,
								"Set Antenna Power");
		if(err != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterRequest (nRpcHandle, %s) returned error(%d)\n", RPC_OAPI_SYSINFO_SetAntennaPower, err);
		}


		err = HLIB_RPC_RegisterRequest (nRpcHandle, RPC_OAPI_SYSINFO_SetSeriveUpdateFlag, (HCHAR *)"i", (HxRPC_Func_t)om_sysinfo_stb_SetServiceUpdateBlockFlag, NULL,
								"Set Service Update Flag");
		if(err != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterRequest (nRpcHandle, %s) returned error(%d)\n", RPC_OAPI_SYSINFO_SetSeriveUpdateFlag, err);
		}

#if defined(CONFIG_NETFLIX_APP)
        err = HLIB_RPC_RegisterRequest (nRpcHandle, RPC_OAPI_SYSINFO_SetNetflixESN, (HCHAR *)"s", (HxRPC_Func_t)om_sysinfo_stb_SetNetflixESN, NULL,
								"Set Netflix ESN");
		if(err != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterRequest (nRpcHandle, %s) returned error(%d)\n", RPC_OAPI_SYSINFO_SetNetflixESN, err);
		}
#endif

		err = HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_SYSINFO_STB_onStatusChanged, "b",
				"notifying when Tuner information is changed : (OxSysInfoStbInfo_t *pstStbInfo)");

	}


	return err;
}
