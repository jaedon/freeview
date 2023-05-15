/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_lcn_ziggo.c
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
#include <_xsvc_si.h>
#include <_xsvc_si_lib_lcn_base.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define CHECK_THE_SERVICE_IN_SLD		(0)


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef  struct
{
	HUINT16		usTsId;
	HUINT16		usOnId;
	HUINT16		usServiceId;
	HUINT8		ucVisibleServiceFlag;
	HUINT16		usLogicalChannelNumber;
	HBOOL		bIsHdSimu;
	HUINT8		ucHdSimuVisibleServiceFlag;
	HUINT16 	usHdSimuLogicalChannelNumber;
} xsvcSi_LcnInfo_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

#define ___LOCAL_FUNCTIONS___

#define ___MEMBER_FUNCTIONS___

#define ___API_FUNCTIONS___

STATIC HERROR xsvc_si_LcnFindLogicalChannelFromList_Ziggo (HxList_t *pChDb, HUINT16 usOnId, HUINT16 usTsId, HUINT16 usSvcId, HUINT32 *pulPos)
{
	HUINT32 ulPos = 0;
	HxList_t *pstItem = NULL;

	HLIB_LIST_First(pChDb);
	for (pstItem = pChDb, ulPos = 0; NULL != pstItem; pstItem = pstItem->next, ulPos++)
	{
		xsvcSi_LcnInfo_t *pstLcnInfo = (xsvcSi_LcnInfo_t *)HLIB_LIST_Data(pstItem);
		if ( (pstLcnInfo->usTsId == usTsId)
			&& (pstLcnInfo->usOnId == usOnId)
			&& (pstLcnInfo->usServiceId == usSvcId) )
		{
			*pulPos = ulPos;
			return ERR_OK;
		}
	}
	return ERR_FAIL;
}

