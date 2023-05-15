/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxdm_pp.h $
 * $brcm_Revision: Hydra_Software_Devel/25 $
 * $brcm_Date: 8/2/12 10:31a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/xdm/bxdm_pp.h $
 * 
 * Hydra_Software_Devel/25   8/2/12 10:31a btosi
 * SW7425-1064: support for flushing a held picture on the enhanced
 * channel at StartDecode
 * 
 * Hydra_Software_Devel/24   7/27/12 11:21a btosi
 * SW7425-1064: fixed function name
 * 
 * Hydra_Software_Devel/23   7/25/12 9:45a btosi
 * SW7425-3558: added BXDM_PictureProvider_GetCurrentPicturePtr_isr
 * 
 * Hydra_Software_Devel/22   7/2/12 9:53a btosi
 * SW7425-3358: first cut at the FNRT changes
 * 
 * Hydra_Software_Devel/21   3/26/12 1:56p btosi
 * SW7425-2536: removed
 * BXDM_PictureProvider_GetDefaultDecoderInterface_isr
 * 
 * Hydra_Software_Devel/20   3/16/12 11:56a btosi
 * SW7425-2536: added support for displayInterruptStatus_isr
 * 
 * Hydra_Software_Devel/19   2/1/12 1:57p btosi
 * SW7425-2270: add support for IgnoreNRTUnderflow
 * 
 * Hydra_Software_Devel/18   11/9/11 9:43a btosi
 * SW7425-1690: fixed enums
 * 
 * Hydra_Software_Devel/17   10/3/11 2:19p btosi
 * SW7425-1264:  support for a SW STC internal to XDM
 * 
 * Hydra_Software_Devel/16   8/18/11 3:19p btosi
 * SW7425-966: added support for eLeftRightEnhancedResolution
 * 
 * Hydra_Software_Devel/15   7/26/11 11:02a btosi
 * SW7635-51: merged BXDM_PictureProvider_PictureHandlingMode_eWait
 * changes
 * 
 * Hydra_Software_Devel/SW7635-51/1   5/11/11 9:28a btosi
 * SW7635-51: added BXDM_PictureProvider_PictureHandlingMode_eWait
 * 
 * Hydra_Software_Devel/14   5/10/11 1:40p btosi
 * SW7405-4736: added range checking and debug messages for the input
 * parameters
 * 
 * Hydra_Software_Devel/13   11/24/10 1:22p btosi
 * SW7422-72: added bSetNextPointer for MVC on older DVD chips
 * 
 * Hydra_Software_Devel/12   11/22/10 3:12p btosi
 * SW7422-72: renamed flag in BXDM_PictureProvider_3DSettings
 * 
 * Hydra_Software_Devel/11   11/17/10 9:37a btosi
 * SW7422-72: added new BXVD and XDM 3D API's
 * 
 * Hydra_Software_Devel/10   10/1/10 2:50p btosi
 * SWDEPRECATED-1003: added support for bTreatAsSingleElement
 * 
 * Hydra_Software_Devel/9   9/23/10 3:09p btosi
 * SW7405-4736: add support for a XDM instance ID to help debug multi-
 * channel issues
 * 
 * Hydra_Software_Devel/8   9/15/10 12:06p btosi
 * SWDEPRECATED-1003: added BXDM_PictureProvider_SetFrameRateOverride_isr
 * and supporting logic
 * 
 * Hydra_Software_Devel/7   9/14/10 11:56a btosi
 * SWDEPRECATED-1003: added BXDM_PictureProvider_SetTrickMode_isr to
 * primarily disable the FIC logic when receiving video with sparse
 * pictures
 * 
 * Hydra_Software_Devel/6   8/5/10 8:29a delkert
 * SW7405-4703: Add HorizontalOverscanMode to allow bypass of Overscan
 * calculation.  Added appropriate Get/Set APIs
 * 
 * Hydra_Software_Devel/5   5/11/10 1:33p btosi
 * SW7405-4117: added usage note
 * 
 * Hydra_Software_Devel/4   4/29/10 10:47a btosi
 * SW7405-4117: added support for ...DisplayFieldMode_eAuto
 * 
 * Hydra_Software_Devel/3   2/19/10 11:09a nilesh
 * SW7405-2993: Made the DIH handle a property of the picture provider so
 * that the PDR callback can persist over a stop/start decode
 * 
 * Hydra_Software_Devel/2   2/17/10 4:58p nilesh
 * SW7405-2993: Removed unused
 * BXDM_PictureProvider_Counters.uiDecoderInputOverflow
 * 
 * Hydra_Software_Devel/1   2/16/10 10:51a nilesh
 * SW7405-2993: Initial XDM version
 *
 ***************************************************************************/

#ifndef BXDM_PP_H_
#define BXDM_PP_H_

#include "btmr.h"
#include "bxdm_decoder.h"
#include "bxdm.h"
#include "bxdm_dih.h"

#ifdef __cplusplus
extern "C" {
#endif

#if 0
}
#endif

typedef struct BXDM_PictureProvider_P_Context *BXDM_PictureProvider_Handle;

#if 0
typedef struct BXDM_PictureProvider_Settings
{
      uint32_t uiMaxBuffersPerPicture; /* This value indicates how many video buffers are
                                        * linked together in a single BXDM_Picture entry.
                                        * E.g. for MVC 3D, there are 2 video buffers per
                                        * BXDM_Picture entry */
} BXDM_PictureProvider_Settings;
#endif

BERR_Code
BXDM_PictureProvider_GetDefaultSettings(
         BXDM_PictureProvider_Settings *pstPictureProviderSettings
         );

BERR_Code
BXDM_PictureProvider_Create(
         BXDM_PictureProvider_Handle *phXdmPP,
         const BXDM_PictureProvider_Settings *pstPictureProviderSettings
         );

