/**
	@file     mgr_tplist_default_ter.c
	@brief    file_name & simple comment.

	Description: dimension header file for kraken \n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2008/07/16		init					wjmoh\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#include <octo_common.h>

#include <db_svc.h>
#include <isosvc.h>

#include <bus.h>
#include <mgr_common.h>
#include <mgr_tplist.h>

#include "local_include/_mgr_tplist.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


HERROR mgr_tplist_ConvDefaultTerFrequencyToTuningInfo(const MgrTplist_TerTsInfo_t *pstDefaultFrequency, DbSvc_TuningInfo_t *pstTuningInfo)
{
	if (NULL == pstDefaultFrequency || NULL == pstTuningInfo)
		return ERR_FAIL;

	if(pstDefaultFrequency->ucChannelNo < 100)
	{
		pstTuningInfo->ter.ucCodeRate					= eDxTER_CODERATE_AUTO;
		pstTuningInfo->ter.ucConstellation				= eDxTER_CONSTELLATION_AUTO;
		pstTuningInfo->ter.ucHierachy					= eDxTER_HIERACHY_AUTO;
		pstTuningInfo->ter.ucOffset						= eDxTER_OFFSET_AUTO;
		pstTuningInfo->ter.ucStream						= eDxTER_STREAM_HP;
		pstTuningInfo->ter.ucTransMode					= eDxTER_TRANSMODE_AUTO;
		pstTuningInfo->ter.ucGuardInterval				= eDxTER_GUARDINTERVAL_AUTO;
#if	defined(CONFIG_MW_CH_DVB_T2)
		pstTuningInfo->ter.stT2param.ulPlpId 			= 0x0000FFFF;
		pstTuningInfo->ter.stT2param.ucPreambleFormat 	= eDxTER_SISO;
		pstTuningInfo->ter.stT2param.ucMixed 			= eDxTER_NOT_MIXED;
		pstTuningInfo->ter.stT2param.ucPilotPattern 	= eDxTER_PP_AUTO;
		pstTuningInfo->ter.stT2param.ucExtenedCarrier 	= eDxTER_T2NOT_USED;
		pstTuningInfo->ter.stT2param.ucPAPRreduction	= eDxTER_REDUCTION_NONE;
		pstTuningInfo->ter.stT2param.ulNumPlpBlocks 	= 0;
#endif
	}

	pstTuningInfo->ter.ulFreq		= pstDefaultFrequency->ulFreq;
	pstTuningInfo->ter.ucBandwidth	= pstDefaultFrequency->ucBw;
	pstTuningInfo->ter.ucChanNum	= pstDefaultFrequency->ucChannelNo;
#if	defined(CONFIG_MW_CH_DVB_T2)
	pstTuningInfo->ter.ucSystem		= pstDefaultFrequency->ucSystem;
#endif

	return ERR_OK ;
}


#define _____GLOBAL_FUNCIONS_____

HERROR MGR_TPLIST_TerCountDefaultTpList (HxCountry_e eCountryId, HUINT32 *pulTpNum)
{
	HUINT32			 ulTpNum;
	MgrTplist_TerTsInfo_t	*pstTerTsArray;
	HERROR			 hErr;


	if (pulTpNum == NULL)			{ return ERR_BUS_TARGET_NULL; }

	hErr = mgr_tplist_GetDefaultFrequencyTableByCountryIndex (eCountryId, &pstTerTsArray, &ulTpNum);
	if (hErr != ERR_OK)
	{
		return ERR_BUS_NO_OBJECT;
	}

	*pulTpNum = ulTpNum;

	return ERR_OK;
}


HERROR MGR_TPLIST_TerGetDefaultTpList (HxCountry_e eCountryId, HUINT32 ulTpIdx, DbSvc_TuningInfo_t *pstTuneInfo)
{
	HUINT32					 ulTpNum;
	MgrTplist_TerTsInfo_t	*pstTerTsArray;
	HERROR					 hErr;

	if (pstTuneInfo == NULL)			{ return ERR_BUS_TARGET_NULL; }

	hErr = mgr_tplist_GetDefaultFrequencyTableByCountryIndex (eCountryId, &pstTerTsArray, &ulTpNum);
	if (hErr != ERR_OK)
	{
		return ERR_BUS_NO_OBJECT;
	}

	if (ulTpIdx >= ulTpNum)
	{
		return ERR_BUS_OUT_OF_RANGE;
	}

	HxSTD_memset (pstTuneInfo, 0, sizeof(DbSvc_TuningInfo_t));
	mgr_tplist_ConvDefaultTerFrequencyToTuningInfo((const MgrTplist_TerTsInfo_t *)&(pstTerTsArray[ulTpIdx]), pstTuneInfo);

	return ERR_OK;
}

HERROR MGR_TPLIST_TerGetDefaultChNum (HxCountry_e eCountryId, HUINT32 ulTpIdx, HUINT32 *pulChNum, HUINT8 *pucBW)
{
	HUINT32			 ulTpNum;
	MgrTplist_TerTsInfo_t	*pstTerTsArray;
	HERROR			 hErr;

	if (pulChNum == NULL && pucBW == NULL)			{ return ERR_BUS_TARGET_NULL; }

	hErr = mgr_tplist_GetDefaultFrequencyTableByCountryIndex (eCountryId, &pstTerTsArray, &ulTpNum);
	if (hErr != ERR_OK)
	{
		return ERR_BUS_NO_OBJECT;
	}

	if (ulTpIdx >= ulTpNum)
	{
		return ERR_BUS_OUT_OF_RANGE;
	}

	if (pulChNum != NULL)
	{
		*pulChNum = pstTerTsArray[ulTpIdx].ucChannelNo;
	}

	if (pucBW != NULL)
	{
		*pucBW = pstTerTsArray[ulTpIdx].ucBw;
	}

	return ERR_OK;
}

HERROR MGR_TPLIST_TerGetTpIdxByChNum (HxCountry_e eCountryId, HUINT32 ulChNum, HUINT32 *pulTpIdx)
{
	HUINT32			 ulTpIdx, ulTpNum;
	MgrTplist_TerTsInfo_t	*pstTerTsArray;
	HERROR			 hErr;

	if (pulTpIdx == NULL)			{ return ERR_BUS_TARGET_NULL; }

	hErr = mgr_tplist_GetDefaultFrequencyTableByCountryIndex (eCountryId, &pstTerTsArray, &ulTpNum);
	if (hErr != ERR_OK)
	{
		return ERR_BUS_NO_OBJECT;
	}

	for (ulTpIdx = 0; ulTpIdx < ulTpNum; ulTpIdx++)
	{
		if (pstTerTsArray[ulTpIdx].ucChannelNo == (HUINT8)ulChNum)
		{
			*pulTpIdx = ulTpIdx;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}


HERROR MGR_TPLIST_TerGetTpIdxByFreqency (HxCountry_e eCountryId, HUINT32 ulFrequency, HUINT32 *pulTpIdx)
{
	HUINT32			i;
	HUINT32			ulTotalTp;
	HERROR			hErr;
	MgrTplist_TerTsInfo_t	*pstTerTsArray;

	if (pulTpIdx == NULL)
		return ERR_BUS_TARGET_NULL;

	hErr = mgr_tplist_GetDefaultFrequencyTableByCountryIndex (eCountryId, &pstTerTsArray, &ulTotalTp);
	if (hErr != ERR_OK)
		return ERR_BUS_NO_OBJECT;

	for (i = 0 ; i < ulTotalTp ; i++)
	{
		if (pstTerTsArray[i].ulFreq == ulFrequency)
		{
			*pulTpIdx	= i;

			return ERR_OK;
		}
	}

	return ERR_FAIL;
}


HERROR MGR_TPLIST_TerGetDefaultChName (HxCountry_e eCountryId, HUINT32 ulTpIdx, HUINT8 *szChanName, HINT32 nLen)
{
	HUINT32			 ulTpNum;
	MgrTplist_TerTsInfo_t	*pstTerTsArray;
	HERROR			 hErr;

	if (szChanName == NULL)			{ return ERR_BUS_TARGET_NULL; }

	hErr = mgr_tplist_GetDefaultFrequencyTableByCountryIndex (eCountryId, &pstTerTsArray, &ulTpNum);
	if (hErr != ERR_OK)
	{
		return ERR_BUS_NO_OBJECT;
	}

	if (ulTpIdx >= ulTpNum)
	{
		return ERR_BUS_OUT_OF_RANGE;
	}

	HLIB_STD_StrNCpySafe(szChanName, pstTerTsArray[ulTpIdx].szChannelName, nLen);

	return ERR_OK;
}

HERROR		MGR_TPLIST_TerCheckFreqRangeValid(HxCountry_e eCountryId, HUINT32 ulUserFreq)
{
	HUINT32			i = 0;
	HUINT32			ulNumOfTable = 0;
	MgrTplist_TerTsInfo_t	*pFrequencyTable = NULL;
	HERROR			hErr = ERR_FAIL;

	hErr = mgr_tplist_GetDefaultFrequencyTableByCountryIndex(eCountryId, &pFrequencyTable, &ulNumOfTable);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");

		return ERR_FAIL;
	}

	for(i = 1; i < ulNumOfTable ; i++)
	{
		if((ulUserFreq >= pFrequencyTable[i-1].ulFreq) && (ulUserFreq <= pFrequencyTable[i].ulFreq))
		{
			return ERR_OK;
		}
	}

	HxLOG_Critical("\n\n");

	return ERR_FAIL;
}

HERROR		MGR_TPLIST_TerGetCenterFreqByUserFreq(HxCountry_e eCountryId, HUINT32 ulUserFreq, HUINT32 *pulCenterFreq)
{
	HUINT32			i = 0;
	HUINT32			ulNumOfTable = 0;
	MgrTplist_TerTsInfo_t		*pFrequencyTable = NULL;
	HERROR			hErr = ERR_FAIL;

	hErr = mgr_tplist_GetDefaultFrequencyTableByCountryIndex(eCountryId, &pFrequencyTable, &ulNumOfTable);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");

		return ERR_FAIL;
	}

	for(i = 0; i < ulNumOfTable ; i++)
	{
		if((ulUserFreq >= pFrequencyTable[i].ulFreq - 4000) && (ulUserFreq < pFrequencyTable[i].ulFreq + 4000))
		{

			*pulCenterFreq = pFrequencyTable[i].ulFreq;

			return ERR_OK;
		}
	}

	HxLOG_Critical("\n\n");

	return ERR_FAIL;
}


HERROR		MGR_TPLIST_TerGetChNumByFrequency(HxCountry_e eCountryId, HUINT32 ulFrequency, HUINT32 *pulChNum, HUINT8 * pucBW)
{
	HUINT32			i = 0;
	HUINT32			ulNumOfTable = 0;
	MgrTplist_TerTsInfo_t		*pFrequencyTable = NULL;
	HERROR			hErr = ERR_FAIL;
	MgrTplist_TerTsInfo_t	stTerTsInfo;

	hErr = mgr_tplist_GetDefaultFrequencyTableByCountryIndex(eCountryId, &pFrequencyTable, &ulNumOfTable);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");

		return ERR_FAIL;
	}

#if 0
	/* 절대 테이블이 하나 인 경우는 없으니까 들어오지는 않겠지만 중심 주파수로 생각되는데... */
	if(ulNumOfTable == 1)
	{
		if(ulFrequency >= pFrequencyTable[0] - 4000) && (ulFrequency <= pFrequencyTable[0] + 4000)
		{
			*pulChNum = pFrequencyTable[0].ucChannelNo;
			*pucBW = pFrequencyTable[0].ucBw;

			return ERR_OK;
		}
		else
		{
			return ERR_FAIL;
		}
	}

	for(i = 0; i < ulNumOfTable ; i++)
	{
		if((ulFrequency >= pFrequencyTable[i].ulFreq - 4000) && (ulFrequency < pFrequencyTable[i].ulFreq + 4000))
		{

			*pulChNum = pFrequencyTable[i].ucChannelNo;
			*pucBW = pFrequencyTable[i].ucBw;

			return ERR_OK;
		}
	}
