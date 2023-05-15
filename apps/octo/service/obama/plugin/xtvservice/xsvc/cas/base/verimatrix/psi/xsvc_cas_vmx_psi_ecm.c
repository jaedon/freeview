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
	@file     xsvc_cas_vmx_psi_ecm.c
	@brief    xsvc_cas_vmx_psi_ecm.c (Verimatrix ECM)

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
#include "vmx_vr_iptvh_mgr.h"
#elif defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
#include "_xsvc_cas_vmx_bc_main.h"
#endif
#include <util.h>
#include "_xsvc_cas_vmx_psi.h"
#include "_xsvc_cas_vmx_os.h"
#include "_xsvc_cas_vmx_dmx.h"

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
STATIC HULONG 			s_ulVmxEcmSemId;

Vmx_Ecm_t 					s_astVmxEcm[VMX_MAX_NUM_OF_ECM];

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

STATIC void vmx_ecm_FilterInit(Vmx_Ecm_t *pEcm);

#define INIT_________________________________________________

STATIC HERROR vmx_ecm_InitCore(void)
{
	HUINT16 		i;
	HINT32 	nResult = 0;

	HxLOG_Print("[Enter]\r\n");

	nResult = VMX_SEM_Create("VmxEcmSem", &s_ulVmxEcmSemId);
	if(nResult != ERR_OK)
	{
		HxLOG_Error("Failure in Semaphore Creation \r\n");
		return ERR_FAIL;
	}

	VMX_memset(s_astVmxEcm, 0, sizeof(Vmx_Ecm_t) * VMX_MAX_NUM_OF_ECM);

	for (i = 0; i < VMX_MAX_NUM_OF_ECM; i++)
	{
		s_astVmxEcm[i].usEcmPid 			= PID_NULL;
		s_astVmxEcm[i].usRefCnt 			= 0;
		s_astVmxEcm[i].hSession 			= HANDLE_NULL;
		s_astVmxEcm[i].eState 				= eVmx_Ecm_Init;
		s_astVmxEcm[i].ulFilterId	 			= 0;
		s_astVmxEcm[i].usUniqueId			= 0;
		vmx_ecm_FilterInit(&s_astVmxEcm[i]);		// dvb only
	}

	HxLOG_Print("[Exit]\r\n");
	return ERR_OK;
}

STATIC void vmx_ecm_FilterInit(Vmx_Ecm_t *pEcm)
{
	HxLOG_Print("[Enter]\n");
	if(pEcm == NULL)
	{
		HxLOG_Error("fail\n");
		return;
	}

	pEcm->stFilter.eFilterMode 	=  eVmxBc_EcmFilterMode_None;
	pEcm->stFilter.ucVmxFilterId 	= VMX_INVALID_SERVICE_IDX;
	pEcm->stFilter.ePageSearchMode	= eVmx_EcmPageSearch_None;
	pEcm->stFilter.ucTableId		= VMX_INVALID_FILTER_VALUE;
	pEcm->stFilter.ucVersion		= VMX_INVALID_FILTER_VALUE;
	pEcm->stFilter.ucPage		= VMX_INVALID_FILTER_VALUE;

	HxLOG_Print("[Exit]\n");
}

STATIC Handle_t vmx_ecm_GetStbHandle(Handle_t hEcm)
{
	Vmx_Ecm_t *pEcm;
	HxLOG_Print("[Enter]\n");
	if (hEcm == HANDLE_NULL)
	{
		HxLOG_Error("null handle\r\n");
		return HANDLE_NULL;
	}

	pEcm = (Vmx_Ecm_t *)hEcm;

	if (pEcm == NULL)
	{
		HxLOG_Error("null ptr\r\n");
		return HANDLE_NULL;
	}

	return xsvc_vmx_psi_session_GetStbHandle(pEcm->hSession);
}

STATIC Handle_t vmx_ecm_GetHandleBySession(Handle_t hSession)
{
	HUINT16 i;
	HxLOG_Print("[Enter]\n");
	for(i = 0 ; i < VMX_MAX_NUM_OF_ECM ; i++)
	{
		if(s_astVmxEcm[i].hSession == hSession)
		{
			break;
		}
	}

	if(i == VMX_MAX_NUM_OF_ECM)
	{
		HxLOG_Error("fail\n");
		return HANDLE_NULL;
	}
	HxLOG_Print("[Exit]\n");
	return (Handle_t)&s_astVmxEcm[i];
}

