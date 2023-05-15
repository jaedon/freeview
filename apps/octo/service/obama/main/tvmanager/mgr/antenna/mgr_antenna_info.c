/**
	@file     mgr_antenna_info.c
	@brief    file_name & simple comment.
	위성, 안테나 등의 각종 유틸리티적인 기능을 제공하는 함수들을 모은 파일

	Description: Application for Navigation Settings Menu\n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2008/08/05		initiated					wjmoh\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <db_svc.h>
#include <db_param.h>

//#include <mwc_util.h>

#include <svc_pipe.h>
#include <svc_resources.h>
#include <svc_ch.h>

#include <bus.h>

#include <mgr_common.h>
#include <mgr_tplist.h>
#include <mgr_antenna.h>
#include <mgr_search.h>

#if defined(CONFIG_MW_CH_SATELLITE)

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define	INDEX_MOTOR_USERDEFINE			29
#define	MAX_NUM_MOTOR_SATELLITE			49

#define DEGREE_TYPE_LONGITUDE			0x01
#define DEGREE_TYPE_LATITUDE			0x02

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HUINT16		 usAntId;
	DbSvc_AntInfo_t	 stAntInfo;
	HBOOL		 bUpdated;
	HBOOL		 bSameSatIdx;
	HUINT32		 ulCount;
} MgrAnt_Info_t;

typedef struct
{
	SVC_SatType_t		 eSatType;
	HUINT8 				 ucMotorPosition;
} MgrAnt_MoterPos_t;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HBOOL			 s_bSearchAnt_NewAntAvailable = FALSE;
STATIC DbSvc_UiAntType_e		 s_eSearchAnt_AntType = eUiAntType_LNB_Only;
STATIC DbSvc_AntInfo_t		 s_stSearchAnt_AntInfo;

/*Pre-defined된 것 이외의 것들의 위해 사용*/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
#endif

/**
  @remark
  @remark
  @return
*/
HBOOL MGR_ANTENNA_IsSatTypeUserAntennaType (SVC_SatType_t eSatType)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	return (eSatType >= eSatType_USERANT_1 && eSatType <= eSatType_USERANT_MAX) ? TRUE : FALSE;
#else
	return FALSE;
#endif
}

/**
  @remark
  @remark
  @return
*/
HERROR MGR_ANTENNA_GetLnbId (HUINT16 *pusAntIdx)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	HUINT32		 ulAntNum;
	HUINT16		 usAntIdx;
	HINT32		*pnAntIdxArray;
	DbSvc_AntInfo_t	 stAntInfo;
	HERROR		 hErr;

	if (pusAntIdx == NULL)					{ return ERR_FAIL; }

	pnAntIdxArray = NULL;
	hErr = DB_SVC_FindAllAntIdxList (&ulAntNum, &pnAntIdxArray);
	if (hErr != ERR_OK || ulAntNum == 0 || pnAntIdxArray == NULL)
	{
		if (pnAntIdxArray)
		{
			DB_SVC_FreeAntList (pnAntIdxArray);
		}
		HxLOG_Critical("\n\n\n");
		return ERR_FAIL;
	}

	for (usAntIdx = 0; usAntIdx < ulAntNum; usAntIdx++)
	{
		hErr = DB_SVC_GetAntInfo ((HUINT16)pnAntIdxArray[usAntIdx], &stAntInfo);
		if (hErr == ERR_OK)
		{
			if (stAntInfo.eAntennaType == eDxANT_TYPE_LNB_ONLY)
			{
				*pusAntIdx = (HUINT16)pnAntIdxArray[usAntIdx];
				DB_SVC_FreeAntList (pnAntIdxArray);
				return ERR_OK;
			}
		}
	}

	DB_SVC_FreeAntList (pnAntIdxArray);
	return ERR_FAIL;
#else
	return ERR_FAIL;
#endif
}

HERROR MGR_ANTENNA_GetDiSEqCAntennaId (HUINT16 usSequenceIdx, HUINT16 *pusAntIdx)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	HUINT32		 ulAntNum;
	HUINT16		 usAntIdx;
	HINT32		*pnAntIdxArray;
	HINT16		 count = 0;
	DbSvc_AntInfo_t	 stAntInfo;
	HERROR		 hErr;

	if (pusAntIdx == NULL)					{ return ERR_FAIL; }

	pnAntIdxArray = NULL;
	hErr = DB_SVC_FindAllAntIdxList (&ulAntNum, &pnAntIdxArray);
	if (hErr != ERR_OK || ulAntNum == 0 || pnAntIdxArray == NULL)
	{
		HxLOG_Critical("\n\n\n");
		DB_SVC_FreeAntList (pnAntIdxArray);
		return ERR_FAIL;
	}

	for (usAntIdx = 0; usAntIdx < ulAntNum; usAntIdx++)
	{
		hErr = DB_SVC_GetAntInfo ((HUINT16)pnAntIdxArray[usAntIdx], &stAntInfo);
		if (hErr == ERR_OK)
		{
			if (stAntInfo.eAntennaType == eDxANT_TYPE_DISEQC)
			{
				count++;
				if (count == usSequenceIdx)
				{
					*pusAntIdx = (HUINT16)pnAntIdxArray[usAntIdx];
					DB_SVC_FreeAntList (pnAntIdxArray);
					return ERR_OK;
				}
			}
		}
	}

	DB_SVC_FreeAntList (pnAntIdxArray);
	return ERR_FAIL;
