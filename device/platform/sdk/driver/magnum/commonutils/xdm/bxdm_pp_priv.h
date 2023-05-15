/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxdm_pp_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/48 $
 * $brcm_Date: 8/15/12 9:24a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/xdm/bxdm_pp_priv.h $
 * 
 * Hydra_Software_Devel/48   8/15/12 9:24a btosi
 * SW7405-5683:  Disable the FIC and force eSingleField mode when
 * delivering interlaced content to a display running at <= 30 Hz.
 * 
 * Hydra_Software_Devel/47   7/27/12 12:57p btosi
 * SW7425-1064: added message filter to BXDM_PPOUT_S_3DFixup
 * 
 * Hydra_Software_Devel/46   7/25/12 9:45a btosi
 * SW7425-3558: added BXDM_PictureProvider_GetCurrentPicturePtr_isr
 * 
 * Hydra_Software_Devel/45   7/6/12 10:40a btosi
 * SW7425-3358: added support for FNRT
 * 
 * Hydra_Software_Devel/44   7/2/12 9:53a btosi
 * SW7425-3358: first cut at the FNRT changes
 * 
 * Hydra_Software_Devel/43   6/13/12 11:21a btosi
 * SW7425-3177: added support frame rates e10,e15,e20.
 * 
 * Hydra_Software_Devel/42   5/4/12 3:35p btosi
 * SW7425-2915: fixed issue with 2nd slot delay at startup and added
 * additional debug support for NRT mode
 * 
 * Hydra_Software_Devel/41   3/22/12 8:42a btosi
 * SW7425-2656: when paused during a flush, prevent the next picture from
 * being displayed until unpaused
 * 
 * Hydra_Software_Devel/40   3/16/12 11:56a btosi
 * SW7425-2536: added support for displayInterruptStatus_isr
 * 
 * Hydra_Software_Devel/39   2/21/12 4:37p btosi
 * SW7425-2424: force the initialization of
 * ePolarityWhenDeltaStcWasNonZero
 * 
 * Hydra_Software_Devel/38   2/16/12 9:29a btosi
 * SW7425-2255: bind the PTS offset to the picture when it is selected for
 * display
 * 
 * Hydra_Software_Devel/37   2/1/12 1:57p btosi
 * SW7425-2270: add support for IgnoreNRTUnderflow
 * 
 * Hydra_Software_Devel/36   1/17/12 10:42a btosi
 * SW7405-4736: added support for frame rate type and playback rate
 * 
 * Hydra_Software_Devel/35   1/16/12 3:21p nilesh
 * SW7425-767: Simplificed bStallStc and bIgnorePicture logic
 * 
 * Hydra_Software_Devel/34   1/12/12 12:57p btosi
 * SW7425-1001: added support for BAVC_MFD_Picture.bLast
 * 
 * Hydra_Software_Devel/33   1/10/12 4:37p nilesh
 * SW7425-767: Merge to mainline
 * 
 * Hydra_Software_Devel/SW7425-767/1   1/4/12 10:43a nilesh
 * SW7425-767: Prevent field inversion when STC is stalled in NRT mode
 * 
 * Hydra_Software_Devel/32   1/5/12 10:48a btosi
 * SW7405-5549: re-apply the CDT when the monitor refesh rate changes
 * while the system is paused
 * 
 * Hydra_Software_Devel/31   10/3/11 2:19p btosi
 * SW7425-1264:  support for a SW STC internal to XDM
 * 
 * Hydra_Software_Devel/30   7/20/11 10:44a btosi
 * SW7635-51: use the SW PCR offset bound to the picture
 * 
 * Hydra_Software_Devel/29   7/20/11 9:28a btosi
 * SW7635-51: snapshot the software PCR offset
 * 
 * Hydra_Software_Devel/28   6/24/11 4:05p btosi
 * SW7425-764: added support for eInterruptRefreshRate in the MFD
 * structure
 * 
 * Hydra_Software_Devel/27   6/21/11 9:45a btosi
 * SW7425-104: merged bRepeatField changes to mainline
 * 
 * Hydra_Software_Devel/SW7425-104/1   6/20/11 1:43p btosi
 * SW7425-104: added support for the MFD bRepeatField flag
 * 
 * Hydra_Software_Devel/26   6/7/11 4:59p nilesh
 * SW7425-44: Add support for BAVC_MFD_Picture.bStallStc
 * 
 * Hydra_Software_Devel/25   5/25/11 4:04p nilesh
 * SW7425-44: Add support for BAVC_MFD_Picture.bIgnorePicture
 * 
 * Hydra_Software_Devel/24   5/10/11 1:41p btosi
 * SW7405-4736: added range checking and debug messages for the input
 * parameters
 * 
 * Hydra_Software_Devel/23   4/18/11 1:04p btosi
 * SW7405-4736: save the average STC delta for use in the debug messages
 * 
 * Hydra_Software_Devel/22   4/5/11 2:26p btosi
 * SW7405-4736: removed BXDM_PP_ENABLE_YUV_GRAB_MODE, grab is implemented
 * in bxvd_decoder.c
 * 
 * Hydra_Software_Devel/21   12/21/10 4:26p delkert
 * SW7405-5043: Split STC stored in local state into two parts: STC from
 * decoder and STC after JTI adjustment.
 * Changes for rate match detection
 * Store STC jitter threshold
 *
 * Hydra_Software_Devel/20   11/17/10 9:37a btosi
 * SW7422-72: added new BXVD and XDM 3D API's
 *
 * Hydra_Software_Devel/19   10/1/10 2:51p btosi
 * SWDEPRECATED-1003: added support for bTreatAsSingleElement
 *
 * Hydra_Software_Devel/18   9/28/10 3:04p btosi
 * SWBLURAY-22390: for BD, don't load parameters for the next clip until
 * both the stop event has occurred and the marker has been seen.
 *
 * Hydra_Software_Devel/17   9/23/10 3:09p btosi
 * SW7405-4736: add support for a XDM instance ID to help debug multi-
 * channel issues
 *
 * Hydra_Software_Devel/16   9/22/10 3:11p btosi
 * SW7405-4883: removed unused variable "bProgressiveScanout"
 *
 * Hydra_Software_Devel/15   9/15/10 12:06p btosi
 * SWDEPRECATED-1003: added BXDM_PictureProvider_SetFrameRateOverride_isr
 * and supporting logic
 *
 * Hydra_Software_Devel/14   9/14/10 11:56a btosi
 * SWDEPRECATED-1003: added BXDM_PictureProvider_SetTrickMode_isr to
 * primarily disable the FIC logic when receiving video with sparse
 * pictures
 *
 * Hydra_Software_Devel/13   8/12/10 9:38a btosi
 * SW7405-4736: modified QM debug messages
 *
 * Hydra_Software_Devel/12   8/5/10 8:29a delkert
 * SW7405-4703: Add HorizontalOverscanMode to allow bypass of Overscan
 * calculation.  Added appropriate Get/Set APIs
 *
 * Hydra_Software_Devel/11   8/4/10 1:03p btosi
 * SWBLURAY-21565: to aid debug, increment the picture's uiPPBIndex when
 * the picture is validated
 *
 * Hydra_Software_Devel/10   7/29/10 11:22a btosi
 * SW7405-4560: added support for the SEI "new message" flag.
 *
 * Hydra_Software_Devel/9   7/20/10 2:57p btosi
 * SWBLURAY-21565: only allow the last picture of a clip to generate one
 * stop event
 *
 * Hydra_Software_Devel/8   7/12/10 8:05a delkert
 * SW7335-781: Output warning when forcing picture selection override to
 * allow easier detection of conditions such as drop@decode and pcr
 * discontinuity
 *
 * Hydra_Software_Devel/7   6/8/10 3:14p btosi
 * SW7405-4378: fixed reporting of source size by the PictureParameter
 * callback when the first picture(s) is dropped
 *
 * Hydra_Software_Devel/6   5/11/10 1:33p btosi
 * SW7405-4117: added usage note
 *
 * Hydra_Software_Devel/5   5/10/10 3:05p btosi
 * SW7550-379: fixed issue with the change for SW7405-3774
 *
 * Hydra_Software_Devel/4   4/29/10 10:47a btosi
 * SW7405-4117: added support for ...DisplayFieldMode_eAuto
 *
 * Hydra_Software_Devel/3   3/18/10 10:54a btosi
 * SW7405-3774: added bExecutedPtsStcOffsetCB flag.
 *
 * Hydra_Software_Devel/2   2/19/10 11:09a nilesh
 * SW7405-2993: Made the DIH handle a property of the picture provider so
 * that the PDR callback can persist over a stop/start decode
 *
 * Hydra_Software_Devel/1   2/16/10 10:51a nilesh
 * SW7405-2993: Initial XDM version
 *
 ***************************************************************************/

