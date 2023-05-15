/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  om_media_play_live.c
	@brief

	Description:  									\n
	Module: TVMANAGER / OAPI Manager				\n

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
#include <obama_rpcprotocol.h>
#include <oapi.h>

#include <octo_common.h>

#include <db_param.h>
#include <svc_av.h>
#include <svc_output.h>
#include <svc_pipe.h>
#include <svc_si.h>
#include <svc_cas.h>

#include <bus.h>

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_live.h>
#include <mgr_svclist.h>
#include <mgr_copyrightcontrol.h>
#include <mgr_hbbtv.h>
#include <mgr_output.h>

#include "_om_media_play.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

// main action id로 오는 msg만 oapi로 알리자
#if defined(CONFIG_SUPPORT_FCC)
#define LIVE_CHECK_FCC_VIEWID(ulViewId)							\
		if (ulViewId != 0)										\
		{														\
			return;												\
		}

#define LIVE_CHECK_FCC_VIEWID_WITH_RET(ulViewId, returnValue)	\
		if (ulViewId != 0)										\
		{														\
			return returnValue;									\
		}
#else
#define LIVE_CHECK_FCC_VIEWID(ulViewId)
#define LIVE_CHECK_FCC_VIEWID_WITH_RET(ulViewId, returnValue)
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HBOOL			 bAllocated;
	DxTuneParam_t	 stTuneInfo;
} omMediaPlayLiveRfInputStatus_t;

typedef struct
{
	HBOOL			 bAllocated;
	DxTuneParam_t	 stTuneInfo;
} omMediaPlayLiveTunerStatus_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HINT32				 	s_nRpcHandle = 0;
#if defined(CONFIG_3RD_MHEG)
STATIC HBOOL					s_bMhegInterruptibleFlag_InOm = FALSE;
#endif

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___

STATIC HERROR om_media_play_live_getCurrentResourceStatus (HUINT32 ulLiveActId, MgrAction_Priority_e eActPriority, OxMediaPlay_ConflictInfo_t *pstLiveConfInfo)
{
	STATIC HBOOL			 s_bInited = FALSE;
	STATIC HUINT32			 s_ulNumRfInput = 0, s_ulNumTunerGroup = 0;
	STATIC omMediaPlayLiveRfInputStatus_t	*s_pstRfInputArray = NULL;
	STATIC omMediaPlayLiveTunerStatus_t		*s_pstTunerArray = NULL;

	HUINT32					 ulActionId;
	HUINT32					 ulRfInputId, ulTunerGroupId;
//	DxTuneParam_t			 stChTuneInfo;
//	DxTuneParam_t			 stDxTuneInfo;
	RxRscObject_t			 stRscObj;
	HERROR					 hErr;

	if (TRUE != s_bInited)
	{
		s_ulNumRfInput = SVC_CH_CountSatelliteRfInput();
		SVC_CH_GetNumberOfTunerGroup (&s_ulNumTunerGroup);

		s_pstRfInputArray = (omMediaPlayLiveRfInputStatus_t *)OxAP_Malloc(sizeof(omMediaPlayLiveRfInputStatus_t) * (s_ulNumRfInput + 2));
		s_pstTunerArray = (omMediaPlayLiveTunerStatus_t *)OxAP_Malloc(sizeof(omMediaPlayLiveTunerStatus_t) * (s_ulNumTunerGroup + 2));
		if ((NULL == s_pstRfInputArray) || (NULL == s_pstTunerArray))
		{
			if (NULL != s_pstRfInputArray)					{ OxAP_Free (s_pstRfInputArray); }
			if (NULL != s_pstTunerArray)					{ OxAP_Free (s_pstTunerArray); }

			return ERR_FAIL;
		}

		s_bInited = TRUE;
	}

	HxSTD_MemSet (pstLiveConfInfo, 0, sizeof(OxMediaPlay_ConflictInfo_t));
	HxSTD_MemSet (s_pstRfInputArray, 0, sizeof(omMediaPlayLiveRfInputStatus_t) * s_ulNumRfInput);
	HxSTD_MemSet (s_pstTunerArray, 0, sizeof(omMediaPlayLiveTunerStatus_t) * s_ulNumTunerGroup);

	pstLiveConfInfo->ulRfInfoNum		= s_ulNumRfInput;
	pstLiveConfInfo->ulTunerInfoNum		= s_ulNumTunerGroup;

	for (ulActionId = 0; ulActionId < eActionId_MAX; ulActionId++)
	{

		if ((ulActionId == ulLiveActId) ||
			(MGR_ACTION_GetPrePriority(ulActionId) < eActPriority))
		{
			continue;
		}

		// RF-Input:
		if (s_ulNumRfInput > 0)
		{
			hErr = SVC_PIPE_GetResourceId(ulActionId, eRxRSCTYPE_RFINPUT, &ulRfInputId);
			if (ERR_OK == hErr)
			{
				hErr = SVC_PIPE_GetResourceObject(ulActionId, eRxRSCTYPE_RFINPUT, &stRscObj);
				if (ERR_OK == hErr)
				{
					s_pstRfInputArray[ulRfInputId].bAllocated = TRUE;
					HxSTD_MemCopy (&(s_pstRfInputArray[ulRfInputId].stTuneInfo), &(stRscObj.utObj.stRfInput.utObj.stSatRf.stMasterTuneParam), sizeof(DxTuneParam_t));
				}
			}
		}

		// Tuner
		hErr = SVC_PIPE_GetResourceId(ulActionId, eRxRSCTYPE_TUNER_GROUP, &ulTunerGroupId);
		if (ERR_OK == hErr)
		{
			hErr = SVC_PIPE_GetResourceObject(ulActionId, eRxRSCTYPE_TUNER_GROUP, &stRscObj);
			if (ERR_OK == hErr)
			{
				s_pstTunerArray[ulTunerGroupId].bAllocated = TRUE;
				HxSTD_MemCopy (&(s_pstTunerArray[ulTunerGroupId].stTuneInfo), &(stRscObj.utObj.stTuner.stTuneParam), sizeof(DxTuneParam_t));
			}
		}
	}

	for (ulRfInputId = 0; ulRfInputId < s_ulNumRfInput; ulRfInputId++)
	{
		HBOOL						 bRfAvail = FALSE;
		OxMediaPlay_ConfRfInfo_t		*pstConfRfInfo = &(pstLiveConfInfo->astRfInfo[ulRfInputId]);

		hErr = SVC_PIPE_GetRfInputAbility (ulRfInputId, &bRfAvail);
		if ((ERR_OK == hErr) && (TRUE == bRfAvail))
		{
			pstConfRfInfo->bAvailable = TRUE;
			pstConfRfInfo->eRfDeliType = eDxDELIVERY_TYPE_SAT;

			if (TRUE == s_pstRfInputArray[ulRfInputId].bAllocated)
			{
				pstConfRfInfo->bAllocated = TRUE;
				HxSTD_MemCopy (&(pstConfRfInfo->unTuneInfo), &(s_pstRfInputArray[ulRfInputId].stTuneInfo), sizeof(DxTuneParam_t) );
			}
		}
	}

	for (ulTunerGroupId = 0; ulTunerGroupId < s_ulNumTunerGroup; ulTunerGroupId++)
	{
		OxMediaplay_ConfTunerInfo_t	*pstConfTunerInfo = &(pstLiveConfInfo->astTunerInfo[ulTunerGroupId]);

		pstConfTunerInfo->bAvailable = TRUE;
		(void)SVC_CH_GetTunerType (ulTunerGroupId, &(pstConfTunerInfo->eTunerDeliType));

		if (TRUE == s_pstTunerArray[ulTunerGroupId].bAllocated)
		{
			pstConfTunerInfo->bAllocated = TRUE;
			HxSTD_MemCopy (&(pstConfTunerInfo->unTuneInfo), &(s_pstTunerArray[ulTunerGroupId].stTuneInfo), sizeof(DxTuneParam_t));
		}
	}

	return ERR_OK;
}

#if defined(CONFIG_DEBUG)
STATIC HERROR om_media_play_live_printResourceStatus (OxMediaPlay_ConflictInfo_t *pstLiveConfInfo)
{
	HUINT32			 ulIndex;

	HxLOG_Debug ("\n\n\n\n");
	HxLOG_Debug ("[RSCSTATUS] RF-Input Number = %d\n", pstLiveConfInfo->ulRfInfoNum);
	for (ulIndex = 0; ulIndex < pstLiveConfInfo->ulRfInfoNum; ulIndex++)
	{
#if defined(CONFIG_MW_CH_SATELLITE)
		HxLOG_Debug ("[RSCSTATUS] RF-Input %d : Available(%d), Allocated(%d), Freq(%d)\n", ulIndex,
						pstLiveConfInfo->astRfInfo[ulIndex].bAvailable,
						pstLiveConfInfo->astRfInfo[ulIndex].bAllocated,
						pstLiveConfInfo->astRfInfo[ulIndex].unTuneInfo.sat.tuningInfo.ulFrequency);
#elif defined(CONFIG_MW_CH_CABLE)
		HxLOG_Debug ("[RSCSTATUS] RF-Input %d : Available(%d), Allocated(%d), Freq(%d)\n", ulIndex,
						pstLiveConfInfo->astRfInfo[ulIndex].bAvailable,
						pstLiveConfInfo->astRfInfo[ulIndex].bAllocated,
						pstLiveConfInfo->astRfInfo[ulIndex].unTuneInfo.cab.ulFrequency);
#elif defined(CONFIG_MW_CH_TERRESTRIAL)
		HxLOG_Debug ("[RSCSTATUS] RF-Input %d : Available(%d), Allocated(%d), Freq(%d)\n", ulIndex,
						pstLiveConfInfo->astRfInfo[ulIndex].bAvailable,
						pstLiveConfInfo->astRfInfo[ulIndex].bAllocated,
						pstLiveConfInfo->astRfInfo[ulIndex].unTuneInfo.ter.ulFrequency);
#endif
	}

	HxLOG_Debug ("\n");
	HxLOG_Debug ("[RSCSTATUS] Tuner Number = %d\n", pstLiveConfInfo->ulTunerInfoNum);
	for (ulIndex = 0; ulIndex < pstLiveConfInfo->ulTunerInfoNum; ulIndex++)
	{
#if defined(CONFIG_MW_CH_SATELLITE)
		HxLOG_Debug ("[RSCSTATUS] Tuner %d : Available(%d), Allocated(%d), Freq(%d)\n", ulIndex,
						pstLiveConfInfo->astTunerInfo[ulIndex].bAvailable,
						pstLiveConfInfo->astTunerInfo[ulIndex].bAllocated,
						pstLiveConfInfo->astTunerInfo[ulIndex].unTuneInfo.sat.tuningInfo.ulFrequency);
#elif defined(CONFIG_MW_CH_CABLE)
		HxLOG_Debug ("[RSCSTATUS] Tuner %d : Available(%d), Allocated(%d), Freq(%d)\n", ulIndex,
						pstLiveConfInfo->astTunerInfo[ulIndex].bAvailable,
						pstLiveConfInfo->astTunerInfo[ulIndex].bAllocated,
						pstLiveConfInfo->astTunerInfo[ulIndex].unTuneInfo.cab.ulFrequency);
#elif defined(CONFIG_MW_CH_TERRESTRIAL)
		HxLOG_Debug ("[RSCSTATUS] Tuner %d : Available(%d), Allocated(%d), Freq(%d)\n", ulIndex,
						pstLiveConfInfo->astTunerInfo[ulIndex].bAvailable,
						pstLiveConfInfo->astTunerInfo[ulIndex].bAllocated,
						pstLiveConfInfo->astTunerInfo[ulIndex].unTuneInfo.ter.ulFrequency);
#endif
	}

	HxLOG_Debug ("\n\n\n\n");
	return ERR_OK;
}

STATIC void om_media_play_live_PrintRctLinkInfo(HUINT32 ulIndex, DxService_RctLinkInfo_t *pstRctLinkInfo)
{
	HxLOG_Debug("===============< %d th RCT Link INFO> =======================\n", ulIndex);
	HxLOG_Debug("LinkType (%d) HowRelatedSchemeId(%d), TermId(%d) GroupId(%d) Precedence(%d) \n",
		pstRctLinkInfo->ucLinkType, pstRctLinkInfo->ucHowRelatedSchemeId, pstRctLinkInfo->usTermId, pstRctLinkInfo->ucGroupId, pstRctLinkInfo->ucPrecedence);

	if(pstRctLinkInfo->stMediaUri.ucMediaUriLen > 0)
		HxLOG_Debug("UriInfo.szMediaUri : %s \n", pstRctLinkInfo->stMediaUri.szMediaUri);
	else
		HxLOG_Debug("UriInfo NULL\n");

	HxLOG_Debug("IdentifierType(%d) ScheTimeReliability(%d) InlineSvc(%d) StartDate(0x%x) \n",
		pstRctLinkInfo->stDvbBinLocator.ucIdentifierType, pstRctLinkInfo->stDvbBinLocator.ucScheTimeReliability,
		pstRctLinkInfo->stDvbBinLocator.ucInlineSvc, pstRctLinkInfo->stDvbBinLocator.usStartDate );

	if(pstRctLinkInfo->stDvbBinLocator.ucInlineSvc == 0)
		HxLOG_Debug("DvbSvcTripletId(0x%x)\n", pstRctLinkInfo->stDvbBinLocator.usDvbSvcTripletId );
	else
		HxLOG_Debug("TsId(0x%x) OnId(0x%x) SvcId(0x%x)\n", pstRctLinkInfo->stDvbBinLocator.usTsId, pstRctLinkInfo->stDvbBinLocator.usOnId, pstRctLinkInfo->stDvbBinLocator.usSvcId );

	HxLOG_Debug("StartTime(0x%x) Duration(0x%x) \n", pstRctLinkInfo->stDvbBinLocator.usStartDate, pstRctLinkInfo->stDvbBinLocator.usDuration);

	if(pstRctLinkInfo->stDvbBinLocator.ucIdentifierType== 1)
		HxLOG_Debug("EvtID(0x%x) \n", pstRctLinkInfo->stDvbBinLocator.usEvtId);
	else if(pstRctLinkInfo->stDvbBinLocator.ucIdentifierType== 2)
		HxLOG_Debug("TvaId(0x%x) \n", pstRctLinkInfo->stDvbBinLocator.usTvaId);
	else if(pstRctLinkInfo->stDvbBinLocator.ucIdentifierType== 3)
		HxLOG_Debug("TvaId(0x%x) Component(%d) \n", pstRctLinkInfo->stDvbBinLocator.usTvaId,  pstRctLinkInfo->stDvbBinLocator.ucComponent);
	else
		HxLOG_Debug("DvbBinLocator.ucIdentifierType is Invalid \n");

	if(pstRctLinkInfo->stDvbBinLocator.ucIdentifierType == 0 && pstRctLinkInfo->stDvbBinLocator.ucScheTimeReliability ==1)
		HxLOG_Debug("EarlyStartWin(%d), LateEndWin(%d) \n", pstRctLinkInfo->stDvbBinLocator.ucEarlyStartWin, pstRctLinkInfo->stDvbBinLocator.ucLateEndWin);

	HxLOG_Debug("ucProTextNum(%d) \n",pstRctLinkInfo->ucProTextNum );

	if(pstRctLinkInfo->stPromotionalText.ucPromotionalTextLen >0)
		HxLOG_Debug("PromotionalTextLen(%d) LangCode(%s) PromotionalText(%s)\n",
			pstRctLinkInfo->stPromotionalText.ucPromotionalTextLen, pstRctLinkInfo->stPromotionalText.szLangCode, pstRctLinkInfo->stPromotionalText.szPromotionalText);
	else
		HxLOG_Debug("PromotionalText is Invalid \n");

	HxLOG_Debug("=============================================================\n\n");
}
STATIC void om_media_play_live_PrintRctEvtInfo(DxService_RctShortEvtInfo_t *pstRctShortEvtInfo)
{
	HxLOG_Debug("======== <RCT Event Info> ======================================= \n");
	HxLOG_Debug("LangCode (%s) EventName(%s) \n", pstRctShortEvtInfo->szLangCode, pstRctShortEvtInfo->szEventName);
	HxLOG_Debug("Text (%s)  \n", pstRctShortEvtInfo->szEventText);
	HxLOG_Debug("=============================================================\n\n");
}
STATIC void om_media_play_live_PrintRctImgIconInfo(DxService_RctImageIconInfo_t *pstRctImgIconInfo)
{
	HxLOG_Debug("======== <RCT ImgIcon Info> ======================================= \n");
	HxLOG_Debug("IconId(0x%04x) TransPortMode(0x%04x) PositionFlag(%d) CoordinateSystem(%d) HorizontalOrigin(%d) VerticalOrigin(%d) \n",
		pstRctImgIconInfo->ucIconId,
		pstRctImgIconInfo->ucTransPortMode,
		pstRctImgIconInfo->ucPositionFlag,
		pstRctImgIconInfo->ucCoordinateSystem,
		pstRctImgIconInfo->usHorizontalOrigin,
		pstRctImgIconInfo->usVerticalOrigin);

	HxLOG_Debug("szIconTypeChar(%s) \n", pstRctImgIconInfo->szIconTypeChar);
	if(pstRctImgIconInfo->ulDataByteSize>0)
		HxLOG_Debug("ulDataByteSize(%d)  pucDatabyte(%s) \n",	pstRctImgIconInfo->ulDataByteSize, pstRctImgIconInfo->pucDatabyte);
	else
		HxLOG_Debug("ulDataByteSize is Zero \n");

	HxLOG_Debug("szIconUrl(%s) \n", pstRctImgIconInfo->szIconUrl);
	HxLOG_Debug("=============================================================\n\n");
}

