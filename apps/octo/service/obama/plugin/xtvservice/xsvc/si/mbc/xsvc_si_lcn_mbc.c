/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_lcn_mbc.c
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
#define MBC_NON_LCN_START_NUMBER		1001

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

STATIC HERROR xsvc_si_LcnCheckPriority_Mbc(DbSvc_Info_t *pstSvcSrc, DbSvc_Info_t *pstSvcDest, HBOOL *pbHigher)
{
	// 1. Prameter Validation check.
	if (!pstSvcSrc) 	return ERR_FAIL;
	if (!pstSvcDest)	return ERR_FAIL;
	if (!pbHigher)		return ERR_FAIL;

	// 2. Service Type이 다르면 비교가 의미가 없으므로 리턴.
	if (DbSvc_GetSvcType(pstSvcSrc) != DbSvc_GetSvcType(pstSvcDest))
	{
		return ERR_FAIL;
	}

	// 3. Service Type이 Tv나 Radio가 아니면 의미가 없으므로 리턴.
	if ((DbSvc_GetSvcType(pstSvcSrc) != eDxSVC_TYPE_TV) && (DbSvc_GetSvcType(pstSvcSrc) != eDxSVC_TYPE_RADIO))
	{
		return ERR_FAIL;
	}

	// 4. Service Type이 Tv인 경우 OrgSvcType을 비교해서 HD에 LCN 할당.
	//	 - eDxOrgSVC_TYPE_AVC_HD_DIGITAL_TV (0x19) / eDxOrgSVC_TYPE_AVC_SD_DIGITAL_TV (0x16)
	if ((DbSvc_GetSvcType(pstSvcSrc) == eDxSVC_TYPE_TV)
		&& (DbSvc_GetOrgSvcType(pstSvcSrc) != DbSvc_GetOrgSvcType(pstSvcDest)))
	{
		if (DbSvc_GetOrgSvcType(pstSvcSrc) > DbSvc_GetOrgSvcType(pstSvcDest))
		{
			*pbHigher = TRUE;
		}
		else
		{
			*pbHigher = FALSE;
		}
	}
	// 5. Service Type이 Radio인 경우 OrgSvcType을 비교해서 FM Radio에 LCN 할당.
	//	 - eDxOrgSVC_TYPE_FM_RADIO (0x0A) / eDxOrgSVC_TYPE_DIGITAL_RADIO (0x02)
	else if ((DbSvc_GetSvcType(pstSvcSrc) == eDxSVC_TYPE_RADIO)
		&& (DbSvc_GetOrgSvcType(pstSvcSrc) != DbSvc_GetOrgSvcType(pstSvcDest)))
	{
		if (DbSvc_GetOrgSvcType(pstSvcSrc) > DbSvc_GetOrgSvcType(pstSvcDest))
		{
			*pbHigher = TRUE;
		}
		else
		{
			*pbHigher = FALSE;
		}
	}
	// 6. Service Priority Check.
	else
	{
		if (DbSvc_GetTsId(pstSvcSrc) == DbSvc_GetTsId(pstSvcDest))
		{
			if (DbSvc_GetSvcId(pstSvcSrc) == DbSvc_GetSvcId(pstSvcDest))
			{
				*pbHigher = FALSE;
			}
			else
			{
				if (DbSvc_GetSvcId(pstSvcSrc) < DbSvc_GetSvcId(pstSvcDest))
				{
					*pbHigher = TRUE;
				}
				else
				{
					*pbHigher = FALSE;
				}
			}
		}
		else
		{
			if (DbSvc_GetTsId(pstSvcSrc) < DbSvc_GetTsId(pstSvcDest))
			{
				*pbHigher = TRUE;
			}
			else
			{
				*pbHigher = FALSE;
			}
		}
	}

	return ERR_OK;
}