#ifndef BXDM_PP_PRIV_H_
#define BXDM_PP_PRIV_H_

#include "bxdm_pp_fp.h"
#include "bxdm_pp_jrc.h"
#include "bxdm_pp_avg.h"
#include "bxdm_pp_timer.h"

#ifndef BXVD_DBG_MSG
#define BXVD_DBG_MSG(a,b) BSTD_UNUSED(a); BDBG_MSG(b)
#endif

#ifndef BXVD_DBG_WRN
#define BXVD_DBG_WRN(a,b) BSTD_UNUSED(a); BDBG_WRN(b)
#endif

#ifndef BXVD_DBG_ERR
#define BXVD_DBG_ERR(a,b) BSTD_UNUSED(a); BDBG_ERR(b)
#endif

/* SW7425-3177: create an XDM specific define to force a warning 
 * when new BAVC_FrameRateCode enums are added.
 */
#define BXDM_PictureProvider_P_MAX_FRAMERATE ( BAVC_FrameRateCode_e20 + 1 )

/* Default TSM Thresholds for STB */
#define BXDM_PictureProvider_P_DISCARD_THRESHOLD_MPEG (750*60*2)  /* 60 fields, 2 seconds */
#define BXDM_PictureProvider_P_DISCARD_THRESHOLD_AVC  (750*60*10) /* 60 fields, 10 seconds */
#define BXDM_PictureProvider_P_DISCARD_THRESHOLD_VC1  (750*60*10) /* 60 fields, 10 seconds */
#define BXDM_PictureProvider_P_VERYLATE_THRESHOLD     (750*30)    /* 30 fields, 1/2 second */

/* Maximum possible number of picture elements per PPB. */
#define BXDM_PictureProvider_P_MAX_ELEMENTS_PER_PPB    4
#define BXDM_PictureProvider_P_MAX_PPB_PULLDOWN_HISTORY 4

/* In support of defining playback rate as a percentage instead of a factor.
 * Previously "1" was normal, "2' as half speed ....
 * Values less than BXDM_PP_P_NORMAL_PLAYBACK_RATE are slow motion, greater
 * than BXDM_PP_P_NORMAL_PLAYBACK_RATE are fast play.
 */
#define BXDM_PICTUREPROVIDER_NORMAL_PLAYBACK_RATE_PERCENTAGE     100
#define BXDM_PICTUREPROVIDER_NORMAL_PLAYBACK_RATE_EXTRA_DECIMALS 10

#define BXDM_PICTUREPROVIDER_NORMAL_PLAYBACK_RATE ( BXDM_PICTUREPROVIDER_NORMAL_PLAYBACK_RATE_PERCENTAGE * BXDM_PICTUREPROVIDER_NORMAL_PLAYBACK_RATE_EXTRA_DECIMALS )

typedef enum BXDM_PictureProvider_P_PictureType
{
      BXDM_PictureProvider_P_PictureType_eProgressive,
      BXDM_PictureProvider_P_PictureType_eInterlaced,

      BXDM_PictureProvider_P_PictureType_eMax
} BXDM_PictureProvider_P_PictureType;

typedef enum BXDM_PictureProvider_P_ScanMode
{
   BXDM_PictureProvider_P_ScanMode_eInterlaced = 0,
   BXDM_PictureProvider_P_ScanMode_eProgressive,

   BXDM_PictureProvider_P_ScanMode_eMax
} BXDM_PictureProvider_P_ScanMode;

typedef enum BXDM_PictureProvider_P_RepeatMode
{
   BXDM_PictureProvider_P_RepeatMode_eField = 0,
   BXDM_PictureProvider_P_RepeatMode_eFrame,

   BXDM_PictureProvider_P_RepeatMode_eMax
} BXDM_PictureProvider_P_RepeatMode;

typedef enum BXDM_PictureProvider_P_ClockRate
{
   BXDM_PictureProvider_P_ClockRate_eMPEG2,
   BXDM_PictureProvider_P_ClockRate_eDirecTV,

   /* Add new values ABOVE this line */
   BXDM_PictureProvider_P_ClockRate_eMax
} BXDM_PictureProvider_P_ClockRate;

typedef enum BXDM_PictureProvider_P_PTSIndex
{
   BXDM_PictureProvider_P_PTSIndex_eActual,
   BXDM_PictureProvider_P_PTSIndex_eVirtual,

   /* Add new enums ABOVE this line */
   BXDM_PictureProvider_P_PTSIndex_eMax
} BXDM_PictureProvider_P_PTSIndex;

#define BXDM_PictureProvider_P_STC_JITTER_THRESHOLD 10

typedef enum BXDM_PictureProvider_P_STCTrickMode
{
   BXDM_PictureProvider_P_STCTrickMode_eOff = 0,
   BXDM_PictureProvider_P_STCTrickMode_eFastForward,
   BXDM_PictureProvider_P_STCTrickMode_eSlowMotion,
   BXDM_PictureProvider_P_STCTrickMode_ePause,
   BXDM_PictureProvider_P_STCTrickMode_eFastRewind,   /* SW7425-1264: two rewind modes can occur when */
   BXDM_PictureProvider_P_STCTrickMode_eSlowRewind,   /* the SW STC is enabled and run in reverse     */

   /* Add new values ABOVE this line */
   BXDM_PictureProvider_P_STCTrickMode_eMax
} BXDM_PictureProvider_P_STCTrickMode;

