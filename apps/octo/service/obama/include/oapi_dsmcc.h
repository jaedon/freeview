/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_dsmcc.h
	@brief

	Description:  									\n
	Module: OAPI / API					\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
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

#ifndef __OAPI_DSMCC_H__
#define __OAPI_DSMCC_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <dlib.h>
/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
// From AKP To OBAMA
#define RPC_OAPI_DSMCC_Init							"oapi.dsmcc.Init"
#define RPC_OAPI_DSMCC_UnInit						"oapi.dsmcc.UnInit"
#define RPC_OAPI_DSMCC_CreateHandle					"oapi.dsmcc.CreateHandle"
#define RPC_OAPI_DSMCC_Start							"oapi.dsmcc.Start"
#define RPC_OAPI_DSMCC_ReStart						"oapi.dsmcc.ReStart"
#define RPC_OAPI_DSMCC_Stop							"oapi.dsmcc.Stop"
#define RPC_OAPI_DSMCC_RequestEvent					"oapi.dsmcc.RequestEvent"
#define RPC_OAPI_DSMCC_Open							"oapi.dsmcc.Open"
#define RPC_OAPI_DSMCC_Close					"oapi.dsmcc.Close"


// From OBAMA To AKP
#define RPC_OAPI_DSMCC_EventNotifier				"oapi.dsmcc.EventNotifier"

//Live AIT
#define OxDSMCC_AIT_SECTION_NUM_MAX					8


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef	void	(*OAPI_DSMCC_Notifiler_t)(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
typedef void	(*OAPI_DSMCC_EventFreeFunc_t)	(HINT32 nArgc, void *apArgV[]);
typedef	void	(*OAPI_DSMCC_NotifilerForAppkit_t)(OAPI_DSMCC_Notifiler_t pfnCbFunc, OAPI_DSMCC_EventFreeFunc_t pfnFreeFunc, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);

typedef	enum
{
	eDsmccAitState_None		= 0,
	eDsmccAitState_Timeout,
	eDsmccAitState_Received
} OxDsmcc_AitState_e;

typedef struct
{
	OxDsmcc_AitState_e			 eAitState;
	HUINT32						 ulAitSecCount;								// ait section number
	HUINT16						 usPrevAitPid;
	HUINT16						 usCurrAitPid;

	HUINT32						 ulAitTotaSeclLen;							// sizeof pucAitRawSec
	HUINT32						 aulAitSecLen[OxDSMCC_AIT_SECTION_NUM_MAX];	// length of each ait section
	HUINT8						*paucAitRawSec[OxDSMCC_AIT_SECTION_NUM_MAX];
} OxDsmcc_AitSectionData_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

void	OAPI_DSMCC_Init(OAPI_DSMCC_NotifilerForAppkit_t pfnAppkitNotifier);
void	OAPI_DSMCC_DeInit(void);
HERROR OAPI_DSMCC_Create(DxDSMCC_Type_e eType, Handle_t *phDsmcc, HCHAR **ppszCachePath);
HERROR  OAPI_DSMCC_Destroy(Handle_t hDsmcc);
void	OAPI_DSMCC_FreePath(HCHAR *ppszCachePath);
void	OAPI_DSMCC_RegisterNotifier(Handle_t hDsmcc, OAPI_DSMCC_Notifiler_t pfnObjectHandler);
void	OAPI_DSMCC_UnRegisterNotifier(Handle_t hDsmcc, OAPI_DSMCC_Notifiler_t pfnObjectHandler);
HERROR OAPI_DSMCC_GetCarouselList(Handle_t hDsmcc, DxDSMCC_CarouselList_t *pstDsmccCarouselList);
void	OAPI_DSMCC_FreeCarouselInfo(DxDSMCC_CarouselInfo_t *pCarouselInfo);
HERROR OAPI_DSMCC_Start(Handle_t hDsmcc, Handle_t hCarousel, const HCHAR *pszCachePath);
HERROR OAPI_DSMCC_ReStart(Handle_t hDsmcc, Handle_t hCarousel);
HERROR OAPI_DSMCC_Stop(Handle_t hDsmcc, Handle_t hCarousel);
HERROR OAPI_DSMCC_RequestEvent(Handle_t hDsmcc, HBOOL bAll);
HERROR OAPI_DSMCC_Open(Handle_t hDsmcc, Handle_t hSession);
HERROR OAPI_DSMCC_Close(Handle_t hDsmcc, Handle_t hSession);
HERROR OAPI_DSMCC_CountAitSection (Handle_t hDsmcc, HUINT32 *pulAitNum);
HERROR OAPI_DSMCC_GetAitPid (Handle_t hDsmcc, HUINT16 *pusPrevAitPid, HUINT16 *pusCurrAitPid);
HERROR OAPI_DSMCC_GetAitSection (Handle_t hDsmcc, HUINT32 ulIndex, HUINT8 **ppucAitSec, HUINT32 *ulSecLen);
HERROR OAPI_DSMCC_FreeAitSection (HUINT8 *pucAitSec);

#endif // __OAPI_DSMCC_H__
