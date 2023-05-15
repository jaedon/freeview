/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_live_ukdtt.c
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
#include <pal_audio.h>
#include <pal_sirawpool.h>

#include <xdb_svc.h>
#include <svc_si.h>
#include <_svc_si.h>
#include <_svc_si_lib.h>
#include <_svc_si_filtering.h>
#include <_svc_si_live.h>
#include <_xsvc_si.h>
#include <_xsvc_si_lib_live_base.h>

#include <_xsvc_si_spec_ukdtt.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct
{
	svcSi_LiveData_t		stBase;

	// only For UkDtt
	HBOOL				bUseRCT;
} svcSi_LiveData_UkDtt_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HBOOL							s_bUKServiceReplacement = FALSE;
STATIC SvcSi_SdtRunningStatusType_e		s_eRunningStatus = eSdtRunningStatusType_RUNNING;


/*******************************************************************/
/********************     extern Variables     ********************/
/*******************************************************************/
extern HERROR xsvc_si_NcdProcessFromNit_UkDtt(svcSi_LiveData_t  *pstLive, SvcSi_Arg_SecCb_t *pstSecCb, HUINT32	ulHead);


/*******************************************************************/
/********************      Static Functions     ********************/
/*******************************************************************/

#define ___LOCAL_FUNCTIONS_CHECK___
#if defined(CONFIG_DEBUG)
STATIC void xvsc_si_PrintImageIcon(HUINT32 ulActionId, HxList_t *pstDesLoop)
{
	HxList_t	*pstImgDescLoop = NULL, *pstCurrLoop= NULL;

	pstImgDescLoop = pstDesLoop;

	for(pstCurrLoop = pstImgDescLoop;pstCurrLoop != NULL; pstCurrLoop = pstCurrLoop->next)
	{
		HUINT8	*pucRawDes = (HUINT8*)HLIB_LIST_Data(pstCurrLoop);

		if(*pucRawDes == eSIxDESCTAG_EXTENSION)
		{
			SIxExtensionDes_t *pstExtensionDes = (SIxExtensionDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);
			if (NULL != pstExtensionDes)
			{
				if(pstExtensionDes->pucRawData[0] == eSIxDESCTAG_EXT_IMG_ICON)
				{
					SIxImageIconDes_t *pstImgIconDes = (SIxImageIconDes_t *)PAL_SIRAWPOOL_ParseDvbExtDes (ulActionId, pstExtensionDes->pucRawData);

					if(NULL != pstImgIconDes)
					{
						HxLOG_Debug("\t\t---------------------Image Icon Info ------------------\n");
						HxLOG_Debug("\t\tDesNum(%d) LastDesNum(%d) IconId(%d) \n", pstImgIconDes->ucDesNum, pstImgIconDes->ucLastDesNum,pstImgIconDes->ucIconId);

						if(pstImgIconDes->pstImgHeadInfo!= NULL)
						{
							HxLOG_Debug("\t\tIconTranMode(%d) IconPosFlag(%d) CoordSystem(%d) IconHorOrigin(%d) IconVerOrigin(%d) \n",
								pstImgIconDes->pstImgHeadInfo->ucIconTransMode , pstImgIconDes->pstImgHeadInfo->ucIconPosFlag,pstImgIconDes->pstImgHeadInfo->ucCoordSystem,
								pstImgIconDes->pstImgHeadInfo->usIconHorOrigin, pstImgIconDes->pstImgHeadInfo->usIconVerOrigin);

							if(pstImgIconDes->pstImgHeadInfo->pstImgTypeInfo != NULL)
							{
								if(pstImgIconDes->pstImgHeadInfo->pstImgTypeInfo->usTypeLen > 0)
									HxLOG_Debug("\t\tusTypeLen (%d)  pucImgType (%s) \n", pstImgIconDes->pstImgHeadInfo->pstImgTypeInfo->usTypeLen , pstImgIconDes->pstImgHeadInfo->pstImgTypeInfo->pucImgType);
								else
									HxLOG_Debug("\t\tpucImgType is NULL \n");
							}
							else
							{
								HxLOG_Debug("\t\tpstImgTypeInfo is NULL \n");
							}

							if(pstImgIconDes->pstImgHeadInfo->pstImgUrlInfo != NULL)
							{
								if(pstImgIconDes->pstImgHeadInfo->pstImgUrlInfo->usUrlLen > 0)
									HxLOG_Debug("\t\tusUrlLen(%d)  pucImgUrl(%s) \n", pstImgIconDes->pstImgHeadInfo->pstImgUrlInfo->usUrlLen, pstImgIconDes->pstImgHeadInfo->pstImgUrlInfo->pucImgUrl);
								else
									HxLOG_Debug("\t\tpucImgUrl is NULL \n");
							}
							else
							{
								HxLOG_Debug("\t\tpstImgUrlInfo is NULL \n");
							}

							if(pstImgIconDes->pstImgHeadInfo->pstImgIconInfo != NULL)
							{
								if(pstImgIconDes->pstImgHeadInfo->pstImgIconInfo->ucDataLen > 0)
									HxLOG_Debug("\t\tucDataLen(%d)  pucImgData(%s) \n", pstImgIconDes->pstImgHeadInfo->pstImgIconInfo->ucDataLen, pstImgIconDes->pstImgHeadInfo->pstImgIconInfo->pucImgData);
								else
									HxLOG_Debug("\t\tpucImgData is NULL \n");
							}
							else
							{
								HxLOG_Debug("\t\tpstImgIconInfo is NULL \n");
							}
						}
						else
						{
							HxLOG_Debug("\t\tpstImgHeadInfo is NULL \n");
						}

						if(pstImgIconDes->pstImgIconInfo!= NULL)
						{
							if(pstImgIconDes->pstImgIconInfo->ucDataLen > 0)
								HxLOG_Debug("\t\tucDataLen(%d)	pucImgData(%s) \n", pstImgIconDes->pstImgIconInfo->ucDataLen, pstImgIconDes->pstImgIconInfo->pucImgData);
							else
								HxLOG_Debug("\t\tpucImgData is NULL \n");
						}
						else
						{
							HxLOG_Debug("\t\tpstImgIconInfo is NULL \n");
						}

						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstImgIconDes);

						HxLOG_Debug("\t\t-------------------------------------------------------\n");
					}
				}

				PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstExtensionDes);
			}
		}
	}
}

STATIC void xsvc_si_LivePrintRct(svcSi_LiveData_t *pstLive, SvcSi_Arg_SecCb_t *pstSecCb)
{
	HUINT32	ulActionId;
	HUINT32	ulSecCnt, ulLinkCnt, ulPromCnt;
	SIxTable_PsiTable_t		*pstCurrRct= NULL, *pstRctTable = NULL;
	SIxTable_RctSec_t		*pstRctSecInfo = NULL;
	HxList_t				*pst2ndLoop= NULL, *pstPromoTextLoop=NULL;

	ulActionId = PAL_PIPE_GetActionId (pstLive->hAction);

	pstCurrRct = pstRctTable = PAL_SIRAWPOOL_ParseTable(ulActionId, eSIxTABLETYPE_Rct, SILIB_TABLE_EXT_ID_ALL);
	if(pstRctTable == NULL)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable ulActionId=%d, RCT \n", ulActionId);
		return;
	}

	ulSecCnt = 0;

	while(pstCurrRct!=NULL)
	{
		pstRctSecInfo = &(pstCurrRct->unInfo.stRct);

		HxLOG_Debug("################ < RCT (%02d) th SECTION INFO > ################ \n", ulSecCnt);

		HxLOG_Debug("SvcId[0x%04x] TableIdExtFlag[%d] YearOffset[%d] \n", pstRctSecInfo->usSvcId, pstRctSecInfo->ucTableIdExtFlag, pstRctSecInfo->usYearOffset);

		ulLinkCnt = 0;

		// 2nd loop : link info
		for (pst2ndLoop = pstCurrRct->pst2ndLoop; NULL != pst2ndLoop; pst2ndLoop = pst2ndLoop->next)
		{
			SIxTable_2ndLoop_t		*pst2ndItem = (SIxTable_2ndLoop_t*)HLIB_LIST_Data (pst2ndLoop);
			SIxTable_RctLinkInfo_t		*pstLinkInfo = NULL ;

			pstLinkInfo = &(pst2ndItem->unInfo.stRct);

			HxLOG_Debug("\t============< RCT (%02d) th LINK INFO > ============================= \n", ulLinkCnt);

			HxLOG_Debug("\tLinkType (%d) HowRelatedSchemeId(%d), TermId(%d) GroupId(%d) Precedence(%d) \n",
				pstLinkInfo->ucLinkType, pstLinkInfo->ucHowSchemeId, pstLinkInfo->usTermId, pstLinkInfo->ucGroupId, pstLinkInfo->ucPrecedence);

			if(pstLinkInfo->ucMediaUriLen > 0)
				HxLOG_Debug("\tpucMediaUriByte: %s \n", pstLinkInfo->pucMediaUriByte);
			else
				HxLOG_Debug("\tpucMediaUriByte is NULL\n");

			if(pstLinkInfo->pstDvbBinLocator != NULL)
			{
				HxLOG_Debug("\tIdentifierType(%d) ScheTimeReliability(%d) InlineSvc(%d) StartDate(0x%x) \n",
					pstLinkInfo->pstDvbBinLocator->ucIdType , pstLinkInfo->pstDvbBinLocator->ucSchTimeReliability,
					pstLinkInfo->pstDvbBinLocator->ucInlineService, pstLinkInfo->pstDvbBinLocator->usStartDate );

				if(pstLinkInfo->pstDvbBinLocator->ucInlineService == 0)
					HxLOG_Debug("\tDvbSvcTripletId(0x%x)\n", pstLinkInfo->pstDvbBinLocator->usDvbSvcTripletId );
				else
					HxLOG_Debug("\tTsId(0x%x) OnId(0x%x) SvcId(0x%x)\n", pstLinkInfo->pstDvbBinLocator->usTsId, pstLinkInfo->pstDvbBinLocator->usOnId, pstLinkInfo->pstDvbBinLocator->usSvcId );

				HxLOG_Debug("\tStartTime(0x%x) Duration(0x%x) \n", pstLinkInfo->pstDvbBinLocator->usStartTime, pstLinkInfo->pstDvbBinLocator->usDuration);

				if(pstLinkInfo->pstDvbBinLocator->ucIdType== 1)
					HxLOG_Debug("\tEvtID(0x%x) \n", pstLinkInfo->pstDvbBinLocator->usEventId);
				else if(pstLinkInfo->pstDvbBinLocator->ucIdType== 2)
					HxLOG_Debug("\tTvaId(0x%x) \n", pstLinkInfo->pstDvbBinLocator->usTvaId);
				else if(pstLinkInfo->pstDvbBinLocator->ucIdType== 3)
					HxLOG_Debug("\tTvaId(0x%x) Component(%d) \n", pstLinkInfo->pstDvbBinLocator->usTvaId,  pstLinkInfo->pstDvbBinLocator->ucComponent);
				else
					HxLOG_Debug("\tDvbBinLocator.ucIdentifierType is Invalid \n");

				if(pstLinkInfo->pstDvbBinLocator->ucIdType == 0 &&  pstLinkInfo->pstDvbBinLocator->ucSchTimeReliability ==1)
					HxLOG_Debug("\tEarlyStartWin(%d), LateEndWin(%d) \n", pstLinkInfo->pstDvbBinLocator->ucEarlyStartWindow, pstLinkInfo->pstDvbBinLocator->ucLateEndWindow);
			}
			else
			{
				HxLOG_Debug("\tpstDvbBinLocator is NULL\n");
			}

			if(pstLinkInfo->pstPromoTextLoop != NULL)
			{
				ulPromCnt = 0;

				for(pstPromoTextLoop = pstLinkInfo->pstPromoTextLoop;NULL != pstPromoTextLoop; pstPromoTextLoop = pstPromoTextLoop->next)
				{
					SIxTable_RctPromoText_t *pstPromoText = (SIxTable_RctPromoText_t*)HLIB_LIST_Data(pstPromoTextLoop);

					HxLOG_Debug("\t**********< RCT (%02d) th PromoText INFO > ***********************\n", ulPromCnt);

					if(pstPromoText->ucTxtLen > 0)
						HxLOG_Debug("\tLangCode [%s]  pszPromoText [%s] \n", pstPromoText->szIso639LangCode, pstPromoText->pszPromoText);
					else
						HxLOG_Debug("\tpszPromoText is NULL\n");

					ulPromCnt++;
				}

				HxLOG_Debug("\t***************************************************************\n");
			}

			xvsc_si_PrintImageIcon(ulActionId, pst2ndItem->pstDesLoop);

			HxLOG_Debug("DefaultIconFlag(%d) IconId(%d) \n", pstLinkInfo->ucDefaultIconFlag, pstLinkInfo->ucIconId);

			ulLinkCnt++;
			HxLOG_Debug("\t============================================================== \n");
		}

		xvsc_si_PrintImageIcon(ulActionId, pstCurrRct->pstDesLoop);

		pstCurrRct = pstCurrRct->pstNextTable;
		ulSecCnt++;
	}

	HxLOG_Debug("############################################################## \n");

	if(pstRctTable != NULL)		PAL_SIRAWPOOL_FreeTable(pstRctTable);

}