STATIC Vmx_EcmFilter_t * vmx_ecm_GetEcmFilterInfoFromEcmPid(HUINT16 usEcmPid)
{
	HUINT16 i;
	HxLOG_Print("[Enter]\n");
	for(i=0; i< VMX_MAX_NUM_OF_ECM; i++)
	{
		if(s_astVmxEcm[i].usEcmPid == usEcmPid)
			return &s_astVmxEcm[i].stFilter;
	}
	HxLOG_Error("fail\n");
	return NULL;
}

STATIC Handle_t vmx_ecm_GetEcmHandleFromEcmPid(HUINT16 usEcmPid)
{
	HUINT16 i;
	HxLOG_Print("[Enter]\n");
	for(i=0; i< VMX_MAX_NUM_OF_ECM; i++)
	{
		if(s_astVmxEcm[i].usEcmPid == usEcmPid)
		{
			break;
		}
	}

	if(i == VMX_MAX_NUM_OF_ECM)
	{
		HxLOG_Error("fail\n");
		return HANDLE_NULL;
	}
	HxLOG_Print("[Exit]\n");
	return (Handle_t)&s_astVmxEcm[i];
}

STATIC Vmx_EcmFilter_t * vmx_ecm_GetEcmFilterInfoFromDescFilterId(HUINT32 usDescFilterId)
{
	HUINT16 i;
	HxLOG_Print("[Enter]\n");
	for(i=0; i< VMX_MAX_NUM_OF_ECM; i++)
	{
		if(s_astVmxEcm[i].ulFilterId == usDescFilterId)
		{
			break;
		}
	}

	if(i == VMX_MAX_NUM_OF_ECM)
	{
		HxLOG_Error("fail\n");
		return NULL;
	}
	HxLOG_Print("[Exit]\n");
	return &s_astVmxEcm[i].stFilter;
}

#if 0//defined(CONFIG_MW_MM_PVR) // humandks_add
HUINT8	vmx_ecm_IsEcmQueryMode(Handle_t hEcm)
{
	Vmx_Ecm_t *pEcm;

	if (hEcm == HANDLE_NULL)
	{
		HxLOG_Error("null handle\r\n");
		return FALSE;
	}

	pEcm = (Vmx_Ecm_t *)hEcm;

	if(CXMGR_GetActionTypeFromStbHandle(CXECM_GetStbHandle(hEcm)) != eCAS_ActionType_Recording)
	{
		pEcm->ucQueryMode = FALSE;
	}

	return pEcm->ucQueryMode;
}

HERROR	CXECM_SetEcmOperationalMode(Handle_t hEcm)
{
	Vmx_Ecm_t *pEcm;

	if (hEcm == HANDLE_NULL)
	{
		HxLOG_Error("null handle\r\n");
		return HANDLE_NULL;
	}

	pEcm = (Vmx_Ecm_t *)hEcm;

	pEcm->ucQueryMode = FALSE;

	return ERR_OK;
}

HERROR	CXECM_SetAccessStatus(Handle_t hEcm, HUINT8 ucProgMr)
{
	Vmx_Ecm_t *pEcm;

	if (hEcm == HANDLE_NULL)
	{
		HxLOG_Error("null handle\r\n");
		return HANDLE_NULL;
	}

	pEcm = (Vmx_Ecm_t *)hEcm;

	pEcm->ucProgMrValue = ucProgMr;

	return ERR_OK;
}

HERROR	CXECM_GetAccessStatus(Handle_t hEcm, HUINT8 *pucProgMr)
{
	Vmx_Ecm_t *pEcm;

	if (hEcm == HANDLE_NULL)
	{
		HxLOG_Error("null handle\r\n");
		return ERR_FAIL;
	}

	pEcm = (Vmx_Ecm_t *)hEcm;

	*pucProgMr = pEcm->ucProgMrValue;

	return ERR_OK;
}
#endif


#define ECM_TABLE__________________________________________________