BERR_Code
BXDM_PictureProvider_Destroy(
         BXDM_PictureProvider_Handle hXdmPP
         );

BERR_Code
BXDM_PictureProvider_StartDecode_isr(
         BXDM_PictureProvider_Handle hXdmPP
         );

BERR_Code
BXDM_PictureProvider_StopDecode_isr(
         BXDM_PictureProvider_Handle hXdmPP
         );

BERR_Code
BXDM_PictureProvider_FlushHeldPicture_isr(
         BXDM_PictureProvider_Handle hXdmPP
         );

BERR_Code
BXDM_PictureProvider_WatchdogReset_isr(
         BXDM_PictureProvider_Handle hXdmPP
         );

BERR_Code
BXDM_PictureProvider_GetPicture_isr(
         void *pPrivateContext,
         const BXDM_DisplayInterruptInfo *pstDisplayInterruptInfo,
         BAVC_MFD_Picture **pMFDPicture
         );

BERR_Code
BXDM_PictureProvider_SetDecoderInterface_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_Decoder_Interface *pstDecoderInterface,
         void *pPrivateContext
         );

#if 0
typedef struct BXDM_PictureProvider_ChannelChangeSettings
{
      bool bHoldLastPicture;
      bool bFirstPicturePreview;
} BXDM_PictureProvider_ChannelChangeSettings;
#endif

BERR_Code
BXDM_PictureProvider_GetDefaultChannelChangeSettings(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_ChannelChangeSettings *pstChannelChangeSettings
         );

BERR_Code
BXDM_PictureProvider_SetChannelChangeSettings_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         const BXDM_PictureProvider_ChannelChangeSettings *pstChannelChangeSettings
         );

BERR_Code
BXDM_PictureProvider_GetChannelChangeSettings_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_ChannelChangeSettings *pstChannelChangeSettings
         );

BERR_Code
BXDM_PictureProvider_SetMuteMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool bEnable
         );

BERR_Code
BXDM_PictureProvider_GetMuteMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool *pbEnable
         );


/***************************************************************************

Below is an explanation of the new eDisplayFieldMode mode 
"BXDM_PictureProvider_DisplayFieldMode_eAuto" and the supporting API 
"BXDM_PictureProvider_SetDeinterlacerMaxHeight".

For normal play and fast forward, eDisplayFieldMode should be set to "eBothField".  
The question is how to set eDisplayFieldMode for slow motion and pause. 
If there is an "appropriate" deinterlacer in the output path, it should be set to 
"eBothField" for the best results.  If there is NOT an "appropriate" deinterlacer, 
it should be set to "eSingleField"; this will minimize jitter at the expense of a lower 
resolution image. The new eDisplayFieldMode of "BXDM_PictureProvider_DisplayFieldMode_eAuto" solves this 
problem by automatically selecting either "eBothField" or "eSingleField" during slow 
motion and pause.

Having an "appropriate" deinterlacer is defined as:
- every output is driven by a deinterlacer
- AND the least capable deinterlacer can support the stream height
- AND the deinterlacer(s) are enabled

The API "BXDM_PictureProvider_SetDeinterlacerMaxHeight" is used to set the 
variable "uiMaxHeightSupportedByDeinterlacer".  
The application/middleware will use "uiMaxHeightSupportedByDeinterlacer" to indicate if there 
is an "appropriate" deinterlacer.  If "uiMaxHeightSupportedByDeinterlacer" is set to "0", 
there is NOT an "appropriate" deinterlacer and the scan out will be "eSingleField". 
If "uiMaxHeightSupportedByDeinterlacer" is set to a non-zero value, the scan out will be "eBothField" if 
the stream height is less than or equal to "uiMaxHeightSupportedByDeinterlacer".  
If the height is greater than "uiMaxHeightSupportedByDeinterlacer", the scan out will be "eSingleField".  

The following pseudo code summaries the behavior:

   if ( stGlobal.eDisplayFieldMode == BXDM_PictureProvider_DisplayFieldMode_eAuto )
   {
      // Default to eBothField

      stLocal.eDisplayFieldMode = BXDM_PictureProvider_DisplayFieldMode_eBothField;

      // When paused or in slow motion, revert to eSingleField if the deinterlacer cannot support the stream.

      if ( stSource.uiHeight > uiMaxHeightSupportedByDeinterlacer
            && true == bPausedOrSlowMotion
         )
      {
         stLocal.eDisplayFieldMode = BXDM_PictureProvider_DisplayFieldMode_eSingleField;
      }

   }

****************************************************************************/

#if 0
typedef enum BXDM_PictureProvider_DisplayFieldMode
{
      BXDM_PictureProvider_DisplayFieldMode_eBothField,       /* regular both field display */
      BXDM_PictureProvider_DisplayFieldMode_eTopFieldOnly,    /* Display Top fld only */
      BXDM_PictureProvider_DisplayFieldMode_eBottomFieldOnly,  /* Display Bottom fld only */
      BXDM_PictureProvider_DisplayFieldMode_eSingleField, /* Display single field only.
                                                           * The field that is displayed
                                                           * is the same as the one that
                                                           * is currently being
                                                           * displayed. */
      BXDM_PictureProvider_DisplayFieldMode_eAuto,    /* SW7405-4117: select either eSingleField or eBothField 
                                                       * based on the steam height during slow motion (and preroll). */
      BXDM_PictureProvider_DisplayFieldMode_eMax
} BXDM_PictureProvider_DisplayFieldMode;
#endif

BERR_Code
BXDM_PictureProvider_SetDisplayFieldMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_DisplayFieldMode eDisplayFieldMode
         );

BERR_Code
BXDM_PictureProvider_GetDisplayFieldMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_DisplayFieldMode *peDisplayFieldMode
         );

