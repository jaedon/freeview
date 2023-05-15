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
 * @file	  		nx_core_sat_tp_sata.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_CORE_SAT_TP_DATA_H__
#define __NX_CORE_SAT_TP_DATA_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/
#include 	<ondk.h>
#include	<hapi.h>
#include	<papi.h>

#include	<oapi.h>
#include	<apk.h>

#include <nx_port_meta_antenna.h>

/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/


/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/

// TODO: sattype_map the name of otl_conv_common.c are the same

#define	NX_SAT_SATNAME_AMOS_2_3							"AMOS_2&3"
#define	NX_SAT_SATNAME_APSTAR_2R						"APSTAR_2R"
#define	NX_SAT_SATNAME_ARABSAT_5A						"ARABSAT_5A"
#define	NX_SAT_SATNAME_ASIASAT_3S						"ASIASAT_3S"
#define	NX_SAT_SATNAME_ASIASAT_5						"ASIASAT_5"
#define	NX_SAT_SATNAME_ASTRA_1							"ASTRA_1"
#define	NX_SAT_SATNAME_ASTRA_2							"ASTRA_2"
#define	NX_SAT_SATNAME_ASTRA_3A_3B						"ASTRA_3A&3B"
#define	NX_SAT_SATNAME_ASTRA_4A							"ASTRA_4A"
#define	NX_SAT_SATNAME_ATLANTIC_BIRD_1					"ATLANTIC_BIRD_1"
#define	NX_SAT_SATNAME_ATLANTIC_BIRD_2					"ATLANTIC_BIRD_2"
#define	NX_SAT_SATNAME_ATLANTIC_BIRD_3					"ATLANTIC_BIRD_3"
#define	NX_SAT_SATNAME_BADR_4_6							"BADR_4&6"
#define	NX_SAT_SATNAME_EUROBIRD_1						"EUROBIRD_1"		//Eurobird 1 -> Eutelsat 28A -> Eutelsat 33C
#define	NX_SAT_SATNAME_EUROBIRD_3						"EUROBIRD_3"		//EUROBIRD 3 -> Eutelsat 33B/33C
#define	NX_SAT_SATNAME_EUROBIRD_9A						"EUROBIRD_9A"		//Eurobird 9A -> Eutelsat 9A
#define	NX_SAT_SATNAME_EUTELSAT_W2A						"EUTELSAT_W2A"		//Eutelsat W2A -> Eutelsat 10A
#define	NX_SAT_SATNAME_EUTELSAT_W3A						"EUTELSAT_W3A"		//Eutelsat W3A -> Eutelsat 7A
#define	NX_SAT_SATNAME_EUTELSAT_W4_W7					"EUTELSAT_W4&W7"	//Eutelsat W7 -> Eutelsat 36B
//#define	NX_SAT_SATNAME_EUTELSAT_W4_W7					"EUTELSAT_W4&W7"	//Eutelsat W4 -> Eutelsat 36A
#define	NX_SAT_SATNAME_EUTELSAT_W6						"EUTELSAT_W6"	//EUTELSAT W6 -> Eutelsat 21B
#define	NX_SAT_SATNAME_EXPRESS_AM1						"EXPRESS_AM1"
#define	NX_SAT_SATNAME_EXPRESS_AM22						"EXPRESS_AM22"
#define	NX_SAT_SATNAME_HELLAS_SAT2						"HELLAS_SAT2"
#define	NX_SAT_SATNAME_HISPASAT_1C_1D					"HISPASAT_1C&1D"
#define	NX_SAT_SATNAME_HOTBIRD							"HOTBIRD"
#define	NX_SAT_SATNAME_INSAT_3A_4B						"INSAT_3A&4B"
#define	NX_SAT_SATNAME_INTELSAT_10_02					"INTELSAT_10-02"
#define	NX_SAT_SATNAME_INTELSAT_12						"INTELSAT_12"
#define	NX_SAT_SATNAME_INTELSAT_7_10					"INTELSAT_7&10"
#define	NX_SAT_SATNAME_INTELSAT_901						"INTELSAT_901"
#define	NX_SAT_SATNAME_INTELSAT_902						"INTELSAT_902"
#define	NX_SAT_SATNAME_INTELSAT_904						"INTELSAT_904"
#define	NX_SAT_SATNAME_INTELSAT_906						"INTELSAT_906"
#define	NX_SAT_SATNAME_INTELSAT_907						"INTELSAT_907"
#define	NX_SAT_SATNAME_NILESAT_101_102					"NILESAT_101&102"
#define	NX_SAT_SATNAME_NSS_12							"NSS_12"
#define	NX_SAT_SATNAME_NSS_7							"NSS_7"
#define	NX_SAT_SATNAME_TELSTAR_12						"TELSTAR_12"
#define	NX_SAT_SATNAME_THOR_5_6							"THOR_5&6"
#define	NX_SAT_SATNAME_TURKSAT_2A_3A					"TURKSAT_2A&3A"
#define	NX_SAT_SATNAME_YAMAL_201						"YAMAL_201"
#define	NX_SAT_SATNAME_ARABSAT_2B						"ARABSAT_2B"
#define	NX_SAT_SATNAME_EUTELSAT_W1						"EUTELSAT_W1"
#define	NX_SAT_SATNAME_EUTELSAT_W2						"EUTELSAT_W2"
#define	NX_SAT_SATNAME_NSS_703							"NSS_703"
#define	NX_SAT_SATNAME_SIRIUS							"SIRIUS"
#define	NX_SAT_SATNAME_TELECOM_2D						"TELECOM_2D"
#define	NX_SAT_SATNAME_EUROBIRD_9						"EUROBIRD_9"
#define	NX_SAT_SATNAME_INTELSAT_802						"INTELSAT_802"
#define	NX_SAT_SATNAME_THOR_3_5							"THOR_3_5"
#define	NX_SAT_SATNAME_INSAT_2E4A						"INSAT_2E4A"
#define	NX_SAT_SATNAME_INTELSAT_17						"INTELSAT_17"
#define	NX_SAT_SATNAME_YAHLIVE							"YAHLIVE"
#define	NX_SAT_SATNAME_HISPASAT_1D						"HISPASAT_1D"
#define	NX_SAT_SATNAME_HISPASAT_1E						"HISPASAT_1E"
#define	NX_SAT_SATNAME_SES_2							"SES_2"
#define	NX_SAT_SATNAME_SES_3							"SES_3"
#define	NX_SAT_SATNAME_SES_6							"SES_6"
#define	NX_SAT_SATNAME_EUTELSAT_5_WEST_A				"EUTELSAT_5_WEST_A"
#define	NX_SAT_SATNAME_EUTELSAT_12_WEST_B				"EUTELSAT_12_WEST_B"
#define	NX_SAT_SATNAME_EUTELSAT_113_WEST_A				"EUTELSAT_113_WEST_A"
#define	NX_SAT_SATNAME_EUTELSAT_117_WEST_A				"EUTELSAT_117_WEST_A"
#define	NX_SAT_SATNAME_EUTELSAT_HOTBIRD_13B				"EUTELSAT_HOTBIRD_13B"
#define	NX_SAT_SATNAME_EUTELSAT_HOTBIRD_13C				"EUTELSAT_HOTBIRD_13C"
#define	NX_SAT_SATNAME_EUTELSAT_HOTBIRD_13D				"EUTELSAT_HOTBIRD_13D"
#define	NX_SAT_SATNAME_INTELSAT_1R						"INTELSAT_1R"
#define	NX_SAT_SATNAME_INTELSAT_11						"INTELSAT_11"
#define	NX_SAT_SATNAME_INTELSAT_14						"INTELSAT_14"
#define	NX_SAT_SATNAME_INTELSAT_21						"INTELSAT_21"
#define	NX_SAT_SATNAME_INTELSAT_23						"INTELSAT_23"
#define	NX_SAT_SATNAME_INTELSAT_34						"INTELSAT_34"
#define	NX_SAT_SATNAME_INTELSAT_903						"INTELSAT_903"
#define	NX_SAT_SATNAME_GALAXY_18						"GALAXY_18"
#define	NX_SAT_SATNAME_GALAXY_19						"GALAXY_19"
#define	NX_SAT_SATNAME_GALAXY_28						"GALAXY_28"
#define	NX_SAT_SATNAME_SPACEWAY_1_AND_DIRECTV_10_12		"SPACEWAY_1_AND_DIRECTV_10_02"
#define	NX_SAT_SATNAME_AMAZONAS_2						"AMAZONAS_2"
#define	NX_SAT_SATNAME_AMAZONAS_3						"AMAZONAS_3"
#define	NX_SAT_SATNAME_AMC_6							"AMC_6"
#define	NX_SAT_SATNAME_AMC_8							"AMC_8"
#define	NX_SAT_SATNAME_AMC_15							"AMC_15"
#define	NX_SAT_SATNAME_AMC_18							"AMC_18"
#define	NX_SAT_SATNAME_AMC_21							"AMC_21"
#define	NX_SAT_SATNAME_BSAT_3C_JCSAT_110R				"BSAT_3C_JCSAT_110R"
#define	NX_SAT_SATNAME_N_SAT_110						"N_SAT_110"
#define	NX_SAT_SATNAME_TELSTAR_12_VANTAGE				"TELSTAR_12_VANTAGE"
#define	NX_SAT_SATNAME_TELSTAR_14R						"TELSTAR_14R"
#define	NX_SAT_SATNAME_ST_2								"ST_2"
#define	NX_SAT_SATNAME_SIMON_BOLIVAR					"SIMON_BOLIVAR"
#define	NX_SAT_SATNAME_STAR_ONE_C1						"STAR_ONE_C1"
#define	NX_SAT_SATNAME_STAR_ONE_C2						"STAR_ONE_C2"
#define	NX_SAT_SATNAME_STAR_ONE_C4						"STAR_ONE_C4"
#define	NX_SAT_SATNAME_BRASILSAT_B4						"BRASILSAT_B4"
#define	NX_SAT_SATNAME_NSS_10							"NSS_10"
#define	NX_SAT_SATNAME_USERDEFINE						"User_Define"
#define	NX_SAT_SATNAME_USERDEFINE1						"User_Define_1"
#define	NX_SAT_SATNAME_USERDEFINE2						"User_Define_2"
#define	NX_SAT_SATNAME_USERDEFINE3						"User_Define_3"
#define	NX_SAT_SATNAME_USERDEFINE4						"User_Define_4"
#define	NX_SAT_SATNAME_USERDEFINE5						"User_Define_5"
#define	NX_SAT_SATNAME_NONE								"NONE"


