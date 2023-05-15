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
	@file     xsvc_cas_vmx_psi_session.c
	@brief    xsvc_cas_vmx_psi_session.c (Verimatrix PSI SESSION)

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
#elif defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
#include "_xsvc_cas_vmx_bc_main.h"
#endif
#include <util.h>
#include "_xsvc_cas_vmx_psi.h"
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

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HULONG 		s_ulVmxSessionSemId;

Vmx_Session_t 		s_astVmxSession[VMX_MAX_NUM_OF_SESSION];


/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
STATIC Vmx_Session_t *vmx_session_GetSession(Handle_t hStbHandle);
STATIC HINT32 vmx_session_FindIndexByStbHandle(Handle_t hStbHandle);
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
#define INIT_________________________________________________

STATIC HERROR vmx_session_InitCore(void)
{
	HUINT16 i;
	HINT32 nResult = 0;

	nResult = VMX_SEM_Create("VmxSessSem", &s_ulVmxSessionSemId);
	if(nResult != ERR_OK)
	{
		HxLOG_Error("Failure in Semaphore Creation \r\n");
		return ERR_FAIL;
	}

	VMX_memset(s_astVmxSession, 0, sizeof(Vmx_Session_t) * VMX_MAX_NUM_OF_SESSION);

	for (i = 0; i < VMX_MAX_NUM_OF_SESSION; i++)
	{
		s_astVmxSession[i].hStbHandle			= HANDLE_NULL;
		s_astVmxSession[i].nSessionIdx			= VMX_INVALID_SESSION_NUM;
		s_astVmxSession[i].eState 				= eVmx_Session_Init;
		s_astVmxSession[i].bSelected 			= FALSE;
		s_astVmxSession[i].usUniqueTsId 			= 0;
		s_astVmxSession[i].pstPmtInfo			= NULL;
		s_astVmxSession[i].stEsPidInfo.ulPidNum 	= 0;
	}

	return ERR_OK;
}

#define SESSION_TABLE______________________________________________

STATIC void vmx_session_DeleteEmm(HUINT16 usUniqueTsId)
{
	HUINT16 		i;
	HBOOL 	bTsIdNotFound = TRUE;

	for (i = 0; i < VMX_MAX_NUM_OF_SESSION; i++)
	{
		if (s_astVmxSession[i].usUniqueTsId == usUniqueTsId)
		{
			bTsIdNotFound = FALSE;
			break;
		}
	}

	if (bTsIdNotFound)
	{
		// ���� TS ID�� �����ϴ� session�� ���ٸ� EMM ����
		if(xsvc_vmx_psi_emm_DeleteByTsId(usUniqueTsId) != ERR_OK)
		{

		}
	}
}

STATIC Vmx_Session_t *vmx_session_GetSession(Handle_t hStbHandle)
{
	Vmx_Session_t 	*pstSession = NULL;
	HUINT16 			i;

	HxLOG_Print("[Enter]\r\n");

	for (i = 0; i < VMX_MAX_NUM_OF_SESSION; i++)
	{
		if (s_astVmxSession[i].hStbHandle == hStbHandle)
		{
			pstSession = &s_astVmxSession[i];
			break;
		}
	}
	HxLOG_Print("[Exit]\r\n");
	return pstSession;
}

STATIC HINT32 vmx_session_FindIndexByStbHandle(Handle_t hStbHandle)
{
	HINT32 nIdx;

	HxLOG_Print("[Enter]\r\n");
	for (nIdx = 0; nIdx < VMX_MAX_NUM_OF_SESSION; nIdx++)
	{
		if (s_astVmxSession[nIdx].hStbHandle == hStbHandle)
		{
			HxLOG_Print("[Exit]\r\n");
			return nIdx;
		}
	}

	HxLOG_Error("\r\n");
	return -1;
}

