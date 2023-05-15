/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxdm_pp_vtsm.c $
 * $brcm_Revision: Hydra_Software_Devel/12 $
 * $brcm_Date: 9/11/12 11:33a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/xdm/bxdm_pp_vtsm.c $
 * 
 * Hydra_Software_Devel/12   9/11/12 11:33a btosi
 * SW7405-4736: (Enhance XDM debug capabilities) standardized the instance
 * Id in the debug message
 * 
 * Hydra_Software_Devel/11   8/2/12 3:43p btosi
 * SW7405-4736: change the XDM uiInstanceId format to
 * uDecoderInstance.uiVDCRectangleNum
 * 
 * Hydra_Software_Devel/10   3/24/12 10:39p btosi
 * SW7425-2656: fix for frame advance
 * 
 * Hydra_Software_Devel/9   3/22/12 8:42a btosi
 * SW7425-2656: when paused during a flush, prevent the next picture from
 * being displayed until unpaused
 * 
 * Hydra_Software_Devel/8   2/16/12 9:29a btosi
 * SW7425-2255: bind the PTS offset to the picture when it is selected for
 * display
 * 
 * Hydra_Software_Devel/7   2/3/12 3:04p btosi
 * SW7425-1264: fixed a problem with virtual PTS values when playing in
 * reverse
 * 
 * Hydra_Software_Devel/6   10/27/11 10:22a btosi
 * SW7425-1264: interpolated the virtual STC/PTS in reverse when the SW
 * STC is running in reverse
 * 
 * Hydra_Software_Devel/5   12/21/10 4:24p delkert
 * SW7405-5043: Split STC stored in local state into two parts: STC from
 * decoder and STC after JTI adjustment
 *
 * Hydra_Software_Devel/4   9/23/10 3:09p btosi
 * SW7405-4736: add support for a XDM instance ID to help debug multi-
 * channel issues
 *
 * Hydra_Software_Devel/3   8/11/10 10:09a btosi
 * SW7405-4736: added PPB index to debug messages
 *
 * Hydra_Software_Devel/2   3/23/10 10:51a nilesh
 * SW7405-4072: Fixed issue with frame advance occurring continuously
 *
 * Hydra_Software_Devel/1   2/16/10 10:52a nilesh
 * SW7405-2993: Initial XDM version
 *
 ***************************************************************************/

#include "bstd.h"
#include "bdbg.h"                /* Dbglib */

#include "bxdm_pp.h"
#include "bxdm_pp_priv.h"
#include "bxdm_pp_qm.h"
#include "bxdm_pp_fp.h"
#include "bxdm_pp_vtsm.h"

BDBG_MODULE(BXDM_PPVTSM);

const char BXDM_PictureProvider_P_DISPMGR_VTSM_NODE[]="DMVTSM:\t""$brcm_Revision: Hydra_Software_Devel/12 $";

void BXDM_PPVTSM_P_VirtualStcSet(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState
   )
{
   BDBG_ENTER(BXDM_PictureProvider_P_VirtualStcSet);

   /* If we've selected the current picture using TSM, we set the
    * virtual STC (vSTC) to be equal to the actual STC so that we
    * can have smooth transitions from TSM --> VSYNC
    * mode. (e.g. when bVsyncModeOnPcrDiscontinuity is enabled) */

   /* PR52803: We don't want the vSTC to track the STC if it is
    * invalid */
   if (hXdmPP->stDMState.stChannel.stSelectedPicture.bValidated
       && (BXDM_PictureProvider_DisplayMode_eTSM == hXdmPP->stDMState.stChannel.stSelectedPicture.stPicParms.stTSM.stDynamic.eSelectionMode)
       && (true == hXdmPP->stDMConfig.bSTCValid)
      )
   {
      /* The actual STC = PCRoffset + STCsnapshot */
      BXDM_PPQM_P_GetCookedPcrOffset(
         hXdmPP,
         &(hXdmPP->stDMState.stChannel.stSelectedPicture),
         &(hXdmPP->stDMState.stDecode.stVTSM.stVirtualSTC.uiWhole)
         );

      hXdmPP->stDMState.stDecode.stVTSM.stVirtualSTC.uiWhole += pLocalState->uiAdjustedStc;
      hXdmPP->stDMState.stDecode.stVTSM.stVirtualSTC.uiFractional = 0;
   }

   BDBG_LEAVE(BXDM_PictureProvider_P_VirtualStcSet);
   return;
}