#if 0
typedef enum BXDM_PictureProvider_SourceFormatOverride
{
      BXDM_PictureProvider_SourceFormatOverride_eDefault,
      BXDM_PictureProvider_SourceFormatOverride_eInterlaced,
      BXDM_PictureProvider_SourceFormatOverride_eProgressive,

      BXDM_PictureProvider_SourceFormatOverride_eMax
} BXDM_PictureProvider_SourceFormatOverride;
#endif

BERR_Code
BXDM_PictureProvider_SetSourceFormatOverride_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_SourceFormatOverride eSourceFormatOverride
         );

BERR_Code
BXDM_PictureProvider_GetSourceFormatOverride_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_SourceFormatOverride *peSourceFormatOverride
         );

#if 0
typedef enum BXDM_PictureProvider_ScanModeOverride
{
      BXDM_PictureProvider_ScanModeOverride_eDefault,
      BXDM_PictureProvider_ScanModeOverride_eInterlaced,
      BXDM_PictureProvider_ScanModeOverride_eProgressive,

      BXDM_PictureProvider_ScanModeOverride_eMax
} BXDM_PictureProvider_ScanModeOverride;
#endif

BERR_Code
BXDM_PictureProvider_SetScanModeOverride_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_ScanModeOverride eScanModeOverride
         );

BERR_Code
BXDM_PictureProvider_GetScanModeOverride_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_ScanModeOverride *peScanModeOverride
         );

BERR_Code
BXDM_PictureProvider_SetFreeze_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool bEnable
         );

BERR_Code
BXDM_PictureProvider_GetFreeze_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool *pbEnable
         );

#if 0
typedef enum BXDM_PictureProvider_240iScanMode
{
      /* Assume non-AVC 240i/288i has pulldown="F" (default) */
      BXDM_PictureProvider_240iScanMode_eForceProgressive,

      /* Assume non-AVC 240i/288i is coded correctly */
      BXDM_PictureProvider_240iScanMode_eUseEncodedFormat,

      BXDM_PictureProvider_240iScanMode_eMax
} BXDM_PictureProvider_240iScanMode;
#endif

BERR_Code
BXDM_PictureProvider_Set240iScanMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_240iScanMode e240iScanMode
         );

BERR_Code
BXDM_PictureProvider_Get240iScanMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_240iScanMode *pe240iScanMode
         );

#if 0
typedef enum BXDM_PictureProvider_PulldownMode
{
   BXDM_PictureProvider_PulldownMode_eTopBottom,
   BXDM_PictureProvider_PulldownMode_eBottomTop,
   BXDM_PictureProvider_PulldownMode_eUseEncodedFormat,

   BXDM_PictureProvider_PulldownMode_eMax
} BXDM_PictureProvider_PulldownMode;
#endif

BERR_Code
BXDM_PictureProvider_Set480pPulldownMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_PulldownMode ePulldownMode
         );

BERR_Code
BXDM_PictureProvider_Get480pPulldownMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_PulldownMode *pePulldownMode
         );

BERR_Code
BXDM_PictureProvider_Set1080pPulldownMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_PulldownMode ePulldownMode
         );

BERR_Code
BXDM_PictureProvider_Get1080pPulldownMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_PulldownMode *pePulldownMode
         );

BERR_Code
BXDM_PictureProvider_SetPTSOffset_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         uint32_t uiOffset
         );

BERR_Code
BXDM_PictureProvider_GetPTSOffset_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         uint32_t *puiOffset
         );

#if 0
typedef enum BXDM_PictureProvider_DisplayMode
{
      BXDM_PictureProvider_DisplayMode_eTSM,   /* Use PTS for display timing */
      BXDM_PictureProvider_DisplayMode_eVirtualTSM, /* Ignore PTS and follow pulldown and frame rate */

      BXDM_PictureProvider_DisplayMode_eMax
} BXDM_PictureProvider_DisplayMode;
#endif

BERR_Code
BXDM_PictureProvider_SetDisplayMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_DisplayMode eDisplayMode
         );

BERR_Code
BXDM_PictureProvider_GetDisplayMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_DisplayMode *peDisplayMode
         );

#if 0
typedef struct BXDM_PictureProvider_TSMThresholdSettings
{
   uint32_t uiTooEarlyThreshold;
   uint32_t uiTooLateThreshold;
   uint32_t uiDeltaStcPtsDiffThreshold;
} BXDM_PictureProvider_TSMThresholdSettings;
#endif

BERR_Code
BXDM_PictureProvider_GetDefaultTSMThresholdSettings_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_TSMThresholdSettings *pstTSMThresholdSettings
         );

BERR_Code
BXDM_PictureProvider_SetTSMThresholdSettings_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         const  BXDM_PictureProvider_TSMThresholdSettings *pstTSMThresholdSettings
         );

BERR_Code
BXDM_PictureProvider_GetTSMThresholdSettings_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_TSMThresholdSettings *pstTSMThresholdSettings
         );

/* These don't necessarily fit into PictureProvider role */
BERR_Code
BXDM_PictureProvider_SetSTCValid_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool bValid
         );

BERR_Code
BXDM_PictureProvider_GetSTCValid_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool *pbValid
         );


