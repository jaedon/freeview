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
/********************************************************************
 * $Workfile:   _xsvc_cas_vmx_os.h  $
 * Project: 	 Verimatrix CAS
 * Author:		ahn
 * Description:	API for Verimatrix
 *
 *                              Copyright (c) 2015 by HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/

#ifndef	__INT_XSVC_VMXOS_H__
#define	__INT_XSVC_VMXOS_H__

#include "vkernel.h"
#include <string.h>
#include <stdarg.h>

// TODO_VMX : 아래 TASK 정리 필요.

#define VMX_TASK_PRIORITY 					(VK_TASK_PRIORITY_MW_REF+3)
#define VMX_TASK_STACK_SIZE				(SIZE_32K)
#define VMX_TASK_MSGQ_SIZE				(64)

// vmxmain
#define VMX_MAIN_TASK_PRIORITY			VMX_TASK_PRIORITY
#define VMX_MAIN_TASK_NAME				"VMXMAIN"
#define VMX_MAIN_STACK_SIZE				VMX_TASK_STACK_SIZE

// bctacrypt task
#define VMXBC_BCLIB_TASK_PRIORITY		VK_TASK_PRIORITY_MW_REF+0//VMX_TASK_PRIORITY
#define VMXBC_BCLIB_TASK_NAME			"VMXBCLIB"
#define VMXBC_BCLIB_STACK_SIZE			VMX_TASK_STACK_SIZE

// smartcard task
#define VMXBC_SC_TASK_PRIORITY			VK_TASK_PRIORITY_MW_REF+1
#define VMXBC_SC_TASK_NAME				"VMXBCSCT"
#define VMXBC_SC_QUEUE_NAME			"VMXBCSCQ"
#define VMXBC_SC_STACK_SIZE				VMX_TASK_STACK_SIZE

// psi mgr task
#define VMX_PSI_TASK_PRIORITY				VK_TASK_PRIORITY_MW_REF+1
#define VMX_PSI_TASK_NAME				"VMXPSIT"
#define VMX_PSI_QUEUE_NAME				"VMXPSIQ"
#define VMX_PSI_STACK_SIZE				VMX_TASK_STACK_SIZE

// iptvh mgr
#define VMX_IPTVH_MGR_TASK_PRIORITY		VMX_TASK_PRIORITY
#define VMX_IPTVH_MGR_TASK_NAME		"VMXIPTVHT"
#define VMX_IPTVH_MGR_STACK_SIZE		VMX_TASK_STACK_SIZE
#define VMX_IPTVH_MGR_TASK_MSGQ_SIZE	VMX_TASK_MSGQ_SIZE
#define VMX_IPTVH_MGR_TASK_MSGQ_NAME	"VMXIPTVHQ"

#define VMX_TASK_Create(a,b,c,d,e,f,g)	VK_TASK_Create(a,b,c,d,e,f,g)
#define VMX_TASK_Destroy(a)				VK_TASK_Destroy(a)
#define VMX_TASK_Start(a)				VK_TASK_Start(a)
#define VMX_TASK_Sleep(a)				VK_TASK_Sleep(a)

#define VMX_SEM_Create(a,b)				VK_SEM_Create(b,a,VK_SUSPENDTYPE_PRIORITY)		// name,id --> id, name
#define VMX_SEM_Destroy(a)					VK_SEM_Destroy(a)
#define VMX_SEM_Get(a)						VK_SEM_Get(a)
#define VMX_SEM_Release(a)				VK_SEM_Release(a)

#define VMX_MSG_Create(a,b,c,d)			VK_MSG_Create(a,b,c,d,VK_SUSPENDTYPE_PRIORITY)
#define VMX_MSG_Destroy(a)					VK_MSG_Destroy(a)
#define VMX_MSG_Send(a,b,c)				VK_MSG_Send(a,b,c)
#define VMX_MSG_Receive(a,b,c)				VK_MSG_Receive(a,b,c)
#define VMX_MSG_ReceiveTimeout(a,b,c,d)	VK_MSG_ReceiveTimeout(a,b,c,d)

#define VMX_MEM_Alloc(a)					CA_MEM_Alloc(a)
#define VMX_MEM_Free(a)					CA_MEM_Free(a)

#define VMX_TIMER_EventAfter(a,b,c,d,e)		VK_TIMER_EventAfter(a,b,c,d,e)

#define VMX_memset(a,b,c)					VK_memset(a,b,c)
#define VMX_memcpy(a,b,c)					VK_memcpy(a,b,c)
#define VMX_memcmp(a,b,c)					memcmp(a,b,c)
#define VMX_strcmp(a,b)						strcmp(a,b)
#define VMX_strcpy(a,b)						strcpy(a,b)
#define VMX_memmove(a, b, c)				memmove(a, b, c)
#endif /* __INT_XSVC_VMXOS_H__ */