#endif

STATIC HERROR xsvc_si_LiveCheckAuthorityAndFTAFromNit_Ukdtt(svcSi_LiveData_t *pstLive, SvcSi_Arg_SecCb_t *pstSecCb, HUINT32 ulHead)
{
	HERROR				hErr = ERR_FAIL;
	HBOOL				bUpdateInform = FALSE, bFoundPrivate = FALSE;
	HUINT16				i, usNetIdx;
	HINT32				nTsCnt, nNetInfoCnt;
	HUINT32				ulActionId, ulFTAContentMgrValue, ulPrivateSpecifyValue;
	DbSvc_Info_t			stSvcInfo;
	DbSvc_TsInfo_t		*pstTsInfoList= NULL, *pstTsInfo = NULL;
	DbSvc_NetInfo_t		*pstNetInfoList = NULL, *pstNetInfoInDb = NULL;
	DbSvc_TsCondition_t	stTsAttr;

	HxList_t					*pstList = NULL, *pstDesList = NULL;
	SIxTable_PsiTable_t			*pstNitTable = NULL, *pstCurrNit = NULL;
	SIxDefaultAuthorityDes_t		*pstDefaultAuthorithyDesc = NULL;
	SIxFtaContentManagementDes_t	*pstFtaContentManagementDesc = NULL;
	SvcSi_SiSpec_t					unSiSpec;

	hErr = DB_SVC_GetServiceInfo(pstLive->hService, &stSvcInfo);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("DB_SVC_GetServiceInfo Error  !! \n");
		goto Error;
	}

	/* Get All TS Info - Default Authority */
	DB_SVC_InitTsFindCondition(&stTsAttr);
	stTsAttr.nDeliType = eDxDELIVERY_TYPE_TER;

	hErr = DB_SVC_FindTsInfoList(&stTsAttr, &nTsCnt, &pstTsInfoList);
	if(hErr != ERR_OK || nTsCnt <= 0 || pstTsInfoList == NULL)
	{
		HxLOG_Error("DB_SVC_FindTsInfoList Error  !! \n");
		goto Error;
	}

	hErr = DB_SVC_FindNetworkInfoByDeliType(eDxDELIVERY_TYPE_TER, &nNetInfoCnt, &pstNetInfoList);
	if(hErr != ERR_OK || nNetInfoCnt <= 0 || pstNetInfoList == NULL)
	{
		HxLOG_Error("DB_SVC_FindNetworkInfoByDeliType Error  !! \n");
		goto Error;
	}

	bUpdateInform = FALSE;

	ulActionId = PAL_PIPE_GetActionId(pstLive->hAction);
	if(ulHead == ACT_HEAD)
	{
		pstNitTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_NitAct, SILIB_TABLE_EXT_ID_ALL);
	}
	else
	{
		pstNitTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_NitOth, SILIB_TABLE_EXT_ID_ALL);
	}

	if (NULL == pstNitTable)
	{
		HxLOG_Error("PAL_SIRAWPOOL_ParseTable Error  !! \n");
		goto Error;
	}

	hErr = svc_si_GetSiSpec (ulActionId, &unSiSpec);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_GetSiSpec err:\n");
		goto Error;
	}

	hErr = svc_si_GetPrivateSpecifyValue(unSiSpec.stLiveSpec.ulActionSpec,&ulPrivateSpecifyValue);
	if(hErr != ERR_OK)
	{
		HxLOG_Error ("svc_si_GetPrivateSpecifyValue err:\n");
		goto Error;
	}

	for (pstCurrNit = pstNitTable; pstCurrNit != NULL; pstCurrNit = pstCurrNit->pstNextTable)
	{
		SIxTable_NitSec_t		*pstNitSec = &pstCurrNit->unInfo.stNit;
		HUINT8				*aucDefaultAuthority;
		HUINT8				*aucUriPrefix;

		pstNetInfoInDb = NULL;
		usNetIdx = 0;
		for(i = 0; i < nNetInfoCnt; i++)
		{
			if(pstNetInfoList[i].usNetId == pstNitSec->usNetId)
			{
				usNetIdx = pstNetInfoList[i].usNetIdx;
				pstNetInfoInDb = &pstNetInfoList[i];
				break;
			}
		}

		if(pstNetInfoInDb == NULL)
		{
			HxLOG_Warning("pstNetInfoInDb is NULL  !! \n");
			continue;
		}

		hErr = DB_SVC_GetNetworkDefaultAuthority(pstNetInfoInDb, &aucDefaultAuthority);
		if(hErr != ERR_OK)
		{
			HxLOG_Warning("DB_SVC_NetworkGetDefaultAuthority Error  !! \n");
			continue;
		}

		hErr = DB_SVC_GetNetworkUriPrefix(pstNetInfoInDb, &aucUriPrefix);
		if(hErr != ERR_OK)
		{
			HxLOG_Warning("DB_SVC_GetNetworkUriPrefix Error !! \n");
			continue;
		}

		bFoundPrivate= FALSE;

		// parse 1st des loop
		for (pstList = pstCurrNit->pstDesLoop; pstList != NULL; pstList = pstList->next)
		{
			HUINT8		*pucRawDes = HLIB_LIST_Data (pstList);

			/*****************************/
			/***	  Default Authority 		 ***/
			/*****************************/

			if (*pucRawDes == eSIxDESCTAG_DEFAULT_AUTHORITY)
			{
				pstDefaultAuthorithyDesc = (SIxDefaultAuthorityDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);

				if(pstDefaultAuthorithyDesc!= NULL)
				{
					if(HxSTD_StrCmp (pstDefaultAuthorithyDesc->pszDefaultAuthority, aucDefaultAuthority) != 0)
					{
						/* Default authority changed in NIT first loop */
						bUpdateInform = TRUE;
						DB_SVC_SetNetworkDefaultAuthority(pstNetInfoInDb, pstDefaultAuthorithyDesc->pszDefaultAuthority);
						DB_SVC_UpdateNetworkInfo (usNetIdx, pstNetInfoInDb);
					}
				}
				else
				{
					if(HxSTD_StrLen(aucDefaultAuthority) > 0)
					{
						/* Disappear default authority */
						bUpdateInform = TRUE;
						DB_SVC_SetNetworkDefaultAuthority(pstNetInfoInDb, 0x0);
						DB_SVC_UpdateNetworkInfo (usNetIdx, pstNetInfoInDb);
					}
				}

				if(pstDefaultAuthorithyDesc != NULL)
				{
					PAL_SIRAWPOOL_FreeDes((SIxDescriptor_t*) pstDefaultAuthorithyDesc);
					pstDefaultAuthorithyDesc = NULL;
				}
			}

			/*****************************/
			/***	  FTA Content Mgr		   ***/
			/*****************************/
			if(*pucRawDes == eSIxDESCTAG_FTA_CONTENT_MANAGEMENT && bFoundPrivate == TRUE)
			{
				pstFtaContentManagementDesc = (SIxFtaContentManagementDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);
				ulFTAContentMgrValue = 0;

				if(pstFtaContentManagementDesc != NULL)
				{
					ulFTAContentMgrValue = (HUINT32)(((HUINT32)eFTA_CONTENT_NET_LEVEL << 24) & 0xFF000000);
					ulFTAContentMgrValue |= (HUINT32)( (HUINT32)(pstFtaContentManagementDesc->ucDoNotScramble << 16) & 0x00FF0000);
					ulFTAContentMgrValue |= (HUINT32)( (HUINT32)(pstFtaContentManagementDesc->ucCtrlRAccOverINet << 8) & 0x0000FF00);
					ulFTAContentMgrValue |= (HUINT32)( (HUINT32)pstFtaContentManagementDesc->ucDoNotApplyRevocation & 0x000000FF);

					HxLOG_Debug("\t hAction(0x%x) ulActionId(%d) \n", pstLive->hAction,ulActionId );
					HxLOG_Debug("\t Found FTA Contents Management Descript, NetWork Level in NIT\n\t\t-- DoNotScramble:0x%X CtrlRAccOverINet:0x%X, DoNotApplyRevocation:0x%X >>>>> ulFTAContentMgrValue:0x%X\n",
								pstFtaContentManagementDesc->ucDoNotScramble,pstFtaContentManagementDesc->ucCtrlRAccOverINet,
								pstFtaContentManagementDesc->ucDoNotApplyRevocation, ulFTAContentMgrValue);
					HxLOG_Debug("svc_si_PostMsgToAp ==> eSEVT_SI_FTA_CONTENTS_SIGNAL\n", pstLive->hAction,ulActionId );
					svc_si_PostMsgToAp (eSEVT_SI_FTA_CONTENTS_SIGNAL, (HUINT32)pstLive->hAction, (HUINT32)usNetIdx, (HUINT32)ulFTAContentMgrValue, 0);

					if(pstNetInfoInDb->utExt.stTvAnyTime.ulFTAContentMgr != ulFTAContentMgrValue)
					{
						/* Default authority changed in NIT first loop */
						bUpdateInform = TRUE;
						pstNetInfoInDb->utExt.stTvAnyTime.ulFTAContentMgr = ulFTAContentMgrValue;
						DB_SVC_UpdateNetworkInfo (usNetIdx, pstNetInfoInDb);
					}
				}
				else
				{
					if(pstNetInfoInDb->utExt.stTvAnyTime.ulFTAContentMgr != 0)
					{
						/* Disappear default authority */
						bUpdateInform = TRUE;
						pstNetInfoInDb->utExt.stTvAnyTime.ulFTAContentMgr = 0;
						DB_SVC_UpdateNetworkInfo (usNetIdx, pstNetInfoInDb);
					}
				}

				if(pstFtaContentManagementDesc != NULL)
				{
					PAL_SIRAWPOOL_FreeDes((SIxDescriptor_t*) pstFtaContentManagementDesc);
					pstFtaContentManagementDesc = NULL;
				}
			}

			/*****************************/
			/***	 Uri Prefix 		 ***/
			/*****************************/
			if (*pucRawDes == eSIxDESCTAG_EXTENSION && bFoundPrivate == TRUE)
			{
				SIxExtensionDes_t *pstExtensionDes = (SIxExtensionDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);

				if(pstExtensionDes!= NULL)
				{
					if(pstExtensionDes->pucRawData[0] == eSIxDESCTAG_EXT_URI_LINKAGE )
					{
						SIxUriLinkageDes_t *pstUriLinkageDes = (SIxUriLinkageDes_t *)PAL_SIRAWPOOL_ParseDvbExtDes (ulActionId, pstExtensionDes->pucRawData);
						if(NULL != pstUriLinkageDes)
						{
							/* uriprefix changed in NIT first loop */
							bUpdateInform = TRUE;
							DB_SVC_SetNetworkUriPrefix(pstNetInfoInDb, pstUriLinkageDes->pszUriChar);
							DB_SVC_UpdateNetworkInfo (usNetIdx, pstNetInfoInDb);
							svc_si_PostMsgToAp(eSEVT_SI_NETWORK_URI_PREFIX_CHANGED, (HUINT32)pstLive->hAction, usNetIdx, 0, 0);
						}
						else
						{
							if(HxSTD_StrLen(aucUriPrefix) > 0)
							{
								/* Disappear default uriprefix */
								bUpdateInform = TRUE;
								DB_SVC_SetNetworkUriPrefix(pstNetInfoInDb, 0x0);
								DB_SVC_UpdateNetworkInfo (usNetIdx, pstNetInfoInDb);
								svc_si_PostMsgToAp(eSEVT_SI_NETWORK_URI_PREFIX_CHANGED, (HUINT32)pstLive->hAction, usNetIdx, 0, 0);
							}
						}
						if(pstUriLinkageDes != NULL)
						{
							PAL_SIRAWPOOL_FreeDes((SIxDescriptor_t*) pstUriLinkageDes);
							pstUriLinkageDes = NULL;
						}
					}
					if(pstExtensionDes != NULL)
					{
						PAL_SIRAWPOOL_FreeDes((SIxDescriptor_t*) pstExtensionDes);
						pstExtensionDes = NULL;
					}
				}
			}

			if(*pucRawDes == eSIxDESCTAG_PRIVATE_DATA_SPECIFIER)
			{
				SIxPrivateDataSpecDes_t *pPrivDataSpecDes = (SIxPrivateDataSpecDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);


				if(pPrivDataSpecDes != NULL)
				{
					HxLOG_Debug("\t	Network Level in NIT ::	ulPrivateDataSpec(0x%x) ulPrivateSpecifyValue (0x%x)  \n", pPrivDataSpecDes->ulPrivateDataSpec, ulPrivateSpecifyValue);

					if (pPrivDataSpecDes->ulPrivateDataSpec == ulPrivateSpecifyValue)
					{
						bFoundPrivate = TRUE;
					}

					PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pPrivDataSpecDes);
				}
				else
				{
					HxLOG_Debug("\t	Network Level in NIT :: pPrivDataSpecDes is NULL  \n");
				}
			}

			if(*pucRawDes == eSIxDESCTAG_PRIVATE_DATA_INDICATOR)
			{
				bFoundPrivate = FALSE;
			}
		}

		// parse 2nd loop
		for (pstList = pstCurrNit->pst2ndLoop; NULL != pstList; pstList = pstList->next)
		{
			SIxTable_2ndLoop_t *pstNitTsItem = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstList);

			bFoundPrivate = FALSE;

			pstTsInfo = NULL;
			for(i = 0; i < nTsCnt; i++)
			{
				if(pstTsInfoList[i].usNetIdx == usNetIdx
					&& pstTsInfoList[i].usOnId == pstNitTsItem->unInfo.stNit.usOrgNetId
					&& pstTsInfoList[i].usTsId == pstNitTsItem->unInfo.stNit.usTsId)
				{
					pstTsInfo = &pstTsInfoList[i];
					break;
				}
			}

			if(pstTsInfo == NULL)
			{
				HxLOG_Info("pstTsInfo is NULL  !! \n");
				continue;
			}

			hErr = DB_SVC_GetTsDefaultAuthority(pstTsInfo, &aucDefaultAuthority);
			if(hErr != ERR_OK)
			{
				HxLOG_Warning("DB_SVC_NetworkGetDefaultAuthority Error  !! \n");
				continue;
			}

			for (pstDesList = pstNitTsItem->pstDesLoop; NULL != pstDesList; pstDesList = pstDesList->next)
			{
				HUINT8		*pucRawDes = HLIB_LIST_Data (pstDesList);

				/*****************************/
				/***	  Default Authority 		 ***/
				/*****************************/

				if (*pucRawDes == eSIxDESCTAG_DEFAULT_AUTHORITY)
				{
					pstDefaultAuthorithyDesc = (SIxDefaultAuthorityDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);

					if(pstDefaultAuthorithyDesc!= NULL)
					{
						if(HxSTD_StrCmp (pstDefaultAuthorithyDesc->pszDefaultAuthority, aucDefaultAuthority) != 0)
						{
							/* Default authority changed in NIT first loop */
							bUpdateInform = TRUE;

							DB_SVC_SetTsDefaultAuthority(pstTsInfo, pstDefaultAuthorithyDesc->pszDefaultAuthority);
							DB_SVC_UpdateTsInfo(pstTsInfo->usTsIdx, pstTsInfo);
						}
					}
					else
					{
						if(HxSTD_StrLen(aucDefaultAuthority) > 0)
						{
							/* Disappear default authority */
							bUpdateInform = TRUE;
							DB_SVC_SetTsDefaultAuthority(pstTsInfo, 0x00);
							DB_SVC_UpdateTsInfo(pstTsInfo->usTsIdx, pstTsInfo);
						}
					}

					if(pstDefaultAuthorithyDesc != NULL)
					{
						PAL_SIRAWPOOL_FreeDes((SIxDescriptor_t*) pstDefaultAuthorithyDesc);
						pstDefaultAuthorithyDesc = NULL;
					}
				}


				/*****************************/
				/***	  FTA Content Mgr		   ***/
				/*****************************/
				if(*pucRawDes == eSIxDESCTAG_FTA_CONTENT_MANAGEMENT && bFoundPrivate == TRUE)
				{
					pstFtaContentManagementDesc = (SIxFtaContentManagementDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);
					ulFTAContentMgrValue = 0;
					if(pstFtaContentManagementDesc != NULL)
					{
						ulFTAContentMgrValue = (HUINT32)(((HUINT32)eFTA_CONTENT_TS_LEVEL << 24) & 0xFF000000);
						ulFTAContentMgrValue |= (HUINT32)( (HUINT32)(pstFtaContentManagementDesc->ucDoNotScramble << 16) & 0x00FF0000);
						ulFTAContentMgrValue |= (HUINT32)( (HUINT32)(pstFtaContentManagementDesc->ucCtrlRAccOverINet << 8) & 0x0000FF00);
						ulFTAContentMgrValue |= (HUINT32)( (HUINT32)pstFtaContentManagementDesc->ucDoNotApplyRevocation & 0x000000FF);

						svc_si_PostMsgToAp (eSEVT_SI_FTA_CONTENTS_SIGNAL, (HUINT32)pstLive->hAction, (HUINT32)pstTsInfo->usTsIdx, (HUINT32)ulFTAContentMgrValue, 0);

						HxLOG_Debug("\t hAction(0x%x) ulActionId(%d) \n", pstLive->hAction,ulActionId );
						HxLOG_Debug("\t Found FTA Contents Management Descript, TS Level in NIT\n\t\t-- DoNotScramble:0x%X CtrlRAccOverINet:0x%X, DoNotApplyRevocation:0x%X >>>>> ulFTAContentMgrValue:0x%X\n",
									pstFtaContentManagementDesc->ucDoNotScramble,pstFtaContentManagementDesc->ucCtrlRAccOverINet,
									pstFtaContentManagementDesc->ucDoNotApplyRevocation, ulFTAContentMgrValue);

						if(pstTsInfo->utExt.stTvAnyTime.ulFTAContentMgr != ulFTAContentMgrValue)
						{
							/* Default authority changed in NIT second loop */
							bUpdateInform = TRUE;
							pstTsInfo->utExt.stTvAnyTime.ulFTAContentMgr = ulFTAContentMgrValue;
							DB_SVC_UpdateTsInfo(pstTsInfo->usTsIdx, pstTsInfo);

							HxLOG_Debug("svc_si_PostMsgToAp ==> eSEVT_SI_FTA_CONTENTS_SIGNAL\n" );
						}
					}
					else
					{
						if(pstTsInfo->utExt.stTvAnyTime.ulFTAContentMgr != 0)
						{
							/* Disappear default authority */
							bUpdateInform = TRUE;
							pstTsInfo->utExt.stTvAnyTime.ulFTAContentMgr = 0;
							DB_SVC_UpdateTsInfo(pstTsInfo->usTsIdx, pstTsInfo);
						}
					}

					if(pstFtaContentManagementDesc != NULL)
					{
						PAL_SIRAWPOOL_FreeDes((SIxDescriptor_t*) pstFtaContentManagementDesc);
						pstFtaContentManagementDesc = NULL;
					}
				}

				if(*pucRawDes == eSIxDESCTAG_PRIVATE_DATA_SPECIFIER)
				{
					SIxPrivateDataSpecDes_t *pPrivDataSpecDes = (SIxPrivateDataSpecDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);

					if(pPrivDataSpecDes != NULL)
					{
						HxLOG_Debug("\t	TS Level in NIT ::  ulPrivateDataSpec(0x%x) ulPrivateSpecifyValue (0x%x)  \n", pPrivDataSpecDes->ulPrivateDataSpec, ulPrivateSpecifyValue);

						if (pPrivDataSpecDes->ulPrivateDataSpec == ulPrivateSpecifyValue)
						{
							bFoundPrivate = TRUE;
						}

						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pPrivDataSpecDes);
					}
					else
					{
						HxLOG_Debug("\t	TS Level in NIT :: pPrivDataSpecDes is NULL  \n");
					}
				}

				if(*pucRawDes == eSIxDESCTAG_PRIVATE_DATA_INDICATOR)
				{
					bFoundPrivate = FALSE;
				}
			}
		}
	}

	hErr = ERR_OK;

