/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxdm_pp_qm.c $
 * $brcm_Revision: Hydra_Software_Devel/25 $
 * $brcm_Date: 9/11/12 11:33a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/xdm/bxdm_pp_qm.c $
 * 
 * Hydra_Software_Devel/25   9/11/12 11:33a btosi
 * SW7405-4736: (Enhance XDM debug capabilities) standardized the instance
 * Id in the debug message
 * 
 * Hydra_Software_Devel/24   8/15/12 9:24a btosi
 * SW7405-5683:  Disable the FIC and force eSingleField mode when
 * delivering interlaced content to a display running at <= 30 Hz.
 * 
 * Hydra_Software_Devel/23   6/28/12 12:56p btosi
 * SW7425-3237: check uiPendingDropCount prior to calling
 * BXDM_PPVTSM_P_VirtualPtsInterpolate
 * 
 * Hydra_Software_Devel/22   2/16/12 9:29a btosi
 * SW7425-2255: bind the PTS offset to the picture when it is selected for
 * display
 * 
 * Hydra_Software_Devel/21   1/12/12 12:57p btosi
 * SW7425-1001: added support for BAVC_MFD_Picture.bLast
 * 
 * Hydra_Software_Devel/20   10/3/11 2:19p btosi
 * SW7425-1264:  support for a SW STC internal to XDM
 * 
 * Hydra_Software_Devel/19   7/20/11 10:44a btosi
 * SW7635-51: use the SW PCR offset bound to the picture
 * 
 * Hydra_Software_Devel/18   7/20/11 9:38a btosi
 * SW7635-51: snapshot the software PCR offset when a picture is selected
 * for display
 * 
 * Hydra_Software_Devel/SW7635-51/1   5/11/11 9:43a btosi
 * SW7635-51: snapshot the STC offset (uiSoftwarePCROffset) when a picture
 * is selected for display
 * 
 * Hydra_Software_Devel/17   4/21/11 1:53p btosi
 * SW7405-4736: moved BXDM_PPQM_P_PrintUnifiedPicture to bxdm_pp_dbg.c,
 * tweaked the BXDM_PPQM message
 * 
 * Hydra_Software_Devel/16   1/6/11 2:21p delkert
 * SW7405-4736: Change STC display in BXDM_PPQM and BXDM_PPDBG logs to
 * show original STC from decoder with marker indicating JTI application
 * Add output of Playback vs Live indication in BXDM_PPDBG logs.
 *
 * Hydra_Software_Devel/15   12/21/10 4:24p delkert
 * SW7405-5043: Split STC stored in local state into two parts: STC from
 * decoder and STC after JTI adjustment
 *
 * Hydra_Software_Devel/14   12/16/10 9:12a btosi
 * SW7422-72: fixed warnings
 *
 * Hydra_Software_Devel/13   11/30/10 3:59p delkert
 * SW7405-4736: Add picture type to QM message output
 *
 * Hydra_Software_Devel/12   10/20/10 11:47a btosi
 * SW7405-4804: deliver the SEI messages for pictures that are dropped
 *
 * Hydra_Software_Devel/11   10/5/10 7:34a btosi
 * SW7405-4736: for the QM message, print characters for the TSM result
 *
 * Hydra_Software_Devel/10   9/24/10 11:34a btosi
 * SW7405-4736: added a comment
 *
 * Hydra_Software_Devel/9   9/24/10 8:38a btosi
 * SW7405-4736: check for a null picture pointer in
 * BXDM_PPQM_P_PrintUnifiedPicture
 *
 * Hydra_Software_Devel/8   9/23/10 3:09p btosi
 * SW7405-4736: add support for a XDM instance ID to help debug multi-
 * channel issues
 *
 * Hydra_Software_Devel/7   8/12/10 9:38a btosi
 * SW7405-4736: modified QM debug messages
 *
 * Hydra_Software_Devel/6   8/4/10 1:06p btosi
 * SWBLURAY-21565: to aid debug, increment the picture's uiPPBIndex when
 * the picture is validated
 *
 * Hydra_Software_Devel/5   7/12/10 8:05a delkert
 * SW7335-781: Output warning when forcing picture selection override to
 * allow easier detection of conditions such as drop@decode and pcr
 * discontinuity
 *
 * Hydra_Software_Devel/4   6/29/10 11:25a btosi
 * SW7405-4365: revamped the timer code to selectively enable the function
 * and callback timing
 *
 * Hydra_Software_Devel/3   4/29/10 10:47a btosi
 * SW7405-4117: added support for ...DisplayFieldMode_eAuto
 *
 * Hydra_Software_Devel/2   2/22/10 11:36a nilesh
 * SW7405-2993: Added BXDM_Decoder_ReleasePictureInfo pointer to
 * BXDM_Decoder_ReleasePicture call
 *
 * Hydra_Software_Devel/1   2/16/10 10:51a nilesh
 * SW7405-2993: Initial XDM version
 *
 ***************************************************************************/

#include "bstd.h"
#include "bdbg.h"                /* Dbglib */
#include "bkni.h"

#include "bxdm_pp.h"
#include "bxdm_pp_priv.h"
#include "bxdm_pp_qm.h"
#include "bxdm_pp_tsm.h"
#include "bxdm_pp_vtsm.h"
#include "bxdm_pp_callback_priv.h"
#include "bxdm_pp_output.h"
#include "bxdm_pp_clip.h"

BDBG_MODULE(BXDM_PPQM);		/* Register software module with debug interface */

const char BXDM_PictureProvider_P_DISPMGR_QM_NODE[]="DMQM:\t""$brcm_Revision: Hydra_Software_Devel/25 $";

static void BXDM_PPQM_S_ValidatePictureContext(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState * pLocalState,
   BXDM_PictureProvider_P_Picture_Context * pstPicCntxt,
   BXDM_PictureProvider_P_Picture_Context * pstPrevPicCntxt,
   BXDM_PictureProvider_P_Picture_Context * pstSelectedPicCntxt
   );

void BXDM_PPQM_P_InvalidatePictureContext(
   BXDM_PictureProvider_P_Picture_Context * pstPicCntxt
   )
{
   BDBG_ENTER( BXDM_PPQM_P_InvalidatePictureContext );

   BKNI_Memset( pstPicCntxt, 0, sizeof ( BXDM_PictureProvider_P_Picture_Context ) );

   BDBG_LEAVE( BXDM_PPQM_P_InvalidatePictureContext );

   return;
}

