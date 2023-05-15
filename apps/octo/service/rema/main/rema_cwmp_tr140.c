/**************************************************************
 *	@file		trport_tr140.c
 *	appkit for cwmp Module
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2013/05/14
 *	@author			humax
 *	@brief			trport for cwmp
 **************************************************************/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <wait.h>
#include <hlib.h>
#include <hapi.h>
#include <papi.h>
#include <uapi.h>

#include "rema_int.h"
#include "rema_cwmp.h"
#include "rema_DM.h"
#include "rema_module_service.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/



/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#if 0
#define HxLog_Print printf
#define HxLog_Debug printf
#define HxLog_Message printf
#define HxLog_info printf
#endif


extern HERROR rema_MOD_Service_GetMax_Storage_PhysicalMedium(void *pvCtx, HUINT32 *punMax);

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
static HERROR get_StorageSvc_M_PhysicalMediumNumberOfEntries(HINT32 *pszOutput);

static HERROR get_Service_StorageServiceNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;

	//1 Storage Service !!
	// always "1", not entry

	*pszOutput = 1;

#if 0
	HINT32 nEntry = 0;
	eTRXDMInstancetype_e eType = eTR140A1_Device_Services_StorageServiceNumberOfEntries;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
		*pszOutput = 1;
#else
	//APK_3RDPARTY_CWMP_TR140_Get_CommonDM_Entry(&nEntry,eType);
	printf("nEntry(%d)\n",nEntry);
	if(nEntry == 0)
	{
		*pszOutput = 255;
	}
	else
	{
		*pszOutput = nEntry;
	}
#endif
#endif
	REM_LEAVE(hErr);
}

static HERROR get_Service_StorageNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nEntry = 0;
	eTRXDMInstancetype_e eType = eTR140A1_Device_Services_StorageServiceNumberOfEntries;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
		*pszOutput = 1;
#else
	//2 Storage(HDD) Entry

	//APK_3RDPARTY_CWMP_TR140_Get_CommonDM_Entry(&nEntry,eType);

	*pszOutput = nEntry;
#endif

	REM_LEAVE(hErr);
}

static HERROR get_StorageSvc_M_Enable(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nConditionCnt = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "true");
#else

	//2 Connected or None
	get_StorageSvc_M_PhysicalMediumNumberOfEntries(&nConditionCnt);
	if(nConditionCnt > 0)
	{
		HxSTD_StrNCpy(pszOutput, "true",(SIZE_16-1));
	}
	else
	{
		HxSTD_StrNCpy(pszOutput, "false",(SIZE_16-1));
	}
#endif

	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_PhysicalMediumNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;

	DxStorage_Info_t *pstInfoList, *pstInfo;
	HUINT32 ulDevNum = 0;
	
	hErr = UAPI_FS_GetList(eDxSTORAGE_FILTER_All, &pstInfoList, &ulDevNum);

	// Even though device is zero, offline should be sended.
	if(ulDevNum == 0)
		ulDevNum = 1;
	
	*pszOutput = ulDevNum;

	RemLOG_Print("PhysicalMediumNumber : %d\r\n",__func__,__LINE__, *pszOutput);

	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_LogicalVolumeNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	DxStorage_Info_t *pstInfoList, *pstInfo;
	HUINT32 ulDevNum = 0;
	
	hErr = UAPI_FS_GetList(eDxSTORAGE_FILTER_All, &pstInfoList, &ulDevNum);

	// Even though device is zero, offline should be sended.
	if(ulDevNum == 0)
		ulDevNum = 1;
	
	*pszOutput = ulDevNum;

	RemLOG_Print("LogicalVolumeNumber : %d\r\n",__func__,__LINE__, *pszOutput);
	
	REM_LEAVE(hErr);
}
static HERROR get_StorageSvc_M_USB_HDDNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0;
	eTRXDMInstancetype_e eType = eTR140A1_DSStorageSvc_M_USB_HDDNumberOfEntries;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	*pszOutput = 0;