#else
	return ERR_FAIL;
#endif
}

/**
  @remark
  @remark
  @return
*/
HERROR MGR_ANTENNA_GetIdBySwitchOptions (HUINT32 ulDiseqcInput, HUINT32 ul22KhzTone, HUINT32 ulToneBurst, HUINT32 *pulAntIdx)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	HUINT32		 ulAntIdx, ulAntNum;
	HINT32		*pnAntIdxArray;
	DbSvc_AntInfo_t	 stAntInfo;
	HERROR		 hErr;

	if (pulAntIdx == NULL)					{ return ERR_FAIL; }

	pnAntIdxArray = NULL;
	hErr = DB_SVC_FindAllAntIdxList (&ulAntNum, &pnAntIdxArray);
	if (hErr != ERR_OK || ulAntNum == 0 || pnAntIdxArray == NULL)
	{
		HxLOG_Critical("\n\n\n");
		DB_SVC_FreeAntList (pnAntIdxArray);
		return ERR_FAIL;
	}

	// Antenna Index 는 1부터
	for (ulAntIdx = 0; ulAntIdx < ulAntNum; ulAntIdx++)
	{
		hErr = DB_SVC_GetAntInfo ((HUINT16)pnAntIdxArray[ulAntIdx], &stAntInfo);
		if (hErr == ERR_OK)
		{
			if (stAntInfo.eAntennaType != eDxANT_TYPE_DISEQC)			{ continue; }

			if ((HUINT32)stAntInfo.utAntInfo.stDiseqcAnt.eDiseqcInput == ulDiseqcInput &&
				(HUINT32)stAntInfo.utAntInfo.stDiseqcAnt.b22kTone == ul22KhzTone &&
				(HUINT32)stAntInfo.utAntInfo.stDiseqcAnt.eToneBurst == ulToneBurst)
			{
				*pulAntIdx = (HUINT32)pnAntIdxArray[ulAntIdx];
				DB_SVC_FreeAntList (pnAntIdxArray);
				return ERR_OK;
			}
		}
	}

	DB_SVC_FreeAntList (pnAntIdxArray);
	return ERR_FAIL;
#else
	return ERR_FAIL;
#endif
}

HERROR MGR_ANTENNA_GetScdId (DxSAT_ScdInput_e eScdInput, HUINT32 *pulAntIdx)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	HUINT32		 ulAntIdx, ulAntNum;
	HINT32		*pnAntIdxArray;
	DbSvc_AntInfo_t	 stAntInfo;
	HERROR		 hErr;

	if (pulAntIdx == NULL)					{ return ERR_FAIL; }

	pnAntIdxArray = NULL;
	hErr = DB_SVC_FindAllAntIdxList (&ulAntNum, &pnAntIdxArray);
	if (hErr != ERR_OK || ulAntNum == 0 || pnAntIdxArray == NULL)
	{
		if (pnAntIdxArray)
		{
			DB_SVC_FreeAntList (pnAntIdxArray);
		}
		HxLOG_Critical("\n\n\n");
		return ERR_FAIL;
	}

	// Antenna Index 는 1부터
	for (ulAntIdx = 0; ulAntIdx < ulAntNum; ulAntIdx++)
	{
		hErr = DB_SVC_GetAntInfo ((HUINT16)pnAntIdxArray[ulAntIdx], &stAntInfo);
		if (hErr == ERR_OK)
		{
			if (stAntInfo.eAntennaType != eDxANT_TYPE_SCD)			{ continue; }

			if (stAntInfo.utAntInfo.stScdAnt.eScdInput == eScdInput)
			{
				*pulAntIdx = (HUINT32)pnAntIdxArray[ulAntIdx];
				DB_SVC_FreeAntList (pnAntIdxArray);
				return ERR_OK;
			}
		}
	}

	DB_SVC_FreeAntList (pnAntIdxArray);
	return ERR_FAIL;
#else
	return ERR_FAIL;
#endif
}

