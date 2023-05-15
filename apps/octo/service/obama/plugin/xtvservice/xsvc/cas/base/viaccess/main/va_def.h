/*
 * (c) 2011-2012 Humax Co., Ltd.
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
/*
	viaccess global header
*/

#ifndef VA_DEF_H
#define VA_DEF_H

#include <octo_common.h>
//#include "_svc_cas_mgr.h"
#include <_svc_cas_mgr_common.h>

// DATE_TIME_T, UNIXTIME가 정의된 header file
#include <hlib.h> // for hxdatetime

//#include "taskdef.h"
#include <util.h>
#include <vkernel.h>

#define __VIACCESS50__	1
// BBX data update가 되었는지 확인하기 위한 임시 조치..
#define VA_BBX_INIT_CHECK

#define VA_ERR_OK		0			// ERR_OK
#define VA_ERR			(-1)		// ERR_FAIL 값과 다르므로 주의 요함. 리턴값 체크시 VA_ERR 가 아닌 VA_ERR_OK 로 체크할 것!
#define VA_ERR_UI		(-1000)		//

#define VA_INVALID_HANDLE		0xFFFFFFFF
#define VA_INVALID_PID		0x2000

// semaphore
#define VA_DMX_SEM_NAME				"VDMS"
#define VA_DSCR_SEM_NAME				"VDSS"
#define VA_PI_SEM_NAME					"VPIS"
#define VA_PIMW_SEM_NAME				"VPMS"
#define VA_UI_SEM_NAME					"VUIS"
#define VA_NVM_SEM_NAME				"VNVS"
#define VA_PVR_SEM_NAME				"VPVS"
#define VA_TSR_SEM_NAME				"VTSS"
#define VA_UI_ADAPT_SEM_NAME			"VUAS"
#define VA_OP_MSG_SEM_NAME			"VOMS"
#if defined(CONFIG_MW_CAS_VIACCESS_NET)
#define VA_NET_SEM_NAME				"VNES"
#endif

// main task : ACS main thread 가 기생하는 이 task 의 priority 는 다른 task 보다 낮아야 한다 (Viaccess Spec 왈 The ACS-dedicated thread does not have priority over the driver thread.)
#if defined(CONFIG_PROD_PROFILE_MINIOCTO)
#define VA_MAIN_TASK_PRIORITY			VK_TASK_PRIORITY_MW_REF-3
#else
#define VA_MAIN_TASK_PRIORITY			VK_TASK_PRIORITY_MW_REF-2
#endif
#define VA_MAIN_TASK_STACK_SIZE		SIZE_32K
#define VA_MAIN_TASK_NAME				"VACT"

// NVM task
#define VA_NVM_TASK_PRIORITY			VK_TASK_PRIORITY_MW_REF+1
#define VA_NVM_TASK_STACK_SIZE		SIZE_16K
#define VA_NVM_TASK_NAME				"VNVT"
#define VA_NVM_QUEUE_NAME				"VNVQ"

// smartcard task
#define VA_SC_TASK_PRIORITY			VK_TASK_PRIORITY_MW_REF+1
#define VA_SC_TASK_STACK_SIZE			SIZE_16K
#define VA_SC_TASK_NAME				"VSCT"
#define VA_SC_QUEUE_NAME				"VSCQ"

#if defined(CONFIG_MW_CAS_VIACCESS_NET)
// NET task
#define VA_NET_TASK_PRIORITY			VK_TASK_PRIORITY_MW_REF+1
#define VA_NET_TASK_STACK_SIZE			SIZE_16K
#define VA_NET_TASK_NAME				"VNET"
#define VA_NET_QUEUE_NAME				"VNEQ"
#define VA_NET_RECV_TASK_NAME			"VNER"
#define VA_NET_RECV_QUEUE_NAME			"VNRQ"
#endif

// OS task (timer)
#define VA_OS_TASK_PRIORITY			VK_TASK_PRIORITY_MW_REF+1
#define VA_OS_TASK_STACK_SIZE			SIZE_16K
#define VA_OS_TASK_NAME				"VOST"

// UI task
#define VA_UI_REQUEST_QUEUE_NAME		"VURQ"

// PVR task
#define VA_PVR_TASK_PRIORITY			VK_TASK_PRIORITY_MW_REF+1
#define VA_PVR_TASK_STACK_SIZE		SIZE_32K
#define VA_PVR_TASK_NAME				"VPVT"
#define VA_PVR_QUEUE_NAME				"VPVQ"

// OS Porting (NEO와 차이때문에)
#define VA_SEM_Create(a,b)				VK_SEM_Create(b,a,VK_SUSPENDTYPE_PRIORITY)		// name,id --> id, name
#define VA_SEM_CreateWithCount(a,b,c)	VK_SEM_CreateWithCount(b,c,a,VK_SUSPENDTYPE_PRIORITY)		// name,id,count --> id, count, name
#define VA_SEM_Destroy(a)				VK_SEM_Destroy(a)
#define VA_SEM_Get(a)					VK_SEM_Get(a)
#define VA_SEM_Release(a)				VK_SEM_Release(a)

#define VA_MSG_Create(a,b,c,d)			VK_MSG_Create(a,b,c,d,VK_SUSPENDTYPE_PRIORITY)
#define VA_MSG_Destroy(a)				VK_MSG_Destroy(a)
#define VA_MSG_Send(a,b,c)				VK_MSG_Send(a,b,c)
#define VA_MSG_Receive(a,b,c)			VK_MSG_Receive(a,b,c)
#define VA_MSG_ReceiveTimeout(a,b,c,d)	VK_MSG_ReceiveTimeout(a,b,c,d)

#define VA_MEM_Alloc(a)					OxCAS_Malloc(a)
#define VA_MEM_Free(a)					OxCAS_Free(a)

#define VA_memset(a,b,c)					VK_memset(a,b,c)
#define VA_memcpy(a,b,c)				VK_memcpy(a,b,c)
#define VA_memcmp(a,b,c)				memcmp(a,b,c)
#define VA_strcmp(a,b)					strcmp(a,b)

// msgQ로 전송하는 data가 "UINT32 msg[4]" 임 -> 4 * 4 = 16 bytes
#define VA_CreateMessageQueue( a, b )	VA_MSG_Create(32, 16, a, b)
#define VA_PostMessage(a,b) 				VA_MSG_Send(a,b,16)
#define VA_GetMessage(a,b)				VA_MSG_Receive(a,b,16)
#define VA_GetMessageTimeout(a,b,c)		VA_MSG_ReceiveTimeout(a,b,16,c)

#define VA_ASSERT(a)		if (!(a)) { HxLOG_Print("[VA_ASSERT ERROR](%s, %d)\n", __FILE__, __LINE__); while(1); }
#define VA_ASSERT2(a, b)		if (!(a)) { HxLOG_Print("[VA_ASSERT ERROR](%s, %d) %s\n", __FILE__, __LINE__, b); while(1); }

#define	Get12bit(a)					get12bit(a)
#define	Get13bit(a)					get13bit(a)
#define	Get16bit(a)					get16bit(a)
#define	Get24bit(a)					get24bit(a)
#define	Get32bit(a)					get32bit(a)
#define	Put16bit(p,data)				put16bit(p,data)
#define	Put32bit(p,data)				put32bit(p,data)

#endif // VA_DEF_H

