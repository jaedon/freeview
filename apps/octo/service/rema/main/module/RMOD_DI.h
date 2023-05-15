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



#ifndef	__REMA_MODULE_DI_H_
#define	__REMA_MODULE_DI_H_

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

HERROR RMOD_DI_Init(void);
void RMOD_DI_Deinit(void);

HINT32 RMOD_DI_resetUpdate(void);

int RMOD_DI_ProcessorInfo(void);


HERROR RMOD_DI_GetProcessStatus_Max(HUINT32 *punMax);
HERROR RMOD_DI_GetProcessStatus_Pid(HUINT32 unIndex, HUINT32 *punData);
HERROR RMOD_DI_GetProcessStatus_Command(HUINT32 unIndex, HCHAR *pBuf, HINT32 nBufLen);
HERROR RMOD_DI_GetProcessStatus_Size(HUINT32 unIndex, HUINT32 *punData);
HERROR RMOD_DI_GetProcessStatus_Priority(HUINT32 unIndex, HUINT32 *punData);
HERROR RMOD_DI_GetProcessStatus_CPUTime(HUINT32 unIndex, HUINT32 *punData);
HERROR RMOD_DI_GetProcessStatus_State(HUINT32 unIndex, HCHAR *pBuf, HINT32 nBufLen);
HERROR RMOD_DI_GetTemperatureSensor_Max(HUINT32 *punMax);



//void *rema_MOD_Internal_GetCTXNSLookupInfo(void);
HERROR RMOD_DI_GetStrLAN_Statues(const HCHAR *pszName, HCHAR *pBuf, HINT32 nBufLen);
HERROR RMOD_DI_GetUINTLAN_Statues(const HCHAR *pszName, HUINT32 *punData);


HERROR RMOD_DI_GetCPUUsage(HUINT32 *CPUUsage);


/*******************************************************************/
/********************      extern Functions     ********************/
/*******************************************************************/

#ifdef __cplusplus
}
#endif

#endif //__REMA_CORE_H__
