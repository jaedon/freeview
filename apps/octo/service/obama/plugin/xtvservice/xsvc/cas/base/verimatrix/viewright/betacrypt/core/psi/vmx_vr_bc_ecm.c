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
	@file     vmx_vr_bc_ecm.c
	@brief    vmx_vr_bc_ecm.c (Verimatrix BC ECM)

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
#include "_xsvc_cas_vmx_bc_main.h"
#include "_xsvc_cas_vmx_dmx.h"

#if defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
#include "vmx_vr_bc_mgr.h"
#include "vmx_vr_bc_api.h"
#endif

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define MAX_VMX_ECM_FILTER_NUM		(3) // TODO_VMX
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


typedef struct _VmxBcEcmBufNode_t
{
	VmxBc_EcmFilterMode_t eFilterMode;	
	HUINT8 	ucBuf[1024];	/* one section size is 1024 byte */
	HUINT16 usSize;
	struct _VmxBcEcmBufNode_t 	*pNextNode;
} VmxBc_EcmBufNode_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

STATIC VmxBc_EcmBufNode_t		s_stVmxBcEcmBuf[MAX_VMX_ECM_FILTER_NUM];
STATIC HUINT8  TempReadEcmBuf[1024];	/* one section size is 1024 byte */

STATIC HULONG 			s_ulVmxEcmBufSemId;

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
STATIC void bc_ecm_FlushEcmBuf(HUINT8 ucFilterId);
STATIC HERROR bc_ecm_EcmBufInitByServiceIdx(HUINT8 ucServiceIdx);
STATIC HERROR bc_ecm_EcmInitByService(HUINT8 ucServiceIdx);
STATIC VmxBc_EcmBufNode_t * bc_ecm_GetEcmBuf(HUINT8 ucIdx);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define DEF_ECM_FIL______________________________________________________________________________________
#ifdef VMX_SVC_TEST
#else
STATIC HERROR bc_ecm_StartDescrambling(Handle_t hStbHandle, CAS_PmtInfo_t  *pstPmtInfo)
{
	Handle_t hAction;
	Vmx_ActionTable_t *pActionTable;
	HUINT16 i = 0, j = 0;
	HUINT8  usFound = FALSE;
	HUINT16	usEcmPid[8]; // vmx에서는 max 8개
	HUINT16	usStreamPId[8]; // vmx에서는 max 8개

	HxLOG_Print(DBG_PRINT, ("[Enter]\n");

	if(pstPmtInfo == NULL)
	{
		VmxBcEcmError(DBG_ASSERT, ("Error\n"));
		return ERR_FAIL;
	}
	
	#if 1 // TODO_VMX : 아래 둘중에서 하나를 선택해서 사용하자. // 두가지 테스트해서 모두 같게 나와야한다.
	// TODO_VMX : VMX 문의 : 중간에 ECMPID가 변경되는경우. or PID가 변경되는 경우 BC_StopDescrambling을 다시 호홀해주어야하는지?
	hAction = xsvc_vmx_main_GetActionHandleFromStbHdl(hStbHandle);
	pActionTable = xsvc_vmx_main_GetActionByActionHandle(hAction);

	VMX_memset(&usEcmPid[0], 0, sizeof(usEcmPid));	
	VMX_memset(&usStreamPId[0], 0, sizeof(usStreamPId));

	HxLOG_Print(DBG_PRINT, ("ulNumOfElement(%d)\n", pstPmtInfo->ulNumOfElement);

	for (i = 0; i < pstPmtInfo->ulNumOfElement; i++)
	{
		HxLOG_Print(DBG_PRINT, ("[%d] pid(0x%x)\n", i, pstPmtInfo->stElementInfo[i].usPid);
		if (pstPmtInfo->stElementInfo[i].usPid != PID_NULL && pstPmtInfo->stElementInfo[i].usPid != 0)
		{
			usFound = TRUE;
			usStreamPId[j] = pstPmtInfo->stElementInfo[i].usPid;
			usEcmPid[j++] = xsvc_vmx_psi_stream_GetEcmPidFromPmt(pstPmtInfo, &pstPmtInfo->stElementInfo[i]);
		}
	}

	HxLOG_Print(DBG_PRINT, ("usFound(%d)\n",usFound);

	if(usFound == TRUE)
	{
		vmx_vr_bc_api_StartDescrambling(pstPmtInfo->usServiceId, pstPmtInfo->ulNumOfElement, usEcmPid,  usStreamPId, pActionTable->ucVmxServiceIdx);
	}
	HxLOG_Print(DBG_PRINT, ("[Exit]\n");
	return ERR_OK;
	#else
	Vmx_EsPidInfo_t *pEsPidInfo;
	HUINT8    ucCnt;
	
	if(hSession == HANDLE_NULL || pstPmtInfo == NULL)
	{
		return ERR_FAIL;
	}

	// TODO_VMX : VMX 문의 : 중간에 ECMPID가 변경되는경우. or PID가 변경되는 경우 BC_StopDescrambling을 다시 호홀해주어야하는지?
	hAction = xsvc_vmx_main_GetActionHandleFromStbHdl(hStbHandle);
	pActionTable = xsvc_vmx_main_GetActionByActionHandle(hAction);
	
	pEsPidInfo = xsvc_vmx_psi_session_GetSessonInfoByStbHandle(hStbHandle);
	if(pEsPidInfo == NULL)
	{
		return ERR_FAIL;
	}

	VMX_memset(&pEsPidInfo->usPid[0], 0, sizeof(pEsPidInfo->usPid));
	vmx_stream_GetEcmListOfService(hSession, &pEsPidInfo->usEcmPid[0], &ucCnt);

	// PID 갯수만틈 ecm pid로 등록이 되어야한다.
	if(ucCnt == 1)
	{
		VmxCorePrint(DBG_PRINT, ("A EcmPid is exist in a Service\n");
		for(i=1;i<pEsPidInfo->ulPidNum;i++)
			pEsPidInfo->usEcmPid[i] = pEsPidInfo->usEcmPid[0];
	}
	else if(ucCnt == 0) // ecm pid가 존재하지 않는 경우
	{
		VmxCorePrint(DBG_PRINT, ("EcmPid is not exist in a Service\n");		
	}
	else
	{
		// 각각의 pid에 서로다른 ecmpid가 존재하는경우 : 
		VmxCorePrint(DBG_PRINT, ("Some of EcmPids are exist in a Service\n");		
	}

	vmx_vr_bc_api_StartDescrambling(pstPmtInfo->usServiceId, pEsPidInfo->ulPidNum, &pEsPidInfo->usEcmPid[0],  &pEsPidInfo->usPid[0], pActionTable->ucVmxServiceIdx);	
	#endif
}
#endif

STATIC HERROR  bc_ecm_StopDescrambling(Handle_t hStbHandle, CAS_PmtInfo_t *pstPmtInfo)
{	
	Handle_t hAction;
	Vmx_ActionTable_t *pActionTable = NULL;

	HxLOG_Print(DBG_PRINT, ("[Enter]\n");
	hAction = xsvc_vmx_main_GetActionHandleFromStbHdl(hStbHandle);
	pActionTable = xsvc_vmx_main_GetActionByActionHandle(hAction);
	//pstPmtInfo = s_astVmxSession[nIdx].pstPmtInfo;
	//nIdx = vmx_session_FindIndexByStbHandle(hStbHandle);

	if(pActionTable == NULL || pstPmtInfo == NULL)
	{
		return ERR_FAIL;
	}

	if(vmx_vr_bc_api_StopDescrambling(pstPmtInfo->usServiceId, pActionTable->ucVmxServiceIdx) != ERR_OK)
	{
		HxLOG_Print(DBG_PRINT, ("ERROR\r\n");
	}

	HxLOG_Print(DBG_PRINT, ("[Exit]\n");
	return bc_ecm_EcmBufInitByServiceIdx(pActionTable->ucVmxServiceIdx);
}

STATIC HINT16 bc_ecm_SetEcmFilter(HUINT8 ucFilterId, VmxBc_EcmFilterMode_t eFilterMode, HUINT16 usEcmPid, HUINT8 ucTableId, HUINT8 ucVersion, HUINT8 ucPage)
{
	Handle_t hStbHandle;
	Handle_t hSession;
	Handle_t hEcm;
	VmxBc_EcmBufNode_t *pEcmBuf = NULL;
	Vmx_EcmFilter_t *pEcmFilter = NULL;

	HxLOG_Print(DBG_PRINT, ("[Enter] bFilterId(%d), eFilterMode(%d), wEcmPid(0x%x), bTableId(0x%x), bVersion(%d), bPage(%d)\n", ucFilterId, eFilterMode, usEcmPid, ucTableId, ucVersion, ucPage);

	pEcmBuf = bc_ecm_GetEcmBuf(ucFilterId);
	if(pEcmBuf == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("Error\n");
		return ERR_FAIL;
	}

	pEcmBuf->eFilterMode = eFilterMode;

	//ucFilterId is the same as serviceIdx.
	switch(eFilterMode)
	{
		/* No futher ECMs are delivered to the CA system and ecm buffer should be flushed */
		case eVmxBc_EcmFilterModeDisableFilter:
			hEcm = xsvc_vmx_psi_ecm_GetEcmHandleFromEcmPid(usEcmPid);
			pEcmFilter = xsvc_vmx_psi_ecm_GetEcmFilterInfoFromEcmPid(usEcmPid);
			if(pEcmFilter == NULL || hEcm == HANDLE_NULL)
			{
				HxLOG_Error(DBG_ASSERT, ("Error\n");
				goto err_fail;
			}
			// Filter Stop
			hStbHandle = xsvc_vmx_main_GetStbHandleFormServiceIndex(ucFilterId);
			hSession = xsvc_vmx_psi_session_GetHandleByStbHandle(hStbHandle);
			xsvc_vmx_psi_stream_StopAllEcmFilter(hSession);

			//bc_ecm_EcmBufInitByServiceIdx(ucFilterId);
			bc_ecm_EcmInitByService(ucFilterId);
			break;
		/* This is used during descrambling a service */
		case eVmxBc_EcmFilterModePageLocked:	
			// BUF의 와 Filter 정보를 가지고 온다.
			hEcm = xsvc_vmx_psi_ecm_GetEcmHandleFromEcmPid(usEcmPid);
			pEcmFilter = xsvc_vmx_psi_ecm_GetEcmFilterInfoFromEcmPid(usEcmPid);
			if(pEcmFilter == NULL || hEcm == HANDLE_NULL)
			{
				HxLOG_Error(DBG_ASSERT, ("Error\n");
				goto err_fail;
			}
			// BUF 초기화
			bc_ecm_EcmBufInitByServiceIdx(ucFilterId);
			/* tableId, Ver, Page가 모두 같은경우와 Ver이 변경된 경우만 필요하다. 
			    우선 여기서 2개의 filter를 따로 걸지말로 ecm을 받으면 그때 처리하도록 하자 */
			if(xsvc_vmx_psi_ecm_StartFilter(hEcm, eVmx_Ecm_Both, NULL) == ERR_FAIL)
			{
				HxLOG_Error(DBG_ASSERT, ("Error\n");
				goto err_fail;
			}
			// Ecm Info : setup
			pEcmFilter->ucVmxFilterId = ucFilterId;
			pEcmFilter->eFilterMode = eVmxBc_EcmFilterModePageLocked;
			pEcmFilter->ePageSearchMode = eVmx_EcmPageSearch_None;
			pEcmFilter->ucTableId = ucTableId;
			pEcmFilter->ucVersion = ucVersion;
			pEcmFilter->ucPage = ucPage;
			break;
		/* This is used e.g. after a channel change */
		case eVmxBc_EcmFilterMode_PageSearch:
			// ECM 정보를 가지고 온다.
			hEcm = xsvc_vmx_psi_ecm_GetEcmHandleFromEcmPid(usEcmPid);
			pEcmFilter = xsvc_vmx_psi_ecm_GetEcmFilterInfoFromEcmPid(usEcmPid);			
			if(pEcmFilter == NULL || hEcm== HANDLE_NULL)
			{
				HxLOG_Error(DBG_ASSERT, ("Error\n");
				goto err_fail;
			}

			HxLOG_Print(DBG_PRINT, ("OLD_FilterMode(%d), NEW_FilterMode(%d), ePageSearchMode(%d)\n", pEcmFilter->eFilterMode, eFilterMode, pEcmFilter->ePageSearchMode);
			// Filter Start
			if(  (pEcmFilter->eFilterMode != eFilterMode) || /* 모드가 변경된 겨우 */
			     (pEcmFilter->eFilterMode == eFilterMode && pEcmFilter->ePageSearchMode == eVmx_EcmPageSearch_Complete) ) /* 같은 모드에서 complete되지 않은경우에 call 된경우 무시한다 */
			{
				HxLOG_Print(DBG_PRINT, ("Start PageSearch...\n");
				if(xsvc_vmx_psi_ecm_StartFilter(hEcm, eVmx_Ecm_Both, NULL) == ERR_FAIL)
				{
				HxLOG_Error(DBG_ASSERT, ("Error\n");
					goto err_fail;
				}
				// Ecm Info : setup
				pEcmFilter->eFilterMode = eVmxBc_EcmFilterMode_PageSearch;
				pEcmFilter->ucVmxFilterId = ucFilterId;
				pEcmFilter->ePageSearchMode = eVmx_EcmPageSearch_Ready;	
			}			
			break;
		default:
			pEcmBuf->eFilterMode = eVmxBc_EcmFilterMode_None;
			HxLOG_Error(DBG_ASSERT, ("Error\n");
			goto err_fail;
			break;
	}
	HxLOG_Print(DBG_PRINT, ("[Exit]\n");
	return ERR_OK;

err_fail:
	HxLOG_Error(DBG_ASSERT, ("[Exit]\n");
	return ERR_FAIL;
}

STATIC HERROR bc_ecm_OnEcmReceived(HUINT32 ulFilterId, HUINT8 *pucRawData, HUINT16 usLen, HUINT32 hAction)
{
	Vmx_EcmFilter_t *pEcmFilter = NULL;
	HBOOL	bFirstPage = FALSE;
	HUINT8 ucTableId = 0;
	HUINT8 ucVersion = 0;
	HUINT8 ucPage = 0;
	HUINT8 ucLastPage = 0;

	HxLOG_Print(DBG_PRINT, ("[Enter]\n");

	if(pucRawData == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("pucRawData is NULL\n");
		return ERR_FAIL;
	}

	ucTableId = pucRawData[0];
	ucVersion = VMXGETBIT8(&pucRawData[3], 1, 5);
	ucPage = VMXGETBIT8(&pucRawData[4], 0, 8);
	ucLastPage = VMXGETBIT8(&pucRawData[5], 0, 8);

	if(ucTableId != VMX_ECM_TABLE_ID_80 && ucTableId != VMX_ECM_TABLE_ID_81)
	{
		goto err_fail;
	}

	pEcmFilter = xsvc_vmx_psi_ecm_GetEcmFilterInfoFromDescFilterId(ulFilterId);
	if(pEcmFilter == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("pEcmFilter is NULL\n");
		goto err_fail;
	}

	xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_PSI_ECM, "OnEcmReceived", pucRawData, usLen);
	HxLOG_Print(DBG_PRINT, ("NEW : ucTableId(0x%x), ucVersion(0x%x), ucPage(%d), ucLastPage(%d)\n",
						ucTableId, ucVersion, ucPage, ucLastPage);
	HxLOG_Print(DBG_PRINT, ("OLD : ucTableId(0x%x), ucVersion(0x%x), ucPage(%d), eFilterMode(%d), ucVmxFilterId(%d), ePageSearchMode(%d)\n", 
						pEcmFilter->ucTableId, pEcmFilter->ucVersion, pEcmFilter->ucPage, pEcmFilter->eFilterMode, pEcmFilter->ucVmxFilterId, pEcmFilter->ePageSearchMode);

	if(pEcmFilter->eFilterMode  == eVmxBc_EcmFilterMode_PageSearch)
	{
		HxLOG_Print(DBG_PRINT, ("PS : PageSearch\n");
		/* ECMs은 모으는중 table_id or version number가 변경되면 */
		if(ucTableId != pEcmFilter->ucTableId || ucVersion != pEcmFilter->ucVersion)
		{
			/* buffer clear하고 다시모아야함 */
			bc_ecm_EcmBufInitByServiceIdx(pEcmFilter->ucVmxFilterId);
			/* 항상 page 0부터 시작해야한다 */
			if(ucPage != 0)
			{
				HxLOG_Print(DBG_PRINT, ("PS Skip -Wait Page 0\n");
				goto err_fail;
			}

			HxLOG_Print(DBG_PRINT, ("PS Ready - FirstPage...\n");
			/* page가 0인 가장 먼저 들어온 tableId, version로 찾음 */
			pEcmFilter->ePageSearchMode = eVmx_EcmPageSearch_Ready;
			bFirstPage = TRUE;			
		}

		if(pEcmFilter->ePageSearchMode == eVmx_EcmPageSearch_Complete)
		{
			HxLOG_Print(DBG_PRINT, ("PS Skip - PageSearch completed\n");
			goto err_fail;
		}

		/* table or version이 변경된후 바로 or first filter */
		if(bFirstPage == TRUE || pEcmFilter->ucPage == VMX_INVALID_FILTER_VALUE)
		{
			/* page가 0인경우부터 등록 한다 */
			if(ucPage == 0)
			{
				pEcmFilter->ucTableId  = ucTableId;
				pEcmFilter->ucVersion = ucVersion;
				pEcmFilter->ucPage = ucPage;
				HxLOG_Print(DBG_PRINT, ("PS  Go - Start FirstPage : TableId(0x%x), Version(0x%x), Page(%d)\n", ucTableId, ucVersion, ucPage);
			}
			/* pEcmFilter->ucPage == VMX_INVALID_FILTER_VALUE 인데 page가 0이아니경우가 들어오는 경우 */
			else
			{
				HxLOG_Print(DBG_PRINT, ("PS Skip - Error\n");
				goto err_fail;
			}
		}
		/* last page까지 계속해서 등록한다 */
		else
		{
			/* tableId, version은 같은데 1증가된 page를 전달받아야하는데 다른 page가 전달되는경우 skip한다. */
			if(ucPage != (pEcmFilter->ucPage+1))
			{
				HxLOG_Print(DBG_PRINT, ("PS Skip : Old Page(%d), New Page(%d)\n", pEcmFilter->ucPage, ucPage);
				goto err_fail;
			}
			pEcmFilter->ucPage = ucPage;
			HxLOG_Print(DBG_PRINT, ("PS Go -  Page is %d of %d\n", pEcmFilter->ucPage, ucLastPage);
			// page search를 위한 모든 ecm page을 전달 받음.
			if(ucLastPage == pEcmFilter->ucPage)
			{
				pEcmFilter->ePageSearchMode = eVmx_EcmPageSearch_Complete;
				pEcmFilter->ucPage = VMX_INVALID_FILTER_VALUE;
				HxLOG_Print(DBG_PRINT, ("PS Now Complete\n");
			}
		}
	}
	else if(pEcmFilter->eFilterMode == eVmxBc_EcmFilterModePageLocked)
	{
		HxLOG_Print(DBG_PRINT, ("PL - PageLock\n");
		if(ucVersion == pEcmFilter->ucVersion)
		{
			if(ucTableId == pEcmFilter->ucTableId && ucPage == pEcmFilter->ucPage)
			{
				/* tableId, version, page가 모두 같은경우 buf로 전달 */
				HxLOG_Print(DBG_PRINT, ("PL Go -  Oll of them are Same\n");
			}
			else
			{
				HxLOG_Print(DBG_PRINT, ("PL Skip - Veraion is same but tableId(0x%x)(0x%x) or page(0x%x)(0x%x) are different\n", pEcmFilter->ucTableId, ucTableId, pEcmFilter->ucPage, ucPage);
				goto err_fail;
			}
		}
		else
		{
			HxLOG_Print(DBG_PRINT, ("PL Go -  Version changed - Old Ver(0x%x), New Ver(0x%x)\n", pEcmFilter->ucVersion, ucVersion);
			pEcmFilter->ucVersion = ucVersion;
			/* version이 다르면 무조건 buf로 전달 */
		}
	}
	else /*if(pEcmFilter->eFilterMode == eVmxBc_EcmFilterModeDisableFilter || pEcmFilter->eFilterMode == eVmxBc_EcmFilterMode_None) */
	{
		HxLOG_Print(DBG_PRINT, ("This case is not happened - Error\n");
		// 걸리 filter가 없기 때문에 이경우는 들어오지 말아야한다.
		goto err_fail;
	}

	if(bc_ecm_WriteEcmBuf(pEcmFilter->ucVmxFilterId, pucRawData, usLen) != ERR_OK)
	{
		HxLOG_Error(DBG_ASSERT, ("It is fail to write to buf\n");
	}

	HxLOG_Print(DBG_PRINT, ("[Exit]\n");
	return ERR_OK;

err_fail:
	HxLOG_Error(DBG_ASSERT, ("[Exit]\n");
	return ERR_FAIL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define DEF_ECM_BUF______________________________________________________________________________________

STATIC VmxBc_EcmBufNode_t * bc_ecm_GetEcmBuf(HUINT8 ucIdx)
{
	return &s_stVmxBcEcmBuf[ucIdx];
}

STATIC HERROR bc_ecm_EcmBufInitByServiceIdx(HUINT8 ucServiceIdx)
{
	HxLOG_Print(DBG_PRINT, ("[Enter]\n");

	bc_ecm_FlushEcmBuf(ucServiceIdx);
	s_stVmxBcEcmBuf[ucServiceIdx].pNextNode	= NULL;

	HxLOG_Print(DBG_PRINT, ("[Exit]\n");
	return ERR_OK;
}

STATIC HERROR bc_ecm_EcmInitByService(HUINT8 ucServiceIdx)
{
	bc_ecm_EcmBufInitByServiceIdx(ucServiceIdx);
	s_stVmxBcEcmBuf[ucServiceIdx].eFilterMode = eVmxBc_EcmFilterMode_None;
	return ERR_OK;
}

STATIC HERROR bc_ecm_EcmInit(void)
{
	int i;
	HINT32 	nResult = 0;

	HxLOG_Print(DBG_PRINT, ("[Enter]\n");

	nResult = VMX_SEM_Create("VmxEcmBufSem", &s_ulVmxEcmBufSemId);	
	if(nResult != ERR_OK)
	{
		HxLOG_Error(DBG_ASSERT, ("Failure in Semaphore Creation \r\n");
		return ERR_FAIL;
	}	

	for(i=0;i<MAX_VMX_ECM_FILTER_NUM;i++)
	{
		bc_ecm_EcmInitByService(i);
	}

	VMX_memset(TempReadEcmBuf, 0, sizeof(TempReadEcmBuf));

	HxLOG_Print(DBG_PRINT, ("[Exit]\n");

	return ERR_OK;
}

STATIC HERROR bc_ecm_ReadEcmBuf(HUINT8 ucFilterId, HUINT8* pabBuffer, HUINT16* pwLen)
{
	VmxBc_EcmBufNode_t *pFirstNode, *pNode;
	VmxBc_EcmBufNode_t *pEcm = bc_ecm_GetEcmBuf(ucFilterId);

	HxLOG_Print(DBG_PRINT, ("[Enter ]bFilterId(%d)\n", ucFilterId);

	if(pEcm == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("Ecm is null\n");
		return ERR_FAIL;
	}

	if(pEcm->eFilterMode == eVmxBc_EcmFilterMode_None || pEcm->eFilterMode == eVmxBc_EcmFilterModeDisableFilter)
	{
		HxLOG_Error(DBG_ASSERT, ("FilterMode[%d] is error\n", pEcm->eFilterMode);
		return ERR_FAIL;
	}

	 //*pwLen = 0;

	VMX_SEM_Get(s_ulVmxEcmBufSemId);

	pNode =  bc_ecm_GetEcmBuf(ucFilterId);
	if(pNode == NULL)
	{
		VMX_SEM_Release(s_ulVmxEcmBufSemId);
		HxLOG_Print(DBG_PRINT, ("pNode is null\r\n");
		 *pwLen = 0;
		return ERR_OK;
	}

	pFirstNode = pNode->pNextNode;
	if (pFirstNode == NULL)
	{
		// 남아 있는 node가 없음
		VMX_SEM_Release(s_ulVmxEcmBufSemId);
		HxLOG_Print(DBG_PRINT, ("ECM QUEUE EMPTY\r\n");
		 *pwLen = 0;
		return ERR_OK;
	}

	if(* pwLen < pFirstNode->usSize)
	{
		VMX_SEM_Release(s_ulVmxEcmBufSemId);
		HxLOG_Print(DBG_PRINT, ("ECM QUEUE EMPTY\r\n");
		 *pwLen = 0;
		return ERR_FAIL;
	}

	// 첫번째 node의 값을 pNode에 복사해준다
	VMX_memset(&TempReadEcmBuf, 0, sizeof(TempReadEcmBuf));
	VMX_memcpy(&TempReadEcmBuf, pFirstNode->ucBuf, pFirstNode->usSize);
	VMX_memcpy(pabBuffer, &TempReadEcmBuf, pFirstNode->usSize);
	 *pwLen = pFirstNode->usSize;

	pNode->pNextNode = pFirstNode->pNextNode;

	VMX_MEM_Free(pFirstNode);

	VMX_SEM_Release(s_ulVmxEcmBufSemId);

	HxLOG_Print(DBG_PRINT, ("[Exit]\r\n");
	return ERR_OK;;
}

STATIC HERROR bc_ecm_WriteEcmBuf(HUINT8 ucFilterId, HUINT8 * pucEcmBuf, HUINT16 usEcmSize)
{
	VmxBc_EcmBufNode_t *pNode, *pNewNode;

	HxLOG_Print(DBG_PRINT, ("[Enter]\n");

	VMX_SEM_Get(s_ulVmxEcmBufSemId);

	pNewNode = VMX_MEM_Alloc(sizeof(VmxBc_EcmBufNode_t));
	if (pNewNode == NULL)
	{
		VMX_SEM_Release(s_ulVmxEcmBufSemId);
		HxLOG_Print(DBG_ASSERT, ("alloc fail\r\n");
		return ERR_FAIL;
	}

	VMX_memset(&pNewNode->ucBuf, 0, 1024);
	VMX_memcpy(&pNewNode->ucBuf, pucEcmBuf, usEcmSize);
	pNewNode->usSize 	= usEcmSize;
	pNewNode->pNextNode 	= NULL;

	//xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_PSI_ECM, "WriteECM", pNewNode->ucBuf, usEcmSize);

	// 가장 마지막 node를 찾는다
	pNode = bc_ecm_GetEcmBuf(ucFilterId);

	while (pNode->pNextNode != NULL)
	{
		pNode = pNode->pNextNode;
	}

	pNode->pNextNode = pNewNode;

	VMX_SEM_Release(s_ulVmxEcmBufSemId);
	HxLOG_Print(DBG_PRINT, ("[Exit]\n");
	return ERR_OK;
}

STATIC HERROR bc_ecm_FreeEcmBuf(HUINT8 ucFilterId)
{
	VmxBc_EcmBufNode_t *pFirstNode, *pNode;
	VmxBc_EcmBufNode_t *pEcm = bc_ecm_GetEcmBuf(ucFilterId);

	HxLOG_Print(DBG_PRINT, ("[Enter ]bFilterId(%d)\n", ucFilterId);

	if(pEcm == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("Ecm is null\n");
		return ERR_FAIL;
	}

	VMX_SEM_Get(s_ulVmxEcmBufSemId);

	pNode =  bc_ecm_GetEcmBuf(ucFilterId);
	if(pNode == NULL)
	{
		VMX_SEM_Release(s_ulVmxEcmBufSemId);
		HxLOG_Print(DBG_PRINT, ("pNode is null\r\n");
		return ERR_FAIL;
	}

	pFirstNode = pNode->pNextNode;
	if (pFirstNode == NULL)
	{
		// 남아 있는 node가 없음
		VMX_SEM_Release(s_ulVmxEcmBufSemId);
		HxLOG_Print(DBG_PRINT, ("ECM QUEUE EMPTY\r\n");
		return ERR_FAIL;
	}

	pNode->pNextNode = pFirstNode->pNextNode;

	VMX_MEM_Free(pFirstNode);

	VMX_SEM_Release(s_ulVmxEcmBufSemId);

	HxLOG_Print(DBG_PRINT, ("[Exit]\r\n");
	return ERR_OK;;
}

STATIC void bc_ecm_FlushEcmBuf(HUINT8 ucFilterId)
{
	while(1)
	{
		if(bc_ecm_FreeEcmBuf(ucFilterId) != ERR_OK)
			break;
	}
}

#ifdef VMX_SVC_TEST
#else
HERROR vmx_vr_bc_StartDescrambling(Handle_t hStbHandle, CAS_PmtInfo_t *pstPmtInfo)
{
	return bc_StartDescrambling(hStbHandle, pstPmtInfo);
}
#endif

HERROR  vmx_vr_bc_StopDescrambling( Handle_t hStbHandle, CAS_PmtInfo_t *pstPmtInfo)
{
	return bc_ecm_StopDescrambling(hStbHandle, pstPmtInfo);
}

HINT16 vmx_vr_bc_ecm_SetEcmFilter(HUINT8 ucFilterId, VmxBc_EcmFilterMode_t eFilterMode, HUINT16 usEcmPid, HUINT8 ucTableId, HUINT8 ucVersion, HUINT8 ucPage)
{
	return bc_ecm_SetEcmFilter(ucFilterId, eFilterMode, usEcmPid, ucTableId, ucVersion, ucPage);
}

HERROR vmx_vr_bc_ecm_OnEcmReceived(HUINT32 ulFilterId, HUINT8 *pucRawData, HUINT16  usLen, HUINT32 hAction)
{
	return bc_ecm_OnEcmReceived(ulFilterId, pucRawData,usLen,  hAction);
}

HERROR vmx_vr_ecm_GetCaSystemId(HUINT16* pusCaSystemId)
{
	return vmx_vr_bc_mgr_GetCaSystemId(pusCaSystemId);
}

HERROR vmx_vr_bc_ecm_ReadEcmBuf(HUINT8 bFilterId, HUINT8* pabBuffer, HUINT16* pwLen)
{
	return bc_ecm_ReadEcmBuf(bFilterId, pabBuffer, pwLen);
}

HERROR vmx_vr_bc_ecm_WriteEcmBuf(HUINT8 ucFilterId, HUINT8 * pucEcmBuf, HUINT16 usEcmSize)
{
	return bc_ecm_WriteEcmBuf(ucFilterId, pucEcmBuf, usEcmSize);
}

void vmx_vr_bc_ecm_FlushEcmBuf(HUINT8 ucFilterId)
{
	return bc_ecm_FlushEcmBuf(ucFilterId);
}

HERROR vmx_vr_bc_ecm_Init(void)
{
	return bc_ecm_EcmInit();
}
