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
	@file     vmx_vr_bc_psi_mgr.c
	@brief    vmx_vr_bc_psi_mgr.c (Verimatrix send Data to vmx)

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
#include "_xsvc_cas_vmx_bc_main.h"
#include "vmx_vr_bc_mgr.h"
#include "vmx_vr_bc_api.h"
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
STATIC HULONG sVmxPsiQid;
STATIC HULONG sVmxPsiTid;
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

#if defined(CONFIG_MW_CAS_VERIMATRIX_DVB) && defined(VMX_SVC_TEST)
STATIC void vr_bc_psi_Task ( void * args )
{
	Vmx_Message_t 	 stMsg;
	Vmx_Instance_t	*pstVmxInst = NULL;
	HUINT32		  	 i;

	while (1)
	{
		if (VMX_MSG_Receive(sVmxPsiQid, (void*)&stMsg, sizeof(Vmx_Message_t)) == VK_OK)
		{
			VmxCorePrint(DBG_PRINT, ("msg : param1(%d)\n", stMsg.param1));
			switch (stMsg.param1)
			{
				case eVmx_PsiMsg_Init:
					break;
				case eVmx_PsiMsg_StopSvc:
					{
						HUINT8 		ucServiceIdx = (HUINT8)stMsg.param2;
						pstVmxInst = xsvc_vmx_main_GetInstance();

						if(pstVmxInst != NULL)
						{
							for(i=0;i<VMX_CAS_MAX_NUM_OF_ACTION;i++)
							{
								if(pstVmxInst->stActionList[i].ucVmxServiceIdx == ucServiceIdx)
								{
									vmx_act_bc_StopSessionFromBc(pstVmxInst->stActionList[i].hAction, HANDLE_NULL); // HANDLE_NULL : test only
								}
							}
						}
 					}
					break;
				default:
					VmxCorePrint(DBG_ASSERT, ("Unknown Msg (%d)\n", stMsg.param1));
					break;
			}
		}
	}
}

STATIC HERROR vr_bc_psi_CreateTask(void)
{
	int nErr;
	VmxCorePrint(DBG_PRINT, ("[Enter]\n"));

	VMX_MSG_Create(VMX_TASK_MSGQ_SIZE,  sizeof(Vmx_Message_t), VMX_PSI_QUEUE_NAME, &sVmxPsiQid);

	nErr = VK_TASK_Create((void *)vr_bc_psi_Task, VMX_PSI_TASK_PRIORITY, VMX_PSI_STACK_SIZE, VMX_PSI_TASK_NAME, NULL, &sVmxPsiTid, 0);
	if (nErr)
	{
		VmxCoreError(DBG_ASSERT, ("failed to create task (%d)\n", nErr));
		return ERR_FAIL;
	}

	nErr = VK_TASK_Start(sVmxPsiTid);
	if (nErr)
	{
		VmxCoreError(DBG_ASSERT, ("failed to start task (%d)\n", nErr));
		return ERR_FAIL;
	}
	VmxCorePrint(DBG_PRINT, ("[Exit]\n"));

	return ERR_OK;
}

HERROR vmx_bc_psi_SendMsg(HUINT32 param1, HUINT32 param2, HUINT32 param3, HUINT32 param4)
{
	HERROR rst;
	Vmx_Message_t stMsg;

	stMsg.param1 = param1;
	stMsg.param2 = param2;
	stMsg.param3 = param3;
	stMsg.param4 = param4;

	return VMX_MSG_Send(sVmxPsiQid, &stMsg, sizeof(Vmx_Message_t));
}
#endif

HERROR vmx_vr_bc_psi_mgr_Init(void)
{
	return vr_bc_psi_CreateTask();
}