STATIC HERROR vmx_ecm_StartFilter(Handle_t hEcm, Vmx_EcmMode_t eMode, Vmx_EcmFilter_t *pstEcmFilter)
{
	Vmx_Ecm_t 		*pEcm;
	HERROR 		nErr;

	HxLOG_Print("[Enter]\r\n");

	if (hEcm == HANDLE_NULL)
	{
		HxLOG_Error("null handle\r\n");
		return ERR_FAIL;
	}

	pEcm = (Vmx_Ecm_t *)hEcm;
	if (pEcm == NULL)
	{
		HxLOG_Error("null ptr\r\n");
		return ERR_FAIL;
	}

//	VMX_SEM_Get(s_ulVmxEcmSemId);

	if (pEcm->usEcmPid == PID_NULL)
	{
//		VMX_SEM_Release(s_ulVmxEcmSemId);
		HxLOG_Error("null pid\r\n");
		return ERR_FAIL;
	}

	if (pEcm->eState == eVmx_Ecm_Running)
	{
//		VMX_SEM_Release(s_ulVmxEcmSemId);
		HxLOG_Error("already running usEcmPid(0x%X)\r\n", pEcm->usEcmPid);
		return ERR_OK;
	}


	VMX_memset(pEcm->aucFilter, 0, VMX_ECM_FILTER_LEN);
	VMX_memset(pEcm->aucMask, 0, VMX_ECM_FILTER_LEN);

	if (eMode == eVmx_Ecm_Both)
	{
		pEcm->aucFilter[0] = 0x80;
		pEcm->aucMask[0] = 0xFE;
	}
	else
	{
		pEcm->aucFilter[0] = (HUINT8)eMode;
		pEcm->aucMask[0] = 0xFF;
	}

#if 0//defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
	// Betacrypt CAS인경우 즉 DVB Only인경우에만 해당된다.
	if(pstEcmFilter)
	{
		if(pstEcmFilter->eFilterMode == eVmxBc_EcmFilterModePageLocked)
		{
			// TODO_VMX : 아래내용 검증 필요.
			// tableId는 위에서 이미 정의해줌.
			pEcm->aucFilter[3] = (pstEcmFilter->ucVersion<<1) & 0xff;
			pEcm->aucMask[3] = 0x1e;
			pEcm->aucFilter[4] = pstEcmFilter->ucPage;
			pEcm->aucMask[4] = 0xff;

			pEcm->eFilterMode = pstEcmFilter->eFilterMode;
			pEcm->ucVmxFilterId = pstEcmFilter->ucVmxFilterId;
		}
	}
#endif

	nErr = xsvc_vmx_dmx_RetrieveEcmSection(xsvc_vmx_psi_session_GetStbHandle(pEcm->hSession), pEcm->usEcmPid, pEcm->aucFilter, pEcm->aucMask, &pEcm->ulFilterId);
	if (nErr != ERR_OK)
	{
		HxLOG_Error("Error! xsvc_vmx_dmx_RetrieveEcmSection() failed. ecmPid(0x%X) \n", pEcm->usEcmPid);
	}
	else
	{
		pEcm->eState = eVmx_Ecm_Running;
	}

//	VMX_SEM_Release(s_ulVmxEcmSemId);

	HxLOG_Print("[Exit]\r\n");
	return nErr;
}

STATIC HERROR vmx_ecm_StopFilter(Handle_t hEcm)
{
	Vmx_Ecm_t *pEcm;

	HxLOG_Print("[Enter]\r\n");

	if (hEcm == HANDLE_NULL)
	{
		HxLOG_Error("null handle\r\n");
		return ERR_FAIL;
	}

	pEcm = (Vmx_Ecm_t *)hEcm;
	if (pEcm == NULL)
	{
		HxLOG_Error("null ptr\r\n");
		return ERR_FAIL;
	}

//	VMX_SEM_Get(s_ulVmxEcmSemId);

	if (pEcm->usEcmPid == PID_NULL)
	{
//		VMX_SEM_Release(s_ulVmxEcmSemId);
		HxLOG_Error("null pid\r\n");
		return ERR_FAIL;
	}

	if (pEcm->eState != eVmx_Ecm_Running)
	{
//		VMX_SEM_Release(s_ulVmxEcmSemId);
		HxLOG_Error("not running\r\n");
		return ERR_OK;
	}

	pEcm->eState = eVmx_Ecm_Stopped;

//	VMX_SEM_Release(s_ulVmxEcmSemId);

	HxLOG_Print("[Exit]\r\n");
	return xsvc_vmx_dmx_StopEcmSection(pEcm->ulFilterId);
}