#endif

	if((ulFrequency < pFrequencyTable[0].ulFreq) || (ulFrequency > pFrequencyTable[ulNumOfTable-1].ulFreq))
	{
		HxLOG_Critical("\n\n");

		return ERR_FAIL;
	}

	if(mgr_tplist_GetVHFBandIII(eCountryId, ulFrequency, &stTerTsInfo) == ERR_OK)
	{
		*pulChNum = stTerTsInfo.ucChannelNo;
		*pucBW = stTerTsInfo.ucBw;
		return ERR_OK;
	}

	for(i = 1; i < ulNumOfTable ; i++)
	{
		if((ulFrequency >= pFrequencyTable[i-1].ulFreq) && (ulFrequency <= pFrequencyTable[i].ulFreq))
		{
			if((ulFrequency - pFrequencyTable[i-1].ulFreq) <= (pFrequencyTable[i].ulFreq - ulFrequency))
			{
				*pulChNum = pFrequencyTable[i-1].ucChannelNo;
				*pucBW = pFrequencyTable[i-1].ucBw;
			}
			else
			{
				*pulChNum = pFrequencyTable[i].ucChannelNo;
				*pucBW = pFrequencyTable[i].ucBw;
			}

			return ERR_OK;
		}
	}

	HxLOG_Critical("\n\n");

	return ERR_FAIL;
}