Error:

	if(pstTsInfoList !=NULL)
		DB_SVC_FreeTsInfoList (pstTsInfoList);

	if(pstNetInfoList!=NULL)
		DB_SVC_FreeNetInfoList(pstNetInfoList);

	if(pstNitTable != NULL)
		PAL_SIRAWPOOL_FreeTable(pstNitTable);

	if (bUpdateInform == TRUE)
	{
		HxLOG_Debug("send eSEVT_DB_NOTIFY_UPDATED \n", __FUNCTION__, __LINE__);
		svc_si_PostMsgToAp(eSEVT_DB_NOTIFY_UPDATED, pstLive->hAction, 0, 0, 0);
	}

	return hErr;
}

STATIC HERROR xsvc_si_LiveCheckAuthorityAndFTAFromSdt_Ukdtt(svcSi_LiveData_t *pstLive, SvcSi_Arg_SecCb_t *pstSecCb, HUINT32 ulHead)
{
	HERROR		 		 hErr = ERR_FAIL;
	HBOOL				 bUpdateInform = FALSE, bFoundPrivate = FALSE;
	HUINT16				usRealTsId, usRealOnId;
	HINT32				 nSvcIdx, nSvcCount;
	HUINT32				ulActionId;
	Handle_t				*phSvcList = NULL;
	HUINT32				ulFTAContentMgrValue, ulPrivateSpecifyValue;
	DbSvc_Info_t			*pstSvcInfoList = NULL;
	DbSvc_Condition_t		stSvcCond;

	HxList_t				*pstSdtSvcLoop = NULL, *pstSdtSvcDesLoop = NULL;
	SIxTable_PsiTable_t		*pstSdtTable = NULL, *pstCurrSdt = NULL;
	SIxFtaContentManagementDes_t	*pstFtaContentManagementDesc = NULL;
	SIxDefaultAuthorityDes_t		*pstDefaultAuthorithyDesc = NULL;
	SvcSi_SiSpec_t				unSiSpec;

	/* 속도 향상을 위해 Query를 최대한 하지 않도록 하자 */
	DB_SVC_InitSvcFindCondition (&stSvcCond);
	stSvcCond.nOrgDeliType = eDxDELIVERY_TYPE_TER;

	hErr = DB_SVC_FindSvcHandleList (&stSvcCond, &nSvcCount, &phSvcList);
	if(hErr != ERR_OK || nSvcCount <= 0 || phSvcList == NULL)
	{
		HxLOG_Error("DB_SVC_FindSvcHandleList is Error \n",__FUNCTION__, __LINE__);
		goto Error;
	}

	hErr = DB_SVC_FindSvcInfoList (&stSvcCond, &nSvcCount, &pstSvcInfoList);
	if (hErr != ERR_OK || nSvcCount <= 0 || pstSvcInfoList == NULL)
	{
		HxLOG_Error("DB_SVC_FindSvcInfoList is Error \n");
		goto Error;
	}

	bUpdateInform = FALSE;

	ulActionId = PAL_PIPE_GetActionId(pstLive->hAction);
	if(ulHead == ACT_HEAD)
	{
		pstSdtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtAct, SILIB_TABLE_EXT_ID_ALL);
	}
	else
	{
		pstSdtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtOth, SILIB_TABLE_EXT_ID_ALL);
	}

	if (NULL == pstSdtTable)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable ulActionId=%d, SDT ulHead(%d) \n", ulActionId, ulHead);
		goto Error;
	}

	hErr = svc_si_GetSiSpec (ulActionId, &unSiSpec);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_GetSiSpec err:\n");
		goto Error;
	}

	hErr = svc_si_GetPrivateSpecifyValue(unSiSpec.stLiveSpec.ulActionSpec,&ulPrivateSpecifyValue);
	if(hErr != ERR_OK)
	{
		HxLOG_Error ("svc_si_GetPrivateSpecifyValue err:\n");
		goto Error;
	}

	for (pstCurrSdt = pstSdtTable; pstCurrSdt != NULL; pstCurrSdt = pstCurrSdt->pstNextTable)
	{
		SIxTable_SdtSec_t		*pstSdtSec = &pstCurrSdt->unInfo.stSdt;

		if(pstSdtSec == NULL)
		{
			HxLOG_Warning("pstSdtSec is NULL \n");
			continue;
		}

		/* Get Service Information from db */
		usRealTsId = pstSdtSec->usTsId;
		usRealOnId = pstSdtSec->usOrgNetId;

		HxLOG_Debug("usRealOnId(0x%x),  usRealTsId(0x%x) \n", usRealOnId, usRealTsId);

		for (pstSdtSvcLoop = pstCurrSdt->pst2ndLoop; NULL != pstSdtSvcLoop; pstSdtSvcLoop = pstSdtSvcLoop->next)
		{
			SIxTable_2ndLoop_t	*pstSdtSvcItem = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstSdtSvcLoop);

			bFoundPrivate = FALSE;

			for (pstSdtSvcDesLoop = pstSdtSvcItem->pstDesLoop; NULL != pstSdtSvcDesLoop; pstSdtSvcDesLoop = pstSdtSvcDesLoop->next)
			{
				HUINT8						*pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstSdtSvcDesLoop);
				HUINT8						*aucDefaultAuthority;

				/*****************************/
				/***	  Default Authority 		 ***/
				/*****************************/
				if (*pucRawDes == eSIxDESCTAG_DEFAULT_AUTHORITY)
				{
					pstDefaultAuthorithyDesc = (SIxDefaultAuthorityDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);

					for (nSvcIdx = 0; nSvcIdx < nSvcCount; nSvcIdx ++)
					{
						if (usRealTsId == DbSvc_GetTsId(&pstSvcInfoList[nSvcIdx])
							&& usRealOnId == DbSvc_GetOnId(&pstSvcInfoList[nSvcIdx])
							&& pstSdtSvcItem->unInfo.stSdt.usSvcId == DbSvc_GetSvcId(&pstSvcInfoList[nSvcIdx]))
						{
							DB_SVC_GetServiceDefaultAuthority(&pstSvcInfoList[nSvcIdx], &aucDefaultAuthority);

							if(pstDefaultAuthorithyDesc != NULL)
							{
								// Update Default Authority
								if(0 != HxSTD_StrCmp(pstDefaultAuthorithyDesc->pszDefaultAuthority, aucDefaultAuthority))
								{
									bUpdateInform = TRUE;
									DB_SVC_SetServiceDefaultAuthority(&pstSvcInfoList[nSvcIdx], pstDefaultAuthorithyDesc->pszDefaultAuthority);
									DB_SVC_UpdateServiceInfo (phSvcList[nSvcIdx], &(pstSvcInfoList[nSvcIdx]));
								}
							}
							else if(HxSTD_StrLen(aucDefaultAuthority) > 0)
							{
								/* Clear Default Authority */
								bUpdateInform = TRUE;
								DB_SVC_SetServiceDefaultAuthority(&pstSvcInfoList[nSvcIdx], 0x0);
								DB_SVC_UpdateServiceInfo(phSvcList[nSvcIdx], &(pstSvcInfoList[nSvcIdx]));
							}
						}
					}

					if(pstDefaultAuthorithyDesc != NULL)
					{
						PAL_SIRAWPOOL_FreeDes((SIxDescriptor_t*) pstDefaultAuthorithyDesc);
						pstDefaultAuthorithyDesc = NULL;
					}
				}

				/*****************************/
				/***	  FTA Content Mgr***/
				/*****************************/
				if(*pucRawDes == eSIxDESCTAG_FTA_CONTENT_MANAGEMENT && bFoundPrivate == TRUE)
				{
					pstFtaContentManagementDesc = (SIxFtaContentManagementDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);
					ulFTAContentMgrValue = 0;

					if(pstFtaContentManagementDesc != NULL)
					{
						ulFTAContentMgrValue = (HUINT32)(((HUINT32)eFTA_CONTENT_SVC_LEVEL << 24) & 0xFF000000);
						ulFTAContentMgrValue |= (HUINT32)( (HUINT32)(pstFtaContentManagementDesc->ucDoNotScramble << 16) & 0x00FF0000);
						ulFTAContentMgrValue |= (HUINT32)( (HUINT32)(pstFtaContentManagementDesc->ucCtrlRAccOverINet << 8) & 0x0000FF00);
						ulFTAContentMgrValue |= (HUINT32)( (HUINT32)pstFtaContentManagementDesc->ucDoNotApplyRevocation & 0x000000FF);

						HxLOG_Debug("\t hAction(0x%x) ulActionId(%d) \n", pstLive->hAction,ulActionId );
						HxLOG_Debug("\t Found FTA Contents Management Descript, in SDT\n\t\t-- DoNotScramble:0x%X CtrlRAccOverINet:0x%X, DoNotApplyRevocation:0x%X >>>>> ulFTAContentMgrValue : 0x%x\n",
									pstFtaContentManagementDesc->ucDoNotScramble,pstFtaContentManagementDesc->ucCtrlRAccOverINet,
									pstFtaContentManagementDesc->ucDoNotApplyRevocation, ulFTAContentMgrValue);
					}

					for (nSvcIdx = 0; nSvcIdx < nSvcCount; nSvcIdx ++)
					{
						if (usRealTsId == DbSvc_GetTsId(&pstSvcInfoList[nSvcIdx])
							&& usRealOnId == DbSvc_GetOnId(&pstSvcInfoList[nSvcIdx])
							&& pstSdtSvcItem->unInfo.stSdt.usSvcId == DbSvc_GetSvcId(&pstSvcInfoList[nSvcIdx]))
						{
							if(ulFTAContentMgrValue != 0)
							{
								svc_si_PostMsgToAp (eSEVT_SI_FTA_CONTENTS_SIGNAL, (HUINT32)pstLive->hAction, (HUINT32)phSvcList[nSvcIdx], (HUINT32)ulFTAContentMgrValue, 0);
								if(ulFTAContentMgrValue != XdbSvc_TvAnyTimeGetContentMgr(&pstSvcInfoList[nSvcIdx]))
								{
									HxLOG_Debug("\tChange FTA Content Mgr : SvcName(%s) SvcId(0x%04x) FTA(0x%08x)\n",
												DbSvc_GetName(&pstSvcInfoList[nSvcIdx]), DbSvc_GetSvcId(&pstSvcInfoList[nSvcIdx]), ulFTAContentMgrValue);

									bUpdateInform = TRUE;
									XdbSvc_TvAnyTimeSetContentMgr(&pstSvcInfoList[nSvcIdx], ulFTAContentMgrValue);
									DB_SVC_UpdateServiceInfo (phSvcList[nSvcIdx], &(pstSvcInfoList[nSvcIdx]));
								}
							}
							else
							{
								if(XdbSvc_TvAnyTimeGetContentMgr(&pstSvcInfoList[nSvcIdx]) != 0)
								{
									/* Clear Default Authority */
									bUpdateInform = TRUE;
									XdbSvc_TvAnyTimeSetContentMgr(&pstSvcInfoList[nSvcIdx], 0);
									DB_SVC_UpdateServiceInfo(phSvcList[nSvcIdx], &(pstSvcInfoList[nSvcIdx]));
								}
							}
							break;
						}
					}

					if(pstFtaContentManagementDesc != NULL)
					{
						PAL_SIRAWPOOL_FreeDes((SIxDescriptor_t*) pstFtaContentManagementDesc);
						pstFtaContentManagementDesc = NULL;
					}
				}

				if(*pucRawDes == eSIxDESCTAG_PRIVATE_DATA_SPECIFIER)
				{
					SIxPrivateDataSpecDes_t *pPrivDataSpecDes = (SIxPrivateDataSpecDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);

					if(pPrivDataSpecDes != NULL)
					{
						HxLOG_Debug("\t	in SDT :: ulPrivateDataSpec(0x%x) ulPrivateSpecifyValue (0x%x)  \n", pPrivDataSpecDes->ulPrivateDataSpec, ulPrivateSpecifyValue);

						if (pPrivDataSpecDes->ulPrivateDataSpec == ulPrivateSpecifyValue)
						{
							bFoundPrivate = TRUE;
						}

						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pPrivDataSpecDes);
					}
					else
					{
						HxLOG_Debug("\t	in SDT :: pPrivDataSpecDes is NULL  \n");
					}
				}

				if(*pucRawDes == eSIxDESCTAG_PRIVATE_DATA_INDICATOR)
				{
					bFoundPrivate = FALSE;
				}
			}
		}
	}

	hErr = ERR_OK;