STATIC HERROR vmx_session_StartSvc(Handle_t hStbHandle)
{
	HUINT32 	i;
//	Handle_t	hSession = HANDLE_NULL;

	HxLOG_Print("[Enter]\r\n");

	VMX_SEM_Get(s_ulVmxSessionSemId);

	// ��� �� session �Ҵ� : �ϴ� �׳� �տ��� ���� �þ� ���´�
	for (i = 0; i < VMX_MAX_NUM_OF_SESSION; i++)
	{
		if (s_astVmxSession[i].hStbHandle == HANDLE_NULL)
		{
			s_astVmxSession[i].hStbHandle 	= hStbHandle;

			if (s_astVmxSession[i].pstPmtInfo)
			{
				HxLOG_Error("\r\n");
				VMX_MEM_Free(s_astVmxSession[i].pstPmtInfo);
				s_astVmxSession[i].pstPmtInfo	= NULL;
			}

//			hSession = (Handle_t)&s_astVmxSession[i];
			break;
		}
	}
	VMX_SEM_Release(s_ulVmxSessionSemId);

	HxLOG_Print("[Exit]\r\n");
	return ERR_OK;
}

// bNextSvcExist; �𸦶��� TRUE�� �ش�.
STATIC HERROR vmx_session_StopSvc(Handle_t hStbHandle, HBOOL bNextSvcExist)
{
	HINT32 		nIdx;
	//HUINT16		usTmp;

	HxLOG_Print("[Enter]\r\n");

	VMX_SEM_Get(s_ulVmxSessionSemId);
	nIdx = vmx_session_FindIndexByStbHandle(hStbHandle);

	if (nIdx < 0)
	{
		VMX_SEM_Release(s_ulVmxSessionSemId);
		return ERR_FAIL;
	}

	// session�� �ش��ϴ� stream�� �����ϸ� link�� ecm���� ���������� �ڵ����� ����
	xsvc_vmx_psi_stream_DeleteAll(hStbHandle,(Handle_t)&s_astVmxSession[nIdx], s_astVmxSession[nIdx].pstPmtInfo); // humandks_add

	// EsPidInfo �ʱ�ȭ
	VMX_memset(&s_astVmxSession[nIdx].stEsPidInfo, 0, sizeof(Vmx_EsPidInfo_t));

	s_astVmxSession[nIdx].hStbHandle 	= HANDLE_NULL;
	s_astVmxSession[nIdx].bSelected 	= FALSE;

	if (s_astVmxSession[nIdx].pstPmtInfo)
	{
		VMX_MEM_Free(s_astVmxSession[nIdx].pstPmtInfo);
		s_astVmxSession[nIdx].pstPmtInfo	= NULL;
	}

	if (bNextSvcExist == FALSE)
	{
		// ���� svc�� ������ �ش� TP�� ���̻� ������� �ʴ� ������ ����
		//usTmp = s_astVmxSession[nIdx].usUniqueTsId;
		s_astVmxSession[nIdx].usUniqueTsId = 0;
		vmx_session_DeleteEmm(s_astVmxSession[nIdx].usUniqueTsId);
	}

	VMX_SEM_Release(s_ulVmxSessionSemId);
	HxLOG_Print("[Exit]\r\n");
	return ERR_OK;
}

