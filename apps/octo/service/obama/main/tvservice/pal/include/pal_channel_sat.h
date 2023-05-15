/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pal_channel_sat.h
	@brief	  Octo pal channel 의 DVBS2 관련 파라미터 header

	Description: MW channel과 PAL channel에서 사용하는 channel standard 중 DVBS2에 관한 파라미터를 정의한 헤더파일.\n
	Module: PAL/CH			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__PAL_CHANNEL_SAT_H__
#define	__PAL_CHANNEL_SAT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define PAL_MAX_SATTUNER_NO					8
#define PAL_MAX_RFINPUT_NO					8

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HUINT32				 ulTunerGroupId;

	HUINT32				 ulNumRfInput;
	HUINT32				 aulRfInputId[PAL_MAX_RFINPUT_NO];
	HBOOL				 bLoopThroughOut;
} PalCh_SatTunerCapacity_t;

typedef struct
{
	HBOOL						 bAvailable;

	// Connectable Tuner:
	HUINT32						 ulConnectableTunerNum;
	HUINT32						 aulConnectableTunerId[PAL_MAX_SATTUNER_NO];

	// Connected Tuner:
	HUINT32						 ulConnectedTunerNum;
	HUINT32						 aulConnectedTunerId[PAL_MAX_SATTUNER_NO];
} PalCh_SatRfInput_t;


/*******************************************************************/
/********************      	API's         **************************/
/*******************************************************************/

extern HERROR PAL_CH_SAT_Init (void);

extern HERROR PAL_CH_SAT_GetTunerCapacity (HUINT32 ulTunerGroupId, PalCh_SatTunerCapacity_t *pstSatTunerCap);

extern HERROR PAL_CH_LNB_StartTune(
									HUINT32					ulTunerGroupId,
									HUINT32 				ulRequestId,
									HUINT32					ulFrequency,
									HUINT32					ulSymbolRate,
									DxSat_CodeRate_e			fecCodeRate,
									DxSat_Polarization_e		polarization,
									DxSat_TransportSpec_e		transSpec,
									DxSat_PskModulation_e 	pskMod,
									DxSat_Pilot_e				pilot,
									HUINT32 				ulLnbFreq,
									DxSAT_LnbVoltage_e 		lnbVoltage
									);

extern HERROR PAL_CH_DISEQC_StartTune(
									HUINT32					ulTunerGroupId,
									HUINT32					ulRequestId,
									HUINT32					ulFrequency,
									HUINT32					ulSymbolRate,
									DxSat_CodeRate_e			fecCodeRate,
									DxSat_Polarization_e		polarization,
									DxSat_TransportSpec_e		transSpec,
									DxSat_PskModulation_e 	pskMod,
									DxSat_Pilot_e				pilot,
									HUINT32 				ulLnbFreq,
									DxSAT_LnbVoltage_e 		lnbVoltage,
									HBOOL					b22kTone,
									DxSAT_DiseqcVer_e			diseqcVersion,
									DxSAT_DiseqcInput_e		diseqcInput,
									DxSAT_ToneBurst_e			toneBurst,
									HUINT8					ucMotorPosition,
									HINT16					sSatLonggitude
									);


extern HERROR PAL_CH_SCD_StartTune(
									HUINT32 				ulTunerGroupId,
									HUINT32 				ulRequestId,
									HUINT32 				ulFrequency,
									HUINT32 				ulSymbolRate,
									DxSat_CodeRate_e			fecCodeRate,
									DxSat_Polarization_e		polarization,
									DxSat_TransportSpec_e 	transSpec,
									DxSat_PskModulation_e 	pskMod,
									DxSat_Pilot_e 			pilot,
									HUINT32 				ulLnbFreq,
									DxSAT_LnbVoltage_e		lnbVoltage,
									HBOOL					b22kTone,
									DxSAT_ScdInput_e			scdInput,
									HUINT32 				ulScdUserBandNum,
									HUINT32 				ulScdUserBandFreq
									);

extern HERROR PAL_CH_SMATV_StartTune(
									HUINT32					ulTunerGroupId,
									HUINT32 				ulRequestId,
									HUINT32					ulFrequency,
									HUINT32					ulSymbolRate,
									DxSat_CodeRate_e			fecCodeRate,
									DxSat_Polarization_e		polarization,
									DxSat_TransportSpec_e		transSpec,
									DxSat_PskModulation_e 	pskMod,
									DxSat_Pilot_e				pilot
									);

extern HERROR PAL_CH_SAT_StopTune(HUINT32 ulTunerGroupId);

extern HERROR PAL_CH_SAT_SendDiseqcCommand(
									HUINT8					ulTunerGroupId,
									HUINT8					*aucCommand,
									HUINT32					ulLength
									);

extern HUINT32 PAL_CH_LNB_GetActualLnbFreq(HUINT32 ulTpFreq);
extern HUINT32 PAL_CH_LNB_GetActualMduLnbFreq(HUINT32 ulLnbFreq, DxSat_Polarization_e ePol, HUINT32 ulFreq);
extern HBOOL PAL_CH_LNB_GetActual22KTone(HUINT32 ulTpFreq);

extern HERROR PAL_CH_LNB_GetLockedInfo(
									HUINT32 				ulTunerGroupId,
									HUINT32 				ulTpFreq,
									HUINT32 				ulLnbFreq,
									HUINT32 				*pulActualTpFreq, // MHz
									DxSat_Polarization_e		*pActualPolar,
									DxSat_CodeRate_e			*pActualCodeRate,
									DxSat_PskModulation_e		*pActualPskMod,
									DxSat_Pilot_e 			*pActualPilot
									);