Error:

	/* Free svc list & info */
	if(phSvcList != NULL || pstSvcInfoList != NULL)	{DB_SVC_FreeServiceInfoList (NULL, phSvcList, pstSvcInfoList);}
	// Free SDT Table
	if(pstSdtTable != NULL)						{PAL_SIRAWPOOL_FreeTable(pstSdtTable);}

	if (bUpdateInform == TRUE)
	{
		HxLOG_Debug("send eSEVT_DB_NOTIFY_UPDATED  \n");
		svc_si_PostMsgToAp(eSEVT_DB_NOTIFY_UPDATED, pstLive->hAction, 0, 0, 0);
	}

	return hErr;
}

STATIC HERROR xsvc_si_LiveCheckGuidanceInfoFromSdt_UkDtt(svcSi_LiveData_t *pstLive, SvcSi_Arg_SecCb_t *pstSecCb, HUINT32 ulHead)
{
	HBOOL			bUpdateInform = FALSE, bFoundPrivate = FALSE;
	HERROR			hErr;
	HUINT16 			usRealTsId, usRealOnId;
	HUINT32 			ulPrivateSpecifyValue;
	HINT32				nSvcIdx, nSvcCount;
	HUINT32				ulActionId;
	Handle_t				*phSvcList = NULL;
	DbSvc_Condition_t		stSvcCond;

	DbSvc_Info_t			*pstSvcInfoList = NULL;
	HxLANG_Id_e			eMenuLangId, eCurLangId, eUndLangId;
	SIxTable_PsiTable_t		*pstSdtTable = NULL, *pstCurrSdt = NULL;
	HxList_t				*pstSdtSvcLoop = NULL, *pstSdtSvcDesLoop = NULL;
	SIxUkdttGuidanceDes_t 	*pstUkdttSelGuidanceDesc = NULL;
	SvcSi_SiSpec_t			unSiSpec;

	eMenuLangId = eLangID_MAX;
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_MAIN, (HINT32 *)&eMenuLangId,0);
	if ((hErr != ERR_OK) || (eMenuLangId == eLangID_MAX))
	{
		HxLOG_Error("DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_MAIN) Error!!\n");
		goto Error;
	}

	eUndLangId = eLangID_English;

	/* 속도 향상을 위해 Query를 최대한 하지 않도록 하자 */
	DB_SVC_InitSvcFindCondition (&stSvcCond);
	stSvcCond.nOrgDeliType = eDxDELIVERY_TYPE_TER;

	hErr = DB_SVC_FindSvcHandleList (&stSvcCond, &nSvcCount, &phSvcList);
	if(hErr != ERR_OK || nSvcCount <= 0 || phSvcList == NULL)
	{
		HxLOG_Error("DB_SVC_FindSvcHandleList is Error \n");
		goto Error;
	}

	hErr = DB_SVC_FindSvcInfoList (&stSvcCond, &nSvcCount, &pstSvcInfoList);
	if (hErr != ERR_OK || nSvcCount <= 0 || pstSvcInfoList == NULL)
	{
		HxLOG_Error("DB_SVC_FindSvcInfoList is Error \n");
		goto Error;
	}

	bUpdateInform = FALSE;


	// SDT parsing해서
	ulActionId = PAL_PIPE_GetActionId(pstLive->hAction);
	if(ulHead == ACT_HEAD)
		pstSdtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtAct, SILIB_TABLE_EXT_ID_ALL);
	else
		pstSdtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtOth, SILIB_TABLE_EXT_ID_ALL);

	if (NULL == pstSdtTable)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable ulActionId=%d, SDT ulHead(%d) \n", ulActionId, ulHead);
		goto Error;
	}

	hErr = svc_si_GetSiSpec (ulActionId, &unSiSpec);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_GetSiSpec err:\n");
		goto Error;
	}

	hErr = svc_si_GetPrivateSpecifyValue(unSiSpec.stLiveSpec.ulActionSpec,&ulPrivateSpecifyValue);
	if(hErr != ERR_OK)
	{
		HxLOG_Error ("svc_si_GetPrivateSpecifyValue err:\n");
		goto Error;
	}

	for (pstCurrSdt = pstSdtTable; pstCurrSdt != NULL; pstCurrSdt = pstCurrSdt->pstNextTable)
	{
		SIxTable_SdtSec_t		*pstSdtSec = &pstCurrSdt->unInfo.stSdt;

		if(pstSdtSec == NULL)
		{
			HxLOG_Warning("pstSdtSec is NULL \n");
			continue;
		}

		/* Get Service Information from db */
		usRealTsId = pstSdtSec->usTsId;
		usRealOnId = pstSdtSec->usOrgNetId;

		for (pstSdtSvcLoop = pstCurrSdt->pst2ndLoop; NULL != pstSdtSvcLoop; pstSdtSvcLoop = pstSdtSvcLoop->next)
		{
			SIxTable_2ndLoop_t	*pstSdtSvcItem = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstSdtSvcLoop);

			bFoundPrivate = FALSE;
			pstUkdttSelGuidanceDesc = NULL;

			for (pstSdtSvcDesLoop = pstSdtSvcItem->pstDesLoop; NULL != pstSdtSvcDesLoop; pstSdtSvcDesLoop = pstSdtSvcDesLoop->next)
			{
				HUINT8			*pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstSdtSvcDesLoop);

				if ((*pucRawDes == eSIxDESCTAG_UKDTT_GUIDANCE) && (bFoundPrivate == TRUE))
				{
					SIxUkdttGuidanceDes_t *pstUkdttGuidanceDesc = (SIxUkdttGuidanceDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);

					if (NULL != pstUkdttGuidanceDesc && (pstUkdttGuidanceDesc->ucGuidanceType == 0 || pstUkdttGuidanceDesc->ucGuidanceType == 1) )
					{
						if (pstUkdttSelGuidanceDesc == NULL)
						{
							pstUkdttSelGuidanceDesc = pstUkdttGuidanceDesc;
						}

						eCurLangId = HLIB_LANG_639ToId(pstUkdttGuidanceDesc->unInfo.pUkDttGuidanceInfo->szIso639LangCode);
						if (eCurLangId == eMenuLangId)
						{
							/* Desc Free는 아래에서 사용 후에 */
							pstUkdttSelGuidanceDesc = pstUkdttGuidanceDesc;
							break;
						}
						else if ((eUndLangId != eLangID_MAX) && (eCurLangId == eUndLangId))
						{
							pstUkdttSelGuidanceDesc = pstUkdttGuidanceDesc;
						}

						if(pstUkdttSelGuidanceDesc != pstUkdttGuidanceDesc)
							PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstUkdttGuidanceDesc);
					}
				}
				else if (*pucRawDes == eSIxDESCTAG_PRIVATE_DATA_SPECIFIER)
				{
					SIxPrivateDataSpecDes_t *pPrivDataSpecDes = (SIxPrivateDataSpecDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);

					if(pPrivDataSpecDes != NULL)
					{
						if (pPrivDataSpecDes->ulPrivateDataSpec == ulPrivateSpecifyValue)
						{
							bFoundPrivate = TRUE;
						}

						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pPrivDataSpecDes);
					}
				}
				else if (*pucRawDes == eSIxDESCTAG_PRIVATE_DATA_INDICATOR)
				{
					bFoundPrivate = FALSE;
				}
			}

			/* Update Service Info */
			for (nSvcIdx = 0; nSvcIdx < nSvcCount; nSvcIdx ++)
			{
				if (usRealTsId == DbSvc_GetTsId(&pstSvcInfoList[nSvcIdx])
					&& usRealOnId == DbSvc_GetOnId(&pstSvcInfoList[nSvcIdx])
					&& pstSdtSvcItem->unInfo.stSdt.usSvcId == DbSvc_GetSvcId(&pstSvcInfoList[nSvcIdx]))
				{
					HxLOG_Print("[%s:%d]phSvcList[%d] : %x\n",__FUNCTION__,__LINE__,nSvcIdx,phSvcList[nSvcIdx]);
					if(pstUkdttSelGuidanceDesc)
					{
						HxLOG_Print("[%s:%d] svcid(%d) pstUkdttSelGuidanceDesc(%p) guidcneType(%d)guidanceStr(%s) \n\n",__FUNCTION__,__LINE__,pstSdtSvcItem->unInfo.stSdt.usSvcId,pstUkdttSelGuidanceDesc,pstUkdttSelGuidanceDesc->ucGuidanceType,pstUkdttSelGuidanceDesc->unInfo.pUkDttGuidanceInfo->pszGuidance);
						// Update Guidance Info
						if(pstUkdttSelGuidanceDesc->ucGuidanceType != XdbSvc_TvAnyTimeGetGuidanceType(&pstSvcInfoList[nSvcIdx])
							|| (pstUkdttSelGuidanceDesc->ucGuidanceType == 0x01 && pstUkdttSelGuidanceDesc->unInfo.pUkDttGuidanceInfo->ucGuidanceMode != XdbSvc_TvAnyTimeGetGuidanceMode(&pstSvcInfoList[nSvcIdx]))
							|| 0 != HxSTD_StrCmp(pstUkdttSelGuidanceDesc->unInfo.pUkDttGuidanceInfo->pszGuidance, XdbSvc_TvAnyTimeGetGuidanceStr(&pstSvcInfoList[nSvcIdx])))
						{
							bUpdateInform = TRUE;

							XdbSvc_TvAnyTimeSetGuidanceType(&pstSvcInfoList[nSvcIdx], pstUkdttSelGuidanceDesc->ucGuidanceType);
							if(pstUkdttSelGuidanceDesc->ucGuidanceType == 0x01)
							{
								XdbSvc_TvAnyTimeSetGuidanceMode(&pstSvcInfoList[nSvcIdx], pstUkdttSelGuidanceDesc->unInfo.pUkDttGuidanceInfo->ucGuidanceMode);
							}
							else
							{
								XdbSvc_TvAnyTimeSetGuidanceMode(&pstSvcInfoList[nSvcIdx], 0);
							}

							XdbSvc_TvAnyTimeSetGuidanceStr(&pstSvcInfoList[nSvcIdx], pstUkdttSelGuidanceDesc->unInfo.pUkDttGuidanceInfo->pszGuidance);

							HxLOG_Debug("GuidanceMode(%d) GuidanceType(%d) GuidanceStr (%s) \n",
								XdbSvc_TvAnyTimeGetGuidanceMode(&pstSvcInfoList[nSvcIdx]),
								XdbSvc_TvAnyTimeGetGuidanceType(&pstSvcInfoList[nSvcIdx]),
								XdbSvc_TvAnyTimeGetGuidanceStr(&pstSvcInfoList[nSvcIdx]));
							DB_SVC_UpdateServiceInfo(phSvcList[nSvcIdx], &(pstSvcInfoList[nSvcIdx]));

						}
					}
					else if( (HxSTD_StrLen(XdbSvc_TvAnyTimeGetGuidanceStr(&pstSvcInfoList[nSvcIdx])) > 0)
						|| (XdbSvc_TvAnyTimeGetGuidanceType(&pstSvcInfoList[nSvcIdx]) != 0xFF))
					{
						HxLOG_Print("[%s:%d] svcid(%d) CLEAR \n\n",__FUNCTION__,__LINE__,pstSdtSvcItem->unInfo.stSdt.usSvcId);
						/* Clear Default Authority */
						bUpdateInform = TRUE;

						XdbSvc_TvAnyTimeSetGuidanceType(&pstSvcInfoList[nSvcIdx], 0xFF);
						XdbSvc_TvAnyTimeSetGuidanceMode(&pstSvcInfoList[nSvcIdx], 0);
						XdbSvc_TvAnyTimeResetGuidanceStr(&pstSvcInfoList[nSvcIdx]);

						DB_SVC_UpdateServiceInfo(phSvcList[nSvcIdx], &(pstSvcInfoList[nSvcIdx]));
					}
					break;
				}
			}

			/* Free Select Guidance Info */
			if(pstUkdttSelGuidanceDesc != NULL)
			{
				PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstUkdttSelGuidanceDesc);
				pstUkdttSelGuidanceDesc = NULL;
			}
		}
	}

	hErr = ERR_OK;