STATIC HERROR vmx_session_SetPid(Handle_t hStbHandle, Vmx_EsPidInfo_t *pstEsPidInfo)
{
	Vmx_Session_t 	*pSession;
	HUINT16		usPid, usNewPid;
	HBOOL bDscrStartFlag = FALSE;
	HBOOL bDscrStopFlag = FALSE;
	int			i;

	HxLOG_Print("[Enter]\r\n");

	VMX_SEM_Get(s_ulVmxSessionSemId);

	pSession = vmx_session_GetSession(hStbHandle);
	if (pSession == NULL)
	{
		VMX_SEM_Release(s_ulVmxSessionSemId);
		HxLOG_Error("Error! vmx_session_GetSession() fail. hStbHandle(0x%X)\n", hStbHandle);
		return ERR_FAIL;
	}

	//stream table�� ���

	// ������ ��� �ִ� pid ��
	for (i = 0; i  < pSession->stEsPidInfo.ulPidNum; i++)
	{
		usPid = pSession->stEsPidInfo.usPid[i];
		usNewPid = pstEsPidInfo->usPid[i];

		if (usPid != usNewPid)
		{
#ifdef VMX_SVC_TEST
			// VMX_TODO vmx lib���� descramble stop���� di dsc�� close�ؾ������� �켱 ���⼭ delete�� ���� ����.
#else
			//pid�� �ٲ������ ���� �Ŵ� �����, ���ο� ���� ���
			if (usPid != PID_NULL && usPid != 0x0)
			{
				xsvc_vmx_psi_stream_SetPid((Handle_t)pSession, usPid, VMX_PID_DELETE, &bDscrStartFlag, &bDscrStopFlag);
			}
#endif
			if (usNewPid != PID_NULL && usNewPid != 0x0)
			{
				xsvc_vmx_psi_stream_SetPid((Handle_t)pSession, usNewPid, VMX_PID_ADD, &bDscrStartFlag, &bDscrStopFlag);
			}

			pSession->stEsPidInfo.usPid[i] = usNewPid;
		}
	}

	// ���� �߰��� pid
	for (; i < pstEsPidInfo->ulPidNum; i++)
	{
		usNewPid = pstEsPidInfo->usPid[i];

		if (usNewPid != PID_NULL && usNewPid != 0x0)
		{
			xsvc_vmx_psi_stream_SetPid((Handle_t)pSession, usNewPid, VMX_PID_ADD, &bDscrStartFlag, &bDscrStopFlag);
		}

		// ���ο� pid�� ���
		pSession->stEsPidInfo.usPid[i] = usNewPid;
	}

	// ���ο� pidNum ���
	pSession->stEsPidInfo.ulPidNum = pstEsPidInfo->ulPidNum;

#if 1 /* VMX_TODO : ���� ���� �ʿ� */
	if(bDscrStopFlag)
	{
		HxLOG_Print("bDscrStopFlag = TRUE\n");
#if defined(CONFIG_MW_CAS_VERIMATRIX_DVB)

#elif  defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
		vmx_vr_iptvh_psi_mgr_StopService(hStbHandle);
#endif
	}

	/**
	  *	network search�� ã���� ���񽺷� ó�� zapping�ϴ� ��� descramble �ȵǴ� ���� ����
	  *
	  *	pid�� ���� ��Ȳ���� cas�� ���۵Ǿ��ٸ� ó�� set pid�ÿ� ��� PID_NULL�� �����´�
	  *	pmt update�Ǹ鼭 add es pid, descrambler open, set ecm filter�� ����ǰ�
	  *	���� set pid�� ����� �� pid�� ���������� �̹� vmx stream�ʿ� add es pid�� �Ǿ� �����Ƿ� bDscrStartFlag�� FALSE�� ���ϵȴ�
	  * 	���� try�� �������� �ϴ� start service�� ȣ���غ��� (���ǿ� �� ������ ���۵��� ���� ����)
	**/
	//if(bDscrStartFlag)
	{
		HxLOG_Print("bDscrStartFlag = TRUE\n");
#if defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
	#ifdef VMX_SVC_TEST
		vmx_vr_dscr_StartDescrambling(hStbHandle, pSession->pstPmtInfo);
	#else
		// TODO_VMX : VMX ���� : �߰��� ECMPID�� ����Ǵ°��. or PID�� ����Ǵ� ��� BC_StopDescrambling�� �ٽ� ȣȦ���־���ϴ���?
		vmx_vr_bc_StartDescrambling(hStbHandle, pSession->pstPmtInfo);
	#endif
#elif defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
		vmx_vr_iptvh_psi_mgr_StartService(hStbHandle);
#endif
	}
#endif

	VMX_SEM_Release(s_ulVmxSessionSemId);

	HxLOG_Print("[Exit]\r\n");
	return ERR_OK;
}

