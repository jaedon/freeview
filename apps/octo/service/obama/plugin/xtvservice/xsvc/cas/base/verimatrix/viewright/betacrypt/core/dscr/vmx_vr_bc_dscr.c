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
	@file     vmx_vr_bc_dscr.c
	@brief    vmx_vr_bc_sc.c (Verimatrix BC SC)

	Description:   \n
	Module: MW/CAS/ VERIMATRIX \n
	Remarks :\n

	Copyright (c) 2015 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "_xsvc_cas_vmx_bc_main.h"
#include "_xsvc_cas_vmx_os.h"
#include "vmx_vr_bc_api.h"
/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#ifdef CONFIG_DEBUG
HUINT32 g_BcDscr_Level = (VMX_DBG_VR_BC_DSCR == 1 ? 0xff : 0);

#define BcDscrPrint(level, message)		(((level) & g_BcDscr_Level) ? ((VK_Print("\tVMX_BCDSCR [%d][%s], ", __LINE__, __FUNCTION__)), (VK_Print message)) : 0)
#define BcDscrError(level, message)		(((level) & g_BcDscr_Level) ? ((VK_Print("\tVMX_ERR_BCDSCR [%d][%s] ERROR, ", __LINE__, __FUNCTION__)), (VK_Print message)) : 0)
#define BcDscrAssert(c) 					((c) ? 0 : (VK_Print("\n SC assertion on %s: line%d\n\n", __FILE__, __LINE__)))
#define BcDscrData(level, message)		(((level) & g_BcDscr_Level) ? ( (VK_Print message)) : 0)
#else
#define BcDscrPrint(level, message)
#define BcDscrError(level, message)
#define BcDscrAssert(c)
#define BcDscrData(level, message)
#endif


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

HERROR vmx_vr_dscr_StartDescrambling(Handle_t hStbHandle, CAS_PmtInfo_t *pstPmtInfo)
{
	Handle_t hAction;
	Vmx_ActionTable_t *pActionTable;
	Handle_t hSession;
	HUINT16 i = 0, j = 0;
	HUINT8  usFound = FALSE;
	HUINT16	usEcmPid[8]; // vmx에서는 max 8개
	HUINT16	usStreamPId[8]; // vmx에서는 max 8개

	BcDscrPrint(DBG_PRINT, ("[Enter]\n"));

	if(pstPmtInfo == NULL)
	{
		BcDscrError(DBG_ASSERT, ("Error\n"));
		return ERR_FAIL;
	}
	
	#if 1 // TODO_VMX : 아래 둘중에서 하나를 선택해서 사용하자. // 두가지 테스트해서 모두 같게 나와야한다.
	// TODO_VMX : VMX 문의 : 중간에 ECMPID가 변경되는경우. or PID가 변경되는 경우 BC_StopDescrambling을 다시 호홀해주어야하는지?
	hAction = xsvc_vmx_main_GetActionHandleFromStbHdl(hStbHandle);
	pActionTable = xsvc_vmx_main_GetActionByActionHandle(hAction);
#if 0//def VMX_SVC_TEST
	hSession = xsvc_vmx_psi_session_GetHandleByStbHandle(hStbHandle);
#endif
	VMX_memset(&usEcmPid[0], 0, sizeof(usEcmPid));	
	VMX_memset(&usStreamPId[0], 0, sizeof(usStreamPId));

	BcDscrPrint(DBG_PRINT, ("ulNumOfElement(%d)\n", pstPmtInfo->ulNumOfElement));

	for (i = 0; i < pstPmtInfo->ulNumOfElement; i++)
	{
		BcDscrPrint(DBG_PRINT, ("[%d] pid(0x%x)\n", i, pstPmtInfo->stElementInfo[i].usPid));
		if (pstPmtInfo->stElementInfo[i].usPid != PID_NULL && pstPmtInfo->stElementInfo[i].usPid != 0)
		{
#ifdef VMX_SVC_TEST
			// 현재 stream안에 pmt안에 포함되어 있는 pid가 존재하는 pid만 setting한다.
			//if(hSession != HANDLE_NULL && xsvc_vmx_psi_stream_CompareValidPidFromSession(hSession, pstPmtInfo->stElementInfo[i].usPid) == ERR_OK)
			// DSCR OPEN되어 있는 PID만 setting한다. 그렇지 않으면 BRCM_GetStreamHandle()에서 null로 return되어 kill되게 된다.
			if(xsvc_vmx_dscr_VerifyValidDscrPid(pstPmtInfo->stElementInfo[i].usPid) == ERR_OK)
#endif				
			{
				usFound = TRUE;
				usStreamPId[j] = pstPmtInfo->stElementInfo[i].usPid;
				usEcmPid[j++] = xsvc_vmx_psi_stream_GetEcmPidFromPmt(pstPmtInfo, &pstPmtInfo->stElementInfo[i]);
			}
		}
	}

	BcDscrPrint(DBG_PRINT, ("usFound(%d)\n",usFound));

	if(usFound == TRUE)
	{
		vmx_vr_bc_api_StartDescrambling(pstPmtInfo->usServiceId, pstPmtInfo->ulNumOfElement, usEcmPid,  usStreamPId, pActionTable->ucVmxServiceIdx);
	}
	BcDscrPrint(DBG_PRINT, ("[Exit]\n"));
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
		VmxCorePrint(DBG_PRINT, ("A EcmPid is exist in a Service\n"));
		for(i=1;i<pEsPidInfo->ulPidNum;i++)
			pEsPidInfo->usEcmPid[i] = pEsPidInfo->usEcmPid[0];
	}
	else if(ucCnt == 0) // ecm pid가 존재하지 않는 경우
	{
		VmxCorePrint(DBG_PRINT, ("EcmPid is not exist in a Service\n"));		
	}
	else
	{
		// 각각의 pid에 서로다른 ecmpid가 존재하는경우 : 
		VmxCorePrint(DBG_PRINT, ("Some of EcmPids are exist in a Service\n"));		
	}

	vmx_vr_bc_api_StartDescrambling(pstPmtInfo->usServiceId, pEsPidInfo->ulPidNum, &pEsPidInfo->usEcmPid[0],  &pEsPidInfo->usPid[0], pActionTable->ucVmxServiceIdx);	
	#endif
}

HERROR vmx_vr_dscr_StopDescrambling(Handle_t hStbHandle, CAS_PmtInfo_t *pstPmtInfo)
{	
	Handle_t hAction;
	Vmx_ActionTable_t *pActionTable = NULL;

	BcDscrPrint(DBG_PRINT, ("[Enter]\n"));

	hAction = xsvc_vmx_main_GetActionHandleFromStbHdl(hStbHandle);
	pActionTable = xsvc_vmx_main_GetActionByActionHandle(hAction);

	if(pActionTable == NULL || pstPmtInfo == NULL)
	{
		BcDscrError(DBG_ASSERT, ("Error\n"));
		return ERR_FAIL;
	}

	if(vmx_vr_bc_api_StopDescrambling(pstPmtInfo->usServiceId, pActionTable->ucVmxServiceIdx) != ERR_OK)
	{
		BcDscrError(DBG_PRINT, ("ERROR\r\n"));
	}

	BcDscrPrint(DBG_PRINT, ("[Exit]\n"));

	return ERR_OK;
}