HERROR		MGR_TPLIST_TerGetChNameByFrequency (HxCountry_e eCountryId, HUINT32 ulFrequency, HUINT8 *szChanName)
{
	HUINT32			i = 0;
	HUINT32			ulNumOfTable = 0;
	MgrTplist_TerTsInfo_t		*pFrequencyTable = NULL;
	HERROR			hErr = ERR_FAIL;
	MgrTplist_TerTsInfo_t	stTerTsInfo;

	hErr = mgr_tplist_GetDefaultFrequencyTableByCountryIndex (eCountryId, &pFrequencyTable, &ulNumOfTable);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");

		return ERR_FAIL;
	}

	if((ulFrequency < pFrequencyTable[0].ulFreq) || (ulFrequency > pFrequencyTable[ulNumOfTable-1].ulFreq))
	{
		HxLOG_Critical("\n\n");

		return ERR_FAIL;
	}

	if(mgr_tplist_GetVHFBandIII(eCountryId, ulFrequency, &stTerTsInfo) == ERR_OK)
	{
		MWC_UTIL_DvbStrcpy (szChanName, stTerTsInfo.szChannelName);
		return ERR_OK;
	}

	for(i = 1; i < ulNumOfTable ; i++)
	{
		if((ulFrequency >= pFrequencyTable[i-1].ulFreq) && (ulFrequency <= pFrequencyTable[i].ulFreq))
		{
			if((ulFrequency - pFrequencyTable[i-1].ulFreq) <= (pFrequencyTable[i].ulFreq - ulFrequency))
			{
				MWC_UTIL_DvbStrcpy (szChanName, pFrequencyTable[i-1].szChannelName);
			}
			else
			{
				MWC_UTIL_DvbStrcpy (szChanName, pFrequencyTable[i].szChannelName);
			}

			return ERR_OK;
		}
	}

	HxLOG_Critical("\n\n");

	return ERR_FAIL;
}

