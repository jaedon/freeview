/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xdb_svc_dama_adapter_base.c
	@brief

	Description: main main procedure 관련 내용들을 분류한 모듈이다. \n
	Module:
	Remarks : 										\n
 #include <mwc_util.h>

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <dapi.h>
#include <dlib.h>
#include <octo_common.h>

#include <db_svc.h>
#include <_db_svc_service.h>
#include <db_param.h>
#include <pdb_svc.h>

//#include <mwc_util.h>
//#include <svc_fs.h>

#include "_db_main.h"
#include "_xdb_svc.h"

#include <otl.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

#define	RETURN_IF_VOID(expr)	do{if(expr){HxLOG_Debug("%s failed!!\n", #expr);return;}}while(0)
#define	RETURN_IF(expr, err)	do{if(expr){HxLOG_Debug("%s failed!!\n", #expr);return (err);}}while(0)
//#define	GOTO_IF(expr, pos)		do{if(expr){HxLOG_Debug("%s failed!!\n", #expr);goto pos;}}while(0)

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct
{
	HUINT32		ulLength;
	HUINT8		aucData[4];
} xdbSvc_DmAdapterArray_t;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

STATIC const HINT32	s_nLnbVoltage[][2] =
{
  	{eDxLNB_VOLT_STD, eDxLNB_VOLT_STD},
 	{eDxLNB_VOLT_HIGH, eDxLNB_VOLT_HIGH}
};

STATIC const HINT32	s_nDiseqcVer[][2] =
{
	{eDxDISEQC_VER_1_0, eDxDISEQC_VER_1_0},
	{eDxDISEQC_VER_1_1, eDxDISEQC_VER_1_1},
	{eDxDISEQC_VER_1_2, eDxDISEQC_VER_1_2},
	{eDxDISEQC_VER_1_3, eDxDISEQC_VER_1_3},
	{eDxDISEQC_VER_2_0, eDxDISEQC_VER_2_0}
};

STATIC const HINT32	s_nDiseqcInput[][2] =
{
	{eDxDISEQC_INPUT_NONE, eDxDISEQC_INPUT_NONE},
	{eDxDISEQC_INPUT_A, eDxDISEQC_INPUT_A},
	{eDxDISEQC_INPUT_B, eDxDISEQC_INPUT_B},
	{eDxDISEQC_INPUT_C, eDxDISEQC_INPUT_C},
	{eDxDISEQC_INPUT_D, eDxDISEQC_INPUT_D}
};

STATIC const HINT32	s_nScdInput[][2] = {
	{eDxSCD_INPUT_NONE, eDxSCD_INPUT_NONE},
	{eDxSCD_INPUT_SCD_A, eDxSCD_INPUT_SCD_A},
	{eDxSCD_INPUT_SCD_B, eDxSCD_INPUT_SCD_B}
};

STATIC const HINT32	s_nToneBurst[][2] =
{
	{eDxTONE_BURST_NONE, eDxTONE_BURST_NONE},
	{eDxTONE_BURST_A, eDxTONE_BURST_A},
	{eDxTONE_BURST_B, eDxTONE_BURST_B}
};

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

STATIC DxAntInfo_t *		xdb_svc_DamaAdaptorAntenna2DxType (DxAntInfo_t *pstDest, const DbSvc_AntInfo_t *pstSource, HUINT16 usId)
{
	const HCHAR *pszType;

	HxSTD_MemSet(pstDest, 0, sizeof(DxAntInfo_t));

	pszType = OTL_CONV_SVC_SatType2String(pstSource->eSatType);
	HLIB_STD_StrUtf8NCpy(pstDest->satType, pszType ? pszType : "NONE", DxANTINFO_NAME_LEN);
	HLIB_STD_StrUtf8NCpy(pstDest->satName, pstSource->szName, DxANTINFO_NAME_LEN);

	//switch (pstDest->antType)
	switch (pstSource->eAntennaType)
	{
	case eDxANT_TYPE_LNB_ONLY:
	{
		const DbSvc_LnbAntInfo_t *pstSourceInfo = &(pstSource->utAntInfo.stLnbAnt);
		DxLnbAntenna_t *pstDestInfo = &(pstDest->info.lnb);

		pstDestInfo->ulLnbFreq  = pstSourceInfo->ulLnbFreq;
		pstDestInfo->lnbVoltage = HLIB_STD_Map(1, pstSourceInfo->eLnbVoltage, s_nLnbVoltage, sizeof(s_nLnbVoltage) / sizeof(s_nLnbVoltage[0]), eDxLNB_VOLT_STD);
		pstDest->antType = eDxANT_TYPE_LNB_ONLY;
		break;
	}

	case eDxANT_TYPE_DISEQC:
	{
		const DbSvc_DiseqcAntInfo_t		*pstSourceInfo = &(pstSource->utAntInfo.stDiseqcAnt);
		DxDiseqcAntenna_t				*pstDestInfo = &(pstDest->info.diseqc);

		pstDestInfo->ulLnbFreq			= pstSourceInfo->ulLnbFreq;
		pstDestInfo->lnbVoltage			= HLIB_STD_Map(1, pstSourceInfo->eLnbVoltage, s_nLnbVoltage, sizeof(s_nLnbVoltage) / sizeof(s_nLnbVoltage[0]), eDxLNB_VOLT_STD);
		pstDestInfo->b22kTone			= pstSourceInfo->b22kTone;
		pstDestInfo->diseqcVersion		= HLIB_STD_Map(1, pstSourceInfo->eDiseqcVer, s_nDiseqcVer, sizeof(s_nDiseqcVer) / sizeof(s_nDiseqcVer[0]), eDxDISEQC_VER_1_0);
		pstDestInfo->diseqcInput		= HLIB_STD_Map(1, pstSourceInfo->eDiseqcInput, s_nDiseqcInput, sizeof(s_nDiseqcInput) / sizeof(s_nDiseqcInput[0]), eDxDISEQC_INPUT_NONE);
		pstDestInfo->toneBurst			= HLIB_STD_Map(1, pstSourceInfo->eToneBurst, s_nToneBurst, sizeof(s_nToneBurst) / sizeof(s_nToneBurst[0]), eDxTONE_BURST_NONE);
		pstDestInfo->ucMotorPosition	= pstSourceInfo->ucMotorPosition;
		pstDestInfo->sSatLonggitude		= pstSourceInfo->sSatLonggitude;

		pstDest->antType = eDxANT_TYPE_DISEQC;
		break;
	}

	case eDxANT_TYPE_SCD:
	{
		const DbSvc_ScdAntInfo_t	*pstSourceInfo = &(pstSource->utAntInfo.stScdAnt);
		DxScdAntenna_t				*pstDestInfo = &(pstDest->info.scd);

		pstDestInfo->ulLnbFreq				= pstSourceInfo->ulLnbFreq;
		pstDestInfo->lnbVoltage				= HLIB_STD_Map(1, pstSourceInfo->eLnbVoltage, s_nLnbVoltage, sizeof(s_nLnbVoltage) / sizeof(s_nLnbVoltage[0]), eDxLNB_VOLT_STD);
		pstDestInfo->b22kTone				= pstSourceInfo->b22kTone;
		pstDestInfo->scdInput				= HLIB_STD_Map(1, pstSourceInfo->eScdInput, s_nScdInput, sizeof(s_nScdInput) / sizeof(s_nScdInput[0]), eDxSCD_INPUT_NONE);
		pstDestInfo->ulScdUserBandNum		= pstSourceInfo->ulScdUserBandNum;
		pstDestInfo->ulScdUserBandFreq		= pstSourceInfo->ulScdUserBandFreq;
		pstDestInfo->ulScdUserBandNum1		= pstSourceInfo->ulScdUserBandNum1;
		pstDestInfo->ulScdUserBandFreq1		= pstSourceInfo->ulScdUserBandFreq1;
		pstDestInfo->ulScdUserBandNum2		= pstSourceInfo->ulScdUserBandNum2; // MOON_SCD
		pstDestInfo->ulScdUserBandFreq2		= pstSourceInfo->ulScdUserBandFreq2;
		pstDestInfo->ulScdUserBandNum3		= pstSourceInfo->ulScdUserBandNum3;
		pstDestInfo->ulScdUserBandFreq3		= pstSourceInfo->ulScdUserBandFreq3;

		pstDest->antType = eDxANT_TYPE_SCD;
		break;
	}

	case eDxANT_TYPE_SMATV:
	case eDxANT_TYPE_MONOBLOCK_LNB:
	default:
		HxLOG_Warning("[%d:%s] antenna type not implemented yet!..\n", __FUNCTION__, __LINE__);
		return NULL;
	}
	pstDest->uid = (HINT32)usId;
	return pstDest;
}



