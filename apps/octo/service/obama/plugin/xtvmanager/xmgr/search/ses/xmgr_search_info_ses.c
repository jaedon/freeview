/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xmgr_search_info_ses.c
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
#include <octo_common.h>


#include <mgr_action.h>
#include <bus.h>
#include <svc_pipe.h>
#include <db_common.h>
#include <db_svc.h>
#include <db_param.h>
#include <linkedlist.h>
#include <svc_si.h>
#include <svc_sys.h>
#include <svc_epg.h>

#include <isosvc.h>

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_antenna.h>
#include <mgr_search.h>
#include <mgr_spec.h>

#include <svc_resources.h>
#include <mgr_rscmap.h>
#include <xmgr_search.h>
#include <xmgr_module_search.h>
#include "../local_include/_xmgr_search.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define MAX_ANCHOR_TP			32


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

static DbSvc_SatTuningInfo_t	 s_stSesAnchorTuneInfo[MAX_ANCHOR_TP] =
{
/*1*/	{	12603,	22000,	eDxSAT_POLAR_HOR,	eDxSAT_CODERATE_5_6,	eDxSAT_TRANS_DVBS,	eDxSAT_PSK_QPSK,	0,	0,	0, 	0},
/*2*/	{	12552,	22000,	eDxSAT_POLAR_VER,	eDxSAT_CODERATE_5_6,	eDxSAT_TRANS_DVBS,	eDxSAT_PSK_QPSK,	0,	0,	0, 	0},
/*3*/	{	10832,	22000,	eDxSAT_POLAR_HOR,	eDxSAT_CODERATE_5_6,	eDxSAT_TRANS_DVBS,	eDxSAT_PSK_QPSK,	0,	0,	0, 	0},
/*4*/	{	0,		0,		0,					0,				  		0,					0,	 				0,	0,	0,	0},
/*5*/	{	0,		0,		0,					0,				  		0,					0,	 				0,	0,	0,	0},
/*6*/	{	0,		0,		0,					0,				  		0,					0,	 				0,	0,	0,	0},
/*7*/	{	0,		0,		0,					0,				  		0,					0,	 				0,	0,	0,	0},
/*8*/	{	0,		0,		0,					0,				  		0,					0,	 				0,	0,	0,	0},
/*9*/	{	0,		0,		0,					0,				  		0,					0,	 				0,	0,	0,	0},
/*10*/	{	0,		0,		0,					0,				  		0,					0,	 				0,	0,	0,	0},
/*11*/	{	0,		0,		0,					0,				  		0,					0,	 				0,	0,	0,	0},
/*12*/	{	0,		0,		0,					0,				  		0,					0,	 				0,	0,	0,	0},
/*13*/	{	0,		0,		0,					0,				  		0,					0,	 				0,	0,	0,	0},
/*14*/	{	0,		0,		0,					0,				  		0,					0,	 				0,	0,	0,	0},
/*15*/	{	0,		0,		0,					0,				  		0,					0,	 				0,	0,	0,	0},
/*16*/	{	0,		0,		0,					0,				  		0,					0,	 				0,	0,	0,	0},
/*17*/	{	0,		0,		0,					0,				  		0,					0,	 				0,	0,	0,	0},
/*18*/	{	0,		0,		0,					0,				  		0,					0,	 				0,	0,	0,	0},
/*19*/	{	0,		0,		0,					0,				  		0,					0,	 				0,	0,	0,	0},
/*20*/	{	0,		0,		0,					0,				  		0,					0,	 				0,	0,	0,	0},
/*21*/	{	0,		0,		0,					0,				  		0,					0,	 				0,	0,	0,	0},
/*22*/	{	0,		0,		0,					0,				  		0,					0,	 				0,	0,	0,	0},
/*23*/	{	0,		0,		0,					0,				  		0,					0,	 				0,	0,	0,	0},
/*24*/	{	0,		0,		0,					0,				  		0,					0,	 				0,	0,	0,	0},
/*25*/	{	0,		0,		0,					0,				  		0,					0,	 				0,	0,	0,	0},
/*26*/	{	0,		0,		0,					0,				  		0,					0,	 				0,	0,	0,	0},
/*27*/	{	0,		0,		0,					0,				  		0,					0,	 				0,	0,	0,	0},
/*28*/	{	0,		0,		0,					0,				  		0,					0,	 				0,	0,	0,	0},
/*29*/	{	0,		0,		0,					0,				  		0,					0,	 				0,	0,	0,	0},
/*30*/	{	0,		0,		0,					0,				  		0,					0,	 				0,	0,	0,	0},
/*31*/	{	0,		0,		0,					0,				  		0,					0,	 				0,	0,	0,	0},
/*32*/	{	0,		0,		0,					0,				 	 	0,					0,	 				0,	0,	0,	0}
};

static POINTER_LIST_T		*s_pstSes_NetworkInfoList = NULL;		// DbSvc_NetInfo_t

static POINTER_LIST_T		*s_pstSes_DeliveryTsList = NULL;		// DbSvc_TsInfo_t

static POINTER_LIST_T		*s_pstSes_AstraRefTsList = NULL;		// DbSvc_TsInfo_t

static POINTER_LIST_T		*s_pstSes_AstraSgtTsList = NULL;		// DbSVc_TsInfo_t
static POINTER_LIST_T		*s_pstSes_AstraSgtTripleIdList = NULL;	// DbSvc_TripleId_t

#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
static POINTER_LIST_T		*s_pstSes_AstraPortalRefTsList = NULL;		// DbSvc_TsInfo_t

static POINTER_LIST_T		*s_pstSes_AstraPortalTsList = NULL;		// DbSvc_TsInfo_t
static POINTER_LIST_T		*s_pstSes_AstraPortalTripleIdList = NULL;	// DbSvc_TripleId_t

static POINTER_LIST_T		*s_pstSes_AstraRegionalPortalTsList = NULL;		// DbSvc_TsInfo_t
static POINTER_LIST_T		*s_pstSes_AstraRegionalPortalTripleIdList = NULL;	// DbSvc_TripleId_t
#endif

static HBOOL				 s_bSes_SesLcnAllSearchFlag = TRUE;

static SesHdFuncMode_t		 s_eSes_SesHdFuncMode = eSesHdFuncMode_Normal;

#if defined(CONFIG_MW_CAS_NAGRA_SES_CSC_DOWNLOAD)

#define MAX_SC_REVISION_COUNT				64
#define MAX_SC_REVISION_STRSIZE				64


typedef struct tagAPRevision_Info_t
{
	HUINT16		usCASytemID;
	HUINT8		ucTextLen;
	HUINT8		aucTextChar[MAX_SC_REVISION_STRSIZE];
} APRevision_Info_t; 			//1 SC_Revision_Info_t 와 마추어야 함

