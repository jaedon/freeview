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
	@file     xsvc_cas_vmx_ui.c
	@brief    xsvc_cas_vmx_ui.c (Verimatrix UI)

	Description:   \n
	Module: MW/CAS/ VERIMATRIX \n
	Remarks :\n

	Copyright (c) 2015 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
#include "_xsvc_cas_vmx_iptv_main.h"
#include "_xsvc_cas_vmx_iptvh_ui.h"
#elif defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
#include "_xsvc_cas_vmx_bc_main.h"
#include "_xsvc_cas_vmx_bc_ui.h"
#endif
#include "_xsvc_cas_vmx_os.h"

#include <isosvc.h>
/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/*
typedef enum
{
	eSvcXxx_Type,

} xsvcXxx_Type_e;

typedef  struct
{
	...
} xsvcXxx_Type_t;
*/


/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
/*
// Local function
STATIC HERROR xsvc_xxx_DoSomething_Yyy( ... )
{
	...
}
*/

/*
// Global function
HERROR xsvc_xxx_DoSomething_Yyy( ... )
{
	...
}
*/

// send to core
HERROR xsvc_cas_vmx_UpdateCasMessage(Handle_t hAction,SvcCas_MsgType_e eMsgType, DxCAS_GroupId_e eCasId, void *pMessage, SvcCas_ShowType_e eShowType)
{
	HERROR hErr = ERR_FAIL;
	HxLOG_Print("[Enter]\n");

	hErr = svc_cas_MgrSubUpdateCasMessage(hAction, eMsgType, eCasId, pMessage, eShowType);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	HxLOG_Print("[Exit]\n");
	return ERR_OK;
}

#if defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
// receive from core
HERROR xsvc_cas_vmx_SetPinVerificationToBC(VmxBc_VerificationPin_t *pVPin)
{
	HxLOG_Print("[Enter]\n");
	if(pVPin == NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}
	HxLOG_Print("[Exit]\n");
	return vmx_vr_bc_ui_SetPinVerificationToBC(pVPin);
}

#if defined(CONFIG_DEBUG)
void xsvc_cas_vmx_DebugPrintForRequest(VmxBc_Requst_t *pstRequest, HUINT8 *pMsg)
{
#if defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
	vmx_ui_DebugPrintforRequest(pstRequest, pMsg);
#endif
}
#endif
#endif
