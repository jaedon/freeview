/**
	@file     	xmgr_tplist_freesat.c
	@brief

	Description:  			\n
	Module: 					 					\n
	Remarks : 										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <bus.h>
#include <mgr_common.h>
#include <mgr_tplist.h>
#include "../local_include/_xmgr_tplist.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
#define	MAX_FREESAT_TUNING_INFO		4
STATIC DbSvc_TuningInfo_t s_astFreesatTuningInfo[MAX_FREESAT_TUNING_INFO] =
{
	/* 0 : Not Assigned */
	{	0, },
	/* 1 : Home TP */
	{	11428,	27500,	eDxSAT_POLAR_HOR, eDxSAT_CODERATE_2_3,	eDxSAT_TRANS_DVBS,	eDxSAT_PSK_QPSK,	eDxSAT_PILOT_OFF,	eDxSAT_ROLL_020,	eSatType_ASTRA_2	},
	/* 2 : BBC HD 또는 인증 Suite의 서비스 TP */
	{	10847,	22000,	eDxSAT_POLAR_VER,	eDxSAT_CODERATE_5_6,	eDxSAT_TRANS_DVBS,	eDxSAT_PSK_QPSK,	eDxSAT_PILOT_OFF,	eDxSAT_ROLL_020,	eSatType_ASTRA_2	},
	/* 3 : BBC 1 London */
	{	10773,	22000,	eDxSAT_POLAR_HOR,	eDxSAT_CODERATE_5_6,	eDxSAT_TRANS_DVBS,	eDxSAT_PSK_QPSK,	eDxSAT_PILOT_OFF,	eDxSAT_ROLL_020,	eSatType_ASTRA_2	},
};

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

