/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  psvc_si.c
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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <pal_pipe.h>
#include <pal_audio.h>

#include <isosvc.h>
#include <svc_si.h>
#include <_svc_si.h>
#include <_svc_si_lib.h>
#include <psvc_si.h>

#include "_xsvc_si_time.h"
#include "_xsvc_si.h"
#include "_xsvc_si_lib_service_base.h"
#include "_xsvc_si_lib_lcn_base.h"
#include "_xsvc_si_lib_makesec_base.h"

#include "_xsvc_si_search.h"
#include "_xsvc_si_live.h"
#include "_xsvc_si_lcn.h"
#include "_xsvc_si_mediapb.h"
#include "_xsvc_si_hbbtv.h"

#if defined(CONFIG_OP_SES)
#include "_xsvc_si_lcnupdate_ses.h"
#include "_xsvc_si_satrecord_ses.h"
#endif

#if defined(CONFIG_OP_AUS_DTT)
#include "_xsvc_si_dynamicupdate_ausdtt.h"
#endif
#if defined(CONFIG_OP_UK_DTT)
#include "_xsvc_si_dynamicupdate_ukdtt.h"
#include "_xsvc_si_lib_service_ukdtt.h"
#include "_xsvc_si_spec_ukdtt.h"
#endif

#if defined(CONFIG_OP_NORDIG)
#include "_xsvc_si_dynamicupdate_nordig.h"
#include "_xsvc_si_lib_service_nordig.h"
#endif