/* AP_SPEC으로 ????
*/
HERROR MGR_ANTENNA_MakeDefaultInfo (DbSvc_AntInfo_t *pstAntInfo)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	if (pstAntInfo == NULL)				{ return ERR_FAIL; }

	HxSTD_memset (pstAntInfo, 0, sizeof(DbSvc_AntInfo_t));

	MGR_ANTENNA_GetDefaultInfo (&pstAntInfo->eSatType);
	pstAntInfo->eAntennaType				 = eDxANT_TYPE_LNB_ONLY;
	pstAntInfo->utAntInfo.stLnbAnt.ulLnbFreq	 = CH_UNIVERSAL_LNB_FREQ;
	pstAntInfo->utAntInfo.stLnbAnt.eLnbVoltage = eDxLNB_VOLT_STD;

	MGR_ANTENNA_GetSatelliteName (pstAntInfo->eSatType, pstAntInfo->szName, MAX_ANT_NAME);
	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}

/**
    @remark   Initialize Antenna Information structure.
                    Default antenna type is LNB ONLY.
    @return	ERR_OK or ERR_FAIL
*/
HERROR MGR_ANTENNA_InitInfo (DbSvc_AntInfo_t *pstAntInfo)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	if (pstAntInfo == NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_memset (pstAntInfo, 0, sizeof(DbSvc_AntInfo_t));

	pstAntInfo->eSatType			 = eSatType_NONE;
	pstAntInfo->eAntennaType		 = eDxANT_TYPE_LNB_ONLY;
	pstAntInfo->utAntInfo.stLnbAnt.ulLnbFreq	 = CH_UNIVERSAL_LNB_FREQ;
	pstAntInfo->utAntInfo.stLnbAnt.eLnbVoltage = eDxLNB_VOLT_STD;

	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}

HERROR MGR_ANTENNA_GetNewInfo (DbSvc_AntInfo_t *pstAntInfo)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	if (pstAntInfo == NULL)				{ return ERR_FAIL; }

	if (s_bSearchAnt_NewAntAvailable == TRUE)
	{
		HxSTD_memcpy (pstAntInfo, &s_stSearchAnt_AntInfo, sizeof(DbSvc_AntInfo_t));
	}
	else
	{
		MGR_ANTENNA_MakeDefaultInfo (pstAntInfo);
	}

	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}

HERROR MGR_ANTENNA_SetNewInfo (DbSvc_AntInfo_t *pstAntInfo)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	if (pstAntInfo == NULL)				{ return ERR_BUS_SOURCE_NULL; }

	HxSTD_memcpy (&s_stSearchAnt_AntInfo, pstAntInfo, sizeof(DbSvc_AntInfo_t));
	s_bSearchAnt_NewAntAvailable = TRUE;

	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}

HERROR MGR_ANTENNA_GetType (DbSvc_UiAntType_e *peAntType)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	if (peAntType == NULL)				{ return ERR_FAIL; }

	*peAntType = s_eSearchAnt_AntType;
	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}

HERROR MGR_ANTENNA_SetType (DbSvc_UiAntType_e eAntType)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	s_eSearchAnt_AntType = eAntType;
	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}

