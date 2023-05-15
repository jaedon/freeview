/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_swup_ertelecom.c
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
#if defined(CONFIG_OP_ERTELECOM)
STATIC HUINT8 s_ucSwupDownType = SvcSi_SWUP_TYPE_USER_SELECT;
#endif

/*******************************************************************/
/********************      Static Functions     ********************/
/*******************************************************************/

HERROR xsvc_si_SwupCheckGetSwupDownType_Ertelecom(HUINT8 *pucDownType)
{
	*pucDownType = s_ucSwupDownType;	//SvcSi_SWUP_TYPE_USER_SELECT;
	return ERR_OK;
}

HERROR xsvc_si_SwupCheckSetSwupDownType_Ertelecom(HUINT8 ucDownType)
{
	//s_ucSwupDownType = ucDownType;
	return ERR_OK;
}


HERROR xsvc_si_SwupCheckGetFilteringType_Ertelecom(xsvcSi_SwupData_t *pstSwupCheck)
{
	HERROR	hErr = ERR_FAIL;
	
	if(pstSwupCheck == NULL)
	{
		return ERR_FAIL;
	}
		
	if(pstSwupCheck->stBase.bLiveSwupCheck == TRUE)
	{
		pstSwupCheck->stBase.eFiltering = bSI_FILTERING_NIT_ACT | bSI_FILTERING_PAT;
		hErr = ERR_OK;
	}

	HxLOG_Print("[%s:%d] eFiltering (0x%08x) hErr(0x%x) \n", __FUNCTION__, __LINE__, pstSwupCheck->stBase.eFiltering, hErr);

	return hErr;
	
}

#define _____SWUP_PARSING_____

STATIC HERROR xsvc_si_SwupCheckParseSelectorBytes_Ertelecom (HUINT8 *pucRaw, SvcSi_SwupSignal_t *pstSelector)
{
	if (pucRaw == NULL)					{ return ERR_FAIL; }
	if (pstSelector == NULL)			{ return ERR_FAIL; }

	pstSelector->ulStartSystemId =HxMACRO_Get32Bit (pucRaw);			pucRaw += 4;		// 0~3 : Start Product ID
	pstSelector->usHdfVer =HxMACRO_Get16Bit (pucRaw);					pucRaw += 2;		// 4~5 : HDF Version
	pstSelector->usDataType =HxMACRO_Get16Bit (pucRaw);					pucRaw += 2;		// 6~7 : Data Type
	svc_si_SwupCheckConvertSwupSiDataTypeToVersionType((SvcSi_SwupImageType_e)pstSelector->usDataType, &(pstSelector->eDataType));
	pstSelector->ulDataVersion = HxMACRO_Get32Bit (pucRaw);				pucRaw += 4;		// 8~11 : Data Version
	pstSelector->ulEndSystemId = HxMACRO_Get32Bit (pucRaw);				pucRaw += 4;		// 12~15 : End Product ID
	
	if (*pucRaw > 0)
	{
		if(pucRaw[1] ==1)
		{
			pstSelector->ucSwupType = SvcSi_SWUP_TYPE_USER_SELECT;
		}
		else if(pucRaw[1] ==2)
		{
			pstSelector->ucSwupType = SvcSi_SWUP_TYPE_FORCED;
		}
		
	}
	else
	{
		pstSelector->ucSwupType = SvcSi_SWUP_TYPE_USER_SELECT;
	}

	return ERR_OK;
}


#define _____SWUP_INFO_TO_API_____

