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
	@file     vmx_vr_iptvh_ecm.c
	@brief    vmx_vr_iptvh_ecm.c (Verimatrix BC ECM)

	Description:   \n
	Module: MW/CAS/ VERIMATRIX \n
	Remarks :\n

	Copyright (c) 2015 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
//#include "bc_consts.h"

#include "_xsvc_cas_vmx_psi.h"
#include "_xsvc_cas_vmx_os.h"
#include "_xsvc_cas_vmx_dmx.h"

#include "_xsvc_cas_vmx_iptv_main.h"
#include "vmx_vr_iptvh_connection.h"
#include "vmx_vr_iptvh_mgr.h"
#include "vmx_vr_iptvh_api.h"
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

typedef struct _VmxIPtvEcmNode_t
{
	HUINT8 				*pucRawData;
	Vmx_Ecm_t 			*pEcm;
	HUINT32				ulLength;
	struct _VmxIPtvEcmNode_t 	*pNextNode;
} VmxIptv_EcmNode_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HULONG 			s_ulVmxIptvEcmQSemId;
VmxIptv_EcmNode_t 		s_stVmxIptvEcmQueue;
Vmx_Ecm_t 				s_astVmxIptvEcm[VMX_MAX_NUM_OF_ECM];

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
STATIC HERROR vmxiptv_ecm_QueueInit(void);
STATIC HERROR vmxiptv_ecm_QueuePut(Vmx_Ecm_t *pEcm, HUINT8 *pucRawData);

STATIC HERROR vmxiptv_ecm_QueueInit(void)
{
	HxLOG_Print("[Enter]\n");

	s_stVmxIptvEcmQueue.pEcm 		= NULL;			// 의미 없음
	s_stVmxIptvEcmQueue.pucRawData 	= NULL;			// 의미 없음
	s_stVmxIptvEcmQueue.pNextNode 	= NULL;		// 이 pointer는 항상 first node를 가리키고 있어야지요,

	HxLOG_Print("[Exit]\n");
	return ERR_OK;
}

STATIC HERROR iptvh_ecm_Init(void)
{
	HINT32 	nResult = 0;

	HxLOG_Print("[Enter]\n");


	nResult = VMX_SEM_Create("VmxIptvCEcmQSem", &s_ulVmxIptvEcmQSemId);
	if(nResult != ERR_OK)
	{
		HxLOG_Error("Failure in Semaphore Creation \n");
		return ERR_FAIL;
	}

	vmxiptv_ecm_QueueInit();

	HxLOG_Print("[Exit]\n");

	return ERR_OK;
}


STATIC HERROR iptvh_ecm_QueuePut(Vmx_Ecm_t *pEcm, HUINT8 *pucRawData)
{
	VmxIptv_EcmNode_t *pNode = NULL, *pNewNode = NULL;

//	HxLOG_Print("[Enter]\n");

	VMX_SEM_Get(s_ulVmxIptvEcmQSemId);

	// 새로은 node 생성
	pNewNode = VMX_MEM_Alloc(sizeof(VmxIptv_EcmNode_t));
	if (pNewNode == NULL)
	{
		VMX_SEM_Release(s_ulVmxIptvEcmQSemId);
		HxLOG_Error("alloc fail\r\n");
		return ERR_FAIL;
	}

	pNewNode->pEcm 			= pEcm;
	pNewNode->pucRawData 	= pucRawData;
	pNewNode->pNextNode 	= NULL;

	// 가장 마지막 node를 찾는다
	pNode = &s_stVmxIptvEcmQueue;

	while (pNode->pNextNode != NULL)
	{
		pNode = pNode->pNextNode;
	}

	pNode->pNextNode = pNewNode;

	VMX_SEM_Release(s_ulVmxIptvEcmQSemId);

//	HxLOG_Print("[Exit]\n");
	return ERR_OK;
}

// return value is the pointer to the ECM raw data
STATIC HUINT8 *iptvh_ecm_QueueGet(Handle_t *pSessionHandle, Handle_t *pEcmHandle)
{
	VmxIptv_EcmNode_t *pFirstNode;
	HUINT8 *pucData = NULL;

//	HxLOG_Print("[Enter]\n");

	if (pSessionHandle == NULL || pEcmHandle == NULL)
	{
		HxLOG_Error("null ptr\r\n");
		return NULL;
	}

	VMX_SEM_Get(s_ulVmxIptvEcmQSemId);

	pFirstNode = s_stVmxIptvEcmQueue.pNextNode;
	if (pFirstNode == NULL)
	{
		// 남아 있는 node가 없음
		VMX_SEM_Release(s_ulVmxIptvEcmQSemId);
//		HxLOG_Print("ECM QUEUE EMPTY\r\n");
		return NULL;
	}

	// 첫번째 node의 값을 pNode에 복사해준다

	if (pFirstNode->pEcm == NULL)
	{
		*pSessionHandle = HANDLE_NULL;
	}
	else
	{
		*pSessionHandle = pFirstNode->pEcm->hSession;
	}

	*pEcmHandle 		= (Handle_t)pFirstNode->pEcm;
	pucData 				= pFirstNode->pucRawData;

	s_stVmxIptvEcmQueue.pNextNode = pFirstNode->pNextNode;

	VMX_MEM_Free(pFirstNode);

	VMX_SEM_Release(s_ulVmxIptvEcmQSemId);

//	HxLOG_Print("[Exit]\n");
	return pucData;
}