STATIC void om_media_play_live_PrintRctInfo(DxService_RctInfo_t *pstRctInfo)
{
	HUINT32	ulIndex;

	HxLOG_Debug("===============< RCT INFO> =======================\n\n");

	HxLOG_Debug("usServiceId(0x%04x) usYearOffset(0x%04x) ulTotalLinkCnt(%d) \n\n",
		pstRctInfo->usServiceId, pstRctInfo->usYearOffset, pstRctInfo->ulTotalLinkCnt);

	// Link Info Print 필요..
	for(ulIndex = 0; ulIndex < pstRctInfo->ulTotalLinkCnt; ulIndex++)
	{
		om_media_play_live_PrintRctLinkInfo(ulIndex, &pstRctInfo->pstLinkInfoList[ulIndex]);
	}

	// short event print
	om_media_play_live_PrintRctEvtInfo(&pstRctInfo->stShortEvtInfo);

	// img icon Print
	om_media_play_live_PrintRctImgIconInfo(&pstRctInfo->stImageIconInfo);

	HxLOG_Debug("============================================\n\n");

}

#endif

STATIC HERROR om_media_play_live_UpdateSvcInfoPID(Handle_t hAction, Handle_t hService)
{
	HBOOL			 	 bChanged = FALSE;
	DbSvc_Info_t		*pstOrgSvcInfo = NULL, *pstDstSvcInfo = NULL;
	HUINT16				 usSvcId = 0;
	HERROR				 hErr = ERR_FAIL;

	pstOrgSvcInfo = (DbSvc_Info_t *)OxMGR_Malloc (sizeof(DbSvc_Info_t));
	pstDstSvcInfo = (DbSvc_Info_t *)OxMGR_Malloc (sizeof(DbSvc_Info_t));

	if ((NULL != pstOrgSvcInfo) && (NULL != pstDstSvcInfo))
	{
		// 현재 db에 저장하고 있는 service info를 가져오고
		hErr = DB_SVC_GetServiceInfo(hService, pstOrgSvcInfo);
		if (ERR_FAIL == hErr)
		{
			HxLOG_Error("DB_SVC_GetServiceInfo() failed.. : hService(0x%X)", hService);
			goto EXIT_FUNC;
		}

		// si table에서 svc info 를 가져오기 위해 svc id를 가져오고
		hErr = DB_SVC_GetServiceIdFromSvcHandle (hService, &usSvcId);
		if (ERR_FAIL == hErr)
		{
			HxLOG_Error("DB_SVC_GetServiceIdFromSvcHandle() failed.. : hService(0x%X)", hService);
			goto EXIT_FUNC;
		}

		// si raw에서 svc info를 가져온다
		hErr = MGR_LIVE_UpdateServiceInfo (hAction, (HUINT32)usSvcId, pstOrgSvcInfo, pstDstSvcInfo, &bChanged);
		HxLOG_Info("pstOrgSvcInfo!=pstDstSvcInfo,  bChanged: %d \n", bChanged);
		if (ERR_FAIL == hErr)
		{
			HxLOG_Error("MGR_LIVE_UpdateServiceInfo() failed.. : hService(0x%X)", hService);
			goto EXIT_FUNC;
		}

		// 두 svc info를 비교해서 다르면 db에 업데이트 한다
		if (TRUE == bChanged)
		{
			hErr = DB_SVC_UpdateServiceInfo (hService, pstDstSvcInfo);
			if (ERR_FAIL == hErr)
			{
				HxLOG_Error("DB_SVC_UpdateServiceInfo() failed.. : hService(0x%X)", hService);
				goto EXIT_FUNC;
			}
		}
	}

	hErr = ERR_OK;

EXIT_FUNC:

	if (NULL != pstOrgSvcInfo)				{ OxMGR_Free (pstOrgSvcInfo); }
	if (NULL != pstDstSvcInfo)				{ OxMGR_Free (pstDstSvcInfo); }

	return hErr;
}

#if defined(CONFIG_3RD_MHEG)
/**
  *	[IN] 	ppaucRawSec : 배열로 구성된 raw section data
  * 	[IN]		ulSecCount : section의 갯수
    *	[OUT]	paulSecLen : 각 section의 길이
  *	[OUT] 	pulTotalSecLen : 전체 raw section len (== sizeof pucMergedRawSec)
  *	[OUT]	ppucMergedRawSec : 배열로 구성된 raw section을 이어붙여 하나로 합침 (이 함수내에서 alloc)
**/
STATIC HERROR om_media_play_live_PrepareRawSection(HUINT8 **ppaucRawSec, HUINT32 ulSecCount, HUINT32 *paulSecLen, HUINT32 *pulTotalSecLen, HUINT8 **ppucMergedRawSec)
{
	HUINT32		i;
	HUINT8		*pucRaw = NULL;
	HUINT8		*pucNewRawSec = NULL;
	HUINT32		ulTotalSecLen = 0, ulCurPos = 0, ulSize = 0;

	if ((ppaucRawSec == NULL)
		|| (ulSecCount < 1)
		|| (paulSecLen == NULL)
		|| (pulTotalSecLen == NULL)
		|| (ppucMergedRawSec == NULL))
	{
		HxLOG_Error("Invalid param..! \n");
		return ERR_FAIL;
	}

	// get section length
	for (i = 0; i < ulSecCount; i++)
	{
		pucRaw = ppaucRawSec[i];
		if(pucRaw == NULL)
		{
			HxLOG_Error("Invalid param..! \n");
			return ERR_FAIL;
		}

		ulSize = (HUINT32)HxMACRO_Get12Bit(pucRaw + 1) + 3;
		paulSecLen[i] = ulSize;
		ulTotalSecLen += ulSize;
	}

	if (ulTotalSecLen == 0)
	{
		HxLOG_Error("Check this..! Zero section length..!! \n");
		return ERR_FAIL;
	}
	*pulTotalSecLen = ulTotalSecLen;

	// pack raw section for OAPI
	pucNewRawSec = (HUINT8 *)OxMGR_Malloc(sizeof(HUINT8) * ulTotalSecLen);
	if (pucNewRawSec == NULL)
	{
		HxLOG_Error("OxMGR_Calloc Error: size(%d) \n", ulTotalSecLen);
		return ERR_FAIL;
	}

	for (i = 0; i < ulSecCount; i++)
	{
		pucRaw = ppaucRawSec[i];
		ulSize = paulSecLen[i];

		HxSTD_MemCopy(&pucNewRawSec[ulCurPos], pucRaw, ulSize);
		ulCurPos += ulSize;
	}

	*ppucMergedRawSec = pucNewRawSec;
	return ERR_OK;
}
#endif

#define _____OAPI_CALLBACK_FUNCTIONS_____
STATIC void om_media_play_live_CbOnLocked (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HUINT32 ulSvcUid)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnLocked, "iiii", ulViewId, ulSessionId, eMediaType, ulSvcUid);
}
STATIC void om_media_play_live_CbOnNoSignal (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HUINT32 ulSvcUid)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnNoSignal, "iiii", ulViewId, ulSessionId, eMediaType, ulSvcUid);
}
STATIC void om_media_play_live_CbOnAntennaOk (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HUINT32 ulSvcUid)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAntennaOk, "iiii", ulViewId, ulSessionId, eMediaType, ulSvcUid);
}
STATIC void om_media_play_live_CbOnAntennaNok (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HUINT32 ulSvcUid)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAntennaNok, "iiii", ulViewId, ulSessionId, eMediaType, ulSvcUid);
}
STATIC void om_media_play_live_CbOnSvcChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HUINT32 ulSvcUid)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSvcChanged, "iiii", ulViewId, ulSessionId, eMediaType, ulSvcUid);
}
STATIC void om_media_play_live_CbOnSvcStopped (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HUINT32 ulSvcUid)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSvcStopped, "iiii", ulViewId, ulSessionId, eMediaType, ulSvcUid);
}
STATIC void om_media_play_live_CbOnConflictChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_ConflictInfo_t *pstLiveConflictInfo)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnConflictChanged, "iiib", ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, pstLiveConflictInfo, sizeof(OxMediaPlay_ConflictInfo_t) );
}
STATIC void om_media_play_live_CbOnEventRelay (HUINT32 ulViewId, HUINT32 ulSessionId,  HINT32 nEvtRealyHandle)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnEventRelay, "iiii", ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, nEvtRealyHandle);
}
STATIC void om_media_play_live_CbOnEws (HUINT32 ulViewId, HUINT32 ulSessionId, HINT32	nEwssvcuid, HINT32 nEwscursvcuid)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnEws, "iiii", ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, nEwssvcuid, nEwscursvcuid);
}
STATIC void om_media_play_live_CbOnViewStateChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_LiveViewState_e eViewState)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnViewStateChanged, "iiii", ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, eViewState);
}
STATIC void om_media_play_live_CbOnLockStateChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_LiveLockState_e eLockState)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnLockStateChanged, "iiii", ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, eLockState);
}
#if defined(CONFIG_3RD_MHEG_SI_MONITORING)
STATIC void om_media_play_live_CbOnMhegNDTChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_TuningMode_e eTuneMode)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnMhegNDTChanged, "iiii", ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, eTuneMode);
}
#endif
#if 0//defined(CONFIG_MW_SI_AIT)
STATIC HERROR om_media_play_live_CbOnSiAITChanged (omMediaPlay_Context_t *pstContext, Handle_t hAction, MgrHbbtv_AitState_e eAitState, Handle_t hHbbtvAction, HUINT16 usAitPid)
{
	HUINT32							 ulViewId = 0;
	HUINT32							 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32							 ulSessionId = pstContext->ulSessionId;
	HUINT32							 i, ulAitSecCount = 0, ulAitTotalLen = 0;
	HUINT8							 ucDummyRaw = 0;
	HUINT8							*pucAllAitRawSec = NULL;
	HUINT8							*pucMemBuffer = NULL;
	HUINT32							 aulSecLen[OxMP_AIT_SECTION_NUM_MAX];
	OxMediaPlay_AitSectionData_t	 stAitSectionData;
	HERROR							 hErr, hResult = ERR_FAIL;

	if (pstContext->ulActionId != ulActionId)
	{
		goto END_FUNC;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		goto END_FUNC;
	}

	HxSTD_MemSet(aulSecLen, 0x00, sizeof(aulSecLen));
	HxSTD_MemSet(&stAitSectionData, 0x00, sizeof(OxMediaPlay_AitSectionData_t));

	switch(eAitState)
	{
		case eMgrHbbtvAitState_NoAit:		stAitSectionData.eAitState = eAitState_None;		break;
		case eMgrHbbtvAitState_Timeout:		stAitSectionData.eAitState = eAitState_Timeout;		break;
		case eMgrHbbtvAitState_Exist:		stAitSectionData.eAitState = eAitState_Received;	break;

		default:
			HxLOG_Error("invalid eAitState(%d) \n", eAitState);
			goto END_FUNC;
	}
	pstContext->stCache.stAitSecData.usPrevAitPid = pstContext->stCache.stAitSecData.usCurrAitPid;
	pstContext->stCache.stAitSecData.usCurrAitPid = usAitPid;
	stAitSectionData.usPrevAitPid = pstContext->stCache.stAitSecData.usPrevAitPid;
	stAitSectionData.usCurrAitPid = pstContext->stCache.stAitSecData.usCurrAitPid;

	if (stAitSectionData.eAitState == eAitState_Received)
	{
		HUINT32		ulCurPos 			= 0;
		HUINT32 	 ulHbbtvActId = SVC_PIPE_GetActionId(hHbbtvAction);
		HUINT8		*pucSiRawSection	=	NULL;
		HUINT8		*paucSitRawSec[OxMP_AIT_SECTION_NUM_MAX]	=	{NULL, };

		hErr = SVC_SI_GetAitRawSection(ulHbbtvActId, OxMP_AIT_SECTION_NUM_MAX, &paucSitRawSec[0], &ulAitSecCount);
		if ((ERR_OK == hErr) && (ulAitSecCount > 0))
		{
			// count total ait length
			for (i = 0; i < ulAitSecCount; i++)
			{
				pucSiRawSection = paucSitRawSec[i];
				aulSecLen[i] = (HUINT32)HxMACRO_Get12Bit(pucSiRawSection + 1) + 3;
				ulAitTotalLen += aulSecLen[i];
			}

			// make ait section data for OAPI
			pucMemBuffer = (HUINT8 *)OxMGR_Calloc(sizeof(HUINT8) * ulAitTotalLen);
			if (pucMemBuffer == NULL)
			{
				HxLOG_Error("OxMGR_Calloc Error: size(%d) \n", ulAitTotalLen);
				goto END_FUNC;
			}

			ulCurPos = 0;
			for (i = 0; i < ulAitSecCount; i++)
			{
				pucSiRawSection = paucSitRawSec[i];
				HxSTD_MemCopy(&pucMemBuffer[ulCurPos], pucSiRawSection, aulSecLen[i]);
				ulCurPos += aulSecLen[i];
			}

			stAitSectionData.ulAitSecCount 	= ulAitSecCount;
			stAitSectionData.ulAitTotaSeclLen 	= ulAitTotalLen;
			HxSTD_MemCopy(stAitSectionData.aulAitSecLen, aulSecLen, sizeof(stAitSectionData.aulAitSecLen));

			pucAllAitRawSec = pucMemBuffer;
		}
		else
		{
			HxLOG_Error("SVC_SI_GetAitRawSection err: ulHbbtvActId (%d)\n", ulHbbtvActId);
			goto END_FUNC;
		}
	}
	else
	{
		stAitSectionData.ulAitTotaSeclLen = 1;
		pucAllAitRawSec = &ucDummyRaw;
	}

	HxSTD_MemSet(&(pstContext->stCache.stAitSecData),	0, sizeof(OxMediaPlay_AitSectionData_t) );
	pstContext->stCache.stAitSecData.eAitState = stAitSectionData.eAitState;
	pstContext->stCache.stAitSecData.ulAitSecCount = stAitSectionData.ulAitSecCount;
	pstContext->stCache.stAitSecData.ulAitTotaSeclLen = stAitSectionData.ulAitTotaSeclLen;

	om_media_play_CbOnSiAITChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, &stAitSectionData, pucAllAitRawSec);

	hResult = ERR_OK;