HERROR 	xsvc_si_SwupCheckFindProperOuiDataFromLinkDes_Ertelecom(SIxLinkDes_t *pstLinkDes)
{
	HUINT32 					 ulSelectorIdx, ulSelectorNum;
	HUINT32 					 ulSystemId, ulSystemVer;
	HERROR						 hErr;
	HUINT8						*pucRaw = NULL;
	HBOOL						 bFound = FALSE;
	svcSi_SwupCheck_SsuInfo_t	*pstOuiSelectors;
	SvcSi_SwupSignal_t			 stSwupInfo;
	HUINT32 					 ulSelectorIdxMulti;

	hErr = svc_si_SwupCheckGetSsuInfoFromSsuLinkDes (pstLinkDes, &ulSelectorNum, &pstOuiSelectors);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("svc_si_SwupCheckGetSsuInfoFromSsuLinkDes Error \n");
		return ERR_FAIL;
	}

	// DVBSSU simple 사양을 지원하기 위해 추가된 부분임
	for (ulSelectorIdx = 0; ulSelectorIdx < ulSelectorNum; ulSelectorIdx++)
	{
		if (pstOuiSelectors[ulSelectorIdx].ulOui == eSWUP_OUI_HUMAX
			|| pstOuiSelectors[ulSelectorIdx].ulOui == eSWUP_OUI_DVB
			)

		{
			break;
		}
	}

	//ER Telecom 사양을 지원하기 위해 추가된 부분임
	for (ulSelectorIdxMulti = 0; ulSelectorIdxMulti < ulSelectorNum; ulSelectorIdxMulti++)
	{
		if (pstOuiSelectors[ulSelectorIdxMulti].ulOui == eSWUP_OUI_HUMAX || pstOuiSelectors[ulSelectorIdxMulti].ulOui == eSWUP_OUI_DVB)
		{
			if (pstOuiSelectors[ulSelectorIdxMulti].ulSelectorLen < 18)
			{
				continue;
				//s_ucSwupDownType = SvcSi_SWUP_TYPE_USER_SELECT;
				//return svc_si_SwupCheckFindProperOuiDataFromLinkDes(pstLinkDes);
			}

			pucRaw = pstOuiSelectors[ulSelectorIdxMulti].pucSelectSelectorPnt;
			if (pucRaw == NULL)
			{
				continue;
			}

			HxSTD_memset(&stSwupInfo, 0, sizeof(SvcSi_SwupSignal_t));

			hErr = xsvc_si_SwupCheckParseSelectorBytes_Ertelecom(pucRaw, &stSwupInfo);
			if(hErr == ERR_OK)
			{
				HxLOG_Print("\n==[DVBSSU_SWUP] NIT : SelectorByte Info==\n");
				HxLOG_Print("\n==System ID : 0x%x ~ 0x%x", stSwupInfo.ulStartSystemId, stSwupInfo.ulEndSystemId);
				HxLOG_Print("\n==SW Version : %4d", stSwupInfo.ulDataVersion);
				HxLOG_Print("\n==Data Type : %d", stSwupInfo.usDataType);
				HxLOG_Print("\n==SWUP Type : %d", stSwupInfo.ucSwupType);

				if (PalParam_Activated())
				{
					hErr = PAL_PARAM_GetField(ePAL_PARAM_SYSTEM_ID, (void *)&ulSystemId, sizeof(HUINT32));
					hErr |= PAL_PARAM_GetField(ePAL_PARAM_APP_VERSION, (void *)&ulSystemVer, sizeof(HUINT32));
				}
				else
				{
					hErr = PAL_SYS_GetSystemId (&ulSystemId);
					hErr |= PAL_SYS_GetSystemVersion (stSwupInfo.eDataType, &ulSystemVer);
				}
				if(hErr == ERR_OK)
				{
					if ((stSwupInfo.ulStartSystemId <= ulSystemId) &&
					(stSwupInfo.ulEndSystemId >= ulSystemId) &&
					(stSwupInfo.ulDataVersion > ulSystemVer))
					{
						bFound = TRUE;
						s_ucSwupDownType = stSwupInfo.ucSwupType;
						break;
					}
				}
			}
		}
	}

	svc_si_SwupCheckFreeSsuInfo (pstOuiSelectors);

	return (ulSelectorIdx < ulSelectorNum) ? ERR_OK : ERR_FAIL;
}



STATIC void xsvc_si_SwupCheckFreeDes(void *pvData)
{
	PAL_SIRAWPOOL_FreeDes((SIxDescriptor_t *)pvData);
}

