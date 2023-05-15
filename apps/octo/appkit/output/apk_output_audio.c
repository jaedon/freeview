/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_output_audio.c
	@brief

	Description:  									\n
	Module: APPKITv2			\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/
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

#include <oapi.h>
#include <apk.h>

#define ___INTERNAL_STRUCTURES___

#define ___INTERNAL_VARIABLES___
STATIC APK_OUTPUT_AUDIO_Notifier_t	s_pfAppNotifier = NULL;

#define ___PRIVATE_FUNCTION___
#define _____NOTIFIER_FUNCTION_____
STATIC void apk_output_audio_onStatusChanged(void)
{
	if(s_pfAppNotifier != NULL)
	{
		APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

		HxLOG_Warning("[APK EMIT]AUDIO s_pfAppNotifier [%p]\n", s_pfAppNotifier);
		// TODO: Queue에 넣는 방식으로 고쳐야 함.

		stApkEvtArgs.pfnFreeFunc = NULL;
		stApkEvtArgs.ulArgCount = 1;
		stApkEvtArgs.apArgV[0] = (void *)s_pfAppNotifier;
		APK_EVENT_Emit(&stApkEvtArgs);
	}
}

#define ___MEMBER_FUNCTION___

#define ___API_FUNCTION___
HERROR APK_OUTPUT_AUDIO_Init(void)
{
	OAPI_OUTPUT_AUDIO_Init(apk_output_audio_onStatusChanged);

	return ERR_OK;
}

HERROR APK_OUTPUT_AUDIO_DeInit(void)
{
	OAPI_OUTPUT_AUDIO_DeInit();

	if (s_pfAppNotifier != NULL)
	{
		s_pfAppNotifier = NULL;
	}
	return ERR_OK;
}

HERROR APK_OUTPUT_AUDIO_RegisterNotifier(APK_OUTPUT_AUDIO_Notifier_t fnNotifier)
{
	if(NULL == fnNotifier)
	{
		HxLOG_Error ("[%s:%d] fnNotifier NULL \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}
	else
	{
		s_pfAppNotifier = fnNotifier;
	}

	return ERR_OK;
}

HERROR APK_OUTPUT_AUDIO_UnregisterNotifier(void)
{
	s_pfAppNotifier = NULL;

	return ERR_OK;
}

HERROR APK_OUTPUT_AUDIO_SupportSpdif(HBOOL *pbSupport)
{
	HERROR					err;
	OxOutputAudio_Cache_t	stStatus;

	err = OAPI_OUTPUT_AUDIO_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*pbSupport = stStatus.bSupportSpdif;
	}
	else
	{
		*pbSupport = FALSE;
	}

	return err;
}

HERROR APK_OUTPUT_AUDIO_GetMasterMute(HBOOL *pbMute)
{
	HERROR					err;
	OxOutputAudio_Cache_t	stStatus;

	err = OAPI_OUTPUT_AUDIO_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*pbMute = stStatus.bMasterMute;
	}
	else
	{
		*pbMute = FALSE;
	}

	return err;
}


HERROR APK_OUTPUT_AUDIO_SetMasterMute(HBOOL bMute)
{
	return OAPI_OUTPUT_AUDIO_SetMasterMute(bMute);
}

HERROR APK_OUTPUT_AUDIO_GetVolumeMax(HUINT32 *pulMaxVolume)
{
	HERROR					err;
	OxOutputAudio_Cache_t	stStatus;

	err = OAPI_OUTPUT_AUDIO_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*pulMaxVolume = stStatus.ulMaxVolume;
	}
	else
	{
		*pulMaxVolume = 0;
	}

	return err;
}


HERROR APK_OUTPUT_AUDIO_GetVolume(HUINT32 *pulVolume)
{
	HERROR					err;
	OxOutputAudio_Cache_t	stStatus;

	err = OAPI_OUTPUT_AUDIO_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*pulVolume = stStatus.ulVolume;
	}
	else
	{
		*pulVolume = 0;
	}

	return err;
}


HERROR APK_OUTPUT_AUDIO_GetDecorderVolume(HUINT32 *pulVolume)
{
	HERROR					err;
	OxOutputAudio_Cache_t	stStatus;

	err = OAPI_OUTPUT_AUDIO_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*pulVolume = stStatus.ulDecoderVolume;
	}
	else
	{
		*pulVolume = 0;
	}

	return err;
}


HERROR APK_OUTPUT_AUDIO_SetVolume(HUINT32 ulVolume)
{
	return OAPI_OUTPUT_AUDIO_SetVolume(ulVolume);
}