typedef struct BXDM_PictureProvider_P_ClipCallbackInfo
{
   bool bTriggered;
   BXDM_PictureProvider_Callback_ClipEventInfo stClipEventInfo;
} BXDM_PictureProvider_P_ClipCallbackInfo;

typedef struct BXDM_PictureProvider_P_CallbackInfo
{
   BXDM_PictureProvider_P_ClipCallbackInfo stClipStart;
   BXDM_PictureProvider_P_ClipCallbackInfo stClipStop;
   BXDM_PictureProvider_P_ClipCallbackInfo stPictureMarker;
} BXDM_PictureProvider_P_CallbackInfo;

/* SW7405-3137: to support MVC and other protocols delivering sets of pictures.  */
typedef enum BXDM_PictureProvider_P_InterruptType
{
      BXDM_PictureProvider_P_InterruptType_eSingle,     /* VDC is expecting a single picture on this vsync */
      BXDM_PictureProvider_P_InterruptType_eBase,       /* VDC is expecting the base picture of a set on this vsync */
      BXDM_PictureProvider_P_InterruptType_eDependent,  /* VDC is expecting a dependent picture of a set on this vsync */
      BXDM_PictureProvider_P_InterruptType_eMax
} BXDM_PictureProvider_P_InterruptType;

typedef struct BXDM_PictureProvider_P_LocalState
{
      /* Updated on every vsync */
      BAVC_Polarity eVsyncPolarity; /* the vsync/interrupt polarity */
      BXDM_PictureProvider_P_InterruptType eInterruptType;
      BXDM_PictureProvider_P_ClockRate eClockRate;

      uint32_t uiSlowMotionRate; /* 0 = pause, 1 = normal, >1 = slow motion rate */

      /*
      ** The "local" callback flags.  These will be set to "true" when the
      ** callback criteria has been met.
      */

      bool bFirstPPBReceived; /* formerly the "bFirstPTSReady" flag */
      bool bCodedPtsReceived; /* formerly the "bFirstCodedPTS" flag */

      bool bTSMPass;
      bool bPtsError;
      bool bPtsStcOffset;
      bool bIFrameReceived;
      bool bPictureParameters;
      bool bASTMModeTSMPass;
      bool bPPBReceived;
      bool bPictureUnderEvaluation;
      bool bDecodeError;

      BXDM_PictureProvider_DisplayMode eDisplayMode;
      BXDM_PictureProvider_DisplayMode eSelectionMode;

      bool bForcePtsError;

      uint32_t uiStcSnapshot;    /* The STC Snapshot */
      uint32_t uiAdjustedStc;    /* STC after jitter correction (does not include PCR Offset) */

      /* PR47521: Keep track of actual new PPBs that were evaluated on
       * this vsync */
      uint32_t uiPicturesEvaluated;

      bool bPictureQueueIsEmpty;

      /*  PR47453: A running calculation of the lowest ABS of the
       *  StcPtsDifferenceEvaluated of the element displayed in the
       *  previous vsync and the element(s) that have been evaluated
       *  so far on this vsync.  Typically, to prevent STC/PTS jitter,
       *  the element that results in the lowest delta in the
       *  PtsStcDifferenceEvaluated is selected */
      int32_t iStcPtsDifferenceEvaluatedDeltaAbsMin[BXDM_PictureProvider_P_PTSIndex_eMax];
      int32_t iStcPtsDifferenceEvaluatedBest[BXDM_PictureProvider_P_PTSIndex_eMax];

      /* PR48593: True if the Field Inversion Correction offset has
       * changed this vsync */
      bool bFICChanged;

   uint32_t uiCallbackTriggered;

   BXDM_PPFP_P_DataType stSTCDelta;
   BXDM_PPFP_P_DataType stDeltaSTCAvg;    /* The average STC is calculated for use in the Jitter Tolerance Improvement  */
   uint32_t             uiNumSTCSamples;  /* logic. Save the values in this structure for use in the debug messages.    */

   BXDM_PictureProvider_P_STCTrickMode eSTCTrickMode;
   uint32_t uiSTCJitterThreshold;

   BXDM_PictureProvider_P_CallbackInfo stCallbackInfo;

   BXDM_PPFP_P_DataType stEffectiveSTCDelta[BXDM_PictureProvider_DisplayMode_eMax];

   /* SW7405-3085: Channel Synchronization */
   uint32_t uiDisplayInterruptCount; /* A monotonically increasing count of the number of display interrupts that have occurred so far */
   bool bDisplayInterruptCountValid; /* Should be set to true when the app updates the display interrupt count. DM auto-invalidates this field at the end of each vsync */

   /* SW7425-2915: added for issues at startup in NRT mode.
    * See comment in BXDM_PPOUT_P_CalculateVdcData. */
   bool bWaitDueTo2ndSlotTransition;

} BXDM_PictureProvider_P_LocalState;

#define BXDM_PictureProvider_P_MAX_EARLY_BEFORE_DROP 3
#define BXDM_PictureProvider_P_MAX_LATE_BEFORE_DECODER_DROP 3

/* SW7405-4736: Dirty bit definitions for the BXDM_PictureProvider_P_Config struture.
 * The bit will be set when a parameter is changed.  These were added to trigger 
 * debug messages, perhaps we'll find other uses.
 */
#define BXDM_PictureProvider_P_DIRTY_NONE          0x00000000
#define BXDM_PictureProvider_P_DIRTY_ALL           0xFFFFFFFF

