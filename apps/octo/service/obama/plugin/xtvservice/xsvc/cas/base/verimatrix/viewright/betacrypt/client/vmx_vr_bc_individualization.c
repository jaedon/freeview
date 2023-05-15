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
	@file     vmx_vr_bc_individualization.c
	@brief    vmx_vr_bc_individualization.c (Verimatrix BC INDIVIDUALIZATION)

	Description:   \n
	Module: MW/CAS/ VERIMATRIX \n
	Remarks :\n

	Copyright (c) 2015 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

#include "_xsvc_cas_vmx_bc_main.h"
#include "_xsvc_cas_vmx_os.h"
#include "vmx_vr_bc_mgr.h"
#include "vmx_vr_bc_api.h"
#include "vmx_vr_bc_Individualization.h"

#define MAX_INDIV_DATA_LEN		(48)
#define MAX_INDIV_BOXID_LEN		(8)

typedef struct
{
	HUINT8 ucData[MAX_INDIV_DATA_LEN];
	HUINT16 usLen;
	HUINT8 ucBoxSerNo[MAX_INDIV_BOXID_LEN];
	HUINT8 ucManuId[8];
	HUINT8 ucManuData[256];
	HUINT8 ucProvId[8];
	HUINT8 ucProvData[256];	
} VmxBc_Indiv_t;

STATIC VmxBc_Indiv_t stIndiv;

#if defined(CONFIG_DEBUG)
void vmx_vr_bc_Indiv_StartPrint(void)
{
	if(g_VmxBcIndiv_Level != 0xff)
		return;
	
	HxLOG_Print ("+++++++++ Input Indiv Print +++++++++++\n");
	HxLOG_Print ("+ Data Length = %d\n", stIndiv.usLen);	
	xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_INDIV, "Data", stIndiv.ucData, MAX_INDIV_DATA_LEN);
	xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_INDIV, "BoxSerNo", stIndiv.ucBoxSerNo, MAX_INDIV_BOXID_LEN);
	xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_INDIV, "ManuId", stIndiv.ucManuId, 8);
	xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_INDIV, "ManuData", stIndiv.ucManuData, 256);
	xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_INDIV, "ProvId", stIndiv.ucProvId, 8);
	xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_INDIV, "ProvData", stIndiv.ucProvData, 256);	
}

void vmx_vr_bc_Indiv_EndPrint(HUINT8 *pucIndivData, HUINT16 *pusLen )
{
	if(g_VmxBcIndiv_Level != 0xff)
		return;
#if 0
	HxLOG_Print ("+++++++++ Output Indiv Print +++++++++++\n");
	HxLOG_Print ("+ Data Length = %d\n", stIndiv.usLen);
	xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_INDIV, "Data", stIndiv.ucData, MAX_INDIV_DATA_LEN);
	xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_INDIV, "BoxSerNo", stIndiv.ucBoxSerNo, MAX_INDIV_BOXID_LEN);
	xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_INDIV, "ManuId", stIndiv.ucManuId, 8);
	xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_INDIV, "ProvId", stIndiv.ucManuData, 256);	
	xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_INDIV, "ProvId", stIndiv.ucProvId, 8);
	xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_INDIV, "ProvData", stIndiv.ucProvData, 256);	
#endif
	HxLOG_Print ("\n+++++++++ Output WriteData Print +++++++++++\n");
	HxLOG_Print ("WriteData Len = %d\n", *pusLen);
	xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_INDIV, "WriteData", pucIndivData, *pusLen);
}
#endif

void vmx_vr_bc_Indiv_Start(void)
{
	HxLOG_Print(DBG_PRINT, ("[Enter]\n");

	VMX_memset(&stIndiv, 0, sizeof(stIndiv));

	if(vmx_vr_bc_mgr_GetBoxId(stIndiv.ucBoxSerNo) != ERR_OK)
	{
		HxLOG_Error(DBG_ASSERT, ("Get Maxid - Fail\n");
		return;
	}
	xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_INDIV, "BoxSerNo", stIndiv.ucBoxSerNo, MAX_INDIV_BOXID_LEN);
	if(vmx_vr_bc_indiv_GetVendorId(stIndiv.ucManuId) != ERR_OK)
	{
		HxLOG_Error(DBG_ASSERT, ("Get ucManuId - Fail\n");
		return;
	}
	xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_INDIV, "ManuId", stIndiv.ucManuId, 8);
	if(vmx_vr_bc_indiv_GetVendorData(stIndiv.ucManuData) != ERR_OK)
	{
		HxLOG_Error(DBG_ASSERT, ("Get ucManuData - Fail\n");
		return;
	}	
	xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_INDIV, "ManuData", stIndiv.ucManuData, 256);
	if(vmx_vr_bc_indiv_GetProviderId(stIndiv.ucProvId) != ERR_OK)
	{
		HxLOG_Error(DBG_ASSERT, ("Get ucProvId - Fail\n");
		return;
	}	
	xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_INDIV, "ProvId", stIndiv.ucProvId, 8);
	if(vmx_vr_bc_indiv_GetProviderData(stIndiv.ucProvData) != ERR_OK)
	{
		HxLOG_Error(DBG_ASSERT, ("Get ucProvData - Fail\n");
		return;
	}
	xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_INDIV, "ProvData", stIndiv.ucProvData, 256);
	stIndiv.usLen= MAX_INDIV_DATA_LEN;

	if(vmx_vr_bc_api_InitNScComm(stIndiv.ucData, &stIndiv.usLen, stIndiv.ucBoxSerNo, stIndiv.ucManuId, stIndiv.ucManuData, stIndiv.ucProvId, stIndiv.ucProvData) != ERR_OK)
	{
		HxLOG_Error(DBG_ASSERT, ("Call vmx_vr_bc_api_InitNScComm - Fail\n");
	}

//#if defined(CONFIG_DEBUG)
	//vmx_vr_bc_Indiv_StartPrint();
	xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_INDIV, "NSC48Byte", stIndiv.ucData, stIndiv.usLen);
//#endif
	// TODO_VMX : Client로 전달..
	HxLOG_Print(DBG_PRINT, ("[Exit]\n");
}

// TODO_VMX : call 해주어한다.
void vmx_vr_bc_indiv_WriteIndivData(HUINT8 *pucIndivData, HUINT16 *pusLen )
{
	HxLOG_Print(DBG_PRINT, ("[Enter]\n");	
	if(pucIndivData == NULL || pusLen == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("vmx_vr_bc_indiv_WriteIndivData - Fail\n");
		return;
	}

#if defined(CONFIG_DEBUG)
	//vmx_vr_bc_Indiv_EndPrint(pucIndivData, pusLen);
	xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_INDIV, "STB Data", pucIndivData, *pusLen);
#endif

	if(vmx_vr_bc_api_InitNSc(pucIndivData, pusLen) != ERR_OK)
	{
		HxLOG_Error(DBG_ASSERT, ("vmx_vr_bc_api_InitNSc - Fail\n");
	}
	else
	{
		HxLOG_Print(DBG_PRINT, ("Individualization ========================> OKl\n");
	}

	HxLOG_Print(DBG_PRINT, ("[Exit]\n");
}