#if 0
HERROR MGR_ANTENNA_RenameAllAntInfo (void)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	HBOOL			 bUpdated;
	HINT32			 nAntIdx, nNextAntIdx;
	HINT32			 nAntNum, nAntRealNum;
	HUINT32			 ulNextCount;
	HINT32			*pnAntIdArray;
	MgrAnt_Info_t	*pstAntInfoArray=NULL;
	HUINT8			 szSatOrgName[MAX_ANT_NAME + 16], szSatName[MAX_ANT_NAME + 16];
	HERROR			 hErr;

	pnAntIdArray = NULL;
	hErr = DB_SVC_FindAllAntIdxList (&nAntNum, &pnAntIdArray);
	if (hErr != ERR_OK || nAntNum <= 0 || pnAntIdArray == NULL)
	{
		HxLOG_Critical("\n\n\n");
		hErr = ERR_FAIL;
		goto Error;
	}

	pstAntInfoArray = (MgrAnt_Info_t *)OxAP_Malloc (sizeof(MgrAnt_Info_t) * nAntNum);
	if (pstAntInfoArray == NULL)
	{
		HxLOG_Critical("\n\n\n");
		hErr = ERR_FAIL;
		goto Error;
	}

	nAntRealNum = 0;

	for (nAntIdx = 0; nAntIdx < nAntNum; nAntIdx++)
	{
		hErr = DB_SVC_GetAntInfo ((HUINT16)pnAntIdArray[nAntIdx], &(pstAntInfoArray[nAntRealNum].stAntInfo));
		if (hErr == ERR_OK)
		{
			pstAntInfoArray[nAntRealNum].usAntId = (HUINT16)pnAntIdArray[nAntIdx];
			pstAntInfoArray[nAntRealNum].bSameSatIdx = FALSE;
			pstAntInfoArray[nAntRealNum].bUpdated = FALSE;
			pstAntInfoArray[nAntRealNum].ulCount = 1;

			nAntRealNum++;
		}
	}

	if (nAntRealNum == 0)
	{
		HxLOG_Critical("\n\n\n");
		hErr = ERR_FAIL;
		goto Error;
	}

	for (nAntIdx = 0; nAntIdx < nAntRealNum; nAntIdx++)
	{
		if (pstAntInfoArray[nAntIdx].bSameSatIdx == TRUE)
		{
			continue;
		}

		ulNextCount = 2;

		for (nNextAntIdx = nAntIdx + 1; nNextAntIdx < nAntRealNum; nNextAntIdx++)
		{
			if (pstAntInfoArray[nAntIdx].stAntInfo.eSatType == pstAntInfoArray[nNextAntIdx].stAntInfo.eSatType)
			{
				pstAntInfoArray[nAntIdx].bSameSatIdx = TRUE;

				pstAntInfoArray[nNextAntIdx].bSameSatIdx = TRUE;
				pstAntInfoArray[nNextAntIdx].ulCount = ulNextCount;
				ulNextCount++;
			}
		}
	}

	bUpdated = FALSE;

	for (nAntIdx = 0; nAntIdx < nAntRealNum; nAntIdx++)
	{
		MGR_ANTENNA_GetSatelliteName (pstAntInfoArray[nAntIdx].stAntInfo.eSatType, szSatOrgName, MAX_ANT_NAME+16);
		if (pstAntInfoArray[nAntIdx].bSameSatIdx == TRUE)
		{
			snprintf (szSatName, MAX_ANT_NAME + 16 -1, "%s (%d)", szSatOrgName, pstAntInfoArray[nAntIdx].ulCount);
		}
		else
		{
			HLIB_STD_StrNCpySafe(szSatName, szSatOrgName, MAX_ANT_NAME + 16);
		}

		if (HxSTD_StrNCmp(szSatName, pstAntInfoArray[nAntIdx].stAntInfo.szName, MAX_ANT_NAME) != 0)
		// Name Changed:
		{
			bUpdated = TRUE;
			HLIB_STD_StrUtf8NCpy(pstAntInfoArray[nAntIdx].stAntInfo.szName, szSatName, MAX_ANT_NAME);
			DB_SVC_UpdateAntInfo (pstAntInfoArray[nAntIdx].usAntId, &(pstAntInfoArray[nAntIdx].stAntInfo));
		}
	}

	if (bUpdated == TRUE)
	{
		DB_PARAM_Sync();
		DB_SVC_AntennaSync();
	}

	hErr = ERR_OK;

Error:
	if (pnAntIdArray != NULL)
	{
		DB_SVC_FreeAntList (pnAntIdArray);
	}

	if (pstAntInfoArray != NULL)
	{
		OxAP_Free (pstAntInfoArray);
	}

	return hErr;
#else
	return ERR_FAIL;
#endif
}

HERROR MGR_ANTENNA_DeleteAllAntRelatedInfo (HBOOL bSyncSetupDB, HBOOL bSyncSvcDB)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	DB_SVC_DeleteAllNetworkService(eDxDELIVERY_TYPE_SAT);
	DB_SVC_DeleteAllAntInfo();

	if (TRUE == bSyncSvcDB)
	{
		DB_SVC_Sync(eSyncMode);
	}

	if (TRUE == bSyncSetupDB)
	{
		DB_PARAM_Sync();
		DB_SVC_AntennaSync();
	}

	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}
#endif

#define ___MOTOR_ANTENNA_____

HERROR MGR_ANTENNA_UpdateMotorArguments (HUINT32 ulLnbFreq, HBOOL bLnbPower, DxSAT_DiseqcVer_e eMotorType)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	HUINT32		 ulAntIdx, ulAntNum;
	HINT32		*pnAntIdxArray;
	DbSvc_AntInfo_t	 stAntInfo;
	HERROR		 hErr, hRet = ERR_FAIL;

	if (eMotorType != eDxDISEQC_VER_1_2 && eMotorType != eDxDISEQC_VER_1_3)
	{
		return ERR_FAIL;
	}

	pnAntIdxArray = NULL;
	hErr = DB_SVC_FindAllAntIdxList (&ulAntNum, &pnAntIdxArray);
	if (hErr != ERR_OK || ulAntNum == 0 || pnAntIdxArray == NULL)
	{
		DB_SVC_FreeAntList (pnAntIdxArray);
		return ERR_FAIL;
	}

	for (ulAntIdx = 0; ulAntIdx < ulAntNum; ulAntIdx++)
	{
		hErr = DB_SVC_GetAntInfo ((HUINT16)pnAntIdxArray[ulAntIdx], &stAntInfo);
		if (hErr == ERR_OK)
		{
			if (stAntInfo.eAntennaType != eDxANT_TYPE_DISEQC)			{ continue; }
			if (stAntInfo.utAntInfo.stDiseqcAnt.eDiseqcVer != eDxDISEQC_VER_1_2 &&
				stAntInfo.utAntInfo.stDiseqcAnt.eDiseqcVer != eDxDISEQC_VER_1_3)
			{
				continue;
			}

			stAntInfo.utAntInfo.stDiseqcAnt.ulLnbFreq = ulLnbFreq;
			stAntInfo.utAntInfo.stDiseqcAnt.eLnbVoltage = (bLnbPower == TRUE) ? eDxLNB_VOLT_HIGH : eDxLNB_VOLT_STD;
			stAntInfo.utAntInfo.stDiseqcAnt.eDiseqcVer = eMotorType;

			DB_SVC_UpdateAntInfo ((HUINT16)pnAntIdxArray[ulAntIdx], &stAntInfo);
			hRet = ERR_OK;
		}
	}

	DB_SVC_FreeAntList (pnAntIdxArray);
	return hRet;
