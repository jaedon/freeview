/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pal_channel_cab.h
	@brief	  Octo pal channel 의 DVBT 관련 파라미터 header

	Description: MW channel과 PAL channel에서 사용하는 channel standard 중 DVBT에 관한 파라미터를 정의한 헤더파일.\n
	Module: PAL/CH			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__PAL_CHANNEL_CAB_H__
#define	__PAL_CHANNEL_CAB_H__

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
/********************      	API's         **************************/
/*******************************************************************/
extern HERROR PAL_CH_CAB_StartTune(
								HUINT32					ulTunerGroupId,
								HUINT32					ulRequestId,
								HUINT32 				ulFrequency,
								HUINT32  				ulSymbolRate,
								DxCab_Constellation_e  	constellation,
								DxCab_Spectrum_e   		spectrum,
								HINT32 					usTsId,		/* Only for Japanese cable */
								HINT32 					usNetId		/* Only for Japanese cable */
								);

extern HERROR PAL_CH_CAB_StopTune(HUINT32 ulTunerGroupId);

extern HERROR PAL_CH_CAB_GetLockedInfo(
								HUINT32				ulTunerGroupId,
								DxCab_Constellation_e	*pActualConstellation,
								DxCab_Spectrum_e   	*pActualSpectrum,
								HUINT32				*pulActulaSymbolRate
								);

extern HERROR PAL_CH_CAB_GetSignalInfo(
								HUINT32			ulTunerGroupId,
								HUINT32			*pulSignalLevel,
								HUINT32			*pulSignalQuality,
								HUINT32			*pulAgc,
								float			*pfBer,
								float			*pfSnr,
								float			*pfSignalInputPower
								);

HERROR PAL_CH_CAB_GetSignalInfoForLogCapture(
								HUINT32			ulTunerGroupId,
								float			*pfBer,
								float			*pfSnr,
								HUINT32			*pulUnCorrectedNo,
								float			*pfSignalInputPower
								);

extern HERROR PAL_CH_CAB_MakeCabTuneParam (
								HUINT32 				ulFrequency,
								HUINT32 				ulSymbolRate,
								DxCab_Constellation_e 	constellation,
								DxCab_Spectrum_e			spectrum,
								HUINT32 				*pulDataSize,
								void					**ppvDiParam);

#endif /* __PAL_CHANNEL_CAB_H__*/