static void BXDM_PPQM_S_Detect32FilmContent(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState * pLocalState,
   BXDM_PictureProvider_P_Picture_Context * pstPicCntxt
)
{
   uint32_t i, uiIndex;
   BXDM_Picture_PullDown eExpected32Pulldown = 0;
   bool bDone = false;

   BSTD_UNUSED(pLocalState);

   hXdmPP->stDMState.stDecode.ePPBPulldownHistory[hXdmPP->stDMState.stDecode.uiPPBPulldownHistoryIndex] = pstPicCntxt->pstUnifiedPicture->stBufferInfo.ePulldown;

   if ( hXdmPP->stDMState.stDecode.uiPPBPulldownHistoryIndex == ( BXDM_PictureProvider_P_MAX_PPB_PULLDOWN_HISTORY - 1 ) )
   {
      hXdmPP->stDMState.stDecode.uiPPBPulldownHistoryIndex = 0;
   }
   else
   {
      hXdmPP->stDMState.stDecode.uiPPBPulldownHistoryIndex++;
   }

   if ( hXdmPP->stDMState.stDecode.uiPPBPulldownHistoryCount < BXDM_PictureProvider_P_MAX_PPB_PULLDOWN_HISTORY )
   {
      hXdmPP->stDMState.stDecode.uiPPBPulldownHistoryCount++;
   }

   /* Check for 3:2 cadence */
   if ( hXdmPP->stDMState.stDecode.uiPPBPulldownHistoryCount < BXDM_PictureProvider_P_MAX_PPB_PULLDOWN_HISTORY )
   {
      uiIndex = 0;
   }
   else
   {
      uiIndex = hXdmPP->stDMState.stDecode.uiPPBPulldownHistoryIndex;
   }

   for (i = 0; (i < hXdmPP->stDMState.stDecode.uiPPBPulldownHistoryCount) && (false == bDone); i++ )
   {
      /* We need to seed the initial values */
      if ( ( i != 0 )
           && ( hXdmPP->stDMState.stDecode.ePPBPulldownHistory[uiIndex] != eExpected32Pulldown ) )
      {
         pstPicCntxt->stPicParms.stDisplay.stStatic.b32FilmSource = false;
         bDone = true;
      }
      else
      {
         switch ( hXdmPP->stDMState.stDecode.ePPBPulldownHistory[uiIndex] )
         {
            case BXDM_Picture_PullDown_eTopBottom:
               eExpected32Pulldown = BXDM_Picture_PullDown_eTopBottomTop;
               break;

            case BXDM_Picture_PullDown_eBottomTop:
               eExpected32Pulldown = BXDM_Picture_PullDown_eBottomTopBottom;
               break;

            case BXDM_Picture_PullDown_eTopBottomTop:
               eExpected32Pulldown = BXDM_Picture_PullDown_eBottomTop;
               pstPicCntxt->stPicParms.stDisplay.stStatic.b32FilmSource = true;
               break;

            case BXDM_Picture_PullDown_eBottomTopBottom:
               eExpected32Pulldown = BXDM_Picture_PullDown_eTopBottom;
               pstPicCntxt->stPicParms.stDisplay.stStatic.b32FilmSource = true;
               break;

            default:
               bDone = true;
               break;
         }
      }

      if ( uiIndex == ( BXDM_PictureProvider_P_MAX_PPB_PULLDOWN_HISTORY - 1 ) )
      {
         uiIndex = 0;
      }
      else
      {
         uiIndex++;
      }
   }
}

