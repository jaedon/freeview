/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_pvr.c $
 * $brcm_Revision: Hydra_Software_Devel/39 $
 * $brcm_Date: 7/6/12 12:04p $
 *
 * Module Description:
 *   See Module Overview below
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/src/bxvd_pvr.c $
 * 
 * Hydra_Software_Devel/39   7/6/12 12:04p btosi
 * SW7425-3358: added support for FNRT
 * 
 * Hydra_Software_Devel/38   7/2/12 10:00a btosi
 * SW7425-3358: first cut at the FNRT changes
 * 
 * Hydra_Software_Devel/37   2/1/12 1:58p btosi
 * SW7425-2270: add support for IgnoreNRTUnderflow
 * 
 * Hydra_Software_Devel/36   7/20/11 3:05p davidp
 * SW7420-2001: Reorder header file includes.
 * 
 * Hydra_Software_Devel/35   8/3/10 2:03p btosi
 * SW7400-2870: added BXVD_[Set/Get]PlaybackRate(_isr)
 * 
 * Hydra_Software_Devel/34   4/27/10 4:59p nilesh
 * SW7601-186: Fixed pause/slow motion interaction in XDM to be compatible
 * with XVD DMv2
 * 
 * Hydra_Software_Devel/33   3/24/10 3:00p davidp
 * SW7405-3137: Save and restore extended CDB read register address for
 * decode restart cases.
 * 
 * Hydra_Software_Devel/32   2/25/10 4:24p nilesh
 * SW7405-2993: XDM Merge
 * 
 * Hydra_Software_Devel/31   11/30/09 4:41p btosi
 * SW7405-3245: added BXVD_DBG_* macros.  Map to either BDBG_INSTANCE_* or
 * BDBG_* at compile.
 *
 * Hydra_Software_Devel/30   7/29/09 2:45p pblanco
 * PR57177: There is no need for a GetFrameAdvanceMode API. Removing it.
 *
 * Hydra_Software_Devel/29   7/29/09 1:45p pblanco
 * PR57177: Added BXVD_PVR_GetFrameAdvance and
 * BXVD_PVR_GetFrameAdvance_isr functions.
 *
 * Hydra_Software_Devel/28   3/27/09 3:02p btosi
 * PR52959: added support for programmable playback rate in vsync mode
 *
 * Hydra_Software_Devel/27   3/26/09 6:13p nilesh
 * PR53580: Cleaned up DM structs
 *
 * Hydra_Software_Devel/26   3/24/09 4:52p nilesh
 * PR52905: Added ISR versions of most get/set functions.  Cleaned up
 * formatting.  Removed critical sections for some get functions.
 *
 * Hydra_Software_Devel/25   1/21/09 2:02p nilesh
 * PR45052: Converted BDBG_xxx to BDBG_INSTANCE_xxx calls to support
 * multiple channels
 *
 * Hydra_Software_Devel/24   12/5/08 5:53p davidp
 * PR49533: Do not preserve decoder counters if decoder is not actively
 * decoding.
 *
 * Hydra_Software_Devel/23   11/20/08 5:12p davidp
 * PR49533: Add bPreserveCounters boolean to channel structure to not
 * reset decoder counters for certain StartDecode situations.
 *
 * Hydra_Software_Devel/22   8/5/08 2:00p nilesh
 * PR29915: Improve BDBG_MSG output
 *
 * Hydra_Software_Devel/21   10/22/07 10:39a btosi
 * PR30310: added BXVD_PVR_GetGopTrickMode()
 *
 * Hydra_Software_Devel/20   9/21/07 7:45a pblanco
 * PR35163: Fix compiler warning about unused argument.
 *
 * Hydra_Software_Devel/19   9/6/07 5:35p nilesh
 * PR29915: Added BERR_TRACE wrapper around all return codes
 *
 * Hydra_Software_Devel/18   8/30/07 4:46p rayshap
 * PR33893: Add support for BXVD_PVR_Set/GetAutoValidateStcOnPause
 *
 * Hydra_Software_Devel/17   6/13/07 8:07a btosi
 * PR30310: added BXVD_PVR_SetGopTrickMode() to mainline branch
 *
 * Hydra_Software_Devel/17   6/12/07 3:50p btosi
 * PR30310: added BXVD_PVR_SetGopTrickMode() to mainline branch
 *
 * Hydra_Software_Devel/17   6/12/07 3:48p btosi
 * PR30310: added BXVD_PVR_SetGopTrickMode() to mainline branch
 *
 * Hydra_Software_Devel/16   4/11/07 2:27p davidp
 * PR29581: Update channels bHostSparseMode before decoder is started in
 * BXVD_PVR_SetHostSparseMode().
 *
 * Hydra_Software_Devel/15   1/10/07 5:58p davidp
 * PR26837: If in sparse mode when bxvd_SetSkipPictureModeDecode is
 * called, stop/start decode.
 *
 * Hydra_Software_Devel/14   12/12/06 3:01p davidp
 * PR25443: Merge Simplified FW API branch into mainline
 *
 * Hydra_Software_Devel/xvd_7401_PR25443_Simplified_FW_API/3   12/11/06 4:58p davidp
 * PR25443: Clear PVR state when BXVD_StopDecode is called unless preserve
 * flag is set.
 *
 * Hydra_Software_Devel/xvd_7401_PR25443_Simplified_FW_API/2   11/15/06 4:16p davidp
 * PR25443: Merge mainline changes into branch, cleanup code.
 *
 * Hydra_Software_Devel/xvd_7401_PR25443_Simplified_FW_API/1   11/6/06 6:15p davidp
 * PR25443: Initial checkin of Simplified FW API support
 *
 * Hydra_Software_Devel/13   3/28/06 11:48a pblanco
 * PR20274: Removed old trick mode API code.
 *
 * Hydra_Software_Devel/12   3/24/06 1:35p pblanco
 * PR20274: Interim checkin prior to FW implementation.
 *
 * Hydra_Software_Devel/11   3/15/06 4:10p pblanco
 * PR20197: It is no longer necessary for BXVD_PVR_SetSlowMotionRate to
 * explicitly set the display field mode now that the correct structure
 * member is being set by BXVD_SetDisplayFieldMode.
 *
 * Hydra_Software_Devel/10   3/15/06 3:35p pblanco
 * PR20197: Added BDBG_MSG output to BXVD_PVR_SetSlowMotionRate.
 *
 * Hydra_Software_Devel/9   3/14/06 4:43p pblanco
 * PR20197: Fixed test for slow motion rate so that only value > 1 changed
 * to top field only. Also made sure that values of 0 are allowed as
 * parameters.
 *
 * Hydra_Software_Devel/8   3/14/06 3:53p pblanco
 * PR20197: Don't allow BXVD_SetSlowMotionRate to force field mode to top
 * only when setting rate to 0 or 1 (normal).
 *
 * Hydra_Software_Devel/7   3/9/06 5:06p pblanco
 * PR19785: Removed extraneous top field only settings for BTP trick mode
 * enable.
 *
 * Hydra_Software_Devel/6   3/9/06 2:52p pblanco
 * PR19792: interim checkin.
 *
 * Hydra_Software_Devel/5   1/30/06 9:01a pblanco
 * PR19308: Pass trick mode as an unsigned long in
 * BXVD_P_HostCmdDecEnableBRCMTrickPlay.
 *
 * Hydra_Software_Devel/4   1/10/06 11:19a pblanco
 * PR16052: Added call to BXVD_P_HostCmdDecEnableBRCMTrickPlay to
 * BXVD_PVR_EnableBTPMode.
 *
 * Hydra_Software_Devel/3   1/5/06 2:44p pblanco
 * PR16052: Removed deprecated code.
 *
 * Hydra_Software_Devel/2   1/3/06 2:43p pblanco
 * PR16052: Changed BRCM trick mode APIs to new BTPMode format. Removed
 * unused Aas well.
 *
 * Hydra_Software_Devel/1   12/13/05 11:33a pblanco
 * PR16052: Initial check in.
 *
 ****************************************************************************/
