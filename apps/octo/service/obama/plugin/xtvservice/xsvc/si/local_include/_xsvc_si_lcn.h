/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _xsvc_si_lcn.h
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

#ifndef ___XSVC_SI_LCN_H___
#define ___XSVC_SI_LCN_H___

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
extern HERROR 			xsvc_si_LcnSetServiceNumber_Eusat(Handle_t hAction, HUINT32 ulNum, DbSvc_Info_t *pstSvcArray, SRCHSVC_AppendFlag_t *peFlagArray);

/* ME SAT */
extern HERROR 			xsvc_si_LcnSetServiceNumber_MeSat(Handle_t hAction, HUINT32 ulNum, DbSvc_Info_t *pstSvcArray, SRCHSVC_AppendFlag_t *peFlagArray);

/* SES */
extern HERROR 			xsvc_si_LcnSetServiceNumber_Ses(Handle_t hAction, HUINT32 ulNum, DbSvc_Info_t *pstSvcArray, SRCHSVC_AppendFlag_t *peFlagArray);

/* EU DTT */
extern HERROR 			xsvc_si_LcnSetServiceNumber_Eudtt(Handle_t hAction, HUINT32 ulNum, DbSvc_Info_t *pstSvcArray, SRCHSVC_AppendFlag_t *peFlagArray);

/* AUS DTT */
extern HERROR 			xsvc_si_LcnSetServiceNumber_AusDtt(Handle_t hAction, HUINT32 ulNum, DbSvc_Info_t *pstSvcArray, SRCHSVC_AppendFlag_t *peFlagArray);

/* ER TELECOM */
extern HERROR 			xsvc_si_LcnSetServiceNumber_Ertelecom(Handle_t hAction, HUINT32 ulNum, DbSvc_Info_t *pstSvcArray, SRCHSVC_AppendFlag_t *peFlagArray);

/* NORDIG */
extern HERROR 			xsvc_si_LcnSetServiceNumber_Nordig (Handle_t hAction, HUINT32 ulNum, DbSvc_Info_t *pstSvcArray, SRCHSVC_AppendFlag_t *peFlagArray);

/* UK DTT */
extern HERROR 			xsvc_si_LcnSetServiceNumber_UkDtt(Handle_t hAction, HUINT32 ulNum, DbSvc_Info_t *pstSvcArray, SRCHSVC_AppendFlag_t *peFlagArray);
#if defined(CONFIG_OP_UK_DTT)
extern HERROR 			xsvc_si_LcnGetTrdConflictedList_UkDtt(HUINT32 ulDepth,HUINT32 *pulconflictcount,HxList_t **ppConflictList);
extern HERROR 			xsvc_si_LcnSetTrdConflictedList_UkDtt(HUINT32 ulDepthIdx, HUINT32 ulSelectIdx);
extern HERROR			xsvc_si_LcnInitConflictSvcHandleTable_UkDtt(void);
#endif

/* ITA DTT */
extern HERROR 			xsvc_si_LcnSetServiceNumber_ItaDtt(Handle_t hAction, HUINT32 ulNum, DbSvc_Info_t *pstSvcArray, SRCHSVC_AppendFlag_t *peFlagArray);

extern HERROR 			xsvc_si_LcnSetServiceNumber_HdFoxIrConnect(Handle_t hAction, HUINT32 ulNum, DbSvc_Info_t *pstSvcArray, SRCHSVC_AppendFlag_t *peFlagArray);

#if defined(CONFIG_OP_ITALY_DBOOK)
extern HERROR			xsvc_si_LcnSetSelectedConflictLcn_ItaDtt (Handle_t hAction, HUINT32 ulNum, DxBuilder_SearchedInfo_t *pstInfoArray);
#endif

#if defined(CONFIG_DEBUG)
#if defined(CONFIG_OP_UK_DTT)
extern Cutest_Suite_t	*xsvc_si_UtGetSuites_UkDtt(void);
#endif
#endif

/* AMX CENAM */
extern HERROR xsvc_si_LcnSetServiceNumber_Amx_Cenam (Handle_t hAction, HUINT32 ulNum, DbSvc_Info_t *pstSvcArray, SRCHSVC_AppendFlag_t *peFlagArray);

#endif

/* end of file */
