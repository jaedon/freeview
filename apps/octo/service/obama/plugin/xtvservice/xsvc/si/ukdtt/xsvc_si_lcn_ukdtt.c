/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_lcn_UkDtt.c
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
#include <xdb_svc.h>
#include <db_param.h>

#include <svc_si.h>
#include <_svc_si.h>
#include <_xsvc_si.h>
#include <_xsvc_si_lib_lcn_base.h>

#include <_xsvc_si_spec_ukdtt.h>
/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define CHECK_OVERLAY_AREA	(1)


#define UK_LCNZONE_ASSIGNMENT_START			(1)
#define UK_LCNZONE_ASSIGNMENT_END				(799)
#define UK_LCNZONE_REGIONAL_START				(800)
#define UK_LCNZONE_REGIONAL_END				(899)
#define UK_LCNZONE_ANALOG_START				(900)
#define UK_LCNZONE_ANALOG_END					(999)
#define UK_LCNZONE_OVERFLOW_AREA_START		(1000)
#define UK_LCNZONE_OVERFLOW_AREA_END			(1999)

#define UK_LCNZONE_REGIONAL_MAX_NUM			(UK_LCNZONE_REGIONAL_END - UK_LCNZONE_REGIONAL_START+1)

#define LOCAL_MACRO_IS_ASSIGNMENT_ZONE(usLcn) 			((usLcn >= UK_LCNZONE_ASSIGNMENT_START && usLcn <= UK_LCNZONE_ASSIGNMENT_END) ? TRUE : FALSE)
#define LOCAL_MACRO_IS_REGIONAL_ZONE(usLcn) 			((usLcn >= UK_LCNZONE_REGIONAL_START && usLcn <= UK_LCNZONE_REGIONAL_END) ? TRUE : FALSE)
#define LOCAL_MACRO_IS_ANALOG_ZONE(usLcn) 				((usLcn >= UK_LCNZONE_ANALOG_START && usLcn <= UK_LCNZONE_ANALOG_END) ? TRUE : FALSE)

#define UK_TER_SIGNAL_QUALITY_THRESHOLD			5
#define UK_TER_SIGNAL_LEVEL_THRESHOLD				5

#define INVALID_VALUE								0xFFFFFFFF
#define CONFLICT_LIST_DISPLAYABLE_OBJECT_WINDOW		6 // Max List 갯수
#define MAX_CONFLICT_SVC_NAME						128
#define REMOVE_NOT_VAILD_LCN				1

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eUK_LCNZONE_Assignment,
	eUK_LCNZONE_Regional,
	eUK_LCNZONE_Analog,
	eUK_LCNZONE_Max,
} xsvcSi_LCNZONE_t;

typedef enum
{
	eTRegion_Only_InOrgSvc = 0,
	eTRegion_Only_InNewSvc,
	eTRegion_InBoth,
	eTRegion_InNone
} xsvcSi_TRegion_Status;

typedef  struct
{
	HUINT16		usTsId;
	HUINT16		usOnId;
	HUINT16		usServiceId;
	HUINT8		ucVisibleServiceFlag;
	HUINT16		usLogicalChannelNumber;
	HUINT8		ucNumberSelectFlag;
	HUINT16		usLogicalChannelNumberSub;
} xsvcSi_LcnInfo_t;

typedef struct tagSvcUniqueData
{
	HUINT16		usTsIdx;
	HUINT16		usSvcId;
} xsvcSi_SvcUniqueData_t;

typedef struct tagConflictServiceList
{
	HUINT32					 ulSvcNum, ulArrayLen;
	HUINT16			 		usLcn;
	xsvcSi_SvcUniqueData_t		*pstUniqDataArray;
} xsvcSi_ConflictServiceList_t;

typedef struct tagConflictServiceTable
{
	HUINT32			 		 ulTblNum, ulArrayLen;
	xsvcSi_ConflictServiceList_t		*pstArray;
} xsvcSi_ConflictServiceTable_t;

typedef struct tagConflictListContent
{
	HUINT16			usUiIdx;					/* Confilict숫자에 의해 결정 되는 UI index*/
	HUINT32			ulConflictCnt;				/* Conflict가 난 Channel pair의 수 */
	HUINT32			ulCurConflictIdx;			/* Conflict중인 Index, Country(0),Primary(1),Secondary(2),Teritary(3) */
											/* 이 값이 4일 경우 TRD로 Conflict를 결정하지 않는다. */
	HUINT32			ulConflictTrdCnt;			/* */

	HUINT8			szSelCountryCode[4];
	HUINT32			ulSelPrRgCode;
	HUINT32			ulSelSeRgCode;
	HUINT32			ulSelTrRgCode;

	HUINT8			szConflictCountryCode[DxMAX_T_REGION_NUM][4];
	HUINT32			ulConflictPrRgCode[DxMAX_T_REGION_NUM];
	HUINT32			ulConflictSeRgCode[DxMAX_T_REGION_NUM];
	HUINT32			ulConflictTrRgCode[DxMAX_T_REGION_NUM];
} xsvcSi_ConflictListContent_t;



/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
SIxPrivateDataSpecDes_t	*pstPrivateDataSpecDes = NULL;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HBOOL		s_bRegionalLcnStatus[UK_LCNZONE_REGIONAL_MAX_NUM];
STATIC HUINT16		s_usOverFlowAreaNum ;

STATIC xsvcSi_ConflictServiceTable_t		s_stUkConflictServiceTable;
STATIC HUINT8								szTRDContent[DxMAX_T_REGION_NAME_LEN*3+4]; // 3 TRD and 1 Country Code.
STATIC xsvcSi_ConflictListContent_t			s_stConflictListContents;	/**< TRD UI Contents. */



#define ___LOCAL_FUNCTIONS___
STATIC HUINT32 xsvc_si_LcnGetTrdConflictedSvcCount_UkDtt(void);
STATIC void xsvc_si_LcnInitTrdConflictedInfo_UkDtt(xsvcSi_ConflictListContent_t *pstConflictContents);
STATIC HERROR xsvc_si_LcnGetTrdconflictedInfo_UkDtt( DbSvc_Info_t *pstSvcInfo , DbSvc_T_RegionInfo_t *pstTRgInfo);
STATIC HERROR xsvc_si_LcnGetTrdConflictSvcHandleTable_UkDtt(HUINT32 ulIndex, HUINT16 *pusLcn, HUINT32 *pulCount, void **pUniqDataList);
STATIC HERROR xsvc_si_LcnIsSignalStrongerThanDbTsInfo_UkDtt (DbSvc_TsInfo_t *pstNewTsInfo, DbSvc_TsInfo_t *pstDbTsInfo, HBOOL *pbStronger);
STATIC HERROR xsvc_si_LcnGetTrdConflictedCount_UkDtt(HUINT32 *pulConflictTrdCnt);


#define ___MEMBER_FUNCTIONS___

#define ___API_FUNCTIONS___
/****************************************************************************
Description        :
Parameters        :
Output             :
Return              :
****************************************************************************/

STATIC	xsvcSi_ConflictListContent_t *xsvc_si_LcnGetConflictContent(void)
{
	return &s_stConflictListContents;
}


STATIC void xsvc_si_LcnInitAsnMark(void)
{
	HERROR           	 	hError = ERR_FAIL;
	HINT32			nSvcCount = 0;
	HUINT32			ulIndex;
	DbSvc_Condition_t	stSvcCond;
	DbSvc_Info_t		*pstSvcInfoList = NULL;

	DB_SVC_InitSvcFindCondition(&stSvcCond);
	stSvcCond.nOrgDeliType	= eDxDELIVERY_TYPE_TER;
	stSvcCond.nStartLcn 	= UK_LCNZONE_REGIONAL_START;
	stSvcCond.nEndLcn 		= UK_LCNZONE_REGIONAL_END;
	stSvcCond.nSvcType 	= eDxSVC_TYPE_All;

	hError = DB_SVC_FindSvcInfoList((DbSvc_Condition_t*)&stSvcCond,&nSvcCount, &pstSvcInfoList);
	if(hError != ERR_OK|| nSvcCount == 0 ||pstSvcInfoList == NULL )
	{
		HxLOG_Warning("DB_SVC_FindSvcInfoList Error \n");
	}

	HxLOG_Print("Regional Svc Area's -- SvcCount(%d) , pstSvcInfoList(%p) \n", nSvcCount, pstSvcInfoList);

	HxSTD_memset(s_bRegionalLcnStatus, 0x00, UK_LCNZONE_REGIONAL_MAX_NUM*sizeof(HBOOL));

	for (ulIndex = 0; ((ulIndex < nSvcCount) &&( pstSvcInfoList!= NULL)) ; ulIndex++)
	{
		HxLOG_Print("ulIndex[%03d] ,  Lcn[%03d: 0x%04x]\n",ulIndex, DbSvc_GetLcn(&pstSvcInfoList[ulIndex]), DbSvc_GetLcn(&pstSvcInfoList[ulIndex]));

		if(LOCAL_MACRO_IS_REGIONAL_ZONE(DbSvc_GetLcn(&pstSvcInfoList[ulIndex])))
			s_bRegionalLcnStatus[(DbSvc_GetLcn(&pstSvcInfoList[ulIndex]) - UK_LCNZONE_REGIONAL_START)] = TRUE;
	}

	s_usOverFlowAreaNum = 0;

	if(pstSvcInfoList)
	{
		DB_SVC_FreeServiceInfoList(NULL,NULL, pstSvcInfoList);
	}
}


/***************************************************************************************
	FILE:    		xsvc_si_LcnGetEmptyNumberInLcnZone
	Description: 	DB에서 start, end 번호 사이에서 제일 큰 번호를 가지고 오는 함수
	Module:
	Remarks :

***********************************************************************************************/
STATIC HINT32 xsvc_si_LcnGetEmptyNumberInLcnZone(xsvcSi_LCNZONE_t eLcnZone)
{
	HINT32	nLcn = 0;
	HINT32	uIdx;

	HxLOG_Print("eLcnZone : 0x%X \n", eLcnZone);
	switch(eLcnZone)
	{
		case eUK_LCNZONE_Regional:
			for(uIdx=0; uIdx < UK_LCNZONE_REGIONAL_MAX_NUM; uIdx++)
			{
				HxLOG_Print("s_bRegionalLcnStatus[%d]:0x%X\n",uIdx,s_bRegionalLcnStatus[uIdx]);
				if(!s_bRegionalLcnStatus[uIdx])
				{
					break;
				}
			}
			HxLOG_Print("\tuIdx:%d\n", uIdx);
			if( uIdx < UK_LCNZONE_REGIONAL_MAX_NUM)
			{
				s_bRegionalLcnStatus[uIdx] = TRUE;
				nLcn = uIdx + UK_LCNZONE_REGIONAL_START;
			}
			else
			{
				DB_SVC_GetMaxLcnInLcnInInterval(eDxDELIVERY_TYPE_TER, eDxSVC_TYPE_All, UK_LCNZONE_OVERFLOW_AREA_START, UK_LCNZONE_OVERFLOW_AREA_END, &nLcn);
				HxLOG_Print("\tnLcn:%d s_usOverFlowAreaNum:%d\n", nLcn,s_usOverFlowAreaNum);
				if(!s_usOverFlowAreaNum)
				{
					if(!nLcn)
						nLcn =  UK_LCNZONE_OVERFLOW_AREA_START;
					s_usOverFlowAreaNum = ( nLcn +1);
				}
				else
				{
					nLcn = s_usOverFlowAreaNum;
					s_usOverFlowAreaNum++;
				}
			}
			break;
		case eUK_LCNZONE_Analog:
			DB_SVC_GetMaxLcnInLcnInInterval(eDxDELIVERY_TYPE_TER, eDxSVC_TYPE_All, UK_LCNZONE_ANALOG_START, UK_LCNZONE_ANALOG_END, &nLcn);
			nLcn++;
			break;
		default:
			break;
	}

	HxLOG_Print("\tnLcn : 0x%X[%d] \n", nLcn,nLcn);
	HxLOG_Print("[xsvc_si_LcnGetEmptyNumberInLcnZone] Leave !\n\r");
	return nLcn;
}

/****************************************************************************
Description	:
Parameters	:
Output		:
Return		:
****************************************************************************/
STATIC HBOOL xsvc_si_LcnSetToRegionalArea(HUINT16 *pusLcn)
{
	HINT32		nEmptyLcnInZone;

	HxLOG_Print("[xsvc_si_LcnSetToRegionalArea] Enter!\n\r");

	nEmptyLcnInZone = xsvc_si_LcnGetEmptyNumberInLcnZone(eUK_LCNZONE_Regional);
	HxLOG_Print ("\tnEmptyLcnInZone : 0x%X \n", nEmptyLcnInZone);
	if(nEmptyLcnInZone)
	{
		*pusLcn = (HUINT16)nEmptyLcnInZone;
		HxLOG_Print("[xsvc_si_LcnSetToRegionalArea] Leave!\n\r");
		return TRUE;
	}
	else
	{
		HxLOG_Print("\t\t--> regional services fail\n");
		HxLOG_Print("[xsvc_si_LcnSetToRegionalArea] Leave Error!\n\r");
		return FALSE;
	}

	HxLOG_Print("[xsvc_si_LcnSetToRegionalArea] Leave Error!\n\r");

	return FALSE;
}


/****************************************************************************
Description	:
Parameters	:
Output		:
Return		:
****************************************************************************/
STATIC HBOOL xsvc_si_LcnCheckExistanceTRegion(DbSvc_Info_t *pstSvcInfo , HUINT32 *pulFrom)
{
	HUINT32			i,ulCnt;
	HBOOL			bFound=FALSE;
	DbSvc_TsInfo_t		stTsInfo;
	DbSvc_NetInfo_t	stNetInfo;
	DbSvc_T_RegionInfo_t	*pstTRgInfo;

	ulCnt = 0;
	for ( i = 0 ; i < DxMAX_T_REGION_NUM ; i++ )
	{
		pstTRgInfo = XdbSvc_TvAnyTimeGetRegionInfo(pstSvcInfo, i);

		if ( 0xFF != pstTRgInfo->ucRegionDepth )
		{
			bFound = TRUE;
			ulCnt++;
		}
	}
	if ( TRUE == bFound)
	{
		*pulFrom = 0; // Svc Level
		return bFound;
	}

	ulCnt = 0;
	HxSTD_memset(&stTsInfo, 0x00, sizeof(DbSvc_TsInfo_t));

	DB_SVC_GetTsInfo (DbSvc_GetTsIdx(pstSvcInfo), &stTsInfo);
	for ( i = 0 ; i < DxMAX_T_REGION_NUM ; i++ )
	{
		pstTRgInfo = &stTsInfo.utExt.stTvAnyTime.stRegionInfo[i];

		if ( 0xFF != pstTRgInfo->ucRegionDepth )
		{
			bFound = TRUE;
			ulCnt++;
		}
	}
	if ( TRUE == bFound)
	{
		*pulFrom = 1; // Ts Level
		return bFound;
	}

	ulCnt = 0;
	HxSTD_memset(&stNetInfo, 0x00, sizeof(DbSvc_NetInfo_t));

	DB_SVC_GetNetworkInfo (stTsInfo.usNetIdx, &stNetInfo);
	for ( i = 0 ; i < DxMAX_T_REGION_NUM ; i++ )
	{
		pstTRgInfo = &stNetInfo.utExt.stTvAnyTime.stRegionInfo[i];

		if ( 0xFF != pstTRgInfo->ucRegionDepth )
		{
			bFound = TRUE;
			ulCnt++;
		}
	}

	if ( TRUE == bFound)
	{
		*pulFrom = 2; // Network Level
		return bFound;
	}

	return  FALSE;
}

/****************************************************************************
Description	:
Parameters	:
Output		:
Return		:
****************************************************************************/
STATIC HBOOL xsvc_si_LcnIsDifferentTRegion( DbSvc_T_RegionInfo_t *pstTRgInfo1 , DbSvc_T_RegionInfo_t *pstTRgInfo2 )
{
	HUINT32			i;
	HBOOL			bDifferent = FALSE;
	DbSvc_T_RegionInfo_t *pstTmpTRgInfo1 = NULL , *pstTmpTRgInfo2 = NULL;

	for ( i = 9 ; i < DxMAX_T_REGION_NUM ; i++ )
	{
		pstTmpTRgInfo1 = &pstTRgInfo1[i];
		pstTmpTRgInfo2 = &pstTRgInfo2[i];

		if ( ( 0xFF != pstTmpTRgInfo1->ucRegionDepth ) && ( 0xFF != pstTmpTRgInfo2->ucRegionDepth ) )
		{
			if ( pstTmpTRgInfo1->ulRegionCode != pstTmpTRgInfo1->ulRegionCode)
			{
				bDifferent = TRUE;
			}
		}
		else
		{
			bDifferent = TRUE;
		}
	}

	return bDifferent;
}


/****************************************************************************
Description	:
Parameters	:
Output		:
Return		:
****************************************************************************/
STATIC xsvcSi_TRegion_Status xsvc_si_LcnCheckTRegionCase(DbSvc_Info_t *pstOrgSvc, DbSvc_Info_t *pstNewSvc)
{
	HUINT32			ulFrom;
	HBOOL			bExistOrgSvc=FALSE, bExistNewSvc=FALSE;

	bExistOrgSvc = xsvc_si_LcnCheckExistanceTRegion( pstOrgSvc , &ulFrom );
	bExistNewSvc = xsvc_si_LcnCheckExistanceTRegion( pstNewSvc , &ulFrom );

	if ( ( TRUE == bExistOrgSvc ) && ( TRUE == bExistNewSvc ) )
	{
		return eTRegion_InBoth;
	}
	if ( ( TRUE != bExistOrgSvc ) && ( TRUE != bExistNewSvc ) )
	{
		return eTRegion_InNone;
	}
	if ( ( TRUE == bExistOrgSvc ) && ( TRUE != bExistNewSvc ) )
	{
		return eTRegion_Only_InOrgSvc;
	}
	if ( ( TRUE != bExistOrgSvc ) && ( TRUE == bExistNewSvc ) )
	{
		return eTRegion_Only_InNewSvc;
	}

	return eTRegion_InBoth;
}

