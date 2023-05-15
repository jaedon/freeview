/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_decoder.h $
 * $brcm_Revision: Hydra_Software_Devel/12 $
 * $brcm_Date: 9/6/12 8:55a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/src/bxvd_decoder.h $
 * 
 * Hydra_Software_Devel/12   9/6/12 8:55a btosi
 * FWAVD-289: added logic to ensure that the PPBReceived callback is only
 * executed once per PPB
 * 
 * Hydra_Software_Devel/11   8/16/12 2:11p btosi
 * SW7405-4736: (Enhance XDM debug capabilities) added BXVD_QDBG message
 * to help detect issues with the delivery queue write offset
 * 
 * Hydra_Software_Devel/10   7/27/12 11:30a btosi
 * SW7425-1064: added support for BXVD_Decoder_OpenChannel
 * 
 * Hydra_Software_Devel/9   7/2/12 9:56a btosi
 * SW7425-3358: first cut at the FNRT changes
 * 
 * Hydra_Software_Devel/8   5/8/12 12:02p btosi
 * SW7125-1285: read back PPB and offset to ensure that the values are
 * written to memory
 * 
 * Hydra_Software_Devel/7   9/20/11 3:33p btosi
 * SW7425-1261: only clear bTruncatingGop in ..._ReleaseGopTail
 * 
 * Hydra_Software_Devel/6   4/4/11 11:28a btosi
 * SW7405-4736: added support for BXVD_DecoderDbg_P_PrintUnifiedPicture
 * 
 * Hydra_Software_Devel/5   10/29/10 11:31a btosi
 * SW7422-27: adding support for 3D
 * 
 * Hydra_Software_Devel/4   6/16/10 12:56p btosi
 * SW7405-4365: added support for using the new timer functions
 * 
 * Hydra_Software_Devel/3   6/7/10 10:24a delkert
 * SW7405-3925: Modify RequestPictureDrop_isr API to allow update of
 * requested count with actual count requested from decoder
 *
 * Hydra_Software_Devel/2   6/3/10 11:55a btosi
 * SW7630-83: removed unused elements
 *
 * Hydra_Software_Devel/1   2/25/10 4:24p nilesh
 * SW7405-2993: XDM Merge
 *
 ***************************************************************************/

#ifndef BXVD_DECODER_H_
#define BXVD_DECODER_H_

#include "bxvd.h"
#include "bxvd_decoder_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * type definitions
 */

#define BXVD_DECODER_DQT_DEADLOCK_THRESHOLD     4

/* The maximum number of fields that the decoder will request AVD to drop.
 */
#define BXVD_DECODER_MAX_FIELDS_TO_DROP   16


/* Support for processing sets of picture.  Stereo/3D is an
 * example of when multiple pictures would be processed as a set.
 */
typedef enum BXVD_Decoder_P_PictureSetType
{
   BXVD_Decoder_P_PictureSet_eSingle = 0,
   BXVD_Decoder_P_PictureSet_eBase,
   BXVD_Decoder_P_PictureSet_eDependent,

   BXVD_Decoder_P_PictureSet_eMax

} BXVD_Decoder_P_PictureSetType;


#define BXVD_P_DECODER_PICTURE_QUEUE_DEPTH 64

typedef struct BXVD_Decoder_P_PictureContext
{
   /* Support for processing sets of pictures. Currently sets are only of
    * size "2", hence "uiSetCount" is not used.  It will be needed to
    * support larger multiview sets.
    */
   BXVD_Decoder_P_PictureSetType    eSetType;
   uint32_t                         uiSetCount;

   /* Drop if not a complete set of pictures (not TSM related) */
   bool                       bDropPicture;

   /* SW7422-72: for 3D support dereferenced some of the PPB data
    * prior to calling BXVD_Decoder_S_UnifiedQ_ValidatePicture
    * TODO: redo logic only parse this information once.
    */
   BXDM_Picture_3D st3D;

   /* TODO: does this need to be a display element or
    * could it just be a BXVD_P_PPB *?
    */
   BXVD_P_DisplayElement stPPB;

   /* If a dependent picture, will point to the associated base picture. */
   struct BXVD_Decoder_P_PictureContext * pBasePicture;

   /* Points to the next dependent picture in the list.  NULL if the last one. */
   struct BXVD_Decoder_P_PictureContext * pDependentPicture;

} BXVD_Decoder_P_PictureContext;

typedef struct BXVD_Decoder_P_PictureContextQueue
{
   uint32_t uiWriteOffset;

   BXVD_Decoder_P_PictureContext    astPictureContext[ BXVD_P_MAX_ELEMENTS_IN_DISPLAY_QUEUE ];

} BXVD_Decoder_P_PictureContextQueue;

/*
 * Unified picture queue types.
 */