Error :

	/* Free svc list & info */
	if(phSvcList!= NULL || pstSvcInfoList != NULL)	{	DB_SVC_FreeServiceInfoList (NULL, phSvcList, pstSvcInfoList);}
	// Free SDT Table
	if(pstSdtTable != NULL)						{	PAL_SIRAWPOOL_FreeTable(pstSdtTable);	}

	if (bUpdateInform == TRUE)
	{
		HxLOG_Print("[%s:%d] bUpdateInform is TRUE  pstLive->hAction:%d\n\n",__FUNCTION__,__LINE__,pstLive->hAction);
		svc_si_PostMsgToAp(eSEVT_DB_NOTIFY_UPDATED, pstLive->hAction, 0, 0, 0);
	}

	return hErr;
}

STATIC HERROR xsvc_si_LiveCheckRunningStatusFromSdt_UkDtt(svcSi_LiveData_t *pstLive, SvcSi_Arg_SecCb_t *pstSecCb, HUINT32 ulHead, HUINT8 *pucRunningStatus)
{
	HERROR			hError;
	HUINT32			ulActionId;
	DbSvc_Info_t		stSvcInfo;

	SIxTable_PsiTable_t 	*pstSdtTable = NULL, *pstCurrSdt = NULL;
	HxList_t				*pstSdtSvcLoop = NULL;

	hError = DB_SVC_GetServiceInfo(pstLive->hService , &stSvcInfo);
	if(hError != ERR_OK)
	{
		HxLOG_Error("DB_SVC_GetServiceInfo Error \n");
		goto Error;
	}

	ulActionId = PAL_PIPE_GetActionId(pstLive->hAction);
	if(ulHead == ACT_HEAD)
		pstSdtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtAct, SILIB_TABLE_EXT_ID_ALL);
	else
		pstSdtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtOth, SILIB_TABLE_EXT_ID_ALL);

	if (NULL == pstSdtTable)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable ulActionId=%d, SDT ulHead(%d) \n", ulActionId, ulHead);
		goto Error;
	}

	for (pstCurrSdt = pstSdtTable; pstCurrSdt != NULL; pstCurrSdt = pstCurrSdt->pstNextTable)
	{
		SIxTable_SdtSec_t		*pstSdtSec = &pstCurrSdt->unInfo.stSdt;

		if(pstSdtSec == NULL)
		{
			HxLOG_Warning("pstSdtSec is NULL \n");
			continue;
		}

		for (pstSdtSvcLoop = pstCurrSdt->pst2ndLoop; NULL != pstSdtSvcLoop; pstSdtSvcLoop = pstSdtSvcLoop->next)
		{
			SIxTable_2ndLoop_t	*pstSdtSvcItem = NULL;
			SIxTable_SdtService_t	*pstSdtSvc = NULL;

			pstSdtSvcItem = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstSdtSvcLoop);
			if(pstSdtSvcItem == NULL)
			{
				HxLOG_Warning("pstSdtSvcItem is NULL \n");
				continue;
			}

			pstSdtSvc = &pstSdtSvcItem->unInfo.stSdt;
			if(pstSdtSvc == NULL)
			{
				HxLOG_Warning("pstSdtSvc is NULL \n");
				continue;
			}

			if(pstSdtSvcItem->unInfo.stSdt.usSvcId == DbSvc_GetSvcId(&stSvcInfo))
			{
				HxLOG_Debug("\tSvcId:0x%X bEitSched:%d bEitPreFollow:%d ucRunningStatus:0x%X bCaFreeL[%d\n\r",
					pstSdtSvc->usSvcId,pstSdtSvc->bEitSched,pstSdtSvc->bEitPreFollow,
					pstSdtSvc->ucRunningStatus,pstSdtSvc->bCaFree);

				*pucRunningStatus = pstSdtSvcItem->unInfo.stSdt.ucRunningStatus;

				if(pstSdtTable!= NULL)
					PAL_SIRAWPOOL_FreeTable(pstSdtTable);

				return ERR_OK;
			}
		}
	}
Error:

	if(pstSdtTable!= NULL)
		PAL_SIRAWPOOL_FreeTable(pstSdtTable);

	return ERR_FAIL;
}

STATIC HERROR xsvc_si_FindTripleIdFromLinkageDescInSdt_UkDtt(svcSi_LiveData_t *pstLive, SvcSi_Arg_SecCb_t *pstSecCb, HUINT32 ulHead, DbSvc_TripleId_t *pstTripleId)
{
	HERROR			hError = ERR_FAIL;
	HBOOL			bFound = FALSE;
	HUINT32			ulActionId;
	DbSvc_Info_t		stSvcInfo;
	DbSvc_TsInfo_t	stTsInfo;

	SIxTable_PsiTable_t 	*pstSdtTable = NULL, *pstCurrSdt = NULL;
	HxList_t				*pstSdtSvcLoop = NULL, *pstSdtSvcDesLoop = NULL;

	HxSTD_memset(&stSvcInfo, 0x00, sizeof(DbSvc_Info_t));
	HxSTD_memset(&stTsInfo, 0x00, sizeof(DbSvc_TsInfo_t));

	hError = DB_SVC_GetServiceInfo(pstLive->hService , &stSvcInfo);
	if(hError != ERR_OK)
	{
		HxLOG_Error("DB_SVC_GetServiceInfo Error \n");
		goto Error;
	}

	hError = DB_SVC_GetTsInfo(DbSvc_GetTsIdx(&stSvcInfo), &stTsInfo);
	if(hError != ERR_OK)
	{
		HxLOG_Error("DB_SVC_GetServiceInfo Error \n");
		goto Error;
	}

	// SDT parsing해서
	ulActionId = PAL_PIPE_GetActionId(pstLive->hAction);
	if(ulHead == ACT_HEAD)
		pstSdtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtAct, SILIB_TABLE_EXT_ID_ALL);
	else
		pstSdtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtOth, SILIB_TABLE_EXT_ID_ALL);

	if (NULL == pstSdtTable)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable ulActionId=%d, SDT ulHead(%d) \n", ulActionId, ulHead);
		goto Error;
	}

	for (pstCurrSdt = pstSdtTable; pstCurrSdt != NULL; pstCurrSdt = pstCurrSdt->pstNextTable)
	{
		for (pstSdtSvcLoop = pstCurrSdt->pst2ndLoop; NULL != pstSdtSvcLoop; pstSdtSvcLoop = pstSdtSvcLoop->next)
		{
			SIxTable_2ndLoop_t	*pstSdtSvcItem = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstSdtSvcLoop);

			for (pstSdtSvcDesLoop = pstSdtSvcItem->pstDesLoop; NULL != pstSdtSvcDesLoop; pstSdtSvcDesLoop = pstSdtSvcDesLoop->next)
			{
				HUINT8			*pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstSdtSvcDesLoop);

				if (*pucRawDes == eSIxDESCTAG_LINKAGE)
				{
					SIxLinkDes_t *pstLinkDesc = (SIxLinkDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);

					if(pstLinkDesc != NULL)
					{
						if(pstLinkDesc->ucLinkType == xSvcSi_UKDTT_SERVICE_REPLACEMENT_TYPE
							&& pstLinkDesc->usOrgNetId == DbSvc_GetOnId(&stSvcInfo)
							&& pstLinkDesc->usTsId ==  DbSvc_GetTsId(&stSvcInfo)
							&& pstLinkDesc->usSvcId ==  DbSvc_GetSvcId(&stSvcInfo))
						{
							pstTripleId->usOnId = pstLinkDesc->usOrgNetId;
							pstTripleId->usTsId = pstLinkDesc->usTsId;
							pstTripleId->usSvcId= pstLinkDesc->usSvcId;

							bFound = TRUE;
						}

						PAL_SIRAWPOOL_FreeDes((SIxDescriptor_t*)pstLinkDesc);

						if( bFound == TRUE)
						{
							if(pstSdtTable != NULL)
								PAL_SIRAWPOOL_FreeTable(pstSdtTable);

							return ERR_OK;
						}
					}
				}
			}
		}
	}

Error:

	if(pstSdtTable != NULL)
		PAL_SIRAWPOOL_FreeTable(pstSdtTable);

	return hError;
}