STATIC HERROR xsvc_si_LcnAddSvcUniqDataToConflictList (xsvcSi_ConflictServiceList_t *pUniqDataList, HUINT16 usTsIdx, HUINT16 usSvcId, HUINT16 usLcn, HUINT32 *pulSvcIndex)
{
	HUINT32				ulSvcIdx;
	xsvcSi_SvcUniqueData_t	*pstUniqDatarray;

	if (pUniqDataList == NULL || pulSvcIndex == NULL)
	{
		HxLOG_Error("pUniqDataList is NULL or pulSvcIndex is NULL\n");
		return ERR_FAIL;
	}

	HxLOG_Print("UkConflictServiceList_t -- LCN:%03d ulSvcNum:%d ulArrayLen:0x%X pstUniqDataArray:0x%X\n" ,
			pUniqDataList->usLcn , pUniqDataList->ulSvcNum , pUniqDataList->ulArrayLen , (unsigned int)pUniqDataList->pstUniqDataArray );

	for (ulSvcIdx = 0; ulSvcIdx < pUniqDataList->ulSvcNum; ulSvcIdx++ )
	{
		if (pUniqDataList->pstUniqDataArray[ulSvcIdx].usSvcId == usSvcId && pUniqDataList->pstUniqDataArray[ulSvcIdx].usTsIdx == usTsIdx)
		{
			pUniqDataList->usLcn = usLcn;
			*pulSvcIndex = ulSvcIdx;
			return ERR_OK;
		}
	}

	if (pUniqDataList->ulSvcNum >= pUniqDataList->ulArrayLen)
	{
		pstUniqDatarray = (xsvcSi_SvcUniqueData_t *)OxSI_Malloc(sizeof(xsvcSi_SvcUniqueData_t) * (pUniqDataList->ulArrayLen + SvcSi_MAX_NUM_SVC));
		if (pstUniqDatarray == NULL)
		{
			return ERR_FAIL;
		}

		if (pUniqDataList->ulArrayLen != 0)
		{
			HxSTD_memcpy (pstUniqDatarray, pUniqDataList->pstUniqDataArray, sizeof(xsvcSi_SvcUniqueData_t) * pUniqDataList->ulArrayLen);
			OxSI_Free(pUniqDataList->pstUniqDataArray);
		}

		pUniqDataList->pstUniqDataArray = pstUniqDatarray;
		pUniqDataList->ulArrayLen += SvcSi_MAX_NUM_SVC;
	}

	*pulSvcIndex = ulSvcIdx;
	pUniqDataList->usLcn = usLcn;
	pUniqDataList->pstUniqDataArray[ulSvcIdx].usTsIdx = usTsIdx;
	pUniqDataList->pstUniqDataArray[ulSvcIdx].usSvcId = usSvcId;
	pUniqDataList->ulSvcNum++;

	HxLOG_Print("UkConflictServiceList_t -- LCN:%03d ulSvcNum:%d ulArrayLen:0x%X *pulSvcIndex:%d usTsIdx:%d , usSvcId:0x%X\n" ,
			pUniqDataList->usLcn , pUniqDataList->ulSvcNum , pUniqDataList->ulArrayLen , *pulSvcIndex ,  usTsIdx , usSvcId );

	return ERR_OK;
}

STATIC HERROR xsvc_si_LcnAddServiceListToConflictTable (xsvcSi_ConflictServiceTable_t *pSvcTable, xsvcSi_ConflictServiceList_t *pSvcList, HUINT32 *pulSvcIndex)
{
	HUINT32			 ulIdx;
	xsvcSi_ConflictServiceList_t	*pstLcnArray;

	if (pSvcTable == NULL || pSvcList == NULL || pulSvcIndex == NULL)
	{
		HxLOG_Error("pSvcTable is NULL ||  pSvcList is NULL || pulSvcIndex is NULL\n");
		return ERR_FAIL;
	}

	HxLOG_Print("UkConflictServiceTable_t - ulTblNum:%03d ulArrayLen:%d\n" , 
		pSvcTable->ulTblNum , pSvcTable->ulArrayLen );
	HxLOG_Print("UkConflictServiceList_t - LCN:%03d ulSvcNum:%d ulArrayLen:%d\n" , 
		pSvcList->usLcn , pSvcList->ulSvcNum , pSvcList->ulArrayLen);

	for (ulIdx = 0; ulIdx < pSvcTable->ulTblNum; ulIdx++ )
	{
		HxLOG_Print("\tUkConflictServiceTable_t - pSvcTable->pstArray[%d].usLcn:%d\n" , ulIdx ,  pSvcTable->pstArray[ulIdx].usLcn);
		if (pSvcTable->pstArray[ulIdx].usLcn == pSvcList->usLcn)
		{
			pSvcTable->pstArray[ulIdx] = *pSvcList;
			*pulSvcIndex = ulIdx;
			return ERR_OK;
		}
	}

	if (pSvcTable->ulTblNum >= pSvcTable->ulArrayLen)
	{
		pstLcnArray = (xsvcSi_ConflictServiceList_t *)OxSI_Malloc(sizeof(xsvcSi_ConflictServiceList_t) * (pSvcTable->ulArrayLen + SvcSi_MAX_NUM_SVC));
		if (pstLcnArray == NULL)
		{
			return ERR_FAIL;
		}

		if (pSvcTable->ulArrayLen != 0)
		{
			HxSTD_memcpy (pstLcnArray, pSvcTable->pstArray, sizeof(xsvcSi_ConflictServiceList_t) * pSvcTable->ulArrayLen);
			OxSI_Free(pSvcTable->pstArray);
		}

		pSvcTable->pstArray = pstLcnArray;
		pSvcTable->ulArrayLen += SvcSi_MAX_NUM_SVC;
	}

	*pulSvcIndex = ulIdx;
	pSvcTable->pstArray[ulIdx] = *pSvcList;
	pSvcTable->ulTblNum ++;

	return ERR_OK;
}

STATIC HERROR xsvc_si_LcnAddConfilctSvc(HUINT16 usLcn, HUINT16 usTsIdx, HUINT16 usSvcId)
{
	HUINT32					ulSvcIndex, ulCnt;
	xsvcSi_ConflictServiceList_t	stSvcHandleList;
	xsvcSi_ConflictServiceTable_t	*pSvcTable = &s_stUkConflictServiceTable;

	HxSTD_memset((char *)&stSvcHandleList, 0x00, sizeof(xsvcSi_ConflictServiceList_t));

	HxLOG_Print("LCN:%03d TsIdx:%d SvcId:0x%X\n"   , usLcn, usTsIdx , usSvcId );
	HxLOG_Print("ulTblNum:%d ulArrayLen:%d pstArray:0x%X\n", pSvcTable->ulTblNum , pSvcTable->ulArrayLen, (unsigned int)pSvcTable->pstArray );

	if(pSvcTable->ulTblNum == 0)
	{
		xsvc_si_LcnAddSvcUniqDataToConflictList(&stSvcHandleList, usTsIdx, usSvcId, usLcn, &ulSvcIndex);
		xsvc_si_LcnAddServiceListToConflictTable(pSvcTable, &stSvcHandleList, &ulSvcIndex);
	}
	else
	{
		for(ulCnt = 0; ulCnt < pSvcTable->ulTblNum; ulCnt ++)
		{
			if(pSvcTable->pstArray[ulCnt].usLcn == usLcn)
			{
				xsvc_si_LcnAddSvcUniqDataToConflictList(&(pSvcTable->pstArray[ulCnt]), usTsIdx, usSvcId, usLcn, &ulSvcIndex);
				return ERR_OK;
			}
		}

		xsvc_si_LcnAddSvcUniqDataToConflictList(&stSvcHandleList, usTsIdx, usSvcId, usLcn, &ulSvcIndex);
		xsvc_si_LcnAddServiceListToConflictTable(pSvcTable, &stSvcHandleList, &ulSvcIndex);
	}

	return ERR_OK;
}

/****************************************************************************
Description	:
Parameters	:
Output		:
Return		:
****************************************************************************/
STATIC HERROR	xsvc_si_LcnProcessLcnDuplicate(Handle_t hOrgSvc, DbSvc_Info_t *pstOrgSvc, DbSvc_Info_t *pstNewSvc,SRCHSVC_AppendFlag_t *result)
{
	xsvcSi_TRegion_Status	eCase;
	SRCHSVC_AppendFlag_t	eResult = eSRCHSVC_ADD;
	HERROR					hError = ERR_FAIL;
	HUINT16					usLcn = 0;
	DbSvc_TsInfo_t			stNewTsInfo,stOldTsInfo;
	HBOOL					bStronger;

	eCase = xsvc_si_LcnCheckTRegionCase( pstOrgSvc , pstNewSvc);
	switch ( eCase )
	{
		case eTRegion_InBoth:
		case eTRegion_Only_InNewSvc:
			// add conflict two services to conflict list.
			xsvc_si_LcnAddConfilctSvc(DbSvc_GetLcn(pstNewSvc), DbSvc_GetTsIdx(pstNewSvc), DbSvc_GetSvcId(pstNewSvc));
			xsvc_si_LcnAddConfilctSvc(DbSvc_GetLcn(pstNewSvc), DbSvc_GetTsIdx(pstOrgSvc), DbSvc_GetSvcId(pstOrgSvc));

			HxLOG_Print("\tExistSvc SvcId:0x%X NewSvc's SvcId:0x%X\n", DbSvc_GetSvcId(pstOrgSvc), DbSvc_GetSvcId(pstNewSvc));
			(void)xsvc_si_LcnSetToRegionalArea(&usLcn);
			DbSvc_SetLcn(pstNewSvc,usLcn);
			HxLOG_Print("\tWe will assign new regional lcn number:%d DbSvc_GetLcnFlag:%d\n", DbSvc_GetLcn(pstNewSvc),DbSvc_GetLcnFlag(pstNewSvc));
			eResult = eSRCHSVC_ADD;
			hError = ERR_OK;
			break;

		// 아래는 기존 Conflict처리 코드이다.
		// 경우의 수를 위해서 남겨 놓았지만 Target Region정보가 모두 존재하는 경우 아래의 경우로
		// 진입되지 않는다.
		// 아마도 기존의 스트림을 돌리거나 하면 아래 경우가 생길 수 있다.
		case eTRegion_Only_InOrgSvc:
		case eTRegion_InNone:
		default:
			if(DbSvc_GetSvcId(pstOrgSvc) != DbSvc_GetSvcId(pstNewSvc))
			{
				HxLOG_Print("\tExistSvc SvcId:0x%X NewSvc's SvcId:0x%X\n", DbSvc_GetSvcId(pstOrgSvc),DbSvc_GetSvcId(pstNewSvc));
				xsvc_si_LcnSetToRegionalArea(&usLcn);
				DbSvc_SetLcn(pstNewSvc,usLcn);
				HxLOG_Print("\tWe will assign new regional lcn number:%d\n", DbSvc_GetLcn(pstNewSvc));
			}
			else
			{

				// Multiple SIP
				DB_SVC_GetTsInfo(DbSvc_GetTsIdx(pstNewSvc), &stNewTsInfo);
				DB_SVC_GetTsInfo(DbSvc_GetTsIdx(pstOrgSvc), &stOldTsInfo);

				hError = xsvc_si_LcnIsSignalStrongerThanDbTsInfo_UkDtt(&stNewTsInfo, &stOldTsInfo, &bStronger);
				if ( TRUE == bStronger )
				{
					HxLOG_Print("Update Exist Svc by New Service Infomation\n");
					DB_SVC_UpdateServiceInfo (hOrgSvc, pstNewSvc);
					eResult = eSRCHSVC_UPDATE;
				}
				else
				{
					HxLOG_Print("Update Exist Svc by eSRCHSVC_DONTADD\n");
					eResult = eSRCHSVC_DONTADD;
				}
			}
			break;
	}
	*result = eResult;
	return hError;
}


/*---------------------------------------------------------------------------------------------
    UK DTT LCN Service Number Zone - DBOOK7.x
    ----------------------------------------------------------------------------------------
	local_channel_number || Description  							||
	----------------------------------------------------------------------------------------
           0            || Shall not be used 						||
    ----------------------------------------------------------------------------------------
       1 ~ 799          || Broacast									||
    ----------------------------------------------------------------------------------------
       800 ~ 899        || Undefined or regional variant services 	||
    ----------------------------------------------------------------------------------------
       900 ~ 999        || Receiver defined logocal						 	||
    ----------------------------------------------------------------------------------------
       1000+	        || Shall not be used 							 	||
    ----------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------*/