static void BXDM_PPQM_S_ValidatePictureContext(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState * pLocalState,
   BXDM_PictureProvider_P_Picture_Context * pstPicCntxt,
   BXDM_PictureProvider_P_Picture_Context * pstPrevPicCntxt,
   BXDM_PictureProvider_P_Picture_Context * pstSelectedPicCntxt
   )
{
   BDBG_ENTER( BXDM_PPQM_S_ValidatePictureContext );
   /*
   ** If this context has already been validated, there is nothing to do.
   ** This logic is a precursor to having an array of picture contexts.
   */
   if ( true == pstPicCntxt->bValidated )
   {
      goto AllDone;
   }
   else
   {
      pstPicCntxt->bValidated = true;
   }

   BXDM_PPTMR_P_SnapshotFunctionStartTime( hXdmPP, BXDM_PPTIMER_P_Function_eValidatePictureHead );

   hXdmPP->stDMState.stDecode.uiPPBIndex++;
   pstPicCntxt->stPicParms.uiPPBIndex = hXdmPP->stDMState.stDecode.uiPPBIndex;

   /* PR50196: Detect 3:2 film content and override source format as
    * needed */
   BXDM_PPQM_S_Detect32FilmContent( hXdmPP, pLocalState, pstPicCntxt );

   /* Set TSM parameters */
   BXDM_PPTSM_P_PtsCalculateParameters( hXdmPP, pLocalState, pstPicCntxt );

   /* By default, set the selection mode for this picture equal to the
    * display mode */
   pstPicCntxt->stPicParms.stTSM.stDynamic.eSelectionMode = pLocalState->eSelectionMode;

   /*
    * If in playback mode, force PCR offset and all the associated
    * flags to '0'.
    *
    * If "live" playback, set the offset and flags based on the
    * data in the PPB and the system state.
    */

   /* Zero out local PCR copy */
   BKNI_Memset(
            &pstPicCntxt->stPicParms.stTSM.stStatic.stPCROffsetXDM,
            0,
            sizeof( BXDM_PictureProvider_P_PCROffset )
            );

   /* Set local PCR copy to be same as coded copy */
   if ( false == hXdmPP->stDMConfig.bPlayback )
   {
      pstPicCntxt->stPicParms.stTSM.stStatic.stPCROffsetXDM.stPCROffsetCopy = pstPicCntxt->pstUnifiedPicture->stPCROffset;
   }

   /* Handle coded PTS related logic */
   if ( true == pstPicCntxt->pstUnifiedPicture->stPTS.bValid )
   {
      /* used to generated the first coded PTS callback */
      pLocalState->bCodedPtsReceived = true;

      /* Snapshot the first coded PTS received, this value will be delivered to
       * the application via the "BXVD_Interrupt_eFirstPTSReady" callback.
       * Previously the value returned by this callback was "stLatestCodedPts".
       * Testing preroll mode highlighted the need for the snapshot; preroll starts in vsync mode,
       * the first picture received will be promoted for display.  When the queue was empty, the PTS
       * value returned by "BXVD_Interrupt_eFirstPTSReady" was from the first picture.  When the queue
       * was not empty, the PTS returned was from the second picture.
       */
      if ( false == hXdmPP->stDMState.stDecode.bFirstCodedPtsReceived )
      {
         hXdmPP->stDMState.stDecode.stFirstCodedPts.ui32RunningPTS = pstPicCntxt->pstUnifiedPicture->stPTS.uiValue;
         hXdmPP->stDMState.stDecode.stFirstCodedPts.ePTSType = BXDM_PictureProvider_PTSType_eCoded;
         hXdmPP->stDMState.stDecode.bFirstCodedPtsReceived = true;
      }

      /* save the PTS */
      hXdmPP->stDMState.stDecode.stLatestCodedPts.ui32RunningPTS = pstPicCntxt->pstUnifiedPicture->stPTS.uiValue;
      hXdmPP->stDMState.stDecode.stLatestCodedPts.ePTSType = BXDM_PictureProvider_PTSType_eCoded;

      /* PR52901: save the PCR info.  An application can use this when it receives
       * the BXVD_Interrupt_eFirstPTSReady callback.  Or when calling BXVD_GetPTS
       * before a picture has been promoted for display.
       */
      hXdmPP->stDMState.stDecode.stLatestCodedPts.uiPCROffset = pstPicCntxt->stPicParms.stTSM.stStatic.stPCROffsetXDM.stPCROffsetCopy.uiValue;
      hXdmPP->stDMState.stDecode.stLatestCodedPts.bPcrOffsetValid = pstPicCntxt->stPicParms.stTSM.stStatic.stPCROffsetXDM.stPCROffsetCopy.bValid;
      hXdmPP->stDMState.stDecode.stLatestCodedPts.bPcrDiscontinuity = pstPicCntxt->stPicParms.stTSM.stStatic.stPCROffsetXDM.stPCROffsetCopy.bDiscontinuity;
   }

   /* Override local PCR copy if it is not valid.  Use the previous PCR copy (if valid). */
   if ( ( false == hXdmPP->stDMConfig.bPlayback )
        && ( false == pstPicCntxt->stPicParms.stTSM.stStatic.stPCROffsetXDM.stPCROffsetCopy.bValid ) )
   {
      /* The PCR is invalid, so we set the PCR for this PPB to be the
       * same as the previous PPB if it is valid, otherwise, we set it
       * to zero */
      if ( true == pstPrevPicCntxt->bValidated )
      {
         pstPicCntxt->stPicParms.stTSM.stStatic.stPCROffsetXDM.stPCROffsetCopy.uiValue = pstPrevPicCntxt->stPicParms.stTSM.stStatic.stPCROffsetXDM.stPCROffsetCopy.uiValue;
         pstPicCntxt->stPicParms.stTSM.stStatic.stPCROffsetXDM.stPCROffsetCopy.bValid = pstPrevPicCntxt->stPicParms.stTSM.stStatic.stPCROffsetXDM.stPCROffsetCopy.bValid;
      }
      else
      {
         pstPicCntxt->stPicParms.stTSM.stStatic.stPCROffsetXDM.stPCROffsetCopy.uiValue = 0;
         pstPicCntxt->stPicParms.stTSM.stStatic.stPCROffsetXDM.stPCROffsetCopy.bValid = false;
      }
   }

   if ( true == hXdmPP->stDMConfig.bPlayback )
   {
      hXdmPP->stDMState.stDecode.bValidPcrOffsetReceived = true;
   }
   else
   {
      hXdmPP->stDMState.stDecode.bValidPcrOffsetReceived = pstPicCntxt->stPicParms.stTSM.stStatic.stPCROffsetXDM.stPCROffsetCopy.bValid;

      /* PR50623: If the application has specified to NOT use
       * the HW PCR offset set the overloaded flags accordingly.
       * Otherwise load them with the derived values.
       */
      if ( false == hXdmPP->stDMConfig.bUseHardwarePCROffset )
      {
         pstPicCntxt->stPicParms.stTSM.stStatic.stPCROffsetXDM.bValidOverloaded = true;
         pstPicCntxt->stPicParms.stTSM.stStatic.stPCROffsetXDM.bDiscontinuityOverloaded = false;
      }
      else
      {
         pstPicCntxt->stPicParms.stTSM.stStatic.stPCROffsetXDM.bValidOverloaded =
                  pstPicCntxt->stPicParms.stTSM.stStatic.stPCROffsetXDM.stPCROffsetCopy.bValid;

         pstPicCntxt->stPicParms.stTSM.stStatic.stPCROffsetXDM.bDiscontinuityOverloaded =
                  pstPicCntxt->stPicParms.stTSM.stStatic.stPCROffsetXDM.stPCROffsetCopy.bDiscontinuity;
      }
   }

   /* If an error, bump the error count.  */
   if( true == pstPicCntxt->pstUnifiedPicture->stError.bThisPicture )
   {
      hXdmPP->stDMStatus.stCounters.uiDecodeErrorCount++;
      pLocalState->bDecodeError = true;
   }

   /* Check for dummy PPB */
   if ( true == pstPicCntxt->pstUnifiedPicture->stBufferInfo.bValid )
   {
      hXdmPP->stDMState.stDecode.bFirstPPBSeen = true;
   }

   /*
    * Count the number of I-Frames.  Informational only, for use by an application.
    */
   if ( BXDM_Picture_Coding_eI == pstPicCntxt->pstUnifiedPicture->stPictureType.eCoding )
   {
      hXdmPP->stDMStatus.stCounters.uiIFrameCount++;

      if ( true == pstPicCntxt->pstUnifiedPicture->stError.bThisPicture )
      {
         hXdmPP->stDMStatus.stCounters.uiErrorIFrameCount++;
      }
   }

   /* SW7425-1001: check for "bLastPicture" being set, effectively an EOS flag.  
    * Currently defined to only be delivered with a "picture-less" picture.
    * 
    * If the flag is set for a "picture-less" picture, the picture currently 
    * selected for display is the last one of this video.  When the selected picture 
    * has been displayed completely, "bLast" should be set in the MFD picture structure.
    * This is done in BXDM_PPOUT_P_CalculateVdcData.
    * 
    * It is an error condition if "bLastPicture" is set for a "standard" picture.  The flag
    * will be ignored in this instance.
    */
   if ( true == pstPicCntxt->pstUnifiedPicture->stPictureType.bLastPicture )
   {
      /* "stBufferInfo.bValid" being "false" means that this is a "picture-less" picture.
       * Mark the currently selected picture as the last one.
       */
      if ( false == pstPicCntxt->pstUnifiedPicture->stBufferInfo.bValid )
      {
         pstSelectedPicCntxt->stPicParms.stDisplay.stDynamic.bLastPictureSeen = true;
      }
      else
      {
         BDBG_ERR(("%x:[%02x.%03x]  BXDM_PPQM_S_ValidatePictureContext:: bLastPicture is set for a standard picture.",
                       hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                       BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ),
                       pstPicCntxt->stPicParms.uiPPBIndex & 0xFFF ));
      }
   }


   /*
    * Bump the running count of pictures seen by the video decoder.
    */
   hXdmPP->stDMStatus.stCounters.uiPicturesReceivedCount += pstPicCntxt->pstUnifiedPicture->stStats.uiDeltaPicturesSeen;

   BXDM_PPOUT_P_CalculateStaticVdcData( hXdmPP, pstPicCntxt );

   /* Conditionally execute the PPB parameters callback. */
   pLocalState->bPictureUnderEvaluation = true;
   BXDM_PPCB_P_ExecuteSingleCallback( hXdmPP, pLocalState, BXDM_PictureProvider_Callback_ePictureUnderEvaluation );

   BXDM_PPTMR_P_SnapshotFunctionEndTime( hXdmPP, BXDM_PPTIMER_P_Function_eValidatePictureHead );

