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
	@file     xsvc_cas_vmx_psi_stream.c
	@brief    xsvc_cas_vmx_psi_stream.c (Verimatrix PSI STREAM)

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

#include <_svc_cas_sub_svc_api.h>

#include "_xsvc_cas_vmx_psi.h"
#include "_xsvc_cas_vmx_dscr.h"
#include "_xsvc_cas_vmx_os.h"

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
#if 0
typedef enum
{
	eVmx_Stream_NotUsed,
	eVmx_Stream_Used,
} Vmx_StreamState_t;

// information about each elementary stream
typedef struct
{
	HUINT16			usPid;
	Vmx_StreamState_t	eState;
	Handle_t			hSession;
	Handle_t			hEcm;
	Handle_t			hDescrambler;
} Vmx_Stream_t;
#endif

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HULONG	 			s_ulVmxStreamSemId;
Vmx_Stream_t 			s_astVmxStream[VMX_MAX_NUM_OF_ES];

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
STATIC HERROR vmx_stream_LinkEcm(Handle_t hSession, HINT32 nIdx, HUINT16 usEcmPid);
STATIC HERROR  vmx_stream_StartEcmFilter(HINT32 nIdx, Vmx_EcmFilter_t *pstEcmFilter);
STATIC HERROR vmx_stream_UnlinkEcm(HINT32 nIdx);

#define INIT_________________________________________________

STATIC HERROR vmx_stream_InitCore(void)
{
	int i;
	HINT32 nResult = 0;

	nResult = VMX_SEM_Create("VmxStrSem", &s_ulVmxStreamSemId);
	if(nResult != ERR_OK)
	{
		HxLOG_Error("Failure in Semaphore Creation \r\n");
		return ERR_FAIL;
	}

	VMX_memset(s_astVmxStream, 0, sizeof(Vmx_Stream_t) * VMX_MAX_NUM_OF_ES);

	for (i = 0; i < VMX_MAX_NUM_OF_ES; i++)
	{
		s_astVmxStream[i].usPid 			= PID_NULL;
		s_astVmxStream[i].eState 			= eVmx_Stream_NotUsed;
		s_astVmxStream[i].hSession 		= HANDLE_NULL;
		s_astVmxStream[i].hEcm		 	= HANDLE_NULL;
		s_astVmxStream[i].hDescrambler	= HANDLE_NULL;
	}

	return ERR_OK;
}

#define DESCRAMBLER__________________________________________________
// 무조건 descrambler slot을 open 하는 것은 아니고, 연관된 ecm이 있는지 (scrambled 방송인지)확인 하고 open 함.
// 유효한 stream 인지, 이미 slot을 가지고 있는 지 등도 확인함.
// 이 함수를 호출하는 쪽에서는 조건 확인을 할 필요 없이, 일단 try 해본다는 개념으로.. 되면 되는거고 아니면 말고..
// nIdx는 stream table 상의 index
STATIC HERROR vmx_stream_OpenDescrambler(int nIdx, Handle_t hSession)
{
	HUINT32 ulHandle;
	Handle_t hSTBSession = HANDLE_NULL;

	HxLOG_Print("[Enter]\r\n");

	// for removing prevent issue
	if (nIdx < 0 || nIdx >= VMX_MAX_NUM_OF_ES)
	{
		HxLOG_Error("wrong idx(%d)\r\n", nIdx);
		return ERR_FAIL;
	}

	// 유효한 ES인지 확인
	if (s_astVmxStream[nIdx].usPid == PID_NULL)
	{
		HxLOG_Error("null pid\r\n");
		return ERR_FAIL;
	}
	if (s_astVmxStream[nIdx].eState != eVmx_Stream_Used)
	{
		HxLOG_Error("not used stream - idx(%d) state(%d)\r\n", nIdx, s_astVmxStream[nIdx].eState);
		return ERR_FAIL;
	}

	// 이미 descrambler slot 할당 받았는지 확인
	if (s_astVmxStream[nIdx].hDescrambler != HANDLE_NULL)
	{
		HxLOG_Error("descrambler slot is already assigned\r\n");
		return ERR_FAIL;
	}

	// hEcm 있는지 확인 (hEcm == NULL이면 free 서비스이고 descrambler 필요 없는거 같은데)
	if (s_astVmxStream[nIdx].hEcm == HANDLE_NULL)
	{
		HxLOG_Debug("free service\r\n");
		return ERR_FAIL;
	}

	hSTBSession = xsvc_vmx_psi_session_GetStbHandle(hSession);
	// descrambler slot 할당
	ulHandle = xsvc_vmx_dscr_Open(s_astVmxStream[nIdx].hEcm, nIdx, s_astVmxStream[nIdx].usPid, svc_cas_SubSvcRmGetActionId(hSTBSession));

	HxLOG_Print("nIdx[%d],  usPid[0x%x], ulHandle[0x%08x]\r\n", nIdx, s_astVmxStream[nIdx].usPid, ulHandle);

	if (ulHandle == HANDLE_NULL)
	{
		HxLOG_Error("fail to open descrambler slot\r\n");
		return ERR_FAIL;
	}

	s_astVmxStream[nIdx].hDescrambler = (Handle_t)ulHandle;

	return ERR_OK;
}

// nIdx는 stream table 상의 index
STATIC HERROR vmx_stream_CloseDescrambler(int nIdx)
{
	HxLOG_Print("[Enter]\r\n");

	// for removing prevent issue
	if (nIdx < 0 || nIdx >= VMX_MAX_NUM_OF_ES)
	{
		HxLOG_Error("wrong idx(%d)\r\n", nIdx);
		return ERR_FAIL;
	}

	// 유효한 ES인지 확인
	if (s_astVmxStream[nIdx].usPid == PID_NULL)
	{
		HxLOG_Debug("null pid. idx(%d) \r\n", nIdx);
		return ERR_FAIL;
	}

	// 이미 descrambler slot 할당 받았다면 close
	if (s_astVmxStream[nIdx].hDescrambler != HANDLE_NULL)
	{
		//## descrambler slot close
		xsvc_vmx_dscr_Close(s_astVmxStream[nIdx].hDescrambler, s_astVmxStream[nIdx].usPid);
		HxLOG_Print("nIdx[%d],  usPid[0x%x], ulHandle[0x%08x]\r\n", nIdx, s_astVmxStream[nIdx].usPid, s_astVmxStream[nIdx].hDescrambler);
		s_astVmxStream[nIdx].hDescrambler = HANDLE_NULL;
	}

	HxLOG_Print("[Exit]\r\n");
	return ERR_OK;
}