#define BXDM_PictureProvider_P_DIRTY_1_STC_VALID                  0x00000001
#define BXDM_PictureProvider_P_DIRTY_1_PTS_OFFSET                 0x00000002
#define BXDM_PictureProvider_P_DIRTY_1_DISPLAY_FIELD_MODE         0x00000004
#define BXDM_PictureProvider_P_DIRTY_1_MONITOR_REFRESH_RATE       0x00000008
#define BXDM_PictureProvider_P_DIRTY_1_3D_SETTINGS                0x00000010
#define BXDM_PictureProvider_P_DIRTY_1_SRC_FORMAT_OVERRIDE        0x00000020
#define BXDM_PictureProvider_P_DIRTY_1_SCAN_MODE_OVERRIDE         0x00000040
#define BXDM_PictureProvider_P_DIRTY_1_1080P_SCAN_MODE            0x00000080
#define BXDM_PictureProvider_P_DIRTY_1_240I_SCAN_MODE             0x00000100
#define BXDM_PictureProvider_P_DIRTY_1_480P_PULLDOWN_MODE         0x00000200
#define BXDM_PictureProvider_P_DIRTY_1_1080P_PULLDOWN_MODE        0x00000400
#define BXDM_PictureProvider_P_DIRTY_1_PICTURE_DROP_MODE          0x00000800
#define BXDM_PictureProvider_P_DIRTY_1_FREEZE                     0x00001000
#define BXDM_PictureProvider_P_DIRTY_1_FRAME_ADVANCE_MODE         0x00002000
#define BXDM_PictureProvider_P_DIRTY_1_REVERSE_FIELDS             0x00004000
#define BXDM_PictureProvider_P_DIRTY_1_AUTO_VALIDATE_ON_PAUSE     0x00008000
#define BXDM_PictureProvider_P_DIRTY_1_PLAYBACK_RATE              0x00010000
#define BXDM_PictureProvider_P_DIRTY_1_SOFTWARE_PCR_OFFSET        0x00020000
#define BXDM_PictureProvider_P_DIRTY_1_USE_HW_PCR_OFFSET          0x00040000
#define BXDM_PictureProvider_P_DIRTY_1_JITTER_TOLERANCE           0x00080000
#define BXDM_PictureProvider_P_DIRTY_1_PLAYBACK                   0x00100000
#define BXDM_PictureProvider_P_DIRTY_1_FRAME_RATE_DETECTION_MODE  0x00200000
#define BXDM_PictureProvider_P_DIRTY_1_DEFAULT_FRAME_RATE         0x00400000
#define BXDM_PictureProvider_P_DIRTY_1_ATSM_MODE                  0x00800000
#define BXDM_PictureProvider_P_DIRTY_1_VTSM_ON_PCR_DISCON         0x01000000
#define BXDM_PictureProvider_P_DIRTY_1_ERROR_HANDLING_MODE        0x02000000
#define BXDM_PictureProvider_P_DIRTY_1_PRE_ROLL_RATE              0x04000000
#define BXDM_PictureProvider_P_DIRTY_1_PROTOCOL                   0x08000000
#define BXDM_PictureProvider_P_DIRTY_1_MAX_DEINT_HEIGHT           0x10000000
#define BXDM_PictureProvider_P_DIRTY_1_HORIZON_OVERSCAN_MODE      0x20000000
#define BXDM_PictureProvider_P_DIRTY_1_TRICK_MODE                 0x40000000
#define BXDM_PictureProvider_P_DIRTY_1_FRAMERATE_OVERRIDE         0x80000000

#define BXDM_PictureProvider_P_DIRTY_2_DISPLAY_MODE               0x00000001
#define BXDM_PictureProvider_P_DIRTY_2_TSM_THRESHOLD              0x00000002
#define BXDM_PictureProvider_P_DIRTY_2_CLIP_TIMING                0x00000004
#define BXDM_PictureProvider_P_DIRTY_2_PRESERVE_SETTINGS          0x00000008
#define BXDM_PictureProvider_P_DIRTY_2_STC_INDEX                  0x00000010
#define BXDM_PictureProvider_P_DIRTY_2_REMOVAL_DELAY              0x00000020
#define BXDM_PictureProvider_P_DIRTY_2_CHANNEL_SYNC_MODE          0x00000040
#define BXDM_PictureProvider_P_DIRTY_2_MUTE                       0x00000080
#define BXDM_PictureProvider_P_DIRTY_2_CHANNEL_CHANGE_SETTINGS    0x00000100
#define BXDM_PictureProvider_P_DIRTY_2_CLOCK_OVERRIDE             0x00000200
#define BXDM_PictureProvider_P_DIRTY_2_IGNORE_NRT_UNDERFLOW       0x00000400
#define BXDM_PictureProvider_P_DIRTY_2_FNRT_SETTINGS              0x00000800

typedef struct BXDM_PictureProvider_P_Config
{
   /* Decoder */
   BXDM_Decoder_Interface stDecoderInterface;
   void *pDecoderPrivateContext;

   /* Display */
   bool bMute;
   BXDM_PictureProvider_DisplayFieldMode eDisplayFieldMode;
   BXDM_PictureProvider_ChannelChangeSettings stChannelChangeSettings;
   BXDM_PictureProvider_SourceFormatOverride eSourceFormatOverride; /* Formerly eSPIM */
   BXDM_PictureProvider_ScanModeOverride eScanModeOverride; /* Formerly eMPIM */
   bool bFreeze;

   BXDM_PictureProvider_MonitorRefreshRate eMonitorRefreshRate;
   BXDM_PictureProvider_1080pScanMode e1080pScanMode;
   BXDM_PictureProvider_PictureDropMode ePictureDropMode;

   BXDM_PictureProvider_PulldownMode e480pPulldownMode; /* Formerly eProgressiveOverrideMode */
   BXDM_PictureProvider_PulldownMode e1080pPulldownMode; /* Formerly eProgressiveOverrideMode */
   BXDM_PictureProvider_240iScanMode e240iScanMode;

   /* Trick Mode */
   BXDM_PictureProvider_FrameAdvanceMode eFrameAdvanceMode;
   uint32_t uiFrameAdvanceByFieldCount; /* This shouldn't be here */
   bool bReverseFields;
   bool bAutoValidateStcOnPause;

   BXDM_Picture_Rate stPlaybackRate;

   BXDM_PictureProvider_PreserveStateSettings stPreserveStateSettings;

   /* TSM */
   BXDM_PictureProvider_DisplayMode eDisplayMode;
   bool bSTCValid;
   uint32_t uiPTSOffset;
   BXDM_PictureProvider_TSMThresholdSettings stTSMThresholdSettings;
   uint32_t uiSTCIndex;

   BXDM_PictureProvider_ClipTimeSettings stClipTimeSettings;

   uint32_t uiSoftwarePCROffset;
   bool bUseHardwarePCROffset;

   bool bJitterToleranceImprovement;
   bool bPlayback;

   BXDM_PictureProvider_FrameRateDetectionMode eFrameRateDetectionMode;
   BAVC_FrameRateCode eDefaultFrameRate;

   bool bAstmMode;
   bool bVirtualTSMOnPCRDiscontinuity;

   BXDM_PictureProvider_ErrorHandlingMode eErrorHandlingMode;

   uint32_t uiRemovalDelay; /* Pictures on the queue are not
                             * evaluated until the removal delay is
                             * 0.  removal delay is decremented on
                             * every vsync when the queue is not
                             * empty */

   BXDM_Picture_Rate stPreRollRate;

   /* Stream Info */
   BAVC_VideoCompressionStd eProtocol;

   /* Debug */
   bool bCRCMode;

   /* SW7405-3085: Channel Synchronization */
   bool bChannelSyncMode;

   /* SW7405-4117: used in conjuction with BXDM_PictureProvider_DisplayFieldMode_eAuto to choose
    * either eSingleField or eBothField based on the steam height during slow motion (and preroll).
    */
   uint32_t uiMaxHeightSupportedByDeinterlacer;

   /* SW7405-4703: Allows bypass of BXDM_PPOUT_S_CalculateHorizontalOverscan in BXDM_PPOUT_S_SetAspectRatio */
   BXDM_PictureProvider_HorizontalOverscanMode eHorizontalOverscanMode;

   /* SWDEPRECATED-1003: used to disable the FIC logic. */
   BXDM_PictureProvider_TrickMode eTrickMode;

   /* SWDEPRECATED-1003: specifed by the middleware/application.  The frame rate
    * will be forced to the value specified in stFrameRateOverride.stRate.
    */
   BXDM_Picture_FrameRateOverride stFrameRateOverride;

   /* SW7405-4736: This is to help debug multi channel issues,
    * i.e. PIP and mosaic mode.  Many of the BXVD_DBG_MSG statements will print this ID
    * to help associate messages with a particular channel.  This can be set to any value,
    * perhaps what makes the most sense is to set it equal to the VDC rectangle number.
    */
   uint32_t uiInstanceID;

   /* SW7422-72: the middleware/application can use this to specify the orientation of a picture. */
   BXDM_PictureProvider_3DSettings st3DSettings;

   /* SW7425-1264: support for a synthesized STC; can create a clock that run backwards. */
   BXDM_PictureProvider_ClockOverride stClockOverride;

   /* SW7425-1264: load the SW STC on this vsync. */
   bool bInitializeSwStc;

   /* SW7425-2270: repeated picture should be encoded. */
   bool bIgnoreNRTUnderflow;

   /* SW7425-3358: support for FNRT. */
   BXDM_PictureProvider_FNRTSettings stFNRTSettings;

   /* SW7405-4736: for debug, flag changes to trigger debug messages. */
   uint32_t uiDirtyBits_1;
   uint32_t uiDirtyBits_2;
   
} BXDM_PictureProvider_P_Config;

