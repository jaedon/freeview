/*
 * (c) 2011-2015 Humax Co., Ltd.
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
	@file     xsvc_cas_vmx_main.c
	@brief    xsvc_cas_vmx_main.c (Verimatrix MAIN)

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
#include "vmx_vr_iptvh_mgr.h"
#include "vmx_vr_iptvh_connection.h"
#elif defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
#include "_xsvc_cas_vmx_bc_main.h"
#include "_xsvc_cas_vmx_bc_sc.h"
#include "vmx_vr_bc_mgr.h"
#endif
#include "_xsvc_cas_vmx_os.h"
#include "_xsvc_cas_vmx_dmx.h"
#include "_xsvc_cas_vmx_dscr.h"
#include "_xsvc_cas_vmx_psi.h"
#include "_xsvc_cas_vmx_action.h"
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
STATIC Vmx_Instance_t s_stVmxInst;

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

STATIC Vmx_Instance_t * vmx_main_GetInstance(void);

STATIC void vmx_main_PrintfDump(char flag, char *title, void *buf, unsigned int len)
{
#if defined(CONFIG_DEBUG)
	HUINT16 *p = (HUINT16 *)buf;
	int i;

	if(flag==0)
		return;

	if (title)
	{
		HxLOG_Print("\n\t/* %s's Buffer Length = %d */ ", title, len);
		HxLOG_Print("\n\t%s[%d] = \n\t{ ", title, len);
	}

	for (i = 0; i < len; i++)
	{
		if (i % 16 == 0 )
		{
		HxLOG_Print("\n\t\t");
		}
		if(i==len-1)
			HxLOG_Print("0x%02x", p[i]);
//			HxLOG_Print("%02x", p[i]);
		else
			HxLOG_Print("0x%02x, ", p[i]);
//			HxLOG_Print("%02x ", p[i]);
	}

	HxLOG_Print("\n\t}\n\n");
#endif
}

STATIC void vmx_main_PrintContext(void)
{
	HUINT16 i = 0;
	Vmx_ActionTable_t *pAct = NULL;
	Vmx_Instance_t *pstInst = xsvc_vmx_main_GetInstance();

	if(pstInst == NULL)
	{
		HxLOG_Error("\r\n");
		return;
	}

	for (i = 0; i < VMX_CAS_MAX_NUM_OF_ACTION; i++)
	{
		pAct = &pstInst->stActionList[i];
		if(pAct == NULL)
		{
			continue;
		}

		HxLOG_Print("nActionId(%d)\n", i);
		HxLOG_Print("\t pContext(0x%08x)\n", pAct->pContext);
#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
		HxLOG_Print("\t ucVmxServiceIdx(0x%x)\n", pAct->ucVmxServiceIdx);
#endif
	}
}


STATIC HERROR vmx_main_InitContext(void)
{
	HUINT16 i = 0;
	Vmx_ActionTable_t *pAct = NULL;
	Vmx_Instance_t *pstInst = vmx_main_GetInstance();

	if(pstInst == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_FAIL;
	}

	VMX_memset(pstInst, 0x00, sizeof(Vmx_Instance_t));

	for (i = 0; i < VMX_CAS_MAX_NUM_OF_ACTION; i++)
	{
		pAct = &pstInst->stActionList[i];

		if(pAct == NULL)
			continue;

		pAct->bConnect						= FALSE;
		pAct->hAction 						= HANDLE_NULL;
		pAct->hSvc 							= HANDLE_NULL;

#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
		VMX_VR_IPTVH_MGR_InitContext(pAct);
#elif defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
		pAct->ucVmxServiceIdx 				= VMX_INVALID_SERVICE_IDX;
#endif
	}

	return ERR_OK;
}

STATIC Vmx_Instance_t * vmx_main_GetInstance(void)
{
	return &s_stVmxInst;
}