#else
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	//APK_3RDPARTY_CWMP_TR140_Get_CommonDM_D1_Entry(pszOutput,eType,nArg1);
#endif


	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_Capa_FTPCapable(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrNCpy(pszOutput, "false",(SIZE_16-1));

	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_Capa_SFTPCapable(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrNCpy(pszOutput, "false",(SIZE_16-1));


	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_Capa_HTTPCapable(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrNCpy(pszOutput, "false",(SIZE_16-1));


	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_Capa_HTTPSCapable(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrNCpy(pszOutput, "false",(SIZE_16-1));


	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_Capa_HTTPWritable(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrNCpy(pszOutput, "false",(SIZE_16-1));


	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_Capa_SupportedNetworkProtocols(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrNCpy(pszOutput, "HTTP",(SIZE_16-1));


	REM_LEAVE(hErr);

}

static HERROR get_StorageSvc_M_Capa_SupportedFileSystemTypes(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "EXT");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);

	//APK_3RDPARTY_CWMP_TR140_Get_CommonDM_Multi_D1(pszOutput,eTR140A1_DSStorageSvc_M_Capabilities_SupportedFileSystemTypes,nArg1);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_Capa_VolumeEncryptionCapable(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrNCpy(pszOutput, "false",(SIZE_16-1));


	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_NetInfo_HostName(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1

#else
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);

	//APK_3RDPARTY_CWMP_TR140_Get_CommonDM_Multi_D1(pszOutput,eTR140A1_DSStorageSvc_M_NetInfo_HostName,nArg1);
#endif


	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_NetInfo_DomainName(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1

#else
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	//APK_3RDPARTY_CWMP_TR140_Get_CommonDM_Multi_D1(pszOutput,eTR140A1_DSStorageSvc_M_NetInfo_DomainName,nArg1);
#endif


	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_NetworkServer_AFPEnable(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrNCpy(pszOutput, "false",(SIZE_16-1));


	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_NetworkServer_NFSEnable(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrNCpy(pszOutput, "false",(SIZE_16-1));


	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_NetworkServer_SMBEnable(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrNCpy(pszOutput, "false",(SIZE_16-1));


	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_NetworkServer_NetworkProtocolAuthReq(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrNCpy(pszOutput, "false",(SIZE_16-1));


	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_PhysicalMedium_M_Name(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1

#else
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_PhysicalMediumNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR140_Get_CommonDM_Multi_D2(pszOutput,eTR140A1_DSStorageSvc_PhysicalMedium_M_Name,nArg1,nArg2);
#endif


	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_PhysicalMedium_M_Vendor(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	CHECK_NULL(pszOutput);

#if 1

#else
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_PhysicalMediumNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR140_Get_CommonDM_Multi_D2(pszOutput,eTR140A1_DSStorageSvc_PhysicalMedium_M_Vendor,nArg1,nArg2);
#endif


	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_PhysicalMedium_M_Model(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	CHECK_NULL(pszOutput);

#if 1

#else
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_PhysicalMediumNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR140_Get_CommonDM_Multi_D2(pszOutput,eTR140A1_DSStorageSvc_PhysicalMedium_M_Model,nArg1,nArg2);
#endif


	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_PhysicalMedium_M_SerialNumber(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	CHECK_NULL(pszOutput);

#if 1

#else
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_PhysicalMediumNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR140_Get_CommonDM_Multi_D2(pszOutput,eTR140A1_DSStorageSvc_PhysicalMedium_M_SerialNumber,nArg1,nArg2);
#endif


	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_PhysicalMedium_M_FirmwareVersion(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	CHECK_NULL(pszOutput);

	//NOT SUPPORTED
	pszOutput[0]='\0';


	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_PhysicalMedium_M_ConnectionType(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "USB 2.0");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_PhysicalMediumNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR140_Get_CommonDM_Multi_D2(pszOutput,eTR140A1_DSStorageSvc_PhysicalMedium_M_ConnectionType,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_PhysicalMedium_M_Removable(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "true");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_PhysicalMediumNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR140_Get_CommonDM_Multi_D2(pszOutput,eTR140A1_DSStorageSvc_PhysicalMedium_M_Removable,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_PhysicalMedium_M_Capacity(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;
	HUINT64 ullTotalSize = 0;
	HUINT64 ullAvailSize = 0;
	HUINT64 ullUsedSize = 0;
	HUINT64 ullReservedSize = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_PhysicalMediumNumberOfEntries,&nArg2);

	// M/W로 내리지 않는다면, Offset 다시 원복 해줘야 한다.
	nArg2 = (nArg2+ENTRY_OFFSET);

	//APK_SQCMS_UTIL_GetSize(nArg2, &ullTotalSize, &ullAvailSize, &ullUsedSize, &ullReservedSize);

	HxLog_Debug("//APK_SQCMS_UTIL_GetSize result ullTotalSize(%lld), ullAvailSize(%lld), ullReservedSize(%lld)\n",
			(ullTotalSize/1024),(ullAvailSize/1024),(ullReservedSize/1024));

	HxSTD_PrintToStrN(pszOutput, SIZE_16,"%lld",(ullTotalSize/1024));

	//Not Use
	////APK_3RDPARTY_CWMP_TR140_Get_CommonDM_Multi_D2(pszOutput,eTR140A1_DSStorageSvc_PhysicalMedium_M_Capacity,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_PhysicalMedium_M_Status(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;
	HINT32 devIdx = 0;
	HUINT64 ulTotalKbSize, ulAvailableKbSize, ullUsedKbSize, pullReservedKbSize, ullAvailablePvrKbSize;
	DxStorage_Info_t *pstInfoList, *pstInfo;
	HBOOL	bIsConnected = FALSE;
	HUINT32 ulDevNum = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	// Currently STB don't know "Standby"
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_PhysicalMediumNumberOfEntries,&nArg2);

	UAPI_FS_GetSize(nArg2, &ulTotalKbSize, &ulAvailableKbSize, &ullUsedKbSize, &pullReservedKbSize, &ullAvailablePvrKbSize);
	UAPI_FS_GetList(eDxSTORAGE_FILTER_All, &pstInfoList, &ulDevNum);
	if(ulDevNum == 0)
	{
		HxSTD_StrCpy(pszOutput, "Offline");
	}
	else
	{
		devIdx = nArg2;
		pstInfo = &pstInfoList[devIdx];

		if(pstInfo)
		{
			bIsConnected = (pstInfo->ulFlags & eDxSTORAGE_FLAG_CONNECTED) ? TRUE : FALSE;

			DMO_Print_PhysicalMedium_StorageInfo(pstInfo);
		}

		if(bIsConnected == TRUE)
			HxSTD_StrCpy(pszOutput, "Online");			
		else
			HxSTD_StrCpy(pszOutput, "Offline");			
		
	}
	
	UAPI_FS_FreeList(pstInfoList, ulDevNum);

	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_PhysicalMedium_M_Uptime(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
	//NOT SUPPORTED
	HxSTD_StrNCpy(pszOutput, "0",(SIZE_16-1));


	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_PhysicalMedium_M_SMARTCapable(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
	//NOT SUPPORTED
	HxSTD_StrNCpy(pszOutput, "false",(SIZE_16-1));


	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_PhysicalMedium_M_Health(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;
	HUINT64 ulTotalKbSize, ulAvailableKbSize, ullUsedKbSize, pullReservedKbSize, ullAvailablePvrKbSize;	
	HINT32 devIdx = 0;
	HUINT32 ulDevNum = 0;
	DxStorage_Info_t *pstInfoList, *pstInfo;	
	HBOOL	bIsConnected = FALSE;
	
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
	
	// Currently STB don't know "Failing"
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_PhysicalMediumNumberOfEntries,&nArg2);

	UAPI_FS_GetSize(nArg2, &ulTotalKbSize, &ulAvailableKbSize, &ullUsedKbSize, &pullReservedKbSize, &ullAvailablePvrKbSize);
	UAPI_FS_GetList(eDxSTORAGE_FILTER_All, &pstInfoList, &ulDevNum);
	if(ulDevNum == 0)
	{
		HxSTD_StrCpy(pszOutput, "Error");
	}
	else
	{
		devIdx = nArg2;
		pstInfo = &pstInfoList[devIdx];
		if(pstInfo)
		{
			bIsConnected = (pstInfo->ulFlags & eDxSTORAGE_FLAG_CONNECTED) ? TRUE : FALSE;
		}

		if(bIsConnected == TRUE)
			HxSTD_StrCpy(pszOutput, "OK");
		else
			HxSTD_StrCpy(pszOutput, "Error");
	}

	UAPI_FS_FreeList(pstInfoList, ulDevNum);
	
	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_PhysicalMedium_M_HotSwappable(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "false");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_PhysicalMediumNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR140_Get_CommonDM_Multi_D2(pszOutput,eTR140A1_DSStorageSvc_PhysicalMedium_M_HotSwappable,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_PhysicalMedium_M_JCOM_PowerSupply_Interlocking(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "false");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_PhysicalMediumNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR140_Get_CommonDM_Multi_D2(pszOutput,eTR140A1_DSStorageSvc_PhysicalMedium_M_X_JCOM_PowerSupply_Interlocking,nArg1,nArg2);
#endif


	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_LogicalVolume_M_Name(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1

#else
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_LogicalVolumeNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR140_Get_CommonDM_Multi_D2(pszOutput,eTR140A1_DSStorageSvc_LogicalVolume_M_Name,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_LogicalVolume_M_Status(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "Offline");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_LogicalVolumeNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR140_Get_CommonDM_Multi_D2(pszOutput,eTR140A1_DSStorageSvc_LogicalVolume_M_Status,nArg1,nArg2);
#endif


	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_LogicalVolume_M_Enable(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "true");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_LogicalVolumeNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR140_Get_CommonDM_Multi_D2(pszOutput,eTR140A1_DSStorageSvc_LogicalVolume_M_Enable,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_LogicalVolume_M_PhysicalReference(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	CHECK_NULL(pszOutput);

#if 1

#else
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_LogicalVolumeNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR140_Get_CommonDM_Multi_D2(pszOutput,eTR140A1_DSStorageSvc_LogicalVolume_M_PhysicalReference,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_LogicalVolume_M_FileSystem(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "EXT");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_LogicalVolumeNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR140_Get_CommonDM_Multi_D2(pszOutput,eTR140A1_DSStorageSvc_LogicalVolume_M_FileSystem,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_LogicalVolume_M_Capacity(HUINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;
	HUINT64 ulTotalKbSize, ulAvailableKbSize, ullUsedKbSize, pullReservedKbSize, ullAvailablePvrKbSize;	
	DxStorage_Info_t *pstInfoList, *pstInfo;
	HUINT32 ulDevNum = 0;	

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_LogicalVolumeNumberOfEntries,&nArg2);

	UAPI_FS_GetList(eDxSTORAGE_FILTER_All, &pstInfoList, ulDevNum);
	pstInfo = &pstInfoList[nArg2];
	if(pstInfo)
	{
		hErr = UAPI_FS_GetSize(pstInfo->ulDeviceId, &ulTotalKbSize, &ulAvailableKbSize, &ullUsedKbSize, &pullReservedKbSize, &ullAvailablePvrKbSize);
		if(hErr == ERR_FAIL)
		{
			RemLOG_Print("UAPI_FS_GetSize is Failed!!!\r\n");
			*pszOutput = 0;
		}
		else
		{
			// The Capacity of the Logical Volume in MB. 
			*pszOutput = ulTotalKbSize/1024;
		}
	}

	UAPI_FS_FreeList(pstInfoList, ulDevNum);
	
	REM_LEAVE(hErr);
}
static HERROR get_StorageSvc_M_LogicalVolume_M_UsedSpace(HUINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;
	HUINT64 ulTotalKbSize, ulAvailableKbSize, ullUsedKbSize, pullReservedKbSize, ullAvailablePvrKbSize;	
	DxStorage_Info_t *pstInfoList, *pstInfo;
	HUINT32 ulDevNum = 0;	

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_LogicalVolumeNumberOfEntries,&nArg2);

	UAPI_FS_GetList(eDxSTORAGE_FILTER_All, &pstInfoList, ulDevNum);
	pstInfo = &pstInfoList[nArg2];
	if(pstInfo)
	{
		hErr = UAPI_FS_GetSize(pstInfo->ulDeviceId, &ulTotalKbSize, &ulAvailableKbSize, &ullUsedKbSize, &pullReservedKbSize, &ullAvailablePvrKbSize);
		if(hErr == ERR_FAIL)
		{
			RemLOG_Print("UAPI_FS_GetSize is Failed!!!\r\n");
			*pszOutput = 0;
		}
		else
		{
			// The Amount of Used Space on the Logical Volume in MB. 
			*pszOutput = ullUsedKbSize/1024;
		}
	}
	
	UAPI_FS_FreeList(pstInfoList, ulDevNum);
	
	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_LogicalVolume_M_Encrypted(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "false");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_LogicalVolumeNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR140_Get_CommonDM_Multi_D2(pszOutput,eTR140A1_DSStorageSvc_LogicalVolume_M_Encrypted,nArg1,nArg2);
#endif


	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_LogicalVolume_M_FolderNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	*pszOutput = 0;
#else
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_LogicalVolumeNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR140_Get_CommonDM_D2_Entry(pszOutput,eTR140A1_DSStorageSvc_LogicalVolume_M_FolderNumberOfEntries,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_LogicalVolume_M_Folder_M_Name(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	pszOutput[0]='\0';


	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_LogicalVolume_M_Folder_M_Enable(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrNCpy(pszOutput, "false",(SIZE_16-1));


	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_LogicalVolume_M_Folder_M_UserAccountAccess(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	pszOutput[0]='\0';


	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_JCOM_USB_HDD_M_RegistrationEnable(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "false");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_USB_HDDNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR140_Get_CommonDM_Multi_D2(pszOutput,eTR140A1_DSStorageSvc_M_X_JCOM_USB_HDD_M_RegistrationEnable,nArg1,nArg2);
#endif


	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_JCOM_USB_HDD_M_USB_HDDEnable(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "false");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_USB_HDDNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR140_Get_CommonDM_Multi_D2(pszOutput,eTR140A1_DSStorageSvc_M_X_JCOM_USB_HDD_M_USB_HDDEnable,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_JCOM_USB_HDD_M_Status(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "Disconnected");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_USB_HDDNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR140_Get_CommonDM_Multi_D2(pszOutput,eTR140A1_DSStorageSvc_M_X_JCOM_USB_HDD_M_Status,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_JCOM_USB_HDD_M_Name(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	CHECK_NULL(pszOutput);

#if 1

#else
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_USB_HDDNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR140_Get_CommonDM_Multi_D2(pszOutput,eTR140A1_DSStorageSvc_M_X_JCOM_USB_HDD_M_Name,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_JCOM_USB_HDD_M_Initialization(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "false");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_USB_HDDNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR140_Get_CommonDM_Multi_D2(pszOutput,eTR140A1_DSStorageSvc_M_X_JCOM_USB_HDD_M_Initialization,nArg1,nArg2);
#endif


	REM_LEAVE(hErr);

}
static HERROR get_StorageSvc_M_JCOM_USB_HDD_M_Diagnostics_DiagnosticsState(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "None");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_USB_HDDNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR140_Get_CommonDM_Multi_D2(pszOutput,eTR140A1_DSStorageSvc_M_X_JCOM_USB_HDD_M_Diagnostics_DiagnosticsState,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);
}



#define ______________set______________

static HERROR set_StorageSvc_M_NetInfo_HostName(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);

	//APK_3RDPARTY_CWMP_TR140_Set_D1(pszInput,eTR140A1_DSStorageSvc_M_NetInfo_HostName,nArg1);

	REM_LEAVE(hErr);

}
static HERROR set_StorageSvc_M_NetInfo_DomainName(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);

	//APK_3RDPARTY_CWMP_TR140_Set_D1(pszInput,eTR140A1_DSStorageSvc_M_NetInfo_DomainName,nArg1);

	REM_LEAVE(hErr);
}
static HERROR set_StorageSvc_M_NetworkServer_AFPEnable(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);
	RemLOG_Print("Not Supported!\r\n");
	REM_LEAVE(hErr);
}
static HERROR set_StorageSvc_M_NetworkServer_NFSEnable(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);
	RemLOG_Print("Not Supported!\r\n");
	REM_LEAVE(hErr);
}
static HERROR set_StorageSvc_M_NetworkServer_SMBEnable(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);
	RemLOG_Print("Not Supported!\r\n");
	REM_LEAVE(hErr);
}
static HERROR set_StorageSvc_M_NetworkServer_NetworkProtocolAuthReq(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);
	RemLOG_Print("Not Supported!\r\n");
	REM_LEAVE(hErr);
}
static HERROR set_StorageSvc_M_PhysicalMedium_M_Name(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_PhysicalMediumNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR140_Set_D2(pszInput,eTR140A1_DSStorageSvc_PhysicalMedium_M_Name,nArg1,nArg2);

	REM_LEAVE(hErr);
}
static HERROR set_StorageSvc_M_LogicalVolume_M_Name(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_LogicalVolumeNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR140_Set_D2(pszInput,eTR140A1_DSStorageSvc_LogicalVolume_M_Name,nArg1,nArg2);

	REM_LEAVE(hErr);
}
static HERROR set_StorageSvc_M_LogicalVolume_M_Enable(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_LogicalVolumeNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR140_Set_D2(pszInput,eTR140A1_DSStorageSvc_LogicalVolume_M_Enable,nArg1,nArg2);

	REM_LEAVE(hErr);
}
static HERROR set_StorageSvc_M_LogicalVolume_M_PhysicalReference(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_LogicalVolumeNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR140_Set_D2(pszInput,eTR140A1_DSStorageSvc_LogicalVolume_M_PhysicalReference,nArg1,nArg2);

	REM_LEAVE(hErr);
}
static HERROR set_StorageSvc_M_LogicalVolume_M_Capacity(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_LogicalVolumeNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR140_Set_D2(pszInput,eTR140A1_DSStorageSvc_LogicalVolume_M_Capacity,nArg1,nArg2);

	REM_LEAVE(hErr);
}
static HERROR set_StorageSvc_M_LogicalVolume_M_Folder_M_Name(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);
	RemLOG_Print("Not Supported!\r\n");
	REM_LEAVE(hErr);
}
static HERROR set_StorageSvc_M_LogicalVolume_M_Folder_M_Enable(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);
	RemLOG_Print("Not Supported!\r\n");
	REM_LEAVE(hErr);
}
static HERROR set_StorageSvc_M_LogicalVolume_M_Folder_M_UserAccountAccess(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);
	RemLOG_Print("Not Supported!\r\n");
	REM_LEAVE(hErr);
}
static HERROR set_StorageSvc_M_Enable(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);

	//APK_3RDPARTY_CWMP_TR140_Set_D1(pszInput,eTR140A1_DSStorageSvc_M_Enable,nArg1);

	REM_LEAVE(hErr);
}
static HERROR set_StorageSvc_M_JCOM_USB_HDD_M_RegistrationEnable(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_USB_HDDNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR140_Set_D2(pszInput,eTR140A1_DSStorageSvc_M_X_JCOM_USB_HDD_M_RegistrationEnable,nArg1,nArg2);

	REM_LEAVE(hErr);
}
static HERROR set_StorageSvc_M_JCOM_USB_HDD_M_USB_HDDEnable(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_USB_HDDNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR140_Set_D2(pszInput,eTR140A1_DSStorageSvc_M_X_JCOM_USB_HDD_M_USB_HDDEnable,nArg1,nArg2);

	REM_LEAVE(hErr);
}
static HERROR set_StorageSvc_M_JCOM_USB_HDD_M_Status(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_USB_HDDNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR140_Set_D2(pszInput,eTR140A1_DSStorageSvc_M_X_JCOM_USB_HDD_M_Status,nArg1,nArg2);

	REM_LEAVE(hErr);
}
static HERROR set_StorageSvc_M_JCOM_USB_HDD_M_Name(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_USB_HDDNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR140_Set_D2(pszInput,eTR140A1_DSStorageSvc_M_X_JCOM_USB_HDD_M_Name,nArg1,nArg2);

	REM_LEAVE(hErr);
}
static HERROR set_StorageSvc_M_JCOM_USB_HDD_M_Initialization(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_USB_HDDNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR140_Set_D2(pszInput,eTR140A1_DSStorageSvc_M_X_JCOM_USB_HDD_M_Initialization,nArg1,nArg2);

	REM_LEAVE(hErr);
}
static HERROR set_StorageSvc_M_JCOM_USB_HDD_M_Diagnostics_DiagnosticsState(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	rema_DM_MultiInstance_GetEntryCount(eTR140A1_Device_Services_StorageServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR140A1_DSStorageSvc_M_USB_HDDNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR140_Set_D2(pszInput,eTR140A1_DSStorageSvc_M_X_JCOM_USB_HDD_M_Diagnostics_DiagnosticsState,nArg1,nArg2);

	REM_LEAVE(hErr);
}



#define ______________PUBLIC_________________

HERROR TRP140_Get_Service_StorageServiceNumberOfEntries(HINT32 *pszOutput)
{
	return	get_Service_StorageServiceNumberOfEntries(pszOutput);
}
HERROR TRP140_Get_Service_StorageNumberOfEntries(HINT32 *pszOutput)
{
	return	get_Service_StorageNumberOfEntries(pszOutput);
}

HERROR TRP140_Get_StorageSvc_M_Enable(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_Enable(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_PhysicalMediumNumberOfEntries(HINT32 *pszOutput)
{
	return	get_StorageSvc_M_PhysicalMediumNumberOfEntries(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_LogicalVolumeNumberOfEntries(HINT32 *pszOutput)
{
	return	get_StorageSvc_M_LogicalVolumeNumberOfEntries(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_USB_HDDNumberOfEntries(HINT32 *pszOutput)
{
	return	get_StorageSvc_M_USB_HDDNumberOfEntries(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_Capa_FTPCapable(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_Capa_FTPCapable(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_Capa_SFTPCapable(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_Capa_SFTPCapable(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_Capa_HTTPCapable(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_Capa_HTTPCapable(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_Capa_HTTPSCapable(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_Capa_HTTPSCapable(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_Capa_HTTPWritable(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_Capa_HTTPWritable(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_Capa_SupportedNetworkProtocols(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_Capa_SupportedNetworkProtocols(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_Capa_SupportedFileSystemTypes(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_Capa_SupportedFileSystemTypes(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_Capa_VolumeEncryptionCapable(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_Capa_VolumeEncryptionCapable(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_NetInfo_HostName(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_NetInfo_HostName(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_NetInfo_DomainName(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_NetInfo_DomainName(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_NetworkServer_AFPEnable(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_NetworkServer_AFPEnable(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_NetworkServer_NFSEnable(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_NetworkServer_NFSEnable(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_NetworkServer_SMBEnable(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_NetworkServer_SMBEnable(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_NetworkServer_NetworkProtocolAuthReq(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_NetworkServer_NetworkProtocolAuthReq(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_PhysicalMedium_M_Name(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_PhysicalMedium_M_Name(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_PhysicalMedium_M_Vendor(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_PhysicalMedium_M_Vendor(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_PhysicalMedium_M_Model(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_PhysicalMedium_M_Model(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_PhysicalMedium_M_SerialNumber(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_PhysicalMedium_M_SerialNumber(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_PhysicalMedium_M_FirmwareVersion(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_PhysicalMedium_M_FirmwareVersion(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_PhysicalMedium_M_ConnectionType(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_PhysicalMedium_M_ConnectionType(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_PhysicalMedium_M_Removable(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_PhysicalMedium_M_Removable(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_PhysicalMedium_M_Capacity(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_PhysicalMedium_M_Capacity(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_PhysicalMedium_M_Status(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_PhysicalMedium_M_Status(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_PhysicalMedium_M_Uptime(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_PhysicalMedium_M_Uptime(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_PhysicalMedium_M_SMARTCapable(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_PhysicalMedium_M_SMARTCapable(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_PhysicalMedium_M_Health(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_PhysicalMedium_M_Health(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_PhysicalMedium_M_HotSwappable(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_PhysicalMedium_M_HotSwappable(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_PhysicalMedium_M_JCOM_PowerSupply_Interlocking(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_PhysicalMedium_M_JCOM_PowerSupply_Interlocking(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_LogicalVolume_M_Name(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_LogicalVolume_M_Name(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_LogicalVolume_M_Status(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_LogicalVolume_M_Status(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_LogicalVolume_M_Enable(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_LogicalVolume_M_Enable(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_LogicalVolume_M_PhysicalReference(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_LogicalVolume_M_PhysicalReference(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_LogicalVolume_M_FileSystem(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_LogicalVolume_M_FileSystem(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_LogicalVolume_M_Capacity(HUINT32 *pszOutput)
{
	return	get_StorageSvc_M_LogicalVolume_M_Capacity(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_LogicalVolume_M_UsedSpace(HUINT32 *pszOutput)
{
	return	get_StorageSvc_M_LogicalVolume_M_UsedSpace(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_LogicalVolume_M_Encrypted(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_LogicalVolume_M_Encrypted(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_LogicalVolume_M_FolderNumberOfEntries(HINT32 *pszOutput)
{
	return	get_StorageSvc_M_LogicalVolume_M_FolderNumberOfEntries(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_LogicalVolume_M_Folder_M_Name(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_LogicalVolume_M_Folder_M_Name(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_LogicalVolume_M_Folder_M_Enable(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_LogicalVolume_M_Folder_M_Enable(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_LogicalVolume_M_Folder_M_UserAccountAccess(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_LogicalVolume_M_Folder_M_UserAccountAccess(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_JCOM_USB_HDD_M_RegistrationEnable(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_JCOM_USB_HDD_M_RegistrationEnable(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_JCOM_USB_HDD_M_USB_HDDEnable(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_JCOM_USB_HDD_M_USB_HDDEnable(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_JCOM_USB_HDD_M_Status(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_JCOM_USB_HDD_M_Status(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_JCOM_USB_HDD_M_Name(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_JCOM_USB_HDD_M_Name(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_JCOM_USB_HDD_M_Initialization(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_JCOM_USB_HDD_M_Initialization(pszOutput);
}
HERROR TRP140_Get_StorageSvc_M_JCOM_USB_HDD_M_Diagnostics_DiagnosticsState(HCHAR *pszOutput)
{
	return	get_StorageSvc_M_JCOM_USB_HDD_M_Diagnostics_DiagnosticsState(pszOutput);
}



#define ___________SET__________________


HERROR TRP140_Set_StorageSvc_M_NetInfo_HostName(HCHAR *pszInput)
{
	return	set_StorageSvc_M_NetInfo_HostName(pszInput);
}
HERROR TRP140_Set_StorageSvc_M_NetInfo_DomainName(HCHAR *pszInput)
{
	return	set_StorageSvc_M_NetInfo_DomainName(pszInput);
}
HERROR TRP140_Set_StorageSvc_M_NetworkServer_AFPEnable(HCHAR *pszInput)
{
	return	set_StorageSvc_M_NetworkServer_AFPEnable(pszInput);
}
HERROR TRP140_Set_StorageSvc_M_NetworkServer_NFSEnable(HCHAR *pszInput)
{
	return	set_StorageSvc_M_NetworkServer_NFSEnable(pszInput);
}
HERROR TRP140_Set_StorageSvc_M_NetworkServer_SMBEnable(HCHAR *pszInput)
{
	return	set_StorageSvc_M_NetworkServer_SMBEnable(pszInput);
}
HERROR TRP140_Set_StorageSvc_M_NetworkServer_NetworkProtocolAuthReq(HCHAR *pszInput)
{
	return	set_StorageSvc_M_NetworkServer_NetworkProtocolAuthReq(pszInput);
}
HERROR TRP140_Set_StorageSvc_M_PhysicalMedium_M_Name(HCHAR *pszInput)
{
	return	set_StorageSvc_M_PhysicalMedium_M_Name(pszInput);
}
HERROR TRP140_Set_StorageSvc_M_LogicalVolume_M_Name(HCHAR *pszInput)
{
	return	set_StorageSvc_M_LogicalVolume_M_Name(pszInput);
}
HERROR TRP140_Set_StorageSvc_M_LogicalVolume_M_Enable(HCHAR *pszInput)
{
	return	set_StorageSvc_M_LogicalVolume_M_Enable(pszInput);
}
HERROR TRP140_Set_StorageSvc_M_LogicalVolume_M_PhysicalReference(HCHAR *pszInput)
{
	return	set_StorageSvc_M_LogicalVolume_M_PhysicalReference(pszInput);
}
HERROR TRP140_Set_StorageSvc_M_LogicalVolume_M_Capacity(HCHAR *pszInput)
{
	return	set_StorageSvc_M_LogicalVolume_M_Capacity(pszInput);
}
HERROR TRP140_Set_StorageSvc_M_LogicalVolume_M_Folder_M_Name(HCHAR *pszInput)
{
	return	set_StorageSvc_M_LogicalVolume_M_Folder_M_Name(pszInput);
}
HERROR TRP140_Set_StorageSvc_M_LogicalVolume_M_Folder_M_Enable(HCHAR *pszInput)
{
	return	set_StorageSvc_M_LogicalVolume_M_Folder_M_Enable(pszInput);
}
HERROR TRP140_Set_StorageSvc_M_LogicalVolume_M_Folder_M_UserAccountAccess(HCHAR *pszInput)
{
	return	set_StorageSvc_M_LogicalVolume_M_Folder_M_UserAccountAccess(pszInput);
}
HERROR TRP140_Set_StorageSvc_M_Enable(HCHAR *pszInput)
{
	return	set_StorageSvc_M_Enable(pszInput);
}
HERROR TRP140_Set_StorageSvc_M_JCOM_USB_HDD_M_RegistrationEnable(HCHAR *pszInput)
{
	return	set_StorageSvc_M_JCOM_USB_HDD_M_RegistrationEnable(pszInput);
}
HERROR TRP140_Set_StorageSvc_M_JCOM_USB_HDD_M_USB_HDDEnable(HCHAR *pszInput)
{
	return	set_StorageSvc_M_JCOM_USB_HDD_M_USB_HDDEnable(pszInput);
}
HERROR TRP140_Set_StorageSvc_M_JCOM_USB_HDD_M_Status(HCHAR *pszInput)
{
	return	set_StorageSvc_M_JCOM_USB_HDD_M_Status(pszInput);
}
HERROR TRP140_Set_StorageSvc_M_JCOM_USB_HDD_M_Name(HCHAR *pszInput)
{
	return	set_StorageSvc_M_JCOM_USB_HDD_M_Name(pszInput);
}
HERROR TRP140_Set_StorageSvc_M_JCOM_USB_HDD_M_Initialization(HCHAR *pszInput)
{
	return	set_StorageSvc_M_JCOM_USB_HDD_M_Initialization(pszInput);
}
HERROR TRP140_Set_StorageSvc_M_JCOM_USB_HDD_M_Diagnostics_DiagnosticsState(HCHAR *pszInput)
{
	return	set_StorageSvc_M_JCOM_USB_HDD_M_Diagnostics_DiagnosticsState(pszInput);
}