STATIC HERROR vmx_session_UpdatePmt(Handle_t hStbHandle, SvcCas_PmtInfo_t *pstPmtInfo)
{
	Vmx_Session_t 	*pSession;
	HBOOL		bScrambled = FALSE;
	HBOOL 		bDscrStartFlag = FALSE;
	HBOOL 		bDscrStopFlag = FALSE;

	HxLOG_Print("[Enter]\r\n");

	if (pstPmtInfo == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_FAIL;
	}
	VMX_SEM_Get(s_ulVmxSessionSemId);

	pSession = vmx_session_GetSession(hStbHandle);
	if (pSession == NULL)
	{
		VMX_SEM_Release(s_ulVmxSessionSemId);
		HxLOG_Error("\r\n");
		return ERR_FAIL;
	}

	// pmt ����
	if (pSession->pstPmtInfo == NULL)
	{
		pSession->pstPmtInfo = VMX_MEM_Alloc(sizeof(SvcCas_PmtInfo_t));
	}

	if (pSession->pstPmtInfo != NULL)
	{
		VMX_memcpy(pSession->pstPmtInfo, pstPmtInfo, sizeof(SvcCas_PmtInfo_t));
	}

	if (xsvc_vmx_psi_stream_ParsePmt((Handle_t)pSession, &bDscrStartFlag, &bDscrStopFlag) == ERR_OK)
	{
		if (xsvc_vmx_psi_stream_IsScrambledSvc((Handle_t)pSession, &bScrambled) == ERR_OK)
		{
			if (bScrambled == TRUE)
			{
				//scrambled svc
				pSession->eState = eVmx_Session_Used;
			}
			else
			{
				// free svc
				if (pSession->eState == eVmx_Session_Used)
				{
					// scrambed -> free��Ȳ�̹Ƿ� reset session �ʿ���
					//CXSC_ResetSession(pSession->nScSlot, pSession->nSessionIdx); humandks_add
				}
				pSession->eState = eVmx_Session_Ready;
			}
		}

#if 1 /* VMX_TODO : ���� ���� �ʿ� */
	if(bDscrStopFlag)
	{
		HxLOG_Print("bDscrStopFlag = TRUE\n");
#if defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
		// VMX_TODO
#elif  defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
		vmx_vr_iptvh_psi_mgr_StopService(hStbHandle);
#endif
	}

	if(bDscrStartFlag)
	{
		// TODO_VMX : VMX ���� : �߰��� ECMPID�� ����Ǵ°��. or PID�� ����Ǵ� ��� BC_StopDescrambling�� �ٽ� ȣȦ���־���ϴ���?
		HxLOG_Print("bDscrStartFlag = TRUE\n");
#if defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
	#ifdef VMX_SVC_TEST
		vmx_vr_dscr_StartDescrambling(hStbHandle, pstPmtInfo);
	#else
		vmx_vr_bc_StartDescrambling(hStbHandle, pstPmtInfo);
	#endif
#elif defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
		vmx_vr_iptvh_psi_mgr_StartService(hStbHandle);
#endif
	}
#else
		// ecm filter start (�غ��µ�, s/c�� ���ų� used ������ �ȵǸ� filter start�� �ȵ� ����.. try�� ����)
		xsvc_vmx_psi_stream_StartAllEcmFilter((Handle_t)pSession);
#endif
	}

	#if 0
	if (bScrambled || xsvc_vmx_psi_stream_IsVmxStream((Handle_t)pSession))
	{
//		VMXIPTV_Main_ServiceStart(hStbHandle)
		//CXUI_SessionStart((Handle_t)pSession); // humandks_add
	}
	#endif

	VMX_SEM_Release(s_ulVmxSessionSemId);
	HxLOG_Print("[Exit]\r\n");

	return ERR_OK;
}