STATIC DbSvc_AntInfo_t *		xdb_svc_DamaAdaptorAntenna2ObamaType (DbSvc_AntInfo_t *pstDest, HUINT16 *pusId, const DxAntInfo_t *pstSource)
{
	HxSTD_MemSet(pstDest, 0, sizeof(DbSvc_AntInfo_t));

	pstDest->eSatType = (SVC_SatType_t)OTL_CONV_SatString2SVC_SatType(pstSource->satType);
	HLIB_STD_StrUtf8NCpy(pstDest->szName, pstSource->satName, MAX_ANT_NAME);

	switch (pstSource->antType)
	{
	case eDxANT_TYPE_LNB_ONLY:
	{
		DbSvc_LnbAntInfo_t		*pstDestInfo = &(pstDest->utAntInfo.stLnbAnt);
		const DxLnbAntenna_t	*pstSourceInfo = &(pstSource->info.lnb);

		pstDestInfo->ulLnbFreq		= pstSourceInfo->ulLnbFreq;
		pstDestInfo->eLnbVoltage	= HLIB_STD_Map(0, pstSourceInfo->lnbVoltage, s_nLnbVoltage, sizeof(s_nLnbVoltage) / sizeof(s_nLnbVoltage[0]), eDxLNB_VOLT_STD);
		pstDest->eAntennaType		= eDxANT_TYPE_LNB_ONLY;
		break;
	}

	case eDxANT_TYPE_DISEQC:
	{
		DbSvc_DiseqcAntInfo_t		*pstDestInfo = &(pstDest->utAntInfo.stDiseqcAnt);
		const DxDiseqcAntenna_t		*pstSourceInfo = &(pstSource->info.diseqc);

		pstDestInfo->ulLnbFreq			= pstSourceInfo->ulLnbFreq;
		pstDestInfo->eLnbVoltage		= HLIB_STD_Map(0, pstSourceInfo->lnbVoltage, s_nLnbVoltage, sizeof(s_nLnbVoltage) / sizeof(s_nLnbVoltage[0]), eDxLNB_VOLT_STD);
		pstDestInfo->b22kTone			= pstSourceInfo->b22kTone;
		pstDestInfo->eDiseqcVer			= HLIB_STD_Map(0, pstSourceInfo->diseqcVersion, s_nDiseqcVer, sizeof(s_nDiseqcVer) / sizeof(s_nDiseqcVer[0]), eDxDISEQC_VER_1_0);
		pstDestInfo->eDiseqcInput		= HLIB_STD_Map(0, pstSourceInfo->diseqcInput, s_nDiseqcInput, sizeof(s_nDiseqcInput) / sizeof(s_nDiseqcInput[0]), eDxDISEQC_INPUT_NONE);
		pstDestInfo->eToneBurst			= HLIB_STD_Map(0, pstSourceInfo->toneBurst, s_nToneBurst, sizeof(s_nToneBurst) / sizeof(s_nToneBurst[0]), eDxTONE_BURST_NONE);
		pstDestInfo->ucMotorPosition	= pstSourceInfo->ucMotorPosition;
		pstDestInfo->sSatLonggitude		= pstSourceInfo->sSatLonggitude;

		pstDest->eAntennaType = eDxANT_TYPE_DISEQC;
		break;
	}

	case eDxANT_TYPE_SCD:
	{
		DbSvc_ScdAntInfo_t *pstDestInfo = &(pstDest->utAntInfo.stScdAnt);
		const DxScdAntenna_t *pstSourceInfo = &(pstSource->info.scd);

		pstDestInfo->ulLnbFreq   = pstSourceInfo->ulLnbFreq;
		pstDestInfo->eLnbVoltage = HLIB_STD_Map(0, pstSourceInfo->lnbVoltage, s_nLnbVoltage, sizeof(s_nLnbVoltage) / sizeof(s_nLnbVoltage[0]), eDxLNB_VOLT_STD);
		pstDestInfo->b22kTone    = pstSourceInfo->b22kTone;
		pstDestInfo->eScdInput   = HLIB_STD_Map(0, pstSourceInfo->scdInput, s_nScdInput, sizeof(s_nScdInput) / sizeof(s_nScdInput[0]), eDxSCD_INPUT_NONE);
		pstDestInfo->ulScdUserBandNum   = pstSourceInfo->ulScdUserBandNum;
		pstDestInfo->ulScdUserBandFreq  = pstSourceInfo->ulScdUserBandFreq;
		pstDestInfo->ulScdUserBandNum1  = pstSourceInfo->ulScdUserBandNum1;
		pstDestInfo->ulScdUserBandFreq1 = pstSourceInfo->ulScdUserBandFreq1;
		pstDestInfo->ulScdUserBandNum2  = pstSourceInfo->ulScdUserBandNum2; // MOON_SCD
		pstDestInfo->ulScdUserBandFreq2 = pstSourceInfo->ulScdUserBandFreq2;
		pstDestInfo->ulScdUserBandNum3  = pstSourceInfo->ulScdUserBandNum3;
		pstDestInfo->ulScdUserBandFreq3 = pstSourceInfo->ulScdUserBandFreq3;

		pstDest->eAntennaType = eDxANT_TYPE_SCD;
		break;
	}

	case eDxANT_TYPE_SMATV:
	case eDxANT_TYPE_MONOBLOCK_LNB:
	default:
		HxLOG_Warning("antenna type not implemented yet!..\n");
		return NULL;
	}
	*pusId = (HUINT16)pstSource->uid;
	return pstDest;
}



