/**************************************************************
*	@file	rema.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			rema
**************************************************************/

/*
* ⓒ 2011-2012 Humax Co., Ltd.
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
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

#define ___HEADER_FILES___

#include <vkernel.h>
#include <hlib.h>

#include <rema_int.h>

#include "rema_MOD.h"
#include "rema_module_service.h"

#include <oapi.h>
#include <napi.h>
#include <uapi.h>

#define false			FALSE

#define ___DEFINES___

#define ___MACROS___

#define ___TYPEDEF___
typedef struct RMS_NetConfinfo_t
{
    Base_MOD_t stMOD;
    NAPINetConfInfo_t *pstNetInfo;
} RMS_NetConfinfo_t;

typedef struct RMS_StorageInfo_t
{
    Base_MOD_t			stMOD;
    DxStorage_Info_t	*pstInfoList;
    HUINT32				unNumPhysical;
    HUINT32				unNumLogical;
} RMS_StorageInfo_t;

#define ___STATIC_VARIABLES___
RMS_NetConfinfo_t gstNetConfInfo; 
RMS_StorageInfo_t gstStorageInfo;
#define ___INTERNAL_FUNCTIONS___
static void Stb_Notifier_t(void)
{
    REM_FUNC_ENTER;

    REM_FUNC_LEAVE;
}

static void rema_netconf_callback(HINT32 event, void* data)
{
    HUINT32 param1 = 0 ; // param2 = 0, param3 = 0;

    if(data)
	param1 = ((NAPINetConfInfo_t *)data)->id;

    switch( event )
    {
    case NAPI_EVENT_NETCONF_CONN_SUCCESS:
	break;
    case NAPI_EVENT_NETCONF_CONN_FAIL:
	break;
    case NAPI_EVENT_NETCONF_AUTOIP_SUCCESS:
	break;
    case NAPI_EVENT_NETCONF_AUTOIP_FAIL:
	break;
    case NAPI_EVENT_NETCONF_LINK_CONNECTED:
	break;
    case NAPI_EVENT_NETCONF_LINK_DISCONNECTED:
	break;
    case NAPI_EVENT_NETCONF_STATUS_UPDATED:
	break;
    default :
	return ;
    }

    return;
}
void	mgr_storage_umma_Notifier(DxStorage_NotiType_e eType, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
#if 0
    HxLOG_Trace();
    if (eDxSTORAGE_NOTI_DevChange == eType)
    {
	HBOOL	bConnect = (HBOOL)nParam1;
	DxStorage_Info_t	*pstDxInfo = (DxStorage_Info_t	*)nParam2;
	HBOOL	bInternal = FALSE, bPvr = FALSE;
	HUINT32	ulPvrIdx = 0xff, i;
	SvcFs_DevInfo_t	*pstDevInfo;
	SvcFs_DevIdx_e	eDevIdx = (SvcFs_DevIdx_e)pstDxInfo->ulDeviceId;

	HxLOG_Warning("eDevIdx : %d, bConnect : %s \n", eDevIdx, (bConnect)?"TRUE":"FALSE");

	if (pstDxInfo->eDevType == eDxSTORAGE_DEVTYPE_INTHDD || pstDxInfo->eDevType == eDxSTORAGE_DEVTYPE_ESATAHDD)
	{
	    bInternal = TRUE;
	}

	if ((pstDxInfo->ulFlags & eDxSTORAGE_FLAG_PVRDEVICE) || (pstDxInfo->ulFlags & eDxSTORAGE_FLAG_SUBPVRDEVICE))
	{
	    bPvr = TRUE;

	    for (i = 0 ; i < pstDxInfo->nPartitionNum ; i++)
	    {
		if ( (pstDxInfo->astPartitionInfos[i].ulFlags& eDxSTORAGE_FLAG_PVRDEVICE)  || (pstDxInfo->astPartitionInfos[i].ulFlags& eDxSTORAGE_FLAG_SUBPVRDEVICE))
		{
		    ulPvrIdx = (HUINT32)pstDxInfo->astPartitionInfos[i].nPvrIdx;
		    break;
		}
	    }
	}

	if (TRUE == bConnect)
	{
	    BUS_PostMessage(NULL, eMEVT_UMMA_NOTIFY_STORAGE_CONNECTED, HANDLE_NULL, (HINT32)eDevIdx, (HINT32)bInternal, (HINT32)bPvr);
	}
	else
	{
	    pstDevInfo = (SvcFs_DevInfo_t *)OxAP_Calloc(sizeof(SvcFs_DevInfo_t));
	    if (pstDevInfo == NULL)
	    {
		HxLOG_Error("OxAP_Calloc Error !!!! \n");
		return ;
	    }
	    SVC_FS_ConvertDevDxInfo2SvcFs(pstDxInfo, pstDevInfo);
	    BUS_PostData(NULL, eMEVT_UMMA_NOTIFY_STORAGE_DISCONNECTED, HANDLE_NULL, (HINT32)eDevIdx, (HINT32)ulPvrIdx, (void *)pstDevInfo, sizeof(SvcFs_DevInfo_t));

	    if(pstDevInfo != NULL)
	    {
		OxMGR_Free (pstDevInfo);
		pstDevInfo = NULL;
	    }
	}
    }

    else if (eDxSTORAGE_NOTI_PvrReady == eType)
    {
	// device의 연결/해제, 혹은 Format으로 인해 PVR device 정보가 바뀌었을때 발생
	HBOOL	bPvr = (HBOOL)nParam1;	//TRUE : PVR storage, FALSE : Not a PVR storage (disable or unpaired)
	SvcFs_DevIdx_e	eDevIdx = (SvcFs_DevIdx_e)nParam2;
	HUINT32	ulPvrIdx =(HUINT32)nParam3;

	if (bPvr == TRUE)
	{
	    //BUS_PostMessage(NULL, eMEVT_UMMA_NOTIFY_STORAGE_PVR_ENABLED, HANDLE_NULL, (HINT32)eDevIdx, (HINT32)ulPvrIdx, (HINT32)NULL);
	}
	else
	{
	    //BUS_PostMessage(NULL, eMEVT_UMMA_NOTIFY_STORAGE_PVR_DISABLED, HANDLE_NULL, (HINT32)eDevIdx, (HINT32)ulPvrIdx, (HINT32)NULL);
	}

	//HxLOG_Warning("PvrReady : %s, eDevIdx : %d \n", (bPvr)?"T":"F", eDevIdx);

    }
    else if(eDxSTORAGE_NOTI_FormatState)
    {

    }

    else if (eDxSTORAGE_NOTI_Result == eType)
    {
	// API Call에 대한 결과를 리턴
	DxStorage_ResultType_e eResultType = (DxStorage_ResultType_e)nParam1;
	HUINT32 	ulDeviceId = (HUINT32)nParam2;	// == eDevIdx
	HERROR	hErr = (HERROR)nParam3;				// API 결과 (ERR_OK:0, ERR_FAIL:1)

	//HxLOG_Warning("eResultType : %d, eDevIdx : %d, Result : %d\n", eResultType, ulDeviceId, hErr);
	switch(eResultType)
	{
	case	eDxSTORAGE_RESULT_Format:
	    // Add event if it is necessary.
	    break;

	case	eDxSTORAGE_RESULT_Dst:
	    // Add event if it is necessary.
	    break;

	case	eDxSTORAGE_RESULT_SetDefaultHdd:
	    // Add event if it is necessary.
	    break;

	case	eDxSTORAGE_RESULT_SetName:
	    break;

	case	eDxSTORAGE_RESULT_DisableHdd:
	    // Add event if it is necessary.
	    break;

	case	eDxSTORAGE_RESULT_Pairing:
	    break;

	case	eDxSTORAGE_RESULT_UnPairing:
	    break;

	case	eDxSTORAGE_RESULT_SafeRemove:
	    break;

	default:
	    break;
	}
    }
    else
    {
	//HxLOG_Error("Unknown Type : %d \n", eType);
    }
#endif
}
static HINT32 rema_MOD_Service_NetConfInfo_Init(void)
{
    REM_FUNC_ENTER;

    gstNetConfInfo.pstNetInfo = (NAPINetConfInfo_t*) HLIB_STD_MemAlloc(sizeof(NAPINetConfInfo_t));
    if(gstNetConfInfo.pstNetInfo)
    {
	HxSTD_MemSet(gstNetConfInfo.pstNetInfo, 0, sizeof(NAPINetConfInfo_t));
    }

    REM_FUNC_LEAVE;

    return 0;
}
static HINT32 rema_MOD_Service_NetConfInfo_Deinit(void)
{
    REM_FUNC_ENTER;

    if(gstNetConfInfo.pstNetInfo )
    {
	HLIB_STD_MemFree(gstNetConfInfo.pstNetInfo);

    }

    REM_FUNC_LEAVE;

    return 0;
}

static HINT32 rema_MOD_Service_NetConfInfo_Update(void)
{


    REM_FUNC_ENTER;

    NAPI_NetConf_GetInfo(0, gstNetConfInfo.pstNetInfo);


    REM_FUNC_LEAVE;

    return 0;
}


static HINT32 rema_MOD_Service_NetConfInfo_checkCTX(void *pvCtx)
{
    REM_FUNC_ENTER;
    if(!pvCtx)
	return -1;

    if(!gstNetConfInfo.pstNetInfo )
	return -1;

    if( pvCtx != gstNetConfInfo.pstNetInfo )
	return -1;

    REM_FUNC_LEAVE;
    return 0;
}


static HINT32 rema_MOD_Service_StorageInfo_Init(void)
{
    REM_FUNC_ENTER;

    gstStorageInfo.pstInfoList = NULL;
    gstStorageInfo.unNumPhysical=0;
    gstStorageInfo.unNumLogical = 0;


    REM_FUNC_LEAVE;

    return 0;
}
static HINT32 rema_MOD_Service_StorageInfo_Deinit(void)
{
    REM_FUNC_ENTER;

    if(gstStorageInfo.pstInfoList )
    {
	UAPI_FS_FreeList(gstStorageInfo.pstInfoList, gstStorageInfo.unNumPhysical);
	gstStorageInfo.pstInfoList = 0;
	gstStorageInfo.unNumPhysical = 0;
	gstStorageInfo.unNumLogical = 0;
    }

    REM_FUNC_LEAVE;

    return 0;
}

static HINT32 rema_MOD_Service_StorageInfo_Update(void)
{
    HUINT32 ulIdx = 0;
    DxStorage_Info_t *pstInfo = NULL;
    HUINT32 ulDevNum =0;
    HUINT32 i=0;
    REM_FUNC_ENTER;

    if(gstStorageInfo.pstInfoList  )
    {
		UAPI_FS_FreeList(gstStorageInfo.pstInfoList, gstStorageInfo.unNumPhysical);
		gstStorageInfo.pstInfoList = 0;
		gstStorageInfo.unNumPhysical = 0;
		gstStorageInfo.unNumLogical = 0;
    }
	
    UAPI_FS_GetList(eDxSTORAGE_FILTER_All, &gstStorageInfo.pstInfoList, &gstStorageInfo.unNumPhysical);
    if(gstStorageInfo.pstInfoList)
    {
		for (ulIdx = 0 ; ulIdx < gstStorageInfo.unNumPhysical ; ulIdx++)
		{
		    pstInfo = &gstStorageInfo.pstInfoList[ulIdx];
			if(pstInfo)	
				gstStorageInfo.unNumLogical += pstInfo->nPartitionNum;
		}
    }

    ulDevNum = gstStorageInfo.unNumPhysical;
    RemLOG_Debug("================ xxxxx ================ \n");
    for (ulIdx = 0 ; ulIdx < ulDevNum ; ulIdx++)
    {
		pstInfo = &gstStorageInfo.pstInfoList[ulIdx];
		RemLOG_Debug("================ Storage Info ================ \n");
		if(pstInfo)
		{
			RemLOG_Debug("ulDeviceId : [%d]\n", pstInfo->ulDeviceId);
			RemLOG_Debug("DevType : [%s]\n", UAPI_FS_GetDevTypeString(pstInfo->eDevType));
			RemLOG_Debug("szMountPath : [%s]\n", pstInfo->szMountPath);
			RemLOG_Debug("szDevName : [%s] \n", pstInfo->szDevName);
			RemLOG_Debug("ullTotalSizeKb : [%lld] \n", pstInfo->ullTotalSizeKb);
			RemLOG_Debug("szLabel : [%s]\n", pstInfo->szLabel);
			RemLOG_Debug("szUUID : [%s]\n", pstInfo->szUUID);
			RemLOG_Debug("ulFlags : [0x%x]\n", pstInfo->ulFlags);
			RemLOG_Debug("	%s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_CONNECTED));
			RemLOG_Debug("	%s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_MOUNTED));
			RemLOG_Debug("	%s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_WRITEPROTECTED));
			RemLOG_Debug("	%s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_SECURED));
			RemLOG_Debug("	%s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_INTERNAL));
			RemLOG_Debug("	%s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_PVRDEVICE));
			RemLOG_Debug("	%s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_NEEDFORMAT));
			RemLOG_Debug("	%s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_PAIRED));
			RemLOG_Debug("	%s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_DEFAULTHDD));
			RemLOG_Debug("	%s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_SUBPVRDEVICE));
			RemLOG_Debug("	%s \n", DxSTORAGE_CHECK_FLAG(pstInfo->ulFlags, eDxSTORAGE_FLAG_NEW2NDHDD));

			HLIB_CMD_Printf("\n--------Partition Info--------\n");
			for (i=0; i < pstInfo->nPartitionNum ; i++)
			{
			    RemLOG_Debug("\n   [%d] \n",i);
			    RemLOG_Debug("	bAvailable : [%d]\n", pstInfo->astPartitionInfos[i].bAvailable);
			    RemLOG_Debug("	FsType: [%s]\n", UAPI_FS_GetFsString(pstInfo->astPartitionInfos[i].eFsType));
			    RemLOG_Debug("	ullPartitionSizeKb : [%lld]\n", pstInfo->astPartitionInfos[i].ullPartitionSizeKb);
			    RemLOG_Debug("	nPvrIdx : [%d]\n", pstInfo->astPartitionInfos[i].nPvrIdx);
			    RemLOG_Debug("	szMountPath : [%s]\n", pstInfo->astPartitionInfos[i].szMountPath);
			    RemLOG_Debug("	szTsrPath : [%s]\n", pstInfo->astPartitionInfos[i].szTsrPath);
			    RemLOG_Debug("	szVolumeName : [%s]\n", pstInfo->astPartitionInfos[i].szVolumeName);
			    RemLOG_Debug("	szRecordPath : [%s]\n", pstInfo->astPartitionInfos[i].szRecordPath);
			    RemLOG_Debug("	szMediaPath : [%s]\n", pstInfo->astPartitionInfos[i].szMediaPath);
			    RemLOG_Debug("	szMediaVideo : [%s]\n", pstInfo->astPartitionInfos[i].szMediaVideo);
			    RemLOG_Debug("	szMediaAudio : [%s]\n", pstInfo->astPartitionInfos[i].szMediaAudio);
			    RemLOG_Debug("	szMediaPic : [%s]\n", pstInfo->astPartitionInfos[i].szMediaPic);
			    RemLOG_Debug("	szMediaDownload : [%s]\n", pstInfo->astPartitionInfos[i].szMediaDownload);
			    RemLOG_Debug("	ulStartUnit : [%d], ulEndUnit : [%d]\n", pstInfo->astPartitionInfos[i].ulStartUnit, pstInfo->astPartitionInfos[i].ulEndUnit);
			    RemLOG_Debug("	ulFlags : [0x%x]\n", pstInfo->astPartitionInfos[i].ulFlags);
			    RemLOG_Debug("	   %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_MOUNTED));
			    RemLOG_Debug("	   %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_WRITEPROTECTED));
			    RemLOG_Debug("	   %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_SECURED));
			    RemLOG_Debug("	   %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_PVRDEVICE));
			    RemLOG_Debug("	   %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_DEFAULTHDD));
			    RemLOG_Debug("	   %s \n", DxSTORAGE_CHECK_FLAG(pstInfo->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_SUBPVRDEVICE));
			}
		}

		RemLOG_Debug("===========================================\n");

    }

    REM_FUNC_LEAVE;

    return 0;
}


static HINT32 rema_MOD_Service_StorageInfo_checkCTX(void *pvCtx)
{
    REM_FUNC_ENTER;
    if(!pvCtx)
	return -1;

    if(!gstStorageInfo.pstInfoList )
	return -1;

    if( pvCtx != gstStorageInfo.pstInfoList )
	return -1;

    REM_FUNC_LEAVE;
    return 0;
}

#define ___PUBLIC_FUNCTIONS___
HERROR rema_MOD_Service_Init(void)
{
    REM_FUNC_ENTER;

   //OAPI_SYSINFO_STB_Init(Stb_Notifier_t);

    //NAPI_Init();
    //NAPI_NetConf_RegisterCallback(rema_netconf_callback);
    UAPI_Init();
    UAPI_FS_Init();
    UAPI_FS_SetNotifier(mgr_storage_umma_Notifier);
    //UAPI_FM_SetGlobalListener(NULL, __system_file_notifier);

#if 0
    gstNetConfInfo.stMOD.bIsInit = false;
    gstNetConfInfo.stMOD.bIsUpdate = false;
    gstNetConfInfo.stMOD.pcbMod_Init = rema_MOD_Service_NetConfInfo_Init;
    gstNetConfInfo.stMOD.pcbMod_Deinit = rema_MOD_Service_NetConfInfo_Deinit;
    gstNetConfInfo.stMOD.pcbMod_Update = rema_MOD_Service_NetConfInfo_Update;
#endif

    gstStorageInfo.stMOD.bIsInit = false;
    gstStorageInfo.stMOD.bIsUpdate = false;
    gstStorageInfo.stMOD.pcbMod_Init = rema_MOD_Service_StorageInfo_Init;
    gstStorageInfo.stMOD.pcbMod_Deinit = rema_MOD_Service_StorageInfo_Deinit;
    gstStorageInfo.stMOD.pcbMod_Update = rema_MOD_Service_StorageInfo_Update;

    //rema_MOD_QInit(gstNetConfInfo);
    rema_MOD_QInit(gstStorageInfo);

    REM_FUNC_LEAVE;

    return ERR_OK;
}
void rema_MOD_Service_Deinit(void)
{
    REM_FUNC_ENTER;

    //rema_MOD_QDeinit(gstNetConfInfo);
    rema_MOD_QDeinit(gstStorageInfo);

    NAPI_DeInit();
    OAPI_SYSINFO_STB_DeInit();
    REM_FUNC_LEAVE;
    return;
}

HINT32 rema_MOD_Service_resetUpdate(void)
{
    rema_MOD_QResetUpdate(gstNetConfInfo);
    rema_MOD_QResetUpdate(gstStorageInfo);

    return 0 ; 
}



HERROR rema_MOD_Service_GetStrLAN(const HCHAR *pszName, HCHAR *pBuf, HINT32 nBufLen)
{

    if(!pszName)
	return ERR_FAIL;

    if(!pBuf)
	return ERR_FAIL;

    if(rema_MOD_QUpdate(gstNetConfInfo))
    {


	if(!HxSTD_StrCmp(pszName, "AddressingType"))
	{
	    if(gstNetConfInfo.pstNetInfo->dhcp)
	    {
		HxSTD_PrintToStrN(pBuf, nBufLen, "DHCP");
	    }
	    else
	    {
		HxSTD_PrintToStrN(pBuf, nBufLen, "Static");
	    }
	}
	else if(!HxSTD_StrCmp(pszName, "IPAddress"))
	{ 
	    if(gstNetConfInfo.pstNetInfo->ip_ver == eNAPI_NETCONF_IPV4)
	    {

		HxSTD_PrintToStrN(pBuf, nBufLen, "%s", gstNetConfInfo.pstNetInfo->ip_info.ipv4.ipAddr);
	    }
	    else if(gstNetConfInfo.pstNetInfo->ip_ver == eNAPI_NETCONF_IPV6)
	    {
		HxSTD_PrintToStrN(pBuf, nBufLen, "%s", gstNetConfInfo.pstNetInfo->ip_info.ipv6.ipAddr);
	    }
	}
	else if(!HxSTD_StrCmp(pszName, "SubnetMask"))
	{ 
	    if(gstNetConfInfo.pstNetInfo->ip_ver == eNAPI_NETCONF_IPV4)
	    {

		HxSTD_PrintToStrN(pBuf, nBufLen, "%s", gstNetConfInfo.pstNetInfo->ip_info.ipv4.netmask);
	    }
	}
	else if(!HxSTD_StrCmp(pszName, "DefaultGateway"))
	{ 
	    if(gstNetConfInfo.pstNetInfo->ip_ver == eNAPI_NETCONF_IPV4)
	    {

		HxSTD_PrintToStrN(pBuf, nBufLen, "%s", gstNetConfInfo.pstNetInfo->ip_info.ipv4.gateway);
	    }
	    else if(gstNetConfInfo.pstNetInfo->ip_ver == eNAPI_NETCONF_IPV6)
	    {
		HxSTD_PrintToStrN(pBuf, nBufLen, "%s", gstNetConfInfo.pstNetInfo->ip_info.ipv6.gateway);
	    }
	}
	else if(!HxSTD_StrCmp(pszName, "DNSServers"))
	{ 
	    if(gstNetConfInfo.pstNetInfo->ip_ver == eNAPI_NETCONF_IPV4)
	    {
		HxSTD_PrintToStrN(pBuf, nBufLen, "%s,%s", 
		    gstNetConfInfo.pstNetInfo->ip_info.ipv4.dns1, 
		    gstNetConfInfo.pstNetInfo->ip_info.ipv4.dns2);
	    }
	    else if(gstNetConfInfo.pstNetInfo->ip_ver == eNAPI_NETCONF_IPV6)
	    {
		HxSTD_PrintToStrN(pBuf, nBufLen, "%s,%s",
		    gstNetConfInfo.pstNetInfo->ip_info.ipv6.dns1, 
		    gstNetConfInfo.pstNetInfo->ip_info.ipv4.dns2);
	    }
	}
	else if(!HxSTD_StrCmp(pszName, "MACAddress"))
	{ 
	    if(gstNetConfInfo.pstNetInfo->ip_ver == eNAPI_NETCONF_IPV4)
	    {
		HxSTD_PrintToStrN(pBuf, nBufLen, "%s", 
		    gstNetConfInfo.pstNetInfo->ip_info.ipv4.macaddr);
	    }
	    else if(gstNetConfInfo.pstNetInfo->ip_ver == eNAPI_NETCONF_IPV6)
	    {
		HxSTD_PrintToStrN(pBuf, nBufLen, "%s",
		    gstNetConfInfo.pstNetInfo->ip_info.ipv6.macaddr);
	    }
	}
    }

    return ERR_OK;
}
#if 0
HERROR rema_MOD_Service_GetMax_Storage(void *pvCtx, HUINT32 *punMax)
{

    REM_FUNC_ENTER;
    if(rema_MOD_QUpdate(gstStorageInfo))
    {
	*punMax = gstStorageInfo.unNumPhysical;
    }
    REM_FUNC_LEAVE;
    return ERR_OK;
}

void * rema_MOD_Service_GetCTX_Storage(int nIdx)
{

    return NULL; //gstStorageInfo.pstInfoList;
}
#endif
HERROR rema_MOD_Service_GetMax_Storage_PhysicalMedium(void *pvCtx, HUINT32 *punMax)
{
    REM_FUNC_ENTER;
    if(rema_MOD_QUpdate(gstStorageInfo))
    {
	*punMax = gstStorageInfo.unNumPhysical;
    }
    REM_FUNC_LEAVE;
    return ERR_OK;
}
void * rema_MOD_Service_GetCTX_Storage_PhysicalMedium(int nIdx)
{
    return &gstStorageInfo.pstInfoList[nIdx];
}
HERROR rema_MOD_Service_GetStr_Storage_PhysicalMedium(void *pvCtx, const HCHAR *pszName, HCHAR *pBuf, HINT32 nBufLen)
{
    DxStorage_Info_t *pstInfo = NULL;

    if(!pvCtx)
	return ERR_FAIL;
    if(!pszName)
	return ERR_FAIL;

    pstInfo = (DxStorage_Info_t *)pvCtx;
    RemLOG_Debug("%s", pszName);
    if(!HxSTD_StrCmp(pszName, "Name"))
    {
	HxSTD_PrintToStrN(pBuf, nBufLen, "%s", 
	    pstInfo->szDevName);
    }
    else
    {
	return ERR_FAIL;
    }


    return ERR_OK; 
}
HERROR rema_MOD_Service_GetUINT_Storage_PhysicalMedium(void *pvCtx, const HCHAR *pszName, HUINT32 *punData)
{
    DxStorage_Info_t *pstInfo = NULL;

    if(!pvCtx)
	return ERR_FAIL;
    if(!pszName)
	return ERR_FAIL;

    pstInfo = (DxStorage_Info_t *)pvCtx;

    if(!HxSTD_StrCmp(pszName, "Capacity"))
    {
	*punData = pstInfo->ullTotalSizeKb;
    }
    else
    {
	return ERR_FAIL;
    }
    return ERR_OK; 
}

HERROR rema_MOD_Service_GetMax_Storage_LogicalVolume(void *pvCtx, HUINT32 *punMax)
{
    REM_FUNC_ENTER;
    if(rema_MOD_QUpdate(gstStorageInfo))
    {
	*punMax = gstStorageInfo.unNumLogical;
    }
    REM_FUNC_LEAVE;
    return ERR_OK;
}
void * rema_MOD_Service_GetCTX_Storage_LogicalVolume(int nIdx)
{
    void *pvData =NULL;
    DxStorage_Info_t *pstInfo = NULL;
    HUINT32 ulIdx, i; 
    HUINT32 ulNum = 0;; 

    if(gstStorageInfo.pstInfoList)
    {
	for (ulIdx = 0 ; ulIdx < gstStorageInfo.unNumPhysical ; ulIdx++)
	{
	    pstInfo = &gstStorageInfo.pstInfoList[ulIdx];
	    for(i=0; i< pstInfo->nPartitionNum; i++)
	    {
		if(ulNum == nIdx)
		{
		    pvData = &pstInfo->astPartitionInfos[i];
		    return pvData;
		}
		ulNum++;
	    }

	}
    }
    return pvData; 
}
HERROR rema_MOD_Service_GetStr_Storage_LogicalVolume(void *pvCtx, const HCHAR *pszName, HCHAR *pBuf, HINT32 nBufLen)
{
    DxStorage_PartitionInfo_t *pstInfo = NULL;

    if(!pvCtx)
	return ERR_FAIL;
    if(!pszName)
	return ERR_FAIL;

    pstInfo = (DxStorage_PartitionInfo_t *)pvCtx;
    RemLOG_Debug("%s", pszName);
    if(!HxSTD_StrCmp(pszName, "Name"))
    {
	HxSTD_PrintToStrN(pBuf, nBufLen, "%s", 
	    pstInfo->szVolumeName);
    }
    else
    {
	return ERR_FAIL;
    }


    return ERR_OK; 
}
HERROR rema_MOD_Service_GetUINT_Storage_LogicalVolume(void *pvCtx, const HCHAR *pszName, HUINT32 *punData)
{
    DxStorage_PartitionInfo_t *pstInfo = NULL;

    if(!pvCtx)
	return ERR_FAIL;
    if(!pszName)
	return ERR_FAIL;

    pstInfo = (DxStorage_PartitionInfo_t *)pvCtx;

    if(!HxSTD_StrCmp(pszName, "Capacity"))
    {
	*punData = pstInfo->ullPartitionSizeKb;
    }
    else
    {
	return ERR_FAIL;
    }
    return ERR_OK; 
}
#define ___EXTERNAL_FUNCTIONS___

#define _____END_OF_FILE_________

