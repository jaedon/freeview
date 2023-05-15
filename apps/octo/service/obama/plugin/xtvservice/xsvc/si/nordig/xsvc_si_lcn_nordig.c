/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_lcn_nordig.c
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
//#define LABWISE_CABLE_TEST


#define TELENOR_CDK_ONID			(70)
#define TELENOR_B2_ONID				(100)
#define COMHEM_NETWORK_ONID			(40999)
#define LABWISE_CABLE_TEST_ONID		(0x20F6)
#define TDC_YOUSEE_ONID				(0xFE00)	// TeleDenmark Cable TV

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

STATIC HERROR xsvc_si_LcnGetLogicalChannelFromNit_Nordig (HUINT32 ulActionId, HxList_t **ppChList)
{
	HxList_t					*pstItem1, *pstItem2;
	SIxTable_PsiTable_t		*pstNitTable = NULL;
	HERROR					 hResult = ERR_FAIL;
	HxList_t 					*pChList = NULL;

	if (NULL == ppChList)
	{
		HxLOG_Error ("Invalid paremeters \n");
		goto END_FUNC;
	}

	pstNitTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_NitAct, SILIB_TABLE_EXT_ID_ALL);
	if (pstNitTable == NULL)
	{
		HxLOG_Error ("Invalid pstNitTable \n");
		goto END_FUNC;
	}

	for (pstItem1 = pstNitTable->pst2ndLoop; NULL != pstItem1; pstItem1 = pstItem1->next)
	{
		SIxTable_2ndLoop_t *pst2ndLoop = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstItem1);

		for (pstItem2 = pst2ndLoop->pstDesLoop; NULL != pstItem2; pstItem2 = pstItem2->next)
		{
			HUINT8 *pucDesRaw = (HUINT8 *)HLIB_LIST_Data (pstItem2);

			switch (*pucDesRaw)
			{
				case eSIxDESCTAG_LOGICAL_CHANNEL:
				{
					SIxNordigLogicalChannelDes_t	*pstLogicChDes = (SIxNordigLogicalChannelDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);
					if (NULL != pstLogicChDes)
					{
						SIxNordigLogicalChannelInfo_t *pChListItem = pstLogicChDes->pstInfo;

						while(pChListItem)
						{
							xsvcSi_LcnInfo_Nordig_t *pstLcnInfo = NULL;

							pstLcnInfo = (xsvcSi_LcnInfo_Nordig_t*)OxSI_Malloc(sizeof(xsvcSi_LcnInfo_Nordig_t));
							if (pstLcnInfo != NULL)
							{
								pstLcnInfo->usTsId = pst2ndLoop->unInfo.stNit.usTsId;
								pstLcnInfo->usOnId = pst2ndLoop->unInfo.stNit.usOrgNetId;

								pstLcnInfo->usServiceId = pChListItem->usServiceId;
								pstLcnInfo->ucVisibleServiceFlag = TRUE;	/* always true in Nordig */
								pstLcnInfo->usLogicalChannelNumber = pChListItem->usLogicalChannelNumber;

								HxLOG_Info("LCN Info (0x%04x:0x%04x:0x%04x) visible_service_flag : %s, LCN : %d\n", pstLcnInfo->usOnId, pstLcnInfo->usTsId, pstLcnInfo->usServiceId, pstLcnInfo->ucVisibleServiceFlag ? "TRUE " : "FALSE", pstLcnInfo->usLogicalChannelNumber);
								pChList = HLIB_LIST_Append(pChList, (void*)pstLcnInfo);
							}
							pChListItem = pChListItem->pstNext;
						}

						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstLogicChDes);
					}
				}
				break;

			default:
				break;
			}
		}
	}

	*ppChList = pChList;
	hResult = ERR_OK;

END_FUNC:
	if (NULL != pstNitTable)	{ PAL_SIRAWPOOL_FreeTable (pstNitTable); }

	return hResult;
}