STATIC HERROR xsvc_si_LcnGetLogicalChannelFromNit_UkDtt (HUINT32 ulActionId, HxList_t **ppChList)
{
	HxList_t					*pstItem1, *pstItem2;
	SIxTable_PsiTable_t			*pstNitTable = NULL, *pstSdtTable = NULL;
	HERROR						hResult = ERR_FAIL;
	HxList_t 					*pChList = NULL;
	HBOOL						bUkCheckedPrSpecifier = FALSE;
	HBOOL						bFoundLCD = FALSE,bFoundSAD = FALSE;//,bFoundValidSAD = FALSE;
	HUINT32						ulNumAttr = 0,ulattrCnt;

	SIxUkdttSvcAttrInfo_t		*pstSvcAttrInfoArray = NULL, *pstSvcAttrInfo = NULL;
	HUINT32						ulLcnNum = 0;
	HUINT16						*paProcessedSvcId = NULL;
	HUINT32						ulPrivateSpecifyValue = PRIVATE_DATA_SPECIFIER_UNDEFINED;
	HUINT16						usCurTsId = 0, usCurOnId = 0;

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

	pstSdtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtAct, SILIB_TABLE_EXT_ID_ALL);
	if(pstSdtTable == NULL)
	{
		HxLOG_Error("pstSdtTable is NULL\n");
		goto END_FUNC;
	}

	HxLOG_Print("============== Enter (+) \n");

	usCurTsId = pstSdtTable->unInfo.stSdt.usTsId;
	usCurOnId = pstSdtTable->unInfo.stSdt.usOrgNetId;

	/************************************************************
		Service Attribute Descriptor 와 Logical Channel Descriptor 를 처리함..
	*************************************************************/
	HxLOG_Print("===== Process Service Attribute Descriptor  &  Logical Channel Descriptor =========\n");

	for (pstItem1 = pstNitTable->pst2ndLoop; NULL != pstItem1; pstItem1 = pstItem1->next)
	{
		SIxTable_2ndLoop_t *pst2ndLoop = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstItem1);

		if(usCurTsId != pst2ndLoop->unInfo.stNit.usTsId
			|| usCurOnId != pst2ndLoop->unInfo.stNit.usOrgNetId)
		{
			continue;
		}

		HxLOG_Print("TsId & OnId is same !! SDT : NIT = ( 0x%x, 0x%x : 0x%x, 0x%x )\n", usCurTsId, usCurOnId, pst2ndLoop->unInfo.stNit.usTsId, pst2ndLoop->unInfo.stNit.usOrgNetId);

		for (pstItem2 = pst2ndLoop->pstDesLoop; NULL != pstItem2; pstItem2 = pstItem2->next)
		{
			HUINT8 *pucDesRaw = (HUINT8 *)HLIB_LIST_Data (pstItem2);

			switch (*pucDesRaw)
			{
				case eSIxDESCTAG_PRIVATE_DATA_SPECIFIER:
					pstPrivateDataSpecDes = (SIxPrivateDataSpecDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);
					// To check the Private Data Specifier for UK

					hResult = svc_si_GetPrivateSpecifyValue((HUINT32)eSVCSI_ACT_SPEC_UkDtt,&ulPrivateSpecifyValue);
					if(hResult != ERR_OK)
					{
						ulPrivateSpecifyValue = PRIVATE_DATA_SPECIFIER_UNDEFINED;
					}

					if (pstPrivateDataSpecDes->ulPrivateDataSpec != ulPrivateSpecifyValue)
					{
						bUkCheckedPrSpecifier = FALSE;
					}
					else
					{
						bUkCheckedPrSpecifier = TRUE;
					}
					PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstPrivateDataSpecDes);
					break;

				case eSIxDESCTAG_PRIVATE_DATA_INDICATOR:
					bUkCheckedPrSpecifier = FALSE;
					break;

				case eSIxDESCTAG_UKDTT_LOGICAL_CHANNEL:
					if(( FALSE == bFoundLCD ) && (TRUE == bUkCheckedPrSpecifier ))
					{
						SIxUkdttLogicalChannelDes_t *pstLogicChDes = (SIxUkdttLogicalChannelDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);
						if (NULL != pstLogicChDes)
						{
							SIxUkdttLogicalChannelInfo_t *pChListItem = pstLogicChDes->pstInfo;

							bFoundLCD = TRUE;

							while(pChListItem)
							{
								xsvcSi_LcnInfo_t *pstLcnInfo = NULL;

								pstLcnInfo = (xsvcSi_LcnInfo_t*)OxSI_Malloc(sizeof(xsvcSi_LcnInfo_t));
								if (pstLcnInfo != NULL)
								{
									pstLcnInfo->usTsId = pst2ndLoop->unInfo.stNit.usTsId;
									pstLcnInfo->usOnId = pst2ndLoop->unInfo.stNit.usOrgNetId;

									pstLcnInfo->usServiceId 			= pChListItem->usServiceId;
									pstLcnInfo->ucVisibleServiceFlag 		= pChListItem->ucVisibleServiceFlag;
									pstLcnInfo->usLogicalChannelNumber 	= pChListItem->usLogicalChannelNumber;
									pstLcnInfo->ucNumberSelectFlag = 1;

									if(pstSvcAttrInfoArray != NULL)
									{
										for(ulattrCnt = 0; ulattrCnt < ulNumAttr; ulattrCnt++)
										{
											if(pstLcnInfo->usServiceId == pstSvcAttrInfoArray[ulattrCnt].usServiceId)
											{
												pstLcnInfo->ucVisibleServiceFlag = (HUINT8)pstSvcAttrInfoArray[ulattrCnt].bVisibleSvcFlag;
												pstLcnInfo->ucNumberSelectFlag = (HUINT8)pstSvcAttrInfoArray[ulattrCnt].bNumericSelectFlag;
											}
										}
									}

									HxLOG_Print("LCN Info (0x%x:0x%x:0x%x) VisibleServiceFlag (%d) ucNumberSelectFlag(%d) usLogicalChannelNumber(%d)\n",
										pstLcnInfo->usOnId, pstLcnInfo->usTsId, pstLcnInfo->usServiceId, pstLcnInfo->ucVisibleServiceFlag, pstLcnInfo->ucNumberSelectFlag, pstLcnInfo->usLogicalChannelNumber);

									ulLcnNum++;

									pChList = HLIB_LIST_Append(pChList, (void*)pstLcnInfo);
								}

								pChListItem = pChListItem->pstNext;
							}
							PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstLogicChDes);
						}
					}
					break;
				case eSIxDESCTAG_UKDTT_SERVICE_ATTRIBUTE:
					if(( FALSE == bFoundSAD ) && (TRUE == bUkCheckedPrSpecifier ))
					{
						xsvcSi_LcnInfo_t 		*pstLcnInfo = NULL;
						SIxUkdttSvcAttrDes_t 	*pstSvcAttrDes = (SIxUkdttSvcAttrDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);

						if (pstSvcAttrDes != NULL)
						{
							bFoundSAD = TRUE;

							HxLOG_Debug("Service count from Service Attribute descriptor = %d\n", pstSvcAttrDes->ulNumAttrInfo);

							if(pstSvcAttrInfoArray != NULL)
								OxSI_Free(pstSvcAttrInfoArray);

							ulNumAttr = pstSvcAttrDes->ulNumAttrInfo;

							pstSvcAttrInfoArray = (SIxUkdttSvcAttrInfo_t *)OxSI_Calloc(sizeof(SIxUkdttSvcAttrInfo_t)*ulNumAttr);
							if(pstSvcAttrInfoArray != NULL)
							{
								HxList_t			*pCurLcnList = NULL;

								ulattrCnt =0 ;

								for (pstSvcAttrInfo = pstSvcAttrDes->pstInfo; NULL != pstSvcAttrInfo; pstSvcAttrInfo = pstSvcAttrInfo->pstNext)
								{
									HxSTD_memcpy(&(pstSvcAttrInfoArray[ulattrCnt]),pstSvcAttrInfo,sizeof(SIxUkdttSvcAttrInfo_t));

									pCurLcnList = pChList;

									while(pCurLcnList)
									{
										pstLcnInfo	= (xsvcSi_LcnInfo_t *)HLIB_LIST_Data(pCurLcnList);

										if(pstLcnInfo->usServiceId == pstSvcAttrInfoArray[ulattrCnt].usServiceId)
										{
											pstLcnInfo->ucVisibleServiceFlag = (HUINT8)pstSvcAttrInfoArray[ulattrCnt].bVisibleSvcFlag;
											pstLcnInfo->ucNumberSelectFlag = (HUINT8)pstSvcAttrInfoArray[ulattrCnt].bNumericSelectFlag;
										}

										pCurLcnList = HLIB_LIST_Next(pCurLcnList);

										if(pCurLcnList == pChList)
											break;
									}

		 							HxLOG_Debug("Service Attribute: SVC ID = 0x%x, bVisibleSvcFlag = %d, bNumericSelectFlag = %d\n",
										 pstSvcAttrInfoArray[ulattrCnt].usServiceId, pstSvcAttrInfoArray[ulattrCnt].bVisibleSvcFlag, pstSvcAttrInfoArray[ulattrCnt].bNumericSelectFlag);

									ulattrCnt++;

								}
							}

							PAL_SIRAWPOOL_FreeDes((SIxDescriptor_t *)pstSvcAttrDes);
						}
					}
				break;
			default:
				break;
			}
		}
	}

	bUkCheckedPrSpecifier = FALSE;

	/************************************************************
		HD SimulCast Logical Descriptor 르 처리함...
	*************************************************************/
	HxLOG_Print("===== Process HD SimulCast Logical Descriptor =========\n");

	for (pstItem1 = pstNitTable->pst2ndLoop; NULL != pstItem1; pstItem1 = pstItem1->next)
	{
		SIxTable_2ndLoop_t *pst2ndLoop = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstItem1);

		if(usCurTsId != pst2ndLoop->unInfo.stNit.usTsId
			|| usCurOnId != pst2ndLoop->unInfo.stNit.usOrgNetId)
		{
			continue;
		}

		HxLOG_Print("TsId & OnId is same !! SDT : NIT = ( 0x%x, 0x%x: 0x%x, 0x%x )\n", usCurTsId, usCurOnId, pst2ndLoop->unInfo.stNit.usTsId, pst2ndLoop->unInfo.stNit.usOrgNetId);

		for (pstItem2 = pst2ndLoop->pstDesLoop; NULL != pstItem2; pstItem2 = pstItem2->next)
		{
			HUINT8 *pucDesRaw = (HUINT8 *)HLIB_LIST_Data (pstItem2);

			switch (*pucDesRaw)
			{
				case eSIxDESCTAG_PRIVATE_DATA_SPECIFIER:
					pstPrivateDataSpecDes = (SIxPrivateDataSpecDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);

					// To check the Private Data Specifier for UK
					hResult = svc_si_GetPrivateSpecifyValue((HUINT32)eSVCSI_ACT_SPEC_UkDtt,&ulPrivateSpecifyValue);
					if(hResult != ERR_OK)
					{
						ulPrivateSpecifyValue = PRIVATE_DATA_SPECIFIER_UNDEFINED;
					}

					if (pstPrivateDataSpecDes->ulPrivateDataSpec != ulPrivateSpecifyValue)
					{
						bUkCheckedPrSpecifier = FALSE;
					}
					else
					{
						bUkCheckedPrSpecifier = TRUE;
					}
					PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstPrivateDataSpecDes);
					break;
				case eSIxDESCTAG_PRIVATE_DATA_INDICATOR:
					bUkCheckedPrSpecifier = FALSE;
					break;
				case eSIxDESCTAG_UKDTT_HD_SIMULCAST_LOGICAL_CHANNEL:
					if ( ( TRUE == bUkCheckedPrSpecifier ))
					{
						SIxUkdttHDSimulcastLcnDes_t *pstHdUkSimulcastLcnDes = (SIxUkdttHDSimulcastLcnDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);
						if (pstHdUkSimulcastLcnDes != NULL)
						{
							HBOOL		bFound = FALSE, bAlreadyProcessed = FALSE;
							HUINT16 		usHdLcnNum,usHdSvcId,usMaxNum,usProcessIdx;
							HUINT8		ucHdVisibleFlag;
							HUINT32	j;
							HxList_t			*pCurLcnList = NULL;
							SIxUkdttHDSimulLcnInfo_t	*pstHdUkSimulcastLcnInfo;

							HxLOG_Print("Service count from HD Simulcast Logical Descriptor  = %d\n", pstHdUkSimulcastLcnDes->ulNumLcn);

							usProcessIdx = 0;
							usMaxNum = (pstHdUkSimulcastLcnDes->ulNumLcn > ulLcnNum) ? pstHdUkSimulcastLcnDes->ulNumLcn : ulLcnNum;
							paProcessedSvcId = (HUINT16 *)OxSI_Malloc(usMaxNum*sizeof(HUINT16));
							HxSTD_memset(paProcessedSvcId,0xff,usMaxNum*sizeof(HUINT16));

							for (pstHdUkSimulcastLcnInfo = pstHdUkSimulcastLcnDes->pstInfo; NULL != pstHdUkSimulcastLcnInfo; pstHdUkSimulcastLcnInfo = pstHdUkSimulcastLcnInfo->pstNext)
							{
								HUINT16			usTmpLcn=0xFFFF;
								HBOOL			bTmpVisibleSvcFlag=FALSE;
								HUINT16 			usTmpSvcId=0xFFFF;

								usHdLcnNum 		= pstHdUkSimulcastLcnInfo->usLcnNumber;
								usHdSvcId 		= pstHdUkSimulcastLcnInfo->usSvcId;
								ucHdVisibleFlag 	= pstHdUkSimulcastLcnInfo->bVisibleSvcFlag;

								HxLOG_Print("HdSvcId (0x%x) HdLcnNum(%d) HdVisibleFlag(%d) \n", usHdSvcId, usHdLcnNum, ucHdVisibleFlag);
								bFound = FALSE;

								/* This Process service's ignore function followed by D-BOOK 6.1
									8.5.3.6.2 Rules for the HD simulcast logical channel descriptor 's below rule.
									6.  If receivers subsequently find an entry in the HD simulcast logical channel descriptor
									     for the replaced service, this entry shall be ignored. */
								for(j = 0; j < (usProcessIdx + 1); j++)
								{
									if (usHdSvcId == paProcessedSvcId[j])
									{
										bAlreadyProcessed = TRUE;
										break;
									}
								}

								if (TRUE == bAlreadyProcessed)
									continue;

								paProcessedSvcId[usProcessIdx] = usHdSvcId;
								usProcessIdx++;

								/* set LCN */
								pCurLcnList = pChList;
								while(pCurLcnList)
								{
									xsvcSi_LcnInfo_t	*pstLocalLcnInfo = NULL;

									pstLocalLcnInfo = (xsvcSi_LcnInfo_t *)HLIB_LIST_Data(pCurLcnList);
									if(usHdSvcId == pstLocalLcnInfo->usServiceId)
									{
										// Found Same Service Id, Change LCN number
										usTmpLcn 			= pstLocalLcnInfo->usLogicalChannelNumber;
										bTmpVisibleSvcFlag		= (HBOOL)pstLocalLcnInfo->ucVisibleServiceFlag;
										usTmpSvcId 			= pstLocalLcnInfo->usServiceId;

										pstLocalLcnInfo->usLogicalChannelNumber 	= usHdLcnNum;
										pstLocalLcnInfo->ucVisibleServiceFlag 		= ucHdVisibleFlag;

										HxLOG_Print("==> [SearchLcn()] Change HD LCN - usSvcId = 0x%x, LCN : %d, bVisibleSvcFlag : %d\n",
											usHdSvcId, usHdLcnNum,ucHdVisibleFlag);

										bFound = TRUE;
									}

									pCurLcnList = HLIB_LIST_Next(pCurLcnList);

									if(pCurLcnList == pChList)
										break;
								}

								if (FALSE == bFound)
								{
									xsvcSi_LcnInfo_t *pstLcnInfo = NULL;

									pstLcnInfo = (xsvcSi_LcnInfo_t*)OxSI_Malloc(sizeof(xsvcSi_LcnInfo_t));

									HxSTD_MemSet(pstLcnInfo, 0, sizeof(xsvcSi_LcnInfo_t));
									pstLcnInfo->usServiceId 			= usHdSvcId;
									pstLcnInfo->usLogicalChannelNumber 	= usHdLcnNum;
									pstLcnInfo->ucVisibleServiceFlag 		= ucHdVisibleFlag;

									HxLOG_Print("==> [SearchLcn()] New HD LCN -  usSvcId = 0x%x, LCN : %d, bVisibleSvcFlag : %d\n", usHdSvcId,usHdLcnNum,ucHdVisibleFlag);

									ulLcnNum++;

									// be care full , this point have to be debugging.
									pCurLcnList = HLIB_LIST_Append(pCurLcnList, (void*)pstLcnInfo);


								}
								else
								{
									HxLOG_Print("usTmpSvcId:0x%X,nTmpLcn:%d,bTmpVisibleSvcFlag:0x%X\n",usTmpSvcId, usTmpLcn, bTmpVisibleSvcFlag);

									pCurLcnList = pChList;
									while(pCurLcnList)
									{
										xsvcSi_LcnInfo_t	*pstLocalLcnInfo = NULL;

										pstLocalLcnInfo = (xsvcSi_LcnInfo_t *)HLIB_LIST_Data(pCurLcnList);
										HxLOG_Print("LcnNum:%d SvcId:0x%X\n",pstLocalLcnInfo->usLogicalChannelNumber,pstLocalLcnInfo->usServiceId);

										if ((usHdLcnNum == pstLocalLcnInfo->usLogicalChannelNumber) && (usHdSvcId != pstLocalLcnInfo->usServiceId))
										{
											HxLOG_Print("==> [SearchLcn()] Previous Assign LCN change (%d)->(%d)\n", pstLocalLcnInfo->usLogicalChannelNumber,usTmpLcn);
											pstLocalLcnInfo->usLogicalChannelNumber 	= usTmpLcn;
											paProcessedSvcId[usProcessIdx] 			= pstLocalLcnInfo->usServiceId;
											usProcessIdx++;
										}
										else if ((usHdLcnNum == pstLocalLcnInfo->usLogicalChannelNumber) && (usHdSvcId == pstLocalLcnInfo->usServiceId))
										{
											pstLocalLcnInfo->usLogicalChannelNumberSub = usTmpLcn;
										}
										pCurLcnList = HLIB_LIST_Next(pCurLcnList);

										if(pCurLcnList == pChList)
											break;
									}
								}
							}
							PAL_SIRAWPOOL_FreeDes((SIxDescriptor_t *)pstHdUkSimulcastLcnDes);
							if ( NULL != paProcessedSvcId )
								OxSI_Free(paProcessedSvcId);
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
	if (NULL != pstNitTable)		{ PAL_SIRAWPOOL_FreeTable (pstNitTable); }
	if (NULL != pstSdtTable)		{ PAL_SIRAWPOOL_FreeTable (pstSdtTable); }
	if(NULL != pstSvcAttrInfoArray)	{OxSI_Free(pstSvcAttrInfoArray);}

	HxLOG_Print("============== Leave (-) \n");

	return hResult;
}

STATIC HERROR xsvc_si_LcnIsSignalStrongerThanDbTsInfo_UkDtt (DbSvc_TsInfo_t *pstNewTsInfo, DbSvc_TsInfo_t *pstDbTsInfo, HBOOL *pbStronger)
{
	DbSvc_TerTuningInfo_t	*pstNewTuneInfo, *pstDbTuneInfo;

	pstNewTuneInfo	= &(pstNewTsInfo->stTuningParam.ter);
	pstDbTuneInfo		= &(pstDbTsInfo->stTuningParam.ter);

	// Quality 가 높으면 TRUE
	HxLOG_Debug ("\t Qulity Check (%d, %d)\n", pstNewTuneInfo->ucQuality, (pstDbTuneInfo->ucQuality + UK_TER_SIGNAL_QUALITY_THRESHOLD));

	if ((HUINT32)pstNewTuneInfo->ucQuality > ((HUINT32)pstDbTuneInfo->ucQuality + UK_TER_SIGNAL_QUALITY_THRESHOLD))
	{
		*pbStronger = TRUE;
		return ERR_OK;
	}

	// Quality 가 낮으면 FALSE
	if (((HUINT32)pstNewTuneInfo->ucQuality + UK_TER_SIGNAL_QUALITY_THRESHOLD) < (HUINT32)pstDbTuneInfo->ucQuality)
	{
		*pbStronger = FALSE;
		return ERR_OK;
	}

	// Quality가 거의 동일
	// Level 이 높으면 TRUE
	HxLOG_Debug ("\t Level Check (%d, %d)\n",pstNewTuneInfo->ucLevel, (pstDbTuneInfo->ucLevel + UK_TER_SIGNAL_LEVEL_THRESHOLD));
	if ((HUINT32)pstNewTuneInfo->ucLevel > ((HUINT32)pstDbTuneInfo->ucLevel + UK_TER_SIGNAL_LEVEL_THRESHOLD))
	{
		*pbStronger = TRUE;
		return ERR_OK;
	}

	// 그 외에는 FALSE
	*pbStronger = FALSE;

	return ERR_OK;
}

STATIC HERROR xsvc_si_LcnReassignLcn_UkDtt(DxDeliveryType_e	eOrgDeliType, DxSvcType_e eSvcType, HUINT16 *pusLcn)
{
	HUINT16 	usStartNum;

	if (pusLcn == NULL)
		return ERR_FAIL;

	usStartNum = 0;
	DB_SVC_GetLcnOrderNumber (eOrgDeliType, eSvcType, &usStartNum);

	usStartNum += xsvc_si_LibLcnGetSvcNumBase (eDEFAULTLCN_UKDTT, eOrgDeliType, eSvcType);

	*pusLcn = usStartNum;
	return ERR_OK;
}

#if 0
STATIC HERROR xsvc_si_LcnCheckOverlayLcn_UkDtt (DbSvc_Info_t *pstNewSvcInfo, SRCHSVC_AppendFlag_t *peSvcFlag)
{
	HERROR 		hError ;
	HUINT32 		i, ulSvcCnt;
	Handle_t 		*phSvcList = NULL;
	DbSvc_Attr_t 	stAttr;
	SRCHSVC_AppendFlag_t	eResult;

	if (pstNewSvcInfo == NULL || peSvcFlag == NULL)
	{
		return ERR_FAIL;
	}

	DB_SVC_InitSvcAttribute (&stAttr);

	stAttr.eOrgDeliType = DbSvc_GetDeliTypePtr(pstNewSvcInfo);
	stAttr.eSvcType = DbSvc_GetSvcType(pstNewSvcInfo);

	hError = DB_SVC_FindServiceList(eSvcGroup_All, &stAttr, 0, HANDLE_NULL, eSvcFindDirection_FowardFromStart, &ulSvcCnt, &phSvcList);

	HxLOG_Debug ("\n DB_SVC_FindServiceList (cnt = %d)\n", ulSvcCnt);
	if (hError == ERR_OK)
	{
		HUINT16 		usLcn;
		DbSvc_Info_t	stSvcInfo;

		usLcn = DbSvc_GetLcn(pstNewSvcInfo);
		for (i=0; i<ulSvcCnt; i++)
		{
			hError = DB_SVC_GetServiceInfo(phSvcList[i], &stSvcInfo);
			if (hError != ERR_OK)
			{
			      continue;
			}

			if (usLcn == DbSvc_GetLcn(&stSvcInfo))
			{
				HBOOL bStronger;
				DbSvc_TsInfo_t stNewTsInfo, stDbTsInfo;

				hError =  xsvc_si_LcnProcessLcnDuplicate( phSvcList[i] , &stSvcInfo , pstNewSvcInfo, &eResult);
				if(ERR_OK == hError)
				{
					//확인 필요
					DbSvc_SetLcn(&stSvcInfo, DbSvc_GetLcn(pstNewSvcInfo));
					DB_SVC_UpdateServiceInfo(phSvcList[i], &stSvcInfo);
				}

				HxLOG_Debug ("\n found Same LCN (%d)\n", usLcn);

				hError = DB_SVC_GetTsInfo(DbSvc_GetTsIdx(pstNewSvcInfo), &stNewTsInfo);
				hError |= DB_SVC_GetTsInfo(DbSvc_GetTsIdx(&stSvcInfo), &stDbTsInfo);
				if (hError != ERR_OK)
					continue;

				if (stNewTsInfo.usTsIdx == stDbTsInfo.usTsIdx) // same TP
				{
					HxLOG_Print ("\n found same ts info (0x%x, 0x%x)... so skipped.....\n", stNewTsInfo.usTsIdx, stDbTsInfo.usTsIdx);
					continue;
				}

				HxLOG_Debug ("\n found diff ts info (0x%x, 0x%x)\n", stNewTsInfo.usTsIdx, stDbTsInfo.usTsIdx);
				hError = xsvc_si_LcnIsSignalStrongerThanDbTsInfo_UkDtt(&stNewTsInfo, &stDbTsInfo, &bStronger);
				if (hError != ERR_OK)
				{
					HxLOG_Error ("\n error in xsvc_si_LcnIsSignalStrongerThanDbTsInfo_UkDtt...\n");
					continue;
				}

				if ( (DbSvc_GetOnId(pstNewSvcInfo) == DbSvc_GetOnId(&stSvcInfo))
					&& (DbSvc_GetTsId(pstNewSvcInfo) == DbSvc_GetTsId(&stSvcInfo)) )
				{
					HxLOG_Debug  ("\n Is same TP (new=0x%x:0x%x : old=0x%x:0x%x), bStronger = %d\n", DbSvc_GetOnId(pstNewSvcInfo), DbSvc_GetTsId(pstNewSvcInfo), DbSvc_GetOnId(&stSvcInfo), DbSvc_GetTsId(&stSvcInfo), bStronger);
					if ( (stNewTsInfo.utExt.stUkDtt.bFreqListed == TRUE)
						&& (stDbTsInfo.utExt.stUkDtt.bFreqListed == TRUE) )
					{
						if (bStronger)
						{
							/* 기존 service 삭제처리 */
							DB_SVC_DeleteServiceInfo(phSvcList[i]);
						}
						else
						{
							*peSvcFlag = eSRCHSVC_DONTADD;
						}
					}
					else
					{
						if (bStronger)
						{
							HUINT16 usNewLcn;
							if (xsvc_si_LcnReassignLcn_UkDtt(DbSvc_GetDeliType(stSvcInfo), DbSvc_GetSvcType(&stSvcInfo), &usNewLcn) == ERR_OK)
							{
								DbSvc_SetLcnFlag(&stSvcInfo, FALSE);
								DbSvc_SetLcn(&stSvcInfo, usNewLcn);

								DB_SVC_UpdateServiceInfo(phSvcList[i], &stSvcInfo);
							}
						}
						else
						{
							DbSvc_SetLcnFlag(pstNewSvcInfo, FALSE);
						}
					}
				}
				else
				{
					HxLOG_Debug  ("\n Is diff TP (new=0x%x:0x%x : old=0x%x:0x%x), bStronger = %d\n", DbSvc_GetOnId(pstNewSvcInfo), DbSvc_GetTsId(pstNewSvcInfo), DbSvc_GetOnId(&stSvcInfo), DbSvc_GetTsId(&stSvcInfo), bStronger);
					if (bStronger)
					{
						HUINT16 usNewLcn;
						if (xsvc_si_LcnReassignLcn_UkDtt(DbSvc_GetDeliType(stSvcInfo), DbSvc_GetSvcType(&stSvcInfo), &usNewLcn) == ERR_OK)
						{
							DbSvc_SetLcnFlag(&stSvcInfo, FALSE);
							DbSvc_SetLcn(&stSvcInfo, usNewLcn);

							DB_SVC_UpdateServiceInfo(phSvcList[i], &stSvcInfo);
						}
					}
					else
					{
						DbSvc_SetLcnFlag(pstNewSvcInfo, FALSE);
					}
				}
			}
		}
	}

	if (phSvcList != NULL)
	{
		DB_SVC_FreeServiceList(phSvcList);
	}

	return ERR_OK;
}
#endif
STATIC SRCHSVC_AppendFlag_t xsvc_si_LcnAssignLcn_UkDtt(DbSvc_Info_t *pSvcInfo)
{
	Handle_t			hSvc;
	HERROR			hError = ERR_FAIL;
	DbSvc_Info_t 		stSvcInfo;
	SRCHSVC_AppendFlag_t	eResult;
	HUINT16 				usLcn;

	HxLOG_Print("============== Ente(+)\n\r");
	HxLOG_Print("bLcnFlag = %d, szSvcName = %s \n",DbSvc_GetLcnFlag(pSvcInfo), DbSvc_GetName(pSvcInfo));

	if(DbSvc_GetLcnFlag(pSvcInfo) == TRUE)
	{
		// let's find a duplicate svc in svc DB
		hError = DB_SVC_GetServiceHandleByNumber(eDxDELIVERY_TYPE_TER, eDxSVC_TYPE_All, DbSvc_GetLcn(pSvcInfo), &hSvc);
		if(hError == ERR_OK)
		{
			/* CASE 1 : Duplicate !! */
			HxLOG_Print("[CASE 1]  DUPLICATE --pSvcInfo->usLcn : %d\n", DbSvc_GetLcn(pSvcInfo));

			DB_SVC_GetServiceInfo(hSvc, &stSvcInfo);

			hError =  xsvc_si_LcnProcessLcnDuplicate( hSvc , &stSvcInfo , pSvcInfo, &eResult);
			HxLOG_Print("----->pSvcInfo->usLcn : %d \n", DbSvc_GetLcn(pSvcInfo));
			return eResult;

		}
		else
		{	 /* CASE 2 : None*/
			if(DbSvc_GetLcn(pSvcInfo) == 0)
			{
				xsvc_si_LcnSetToRegionalArea(&usLcn);
				DbSvc_SetLcn(pSvcInfo,usLcn);

				HxLOG_Print ("[CASE 2]  LCN == 0  --pSvcInfo->usLcn : %d ", DbSvc_GetLcn(pSvcInfo));
			}
			else
			{
				// do nothing -> we'll use an orginal LCN.
				HxLOG_Print("\t\t--> usLcn = %d, szSvcName = %s (not duplicated)\n", DbSvc_GetLcn(pSvcInfo), DbSvc_GetName(pSvcInfo));
			}
		}
	}
	else
	{ /* non-LCN, undefined 영역에 할당 (LCD에 LCN이 없는 경우) */
		xsvc_si_LcnSetToRegionalArea(&usLcn);
		DbSvc_SetLcn(pSvcInfo,usLcn);
		DbSvc_SetLcnFlag(pSvcInfo,TRUE);  // LCN Flag값이 setting 되지 않으면 LCN을 Web에서 표시하지 않음. 추후 Ch list를 보여줄 경우 LCN 표시하는 데에 확인 필요
		HxLOG_Print ("[CASE 3]	LCN == 0  --pSvcInfo->usLcn : %d ", DbSvc_GetLcn(pSvcInfo));
	}

	HxLOG_Print("---- xsvc_si_LcnAssignLcn_UkDtt (usLcn = %d, bLcnFlag = %d, szSvcName = %s)\n", DbSvc_GetLcn(pSvcInfo), DbSvc_GetLcnFlag(pSvcInfo), DbSvc_GetName(pSvcInfo));
	HxLOG_Print("========== Leave!\n\n\r");

	return eSRCHSVC_ADD;
}



#define _______________TARGET_REGION_INFO___________________________________


STATIC void xsvc_si_LcnInitTrdConflictedInfo_UkDtt(xsvcSi_ConflictListContent_t *pstConflictContents)
{
	HUINT32		i;

	HxSTD_MemSet(pstConflictContents, 0, sizeof(xsvcSi_ConflictListContent_t));

	for ( i = 0 ; i < DxMAX_T_REGION_NUM ; i++ )
	{
		HxSTD_MemSet(&pstConflictContents->szConflictCountryCode[i],0,4);
		pstConflictContents->ulConflictPrRgCode[i] = INVALID_VALUE;
		pstConflictContents->ulConflictSeRgCode[i] = INVALID_VALUE;
		pstConflictContents->ulConflictTrRgCode[i] = INVALID_VALUE;
	}

	HxSTD_MemSet(pstConflictContents->szSelCountryCode,0,4);
	pstConflictContents->ulConflictCnt = 0;
	pstConflictContents->ulSelPrRgCode = INVALID_VALUE;
	pstConflictContents->ulSelSeRgCode = INVALID_VALUE;
	pstConflictContents->ulSelTrRgCode = INVALID_VALUE;
}

STATIC HERROR xsvc_si_LcnGetTrdconflictedInfo_UkDtt( DbSvc_Info_t *pstSvcInfo , DbSvc_T_RegionInfo_t *pstTRgInfo)
{
	HUINT32			i,nTIdx;
	HBOOL			bFound = FALSE, bFoundSame = FALSE;
	DbSvc_TsInfo_t		stTsInfo;
	DbSvc_NetInfo_t		stNetInfo;
	DbSvc_T_RegionInfo_t	*pstTmpTRgInfo;

	// 01)  Initialize.....
	HxSTD_MemSet( pstTRgInfo , 0 ,  sizeof(DbSvc_T_RegionInfo_t) * DxMAX_T_REGION_NUM );
	for ( nTIdx = 0 ; nTIdx < DxMAX_T_REGION_NUM ; nTIdx++ )
	{
		pstTRgInfo[nTIdx].ucRegionDepth = 0xFF;
		HxSTD_MemSet(pstTRgInfo[nTIdx].szCountryCode,0,4);
	}
	HxSTD_MemSet(&stTsInfo, 0, sizeof(DbSvc_TsInfo_t));
	HxSTD_MemSet(&stNetInfo, 0, sizeof(DbSvc_NetInfo_t));

	// 02)  Get Network & TS Information.
	DB_SVC_GetTsInfo(DbSvc_GetTsIdx(pstSvcInfo) , &stTsInfo);
	DB_SVC_GetNetworkInfo(stTsInfo.usNetIdx, &stNetInfo);

	// 03)  Copy Network's Target Region Information.
	for ( i = 0 ; i < DxMAX_T_REGION_NUM ; i++ )
	{
		pstTmpTRgInfo =  &stNetInfo.utExt.stTvAnyTime.stRegionInfo[i];
		if ( 0xFF != pstTmpTRgInfo->ucRegionDepth )
		{
			bFound = TRUE;
			for ( nTIdx = 0 ; nTIdx < DxMAX_T_REGION_NUM ; nTIdx++ )
			{
				if ( 0xFF == pstTRgInfo[nTIdx].ucRegionDepth )
				{
					pstTRgInfo[nTIdx].ucRegionDepth = pstTmpTRgInfo->ucRegionDepth;
					pstTRgInfo[nTIdx].ulRegionCode = pstTmpTRgInfo->ulRegionCode;
					HxSTD_MemCopy(	pstTRgInfo[nTIdx].szCountryCode , pstTmpTRgInfo->szCountryCode , 4 );

					HxLOG_Print("pstTRgInfo[nTIdx].szCountryCode:%s, pstTmpTRgInfo->szCountryCode:%s\n",pstTRgInfo[nTIdx].szCountryCode,pstTmpTRgInfo->szCountryCode);
					break;
				}
			}
		}
	}

	// 04) Copy TS's Target Region Information.
	bFoundSame = FALSE;
	for ( i = 0 ; i < DxMAX_T_REGION_NUM ; i++ )
	{
		pstTmpTRgInfo = &stTsInfo.utExt.stTvAnyTime.stRegionInfo[i];
		if ( 0xFF != pstTmpTRgInfo->ucRegionDepth )
		{
			bFound = TRUE;
			bFoundSame = TRUE;
		}
	}

	//	conflict 발생하는  service의 target region 정보를 수집시에,.
	//	NIT Ts level 의 target region 정보가 있으면, NIT NET level 의 target region 정보는 무시하고,
	//	SDT 의 target region 정보가 있으면, NIT의 NET,TS level의 target region 정보를 무시하도록 수정함 by kcjung. (이전 구현사항은 svn log 참고 )
	if(bFoundSame == TRUE)
	{
		for ( nTIdx = 0 ; nTIdx < DxMAX_T_REGION_NUM ; nTIdx++ )
		{
			HxSTD_MemSet(&pstTRgInfo[nTIdx], 0, sizeof(DbSvc_T_RegionInfo_t));
			HxSTD_MemSet(pstTRgInfo[nTIdx].szCountryCode, 0, 4);
			pstTRgInfo[nTIdx].ucRegionDepth = 0xFF;
		}
	}

	for ( i = 0 ; i < DxMAX_T_REGION_NUM ; i++ )
	{
		pstTmpTRgInfo = &stTsInfo.utExt.stTvAnyTime.stRegionInfo[i];

		if ( 0xFF != pstTmpTRgInfo->ucRegionDepth )
		{
			for ( nTIdx = 0 ; nTIdx < DxMAX_T_REGION_NUM ; nTIdx++ )
			{
				if ( 0xFF == pstTRgInfo[nTIdx].ucRegionDepth )
				{
					pstTRgInfo[nTIdx].ucRegionDepth = pstTmpTRgInfo->ucRegionDepth;
					pstTRgInfo[nTIdx].ulRegionCode = pstTmpTRgInfo->ulRegionCode;
					HxSTD_MemCopy(	pstTRgInfo[nTIdx].szCountryCode , pstTmpTRgInfo->szCountryCode , 4 );
					HxLOG_Print("pstTRgInfo[nTIdx].szCountryCode:%s, pstTmpTRgInfo->szCountryCode:%s\n",pstTRgInfo[nTIdx].szCountryCode,pstTmpTRgInfo->szCountryCode);
					break;
				}
			}
		}
	}

	// 05) Copy SVC's Target Region Information.
	bFoundSame = FALSE;

	for ( i = 0 ; i < DxMAX_T_REGION_NUM ; i++ )
	{
		pstTmpTRgInfo = XdbSvc_TvAnyTimeGetRegionInfo(pstSvcInfo, i);
		if ( 0xFF != pstTmpTRgInfo->ucRegionDepth )
		{
			bFound = TRUE;
			bFoundSame = TRUE;

		}
	}
	//	conflict 발생하는  service의 target region 정보를 수집시에,.
	//	NIT Ts level 의 target region 정보가 있으면, NIT NET level 의 target region 정보는 무시하고,
	//	SDT 의 target region 정보가 있으면, NIT의 NET,TS level의 target region 정보를 무시하도록 수정함 by kcjung. (이전 구현사항은 svn log 참고 )
	if(bFoundSame == TRUE)
	{
		for ( nTIdx = 0 ; nTIdx < DxMAX_T_REGION_NUM ; nTIdx++ )
		{
			HxSTD_MemSet(&pstTRgInfo[nTIdx], 0, sizeof(DbSvc_T_RegionInfo_t));
			pstTRgInfo[nTIdx].ucRegionDepth = 0xFF;
			HxSTD_MemSet(pstTRgInfo[nTIdx].szCountryCode, 0, 4);
		}
	}

	for ( i = 0 ; i < DxMAX_T_REGION_NUM ; i++ )
	{
		pstTmpTRgInfo = XdbSvc_TvAnyTimeGetRegionInfo(pstSvcInfo, i);

		if ( 0xFF != pstTmpTRgInfo->ucRegionDepth )
		{
			for ( nTIdx = 0 ; nTIdx < DxMAX_T_REGION_NUM ; nTIdx++ )
			{
				if ( 0xFF == pstTRgInfo[nTIdx].ucRegionDepth )
				{
					pstTRgInfo[nTIdx].ucRegionDepth = pstTmpTRgInfo->ucRegionDepth;
					pstTRgInfo[nTIdx].ulRegionCode = pstTmpTRgInfo->ulRegionCode;
					HxSTD_MemCopy(	pstTRgInfo[nTIdx].szCountryCode , pstTmpTRgInfo->szCountryCode , 4 );
					HxLOG_Print("pstTRgInfo[nTIdx].szCountryCode:%s, pstTmpTRgInfo->szCountryCode:%s\n", pstTRgInfo[nTIdx].szCountryCode,pstTmpTRgInfo->szCountryCode);
					break;
				}
			}
		}
	}

	if ( TRUE == bFound)
	{
		return ERR_OK;
	}

	return  ERR_FAIL;
}


STATIC void xsvc_si_LcnSetTrdConflictedCountry_UkDtt(xsvcSi_ConflictListContent_t *pstConflictContents, HUINT8 *szCountry)
{
	HUINT32 i;
	HBOOL	bExist=FALSE;

	for ( i = 0 ; i < DxMAX_T_REGION_NUM ; i++ )
	{
		HxLOG_Print("pstConflictContents->szConflictCountryCode[i] : %s, szCountry:%s\n",pstConflictContents->szConflictCountryCode[i],szCountry);
		if ( ( 0 != HxSTD_StrLen(pstConflictContents->szConflictCountryCode[i]) )
			&&  (0 == HxSTD_StrCmp(pstConflictContents->szConflictCountryCode[i], szCountry)) )
		{
			bExist = TRUE;
		}
	}

	if ( FALSE == bExist )
	{
		for ( i = 0 ; i < DxMAX_T_REGION_NUM ; i++ )
		{
			HxLOG_Print("pstConflictContents->szConflictCountryCode[i] : %s, szCountry:%s\n",pstConflictContents->szConflictCountryCode[i],szCountry);
			if ( 0 == HxSTD_StrLen(pstConflictContents->szConflictCountryCode[i]) )
			{
				HxSTD_MemCopy(pstConflictContents->szConflictCountryCode[i], szCountry, 4);
				HxLOG_Print("pstConflictContents->szConflictCountryCode[i] : %s, szCountry:%s\n",pstConflictContents->szConflictCountryCode[i],szCountry);
				return;
			}
		}
	}
}

STATIC HERROR xsvc_si_LcnCheckTrdCountryCodeInConflictData_UkDtt(xsvcSi_ConflictListContent_t *pstConflictContents)
{
	HERROR		hErr = ERR_FAIL;
	HUINT32 		ulMaxConflictCnt = 0;
	HUINT32		j,ulSvcIdx,ulConfIdx;
	Handle_t		hSvc;
	DbSvc_Info_t	stSvcInfo;
	DbSvc_T_RegionInfo_t	stTmpTRgInfo[DxMAX_T_REGION_NUM];
	HUINT16				usConflictLcn;
	HUINT32				ulSvcCount;
	xsvcSi_SvcUniqueData_t	*pstUniqData;


	ulMaxConflictCnt =  xsvc_si_LcnGetTrdConflictedSvcCount_UkDtt();

	HxLOG_Print("ulMaxConflictCnt:%d\n", ulMaxConflictCnt);

	// Checking Country Code;
	for ( ulConfIdx = 0 ; ulConfIdx < ulMaxConflictCnt ; ulConfIdx++ )
	{
		usConflictLcn = 0;
		ulSvcCount = 0;
		pstUniqData = NULL;

		hErr = xsvc_si_LcnGetTrdConflictSvcHandleTable_UkDtt(ulConfIdx , &usConflictLcn, &ulSvcCount, (void *)&pstUniqData);
		if(hErr == ERR_FAIL)		{	continue;	}

		HxLOG_Print("ulConfIdx[%02d] usConflictLcn[%d] ulSvcCount[%d] pstUniqData[%p] \n"  , ulConfIdx, usConflictLcn, ulSvcCount, pstUniqData);

		for ( ulSvcIdx = 0 ; ulSvcIdx < ulSvcCount ; ulSvcIdx++ )
		{
			HxLOG_Print("ulSvcIdx(%02d)  usTsIdx(%02d) usSvcId(0x%04x) \n"  , ulSvcIdx, pstUniqData[ulSvcIdx].usTsIdx, pstUniqData[ulSvcIdx].usSvcId);
			hErr = DB_SVC_GetServiceHandle (eDxSvcSection_All, pstUniqData[ulSvcIdx].usTsIdx, pstUniqData[ulSvcIdx].usSvcId, &hSvc);
			HxLOG_Print("DB_SVC_GetServiceHandle :hErr(%d) \n"  , hErr);
			if(hErr == ERR_FAIL)		{	continue;	}

			HxSTD_memset(&stSvcInfo, 0x00, sizeof(DbSvc_Info_t));

			hErr = DB_SVC_GetServiceInfo(hSvc, &stSvcInfo);
			HxLOG_Print("DB_SVC_GetServiceInfo :hErr(%d) \n"  , hErr);
			if(hErr == ERR_FAIL)		{	continue;	}

			HxSTD_memset(&stTmpTRgInfo, 0x00, sizeof(DbSvc_T_RegionInfo_t) * DxMAX_T_REGION_NUM);

			hErr = xsvc_si_LcnGetTrdconflictedInfo_UkDtt (&stSvcInfo, stTmpTRgInfo);
			HxLOG_Print("xsvc_si_LcnGetTrdconflictedInfo_UkDtt :hErr(%d) \n"  , hErr);
			if(hErr == ERR_FAIL)		{	continue;	}

			for ( j = 0 ; j < DxMAX_T_REGION_NUM  ; j++ )
			{
				// Check Country
				// proc init 시에는 Conflict county list 만 구성하고, primary list 는 Contry list 선택후에 구성하도록 변경. by kcjung.
				if ( ( 0 != HxSTD_StrLen(stTmpTRgInfo[j].szCountryCode) ) && ( 0xFF != stTmpTRgInfo[j].ucRegionDepth ) )
					xsvc_si_LcnSetTrdConflictedCountry_UkDtt (pstConflictContents, stTmpTRgInfo[j].szCountryCode);
			}
		}
	}

	return ERR_OK;
}

STATIC HUINT32 xsvc_si_LcnGetTrdConflictedSvcCount_UkDtt(void)
{
#if defined(CONFIG_DEBUG)
	HUINT32 	i,j;
	xsvcSi_ConflictServiceList_t 	*pstUkCSvcList;
	xsvcSi_SvcUniqueData_t		*pstSvcUniqData;

	HxLOG_Print("============ Conflict Table Num:%d ============\n",s_stUkConflictServiceTable.ulTblNum);
	for ( i = 0 ; i < s_stUkConflictServiceTable.ulTblNum ; i++ )
	{
		pstUkCSvcList = &s_stUkConflictServiceTable.pstArray[i];
		HxLOG_Print("\tConflict LCN : %03d -- SvnNum:%d\n", pstUkCSvcList->usLcn , pstUkCSvcList->ulSvcNum );
		for ( j = 0 ; j < pstUkCSvcList->ulSvcNum ; j++ )
		{
			pstSvcUniqData = &pstUkCSvcList->pstUniqDataArray[j];
			HxLOG_Print("\t\tTsIdx:%d - SvcId:0x%X\n", pstSvcUniqData->usTsIdx , pstSvcUniqData->usSvcId );
		}

	}
	HxLOG_Print("================================================\n");
#endif
	HxLOG_Print("\tThere are [%d] conflict services\n",s_stUkConflictServiceTable.ulTblNum);

	return s_stUkConflictServiceTable.ulTblNum;

}

STATIC HERROR xsvc_si_LcnGetTrdConflictSvcHandleTable_UkDtt(HUINT32 ulIndex, HUINT16 *pusLcn, HUINT32 *pulCount, void **pUniqDataList)
{
	HERROR hErr = ERR_OK;

	if(s_stUkConflictServiceTable.ulTblNum == 0 || s_stUkConflictServiceTable.pstArray == NULL)
	{
		HxLOG_Error("\tThere are no conflict services\n");
		return ERR_FAIL;
	}

	*pusLcn = s_stUkConflictServiceTable.pstArray[ulIndex].usLcn;
	*pulCount = s_stUkConflictServiceTable.pstArray[ulIndex].ulSvcNum;
	*pUniqDataList = (void *)s_stUkConflictServiceTable.pstArray[ulIndex].pstUniqDataArray;

#if defined(CONFIG_DEBUG)
	{
		HUINT32 ulCnt;
		Handle_t hSvc;
		DbSvc_Info_t stSvcInfo;

		for(ulCnt = 0; ulCnt < s_stUkConflictServiceTable.pstArray[ulIndex].ulSvcNum; ulCnt ++)
		{
			hErr = DB_SVC_GetServiceHandle (eDxSvcSection_All, s_stUkConflictServiceTable.pstArray[ulIndex].pstUniqDataArray[ulCnt].usTsIdx, s_stUkConflictServiceTable.pstArray[ulIndex].pstUniqDataArray[ulCnt].usSvcId, &hSvc);
			if(hErr != ERR_OK)
			{
				continue;
			}

			DB_SVC_GetServiceInfo(hSvc, &stSvcInfo);
			HxLOG_Print("[%d][%d] Conflict SVC #%d, %s\n",ulIndex, ulCnt, DbSvc_GetLcn(&stSvcInfo), DbSvc_GetName(&stSvcInfo));
		}
	}
#endif

	return hErr;
}


STATIC void xsvc_si_LcnGetTrdConflictedCountryCnt_UkDtt(xsvcSi_ConflictListContent_t *pContents, HUINT32 *pulCnt)
{
	HUINT32 i,ulCnt=0;

	for ( i = 0 ; i < DxMAX_T_REGION_NUM ; i++ )
	{
		if ( 0 != HxSTD_StrLen(pContents->szConflictCountryCode[i]) )
			ulCnt++;
	}

	*pulCnt = ulCnt;
}

// First Country List를 불러오는 경우 부르는 함수
HERROR xsvc_si_LcnGetTrdFirstConflictedList_UkDtt(void)
{
	HUINT32 					ulConflictCnt = 0;
	xsvcSi_ConflictListContent_t 	*pConflictContents = NULL;

	pConflictContents = xsvc_si_LcnGetConflictContent();
	if(pConflictContents== NULL)
	{
		HxLOG_Warning("pConflictContents is NULL \n");
	}

	xsvc_si_LcnInitTrdConflictedInfo_UkDtt(pConflictContents);

	xsvc_si_LcnCheckTrdCountryCodeInConflictData_UkDtt(pConflictContents);

	xsvc_si_LcnGetTrdConflictedCountryCnt_UkDtt (pConflictContents, &ulConflictCnt);

	if ( 0 < ulConflictCnt)
	{
	//	pConflictContents->ulCurConflictIdx = 0;
		pConflictContents->ulConflictCnt = 1;
		pConflictContents->ulConflictTrdCnt = ulConflictCnt;
	}


#if defined(CONFIG_DEBUG)
{
	HUINT32 i;

	HxLOG_Print("Conflict Country :\n");
	for ( i = 0 ; i < DxMAX_T_REGION_NUM ; i++ )
	{
		HxLOG_Print("\t%s\n",	pConflictContents->szConflictCountryCode[i]);
	}
	HxLOG_Print("Conflict Primary Region Code :\n");
	for ( i = 0 ; i < DxMAX_T_REGION_NUM ; i++ )
	{
		HxLOG_Print("\t0x%02X\n",	pConflictContents->ulConflictPrRgCode[i] );
	}
	HxLOG_Print("Conflict Secondary Region Code :\n");
	for ( i = 0 ; i < DxMAX_T_REGION_NUM ; i++ )
	{
		HxLOG_Print("\t0x%04X\n",	pConflictContents->ulConflictSeRgCode[i]);
	}
	HxLOG_Print("Conflict Teritary Region Code :\n");
	for ( i = 0 ; i < DxMAX_T_REGION_NUM ; i++ )
	{
		HxLOG_Print("\t0x%08X\n",	pConflictContents->ulConflictTrRgCode[i] );
	}
}
#endif

	return ERR_OK;


}
static void xsvc_si_LcnSetTrdConflicted_PrimaryCode_UkDtt(xsvcSi_ConflictListContent_t *pContents, HUINT32 ulTRg)
{
	HUINT32 i;
	HBOOL	bExist=FALSE;

	for ( i = 0 ; i < DxMAX_T_REGION_NUM ; i++ )
	{
		if ( ( INVALID_VALUE != pContents->ulConflictPrRgCode[i] ) &&
			( pContents->ulConflictPrRgCode[i] == ulTRg ) )
			bExist = TRUE;
	}

	if ( FALSE == bExist )
	{
		for ( i = 0 ; i < DxMAX_T_REGION_NUM ; i++ )
		{
			if ( INVALID_VALUE == pContents->ulConflictPrRgCode[i] )
			{
				pContents->ulConflictPrRgCode[i] = ulTRg;
				return;
			}
		}
	}
}
static void xsvc_si_LcnSetTrdConflicted_SecondaryCode_UkDtt(xsvcSi_ConflictListContent_t *pContents, HUINT32 ulTRg)
{
	HUINT32 i;
	HBOOL	bExist=FALSE;

	for ( i = 0 ; i < DxMAX_T_REGION_NUM ; i++ )
	{
		if ( ( INVALID_VALUE != pContents->ulConflictSeRgCode[i] ) &&
			( pContents->ulConflictSeRgCode[i] == ulTRg ) )
			bExist = TRUE;
	}

	if ( FALSE == bExist )
	{
		for ( i = 0 ; i < DxMAX_T_REGION_NUM ; i++ )
		{
			if ( INVALID_VALUE == pContents->ulConflictSeRgCode[i] )
			{
				pContents->ulConflictSeRgCode[i] = ulTRg;
				return;
			}
		}
	}
}

static void xsvc_si_LcnGetTrdconflicted_SecondaryCodeIdx_UkDtt(xsvcSi_ConflictListContent_t *pContents, HUINT32 ulIdx, HUINT32 *pulConvIdx)
{
	HUINT32 i,ulCnt=0;

	*pulConvIdx = 0;

	for ( i = 0 ; i < DxMAX_T_REGION_NUM ; i++ )
	{
		if ( ( INVALID_VALUE != pContents->ulConflictSeRgCode[i] ) && ( INVALID_VALUE != pContents->ulSelPrRgCode ) )
		{
			if ( ( pContents->ulConflictSeRgCode[i] & 0xFF000000 ) == pContents->ulSelPrRgCode )
			{
				if ( ulCnt == ulIdx )
				{
					*pulConvIdx = i;
					break;
				}
				ulCnt++;
			}
		}
		else
		{
			if ( INVALID_VALUE != pContents->ulConflictSeRgCode[i] )
			{
				if ( ulCnt == ulIdx )
				{
					*pulConvIdx = i;
					break;
				}
				ulCnt++;
			}
		}
	}
}

static void xsvc_si_LcnSetTrdConflicted_TeritaryCode_UkDtt(xsvcSi_ConflictListContent_t *pContents, HUINT32 ulTRg)
{
	HUINT32 i;
	HBOOL	bExist=FALSE;

	for ( i = 0 ; i < DxMAX_T_REGION_NUM ; i++ )
	{
		if ( ( INVALID_VALUE != pContents->ulConflictTrRgCode[i] ) &&
			( pContents->ulConflictTrRgCode[i] == ulTRg ) )
			bExist = TRUE;
	}

	if ( FALSE == bExist )
	{
		for ( i = 0 ; i < DxMAX_T_REGION_NUM ; i++ )
		{
			if ( INVALID_VALUE == pContents->ulConflictTrRgCode[i] )
			{
				pContents->ulConflictTrRgCode[i] = ulTRg;
				return;
			}
		}
	}
}

static void xsvc_si_LcnGetTrdConflicted_TeritaryCodeIdx_UkDtt(xsvcSi_ConflictListContent_t *pContents, HUINT32 ulIdx, HUINT32 *pulConvIdx)
{
	HUINT32 i,ulCnt=0;

	*pulConvIdx = 0;

	for ( i = 0 ; i < DxMAX_T_REGION_NUM ; i++ )
	{
		if ( ( INVALID_VALUE != pContents->ulConflictTrRgCode[i] ) && ( INVALID_VALUE != pContents->ulSelSeRgCode ) )
		{
			if ( ( pContents->ulConflictTrRgCode[i] & 0xFFFF0000 ) == pContents->ulSelSeRgCode )
			{
				if ( ulCnt == ulIdx )
				{
					*pulConvIdx = i;
					break;
				}
				ulCnt++;
			}
		}
		else
		{
			if ( INVALID_VALUE != pContents->ulConflictTrRgCode[i] )
			{
				if ( ulCnt == ulIdx )
				{
					*pulConvIdx = i;
					break;
				}
				ulCnt++;
			}
		}
	}
}

static void xsvc_si_LcnMadeTrdConflictedItem_UkDtt( xsvcSi_ConflictListContent_t *pContents, HUINT32 ulSelectIdx )
{

	HERROR hErr = ERR_FAIL;
	HUINT32 ulMaxConflictCnt = 0;
	HUINT32	ulConfIdx;

	HUINT32		j,ulSvcIdx;
	Handle_t		hSvc;
	DbSvc_Info_t	stSvcInfo;
	DbSvc_T_RegionInfo_t	stTmpTRgInfo[DxMAX_T_REGION_NUM];
	HUINT16				usConflictLcn;
	HUINT32				ulSvcCount;
	xsvcSi_SvcUniqueData_t	*pstUniqData;


	ulMaxConflictCnt = xsvc_si_LcnGetTrdConflictedSvcCount_UkDtt();

	HxLOG_Print("[%s:%d] ulMaxConflictCnt:%d\n",__FUNCTION__ , __LINE__ , ulMaxConflictCnt);

	// Checking Country Code;
	for ( ulConfIdx = 0 ; ulConfIdx < ulMaxConflictCnt ; ulConfIdx++ )
	{
		xsvc_si_LcnGetTrdConflictSvcHandleTable_UkDtt(ulConfIdx , &usConflictLcn, &ulSvcCount, (void *)&pstUniqData);
		for ( ulSvcIdx = 0 ; ulSvcIdx < ulSvcCount ; ulSvcIdx++ )
		{
			hErr = DB_SVC_GetServiceHandle (eDxSvcSection_All, pstUniqData[ulSvcIdx].usTsIdx, pstUniqData[ulSvcIdx].usSvcId, &hSvc);

			if ( ERR_OK == hErr )
			{
				hErr = DB_SVC_GetServiceInfo(hSvc, &stSvcInfo);
				if ( ERR_OK == hErr )
				{
					hErr = xsvc_si_LcnGetTrdconflictedInfo_UkDtt (&stSvcInfo, stTmpTRgInfo);
					if ( ERR_OK == hErr )
					{
						for ( j = 0 ; j < DxMAX_T_REGION_NUM  ; j++ )
						{
							switch ( pContents->ulCurConflictIdx )
							{
								case 0 :
									//pContents 의 primary array를 만들것 .    Checking Primary Region
									if ( ( 0xFF != stTmpTRgInfo[j].ucRegionDepth ) && (0 != ( stTmpTRgInfo[j].ulRegionCode & 0xFF000000 ) ) && (0 == HxSTD_StrCmp(stTmpTRgInfo[j].szCountryCode, pContents->szConflictCountryCode[ulSelectIdx]) ) )
										xsvc_si_LcnSetTrdConflicted_PrimaryCode_UkDtt (pContents, (HUINT32)( stTmpTRgInfo[j].ulRegionCode & 0xFF000000 ) );
									break;
								case 1 :
									//pContents 의 secondary array를 만들것 .  Checking Secondary Region
									if ( ( 0xFF != stTmpTRgInfo[j].ucRegionDepth ) && (0 != ( stTmpTRgInfo[j].ulRegionCode & 0x00FF0000 ) ) && ((stTmpTRgInfo[j].ulRegionCode & 0xFF000000)==(pContents->ulSelPrRgCode & 0xFF000000) ) )
										xsvc_si_LcnSetTrdConflicted_SecondaryCode_UkDtt (pContents, (HUINT32)( stTmpTRgInfo[j].ulRegionCode & 0xFFFF0000 ) );
									break;
								case 2 :
									//pContents 의 3순위 array를 만들것 .
									if ( ( 0xFF != stTmpTRgInfo[j].ucRegionDepth ) && (0 != ( stTmpTRgInfo[j].ulRegionCode & 0x0000FFFF ) ) && ((stTmpTRgInfo[j].ulRegionCode & 0xFFFF0000)==(pContents->ulSelSeRgCode & 0xFFFF0000) ) )
										xsvc_si_LcnSetTrdConflicted_TeritaryCode_UkDtt (pContents, (HUINT32) stTmpTRgInfo[j].ulRegionCode );
									break;
								default :

									break;
							}
						}
					}
				}
			}
		}
	}
	return;

}
STATIC HINT32 xsvc_si_LcnCompareTrdConflicted_ChannelNumber_UkDtt(const void *p1, const void *p2)
{
	HERROR	hError = ERR_OK;

	xsvcSi_SvcUniqueData_t *pSvc1 = (xsvcSi_SvcUniqueData_t *)p1;
	xsvcSi_SvcUniqueData_t *pSvc2 = (xsvcSi_SvcUniqueData_t *)p2;
	Handle_t			hSvc1 = HANDLE_NULL, hSvc2= HANDLE_NULL;
	HINT32			svcNum1 =0 , svcNum2 =0 ;

	hError = DB_SVC_GetServiceHandle(eDxSvcSection_All, pSvc1->usTsIdx, pSvc1->usSvcId, &hSvc1);
	hError |= DB_SVC_GetServiceHandle(eDxSvcSection_All, pSvc2->usTsIdx, pSvc2->usSvcId, &hSvc2);

	if(hError == ERR_OK)
	{
		DB_SVC_GetServiceNumber(hSvc1, &svcNum1);
		DB_SVC_GetServiceNumber(hSvc2, &svcNum2);
	}

	return svcNum1 - svcNum2;
}

STATIC  void xsvc_si_LcnSetTrdConflicttrdSerivice_UkDtt ( xsvcSi_ConflictListContent_t *pContents )
{
	HUINT32 		ulMaxConflictCnt,ulSvcCount,ulSvcIdx,i,j,ulChoosenSvcIdx;
	HUINT16 		usConflictLcn;
	xsvcSi_SvcUniqueData_t	*pstUniqData;
	Handle_t			hSvc = HANDLE_NULL, hFirstSvcHandle = HANDLE_NULL, hSelectedSvcHandle = HANDLE_NULL;
	DbSvc_Info_t		stSvcInfo1,stSvcInfo2;
	HERROR			hErr;
	DbSvc_T_RegionInfo_t	stTmpTRgInfo[DxMAX_T_REGION_NUM];
	HUINT16 	usTempLcn;
	HBOOL		bLcnFlag, bNotLcnMatchFlag;
	HUINT32 	ulHighRegionMatchValue, ulTmpRegionMatchValue, ulCurRegionMatchValue,ulSecondRegionMatchValue,ulSecondHighRegionMatchValue;
	HUINT32 	ulChoosenSubSvcIdx,ulChoosenNewSubSvcIdx;
	HUINT32		ulStartSvcIdx = 0;

	ulMaxConflictCnt = xsvc_si_LcnGetTrdConflictedSvcCount_UkDtt();

	HxLOG_Print("[%s:%d] ulMaxConflictCnt :%d\n",__FUNCTION__ , __LINE__ ,ulMaxConflictCnt );

	// Checking Country Code;
	for ( i = 0 ; i < ulMaxConflictCnt ; i++ )
	{
		xsvc_si_LcnGetTrdConflictSvcHandleTable_UkDtt(i , &usConflictLcn, &ulSvcCount, (void *)&pstUniqData);
		HxLOG_Debug("ConflictIndex(%02d) ulSvcCount (%02d) \n" , i, ulSvcCount );

		if ( ulSvcCount < 2 )
		{
			HxLOG_Error("the conflicted service must be more two, so this will be skipped. \n");
			continue;
		}

		if ( ulSvcCount > 0 )
		{
			qsort(pstUniqData, ulSvcCount, sizeof(xsvcSi_SvcUniqueData_t), xsvc_si_LcnCompareTrdConflicted_ChannelNumber_UkDtt);
		}

		ulChoosenSvcIdx = INVALID_VALUE;
		ulChoosenSubSvcIdx = INVALID_VALUE;
		ulChoosenNewSubSvcIdx = INVALID_VALUE;
		ulHighRegionMatchValue = 0;
		ulSecondHighRegionMatchValue = 0;
		bNotLcnMatchFlag = FALSE;
		ulStartSvcIdx = 0;

#if defined(REMOVE_NOT_VAILD_LCN)
		/* Check first service is vaild LCN compared with usConflictLcn */
		hErr = DB_SVC_GetServiceHandle(eDxSvcSection_All, pstUniqData[0].usTsIdx,pstUniqData[0].usSvcId, &hSvc);
		if( ERR_OK == hErr )
		{
			hErr = DB_SVC_GetServiceInfo(hSvc, &stSvcInfo1);
			if ( ERR_OK == hErr )
			{
				if(DbSvc_GetLcn(&stSvcInfo1) != usConflictLcn)
				{
					bNotLcnMatchFlag = TRUE;
					ulStartSvcIdx = 1;
				}
			}	
		}

		/* If bNotLcnMatchFlag is TRUE, skipping first service and assigned conflicted LCN into 2th service. */
		if((bNotLcnMatchFlag == TRUE))
		{
			hErr = DB_SVC_GetServiceHandle(eDxSvcSection_All, pstUniqData[ulStartSvcIdx].usTsIdx,pstUniqData[ulStartSvcIdx].usSvcId, &hSvc);
			if( ERR_OK == hErr )
			{
				hErr = DB_SVC_GetServiceInfo(hSvc, &stSvcInfo1);
				if ( ERR_OK == hErr )
				{
					DbSvc_SetLcn(&stSvcInfo1, usConflictLcn);
					DB_SVC_UpdateServiceInfo(hSvc, &stSvcInfo1);
				}	
			}
		}
#endif
		for ( ulSvcIdx = ulStartSvcIdx ; ulSvcIdx < ulSvcCount ; ulSvcIdx++ )
		{
			HxLOG_Print("SvcIndex[%d] =============\n", ulSvcIdx );

			ulCurRegionMatchValue = 0;
			ulSecondRegionMatchValue = 0;

			hErr = DB_SVC_GetServiceHandle(eDxSvcSection_All, pstUniqData[ulSvcIdx].usTsIdx,pstUniqData[ulSvcIdx].usSvcId, &hSvc);
			if ( ERR_OK == hErr )
			{
				hErr = DB_SVC_GetServiceInfo(hSvc, &stSvcInfo1);
				if ( ERR_OK == hErr )
				{
					HxLOG_Print("svc  name(%s) LCN(%d) LCN_flag(%d) onid(%d) tsid(%d) svcid(%d)\n",DbSvc_GetName(&stSvcInfo1),DbSvc_GetLcn(&stSvcInfo1),DbSvc_GetLcnFlag(&stSvcInfo1),DbSvc_GetOnId(&stSvcInfo1),DbSvc_GetTsId(&stSvcInfo1),DbSvc_GetSvcId(&stSvcInfo1));

					hErr = xsvc_si_LcnGetTrdconflictedInfo_UkDtt (&stSvcInfo1, stTmpTRgInfo);
					if ( ERR_OK == hErr )
					{
#if defined(CONFIG_DEBUG)
						for ( j = 0 ; j < DxMAX_T_REGION_NUM  ; j++ )
						{
							HxLOG_Print("stTmpTRgInfo[%d] ulRegionCode(0x%X) ucRegionDepth(%d) szCountryCode(%s)\n",j,stTmpTRgInfo[j].ulRegionCode,stTmpTRgInfo[j].ucRegionDepth,stTmpTRgInfo[j].szCountryCode );
						}
						HxLOG_Print(" pContents->szSelCountryCode(%s)\n",pContents->szSelCountryCode);
						HxLOG_Print("pContents->ulSelPrRgCode(0x%X) \n",pContents->ulSelPrRgCode);
						HxLOG_Print("pContents->ulSelSeRgCode(0x%X) \n",pContents->ulSelSeRgCode);
						HxLOG_Print("pContents->ulSelTrRgCode(0x%X) \n",pContents->ulSelTrRgCode);
#endif

						for ( j = 0 ; j < DxMAX_T_REGION_NUM  ; j++ )
						{
							ulTmpRegionMatchValue = 0;
							if ( 0xFF != stTmpTRgInfo[j].ucRegionDepth )
							{
								if ( 0 == HxSTD_StrCmp (stTmpTRgInfo[j].szCountryCode, pContents->szSelCountryCode) )
								{
									ulTmpRegionMatchValue = 10;

									if ( pContents->ulSelTrRgCode == stTmpTRgInfo[j].ulRegionCode )
									{
										ulTmpRegionMatchValue = 40;
									}
									else if ( pContents->ulSelSeRgCode == ( stTmpTRgInfo[j].ulRegionCode & 0xFFFF0000 ) )
									{
										ulTmpRegionMatchValue = 30;
									}
									else if ( pContents->ulSelPrRgCode == ( stTmpTRgInfo[j].ulRegionCode & 0xFF000000 ) )
									{
										ulTmpRegionMatchValue = 20;
									}
								}

								if (ulTmpRegionMatchValue > ulCurRegionMatchValue)
								{
									ulCurRegionMatchValue = ulTmpRegionMatchValue;
								}
								if(ulTmpRegionMatchValue < 40 && ulTmpRegionMatchValue > ulSecondRegionMatchValue )
									{
										ulSecondRegionMatchValue = ulTmpRegionMatchValue;
									}
								
								
								HxLOG_Print("index(%d) ulTmpRegionMatchValue(%d) \n",j,ulTmpRegionMatchValue);
								}
							}
						
						HxLOG_Print("ulSecondRegionMatchValue(%d)  ulCurRegionMatchValue(%d) \n",ulSecondRegionMatchValue,ulCurRegionMatchValue);
					}
				}
			}

			if (ulCurRegionMatchValue > ulHighRegionMatchValue)
			{
				ulHighRegionMatchValue = ulCurRegionMatchValue;
				ulChoosenSvcIdx = ulSvcIdx;
				
			}
			else if (ulCurRegionMatchValue == ulHighRegionMatchValue)
			{
				DbSvc_Info_t	stNewSvcInfo, stOldSvcInfo;
				Handle_t	hNewSvc = HANDLE_NULL, hOldSvc = HANDLE_NULL;

				hErr = DB_SVC_GetServiceHandle(eDxSvcSection_All, pstUniqData[ulChoosenSvcIdx].usTsIdx, pstUniqData[ulChoosenSvcIdx].usSvcId, &hOldSvc);
				if(hErr == ERR_OK)
				{
					hErr = DB_SVC_GetServiceInfo(hOldSvc, &stOldSvcInfo);
					if(hErr == ERR_OK)
					{
						hErr = DB_SVC_GetServiceHandle(eDxSvcSection_All, pstUniqData[ulSvcIdx].usTsIdx, pstUniqData[ulSvcIdx].usSvcId, &hNewSvc);
						if(hErr == ERR_OK)
						{
							hErr = DB_SVC_GetServiceInfo(hNewSvc, &stNewSvcInfo);
							if(hErr == ERR_OK)
							{
								if((DbSvc_GetVideoType(&stOldSvcInfo) == eDxVIDEO_TYPE_SD) && (DbSvc_GetVideoType(&stNewSvcInfo) == eDxVIDEO_TYPE_HD))
								{
									ulChoosenSvcIdx = ulSvcIdx;
								}
								else
								{
									DbSvc_TsInfo_t		stNewTsInfo,stOldTsInfo;
									HBOOL				bHigh;

									HxSTD_MemSet(&stOldTsInfo, 0, sizeof(DbSvc_TsInfo_t));
									if (ulChoosenSvcIdx < ulSvcCount)
									{
										DB_SVC_GetTsInfo(pstUniqData[ulChoosenSvcIdx].usTsIdx, &stOldTsInfo);

									}

									HxSTD_MemSet(&stNewTsInfo, 0, sizeof(DbSvc_TsInfo_t));
									DB_SVC_GetTsInfo(pstUniqData[ulSvcIdx].usTsIdx, &stNewTsInfo);

									hErr = xsvc_si_LcnIsSignalStrongerThanDbTsInfo_UkDtt(&stNewTsInfo, &stOldTsInfo, &bHigh);
									if (TRUE == bHigh)
									{
										ulChoosenSvcIdx = ulSvcIdx;
									}
								}
								HxLOG_Print("ulChoosenSvcIdx(%d)	 BECAUSE< curRegionMatchValue(%d) highRegionMatchValue(%d) > \n",ulChoosenSvcIdx,ulCurRegionMatchValue,ulHighRegionMatchValue);
							}
						}
					}
				}
			}

			{
				HxLOG_Print("index(%d) ulSecondRegionMatchValue(%d)ulSecondHighRegionMatchValue(%d) \n",j,ulSecondRegionMatchValue,ulSecondHighRegionMatchValue);
				// 나머지 level이 낮을 내들 중에.. 같은 애들 HD SD 처리... 
				if (ulSecondRegionMatchValue > ulSecondHighRegionMatchValue)
				{
					ulSecondHighRegionMatchValue = ulSecondRegionMatchValue;
					ulChoosenSubSvcIdx = ulSvcIdx;
				}
				else if (ulSecondRegionMatchValue == ulSecondHighRegionMatchValue)
				{
					DbSvc_Info_t	stNewSvcInfo, stOldSvcInfo;
					Handle_t	hNewSvc = HANDLE_NULL, hOldSvc = HANDLE_NULL;

					hErr = DB_SVC_GetServiceHandle(eDxSvcSection_All, pstUniqData[ulChoosenSubSvcIdx].usTsIdx, pstUniqData[ulChoosenSubSvcIdx].usSvcId, &hOldSvc);
					if(hErr == ERR_OK)
					{
						hErr = DB_SVC_GetServiceInfo(hOldSvc, &stOldSvcInfo);
						if(hErr == ERR_OK)
						{
							hErr = DB_SVC_GetServiceHandle(eDxSvcSection_All, pstUniqData[ulSvcIdx].usTsIdx, pstUniqData[ulSvcIdx].usSvcId, &hNewSvc);
							if(hErr == ERR_OK)
							{
								hErr = DB_SVC_GetServiceInfo(hNewSvc, &stNewSvcInfo);
								if(hErr == ERR_OK)
								{
									if((DbSvc_GetVideoType(&stOldSvcInfo) == eDxVIDEO_TYPE_SD) && (DbSvc_GetVideoType(&stNewSvcInfo) == eDxVIDEO_TYPE_HD))
									{
									
										ulChoosenNewSubSvcIdx = ulSvcIdx;
										HxLOG_Print("SD HD ulChoosenSubSvcIdx(%d)  BECAUSE< ulSecondRegionMatchValue(%d) ulSecondHighRegionMatchValue(%d) > \n",ulChoosenSubSvcIdx,ulSecondRegionMatchValue,ulSecondHighRegionMatchValue);
									}
									else
									{
										DbSvc_TsInfo_t		stNewTsInfo,stOldTsInfo;
										HBOOL				bHigh;

										HxSTD_MemSet(&stOldTsInfo, 0, sizeof(DbSvc_TsInfo_t));
										if (ulChoosenSubSvcIdx < ulSvcCount)
										{
											DB_SVC_GetTsInfo(pstUniqData[ulChoosenSubSvcIdx].usTsIdx, &stOldTsInfo);

										}

										HxSTD_MemSet(&stNewTsInfo, 0, sizeof(DbSvc_TsInfo_t));
										DB_SVC_GetTsInfo(pstUniqData[ulSvcIdx].usTsIdx, &stNewTsInfo);

										hErr = xsvc_si_LcnIsSignalStrongerThanDbTsInfo_UkDtt(&stNewTsInfo, &stOldTsInfo, &bHigh);
										if (TRUE == bHigh)
										{
											ulChoosenNewSubSvcIdx = ulSvcIdx;
										}
									}
									HxLOG_Print("ulChoosenSubSvcIdx(%d)  BECAUSE< ulSecondRegionMatchValue(%d) ulSecondHighRegionMatchValue(%d) > \n" ,ulChoosenSubSvcIdx,ulSecondRegionMatchValue,ulSecondHighRegionMatchValue);
								}
							}
						}
					}
				}
			}
		}

//CONFLICT_CHOOSED:

		if ( INVALID_VALUE != ulChoosenSvcIdx )
		{
			if ( 0 != ulChoosenSvcIdx )
			{
				hErr = DB_SVC_GetServiceHandle(eDxSvcSection_All, pstUniqData[ulStartSvcIdx].usTsIdx, pstUniqData[ulStartSvcIdx].usSvcId, &hFirstSvcHandle);
				if(hErr == ERR_OK)
				{
					hErr = DB_SVC_GetServiceHandle(eDxSvcSection_All, pstUniqData[ulChoosenSvcIdx].usTsIdx, pstUniqData[ulChoosenSvcIdx].usSvcId, &hSelectedSvcHandle);
					if(hErr == ERR_OK)
					{
						hErr = DB_SVC_GetServiceInfo (hFirstSvcHandle, &stSvcInfo1);
						hErr |= DB_SVC_GetServiceInfo (hSelectedSvcHandle, &stSvcInfo2);
						if(hErr == ERR_OK)
						{
							HxLOG_Print("firstSvcInfo usLcn(%d) Lcnflag(%d) szSvcName(%s) onid(%d) tsid(%d) svcid(%d) \n", DbSvc_GetLcn(&stSvcInfo1),DbSvc_GetLcnFlag(&stSvcInfo1),DbSvc_GetName(&stSvcInfo1),DbSvc_GetOnId(&stSvcInfo1),DbSvc_GetTsId(&stSvcInfo1),DbSvc_GetSvcId(&stSvcInfo1));
							HxLOG_Print("SelectedSvcInfo usLcn(%d)	Lcnflag(%d) szSvcName(%s) onid(%d) tsid(%d) svcid(%d) \n",DbSvc_GetLcn(&stSvcInfo2),DbSvc_GetLcnFlag(&stSvcInfo2),DbSvc_GetName(&stSvcInfo2),DbSvc_GetOnId(&stSvcInfo2),DbSvc_GetTsId(&stSvcInfo2),DbSvc_GetSvcId(&stSvcInfo2));

							if((DbSvc_GetVideoType(&stSvcInfo1) == eDxVIDEO_TYPE_SD) && (DbSvc_GetVideoType(&stSvcInfo2) == eDxVIDEO_TYPE_HD))
							{
								if(XdbSvc_TvAnyTimeGetLcnSub(&stSvcInfo2))
									usTempLcn = XdbSvc_TvAnyTimeGetLcnSub(&stSvcInfo2);
								else
									usTempLcn = DbSvc_GetLcn(&stSvcInfo2);

								DbSvc_SetLcn(&stSvcInfo2,DbSvc_GetLcn(&stSvcInfo1));
								DbSvc_SetLcn(&stSvcInfo1,usTempLcn);
							}
							else if((DbSvc_GetVideoType(&stSvcInfo1) == eDxVIDEO_TYPE_HD) && (DbSvc_GetVideoType(&stSvcInfo2) == eDxVIDEO_TYPE_SD))
							{
								usTempLcn = DbSvc_GetLcn(&stSvcInfo2);
								if(XdbSvc_TvAnyTimeGetLcnSub(&stSvcInfo1))
									DbSvc_SetLcn(&stSvcInfo2,XdbSvc_TvAnyTimeGetLcnSub(&stSvcInfo1));
								else
									DbSvc_SetLcn(&stSvcInfo2,DbSvc_GetLcn(&stSvcInfo1));
								DbSvc_SetLcn(&stSvcInfo1,usTempLcn);
							}
							else
							{
								usTempLcn = DbSvc_GetLcn(&stSvcInfo2);
								DbSvc_SetLcn(&stSvcInfo2,DbSvc_GetLcn(&stSvcInfo1));
								DbSvc_SetLcn(&stSvcInfo1,usTempLcn);
							}

							bLcnFlag = DbSvc_GetLcnFlag(&stSvcInfo2);
							DbSvc_SetLcnFlag(&stSvcInfo2,DbSvc_GetLcnFlag(&stSvcInfo1));
							DbSvc_SetLcnFlag(&stSvcInfo1,bLcnFlag);

							DB_SVC_UpdateServiceInfo(hFirstSvcHandle, &stSvcInfo1);
							DB_SVC_UpdateServiceInfo(hSelectedSvcHandle, &stSvcInfo2);
						}
					}
				}
				//제일 우선순위 다음 Lcn 중 Swap 필요한지 확인
				if ( INVALID_VALUE != ulChoosenSubSvcIdx && INVALID_VALUE != ulChoosenNewSubSvcIdx )
				{
					hErr = DB_SVC_GetServiceHandle(eDxSvcSection_All, pstUniqData[ulChoosenSubSvcIdx].usTsIdx, pstUniqData[ulChoosenSubSvcIdx].usSvcId, &hFirstSvcHandle);
					if(hErr == ERR_OK)
					{
						hErr = DB_SVC_GetServiceHandle(eDxSvcSection_All, pstUniqData[ulChoosenNewSubSvcIdx].usTsIdx, pstUniqData[ulChoosenNewSubSvcIdx].usSvcId, &hSelectedSvcHandle);
						if(hErr == ERR_OK)
						{
							hErr = DB_SVC_GetServiceInfo (hFirstSvcHandle, &stSvcInfo1);
							hErr |= DB_SVC_GetServiceInfo (hSelectedSvcHandle, &stSvcInfo2);
							if(hErr == ERR_OK)
							{
								if((LOCAL_MACRO_IS_REGIONAL_ZONE(DbSvc_GetLcn(&stSvcInfo1)))) //assign이 되지 않은 경우에만 swap
								{
									HxLOG_Print("firstSvcInfo SulChoosenSubSvc	Lcn(%d) SubLcn(%d)Lcnflag(%d) szSvcName(%s) onid(%d) tsid(%d) svcid(%d) \n",DbSvc_GetLcn(&stSvcInfo1),XdbSvc_TvAnyTimeGetLcnSub(&stSvcInfo1),DbSvc_GetLcnFlag(&stSvcInfo1),DbSvc_GetName(&stSvcInfo1),DbSvc_GetOnId(&stSvcInfo1),DbSvc_GetTsId(&stSvcInfo1),DbSvc_GetSvcId(&stSvcInfo1));
									HxLOG_Print("SelectedSvcInfo SulChoosenNewSubSvc usLcn(%d) SubLcn(%d)	Lcnflag(%d) szSvcName(%s) onid(%d) tsid(%d) svcid(%d) \n",DbSvc_GetLcn(&stSvcInfo2),XdbSvc_TvAnyTimeGetLcnSub(&stSvcInfo2),DbSvc_GetLcnFlag(&stSvcInfo2),DbSvc_GetName(&stSvcInfo2),DbSvc_GetOnId(&stSvcInfo2),DbSvc_GetTsId(&stSvcInfo2),DbSvc_GetSvcId(&stSvcInfo2));
									if((DbSvc_GetVideoType(&stSvcInfo1) == eDxVIDEO_TYPE_SD) && (DbSvc_GetVideoType(&stSvcInfo2) == eDxVIDEO_TYPE_HD))
									{
										usTempLcn = DbSvc_GetLcn(&stSvcInfo1);
										
										if(XdbSvc_TvAnyTimeGetLcnSub(&stSvcInfo2 )) //HD Have SubLcn
										{
											
											if(DbSvc_GetLcn(&stSvcInfo2) < DbSvc_GetLcn(&stSvcInfo1))
											{
												DbSvc_SetLcn(&stSvcInfo1,DbSvc_GetLcn(&stSvcInfo2 ));
											}
											
											DbSvc_SetLcn(&stSvcInfo2,XdbSvc_TvAnyTimeGetLcnSub(&stSvcInfo2 ));
		
											bLcnFlag = DbSvc_GetLcnFlag(&stSvcInfo2);
											DbSvc_SetLcnFlag(&stSvcInfo1,DbSvc_GetLcnFlag(&stSvcInfo2 ));
											DbSvc_SetLcnFlag(&stSvcInfo2 ,bLcnFlag);
											
											DB_SVC_UpdateServiceInfo(hFirstSvcHandle, &stSvcInfo2 );
											DB_SVC_UpdateServiceInfo(hSelectedSvcHandle, &stSvcInfo1);	
										}
										else
										{
											if(DbSvc_GetLcn(&stSvcInfo2) < DbSvc_GetLcn(&stSvcInfo1))
											{
												DbSvc_SetLcn(&stSvcInfo1,DbSvc_GetLcn(&stSvcInfo2 ));
												DbSvc_SetLcn(&stSvcInfo2 ,usTempLcn);
		
												bLcnFlag = DbSvc_GetLcnFlag(&stSvcInfo2);
												DbSvc_SetLcnFlag(&stSvcInfo1,DbSvc_GetLcnFlag(&stSvcInfo2 ));
												DbSvc_SetLcnFlag(&stSvcInfo2 ,bLcnFlag);
												
												DB_SVC_UpdateServiceInfo(hFirstSvcHandle, &stSvcInfo2 );
												DB_SVC_UpdateServiceInfo(hSelectedSvcHandle, &stSvcInfo1);	
											}
										}							
									}
									else
									{
										//그대로...
									}
								}
							}
						}
					}
				}
				else
				{
					// 그대로....
				}
			}
			else
			{
				// 그대로....
			}
			
			
			{
				HUINT32 	tmpIdx;
				Handle_t	tmpSvcHandle;

				for ( tmpIdx = 0 ; tmpIdx < ulSvcCount ; tmpIdx++ )
				{
					if (tmpIdx != ulChoosenSvcIdx)
					{
						if (pstUniqData[tmpIdx].usSvcId == pstUniqData[ulChoosenSvcIdx].usSvcId)
						{
							tmpSvcHandle = HANDLE_NULL;
							hErr = DB_SVC_GetServiceHandle(eDxSvcSection_All, pstUniqData[tmpIdx].usTsIdx, pstUniqData[tmpIdx].usSvcId, &tmpSvcHandle);
							if ((hErr == ERR_OK) && (tmpSvcHandle != HANDLE_NULL))
							{
								DB_SVC_DeleteServiceInfo(tmpSvcHandle);
							}
						}
					}
				}
			}
		}
		else
		{
			DbSvc_TsInfo_t		stCurTsInfo,stPreTsInfo;
			HBOOL				bHigh;

			HxSTD_memset(&stCurTsInfo,0x0,sizeof(DbSvc_TsInfo_t));			
			HxSTD_memset(&stPreTsInfo,0x0,sizeof(DbSvc_TsInfo_t));

			hSelectedSvcHandle	= HANDLE_NULL;
			for ( ulSvcIdx = ulStartSvcIdx ; ulSvcIdx < ulSvcCount ; ulSvcIdx++ )
			{
				hErr = DB_SVC_GetServiceHandle(eDxSvcSection_All, pstUniqData[ulSvcIdx].usTsIdx, pstUniqData[ulSvcIdx].usSvcId, &hSvc);
				if ( ERR_OK == hErr )
				{
					if ( HANDLE_NULL == hSelectedSvcHandle )
					{
						hSelectedSvcHandle = hSvc;
						hFirstSvcHandle = hSvc;
						hErr = DB_SVC_GetServiceInfo(hSvc, &stSvcInfo1);
						if ( ERR_OK == hErr )
						{
							DB_SVC_GetTsInfo(DbSvc_GetTsIdx(&stSvcInfo1), &stCurTsInfo);
							stPreTsInfo = stCurTsInfo;
							stPreTsInfo.stTuningParam.ter = stCurTsInfo.stTuningParam.ter;
							continue;
						}
					}
					else
					{
						hErr = DB_SVC_GetServiceInfo(hSvc, &stSvcInfo1);
						if ( ERR_OK == hErr )
						{
							DB_SVC_GetTsInfo(DbSvc_GetTsIdx(&stSvcInfo1), &stCurTsInfo);
							hErr = xsvc_si_LcnIsSignalStrongerThanDbTsInfo_UkDtt(&stCurTsInfo, &stPreTsInfo, &bHigh);
							if ( TRUE == bHigh )
							{
								hSelectedSvcHandle = hSvc;
								stPreTsInfo = stCurTsInfo;
								stPreTsInfo.stTuningParam.ter = stCurTsInfo.stTuningParam.ter;
							}
						}
					}
				}
			}
			if (hFirstSvcHandle != hSelectedSvcHandle)
			{
				hErr = DB_SVC_GetServiceInfo(hFirstSvcHandle, &stSvcInfo1);
				if ( ERR_OK == hErr )
				{
					hErr = DB_SVC_GetServiceInfo(hSelectedSvcHandle, &stSvcInfo2);
					if ( ERR_OK == hErr )
					{
						usTempLcn = DbSvc_GetLcn(&stSvcInfo2);
						DbSvc_SetLcn(&stSvcInfo2,DbSvc_GetLcn(&stSvcInfo1));
						DbSvc_SetLcn(&stSvcInfo1,usTempLcn);

						bLcnFlag = DbSvc_GetLcnFlag(&stSvcInfo2);
						DbSvc_SetLcnFlag(&stSvcInfo2,DbSvc_GetLcnFlag(&stSvcInfo1));
						DbSvc_SetLcnFlag(&stSvcInfo1,bLcnFlag);


						DB_SVC_UpdateServiceInfo(hFirstSvcHandle, &stSvcInfo1);
						DB_SVC_UpdateServiceInfo(hSelectedSvcHandle, &stSvcInfo2);
					}
				}
			}
		}
	}

	svc_si_PostMsgToAp(eSEVT_DB_NOTIFY_UPDATED, HANDLE_NULL, 0, 0, 0);
}

/****************************************************************************
Description	:
Parameters	:
Output		:
Return		:
****************************************************************************/
static HERROR xsvc_si_LcnClearConflictSvcHandleTable(void)
{
	HUINT32 i;
	xsvcSi_ConflictServiceList_t *pstSvcList;

	if(s_stUkConflictServiceTable.pstArray)
	{
		for(i = 0; i < s_stUkConflictServiceTable.ulTblNum ; i ++)
		{
			pstSvcList = s_stUkConflictServiceTable.pstArray;

			if(pstSvcList[i].pstUniqDataArray != NULL)
			{
				OxSI_Free (pstSvcList[i].pstUniqDataArray);
				pstSvcList[i].ulArrayLen = 0;
				pstSvcList[i].ulSvcNum = 0;
				pstSvcList[i].usLcn = 0;
			}
		}

		OxSI_Free (s_stUkConflictServiceTable.pstArray);
		s_stUkConflictServiceTable.pstArray = NULL;
	}

	s_stUkConflictServiceTable.ulTblNum = 0;
	s_stUkConflictServiceTable.ulArrayLen = 0;

	return ERR_OK;
}

//_l_conflicttrd_SelectItem (select할 경우 save 해야하는 data add 필요 )
HERROR xsvc_si_LcnSetTrdConflictedList_UkDtt(HUINT32 ulDepthIdx, HUINT32 ulSelectIdx)
{
	HUINT32					ulConvIdx;
	xsvcSi_ConflictListContent_t 	*pConflictContents = NULL;
//	HUINT32					ulConflictCnt = 0;

	pConflictContents = xsvc_si_LcnGetConflictContent();

	pConflictContents->ulCurConflictIdx = ulDepthIdx;
	HxLOG_Print("[%s] pContents->ulCurConflictIdx:%d\n", __FUNCTION__, pConflictContents->ulCurConflictIdx);

	switch ( pConflictContents->ulCurConflictIdx )
	{
		case 0 :
			HxSTD_MemCopy(pConflictContents->szSelCountryCode, pConflictContents->szConflictCountryCode[ulSelectIdx], 4) ;
			xsvc_si_LcnMadeTrdConflictedItem_UkDtt( pConflictContents , ulSelectIdx);
			break;
		case 1 :
			pConflictContents->ulSelPrRgCode = pConflictContents->ulConflictPrRgCode[ulSelectIdx];
			xsvc_si_LcnMadeTrdConflictedItem_UkDtt( pConflictContents , ulSelectIdx);
			break;
		case 2 :
			xsvc_si_LcnGetTrdconflicted_SecondaryCodeIdx_UkDtt ( pConflictContents, ulSelectIdx, &ulConvIdx);
			pConflictContents->ulSelSeRgCode = pConflictContents->ulConflictSeRgCode[ulConvIdx];
			xsvc_si_LcnMadeTrdConflictedItem_UkDtt( pConflictContents , ulSelectIdx);
			break;
		case 3 :
			xsvc_si_LcnGetTrdConflicted_TeritaryCodeIdx_UkDtt ( pConflictContents, ulSelectIdx, &ulConvIdx);
			pConflictContents->ulSelTrRgCode = pConflictContents->ulConflictTrRgCode[ulConvIdx];
			xsvc_si_LcnMadeTrdConflictedItem_UkDtt( pConflictContents , ulSelectIdx);
			break;
		default :
			break;

	}

	return ERR_OK;

}

STATIC void xsvc_si_LcnGetTrdConflicted_PrimaryCodeCnt_UkDtt(xsvcSi_ConflictListContent_t *pContents, HUINT32 *pulCnt)
{
	HUINT32 i,ulCnt=0;

	for ( i = 0 ; i < DxMAX_T_REGION_NUM ; i++ )
	{
		if ( INVALID_VALUE != pContents->ulConflictPrRgCode[i] )
			ulCnt++;
	}

	*pulCnt = ulCnt;
}
STATIC void xsvc_si_LcnGetTrdConflicted_SecondaryCodeCnt_UkDtt(xsvcSi_ConflictListContent_t *pContents, HUINT32 *pulCnt)
{
	HUINT32 i,ulCnt=0;

	for ( i = 0 ; i < DxMAX_T_REGION_NUM ; i++ )
	{
		if ( ( INVALID_VALUE != pContents->ulConflictSeRgCode[i] ) && ( INVALID_VALUE != pContents->ulSelPrRgCode ) )
		{
			if ( ( pContents->ulConflictSeRgCode[i] & 0xFF000000 ) == pContents->ulSelPrRgCode )
				ulCnt++;
		}
		else
		{
			if ( INVALID_VALUE != pContents->ulConflictSeRgCode[i] )
				ulCnt++;
		}
	}

	*pulCnt = ulCnt;
}
STATIC void xsvc_si_LcnGetTrdConflicted_TeritaryCodeCnt_UkDtt(xsvcSi_ConflictListContent_t *pContents, HUINT32 *pulCnt)
{
	HUINT32 i,ulCnt=0;

	for ( i = 0 ; i < DxMAX_T_REGION_NUM ; i++ )
	{
		if ( ( INVALID_VALUE != pContents->ulConflictTrRgCode[i] ) && ( INVALID_VALUE != pContents->ulSelSeRgCode ) )
		{
			if ( ( pContents->ulConflictTrRgCode[i] & 0xFFFF0000 ) == pContents->ulSelSeRgCode )
				ulCnt++;
		}
		else
		{
			if ( INVALID_VALUE != pContents->ulConflictTrRgCode[i] )
				ulCnt++;
		}
	}

	*pulCnt = ulCnt;
}

STATIC HERROR xsvc_si_LcnGetTrdConflicted_RegionName_UkDtt(HUINT32 ulRegionCode, HUINT32 ulBufLen, HUINT8 *pszString)
{
	HINT32		i,nTRgCnt;
	HERROR		hErr = ERR_FAIL;
	DbSvc_T_RegionNameInfo_t	*pstTRgNameInfo=NULL;

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_REGION_NAMEINFO_CNT, &nTRgCnt, 0);

	if ( 0 != nTRgCnt )
	{
		pstTRgNameInfo = (DbSvc_T_RegionNameInfo_t*)OxSI_Calloc(sizeof(DbSvc_T_RegionNameInfo_t) * nTRgCnt);
		if(pstTRgNameInfo == NULL)
		{
			return	ERR_FAIL;
		}

		hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_REGION_NAMEINFO,
													(HUINT32 *)pstTRgNameInfo,
													sizeof(DbSvc_T_RegionNameInfo_t)*nTRgCnt);
		HxLOG_Print("ulSize:%d nTRgCnt:%d\n",sizeof(DbSvc_T_RegionNameInfo_t)*nTRgCnt,nTRgCnt);
		for ( i = 0 ; i < nTRgCnt; i++ )
		{
			if ( ( 0xFF != pstTRgNameInfo[i].ucRegionDepth ) && ( ulRegionCode == pstTRgNameInfo[i].ulRegionCode ) )
			{
				HxLOG_Print ("[%02d] [Country:%s][TRegionDepth:0x%X][TRgName:%s][RCode:0x%X]\n",i,
							pstTRgNameInfo[i].szCountryCode, pstTRgNameInfo[i].ucRegionDepth,
							pstTRgNameInfo[i].szTRegionName, pstTRgNameInfo[i].ulRegionCode);

				HxSTD_StrNCat(pszString, pstTRgNameInfo[i].szTRegionName, ulBufLen);

				OxSI_Free(pstTRgNameInfo);
				return ERR_OK;
			}
		}
	}

	if (NULL != pstTRgNameInfo)
		OxSI_Free(pstTRgNameInfo);

	return  ERR_FAIL;
}

