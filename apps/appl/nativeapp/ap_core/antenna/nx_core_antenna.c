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

/******************************************************************************/
/**
 * @file	  		nx_core_search.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/


/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <nx_core_antenna.h>
#include <nx_core_system.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_OTL_SAT_SATNAME_AMOS_2_3							"AMOS_2&3"
#define	NX_OTL_SAT_SATNAME_APSTAR_2R						"APSTAR_2R"
#define	NX_OTL_SAT_SATNAME_ARABSAT_5A						"ARABSAT_5A"
#define	NX_OTL_SAT_SATNAME_ASIASAT_3S						"ASIASAT 3S"
#define	NX_OTL_SAT_SATNAME_ASIASAT_5						"ASIASAT_5"
#define	NX_OTL_SAT_SATNAME_ASTRA_1							"ASTRA_1"
#define	NX_OTL_SAT_SATNAME_ASTRA_2							"ASTRA_2"
#define	NX_OTL_SAT_SATNAME_ASTRA_3A_3B						"ASTRA_3A&3B"
#define	NX_OTL_SAT_SATNAME_ASTRA_4A							"ASTRA_4A"
#define	NX_OTL_SAT_SATNAME_ATLANTIC_BIRD_1					"ATLANTIC_BIRD_1"
#define	NX_OTL_SAT_SATNAME_ATLANTIC_BIRD_2					"ATLANTIC_BIRD_2"
#define	NX_OTL_SAT_SATNAME_ATLANTIC_BIRD_3					"ATLANTIC_BIRD_3"
#define	NX_OTL_SAT_SATNAME_BADR_4_6							"BADR_4&6"
#define	NX_OTL_SAT_SATNAME_EUROBIRD_1						"EUROBIRD_1"		//Eurobird 1 -> Eutelsat 28A -> Eutelsat 33C
#define	NX_OTL_SAT_SATNAME_EUROBIRD_3						"EUROBIRD_3"		//EUROBIRD 3 -> Eutelsat 33B/33C
#define	NX_OTL_SAT_SATNAME_EUROBIRD_9A						"EUROBIRD_9A"		//Eurobird 9A -> Eutelsat 9A
#define	NX_OTL_SAT_SATNAME_EUTELSAT_W2A						"EUTELSAT_W2A"		//Eutelsat W2A -> Eutelsat 10A
#define	NX_OTL_SAT_SATNAME_EUTELSAT_W3A						"EUTELSAT_W3A"		//Eutelsat W3A -> Eutelsat 7A
#define	NX_OTL_SAT_SATNAME_EUTELSAT_W4_W7					"EUTELSAT_W4&W7"	//Eutelsat W7 -> Eutelsat 36B
//#define	NX_OTL_SAT_SATNAME_EUTELSAT_W4_W7					"EUTELSAT_W4&W7"	//Eutelsat W4 -> Eutelsat 36A
#define	NX_OTL_SAT_SATNAME_EUTELSAT_W6						"EUTELSAT_W6"	//EUTELSAT W6 -> Eutelsat 21B
#define	NX_OTL_SAT_SATNAME_EXPRESS_AM1						"EXPRESS_AM1"
#define	NX_OTL_SAT_SATNAME_EXPRESS_AM22						"EXPRESS_AM22"
#define	NX_OTL_SAT_SATNAME_HELLAS_SAT2						"HELLAS_SAT2"
#define	NX_OTL_SAT_SATNAME_HISPASAT_1C_1D					"HISPASAT_1C&1D"
#define	NX_OTL_SAT_SATNAME_HOTBIRD							"HOTBIRD"
#define	NX_OTL_SAT_SATNAME_INSAT_3A_4B						"INSAT_3A&4B"
#define	NX_OTL_SAT_SATNAME_INTELSAT_10_02					"INTELSAT_10-02"
#define	NX_OTL_SAT_SATNAME_INTELSAT_12						"INTELSAT_12"
#define	NX_OTL_SAT_SATNAME_INTELSAT_7_10					"INTELSAT_7&10"
#define	NX_OTL_SAT_SATNAME_INTELSAT_901						"INTELSAT_901"
#define	NX_OTL_SAT_SATNAME_INTELSAT_902						"INTELSAT_902"
#define	NX_OTL_SAT_SATNAME_INTELSAT_904						"INTELSAT_904"
#define	NX_OTL_SAT_SATNAME_INTELSAT_906						"INTELSAT_906"
#define	NX_OTL_SAT_SATNAME_INTELSAT_907						"INTELSAT_907"
#define	NX_OTL_SAT_SATNAME_NILESAT_101_102					"NILESAT_101&102"
#define	NX_OTL_SAT_SATNAME_NSS_12							"NSS_12"
#define	NX_OTL_SAT_SATNAME_NSS_7							"NSS_7"
#define	NX_OTL_SAT_SATNAME_TELSTAR_12						"TELSTAR_12"
#define	NX_OTL_SAT_SATNAME_THOR_5_6							"THOR_5&6"
#define	NX_OTL_SAT_SATNAME_TURKSAT_2A_3A					"TURKSAT_2A&3A"
#define	NX_OTL_SAT_SATNAME_YAMAL_201						"YAMAL_201"
#define	NX_OTL_SAT_SATNAME_ARABSAT_2B						"ARABSAT_2B"
#define	NX_OTL_SAT_SATNAME_EUTELSAT_W1						"EUTELSAT_W1"
#define	NX_OTL_SAT_SATNAME_EUTELSAT_W2						"EUTELSAT_W2"
#define	NX_OTL_SAT_SATNAME_NSS_703							"NSS_703"
#define	NX_OTL_SAT_SATNAME_SIRIUS							"SIRIUS"
#define	NX_OTL_SAT_SATNAME_TELECOM_2D						"TELECOM_2D"
#define	NX_OTL_SAT_SATNAME_EUROBIRD_9						"EUROBIRD_9"
#define	NX_OTL_SAT_SATNAME_INTELSAT_802						"INTELSAT_802"
#define	NX_OTL_SAT_SATNAME_THOR_3_5							"THOR_3_5"
#define	NX_OTL_SAT_SATNAME_INSAT_2E4A						"INSAT_2E4A"
#define	NX_OTL_SAT_SATNAME_INTELSAT_17						"INTELSAT_17"
#define	NX_OTL_SAT_SATNAME_YAHLIVE							"YAHLIVE"
#define	NX_OTL_SAT_SATNAME_USERDEFINE						"User_Define"
#define	NX_OTL_SAT_SATNAME_USERDEFINE1						"User_Define_1"
#define	NX_OTL_SAT_SATNAME_USERDEFINE2						"User_Define_2"
#define	NX_OTL_SAT_SATNAME_USERDEFINE3						"User_Define_3"
#define	NX_OTL_SAT_SATNAME_USERDEFINE4						"User_Define_4"
#define	NX_OTL_SAT_SATNAME_USERDEFINE5						"User_Define_5"
#define	NX_OTL_SAT_SATNAME_NONE								"NONE"


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/


/**************************************************************************************************/
#define ___________________Private_Prototype________________________________________________
/**************************************************************************************************/
static void		nx_antenna_Command_Init(void);

