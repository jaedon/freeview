/*******************************************************************
	File Description
********************************************************************/
/**
	@file     _xmgr_output.h
	@brief

	Description:					\n
	Module :						\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___XMGR_OUTPUT_INT_H___
#define	___XMGR_OUTPUT_INT_H___


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

DxDfcSelect_e xmgr_output_GetInitialDfcSelect_BASE(void);
DxResolutionSelect_e xmgr_output_GetInitialResolutionSelect_BASE(void);
DxAspectRatio_e xmgr_output_GetInitialTvAspectRatio_BASE(void);
DxHdmi3DSelect_e xmgr_output_GetInitial3DMode_BASE(void);
DxScartFormat_e xmgr_output_GetInitialTvScartFormat_BASE(void);
DxScartFormat_e xmgr_output_GetInitialVcrScartFormat_BASE(void);
HUINT32 xmgr_output_GetInitialVolume_BASE(void);
HUINT32 xmgr_output_GetInitialDecoderVolume_BASE(void);
void xmgr_output_SetInitialHdmiAudioTransCode_BASE(void);
DxDigitalAudioFormat_e xmgr_output_GetInitialHdmiAudioOutFormat_BASE(void);
void xmgr_output_SetInitialSpdifAudioTransCode_BASE(void);
DxDigitalAudioFormat_e xmgr_output_GetInitialSpdifAudioOutFormat_BASE(void);
HBOOL xmgr_output_SupportTvScart_BASE(void);

/* Ukdtt */
HBOOL	xmgr_output_GetHdmiAfdEnableSpec_UkDtt(void);

/* Tdc */
HERROR xmgr_output_GetSupportTvScartFormat_Tdc (DxScartFormat_b *pulScartFormatCapability);

/* Amx cenam */
HERROR xmgr_model_GetCompositeSpec_HA7100S (DxCompositeStandard_e *peComSpec);


#endif /* !___XMGR_OUTPUT_INT_H___ */