#else
	return ERR_FAIL;
#endif
}

HERROR MGR_ANTENNA_MakeMotorDegreeString (HINT32 nInputMode, HINT32 nDegree, HBOOL bPrintZero, HUINT8 *szString, HUINT32 ulBuffSize)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	HUINT8		 ucDirChar;
	HUINT32		 ulAbsDegree;
	HUINT32		 ulDegInt, ulDegDec;

	if (szString == NULL)					{ return ERR_FAIL; }

	ulAbsDegree = (nDegree >= 0) ? nDegree : (nDegree * -1);
	ulDegInt = ulAbsDegree / 10;
	ulDegDec = ulAbsDegree % 10;

	switch (nInputMode)
	{
	case DEGREE_TYPE_LONGITUDE:
		ucDirChar = (nDegree >= 0) ? 'E' : 'W';
		if (bPrintZero == TRUE)
		{
			snprintf (szString, ulBuffSize-1, "%03d.%d%c", ulDegInt, ulDegDec, ucDirChar);
		}
		else
		{
			snprintf (szString, ulBuffSize-1, "%d.%d%c", ulDegInt, ulDegDec, ucDirChar);
		}

		break;
	case DEGREE_TYPE_LATITUDE:
	default:
		ucDirChar = (nDegree >= 0) ? 'N' : 'S';
		if (bPrintZero == TRUE)
		{
			snprintf (szString, ulBuffSize-1, "%02d.%d%c", ulDegInt, ulDegDec, ucDirChar);
		}
		else
		{
			snprintf (szString, ulBuffSize-1, "%d.%d%c", ulDegInt, ulDegDec, ucDirChar);
		}

		break;
	}

	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}

HERROR MGR_ANTENNA_GetUserMotorLongitude (HINT32 *pnLongitude)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	HERROR		 hErr;

	if (pnLongitude == NULL)			{ return ERR_BUS_SOURCE_NULL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_MOTOR_USER_LONGITUDE, (HUINT32 *)pnLongitude, 0);
	if (hErr != ERR_OK)
	{
		*pnLongitude = 0;
	}

	return hErr;
#else
	return ERR_FAIL;
#endif
}

HERROR MGR_ANTENNA_SetUserMotorLongitude (HINT32 nLongitude)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	HERROR		 hErr;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_MOTOR_USER_LONGITUDE, (HUINT32)nLongitude, (HUINT32)0);
	return hErr;
#else
	return ERR_FAIL;
#endif
}


#define ___ANTENNA_NAMES_____