#define STREAM_TABLE__________________________________________________

// 주어진 es의 ecm pid를 찾는다.
STATIC HUINT16 vmx_stream_GetEcmPidFromPmt(SvcCas_PmtInfo_t *pstPmtInfo, SvcCas_PmtElement_t *pstEsInfo)
{
	int 				i;
	HUINT16				usPid = PID_NULL;
	HUINT16				usCaSysId;

	HxLOG_Print("[Enter]\r\n");

	if ((pstPmtInfo == NULL) || (pstEsInfo == NULL))
	{
		HxLOG_Error("wrong param\r\n");
		return PID_NULL;
	}

	if(vmx_vr_ecm_GetCaSystemId(&usCaSysId) != ERR_OK)
	{
		HxLOG_Error("vmx_vr_ecm_GetCaSystemId() fail \n");
		return PID_NULL;
	}

	if (pstEsInfo->bElementCaFlag)
	{
		for (i = 0; i < pstEsInfo->ulNumOfCaDescriptor; i++)
		{
			//## ca system id 비교 (ca_system_id는 카드에서 얻을 수 있을 것이다.. 그런데 카드가 없다면 어떻게 하지?)
			HxLOG_Print("ElementCaDescriptor Cmp[%d] => CaSysId : (0x%x)(0x%x)\n", i, usCaSysId, pstEsInfo->stElementCaDescriptor[i].usCaSysId);
			if (pstEsInfo->stElementCaDescriptor[i].usCaSysId == usCaSysId)
			{
				HxLOG_Print("ElementCaDescriptor Cmp[%d] => usCaPid(0x%x)\n", i,pstEsInfo->stElementCaDescriptor[i].usCaPid);
				usPid = pstEsInfo->stElementCaDescriptor[i].usCaPid;
				break;
			}
		}
	}

	// 만약 elementary stream level에 ecm pid를 못찾으면 (ca_Descriptor가 없다면), program level에서 찾는다.
	if (usPid == PID_NULL)
	{
		if (pstPmtInfo->bProgramCaFlag)
		{
			for (i = 0; i < pstPmtInfo->ulNumOfProgramCaDescriptor; i++)
			{
				HxLOG_Print("ProgramCaDescriptor Cmp[%d] => CaSysId : (0x%x)(0x%x)\n", i, usCaSysId,pstPmtInfo->stProgramCaDescriptor[i].usCaSysId);
				//## ca system id 비교 (ca_system_id는 카드에서 얻을 수 있을 것이다.. 그런데 카드가 없다면 어떻게 하지?)
				if (pstPmtInfo->stProgramCaDescriptor[i].usCaSysId ==usCaSysId)
				{
					HxLOG_Print("ProgramCaDescriptor Cmp[%d] => usCaPid(0x%x)\n", i, pstPmtInfo->stProgramCaDescriptor[i].usCaPid);
					usPid = pstPmtInfo->stProgramCaDescriptor[i].usCaPid;
					break;
				}
			}
		}
	}

	HxLOG_Print("[Exit:ecm pid=%x]\r\n", usPid);
	return usPid;
}

STATIC HERROR vmx_stream_Add(Handle_t hSession, SvcCas_PmtInfo_t *pstPmtInfo, SvcCas_PmtElement_t *pstEsInfo, HBOOL * pbDscrStartFlag)
{
	int i;
	HUINT16 usEcmPid;

	HxLOG_Print("[Enter]\r\n");

	if ((pstPmtInfo == NULL) || (pstEsInfo == NULL))
	{
		HxLOG_Error("wrong param. hSession(%p) pstPmtInfo(%p) pstEsInfo(%p)\n", hSession, pstPmtInfo, pstEsInfo);
		return ERR_FAIL;
	}

	for (i = 0; i < VMX_MAX_NUM_OF_ES; i++)
	{
		// 일단 pid로 빈 slot을 확인해보자
		if (s_astVmxStream[i].usPid == PID_NULL)
		{
			s_astVmxStream[i].usPid 	= pstEsInfo->usPid;
			s_astVmxStream[i].hSession	= hSession;

///////////////////////////////////////////////////////////////////////////////////////////////
//
// !!! 만약 setPid를 안쓰고 pmt 상의 모든 stream을 처리해야 한다면, 여기서 state를 무조건 used로 해주면 된다.. 참 쉽죠~잉~
//s_astVmxStream[i].eState = eVmx_Stream_Used;
//
///////////////////////////////////////////////////////////////////////////////////////////////

			usEcmPid = vmx_stream_GetEcmPidFromPmt(pstPmtInfo, pstEsInfo);
			if (usEcmPid != PID_NULL)
			{
				// ecm pid가 있다면 hEcm 등록하고
				vmx_stream_LinkEcm(hSession, i, usEcmPid);
#if 1//def VMX_SVC_TEST // 다식 확인 필요.
				s_astVmxStream[i].eState = eVmx_Stream_Used;
#endif
				// [improve speed]: 아래 함수 안에 들어가서도 used인지 확인 함
				if (s_astVmxStream[i].eState == eVmx_Stream_Used)
				{
					vmx_stream_StartEcmFilter(i, NULL);
					// descrambler도 등록해야지
					vmx_stream_OpenDescrambler(i, hSession);
					* pbDscrStartFlag = TRUE;
				}
			}
			break;
		}
	}

	HxLOG_Print("[Exit]\r\n");
	return ERR_OK;
}

STATIC HERROR vmx_stream_bc_Add(Handle_t hSession, SvcCas_PmtInfo_t *pstPmtInfo, SvcCas_PmtElement_t *pstEsInfo, HBOOL *pbDscrStartFlag)
{
	int i;
	HUINT16 usEcmPid;

	HxLOG_Print("[Enter]\r\n");

	if (pstPmtInfo == NULL || pstEsInfo == NULL)
	{
		HxLOG_Error("wrong param\r\n");
		return ERR_FAIL;
	}

	for (i = 0; i < VMX_MAX_NUM_OF_ES; i++)
	{
		if (s_astVmxStream[i].usPid == PID_NULL)
		{
			s_astVmxStream[i].usPid 	= pstEsInfo->usPid;
			s_astVmxStream[i].hSession	= hSession;

			usEcmPid = vmx_stream_GetEcmPidFromPmt(pstPmtInfo, pstEsInfo);
			if (usEcmPid != PID_NULL)
			{
				// ecm pid가 있다면 hEcm 등록하고
				vmx_stream_LinkEcm(hSession, i, usEcmPid);

				// [improve speed]: 아래 함수 안에 들어가서도 used인지 확인 함
#if 1//def VMX_SVC_TEST // 다식 확인 필요.
				s_astVmxStream[i].eState = eVmx_Stream_Used;
#endif
				if (s_astVmxStream[i].eState == eVmx_Stream_Used)
				{
					//vmx_stream_StartEcmFilter(i);

					// descrambler도 등록해야지
					vmx_stream_OpenDescrambler(i, hSession);
					* pbDscrStartFlag = TRUE;
				}
			}
			break;
		}
	}

	HxLOG_Print("[Exit]\r\n");
	return ERR_OK;
}

