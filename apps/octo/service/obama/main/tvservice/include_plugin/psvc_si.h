/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  psvc_si.h
	@brief

	Description:  									\n
	Module: SVC / SI_NEW					\n

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

#ifndef ___PSVC_SI_LIVE_H___
#define ___PSVC_SI_LIVE_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <silib.h>
#include <octo_common.h>

#include <db_svc.h>

#include <svc_si.h>
//#include <_svc_si.h>
#include <_svc_si_lib.h>
#if defined(CONFIG_DEBUG)
#include <cutest.h>
#endif

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
/********************      API Functions     ********************/
/*******************************************************************/
extern SvcSi_ActFunc	psvc_si_GetActionFunc(SvcSi_ActionKind_e eActKind, HUINT32 ulActSpec);
extern void*			psvc_si_AllocActionData(SvcSi_ActionKind_e eActKind, HUINT32 ulActSpec);
extern void				psvc_si_FreeActionData(SvcSi_ActionKind_e eActKind, HUINT32 ulActSpec, void *pvData);

extern HERROR			psvc_si_SetSiMode(HUINT32 ulSiMode);
extern HERROR			psvc_si_GetSiMode(HUINT32 *pulSiMode);

extern HERROR			psvc_si_GetSiPrivateSpecifyValue(HUINT32 ulActSpec, HUINT32 *pulPrivateSpecifyValue);

// Actions
extern HERROR	psvc_si_GetPmtEsInfoFromPmtSiTable(HUINT32 ulActionId, SIxTable_PsiTable_t *pstSiTable, svcSi_PmtEsInfo_t *pstPmtEsInfo);
extern HERROR	psvc_si_SelectVideoEs(HUINT32 ulActionId, svcSi_PmtEsInfo_t *pstPmtEsInfo, DbSvc_Info_t *pstSvcInfo, svc_si_SelectedEs_t *pstSelectedVideo);
extern HERROR	psvc_si_SelectAudioEs(HUINT32 ulActionId, svcSi_PmtEsInfo_t *pstPmtEsInfo, DbSvc_Info_t *pstSvcInfo, svc_si_SelectedAudio_t *pstSelectedAudio);
extern HERROR	psvc_si_SelectSubtitleEs(HUINT32 ulActionId, svcSi_PmtEsInfo_t *pstPmtEsInfo, DbSvc_Info_t *pstSvcInfo, svc_si_SelectedEs_t *pstSelectedSbtl);
extern HERROR	psvc_si_SelectTeletextEs(HUINT32 ulActionId, svcSi_PmtEsInfo_t *pstPmtEsInfo, DbSvc_Info_t *pstSvcInfo, svc_si_SelectedEs_t *pstSelectedTtx);
extern HERROR	psvc_si_ComposeServiceInfoInTp(HUINT32 ulActionId, HUINT32 *pulSvcNum, DbSvc_Info_t **ppstSvcArray, HUINT32 *pulPrvNum, DbSvc_PrvInfo_t **ppstPrvArray);
extern HERROR	psvc_si_GetSvcAndVideoTypeFromDvbSvcType(HUINT32 ulActionId, HUINT32 ulDvbSvcType, DxSvcType_e *peSvcType, DxVideoType_e *peVideoType);
extern HERROR	psvc_si_UpdateTimeByTdt(HUINT32 ulActionId, Handle_t hService, SIxTable_PsiTable_t *pstTdtTable);
extern HERROR	psvc_si_UpdateTimeByTot(HUINT32 ulActionId);
extern HERROR psvc_si_ComposeServiceInfoBySdt(HUINT32 ulActionId, SIxTable_PsiTable_t *pstSdtTable, HUINT32 *pulSvcNum, DbSvc_Info_t **ppstSvcArray, HUINT32 *pulPrvNum, DbSvc_PrvInfo_t **ppstPrvArray);
extern HERROR 	psvc_si_GetSwupCheckFlag(Handle_t hAction, HUINT32 *pulSwupCheckFlag);
extern HERROR 	psvc_si_GetAudioComponent (HUINT32 ulActionId, SvcSi_AudCpnt_e *peAudCpnt);

extern HERROR	psvc_si_GetUndefinedServiceName(HUINT32 ulActionId, HUINT8 **ppszServiceName);
extern HERROR	psvc_si_GetUndefinedProviderName(HUINT32 ulActionId, HUINT8 **ppszProviderName);
extern HERROR	psvc_si_GetUndefinedNetworkName(HUINT32 ulActionId, HUINT8 **ppszNetworkName);