#ifndef CONFIG_OTA_NONE
#include "_svc_si_swup.h"
#include "_xsvc_si_swup_base.h"
#include "_xsvc_si_swup.h"
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
/********************      Static Variables     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___

STATIC SvcSi_ActFunc psvc_si_GetLiveActionFunc(XsvcSi_ActionSpec_e eActSpec)
{
	switch (eActSpec)
	{
#if defined(CONFIG_MW_CH_SATELLITE)
	case eSVCSI_ACT_SPEC_EuSat:
		return xsvc_si_LiveGetLiveActionFunc_Eusat();
#endif

#if defined(CONFIG_MW_CH_TERRESTRIAL)
	case eSVCSI_ACT_SPEC_EuTer:
		return xsvc_si_LiveGetLiveActionFunc_Eudtt();
#endif

#if defined(CONFIG_MW_CH_CABLE)
	case eSVCSI_ACT_SPEC_EuCab:
		return xsvc_si_LiveGetLiveActionFunc_Eucab();
#endif

#if defined(CONFIG_OP_ERTELECOM)
	case eSVCSI_ACT_SPEC_Ertelecom:
		return xsvc_si_LiveGetLiveActionFunc_Ertelecom();
#endif

#if defined(CONFIG_OP_NORDIG)
	case eSVCSI_ACT_SPEC_Nordig:
		return xsvc_si_LiveGetLiveActionFunc_Nordig();
#endif

#if defined(CONFIG_OP_MIDDLE_EAST)
	case eSVCSI_ACT_SPEC_MeSat:
		return xsvc_si_LiveGetLiveActionFunc_MeSat();
#endif

#if defined(CONFIG_OP_SES)
	case eSVCSI_ACT_SPEC_Ses:
		return xsvc_si_LiveGetLiveActionFunc_Ses();
#endif

#if defined(CONFIG_OP_AUS_DTT)
	case eSVCSI_ACT_SPEC_AusDtt:
		return xsvc_si_LiveGetLiveActionFunc_AusDtt();
#endif

#if defined(CONFIG_OP_FREESAT)
	case eSVCSI_ACT_SPEC_Freesat:
		return xsvc_si_LiveGetLiveActionFunc_UkFsat();
#endif

#if defined(CONFIG_OP_UK_DTT)
	case eSVCSI_ACT_SPEC_UkDtt:
		return xsvc_si_LiveGetLiveActionFunc_UkDtt();
#endif

#if defined(CONFIG_OP_ZIGGO)
	case eSVCSI_ACT_SPEC_Ziggo:
		return xsvc_si_LiveGetLiveActionFunc_Ziggo();
#endif

#if defined(CONFIG_OP_ITALY_DBOOK)
	case eSVCSI_ACT_SPEC_ItaDtt:
		return xsvc_si_LiveGetLiveActionFunc_ItaDtt();
#endif

#if defined(CONFIG_OP_ORF)
	case eSVCSI_ACT_SPEC_Orf:
		return xsvc_si_LiveGetLiveActionFunc_HdFoxIrConnect();
#endif

#if defined(CONFIG_OP_MBC)
	case eSVCSI_ACT_SPEC_Mbc:
		return xsvc_si_LiveGetLiveActionFunc_Mbc();
#endif

#if defined(CONFIG_OP_AMX)
	case eSVCSI_ACT_SPEC_AmxCenam:
		return xsvc_si_LiveGetLiveActionFunc_Amx_Cenam();
#endif

	default:
		break;
	}

	return NULL;
}

STATIC void *psvc_si_AllocLiveActionData(XsvcSi_ActionSpec_e eActSpec)
{
	switch (eActSpec)
	{
#if defined(CONFIG_MW_CH_SATELLITE)
	case eSVCSI_ACT_SPEC_EuSat:
		return xsvc_si_LiveAllocLiveActionData_Eusat();
#endif

#if defined(CONFIG_MW_CH_TERRESTRIAL)
	case eSVCSI_ACT_SPEC_EuTer:
		return xsvc_si_LiveAllocLiveActionData_Eudtt();
#endif

#if defined(CONFIG_MW_CH_CABLE)
	case eSVCSI_ACT_SPEC_EuCab:
		return xsvc_si_LiveAllocLiveActionData_Eucab();
#endif

#if defined(CONFIG_OP_ERTELECOM)
	case eSVCSI_ACT_SPEC_Ertelecom:
		return xsvc_si_LiveAllocLiveActionData_Ertelecom();
#endif

#if defined(CONFIG_OP_NORDIG)
	case eSVCSI_ACT_SPEC_Nordig:
		return xsvc_si_LiveAllocLiveActionData_Nordig();
#endif

#if defined(CONFIG_OP_MIDDLE_EAST)
	case eSVCSI_ACT_SPEC_MeSat:
		return xsvc_si_LiveAllocLiveActionData_MeSat();
#endif

#if defined(CONFIG_OP_SES)
	case eSVCSI_ACT_SPEC_Ses:
		return xsvc_si_LiveAllocLiveActionData_Ses();
#endif

#if defined(CONFIG_OP_AUS_DTT)
	case eSVCSI_ACT_SPEC_AusDtt:
		return xsvc_si_LiveAllocLiveActionData_AusDtt();
#endif

#if defined(CONFIG_OP_FREESAT)
	case eSVCSI_ACT_SPEC_Freesat:
		return xsvc_si_LiveAllocLiveActionData_UkFsat();
#endif

#if defined(CONFIG_OP_UK_DTT)
	case eSVCSI_ACT_SPEC_UkDtt:
		return xsvc_si_LiveAllocLiveActionData_UkDtt();
#endif

#if defined(CONFIG_OP_ZIGGO)
	case eSVCSI_ACT_SPEC_Ziggo:
		return xsvc_si_LiveAllocLiveActionData_Ziggo();
#endif

#if defined(CONFIG_OP_ITALY_DBOOK)
	case eSVCSI_ACT_SPEC_ItaDtt:
		return xsvc_si_LiveAllocLiveActionData_ItaDtt();
#endif

#if defined(CONFIG_OP_ORF)
	case eSVCSI_ACT_SPEC_Orf:
		return xsvc_si_LiveAllocLiveActionData_HdFoxIrConnect();
#endif

#if defined(CONFIG_OP_MBC)
	case eSVCSI_ACT_SPEC_Mbc:
		return xsvc_si_LiveAllocLiveActionData_Mbc();
#endif

#if defined(CONFIG_OP_AMX)
	case eSVCSI_ACT_SPEC_AmxCenam:
		return xsvc_si_LiveAllocLiveActionData_Amx_Cenam();
#endif

	default:
		break;
	}

	return NULL;
}

STATIC void psvc_si_FreeLiveActionData(XsvcSi_ActionSpec_e eActSpec, void *pvData)
{
	switch (eActSpec)
	{
#if defined(CONFIG_MW_CH_SATELLITE)
	case eSVCSI_ACT_SPEC_EuSat:
		xsvc_si_LiveFreeLiveActionData_Eusat(pvData);
		return;
#endif

#if defined(CONFIG_MW_CH_TERRESTRIAL)
	case eSVCSI_ACT_SPEC_EuTer:
		xsvc_si_LiveFreeLiveActionData_Eudtt(pvData);
		return;
#endif

#if defined(CONFIG_MW_CH_CABLE)
	case eSVCSI_ACT_SPEC_EuCab:
		xsvc_si_LiveFreeLiveActionData_Eucab(pvData);
		return;
#endif

#if defined(CONFIG_OP_ERTELECOM)
	case eSVCSI_ACT_SPEC_Ertelecom:
		xsvc_si_LiveFreeLiveActionData_Ertelecom(pvData);
		return;
#endif

#if defined(CONFIG_OP_NORDIG)
	case eSVCSI_ACT_SPEC_Nordig:
		xsvc_si_LiveFreeLiveActionData_Nordig(pvData);
		return;
#endif

#if defined(CONFIG_OP_MIDDLE_EAST)
	case eSVCSI_ACT_SPEC_MeSat:
		xsvc_si_LiveFreeLiveActionData_MeSat(pvData);
		return;
#endif

#if defined(CONFIG_OP_SES)
	case eSVCSI_ACT_SPEC_Ses:
		xsvc_si_LiveFreeLiveActionData_Ses(pvData);
		return;
#endif

#if defined(CONFIG_OP_AUS_DTT)
	case eSVCSI_ACT_SPEC_AusDtt:
		xsvc_si_LiveFreeLiveActionData_AusDtt(pvData);
		return;
#endif

#if defined(CONFIG_OP_FREESAT)
	case eSVCSI_ACT_SPEC_Freesat:
		xsvc_si_LiveFreeLiveActionData_UkFsat(pvData);
		return;
#endif

#if defined(CONFIG_OP_UK_DTT)
	case eSVCSI_ACT_SPEC_UkDtt:
		xsvc_si_LiveFreeLiveActionData_UkDtt(pvData);
		return;
#endif

#if defined(CONFIG_OP_ZIGGO)
	case eSVCSI_ACT_SPEC_Ziggo:
		xsvc_si_LiveFreeLiveActionData_Ziggo(pvData);
		return;
#endif

#if defined(CONFIG_OP_ITALY_DBOOK)
	case eSVCSI_ACT_SPEC_ItaDtt:
		xsvc_si_LiveFreeLiveActionData_ItaDtt(pvData);
		return;
#endif

#if defined(CONFIG_OP_ORF)
	case eSVCSI_ACT_SPEC_Orf:
		xsvc_si_LiveFreeLiveActionData_HdFoxIrConnect(pvData);
		return;
#endif

#if defined(CONFIG_OP_MBC)
	case eSVCSI_ACT_SPEC_Mbc:
		xsvc_si_LiveFreeLiveActionData_Mbc(pvData);
		return;
#endif

#if defined(CONFIG_OP_AMX)
	case eSVCSI_ACT_SPEC_AmxCenam:
		xsvc_si_LiveFreeLiveActionData_Amx_Cenam(pvData);
		return;
#endif

	default:
		break;
	}

	return;
}

STATIC SvcSi_ActFunc psvc_si_GetSearchActionFunc(XsvcSi_ActionSpec_e eActSpec)
{
	switch (eActSpec)
	{
#if defined(CONFIG_MW_CH_SATELLITE)
	case eSVCSI_ACT_SPEC_EuSat:
		return xsvc_si_SrchGetSearchActionFunc_Eusat();
#endif

#if defined(CONFIG_MW_CH_TERRESTRIAL)
	case eSVCSI_ACT_SPEC_EuTer:
		return xsvc_si_SrchGetSearchActionFunc_Eudtt();
#endif

#if defined(CONFIG_MW_CH_CABLE)
	case eSVCSI_ACT_SPEC_EuCab:
		return xsvc_si_SrchGetSearchActionFunc_Eucab();
#endif

#if defined(CONFIG_OP_ERTELECOM)
	case eSVCSI_ACT_SPEC_Ertelecom:
		return xsvc_si_SrchGetSearchActionFunc_Ertelecom();
#endif

#if defined(CONFIG_OP_NORDIG)
	case eSVCSI_ACT_SPEC_Nordig:
		return xsvc_si_SrchGetSearchActionFunc_Nordig();
#endif

#if defined(CONFIG_OP_MIDDLE_EAST)
	case eSVCSI_ACT_SPEC_MeSat:
		return xsvc_si_SrchGetSearchActionFunc_MeSat();
#endif

#if defined(CONFIG_OP_SES)
	case eSVCSI_ACT_SPEC_Ses:
		return xsvc_si_SrchGetSearchActionFunc_Ses();
#endif

#if defined(CONFIG_OP_AUS_DTT)
	case eSVCSI_ACT_SPEC_AusDtt:
		return xsvc_si_SrchGetSearchActionFunc_AusDtt();
#endif

#if defined(CONFIG_OP_UK_DTT)
	case eSVCSI_ACT_SPEC_UkDtt:
		return xsvc_si_SrchGetSearchActionFunc_UkDtt();
#endif

#if defined(CONFIG_OP_ZIGGO)
	case eSVCSI_ACT_SPEC_Ziggo:
		return xsvc_si_SrchGetSearchActionFunc_Ziggo();
#endif

#if defined(CONFIG_OP_ITALY_DBOOK)
	case eSVCSI_ACT_SPEC_ItaDtt:
		return xsvc_si_SrchGetSearchActionFunc_ItaDtt();
#endif

#if defined(CONFIG_OP_ORF)
	case eSVCSI_ACT_SPEC_Orf:
		return xsvc_si_SrchGetSearchActionFunc_HdFoxIrConnect();
#endif

#if defined(CONFIG_OP_MBC)
	case eSVCSI_ACT_SPEC_Mbc:
		return xsvc_si_SrchGetSearchActionFunc_Mbc();
#endif

#if defined(CONFIG_OP_AMX)
	case eSVCSI_ACT_SPEC_AmxCenam:
		return xsvc_si_SrchGetSearchActionFunc_Amx_Cenam();
#endif

	default:
		break;
	}

	return NULL;
}

STATIC void *psvc_si_AllocSearchActionData(XsvcSi_ActionSpec_e eActSpec)
{
	switch (eActSpec)
	{
#if defined(CONFIG_MW_CH_SATELLITE)
	case eSVCSI_ACT_SPEC_EuSat:
		return xsvc_si_SrchAllocSearchActionData_Eusat();
#endif

#if defined(CONFIG_MW_CH_TERRESTRIAL)
	case eSVCSI_ACT_SPEC_EuTer:
		return xsvc_si_SrchAllocSearchActionData_Eudtt();
#endif

#if defined(CONFIG_MW_CH_CABLE)
	case eSVCSI_ACT_SPEC_EuCab:
		return xsvc_si_SrchAllocSearchActionData_Eucab();
#endif

#if defined(CONFIG_OP_ERTELECOM)
	case eSVCSI_ACT_SPEC_Ertelecom:
		return xsvc_si_SrchAllocSearchActionData_Ertelecom();
#endif

#if defined(CONFIG_OP_NORDIG)
	case eSVCSI_ACT_SPEC_Nordig:
		return xsvc_si_SrchAllocSearchActionData_Nordig();
#endif

#if defined(CONFIG_OP_MIDDLE_EAST)
	case eSVCSI_ACT_SPEC_MeSat:
		return xsvc_si_SrchAllocSearchActionData_MeSat();
#endif

#if defined(CONFIG_OP_SES)
	case eSVCSI_ACT_SPEC_Ses:
		return xsvc_si_SrchAllocSearchActionData_Ses();
#endif

#if defined(CONFIG_OP_AUS_DTT)
	case eSVCSI_ACT_SPEC_AusDtt:
		return xsvc_si_SrchAllocSearchActionData_AusDtt();
#endif

#if defined(CONFIG_OP_UK_DTT)
	case eSVCSI_ACT_SPEC_UkDtt:
		return xsvc_si_SrchAllocSearchActionData_UkDtt();
#endif

#if defined(CONFIG_OP_ZIGGO)
	case eSVCSI_ACT_SPEC_Ziggo:
		return xsvc_si_SrchAllocSearchActionData_Ziggo();
#endif

#if defined(CONFIG_OP_ITALY_DBOOK)
	case eSVCSI_ACT_SPEC_ItaDtt:
		return xsvc_si_SrchAllocSearchActionData_ItaDtt();
#endif

#if defined(CONFIG_OP_ORF)
	case eSVCSI_ACT_SPEC_Orf:
		return xsvc_si_SrchAllocSearchActionData_HdFoxIrConnect();
#endif

#if defined(CONFIG_OP_MBC)
	case eSVCSI_ACT_SPEC_Mbc:
		return xsvc_si_SrchAllocSearchActionData_Mbc();
#endif

#if defined(CONFIG_OP_AMX)
	case eSVCSI_ACT_SPEC_AmxCenam:
		return xsvc_si_SrchAllocSearchActionData_Amx_Cenam();
#endif

	default:
		break;
	}

	return NULL;
}

STATIC void psvc_si_FreeSearchActionData(XsvcSi_ActionSpec_e eActSpec, void *pvData)
{
	switch (eActSpec)
	{
#if defined(CONFIG_MW_CH_SATELLITE)
	case eSVCSI_ACT_SPEC_EuSat:
		xsvc_si_SrchFreeSearchActionData_Eusat(pvData);
		return;
#endif

#if defined(CONFIG_MW_CH_TERRESTRIAL)
	case eSVCSI_ACT_SPEC_EuTer:
		xsvc_si_SrchFreeSearchActionData_Eudtt(pvData);
		return;
#endif

#if defined(CONFIG_MW_CH_CABLE)
	case eSVCSI_ACT_SPEC_EuCab:
		xsvc_si_SrchFreeSearchActionData_Eucab();
		return;
#endif

#if defined(CONFIG_OP_ERTELECOM)
	case eSVCSI_ACT_SPEC_Ertelecom:
		xsvc_si_SrchFreeSearchActionData_Ertelecom(pvData);
		return;
#endif

#if defined(CONFIG_OP_NORDIG)
	case eSVCSI_ACT_SPEC_Nordig:
		xsvc_si_SrchFreeSearchActionData_Nordig(pvData);
		return;
#endif

#if defined(CONFIG_OP_MIDDLE_EAST)
	case eSVCSI_ACT_SPEC_MeSat:
		xsvc_si_SrchFreeSearchActionData_MeSat(pvData);
		return;
#endif

#if defined(CONFIG_OP_SES)
	case eSVCSI_ACT_SPEC_Ses:
		xsvc_si_SrchFreeSearchActionData_Ses(pvData);
		return;
#endif

#if defined(CONFIG_OP_AUS_DTT)
	case eSVCSI_ACT_SPEC_AusDtt:
		xsvc_si_SrchFreeSearchActionData_AusDtt(pvData);
		return;
#endif

#if defined(CONFIG_OP_UK_DTT)
	case eSVCSI_ACT_SPEC_UkDtt:
		xsvc_si_SrchFreeSearchActionData_UkDtt(pvData);
		return;
#endif

#if defined(CONFIG_OP_ZIGGO)
	case eSVCSI_ACT_SPEC_Ziggo:
		xsvc_si_SrchFreeSearchActionData_Ziggo(pvData);
		return;
#endif

#if defined(CONFIG_OP_ITALY_DBOOK)
	case eSVCSI_ACT_SPEC_ItaDtt:
		xsvc_si_SrchFreeSearchActionData_ItaDtt(pvData);
		return;
#endif

#if defined(CONFIG_OP_ORF)
	case eSVCSI_ACT_SPEC_Orf:
		xsvc_si_SrchFreeSearchActionData_HdFoxIrConnect(pvData);
		return;
#endif

#if defined(CONFIG_OP_MBC)
	case eSVCSI_ACT_SPEC_Mbc:
		xsvc_si_SrchFreeSearchActionData_Mbc(pvData);
		return;
#endif

#if defined(CONFIG_OP_AMX)
	case eSVCSI_ACT_SPEC_AmxCenam:
		xsvc_si_SrchFreeSearchActionData_Amx_Cenam(pvData);
		return;
#endif

	default:
		break;
	}

	return;
}

STATIC SvcSi_ActFunc psvc_si_GetExtSearchActionFunc(XsvcSi_ActionSpec_e eActSpec)
{
	switch (eActSpec)
	{
#if defined(CONFIG_OP_SES)
	case eSVCSI_ACT_SPEC_Ses:
		return xsvc_si_LcnUpdateSrchGetSearchActionFunc_Ses();
#endif

	default:
		break;
	}

	return NULL;
}

STATIC void *psvc_si_AllocExtSearchActionData(XsvcSi_ActionSpec_e eActSpec)
{
	switch (eActSpec)
	{
#if defined(CONFIG_OP_SES)
	case eSVCSI_ACT_SPEC_Ses:
		return xsvc_si_LcnUpdateSrchAllocSearchActionData_Ses();
#endif

	default:
		break;
	}

	return NULL;
}

STATIC void psvc_si_FreeExtSearchActionData(XsvcSi_ActionSpec_e eActSpec, void *pvData)
{
	switch (eActSpec)
	{
#if defined(CONFIG_OP_SES)
	case eSVCSI_ACT_SPEC_Ses:
		return xsvc_si_LcnUpdateSrchFreeSearchActionData_Ses(pvData);
#endif

	default:
		break;
	}

	return;
}

STATIC SvcSi_ActFunc psvc_si_GetSatRecordActionFunc(XsvcSi_ActionSpec_e eActSpec)
{
	switch (eActSpec)
	{
#if defined(CONFIG_OP_SES)
	case eSVCSI_ACT_SPEC_Ses:
		return xsvc_si_SatRecordGetSearchActionFunc_Ses();
#endif

	default:
		break;
	}

	return NULL;
}

STATIC void *psvc_si_AllocSatRecordActionData(XsvcSi_ActionSpec_e eActSpec)
{
	switch (eActSpec)
	{
#if defined(CONFIG_OP_SES)
	case eSVCSI_ACT_SPEC_Ses:
		return xsvc_si_SatRecordAllocSearchActionData_Ses();
#endif

	default:
		break;
	}

	return NULL;
}

STATIC void psvc_si_FreeSatRecordActionData(XsvcSi_ActionSpec_e eActSpec, void *pvData)
{
	switch (eActSpec)
	{
#if defined(CONFIG_OP_SES)
	case eSVCSI_ACT_SPEC_Ses:
		return xsvc_si_SatRecordFreeSearchActionData_Ses(pvData);
#endif

	default:
		break;
	}

	return;
}


STATIC SvcSi_ActFunc psvc_si_GetSwupCheckActionFunction(XsvcSi_ActionSpec_e eActSpec)
{
	switch (eActSpec)
	{
#if defined(CONFIG_OTA_HUMAX)
	case eSVCSI_ACT_SPEC_Humax:
		return xsvc_si_SwupCheckGetActionFunc_Humax();
#endif

#if defined(CONFIG_OTA_IRDETO)
	case eSVCSI_ACT_SPEC_Irdeto:
		return xsvc_si_SwupCheckGetActionFunc_Irdeto();
#endif

#if defined(CONFIG_OTA_SES)
	case eSVCSI_ACT_SPEC_Ses:
		return xsvc_si_SwupCheckGetActionFunc_Ses();
#endif

#if defined(CONFIG_OTA_ASTRA)
	case eSVCSI_ACT_SPEC_Astra:
		return xsvc_si_SwupCheckGetActionFunc_Astra();
#endif

#if defined(CONFIG_OTA_DVBSSU)
	case eSVCSI_ACT_SPEC_DvbSsu:
		return xsvc_si_SwupCheckGetActionFunc_Dvbssu();
#endif

#if defined(CONFIG_OTA_DTT_EU)
	case eSVCSI_ACT_SPEC_DTT:
		return xsvc_si_SwupCheckGetActionFunc_EuDtt();
#endif

	default:
		break;
	}

	return NULL;
}

STATIC void *psvc_si_AllocSwupCheckActionData(XsvcSi_ActionSpec_e eActSpec)
{
	switch (eActSpec)
	{
#if defined(CONFIG_OTA_HUMAX)
	case eSVCSI_ACT_SPEC_Humax:
		return xsvc_si_SwupCheckAllocData_Humax();
#endif

#if defined(CONFIG_OTA_IRDETO)
	case eSVCSI_ACT_SPEC_Irdeto:
		return xsvc_si_SwupCheckAllocData_Irdeto();
#endif

#if defined(CONFIG_OTA_SES)
	case eSVCSI_ACT_SPEC_Ses:
		return xsvc_si_SwupCheckAllocData_Ses();
#endif

#if defined(CONFIG_OTA_ASTRA)
	case eSVCSI_ACT_SPEC_Astra:
		return xsvc_si_SwupCheckAllocData_Astra();
#endif

#if defined(CONFIG_OTA_DVBSSU)
	case eSVCSI_ACT_SPEC_DvbSsu:
		return xsvc_si_SwupCheckAllocData_Dvbssu();
#endif
#if defined(CONFIG_OTA_DTT_EU)
	case eSVCSI_ACT_SPEC_DTT:
		return xsvc_si_SwupCheckAllocData_EuDtt();
#endif

	default:
		break;
	}

	return NULL;
}

STATIC void psvc_si_FreeSwupCheckActionData(XsvcSi_ActionSpec_e eActSpec, void *pvData)
{
	switch (eActSpec)
	{
#if defined(CONFIG_OTA_HUMAX)
	case eSVCSI_ACT_SPEC_Humax:
		xsvc_si_SwupCheckFreeData_Humax(pvData);
		return;
#endif

#if defined(CONFIG_OTA_IRDETO)
	case eSVCSI_ACT_SPEC_Irdeto:
		xsvc_si_SwupCheckFreeData_Irdeto(pvData);
		return;
#endif

#if defined(CONFIG_OTA_SES)
	case eSVCSI_ACT_SPEC_Ses:
		xsvc_si_SwupCheckFreeData_Ses(pvData);
		return;
#endif

#if defined(CONFIG_OTA_ASTRA)
	case eSVCSI_ACT_SPEC_Astra:
		xsvc_si_SwupCheckFreeData_Astra(pvData);
		return;
#endif

#if defined(CONFIG_OTA_DVBSSU)
	case eSVCSI_ACT_SPEC_DvbSsu:
		xsvc_si_SwupCheckFreeData_Dvbssu(pvData);
		return;
#endif
#if defined(CONFIG_OTA_DTT_EU)
	case eSVCSI_ACT_SPEC_DTT:
		xsvc_si_SwupCheckFreeData_EuDtt(pvData);
		return;
#endif

	default:
		break;
	}

	return;
}

STATIC SvcSi_ActFunc psvc_si_GetLiveSwupCheckActionFunction(XsvcSi_ActionSpec_e eActSpec)
{
#if defined(CONFIG_OTA_LIVE_ENABLE)
	switch (eActSpec)
	{
#if defined(CONFIG_OTA_HUMAX)
	case eSVCSI_ACT_SPEC_Humax:
		return; 	// not yet
#endif

#if defined(CONFIG_OTA_IRDETO)
	case eSVCSI_ACT_SPEC_Irdeto:
		return; 	// not yet
#endif

#if defined(CONFIG_OTA_SES)
	case eSVCSI_ACT_SPEC_Ses:
		return; 	// not yet
#endif

#if defined(CONFIG_OTA_ASTRA)
	case eSVCSI_ACT_SPEC_Astra:
		return; 	// not yet
#endif

#if defined(CONFIG_OTA_DVBSSU)
	case eSVCSI_ACT_SPEC_DvbSsu:
		return xsvc_si_LiveSwupCheckGetActionFunc_Dvbssu();
#endif

#if defined(CONFIG_OTA_DTT_EU)
	case eSVCSI_ACT_SPEC_DTT:
		return; 	// not yet
#endif

	default:
		break;
	}
#endif

	return NULL;
}

STATIC void *psvc_si_AllocLiveSwupCheckActionData(XsvcSi_ActionSpec_e eActSpec)
{
#if defined(CONFIG_OTA_LIVE_ENABLE)
	switch (eActSpec)
	{
#if defined(CONFIG_OTA_HUMAX)
	case eSVCSI_ACT_SPEC_Humax:
		return; 	// not yet
#endif

#if defined(CONFIG_OTA_IRDETO)
	case eSVCSI_ACT_SPEC_Irdeto:
		return; 	// not yet
#endif

#if defined(CONFIG_OTA_SES)
	case eSVCSI_ACT_SPEC_Ses:
		return; 	// not yet
#endif

#if defined(CONFIG_OTA_ASTRA)
	case eSVCSI_ACT_SPEC_Astra:
		return; 	// not yet
#endif

#if defined(CONFIG_OTA_DVBSSU)
	case eSVCSI_ACT_SPEC_DvbSsu:
		return xsvc_si_LiveSwupCheckAllocData_Dvbssu();
#endif

#if defined(CONFIG_OTA_DTT_EU)
	case eSVCSI_ACT_SPEC_DTT:
		return; 	// not yet
#endif

	default:
		break;
	}
#endif
	return NULL;
}

STATIC void psvc_si_FreeLiveSwupCheckActionData(XsvcSi_ActionSpec_e eActSpec, void *pvData)
{
#if defined(CONFIG_OTA_LIVE_ENABLE)
	switch (eActSpec)
	{
#if defined(CONFIG_OTA_HUMAX)
	case eSVCSI_ACT_SPEC_Humax:
		return; 	// not yet
#endif

#if defined(CONFIG_OTA_IRDETO)
	case eSVCSI_ACT_SPEC_Irdeto:
		return; 	// not yet
#endif

#if defined(CONFIG_OTA_SES)
	case eSVCSI_ACT_SPEC_Ses:
		return; 	// not yet
#endif

#if defined(CONFIG_OTA_ASTRA)
	case eSVCSI_ACT_SPEC_Astra:
		return; 	// not yet
#endif

#if defined(CONFIG_OTA_DVBSSU)
	case eSVCSI_ACT_SPEC_DvbSsu:
		return xsvc_si_LiveSwupCheckFreeData_Dvbssu(pvData);
#endif

#if defined(CONFIG_OTA_DTT_EU)
	case eSVCSI_ACT_SPEC_DTT:
		return; 	// not yet
#endif

	default:
		break;
	}
#endif
	return;
}

STATIC SvcSi_ActFunc psvc_si_GetTimeUpdateActionFunction(XsvcSi_ActionSpec_e eActSpec)
{
	switch (eActSpec)
	{
	case eSVCSI_ACT_SPEC_EuSat:
	case eSVCSI_ACT_SPEC_EuCab:
	case eSVCSI_ACT_SPEC_EuTer:
	case eSVCSI_ACT_SPEC_Freesat:
	case eSVCSI_ACT_SPEC_MeSat:
	case eSVCSI_ACT_SPEC_Ses:
	case eSVCSI_ACT_SPEC_Ertelecom:
	case eSVCSI_ACT_SPEC_Nordig:
	case eSVCSI_ACT_SPEC_AusDtt:
	case eSVCSI_ACT_SPEC_UkDtt:
	case eSVCSI_ACT_SPEC_Ziggo:
	case eSVCSI_ACT_SPEC_ItaDtt:
	case eSVCSI_ACT_SPEC_Orf:
	case eSVCSI_ACT_SPEC_Mbc:
	case eSVCSI_ACT_SPEC_AmxCenam:
		return xsvc_si_TimeGetTimeActionFunc();

	default:
		break;
	}

	return NULL;
}

STATIC void *psvc_si_AllocTimeUpdateActionData(XsvcSi_ActionSpec_e eActSpec)
{
	switch (eActSpec)
	{
	case eSVCSI_ACT_SPEC_EuSat:
	case eSVCSI_ACT_SPEC_EuCab:
	case eSVCSI_ACT_SPEC_EuTer:
	case eSVCSI_ACT_SPEC_Freesat:
	case eSVCSI_ACT_SPEC_MeSat:
	case eSVCSI_ACT_SPEC_Ses:
	case eSVCSI_ACT_SPEC_Ertelecom:
	case eSVCSI_ACT_SPEC_Nordig:
	case eSVCSI_ACT_SPEC_AusDtt:
	case eSVCSI_ACT_SPEC_UkDtt:
	case eSVCSI_ACT_SPEC_Ziggo:
	case eSVCSI_ACT_SPEC_ItaDtt:
	case eSVCSI_ACT_SPEC_Orf:
	case eSVCSI_ACT_SPEC_Mbc:
	case eSVCSI_ACT_SPEC_AmxCenam:
		return xsvc_si_TimeAllocTimeActionData();

	default:
		break;
	}

	return NULL;
}

STATIC void psvc_si_FreeTimeUpdateActionData(XsvcSi_ActionSpec_e eActSpec, void *pvData)
{
	switch (eActSpec)
	{
	case eSVCSI_ACT_SPEC_EuSat:
	case eSVCSI_ACT_SPEC_EuCab:
	case eSVCSI_ACT_SPEC_EuTer:
	case eSVCSI_ACT_SPEC_Freesat:
	case eSVCSI_ACT_SPEC_MeSat:
	case eSVCSI_ACT_SPEC_Ses:
	case eSVCSI_ACT_SPEC_Ertelecom:
	case eSVCSI_ACT_SPEC_Nordig:
	case eSVCSI_ACT_SPEC_AusDtt:
	case eSVCSI_ACT_SPEC_UkDtt:
	case eSVCSI_ACT_SPEC_Ziggo:
	case eSVCSI_ACT_SPEC_ItaDtt:
	case eSVCSI_ACT_SPEC_Orf:
	case eSVCSI_ACT_SPEC_Mbc:
	case eSVCSI_ACT_SPEC_AmxCenam:
		xsvc_si_TimeFreeTimeActionData(pvData);
		return;

	default:
		break;
	}

	return;
}

STATIC SvcSi_ActFunc psvc_si_GetDynamicUpdateActionFunction(XsvcSi_ActionSpec_e eActSpec)
{
	switch (eActSpec)
	{
#if defined(CONFIG_OP_AUS_DTT)
	case eSVCSI_ACT_SPEC_AusDtt:
		return xsvc_si_DuGetDynamicUpdateActionFunc_AusDtt();
#endif

#if defined(CONFIG_OP_UK_DTT)
	case eSVCSI_ACT_SPEC_UkDtt:
		return xsvc_si_DuGetDynamicUpdateActionFunc_UkDtt();
#endif

#if defined(CONFIG_OP_ITALY_DBOOK)
	case eSVCSI_ACT_SPEC_ItaDtt:
		return xsvc_si_DuGetDynamicUpdateActionFunc_ItaDtt();
#endif

#if defined(CONFIG_OP_NORDIG)
	case eSVCSI_ACT_SPEC_Nordig:
		return xsvc_si_DuGetDynamicUpdateActionFunc_Nordig();
#endif

/* Ziggo 기능 추가 */
	default:
		break;
	}

	return NULL;
}