HERROR MGR_ANTENNA_GetSatelliteShortName (SVC_SatType_t eSatIdx, HUINT8 *szSatName, HUINT32 ulBuffLen)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	HUINT8	*szName;

	switch (eSatIdx)
	{
		case eSatType_AGILA_2:						szName = "AGILA2";						break;
		case eSatType_AMOS_1:						szName = "AMOS 2&3";					break;
		case eSatType_ANIK_F1:						szName = "ANIK F1";						break;
		case eSatType_APSTAR_1:						szName = "APSTAR 1";					break;
		case eSatType_APSTAR_1A:					szName = "APSTAR 1A";					break;
		case eSatType_APSTAR_2R:					szName = "APSTAR 2R";					break;
		case eSatType_ARABSAT_2A3A:					szName = "BADR 4&6";					break;
		case eSatType_ARABSAT_2B:					szName = "ARABSAT 2B";					break;
		case eSatType_ASIASAT_2:					szName = "ASIASAT 2";					break;
		case eSatType_ASIASAT_3S:					szName = "ASIASAT 3S";					break;
		case eSatType_ASTRA_1:						szName = "ASTRA 1";						break;
		case eSatType_ASTRA_2:						szName = "ASTRA 2";						break;
		case eSatType_ASTRA_3:						szName = "ASTRA 3A&3B";					break;
		case eSatType_ATLANTA_BIRD_1:				szName = "ATLANTIC BIRD 1";				break;
		case eSatType_BRASILSAT_B1:					szName = "BrasilSat B1";				break;
		case eSatType_BRASILSAT_B2:					szName = "BrasilSat B2";				break;
		case eSatType_BRASILSAT_B3:					szName = "BrasilSat B3";				break;
		case eSatType_BRASILSAT_B4:					szName = "BrasilSat B4";				break;
		case eSatType_EURASIASAT_1:					szName = "EurasiaSat 1";				break;
		case eSatType_EUTELSAT_II_F2:				szName = "EutelSat II";					break;
		case eSatType_EUTELSAT_W1:					szName = "Eutelsat W1";					break;
		case eSatType_EUTELSAT_W2:					szName = "Eutelsat W2";					break;
		case eSatType_EUTELSAT_W3:					szName = "Eutelsat W3A";				break;
		case eSatType_EXPRESS_3A:					szName = "Express 3A";					break;
		case eSatType_EXPRESS_6A:					szName = "Express 6A";					break;
		case eSatType_GORIZONT_26:					szName = "Gorizont 26";					break;
		case eSatType_GORIZONT_33:					szName = "Gorizont 33";					break;
		case eSatType_HISPASAT:						szName = "HISPASAT 1C&1D";				break;
		case eSatType_HOTBIRD:						szName = "HOTBIRD";						break;
		case eSatType_INSAT_2DT:					szName = "Insat 2D";					break;
		case eSatType_INSAT_2E3B:					szName = "Insat 2E-3B";					break;
		case eSatType_INSAT_3C:						szName = "Insat 3C";					break;
		case eSatType_INTELSAT_605:					szName = "IntelSat 605";				break;
		case eSatType_INTELSAT_704:					szName = "IntelSat 704";				break;
		case eSatType_INTELSAT_706:					szName = "IntelSat 706";				break;
		case eSatType_INTELSAT_707:					szName = "IntelSat 707";				break;
		case eSatType_INTELSAT_709:					szName = "IntelSat 709";				break;
		case eSatType_INTELSAT_805:					szName = "IntelSat 805";				break;
		case eSatType_INTELSAT_901:					szName = "IntelSat 901";				break;
		case eSatType_INTELSAT_902:					szName = "IntelSat 902";				break;
		case eSatType_INTELSAT_904:					szName = "IntelSat 904";				break;
		case eSatType_INTELSAT_906:					szName = "IntelSat 906";				break;
		case eSatType_JCSAT_3:						szName = "JCSAT 3";						break;
		case eSatType_KOREASAT_3:					szName = "KoreaSat 3";					break;
		case eSatType_LMI_1:						szName = "LMI 1";						break;
		case eSatType_MEASAT_1:						szName = "Measat 1";					break;
		case eSatType_MEASAT_2:						szName = "Measat 2";					break;
		case eSatType_NAHUEL_1:						szName = "Nahuel 1";					break;
		case eSatType_NILESAT:						szName = "NILESAT 101&102";				break;
		case eSatType_NSS_703:						szName = "NSS 703";						break;
		case eSatType_NSS_803:						szName = "NSS 803";						break;
		case eSatType_NSS_806:						szName = "NSS 806";						break;
		case eSatType_OPTUS_B1:						szName = "Optus B1";					break;
		case eSatType_OPTUS_B3:						szName = "Optus B3";					break;
		case eSatType_PALAPA_C2:					szName = "Palapa C2";					break;
		case eSatType_PAS_1R:						szName = "PAS 1R";						break;
		case eSatType_PAS_2:						szName = "PAS 2";						break;
		case eSatType_PAS_3R_6_6B:					szName = "PAS 3R-6-6B";					break;
		case eSatType_PAS_7_10:						szName = "PAS 7-10";					break;
		case eSatType_PAS_8:						szName = "PAS 8";						break;
		case eSatType_PAS_9:						szName = "PAS 9";						break;
		case eSatType_SATMEX_5:						szName = "SatMex 5";					break;
		case eSatType_SINOSAT_1:					szName = "SINOSAT 1";					break;
		case eSatType_SIRIUS:						szName = "SIRIUS";						break;
		case eSatType_ST_1:							szName = "ST 1";						break;
		case eSatType_TELECOM_2A:					szName = "TELECOM 2A";					break;
		case eSatType_TELECOM_2C:					szName = "TELECOM 2C";					break;
		case eSatType_TELECOM_2D:					szName = "TELECOM 2D";					break;
		case eSatType_TELKOM_1:						szName = "Telcom1";						break;
		case eSatType_TELSTAR_12:					szName = "TELSTAR 12";					break;
		case eSatType_THAICOM_1A:					szName = "Thaicom 1A";					break;
		case eSatType_THAICOM_2_3:					szName = "Thaicom 2-3";					break;
		case eSatType_THOR:							szName = "THOR";						break;
		case eSatType_TURKSAT_1B:					szName = "Turksat 1B";					break;
		case eSatType_TURKSAT_1C:					szName = "Turksat 1C";					break;
		case eSatType_YAMAL_102:					szName = "Yamal";						break;
		case eSatType_AMC_1:						szName = "AMC1";						break;
		case eSatType_AMC_3:						szName = "AMC3";						break;
		case eSatType_AMC_4:						szName = "AMC4";						break;
		case eSatType_AMC_7:						szName = "AMC7";						break;
		case eSatType_ANIK_E2:						szName = "ANIK E2";						break;
		case eSatType_GALAXY_10R:					szName = "Galaxy 10R";					break;
		case eSatType_GALAXY_11:					szName = "Galaxy 11";					break;
		case eSatType_GALAXY_1R:					szName = "Galaxy 1R";					break;
		case eSatType_GALAXY_3C:					szName = "Galaxy 3C";					break;
		case eSatType_GALAXY_4R:					szName = "Galaxy 4R";					break;
		case eSatType_GALAXY_9:						szName = "Galaxy 9";					break;
		case eSatType_SATCOM_C3:					szName = "SatCom C3";					break;
		case eSatType_SATCOM_C4:					szName = "SatCom C4";					break;
		case eSatType_SOLIDARIDAD_2:				szName = "Solidaridad 2";				break;
		case eSatType_TELSTAR_4:					szName = "TelStar 4";					break;
		case eSatType_TELSTAR_5:					szName = "TelStar 5";					break;
		case eSatType_TELSTAR_7:					szName = "TelStar 7";					break;
		case eSatType_EUROBIRD_1:					szName = "EUROBIRD 1";					break;
		case eSatType_EUROBIRD_3:					szName = "EUROBIRD 3";					break;
		case eSatType_EUROBIRD_9:					szName = "EUROBIRD 9";					break;
		case eSatType_EUTELSAT_W4:					szName = "EUTELSAT SESAT&W4";			break;
		case eSatType_EUTELSAT_W6:					szName = "EUTELSAT W6";					break;
		case eSatType_EXPRESS_AM1:					szName = "EXPRESS AM1";					break;
		case eSatType_EXPRESS_AM22:					szName = "EXPRESS AM22";				break;
		case eSatType_HELLAS_SAT2:					szName = "HELLAS SAT 2";				break;
		case eSatType_INTELSAT_10_02:				szName = "INTELSAT 10-02";				break;
		case eSatType_INTELSAT_12:					szName = "INTELSAT 12";					break;
		case eSatType_INTELSAT_7_10:				szName = "INTELSAT 7&10";				break;
		case eSatType_INTELSAT_802:					szName = "INTELSAT 802";				break;
		case eSatType_INTELSAT_907:					szName = "INTELSAT 907";				break;
		case eSatType_NSS_7:						szName = "NSS 7";						break;
		case eSatType_ATLANTA_BIRD_2:				szName = "ATLANTIC BIRD 2";				break;
		case eSatType_ATLANTA_BIRD_3:				szName = "ATLANTIC BIRD 3";				break;
		case eSatType_THOR_3_5: 					szName = "THOR 3&5";					break;
		case eSatType_TURKSAT_2A:					szName = "TURKSAT 2A&3A";				break;
		case eSatType_NILESAT_101_102:				szName = "NILESAT 101&102";				break;
		case eSatType_BADR_4_6:						szName = "BADR 4&6";					break;
		case eSatType_YAHLIVE:						szName = "YAHLIVE";						break;
		case eSatType_HISPASAT_1D:					szName = "Hispasat 1D";					break;
		case eSatType_HISPASAT_1E:					szName = "Hispasat 1E";					break;
		case eSatType_SES_2:						szName = "SES 2";						break;
		case eSatType_SES_3:						szName = "SES 3";						break;
		case eSatType_SES_6:						szName = "SES 6";						break;
		case eSatType_EUTELSAT_5_WEST_A:			szName = "EUTELSAT 5 West A";			break;
		case eSatType_EUTELSAT_12_WEST_B:			szName = "EUTELSAT 12 West B";			break;
		case eSatType_EUTELSAT_113_WEST_A:			szName = "EUTELSAT 113 West A";			break;
		case eSatType_EUTELSAT_117_WEST_A:			szName = "EUTELSAT 117 West A";			break;
		case eSatType_EUTELSAT_HOTBIRD_13B:			szName = "EUTELSAT HOTBIRD 13B";		break;
		case eSatType_EUTELSAT_HOTBIRD_13C:			szName = "EUTELSAT HOTBIRD 13C";		break;
		case eSatType_EUTELSAT_HOTBIRD_13D:			szName = "EUTELSAT HOTBIRD 13D";		break;
		case eSatType_INTELSAT_1R:					szName = "INTELSAT 1R";					break;
		case eSatType_INTELSAT_11:					szName = "INTELSAT 11";					break;
		case eSatType_INTELSAT_14:					szName = "INTELSAT 14";					break;
		case eSatType_INTELSAT_21:					szName = "INTELSAT 21";					break;
		case eSatType_INTELSAT_23:					szName = "INTELSAT 23";					break;
		case eSatType_INTELSAT_34:					szName = "INTELSAT 34";					break;
		case eSatType_INTELSAT_903:					szName = "INTELSAT 903";				break;
		case eSatType_GALAXY_18:					szName = "Galaxy 18";					break;
		case eSatType_GALAXY_19:					szName = "Galaxy 19";					break;
		case eSatType_GALAXY_28:					szName = "Galaxy 28";					break;
		case eSatType_SPACEWAY_1_AND_DIRECTV_10_02:	szName = "Spaceway 1 & DirecTV 10/12";	break;
		case eSatType_AMAZONAS_2:					szName = "AMAZONAS 2";					break;
		case eSatType_AMAZONAS_3:					szName = "AMAZONAS 3";					break;
		case eSatType_AMC_6:						szName = "AMC 6";						break;
		case eSatType_AMC_8:						szName = "AMC 8";						break;
		case eSatType_AMC_15:						szName = "AMC 15";						break;
		case eSatType_AMC_18:						szName = "AMC 18";						break;
		case eSatType_AMC_21:						szName = "AMC 21";						break;
		case eSatType_BSAT_3C_JCSAT_110R:			szName = "BSAT 3C JCSAT 110R";			break;
		case eSatType_N_SAT_110:					szName = "N SAT 110";					break;
		case eSatType_TELSTAR_12_VANTAGE:			szName = "TELSTAR 12 VANTAGE";			break;
		case eSatType_TELSTAR_14R:					szName = "TELSTAR 14R";					break;
		case eSatType_ST_2:							szName = "ST 2";						break;
		case eSatType_SIMON_BOLIVAR:				szName = "Simon Bolivar";				break;
		case eSatType_STAR_ONE_C1:					szName = "Star One C1";					break;
		case eSatType_STAR_ONE_C2:					szName = "Star One C2";					break;
		case eSatType_STAR_ONE_C4:					szName = "Star One C4";					break;
		case eSatType_NSS_10:						szName = "NSS 10";						break;

		default:
			if (MGR_ANTENNA_IsSatTypeUserAntennaType (eSatIdx) == TRUE)
			{
				szName = "User Define";
				break;
			}

			HxLOG_Error("unknown sat idx(%d) ", eSatIdx);
			return ERR_FAIL;
	}

	HLIB_STD_StrUtf8NCpy (szSatName, szName, ulBuffLen);

	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}