END_FUNC:
	if (NULL != pucMemBuffer)
	{
		OxMGR_Free(pucMemBuffer);
	}

	return hResult;
}
#endif

STATIC HERROR om_media_play_live_CbOnSiRCTChanged (omMediaPlay_Context_t *pstContext, Handle_t hAction, HUINT32 ulRctMsg)
{
	HUINT8							 ucDummyRaw = 0;
	HUINT32							 ulViewId;
	HUINT32							 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32							 ulSessionId = pstContext->ulSessionId;
	HUINT32							 ulSize, ulSize1;
	HINT32							 nSrzr, nSrzr1;
	Handle_t						 hSvc;
	OxMediaPlay_RctData_t			 stRctData;
	const HCHAR						*szText = NULL;
	void							*pvData, *pvData1;
	HERROR							 hErr = ERR_FAIL;

	if (pstContext->ulActionId != ulActionId)
	{
		return ERR_FAIL;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return ERR_FAIL;
	}

	hErr = DB_SVC_GetServiceHandleBySvcIdx((HUINT16)pstContext->stCache.stLive.ulMasterSvcUid, &hSvc);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("DB_SVC_GetServiceHandleBySvcIdx err: SvcUid(%d)\n", pstContext->stCache.stLive.ulMasterSvcUid);
		return ERR_FAIL;
	}

	HxSTD_MemSet(&stRctData, 0x00, sizeof(OxMediaPlay_RctData_t));

	switch(ulRctMsg)
	{
		case eSEVT_SI_RCT_NONE:		stRctData.eRctState = eRctState_None;		break;
		case eSEVT_SI_RCT_TIMEOUT:	stRctData.eRctState = eRctState_Timeout;	break;
		case eSEVT_SI_RCT:			stRctData.eRctState = eRctState_Received;	break;
		default:
			HxLOG_Error("invalid evt si RCT msg(%d) \n",  ulRctMsg);
			return ERR_FAIL;
	}

	HxLOG_Debug("ulRctMsg(0x%08x) stRctData.eRctState (%d) \n", ulRctMsg, stRctData.eRctState);

	if (stRctData.eRctState == eRctState_Received)
	{
#if defined(CONFIG_3RD_MHEG)
		hErr = SVC_SI_GetRctInfo(hAction, hSvc, &(stRctData.stRctInfo), s_bMhegInterruptibleFlag_InOm);
#endif
		if (ERR_OK == hErr)
		{
#if defined(CONFIG_DEBUG)
			//om_media_play_live_PrintRctInfo(&(stRctData.stRctInfo));
#endif
			//  <- pstRctInfo serializied ..
			nSrzr = HLIB_SERIALIZER_Open();

			// <-image icon descriptor serialized data
			nSrzr1 = HLIB_SERIALIZER_Open();

			if(stRctData.stRctInfo.stImageIconInfo.ulDataByteSize)
			{
				HLIB_SERIALIZER_MakeSerializedData(nSrzr1, "iiiiiibibb",
												(HINT32)stRctData.stRctInfo.stImageIconInfo.ucIconId,
												(HINT32)stRctData.stRctInfo.stImageIconInfo.ucTransPortMode,
												(HINT32)stRctData.stRctInfo.stImageIconInfo.ucPositionFlag,
												(HINT32)stRctData.stRctInfo.stImageIconInfo.ucCoordinateSystem,
												(HINT32)stRctData.stRctInfo.stImageIconInfo.usHorizontalOrigin,
												(HINT32)stRctData.stRctInfo.stImageIconInfo.usVerticalOrigin,
												stRctData.stRctInfo.stImageIconInfo.szIconTypeChar,
												(HINT32) ((DxRCT_MAX_IMAGE_ICON_TYPE_CHART + 1) * sizeof(HUINT8)),
												(HINT32)stRctData.stRctInfo.stImageIconInfo.ulDataByteSize,
												stRctData.stRctInfo.stImageIconInfo.pucDatabyte,
												(HINT32) ((stRctData.stRctInfo.stImageIconInfo.ulDataByteSize) * sizeof(HUINT8)),
												stRctData.stRctInfo.stImageIconInfo.szIconUrl,
												(HINT32) ((DxRCT_MAX_IMAGE_ICON_URL_LEN + 1) * sizeof(HUINT8))
												);
			}
			else
			{
				HLIB_SERIALIZER_MakeSerializedData(nSrzr1, "iiiiiibib",
												(HINT32)stRctData.stRctInfo.stImageIconInfo.ucIconId,
												(HINT32)stRctData.stRctInfo.stImageIconInfo.ucTransPortMode,
												(HINT32)stRctData.stRctInfo.stImageIconInfo.ucPositionFlag,
												(HINT32)stRctData.stRctInfo.stImageIconInfo.ucCoordinateSystem,
												(HINT32)stRctData.stRctInfo.stImageIconInfo.usHorizontalOrigin,
												(HINT32)stRctData.stRctInfo.stImageIconInfo.usVerticalOrigin,
												stRctData.stRctInfo.stImageIconInfo.szIconTypeChar,
												(HINT32) ((DxRCT_MAX_IMAGE_ICON_TYPE_CHART + 1) * sizeof(HUINT8)),
												(HINT32)stRctData.stRctInfo.stImageIconInfo.ulDataByteSize,
												stRctData.stRctInfo.stImageIconInfo.szIconUrl,
												(HINT32) ((DxRCT_MAX_IMAGE_ICON_URL_LEN + 1) * sizeof(HUINT8))
												);
			}

			if ((szText = HLIB_SERIALIZER_Error(nSrzr1)) != NULL)
			{
				HxLOG_Error("Serialize failed : %s\n", szText);
				HLIB_SERIALIZER_Close(nSrzr1);
				HLIB_SERIALIZER_Close(nSrzr);
				goto Error;
			}

			pvData1 = HLIB_SERIALIZER_Pack(nSrzr1, &ulSize1);

			// -> image icon descriptor serialized data
			HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iiibbb",
														(HINT32)stRctData.stRctInfo.usServiceId,
														(HINT32)stRctData.stRctInfo.usYearOffset,
														(HINT32)stRctData.stRctInfo.ulTotalLinkCnt,
														stRctData.stRctInfo.pstLinkInfoList,
														(HINT32)(sizeof(DxService_RctLinkInfo_t) *stRctData.stRctInfo.ulTotalLinkCnt ),
														&stRctData.stRctInfo.stShortEvtInfo,
														(HINT32)(sizeof(DxService_RctShortEvtInfo_t)),
														pvData1,
														ulSize1);
			if ((szText = HLIB_SERIALIZER_Error(nSrzr)) != NULL)
			{
				HxLOG_Error("Serialize failed : %s\n", szText);
				HLIB_SERIALIZER_Close(nSrzr1);
				HLIB_SERIALIZER_Close(nSrzr);

				goto Error;
			}
			//	-> pstRctInfo serializied ..

			pvData = HLIB_SERIALIZER_Pack(nSrzr, &ulSize);

			// DSMCC로 오는 RCT Image Icon 을 webapp에서 가져갈 때 sync 문제가 생겨서 약간의 delay를 줌.
			HLIB_STD_TaskSleep(10);

			om_media_play_CbOnSiRCTChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, stRctData.eRctState, pvData, ulSize);

			HLIB_SERIALIZER_Close(nSrzr1);
			HLIB_SERIALIZER_Close(nSrzr);
		}
		else
		{
			pvData = &ucDummyRaw;
			ulSize = 1;

			om_media_play_CbOnSiRCTChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, eRctState_None, pvData, ulSize);
		}
	}
	else
	{
		pvData = &ucDummyRaw;
		ulSize = 1;

		om_media_play_CbOnSiRCTChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, stRctData.eRctState, pvData, ulSize);
	}

Error:
	if(stRctData.stRctInfo.pstLinkInfoList != NULL)
		OxAP_Free(stRctData.stRctInfo.pstLinkInfoList);

	if(stRctData.stRctInfo.stImageIconInfo.pucDatabyte!= NULL)
		OxAP_Free(stRctData.stRctInfo.stImageIconInfo.pucDatabyte);

	return MESSAGE_PASS;
}

#define _____MSG_PROCESS_FUNCTIONS_____
STATIC HERROR om_media_play_live_resetSubtitleTable(omMediaPlay_Context_t *pstContext, Handle_t hAction)
{
	OxMediaPlay_SubtitleCompTable_t 	*pstSubtitleCompTable;
	HUINT32				 				 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32				 				 ulViewId = 0;
	HUINT32								 ulSessionId;
	Handle_t			 				 hErr;


	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return ERR_FAIL;
	}

	ulSessionId = pstContext->ulSessionId;
	pstSubtitleCompTable = &pstContext->stCache.stSubtitleCompTable;

	pstSubtitleCompTable->nArraySize	=	0;
	pstSubtitleCompTable->nMaxSize		=	sizeof(OxMediaPlay_SubtitleCompTable_t);
	pstSubtitleCompTable->nCurrIndex	=	-1;

	om_media_play_CbOnComponentSubtitleChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, pstSubtitleCompTable);
	om_media_play_CbOnComponentSelected(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, eOxMP_COMPONENT_SUBTITLE, pstSubtitleCompTable->nCurrIndex, -1);

	return ERR_OK;
}