AllDone:

   BXDM_PPTMR_P_SnapshotFunctionStartTime( hXdmPP, BXDM_PPTIMER_P_Function_eValidatePictureTail );

   /* PR52898: Remember the current eDisplayFieldMode setting for this
    * picture to prevent a change to the display field mode affecting
    * the picture after it's been displayed on the screen
    *
    * SW7405-4117: when BXDM_PictureProvider_DisplayFieldMode_eAuto is specified, the stream
    * height is used in conjunction with uiMaxHeightSupportedByDeinterlacer
    * to choose either eSingleField or eBothField slow motion (and preroll).
    *
    * SW7405-5683:  Delivering interlaced content to a display running at <= 30 Hz is not supported by the system. 
    * The most common case would be 1080i60 to a 30p display. To get the best possible results, the FIC is disabled 
    * and the display mode is forced to eSingleField for interlaced content when the monitor rate is <= 30 Hz.
    */
   if ( true == pstPicCntxt->stPicParms.stDisplay.stDynamic.bForceSingleFieldMode )
   {
      /* SW7405-5683: */
      pstPicCntxt->stPicParms.stDisplay.stDynamic.eDisplayFieldMode = BXDM_PictureProvider_DisplayFieldMode_eSingleField;
   }
   else if ( BXDM_PictureProvider_DisplayFieldMode_eAuto == hXdmPP->stDMConfig.eDisplayFieldMode )
   {
      bool bPausedOrSlowMotion;

      /* Check for a STC trick mode of pause or slow motion.
       */
      bPausedOrSlowMotion = ( BXDM_PictureProvider_P_STCTrickMode_ePause == pLocalState->eSTCTrickMode );
      bPausedOrSlowMotion |= ( BXDM_PictureProvider_P_STCTrickMode_eSlowMotion == pLocalState->eSTCTrickMode );
      bPausedOrSlowMotion |= ( BXDM_PictureProvider_P_STCTrickMode_eSlowRewind == pLocalState->eSTCTrickMode );
      
      /* Check for a decoder trick mode of pause or slow motion.  This check encompasses preroll.
       */
      bPausedOrSlowMotion |= ( pLocalState->uiSlowMotionRate < BXDM_PICTUREPROVIDER_NORMAL_PLAYBACK_RATE );

      /* Default to eBothField */
      pstPicCntxt->stPicParms.stDisplay.stDynamic.eDisplayFieldMode = BXDM_PictureProvider_DisplayFieldMode_eBothField;

      /* When paused or in slow motion, revert to eSingleField if the deinterlacer cannot support the stream. */
      if ( pstPicCntxt->pstUnifiedPicture->stBufferInfo.stSource.uiHeight > hXdmPP->stDMConfig.uiMaxHeightSupportedByDeinterlacer
            && true == bPausedOrSlowMotion
         )
      {
         pstPicCntxt->stPicParms.stDisplay.stDynamic.eDisplayFieldMode = BXDM_PictureProvider_DisplayFieldMode_eSingleField;
      }
   }
   else
   {
      pstPicCntxt->stPicParms.stDisplay.stDynamic.eDisplayFieldMode = hXdmPP->stDMConfig.eDisplayFieldMode;
   }

   BXDM_PPCLIP_P_ClipTimeQueueTransitionHandler(
      hXdmPP,
      pLocalState,
      pstPicCntxt
      );

   /* We enter auto-vsync mode when ALL of the following are true:
    *
    * 1) We're in live mode (PCR is invalid during playback)
    * 2) The STC is NOT invalid (STC has been valid at least once
    *    during the decode)
    * 3) The "bVsyncModeOnPcrDiscontinuity" is set.
    * 4) The currently selected picture indicates a PCR discontinuity.
    *    (This is the trigger for auto-vsync mode)
    *    (Note: if the HW PCR offset is being ignored, discontinuity will
    *     be ignored as well.  Hence the use of "bDiscontinuityOverloaded")
    */
   if ( false == hXdmPP->stDMConfig.bPlayback
         && true == hXdmPP->stDMConfig.bSTCValid
         && true == hXdmPP->stDMConfig.bVirtualTSMOnPCRDiscontinuity
         && true == pstPicCntxt->stPicParms.stTSM.stStatic.stPCROffsetXDM.bDiscontinuityOverloaded
      )
   {
      /* SW7335-781: Output warning when forcing picture selection override */
      BXDM_PPDBG_P_PrintSelectionModeOverride("PCR Discontinuity", hXdmPP, pstPicCntxt);
      pstPicCntxt->stPicParms.stTSM.stDynamic.eSelectionMode = BXDM_PictureProvider_DisplayMode_eVirtualTSM;

   }

   /* If the decode supports drop@decode, check if there is an outstanding drop request.
    * 
    * SW7425-3237: The vPTS (virtual PTS) of the current picture is based on "eSelectionMode", 
    * the picture selection mode.
    * - If the mode is eTSM, vPTS will be based on the preceding picture's ACTUAL PTS value.
    * - If the mode is eVirtual (vsync), vPTS will be based on the preceding picture's VIRTUAL PTS.
    * 
    * With the above in mind, any "eSelectionMode" overrides should occur before the vPTS is set, 
    * i.e. before "BXDM_PPVTSM_P_VirtualPtsInterpolate" is called.
    * 
    * The failure reported by SW7425-3237 was due to the following code block being located 
    * in "BXDM_PPTSM_S_CompareStcAndPts".  The failure was found by doing a "hot swap"; the streamer 
    * was playing one stream and then switched to a second stream without executing a Stop/Start decode.
    * This resulted in the coded PTS values taking a large step back in time. In addition, the system was 
    * running in vsync mode due to the PCR being discontinuous. The failure came about when the PCR became 
    * "continuous", the vPTS for that picture was based the ACTUAL PTS of the preceding picture, however 
    * the virtual STC was still running on the old time base.  As a result the picture was "late" as were 
    * all subsequent ones.  The virtual STC didn't reset because the system never transitioned out of vsync mode.
    *
    * By moving this code block here, when "uiPendingDropCount" is non-zero, the picture's vPTS will
    * be based on the preceding picture's VIRTUAL PTS.
    */
   if ( ( NULL != hXdmPP->stDMConfig.stDecoderInterface.requestPictureDrop_isr )
        && ( NULL != hXdmPP->stDMConfig.stDecoderInterface.getPictureDropPendingCount_isr )
      )
   {
      /* Snapshot the start time. */
      BXDM_PPTMR_P_SnapshotFunctionStartTime( hXdmPP, BXDM_PPTIMER_P_Function_eDecoderGetPictureDropPendingCountIsr );

      hXdmPP->stDMConfig.stDecoderInterface.getPictureDropPendingCount_isr(
               hXdmPP->stDMConfig.pDecoderPrivateContext,
               &hXdmPP->stDMState.stDecode.uiPendingDropCount
               );

      /* Snapshot the end time. */
      BXDM_PPTMR_P_SnapshotFunctionEndTime( hXdmPP, BXDM_PPTIMER_P_Function_eDecoderGetPictureDropPendingCountIsr );

      if ( 0 != hXdmPP->stDMState.stDecode.uiPendingDropCount )
      {
         /* SW7335-781: Output warning when forcing picture selection override */
         BXDM_PPDBG_P_PrintSelectionModeOverride("Drop@Decode", hXdmPP, pstPicCntxt);
         /* SW7405-3929: If there is a pending drop@decode, then override selection mode to vTSM mode for this picture */
         pstPicCntxt->stPicParms.stTSM.stDynamic.eSelectionMode = BXDM_PictureProvider_DisplayMode_eVirtualTSM;
      }
   }


   if ( BXDM_PictureProvider_DisplayMode_eVirtualTSM != pstPicCntxt->stPicParms.stTSM.stDynamic.eSelectionMode )
   {
      /* Handle auto-vsync mode for this picture */
      pstPicCntxt->stPicParms.stTSM.stDynamic.eSelectionMode = pLocalState->eSelectionMode;

   }

   /* Now that all the necessary parameters have been dereferenced,
    * calculate the appropriate PTS values.
    */
   BXDM_PPTSM_P_PtsInterpolate( hXdmPP, pLocalState, pstPicCntxt, pstPrevPicCntxt );
   BXDM_PPVTSM_P_VirtualPtsInterpolate( hXdmPP, pLocalState, pstPicCntxt, pstPrevPicCntxt, pstSelectedPicCntxt );

   BXDM_PPTMR_P_SnapshotFunctionEndTime( hXdmPP, BXDM_PPTIMER_P_Function_eValidatePictureTail );

   BDBG_LEAVE( BXDM_PPQM_S_ValidatePictureContext );

   return;

}

