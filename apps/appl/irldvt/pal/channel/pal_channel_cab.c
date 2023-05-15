/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pal_channel_cab.c
	@brief	  Octo pal channel cable part

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <pal_channel.h>

#include <di_channel.h>
#include <di_channel_c.h>

/*******************************************************************/
/************************     Definition   * ***********************/
/*******************************************************************/



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/

#define _____________ConvPAL2DI_CAB__________________
static CH_CAB_Modulation_e pal_ch_cab_ConvPAL2DI_Constellation(DxCab_Constellation_e contellation)
{
	switch (contellation)
	{
		case eDxCAB_CONSTELLATION_16QAM:	return DI_CH_CMOD_16QAM;
		case eDxCAB_CONSTELLATION_32QAM:	return DI_CH_CMOD_32QAM;
		case eDxCAB_CONSTELLATION_64QAM:	return DI_CH_CMOD_64QAM;
		case eDxCAB_CONSTELLATION_128QAM:	return DI_CH_CMOD_128QAM;
		case eDxCAB_CONSTELLATION_256QAM:	return DI_CH_CMOD_256QAM;
		case eDxCAB_CONSTELLATION_AUTO:	return DI_CH_CMOD_AUTO;
		default:
			HxLOG_Critical("\n\n");
			return 0xff;
	}
}

static CH_CAB_Spectrum_e pal_ch_cab_ConvPAL2DI_Sprectrum(DxCab_Spectrum_e spectrum)
{
	switch (spectrum)
	{
		case eDxCAB_SPECTRUM_NORMAL:		return DI_CH_INVERSION_NORMAL;
		case eDxCAB_SPECTRUM_INVERTED:		return DI_CH_INVERSION_INVERTED;
		case eDxCAB_SPECTRUM_AUTO:			return DI_CH_INVERSION_AUTO;
		default:
			HxLOG_Critical("\n\n");
			return 0xff;
	}
}

#define _____________ConvDI2PAL_CH_CAB__________________

static DxCab_Constellation_e pal_ch_cab_ConvDI2PAL_Constellation(CH_CAB_Modulation_e etModulation)
{
	switch (etModulation)
	{
		case DI_CH_CMOD_16QAM:			return eDxCAB_CONSTELLATION_16QAM;
		case DI_CH_CMOD_32QAM:			return eDxCAB_CONSTELLATION_32QAM;
		case DI_CH_CMOD_64QAM:			return eDxCAB_CONSTELLATION_64QAM;
		case DI_CH_CMOD_128QAM:			return eDxCAB_CONSTELLATION_128QAM;
		case DI_CH_CMOD_256QAM:			return eDxCAB_CONSTELLATION_256QAM;
		case DI_CH_CMOD_AUTO:			return eDxCAB_CONSTELLATION_AUTO;
		default:
			HxLOG_Critical("\n\n");
			return 0xff;
	}
}

static DxCab_Spectrum_e pal_ch_cab_ConvDI2PAL_Sprectrum(CH_CAB_Spectrum_e etSpectrum)
{
	switch (etSpectrum)
	{
		case DI_CH_INVERSION_NORMAL:		return eDxCAB_SPECTRUM_NORMAL;
		case DI_CH_INVERSION_INVERTED:		return eDxCAB_SPECTRUM_INVERTED;
		case DI_CH_INVERSION_AUTO:			return eDxCAB_SPECTRUM_AUTO;
		default:
			HxLOG_Critical("\n\n");
			return 0xff;
	}
}

#define ____________________EXTERN_API______________________
/*****************************************************************
*************************** EXTERN API ****************************
*****************************************************************/

