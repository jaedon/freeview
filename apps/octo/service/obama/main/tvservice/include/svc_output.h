/*
 * (c) 2011-2012 Humax Co., Ltd.
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
/*******************************************************************
	File Description
********************************************************************/
/**
	@file	mw_output.h
	@brief	mw_output.h

	Description: video, audio 등의 output에 대한 정의 및 처리.		\n
	Module: MW /OUTPUT												\n

	Copyright (c) 2009 HUMAX Co., Ltd.								\n
	All rights reserved.											\n
*/

#ifndef	__SVC_OUTPUT_H__
#define	__SVC_OUTPUT_H__


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <dlib.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

#define OUTPUT_TV_SCART				0
#define OUTPUT_VCR_SCART			1

#define SET_RECT(rect, xx, yy, ww, hh) { \
    (rect)->x = (xx); \
    (rect)->y = (yy); \
    (rect)->w = (ww); \
    (rect)->h = (hh); \
}

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
enum
{
	eSEVT_AVPORT = eSEVT_OUT_START,

	eSEVT_OUT_CHECK_HDMI,
	eSEVT_OUT_CHECK_HDCP,				// p1 : eHdcpStatus_e(OK, authentication fail block, no device block)

	eSEVT_OUT_CHECK_SCART,			// p1 : SCART Interrupt Type,  p2 : Argument  p3 : SCART ID
	eSEVT_OUT_CHECK_VIDEO,
	eSEVT_OUT_CHECK_AUDIO,

	eSEVT_OUT_DISPLAYFORMAT_CHANGED_BY_MENU,

#if defined(CONFIG_MW_SYS_THERMAL_PROTECTION)
	eSEVT_OUT_OVERTEMPERATURE,
	eSEVT_OUT_FANLOCK,
#endif

	eSEVT_OUT_END
}; /* Message to GWM */

HERROR SVC_OUT_Init(PostMsgToMgrCb_t pfncPostMsgCallback);

HERROR SVC_OUT_SetInitParameters(
							HBOOL						bVbiTeletextEnable,
							VideoBlankSpec_e			eVideoBlankSpec,
							DxCompositeStandard_e		eCompositeStandard,
							DxDfcSelect_e 				eDfc,
							DxResolutionSelect_e		eResolutionSelect,
							DxAspectRatio_e 			eTvAspectRatio,
							DxHdmi3DSelect_e			e3DMode,
							HBOOL						bUseEdidAspectRatio,
							HBOOL						bWideSignaloLetterBox,
							HBOOL						bEnableHdmiAfd,
							HdcpBlockMode_e				eHdcpFailBlockMode,
							HdcpBlockMode_e				eHdcpNoDeviceBlockMode,
							DxScartFormat_e				eTvScartFormat,
							DxScartFormat_e				eVcrScartFormat,
							DxDigitalAudioFormat_e		eHdmiAudioOutFormat,
							DxDigitalAudioFormat_e		eSpdifAudioOutFormat
							);

HERROR SVC_OUT_SetAvEnable(HBOOL bEnable);
HERROR SVC_OUT_GetAvEnable(HBOOL *pbEnable);
HERROR SVC_OUT_GetHdmiPlugStatus(HBOOL *pbPlugIn);
HERROR SVC_OUT_SetHighPriorityHD(HBOOL bEnable);
void SVC_OUT_CheckPriorityOutput(void);
void SVC_OUT_SetEdidAspectRatio(HBOOL bEnable);
void SVC_OUT_SetWideSignalLetterbox(HBOOL bEnable);

HERROR SVC_OUT_SetTvAspectRatio(DxAspectRatio_e eTvAspectRatio, HBOOL bSyncNow);
HERROR SVC_OUT_GetTvAspectRatio(DxAspectRatio_e *peTvAspectRatio);

HERROR SVC_OUT_SetResolutionSelect(DxResolutionSelect_e eResolutionSelect, HBOOL bSyncNow);
HERROR SVC_OUT_GetResolutionSelect(DxResolutionSelect_e *peResolutionSelect);

