/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  re_init.h
	@brief

	Description:  									\n
	Module: HOMMA / API								\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
*
*/
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



#ifndef	__REMA_MODULE_INTERNAL_H_
#define	__REMA_MODULE_INTERNAL_H_

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>

#ifdef __cplusplus
extern "C"
{
#endif
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

HERROR rema_MOD_Internal_Init(void);
void rema_MOD_Internal_Deinit(void);

HINT32 rema_MOD_Internal_resetUpdate(void);

void *rema_MOD_Internal_GetCTXNSLookupInfo(void);
HERROR rema_MOD_Internal_GetStrNSLookupInfo(void *pvCtx, const HCHAR *pszName, HCHAR *pBuf, HINT32 nBufLen);
HERROR rema_MOD_Internal_SetStrNSLookupInfo(void *pvCtx, const HCHAR *pszName, const HCHAR *pBuf);
HERROR rema_MOD_Internal_GetUINTNSLookupInfo(void *pvCtx, const HCHAR *pszName, HUINT32 *punData);
HERROR rema_MOD_Internal_SetUINTNSLookupInfo(void *pvCtx, const HCHAR *pszName, const HUINT32 unData);


void *rema_MOD_Internal_GetCTXStatisicsInfo(void);
HERROR rema_MOD_Internal_GetUINTStatisic_MinSampleInterval(void *pvCtx, HUINT32 *punData);
HERROR rema_MOD_Internal_GetUINTStatisic_MaxReportSamples(void *pvCtx, HUINT32 *punData);

HERROR rema_MOD_Internal_GetStatisic_SampleSet_Max(void *pvCtx, HUINT32 *punMax);

HERROR rema_MOD_Internal_AddStatisic_SampleSet(void *pvCtx, HUINT32 *punAddedNum);
HERROR rema_MOD_Internal_DelStatisic_SampleSet(void *pvCtx, HUINT32 unDelNum);

void *rema_MOD_Internal_GetCTXAddStatisic_SampleSet(void *pvCtx, HUINT32 unNum);

HERROR rema_MOD_Internal_GetStatisic_SampleSet_Name(void *pvCtx, HCHAR *pBuf, HINT32 nBufLen);


HERROR rema_MOD_Internal_GetStatisic_SampleSet_Parameter_Max(void *pvCtx, HUINT32 *punMax);

/*
HERROR rema_MOD_Internal_GetStrStatisicsInfo(void *pvCtx, const HCHAR *pszName, HCHAR *pBuf, HINT32 nBufLen);
HERROR rema_MOD_Internal_SetStrStatisicsInfo(void *pvCtx, const HCHAR *pszName, const HCHAR *pBuf);
HERROR rema_MOD_Internal_GetUINTStatisicsInfo(void *pvCtx, const HCHAR *pszName, HUINT32 *punData);
HERROR rema_MOD_Internal_SetUINTStatisicsInfo(void *pvCtx, const HCHAR *pszName, const HUINT32 unData);
*/
/*******************************************************************/
/********************      extern Functions     ********************/
/*******************************************************************/

#ifdef __cplusplus
}
#endif

#endif //__REMA_CORE_H__
