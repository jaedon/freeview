/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _xsvc_si_search.h
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

#ifndef ___XSVC_SI_SEARCH_H___
#define ___XSVC_SI_SEARCH_H___

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
extern SvcSi_ActFunc	xsvc_si_SrchGetSearchActionFunc_Eusat (void);
extern void*			xsvc_si_SrchAllocSearchActionData_Eusat (void);
extern void				xsvc_si_SrchFreeSearchActionData_Eusat (void *pvData);

/* ME SAT */
extern SvcSi_ActFunc	xsvc_si_SrchGetSearchActionFunc_MeSat (void);
extern void*			xsvc_si_SrchAllocSearchActionData_MeSat (void);
extern void				xsvc_si_SrchFreeSearchActionData_MeSat (void *pvData);

/* SES */
extern SvcSi_ActFunc	xsvc_si_SrchGetSearchActionFunc_Ses (void);
extern void*			xsvc_si_SrchAllocSearchActionData_Ses (void);
extern void				xsvc_si_SrchFreeSearchActionData_Ses (void *pvData);
extern HERROR			xsvc_si_SrchSrchGetServiceOpType_Ses (HUINT32 ulActionId, DxOperator_e *peSvcOpType);

/* EU DTT */
extern SvcSi_ActFunc	xsvc_si_SrchGetSearchActionFunc_Eudtt (void);
extern void*			xsvc_si_SrchAllocSearchActionData_Eudtt (void);
extern void				xsvc_si_SrchFreeSearchActionData_Eudtt (void *pvData);

/* AUS DTT */
extern HERROR 			xsvc_si_SrchCollectServiceDataFromSi_AusDtt (void *pvData);

extern SvcSi_ActFunc	xsvc_si_SrchGetSearchActionFunc_AusDtt (void);
extern void*			xsvc_si_SrchAllocSearchActionData_AusDtt (void);
extern void				xsvc_si_SrchFreeSearchActionData_AusDtt (void *pvData);

/* ER TELECOM */
extern HERROR 			xsvc_si_SrchGetSearchedResultFromSi_Ertelecom (void *pvData);
extern HERROR			xsvc_si_SrchSvcEsComposeServiceInfoInTp_Ertelecom (HUINT32 ulActionId, HUINT32 *pulSvcNum, DbSvc_Info_t **ppstSvcArray, HUINT32 *pulPrvNum, DbSvc_PrvInfo_t **ppstPrvArray);

extern SvcSi_ActFunc	xsvc_si_SrchGetSearchActionFunc_Ertelecom (void);
extern void*			xsvc_si_SrchAllocSearchActionData_Ertelecom (void);
extern void				xsvc_si_SrchFreeSearchActionData_Ertelecom (void *pvData);

/* NORDIG */
extern HERROR 			xsvc_si_SrchGetSearchedResultFromSi_Nordig (void *pvData);
extern HERROR			xsvc_si_SrchSvcEsComposeServiceInfoInTp_Nordig (HUINT32 ulActionId, HUINT32 *pulSvcNum, DbSvc_Info_t **ppstSvcArray, HUINT32 *pulPrvNum, DbSvc_PrvInfo_t **ppstPrvArray);

extern SvcSi_ActFunc	xsvc_si_SrchGetSearchActionFunc_Nordig (void);
extern void*			xsvc_si_SrchAllocSearchActionData_Nordig (void);
extern void				xsvc_si_SrchFreeSearchActionData_Nordig (void *pvData);
extern HERROR			xsvc_si_SrchProcessForSimualcastService_Nordig (void);


/* UK DTT */
extern HERROR 			xsvc_si_SrchCollectServiceDataFromSi_UkDtt (void *pvData);

extern SvcSi_ActFunc	xsvc_si_SrchGetSearchActionFunc_UkDtt (void);
extern void*			xsvc_si_SrchAllocSearchActionData_UkDtt (void);
extern void				xsvc_si_SrchFreeSearchActionData_UkDtt (void *pvData);

/* ITA DTT */
extern HERROR 			xsvc_si_SrchCollectServiceDataFromSi_ItaDtt (void *pvData);

extern SvcSi_ActFunc	xsvc_si_SrchGetSearchActionFunc_ItaDtt (void);
extern void*			xsvc_si_SrchAllocSearchActionData_ItaDtt (void);
extern void				xsvc_si_SrchFreeSearchActionData_ItaDtt (void *pvData);

/* ORF SAT */
extern SvcSi_ActFunc 	xsvc_si_SrchGetSearchActionFunc_HdFoxIrConnect (void);
extern void*			xsvc_si_SrchAllocSearchActionData_HdFoxIrConnect (void);
extern void 			xsvc_si_SrchFreeSearchActionData_HdFoxIrConnect (void *pvData);


/* EU CABLE*/
extern SvcSi_ActFunc 	xsvc_si_SrchGetSearchActionFunc_Eucab (void);
extern void*			xsvc_si_SrchAllocSearchActionData_Eucab (void);
extern void 			xsvc_si_SrchFreeSearchActionData_Eucab(void);

/* MBC */
extern SvcSi_ActFunc 	xsvc_si_SrchGetSearchActionFunc_Mbc (void);
extern void*			xsvc_si_SrchAllocSearchActionData_Mbc (void);
extern void 			xsvc_si_SrchFreeSearchActionData_Mbc (void *pvData);
extern HERROR 			xsvc_si_SrchGetSearchedResultFromSi_Mbc (void *pvData);

/* AMX CENAM */
extern SvcSi_ActFunc	xsvc_si_SrchGetSearchActionFunc_Amx_Cenam (void);
extern void*			xsvc_si_SrchAllocSearchActionData_Amx_Cenam (void);
extern void				xsvc_si_SrchFreeSearchActionData_Amx_Cenam (void *pvData);

#endif
/* end of file */