typedef struct BXDM_PictureProvider_P_Callback
{
   union {
         BXDM_PictureProvider_Callback_StcPtsOffset_isr fStcPtsOffset;
         BXDM_PictureProvider_Callback_FirstPTSReady_isr fFirstPTSReady;
         BXDM_PictureProvider_Callback_FirstCodedPTSReady_isr fFirstCodedPTSReady;
         BXDM_PictureProvider_Callback_FirstPTSPassed_isr fFirstPTSPassed;
         BXDM_PictureProvider_Callback_PTSError_isr fPTSError;
         BXDM_PictureProvider_Callback_IFrame_isr fIFrame;
         BXDM_PictureProvider_Callback_PictureParameters_isr fPictureParameters;
         BXDM_PictureProvider_Callback_TSMPassInASTMMode_isr fTSMPassInASTMMode;
         BXDM_PictureProvider_Callback_ClipStart_isr fClipStart;
         BXDM_PictureProvider_Callback_ClipStop_isr fClipStop;
         BXDM_PictureProvider_Callback_PictureMarker_isr fPictureMarker;
         BXDM_PictureProvider_Callback_RequestSTC_isr fRequestSTC;
         BXDM_PictureProvider_Callback_PictureUnderEvaluation_isr fPictureUnderEvaluation;
         BXDM_PictureProvider_Callback_TSMResult_isr fTSMResult;
         BXDM_PictureProvider_Callback_PictureExtensionData_isr fPictureExtensionData;
         BXDM_PictureProvider_Callback_DecodeError_isr fDecodeError;
         BXDM_PictureProvider_Callback_ChunkDone_isr fChunkDone;
   } stFunction;

   void *pPrivateContext;
   int32_t iPrivateParam;

   bool bEnable;
} BXDM_PictureProvider_P_Callback;


typedef struct BXDM_PictureProvider_P_Element_Params
{
      /* Each element has two sets of PTS values, the actual and
       * virtual */
      BXDM_PPFP_P_DataType stPTS[BXDM_PictureProvider_P_PTSIndex_eMax];

      /* Each element has a single polarity associated with it based
       * on the pulldown information */
      BAVC_Polarity ePolarity;
}  BXDM_PictureProvider_P_Element_Params;

typedef struct BXDM_PictureProvider_P_PCROffset
{
      BXDM_Picture_PCROffset stPCROffsetCopy;
      bool bValidOverloaded;           /* will be forced TRUE if the "bUseHwPcrOffset == false" */
      bool bDiscontinuityOverloaded;   /* will be forced FALSE if the "bUseHwPcrOffset == false" */
} BXDM_PictureProvider_P_PCROffset;

/* Currently informational only, to help with debug.
 */
typedef enum BXDM_PictureProvider_P_FrameRateType
{
   BXDM_PictureProvider_P_FrameRateType_eCoded=0,     /* coded in the stream */
   BXDM_PictureProvider_P_FrameRateType_eDefault,     /* as specified by BXDM_PictureProvider_SetDefaultFrameRate_isr   */
   BXDM_PictureProvider_P_FrameRateType_eOverride,    /* as specified by BXDM_PictureProvider_SetFrameRateOverride_isr  */
   BXDM_PictureProvider_P_FrameRateType_eCalculated,  /* calculated in the FRD code using the PTS values                */ 
   BXDM_PictureProvider_P_FrameRateType_eForced,      /* using the values hardcoded in BXDM_PPTSM_P_PtsCalculateParameters */

   BXDM_PictureProvider_P_FrameRateType_eMax
} BXDM_PictureProvider_P_FrameRateType;



typedef struct BXDM_PictureProvider_P_TSM_Static_Picture_Parms
{
   /* Element information */
   uint32_t uiNumElements; /* The number of elements in this picture determined from pulldown information */
   BXDM_PictureProvider_P_Element_Params astElmts[BXDM_PictureProvider_P_MAX_ELEMENTS_PER_PPB]; /* The parameters specific to each element */

   BAVC_FrameRateCode eFrameRatePreFRD; /* source frame rate for this PPB before FRD */
   BAVC_FrameRateCode eFrameRate;   /* source frame rate for this PPB w/ FRD (reported value) */
   BAVC_FrameRateCode eFrameRateXVD;   /* source frame rate for this PPB that XVD uses internally for PTS interpolation */

   BXDM_PictureProvider_P_FrameRateType eFrameRateType;  /* currently only to help with debug. */

   BXDM_PictureProvider_P_PCROffset stPCROffsetXDM;
   BXDM_PictureProvider_PTSType ePtsType;             /* is this PTS valid, coded, from the host or interpolated? */
   BXDM_PPFP_P_DataType stPTSDelta;   /* The delta PTS between between each field of this picture */
   uint32_t uiDroppedCount;           /* a copy of the dropped count from the last PPB that had a CODED PTS */
   uint32_t uiVeryEarlyThreshold;      /* i.e. way too early */
   uint32_t uiVeryLateThreshold;       /* i.e. way too late */
   uint32_t uiPtsVsyncDiffThreshold;
   /* best guess (by interpolation) of the PTS of the next PPB.  We
   * have both actual and virtual interpolated values */
   BXDM_PPFP_P_DataType stPTSOfNextPPB[BXDM_PictureProvider_P_PTSIndex_eMax];
   int32_t iPTSJitterCorrection;

   /* SW7405-5549: save a copy of the pulldown information. */
   BXDM_Picture_PullDown ePullDownRangeChecked; 

} BXDM_PictureProvider_P_TSM_Static_Picture_Parms;

