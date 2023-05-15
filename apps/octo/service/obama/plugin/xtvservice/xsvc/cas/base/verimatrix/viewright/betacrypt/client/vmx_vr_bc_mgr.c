/* (c) 2011-2015 Humax Co., Ltd. 
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
	@file     vmx_vr_bc_mgr.c
	@brief    vmx_vr_bc_mgr.c (Verimatrix BC MGR)

	Description:   \n
	Module: MW/CAS/ VERIMATRIX \n
	Remarks :\n

	Copyright (c) 2015 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

//#include <stdarg.h>
//#include <stdio.h>
//#include <stdlib.h>

#include "bc_consts.h"

#include <osd.h>
#include "_xsvc_cas_vmx_bc_main.h"
#include "_xsvc_cas_vmx_os.h"
#include "_xsvc_cas_vmx_bc_ui.h"
#include "vmx_vr_bc_mgr.h"
#include "vmx_vr_bc_api.h"
#include "vmx_vr_bc_Individualization.h"

#define MAX_VERSION_STRING_LEN		(32)	// normal : max 20 bytes, advanced security : max 32bytes
#define MAX_DATE_STRING_LEN			(20)
#define MAX_TIME_STRING_LEN			(20)
#define MAX_BOXID_STRING_LEN			(8)

typedef struct
{
	HUINT8 ucVersion[MAX_VERSION_STRING_LEN];
	HUINT8 ucDate[MAX_VERSION_STRING_LEN];
	HUINT8 ucTime[MAX_VERSION_STRING_LEN];
} VmxBc_VersionInfo_t;

typedef struct
{
	VmxBc_VersionInfo_t 	stVer;
	HUINT8					ucBoxId[MAX_BOXID_STRING_LEN];
	HUINT16					ulCaSystemId;
} VmxBc_Information_t;

STATIC HULONG s_ulVmxDvbTid;

STATIC VmxBc_Information_t	stVmxBcInfo;

STATIC VmxBc_Information_t * bc_mgr_GetVmxInfo(void)
{
	return &stVmxBcInfo;
}

STATIC HERROR bc_mgr_InitContext(void)
{
	HUINT32 nErr;	
	HINT16 ret_BcInit = k_BcSuccess;
	HINT16 ret_CheckNSc = k_BcSuccess;
	VmxBc_Information_t *pVmxBcInfo = bc_mgr_GetVmxInfo();

	HxLOG_Print(DBG_PRINT, ("[Enter]\n");

	if(pVmxBcInfo == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("pVmxBcInfo is null\n");
		return ERR_FAIL;
	}

	VMX_memset(pVmxBcInfo, 0, sizeof(VmxBc_Information_t));

	ret_BcInit = vmx_vr_bc_api_BcInit();
	HxLOG_Print(DBG_PRINT, ("BC-Init :%d\n", ret_BcInit);

  	vmx_vr_bc_api_GetVersion(&pVmxBcInfo->stVer.ucVersion[0], &pVmxBcInfo->stVer.ucDate[0], &pVmxBcInfo->stVer.ucTime[0]);
	HxLOG_Print(DBG_PRINT, ("BC : Ver(%s), Date(%s), Time(%s)\n",pVmxBcInfo->stVer.ucVersion, pVmxBcInfo->stVer.ucDate, pVmxBcInfo->stVer.ucTime);

	if(ret_BcInit == k_BcNSc)		/* NSc로 성공적으로 init되는 경우 */
	{
		HxLOG_Print(DBG_PRINT, ("BC_Init() : NSc solution is initialised successfully - Ok\n");
	}
	else if(ret_BcInit == k_BcSuccess) // NSc로 부팅이 안된상태에서 library init가 성공적으로 동작된 경우.
	{
		HxLOG_Print(DBG_PRINT, ("BC_Init() : No NSc solution is initialised successfully - Ok\n");

#ifdef BC_NSC_INCLUDED
		ret_CheckNSc = vmx_vr_bc_api_CheckNSc();
	
		if(ret_CheckNSc == k_BcNotSupported)
		{
			HxLOG_Print(DBG_PRINT, ("BC_CheckNSc() : no NSc implementation exists\n");
		}
		else if(ret_CheckNSc == k_BcSuccess) // NSc가 library에 include 되고 이미 individualization 되어 있다. // Field에서는 이상태이어야한다.
		{
			HxLOG_Print(DBG_PRINT, ("BC_CheckNSc() : the NSc implementation exists and the set top box is already individualized\n");
		}
		else if(ret_CheckNSc == k_BcError)
		{
			HxLOG_Print(DBG_PRINT, ("BC_CheckNSc() : the NSc implementation exists and the set top box is not yet individualized\n");

//			vmx_vr_bc_Indiv_Start();
		}
		else
		{
			HxLOG_Error(DBG_ASSERT, ("BC_CheckNSc() = %d : Fail Fail Fail Fail Fail Fail\n", ret_CheckNSc);
		}
#endif
	}
	else if(ret_BcInit == k_BcError)
	{
		pVmxBcInfo->ulCaSystemId = k_CaIdNotAvailable;
		HxLOG_Error(DBG_ASSERT, ("BC_Init() : - Fail : SC_IOCTL() or initialising the library internals failed\n");
	}
	else
	{
		pVmxBcInfo->ulCaSystemId = k_CaIdNotAvailable;
		HxLOG_Error(DBG_ASSERT, ("BC_Init() : - Fail Fail Fail Fail Fail Fail\n");
	}

	if(vmx_vr_bc_api_SetIoctlNotify() != ERR_OK)
	{
		HxLOG_Error(DBG_ASSERT, ("BC_Ioctl() : - Fail\n");
	}

	vmx_vr_bc_api_InitWindow(OSD_SCREEN_WIDTH, OSD_SCREEN_HEIGHT, NULL);

	nErr = VK_TASK_Create((void*)vmx_vr_bc_api_Task, VMXBC_BCLIB_TASK_PRIORITY, VMXBC_BCLIB_STACK_SIZE, VMXBC_BCLIB_TASK_NAME, NULL, &s_ulVmxDvbTid, 0);
	
	if (!nErr)
	{
		nErr = VK_TASK_Start(s_ulVmxDvbTid);
	}
	else
	{
		HxLOG_Error(DBG_ASSERT, ("VK_TASK_Create : fail\n");
		return ERR_FAIL;
	}

 	HxLOG_Print(DBG_PRINT, ("[Exit]\n");

	return ERR_OK;
}

STATIC void bc_mgr_SetCaSystemId(HUINT16 usCaSystemId)
{
	VmxBc_Information_t *pVmxBcInfo = bc_mgr_GetVmxInfo();

	if(pVmxBcInfo == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("pVmxBcInfo is null\n");
		return;
	}

	pVmxBcInfo->ulCaSystemId = usCaSystemId;
}

STATIC void bc_mgr_SetBoxId(HUINT8* pusBoxId)
{
	VmxBc_Information_t *pVmxBcInfo = bc_mgr_GetVmxInfo();

	if(pVmxBcInfo == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("pVmxBcInfo is null\n");
		return;
	}

	VMX_memcpy(&pVmxBcInfo->ucBoxId[0], pusBoxId, sizeof(HUINT8)*MAX_BOXID_STRING_LEN);

	HxLOG_Print(DBG_PRINT, ("Chipset BoxId = { %x %x %x %x %x %x %x %x }\n",  
		pVmxBcInfo->ucBoxId[0], pVmxBcInfo->ucBoxId[1], pVmxBcInfo->ucBoxId[2], pVmxBcInfo->ucBoxId[3],
		pVmxBcInfo->ucBoxId[4], pVmxBcInfo->ucBoxId[5], pVmxBcInfo->ucBoxId[6], pVmxBcInfo->ucBoxId[7]);
}

HERROR vmx_vr_bc_mgr_GetCaSystemId(HUINT16* pusCaSystemId)
{
	VmxBc_Information_t *pVmxBcInfo = bc_mgr_GetVmxInfo();
	if(pVmxBcInfo == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("pVmxBcInfo is null\n");
		return ERR_FAIL;
	}
#ifdef VMX_TEST
	* pusCaSystemId =  0x1724;//pVmxBcInfo->ulCaSystemId;
	return ERR_OK;
#endif
	* pusCaSystemId =  pVmxBcInfo->ulCaSystemId;
	return ERR_OK;
}

HERROR vmx_vr_bc_mgr_GetBoxId(HUINT8* pusBoxId)
{
	VmxBc_Information_t *pVmxBcInfo = bc_mgr_GetVmxInfo();
	if(pVmxBcInfo == NULL || pusBoxId == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("pVmxBcInfo is null\n");
		return ERR_FAIL;
	}

	VMX_memcpy(pusBoxId, pVmxBcInfo->ucBoxId, MAX_BOXID_STRING_LEN);
	return ERR_OK;
}

void vmx_vr_bc_mgr_SetCaSystemId(HUINT16 usCaSystemId)
{
	bc_mgr_SetCaSystemId(usCaSystemId);
}

void vmx_vr_bc_mgr_SetBoxId(HUINT8* pusBoxId)
{
	bc_mgr_SetBoxId(pusBoxId);
}

HERROR vmx_vr_bc_mgr_InitContext(void)
{
	if(vmx_vr_ui_Init() != ERR_OK)
	{
		HxLOG_Error("vmx_vr_ui_Init() : error\n");
	}

	if(vmx_vr_bc_psi_mgr_Init() != ERR_OK)
	{
		HxLOG_Error("vmx_vr_bc_psi_mgr_Init() : error\n");
	}	

	if(vmx_vr_bc_ecm_Init() != ERR_OK)
	{
		HxLOG_Error("vmx_vr_bc_ecm_Init() : error\n");
	}

	if(vmx_vr_bc_emm_Init() != ERR_OK)
	{
		HxLOG_Error("vmx_vr_bc_emm_Init() : error\n");
	}
	
	if(bc_mgr_InitContext() != ERR_OK)
	{
		HxLOG_Error("bc_mgr_InitContext() : error\n");
	}

	return ERR_OK;
}


