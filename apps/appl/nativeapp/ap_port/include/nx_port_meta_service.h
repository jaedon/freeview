/*
 * (c) 2011-2013 Humax Co., Ltd.
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

/******************************************************************************/
/**
 * @file	  		nx_port_meta_service.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_PORT_META_SERVICE_H__
#define __NX_PORT_META_SERVICE_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/

#include <ondk.h>
#include <hapi.h>
#include <papi.h>
#include <hlib.h>
#include <dlib.h>
#include <apk.h>

#include <nx_port_type.h>

/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/


/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/


/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/
typedef HBOOL (* NX_MetaUpdateChecker) (const HCHAR *typeName, const void *_old, const void *_new, HBOOL *updated);
typedef void  (* NxMetaService_Notifier_t) (HINT32 event, const HINT32 *args, void *userdata);

/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/


/*******************************************************************/
/********************	global function prototype   ****************/
/*******************************************************************/
HxVector_t *	NX_PORT_META_SVC_GetServiceList (void);
void	NX_PORT_META_SVC_SetUpdateChecker (NX_MetaUpdateChecker pfChecker);
HERROR	NX_PORT_META_SVC_SetListener (NxMetaService_Notifier_t pfOnUpdate, void *pvUserdata);
HERROR	NX_PORT_META_SVC_Reset(void);
HERROR	NX_PORT_META_SVC_SetService(DxService_t *pstService);
HERROR	NX_PORT_META_SVC_RemoveService(DxService_t *pstService);
HBOOL	NX_PORT_META_SVC_AllowTableNotifyMyself(HCHAR *pTableName, HBOOL bAllow);

#endif // __NX_PORT_META_SERVICE_H__