STATIC Handle_t vmx_ecm_Add(Handle_t hSession, HUINT16 usEcmPid)
{
	Vmx_Ecm_t 		*pEcm = NULL;
	int 			i;

	HxLOG_Print("[Enter]\r\n");

	// 이미 사용되고 있는 ecm pid 인지 확인
	VMX_SEM_Get(s_ulVmxEcmSemId);

	for (i = 0; i < VMX_MAX_NUM_OF_ECM; i++)
	{
		if ((s_astVmxEcm[i].usEcmPid == usEcmPid)
			&& (xsvc_vmx_psi_session_GetStbActionId(hSession) == xsvc_vmx_psi_session_GetStbActionId(s_astVmxEcm[i].hSession))
		)
		{
			// 이미 있다면 ecm table의 index를 return 한다
			s_astVmxEcm[i].usRefCnt++;
			pEcm = &s_astVmxEcm[i];
			break;
		}
	}

	if (pEcm == NULL)
	{
		// 사용되지 않고 있다면 ecm pid 새로 등록
		for (i = 0; i < VMX_MAX_NUM_OF_ECM; i++)
		{
			if (s_astVmxEcm[i].usEcmPid == PID_NULL)
			{
				s_astVmxEcm[i].usEcmPid 	= usEcmPid;
				s_astVmxEcm[i].usRefCnt 	= 1;
				s_astVmxEcm[i].hSession 	= hSession;
				s_astVmxEcm[i].ulFilterId 	= 0;
				vmx_ecm_FilterInit(&s_astVmxEcm[i]);		// dvb only

				pEcm = &s_astVmxEcm[i];
				break;
			}
		}
	}

	VMX_SEM_Release(s_ulVmxEcmSemId);

	// pEcm == null인 경우, 빈 slot이 없어서 등록할 수 없음
	HxLOG_Print("[Exit]\r\n");
	return (Handle_t)pEcm;
}

// 특정 ecm 을 삭제한다. 단, 다른 stream과 연결되어 있다면 지우지 않는다
STATIC HERROR vmx_ecm_Delete(Handle_t hEcm)
{
	Vmx_Ecm_t 		*pEcm;

	HxLOG_Print("[Enter]\r\n");

	if (hEcm == HANDLE_NULL)
	{
		HxLOG_Debug("null handle\r\n");
		return ERR_FAIL;
	}

	pEcm = (Vmx_Ecm_t *)hEcm;

	if (pEcm == NULL)
	{
		HxLOG_Error("null ptr\r\n");
		return ERR_FAIL;
	}

	VMX_SEM_Get(s_ulVmxEcmSemId);

	// 유효한 ecm 인지 확인
	if (pEcm->usEcmPid != PID_NULL)
	{

		// reference한 곳이 1개 이하 이면 삭제
		if (pEcm->usRefCnt <= 1)
		{
			//## 다른 곳에서는 안쓰고 있으니 pEcm을 지워 버립시다
			// filter running 중이면 중단
			// table에서 제거

			if (pEcm->eState == eVmx_Ecm_Running)
			{
				// filter 멈춘다
				vmx_ecm_StopFilter((Handle_t)pEcm);
			}

			pEcm->usEcmPid 			= PID_NULL;
			pEcm->usRefCnt 			= 0;
			pEcm->hSession 			= HANDLE_NULL;
			pEcm->ulFilterId 				= 0;
			vmx_ecm_FilterInit(pEcm);		// dvb only
		}
		else
		{
			pEcm->usRefCnt--;
			//## stream과의 연결을 생각해봅시다.
			if (xsvc_vmx_psi_stream_FindStreamBySessionAndEcm(pEcm->hSession, (Handle_t)pEcm) != ERR_OK)
			{
				// 같은 ecm을 쓰는 stream 중에 같은 session이 없다면, 다른 session을 찾아서 바꿔주어야지요
				pEcm->hSession = xsvc_vmx_psi_stream_GetFirstSessionByEcm((Handle_t)pEcm);
			}
		}
	}
	else
	{
		HxLOG_Error("null ecm pid\r\n");
	}

	VMX_SEM_Release(s_ulVmxEcmSemId);

	HxLOG_Print("[Exit]\r\n");
	return ERR_OK;
}

