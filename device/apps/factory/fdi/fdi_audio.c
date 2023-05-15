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
#include "fdi_common.h"
#include "fdi_audio.h"

#include "di_err.h"
#include "di_audio.h"
#include "drv_err.h"

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
FDI_AUDIO_EventCallback g_pfnAudioCallback;

void INTRN_FDI_AUDIO_Callback(void *pData);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_AUDIO_GetNumOfDevice(unsigned long *pulNumOfDevice)
{
	DI_AUDIO_CAP_t *pCapInfo;
	
	DI_AUDIO_GetCapability((HUINT32*)pulNumOfDevice, &pCapInfo);
	
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
FDI_ERR_CODE FDI_AUDIO_GetCapability()
{
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
FDI_ERR_CODE FDI_AUDIO_Open(unsigned long ulDeviceId)
{
	DI_ERR_CODE eDiErr;

#if defined(CONFIG_DI10)
	eDiErr = DI_AUDIO_SetDigitalOutput(ulDeviceId, DI_AUDOUT_DOUT_PCM);
	if( eDiErr != DI_ERR_OK )
	{
		return FDI_ERR;
	}
#endif
#if defined(CONFIG_DI10)
	eDiErr = DI_AUDIO_SetVolume(ulDeviceId, 20);
	if( eDiErr != DI_ERR_OK )
	{
		return FDI_ERR;
	}
#endif
#if defined(CONFIG_DI20)
	eDiErr = DI_AUDIO_SetVolume(ulDeviceId, 100);
	if( eDiErr != DI_ERR_OK )
	{
		return FDI_ERR;
	}
#endif
	eDiErr = DI_AUDIO_SetMute(ulDeviceId, FALSE);
	if( eDiErr != DI_ERR_OK )
	{
		return FDI_ERR;
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
FDI_ERR_CODE FDI_AUDIO_Start(unsigned long ulDeviceId, FDI_AUDIO_FORMAT eAudioType)
{
	DI_ERR_CODE eDiErr;
	DI_AUDIO_FORMAT eDiAudioType;


	switch(eAudioType)
	{
		case FDI_AUDIO_FORMAT_MPEG :
			eDiAudioType = DI_AUDFORMAT_MPEG;
			break;
		case FDI_AUDIO_FORMAT_MP3 :
			eDiAudioType = DI_AUDFORMAT_MP3;
			break;
		case FDI_AUDIO_FORMAT_AAC :
			eDiAudioType = DI_AUDFORMAT_AAC;
			break;
		case FDI_AUDIO_FORMAT_AAC_PLUS :
			eDiAudioType = DI_AUDFORMAT_AAC_PLUS;
			break;
		case FDI_AUDIO_FORMAT_AC3 :
			eDiAudioType = DI_AUDFORMAT_AC3;
			break;
		case FDI_AUDIO_FORMAT_AC3_PLUS :
			eDiAudioType = DI_AUDFORMAT_AC3_PLUS;
			break;
		case FDI_AUDIO_FORMAT_PCM :
			eDiAudioType = DI_AUDFORMAT_PCM;
			break;
		default :
			eDiAudioType = DI_AUDFORMAT_MPEG;
	}		

	eDiErr = DI_AUDIO_Start((unsigned char)ulDeviceId, eDiAudioType);
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_DBGINFO, ("[FDI_AUDIO_Start] DI_AUDIO_Start Error : %d\n", eDiErr));
		return FDI_ERR;
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
FDI_ERR_CODE FDI_AUDIO_Stop(unsigned long ulDeviceId)
{
	DI_ERR_CODE eDiErr;
	
	eDiErr = DI_AUDIO_Stop(ulDeviceId);
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_DBGINFO, ("[FDI_AUDIO_Stop] DI_AUDIO_Stop Error : %d\n", eDiErr));
		return FDI_ERR;
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
FDI_ERR_CODE FDI_AUDIO_SetMute(unsigned long ulDeviceId, BOOL bMute)
{
	int i;
	DI_ERR_CODE eDiErr;
	DI_AUDIO_OUTPUT	eOutType[] = {DI_AUDIO_OUTPUT_DAC, DI_AUDIO_OUTPUT_SPDIF, DI_AUDIO_OUTPUT_HDMI, DI_AUDIO_OUTPUT_SCART_2};
	

	for(i=0; i< sizeof(eOutType)/sizeof(DI_AUDIO_OUTPUT); i++)
	{
		eDiErr = DI_AUDIO_SetMute(eOutType[i], bMute);
		if(eDiErr != DI_ERR_OK)
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[FDI_AUDIO_SetMute] DDI_AUDIO_SetMute Error : %d\n", eDiErr));
			return FDI_ERR;
		}
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
FDI_ERR_CODE FDI_AUDIO_SetHWtMute(BOOL bMute)
{
#if defined(CONFIG_PLATFORM_ALI)
	DI_ERR_CODE eDiErr;

	eDiErr = DI_AUDIO_SetHwMute((HBOOL)bMute);
	if(eDiErr != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_AUDIO_SetHWtMute] DI_AUDIO_SetHwMute Error : %d\n", eDiErr));
		return FDI_ERR;
	}
#else
	DRV_Error eDrvErr;

	eDrvErr = DRV_AUDIO_HWMUTE(bMute);
	if(eDrvErr != DRV_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_AUDIO_SetHWtMute] DRV_AUDIO_HWMUTE Error : %d\n", eDrvErr));
		return FDI_ERR;
	}
#endif

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
FDI_ERR_CODE FDI_AUDIO_SetBeepSound(unsigned long ulDeviceId, BOOL bOnOff)
{
	DI_ERR_CODE eDiErr;

#if defined(CONFIG_DI10)
	eDiErr = DI_AUDIO_SetDigitalOutput(DI_AUDIO_DECODER_SPDIF, DI_AUDOUT_DOUT_PCM);
	if( eDiErr != DI_ERR_OK )
	{
		return FDI_ERR;
	}
#endif
#if defined(CONFIG_DI10)
	eDiErr = DI_AUDIO_SetVolume(DI_AUDIO_DECODER_SPDIF, 20);
	if( eDiErr != DI_ERR_OK )
	{
		return FDI_ERR;
	}	
#endif
#if defined(CONFIG_DI20)
	eDiErr = DI_AUDIO_SetVolume(DI_AUDIO_DECODER_SPDIF, 100);
	if( eDiErr != DI_ERR_OK )
	{
		return FDI_ERR;
	}
#endif
	eDiErr = DI_AUDIO_SetMute(DI_AUDIO_DECODER_SPDIF, FALSE);
	if( eDiErr != DI_ERR_OK )
	{
		return FDI_ERR;
	}
#if defined(CONFIG_PLATFORM_ALI)
	DI_AUDIO_BeepPlay(bOnOff);
#else
	DRV_AUDIO_BeepPlay(bOnOff);
#endif

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
FDI_ERR_CODE FDI_AUDIO_SetVolume(unsigned long ulDeviceId, unsigned long ulVolume)
{
	DI_ERR_CODE eDiErr;

#if defined(CONFIG_DI10)
 	eDiErr = DI_AUDIO_SetVolume(ulDeviceId, ulVolume/5);
#endif
#if defined(CONFIG_DI20)
	eDiErr = DI_AUDIO_SetVolume(ulDeviceId, ulVolume);
#endif
	if(eDiErr != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_AUDIO_SetVolume] DI_AUDIO_SetVolume Error : %d\n", eDiErr));
		return FDI_ERR;
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
FDI_ERR_CODE FDI_AUDIO_RegisterEventCallback(unsigned long ulDeviceId, FDI_AUDIO_EventCallback pfnCallback)
{
	DI_ERR_CODE eDiErr;
	HUINT32 unCallerId = 0;
	
	g_pfnAudioCallback = pfnCallback;
	
	eDiErr = DI_AUDIO_RegisterEventCallback(ulDeviceId, DI_AUDIO_RUNNING_STATE_CHANGED, INTRN_FDI_AUDIO_Callback, &unCallerId);
	if(eDiErr != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_AUDIO_RegisterEventCallback] DI_AUDIO_RegisterEventCallback Error : %d\n", eDiErr));
		return FDI_ERR;
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
void INTRN_FDI_AUDIO_Callback(void *pData)
{
	DI_AUDIO_CALLBACK_DATA_t *ptCallbackData;
	DI_AUDIO_RUNNING_STATE_EVT_t *ptAudioRunningState;
	FDI_AUDIO_NOTIFY_EVT		eEvent;
	FDI_AUDIO_RUNNING_STATE		eState;

	ptCallbackData = (DI_AUDIO_CALLBACK_DATA_t *)pData;
	ptAudioRunningState = (DI_AUDIO_RUNNING_STATE_EVT_t *)ptCallbackData->pvEventData;

	switch(ptCallbackData->nEvent)
	{
		case DI_AUDIO_RUNNING_STATE_CHANGED :
				eEvent = FDI_AUDIO_RUNNING_STATE_CHANGED;
				break;				
		case DI_AUDIO_FORMAT_CHANGED :
				eEvent = FDI_AUDIO_FORMAT_CHANGED;
				break;				
		case DI_AUDIO_MEMORY_STREAM_STARTED :
				eEvent = FDI_AUDIO_MEMORY_STREAM_STARTED;
				break;				
		case DI_AUDIO_MEMORY_STREAM_COMPLETED :
				eEvent = FDI_AUDIO_MEMORY_STREAM_COMPLETED;
				break;				
		default :
				eEvent = FDI_AUDIO_EVENT_NOTIFY_END;
	}

	switch(ptAudioRunningState->state)
	{
		case DI_AUDIO_STATE_DECODING :
			eState = FDI_AUDIO_STATE_DECODING;
			break;
		case DI_AUDIO_STATE_NODATA :
			eState = FDI_AUDIO_STATE_NODATA;
			break;
		default :
			eState = FDI_AUDIO_STATE_UNKNOWN;
	}

	if( g_pfnAudioCallback != NULL )
	{
		g_pfnAudioCallback(eEvent, eState);
	}
}

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_AUDIO_CheckRCAJack(unsigned long ulOutputPortId, FDI_AUDIO_RCAStatus *tStatus)
{
	return FDI_FEATURE_NOT_SUPPORTED;
}

