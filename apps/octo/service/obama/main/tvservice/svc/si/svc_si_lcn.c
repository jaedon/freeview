/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  svc_si_lcn.c
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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <silib.h>
#include <octo_common.h>
#include <isosvc.h>

#include <pal_sirawpool.h>
#include <pal_pipe.h>
#include <svc_si.h>
#include <db_param.h>
#include <otl.h>

#include <_svc_si.h>
#include <_svc_si_lib.h>
#include <_svc_si_lcn.h>
#include <psvc_si.h>



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
STATIC HERROR svc_si_LcnSvcNumCopyUserOptionsInSvcInfo (DbSvc_Info_t *pstDstSvcInfo, DbSvc_Info_t *pstSrcSvcInfo)
{
	if (pstDstSvcInfo == NULL || pstSrcSvcInfo == NULL)
	{
		return ERR_FAIL;
	}

	// User 사양인 내용물을 Copy 해둔다.
	if (DbSvc_GetRenamed(pstSrcSvcInfo) == TRUE)
	{
		DbSvc_SetName(pstDstSvcInfo, DbSvc_GetName(pstSrcSvcInfo));
	}

	DbSvc_SetRenamed(pstDstSvcInfo, DbSvc_GetRenamed(pstSrcSvcInfo));
	DbSvc_SetLocked(pstDstSvcInfo, DbSvc_GetLocked(pstSrcSvcInfo));
	DbSvc_SetSubttlIdx(pstDstSvcInfo, DbSvc_GetSubttlIdx(pstSrcSvcInfo));
	DbSvc_SetStereoSelect(pstDstSvcInfo, DbSvc_GetStereoSelect(pstSrcSvcInfo));
	DbSvc_SetDualmonoSelect(pstDstSvcInfo, DbSvc_GetDualmonoSelect(pstSrcSvcInfo));
	DbSvc_SetAudUserSelected(pstDstSvcInfo, DbSvc_GetAudUserSelected(pstSrcSvcInfo));

	// Service 번호도 덮어져야 한다.
	// 새 Service 와 기존 Service가 둘 다 Non-LCN 일 때에만 해당된다.
	if (DbSvc_GetLcnFlag(pstDstSvcInfo) == FALSE && DbSvc_GetLcnFlag(pstSrcSvcInfo) == FALSE)
	{
		DbSvc_SetLcnFlag(pstDstSvcInfo, DbSvc_GetLcnFlag(pstSrcSvcInfo));
		DbSvc_SetLcn(pstDstSvcInfo, DbSvc_GetLcn(pstSrcSvcInfo));
	}

	return ERR_OK;
}

STATIC HBOOL svc_si_LcnSvcNumIsSvcInfoNeedToUpdate (DbSvc_Info_t *pstDstSvcInfo, DbSvc_Info_t *pstSrcSvcInfo, HBOOL bDynamicUpdate)
{
	// Name 이 바뀌었는가?
	if (MWC_UTIL_DvbStrncmp (DbSvc_GetName(pstDstSvcInfo), DbSvc_GetName(pstSrcSvcInfo), DbSvc_GetNameLength()) != 0)
	{
		return TRUE;
	}

	// LCN에 해당되는가?
	if ((DbSvc_GetLcnFlag(pstDstSvcInfo) != DbSvc_GetLcnFlag(pstSrcSvcInfo)) ||
		(DbSvc_GetLcn(pstDstSvcInfo) != DbSvc_GetLcn(pstSrcSvcInfo)) ||
		(DbSvc_GetVisibleFlag(pstDstSvcInfo) != DbSvc_GetVisibleFlag(pstSrcSvcInfo)))
	{
		return TRUE;
	}

	// Service Attribute가  전혀 다르다
	if ( (DbSvc_GetSvcType(pstDstSvcInfo) != DbSvc_GetSvcType(pstSrcSvcInfo))
		|| (DbSvc_GetVideoType(pstDstSvcInfo) != DbSvc_GetVideoType(pstSrcSvcInfo))
		|| ( (DbSvc_GetCasType(pstDstSvcInfo) != eDxCAS_TYPE_All) && (DbSvc_GetCasType(pstDstSvcInfo) != DbSvc_GetCasType(pstSrcSvcInfo)) )
		|| (DbSvc_GetDeliTypePtr(pstDstSvcInfo) != DbSvc_GetDeliTypePtr(pstSrcSvcInfo))
#if defined(CONFIG_MW_CH_SATELLITE)
		|| (DbSvc_GetSatType(pstDstSvcInfo) != DbSvc_GetSatType(pstSrcSvcInfo))
		|| (DbSvc_GetAntIdx(pstDstSvcInfo) != DbSvc_GetAntIdx(pstSrcSvcInfo))
#endif
		)
	{
		return TRUE;
	}

	// 삭제된 서비스는 서치하면 다시 살린다 - OCTO 2 기본 사양
	if ((bDynamicUpdate == FALSE) && (DbSvc_GetSvcRemoved(pstSrcSvcInfo) == TRUE))
	{
		// 삭제 flag 지워줌
		DbSvc_SetSvcRemoved(pstDstSvcInfo, FALSE);
		return TRUE;
	}

	return FALSE;
}


#define ___MEMBER_FUNCTIONS___

