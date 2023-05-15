/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _xsvc_si_lib_service_base.h
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
* MS949 encoding �� ����ϴ� source file �Դϴ�.
* MS949 encoding�����Ī��� source file�Ǫ���
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

#ifndef ___INT_XSVC_SI_LIB_SERVICE_BASE_H___
#define ___INT_XSVC_SI_LIB_SERVICE_BASE_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

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

#if defined(CONFIG_MW_SI_LOCALTIME)
#if defined(CONFIG_MW_SI_TIMEZONEDB_BYTOT)
HERROR xsvc_si_LibSvcEsUpdateTime_localtimeByTot(HCHAR *szTzDstString, UNIXTIME ulNextTimeOfChange, HINT32 nNowLocalTimeOffset, HINT32 nNextLocalTimeOffset);
#endif
HERROR xsvc_si_LibSvcEsUpdateLocaltime_linkTzDB(HxCountry_e eCountryId, HINT32 nTimeRegion);
#endif

extern HERROR 	xsvc_si_LibSvcEsComposeServiceInfoBySdt_Base (HUINT32 ulActionId, SIxTable_PsiTable_t *pstSdtTable, HUINT32 *pulSvcNum, DbSvc_Info_t **ppstSvcArray, HUINT32 *pulPrvNum, DbSvc_PrvInfo_t **ppstPrvArray);
extern HERROR	xsvc_si_LibSvcEsComposeServiceInfoByPmt_Base (HUINT32 ulActionId, SIxTable_PsiTable_t *pstPmtTable, HUINT32 *pulSvcNum, DbSvc_Info_t **ppstSvcArray, HUINT32 *pulPrvNum, DbSvc_PrvInfo_t **ppstPrvArray);

extern HERROR	xsvc_si_LibSvcEsGetPmtEsInfo_Base (HUINT32 ulActionId, SIxTable_PsiTable_t *pstSiTable, svcSi_PmtEsInfo_t *pstPmtEsInfo);
extern HERROR	xsvc_si_LibSvcEsSelectVideo_Base (HUINT32 ulActionId, svcSi_PmtEsInfo_t *pstPmtEsInfo, DbSvc_Info_t *pstSvcInfo, svc_si_SelectedEs_t *pstSelectedVideo);
extern HERROR	xsvc_si_LibSvcEsSelectAudio_Base (HUINT32 ulActionId, svcSi_PmtEsInfo_t *pstPmtEsInfo, DbSvc_Info_t *pstSvcInfo, svc_si_SelectedAudio_t *pstSelectedAudio);
extern HERROR	xsvc_si_LibSvcEsSelectSubtitle_Base (HUINT32 ulActionId, svcSi_PmtEsInfo_t *pstPmtEsInfo, DbSvc_Info_t *pstSvcInfo, svc_si_SelectedEs_t *pstSelectedSubttl);
extern HERROR	xsvc_si_LibSvcEsSelectTeletext_Base (HUINT32 ulActionId, svcSi_PmtEsInfo_t *pstPmtEsInfo, DbSvc_Info_t *pstSvcInfo, svc_si_SelectedEs_t *pstSelectedTtx);

extern HERROR	xsvc_si_LibSvcEsComposeServiceInfoInTp_Base (HUINT32 ulActionId, HUINT32 *pulSvcNum, DbSvc_Info_t **ppstSvcArray, HUINT32 *pulPrvNum, DbSvc_PrvInfo_t **ppstPrvArray);
extern HERROR	xsvc_si_LibSvcEsGetSvcAndVideoTypeFromDvbSvcType_Base (HUINT32 ulActionId, HUINT32 ulDvbSvcType, DxSvcType_e *peSvcType, DxVideoType_e *peVideoType);

extern HERROR	xsvc_si_LibSvcEsUpdateTimeByTdt_Base (HUINT32 ulActionId, SIxTable_PsiTable_t *pstTdtTable);
extern HERROR	xsvc_si_LibSvcEsUpdateTimeByTot_Base (HUINT32 ulActionId);


#endif
/* end of file */