typedef struct BXVD_Decoder_P_UnifiedPictureContext
{
   bool  bInUse;

   BXVD_P_PPB * pPPBPhysical;
   BXVD_P_PPB * pPPB;

   BXVD_Decoder_P_PictureSetType    eSetType;
   uint32_t                         uiSetCount;
   uint32_t                         uiDropCount;   /* the AVD drop count at the time this picture was received. */

   /* for debug */
   uint32_t uiIndex;

   BXDM_Picture stUnifiedPicture;

   /* For implementing the Unified Queue as a linked list. */
   struct BXVD_Decoder_P_UnifiedPictureContext * pstPreviousUniContext;
   struct BXVD_Decoder_P_UnifiedPictureContext * pstNextUniContext;

   /* For managing sets of pictures; e.g. MVC. */
   struct BXVD_Decoder_P_UnifiedPictureContext * pstDependentUniContext;

} BXVD_Decoder_P_UnifiedPictureContext;


typedef struct BXVD_Decoder_P_UnifiedPictureQueue
{
   uint32_t uiReadOffset; /* Always the picture under evaluation */
   uint32_t uiWriteOffset; /* if uiWriteOffset == uiReadOffset, queue is empty */

   uint32_t uiSearchIndex;

   int32_t  iNumPictures;

   int32_t  iNumUsedElements;

   BXVD_Decoder_P_UnifiedPictureContext * pstContextTail;
   BXVD_Decoder_P_UnifiedPictureContext * pstContextHead;

   BXVD_Decoder_P_UnifiedPictureContext astUnifiedContext[ BXVD_P_DECODER_PICTURE_QUEUE_DEPTH ];

} BXVD_Decoder_P_UnifiedPictureQueue;


typedef struct BXVD_Decoder_P_DQTContext
{
   /*
    * Parameters to support "Display Queue Trick Mode"
    */
   bool    bFoundStartOfGop;   /* indicates that the beginning of a GOP has been detected */
   uint32_t    uiSearchRdOffset;   /* for looking ahead to find the end of the GOP */

   bool  bDqtEnabled;       /* enables the reverse playback of GOPs */

   bool  bConvertGopToUniPics;   /* indicates in the middle of playing a GOP backwards */

   uint32_t  uiReverseReadOffset;    /* read pointer when walking backwards through the display queue */

   uint32_t  uiEndOfGopOffset;   /* end of the current GOP, used to updated the display queue read pointer */

   /*
    * Variables for dectecting a deadlock condition and dealing with the subsequent clean up.
    */
   uint32_t  uiPreviousWrOffset;                 /* delivery queue write pointer from the previous vsync */
   uint32_t  uiTimesOffsetRepeated;        /* how many vsync's the write pointer has remained the same */
   uint32_t    uiDeadlockThreshold;        /* number of vsync's to wait for a new picture in the delivery queue */

   bool    bTruncatingGop;           /* truncating the current GOP  */
   uint32_t    uiCleanupOffset;    /* delivery queue offset for releasing the pictures at the end of a GOP */

   bool        bValidPicTag;               /* indicates if "uiCurrentPicTag" is valid */
   uint32_t    uiCurrentPicTag;        /* picture tag of the current GOP */

} BXVD_Decoder_P_DQTContext;

typedef struct BXVD_Decoder_P_DNR
{
 unsigned int saved_mb_cnt;
 unsigned int adj_Qp_ref;
 unsigned int adj_Qp_pre;
} BXVD_Decoder_P_DNR;


/* A bit of overkill for one counter, perhaps more
 * will be added later.
 */
typedef struct BXVD_Decoder_Counters
{
   uint32_t uiDecoderInputOverflow;  /* Number of times the decoder input has gotten into an overflow state. */
   uint32_t uiVsyncCount;           /* The number times BXVD_Decoder_DisplayInterruptEvent_isr has 
                                     * been called.  Conditionally reset in BXVD_Decoder_StartDecode_isr.
                                     */
}  BXVD_Decoder_Counters;

typedef struct BXVD_Decoder_P_LogData
{

   /* Used for debug logging. */
   uint32_t    uiVsyncsPerSecond;
   uint32_t    uiCallbackCount;
   uint32_t    uiPicturesFromAvd;      /* count of pictures received from AVD. */
   uint32_t    uiPicturesToAvd;        /* count of pictures returned to AVD. */
   uint32_t    uiPicturesToPP;         /* count of pictures sent to PP */
   uint32_t    uiPicturesFromPP;         /* count of pictures returned from PP */

   /* For timing callbacks and functions calls. */
   BXVD_DecoderTimer_P_Data   stTimerData;

   /* SW7125-1285: read these values back to ensure that they have been written to memory.
    * - physical address of the last PPB released
    * - release queue shadow index
    */
   BXVD_P_PPB * pLastPPBReleased;
   uint32_t     uiShadowIndexAfterLastRelease;

} BXVD_Decoder_P_LogData;

