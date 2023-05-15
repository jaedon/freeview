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
@file				cxcore_ecm.h
@brief			cxcore_ecm.h

	Description:   \n
	Module: MW/CAS \n

	Copyright (c) 2009 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

#ifndef	__CXCORE_ECM_H__
#define	__CXCORE_ECM_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <conax.h>

/*******************************************************************/
/********************      Global Variables 	********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition 	********************/
/*******************************************************************/

#define CX_ECM_FILTER_LEN					8

/*******************************************************************/
/********************      Typedef  	   *************************/
/*******************************************************************/

typedef enum
{
	eCxEcm_Both	= 0,
	eCxEcm_Even	= 0x80,
	eCxEcm_Odd	= 0x81
} CxEcmMode_t;

/*******************************************************************/
/********************      Public Functions 	********************/
/*******************************************************************/
HERROR CXECM_InitCore(void);
Handle_t CXECM_GetStbHandle(Handle_t hEcm);
Handle_t CXECM_GetHandleBySession(Handle_t hSession);

HERROR CXECM_StartFilter(Handle_t hEcm, CxEcmMode_t eMode);
HERROR CXECM_StopFilter(Handle_t hEcm);
HERROR CXECM_RestartFilter(Handle_t hEcm, CxEcmMode_t eMode);
Handle_t CXECM_Add(Handle_t hSession, HUINT16 usEcmPid);
HERROR CXECM_Delete(Handle_t hEcm);
HUINT16 CXECM_GetPid(Handle_t hEcm);
HERROR CXECM_SetCW(Handle_t hEcm, HUINT8* pucOddCW, HUINT32 ulOddLen, HUINT8* pucEvenCW, HUINT32 ulEvenLen);
HERROR CXECM_GetCW(Handle_t hEcm, HUINT8* pucOddCW, HUINT8* pucEvenCW);
#if defined(CONFIG_MW_CAS_CONAX_CPM)
HERROR CXECM_SetCWPK(Handle_t hEcm, HUINT8* pucCWPK);
HERROR CXECM_GetCWPK(Handle_t hEcm, HUINT8* pucCWPK);
#endif

HBOOL CXECM_IsQueueEmpty(void);
HUINT8* CXECM_QueueGet(Handle_t* pSessionHandle, Handle_t* pEcmHandle);
HERROR CXECM_QueueClear(void);

#if defined(CONFIG_MW_MM_PVR)
HERROR CXECM_GetAccessStatus(Handle_t hEcm, HUINT8* pucProgMr);
HERROR CXECM_SetAccessStatus(Handle_t hEcm, HUINT8 ucProgMr);
#endif

#if defined(CONFIG_DEBUG)
void CMD_CXECM_PrintEcmContext(void);
#endif
#endif /* !__CXCORE_ECM_H__ */