HERROR SVC_OUT_SetMediaRect(HUINT32 ulActionId, DxRect_t stMediaRect, HUINT32 ulRefW, HUINT32 ulRefH);
HERROR SVC_OUT_GetMediaRect(HUINT32 ulActionId, DxRect_t *pstMediaRect, HUINT32 *pulRefW, HUINT32 *pulRefH);

HERROR SVC_OUT_SetOsdLayerOutput(OsdZOrder_t		ulZOrder,
							DxAspectRatio_e		eOsdAspectRatio,
							DxDfcSelect_e 			eDfc,
							PIG_Rect_t				stPigRect,
							DxAspectRatio_e 	ePigAspectRatio
							);
HERROR SVC_OUT_GetOsdLayerOutput(OsdZOrder_t		ulZOrder,
							DxAspectRatio_e 	*peOsdAspectRatio,
							DxDfcSelect_e 			*peDfc,
							PIG_Rect_t				*pstPigRect,
							DxAspectRatio_e 	*pePigAspectRatio
							);

HERROR SVC_OUT_SetDfcSelect(DxDfcSelect_e eDfc, HBOOL bSyncNow);
HERROR SVC_OUT_GetDfcSelect(DxDfcSelect_e *peDfc);

HERROR SVC_OUT_SyncVideoSetting(void);

HERROR SVC_OUT_GetOsdDisplayPoint(HUINT32 ulRefWidth, HUINT32 ulRefHeight, HINT32 nX, HINT32 nY, HINT32 *pnScaledX, HINT32 *pnScaledY);
HERROR SVC_OUT_GetActiveVideoRect(HUINT32 *pulScreenWidth, HUINT32 *pulScreenHeight, DxRect_t *pstActiveRect, HBOOL *pbPig);

HERROR SVC_OUT_SetBackgroundColor(HUINT8 ucR, HUINT8 ucG, HUINT8 ucB);
HERROR SVC_OUT_GetBackgroundColor(HUINT8 *pucR, HUINT8 *pucG, HUINT8 *pucB);

HERROR SVC_OUT_SetCompositeStandard(DxCompositeStandard_e eCompStandard);
HERROR SVC_OUT_GetCompositeStandard(DxCompositeStandard_e *peCompStandard);

HERROR SVC_OUT_SetVolume(HUINT32 ulVolume);
HERROR SVC_OUT_GetVolume(HUINT32 *pulVolume);
HERROR SVC_OUT_SetDecoderVolume(HUINT32 ulVolume);
HERROR SVC_OUT_GetDecoderVolume(HUINT32 *pulVolume);

HERROR SVC_OUT_GetVolumeRange(HUINT32 *pulVolumRange);

HERROR SVC_OUT_SetMasterAudioMute(HBOOL bEnable);
HBOOL SVC_OUT_GetMasterAudioMute(void);

HERROR SVC_OUT_GetSupportHdmiVideoTimingList(HUINT32 *pulNumOfList, VIDEO_Timing_t **ppstVideoTimingList);
HERROR SVC_OUT_GetSupportHdmiAudioCodecList(HUINT32 *pulNumOfList, DxAudioCodec_e **ppeAudioCodecList);
HERROR SVC_OUT_GetHdmi3DFormatCapability(DxVideo3DFormat_b *pulHdmi3DCapa);

HERROR SVC_OUT_CheckVideoTimingAvailable(VIDEO_Timing_t stTiming);
HERROR SVC_OUT_CheckAudioCodecAvailable(DxAudioCodec_e eAudioCodec);

HERROR SVC_OUT_SetCartOutput(HBOOL bEnable);

HERROR SVC_OUT_GetTvScartFormat (DxScartFormat_e *peScartFormat);
HERROR SVC_OUT_SetTvScartFormat (DxScartFormat_e eScartFormat);

HERROR SVC_OUT_GetVcrScartFormat (DxScartFormat_e *peScartFormat);
HERROR SVC_OUT_SetVcrScartFormat (DxScartFormat_e eScartFormat);

HERROR SVC_OUT_GetScartPath(DxScartPath_e *peScartPath);
HERROR SVC_OUT_SetScartPath(DxScartPath_e eScartPath);