/****************************************************************************
Description	:
Parameters	:
Output		:
Return		:
****************************************************************************/
HERROR xsvc_si_LcnInitConflictSvcHandleTable_UkDtt(void)
{
	HERROR hError = ERR_FAIL;
	HINT32	nTRgCnt = 0,i;

	DbSvc_T_RegionNameInfo_t	*p_stDefaultTRegionNameInfo = NULL;//= {"", 0xFF, 0, "" };

	s_stUkConflictServiceTable.pstArray = NULL;
	s_stUkConflictServiceTable.ulTblNum = 0;
	s_stUkConflictServiceTable.ulArrayLen = 0;

	hError = DB_PARAM_GetItem (eDB_PARAM_ITEM_REGION_NAMEINFO_CNT, &nTRgCnt, 0);
	p_stDefaultTRegionNameInfo = (DbSvc_T_RegionNameInfo_t *)OxAP_Malloc (sizeof(DbSvc_T_RegionNameInfo_t) * nTRgCnt);
	for(i = 0;i < nTRgCnt;i++)
	{
		HxSTD_MemSet(&p_stDefaultTRegionNameInfo[i],0x00, sizeof(DbSvc_T_RegionNameInfo_t));
	}

	hError = DB_PARAM_SetItem(eDB_PARAM_ITEM_REGION_NAMEINFO,(HUINT32) p_stDefaultTRegionNameInfo, (sizeof(DbSvc_T_RegionNameInfo_t)*nTRgCnt));
	hError = DB_PARAM_SetItem(eDB_PARAM_ITEM_REGION_NAMEINFO_CNT,(HUINT32)0, 0);

	return ERR_OK;
}

