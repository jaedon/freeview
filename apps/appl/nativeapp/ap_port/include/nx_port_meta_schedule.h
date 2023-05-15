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

/******************************************************************************/
/**
 * @file	  		nx_port_meta_service.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_PORT_META_SCHEDULE_H__
#define __NX_PORT_META_SCHEDULE_H__

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
#define NX_META_SCHED_MAX_CONFLICT	10
#define NX_META_SCHED_MAX_SERIESID	128
#define NX_MAX_CONFLICT_TPNUM		4

/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/
typedef struct _META_SCHEDULER_t_* NX_MetaSched_Handle_t;

typedef enum
{
	eNX_METASCH_CONFTYPE_None,

	eNX_METASCH_CONFTYPE_Tp,
	eNX_METASCH_CONFTYPE_OverCapacity,
	eNX_METASCH_CONFTYPE_View,
	eNX_METASCH_CONFTYPE_Duplicated,
	eNX_METASCH_CONFTYPE_PaddingTime,
	eNX_METASCH_CONFTYPE_Alternative,
	eNX_METASCH_CONFTYPE_RfOta,
	eNX_METASCH_CONFTYPE_DataOnly,
	eNX_METASCH_CONFTYPE_Unknown,

	eNX_METASCH_CONFTYPE_EndOfCase
} NX_META_SCH_Conflict_e;

typedef struct
{
	NX_META_SCH_Conflict_e	type;
	HUINT32					length[NX_MAX_CONFLICT_TPNUM];
	HINT32					uidList[NX_MAX_CONFLICT_TPNUM][NX_META_SCHED_MAX_CONFLICT];
	HUINT32					alt_length;
	HINT32					alt_uidList[NX_META_SCHED_MAX_CONFLICT];
	HUID					alt_evtuidList[NX_META_SCHED_MAX_CONFLICT];
} NX_SCHED_CONFLICT_t;

typedef void(* NX_MetaSched_Listener_t) (NX_MetaSched_Handle_t self, HINT32 event, HUINT32 ulUid, DxSCHEDULE_t *pstDxSchedule, NX_SCHED_CONFLICT_t *pstConflict, void *userdata);

/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/
NX_MetaSched_Handle_t	NX_PORT_SCHEDULE_GetInstance(void);


/*******************************************************************/
/********************	global function prototype   ****************/
/*******************************************************************/

#endif // __NX_PORT_META_SCHEDULE_H__


