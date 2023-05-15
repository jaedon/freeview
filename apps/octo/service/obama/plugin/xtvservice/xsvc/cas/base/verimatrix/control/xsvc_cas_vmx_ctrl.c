/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_cas_vmx_ctrl.c
	@brief

	Description:  									\n
	Module: 	OBAMA / TvService				\n

	Copyright (c) 2014 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
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
#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
#include "_xsvc_cas_vmx_iptv_main.h"
#include "vmx_vr_iptvh_mgr.h"
#include "vmx_vr_iptvh_api.h"
#elif defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
#include "_xsvc_cas_vmx_bc_main.h"
#endif
#include "_xsvc_cas_vmx_ctrl.h"



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
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

static HERROR vmxiptv_ctrl_SetOverrideRatingLevel(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR vmxiptv_ctrl_SetRatingLevel(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR vmxiptv_ctrl_GetRatingLevel(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR vmxiptv_ctrl_GetVersionString(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);



static const SvcCas_Ctrl_t s_astVmxIptvCtrlTable[eVmxIptvCtrl_MAX] =
{
	{
		eVmxIptvCtrl_SetOverrideRatingLevel,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		vmxiptv_ctrl_SetOverrideRatingLevel,	// Set
		NULL,	// Request
		NULL,	// Noti
	},

	{
		eVmxIptvCtrl_SetRatingLevel,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		vmxiptv_ctrl_SetRatingLevel,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eVmxIptvCtrl_GetRatingLevel,
		eCasCtrl_Get,
		NULL,	// Control
		vmxiptv_ctrl_GetRatingLevel,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eVmxIptvCtrl_GetVersionString,
		eCasCtrl_Get,
		NULL,	// Control
		vmxiptv_ctrl_GetVersionString,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	}
};

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
static inline VmxIptv_InParam_t *local_getInParam(SvcCas_CtrlParam_t *pstParam)
{
	return (VmxIptv_InParam_t *)pstParam->pvIn;
}

static inline VmxIptv_OutParam_t *local_getOutParam(SvcCas_CtrlParam_t *pstParam)
{
	return (VmxIptv_OutParam_t *)pstParam->pvOut;
}

// Local function
static HERROR vmxiptv_ctrl_SetOverrideRatingLevel(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	VMXIPTV_SetOverrideRatingLevelInParam_t *pstInParam = NULL;
	Vmx_ActionTable_t *pAct = NULL;
	Handle_t		hAction = HANDLE_NULL;
	HUINT8 ucCurrentStbRatingLevel;

	HxLOG_Print("[Enter]\n");

	if (ulControlType >= eVmxIptvCtrl_MAX)
	{
		HxLOG_Error("Error! Invalid control type(%d) \n", ulControlType);
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		HxLOG_Error("Error! NULL param \n");
		return ERR_FAIL;
	}

	pstInParam = (VMXIPTV_SetOverrideRatingLevelInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		HxLOG_Error("Error! local_getInParam() failed \n");
		return ERR_FAIL;
	}

	hAction = pstInParam->hAction;
	if(hAction == HANDLE_NULL)
	{
		hAction = xsvc_vmx_main_GetActionHandleFromActionId(pstInParam->nActionId);
	}

	pAct = xsvc_vmx_main_GetActionByActionHandle(hAction);
	if(pAct == NULL)
	{
		HxLOG_Error("Error! xsvc_vmx_main_GetActionByActionHandle() failed \n");
		return ERR_FAIL;
	}

	if(vmx_vr_iptvh_api_OverrideRatingLevel(pAct->pContext, &ucCurrentStbRatingLevel) != ERR_OK)
	{
		HxLOG_Error("Error! vmx_vr_iptvh_api_OverrideRatingLevel() failed \n");
	}

	return ERR_OK;
}

static HERROR vmxiptv_ctrl_SetRatingLevel(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	VMXIPTV_SetRatingLevelInParam_t *pstInParam = NULL;
	Vmx_ActionTable_t *pAct = NULL;

	HxLOG_Print("[Enter]\n");

	if (ulControlType >= eVmxIptvCtrl_MAX)
	{
		HxLOG_Error("\r\n");
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_FAIL;
	}

	pstInParam = (VMXIPTV_SetRatingLevelInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_FAIL;
	}

	pAct = xsvc_vmx_main_GetActionByActionHandle(pstInParam->hAction);
	if(pAct == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_FAIL;
	}

	if(vmx_vr_iptvh_api_SetRatingLevel(pAct->pContext, pstInParam->ucRatingLevel) != ERR_OK)
	{
		HxLOG_Error("\r\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR vmxiptv_ctrl_GetRatingLevel(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	VMXIPTV_GetRatingLevelOutParam_t *pstOutParam;
	Vmx_ActionTable_t *pAct = NULL;
	HUINT8 ucOride = 0;

	HxLOG_Print("[Enter]\n");

	if (ulControlType >= eVmxIptvCtrl_MAX)
	{
		HxLOG_Error("\r\n");
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_FAIL;
	}

	pstOutParam = (VMXIPTV_GetRatingLevelOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_FAIL;
	}

	pAct = xsvc_vmx_main_GetActionByActionHandle(pstOutParam->hAction);
	if(pAct == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_FAIL;
	}

	if(vmx_vr_iptvh_api_GetRatingLevel( pAct->pContext, &ucOride, &pstOutParam->ucRatingLevel, NULL) != ERR_OK)
	{
		HxLOG_Error("\r\n");
	}

	return ERR_OK;
}

static HERROR vmxiptv_ctrl_GetVersionString(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	VMXIPTV_GetVersionStringOutParam_t *pstOutParam;

	HxLOG_Print("[Enter]\n");

	if (ulControlType >= eVmxIptvCtrl_MAX)
	{
		HxLOG_Error("\r\n");
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_FAIL;
	}

	pstOutParam = (VMXIPTV_GetVersionStringOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_FAIL;
	}

	pstOutParam->pszVmxVersion = HLIB_STD_StrDup(vmx_vr_iptvh_api_GetVersionString());

	return ERR_OK;
}

HERROR vmxiptv_ctrl_RegisterCtrlList(void)
{
	HERROR	hErr;

	hErr = svc_cas_MgrSubInitControl(eDxCAS_GROUPID_VX);
	if (hErr == ERR_OK)
	{
		hErr = svc_cas_MgrSubRegisterControl(eDxCAS_GROUPID_VX, eVmxIptvCtrl_MAX, (SvcCas_Ctrl_t *)&s_astVmxIptvCtrlTable[0]);
	}

	return hErr;
}

// Global function
HERROR xsvc_vmx_ctrl_RegisterControlList(void)
{
	return vmxiptv_ctrl_RegisterCtrlList();
}


/*
// Global function
HERROR xsvc_xxx_DoSomething_Yyy( ... )
{
	...
}
*/
