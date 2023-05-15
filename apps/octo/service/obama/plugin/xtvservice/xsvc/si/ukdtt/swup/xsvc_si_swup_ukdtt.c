/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_swup_ukdtt.c
	@brief

	Description:  									\n
	Module: Porting Layer/TV Service/SI /SWUP CHECK	\n

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
#include <silib.h>
#include <octo_common.h>
#include <isosvc.h>
#include <pal_pipe.h>
#include <pal_sirawpool.h>
#include <pal_sys.h>
#include <pal_param.h>

#include <db_svc.h>
#include <svc_si.h>
#include <xsvc_si.h>
#include <svc_swup.h>
#include <_svc_si.h>
#include <_svc_si_lib.h>
#include <_svc_si_filtering.h>
#include <_svc_si_swup.h>

#include <_xsvc_si_lib_swup_base.h>
#include <_xsvc_si_swup_base.h>
#include <_xsvc_si_swup.h>
#include <db_param.h>

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static variable     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Functions     ********************/
#if defined(CONFIG_DEBUG)
STATIC const HINT8 * xsvc_si_SwupCheckMakeStrMode_UkDtt(XsvcSi_SwupMode_e eSwupMode)
{
	switch (eSwupMode)
	{
		ENUM_TO_STR(eSI_SWUP_MODE_DVBSSU_HOME_TP);
		ENUM_TO_STR(eSI_SWUP_MODE_DVBSSU_SSUSVC_TP);
		default:
			break;
	}
	return ("Unknown");

}
#endif
/*******************************************************************/

HERROR xsvc_si_SwupCheckGetFilteringTypes_UkDtt(xsvcSi_SwupData_t *pstSwupCheck)
{
	if (NULL == pstSwupCheck)
	{
		HxLOG_Error ("[%s][%d] pstSwupCheck is NULL\n",  __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Print("[%s][%d]ulSwupMode(%s) \n",  __FUNCTION__, __LINE__, xsvc_si_SwupCheckMakeStrMode_UkDtt(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif

	if(pstSwupCheck->stBase.bLiveSwupCheck == TRUE)
	{
		HxLOG_Error ("bCheckInLive is TRUE\n");
		return ERR_FAIL;
	}
	
	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
	case eSI_SWUP_MODE_DVBSSU_HOME_TP:
		pstSwupCheck->stBase.eFiltering = bSI_FILTERING_NIT_ACT ;

		if (pstSwupCheck->usNetIdForNitOth)
		{
			pstSwupCheck->stBase.eFiltering |= bSI_FILTERING_NIT_OTH;
		}
		break;

	case eSI_SWUP_MODE_DVBSSU_SSUSVC_TP:
		pstSwupCheck->stBase.eFiltering = bSI_FILTERING_PAT;
		break;

	default:
		pstSwupCheck->stBase.eFiltering = 0;
		return ERR_FAIL;
	}
	return ERR_OK;
}

void xsvc_si_SwupCheckMsgEvtNitTable_UkDtt(xsvcSi_SwupData_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HERROR	hErr;
	SIxTable_Type_e eSTableType;

	if (pstSwupCheck == NULL)
	{
		HxLOG_Error ("[%s][%d] pstSwupCheck is NULL\n",  __FUNCTION__, __LINE__);
		return ;
	} 

#if defined(CONFIG_DEBUG)
	HxLOG_Print("[%s][%d] ulSwupMode (%s) \n" , __FUNCTION__, __LINE__, xsvc_si_SwupCheckMakeStrMode_UkDtt(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif

	if(pstMsg->eMsgClass == eSIMSG_EVT_NIT_TABLE)
		eSTableType = eSIxTABLETYPE_NitAct;
	else
		eSTableType = eSIxTABLETYPE_NitOth;

	switch (pstSwupCheck->stBase.stSwupOption.ulSwupMode)
	{
		case eSI_SWUP_MODE_DVBSSU_HOME_TP:
			{
				hErr = xsvc_si_LibSwupOnSiTableReceived_Base(&pstSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));
				if(hErr != ERR_OK)
				{
					HxLOG_Error ("xsvc_si_LibSwupOnSiTableReceived_Base Error ( send message 'eSEVT_SWUP_DETECT_FAIL')\n");
					svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
					return ;
				}

				// SSU Linkage 정보를 찾는다.
				hErr = svc_si_SwupCheckFindLinkage(&pstSwupCheck->stBase, eSTableType, eLINKAGE_ALL, NULL, NULL);
				if (hErr != ERR_OK)
				{				
					HxLOG_Error ("svc_si_SwupCheckFindLinkage Error ( send message 'eSEVT_SWUP_DETECT_FAIL') \n");
					svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_FAIL, pstSwupCheck->stBase.hAction, 0, 0, 0);
					return ;
				}

				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_SUCCESS, pstSwupCheck->stBase.hAction, 0, 0, 0);
			}
			break;

		default:
#if defined(CONFIG_DEBUG)
			HxLOG_Error("Non-processing ulSwupMode (%s) -> skipped:\n", xsvc_si_SwupCheckMakeStrMode_UkDtt(pstSwupCheck->stBase.stSwupOption.ulSwupMode));
#endif
		break;
	}
	
	return ;
}

HERROR xsvc_si_SwupCheckMakeUntExtIdByOui_UkDtt(HUINT16	*pusUntExtId)
{
	HERROR	hErr = ERR_FAIL;
	
	hErr = xsvc_si_LibSwupMakeUntExtIdByOui_Base(eSWUP_OUI_DVB, 1, pusUntExtId);

	return hErr;
}
/* end of file */

