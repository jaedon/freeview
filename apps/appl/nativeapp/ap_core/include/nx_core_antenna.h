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
 * @file	  		nx_core_antenna.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_CORE_ANTENNA_H__
#define __NX_CORE_ANTENNA_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/
#include <ondk.h>
#include <hapi.h>
#include <papi.h>
#include <oapi.h>
#include <apk.h>

#include <nx_common.h>
#include <nx_port.h>

#include <nx_core_sat_tp_data.h>


/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/


/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/


/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/
//#define	NX_COM_SAT_DEFAULT_SATELLITE_NAME_EU01			NX_SAT_SATNAME_ASTRA_1
//#define	NX_COM_SAT_DEFAULT_SATELLITE_NAME_EU02			NX_SAT_SATNAME_HOTBIRD
//#define	NX_COM_SAT_DEFAULT_SATELLITE_NAME_ME			NX_SAT_SATNAME_NILESAT_101_102

#define	NX_COM_CH_UNIT_MHZ								1000

// TODO: Add Other Defualt Antenna name


/********************	LNB Parameters		************************/
#define	NX_COM_CH_UNIVERSAL_LNB_FREQ					0xFFFFFFFF
#define	NX_COM_CH_MDU1_LNB_FREQ							(NX_COM_CH_UNIVERSAL_LNB_FREQ - 1)			// for turkish LNB types MDU1 using in digitiruk OP
#define	NX_COM_CH_MDU2_LNB_FREQ							(NX_COM_CH_UNIVERSAL_LNB_FREQ - 2)			// for turkish LNB types MDU2 using in digitiruk OP
#define	NX_COM_CH_MDU3_LNB_FREQ							(NX_COM_CH_UNIVERSAL_LNB_FREQ - 3)			// for turkish LNB types MDU3 using in digitiruk OP
#define	NX_COM_CH_MDU4_LNB_FREQ							(NX_COM_CH_UNIVERSAL_LNB_FREQ - 3)			// for turkish LNB types MDU4 using in digitiruk OP
#define	NX_COM_CH_MDU5_LNB_FREQ							(NX_COM_CH_UNIVERSAL_LNB_FREQ - 4)			// for turkish LNB types MDU5 using in digitiruk OP
#define	NX_COM_CH_MDU_USER_DEFINE						(NX_COM_CH_UNIVERSAL_LNB_FREQ - 5)			// for turkish LNB types User define using in digitiruk OP


/********************	Antenna, Switch Parameters		************************/
// Antenna set
// TODO: 주의!! db_svc.h의 DbSvc_UiAntType_e의 enum과 Sync가 맞아야 한다.
typedef	enum
{
	eNxSat_AntType_LNB_Only	= 0,
	eNxSat_AntType_DiSEqC_1_0,
	eNxSat_AntType_DiSEqC_2_0,
	eNxSat_AntType_Multiswitch,
	eNxSat_AntType_SCD,
	eNxSat_AntType_Motor,
/*	eNxSat_AntType_SMATV,	*/
	eNxSat_AntType_Monoblock_LNB,
	eNxSat_AntType_MAX
} NX_SAT_AntennaType_e;

typedef enum
{
	eNxSat_SatType_USERANT_1 = 1,
	eNxSat_SatType_USERANT_2,
	eNxSat_SatType_USERANT_3,
	eNxSat_SatType_USERANT_4,
	eNxSat_SatType_USERANT_5,
	eNxSat_SatType_USERANT_MAX = eNxSat_SatType_USERANT_5,
} NX_SAT_SatelliteUserAntType_t;


// TODO: 대부분의 위성 정보는 Dlb에 정의된 내용을 사용하다.
// LNB VolTage - DxSAT_LnbVoltage_e
// Diseqc Verion - DxSAT_DiseqcVer_e
// Diseqc Input - DxSAT_DiseqcInput_e

// SCD Input - DxSAT_ScdInput_e
// Tone Burst - DxSAT_ToneBurst_e


/********************	Tuner, Demudulator Parameters	************************/
// Polarization - DxSat_Polarization_e
// CodeRate - DxSat_CodeRate_e
// Transport - DxSat_TransportSpec_e
// PskModulation - DxSat_PskModulation_e
// Pilot - DxSat_Pilot_e
// Roll - DxSat_RollOff_e

/********************	Motor Parameters	************************/
// MotorDirection - DxSat_MOTOR_DIRECTION_e
// MotorDriveMode - DxSat_MOTOR_DRIVEMODE_e


