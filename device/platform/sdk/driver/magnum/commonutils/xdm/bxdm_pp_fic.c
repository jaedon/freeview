/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxdm_pp_fic.c $
 * $brcm_Revision: Hydra_Software_Devel/16 $
 * $brcm_Date: 9/11/12 10:01a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/xdm/bxdm_pp_fic.c $
 * 
 * Hydra_Software_Devel/16   9/11/12 10:01a btosi
 * SW7405-4736: (Enhance XDM debug capabilities) standardized the instance
 * Id in the debug messages
 * 
 * Hydra_Software_Devel/15   8/15/12 9:24a btosi
 * SW7405-5683:  Disable the FIC and force eSingleField mode when
 * delivering interlaced content to a display running at <= 30 Hz.
 * 
 * Hydra_Software_Devel/14   8/2/12 3:43p btosi
 * SW7405-4736: change the XDM uiInstanceId format to
 * uDecoderInstance.uiVDCRectangleNum
 * 
 * Hydra_Software_Devel/13   5/4/12 3:35p btosi
 * SW7425-2915: fixed issue with 2nd slot delay at startup and added
 * additional debug support for NRT mode
 * 
 * Hydra_Software_Devel/12   10/3/11 2:19p btosi
 * SW7425-1264:  support for a SW STC internal to XDM
 * 
 * Hydra_Software_Devel/11   10/1/10 2:50p btosi
 * SWDEPRECATED-1003: added support for bTreatAsSingleElement
 * 
 * Hydra_Software_Devel/10   9/23/10 3:09p btosi
 * SW7405-4736: add support for a XDM instance ID to help debug multi-
 * channel issues
 * 
 * Hydra_Software_Devel/9   9/21/10 3:06p btosi
 * SWBLURAY-22573: Turn off the FIC logic for MVC streams
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
 * Hydra_Software_Devel/6   8/10/10 4:16p btosi
 * SW7405-4736: added PPB index to debug messages
 * 
 * Hydra_Software_Devel/5   8/4/10 1:22p btosi
 * SWBLURAY-21565: to aid debug, added uiPPBIndex to debug messages
 * 
 * Hydra_Software_Devel/4   7/16/10 9:41a delkert
 * SW7405-4632: Replace missing single field display mode check in
 * IgnoreCadenceMatch calculation
 *
 * Hydra_Software_Devel/3   3/25/10 3:55p nilesh
 * SW7550-349: Fixed issue with STC pause/frame advance from occasionally
 * not occurring
 *
 * Hydra_Software_Devel/2   3/24/10 3:06p btosi
 * SW7340-116: only perform FIC if the CDT logic set the scan mode to
 * interlaced.
 *
 * Hydra_Software_Devel/1   2/16/10 10:51a nilesh
 * SW7405-2993: Initial XDM version
 *
 ***************************************************************************/

#include "bstd.h"
#include "bdbg.h"                /* Dbglib */

#include "bxdm_pp.h"
#include "bxdm_pp_priv.h"
#include "bxdm_pp_fic.h"
#include "bxdm_pp_qm.h"

BDBG_MODULE(BXDM_PPFIC); /* Register software module with debug interface */

const char BXDM_PictureProvider_P_DISPMGR_FIC_NODE[]="DMFIC:\t""$brcm_Revision: Hydra_Software_Devel/16 $";

