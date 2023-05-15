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
#include "rema_cwmp.h"

#include "rema_DMO_Storage_LogicalVolume.h"

//#include "rema_module_service.h"

#define ___DEFINES___

#define ___MACROS___

#define ___TYPEDEF___

#define ___STATIC_VARIABLES___

#define ___INTERNAL_FUNCTIONS___
#define ___PUBLIC_FUNCTIONS___
#if 0
hCwmpResult_e DMO_Update_StorageService_LogicalVolume(HCWMP_UPDATER_PARAM)
{
	hCwmpResult_e eRs = ehCwmp_R_INTERNAL_ERROR;
	HUINT32 unMax  = 0;
	int i = 0;
	void *pData =NULL; 
	
	REM_FUNC_ENTER;
	
	if(ERR_OK ==  rema_MOD_Service_GetMax_Storage_LogicalVolume(pvCtx, &unMax))
	{

		if(rema_cwmp_GetTotalMultiChildren(pstDMManager, nPd) != unMax)
		{
			RemLOG_Debug("Do add");
			hCwmpDMManager_DelMultiChildren(pstDMManager, nPd);
			for(i =0 ; i< unMax ; i++)
			{
				pData = rema_MOD_Service_GetCTX_Storage_LogicalVolume(i);
				rema_cwmp_AddMultiObject(pstDMManager, nPd, i, pData);
			}
		}
		eRs = ehCwmp_R_OK;
	}

	REM_FUNC_LEAVE;
	return eRs;
}
hCwmpResult_e DMO_Get_StorageService_LogicalVolume(HCWMP_GETTER_PARAM)
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
		if(!rema_MOD_Service_GetStr_Storage_LogicalVolume(pvCtx, pcTagName, pszBuf, nBufSize))
		{
			*pnOutLen = HxSTD_StrLen(pszBuf);
			eRs = ehCwmp_R_OK;
		}

	}
	else if(eParamType == ehCwmp_PTYPE_UINT)
	{
		
		if(!rema_MOD_Service_GetUINT_Storage_LogicalVolume(pvCtx, pcTagName, &unValue))
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

hCwmpResult_e DMO_Get_LogicalVolume_M_UsedSpace(HCWMP_GETTER_PARAM)
{
	RemLOG_Debug("[%s] TBD\n", pszParamName);
	return ehCwmp_R_OK;
}

hCwmpResult_e DMO_Set_LogicalVolume_M_Capacity(HCWMP_SETTER_PARAM)
{
	RemLOG_Debug("[%s] TBD\n", pszParamName);
	return ehCwmp_R_OK;
}
#endif
#define ___EXTERNAL_FUNCTIONS___

#define _____END_OF_FILE_________

