/*******************************************************************
	File Description
********************************************************************/
/**
	@file     pmgr_output.h
	@brief

	Description:					\n
	Module :						\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___PMGR_OUTPUT_H___
#define	___PMGR_OUTPUT_H___


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <mgr_output.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Plug-In Functions    ********************/
/*******************************************************************/

HBOOL						pmgr_output_GetDefaultHdcp(void);
HdcpBlockMode_e				pmgr_output_GetHdcpFailBlockSpec(void);
HdcpBlockMode_e				pmgr_output_GetHdcpNoDeviceBlockSpec(void);
HBOOL						pmgr_output_GetEdidAspectRatioSpec(void);
HBOOL						pmgr_output_GetWideSignalLetterBoxSpec(void);
HBOOL						pmgr_output_GetVbiTeletextSpec(void);
VideoBlankSpec_e			pmgr_output_GetVideoBlankSpec(void);
DxCompositeStandard_e		pmgr_output_GetCompositeSpec(void);
HBOOL						pmgr_output_GetHdmiAfdEnableSpec(void);
HBOOL 						pmgr_output_SupportTvScart(void);
HBOOL 						pmgr_output_SupportVcrScart(void);
HBOOL						pmgr_output_SupportSpdif(void);
HBOOL						pmgr_output_SupportCec(void);
DxScartFormat_b				pmgr_output_GetSupportTvScartFormat(void);
DxScartFormat_b				pmgr_output_GetSupportVcrScartFormat(void);
DxDfcSelect_e				pmgr_output_GetInitialDfcSelect(void);
DxResolutionSelect_e		pmgr_output_GetInitialResolutionSelect(void);
DxAspectRatio_e				pmgr_output_GetInitialTvAspectRatio(void);
DxHdmi3DSelect_e			pmgr_output_GetInitial3DMode(void);
DxScartFormat_e				pmgr_output_GetInitialTvScartFormat(void);
DxScartFormat_e				pmgr_output_GetInitialVcrScartFormat(void);
HUINT32						pmgr_output_GetInitialVolume(void);
HUINT32 					pmgr_output_GetInitialDecoderVolume(void);
void						pmgr_output_SetInitialHdmiAudioTransCode(void);
DxDigitalAudioFormat_e		pmgr_output_GetInitialHdmiAudioOutFormat(void);
void						pmgr_output_SetInitialSpdifAudioTransCode(void);
DxDigitalAudioFormat_e		pmgr_output_GetInitialSpdifAudioOutFormat(void);



#endif /* !___PMGR_OUTPUT_H___ */