STATIC BUS_Result_t om_media_play_live_MsgWtvSvcChanged (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32				 ulViewId = 0, ulSvcUid = 0;
	HUINT32				 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32				 ulSessionId = pstContext->ulSessionId;
	Handle_t			 hSuppleSvc = (Handle_t)p1;
	Handle_t			 hErr;
#if defined(CONFIG_3RD_MHEG_SI_MONITORING)
	OxMediaPlay_TuningMode_e	eTuneMode = (OxMediaPlay_TuningMode_e)p2;
#endif

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	om_media_play_live_resetSubtitleTable(pstContext, hAction);

	ulSvcUid = (HUINT32)DB_SVC_GetServiceUID(hSuppleSvc);
#if defined(CONFIG_3RD_MHEG_SI_MONITORING)
	if(eTuneMode & eOxMP_TUNEMODE_NDT_TUNE)
	{
		om_media_play_live_CbOnMhegNDTChanged(ulViewId, ulSessionId, eTuneMode);
	}
#endif
	om_media_play_live_CbOnSvcChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, ulSvcUid);

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_live_MsgWtvStop (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32				 ulViewId = 0, ulSvcUid = 0;
	HUINT32				 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32				 ulSessionId = pstContext->ulSessionId;
	Handle_t			 hSuppleSvc = (Handle_t)p1;
	Handle_t			 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	ulSvcUid = (HUINT32)DB_SVC_GetServiceUID(hSuppleSvc);

	/* clean cache */
	om_media_play_CleanCache(pstContext, eOxMP_MEDIATYPE_LIVE);

	om_media_play_live_CbOnSvcStopped(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, ulSvcUid);

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_live_MsgLiveViewState (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulViewId = 0;
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32					 ulSessionId = pstContext->ulSessionId;
	MgrLiveView_UiState_e	 eViewState;
	Handle_t				 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	hErr  = MGR_LIVE_GetState(ulActionId, eLiveProc_View, (HUINT32 *)&eViewState);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_LIVE_GetState Error\n");
		return MESSAGE_PASS;
	}

	om_media_play_live_CbOnViewStateChanged(ulViewId, ulSessionId, eViewState);

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_live_MsgLiveLockState (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulViewId = 0;
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32					 ulSessionId = pstContext->ulSessionId;
	MgrLiveLock_UiState_e	 eLockState;
	Handle_t				 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	hErr  = MGR_LIVE_GetState(ulActionId, eLiveProc_Lock, (HUINT32 *)&eLockState);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_LIVE_GetState Error\n");
		return MESSAGE_PASS;
	}

	om_media_play_live_CbOnLockStateChanged(ulViewId, ulSessionId, eLockState);

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_live_MsgEventRelay (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32					 nEvtRealyHandle = (HINT32)p1;
	HUINT32					 ulViewId = 0;
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32					 ulSessionId = pstContext->ulSessionId;
	Handle_t				 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	om_media_play_live_CbOnEventRelay(ulViewId, ulSessionId, nEvtRealyHandle);

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_live_MsgEventEws (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32					 nEwssvcuid = (HINT32)p1;
	HINT32					 nEwscursvcuid = (HINT32)p2;
	HUINT32					 ulViewId = 0;
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32					 ulSessionId = pstContext->ulSessionId;
	Handle_t				 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	om_media_play_live_CbOnEws(ulViewId, ulSessionId, nEwssvcuid, nEwscursvcuid);

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_live_MsgSvclistRadioBgChanged (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HBOOL					 bRadioBackground = (HBOOL)p1;
	HUINT32					 ulViewId = 0;
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32					 ulSessionId = pstContext->ulSessionId;
	Handle_t				 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	om_media_play_CbOnRadioBGChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, bRadioBackground);

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_live_MsgRatingCtrlChanged (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32					 nBlocked =	(HINT32)p1;
	HINT32					 nRating =	(HINT32)p2;
#if defined (CONFIG_OP_SES)
	HINT32					 nBlockedByCas = (HINT32)p3;
#else
	HINT32					 nDrmSystemId = (HINT32)p3;
#endif
	HUINT32					 ulViewId = 0;
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32					 ulSessionId = pstContext->ulSessionId;
	Handle_t				 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

#if defined (CONFIG_OP_SES)
	om_media_play_CbOnRatingChanged( ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, nBlocked, nRating, nBlockedByCas);
#else
	om_media_play_CbOnRatingChanged( ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, nBlocked, nRating, nDrmSystemId );
#endif
	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

#if 0//defined(CONFIG_MW_SI_AIT)
STATIC BUS_Result_t om_media_play_live_MsgHbbtvNotifyAit (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR					 hErr;

	hErr = om_media_play_live_CbOnSiAITChanged(pstContext, hAction, (MgrHbbtv_AitState_e)p1, (Handle_t)p2, (HUINT16)p3);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("om_media_play_live_CbOnSiAITChanged err: State(%d), hHbbtvAction(0x%08x)\n", p1, p2);
	}

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}
#endif

STATIC BUS_Result_t om_media_play_live_EvtChLocked (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulViewId = 0;
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32					 ulSessionId = pstContext->ulSessionId;
	Handle_t				 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	om_media_play_live_CbOnLocked(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, pstContext->stCache.stLive.ulSuppleSvcUid);

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_live_EvtChNoSignal (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulViewId = 0;
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32					 ulSessionId = pstContext->ulSessionId;
	Handle_t				 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	om_media_play_live_CbOnNoSignal(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, pstContext->stCache.stLive.ulSuppleSvcUid);

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_live_EvtChAntennaOk (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulViewId = 0;
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32					 ulSessionId = pstContext->ulSessionId;
	Handle_t				 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	om_media_play_live_CbOnAntennaOk(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, pstContext->stCache.stLive.ulSuppleSvcUid);

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_live_EvtChAntennaNok (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulViewId = 0;
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32					 ulSessionId = pstContext->ulSessionId;
	Handle_t				 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	om_media_play_live_CbOnAntennaNok(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, pstContext->stCache.stLive.ulSuppleSvcUid);

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_live_MsgOutputAVStarted (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulViewId = 0;
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32					 ulSessionId = pstContext->ulSessionId;
	Handle_t				 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	om_media_play_CbOnAVStarted(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE);

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_live_MsgOutputAVStoped (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulViewId = 0;
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32					 ulSessionId = pstContext->ulSessionId;
	Handle_t				 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	om_media_play_CbOnAVStopped(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE);

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}


STATIC BUS_Result_t om_media_play_live_EvtAvVideoStarted (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32							 ulViewId = 0;
	HUINT32							 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32							 ulSessionId = pstContext->ulSessionId;
	VideoStreamInfo_t				 stVideoStreamInfo;
	OxMediaPlay_VideoStreamInfo_t	 stOapiVideoStreamInfo = {0,};
	Handle_t						 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	hErr = SVC_AV_GetVideoStreamInfo(ulActionId, &stVideoStreamInfo);
	if (ERR_OK == hErr)
	{
		stOapiVideoStreamInfo.ulHSize = stVideoStreamInfo.ulHorizontalSize;
		stOapiVideoStreamInfo.ulVSize = stVideoStreamInfo.ulVerticalSize;
		stOapiVideoStreamInfo.eAspectRatio = stVideoStreamInfo.eAspectRatio;
	}

	om_media_play_CbOnVideoStreamInfo(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, &stOapiVideoStreamInfo);

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_live_EvtAvVideoUnderrun (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_live_EvtAvAudioClipCompleted (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulViewId = 0;
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32					 ulSessionId = pstContext->ulSessionId;
	HUINT32					 ulReqId = (HUINT32)p1;
	Handle_t				 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	om_media_play_CbOnAudioClipCompleted(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, ulReqId);

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

#if defined(CONFIG_OP_SES)
STATIC HERROR om_media_play_live_IsCasChannel (omMediaPlay_Context_t *pstContext, Handle_t hAction)
{
	Handle_t		hSvc;
	DbSvc_Info_t	stSvcInfo;
	HERROR			hErr;

	if (NULL == pstContext)
	{
		HxLOG_Error("Error, pstContext is NULL\n");
		return ERR_FAIL;
	}

	hErr = DB_SVC_GetServiceHandleBySvcIdx((HUINT16)pstContext->stCache.stLive.ulMasterSvcUid, &hSvc);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("DB_SVC_GetServiceHandleBySvcIdx err: svcUID(%d)\n", pstContext->stCache.stLive.ulMasterSvcUid);
		goto END_FUNC;
	}

	hErr = DB_SVC_GetServiceInfo (hSvc, &stSvcInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("DB_SVC_GetServiceInfo err: SvcHandle(0x%08x)\n", hSvc);
		goto END_FUNC;
	}

	if(stSvcInfo.stDxInfo.casType != eDxCAS_TYPE_FTA)
	{
		return ERR_OK;
	}

END_FUNC:
	return ERR_FAIL;
}
#endif

STATIC BUS_Result_t om_media_play_live_EvtSiPmt (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{

	Handle_t				 hSvc;
	HUINT32					 ulViewId = 0;
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32					 ulSessionId = pstContext->ulSessionId;
	DbSvc_Info_t			 stSvcInfo;
	Handle_t				 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	hErr = DB_SVC_GetServiceHandleBySvcIdx ((HUINT16)pstContext->stCache.stLive.ulMasterSvcUid, &hSvc);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("DB_SVC_GetServiceHandleBySvcIdx() failed..!! hAction(0x%X) ulMasterSvcUid(%d) \n", hAction, pstContext->stCache.stLive.ulMasterSvcUid);
		goto END_FUNC;
	}

	hErr = DB_SVC_GetServiceInfo (hSvc, &stSvcInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("Error, SvcHandle(0x%08x)\n", hSvc);
		goto END_FUNC;
	}

	/*Video*/
	{
		OxMediaPlay_VideoCompTable_t *pstVideoCompTable	=	&pstContext->stCache.stVideoCompTable;

		HINT32 i = 0;
		HINT32	nVideo = 0;

		SvcSi_VideoList_t stVideoList;

		HxSTD_MemSet (&stVideoList, 0, sizeof(SvcSi_VideoList_t));
		HxSTD_MemSet (pstVideoCompTable, 0, sizeof(OxMediaPlay_VideoCompTable_t));

		hErr = SVC_SI_GetVideoList (ulActionId, DbSvc_GetSvcId(&stSvcInfo), &stVideoList);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("SVC_SI_GetVideoList Error. hSvc(0x%08x)\n", hSvc);
		}

		// min
		nVideo = stVideoList.nVideo<OxMP_VIDEO_ES_NUM_MAX?stVideoList.nVideo: OxMP_VIDEO_ES_NUM_MAX;

		pstVideoCompTable->nArraySize	=	nVideo;
		pstVideoCompTable->nMaxSize	=	sizeof(OxMediaPlay_VideoCompTable_t);
		pstVideoCompTable->nCurrIndex	=	0;

		{
			for( i=0 ; i<nVideo; i++ )
			{
				pstVideoCompTable->astCompArray[i].nPid				=	stVideoList.pstVideoInfo[i].usPid;
				pstVideoCompTable->astCompArray[i].eCodec			=	stVideoList.pstVideoInfo[i].eCodec;
				pstVideoCompTable->astCompArray[i].eCompType		=	eOxMP_COMPONENT_VIDEO;
				pstVideoCompTable->astCompArray[i].nHdVideo			=	stVideoList.pstVideoInfo[i].bIsHd;
				pstVideoCompTable->astCompArray[i].nComponentTag	=	stVideoList.pstVideoInfo[i].ucComponentTag;
				pstVideoCompTable->astCompArray[i].nCasFound		=	stVideoList.pstVideoInfo[i].bCaDesFound;
				pstVideoCompTable->astCompArray[i].nComponentType	=	stVideoList.pstVideoInfo[i].ucComponentType;
				HxLOG_Debug("Video component (%d/%d) pid(%d) codec(%d) tag(%d) type(%d)\n",
						i,nVideo, pstVideoCompTable->astCompArray[i].nPid
						,pstVideoCompTable->astCompArray[i].eCodec
						,pstVideoCompTable->astCompArray[i].nComponentTag
						,pstVideoCompTable->astCompArray[i].nComponentType);
		}
		}

		SVC_SI_FreeVideoList(&stVideoList);

		HxLOG_Debug("Video component table is changed [%d]\n", pstVideoCompTable->nArraySize);

		/* broadcast */
		om_media_play_CbOnComponentVideoChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, pstVideoCompTable );
	}

	/*Audio*/
	{
		OxMediaPlay_AudioCompTable_t *pstAudioCompTable = &pstContext->stCache.stAudioCompTable;

		HINT32 i = 0, j = 0;
		HINT32 nCurrIndex = -1;
		HUINT16	usAudioPid = 0x2000;
		HBOOL	bDolbyflag = FALSE;

		SvcSi_AudioList_t stAudioList;

		HxSTD_MemSet (&stAudioList, 0, sizeof(SvcSi_AudioList_t));
		HxSTD_MemSet (pstAudioCompTable, 0, sizeof(OxMediaPlay_AudioCompTable_t));

		hErr = SVC_SI_GetAudioList(ulActionId, DbSvc_GetSvcId(&stSvcInfo), &stAudioList);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("SVC_SI_GetAudioList Error. hSvc(0x%08x)\n", hSvc);
		}

		bDolbyflag = DbSvc_GetDolbyFlag(&stSvcInfo);
		if (bDolbyflag == TRUE)
		{
			usAudioPid = DbSvc_GetDolbyPid(&stSvcInfo);
		}
		else
		{
			usAudioPid = DbSvc_GetAudioPid(&stSvcInfo);
		}

		for( i=0 ; i<stAudioList.nAudio; i++ )
		{
			if( stAudioList.pstAudioInfo[i].usPid == usAudioPid )
			{
				nCurrIndex = i;
				break;
			}
		}

		// aux audio가 별도로 필요한 모델은 이렇게 처리
		// 아직 수요가 많지 않아서 UK DTT로 처리했는데 향후 다른 모델들도 지원이 필요하면
		// 여기에 define을 늘리지 말고 CONFIG을 추가해서 적용해야 함
#if defined(CONFIG_OP_UK_DTT)
		if (nCurrIndex == -1)
		{
			nCurrIndex = 0;
		}

		pstAudioCompTable->nCurrSubIndex = -1;
		usAudioPid = DbSvc_GetAudioAuxPid(&stSvcInfo);
		for(i = 0; i < stAudioList.nAudio; i++ )
		{
			if( stAudioList.pstAudioInfo[i].usPid == usAudioPid )
			{
				pstAudioCompTable->nCurrSubIndex = i;
				break;
			}
		}
#else
		if (nCurrIndex == -1) {
			usAudioPid = DbSvc_GetAudioAuxPid(&stSvcInfo);
			for( i=0 ; i<stAudioList.nAudio; i++ )
			{
				if( stAudioList.pstAudioInfo[i].usPid == usAudioPid )
				{
					nCurrIndex = i;
					break;
				}
			}
		}

		if (nCurrIndex == -1)
		{
			nCurrIndex = 0;
		}
#endif

		pstAudioCompTable->nArraySize	=	stAudioList.nAudio;
		pstAudioCompTable->nMaxSize	=	sizeof(OxMediaPlay_AudioCompTable_t);
		pstAudioCompTable->nCurrIndex	=	nCurrIndex;

		{
			HUINT32 ulSize = 0;

			for( i=0 ; i<stAudioList.nAudio; i++ )
			{
				pstAudioCompTable->astCompArray[i].nPid			=	stAudioList.pstAudioInfo[i].usPid;
				pstAudioCompTable->astCompArray[i].eCompType		=	eOxMP_COMPONENT_AUDIO;
				pstAudioCompTable->astCompArray[i].nComponentTag	=	stAudioList.pstAudioInfo[i].ucComponentTag;
				pstAudioCompTable->astCompArray[i].eCodec			=	stAudioList.pstAudioInfo[i].eCodec;
				pstAudioCompTable->astCompArray[i].bIsDolby		=	stAudioList.pstAudioInfo[i].bIsDolby;

				ulSize = sizeof(pstAudioCompTable->astCompArray[i].astLangInfo);
				if (ulSize > sizeof(stAudioList.pstAudioInfo[i].stIsoLangInfo))
				{
					ulSize = sizeof(stAudioList.pstAudioInfo[i].stIsoLangInfo);
				}
				HxSTD_MemCopy(pstAudioCompTable->astCompArray[i].astLangInfo, stAudioList.pstAudioInfo[i].stIsoLangInfo, ulSize);
				pstAudioCompTable->astCompArray[i].nLangNum = 0;

				for (j = 0; j < OxMP_AUDIO_LANG_NUM; j++)
				{
					if (pstAudioCompTable->astCompArray[i].astLangInfo[j].szLangCode[0] != 0)
					{
						pstAudioCompTable->astCompArray[i].nLangNum++;
					}
				}

				pstAudioCompTable->astCompArray[i].eSupAudType= stAudioList.pstAudioInfo[i].suppAudioEditorial;
				pstAudioCompTable->astCompArray[i].ucMixType = stAudioList.pstAudioInfo[i].ucMixType;
			}
		}

		SVC_SI_FreeAudioList(&stAudioList);

		HxLOG_Debug("Audio component table is changed [%d]\n", pstAudioCompTable->nArraySize);

		/* broadcast */
		om_media_play_CbOnComponentAudioChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, pstAudioCompTable );
		om_media_play_CbOnComponentSelected( ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, eOxMP_COMPONENT_AUDIO, nCurrIndex, pstAudioCompTable->nCurrSubIndex);
	}

	/*Subtitle*/
	{
		OxMediaPlay_SubtitleCompTable_t *pstSubtitleCompTable = &pstContext->stCache.stSubtitleCompTable;

		HINT32 i;
		HINT32 nCurrIndex = 0;
		SvcSi_SubtitleList_t stSubtitleList;

		HxSTD_MemSet (&stSubtitleList, 0, sizeof(SvcSi_SubtitleList_t));
		HxSTD_MemSet (pstSubtitleCompTable, 0, sizeof(OxMediaPlay_SubtitleCompTable_t));

		hErr = SVC_SI_GetSubtitleList(ulActionId, DbSvc_GetSvcId(&stSvcInfo), &stSubtitleList);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("SVC_SI_GetSubtitleList Error. hSvc(0x%08x)\n", hSvc);
#if !defined(CONFIG_SUPPORT_FCC)
			goto END_FUNC;
#endif
		}

		HxLOG_Debug("ucSubttlIdx (0x%x)\n", DbSvc_GetSubttlIdx(&stSvcInfo));
		if (DbSvc_GetSubttlIdx(&stSvcInfo) == 0xff)
		{
			nCurrIndex = -1;
		}
		else
		{
			nCurrIndex = DbSvc_GetSubttlIdx(&stSvcInfo);
		}

		pstSubtitleCompTable->nArraySize	=	stSubtitleList.ulSubtitleNum;
		pstSubtitleCompTable->nMaxSize	=	sizeof(OxMediaPlay_SubtitleCompTable_t);
		pstSubtitleCompTable->nCurrIndex	=	nCurrIndex;

		for( i=0 ; i<stSubtitleList.ulSubtitleNum; i++ )
		{
			pstSubtitleCompTable->astCompArray[i].nPid			=	stSubtitleList.pstSubtitleInfo[i].usPid;
			pstSubtitleCompTable->astCompArray[i].eCompType	=	eOxMP_COMPONENT_SUBTITLE;

			switch(stSubtitleList.pstSubtitleInfo[i].eType)
			{
				case eSI_SUBTITLE_DVB:
				{
					pstSubtitleCompTable->astCompArray[i].eSubtitleType	=	eOxMP_SUBTITLETYPE_Dvb;
					pstSubtitleCompTable->astCompArray[i].utArg.stDvbInfo.nTypeValue			=	stSubtitleList.pstSubtitleInfo[i].utArg.stDvbInfo.ucType;
					pstSubtitleCompTable->astCompArray[i].utArg.stDvbInfo.nComponentTag			=	stSubtitleList.pstSubtitleInfo[i].utArg.stDvbInfo.ucComponentTag;
					pstSubtitleCompTable->astCompArray[i].utArg.stDvbInfo.nCompositionPageId	=	stSubtitleList.pstSubtitleInfo[i].utArg.stDvbInfo.usCompositionPageId;
					pstSubtitleCompTable->astCompArray[i].utArg.stDvbInfo.nAncillaryPageId		=	stSubtitleList.pstSubtitleInfo[i].utArg.stDvbInfo.usAncillaryPageId;

					HLIB_STD_MemCpySafe((pstSubtitleCompTable->astCompArray[i].utArg.stDvbInfo.szLangCode), 4,
										(stSubtitleList.pstSubtitleInfo[i].utArg.stDvbInfo.aucLangCode), 4);
				}
				break;

				case eSI_SUBTITLE_EBU:
				{
					pstSubtitleCompTable->astCompArray[i].eSubtitleType	=	eOxMP_SUBTITLETYPE_Ebu;
					pstSubtitleCompTable->astCompArray[i].utArg.stEbuInfo.nTypeValue		=	stSubtitleList.pstSubtitleInfo[i].utArg.stEbuInfo.ucType;
					pstSubtitleCompTable->astCompArray[i].utArg.stEbuInfo.nComponentTag		=	stSubtitleList.pstSubtitleInfo[i].utArg.stEbuInfo.ucComponentTag;
					pstSubtitleCompTable->astCompArray[i].utArg.stEbuInfo.nMagazineNum		=	stSubtitleList.pstSubtitleInfo[i].utArg.stEbuInfo.ucMagazineNum;
					pstSubtitleCompTable->astCompArray[i].utArg.stEbuInfo.nPageNum			=	stSubtitleList.pstSubtitleInfo[i].utArg.stEbuInfo.ucPageNum;

					HLIB_STD_MemCpySafe((pstSubtitleCompTable->astCompArray[i].utArg.stEbuInfo.szLangCode), 4,
										(stSubtitleList.pstSubtitleInfo[i].utArg.stEbuInfo.aucLangCode), 4);
				}
				break;

				case eSI_SUBTITLE_SUPERCC:
				default:
					break;
			}
		}

		SVC_SI_FreeSubtitleList(&stSubtitleList);

		HxLOG_Debug("Subtitle component table is changed [%d]\n", pstSubtitleCompTable->nArraySize);
		/* broadcast */
		om_media_play_CbOnComponentSubtitleChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, pstSubtitleCompTable );
		om_media_play_CbOnComponentSelected( ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, eOxMP_COMPONENT_SUBTITLE, nCurrIndex, -1);
	}

	/* broadcast */
	om_media_play_CbOnSiPMTChanged( ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE );

END_FUNC:
	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_live_EvtSiEit (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulViewId = 0;
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32					 ulSessionId = pstContext->ulSessionId;
	Handle_t				 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	om_media_play_CbOnSiEITChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE);


	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_live_EvtSiRct (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	om_media_play_live_CbOnSiRCTChanged(pstContext, hAction, eSEVT_SI_RCT);
	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_live_EvtSiRctTimeout (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	om_media_play_live_CbOnSiRCTChanged(pstContext, hAction, eSEVT_SI_RCT_TIMEOUT);
	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_live_EvtSiRctNone(omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	om_media_play_live_CbOnSiRCTChanged(pstContext, hAction, eSEVT_SI_RCT_NONE);
	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_live_EvtRequestRctImageIcon(omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulViewId = 0;
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
//	HUINT32					 ulSessionId = pstContext->ulSessionId;
	HUINT8					 szIconUrl[DxRCT_MAX_IMAGE_ICON_URL_LEN + 1];
	Handle_t				 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	HLIB_STD_MemCpySafe(szIconUrl, DxRCT_MAX_IMAGE_ICON_URL_LEN + 1, (void *)p3, DxRCT_MAX_IMAGE_ICON_URL_LEN + 1);

	om_media_play_CbOnSiRCTRequestImageIcon(ulViewId, ulActionId, eOxMP_MEDIATYPE_LIVE, szIconUrl);

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_live_EvtAvMediaRectChanged (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulViewId = 0;
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32					 ulSessionId = pstContext->ulSessionId;
	Handle_t				 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	om_media_play_CbOnAVMediaRectChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE);

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_live_SvcPidChanged (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	Handle_t				 hSvc;
	HUINT32					 ulViewId = 0;
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32					 ulSessionId = pstContext->ulSessionId;
	DbSvc_Info_t			 stSvcInfo;
	Handle_t				 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	hErr = DB_SVC_GetServiceHandleBySvcIdx ((HUINT16)pstContext->stCache.stLive.ulMasterSvcUid, &hSvc);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("DB_SVC_GetServiceHandleBySvcIdx() failed..!! hAction(0x%X) ulMasterSvcUid(%d) \n", hAction, pstContext->stCache.stLive.ulMasterSvcUid);
		return MESSAGE_PASS;
	}

	HxSTD_memset(&stSvcInfo, 0x00, sizeof(DbSvc_Info_t));

	hErr = DB_SVC_GetServiceInfo (hSvc, &stSvcInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("Error, SvcHandle(0x%08x)\n", hSvc);
		return MESSAGE_PASS;
	}

	// Main language
	// eDB_PARAM_ITEM_LANG_MAIN
	{
		SvcSi_AudioList_t 	stAudioList;

		HUINT32			i , j, ulSize = 0;
		HBOOL			bDolbyflag = FALSE;
		HINT32 			nOriCurrIndex = -1, nOriCurrSubIndex = -1;
		HINT32			nCurrIndex = -1, nCurrSubIndex = -1;
		HUINT16 		usAudioPid = 0x2000;
#if defined(CONFIG_OP_UK_DTT)
		HUINT16			usAuxAudioPid = 0x2000;
#endif
		OxMediaPlay_AudioCompTable_t *pstAudioCompTable = &pstContext->stCache.stAudioCompTable;

		HxSTD_memset(&stAudioList, 0x00, sizeof(SvcSi_AudioList_t));

		nOriCurrIndex = pstContext->stCache.stAudioCompTable.nCurrIndex;
		nOriCurrSubIndex = pstContext->stCache.stAudioCompTable.nCurrSubIndex;

		hErr = SVC_SI_GetAudioList(ulActionId, DbSvc_GetSvcId(&stSvcInfo), &stAudioList);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("SVC_SI_GetAudioList Error. hSvc(0x%08x)\n", hSvc);
		}
		else
		{
			bDolbyflag = DbSvc_GetDolbyFlag(&stSvcInfo);
			if (bDolbyflag == TRUE)
			{
				usAudioPid = DbSvc_GetDolbyPid(&stSvcInfo);
			}
			else
			{
				usAudioPid = DbSvc_GetAudioPid(&stSvcInfo);
			}

			for( i=0 ; i<stAudioList.nAudio; i++ )
			{
				if( stAudioList.pstAudioInfo[i].usPid == usAudioPid )
				{
					nCurrIndex = i;
					break;
				}
			}

			// aux audio가 별도로 필요한 모델은 이렇게 처리
			// 아직 수요가 많지 않아서 UK DTT로 처리했는데 향후 다른 모델들도 지원이 필요하면
			// 여기에 define을 늘리지 말고 CONFIG을 추가해서 적용해야 함
#if defined(CONFIG_OP_UK_DTT)
			usAuxAudioPid = DbSvc_GetAudioAuxPid(&stSvcInfo);
			for(i = 0; i < stAudioList.nAudio; i++ )
			{
				if( stAudioList.pstAudioInfo[i].usPid == usAuxAudioPid )
				{
					nCurrSubIndex = i;
					break;
				}
			}

			if (nCurrIndex == -1)
			{
				nCurrIndex = 0;
			}

#else
			if (nCurrIndex == -1) {
				usAudioPid = DbSvc_GetAudioAuxPid(&stSvcInfo);
				for( i=0 ; i<stAudioList.nAudio; i++ )
				{
					if( stAudioList.pstAudioInfo[i].usPid == usAudioPid )
					{
						nCurrIndex = i;
						break;
					}
				}
			}

			if (nCurrIndex == -1)
			{
				nCurrIndex = 0;
			}
#endif

			pstAudioCompTable->nArraySize	=	stAudioList.nAudio;
			pstAudioCompTable->nMaxSize	=	sizeof(OxMediaPlay_AudioCompTable_t);
			pstAudioCompTable->nCurrIndex	=	nCurrIndex;
			pstAudioCompTable->nCurrSubIndex	=	nCurrSubIndex;

			for( i=0 ; i<stAudioList.nAudio; i++ )
			{
				pstAudioCompTable->astCompArray[i].nPid 		=	stAudioList.pstAudioInfo[i].usPid;
				pstAudioCompTable->astCompArray[i].eCompType		=	eOxMP_COMPONENT_AUDIO;
				pstAudioCompTable->astCompArray[i].nComponentTag	=	stAudioList.pstAudioInfo[i].ucComponentTag;
				pstAudioCompTable->astCompArray[i].eCodec			=	stAudioList.pstAudioInfo[i].eCodec;
				pstAudioCompTable->astCompArray[i].bIsDolby 	=	stAudioList.pstAudioInfo[i].bIsDolby;

				ulSize = sizeof(pstAudioCompTable->astCompArray[i].astLangInfo);
				if (ulSize > sizeof(stAudioList.pstAudioInfo[i].stIsoLangInfo))
				{
					ulSize = sizeof(stAudioList.pstAudioInfo[i].stIsoLangInfo);
				}
				HxSTD_MemCopy(pstAudioCompTable->astCompArray[i].astLangInfo, stAudioList.pstAudioInfo[i].stIsoLangInfo, ulSize);
				pstAudioCompTable->astCompArray[i].nLangNum = 0;

				for (j = 0; j < OxMP_AUDIO_LANG_NUM; j++)
				{
					if (pstAudioCompTable->astCompArray[i].astLangInfo[j].szLangCode[0] != 0)
					{
						pstAudioCompTable->astCompArray[i].nLangNum++;
					}
				}

				pstAudioCompTable->astCompArray[i].eSupAudType= stAudioList.pstAudioInfo[i].suppAudioEditorial;
				pstAudioCompTable->astCompArray[i].ucMixType = stAudioList.pstAudioInfo[i].ucMixType;
			}

			SVC_SI_FreeAudioList(&stAudioList);

			if(pstAudioCompTable->nCurrIndex != nOriCurrIndex || pstAudioCompTable->nCurrSubIndex != nOriCurrSubIndex )
			{
				HxLOG_Debug("eOxMP_COMPONENT_AUDIO = nCurrIndex(0x%x) \n", pstAudioCompTable->nCurrIndex);
				om_media_play_CbOnComponentAudioChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, pstAudioCompTable );
				om_media_play_CbOnComponentSelected( ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, eOxMP_COMPONENT_AUDIO, pstAudioCompTable->nCurrIndex, pstAudioCompTable->nCurrSubIndex);
			}
		}
	}
	{
		SvcSi_SubtitleList_t 			stSubtitleList;

		HINT32 	i;
		HINT32 	nCurrIndex = -1, nOriCurrIndex = -1;
		HBOOL	bSubtitleEnable = FALSE;

		OxMediaPlay_SubtitleCompTable_t *pstSubtitleCompTable = &pstContext->stCache.stSubtitleCompTable;

		nOriCurrIndex = pstContext->stCache.stSubtitleCompTable.nCurrIndex;

		HxSTD_memset(&stSubtitleList, 0x00, sizeof(SvcSi_SubtitleList_t));

		hErr = SVC_SI_GetSubtitleList(ulActionId, DbSvc_GetSvcId(&stSvcInfo), &stSubtitleList);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("SVC_SI_GetSubtitleList Error. hSvc(0x%08x)\n", hSvc);
		}
		else
		{
			hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_SUBTITLE_SETTING, (HUINT32 *)&bSubtitleEnable, 0);
			if((hErr != ERR_OK) || bSubtitleEnable == FALSE)
			{
				nCurrIndex = -1;
				DbSvc_SetSubttlIdx(&stSvcInfo, nCurrIndex);
			}
			else
			{
				if (DbSvc_GetSubttlIdx(&stSvcInfo) == 0xff)
				{
					nCurrIndex = -1;
				}
				else
				{
					nCurrIndex = DbSvc_GetSubttlIdx(&stSvcInfo);
				}
			}

			pstSubtitleCompTable->nArraySize		=	stSubtitleList.ulSubtitleNum;
			pstSubtitleCompTable->nMaxSize		=	sizeof(OxMediaPlay_SubtitleCompTable_t);
			pstSubtitleCompTable->nCurrIndex	=	nCurrIndex;

			for( i=0 ; i<stSubtitleList.ulSubtitleNum; i++ )
			{
				pstSubtitleCompTable->astCompArray[i].nPid			=	stSubtitleList.pstSubtitleInfo[i].usPid;
				pstSubtitleCompTable->astCompArray[i].eCompType =	eOxMP_COMPONENT_SUBTITLE;

				switch(stSubtitleList.pstSubtitleInfo[i].eType)
				{
					case eSI_SUBTITLE_DVB:
					{
						pstSubtitleCompTable->astCompArray[i].eSubtitleType =	eOxMP_SUBTITLETYPE_Dvb;
						pstSubtitleCompTable->astCompArray[i].utArg.stDvbInfo.nTypeValue			=	stSubtitleList.pstSubtitleInfo[i].utArg.stDvbInfo.ucType;
						pstSubtitleCompTable->astCompArray[i].utArg.stDvbInfo.nComponentTag 		=	stSubtitleList.pstSubtitleInfo[i].utArg.stDvbInfo.ucComponentTag;
						pstSubtitleCompTable->astCompArray[i].utArg.stDvbInfo.nCompositionPageId	=	stSubtitleList.pstSubtitleInfo[i].utArg.stDvbInfo.usCompositionPageId;
						pstSubtitleCompTable->astCompArray[i].utArg.stDvbInfo.nAncillaryPageId		=	stSubtitleList.pstSubtitleInfo[i].utArg.stDvbInfo.usAncillaryPageId;

						HLIB_STD_MemCpySafe((pstSubtitleCompTable->astCompArray[i].utArg.stDvbInfo.szLangCode), 4,
											(stSubtitleList.pstSubtitleInfo[i].utArg.stDvbInfo.aucLangCode), 4);
					}
					break;

					case eSI_SUBTITLE_EBU:
					{
						pstSubtitleCompTable->astCompArray[i].eSubtitleType =	eOxMP_SUBTITLETYPE_Ebu;
						pstSubtitleCompTable->astCompArray[i].utArg.stEbuInfo.nTypeValue		=	stSubtitleList.pstSubtitleInfo[i].utArg.stEbuInfo.ucType;
						pstSubtitleCompTable->astCompArray[i].utArg.stEbuInfo.nComponentTag 	=	stSubtitleList.pstSubtitleInfo[i].utArg.stEbuInfo.ucComponentTag;
						pstSubtitleCompTable->astCompArray[i].utArg.stEbuInfo.nMagazineNum		=	stSubtitleList.pstSubtitleInfo[i].utArg.stEbuInfo.ucMagazineNum;
						pstSubtitleCompTable->astCompArray[i].utArg.stEbuInfo.nPageNum			=	stSubtitleList.pstSubtitleInfo[i].utArg.stEbuInfo.ucPageNum;

						HLIB_STD_MemCpySafe((pstSubtitleCompTable->astCompArray[i].utArg.stEbuInfo.szLangCode), 4,
											(stSubtitleList.pstSubtitleInfo[i].utArg.stEbuInfo.aucLangCode), 4);
					}
					break;

					case eSI_SUBTITLE_SUPERCC:
					default:
						break;
				}
			}

			SVC_SI_FreeSubtitleList(&stSubtitleList);

			if(pstSubtitleCompTable->nCurrIndex != nOriCurrIndex )
			{
				HxLOG_Debug("eOxMP_COMPONENT_SUBTITLE = nCurrIndex(0x%x) \n", pstSubtitleCompTable->nCurrIndex);
				om_media_play_CbOnComponentSubtitleChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, pstSubtitleCompTable );
				om_media_play_CbOnComponentSelected(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, eOxMP_COMPONENT_SUBTITLE, pstSubtitleCompTable->nCurrIndex, -1);
			}
		}
	}

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_live_EvtSiNetworkChanged (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulViewId = 0;
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32					 ulSessionId = pstContext->ulSessionId;
	DxNcdNetworkInfo_t		 stNetworkMsg;
	Handle_t				 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	HxSTD_memset(&stNetworkMsg, 0x00, sizeof(DxNcdNetworkInfo_t));
	stNetworkMsg.eventType = eDxNCDEVT_GET_INFO;
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_ASO_NETWORK_MSG_INFO, (HUINT32 *)stNetworkMsg.szNcdnetworkMsg, (HUINT32)DxNCD_MAX_MESSAGE_LEN);
	if (hErr == ERR_OK)
	{
		om_media_play_CbOnSiNetworkChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, p1, p2, &stNetworkMsg);
	}

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_live_EvtSiNetworkUriChanged (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 	ulActionId	=	SVC_PIPE_GetActionId (hAction);
	HUINT32 	ulViewId;
	HUINT32 	ulSessionId 	=	pstContext->ulSessionId;
	HERROR	hErr 		= ERR_FAIL;
	DxNetworkUriInfo_t stNetworkUriMsg;
	DbSvc_NetInfo_t		stNetInfo;
	HUINT8				*pucNetworkUri;
	HUINT16				usNetIdx = (HUINT16)p1;

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	HxSTD_memset(&stNetworkUriMsg, 0x00, sizeof(DxNetworkUriInfo_t));
	hErr = DB_SVC_GetNetworkInfo (usNetIdx, &stNetInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("DB_SVC_GetNetworkInfo failed. netidx=(%d) \n",usNetIdx);
		return MESSAGE_PASS;
	}
	DB_SVC_GetNetworkUriPrefix(&stNetInfo,&pucNetworkUri);

	HLIB_STD_MemCpySafe(stNetworkUriMsg.szNetworkUriPrefix,DxMAX_URI_PREFIX_INFO_LEN,pucNetworkUri,DxMAX_URI_PREFIX_INFO_LEN);
	stNetworkUriMsg.szNetworkUriPrefix[DxMAX_URI_PREFIX_INFO_LEN-1] = '\0';
	stNetworkUriMsg.eventType = eDxNETWORKURIEVT_GET_INFO;

	om_media_play_CbOnSiNetworkUriChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, &stNetworkUriMsg);

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_live_EvtSiDrm (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32									 ulViewId = 0;
	HUINT32									 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32									 ulSessionId = pstContext->ulSessionId;
	DxCopyrightControl_t					 stSiCcInfo = {0, };
	MgrCopyrightControl_MediaControlInfo_t	 stDetailDrmInfo;
	Handle_t								 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	HLIB_STD_MemCpySafe(&(stSiCcInfo), sizeof(DxCopyrightControl_t), (void *)p3, sizeof(DxCopyrightControl_t));

	pstContext->stOmCache.stCcInfo.bSiCc = TRUE;
	HLIB_STD_MemCpySafe(&(pstContext->stOmCache.stCcInfo.stSiCcInfo), sizeof(DxCopyrightControl_SiCcInfo_t), &(stSiCcInfo.stSiCcInfo), sizeof(DxCopyrightControl_SiCcInfo_t));

	// SI DRM은 OP 요구사항 이므로 OP type에 따라서 선택하도록 하자.
	if(pstContext->stOmCache.stCcInfo.bCasCc)
	{
		switch(pstContext->stOmCache.stCcInfo.stSiCcInfo.eSiCcType)
		{
			case eDxCopyrightControl_SiCcType_Ses:
				// SES는 SI보다 CAS 우선순위이다. 그래서 이미 CAS DRM이 수신 된경우에는 Web으로 drm 정보를 올려주지 않는다.
				if(pstContext->stOmCache.stCcInfo.stCasCcInfo.eCasCcType == eDxCopyrightControl_CasCcType_NagraSes)
				{
					return MESSAGE_PASS;
				}
				break;
			case eDxCopyrightControl_SiCcType_Nordig:
				// TODO : OP 사양 확인
				break;
			default:
				HxLOG_Error("Check the SI DRM... Is the SI DRM should affected?\n");
				return MESSAGE_PASS;
		}
	}

	hErr = MGR_COPYRIGHTCONTROL_GetMediaControlInfo(TRUE, &(pstContext->stOmCache.stCcInfo), &stDetailDrmInfo);
	if(hErr == ERR_OK)
	{
		pstContext->stCache.uTrickRestrictMode = stDetailDrmInfo.eTrickModeCtrl;
#if defined (CONFIG_OP_SES)
		pstContext->stCache.eStorageRight = stDetailDrmInfo.eStorageRight;
		om_media_play_CbCcTrickRestrictModeChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, pstContext->stCache.uTrickRestrictMode, pstContext->stCache.eStorageRight);
#else
		om_media_play_CbCcTrickRestrictModeChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, pstContext->stCache.uTrickRestrictMode);
#endif
	}
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_live_EvtCasDrm (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32									 ulViewId = 0;
	HUINT32									 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32									 ulSessionId = pstContext->ulSessionId;
	DxCopyrightControl_t					 stCasCcInfo = {0, };
	MgrCopyrightControl_MediaControlInfo_t	 stDetailDrmInfo;
	Handle_t								 hErr;

	if (pstContext->ulActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

#if defined(CONFIG_OP_SES)
	// cas channel인경우에만 drm 정보를 올려준다.
	if(om_media_play_live_IsCasChannel(pstContext, hAction) != ERR_OK)
	{
		return MESSAGE_PASS;
	}
#endif

	HLIB_STD_MemCpySafe(&(stCasCcInfo), sizeof(DxCopyrightControl_t), (void *)p3, sizeof(DxCopyrightControl_t));

	// DRM(URI) 정보가 올라 올때 몇몇 CAS spec에서 요구하는 Trick Restrict를동작시키기 위한 내용으로 상관없는 type의 DRM은 적용되지 않도록 한다. Base에서는 필요한 CAS Type에 대해서만 처리하도록 수정
	// 이력: Nagra & SES spec을 맞추기 위해 iCord-Pro Branch에서 처음 적용된 부분이고, 이후 Base로 들어옴. CDIG-2000 모델에서 Conax spec을 처리하기 위해 이 부분을 사용함.
	switch(stCasCcInfo.stCasCcInfo.eCasCcType)
	{
		case eDxCopyrightControl_CasCcType_CiPlus:
			HxLOG_Info("URI of CI+ do not affect at om media play...\n");
			return MESSAGE_PASS;
		case eDxCopyrightControl_CasCcType_Conax:
		case eDxCopyrightControl_CasCcType_NagraSes:
			HxLOG_Info("DRM(URI) is going to be affected...\n");
			break;
		default:
			HxLOG_Error("Check the DRM... Is the DRM should affected?\n");
			return MESSAGE_PASS;
	}

	pstContext->stOmCache.stCcInfo.bCasCc = TRUE;
	HLIB_STD_MemCpySafe(&(pstContext->stOmCache.stCcInfo.stCasCcInfo), sizeof(DxCopyrightControl_CasCcInfo_t), &(stCasCcInfo.stCasCcInfo), sizeof(DxCopyrightControl_CasCcInfo_t));

	hErr = MGR_COPYRIGHTCONTROL_GetMediaControlInfo(TRUE, &(pstContext->stOmCache.stCcInfo), &stDetailDrmInfo);
	if(hErr == ERR_OK)
	{
		pstContext->stCache.uTrickRestrictMode = stDetailDrmInfo.eTrickModeCtrl;
#if defined(CONFIG_OP_SES)
		pstContext->stCache.eStorageRight = stDetailDrmInfo.eStorageRight;
		om_media_play_CbCcTrickRestrictModeChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, pstContext->stCache.uTrickRestrictMode, pstContext->stCache.eStorageRight);
#else
		om_media_play_CbCcTrickRestrictModeChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, pstContext->stCache.uTrickRestrictMode);
#endif
	}
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_live_FccTuneStatus (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulTuneMsg = (HUINT32)p1;

	switch (ulTuneMsg)
	{
		case eSEVT_CH_LOCKED:
			om_media_play_live_EvtChLocked(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CH_NO_SIGNAL:
			om_media_play_live_EvtChNoSignal(pstContext, hAction, p1, p2, p3);
			break;

		default:
			// do nothing
			break;
	}

	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_live_HbbtvOnHdplusRedbutton (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32	ulActionId 		= SVC_PIPE_GetActionId (hAction);
	HUINT32	ulSessionId 	= pstContext->ulSessionId;
	HUINT32	ulViewId		= ulActionId - eActionId_VIEW_FIRST;

	om_media_play_CbHbbtvOnHdplusRedbutton (ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, p3);

	return MESSAGE_BREAK;
}

#define _____OAPI_COMMAND_FUNCTIONS_____
STATIC HERROR om_media_play_live_CbSetTSREnable (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HUINT32					 ulSessionId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
#if defined(CONFIG_MW_MM_PVR)
	HERROR					 hErr = ERR_FAIL;
	HUINT32					 ulViewId 		= (HUINT32)HxOBJECT_INT (apArgv[1]);
	HBOOL 					 bEnable		= (HBOOL)HxOBJECT_INT (apArgv[2]);
#endif

#if defined(CONFIG_MW_MM_PVR)
	hErr = MGR_PVR_SetTsrAutoTracking(ulViewId, bEnable);
	if( ERR_OK != hErr )
	{
		HxLOG_Error("MGR_PVR_SetTsrAcceptance Error. - viewId(%d)\n", ulViewId);
	}

	if (ERR_OK == hErr)
	{
		/*EMIT CHANGED EVENT*/
	}
#endif
	(void)(ulSessionId);

	return ERR_OK;
}

STATIC HERROR om_media_play_live_CbCheckAitSection (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
#if defined(CONFIG_MW_SI_AIT)
	HERROR					hErr			= ERR_FAIL;
	omMediaPlay_Context_t	*pstContext		= NULL;
//	HUINT32 				ulSessionId		= (HUINT32)HxOBJECT_INT (apArgv[0]);
	HUINT32 				ulViewId		= (HUINT32)HxOBJECT_INT (apArgv[1]);
	HUINT32 				ulActionId;
	Handle_t				hAction			= HANDLE_NULL;

	hErr = MGR_ACTION_GetActionIdByViewId(ulViewId, &ulActionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_ACTION_GetActionIdByViewId err: ViewId(%d)\n", ulViewId);
		return ERR_FAIL;
	}

	hAction = SVC_PIPE_GetActionHandle(ulActionId);

	pstContext = om_media_play_GetContext (ulActionId);
	if(pstContext)
	{
		if(pstContext->stCache.stAitSecData.ulAitSecCount > 0 && pstContext->stCache.stAitSecData.eAitState == eAitState_Received)
		{
			HUINT32			 ulHbbtvActId;
			Handle_t		 hHbbtvAction;

			// 받은적 있다면, 다시 한번 위로...
			hErr = MGR_HBBTV_GetActionId(&ulHbbtvActId);
			if (ERR_OK == hErr)
			{
				hHbbtvAction = SVC_PIPE_GetActionHandle(ulHbbtvActId);

				hErr = om_media_play_AitChanged(pstContext, hAction, eMgrHbbtvAitState_Exist, hHbbtvAction, pstContext->stCache.stAitSecData.usCurrAitPid);
				if (ERR_OK != hErr)
				{
					HxLOG_Error("om_media_play_live_CbOnSiAITChanged err: hHbbtvAction(0x%08x)\n", hHbbtvAction);
				}
			}
			else
			{
				HxLOG_Error("MGR_HBBTV_GetActionId err:\n");
			}
		}
	}

	return hErr;
#else
	return ERR_OK;
#endif
}


#define ___MEMBER_FUNCTIONS___
BUS_Result_t proc_om_media_play_live_Proto (omMediaPlay_Context_t *pstContext, HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			 eResult;
	switch (message)
	{
	case eMEVT_LIVE_NOTIFY_SVC_CHANGED:
		eResult = om_media_play_live_MsgWtvSvcChanged (pstContext, hHandle, p1, p2, p3);
		eResult = om_media_play_live_UpdateConflictInfoChanged (pstContext, hHandle, p1, p2, p3);
		break;

	case eMEVT_LIVE_NOTIFY_SVC_STOPPED:
		eResult = om_media_play_live_MsgWtvStop (pstContext, hHandle, p1, p2, p3);
		break;

	case eMEVT_LIVE_NOTIFY_VIEW_STATE_CHANGED:
		eResult = om_media_play_live_MsgLiveViewState(pstContext, hHandle, p1, p2, p3);
		break;

	case eMEVT_LIVE_NOTIFY_LOCK_STATE_CHANGED:
		eResult = om_media_play_live_MsgLiveLockState(pstContext, hHandle, p1, p2, p3);
		break;

	case eMEVT_LIVE_NOTIFY_EVENT_RELAY:
		eResult = om_media_play_live_MsgEventRelay(pstContext, hHandle, p1, p2, p3);
		break;

	case eMEVT_LIVE_NOTIFY_EVENT_EWS:
		eResult = om_media_play_live_MsgEventEws(pstContext, hHandle, p1, p2, p3);
		break;

	case eMEVT_SVCLIST_RADIOBG_CHANGED:
		eResult = om_media_play_live_MsgSvclistRadioBgChanged(pstContext, hHandle, p1, p2, p3);
		break;

	case eMEVT_LIVE_NOTIFY_EVENT_RATING_CTRL_CHANGED:
		eResult = om_media_play_live_MsgRatingCtrlChanged(pstContext, hHandle, p1, p2, p3);
		break;

#if 0 //defined(CONFIG_MW_SI_AIT)
	case eMEVT_HBBTV_NOTIFY_AIT:
		eResult = om_media_play_live_MsgHbbtvNotifyAit(pstContext, hHandle, p1, p2, p3);
		break;
#endif

	case eSEVT_CH_LOCKED:
		eResult = om_media_play_live_EvtChLocked (pstContext, hHandle, p1, p2, p3);
		break;

	case eSEVT_CH_NO_SIGNAL:
		eResult = om_media_play_live_EvtChNoSignal (pstContext, hHandle, p1, p2, p3);
		break;

	case eSEVT_CH_ANTENNA_OK:
		eResult = om_media_play_live_EvtChAntennaOk (pstContext, hHandle, p1, p2, p3);
		break;

	case eSEVT_CH_ANTENNA_FAULT:
		eResult = om_media_play_live_EvtChAntennaNok (pstContext, hHandle, p1, p2, p3);
		break;

	case eMEVT_OUTPUT_AV_STARTED:
		HxLOG_Trace();
		eResult = om_media_play_live_MsgOutputAVStarted (pstContext, hHandle, p1, p2, p3);
		break;

	case eMEVT_OUTPUT_AV_STOPPED:
		HxLOG_Trace();
		eResult = om_media_play_live_MsgOutputAVStoped (pstContext, hHandle, p1, p2, p3);
		break;

	case eSEVT_AV_VIDEO_STARTED:
		eResult = om_media_play_live_EvtAvVideoStarted (pstContext, hHandle, p1, p2, p3);
		break;

	case eSEVT_AV_VIDEO_UNDERRUN:
		eResult = om_media_play_live_EvtAvVideoUnderrun (pstContext, hHandle, p1, p2, p3);
		break;

	case eSEVT_AV_AUDIO_CLIP_COMPLETED:
		eResult = om_media_play_live_EvtAvAudioClipCompleted (pstContext, hHandle, p1, p2, p3);
		break;

	case eSEVT_SI_PMT:
		eResult = om_media_play_live_EvtSiPmt (pstContext, hHandle, p1, p2, p3);
		break;

	case eSEVT_SI_EIT:
		eResult = om_media_play_live_EvtSiEit (pstContext, hHandle, p1, p2, p3);
		break;

	case eSEVT_SI_RCT:
		eResult = om_media_play_live_EvtSiRct (pstContext, hHandle, p1, p2, p3);
		break;

	case eSEVT_SI_RCT_TIMEOUT:
		eResult = om_media_play_live_EvtSiRctTimeout (pstContext, hHandle, p1, p2, p3);
		break;

	case eSEVT_SI_RCT_NONE:
		eResult = om_media_play_live_EvtSiRctNone (pstContext, hHandle, p1, p2, p3);
		break;

	case eSEVT_SI_REQUEST_RCT_IMAGE_BY_MHEG:
		eResult = om_media_play_live_EvtRequestRctImageIcon (pstContext, hHandle, p1, p2, p3);
		break;

	case eSEVT_AV_MEDIA_RECT_CHANGED:
		eResult = om_media_play_live_EvtAvMediaRectChanged (pstContext, hHandle, p1, p2, p3);
		break;

	case eMEVT_SVCLIST_SVC_PID_CHANGED:
		eResult = om_media_play_live_SvcPidChanged (pstContext, hHandle, p1, p2, p3);
		break;

	case eSEVT_SI_NETWORK_CHANGED_NOTIFY:
		eResult = om_media_play_live_EvtSiNetworkChanged (pstContext, hHandle, p1, p2, p3);
		break;

	case eSEVT_SI_NETWORK_URI_PREFIX_CHANGED:
		eResult = om_media_play_live_EvtSiNetworkUriChanged (pstContext, hHandle, p1, p2, p3);
		break;

	case eSEVT_SI_DRM_SIGNALLED:
		eResult = om_media_play_live_EvtSiDrm (pstContext, hHandle, p1, p2, p3);
		break;

	case eSEVT_CAS_DRM_SIGNALLED:
		eResult = om_media_play_live_EvtCasDrm (pstContext, hHandle, p1, p2, p3);
		break;

	case eMEVT_LIVE_NOTIFY_FCC_TUNE_STATUS:
		eResult = om_media_play_live_FccTuneStatus (pstContext, hHandle, p1, p2, p3);
		break;
	case eMEVT_LIVE_SET_HBBTV_ON_HDPLUS_REDBUTTON:
		eResult = om_media_play_live_HbbtvOnHdplusRedbutton (pstContext, hHandle, p1, p2, p3);
		break;
	default:
		eResult = MESSAGE_PASS;
		break;
	}

	return (MESSAGE_BREAK == eResult) ? MESSAGE_BREAK : MESSAGE_PASS;
}

HERROR om_media_play_live_Init(void)
{
	OxMediaPlay_ConflictInfo_t stLiveConfInfo	=	{0,};
	HERROR	hErr;
	HUINT32	ulActionId = MGR_ACTION_GetMainActionId();

	hErr = om_media_play_live_getCurrentResourceStatus (ulActionId, eAmPriority_VIEW_ACTION, &stLiveConfInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("om_media_play_live_getCurrentResourceStatus err: actionId(%d)\n", ulActionId);
		return ERR_FAIL;
	}

	{
		HINT32 i;
		for( i=0 ; i<NUM_VIEW_ACTION ; i++ )
		{
			omMediaPlay_Context_t *pstContext = om_media_play_GetContext (i);
			if( NULL != pstContext )
			{
				HxSTD_MemCopy ( &(pstContext->stCache.stLiveConflictInfo), &stLiveConfInfo, sizeof(OxMediaPlay_ConflictInfo_t) );
			}
		}
	}
#if defined(CONFIG_DEBUG)
	om_media_play_live_printResourceStatus(&stLiveConfInfo);
#endif
	return ERR_OK;
}

// 이 함수내에서 ulActionId는 사실상 viewId이다
HERROR om_media_play_live_Start (HUINT32 ulActionId,
#if defined(CONFIG_SUPPORT_FCC)
											HUINT32 				ulSessionId,
											HUINT32					*pulTakenActionId,
#endif
											OxMediaPlay_StartLive_t *pstStart)
{
	HUINT32				 ulTakenActionId = (HUINT32)-1;
	MgrLive_START_t		 stStartArg;
	omMediaPlay_Context_t		*pstContext = om_media_play_GetContext (ulActionId);
	HERROR				 hErr;
	MgrLiveView_SessionType_t eSessionType;

	if (NULL == pstContext)					{ return ERR_FAIL; }

	HxSTD_MemSet(&stStartArg, 0x00, sizeof(MgrLive_START_t));


	switch(pstStart->eLiveType)
	{
		case eOxMP_LIVETYPE_MAIN:	stStartArg.eLiveMode = eMgrLiveMode_MainLive;		break;
		case eOxMP_LIVETYPE_PIP:	stStartArg.eLiveMode = eMgrLiveMode_PipLive;		break;
#if defined(CONFIG_SUPPORT_FCC)
		case eOxMP_LIVETYPE_FCC:	stStartArg.eLiveMode = eMgrLiveMode_FccLive;		break;
#endif
		default:
			HxLOG_Error("invalid eLiveType(%d)\n", pstStart->eLiveType);
			return ERR_FAIL;
	}

	switch(pstStart->eSuppSvcType)
	{
		case eOxMP_SUPPLESVCTYPE_Normal:		eSessionType = eLiveViewSession_NORMAL;		break;
		case eOxMP_SUPPLESVCTYPE_SilentTune:	eSessionType = eLiveViewSession_MHEG_QUIET;	break;
		case eOxMP_SUPPLESVCTYPE_Replace:		eSessionType = eLiveViewSession_REPLACE;	break;
		case eOxMP_SUPPLESVCTYPE_Subfeed:		eSessionType = eLiveViewSession_SUBFEED;	break;
		case eOxMP_SUPPLESVCTYPE_Tsmf_Low:		eSessionType = eLiveViewSession_TSMF_LOW;	break;

		default:
			eSessionType = eLiveViewSession_NORMAL;		break;
	}

	hErr  = DB_SVC_GetServiceHandleBySvcIdx ((HUINT16)pstStart->ulMasterSvcUid, &(stStartArg.hMasterSvc));
	hErr |= DB_SVC_GetServiceHandleBySvcIdx ((HUINT16)pstStart->ulSuppleSvcUid, &(stStartArg.hService));

	stStartArg.unExtArg.stMainLiveArg.ulMajorCHNum = pstStart->ulMajorCHNum;

	if (ERR_OK != hErr)
	{
		HxLOG_Error("Argument Wrong: MasterUid(%d), SuppleUid(%d)\n", pstStart->ulMasterSvcUid, pstStart->ulSuppleSvcUid);
		return ERR_FAIL;
	}

	stStartArg.unExtArg.stMainLiveArg.eTuneMode  = (MgrLive_TuningMode_t)pstStart->eTuneMode;
	
#if defined(CONFIG_SUPPORT_FCC)
	// eMediaType과 sessionId를 여기서 context에 넣어준다
	// MGR_LIVE_Start() 에서 live start하면서 svc changed 같은 메시지들이 oapi로 올라가는데
	// 여기서 미리 해주지 않으면 context에 있는 old session id를 사용하는 문제가 생긴다..!
	if ((pstStart->eLiveType == eOxMP_LIVETYPE_MAIN) || (pstStart->eLiveType == eOxMP_LIVETYPE_FCC))
	{
		HUINT32						ulNewActionId = MGR_ACTION_GetMainActionId();
		omMediaPlay_Context_t		*pstContext = NULL;

		hErr = MGR_LIVE_FindFccActionIdBySvc(stStartArg.hService, &ulNewActionId);
		if (hErr != ERR_OK)
		{
			// fcc context중에 동일한 hSvc를 가진 것이 없다..

			if (pstStart->eLiveType == eOxMP_LIVETYPE_MAIN)
			{
				// *** Main Live
				// normal zapping을 해야 하니 현재 main actionid가 next main action id이다
				// ERR_FAIL이라면 현재의 main action id를 가져다 사용한다
				// ERR_OK라면 fcc context에서 동일한 hSvc를 가진 것을 찾은 것이니 얻어온 action id를  main action id로 사용한다
				ulNewActionId = MGR_ACTION_GetMainActionId();
			}
			else
			{
				// *** Fcc Live
				// fcc에 사용할 action id를 새로 가져오자
				ulNewActionId = MGR_LIVE_GetFccActionId();
			}
		}

		pstContext = om_media_play_GetContext (ulNewActionId);
		if (pstContext != NULL)
		{
			pstContext->eMediaType = eOxMP_MEDIATYPE_LIVE;
			pstContext->ulSessionId = ulSessionId;

			*pulTakenActionId = ulNewActionId;
		}
		else
		{
			HxLOG_Critical("om_media_play_GetContext() failed.. actionId(%d) \n", ulNewActionId);
		}
	}
#endif

	hErr = MGR_LIVE_Start (&stStartArg, &ulTakenActionId);

	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_LIVE_Start err: ulActionId(%d)\n", ulActionId);
		return ERR_FAIL;
	}

	NOT_USED_PARAM(eSessionType);

	return ERR_OK;
}

HERROR om_media_play_live_Stop (HUINT32 ulActionId)
{
	omMediaPlay_Context_t		*pstContext = om_media_play_GetContext (ulActionId);
	HERROR				 hErr;

	if (NULL == pstContext)					{ return ERR_FAIL; }

	hErr = MGR_LIVE_Stop (ulActionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_LIVE_Stop err: ActionId(%d)\n");
	}

	return ERR_OK;
}

HERROR om_media_play_live_SetComponentIndex (HUINT32 ulActionId, OxMediaPlay_Component_e eCompType, HINT32 nCompIndex, HUINT8 *pstrLang)
{
	omMediaPlay_Context_t		*pstContext = om_media_play_GetContext (ulActionId);
	HERROR				 hErr;

	HxLOG_Debug("ulActionId [%d], eCompType [%d], nCompIndex [%d]\n", ulActionId, eCompType, nCompIndex);
	if (NULL == pstContext)					{ return ERR_FAIL; }

	switch(eCompType)
	{
		case eOxMP_COMPONENT_VIDEO:
		case eOxMP_COMPONENT_AUDIO:
		{
			if( -1 == nCompIndex ) // unselect!!!
			{
				HBOOL bAudio=FALSE, bVideo=FALSE;

				if( eOxMP_COMPONENT_VIDEO == eCompType )	bVideo = TRUE;
				if( eOxMP_COMPONENT_AUDIO == eCompType )	bAudio = TRUE;

				BUS_SendMessage (NULL, eMEVT_LIVE_SET_UNSELECT_COMPONENT,
									SVC_PIPE_GetActionHandle(ulActionId),
									(HINT32)bVideo, 	//<< unselect video
									(HINT32)bAudio, 	//<< unselect audio
									0);

				return ERR_OK;
			}

			HxLOG_Debug("nCompIndex (%d : %d) / nArraySize : (%d)\n", nCompIndex, pstContext->stCache.stAudioCompTable.nCurrIndex, pstContext->stCache.stAudioCompTable.nArraySize);

			if( pstContext->stCache.stAudioCompTable.nArraySize <= nCompIndex )
				return ERR_FAIL;

			if( pstContext->stCache.stAudioCompTable.nCurrIndex != nCompIndex )
			{
				HBOOL			bUpdateSvc	= FALSE;
				Handle_t		hSvc;
				DbSvc_Info_t	stSvcInfo;
				OxMediaPlay_AudioComp_t *pstAudioInfo = &(pstContext->stCache.stAudioCompTable.astCompArray[nCompIndex]);

				HxLOG_Debug("Audio Selected : [%d]\n", nCompIndex);
				pstContext->stCache.stAudioCompTable.nCurrIndex = nCompIndex;

				hErr  =	 DB_SVC_GetServiceHandleBySvcIdx ((HUINT16)pstContext->stCache.stLive.ulMasterSvcUid, &hSvc);
				hErr  != DB_SVC_GetServiceInfo (hSvc, &stSvcInfo);
				if (ERR_OK != hErr)
				{
					HxLOG_Error("OxDB_SVC_GetServiceInfo Error :\n");
					return ERR_FAIL;
				}

				HxLOG_Debug("bIsDolby [%p:%p] pid [%p:%p] eCodec [%p:%p]\n",
					pstAudioInfo->bIsDolby, DbSvc_GetAudioCodec(&stSvcInfo), pstAudioInfo->nPid, DbSvc_GetAudioPid(&stSvcInfo), pstAudioInfo->eCodec, DbSvc_GetAudioCodec(&stSvcInfo));

				if (pstAudioInfo->bIsDolby != DbSvc_GetDolbyFlag(&stSvcInfo))
				{
					DbSvc_SetAudUserSelected(&stSvcInfo, TRUE);
					DbSvc_SetDolbyFlag(&stSvcInfo, pstAudioInfo->bIsDolby);
					bUpdateSvc = TRUE;
				}

				if (pstAudioInfo->nPid != DbSvc_GetAudioPid(&stSvcInfo))
				{
					DbSvc_SetAudUserSelected(&stSvcInfo, TRUE);
					DbSvc_SetAudioPid(&stSvcInfo, pstAudioInfo->nPid);
					bUpdateSvc = TRUE;
				}

				if (pstAudioInfo->eCodec != DbSvc_GetAudioCodec(&stSvcInfo))
				{
					DbSvc_SetAudUserSelected(&stSvcInfo, TRUE);
 					DbSvc_SetAudioCodec(&stSvcInfo, pstAudioInfo->eCodec);
 					bUpdateSvc = TRUE;
				}
/*
				if (NULL != pstrLang)
				{
					if (HLIB_STD_StrCaseCmp(pstrLang, DbSvc_GetAudioLanguage(&stSvcInfo)) == 0)
					{
						DbSvc_SetAudUserSelected(&stSvcInfo, TRUE);
						DbSvc_SetAudioLanguage(&stSvcInfo, pstrLang);
						bUpdateSvc = TRUE;
					}
				}
*/
				if (TRUE == bUpdateSvc)
				{
					if (DbSvc_GetDolbyFlag(&stSvcInfo) == TRUE) // MOON_aud
					{
						if ( (DbSvc_GetAudioCodec(&stSvcInfo) == eDxAUDIO_CODEC_DOLBY_AC3)
							|| (DbSvc_GetAudioCodec(&stSvcInfo) == eDxAUDIO_CODEC_DOLBY_AC3P) )
						{
							if (DbSvc_GetAudioPid(&stSvcInfo) != DbSvc_GetDolbyPid(&stSvcInfo))
								 DbSvc_SetDolbyPid(&stSvcInfo, DbSvc_GetAudioPid(&stSvcInfo));
						}
					}

					HxLOG_Debug("Audio selected [%d]\n", pstContext->stCache.stAudioCompTable.nCurrIndex);

					DbSvc_SetAudioLanguage(&stSvcInfo, pstAudioInfo->astLangInfo[0].szLangCode);
					DB_SVC_UpdateServiceInfo (hSvc, &stSvcInfo);
					MGR_LIVE_SetAudioIndex(ulActionId, nCompIndex);
					BUS_SendMessage (NULL, eMEVT_SVCLIST_SVC_PID_CHANGED, SVC_PIPE_GetActionHandle (pstContext->ulActionId), 0, 0, 0);
					BUS_PostMessage (NULL, eSEVT_DB_NOTIFY_UPDATED, HANDLE_NULL, 0, 0, 0);
				}
			}

			break;
		}
		case eOxMP_COMPONENT_SUBTITLE:
			{
				HBOOL			bUpdate = FALSE;
				Handle_t		hSvc;
				DbSvc_Info_t	stSvcInfo;
				OxMediaPlay_SubtitleComp_t *pstSubtitleInfo = NULL;

				HxLOG_Debug("Subtitle Selected : [%d]\n", nCompIndex);

				hErr  =	 DB_SVC_GetServiceHandleBySvcIdx ((HUINT16)pstContext->stCache.stLive.ulMasterSvcUid, &hSvc);
				hErr  |= DB_SVC_GetServiceInfo (hSvc, &stSvcInfo);
				if (ERR_OK != hErr)
				{
					HxLOG_Error("OxDB_SVC_GetServiceInfo Error :\n");
					return ERR_FAIL;
				}

				/* subtitle OFF track :
				 *	- flag is TRUE
				 *	- index is 255 (0xff)
				 */
				if (nCompIndex == -1)
				{
					pstContext->stCache.stSubtitleCompTable.nCurrIndex = nCompIndex;
					DbSvc_SetSubttlIdx(&stSvcInfo, SUBTITLE_OFF_IDX);
					DbSvc_SetSubttlFlag(&stSvcInfo, TRUE);
					DbSvc_SetSubttlLanguage(&stSvcInfo, "");
					bUpdate = TRUE;
				}
				else if ((nCompIndex != pstContext->stCache.stSubtitleCompTable.nCurrIndex) && (nCompIndex < pstContext->stCache.stSubtitleCompTable.nArraySize))
				{
					HCHAR *pszLangCode = "";

					if (nCompIndex < -1)
					{
						HxLOG_Error("nCompIndex(%d) Error!\n",nCompIndex);
						return ERR_FAIL;
					}

					pstSubtitleInfo = &(pstContext->stCache.stSubtitleCompTable.astCompArray[nCompIndex]);

					switch (pstSubtitleInfo->eSubtitleType)
					{
						case eOxMP_SUBTITLETYPE_Dvb:
							pszLangCode = pstSubtitleInfo->utArg.stDvbInfo.szLangCode;
							break;

						case eOxMP_SUBTITLETYPE_Ebu:
							pszLangCode = pstSubtitleInfo->utArg.stEbuInfo.szLangCode;
							break;

						case eOxMP_SUBTITLETYPE_Media:
							pszLangCode = pstSubtitleInfo->utArg.stMediaInfo.szLangCode;
							break;

						case eOxMP_SUBTITLETYPE_Unknown:
							pszLangCode = "";
							break;
					}

					pstContext->stCache.stSubtitleCompTable.nCurrIndex = nCompIndex;
					DbSvc_SetSubttlIdx(&stSvcInfo, nCompIndex);
					DbSvc_SetSubttlFlag(&stSvcInfo, TRUE);
					DbSvc_SetSubttlLanguage(&stSvcInfo, pszLangCode);
					bUpdate = TRUE;
				}
				else
				{
					return ERR_FAIL;
				}

				if (bUpdate)
				{
					HxLOG_Debug("subtitle changed : (%s)[%d]\n", DbSvc_GetName(&stSvcInfo), DbSvc_GetSubttlIdx(&stSvcInfo));
					DB_SVC_UpdateServiceInfo (hSvc, &stSvcInfo);
					BUS_SendMessage (NULL, eMEVT_SVCLIST_SVC_PID_CHANGED, SVC_PIPE_GetActionHandle (pstContext->ulActionId), 0, 0, 0);
					BUS_PostMessage (NULL, eSEVT_DB_NOTIFY_UPDATED, HANDLE_NULL, 0, 0, 0);
				}
			}
			break;

		case eOxMP_COMPONENT_TELETEXT:
		case eOxMP_COMPONENT_CLOSEDCAPTION:
		case eOxMP_COMPONENT_DATA:
		default:
			break;
	}

	return ERR_OK;
}

HERROR om_media_play_live_SetPlaySpeed (HUINT32 ulActionId, HINT32 nSpeed)
{
	return ERR_OK;
}

HERROR om_media_play_live_SetPlayPosition (HUINT32 ulActionId, HINT32 ulPosition)
{
	return ERR_OK;
}

HERROR om_media_play_live_SetSubtitleEnable (HUINT32 ulActionId, HBOOL bEnable)
{
	/* 추후 index 를 통한 off가 아닌 본 API 통하여 하도록 한다.*/
	//STATIC HERROR mgr_appmgr_mctrlusrpb_setSubtitleInfo (_mctrlUsrPbContext_t *pstContext, MEDIADEC_SUBTITLE_t	*pstSubtitle)
	return ERR_OK;
}

HERROR om_media_play_live_SetVideoHideEnable (HUINT32 ulActionId, HBOOL bEnable)
{
	HERROR hErr = ERR_FAIL;

	hErr = MGR_LIVE_SetVideoHideByAppkit (ulActionId, bEnable);
	if( ERR_OK != hErr )
	{
		HxLOG_Error("MGR_LIVE_SetVideoHideByAppkit Error.\n");
		return hErr;
	}

	return ERR_OK;
}

BUS_Result_t om_media_play_live_UpdateConflictInfoChanged (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32						 ulViewId = 0;
	HUINT32						 ulActionId = pstContext->ulActionId;
	HUINT32						 ulSessionId = pstContext->ulSessionId;
	OxMediaPlay_ConflictInfo_t	*pstLiveConfInfo = NULL;
	HERROR						 hErr;

	if (ulActionId >= NUM_MAX_ACTION)
	{
		goto END_FUNC;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		goto END_FUNC;
	}

	pstLiveConfInfo = (OxMediaPlay_ConflictInfo_t *)OxAP_Malloc(sizeof(OxMediaPlay_ConflictInfo_t));
	if (NULL == pstLiveConfInfo)
	{
		HxLOG_Error("Memory allocation failed:\n");
		goto END_FUNC;
	}

	hErr = om_media_play_live_getCurrentResourceStatus (ulActionId, eAmPriority_VIEW_ACTION, pstLiveConfInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("om_media_play_live_getCurrentResourceStatus err: actionId(%d)\n", ulActionId);
		goto END_FUNC;
	}

	if (HxSTD_MemCmp (&(pstContext->stCache.stLiveConflictInfo), pstLiveConfInfo, sizeof(OxMediaPlay_ConflictInfo_t)) != 0)
	{
		HxSTD_MemCopy (&(pstContext->stCache.stLiveConflictInfo), pstLiveConfInfo, sizeof(OxMediaPlay_ConflictInfo_t));
		om_media_play_live_CbOnConflictChanged( ulViewId, ulSessionId, pstLiveConfInfo );
#if defined(CONFIG_DEBUG)
		om_media_play_live_printResourceStatus(pstLiveConfInfo);
#endif
	}

#if defined(CONFIG_OP_SES)
	HxSTD_MemSet(&pstContext->stOmCache.stCcInfo, 0, sizeof(DxCopyrightControl_t));
#endif

END_FUNC:
	if (NULL != pstLiveConfInfo)					{ OxAP_Free (pstLiveConfInfo); }

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

#if defined(CONFIG_3RD_MHEG)
void om_media_play_live_SetMhegInterruptibleFlag (HBOOL bMhegInterruptibleFlag)
{
	s_bMhegInterruptibleFlag_InOm = bMhegInterruptibleFlag;
	return ;
}
#endif

#define _____RPC_INITIALIZATION_____
HERROR om_media_play_live_InitRpcFunctions (HINT32 nRpcHandle)
{
	HLIB_RPC_RegisterCall (nRpcHandle,RPC_OAPI_MEDIA_PLAY_SetTSREnable,		"iii",	om_media_play_live_CbSetTSREnable, NULL,"SetTsrEnable: (HUINT32 ulViewId, HBOOL bEnable)");
	HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_MEDIA_PLAY_CheckAitSection,	"ii",	om_media_play_live_CbCheckAitSection , NULL, "CheckAitSection: (HUINT32 ulViewId)");

	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnLocked					,"iiii", "LIVE Locked. 			\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, Handle_t hSvc)\n");
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnNoSignal 				,"iiii", "LIVE NoSignal.			\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, Handle_t hSvc)\n");
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAntennaOk				,"iiii", "LIVE AntennaOk. 		\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, Handle_t hSvc)\n");
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAntennaNok 			,"iiii", "LIVE AntennaNok.		\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, Handle_t hSvc)\n");
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSvcChanged				,"iiii", "LIVE SvcChanged. 		\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, Handle_t hSvc)\n");
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSvcStopped				,"iiii", "LIVE SvcStopped. 		\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, Handle_t hSvc)\n");
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnConflictChanged			,"iiib", "LIVE Conflict.			\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_ConflictInfo_t *pstLiveConflictInfo)\n");
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnEventRelay				,"iiii", "LIVE EventRelay. 		\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId,  HINT32 nEvtRealyHandle)\n");
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnEws					,"iiii", "LIVE Ews.				\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId,  HINT32 nEwssvcuid, HINT32 nEwscursvcuid)\n");
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnBsCtrlStarted				,"iiii", "LIVE BsCtrlStarted.		\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId,  Handle_t hMasterSvc)\n");
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPinCtrlChangedByBCAS 	,"iiii", "LIVE PinCtrlChByBCAS.	\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId)\n");
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnViewStateChanged 		,"iiii", "LIVE ViewState.			\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId,  OxMediaPlay_LiveViewState_e eViewState)\n");
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnLockStateChanged 		,"iiii", "LIVE LockState.			\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId,  OxMediaPlay_LiveLockState_e eLockState)\n");