BERR_Code BXDM_PPFIC_P_CalculateIgnoreCadenceMatch(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState,
   BXDM_PictureProvider_P_Picture_Context* pstPicture
   )
{
   BSTD_UNUSED(pLocalState);

   /* PR49634: In order to prevent VDC's sync-slipped multi-buffer
    * path from bouncing caused by regular field inversions, we
    * tell VDC to ignore cadence match if ALL of the following are
    * true:
    *
    *  1) the content is not frame AND
    *  2) We don't expect source -> display polarity match
    *     a) FIC and progressive override are not enabled OR
    *     b) we're in a single field display mode OR
    *     c) we've stopped decode
    *     d) we're holding the last picture and/or previewing the first picture
    *     e) we're frozen
    *     f) the deltaStcPts is negative (E.g. PTSOffset has increased causing repeat of current picture)
    */
   if ( ( BAVC_Polarity_eFrame != pstPicture->stPicParms.stDisplay.stDynamic.eSourcePolarity ) /* content is truly interlaced */
        /* we don't expect polarity match */
        && ( ( ( false == pstPicture->stPicParms.stDisplay.stDynamic.bFICEnabled ) /* FIC is not enabled */
               && ( BXDM_PictureProvider_P_RepeatMode_eField == pstPicture->stPicParms.stDisplay.stDynamic.eRateConversionRepeatMode ) ) /* Progressive Override not enabled */
             || ( BXDM_PictureProvider_P_DecodeState_eStopped == hXdmPP->stDMState.stChannel.eDecodeState )
             || ( true == pstPicture->stPicParms.stDisplay.stDynamic.bHolding )
             || ( hXdmPP->stDMConfig.bFreeze )
             || ( ( ( pstPicture->stPicParms.stTSM.stDynamic.iStcPtsDifferenceEvaluated > (int32_t) pstPicture->stPicParms.stTSM.stStatic.stPTSDelta.uiWhole )
                    || ( pstPicture->stPicParms.stTSM.stDynamic.iStcPtsDifferenceEvaluated < 0 ) )
                  && ( BXDM_PictureProvider_P_RepeatMode_eField == pstPicture->stPicParms.stDisplay.stDynamic.eTrickPlayRepeatMode ) ) /* Progressive Override not enabled */
             || ( BXDM_PictureProvider_DisplayFieldMode_eBothField != pstPicture->stPicParms.stDisplay.stDynamic.eDisplayFieldMode)    /* Single-field display mode */
           )
      )
   {
      BXVD_DBG_MSG(hXdmPP,("%x:[%02x.%03x] VDC Cadence Matching Disabled", 
                                 hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                                 BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ),
                                 pstPicture->stPicParms.uiPPBIndex & 0xFFF));
      pstPicture->stPicParms.stDisplay.stDynamic.bIgnoreCadenceMatch = true;
   }

   return BERR_SUCCESS;
}

/* PR55439: It is better to prevent inversion than to maintain tight TSM.
 * In light of this, if we have uncorrectable field inversion caused by
 * stream errors (e.g. decoder errors, stream corruption, PCR discontinuities, etc.),
 * we try to deliver the previous element if it was never displayed.  If the
 * PPB was already displayed, then we repeat the previously displayed element
 * instead of displaying a new one, since the MAD can at least do a better
 * job with preventing jitter if an element is repeated.
 */
static void BXDM_PPFIC_S_SelectPreviousElement(BXDM_PictureProvider_Handle hXdmPP,
                                               BXDM_PictureProvider_P_LocalState* pLocalState,
                                               BXDM_PictureProvider_P_Picture_Context* pstPicture)
{
   BSTD_UNUSED(pLocalState);

   if ( pstPicture->stPicParms.stDisplay.stDynamic.uiSelectedElement )
   {
      if ( 0 == pstPicture->stPicParms.stDisplay.stDynamic.stDisplayDuration.uiWhole )
      {
         /* If this PPB has not been displayed yet, then we can
          * just select the previous element for display
          */
         pstPicture->stPicParms.stDisplay.stDynamic.uiSelectedElement--;
         BXDM_PPDBG_P_SelectionLog( hXdmPP, BXDM_PPDBG_Selection_PolarityOverride_eSelectPrevious );
      }
      else
      {
         /* We've already started displaying this PPB, so it's better
          * to repeat the same field we sent before
          */
         pstPicture->stPicParms.stDisplay.stDynamic.uiSelectedElement = hXdmPP->stDMState.stChannel.stPreviousPicture.stPicParms.stDisplay.stDynamic.uiSelectedElement;
         BXDM_PPDBG_P_SelectionLog( hXdmPP, BXDM_PPDBG_Selection_PolarityOverride_eRepeatPrevious );
      }
   }
}

