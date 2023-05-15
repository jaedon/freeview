/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_lcn_mesat.c
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

#include "_xsvc_si_hdfoxirconnect.h"

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
// Function : local_si_lcn_SetServiceNumber_RayNormal
//	RAY의 기본적인 Non LCN Service의 Numbering 방식
//	1. Service가 Satellite나 Terrestrial일 경우에는 TV와 Radio가 둘 다 따로 번호대를 가진다.
//	2. Cable일 경우, TV와 Radio는 같은 번호대와 같은 서비스 영역을 공유한다. (1:TV, 2:TV, 3:Radio 식) --> 시나리오 확인후 수정 필요.
HERROR xsvc_si_LcnSetServiceNumber_HdFoxIrConnect(Handle_t hAction, HUINT32 ulNum, DbSvc_Info_t *pstSvcArray, SRCHSVC_AppendFlag_t *peFlagArray)
{
	HBOOL			 bStartChNumFound;
	HUINT16 		 usStartTvNum, usStartRadioNum, usStartChNum;
	HUINT32 		 ulIndex;
	DbSvc_Info_t		*pstSvcInfo;

	// Add 되는 Service 는 Numbering을 시작하도록 한다.
	//		- 위성과 지상파 채널의 경우 TV/Radio 구분이 있으므로 아래처럼 하면 빠름
	//		- 케이블의 경우 TV/Radio 구분이 없음 --> Cable의 경우에도 지상파, 위성처럼 수정 필요할 것 같음.
	if((DbSvc_GetDeliType(pstSvcArray[0]) == eDxDELIVERY_TYPE_SAT) || (DbSvc_GetDeliType(pstSvcArray[0]) == eDxDELIVERY_TYPE_TER))
	{
		usStartTvNum = usStartRadioNum = 0;
		DB_SVC_GetLcnOrderNumber (DbSvc_GetDeliType(pstSvcArray[0]), eDxSVC_TYPE_TV, &usStartTvNum);
		DB_SVC_GetLcnOrderNumber (DbSvc_GetDeliType(pstSvcArray[0]), eDxSVC_TYPE_RADIO, &usStartRadioNum);

		/* base lcn 만큼 보정 */
		usStartTvNum += xsvc_si_LibLcnGetSvcNumBase (eDEFAULTLCN_NORMAL, DbSvc_GetDeliType(pstSvcArray[0]), eDxSVC_TYPE_TV);
		usStartRadioNum += xsvc_si_LibLcnGetSvcNumBase (eDEFAULTLCN_NORMAL, DbSvc_GetDeliType(pstSvcArray[0]), eDxSVC_TYPE_RADIO);

		for (ulIndex = 0; ulIndex < ulNum; ulIndex++)
		{
			if (peFlagArray[ulIndex] != eSRCHSVC_ADD &&
				peFlagArray[ulIndex] != eSRCHSVC_UPDATE_CHANGING_SVCNUM)
			{
				continue;
			}

			pstSvcInfo = &(pstSvcArray[ulIndex]);
			switch (DbSvc_GetSvcTypePtr(pstSvcInfo))
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
				HxLOG_Error("===> Strange Type %d\n", DbSvc_GetSvcTypePtr(pstSvcInfo));
				break;
			}
		}
	}
	// 케이블에서는 TV/Radio는 Service Number 영역을 같이 쓴다. (지상파나 위성과 동일하게 처리 필요해 보임. 시나리오 확인 후 수정 필요.)
	else
	{
		usStartChNum = 0;
		bStartChNumFound = FALSE;

		for (ulIndex = 0; ulIndex < ulNum; ulIndex++)
		{
			pstSvcInfo = &(pstSvcArray[ulIndex]);

			// TV/RADIO 이외의 Service가 Count 되어서는 안된다.
			// LCN을 0xffff 로 놓고 넘어가다.
			if (DbSvc_GetSvcTypePtr(pstSvcInfo) != eDxSVC_TYPE_TV && DbSvc_GetSvcTypePtr(pstSvcInfo) != eDxSVC_TYPE_RADIO)
			{
				// Normal Numbering 시
				DbSvc_SetLcn(pstSvcInfo, 0xFFFF);
				continue;
			}

			// TV/Radio 서비스 : 같은 번호대를 공유한다.
			// 여기서 Start Channel Number 를 계산하는 이유는 Service List의 앞 쪽에 TV/Radio 이외의 Service가 왔을 경우
			// 그것들을 제대로 skip 하기 위한 것이다.
			if (bStartChNumFound == FALSE)
			{
				DB_SVC_GetLcnOrderNumberFromTVsAndRadios (DbSvc_GetDeliTypePtr(pstSvcInfo), &usStartChNum);
				usStartChNum += xsvc_si_LibLcnGetSvcNumBase (eDEFAULTLCN_NORMAL, DbSvc_GetDeliTypePtr(pstSvcInfo), eDxSVC_TYPE_TV);

				bStartChNumFound = TRUE;
			}

			if (peFlagArray[ulIndex] != eSRCHSVC_ADD &&
				peFlagArray[ulIndex] != eSRCHSVC_UPDATE_CHANGING_SVCNUM)
			{
				continue;
			}


			DbSvc_SetLcn(pstSvcInfo, usStartChNum);
			usStartChNum++;
		}
	}

	return ERR_OK;
}


/* end of file */