STATIC void *psvc_si_AllocDynamicUpdateActionData(XsvcSi_ActionSpec_e eActSpec)
{
	switch (eActSpec)
	{
#if defined(CONFIG_OP_AUS_DTT)
	case eSVCSI_ACT_SPEC_AusDtt:
		return xsvc_si_DuAllocDynamicUpdateActionData_AusDtt();
#endif

#if defined(CONFIG_OP_UK_DTT)
	case eSVCSI_ACT_SPEC_UkDtt:
		return xsvc_si_DuAllocDynamicUpdateActionData_UkDtt();
#endif

#if defined(CONFIG_OP_ITALY_DBOOK)
	case eSVCSI_ACT_SPEC_ItaDtt:
		return xsvc_si_DuAllocDynamicUpdateActionData_ItaDtt();
#endif

#if defined(CONFIG_OP_NORDIG)
	case eSVCSI_ACT_SPEC_Nordig:
		return xsvc_si_DuAllocDynamicUpdateActionData_Nordig();
#endif

/* Ziggo 기능 추가 */
	default:
		break;
	}

	return NULL;
}

STATIC void psvc_si_FreeDynamicUpdateActionData(XsvcSi_ActionSpec_e eActSpec, void *pvData)
{
	switch (eActSpec)
	{
#if defined(CONFIG_OP_AUS_DTT)
	case eSVCSI_ACT_SPEC_AusDtt:
		return xsvc_si_DuFreeDynamicUpdateActionData_AusDtt(pvData);
#endif

#if defined(CONFIG_OP_UK_DTT)
	case eSVCSI_ACT_SPEC_UkDtt:
		return xsvc_si_DuFreeDynamicUpdateActionData_UkDtt(pvData);
#endif

#if defined(CONFIG_OP_ITALY_DBOOK)
	case eSVCSI_ACT_SPEC_ItaDtt:
		return xsvc_si_DuFreeDynamicUpdateActionData_ItaDtt(pvData);
#endif

#if defined(CONFIG_OP_NORDIG)
	case eSVCSI_ACT_SPEC_Nordig:
		return xsvc_si_DuFreeDynamicUpdateActionData_Nordig(pvData);
#endif

/* Ziggo 기능 추가 */
	default:
		break;
	}

	return;
}

STATIC SvcSi_ActFunc psvc_si_GetMediapbActionFunc(XsvcSi_ActionSpec_e eActSpec)
{
	switch (eActSpec)
	{
#if defined(CONFIG_OP_ERTELECOM)
	case eSVCSI_ACT_SPEC_Ertelecom:
		return xsvc_si_MediapbGetMediapbActionFunc_Ertelecom();
#endif
#if defined(CONFIG_OP_NORDIG)
	case eSVCSI_ACT_SPEC_Nordig:
		return xsvc_si_MediapbGetMediapbActionFunc_Nordig();
#endif
#if defined(CONFIG_OP_SES)
	case eSVCSI_ACT_SPEC_Ses:
		return xsvc_si_MediapbGetMediapbActionFunc_Ses();
#endif

	default:
		break;
	}

	return NULL;
}