STATIC HERROR vmx_stream_Delete(HINT32 nIdx)
{
	HxLOG_Print("[Enter]\r\n");

	if (s_astVmxStream[nIdx].hSession == HANDLE_NULL)
	{
		HxLOG_Error("null handle\r\n");
		return ERR_FAIL;
	}

	// descrambler slot을 close한다
	vmx_stream_CloseDescrambler(nIdx);

	// ecm과 link 끊어요
	vmx_stream_UnlinkEcm(nIdx);

	s_astVmxStream[nIdx].hSession 	= HANDLE_NULL;
	s_astVmxStream[nIdx].eState	 	= eVmx_Stream_NotUsed;
	s_astVmxStream[nIdx].usPid 		= PID_NULL;

	HxLOG_Print("[Exit]\r\n");
	return ERR_OK;
}

STATIC HERROR vmx_stream_LinkEcm(Handle_t hSession, HINT32 nIdx, HUINT16 usEcmPid)
{
	Handle_t hEcm;

	HxLOG_Print("[Enter]\r\n");

	//같은 ecm 있나 찾아보고 있으면 연결, 없으면 추가
	hEcm = xsvc_vmx_psi_ecm_Add(hSession, usEcmPid);

	s_astVmxStream[nIdx].hEcm = hEcm;

	if (hEcm == HANDLE_NULL)
	{
		HxLOG_Error("null handle\r\n");
		return ERR_FAIL;
	}

	//## ecm filter start 해야되나 ?
//	vmx_stream_StartEcmFilter(nIdx);
	HxLOG_Print("[Exit]\r\n");
	return ERR_OK;
}

STATIC HERROR vmx_stream_UnlinkEcm(HINT32 nIdx)
{
	HxLOG_Print("[Enter]\r\n");

	// ecm table에서 삭제 될지는 ecm쪽에서 알아서 할 거임 (filter stop도 알아서 할 것임)
	xsvc_vmx_psi_ecm_Delete(s_astVmxStream[nIdx].hEcm);

	s_astVmxStream[nIdx].hEcm = HANDLE_NULL;

	HxLOG_Print("[Exit]\r\n");
	return ERR_OK;
}

//stream이 not used이면 filter start 할 필요 없음
STATIC HERROR  vmx_stream_StartEcmFilter(HINT32 nIdx, Vmx_EcmFilter_t *pstEcmFilter)
{
	Handle_t	hEcm;

	HxLOG_Print("[Enter]\r\n");

	hEcm = s_astVmxStream[nIdx].hEcm;
	if (hEcm == HANDLE_NULL)
	{
		HxLOG_Debug("wrong handle\r\n");
		return ERR_FAIL;
	}

	if (s_astVmxStream[nIdx].eState != eVmx_Stream_Used)
	{
		HxLOG_Error("not used stream\r\n");
		return ERR_FAIL;
	}

	if (xsvc_vmx_psi_session_IsSelected(s_astVmxStream[nIdx].hSession) == FALSE)
	{
		//아직 descramble하도록 선택되지 않은 상태이므로 ecm filter를 걸지 않도록 한다.
		HxLOG_Error("not selected\r\n");
		return ERR_FAIL;
	}

	// filter start: both로 건다.. 이후로는 자동 toggle
	xsvc_vmx_psi_ecm_StartFilter(hEcm, eVmx_Ecm_Both, pstEcmFilter);

	HxLOG_Print("[Exit]\r\n");
	return ERR_OK;
}

// 다른 session에서 쓰고 있다면 filter를 그냥 두고, 아니면 stop 시켜요
STATIC HERROR vmx_stream_StopEcmFilter(HINT32 nIdx)
{
	int i;
	Handle_t hEcm;
	Handle_t hSession;

	HxLOG_Print("[Enter]\r\n");

	hSession = s_astVmxStream[nIdx].hSession;
	hEcm = s_astVmxStream[nIdx].hEcm;

	if (hSession == HANDLE_NULL || hEcm == HANDLE_NULL)
	{
		HxLOG_Error("wrong param\r\n");
		return ERR_FAIL;
	}

	//다른 session에서 사용되는지 찾아보자
	for (i = 0; i < VMX_MAX_NUM_OF_ES; i++)
	{
		if (s_astVmxStream[i].usPid != PID_NULL &&			// 유효한 slot
			s_astVmxStream[i].hSession != hSession && 		// 다른 session
			s_astVmxStream[i].hEcm == hEcm && 				// 같은 ecm
			s_astVmxStream[i].eState == eVmx_Stream_Used)	// 사용중인 stream
		{
			 break;
		}
	}

	if (i >= VMX_MAX_NUM_OF_ES)
	{
		// 못 찾았다면 filter stop
		xsvc_vmx_psi_ecm_StopFilter(hEcm);
	}

	HxLOG_Print("[Exit]\r\n");
	return ERR_OK;
}