STATIC HERROR xsvc_si_SwupCheckFindLinkage_Ertelecom(svcSi_SwupCheck_Base_t *pstSwupCheckBase, SIxTable_Type_e eSiType, HUINT8 ucLinkType, HUINT32 *pulNumLink, DbSvc_TripleId_t **ppstTripleIdArray)
{
	HUINT32 				 ulActionId;
	HUINT32 				 ulNumLinkage, ulCntLinkage, i, j;
	HERROR					 hErr;
	SIxTable_PsiTable_t 	*pstPsiTable = NULL, *pstCurrPsi = NULL;
	HxList_t				*pst2ndLoop = NULL, *pstDesLoop = NULL;
	SIxLinkDes_t			*pstLinkDes = NULL;
	HxList_t				*pstLinkDesList = NULL, *pstList = NULL;
	DbSvc_TripleId_t		*pstTripleIdArray = NULL;

	ulActionId = PAL_PIPE_GetActionId (pstSwupCheckBase->hAction);
	pstPsiTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSiType, SILIB_TABLE_EXT_ID_ALL);
	if (NULL == pstPsiTable)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable ulActionId=%d, eSiType(%x) \n", ulActionId, eSiType);
		return ERR_FAIL;
	}

	for (pstCurrPsi = pstPsiTable; pstCurrPsi != NULL; pstCurrPsi = pstCurrPsi->pstNextTable)
	{
		// parse 1st des loop
		for (pstDesLoop = pstCurrPsi->pstDesLoop; pstDesLoop != NULL; pstDesLoop = pstDesLoop->next)
		{
			HUINT8		*pucRawDes = HLIB_LIST_Data (pstDesLoop);

			//HxLOG_Print("start 1st loop desc parsing \n");

			if (pucRawDes == NULL)
			{
				HxLOG_Print("pucRawDes == NULL, continue \n");
				continue;
			}

			if (*pucRawDes == eSIxDESCTAG_LINKAGE)
			{
				pstLinkDes = (SIxLinkDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucRawDes);
				if (pstLinkDes != NULL)
				{
					if (ucLinkType == pstLinkDes->ucLinkType)
					{
						HxLOG_Print("[%s:%d] 1. eSiType(%d) : ucLinkType(0x%08x) Found (%04x:%04x:%04x)\n", __FUNCTION__, __LINE__, eSiType, ucLinkType, pstLinkDes->usOrgNetId, pstLinkDes->usTsId, pstLinkDes->usSvcId);
						pstLinkDesList = HLIB_LIST_Append (pstLinkDesList, (void *)pstLinkDes);
					}
					else if(ucLinkType == eLINKAGE_ALL)
					{
						hErr = xsvc_si_SwupCheckFindProperOuiDataFromLinkDes_Ertelecom(pstLinkDes);
						if (hErr == ERR_OK)
						{
							HxLOG_Print("[%s:%d] 1. eSiType(%d) : eLINKAGE_ALL Found (%04x:%04x:%04x)\n", __FUNCTION__, __LINE__, eSiType, pstLinkDes->usOrgNetId, pstLinkDes->usTsId, pstLinkDes->usSvcId);
							pstLinkDesList = HLIB_LIST_Append (pstLinkDesList, (void *)pstLinkDes);
						}
						else
						{
							HxLOG_Print("[%s:%d] 1. eSiType(%d) : eLINKAGE_ALL Not Found (%04x:%04x:%04x)\n", __FUNCTION__, __LINE__, eSiType, pstLinkDes->usOrgNetId, pstLinkDes->usTsId, pstLinkDes->usSvcId);
							PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstLinkDes);
						}
					}
					else
					{
						HxLOG_Print("[%s:%d] 1. eSiType(%d) : Not Support ucLinkType (0x%x) \n", __FUNCTION__, __LINE__, eSiType, ucLinkType, pstLinkDes->usOrgNetId, pstLinkDes->usTsId, pstLinkDes->usSvcId);
						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstLinkDes);
					}
				}
			}
		}

		// parse 2nd des loop
		for (pst2ndLoop = pstCurrPsi->pst2ndLoop; pst2ndLoop != NULL; pst2ndLoop = pst2ndLoop->next)
		{
			SIxTable_2ndLoop_t	*pst2ndItem = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pst2ndLoop);
			HxList_t				*pst2ndDesLoop = NULL;

			//HxLOG_Print("start 2nd loop desc parsing \n");

			if (pst2ndItem == NULL)
			{
				HxLOG_Print("pst2ndItem == NULL, continue \n");
				continue;
			}

			for (pst2ndDesLoop = pst2ndItem->pstDesLoop; pst2ndDesLoop != NULL; pst2ndDesLoop = pst2ndDesLoop->next)
			{
				HUINT8				*pucRawDes = (HUINT8 *)HLIB_LIST_Data (pst2ndDesLoop);

				if (pucRawDes == NULL)
				{
					HxLOG_Print("pucRawDes == NULL, continue \n");
					continue;
				}

				if (*pucRawDes == eSIxDESCTAG_LINKAGE)
				{
					pstLinkDes = (SIxLinkDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucRawDes);
					if (pstLinkDes != NULL)
					{
						if (ucLinkType == pstLinkDes->ucLinkType)
						{
							HxLOG_Print("[%s:%d] 2. eSiType(%d) : ucLinkType(0x%08x) Found (%04x:%04x:%04x)\n", __FUNCTION__, __LINE__, eSiType, ucLinkType, pstLinkDes->usOrgNetId, pstLinkDes->usTsId, pstLinkDes->usSvcId);
							pstLinkDesList = HLIB_LIST_Append (pstLinkDesList, (void *)pstLinkDes);
						}
						else if(ucLinkType ==eLINKAGE_ALL)
						{
							hErr = xsvc_si_SwupCheckFindProperOuiDataFromLinkDes_Ertelecom(pstLinkDes);
							if (hErr == ERR_OK)
							{
								HxLOG_Print("[%s:%d] 2. eSiType(%d) : eLINKAGE_ALL Found (%04x:%04x:%04x)\n", __FUNCTION__, __LINE__, eSiType, pstLinkDes->usOrgNetId, pstLinkDes->usTsId, pstLinkDes->usSvcId);
								pstLinkDesList = HLIB_LIST_Append (pstLinkDesList, (void *)pstLinkDes);
							}
							else
							{
								HxLOG_Print("[%s:%d] 2. eSiType(%d) : eLINKAGE_ALL Not Found (%04x:%04x:%04x)\n", __FUNCTION__, __LINE__, eSiType, pstLinkDes->usOrgNetId, pstLinkDes->usTsId, pstLinkDes->usSvcId);
								PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstLinkDes);
							}
						}
						else
						{
							HxLOG_Print("[%s:%d] 2. eSiType(%d) : Not Support ucLinkType (0x%x) \n", __FUNCTION__, __LINE__, eSiType, ucLinkType, pstLinkDes->usOrgNetId, pstLinkDes->usTsId, pstLinkDes->usSvcId);
							PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstLinkDes);
						}
					}
				}
			}
		}
	}

	// list에 저장된 정보가 하나도 없다면 error~~
	ulNumLinkage = HLIB_LIST_Length(pstLinkDesList);
	if (ulNumLinkage == 0)
	{
		hErr = ERR_FAIL;
		goto Error;
	}

	HxLOG_Print("[%s, %d] ulNumLinkage: %d \n", __FUNCTION__, __LINE__, ulNumLinkage);

	if (pulNumLink == NULL && ppstTripleIdArray == NULL)
	{
		hErr = ERR_OK;
		goto Error;
	}

	pstTripleIdArray = (DbSvc_TripleId_t *)OxSI_Malloc (sizeof(DbSvc_TripleId_t) * ulNumLinkage);
	if (pstTripleIdArray == NULL)
	{
		hErr = ERR_FAIL;
		goto Error;
	}

	ulCntLinkage = 0;

	for (i = 0; i < ulNumLinkage; i++)
	{
		pstList = HLIB_LIST_GetListItem(pstLinkDesList, i);
		if (pstList == NULL)
		{
			HxLOG_Critical ("HLIB_LIST_GetListItem() failed.. len(%d) curPos(%d)\n", ulNumLinkage, i);
			continue;
		}

		pstLinkDes = (SIxLinkDes_t *)HLIB_LIST_Data(pstList);
		if (pstLinkDes == NULL)
		{
			HxLOG_Critical ("HLIB_LIST_Data() failed.. curPos(%d)\n", i);
			continue;
		}

		for (j = 0; j < ulCntLinkage; j++)
		{
			if (pstTripleIdArray[j].usTsId == pstLinkDes->usTsId &&
				pstTripleIdArray[j].usOnId == pstLinkDes->usOrgNetId)
			{
				break;
			}
		}

		// pstTripleIdArray 안에 미리 들어 있는 동일한 Triple ID가 없다 : New One
		if (j == ulCntLinkage)
		{
			pstTripleIdArray[ulCntLinkage].usOnId  = pstLinkDes->usOrgNetId;
			pstTripleIdArray[ulCntLinkage].usTsId  = pstLinkDes->usTsId;
			pstTripleIdArray[ulCntLinkage].usSvcId = pstLinkDes->usSvcId;

			ulCntLinkage++;
		}
	}

	if (ulCntLinkage == 0)
	{
		hErr = ERR_FAIL;
		goto Error;
	}

	if (pulNumLink != NULL)
	{
		*pulNumLink = ulCntLinkage;
	}

	if (ppstTripleIdArray != NULL)
	{
		*ppstTripleIdArray = pstTripleIdArray;
		pstTripleIdArray = NULL;
	}

	hErr = ERR_OK;