extern HERROR PAL_CH_SMATV_GetLockedInfo(
									HUINT32					ulTunerGroupId,
									HUINT32					*pulActualTpFreq, // MHz
									DxSat_Polarization_e		*pActualPolar,
									DxSat_CodeRate_e			*pActualCodeRate,
									DxSat_PskModulation_e		*pActualPskMod,
									DxSat_Pilot_e				*pActualPilot
									);

extern HERROR PAL_CH_SAT_GetSignalInfo(
									HUINT32					ulTunerGroupId,
									HUINT32					*pulSignalLevel,
									HUINT32					*pulSignalQuality,
									HUINT32					*pulAgc,
									float					*pfBer,
									float					*pfSnr,
									float					*pfSignalInputPower
									);

extern HERROR PAL_CH_MOTOR_Stop(HUINT32 ulTunerGroupId);
extern HERROR PAL_CH_MOTOR_GotoPosition(HUINT32 ulTunerGroupId, HUINT8 ucPosition);
extern HERROR PAL_CH_MOTOR_StorePosition(HUINT32 ulTunerGroupId, HUINT8 ucPosition);
extern HERROR PAL_CH_MOTOR_Drive(HUINT32 ulTunerGroupId, CH_MotorDirection_t direction, CH_MotorDriveMode_t driveMode, HUINT8 ucValue);
extern HERROR PAL_CH_MOTOR_SetLimit(HUINT32 ulTunerGroupId, CH_MotorDirection_t direction);
extern HERROR PAL_CH_MOTOR_DisableLimit(HUINT32 ulTunerGroupId);
extern HERROR PAL_CH_MOTOR_EnableLimit(HUINT32 ulTunerGroupId);
extern HERROR PAL_CH_MOTOR_Recalculate(HUINT32 ulTunerGroupId, HUINT8 ucPosition);
extern HERROR PAL_CH_MOTOR_GotoDegree(HUINT32 ulTunerGroupId, HINT16 sDegree);
extern HERROR PAL_CH_MOTOR_GotoSatellite(HUINT32 ulTunerGroupId, HINT16 sSatPos);

extern HERROR PAL_CH_LNB_MakeSatTuneParam (
									HUINT32							ulFrequency,
									HUINT32							ulSymbolRate,
									DxSat_CodeRate_e					fecCodeRate,
									DxSat_Polarization_e				polarization,
									DxSat_TransportSpec_e				transSpec,
									DxSat_PskModulation_e 			pskMod,
									DxSat_Pilot_e						pilot,
									HUINT32 						ulLnbFreq,
									DxSAT_LnbVoltage_e 				lnbVoltage,
									HUINT32							*pulDataSize,
									void							**ppvDiParam
									);

extern HERROR PAL_CH_DISEQC_MakeSatTuneParam (
									HUINT32							ulFrequency,
									HUINT32							ulSymbolRate,
									DxSat_CodeRate_e					fecCodeRate,
									DxSat_Polarization_e				polarization,
									DxSat_TransportSpec_e				transSpec,
									DxSat_PskModulation_e 			pskMod,
									DxSat_Pilot_e						pilot,
									HUINT32 						ulLnbFreq,
									DxSAT_LnbVoltage_e 				lnbVoltage,
									HBOOL							b22kTone,
									DxSAT_DiseqcVer_e					diseqcVersion,
									DxSAT_DiseqcInput_e				diseqcInput,
									DxSAT_ToneBurst_e					toneBurst,
									HUINT8							ucMotorPosition,
									HINT16							sSatLonggitude,
									HUINT32 						*pulDataSize,
									void							**ppvDiParam
									);

extern HERROR PAL_CH_SCD_MakeSatTuneParam (
									HUINT32 						ulFrequency,
									HUINT32 						ulSymbolRate,
									DxSat_CodeRate_e					fecCodeRate,
									DxSat_Polarization_e				polarization,
									DxSat_TransportSpec_e 			transSpec,
									DxSat_PskModulation_e 			pskMod,
									DxSat_Pilot_e 					pilot,
									HUINT32 						ulLnbFreq,
									DxSAT_LnbVoltage_e				lnbVoltage,
									HBOOL							b22kTone,
									DxSAT_ScdInput_e					scdInput,
									HUINT32 						ulScdUserBandNum,
									HUINT32 						ulScdUserBandFreq,
									HUINT32 						*pulDataSize,
									void							**ppvDiParam
									);

extern HERROR PAL_CH_SMATV_MakeSatTuneParam (
									HUINT32							ulFrequency,
									HUINT32							ulSymbolRate,
									DxSat_CodeRate_e					fecCodeRate,
									DxSat_Polarization_e				polarization,
									DxSat_TransportSpec_e				transSpec,
									DxSat_PskModulation_e 			pskMod,
									DxSat_Pilot_e						pilot,
									HUINT32 						*pulDataSize,
									void							**ppvDiParam
									);

extern HERROR PAL_CH_SAT_SetRfInputConnection (HUINT32 ulTunerGroupId, HUINT32 ulRfInputId);
extern HERROR PAL_CH_SAT_ResetRfInputConnection (HUINT32 ulTunerGroupId);

extern HERROR PAL_CH_SAT_SetAntennaType(HUINT32 ulTunerGroupId, DxAntennaConnection_e eAntType);

extern HUINT32	PAL_CH_SAT_CountRfInput (void);

extern HERROR	PAL_CH_SAT_GetRfInput (HUINT32 ulRfInputIdx, PalCh_SatRfInput_t *pstRfInput);

#endif /* __PAL_CHANNEL_SAT_H__*/