STATIC void		xdb_svc_DamaAdaptorDataGetter (const HCHAR *pszName, HUID hUid, const void *pvData, HUINT32 ulSize, void *pvUser)
{
	DAxIterator_t				 stIter;
	HUINT32						 ulLength;
	xdbSvc_DmAdapterArray_t		*pstArray;

	RETURN_IF_VOID(ulSize == 0 || pvData == NULL || pvUser == NULL);

	ulLength = 0;
	DAPI_InitIterator(&stIter, pvData, ulSize, 0);
	while (DAPI_NextIterator(&stIter))
		ulLength++;

#define	DGETTER(dtype,otype,converter)	\
	do {\
		pstArray = (xdbSvc_DmAdapterArray_t *)HLIB_STD_MemCalloc(sizeof(xdbSvc_DmAdapterArray_t) + sizeof(otype) * ulLength);\
		if (pstArray) {\
			otype *info = (otype *)pstArray->aucData;\
			while (DAPI_NextIterator(&stIter)){\
				HxLOG_Assert(stIter.size == sizeof(dtype));\
				if (converter(&info[pstArray->ulLength], (const dtype *)stIter.data))\
					pstArray->ulLength++;\
			}\
			if (pstArray->ulLength == 0) {\
				HLIB_STD_MemFree(pstArray);\
				pstArray = NULL;\
			}\
		}\
	} while (0)

	pstArray = NULL;
	DAPI_InitIterator(&stIter, pvData, ulSize, 0);

	if (HxSTD_StrNCmp(pszName, DxNAMEOF(DxService_t), HxSTD_StrLen(DxNAMEOF(DxService_t))) == 0)
	{
		DGETTER(DxService_t, DbSvc_Info_t, OTL_CONV_DxService2SVC_Info);
	}
	else if (HxSTD_StrNCmp(pszName, DxNAMEOF(DxTransponder_t), HxSTD_StrLen(DxNAMEOF(DxTransponder_t))) == 0)
	{
		DGETTER(DxTransponder_t, DbSvc_TsInfo_t, OTL_CONV_DxTransponder2SVC_TsInfo);
	}
	else if (HxSTD_StrNCmp(pszName, DxNAMEOF(DxNetwork_t), HxSTD_StrLen(DxNAMEOF(DxNetwork_t))) == 0)
	{
		DGETTER(DxNetwork_t, DbSvc_NetInfo_t, OTL_CONV_DxNetwork2SVC_NetInfo);
	}
	else if (HxSTD_StrNCmp(pszName, DxNAMEOF(DxProvider_t), HxSTD_StrLen(DxNAMEOF(DxProvider_t))) == 0)
	{
		DGETTER(DxProvider_t, DbSvc_PrvInfo_t, OTL_CONV_DxProvider2SVC_PrvInfo);
	}
	else if (HxSTD_StrNCmp(pszName, DxNAMEOF(DxBouquet_t), HxSTD_StrLen(DxNAMEOF(DxBouquet_t))) == 0)
	{
		DGETTER(DxBouquet_t, DbSvc_BouqInfo_t, OTL_CONV_DxBouquet2SVC_BouqInfo);
	}
	else if (HxSTD_StrNCmp(pszName, DxNAMEOF(DxGroup_t), HxSTD_StrLen(DxNAMEOF(DxGroup_t))) == 0)
	{
		DGETTER(DxGroup_t, DbSvc_GroupInfo_t, OTL_CONV_DxGroup2SVC_GroupInfo);
	}
	else
	{
		HxLOG_Critical("%s() WTF! pszName:%s\n", __FUNCTION__, pszName);
	}
#undef	DGETTER

	*(xdbSvc_DmAdapterArray_t **)pvUser = pstArray;
}


STATIC void		xdb_svc_DamaAdaptorAntennaGetter (const HCHAR *pszName, HUID hUid, const void *pvData, HUINT32 ulSize, void *pvUser)
{
	HUINT32  ulCnt = 0;
	HUINT16			usAntIdx;
	DbSvc_AntInfo_t	stAntenna;
	DAxIterator_t	stIter;

	HxLOG_Trace();

	DB_SVC_DeleteAllAntInfo();

	if (ulSize == 0 || pvData == NULL)
		return;

	DAPI_InitIterator(&stIter, pvData, ulSize, sizeof(DxAntInfo_t));
	while (DAPI_NextIterator(&stIter))
	{
		if (xdb_svc_DamaAdaptorAntenna2ObamaType(&stAntenna, &usAntIdx, (DxAntInfo_t*)stIter.data) == NULL)
			continue;
		if (usAntIdx == 0)
			continue;
		ulCnt += DB_SVC_PutAntInfo(usAntIdx, &stAntenna);
	}
	DB_SVC_CommitAllAntInfo();
	db_main_NotifyUpdate(eSEVT_DB_INITIALIZED, (Handle_t)0, eSvcTable_Antenna, 0, 0);



	HxLOG_Print("[%s] [%d] ulCnt : %d  \n",__FUNCTION__,__LINE__, ulCnt);
}

static HBOOL	xdb_svc_DamaAdaptorServiceInit (const HCHAR *name, HUID uid, const void *pvData, HUINT32 size, void *pvUser)
{
	HUINT32	ulCnt = 0;
	DbSvc_Info_t  stInfo;
	DxService_t *pstList;
	DAxIterator_t stIter;

	RETURN_IF(size == 0 || pvData == NULL, FALSE);
	DB_SVC_DeleteAllServiceInfo();

	DAPI_InitIterator(&stIter, pvData, size, sizeof(DxService_t));
	while (DAPI_NextIterator(&stIter))
	{
		pstList = (DxService_t *)stIter.data;
		if (OTL_CONV_DxService2SVC_Info(&stInfo, pstList) == NULL)
			continue;
		ulCnt += DB_SVC_PutServiceInfoList(&stInfo, 1);
	}
	HxLOG_Print("[%s] [%d] ulCnt : %d  \n",__FUNCTION__,__LINE__, ulCnt);
	DB_SVC_CommitAllSvcInfo();

	DB_SVC_ResetRatingPassedEventId();
	db_main_NotifyUpdate(eSEVT_DB_INITIALIZED, (Handle_t)0, eSvcTable_Service, 0, 0);
	return TRUE;
}