//static HERROR	nx_antenna_SetDefaultUserAntennaValue(HINT32 lSlot);
//static HERROR	nx_antenna_SetAntennaSettingToUserDB(void);
static HERROR	nx_antenna_SetInitAntennaIndexToUDB(void);

static HERROR	nx_antenna_ConvDefaultTpInfoToTuningInfo (const Nx_SAT_TunningInfo_t *pDefaultTpInfo, NX_TP_TuningInfo_t *pstTpInfo);
static HERROR	nx_antenna_FreeTpListArray(NX_TP_TuningInfo_t *pstTpInfoArray);
static void		nx_antenna_MakeSatelliteTpString (NX_TP_TuningInfo_t *tp, HUINT8 *pString);
static HINT32	nx_antenna_FindAssignAntennaUid(void);

#if defined(CONFIG_DEBUG)
static void		nx_antenna_printAntennaInfo(DxAntInfo_t *pstAntInfo);
#endif

/**************************************************************************************************/
#define ___________________Private_functions________________________________________________
/**************************************************************************************************/

static HERROR	nx_antenna_SetInitAntennaIndexToUDB(void)
{
	HINT32			i = 0, lSlotIdx = eNxSat_ANTENAA_SLOT1, lUid = -1;
	HUINT32 		ulAntCount = 0, ulAntUDBCount = 0;
	HERROR			hError = ERR_FAIL;
	DxAntInfo_t 	stAntInfo;


	ulAntUDBCount = NX_PORT_META_ANTENNA_GetAntennaUDBCount();
	if (ulAntUDBCount == 0)
	{
		NX_APP_Info("No UDB Antenna Setting!! [%d]\n", ulAntUDBCount);
		for (i = 0; i < NX_PORT_ANTENNA_MAX_ANT_NUM; i++)
		{
			NX_PORT_META_ANTENNA_SetAntIndexUDB(eNxSat_ANTENAA_SLOT1 + i, NX_ANT_IDX_NULL);
		}
	}

	ulAntCount = NX_ANTENNA_Count();
	if (ulAntCount == 0)
	{
		NX_APP_Info("No Antenna Setting!! [%d]\n", ulAntCount);
		return	ERR_OK;
	}

	if (ulAntCount != ulAntUDBCount)
	{
		NX_APP_Error("[%s:%d] UDB&Appiet Sync Error  Antenna Setting!! [%d]\n", __FUNCTION__, __LINE__, ulAntCount);

		for (i = 0; i <ulAntCount; i++)
		{
			lUid = NX_ANTENNA_GetId(i);
			HxSTD_memset(&stAntInfo, 0x00, sizeof(DxAntInfo_t));
			hError = NX_ANTENNA_GetInfo(lUid, &stAntInfo);
			if ((hError == ERR_OK)	&& (HxSTD_StrCmp(stAntInfo.satName, (char*)NX_SAT_SATNAME_NONE) != 0))
			{
				NX_APP_Info("Add Antenna Setting!! [%d], Uid:[%d], Name:[%s]\n", i, lUid, stAntInfo.satName);
				NX_PORT_META_ANTENNA_SetAntIndexUDB(lSlotIdx, stAntInfo.uid);
				lSlotIdx++;
			}
		}
	}

	return	ERR_OK;
}


static HERROR	nx_antenna_FreeTpListArray(NX_TP_TuningInfo_t *pstTpInfoArray)
{
	if (pstTpInfoArray != NULL)
	{
		NX_APP_Free(pstTpInfoArray);
	}

	return	ERR_OK;
}