HERROR PAL_CH_CAB_StartTune(
							HUINT32					ulTunerGroupId,
							HUINT32					ulRequestId,
							HUINT32 				ulFrequency,
							HUINT32  				ulSymbolRate,
							DxCab_Constellation_e  	constellation,
							DxCab_Spectrum_e   		spectrum,
							HINT32					nTsId,		/* Only for Japanese cable */
							HINT32					nNetId		/* Only for Japanese cable */
							)
{
	CH_CAB_TuneParam_t		stDiParam;
	DI_CHANNEL_ERR			err;

	VK_MEM_Memset(&stDiParam, 0, sizeof(CH_CAB_TuneParam_t)); /* 이거 안하면 garbage 때문에 다른 튜닝 파라미터로 인식함. */

	stDiParam.ulFrequency	= ulFrequency; /* KHz */
	stDiParam.ulSymbolRate	= ulSymbolRate; /* kSymbols/sec */
	stDiParam.etModulation	= pal_ch_cab_ConvPAL2DI_Constellation(constellation);
	stDiParam.etSpectrum	= pal_ch_cab_ConvPAL2DI_Sprectrum(spectrum);
	stDiParam.nTsId			= (HINT32)nTsId;
	stDiParam.nNetId		= (HINT32)nNetId;

/* DI를 부른다... */
	err = DI_CH_C_RequestLock(ulTunerGroupId, ulRequestId, &stDiParam);
	if(err != DI_CH_OK)
	{
		HxLOG_Error("Error from DI_CH_C_RequestLock() in PAL_CH_CAB_StartTune(): %d\n", err);
		return ERR_FAIL;
	}

#ifdef CONFIG_DEBUG
	{
		HxLOG_Print("===================================================\n");
		HxLOG_Print("	 CAB TUNE PARAM\n");
		HxLOG_Print("===================================================\n");
		HxLOG_Print("	 ulFrequency	  [%d]\n", stDiParam.ulFrequency);
		HxLOG_Print("	 ulSymbolRate	  [%d]\n", stDiParam.ulSymbolRate);
		HxLOG_Print("	 etModulation	  [%d]\n", stDiParam.etModulation);
		switch(stDiParam.etModulation)
		{
			case DI_CH_CMOD_16QAM:	HxLOG_Print("	 \t-->16QAM\n");	break;
			case DI_CH_CMOD_32QAM:	HxLOG_Print("	 \t-->32QAM\n");	break;
			case DI_CH_CMOD_64QAM:	HxLOG_Print("	 \t-->64QAM\n");	break;
			case DI_CH_CMOD_128QAM: HxLOG_Print("	 \t-->128QAM\n");	break;
			case DI_CH_CMOD_256QAM: HxLOG_Print("	 \t-->256QAM\n");	break;
			case DI_CH_CMOD_AUTO:	HxLOG_Print("	 \t-->AUTO\n"); 		break;
			default:				HxLOG_Print("	 \t-->unknown\n");	break;
		}
		HxLOG_Print("	 etSpectrum 	  [%d]\n", stDiParam.etSpectrum);
		HxLOG_Print("===================================================\n");
	}
#endif

	return ERR_OK;
}