static HBOOL	xdb_svc_DamaAdaptorTransponderInit (const HCHAR *name, HUID uid, const void *pvData, HUINT32 size, void *pvUser)
{
	HUINT32	ulCnt = 0;
	DbSvc_TsInfo_t	stInfo;
	DxTransponder_t *pstList;
	DAxIterator_t stIter;

	RETURN_IF(size == 0 || pvData == NULL, FALSE);
	DB_SVC_DeleteAllTsInfo();

	DAPI_InitIterator(&stIter, pvData, size, sizeof(DxTransponder_t));
	while (DAPI_NextIterator(&stIter))
	{
		pstList = (DxTransponder_t *)stIter.data;
		if (OTL_CONV_DxTransponder2SVC_TsInfo(&stInfo, pstList) == NULL)
		continue;

		ulCnt += DB_SVC_PutTsInfoList(&stInfo, 1);
	}
	HxLOG_Print("[%s] [%d] ulCnt : %d  \n",__FUNCTION__,__LINE__, ulCnt);
	DB_SVC_CommitAllTsInfo();
	db_main_NotifyUpdate(eSEVT_DB_INITIALIZED, (Handle_t)0, eSvcTable_Transponder, 0, 0);

	return TRUE;
}

static HBOOL	xdb_svc_DamaAdaptorNetworkInit (const HCHAR *name, HUID uid, const void *pvData, HUINT32 size, void *pvUser)
{
	HUINT32	ulCnt = 0;
	DbSvc_NetInfo_t	stInfo;
	DxNetwork_t *pstList;
	DAxIterator_t stIter;

	RETURN_IF(size == 0 || pvData == NULL, FALSE);
	DB_SVC_DeleteAllNetworkInfo();

	DAPI_InitIterator(&stIter, pvData, size, sizeof(DxNetwork_t));
	while (DAPI_NextIterator(&stIter))
	{
		pstList = (DxNetwork_t *)stIter.data;
		if (OTL_CONV_DxNetwork2SVC_NetInfo(&stInfo, pstList) == NULL)
			continue;

		ulCnt += DB_SVC_PutNetworkInfoList(&stInfo, 1);
	}
	HxLOG_Print("[%s] [%d] ulCnt : %d  \n",__FUNCTION__,__LINE__, ulCnt);
	DB_SVC_CommitAllNetInfo();
	db_main_NotifyUpdate(eSEVT_DB_INITIALIZED, (Handle_t)0, eSvcTable_Network, 0, 0);

	return TRUE;
}

static HBOOL	xdb_svc_DamaAdaptorGroupInit (const HCHAR *name, HUID uid, const void *pvData, HUINT32 size, void *pvUser)
{
	HUINT32	ulCnt = 0;
	DbSvc_GroupInfo_t stInfo;
	DxGroup_t	*pstList;
	DAxIterator_t stIter;

	RETURN_IF(size == 0 || pvData == NULL, FALSE);
	DB_SVC_DeleteAllGroupInfo();

	DAPI_InitIterator(&stIter, pvData, size, sizeof(DxGroup_t));
	while (DAPI_NextIterator(&stIter))
	{
		pstList = (DxGroup_t *)stIter.data;
		if (OTL_CONV_DxGroup2SVC_GroupInfo(&stInfo, pstList) == NULL)
			continue;

		ulCnt += DB_SVC_PutGroupInfoList(&stInfo, 1);
	}
	HxLOG_Print("[%s] [%d] ulCnt : %d  \n",__FUNCTION__,__LINE__, ulCnt);
	DB_SVC_CommitAllGroupInfo();
	db_main_NotifyUpdate(eSEVT_DB_INITIALIZED, (Handle_t)0, eSvcTable_Group, 0, 0);

	return TRUE;
}

static HBOOL	xdb_svc_DamaAdaptorProviderInit (const HCHAR *name, HUID uid, const void *pvData, HUINT32 size, void *pvUser)
{
	HUINT32	ulCnt = 0;
	DbSvc_PrvInfo_t	stInfo;
	DxProvider_t 	*pstList;
	DAxIterator_t stIter;

	RETURN_IF(size == 0 || pvData == NULL, FALSE);
	DB_SVC_DeleteAllProviderInfo();

	DAPI_InitIterator(&stIter, pvData, size, sizeof(DxProvider_t));
	while (DAPI_NextIterator(&stIter))
	{
		pstList = (DxProvider_t *)stIter.data;
		if (OTL_CONV_DxProvider2SVC_PrvInfo(&stInfo, pstList) == NULL)
			continue;

		ulCnt += DB_SVC_PutProviderInfoList(&stInfo, 1);
	}
	HxLOG_Print("[%s] [%d] ulCnt : %d  \n",__FUNCTION__,__LINE__, ulCnt);
	DB_SVC_CommitAllProviderInfo();
	db_main_NotifyUpdate(eSEVT_DB_INITIALIZED, (Handle_t)0, eSvcTable_Provider, 0, 0);

	return TRUE;
}

static HBOOL	xdb_svc_DamaAdaptorBouquetInit (const HCHAR *name, HUID uid, const void *pvData, HUINT32 size, void *pvUser)
{
	HUINT32	ulCnt = 0;
	DbSvc_BouqInfo_t  stInfo;
	DxBouquet_t *pstList;
	DAxIterator_t stIter;

	RETURN_IF(size == 0 || pvData == NULL, FALSE);
	DB_SVC_DeleteAllBouquetInfo();

	DAPI_InitIterator(&stIter, pvData, size, sizeof(DxBouquet_t));
	while (DAPI_NextIterator(&stIter))
	{
		pstList = (DxBouquet_t *)stIter.data;
		if (OTL_CONV_DxBouquet2SVC_BouqInfo(&stInfo, pstList) == NULL)
			continue;

		ulCnt += DB_SVC_PutBouquetInfoList(&stInfo, 1);
	}
	DB_SVC_CommitAllBouquetInfo();
	db_main_NotifyUpdate(eSEVT_DB_INITIALIZED, (Handle_t)0, eSvcTable_Bouquet, 0, 0);

	HxLOG_Print("[%s] [%d] ulCnt : %d  \n",__FUNCTION__,__LINE__, ulCnt);

	return TRUE;
}

STATIC void		xdb_svc_DamaAdaptorOnAntennaUpdated (const HCHAR *pszName, HUID hUid, const void *pvData, HUINT32 ulSize, void *pvUser)
{
	(void)pszName;
	(void)hUid;
	(void)pvData;
	(void)ulSize;
	(void)pvUser;

	HxLOG_Trace();

	DAPI_GetAll(DxNAMEOF(DxAntInfo_t), xdb_svc_DamaAdaptorAntennaGetter, NULL);
}