#include "bstd.h"
#include "bxvd.h"
#include "bxvd_platform.h"
#include "bxvd_priv.h"
#include "bxvd_pvr.h"

BDBG_MODULE(BXVD_PVR);

/***************************************************************************
***************************************************************************/
BERR_Code BXVD_PVR_EnablePause
(
   BXVD_ChannelHandle hXvdCh,
   bool bEnablePause
   )
{
   BERR_Code rc = BERR_SUCCESS;

   BDBG_ENTER(BXVD_PVR_EnablePause);

   BKNI_EnterCriticalSection();
   rc = BXVD_PVR_EnablePause_isr(
      hXvdCh,
      bEnablePause
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_PVR_EnablePause);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_PVR_EnablePause_isr
(
   BXVD_ChannelHandle hXvdCh,
   bool bEnablePause
   )
{
   BERR_Code rc;
   BXDM_Picture_Rate stCurrentPlaybackRate;
   BDBG_ENTER(BXVD_PVR_EnablePause_isr);

   BDBG_ASSERT(hXvdCh);

   rc = BXDM_PictureProvider_GetPlaybackRate_isr(
            hXvdCh->hPictureProvider,
            &stCurrentPlaybackRate);

   BXVD_DBG_MSG(hXvdCh, ("BXVD_PVR_EnablePause(%d)", bEnablePause));

   if ( BERR_SUCCESS == rc )
   {
      switch ( bEnablePause )
      {
         case true:

            /* SW7400-2870: pause if not already paused by a previous call to BXVD_PVR_EnablePause.*/
            if ( false == hXvdCh->bPauseActive  )
            {
               hXvdCh->bPauseActive = true;
               
               hXvdCh->stSavedPlaybackRate = stCurrentPlaybackRate;
               hXvdCh->bSavedPlaybackRateValid = true;

               stCurrentPlaybackRate.uiNumerator = 0;
               stCurrentPlaybackRate.uiDenominator = 1;

               rc = BXDM_PictureProvider_SetPlaybackRate_isr(
                        hXvdCh->hPictureProvider,
                        &stCurrentPlaybackRate
                        );
            }
            break;

         case false:

            /* SW7400-2870: un-pause if previously paused by calling BXVD_PVR_EnablePause. */
            if ( true == hXvdCh->bPauseActive  )
            {
               hXvdCh->bPauseActive = false;

               if ( true == hXvdCh->bSavedPlaybackRateValid )
               {
                  rc = BXDM_PictureProvider_SetPlaybackRate_isr(
                           hXvdCh->hPictureProvider,
                           &hXvdCh->stSavedPlaybackRate
                           );
               }
               else
               {
                  stCurrentPlaybackRate.uiNumerator = 1;
                  stCurrentPlaybackRate.uiDenominator = 1;

                  rc = BXDM_PictureProvider_SetPlaybackRate_isr(
                           hXvdCh->hPictureProvider,
                           &stCurrentPlaybackRate
                           );

                  hXvdCh->stSavedPlaybackRate = stCurrentPlaybackRate;
                  hXvdCh->bSavedPlaybackRateValid = true;
               }
            }
            break;
      }
   }

   BDBG_LEAVE(BXVD_PVR_EnablePause_isr);
   return BERR_TRACE(rc);
}

/***************************************************************************
***************************************************************************/
BERR_Code BXVD_PVR_GetPauseStatus
(
   BXVD_ChannelHandle hXvdCh,
   bool *pbPauseStatus
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_PVR_GetPauseStatus);

   rc = BXVD_PVR_GetPauseStatus_isr(
      hXvdCh,
      pbPauseStatus
      );

   BDBG_LEAVE(BXVD_PVR_GetPauseStatus);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_PVR_GetPauseStatus_isr
(
   BXVD_ChannelHandle hXvdCh,
   bool *pbPauseStatus
   )
{
   BERR_Code rc=BERR_SUCCESS;
   BDBG_ENTER(BXVD_PVR_GetPauseStatus_isr);

   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(pbPauseStatus);

   /* SW7400-2870: within XDM, stCurrentPlaybackRate.uiNumerator can be set to '0' by either 
    * BXVD_PVR_EnablePause or BXVD_SetPlaybackRate. Use the new flag 'bPauseActive'
    * to determine if the system was paused by calling BXVD_PVR_EnablePause.
    */
   *pbPauseStatus = hXvdCh->bPauseActive;

   BDBG_LEAVE(BXVD_PVR_GetPauseStatus_isr);
   return BERR_TRACE( rc );
}

/***************************************************************************
DEPRECATED: SW7400-2870: use BXVD_SetPlaybackRate(isr) instead
***************************************************************************/
BERR_Code BXVD_PVR_SetSlowMotionRate
(
   BXVD_ChannelHandle hXvdCh,
   unsigned long ulRate
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_PVR_SetSlowMotionRate);

   BKNI_EnterCriticalSection();
   rc = BXVD_PVR_SetSlowMotionRate_isr(
      hXvdCh,
      ulRate
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_PVR_SetSlowMotionRate);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_PVR_SetSlowMotionRate_isr
(
   BXVD_ChannelHandle hXvdCh,
   unsigned long ulRate
   )
{
   BERR_Code rc=BERR_SUCCESS;
   BXDM_Picture_Rate stPlaybackRate;
   BDBG_ENTER(BXVD_PVR_SetSlowMotionRate_isr);

   BDBG_ASSERT(hXvdCh);

   BXVD_DBG_MSG(hXvdCh, ("BXVD_PVR_SetSlowMotionRate(%d)", ulRate));

   /* The addition of "uiPvrPlaybackRate" allows the playback rate to be treated roughly
    * as a percentage of normal playback.
    */
   BKNI_Memset( &stPlaybackRate, 0, sizeof( BXDM_Picture_Rate ) );

   stPlaybackRate.uiNumerator = 1;
   if ( ulRate > 1 )
   {
      stPlaybackRate.uiDenominator = ulRate;
   }
   else
   {
      stPlaybackRate.uiDenominator = 1;
   }

   /* SW7601-186: XVD + DMv2 maintained separate pause and slow motion states
    * and put precedence on the paused state.  However, XVD + XDM implements pause
    * and slow motion using the same single state (the playback rate) in XDM.
    *
    * I.e. pause --> playback_rate = 0
    *      slow motion --> playback_rate = slow motion rate
    *
    * The following call sequence exposed an XDM compatibility issue:
    *    1) BXVD_PVR_EnablePause_isr(true) <-- playback_rate = 0
    *    2) BXVD_PVR_SetSlowMotionRate_isr(1) <-- playback_rate = 100% (but should stay at 0)
    *
    * To maintain backwards compatibility with XVD + DMv2, the XDM playback rate
    * is not changed if the system is in a paused state.  The slow motion rate is
    * instead remembered and set when unpaused.
    */
   if ( false == hXvdCh->bPauseActive )
   {
      rc = BXDM_PictureProvider_SetPlaybackRate_isr(
               hXvdCh->hPictureProvider,
               &stPlaybackRate
               );
   }

   hXvdCh->stSavedPlaybackRate = stPlaybackRate;
   hXvdCh->bSavedPlaybackRateValid = true;

   BDBG_LEAVE(BXVD_PVR_SetSlowMotionRate);
   return BERR_TRACE( rc );
}

/***************************************************************************
***************************************************************************/
BERR_Code BXVD_PVR_GetSlowMotionRate
(
   BXVD_ChannelHandle hXvdCh,
   unsigned long *pSMRate
   )
{
   BERR_Code rc;
   BDBG_ENTER(BXVD_PVR_GetSlowMotionRate);

   rc = BXVD_PVR_GetSlowMotionRate_isr(
      hXvdCh,
      pSMRate
      );

   BDBG_LEAVE(BXVD_PVR_GetSlowMotionRate);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_PVR_GetSlowMotionRate_isr
(
   BXVD_ChannelHandle hXvdCh,
   unsigned long *pSMRate
   )
{
   BERR_Code rc=BERR_SUCCESS;
   BXDM_Picture_Rate stPlaybackRate;
   BDBG_ENTER(BXVD_PVR_GetSlowMotionRate_isr);

   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(pSMRate);

   /* SW7400-2870: if not paused by a call to BXVD_PVR_EnablePause, retrieve 
    * the playback rate from XDM.  If paused, return the valued saved in the 
    * channel structure.
    */
   if ( false == hXvdCh->bPauseActive )
   {
      rc = BXDM_PictureProvider_GetPlaybackRate_isr(
                  hXvdCh->hPictureProvider,
                  &stPlaybackRate
                  );

      if (  BERR_SUCCESS != rc )
      {
         *pSMRate = 0;
         BXVD_DBG_WRN(hXvdCh, ("BXVD_PVR_GetSlowMotionRate_isr() BXDM_PictureProvider_GetPlaybackRate_isr returned %d", rc ));
         return BERR_TRACE( rc );
      }
   }
   else
   {
      if ( true == hXvdCh->bSavedPlaybackRateValid )
      {
         stPlaybackRate = hXvdCh->stSavedPlaybackRate;
      }
      else
      {
         /* It should be impossible to hit this case.  If "bPauseActive" is true,
          * there should always be a saved playback rate.  Add this warning in the
          * event a logic bug creeps in.
          */
         stPlaybackRate.uiNumerator = 1;
         stPlaybackRate.uiDenominator = 1;
         BXVD_DBG_WRN(hXvdCh, ("BXVD_PVR_GetSlowMotionRate_isr() bPauseActive but no saved playback rate."));
      }
   }

   if ( 0 == stPlaybackRate.uiNumerator )
   {
      *pSMRate = 0;
   }
   else
   {
      *pSMRate = stPlaybackRate.uiDenominator / stPlaybackRate.uiNumerator;
   }

   BDBG_LEAVE(BXVD_PVR_GetSlowMotionRate_isr);
   return BERR_TRACE( rc );
}

/***************************************************************************
***************************************************************************/
BERR_Code BXVD_PVR_FrameAdvance
(
   BXVD_ChannelHandle hXvdCh,
   BXVD_PVR_FrameAdvanceMode eFrameAdvMode
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_PVR_FrameAdvance);

   BKNI_EnterCriticalSection();
   rc = BXVD_PVR_FrameAdvance_isr(
      hXvdCh,
      eFrameAdvMode
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_PVR_FrameAdvance);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_PVR_FrameAdvance_isr
(
   BXVD_ChannelHandle hXvdCh,
   BXVD_PVR_FrameAdvanceMode eFrameAdvMode
   )
{
   BERR_Code rc;
   BDBG_ENTER(BXVD_PVR_FrameAdvance_isr);

   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(eFrameAdvMode <= BXVD_PVR_FrameAdvanceMode_eField_by_Field);

   switch ( eFrameAdvMode )
   {
      case BXVD_PVR_FrameAdvanceMode_eFrame_by_Frame:
         rc = BXDM_PictureProvider_SetFrameAdvanceMode_isr(
               hXvdCh->hPictureProvider,
               BXDM_PictureProvider_FrameAdvanceMode_eFrameByField
               );
         break;

      case BXVD_PVR_FrameAdvanceMode_eField_by_Field:
         rc = BXDM_PictureProvider_SetFrameAdvanceMode_isr(
               hXvdCh->hPictureProvider,
               BXDM_PictureProvider_FrameAdvanceMode_eField
               );
         break;

      default:
         BXVD_DBG_ERR(hXvdCh, ("Unknown frame advance mode (%d)", eFrameAdvMode));
         return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   BXVD_DBG_MSG(hXvdCh, ("BXVD_PVR_FrameAdvance(%d)", eFrameAdvMode));

   BDBG_LEAVE(BXVD_PVR_FrameAdvance_isr);
   return BERR_TRACE( rc );
}

/***************************************************************************
***************************************************************************/
BERR_Code BXVD_PVR_EnableReverseFields(
   BXVD_ChannelHandle hXvdCh,
   bool bEnable
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_PVR_EnableReverseFields);

   BKNI_EnterCriticalSection();
   rc = BXVD_PVR_EnableReverseFields_isr(
      hXvdCh,
      bEnable
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_PVR_EnableReverseFields);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_PVR_EnableReverseFields_isr(
   BXVD_ChannelHandle hXvdCh,
   bool bEnable
   )
{
   BERR_Code rc;
   BDBG_ENTER(BXVD_PVR_EnableReverseFields_isr);

   BDBG_ASSERT(hXvdCh);

   rc = BXDM_PictureProvider_SetReverseFieldsMode_isr(
         hXvdCh->hPictureProvider,
         bEnable
         );

   BXVD_DBG_MSG(hXvdCh, ("BXVD_PVR_EnableReverseFields(%d)", bEnable));

   BDBG_LEAVE(BXVD_PVR_EnableReverseFields_isr);
   return BERR_TRACE( rc );
}

/***************************************************************************
***************************************************************************/
BERR_Code BXVD_PVR_GetReverseFieldStatus
(
   BXVD_ChannelHandle hXvdCh,
   bool *pbEnable
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_PVR_GetReverseFieldStatus);

   rc = BXVD_PVR_GetReverseFieldStatus_isr(
      hXvdCh,
      pbEnable
      );

   BDBG_LEAVE(BXVD_PVR_GetReverseFieldStatus);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_PVR_GetReverseFieldStatus_isr
(
   BXVD_ChannelHandle hXvdCh,
   bool *pbEnable
   )
{
   BERR_Code rc;
   BDBG_ENTER(BXVD_PVR_GetReverseFieldStatus_isr);

   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(pbEnable);

   rc = BXDM_PictureProvider_GetReverseFieldsMode_isr(
         hXvdCh->hPictureProvider,
         pbEnable
         );

   BDBG_LEAVE(BXVD_PVR_GetReverseFieldStatus_isr);
   return BERR_TRACE( rc );
}

/***************************************************************************
***************************************************************************/
BERR_Code BXVD_PVR_SetHostSparseMode(
   BXVD_ChannelHandle hXvdCh,
   bool bSparseMode
   )
{
   uint32_t            i;

   BAVC_XptContextMap  XptContextMap;
   BAVC_XptContextMap  aXptContextMap_Extended[BXVD_NUM_EXT_RAVE_CONTEXT];

   bool bCurSparseMode = hXvdCh->stDecoderContext.bHostSparseMode;

   BXDM_PictureProvider_PreserveStateSettings stPreserveStateSettings;
   BERR_Code rc = BERR_SUCCESS;

   BDBG_ENTER(BXVD_PVR_SetHostSparseMode);
   BDBG_ASSERT(hXvdCh);

   hXvdCh->stDecoderContext.bHostSparseMode = bSparseMode;

   if ((bCurSparseMode != bSparseMode) && (hXvdCh->eDecoderState == BXVD_P_DecoderState_eActive))
   {
      if ((hXvdCh->sDecodeSettings.eVideoCmprStd != BAVC_VideoCompressionStd_eH264) &&
	  (bSparseMode == true))
      {
	 BXVD_DBG_WRN(hXvdCh, ("Sparse mode enabled on Non H264 stream, Sparse mode now disabled"));
         hXvdCh->stDecoderContext.bHostSparseMode = false;
      }
      else
      {
	 BXVD_DBG_MSG(hXvdCh, ("BXVD_PVR_SetHostSparseMode: Stopping and starting decoding"));

	 BERR_TRACE(BXDM_PictureProvider_GetDefaultPreserveStateSettings(
	          hXvdCh->hPictureProvider,
	          &stPreserveStateSettings
	          ));

	 stPreserveStateSettings.bDisplay = true;
	 stPreserveStateSettings.bCounters = true;
	 hXvdCh->bPreserveState = true;

	 BERR_TRACE(BXDM_PictureProvider_SetPreserveStateSettings_isr(
	          hXvdCh->hPictureProvider,
	          &stPreserveStateSettings
	          ));

	 BERR_TRACE(BXVD_StopDecode(hXvdCh));

	 /* Reset XPT Rave CDB read register address */
	 XptContextMap.CDB_Read = hXvdCh->ulXptCDB_Read;
	 hXvdCh->sDecodeSettings.pContextMap = &XptContextMap;

         for (i = 0; i < hXvdCh->sDecodeSettings.uiContextMapExtNum; i++)
         {
            hXvdCh->sDecodeSettings.aContextMapExtended[i] = &aXptContextMap_Extended[i];
            aXptContextMap_Extended[i].CDB_Read = hXvdCh->aulXptCDB_Read_Extended[i];
         }

	 rc = BERR_TRACE(BXVD_StartDecode(hXvdCh, &hXvdCh->sDecodeSettings));

	 hXvdCh->bPreserveState = false;
      }
   }
   else
   {
      BXVD_DBG_MSG(hXvdCh, ("BXVD_PVR_SetHostSparseMode: NOT stopping/starting decoding.. Letting flush do it!!!"));
   }

   BDBG_LEAVE(BXVD_PVR_SetHostSparseMode);
   return BERR_TRACE(rc);
}

/***************************************************************************
***************************************************************************/
BERR_Code BXVD_PVR_GetHostSparseMode
(
   BXVD_ChannelHandle hXvdCh,
   bool *pbSparseMode
   )
{
   BDBG_ENTER(BXVD_PVR_GetHostSparseMode);
   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(pbSparseMode);

   *pbSparseMode = hXvdCh->stDecoderContext.bHostSparseMode;

   BDBG_LEAVE(BXVD_PVR_GetHostSparseMode);
   return BERR_TRACE(BERR_SUCCESS);
}

/***************************************************************************
***************************************************************************/
BERR_Code BXVD_PVR_SetGopTrickMode
(
   BXVD_ChannelHandle hXvdCh,
   bool bEnableTrickMode
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_PVR_SetGopTrickMode);

   BKNI_EnterCriticalSection();
   rc = BXVD_PVR_SetGopTrickMode_isr(
      hXvdCh,
      bEnableTrickMode
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_PVR_SetGopTrickMode);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_PVR_SetGopTrickMode_isr
(
   BXVD_ChannelHandle hXvdCh,
   bool bEnableTrickMode
   )
{
   BDBG_ENTER(BXVD_PVR_SetGopTrickMode_isr);

   BDBG_ASSERT(hXvdCh);

   hXvdCh->stDecoderContext.bReversePlayback = bEnableTrickMode;

   BXVD_DBG_MSG(hXvdCh, ("BXVD_PVR_SetGopTrickMode(%d)", bEnableTrickMode));

   BDBG_LEAVE(BXVD_PVR_SetGopTrickMode_isr);
   return BERR_TRACE(BERR_SUCCESS);
}

/***************************************************************************
***************************************************************************/
BERR_Code BXVD_PVR_GetGopTrickMode
(
   BXVD_ChannelHandle hXvdCh,
   bool *pbEnableTrickMode
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_PVR_GetGopTrickMode);

   rc = BXVD_PVR_GetGopTrickMode_isr(
      hXvdCh,
      pbEnableTrickMode
      );

   BDBG_LEAVE(BXVD_PVR_GetGopTrickMode);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_PVR_GetGopTrickMode_isr
(
   BXVD_ChannelHandle hXvdCh,
   bool *pbEnableTrickMode
   )
{
   BDBG_ENTER(BXVD_PVR_GetGopTrickMode_isr);

   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(pbEnableTrickMode);

   *pbEnableTrickMode = hXvdCh->stDecoderContext.bReversePlayback;

   BDBG_LEAVE(BXVD_PVR_GetGopTrickMode_isr);
   return BERR_TRACE(BERR_SUCCESS);
}

/***************************************************************************
***************************************************************************/
BERR_Code BXVD_PVR_SetAutoValidateStcOnPause
(
   BXVD_ChannelHandle hXvdCh,
   bool bAutoValidateStcOnPause
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_PVR_SetAutoValidateStcOnPause);

   BKNI_EnterCriticalSection();
   rc = BXVD_PVR_SetAutoValidateStcOnPause_isr(
      hXvdCh,
      bAutoValidateStcOnPause
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_PVR_SetAutoValidateStcOnPause);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_PVR_SetAutoValidateStcOnPause_isr
(
   BXVD_ChannelHandle hXvdCh,
   bool bAutoValidateStcOnPause
   )
{
   BERR_Code rc;
   BDBG_ENTER(BXVD_PVR_SetAutoValidateStcOnPause_isr);

   BDBG_ASSERT(hXvdCh);

   rc = BXDM_PictureProvider_SetAutoValidateStcOnPauseMode_isr(
         hXvdCh->hPictureProvider,
         bAutoValidateStcOnPause
         );

   BXVD_DBG_MSG(hXvdCh, ("BXVD_PVR_SetAutoValidateStcOnPause(%d)", bAutoValidateStcOnPause));

   BDBG_LEAVE(BXVD_PVR_SetAutoValidateStcOnPause_isr);
   return BERR_TRACE(rc);
}

/***************************************************************************
***************************************************************************/
BERR_Code BXVD_PVR_GetAutoValidateStcOnPause
(
   BXVD_ChannelHandle hXvdCh,
   bool *pbAutoValidateStcOnPause
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_PVR_GetAutoValidateStcOnPause);

   rc = BXVD_PVR_GetAutoValidateStcOnPause_isr(
      hXvdCh,
      pbAutoValidateStcOnPause
      );

   BDBG_LEAVE(BXVD_PVR_GetAutoValidateStcOnPause);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_PVR_GetAutoValidateStcOnPause_isr
(
   BXVD_ChannelHandle hXvdCh,
   bool *pbAutoValidateStcOnPause
   )
{
   BERR_Code rc;
   BDBG_ENTER(BXVD_PVR_GetAutoValidateStcOnPause_isr);

   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(pbAutoValidateStcOnPause);

   rc = BXDM_PictureProvider_GetAutoValidateStcOnPauseMode_isr(
         hXvdCh->hPictureProvider,
         pbAutoValidateStcOnPause
         );

   BDBG_LEAVE(BXVD_PVR_GetAutoValidateStcOnPause_isr);
   return BERR_TRACE( rc );
}

/***************************************************************************
   SW7425-2270: 
   The application will call SetIgnoreNRTUnderflow when it determines that an 
   NRT underflow is actually a gap in the content.
   see expanded comment in bxvd_pvr.h
****************************************************************************/

BERR_Code BXVD_PVR_SetIgnoreNRTUnderflow(
   BXVD_ChannelHandle hXvdCh,
   bool bIgnoreNRTUnderflow
   )
{
   BERR_Code rc;

   BDBG_ENTER( BXVD_PVR_SetIgnoreNRTUnderflow );

   BKNI_EnterCriticalSection();

   rc = BXVD_PVR_SetIgnoreNRTUnderflow_isr( hXvdCh, bIgnoreNRTUnderflow );

   BKNI_LeaveCriticalSection();
   
   BDBG_LEAVE( BXVD_PVR_SetIgnoreNRTUnderflow );

   return BERR_TRACE( rc );
}

BERR_Code BXVD_PVR_SetIgnoreNRTUnderflow_isr(
   BXVD_ChannelHandle hXvdCh,
   bool bIgnoreNRTUnderflow
   )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER( BXVD_PVR_SetIgnoreNRTUnderflow_isr );
   
   BDBG_ASSERT( hXvdCh );

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }
   
   rc = BXDM_PictureProvider_SetIgnoreNRTUnderflow_isr(
            hXvdCh->hPictureProvider,
            bIgnoreNRTUnderflow
            );

   BXVD_DBG_MSG(hXvdCh, ("BXVD_PVR_SetIgnoreNRTUnderflow::  bIgnoreNRTUnderflow:%d",
                  bIgnoreNRTUnderflow
                  ));


   BDBG_LEAVE( BXVD_PVR_SetIgnoreNRTUnderflow_isr );

   return BERR_TRACE( rc );
}

BERR_Code BXVD_PVR_GetIgnoreNRTUnderflow(
   BXVD_ChannelHandle hXvdCh,
   bool * pbIgnoreNRTUnderflow
   )
{
   BERR_Code rc;

   BDBG_ENTER( BXVD_PVR_GetIgnoreNRTUnderflow );

   BKNI_EnterCriticalSection();

   rc = BXVD_PVR_GetIgnoreNRTUnderflow_isr( hXvdCh, pbIgnoreNRTUnderflow );

   BKNI_LeaveCriticalSection();
   
   BDBG_LEAVE( BXVD_PVR_GetIgnoreNRTUnderflow );

   return BERR_TRACE( rc );
}

BERR_Code BXVD_PVR_GetIgnoreNRTUnderflow_isr(
   BXVD_ChannelHandle hXvdCh,
   bool * pbIgnoreNRTUnderflow
   )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER( BXVD_PVR_GetIgnoreNRTUnderflow_isr );
   
   BDBG_ASSERT( hXvdCh );
   BDBG_ASSERT( pbIgnoreNRTUnderflow );

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }
   
   rc = BXDM_PictureProvider_GetIgnoreNRTUnderflow_isr(
            hXvdCh->hPictureProvider,
            pbIgnoreNRTUnderflow
            );

   BXVD_DBG_MSG(hXvdCh, ("BXVD_PVR_GetIgnoreNRTUnderflow::  bIgnoreNRTUnderflow:%d",
                  *pbIgnoreNRTUnderflow
                  ));

   BDBG_LEAVE( BXVD_PVR_GetIgnoreNRTUnderflow_isr );

   return BERR_TRACE( rc );
}