STATIC void *psvc_si_AllocMediapbActionData(XsvcSi_ActionSpec_e eActSpec)
{
	switch (eActSpec)
	{
#if defined(CONFIG_OP_ERTELECOM)
	case eSVCSI_ACT_SPEC_Ertelecom:
		return xsvc_si_MediapbAllocMediapbActionData_Ertelecom();
#endif
#if defined(CONFIG_OP_NORDIG)
	case eSVCSI_ACT_SPEC_Nordig:
		return xsvc_si_MediapbAllocMediapbActionData_Nordig();
#endif
#if defined(CONFIG_OP_SES)
	case eSVCSI_ACT_SPEC_Ses:
		return xsvc_si_MediapbAllocMediapbActionData_Ses();
#endif

	default:
		break;
	}

	return NULL;
}

STATIC void psvc_si_FreeMediapbActionData(XsvcSi_ActionSpec_e eActSpec, void *pvData)
{
	switch (eActSpec)
	{
#if defined(CONFIG_OP_ERTELECOM)
	case eSVCSI_ACT_SPEC_Ertelecom:
		xsvc_si_MediapbFreeMediapbActionData_Ertelecom(pvData);
#endif
#if defined(CONFIG_OP_NORDIG)
	case eSVCSI_ACT_SPEC_Nordig:
		xsvc_si_MediapbFreeMediapbActionData_Nordig(pvData);
#endif
#if defined(CONFIG_OP_SES)
	case eSVCSI_ACT_SPEC_Ses:
		xsvc_si_MediapbFreeMediapbActionData_Ses(pvData);
#endif

	default:
		break;
	}

	return;
}

STATIC HERROR xsvc_si_GetEsSelectSpecs (HUINT32 ulActionId, SvcSi_EsSelect_e *peVideo, SvcSi_EsSelect_e *peAudio, SvcSi_EsSelect_e *peSubttl, SvcSi_EsSelect_e *peTeltxt)
{
	SvcSi_SiSpec_t			 unSiSpec;
	SvcSi_EsSelect_e		 eVideoSelect;
	SvcSi_EsSelect_e		 eAudioSelect;
	SvcSi_EsSelect_e		 eSbtlSelect;
	SvcSi_EsSelect_e		 eTtxSelect;
	HERROR					 hErr;

	hErr = svc_si_GetSiSpec (ulActionId, &unSiSpec);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_GetSiSpec err:\n");
		return ERR_FAIL;
	}

	switch (unSiSpec.stTemplate.eActionKind)
	{
	case eSVCSI_ACT_KIND_Live:
	case eSVCSI_ACT_KIND_Record:
	case eSVCSI_ACT_KIND_Playback:
	case eSVCSI_ACT_KIND_Search:
	case eSVCSI_ACT_KIND_Satip:
		eVideoSelect	= unSiSpec.stLiveSpec.eVideo;
		eAudioSelect	= unSiSpec.stLiveSpec.eAudio;
		eSbtlSelect		= unSiSpec.stLiveSpec.eSubtitle;
		eTtxSelect		= unSiSpec.stLiveSpec.eTeletext;
		break;

	default:
		HxLOG_Error ("eActionKind(%d) not supported:\n", unSiSpec.stTemplate.eActionKind);
		return ERR_FAIL;
	}

	if (NULL != peVideo)					{ *peVideo  = eVideoSelect; }
	if (NULL != peAudio)					{ *peAudio  = eAudioSelect; }
	if (NULL != peSubttl)					{ *peSubttl = eSbtlSelect; }
	if (NULL != peTeltxt)					{ *peTeltxt = eTtxSelect; }

	return ERR_OK;
}

#if defined(CONFIG_MW_SI_AIT)
STATIC SvcSi_ActFunc psvc_si_GetHbbtvActionFunction (XsvcSi_ActionSpec_e eActSpec)
{
	(void)eActSpec;
	return xsvc_si_HbbtvGetActionFunc_Base();
}

STATIC void *psvc_si_AllocHbbtvActionData(XsvcSi_ActionSpec_e eActSpec)
{
	(void)eActSpec;
	return xsvc_si_HbbtvAllocActionData_Base();
}

STATIC void psvc_si_FreeHbbtvActionData(XsvcSi_ActionSpec_e eActSpec, void *pvData)
{
	(void)eActSpec;
	xsvc_si_HbbtvFreeActionData_Base(pvData);
}
#endif

#define ___ACTION_API_FUNCTIONS___

SvcSi_ActFunc psvc_si_GetActionFunc(SvcSi_ActionKind_e eActKind, HUINT32 ulActSpec)
{
	XsvcSi_ActionSpec_e eActSpec = (XsvcSi_ActionSpec_e)ulActSpec;

	switch (eActKind)
	{
	case eSVCSI_ACT_KIND_Live:
	case eSVCSI_ACT_KIND_Record:
	case eSVCSI_ACT_KIND_Playback:
	case eSVCSI_ACT_KIND_Satip:
		return psvc_si_GetLiveActionFunc(eActSpec);

	case eSVCSI_ACT_KIND_MediaPlayback:
		return psvc_si_GetMediapbActionFunc(eActSpec);

	case eSVCSI_ACT_KIND_Search:
		return psvc_si_GetSearchActionFunc(eActSpec);

	case eSVCSI_ACT_KIND_ExtSearch:
		return psvc_si_GetExtSearchActionFunc(eActSpec);

	case eSVCSI_ACT_KIND_SatRecord:
		return psvc_si_GetSatRecordActionFunc(eActSpec);

	case eSVCSI_ACT_KIND_SwupCheck:
		return psvc_si_GetSwupCheckActionFunction(eActSpec);

	case eSVCSI_ACT_KIND_LiveSwupCheck:
		return psvc_si_GetLiveSwupCheckActionFunction(eActSpec);

	case eSVCSI_ACT_KIND_TimeUpdate:
		return psvc_si_GetTimeUpdateActionFunction(eActSpec);

	case eSVCSI_ACT_KIND_DynamicUpdate:
		return psvc_si_GetDynamicUpdateActionFunction(eActSpec);

#if defined(CONFIG_MW_SI_AIT)
	case eSVCSI_ACT_KIND_HbbTv:
		return psvc_si_GetHbbtvActionFunction(eActSpec);
#endif

	default:
		break;
	}

	return NULL;
}

void *psvc_si_AllocActionData(SvcSi_ActionKind_e eActKind, HUINT32 ulActSpec)
{
	XsvcSi_ActionSpec_e eActSpec = (XsvcSi_ActionSpec_e)ulActSpec;

	switch (eActKind)
	{
	case eSVCSI_ACT_KIND_Live:
	case eSVCSI_ACT_KIND_Record:
	case eSVCSI_ACT_KIND_Playback:
	case eSVCSI_ACT_KIND_Satip:
		return psvc_si_AllocLiveActionData(eActSpec);

	case eSVCSI_ACT_KIND_MediaPlayback:
		return psvc_si_AllocMediapbActionData(eActSpec);

	case eSVCSI_ACT_KIND_Search:
		return psvc_si_AllocSearchActionData(eActSpec);

	case eSVCSI_ACT_KIND_ExtSearch:
		return psvc_si_AllocExtSearchActionData(eActSpec);

	case eSVCSI_ACT_KIND_SatRecord:
		return psvc_si_AllocSatRecordActionData(eActSpec);

	case eSVCSI_ACT_KIND_SwupCheck:
		return psvc_si_AllocSwupCheckActionData(eActSpec);

	case eSVCSI_ACT_KIND_LiveSwupCheck:
		return psvc_si_AllocLiveSwupCheckActionData(eActSpec);

	case eSVCSI_ACT_KIND_TimeUpdate:
		return psvc_si_AllocTimeUpdateActionData(eActSpec);

	case eSVCSI_ACT_KIND_DynamicUpdate:
		return psvc_si_AllocDynamicUpdateActionData(eActSpec);

#if defined(CONFIG_MW_SI_AIT)
	case eSVCSI_ACT_KIND_HbbTv:
		return psvc_si_AllocHbbtvActionData(eActSpec);
#endif

	default:
		break;
	}

	return NULL;
}

void psvc_si_FreeActionData(SvcSi_ActionKind_e eActKind, XsvcSi_ActionSpec_e ulActSpec, void *pvData)
{
	XsvcSi_ActionSpec_e eActSpec = (XsvcSi_ActionSpec_e)ulActSpec;

	switch (eActKind)
	{
	case eSVCSI_ACT_KIND_Live:
	case eSVCSI_ACT_KIND_Record:
	case eSVCSI_ACT_KIND_Playback:
	case eSVCSI_ACT_KIND_Satip:
		psvc_si_FreeLiveActionData(eActSpec, pvData);
		return;

	case eSVCSI_ACT_KIND_MediaPlayback:
		psvc_si_FreeMediapbActionData(eActSpec, pvData);
		return;

	case eSVCSI_ACT_KIND_Search:
		psvc_si_FreeSearchActionData(eActSpec, pvData);
		return;

	case eSVCSI_ACT_KIND_ExtSearch:
		psvc_si_FreeExtSearchActionData(eActSpec, pvData);
		return;

	case eSVCSI_ACT_KIND_SatRecord:
		psvc_si_FreeSatRecordActionData(eActSpec, pvData);
		return;

	case eSVCSI_ACT_KIND_SwupCheck:
		psvc_si_FreeSwupCheckActionData(eActSpec, pvData);
		return;

	case eSVCSI_ACT_KIND_LiveSwupCheck:
		psvc_si_FreeLiveSwupCheckActionData(eActSpec, pvData);
		return;

	case eSVCSI_ACT_KIND_TimeUpdate:
		psvc_si_FreeTimeUpdateActionData(eActSpec, pvData);
		return;

	case eSVCSI_ACT_KIND_DynamicUpdate:
		psvc_si_FreeDynamicUpdateActionData(eActSpec, pvData);
		return;

#if defined(CONFIG_MW_SI_AIT)
	case eSVCSI_ACT_KIND_HbbTv:
		return psvc_si_FreeHbbtvActionData(eActSpec, pvData);
#endif

	default:
		break;
	}

	return;
}

#define ___SERVICE_API_FUNCTIONS___

HERROR psvc_si_GetPmtEsInfoFromPmtSiTable (HUINT32 ulActionId, SIxTable_PsiTable_t *pstSiTable, svcSi_PmtEsInfo_t *pstPmtEsInfo)
{
	SvcSi_SiSpec_t			 unSiSpec;
	HERROR					 hErr;

	if ((NULL == pstSiTable) || (NULL == pstPmtEsInfo))
	{
		return ERR_FAIL;
	}

	hErr = svc_si_GetSiSpec (ulActionId, &unSiSpec);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_GetSiSpec err:\n");
		return ERR_FAIL;
	}

	switch (unSiSpec.stTemplate.ulActionSpec)
	{
	case eSVCSI_ACT_SPEC_EuSat:
	case eSVCSI_ACT_SPEC_EuCab:
	case eSVCSI_ACT_SPEC_EuTer:
	case eSVCSI_ACT_SPEC_MeSat:
	case eSVCSI_ACT_SPEC_Freesat:
	case eSVCSI_ACT_SPEC_Ses:
	case eSVCSI_ACT_SPEC_Ertelecom:
	case eSVCSI_ACT_SPEC_Nordig:
	case eSVCSI_ACT_SPEC_AusDtt:
	case eSVCSI_ACT_SPEC_UkDtt:
	case eSVCSI_ACT_SPEC_Ziggo:
	case eSVCSI_ACT_SPEC_ItaDtt:
	case eSVCSI_ACT_SPEC_Orf:
	case eSVCSI_ACT_SPEC_Mbc:
	case eSVCSI_ACT_SPEC_AmxCenam:
		hErr = xsvc_si_LibSvcEsGetPmtEsInfo_Base (ulActionId, pstSiTable, pstPmtEsInfo);
		break;

	default:
		HxLOG_Error ("Action Spec (%d) Not Support !!!\n", unSiSpec.stTemplate.ulActionSpec);
		hErr = ERR_FAIL;
		break;
	}

	return hErr;
}