STATIC HERROR vmx_session_ProcessCat(Handle_t hStbHandle, HUINT16 usUniqueTsId, HUINT8 *pucCat)
{
	Vmx_Session_t 	*pSession;
	HUINT16		usTmp;

	HxLOG_Print("[Enter]\r\n");

	if (pucCat == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_FAIL;
	}

	VMX_SEM_Get(s_ulVmxSessionSemId);
	pSession = vmx_session_GetSession(hStbHandle);
	VMX_SEM_Release(s_ulVmxSessionSemId);

	if (pSession == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_FAIL;
	}

	if (pSession->usUniqueTsId != usUniqueTsId)
	{
		// ���� ����� uniqueTsId�� �ٸ��� ���� ts id�� �ݵ��� ��û
		usTmp = pSession->usUniqueTsId;
		pSession->usUniqueTsId = usUniqueTsId;

		vmx_session_DeleteEmm(usTmp);
	}

	// cat �����ؼ� ó��
	xsvc_vmx_psi_emm_UpdateCat((Handle_t)pSession, usUniqueTsId, pucCat);

//	VMX_SEM_Release(s_ulVmxSessionSemId);

	HxLOG_Print("[Exit]\r\n");

	return ERR_OK;
}

STATIC HERROR vmx_session_Select(Handle_t hStbHandle, HBOOL bSelect)
{
	Vmx_Session_t 	*pSession;

	HxLOG_Print("[Enter][%x]\r\n", hStbHandle);

	VMX_SEM_Get(s_ulVmxSessionSemId);

	pSession = vmx_session_GetSession(hStbHandle);
	if (pSession == NULL)
	{
		VMX_SEM_Release(s_ulVmxSessionSemId);
		HxLOG_Error("\r\n");
		return ERR_FAIL;
	}

	pSession->bSelected = bSelect;

	VMX_SEM_Release(s_ulVmxSessionSemId);
	HxLOG_Print("[Exit(%d)]\r\n", pSession->bSelected);
	return ERR_OK;
}

STATIC HERROR vmx_session_UpdateCat(Handle_t hStbHandle, HUINT16 usUniqueTsId, HUINT8 *pucCat)
{
	HUINT16		usLen;
	HUINT8		*pucCatRaw;

	HxLOG_Print("[Enter]\r\n");

	if (pucCat == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_FAIL;
	}

	usLen = get12bit(pucCat + 1) + 3;

	pucCatRaw = (HUINT8 *)VMX_MEM_Alloc(usLen);
	if (pucCatRaw == NULL)
	{
		HxLOG_Error("null cat\r\n");
		return ERR_FAIL;
	}

	VMX_memcpy(pucCatRaw, pucCat, usLen);

	vmx_session_ProcessCat(hStbHandle, usUniqueTsId, pucCatRaw);

	HxLOG_Print("[Exit]\r\n");

	return ERR_OK;
}

STATIC Handle_t vmx_session_GetStbHandle(Handle_t hSession)
{
	Vmx_Session_t *pSession;

	if (hSession == HANDLE_NULL)
	{
		HxLOG_Error("\r\n");
		return HANDLE_NULL;
	}

	pSession = (Vmx_Session_t *)hSession;

	if (pSession == NULL)
	{
		HxLOG_Error("\r\n");
		return HANDLE_NULL;
	}

	return pSession->hStbHandle;
}

STATIC HUINT32 vmx_session_GetStbActionId(Handle_t hSession)
{
	Vmx_Session_t *pSession;

	if (hSession == HANDLE_NULL)
	{
		HxLOG_Error("\r\n");
		return HANDLE_NULL;
	}

	pSession = (Vmx_Session_t *)hSession;

	if (pSession == NULL)
	{
		HxLOG_Error("\r\n");
		return HANDLE_NULL;
	}

	return (pSession->hStbHandle >> 24);
}

