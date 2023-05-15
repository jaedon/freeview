/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_lcn_ses.c
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
#include <hlib.h>
#include <silib.h>
#include <vkernel.h>
#include <octo_common.h>
#include <isosvc.h>
#include <pal_pipe.h>
#include <pal_sirawpool.h>
#include <db_svc.h>
#include <db_param.h>

#include <svc_si.h>
#include <_svc_si.h>
#include <_xsvc_si_lib_lcn_base.h>

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

#define ___MEMBER_FUNCTIONS___

#define ___API_FUNCTIONS___

// ============================================================================================
// Function : local_mwclcn_SetServiceNumber_ApsHdPlus
//	APS LCN이 적용되는 모델에서의 Non LCN 정책
//	1. Non-LCN Service만 있을 경우에는 100번부터 시작한다.
HERROR xsvc_si_LcnSetServiceNumber_Ses (Handle_t hAction, HUINT32 ulNum, DbSvc_Info_t *pstSvcArray, SRCHSVC_AppendFlag_t *peFlagArray)
{
	HUINT16			 usStartTvNum, usStartRadioNum;
	HUINT32			 ulIndex;
	DbSvc_Info_t		*pstSvcInfo;

	usStartTvNum = usStartRadioNum = 0;
	DB_SVC_GetLcnOrderNumber (DbSvc_GetDeliType(pstSvcArray[0]), eDxSVC_TYPE_TV, &usStartTvNum);
	DB_SVC_GetLcnOrderNumber (DbSvc_GetDeliType(pstSvcArray[0]), eDxSVC_TYPE_RADIO, &usStartRadioNum);

	/* base lcn 만큼 보정 */
	usStartTvNum += xsvc_si_LibLcnGetSvcNumBase (eDEFAULTLCN_APSLCN, DbSvc_GetDeliType(pstSvcArray[0]), eDxSVC_TYPE_TV);
	usStartRadioNum += xsvc_si_LibLcnGetSvcNumBase (eDEFAULTLCN_APSLCN, DbSvc_GetDeliType(pstSvcArray[0]), eDxSVC_TYPE_RADIO);

	for (ulIndex = 0; ulIndex < ulNum; ulIndex++)
	{
		if (peFlagArray[ulIndex] != eSRCHSVC_ADD &&
			peFlagArray[ulIndex] != eSRCHSVC_UPDATE_CHANGING_SVCNUM)
		{
			continue;
		}

		pstSvcInfo = &(pstSvcArray[ulIndex]);

		switch (DbSvc_GetSvcType(pstSvcInfo))
		{
		case eDxSVC_TYPE_TV:
			DbSvc_SetLcn(pstSvcInfo, usStartTvNum);
			usStartTvNum++;
			break;

		case eDxSVC_TYPE_RADIO:
			DbSvc_SetLcn(pstSvcInfo, usStartRadioNum);
			usStartRadioNum++;
			break;

		default:
			HxLOG_Critical("\n\n");
			HxLOG_Error("===> Strange Type %d\n", DbSvc_GetSvcType(pstSvcInfo));
			break;
		}
	}

	return ERR_OK;
}

/* end of file */