/*******************************************************************************
 **
 ** end "private" display queue management routines.
 **
 *******************************************************************************/


/*******************************************************************************
 **
 ** Routines for manipulating picture context.
 **
 *******************************************************************************/


void BXDM_PPQM_P_GetHwPcrOffset(
   BXDM_PictureProvider_P_Picture_Context * pstPicture,
   uint32_t * puiPcrOffset
   )
{
   *puiPcrOffset = pstPicture->stPicParms.stTSM.stStatic.stPCROffsetXDM.stPCROffsetCopy.uiValue;
   return;
}

/* PR50623: return the "cooked" PCR offset, i.e. the sum
 * of the HW and SW PCR offsets.
 */
void BXDM_PPQM_P_GetCookedPcrOffset(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_Picture_Context * pstPicture,
   uint32_t * puiPcrOffset
   )
{
   /* The "returned" PCR offset always includes the software value.
    * ( This is '0' by default.  It is optionally set by the
    *  application or PCRLib.)
    *
    * SW7635-51: when a picture is selected for display, uiSoftwarePCROffset is bound
    * to the picture and "bSwPcrOffsetValid" is set true.  Prior to selecting a picture,
    * "uiSoftwarePCROffset" will be false.
    */
   if ( true == pstPicture->stPicParms.stDisplay.stDynamic.bSwPcrOffsetValid )
   {
      *puiPcrOffset = pstPicture->stPicParms.stDisplay.stDynamic.uiSoftwarePCROffset;
   }
   else
   {
      *puiPcrOffset = hXdmPP->stDMConfig.uiSoftwarePCROffset;
   }

   /* Conditionally add the hardware PCR offset. The default behavior
    * at channel open is to set "bUseHwPcrOffset" to true.
    */
   if ( true == hXdmPP->stDMConfig.bUseHardwarePCROffset )
   {
      *puiPcrOffset += pstPicture->stPicParms.stTSM.stStatic.stPCROffsetXDM.stPCROffsetCopy.uiValue;
   }

   return;
}

/* SW7425-2255: return the appropriate display offset.
 */
void BXDM_PPQM_P_GetPtsOffset(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_Picture_Context * pstPicture,
   uint32_t * puiPtsOffset
   )
{
   /* SW7425-2255: when a picture is selected for display, uiPTSOffset is bound
    * to the picture and "bPtsOffsetValid" is set true.  Prior to selecting a picture,
    * "bPtsOffsetValid" will be false.
    */
   if ( true == pstPicture->stPicParms.stDisplay.stDynamic.bPtsOffsetValid )
   {
      *puiPtsOffset = pstPicture->stPicParms.stDisplay.stDynamic.uiPtsOffset;
   }
   else
   {
      *puiPtsOffset = hXdmPP->stDMConfig.uiPTSOffset;
   }

   return;
}