STATIC HERROR vmx_stream_SetPid(Handle_t hSession, HUINT16 usPid, HBOOL bAdd, HBOOL *pbDscrStartFlag, HBOOL * pbDscrStopFlag)
{
	HUINT16 i;

	HxLOG_Print("[Enter(%x,%x)]\r\n", usPid, bAdd);

	if (hSession == HANDLE_NULL)
	{
		HxLOG_Error("wrong handle\r\n");
		return ERR_FAIL;
	}

	if (usPid == PID_NULL)
	{
		HxLOG_Error("null pid\r\n");
		return ERR_FAIL;
	}

	VMX_SEM_Get(s_ulVmxStreamSemId);

	for (i = 0; i < VMX_MAX_NUM_OF_ES; i++)
	{
//		if (s_astVmxStream[i].hSession == hSession && s_astVmxStream[i].eType == eType)
// type 구분을 하지 말아보자...
		if (s_astVmxStream[i].hSession == hSession)
		{
			HxLOG_Print("[%d], usPid(0x%x), eState(%d), usPid(%d)\n", i, s_astVmxStream[i].usPid, s_astVmxStream[i].eState, usPid);

			if (s_astVmxStream[i].usPid == usPid)
			{
				// 이미 등록된 pid (여기로 오면 pmt를 받았다는 것임)
				if (bAdd)
				{
					// 등록
					if (s_astVmxStream[i].eState != eVmx_Stream_Used)
					{
						s_astVmxStream[i].eState = eVmx_Stream_Used;
						// 아직 descrambler slot이 open되지 않았다면 open 시도
						vmx_stream_OpenDescrambler(i, hSession);
						//## ecm filter start 해야되나 ?
						vmx_stream_StartEcmFilter(i, NULL);
						* pbDscrStartFlag = TRUE;
					}
				}
				else
				{
					// 제거
					if (s_astVmxStream[i].eState != eVmx_Stream_NotUsed)
					{
						s_astVmxStream[i].eState = eVmx_Stream_NotUsed;
						// descrambler slot을 close한다
						vmx_stream_CloseDescrambler(i);
						// ecm filter 처리 - 다른데서 쓰면 그냥 두고, 쓰지 않으면 없앤다.
						* pbDscrStopFlag = TRUE;
					}
				}

				VMX_SEM_Release(s_ulVmxStreamSemId);
				HxLOG_Print("[Exit]1\r\n");
				return ERR_OK;
			}
		}
	}

	// 없으면 추가 (여기로 왔다면 아직 pmt를 못받은 상태임)
	if (bAdd)
	{
		for (i = 0; i < VMX_MAX_NUM_OF_ES; i++)
		{
			// 일단 pid로 빈 slot을 확인해보자
			if (s_astVmxStream[i].usPid == PID_NULL)
			{
				s_astVmxStream[i].usPid 	= usPid;
				s_astVmxStream[i].eState 	= eVmx_Stream_Used;
				s_astVmxStream[i].hSession	= hSession;
				VMX_SEM_Release(s_ulVmxStreamSemId);
				HxLOG_Print("[Exit]2\r\n");
				return ERR_OK;
			}
		}
	}
	VMX_SEM_Release(s_ulVmxStreamSemId);

	HxLOG_Print("[Exit]3\r\n");
	return ERR_FAIL;
}

STATIC HERROR vmx_stream_bc_SetPid(Handle_t hSession, HUINT16 usPid, HBOOL bAdd, HBOOL *pbDscrStartFlag, HBOOL * pbDscrStopFlag)
{
	HUINT16 i;
	HxLOG_Print("[Enter(0x%x, %d)]\r\n", usPid, bAdd);

	if (hSession == HANDLE_NULL)
	{
		HxLOG_Error("wrong handle\r\n");
		return ERR_FAIL;
	}

	if (usPid == PID_NULL)
	{
		HxLOG_Error("null pid\r\n");
		return ERR_FAIL;
	}

	VMX_SEM_Get(s_ulVmxStreamSemId);

	for (i = 0; i < VMX_MAX_NUM_OF_ES; i++)
	{
		if (s_astVmxStream[i].hSession == hSession)
		{
			HxLOG_Print("[%d], usPid(0x%x), eState(%d), usPid(0x%x)\n", i, s_astVmxStream[i].usPid, s_astVmxStream[i].eState, usPid);

			if (s_astVmxStream[i].usPid == usPid)
			{
				// 이미 등록된 pid (여기로 오면 pmt를 받았다는 것임)
				if (bAdd)
				{
					// 등록
					if (s_astVmxStream[i].eState != eVmx_Stream_Used)
					{
						s_astVmxStream[i].eState = eVmx_Stream_Used;
						// 아직 descrambler slot이 open되지 않았다면 open 시도
						vmx_stream_OpenDescrambler(i, hSession);
						*pbDscrStartFlag = TRUE;
						#if 0
						//## ecm filter start 해야되나 ?
						vmx_stream_StartEcmFilter(i);
						#endif
					}
				}
				else
				{
					// 제거
					if (s_astVmxStream[i].eState != eVmx_Stream_NotUsed)
					{
						s_astVmxStream[i].eState = eVmx_Stream_NotUsed;
						// descrambler slot을 close한다
						vmx_stream_CloseDescrambler(i);
						// ecm filter 처리 - 다른데서 쓰면 그냥 두고, 쓰지 않으면 없앤다.
						*pbDscrStopFlag = TRUE;
					}
				}

				VMX_SEM_Release(s_ulVmxStreamSemId);
				HxLOG_Print("[Exit]1\r\n");
				return ERR_OK;
			}
		}
	}

	// 없으면 추가 (여기로 왔다면 아직 pmt를 못받은 상태임)
	if (bAdd)
	{
		for (i = 0; i < VMX_MAX_NUM_OF_ES; i++)
		{
			// 일단 pid로 빈 slot을 확인해보자
			if (s_astVmxStream[i].usPid == PID_NULL)
			{
				s_astVmxStream[i].usPid 	= usPid;
				s_astVmxStream[i].eState 	= eVmx_Stream_Used;
				s_astVmxStream[i].hSession	= hSession;
				VMX_SEM_Release(s_ulVmxStreamSemId);
				HxLOG_Print("[Exit]2\r\n");
				return ERR_OK;
			}
		}
	}

	VMX_SEM_Release(s_ulVmxStreamSemId);

	HxLOG_Print("[Exit]3\r\n");
	return ERR_FAIL;
}

