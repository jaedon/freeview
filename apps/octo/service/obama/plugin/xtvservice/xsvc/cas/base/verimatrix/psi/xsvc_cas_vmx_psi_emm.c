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
	@file     xsvc_cas_vmx_psi_emm.c
	@brief    xsvc_cas_vmx_psi_emm.c (Verimatrix EMM)

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
#include "vmx_vr_iptvh_api.h"
#include "vmx_vr_iptvh_mgr.h"
#elif defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
#include "_xsvc_cas_vmx_bc_main.h"
#include "vmx_vr_bc_api.h"
#include "vmx_vr_bc_mgr.h"
#endif

#include <util.h>

#include "_svc_cas_mgr_common.h"
#include "_svc_cas_mgr_svc_port.h"
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
STATIC HULONG 			s_ulVmxEmmSemId;
STATIC Vmx_Emm_t 		s_astVmxEmm[VMX_MAX_NUM_OF_EMM];

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

STATIC HERROR vmx_emm_InitCore(void)
{
	int 		i;
	HINT32 	nResult = 0;

	HxLOG_Print("[Enter]\r\n");

	nResult = VMX_SEM_Create("VmxEmmSem", &s_ulVmxEmmSemId);
	if(nResult != ERR_OK)
	{
		HxLOG_Error("Failure in Semaphore Creation \r\n");
		return ERR_FAIL;
	}

	VMX_memset(s_astVmxEmm, 0, sizeof(Vmx_Emm_t) * VMX_MAX_NUM_OF_EMM);

	for (i = 0; i < VMX_MAX_NUM_OF_EMM; i++)
	{
		s_astVmxEmm[i].hSession 			= HANDLE_NULL;
		s_astVmxEmm[i].usUniqueTsId		= 0;
//		s_astVmxEmm[i].nScSlot 			=
		s_astVmxEmm[i].pucCatRaw 			= NULL;
		s_astVmxEmm[i].usEmmPid 			= PID_NULL;
		s_astVmxEmm[i].usEmmAddrCnt		= VMX_MAX_EMM_FILTER_NUM;
	}

	HxLOG_Print("[Exit]\r\n");
	return ERR_OK;
}

HERROR vmx_emm_IsVmxCasystemId(HUINT16 usCaSysId)
{
	HUINT16	usCurCaSysId;

	HxLOG_Print("[Enter] usCaSysId = 0x%x\n", usCaSysId);
#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
	usCurCaSysId = eDxCAS_GROUPID_VX;
#elif defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
//	usCurCaSysId = eGroupID_VMXBC; // test only 적요해야함.
	vmx_vr_bc_mgr_GetCaSystemId(&usCurCaSysId); // test only test 제거
#endif

	if((usCaSysId/* & 0xff00*/) == (usCurCaSysId/* & 0xff00*/))
	{
		HxLOG_Print("[Exit] usCaSysId = 0x%x\n", usCaSysId);
		return ERR_OK;
	}
	else
	{
		HxLOG_Error("CasystemId is not exist \r\n");
		return ERR_FAIL;
	}
}