static void		nx_antenna_MakeSatelliteTpString (NX_TP_TuningInfo_t *tp, HUINT8 *pString)
{
	HCHAR		szTextBuffer[NX_TEXT_SIZE_16];

	/* tp frequecy */
	HxSTD_memset (szTextBuffer, 0x00, NX_TEXT_SIZE_16);
	szTextBuffer[0] = '\0';

	HxSTD_snprintf(szTextBuffer, NX_TEXT_SIZE_16, (char*)"%d, ", (tp->sat.ulFrequency/NX_COM_CH_UNIT_MHZ));
	HxSTD_StrCpy((char*)pString, szTextBuffer);

	/* polarization */
	HxSTD_memset (szTextBuffer, 0x00, NX_TEXT_SIZE_16);
	szTextBuffer[0] = '\0';
	switch(tp->sat.ePolarization)
	{
		case eDxSAT_POLAR_AUTO:		HxSTD_snprintf((char*)szTextBuffer, NX_TEXT_SIZE_16, (char*)"%s, ", "A");		break;
		case eDxSAT_POLAR_HOR:		HxSTD_snprintf((char*)szTextBuffer, NX_TEXT_SIZE_16, (char*)"%s, ", "H");		break;
		case eDxSAT_POLAR_VER:		HxSTD_snprintf((char*)szTextBuffer, NX_TEXT_SIZE_16, (char*)"%s, ", "V");		break;
		case eDxSAT_POLAR_LEFT:		HxSTD_snprintf((char*)szTextBuffer, NX_TEXT_SIZE_16, (char*)"%s, ", "L");		break;
		case eDxSAT_POLAR_RIGHT:	HxSTD_snprintf((char*)szTextBuffer, NX_TEXT_SIZE_16, (char*)"%s, ", "R");		break;
		default:					HxSTD_snprintf((char*)szTextBuffer, NX_TEXT_SIZE_16, (char*)"NOT, ");			break;
	}
	HxSTD_StrCat((char*)pString, szTextBuffer);

	/* symbol rate */
	HxSTD_memset (szTextBuffer, 0x00, NX_TEXT_SIZE_16);
	szTextBuffer[0] = '\0';
	HxSTD_snprintf((char*)szTextBuffer, NX_TEXT_SIZE_16, (char*)"%d, ", (tp->sat.ulSymbolRate/NX_COM_CH_UNIT_MHZ));
	HxSTD_StrCat ((char*)pString, szTextBuffer);

	/* FEC */
	HxSTD_memset (szTextBuffer, 0x00, NX_TEXT_SIZE_16);
	szTextBuffer[0] = '\0';
	switch(tp->sat.eFecCodeRate)
	{
		case eDxSAT_CODERATE_1_2:	HxSTD_snprintf((char*)szTextBuffer, NX_TEXT_SIZE_16, (char*)"1/2");		break;
		case eDxSAT_CODERATE_2_3:	HxSTD_snprintf((char*)szTextBuffer, NX_TEXT_SIZE_16, (char*)"2/3");		break;
		case eDxSAT_CODERATE_3_4:	HxSTD_snprintf((char*)szTextBuffer, NX_TEXT_SIZE_16, (char*)"3/4");		break;
		case eDxSAT_CODERATE_3_5:	HxSTD_snprintf((char*)szTextBuffer, NX_TEXT_SIZE_16, (char*)"3/5");		break;
		case eDxSAT_CODERATE_4_5:	HxSTD_snprintf((char*)szTextBuffer, NX_TEXT_SIZE_16, (char*)"4/5");		break;
		case eDxSAT_CODERATE_5_11:	HxSTD_snprintf((char*)szTextBuffer, NX_TEXT_SIZE_16, (char*)"5/11");	break;
		case eDxSAT_CODERATE_5_6:	HxSTD_snprintf((char*)szTextBuffer, NX_TEXT_SIZE_16, (char*)"5/6");		break;
		case eDxSAT_CODERATE_6_7:	HxSTD_snprintf((char*)szTextBuffer, NX_TEXT_SIZE_16, (char*)"6/7");		break;
		case eDxSAT_CODERATE_7_8:	HxSTD_snprintf((char*)szTextBuffer, NX_TEXT_SIZE_16, (char*)"7/8");		break;
		case eDxSAT_CODERATE_8_9:	HxSTD_snprintf((char*)szTextBuffer, NX_TEXT_SIZE_16, (char*)"8/9");		break;
		case eDxSAT_CODERATE_9_10:	HxSTD_snprintf((char*)szTextBuffer, NX_TEXT_SIZE_16, (char*)"9/10");	break;
		case eDxSAT_CODERATE_AUTO:	HxSTD_snprintf((char*)szTextBuffer, NX_TEXT_SIZE_16, (char*)"A");		break;
		case eDxSAT_CODERATE_NONE:
		default:
			HxSTD_snprintf((char*)szTextBuffer, NX_TEXT_SIZE_16, (char*)"NOT");
			break;
	}

	HxSTD_StrCat ((char*)pString, szTextBuffer);
}


static HINT32		nx_antenna_FindAssignAntennaUid(void)
{
	HINT32			lAntUid = NX_ANT_IDX_NULL, lFindAntUid = NX_ANT_IDX_NULL;
	HUINT32			ulAntCount = 0, i = 0;
	HERROR			hError = ERR_FAIL;
	DxAntInfo_t 	stAntInfo;


	ulAntCount = NX_ANTENNA_Count();
	if (ulAntCount == 0)
	{
		lFindAntUid = NX_ANTENNA_New();
		HxLOG_Info("Empty Antenna Slot Info! Add New Antenna(lFindAntUid:[0x%x])\n", lFindAntUid);
	}
	else
	{
		for (i = 0; i < ulAntCount; i++)
		{
			lAntUid = NX_ANTENNA_GetId(i);
			HxSTD_MemSet(&stAntInfo, 0x00, sizeof(DxAntInfo_t));
			hError = NX_ANTENNA_GetInfo(lAntUid, &stAntInfo);
			if (hError == ERR_OK)
			{
				if (HxSTD_StrCmp(stAntInfo.satType, (char*)NX_SAT_SATNAME_NONE) == 0)
				{
					lFindAntUid = lAntUid;
					break;
				}
			}
		}

		if (lFindAntUid == NX_ANT_IDX_NULL)
		{
			lFindAntUid = NX_ANTENNA_New();
		}
	}

	return	lFindAntUid;
}


/**************************************************************************************************/
#define ___________________Public_functions________________________________________________
/**************************************************************************************************/
HERROR NX_ANTENNA_Init(void)
{
#if defined(CONFIG_DEBUG)
	nx_antenna_Command_Init();
#endif

	return ERR_OK;
}

#define ________________RELATED_META_ANTENNA_________________

HUINT32	NX_ANTENNA_Count(void)
{
	return NX_PORT_META_ANTENNA_Count();
}

HINT32	NX_ANTENNA_GetId(HINT32 nIndex)
{
	return NX_PORT_META_ANTENNA_GetUidAt(nIndex);
}

HINT32	NX_ANTENNA_New(void)
{
	DxAntInfo_t	stAntInfo;

	HxSTD_MemSet(&stAntInfo, 0, sizeof(DxAntInfo_t));

	return NX_PORT_META_ANTENNA_Add(&stAntInfo);
}

HERROR	NX_ANTENNA_RemoveAll(void)
{
	NX_PORT_META_ANTENNA_RemoveAll();

	return ERR_OK;
}

HERROR	NX_ANTENNA_GetAntennaType(HINT32 *lValue)
{
	HERROR			hError = ERR_FAIL;

	hError = NX_PORT_DB_GetInt(USERDB_ANTSET_ANTENNATYPE_ID, lValue);
	if (hError != ERR_OK)
	{
		NX_APP_Error("[%s:%d] Error!! ID : [%s][%d]\n", __FUNCTION__, __LINE__, USERDB_ANTSET_ANTENNATYPE_ID, lValue);
		return	ERR_FAIL;
	}

	return	ERR_OK;
}


