/**************************************************************************************/
/**
 * @file oplantinfo.c
 *
 * Defines the channel structure.
 *
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include "oplantinfo.h"

#include "oapi.h"
#include "hlib.h"
#include <apk.h>





HUINT32		OPL_ANTINFO_Count(void)
{
	HxLOG_Trace();

	return APK_META_ANTENNA_Count();
}

HINT32		OPL_ANTINFO_GetId(HINT32 nIndex)
{
	HxLOG_Trace();

	return APK_META_ANTENNA_GetUidAt (nIndex);
}

HINT32		OPL_ANTINFO_New(void)
{
	DxAntInfo_t	stAntInfo;

	HxSTD_MemSet(&stAntInfo, 0, sizeof(DxAntInfo_t));

	return APK_META_ANTENNA_Add(&stAntInfo);
}

HERROR	OPL_ANTINFO_RemoveAll(void)
{
	APK_META_ANTENNA_RemoveAll();

	return ERR_OK;
}

HINT32	OPL_ANTINFO_GetConnectionType(void)
{
	DxAntennaConnection_e	antConn;

	APK_SCENARIO_GetAntennaConnection(&antConn);

	return antConn;
}

HERROR	OPL_ANTINFO_SetConnectionType(HINT32 n)
{
#if 1
	fflush(NULL);
	printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");fflush(NULL);
	printf("\n\n\n\n\tSingle Connection Value is 0\n");fflush(NULL);
	printf("\tDual Same Connection Value is 1\n");fflush(NULL);
	printf("\n[%s:%d] Antenna Connection Type : %d\n\n\n\n",__FUNCTION__,__LINE__,n);fflush(NULL);
	printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");fflush(NULL);
#endif

	APK_SCENARIO_SetAntennaConnection(n);

	return ERR_OK;
}

HERROR		OPL_ANTINFO_GetInfo(HINT32 uid, OPL_AntennaInfo_t *pstAntInfo)
{
	HERROR				 hErr;
	DxAntInfo_t	*pstInfo;

	HxLOG_Trace();
	HxLOG_Assert(pstAntInfo);

	pstInfo = APK_META_ANTENNA_GetInfo(uid);
	if (pstInfo == NULL)
		return ERR_FAIL;

	HxLOG_Debug("pstInfo : antType : [%d] satType : [%s] satName : [%s]\n", pstInfo->antType, pstInfo->satType, pstInfo->satName);
	HxSTD_MemSet(pstAntInfo, 0, sizeof(OPL_AntennaInfo_t));

	pstAntInfo->eAntennaType		= pstInfo->antType;
	//	alloc하여야 하나 윗단에서 쓰고 버리는 조건으로 그냥 pointer만 매핑한다.
	HxSTD_StrNCpy(pstAntInfo->satelliteType, pstInfo->satType, (OPL_ANTINFO_NAME_LEN - 1));
	HxSTD_StrNCpy(pstAntInfo->satelliteName, pstInfo->satName, (OPL_ANTINFO_NAME_LEN - 1));
	pstAntInfo->satelliteName[OPL_ANTINFO_NAME_LEN - 1] = 0;
	pstAntInfo->satelliteType[OPL_ANTINFO_NAME_LEN - 1] = 0;

	hErr = ERR_OK;
	switch (pstInfo->antType)
	{
		case	eDxANT_TYPE_LNB_ONLY:
			pstAntInfo->lnb_freq	= (int)pstInfo->info.lnb.ulLnbFreq;
			pstAntInfo->lnb_voltage = pstInfo->info.lnb.lnbVoltage;
			break;
		case	eDxANT_TYPE_DISEQC:
			pstAntInfo->lnb_freq		= (int)pstInfo->info.diseqc.ulLnbFreq;
			pstAntInfo->lnb_voltage 	= pstInfo->info.diseqc.lnbVoltage;
			pstAntInfo->diseqc_version	= pstInfo->info.diseqc.diseqcVersion;
			pstAntInfo->switch_input	= pstInfo->info.diseqc.diseqcInput;
			pstAntInfo->enable_22khztone= pstInfo->info.diseqc.b22kTone;
			pstAntInfo->tone_burst		= pstInfo->info.diseqc.toneBurst;
			pstAntInfo->position		= pstInfo->info.diseqc.ucMotorPosition;
			pstAntInfo->longitude		= pstInfo->info.diseqc.sSatLonggitude;
			break;
		case	eDxANT_TYPE_SCD:
			pstAntInfo->lnb_freq		= (int)pstInfo->info.scd.ulLnbFreq;
			pstAntInfo->lnb_voltage 	= pstInfo->info.scd.lnbVoltage;
			pstAntInfo->enable_22khztone= pstInfo->info.scd.b22kTone;
			pstAntInfo->switch_input	= pstInfo->info.scd.scdInput;
			pstAntInfo->scd_band_num[0] = pstInfo->info.scd.ulScdUserBandNum;
			pstAntInfo->scd_band_num[1] = pstInfo->info.scd.ulScdUserBandNum1;// MOON_SCD
			pstAntInfo->scd_band_num[2] = pstInfo->info.scd.ulScdUserBandNum2;
			pstAntInfo->scd_band_num[3] = pstInfo->info.scd.ulScdUserBandNum3;
			pstAntInfo->scd_band_freq[0]= pstInfo->info.scd.ulScdUserBandFreq;
			pstAntInfo->scd_band_freq[1]= pstInfo->info.scd.ulScdUserBandFreq1;
			pstAntInfo->scd_band_freq[2]= pstInfo->info.scd.ulScdUserBandFreq2;
			pstAntInfo->scd_band_freq[3]= pstInfo->info.scd.ulScdUserBandFreq3;
			break;
		default:
			hErr = ERR_OK;
			break;
	}

	APK_META_ANTENNA_InfoFree(pstInfo);

	HxLOG_Trace();

	return hErr;
}

HERROR		OPL_ANTINFO_SetInfo(HINT32 uid, OPL_AntennaInfo_t *pstAntInfo)
{
	HERROR				 hErr;
	DxAntInfo_t	*pstInfo;

	HxLOG_Trace();
	HxLOG_Assert(pstAntInfo);
	HxLOG_Debug("OPL_ANTINFO_SetInfo : [%s][%s] [%d] [%d]\n", pstAntInfo->satelliteType, pstAntInfo->satelliteName, pstAntInfo->lnb_freq, pstAntInfo->switch_input);

	pstInfo = APK_META_ANTENNA_GetInfo(uid);
	if (pstInfo == NULL)
		return ERR_FAIL;

	HxSTD_MemSet(pstInfo, 0, sizeof(DxAntInfo_t));
	pstInfo->uid			= uid;
	pstInfo->antType		= pstAntInfo->eAntennaType;
	HxSTD_StrNCpy(pstInfo->satType, pstAntInfo->satelliteType, (DxANTINFO_NAME_LEN - 1));
	HxSTD_StrNCpy(pstInfo->satName, pstAntInfo->satelliteName, (DxANTINFO_NAME_LEN - 1));

	hErr = ERR_OK;
	switch (pstInfo->antType)
	{
		case	eDxANT_TYPE_LNB_ONLY:
			pstInfo->info.lnb.ulLnbFreq 	= (HUINT32)pstAntInfo->lnb_freq;
			pstInfo->info.lnb.lnbVoltage	= pstAntInfo->lnb_voltage;
			break;
		case	eDxANT_TYPE_DISEQC:
			pstInfo->info.diseqc.ulLnbFreq		= (HUINT32)pstAntInfo->lnb_freq;
			pstInfo->info.diseqc.lnbVoltage 	= pstAntInfo->lnb_voltage;
			pstInfo->info.diseqc.diseqcVersion	= pstAntInfo->diseqc_version;
			pstInfo->info.diseqc.diseqcInput	= pstAntInfo->switch_input;
			pstInfo->info.diseqc.b22kTone		= pstAntInfo->enable_22khztone;
			pstInfo->info.diseqc.toneBurst		= pstAntInfo->tone_burst;
			pstInfo->info.diseqc.ucMotorPosition= pstAntInfo->position;
			pstInfo->info.diseqc.sSatLonggitude = pstAntInfo->longitude;
			break;
		case	eDxANT_TYPE_SCD:
			pstInfo->info.scd.ulLnbFreq 		= (HUINT32)pstAntInfo->lnb_freq;
			pstInfo->info.scd.lnbVoltage		= pstAntInfo->lnb_voltage;
			pstInfo->info.scd.b22kTone			= pstAntInfo->enable_22khztone;
			pstInfo->info.scd.scdInput			= pstAntInfo->switch_input;
			pstInfo->info.scd.ulScdUserBandNum	= pstAntInfo->scd_band_num[0];
			pstInfo->info.scd.ulScdUserBandNum1 = pstAntInfo->scd_band_num[1];
			pstInfo->info.scd.ulScdUserBandNum2	= pstAntInfo->scd_band_num[2]; // MOON_SCD
			pstInfo->info.scd.ulScdUserBandNum3 = pstAntInfo->scd_band_num[3];
			pstInfo->info.scd.ulScdUserBandFreq = pstAntInfo->scd_band_freq[0];
			pstInfo->info.scd.ulScdUserBandFreq1 = pstAntInfo->scd_band_freq[1];
			pstInfo->info.scd.ulScdUserBandFreq2 = pstAntInfo->scd_band_freq[2];
			pstInfo->info.scd.ulScdUserBandFreq3 = pstAntInfo->scd_band_freq[3];
			break;
		default:
			HLIB_STD_MemFree(pstInfo);
			return ERR_FAIL;
	}

	APK_META_ANTENNA_SetInfo(uid, pstInfo);
	APK_META_ANTENNA_InfoFree(pstInfo);

	APK_META_ANTENNA_Save();

	return hErr;
}

HERROR	OPL_ANTINFO_Sync(void)
{
	APK_META_ANTENNA_Save();

	return ERR_OK;
}

HERROR	OPL_ANTINFO_Refresh(void)
{
	APK_META_ANTENNA_Load();

	return ERR_OK;
}