/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/

typedef enum
{
	eNxSat_SatRegion_Normal = 0,
	eNxSat_SatRegion_MeSat,
	eNxSat_SatRegion_SouthAmerica,
	// TODO: Add here

	eNxSat_SatRegion_MAX
} NX_SAT_SatelliteRegionType_t;

typedef struct
{
	HUINT32						ulFrequency;
	HUINT32						ulSymbolRate;
	DxSat_Polarization_e		ePolarization;
	DxSat_CodeRate_e			eFecCodeRate;
	DxSat_PskModulation_e		ePskMod;
	DxSat_TransportSpec_e		eTransSpec;
	HBOOL						bFindSat;
} Nx_SAT_TunningInfo_t;

typedef struct {
	HCHAR						*szSatName;
	HUINT32						ulTpNum;
	const Nx_SAT_TunningInfo_t	*pstTpArray;
} Nx_SAT_DefaultTpList_t;

typedef struct {
	HINT32				lSatIdx;
	HCHAR				*szSatTypeName;
} Nx_SAT_SatRegionList_t;

typedef	struct
{
	HCHAR		*szSatName;
	HINT32		lPosNum;		/**< predefined satellite diseqc position in positioner */
	HINT16		sSatDegree;		/**< satellite's degree, ex) 120.5E : 1205, 55.0W : -550 ==> East is a positive value, West a negetive */
	HCHAR		*szLongitude;   /**< sSatDegree's text */
} Nx_SAT_SatPosInfo_t;


/*******************************************************************/
/********************	Private Variables		********************/
/*******************************************************************/


/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/

HERROR	NX_ANTENNA_CountDefaultTpBySatTypeName(const HCHAR *szSatName, HUINT32 *pulNumberOfTps);
HERROR	NX_ANTENNA_GetDefaultTpBySatTypeName(const HCHAR *szSatName, HUINT32 ulTpIdx, NX_TP_TuningInfo_t *pstTpInfo);
HERROR	NX_ANTENNA_GetSatDegree(HCHAR *szSatName, HINT16 *psSatDegree);
Nx_SAT_SatRegionList_t *NX_ANTENNA_GetRegionSatList (NX_SAT_SatelliteRegionType_t eSatRegionType, HUINT32 *ulNum);

#endif	//__NX_CORE_SAT_TP_DATA_H__

