/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pal_channel_cab.h
	@brief	  Octo pal channel �� DVBT ���� �Ķ���� header

	Description: MW channel�� PAL channel���� ����ϴ� channel standard �� DVBT�� ���� �Ķ���͸� ������ �������.\n
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