#if 0
HBOOL vmx_stream_IsVmxStream(Handle_t hSession)
{
	int 					i, j;
	SvcCas_PmtInfo_t		*pstPmtInfo;
	SvcCas_PmtElement_t 	*pstEsInfo;
	HBOOL				bFound = FALSE;

	HxLOG_Print("[Enter]\r\n");

	pstPmtInfo = xsvc_vmx_psi_session_GetPmtInfo(hSession);

	if (pstPmtInfo == NULL)
	{
		HxLOG_Error("no pmt\r\n");
		return FALSE;
	}

	if (pstPmtInfo->bProgramCaFlag)
	{
		for (i = 0; i < pstPmtInfo->ulNumOfProgramCaDescriptor; i++)
		{
	#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
			if ((pstPmtInfo->stProgramCaDescriptor[i].usCaSysId & 0xFF00) == (eDxCAS_GROUPID_VMXIPTV & 0xFF00))
	#else
			if ((pstPmtInfo->stProgramCaDescriptor[i].usCaSysId & 0xFF00) == (eDxCAS_GROUPID_VMXBC & 0xFF00))
	#endif
			{
				bFound = TRUE;
				break;
			}
		}
	}

	if (bFound == FALSE)
	{
		for (i = 0; i < pstPmtInfo->ulNumOfElement; i++)
		{
			pstEsInfo = &pstPmtInfo->stElementInfo[i];

			if (pstEsInfo->bElementCaFlag)
			{
				for (j = 0; j < pstEsInfo->ulNumOfCaDescriptor; j++)
				{
					if ((pstEsInfo->stElementCaDescriptor[j].usCaSysId & 0xFF00) == (eDxCAS_GROUPID_VMX & 0xFF00))
					{
						bFound = TRUE;
						break;
					}
				}
			}

			if (bFound)
				break;
		}
	}

	HxLOG_Print("[Exit(%d)]\r\n", bFound);
	return bFound;
}
#endif

// pmt parsing하는 곳
STATIC HERROR vmx_stream_ParsePmt(Handle_t hSession, HBOOL *pbDscrStartFlag, HBOOL * pbDscrStopFlag)
{
	int 				 i, j;
	HBOOL 				 bFound = FALSE;
	HUINT16				 usEcmPid = PID_NULL;
	SvcCas_PmtInfo_t 	*pstPmtInfo = NULL;

	HxLOG_Print("[Enter]\r\n");

	if (hSession == HANDLE_NULL)
	{
		HxLOG_Error("wrong handle\r\n");
		return ERR_FAIL;
	}

	pstPmtInfo = xsvc_vmx_psi_session_GetPmtInfo(hSession);
	if (pstPmtInfo == NULL)
	{
		HxLOG_Error("null pmt info\r\n");
		return ERR_FAIL;
	}

	VMX_SEM_Get(s_ulVmxStreamSemId);

	//## 1. 이미 등록된 stream 정보를 updated pmt보고 업데이트 혹은 삭제
	//##		hSession, pid, type 같으면 update
	//##			hEcm도 같으면 skip
	//##			hEcm 다르면 기존 hEcm close 시도 (다른 곳서 사용중이면 지워지지는 않을 것), 새로운 hEcm 추가 (state=used이면 filter start)
	//##			descrambler open 시도 (hEcm 있는데 descrambler 할당 못받은 경우라면)
	//##		같은거 발견되지 않으면 delete
	//##			hEcm close 시도
	//##			descrambler close 시도
	//## 2. updated pmt에 남은 stream이 있다면 추가
	//##		추가 하고, hEcm도 추가하고.. 봐서 filter나 descrambler도 open하고..

	for (i = 0; i < VMX_MAX_NUM_OF_ES; i++)
	{
		if (s_astVmxStream[i].hSession == hSession && s_astVmxStream[i].usPid != PID_NULL)
		{
			// 이미 저장된 elementary stream의 변경/삭제 여부를 확인한다
			bFound = FALSE;
			for (j = 0; j < pstPmtInfo->ulNumOfElement; j++)
			{
				if (s_astVmxStream[i].usPid == pstPmtInfo->stElementInfo[j].usPid)
				{
					//동일 es가 새로운 pmt에도 존재함 - 변경 여부 확인 (free <-> scrambled, ecm pid 변경 여부)
					HxLOG_Print("ALREADY EXIST: pid(0x%X) esType(%d)\n", pstPmtInfo->stElementInfo[j].usPid, pstPmtInfo->stElementInfo[j].eEsType);

					bFound = TRUE;
					usEcmPid = vmx_stream_GetEcmPidFromPmt(pstPmtInfo, &pstPmtInfo->stElementInfo[j]);
					if (usEcmPid == PID_NULL)
					{
						// hEcm = null 이면 변동 없는 것임, 아니라면 update
						if (s_astVmxStream[i].hEcm != HANDLE_NULL)
						{
							HxLOG_Print("UPDATE(free)\n");
							// scrambled --> free로 변경
							// descrambler slot 내린다.
							vmx_stream_CloseDescrambler(i);
							// hEcm 내린다..
							vmx_stream_UnlinkEcm(i);
							*pbDscrStopFlag = TRUE;
						}
					}
					else
					{
						// ecm pid 가 같다면 변경이 없다는 것, 아니라면 update
						if (xsvc_vmx_psi_ecm_GetPid(s_astVmxStream[i].hEcm) != usEcmPid)
						{
							HxLOG_Print("UPDATE(scr)\n");
							// hEcm 변경 필요함
							//기존 hEcm은 확인해서 내리도록하고, 새 hEcm 추가한다.
							if (s_astVmxStream[i].hEcm != HANDLE_NULL)
							{
								vmx_stream_UnlinkEcm(i);
							}
							vmx_stream_LinkEcm(hSession, i, usEcmPid);

							// descrambler는 그냥 두고, 만약 free 채널 이었다면, descrambler slot 받아야함
							if (s_astVmxStream[i].hDescrambler == HANDLE_NULL)
							{
								vmx_stream_OpenDescrambler(i, hSession);
								* pbDscrStartFlag = TRUE;
							}

							vmx_stream_StartEcmFilter(i, NULL);
						}
					}
					break;
				}
			}

			if (bFound == FALSE)
			{
				HxLOG_Print("NOT FOUND-DELETE(%x)\n", s_astVmxStream[i].usPid);
				// 동일 es가 새로운 pmt에 존재하지 않음 - 삭제 처리
				vmx_stream_Delete(i);
			}
		}
	}

	// 새로운 pmt에 추가된 stream이 있는지 확인
	for (i = 0; i < pstPmtInfo->ulNumOfElement; i++)
	{
		if ((pstPmtInfo->stElementInfo[i].usPid != PID_NULL) && (pstPmtInfo->stElementInfo[i].usPid != 0x0))
		{
			// 이미 존재하는 pid면 앞에서 update되었을 것이니 skip하고, 없던 것만 새로 추가
			for (j = 0; j < VMX_MAX_NUM_OF_ES; j++)
			{
				if ((s_astVmxStream[j].hSession == hSession) && (s_astVmxStream[j].usPid == pstPmtInfo->stElementInfo[i].usPid))
				{
					break;
				}
			}

			if (j == VMX_MAX_NUM_OF_ES)
			{
				HxLOG_Print("NEW! pid(%x) esType(%d)\n", pstPmtInfo->stElementInfo[i].usPid, pstPmtInfo->stElementInfo[i].eEsType);

				// 동일 pid 발견되지 않았음, 추가
				vmx_stream_Add(hSession, pstPmtInfo, &pstPmtInfo->stElementInfo[i], pbDscrStartFlag);
			}
		}
	}

	VMX_SEM_Release(s_ulVmxStreamSemId);
	HxLOG_Print("[Exit]\r\n");
	return ERR_OK;
}