STATIC HERROR xmgr_tplist_GetFactoryTuningInfo_Freesat(HUINT8 ucType, DbSvc_TuningInfo_t *pstTuningInfo)
{
	if (NULL == pstTuningInfo)
	{
		HxLOG_Print("[%s][%d]: pstTuningInfo == NULL \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	switch (ucType)
	{
	/* Home TP */
	case 1:		*pstTuningInfo = s_astFreesatTuningInfo[1];		break;
	/* BBC HD 또는 인증 Suite의 서비스 TP */
	case 2:		*pstTuningInfo = s_astFreesatTuningInfo[2];		break;
	/* BBC 1 London */
	case 3:		*pstTuningInfo = s_astFreesatTuningInfo[3];		break;
	/* Home TP */
	case 4:		*pstTuningInfo = s_astFreesatTuningInfo[1];		break;
	/* BBC 1 London */
	default:	*pstTuningInfo = s_astFreesatTuningInfo[3];		break;
	}

#if 0
	switch (ucType)
	{
		case 1:		/* Home TP */
			pstTuningInfo->sat.ulFrequency		= 11428;
			pstTuningInfo->sat.ulSymbolRate 	= 27500;
			pstTuningInfo->sat.ePolarization	= eDxSAT_POLAR_HOR;
			pstTuningInfo->sat.eFecCodeRate 	= eDxSAT_CODERATE_2_3;

			pstTuningInfo->sat.eTransSpec		= eDxSAT_TRANS_DVBS;
			pstTuningInfo->sat.ePskMod			= eDxSAT_PSK_QPSK;
			pstTuningInfo->sat.ePilot			= eDxSAT_PILOT_OFF;
			pstTuningInfo->sat.eRollOff 		= eDxSAT_ROLL_020;
			pstTuningInfo->sat.eSatType			= eSatType_ASTRA_2;
			break;

		case 2:		/* BBC HD 또는 인증 Suite의 서비스 TP */
			pstTuningInfo->sat.ulFrequency		= 10847;
			pstTuningInfo->sat.ulSymbolRate 	= 22000;
			pstTuningInfo->sat.ePolarization	= eDxSAT_POLAR_VER;
			pstTuningInfo->sat.eFecCodeRate 	= eDxSAT_CODERATE_5_6;

			pstTuningInfo->sat.eTransSpec		= eDxSAT_TRANS_DVBS;
			pstTuningInfo->sat.ePskMod			= eDxSAT_PSK_QPSK;
			pstTuningInfo->sat.ePilot			= eDxSAT_PILOT_OFF;
			pstTuningInfo->sat.eRollOff 		= eDxSAT_ROLL_020;
			pstTuningInfo->sat.eSatType			= eSatType_ASTRA_2;
			break;

		case 3:		/* BBC 1 London */
			pstTuningInfo->sat.ulFrequency		= 10773;
			pstTuningInfo->sat.ulSymbolRate 	= 22000;
			pstTuningInfo->sat.ePolarization	= eDxSAT_POLAR_HOR;
			pstTuningInfo->sat.eFecCodeRate 	= eDxSAT_CODERATE_5_6;

			pstTuningInfo->sat.eTransSpec		= eDxSAT_TRANS_DVBS;
			pstTuningInfo->sat.ePskMod			= eDxSAT_PSK_QPSK;
			pstTuningInfo->sat.ePilot			= eDxSAT_PILOT_OFF;
			pstTuningInfo->sat.eRollOff 		= eDxSAT_ROLL_020;
			pstTuningInfo->sat.eSatType			= eSatType_ASTRA_2;
			break;

		case 4:		/* Home TP */
			pstTuningInfo->sat.ulFrequency		= 11428;
			pstTuningInfo->sat.ulSymbolRate 	= 27500;
			pstTuningInfo->sat.ePolarization	= eDxSAT_POLAR_HOR;
			pstTuningInfo->sat.eFecCodeRate 	= eDxSAT_CODERATE_5_6;

			pstTuningInfo->sat.eTransSpec		= eDxSAT_TRANS_DVBS;
			pstTuningInfo->sat.ePskMod			= eDxSAT_PSK_QPSK;
			pstTuningInfo->sat.ePilot			= eDxSAT_PILOT_OFF;
			pstTuningInfo->sat.eRollOff 		= eDxSAT_ROLL_020;
			pstTuningInfo->sat.eSatType			= eSatType_ASTRA_2;
			break;

		default:	/* BBC 1 London */
			pstTuningInfo->sat.ulFrequency		= 10773;
			pstTuningInfo->sat.ulSymbolRate 	= 22000;
			pstTuningInfo->sat.ePolarization	= eDxSAT_POLAR_HOR;
			pstTuningInfo->sat.eFecCodeRate 	= eDxSAT_CODERATE_5_6;

			pstTuningInfo->sat.eTransSpec		= eDxSAT_TRANS_DVBS;
			pstTuningInfo->sat.ePskMod			= eDxSAT_PSK_QPSK;
			pstTuningInfo->sat.ePilot			= eDxSAT_PILOT_OFF;
			pstTuningInfo->sat.eRollOff 		= eDxSAT_ROLL_020;
			pstTuningInfo->sat.eSatType		= eSatType_ASTRA_2;
			break;
	}
#endif

	return ERR_OK;
}

STATIC HERROR xmgr_tplist_GetAntennaIndex_Freesat (SVC_SatType_t eSatType, HUINT32 *pulAntIdx)
{
	HBOOL			 bFound;
	HUINT32			 ulAntIdx;
	HINT32			 nAntCnt, nTotalAntNum;
	HINT32			*pnAntIdxArray;
	DbSvc_AntInfo_t		 stAntInfo;
	HERROR			 hErr;

	if (pulAntIdx == NULL)				{ return ERR_BUS_TARGET_NULL; }

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
				if (stAntInfo.eSatType == eSatType)
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
		HxLOG_Critical("\n\n\n");
		return ERR_FAIL;
	}

	*pulAntIdx = ulAntIdx;
	return ERR_OK;
}

STATIC HERROR xmgr_tplist_GetHomeTpListOta_Freesat(DbSvc_TsInfo_t **ppstTsList, HUINT32 *pulNumTs)
{
	HINT32	nValue, ulNumTs, ulCountTs;
	HUINT32					  ulAntIdx;

	HERROR	hError;
	HUINT32	nSatTpNum = 0;
	DbSvc_TuningInfo_t	*pSatTpInfo = NULL;
	DbSvc_TuningInfo_t	stNewTuningInfo;
	DbSvc_TsInfo_t		stTsInfo;
	DbSvc_TsInfo_t		*astTsInfo;

	// Freesat default tp 3개로 임시 HardCoding.
	ulNumTs = 3;
	ulCountTs = 0;

	astTsInfo = (DbSvc_TsInfo_t *)HLIB_STD_MemAlloc(sizeof(DbSvc_TsInfo_t)*ulNumTs);
	if (NULL == astTsInfo)
	{
		HxLOG_Critical("[xmgr_tplist_GetHomeTpListOta_Freesat] mem alloc error\n");
		return ERR_FAIL;
	}

	HxSTD_MemSet(astTsInfo, sizeof(DbSvc_TsInfo_t)*ulNumTs, 0);

	xmgr_tplist_GetAntennaIndex_Freesat (eSatType_ASTRA_2, &ulAntIdx);

	// HOME TP1 // Hardcoding 된 값을 저장하는 것은 여기서만 하도록 한다.
	hError = DB_PARAM_GetItem(eDB_PARAM_ITEM_HOME_TP_INFO_1, (HUINT32 *) &stNewTuningInfo.sat, sizeof(DbSvc_SatTuningInfo_t));
	if ((hError != ERR_OK) || (stNewTuningInfo.sat.ulFrequency == 0))
	{
		xmgr_tplist_GetFactoryTuningInfo_Freesat(1, &stNewTuningInfo);
		nValue = sizeof(DbSvc_SatTuningInfo_t);
		DB_PARAM_SetItem(eDB_PARAM_ITEM_HOME_TP_INFO_1, (HUINT32)&stNewTuningInfo.sat, nValue);
	}

	astTsInfo[ulCountTs].eDeliType					= eDxDELIVERY_TYPE_SAT;
	HxSTD_MemCopy(&(astTsInfo[ulCountTs].stTuningParam), &stNewTuningInfo, sizeof(DbSvc_TuningInfo_t));
	astTsInfo[ulCountTs].stTuningParam.sat.ucAntId	= (HUINT8)ulAntIdx;
	ulCountTs++;

	// HOME TP2
	hError = DB_PARAM_GetItem(eDB_PARAM_ITEM_HOME_TP_INFO_2, (HUINT32 *) &stNewTuningInfo.sat, sizeof(DbSvc_SatTuningInfo_t));
	if ((hError != ERR_OK) || (stNewTuningInfo.sat.ulFrequency == 0))
	{
		xmgr_tplist_GetFactoryTuningInfo_Freesat(2, &stNewTuningInfo);
		nValue = sizeof(DbSvc_SatTuningInfo_t);
		DB_PARAM_SetItem(eDB_PARAM_ITEM_HOME_TP_INFO_2, (HUINT32)&stNewTuningInfo.sat, nValue);
	}

	astTsInfo[ulCountTs].eDeliType					= eDxDELIVERY_TYPE_SAT;
	HxSTD_MemCopy(&(astTsInfo[ulCountTs].stTuningParam), &stNewTuningInfo, sizeof(DbSvc_TuningInfo_t));
	astTsInfo[ulCountTs].stTuningParam.sat.ucAntId	= (HUINT8)ulAntIdx;
	ulCountTs++;

	// HOME TP3
	hError = DB_PARAM_GetItem(eDB_PARAM_ITEM_HOME_TP_INFO_3, (HUINT32 *) &stNewTuningInfo.sat, sizeof(DbSvc_SatTuningInfo_t));
	if ((hError != ERR_OK) || (stNewTuningInfo.sat.ulFrequency == 0))
	{
		xmgr_tplist_GetFactoryTuningInfo_Freesat(3, &stNewTuningInfo);
		nValue = sizeof(DbSvc_SatTuningInfo_t);
		DB_PARAM_SetItem(eDB_PARAM_ITEM_HOME_TP_INFO_3, (HUINT32)&stNewTuningInfo.sat, nValue);
	}

	astTsInfo[ulCountTs].eDeliType					= eDxDELIVERY_TYPE_SAT;
	HxSTD_MemCopy(&(astTsInfo[ulCountTs].stTuningParam), &stNewTuningInfo, sizeof(DbSvc_TuningInfo_t));
	astTsInfo[ulCountTs].stTuningParam.sat.ucAntId	= (HUINT8)ulAntIdx;


	*ppstTsList = astTsInfo;
	*pulNumTs = ulNumTs;

	return ERR_OK;
}

HERROR	xmgr_tplist_GetHomeTpList_Freesat (MgrTplist_HomeTsKind_e eHomeTsKind, DbSvc_TsInfo_t **ppstTsList, HUINT32 *pulNumTs)
{
	HERROR		hErr = ERR_FAIL;

	if (NULL == pulNumTs)		return ERR_FAIL;

	switch (eHomeTsKind)
	{
	case eHomeTs_Search:
	case eHomeTs_OTA:		hErr = xmgr_tplist_GetHomeTpListOta_Freesat(ppstTsList, pulNumTs);		break;
	default:				break;
	}

	return hErr;
}

HERROR XMGR_TPLIST_GetFactoryTuningInfo_Freesat(HUINT8 ucType, DbSvc_TuningInfo_t *pstTuningInfo)
{
	return xmgr_tplist_GetFactoryTuningInfo_Freesat(ucType, pstTuningInfo);
}

/*********************** End of File ******************************/