STATIC SvcCas_PmtInfo_t *vmx_session_GetPmtInfo(Handle_t hSession)
{
	Vmx_Session_t *pSession;

	if (hSession == HANDLE_NULL)
	{
		HxLOG_Error(" Handle is null....\r\n");
		return NULL;
	}

	pSession = (Vmx_Session_t *)hSession;

	if (pSession == NULL)
	{
		HxLOG_Error("Session is null....\r\n");
		return NULL;
	}

	return pSession->pstPmtInfo;
}

STATIC HINT32 vmx_session_GetSessionCnt(void)
{
	int i;
	HINT32	nCnt=0;

	for (i = 0; i < VMX_MAX_NUM_OF_SESSION; i++)
	{
		if (s_astVmxSession[i].hStbHandle != HANDLE_NULL)
		{
			nCnt++;
		}
	}
	return nCnt;
}

STATIC HINT32 vmx_session_GetSessionIdx(Handle_t hSession)
{
	Vmx_Session_t *pSession;

	if (hSession == HANDLE_NULL)
	{
		HxLOG_Error("\r\n");
		return VMX_INVALID_SESSION_NUM;
	}

	pSession = (Vmx_Session_t *)hSession;

	if (pSession == NULL)
	{
		HxLOG_Error("\r\n");
		return VMX_INVALID_SESSION_NUM;
	}

	return pSession->nSessionIdx;
}

STATIC HBOOL vmx_session_IsSelected(Handle_t hSession)
{
	Vmx_Session_t *pSession = NULL;

	if (hSession == HANDLE_NULL)
	{
		HxLOG_Error("\r\n");
		return FALSE;
	}

	pSession = (Vmx_Session_t *)hSession;
	if (pSession == NULL)
	{
		HxLOG_Error("\r\n");
		return FALSE;
	}

	return pSession->bSelected;
}

STATIC HBOOL vmx_session_IsTsrSession(Handle_t hSession)
{
	Vmx_Session_t *pSession;
	HBOOL		bIsTsr = FALSE;

	if (hSession == HANDLE_NULL)
	{
		HxLOG_Error("\r\n");
		return HANDLE_NULL;
	}

	pSession = (Vmx_Session_t *)hSession;

	if (pSession == NULL)
	{
		HxLOG_Error("\r\n");
		return HANDLE_NULL;
	}

//	bIsTsr = CXMGR_IsTsrActionTypeFromStbHandle(pSession->hStbHandle); humandks_add

	return bIsTsr;
}

STATIC Handle_t vmx_session_GetHandleByStbHandle(Handle_t hStbHandle)
{
	Vmx_Session_t 	*pSession;

	VMX_SEM_Get(s_ulVmxSessionSemId);

	pSession = vmx_session_GetSession(hStbHandle);

	VMX_SEM_Release(s_ulVmxSessionSemId);

	if (pSession == NULL)
	{
		HxLOG_Error("pSession is NULL\n");
		return HANDLE_NULL;
	}

	return (Handle_t)pSession;
}

STATIC HUINT32 vmx_session_GetPidNum(Handle_t hStbHandle)
{
	Vmx_Session_t *pSession = vmx_session_GetSession(hStbHandle);

	if (pSession == NULL)
	{
		return 0;
	}

	return 	pSession->stEsPidInfo.ulPidNum;
}

STATIC HUINT16 *vmx_session_GetPidList(Handle_t hStbHandle)
{
	Vmx_Session_t *pSession = vmx_session_GetSession(hStbHandle);

	if (pSession == NULL)
	{
		return 0;
	}

	return 	&pSession->stEsPidInfo.usPid[0];
}

STATIC Vmx_EsPidInfo_t * vmx_session_GetSessonInfoByStbHandle(Handle_t hStbHandle)
{
	Vmx_Session_t *pSession = vmx_session_GetSession(hStbHandle);

	if (pSession == NULL)
	{
		return 0;
	}

	return 	&pSession->stEsPidInfo;
}