/******************************************************************************/
/*																			*/
/*	Conflicted Count 수를 가져가는 함수 ( 보여주어야 할 List 갯수 )							*/
/*																			*/
/*																			*/
/******************************************************************************/

STATIC HERROR xsvc_si_LcnGetTrdConflictedCount_UkDtt(HUINT32 *pulConflictTrdCnt)
{
	HUINT32 					ulConflictCnt = 0;
	xsvcSi_ConflictListContent_t 	*pConflictContents	= NULL;

	pConflictContents = xsvc_si_LcnGetConflictContent();


	HxLOG_Print("[%s] pContents->ulCurConflictIdx:%d\n", __FUNCTION__, pConflictContents->ulCurConflictIdx);

	switch ( pConflictContents->ulCurConflictIdx )
	{
		case 0:
			xsvc_si_LcnGetTrdConflictedCountryCnt_UkDtt (pConflictContents, &ulConflictCnt);
			break;
		case 1 :
			xsvc_si_LcnGetTrdConflicted_PrimaryCodeCnt_UkDtt ( pConflictContents, &pConflictContents->ulConflictTrdCnt);
			ulConflictCnt = pConflictContents->ulConflictTrdCnt;
			break;
		case 2 :
			xsvc_si_LcnGetTrdConflicted_SecondaryCodeCnt_UkDtt ( pConflictContents, &pConflictContents->ulConflictTrdCnt);
			ulConflictCnt = pConflictContents->ulConflictTrdCnt;
			break;
		case 3 :
			xsvc_si_LcnGetTrdConflicted_TeritaryCodeCnt_UkDtt ( pConflictContents, &pConflictContents->ulConflictTrdCnt);
			ulConflictCnt = pConflictContents->ulConflictTrdCnt;
			break;
		default :
			pConflictContents->ulConflictTrdCnt = 0;
			ulConflictCnt = pConflictContents->ulConflictTrdCnt;
			break;
	}

	HxLOG_Print("[%s] pContents->ulConflictTrdCnt:%d\n",__FUNCTION__,  pConflictContents->ulConflictTrdCnt);

	if ( 1 <= pConflictContents->ulConflictTrdCnt )
	{
		pConflictContents->ulConflictCnt++;
	}

	HxLOG_Print(C_CODE_F_YELLOW"[%s] pContents->ulCurConflictIdx:%d, pConflictContents->ulConflictCnt:%d"C_CODE_RESET"\n", __FUNCTION__, pConflictContents->ulCurConflictIdx,pConflictContents->ulConflictCnt);
	// Service Update
	if((pConflictContents->ulCurConflictIdx +1) >= pConflictContents->ulConflictCnt)
	{
		xsvc_si_LcnSetTrdConflicttrdSerivice_UkDtt(pConflictContents);
	}


	*pulConflictTrdCnt = ulConflictCnt;


	return ERR_OK;

}
/****************************************************************************/
/*																			*/
/*	Current Selected 된 Trd Info 를 전달 												*/
/*																			*/
/****************************************************************************/