#if 0
typedef enum BXDM_PictureProvider_ClipTimeType
{
   /* Disables all clip logic. If clipping is currently active, it
    * will be disabled and normal TSM will resume. If a clip time was
    * previously set (but not yet used), it will be reset */
      BXDM_PictureProvider_ClipTimeType_eDisable = 0,

   /* Both clip start and stop times are valid w/ DVD specific
    * functionality:
      - Pictures causing PTS Errors are displayed in vsync mode
      - Start Time of 0 means start immediately
      - Stop Time of 0xFFFFFFFF means stop on a marker
      - Stop Event:
         - TSM Mode: STC value is compared to stop time
         - vTSM Mode: PTS value is compared to stop time
         - EOC/VSYNC Mode: stop time is ignored.  Marker is used to
           indicate stop.
   */
      BXDM_PictureProvider_ClipTimeType_eEnableBothWithDVDCompatibility,

   /* Both clip start and stop times are valid */
      BXDM_PictureProvider_ClipTimeType_eEnableBoth,

   /* Only clip start time is valid */
      BXDM_PictureProvider_ClipTimeType_eClipStartOnly,

   /* Only clip stop time is valid */
      BXDM_PictureProvider_ClipTimeType_eClipStopOnly,

      BXDM_PictureProvider_ClipTimeType_eMax
} BXDM_PictureProvider_ClipTimeType;

typedef struct BXDM_PictureProvider_ClipTimeSettings
{
      bool bValid;

      BXDM_PictureProvider_ClipTimeType eType;
      uint32_t uiStart;
      uint32_t uiStop;
      uint32_t uiId;
} BXDM_PictureProvider_ClipTimeSettings;
#endif

BERR_Code
BXDM_PictureProvider_GetDefaultClipTimeSettings_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_ClipTimeSettings *pstClipTimeSettings
         );

BERR_Code
BXDM_PictureProvider_SetClipTimeSettings_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         const BXDM_PictureProvider_ClipTimeSettings *pstClipTimeSettings
         );

BERR_Code
BXDM_PictureProvider_SetSoftwarePCROffset_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         uint32_t uiOffset
         );

BERR_Code
BXDM_PictureProvider_GetSoftwarePCROffset_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         uint32_t *puiOffset
         );

BERR_Code
BXDM_PictureProvider_SetHardwarePCROffsetMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool bEnable
         );

BERR_Code
BXDM_PictureProvider_GetHardwarePCROffsetMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool *pbEnable
         );

BERR_Code
BXDM_PictureProvider_SetPlaybackRate_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         const BXDM_Picture_Rate *pstPlaybackRate
         );

BERR_Code
BXDM_PictureProvider_GetPlaybackRate_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_Picture_Rate *pstPlaybackRate
         );

#if 0
typedef enum BXDM_PictureProvider_PictureDropMode
{
      BXDM_PictureProvider_PictureDropMode_eField,
      BXDM_PictureProvider_PictureDropMode_eFrame,

      /* Add more enums ABOVE this line */
      BXDM_PictureProvider_PictureDropMode_eMax
} BXDM_PictureProvider_PictureDropMode;
#endif

BERR_Code
BXDM_PictureProvider_SetPictureDropMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_PictureDropMode ePictureDropMode
         );

BERR_Code
BXDM_PictureProvider_GetPictureDropMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_PictureDropMode *pePictureDropMode
         );

#if 0
typedef enum BXDM_PictureProvider_PTSType
{
      BXDM_PictureProvider_PTSType_eCoded,
      BXDM_PictureProvider_PTSType_eInterpolatedFromValidPTS,
      BXDM_PictureProvider_PTSType_eHostProgrammedPTS_Deprecated,
      BXDM_PictureProvider_PTSType_eInterpolatedFromInvalidPTS,

      BXDM_PictureProvider_PTSType_eMax
} BXDM_PictureProvider_PTSType;

typedef struct BXDM_PictureProvider_PTSInfo
{
      /* generally the PTS of the picture being delivered to VDC. */
      uint32_t ui32RunningPTS;

      /* The effective PTS value (accounting for all offsets) that DM
       * uses for TSM evaluation */
      uint32_t ui32EffectivePTS;

      /* The PTS type tag */
      BXDM_PictureProvider_PTSType ePTSType;

      /* the STC delivered by the video decoder on this vsync. */
      uint32_t uiSTCSnapShot;

      /* PCR offset used by the TSM logic for the picture being delivered to VDC. */
      uint32_t uiPCROffset;

      /* Indicates the state of the 'uiPCROffset' field */
      bool bPcrOffsetValid;

      /* the PCR discontiniuty flag for the picture being delivered to VDC. */
      bool bPcrDiscontinuity;

      /*
       * A running count of the number of elements (fields/frames) dropped by
       * the "Display Manager" due to "Time Stamp Management" failures.
       * Reset upon calling "BXVD_StartDecode()".
       */
      uint32_t uiDisplayManagerDroppedCount;

      /*
       * A running count of the number of pictures the decoder delivered to
       * the "Display Manager". Reset upon calling "BXVD_StartDecode()".
       */
      uint32_t uiPicturesDecodedCount;

      /* PR50623: A programmable offset added to the STC prior to TSM evaluation. */
      uint32_t uiSwPcrOffset;

      /* PR50623: A programmable flag which indicates whether or not to include the
       * HW PCR offset in the TSM equation.
       */
      bool bUseHwPcrOffset;
} BXDM_PictureProvider_PTSInfo;

typedef enum BXDM_PictureProvider_TSMResult
{
   BXDM_PictureProvider_TSMResult_eTooEarly, /* (PTS > STC)  and ((PTS - STC) > discard threshold) */
   BXDM_PictureProvider_TSMResult_eWait,     /* (PTS > STC)  and ((PTS - STC) <= discard threshold) */
   BXDM_PictureProvider_TSMResult_ePass,     /* (PTS <= STC) and ((STC - PTS) <= too late threshold) */
   BXDM_PictureProvider_TSMResult_eTooLate,  /* (PTS < STC)  and ((STC - PTS) > too late threshold) */
   BXDM_PictureProvider_TSMResult_eDrop,     /* Drop for non-TSM reason (freeze, pts filtering, invalid pts, invalid pcr, etc) */

   BXDM_PictureProvider_TSMResult_eMax
} BXDM_PictureProvider_TSMResult;