STATIC HERROR	xdb_svc_DamaAdaptorInitAntennaDB (void)
{
	HINT32	id;

	HxLOG_Trace();

	id = DAPI_GetAll(DxNAMEOF(DxAntInfo_t), xdb_svc_DamaAdaptorAntennaGetter, NULL);
	if (id == 0)
	{
		HxLOG_Error("DAPI_GetAll failed. \n");
	}
	return ERR_OK;
}

STATIC HERROR	xdb_svc_DamaAdaptorInitServiceDB(void)
{
	HINT32	id;

	HxLOG_Trace();

	id = DAPI_GetAll(DxNAMEOF(DxService_t), (DAPI_Getter)xdb_svc_DamaAdaptorServiceInit, NULL);
	if (id == 0)
	{
		HxLOG_Error("DAPI_GetAll failed. \n");
	}

	return ERR_OK;
}
STATIC HERROR	xdb_svc_DamaAdaptorInitTransponderDB(void)
{
	HINT32	id;

	HxLOG_Trace();

	id = DAPI_GetAll(DxNAMEOF(DxTransponder_t), (DAPI_Getter)xdb_svc_DamaAdaptorTransponderInit, NULL);
	if (id == 0)
	{
		HxLOG_Error("DAPI_GetAll failed. \n");
	}

	return ERR_OK;
}
STATIC HERROR	xdb_svc_DamaAdaptorInitNetworkDB(void)
{
	HINT32	id;

	HxLOG_Trace();

	id = DAPI_GetAll(DxNAMEOF(DxNetwork_t), (DAPI_Getter)xdb_svc_DamaAdaptorNetworkInit, NULL);
	if (id == 0)
	{
		HxLOG_Error("DAPI_GetAll failed. \n");
	}

	return ERR_OK;
}
STATIC HERROR	xdb_svc_DamaAdaptorInitGroupDB(void)
{
	HINT32	id;

	HxLOG_Trace();

	id = DAPI_GetAll(DxNAMEOF(DxGroup_t), (DAPI_Getter)xdb_svc_DamaAdaptorGroupInit, NULL);
	if (id == 0)
	{
		HxLOG_Error("DAPI_GetAll failed. \n");
	}

	return ERR_OK;
}
STATIC HERROR	xdb_svc_DamaAdaptorInitProviderDB(void)
{
	HINT32	id;
	HxLOG_Trace();
	id = DAPI_GetAll(DxNAMEOF(DxProvider_t), (DAPI_Getter)xdb_svc_DamaAdaptorProviderInit, NULL);
	if (id == 0)
	{
		HxLOG_Error("DAPI_GetAll failed. \n");
	}

	return ERR_OK;
}
STATIC HERROR	xdb_svc_DamaAdaptorInitBouquetDB(void)
{
	HINT32	id;
	HxLOG_Trace();
	id = DAPI_GetAll(DxNAMEOF(DxBouquet_t), (DAPI_Getter)xdb_svc_DamaAdaptorBouquetInit, NULL);
	if (id == 0)
	{
		HxLOG_Error("DAPI_GetAll failed. \n");
	}

	return ERR_OK;
}



STATIC void		xdb_svc_DamaAdaptorServiceGetter (const HCHAR *pszName, HUID hUid, const void *pvData, HUINT32 ulSize, void *pvUser)
{
	xdbSvc_DmAdapterArray_t	*pstArray;
	Handle_t 				 hSvc;
	DbSvc_Info_t 			*pstInfos;
	HxPair_t				*pstItem = (HxPair_t *)pvUser;
	HUINT32 				 ulUid;
	HINT32					 i, j;
	HUINT32					 ulNumberOfSvc = 0;
	HUID					*pulUidlist = NULL;

	HBOOL			bChanged = FALSE, bCheckLock = FALSE;
	HUINT32	*pulUidCheck = NULL;
	HUINT32	ulIdx = 0;

	if(pstItem)
	{
		ulNumberOfSvc = (HUINT32)pstItem->left;
		pulUidlist = (HUID *)pstItem->right;
	}
	else
	{
		return;
	}

	if(pstItem->right == NULL)
	{
		HLIB_STD_MemFree(pstItem);
		return;
	}

	if(ulNumberOfSvc == 0)
	{
		HLIB_STD_MemFree(pstItem->right);
		HLIB_STD_MemFree(pstItem);
		return;
	}


	pulUidCheck = (HUINT32 *)HLIB_MEM_Calloc(sizeof(HUINT32) * ulNumberOfSvc);

	if(pulUidCheck == NULL)
	{
		HLIB_STD_MemFree(pstItem->right);
		HLIB_STD_MemFree(pstItem);
		return;
	}
#if defined(CONFIG_DEBUG)
	{
		HUID	*pulTemp = pulUidlist;

		for (i = ulNumberOfSvc;  i-- ; pulTemp++)
		{
			ulUid = (HUINT32)*pulTemp;
			HxLOG_Print("[%s] [%d]svcuid : %d\n",__FUNCTION__,__LINE__, ulUid);
		}
	}
#endif

	pstArray = NULL;

	(void)xdb_svc_DamaAdaptorDataGetter(pszName, hUid, pvData, ulSize, (void *)&pstArray);

	if (pstArray== NULL)
	{
		for (i = ulNumberOfSvc;  i-- ; pulUidlist++)
		{
			ulUid = (HUINT32)*pulUidlist;
			if (DB_SVC_GetServiceHandleBySvcIdx((HUINT16)ulUid, &hSvc) == ERR_OK)
			{
				// Delete
				HxLOG_Warning("Delete service !!(ulUid: %d) \n", ulUid);
				DB_SVC_DeleteServiceInfo(hSvc);
				bChanged = TRUE;
			}
		}
 	}
	else
	{
		pstInfos = (DbSvc_Info_t *) pstArray->aucData;
		if (pstInfos)
		{
			for(i = 0 ; i < pstArray->ulLength ; i++)
			{
				// Update
				if (DB_SVC_GetServiceHandleBySvcIdx(DbSvc_GetSvcIdx(&pstInfos[i]), &hSvc) == ERR_OK)
				{
					DbSvc_Info_t	stOld;
					HERROR		hErr;

					HxLOG_Warning("Update service !!(uid: %d) \n", DbSvc_GetSvcIdx(&pstInfos[i]));
					hErr = DB_SVC_GetServiceInfo(hSvc, &stOld);
					if (hErr != ERR_OK)
					{
						continue;
					}

					// 실제 변경사항이 있는지 비교!!
					if (HxSTD_memcmp(&pstInfos[i], &stOld, sizeof(DbSvc_Info_t)) != 0)
					{
						hErr = DB_SVC_UpdateServiceInfo(hSvc, &pstInfos[i]);
						if (hErr == ERR_OK && DbSvc_GetLocked(&stOld) != DbSvc_GetLocked(&pstInfos[i]))
						{
							bCheckLock = TRUE;
						}
						bChanged = TRUE;
					}

				}
				else
				{
					// Add
					HxLOG_Warning("Add service !!(uid: %d) \n", DbSvc_GetSvcIdx(&pstInfos[i]));
					DB_SVC_PutServiceInfoList(&pstInfos[i], 1);

					bChanged = TRUE;
				}
				pulUidCheck[ulIdx++] = DbSvc_GetSvcIdx(&pstInfos[i]);

			}
		}


		//2 Delete svclist
		// Appkit 에서 처리한 service 개수(ulIdx)와 dama로 부터 넘어온 service 개수(ulNumberOfSvc)를 비교
		if (ulIdx != ulNumberOfSvc)
		{
			HxLOG_Debug("The number of remaining services : %d\n", ulNumberOfSvc - ulIdx);
			 //다르면 삭제된 service가 남아있다는 의미
			for (i = ulNumberOfSvc;  i-- ; pulUidlist++)
			{
				 //처리되지 않은 UID를 찾아서 해당 service를 제거한다.
				ulUid = (HUINT32)*pulUidlist;
				for (j = 0 ; j < ulIdx; j++)
				{
					HxLOG_Debug("ulUid : %d,  pulUidCheck[%d] : %d\n",ulUid, j, pulUidCheck[j]  );
					if (pulUidCheck[j] == ulUid)
					{
						break;
					}
				}

				 //루프 끝까지 왔으니 처리된 리스트에서 못찾았다는 의미이다
				HxLOG_Debug(" j : %d, ulIdx : %d\n", j, ulIdx);
				if (j == ulIdx)
				{
					if (DB_SVC_GetServiceHandleBySvcIdx((HUINT16)ulUid, &hSvc) == ERR_OK)
					{
						// Delete
						HxLOG_Warning("Delete service !!(ulUid: %d) \n", ulUid);
						DB_SVC_DeleteServiceInfo(hSvc);
						bChanged = TRUE;
					}
				}
			}
		}



		HLIB_STD_MemFree(pstArray);
	}

	if(bCheckLock)
	{
		db_main_NotifyUpdate(eSEVT_DB_SVCLIST_CHECK_LOCK, (Handle_t)NULL, 0, 0, 0);
	}

	if (bChanged)
	{
		// svc list가 변경되었음을 알림!!
		db_main_NotifyUpdate(eSEVT_DB_SVCLIST_CHANGED_BY_DAMA, (Handle_t)NULL, 0, 0, 0);
	}

	if (pulUidCheck)
	{
		HLIB_STD_MemFree(pulUidCheck);
	}
	HLIB_STD_MemFree(pstItem->right);
	HLIB_STD_MemFree(pstItem);
	DB_SVC_Commit();
}