void BXDM_PPVTSM_P_VirtualStcIncrement(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState
   )
{
   bool bInterpolateInReverse;
   
   /* Virtual TSM - Setting the virtual STC (vSTC) */
   BDBG_ENTER(BXDM_PPVTSM_P_VirtualStcIncrement);

   /* PR52803: If the STC is validated or we're in VSYNC display mode,
    * then we can exit trick mode transition */
   if ( ( true == hXdmPP->stDMState.stDecode.stVTSM.bTrickModeTransition ) /* We're in a trick mode transition */
        && ( ( true == hXdmPP->stDMConfig.bSTCValid ) /* The STC is valid */
             || ( BXDM_PictureProvider_DisplayMode_eVirtualTSM == pLocalState->eDisplayMode ) /* We're in VSYNC display mode */
             || ( BXDM_PICTUREPROVIDER_NORMAL_PLAYBACK_RATE != pLocalState->uiSlowMotionRate ) /* We've entered into another trick mode */
           )
      )
   {
      BXVD_DBG_MSG(hXdmPP, ("%x:[%02x.xxx] Exiting trick mode transition (stc: %x)",
                                 hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                                 BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ),
                                 pLocalState->uiAdjustedStc));
      hXdmPP->stDMState.stDecode.stVTSM.bTrickModeTransition = false;
   }

   /* SW7425-1264: if the "physical" STC is running in reverse, interpolate the virtual STC in
    * reverse as well.
    */
   bInterpolateInReverse = ( true == hXdmPP->stDMConfig.stClockOverride.bEnableClockOverride );
   bInterpolateInReverse &= ( hXdmPP->stDMConfig.stClockOverride.iStcDelta < 0 );

   /* If in CRC mode
    * - use the delta PTS for the selected picture to increment the virtual STC
    * else
    * - use the delta STC to increment the virtual STC
    * (The delta STC is based on the display rate)
    */
   if ( true == hXdmPP->stDMConfig.bCRCMode )
   {
      if ( true == hXdmPP->stDMState.stChannel.stSelectedPicture.bValidated )
      {
         pLocalState->stSTCDelta = hXdmPP->stDMState.stChannel.stSelectedPicture.stPicParms.stTSM.stStatic.stPTSDelta;
      }
      else
      {
         pLocalState->stSTCDelta.uiWhole = 0;
         pLocalState->stSTCDelta.uiFractional = 0;
      }
   }

#if 0
   BKNI_Printf("[eClockRate = %d, eMonitorRefreshRate = %d] --> deltaSTC=(%d, %d)\n",
               pLocalState->eClockRate, hXdmPP->stDMState.stDecode.eMonitorRefreshRate,
               pLocalState->stSTCDelta.uiWhole,
               pLocalState->stSTCDelta.uiFractional);
