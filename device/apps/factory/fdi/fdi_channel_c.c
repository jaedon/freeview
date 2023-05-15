/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author : 
* description :		 Factory Application
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */

/* chip manufacture's  header files */

/* humax header files */
#include "fvk.h"
#include "fdi_common.h"
#include "fdi_channel.h"

#include "di_err.h"
#include "di_channel.h"
#include "di_channel_c.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */

static unsigned long s_ulRequestId;

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_C_Open(unsigned long ulDeviceId)
{
	DI_CHANNEL_ERR eDiErr;
	
	FDI_PRINT(FDI_PRT_DBGINFO, ("[FDI_CHANNEL_C_Open] : ++\n"));
	eDiErr = DI_CH_C_ActivateChannel((unsigned char)ulDeviceId);
	if(eDiErr != DI_CH_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_C_Open] FDI_CHANNEL_S_Open(%d) Error : %d\n", ulDeviceId, eDiErr));
		return FDI_ERR;
	}
	FDI_PRINT(FDI_PRT_DBGINFO, ("[FDI_CHANNEL_C_Open] : --\n"));

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_C_RequestLock(unsigned long ulDeviceId, FDI_CH_CAB_TUNE_PARAM_t *ptParam)
{
	DI_CHANNEL_ERR eDiErr;
	CH_CAB_TuneParam_t tDiParam;
	
	FDI_PRINT(FDI_PRT_DBGINFO, ("[FDI_CHANNEL_C_RequestLock] : ++\n"));

	if( ptParam == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_C_RequestLock] Invalid Parameter. ptParams==NULL\n"));
		return FDI_ERR;
	}
	
	FVK_MEM_Set(&tDiParam, 0, sizeof(CH_CAB_TuneParam_t));

	tDiParam.ulFrequency = ptParam->ulFrequency; 				//unit KHz
	tDiParam.ulSymbolRate = ptParam->ulSymbolRate;

	switch( ptParam->etModulation )
	{
		case FDI_CH_CMOD_AUTO :
			tDiParam.etModulation = DI_CH_CMOD_AUTO;
			break;
		case FDI_CH_CMOD_16QAM :
			tDiParam.etModulation = DI_CH_CMOD_16QAM;
			break;
		case FDI_CH_CMOD_32QAM :
			tDiParam.etModulation = DI_CH_CMOD_32QAM;
			break;
		case FDI_CH_CMOD_64QAM :
			tDiParam.etModulation = DI_CH_CMOD_64QAM;
			break;
		case FDI_CH_CMOD_128QAM :
			tDiParam.etModulation = DI_CH_CMOD_128QAM;
			break;
		case FDI_CH_CMOD_256QAM :
			tDiParam.etModulation = DI_CH_CMOD_256QAM;
			break;
		default :
			tDiParam.etModulation = DI_CH_CMOD_AUTO;
	}

	switch( ptParam->etSpectrum )
	{
		case FDI_CH_INVERSION_AUTO :
			tDiParam.etSpectrum = DI_CH_INVERSION_AUTO;
			break;
		case FDI_CH_INVERSION_NORMAL :
			tDiParam.etSpectrum = DI_CH_INVERSION_NORMAL;
			break;
		case FDI_CH_INVERSION_INVERTED :
			tDiParam.etSpectrum = DI_CH_INVERSION_INVERTED;
			break;
		default :
			tDiParam.etSpectrum = DI_CH_INVERSION_AUTO;
	}

	s_ulRequestId++;

#if (FACTORY_USE_CHANNEL_TER == YES)
	DI_CH_T_CancelLock(ulDeviceId);
	DI_CH_T_DeactivateChannel(ulDeviceId);
#endif
#if (FACTORY_USE_CHANNEL_SAT == YES)
	DI_CH_S_CancelLock(ulDeviceId);
	DI_CH_S_DeactivateChannel(ulDeviceId);
