/*******************************************************************
	File Description
********************************************************************/
/**
	@file	pal_output.h
	@brief	pal_output.h

	Description: video, audio 등의 output에 대한 정의 및 처리.		\n
	Module: PAL /OUTPUT												\n

	Copyright (c) 2009 HUMAX Co., Ltd.								\n
	All rights reserved.											\n
*/


#ifndef	__PAL_OUTPUT_H__
#define	__PAL_OUTPUT_H__


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/* Public API 정의 **************************************************************************************/

HERROR PAL_OUT_Init(void);
HERROR PAL_OUT_SetInitParameters(
							HBOOL						bVbiTeletextEnable,
							VideoBlankSpec_e			eVideoBlankSpec,
							DxDigitalAudioFormat_e		eHdmiAudioOutFormat,
							DxDigitalAudioFormat_e		eSpdifAudioOutFormat
							);

HERROR PAL_OUT_SetAvEnable(HBOOL bEnable);
HERROR PAL_OUT_GetAvEnable(HBOOL *pbEnable);
HERROR PAL_OUT_SetHighPriorityHD(HBOOL bEnable);

HERROR PAL_OUT_SetMasterAudioMute(HBOOL bMute);
HBOOL PAL_OUT_GetMasterAudioMute(void);
HERROR PAL_OUT_SetHdmiAudioMute(HBOOL bMute);
HERROR PAL_OUT_SetHdcpAudioMute(HBOOL bMute);
HERROR PAL_OUT_SetDecoderAudioMute(HBOOL bMute);
HERROR PAL_OUT_SetBluetoothAudioMute(HBOOL bMute);

HERROR PAL_OUT_GetVolumeRange(HUINT32 *pulVolmeRange);
HERROR PAL_OUT_SetVolume(HUINT32 ulVolume);
HERROR PAL_OUT_GetVolume(HUINT32 *pulVolume);
HERROR PAL_OUT_SetDecoderVolume(HUINT32 ulVolume);
HERROR PAL_OUT_GetDecoderVolume(HUINT32 *pulVolume);

HERROR PAL_OUT_SetDigitalAudioOutCodec(DxAudioCodec_e eMainStreamAudioCodec, DxAudioCodec_e eSubStreamAudioCodec);
HERROR PAL_OUT_CheckDigitalAudioOut(void);

HERROR PAL_OUT_SetHdmiAudioTransCode(DxAudioCodec_e eSourceCodec, DxAudioCodec_e eTransCodec);
HERROR PAL_OUT_GetHdmiAudioTransCode(DxAudioCodec_e eSourceCodec, DxAudioCodec_e *peTransCodec);
HERROR PAL_OUT_SetHdmiAudioFormat(DxDigitalAudioFormat_e eDigitalAudioFormat);
HERROR PAL_OUT_GetHdmiAudioFormat(DxDigitalAudioFormat_e *peDigitalAudioFormat);

HERROR PAL_OUT_SetSpdifAudioTransCode(DxAudioCodec_e eSourceCodec, DxAudioCodec_e eTransCodec);
HERROR PAL_OUT_GetSpdifAudioTransCode(DxAudioCodec_e eSourceCodec, DxAudioCodec_e *peTransCodec);
HERROR PAL_OUT_SetSpdifAudioFormat(DxDigitalAudioFormat_e eDigitalAudioFormat);
HERROR PAL_OUT_GetSpdifAudioFormat(DxDigitalAudioFormat_e *peDigitalAudioFormat);


HERROR PAL_OUT_SetMacrovision(Macrovision_t eMacrovision);
HERROR PAL_OUT_SetCgmsMacrovision(Macrovision_t eMacrovision);
HERROR PAL_OUT_SetScmsCopyRight(CopyRight_t eMode);
HERROR PAL_OUT_SetVideoBlankCopyRight(CopyRight_t eCopyRight);
HERROR PAL_OUT_SetVideoBlankAspectRatio(VIDEO_Wss_t eWss);

HERROR PAL_OUT_GetOutputStatus(
							DxAudioCodec_e		*peHdmiAudioCodec,
							DxAudioCodec_e		*peSpdifAudioCodec,
							Macrovision_t		*peVideoMacrovision,
							Macrovision_t		*peCgmsMacrovision,
							CopyRight_t			*peCsmsCopyRight,
							CopyRight_t			*peVideoBlankCopyRight,
							VIDEO_Wss_t			*peVideoBlankWss
							);


HERROR PAL_OUT_GetAudioTransSampleRate(DxHdmiAudioSR_e eInput, HBOOL bIsHRA, DxHdmiAudioSR_e *peOutput);

#endif	//__PAL_OUTPUT_H__