HERROR	NX_ANTENNA_SetAntennaType(HINT32 lValue)
{
	HERROR			hError = ERR_FAIL;

	hError = NX_PORT_DB_SetInt(USERDB_ANTSET_ANTENNATYPE_ID, lValue);
	if (hError != ERR_OK)
	{
		NX_APP_Error("[%s:%d] Error!! ID : [%s][%d]\n", __FUNCTION__, __LINE__, USERDB_ANTSET_ANTENNATYPE_ID, lValue);
		return	ERR_FAIL;
	}

	return	ERR_OK;
}

HERROR	NX_ANTENNA_SetInfo(HINT32 uid, DxAntInfo_t *pstAntInfo)
{
	DxAntInfo_t			*pstInfo = NULL;

	HxLOG_Trace();
	HxLOG_Assert(pstAntInfo);

	pstInfo = NX_PORT_META_ANTENNA_GetInfo(uid);
	if (pstInfo == NULL)
	{
		HxLOG_Error("Empty Antenna Slot Info\n");
		NX_PORT_META_ANTENNA_SetInfo(uid, pstAntInfo);
	}
	else
	{
		HxSTD_MemSet(pstInfo, 0, sizeof(DxAntInfo_t));
		HxSTD_memcpy(pstInfo, pstAntInfo, sizeof(DxAntInfo_t));
		NX_PORT_META_ANTENNA_SetInfo(uid, pstInfo);
		NX_PORT_META_ANTENNA_InfoFree(pstInfo);
	}

#if defined(CONFIG_DEBUG)
	nx_antenna_printAntennaInfo(pstAntInfo);
#endif

	NX_PORT_META_ANTENNA_Save();

	return	ERR_OK;
}

HERROR	NX_ANTENNA_GetInfo(HINT32 uid, DxAntInfo_t *pstAntInfo)
{
	HERROR		 hErr = ERR_OK;
	DxAntInfo_t	*pstInfo;

	HxLOG_Trace();
	HxLOG_Assert(pstAntInfo);

	pstInfo = NX_PORT_META_ANTENNA_GetInfo(uid);
	if (pstInfo == NULL)
		return ERR_FAIL;

	HxLOG_Debug("pstInfo : antType : [%d] satType : [%s] satName : [%s]\n", pstInfo->antType, pstInfo->satType, pstInfo->satName);


	HxSTD_MemSet(pstAntInfo, 0x00, sizeof(DxAntInfo_t));
	HxSTD_MemCopy(pstAntInfo, pstInfo, sizeof(DxAntInfo_t));
/*
	HxSTD_MemSet(pstAntInfo, 0, sizeof(NX_AntennaInfo_t));

	pstAntInfo->eAntennaType		= pstInfo->antType;
	//	alloc하여야 하나 윗단에서 쓰고 버리는 조건으로 그냥 pointer만 매핑한다.
	HxSTD_StrNCpy(pstAntInfo->satelliteType, pstInfo->satType, (NX_PORT_ANTINFO_NAME_LEN - 1));
	HxSTD_StrNCpy(pstAntInfo->satelliteName, pstInfo->satName, (NX_PORT_ANTINFO_NAME_LEN - 1));
	pstAntInfo->satelliteName[NX_PORT_ANTINFO_NAME_LEN - 1] = 0;
	pstAntInfo->satelliteType[NX_PORT_ANTINFO_NAME_LEN - 1] = 0;

	hErr = ERR_OK;
	switch (pstInfo->antType)
	{
		case	eDxANT_TYPE_LNB_ONLY:
			pstAntInfo->lnb_freq	= pstInfo->info.lnb.ulLnbFreq;
			pstAntInfo->lnb_voltage = pstInfo->info.lnb.lnbVoltage;
			break;
		case	eDxANT_TYPE_DISEQC:
			pstAntInfo->lnb_freq		= pstInfo->info.diseqc.ulLnbFreq;
			pstAntInfo->lnb_voltage 	= pstInfo->info.diseqc.lnbVoltage;
			pstAntInfo->diseqc_version	= pstInfo->info.diseqc.diseqcVersion;
			pstAntInfo->switch_input	= pstInfo->info.diseqc.diseqcInput;
			pstAntInfo->enable_22khztone= pstInfo->info.diseqc.b22kTone;
			pstAntInfo->tone_burst		= pstInfo->info.diseqc.toneBurst;
			pstAntInfo->position		= pstInfo->info.diseqc.ucMotorPosition;
			pstAntInfo->longitude		= pstInfo->info.diseqc.sSatLonggitude;
			break;
		case	eDxANT_TYPE_SCD:
			pstAntInfo->lnb_freq		= pstInfo->info.scd.ulLnbFreq;
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
*/
	NX_PORT_META_ANTENNA_InfoFree(pstInfo);

	HxLOG_Trace();

	return hErr;
}

HERROR	NX_ANTENNA_SetConnectionType(HINT32 n)
{
	NX_COMMON_NOT_USED_PARAM(n);
	return ERR_OK;
}

HERROR	NX_ANTENNA_GetConnectionType(DxAntennaConnection_e *eAntConnetion)
{
	return NX_PORT_META_ANTENNA_GetConnectionType(eAntConnetion);
}

HERROR	NX_ANTENNA_Sync(void)
{
	return NX_PORT_META_ANTENNA_Save();
}

HERROR	NX_ANTENNA_Refresh(void)
{
	return NX_PORT_META_ANTENNA_Load();
}


#define ________________RELATED_USER_ANTENNA_________________

HERROR	NX_ANTENNA_GetUserTPInfo(HINT32 slot, Nx_TpSelectInfo_t *pstTpInfo)
{
	return NX_PORT_META_ANTENNA_LoadUserTp(slot, pstTpInfo);
}

HERROR	NX_ANTENNA_SetUserTPInfo(HINT32 slot, Nx_TpSelectInfo_t *pstTpInfo)
{
	return NX_PORT_META_ANTENNA_SaveUserTp(slot, pstTpInfo);
}

HERROR	NX_ANTENNA_SetUserAntIndexUDB(HINT32 lSlot, HINT32 lAntIdx)
{
	return	NX_PORT_META_ANTENNA_SetAntIndexUDB(lSlot, lAntIdx);
}

HERROR	NX_ANTENNA_GetUserAntIndexUDB(HINT32 lSlot, HINT32 *plAntIdx)
{
	return	NX_PORT_META_ANTENNA_GetAntIndexUDB(lSlot, plAntIdx);
}