#endif

	DI_CH_C_DeactivateChannel(ulDeviceId);
	DI_DEMUX_BindChannel(ulDeviceId, DI_CH_TYPE_CAB);
	DI_CH_C_ActivateChannel(ulDeviceId);
		

	/* request lock  */
	eDiErr = DI_CH_C_RequestLock(ulDeviceId, s_ulRequestId, &tDiParam);
	if(eDiErr != DI_CH_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_C_RequestLock] DI_CH_S_RequestLock Error : %d\n", eDiErr));
		return FDI_ERR;
	}

	FDI_PRINT(FDI_PRT_DBGINFO, ("[FDI_CHANNEL_C_RequestLock] : --\n"));
	
	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_C_GetLockedInfo(unsigned long ulDeviceId, 
											FDI_CH_CAB_TUNE_PARAM_t *ptParam)
{
	DI_CHANNEL_ERR eDiErr;
	CH_CAB_TuneParam_t tDiParam;
		
	if( ptParam == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_C_GetLockedInfo] Invalid Parameter. ptParams==NULL\n"));
		return FDI_ERR;
	}

	eDiErr = DI_CH_C_GetLockedInfo(ulDeviceId, &tDiParam);
	if(eDiErr != DI_CH_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_C_GetLockedInfo] DI_CH_S_GetLockedInfo Error : %d\n", eDiErr));
		return FDI_ERR;
	}

	ptParam->ulFrequency = tDiParam.ulFrequency;
	ptParam->ulSymbolRate = tDiParam.ulSymbolRate;

	switch( tDiParam.etModulation )
	{
		case DI_CH_CMOD_AUTO :
			ptParam->etModulation = FDI_CH_CMOD_AUTO;
			break;
		case DI_CH_CMOD_16QAM :
			ptParam->etModulation = FDI_CH_CMOD_16QAM;
			break;
		case DI_CH_CMOD_32QAM :
			ptParam->etModulation = FDI_CH_CMOD_32QAM;
			break;
		case DI_CH_CMOD_64QAM :
			ptParam->etModulation = FDI_CH_CMOD_64QAM;
			break;
		case DI_CH_CMOD_128QAM :
			ptParam->etModulation = FDI_CH_CMOD_128QAM;
			break;
		case DI_CH_CMOD_256QAM :
			ptParam->etModulation = FDI_CH_CMOD_256QAM;
			break;
		default :
			ptParam->etModulation = FDI_CH_CMOD_AUTO;
	}

	switch( tDiParam.etSpectrum )
	{
		case DI_CH_INVERSION_AUTO :
			ptParam->etSpectrum = FDI_CH_INVERSION_AUTO;
			break;
		case DI_CH_INVERSION_NORMAL :
			ptParam->etSpectrum = FDI_CH_INVERSION_NORMAL;
			break;
		case DI_CH_INVERSION_INVERTED :
			ptParam->etSpectrum = FDI_CH_INVERSION_INVERTED;
			break;
		default :
			ptParam->etSpectrum = FDI_CH_INVERSION_AUTO;
	}
	
	return FDI_NO_ERR;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_C_IsLocked(unsigned long ulDeviceId, BOOL *pbIsLocked)
{
	DI_CHANNEL_ERR eDiErr;
	CH_CAB_TuneParam_t tDiParam; // fixed by shinms
		
	if( pbIsLocked == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_C_IsLocked] Invalid Parameter. pbIsLocked==NULL\n"));
		return FDI_ERR;
	}

	eDiErr = DI_CH_C_GetLockedInfo(ulDeviceId, &tDiParam);
	if(eDiErr != DI_CH_OK)
	{
		*pbIsLocked = FALSE;
	}
	else
	{
		*pbIsLocked = TRUE;
	}
	
	return FDI_NO_ERR;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_C_CancelLock(unsigned long ulDeviceId)
{
	DI_CHANNEL_ERR eDiErr;

	DI_CH_C_CancelLock(ulDeviceId);
	
	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_C_GetSignalInfo(unsigned long ulDeviceId, FDI_CH_CAB_SIGNAL_QUALITY_t *ptSignalInfo)
{
	DI_CHANNEL_ERR eDiErr;
	DI_CH_SignalQuality_t tDiParam;

	if( ptSignalInfo == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_C_GetSignalInfo] Invalid Parameter. ptSignalInfo==NULL\n"));
		return FDI_ERR;
	}

	eDiErr = DI_CH_C_GetSignalInfo(ulDeviceId, &tDiParam);
	if(eDiErr != DI_CH_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_C_GetSignalInfo] DI_CH_S_GetSignalInfo Error : %d\n", eDiErr));
		return FDI_ERR;
	}

	FDI_PRINT(0, ("[FDI_CHANNEL_S_GetSignalInfo] AGC:%d\n", tDiParam.ulAgc));
	FDI_PRINT(0, ("[FDI_CHANNEL_S_GetSignalInfo] BER:%f\n", tDiParam.fBer));
	FDI_PRINT(0, ("[FDI_CHANNEL_S_GetSignalInfo] SNR:%f\n", tDiParam.fSnr));
	FDI_PRINT(0, ("[FDI_CHANNEL_S_GetSignalInfo] strength:%d\n", tDiParam.ulStrength));
	FDI_PRINT(0, ("[FDI_CHANNEL_S_GetSignalInfo] quality:%d\n", tDiParam.ulQuality));

	ptSignalInfo->unAgc = tDiParam.ulAgc;
	ptSignalInfo->fBer = tDiParam.fBer;
	ptSignalInfo->fSnr = tDiParam.fSnr;
	ptSignalInfo->ulStrength = tDiParam.ulStrength;
	ptSignalInfo->ulQuality = tDiParam.ulQuality;

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_C_RegisterCallback(unsigned long ulDeviceId, pfnFDI_CHANNEL_NotifySignalInfo pfnNotify)
{
	DI_CHANNEL_ERR eDiErr;
	pfnDI_CH_NotifySignalInfo pfnDiNotify;

	pfnDiNotify = (pfnDI_CH_NotifySignalInfo)pfnNotify;
	eDiErr = DI_CH_C_RegisterCallback(ulDeviceId, pfnDiNotify);
	if(eDiErr != DI_CH_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_C_RegisterCallback] DI_CH_S_RegisterCallback Error : %d\n", eDiErr));
		return FDI_ERR;
	}	
	return FDI_NO_ERR;
}