typedef struct BXDM_PictureProvider_P_TSM_Dynamic_Picture_Parms
{
   BXDM_PictureProvider_DisplayMode eSelectionMode; /* Determines is TSM or vTSM should be used for picture selection */

   BXDM_PictureProvider_TSMResult eTsmResult;          /* The TSM Result */
   uint32_t uiTSMSelectedElement;      /* The element that TSM has selected */
   uint32_t uiEffectivePTS[BXDM_PictureProvider_P_PTSIndex_eMax]; /* The effective PTS of the picture (taking all offsets into account) */
   int32_t iStcPtsDifferenceActual;    /* The actual STC/PTS difference reported to application */
   int32_t iStcPtsDifferenceEvaluated; /* The STC/PTS difference used for TSM evaluation (including FIC offset) */
   int32_t iStcPtsDifferenceDisplayed; /* The STC/PTS difference when last displayed (including FIC offset) */
   int32_t iStcPtsDifferenceEvaluatedDeltaAbs; /* The delta of the STC/PTS evaluated differnece */

   BXDM_PictureProvider_PictureHandlingMode ePictureHandlingMode;
   bool bClipTransition;
   bool bEvaluatedWithSwStc;  /* SW7425-1264: SW STC was used to evaluate this picture. */

   /* SW7425-2536: added to support "drop at decode" for non-AVD decoders. 
    * The STC used to evalute the picture, includes the STC snapshot, 
    * jitter correction and PCR offsets when appropriate.
    */
   uint32_t uiEffectiveStc[BXDM_PictureProvider_P_PTSIndex_eMax];
   
} BXDM_PictureProvider_P_TSM_Dynamic_Picture_Parms;

typedef struct BXDM_PictureProvider_P_TSM_Picture_Parms
{
   BXDM_PictureProvider_P_TSM_Static_Picture_Parms stStatic;
   BXDM_PictureProvider_P_TSM_Dynamic_Picture_Parms stDynamic;
} BXDM_PictureProvider_P_TSM_Picture_Parms;

typedef struct BXDM_PictureProvider_P_Display_Static_Picture_Parms
{
   bool b32FilmSource;                /* true if a 3:2 interlaced source is detected (e.g. pulldown is 3:2 or framerate=24) */

   BXDM_PictureProvider_P_ScanMode eScanMode;

   /* SW7425-764: the monitor refresh rate, passed to VDC in the MFD structure.  */
   BFMT_Vert eBFMTRefreshRate;

   /* SW7405-5549: save a copy of the monitor refresh rate. */
   BXDM_PictureProvider_MonitorRefreshRate eMonitorRefreshRate;


} BXDM_PictureProvider_P_Display_Static_Picture_Parms;

typedef struct BXDM_PictureProvider_P_Display_Dynamic_Picture_Parms
{
   uint32_t uiSelectedElement; /* The element that is ultimately selected after FIC */
   BAVC_Polarity eSourcePolarity; /* The source polarity that is ultimately displayed */

   bool bPPBRepeated;      /* True if any element within this PPB was displayed in the prior vsync  */
   bool bPPBEncoded;       /* True if any element within this PPB was encoded in the prior vsync. Used to aid in differentiating FIC vs underflow stall. */
   bool bElementRepeated;  /* True if the same element within this PPB was was displayed in the prior vsync */
   BXDM_PPFP_P_DataType stDisplayDuration; /* # of clock ticks this picture has been displayed */
   bool bHolding;   /* Indicates if the picture is being held during a channel change */

   BXDM_PictureProvider_DisplayFieldMode eDisplayFieldMode; /* What fields should be displayed, Top only, Bottom only or both */
   BXDM_PictureProvider_SourceFormatOverride eSourceFormatOverride; /* Formerly eSPIM */

   bool bFICEnabled; /* True if FIC correction attempted for this picture */
   bool bIgnoreCadenceMatch; /* PR49634: Indicates if VDC's cadence
                              * matching should be enabled for the
                              * sync-slipped (multi-buffer) path */

   BXDM_PictureProvider_P_RepeatMode eRateConversionRepeatMode;
   BXDM_PictureProvider_P_RepeatMode eTrickPlayRepeatMode;

   /* SWBLURAY-21565: only allow the last picture of a clip to generate one stop event */
   bool  bGeneratedClipStop;

   /* SW7405-4560: only deliver the SEI messages and other extension data once
    * per picture, i.e. don't deliver it again if the picture is repeated.
    */
   bool  bExtensionDataDelivered;

   /* SW7425-104: Set the MFD bRepeatField flag based on how the picture was previously 
    * delivered to VDC.  If an element is being repeated, set the flag.  The following
    * flags track which elements were delivered.
    */
   bool bTopFieldDelivered;
   bool bBottomFieldDelivered;
   bool bFrameDelivered;

   /* SW7635-51: snapshot the STC offset (uiSoftwarePCROffset) when a 
    * picture is selected for display.  The valid bit will true once
    * the picture has been promoted for display and false during the
    * initial evaluation.
    */
   bool     bSwPcrOffsetValid;
   uint32_t uiSoftwarePCROffset;

   /* SW7425-2255: snapshot the display offset (uiPTSOffset) when a 
    * picture is selected for display.  The valid bit will be false 
    * during the initial evaluation and true once the picture has been 
    * promoted for display.
    */
   bool     bPtsOffsetValid;
   uint32_t uiPtsOffset;

   /* SW7425-1001: the following two flags are used to determine when to set
    * "bLast" in the MFD picture structure, effectively an EOS flag.
    *
    * "bLastPictureSeen" is conditionally set when "bLastPicture" is set in a PPB.
    * "bSetLastPictureFlag" is set after the last field of the last picture has been
    * delivered to VDC.  It indicates that "MFD.bLast" should be set.
    *
    * The two flags may be overkill, but they ensure that once "MFD.bLast" is set,
    * it stays set.
    */
   bool bLastPictureSeen;
   bool bSetLastPictureFlag;

   /* SW7405-5683: Delivering interlaced content to a display running at <= 30 Hz is not supported by the system. 
    * The most common case would be 1080i60 to a 30p display. To get the best possible results, the FIC is disabled 
    * and the display mode is forced to eSingleField for interlaced content when the monitor rate is <= 30 Hz.
    */
   bool bForceSingleFieldMode;

} BXDM_PictureProvider_P_Display_Dynamic_Picture_Parms;

typedef struct BXDM_PictureProvider_P_Display_Picture_Parms
{
   BXDM_PictureProvider_P_Display_Static_Picture_Parms stStatic;
   BXDM_PictureProvider_P_Display_Dynamic_Picture_Parms stDynamic;
} BXDM_PictureProvider_P_Display_Picture_Parms;

typedef struct BXDM_PictureProvider_P_Picture_Params
{
   /* Misc Information */
   uint32_t uiPPBIndex;

   /* SW7405-4736: when QM debug enabled, dump the picture state if an eWait
    * result for roughly a second.
    */
   uint32_t uiNumTimesWait;

   /* Dynamic State Information */
   uint32_t uiStateBits;
   BXDM_PictureProvider_P_TSM_Picture_Parms stTSM;
   BXDM_PictureProvider_P_Display_Picture_Parms stDisplay;
} BXDM_PictureProvider_P_Picture_Params;

typedef struct BXDM_PictureProvider_P_Picture_Context
{
      bool                       bValidated; /* Indicates if the context is valid. */

      /* The following values are constant for each PPB */
      const BXDM_Picture *pstUnifiedPicture;

      BXDM_PictureProvider_P_Picture_Params   stPicParms;
      BAVC_MFD_Picture           stMFDPicture;
} BXDM_PictureProvider_P_Picture_Context;