typedef struct
{
	NX_SAT_AntennaType_e	eAntennaType;
	HCHAR					satelliteType[NX_PORT_ANTINFO_NAME_LEN];
	HCHAR					satelliteName[NX_PORT_ANTINFO_NAME_LEN];

	HUINT32					lnb_freq;
	DxSAT_LnbVoltage_e		lnb_voltage;		//	0 : standard, 1 : high

	DxSAT_DiseqcVer_e		diseqc_version;		//	0, 1, 2, 3, 4 (1.0, 1.1, 1.2, 1.3, 2.0)
	HINT32					switch_input;		//	diseqc 1.0, 1.1 : 0 : none, 1 : a, 2 : b, 3 : c, 4 : d
									//	scd             : 0 : none, 1 : a, 2 : b
	HBOOL					enable_22khztone;
	DxSAT_ToneBurst_e		tone_burst;			//	0 : none, 1 : tone_burst_a, 2 : tone_burst_b
	HINT32					position;			//	diseqc 1.2 motor position
	HINT32					longitude;			//	diseqc 1.3 satellite longitude
	HINT32					scd_band_num[NX_PORT_ANTINFO_SCD_SLOT];	//	basically 2 tuner model. MOON_SCD
	HINT32					scd_band_freq[NX_PORT_ANTINFO_SCD_SLOT];
} NX_AntennaInfo_t;

typedef struct
{
	HINT32	nAntNum;
} NX_AntennaManager_t;

// Default Satellite Name Select type
typedef	enum
{
	eNxSat_DefSatType_None = 0,
	eNxSat_DefSatType_EU01,
	eNxSat_DefSatType_EU02,
	eNxSat_DefSatType_ME,
	eNxSat_DefSatType_AMX,
	eNxSat_DefSatType_Max
} NX_SAT_DefaultSatType_e;


/*******************************************************************/
/********************	Private Variables		********************/
/*******************************************************************/

/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/
HERROR 	NX_ANTENNA_Init (void);
HUINT32	NX_ANTENNA_Count (void);
HINT32	NX_ANTENNA_GetId (HINT32 nIndex);
HINT32	NX_ANTENNA_New (void);
HERROR	NX_ANTENNA_RemoveAll (void);
HERROR	NX_ANTENNA_GetAntennaType (HINT32 *lValue);
HERROR	NX_ANTENNA_SetAntennaType (HINT32 lValue);
HERROR	NX_ANTENNA_SetInfo (HINT32 uid, DxAntInfo_t *pstAntInfo);
HERROR	NX_ANTENNA_GetInfo (HINT32 uid, DxAntInfo_t *pstAntInfo);
HERROR	NX_ANTENNA_SetConnectionType (HINT32 n);
HERROR	NX_ANTENNA_GetConnectionType(DxAntennaConnection_e *eAntConnetion);
HERROR	NX_ANTENNA_Sync (void);
HERROR	NX_ANTENNA_Refresh (void);

HERROR	NX_ANTENNA_GetUserTPInfo (HINT32 slot, Nx_TpSelectInfo_t *pstTpInfo);
HERROR	NX_ANTENNA_SetUserTPInfo(HINT32 slot, Nx_TpSelectInfo_t *pstTpInfo);
HINT32	NX_ANTENNA_GetUserAntCountUDB (void);
HERROR	NX_ANTENNA_SetUserAntIndexUDB (HINT32 lSlot, HINT32 lAntIdx);
HERROR	NX_ANTENNA_GetUserAntIndexUDB (HINT32 lSlot, HINT32 *plAntIdx);

HERROR	NX_ANTENNA_LoadAntennaSetting (void);

HCHAR*	NX_ANTENNA_GetDefaultSatelliteName (NX_SAT_DefaultSatType_e eDefaultSatType);
HERROR	NX_ANTENNA_MakeDefaultAntennaInfo (DxAntennaType_e eAntennaType, DxAntInfo_t *pstDefaultAntInfo);
HERROR	NX_ANTENNA_GetTpListBySatelliteTypeName (const HCHAR *szSatTypeName, HUINT32 *pulTpNum, NX_TP_TuningInfo_t **ppstTpInfo);
HERROR	NX_ANTENNA_GetTpBySatelliteTypeName(const HCHAR *szSatTypeName, HUINT32 ulTpIdx, NX_TP_TuningInfo_t *pstTpInfo);
HERROR	NX_ANTENNA_FreeTpListArray (NX_TP_TuningInfo_t *pstTpInfoArray);
void	NX_ANTENNA_GetSatelliteRegionType (NX_SAT_SatelliteRegionType_t *peSatRegionType);
HERROR	NX_ANTENNA_CountRegionSatelliteList (NX_SAT_SatelliteRegionType_t eSatRegionType, HUINT32 *pulSatListNum);
HERROR	NX_ANTENNA_GetRegionSatelliteList (NX_SAT_SatelliteRegionType_t eSatRegionType, Nx_SAT_SatRegionList_t **ppstSatList, HUINT32 *pulSatListNum);
HERROR	NX_ANTENNA_GetSatellitePositionString (HCHAR *szSatName, HUINT8 *szSatPosStr);
HERROR	NX_ANTENNA_GetSatelliteNameWithSatPosition (HCHAR *szSatTypeName, HCHAR *szDispSatName, HUINT8 *szSatFullName);
void	NX_ANTENNA_MakeTpString (DxDeliveryType_b ulDeliType, NX_TP_TuningInfo_t *tp, DxNetwork_t *pstNetwork, HUINT8 *pString);
HINT32	NX_ANTENNA_FindAssignAntennaUid(void);

#endif // __NX_CORE_ANTENNA_H__