#endif

   /* If the selected picture is not valid (i.e. on start-up) then we
    * always set PTS initialized to FALSE so that we reassign PTS
    * until we have a valid picture displayed */
   if ( !hXdmPP->stDMState.stChannel.stSelectedPicture.bValidated )
   {
      hXdmPP->stDMState.stDecode.stVTSM.bVirtualPTSInitialized = false;
      BXVD_DBG_MSG(hXdmPP,("%x:[%02x.xxx] vPTS Reset (Invalid)",
                                hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                                BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ) ));
   }

   /* Handle decoder trick modes */
   if ( true == hXdmPP->stDMState.stDecode.stVTSM.bTrickModeTransition )
   {
      /* PR52803: Don't increment vSTC when in a trick mode
       * transition */
      pLocalState->stEffectiveSTCDelta[BXDM_PictureProvider_DisplayMode_eVirtualTSM].uiWhole = 0;
      pLocalState->stEffectiveSTCDelta[BXDM_PictureProvider_DisplayMode_eVirtualTSM].uiFractional = 0;
   }
   else if ( pLocalState->uiSlowMotionRate )
   {
      /* We are not paused, so we can increment the vSTC on this
       * VSYNC */
      if ( BXDM_PICTUREPROVIDER_NORMAL_PLAYBACK_RATE != pLocalState->uiSlowMotionRate )
      {
         /* We're not in normal playback. Multiply the deltaSTC by
          * the playback rate, before adding it to the vSTC */
         BXDM_PPFP_P_FixPtFractionalMul(
            &pLocalState->stSTCDelta,
            pLocalState->uiSlowMotionRate,
            BXDM_PICTUREPROVIDER_NORMAL_PLAYBACK_RATE,
            &pLocalState->stSTCDelta);
      }

      pLocalState->stEffectiveSTCDelta[BXDM_PictureProvider_DisplayMode_eVirtualTSM] = pLocalState->stSTCDelta;

      /* Increment the vSTC by delta STC (which already has taken
       * slow motion into account) */

      if ( true == bInterpolateInReverse )
      {
         BXDM_PPFP_P_FixPtSub(
            &(hXdmPP->stDMState.stDecode.stVTSM.stVirtualSTC),
            &pLocalState->stSTCDelta,
            &(hXdmPP->stDMState.stDecode.stVTSM.stVirtualSTC)
            );
      }
      else
      {
         BXDM_PPFP_P_FixPtAdd(
            &(hXdmPP->stDMState.stDecode.stVTSM.stVirtualSTC),
            &pLocalState->stSTCDelta,
            &(hXdmPP->stDMState.stDecode.stVTSM.stVirtualSTC)
            );
      }

   }
   else
   {
      pLocalState->stEffectiveSTCDelta[BXDM_PictureProvider_DisplayMode_eVirtualTSM].uiWhole = 0;
      pLocalState->stEffectiveSTCDelta[BXDM_PictureProvider_DisplayMode_eVirtualTSM].uiFractional = 0;

      /* We're paused, so we only increment the STC if we need to
       * handle a frame advance request */

      /* PR51720: Frame advance is now performed by doing one field
       * advance per vsync until we get to the next frame */
      if ( BXDM_PictureProvider_FrameAdvanceMode_eFrameByField == hXdmPP->stDMConfig.eFrameAdvanceMode )
      {
         /* We ADD to the existing frame advance count (vs setting it)
          * to ensure we handle FRAME advance requests while in the
          * middle of honoring a previous one */
         if ( 0 == hXdmPP->stDMConfig.uiFrameAdvanceByFieldCount )
         {
            hXdmPP->stDMConfig.uiFrameAdvanceByFieldCount += hXdmPP->stDMState.stChannel.stSelectedPicture.stPicParms.stTSM.stStatic.uiNumElements - hXdmPP->stDMState.stChannel.stSelectedPicture.stPicParms.stDisplay.stDynamic.uiSelectedElement;
         }
         else
         {
            BXVD_DBG_WRN(hXdmPP,("%x:[%02x.xxx] Prior Frame Advance was not finished, yet.  Frame may not advance properly on 3:2 content", 
                                                   hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                                                   BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ) ));
            hXdmPP->stDMConfig.uiFrameAdvanceByFieldCount += hXdmPP->stDMState.stChannel.stSelectedPicture.stPicParms.stTSM.stStatic.uiNumElements;
         }
      }

      if ( hXdmPP->stDMConfig.eFrameAdvanceMode != BXDM_PictureProvider_FrameAdvanceMode_eOff )
      {
         hXdmPP->stDMState.stDecode.stVTSM.eFrameAdvanceMode = hXdmPP->stDMConfig.eFrameAdvanceMode;
         hXdmPP->stDMConfig.eFrameAdvanceMode = BXDM_PictureProvider_FrameAdvanceMode_eOff;

         if ( true == hXdmPP->stDMState.stChannel.bPostFlushDecode )
         {
            /* SW7425-2656:  If this is the first picture after a "flush" and the system is paused,
             * we want to hold off displaying the picture until the system is unpaused or a
             * "frame advance" command is issued.  The second case is handled here by clearing
             * "bPostFlushDecode" flag.  Clearing this flag prevents vPTS from being
             * reset in BXDM_PPVTSM_P_VirtualPtsInterpolate.
             */
            hXdmPP->stDMState.stChannel.bPostFlushDecode = false;

            BXVD_DBG_MSG(hXdmPP,("%x:[%02x.xxx] clear bPostFlushDecode due to frame advance",
                                hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                                BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP )
                                ));

         }

      }

      /* Handle frame advance */
      switch ( hXdmPP->stDMState.stDecode.stVTSM.eFrameAdvanceMode )
      {
         case BXDM_PictureProvider_FrameAdvanceMode_eField:
            /* We're doing a FIELD advance, so we need to increment
             * the vSTC by one field time, which is deltaPTS */
            BXVD_DBG_MSG(hXdmPP,("%x:[%02x.xxx] Frame Advance: Field", 
                                          hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                                          BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ) ));

            if ( true == bInterpolateInReverse )
            {
               BXDM_PPFP_P_FixPtSub(
                  &(hXdmPP->stDMState.stDecode.stVTSM.stVirtualSTC),
                  &(hXdmPP->stDMState.stChannel.stSelectedPicture.stPicParms.stTSM.stStatic.stPTSDelta),
                  &(hXdmPP->stDMState.stDecode.stVTSM.stVirtualSTC)
                  );
            }
            else
            {
               BXDM_PPFP_P_FixPtAdd(
                  &(hXdmPP->stDMState.stDecode.stVTSM.stVirtualSTC),
                  &(hXdmPP->stDMState.stChannel.stSelectedPicture.stPicParms.stTSM.stStatic.stPTSDelta),
                  &(hXdmPP->stDMState.stDecode.stVTSM.stVirtualSTC)
                  );
            }

            /* Frame advance is a one-shot request, so we reset it to
             * disabled when we've honored the request */
            hXdmPP->stDMState.stDecode.stVTSM.eFrameAdvanceMode = BXDM_PictureProvider_FrameAdvanceMode_eOff;

            break;

         case BXDM_PictureProvider_FrameAdvanceMode_eFrame:
            /* We're doing a FRAME advance, so we need to set the
             * vSTC to the vPTS of the NEXT PPB */
            BXVD_DBG_MSG(hXdmPP,("%x:[%02x.xxx] Frame Advance: Frame", 
                                       hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                                       BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ) ));
            hXdmPP->stDMState.stDecode.stVTSM.stVirtualSTC = hXdmPP->stDMState.stChannel.stSelectedPicture.stPicParms.stTSM.stStatic.stPTSOfNextPPB[BXDM_PictureProvider_P_PTSIndex_eVirtual];

            /* We need to account for the current field inversion
             * correct offset to ensure the next frame is actually
             * selected by TSM */
            if ( true == bInterpolateInReverse )
            {
               BXDM_PPFP_P_FixPtSub(
                  &(hXdmPP->stDMState.stDecode.stVTSM.stVirtualSTC),
                  &(hXdmPP->stDMState.stDecode.stFieldInversionCorrectionPTSOffset),
                  &(hXdmPP->stDMState.stDecode.stVTSM.stVirtualSTC)
                  );
            }
            else
            {
               BXDM_PPFP_P_FixPtAdd(
                  &(hXdmPP->stDMState.stDecode.stVTSM.stVirtualSTC),
                  &(hXdmPP->stDMState.stDecode.stFieldInversionCorrectionPTSOffset),
                  &(hXdmPP->stDMState.stDecode.stVTSM.stVirtualSTC)
                  );
            }

            /* Frame advance is a one-shot request, so we reset it to
             * disabled when we've honored the request */
            hXdmPP->stDMState.stDecode.stVTSM.eFrameAdvanceMode = BXDM_PictureProvider_FrameAdvanceMode_eOff;

            break;

         case BXDM_PictureProvider_FrameAdvanceMode_eFrameByField:
            /* We're doing a FRAME advance by fields, so we need to do
             * field advances until we get to the next frame */
            BXVD_DBG_MSG(hXdmPP,("%x:[%02x.xxx] Frame Advance: Frame by Field[%d]",
                                      hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                                      BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ), 
                                      hXdmPP->stDMConfig.uiFrameAdvanceByFieldCount ));

            if ( hXdmPP->stDMConfig.uiFrameAdvanceByFieldCount > 0 )
            {
               if ( true == bInterpolateInReverse )
               {
                  BXDM_PPFP_P_FixPtSub(
                     &(hXdmPP->stDMState.stDecode.stVTSM.stVirtualSTC),
                     &(hXdmPP->stDMState.stChannel.stSelectedPicture.stPicParms.stTSM.stStatic.stPTSDelta),
                     &(hXdmPP->stDMState.stDecode.stVTSM.stVirtualSTC)
                     );
               }
               else
               {
                  BXDM_PPFP_P_FixPtAdd(
                     &(hXdmPP->stDMState.stDecode.stVTSM.stVirtualSTC),
                     &(hXdmPP->stDMState.stChannel.stSelectedPicture.stPicParms.stTSM.stStatic.stPTSDelta),
                     &(hXdmPP->stDMState.stDecode.stVTSM.stVirtualSTC)
                     );
               }

               hXdmPP->stDMConfig.uiFrameAdvanceByFieldCount--;
            }

            if ( 0 == hXdmPP->stDMConfig.uiFrameAdvanceByFieldCount )
            {
               hXdmPP->stDMState.stDecode.stVTSM.eFrameAdvanceMode = BXDM_PictureProvider_FrameAdvanceMode_eOff;
            }
            break;

         default:
            break;
      }
   }

   BDBG_LEAVE(BXDM_PPVTSM_P_VirtualStcIncrement);
   return;
}

