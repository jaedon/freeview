/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pmgr_time.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___PMGR_TIME_H___
#define	___PMGR_TIME_H___


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <mgr_time.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eREF_SAT_TP,
	eREF_TER_TP,
	eREF_FIRST_SVC_SAT,
	eREF_FIRST_SVC_TER,
	eREF_FIRST_SVC_CAB,
	eREF_WAKEUP_SVC,
	eREF_LAST_SVC,
	eREF_END
} MgrTime_ClockRecoverRefMethod_e;

typedef struct
{
	MgrTime_ClockRecoverRefMethod_e		eRefMethod;				/**< Method for clock setup */

#if defined(CONFIG_MW_CH_SATELLITE)
	SVC_SatType_t						eRefSatType;			/**< Satellite ID in case of satellite TP */
	HUINT32								ulFrequency;			/**< Tuning Frequency, KHz(Unit) */
	HUINT32								ulSymbolRate;			/**< Symbol Rate, KSymbol/sec */
	DxSat_CodeRate_e						fecCodeRate;			/**< FEC Code Rate */
	DxSat_Polarization_e					polarization;			/**< Polarity : Vertical/Horizontal*/
	DxSat_TransportSpec_e					transSpec;				/**< DVBS, DVBS2 */
	DxSat_PskModulation_e 				pskMod;					/**< Modulation Type : QPSK, 8PSK, AUTO*/
	DxSat_Pilot_e							pilot;					/**< Pilot On/Off/Auto */
	DxSat_RollOff_e						rollOff;				/**< Roll off : 0.20, 0.25, 0.35 */
#endif
#if defined(CONFIG_MW_CH_TERRESTRIAL)
	HUINT32 					ulRefTerType;			/**< Terrestrial Search Type */
	HUINT32 					ulTer_Frequency;			/**< Tuning Frequency, KHz(Unit) */
	DxTer_BandWidth_e			eBandWidth;
	DxTer_Constellation_e 		eTer_constellation;
	DxTer_TransMode_e 			eTransMode;
	DxTer_Stream_e				eStream;
	DxTer_System_e				eSystem;
#if defined(CONFIG_MW_CH_DVB_T2)
	HUINT32						ulPlpId;
	DxTer_PreambleFormat_e		ucPreambleFormat;
	DxTer_Mixed_e				ucMixed;
	DxTer_Pilot_Pattern_e		ucPilotPattern;
	DxTer_ExtendedCarrier_e		ucExtenedCarrier;
	DxTer_PAPR_Reduction_e		ucPAPRreduction;
	HUINT32						ulNumPlpBlocks;
#endif
	HUINT16 					usLcn;
	HINT32						nTer_TsId;
	HINT32						nTer_OnId;					/**< Original Network ID */
#endif

} MgrTime_ClockRecoverRefTpInfo_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Plug-In Functions    ********************/
/*******************************************************************/
HERROR pmgr_time_Init (void);
HERROR pmgr_time_ClockRecoverGetDstMode (VK_CLOCK_DST_MODE *peDstMode);
MgrTime_ClockRecoverRefTpInfo_t *pmgr_time_ClockRecoverGetRefTpList (void);
HERROR pmgr_time_ClockRecoverSearchByFirstSvc (MgrTime_ClockRecoverRefMethod_e eRefMethod, DxTuneParam_t *tuningInfo);
HERROR pmgr_time_SetGmtOffset(HINT32 nGmtOffset, HBOOL bUpdateTzFile);
HERROR pmgr_time_ClockRecoverSearchByTerTp (HUINT32 ulRefTpIdx, DxTuneParam_t	*tuningInfo);

#endif /* !___PMGR_TIME_H___ */