Vmx_Session_t *xsvc_vmx_psi_session_GetSessionFromhStbHandle(Handle_t hStbHandle)
{
	return vmx_session_GetSession(hStbHandle);
}

HUINT32 xsvc_vmx_psi_session_GetPidNum(Handle_t hStbHandle)
{
	return vmx_session_GetPidNum(hStbHandle);
}

HUINT16 *xsvc_vmx_psi_session_GetPidList(Handle_t hStbHandle)
{
	return vmx_session_GetPidList(hStbHandle);
}

Handle_t xsvc_vmx_psi_session_GetStbHandle(Handle_t hSession)
{
	return vmx_session_GetStbHandle(hSession);
}

HUINT32 xsvc_vmx_psi_session_GetStbActionId(Handle_t hSession)
{
	return vmx_session_GetStbActionId(hSession);
}

SvcCas_PmtInfo_t *xsvc_vmx_psi_session_GetPmtInfo(Handle_t hSession)
{
	return vmx_session_GetPmtInfo(hSession);
}

HINT32 xsvc_vmx_psi_session_GetSessionCnt(void)
{
	return vmx_session_GetSessionCnt();
}

HINT32 xsvc_vmx_psi_session_GetSessionIdx(Handle_t hSession)
{
	return vmx_session_GetSessionIdx(hSession);
}

HBOOL xsvc_vmx_psi_session_IsSelected(Handle_t hSession)
{
	return vmx_session_IsSelected(hSession);
}

HBOOL xsvc_vmx_psi_session_IsTsrSession(Handle_t hSession)
{
	return vmx_session_IsTsrSession(hSession);
}

Handle_t xsvc_vmx_psi_session_GetHandleByStbHandle(Handle_t hStbHandle)
{
	return vmx_session_GetHandleByStbHandle(hStbHandle);
}

Vmx_EsPidInfo_t * xsvc_vmx_psi_session_GetSessonInfoByStbHandle(Handle_t hStbHandle)
{
	return vmx_session_GetSessonInfoByStbHandle(hStbHandle);
}

HERROR xsvc_vmx_psi_session_StartSvc(Handle_t hStbHandle)
{
	return vmx_session_StartSvc(hStbHandle);
}

HERROR xsvc_vmx_psi_session_StopSvc(Handle_t hStbHandle, HBOOL bNextSvcExist)
{
	return vmx_session_StopSvc(hStbHandle, bNextSvcExist);
}

// pstEsPidInfo���� EsType�� ������� �ʴ´�. ���, �� esType�� �׻� ���� index�� ���;� �Ѵ�.
// ���� ���, (V, A1, A2, A3, S, T)�� ����� �����ߴٸ� �׻� �̷��� ������ ������ ���缭 ���;� �Ѵ�.
// �ϳ��� pid�� �ٲ���ٰ� �ϳ��� ������ �ٸ� pid�� ������� �ʴ� ������ �����Ѵ�.
HERROR xsvc_vmx_psi_session_SetPid(Handle_t hStbHandle, Vmx_EsPidInfo_t *pstEsPidInfo)
{
	return vmx_session_SetPid(hStbHandle, pstEsPidInfo);
}

HERROR xsvc_vmx_psi_session_UpdatePmt(Handle_t hStbHandle, SvcCas_PmtInfo_t *pstPmtInfo)
{
	return vmx_session_UpdatePmt(hStbHandle, pstPmtInfo);
}

HERROR xsvc_vmx_psi_session_UpdateCat(Handle_t hStbHandle, HUINT16 usUniqueTsId, HUINT8 *pucCat)
{
	return vmx_session_UpdateCat(hStbHandle, usUniqueTsId, pucCat);
}

HERROR xsvc_vmx_psi_session_Select(Handle_t hStbHandle, HBOOL bSelect)
{
	return vmx_session_Select(hStbHandle, bSelect);
}

HERROR xsvc_vmx_psi_session_Init(void)
{
	return vmx_session_InitCore();
}