HERROR vmx_emm_ParseCat(Vmx_Emm_t *pEmm,  HUINT8 *pucCat)
{
	HUINT8 *pucTmp = NULL;
	HUINT32  ulLen = 0;
	HUINT32 ulLenDec = 0;
	HUINT16 usEmmPid = 0xFFFF;

	HxLOG_Print("[Enter]\r\n");

	if(pEmm == NULL || pucCat == NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	//MDrv_VmxCA_FreeAllEmm(); EMM FREE 확인 필요. TODO_VMX

	ulLen = pucCat[1] & 0x0f;
	ulLen = (ulLen << 8) | pucCat[2];
	ulLen = ulLen - 5;

	pucTmp = pucCat + 8;

	HxLOG_Print("ulLen = %d\n", ulLen);

	xsvc_vmx_main_PrintDump(VMX_DBG_PSI_EMM, "Parse CAT", pucTmp, ulLen);

	while((ulLen - 4) > 0)
	{
		ulLenDec = pucTmp[1];

		if(pucTmp[0] == 0x09)	// descriptor tag
		{
			HxLOG_Print("CaSystemId = 0x%x\n", pucTmp[2] << 8 | pucTmp[3]);
			if(vmx_emm_IsVmxCasystemId(pucTmp[2] << 8 | pucTmp[3]) == ERR_OK)
			{
				usEmmPid = (pucTmp[4] << 8 | pucTmp[5]) & 0x1fff;
				HxLOG_Print("usEmmPid = 0x%x\n", usEmmPid);
			}
		}

		pucTmp = pucTmp + ulLenDec + 2;
		ulLen = ulLen - ulLenDec - 2;
	}

#if defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
	vmx_vr_bc_emm_SetEmmPidToBC(usEmmPid);
#endif

	HxLOG_Print("[Exit]\r\n");

	return ERR_OK;
}

STATIC HERROR vmx_emm_UpdateCa(Handle_t hSession, HUINT16 usUniqueTsId, HUINT8 *pucCat)
{
	int 		i;
//	HINT32 	nScSlot;
	HBOOL	bNotFound = TRUE;

	HxLOG_Print("[Enter]\r\n");

	VMX_SEM_Get(s_ulVmxEmmSemId);

//	nScSlot = CXSESSION_GetScSlot(hSession);

	// unique ts id를 우선 비교, 같은 것이 있으면 cat 비교후 처리. 같은게 없으면 새로 등록
	for (i = 0; i <	VMX_MAX_NUM_OF_EMM; i++)
	{
		// ts id
		if (s_astVmxEmm[i].usUniqueTsId == usUniqueTsId)
		{
			// 기존 cat와 비교해서 같으면 무시
			if (s_astVmxEmm[i].pucCatRaw && get12bit(pucCat + 1) == get12bit(s_astVmxEmm[i].pucCatRaw + 1))
			{
				if (VMX_memcmp(pucCat, s_astVmxEmm[i].pucCatRaw, get12bit(pucCat + 1)) == 0)
				{
					HxLOG_Error("same pucCat: ignore\r\n");
					VMX_SEM_Release(s_ulVmxEmmSemId);
					return ERR_FAIL;
				}
			}

			bNotFound = FALSE;
			break;
		}
	}

	if (bNotFound)
	{
		// 같은 ts가 없으면 새로 등록한다.
		for (i = 0; i <	VMX_MAX_NUM_OF_EMM; i++)
		{
			if (s_astVmxEmm[i].usUniqueTsId == 0)
			{
//				s_astVmxEmm[i].nScSlot 		= nScSlot;
				s_astVmxEmm[i].usUniqueTsId 	= usUniqueTsId;
				bNotFound = FALSE;
				break;
			}
		}

		if (bNotFound)
		{
			// 빈곳도 없다면 에러
			HxLOG_Error("no slot available\r\n");
			VMX_SEM_Release(s_ulVmxEmmSemId);
			return ERR_FAIL;
		}
	}

	// cat가 update될 것이므로 이전 것은 지운다.
	if (s_astVmxEmm[i].pucCatRaw)
	{
		VMX_MEM_Free(s_astVmxEmm[i].pucCatRaw);
	}

	// cat 등록
	s_astVmxEmm[i].hSession 		= hSession;
	s_astVmxEmm[i].pucCatRaw 		= pucCat;

	vmx_emm_ParseCat(&s_astVmxEmm[i], pucCat);

	VMX_SEM_Release(s_ulVmxEmmSemId);

	HxLOG_Print("[Exit]\r\n");
	return ERR_OK;
}

STATIC HERROR vmx_emm_StartFilter(Vmx_Emm_t *pEmm)
{
	HUINT16 	i;
	HERROR		nErr = ERR_OK;

	HxLOG_Print("[Enter]\r\n");

	if (pEmm == NULL)
	{
		HxLOG_Error("null ptr\r\n");
		return ERR_FAIL;
	}

	if (pEmm->usEmmPid == PID_NULL)
	{
		HxLOG_Error("null pid\r\n");
		return ERR_FAIL;
	}

	for (i = 0; i < pEmm->usEmmAddrCnt; i++)
	{
		if (pEmm->stEmmAddr[i].eState != eVmx_Emm_Running)
		{
			nErr = xsvc_vmx_dmx_RetrieveEmmSection(xsvc_vmx_psi_session_GetStbHandle(pEmm->hSession), pEmm->usEmmPid, pEmm->stEmmAddr[i].aucFilter, pEmm->stEmmAddr[i].aucMask, &pEmm->stEmmAddr[i].ulFilterId);
			if (nErr == ERR_OK)
			{
				pEmm->stEmmAddr[i].eState = eVmx_Emm_Running;
			}
			else
			{
				HxLOG_Error("xsvc_vmx_dmx_RetrieveEmmSection() is error\n");
			}
		}
	}

	HxLOG_Print("[Exit]\r\n");
	return nErr;
}

STATIC HERROR vmx_emm_StopFilter(Vmx_Emm_t *pEmm)
{
	HUINT16 	i;
	HERROR		nErr = ERR_OK;

	HxLOG_Print("[Enter]\r\n");

	if (pEmm == NULL)
	{
		HxLOG_Error("null ptr\r\n");
		return ERR_FAIL;
	}

	if (pEmm->usEmmPid == PID_NULL)
	{
		HxLOG_Error("null pid\r\n");
 		return ERR_FAIL;
	}

	for (i = 0; i < pEmm->usEmmAddrCnt; i++)
	{
		if (pEmm->stEmmAddr[i].eState == eVmx_Emm_Running)
		{
			pEmm->stEmmAddr[i].eState = eVmx_Emm_Stopped;
			nErr = xsvc_vmx_dmx_StopEmmSection(pEmm->stEmmAddr[i].ulFilterId);
		}
	}

	HxLOG_Print("[Exit]\r\n");
	return nErr;
}

STATIC HERROR vmx_emm_Add(Vmx_Emm_t *pEmm)
{
	HERROR			nErr;

	HxLOG_Print("[Enter]\r\n");

	if (pEmm == NULL)
	{
		HxLOG_Error("null ptr\r\n");
		return ERR_FAIL;
	}

	// 이미 있는가 확인하여 있다면 내리고
//	if (pEmm->usEmmAddrCnt)
	{
		vmx_emm_StopFilter(pEmm);
	}

//	pEmm->usEmmPid = usEmmPid;
//	pEmm->usEmmAddrCnt = usAddrCnt;
//	pEmm->pstEmmAddr = pstAddr;

	nErr = vmx_emm_StartFilter(pEmm);

	HxLOG_Print("[Exit]\r\n");
	return nErr;
}

STATIC HERROR vmx_emm_Delete(Vmx_Emm_t *pEmm)
{
	HxLOG_Print("[Enter]\r\n");

	if (pEmm == NULL)
	{
		HxLOG_Error("null ptr\r\n");
		return ERR_FAIL;
	}

	vmx_emm_StopFilter(pEmm);
//	pEmm->usEmmAddrCnt 	= 0;
	pEmm->usEmmPid 		=  PID_NULL;
	pEmm->usUniqueTsId 	= 0;
	pEmm->hSession 		= HANDLE_NULL;
	if (pEmm->pucCatRaw)
	{
		VMX_MEM_Free(pEmm->pucCatRaw);
		pEmm->pucCatRaw = NULL;
	}

	HxLOG_Print("[Exit]\r\n");

	return ERR_OK;
}

STATIC HERROR vmx_emm_RestartFilter(Vmx_Emm_t *pEmm)
{
	HERROR 		nErr;

	HxLOG_Print("[Enter]\r\n");

	VMX_SEM_Get(s_ulVmxEmmSemId);

	vmx_emm_StopFilter(pEmm);
	nErr = vmx_emm_StartFilter(pEmm);

	VMX_SEM_Release(s_ulVmxEmmSemId);

	HxLOG_Print("[Exit]\r\n");
	return nErr;
}

STATIC HERROR vmx_emm_StartFilter_ByFilterIndex(Vmx_Emm_t *pEmm,  HUINT8 ucFilterIndex)
{
	HUINT16 	i;
	HERROR		nErr = ERR_OK;

	HxLOG_Print("[Enter]\r\n");

	if (pEmm == NULL)
	{
		HxLOG_Error("null ptr\r\n");
		return ERR_FAIL;
	}

	if (pEmm->usEmmPid == PID_NULL)
	{
		HxLOG_Error("null pid\r\n");
		return ERR_FAIL;
	}

	i = ucFilterIndex;
	if (pEmm->stEmmAddr[i].eState != eVmx_Emm_Running)
	{
		nErr = xsvc_vmx_dmx_RetrieveEmmSection(xsvc_vmx_psi_session_GetStbHandle(pEmm->hSession), pEmm->usEmmPid, pEmm->stEmmAddr[i].aucFilter, pEmm->stEmmAddr[i].aucMask, &pEmm->stEmmAddr[i].ulFilterId);
		if (nErr == ERR_OK)
		{
			pEmm->stEmmAddr[i].eState = eVmx_Emm_Running;
		}
		else
		{
			HxLOG_Error("xsvc_vmx_dmx_RetrieveEmmSection() is error\n");
		}
	}

	HxLOG_Print("[Exit]\r\n");
	return nErr;
}


STATIC HERROR vmx_emm_StopFilter_ByFilterIndex(Vmx_Emm_t *pEmm,  HUINT8 ucFilterIndex)
{
	HUINT16 	i;
	HERROR		nErr = ERR_OK;

	HxLOG_Print("[Enter]\r\n");

	if (pEmm == NULL)
	{
		HxLOG_Error("null ptr\r\n");
		return ERR_FAIL;
	}

	if (pEmm->usEmmPid == PID_NULL)
	{
		HxLOG_Error("null pid\r\n");
 		return ERR_FAIL;
	}

	i = ucFilterIndex;
	if (pEmm->stEmmAddr[i].eState == eVmx_Emm_Running)
	{
		pEmm->stEmmAddr[i].eState = eVmx_Emm_Stopped;
		nErr = xsvc_vmx_dmx_StopEmmSection(pEmm->stEmmAddr[i].ulFilterId);
	}

	HxLOG_Print("[Exit]\r\n");
	return nErr;
}

STATIC HERROR  vmx_emm_AddByFilterIndex(Vmx_Emm_t *pEmm, HUINT8 ucFilterIndex)
{
	HERROR			nErr;

	HxLOG_Print("[Enter]\r\n");

	if (pEmm == NULL)
	{
		HxLOG_Error("null ptr\r\n");
		return ERR_FAIL;
	}

	vmx_emm_StopFilter_ByFilterIndex(pEmm, ucFilterIndex);
	nErr = vmx_emm_StartFilter_ByFilterIndex(pEmm, ucFilterIndex);

	HxLOG_Print("[Exit]\r\n");

	return nErr;
}

STATIC HERROR vmx_emm_RestartFilterByFilterIndex(Vmx_Emm_t *pEmm, HUINT8 ucFilterIndex)
{
	HERROR 		nErr;

	HxLOG_Print("[Enter]\r\n");

	VMX_SEM_Get(s_ulVmxEmmSemId);

	vmx_emm_StopFilter_ByFilterIndex(pEmm, ucFilterIndex);
	nErr = vmx_emm_StartFilter_ByFilterIndex(pEmm,ucFilterIndex );

	VMX_SEM_Release(s_ulVmxEmmSemId);

	HxLOG_Print("[Exit]\r\n");
	return nErr;
}

STATIC HERROR vmx_emm_DeleteEmmAdd_ByFilterIndex( HUINT8 ucFilterIndex)
{
	Vmx_Emm_t *pEmm = NULL;
	HUINT16 i;

	HxLOG_Print("[Enter]\r\n");

	for (i = 0; i <	VMX_MAX_NUM_OF_EMM; i++)
	{
		// CAT Parse하면서 이미 set 하여서 여기서 set할 필요는 없음.
		if (s_astVmxEmm[i].usEmmPid != PID_NULL)
		{
			pEmm = &s_astVmxEmm[i];
		}
	}

	if(pEmm == NULL)
	{
		HxLOG_Error("null ptr\r\n");
		return ERR_FAIL;
	}

	vmx_emm_StopFilter_ByFilterIndex(pEmm, ucFilterIndex);
	VMX_memset(&pEmm->stEmmAddr[ucFilterIndex], 0, sizeof(Vmx_EmmAddr_t));

	HxLOG_Print("[Exit]\r\n");
	return ERR_OK;
}

STATIC HERROR  vmx_emm_DeleteByTsId(HUINT16 usUniqueTsId)
{
	HUINT16	i;
	HERROR 		nErr = ERR_OK;

	HxLOG_Print("[Enter]\r\n");

	VMX_SEM_Get(s_ulVmxEmmSemId);

	for (i = 0; i <	VMX_MAX_NUM_OF_EMM; i++)
	{
		if (s_astVmxEmm[i].usUniqueTsId == usUniqueTsId)
		{
			nErr = vmx_emm_Delete(&s_astVmxEmm[i]);
		}
	}

	VMX_SEM_Release(s_ulVmxEmmSemId);
	HxLOG_Print("[Exit]\r\n");
	return nErr;
}

STATIC HERROR vmx_emm_OnEmmReceived(HUINT32 ulFilterId, HUINT8 *pucRawData)
{
	Vmx_Emm_t		*pEmm = NULL;
	HUINT16 		i, j;
	HUINT16			usLen;
	HERROR			nErr = ERR_OK;
	HBOOL			bMsg = TRUE;
	HUINT8			*pucRawEmm;

	HxLOG_Print("[Enter]\r\n");

	if (pucRawData == NULL)
	{
		HxLOG_Error("null ptr\r\n");
		return ERR_FAIL;
	}

	VMX_SEM_Get(s_ulVmxEmmSemId);

	for (i = 0; i < VMX_MAX_NUM_OF_EMM; i++)
	{
		for (j = 0; j < s_astVmxEmm[i].usEmmAddrCnt; j++)
		{
			if (s_astVmxEmm[i].stEmmAddr[j].ulFilterId == ulFilterId && s_astVmxEmm[i].stEmmAddr[j].eState == eVmx_Emm_Running)
			{
				pEmm = &s_astVmxEmm[i];
				//pEmmAdd = &pEmm->stEmmAddr[j];
				break;
			}
		}
	}

	if (pEmm == NULL)
	{
		VMX_SEM_Release(s_ulVmxEmmSemId);
		HxLOG_Error("null emm\r\n");
		return ERR_FAIL;
	}

	usLen = get12bit(pucRawData + 1) + 3;

	nErr = ERR_OK;
	bMsg = TRUE;
	pucRawEmm = NULL;

#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
	pucRawEmm = (HUINT8 *)VMX_MEM_Alloc(usLen);
	if (pucRawEmm == NULL)
	{
		VMX_SEM_Release(s_ulVmxEmmSemId);
		HxLOG_Error("alloc fail\r\n");
		return ERR_FAIL;
	}

	VMX_memcpy(pucRawEmm, pucRawData, usLen);

	bMsg = TRUE;
	//## emm queue에다가 넣고 mgr task에 msg 보내준다.
	nErr = vmx_vr_iptvh_emm_OnReceived(pEmm, pucRawEmm, &bMsg);
	if (nErr != ERR_OK)
	{
		HxLOG_Error("vmx_vr_iptvh_emm_OnReceived() failed! \n");
	}
	#if 0
	// emm buffer size 이상 emm event를 발생할 필요 없습니다...(Queue full의 원인...)
	if(nErr == ERR_OK && bMsg == TRUE)
	{
		VMX_VR_IPTVH_MGR_SendMessage((HUINT32)eVmxIptv_Msg_Emm, (HUINT32)HANDLE_NULL, (HUINT32)0, (HUINT32)usLen);
	}
	#endif
#elif defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
	vmx_vr_bc_emm_SaveEmmData(pEmm, pucRawData, usLen);
#endif

	VMX_SEM_Release(s_ulVmxEmmSemId);
	HxLOG_Print("[Exit]\r\n");
	return ERR_OK;
}

STATIC Vmx_Emm_t *vmx_emm_GetEmmInfo(void)
{
	Vmx_Emm_t		*pEmm = NULL;
	HUINT16	i;

	HxLOG_Print("[Enter]\r\n");

	for (i = 0; i <	VMX_MAX_NUM_OF_EMM; i++)
	{
		HxLOG_Print("[%d] EMM Pid(0x%x)\n", i, s_astVmxEmm[i].usEmmPid);

		if(s_astVmxEmm[i].usEmmPid != PID_NULL)
		{
			pEmm = &s_astVmxEmm[i];
		}
	}

	HxLOG_Print("[Exit]\r\n");
	return pEmm;
}

#if 0
HERROR xsvc_vmx_psi_emm_SetEmmFilter(HUINT8 ucFilterIndex, HUINT8 ucAddressLength, HUINT8* pucAddress)
{
	HUINT16	i;
	Vmx_Emm_t		*pEmm = NULL;
	Vmx_EmmAddr_t	*pEmmAdd = NULL;

	for (i = 0; i <	VMX_MAX_NUM_OF_EMM; i++)
	{
		if (s_astVmxEmm[i].usEmmPid != PID_NULL)
		{
			pEmm = &s_astVmxEmm[i];
		}
	}

	if(pEmm == NULL)
	{
		return ERR_FAIL;
	}

	pEmmAdd = &pEmm->stEmmAddr[ucFilterIndex];
	if(pEmmAdd == NULL)
	{
		return ERR_FAIL;
	}

	memset(&pEmmAdd->aucFilter, 0x00, sizeof(pEmmAdd->aucFilter);
	memset(&pEmmAdd->aucMask, 0x00, sizeof(pEmmAdd->aucMask);

	pEmmAdd->aucFilter[0] 	= VMX_EMM_TABLE_ID;
	pEmmAdd->aucMask[0] 	= 0xff;

	pEmmAdd->aucFilter[3] 	= ucFilterIndex;
	pEmmAdd->aucMask[3] 	= 0xff;

	pEmmAdd->aucFilter[5] 	= pucAddress[0];
	pEmmAdd->aucMask[5] 	= 0xff;

	VMX_memcpy(&pEmmAdd->stEmmFilterAdd.address, pucAddress, ucAddressLength);
	pEmmAdd->ucFilterIndex = ucFilterIndex;

	return vmx_emm_Add_ByFilterIndex(pEmm, pEmmAdd->ucFilterIndex);
}
#endif

STATIC HERROR vmx_emm_SetEmmPid(HUINT16 usEmmPid)
{
	HUINT16	i;

	HxLOG_Print("[Enter] - usEmmPid(0x%x)\n", usEmmPid);

//	VMX_SEM_Get(s_ulVmxEmmSemId);
	for (i = 0; i <	VMX_MAX_NUM_OF_EMM; i++)
	{
		// CAT Parse하면서 이미 set 하여서 여기서 set할 필요는 없음.
		if (s_astVmxEmm[i].usEmmPid == PID_NULL)
		{
			// TODO_VMX
			//FS_SetEMM_Pid 가 우선인지 FS_SetEMMFilter가 우선인지 확인후
			// 여기서 바로 filtering을 할지 확인필요.
			s_astVmxEmm[i].usEmmPid = usEmmPid;
			HxLOG_Print("[%d] usEmmPid(0x%x)\r\n", i, usEmmPid);
		}
	}

//	VMX_SEM_Release(s_ulVmxEmmSemId);
	HxLOG_Print("[Exit]\r\n");

	return ERR_OK;
}

STATIC HERROR vmx_emm_DeleteAll(void)
{
	HUINT16	i;
	HERROR 		nErr = ERR_OK;

	HxLOG_Print("[Enter]\r\n");

	//VMX_SEM_Get(s_ulVmxEmmSemId); // hang 문제로 막자.

	for (i = 0; i <	VMX_MAX_NUM_OF_EMM; i++)
	{
		nErr = vmx_emm_Delete(&s_astVmxEmm[i]);
	}

	//VMX_SEM_Release(s_ulVmxEmmSemId);
	HxLOG_Print("[Exit]\r\n");

	return nErr;
}

HERROR xsvc_vmx_psi_emm_UpdateCat(Handle_t hSession, HUINT16 usUniqueTsId, HUINT8 *pucCat)
{
	return vmx_emm_UpdateCa(hSession, usUniqueTsId, pucCat);
}

HERROR xsvc_vmx_psi_emm_RestartFilter(Vmx_Emm_t *pEmm)
{
	return vmx_emm_RestartFilter(pEmm);
}

HERROR  xsvc_vmx_psi_emm_AddByFilterIndex(Vmx_Emm_t *pEmm, HUINT8 ucFilterIndex)
{
	return vmx_emm_AddByFilterIndex(pEmm, ucFilterIndex);
}

HERROR xsvc_vmx_psi_emm_RestartFilterByFilterIndex(Vmx_Emm_t *pEmm, HUINT8 ucFilterIndex)
{
	return vmx_emm_RestartFilterByFilterIndex(pEmm, ucFilterIndex);
}

HERROR xsvc_vmx_psi_emm_DeleteEmmAdd_ByFilterIndex( HUINT8 ucFilterIndex)
{
	return vmx_emm_DeleteEmmAdd_ByFilterIndex(ucFilterIndex);
}

HERROR  xsvc_vmx_psi_emm_DeleteByTsId(HUINT16 usUniqueTsId)
{
	return vmx_emm_DeleteByTsId(usUniqueTsId);
}

HERROR xsvc_vmx_psi_emm_OnEmmReceived(HUINT32 ulFilterId, HUINT8 *pucRawData)
{
	return vmx_emm_OnEmmReceived(ulFilterId, pucRawData);
}

Vmx_Emm_t *xsvc_vmx_psi_emm_GetEmmInfo(void)
{
	return vmx_emm_GetEmmInfo();
}

HERROR xsvc_vmx_psi_emm_SetEmmPid(HUINT16 usEmmPid)
{
	return vmx_emm_SetEmmPid(usEmmPid);
}

HERROR xsvc_vmx_psi_emm_DeleteAll(void)
{
	return vmx_emm_DeleteAll();
}

HERROR xsvc_vmx_psi_emm_InitCore(void)
{
	return vmx_emm_InitCore();
}