typedef enum BXDM_PictureProvider_PictureHandlingMode
{
   BXDM_PictureProvider_PictureHandlingMode_eDefault,   /* In Broadcast:
                                              - TooEarly --> eDrop
                                              - TooLate --> eHonorPTS
                                           In Playback:
                                              - TooEarly --> eHonorPTS for 3 vsyncs.  eDrop after 3 vsyncs.  Reset on non-TooEarly.
                                              - TooLate --> eIgnorePTS for first picture.  eHonorPTS for susequent pictures.  Reset on non-TooLate.
                                        */
   BXDM_PictureProvider_PictureHandlingMode_eHonorPTS,  /* Honor the PTS of the picture and assume it is correct */
   BXDM_PictureProvider_PictureHandlingMode_eIgnorePTS, /* Ignore the PTS of the picture and instead interpolate from the previous PTS */
   BXDM_PictureProvider_PictureHandlingMode_eDrop,      /* Drop the picture immediately */
   BXDM_PictureProvider_PictureHandlingMode_eWait,      /* SW7635-51: give the middleware/application the option to hold off processing a picture. */

   BXDM_PictureProvider_PictureHandlingMode_eMax
} BXDM_PictureProvider_PictureHandlingMode;

typedef struct BXDM_PictureProvider_TSMInfo
{
   BXDM_PictureProvider_TSMResult eTSMResult; /* The TSM Result for the currently evaluated picture.  *read only* */
   BXDM_PictureProvider_PictureHandlingMode ePictureHandlingMode; /* The picture handling mode for the currently evaluated picture */
} BXDM_PictureProvider_TSMInfo;

typedef struct BXDM_PictureProvider_Counters
{
   uint32_t uiDisplayManagerDroppedCount;  /* Number of Elements (Fields/Frames) DM decided to drop because of TSM. Running Counter. */
   uint32_t uiDecoderDroppedCount;         /* Number of Elements (Fields/Frames) DM asked the Decoder to drop. Running Counter. */
   uint32_t uiDisplayedParityFailureCount; /* Number of Elements (Fields/Frames) DM displayed eventhough there was Parity Failure, under normal display condition. Running Counter. */
   uint32_t uiDisplayedCount;              /* Number of Elements (Fields/Frames) DM delevered to VDC under normal display conditions. Running Counter. */
   uint32_t uiPicturesDecodedCount;        /* Number of Entries (PPBs) DM received from the AVD FW. Running Counter. */
   uint32_t uiVsyncUnderflowCount;         /* Number of Vsync the DM had Underflow in the Disp FIFO. Running Counter. */
   uint32_t uiDecodeErrorCount;            /* Number of Errors Decoder encountered. DM only passes this info as recieved from the AVD through the PPB */
   uint32_t uiVsyncCount;                  /* Running count, number of times DM callback has been executed for this channel. */
   uint32_t uiIFrameCount;                 /* Number of I frames DM received (inclusive of errors). Running Counter. */
   uint32_t uiErrorIFrameCount;            /* Number of I frames DM received with the error bit set. Running Counter. */
   uint32_t uiPicturesReceivedCount;       /* Number of pictures received from the decoder. */
   uint32_t uiPicturesReleasedCount;       /* Number of pictures released by the DM. */
   uint32_t uiUnderflowCount;              /* Number of times the delivery queue has gotten into an underflow state. */
} BXDM_PictureProvider_Counters;
#endif

BERR_Code
BXDM_PictureProvider_GetCounters_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_Counters *pstCounters
         );

BERR_Code
BXDM_PictureProvider_GetCurrentPTSInfo_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_PTSInfo *pPTSInfo
         );

BERR_Code
BXDM_PictureProvider_GetLastCodedPTSInfo_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_PTSInfo *pPTSInfo
         );

BERR_Code
BXDM_PictureProvider_GetNextPTSInfo_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_PTSInfo *pPTSInfo
         );

BERR_Code
BXDM_PictureProvider_GetIPictureFoundStatus_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool *pbFound
         );

BERR_Code
BXDM_PictureProvider_GetPictureTag_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         uint32_t *puiValue
         );

BERR_Code
BXDM_PictureProvider_GetCurrentTimeCode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_Picture_GopTimeCode *pstTimeCode
         );

BERR_Code
BXDM_PictureProvider_GetCurrentPicture_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_Picture *pPicture
         );

/* SW7425-3558: return a pointer to "stCurrentPPBParameterInfo",
 * the picture currently selected for display.
 * If the data isn't valid, "pPicture" is set to NULL.
 */
BERR_Code
BXDM_PictureProvider_GetCurrentPicturePtr_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         const BXDM_Picture **pPicture
         );

BERR_Code
BXDM_PictureProvider_SetPauseMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool bEnable
         );

BERR_Code
BXDM_PictureProvider_GetPauseMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool *pbEnable
         );

#if 0
typedef enum BXDM_PictureProvider_FrameAdvanceMode
{
      BXDM_PictureProvider_FrameAdvanceMode_eOff,
      BXDM_PictureProvider_FrameAdvanceMode_eField,
      BXDM_PictureProvider_FrameAdvanceMode_eFrame,
      BXDM_PictureProvider_FrameAdvanceMode_eFrameByField,

      BXDM_PictureProvider_FrameAdvanceMode_eMax
} BXDM_PictureProvider_FrameAdvanceMode;
#endif

BERR_Code
BXDM_PictureProvider_SetFrameAdvanceMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_FrameAdvanceMode eFrameAdvanceMode
      );

#if 0
typedef struct BXDM_PictureProvider_PreserveStateSettings
{
      bool bDisplay;
      bool bCounters;
} BXDM_PictureProvider_PreserveStateSettings;
#endif

BERR_Code
BXDM_PictureProvider_GetDefaultPreserveStateSettings(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_PreserveStateSettings *pstSettings
         );