#if defined(CONFIG_3RD_MHEG)
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnMhegPmtChanged 		,"iiibbbi", "LIVE Pat/Pmt raw section.	\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId,  OxMediaPlay_MhegSectionData_t *stMhegSecData, HUINT8 *pucPatRawSection, HUINT8 *pucPmtRawSection, OxMediaPlay_MhegPmtStatus_e	eMhegPmtState)\n");
#endif
#if defined(CONFIG_3RD_MHEG_SI_MONITORING)
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnMhegNDTChanged		,"iiii", "LIVE NDTChanged. 		\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_TuningMode_e eTuneMode)\n");
#endif

	s_nRpcHandle = nRpcHandle;

	return ERR_OK;
}

#define _____DEBUG_FUNCTION_____
#if 0
void om_media_play_live_PrintContext(void)
{
	HUINT32 				i;
	omMediaPlay_Context_t	*pstTmpContext = NULL;
	Handle_t				hTmpSvc = HANDLE_NULL;
	DbSvc_Info_t			stTmpSvcInfo;
	HERROR					hErr;

	for (i=eActionId_VIEW_FIRST; i<= eActionId_VIEW_LAST; i++)
	{
		pstTmpContext = om_media_play_GetContext(i);
		if (pstTmpContext == NULL)
		{
			HxLOG_Error("idx(%d) NULL context", i);
		}
		else
		{
			HUINT32 ulVid = 0xff;

			MGR_ACTION_GetViewIdByActionId(pstTmpContext->ulActionId, &ulVid);

			hErr = DB_SVC_GetServiceHandleBySvcIdx ((HUINT16)pstTmpContext->stCache.stLive.ulMasterSvcUid, &hTmpSvc);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("DB_SVC_GetServiceHandleBySvcIdx() failed..!! idx(%d) ulMasterSvcUid(%d) \n", i, pstTmpContext->stCache.stLive.ulMasterSvcUid);
				continue;
			}

			hErr = DB_SVC_GetServiceInfo (hTmpSvc, &stTmpSvcInfo);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("Error, SvcHandle(0x%08x)\n", hTmpSvc);
				continue;
			}

			HxLOG_Error("idx(%d) aid(%d)->vid(%d) hSvc(0x%X) svcName(%s)",
					i,
					pstTmpContext->ulActionId,
					ulVid,
					hTmpSvc,
					stTmpSvcInfo.szSvcName);
		}

		HxLOG_Error("-------------------------------------\n");
	}

	HxLOG_Error("=========================================\n\n\n");
}
#endif

/* end of file */