/***************************************************************************
   SW7425-3358: support for FNRT.
****************************************************************************/

BERR_Code BXVD_PVR_SetFNRTSettings(
   BXVD_ChannelHandle hXvdCh,
   const BXVD_PVR_FNRTSettings * pstFNRTSettings
   )
{
   BERR_Code rc;

   BDBG_ENTER( BXVD_PVR_SetFNRTSettings );

   BKNI_EnterCriticalSection();

   rc = BXVD_PVR_SetFNRTSettings_isr( hXvdCh, pstFNRTSettings );

   BKNI_LeaveCriticalSection();
   
   BDBG_LEAVE( BXVD_PVR_SetFNRTSettings );

   return BERR_TRACE( rc );
}

BERR_Code BXVD_PVR_SetFNRTSettings_isr(
   BXVD_ChannelHandle hXvdCh,
   const BXVD_PVR_FNRTSettings * pstFNRTSettings
   )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER( BXVD_PVR_SetFNRTSettings_isr );
   
   BDBG_ASSERT( hXvdCh );

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }
   
   rc = BXDM_PictureProvider_SetFNRTSettings_isr(
            hXvdCh->hPictureProvider,
            pstFNRTSettings
            );

   BXVD_DBG_MSG(hXvdCh, ("BXVD_PVR_SetFNRTSettings::  pstFNRTSettings:%08x",
                  pstFNRTSettings
                  ));


   BDBG_LEAVE( BXVD_PVR_SetFNRTSettings_isr );

   return BERR_TRACE( rc );
}