typedef struct BXDM_PictureProvider_P_ClipEvent
{
      bool bOccurred;
      uint32_t uiTime;
      bool bDelayedCallback;
} BXDM_PictureProvider_P_ClipEvent;

typedef struct BXDM_PictureProvider_P_ClipTimeV2
{
      BXDM_PictureProvider_ClipTimeSettings stTime;

      BXDM_PictureProvider_P_ClipEvent stStartEvent;
      BXDM_PictureProvider_P_ClipEvent stRequestEvent;
      BXDM_PictureProvider_P_ClipEvent stDisplayEvent;
      BXDM_PictureProvider_P_ClipEvent stStopEvent;
      BXDM_PictureProvider_P_ClipEvent stMarkerEvent;
} BXDM_PictureProvider_P_ClipTimeV2;

typedef struct BXDM_PictureProvider_P_ClipInfo
{
      BXDM_PictureProvider_P_ClipTimeV2 stCurrentClip;

      bool bClipActive;    /* SWBLURAY-22390: true when new clip parameters have been loaded and
                            * the clip stop time has not been reached. */
      uint32_t uiLastMarker;     /* the last marker seen */

      bool bClipTransition;      /* true if we're in the middle of a clip transition */
      bool bForcePTSError;  /* true if we want to force sending a PTS
                             * error on the next VSYNC pass*/
      bool bTrickModeTransition; /* true if we're in the middle of a trick mode transition */
} BXDM_PictureProvider_P_ClipInfo;

typedef enum BXDM_PictureProvider_P_DecodeState
{
      BXDM_PictureProvider_P_DecodeState_eStarted,
      BXDM_PictureProvider_P_DecodeState_eStopped,

      BXDM_PictureProvider_P_DecodeState_eMax
} BXDM_PictureProvider_P_DecodeState;

/* DM's persistent state between BXVD_OpenChannel and
 * BXVD_CloseChannel() */
typedef struct BXDM_PictureProvider_P_State_Channel
{
      BXDM_PictureProvider_P_Picture_Context stSelectedPicture; /* The currently selected picture */
      BXDM_PictureProvider_P_Picture_Context stPreviousPicture; /* The previously selected picture (PPB may be invalid!) */
      BXDM_PictureProvider_P_Picture_Context stPictureUnderEvaluation; /* The currently evaluated picture */
      BXDM_PictureProvider_P_Picture_Context stPreviousPictureUnderEvaluation; /* The previously evaluated picture */

      /* ePrevSourcePolarity helps us figure out the eSourcePolarity
       * when MPIM is set to interlaced on progressive sources on a
       * progressive display */
      BAVC_Polarity ePrevSourcePolarity;
      bool bPrevMute;

      bool bDMVersionDisplayed;

      BAVC_MFD_Picture stDefaultPicParams;

   BXDM_PictureProvider_P_ClipInfo stClipInfo;
   uint32_t uiPPBSerialNumber;

   BXDM_PPJRC_P_Handle hJrc;
   BXDM_PPAVG_P_Handle hDeltaSTCAvg;   /* used for rate matching for JTI */

   BXDM_PictureProvider_ChannelChangeSettings stSavedChannelChangeSettings;
   bool bSavedChannelChangeSettingsValid;

   /* SW7425-2656: essentially flags that BXVD_DisableForFlush, BXVD_SetSkipPictureModeDecode 
    * or BXVD_PVR_SetHostSparseMode has been called.
    */
   bool bPostFlushDecode;

   BXDM_PictureProvider_P_DecodeState eDecodeState;

   /* Display Interrupt Handler that this Picture Provider is associated with,
    * i.e. the sink for pictures from this PP.
    */
   BXDM_DisplayInterruptHandler_Handle hXdmDih;

   /* SW7425-1264: the internal interpolated software STC. */
   uint32_t uiSwStc;

} BXDM_PictureProvider_P_State_Channel;

#include "bxdm_pp_vtsm.h"
#include "bxdm_pp_dbg.h"
#include "bxdm_pp_frd.h"

typedef enum BXDM_PictureProvider_P_STCStallType
{
   BXDM_PictureProvider_P_STCStallType_eUnderflow,
   BXDM_PictureProvider_P_STCStallType_eFIC,

   BXDM_PictureProvider_P_STCStallType_eMax
} BXDM_PictureProvider_P_STCStallType;

typedef struct BXDM_PictureProvider_P_State_NonRealTime
{
   uint32_t uiLastStc;
   bool bLastStcValid;
   bool bLastStallStc;
   bool bLastStallStcValid;
   BXDM_PictureProvider_P_STCStallType eSTCStallType; /* If bLastStallStc = true, this indicates WHY the STC was stalled */
   BAVC_Polarity ePolarityWhenDeltaStcWasNonZero;
   bool bPolarityFieldValid;                          /* SW7425-2424: force "ePolarityWhenDeltaStcWasNonZero" to be set */

   /* SW7425-2270: if specified by the application, ignore picture underflows, i.e. 
    * don't stall the STC or set the MFD ignore flag.
    */
   bool bIgnoringUnderflow;

   /* SW7425-2915: added for issues at startup in NRT mode.
    * See comment in BXDM_PPOUT_P_CalculateVdcData. */
   bool blastIgnorePicture;

   /* SW7425-3358: support for FNRT. */
   uint32_t uiFNRTPreChargeCnt;
   uint32_t uiFNRTPostChargeCnt;
   uint32_t uiChunkId;
   bool     bNextChunkReceived;
   bool     bChunkDone;

} BXDM_PictureProvider_P_State_NonRealTime;

/* DM's persistent state between BXVD_StartDecode and
 * BXVD_StopDecode */