STATIC void xdb_svc_DamaAdaptorAllServiceGetter (const HCHAR *pszName, HUID hUid, const void *pvData, HUINT32 ulSize, void *pvUser)
{
	HBOOL					 bUpdateDb = FALSE;
	HUINT32					 ulDbSvcCnt, ulDbSvcNum = 0;
	DbSvc_Attr_t			 stAttr;
	Handle_t				*phSvcList = NULL;
	xdbSvc_DmAdapterArray_t	*pstArray = NULL;
	HERROR					 hErr;

	HxLOG_Trace();

	DB_SVC_InitSvcAttribute(&stAttr);
	hErr = DB_SVC_FindServiceList(eSvcGroup_All, &stAttr, 0, HANDLE_NULL, eSvcFindDirection_FowardFromStart, &ulDbSvcNum, &phSvcList);
	if ((ERR_OK != hErr) || (0 == ulDbSvcNum) || (NULL == phSvcList))
	{
		ulDbSvcNum = 0;
		if (NULL != phSvcList)
		{
			DB_SVC_FreeServiceList(phSvcList);
			phSvcList = NULL;
		}
	}

	(void)xdb_svc_DamaAdaptorDataGetter(pszName, hUid, pvData, ulSize, (void *)&pstArray);
	if ((NULL != pstArray) && (pstArray->ulLength > 0))
	{
		HUINT32				 ulDamaSvcCnt;
		DbSvc_Info_t		*pstSvcArray = (DbSvc_Info_t *)pstArray->aucData;

		for (ulDamaSvcCnt = 0; ulDamaSvcCnt < pstArray->ulLength; ulDamaSvcCnt++)
		{
			Handle_t			 hSvc = HANDLE_NULL;
			DbSvc_Info_t		*pstSvcInfo = &(pstSvcArray[ulDamaSvcCnt]);

			for (ulDbSvcCnt = 0; ulDbSvcCnt < ulDbSvcNum; ulDbSvcCnt++)
			{
				if (DB_SVC_GetServiceUID(phSvcList[ulDbSvcCnt]) == DbSvc_GetSvcIdx(pstSvcInfo))
				{
					phSvcList[ulDbSvcCnt] = HANDLE_NULL;
					break;
				}
			}

			if (ulDbSvcCnt >= ulDbSvcNum)
			{
				hErr = DB_SVC_AddServiceInfo(pstSvcInfo, &hSvc);
				if (ERR_OK == hErr)
				{
					bUpdateDb = TRUE;
				}
				else
				{
					HxLOG_Critical("DB_SVC_AddServiceInfo failed: SvcNo:%d, SvcUid:%d\n", DbSvc_GetSvcIdx(pstSvcInfo), DbSvc_GetLcn(pstSvcInfo));
				}
			}
		}

		for (ulDbSvcCnt = 0; ulDbSvcCnt < ulDbSvcNum; ulDbSvcCnt++)
		{
			if (HANDLE_NULL != phSvcList[ulDbSvcCnt])
			{
				hErr = DB_SVC_DeleteServiceInfo(phSvcList[ulDbSvcCnt]);
				if (ERR_OK == hErr)
				{
					bUpdateDb = TRUE;
				}
				else
				{
					HxLOG_Critical("DB_SVC_DeleteServiceInfo failed: hSvc(0x%08x)\n", phSvcList[ulDbSvcCnt]);
				}
			}
		}
	}
	else
	{
		DB_SVC_DeleteAllServiceInfo();
		DB_SVC_DeleteAllTsInfo();
		DB_SVC_DeleteAllNetworkInfo();
		DB_SVC_DeleteAllProviderInfo();
	}

	if (NULL != pstArray)
	{
		HLIB_STD_MemFree(pstArray);
	}

	if (NULL != phSvcList)
	{
		DB_SVC_FreeServiceList(phSvcList);
	}

	if (TRUE == bUpdateDb)
	{
		DB_SVC_Commit();
	}
}

