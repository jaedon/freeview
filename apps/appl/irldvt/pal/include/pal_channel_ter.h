/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pal_channel_ter.h
	@brief	  Octo pal channel 의 DVBT 관련 파라미터 header

	Description: MW channel과 PAL channel에서 사용하는 channel standard 중 DVBT에 관한 파라미터를 정의한 헤더파일.\n
	Module: PAL/CH			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__PAL_CHANNEL_TER_H__
#define	__PAL_CHANNEL_TER_H__

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


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      API's         *************************/
/*******************************************************************/
HERROR PAL_CH_TER_StartTune(
							HUINT32					ulTunerGroupId,
							HUINT32					ulRequestId,
							HUINT32					ulFrequency,
							DxTer_BandWidth_e			bandWidth,
							DxTer_Constellation_e		constellation,
							DxTer_Hierachy_e			hierachy,
							DxTer_CodeRate_e			codeRate,
							DxTer_TransMode_e			transMode,
							DxTer_GuardInterval_e		guardInterval,
							DxTer_Stream_e			stream,
							DxTer_Offset_e			offset,
							DxTer_System_e			system,
							DxTer_PreambleFormat_e			preambleFormat,
							DxTer_Mixed_e						mixed,
							DxTer_Pilot_Pattern_e				pilotPattern,
							DxTer_ExtendedCarrier_e			extenedCarrier,
							DxTer_PAPR_Reduction_e			PAPRreduction,
							HUINT32							ulPlpId,
							HUINT32							numPlpBlocks
						);

extern HERROR PAL_CH_TER_StopTune(HUINT32 ulTunerGroupId);

extern HERROR PAL_CH_TER_GetDataPlpCount(
								HUINT32			 ulTunerGroupId,
								HUINT8			*pucNumPlps
								);

extern HERROR PAL_CH_TER_GetSignalInfo(
								HUINT32			 ulTunerGroupId,
								HUINT32			*pulSignalLevel,
								HUINT32			*pulSignalQuality,
								HUINT32			*pulAgc,
								float			*pfBer,
								float			*pfSnr,
								float			*pfSignalInputPower
								);

extern void PAL_CH_TER_SetAntennaPower(
								HUINT32			 ulTunerGroupId,
								HBOOL			 bOnOff
								);

extern HERROR PAL_CH_TER_GetAntennaPower(
								HUINT32 		 ulTunerGroupId,
								HBOOL 			*pbOnOff);

extern HERROR PAL_CH_TER_GetLockedInfo(
								HUINT32					ulTunerGroupId,
								HUINT32 				*pulActualFreq, // KHz
								DxTer_BandWidth_e 		*pActualBandWidth,
								DxTer_Constellation_e 	*pActualConstellation,
								DxTer_Hierachy_e			*pActualHierachy,
								DxTer_CodeRate_e			*pActualCodeRate,
								DxTer_TransMode_e 		*pActualTransMode,
								DxTer_GuardInterval_e 	*pActualGuardInterval,
								DxTer_Offset_e			*pActualOffset,
								DxTer_System_e			*pSystem,
								DxTer_PreambleFormat_e	*pPreambleFormat,
								DxTer_Mixed_e				*pMixed,
								DxTer_Pilot_Pattern_e		*pPilotPattern,
								DxTer_ExtendedCarrier_e	*pExtenedCarrier,
								DxTer_PAPR_Reduction_e	*pPAPRreduction,
								HUINT32					*pPlpId,
								HUINT32					*pNumPlpBlocks
								);


extern HERROR PAL_CH_TER_GetDataPlpInfo( HUINT32 ulTunerGroupId,  HUINT8 *pucPlpIds, HUINT8 *pucNumPlps );

extern HERROR PAL_CH_TER_MakeTerTuneParam(
							HUINT32					ulFrequency,
							DxTer_BandWidth_e			bandWidth,
							DxTer_Constellation_e		constellation,
							DxTer_Hierachy_e			hierachy,
							DxTer_CodeRate_e			codeRate,
							DxTer_TransMode_e			transMode,
							DxTer_GuardInterval_e		guardInterval,
							DxTer_Stream_e			stream,
							DxTer_Offset_e			offset,
							DxTer_System_e			system,
							HUINT32 				plpId,
							DxTer_PreambleFormat_e	preambleFormat,
							DxTer_Mixed_e				mixed,
							DxTer_Pilot_Pattern_e		pilotPattern,
							DxTer_ExtendedCarrier_e	extenedCarrier,
							DxTer_PAPR_Reduction_e	pAPRreduction,
							HUINT32					numPlpBlocks,
							HUINT32					*pulDataSize,
							void					**ppvDiParam
							);

extern HERROR PAL_CH_TER_GetTunerCellId(HUINT32 ulTunerGroupId, HUINT16 *pCellId);
#endif /* __PAL_CHANNEL_TER_H__*/