BERR_Code
BXDM_PictureProvider_SetPreserveStateSettings_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         const BXDM_PictureProvider_PreserveStateSettings *pstSettings
         );

BERR_Code
BXDM_PictureProvider_GetPreserveStateSettings_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_PreserveStateSettings *pstSettings
         );

BERR_Code
BXDM_PictureProvider_SetReverseFieldsMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool bEnable
         );

BERR_Code
BXDM_PictureProvider_GetReverseFieldsMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool *pbEnable
         );

BERR_Code
BXDM_PictureProvider_SetAutoValidateStcOnPauseMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool bEnable
      );

BERR_Code
BXDM_PictureProvider_GetAutoValidateStcOnPauseMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool *pbEnable
      );

BERR_Code
BXDM_PictureProvider_SetProtocol_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BAVC_VideoCompressionStd eProtocol
         );

BERR_Code
BXDM_PictureProvider_GetProtocol_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BAVC_VideoCompressionStd *peProtocol
         );

BERR_Code
BXDM_PictureProvider_SetJitterToleranceImprovementMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool bEnable
         );

BERR_Code
BXDM_PictureProvider_GetJitterToleranceImprovementMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool *pbEnable
         );

#if 0
typedef enum BXDM_PictureProvider_MonitorRefreshRate
{
   BXDM_PictureProvider_MonitorRefreshRate_eUnknown = 0,
   BXDM_PictureProvider_MonitorRefreshRate_e23_976Hz,
   BXDM_PictureProvider_MonitorRefreshRate_e24Hz,
   BXDM_PictureProvider_MonitorRefreshRate_e25Hz,
   BXDM_PictureProvider_MonitorRefreshRate_e29_97Hz,
   BXDM_PictureProvider_MonitorRefreshRate_e30Hz,
   BXDM_PictureProvider_MonitorRefreshRate_e50Hz,
   BXDM_PictureProvider_MonitorRefreshRate_e59_94Hz,
   BXDM_PictureProvider_MonitorRefreshRate_e60Hz,
   BXDM_PictureProvider_MonitorRefreshRate_e48Hz,

   /* Add new enums above this line */
   BXDM_PictureProvider_MonitorRefreshRate_eMax
} BXDM_PictureProvider_MonitorRefreshRate;
#endif

BERR_Code
BXDM_PictureProvider_SetMonitorRefreshRate_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_MonitorRefreshRate eMonitorRefreshRate
         );

BERR_Code
BXDM_PictureProvider_GetMonitorRefreshRate_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_MonitorRefreshRate *peMonitorRefreshRate
         );

#if 0
typedef enum BXDM_PictureProvider_1080pScanMode
{
      /* eDefault: 1080p pass-thru, else 1080i
       *
       * 1080p pass-thru cases:
       *   - 1080p23.97/24 --> 23.97/24 Hz
       *   - 1080p25 --> 25 Hz
       *   - 1080p29.97/30 --> 29.97/30 Hz
       */
       BXDM_PictureProvider_1080pScanMode_eDefault = 0,

      /* eAdvanced: 1080p pass-thru + 1080p pulldown, else 1080i
       *
       * 1080p pulldown cases:
       *   - 1080p23.97/24 --> 59.94/60 Hz (3:2 frame cadence)
       *   - 1080p29.97/30 --> 59.94/60 Hz (2:2 frame cadence)
       *   - 1080p25 --> 50Hz (2:2 frame cadence)
       */
       BXDM_PictureProvider_1080pScanMode_eAdvanced,

      /* Add new enums ABOVE this line */
       BXDM_PictureProvider_1080pScanMode_eMax
} BXDM_PictureProvider_1080pScanMode;
#endif

BERR_Code
BXDM_PictureProvider_Set1080pScanMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_1080pScanMode eScanMode
         );

BERR_Code
BXDM_PictureProvider_Get1080pScanMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_1080pScanMode *peScanMode
         );

BERR_Code
BXDM_PictureProvider_SetSTCIndex_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         uint32_t uiValue
         );

BERR_Code
BXDM_PictureProvider_GetSTCIndex_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         uint32_t *puiValue
         );

BERR_Code
BXDM_PictureProvider_SetCRCMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool bEnable
         );

BERR_Code
BXDM_PictureProvider_GetCRCMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool *pbEnable
         );

BERR_Code
BXDM_PictureProvider_SetRemovalDelay_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         uint32_t uiValue
         );

BERR_Code
BXDM_PictureProvider_GetRemovalDelay_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         uint32_t *puiValue
         );

BERR_Code
BXDM_PictureProvider_SetPreRollRate_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         const BXDM_Picture_Rate *pstPreRollRate
         );

BERR_Code
BXDM_PictureProvider_GetPreRollRate_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_Picture_Rate *pstPreRollRate
         );

BERR_Code
BXDM_PictureProvider_SetPlaybackMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool bEnable
         );

BERR_Code
BXDM_PictureProvider_GetPlaybackMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool *pbEnable
         );

BERR_Code
BXDM_PictureProvider_SetDefaultFrameRate_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BAVC_FrameRateCode eFrameRate
         );

BERR_Code
BXDM_PictureProvider_GetDefaultFrameRate_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BAVC_FrameRateCode *peFrameRate
         );

#if 0
typedef enum BXDM_PictureProvider_FrameRateDetectionMode
{
   /* Disable frame rate detection.  Will always return unknown */
      BXDM_PictureProvider_FrameRateDetectionMode_eOff,

   /* Fast frame rate detection.  Will return best guess as soon as
    * possible.  Result may oscillate initially for very close frame
    * rates (e.g. 29.97 vs 30 fps) */
      BXDM_PictureProvider_FrameRateDetectionMode_eFast,

   /* Stable frame rate detection.  Will return "unknown" until frame
    * rate stabilizes.  Good for differentiating between close frame
    * rates (e.g. 29.97 vs 30 fps) */
      BXDM_PictureProvider_FrameRateDetectionMode_eStable,

      BXDM_PictureProvider_FrameRateDetectionMode_eMax

} BXDM_PictureProvider_FrameRateDetectionMode;
#endif

