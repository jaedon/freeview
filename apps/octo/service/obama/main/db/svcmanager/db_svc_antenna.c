/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  db_svc_antenna.c
	@brief

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다. 			\n
	Remarks :										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "_db_svc_common.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Private Functions    ********************/
/*******************************************************************/

STATIC dbSvc_Antenna_t	*db_svc_AntennaCtor (dbSvc_Antenna_t *pstSelf, const DbSvc_AntInfo_t *pstData)
{
	if (pstData)
		HxSTD_MemCopy(&pstSelf->stData, pstData, sizeof(DbSvc_AntInfo_t));
	else
		(void) db_svc_AntennaInitData(&pstSelf->stData);
	return pstSelf;
}

STATIC void			db_svc_AntennaDtor (dbSvc_Antenna_t *pstSelf)
{
	// do nothing...
	(void)pstSelf;
}

STATIC HINT32		db_svc_AntennaGetKey (const dbSvc_Antenna_t *pstSelf)
{
	HxLOG_Trace();
	if (pstSelf)
	{
		return pstSelf->nKey;
	}
	return 0;
}

STATIC void			db_svc_AntennaSetKey (dbSvc_Antenna_t *pstSelf, HINT32 nKey)
{
	HxLOG_Trace();
	if (pstSelf)
	{
		pstSelf->nKey = nKey;
	}
}

STATIC HINT32		db_svc_AntennaToString (const dbSvc_Antenna_t *pstSelf, FILE *pfStream)
{
	HINT32	ret;

	ret = fprintf(pfStream
			, "Antenna: {\n"
			  "             key: %d\n"
			  "         sattype: %d\n"
			  "            name: %s\n"
			, pstSelf->nKey
			, pstSelf->stData.eSatType
			, pstSelf->stData.szName
		);

	switch (pstSelf->stData.eAntennaType)
	{
	case eDxANT_TYPE_LNB_ONLY:
		ret += fprintf(pfStream, "    antenna type: LNB Only\n");
		ret += fprintf(pfStream,
				"        lnb freq: %d\n"
				"     lnb voltage: %d\n"
				, pstSelf->stData.utAntInfo.stLnbAnt.ulLnbFreq
				, pstSelf->stData.utAntInfo.stLnbAnt.eLnbVoltage
			);
		break;

	case eDxANT_TYPE_DISEQC:
		ret += fprintf(pfStream, "    antenna type: DiSEqC\n");
		ret += fprintf(pfStream,
				"        lnb freq: %d\n"
				"     lnb voltage: %d\n"
				"        22k Tone: %d\n"
				"      diseqc ver: %d\n"
				"    diseqc input: %d\n"
				"      tone burst: %d\n"
				"  motor position: %d\n"
				"      longgitude: %d\n"
				, pstSelf->stData.utAntInfo.stDiseqcAnt.ulLnbFreq
				, pstSelf->stData.utAntInfo.stDiseqcAnt.eLnbVoltage
				, pstSelf->stData.utAntInfo.stDiseqcAnt.b22kTone
				, pstSelf->stData.utAntInfo.stDiseqcAnt.eDiseqcVer
				, pstSelf->stData.utAntInfo.stDiseqcAnt.eDiseqcInput
				, pstSelf->stData.utAntInfo.stDiseqcAnt.eToneBurst
				, pstSelf->stData.utAntInfo.stDiseqcAnt.ucMotorPosition
				, pstSelf->stData.utAntInfo.stDiseqcAnt.sSatLonggitude
			);
		break;

	case eDxANT_TYPE_SCD:
		ret += fprintf(pfStream, "    antenna type: SCD\n");
		ret += fprintf(pfStream,
				"        lnb freq: %d\n"
				"     lnb voltage: %d\n"
				"        22k Tone: %d\n"
				"       scd input: %d\n"
				"   user band num: %d\n"
				"  user band freq: %d\n"
				"  user band num1: %d\n"
				" user band freq1: %d\n"
				, pstSelf->stData.utAntInfo.stScdAnt.ulLnbFreq
				, pstSelf->stData.utAntInfo.stScdAnt.eLnbVoltage
				, pstSelf->stData.utAntInfo.stScdAnt.b22kTone
				, pstSelf->stData.utAntInfo.stScdAnt.eScdInput
				, pstSelf->stData.utAntInfo.stScdAnt.ulScdUserBandNum
				, pstSelf->stData.utAntInfo.stScdAnt.ulScdUserBandFreq
				, pstSelf->stData.utAntInfo.stScdAnt.ulScdUserBandNum1
				, pstSelf->stData.utAntInfo.stScdAnt.ulScdUserBandFreq1
			);
		break;

	case eDxANT_TYPE_SMATV:
		ret += fprintf(pfStream, "    antenna type: SMATV\n");
		break;

	default:
		break;
	}
	ret += fprintf(pfStream, "}\n");
	return ret;
}

STATIC dbSvc_TableClass_t *	db_svc_AntennaClass (void)
{
	STATIC dbSvc_TableClass_t	s_stClass = {
		sizeof(dbSvc_Antenna_t)
		, sizeof(DbSvc_AntInfo_t)
		, "OxAntenna"
		, (dbSvc_Table_t *(*)(dbSvc_Table_t *, void *))db_svc_AntennaCtor
		, (             void (*) (dbSvc_Table_t *))db_svc_AntennaDtor
		, (      HINT32 (*)(const dbSvc_Table_t *))db_svc_AntennaGetKey
		, (      void (*)(dbSvc_Table_t *, HINT32))db_svc_AntennaSetKey
		, NULL
		, (HINT32 (*) (const dbSvc_Table_t *, FILE *))db_svc_AntennaToString
	};
	return &s_stClass;
}


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

DbSvc_AntInfo_t *	db_svc_AntennaInitData (DbSvc_AntInfo_t *pstData)
{
	HxLOG_Trace();
	HxLOG_Assert(pstData);

	HxSTD_MemSet (pstData, 0, sizeof(DbSvc_AntInfo_t));

	return ERR_OK;
}


dbSvc_Antenna_t *	db_svc_AntennaInit (dbSvc_Antenna_t *pstSelf, const DbSvc_AntInfo_t *pstData)
{
	HxLOG_Trace();

	return (dbSvc_Antenna_t *)db_svc_TableInit(db_svc_AntennaClass(), (dbSvc_Table_t *)pstSelf, (void *)pstData);
}

dbSvc_Antenna_t *	db_svc_AntennaNew (const DbSvc_AntInfo_t *pstData)
{
	HxLOG_Trace();

	return (dbSvc_Antenna_t *)db_svc_TableNew(db_svc_AntennaClass(), (void *)pstData);
}


dbSvc_Antenna_t *	db_svc_AntennaDelete (dbSvc_Antenna_t *pstData)
{
	HxLOG_Trace();

	return (dbSvc_Antenna_t *)db_svc_TableDecref((dbSvc_Table_t *)pstData);
}