// pmt parsing하는 곳
STATIC HERROR vmx_stream_bc_ParsePmt(Handle_t hSession, HBOOL *pbDscrStartFlag, HBOOL * pbDscrStopFlag)
{
	int 				i, j;
	HBOOL 			bFound;
	HUINT16			usEcmPid;
	SvcCas_PmtInfo_t 	*pstPmtInfo;

	HxLOG_Print("[Enter]\r\n");

	if (hSession == HANDLE_NULL)
	{
		HxLOG_Error("wrong handle\r\n");
		return ERR_FAIL;
	}

	pstPmtInfo = xsvc_vmx_psi_session_GetPmtInfo(hSession);

	if (pstPmtInfo == NULL)
	{
		HxLOG_Error("null pmt info\r\n");
		return ERR_FAIL;
	}

	VMX_SEM_Get(s_ulVmxStreamSemId);

	//## 1. 이미 등록된 stream 정보를 updated pmt보고 업데이트 혹은 삭제
	//##		hSession, pid, type 같으면 update
	//##			hEcm도 같으면 skip
	//##			hEcm 다르면 기존 hEcm close 시도 (다른 곳서 사용중이면 지워지지는 않을 것), 새로운 hEcm 추가 (state=used이면 filter start)
	//##			descrambler open 시도 (hEcm 있는데 descrambler 할당 못받은 경우라면)
	//##		같은거 발견되지 않으면 delete
	//##			hEcm close 시도
	//##			descrambler close 시도
	//## 2. updated pmt에 남은 stream이 있다면 추가
	//##		추가 하고, hEcm도 추가하고.. 봐서 filter나 descrambler도 open하고..

	for (i = 0; i < VMX_MAX_NUM_OF_ES; i++)
	{
		if (s_astVmxStream[i].hSession == hSession && s_astVmxStream[i].usPid != PID_NULL)
		{
			// 이미 저장된 elementary stream의 변경/삭제 여부를 확인한다

			bFound = FALSE;
			for (j = 0; j < pstPmtInfo->ulNumOfElement; j++)
			{
				if (s_astVmxStream[i].usPid == pstPmtInfo->stElementInfo[j].usPid)
				{
					//동일 es가 새로운 pmt에도 존재함 - 변경 여부 확인 (free <-> scrambled, ecm pid 변경 여부)
					HxLOG_Print("ALREADY EXIST(0x%x)\n", pstPmtInfo->stElementInfo[i].usPid);

					bFound = TRUE;
					usEcmPid = vmx_stream_GetEcmPidFromPmt(pstPmtInfo, &pstPmtInfo->stElementInfo[j]);

					if (usEcmPid == PID_NULL)
					{
						// TODO_VMX : 아래가 필요한지 확인 필요.
						// hEcm = null 이면 변동 없는 것임, 아니라면 update
						if (s_astVmxStream[i].hEcm != HANDLE_NULL)
						{
							HxLOG_Print("UPDATE(free)\n");
							// scrambled --> free로 변경
							// descrambler slot 내린다.
							vmx_stream_CloseDescrambler(i);
							*pbDscrStopFlag = TRUE;
							// hEcm 내린다..
							vmx_stream_UnlinkEcm(i); // ecm 관련 처리는 모두 lib outfunc에서 처리하자.
						}
					}
					else
					{
						// ecm pid 가 같다면 변경이 없다는 것, 아니라면 update
						if (xsvc_vmx_psi_ecm_GetPid(s_astVmxStream[i].hEcm) != usEcmPid)
						{
							HxLOG_Print("UPDATE(scr)\n");
							// TODO_VMX : 아래가 필요한지 확인 필요.
							#if 1
							// hEcm 변경 필요함
							//기존 hEcm은 확인해서 내리도록하고, 새 hEcm 추가한다.
							if (s_astVmxStream[i].hEcm != HANDLE_NULL)
							{
								vmx_stream_UnlinkEcm(i);
							}
							vmx_stream_LinkEcm(hSession, i, usEcmPid);
							#endif
							// descrambler는 그냥 두고, 만약 free 채널 이었다면, descrambler slot 받아야함
							if (s_astVmxStream[i].hDescrambler == HANDLE_NULL)
							{
								vmx_stream_OpenDescrambler(i, hSession);
								*pbDscrStartFlag = TRUE;
							}
							//vmx_stream_StartEcmFilter(i);
						}
					}
					break;
				}
			}

			// TODO_VMX : 아래가 필요한지 확인 필요.
			#if 1
			if (bFound == FALSE)
			{
				HxLOG_Print("NOT FOUND-DELETE(%x)\n", s_astVmxStream[i].usPid);
				// 동일 es가 새로운 pmt에 존재하지 않음 - 삭제 처리
				//vmx_stream_Delete(i);
					// descrambler slot을 close한다
				vmx_stream_CloseDescrambler(i);
				// ecm과 link 끊어요
//				vmx_stream_UnlinkEcm(i);
				s_astVmxStream[i].hSession 	= HANDLE_NULL;
				s_astVmxStream[i].eState	 	= eVmx_Stream_NotUsed;
				s_astVmxStream[i].usPid 		= PID_NULL;

				*pbDscrStopFlag  = TRUE;
			}
			#endif
		}
	}

	// 새로운 pmt에 추가된 stream이 있는지 확인
	for (i = 0; i < pstPmtInfo->ulNumOfElement; i++)
	{
		if (pstPmtInfo->stElementInfo[i].usPid != PID_NULL && pstPmtInfo->stElementInfo[i].usPid != 0x0)
		{
			// 이미 존재하는 pid면 앞에서 update되었을 것이니 skip하고, 없던 것만 새로 추가
			for (j = 0; j < VMX_MAX_NUM_OF_ES; j++)
			{
				if (s_astVmxStream[j].hSession == hSession && s_astVmxStream[j].usPid == pstPmtInfo->stElementInfo[i].usPid)
					break;
			}

			if (j == VMX_MAX_NUM_OF_ES)
			{
				HxLOG_Print("NEW!(%x)\n", pstPmtInfo->stElementInfo[i].usPid);
				// 동일 pid 발견되지 않았음, 추가
				vmx_stream_bc_Add(hSession, pstPmtInfo, &pstPmtInfo->stElementInfo[i], pbDscrStartFlag);
			}
		}
	}

	VMX_SEM_Release(s_ulVmxStreamSemId);
	HxLOG_Print("[Exit]\r\n");
	return ERR_OK;
}