BERR_Code
BXDM_PictureProvider_SetFrameRateDetectionMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_FrameRateDetectionMode eFrameRateDetectionMode
         );

BERR_Code
BXDM_PictureProvider_GetFrameRateDetection_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_FrameRateDetectionMode *peFrameRateDetectionMode
         );

BERR_Code
BXDM_PictureProvider_SetASTMMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool bEnable
         );

BERR_Code
BXDM_PictureProvider_GetASTMMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool *pbEnable
         );

BERR_Code
BXDM_PictureProvider_SetVirtualTSMOnPCRDiscontinuityMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool bEnable
         );

BERR_Code
BXDM_PictureProvider_GetVirtualTSMOnPCRDiscontinuityMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool *pbEnable
         );

#if 0
typedef enum BXDM_PictureProvider_ErrorHandlingMode
{
      BXDM_PictureProvider_ErrorHandlingMode_eOff, /* display all pictures */
      BXDM_PictureProvider_ErrorHandlingMode_ePicture, /* drop pictures if Decode Error is set  */
      BXDM_PictureProvider_ErrorHandlingMode_ePrognostic, /* drop pictures from Decode Error to the next RAP */

      BXDM_PictureProvider_ErrorHandlingMode_eMax
} BXDM_PictureProvider_ErrorHandlingMode;
#endif

BERR_Code
BXDM_PictureProvider_SetErrorHandlingMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_ErrorHandlingMode eErrorHandlingMode
         );

BERR_Code
BXDM_PictureProvider_GetErrorHandlingMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_ErrorHandlingMode *peErrorHandlingMode
         );

BERR_Code
BXDM_PictureProvider_SetTimerHandle_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BTMR_TimerHandle hTimer
         );

BERR_Code
BXDM_PictureProvider_GetTimerHandle_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BTMR_TimerHandle *phTimer
         );

BERR_Code
BXDM_PictureProvider_SetChannelSyncMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool bEnable
         );

BERR_Code
BXDM_PictureProvider_GetChannelSyncMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool *pbEnable
         );

BERR_Code
BXDM_PictureProvider_SetDIH(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_DisplayInterruptHandler_Handle hXdmDih
         );

BERR_Code
BXDM_PictureProvider_GetDIH(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_DisplayInterruptHandler_Handle *phXdmDih
         );

BERR_Code
BXDM_PictureProvider_SetDeinterlacerMaxHeight(
         BXDM_PictureProvider_Handle hXdmPP,
         uint32_t uiMaxHeight
         );

BERR_Code
BXDM_PictureProvider_GetDeinterlacerMaxHeight(
         BXDM_PictureProvider_Handle hXdmPP,
         uint32_t * puiMaxHeight
         );

#if 0
/* SW7405-4703: Add APIs for controlling Horizontal Overscan processing */
typedef enum BXDM_PictureProvider_HorizontalOverscanMode
{
   BXDM_PictureProvider_HorizontalOverscanMode_eAuto,
   BXDM_PictureProvider_HorizontalOverscanMode_eDisable,

   /* Add more enums ABOVE this line */
   BXDM_PictureProvider_HorizontalOverscanMode_eMax
} BXDM_PictureProvider_HorizontalOverscanMode;
#endif

BERR_Code
BXDM_PictureProvider_SetHorizontalOverscanMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_HorizontalOverscanMode eHorizontalOverscanMode
         );

BERR_Code
BXDM_PictureProvider_GetHorizontalOverscanMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_HorizontalOverscanMode *peHorizontalOverscanMode
         );

#if 0
/* SWDEPRECATED-1003: needed to turn off the FIC logic during certain trick modes */
typedef enum BXDM_PictureProvider_TrickMode
{
   BXDM_PictureProvider_TrickMode_eAuto=0,
   BXDM_PictureProvider_TrickMode_eNormal,
   BXDM_PictureProvider_TrickMode_eSparsePictures,
   BXDM_PictureProvider_TrickMode_ePause,
   BXDM_PictureProvider_TrickMode_eRewind,
   BXDM_PictureProvider_TrickMode_eFastForward,

   /* Add more enums ABOVE this line */
   BXDM_PictureProvider_TrickMode_eMax
} BXDM_PictureProvider_TrickMode;
#endif

BERR_Code 
BXDM_PictureProvider_SetTrickMode_isr(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_TrickMode eTrickMode
   );

BERR_Code 
BXDM_PictureProvider_GetTrickMode_isr(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_TrickMode * peTrickMode
   );

BERR_Code
BXDM_PictureProvider_SetFrameRateOverride_isr(
   BXDM_PictureProvider_Handle hXdmPP,
   const BXDM_Picture_FrameRateOverride *pstFrameRateOverride
   );

BERR_Code
BXDM_PictureProvider_GetFrameRateOverride_isr(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_Picture_FrameRateOverride *pstFrameRateOverride
   );

BERR_Code
BXDM_PictureProvider_SetInstanceID_isr(
   BXDM_PictureProvider_Handle hXdmPP,
   uint32_t uiInstanceID
   );

BERR_Code
BXDM_PictureProvider_GetInstanceID_isr(
   BXDM_PictureProvider_Handle hXdmPP,
   uint32_t * puiInstanceID
   );