STATIC void	xdb_svc_DamaAdaptorOnServiceUpdated(const HCHAR *pszName, HUID hUid, const void *pvData, HUINT32 ulSize, void *pvUser)
{
	HUINT32		 ulNumberOfSvc = 0;
	HUID		*pulUidlist = (HUID *)pvData;
	HUINT32 	 ulUid;
	HxPair_t	*stItem;

	ulNumberOfSvc = (HUINT32)(ulSize / sizeof(HUID));

	// Reset 되어있는지 확인
	if (ulNumberOfSvc == 1)
	{
		ulUid = *pulUidlist;
		if (ulUid == -1)
		{
			// Delete all list
			HxLOG_Warning("Remove ALL info\n");
			DB_SVC_ClearAllInfo();
			return ;
		}
	}
	stItem= (HxPair_t *)HLIB_STD_MemCalloc(sizeof(HxPair_t));
	if (stItem == NULL)
	{
		HxLOG_Error(" Fail to mem Calloc\n");
		return ;
	}

	stItem->left = (void *)ulNumberOfSvc;
	stItem->right = (void *)HLIB_STD_MemCalloc(ulSize);
	if (stItem->right)
	{
		HxSTD_memcpy(stItem->right, (void *)pulUidlist, ulSize);
	}

	DAPI_GetUIDInfos(DxNAMEOF(DxService_t), (HUID*)pvData, ulNumberOfSvc, xdb_svc_DamaAdaptorServiceGetter, (void *)stItem);
}

// Register DAMA notifier for service update
HERROR	xdb_svc_DamaAdaptorRegisterNotifier_Base(HBOOL bReg)
{
	STATIC	HUINT32	s_ulNotiId = 0;

	if (bReg)
	{
		if (s_ulNotiId == 0)
		{
			HxLOG_Print("call DAPI_AddTableNotifier (s_ulNotiId = %d)\n", s_ulNotiId);
			s_ulNotiId = DAPI_AddTableNotifier(DxNAMEOF(DxService_t), xdb_svc_DamaAdaptorOnServiceUpdated, NULL);
		}
		else
		{
			return ERR_FAIL;
		}
	}
	else
	{
		if (s_ulNotiId != 0)
		{
			HxLOG_Print("call DAPI_RemoveNotifier (s_ulNotiId = %d)\n", s_ulNotiId);
			DAPI_RemoveNotifier(s_ulNotiId);
			s_ulNotiId = 0;
			xdb_svc_DamaAdaptorLoadServiceInfoDB_Base();
		}
		else
		{
			return ERR_FAIL;
		}
	}

	return ERR_OK;
}

#if 0
HERROR	pdb_svc_DamaAdaptorInitServiceDB (void)
{
	return xdb_svc_DamaAdaptorRegisterNotifier(TRUE);
}
HERROR pdb_svc_DamaAdaptorRegisterNotifier(HBOOL bReg)
{
	return xdb_svc_DamaAdaptorRegisterNotifier(bReg);
}
#endif

HERROR	xdb_svc_DamaAdaptorInitAntennaDB_Base(void)
{
	HERROR	hErr;

	hErr = xdb_svc_DamaAdaptorInitAntennaDB();
	return hErr;
}
HERROR	xdb_svc_DamaAdaptorInitServiceDB_Base(void)
{
	HERROR	hErr;
	hErr = xdb_svc_DamaAdaptorInitServiceDB();
	return hErr;
}
HERROR	xdb_svc_DamaAdaptorInitTransponderDB_Base(void)
{
	HERROR	hErr;
	hErr = xdb_svc_DamaAdaptorInitTransponderDB();
	return hErr;
}
HERROR	xdb_svc_DamaAdaptorInitNetworkDB_Base(void)
{
	HERROR	hErr;
	hErr = xdb_svc_DamaAdaptorInitNetworkDB();
	return hErr;
}
HERROR	xdb_svc_DamaAdaptorInitGroupDB_Base(void)
{
	HERROR	hErr;
	hErr = xdb_svc_DamaAdaptorInitGroupDB();
	return hErr;
}
HERROR	xdb_svc_DamaAdaptorInitProviderDB_Base(void)
{
	HERROR	hErr;
	hErr = xdb_svc_DamaAdaptorInitProviderDB();
	return hErr;
}
HERROR	xdb_svc_DamaAdaptorInitBouquetDB_Base(void)
{
	HERROR	hErr;
	hErr = xdb_svc_DamaAdaptorInitBouquetDB();
	return hErr;
}


HERROR	xdb_svc_DamaAdaptorUpdateSvcInfo_Base (const DbSvc_Info_t *pstInfo)
{
#if 0
	HERROR		hErr;
	DxService_t	stService;

	HxLOG_Assert(pstInfo);
	HxLOG_Print("%s("HxANSI_COLOR_YELLOW("%d. %s")")\n", __FUNCTION__, pstInfo->usLcn, pstInfo->szSvcName);
	if (OTL_CONV_SVC_Info2DxService(&stService, pstInfo) == NULL)
	{
		HxLOG_Error("%s() error to convert obama type to dlib!\n", __FUNCTION__);
		return ERR_FAIL;
	}

	hErr = DAPI_LazySet(DxNAMEOF(DxService_t), stService.uid, &stService, sizeof(DxService_t), 200);
	//if (hErr == ERR_OK)
	//	hErr = DAPI_LazySync(DxNAMEOF(DxService_t), 1000);
	return hErr;
#endif
	return ERR_OK;
}

HERROR	xdb_svc_DamaAdaptorLoadServiceInfoDB_Base (void)
{
	HUINT32		ulId;

	HxLOG_Trace();

	ulId = DAPI_GetAll(DxNAMEOF(DxService_t), xdb_svc_DamaAdaptorAllServiceGetter, NULL);

	return DAPI_Wait(60000, ulId) == DERR_TIMEOUT ? ERR_FAIL : ERR_OK;
}

HERROR	xdb_svc_DamaAdaptorLoadAntennaDB_Base (void)
{
	HUINT32	ulId;

	HxLOG_Trace();

	ulId = DAPI_GetAll(DxNAMEOF(DxAntInfo_t), xdb_svc_DamaAdaptorAntennaGetter, NULL);
	if (ulId == 0)
	{
		HxLOG_Error("DAPI_GetAll failed. \n");
	}


	return ERR_OK;
}