// return ECM PID
STATIC HUINT16 vmx_ecm_GetPid(Handle_t hEcm)
{
	Vmx_Ecm_t *pEcm;

	HxLOG_Print("[Enter]\r\n");

	if (hEcm == HANDLE_NULL)
	{
		HxLOG_Error("null handle\r\n");
		return PID_NULL;
	}

	pEcm = (Vmx_Ecm_t *)hEcm;
	if (pEcm == NULL)
	{
		HxLOG_Error("null ptr\r\n");
		return PID_NULL;
	}

	HxLOG_Print("[Exit]\r\n");
	return pEcm->usEcmPid;
}

STATIC HUINT16 vmx_ecm_GetEcmLength(Handle_t hEcm)
{
	Vmx_Ecm_t *pEcm;

	HxLOG_Print("[Enter]\r\n");

	if (hEcm == HANDLE_NULL)
	{
		HxLOG_Error("null handle\r\n");
		return PID_NULL;
	}

	pEcm = (Vmx_Ecm_t *)hEcm;

	if (pEcm == NULL)
	{
		HxLOG_Error("null ptr\r\n");
		return PID_NULL;
	}

	HxLOG_Print("[Exit]\r\n");
	return pEcm->usLen;
}

// pucRawData는 여기서 copy해서 쓸거이니, 준곳에서 free하시길.
//## copy한 놈은 queue에서 관리하자.
STATIC HERROR vmx_ecm_OnEcmReceived(HUINT32 ulFilterId, HUINT8 *pucRawData, HUINT32 hAction)
{
	Vmx_EcmMode_t 	eMode;
	Vmx_Ecm_t 			*pEcm = NULL;
	HUINT8			*pucRawEcm;
	HUINT16			usLen;
	register int		i;

//	HxLOG_Print("[Enter]\r\n");

	if (pucRawData == NULL)
	{
		HxLOG_Error("null ptr\r\n");
		return ERR_FAIL;
	}

	VMX_SEM_Get(s_ulVmxEcmSemId);

	for (i = 0; i < VMX_MAX_NUM_OF_ECM; i++)
	{
		if (s_astVmxEcm[i].ulFilterId == ulFilterId && s_astVmxEcm[i].eState == eVmx_Ecm_Running)
		{
			HxLOG_Print("ECM On received : idx[%d] FilterId(0x%08x)(0x%08x), state(%d)(%d)\n",  i, s_astVmxEcm[i].ulFilterId, ulFilterId, s_astVmxEcm[i].eState, eVmx_Ecm_Running);
			pEcm = &s_astVmxEcm[i];
			break;
		}
	}

	if (pEcm == NULL)
	{
		VMX_SEM_Release(s_ulVmxEcmSemId);
		HxLOG_Error("null ecm\r\n");
		return ERR_FAIL;
	}

	usLen = get12bit(pucRawData + 1) + 3;
#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
	pucRawEcm = (HUINT8 *)VMX_MEM_Alloc(usLen);
	if (pucRawEcm == NULL)
	{
		VMX_SEM_Release(s_ulVmxEcmSemId);
		HxLOG_Error("alloc fail\r\n");
		return ERR_FAIL;
	}

	VMX_memcpy(pucRawEcm, pucRawData, usLen);

	// filter toggle해서 다시 건다 (0x80 <-> 0x81)
	if (*pucRawData == eVmx_Ecm_Even)
	{
		eMode = eVmx_Ecm_Odd;
	}
	else
	{
		eMode = eVmx_Ecm_Even;
	}

	//## ecm queue에다가 넣고 mgr task에 msg 보내준다.
	pEcm->usLen = usLen;
	vmx_vr_iptvh_ecm_OnEcmReceived(pEcm, pucRawEcm);

//	VMX_VR_IPTVH_MGR_SendMessage((HUINT32)eVmxIptv_Msg_Ecm, (HUINT32)pEcm, (HUINT32)eMode, (HUINT32)usLen);

	VMX_SEM_Release(s_ulVmxEcmSemId);

	// filter restart를 해야 even/odd toggle이 동작함
	xsvc_vmx_psi_ecm_RestartFilter((Handle_t)pEcm, eMode, NULL);

#elif defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
	vmx_vr_bc_ecm_OnEcmReceived(ulFilterId, pucRawData, usLen, hAction);

	VMX_SEM_Release(s_ulVmxEcmSemId);
#endif
//	HxLOG_Print("[Exit]\r\n");
	return ERR_OK;
}