/*
** Public functions.
*/
void BXDM_PPQM_P_GetPredictedPts(
   BXDM_PictureProvider_P_Picture_Context * pstPicture,
   BXDM_PictureProvider_P_PTSIndex ePTSIndex,
   uint32_t * puiPts
   )
{
   BDBG_ASSERT(puiPts);

   if ( false == pstPicture->bValidated )
   {
      *puiPts = 0;
   }
   else
   {
      *puiPts = pstPicture->stPicParms.stTSM.stStatic.stPTSOfNextPPB[ePTSIndex].uiWhole;
   }

   return;

}

void BXDM_PPQM_P_GetPredictedPtsWithFrac(
   BXDM_PictureProvider_P_Picture_Context * pstPicture,
   BXDM_PictureProvider_P_PTSIndex ePTSIndex,
   BXDM_PPFP_P_DataType *pstPTS
   )
{
   BDBG_ASSERT(pstPTS);

   if ( false == pstPicture->bValidated )
   {
      pstPTS->uiWhole = 0;
      pstPTS->uiFractional = 0;
   }
   else
   {
      *pstPTS = pstPicture->stPicParms.stTSM.stStatic.stPTSOfNextPPB[ePTSIndex];
   }

   return;

}

void BXDM_PPQM_P_SetPredictedPtsWithFrac(
   BXDM_PictureProvider_P_Picture_Context * pstPicture,
   BXDM_PictureProvider_P_PTSIndex ePTSIndex,
   BXDM_PPFP_P_DataType stPTS
   )
{

   pstPicture->stPicParms.stTSM.stStatic.stPTSOfNextPPB[ePTSIndex] = stPTS;

   return;
}

void BXDM_PPQM_P_GetPtsType(
   BXDM_PictureProvider_P_Picture_Context * pstPicture,
   BXDM_PictureProvider_PTSType * pePtsType
   )
{

   if ( false == pstPicture->bValidated )
   {
      *pePtsType = BXDM_PictureProvider_PTSType_eInterpolatedFromInvalidPTS;
   }
   else
   {
      *pePtsType = pstPicture->stPicParms.stTSM.stStatic.ePtsType;
   }

   return;
}

void BXDM_PPQM_P_GetPtsTypeUnfiltered(
   BXDM_PictureProvider_P_Picture_Context * pstPicture,
   BXDM_PictureProvider_PTSType * pePtsType
   )
{
   *pePtsType = pstPicture->stPicParms.stTSM.stStatic.ePtsType;

   return;
}


void BXDM_PPQM_P_SetPtsWithFrac(
   BXDM_PictureProvider_P_Picture_Context * pstPicture,
   BXDM_PictureProvider_P_PTSIndex ePTSIndex,
   uint32_t    uiIndex,
   BXDM_PPFP_P_DataType stPTS
   )
{
   BDBG_ASSERT( uiIndex < BXDM_PictureProvider_P_MAX_ELEMENTS_PER_PPB );

   /* A runtime check to keep Coverity happy. */
   if ( uiIndex >= BXDM_PictureProvider_P_MAX_ELEMENTS_PER_PPB )
   {
      uiIndex = BXDM_PictureProvider_P_MAX_ELEMENTS_PER_PPB - 1;
   }

   pstPicture->stPicParms.stTSM.stStatic.astElmts[ uiIndex ].stPTS[ePTSIndex] = stPTS;

   return;
}

void BXDM_PPQM_P_GetPtsWithFrac(
   BXDM_PictureProvider_P_Picture_Context * pstPicture,
   BXDM_PictureProvider_P_PTSIndex ePTSIndex,
   uint32_t    uiIndex,
   BXDM_PPFP_P_DataType *pstPTS
   )
{
   BDBG_ASSERT( uiIndex < BXDM_PictureProvider_P_MAX_ELEMENTS_PER_PPB );

   /* A runtime check to keep Coverity happy. */
   if ( uiIndex >= BXDM_PictureProvider_P_MAX_ELEMENTS_PER_PPB )
   {
      uiIndex = BXDM_PictureProvider_P_MAX_ELEMENTS_PER_PPB - 1;
   }

   if ( false == pstPicture->bValidated )
   {
      pstPTS->uiWhole = 0;
      pstPTS->uiFractional = 0;
   }
   else
   {
      *pstPTS = pstPicture->stPicParms.stTSM.stStatic.astElmts[ uiIndex ].stPTS[ePTSIndex];
   }

   return;
}


void BXDM_PPQM_P_GetPts(
   BXDM_PictureProvider_P_Picture_Context * pstPicture,
   BXDM_PictureProvider_P_PTSIndex ePTSIndex,
   uint32_t    uiIndex,
   uint32_t * puiPts
   )
{
   BDBG_ASSERT( uiIndex < BXDM_PictureProvider_P_MAX_ELEMENTS_PER_PPB );

   /* A runtime check to keep Coverity happy. */
   if ( uiIndex >= BXDM_PictureProvider_P_MAX_ELEMENTS_PER_PPB )
   {
      uiIndex = BXDM_PictureProvider_P_MAX_ELEMENTS_PER_PPB - 1;
   }


   if ( false == pstPicture->bValidated )
   {
      *puiPts = 0;
   }
   else
   {
      *puiPts = pstPicture->stPicParms.stTSM.stStatic.astElmts[ uiIndex ].stPTS[ePTSIndex].uiWhole;
   }

   return;
}

void BXDM_PPQM_P_GetPtsUnfiltered(
   BXDM_PictureProvider_P_Picture_Context * pstPicture,
   BXDM_PictureProvider_P_PTSIndex ePTSIndex,
   uint32_t    uiIndex,
   uint32_t * puiPts
   )
{
   BDBG_ASSERT( uiIndex < BXDM_PictureProvider_P_MAX_ELEMENTS_PER_PPB );

   /* A runtime check to keep Coverity happy. */
   if ( uiIndex >= BXDM_PictureProvider_P_MAX_ELEMENTS_PER_PPB )
   {
      uiIndex = BXDM_PictureProvider_P_MAX_ELEMENTS_PER_PPB - 1;
   }

   *puiPts = pstPicture->stPicParms.stTSM.stStatic.astElmts[ uiIndex ].stPTS[ePTSIndex].uiWhole;

   return;
}