void BXDM_PPVTSM_P_VirtualPtsInterpolate(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState,
   BXDM_PictureProvider_P_Picture_Context* pstPicture,
   BXDM_PictureProvider_P_Picture_Context* pstPrevPicture,
   BXDM_PictureProvider_P_Picture_Context* pstSelectedPicture
   )
{
   BXDM_PPFP_P_DataType stVirtualPTSTemp;
   BXDM_PPFP_P_DataType stVirtualPTSOfNextPPB;
   uint32_t i;
   bool bInterpolateInReverse;

   BDBG_ENTER(BXDM_PPVTSM_P_VirtualPtsInterpolate);

   BSTD_UNUSED( pLocalState );

   /* Virtual TSM */

   /* Re-initialize vPTS if the currently selected picture did not
    * pass TSM */

   /* PR52803: We don't want to do a vPTS reset if we're in a trick
    * mode transition to prevent the next picture from being displayed
    * too early if entering back into a trick mode.
    * E.g. pause->play->pause */
   if ( ( BXDM_PictureProvider_TSMResult_ePass != pstPrevPicture->stPicParms.stTSM.stDynamic.eTsmResult )
        && ( false == hXdmPP->stDMState.stDecode.stVTSM.bTrickModeTransition )
      )
   {
      hXdmPP->stDMState.stDecode.stVTSM.bVirtualPTSInitialized = false;
      BXVD_DBG_MSG(hXdmPP,("%x:[%02x.%03x] vPTS Reset (TSM Failure: %d)",
                                hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                                BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ),
                                pstPicture->stPicParms.uiPPBIndex & 0xFFF,
                                pstPrevPicture->stPicParms.stTSM.stDynamic.eTsmResult));
   }

   /* We interpolate the vPTS from the previous PPB */
   if ( (pstPicture->stPicParms.stTSM.stDynamic.eSelectionMode == BXDM_PictureProvider_DisplayMode_eTSM)
        && ( false == pstPrevPicture->bValidated ) )
   {
      /* If we're in TSM mode and the previous picture is invalid, we
       * set the vPTS of the next PPB based on the CURRENT picture's
       * ACTUAL interpolated PTS.  This causes the vPTS to track
       * actual PTS when in TSM mode so that we can have a seamless
       * transition from TSM --> VSYNC mode */
      BXDM_PPQM_P_GetPredictedPtsWithFrac(pstPicture,
                                          BXDM_PictureProvider_P_PTSIndex_eActual,
                                          &stVirtualPTSOfNextPPB);

      /* We need to add the JRC jitter offset so that the vPTS tracks the de-jittered PTS values */
      stVirtualPTSOfNextPPB.uiWhole += pstPicture->stPicParms.stTSM.stStatic.iPTSJitterCorrection;

      /* Since we're setting the vPTS based on an actual PTS, we
       * indicate that the vPTS has been initialized */
      hXdmPP->stDMState.stDecode.stVTSM.bVirtualPTSInitialized= true;
   }
   else
   {
      if (pstPicture->stPicParms.stTSM.stDynamic.eSelectionMode == BXDM_PictureProvider_DisplayMode_eTSM)
      {
         /* If we're in TSM mode, we set the vPTS of the next PPB based
          * on the previous picture's ACTUAL interpolated PPB */
         BXDM_PPQM_P_GetPredictedPtsWithFrac(pstPrevPicture,
                                             BXDM_PictureProvider_P_PTSIndex_eActual,
                                             &stVirtualPTSTemp);

         /* We need to add the JRC jitter offset so that the vPTS tracks the de-jittered PTS values */
         stVirtualPTSTemp.uiWhole += pstPrevPicture->stPicParms.stTSM.stStatic.iPTSJitterCorrection;

         hXdmPP->stDMState.stDecode.stVTSM.bVirtualPTSInitialized = true;
      }
      else
      {
         /* If we're in VSYNC mode, we set the vPTS of the next PPB based
          * on the previous picture's VIRTUAL interpolated PPB */
         BXDM_PPQM_P_GetPredictedPtsWithFrac(pstPrevPicture,
                                             BXDM_PictureProvider_P_PTSIndex_eVirtual,
                                             &stVirtualPTSTemp);

         /* Handle vPTS skew */
         /* Detect and correct for PTS skew in vTSM mode.  Skew can occur
          * when the delivery queue runs dry for one or more vsyncs and
          * the interpolated PTS for the NEXT PPB is no longer accurate.
          * The queue can run dry for multiple reasons...errors in the
          * stream, a stream wrap, etc. In such a case, without this
          * logic, subsequent PPBs would always have been evaluated as
          * LATE because the interpolated vPTS would be far behind the
          * vSTC */

         /* PR52803: We don't want to do vPTS skew detection when in a
          * trick mode transition to prevent the next picture from
          * being displayed too early if entering back into a trick
          * mode.  E.g. pause->play->pause */
         if ( ( hXdmPP->stDMState.stDecode.stVTSM.bVirtualPTSInitialized )
              && ( false == hXdmPP->stDMState.stDecode.stVTSM.bTrickModeTransition )
            )
         {
            uint32_t uiStcPtsDifference = 0;
            /* PR57049: We use the dStcPts when last displayed (vs the evaluated) one
             * because the evaluated one has been updated already before we started
             * evaluating this picture */
            if ( pstSelectedPicture->stPicParms.stTSM.stDynamic.iStcPtsDifferenceDisplayed > 0 )
            {
               uiStcPtsDifference = (uint32_t) pstSelectedPicture->stPicParms.stTSM.stDynamic.iStcPtsDifferenceDisplayed;
            }

            if ( uiStcPtsDifference >= ( pstSelectedPicture->stPicParms.stTSM.stStatic.stPTSDelta.uiWhole +
                                         ( pstSelectedPicture->stPicParms.stTSM.stStatic.stPTSDelta.uiFractional ? 1 : 0 ) ) )
            {
               hXdmPP->stDMState.stDecode.stVTSM.bVirtualPTSInitialized = false;
               BXVD_DBG_MSG(hXdmPP,("%x:[%02x.%03x] vPTS Reset (vPTS Skew Detected: %d)",
                                         hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                                         BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ),
                                         pstPicture->stPicParms.uiPPBIndex & 0xFFF,
                                         uiStcPtsDifference));
            }
         }

         /* We need to override the predicted virtualPTS for this PPB */
         if ( false == hXdmPP->stDMState.stDecode.stVTSM.bVirtualPTSInitialized)
         {
            /* We've just entered into VSYNC mode, so we need to set the
             * vPTS to a default starting value equal to the starting
             * vSTC value.  Also, When in vTSM mode, make sure we don't
             * assign a stale PTS value for this PPB, so we need to
             * assign this PPB the larger of the current STC or the
             * interpolated PTS */

            stVirtualPTSTemp = hXdmPP->stDMState.stDecode.stVTSM.stVirtualSTC;

            /* SW7425-2656:  If this is the first picture after a "flush" and the system is paused, 
             * we don't want to display this picture until the system is unpaused or a "frame advance"
             * command is issued.  This is achieved by setting the vPTS to be larger than the vSTC.  
             * The value doesn't matter since the vPTS will be reset again once the system is unpaused.
             *
             * "hold last picture" is implicit with the "flush"; by forcing this picture to be a 
             * little early, the picture that was being displayed will continue to be displayed.
             *
             * In contrast to the behavior after a "flush", if the system is paused prior to calling BXVD_StartDecode 
             * in the normal manner, the first picture will be displayed. 
             */
            if ( ( true == hXdmPP->stDMState.stChannel.bPostFlushDecode )
                 && ( 0 == pLocalState->uiSlowMotionRate )
                 && ( 1 == pstPicture->stPicParms.uiPPBIndex )
               )
            {
               /* The amount added doesn't matter. */
               stVirtualPTSTemp.uiWhole += 10;

               BXVD_DBG_MSG(hXdmPP,("%x:[%02x.%03x] set vPTS for 1st picture with system paused: vStc:%08x vPTS:%08x",
                                         hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                                         BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ),
                                         pstPicture->stPicParms.uiPPBIndex & 0xFFF,
                                         hXdmPP->stDMState.stDecode.stVTSM.stVirtualSTC.uiWhole,
                                         stVirtualPTSTemp.uiWhole
                                         ));
            }
            else
            {
               uint32_t uiPtsOffset;
         
               /* SW7425-2255: once a picture has been selected for display, use the 
                * offset bound to the picture.
                */
               BXDM_PPQM_P_GetPtsOffset( hXdmPP, pstPicture, &uiPtsOffset );

               /* Subtract the PTS offset */
               stVirtualPTSTemp.uiWhole -= uiPtsOffset;

               /* Subtract the field inversion correction offset */
               BXDM_PPFP_P_FixPtSub(
                  &stVirtualPTSTemp,
                  &hXdmPP->stDMState.stDecode.stFieldInversionCorrectionPTSOffset,
                  &stVirtualPTSTemp);
            }

            hXdmPP->stDMState.stDecode.stVTSM.bVirtualPTSInitialized = true;

         }  /* end of if ( false == hXdmPP->stDMState.stDecode.stVTSM.bVirtualPTSInitialized ) */

      }

      /* SW7425-1264: if the "physical" STC is running in reverse, interpolate the virtual PTS's in
       * reverse as well.
       */
      bInterpolateInReverse = ( true == hXdmPP->stDMConfig.stClockOverride.bEnableClockOverride );
      bInterpolateInReverse &= ( hXdmPP->stDMConfig.stClockOverride.iStcDelta < 0 );

      /* Set the PTS for each element in the PPB */
      stVirtualPTSOfNextPPB = stVirtualPTSTemp;
      for (i = 0; i < pstPicture->stPicParms.stTSM.stStatic.uiNumElements; i++)
      {

         BXDM_PPQM_P_SetPtsWithFrac(pstPicture, BXDM_PictureProvider_P_PTSIndex_eVirtual, i, stVirtualPTSTemp);

#if 0
         BKNI_Printf("vPTS[%d]: %08x\n", i, stVirtualPTSTemp.uiWhole);
#endif

         if ( true == bInterpolateInReverse )
         {
            BXDM_PPFP_P_FixPtSub(
               &stVirtualPTSTemp,
               &(pstPicture->stPicParms.stTSM.stStatic.stPTSDelta),
               &stVirtualPTSTemp
               );

            /* Calculate the next PPB's virtual PTS */
            BXDM_PPFP_P_FixPtSub(
               &stVirtualPTSOfNextPPB,
               &(pstPicture->stPicParms.stTSM.stStatic.stPTSDelta),
               &stVirtualPTSOfNextPPB
               );
         }
         else
         {
            BXDM_PPFP_P_FixPtAdd(
               &stVirtualPTSTemp,
               &(pstPicture->stPicParms.stTSM.stStatic.stPTSDelta),
               &stVirtualPTSTemp
               );

            /* Calculate the next PPB's virtual PTS */
            BXDM_PPFP_P_FixPtAdd(
               &stVirtualPTSOfNextPPB,
               &(pstPicture->stPicParms.stTSM.stStatic.stPTSDelta),
               &stVirtualPTSOfNextPPB
               );
         }

      }

