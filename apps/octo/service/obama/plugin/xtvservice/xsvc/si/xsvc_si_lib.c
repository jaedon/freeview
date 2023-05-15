/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_lib.c
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
#include <isosvc.h>

#include <svc_si.h>
#include <_svc_si.h>
#include <_svc_si_lib.h>
#include <_svc_si_filtering.h>
#include <_svc_si_search.h>

#include <_xsvc_si_lib.h>
#include <_xsvc_si_lib_search_base.h>

#include <_xsvc_si_search.h>

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

HERROR xsvc_si_LibSrchCollectServiceDataFromSi (svcSi_SrchData_t *pstSrch)
{
	HERROR			hErr = ERR_FAIL;

	switch(pstSrch->unSiSpec.stSrchSpec.ulActionSpec)
	{
		case eSVCSI_ACT_SPEC_EuSat:
		case eSVCSI_ACT_SPEC_EuCab:		/* fall through */
		case eSVCSI_ACT_SPEC_EuTer:		/* fall through */
			hErr = xsvc_si_LibSrchCollectServiceDataFromSi_Base(pstSrch);
			break;

#if defined(CONFIG_OP_MIDDLE_EAST)
		case eSVCSI_ACT_SPEC_MeSat:
			hErr = xsvc_si_LibSrchCollectServiceDataFromSi_Base(pstSrch);
			break;
#endif

#if defined(CONFIG_OP_SES)
		case eSVCSI_ACT_SPEC_Ses:
			hErr = xsvc_si_LibSrchCollectServiceDataFromSi_Base(pstSrch);
			break;
#endif

#if defined(CONFIG_OP_ERTELECOM)
		case eSVCSI_ACT_SPEC_Ertelecom:
			hErr = xsvc_si_SrchGetSearchedResultFromSi_Ertelecom((void*)pstSrch);
			break;
#endif

#if defined(CONFIG_OP_NORDIG)
		case eSVCSI_ACT_SPEC_Nordig:
			hErr = xsvc_si_SrchGetSearchedResultFromSi_Nordig((void*)pstSrch);
			break;
#endif

#if defined(CONFIG_OP_AUS_DTT)
		case eSVCSI_ACT_SPEC_AusDtt:
			hErr = xsvc_si_SrchCollectServiceDataFromSi_AusDtt((void*)pstSrch);
			break;
#endif

#if defined(CONFIG_OP_UK_DTT)
		case eSVCSI_ACT_SPEC_UkDtt:
			hErr = xsvc_si_SrchCollectServiceDataFromSi_UkDtt((void*)pstSrch);
			break;
#endif

#if defined(CONFIG_OP_ZIGGO)
		case eSVCSI_ACT_SPEC_Ziggo:
			hErr = xsvc_si_SrchCollectServiceDataFromSi_Ziggo((void*)pstSrch);
			break;
#endif

#if defined(CONFIG_OP_ITALY_DBOOK)
		case eSVCSI_ACT_SPEC_ItaDtt:
			hErr = xsvc_si_SrchCollectServiceDataFromSi_ItaDtt((void*)pstSrch);
			break;
#endif

#if defined(CONFIG_OP_ORF)
		case eSVCSI_ACT_SPEC_Orf:
			hErr = xsvc_si_LibSrchCollectServiceDataFromSi_Base(pstSrch);
			break;
#endif

#if defined(CONFIG_OP_MBC)
		case eSVCSI_ACT_SPEC_Mbc:
			hErr = xsvc_si_SrchGetSearchedResultFromSi_Mbc(pstSrch);
			break;
#endif

#if defined(CONFIG_OP_AMX)
		case eSVCSI_ACT_SPEC_AmxCenam:
			hErr = xsvc_si_LibSrchCollectServiceDataFromSi_Base(pstSrch);
			break;
#endif

		default:
			HxLOG_Error ("Action Spec (%d) Not Support !!!\n", pstSrch->unSiSpec.stSrchSpec.ulActionSpec);
			hErr = ERR_FAIL;
			break;
	}

	return hErr;
}


#define ___MEMBER_FUNCTIONS___

HERROR xsvc_si_LibSrchCheckAndComposeSearchData (svcSi_SrchData_t *pstSrch)
{
	HINT32			 nMsgType;
	HERROR			 hErr;

	if (svc_si_SrchCheckAllSiTableReceived (pstSrch) == TRUE)
	{
		hErr = xsvc_si_LibSrchCollectServiceDataFromSi (pstSrch);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("xsvc_si_LibSrchCollectServiceDataFromSi err:\n");
		}

		nMsgType = (ERR_OK == hErr) ? eSEVT_SI_SEARCH_DONE : eSEVT_SI_SEARCH_FAIL;
		svc_si_PostMsgToAp (nMsgType, pstSrch->hAction, 0, 0, 0);

		/* filter stop은 search stop시에 한다 */
	}

	return ERR_OK;
}

HERROR xsvc_si_LibSrchGetServiceOptype (HUINT32 ulActionId, DxOperator_e *peSvcOpType)
{
	HERROR				hErr = ERR_FAIL;
	SvcSi_SiSpec_t		unSiSpec;

	if (peSvcOpType == NULL)
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
	case eSVCSI_ACT_SPEC_Ertelecom:
	case eSVCSI_ACT_SPEC_Nordig:
	case eSVCSI_ACT_SPEC_AusDtt:
	case eSVCSI_ACT_SPEC_UkDtt:
	case eSVCSI_ACT_SPEC_Ziggo:
	case eSVCSI_ACT_SPEC_ItaDtt:
	case eSVCSI_ACT_SPEC_Orf:
	case eSVCSI_ACT_SPEC_Mbc:
	case eSVCSI_ACT_SPEC_AmxCenam:
		hErr = xsvc_si_LibSrchGetServiceOpType_Base (peSvcOpType);
		break;

#if defined(CONFIG_OP_SES)
	case eSVCSI_ACT_SPEC_Ses:
		hErr = xsvc_si_SrchSrchGetServiceOpType_Ses (ulActionId, peSvcOpType);
		break;
#endif

	default:
		HxLOG_Error ("eActionSpec(%d) Not supported!!! \n", unSiSpec.stTemplate.ulActionSpec);
		hErr = ERR_FAIL;
		break;
	}

	return ERR_OK;
}


/* end of file */