typedef struct tagAPCSCDNContentInfo_t
{
	HUINT16			usTsId;
	HUINT16			usOrgNetId;
	HUINT16			usSvcId;

	DbSvc_TsInfo_t	stTs;

	HUINT8			ucVersion;
	HUINT8			ucCSCDataIndex;
	HUINT16			usCSCTimeout;
	HUINT16			usEMMTimeout;

	HUINT8					ucSCRevCnt;
	APRevision_Info_t		astRevs[MAX_SC_REVISION_COUNT];
}APCSCDNContentInfo_t; 		//1 SI_CSCDNContentInfo_t 과 같게 마추어야 함


static APCSCDNContentInfo_t	*s_pstCscDownloadData = NULL;
#endif


/*******************************************************************/
/**************** Static Function Prototypes  **********************/
/*******************************************************************/

static HERROR		xmgr_search_info_GetAstraAntIdx (HUINT32 *pulAntIdx);
static void			xmgr_search_info_FreeTsInfo (void *pvItem);


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

static HERROR xmgr_search_info_GetAstraAntIdx (HUINT32 *pulAntIdx)
{
	HBOOL				 bFound;
	HUINT32				 ulAntIdx;
	HINT32				 nAntCnt, nTotalAntNum;
	HINT32				*pnAntIdxArray;
	DbSvc_AntInfo_t	 stAntInfo;
	HERROR				 hErr;

	if (pulAntIdx == NULL)				{ return ERR_FAIL; }

	// ASTRA로 설정된 Antenna를 찾아야 한다.
	bFound = FALSE;
	pnAntIdxArray = NULL;
	ulAntIdx = 0;

	hErr = DB_SVC_FindAllAntIdxList (&nTotalAntNum, &pnAntIdxArray);
	if (hErr == ERR_OK)
	{
		for (nAntCnt = 0; nAntCnt < nTotalAntNum; nAntCnt++)
		{
			hErr = DB_SVC_GetAntInfo ((HUINT16)pnAntIdxArray[nAntCnt], &stAntInfo);
			if (hErr == ERR_OK)
			{
				if (stAntInfo.eSatType == eSatType_ASTRA_1)
				{
					bFound = TRUE;
					ulAntIdx = (HUINT32)pnAntIdxArray[nAntCnt];
					break;
				}
			}
		}

		DB_SVC_FreeAntList (pnAntIdxArray);
	}

	if (bFound == FALSE)
	{
		return ERR_FAIL;
	}

	*pulAntIdx = ulAntIdx;
	return ERR_OK;
}

static void xmgr_search_info_FreeTsInfo (void *pvItem)
{
	if (pvItem != NULL)
	{
		OxAP_Free (pvItem);
	}
}

static void xmgr_search_info_FreeTripleId (void *pvItem)
{
	if (pvItem != NULL)
	{
		OxAP_Free (pvItem);
	}
}

static void xmgr_search_info_FreeNetInfo (void *pvItem)
{
	if (pvItem != NULL)
	{
		OxAP_Free (pvItem);
	}
}

static HBOOL xmgr_search_info_FilterSameOnTsIdInTsInfo (void *pvItem, void *pvFilter)
{
	DbSvc_TsInfo_t	*pstItem, *pstFilter;

	pstItem = (DbSvc_TsInfo_t *)pvItem;
	pstFilter = (DbSvc_TsInfo_t *)pvFilter;

	if (pstItem != NULL && pstFilter != NULL)
	{
		if (pstItem->usOnId == pstFilter->usOnId &&
			pstItem->usTsId == pstFilter->usTsId)
		{
			return TRUE;
		}
	}

	return FALSE;
}

static HBOOL xmgr_search_info_FilterSameTripleId (void *pvItem, void *pvFilter)
{
	DbSvc_TripleId_t	*pstItem, *pstFilter;

	pstItem = (DbSvc_TripleId_t *)pvItem;
	pstFilter = (DbSvc_TripleId_t *)pvFilter;

	if (pstItem != NULL && pstFilter != NULL)
	{
		if (pstItem->usOnId == pstFilter->usOnId &&
			pstItem->usTsId == pstFilter->usTsId &&
			pstItem->usSvcId == pstFilter->usSvcId)
		{
			return TRUE;
		}
	}

	return FALSE;
}

static HBOOL xmgr_search_info_FilterSameNetIdInNetInfo (void *pvItem, void *pvFilter)
{
	DbSvc_NetInfo_t	*pstItem, *pstFilter;

	pstItem = (DbSvc_NetInfo_t *)pvItem;
	pstFilter = (DbSvc_NetInfo_t *)pvFilter;

	if (pstItem != NULL && pstFilter != NULL)
	{
		if (pstItem->usNetId == pstFilter->usNetId)
		{
			return TRUE;
		}
	}

	return FALSE;
}


#define _____EXTERNAL_FUNCTIONS_____

HERROR XMGR_SEARCH_INFO_GetSesHdFuncMode (SesHdFuncMode_t *peMode)
{
	if (peMode == NULL)				{ return ERR_FAIL; }

	*peMode = s_eSes_SesHdFuncMode;
	return ERR_OK;
}

HERROR XMGR_SEARCH_INFO_SetSesHdFuncMode (SesHdFuncMode_t eMode)
{
	if (eMode < eSesHdFuncMode_Normal || eMode > eSesHdFuncMode_StandBy)
	{
		eMode = eSesHdFuncMode_Normal;
	}

	s_eSes_SesHdFuncMode = eMode;
	return ERR_OK;
}

HERROR XMGR_SEARCH_INFO_GetAstraAntIdx (HUINT32 *pulAntIdx)
{
	return xmgr_search_info_GetAstraAntIdx (pulAntIdx);
}

// Network Info
HERROR XMGR_SEARCH_INFO_InitNetworkInfo (void)
{
	s_pstSes_NetworkInfoList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (s_pstSes_NetworkInfoList, xmgr_search_info_FreeNetInfo);
	return ERR_OK;
}

HERROR XMGR_SEARCH_INFO_AddNetworkInfo (DbSvc_NetInfo_t *pstNetInfo)
{
	POINTER_LIST_T	*pstItem;
	DbSvc_NetInfo_t	*pstDstNetInfo;

	if (pstNetInfo == NULL)						{ return ERR_BUS_SOURCE_NULL; }

	pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (s_pstSes_NetworkInfoList, 0, xmgr_search_info_FilterSameNetIdInNetInfo, (void *)pstNetInfo);
	if (pstItem != NULL)
	{
		return ERR_OK;
	}

	pstDstNetInfo = (DbSvc_NetInfo_t *)OxAP_Malloc(sizeof(DbSvc_NetInfo_t));
	if (pstDstNetInfo == NULL)					{ return ERR_FAIL; }

	*pstDstNetInfo = *pstNetInfo;
	s_pstSes_NetworkInfoList = UTIL_LINKEDLIST_AppendListItemToPointerList (s_pstSes_NetworkInfoList, (void *)pstDstNetInfo);
	return ERR_OK;
}

HUINT32 XMGR_SEARCH_INFO_CountNetworkInfo (void)
{
	return (HUINT32)UTIL_LINKEDLIST_GetNumListItemsInPointerList (s_pstSes_NetworkInfoList, NULL, NULL);
}