HERROR psvc_si_SelectVideoEs (HUINT32 ulActionId, svcSi_PmtEsInfo_t *pstPmtEsInfo, DbSvc_Info_t *pstSvcInfo, svc_si_SelectedEs_t *pstSelectedVideo)
{
	SvcSi_EsSelect_e		 eVideoSelect;
	HERROR					 hErr;

	if ((NULL == pstPmtEsInfo) || (NULL == pstSvcInfo) || (NULL == pstSelectedVideo))
	{
		return ERR_FAIL;
	}

	hErr = xsvc_si_GetEsSelectSpecs (ulActionId, &eVideoSelect, NULL, NULL, NULL);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_GetSiSpec err:\n");
		return ERR_FAIL;
	}

	switch (eVideoSelect)
	{
	case eSVCSI_ES_SELECT_Normal:
		hErr = xsvc_si_LibSvcEsSelectVideo_Base (ulActionId, pstPmtEsInfo, pstSvcInfo, pstSelectedVideo);
		break;

#if defined(CONFIG_OP_NORDIG)
	case eSVCSI_ES_SELECT_Nordig:
		hErr = xsvc_si_LibSvcEsSelectVideo_Nordig(ulActionId, pstPmtEsInfo, pstSvcInfo, pstSelectedVideo);
		break;
#endif

#if defined(CONFIG_OP_UK_DTT)
	case eSVCSI_ES_SELECT_Ukdtt:
		hErr = xsvc_si_LibSvcEsSelectVideo_Ukdtt(ulActionId, pstPmtEsInfo, pstSvcInfo, pstSelectedVideo);
		break;
#endif

	default:
		HxLOG_Error ("eVideoSelect(%d) Not supported!!! \n", eVideoSelect);
		hErr = ERR_FAIL;
		break;
	}

	return hErr;
}

HERROR psvc_si_SelectAudioEs (HUINT32 ulActionId, svcSi_PmtEsInfo_t *pstPmtEsInfo, DbSvc_Info_t *pstSvcInfo, svc_si_SelectedAudio_t *pstSelectedAudio)
{
	SvcSi_EsSelect_e		 eAudioSelect;
	HERROR					 hErr;

	if ((NULL == pstPmtEsInfo) || (NULL == pstSvcInfo) || (NULL == pstSelectedAudio))
	{
		return ERR_FAIL;
	}

	hErr = xsvc_si_GetEsSelectSpecs (ulActionId, NULL, &eAudioSelect, NULL, NULL);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_GetSiSpec err:\n");
		return ERR_FAIL;
	}

	switch (eAudioSelect)
	{
	case eSVCSI_ES_SELECT_Normal:
		hErr = xsvc_si_LibSvcEsSelectAudio_Base (ulActionId, pstPmtEsInfo, pstSvcInfo, pstSelectedAudio);
		break;

#if defined(CONFIG_OP_UK_DTT)
	case eSVCSI_ES_SELECT_Ukdtt:
		hErr = xsvc_si_LibSvcEsSelectAudio_Ukdtt (ulActionId, pstPmtEsInfo, pstSvcInfo, pstSelectedAudio);
		break;
#endif

	default:
		HxLOG_Error ("eAudioSelect(%d) Not supported!!! \n", eAudioSelect);
		hErr = ERR_FAIL;
		break;
	}

	return hErr;
}

HERROR psvc_si_SelectSubtitleEs (HUINT32 ulActionId, svcSi_PmtEsInfo_t *pstPmtEsInfo, DbSvc_Info_t *pstSvcInfo, svc_si_SelectedEs_t *pstSelectedSubttl)
{
	SvcSi_EsSelect_e		 eSubttlSelect;
	HERROR					 hErr;

	if ((NULL == pstPmtEsInfo) || (NULL == pstSvcInfo) || (NULL == pstSelectedSubttl))
	{
		return ERR_FAIL;
	}

	hErr = xsvc_si_GetEsSelectSpecs (ulActionId, NULL, NULL, &eSubttlSelect, NULL);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_GetSiSpec err:\n");
		return ERR_FAIL;
	}

	switch (eSubttlSelect)
	{
	case eSVCSI_ES_SELECT_Normal:
		hErr = xsvc_si_LibSvcEsSelectSubtitle_Base (ulActionId, pstPmtEsInfo, pstSvcInfo, pstSelectedSubttl);
		break;

#if defined(CONFIG_OP_UK_DTT)
	case eSVCSI_ES_SELECT_Ukdtt:
		hErr = xsvc_si_LibSvcEsSelectSubtitle_Ukdtt (ulActionId, pstPmtEsInfo, pstSvcInfo, pstSelectedSubttl);
		break;
#endif

	default:
		HxLOG_Error ("eSubttlSelect(%d) Not supported!!! \n", eSubttlSelect);
		hErr = ERR_FAIL;
		break;
	}

	return hErr;
}

HERROR psvc_si_SelectTeletextEs (HUINT32 ulActionId, svcSi_PmtEsInfo_t *pstPmtEsInfo, DbSvc_Info_t *pstSvcInfo, svc_si_SelectedEs_t *pstSelectedTtx)
{
	SvcSi_EsSelect_e		 eTtxSelect;
	HERROR					 hErr;

	if ((NULL == pstPmtEsInfo) || (NULL == pstSvcInfo) || (NULL == pstSelectedTtx))
	{
		return ERR_FAIL;
	}

	hErr = xsvc_si_GetEsSelectSpecs (ulActionId, NULL, NULL, NULL, &eTtxSelect);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("xsvc_si_GetEsSelectSpecs err:\n");
		return ERR_FAIL;
	}

	switch (eTtxSelect)
	{
	case eSVCSI_ES_SELECT_Normal:
		hErr = xsvc_si_LibSvcEsSelectTeletext_Base (ulActionId, pstPmtEsInfo, pstSvcInfo, pstSelectedTtx);
		break;
	default:
		HxLOG_Error ("eTtxSelect(%d) Not supported!!! \n", eTtxSelect);
		hErr = ERR_FAIL;
		break;
	}

	return hErr;
}

HERROR psvc_si_ComposeServiceInfoInTp (HUINT32 ulActionId, HUINT32 *pulSvcNum, DbSvc_Info_t **ppstSvcArray, HUINT32 *pulPrvNum, DbSvc_PrvInfo_t **ppstPrvArray)
{
	SvcSi_SiSpec_t			 unSiSpec;
	HERROR					 hErr;

	hErr = svc_si_GetSiSpec (ulActionId, &unSiSpec);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_GetSiSpec err:\n");
		return ERR_FAIL;
	}

	switch (unSiSpec.stTemplate.ulActionSpec)
	{
	case eSVCSI_ACT_SPEC_EuSat:
	case eSVCSI_ACT_SPEC_EuCab:
	case eSVCSI_ACT_SPEC_EuTer:
	case eSVCSI_ACT_SPEC_MeSat:
	case eSVCSI_ACT_SPEC_Freesat:
	case eSVCSI_ACT_SPEC_Ses:
	case eSVCSI_ACT_SPEC_AusDtt:
	case eSVCSI_ACT_SPEC_UkDtt:
	case eSVCSI_ACT_SPEC_Ziggo:
	case eSVCSI_ACT_SPEC_ItaDtt:
	case eSVCSI_ACT_SPEC_Orf:
	case eSVCSI_ACT_SPEC_Mbc:
	case eSVCSI_ACT_SPEC_AmxCenam:
		hErr = xsvc_si_LibSvcEsComposeServiceInfoInTp_Base (ulActionId, pulSvcNum, ppstSvcArray, pulPrvNum, ppstPrvArray);
		break;
#if defined(CONFIG_OP_ERTELECOM)
	case eSVCSI_ACT_SPEC_Ertelecom:
		hErr = xsvc_si_SrchSvcEsComposeServiceInfoInTp_Ertelecom (ulActionId, pulSvcNum, ppstSvcArray, pulPrvNum, ppstPrvArray);
		break;
#endif
#if defined(CONFIG_OP_NORDIG)
	case eSVCSI_ACT_SPEC_Nordig:
		hErr = xsvc_si_SrchSvcEsComposeServiceInfoInTp_Nordig (ulActionId, pulSvcNum, ppstSvcArray, pulPrvNum, ppstPrvArray);
		break;
#endif
	default:
		HxLOG_Error ("eActionSpec(%d) Not supported!!! \n", unSiSpec.stTemplate.ulActionSpec);
		hErr = xsvc_si_LibSvcEsComposeServiceInfoInTp_Base (ulActionId, pulSvcNum, ppstSvcArray, pulPrvNum, ppstPrvArray);
		break;
	}

	return hErr;
}

HERROR psvc_si_GetSvcAndVideoTypeFromDvbSvcType (HUINT32 ulActionId, HUINT32 ulDvbSvcType, DxSvcType_e *peSvcType, DxVideoType_e *peVideoType)
{
	SvcSi_SiSpec_t			 unSiSpec;
	HERROR					 hErr;

	hErr = svc_si_GetSiSpec (ulActionId, &unSiSpec);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_GetSiSpec err:\n");
		return ERR_FAIL;
	}

	switch (unSiSpec.stTemplate.ulActionSpec)
	{
	case eSVCSI_ACT_SPEC_EuSat:
	case eSVCSI_ACT_SPEC_EuCab:
	case eSVCSI_ACT_SPEC_EuTer:
	case eSVCSI_ACT_SPEC_MeSat:
	case eSVCSI_ACT_SPEC_Freesat:
	case eSVCSI_ACT_SPEC_Ertelecom:
	case eSVCSI_ACT_SPEC_Nordig:
	case eSVCSI_ACT_SPEC_AusDtt:
	case eSVCSI_ACT_SPEC_Ziggo:
	case eSVCSI_ACT_SPEC_ItaDtt:
	case eSVCSI_ACT_SPEC_Orf:
	case eSVCSI_ACT_SPEC_Mbc:
	case eSVCSI_ACT_SPEC_AmxCenam:
		hErr = xsvc_si_LibSvcEsGetSvcAndVideoTypeFromDvbSvcType_Base (ulActionId, ulDvbSvcType, peSvcType, peVideoType);
		break;
	case eSVCSI_ACT_SPEC_UkDtt:
#if defined(CONFIG_OP_UK_DTT)
		hErr = xsvc_si_LibSvcEsGetSvcAndVideoTypeFromDvbSvcType_Ukdtt(ulActionId, ulDvbSvcType, peSvcType, peVideoType);
		if (hErr != ERR_OK)
		{
			hErr = xsvc_si_LibSvcEsGetSvcAndVideoTypeFromDvbSvcType_Base (ulActionId, ulDvbSvcType, peSvcType, peVideoType);
		}
		break;
#endif

#if defined(CONFIG_OP_SES)
	case eSVCSI_ACT_SPEC_Ses:
		hErr = xsvc_si_ServiceGetSvcAndVideoTypeFromDvbSvcType_Ses (ulActionId, ulDvbSvcType, peSvcType, peVideoType);
		if (hErr != ERR_OK)
		{
			hErr = xsvc_si_LibSvcEsGetSvcAndVideoTypeFromDvbSvcType_Base (ulActionId, ulDvbSvcType, peSvcType, peVideoType);
		}
		break;
#endif

	default:
		HxLOG_Error ("eActionSpec(%d) Not supported!!! \n", unSiSpec.stTemplate.ulActionSpec);
		hErr = ERR_FAIL;
		break;
	}

	return hErr;
}

HERROR psvc_si_UpdateTimeByTdt (HUINT32 ulActionId, Handle_t hService, SIxTable_PsiTable_t *pstTdtTable)
{
	SvcSi_TimeUpdate_e		 eTimeUpdate;
	SvcSi_SiSpec_t			 unSiSpec;
	HERROR					 hErr = ERR_FAIL;

	hErr = svc_si_GetSiSpec (ulActionId, &unSiSpec);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_GetSiSpec err:\n");
		return ERR_FAIL;
	}

	switch (unSiSpec.stTemplate.eActionKind)
	{
	case eSVCSI_ACT_KIND_Live:
		eTimeUpdate = unSiSpec.stLiveSpec.eTimeUpdate;
		break;

	case eSVCSI_ACT_KIND_ExtSearch:
	case eSVCSI_ACT_KIND_TimeUpdate:
#if defined (CONFIG_PROD_ICORDPRO) || defined(CONFIG_PROD_HDNEO) || defined(CONFIG_PROD_HGS1000S)
	case eSVCSI_ACT_KIND_SwupCheck:
#endif
		eTimeUpdate = eSVCSI_TIME_UPDATE_Normal;	// coldboot에서 timeupdate하는 것이므로 항상 해야 한다
		break;

	default:
		HxLOG_Error ("eActionKind(%d) Not supported!!! \n", unSiSpec.stTemplate.eActionKind);
		return ERR_FAIL;
	}

	switch (eTimeUpdate)
	{
	case eSVCSI_TIME_UPDATE_None:
		return ERR_FAIL;

	case eSVCSI_TIME_UPDATE_Normal:
		hErr = xsvc_si_LibSvcEsUpdateTimeByTdt_Base (ulActionId, pstTdtTable);
		break;

	case eSVCSI_TIME_UPDATE_MiddelEast:
#if defined(CONFIG_OP_MIDDLE_EAST)
		hErr = ERR_FAIL;
		if (xsvc_si_LiveIsTimeUpdateAvailable(hService) == TRUE)
		{
			hErr = xsvc_si_LibSvcEsUpdateTimeByTdt_Base (ulActionId, pstTdtTable);
		}
#endif
		break;

	case eSVCSI_TIME_UPDATE_EU:
#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
		hErr = xsvc_si_LiveSvcEsUpdateTimeByTdt_Eusat(ulActionId, pstTdtTable);
#endif
		break;

	default:
		HxLOG_Error ("eTimeUpdate(%d) Not supported!!! \n", eTimeUpdate);
		return ERR_FAIL;
	}

	return hErr;
}

