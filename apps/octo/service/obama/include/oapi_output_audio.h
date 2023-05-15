/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_output_audio.h
	@brief

	Description:  									\n
	Module: OAPI / API					\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
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

#ifndef __OAPI_OUTPUT_AUDIO_H__
#define __OAPI_OUTPUT_AUDIO_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define RPC_OAPI_OUTPUT_AUDIO_Init					"oapi.output.audio.Init"
#define RPC_OAPI_OUTPUT_AUDIO_SetMasterMute			"oapi.output.audio.SetMasterMute"
#define RPC_OAPI_OUTPUT_AUDIO_SetVolume				"oapi.output.audio.SetVolume"
#define RPC_OAPI_OUTPUT_AUDIO_SetDecoderVolume				"oapi.output.audio.SetDecoderVolume"
#define RPC_OAPI_OUTPUT_AUDIO_SetLipSyncDelay		"oapi.output.audio.SetLipSyncDelay"
#define RPC_OAPI_OUTPUT_AUDIO_SetHdmiFormat			"oapi.output.audio.SetHdmiFormat"
#define RPC_OAPI_OUTPUT_AUDIO_SetSpdifFormat		"oapi.output.audio.SetSpdifFormat"
#define RPC_OAPI_OUTPUT_AUDIO_SetAudioDescription	"oapi.output.audio.SetAudioDescription"
#define RPC_OAPI_OUTPUT_AUDIO_SetStereoSelect		"oapi.output.audio.SetStereoSelect"
#define RPC_OAPI_OUTPUT_AUDIO_SetDualMonoSelect		"oapi.output.audio.SetDualMonoSelect"

#define RPC_OBAMA_OUTPUT_AUDIO_onStatusChanged		"obama.output.audio.onStatusChanged"

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct
{
	/* Capability */
	HBOOL					bSupportSpdif;
	HUINT32					ulMaxVolume;

	/* Setting & Status */
	HBOOL					bMasterMute;
	HUINT32					ulVolume;
	HUINT32					ulDecoderVolume;
	HUINT32					ulLipSyncDelaymsec;
	DxDigitalAudioFormat_e	eHdmiAudioFormat;
	DxDigitalAudioFormat_e	eSpdifAudioFormat;
	HBOOL					bAudioDescription;
	DxStereoSelect_e		eStereoSelect;
	DxDualMonoSelect_e		eDualMonoSelect;
} OxOutputAudio_Cache_t;

typedef void (*OxOutputAudio_Notifier_t) (void);

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

HERROR OAPI_OUTPUT_AUDIO_Init(OxOutputAudio_Notifier_t fnNotifier);
HERROR OAPI_OUTPUT_AUDIO_DeInit(void);
HERROR OAPI_OUTPUT_AUDIO_SetMasterMute(HBOOL bMute);
HERROR OAPI_OUTPUT_AUDIO_SetVolume(HUINT32 ulVolume);
HERROR OAPI_OUTPUT_AUDIO_SetDecoderVolume(HUINT32 ulVolume);
HERROR OAPI_OUTPUT_AUDIO_SetLipSyncDelay(HUINT32 ulmsec);
HERROR OAPI_OUTPUT_AUDIO_SetHdmiFormat(DxDigitalAudioFormat_e eDigitalAudioFormat);
HERROR OAPI_OUTPUT_AUDIO_SetSpdifFormat(DxDigitalAudioFormat_e eDigitalAudioFormat);
HERROR OAPI_OUTPUT_AUDIO_SetAudioDescription(HBOOL bEnable);
HERROR OAPI_OUTPUT_AUDIO_SetStereoSelect(DxStereoSelect_e eStereoSelect);
HERROR OAPI_OUTPUT_AUDIO_SetDualMonoSelect(DxDualMonoSelect_e eDualMonoSelect);
HERROR OAPI_OUTPUT_AUDIO_GetStatus(OxOutputAudio_Cache_t *pstStatus);

#endif // __OAPI_OUTPUT_AUDIO_H__