#if 0
      BKNI_Printf("vPTSPredicted: %d\n", stVirtualPTSOfNextPPB.uiWhole);
#endif
   }

   BXDM_PPQM_P_SetPredictedPtsWithFrac(pstPicture, BXDM_PictureProvider_P_PTSIndex_eVirtual, stVirtualPTSOfNextPPB);

   BDBG_LEAVE(BXDM_PPVTSM_P_VirtualPtsInterpolate);
   return;
} /* end of BXDM_PPVTSM_P_VirtualPtsInterpolate() */

void BXDM_PPVTSM_P_VirtualStcGet(
   const BXDM_PictureProvider_Handle hXdmPP,
   uint32_t* puiStc
   )
{
   bool bInterpolateInReverse;

   *puiStc = hXdmPP->stDMState.stDecode.stVTSM.stVirtualSTC.uiWhole;

   /* SW7425-1264: only round the virtual STC up if the "physical" STC is 
    * running in the forward direction.
    */
   bInterpolateInReverse = ( true == hXdmPP->stDMConfig.stClockOverride.bEnableClockOverride );
   bInterpolateInReverse &= ( hXdmPP->stDMConfig.stClockOverride.iStcDelta < 0 );

   if ( 0 != hXdmPP->stDMState.stDecode.stVTSM.stVirtualSTC.uiFractional
        && false == bInterpolateInReverse 
      )
       
   {
      (*puiStc)++;
   }

   return;
} /* end of BXDM_PPVTSM_P_VirtualStcGet() */

void BXDM_PPVTSM_P_ClipTimeTrickModeTransitionHandler(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState * pLocalState
   )
{
   BSTD_UNUSED(pLocalState);

   BXVD_DBG_MSG(hXdmPP, ("%x:[%02x.xxx] Entering trick mode transition (stc: %x)",
                              hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                              BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ),
                              pLocalState->uiAdjustedStc));
   hXdmPP->stDMState.stDecode.stVTSM.bTrickModeTransition = true;
}
