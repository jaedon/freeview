/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _svc_si_filtering.h
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
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
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

#ifndef ___INT_SVC_SI_FILTERING_H___
#define ___INT_SVC_SI_FILTERING_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <pal_sef.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	bSI_FILTERING_PAT				= 0x00000001,
	bSI_FILTERING_PMT				= 0x00000002,
	bSI_FILTERING_CAT				= 0x00000004,
	bSI_FILTERING_BAT				= 0x00000008,
	bSI_FILTERING_SDT_ACT			= 0x00000010,
	bSI_FILTERING_SDT_OTH			= 0x00000020,
	bSI_FILTERING_NIT_ACT			= 0x00000040,
	bSI_FILTERING_NIT_OTH			= 0x00000080,
	bSI_FILTERING_EIT_ACT_PF		= 0x00000100,
	bSI_FILTERING_EIT_OTH_PF		= 0x00000200,
	bSI_FILTERING_EIT_ACT_SCH		= 0x00000400,
	bSI_FILTERING_EIT_OTH_SCH		= 0x00000800,
	bSI_FILTERING_TDT				= 0x00001000,
	bSI_FILTERING_TOT				= 0x00002000,
	bSI_FILTERING_DSI				= 0x00004000,
	bSI_FILTERING_DIT				= 0x00008000,
	bSI_FILTERING_DDB				= 0x00010000,
	bSI_FILTERING_AIT				= 0x00020000,
	bSI_FILTERING_PMT_N				= 0x00040000,
	bSI_FILTERING_BIT				= 0x00080000,
	bSI_FILTERING_EIT_OTH_PFEXT		= 0x00100000,
	bSI_FILTERING_EIT_OTH_SCHEXT	= 0x00200000,
	bSI_FILTERING_EIT_CC			= 0x00400000,
	bSI_FILTERING_SGT				= 0x00800000,	/* for SESsat, Service Guide Table */
	bSI_FILTERING_UNT				= 0x01000000,	/* SSU UNT */
	bSI_FILTERING_RCT				= 0x02000000,

#if 0 // should be removed..
	bSI_FILTERING_POSTCODE			= 0x01000000,
	bSI_FILTERING_RPD				= 0x20000000, 	/* for TNTsat, Regionalization Pool Descriptor Section Table */
#endif

	bSI_FILTERING_END				= 0xFFFFFFFF
} svcSi_FilteringType_b;

typedef enum
{
	eSI_TABLE_FILTER_MODE_Once,
	eSI_TABLE_FILTER_MODE_Monitor,
	eSI_TABLE_FILTER_MODE_All,			//< eSiFilterMode_Continue 에 대응
	eSI_TABLE_FILTER_MODE_AllMonitor,			//< eSiFilterMode_Continue 에 대응

	eSI_TABLE_FILTER_MODE_None
} SvcSi_TableFilterMode_e;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      API Functions     ********************/
/*******************************************************************/
// svc_si_filtering.c
extern HERROR			svc_si_FilterInit (void);
extern HUINT32			svc_si_FilterGetTimeout (HUINT32 ulActionId, SIxTable_Type_e eTable);
extern HERROR			svc_si_FilterSetTimeout (HUINT32 ulActionId, SIxTable_Type_e eTable, HUINT32 ulTimeout);
extern HERROR			svc_si_FilterResetTimeout (HUINT32 ulActionId, SIxTable_Type_e eTable);

extern HERROR			svc_si_FilterStartTableSection (Handle_t hAction, HUINT32 ulDemuxId, SIxTable_Type_e eTable, HUINT32 ulExtId, HUINT32 ulPid, SvcSi_TableFilterMode_e eFilterMode, HUINT32 *pulFilterId);
extern HERROR			svc_si_FilterStopTableSection (Handle_t hAction, SIxTable_Type_e eTable, HUINT32 ulExtId);
extern HERROR 			svc_si_FilterGetPalFilterMode (HUINT32 ulActionId, SIxTable_Type_e eTable, HUINT16 usFilterId, PalSef_FilterMode_e *peFilterMode);
extern HERROR			svc_si_FilterGetSiFilterMode (HUINT32 ulActionId, SIxTable_Type_e eTable, HUINT16 usFilterId, SvcSi_TableFilterMode_e *peFilterMode);
extern HERROR			svc_si_FilterGetFilterMonitorMode (HUINT32 ulActionId, SIxTable_Type_e eTable, HUINT16 usFilterId, HBOOL *pbMonitor);
extern HERROR			svc_si_FilterSetFilterMonitorMode (HUINT32 ulActionId, SIxTable_Type_e eTable, HUINT16 usFilterId, HBOOL bMonitor);
extern HERROR 			svc_si_FilterGetFilterPid (HUINT32 ulActionId, SIxTable_Type_e eTable, HUINT16 usFilterId, HUINT16 *pusPid);
extern HERROR			svc_si_FilterGetFilterSameCount (HUINT32 ulActionId, SIxTable_Type_e eTable, HUINT16 usFilterId, HUINT32 *pulSameCount);
extern HERROR			svc_si_FilterResetFilterSameCount (HUINT32 ulActionId, SIxTable_Type_e eTable, HUINT16 usFilterId);
extern HERROR			svc_si_FilterIncreaseFilterSameCount (HUINT32 ulActionId, SIxTable_Type_e eTable, HUINT16 usFilterId, HUINT32 *pulSameCount);

#if defined(CONFIG_DEBUG)
extern HUINT32 			svc_si_FilterPrintStatus(void);
#endif

// svc_si_cb.c
extern HERROR				svc_si_CbInit (void);
extern PalSef_Callback_t	svc_si_CbGetTableParseFunc (HUINT32 ulActionId, SIxTable_Type_e eTable);
extern PalSef_Callback_t	svc_si_CbGetTableFreeFunc (HUINT32 ulActionId, SIxTable_Type_e eTable);
extern HERROR				svc_si_CbRegisterCustomCallback (HUINT32 ulActionId, SIxTable_Type_e eTable, PalSef_Callback_t pfParse, PalSef_Callback_t pfFree, HCHAR *szParseName, HCHAR *szFreeName);
extern HERROR				svc_si_CbUnregisterCustomCallback (HUINT32 ulActionId, SIxTable_Type_e eTable);

#endif	// ___INT_SVC_SI_FILTERING_H___
/* end of file */