HERROR	xdb_svc_DamaAdaptorSyncAntennaDB_Base (void)
{
	HERROR				 hErr;
	HINT32				 i, n;
	DbSvc_AntInfo_t		 stAntInfo;
	DxAntInfo_t			 stDxAntInfo;
	HINT32				*anIdxList = NULL;

	HxLOG_Trace();

	RETURN_IF(!DB_SVC_IsAntChanged(), ERR_FAIL);

	hErr = DB_SVC_FindAllAntIdxList(&n, &anIdxList);
	RETURN_IF(hErr != ERR_OK, hErr);

	DAPI_ResetTable(DxNAMEOF(DxAntInfo_t));
	DAPI_BatchBegin(DxNAMEOF(DxAntInfo_t));
	for (i = 0 ; i < n ; i++)
	{
		if (DB_SVC_GetAntInfo((HUINT16)anIdxList[i], &stAntInfo) != ERR_OK)
			continue;

		if (xdb_svc_DamaAdaptorAntenna2DxType(&stDxAntInfo, &stAntInfo, (HUINT16)anIdxList[i]))
		{
			//DLIB_Print(DxNAMEOF(DxAntInfo_t), NULL, &stDxAntInfo);
			DAPI_Set(DxNAMEOF(DxAntInfo_t), stDxAntInfo.uid, &stDxAntInfo, sizeof(DxAntInfo_t));
		}
	}
	DAPI_BatchEnd(DxNAMEOF(DxAntInfo_t));

	DB_SVC_FreeAntList(anIdxList);
	DB_SVC_CommitAllAntInfo();
	return ERR_OK;
}




#if 0

STATIC HERROR	sm_dmadapter_InitDB (void)
{
	HUINT32	tick;
	DERROR	hErr;
	HUINT32	i, n;
	xdbSvc_DmAdapterArray_t	*array;
	HUINT32	ids[10];
	struct __initter {
		const HCHAR *dname;
		HUINT32		(* putData) (const void *, HUINT32);
		xdbSvc_DmAdapterArray_t	*array;
	} items[] = {
		  {DxNAMEOF(DxService_t), (void *)OxDB_SVC_PutServiceInfoList, NULL}
		, {DxNAMEOF(DxTransponder_t), (void *)OxDB_SVC_PutTsInfoList, NULL}
		, {DxNAMEOF(DxNetwork_t), (void *)OxDB_SVC_PutNetworkInfoList, NULL}
		, {DxNAMEOF(DxProvider_t), (void *)OxDB_SVC_PutProviderInfoList, NULL}
		, {DxNAMEOF(DxBouquet_t), (void *)OxDB_SVC_PutBouquetInfoList, NULL}
		, {DxNAMEOF(DxGroup_t), (void *)OxDB_SVC_PutGroupInfoList, NULL}
	};

	HxLOG_Trace();

	OxDB_SVC_ClearAllInfo();

	tick = HLIB_STD_GetSystemTick();
	hErr  = DERR_OK;
	n    = sizeof(items) / sizeof(items[0]);
	for (i = 0 ; i < n ; i++)
	{
		ids[i] = DAPI_GetAll(items[i].dname, sm_dmadapter_DataGetter, &(items[i].array));
		//(void) DAPI_AddNotifier(items[i].dname, 0, sm_dmadapter_OnUpdated, NULL);
		(void)sm_dmadapter_OnUpdated;
	}
	RETURN_IF(DAPI_WaitRequests(60000, ids, n) == DERR_TIMEOUT, ERR_FAIL);

	for (i = 0 ; i < n ; i++)
	{
		array = items[i].array;
		if (array == NULL)
		{
			HxLOG_Info("%s() Empty table(%s)\n", __FUNCTION__, items[i].dname);
			continue;
		}
		(void) items[i].putData((const void *)array->data, array->length);
		HxLOG_Info("%s() %s loading done(%u)\n", __FUNCTION__, items[i].dname, array->length);
		HLIB_STD_MemFree(array);
	}

	HxLOG_Info("%s() elapsed tick:%u\n", __FUNCTION__, HLIB_STD_GetSystemTick() - tick);

	return ERR_OK;
}

STATIC void		sm_dmadapter_OnUpdated (const HCHAR *pszName, HUID hUid, const void *pvData, HUINT32 ulSize, void *pvUser)
{
	HINT32	sd;
	HINT32	uidCount;
	const HINT32	*uidList;
	xdbSvc_DmAdapterArray_t	*array;

	RETURN_IF_VOID(ulSize == 0 || pvData == NULL);

	uidCount = ulSize / sizeof(HINT32);
	uidList  = (const HINT32 *)pvData;
	RETURN_IF_VOID(uidCount == 0);

	sd = HLIB_RWSTREAM_Open(NULL, 0);
	RETURN_IF_VOID(!sd);

	HLIB_RWSTREAM_Print(sd, "SELECT * FROM %s WHERE uid IN(", pszName);
	while (uidCount--)
		HLIB_RWSTREAM_Print(sd, "%d%c", *uidList++, uidCount ? ',' : ')');

	array = (xdbSvc_DmAdapterArray_t *)HLIB_STD_MemAlloc(sizeof(xdbSvc_DmAdapterArray_t) + ulSize);
	if (array)
	{
		memcpy(array->data, pvData, ulSize);
		array->length = ulSize;
		DAPI_Query(HLIB_RWSTREAM_GetBuf(sd), sm_dmadapter_HandleUpdated, (void *)array);
	}
	HLIB_RWSTREAM_Close(sd);
}


STATIC void	sm_dmadapter_HandleUpdated (const HCHAR *pszName, HUID uid, const void *pvData, HUINT32 ulSize, void *pvUser)
{
	xdbSvc_DmAdapterArray_t	*uidList;
	xdbSvc_DmAdapterArray_t	*datList;

	uidList = (xdbSvc_DmAdapterArray_t *)pvUser;
	datList = NULL;
	(void)sm_dmadapter_DataGetter(pszName, uid, pvData, ulSize, (void *)&datList);

	if (datList == NULL)
	{
		// TODO: remove all
	}
	else
	{
		HUID *uid;
		HUINT32	i, n;

		uid = (HUID *)uidList->data;
		if(uidList == NULL)
		{
		}

		n   = uidList->length / sizeof(HUID);

		while (n--)
		{
			DbSvc_Info_t	*data = (DbSvc_Info_t *)datList->data;
			for (i = 0 ; i < datList->length ; i++)
			{
				if (data->usSvcIdx == (HUINT16)*uid)
					break;
				data++;
			}
			if (i == datList->length)
			{
				OxDB_SVC_DeleteServiceInfo((Handle_t)*uid);
			}
			else
			{
				Handle_t hSvc;
				if (OxDB_SVC_GetServiceHandleBySvcIdx(data->usSvcIdx, &hSvc) == ERR_OK)
				{

					OxDB_SVC_UpdateServiceInfo(hSvc, data);
				}
				else{
					OxDB_SVC_AddServiceInfo(data, &hSvc);

					}
			}
			uid++;
		}
		HLIB_STD_MemFree(datList);
	}
	HLIB_STD_MemFree(pvUser);
}

#endif