// select 한 Target Region 을 보여주는 부분 Web에서 보여주기 위해 불러야 함.
HERROR xsvc_si_LcnGetTrdSelectedInfo_UkDtt(HUINT8 *pszTRDContent)
{
	// Target Region History Draw
	HxSTD_MemSet( szTRDContent , 0 , (DxMAX_T_REGION_NAME_LEN*3+4) );
	if (INVALID_VALUE != s_stConflictListContents.ulSelTrRgCode)
	{
		xsvc_si_LcnGetTrdConflicted_RegionName_UkDtt (s_stConflictListContents.ulSelTrRgCode, (DxMAX_T_REGION_NAME_LEN*3+3), szTRDContent);
		HxSTD_StrCat(szTRDContent, (const HUINT8 *)", ");
	}
	if (INVALID_VALUE != s_stConflictListContents.ulSelSeRgCode)
	{
		xsvc_si_LcnGetTrdConflicted_RegionName_UkDtt (s_stConflictListContents.ulSelSeRgCode, (DxMAX_T_REGION_NAME_LEN*3+3), szTRDContent);
		HxSTD_StrCat(szTRDContent, (const HUINT8 *)", ");
	}
	if (INVALID_VALUE != s_stConflictListContents.ulSelPrRgCode)
	{
		xsvc_si_LcnGetTrdConflicted_RegionName_UkDtt (s_stConflictListContents.ulSelPrRgCode, (DxMAX_T_REGION_NAME_LEN*3+3), szTRDContent);
		HxSTD_StrCat(szTRDContent, (const HUINT8 *)", ");
	}
	if ( 0 != HxSTD_StrLen(s_stConflictListContents.szSelCountryCode) )
	{
		HxSTD_StrCat(szTRDContent, s_stConflictListContents.szSelCountryCode);
	}
	if ( 0 != HxSTD_StrLen (szTRDContent) )
	{
		HxSTD_MemCopy(pszTRDContent,szTRDContent,DxMAX_T_REGION_NAME_LEN*3+4);
	}
	HxLOG_Print("[%s:%d] s_stConflictListContents.ulConflictCnt:%d s_stConflictListContents.ulCurConflictIdx:%d\n",__FUNCTION__ , __LINE__ ,
					s_stConflictListContents.ulConflictCnt , s_stConflictListContents.ulCurConflictIdx ) ;

	return ERR_OK;

}