HERROR PAL_CH_CAB_StopTune(HUINT32 ulTunerGroupId)
{
	DI_CHANNEL_ERR	err;

	err = DI_CH_C_CancelLock(ulTunerGroupId);

	if(err != DI_CH_OK)
	{
		HxLOG_Error("Error from DI_CH_C_CancelLock() : %d\n", err);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR PAL_CH_CAB_GetLockedInfo(
								HUINT32				ulTunerGroupId,
								DxCab_Constellation_e	*pActualConstellation,
								DxCab_Spectrum_e   	*pActualSpectrum,
								HUINT32				*pulActulaSymbolRate
								)
{
	CH_CAB_TuneParam_t		stDiParam;
	DI_CHANNEL_ERR			err;

	err = DI_CH_C_GetLockedInfo(ulTunerGroupId, &stDiParam);
	if(err != DI_CH_OK)
	{
		HxLOG_Error("Error from DI_CH_C_GetLockedInfo() : %d\n", err);
		return ERR_FAIL;
	}

	*pActualConstellation	= pal_ch_cab_ConvDI2PAL_Constellation(stDiParam.etModulation);
	*pActualSpectrum 		= pal_ch_cab_ConvDI2PAL_Sprectrum(stDiParam.etSpectrum);
	*pulActulaSymbolRate	= (HUINT32)stDiParam.ulSymbolRate;
	return ERR_OK;
}

HERROR PAL_CH_CAB_GetSignalInfo(
								HUINT32			ulTunerGroupId,
								HUINT32			*pulSignalLevel,
								HUINT32			*pulSignalQuality,
								HUINT32			*pulAgc,
								float			*pfBer,
								float			*pfSnr,
								float			*pfSignalInputPower
								)
{
	DI_CHANNEL_ERR			err_DI;
	DI_CH_SignalQuality_t 	stSignalInfo;

	err_DI = DI_CH_C_GetSignalInfo(ulTunerGroupId, &stSignalInfo);
	if(err_DI == DI_CH_OK)
	{
		*pulSignalLevel		= stSignalInfo.ulStrength;
		*pulSignalQuality	= stSignalInfo.ulQuality;
		*pulAgc				= stSignalInfo.ulAgc;
		*pfBer				= stSignalInfo.fBer;
		*pfSnr				= stSignalInfo.fSnr;
		*pfSignalInputPower	= stSignalInfo.fSignalInputPower;

		return ERR_OK;
	}
	else
	{
		*pulSignalLevel		= 0;
		*pulSignalQuality	= 0;
		*pulAgc				= 0;
		*pfBer				= 0;
		*pfSnr				= 0;
		*pfSignalInputPower	= 0;

		return ERR_FAIL;
	}
}
HERROR PAL_CH_CAB_GetSignalInfoForLogCapture(
								HUINT32			ulTunerGroupId,
								float			*pfBer,
								float			*pfSnr,
								HUINT32			*pulUnCorrectedNo,
								float			*pfSignalInputPower
								)
{
	DI_CHANNEL_ERR			err_DI;
	DI_CH_SignalQuality_t 	stSignalInfo;

	err_DI = DI_CH_C_GetSignalInfo(ulTunerGroupId, &stSignalInfo);
	if(err_DI == DI_CH_OK)
	{
		*pfBer				= stSignalInfo.fBer;
		*pfSnr				= stSignalInfo.fSnr;
		*pulUnCorrectedNo 	= stSignalInfo.ulUnCorrectedNo;
		*pfSignalInputPower	= stSignalInfo.fSignalInputPower;
		return ERR_OK;
	}
	else
	{
		*pfBer				= 0;
		*pfSnr				= 0;
		*pulUnCorrectedNo 	= 0;
		*pfSignalInputPower	= 0;
		return ERR_FAIL;
	}
}

static HERROR pal_ch_cab_MakeCabTuneParam (
							HUINT32 				ulFrequency,
							HUINT32  				ulSymbolRate,
							DxCab_Constellation_e  	constellation,
							DxCab_Spectrum_e   		spectrum,
							CH_CAB_TuneParam_t		*pstDiParam
							)
{
	pstDiParam->ulFrequency		= ulFrequency; /* KHz */
	pstDiParam->ulSymbolRate	= ulSymbolRate; /* kSymbols/sec */
	pstDiParam->etModulation	= pal_ch_cab_ConvPAL2DI_Constellation(constellation);
	pstDiParam->etSpectrum		= pal_ch_cab_ConvPAL2DI_Sprectrum(spectrum);

	return ERR_OK;
}

HERROR PAL_CH_CAB_MakeCabTuneParam (
							HUINT32 				ulFrequency,
							HUINT32 				ulSymbolRate,
							DxCab_Constellation_e 	constellation,
							DxCab_Spectrum_e			spectrum,
							HUINT32 				*pulDataSize,
							void					**ppvDiParam
						)
{
	CH_CAB_TuneParam_t	*pstDiParam;
	HERROR				 hErr;

	if (pulDataSize == NULL || ppvDiParam == NULL)
	{
		return ERR_FAIL;
	}

	pstDiParam = (CH_CAB_TuneParam_t *)OxMW_Malloc (sizeof(CH_CAB_TuneParam_t));
	if (pstDiParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = pal_ch_cab_MakeCabTuneParam (ulFrequency, ulSymbolRate, constellation, spectrum, pstDiParam);

	if (hErr != ERR_OK)
	{
		OxMW_Free (pstDiParam);
		return ERR_FAIL;
	}

	*pulDataSize = (HUINT32)sizeof(CH_CAB_TuneParam_t);
	*ppvDiParam  = (void *)pstDiParam;

	return ERR_OK;
}