BERR_Code BXDM_PPFIC_P_CalculateFieldInversionCorrectionOffset(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState,
   BXDM_PictureProvider_P_Picture_Context* pstPicture
   )
{
   BAVC_Polarity eTargetSourcePolarity;
   bool bTargetSourcePolarityIsStatic = false;

   /* PR51644: If we're on an interlaced display, we want the target
    * polarity to be the same as the vsync polarity.  However, if
    * we're on a progressive display, then we want to try guarantee
    * alternating T/B cadence to keep the MAD happy */
   if ( BAVC_Polarity_eFrame == pLocalState->eVsyncPolarity )
   {
      if ( true == pLocalState->bDisplayInterruptCountValid )
      {
         /* SW7405-3085: Use the display interrupt count to determine
          * the target polarity so that we are synced across all mosaic
          * channels on the same display interrupt */
         switch ( pLocalState->uiDisplayInterruptCount & 0x1 )
         {
            case 0:
               eTargetSourcePolarity = BAVC_Polarity_eBotField;
               break;

            case 1:
            default:
               eTargetSourcePolarity = BAVC_Polarity_eTopField;
               break;
         }
      }
      else
      {
         if ( ( false == hXdmPP->stDMState.stDecode.bFirstPPBHasBeenDisplayed )
              && ( false == hXdmPP->stDMState.stChannel.stSelectedPicture.stPicParms.stDisplay.stDynamic.bHolding )
            )
         {
            /* We don't want to start FIC if we haven't displayed the
             * first picture, yet */
            return BERR_SUCCESS;
         }

         switch ( hXdmPP->stDMState.stChannel.ePrevSourcePolarity )
         {
            case BAVC_Polarity_eTopField:
               eTargetSourcePolarity = BAVC_Polarity_eBotField;
               break;

            case BAVC_Polarity_eBotField:
            case BAVC_Polarity_eFrame:
            default:
               eTargetSourcePolarity = BAVC_Polarity_eTopField;
               break;
         }

         bTargetSourcePolarityIsStatic = true;
      }
   }
   else
   {
      eTargetSourcePolarity = pLocalState->eVsyncPolarity;
   }

   /* Determine if FIC should be enabled or not. We only perform FIC
    * if:
    *  1) we're not in decoder trick mode AND
    *  2) we're not frozen AND
    *  3) we're not in an STC trick mode
    *  4) We're doing an FIC compatible rate conversion
    *  5) The source is interlaced
    *  6) We're not repeating the element (we only perform FIC on new elements)
    *  7) The TSM result is PASS
    *  8) The dStcPtsDiff is within a dPTS (we've achieved tight TSM)
    *  9) The source is not coded progressive AND
    *  10) The source polarity doesn't match the vsync polarity AND
    *  11) CRC mode is not enabled
    */

   /* SW7340-116: only perform FIC if the CDT logic set the scan mode to interlaced. */

   pstPicture->stPicParms.stDisplay.stDynamic.bFICEnabled = false;
   if ( ( BXDM_PictureProvider_P_ScanMode_eInterlaced == pstPicture->stPicParms.stDisplay.stStatic.eScanMode )
         && ( 0 != pLocalState->stEffectiveSTCDelta[pstPicture->stPicParms.stTSM.stDynamic.eSelectionMode].uiWhole ) )
   {
      bool bTrickMode=false;

      /* PR55297: We want to calculate the effective PTS after a
       * trick mode to determine if we need to enable FIC.
       * E.g. 50->60Hz conversion normally does not have FIC enabled,
       * however if in 1.2x or faster trick mode, we DO want to enable
       * FIC to prevent inversion.
       */
      bool bFICRequired = false;

      /* SW7550-349: When in STC pause, the FIC logic is expected to be disabled.
       * However on content that is source/display rate matched, when a
       * frame advance occurs, via the STC being set to GetNextPTSInfo.ui32EffectivePTS,
       * XDM doesn't know if normal playback has resumed or if a frame advance occurred
       * because the stEffectiveSTCDelta is identical for a single vsync.  This caused the
       * FIC logic to be enabled on the same vsync that the STC frame advance occurred,
       * which could cause the "effectivePTS" value to change if the FICOffset happened to
       * change.  This could prevent the frame advance from actually occurring.  To prevent
       * this, the following logic turns off FIC if:
       *    1) The picture's selection mode is TSM
       *    2) The STC Trick Mode (which has hysteresis) is pause
       */
      bTrickMode = ( ( BXDM_PictureProvider_P_STCTrickMode_ePause == pLocalState->eSTCTrickMode )
                     && ( BXDM_PictureProvider_DisplayMode_eTSM == pstPicture->stPicParms.stTSM.stDynamic.eSelectionMode ));


      /* SWDEPRECATED-1003: turns off the FIC logic if the middleware/application has specified
       * that the system is in a trick mode.  For example, I or IP only mode.  This is not required for 
       * STC trick modes since they are detected automatically.  The default setting for "eTrickMode"
       * is "BXDM_PictureProvider_TrickMode_eAuto".
       */
      bTrickMode |= ( !( BXDM_PictureProvider_TrickMode_eAuto == hXdmPP->stDMConfig.eTrickMode )
                     && !( BXDM_PictureProvider_TrickMode_eNormal == hXdmPP->stDMConfig.eTrickMode ) );



      /* SWBLURAY-22573: Turn off the FIC logic for MVC streams, the content will always be progress 
       * (well at least for the near term) and the interrupt polarity will always be frame.
       * However the logic can get fooled due to "overloading".  The CDT logic could set "eScanMode" 
       * to interlaced even though the content is progressive. The interrupt polarity will be " eTopField" 
       * for base/left and " eBottomField" for dependent/right.
       */
      bTrickMode |= ( BAVC_VideoCompressionStd_eMVC == hXdmPP->stDMConfig.eProtocol );

      /* SWDEPRECATED-1003: turn off FIC if forcing everthing to be treated as FrameX1. */
      bTrickMode |= ( true == hXdmPP->stDMConfig.stFrameRateOverride.bTreatAsSingleElement );

      /* SW7425-1264: If using the SW STC, disable FIC.  We should be able to use FIC
       * if the display rate is 1:1, but the assumption is that the SW STC will be used for
       * fast rewind.
       */
      bTrickMode |= ( true == hXdmPP->stDMConfig.stClockOverride.bEnableClockOverride );      

      /* SW7405-5683: Delivering interlaced content to a display running at <= 30 Hz is not supported by the system. 
       * The most common case would be 1080i60 to a 30p display. To get the best possible results, the FIC is disabled 
       * and the display mode is forced to eSingleField for interlaced content when the monitor rate is <= 30 Hz.
       */
      bTrickMode |= ( true == pstPicture->stPicParms.stDisplay.stDynamic.bForceSingleFieldMode );

      if ( true == bTrickMode )
      {
         bFICRequired = false;
      }
      else
      {
         BXDM_PPFP_P_DataType stDisplayRate;

         BXDM_PPFP_P_FixPtDiv(
                  &pstPicture->stPicParms.stTSM.stStatic.stPTSDelta,
                  pLocalState->stEffectiveSTCDelta[pstPicture->stPicParms.stTSM.stDynamic.eSelectionMode].uiWhole,
                  &stDisplayRate);

         /* We always require FIC if the source -> display rate is 1:1 */
         bFICRequired = ( ( stDisplayRate.uiWhole == 1 )
                          && ( stDisplayRate.uiFractional <= ( BXDM_PictureProvider_P_FixedPoint_FractionalOverflow / 100 ) ) );

         switch ( hXdmPP->stDMConfig.ePictureDropMode )
         {
            case BXDM_PictureProvider_PictureDropMode_eFrame:
               /* In frame drop mode, we always want to enable FIC if the
                * STC is moving faster or same as the PTS values.
                * I.e. there's 1 or more fields delivered per vsync.
                */
               bFICRequired |= ( stDisplayRate.uiWhole == 0 );
               break;

            case BXDM_PictureProvider_PictureDropMode_eField:
            default:
               /* In field drop mode, we want to enable FIC only if the
                * STC is moving at the same rate as the PTS values.
                * I.e there's 1 field delivered per vsync.
                *
                * We also treat close rates the same, so we have
                * a +/- 1% window around 1
                *
                * E.g. 29.97 -> 30 and 30->29.97 are considered 1:1
                */
               bFICRequired |= ( ( stDisplayRate.uiWhole == 0 )
                                 && ( stDisplayRate.uiFractional >= ( BXDM_PictureProvider_P_FixedPoint_FractionalOverflow - ( BXDM_PictureProvider_P_FixedPoint_FractionalOverflow / 100 ) ) ) );
               break;
         }
      }
#if 0
      BKNI_Printf("bFICRequired = %d (display rate = %d.%d = (%d.%d / %d)\n",
                  bFICRequired,
                  stDisplayRate.uiWhole,
                  stDisplayRate.uiFractional,
                  pstPicture->stPicParms.stPTSDelta.uiWhole,
                  pstPicture->stPicParms.stPTSDelta.uiFractional,
                  pLocalState->stEffectiveSTCDelta[pLocalState->eDisplayMode].uiWhole);
#endif

      if ( ( true == bFICRequired )
           && ( false == hXdmPP->stDMConfig.bFreeze ) /* We're not in a freeze state */
           && ( false == hXdmPP->stDMConfig.bCRCMode )
         )
      {
         pstPicture->stPicParms.stDisplay.stDynamic.bFICEnabled = true;

         if ( ( ( false == pstPicture->stPicParms.stDisplay.stDynamic.bPPBRepeated )
                || ( ( true == pstPicture->stPicParms.stDisplay.stDynamic.bPPBRepeated )
                     && ( true == hXdmPP->stDMState.stChannel.stPreviousPictureUnderEvaluation.bValidated )
                     && ( hXdmPP->stDMState.stChannel.stPreviousPicture.stPicParms.stDisplay.stDynamic.uiSelectedElement != pstPicture->stPicParms.stDisplay.stDynamic.uiSelectedElement ) /* We're repeating the element */ ) )
              && ( BXDM_PictureProvider_TSMResult_eWait != pstPicture->stPicParms.stTSM.stDynamic.eTsmResult )
              && ( BXDM_PictureProvider_TSMResult_eTooEarly != pstPicture->stPicParms.stTSM.stDynamic.eTsmResult )
              && ( BXDM_PictureProvider_TSMResult_eDrop != pstPicture->stPicParms.stTSM.stDynamic.eTsmResult )
              && ( pstPicture->stPicParms.stDisplay.stDynamic.eSourcePolarity != eTargetSourcePolarity )

            )
         {
            /* PR51134: To prevent jitter, we need to reset the FIC offset if
             * the queue runs dry.  This is to prevent jitter on the display */
            if ( ( pstPicture->stPicParms.stTSM.stDynamic.iStcPtsDifferenceEvaluated > (int32_t) pstPicture->stPicParms.stTSM.stStatic.stPTSDelta.uiWhole )
                 && ( false == bTargetSourcePolarityIsStatic )
               )
            {
               BXDM_PPFIC_P_ResetFieldInversionCorrectionOffset(
                  hXdmPP,
                  pLocalState);
            }

            if ( (0 == hXdmPP->stDMState.stDecode.stFieldInversionCorrectionPTSOffset.uiWhole )
                 && (0 == hXdmPP->stDMState.stDecode.stFieldInversionCorrectionPTSOffset.uiFractional )
               )
            {
               /* We have a first pass slot, so offset the PTS by
                * deltaPTS so that next time we pick the same
                * element which should correct field inversion */
               hXdmPP->stDMState.stDecode.stFieldInversionCorrectionPTSOffset = pstPicture->stPicParms.stTSM.stStatic.stPTSDelta;
               pLocalState->bFICChanged = true;

               BXDM_PPDBG_P_SelectionLog( hXdmPP, BXDM_PPDBG_Selection_PolarityOverride_e1stSlotRepeat );
               if ( pstPicture->stPicParms.stDisplay.stDynamic.uiSelectedElement )
               {
                  BXDM_PPFIC_S_SelectPreviousElement(hXdmPP,
                                                     pLocalState,
                                                     pstPicture);

                  BXVD_DBG_MSG(hXdmPP,("%x:[%02x.%03x] Polarity Override: 1st slot, selected element %d", 
                                             hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                                             BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ),
                                             pstPicture->stPicParms.uiPPBIndex & 0xFFF,
                                             pstPicture->stPicParms.stDisplay.stDynamic.uiSelectedElement));
               }
               else
               {
                  BXVD_DBG_MSG(hXdmPP,("%x:[%02x.%03x] Polarity Override: 1st slot, TSM eWait", 
                                             hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                                             BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ),
                                             pstPicture->stPicParms.uiPPBIndex & 0xFFF));

                  pstPicture->stPicParms.stTSM.stDynamic.eTsmResult = BXDM_PictureProvider_TSMResult_eWait;

                  /* SW7425-2915: added for issues at startup in NRT mode. Used in
                   * BXDM_PPOUT_P_CalculateVdcData to determine how to set bIgnorePicture.
                   */
                  pLocalState->bWaitDueTo2ndSlotTransition = true;
               }
            }
            else
            {
               /* We have a second pass slot, so we reset the PTS
                * offset to zero so that next time we pick the
                * element with the proper polarity */
               hXdmPP->stDMState.stDecode.stFieldInversionCorrectionPTSOffset.uiWhole = 0;
               hXdmPP->stDMState.stDecode.stFieldInversionCorrectionPTSOffset.uiFractional = 0;
               pLocalState->bFICChanged = true;

               if ( pstPicture->stPicParms.stTSM.stStatic.uiNumElements > (pstPicture->stPicParms.stDisplay.stDynamic.uiSelectedElement + 1) )
               {
                  /* If we're in a 2nd slot TSM pass AND there is
                   * more than one element, we can pick the NEXT
                   * element for display which should be in the 1st
                   * TSM slot which should prevent field inversion
                   * on THIS vsync */
                  pstPicture->stPicParms.stDisplay.stDynamic.uiSelectedElement++;
                  BXVD_DBG_MSG(hXdmPP,("%x:[%02x.%03x] Polarity Override: 2nd slot (next element)", 
                                             hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                                             BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ),
                                             pstPicture->stPicParms.uiPPBIndex & 0xFFF));
                  BXDM_PPDBG_P_SelectionLog( hXdmPP, BXDM_PPDBG_Selection_PolarityOverride_e2ndSlotNextElement );
               }
               else
               {
                  BXVD_DBG_MSG(hXdmPP,("%x:[%02x.%03x] Polarity Override: 2nd slot (next ppb)", 
                                             hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                                             BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ),
                                             pstPicture->stPicParms.uiPPBIndex & 0xFFF));
                  BXDM_PPDBG_P_SelectionLog( hXdmPP, BXDM_PPDBG_Selection_PolarityOverride_e2ndSlotNextPPB );

                  BXDM_PPFIC_S_SelectPreviousElement(hXdmPP,
                                                     pLocalState,
                                                     pstPicture);
               }
            }

            /* We need to reset the source polarity if FIC has
             * changed the originally selected element */
            BXDM_PPQM_P_GetElementPolarity(
               pstPicture,
               pstPicture->stPicParms.stDisplay.stDynamic.uiSelectedElement,
               &(pstPicture->stPicParms.stDisplay.stDynamic.eSourcePolarity)
               );
         }
      }
   }

   return BERR_SUCCESS;
}

BERR_Code BXDM_PPFIC_P_ResetFieldInversionCorrectionOffset(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState
   )
{
   /* Determine if the FIC offset should be reset */
   if ( ( 0 != hXdmPP->stDMState.stDecode.stFieldInversionCorrectionPTSOffset.uiWhole )
        && ( 0 != hXdmPP->stDMState.stDecode.stFieldInversionCorrectionPTSOffset.uiFractional )
      )
   {
      BXVD_DBG_MSG(hXdmPP,("%x:[%02x.xxx] FIC Reset", 
                        hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                        BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ) ));
      BXDM_PPDBG_P_SelectionLog( hXdmPP, BXDM_PPDBG_Selection_PolarityOverride_eFICReset );
      hXdmPP->stDMState.stDecode.stFieldInversionCorrectionPTSOffset.uiWhole = 0;
      hXdmPP->stDMState.stDecode.stFieldInversionCorrectionPTSOffset.uiFractional = 0;
      pLocalState->bFICChanged = true;
   }

   return BERR_SUCCESS;
}