STATIC HERROR xsvc_si_LiveProcessRunningStatusFromSdt_UkDtt(svcSi_LiveData_t *pstLive, SvcSi_Arg_SecCb_t *pstSecCb, HUINT32 ulHead, HUINT8 ucRunningStatus)
{
	HERROR	hErr;


	if(ulHead == ACT_HEAD)
	{
		if(ucRunningStatus == eSdtRunningStatusType_NOT_RUNNING)
		{
			Handle_t		hSupSvcHandle;
			DbSvc_TripleId_t stTripleId;

			HxSTD_memset (&stTripleId, 0, sizeof(DbSvc_TripleId_t));

			hErr = xsvc_si_FindTripleIdFromLinkageDescInSdt_UkDtt(pstLive, pstSecCb, ulHead, &stTripleId);
			if(hErr == ERR_OK)
			{

				if (ERR_OK == DB_SVC_FindServiceHandleByTripleId(eDxDELIVERY_TYPE_TER,stTripleId,&hSupSvcHandle))
				{
					pstLive->hSupService = hSupSvcHandle;

					svc_si_PostMsgToAp(eSEVT_SI_SDT_RUNNING_STATUS_CHANGED, pstLive->hAction, pstLive->hSupService, eSdtRunningStatusChanged_ByUkDtt, ucRunningStatus);
					s_bUKServiceReplacement = TRUE;
				}

			}

		}
		else if(ucRunningStatus == eSdtRunningStatusType_RUNNING)
		{
			svc_si_PostMsgToAp(eSEVT_SI_SDT_RUNNING_STATUS_CHANGED, pstLive->hAction, pstLive->hService, eSdtRunningStatusChanged_ByUkDtt, ucRunningStatus);
			s_bUKServiceReplacement = FALSE;
		}

		s_eRunningStatus = ucRunningStatus;
	}
	else
	{
		if(ucRunningStatus == eSdtRunningStatusType_RUNNING )
		{
			svc_si_PostMsgToAp(eSEVT_SI_SDT_RUNNING_STATUS_CHANGED, pstLive->hAction, pstLive->hService, eSdtRunningStatusChanged_ByUkDtt, ucRunningStatus);
			pstLive->hSupService = pstLive->hService;
			s_bUKServiceReplacement = FALSE;
		}

		s_eRunningStatus = ucRunningStatus;
	}
	HxLOG_Print("[%s : %d]s_eRunningStatus (%d),ucRunningStatus (%d) \n",__FUNCTION__, __LINE__, s_eRunningStatus,ucRunningStatus);

	return ERR_OK;

}

STATIC HERROR xsvc_si_LiveCheckRCTAvailableFromPmt_UkDtt(svcSi_LiveData_t *pstLive, SvcSi_Arg_SecCb_t *pstSecCb, HUINT16 *pusRctPid)
{
	HERROR			hError = ERR_FAIL;
	DbSvc_Info_t		stSvcInfo;
	HUINT32 			ulActionId;
	SIxTable_PsiTable_t 	*pstPmtTable = NULL, *pstCurrPmt = NULL;
	HxList_t				*pstPmt2ndLoop = NULL;
	HxList_t				*pst2ndDesLoop = NULL;
	SIxRelatedContentDes_t *pstRelContentDes = NULL;

	HxSTD_memset(&stSvcInfo, 0x00, sizeof(DbSvc_Info_t));

	hError = DB_SVC_GetServiceInfo(pstLive->hService, &stSvcInfo);
	if (hError != ERR_OK)
	{
		HxLOG_Error ("invalid Service Handle(0x%X)\n", pstLive->hService);
		goto Error;
	}

	HxLOG_Debug("SvcId (0x%x), SvcName(%s), hError(0x%x)\n", DbSvc_GetSvcId(&stSvcInfo), DbSvc_GetName(&stSvcInfo), hError);

	// PMT parsing해서
	ulActionId = PAL_PIPE_GetActionId(pstLive->hAction);
	pstPmtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Pmt, SILIB_TABLE_EXT_ID_ALL);
	if (NULL == pstPmtTable)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable ulActionId=%d, PMT\n", ulActionId);
		goto Error;
	}

	for (pstCurrPmt = pstPmtTable; pstCurrPmt != NULL; pstCurrPmt = pstCurrPmt->pstNextTable)
	{
		SIxTable_PmtSec_t		*pstPmtSec = &pstCurrPmt->unInfo.stPmt;

		if(pstPmtSec == NULL || pstPmtSec->usProgNum != DbSvc_GetSvcId(&stSvcInfo))
		{
			HxLOG_Warning("pstPmtSec is NULL, or ServiceId is different \n");
			continue;
		}

		for (pstPmt2ndLoop = pstCurrPmt->pst2ndLoop; NULL != pstPmt2ndLoop; pstPmt2ndLoop = pstPmt2ndLoop->next)
		{
			SIxTable_2ndLoop_t		*pst2ndItem = HLIB_LIST_Data (pstPmt2ndLoop);
			SIxTable_PmtElement_t		*pstPmtElem;

			if (NULL == pst2ndItem)
			{
				HxLOG_Warning("pst2ndItem == NULL, continue \n");
				continue;
			}

			pstPmtElem = (SIxTable_PmtElement_t*)&(pst2ndItem->unInfo.stPmt);
			if(pstPmtElem  == NULL) {continue;}

			for (pst2ndDesLoop = pst2ndItem->pstDesLoop; pst2ndDesLoop != NULL; pst2ndDesLoop = pst2ndDesLoop->next)
			{
				HUINT8				*pucRawDes = (HUINT8 *)HLIB_LIST_Data (pst2ndDesLoop);

				if (pucRawDes == NULL)
				{
					HxLOG_Warning("pucRawDes == NULL, continue \n");
					continue;
				}

				if (*pucRawDes == eSIxDESCTAG_RELATED_CONTENT)
				{
					pstRelContentDes = (SIxRelatedContentDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucRawDes);
					if(pstRelContentDes != NULL)
					{
						*pusRctPid = pstPmtElem->usElemPid;

						HxLOG_Debug("Found RCT PID (0x%x) \n", pstPmtElem->usElemPid);

						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstRelContentDes);

						if(pstPmtTable != NULL)
							PAL_SIRAWPOOL_FreeTable(pstPmtTable);

						return ERR_OK;
					}

					if(pstRelContentDes != NULL)
					{
						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstRelContentDes);
						pstRelContentDes = NULL;
					}
				}
			}
		}
	}

Error:

	if(pstPmtTable != NULL)
		PAL_SIRAWPOOL_FreeTable(pstPmtTable);

	*pusRctPid = PID_NULL;

	return ERR_FAIL;

}

STATIC HERROR xsvc_si_LiveProcessComponentDescFromEit_UkDtt(svcSi_LiveData_t *pstLive, SvcSi_Arg_SecCb_t *pstSecCb)
{
	HERROR			hError = ERR_FAIL;
	DbSvc_Info_t		stSvcInfo;
	HUINT32 			ulActionId;
	SIxTable_PsiTable_t 	*pstEitTable = NULL, *pstCurrEit = NULL;
	DbSvc_TripleId_t		stTripleId;
	SvcSi_AudioList_t 		stAudioList;
	SvcSi_AudCpnt_e		eCpntMode;
//	DxStereoSelect_e		eStrereoMode = eDxSTEREO_SELECT_UNKNOWN, eSvcMode;
	DxStereoFormat_e		eStereoFormat = eDxSTEREO_FORMAT_UNKNOWN;
//	DxStereoSelect_e		eSvcStereoSelect;

	HBOOL				bUpdate = FALSE;


	ulActionId = PAL_PIPE_GetActionId(pstLive->hAction);

	HxSTD_memset(&stSvcInfo, 0x00, sizeof(DbSvc_Info_t));
	HxSTD_memset(&stTripleId, 0x00, sizeof(DbSvc_TripleId_t));
	HxSTD_MemSet (&stAudioList, 0, sizeof(SvcSi_AudioList_t));

	hError = DB_SVC_GetServiceInfo(pstLive->hService, &stSvcInfo);
	if (hError != ERR_OK)
	{
		HxLOG_Error ("invalid Service Handle(0x%X)\n", pstLive->hService);
		goto Error;
	}

	stTripleId.usOnId = DbSvc_GetOnId(&stSvcInfo);
	stTripleId.usTsId = DbSvc_GetTsId(&stSvcInfo);
	stTripleId.usSvcId = DbSvc_GetSvcId(&stSvcInfo);

	hError = svc_si_LibGetAudioInfoList(ulActionId, stTripleId.usSvcId, &stAudioList);
	if (ERR_OK != hError)
	{
		HxLOG_Error ("SVC_SI_GetAudioList Error. ulActionId(0x%08x)\n", ulActionId);
		goto Error;
	}

	pstEitTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_EitActPf, stTripleId.usSvcId);
	if (NULL == pstEitTable)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable ulActionId=%d, EIT\n", ulActionId);
		goto Error;
	}

	for (pstCurrEit = pstEitTable; pstCurrEit != NULL; pstCurrEit = pstCurrEit->pstNextTable)
	{
		HxList_t	*pstElemItem = NULL;

		if ((pstCurrEit->unInfo.stEit.usOrgNetId	!= stTripleId.usOnId)
			|| (pstCurrEit->unInfo.stEit.usTsId	!= stTripleId.usTsId)
			|| (pstCurrEit->unInfo.stEit.usSvcId	!= stTripleId.usSvcId))
		{
			continue;
		}

		for (pstElemItem = pstCurrEit->pst2ndLoop; pstElemItem != NULL; pstElemItem = pstElemItem->next)
		{
			SIxTable_2ndLoop_t		*pst2ndLoop = HLIB_LIST_Data (pstElemItem);
			HxList_t				*pstDescList = NULL;

			if (NULL == pst2ndLoop) 										{ continue; }

			for (pstDescList = pst2ndLoop->pstDesLoop; NULL != pstDescList; pstDescList = pstDescList->next)
			{
				HUINT8					*pucDesRaw = (HUINT8 *)HLIB_LIST_Data (pstDescList);

				switch (*pucDesRaw)
				{
					case eSIxDESCTAG_COMPONENT:
					{
						SIxCompDes_t *pstCompDes = NULL;

						pstCompDes = (SIxCompDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);
						if(pstCompDes != NULL)
						{
							HUINT32	ulAudioCnt;

							for(ulAudioCnt = 0 ; ulAudioCnt < stAudioList.nAudio; ulAudioCnt++)
							{
								if((pstCompDes->ucStmContent == eEsStreamContent_MPEG1_Audio)  && (pstCompDes->ucCompTag == stAudioList.pstAudioInfo[ulAudioCnt].ucComponentTag))
								{
									switch (pstCompDes->ucCompType)
									{
										case 0x01:
											eCpntMode= eSI_AUD_CPNT_SINGLE_MONO;
											eStereoFormat = eDxSTEREO_FORMAT_MONO;
											break;

										case 0x02:
											eCpntMode= eSI_AUD_CPNT_DUAL_MONO;
											eStereoFormat = eDxSTEREO_FORMAT_DUALMONO;
											break;

										case 0x03:
											eCpntMode= eSI_AUD_CPNT_STEREO;
											eStereoFormat = eDxSTEREO_FORMAT_2CH;
											break;

										case 0x04:
											eCpntMode= eSI_AUD_CPNT_MULTILINGUAL;
											eStereoFormat = eDxSTEREO_FORMAT_MULTI;
											break;

										case 0x05:
											eCpntMode= eSI_AUD_CPNT_SURROUND;
											eStereoFormat = eDxSTEREO_SELECT_STEREO;
											break;

										default:
											break;

									}
								}
							}

							PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t*)pstCompDes);
						}
					}
					break;

					default:
						// do nothing..
						break;
				}
			}
		}
	}

#if 0
	eSvcStereoSelect = ((stSvcInfo.eSoundMode == eDxSTEREO_SELECT_MONOLEFT) || (stSvcInfo.eSoundMode == eDxSTEREO_SELECT_MONORIGHT))?eDxSTEREO_SELECT_MONOLEFT: eDxSTEREO_SELECT_STEREO;



	eStereoFormat

	if((eStereoFormat != eDxSTEREO_FORMAT_UNKNOWN && eStereoFormat != eDxSTEREO_FORMAT_MAX_NUM) && (eSvcMode != eStrereoMode))
	{
		stSvcInfo.eSoundMode = eStrereoMode;
		DB_SVC_UpdateServiceInfo (pstLive->hService, &stSvcInfo);

		bUpdate = TRUE;
	}
	else if(eStereoFormat == eDxSTEREO_FORMAT_UNKNOWN)
	{
		PalAudio_StreamInfo_t stStreamInfo;

		hError = PAL_AUDIO_GetStreamInfo(&stStreamInfo);	//EIT가 없다면 MPEG으로 가자
		if(hError  == ERR_OK )
		{
			switch(stStreamInfo.eStereoType)
			{
			case eDxSTEREO_FORMAT_2CH: 		eStrereoMode = eDxSTEREO_SELECT_STEREO; break;
			case eDxSTEREO_FORMAT_MULTI: 		eStrereoMode = eDxSTEREO_SELECT_STEREO; break;
			case eDxSTEREO_FORMAT_MONO: 		eStrereoMode = eDxSTEREO_SELECT_MONOLEFT; break;
			case eDxSTEREO_FORMAT_DUALMONO: 	eStrereoMode = eDxSTEREO_SELECT_MONOLEFT; break;
			default : 	eStrereoMode = eDxSTEREO_SELECT_UNKNOWN; break;
			}

			if((stStreamInfo.eStereoType != eDxSTEREO_FORMAT_UNKNOWN)
				&& (eSvcMode != eStrereoMode))
			{
				stSvcInfo.eSoundMode = eStrereoMode;
				DB_SVC_UpdateServiceInfo (pstLive->hService, &stSvcInfo);
				bUpdate = TRUE;
			}
		}
	}