HERROR psvc_si_UpdateTimeByTot (HUINT32 ulActionId)
{
	SvcSi_TimeUpdate_e		 eTimeUpdate;
	SvcSi_SiSpec_t			 unSiSpec;
	HERROR					 hErr;

	hErr = svc_si_GetSiSpec (ulActionId, &unSiSpec);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_GetSiSpec err:\n");
		return ERR_FAIL;
	}

	switch (unSiSpec.stTemplate.eActionKind)
	{
	case eSVCSI_ACT_KIND_Live:
		eTimeUpdate = unSiSpec.stLiveSpec.eTimeUpdate;
		break;

	case eSVCSI_ACT_KIND_TimeUpdate:
		eTimeUpdate = eSVCSI_TIME_UPDATE_Normal;	// coldboot에서 timeupdate하는 것이므로 항상 해야 한다
		break;

	default:
		HxLOG_Error ("eActionKind(%d) Not supported!!! \n", unSiSpec.stTemplate.eActionKind);
		return ERR_FAIL;
	}

	// TODO: 현재 사용하는 곳이 없어 틀만 구현해 둠.. 구현시 psvc_si_UpdateTimeByTdt()을 참조할 것..!!
	switch (eTimeUpdate)
	{
	case eSVCSI_TIME_UPDATE_Normal:
		hErr = xsvc_si_LibSvcEsUpdateTimeByTot_Base (ulActionId);
		break;
#if defined(CONFIG_OP_UK_DTT)
	case eSVCSI_TIME_UPDATE_Ukdtt:
		hErr = xsvc_si_LibSvcEsUpdateTimeByTot_Ukdtt (ulActionId);
		break;
#endif

	default:
		HxLOG_Error ("eTimeUpdate(%d) Not supported!!! \n", eTimeUpdate);
		return ERR_FAIL;
	}

	return hErr;
}

HERROR psvc_si_ComposeServiceInfoBySdt(HUINT32 ulActionId, SIxTable_PsiTable_t *pstSdtTable, HUINT32 *pulSvcNum, DbSvc_Info_t **ppstSvcArray, HUINT32 *pulPrvNum, DbSvc_PrvInfo_t **ppstPrvArray)
{
	SvcSi_SiSpec_t			 unSiSpec;
	HERROR					 hErr;

	hErr = svc_si_GetSiSpec (ulActionId, &unSiSpec);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_GetSiSpec err:\n");
		return ERR_FAIL;
	}

	HxLOG_Debug("unSiSpec.stTemplate.ulActionSpec: %d \n", unSiSpec.stTemplate.ulActionSpec);

	switch (unSiSpec.stTemplate.ulActionSpec)
	{

#if defined(CONFIG_OP_NORDIG)
	case eSVCSI_ACT_SPEC_Nordig:
		hErr = xsvc_si_LibSvcEsComposeServiceInfoBySdt_Nordig (ulActionId, pstSdtTable, pulSvcNum, ppstSvcArray, pulPrvNum, ppstPrvArray);
		break;
#endif

	default:
		hErr = xsvc_si_LibSvcEsComposeServiceInfoBySdt_Base (ulActionId, pstSdtTable, pulSvcNum, ppstSvcArray, pulPrvNum, ppstPrvArray);
		break;
	}

	return hErr;

}

//HERROR psvc_si_SetLcnServiceNumber(Handle_t hAction, XsvcSi_ActionSpec_e eActSpec, HUINT32 ulNum, DbSvc_Info_t *pstSvcArray, SRCHSVC_AppendFlag_t *peFlagArray)
HERROR psvc_si_SetLcnServiceNumber(Handle_t hAction, HUINT32 ulActSpec, HUINT32 ulNum, DbSvc_Info_t *pstSvcArray, SRCHSVC_AppendFlag_t *peFlagArray)
{
	XsvcSi_ActionSpec_e eActSpec;

	if (0 == ulNum)								{ return ERR_FAIL; }
	if (NULL == pstSvcArray)					{ return ERR_FAIL; }
	if (NULL == peFlagArray)					{ return ERR_FAIL; }

	eActSpec = (XsvcSi_ActionSpec_e)ulActSpec;

	switch (eActSpec)
	{
#if defined(CONFIG_MW_CH_SATELLITE)
	case eSVCSI_ACT_SPEC_EuSat:
		return xsvc_si_LcnSetServiceNumber_Eusat(hAction, ulNum, pstSvcArray, peFlagArray);
		break;
#endif

#if defined(CONFIG_MW_CH_TERRESTRIAL)
	case eSVCSI_ACT_SPEC_EuTer:
		return xsvc_si_LcnSetServiceNumber_Eudtt(hAction, ulNum, pstSvcArray, peFlagArray);
#endif

#if defined(CONFIG_OP_ERTELECOM)
	case eSVCSI_ACT_SPEC_Ertelecom:
		return xsvc_si_LcnSetServiceNumber_Ertelecom(hAction, ulNum, pstSvcArray, peFlagArray);
		break;
#endif

#if defined(CONFIG_OP_NORDIG)
	case eSVCSI_ACT_SPEC_Nordig:
		/**
		 * 2015-04-10
		 * 아래 함수에서 Non-LCN 서비스에 대한 LCN을 부여하는데
		 * Search Flow 변경으로 Save 직전에 부여하도록 수정되어 주석 처리.
		 */
		//return xsvc_si_LcnSetServiceNumber_Nordig(hAction, ulNum, pstSvcArray, peFlagArray);
		return ERR_OK;
#endif

#if defined(CONFIG_OP_MIDDLE_EAST)
	case eSVCSI_ACT_SPEC_MeSat:
		return xsvc_si_LcnSetServiceNumber_MeSat(hAction, ulNum, pstSvcArray, peFlagArray);
#endif

#if defined(CONFIG_OP_SES)
	case eSVCSI_ACT_SPEC_Ses:
		return xsvc_si_LcnSetServiceNumber_Ses(hAction, ulNum, pstSvcArray, peFlagArray);
		break;
#endif

#if defined(CONFIG_OP_AUS_DTT)
	case eSVCSI_ACT_SPEC_AusDtt:
		return xsvc_si_LcnSetServiceNumber_AusDtt(hAction, ulNum, pstSvcArray, peFlagArray);
#endif

#if defined(CONFIG_OP_UK_DTT)
	case eSVCSI_ACT_SPEC_UkDtt:
		return xsvc_si_LcnSetServiceNumber_UkDtt(hAction, ulNum, pstSvcArray, peFlagArray);
#endif

/* Ziggo 기능 추가 */
#if defined(CONFIG_OP_ZIGGO)
	case eSVCSI_ACT_SPEC_Ziggo:
		return xsvc_si_LcnSetServiceNumber_Ziggo(hAction, ulNum, pstSvcArray, peFlagArray);
#endif

#if defined(CONFIG_OP_ITALY_DBOOK)
	case eSVCSI_ACT_SPEC_ItaDtt:
		return xsvc_si_LcnSetServiceNumber_ItaDtt(hAction, ulNum, pstSvcArray, peFlagArray);
#endif

#if defined(CONFIG_OP_ORF)
	case eSVCSI_ACT_SPEC_Orf:
		return xsvc_si_LcnSetServiceNumber_HdFoxIrConnect(hAction, ulNum, pstSvcArray, peFlagArray);
#endif

#if defined(CONFIG_OP_MBC)
	case eSVCSI_ACT_SPEC_Mbc:
		return ERR_OK; // Post LCN Process
#endif

#if defined(CONFIG_OP_AMX)
	case eSVCSI_ACT_SPEC_AmxCenam:
		return xsvc_si_LcnSetServiceNumber_Amx_Cenam(hAction, ulNum, pstSvcArray, peFlagArray);
		break;
#endif

	default:
		HxLOG_Error("Not supported action spec(%d) hAction(0x%X) ", eActSpec, hAction);
		break;
	}

	return ERR_FAIL;
}

// HERROR svc_si_LcnSetSelectedConflictLcn (Handle_t hAction, HUINT32 ulNum, DxBuilder_SearchedInfo_t* pstInfoArray)
HERROR psvc_si_SetSelectedConflictLcn(Handle_t hAction, HUINT32 ulNum, DxBuilder_SearchedInfo_t *pstInfoArray)
{
	HERROR hErr = ERR_FAIL;

	if (0 == ulNum)								{ return ERR_FAIL; }
	if (NULL == pstInfoArray)					{ return ERR_FAIL; }

#if defined(CONFIG_OP_ITALY_DBOOK)
	hErr = xsvc_si_LcnSetSelectedConflictLcn_ItaDtt(hAction, ulNum, pstInfoArray);
#endif

	return hErr;
}


HERROR psvc_si_PostProcessForSvcNumbering (Handle_t hAction, DefaultLcnPolicy_t eLcnPolicy, DxDeliveryType_e eDeliType)
{
	SvcSi_SiSpec_t			 unSiSpec;
	HERROR					 hErr;
	HUINT32					 ulActionId = PAL_PIPE_GetActionId(hAction);

	// Search Done 이후는 eState 값이 Waiting이 된다. 이로인해 SiSpec을 얻어오지 못한다.
	// hErr = svc_si_GetSiSpec(ulActionId, &unSiSpec);
	hErr = svc_si_GetSiSpec_regardlessOfState(ulActionId, &unSiSpec);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_GetSiSpec err:\n");
		return ERR_FAIL;
	}

	switch (unSiSpec.stTemplate.ulActionSpec)
	{
	case eSVCSI_ACT_SPEC_EuSat:
	case eSVCSI_ACT_SPEC_EuCab:
	case eSVCSI_ACT_SPEC_EuTer:
	case eSVCSI_ACT_SPEC_MeSat:
	case eSVCSI_ACT_SPEC_Freesat:
	case eSVCSI_ACT_SPEC_Ses:
	case eSVCSI_ACT_SPEC_AusDtt:
	case eSVCSI_ACT_SPEC_UkDtt:
	case eSVCSI_ACT_SPEC_Ziggo:
	case eSVCSI_ACT_SPEC_ItaDtt:
	case eSVCSI_ACT_SPEC_Orf:
	case eSVCSI_ACT_SPEC_Nordig:
	case eSVCSI_ACT_SPEC_Mbc:
	case eSVCSI_ACT_SPEC_AmxCenam:
		hErr = xsvc_si_LibLcnPostProcessForSvcNumbering(ulActionId, eLcnPolicy, eDeliType);
		break;

	default:
		HxLOG_Error ("eActionSpec(%d) Not supported!!! \n", unSiSpec.stTemplate.ulActionSpec);
		hErr = ERR_FAIL;
		break;
	}

	return hErr;
}

HERROR psvc_si_PostProcessForSimulcastService (Handle_t hAction)
{
	SvcSi_SiSpec_t			 unSiSpec;
	HERROR					 hErr;
	HUINT32					 ulActionId = PAL_PIPE_GetActionId(hAction);

	// Search Done 이후는 eState 값이 Waiting이 된다. 이로인해 SiSpec을 얻어오지 못한다.
	// hErr = svc_si_GetSiSpec(ulActionId, &unSiSpec);
	hErr = svc_si_GetSiSpec_regardlessOfState(ulActionId, &unSiSpec);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_GetSiSpec err:\n");
		return ERR_FAIL;
	}

	switch (unSiSpec.stTemplate.ulActionSpec)
	{
#ifdef CONFIG_OP_NORDIG
	case eSVCSI_ACT_SPEC_Nordig:
		hErr = xsvc_si_SrchProcessForSimualcastService_Nordig();
		break;
#endif

	default:
		HxLOG_Error ("eActionSpec(%d) Not supported!!! \n", unSiSpec.stTemplate.ulActionSpec);
		hErr = ERR_FAIL;
		break;
	}

	return hErr;
}