HERROR svc_si_LcnSetServiceNumbers (Handle_t hAction, HUINT32 ulActSpec, HBOOL bDynamicUpdate, HBOOL bIsNewTp, HUINT32 ulNum, DbSvc_Info_t *pstSvcArray, SRCHSVC_AppendFlag_t *peFlagArray)
{
	HINT32			 	nDbSvcNum;
	HUINT32			 	ulIndex, ulSubIndex;
	DbSvc_Info_t		*pstSvcInfo, *pstPrevSvcInfo;
	DbSvc_Info_t		*pstDbSvcArray;
	DbSvc_Condition_t	 stSvcCond;
	HERROR			 	hErr;
//	DxOperator_e	eOperatorType;

	if (ulNum == 0 || pstSvcArray == NULL || peFlagArray == NULL)
	{
		HxLOG_Error("ulNum == 0 || pstSvcArray == NULL || peFlagArray == NULL \n");
		return ERR_FAIL;
	}

	// 1. 모든 Service는 추가되는 것을 기본으로 한다.
	for (ulIndex = 0; ulIndex < ulNum; ulIndex++)
	{
		peFlagArray[ulIndex] = eSRCHSVC_ADD;
	}

	// 2. 같은 리스트 중에도 같은 서비스가 있는 지 체크해야 한다.
	for (ulIndex = 1; ulIndex < ulNum; ulIndex++)
	{
		pstSvcInfo = &(pstSvcArray[ulIndex]);

		for (ulSubIndex = 0; ulSubIndex < ulIndex; ulSubIndex++)
		{
			pstPrevSvcInfo = &(pstSvcArray[ulSubIndex]);

			// TS IDX와 SVC ID가 같다 : 같은 서비스이므로 추가하지 마라
			if ((DbSvc_GetTsIdx(pstPrevSvcInfo) == DbSvc_GetTsIdx(pstSvcInfo)) &&
				(DbSvc_GetSvcId(pstPrevSvcInfo) == DbSvc_GetSvcId(pstSvcInfo)))
			{
				peFlagArray[ulSubIndex] = eSRCHSVC_DONTADD;
			}
		}
	}

	/* << 여기서 분기 >> */
	// 3. 각 모델별 정책에 따라 LCN 번호 할당 처리를 한다.
	hErr = psvc_si_SetLcnServiceNumber(hAction, ulActSpec, ulNum, pstSvcArray, peFlagArray);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("psvc_si_SetLcnServiceNumber() failed.. actSpec(%d) \n", ulActSpec);
	}

	// 4. 같은 Service가 DB에 존재하는 지 체크한다.
	if (bIsNewTp == FALSE)
	{
		for (ulIndex = 0; ulIndex < ulNum; ulIndex++)
		{
			pstSvcInfo = &(pstSvcArray[ulIndex]);

			nDbSvcNum = 0;
			pstDbSvcArray = NULL;

			DB_SVC_InitSvcFindCondition (&stSvcCond);
			stSvcCond.nOrgDeliType	= (HINT32)DbSvc_GetDeliTypePtr(pstSvcInfo);
			if (DbSvc_GetDeliTypePtr(pstSvcInfo) == eDxDELIVERY_TYPE_SAT)
			{
				stSvcCond.nAntIdx	= (HINT32)DbSvc_GetAntIdx(pstSvcInfo);
			}

			stSvcCond.nOnId 		= (HINT32)DbSvc_GetOnId(pstSvcInfo);
			stSvcCond.nTsId 		= (HINT32)DbSvc_GetTsId(pstSvcInfo);
			stSvcCond.nSvcId		= (HINT32)DbSvc_GetSvcId(pstSvcInfo);
			stSvcCond.nLcnFlag		= (HINT32)DbSvc_GetLcnFlag(pstSvcInfo);
			stSvcCond.eSvcOpType	= (HINT32)DbSvc_GetOpType(pstSvcInfo);

			hErr = DB_SVC_FindSvcInfoList (&stSvcCond, &nDbSvcNum, &pstDbSvcArray);
			if (hErr == ERR_OK && nDbSvcNum > 0)
			{
				if (nDbSvcNum >= 2)
				{
					HxLOG_Warning("Something Wrong!!! Same Services (Num:%d) Exist!!!\n", nDbSvcNum);
				}

				// Same Service Exist :
				svc_si_LcnSvcNumCopyUserOptionsInSvcInfo (pstSvcInfo, &(pstDbSvcArray[0]));

				if (svc_si_LcnSvcNumIsSvcInfoNeedToUpdate (pstSvcInfo, &(pstDbSvcArray[0]), bDynamicUpdate) == TRUE)
				{
					// Service Type이 바뀌면 Numbering도 다시 해야 한다.
					if (DbSvc_GetSvcType(&pstDbSvcArray[0]) != DbSvc_GetSvcType(pstSvcInfo))
					{
						peFlagArray[ulIndex] = eSRCHSVC_UPDATE_CHANGING_SVCNUM;
					}
					else
					{
						peFlagArray[ulIndex] = eSRCHSVC_UPDATE;
					}
				}
				else
				{
					peFlagArray[ulIndex] = eSRCHSVC_DONTADD;
				}
			}

			if (pstDbSvcArray != NULL)
			{
				DB_SVC_FreeServiceInfoList (NULL, NULL, pstDbSvcArray);
				pstDbSvcArray = NULL;
			}
		}
	}

	// Update Service Numbering을 다 Update로 수정한다.
	for (ulIndex = 0; ulIndex < ulNum; ulIndex++)
	{
		if (peFlagArray[ulIndex] == eSRCHSVC_UPDATE_CHANGING_SVCNUM)
		{
			peFlagArray[ulIndex] = eSRCHSVC_UPDATE;
		}
	}

	return ERR_OK;
}

HERROR svc_si_LcnSetSelectedConflictLcn (Handle_t hAction, HUINT32 ulNum, DxBuilder_SearchedInfo_t* pstInfoArray)
{
	HERROR hErr = ERR_FAIL;

	hErr = psvc_si_SetSelectedConflictLcn(hAction, ulNum, pstInfoArray);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("psvc_si_SetSelectedConflictLcn() failed.. \n");
	}

	return hErr;
}


/* end of file */

