/**
	@file     mgr_search_util.c
	@brief    file_name & simple comment.

	Description: Applicatuion for installation menu\n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2008/06/10		initiated					wjmoh\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/
#include <stdio.h>
#include <stdlib.h>
#include <htype.h>

#include <bus.h>

#include <db_svc.h>
#include <db_param.h>

#include <svc_pipe.h>
#include <svc_resources.h>
#include <svc_ch.h>

#include <mgr_search.h>

#include <isosvc.h>


typedef enum
{
	eSat_tuner_01=0,
	eSat_tuner_02,
	eSat_tunerMAX,
} SatTuner_t;

typedef enum
{
	eSat_NetSearch_Disable=0,
	eSat_NetSearch_Enable,
	eSat_NetSearch_MAX,
} SatNetwork_Search_t;

#ifdef CONFIG_MW_CH_SATELLITE
#define	MAX_SAT_TUNER			eSat_tunerMAX

static DxDBSETUP_Item_e		s_QueryTunerText[MAX_SAT_TUNER]={
	eDB_PARAM_ITEM_ANTSET_SAT_TUNER01,
	eDB_PARAM_ITEM_ANTSET_SAT_TUNER02
//	eDB_PARAM_ITEM_ANTSET_SAT_TUNER03
};
#endif

#ifdef CONFIG_MW_CH_CABLE
typedef enum
{
	eCab_tuner_01=0,
	eCab_tuner_02,
	eCab_tunerMAX,
} CabTuner_t;

#define	MAX_CAB_TUNER			eCab_tunerMAX
#endif

// ======================================================
// DB 외에 메모리에만 잠시 저장해 두는 static variables :
// ======================================================

#ifdef CONFIG_MW_CH_TERRESTRIAL
// Terrestrial Tuner ID :
static HBOOL		 s_bSrchVal_TerTunerId_Available = FALSE;
static HUINT32		 s_ulSrchVal_TerTunerId;

// Terrestrial Search Type :
static HBOOL		 s_bSrchVal_TerSrchType_Available = FALSE;
static HUINT32		 s_ulSrchVal_TerSrchType;
#endif



// ======================================================


#ifdef CONFIG_MW_CH_SATELLITE

HERROR MGR_SEARCHUTIL_GetUserSatTpInfo (DbSvc_TuningInfo_t *pstTuneInfo)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (pstTuneInfo == NULL)				{ return ERR_BUS_SOURCE_NULL; }

	HxSTD_memset (pstTuneInfo, 0, sizeof(DbSvc_TuningInfo_t));

	// Frequency
	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_MAN_TP_FREQUENCY, &ulValue, 0);
	if (hErr == ERR_OK)
	{
		pstTuneInfo->sat.ulFrequency = ulValue;
	}

	// Symbol Rate
	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_MAN_TP_SYMBOLRATE, &ulValue, 0);
	if (hErr == ERR_OK)
	{
		pstTuneInfo->sat.ulSymbolRate = ulValue;
	}

	// Polarization
	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_MAN_TP_POLARIZATION, &ulValue, 0);
	if (hErr == ERR_OK)
	{
		pstTuneInfo->sat.ePolarization = ulValue;
	}

	// FEC
	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_MAN_TP_FEC, &ulValue, 0);
	if (hErr == ERR_OK)
	{
		pstTuneInfo->sat.eFecCodeRate = ulValue;
	}

	// Transmission
	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_MAN_TP_TRANSSPEC, &ulValue, 0);
	if (hErr == ERR_OK)
	{
		pstTuneInfo->sat.eTransSpec = ulValue;
	}

	// Modulation
	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_MAN_TP_MODULATION, &ulValue, 0);
	{
		pstTuneInfo->sat.ePskMod = ulValue;
	}

	// Pilot
	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_MAN_TP_PILOT, &ulValue, 0);
	if (hErr == ERR_OK)
	{
		pstTuneInfo->sat.ePilot = ulValue;
	}

	// Roll 은 eDxSAT_ROLL_035
	pstTuneInfo->sat.eRollOff = eDxSAT_ROLL_035;

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetUserSatTpInfo (DbSvc_TuningInfo_t *pstTuneInfo)
{
	HUINT32	ulValue;

	if (pstTuneInfo == NULL)				{ return ERR_BUS_SOURCE_NULL; }

	// Frequency
	ulValue = (HUINT32)pstTuneInfo->sat.ulFrequency;
	DB_PARAM_SetItem (eDB_PARAM_ITEM_MAN_TP_FREQUENCY, ulValue, 0);

	// Symbol Rate
	ulValue = (HUINT32)pstTuneInfo->sat.ulSymbolRate;
	DB_PARAM_SetItem (eDB_PARAM_ITEM_MAN_TP_SYMBOLRATE, ulValue, 0);

	// Polarization
	ulValue = (HUINT32)pstTuneInfo->sat.ePolarization;
	DB_PARAM_SetItem (eDB_PARAM_ITEM_MAN_TP_POLARIZATION, ulValue, 0);

	// FEC
	ulValue = (HUINT32)pstTuneInfo->sat.eFecCodeRate;
	DB_PARAM_SetItem (eDB_PARAM_ITEM_MAN_TP_FEC, ulValue, 0);

	// Transmission
	ulValue = (HUINT32)pstTuneInfo->sat.eTransSpec;
	DB_PARAM_SetItem (eDB_PARAM_ITEM_MAN_TP_TRANSSPEC, ulValue, 0);

	// Modulation
	ulValue = (HUINT32)pstTuneInfo->sat.ePskMod;
	DB_PARAM_SetItem (eDB_PARAM_ITEM_MAN_TP_MODULATION, ulValue, 0);

	// Pilot
	ulValue = (HUINT32)pstTuneInfo->sat.ePilot;
	DB_PARAM_SetItem (eDB_PARAM_ITEM_MAN_TP_PILOT, ulValue, 0);

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_GetSatTunerIdFromDb (HUINT32 *pulTunerId)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (pulTunerId == NULL)					{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_SAT_TUNER_SELECTION, &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_BUS_NO_OBJECT; }

	*pulTunerId = ulValue;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetSatTunerIdToDb (HUINT32 ulTunerId)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (ulTunerId >= MAX_SAT_TUNER)			{ return ERR_BUS_OUT_OF_RANGE; }

	ulValue = (HUINT32)ulTunerId;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_SAT_TUNER_SELECTION, ulValue, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_GetSatAllAntFlagFromDb (HBOOL *pbSrchAllAnt)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (pbSrchAllAnt == NULL)					{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_SAT_SRCH_ALL_ANT, &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_BUS_NO_OBJECT; }

	*pbSrchAllAnt = (HBOOL)ulValue;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetSatAllAntFlagFromDb (HBOOL bSrchAllAnt)
{
	HERROR	hErr;
	HUINT32	ulValue;

	ulValue = (HUINT32)bSrchAllAnt;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_SAT_SRCH_ALL_ANT, ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_FAIL; }

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_GetSatAntennaIdFromDb (HUINT32 ulTunerId, HUINT32 *pulAntIdx)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (pulAntIdx == NULL)					{ return ERR_BUS_TARGET_NULL; }
	if (ulTunerId >= MAX_SAT_TUNER)			{ ulTunerId = 0; }

	hErr = DB_PARAM_GetItem (s_QueryTunerText[ulTunerId], &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_BUS_NO_OBJECT; }

	*pulAntIdx = (HUINT32)ulValue;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetSatAntennaIdToDb (HUINT32 ulTunerId, HUINT32 ulAntIdx)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (ulTunerId >= MAX_SAT_TUNER)			{ ulTunerId = 0; }

	ulValue = (HUINT32)ulAntIdx;

	hErr = DB_PARAM_SetItem (s_QueryTunerText[ulTunerId], ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_FAIL; }

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_GetSatTpIndexFromDb (HUINT32 *pulTpIndex)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (pulTpIndex == NULL)					{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_SAT_TRANSPONDER, &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_BUS_NO_OBJECT; }

	*pulTpIndex = (HUINT32)ulValue;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetSatTpIndexToDb (HUINT32 ulTpIndex)
{
	HERROR	hErr;
	HUINT32	ulValue;

	ulValue = ulTpIndex;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_SAT_TRANSPONDER, ulValue, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_GetSatNetworkSearchFromDb (HUINT32 *pulNetwork)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (pulNetwork == NULL)					{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_SAT_NETWORK_SRCH, &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_BUS_NO_OBJECT; }

	*pulNetwork = (ulValue != 0) ? eSat_NetSearch_Enable : eSat_NetSearch_Disable;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetSatNetworkSearchToDb (HUINT32 ulNetwork)
{
	HERROR	hErr;
	HUINT32	ulValue;

	ulValue = ulNetwork;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_SAT_NETWORK_SRCH, ulValue, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_GetSatScrambleTypeFromDb (HUINT32 *pulCasType)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (pulCasType == NULL)					{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_SAT_CAFREE_TYPE, &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_BUS_NO_OBJECT; }

	*pulCasType = ulValue;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetSatScrambleTypeToDb (HUINT32 ulCasType)
{
	HERROR	hErr;
	HUINT32	ulValue;

	ulValue = ulCasType;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_SAT_CAFREE_TYPE, ulValue, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_GetSatChannelTypeFromDb (HUINT32 *pulChType)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (pulChType == NULL)					{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_SAT_CHANNEL_TYPE, &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_BUS_NO_OBJECT; }

	*pulChType = ulValue;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetSatChannelTypeToDb (HUINT32 ulChType)
{
	HERROR	hErr;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_SAT_CHANNEL_TYPE, ulChType, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_GetSatSearchModeFromDb (HUINT32 *pulSearchMode)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (pulSearchMode == NULL)					{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_SAT_SEARCH_MODE, &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_BUS_NO_OBJECT; }

	*pulSearchMode = ulValue;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetSatSearchModeToDb (HUINT32 ulSearchMode)
{
	HERROR	hErr;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_SAT_SEARCH_MODE, ulSearchMode, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_GetSatAntennaTypeFromDb (HUINT32 *pulAntennaType)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (pulAntennaType == NULL)				{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_ANTSET_ANTENNATYPE, &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_BUS_NO_OBJECT; }

	*pulAntennaType = ulValue;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetSatAntennaTypeToDb (HUINT32 ulAntennaType)
{
	HERROR	hErr;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_ANTSET_ANTENNATYPE, (HINT32)ulAntennaType, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_GetSatCountryIdx (HxCountry_e *peCountryId)
{
	HERROR			hErr;
	HxCountry_e		eCountryId;

	if (peCountryId == NULL)				{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID, (HUINT32 *)&eCountryId, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID) Error!!\n");
	}

	if(FALSE == HLIB_COUNTRY_IsValidCountryId(eCountryId))
	{
		eCountryId = eCountryID_DEU;
	}

	*peCountryId = eCountryId;

	return ERR_BUS_NO_ERROR;
}


HERROR MGR_SEARCHUTIL_GetMotorLnbFreqFromDb (HUINT32 *pulLnbFreq)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (pulLnbFreq == NULL)				{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_MOTORSET_LNBFREQ, &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_BUS_NO_OBJECT; }

	if (ulValue == 0)
	{
		// Default Value
		ulValue = CH_UNIVERSAL_LNB_FREQ;
	}

	*pulLnbFreq = ulValue;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetMotorLnbFreqToDb (HUINT32 ulLnbFreq)
{
	HERROR	hErr;

	if (ulLnbFreq == 0)
	{
		// Default Value
		ulLnbFreq = CH_UNIVERSAL_LNB_FREQ;
	}

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_MOTORSET_LNBFREQ, ulLnbFreq, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_GetMotorLnbPowerFromDb (HBOOL *pbLnbPower)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (pbLnbPower == NULL)				{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_MOTORSET_LNBPOWER, &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_BUS_NO_OBJECT; }

	*pbLnbPower = (HBOOL)ulValue;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetMotorLnbPowerToDb (HBOOL bLnbPower)
{
	HERROR	hErr;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_MOTORSET_LNBPOWER, (HUINT32)bLnbPower, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_GetMotorTypeFromDb (DxSAT_DiseqcVer_e *peMotorType)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (peMotorType == NULL)				{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_MOTORSET_MOTORTYPE, &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_BUS_NO_OBJECT; }

	if (ulValue != eDxDISEQC_VER_1_2 && ulValue != eDxDISEQC_VER_1_3)
	{
		// Default Value
		ulValue = eDxDISEQC_VER_1_2;
	}

	*peMotorType = (DxSAT_DiseqcVer_e)ulValue;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetMotorTypeToDb (DxSAT_DiseqcVer_e eMotorType)
{
	HERROR	hErr;

	if (eMotorType != eDxDISEQC_VER_1_2 && eMotorType != eDxDISEQC_VER_1_3)
	{
		// Default Value
		eMotorType = eDxDISEQC_VER_1_2;
	}

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_MOTORSET_MOTORTYPE, (HUINT32)eMotorType, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_GetMotorLongitudeFromDb (HINT32 *pnLongitude)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (pnLongitude == NULL)				{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_MY_LONGITUDE, &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_BUS_NO_OBJECT; }

	*pnLongitude = ulValue;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetMotorLongitudeToDb (HINT32 nLongitude)
{
	HERROR	hErr;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_MY_LONGITUDE, (HUINT32)nLongitude, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_GetMotorLatitudeFromDb (HINT32 *pnLatitude)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (pnLatitude == NULL)				{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_MY_LATITUDE, &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_BUS_NO_OBJECT; }

	*pnLatitude = (HINT32)ulValue;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetMotorLatitudeToDb (HINT32 nLatitude)
{
	HERROR	hErr;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_MY_LATITUDE, (HUINT32)nLatitude, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_GetPositionSettingSatTypeFromDb (SVC_SatType_t *peSatType)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (peSatType == NULL)				{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_POSITIONSET_SATTYPE, &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_BUS_NO_OBJECT; }

	if (ulValue == eSatType_ALL || ulValue > MAX_NUM_DEFAULT_ANTENNAS)
	{
		ulValue = eSatType_ASTRA_1;
	}

	*peSatType = (SVC_SatType_t)ulValue;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetPositionSettingSatTypeToDb (SVC_SatType_t eSatType)
{
	HERROR	hErr;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_POSITIONSET_SATTYPE, (HUINT32)eSatType, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_GetPositionSettingMotorDriveFromDb (CH_MotorDriveMode_t *peMotorDrive)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (peMotorDrive == NULL)				{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_POSITIONSET_MOTORDRIVE, &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_BUS_NO_OBJECT; }

	*peMotorDrive = (CH_MotorDriveMode_t)ulValue;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetPositionSettingMotorDriveToDb (CH_MotorDriveMode_t eMotorDrive)
{
	HERROR	hErr;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_POSITIONSET_MOTORDRIVE, (HUINT32)eMotorDrive, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_GetPositionSettingDriveSizeFromDb (HUINT32 *pulDriveSize)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (pulDriveSize == NULL)				{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_POSITIONSET_DRIVESIZE, &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_BUS_NO_OBJECT; }

	if (ulValue <= 0)							{ return ERR_BUS_NO_OBJECT; }

	*pulDriveSize = ulValue;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetPositionSettingDriveSizeToDb (HUINT32 ulDriveSize)
{
	HERROR	hErr;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_POSITIONSET_DRIVESIZE, ulDriveSize, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_GetLimitSettingMotorLimitFromDb (HBOOL *pbLimitOnOff)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (pbLimitOnOff == NULL)				{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_LIMITSET_MOTORLIMIT, &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_BUS_NO_OBJECT; }

	if (ulValue != (HUINT32)TRUE && ulValue != (HUINT32)FALSE)
	{
		ulValue = (HUINT32)FALSE;
	}

	*pbLimitOnOff = (HBOOL)ulValue;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetLimitSettingMotorLimitToDb (HBOOL bLimitOnOff)
{
	HERROR	hErr;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_LIMITSET_MOTORLIMIT, (HUINT32)bLimitOnOff, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_GetLimitSettingLimitPositionFromDb (CH_MotorDirection_t *peMotorDir)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (peMotorDir == NULL)				{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_LIMITSET_LIMITPOS, &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_BUS_NO_OBJECT; }

	if (ulValue != eMOTOR_DIR_EAST && ulValue != eMOTOR_DIR_WEST)
	{
		ulValue = eMOTOR_DIR_EAST;
	}

	*peMotorDir = (CH_MotorDirection_t)ulValue;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetLimitSettingLimitPositionToDb (HBOOL bLimitOnOff)
{
	HERROR	hErr;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_LIMITSET_LIMITPOS, (HUINT32)bLimitOnOff, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_BUS_NO_ERROR;
}

#endif //CONFIG_MW_CH_SATELLITE


#ifdef CONFIG_MW_CH_TERRESTRIAL
HERROR MGR_SEARCHUTIL_GetTerTunerId (HUINT32 *pulTunerId)
{
	if (pulTunerId == NULL)					{ return ERR_BUS_TARGET_NULL; }

	if (s_bSrchVal_TerTunerId_Available == FALSE)
	{
		return ERR_BUS_NO_OBJECT;
	}

	*pulTunerId = s_ulSrchVal_TerTunerId;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetTerTunerId (HUINT32 ulTunerId)
{
	s_bSrchVal_TerTunerId_Available = TRUE;
	s_ulSrchVal_TerTunerId = ulTunerId;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_GetTerSearchType (HUINT32 *pulSrchType)
{
	if (pulSrchType == NULL)					{ return ERR_BUS_TARGET_NULL; }

	if (s_bSrchVal_TerSrchType_Available == FALSE)
	{
		return ERR_BUS_NO_OBJECT;
	}

	*pulSrchType = s_ulSrchVal_TerSrchType;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetTerSearchType (HUINT32 ulSrchType)
{
	s_bSrchVal_TerSrchType_Available = TRUE;
	s_ulSrchVal_TerSrchType = ulSrchType;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_GetUserTerTpInfo (DbSvc_TuningInfo_t *pstTuneInfo)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (pstTuneInfo == NULL)				{ return ERR_BUS_SOURCE_NULL; }

	HxSTD_memset (pstTuneInfo, 0, sizeof(DbSvc_TuningInfo_t));

	// Channel No.
	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_TER_SRCH_CHANNEL, &ulValue, 0);
	pstTuneInfo->ter.ucChanNum = (hErr == ERR_OK) ? (HUINT8)ulValue : (HUINT8)21;

	// Frequency
	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_TER_SRCH_FREQUENCY, &ulValue, 0);
	pstTuneInfo->ter.ulFreq = (hErr == ERR_OK) ? (HUINT32)ulValue : 0;

#if defined(CONFIG_MW_CH_DVB_T2)
	// Transmission
	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_TER_SRCH_TRANSMISSION, &ulValue, 0);
	pstTuneInfo->ter.ucSystem = (hErr == ERR_OK) ? (HUINT8)ulValue : (HUINT8)0;
#endif

	// Bandwidth
	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_TER_SRCH_BANDWIDTH, &ulValue, 0);
	pstTuneInfo->ter.ucBandwidth = (hErr == ERR_OK) ? (HUINT8)ulValue : (HUINT8)eDxTER_BANDWIDTH_8M;

	// The other variables can be set automatically.
	pstTuneInfo->ter.ucCodeRate			= eDxTER_CODERATE_AUTO;
	pstTuneInfo->ter.ucConstellation	= eDxTER_CONSTELLATION_AUTO;
	pstTuneInfo->ter.ucHierachy			= eDxTER_HIERACHY_AUTO;
	pstTuneInfo->ter.ucOffset			= eDxTER_OFFSET_AUTO;
	pstTuneInfo->ter.ucStream			= eDxTER_STREAM_HP;
	pstTuneInfo->ter.ucTransMode		= eDxTER_TRANSMODE_AUTO;
	pstTuneInfo->ter.ucGuardInterval	= eDxTER_GUARDINTERVAL_AUTO;

	return ERR_BUS_NO_ERROR;
}

//  ==== Terrestrial ==== //

HERROR MGR_SEARCHUTIL_SetUserTerTpInfo (DbSvc_TuningInfo_t *pstTuneInfo)
{
	HUINT32	ulValue;

	if (pstTuneInfo == NULL)				{ return ERR_BUS_SOURCE_NULL; }

	// Channel No.
	ulValue = (HUINT32)pstTuneInfo->ter.ucChanNum;
	DB_PARAM_SetItem (eDB_PARAM_ITEM_TER_SRCH_CHANNEL, ulValue, 0);

	// Frequency
	ulValue = (HUINT32)pstTuneInfo->ter.ulFreq;
	DB_PARAM_SetItem (eDB_PARAM_ITEM_TER_SRCH_FREQUENCY, ulValue, 0);

#if defined(CONFIG_MW_CH_DVB_T2)
	// Transmission
	ulValue = (HUINT32)pstTuneInfo->ter.ucSystem;
	DB_PARAM_SetItem (eDB_PARAM_ITEM_TER_SRCH_TRANSMISSION, ulValue, 0);
#endif
	// Bandwidth
	ulValue = (HUINT32)pstTuneInfo->ter.ucBandwidth;
	DB_PARAM_SetItem (eDB_PARAM_ITEM_TER_SRCH_BANDWIDTH, ulValue, 0);

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_GetTerNetworkSearchFromDb (HUINT32 *pulNetwork)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (pulNetwork == NULL)					{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_TER_SRCH_NETSEARCH, &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_BUS_NO_OBJECT; }

	*pulNetwork = (ulValue != 0) ? eSat_NetSearch_Enable : eSat_NetSearch_Disable;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetTerNetworkSearchToDb (HUINT32 ulNetwork)
{
	HERROR	hErr;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_TER_SRCH_NETSEARCH, ulNetwork, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_GetTerNetworkIdFromDb (HUINT32 *pulNetId)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (pulNetId == NULL)					{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_TER_SRCH_NETWORKID, &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_BUS_NO_OBJECT; }

	if (ulValue == 0 || ulValue == HANDLE_NULL)
	{
		*pulNetId = HANDLE_NULL;
		return ERR_BUS_NO_ERROR;
	}

	*pulNetId = ulValue;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetTerNetworkIdToDb (HUINT32 ulNetId)
{
	HERROR	hErr;

	if (ulNetId == 0)			{ ulNetId = HANDLE_NULL; }

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_TER_SRCH_NETWORKID, ulNetId, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_GetTerCountryIdx (HxCountry_e *peCountryId)
{
	HERROR			hErr;
	HxCountry_e		eCountryId;

	if (peCountryId == NULL)				{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID, (HUINT32 *)&eCountryId, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID) Error!!\n");
	}

	if(FALSE == HLIB_COUNTRY_IsValidCountryId(eCountryId))
	{
		eCountryId = eCountryID_ITA;
	}

	*peCountryId = eCountryId;

	return ERR_BUS_NO_ERROR;
}

#endif //CONFIG_MW_CH_TERRESTRIAL


#ifdef CONFIG_MW_CH_CABLE
HERROR MGR_SEARCHUTIL_GetCabTunerIdFromDb (HUINT32 *pulTunerId)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (pulTunerId == NULL)					{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_CAB_SRCH_TUNERID, &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_BUS_NO_OBJECT; }

	*pulTunerId = (HUINT32)ulValue;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetCabTunerIdToDb (HUINT32 ulTunerId)
{
	HERROR	hErr;

	if (ulTunerId >= MAX_CAB_TUNER)			{ return ERR_BUS_OUT_OF_RANGE; }

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_CAB_SRCH_TUNERID, ulTunerId, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_GetCabFreqSearchFromDb (HUINT32 *pulFreq)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (pulFreq == NULL)					{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_CAB_SRCH_FREQUENCY, &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_BUS_NO_OBJECT; }

	*pulFreq = (HUINT32)ulValue;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetCabFreqSearchToDb (HUINT32 ulFreq)
{
	HERROR	hErr;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_CAB_SRCH_FREQUENCY, ulFreq, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_GetCabSymbolRateSearchFromDb (HUINT32 *pulSR)
{
	HERROR	hErr;
	HINT32	ulValue;

	if (pulSR == NULL)					{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_CAB_SRCH_SYMBOL_RATE, &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_BUS_NO_OBJECT; }

	*pulSR = (HUINT32)ulValue;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetCabSymbolRateSearchToDb (HUINT32 ulSR)
{
	HERROR	hErr;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_CAB_SRCH_SYMBOL_RATE, ulSR, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_GetCabModulationSearchFromDb (HUINT32 *pulCon)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (pulCon == NULL)					{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_CAB_SRCH_MODULATION, &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_BUS_NO_OBJECT; }

	*pulCon = (HUINT32)ulValue;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetCabModulationSearchToDb (HUINT32 ulCon)
{
	HERROR	hErr;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_CAB_SRCH_MODULATION, ulCon, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_BUS_NO_ERROR;
}


#define __CAB_LINEAR_SEARCH__

HERROR MGR_SEARCHUTIL_GetCabLinearStartFreqSearchFromDb (HUINT32 *pulFreq)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (pulFreq == NULL)					{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_CAB_LINEAR_SRCH_STARTFREQUENCY, &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_BUS_NO_OBJECT; }

	*pulFreq = (HUINT32)ulValue;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetCabLinearStartFreqSearchToDb (HUINT32 ulFreq)
{
	HERROR	hErr;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_CAB_LINEAR_SRCH_STARTFREQUENCY, ulFreq, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_GetCabLinearStopFreqSearchFromDb (HUINT32 *pulFreq)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (pulFreq == NULL)					{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_CAB_LINEAR_SRCH_STOPFREQUENCY, &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_BUS_NO_OBJECT; }

	*pulFreq = (HUINT32)ulValue;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetCabLinearStopFreqSearchToDb (HUINT32 ulFreq)
{
	HERROR	hErr;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_CAB_LINEAR_SRCH_STOPFREQUENCY, ulFreq, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_GetCabLinearStepSearchFromDb (HUINT32 *pulStep)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (pulStep == NULL)					{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_CAB_LINEAR_SRCH_STEP, &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_BUS_NO_OBJECT; }

	*pulStep = (HUINT32)ulValue;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetCabLinearStepSearchToDb (HUINT32 ulStep)
{
	HERROR	hErr;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_CAB_LINEAR_SRCH_STEP, ulStep, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_GetCabLinearModulationSearchFromDb (HUINT32 *pulCon)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (pulCon == NULL)					{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_CAB_LINEAR_SRCH_MODULATION, &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_BUS_NO_OBJECT; }

	*pulCon = ulValue;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetCabLinearModulationSearchToDb (HUINT32 ulCon)
{
	HERROR	hErr;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_CAB_LINEAR_SRCH_MODULATION, ulCon, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_GetCabLinearSymbolRateSearchFromDb (HUINT32 *pulSR)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (pulSR == NULL)					{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_CAB_LINEAR_SRCH_SYMBOL_RATE, &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_BUS_NO_OBJECT; }

	*pulSR = ulValue;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetCabLinearSymbolRateSearchToDb (HUINT32 ulSR)
{
	HERROR	hErr;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_CAB_LINEAR_SRCH_SYMBOL_RATE, ulSR, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_BUS_NO_ERROR;
}

#define __COMMON__

HERROR MGR_SEARCHUTIL_GetCabNetworkSearchFromDb (HUINT32 *pulNetwork)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (pulNetwork == NULL)					{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_CAB_SRCH_NETSEARCH, &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_BUS_NO_OBJECT; }

	*pulNetwork = (ulValue != 0) ? (HUINT32)eSat_NetSearch_Enable : (HUINT32)eSat_NetSearch_Disable;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetCabNetworkSearchToDb (HUINT32 ulNetwork)
{
	HERROR	hErr;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_CAB_SRCH_NETSEARCH, ulNetwork, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_GetCabNetworkIdFromDb (HUINT32 *pulNetworkId)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (pulNetworkId == NULL)					{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_CAB_SRCH_NETWORK_ID, &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_BUS_NO_OBJECT; }

	*pulNetworkId = ulValue;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetCabNetworkIdToDb (HUINT32 ulNetworkId)
{
	HERROR	hErr;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_CAB_SRCH_NETWORK_ID, ulNetworkId, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_GetCabScrambleTypeFromDb (HUINT32 *pulCasType)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (pulCasType == NULL)					{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_CAB_SRCH_CAFREE_TYPE, &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_BUS_NO_OBJECT; }

	*pulCasType = ulValue;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetCabScrambleTypeToDb (HUINT32 ulCasType)
{
	HERROR	hErr;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_CAB_SRCH_CAFREE_TYPE, ulCasType, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_GetCabChannelTypeFromDb (HUINT32 *pulChType)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (pulChType == NULL)					{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_CAB_SRCH_CHANNEL_TYPE, &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_BUS_NO_OBJECT; }

	*pulChType = ulValue;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetCabChannelTypeToDb (HUINT32 ulChType)
{
	HERROR	hErr;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_CAB_SRCH_CHANNEL_TYPE, ulChType, 0);
	if (hErr != ERR_OK) 					{ return ERR_FAIL; }

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_GetCabSearchModeFromDb (HUINT32 *pulSearchMode)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (pulSearchMode == NULL)					{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_CABLE_SEARCH_MODE, &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_BUS_NO_OBJECT; }

	*pulSearchMode = ulValue;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetCabSearchModeFromDb (HUINT32 ulSearchMode)
{
	HERROR	hErr;

	hErr = DB_PARAM_SetItem(eDB_PARAM_ITEM_CABLE_SEARCH_MODE, ulSearchMode, 0);
	if (hErr != ERR_OK)						{ return ERR_FAIL; }

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_GetCabNetworkSrchModeFromDb (HUINT32 *pulSearchMode)
{
	HERROR	hErr;
	HUINT32	ulValue;

	if (pulSearchMode == NULL)					{ return ERR_BUS_TARGET_NULL; }

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_CAB_SRCH_NETWORK_SRCH_MODE, &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_BUS_NO_OBJECT; }

	*pulSearchMode = ulValue;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_SetCabNetworkSrchModeToDb (HUINT32 ulSearchMode)
{
	HERROR	hErr;

	hErr = DB_PARAM_SetItem(eDB_PARAM_ITEM_CAB_SRCH_NETWORK_SRCH_MODE, ulSearchMode, 0);
	if (hErr != ERR_OK)						{ return ERR_FAIL; }

	return ERR_BUS_NO_ERROR;
}

#endif //CONFIG_MW_CH_CABLE


#ifdef CONFIG_MW_CH_SATELLITE
HERROR MGR_SEARCHUTIL_ConvertSatSvcTuningInfoToChTuningParam (HUINT32 ulTunerId,
															DbSvc_AntInfo_t *pstAntInfo,
															DbSvc_TuningInfo_t *pstSvcTuneInfo,
															DxTuneParam_t *pstChTuneInfo)
{
	if (pstAntInfo == NULL || pstSvcTuneInfo == NULL)
	{
		return ERR_BUS_SOURCE_NULL;
	}

	if (pstChTuneInfo == NULL)
	{
		return ERR_BUS_TARGET_NULL;
	}

	HxSTD_memset (pstChTuneInfo, 0, sizeof(DxTuneParam_t));

	pstChTuneInfo->sat.deliveryType	 = eDxDELIVERY_TYPE_SAT;

	pstChTuneInfo->sat.ulTunerGroupId	= ulTunerId;
	pstChTuneInfo->sat.ulRfInputId		= (HUINT32)-1;

	pstChTuneInfo->sat.tuningInfo.ulFrequency	 = pstSvcTuneInfo->sat.ulFrequency;
	pstChTuneInfo->sat.tuningInfo.ulSymbolRate	 = pstSvcTuneInfo->sat.ulSymbolRate;
	pstChTuneInfo->sat.tuningInfo.eFecCodeRate	 = pstSvcTuneInfo->sat.eFecCodeRate;
	pstChTuneInfo->sat.tuningInfo.ePolarization  = pstSvcTuneInfo->sat.ePolarization;

	pstChTuneInfo->sat.tuningInfo.eTransSpec     = pstSvcTuneInfo->sat.eTransSpec;
	pstChTuneInfo->sat.tuningInfo.ePskMod		 = pstSvcTuneInfo->sat.ePskMod;
	pstChTuneInfo->sat.tuningInfo.ePilot		 = pstSvcTuneInfo->sat.ePilot;

	pstChTuneInfo->sat.antennaType					= pstAntInfo->eAntennaType;

	switch (pstAntInfo->eAntennaType)
	{
	case eDxANT_TYPE_LNB_ONLY:
		pstChTuneInfo->sat.antInfo.info.lnb.ulLnbFreq		= pstAntInfo->utAntInfo.stLnbAnt.ulLnbFreq;
		pstChTuneInfo->sat.antInfo.info.lnb.lnbVoltage		= pstAntInfo->utAntInfo.stLnbAnt.eLnbVoltage;
		break;

	case eDxANT_TYPE_DISEQC:
		pstChTuneInfo->sat.antInfo.info.diseqc.ulLnbFreq		 = pstAntInfo->utAntInfo.stDiseqcAnt.ulLnbFreq;
		pstChTuneInfo->sat.antInfo.info.diseqc.lnbVoltage	 = pstAntInfo->utAntInfo.stDiseqcAnt.eLnbVoltage;
		pstChTuneInfo->sat.antInfo.info.diseqc.b22kTone		 = pstAntInfo->utAntInfo.stDiseqcAnt.b22kTone;

		pstChTuneInfo->sat.antInfo.info.diseqc.diseqcVersion	 = pstAntInfo->utAntInfo.stDiseqcAnt.eDiseqcVer;
		pstChTuneInfo->sat.antInfo.info.diseqc.diseqcInput	 = pstAntInfo->utAntInfo.stDiseqcAnt.eDiseqcInput;
		pstChTuneInfo->sat.antInfo.info.diseqc.toneBurst		 = pstAntInfo->utAntInfo.stDiseqcAnt.eToneBurst;

		pstChTuneInfo->sat.antInfo.info.diseqc.ucMotorPosition = pstAntInfo->utAntInfo.stDiseqcAnt.ucMotorPosition;
		pstChTuneInfo->sat.antInfo.info.diseqc.sSatLonggitude = pstAntInfo->utAntInfo.stDiseqcAnt.sSatLonggitude;
		break;

	case eDxANT_TYPE_SCD:
		pstChTuneInfo->sat.antInfo.info.scd.ulLnbFreq			= pstAntInfo->utAntInfo.stScdAnt.ulLnbFreq;
		pstChTuneInfo->sat.antInfo.info.scd.lnbVoltage			= pstAntInfo->utAntInfo.stScdAnt.eLnbVoltage;
		pstChTuneInfo->sat.antInfo.info.scd.b22kTone				= pstAntInfo->utAntInfo.stScdAnt.b22kTone;

		pstChTuneInfo->sat.antInfo.info.scd.scdInput			 	= pstAntInfo->utAntInfo.stScdAnt.eScdInput;
		pstChTuneInfo->sat.antInfo.info.scd.ulScdUserBandNum	 	= pstAntInfo->utAntInfo.stScdAnt.ulScdUserBandNum;
		pstChTuneInfo->sat.antInfo.info.scd.ulScdUserBandFreq	= pstAntInfo->utAntInfo.stScdAnt.ulScdUserBandFreq;
#if !defined(CONFIG_APUI_SAT_1TUNER)
		pstChTuneInfo->sat.antInfo.info.scd.ulScdUserBandNum1	= pstAntInfo->utAntInfo.stScdAnt.ulScdUserBandNum1;
		pstChTuneInfo->sat.antInfo.info.scd.ulScdUserBandFreq1	= pstAntInfo->utAntInfo.stScdAnt.ulScdUserBandFreq1;
#endif
		break;

	default:
		return ERR_FAIL;
	}


	return ERR_BUS_NO_ERROR;
}

#endif //CONFIG_MW_CH_SATELLITE


HERROR MGR_SEARCHUTIL_ConvertChTuningParamToSvcTuningInfo (DxTuneParam_t *pstChTuneInfo,
																		DbSvc_TuningInfo_t *pstSvcTuneInfo)
{
	if (pstChTuneInfo == NULL)
	{
		return ERR_BUS_SOURCE_NULL;
	}

	if (pstSvcTuneInfo == NULL)
	{
		return ERR_BUS_TARGET_NULL;
	}

	HxSTD_memset (pstSvcTuneInfo, 0, sizeof(DbSvc_TuningInfo_t));

	switch (pstChTuneInfo->eDeliType)
	{
		case  eDxDELIVERY_TYPE_SAT:
			pstSvcTuneInfo->sat.ulFrequency			= pstChTuneInfo->sat.tuningInfo.ulFrequency;
			pstSvcTuneInfo->sat.ulSymbolRate		= pstChTuneInfo->sat.tuningInfo.ulSymbolRate;
			pstSvcTuneInfo->sat.ePolarization		= pstChTuneInfo->sat.tuningInfo.ePolarization;
			pstSvcTuneInfo->sat.eFecCodeRate		= pstChTuneInfo->sat.tuningInfo.eFecCodeRate;
			pstSvcTuneInfo->sat.eTransSpec			= pstChTuneInfo->sat.tuningInfo.eTransSpec;
			pstSvcTuneInfo->sat.ePskMod				= pstChTuneInfo->sat.tuningInfo.ePskMod;
			pstSvcTuneInfo->sat.ePilot				= pstChTuneInfo->sat.tuningInfo.ePilot;
			pstSvcTuneInfo->sat.eRollOff			= pstChTuneInfo->sat.tuningInfo.eRollOff;
			break;
		case eDxDELIVERY_TYPE_TER:
			pstSvcTuneInfo->ter.ulFreq			= (HUINT32)pstChTuneInfo->ter.ulFrequency;
			pstSvcTuneInfo->ter.ucBandwidth		= (HUINT8)pstChTuneInfo->ter.eBandWidth;
			pstSvcTuneInfo->ter.ucConstellation	= (HUINT8)pstChTuneInfo->ter.eConstellation;
			pstSvcTuneInfo->ter.ucHierachy		= (HUINT8)pstChTuneInfo->ter.eHierachy;
			pstSvcTuneInfo->ter.ucCodeRate		= (HUINT8)pstChTuneInfo->ter.eCodeRate;
			pstSvcTuneInfo->ter.ucTransMode		= (HUINT8)pstChTuneInfo->ter.eTransMode;
			pstSvcTuneInfo->ter.ucGuardInterval	= (HUINT8)pstChTuneInfo->ter.eGuardInterval;
			pstSvcTuneInfo->ter.ucStream		= (HUINT8)pstChTuneInfo->ter.eStream;
			pstSvcTuneInfo->ter.ucOffset		= (HUINT8)pstChTuneInfo->ter.eOffset;
#if defined(CONFIG_MW_CH_DVB_T2)
			pstSvcTuneInfo->ter.ucSystem					= (HUINT8)pstChTuneInfo->ter.eSystem;
			pstSvcTuneInfo->ter.stT2param.ulPlpId			= (HUINT8)pstChTuneInfo->ter.stT2Param.ulPlpId;
			pstSvcTuneInfo->ter.stT2param.ucPreambleFormat	= (HUINT8)pstChTuneInfo->ter.stT2Param.preambleFormat;
			pstSvcTuneInfo->ter.stT2param.ucMixed			= (HUINT8)pstChTuneInfo->ter.stT2Param.mixed;
			pstSvcTuneInfo->ter.stT2param.ucPilotPattern	= (HUINT8)pstChTuneInfo->ter.stT2Param.pilotPattern;
			pstSvcTuneInfo->ter.stT2param.ucExtenedCarrier	= (HUINT8)pstChTuneInfo->ter.stT2Param.extenedCarrier;
			pstSvcTuneInfo->ter.stT2param.ucPAPRreduction	= (HUINT8)pstChTuneInfo->ter.stT2Param.PAPRreduction;
			pstSvcTuneInfo->ter.stT2param.ulNumPlpBlocks	= (HUINT8)pstChTuneInfo->ter.stT2Param.numPlpBlocks;
#endif
			break;
		case eDxDELIVERY_TYPE_CAB:
			pstSvcTuneInfo->cab.ulFreq			= (HUINT32)pstChTuneInfo->cab.ulFrequency;
			pstSvcTuneInfo->cab.usSR			= (HUINT16)pstChTuneInfo->cab.ulSymbolRate;
			pstSvcTuneInfo->cab.ucCon			= (HUINT8)pstChTuneInfo->cab.constellation;
			pstSvcTuneInfo->cab.ucSpec			= (HUINT8)pstChTuneInfo->cab.spectrum;
			break;
		default:
			return ERR_FAIL;
			break;
	}

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHUTIL_MakeDbNetInfoFromSi(Handle_t hAction, HUINT16 usNetworkId, DbSvc_NetInfo_t *pstDbNetworkInfo)
{
#if	1		//	function is not working --> just comments it
	return ERR_OK;
#else
	HERROR			hError;
	HUINT32			ulNetworkNum;
	HUINT32			ulCnt;
	DbSvc_NetInfo_t	*astNetworkArray;
	HBOOL			bFound;

	if (pstDbNetworkInfo == NULL)
	{
		//HxLOG_Print("[%s (%04d)] pstDbNetworkInfo == NULL\n", __FILE__, __LINE__);
		return ERR_FAIL;
	}

	// Get Network Info
	ulNetworkNum = 0;
//	hError = MW_SI_GetNumNetInfo (hAction, &ulNetworkNum);
	if (hError != ERR_OK || ulNetworkNum == 0)
	{
		//HxLOG_Print("[%s (%04d)] hError != ERR_OK || ulNetworkNum == 0\n", __FILE__, __LINE__);
		return ERR_FAIL;
	}

	//HxLOG_Print("[%s (%04d)] usNetworkId [%d], MW_SI_GetNumNetInfo() [%d]\n", __FILE__, __LINE__, usNetworkId, ulNetworkNum);

	astNetworkArray = (DbSvc_NetInfo_t *) OxAP_Malloc (sizeof (DbSvc_NetInfo_t) * ulNetworkNum);
	if (astNetworkArray == NULL)
	{
		//HxLOG_Print("[%s (%04d)] astNetworkArray == NULL\n", __FILE__, __LINE__);
		return ERR_FAIL;
	}

//	hError = MW_SI_GetNetworkInfo (hAction, astNetworkArray, ulNetworkNum);
	if (hError != ERR_OK)
	{
		//HxLOG_Print("[%s (%04d)] hError != ERR_OK\n", __FILE__, __LINE__);
		OxAP_Free(astNetworkArray);
		return ERR_FAIL;
	}

	bFound = FALSE;
	for (ulCnt = 0; ulCnt < ulNetworkNum; ulCnt++)
	{
		//HxLOG_Print("[%s (%04d)] astNetworkArray[%d].usNetId [%d]\n", __FILE__, __LINE__, ulCnt, astNetworkArray[ulCnt].usNetId, usNetworkId);
		if (astNetworkArray[ulCnt].usNetId == usNetworkId)
		{
			HxSTD_memcpy(pstDbNetworkInfo, &astNetworkArray[ulCnt], sizeof(DbSvc_NetInfo_t));
			bFound = TRUE;
		}
	}

	OxAP_Free(astNetworkArray);

	//HxLOG_Print("[%s (%04d)] bFound [%d]\n", __FILE__, __LINE__, bFound);

	return (bFound) ? ERR_OK : ERR_FAIL;
#endif
}


// =======================================================================================
// TP 선택 정보를 위한 함수
// =======================================================================================

#define MAX_TPSELINFO_NUM			8

static HBOOL		 s_bUserTpInfoInMemory = FALSE;
static MgrSearch_TpSelectInfo_t	 s_astUserTpSelectInfo[MAX_TPSELINFO_NUM];

static HERROR local_srchutil_LoadTpSelectionInfoOnce (void)
{
	HERROR		 hErr;

	if (s_bUserTpInfoInMemory == FALSE)
	{
		s_bUserTpInfoInMemory = TRUE;
		HxSTD_memset (s_astUserTpSelectInfo, 0, sizeof(MgrSearch_TpSelectInfo_t) * MAX_TPSELINFO_NUM);

		hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_SRCH_TPSELECTION, (HUINT32 *)s_astUserTpSelectInfo, sizeof(MgrSearch_TpSelectInfo_t) * MAX_TPSELINFO_NUM);
		if (hErr != ERR_OK)
		{
			return ERR_FAIL;
		}

	}

	return ERR_OK;
}

HERROR MGR_SEARCHUTIL_GetTpSelectionInfo (HUINT32 ulAntIdx, MgrSearch_TpSelectInfo_t *pstTpSelectInfo)
{
	HINT32		 nIndex;

	// Load 실패하면 Clean 상태로 시작하면 된다.
	local_srchutil_LoadTpSelectionInfoOnce();

	if (pstTpSelectInfo == NULL)					{ return ERR_FAIL; }

	for (nIndex = 0; nIndex < MAX_TPSELINFO_NUM; nIndex++)
	{
		if (s_astUserTpSelectInfo[nIndex].ucAvailable == TRUE && s_astUserTpSelectInfo[nIndex].ulAntIdx == ulAntIdx)
		{
			*pstTpSelectInfo = s_astUserTpSelectInfo[nIndex];
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

HERROR MGR_SEARCHUTIL_ResetTpSelectionInfo (void)
{
	// Load 실패하면 Clean 상태로 시작하면 된다.
	local_srchutil_LoadTpSelectionInfoOnce();

	HxSTD_memset (s_astUserTpSelectInfo, 0, sizeof(MgrSearch_TpSelectInfo_t) * MAX_TPSELINFO_NUM);
	return ERR_OK;
}

HERROR MGR_SEARCHUTIL_UpdateTpSelectionInfo (MgrSearch_TpSelectInfo_t *pstTpSelectInfo)
{
	HINT32		 nIndex, nEmptySlot;
	HINT32		 nAntCnt = 0, nAntNum = 0;
	HINT32		*pnAntIdxList = NULL;
	HERROR		 hErr;

	// Load 실패하면 Clean 상태로 시작하면 된다.
	local_srchutil_LoadTpSelectionInfoOnce();

	if (pstTpSelectInfo == NULL)					{ return ERR_FAIL; }

	nEmptySlot = MAX_TPSELINFO_NUM;

	for (nIndex = 0; nIndex < MAX_TPSELINFO_NUM; nIndex++)
	{
		if (s_astUserTpSelectInfo[nIndex].ucAvailable == TRUE && s_astUserTpSelectInfo[nIndex].ulAntIdx == pstTpSelectInfo->ulAntIdx)
		{
			s_astUserTpSelectInfo[nIndex] = *pstTpSelectInfo;
			return ERR_OK;
		}
		else if (s_astUserTpSelectInfo[nIndex].ucAvailable == FALSE)
		{
			nEmptySlot = nIndex;
		}
	}

	// 새 TpSelectInfo를 추가해야 하는데 빈 Slot이 없다면 비워야 한다.
	if (nEmptySlot == MAX_TPSELINFO_NUM)
	{
		hErr = DB_SVC_FindAllAntIdxList (&nAntNum, &pnAntIdxList);
		if (hErr != ERR_OK || nAntNum <= 0)
		{
		    DB_SVC_FreeAntList (pnAntIdxList);
		    return ERR_FAIL;
		}

		for (nIndex = 0; nIndex < MAX_TPSELINFO_NUM; nIndex++)
		{
			for (nAntCnt = 0; nAntCnt < nAntNum; nAntCnt++)
			{
				if (s_astUserTpSelectInfo[nIndex].ulAntIdx == (HUINT32)pnAntIdxList[nAntCnt])
				{
					break;
				}
			}

			// Tp Selection Info 에 지정된 Antenna Idx가 없다 : 지워진 Antenna
			if (nAntCnt >= nAntNum)
			{
				nEmptySlot = nIndex;
				HxSTD_memset (&(s_astUserTpSelectInfo[nIndex]), 0, sizeof(MgrSearch_TpSelectInfo_t));
			}
		}

		DB_SVC_FreeAntList (pnAntIdxList);

		if (nEmptySlot == MAX_TPSELINFO_NUM)
		{
			return ERR_FAIL;
		}
	}

	// 빈 Slot에 새로 추가한다.
	s_astUserTpSelectInfo[nEmptySlot] = *pstTpSelectInfo;
	s_astUserTpSelectInfo[nEmptySlot].ucAvailable = TRUE;

	return ERR_OK;
}

HERROR MGR_SEARCHUTIL_SyncTpSelectionInfo (void)
{
	HERROR	hErr;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_SRCH_TPSELECTION, (HUINT32)s_astUserTpSelectInfo, (HUINT32)sizeof(MgrSearch_TpSelectInfo_t) * MAX_TPSELINFO_NUM);
	if(hErr != ERR_OK) 	{	HxLOG_Critical("\n\n");	}
	DB_PARAM_Sync();

	return ERR_OK;
}

HBOOL MGR_SEARCHUTIL_CompareSearchedSvcType (DxSvcType_e eSvcTypeInOption, DxSvcType_e eSvcType)
{
	if (eSvcTypeInOption == eDxSVC_TYPE_All)
	{
		if (eSvcType == eDxSVC_TYPE_TV || eSvcType == eDxSVC_TYPE_RADIO || eSvcType == eDxSVC_TYPE_DATA || eSvcType == eDxSVC_TYPE_HIDDEN)
		{
			return TRUE;
		}
	}
	else if (eSvcTypeInOption == eDxSVC_TYPE_TV || eSvcTypeInOption == eDxSVC_TYPE_RADIO || eSvcTypeInOption == eDxSVC_TYPE_DATA|| eSvcType == eDxSVC_TYPE_HIDDEN )
	{
		if (eSvcType == eSvcTypeInOption)
		{
			return TRUE;
		}
	}

	return FALSE;
}

Handle_t MGR_SEARCHUTIL_MakeCurrentAppHandle (void)
{
	// Application Handle은 그 Application의 Callback function이다.
	return (Handle_t)BUS_MGR_GetMgrCallback(BUS_MGR_GetThis());
}



/* end of file */

