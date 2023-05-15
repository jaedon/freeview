/**************************************************************
*	@file	rema.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			rema
**************************************************************/

/*
* ¨Ï 2011-2012 Humax Co., Ltd.
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
#include <oapi.h>

#include <hapi.h>

#include "rema_DMO_Storage_PhysicalMedium.h"

#include "rema_cwmp.h"

//#include "rema_module_service.h"


#define ___DEFINES___

#define ___MACROS___

#define ___TYPEDEF___

#define ___STATIC_VARIABLES___

#define ___INTERNAL_FUNCTIONS___
#define ___PUBLIC_FUNCTIONS___

HERROR	DMO_Print_PhysicalMedium_StorageInfo(DxStorage_Info_t *pstInfo)
{
	HERROR hErr = ERR_OK;
	HINT32 i = 0;

	RemLOG_Debug("================ Storage Info ================ \n");
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
	
	RemLOG_Debug("\n--------Partition Info--------\n");
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
	RemLOG_Debug("===========================================\n");

	REM_LEAVE(hErr);
	
}

#if 0
hCwmpResult_e DMO_Update_StorageService_PhysicalMedium(HCWMP_UPDATER_PARAM)
{
	hCwmpResult_e eRs = ehCwmp_R_INTERNAL_ERROR;
	HUINT32 unMax  = 0;
	int i = 0;
	void *pData =NULL; 
	
	REM_FUNC_ENTER;
	
	if(ERR_OK ==  rema_MOD_Service_GetMax_Storage_PhysicalMedium(pvCtx, &unMax))
	{

		if(rema_cwmp_GetTotalMultiChildren(pstDMManager, nPd) != unMax)
		{
			RemLOG_Debug("Do add");
			hCwmpDMManager_DelMultiChildren(pstDMManager, nPd);
			for(i =0 ; i< unMax ; i++)
			{
				pData = rema_MOD_Service_GetCTX_Storage_PhysicalMedium(i);
				rema_cwmp_AddMultiObject(pstDMManager, nPd, i, pData);
			}
		}
		eRs = ehCwmp_R_OK;
	}

	REM_FUNC_LEAVE;
	return eRs;
}
hCwmpResult_e DMO_Get_StorageService_PhysicalMedium(HCWMP_GETTER_PARAM)
{
	HUINT32 unValue = 0;
	hCwmpResult_e eRs = ehCwmp_R_INTERNAL_ERROR;
	HCHAR *pcTagName=NULL; 

	pcTagName = HxSTD_StrRChar(pszParamName, '.');
	if(pcTagName)
		pcTagName++;
	RemLOG_Debug("%s", pcTagName);
	if(eParamType == ehCwmp_PTYPE_STRING)
	{
		if(!rema_MOD_Service_GetStr_Storage_PhysicalMedium(pvCtx, pcTagName, pszBuf, nBufSize))
		{
			*pnOutLen = HxSTD_StrLen(pszBuf);
			eRs = ehCwmp_R_OK;
		}

	}
	else if(eParamType == ehCwmp_PTYPE_UINT)
	{
		
		if(!rema_MOD_Service_GetUINT_Storage_PhysicalMedium(pvCtx, pcTagName, &unValue))
		{
			*pnOutLen = hCwmpUtil_UINT32ToStr(unValue, pszBuf, nBufSize);
			eRs = ehCwmp_R_OK;
		}
		
		
	}
	else
	{
	}
	return eRs;
}

#endif
#define ___EXTERNAL_FUNCTIONS___

#define _____END_OF_FILE_________