STATIC HERROR vmx_stream_DeleteAll(Handle_t hStbHandle,Handle_t hSession, SvcCas_PmtInfo_t *pstPmtInfo)
{
	int i;

	HxLOG_Print("[Enter]\r\n");

	if (hSession == HANDLE_NULL)
	{
		HxLOG_Error("null handle\r\n");
		return ERR_FAIL;
	}

	VMX_SEM_Get(s_ulVmxStreamSemId);

#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
	vmx_vr_iptvh_psi_mgr_StopService(hStbHandle);
#elif defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
	#ifdef VMX_SVC_TEST
	#else
	vmx_vr_bc_StopDescrambling(hStbHandle, pstPmtInfo);
	#endif
#endif

	for (i = 0; i < VMX_MAX_NUM_OF_ES; i++)
	{
		// 해당 session의 모든 stream을 제거한다
		if (s_astVmxStream[i].hSession == hSession)
		{
			vmx_stream_Delete(i);
		}
	}

	VMX_SEM_Release(s_ulVmxStreamSemId);

	HxLOG_Print("[Exit]\r\n");
	return ERR_OK;
}

STATIC HERROR vmx_stream_IsScrambledSvc(Handle_t hSession, HBOOL *pbScrambled)
{
	int i;

	HxLOG_Print("[Enter]\r\n");

	if (hSession == HANDLE_NULL)
	{
		HxLOG_Debug("wrong handle\r\n");
		return ERR_FAIL;
	}

	*pbScrambled = FALSE;

	VMX_SEM_Get(s_ulVmxStreamSemId);

	for (i = 0; i < VMX_MAX_NUM_OF_ES; i++)
	{
		if (s_astVmxStream[i].usPid != PID_NULL && s_astVmxStream[i].hSession == hSession)
		{
			if (s_astVmxStream[i].hEcm != HANDLE_NULL)
			{
				//연결된 ecm이 있다면 scrambled svc로 본다.
				*pbScrambled = TRUE;
				break;
			}
		}
	}

	VMX_SEM_Release(s_ulVmxStreamSemId);

	HxLOG_Print("[Exit]\r\n");
	return ERR_OK;
}

#if 0
STATIC HERROR vmx_stream_GetEcmListOfService(Handle_t hSession, HUINT16 *pEcmList, HUINT8 *pCnt)
{
	int i = 0, j = 0;
	HxLOG_Print("[Enter]\r\n");

	if (hSession == HANDLE_NULL)
	{
		HxLOG_Error("wrong handle\r\n");
		return ERR_FAIL;
	}

	VMX_SEM_Get(s_ulVmxStreamSemId);

	for (i = 0; i < VMX_MAX_NUM_OF_ES; i++)
	{
		if (s_astVmxStream[i].usPid != PID_NULL && s_astVmxStream[i].hSession == hSession)
		{
			if (s_astVmxStream[i].hEcm == HANDLE_NULL)
				continue;

			pEcmList[j++] = xsvc_vmx_psi_ecm_GetPid(s_astVmxStream[i].hEcm);
		}
	}

	*pCnt = j;

	VMX_SEM_Release(s_ulVmxStreamSemId);

	HxLOG_Print("[Exit]\r\n");
	return ERR_OK;
}
#endif

// 해당 session (service)에 속한 stream이 사용하는 모든 ecm filter를 start 시킨다.
STATIC HERROR vmx_stream_StartAllEcmFilter(Handle_t hSession)
{
	int i;

	HxLOG_Print("[Enter]\r\n");

	if (hSession == HANDLE_NULL)
	{
		HxLOG_Error("wrong handle\r\n");
		return ERR_FAIL;
	}

	#if 0 // humandks_add vmx create가 안되면 return하도록 하자.
	// smart card가 없으면 filter start 할 필요 없다
	if (CXSESSION_GetScSlot(hSession) == CX_SC_NONE)
	{
		HxLOG_Error("no smartcard\r\n");
		return ERR_OK;
	}
	#endif

	VMX_SEM_Get(s_ulVmxStreamSemId);

	for (i = 0; i < VMX_MAX_NUM_OF_ES; i++)
	{
		if (s_astVmxStream[i].usPid != PID_NULL && s_astVmxStream[i].hSession == hSession)
		{
			vmx_stream_StartEcmFilter(i, NULL);
		}
	}

	VMX_SEM_Release(s_ulVmxStreamSemId);

	HxLOG_Print("[Exit]\r\n");
	return ERR_OK;
}

// 해당 session (service)에 속한 stream이 사용하는 모든 ecm filter를 stop 시킨다.
STATIC HERROR vmx_stream_StopAllEcmFilter(Handle_t hSession)
{
	int i;

	HxLOG_Print("[Enter]\r\n");

	if (hSession == HANDLE_NULL)
	{
		HxLOG_Error("null handle\r\n");
		return ERR_FAIL;
	}

	VMX_SEM_Get(s_ulVmxStreamSemId);

	for (i = 0; i < VMX_MAX_NUM_OF_ES; i++)
	{
		if (s_astVmxStream[i].usPid != PID_NULL && s_astVmxStream[i].hSession == hSession)
		{
			vmx_stream_StopEcmFilter(i);
		}
	}

	VMX_SEM_Release(s_ulVmxStreamSemId);

	HxLOG_Print("[Exit]\r\n");
	return ERR_OK;
}

// 해당 session (service)에 속한 stream이 사용하는 모든 ecm filter를 restart 시킨다.
STATIC HERROR vmx_stream_RestartAllEcmFilter(Handle_t hSession)
{
	HERROR nErr;

	HxLOG_Print("[Enter]\r\n");

	nErr = vmx_stream_StopAllEcmFilter(hSession);

	if (nErr == ERR_OK)
	{
		nErr = vmx_stream_StartAllEcmFilter(hSession);
	}

	HxLOG_Print("[Exit]\r\n");
	return nErr;
}