/************************************************************************/
/*																		*/
/*	Current Conflicted List 를 전달 												*/
/*																		*/
/*																		*/
/************************************************************************/
HERROR xsvc_si_LcnGetTrdConflictedList_UkDtt(HUINT32 ulDepth,HUINT32 *pulconflictcount,HxList_t **ppConflictList)
{
	HERROR		hError = ERR_FAIL;
	HUINT32 		ulConflictCnt = 0;
	HUINT32		ulConvIdx;
	HUINT32		i;
	HxList_t 		*pConflictList = NULL;

	xsvcSi_ConflictListContent_t *pConflictContents = xsvc_si_LcnGetConflictContent();



	pConflictContents->ulCurConflictIdx = ulDepth;

	if(pConflictContents->ulCurConflictIdx == 0)
	{
		xsvc_si_LcnGetTrdFirstConflictedList_UkDtt();
	}

	hError = xsvc_si_LcnGetTrdConflictedCount_UkDtt(&ulConflictCnt);
	if(hError == ERR_FAIL ||ulConflictCnt == 0 )
	{
		HxLOG_Error("xsvc_si_LcnGetTrdConflictedCount_UkDtt Error ==  hError(%d) ulConflictCnt(%d) \n", hError, ulConflictCnt);
		*pulconflictcount = 0;
		*ppConflictList  = NULL;
		return ERR_FAIL;
	}

	HxLOG_Debug("[%s] TrdConflictCount : (ulConflictCnt : %d) \n",__FUNCTION__, ulConflictCnt);

	// 보여주어야 하는 list string
	for ( i = 0; i < ulConflictCnt; i++ )
	{
		HUINT8		*pucStringBuf = NULL;
		pucStringBuf = (HUINT8 *)OxSI_Malloc(sizeof(HUINT8) * MAX_CONFLICT_SVC_NAME);
		if(pucStringBuf == NULL)
		{
			continue;
		}

		HxLOG_Debug("[%s] pContents->ulCurConflictIdx:%d\n", __FUNCTION__, pConflictContents->ulCurConflictIdx);
		HxSTD_MemSet(pucStringBuf,0x00, sizeof(HUINT8) * MAX_CONFLICT_SVC_NAME);
		switch ( pConflictContents->ulCurConflictIdx )
		{
			case 0 :
				HxSTD_MemCopy( pucStringBuf , pConflictContents->szConflictCountryCode[i], 4);
				break;
			case 1 :
				xsvc_si_LcnGetTrdConflicted_RegionName_UkDtt ( pConflictContents->ulConflictPrRgCode[i] , (MAX_CONFLICT_SVC_NAME-1), pucStringBuf);
				break;
			case 2 :
				xsvc_si_LcnGetTrdconflicted_SecondaryCodeIdx_UkDtt ( pConflictContents, i, &ulConvIdx);
				xsvc_si_LcnGetTrdConflicted_RegionName_UkDtt ( pConflictContents->ulConflictSeRgCode[ulConvIdx] , (MAX_CONFLICT_SVC_NAME-1), pucStringBuf);
				break;
			case 3 :
				xsvc_si_LcnGetTrdConflicted_TeritaryCodeIdx_UkDtt ( pConflictContents, i, &ulConvIdx);
				xsvc_si_LcnGetTrdConflicted_RegionName_UkDtt ( pConflictContents->ulConflictTrRgCode[ulConvIdx] , (MAX_CONFLICT_SVC_NAME-1), pucStringBuf);
				break;
			default:
				break;
		}

		HxLOG_Print("[%s] stringBuf:%s\n", __FUNCTION__, pucStringBuf);

		if(HxSTD_StrLen(pucStringBuf) > 0)
		{
			pConflictList = HLIB_LIST_Append(pConflictList, (void*)pucStringBuf);
		}
		else
		{
			OxSI_Free(pucStringBuf);
		}
	}

	*ppConflictList = pConflictList;
	*pulconflictcount = ulConflictCnt;

	return ERR_OK;
}

