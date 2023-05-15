/*******************************************************************
	File Description
********************************************************************/
/**
	@file	pal_scaler.h
	@brief	pal_scaler.h

	Description: 													\n
	Module: PAL /SCALER												\n

	Copyright (c) 2009 HUMAX Co., Ltd.								\n
	All rights reserved.											\n
*/


#ifndef	__PAL_SCALER_H__
#define	__PAL_SCALER_H__


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
//#include <octo_common.h>

#include <dlib.h>


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	ePAL_SCALER_VIDEOUSAGE_NORMAL,
	ePAL_SCALER_VIDEOUSAGE_STILL,
	ePAL_SCALER_VIDEOUSAGE_CAPTURE
} PalScaler_VideoUsage_e;

/* Public API Á¤ÀÇ **************************************************************************************/
extern void PAL_SCALER_Init(void);
extern void PAL_SCALER_Invalidate(void);

extern void PAL_SCALER_SetInitParameters(
							DxCompositeStandard_e	eCompositeStandard,
							DxDfcSelect_e					eDfc,
							DxResolutionSelect_e			eResolutionSelect,
							DxAspectRatio_e			eTvAspectRatio,
							DxHdmi3DSelect_e				e3DMode,
							HBOOL						bUseEdidAspectRatio,
							HBOOL						bWideSignaloLetterBox,
							HBOOL						bEnableHdmiAfd
							);

extern HERROR PAL_SCALER_SetOsdLayerOutput(OsdZOrder_t		ulZOrder,
							DxAspectRatio_e		eOsdAspectRatio,
							DxDfcSelect_e 			eDfc,
							PIG_Rect_t				stPigRect,
							DxAspectRatio_e 	ePigAspectRatio
							);
extern HERROR PAL_SCALER_GetOsdLayerOutput(OsdZOrder_t		ulZOrder,
							DxAspectRatio_e 	*peOsdAspectRatio,
							DxDfcSelect_e 			*peDfc,
							PIG_Rect_t				*pstPigRect,
							DxAspectRatio_e 	*pePigAspectRatio
							);
extern HERROR PAL_SCALER_SetMediaRect(HUINT32 ulWindowId, DxRect_t stMediaRect, HUINT32 ulRefW, HUINT32 ulRefH);
extern HERROR PAL_SCALER_GetMediaRect(HUINT32 ulWindowId, DxRect_t *pstMediaRect, HUINT32 *pulRefW, HUINT32 *pulRefH);
extern HERROR PAL_SCALER_SetVideoUsage(HUINT32 ulDecId, PalScaler_VideoUsage_e eVideoUsage);
extern HERROR PAL_SCALER_SetVideoAttribute(HUINT32			ulDecId,
							HUINT32					ulWindowId,
							HBOOL					ulMainWindow,
							HUINT32					ulFrameHorizontalSize,
							HUINT32					ulFrameVerticalSize,
							DxAspectRatio_e 	eFrameAspectRatio,
							VIDEO_Afd_t 			eAfd,
							HBOOL					bProgressive,
							VIDEO_FrameRate_t		eFrameRate,
							DxVideo3DFormat_e		e3DFormat
							);
extern void PAL_SCALER_ClearVideoAttribute(HUINT32 ulDecId);
extern void PAL_SCALER_SetEdidAspectRatio(HBOOL bEnable);
extern void PAL_SCALER_SetWideSignalLetterbox(HBOOL bEnable);

extern HERROR PAL_SCALER_SetTvAspectRatio(DxAspectRatio_e eTvAspectRatio, HBOOL bSyncNow);
extern HERROR PAL_SCALER_GetTvAspectRatio(DxAspectRatio_e *peTvAspectRatio);
extern HERROR PAL_SCALER_SetDfcSelect(DxDfcSelect_e eDfc, HBOOL bSyncNow);
extern HERROR PAL_SCALER_GetDfcSelect(DxDfcSelect_e *peDfc);
extern HERROR PAL_SCALER_SetResolutionLimit(HBOOL bEnable);
extern HERROR PAL_SCALER_SetResolutionSelect(DxResolutionSelect_e eResolutionSelect, HBOOL bSyncNow);
extern HERROR PAL_SCALER_GetResolutionSelect(DxResolutionSelect_e *peResolutionSelect);
extern HERROR PAL_SCALER_SyncVideoSetting(void);
extern HERROR PAL_SCALER_GetMhegDisplayPoint(HUINT32	ulRefWidth,
							HUINT32			ulRefHeight,
							HINT32			nX,
							HINT32			nY,
							HINT32			*pnScaledX,
							HINT32			*pnScaledY
							);
extern HERROR PAL_SCALER_GetActiveVideoRect(HUINT32 *pulScreenWidth, HUINT32 *pulScreenHeight, DxRect_t *pstActiveRect, HBOOL *pbPig);
extern HERROR PAL_SCALER_SetBackgroundColor(HUINT8 ucR, HUINT8 ucG, HUINT8 ucB);
extern HERROR PAL_SCALER_GetBackgroundColor(HUINT8 *pucR, HUINT8 *pucG, HUINT8 *pucB);
extern HERROR PAL_SCALER_SetHdmi3DSelect(DxHdmi3DSelect_e e3DSelect);
extern HERROR PAL_SCALER_GetHdmi3DSelect(DxHdmi3DSelect_e *pe3DSelect);
extern HERROR PAL_SCALER_GetOutputStatus(
							VIDEO_Timing_t				*pstOutputTiming,
							DxDfcStatus_b				*pulDfcStatus,
							DxVideo3DFormat_e			*pe3DStatus
							);
extern HERROR PAL_SCALER_SetCompositeStandard(DxCompositeStandard_e eCompStandard);
extern HERROR PAL_SCALER_GetCompositeStandard(DxCompositeStandard_e *peCompStandard);

#endif	//__PAL_SCALER_H__