Error:
	if (pstTripleIdArray != NULL)		{ OxSI_Free (pstTripleIdArray); }
	if (pstPsiTable != NULL)			{ PAL_SIRAWPOOL_FreeTable(pstPsiTable); }
	if (pstLinkDesList != NULL)			{ HLIB_LIST_RemoveAllFunc (pstLinkDesList, xsvc_si_SwupCheckFreeDes);}

	return hErr;

}

void xsvc_si_SwupCheckMsgEvtNitTable_Ertelecom (xsvcSi_SwupData_t *pstLiveSwupCheck, SvcSi_Msg_t *pstMsg)
{
	HERROR				 hErr;
	SIxTable_Type_e		 eSTableType;
	DbSvc_TripleId_t	*pstTripleIdArray = NULL;
	DbSvc_TsInfo_t		 stTsInfo;
	HBOOL				 bCurTP = FALSE;
	HUINT32				 ulNumSsu, ulIndex;

	HxLOG_Debug("+++ \n");

	if (pstLiveSwupCheck == NULL)
	{
		HxLOG_Error ("pstLiveSwupCheck is NULL\n");
		return ;
	}

	HxLOG_Print("[%s:%d] ulTableStatus (%d)  \n", __FUNCTION__, __LINE__, pstMsg->unArg.stSection.ulTableStatus);
			
	(void)xsvc_si_LibSwupOnSiTableReceived_Base(&pstLiveSwupCheck->stBase, pstMsg->hAction, &(pstMsg->unArg.stSection));
	
	if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_RECEIVED)
	{
		if(pstMsg->eMsgClass == eSIMSG_EVT_NIT_TABLE)
			eSTableType = eSIxTABLETYPE_NitAct;
		else
			eSTableType = eSIxTABLETYPE_NitOth;

		// SSU-SCAN Linkage 정보는 없지만 SSU-SVC 정보가 직접 있는가?
		hErr = xsvc_si_SwupCheckFindLinkage_Ertelecom(&pstLiveSwupCheck->stBase, eSTableType, eLINKAGE_ALL, &ulNumSsu, (DbSvc_TripleId_t**)&pstTripleIdArray);
		if(hErr == ERR_OK)
		{
			hErr = DB_SVC_GetTsInfoByServiceHandle(pstLiveSwupCheck->stBase.stLiveSwupOption.hLiveSvcHandle, &stTsInfo);
			if(hErr != ERR_OK)
			{
				ulNumSsu = 0;
				if (pstTripleIdArray != NULL)
				{
					OxSI_Free(pstTripleIdArray);
				}

				HxLOG_Error(" ============================ (-) Error get Ts info about Current Service \n");

				bCurTP = FALSE;
			}

			if ((pstTripleIdArray !=NULL) && (ulNumSsu > 0))
			{
				for(ulIndex=0 ; ulIndex<ulNumSsu; ulIndex++)
				{
					HxLOG_Print("[%s, %d] SWUP Triple : OnId(0x%x) TsId(0x%x) svcId(0x%x) , SvcTs : OnId(0x%x) TsId(0x%x) \n",
						__FUNCTION__, __LINE__, pstTripleIdArray[ulIndex].usOnId, pstTripleIdArray[ulIndex].usTsId, pstTripleIdArray[ulIndex].usSvcId,
						stTsInfo.usOnId, stTsInfo.usTsId);

					if(stTsInfo.usOnId == pstTripleIdArray[ulIndex].usOnId &&
						stTsInfo.usTsId == pstTripleIdArray[ulIndex].usTsId )
					{
						bCurTP = TRUE;
					}
				}
			}

			if (bCurTP != TRUE)
			{
				HxSTD_memset(&pstLiveSwupCheck->stSwupInfo, 0x00, sizeof(SvcSi_SwupSignal_t));

				pstLiveSwupCheck->stSwupInfo.ucSwupType = s_ucSwupDownType;

				svc_si_PostMsgToAp (eSEVT_SWUP_DETECT_SUCCESS, pstLiveSwupCheck->stBase.hAction, eDxSWUPDATE_SIGNAL_MULTI, (HINT32)&pstLiveSwupCheck->stSwupInfo, 0);
			}
		}
		else
		{
			// NIT에서 ERTELECOM Spec을 만족하지 않으면 user_select 로 설정한다..
			s_ucSwupDownType = SvcSi_SWUP_TYPE_USER_SELECT;
		}
	}

	return ;
}

/* end of file */