HERROR psvc_si_GetAudioComponent (HUINT32 ulActionId, SvcSi_AudCpnt_e *peAudCpnt)
{
	SvcSi_SiSpec_t			 unSiSpec;
	HERROR					 hErr;

	if (peAudCpnt == NULL)
	{
		HxLOG_Error ("Error ! Invalid parameter ! \n");
		return ERR_FAIL;
	}

	hErr = svc_si_GetSiSpec(ulActionId, &unSiSpec);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_GetSiSpec err:\n");
		return ERR_FAIL;
	}

	switch (unSiSpec.stTemplate.ulActionSpec)
	{
	case eSVCSI_ACT_SPEC_UkDtt:
		{
			PalAudio_StreamInfo_t stStreamInfo;

			hErr = PAL_AUDIO_GetStreamInfo(&stStreamInfo);
			if (hErr != ERR_OK)
			{
				HxLOG_Error ("PAL_AUDIO_GetStreamInfo() failed..\n");
				return ERR_FAIL;
			}

			switch(stStreamInfo.eStereoType )
			{
			case eDxSTEREO_FORMAT_MONO:		*peAudCpnt = eSI_AUD_CPNT_SINGLE_MONO;	break;
			case eDxSTEREO_FORMAT_DUALMONO:	*peAudCpnt = eSI_AUD_CPNT_DUAL_MONO; 	break;
			case eDxSTEREO_FORMAT_MULTI:		*peAudCpnt = eSI_AUD_CPNT_MULTILINGUAL; 	break;
			default:							*peAudCpnt = eSI_AUD_CPNT_STEREO; 		break;
			}
		}
		break;

	default:
		*peAudCpnt = eSI_AUD_CPNT_STEREO;
		break;
	}

	return ERR_OK;
}

#define ___OTHER_API_FUNCTIONS___

HERROR psvc_si_SetSwupCheckParam(SvcSi_SwupCheckSpec_t *pstSwupSpec, SvcSi_SwupFlag_t *pstSwupCheckParam)
{
	HERROR	hErr = ERR_FAIL;

	switch(pstSwupSpec->ulActionSpec)
	{
#if defined(CONFIG_OTA_IRDETO)
		case eSVCSI_ACT_SPEC_Irdeto:
			hErr = xsvc_si_SwupCheckSetSwupCheckParam_Irdeto(pstSwupCheckParam);
			break;
#endif
		default:
			HxLOG_Error("not supported action spec(%d) \n", pstSwupSpec->ulActionSpec);
			break;

	}

	return hErr;
}

HERROR psvc_si_GetSwupCheckData(Handle_t hAction, SvcSi_SwupCheckSpec_t *pstSwupSpec, HUINT32 ulDataType, HUINT32 *pulDataNum, void **ppvDataArray)
{
	HERROR				hResult = ERR_FAIL;

	if (NULL == pstSwupSpec)									{ return ERR_FAIL; }
	if (NULL == pulDataNum)										{ return ERR_FAIL; }
	if (NULL == ppvDataArray)									{ return ERR_FAIL; }
	if (eSVCSI_ACT_KIND_SwupCheck != pstSwupSpec->eActionKind)	{ return ERR_FAIL; }

	switch(pstSwupSpec->ulActionSpec)
	{
#if defined(CONFIG_OTA_HUMAX)
		case eSVCSI_ACT_SPEC_Humax:
			hResult = xsvc_si_SwupCheckGetSwupCheckData_Humax(hAction, ulDataType, pulDataNum, ppvDataArray);
			break;
#endif

#if defined(CONFIG_OTA_IRDETO)
		case eSVCSI_ACT_SPEC_Irdeto:
			hResult = xsvc_si_SwupCheckGetSwupCheckData_Irdeto(hAction, ulDataType, pulDataNum, ppvDataArray);
			break;
#endif

#if defined(CONFIG_OTA_SES)
		case eSVCSI_ACT_SPEC_Ses:
			hResult = xsvc_si_SwupCheckGetSwupCheckData_Ses(hAction, ulDataType, pulDataNum, ppvDataArray);
			break;
#endif

#if defined(CONFIG_OTA_ASTRA)
		case eSVCSI_ACT_SPEC_Astra:
			hResult = xsvc_si_SwupCheckGetSwupCheckData_Astra(hAction, ulDataType, pulDataNum, ppvDataArray);
			break;
#endif

#if defined(CONFIG_OTA_DVBSSU)
		case eSVCSI_ACT_SPEC_DvbSsu:
			hResult = xsvc_si_SwupCheckGetSwupCheckData_Dvbssu(hAction, ulDataType, pulDataNum, ppvDataArray);
			break;
#endif
#if defined(CONFIG_OTA_DTT_EU)
		case eSVCSI_ACT_SPEC_DTT:
			hResult = xsvc_si_SwupCheckGetSwupCheckData_EuDtt(hAction, ulDataType, pulDataNum, ppvDataArray);
			break;
#endif
		default:
			HxLOG_Error("not supported action spec(%d) \n", pstSwupSpec->ulActionSpec);
			return ERR_FAIL;
	}

	return hResult;
}

HERROR psvc_si_FreeSwupCheckData(Handle_t hAction, SvcSi_SwupCheckSpec_t *pstSwupCheckSpec, HUINT32 ulDataType, void *pvDataArray)
{
	HERROR				hResult = ERR_FAIL;

	if (NULL == pstSwupCheckSpec)									{ return ERR_FAIL; }
	if (NULL == pvDataArray)										{ return ERR_FAIL; }
	if (eSVCSI_ACT_KIND_SwupCheck != pstSwupCheckSpec->eActionKind)		{ return ERR_FAIL; }



	switch(pstSwupCheckSpec->ulActionSpec)
	{
#if defined(CONFIG_OTA_HUMAX)
		case eSVCSI_ACT_SPEC_Humax:
			hResult = xsvc_si_SwupCheckFreeSwupCheckData_Humax(hAction, ulDataType, pvDataArray);
			break;
#endif

#if defined(CONFIG_OTA_IRDETO)
		case eSVCSI_ACT_SPEC_Irdeto:
			hResult = xsvc_si_SwupCheckFreeSwupCheckData_Irdeto(hAction, ulDataType, pvDataArray);
			break;
#endif

#if defined(CONFIG_OTA_SES)
		case eSVCSI_ACT_SPEC_Ses:
			hResult = xsvc_si_SwupCheckFreeSwupCheckData_Ses(hAction, ulDataType, pvDataArray);
			break;
#endif

#if defined(CONFIG_OTA_ASTRA)
		case eSVCSI_ACT_SPEC_Astra:
			hResult = xsvc_si_SwupCheckFreeSwupCheckData_Astra(hAction, ulDataType, pvDataArray);
			break;
#endif

#if defined(CONFIG_OTA_DVBSSU)
		case eSVCSI_ACT_SPEC_DvbSsu:
			hResult = xsvc_si_SwupCheckFreeSwupCheckData_Dvbssu(hAction, ulDataType, pvDataArray);
			break;
#endif

#if defined(CONFIG_OTA_DTT_EU)
		case eSVCSI_ACT_SPEC_DTT:
			hResult = xsvc_si_SwupCheckFreeSwupCheckData_EuDtt(hAction, ulDataType, pvDataArray);
			break;
#endif

		default:
			HxLOG_Error("not supported action spec(%d) \n", pstSwupCheckSpec->ulActionSpec);
			return ERR_FAIL;
	}

	return hResult;

}

HERROR psvc_si_FindProperOuiDataFromLinkDes(SvcSi_SwupCheckSpec_t *pstSwupCheckSpec,SIxLinkDes_t *pstLinkDes)
{
	HERROR				hResult = ERR_FAIL;

	if (NULL == pstSwupCheckSpec)										{ return ERR_FAIL; }
	if (NULL == pstLinkDes)												{ return ERR_FAIL; }
	if (eSVCSI_ACT_KIND_SwupCheck != pstSwupCheckSpec->eActionKind)		{ return ERR_FAIL; }

	switch(pstSwupCheckSpec->ulActionSpec)
	{
#if defined(CONFIG_OTA_HUMAX)
	case eSVCSI_ACT_SPEC_Humax:
		break;
#endif

#if defined(CONFIG_OTA_IRDETO)
	case eSVCSI_ACT_SPEC_Irdeto:
		break;
#endif

#if defined(CONFIG_OTA_SES)
	case eSVCSI_ACT_SPEC_Ses:
		hResult = xsvc_si_SwupCheckFindProperOuiDataFromLinkDes_Ses(pstLinkDes);
		break;
#endif

#if defined(CONFIG_OTA_ASTRA)
	case eSVCSI_ACT_SPEC_Astra:
		hResult = xsvc_si_SwupCheckFindProperOuiDataFromLinkDes_Astra(pstLinkDes);
		break;
#endif

#if defined(CONFIG_OTA_DVBSSU)
	case eSVCSI_ACT_SPEC_DvbSsu:
#if defined(CONFIG_OP_ERTELECOM)
		hResult = xsvc_si_SwupCheckFindProperOuiDataFromLinkDes_Ertelecom(pstLinkDes);
#else
		hResult = xsvc_si_SwupCheckFindProperOuiDataFromLinkDes_Dvbssu(pstLinkDes);
#endif
		break;
#endif

#if defined(CONFIG_OTA_DTT_EU)
	case eSVCSI_ACT_SPEC_DTT:
		hResult = xsvc_si_SwupCheckFindProperOuiDataFromLinkDes_EuDtt(pstLinkDes);
		break;
#endif
	default:
		break;
	}

	return hResult;
}

HERROR psvc_si_SetExtSearchOption(Handle_t hAction, SvcSi_ExtSearchSpec_t *pstExtSrchSpec, SvcSi_ExtSrchOptionType_t eOptionType, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	HERROR				hResult = ERR_FAIL;

	if (NULL == pstExtSrchSpec)										{ return ERR_FAIL; }
	if (eSVCSI_ACT_KIND_ExtSearch != pstExtSrchSpec->eActionKind)	{ return ERR_FAIL; }

	switch(pstExtSrchSpec->ulActionSpec)
	{
#if defined(CONFIG_OP_SES)
		case eSVCSI_ACT_SPEC_Ses:
			hResult = xsvc_si_LcnUpdateSrchSetExtSrchData_Ses(hAction, eOptionType, ulParam1, ulParam2, ulParam3);
			break;
#endif

		default:
			HxLOG_Error("not supported action spec(%d) \n", pstExtSrchSpec->ulActionSpec);
			break;
	}

	return hResult;
}

HERROR psvc_si_GetExtSearchData(Handle_t hAction, SvcSi_ExtSearchSpec_t *pstExtSrchSpec, SvcSi_ExtSrchDataType_t eDataType, HUINT32 *pulDataNum, void **ppvDataArray)
{
	HERROR				hResult = ERR_FAIL;

	if (NULL == pstExtSrchSpec)										{ return ERR_FAIL; }
	if (NULL == pulDataNum)											{ return ERR_FAIL; }
	if (NULL == ppvDataArray)										{ return ERR_FAIL; }
	if (eSVCSI_ACT_KIND_ExtSearch != pstExtSrchSpec->eActionKind)	{ return ERR_FAIL; }

	switch(pstExtSrchSpec->ulActionSpec)
	{
#if defined(CONFIG_OP_SES)
		case eSVCSI_ACT_SPEC_Ses:
			hResult = xsvc_si_LcnUpdateSrchGetExtSrchData_Ses(hAction, eDataType, pulDataNum, ppvDataArray);
			break;
#endif

		default:
			HxLOG_Error("not supported action spec(%d) \n", pstExtSrchSpec->ulActionSpec);
			break;
	}

	return hResult;
}

HERROR psvc_si_FreeExtSearchData(Handle_t hAction, SvcSi_ExtSearchSpec_t *pstExtSrchSpec, SvcSi_ExtSrchDataType_t eDataType, void *pvDataArray)
{
	HERROR				hResult = ERR_FAIL;

	if (NULL == pstExtSrchSpec)										{ return ERR_FAIL; }
	if (NULL == pvDataArray)										{ return ERR_FAIL; }
	if (eSVCSI_ACT_KIND_ExtSearch != pstExtSrchSpec->eActionKind)	{ return ERR_FAIL; }

	switch(pstExtSrchSpec->ulActionSpec)
	{
#if defined(CONFIG_OP_SES)
		case eSVCSI_ACT_SPEC_Ses:
			hResult = xsvc_si_LcnUpdateSrchFreeExtSrchData_Ses(hAction, eDataType, pvDataArray);
			break;
#endif

		default:
			HxLOG_Error("not supported action spec(%d) \n", pstExtSrchSpec->ulActionSpec);
			break;
	}

	return hResult;
}

HERROR psvc_si_GetSatRecordData(Handle_t hAction, SvcSi_SatRecordSpec_t *pstSatRecordSpec, SvcSi_SatRecordDataType_t eDataType, HUINT32 *pulDataNum, void **ppvDataArray)
{
	HERROR				hResult = ERR_FAIL;

	if (NULL == pstSatRecordSpec)									{ return ERR_FAIL; }
	if (NULL == pulDataNum)											{ return ERR_FAIL; }
	if (NULL == ppvDataArray)										{ return ERR_FAIL; }
	if (eSVCSI_ACT_KIND_SatRecord != pstSatRecordSpec->eActionKind)	{ return ERR_FAIL; }

	switch(pstSatRecordSpec->ulActionSpec)
	{
#if defined(CONFIG_OP_SES)
		case eSVCSI_ACT_SPEC_Ses:
			hResult = xsvc_si_SatRecordGetSrchData_Ses(hAction, eDataType, pulDataNum, ppvDataArray);
			break;
#endif

		default:
			HxLOG_Error("not supported action spec(%d) \n", pstSatRecordSpec->ulActionSpec);
			break;
	}

	return hResult;
}