void BXDM_PPQM_P_GetLastPts(
   BXDM_PictureProvider_P_Picture_Context * pstPicture,
   BXDM_PictureProvider_P_PTSIndex ePTSIndex,
   uint32_t * puiPts
   )
{
   uint32_t    uiIndex;

   if ( false == pstPicture->bValidated )
   {
      *puiPts = 0;
   }
   else
   {
      /* This range checking probably isn't required. */

      if ( BXDM_PictureProvider_P_MAX_ELEMENTS_PER_PPB <= pstPicture->stPicParms.stTSM.stStatic.uiNumElements )
      {
         uiIndex = ( BXDM_PictureProvider_P_MAX_ELEMENTS_PER_PPB - 1 );
      }
      else if ( 0 >= pstPicture->stPicParms.stTSM.stStatic.uiNumElements )
      {
         uiIndex = 0;
      }
      else
      {
         uiIndex = ( pstPicture->stPicParms.stTSM.stStatic.uiNumElements  - 1 );
      }

      BXDM_PPQM_P_GetPts(
                     pstPicture,
                     ePTSIndex,
                     uiIndex,
                     puiPts
                     );

   }

   return;

}

void BXDM_PPQM_P_SetElementPolarity(
   BXDM_PictureProvider_P_Picture_Context *pstPicture,
   uint32_t uiElementIndex,
   BAVC_Polarity ePolarity)
{

   pstPicture->stPicParms.stTSM.stStatic.astElmts[ uiElementIndex ].ePolarity = ePolarity;

   return;
}

void BXDM_PPQM_P_GetElementPolarity(
   BXDM_PictureProvider_P_Picture_Context *pstPicture,
   uint32_t uiElementIndex,
   BAVC_Polarity *pePolarity)
{

   BDBG_ASSERT(pePolarity);

   *pePolarity = pstPicture->stPicParms.stTSM.stStatic.astElmts[ uiElementIndex ].ePolarity;

   return;
}


/*******************************************************************************
 **
 ** End routines for manipulating picture context.
 **
 *******************************************************************************/


/*******************************************************************************
 **
 ** "public" display queue management routines.
 **
 *******************************************************************************/
bool BXDM_PPQM_P_PeekAtNextPicture(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState * pLocalState,
   BXDM_PictureProvider_P_Picture_Context * pstPicCntxt,
   BXDM_PictureProvider_P_Picture_Context * pstPrevPicCntxt,
   BXDM_PictureProvider_P_Picture_Context * pstSelectedPicCntxt
   )
{
   uint32_t uiPictureCount = 0;

   BDBG_ENTER( BXDM_PPQM_P_PeekAtNextPicture );

   /* Snapshot the start time. */
   BXDM_PPTMR_P_SnapshotFunctionStartTime( hXdmPP, BXDM_PPTIMER_P_Function_eDecoderGetPictureCountIsr );

   hXdmPP->stDMConfig.stDecoderInterface.getPictureCount_isr(
            hXdmPP->stDMConfig.pDecoderPrivateContext,
            &uiPictureCount
            );

   /* Snapshot the end time. */
   BXDM_PPTMR_P_SnapshotFunctionEndTime( hXdmPP,BXDM_PPTIMER_P_Function_eDecoderGetPictureCountIsr );

   if ( 0 != uiPictureCount )
   {
      /* Used to conditionally generated the "first PTS ready" callback.
       * Which is really the "first PPB received" callback.
       */
      pLocalState->bFirstPPBReceived = true;

      /* Snapshot the start time. */
      BXDM_PPTMR_P_SnapshotFunctionStartTime( hXdmPP, BXDM_PPTIMER_P_Function_eDecoderPeekAtPictureIsr );

      hXdmPP->stDMConfig.stDecoderInterface.peekAtPicture_isr(
               hXdmPP->stDMConfig.pDecoderPrivateContext,
               0,
               &pstPicCntxt->pstUnifiedPicture
               );

      /* Snapshot the end time. */
      BXDM_PPTMR_P_SnapshotFunctionEndTime( hXdmPP, BXDM_PPTIMER_P_Function_eDecoderPeekAtPictureIsr );

      /* Update the dynamic context associate with this PPB. */
      BXDM_PPQM_S_ValidatePictureContext( hXdmPP, pLocalState, pstPicCntxt, pstPrevPicCntxt, pstSelectedPicCntxt );
   }

   BDBG_LEAVE( BXDM_PPQM_P_PeekAtNextPicture );

   return ( 0 != uiPictureCount );
}

void BXDM_PPQM_P_GetNextPicture(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState * pLocalState,
   BXDM_PictureProvider_P_Picture_Context * pstPicCntxt
   )
{
   uint32_t uiPictureCount = 0;

   BDBG_ENTER( BXDM_PPQM_P_GetNextPicture );
   BSTD_UNUSED(pLocalState);

   /* This routine assumes that there is data in the queue.
    * The following check is to help with debug.
    */

   /* Snapshot the start time. */
   BXDM_PPTMR_P_SnapshotFunctionStartTime( hXdmPP, BXDM_PPTIMER_P_Function_eDecoderGetPictureCountIsr );

   hXdmPP->stDMConfig.stDecoderInterface.getPictureCount_isr(
            hXdmPP->stDMConfig.pDecoderPrivateContext,
            &uiPictureCount
            );

   /* Snapshot the end time. */
   BXDM_PPTMR_P_SnapshotFunctionEndTime( hXdmPP, BXDM_PPTIMER_P_Function_eDecoderGetPictureCountIsr );

   BDBG_ASSERT( 0 != uiPictureCount );

   /* Snapshot the start time. */
   BXDM_PPTMR_P_SnapshotFunctionStartTime( hXdmPP, BXDM_PPTIMER_P_Function_eDecoderGetNextPictureIsr );

   hXdmPP->stDMConfig.stDecoderInterface.getNextPicture_isr(
            hXdmPP->stDMConfig.pDecoderPrivateContext,
            &pstPicCntxt->pstUnifiedPicture
            );

   /* Snapshot the end time. */
   BXDM_PPTMR_P_SnapshotFunctionEndTime( hXdmPP, BXDM_PPTIMER_P_Function_eDecoderGetNextPictureIsr );

   /*  Since the PPB has been "removed" from the delivery queue, update the "PPBReceived" count.  */
   hXdmPP->stDMStatus.stCounters.uiPicturesDecodedCount++;

   BXDM_PPDBG_P_PrintUnifiedPicture( hXdmPP, pLocalState, pstPicCntxt );

   BDBG_LEAVE( BXDM_PPQM_P_GetNextPicture );

   return;

}   /* end of BXDM_PPQM_P_GetNextPicture() */