#endif
	hError = ERR_OK;

Error:

	if(pstEitTable != NULL)	{	PAL_SIRAWPOOL_FreeTable(pstEitTable);}

	if(bUpdate== TRUE)
	{
		svc_si_PostMsgToAp (eSEVT_SI_PMT, pstLive->hAction, 0, 0, 0);
	}

	return hError ;

}

#define ___LOCAL_PROCESS_TABLE___
STATIC HERROR xsvc_si_LiveProcessPmt_UkDtt  (svcSi_LiveData_UkDtt_t *pstLive, SvcSi_Arg_SecCb_t *pstSecCb)
{
	HUINT16	usRctPid;
	HERROR	hError = ERR_FAIL;

	if(pstLive ==NULL || pstSecCb == NULL)
	{
		HxLOG_Error("Invalid parameter (pstLive is NULL || pstSecCb is  NULL ) !! \n");
		return hError;
	}

	switch (pstSecCb->ulTableStatus)
	{
	case eSI_TABLE_TIMEOUT:
	case eSI_TABLE_FAIL:
		HxLOG_Warning("pstSecCb->ulTableStatus (0x%x) Error(0x%x)  \n", pstSecCb->ulTableStatus, hError);
		return hError;
	default :
		break;
	}

	if(pstLive->bUseRCT ==TRUE)
	{
		hError =xsvc_si_LiveCheckRCTAvailableFromPmt_UkDtt(&pstLive->stBase, pstSecCb, &usRctPid);
		if(hError == ERR_OK && usRctPid != PID_NULL)
		{
			HUINT32	ulFilterID;

			hError = svc_si_FilterStartTableSection (pstLive->stBase.hAction, pstLive->stBase.ulDemuxId, eSIxTABLETYPE_Rct, SILIB_TABLE_EXT_ID_ALL, usRctPid, eSI_TABLE_FILTER_MODE_AllMonitor, &ulFilterID);
			if(hError == ERR_OK)
			{
				HxLOG_Debug("\n\nStart Rct Table (RctPid : 0x%04x) usFilterID(0x%x) \n\n\n\n", usRctPid, ulFilterID);
				pstLive->stBase.eTableFiltering |=bSI_FILTERING_RCT;
			}
			else
			{
				HxLOG_Warning("svc_si_FilterStartTableSection Error(0x%x)\n", hError);
				svc_si_PostMsgToAp (eSEVT_SI_RCT_NONE, pstLive->stBase.hAction, 0, 0, 0);
			}
		}
		else
		{
			HxLOG_Warning("xsvc_si_LiveCheckRCTAvailableInPmt_UkDtt Error (0x%x)\n", hError);
		}
	}

	return hError;
}

STATIC HERROR xsvc_si_LiveProcessNit_UkDtt  (svcSi_LiveData_UkDtt_t *pstLive, SvcSi_Arg_SecCb_t *pstSecCb, HUINT32 ulHead)
{
	HERROR	hError = ERR_FAIL;

	if (pstLive == NULL || pstSecCb == NULL)
	{
		HxLOG_Error("Invalid parameter (pstLive is NULL || pstSecCb is  NULL ) !! \n");
		return hError;
	}

	switch (pstSecCb->ulTableStatus)
	{
	case eSI_TABLE_TIMEOUT:
	case eSI_TABLE_FAIL:
		HxLOG_Warning("pstSecCb->ulTableStatus (0x%x) Error(0x%x)  \n", pstSecCb->ulTableStatus, hError);
		return hError;
	default :
		break;
	}

	hError = xsvc_si_NcdProcessFromNit_UkDtt(&pstLive->stBase, pstSecCb , ulHead);
	if(hError != ERR_OK)
	{
		HxLOG_Warning("xsvc_si_NcdProcessFromNit_UkDtt Error (0x%x) \n", hError);
	}

	hError = xsvc_si_LiveCheckAuthorityAndFTAFromNit_Ukdtt(&pstLive->stBase, pstSecCb , ulHead);
	if(hError != ERR_OK)
	{
		HxLOG_Warning("xsvc_si_LiveCheckAuthorityAndFTAFromNit_Ukdtt Error (0x%x) \n", hError);
	}

	return hError;
}

STATIC HERROR xsvc_si_LiveProcessSdt_UkDtt  (svcSi_LiveData_UkDtt_t *pstLive, SvcSi_Arg_SecCb_t *pstSecCb, HUINT32 ulHead)
{
	HERROR	hError = ERR_FAIL;
	HUINT8	ucRunningStatus;


	if(pstLive ==NULL || pstSecCb == NULL)
	{
		HxLOG_Error("Invalid parameter (pstLive is NULL Or pstSecCb is NULL!! \n");
		return hError;
	}

	switch (pstSecCb->ulTableStatus)
	{
	case eSI_TABLE_TIMEOUT:
	case eSI_TABLE_FAIL:
		//HxLOG_Warning("pstSecCb->ulTableStatus (0x%x) Error(0x%x)  \n", pstSecCb->ulTableStatus, hError);
		return hError;
	default :
		break;
	}

	hError = xsvc_si_LiveCheckAuthorityAndFTAFromSdt_Ukdtt(&pstLive->stBase, pstSecCb, ulHead);
	if(hError != ERR_OK)
	{
		HxLOG_Warning("xsvc_si_LiveCheckAuthorityAndFTAFromSdt_Ukdtt Error(0x%x)  \n", hError);
	}

	hError = xsvc_si_LiveCheckGuidanceInfoFromSdt_UkDtt(&pstLive->stBase, pstSecCb, ulHead);
	if(hError != ERR_OK)
	{
		HxLOG_Warning("xsvc_si_LiveCheckGuidanceInfoInSdt_UkDtt Error (0x%x) \n", hError);
	}

	hError = xsvc_si_LiveCheckRunningStatusFromSdt_UkDtt(&pstLive->stBase, pstSecCb, ulHead, &ucRunningStatus);
	if(hError != ERR_OK)
	{
		HxLOG_Warning("xsvc_si_LiveCheckRunningStatusforCurServiceFromSdt_UkDtt Error (0x%x) \n", hError);
	}
// TODO... 수정 필요 위에서 처리해주어야할지... pvr에서 먼저 처리하면 live에서 처리가 안됨...
	if((hError == ERR_OK) && ucRunningStatus && (ucRunningStatus != s_eRunningStatus))
	{
		xsvc_si_LiveProcessRunningStatusFromSdt_UkDtt(&pstLive->stBase, pstSecCb, ulHead, ucRunningStatus);
	}


	return hError;
}
STATIC HERROR xsvc_si_LiveProcessOthSdt_UkDtt  (svcSi_LiveData_UkDtt_t *pstLive, SvcSi_Arg_SecCb_t *pstSecCb, HUINT32 ulHead)
{
	HERROR	hError = ERR_FAIL;
	HUINT8	ucRunningStatus;

	HxLOG_Print("[%s:%d]    (+)\n",__FUNCTION__,__LINE__);
	if(pstLive ==NULL || pstSecCb == NULL)
	{
		HxLOG_Error("Invalid parameter (pstLive is NULL Or pstSecCb is NULL!! \n");
		return hError;
	}

	switch (pstSecCb->ulTableStatus)
	{
	case eSI_TABLE_TIMEOUT:
	case eSI_TABLE_FAIL:
		//HxLOG_Warning("pstSecCb->ulTableStatus (0x%x) Error(0x%x)  \n", pstSecCb->ulTableStatus, hError);
		return hError;
	default :
		break;
	}


	if(s_bUKServiceReplacement)
   {

	   // pstLive->usTsUniqId : SDT Other 일지라도 현재 TP usTsUniqId를 보내 줘야 한다.
	   hError = xsvc_si_LiveCheckRunningStatusFromSdt_UkDtt(&pstLive->stBase, pstSecCb, ulHead, &ucRunningStatus);

	   if((hError == ERR_OK) && ucRunningStatus && (ucRunningStatus != s_eRunningStatus))
	   {
		   xsvc_si_LiveProcessRunningStatusFromSdt_UkDtt(&pstLive->stBase, pstSecCb, ulHead, ucRunningStatus);

	   }
   }

	hError = xsvc_si_LiveCheckAuthorityAndFTAFromSdt_Ukdtt(&pstLive->stBase, pstSecCb, ulHead);
	if(hError != ERR_OK)
	{
		HxLOG_Warning("xsvc_si_LiveCheckAuthorityAndFTAFromSdt_Ukdtt Error(0x%x)  \n", hError);
	}

	hError = xsvc_si_LiveCheckGuidanceInfoFromSdt_UkDtt(&pstLive->stBase, pstSecCb, ulHead);
	if(hError != ERR_OK)
	{
		HxLOG_Warning("xsvc_si_LiveCheckGuidanceInfoInSdt_UkDtt Error (0x%x) \n", hError);
	}

	HxLOG_Print("[%s:%d]    (-)\n",__FUNCTION__,__LINE__);
	return hError;
}

STATIC HERROR xsvc_si_LiveProcessEit_UkDtt  (svcSi_LiveData_UkDtt_t *pstLive, SvcSi_Arg_SecCb_t *pstSecCb)
{
	HERROR	hError = ERR_FAIL;

	if(pstLive ==NULL || pstSecCb == NULL)
	{
		HxLOG_Error("Invalid parameter (pstLive is NULL Or pstSecCb is NULL!! \n");
		return hError;
	}

	switch (pstSecCb->ulTableStatus)
	{
	case eSI_TABLE_TIMEOUT:
	case eSI_TABLE_FAIL:
		HxLOG_Warning("pstSecCb->ulTableStatus (0x%x) Error(0x%x)  \n", pstSecCb->ulTableStatus, hError);
		return hError;
	default :
		break;
	}

	hError = xsvc_si_LiveProcessComponentDescFromEit_UkDtt(&pstLive->stBase, pstSecCb);
	if(hError != ERR_OK)
	{
		HxLOG_Warning("xsvc_si_LiveProcessComponentDescFromEit_UkDtt Error (0x%x) \n", hError);
	}

	return hError;
}

#define ___LOCAL_FUNCTIONS_CMD_EVT___

STATIC void xsvc_si_LiveMsgCmdStartLive_UkDtt (svcSi_LiveData_UkDtt_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId(pstMsg->hAction);
	svcSi_FilteringType_b	 bFiltering = 0;
	SvcSi_Arg_StartLive_t	*pstStartLive = &(pstMsg->unArg.stStartLive);

	pstLive->stBase.hAction = pstMsg->hAction;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error("invalid actionHandle(0x%X)\n", pstLive->stBase.hAction);
		return;
	}

	pstLive->bUseRCT = TRUE;

	(void)PAL_SIRAWPOOL_ChangeSiSpecType (ulActionId, eSIxSPEC_UKDTT);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType (ulActionId, eSIxTEXT_ENC_300468_UK);

	bFiltering = (	bSI_FILTERING_PAT |
					bSI_FILTERING_PMT |
					bSI_FILTERING_NIT_ACT |
					bSI_FILTERING_SDT_ACT |
					bSI_FILTERING_SDT_OTH |
					bSI_FILTERING_EIT_ACT_PF |
					bSI_FILTERING_TOT );

	(void)svc_si_LiveSetLiveData (&pstLive->stBase, pstMsg->hAction, pstStartLive->hMasterSvc, pstStartLive->hSuppleSvc, pstStartLive->ulSupType);
	(void)svc_si_LiveStartSection (&pstLive->stBase, bFiltering);
}

STATIC void xsvc_si_LiveMsgCmdStopLive_UkDtt (svcSi_LiveData_UkDtt_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)svc_si_LiveStopSection (&pstLive->stBase);
}

STATIC void xsvc_si_LiveMsgCmdStartRec_UkDtt (svcSi_LiveData_UkDtt_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId(pstMsg->hAction);
	svcSi_FilteringType_b	 bFiltering = 0;
	SvcSi_Arg_StartLive_t	*pstStartLive = &(pstMsg->unArg.stStartLive);

	pstLive->stBase.hAction = pstMsg->hAction;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("invalid actionHandle(0x%X)\n", pstLive->stBase.hAction);
		return;
	}

	(void)PAL_SIRAWPOOL_ChangeSiSpecType (ulActionId, eSIxSPEC_UKDTT);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType (ulActionId, eSIxTEXT_ENC_300468_UK);

	bFiltering = (	bSI_FILTERING_PAT |
					bSI_FILTERING_PMT |
					bSI_FILTERING_NIT_ACT |
					bSI_FILTERING_SDT_ACT |
					bSI_FILTERING_EIT_ACT_PF);

	(void)svc_si_LiveSetLiveData (&pstLive->stBase, pstMsg->hAction, pstStartLive->hMasterSvc, pstStartLive->hSuppleSvc, pstStartLive->ulSupType);
	(void)svc_si_LiveStartSection (&pstLive->stBase, bFiltering);
}

