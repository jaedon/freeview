/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pal_audio.c
	@brief	  audio decoding, setting 등 audio 관련 동작을 관활.

	Description: MW AV의 Sequence에 대한 동작 처리..		\n
	Module: PAL/AUDIO			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

//DI
#include <di_demux.h>
#include <di_audio.h>
#include <di_init.h>

//OCTO
#include <octo_common.h>
#include <get_enum_str.h>
#include <db_param.h>

#include <pal_audio.h>
#include <pal_sef.h>
#include <pal_output.h>
#include <vkernel.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
//#define FCC_DEBUG
#if defined(FCC_DEBUG)
#define HxLOG_FCC			MMMP
#else
#define HxLOG_FCC			HxLOG_Debug
#endif


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/** @brief PAL Audio Context */
typedef struct
{
	DecoderState_t					eState;						/* audio context state */
	HBOOL							bFreeze;

	DxAudioCodec_e 					eMainCodec;					/* main audio codec */
	DxAudioCodec_e 					eSubCodec;					/* aux(audio description) codec */

	DxStereoSelect_e				eStereoConfig;				/* stereo config. LL, LF, RR*/

	HBOOL							bStreamInfoValid;
	PalAudio_StreamInfo_t			stStreamInfo;				/* stream information, header systax */

	AV_HideClient_t					ulHideRequest;
} palAudio_Context_t;

typedef struct
{
	HUINT32							ulDecId;
	DI_AUDIO_FORMAT					eFormat;
} palAudio_DecInfo_t;


typedef struct
{
	HBOOL			bPrimerStarted;
} palAudio_PrimerInfo_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/* Audio Device Context */
STATIC palAudio_Context_t 			s_stAudioContext;
STATIC HBOOL						s_bAudioClip;

#if defined(CONFIG_SUPPORT_FCC) && defined(FCC_USE_AUDIO_PRIMER)
// primer context
STATIC palAudio_PrimerInfo_t		s_stPrimerInfo[NUM_VIEW_ACTION];
#endif


// Bitrate Table...어디서 제공....할 건 지...
STATIC void (*s_pfnEventCallback)(PalAudio_Event_e, HUINT32);
STATIC void (*s_pfnEventCallbackForCas)(PalAudio_Event_e);
STATIC void (*s_pfnEventCallbackForAudioError)(HUINT32, HUINT32);

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

STATIC palAudio_DecInfo_t			s_stDec[NUM_AUDIO_DEC];

STATIC HULONG						s_ulPalAudioSemId;

STATIC HULONG						s_ulPalAudioEventTaskId;
STATIC HULONG						s_ulPalAudioEventMsgQId;

STATIC HULONG						s_ulPalAudioErrorInfoTaskId;
STATIC HULONG						s_ulPalAudioErrorInfoMsgQId;

STATIC HINT32 						s_nPalAudioDelayedValue;


#define	palAudio_ENTER_CRITICAL		{VK_SEM_Get(s_ulPalAudioSemId); /*HxLOG_Debug("*** Enter C_PAL_AUDIO line %d\n", __LINE__);*/}
#define	palAudio_LEAVE_CRITICAL		{VK_SEM_Release(s_ulPalAudioSemId); /*HxLOG_Debug("*** Leave C_PAL_AUDIO line %d\n", __LINE__);*/}


#define _______MAKE_STRING_____________________________________________________

STATIC const HINT8 *pal_audio_GetStrContextState(DecoderState_t cmd)
{
	switch (cmd)
	{
		ENUM_TO_STR(eDECODER_STATE_STOPPED);
		ENUM_TO_STR(eDECODER_STATE_STARTING);
		ENUM_TO_STR(eDECODER_STATE_DECODING);
		ENUM_TO_STR(eDECODER_STATE_UNDERRUN);
	}
	return ("Unknown");
}

STATIC const HINT8 *pal_audio_GetStrDiEvent(DI_AUDIO_NOTIFY_EVT cmd)
{
	switch (cmd)
	{
		ENUM_TO_STR(DI_AUDIO_RUNNING_STATE_CHANGED);
		ENUM_TO_STR(DI_AUDIO_FORMAT_CHANGED);
		ENUM_TO_STR(DI_AUDIO_MEMORY_STREAM_STARTED);
		ENUM_TO_STR(DI_AUDIO_MEMORY_STREAM_COMPLETED);
		ENUM_TO_STR(DI_AUDIO_SIF_MODE_CHANGED);
		ENUM_TO_STR(DI_AUDIO_HEADPHONE_CONNECTION);
		ENUM_TO_STR(DI_AUDIO_DATAEMPTY);
		ENUM_TO_STR(DI_AUDIO_DATAFLOW);
		ENUM_TO_STR(DI_AUDIO_ERRORS_OCCURED);
		ENUM_TO_STR(DI_AUDIO_EVENT_NOTIFY_END);
		default:
			break;
	}
	return ("Unknown");
}

const HINT8 *PAL_AUDIO_GetStrDeviceEvt(PalAudio_Event_e cmd)
{
	switch (cmd)
	{
		ENUM_TO_STR(eAUDIO_EVENT_NONE);
		ENUM_TO_STR(eAUDIO_EVENT_UNDERRUN);
		ENUM_TO_STR(eAUDIO_EVENT_FRAME_START);
		ENUM_TO_STR(eAUDIO_EVENT_PARAM_CHANGED);
		ENUM_TO_STR(eAUDIO_EVENT_CLIP_COMPLETE);
		ENUM_TO_STR(eAUDIO_EVENT_STARTED);
		ENUM_TO_STR(eAUDIO_EVENT_MAX);
	}
	return ("Unknown");
}


#define _____________CONVERT___________________________________________________________________________________________________________________________


STATIC INLINE HERROR pal_audio_ConvertStrereoConfigPal2Di(DxStereoSelect_e mode, DI_AUDIO_STEREO_MODE *di_mode)
{
	switch(mode)
	{
		case eDxSTEREO_SELECT_STEREO:
			*di_mode = DI_AUDIO_DUAL_Stereo;
			break;
		case eDxSTEREO_SELECT_MONOLEFT:
			*di_mode = DI_AUDIO_DUAL_LeftOnly;
			break;
		case eDxSTEREO_SELECT_MONORIGHT:
			*di_mode = DI_AUDIO_DUAL_RightOnly;
			break;
		case eDxSTEREO_SELECT_MONOMIXED:
			*di_mode = DI_AUDIO_DUAL_Mix;
			break;
		default:
			HxLOG_Error("\t%s(ERROR not supported value[%d])\n", __FUNCTION__, mode);
			return ERR_FAIL;
	}
	//HxLOG_Debug("\t\tinput mode (%s) output mode (%d) \n",  OTL_ENUM2STR_StereoSelect(mode), *di_mode);

	return ERR_OK;
}