#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
STATIC HUINT8 vmx_main_GetServiceIdx(HUINT32 ulActionId)
{
	HUINT32 			 i = 0;
	Vmx_Instance_t 		*pstInst = vmx_main_GetInstance();
	HUINT8  			 ucVmxSvcIdx = VMX_INVALID_SERVICE_IDX;

	if(pstInst == NULL)
	{
		HxLOG_Error("vmx_main_GetInstance() failed! \n");
		return VMX_INVALID_SERVICE_IDX;
	}

#if defined(CONFIG_PROD_YSR2000) || defined(CONFIG_PROD_YS1000)
	// 현재는 YSR-2000을 기준으로 작성됨 - 10개 session 운용 가능 (YS-1000도 동일한 cas lib를 사용해서 10개 session 가능)
	// action Id가 view나 rec에 해당하면 0x40 + action id를 svcIdx로 준다 -> 디버깅 용이성
	if (ulActionId <= eActionId_REC_LAST)
	{
		ucVmxSvcIdx = pstInst->stActionList[ulActionId].ucVmxServiceIdx;
		if ((ucVmxSvcIdx < VMX_CAS_VALID_SVCIDX_START) || (ucVmxSvcIdx > VMX_CAS_VALID_SVCIDX_END))
		{
			ucVmxSvcIdx = VMX_CAS_VALID_SVCIDX_START + ulActionId;

			return ucVmxSvcIdx;
		}
		else
		{
			// stop이 제대로 안된 경우.. 이런 경우가 발생하면 안된다..!!
			HxLOG_Error("Verimatrix service index alloc error!! aid(%d) curSvcId(0x%X) \n", ulActionId, ucVmxSvcIdx);
			return VMX_INVALID_SERVICE_IDX;
		}
	}

	// 기타 action id(streaming or bg)는 (0x40 + eActionId_REC_LAST) 이후부터 순서대로 할당한다
	for (i = eActionId_REC_LAST; i < VMX_MAX_NUM_OF_SERVICE_IDX; i++)
	{
		ucVmxSvcIdx = pstInst->stActionList[i].ucVmxServiceIdx;
		if ((ucVmxSvcIdx < VMX_CAS_VALID_SVCIDX_START) || (ucVmxSvcIdx > VMX_CAS_VALID_SVCIDX_END))
		{
			// 찾았다~
			ucVmxSvcIdx = VMX_CAS_VALID_SVCIDX_START + i;
			return ucVmxSvcIdx;
		}
	}
#else
	// session수가 충분하지 않다면 순서대로 할당해 사용해야 한다
	// 해당 action에 해당되는 svcIdx가 이미 사용중이므로 다른 것을 찾는다
	{
		Vmx_ActionTable_t 	*pTmpAct = NULL;
		HUINT8				 aucUseFlag[VMX_MAX_NUM_OF_SERVICE_IDX];

		HxSTD_MemSet(aucUseFlag, 0x00, sizeof(aucUseFlag));
		for (i = 0; i < VMX_CAS_MAX_NUM_OF_ACTION; i++)
		{
			pTmpAct = &pstInst->stActionList[i];
			if(pTmpAct == NULL)
			{
				HxLOG_Error("Error! Null action list!! idx(%d)\n", i);
				continue;
			}

			if ((pTmpAct->ucVmxServiceIdx < VMX_CAS_VALID_SVCIDX_START)
				|| (pTmpAct->ucVmxServiceIdx > VMX_CAS_VALID_SVCIDX_END))
			{
				continue;
			}

			aucUseFlag[pTmpAct->ucVmxServiceIdx - VMX_CAS_VALID_SVCIDX_START] = TRUE;
		}

		for (i = 0; i < VMX_MAX_NUM_OF_SERVICE_IDX; i++)
		{
			if (aucUseFlag[i] == FALSE)
			{
				// found unused service index
				ucVmxSvcIdx = VMX_CAS_VALID_SVCIDX_START + i;
				return ucVmxSvcIdx;
			}
		}
	}
#endif

	HxLOG_Error("Verimatrix service index alloc error!! aid(%d) \n", ulActionId);
	return VMX_INVALID_SERVICE_IDX;
}


#elif defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
STATIC HERROR vmx_main_GetServiceIdx(Vmx_ActionTable_t *pAct)
{
	int i = 0, j = 0;
	HUINT8 ucVmxServiceIdx = VMX_INVALID_SERVICE_IDX;
	Vmx_Instance_t *pstInst = vmx_main_GetInstance();
	Vmx_ActionTable_t *pTmpAct = NULL;

	if(pAct == NULL || pstInst == NULL)
	{
		HxLOG_Error("null\n");
		return ERR_FAIL;
	}

	for (i = 0; i < VMX_MAX_NUM_OF_SERVICE_IDX; i++)
	{
		for(j = 0; j < VMX_CAS_MAX_NUM_OF_ACTION; j++)
		{
			pTmpAct = &pstInst->stActionList[i];
			if(pTmpAct == NULL)
			{
				continue;
			}

			if(pTmpAct->ucVmxServiceIdx == i)
			{
				break;
			}
		}

		if(j >= VMX_CAS_MAX_NUM_OF_ACTION)
		{
			ucVmxServiceIdx = i;
			break;
		}
	}

	if(i >= VMX_MAX_NUM_OF_SERVICE_IDX)
	{
		HxLOG_Error("vmx's service index > max service index\n");
		return ERR_FAIL;
	}

	pAct->ucVmxServiceIdx = ucVmxServiceIdx;

	return ERR_OK;
}

