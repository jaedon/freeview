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

	// 2. Service Type�� �ٸ��� �񱳰� �ǹ̰� �����Ƿ� ����.
	if (DbSvc_GetSvcType(pstSvcSrc) != DbSvc_GetSvcType(pstSvcDest))
	{
		return ERR_FAIL;
	}

	// 3. Service Type�� Tv�� Radio�� �ƴϸ� �ǹ̰� �����Ƿ� ����.
	if ((DbSvc_GetSvcType(pstSvcSrc) != eDxSVC_TYPE_TV) && (DbSvc_GetSvcType(pstSvcSrc) != eDxSVC_TYPE_RADIO))
	{
		return ERR_FAIL;
	}

	// 4. Service Type�� Tv�� ��� OrgSvcType�� ���ؼ� HD�� LCN �Ҵ�.
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
	// 5. Service Type�� Radio�� ��� OrgSvcType�� ���ؼ� FM Radio�� LCN �Ҵ�.
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

	// 1. ��� Service List �� �����´�.
	DB_SVC_InitSvcFindCondition(&stSvcCond);

	hErr = DB_SVC_FindSvcInfoList (&stSvcCond, (HINT32*)&ulSvcNum, &pstSvcInfoArray);
	hErr |= DB_SVC_FindSvcHandleList (&stSvcCond, (HINT32*)&ulSvcNum, &phSvcList);
	if (hErr != ERR_OK)
	{
		goto END_FUNC;
	}

	HxLOG_Print("ulSvcNum : %d \n",ulSvcNum);

	// 2. OnId�� �ش��ϰ� LCN�� ����ϴ� Service �鿡 ���ؼ� Service Numbering �Ѵ�.
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

		// 2) LCN�� ���ų� Service Type�� Tv�� Radio�� �ƴ� ��� Skip
		if ((DbSvc_GetLcnFlag(pstSvcInfo) == FALSE)
			|| ((DbSvc_GetSvcType(pstSvcInfo) != eDxSVC_TYPE_TV) && (DbSvc_GetSvcType(pstSvcInfo) != eDxSVC_TYPE_RADIO)))
		{
			HxLOG_Print("\t\t-> skip, lcnflag(%d) svctype(0x%x)\n", DbSvc_GetLcnFlag(pstSvcInfo), DbSvc_GetSvcType(pstSvcInfo));
			continue;
		}

		// 3) LCN �ߺ��� �����ϴ��� Check
		for (i = 0; i < ulIndex; i++)
		{
			// LCN �ߺ��� �ƴϴ�. ���� Service��
			if (DbSvc_GetLcn(&pstSvcInfoArray[i]) != DbSvc_GetLcn(pstSvcInfo))
			{
				HxLOG_Debug("\t\t-> diff. lcn (%d:%d)\n", DbSvc_GetLcn(&pstSvcInfoArray[i]), DbSvc_GetLcn(pstSvcInfo));
				continue;
			}

			// Service Type�� �ٸ��� LCN�� �ߺ��� ����. ���� Service��
			if (DbSvc_GetSvcType(&pstSvcInfoArray[i]) != DbSvc_GetSvcType(pstSvcInfo))
			{
				HxLOG_Debug("\t\t-> diff. svctype (%x:%x)\n", DbSvc_GetSvcType(&pstSvcInfoArray[i]), DbSvc_GetSvcType(pstSvcInfo));
				continue;
			}

			// ���� Service�� ��쿡�� �ߺ��� �ƴϴ�. ���� Service��
			if (DbSvc_GetOnId(&pstSvcInfoArray[i]) == DbSvc_GetOnId(pstSvcInfo)
				&& DbSvc_GetTsId(&pstSvcInfoArray[i]) == DbSvc_GetTsId(pstSvcInfo)
				&& DbSvc_GetSvcId(&pstSvcInfoArray[i]) == DbSvc_GetSvcId(pstSvcInfo))
			{
				HxLOG_Debug("\t\t-> same service (%x:%x:%x)\n", DbSvc_GetOnId(pstSvcInfo), DbSvc_GetTsId(pstSvcInfo), DbSvc_GetSvcId(pstSvcInfo));
				continue;
			}

			// Service Type�� ������ Priority Check �� Non-Lcn �������� �̵�
			hErr = xsvc_si_LcnCheckPriority_Mbc(pstSvcInfo, &pstSvcInfoArray[i], &bHigher);
			if (hErr == ERR_OK && bHigher == TRUE)
			{
				// LCN�� �浹 �ϴ� ���� : Visible ���� �����Ѵ�.
				DbSvc_SetLcnFlag(&pstSvcInfoArray[i], FALSE);
				DbSvc_SetLcn(&pstSvcInfoArray[i], 0);
				DB_SVC_UpdateServiceInfo(phSvcList[i], &pstSvcInfoArray[i]);
			}
			else
			{
				// LCN�� �浹 �ϴ� ���� : Visible ���� �����Ѵ�.
				DbSvc_SetLcnFlag(pstSvcInfo, FALSE);
				DbSvc_SetLcn(pstSvcInfo, 0);
				DB_SVC_UpdateServiceInfo(phSvcList[ulIndex], pstSvcInfo);
			}

			break;
		}
	}

	// 3. Non-Lcn ������ ���� Service Numbering �Ѵ�.
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
	// 4. ����� Memory�� ���� �Ѵ�.
	DB_SVC_FreeServiceInfoList(NULL, phSvcList, pstSvcInfoArray);

#endif

	return hResult;
}


/* end of file */