/* SW7422-72: API's and constants to allow the middleware/application to specify 
 * an orientation for each picture.  If "bOverrideOrientation" is set
 * in the BXDM_PictureProvider_3DSettings structure, the "eOrientation" field
 * will be used to fill in the MFD structure.  
 * If "bOverrideOrientation" is not set, the MFD will be filled in based on
 * data received from AVD, i.e. SEI messages and/or PPB flags.
 * "bSetNextPointer" is to support MVC on the older DVD chips.  When true, 
 * both of the MFD pointers "pNext" and "pEnhanced" will be set.
 */

#if 0
typedef enum BXDM_PictureProvider_Orientation
{
   BXDM_PictureProvider_Orientation_e2D=0,
   BXDM_PictureProvider_Orientation_eLeftRight,
   BXDM_PictureProvider_Orientation_eOverUnder,
   BXDM_PictureProvider_Orientation_eLeftRightFullFrame,
   BXDM_PictureProvider_Orientation_eRightLeftFullFrame,
   BXDM_PictureProvider_Orientation_eLeftRightEnhancedResolution,

   BXDM_PictureProvider_Orientation_eMax

} BXDM_PictureProvider_Orientation;

typedef struct BXDM_PictureProvider_3DSettings
{
   bool bOverrideOrientation;
   BXDM_PictureProvider_Orientation eOrientation;

   bool bSetNextPointer;

} BXDM_PictureProvider_3DSettings;
#endif

BERR_Code 
BXDM_PictureProvider_Set3D_isr(
   BXDM_PictureProvider_Handle hXdmPP,
   const BXDM_PictureProvider_3DSettings * pst3DSettings
   );

BERR_Code 
BXDM_PictureProvider_Get3D_isr(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_3DSettings * pst3DSettings
   );

BERR_Code 
BXDM_PictureProvider_GetDefault3D(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_3DSettings * pst3DSettings
   );

/*
 * SW7425-1264: support for a synthesized SW STC, can create a clock internal 
 * to XDM that run backwards.
 *
 * The clock override is set by calling "BXDM_PictureProvider_SetClockOverride_isr".
 * "BXDM_PictureProvider_GetClockOverride_isr" returns the most recently written values.
 *
 * When "bEnableClockOverride" is true, XDM will use an internal SW STC.
 *
 * When "bLoadSwStc" is true, the SW STC will be initialized with "uiStcValue".
 * This loading occurs at the beginning of XDM's execution.  If "bLoadSwStc" 
 * is false, the SW STC will not be loaded.  This mechanism allows 
 * "BXDM_PictureProvider_SetClockOverride_isr" to be called without changing the
 * current value of the SW STC.
 * 
 * "iStcDelta" is added to the SW STC every vsync, effectively at the end of XDM's execution.  
 * "iStcDelta" can be negative, this will cause the clock to run backwards.  
 * The units of "iStcDelta" are clock ticks; for example to achieve a 60 Hz display rate,
 * "iStcDelta" would be set to 750.
 *
 * The current SW STC can be retrieved by calling BXDM_PictureProvider_GetCurrentPTSInfo_isr
 * or BXVD_GetPTS_isr with "bEnableClockOverride" set to true.  The SW STC will be returned in
 * "stPTSInfo.uiSTCSnapShot".
 */
#if 0
typedef struct BXDM_PictureProvider_ClockOverride
{
   /* When true, XVD will use an internal SW STC */
   bool       bEnableClockOverride;

   /* Initial value of the internal SW STC */
   uint32_t   uiStcValue;

   /* When true, the internal SW STC will be loaded with uiStcValue.
    * If false, the uiStcValue will not be changed.
    */
   bool       bLoadSwStc;

   /* Added to the internal STC on each vsync.
    * A negative value will cause the STC to decrement
    */
   int32_t    iStcDelta;

} BXDM_PictureProvider_ClockOverride;
#endif


BERR_Code
BXDM_PictureProvider_SetClockOverride_isr(
   BXDM_PictureProvider_Handle hXdmPP,
   const BXDM_PictureProvider_ClockOverride *pstClockOverride
   );


BERR_Code
BXDM_PictureProvider_GetClockOverride_isr(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_ClockOverride *pstClockOverride
   );

/* SW7425-2270:
 * The application will call SetIgnoreNRTUnderflow when it determines that an NRT underflow 
 * is actually a gap in the content (e.g. slideshow or end of stream) and the repeated picture 
 * should actually be encoded.
 *
 * When SetIgnoreNRTUnderflow=true, the "decoder underflow" scenario should be ignored until either: 
 * - the underflow condition ends 
 * - the app explicitly sets SetIgnoreNRTUnderflow=false 
 *
 * Note: only the "decoder underflow" condition is ignored. All other NRT scenarios 
 * (e.g. "Other Transcode Stalled", "FIC Stall", etc) are still in effect. 
 */
BERR_Code
BXDM_PictureProvider_SetIgnoreNRTUnderflow_isr(
   BXDM_PictureProvider_Handle hXdmPP,
   bool bIgnoreNRTUnderflow
   );

BERR_Code
BXDM_PictureProvider_GetIgnoreNRTUnderflow_isr(
   BXDM_PictureProvider_Handle hXdmPP,
   bool * pbIgnoreNRTUnderflow
   );

/*
 * SW7425-3358: support for FNRT.
 */
BERR_Code
BXDM_PictureProvider_SetFNRTSettings_isr(
   BXDM_PictureProvider_Handle hXdmPP,
   const BXDM_PictureProvider_FNRTSettings * pstFNRTSettings
   );

BERR_Code
BXDM_PictureProvider_GetFNRTSettings_isr(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_FNRTSettings * pstFNRTSettings
   );

BERR_Code
BXDM_PictureProvider_GetDefaultFNRTSettings_isr(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_FNRTSettings * pstFNRTSettings
   );

#include "bxdm_pp_callback.h"

#ifdef __cplusplus
}
#endif

#endif /* BXDM_PP_H_ */