BERR_Code BXVD_PVR_GetFNRTSettings(
   BXVD_ChannelHandle hXvdCh,
   BXVD_PVR_FNRTSettings * pstFNRTSettings
   )
{
   BERR_Code rc;

   BDBG_ENTER( BXVD_PVR_GetFNRTSettings );

   BKNI_EnterCriticalSection();

   rc = BXVD_PVR_GetFNRTSettings_isr( hXvdCh, pstFNRTSettings );

   BKNI_LeaveCriticalSection();
   
   BDBG_LEAVE( BXVD_PVR_GetFNRTSettings );

   return BERR_TRACE( rc );
}

BERR_Code BXVD_PVR_GetFNRTSettings_isr(
   BXVD_ChannelHandle hXvdCh,
   BXVD_PVR_FNRTSettings * pstFNRTSettings
   )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER( BXVD_PVR_GetFNRTSettings_isr );
   
   BDBG_ASSERT( hXvdCh );
   BDBG_ASSERT( pstFNRTSettings );

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }
   
   rc = BXDM_PictureProvider_GetFNRTSettings_isr(
            hXvdCh->hPictureProvider,
            pstFNRTSettings
            );

   BXVD_DBG_MSG(hXvdCh, ("BXVD_PVR_GetFNRTSettings::  pstFNRTSettings:%08x",
                  pstFNRTSettings
                  ));

   BDBG_LEAVE( BXVD_PVR_GetFNRTSettings_isr );

   return BERR_TRACE( rc );
}