HINT32	NX_ANTENNA_GetUserAntCountUDB(void)
{
	return	NX_PORT_META_ANTENNA_GetAntennaUDBCount();
}


#define ________________RELATED_ELSE_ANTENNA_________________

HERROR		NX_ANTENNA_LoadAntennaSetting(void)
{
	//return	nx_antenna_SetAntennaSettingToUserDB();
	return	nx_antenna_SetInitAntennaIndexToUDB();
}


HCHAR*	NX_ANTENNA_GetDefaultSatelliteName(NX_SAT_DefaultSatType_e eDefaultSatType)
{
	switch (eDefaultSatType)
	{
		case eNxSat_DefSatType_None:	return	(HCHAR*)NX_SAT_SATNAME_NONE;
		case eNxSat_DefSatType_EU01:	return	(HCHAR*)NX_SAT_SATNAME_ASTRA_1;
		case eNxSat_DefSatType_EU02:	return	(HCHAR*)NX_SAT_SATNAME_HOTBIRD;
		case eNxSat_DefSatType_ME:		return	(HCHAR*)NX_SAT_SATNAME_NILESAT_101_102;
		case eNxSat_DefSatType_AMX:		return 	(HCHAR*)NX_SAT_SATNAME_STAR_ONE_C4;
		default:						return	(HCHAR*)NX_SAT_SATNAME_NONE;
	}
}


HERROR		NX_ANTENNA_MakeDefaultAntennaInfo(DxAntennaType_e eAntennaType, DxAntInfo_t *pstDefaultAntInfo)
{
	HERROR			hError = ERR_FAIL;
	HINT32			lAntUid = NX_ANT_IDX_NULL;
	HCHAR			*szDefaultSatName = NULL;
	DxAntInfo_t		stAntInfo;
	DxOperator_e	eOpType = eDxOPERATOR_None;
	HxCountry_e 	eCountryCode = eCountryID_NONE;
	NX_SAT_DefaultSatType_e		eDefaultSatType = eNxSat_DefSatType_None;


	if (pstDefaultAntInfo == NULL)
	{
		HxLOG_Debug("pstDefaultAntInfo is NULL !\n");
		return	ERR_FAIL;
	}

	HxSTD_MemSet(&stAntInfo, 0x00, sizeof(DxAntInfo_t));

	/* Get Antenna UID */
	lAntUid = nx_antenna_FindAssignAntennaUid();
	stAntInfo.uid = lAntUid;
	stAntInfo.antType = eAntennaType;

	hError = NX_SYSTEM_GetOperatorType(&eOpType);
	if (hError != ERR_OK)
	{
		NX_APP_Error("[%s:%d] ERROR!!! NX_SYSTEM_GetOperatorType() \n", __FUNCTION__, __HxLINE__);
	}

	switch (eOpType)
	{
		case eDxOPERATOR_AMX:
			eDefaultSatType = eNxSat_DefSatType_AMX;
			szDefaultSatName = NX_ANTENNA_GetDefaultSatelliteName(eDefaultSatType);
			break;
		case eDxOPERATOR_MiddleEast:
			eDefaultSatType = eNxSat_DefSatType_ME;
			szDefaultSatName = NX_ANTENNA_GetDefaultSatelliteName(eDefaultSatType);
			break;
		default:
			{
				hError = NX_SYSTEM_GetCountryCode(&eCountryCode);
				if (hError != ERR_OK)
				{
					NX_APP_Error("ERROR!!! NX_SYSTEM_GetCountryCode() Fail!! \n");
				}

				if ((eCountryCode == eCountryID_SWZ) || (eCountryCode == eCountryID_ITA))
				{
					eDefaultSatType = eNxSat_DefSatType_EU02;
				}
				else
				{
					eDefaultSatType = eNxSat_DefSatType_EU01;
				}
				szDefaultSatName = NX_ANTENNA_GetDefaultSatelliteName(eDefaultSatType);
			}
			break;
	}

	HLIB_STD_StrUtf8NCpy(stAntInfo.satType, szDefaultSatName, DxANTINFO_NAME_LEN);
	HLIB_STD_StrUtf8NCpy(stAntInfo.satName, szDefaultSatName, DxANTINFO_NAME_LEN);

	switch (stAntInfo.antType)
	{
		case eDxANT_TYPE_LNB_ONLY:
			stAntInfo.info.lnb.ulLnbFreq = NX_COM_CH_UNIVERSAL_LNB_FREQ;
			stAntInfo.info.lnb.lnbVoltage = eDxLNB_VOLT_STD;
			break;
		case eDxANT_TYPE_DISEQC:
			stAntInfo.info.diseqc.ulLnbFreq = NX_COM_CH_UNIVERSAL_LNB_FREQ;
			stAntInfo.info.diseqc.lnbVoltage = eDxLNB_VOLT_STD;
			stAntInfo.info.diseqc.diseqcVersion = eDxDISEQC_VER_1_0;
			stAntInfo.info.diseqc.diseqcInput = eDxDISEQC_INPUT_A;
			break;
		case eDxANT_TYPE_SCD:
			stAntInfo.info.scd.ulLnbFreq = NX_COM_CH_UNIVERSAL_LNB_FREQ;
			stAntInfo.info.scd.ulScdUserBandNum = 0;
			stAntInfo.info.scd.ulScdUserBandNum1 = 1;
			stAntInfo.info.scd.ulScdUserBandNum2 = 2;
			stAntInfo.info.scd.ulScdUserBandNum3 = 3;
			stAntInfo.info.scd.ulScdUserBandFreq = 1210;
			stAntInfo.info.scd.ulScdUserBandFreq1 = 1400;
			stAntInfo.info.scd.ulScdUserBandFreq2 = 1516;
			stAntInfo.info.scd.ulScdUserBandFreq3 = 1632;
			break;
		case eDxANT_TYPE_SMATV:
		case eDxANT_TYPE_MONOBLOCK_LNB:
		default:
			HxLOG_Debug("Pls add defualt Antenna Info for Type [%d]\n", stAntInfo.antType);
			break;
	}

	HxSTD_MemCopy(pstDefaultAntInfo, &stAntInfo, sizeof(DxAntInfo_t));

	return	ERR_OK;
}