STATIC Handle_t vmx_main_GetActionHandleFromServiceIndex(HUINT8 usFilterId) // FilterId == ServiceIndex
{
	int i = 0;
	Vmx_ActionTable_t *pAct = NULL;
	Vmx_Instance_t *pstInst = vmx_main_GetInstance();

	for (i = 0; i < VMX_CAS_MAX_NUM_OF_ACTION; i++)
	{
		pAct = &pstInst->stActionList[i];
		if(pAct == NULL)
		{
			continue;
		}

		if(pAct->ucVmxServiceIdx == usFilterId)
		{
			return pAct->hAction;
		}
	}

#if 0//defined(CONFIG_DEBUG)
	{
#if defined(FOR_CMD_TEST)
		#if 1 // test only TODO_VMX
		Handle_t hAction;
		hAction = MWC_RM_GetActionHandle(eActionId_VIEW_0/*AP_AM_GetMainActionId()*/);
	//	if(hAction != ERR_FAIL)
		{
			return  hAction;
		}
		#endif
#endif
	}
#endif

	HxLOG_Error("[Exit]\n");

	return HANDLE_NULL;
}

STATIC  Handle_t vmx_main_GetstbHandleFromServiceIndex(HUINT8 usServiceIdx)
 {
 	Vmx_Instance_t *pInst = NULL;
	Handle_t hAction = HANDLE_NULL;
	HINT32 nActionId;


	hAction = vmx_main_GetActionHandleFromServiceIndex(usServiceIdx);
	if(hAction == HANDLE_NULL)
	{
		HxLOG_Error("[Fail]\n");
		return HANDLE_NULL;
	}

	nActionId = GET_ACTION_ID(hAction);
	pInst = xsvc_vmx_main_GetInstance();
	if(pInst == NULL)
	{
		HxLOG_Error("[Fail]\n");
		return HANDLE_NULL;
	}

	return GET_STBHANDLE(hAction, pInst->stActionList[nActionId].hSvc);
 }
#endif

STATIC Handle_t vmx_main_GetActionHandleFromStbHdl(Handle_t hStbHandle)
{
	HINT32 nActionId = GET_ACTION_ID_FROM_STBHANDLE(hStbHandle);
	Vmx_ActionTable_t *pAct = NULL;
	Vmx_Instance_t *pstInst = vmx_main_GetInstance();

	if(nActionId >= VMX_CAS_MAX_NUM_OF_ACTION)
	{
		HxLOG_Error("Error! Invalid nActionId(%d)\n", nActionId);
		return HANDLE_NULL;
	}

	if(pstInst == NULL)
	{
		HxLOG_Error("Error! vmx_main_GetInstance() failed. \n");
		return HANDLE_NULL;
	}

	if (hStbHandle == HANDLE_NULL)
	{
		HUINT16 i;

		HxLOG_Error("Error! invalid hStbHandle!!Try recovery. \n");

		for (i = 0; i < VMX_CAS_MAX_NUM_OF_ACTION; i++)
		{
			pAct = &pstInst->stActionList[i];
			if(pAct == NULL)
			{
				continue;
			}

			if (pAct->eActType == eCAS_ActionType_Live)
			{
				return pAct->hAction;
			}
		}
	}

	return pstInst->stActionList[nActionId].hAction;
}

STATIC Vmx_ActionTable_t * vmx_main_GetActionByActionHandle(Handle_t hAction)
{
	HUINT16 i = 0;
	Vmx_ActionTable_t *pAct = NULL;
	Vmx_Instance_t *pstInst = vmx_main_GetInstance();

	if(pstInst == NULL)
	{
		HxLOG_Error("\r\n");
		return NULL;
	}

	for (i = 0; i < VMX_CAS_MAX_NUM_OF_ACTION; i++)
	{
		pAct = &pstInst->stActionList[i];
		if(pAct == NULL)
		{
			continue;
		}
		if (pAct->hAction == hAction)
		{
			return pAct;
		}
	}

	return NULL;
}

STATIC Handle_t vmx_main_GetActionHandleFromActionId(HUINT32 nActionId)
{
	Vmx_Instance_t *pstInst = vmx_main_GetInstance();

	if(nActionId >= VMX_CAS_MAX_NUM_OF_ACTION)
	{
		HxLOG_Error("\r nActionId(%d)\n", nActionId);
		return HANDLE_NULL;
	}

	if(pstInst == NULL)
	{
		HxLOG_Error("\r\n");
		return HANDLE_NULL;
	}

	return pstInst->stActionList[nActionId].hAction;
}