STATIC HERROR iptvh_ecm_QueueClear(void)
{
	Handle_t hEcm, hSession;
	HUINT8	*pucData;

	HxLOG_Print("[Enter]\n");

	while (1)
	{
		pucData = iptvh_ecm_QueueGet(&hSession, &hEcm);
		if (pucData == NULL)
			break;

		VMX_MEM_Free(pucData);
	}

	HxLOG_Print("[Exit]\n");
	return ERR_OK;
}

STATIC HBOOL iptvh_ecm_IsQueueEmpty(void)
{
	VmxIptv_EcmNode_t *pFirstNode;

//	HxLOG_Print("[Enter]\n");//

	VMX_SEM_Get(s_ulVmxIptvEcmQSemId);

	pFirstNode = s_stVmxIptvEcmQueue.pNextNode;

	VMX_SEM_Release(s_ulVmxIptvEcmQSemId);

	// 남아 있는 node가 없음
	if (pFirstNode == NULL)
	{
//		HxLOG_Print("QUEUE EMPTY\r\n");
		return TRUE;
	}

//	HxLOG_Print("[Exit]\n");
	return FALSE;
}

STATIC void iptvh_ecm_SendEcmMsgToVm(void)
{
	Vmx_Instance_t 		*pstInst = xsvc_vmx_main_GetInstance();
	Vmx_ActionTable_t 	*pAct = NULL;
	HUINT8				*pucData = NULL;
	Handle_t			 hEcm = HANDLE_NULL, hSession = HANDLE_NULL;
	HUINT32				 ulActionId = 0;
	HUINT8 				 ucOride;

//	HxLOG_Print("[Enter]\n");
	if(pstInst == NULL)
	{
		HxLOG_Error("Error! xsvc_vmx_main_GetInstance() faile! \n");
		return;
	}

	if(vmx_vr_iptvh_connection_GetInitFlag()==FALSE)
	{
		HxLOG_Error("Error! connection not initialized yet! \n");
//		return;
	}

	while (1)
	{
		pucData = vmx_vr_iptvh_ecm_QueueGet(&hSession, &hEcm);
		if (pucData == NULL)
		{
			break;
		}

		ulActionId = xsvc_vmx_psi_session_GetStbActionId(hSession);
		pAct = &pstInst->stActionList[ulActionId];

		if ((pAct != NULL) && (ulActionId < VMX_CAS_MAX_NUM_OF_ACTION))
		{
			if(pAct->bServiceStarted == TRUE)
			{
				if(pAct->pContext != NULL)
				{
					HUINT16		usEcmLen = xsvc_vmx_psi_ecm_GetEcmLength(hEcm);

#ifdef CONFIG_DEBUG
					HxLOG_Print("\n\n====================== Send ECM to library ===========================\n");
					HxLOG_Print("ECM : Pid(0x%x), length(%d)\n", xsvc_vmx_psi_ecm_GetPid(hEcm));
					HxLOG_Print("ECM : ulActionId(%d), ucVmxServiceIdx(0x%x)\n",ulActionId, pAct->ucVmxServiceIdx);
					xsvc_vmx_main_PrintDump(VMX_DBG_VR_IPTVH_PSI_ECM, "ECMData", pucData, usEcmLen);
#endif

					if(vmx_vr_iptvh_api_Client_SetEcm(pAct->pContext, 1, xsvc_vmx_psi_ecm_GetPid(hEcm), pucData, usEcmLen) != ERR_OK)
					{
						HxLOG_Error("vmx_vr_iptvh_api_Client_SetEcm() failed\r\n");
					}
					else // ecm이 성공적으로 전달되면 바로 rating 정보를 알수 있다.
					{
						vmx_vr_iptvh_api_GetRatingLevel( pAct->pContext, &ucOride, &pAct->ucChannelRatingLevel, &pstInst->ucCurrentRatingLevel);
						HxLOG_Print("usCurrentRatingLevel(%d), usChannelRatingLevel(%d), ucOride(%d)\n", pstInst->ucCurrentRatingLevel, pAct->ucChannelRatingLevel);
					}
				}
			}
		}
		VMX_MEM_Free(pucData);
	}
//	HxLOG_Print("[Exit]\n");
}

void vmx_vr_iptvh_ecm_SendEcmMsgToVm(void)
{
	iptvh_ecm_SendEcmMsgToVm();
}

HBOOL vmx_vr_iptvh_ecm_IsQueueEmpty(void)
{
	return iptvh_ecm_IsQueueEmpty();
}

// return value is the pointer to the ECM raw data
HUINT8 *vmx_vr_iptvh_ecm_QueueGet(Handle_t *pSessionHandle, Handle_t *pEcmHandle)
{
	return iptvh_ecm_QueueGet(pSessionHandle, pEcmHandle);
}

// TODO_VMX
HERROR vmx_vr_iptvh_ecm_QueueClear(void)
{
	return iptvh_ecm_QueueClear();
}

HERROR vmx_vr_iptvh_ecm_OnEcmReceived(Vmx_Ecm_t *pEcm, HUINT8 *pucRawData)
{
	return iptvh_ecm_QueuePut(pEcm, pucRawData);
}

HERROR vmx_vr_ecm_GetCaSystemId(HUINT16* pusCaSystemId)
{
	return vmx_vr_iptvh_api_GetCaSystemId(pusCaSystemId);
}

HERROR vmx_vr_iptvh_ecm_Init(void)
{
	return iptvh_ecm_Init();
}

/* end of file */
