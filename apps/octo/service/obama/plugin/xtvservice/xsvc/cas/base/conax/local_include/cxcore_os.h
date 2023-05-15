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
/*******************************************************************
	File Description
********************************************************************/
/**
@file				cxcore_os.h
@brief			cxcore_os.h (Conax OS porting)

	Description:   \n
	Module: MW/CAS/ CONAX \n

	Copyright (c) 2009 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

#ifndef	__CXCORE_OS_H__
#define	__CXCORE_OS_H__

#include "vkernel.h"
#include <string.h>
#include <stdarg.h>

#define CX_TASK_Create(a,b,c,d,e,f,g)	VK_TASK_Create(a,b,c,d,e,f,g)
#define CX_TASK_Destroy(a)				VK_TASK_Destroy(a)
#define CX_TASK_Start(a)				VK_TASK_Start(a)
#define CX_TASK_Sleep(a)				VK_TASK_Sleep(a)

#define CX_SEM_Create(a,b)				VK_SEM_Create(b,a,VK_SUSPENDTYPE_PRIORITY)		// name,id --> id, name
#define CX_SEM_Destroy(a)				VK_SEM_Destroy(a)
#define CX_SEM_Get(a)					VK_SEM_Get(a)
#define CX_SEM_Release(a)				VK_SEM_Release(a)

#define CX_MSG_Create(a,b,c,d)			VK_MSG_Create(a,b,c,d,VK_SUSPENDTYPE_PRIORITY)
#define CX_MSG_Destroy(a)				VK_MSG_Destroy(a)
#define CX_MSG_Send(a,b,c)				VK_MSG_Send(a,b,c)
#define CX_MSG_Receive(a,b,c)			VK_MSG_Receive(a,b,c)
#define CX_MSG_ReceiveTimeout(a,b,c,d)	VK_MSG_ReceiveTimeout(a,b,c,d)

#define CX_MEM_Alloc(a)					OxCAS_Malloc(a)
#define CX_MEM_Calloc(a)				OxCAS_Calloc(a)
#define CX_MEM_Free(a)					OxCAS_Free(a)

#define CX_TIMER_EventAfter(a,b,c,d,e)	VK_TIMER_EventAfter(a,b,c,d,e)

#define CX_memset(a,b,c)				HxSTD_memset(a,b,c)
#define CX_memcpy(a,b,c)				HxSTD_memcpy(a,b,c)
#define CX_memcmp(a,b,c)				memcmp(a,b,c)
#define CX_strcmp(a,b)					strcmp(a,b)

#define CX_MemDump(a,b,c)				do {													\
												HxLOG_Warning(HxANSI_COLOR_YELLOW("%s"), a);	\
												HLIB_LOG_Dump(b, c, 0, TRUE);					\
										} while(0)

#endif /* __CXCORE_OS_H__ */