HERROR		NX_ANTENNA_GetTpListBySatelliteTypeName(const HCHAR *szSatTypeName, HUINT32 *pulTpNum, NX_TP_TuningInfo_t **ppstTpInfo)
{
	HERROR				hError = ERR_FAIL;
	HUINT32				ulIndex = 0, ulTotalTpCnt = 0, ulDefaultTpNum = 0, ulTotalTpNum = 0;
	NX_TP_TuningInfo_t	*pstTotalTpList = NULL;


	if ((pulTpNum == NULL) || (ppstTpInfo == NULL))
	{
		NX_APP_Error("[%s:%d] Check parameter is NULL!!\n", __FUNCTION__, __LINE__);
		return	ERR_FAIL;
	}

	*pulTpNum = 0; *ppstTpInfo = NULL;

	// Get Tp list Count by Satellite Name
	hError = NX_ANTENNA_CountDefaultTpBySatTypeName(szSatTypeName, &ulDefaultTpNum);
	if (hError != ERR_OK)
	{
		ulDefaultTpNum = 0;
	}

	ulTotalTpNum = ulDefaultTpNum;
	if (ulTotalTpNum == 0)
	{
		NX_APP_Error("[%s:%d] ulTotalTpNum is Zero!!\n", __FUNCTION__, __LINE__);
		return	ERR_FAIL;
	}

	pstTotalTpList = (NX_TP_TuningInfo_t *)NX_APP_Malloc (sizeof(NX_TP_TuningInfo_t) * ulTotalTpNum);
	if (pstTotalTpList == NULL)
	{
		NX_APP_Error("[%s:%d] pstTotalTpList is NULL!!\n", __FUNCTION__, __LINE__);
		return	ERR_FAIL;
	}

	ulTotalTpCnt = 0;
	for (ulIndex = 0; ulIndex < ulDefaultTpNum; ulIndex++)
	{
		hError = NX_ANTENNA_GetDefaultTpBySatTypeName(szSatTypeName, ulIndex, &(pstTotalTpList[ulTotalTpCnt]));
		if (hError == ERR_OK)
		{
			pstTotalTpList[ulTotalTpCnt].sat.antuid = 0;
			ulTotalTpCnt ++;
		}
	}

	if (ulTotalTpCnt > 0)
	{
		*pulTpNum = ulTotalTpCnt;
		*ppstTpInfo = pstTotalTpList;
		return	ERR_OK;
	}

	if (pstTotalTpList)
	{
		nx_antenna_FreeTpListArray(pstTotalTpList);
	}

	return	ERR_FAIL;
}


HERROR		NX_ANTENNA_GetTpBySatelliteTypeName(const HCHAR *szSatTypeName, HUINT32 ulTpIdx, NX_TP_TuningInfo_t *pstTpInfo)
{
	HERROR				hError = ERR_FAIL;
	HUINT32				ulDefaultTpNum = 0;
	NX_TP_TuningInfo_t	stTpInfo;

	if (pstTpInfo == NULL)
	{
		NX_APP_Error("[%s:%d] Check parameter is NULL!!\n", __FUNCTION__, __LINE__);
		return	ERR_FAIL;
	}

	HxSTD_memset(&stTpInfo, 0x00, sizeof(NX_TP_TuningInfo_t));

	// Get Tp list Count by Satellite Name
	hError = NX_ANTENNA_CountDefaultTpBySatTypeName(szSatTypeName, &ulDefaultTpNum);
	if (hError != ERR_OK)
	{
		ulDefaultTpNum = 0;
	}

	if (ulDefaultTpNum == 0)
	{
		NX_APP_Error("[%s:%d] ulTotalTpNum is Zero!!\n", __FUNCTION__, __LINE__);
		return	ERR_FAIL;
	}

	hError = NX_ANTENNA_GetDefaultTpBySatTypeName(szSatTypeName, ulTpIdx, &stTpInfo);
	if (hError == ERR_OK)
	{
		HxSTD_memcpy(pstTpInfo, &stTpInfo, sizeof(NX_TP_TuningInfo_t));
	}
	else
	{
		NX_APP_Error("[%s:%d] ERROR!!! NX_ANTENNA_GetDefaultTpBySatTypeName() is FAIL!!!\n", __FUNCTION__, __LINE__);
		return	ERR_FAIL;
	}

	return	ERR_OK;
}


HERROR		NX_ANTENNA_FreeTpListArray (NX_TP_TuningInfo_t *pstTpInfoArray)
{
	return	nx_antenna_FreeTpListArray (pstTpInfoArray);
}


void		NX_ANTENNA_GetSatelliteRegionType (NX_SAT_SatelliteRegionType_t *peSatRegionType)
{
	HERROR			hError = ERR_FAIL;
	DxOperator_e	eOpType = eDxOPERATOR_None;

	hError = NX_SYSTEM_GetOperatorType(&eOpType);
	if (hError != ERR_OK)
	{
		eOpType = eDxOPERATOR_None;
		NX_APP_Error("Error!!! NX_SYSTEM_GetOperatorType() Fail!! \n");
	}

	switch (eOpType)
	{
		case eDxOPERATOR_AMX:
			*peSatRegionType = eNxSat_SatRegion_SouthAmerica;
			break;
		case eDxOPERATOR_MBC:
			*peSatRegionType = eNxSat_SatRegion_MeSat;
			break;
		// TODO: 지역별로 위성 리스트를 달리 할 때  필요
		default:
			*peSatRegionType = eNxSat_SatRegion_Normal;
			break;
	}

}


HERROR		NX_ANTENNA_CountRegionSatelliteList (NX_SAT_SatelliteRegionType_t eSatRegionType, HUINT32 *pulSatListNum)
{
	HUINT32		ulCount = 0;
	Nx_SAT_SatRegionList_t		*pszSatList = NULL;

	if (pulSatListNum == NULL)
	{
		NX_APP_Error("[%s:%d] Check parameter is NULL!!\n", __FUNCTION__, __LINE__);
		return	ERR_FAIL;
	}

	pszSatList = NX_ANTENNA_GetRegionSatList(eSatRegionType, &ulCount);
	if (pszSatList == NULL)
	{
		*pulSatListNum = 0;
		NX_APP_Error("[%s:%d] Error!! NX_ANTENNA_GetRegionSatList() List is NULL!!\n", __FUNCTION__, __LINE__);
		return	ERR_FAIL;
	}

	*pulSatListNum = ulCount;

	return	ERR_OK;
}