HERROR		MGR_TPLIST_TerGetFrequencyByChNum(HxCountry_e eCountryId, HUINT32 ulChNum, HUINT32 *pulFrequency, HUINT8 * pucBW)
{
	HUINT32					i = 0;
	HUINT32					ulNumOfTable = 0;
	MgrTplist_TerTsInfo_t	*pFrequencyTable = NULL;
	HERROR					hErr = ERR_FAIL;

	hErr = mgr_tplist_GetDefaultFrequencyTableByCountryIndex(eCountryId, &pFrequencyTable, &ulNumOfTable);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");

		return ERR_FAIL;
	}

	if((ulChNum < pFrequencyTable[0].ucChannelNo) || (ulChNum > pFrequencyTable[ulNumOfTable-1].ucChannelNo))
	{
		HxLOG_Critical("\n\n");

		return ERR_FAIL;
	}

	for(i = 0; i < ulNumOfTable ; i++)
	{
		if(ulChNum == pFrequencyTable[i].ucChannelNo)
		{
			*pulFrequency = pFrequencyTable[i].ulFreq;
			*pucBW = pFrequencyTable[i].ucBw;

			return ERR_OK;
		}
	}

	HxLOG_Critical("\n\n");
	return ERR_FAIL;
}


HERROR		MGR_TPLIST_TerGetFrequencyByTpIndex(HxCountry_e eCountryId, HUINT32 ulTpIdx, HUINT32 *pulFrequency, HUINT8 * pucBW, HUINT32 *pulChNum)
{
	HUINT32 		ulNumOfTable = 0;
	MgrTplist_TerTsInfo_t	*pFrequencyTable = NULL;
	HERROR			hErr = ERR_FAIL;

	hErr = mgr_tplist_GetDefaultFrequencyTableByCountryIndex(eCountryId, &pFrequencyTable, &ulNumOfTable);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");

		return ERR_FAIL;
	}

	*pulFrequency = pFrequencyTable[ulTpIdx].ulFreq;
	*pucBW = pFrequencyTable[ulTpIdx].ucBw;
	*pulChNum = pFrequencyTable[ulTpIdx].ucChannelNo;

	HxLOG_Critical("\n\n");
	return ERR_OK;
}