BERR_Code BXVD_PVR_GetDefaultFNRTSettings(
   BXVD_ChannelHandle hXvdCh,
   BXVD_PVR_FNRTSettings * pstFNRTSettings
   )
{
   BERR_Code rc;

   BDBG_ENTER( BXVD_PVR_GetDefaultFNRTSettings );

   BKNI_EnterCriticalSection();

   rc = BXVD_PVR_GetDefaultFNRTSettings_isr( hXvdCh, pstFNRTSettings );

   BKNI_LeaveCriticalSection();
   
   BDBG_LEAVE( BXVD_PVR_GetDefaultFNRTSettings );

   return BERR_TRACE( rc );
}

BERR_Code BXVD_PVR_GetDefaultFNRTSettings_isr(
   BXVD_ChannelHandle hXvdCh,
   BXVD_PVR_FNRTSettings * pstFNRTSettings
   )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER( BXVD_PVR_GetDefaultFNRTSettings_isr );
   
   BDBG_ASSERT( hXvdCh );
   BDBG_ASSERT( pstFNRTSettings );

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }
   
   rc = BXDM_PictureProvider_GetDefaultFNRTSettings_isr(
            hXvdCh->hPictureProvider,
            pstFNRTSettings
            );

   BXVD_DBG_MSG(hXvdCh, ("BXVD_PVR_GetDefaultFNRTSettings::  pstFNRTSettings:%08x",
                  pstFNRTSettings
                  ));

   BDBG_LEAVE( BXVD_PVR_GetDefaultFNRTSettings_isr );

   return BERR_TRACE( rc );
}