typedef struct BXDM_PictureProvider_P_State_Decode
{
   BXDM_PictureProvider_PTSInfo stErrorPts;               /* the most recent PTS info that generated an error */

   BXDM_PPVTSM_P_State stVTSM;             /* vTSM State */

   /* Field Inversion Correction (FIC) */
   BXDM_PPFP_P_DataType stFieldInversionCorrectionPTSOffset;

   uint32_t    uiStcPtsDeltaSentToApp;    /* last STC PTS delta shipped to the application */
   int32_t     iPreviousStcPtsDifferenceEvaluated;  /* The previous element's StcPtsDifferenceEvaluated  */

   BXDM_PPDBG_P_Info stDebug;    /* Debug Log Information */

   /* Sticky, indicates that a PPB has been promoted to displays status.
    * (it may or may not have passed the TSM test)
    */
   bool  bFirstPPBHasBeenDisplayed;

   /* Sticky, indicates that the first PPB has been seen after a
    * start decode */
   bool  bFirstPPBSeen;

   /* Only set for the very first PPB
    * TODO: still needed?
    */
   bool  bThisIsTheFirstPPB;

   /* Channel Change - First Picture Preview */
   bool bForceDisplayFirstPPB;

   uint32_t uiPPBConsecutiveLateCount; /* A count of how many consecutive *unique* PPBs were late */
   uint32_t uiPPBConsecutiveTooEarlyCount; /* A count of how many PPBs were too early */
   bool bTooLateGimmeDone; /* True if previous PPB's TSM result was late */

   uint32_t uiPendingDropCount;  /* a copy of the pending dropped count */

   bool bPreviousVsyncWasTrickMode; /* Indicates if the previous
                                     * vsync was in trick mode */
   bool bAutoValidateStc; /* Indicates if DM should auto-validate
                           * the STC on a TSM pass */

   uint32_t uiRemovalDelay; /* Pictures on the queue are not
                             * evaluated until the removal delay is
                             * 0.  removal delay is decremented on
                             * every vsync when the queue is not
                             * empty */

   bool bPreRolling;    /* Indicates that pictures are being displayed in the "preroll" mode   */

   uint32_t uiPreRollPlaybackRate;  /* Rate as a precentage of normal playback to use during the preroll
                                     * phase. BXVD_P_NORMAL_PLAYBACK_RATE is normal, i.e. 100% */


   bool bDropUntilNextRAP; /* A decode error has been detected, drop pictures until the next RAP */

   uint32_t uiDecodeErrorDropCount; /* number of pictures dropped due to decode errors. */

   bool bDqUnderflowPrevVsync;      /* The delivery queue underflowed on the previous vsync */

   BXDM_PictureProvider_PTSInfo stLatestCodedPts;   /* the most recent coded PTS seen on the delivery queue.
                                     * Note: this picture may not be displayed. */

   bool         bFirstCodedPtsReceived;   /* the first coded PTS has been received. */
   BXDM_PictureProvider_PTSInfo stFirstCodedPts;          /* the first coded PTS received after BXDM_PP_S_ResetCommon was called. */

   /* The following are used to print BDBG_WRN messages only once
    * per change of parameter */
   BAVC_FrameRateCode eLastUnsupportedFrameRate;
   BXDM_PictureProvider_MonitorRefreshRate eLastIncorrectMonitorRefreshRate;
   uint32_t ui32IncorrectMonitorRefreshRateCount;
   BXDM_Picture_PullDown eLastUnsupportedPullDown;
   /* SW7335-781: Keep track of selection mode override so warning only output on transitions */
   BXDM_PictureProvider_DisplayMode eLastSelectionModeOverride;

   BXDM_PictureProvider_P_STCTrickMode eLastCalculatedSTCTrickMode;
   BXDM_PictureProvider_P_STCTrickMode eLastReportedSTCTrickMode;
   uint32_t uiLastStcSnapshot;
   bool     bLastStcValid;
   bool     bStcStalled;      /* The STC snapshot on this vsync is equal to uiLastStcSnapshot */

   BXDM_Picture_PullDown ePPBPulldownHistory[BXDM_PictureProvider_P_MAX_PPB_PULLDOWN_HISTORY];
   uint32_t uiPPBPulldownHistoryCount;
   uint32_t uiPPBPulldownHistoryIndex;

   BXDM_PPFRD_P_Stats stFRDStats;

   BAVC_Polarity ePreviousVsyncPolarity;
   bool bPreviousVsyncPolarityValid;

   /* In live mode, indicates that a valid PCR offset has been received.
    * TSM evaluation is held off until this variable is true.
    */
   bool bValidPcrOffsetReceived;

   /* For measuring elapse times. */
   BXDM_PPTIMER_P_Data  stTimerData;

   int32_t iStcJitterCorrectionOffset;

   /* Rate Match Detection */
   int32_t iStcPtsDifferenceEvaluatedOfLastPPB;
   bool bStcPtsDifferenceEvaluatedOfLastPPBValid;
   bool bRateMatchedLastReported;

   /* SW7405-3774/SW7550-379: This is a fix for ensuring that the BXDM_PictureProvider_Callback_eStcPtsOffset
    * is always executed after BXVD_StartDecode is called.  There was a corner case with first picture preview
    * and freezing the first picture.  When the picture finally passed TSM, it was being repeated; hence the
    * callback was never generated. This flag essentially overrides the element repeated flag.
    */
   bool bExecutedInitialPtsStcOffsetCB;

   /* The count of PPB's received since StartDecode was called. */
   uint32_t uiPPBIndex;

   /* SW7405-4736: used to print QM debug messages once per second
    * Wrap this element and the preceding in a debug structure?
    */
   uint32_t uiVsyncCountQM;

   BXDM_PictureProvider_P_State_NonRealTime stNonRealTime;

   /* to filter out messages in BXDM_PPOUT_S_3DFixup */
   bool bMissingSecondFrameMessageDelivered;

} BXDM_PictureProvider_P_State_Decode;

/* DM's persistent state between vsyncs */
typedef struct BXDM_PictureProvider_P_State
{
      BXDM_PictureProvider_P_State_Channel stChannel;
      BXDM_PictureProvider_P_State_Decode stDecode;
} BXDM_PictureProvider_P_State;

/* SW7405-4378: a warpper structure primarily for documentation. */

typedef struct BXDM_PictureProvider_P_PictureParameterInfo
{
   bool              bUseCachedPictureParameters;
   BXDM_Picture      stUnifiedPicture;
   BAVC_MFD_Picture  stMFDPicture;

} BXDM_PictureProvider_P_PictureParameterInfo;


typedef struct BXDM_PictureProvider_P_Status
{
   BXDM_PictureProvider_PTSInfo stCurrentPTS;
   BXDM_PictureProvider_Callback_PictureParameterInfo stPictureParameterInfo;

   /* SW7405-4378: at start decode, cache the picture parameters locally
    * until a picture passes the TSM test and is sent to VDC.
    * After that, the ePictureParameters callback will report the picture
    * parameters for the displayed picture.
    */
   BXDM_PictureProvider_P_PictureParameterInfo stCachedPictureParameters;

   BXDM_PictureProvider_PTSInfo stCodedPTS;
   BXDM_PictureProvider_PTSInfo stNextPTS;
   bool bNextPTSInfoValid;

   bool bIPictureFound;
   uint32_t uiPictureTag;
   BXDM_Picture_GopTimeCode stGOPTimeCode;    /* GOP time code of the currently displayed picture */

   BXDM_PictureProvider_Counters stCounters;

   BXDM_Picture stCurrentPPBParameterInfo;
   BXDM_Picture stCurrentLinkedPPBParameterInfo;   /* If present, the dependent picture linked to stCurrentPPBParameterInfo */

   bool bCurrentPPBParameterInfoValid;
} BXDM_PictureProvider_P_Status;

#define BXDM_PictureProvider_P_MFDPicture_Max 2

typedef struct BXDM_PictureProvider_P_Context
{
      BXDM_PictureProvider_P_State stDMState; /* Persistent DM State */
      BXDM_PictureProvider_P_Config stDMConfig; /* DM Configuration */
      BXDM_PictureProvider_P_Status stDMStatus; /* DM Status */

      BXDM_PictureProvider_P_Callback stCallbacks[BXDM_PictureProvider_Callback_eMax];

      BAVC_MFD_Picture *astMFDPicture;
      uint32_t uiMFDPictureCount;

      BTMR_TimerHandle hTimer;
} BXDM_PictureProvider_P_Context;

#endif /* BXDM_PP_PRIV_H_ */