HERROR xsvc_si_LcnPostProcessForSvcNumbering_Mbc (DxDeliveryType_e eDeliType)
{
#if defined(CONFIG_OP_MBC)
	HERROR				hResult = ERR_FAIL;
	HERROR				hErr;
	HUINT32				ulSvcNum, ulIndex, i;
	Handle_t			*phSvcList;
	DbSvc_Info_t		*pstSvcInfoArray = NULL;
	DbSvc_Info_t		*pstSvcInfo = NULL;
	HBOOL				bHigher;
	HINT32				nLastTvNum = 0, nLastRadioNum = 0;
	HINT32				nStartNonLcnTvNum = 0, nStartNonLcnRadioNum = 0;
	DbSvc_Condition_t	stSvcCond;

	// 1. 모든 Service List 를 가져온다.
	DB_SVC_InitSvcFindCondition(&stSvcCond);

	hErr = DB_SVC_FindSvcInfoList (&stSvcCond, (HINT32*)&ulSvcNum, &pstSvcInfoArray);
	hErr |= DB_SVC_FindSvcHandleList (&stSvcCond, (HINT32*)&ulSvcNum, &phSvcList);
	if (hErr != ERR_OK)
	{
		goto END_FUNC;
	}

	HxLOG_Print("ulSvcNum : %d \n",ulSvcNum);

	// 2. OnId에 해당하고 LCN을 사용하는 Service 들에 대해서 Service Numbering 한다.
	for (ulIndex = 0; ulIndex < ulSvcNum; ulIndex++)
	{
		pstSvcInfo = &pstSvcInfoArray[ulIndex];

		HxLOG_Print("\t[%d] onid(0x%x), lcnflag(%d), svctype(0x%x), [%d. %s]\n", ulIndex
			, DbSvc_GetOnId(pstSvcInfo)
			, DbSvc_GetLcnFlag(pstSvcInfo)
			, DbSvc_GetSvcType(pstSvcInfo)
			, DbSvc_GetLcn(pstSvcInfo)
			, DbSvc_GetName(pstSvcInfo)
		);

		// 1) MBC Blocking Service
		if((DbSvc_GetLcnFlag(pstSvcInfo) == TRUE) && (DbSvc_GetVisibleFlag(pstSvcInfo) == FALSE))
		{
			if((DbSvc_GetSvcType(pstSvcInfo) == eDxSVC_TYPE_TV) || (DbSvc_GetSvcType(pstSvcInfo) == eDxSVC_TYPE_RADIO))
			{
				DbSvc_SetSvcType(pstSvcInfo, eDxSVC_TYPE_HIDDEN);
				DB_SVC_UpdateServiceInfo(phSvcList[ulIndex], pstSvcInfo);
				continue;
			}
		}

		// 2) LCN이 없거나 Service Type이 Tv나 Radio가 아닌 경우 Skip
		if ((DbSvc_GetLcnFlag(pstSvcInfo) == FALSE)
			|| ((DbSvc_GetSvcType(pstSvcInfo) != eDxSVC_TYPE_TV) && (DbSvc_GetSvcType(pstSvcInfo) != eDxSVC_TYPE_RADIO)))
		{
			HxLOG_Print("\t\t-> skip, lcnflag(%d) svctype(0x%x)\n", DbSvc_GetLcnFlag(pstSvcInfo), DbSvc_GetSvcType(pstSvcInfo));
			continue;
		}

		// 3) LCN 중복이 존재하는지 Check
		for (i = 0; i < ulIndex; i++)
		{
			// LCN 중복이 아니다. 다음 Service로
			if (DbSvc_GetLcn(&pstSvcInfoArray[i]) != DbSvc_GetLcn(pstSvcInfo))
			{
				HxLOG_Debug("\t\t-> diff. lcn (%d:%d)\n", DbSvc_GetLcn(&pstSvcInfoArray[i]), DbSvc_GetLcn(pstSvcInfo));
				continue;
			}

			// Service Type이 다르면 LCN이 중복이 허용됨. 다음 Service로
			if (DbSvc_GetSvcType(&pstSvcInfoArray[i]) != DbSvc_GetSvcType(pstSvcInfo))
			{
				HxLOG_Debug("\t\t-> diff. svctype (%x:%x)\n", DbSvc_GetSvcType(&pstSvcInfoArray[i]), DbSvc_GetSvcType(pstSvcInfo));
				continue;
			}

			// 같은 Service의 경우에는 중복이 아니다. 다음 Service로
			if (DbSvc_GetOnId(&pstSvcInfoArray[i]) == DbSvc_GetOnId(pstSvcInfo)
				&& DbSvc_GetTsId(&pstSvcInfoArray[i]) == DbSvc_GetTsId(pstSvcInfo)
				&& DbSvc_GetSvcId(&pstSvcInfoArray[i]) == DbSvc_GetSvcId(pstSvcInfo))
			{
				HxLOG_Debug("\t\t-> same service (%x:%x:%x)\n", DbSvc_GetOnId(pstSvcInfo), DbSvc_GetTsId(pstSvcInfo), DbSvc_GetSvcId(pstSvcInfo));
				continue;
			}

			// Service Type이 같으면 Priority Check 후 Non-Lcn 영역으로 이동
			hErr = xsvc_si_LcnCheckPriority_Mbc(pstSvcInfo, &pstSvcInfoArray[i], &bHigher);
			if (hErr == ERR_OK && bHigher == TRUE)
			{
				// LCN이 충돌 하는 서비스 : Visible 값은 유지한다.
				DbSvc_SetLcnFlag(&pstSvcInfoArray[i], FALSE);
				DbSvc_SetLcn(&pstSvcInfoArray[i], 0);
				DB_SVC_UpdateServiceInfo(phSvcList[i], &pstSvcInfoArray[i]);
			}
			else
			{
				// LCN이 충돌 하는 서비스 : Visible 값은 유지한다.
				DbSvc_SetLcnFlag(pstSvcInfo, FALSE);
				DbSvc_SetLcn(pstSvcInfo, 0);
				DB_SVC_UpdateServiceInfo(phSvcList[ulIndex], pstSvcInfo);
			}

			break;
		}
	}

	// 3. Non-Lcn 영역에 대한 Service Numbering 한다.
	if (ERR_OK != DB_SVC_GetMaxLcnInLcnServices(eDeliType, eDxSVC_TYPE_TV, &nLastTvNum))
	{
		nLastTvNum = 0;
	}

	if (ERR_OK != DB_SVC_GetMaxLcnInLcnServices(eDeliType, eDxSVC_TYPE_RADIO, &nLastRadioNum))
	{
		nLastRadioNum = 0;
	}

	HxLOG_Print("nLastTvNum = %d, nLastRadioNum = %d\n", nLastTvNum, nLastRadioNum);
	HxLOG_Print("call DB_SVC_RenumberSavingOrderToChNum\n");

	// Non-Lcn Area 
	if(nLastTvNum < MBC_NON_LCN_START_NUMBER)
	{
		nStartNonLcnTvNum = MBC_NON_LCN_START_NUMBER;
	}
	if(nLastRadioNum < MBC_NON_LCN_START_NUMBER)
	{
		nStartNonLcnRadioNum = MBC_NON_LCN_START_NUMBER;
	}

	DB_SVC_RenumberSavingOrderToChNum(eDeliType, eDxSVC_TYPE_TV, nStartNonLcnTvNum);
	DB_SVC_RenumberSavingOrderToChNum(eDeliType, eDxSVC_TYPE_RADIO, nStartNonLcnRadioNum);

	hResult = ERR_OK;

END_FUNC:
	// 4. 사용한 Memory는 해제 한다.
	DB_SVC_FreeServiceInfoList(NULL, phSvcList, pstSvcInfoArray);

#endif

	return hResult;
}


/* end of file */