typedef struct BXVD_P_Decoder_Context
{
   bool  bDecoderHasBeenInitialized;   /* set in BXVD_Decoder_StartDecode_isr */

   BXVD_Decoder_P_PictureContextQueue  stPictureContextQueue;

   BXVD_Decoder_P_UnifiedPictureQueue  stUnifiedPictureQueue;

   BXVD_Decoder_P_DQTContext  stDqtCntxt;

   BXVD_Decoder_P_DNR         stDNRInfo;

   BXVD_Decoder_Counters      stCounters;

   uint32_t                   uiDropCountSnapshot;

   bool                       bHostSparseMode; /* Host Sparse Mode */
   bool                       bReversePlayback; /* DQT */
   BXVD_MPEGPulldownOverride  ePulldownOverride;
   bool                       bCRCMode;

   /* SW7425-3358: essentially pPPB->timing_marker snapped
    * to the start of a GOP.
    */
   uint32_t                   uiCurrentChunkId;

   /* For debug. */
   BXVD_Decoder_P_LogData   stLogData;

   /* To help debug queue issues. */
   uint32_t    uiPreviousDeliveryQWriteOffset;

   /* FWAVD-289: Use the queue index to detect when a picture is being evaluated for the first time. */
   uint32_t uiPreviousWriteOffset;

#if BXVD_DM_ENABLE_YUV_GRAB_MODE
   /* variables assocated with the "grab" debug mode */
   bool  bGrabRvc;
   bool  bGrabPpb;
   bool  bGrabYuv;
#endif

} BXVD_P_Decoder_Context;

typedef struct BXVD_Decoder_P_LocalState
{
   uint32_t uiDeliveryQueueWriteOffset;   /* snapshot the offset to avoid race conditions. */

} BXVD_Decoder_P_LocalState;


/*
 * Public functions.
 */
BERR_Code
BXVD_Decoder_GetPictureCount_isr(
         BXVD_ChannelHandle hXvdCh,
         uint32_t *puiPictureCount
         );

BERR_Code
BXDM_PP_PP_Decoder_PeekAtPicture_isr(
         BXVD_ChannelHandle hXvdCh,
         uint32_t uiIndex,
         BXDM_Picture **pUnifiedPicture
         );

BERR_Code
BXVD_Decoder_GetNextPicture_isr(
         BXVD_ChannelHandle hXvdCh,
         BXDM_Picture **pUnifiedPicture
         );

BERR_Code
BXVD_Decoder_ReleasePicture_isr(
         BXVD_ChannelHandle hXvdCh,
         BXDM_Picture *pUnifiedPicture,
         const BXDM_Decoder_ReleasePictureInfo *pReleasePictureInfo
         );

BERR_Code
BXVD_Decoder_GetPictureDropPendingCount_isr(
         BXVD_ChannelHandle hXvdCh,
         uint32_t *puiPictureDropPendingCount
         );

BERR_Code
BXVD_Decoder_RequestPictureDrop_isr(
         BXVD_ChannelHandle hXvdCh,
         uint32_t *puiPictureDropRequestCount
         );

BERR_Code
BXVD_Decoder_DisplayInterruptEvent_isr(
         BXVD_ChannelHandle hXvdCh
         );

BERR_Code 
BXVD_Decoder_OpenChannel(
      BXVD_ChannelHandle hXvdCh
      );

BERR_Code
BXVD_Decoder_GetDMInterface(
         BXVD_ChannelHandle hXvdCh,
         BXDM_Decoder_Interface *pstDecoderInterface,
         void **pPrivateContext
         );

BERR_Code
BXVD_Decoder_GetPPBParameterQueueInfo_isr(
         BXVD_ChannelHandle hXvdCh,
         const BXVD_PPBParameterInfo* apstPPBParameterInfo[],
         uint32_t uiPPBParameterInfoCount,
         uint32_t *puiValidPPBParameterInfoCount
   );

BERR_Code
BXVD_Decoder_SetCRCMode_isr(
         BXVD_ChannelHandle hXvdCh,
         bool bEnable);

BERR_Code
BXVD_Decoder_GetCRCMode_isr(
         BXVD_ChannelHandle hXvdCh,
         bool *pbEnable);


BERR_Code
BXVD_Decoder_StartDecode_isr(
         BXVD_ChannelHandle hXvdCh
         );

BERR_Code
BXVD_Decoder_StopDecode_isr(
         BXVD_ChannelHandle hXvdCh
         );

BERR_Code
BXVD_Decoder_WatchdogReset_isr(
         BXVD_ChannelHandle hXvdCh
         );


BERR_Code
BXVD_Decoder_P_ComputeAspectRatio(
         BXVD_P_PPB * pPPB,
         BXDM_Picture * pstXdmPicture
         );

BERR_Code
BXVD_Decoder_GetCounters_isr(
      BXVD_ChannelHandle hXvdCh,
      BXVD_Decoder_Counters * pstCounters
      );


#ifdef __cplusplus
}
#endif

#endif /* BXVD_DECODER_H_ */