STATIC Handle_t vmx_main_GetLiveActionHandle(void)
{
	HUINT8 i = 0;
	Vmx_Instance_t *pstInst = vmx_main_GetInstance();
	Vmx_ActionTable_t *pAct = NULL;

	for (i = 0; i < VMX_CAS_MAX_NUM_OF_ACTION; i++)
	{
		pAct = &pstInst->stActionList[i];
		if(pAct == NULL)
		{
			continue;
		}

		if (pAct->eActType == eCAS_ActionType_Live)
		{
			return pAct->hAction;
		}
	}

#if 0//defined(CONFIG_DEBUG)
	{
#if defined(FOR_CMD_TEST)
		#if 1 // test only TODO_VMX
		Handle_t hAction;
		hAction = MWC_RM_GetActionHandle(eActionId_VIEW_0/*AP_AM_GetMainActionId()*/);
	//	if(hAction != ERR_FAIL)
		{
			return  hAction;
		}
		#endif
#endif
	}
#endif
	HxLOG_Error("\r\n");
	return HANDLE_NULL;
}

STATIC void vmx_main_Init(void)
{
	HxLOG_Print("[Enter]\n");

	if(vmx_main_InitContext() != ERR_OK)
	{
		HxLOG_Error("vmx_main_InitContext() : error\n");
	}

	if(xsvc_vmx_action_Init() != ERR_OK)
	{
		HxLOG_Error("xsvc_vmx_action_Init() : error\n");
	}

	if(xsvc_vmx_psi_Init() != ERR_OK)
	{
		HxLOG_Error("xsvc_vmx_psi_Init() : error\n");
	}

	if(xsvc_vmx_dscr_Init() != ERR_OK)
	{
		HxLOG_Error("xsvc_vmx_dscr_Init() : error\n");
	}

	if(xsvc_vmx_ctrl_RegisterControlList() != ERR_OK)
	{
		HxLOG_Error("xsvc_vmx_ctrl_RegisterControlList() : error\n");
	}

#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
	if(VMX_VR_IPTVH_MGR_Init() != ERR_OK)
	{
		HxLOG_Error("xsvc_vmx_iptv_mgr_init() : error\n");
	}
#elif defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
	// sc
	if(xsvc_cas_vmx_bc_sc_Init() != ERR_OK)
	{
		HxLOG_Error("xsvc_cas_vmx_bc_sc_Init() : error\n");
	}

	if(vmx_vr_bc_mgr_InitContext() != ERR_OK)
	{
		HxLOG_Error("vmx_vr_bc_mgr_InitContext() : error\n");
	}
#endif
	HxLOG_Print("[Exit]\n");
}

//------ Global ---------------------------------------------------------------------
void xsvc_vmx_main_PrintDump(char flag, char *title, void *buf, unsigned int len)
{
	vmx_main_PrintfDump(flag, title, buf, len);
}

void xsvc_vmx_main_PrintContext(void)
{
	vmx_main_PrintContext();
}

Vmx_Instance_t * xsvc_vmx_main_GetInstance(void)
{
	return vmx_main_GetInstance();
}

Vmx_ActionTable_t * xsvc_vmx_main_GetActionByActionHandle(Handle_t hAction)
{
	return vmx_main_GetActionByActionHandle(hAction);
}

Handle_t xsvc_vmx_main_GetActionHandleFromStbHdl(Handle_t hStbHandle)
{
	return vmx_main_GetActionHandleFromStbHdl(hStbHandle);
}

Handle_t xsvc_vmx_main_GetActionHandleFromActionId(HUINT32 nActionId)
{
	return vmx_main_GetActionHandleFromActionId(nActionId);
}

Handle_t xsvc_vmx_main_GetLiveActionHandle(void)
{
	return vmx_main_GetLiveActionHandle();
}

#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
HUINT8 xsvc_vmx_main_GetServiceIdx(HUINT32 ulActionId)
{
	return vmx_main_GetServiceIdx(ulActionId);
}
#elif defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
HERROR xsvc_vmx_main_GetServiceIdx(Vmx_ActionTable_t *pAct)
{
	return vmx_main_GetServiceIdx(pAct);
}
#endif

#if defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
Handle_t xsvc_vmx_main_GetActionHandleFromServiceIndex(HUINT8 usFilterId) // FilterId == ServiceIndex
{
	return vmx_main_GetActionHandleFromServiceIndex(usFilterId);
}

 Handle_t xsvc_vmx_main_GetStbHandleFormServiceIndex(HUINT8 usFilterId)
 {
 	return vmx_main_GetstbHandleFromServiceIndex(usFilterId);
 }
#endif

HERROR xsvc_vmx_main_DefaultSetting(void)
{
	// 지금은 store file 삭제만 하면 된다
	return vmx_vr_iptvh_connection_RemoveStoreFile();
}

//------ Global ---------------------------------------------------------------------
HERROR xsvc_cas_VmxMainInit(void)
{
	vmx_main_Init();
	return ERR_OK;
}

/* end of file */