STATIC HERROR vmx_stream_FindStreamBySessionAndEcm(Handle_t hSession, Handle_t hEcm)
{
	int i;

	HxLOG_Print("[Enter]\r\n");

	for (i = 0; i < VMX_MAX_NUM_OF_ES; i++)
	{
		if (s_astVmxStream[i].usPid != PID_NULL
			&& s_astVmxStream[i].hSession == hSession
			&& s_astVmxStream[i].hEcm == hEcm)
		{
			break;
		}
	}

	if (i >= VMX_MAX_NUM_OF_ES)
	{
		HxLOG_Error("not found\r\n");
		return ERR_FAIL;
	}

	HxLOG_Print("[Exit]\r\n");
	return ERR_OK;
}

STATIC Handle_t vmx_stream_GetFirstSessionByEcm(Handle_t hEcm)
{
	int i;

	HxLOG_Print("[Enter]\r\n");

	for (i = 0; i < VMX_MAX_NUM_OF_ES; i++)
	{
		if (s_astVmxStream[i].usPid != PID_NULL && s_astVmxStream[i].hEcm == hEcm)
		{
			break;
		}
	}

	if (i >= VMX_MAX_NUM_OF_ES)
	{
		HxLOG_Error("not found\r\n");
		return HANDLE_NULL;
	}

	HxLOG_Print("[Exit]\r\n");
	return s_astVmxStream[i].hSession;
}

STATIC HUINT32 vmx_stream_GetAllSessionByEcm(Handle_t hEcm, Handle_t *phList)
{
	int i, j;
	HUINT32 ulCount;

	HxLOG_Print("[Enter]\r\n");

	VMX_SEM_Get(s_ulVmxStreamSemId);

	for (i = 0, ulCount = 0; i < VMX_MAX_NUM_OF_ES; i++)
	{
		if (s_astVmxStream[i].usPid != PID_NULL && s_astVmxStream[i].hEcm == hEcm)
		{
			// 혹시 같은 session을 이미 찾았었는지 확인
			for (j = 0; j < ulCount; j++)
			{
				if (phList[j] == s_astVmxStream[i].hSession)
					break;
			}

			if (j == ulCount)
			{
				phList[ulCount++] = s_astVmxStream[i].hSession;
			}
		}
	}

	VMX_SEM_Release(s_ulVmxStreamSemId);
	HxLOG_Print("[Exit]\r\n");
	return ulCount;
}

STATIC Handle_t vmx_stream_GetEcmHandleByEsPid(HUINT16 usPid)
{
	HUINT32 	i;
	Handle_t	hEcm = HANDLE_NULL;

	HxLOG_Print("[Enter]\r\n");

	if (usPid == PID_NULL)
	{
		return HANDLE_NULL;
	}

	VMX_SEM_Get(s_ulVmxStreamSemId);

	for (i = 0; i < VMX_MAX_NUM_OF_ES; i++)
	{
		if (s_astVmxStream[i].usPid == usPid)
		{
			hEcm = s_astVmxStream[i].hEcm;
			break;
		}
	}

	VMX_SEM_Release(s_ulVmxStreamSemId);
	HxLOG_Print("[Exit]\r\n");

	return hEcm;
}

HERROR  xsvc_vmx_psi_stream_SetPid(Handle_t hSession, HUINT16 usPid, HBOOL bAdd, HBOOL *pbDscrStartFlag, HBOOL * pbDscrStopFlag)
{
// VMX_TODO : 아래 두개를 통합하자.
#if defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
	return vmx_stream_bc_SetPid(hSession, usPid, bAdd, pbDscrStartFlag, pbDscrStopFlag);
#else
	return vmx_stream_SetPid(hSession, usPid, bAdd, pbDscrStartFlag, pbDscrStopFlag);
#endif
}

#if 0
HBOOL   xsvc_vmx_psi_stream_IsVmxStream(Handle_t hSession)
{
	return vmx_stream_IsVmxStream(hSession);
}
#endif

Handle_t xsvc_vmx_psi_stream_GetEcmHandleByEsPid(HUINT16 usPid)
{
	return vmx_stream_GetEcmHandleByEsPid(usPid);
}

HUINT16  xsvc_vmx_psi_stream_GetEcmPidFromPmt(SvcCas_PmtInfo_t *pstPmtInfo, SvcCas_PmtElement_t *pstEsInfo)
{
	return vmx_stream_GetEcmPidFromPmt(pstPmtInfo, pstEsInfo);
}

HERROR  xsvc_vmx_psi_stream_ParsePmt(Handle_t hSession, HBOOL *pbDscrStartFlag, HBOOL * pbDscrStopFlag)
{
// VMX_TODO : 아래 두개를 통합하자.
#if defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
	return vmx_stream_bc_ParsePmt(hSession, pbDscrStartFlag, pbDscrStopFlag);
#else
	return vmx_stream_ParsePmt(hSession, pbDscrStartFlag, pbDscrStopFlag);
#endif
}

HERROR  xsvc_vmx_psi_stream_DeleteAll(Handle_t hStbHandle,Handle_t hSession, SvcCas_PmtInfo_t *pstPmtInfo)
{
	return vmx_stream_DeleteAll(hStbHandle, hSession, pstPmtInfo);
}

HERROR  xsvc_vmx_psi_stream_IsScrambledSvc(Handle_t hSession, HBOOL *pbScrambled)
{
	return vmx_stream_IsScrambledSvc(hSession, pbScrambled);
}

HERROR  xsvc_vmx_psi_stream_StartAllEcmFilter(Handle_t hSession)
{
	return vmx_stream_StartAllEcmFilter(hSession);
}

HERROR  xsvc_vmx_psi_stream_StopAllEcmFilter(Handle_t hSession)
{
	return vmx_stream_StopAllEcmFilter(hSession);
}

HERROR  xsvc_vmx_psi_stream_RestartAllEcmFilter(Handle_t hSession)
{
	return vmx_stream_RestartAllEcmFilter(hSession);
}

HERROR  xsvc_vmx_psi_stream_FindStreamBySessionAndEcm(Handle_t hSession, Handle_t hEcm)
{
	return vmx_stream_FindStreamBySessionAndEcm(hSession, hEcm);
}

Handle_t  xsvc_vmx_psi_stream_GetFirstSessionByEcm(Handle_t hEcm)
{
	return vmx_stream_GetFirstSessionByEcm(hEcm);
}

HUINT32  xsvc_vmx_psi_stream_GetAllSessionByEcm(Handle_t hEcm, Handle_t *phList)
{
	return vmx_stream_GetAllSessionByEcm(hEcm, phList);
}

HERROR xsvc_vmx_psi_stream_InitCore(void)
{
	return vmx_stream_InitCore();
}