extern HERROR 	psvc_si_SetLcnServiceNumber(Handle_t hAction, HUINT32 ulActSpec, HUINT32 ulNum, DbSvc_Info_t *pstSvcArray, SRCHSVC_AppendFlag_t *peFlagArray);
extern HERROR psvc_si_SetSelectedConflictLcn(Handle_t hAction, HUINT32 ulNum, DxBuilder_SearchedInfo_t *pstInfoArray);

extern HERROR 	psvc_si_PostProcessForSvcNumbering (Handle_t hAction, DefaultLcnPolicy_t eLcnPolicy, DxDeliveryType_e eDeliType);
extern HERROR	psvc_si_PostProcessForSimulcastService (Handle_t hAction);

extern HERROR 	psvc_si_SetSwupCheckParam(SvcSi_SwupCheckSpec_t *pstSwupSpec, SvcSi_SwupFlag_t *pstSwupCheckParam);
extern HERROR 	psvc_si_GetSwupCheckData(Handle_t hAction, SvcSi_SwupCheckSpec_t *pstSwupSpec, HUINT32 ulDataType, HUINT32 *pulDataNum, void **ppvDataArray);
extern HERROR 	psvc_si_FreeSwupCheckData(Handle_t hAction, SvcSi_SwupCheckSpec_t *pstSwupCheckSpec, HUINT32 ulDataType, void *pvDataArray);
extern HERROR 	psvc_si_FindProperOuiDataFromLinkDes(SvcSi_SwupCheckSpec_t *pstSwupCheckSpec,SIxLinkDes_t *pstLinkDes);

extern HERROR 	psvc_si_SetExtSearchOption(Handle_t hAction, SvcSi_ExtSearchSpec_t *pstExtSrchSpec, SvcSi_ExtSrchOptionType_t eOptionType, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
extern HERROR 	psvc_si_GetExtSearchData(Handle_t hAction, SvcSi_ExtSearchSpec_t *pstExtSrchSpec, SvcSi_ExtSrchDataType_t eDataType, HUINT32 *pulDataNum, void **ppvDataArray);
extern HERROR 	psvc_si_FreeExtSearchData(Handle_t hAction, SvcSi_ExtSearchSpec_t *pstExtSrchSpec, SvcSi_ExtSrchDataType_t eDataType, void *pvDataArray);

extern HERROR 	psvc_si_GetSatRecordData(Handle_t hAction, SvcSi_SatRecordSpec_t *pstSatRecordSpec, SvcSi_SatRecordDataType_t eDataType, HUINT32 *pulDataNum, void **ppvDataArray);
extern HERROR 	psvc_si_FreeSatRecordData(Handle_t hAction, SvcSi_SatRecordSpec_t *pstSatRecordSpec, SvcSi_SatRecordDataType_t eDataType, void *pvDataArray);

extern HERROR	psvc_si_MakeSiSection (HUINT32 ulActionId, SvcSi_MakeSectionSpec_t *pstSpec, SvcSi_MakeSecArg_t *punArg, HUINT8 *pucRawSec, HUINT32 *pulSecLen);

extern HBOOL		psvc_si_IsValidOui(HUINT32 ulOui);
extern HERROR	psvc_si_GetAlternativeAudioPriority(const svcSi_AudioEs_t *pstAudioEs, HBOOL bLangMatched, HINT32 *pnUserPriority, HxLANG_Id_e *peLangId, DxDualMonoSelect_e *peDualMonoSelect);

extern HERROR	psvc_si_GetTrdConflictedList(Handle_t hAction,HUINT32 ulDepth, HUINT32 *pulconflictcount,HxList_t **ppConflictList);
extern HERROR	psvc_si_SetTrdConflictedList(HUINT32 ulDepthIdx, HUINT32 index);
extern HERROR	psvc_si_InitTrdConflicteSvcHandleTable(void);
extern HERROR	psvc_si_SetNcdNotifyData(HUINT32 ulASONotifyFlag);
extern HERROR	psvc_si_ClearNcdNotifyData(void);

#if defined(CONFIG_MW_SI_LOCALTIME)
extern HERROR 	psvc_si_UpdateLocaltime_linkTzDB(HxCountry_e eCountryId, HINT32 nTimeRegion);
#endif

#if defined(CONFIG_DEBUG)
extern Cutest_Suite_t *psvc_si_UtGetSuites(void);
#endif


#endif	// ___PSVC_SI_LIVE_H___
/* end of file */