HERROR		MGR_TPLIST_TerGetFirstAndLastChNum(HxCountry_e eCountryId, HUINT32 *ulFirstChNum, HUINT32 *ulLastChNum)
{
	HUINT32			ulNumOfTable = 0;
	MgrTplist_TerTsInfo_t		*pFrequencyTable = NULL;
	HERROR			hErr = ERR_FAIL;

	if(ulFirstChNum == NULL && ulLastChNum == NULL)
	{
		HxLOG_Critical("\n\n");

		return ERR_FAIL;
	}

	hErr = mgr_tplist_GetDefaultFrequencyTableByCountryIndex(eCountryId, &pFrequencyTable, &ulNumOfTable);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");

		return ERR_FAIL;
	}

	if(ulFirstChNum != NULL)
		*ulFirstChNum = pFrequencyTable[0].ucChannelNo;

	if(ulLastChNum != NULL)
		*ulLastChNum = pFrequencyTable[ulNumOfTable - 1].ucChannelNo;

	return ERR_OK;
}


HERROR		MGR_TPLIST_TerGetFullInfoByChNum(HxCountry_e eCountryId, HUINT32 ulChNum,HUINT32 *pulFrequency, HUINT8 * pucBW, HUINT8 * pucSystem)
{
	HUINT32 		ulNumOfTable = 0,i;
	MgrTplist_TerTsInfo_t	*pFrequencyTable = NULL;
	HERROR			hErr = ERR_FAIL;

	hErr = mgr_tplist_GetDefaultFrequencyTableByCountryIndex(eCountryId, &pFrequencyTable, &ulNumOfTable);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");

		return ERR_FAIL;
	}


	if((ulChNum < pFrequencyTable[0].ucChannelNo) || (ulChNum > pFrequencyTable[ulNumOfTable-1].ucChannelNo))
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	for(i = 0; i < ulNumOfTable ; i++)
	{
		if(ulChNum == pFrequencyTable[i].ucChannelNo)
		{
			*pulFrequency = pFrequencyTable[i].ulFreq;
			*pucBW = pFrequencyTable[i].ucBw;
			*pucSystem = pFrequencyTable[i].ucSystem;

			return ERR_OK;
		}
	}


	HxLOG_Critical("\n\n");
	return ERR_OK;
}

/* end of file */