HERROR xsvc_si_LcnSetServiceNumber_Nordig (Handle_t hAction, HUINT32 ulNum, DbSvc_Info_t *pstSvcArray, SRCHSVC_AppendFlag_t *peFlagArray)
{
	HINT32			 	nStartTvNum, nStartRadioNum;
	HUINT32			 	ulIndex;
	DbSvc_Info_t		*pstSvcInfo;

	/* LCN은 이미 추가 된 상태이므로, non-LCN 서비스에 대해서만 처리해 주자 */
	nStartTvNum = nStartRadioNum = 0;
	if (ERR_OK != DB_SVC_GetMaxLcnInLcnServices(DbSvc_GetDeliType(pstSvcArray[0]), eDxSVC_TYPE_TV, &nStartTvNum))
	{
		nStartTvNum = 0;
	}
	if (ERR_OK != DB_SVC_GetMaxLcnInLcnServices(DbSvc_GetDeliType(pstSvcArray[0]), eDxSVC_TYPE_RADIO, &nStartRadioNum))
	{
		nStartRadioNum = 0;
	}
	HxLOG_Print(HxANSI_COLOR_YELLOW("(1) nStartTvNum = %d, nStartRadioNum = %d\n"), nStartTvNum, nStartRadioNum);

	/* base lcn 만큼 보정 */
	nStartTvNum += xsvc_si_LibLcnGetSvcNumBase (eDEFAULTLCN_NORDIG, DbSvc_GetDeliType(pstSvcArray[0]), eDxSVC_TYPE_TV);
	nStartRadioNum += xsvc_si_LibLcnGetSvcNumBase (eDEFAULTLCN_NORDIG, DbSvc_GetDeliType(pstSvcArray[0]), eDxSVC_TYPE_RADIO);
	HxLOG_Print(HxANSI_COLOR_YELLOW("(2) nStartTvNum = %d, nStartRadioNum = %d\n"), nStartTvNum, nStartRadioNum);

	for (ulIndex = 0; ulIndex < ulNum; ulIndex++)
	{
		if (peFlagArray[ulIndex] != eSRCHSVC_ADD &&
			peFlagArray[ulIndex] != eSRCHSVC_UPDATE_CHANGING_SVCNUM)
		{
			continue;
		}

		pstSvcInfo = &(pstSvcArray[ulIndex]);
		if(NULL == pstSvcInfo)
		{
			HxLOG_Critical("(%d) pstSvcInfo is NULL\n", ulIndex);
			continue;
		}

		if (TRUE != DbSvc_GetLcnFlag(pstSvcInfo))
		{
			switch (DbSvc_GetSvcType(pstSvcInfo))
			{
			// 2015-04-10 Search Flow 변경으로 Save 직전에 LCN 부여하도록 수정되어 임시로 0 부여
			case eDxSVC_TYPE_TV:
				//DbSvc_SetLcn(pstSvcInfo, (HUINT16)nStartTvNum);
				DbSvc_SetLcn(pstSvcInfo, 0);
				nStartTvNum++;
				break;

			case eDxSVC_TYPE_RADIO:
				//DbSvc_SetLcn(pstSvcInfo, (HUINT16)nStartRadioNum);
				DbSvc_SetLcn(pstSvcInfo, 0);
				nStartRadioNum++;
				break;

			default:
				HxLOG_Critical("\n\n");
				HxLOG_Error("===> Strange Type %d\n", DbSvc_GetSvcType(pstSvcInfo));
				break;
			}
		}

		HxLOG_Info("peFlagArray[%d] = %d, Lcn(%d), %d. %s\n", ulIndex, peFlagArray[ulIndex], DbSvc_GetLcnFlag(pstSvcInfo), DbSvc_GetLcn(pstSvcInfo), DbSvc_GetName(pstSvcInfo));
	}

	return ERR_OK;
}