HERROR XMGR_SEARCH_INFO_GetNetworkInfo (HUINT32 ulIndex, DbSvc_NetInfo_t *pstNetInfo)
{
	POINTER_LIST_T		*pstItem;
	DbSvc_NetInfo_t		*pstSrcNetInfo;

	if (pstNetInfo == NULL)						{ return ERR_FAIL; }

	pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (s_pstSes_NetworkInfoList, (HUINT16)ulIndex, NULL, NULL);
	if (pstItem != NULL)
	{
		pstSrcNetInfo = (DbSvc_NetInfo_t *)UTIL_LINKEDLIST_GetContents (pstItem);
		if (pstSrcNetInfo != NULL)
		{
			*pstNetInfo = *pstSrcNetInfo;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}


// Anchor TP
HUINT32 XMGR_SEARCH_INFO_CountAnchorTsInfo (void)
{
	HUINT32					 ulCount;
	DbSvc_SatTuningInfo_t	*pstTuneInfo;

	for (ulCount = 0; ulCount < MAX_ANCHOR_TP; ulCount++)
	{
		pstTuneInfo = &(s_stSesAnchorTuneInfo[ulCount]);
		if (pstTuneInfo->ulFrequency == 0)			{ break; }
	}

	return ulCount;
}

HERROR XMGR_SEARCH_INFO_GetAnchorTsInfo (HUINT32 ulTsCnt, DbSvc_TsInfo_t *pstTsInfo)
{
	HUINT32					 ulCount;
	DbSvc_SatTuningInfo_t	*pstTuneInfo;

	if (pstTsInfo == NULL)					{ return ERR_FAIL; }

	for (ulCount = 0; ulCount <= ulTsCnt; ulCount++)
	{
		pstTuneInfo = &(s_stSesAnchorTuneInfo[ulCount]);
		if (pstTuneInfo->ulFrequency == 0)
		{
			return ERR_FAIL;
		}
	}

	pstTsInfo->stTuningParam.sat	= s_stSesAnchorTuneInfo[ulTsCnt];

	return ERR_BUS_NO_ERROR;
}

// ASTRA Reference TP
HERROR XMGR_SEARCH_INFO_InitDeliveryTsInfo (void)
{
	s_pstSes_DeliveryTsList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (s_pstSes_DeliveryTsList, xmgr_search_info_FreeTsInfo);
	return ERR_OK;
}

HERROR XMGR_SEARCH_INFO_AddDeliveryTsInfo (DbSvc_TsInfo_t *pstTsInfo)
{
	POINTER_LIST_T	*pstItem;
	DbSvc_TsInfo_t	*pstOrgTsInfo, *pstDstTsInfo;

	if (pstTsInfo == NULL)						{ return ERR_BUS_SOURCE_NULL; }

	pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (s_pstSes_DeliveryTsList, 0, xmgr_search_info_FilterSameOnTsIdInTsInfo, (void *)pstTsInfo);
	if (pstItem != NULL)
	{
		pstOrgTsInfo = (DbSvc_TsInfo_t *)UTIL_LINKEDLIST_GetContents(pstItem);
		if (pstOrgTsInfo != NULL)
		{
			pstOrgTsInfo->ucTunerId = pstTsInfo->ucTunerId;
			pstOrgTsInfo->stTuningParam = pstTsInfo->stTuningParam;
		}

		return ERR_OK;
	}

	pstDstTsInfo = (DbSvc_TsInfo_t *)OxAP_Malloc (sizeof(DbSvc_TsInfo_t));
	if (pstDstTsInfo == NULL)					{ return ERR_FAIL; }

	*pstDstTsInfo = *pstTsInfo;
	s_pstSes_DeliveryTsList = UTIL_LINKEDLIST_AppendListItemToPointerList (s_pstSes_DeliveryTsList, (void *)pstDstTsInfo);
	return ERR_OK;
}

HUINT32 XMGR_SEARCH_INFO_CountDeliveryTsInfo (void)
{
	return (HUINT32)UTIL_LINKEDLIST_GetNumListItemsInPointerList (s_pstSes_DeliveryTsList, NULL, NULL);
}

HERROR XMGR_SEARCH_INFO_GetDeliveryTsInfo (HUINT32 ulIndex, DbSvc_TsInfo_t *pstTsInfo)
{
	POINTER_LIST_T		*pstItem;
	DbSvc_TsInfo_t		*pstSrcTsInfo;

	if (pstTsInfo == NULL)						{ return ERR_FAIL; }

	pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (s_pstSes_DeliveryTsList, (HUINT16)ulIndex, NULL, NULL);
	if (pstItem != NULL)
	{
		pstSrcTsInfo = (DbSvc_TsInfo_t *)UTIL_LINKEDLIST_GetContents (pstItem);
		if (pstSrcTsInfo != NULL)
		{
			*pstTsInfo = *pstSrcTsInfo;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}


// ASTRA Reference TP
HERROR XMGR_SEARCH_INFO_InitAstraReferenceTsInfo (void)
{
	s_pstSes_AstraRefTsList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (s_pstSes_AstraRefTsList, xmgr_search_info_FreeTsInfo);
	return ERR_OK;
}

HERROR XMGR_SEARCH_INFO_AddAstraReferenceTsInfo (DbSvc_TsInfo_t *pstTsInfo)
{
	POINTER_LIST_T	*pstItem;
	DbSvc_TsInfo_t	*pstOrgTsInfo, *pstDstTsInfo;

	if (pstTsInfo == NULL)						{ return ERR_BUS_SOURCE_NULL; }

	pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (s_pstSes_AstraRefTsList, 0, xmgr_search_info_FilterSameOnTsIdInTsInfo, (void *)pstTsInfo);
	if (pstItem != NULL)
	{
		pstOrgTsInfo = (DbSvc_TsInfo_t *)UTIL_LINKEDLIST_GetContents(pstItem);
		if (pstOrgTsInfo != NULL)
		{
			pstOrgTsInfo->ucTunerId = pstTsInfo->ucTunerId;
			pstOrgTsInfo->stTuningParam = pstTsInfo->stTuningParam;
		}

		return ERR_OK;
	}

	pstDstTsInfo = (DbSvc_TsInfo_t *)OxAP_Malloc (sizeof(DbSvc_TsInfo_t));
	if (pstDstTsInfo == NULL)					{ return ERR_FAIL; }

	*pstDstTsInfo = *pstTsInfo;
	s_pstSes_AstraRefTsList = UTIL_LINKEDLIST_AppendListItemToPointerList (s_pstSes_AstraRefTsList, (void *)pstDstTsInfo);
	return ERR_OK;
}

HUINT32 XMGR_SEARCH_INFO_CountAstraReferenceTsInfo (void)
{
	return (HUINT32)UTIL_LINKEDLIST_GetNumListItemsInPointerList (s_pstSes_AstraRefTsList, NULL, NULL);
}

HERROR XMGR_SEARCH_INFO_GetAstraReferenceTsInfo (HUINT32 ulIndex, DbSvc_TsInfo_t *pstTsInfo)
{
	POINTER_LIST_T		*pstItem;
	DbSvc_TsInfo_t		*pstSrcTsInfo;

	if (pstTsInfo == NULL)						{ return ERR_FAIL; }

	pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (s_pstSes_AstraRefTsList, (HUINT16)ulIndex, NULL, NULL);
	if (pstItem != NULL)
	{
		pstSrcTsInfo = (DbSvc_TsInfo_t *)UTIL_LINKEDLIST_GetContents (pstItem);
		if (pstSrcTsInfo != NULL)
		{
			*pstTsInfo = *pstSrcTsInfo;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

// ASTRA SGT TP
HERROR XMGR_SEARCH_INFO_InitAstraSgtTsInfo (void)
{
	s_pstSes_AstraSgtTsList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (s_pstSes_AstraSgtTsList, xmgr_search_info_FreeTsInfo);
	s_pstSes_AstraSgtTripleIdList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (s_pstSes_AstraSgtTripleIdList, xmgr_search_info_FreeTripleId);

	return ERR_OK;
}

HERROR XMGR_SEARCH_INFO_AddAstraSgtTsInfo (DbSvc_TsInfo_t *pstTsInfo, DbSvc_TripleId_t *pstTripleId)
{
	POINTER_LIST_T		*pstItem;
	DbSvc_TripleId_t	*pstDstTripleId;
	DbSvc_TsInfo_t		*pstOrgTsInfo, *pstDstTsInfo;

	if (pstTsInfo == NULL || pstTripleId == NULL)
	{
		return ERR_BUS_SOURCE_NULL;
	}

	if (pstTsInfo->usOnId != pstTripleId->usOnId || pstTsInfo->usTsId != pstTripleId->usTsId)
	{
		return ERR_FAIL;
	}

	// TS Info
	pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (s_pstSes_AstraSgtTsList, 0, xmgr_search_info_FilterSameOnTsIdInTsInfo, (void *)pstTsInfo);
	if (pstItem != NULL)
	{
		pstOrgTsInfo = (DbSvc_TsInfo_t *)UTIL_LINKEDLIST_GetContents(pstItem);
		if (pstOrgTsInfo != NULL)
		{
			pstOrgTsInfo->ucTunerId = pstTsInfo->ucTunerId;
			pstOrgTsInfo->stTuningParam = pstTsInfo->stTuningParam;
		}
	}
	else
	{
		pstDstTsInfo = (DbSvc_TsInfo_t *)OxAP_Malloc (sizeof(DbSvc_TsInfo_t));
		if (pstDstTsInfo == NULL)					{ return ERR_FAIL; }

		*pstDstTsInfo = *pstTsInfo;
		s_pstSes_AstraSgtTsList = UTIL_LINKEDLIST_AppendListItemToPointerList (s_pstSes_AstraSgtTsList, (void *)pstDstTsInfo);
	}

	// Triple ID
	pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (s_pstSes_AstraSgtTripleIdList, 0, xmgr_search_info_FilterSameTripleId, (void *)pstTripleId);
	if (pstItem == NULL)
	{
		pstDstTripleId = (DbSvc_TripleId_t *)OxAP_Malloc (sizeof(DbSvc_TripleId_t));
		if (pstDstTripleId == NULL)					{ return ERR_FAIL; }

		*pstDstTripleId = *pstTripleId;
		s_pstSes_AstraSgtTripleIdList = UTIL_LINKEDLIST_AppendListItemToPointerList (s_pstSes_AstraSgtTripleIdList, (void *)pstDstTripleId);
	}

	return ERR_OK;
}

HUINT32 XMGR_SEARCH_INFO_CountAstraSgtTsInfo (void)
{
	return (HUINT32)UTIL_LINKEDLIST_GetNumListItemsInPointerList (s_pstSes_AstraSgtTripleIdList, NULL, NULL);
}

HERROR XMGR_SEARCH_INFO_GetAstraSgtTsInfo (HUINT32 ulIndex, DbSvc_TsInfo_t *pstTsInfo, DbSvc_TripleId_t *pstTripleId)
{
	DbSvc_TsInfo_t			 stTsInfo;
	POINTER_LIST_T			*pstItem;
	DbSvc_TripleId_t		*pstSrcTripleId;
	DbSvc_TsInfo_t			*pstSrcTsInfo;

	if (pstTsInfo == NULL || pstTripleId == NULL)						{ return ERR_FAIL; }

	pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (s_pstSes_AstraSgtTripleIdList, (HUINT16)ulIndex, NULL, NULL);
	if (pstItem != NULL)
	{
		pstSrcTripleId = (DbSvc_TripleId_t *)UTIL_LINKEDLIST_GetContents (pstItem);
		if (pstSrcTripleId != NULL)
		{
			stTsInfo.usOnId = pstSrcTripleId->usOnId;
			stTsInfo.usTsId = pstSrcTripleId->usTsId;

			pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (s_pstSes_AstraSgtTsList, 0, xmgr_search_info_FilterSameOnTsIdInTsInfo, (void *)&stTsInfo);
			if (pstItem != NULL)
			{
				pstSrcTsInfo = (DbSvc_TsInfo_t *)UTIL_LINKEDLIST_GetContents (pstItem);
				if (pstSrcTsInfo != NULL)
				{
					*pstTripleId	= *pstSrcTripleId;
					*pstTsInfo		= *pstSrcTsInfo;

					return ERR_OK;
				}
			}
		}
	}

	return ERR_FAIL;
}

#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
// Portal Reference TP
HERROR XMGR_SEARCH_INFO_InitAstraPortalReferenceTsInfo (void)
{
	s_pstSes_AstraPortalRefTsList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (s_pstSes_AstraPortalRefTsList, xmgr_search_info_FreeTsInfo);
	return ERR_OK;
}

HERROR XMGR_SEARCH_INFO_AddAstraPortalReferenceTsInfo (DbSvc_TsInfo_t *pstTsInfo)
{
	POINTER_LIST_T	*pstItem;
	DbSvc_TsInfo_t	*pstOrgTsInfo, *pstDstTsInfo;

	if (pstTsInfo == NULL)						{ return ERR_BUS_SOURCE_NULL; }

	pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (s_pstSes_AstraPortalRefTsList, 0, xmgr_search_info_FilterSameOnTsIdInTsInfo, (void *)pstTsInfo);
	if (pstItem != NULL)
	{
		pstOrgTsInfo = (DbSvc_TsInfo_t *)UTIL_LINKEDLIST_GetContents(pstItem);
		if (pstOrgTsInfo != NULL)
		{
			pstOrgTsInfo->ucTunerId = pstTsInfo->ucTunerId;
			pstOrgTsInfo->stTuningParam = pstTsInfo->stTuningParam;
		}

		return ERR_OK;
	}

	pstDstTsInfo = (DbSvc_TsInfo_t *)OxAP_Malloc (sizeof(DbSvc_TsInfo_t));
	if (pstDstTsInfo == NULL)					{ return ERR_FAIL; }

	VK_Print("[xmgr_search_info_AddAstraPortalReferenceTsInfo]   OnID (0x%04x) TsID(0x%04x) Freq(%d) \n", pstTsInfo->usOnId, pstTsInfo->usTsId, pstTsInfo->stTuningParam.sat.ulFrequency);

	*pstDstTsInfo = *pstTsInfo;
	s_pstSes_AstraPortalRefTsList = UTIL_LINKEDLIST_AppendListItemToPointerList (s_pstSes_AstraPortalRefTsList, (void *)pstDstTsInfo);
	return ERR_OK;
}

HUINT32 XMGR_SEARCH_INFO_CountAstraPortalReferenceTsInfo (void)
{
	return (HUINT32)UTIL_LINKEDLIST_GetNumListItemsInPointerList (s_pstSes_AstraPortalRefTsList, NULL, NULL);
}

HERROR XMGR_SEARCH_INFO_GetAstraPortalReferenceTsInfo (HUINT32 ulIndex, DbSvc_TsInfo_t *pstTsInfo)
{
	POINTER_LIST_T		*pstItem;
	DbSvc_TsInfo_t		*pstSrcTsInfo;

	if (pstTsInfo == NULL)						{ return ERR_FAIL; }

	pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (s_pstSes_AstraPortalRefTsList, (HUINT16)ulIndex, NULL, NULL);
	if (pstItem != NULL)
	{
		pstSrcTsInfo = (DbSvc_TsInfo_t *)UTIL_LINKEDLIST_GetContents (pstItem);
		if (pstSrcTsInfo != NULL)
		{
			*pstTsInfo = *pstSrcTsInfo;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

// ASTRA PORTAL TP
HERROR XMGR_SEARCH_INFO_InitAstraPortalTsInfo (void)
{
	s_pstSes_AstraPortalTsList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (s_pstSes_AstraPortalTsList, xmgr_search_info_FreeTsInfo);
	s_pstSes_AstraPortalTripleIdList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (s_pstSes_AstraPortalTripleIdList, xmgr_search_info_FreeTripleId);

	return ERR_OK;
}

HERROR XMGR_SEARCH_INFO_AddAstraPortalTsInfo (DbSvc_TsInfo_t *pstTsInfo, DbSvc_TripleId_t *pstTripleId)
{
	POINTER_LIST_T		*pstItem;
	DbSvc_TripleId_t	*pstDstTripleId;
	DbSvc_TsInfo_t		*pstOrgTsInfo, *pstDstTsInfo;

	if (pstTsInfo == NULL || pstTripleId == NULL)
	{
		return ERR_BUS_SOURCE_NULL;
	}

	if (pstTsInfo->usOnId != pstTripleId->usOnId || pstTsInfo->usTsId != pstTripleId->usTsId)
	{
		return ERR_FAIL;
	}

	// TS Info
	pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (s_pstSes_AstraPortalTsList, 0, xmgr_search_info_FilterSameOnTsIdInTsInfo, (void *)pstTsInfo);
	if (pstItem != NULL)
	{
		pstOrgTsInfo = (DbSvc_TsInfo_t *)UTIL_LINKEDLIST_GetContents(pstItem);
		if (pstOrgTsInfo != NULL)
		{
			pstOrgTsInfo->ucTunerId = pstTsInfo->ucTunerId;
			pstOrgTsInfo->stTuningParam = pstTsInfo->stTuningParam;
		}
	}
	else
	{
		pstDstTsInfo = (DbSvc_TsInfo_t *)OxAP_Malloc (sizeof(DbSvc_TsInfo_t));
		if (pstDstTsInfo == NULL)					{ return ERR_FAIL; }

		*pstDstTsInfo = *pstTsInfo;
		s_pstSes_AstraPortalTsList = UTIL_LINKEDLIST_AppendListItemToPointerList (s_pstSes_AstraPortalTsList, (void *)pstDstTsInfo);
	}

	// Triple ID
	pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (s_pstSes_AstraPortalTripleIdList, 0, xmgr_search_info_FilterSameTripleId, (void *)pstTripleId);
	if (pstItem == NULL)
	{
		pstDstTripleId = (DbSvc_TripleId_t *)OxAP_Malloc (sizeof(DbSvc_TripleId_t));
		if (pstDstTripleId == NULL)					{ return ERR_FAIL; }

		*pstDstTripleId = *pstTripleId;
		s_pstSes_AstraPortalTripleIdList = UTIL_LINKEDLIST_AppendListItemToPointerList (s_pstSes_AstraPortalTripleIdList, (void *)pstDstTripleId);
	}

	return ERR_OK;
}

HUINT32 XMGR_SEARCH_INFO_CountAstraPortalTsInfo (void)
{
	return (HUINT32)UTIL_LINKEDLIST_GetNumListItemsInPointerList (s_pstSes_AstraPortalTripleIdList, NULL, NULL);
}

HERROR XMGR_SEARCH_INFO_GetAstraPortalTsInfo (HUINT32 ulIndex, DbSvc_TsInfo_t *pstTsInfo, DbSvc_TripleId_t *pstTripleId)
{
	DbSvc_TsInfo_t			 stTsInfo;
	POINTER_LIST_T			*pstItem;
	DbSvc_TripleId_t		*pstSrcTripleId;
	DbSvc_TsInfo_t			*pstSrcTsInfo;

	if (pstTsInfo == NULL || pstTripleId == NULL)						{ return ERR_FAIL; }

	pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (s_pstSes_AstraPortalTripleIdList, (HUINT16)ulIndex, NULL, NULL);
	if (pstItem != NULL)
	{
		pstSrcTripleId = (DbSvc_TripleId_t *)UTIL_LINKEDLIST_GetContents (pstItem);
		if (pstSrcTripleId != NULL)
		{
			stTsInfo.usOnId = pstSrcTripleId->usOnId;
			stTsInfo.usTsId = pstSrcTripleId->usTsId;

			pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (s_pstSes_AstraPortalTsList, 0, xmgr_search_info_FilterSameOnTsIdInTsInfo, (void *)&stTsInfo);
			if (pstItem != NULL)
			{
				pstSrcTsInfo = (DbSvc_TsInfo_t *)UTIL_LINKEDLIST_GetContents (pstItem);
				if (pstSrcTsInfo != NULL)
				{
					*pstTripleId	= *pstSrcTripleId;
					*pstTsInfo		= *pstSrcTsInfo;

					return ERR_OK;
				}
			}
		}
	}

	return ERR_FAIL;
}

// ASTRA REGIONAL PORTAL TP
HERROR XMGR_SEARCH_INFO_InitAstraRegionalPortalTsInfo (void)
{
	s_pstSes_AstraRegionalPortalTsList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (s_pstSes_AstraRegionalPortalTsList, xmgr_search_info_FreeTsInfo);
	s_pstSes_AstraRegionalPortalTripleIdList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (s_pstSes_AstraRegionalPortalTripleIdList, xmgr_search_info_FreeTripleId);

	return ERR_OK;
}

HERROR XMGR_SEARCH_INFO_AddAstraRegionalPortalTsInfo (DbSvc_TsInfo_t *pstTsInfo, DbSvc_TripleId_t *pstTripleId)
{
	POINTER_LIST_T		*pstItem;
	DbSvc_TripleId_t	*pstDstTripleId;
	DbSvc_TsInfo_t		*pstOrgTsInfo, *pstDstTsInfo;

	if (pstTsInfo == NULL || pstTripleId == NULL)
	{
		return ERR_BUS_SOURCE_NULL;
	}

	if (pstTsInfo->usOnId != pstTripleId->usOnId || pstTsInfo->usTsId != pstTripleId->usTsId)
	{
		return ERR_FAIL;
	}

	// TS Info
	pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (s_pstSes_AstraRegionalPortalTsList, 0, xmgr_search_info_FilterSameOnTsIdInTsInfo, (void *)pstTsInfo);
	if (pstItem != NULL)
	{
		pstOrgTsInfo = (DbSvc_TsInfo_t *)UTIL_LINKEDLIST_GetContents(pstItem);
		if (pstOrgTsInfo != NULL)
		{
			pstOrgTsInfo->ucTunerId = pstTsInfo->ucTunerId;
			pstOrgTsInfo->stTuningParam = pstTsInfo->stTuningParam;
		}
	}
	else
	{
		pstDstTsInfo = (DbSvc_TsInfo_t *)OxAP_Malloc (sizeof(DbSvc_TsInfo_t));
		if (pstDstTsInfo == NULL)					{ return ERR_FAIL; }

		*pstDstTsInfo = *pstTsInfo;
		s_pstSes_AstraRegionalPortalTsList = UTIL_LINKEDLIST_AppendListItemToPointerList (s_pstSes_AstraRegionalPortalTsList, (void *)pstDstTsInfo);
	}

	// Triple ID
	pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (s_pstSes_AstraRegionalPortalTripleIdList, 0, xmgr_search_info_FilterSameTripleId, (void *)pstTripleId);
	if (pstItem == NULL)
	{
		pstDstTripleId = (DbSvc_TripleId_t *)OxAP_Malloc (sizeof(DbSvc_TripleId_t));
		if (pstDstTripleId == NULL)					{ return ERR_FAIL; }

		*pstDstTripleId = *pstTripleId;
		s_pstSes_AstraRegionalPortalTripleIdList = UTIL_LINKEDLIST_AppendListItemToPointerList (s_pstSes_AstraRegionalPortalTripleIdList, (void *)pstDstTripleId);
	}

	return ERR_OK;
}

HUINT32 XMGR_SEARCH_INFO_CountAstraRegionalPortalTsInfo (void)
{
	return (HUINT32)UTIL_LINKEDLIST_GetNumListItemsInPointerList (s_pstSes_AstraRegionalPortalTripleIdList, NULL, NULL);
}

HERROR XMGR_SEARCH_INFO_GetAstraRegionalPortalTsInfo (HUINT32 ulIndex, DbSvc_TsInfo_t *pstTsInfo, DbSvc_TripleId_t *pstTripleId)
{
	DbSvc_TsInfo_t			 stTsInfo;
	POINTER_LIST_T			*pstItem;
	DbSvc_TripleId_t		*pstSrcTripleId;
	DbSvc_TsInfo_t			*pstSrcTsInfo;

	if (pstTsInfo == NULL || pstTripleId == NULL)						{ return ERR_FAIL; }

	pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (s_pstSes_AstraRegionalPortalTripleIdList, (HUINT16)ulIndex, NULL, NULL);
	if (pstItem != NULL)
	{
		pstSrcTripleId = (DbSvc_TripleId_t *)UTIL_LINKEDLIST_GetContents (pstItem);
		if (pstSrcTripleId != NULL)
		{
			stTsInfo.usOnId = pstSrcTripleId->usOnId;
			stTsInfo.usTsId = pstSrcTripleId->usTsId;

			pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (s_pstSes_AstraRegionalPortalTsList, 0, xmgr_search_info_FilterSameOnTsIdInTsInfo, (void *)&stTsInfo);
			if (pstItem != NULL)
			{
				pstSrcTsInfo = (DbSvc_TsInfo_t *)UTIL_LINKEDLIST_GetContents (pstItem);
				if (pstSrcTsInfo != NULL)
				{
					*pstTripleId	= *pstSrcTripleId;
					*pstTsInfo		= *pstSrcTsInfo;

					return ERR_OK;
				}
			}
		}
	}

	return ERR_FAIL;
}
#endif

#define _____SESLCN_ARGUMENTS_____

HERROR XMGR_SEARCH_INFO_GetSesLcnAllSearchFlag (HBOOL *pbAllFlag)
{
	if (pbAllFlag == NULL)					{ return ERR_FAIL; }

	*pbAllFlag = s_bSes_SesLcnAllSearchFlag;
	return ERR_OK;
}

HERROR XMGR_SEARCH_INFO_SetSesLcnAllSearchFlag (HBOOL bAllFlag)
{
	s_bSes_SesLcnAllSearchFlag = bAllFlag;
	return ERR_OK;
}

HERROR XMGR_SEARCH_INFO_GetSesLcnServiceListIdFromDb (HUINT16 *pusSvcListId)
{
	HUINT32		 ulValue;
	HERROR		 hErr;

	if (pusSvcListId == NULL)					{ return ERR_FAIL; }

//	hErr = DB_SETUP_GetMenuItemValue (eDxSETUP_SYSTEM_SES_SVCLISTID, &ulValue);
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_SVCLISTID, (HUINT32 *)&ulValue, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	*pusSvcListId = (HUINT16)ulValue;

	return ERR_OK;
}

HERROR XMGR_SEARCH_INFO_SetSesLcnServiceListIdToDb (HUINT16 usSvcListId)
{
	HERROR		 hErr;

	hErr = DB_PARAM_SetItem(eDB_PARAM_ITEM_SVCLISTID, (HUINT32)usSvcListId, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_OK;
}

HERROR XMGR_SEARCH_INFO_GetSesLcnServiceListVerFromDb (HUINT8 *pucSvcListVer)
{
	HUINT32		 ulValue;
	HERROR		 hErr;

	if (pucSvcListVer == NULL)					{ return ERR_FAIL; }

//	hErr = DB_SETUP_GetMenuItemValue (eDxSETUP_SYSTEM_SES_SVCLISTVER, &ulValue);
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_SVCLISTVER, (HUINT32 *)&ulValue, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	*pucSvcListVer = (HUINT8)ulValue;

	return ERR_OK;
}

HERROR XMGR_SEARCH_INFO_SetSesLcnServiceListVerToDb (HUINT8 ucSvcListVer)
{
	HERROR		 hErr;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_SVCLISTVER, (HUINT32)ucSvcListVer, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_OK;
}

HERROR XMGR_SEARCH_INFO_GetSesLcnServiceListNameFromDb (HUINT8 *szSvcListName)
{
//	HINT32		 nDataSize;
	HUINT8		 aucDataBuf[64];
	HERROR		 hErr;

	if (szSvcListName == NULL)					{ return ERR_FAIL; }

//	hErr = DB_SETUP_GetMenuItemBinary (eDxSETUP_SYSTEM_SES_SVCLISTNAME, &nDataSize, aucDataBuf);
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_SVCLISTNAME, (HUINT32 *)aucDataBuf, sizeof(HUINT8)*64);
	if (hErr == ERR_OK)
	{
		aucDataBuf[62] = '\0';			aucDataBuf[63] = '\0';
		MWC_UTIL_DvbStrcpy (szSvcListName, aucDataBuf);
		return ERR_OK;
	}

	szSvcListName[0] = '\0';
	return ERR_FAIL;
}

HERROR XMGR_SEARCH_INFO_SetSesLcnServiceListNameToDb (HUINT8 *szSvcListName)
{
	//HINT32		 nDataSize;
	HUINT8		 aucDataBuf[64];
	HERROR		 hErr;

	if (szSvcListName == NULL)					{ return ERR_FAIL; }

	MWC_UTIL_DvbStrncpy (aucDataBuf, szSvcListName, 64);
	aucDataBuf[62] = '\0';			aucDataBuf[63] = '\0';

	hErr = DB_PARAM_SetItem(eDB_PARAM_ITEM_SVCLISTNAME, (HUINT32)aucDataBuf, sizeof(HUINT8)*64);
	if (hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR XMGR_SEARCH_INFO_GetSesLcnServiceNumberFromDb (HUINT32 *pulTvNum, HUINT32 *pulRadioNum)
{
	HUINT32		 ulValue;
	HERROR		 hErr;

	if (pulTvNum != NULL)
	{
//		hErr = DB_SETUP_GetMenuItemValue (eDxSETUP_SYSTEM_SES_TVNUM, &ulValue);
		hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_TVNUM, (HUINT32 *)&ulValue, 0);
		if (hErr != ERR_OK) 					{ return ERR_FAIL; }

		*pulTvNum = (HUINT32)ulValue;
	}

	if (pulRadioNum != NULL)
	{
//		hErr = DB_SETUP_GetMenuItemValue (eDxSETUP_SYSTEM_SES_RADIONUM, &ulValue);
		hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_RADIONUM, (HUINT32 *)&ulValue, 0);
		if (hErr != ERR_OK) 					{ return ERR_FAIL; }

		*pulRadioNum = (HUINT32)ulValue;
	}

	return ERR_OK;
}

HERROR XMGR_SEARCH_INFO_SetSesLcnServiceNumberToDb (HUINT32 ulTvNum, HUINT32 ulRadioNum)
{
	HERROR		 hErr;

	hErr  = DB_PARAM_SetItem (eDB_PARAM_ITEM_TVNUM, ulTvNum, 0);
	hErr |= DB_PARAM_SetItem (eDB_PARAM_ITEM_RADIONUM, ulRadioNum, 0);

	if (hErr == ERR_OK)
	{
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR XMGR_SEARCH_INFO_GetSesLcnLastCheckedTimeFromDb (UNIXTIME *putTime)
{
	HUINT32		 ulValue;
	HERROR		 hErr;

	if (putTime == NULL)					{ return ERR_FAIL; }

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LASTCHECKEDTIME, &ulValue, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	*putTime = (UNIXTIME)ulValue;

	return ERR_OK;
}

HERROR XMGR_SEARCH_INFO_SetSesLcnLastCheckedTimeToDb (UNIXTIME utTime)
{
	HERROR		 hErr;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_LASTCHECKEDTIME, (HUINT32)utTime, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_OK;
}

HERROR XMGR_SEARCH_INFO_GetSesLcnLastUpdatedTimeFromDb (UNIXTIME *putTime)
{
	HUINT32		 ulValue;
	HERROR		 hErr;

	if (putTime == NULL)					{ return ERR_FAIL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_LASTUPDATEDTIME, &ulValue, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	*putTime = (UNIXTIME)ulValue;

	return ERR_OK;
}

HERROR XMGR_SEARCH_INFO_SetSesLcnLastUpdatedTimeToDb (UNIXTIME utTime)
{
	HERROR		 hErr;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_LASTUPDATEDTIME, (HUINT32)utTime, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_OK;
}

HERROR XMGR_SEARCH_INFO_GetSesLcnColdBootResultFromDb (HUINT32 *pulValue)
{
	HUINT32		 ulValue;
	HERROR		 hErr;

	if (pulValue == NULL)					{ return ERR_FAIL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_COLDBOOT_RESULT, &ulValue, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	*pulValue = (HUINT32)ulValue;

	return ERR_OK;
}

HERROR XMGR_SEARCH_INFO_SetSesLcnColdBootResultToDb (HUINT32 ulValue)
{
	HERROR		 hErr;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_COLDBOOT_RESULT, (HUINT32)ulValue, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_OK;
}

HERROR XMGR_SEARCH_INFO_ClearSesLcnRelatedInfo (HBOOL bSyncDb)
{
#if 0		// have to check (yjlim)
	// 완전히 지워야 하는 값 :
	DB_SETUP_DeleteMenuConfigItem (eDxSETUP_SYSTEM_SES_SVCLISTID);
	DB_SETUP_DeleteMenuConfigItem (eDxSETUP_SYSTEM_SES_SVCLISTVER);
	DB_SETUP_DeleteMenuConfigItem (eDxSETUP_SYSTEM_SES_SVCLISTNAME);
	DB_SETUP_DeleteMenuConfigItem (eDxSETUP_SYSTEM_SES_TVNUM);
	DB_SETUP_DeleteMenuConfigItem (eDxSETUP_SYSTEM_SES_RADIONUM);
	DB_SETUP_DeleteMenuConfigItem (eDxSETUP_SYSTEM_SES_LASTCHECKEDTIME);
	DB_SETUP_DeleteMenuConfigItem (eDxSETUP_SYSTEM_SES_LASTUPDATEDTIME);
	DB_SETUP_DeleteMenuConfigItem (eDxSETUP_SYSTEM_SES_NEWSVC_ITEMS);
	DB_SETUP_DeleteMenuConfigItem (eDxSETUP_SYSTEM_SES_REPLACELATER_ITEMS);

    // Cache도 Clear.!!!
	DB_SETUP_CFGCache_Init();
#else
	DB_PARAM_SetItem (eDB_PARAM_ITEM_SVCLISTID, 0, 0);
	DB_PARAM_SetItem (eDB_PARAM_ITEM_SVCLISTVER, 0, 0);
	DB_PARAM_SetItem (eDB_PARAM_ITEM_SVCLISTNAME, 0, 0);
	DB_PARAM_SetItem (eDB_PARAM_ITEM_TVNUM, 0, 0);
	DB_PARAM_SetItem (eDB_PARAM_ITEM_RADIONUM, 0, 0);
	DB_PARAM_SetItem (eDB_PARAM_ITEM_LASTCHECKEDTIME, 0, 0);
	DB_PARAM_SetItem (eDB_PARAM_ITEM_LASTUPDATEDTIME, 0, 0);
	DB_PARAM_SetItem (eDB_PARAM_ITEM_NEWSVC_ITEMS, 0, 0);
	DB_PARAM_SetItem (eDB_PARAM_ITEM_REPLACELATER_ITEMS, 0, 0);
#endif

	// 0으로 세팅
	DB_PARAM_SetItem (eDB_PARAM_ITEM_SCINSERTED, 0, 0);
	DB_PARAM_SetItem (eDB_PARAM_ITEM_NEWSVC_NUM, 0, 0);
	DB_PARAM_SetItem (eDB_PARAM_ITEM_REPLACELATER_NUM, 0, 0);
	DB_PARAM_SetItem (eDB_PARAM_ITEM_COLDBOOT_RESULT, 0, 0);

	if (bSyncDb == TRUE)
	{
		DB_PARAM_Sync();
		DAPI_Sync(DxDBSETUP_SYSTEM);
	}

	return ERR_OK;
}


#if defined(CONFIG_MW_CAS_NAGRA_SES_CSC_DOWNLOAD)

HERROR XMGR_SEARCH_INFO_MakeCscDownloadInfo(void *pData)
{
	APCSCDNContentInfo_t *pCscDnInfo = NULL;

	if(pData == NULL)
	{
		return ERR_FAIL;
	}

	if(s_pstCscDownloadData != NULL)
	{
		OxAP_Free (s_pstCscDownloadData);
		s_pstCscDownloadData = NULL;
	}

	pCscDnInfo = (APCSCDNContentInfo_t*)OxAP_Malloc(sizeof(APCSCDNContentInfo_t));
	if(pCscDnInfo == NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_memcpy(pCscDnInfo, pData, sizeof(APCSCDNContentInfo_t));

	s_pstCscDownloadData = pCscDnInfo;

#if defined(DEBUG)
	VK_Print("------------------------------------------\n");
	VK_Print("-------XMGR_SEARCH_INFO_MakeCscDownloadInfo------\n");

	VK_Print("-- TS ID			[%04x]\n", s_pstCscDownloadData->usTsId);
	VK_Print("-- OnID			[%04x]\n", s_pstCscDownloadData->usOrgNetId);
	VK_Print("-- SvcID			[%04x]\n", s_pstCscDownloadData->usSvcId);
	VK_Print("-- Version		[%d]\n", s_pstCscDownloadData->ucVersion);
	VK_Print("-- ucCSCDataIndex	[%d]\n", s_pstCscDownloadData->ucCSCDataIndex);
	VK_Print("-- usCSCTimeout 	[%d]\n", s_pstCscDownloadData->usCSCTimeout);
	VK_Print("-- usEMMTimeout	[%d]\n", s_pstCscDownloadData->usEMMTimeout);
	VK_Print("-- ucSCRevCnt		[%d]\n", s_pstCscDownloadData->ucSCRevCnt);

	VK_Print("-- 0. usCASytemID	[%04x]\n", s_pstCscDownloadData->astRevs[0].usCASytemID);
	VK_Print("-- 0. ucTextLen	[%d]\n", s_pstCscDownloadData->astRevs[0].ucTextLen);
	VK_Print("-- 0. aucTextChar	[%s]\n", s_pstCscDownloadData->astRevs[0].aucTextChar);
	VK_Print("-- 1. usCASytemID	[%04x]\n", s_pstCscDownloadData->astRevs[1].usCASytemID);
	VK_Print("-- 1. ucTextLen	[%d]\n", s_pstCscDownloadData->astRevs[1].ucTextLen);
	VK_Print("-- 1. aucTextChar	[%s]\n", s_pstCscDownloadData->astRevs[1].aucTextChar);
	VK_Print("------------------------------------------\n");
#endif


	return ERR_OK;
}

HERROR XMGR_SEARCH_INFO_GetCscDownloadInfo(HUINT32 *pulDataIdx, HUINT32 *pulCscTimeOut, HUINT32 *pulEMMTimeOut, DbSvc_TsInfo_t *pstTs)
{
	if(pulDataIdx == NULL ||pulCscTimeOut == NULL || pulEMMTimeOut == NULL || pstTs == NULL)
	{
		return ERR_FAIL;
	}

	if(s_pstCscDownloadData == NULL)
	{
		return ERR_FAIL;
	}

	*pulDataIdx 		= (HUINT32)s_pstCscDownloadData->ucCSCDataIndex;
	*pulCscTimeOut		= (HUINT32)s_pstCscDownloadData->usCSCTimeout;
	*pulEMMTimeOut		= (HUINT32)s_pstCscDownloadData->usEMMTimeout;
	HxSTD_memcpy(pstTs, &(s_pstCscDownloadData->stTs), sizeof(DbSvc_TsInfo_t));

	return ERR_OK;
}

HERROR XMGR_SEARCH_INFO_GetCscDownloadCSCRev(HUINT16 usSCID, HUINT8 *pucRev)
{
	HUINT32 ulCnt = 0;

	if(pucRev == NULL)
	{
		return ERR_FAIL;
	}

	if(s_pstCscDownloadData == NULL)
	{
		return ERR_FAIL;
	}

	VK_Print("--[XMGR_SEARCH_INFO_GetCscDownloadCSCRev] ucSCRevCnt[%d]\n", s_pstCscDownloadData->ucSCRevCnt);

	for(ulCnt = 0; ulCnt < s_pstCscDownloadData->ucSCRevCnt; ulCnt++)
	{
		VK_Print("--[XMGR_SEARCH_INFO_GetCscDownloadCSCRev] SC ID[%04x] [%04x]\n", usSCID, s_pstCscDownloadData->astRevs[ulCnt].usCASytemID);

		if(usSCID == s_pstCscDownloadData->astRevs[ulCnt].usCASytemID)
		{
			VK_Print("--[XMGR_SEARCH_INFO_GetCscDownloadCSCRev] TextChar[%s] [%d]\n", s_pstCscDownloadData->astRevs[ulCnt].aucTextChar, s_pstCscDownloadData->astRevs[ulCnt].ucTextLen);

			VK_strncpy(pucRev, s_pstCscDownloadData->astRevs[ulCnt].aucTextChar, s_pstCscDownloadData->astRevs[ulCnt].ucTextLen);
			break;
		}
	}

	return ERR_OK;
}


HERROR XMGR_SEARCH_INFO_ClearCscDownloadInfo(void)
{
	if(s_pstCscDownloadData != NULL)
	{
		OxAP_Free (s_pstCscDownloadData);
		s_pstCscDownloadData = NULL;
	}

	return ERR_OK;
}
#endif


/*********************** End of File ******************************/