HERROR APK_OUTPUT_AUDIO_SetDecoderVolume(HUINT32 ulVolume)
{
	return OAPI_OUTPUT_AUDIO_SetDecoderVolume(ulVolume);
}

HERROR APK_OUTPUT_AUDIO_GetLipSyncDelay(HUINT32 *pulmsec)
{
	HERROR					err;
	OxOutputAudio_Cache_t	stStatus;

	err = OAPI_OUTPUT_AUDIO_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*pulmsec = stStatus.ulLipSyncDelaymsec;
	}
	else
	{
		*pulmsec = 0;
	}

	return err;
}


HERROR APK_OUTPUT_AUDIO_SetLipSyncDelay(HUINT32 ulmsec)
{
	return OAPI_OUTPUT_AUDIO_SetLipSyncDelay(ulmsec);
}

HERROR APK_OUTPUT_AUDIO_SetHdmiFormat(DxDigitalAudioFormat_e eDigitalAudioFormat)
{
	return OAPI_OUTPUT_AUDIO_SetHdmiFormat(eDigitalAudioFormat);
}

HERROR APK_OUTPUT_AUDIO_GetHdmiFormat(DxDigitalAudioFormat_e *peDigitalAudioFormat)
{
	HERROR					err;
	OxOutputAudio_Cache_t	stStatus;

	err = OAPI_OUTPUT_AUDIO_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*peDigitalAudioFormat = stStatus.eHdmiAudioFormat;
	}
	else
	{
		*peDigitalAudioFormat = eDxDIGITALAUDIO_FORMAT_UNKNOWN;
	}

	return err;
}


HERROR APK_OUTPUT_AUDIO_SetSpdifFormat(DxDigitalAudioFormat_e eDigitalAudioFormat)
{
	return OAPI_OUTPUT_AUDIO_SetSpdifFormat(eDigitalAudioFormat);
}

HERROR APK_OUTPUT_AUDIO_GetSpdifFormat(DxDigitalAudioFormat_e *peDigitalAudioFormat)
{
	HERROR					err;
	OxOutputAudio_Cache_t	stStatus;

	err = OAPI_OUTPUT_AUDIO_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*peDigitalAudioFormat = stStatus.eSpdifAudioFormat;
	}
	else
	{
		*peDigitalAudioFormat = eDxDIGITALAUDIO_FORMAT_UNKNOWN;
	}

	return err;
}


HERROR APK_OUTPUT_AUDIO_GetAudioDescription(HBOOL *pbEnable)
{
	HERROR					err;
	OxOutputAudio_Cache_t	stStatus;

	err = OAPI_OUTPUT_AUDIO_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*pbEnable = stStatus.bAudioDescription;
		printf("[%s:%d]stStatus.bAudioDescription[%d]\n",__FUNCTION__,__LINE__,stStatus.bAudioDescription);
	}
	else
	{
		*pbEnable = FALSE;
		printf("[%s:%d]FALSE\n",__FUNCTION__,__LINE__);
	}

	return err;
}


HERROR APK_OUTPUT_AUDIO_SetAudioDescription(HBOOL bEnable)
{
	printf("\n\n[%s:%d]bEnable[%d]\n",__FUNCTION__,__LINE__,bEnable);
	return OAPI_OUTPUT_AUDIO_SetAudioDescription(bEnable);
}

HERROR APK_OUTPUT_AUDIO_SetStereoSelect(DxStereoSelect_e eStereoSelect)
{
	return OAPI_OUTPUT_AUDIO_SetStereoSelect(eStereoSelect);
}

HERROR APK_OUTPUT_AUDIO_GetStereoSelect(DxStereoSelect_e *peStereoSelect)
{
	HERROR					err;
	OxOutputAudio_Cache_t	stStatus;

	err = OAPI_OUTPUT_AUDIO_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*peStereoSelect = stStatus.eStereoSelect;
	}
	else
	{
		*peStereoSelect = FALSE;
	}

	return err;
}


HERROR APK_OUTPUT_AUDIO_SetDualMonoSelect(DxDualMonoSelect_e eDualMonoSelect)
{
	return OAPI_OUTPUT_AUDIO_SetDualMonoSelect(eDualMonoSelect);
}

HERROR APK_OUTPUT_AUDIO_GetDualMonoSelect(DxDualMonoSelect_e *peDualMonoSelect)
{
	HERROR					err;
	OxOutputAudio_Cache_t	stStatus;

	err = OAPI_OUTPUT_AUDIO_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*peDualMonoSelect = stStatus.eDualMonoSelect;
	}
	else
	{
		*peDualMonoSelect = FALSE;
	}

	return err;
}



/* end of file */