STATIC HERROR xsvc_si_LcnCheckPriority_Nordig(DbSvc_Info_t *pstSvcSrc, DbSvc_Info_t *pstSvcDest, HBOOL *pbHigher)
{
	// 1. Prameter Validation check.
	if (!pstSvcSrc)		return ERR_FAIL;
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
	//   - eDxOrgSVC_TYPE_AVC_HD_DIGITAL_TV (0x19) / eDxOrgSVC_TYPE_AVC_SD_DIGITAL_TV (0x16)
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
	//   - eDxOrgSVC_TYPE_FM_RADIO (0x0A) / eDxOrgSVC_TYPE_DIGITAL_RADIO (0x02)
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

HERROR xsvc_si_LcnPostProcessForSvcNumbering_Nordig (DxDeliveryType_e eDeliType)
{
#if defined(CONFIG_OP_NORDIG)
	HERROR				hErr;
	HUINT32				ulSvcNum, ulIndex, i;
	Handle_t			*phSvcList;
	DbSvc_Info_t		*pstSvcInfoArray, *pstSvcInfo;
	HBOOL				bHigher;
	HINT32				nLastTvNum = 0, nLastRadioNum = 0;
	DbSvc_Condition_t	stSvcCond;


	// 1. 모든 Service List 를 가져온다.
	DB_SVC_InitSvcFindCondition(&stSvcCond);

	hErr = DB_SVC_FindSvcInfoList (&stSvcCond, (HINT32*)&ulSvcNum, &pstSvcInfoArray);
	hErr |= DB_SVC_FindSvcHandleList (&stSvcCond, (HINT32*)&ulSvcNum, &phSvcList);
	if (hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	HxLOG_Print("ulSvcNum : %d \n",ulSvcNum);

	// 2. OnId에 해당하고 LCN을 사용하는 Service 들에 대해서 Service Numbering 한다.
	for (ulIndex = 0; ulIndex < ulSvcNum; ulIndex++)
	{
		pstSvcInfo = &pstSvcInfoArray[ulIndex];

		HxLOG_Print("\t[%d] onid(0x%x), check_onid(%d) lcnflag(%d), svctype(0x%x), [%d. %s]\n", ulIndex
			, DbSvc_GetOnId(pstSvcInfo)
			, xsvc_si_LcnCheckOnId_Nordig(DbSvc_GetOnId(pstSvcInfo))
			, DbSvc_GetLcnFlag(pstSvcInfo)
			, DbSvc_GetSvcType(pstSvcInfo)
			, DbSvc_GetLcn(pstSvcInfo)
			, DbSvc_GetName(pstSvcInfo)
		);

		// 1) OnId에 해당하지 않는 것은 Non-Lcn 영역으로 이동시키고 Skip
		// Telenor OnId : 100(0x64) or 70(0x46)
		if(xsvc_si_LcnCheckOnId_Nordig(DbSvc_GetOnId(pstSvcInfo)) == FALSE)
		{
			// ONID가 일치하지 않는 서비스 : Visible값은 TRUE로 처리한다. (ONID가 일치하지 않는 서비스는 LCD를 받지 않았으니까)
			DbSvc_SetVisibleFlag(pstSvcInfo, TRUE);
			DbSvc_SetLcnFlag(pstSvcInfo, FALSE);
			DbSvc_SetLcn(pstSvcInfo, 0);
			DB_SVC_UpdateServiceInfo(phSvcList[ulIndex], pstSvcInfo);
			continue;
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
			hErr = xsvc_si_LcnCheckPriority_Nordig(pstSvcInfo, &pstSvcInfoArray[i], &bHigher);
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

	DB_SVC_RenumberSavingOrderToChNum(eDeliType, eDxSVC_TYPE_TV, nLastTvNum + 1);
	DB_SVC_RenumberSavingOrderToChNum(eDeliType, eDxSVC_TYPE_RADIO, nLastRadioNum + 1);

	// 4. 사용한 Memory는 해제 한다.
	DB_SVC_FreeServiceInfoList(NULL, phSvcList, pstSvcInfoArray);

#endif

	return ERR_OK;
}

HBOOL xsvc_si_isTelenorB2_Network(HUINT16 onId)
{
	HBOOL 	bCheckValue = FALSE;

#if defined(CONFIG_OP_TELENOR)
	if(TELENOR_B2_ONID == onId)
	{
		bCheckValue = TRUE;
	}
#endif

	return bCheckValue;
}

HBOOL xsvc_si_LcnCheckOnId_Nordig(HUINT16 onId)
{
	HBOOL 	bCheckValue = FALSE;

	switch(onId)
	{
#if defined(CONFIG_OP_TELENOR)
		case TELENOR_CDK_ONID:
		case TELENOR_B2_ONID:
			bCheckValue = TRUE;
			break;
#endif

#if defined(CONFIG_OP_COMHEM)
		case COMHEM_NETWORK_ONID:
			bCheckValue = TRUE;
			break;
#endif

#if defined(LABWISE_CABLE_TEST)
		case LABWISE_CABLE_TEST_ONID:
			bCheckValue = TRUE;
			break;
#endif

		default:
#if defined(CONFIG_OP_TDC)
			/*
				ONID for TDC is 0xFE00, but there are no restrictions about assgining LCN channels.
				It can also support Nordig automatic search.
			*/
			bCheckValue = TRUE;
#else
			bCheckValue = FALSE;
#endif
			break;
	}

	return bCheckValue;
}


/* end of file */