STATIC HERROR pal_audio_ConvertStereoTypeDi2Pal(DI_AUDIO_MODE eDiAudioMode, DxStereoFormat_e *peStereoType)
{
	switch(eDiAudioMode)
	{
		case DI_AUDIO_MODE_STEREO :
			*peStereoType = eDxSTEREO_FORMAT_2CH;
			break;
		case DI_AUDIO_MODE_MUTI_STEREO :
			*peStereoType = eDxSTEREO_FORMAT_MULTI;
			break;
		case DI_AUDIO_MODE_MONO :
			*peStereoType = eDxSTEREO_FORMAT_MONO;
			break;
		case DI_AUDIO_MODE_DUAL_MONO :
			*peStereoType = eDxSTEREO_FORMAT_DUALMONO;
			break;
		default:
			HxLOG_Error("\t%s(ERROR not supported DI audio mode [%d])\n", __FUNCTION__, eDiAudioMode);
			*peStereoType = eDxSTEREO_FORMAT_UNKNOWN;
			return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR pal_audio_ConvertCodecPal2Di(DxAudioCodec_e codec, DI_AUDIO_FORMAT *eDiCodec)
{
	switch(codec)
	{
		case eDxAUDIO_CODEC_MPEG:
			*eDiCodec = DI_AUDFORMAT_MPEG;
			break;
		case eDxAUDIO_CODEC_MP3:
			*eDiCodec = DI_AUDFORMAT_MP3;
			break;
		case eDxAUDIO_CODEC_DOLBY_AC3:
			*eDiCodec = DI_AUDFORMAT_AC3;
			break;
		case eDxAUDIO_CODEC_DOLBY_AC3P:
			*eDiCodec = DI_AUDFORMAT_AC3_PLUS;
			break;
		case eDxAUDIO_CODEC_AAC:
			*eDiCodec = DI_AUDFORMAT_AAC;
			break;
		case eDxAUDIO_CODEC_AAC_LOAS:
			*eDiCodec = DI_AUDFORMAT_AAC_LOAS;
			break;
		case eDxAUDIO_CODEC_AAC_PLUS:
			*eDiCodec = DI_AUDFORMAT_AAC_PLUS;
			break;
		case eDxAUDIO_CODEC_AAC_PLUS_ADTS:
			*eDiCodec = DI_AUDFORMAT_AAC_PLUS_ADTS;
			break;
		case eDxAUDIO_CODEC_DTS:
			*eDiCodec = DI_AUDFORMAT_DTS;
			break;
		case eDxAUDIO_CODEC_DTS_HD:
			*eDiCodec = DI_AUDFORMAT_DTS_HD;
			break;
		case eDxAUDIO_CODEC_LPCM_DVD:
			*eDiCodec = DI_AUDFORMAT_LPCM_DVD;
			break;
		case eDxAUDIO_CODEC_LPCM_HD_DVD:
			*eDiCodec = DI_AUDFORMAT_LPCM_HD_DVD;
			break;
		case eDxAUDIO_CODEC_LPCM_BLUERAY:
			*eDiCodec = DI_AUDFORMAT_LPCM_BLUERAY;
			break;
		case eDxAUDIO_CODEC_DRA:
			*eDiCodec = DI_AUDFORMAT_DRA;
			break;
		case eDxAUDIO_CODEC_WMA_STD:
			*eDiCodec = DI_AUDFORMAT_WMA_STD;
			break;
		case eDxAUDIO_CODEC_WMA_PRO:
			*eDiCodec = DI_AUDFORMAT_WMA_PRO;
			break;
		case eDxAUDIO_CODEC_FLAC:
			*eDiCodec = DI_AUDFORMAT_FLAC;
			break;
		case eDxAUDIO_CODEC_PCMWAVE:
			*eDiCodec = DI_AUDFORMAT_PCMWAVE;
			break;
		case eDxAUDIO_CODEC_AIFF:
		case eDxAUDIO_CODEC_PCM:
		case eDxAUDIO_CODEC_RIFF:
			*eDiCodec = DI_AUDFORMAT_PCM;
			break;
		default:
			HxLOG_Error("\t%s(ERROR not supported codec [%d])\n", __FUNCTION__, codec);
			*eDiCodec = DI_AUDFORMAT_NONE;
			return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR pal_audio_ConvertCodecDi2Pal(DI_AUDIO_FORMAT eDiCodec, DxAudioCodec_e *pePalCodec)
{
	switch(eDiCodec)
	{
		case DI_AUDFORMAT_MPEG:
			*pePalCodec = eDxAUDIO_CODEC_MPEG;
			break;
		case DI_AUDFORMAT_MP3:
			*pePalCodec = eDxAUDIO_CODEC_MP3;
			break;
		case DI_AUDFORMAT_AAC:
			*pePalCodec = eDxAUDIO_CODEC_AAC;
			break;
		case DI_AUDFORMAT_AAC_LOAS:
			*pePalCodec = eDxAUDIO_CODEC_AAC_LOAS;
			break;
		case DI_AUDFORMAT_AAC_PLUS:
			*pePalCodec = eDxAUDIO_CODEC_AAC_PLUS;
			break;
		case DI_AUDFORMAT_AAC_PLUS_ADTS:
			*pePalCodec = eDxAUDIO_CODEC_AAC_PLUS_ADTS;
			break;
		case DI_AUDFORMAT_AC3:
			*pePalCodec = eDxAUDIO_CODEC_DOLBY_AC3;
			break;
		case DI_AUDFORMAT_AC3_PLUS:
			*pePalCodec = eDxAUDIO_CODEC_DOLBY_AC3P;
			break;
		case DI_AUDFORMAT_PCM:
			*pePalCodec = eDxAUDIO_CODEC_PCM;
			break;
		case DI_AUDFORMAT_DTS:
			*pePalCodec = eDxAUDIO_CODEC_DTS;
			break;
		case DI_AUDFORMAT_LPCM_HD_DVD:
			*pePalCodec = eDxAUDIO_CODEC_LPCM_HD_DVD;
			break;
		case DI_AUDFORMAT_LPCM_BLUERAY:
			*pePalCodec = eDxAUDIO_CODEC_LPCM_BLUERAY;
			break;
		case DI_AUDFORMAT_DTS_HD:
			*pePalCodec = eDxAUDIO_CODEC_DTS_HD;
			break;
		case DI_AUDFORMAT_WMA_STD:
			*pePalCodec = eDxAUDIO_CODEC_WMA_STD;
			break;
		case DI_AUDFORMAT_WMA_PRO:
			*pePalCodec = eDxAUDIO_CODEC_WMA_PRO;
			break;
		case DI_AUDFORMAT_LPCM_DVD:
			*pePalCodec = eDxAUDIO_CODEC_LPCM_DVD;
			break;
		case DI_AUDFORMAT_PCMWAVE:
			*pePalCodec = eDxAUDIO_CODEC_PCMWAVE;
			break;
		case DI_AUDFORMAT_DRA:
			*pePalCodec = eDxAUDIO_CODEC_DRA;
			break;
		default:
			HxLOG_Error("\t%s(ERROR not supported codec [%d])\n", __FUNCTION__, eDiCodec);
			*pePalCodec = DI_AUDFORMAT_MPEG;
			return ERR_FAIL;
	}

	return ERR_OK;
}

#define _____________CAPABILITY___________________________________________________________________________________________________________________________

STATIC INLINE HUINT32 pal_audio_GetDecId(AudioDecType_t eType)
{
	return s_stDec[eType].ulDecId;
}

STATIC INLINE palAudio_DecInfo_t *pal_audio_GetDecInfo(AudioDecType_t eType)
{
	return &s_stDec[eType];
}

STATIC HBOOL pal_audio_IsCodecValid(AudioDecType_t eType, DxAudioCodec_e eCodec)
{
	HERROR			err;

	DI_AUDIO_FORMAT	di_format;

	if(s_stDec[eType].eFormat == 0x0)
	{
		HxLOG_Debug("\t%s(Audio DEC[%s] is NULL. Exit...\n", __FUNCTION__, OTL_ENUM2STR_AudioDecType(eType));

		return FALSE;
	}

	// format conversion.
	err = pal_audio_ConvertCodecPal2Di(eCodec, &di_format);
	if(err != ERR_OK)
	{
		return FALSE;
	}

	if((s_stDec[eType].eFormat & di_format) != 0)
	{
		return TRUE;
	}
	else
	{
		HxLOG_Debug("\t%s(Audio DEC[%s] doesn't support required codec[%s]. Exit...\n"
					, __FUNCTION__
					, OTL_ENUM2STR_AudioDecType(eType)
					, OTL_ENUM2STR_AudioCodec(eCodec)
					);

		return FALSE;
	}
}

#define _____DEVICE_EVENT_________________________________________________________________________________________________

STATIC HERROR pal_audio_UpdateStreamInfo(HBOOL *pbChanged)
{
	HERROR						err;
	PalAudio_StreamInfo_t		stStreamInfo;

	DI_AUDIO_STREAM_INFO_t		stDiStreamInfo;
	DI_ERR_CODE					diErr;

	HxLOG_Debug("\t%s()\n", __FUNCTION__);

	*pbChanged = FALSE;

	diErr = DI_AUDIO_GetStreamInfo(pal_audio_GetDecId(eAUDIO_DEC_MAIN), &stDiStreamInfo);
	if(diErr == DI_ERR_OK)
	{
		err = pal_audio_ConvertCodecDi2Pal(stDiStreamInfo.eFormat, &stStreamInfo.eCodec);
		if(err != ERR_OK)
		{
			return ERR_FAIL;
		}

		err = pal_audio_ConvertStereoTypeDi2Pal(stDiStreamInfo.eMode, &stStreamInfo.eStereoType);
		if(err != ERR_OK)
		{
			return ERR_FAIL;
		}

		stStreamInfo.ulSampleRate = stDiStreamInfo.ulSampleRate;
		// TODO: layer랑 bitrate는 ???

		if(stStreamInfo.eStereoType != s_stAudioContext.stStreamInfo.eStereoType)
		{
			HxLOG_Debug("\t\teStereoType [%s] => [%s]\n"
					, OTL_ENUM2STR_StereoFormat(s_stAudioContext.stStreamInfo.eStereoType)
					, OTL_ENUM2STR_StereoFormat(stStreamInfo.eStereoType)
					);

			*pbChanged = TRUE;
		}

		if(stStreamInfo.ulSampleRate != s_stAudioContext.stStreamInfo.ulSampleRate)
		{
			HxLOG_Debug("\t\tulSampleRate [%d] => [%d]\n"
					, s_stAudioContext.stStreamInfo.ulSampleRate
					, stStreamInfo.ulSampleRate
					);

			//*pbChanged = TRUE; No action for this change......
		}

		s_stAudioContext.bStreamInfoValid = TRUE;
		s_stAudioContext.stStreamInfo = stStreamInfo;
	}
	else
	{
		HxLOG_Error("\tERROR DI_AUDIO_GetStreamInfo ret : 0x%X\n",diErr);

		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC void pal_audio_NotifyEvent(PalAudio_Event_e eAudioEvent, HUINT32 ulRequestId)
{
	if(s_pfnEventCallback != NULL)
	{
		s_pfnEventCallback(eAudioEvent, ulRequestId);
	}

	if (s_pfnEventCallbackForCas != NULL)
	{
		s_pfnEventCallbackForCas(eAudioEvent);
	}
}

STATIC void pal_audio_EventTask(void)
{
	HERROR						err;
	HUINT32						nResult = 0;
	HBOOL						bStreamInfoChanged;
	PalAudio_Event_e 		eAudioEvent;

	DI_AUDIO_CALLBACK_DATA_t	stDiCallbackData;

	while(1)
	{
		nResult = (HUINT32)VK_MSG_Receive(s_ulPalAudioEventMsgQId, &stDiCallbackData, sizeof(DI_AUDIO_CALLBACK_DATA_t));
		if(nResult == ERR_OK)
		{
			palAudio_ENTER_CRITICAL;

			HxLOG_Debug("\t%s(DI event [%s] in [%s] state)\n"
							, __FUNCTION__
							, pal_audio_GetStrDiEvent(stDiCallbackData.nEvent)
							, pal_audio_GetStrContextState(s_stAudioContext.eState)
							);

			eAudioEvent = eAUDIO_EVENT_NONE;

			switch (stDiCallbackData.nEvent)
			{
				case DI_AUDIO_RUNNING_STATE_CHANGED:
					switch( ((DI_AUDIO_RUNNING_STATE_EVT_t*)stDiCallbackData.pvEventData)->state )
					{
						case DI_AUDIO_STATE_DECODING:
							HxLOG_Debug("\t\tDI Running State <DI_AUDIO_STATE_DECODING>\n");

							if( (s_stAudioContext.eState == eDECODER_STATE_STARTING)
								|| (s_stAudioContext.eState == eDECODER_STATE_UNDERRUN)
							)
							{ /* STARTING은 디코딩 시작 후 딱 한번만 들어 갈 수 있다. */
								err = pal_audio_UpdateStreamInfo(&bStreamInfoChanged);
								if(err != ERR_OK)
								{ /* 디코딩이 시작되었는데 stream info가 valid 하지 않다. */
									HxLOG_Debug("\t\tDecoding started but still invalid stream info. Should be wait more ...\n");
								}

								s_stAudioContext.eState = eDECODER_STATE_DECODING;
								HxLOG_Debug("\t\tChange audio state to [%s]\n", pal_audio_GetStrContextState(s_stAudioContext.eState));

								eAudioEvent = eAUDIO_EVENT_FRAME_START;
							}
							else
							{
								HxLOG_Debug("\t\tInvalid DI event delivered in [%s]. Exit ...\n", pal_audio_GetStrContextState(s_stAudioContext.eState));
							}

							break;

						case DI_AUDIO_STATE_NODATA:
							HxLOG_Debug("\t\tDI Running State <DI_AUDIO_STATE_NODATA>\n");

							if( (s_stAudioContext.eState == eDECODER_STATE_STARTING)
								|| (s_stAudioContext.eState == eDECODER_STATE_DECODING)
							)
							{
								s_stAudioContext.eState = eDECODER_STATE_UNDERRUN;
								HxLOG_Debug("\t\tChange audio state to [%s]\n", pal_audio_GetStrContextState(s_stAudioContext.eState));

								eAudioEvent = eAUDIO_EVENT_UNDERRUN;
							}
							else
							{
								HxLOG_Debug("\t\tInvalid DI event delivered in [%s]. Exit ...\n", pal_audio_GetStrContextState(s_stAudioContext.eState));
							}

							break;

						default:
							HxLOG_Debug("\t\tInvalid DI event delivered in [%s]. Exit ...\n", pal_audio_GetStrContextState(s_stAudioContext.eState));
							break;
					}

					break;

				case DI_AUDIO_FORMAT_CHANGED:

					if(s_stAudioContext.eState == eDECODER_STATE_DECODING)
					{
						err = pal_audio_UpdateStreamInfo(&bStreamInfoChanged);
						if( (err == ERR_OK) && (bStreamInfoChanged == TRUE) )
						{
							eAudioEvent = eAUDIO_EVENT_PARAM_CHANGED;
						}
						else
						{ /* 파라미터가 바뀌었다고 하는데 stream info가 valid 하지 않다. */
							HxLOG_Error("\t\t Why ERROR from pal_audio_UpdateStreamInfo() ???\n");
						}
					}
					else
					{
						HxLOG_Debug("\t\tInvalid DI event delivered in [%s]. Exit ...\n", pal_audio_GetStrContextState(s_stAudioContext.eState));
					}

					break;

				case DI_AUDIO_MEMORY_STREAM_COMPLETED:

					PAL_OUT_SetDecoderAudioMute(s_stAudioContext.ulHideRequest != 0);

					eAudioEvent = eAUDIO_EVENT_CLIP_COMPLETE;

					break;

				default:
					break;
			}

			palAudio_LEAVE_CRITICAL;

			if(eAudioEvent != eAUDIO_EVENT_NONE)
			{
				pal_audio_NotifyEvent(eAudioEvent, stDiCallbackData.ulRequestId);
			}
		}
	}

	return;
}

STATIC void pal_audio_ErrorInfoTask(void)
{
	HUINT32							nResult = 0;

	DI_AUDIO_CALLBACK_DATA_t		stDiCallbackData;
	DI_AUDIO_ERROR_INFO_EVT_t		*pstAudioErrorInfo = NULL;
	HUINT32							ulPtsCountError = 0;
	HUINT32							ulFrameError = 0;

	while(1)
	{
		nResult = (HUINT32)VK_MSG_Receive(s_ulPalAudioErrorInfoMsgQId, &stDiCallbackData, sizeof(DI_AUDIO_CALLBACK_DATA_t));
		if(nResult == ERR_OK)
		{

			HxLOG_Debug("\t%s(DI event [%s] in [%s] state)\n"
							, __FUNCTION__
							, pal_audio_GetStrDiEvent(stDiCallbackData.nEvent)
							, pal_audio_GetStrContextState(s_stAudioContext.eState)
							);

			pstAudioErrorInfo = stDiCallbackData.pvEventData;
			if(pstAudioErrorInfo == NULL)
			{
				HxLOG_Error("\tERROR Audio Error Info Pointer is NULL\n");
				return;
			}

			ulFrameError = pstAudioErrorInfo->ulFrameError;
			ulPtsCountError = pstAudioErrorInfo->ulPtsCountError;

			if(s_pfnEventCallbackForAudioError!= NULL)
			{
				s_pfnEventCallbackForAudioError(ulPtsCountError, ulFrameError);
			}
		}
	}
}

STATIC void pal_audio_DiEventCallback(void *pParam)
{
	HUINT32						nResult = 0;
	DI_AUDIO_CALLBACK_DATA_t	stDiAudioCallbackData;

	if(pParam == NULL)
	{
		HxLOG_Error("\t%s() ERROR AudioDevice Callback Pointer is NULL\n", __FUNCTION__);
		return;
	}

	stDiAudioCallbackData = *(DI_AUDIO_CALLBACK_DATA_t *)pParam;

	if(stDiAudioCallbackData.ulDecId != pal_audio_GetDecId(eAUDIO_DEC_MAIN))
	{ /* main dec 이 아니면 처리하지 않음. */
		HxLOG_Debug("\t%s() Event from NOT MAIN decoder[%d]. Exit...\n", __FUNCTION__, stDiAudioCallbackData.ulDecId);
		return;
	}

	nResult = (HUINT32)VK_MSG_SendTimeout(s_ulPalAudioEventMsgQId, &stDiAudioCallbackData, sizeof(DI_AUDIO_CALLBACK_DATA_t), 0);
	if(nResult != ERR_OK)
	{
		HxLOG_Error("\n");
	}
}

#define _____BASIC_OPERATION_____________________________________________________________________

STATIC HERROR pal_audio_ResetDecodingContext(void)
{
	HxLOG_Debug("\t%s()\n", __FUNCTION__);

	s_stAudioContext.eState = eDECODER_STATE_STOPPED;
	s_stAudioContext.bFreeze = FALSE;

	s_stAudioContext.eMainCodec = eDxAUDIO_CODEC_UNKNOWN;
	s_stAudioContext.eSubCodec = eDxAUDIO_CODEC_UNKNOWN;

	s_stAudioContext.bStreamInfoValid = FALSE;
	s_stAudioContext.stStreamInfo.eCodec = eDxAUDIO_CODEC_UNKNOWN;
	s_stAudioContext.stStreamInfo.eStereoType = eDxSTEREO_FORMAT_UNKNOWN;
	s_stAudioContext.stStreamInfo.ulSampleRate = 0;

	return ERR_OK;
}

#if defined(CONFIG_DEBUG)
void pal_audio_PrintStatus(void)
{
	HLIB_CMD_Printf("----------- PAL_AUDIO Context Status -------------------------- \n");
	HLIB_CMD_Printf("\tState\t\t\t\t: %s\n", pal_audio_GetStrContextState(s_stAudioContext.eState));
	HLIB_CMD_Printf("\tbFreeze\t\t\t\t: %d\n", s_stAudioContext.bFreeze);
	HLIB_CMD_Printf("\tMaint\t\t\t: %s\n", OTL_ENUM2STR_AudioCodec(s_stAudioContext.eMainCodec));
	HLIB_CMD_Printf("\tAux\t\t\t\t: %s\n", OTL_ENUM2STR_AudioCodec(s_stAudioContext.eSubCodec));
	HLIB_CMD_Printf("\tStereoCfg\t\t\t\t: %s\n", OTL_ENUM2STR_StereoSelect(s_stAudioContext.eStereoConfig));
	HLIB_CMD_Printf("\tStream updated\t\t\t\t: %d\n", s_stAudioContext.bStreamInfoValid);
	HLIB_CMD_Printf("\tHide Request\t\t\t\t: 0x%X\n", s_stAudioContext.ulHideRequest);
	HLIB_CMD_Printf("\n");
}

static int pal_audio_ProcessCommand_status(void *szArgList)
{
	pal_audio_PrintStatus();

	return HxCMD_OK;
}

static int pal_audio_ProcessCommand_ping(void *szArgList)
{
	HLIB_CMD_Printf("Ping pal_audio API semaphore ...\n");

	palAudio_ENTER_CRITICAL;

	HLIB_CMD_Printf("\t\ts_ulPalAudioSemId alive ...\n");

	palAudio_LEAVE_CRITICAL;

	return HxCMD_OK;
}

#define	hCmdHandle		"pal_audio"

static void pal_audio_RegisterCommand(void)
{
	HLIB_CMD_RegisterWord(hCmdHandle,
						pal_audio_ProcessCommand_status,
						"status",
						"Print audio status.",
						"status");

	HLIB_CMD_RegisterWord(hCmdHandle,
						pal_audio_ProcessCommand_ping,
						"ping",
						"Probe if semaphore is alive.",
						"ping");
}
#endif

HERROR PAL_AUDIO_Init(void)
{
	HUINT32 		ulCallbackId = 0;												/* DI 요청사항. 실제 사용 안함 */
	HINT32			nResult;
	HUINT32			ulNumDiDec = 0;
	HUINT32			i;

	HUINT32			ulDiMainDecId;
	HUINT32			ulLipSyncDealy;
	DI_AUDIO_CAP_t	*ppstDiAudioCap = NULL;
	DI_ERR_CODE		eDiErr = DI_ERR_ERROR;

	nResult  = DB_PARAM_GetItem(eDB_PARAM_ITEM_LIPSYNC_DELAY, (HUINT32 *)&ulLipSyncDealy, 0);
	if (nResult == ERR_OK) {
		s_nPalAudioDelayedValue = ulLipSyncDealy;
	}

	eDiErr = DI_AUDIO_GetCapability(&ulNumDiDec, &ppstDiAudioCap);
	if((DI_ERR_OK != eDiErr) || (NULL == ppstDiAudioCap))
	{
		HxLOG_Warning("eDiErr = %d, ppstDiAudioCap = 0x%x\n", eDiErr, (HUINT32)ppstDiAudioCap);
		HxLOG_Assert(0);
		return ERR_FAIL;
	}

	for(i=0; i<NUM_AUDIO_DEC; i++)
	{
		s_stDec[i].ulDecId = -1;
		s_stDec[i].eFormat = (DI_AUDIO_FORMAT)0x0;
	}

	if(NUM_AUDIO_DEC > ulNumDiDec)
	{
		HxLOG_Warning("NUM_AUDIO_DEC = %d, ulNumDiDec = %d\n", NUM_AUDIO_DEC, ulNumDiDec);
		HxLOG_Warning("NUM_AUDIO_DEC > ulNumDiDec\n");
		//HxLOG_Assert(0);
	}

	for(i = 0; i < ulNumDiDec; i++)
	{
		if(ppstDiAudioCap[i].decoder_type == DI_AUDIO_DECODER_MAIN)
		{
			HxLOG_Print("\t\tLoad MAIN DI audio decoder with id[%d], support_format[0x%X]",
								ppstDiAudioCap[i].decoder_id, ppstDiAudioCap[i].support_format);

			s_stDec[eAUDIO_DEC_MAIN].ulDecId = ppstDiAudioCap[i].decoder_id;
			s_stDec[eAUDIO_DEC_MAIN].eFormat = ppstDiAudioCap[i].support_format;
		}
		else if(ppstDiAudioCap[i].decoder_type == DI_AUDIO_DECODER_SUB)
		{
			HxLOG_Print("\t\tLoad SUB DI audio decoder with id[%d], support_format[0x%X]",
								ppstDiAudioCap[i].decoder_id, ppstDiAudioCap[i].support_format);

			s_stDec[eAUDIO_DEC_SUB].ulDecId = ppstDiAudioCap[i].decoder_id;
			s_stDec[eAUDIO_DEC_SUB].eFormat = ppstDiAudioCap[i].support_format;
		}
		else
		{
			HxLOG_Error("ERROR [%s:%d] DI AUDIO[%d] shows unknown decoder type [%d]\n"
						,__FUNCTION__,__LINE__,i, ppstDiAudioCap[i].decoder_type);
		}
	}

	pal_audio_ResetDecodingContext();

#if defined(CONFIG_SUPPORT_FCC) && defined(FCC_USE_AUDIO_PRIMER)
	HxSTD_MemSet(&s_stPrimerInfo, 0x00, sizeof(s_stPrimerInfo));
#endif

	nResult = VK_SEM_Create(&s_ulPalAudioSemId, "PalAudioSem", VK_SUSPENDTYPE_PRIORITY);
	if(nResult != ERR_OK)
	{
		HxLOG_Error("ERROR VK_SEM_Create ret:0x%X\n",nResult);
	}

	/* Message Q & task for DI event callback */
	if(VK_MSG_Create(64, sizeof(DI_AUDIO_CALLBACK_DATA_t), "PalAudioEventMsgQ", &s_ulPalAudioEventMsgQId, VK_SUSPENDTYPE_PRIORITY) != ERR_OK)
	{
		HxLOG_Critical(" \n");
		return ERR_FAIL;
	}

	if(VK_TASK_Create((void*)pal_audio_EventTask,
					PAL_DICALLBACK_TASK_PRIORITY,
					PAL_AUDIO_TASK_STACK_SIZE,
					"PalAudioEventTask",
					NULL,
					&s_ulPalAudioEventTaskId,
					0) != ERR_OK)
	{
		HxLOG_Critical(" \n");
		return ERR_FAIL;
	}

	if(VK_TASK_Start(s_ulPalAudioEventTaskId) != ERR_OK)
	{
		HxLOG_Critical(" \n");
		return ERR_FAIL;
	}

	/* Message Q & task for DI error info callback */
	if(VK_MSG_Create(64, sizeof(DI_AUDIO_CALLBACK_DATA_t), "PalAudioErrorInfoMsgQ", &s_ulPalAudioErrorInfoMsgQId, VK_SUSPENDTYPE_PRIORITY) != ERR_OK)
	{
		HxLOG_Critical(" \n");
		return ERR_FAIL;
	}

	if(VK_TASK_Create((void*)pal_audio_ErrorInfoTask,
					VK_TASK_PRIORITY_MW_REF,
					PAL_AUDIO_TASK_STACK_SIZE,
					"PalAudioErrorInfoTask",
					NULL,
					&s_ulPalAudioErrorInfoTaskId,
					0) != ERR_OK)
	{
		HxLOG_Critical(" \n");
		return ERR_FAIL;
	}

	if(VK_TASK_Start(s_ulPalAudioErrorInfoTaskId) != ERR_OK)
	{
		HxLOG_Critical(" \n");
		return ERR_FAIL;
	}

	ulDiMainDecId = pal_audio_GetDecId(eAUDIO_DEC_MAIN);

	DI_AUDIO_RegisterEventCallback(ulDiMainDecId, DI_AUDIO_RUNNING_STATE_CHANGED, pal_audio_DiEventCallback, &ulCallbackId);
	DI_AUDIO_RegisterEventCallback(ulDiMainDecId, DI_AUDIO_FORMAT_CHANGED, pal_audio_DiEventCallback, &ulCallbackId);
	DI_AUDIO_RegisterEventCallback(ulDiMainDecId, DI_AUDIO_MEMORY_STREAM_COMPLETED, pal_audio_DiEventCallback, &ulCallbackId);

#if defined(CONFIG_DEBUG)
	pal_audio_RegisterCommand();
#endif

	return ERR_OK;
}

HERROR PAL_AUDIO_StartDecoding(PalAudio_DecodeParam_t *pstDecodeParam)
{
	HERROR				err = ERR_OK;
	palAudio_DecInfo_t		*pMainDec = NULL;
	palAudio_DecInfo_t		*pSubDec = NULL;

	DI_ERR_CODE			diErr = DI_ERR_OK;
	DI_AUDIO_FORMAT 	eDiCodec = DI_AUDFORMAT_PCM;

	palAudio_ENTER_CRITICAL;

	HxLOG_Debug("%s(Main[0x%X, %s], Sub[0x%X, %s]\n", __FUNCTION__,
		pstDecodeParam->usMainPid, OTL_ENUM2STR_AudioCodec(pstDecodeParam->eMainCodec),
		pstDecodeParam->usSubPid, OTL_ENUM2STR_AudioCodec(pstDecodeParam->eSubCodec) );

	if(s_stAudioContext.eState != eDECODER_STATE_STOPPED)
	{
		HxLOG_Debug("\tDecoder is NOT stopped. Ignored ...\n");

		err = ERR_FAIL;
		goto exit;
	}

	if(s_bAudioClip == TRUE)
	{
		HxLOG_Debug("\tDecoder is in audio clip play. Ignored ...\n");

		err = ERR_FAIL;
		goto exit;
	}

	pMainDec = pal_audio_GetDecInfo(eAUDIO_DEC_MAIN);
	pSubDec = pal_audio_GetDecInfo(eAUDIO_DEC_SUB);

	if(pMainDec == NULL || pSubDec == NULL)
	{
		HxLOG_Critical("Invalid Param\n");

		err = ERR_FAIL;
		goto exit;
	}
	HxLOG_Debug("\tMain DI support format:0x%X Sub DI support Format(0x%X)\n", pMainDec->eFormat,pSubDec->eFormat);

	/* 이 지점에서 설정해야 함. */
	PAL_OUT_SetDigitalAudioOutCodec(pstDecodeParam->eMainCodec, pstDecodeParam->eSubCodec);

	HxLOG_Print(HxANSI_COLOR_YELLOW("++ mainPid(0x%x) subPid(0x%x)\n"), pstDecodeParam->usMainPid, pstDecodeParam->usSubPid);

	// Because of BCM의 MS-10 Limitation i changed main & sub audio's start sequence.
	// Issue described in http://svn.humaxdigital.com:3000/issues/44818
	if(pstDecodeParam->usSubPid < PID_NULL)
	{
		if(pal_audio_IsCodecValid(eAUDIO_DEC_SUB, pstDecodeParam->eSubCodec) == TRUE)
		{
			err = pal_audio_ConvertCodecPal2Di(pstDecodeParam->eSubCodec, &eDiCodec);
			if(err != ERR_OK)
			{
				goto exit;
			}

			HxLOG_Debug("\teDiCodec:0x%X\n", eDiCodec);
			HxLOG_FCC(" >>>>>>>>>>>>>>>> DI_AUDIO_Start pSubDec->ulDecId(%d)", pSubDec->ulDecId);

			(void)DI_AUDIO_SetMediaType(pSubDec->ulDecId, DI_AUDIOTYPE_STREAM);
			diErr = DI_AUDIO_Start(pSubDec->ulDecId, eDiCodec);
			if(diErr != ERR_OK)
			{
				HxLOG_Error("\tERROR DI_AUDIO_Start ret : 0x%X\n", diErr);
			}
			else
			{
				/* Decoder volume cannot set, if decoder isn't started.  So, decoder volume set again when sub audio start. */
				HUINT32 ulDecoderVolume = 0;

				(void)PAL_OUT_GetDecoderVolume(&ulDecoderVolume);
				(void)DI_AUDIO_SetDecoderVolume(pSubDec->ulDecId, ulDecoderVolume);
			}
			s_stAudioContext.eSubCodec = pstDecodeParam->eSubCodec;
		}
	}

	if(pstDecodeParam->usMainPid < PID_NULL)
	{
		if(pal_audio_IsCodecValid(eAUDIO_DEC_MAIN, pstDecodeParam->eMainCodec) == TRUE)
		{
			err = pal_audio_ConvertCodecPal2Di(pstDecodeParam->eMainCodec, &eDiCodec);
			if(err != ERR_OK)
			{
				goto exit;
			}

			HxLOG_Debug("\teDiCodec:0x%X\n", eDiCodec);
			HxLOG_FCC(" >>>>>>>>>>>>>>>> DI_AUDIO_Start pMainDec->ulDecId(%d)", pMainDec->ulDecId);

			(void)DI_AUDIO_SetMediaType(pMainDec->ulDecId, DI_AUDIOTYPE_STREAM);
			diErr = DI_AUDIO_Start(pMainDec->ulDecId, eDiCodec);
			if(diErr != ERR_OK)
			{
				HxLOG_Error("\tERROR DI_AUDIO_Start ret : 0x%X\n", diErr);
			}

			s_stAudioContext.eMainCodec = pstDecodeParam->eMainCodec;
		}
	}

	if( (pstDecodeParam->eMainCodec != eDxAUDIO_CODEC_UNKNOWN)
		|| (pstDecodeParam->eSubCodec != eDxAUDIO_CODEC_UNKNOWN)
	)
	{
		s_stAudioContext.eState = eDECODER_STATE_STARTING;
	}
	else
	{
		s_stAudioContext.eState = eDECODER_STATE_STOPPED;
	}

exit:
	palAudio_LEAVE_CRITICAL;

	return err;
}

HERROR PAL_AUDIO_StopDecoding(void)
{
	HERROR				err = ERR_OK;
	palAudio_DecInfo_t		*pMainDec = NULL;
	palAudio_DecInfo_t		*pSubDec = NULL;

	DI_ERR_CODE			diErr;

	palAudio_ENTER_CRITICAL;

	HxLOG_Debug("%s()\n", __FUNCTION__);

	if(s_stAudioContext.eState == eDECODER_STATE_STOPPED)
	{
		HxLOG_Debug("\tDecoder already stopped. Ignored ...\n");

		err = ERR_FAIL;
		goto exit;
	}
	else if(s_bAudioClip == TRUE)
	{
		HxLOG_Debug("\tDecoder is in audio clip play. Ignored ...\n");

		err = ERR_FAIL;
		goto exit;
	}

	pMainDec = pal_audio_GetDecInfo(eAUDIO_DEC_MAIN);
	pSubDec = pal_audio_GetDecInfo(eAUDIO_DEC_SUB);

	if(pMainDec == NULL || pSubDec == NULL)
	{
		HxLOG_Critical("\n\n");

		err = ERR_FAIL;
		goto exit;
	}

	HxLOG_Print(HxANSI_COLOR_YELLOW("-- eMainCodec(0x%x) eSubCodec(0x%x)\n"), s_stAudioContext.eMainCodec, s_stAudioContext.eSubCodec);

	if(s_stAudioContext.eMainCodec != eDxAUDIO_CODEC_UNKNOWN)
	{
		HxLOG_FCC(" >>>>>>> DI_AUDIO_Stop MAIN decId(%d) eMainCodec(%d)  <<<<<<<<<<<<<< ", pMainDec->ulDecId, s_stAudioContext.eMainCodec);

		diErr = DI_AUDIO_Stop(pMainDec->ulDecId);
		if(diErr != DI_ERR_OK)
		{
			HxLOG_Error("\tERROR DI_AUDIO_Stop ret : 0x%X\n",diErr);
		}

	}

	if(s_stAudioContext.eSubCodec != eDxAUDIO_CODEC_UNKNOWN)
	{
		HxLOG_FCC(" >>>>>>> DI_AUDIO_Stop SUB decId(%d) eSubCodec(%d)  <<<<<<<<<<<<<< ", pSubDec->ulDecId, s_stAudioContext.eSubCodec);

		diErr = DI_AUDIO_Stop(pSubDec->ulDecId);
		if(diErr != DI_ERR_OK)
		{
			HxLOG_Error("\tERROR DI_AUDIO_Stop ret : 0x%X\n",diErr);
		}

	}

	pal_audio_ResetDecodingContext();

exit:
	palAudio_LEAVE_CRITICAL;

	return err;
}

HERROR PAL_AUDIO_StartPCM(HUINT8		*pucBuffer,
						HUINT32			ulBufferBytes,
						DxAudioCodec_e	eCodec,
						HUINT32			ulloopCnt
						)
{
	HERROR				err = ERR_OK;

	DI_ERR_CODE			diErr;
	DI_AUDIO_FORMAT		eDiAudioFormat;

	palAudio_ENTER_CRITICAL;

	HxLOG_Debug("%s(pucBuffer:0x%X, ulBufferBytes:%d, %s, loop:%d\n", __FUNCTION__,
				pucBuffer, ulBufferBytes, OTL_ENUM2STR_AudioCodec(eCodec), ulloopCnt);

	if( (pucBuffer == NULL) || (ulBufferBytes == 0) )
	{
		HxLOG_Error("\tERROR Invalid Parameter pucBuffer[0x%X],ulBufferBytes[0x%X]!!!\n",pucBuffer,ulBufferBytes);

		err = ERR_FAIL;
		goto exit;
	}

	if( (eCodec != eDxAUDIO_CODEC_PCM)
		&& (eCodec != eDxAUDIO_CODEC_PCMWAVE)
		&& (eCodec != eDxAUDIO_CODEC_AIFF)
	)
	{
		HxLOG_Error("\tERROR Unsupported PCM format [%d]!!!\n",eCodec);

		err = ERR_FAIL;
		goto exit;
	}

	err = pal_audio_ConvertCodecPal2Di(eCodec, &eDiAudioFormat);
	if(err != ERR_OK)
	{
		goto exit;
	}

	PAL_OUT_SetDecoderAudioMute(FALSE);

	diErr = DI_AUDIO_StartMemoryStream(0xffffffff,
										eDiAudioFormat,
										pucBuffer,
										ulBufferBytes,
										ulloopCnt,
										DI_AUDIO_OUTPUT_ALL);

	if(diErr != DI_ERR_OK)
	{
		HxLOG_Error("\tERROR DI_AUDIO_StartMemoryStream ret : 0x%X\n",diErr);

		err = ERR_FAIL;
		goto exit;
	}

exit:
	palAudio_LEAVE_CRITICAL;

	return err;
}

HERROR PAL_AUDIO_StopPCM(void)
{
	HERROR			err = ERR_OK;

	DI_ERR_CODE 	diErr;

	palAudio_ENTER_CRITICAL;

	HxLOG_Debug("%s()\n", __FUNCTION__);

	diErr = DI_AUDIO_StopMemoryStream(0xffffffff);
	if(diErr != DI_ERR_OK)
	{
		HxLOG_Error("\tERROR DI_AUDIO_StopMemoryStream ret : 0x%X\n",diErr);

		err = ERR_FAIL;
		goto exit;
	}

	PAL_OUT_SetDecoderAudioMute(s_stAudioContext.ulHideRequest != 0);

exit:
	palAudio_LEAVE_CRITICAL;

	return err;
}

#if 0
#if 0 // choism temp
#include "pal_audio_arib_beep_riff.c"
#endif
HERROR PAL_AUDIO_PlayAribBeep(HUINT32 ulBeepNum)
{
	HUINT8		*pucBuffer;
	HUINT32		ulBufferBytes;
	HERROR		ret;
#if 0 // choism temp

	AUDIO_PRT(0, ("%s ContextId=%d clip num(%d)\n", __FUNCTION__, ulBeepNum));

	switch (ulBeepNum)
	{
		case 0:
			pucBuffer = gRIFFAlarm_1;
			ulBufferBytes = gRIFFAlarmSize_1;
 			break;

		case 1:
			pucBuffer = gRIFFAlarm_2;
			ulBufferBytes = gRIFFAlarmSize_2;
 			break;

		case 2:
			pucBuffer = gRIFFAlarm_3;
			ulBufferBytes = gRIFFAlarmSize_3;
 			break;

		case 3:
			pucBuffer = gRIFFAlarm_4;
			ulBufferBytes = gRIFFAlarmSize_4;
 			break;

		case 4:
			pucBuffer = gRIFFAlarm_5;
			ulBufferBytes = gRIFFAlarmSize_5;
 			break;

		case 5:
			pucBuffer = gRIFFButton_1;
			ulBufferBytes = gRIFFButtonSize_1;
 			break;

		case 6:
			pucBuffer = gRIFFButton_2;
			ulBufferBytes = gRIFFButtonSize_2;
 			break;

		case 7:
			pucBuffer = gRIFFButton_3;
			ulBufferBytes = gRIFFButtonSize_3;
 			break;

		case 8:
			pucBuffer = gRIFFButton_4;
			ulBufferBytes = gRIFFButtonSize_4;
 			break;

		case 9:
			pucBuffer = gRIFFButton_5;
			ulBufferBytes = gRIFFButtonSize_5;
 			break;

		case 10:
			pucBuffer = gRIFFButton_6;
			ulBufferBytes = gRIFFButtonSize_6;
 			break;

		case 11:
			pucBuffer = gRIFFButton_7;
			ulBufferBytes = gRIFFButtonSize_7;
 			break;

		case 12:
			pucBuffer = gRIFFButton_8;
			ulBufferBytes = gRIFFButtonSize_8;
 			break;

		case 13:
			pucBuffer = gRIFFAlert;
			ulBufferBytes = gRIFFAlertSize;
 			break;

		/* 이건 휴맥스 App 용 경고음 입니다. */
		case 20:
			pucBuffer = gRIFFHumaxAlert;
			ulBufferBytes = gRIFFHumaxAlertSize;
 			break;

		default:
			return;
	}

	eDxAUDIO_CODEC_PCM;

	ret = PAL_AUDIO_StartPCM(pucBuffer, ulBufferBytes, eDxAUDIO_CODEC_PCMWAVE, 1);
	if(ret)
	{
		AUDIO_PRT(DBG_ERROR, ("PAL_AUDIO_StartAudioClip() error. (%d)\n", ret));
 	}
#endif

	return;
}

#endif


HERROR PAL_AUDIO_StartAudioClip(HUINT32 	ulRequestId,
							HUINT8			*pucBuffer,
							HUINT32			ulBufferBytes,
							DxAudioCodec_e	eCodec,
							HUINT32			ulloopCnt
							)
{
	HERROR				err = ERR_OK;

	DI_ERR_CODE			diErr;
	DI_AUDIO_FORMAT		eDiAudioFormat;

	palAudio_ENTER_CRITICAL;

	HxLOG_Debug("%s(id: %d, pucBuffer:0x%X, ulBufferBytes:%d, %s, loop:%d\n", __FUNCTION__,
				ulRequestId, pucBuffer, ulBufferBytes, OTL_ENUM2STR_AudioCodec(eCodec), ulloopCnt);

	if( (pucBuffer == NULL) || (ulBufferBytes == 0) )
	{
		HxLOG_Error("\tERROR Invalid Parameter pucBuffer[0x%X],ulBufferBytes[0x%X]!!!\n",pucBuffer,ulBufferBytes);

		err = ERR_FAIL;
		goto exit;
	}

	if(s_stAudioContext.eState != eDECODER_STATE_STOPPED)
	{
		HxLOG_Debug("\tDecoder is NOT stopped. Ignored ...\n");

		err = ERR_FAIL;
		goto exit;
	}

	err = pal_audio_ConvertCodecPal2Di(eCodec, &eDiAudioFormat);
	if(err != ERR_OK)
	{
		goto exit;
	}

	PAL_OUT_SetDecoderAudioMute(FALSE);

	/* 이 위치에 있어야 함. */
	PAL_OUT_SetDigitalAudioOutCodec(eCodec, eDxAUDIO_CODEC_UNKNOWN);

	diErr = DI_AUDIO_StartMemoryStream(ulRequestId,
								eDiAudioFormat,
								pucBuffer,
								ulBufferBytes,
								ulloopCnt,
								DI_AUDIO_OUTPUT_ALL);

	if(diErr != DI_ERR_OK)
	{
		HxLOG_Error("\tERROR DI_AUDIO_StartMemoryStream ret : 0x%X\n",diErr);

		err = ERR_FAIL;
		goto exit;
	}

	s_bAudioClip = TRUE;
	s_stAudioContext.eState = eDECODER_STATE_STARTING;

exit:
	palAudio_LEAVE_CRITICAL;

	return err;
}

HERROR PAL_AUDIO_StopAudioClip(HUINT32 ulRequestId)
{
	HERROR			err = ERR_OK;
	DI_ERR_CODE		diErr;

	palAudio_ENTER_CRITICAL;

	HxLOG_Debug("%s(id:%d)\n", __FUNCTION__, ulRequestId);

	if(s_stAudioContext.eState == eDECODER_STATE_STOPPED)
	{
		HxLOG_Debug("\tDecoder already stopped. Ignored ...\n");

		err = ERR_FAIL;
		goto exit;
	}

	if(s_bAudioClip != TRUE)
	{
		HxLOG_Debug("\tDecoder is NOT in audio clip play. Ignored ...\n");

		err = ERR_FAIL;
		goto exit;
	}

	diErr = DI_AUDIO_StopMemoryStream(ulRequestId);
	if(diErr != DI_ERR_OK)
	{
		HxLOG_Error("\tERROR DI_AUDIO_StopMemoryStream ret : 0x%X\n",diErr);

		err = ERR_FAIL;
		goto exit;
	}

	PAL_OUT_SetDecoderAudioMute(s_stAudioContext.ulHideRequest != 0);

	s_bAudioClip = FALSE;

	pal_audio_ResetDecodingContext();

exit:
	palAudio_LEAVE_CRITICAL;

	return err;
}

HERROR PAL_AUDIO_PauseAudioClip(HUINT32 ulRequestId)
{
	HERROR			err = ERR_OK;

	DI_ERR_CODE		diErr;

	palAudio_ENTER_CRITICAL;

	HxLOG_Debug("%s(id:%d)\n", __FUNCTION__, ulRequestId);

	if(s_bAudioClip == TRUE)
	{
		diErr = DI_AUDIO_PauseMemoryStream(ulRequestId, TRUE);
		if(diErr != DI_ERR_OK)
		{
			HxLOG_Error("\tERROR DI_AUDIO_PauseMemoryStream ret : 0x%X\n",diErr);
			err = ERR_FAIL;
		}
	}

	palAudio_LEAVE_CRITICAL;

	return err;
}

HERROR PAL_AUDIO_ResumeAudioClip(HUINT32 ulRequestId)
{
	HERROR			err = ERR_OK;
	DI_ERR_CODE		diErr;

	palAudio_ENTER_CRITICAL;

	HxLOG_Debug("%s(id:%d)\n", __FUNCTION__, ulRequestId);

	if(s_bAudioClip == TRUE)
	{
		diErr = DI_AUDIO_PauseMemoryStream(ulRequestId, FALSE);
		if(diErr != ERR_OK)
		{
			HxLOG_Error("\tERROR DI_AUDIO_PauseMemoryStream ret : 0x%X\n",diErr);
			err = ERR_FAIL;
		}
	}

	palAudio_LEAVE_CRITICAL;

	return err;
}

HERROR PAL_AUDIO_RegisterEventCallback(void (*pfnEventCallback)(PalAudio_Event_e eAudioEvent, HUINT32 ulRequestId))
{
	s_pfnEventCallback = pfnEventCallback;

	return ERR_OK;
}

HERROR PAL_AUDIO_RegisterEventCallbackForCas(void (*pfnEventCallback)(PalAudio_Event_e eAudioEvent))
{
	s_pfnEventCallbackForCas = pfnEventCallback;

	return ERR_OK;
}
HERROR PAL_AUDIO_RegisterEventCallbackForAudioError(void (*pfnEventCallback)(HUINT32 ulPtsCountError, HUINT32 ulFrameError))
{
	s_pfnEventCallbackForAudioError = pfnEventCallback;

	return ERR_OK;
}

HERROR PAL_AUDIO_SetEvtTimeOut(HUINT32 msec)
{
	return DI_AUDIO_SetEventTimeOut(0, msec);
}

HERROR PAL_AUDIO_SetStereoConfig(DxStereoSelect_e eStereoConfig)
{
	HERROR					err = ERR_OK;
	HUINT32					ulDecId;

	DI_AUDIO_STEREO_MODE	eDiStereoMode = DI_AUDIO_DUAL_Stereo;

	palAudio_ENTER_CRITICAL;

	HxLOG_Debug("%s(%s)\n", __FUNCTION__, OTL_ENUM2STR_StereoSelect(eStereoConfig));

	if(eStereoConfig == eDxSTEREO_SELECT_UNKNOWN)
	{
		HxLOG_Error("\tERROR eStereoConfig is : 0x%X\n",eStereoConfig);

		err = ERR_FAIL;
		goto exit;
	}

	s_stAudioContext.eStereoConfig = eStereoConfig;

	err = pal_audio_ConvertStrereoConfigPal2Di(eStereoConfig, &eDiStereoMode);
	if(err != ERR_OK)
	{
		goto exit;
	}

	HxLOG_Debug("\t\teDiStereoMode (%d) \n", eDiStereoMode);

	ulDecId = pal_audio_GetDecId(eAUDIO_DEC_MAIN);
	err = DI_AUDIO_SetStereo(ulDecId, eDiStereoMode);
	if(err != ERR_OK)
	{
		HxLOG_Error("\tERROR DI_AUDIO_SetStereo ret : 0x%X\n",err);
	}

exit:
	palAudio_LEAVE_CRITICAL;

	return err;
}

HERROR PAL_AUDIO_SetDelayAudioLipSync(HINT32 msec)
{
	DI_ERR_CODE				diErr;
	palAudio_DecInfo_t			*pMainDec = NULL;

	HxLOG_Debug("%s(%d ms) ", __FUNCTION__, msec);

	s_nPalAudioDelayedValue = msec;

	pMainDec = pal_audio_GetDecInfo(eAUDIO_DEC_MAIN);

	diErr = DI_AUDIO_DelayOutput(pMainDec->ulDecId, msec);
	if(diErr != DI_ERR_OK)
	{
		HxLOG_Error("\tERROR DI_AUDIO_DelayOutput ret : 0x%X\n",diErr);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR PAL_AUDIO_GetDelayAudioLipSync(HINT32 *pmsec)
{
	if (NULL == pmsec)
		return ERR_FAIL;

	HxLOG_Debug("\t\ttime offset (%d)\n",s_nPalAudioDelayedValue);

	*pmsec = s_nPalAudioDelayedValue;

	return ERR_OK;
}

HBOOL PAL_AUDIO_GetFreeze(void)
{
	return s_stAudioContext.bFreeze;
}

void PAL_AUDIO_SetFreeze(HBOOL bFreeze)
{
	palAudio_DecInfo_t		*pMainDec = NULL;
	palAudio_DecInfo_t		*pSubDec = NULL;

	DI_AUDIO_FORMAT		eDiCodec;

	palAudio_ENTER_CRITICAL;

	HxLOG_Debug("%s(%d) ", __FUNCTION__, bFreeze);

	if(s_stAudioContext.eState == eDECODER_STATE_STOPPED)
	{
		goto exit;
	}

	pMainDec = pal_audio_GetDecInfo(eAUDIO_DEC_MAIN);
	pSubDec = pal_audio_GetDecInfo(eAUDIO_DEC_SUB);

	if(pMainDec == NULL || pSubDec == NULL)
	{
		HxLOG_Critical("\n\n");
		goto exit;
	}

	if(bFreeze == TRUE)
	{
		if(s_stAudioContext.bFreeze != TRUE)
		{
			if(s_stAudioContext.eMainCodec != eDxAUDIO_CODEC_UNKNOWN)
			{
				HxLOG_FCC(" >>>>>>>>>>>>>>>> DI_AUDIO_Stop pMainDec->ulDecId(%d)", pMainDec->ulDecId);
				DI_AUDIO_Stop(pMainDec->ulDecId);
			}

			if(s_stAudioContext.eSubCodec != eDxAUDIO_CODEC_UNKNOWN)
			{
				HxLOG_FCC(" >>>>>>>>>>>>>>>> DI_AUDIO_Stop pSubDec->ulDecId(%d)", pSubDec->ulDecId);
				DI_AUDIO_Stop(pSubDec->ulDecId);
			}

			s_stAudioContext.bFreeze = TRUE;
		}
	}
	else
	{
		if(s_stAudioContext.bFreeze == TRUE)
		{
			if(s_stAudioContext.eMainCodec != eDxAUDIO_CODEC_UNKNOWN)
			{
				HxLOG_FCC(" >>>>>>>>>>>>>>>> DI_AUDIO_Start pMainDec->ulDecId(%d)", pMainDec->ulDecId);

				pal_audio_ConvertCodecPal2Di(s_stAudioContext.eMainCodec, &eDiCodec);
				DI_AUDIO_Start(pMainDec->ulDecId, eDiCodec);
			}

			if(s_stAudioContext.eSubCodec != eDxAUDIO_CODEC_UNKNOWN)
			{
				HxLOG_FCC(" >>>>>>>>>>>>>>>> DI_AUDIO_Start pSubDec->ulDecId(%d)", pSubDec->ulDecId);

				pal_audio_ConvertCodecPal2Di(s_stAudioContext.eSubCodec, &eDiCodec);
				DI_AUDIO_Start(pSubDec->ulDecId, eDiCodec);
			}

			s_stAudioContext.bFreeze = FALSE;
		}
	}

exit:
	palAudio_LEAVE_CRITICAL;
}

HERROR PAL_AUDIO_SetHide(AV_HideClient_t eAvHideClient, HBOOL bHide)
{
	palAudio_ENTER_CRITICAL;

	HxLOG_Debug("%s(Client:0x%x, bHide:%d)\n", __FUNCTION__, eAvHideClient, bHide);

	if(bHide == TRUE)
	{
		s_stAudioContext.ulHideRequest |= eAvHideClient;
	}
	else
	{
		s_stAudioContext.ulHideRequest &= ~eAvHideClient;
	}

	if(s_bAudioClip == TRUE)
	{
		PAL_OUT_SetDecoderAudioMute(FALSE);
	}
	else
	{
		PAL_OUT_SetDecoderAudioMute(s_stAudioContext.ulHideRequest != 0);
	}

	palAudio_LEAVE_CRITICAL;

	return ERR_OK;
}

DecoderState_t PAL_AUDIO_GetState(void)
{
	return s_stAudioContext.eState;
}

#define _____CHECK_STATUS______________________________________________________________________________________________________________________

HBOOL PAL_AUDIO_IsSupportCodec(AudioDecType_t eType, DxAudioCodec_e eAudCodec)
{
	return pal_audio_IsCodecValid(eType, eAudCodec);
}

HERROR PAL_AUDIO_GetStreamInfo(PalAudio_StreamInfo_t *pstStreamInfo)
{
	if(s_stAudioContext.bStreamInfoValid == TRUE)
	{
		*pstStreamInfo = s_stAudioContext.stStreamInfo;
		return ERR_OK;
	}

	return ERR_FAIL;
}

#if defined(CONFIG_SUPPORT_FCC) && defined(FCC_USE_AUDIO_PRIMER)
HERROR PAL_AUDIO_StartPrimerDecoding(PalAudio_DecodeParam_t *pstDecodeParam, HBOOL bMainAv, HUINT32 ulPrimerIdx, HBOOL bStartPrimer)
{
	HERROR				err = ERR_OK;
	palAudio_DecInfo_t		*pMainDec = NULL;
	palAudio_DecInfo_t		*pSubDec = NULL;

	DI_ERR_CODE			diErr = DI_ERR_OK;
	DI_AUDIO_FORMAT 	eDiCodec = DI_AUDFORMAT_PCM;
	DI_AUDIO_PRIMER_t 	stPrimerSetting;

	palAudio_ENTER_CRITICAL;

	HxLOG_Debug("%s(Main[0x%X, %s], Sub[0x%X, %s]\n", __FUNCTION__,
		pstDecodeParam->usMainPid, OTL_ENUM2STR_AudioCodec(pstDecodeParam->eMainCodec),
		pstDecodeParam->usSubPid, OTL_ENUM2STR_AudioCodec(pstDecodeParam->eSubCodec) );

	if (bMainAv == TRUE)
	{
		if (s_stAudioContext.eState != eDECODER_STATE_STOPPED)
		{
			HxLOG_Error("\tDecoder is NOT stopped. Ignored ...\n");
			err = ERR_FAIL;
			goto exit;
		}
	}
	else
	{
		// TODO: Primer state를 체크하도록 추가해야 함..!
	}

	if(s_bAudioClip == TRUE)
	{
		HxLOG_Debug("\tDecoder is in audio clip play. Ignored ...\n");

		err = ERR_FAIL;
		goto exit;
	}

	pMainDec = pal_audio_GetDecInfo(eAUDIO_DEC_MAIN);
	pSubDec = pal_audio_GetDecInfo(eAUDIO_DEC_SUB);

	if(pMainDec == NULL || pSubDec == NULL)
	{
		HxLOG_Critical("Invalid Param\n");

		err = ERR_FAIL;
		goto exit;
	}
	HxLOG_Debug("\tMain DI support format:0x%X Sub DI support Format(0x%X)\n", pMainDec->eFormat,pSubDec->eFormat);

	/* 이 지점에서 설정해야 함. */
	PAL_OUT_SetDigitalAudioOutCodec(pstDecodeParam->eMainCodec);

	// Because of BCM의 MS-10 Limitation i changed main & sub audio's start sequence.
	// Issue described in http://svn.humaxdigital.com:3000/issues/44818
#if 0	// TODO: 일단은 main만..
	if(pstDecodeParam->usSubPid < PID_NULL)
	{
		if(pal_audio_IsCodecValid(eAUDIO_DEC_SUB, pstDecodeParam->eSubCodec) == TRUE)
		{
			err = pal_audio_ConvertCodecPal2Di(pstDecodeParam->eSubCodec, &eDiCodec);
			if(err != ERR_OK)
			{
				goto exit;
			}

			HxLOG_Debug("\teDiCodec:0x%X\n", eDiCodec);

			(void)DI_AUDIO_SetMediaType(pSubDec->ulDecId, DI_AUDIOTYPE_STREAM);
			diErr = DI_AUDIO_Start(pSubDec->ulDecId, eDiCodec);
			if(diErr != ERR_OK)
			{
				HxLOG_Error("\tERROR DI_AUDIO_Start ret : 0x%X\n", diErr);
			}

			s_stAudioContext.eSubCodec = pstDecodeParam->eSubCodec;
		}
	}
#endif

	if(pstDecodeParam->usMainPid < PID_NULL)
	{
		if(pal_audio_IsCodecValid(eAUDIO_DEC_MAIN, pstDecodeParam->eMainCodec) == TRUE)
		{
			err = pal_audio_ConvertCodecPal2Di(pstDecodeParam->eMainCodec, &eDiCodec);
			if(err != ERR_OK)
			{
				goto exit;
			}

			HxLOG_Debug("\teDiCodec:0x%X\n", eDiCodec);

			HxSTD_MemSet(&stPrimerSetting, 0x00, sizeof(DI_AUDIO_PRIMER_t));
			stPrimerSetting.ulPrimerIndex = ulPrimerIdx;
			stPrimerSetting.eAudioType = DI_AUDIOTYPE_STREAM;
			stPrimerSetting.eFormat = eDiCodec;

			if (bStartPrimer)
			{
				HxLOG_FCC(" >>>>>>> DI_AUDIO_StartPrimer decId(%d) idx(%d)  <<<<<<<<<<<<<< ", pMainDec->ulDecId, stPrimerSetting.ulPrimerIndex);

				diErr = DI_AUDIO_StartPrimer(pMainDec->ulDecId, &stPrimerSetting);
				if(diErr != ERR_OK)
				{
					HxLOG_Error("\tERROR DI_AUDIO_StartPrimer ret : 0x%X\n", diErr);
				}

				s_stPrimerInfo[ulPrimerIdx].bPrimerStarted = TRUE;
			}

			(void)DI_AUDIO_SetMediaType(pMainDec->ulDecId, DI_AUDIOTYPE_STREAM);

			if (bMainAv == TRUE)
			{
				HxLOG_FCC(" >>>>>>> DI_AUDIO_SelectPrimer decId(%d) ulPrimerIdx(%d) <<<<<<<<<<<<<< ", pMainDec->ulDecId, ulPrimerIdx);

				diErr = DI_AUDIO_SelectPrimer(pMainDec->ulDecId, ulPrimerIdx);
				if(diErr != ERR_OK)
				{
					HxLOG_Error("\tERROR DI_AUDIO_SelectPrimer ret : 0x%X\n", diErr);
				}

				HxLOG_FCC(" >>>>>>> DI_AUDIO_Start decId(%d)  <<<<<<<<<<<<<< ", pMainDec->ulDecId);

				diErr = DI_AUDIO_Start(pMainDec->ulDecId, eDiCodec);
				if(diErr != ERR_OK)
				{
					HxLOG_Error("\tERROR DI_AUDIO_Start ret : 0x%X\n", diErr);
				}
			}

			s_stAudioContext.eMainCodec = pstDecodeParam->eMainCodec;
		}
	}

	if (bMainAv == TRUE)
	{
		if( (pstDecodeParam->eMainCodec != eDxAUDIO_CODEC_UNKNOWN)
			|| (pstDecodeParam->eSubCodec != eDxAUDIO_CODEC_UNKNOWN))
		{
			s_stAudioContext.eState = eDECODER_STATE_STARTING;
		}
		else
		{
			s_stAudioContext.eState = eDECODER_STATE_STOPPED;
		}
	}

exit:
	palAudio_LEAVE_CRITICAL;

	return err;
}

HERROR PAL_AUDIO_StopPrimerDecoding(HUINT32 ulPrimerIdx)
{
	HERROR				err = ERR_OK;
	palAudio_DecInfo_t		*pMainDec = NULL;
	palAudio_DecInfo_t		*pSubDec = NULL;
	DI_AUDIO_PRIMER_t 	 stPrimerSetting;

	DI_ERR_CODE			diErr;

	palAudio_ENTER_CRITICAL;

	HxLOG_Debug("%s()\n", __FUNCTION__);

	if(s_stAudioContext.eState == eDECODER_STATE_STOPPED)
	{
		HxLOG_Error("\tDecoder already stopped. Ignored ...\n");
		err = ERR_FAIL;
		goto exit;
	}
	else if(s_bAudioClip == TRUE)
	{
		HxLOG_Debug("\tDecoder is in audio clip play. Ignored ...\n");

		err = ERR_FAIL;
		goto exit;
	}

	pMainDec = pal_audio_GetDecInfo(eAUDIO_DEC_MAIN);
	pSubDec = pal_audio_GetDecInfo(eAUDIO_DEC_SUB);

	if(pMainDec == NULL || pSubDec == NULL)
	{
		HxLOG_Critical("\n\n");

		err = ERR_FAIL;
		goto exit;
	}

	HxSTD_MemSet(&stPrimerSetting, 0x00, sizeof(DI_AUDIO_PRIMER_t));
	stPrimerSetting.eAudioType = DI_AUDIOTYPE_STREAM;
	stPrimerSetting.ulPrimerIndex = ulPrimerIdx;

	if(s_stAudioContext.eMainCodec != eDxAUDIO_CODEC_UNKNOWN)
	{
		HxLOG_FCC(" >>>>>>> DI_AUDIO_SelectPrimer decId(%d)  ulPrimerIndex(%d) <<<<<<<<<<<<<< ", pMainDec->ulDecId, stPrimerSetting.ulPrimerIndex);

		diErr = DI_AUDIO_SelectPrimer(pMainDec->ulDecId, stPrimerSetting.ulPrimerIndex);
		if(diErr != DI_ERR_OK)
		{
			HxLOG_Error("\tERROR DI_AUDIO_SelectPrimer ret : 0x%X\n",diErr);
		}

		HxLOG_FCC(" >>>>>>> DI_AUDIO_Stop decId(%d) ulPrimerIndex(%d) <<<<<<<<<<<<<< ", pMainDec->ulDecId, stPrimerSetting.ulPrimerIndex);

		diErr = DI_AUDIO_Stop(pMainDec->ulDecId);
		if(diErr != DI_ERR_OK)
		{
			HxLOG_Error("\tERROR DI_AUDIO_Stop ret : 0x%X\n",diErr);
		}
	}


	// TODO: check sub codec if needed..

	pal_audio_ResetDecodingContext();

exit:
	palAudio_LEAVE_CRITICAL;

	return err;
}

HERROR PAL_AUDIO_StopPrimer(HUINT32 ulPrimerIdx)
{
	HERROR				err = ERR_OK;
	palAudio_DecInfo_t		*pMainDec = NULL;
	palAudio_DecInfo_t		*pSubDec = NULL;
	DI_AUDIO_PRIMER_t 	 stPrimerSetting;

	DI_ERR_CODE			diErr;

	palAudio_ENTER_CRITICAL;

	HxLOG_Debug("%s()\n", __FUNCTION__);

	if(s_stPrimerInfo[ulPrimerIdx].bPrimerStarted == FALSE)
	{
		HxLOG_Error("\tAudio primer already stopped. Ignored ...\n");
		err = ERR_FAIL;
		goto exit;
	}

	pMainDec = pal_audio_GetDecInfo(eAUDIO_DEC_MAIN);
	pSubDec = pal_audio_GetDecInfo(eAUDIO_DEC_SUB);

	if(pMainDec == NULL || pSubDec == NULL)
	{
		HxLOG_Critical("\n\n");

		err = ERR_FAIL;
		goto exit;
	}

	HxSTD_MemSet(&stPrimerSetting, 0x00, sizeof(DI_AUDIO_PRIMER_t));
	stPrimerSetting.ulPrimerIndex = ulPrimerIdx;

	HxLOG_FCC(" >>>>>>> DI_AUDIO_StopPrimer decId(%d) ulPrimerIndex(%d) <<<<<<<<<<<<<< ", pMainDec->ulDecId, stPrimerSetting.ulPrimerIndex);

	diErr = DI_AUDIO_StopPrimer(pMainDec->ulDecId, &stPrimerSetting);
	if(diErr != DI_ERR_OK)
	{
		HxLOG_Error("\tERROR DI_AUDIO_StopPrimer ret : 0x%X\n",diErr);
	}

	s_stPrimerInfo[ulPrimerIdx].bPrimerStarted = FALSE;

exit:
	palAudio_LEAVE_CRITICAL;

	return err;
}


#endif

#define _____DEBUG_PRINT___________________________________________________________

/*********************** End of File ******************************/