HERROR		NX_ANTENNA_GetRegionSatelliteList (NX_SAT_SatelliteRegionType_t eSatRegionType, Nx_SAT_SatRegionList_t **ppstSatList, HUINT32 *pulSatListNum)
{
	HUINT32		ulCount = 0;
	Nx_SAT_SatRegionList_t		*pstSatList = NULL;


	if ((pulSatListNum == NULL) || (ppstSatList == NULL))
	{
		NX_APP_Error("[%s:%d] Check parameter is NULL!!\n", __FUNCTION__, __LINE__);
		return	ERR_FAIL;
	}

	pstSatList = NX_ANTENNA_GetRegionSatList(eSatRegionType, &ulCount);
	if (pstSatList == NULL)
	{
		return	ERR_FAIL;
	}

	*ppstSatList = pstSatList;
	*pulSatListNum = ulCount;

	return	ERR_OK;
}


HERROR		NX_ANTENNA_GetSatellitePositionString (HCHAR *szSatName, HUINT8 *szSatPosStr)
{
	HINT32		 lDegreeInt = 0, lDegreeDec = 0;
	HINT16		 sSatDegree = 0;
	HUINT8		 ucDirection = 0;
	HERROR		 hError = ERR_FAIL;


	if ((szSatPosStr == NULL) || (szSatName == NULL))
	{
		NX_APP_Error("[%s:%d] Check parameter is NULL!!\n", __FUNCTION__, __LINE__);
		return	ERR_FAIL;
	}

	hError = NX_ANTENNA_GetSatDegree(szSatName, &sSatDegree);
	if (hError != ERR_OK)
	{
		NX_APP_Error("[%s:%d] NX_ANTENNA_GetSatDegree() is NULL!!\n", __FUNCTION__, __LINE__);
		return	ERR_FAIL;
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

	lDegreeInt = sSatDegree / 10;
	lDegreeDec = sSatDegree % 10;

	HxSTD_sprintf ((char*)szSatPosStr, "%d.%d%c", lDegreeInt, lDegreeDec, ucDirection);

	return	ERR_OK;
}


HERROR	NX_ANTENNA_GetSatelliteNameWithSatPosition(HCHAR *szSatTypeName, HCHAR *szDispSatName, HUINT8 *szSatFullName)
{
	HUINT8		 szPosStr[NX_PORT_ANTINFO_NAME_LEN], szTempFullName[NX_PORT_MAX_ANT_NAME];
	HERROR		 hError = ERR_FAIL;


	HxSTD_memset(szPosStr, 0, NX_PORT_ANTINFO_NAME_LEN);
	HxSTD_memset(szTempFullName, 0, NX_PORT_MAX_ANT_NAME);

	hError = NX_ANTENNA_GetSatellitePositionString (szSatTypeName, szPosStr);
	if (hError == ERR_OK)
	{
		HxSTD_sprintf((char*)szTempFullName, "%s (%s)", szDispSatName, szPosStr);
		HxSTD_StrNCpy((char*)szSatFullName, (char*)szTempFullName, NX_PORT_MAX_ANT_NAME);
		szSatFullName[NX_PORT_MAX_ANT_NAME - 1] = '\0';
	}
	else
	{
		HxSTD_StrNCpy((char*)szSatFullName, (char*)szDispSatName, NX_PORT_MAX_ANT_NAME);
		szSatFullName[NX_PORT_MAX_ANT_NAME - 1] = '\0';
	}

	return	ERR_OK;
}


void	NX_ANTENNA_MakeTpString (DxDeliveryType_b ulDeliType, NX_TP_TuningInfo_t *tp, DxNetwork_t *pstNetwork, HUINT8 *pString)
{
	HCHAR		szTextBuffer[NX_TEXT_SIZE_16];

	switch (ulDeliType)
	{
		case eDxDELIVERY_TYPE_SAT:
			nx_antenna_MakeSatelliteTpString(tp, pString);
			break;

		case eDxDELIVERY_TYPE_TER:
			HxSTD_snprintf ((char*)pString, (NX_TEXT_SIZE_32 - 1), (char*)"%d kHz", tp->ter.ulFrequency);

			/* bandwidth */
			switch (tp->ter.eBandWidth)
			{
				case eDxTER_BANDWIDTH_6M:	HxSTD_StrCat ((char*)pString, (char*)", 6MHz");		break;
				case eDxTER_BANDWIDTH_7M:	HxSTD_StrCat ((char*)pString, (char*)", 7MHz");		break;
				case eDxTER_BANDWIDTH_8M:	HxSTD_StrCat ((char*)pString, (char*)", 8MHz");		break;
				default:																		break;
			}
			break;

		case eDxDELIVERY_TYPE_CAB:
			HxSTD_memset (szTextBuffer, 0x00, NX_TEXT_SIZE_16);
			szTextBuffer[0] = '\0';
			switch (tp->cab.constellation)
			{
				case eDxCAB_CONSTELLATION_16QAM: 	HxSTD_StrCpy(szTextBuffer, (char*)"16");	break;
				case eDxCAB_CONSTELLATION_32QAM: 	HxSTD_StrCpy(szTextBuffer, (char*)"32");	break;
				case eDxCAB_CONSTELLATION_64QAM:	HxSTD_StrCpy(szTextBuffer, (char*)"64");	break;
				case eDxCAB_CONSTELLATION_128QAM:	HxSTD_StrCpy(szTextBuffer, (char*)"128");	break;
				case eDxCAB_CONSTELLATION_256QAM:	HxSTD_StrCpy(szTextBuffer, (char*)"256");	break;
				case eDxCAB_CONSTELLATION_1024QAM:	HxSTD_StrCpy(szTextBuffer, (char*)"1024");	break;
				case eDxCAB_CONSTELLATION_4096QAM:	HxSTD_StrCpy(szTextBuffer, (char*)"4096");	break;
				case eDxCAB_CONSTELLATION_AUTO:		HxSTD_StrCpy(szTextBuffer, (char*)"AUTO");	break;
			}
			HxSTD_snprintf((char*)pString, (NX_TEXT_SIZE_32 - 1), (char*)"%d kHz SR %d %s QAM", tp->cab.ulFrequency, tp->cab.ulSymbolRate, szTextBuffer);
			break;
		default:
			break;
	}

	NX_COMMON_NOT_USED_PARAM(pstNetwork);
}


HINT32		NX_ANTENNA_FindAssignAntennaUid(void)
{
	return	nx_antenna_FindAssignAntennaUid();
}


/***************************************************************************
Related Search Command
***************************************************************************/
#if defined(CONFIG_DEBUG)
static HINT32 cmdSaveUserAntenna(void *szArgs)
{
	DxAntInfo_t antInfo;

	HxSTD_MemSet(&antInfo, 0, sizeof(DxAntInfo_t));

	antInfo.antType	= eDxANT_TYPE_DISEQC;
	antInfo.uid		= 0;
	HxSTD_sprintf(antInfo.satType, "%s", "TESTANTTYPE");
	HxSTD_sprintf(antInfo.satName, "%s", "NILESAT");

	antInfo.info.diseqc.ulLnbFreq		= 12000;
	antInfo.info.diseqc.lnbVoltage		= eDxLNB_VOLT_STD;
	antInfo.info.diseqc.b22kTone		= TRUE;
	antInfo.info.diseqc.diseqcVersion	= eDxDISEQC_VER_1_2;
	antInfo.info.diseqc.diseqcInput		= eDxDISEQC_INPUT_C;
	antInfo.info.diseqc.toneBurst		= eDxTONE_BURST_A;
	antInfo.info.diseqc.ucMotorPosition	= 20150403;
	antInfo.info.diseqc.sSatLonggitude	= 222223232;

	NX_PORT_META_ANTENNA_SaveUserAntenna(0, &antInfo);
	NX_PORT_META_ANTENNA_SaveUserAntenna(1, &antInfo);

	NX_COMMON_NOT_USED_PARAM(szArgs);
	return HxCMD_OK;
}

static HINT32 cmdLoadeUserAntenna(void *szArgs)
{
	DxAntInfo_t antInfo;

	HxSTD_MemSet(&antInfo, 0, sizeof(DxAntInfo_t));

	if (NX_PORT_META_ANTENNA_LoadUserAntenna(0, &antInfo) == ERR_OK)
	{
		NX_APP_Debug("\n[%s.%d] Successed Load User Ant Info!!\n",__FUNCTION__,__LINE__);
		NX_APP_Debug("antType(%d), uid(%u), satType(%s), satName(%s), lnbFreq(%u), lnbVoltage(%d), b22kTone(%d), diseqcVersion(%d), diseqcInput(%d), toneBurst(%d), ucMotorPosition(%u), sSatLonggitude(%d) \n",
			antInfo.antType,
			antInfo.uid,
			antInfo.satType,
			antInfo.satName,
			antInfo.info.diseqc.ulLnbFreq,
			antInfo.info.diseqc.lnbVoltage,
			antInfo.info.diseqc.b22kTone,
			antInfo.info.diseqc.diseqcVersion,
			antInfo.info.diseqc.diseqcInput,
			antInfo.info.diseqc.toneBurst,
			antInfo.info.diseqc.ucMotorPosition,
			antInfo.info.diseqc.sSatLonggitude);
	}
	else
	{
		NX_APP_Debug("\n[%s.%d] Failed Load User Ant Info!!! \n\n",__FUNCTION__,__LINE__);
	}

	NX_COMMON_NOT_USED_PARAM(szArgs);
	return HxCMD_OK;
}

static HINT32 cmdRestUserAntenna(void *szArgs)
{
	DxAntInfo_t antInfo;

	HxSTD_MemSet(&antInfo, 0, sizeof(DxAntInfo_t));

	NX_PORT_META_ANTENNA_SaveUserAntenna(0, &antInfo);
	NX_PORT_META_ANTENNA_SaveUserAntenna(1, &antInfo);

	NX_COMMON_NOT_USED_PARAM(szArgs);

	return HxCMD_OK;
}



static void nx_antenna_Command_Init(void)
{
#define cmdHandle		"nxantennacmd"

	HLIB_CMD_RegisterWord(cmdHandle, cmdSaveUserAntenna,
						 (HINT8 *)"saveant",
						 (HINT8 *)"user ant db save test",
						 (HINT8 *)"saveant");

	HLIB_CMD_RegisterWord(cmdHandle, cmdLoadeUserAntenna,
						 (HINT8 *)"loadant",
						 (HINT8 *)"user ant db load test",
						 (HINT8 *)"loadant");
	HLIB_CMD_RegisterWord(cmdHandle, cmdRestUserAntenna,
						 (HINT8 *)"resetant",
						 (HINT8 *)"user ant db reset test",
						 (HINT8 *)"resetant");



}


static void nx_antenna_printAntennaInfo(DxAntInfo_t *pstAntInfo)
{
	DxAntennaType_e		eAntType;

	eAntType = pstAntInfo->antType;

	NX_APP_Debug("\n[%s.%d] ===== [ Antenna Info ] ===================================\n",__FUNCTION__,__LINE__);
	NX_APP_Debug("===== uid : [%u], antType : [%d] =====\n", pstAntInfo->uid, pstAntInfo->antType);
	NX_APP_Debug("===== satType : [%s], satName : [%s] =====\n", pstAntInfo->satType, pstAntInfo->satName);
	if (eAntType == eDxANT_TYPE_LNB_ONLY)
	{
		NX_APP_Debug("===== ulLnbFreq : [%u], lnbVoltage : [%d] =====\n", pstAntInfo->info.lnb.ulLnbFreq, pstAntInfo->info.lnb.lnbVoltage);
	}
	else if (eAntType == eDxANT_TYPE_DISEQC)
	{
		NX_APP_Debug("===== ulLnbFreq : [%u], b22kTone : [%d] =====\n", pstAntInfo->info.diseqc.ulLnbFreq, pstAntInfo->info.diseqc.b22kTone);
		NX_APP_Debug("===== diseqcVersion : [%d], diseqcInput : [%d] =====\n", pstAntInfo->info.diseqc.diseqcVersion, pstAntInfo->info.diseqc.diseqcInput);
		NX_APP_Debug("===== toneBurst : [%d]\n", pstAntInfo->info.diseqc.toneBurst);
	}
	NX_APP_Debug("\n =================================================================\n");
}

#endif		// #if defined(CONFIG_DEBUG)