HERROR SVC_OUT_SetHdmiAudioTransCode(DxAudioCodec_e eSourceCodec, DxAudioCodec_e eTransCodec);
HERROR SVC_OUT_GetHdmiAudioTransCode(DxAudioCodec_e eSourceCodec, DxAudioCodec_e *peTransCodec);
HERROR SVC_OUT_SetHdmiAudioFormat(DxDigitalAudioFormat_e eDigitalAudioOut);
HERROR SVC_OUT_GetHdmiAudioFormat(DxDigitalAudioFormat_e *peDigitalAudioOut);
HERROR SVC_OUT_SetHdmiAudioMute(HBOOL bMute);

HERROR SVC_OUT_SetSpdifAudioTransCode(DxAudioCodec_e eSourceCodec, DxAudioCodec_e eTransCodec);
HERROR SVC_OUT_GetSpdifAudioTransCode(DxAudioCodec_e eSourceCodec, DxAudioCodec_e *peTransCodec);
HERROR SVC_OUT_SetSpdifAudioFormat(DxDigitalAudioFormat_e eDigitalAudioOut);
HERROR SVC_OUT_GetSpdifAudioFormat(DxDigitalAudioFormat_e *peDigitalAudioOut);

HERROR SVC_OUT_SetVideoBlankCopyRight(HUINT32 ulActionId, CopyRight_t eCopyRight);
HERROR SVC_OUT_SetMacrovision(HUINT32 ulActionId, Macrovision_t eMacrovision);
HERROR SVC_OUT_SetCgmsMacrovision(HUINT32 ulActionId, Macrovision_t eMacrovision);
HERROR SVC_OUT_SetScmsCopyRight(HUINT32 ulActionId, CopyRight_t eCopyRight);
HERROR SVC_OUT_GetHdcp_Version(DxHdcpVersion_e *pVersion);
HERROR SVC_OUT_SetHdcp(HUINT32 ulActionId, HBOOL bEnable);
HERROR SVC_OUT_GetHdcpStatus(DxHdcpStatus_e *peHdcpStatus);

HERROR SVC_OUT_GetNetflixDrmStatus(DxNflixDrmStatus_e *peNflixDrmStaus);
HERROR SVC_OUT_SetNetflixDrmStatus(HUINT32 ulActionId, DxNflixDrmStatus_e eNflixDrmStaus);

void SVC_OUT_SetHdcpFailBlockMode(HdcpBlockMode_e eFailBlockMode);
void SVC_OUT_GetHdcpFailBlockode(HdcpBlockMode_e *peFailBlockMode);
void SVC_OUT_SetHdcpNoDeviceBlockMode(HdcpBlockMode_e eNoDeviceBlockMode);
void SVC_OUT_GetHdcpNoDeviceBlockMode(HdcpBlockMode_e *peNoDeviceBlockMode);

HERROR SVC_OUT_CaptureVideoToBuffer (HUINT32 ulVideoDecId, HUINT32 ulWidth, HUINT32 ulHeight, PixelFormat_e ePixelFormat, HUINT8 **ppucFrameBuffer, HUINT32 *pulBufferSize);
HERROR SVC_OUT_FreeCaptureBuffer (HUINT8 *pucFrameBuffer);
HERROR SVC_OUT_CaptureVideoToPngFile (HUINT32 ulVideoDecId, HUINT32 ulWidth, HUINT32 ulHeight, HCHAR *szFileName);
HERROR SVC_OUT_SetHdmi3DSelect(DxHdmi3DSelect_e e3DMode);
HERROR SVC_OUT_GetHdmi3DSelect(DxHdmi3DSelect_e *pe3DMode);

HERROR SVC_OUT_GetOutputStatus(OutputStatus_t *pstOutputStatus);
HERROR SVC_OUT_CheckHdmiAudioOutput(DxAudioCodec_e eAudioFormat, DxHdmiAudioSR_e eSampleRate, HBOOL bIsHRA, HBOOL *pbSupport);
HERROR SVC_OUT_SetSupportScalerByVideoFrame(HBOOL bEnable);

HERROR SVC_OUT_GetOverTemp(HBOOL *bTemp);
HERROR SVC_OUT_GetFanLock(HBOOL *bTemp);

#endif	//__SVC_OUTPUT_H__