HERROR MGR_ANTENNA_GetSatellitePositionString (SVC_SatType_t eSatIdx, HUINT8 *szSatPosStr, HUINT32 ulBuffLen)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	HINT32		 sDegreeInt, sDegreeDec;
	HUINT8		 ucDirection;
	HINT16		 sSatDegree;
	HERROR		 hErr;

	if (szSatPosStr == NULL)				{ return ERR_FAIL; }

	hErr = SVC_CH_GetSatDegree ((HINT32)eSatIdx, &sSatDegree);
	if (hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	if (sSatDegree >= 0)
	{
		ucDirection = 'E';
	}
	else
	{
		ucDirection = 'W';
		sSatDegree *= -1;
	}

	sDegreeInt = sSatDegree / 10;
	sDegreeDec = sSatDegree % 10;

	snprintf (szSatPosStr, ulBuffLen-1, "%d.%d%c", sDegreeInt, sDegreeDec, ucDirection);
	return ERR_OK;
#else
	snprintf (szSatPosStr, ulBuffLen-1, "%d.%d%c", 0, 0, 0);
	return ERR_OK;
#endif
}

HERROR MGR_ANTENNA_GetSatelliteName (SVC_SatType_t eSatIdx, HUINT8 *szSatFullName, HUINT32 ulBuffLen)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	HUINT8		 szSatName[MAX_ANT_NAME], szPosStr[24], szTempFullName[MAX_ANT_NAME + 28];
	HERROR		 hErr;

	if (szSatFullName == NULL)		{ return ERR_FAIL; }

	HxSTD_MemSet(szSatName, 0x00, sizeof(szSatName));
	HxSTD_MemSet(szPosStr, 0x00, sizeof(szPosStr));
	HxSTD_MemSet(szTempFullName, 0x00, sizeof(szTempFullName));

	hErr = MGR_ANTENNA_GetSatelliteShortName (eSatIdx, szSatName, MAX_ANT_NAME);
	if (hErr != ERR_OK)			{ return hErr; }

	hErr = MGR_ANTENNA_GetSatellitePositionString (eSatIdx, szPosStr, 24);
	if (hErr == ERR_OK)
	{
		snprintf (szTempFullName, MAX_ANT_NAME + 28 -1, "%s (%s)", szSatName, szPosStr);
		HLIB_STD_StrUtf8NCpy(szSatFullName, szTempFullName, ulBuffLen);
	}
	else
	{
		snprintf (szSatFullName, ulBuffLen-1, "%s", szSatName);
	}

	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}

/* end of file */