STATIC void xsvc_si_LiveMsgCmdStopRec_UkDtt (svcSi_LiveData_UkDtt_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)svc_si_LiveStopSection (&pstLive->stBase);
}

STATIC void xsvc_si_LiveMsgCmdStartPb_UkDtt (svcSi_LiveData_UkDtt_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId(pstMsg->hAction);
	svcSi_FilteringType_b	 bFiltering = 0;
	SvcSi_PlaybackOption_t *pstPbOption = &(pstMsg->unArg.stStartPb.stOption);

	pstLive->stBase.hAction = pstMsg->hAction;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("invalid actionHandle(0x%X)\n", pstLive->stBase.hAction);
		return;
	}

	(void)PAL_SIRAWPOOL_ChangeSiSpecType (ulActionId, eSIxSPEC_UKDTT);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType (ulActionId, eSIxTEXT_ENC_300468_UK);

	bFiltering = (	bSI_FILTERING_PAT |
					bSI_FILTERING_PMT |
					bSI_FILTERING_NIT_ACT |
					bSI_FILTERING_SDT_ACT |
					bSI_FILTERING_EIT_ACT_PF );

	(void)svc_si_LiveSetPbData (&pstLive->stBase, pstMsg->hAction, pstPbOption->usTsId, pstPbOption->usSvcId, pstPbOption->usPmtPid );
	(void)svc_si_LiveStartSection (&pstLive->stBase, bFiltering);
}

STATIC void xsvc_si_LiveMsgCmdStopPb_UkDtt (svcSi_LiveData_UkDtt_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)svc_si_LiveStopSection (&pstLive->stBase);
}

STATIC void xsvc_si_LiveMsgEvtPatTable_UkDtt (svcSi_LiveData_UkDtt_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibLiveOnPatReceived_Base (&pstLive->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_LiveMsgEvtPmtTable_UkDtt (svcSi_LiveData_UkDtt_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibLiveOnPmtReceived_Base (&pstLive->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LiveProcessPmt_UkDtt(pstLive,&(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_LiveMsgEvtEitTable_UkDtt (svcSi_LiveData_UkDtt_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibLiveOnEitActPfReceived_Base (&pstLive->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));
//	(void)xsvc_si_LiveProcessEit_UkDtt (pstLive,  &(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_LiveMsgEvtNitTable_UkDtt (svcSi_LiveData_UkDtt_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibLiveOnNitActReceived_Base (&pstLive->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LiveProcessNit_UkDtt(pstLive,&(pstMsg->unArg.stSection), ACT_HEAD);
}

STATIC void xsvc_si_LiveMsgEvtSdtTable_UkDtt (svcSi_LiveData_UkDtt_t *pstLive, SvcSi_Msg_t *pstMsg)
{

	(void)xsvc_si_LibLiveOnSdtActReceived_Base (&pstLive->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));
	(void)xsvc_si_LiveProcessSdt_UkDtt(pstLive,&(pstMsg->unArg.stSection), ACT_HEAD);
}

STATIC void xsvc_si_LiveMsgEvtSdtOthTable_UkDtt (svcSi_LiveData_UkDtt_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LiveProcessOthSdt_UkDtt(pstLive,&(pstMsg->unArg.stSection), OTH_HEAD);
}

// Not Used
STATIC void xsvc_si_LiveMsgEvtTdtTable_UkDtt (svcSi_LiveData_UkDtt_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	HxLOG_Warning("Not Used Table (TDT) \n");
	//(void)xsvc_si_LibLiveOnTdtReceived_Base (pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_LiveMsgEvtTotTable_UkDtt (svcSi_LiveData_UkDtt_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibLiveOnTotReceived_Base (&pstLive->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_LiveMsgEvtRctTable_UkDtt (svcSi_LiveData_UkDtt_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	HxLOG_Print("\n\n[%s] TableStatus (%d) \n\n\n", __FUNCTION__, pstMsg->unArg.stSection.ulTableStatus);

	switch (pstMsg->unArg.stSection.ulTableStatus)
	{
	case eSI_TABLE_TIMEOUT:
	case eSI_TABLE_FAIL:
		if ((FILTER_ID_NULL == pstLive->stBase.aulTimeoutFilterId[eSIxTABLETYPE_Rct]) ||
			(pstMsg->unArg.stSection.ulFilterId != pstLive->stBase.aulTimeoutFilterId[eSIxTABLETYPE_Rct]))
		{
			svc_si_PostMsgToAp (eSEVT_SI_RCT_TIMEOUT, pstLive->stBase.hAction, 0, 0, 0);
		}

		svc_si_PostMsgToAp (eSEVT_SI_RCT_TIMEOUT, pstLive->stBase.hAction, 0, 0, 0);

		pstLive->stBase.aulTimeoutFilterId[eSIxTABLETYPE_Rct] = pstMsg->unArg.stSection.ulFilterId;
		break;

	case eSI_TABLE_RECEIVED:
	default:
		{
			HERROR				hError = ERR_FAIL;

			hError = svc_si_LibCheckRctInfo(pstLive->stBase.hAction, pstLive->stBase.hService);

			if(hError == ERR_OK)
				svc_si_PostMsgToAp (eSEVT_SI_RCT, pstLive->stBase.hAction, 0, 0, 0);
			else
				svc_si_PostMsgToAp (eSEVT_SI_RCT_NONE, pstLive->stBase.hAction, 0, 0, 0);

		}
#if defined(CONFIG_DEBUG)
		//xsvc_si_LivePrintRct(&pstLive->stBase, &(pstMsg->unArg.stSection));
#endif
		break;
	}

	return;
}

STATIC void xsvc_si_LiveMsgCmdCheckNcdInfo_UkDtt(svcSi_LiveData_UkDtt_t * pstLive,SvcSi_Msg_t * pstMsg)
{

	HERROR hErr = ERR_FAIL;
	HUINT32	ulASONotifyFlag = 0;


	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_ASO_NOTIFY_FLAG, (HUINT32 *)&ulASONotifyFlag,  0);
	if(ulASONotifyFlag)
	{
		svc_si_PostMsgToAp (eSEVT_SI_NETWORK_CHANGED_NOTIFY, (HUINT32)pstLive->stBase.hAction, 0, 0, TRUE /* Scan Now */);
	}

}

#define _____SI_MSG_PROCESS_FUNCTION_____
STATIC void xsvc_si_LiveProcessMsg_UkDtt (SvcSi_Msg_t *pstMsg, void *pvData)
{
	svcSi_LiveData_UkDtt_t	*pstLive = (svcSi_LiveData_UkDtt_t *)pvData;

	if (NULL == pstLive)				{ return; }

	switch (pstMsg->eMsgClass)
	{
	case eSIMSG_CMD_START_LIVE:
		HxLOG_Debug("eMsgClass : eSIMSG_CMD_START_LIVE (hAction : 0x%08x)\n", pstMsg->hAction);
		xsvc_si_LiveMsgCmdStartLive_UkDtt (pstLive, pstMsg);
		xsvc_si_LiveMsgCmdCheckNcdInfo_UkDtt (pstLive, pstMsg); //부팅 후 제일 처음... 확인... check
		break;

	case eSIMSG_CMD_STOP_LIVE:
		HxLOG_Debug("eMsgClass : eSIMSG_CMD_STOP_LIVE (hAction : 0x%08x)\n", pstMsg->hAction);
		xsvc_si_LiveMsgCmdStopLive_UkDtt (pstLive, pstMsg);
		break;

	case eSIMSG_CMD_START_REC:
		HxLOG_Debug("eMsgClass : eSIMSG_CMD_START_REC (hAction : 0x%08x)\n", pstMsg->hAction);
		xsvc_si_LiveMsgCmdStartRec_UkDtt (pstLive, pstMsg);
		break;

	case eSIMSG_CMD_STOP_REC:
		HxLOG_Debug("eMsgClass : eSIMSG_CMD_STOP_REC (hAction : 0x%08x)\n", pstMsg->hAction);
		xsvc_si_LiveMsgCmdStopRec_UkDtt (pstLive, pstMsg);
		break;

	case eSIMSG_CMD_START_PB:
		HxLOG_Debug("eMsgClass : eSIMSG_CMD_START_PB (hAction : 0x%08x)\n", pstMsg->hAction);
		xsvc_si_LiveMsgCmdStartPb_UkDtt (pstLive, pstMsg);
		break;

	case eSIMSG_CMD_STOP_PB:
		HxLOG_Debug("eMsgClass : eSIMSG_CMD_STOP_PB (hAction : 0x%08x)\n", pstMsg->hAction);
		xsvc_si_LiveMsgCmdStopPb_UkDtt (pstLive, pstMsg);
		break;

	case eSIMSG_EVT_PAT_TABLE:
		HxLOG_Debug("eMsgClass : eSIMSG_EVT_PAT_TABLE (hAction : 0x%08x)\n", pstMsg->hAction);
		xsvc_si_LiveMsgEvtPatTable_UkDtt (pstLive, pstMsg);
		break;

	case eSIMSG_EVT_PMT_TABLE:
		HxLOG_Debug("eMsgClass : eSIMSG_EVT_PMT_TABLE (hAction : 0x%08x)\n", pstMsg->hAction);
		xsvc_si_LiveMsgEvtPmtTable_UkDtt (pstLive, pstMsg);
		break;

	case eSIMSG_EVT_EIT_TABLE:
		HxLOG_Debug("eMsgClass : eSIMSG_EVT_EIT_TABLE (hAction : 0x%08x)\n", pstMsg->hAction);
		xsvc_si_LiveMsgEvtEitTable_UkDtt (pstLive, pstMsg);
		break;

	case eSIMSG_EVT_NIT_TABLE:
		HxLOG_Debug("eMsgClass : eSIMSG_EVT_NIT_TABLE (hAction : 0x%08x)\n", pstMsg->hAction);
		xsvc_si_LiveMsgEvtNitTable_UkDtt (pstLive, pstMsg);
		break;

	case eSIMSG_EVT_SDT_TABLE:
		HxLOG_Debug("eMsgClass : eSIMSG_EVT_SDT_TABLE (hAction : 0x%08x)\n", pstMsg->hAction);
		xsvc_si_LiveMsgEvtSdtTable_UkDtt (pstLive, pstMsg);
		break;

	case eSIMSG_EVT_SDT_OTH_TABLE:
		HxLOG_Debug("eMsgClass : eSIMSG_EVT_SDT_OTH_TABLE (hAction : 0x%08x)\n", pstMsg->hAction);
		xsvc_si_LiveMsgEvtSdtOthTable_UkDtt (pstLive, pstMsg);
		break;

#if 0
	case eSIMSG_EVT_TDT_TABLE:
		HxLOG_Debug("eMsgClass : eSIMSG_EVT_TDT_TABLE \n");
		//xsvc_si_LiveMsgEvtTdtTable_UkDtt (pstLive, pstMsg);
		break;
#endif

	case eSIMSG_EVT_TOT_TABLE:
		HxLOG_Debug("eMsgClass : eSIMSG_EVT_TOT_TABLE (hAction : 0x%08x)\n", pstMsg->hAction);
		xsvc_si_LiveMsgEvtTotTable_UkDtt(pstLive, pstMsg);
		break;

	case eSIMSG_EVT_RCT_TABLE:
		HxLOG_Debug("eMsgClass : eSIMSG_EVT_RCT_TABLE (hAction : 0x%08x)\n", pstMsg->hAction);
		xsvc_si_LiveMsgEvtRctTable_UkDtt (pstLive, pstMsg);
		break;

	default:
		HxLOG_Debug ("Non-processing MSG (0x%08x) -> skipped:\n", pstMsg->eMsgClass);
		break;
	}
}

#define ___MEMBER_FUNCTIONS___

#define ___API_FUNCTIONS___
SvcSi_ActFunc xsvc_si_LiveGetLiveActionFunc_UkDtt (void)
{
	return xsvc_si_LiveProcessMsg_UkDtt;
}

void *xsvc_si_LiveAllocLiveActionData_UkDtt (void)
{
	svcSi_LiveData_UkDtt_t	*pstLive;

	pstLive = (svcSi_LiveData_UkDtt_t *)OxSI_Calloc (sizeof(svcSi_LiveData_UkDtt_t));
	if (NULL != pstLive)
	{
		pstLive->stBase.eState = eSI_LIVESTATE_Init;
		pstLive->stBase.eTextEncType	= eSITEXT_ENCODED_300468_UK;
		pstLive->bUseRCT	= FALSE;
	}

	return pstLive;
}

void xsvc_si_LiveFreeLiveActionData_UkDtt (void *pvData)
{
	svcSi_LiveData_UkDtt_t	*pstLive = (svcSi_LiveData_UkDtt_t *)pvData;

	if (NULL != pstLive)
	{
		OxSI_Free (pstLive);
	}

	return;
}

/* end of file */

