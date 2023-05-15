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
 * @file	  		nx_common.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef _NX_COMMON_H_
#define _NX_COMMON_H_


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "nxres_strid.h"
#include "nxres_imgid.h"

/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/


/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/
#define	NX_APP_Malloc(size)				HLIB_MEM_Malloc(size)
#define	NX_APP_Calloc(size)				HLIB_MEM_Calloc(size)
#define	NX_APP_Realloc(mem,size)		HLIB_MEM_Realloc(mem, size)
#define	NX_APP_StrDup(str)				HLIB_MEM_StrDup(str)
#define	NX_APP_MemDup(mem,size)			HLIB_MEM_MemDup(mem, size)
#define	NX_APP_Free(mem)				HLIB_MEM_Free(mem)

#define	NX_APP_Print					HxLOG_Print
#define	NX_APP_Info						HxLOG_Info
#define	NX_APP_Trace					HxLOG_Print
#define	NX_APP_Error					HxLOG_Error
#define	NX_APP_Debug					HxLOG_Debug
#define	NX_APP_Message					HxLOG_Message
#define	NX_APP_Assert(x)				HxLOG_Assert(x)

#define	NX_COMMON_NOT_USED_PARAM(x)							(void)(x)
#define	NX_COMMON_NOT_USED_ARGUMENT(x, p1, p2, p3)			(void)(x); (void)(p1); (void)(p2); (void)(p3)

/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/
#define HANDLE_NULL								0xFFFFFFFF
#define	NX_RES_TEXT_OFF_ID						"Off"

// Text Buffer Size
#define	NX_TEXT_SIZE_4							4
#define	NX_TEXT_SIZE_8							8
#define	NX_TEXT_SIZE_16							16
#define	NX_TEXT_SIZE_32							32
#define	NX_TEXT_SIZE_64							64
#define	NX_TEXT_SIZE_128						128
#define	NX_TEXT_SIZE_256						256
#define	NX_TEXT_SIZE_512						512
#define	NX_TEXT_SIZE_1024						1024

// System Value
#define	NX_SYS_VOLUME_STEP						20
#define	NX_SYS_VOLUME_MAX						NX_SYS_VOLUME_STEP
#define	NX_SYS_VOLUME_MIN						0

// Common Time OUT
#define	NX_COMMON_TIME_SECOND					1000	// 1Sec
#define	NX_COMMON_TIME_DEFAULT					5		// 5Sec

#define	NX_COMMON_TIME_HOURS_PER_DAY			24
#define	NX_COMMON_TIME_MINS_PER_HOUR			60
#define	NX_COMMON_TIME_SECONDS_PER_MIN			60
#define	NX_COMMON_TIME_SECONDS_PER_HOUR			(NX_COMMON_TIME_SECONDS_PER_MIN * NX_COMMON_TIME_MINS_PER_HOUR)
#define	NX_COMMON_TIME_SECONDS_PER_HALF			(NX_COMMON_TIME_SECONDS_PER_HOUR / 2)
#define	NX_COMMON_TIME_SECONDS_PER_DAY			(NX_COMMON_TIME_SECONDS_PER_HOUR * NX_COMMON_TIME_HOURS_PER_DAY)
#define	NX_COMMON_TIME_SECONDS_PER_WEEK			(NX_COMMON_TIME_SECONDS_PER_DAY * 7)

#define	NX_COMMON_SERIAL_NUMBER_SIZE			14
#define	NX_COMMON_LANGUAGE_CODE_SIZE			4


/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/


/*******************************************************************/
/********************	global function prototype   ****************/
/*******************************************************************/

#endif // _NX_COMMON_H_

