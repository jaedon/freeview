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
 * @file	  		nx_port_meta_antenna.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_PORT_META_ANTENNA_H__
#define __NX_PORT_META_ANTENNA_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/

#include <ondk.h>
#include <hapi.h>
#include <papi.h>
#include <hlib.h>
#include <dlib.h>
#include <apk.h>

#include <nx_port_type.h>
#include <nx_port_db.h>

/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/


/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/
#define	NX_PORT_META_ANTENNA_ANTINFO_HEAD					8
#define	NX_PORT_META_ANTENNA_TPINFO_HEAD					7


/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/
typedef enum
{
	eNxSat_ANTENAA_SLOT1 = 1,
	eNxSat_ANTENAA_SLOT2,
	eNxSat_ANTENAA_SLOT3,
	eNxSat_ANTENAA_SLOT4,
	eNxSat_ANTENAA_SLOT5,
	eNxSat_ANTENAA_SLOT6,
	eNxSat_ANTENAA_SLOT7,
	eNxSat_ANTENAA_SLOT8,
	eNxSat_ANTENAA_SLOT_TEST,
	eNxSat_ANTENAA_SLOT_MAX = eNxSat_ANTENAA_SLOT8,
} NX_SAT_AntennaSlot_t;

typedef enum
{
	eNxSat_SEARCH_TP_ALL = 0,
	eNxSat_SEARCH_TP_NONE,
	eNxSat_SEARCH_TP_USERDEFINED,
	eNxSat_SEARCH_TP_DEFAULT,

	eNxSat_SEARCH_TP_TYPE_MAX
} NX_SAT_SearchTpType_t;

typedef struct
{
	/* DVBS Parameters */
	HUINT32 					ulFrequency;			/**< Tuning Frequency, MHz(Unit) */
	HUINT32 					ulSymbolRate;			/**< Symbol Rate, KSymbol/sec */
	DxSat_CodeRate_e 			eFecCodeRate;			/**< FEC Code Rate */
	DxSat_Polarization_e 		ePolarization;			/**< Polarity : Vertical/Horizontal*/

	/* DVBS2 Parameters */
	DxSat_TransportSpec_e		eTransSpec;				/**< DVBS, DVBS2 */
	DxSat_PskModulation_e		ePskMod; 				/**< Modulation Type : QPSK, 8PSK, AUTO*/
	DxSat_Pilot_e				ePilot;					/**< Pilot On/Off/Auto */
	DxSat_RollOff_e				eRollOff;				/**< Roll off : 0.20, 0.25, 0.35 */

	HCHAR						satType[DxANTINFO_NAME_LEN];
	HINT32						antuid;
	HBOOL						bFindSat;
} NX_Sat_TuningTPInfo_t;

typedef union
{
	NX_Sat_TuningTPInfo_t	sat;
	DxCabTuningInfo_t		cab;
	DxTerTuningInfo_t		ter;
	DxNetTuneInfo_t			net;
} NX_TP_TuningInfo_t;

typedef struct tag_SatSrchCh_TpSelectInfo
{
	HINT32					uid;
	HUINT8				 	ucAvailable;
	HUINT8				 	ucUserTpDefined;
	NX_SAT_SearchTpType_t	eNxSearchTpType;
	HUINT32				 	ulTpInfoIndex;
	DxDeliveryType_e		eDeliType;
	NX_TP_TuningInfo_t	 	stNxUserTpInfo;
} Nx_TpSelectInfo_t;


/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/


/*******************************************************************/
/********************	global function prototype   ****************/
/*******************************************************************/
HINT32	NX_PORT_META_ANTENNA_Add (const DxAntInfo_t *antenna);
void	NX_PORT_META_ANTENNA_RemoveAll (void);
void	NX_PORT_META_ANTENNA_SetInfo(HINT32 uid, const DxAntInfo_t *info);
void 	NX_PORT_META_ANTENNA_InfoFree (DxAntInfo_t *p);
HERROR	NX_PORT_META_ANTENNA_Save (void);
HERROR	NX_PORT_META_ANTENNA_Load (void);
HUINT32	NX_PORT_META_ANTENNA_Count (void);
HINT32	NX_PORT_META_ANTENNA_GetUidAt (HINT32 index);
DxAntInfo_t * NX_PORT_META_ANTENNA_GetInfo (HINT32 uid);
HERROR	NX_PORT_META_ANTENNA_GetConnectionType(DxAntennaConnection_e *eAntConnetion);

HUINT32	NX_PORT_META_ANTENNA_GetAntennaUDBCount(void);
HERROR	NX_PORT_META_ANTENNA_SetAntIndexUDB(HINT32 lSlot, HINT32 lAntIdx);
HERROR	NX_PORT_META_ANTENNA_GetAntIndexUDB(HINT32 lSlot, HINT32 *plAntIdx);
HERROR	NX_PORT_META_ANTENNA_SaveUserAntenna(HINT32 slot, DxAntInfo_t *pstAntInfo);
HERROR	NX_PORT_META_ANTENNA_LoadUserAntenna(HINT32 slot, DxAntInfo_t *dstAntInfo);
HERROR 	NX_PORT_META_ANTENNA_SaveUserTp(HINT32 slot, Nx_TpSelectInfo_t *pstTpInfo);
HERROR	NX_PORT_META_ANTENNA_LoadUserTp(HINT32 slot, Nx_TpSelectInfo_t *pstTpInfo);

#endif // __NX_PORT_META_ANTENNA_H__