/*******************/
/* Deprecated APIs */
/*******************/

/***************************************************************************
***************************************************************************/
BERR_Code BXVD_PVR_SetBUDMode
(
   BXVD_ChannelHandle hXvdCh,
   bool bBudMode
   )
{
   BDBG_ENTER(BXVD_PVR_SetBUDMode);
   BDBG_ASSERT(hXvdCh);

   BSTD_UNUSED(hXvdCh);
   BSTD_UNUSED(bBudMode);

   BXVD_DBG_WRN(hXvdCh, ("BXVD_PVR_SetBUDMode() is DEPRECATED. Video Decoder auto-detects BUD mode"));

   BDBG_LEAVE(BXVD_PVR_SetBudMode);
   return BERR_TRACE(BERR_SUCCESS);
}

/***************************************************************************
***************************************************************************/
BERR_Code BXVD_PVR_GetBUDMode
(
   BXVD_ChannelHandle hXvdCh,
   bool *pbBudMode
   )
{
   BDBG_ENTER(BXVD_PVR_GetBUDMode);
   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(pbBudMode);

   BSTD_UNUSED(hXvdCh);
   BSTD_UNUSED(pbBudMode);

   BDBG_LEAVE(BXVD_PVR_GetBUDMode);
   return BERR_TRACE(BERR_SUCCESS);
}

/* End of File */
