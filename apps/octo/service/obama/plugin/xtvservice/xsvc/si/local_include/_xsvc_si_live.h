/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _xsvc_si_live.h
	@brief

	Description:  									\n
	Module: Porting Layer/TV Service/SI /LIVE			\n

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

#ifndef ___XSVC_SI_LIVE_H___
#define ___XSVC_SI_LIVE_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <_svc_si.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
/* EU SAT */
extern SvcSi_ActFunc	xsvc_si_LiveGetLiveActionFunc_Eusat (void);
extern void*			xsvc_si_LiveAllocLiveActionData_Eusat (void);
extern void				xsvc_si_LiveFreeLiveActionData_Eusat (void *pvData);
extern HBOOL 			xsvc_si_LiveSvcEsUpdateTimeByTdt_Eusat(HUINT32 ulActionId, SIxTable_PsiTable_t *pstTdtTable);

/* ME SAT */
extern SvcSi_ActFunc	xsvc_si_LiveGetLiveActionFunc_MeSat (void);
extern void*			xsvc_si_LiveAllocLiveActionData_MeSat (void);
extern void				xsvc_si_LiveFreeLiveActionData_MeSat (void *pvData);
extern HBOOL 			xsvc_si_LiveIsTimeUpdateAvailable(Handle_t hService);

/* SES */
extern SvcSi_ActFunc	xsvc_si_LiveGetLiveActionFunc_Ses (void);
extern void*			xsvc_si_LiveAllocLiveActionData_Ses (void);
extern void				xsvc_si_LiveFreeLiveActionData_Ses (void *pvData);

/* Free sat */
extern SvcSi_ActFunc	xsvc_si_LiveGetLiveActionFunc_UkFsat (void);
extern void*			xsvc_si_LiveAllocLiveActionData_UkFsat (void);
extern void				xsvc_si_LiveFreeLiveActionData_UkFsat (void *pvData);

/* EU DTT */
extern SvcSi_ActFunc	xsvc_si_LiveGetLiveActionFunc_Eudtt (void);
extern void*			xsvc_si_LiveAllocLiveActionData_Eudtt (void);
extern void				xsvc_si_LiveFreeLiveActionData_Eudtt (void *pvData);

/* AUS DTT */
extern SvcSi_ActFunc	xsvc_si_LiveGetLiveActionFunc_AusDtt (void);
extern void*			xsvc_si_LiveAllocLiveActionData_AusDtt (void);
extern void				xsvc_si_LiveFreeLiveActionData_AusDtt (void *pvData);

/* ER TELECOM */
extern SvcSi_ActFunc	xsvc_si_LiveGetLiveActionFunc_Ertelecom (void);
extern void*			xsvc_si_LiveAllocLiveActionData_Ertelecom (void);
extern void				xsvc_si_LiveFreeLiveActionData_Ertelecom (void *pvData);

/* UK DTT */
extern SvcSi_ActFunc	xsvc_si_LiveGetLiveActionFunc_UkDtt (void);
extern void*			xsvc_si_LiveAllocLiveActionData_UkDtt (void);
extern void				xsvc_si_LiveFreeLiveActionData_UkDtt (void *pvData);

/* ITA DTT */
extern SvcSi_ActFunc	xsvc_si_LiveGetLiveActionFunc_ItaDtt (void);
extern void*			xsvc_si_LiveAllocLiveActionData_ItaDtt (void);
extern void				xsvc_si_LiveFreeLiveActionData_ItaDtt (void *pvData);

/* EU CAB */
extern SvcSi_ActFunc	xsvc_si_LiveGetLiveActionFunc_Eucab (void);
extern void*			xsvc_si_LiveAllocLiveActionData_Eucab (void);
extern void				xsvc_si_LiveFreeLiveActionData_Eucab (void *pvData);

/* ORF SAT */
extern SvcSi_ActFunc 	xsvc_si_LiveGetLiveActionFunc_HdFoxIrConnect (void);
extern void*			xsvc_si_LiveAllocLiveActionData_HdFoxIrConnect (void);
extern void 			xsvc_si_LiveFreeLiveActionData_HdFoxIrConnect (void *pvData);

/* NORDIC */
extern SvcSi_ActFunc	xsvc_si_LiveGetLiveActionFunc_Nordig (void);
extern void*			xsvc_si_LiveAllocLiveActionData_Nordig (void);
extern void				xsvc_si_LiveFreeLiveActionData_Nordig (void *pvData);

/* Meddile-East MBC */
extern SvcSi_ActFunc	xsvc_si_LiveGetLiveActionFunc_Mbc (void);
extern void*			xsvc_si_LiveAllocLiveActionData_Mbc (void);
extern void				xsvc_si_LiveFreeLiveActionData_Mbc (void *pvData);

/* AMX CENAM */
extern SvcSi_ActFunc	xsvc_si_LiveGetLiveActionFunc_Amx_Cenam (void);
extern void*			xsvc_si_LiveAllocLiveActionData_Amx_Cenam (void);
extern void				xsvc_si_LiveFreeLiveActionData_Amx_Cenam (void *pvData);
extern HBOOL			xsvc_si_LiveSvcEsUpdateTimeByTdt_Amx_Cenam(HUINT32 ulActionId, SIxTable_PsiTable_t *pstTdtTable);

#endif
/* end of file */