// ============================================================================================
// Function : xsvc_si_LcnSetServiceNumber_UkDtt
HERROR xsvc_si_LcnSetServiceNumber_UkDtt (Handle_t hAction, HUINT32 ulNum, DbSvc_Info_t *pstSvcArray, SRCHSVC_AppendFlag_t *peFlagArray)
{
	HERROR			hError;
	HUINT32			 ulIndex;
	DbSvc_Info_t		*pstSvcInfo;
	HxList_t			*pLcnList = NULL;
	HxList_t			*pstItem = NULL;
	DbSvc_TripleId_t  	stTripleId;
	Handle_t			hSvc;
	DbSvc_Info_t		stExistingSvcInfo;
	SRCHSVC_AppendFlag_t				eStatus;
	HUINT32 			j;
	DbSvc_TsInfo_t stNewTsInfo, stDbTsInfo;

	xsvc_si_LcnInitAsnMark();

	//step 1 : NIT로부터 Lcn list를 가져온다.
	if (xsvc_si_LcnGetLogicalChannelFromNit_UkDtt(PAL_PIPE_GetActionId(hAction), &pLcnList) != ERR_OK)
	{
		HxLOG_Error ("\n\t xsvc_si_LcnSetServiceNumber_UkDtt ::: error in xsvc_si_LcnGetLogicalChannelFromNit_UkDtt\n");
	}

	// step 2 : Service 갯수대로 Lcn를 assign한다.(여기서 모든 것들을 체크 )
	for (ulIndex = 0; ulIndex < ulNum; ulIndex++)
	{
		if (peFlagArray[ulIndex] != eSRCHSVC_ADD &&
			peFlagArray[ulIndex] != eSRCHSVC_UPDATE_CHANGING_SVCNUM)
		{
			continue;
		}

		pstSvcInfo = &(pstSvcArray[ulIndex]);

		/* set LCN */
		HLIB_LIST_First(pLcnList);
		for (pstItem = pLcnList; NULL != pstItem; pstItem = pstItem->next)
		{
			xsvcSi_LcnInfo_t *pstLcnInfo = (xsvcSi_LcnInfo_t *)HLIB_LIST_Data(pstItem);

			//1. 이미 존재하는 service 인가 ?
			stTripleId.usOnId  = DbSvc_GetOnId(pstSvcInfo);
			stTripleId.usTsId  = DbSvc_GetTsId(pstSvcInfo);
			stTripleId.usSvcId = DbSvc_GetSvcId(pstSvcInfo);

			if (pstLcnInfo->usTsId == stTripleId.usTsId
				&& pstLcnInfo->usOnId == stTripleId.usOnId
				&& pstLcnInfo->usServiceId == stTripleId.usSvcId)
			{
				DbSvc_SetLcnFlag(pstSvcInfo, TRUE);
				DbSvc_SetLcn(pstSvcInfo, pstLcnInfo->usLogicalChannelNumber);
				XdbSvc_TvAnyTimeSetLcnSub(pstSvcInfo, pstLcnInfo->usLogicalChannelNumberSub);
				DbSvc_SetVisibleFlag(pstSvcInfo, pstLcnInfo->ucVisibleServiceFlag);

				if(pstLcnInfo->ucNumberSelectFlag != FALSE)
				{
					DB_SVC_ServiceSetUserFlag(pstSvcInfo, (const DbSvc_UserFlag1_e)eSvcUserFlag_02_Selectable);
				}
			}
		}


#if defined(CONFIG_DEBUG)
{
		DbSvc_UserFlag1_e bwSvcFlag = 0;
		DB_SVC_ServiceGetUserFlag(&bwSvcFlag, pstSvcInfo);

		HxLOG_Print ("\t===> searched svc info : (0x%x:0x%x:0x%x) LcnFlag(%d) LCN(%04d, 0x%04x) SUB_LCN(%05d, 0x%04x) Type(0x%x) bwSvcFlag(0x%08x) name(%s) \n",
								DbSvc_GetOnId(pstSvcInfo), DbSvc_GetTsId(pstSvcInfo), DbSvc_GetSvcId(pstSvcInfo),
								DbSvc_GetLcnFlag(pstSvcInfo), DbSvc_GetLcn(pstSvcInfo), DbSvc_GetLcn(pstSvcInfo),
								XdbSvc_TvAnyTimeGetLcnSub(pstSvcInfo), XdbSvc_TvAnyTimeGetLcnSub(pstSvcInfo),
								DbSvc_GetSvcType(pstSvcInfo),  bwSvcFlag, DbSvc_GetName(pstSvcInfo));

}
#endif

		stTripleId.usOnId  = DbSvc_GetOnId(pstSvcInfo);
		stTripleId.usTsId  = DbSvc_GetTsId(pstSvcInfo);
		stTripleId.usSvcId = DbSvc_GetSvcId(pstSvcInfo);
		hError = DB_SVC_FindServiceHandleByTripleId (DbSvc_GetDeliTypePtr(pstSvcInfo), stTripleId, &hSvc);
//		HxLOG_Print("hError :%d hSvc : %x\n", hError,hSvc);
//		HxLOG_Print("onid : %d, tsid : %d , svcid : %d\n", stTripleId.usOnId,stTripleId.usTsId,stTripleId.usSvcId);
		if(hError == ERR_OK) //이미 존재하는 service
		{
			// lcn <--> non-lcn인지 등을 확인해야함
			hError = DB_SVC_GetServiceInfo(hSvc, &stExistingSvcInfo);
			if(hError == ERR_OK)
			{
				HxLOG_Print("[case #2-1] usLcn = %d, szSvcName = %s, bLcnFlag = %d (existing svc)\n", DbSvc_GetLcn(&stExistingSvcInfo), DbSvc_GetName(&stExistingSvcInfo), DbSvc_GetLcnFlag(&stExistingSvcInfo));
				/* lcn을 없애놓고 다시 할당하면 됨 */
				hError = DB_SVC_GetTsInfo(DbSvc_GetTsIdx(pstSvcInfo), &stNewTsInfo);
				hError |= DB_SVC_GetTsInfo(DbSvc_GetTsIdx(&stExistingSvcInfo), &stDbTsInfo);
				if (hError != ERR_OK)
					continue;

				if (stNewTsInfo.usTsIdx != stDbTsInfo.usTsIdx) //difference TP
				{
					HxLOG_Print ("\n found same ts info (0x%x, 0x%x)... so skipped.....\n", stNewTsInfo.usTsIdx, stDbTsInfo.usTsIdx);
					peFlagArray[ulIndex] = eSRCHSVC_DONTADD;
				}
				else// same TP
				{
#if 0  //Same TP Same LCN이면 무시해야한다 ? (dynamic update 일 경우 추가해야하는지 확인 필요)
					if(DbSvc_GetLcn(&pstSvcArray[ulIndex]) == DbSvc_GetLcn(pstSvcInfo))
					{
						HxLOG_Print ("\n found same ts info & Same LCN (0x%x, 0x%x)... so skipped.....\n", stNewTsInfo.usTsIdx, stDbTsInfo.usTsIdx);
						continue;
					}
#endif
					if(LOCAL_MACRO_IS_REGIONAL_ZONE(DbSvc_GetLcn(&stExistingSvcInfo)))
							s_bRegionalLcnStatus[(DbSvc_GetLcn(&stExistingSvcInfo) -UK_LCNZONE_REGIONAL_START)] = FALSE;

					DbSvc_SetVisibleFlag(&stExistingSvcInfo,0);
					DbSvc_SetLcn(&stExistingSvcInfo,0);
					DB_SVC_UpdateServiceInfo (hSvc, &stExistingSvcInfo);
					HxLOG_Print("[case #2-2] usLcn = %d, szSvcName = %s, bLcnFlag = %d (searched svc)\n", DbSvc_GetLcn(pstSvcInfo), DbSvc_GetName(pstSvcInfo), DbSvc_GetLcnFlag(pstSvcInfo));
					eStatus = xsvc_si_LcnAssignLcn_UkDtt (pstSvcInfo);
					for (j=0; j<ulIndex; j++)
					{ /* LCN 중복 체크 */
						if (DbSvc_GetLcn(&pstSvcArray[j]) == DbSvc_GetLcn(pstSvcInfo))
						{ /* 중복되었으면 overflow 영역으로 이동 */
							eStatus = xsvc_si_LcnAssignLcn_UkDtt(pstSvcInfo);
							break;
						}
					}

					peFlagArray[ulIndex] = eStatus;
				}
			}
			else
			{
				eStatus = xsvc_si_LcnAssignLcn_UkDtt (pstSvcInfo);
				for (j=0; j<ulIndex; j++)
				{ /* LCN 중복 체크 */
					if (DbSvc_GetLcn(&pstSvcArray[j]) == DbSvc_GetLcn(pstSvcInfo))
					{ /* 중복되었으면 overflow 영역으로 이동 */
						eStatus = xsvc_si_LcnAssignLcn_UkDtt(pstSvcInfo);
						break;
					}
				}
				peFlagArray[ulIndex] = eStatus;
				HxLOG_Print("[case #3-2] usLcn = %d, szSvcName = %s, bLcnFlag = %d (last)\n", DbSvc_GetLcn(pstSvcInfo), DbSvc_GetName(pstSvcInfo), DbSvc_GetLcnFlag(pstSvcInfo));
			}
		}
		else // new service
		{
			eStatus = xsvc_si_LcnAssignLcn_UkDtt (pstSvcInfo);
			// 할당 받은 서비스 넘버보다 앞쪽 번호에 대해서 체크..
			for (j=0; j<ulIndex; j++)
			{
				/* LCN 중복 체크 */
				if (DbSvc_GetLcn(&pstSvcArray[j]) == DbSvc_GetLcn(pstSvcInfo))
				{
					/* 중복되었으면 overflow 영역으로 이동 */
					eStatus = xsvc_si_LcnAssignLcn_UkDtt(pstSvcInfo);
					break;
				}
			}
			peFlagArray[ulIndex] = eStatus;
			HxLOG_Print("[case #3-3] usLcn = %d, szSvcName = %s, bLcnFlag = %d (last)\n", DbSvc_GetLcn(pstSvcInfo), DbSvc_GetName(pstSvcInfo), DbSvc_GetLcnFlag(pstSvcInfo));
		}
		HxLOG_Print("peFlagArray[%d] :%d \n", ulIndex,peFlagArray[ulIndex] );
	}

	if (pLcnList)
	{
		HLIB_LIST_First(pLcnList);
		for (pstItem = pLcnList; NULL != pstItem; pstItem = pstItem->next)
		{
			void *pvData = (void *)HLIB_LIST_Data(pstItem);
			if (pvData)
				OxSI_Free(pvData);
		}

		HLIB_LIST_RemoveAll(pLcnList);
	}

	return ERR_OK;
}


/* end of file */