void BXDM_PPQM_P_ReleasePicture(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_Picture_Context * pstPicCntxt
   )
{
   BDBG_ENTER( BXDM_PPQM_P_ReleasePicture );

   /* Snapshot the start time. */
   BXDM_PPTMR_P_SnapshotFunctionStartTime( hXdmPP, BXDM_PPTIMER_P_Function_eDecoderReleasePictureIsr );

   if ( NULL != pstPicCntxt->pstUnifiedPicture )
   {
      hXdmPP->stDMConfig.stDecoderInterface.releasePicture_isr(
               hXdmPP->stDMConfig.pDecoderPrivateContext,
               pstPicCntxt->pstUnifiedPicture,
               NULL
               );
   }

   /* Snapshot the end time. */
   BXDM_PPTMR_P_SnapshotFunctionEndTime( hXdmPP, BXDM_PPTIMER_P_Function_eDecoderReleasePictureIsr );

   /* Bump the PPB released count. */
   hXdmPP->stDMStatus.stCounters.uiPicturesReleasedCount++;

   /* Invalidate context */
   pstPicCntxt->pstUnifiedPicture = NULL;
   pstPicCntxt->bValidated = false;

   BDBG_LEAVE( BXDM_PPQM_P_ReleasePicture );

   return;

}

void BXDM_PPQM_P_ReleasePictureExt(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState,
   BXDM_PictureProvider_P_Picture_Context * pstPicCntxt
   )
{
   BDBG_ENTER( BXDM_PPQM_P_ReleasePictureExt );

   /* SW7405-4804: Typically the extension data callback will be executed when a
    * picture is delivered to VDC via the "BXDM_PPCB_P_ExecuteCallbacks" function.
    * However this does not happen for pictures that are dropped.  This case is
    * now handled by calling "BXDM_PPCB_P_ExecuteSingleCallbackExt" whenever a
    * picture is released.  The flag "bExtensionDataDelivered" ensures that the
    * callback is only executed once per picture.
    */
   BXDM_PPCB_P_ExecuteSingleCallbackExt(
               hXdmPP,
               pLocalState,
               pstPicCntxt,
               BXDM_PictureProvider_Callback_ePictureExtensionData
               );

   BXDM_PPQM_P_ReleasePicture( hXdmPP, pstPicCntxt );

   BDBG_LEAVE( BXDM_PPQM_P_ReleasePictureExt );

   return;
}

void BXDM_PPQM_P_PromotePictureToDisplayStatus(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState * pLocalState,
   BXDM_PictureProvider_P_Picture_Context * pstNewPicture
   )
{
   BXDM_PictureProvider_P_Picture_Context *  pstSelectedPicture;

   BDBG_ENTER( BXDM_PPQM_P_PromotePictureToDisplayStatus );

   pstSelectedPicture = &(hXdmPP->stDMState.stChannel.stSelectedPicture);

   /* copy the context on the new picture */

   *pstSelectedPicture = *pstNewPicture;

   /*
    * The following logic is esentially cloned from DM V1.
    * "bThisIsTheFirstPPB" will only be true the first time a picture
    * is promoted to display status.
    * "bFirstPPBHasBeenDisplayed" will remain true once a picture is
    * promoted to display status.
    */
   if ( false == hXdmPP->stDMState.stDecode.bFirstPPBHasBeenDisplayed )
   {
      hXdmPP->stDMState.stDecode.bThisIsTheFirstPPB = true;
   }
   else
   {
      hXdmPP->stDMState.stDecode.bThisIsTheFirstPPB = false;
   }

   hXdmPP->stDMState.stDecode.bFirstPPBHasBeenDisplayed = true;

   /* If the picture is the first I frame, generate the BXVD_Interrupt_eIFrame callback */

   BXDM_PPCB_P_EvaluateCriteria_IFrame( hXdmPP, pLocalState, pstSelectedPicture );

   /* Since we've promoted a new PPB, we need to reset the PPB and
    * Element repeated flags to false */
   pstSelectedPicture->stPicParms.stDisplay.stDynamic.bPPBRepeated = false;
   pstSelectedPicture->stPicParms.stDisplay.stDynamic.bElementRepeated = false;

   /* Remember the SPIM setting for this picture to handle cases where
    * SPIM is changed from "both" to "default" during a trick mode
    * transition (e.g. pause -> play */
   pstSelectedPicture->stPicParms.stDisplay.stDynamic.eSourceFormatOverride = hXdmPP->stDMConfig.eSourceFormatOverride;

   /* SW7635-51: snapshot the STC offset (uiSoftwarePCROffset) when a 
    * picture is selected for display.
    */
   pstSelectedPicture->stPicParms.stDisplay.stDynamic.uiSoftwarePCROffset = hXdmPP->stDMConfig.uiSoftwarePCROffset;
   pstSelectedPicture->stPicParms.stDisplay.stDynamic.bSwPcrOffsetValid = true;

   /* SW7425-2255: snapshot the display offset (uiPTSOffset) when a 
    * picture is selected for display.
    */
   pstSelectedPicture->stPicParms.stDisplay.stDynamic.uiPtsOffset = hXdmPP->stDMConfig.uiPTSOffset;
   pstSelectedPicture->stPicParms.stDisplay.stDynamic.bPtsOffsetValid = true;

   /* If the PPB contained a coded PTS, save it for the application. */

   if ( true == pstSelectedPicture->pstUnifiedPicture->stPTS.bValid )
   {
      hXdmPP->stDMStatus.stCodedPTS.ui32RunningPTS = pstSelectedPicture->pstUnifiedPicture->stPTS.uiValue;
      hXdmPP->stDMStatus.stCodedPTS.ePTSType = BXDM_PictureProvider_PTSType_eCoded;
   }

   /* Update the public copy with the current GOP time code. */

   hXdmPP->stDMStatus.stGOPTimeCode = pstSelectedPicture->pstUnifiedPicture->stGOPTimeCode;

   BDBG_LEAVE( BXDM_PPQM_P_PromotePictureToDisplayStatus );

   return;
}

BXDM_PictureProvider_P_Picture_Context * BXDM_PPQM_P_GetFirstPictureContext( BXDM_PictureProvider_Handle hXdmPP )
{
   return &(hXdmPP->stDMState.stChannel.stPictureUnderEvaluation);
}

/*******************************************************************************
 **
 ** end "public" display queue management routines.
 **
 *******************************************************************************/