HERROR psvc_si_FreeSatRecordData(Handle_t hAction, SvcSi_SatRecordSpec_t *pstSatRecordSpec, SvcSi_SatRecordDataType_t eDataType, void *pvDataArray)
{
	HERROR				hResult = ERR_FAIL;

	if (NULL == pstSatRecordSpec)									{ return ERR_FAIL; }
	if (NULL == pvDataArray)										{ return ERR_FAIL; }
	if (eSVCSI_ACT_KIND_SatRecord != pstSatRecordSpec->eActionKind)	{ return ERR_FAIL; }

	switch(pstSatRecordSpec->ulActionSpec)
	{
#if defined(CONFIG_OP_SES)
		case eSVCSI_ACT_SPEC_Ses:
			hResult = xsvc_si_SatRecordFreeSrchData_Ses(hAction, eDataType, pvDataArray);
			break;
#endif

		default:
			HxLOG_Error("not supported action spec(%d) \n", pstSatRecordSpec->ulActionSpec);
			break;
	}

	return hResult;
}

HERROR psvc_si_GetSwupCheckFlag(Handle_t hAction, HUINT32 *pulSwupCheckFlag)
{
	HERROR	hErr = ERR_FAIL;

#if defined(CONFIG_OTA_IRDETO)
	hErr = xsvc_si_SwupCheckFlag_Irdeto(hAction, pulSwupCheckFlag);
#else
	*pulSwupCheckFlag = 0;
#endif

	return hErr;
}

HERROR psvc_si_MakeSiSection (HUINT32 ulActionId, SvcSi_MakeSectionSpec_t *pstSpec, SvcSi_MakeSecArg_t *punArg, HUINT8 *pucRawSec, HUINT32 *pulSecLen)
{
	HERROR				hResult = ERR_FAIL;

	if (NULL == pstSpec)									{ return ERR_FAIL; }
	if (NULL == punArg)										{ return ERR_FAIL; }
	if (NULL == pucRawSec)									{ return ERR_FAIL; }
	if (NULL == pulSecLen)									{ return ERR_FAIL; }

	switch (pstSpec->ulActionSpec)
	{
		case eSVCSI_ACT_SPEC_EuSat:
		case eSVCSI_ACT_SPEC_EuCab:
		case eSVCSI_ACT_SPEC_EuTer:
		case eSVCSI_ACT_SPEC_MeSat:
		case eSVCSI_ACT_SPEC_Ses:
		case eSVCSI_ACT_SPEC_Ertelecom:
		case eSVCSI_ACT_SPEC_Nordig:
		case eSVCSI_ACT_SPEC_AusDtt:
		case eSVCSI_ACT_SPEC_UkDtt:
		case eSVCSI_ACT_SPEC_Ziggo:
		case eSVCSI_ACT_SPEC_ItaDtt:
		case eSVCSI_ACT_SPEC_Orf:
		case eSVCSI_ACT_SPEC_Mbc:
		case eSVCSI_ACT_SPEC_AmxCenam:
			hResult = xsvc_si_LibMakeSiSection_Base(ulActionId, punArg, pucRawSec, pulSecLen);
			break;

		default:
			HxLOG_Error("invalid action kind (%d) \n", pstSpec->eActionKind);
			return ERR_FAIL;
	}

	return hResult;
}

HERROR	psvc_si_GetUndefinedServiceName(HUINT32 ulActionId, HUINT8 **ppszServiceName)
{
#if 1
	*ppszServiceName = (HUINT8 *)UNDEFINED_SERVICE_NAME;
	return ERR_OK;
#else
	SvcSi_SiSpec_t			 unSiSpec;
	HERROR					 hErr;

	hErr = svc_si_GetSiSpec (ulActionId, &unSiSpec);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_GetSiSpec err:\n");
		return ERR_FAIL;
	}

	switch (unSiSpec.stTemplate.eActionSpec)
	{
	case eSVCSI_ACT_SPEC_JpArib:
		*ppszServiceName = (HUINT8 *)"Unknown";
		hErr = ERR_OK;
		break;

	default:
		*ppszServiceName = (HUINT8 *)"";
		hErr = ERR_OK;
		break;
	}

	return hErr;
#endif
}

HERROR	psvc_si_GetUndefinedProviderName(HUINT32 ulActionId, HUINT8 **ppszProviderName)
{
#if 1
	*ppszProviderName = (HUINT8 *)UNDEFINED_SERVICE_NAME;
	return ERR_OK;
#else
	SvcSi_SiSpec_t			 unSiSpec;
	HERROR					 hErr;

	hErr = svc_si_GetSiSpec (ulActionId, &unSiSpec);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_GetSiSpec err:\n");
		return ERR_FAIL;
	}

	switch (unSiSpec.stTemplate.eActionSpec)
	{
	case eSVCSI_ACT_SPEC_JpArib:
		*ppszProviderName = (HUINT8 *)"Unknown";
		hErr = ERR_OK;
		break;

	default:
		*ppszProviderName = (HUINT8 *)"";
		hErr = ERR_OK;
		break;
	}

	return hErr;
#endif
}

HERROR	psvc_si_GetUndefinedNetworkName(HUINT32 ulActionId, HUINT8 **ppszNetworkName)
{
#if 1
	*ppszNetworkName = (HUINT8 *)UNDEFINED_NETWORK_NAME;
	return ERR_OK;
#else
	SvcSi_SiSpec_t			 unSiSpec;
	HERROR					 hErr;

	hErr = svc_si_GetSiSpec (ulActionId, &unSiSpec);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_GetSiSpec err:\n");
		return ERR_FAIL;
	}

	switch (unSiSpec.stTemplate.eActionSpec)
	{
	case eSVCSI_ACT_SPEC_JpArib:
		*ppszNetworkName = (HUINT8 *)"Unknown";
		hErr = ERR_OK;
		break;

	default:
		*ppszNetworkName = (HUINT8 *)"";
		hErr = ERR_OK;
		break;
	}

	return hErr;
#endif
}


HERROR	psvc_si_SetSiMode(HUINT32 ulSiMode)
{
	return ERR_FAIL;
}

HERROR	psvc_si_GetSiMode(HUINT32 *pulSiMode)
{
	return ERR_FAIL;
}
HERROR	psvc_si_GetSiPrivateSpecifyValue(HUINT32 ulActSpec, HUINT32 *pulPrivateSpecifyValue)
{
	HERROR hErr = ERR_FAIL;
	switch (ulActSpec)
	{

		case eSVCSI_ACT_SPEC_Freesat:
			*pulPrivateSpecifyValue = PRIVATE_DATA_SPECIFIER_FOR_UK_FSAT;
			hErr = ERR_OK;
			break;
		case eSVCSI_ACT_SPEC_UkDtt:
			*pulPrivateSpecifyValue = PRIVATE_DATA_SPECIFIER_FOR_UK_DTT;
			hErr = ERR_OK;
			break;
		case eSVCSI_ACT_SPEC_Nordig:
			*pulPrivateSpecifyValue = PRIVATE_DATA_SPECIFIER_FOR_NORDIG;
			hErr = ERR_OK;
			break;
		case eSVCSI_ACT_SPEC_Tdc:
			//*pulPrivateSpecifyValue = PRIVATE_DATA_SPECIFIER_FOR_YOUSEE;
			*pulPrivateSpecifyValue = PRIVATE_DATA_SPECIFIER_FOR_YOUSEE_TEST_STREAM;
			hErr = ERR_OK;
			break;

		case eSVCSI_ACT_SPEC_EuSat:
		case eSVCSI_ACT_SPEC_EuCab:
		case eSVCSI_ACT_SPEC_EuTer:
		case eSVCSI_ACT_SPEC_MeSat:
		case eSVCSI_ACT_SPEC_Ses:
		case eSVCSI_ACT_SPEC_Ertelecom:
		case eSVCSI_ACT_SPEC_AusDtt:
		case eSVCSI_ACT_SPEC_Ziggo:
		case eSVCSI_ACT_SPEC_ItaDtt:
		case eSVCSI_ACT_SPEC_Orf:
		case eSVCSI_ACT_SPEC_Mbc:
		case eSVCSI_ACT_SPEC_AmxCenam:
		default:
			*pulPrivateSpecifyValue = PRIVATE_DATA_SPECIFIER_UNDEFINED;
			break;
	}

	return hErr;
}
HBOOL	psvc_si_IsValidOui(HUINT32 ulOui)
{
	return ((ulOui == eSWUP_OUI_HUMAX) || (ulOui == eSWUP_OUI_DVB)) ? TRUE : FALSE;
}

HERROR psvc_si_GetAlternativeAudioPriority(const svcSi_AudioEs_t *pstAudioEs, HBOOL bLangMatched, HINT32 *pnUserPriority, HxLANG_Id_e *peLangId, DxDualMonoSelect_e *peDualMonoSelect)
{
#if defined(CONFIG_OP_UK_DTT)
	return xsvc_si_GetAlternativeAudioPriority_UkDtt(pstAudioEs, bLangMatched, pnUserPriority, peLangId, peDualMonoSelect);
#else
	return xsvc_si_GetAlternativeAudioPriority_Base(pstAudioEs, bLangMatched, pnUserPriority, peLangId, peDualMonoSelect);
#endif
}

#define ______TARGET_REGION_INFO_____
HERROR psvc_si_GetTrdConflictedList(Handle_t hAction, HUINT32 ulDepth,HUINT32 *pulconflictcount,HxList_t **ppConflictList)
{
	SvcSi_SiSpec_t			 unSiSpec;
	HERROR					 hErr = ERR_FAIL;
	HUINT32					 ulActionId = PAL_PIPE_GetActionId(hAction);

	if(NULL == ppConflictList)  { return ERR_FAIL; }

#if defined(CONFIG_OP_UK_DTT)
	hErr = xsvc_si_LcnGetTrdConflictedList_UkDtt(ulDepth,pulconflictcount,ppConflictList);
	return hErr;
#endif

	hErr = svc_si_GetSiSpec(ulActionId, &unSiSpec);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_GetSiSpec err:\n");
		return ERR_FAIL;
	}

	switch (unSiSpec.stTemplate.ulActionSpec)
	{
#if defined(CONFIG_OP_UK_DTT)
	case eSVCSI_ACT_SPEC_UkDtt:
		hErr = xsvc_si_LcnGetTrdConflictedList_UkDtt(ulDepth,pulconflictcount,ppConflictList);
		break;
#endif
	default:
		HxLOG_Error ("eActionSpec(%d) Not supported!!! \n", unSiSpec.stTemplate.ulActionSpec);
		hErr = ERR_FAIL;
		break;
	}

	return hErr;
}
HERROR psvc_si_SetTrdConflictedList(HUINT32 ulDepthIdx, HUINT32 index)
{
	HERROR					 hErr = ERR_FAIL;
#if defined(CONFIG_OP_UK_DTT)
	hErr = xsvc_si_LcnSetTrdConflictedList_UkDtt(ulDepthIdx, index);
#endif
	return hErr;
}
HERROR psvc_si_InitTrdConflicteSvcHandleTable(void)
{
		HERROR					 hErr = ERR_FAIL;
#if defined(CONFIG_OP_UK_DTT)
		hErr = xsvc_si_LcnInitConflictSvcHandleTable_UkDtt();
#endif
		return hErr;
}

HERROR psvc_si_SetNcdNotifyData(HUINT32 ulASONotifyFlag)
{
		HERROR					 hErr = ERR_FAIL;
#if defined(CONFIG_OP_UK_DTT)
		hErr = xsvc_si_SetNcdNotifyData_UkDtt(ulASONotifyFlag);
#endif
		return hErr;
}
HERROR psvc_si_ClearNcdNotifyData(void)
{
		HERROR					 hErr = ERR_FAIL;
#if defined(CONFIG_OP_UK_DTT)
		hErr = xsvc_si_ClearNcdNotifyData_UkDtt();
#endif
		return hErr;
}

#if defined(CONFIG_MW_SI_LOCALTIME)
HERROR psvc_si_UpdateLocaltime_linkTzDB(HxCountry_e eCountryId, HINT32 nTimeRegion)
{
	return xsvc_si_LibSvcEsUpdateLocaltime_linkTzDB(eCountryId, nTimeRegion);
}
#endif


#if defined(CONFIG_DEBUG)
Cutest_Suite_t *psvc_si_UtGetSuites(void)
{
	Cutest_Suite_t *pstSuite = NULL;

#if defined(CONFIG_OP_UK_DTT)
	pstSuite = xsvc_si_UtGetSuites_UkDtt();
#endif

	return pstSuite;
}
#endif


/* end of file */