STATIC HERROR xsvc_si_LcnGetLogicalChannelFromNit_Ziggo (HUINT32 ulActionId, HxList_t **ppChList)
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
			HBOOL bFoundPrivate = TRUE;
			HUINT8 *pucDesRaw = (HUINT8 *)HLIB_LIST_Data (pstItem2);

			/*  PDSD가 없거나 PDSD의 vaule값이 ZIGGO(0x00000028)인 경우 desc_tag값이 0x80이후인 pivate desc parse하도록 허용 */
			if (*pucDesRaw == eSIxDESCTAG_PRIVATE_DATA_SPECIFIER)
			{
				SIxPrivateDataSpecDes_t *pPrivDataSpecDes = (SIxPrivateDataSpecDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);
				if (pPrivDataSpecDes != NULL)
				{
					if (pPrivDataSpecDes->ulPrivateDataSpec == PRIVATE_DATA_SPECIFIER_FOR_ZIGGO)
						bFoundPrivate = TRUE;
					else
						bFoundPrivate = FALSE;

					PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pPrivDataSpecDes);
				}
			}

			if ( (bFoundPrivate == FALSE) && (*pucDesRaw >= 0x80) )
				continue;

			switch (*pucDesRaw)
			{
				case eSIxDESCTAG_LOGICAL_CHANNEL:
				{
					SIxZiggoLogicalChannelDes_t *pstLogicChDes = (SIxZiggoLogicalChannelDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);
					if (NULL != pstLogicChDes)
					{
						SIxZiggoLogicalChannelInfo_t *pChListItem = pstLogicChDes->pstInfo;

						while(pChListItem)
						{
							HUINT32 ulPos;
							xsvcSi_LcnInfo_t *pstLcnInfo = NULL;

							if (xsvc_si_LcnFindLogicalChannelFromList_Ziggo(pChList, pst2ndLoop->unInfo.stNit.usOrgNetId, pst2ndLoop->unInfo.stNit.usTsId, pChListItem->usServiceId, &ulPos) == ERR_OK)
							{
								pstLcnInfo = (xsvcSi_LcnInfo_t*)HLIB_LIST_Data(HLIB_LIST_GetListItem(pChList, ulPos));
								if (pstLcnInfo != NULL)
								{
									pstLcnInfo->ucVisibleServiceFlag = pChListItem->ucVisibleServiceFlag;
									pstLcnInfo->usLogicalChannelNumber = pChListItem->usLogicalChannelNumber;

									HxLOG_Debug ("\t Registered LCN Info from eSIxDESCTAG_LOGICAL_CHANNEL (0x%x:0x%x:0x%x) VisibleServiceFlag = %d, usLogicalChannelNumber = %d\n",
													pstLcnInfo->usOnId, pstLcnInfo->usTsId, pstLcnInfo->usServiceId,
													pstLcnInfo->ucVisibleServiceFlag, pstLcnInfo->usLogicalChannelNumber);
								}
							}
							else
							{
								pstLcnInfo = (xsvcSi_LcnInfo_t*)OxSI_Malloc(sizeof(xsvcSi_LcnInfo_t));
								if (pstLcnInfo != NULL)
								{
									memset(pstLcnInfo, 0, sizeof(xsvcSi_LcnInfo_t));
									pstLcnInfo->usTsId = pst2ndLoop->unInfo.stNit.usTsId;
									pstLcnInfo->usOnId = pst2ndLoop->unInfo.stNit.usOrgNetId;

									pstLcnInfo->usServiceId = pChListItem->usServiceId;
									pstLcnInfo->ucVisibleServiceFlag = pChListItem->ucVisibleServiceFlag;
									pstLcnInfo->usLogicalChannelNumber = pChListItem->usLogicalChannelNumber;

									HxLOG_Debug ("\t NEW LCN Info from eSIxDESCTAG_LOGICAL_CHANNEL (0x%x:0x%x:0x%x) VisibleServiceFlag = %d, usLogicalChannelNumber = %d\n",
													pstLcnInfo->usOnId, pstLcnInfo->usTsId, pstLcnInfo->usServiceId,
													pstLcnInfo->ucVisibleServiceFlag, pstLcnInfo->usLogicalChannelNumber);

									pChList = HLIB_LIST_Append(pChList, (void*)pstLcnInfo);
								}
							}
							pChListItem = pChListItem->pstNext;
						}

						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstLogicChDes);
					}
				}
				break;

				case eSIxDESCTAG_ZIGGO_HD_SIMULCAST_LOGICAL_CHANNEL:
				{
					SIxZiggoHDSimulcastLcnDes_t *pstLogicChDes = (SIxZiggoHDSimulcastLcnDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);
					if (NULL != pstLogicChDes)
					{
						SIxZiggoHDSimulLcnInfo_t *pChListItem = pstLogicChDes->pstInfo;

						while(pChListItem)
						{
							HUINT32 ulPos;
							xsvcSi_LcnInfo_t *pstLcnInfo = NULL;

							if (xsvc_si_LcnFindLogicalChannelFromList_Ziggo(pChList, pst2ndLoop->unInfo.stNit.usOrgNetId, pst2ndLoop->unInfo.stNit.usTsId, pChListItem->usServiceId, &ulPos) == ERR_OK)
							{
								pstLcnInfo = (xsvcSi_LcnInfo_t*)HLIB_LIST_Data(HLIB_LIST_GetListItem(pChList, ulPos));
								if (pstLcnInfo != NULL)
								{
									pstLcnInfo->bIsHdSimu = TRUE;
									pstLcnInfo->ucHdSimuVisibleServiceFlag = pChListItem->ucVisibleServiceFlag;
									pstLcnInfo->usHdSimuLogicalChannelNumber = pChListItem->usLogicalChannelNumber;

									HxLOG_Debug ("\t Registered LCN Info from eSIxDESCTAG_ZIGGO_HD_SIMULCAST_LOGICAL_CHANNEL (0x%x:0x%x:0x%x) ucHdSimuVisibleServiceFlag = %d, usHdSimuLogicalChannelNumber = %d\n",
												pstLcnInfo->usOnId, pstLcnInfo->usTsId, pstLcnInfo->usServiceId,
												pstLcnInfo->ucHdSimuVisibleServiceFlag, pstLcnInfo->usHdSimuLogicalChannelNumber);
								}
							}
							else
							{
								pstLcnInfo = (xsvcSi_LcnInfo_t*)OxSI_Malloc(sizeof(xsvcSi_LcnInfo_t));
								if (pstLcnInfo != NULL)
								{
									memset(pstLcnInfo, 0, sizeof(xsvcSi_LcnInfo_t));
									pstLcnInfo->usTsId = pst2ndLoop->unInfo.stNit.usTsId;
									pstLcnInfo->usOnId = pst2ndLoop->unInfo.stNit.usOrgNetId;

									pstLcnInfo->usServiceId = pChListItem->usServiceId;
									pstLcnInfo->bIsHdSimu = TRUE;
									pstLcnInfo->ucHdSimuVisibleServiceFlag = pChListItem->ucVisibleServiceFlag;
									pstLcnInfo->usHdSimuLogicalChannelNumber = pChListItem->usLogicalChannelNumber;

									HxLOG_Debug ("\t NEW LCN Info from eSIxDESCTAG_ZIGGO_HD_SIMULCAST_LOGICAL_CHANNEL (0x%x:0x%x:0x%x) ucHdSimuVisibleServiceFlag = %d, usHdSimuLogicalChannelNumber = %d\n",
													pstLcnInfo->usOnId, pstLcnInfo->usTsId, pstLcnInfo->usServiceId,
													pstLcnInfo->ucHdSimuVisibleServiceFlag, pstLcnInfo->usHdSimuLogicalChannelNumber);

									pChList = HLIB_LIST_Append(pChList, (void*)pstLcnInfo);
								}
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

/* HD simulcast lcn에 의해 중복되는 채널은 제거 */
void xsvc_si_LcnRemoveDuplicatedLogicChannel_Ziggo (HUINT16 usOnId, HUINT16 usTsId, HUINT16 usSvcId, HUINT16 usLcn, HUINT32 ulNum, DbSvc_Info_t *pstSvcArray, SRCHSVC_AppendFlag_t *peFlagArray)
{
	HUINT32 		ulIndex;
	DbSvc_Info_t		*pstSvcInfo;

	for (ulIndex=0; ulIndex < ulNum; ulIndex++)
	{
		if ( (peFlagArray[ulIndex] != eSRCHSVC_ADD)
			&& 	(peFlagArray[ulIndex] != eSRCHSVC_UPDATE_CHANGING_SVCNUM) )
		{
			continue;
		}

		pstSvcInfo = &(pstSvcArray[ulIndex]);

		if (pstSvcInfo->usLcn == usLcn)
		{
			if ( (pstSvcInfo->usOnId != usOnId)
				||(pstSvcInfo->usTsId != usTsId)
				||(pstSvcInfo->usSvcId != usSvcId) )
			{
				peFlagArray[ulIndex] = eSRCHSVC_SKIP;
				return;
			}
		}
	}
}

HUINT32 xsvc_si_LcnThisServiceExistOnSLDInNit_Ziggo (HUINT32 ulActionId, HUINT16 usOnId, HUINT16 usTsId, HUINT16 usSvcId)
{
	HxList_t					*pstItem1, *pstItem2;
	SIxTable_PsiTable_t 		*pstNitTable = NULL;
	HERROR					 hResult = ERR_FAIL;
	HxList_t					*pChList = NULL;

	pstNitTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_NitAct, SILIB_TABLE_EXT_ID_ALL);
	if (pstNitTable == NULL)
	{
		HxLOG_Error ("Invalid pstNitTable \n");
		goto END_FUNC;
	}

	for (pstItem1 = pstNitTable->pst2ndLoop; NULL != pstItem1; pstItem1 = pstItem1->next)
	{
		SIxTable_2ndLoop_t *pst2ndLoop = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstItem1);

		if ( (pst2ndLoop->unInfo.stNit.usOrgNetId != usOnId)
			|| (pst2ndLoop->unInfo.stNit.usTsId != usTsId) )
			continue;

		for (pstItem2 = pst2ndLoop->pstDesLoop; NULL != pstItem2; pstItem2 = pstItem2->next)
		{
			HUINT8 *pucDesRaw = (HUINT8 *)HLIB_LIST_Data (pstItem2);

			switch (*pucDesRaw)
			{
				case eSIxDESCTAG_SERVICE_LIST:
				{
					SIxSvcListDes_t *pstSvcListDes = (SIxSvcListDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);

					if (NULL != pstSvcListDes)
					{
						SIxSvcListInfo_t *pSvcListItem = pstSvcListDes->pstInfo;
						while(pSvcListItem)
						{
							if (pSvcListItem->usSvcId == usSvcId)
							{
								hResult = ERR_OK;
								HxLOG_Debug ("\t Found The service in SLD (0x%x:0x%x:0x%x)\n", usOnId, usTsId, usSvcId);
								goto END_FUNC;
							}
							pSvcListItem = pSvcListItem->pstNext;
						}
						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstSvcListDes);
					}
				}
			}
		}
	}

END_FUNC:
	if (NULL != pstNitTable)	{ PAL_SIRAWPOOL_FreeTable (pstNitTable); }

	return hResult;
}


// ============================================================================================
// Function : local_mwclcn_SetServiceNumber_Ziggo
HERROR xsvc_si_LcnSetServiceNumber_Ziggo (Handle_t hAction, HUINT32 ulNum, DbSvc_Info_t *pstSvcArray, SRCHSVC_AppendFlag_t *peFlagArray)
{
	HUINT16			 usStartTvNum, usStartRadioNum;
	HUINT32			 ulIndex;
	DbSvc_Info_t		*pstSvcInfo;
	HxList_t			*pChList = NULL;
	HxList_t			*pstItem = NULL;
#if (CHECK_THE_SERVICE_IN_SLD == 1)
	HBOOL			bIsSvcInSld;
#endif

	usStartTvNum = usStartRadioNum = 0;
	DB_SVC_GetLcnOrderNumber (DbSvc_GetDeliType(pstSvcArray[0]), eDxSVC_TYPE_TV, &usStartTvNum);
	DB_SVC_GetLcnOrderNumber (DbSvc_GetDeliType(pstSvcArray[0]), eDxSVC_TYPE_RADIO, &usStartRadioNum);

	/* base lcn 만큼 보정 */
	usStartTvNum += xsvc_si_LibLcnGetSvcNumBase (eDEFAULTLCN_ZIGGO, DbSvc_GetDeliType(pstSvcArray[0]), eDxSVC_TYPE_TV);
	usStartRadioNum += xsvc_si_LibLcnGetSvcNumBase (eDEFAULTLCN_ZIGGO, DbSvc_GetDeliType(pstSvcArray[0]), eDxSVC_TYPE_RADIO);

	if (xsvc_si_LcnGetLogicalChannelFromNit_Ziggo(PAL_PIPE_GetActionId(hAction), &pChList) != ERR_OK)
	{
		HxLOG_Error ("\n\t xsvc_si_LcnSetServiceNumber_Ziggo ::: error in xsvc_si_LcnGetLogicalChannelFromNit_Ziggo\n");
	}

	for (ulIndex = 0; ulIndex < ulNum; ulIndex++)
	{
		if ( (peFlagArray[ulIndex] != eSRCHSVC_ADD)
			&& (peFlagArray[ulIndex] != eSRCHSVC_UPDATE_CHANGING_SVCNUM) )
		{
			continue;
		}

		pstSvcInfo = &(pstSvcArray[ulIndex]);

#if (CHECK_THE_SERVICE_IN_SLD == 1)
		/* act NIT의 Service list descriptor에 해당 service가 존재하는 검사 */
		bIsSvcInSld = ((xsvc_si_LcnThisServiceExistOnSLDInNit_Ziggo (PAL_PIPE_GetActionId(hAction), pstSvcInfo->usOnId, pstSvcInfo->usTsId, pstSvcInfo->usSvcId) == ERR_OK) ? TRUE : FALSE);
#endif

		/* set LCN */
		/* LCN == 0 이면 add하지 않고 skip */
		if (pChList != NULL)
		{
			HLIB_LIST_First(pChList);
			for (pstItem = pChList; NULL != pstItem; pstItem = pstItem->next)
			{
				xsvcSi_LcnInfo_t *pstLcnInfo = (xsvcSi_LcnInfo_t *)HLIB_LIST_Data(pstItem);
				if (pstLcnInfo->usTsId == DbSvc_GetTsId(pstSvcInfo)
					&& pstLcnInfo->usOnId == DbSvc_GetOnId(pstSvcInfo)
					&& pstLcnInfo->usServiceId == DbSvc_GetSvcId(pstSvcInfo))
				{
					DbSvc_SetLcnFlag(pstSvcInfo, TRUE);

					if (pstLcnInfo->bIsHdSimu == TRUE)
					{
						if (pstLcnInfo->usHdSimuLogicalChannelNumber > 0)
						{
							DbSvc_SetLcn(pstSvcInfo, pstLcnInfo->usHdSimuLogicalChannelNumber);
							DbSvc_SetLcnFlag(pstSvcInfo, pstLcnInfo->ucHdSimuVisibleServiceFlag);
							xsvc_si_LcnRemoveDuplicatedLogicChannel_Ziggo(pstLcnInfo->usOnId, pstLcnInfo->usTsId, pstLcnInfo->usServiceId, pstLcnInfo->usHdSimuLogicalChannelNumber, ulNum, pstSvcArray, peFlagArray);
						}
						else
						{
							HxLOG_Warning ("\t\t ===> This Service is not added(LCN == 0) in HD simulcast desc : (0x%x:0x%x:0x%x) : Name(%s)\n",
											DbSvc_GetOnId(pstSvcInfo), DbSvc_GetTsId(pstSvcInfo), DbSvc_GetSvcId(pstSvcInfo), DbSvc_GetName(pstSvcInfo));

							peFlagArray[ulIndex] = eSRCHSVC_SKIP;
						}
					}
					else
					{
						if (pstLcnInfo->usLogicalChannelNumber > 0)
						{
							DbSvc_SetLcn(pstSvcInfo, pstLcnInfo->usLogicalChannelNumber);
							DbSvc_SetLcnFlag(pstSvcInfo, pstLcnInfo->ucVisibleServiceFlag);
						}
						else
						{
							HxLOG_Warning ("\t\t ===> This Service is not added(LCN == 0) in Logic channel desc : (0x%x:0x%x:0x%x) : Name(%s)\n",
											DbSvc_GetOnId(pstSvcInfo), DbSvc_GetTsId(pstSvcInfo), DbSvc_GetSvcId(pstSvcInfo), DbSvc_GetName(pstSvcInfo));

							peFlagArray[ulIndex] = eSRCHSVC_SKIP;
						}
					}
				}
			}
		}

		HxLOG_Print ("\t\t ===> searched svc info : (0x%x:0x%x:0x%x) name = %s, LCN(%d:0x%x), Type=0x%x, casType=0x%x, flag = %d\n",
								DbSvc_GetOnId(pstSvcInfo), DbSvc_GetTsId(pstSvcInfo), DbSvc_GetSvcId(pstSvcInfo), DbSvc_GetName(pstSvcInfo), DbSvc_GetLcnFlag(pstSvcInfo),
								DbSvc_GetLcn(pstSvcInfo), DbSvc_GetSvcType(pstSvcInfo), DbSvc_GetCasType(pstSvcInfo), peFlagArray[ulIndex]);

		if (DbSvc_GetLcnFlag(pstSvcInfo) != TRUE)
		{
			/* SDL에 존재하나 no LCN채널은 skip 처리 */
#if (CHECK_THE_SERVICE_IN_SLD == 1)
			if (bIsSvcInSld == TRUE)
			{
				peFlagArray[ulIndex] = eSRCHSVC_SKIP;
				HxLOG_Warning ("\t\t ===> This Service is not added(SLD ok, no LCN) : (0x%x:0x%x:0x%x) : Name(%s)\n",
								DbSvc_GetOnId(pstSvcInfo), DbSvc_GetTsId(pstSvcInfo), DbSvc_GetSvcId(pstSvcInfo), DbSvc_GetName(pstSvcInfo));
				continue;
			}
#endif

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
	}

	if (pChList != NULL)
	{
		HLIB_LIST_First(pChList);
		for (pstItem = pChList; NULL != pstItem; pstItem = pstItem->next)
		{
			void *pvData = (void *)HLIB_LIST_Data(pstItem);
			if (pvData) OxSI_Free(pvData);

		}
		HLIB_LIST_RemoveAll(pChList);
	}

	return ERR_OK;
}

/* end of file */