Handle_t xsvc_vmx_psi_ecm_GetStbHandle(Handle_t hEcm)
{
	return vmx_ecm_GetStbHandle(hEcm);
}

Handle_t xsvc_vmx_psi_ecm_GetHandleBySession(Handle_t hSession)
{
	return vmx_ecm_GetHandleBySession(hSession);
}

Vmx_EcmFilter_t * xsvc_vmx_psi_ecm_GetEcmFilterInfoFromEcmPid(HUINT16 usEcmPid)
{
	return vmx_ecm_GetEcmFilterInfoFromEcmPid(usEcmPid);
}

Vmx_EcmFilter_t * xsvc_vmx_psi_ecm_GetEcmFilterInfoFromDescFilterId(HUINT32 usDescFilterId)
{
	return vmx_ecm_GetEcmFilterInfoFromDescFilterId(usDescFilterId);
}

Handle_t xsvc_vmx_psi_ecm_GetEcmHandleFromEcmPid(HUINT16 usEcmPid)
{
	return vmx_ecm_GetEcmHandleFromEcmPid(usEcmPid);
}

Handle_t xsvc_vmx_psi_ecm_Add(Handle_t hSession, HUINT16 usEcmPid)
{
	return vmx_ecm_Add(hSession, usEcmPid);
}

HERROR xsvc_vmx_psi_ecm_Delete(Handle_t hEcm)
{
	return vmx_ecm_Delete(hEcm);
}

HERROR xsvc_vmx_psi_ecm_StartFilter(Handle_t hEcm, Vmx_EcmMode_t eMode, Vmx_EcmFilter_t *pstEcmFilter)
{
	HERROR nErr;

	VMX_SEM_Get(s_ulVmxEcmSemId);

	nErr = vmx_ecm_StartFilter(hEcm, eMode, pstEcmFilter);

	VMX_SEM_Release(s_ulVmxEcmSemId);

	return nErr;
}

HERROR xsvc_vmx_psi_ecm_RestartFilter(Handle_t hEcm, Vmx_EcmMode_t eMode, Vmx_EcmFilter_t *pstEcmFilter)
{
	HERROR 		nErr;

	HxLOG_Print("[Enter]\r\n");

	if (hEcm == HANDLE_NULL)
	{
		HxLOG_Error("null handle\r\n");
		return ERR_FAIL;
	}

//	VMX_SEM_Get(s_ulVmxEcmSemId);

	vmx_ecm_StopFilter(hEcm);
	nErr = vmx_ecm_StartFilter(hEcm, eMode, pstEcmFilter);

//	VMX_SEM_Release(s_ulVmxEcmSemId);

	return nErr;
}

HERROR xsvc_vmx_psi_ecm_StopFilter(Handle_t hEcm)
{
	HERROR nErr;

	VMX_SEM_Get(s_ulVmxEcmSemId);

	nErr = vmx_ecm_StopFilter(hEcm);

	VMX_SEM_Release(s_ulVmxEcmSemId);

	return  nErr;
}

HUINT16 xsvc_vmx_psi_ecm_GetPid(Handle_t hEcm)
{
	return vmx_ecm_GetPid(hEcm);
}

HUINT16 xsvc_vmx_psi_ecm_GetEcmLength(Handle_t hEcm)
{
	return vmx_ecm_GetEcmLength(hEcm);
}

HERROR xsvc_vmx_psi_ecm_OnEcmReceived(HUINT32 ulFilterId, HUINT8 *pucRawData, HUINT32 hAction)
{
	return vmx_ecm_OnEcmReceived(ulFilterId, pucRawData, hAction);
}

HERROR xsvc_vmx_psi_ecm_InitCore(void)
{
	return vmx_ecm_InitCore();
}

