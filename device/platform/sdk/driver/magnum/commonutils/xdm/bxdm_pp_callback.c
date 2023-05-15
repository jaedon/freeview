/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxdm_pp_callback.c $
 * $brcm_Revision: Hydra_Software_Devel/21 $
 * $brcm_Date: 9/11/12 11:33a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/xdm/bxdm_pp_callback.c $
 * 
 * Hydra_Software_Devel/21   9/11/12 11:33a btosi
 * SW7405-4736: (Enhance XDM debug capabilities) standardized the instance
 * Id in the debug message
 * 
 * Hydra_Software_Devel/20   8/2/12 3:43p btosi
 * SW7405-4736: change the XDM uiInstanceId format to
 * uDecoderInstance.uiVDCRectangleNum
 * 
 * Hydra_Software_Devel/19   7/6/12 10:40a btosi
 * SW7425-3358: added support for FNRT
 * 
 * Hydra_Software_Devel/18   12/21/10 4:24p delkert
 * SW7405-5043: Split STC stored in local state into two parts: STC from
 * decoder and STC after JTI adjustment
 *
 * Hydra_Software_Devel/17   11/17/10 9:37a btosi
 * SW7422-72: added new BXVD and XDM 3D API's
 *
 * Hydra_Software_Devel/16   10/27/10 10:30a btosi
 * SW7422-72: merged 3D changes to mainline
 *
 * Hydra_Software_Devel/15   10/20/10 11:47a btosi
 * SW7405-4804: deliver the SEI messages for pictures that are dropped
 *
 * Hydra_Software_Devel/13   9/23/10 3:09p btosi
 * SW7405-4736: add support for a XDM instance ID to help debug multi-
 * channel issues
 *
 * Hydra_Software_Devel/12   8/10/10 4:16p btosi
 * SW7405-4736: added PPB index to debug messages
 *
 * Hydra_Software_Devel/11   8/2/10 10:17a btosi
 * SW7400-2880: (Coverity) handle
 * BXDM_Picture_ExtensionType_eSEIFramePacking in switch statement
 *
 * Hydra_Software_Devel/10   7/29/10 11:22a btosi
 * SW7405-4560: added support for the SEI "new message" flag.
 *
 * Hydra_Software_Devel/9   6/29/10 11:25a btosi
 * SW7405-4365: revamped the timer code to selectively enable the function
 * and callback timing
 *
 * Hydra_Software_Devel/8   6/15/10 2:11p btosi
 * SW7405-4500: fixed compiler warning when DEBUG=n
 *
 * Hydra_Software_Devel/7   5/18/10 9:16a btosi
 * SW7405-4352: added break statement
 *
 * Hydra_Software_Devel/6   5/12/10 12:10p btosi
 * SWBLURAY-20606: enhanced BXDM_PPCB_S_PrintExtensionData
 *
 * Hydra_Software_Devel/5   5/10/10 3:05p btosi
 * SW7550-379: fixed issue with the change for SW7405-3774
 *
 * Hydra_Software_Devel/4   5/4/10 10:46a btosi
 * SW7405-3996: added support for new SEI message format
 *
 * Hydra_Software_Devel/SW7405-3996/1   4/5/10 2:38p btosi
 * SW7405-3996: added code to dump extension data
 *
 * Hydra_Software_Devel/3   3/18/10 3:46p btosi
 * SW7405-3774: clear bExecutedPtsStcOffsetCB when the callback is
 * actually executed
 *
 * Hydra_Software_Devel/2   3/18/10 10:54a btosi
 * SW7405-3774: added bExecutedPtsStcOffsetCB flag.
 *
 * Hydra_Software_Devel/1   2/16/10 10:50a nilesh
 * SW7405-2993: Initial XDM version
 *
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "bdbg.h"                /* Dbglib */

#include "bxdm_pp.h"
#include "bxdm_pp_priv.h"
#include "bxdm_pp_callback_priv.h"
#include "bxdm_pp_qm.h"
#include "bxdm_pp_dbg.h"

BDBG_MODULE(BXDM_PPCB);		/* Register software module with debug interface */

const char BXDM_PictureProvider_P_DISPMGR_CALLBACK_NODE[]="DMCB:\t""$brcm_Revision: Hydra_Software_Devel/21 $";

/*******************************************************************************
 *
 *  Local function protoypes.
 *
 ******************************************************************************/

static void BXDM_PPCB_S_EvaluateCriteria_PtsStcOffset(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState
   );

static void BXDM_PPCB_S_FirstPtsReady( BXDM_PictureProvider_Handle hXdmPP, BXDM_PictureProvider_P_LocalState* pLocalState );
static void BXDM_PPCB_S_FirstCodedPtsReady( BXDM_PictureProvider_Handle hXdmPP, BXDM_PictureProvider_P_LocalState* pLocalState );
static void BXDM_PPCB_S_FirstPtsPassed( BXDM_PictureProvider_Handle hXdmPP, BXDM_PictureProvider_P_LocalState* pLocalState );
static void BXDM_PPCB_S_PtsError( BXDM_PictureProvider_Handle hXdmPP, BXDM_PictureProvider_P_LocalState* pLocalState );
static void BXDM_PPCB_S_PtsStcOffset( BXDM_PictureProvider_Handle hXdmPP, BXDM_PictureProvider_P_LocalState* pLocalState );
static void BXDM_PPCB_S_IFrame( BXDM_PictureProvider_Handle hXdmPP, BXDM_PictureProvider_P_LocalState* pLocalState );
static void BXDM_PPCB_S_PictureParameters( BXDM_PictureProvider_Handle hXdmPP, BXDM_PictureProvider_P_LocalState* pLocalState );
static void BXDM_PPCB_S_TsmPassInASTMMode( BXDM_PictureProvider_Handle hXdmPP, BXDM_PictureProvider_P_LocalState* pLocalState );
static void BXDM_PPCB_S_PictureUnderEvaluation( BXDM_PictureProvider_Handle hXdmPP, BXDM_PictureProvider_P_LocalState* pLocalState );

static void BXDM_PPCB_S_RequestSTC( BXDM_PictureProvider_Handle hXdmPP, BXDM_PictureProvider_P_LocalState* pLocalState );
static void BXDM_PPCB_S_ClipStart( BXDM_PictureProvider_Handle hXdmPP, BXDM_PictureProvider_P_LocalState* pLocalState );
static void BXDM_PPCB_S_ClipStop( BXDM_PictureProvider_Handle hXdmPP, BXDM_PictureProvider_P_LocalState* pLocalState );
static void BXDM_PPCB_S_PictureMarker( BXDM_PictureProvider_Handle hXdmPP, BXDM_PictureProvider_P_LocalState* pLocalState );
static void BXDM_PPCB_S_DecodeError( BXDM_PictureProvider_Handle hXdmPP, BXDM_PictureProvider_P_LocalState* pLocalState );
static void BXDM_PPCB_S_TSMResult( BXDM_PictureProvider_Handle hXdmPP, BXDM_PictureProvider_P_LocalState* pLocalState );
static void BXDM_PPCB_S_ChunkDone( BXDM_PictureProvider_Handle hXdmPP, BXDM_PictureProvider_P_LocalState* pLocalState );

static void BXDM_PPCB_S_PictureExtensionData(
                  BXDM_PictureProvider_Handle hXdmPP,
                  BXDM_PictureProvider_P_LocalState* pLocalState,
                  BXDM_PictureProvider_P_Picture_Context * pstPicture
                  );
static void BXDM_PPCB_S_PrintExtensionData(
                  BXDM_PictureProvider_Handle hXdmPP,
                  const BXDM_Picture_ExtensionInfo * pstExtensionInfo
                  );

/*******************************************************************************
 *
 *  Local functions.
 *
 ******************************************************************************/

static void BXDM_PPCB_S_FirstPtsReady(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState
   )
{
   bool bSignalFirstPPB;

   BDBG_ENTER(BXDM_PPCB_S_FirstPtsReady);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pLocalState );

   /* First check that a callback function has been registered */
   bSignalFirstPPB = ( NULL != hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eFirstPTSReady].stFunction.fFirstPTSReady );

   /* Then check that the callback mask bit is set. */
   bSignalFirstPPB &= ( true == hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eFirstPTSReady].bEnable );

   /* Finally check that the callback criteria has been met. */
   bSignalFirstPPB &= ( true == pLocalState->bFirstPPBReceived );

   /* If all the conditions have been met, execute the callback. */
   if( bSignalFirstPPB )
   {
      /*
       * By design, only execute the callback once per BXVD_StartDecode() call.
       * So, clear the appropriate mask bit(s) when the callback is executed.
       */
         pLocalState->uiCallbackTriggered |= BXDM_PPDBG_Callback_FirstPtsReady;

      hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eFirstPTSReady].bEnable = false;

      /* PR52901: update with the STC for this vsync. */
      hXdmPP->stDMState.stDecode.stFirstCodedPts.uiSTCSnapShot = pLocalState->uiAdjustedStc;

      BXVD_DBG_MSG(hXdmPP, ("%x:[%02x.xxx] BXDM_PictureProvider_Callback_eFirstPtsReady(%08x)",
                                 hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                                 BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ),
                                 hXdmPP->stDMState.stDecode.stFirstCodedPts.ui32RunningPTS ));


      /* Snapshot the start time. */
      BXDM_PPTMR_P_SnapshotCallbackStartTime( hXdmPP, BXDM_PictureProvider_Callback_eFirstPTSReady );

      hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eFirstPTSReady].stFunction.fFirstPTSReady (
                                 hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eFirstPTSReady].pPrivateContext,
                                 hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eFirstPTSReady].iPrivateParam,
                                 &hXdmPP->stDMState.stDecode.stFirstCodedPts
                                 );

      /* Snapshot the end time. */
      BXDM_PPTMR_P_SnapshotCallbackEndTime( hXdmPP, BXDM_PictureProvider_Callback_eFirstPTSReady );
   }

   BDBG_LEAVE(BXDM_PPCB_S_FirstPtsReady );

   return;

}  /* end of BXDM_PPCB_S_FirstPtsReady() */

static void BXDM_PPCB_S_FirstCodedPtsReady(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState
   )
{
   bool bSignalFirstCodedPts;

   BDBG_ENTER(BXDM_PPCB_S_FirstCodedPtsReady);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pLocalState );

   /* First check that a callback function has been registered */
   bSignalFirstCodedPts = ( NULL != hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eFirstCodedPTSReady].stFunction.fFirstCodedPTSReady );

   /* Then check that the callback mask bit is set. */
   bSignalFirstCodedPts &= ( true == hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eFirstCodedPTSReady].bEnable );

   /* Finally check that the callback criteria has been met. */
   bSignalFirstCodedPts &= ( true == pLocalState->bCodedPtsReceived );

   /* If all the conditions have been met, execute the callback. */
   if( bSignalFirstCodedPts )
   {
      pLocalState->uiCallbackTriggered |= BXDM_PPDBG_Callback_FirstCodedPtsReady;

      hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eFirstCodedPTSReady].bEnable = false;

      /* PR52901: update with the STC for this vsync. */
      hXdmPP->stDMState.stDecode.stFirstCodedPts.uiSTCSnapShot = pLocalState->uiAdjustedStc;

      BXVD_DBG_MSG(hXdmPP, ("%x:[%02x.xxx] BXDM_PictureProvider_Callback_eFirstCodedPtsReady(%08x)",
                                 hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                                 BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ),
                                 hXdmPP->stDMState.stDecode.stFirstCodedPts.ui32RunningPTS ));


      /* Snapshot the start time. */
      BXDM_PPTMR_P_SnapshotCallbackStartTime( hXdmPP, BXDM_PictureProvider_Callback_eFirstCodedPTSReady );

      hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eFirstCodedPTSReady].stFunction.fFirstCodedPTSReady (
                                 hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eFirstCodedPTSReady].pPrivateContext,
                                 hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eFirstCodedPTSReady].iPrivateParam,
                                 &hXdmPP->stDMState.stDecode.stFirstCodedPts
                                 );

      /* Snapshot the end time. */
      BXDM_PPTMR_P_SnapshotCallbackEndTime( hXdmPP, BXDM_PictureProvider_Callback_eFirstCodedPTSReady );
   }

   BDBG_LEAVE(BXDM_PPCB_S_FirstCodedPtsReady );

   return;

}  /* end of BXDM_PPCB_S_FirstCodedPtsReady() */

static void BXDM_PPCB_S_FirstPtsPassed(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState
   )
{
   bool bSignalTsmPass;

   BDBG_ENTER(BXDM_PPCB_S_FirstPtsPassed);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pLocalState );

   /* First check that a callback function has been registered */
   bSignalTsmPass = ( NULL != hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eFirstPTSPassed].stFunction.fFirstPTSPassed );

   /* Then check that the callback mask bit is set. */
   bSignalTsmPass &= ( true == hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eFirstPTSPassed].bEnable );

   /* Finally check that the callback criteria has been met. */
   bSignalTsmPass &= ( true == pLocalState->bTSMPass );

   /* If all the conditions have been met, execute the callback. */
   if( bSignalTsmPass )
   {
      pLocalState->uiCallbackTriggered |= BXDM_PPDBG_Callback_FirstPtsPassed;

      /*
       * Clear the interrup mask bit so that the callback is only
       * executed once per BXVD_StartDecode() call.
       */
      hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eFirstPTSPassed].bEnable = false;

      /* TODO: is "stCurrentPTSInfo" the correct value to return? */

      BXVD_DBG_MSG(hXdmPP, ("%x:[%02x.xxx] BXDM_PictureProvider_Callback_eFirstPtsPassed(%08x)",
                                 hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                                 BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ),
                                 hXdmPP->stDMStatus.stCurrentPTS.ui32RunningPTS ));

      /* Snapshot the start time. */
      BXDM_PPTMR_P_SnapshotCallbackStartTime( hXdmPP, BXDM_PictureProvider_Callback_eFirstPTSPassed );

      hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eFirstPTSPassed].stFunction.fFirstPTSPassed (
                                 hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eFirstPTSPassed].pPrivateContext,
                                 hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eFirstPTSPassed].iPrivateParam,
                                 &hXdmPP->stDMStatus.stCurrentPTS
                                 ) ;

      /* Snapshot the end time. */
      BXDM_PPTMR_P_SnapshotCallbackEndTime( hXdmPP, BXDM_PictureProvider_Callback_eFirstPTSPassed );

      /* TODO: from DM V1, is the following logic still needed?
       * If so, is this the appropriate place?
       * PR 32432: Resetting the Count so that Pic on TSM Wait is not counted
       */
      hXdmPP->stDMStatus.stCounters.uiDisplayedCount = 0;
      hXdmPP->stDMStatus.stCounters.uiDisplayedParityFailureCount = 0;
   }

   BDBG_LEAVE(BXDM_PPCB_S_FirstPtsPassed);

   return;

}  /* end of BXDM_PPCB_S_FirstPtsPassed() */

static void BXDM_PPCB_S_PtsError(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState
   )
{
   bool  bSignalPtsError;

   BDBG_ENTER(BXDM_PPCB_S_PtsError);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pLocalState );

   /* First check that a callback function has been registered */
   bSignalPtsError = ( NULL != hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_ePTSError].stFunction.fPTSError );

   /* Then check that the callback mask bit is set. */
   bSignalPtsError &= ( hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_ePTSError].bEnable == true );

   /* Finally check that the callback criteria has been met. */
   bSignalPtsError &= ( (pLocalState->bPtsError == true) || (pLocalState->bForcePtsError == true));

   if (pLocalState->bForcePtsError)
   {
      /* When we are forced to send a PTS Error, we always use the PTS
       * of the currently selected picture */

      BXVD_DBG_MSG(hXdmPP, ("%x:[%02x.xxx] Force PTS Error",
                                 hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                                 BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ) ));

      /* Get PTS for current element */
      BXDM_PPQM_P_GetPts(
         &hXdmPP->stDMState.stChannel.stSelectedPicture,
         BXDM_PictureProvider_P_PTSIndex_eActual,
         hXdmPP->stDMState.stChannel.stSelectedPicture.stPicParms.stTSM.stDynamic.uiTSMSelectedElement,
         &hXdmPP->stDMState.stDecode.stErrorPts.ui32RunningPTS);

      BXDM_PPQM_P_GetPtsType(
         &hXdmPP->stDMState.stChannel.stSelectedPicture,
         &hXdmPP->stDMState.stDecode.stErrorPts.ePTSType);
   }

   /* If all the conditions have been met, execute the callback */
   if( bSignalPtsError )
   {
      pLocalState->uiCallbackTriggered |= BXDM_PPDBG_Callback_PtsError;

      BXVD_DBG_MSG(hXdmPP, ("%x:[%02x.xxx] BXDM_PictureProvider_Callback_ePtsError(%08x)",
                                 hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                                 BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ),
                                 hXdmPP->stDMState.stDecode.stErrorPts.ui32RunningPTS ));

      hXdmPP->stDMState.stDecode.stErrorPts.uiSTCSnapShot = pLocalState->uiAdjustedStc;
      hXdmPP->stDMState.stDecode.stErrorPts.uiPicturesDecodedCount = hXdmPP->stDMStatus.stCounters.uiPicturesDecodedCount;
      hXdmPP->stDMState.stDecode.stErrorPts.uiDisplayManagerDroppedCount = hXdmPP->stDMStatus.stCounters.uiDisplayManagerDroppedCount;


      /* Snapshot the start time. */
      BXDM_PPTMR_P_SnapshotCallbackStartTime( hXdmPP, BXDM_PictureProvider_Callback_ePTSError );

      hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_ePTSError].stFunction.fPTSError (
                                 hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_ePTSError].pPrivateContext,
                                 hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_ePTSError].iPrivateParam,
                                 &hXdmPP->stDMState.stDecode.stErrorPts
                                 ) ;

      /* Snapshot the end time. */
      BXDM_PPTMR_P_SnapshotCallbackEndTime( hXdmPP, BXDM_PictureProvider_Callback_ePTSError );

   }

   BDBG_LEAVE(BXDM_PPCB_S_PtsError);

   return;

}  /* end of BXDM_PPCB_S_PtsError() */

static void BXDM_PPCB_S_PtsStcOffset(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState
   )
{
   bool bSignalPtsStcOffset;

   BDBG_ENTER(BXDM_PPCB_S_PtsStcOffset);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pLocalState );

   /* First check that a callback function has been registered */
   bSignalPtsStcOffset = ( NULL != hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eStcPtsOffset].stFunction.fStcPtsOffset );

   /* Then check that the callback mask bit is set. */
   bSignalPtsStcOffset &= ( hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eStcPtsOffset].bEnable == true );

   /* Finally check that the callback criteria has been met. */
   bSignalPtsStcOffset &= (pLocalState->bPtsStcOffset == true );

   /* If all the conditions have been met, execute the callback */

   if ( bSignalPtsStcOffset )
   {
      /* SW7405-3774: indicate that the callback has been generated for this picture. */
      hXdmPP->stDMState.stDecode.bExecutedInitialPtsStcOffsetCB = true;

      pLocalState->uiCallbackTriggered |= BXDM_PPDBG_Callback_PtsStcOffset;

      BXVD_DBG_MSG(hXdmPP, ("%x:[%02x.xxx] BXDM_PictureProvider_Callback_eStcPtsOffset(%08x)",
                hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ),
                hXdmPP->stDMState.stDecode.uiStcPtsDeltaSentToApp ));

      /* Snapshot the start time. */
      BXDM_PPTMR_P_SnapshotCallbackStartTime( hXdmPP, BXDM_PictureProvider_Callback_eStcPtsOffset );

      hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eStcPtsOffset].stFunction.fStcPtsOffset (
                                 hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eStcPtsOffset].pPrivateContext,
                                 hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eStcPtsOffset].iPrivateParam,
                                 &hXdmPP->stDMState.stDecode.uiStcPtsDeltaSentToApp
                                 ) ;

      /* Snapshot the end time. */
      BXDM_PPTMR_P_SnapshotCallbackEndTime( hXdmPP, BXDM_PictureProvider_Callback_eStcPtsOffset );

   }

   BDBG_LEAVE(BXDM_PPCB_S_PtsStcOffset);

   return;

}   /* end of BXDM_PPCB_S_PtsStcOffset() */


static void BXDM_PPCB_S_IFrame(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState
   )
{
   bool bSignalIFrame;

   BDBG_ENTER(BXDM_PPCB_S_IFrame);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pLocalState );

   /* First check that a callback function has been registered */
   bSignalIFrame = ( NULL != hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eIFrame].stFunction.fIFrame );

   /* Then check that the callback mask bit is set. */
   bSignalIFrame &= ( true == hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eIFrame].bEnable );

   /* Finally check that the callback criteria has been met. */
   bSignalIFrame &= ( true == pLocalState->bIFrameReceived );

   /* If all the conditions have been met, execute the callback. */
   if( bSignalIFrame )
   {
      pLocalState->uiCallbackTriggered |= BXDM_PPDBG_Callback_IFrame;

      /* Clear the interrup mask bit so that the callback is only
       * executed once per BXVD_StartDecode() call.
       */
      hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eIFrame].bEnable = false;

      BXVD_DBG_MSG(hXdmPP, ("%x:[%02x.xxx] BXDM_PictureProvider_Callback_eIFrame(%d)",
                hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ),
                hXdmPP->stDMStatus.bIPictureFound ));

      /* Snapshot the start time. */
      BXDM_PPTMR_P_SnapshotCallbackStartTime( hXdmPP, BXDM_PictureProvider_Callback_eIFrame );

      hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eIFrame].stFunction.fIFrame (
                                 hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eIFrame].pPrivateContext,
                                 hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eIFrame].iPrivateParam,
                                 &hXdmPP->stDMStatus.bIPictureFound
                                 ) ;

      /* Snapshot the end time. */
      BXDM_PPTMR_P_SnapshotCallbackEndTime( hXdmPP, BXDM_PictureProvider_Callback_eIFrame );
   }


   BDBG_LEAVE(BXDM_PPCB_S_IFrame);

   return;

}    /* end of BXDM_PPCB_S_IFrame() */

static void BXDM_PPCB_S_PictureParameters(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState
   )
{
   bool bSignalPictureParams;

   BDBG_ENTER(BXDM_PPCB_S_PictureParameters);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pLocalState );

   /* First check that a callback function has been registered */
   bSignalPictureParams = ( NULL != hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_ePictureParameters].stFunction.fPictureParameters );

   /* Then check that the callback mask bit is set. */
   bSignalPictureParams &= ( true == hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_ePictureParameters].bEnable );

   /* Always generate once the first picture has been seen  */
   bSignalPictureParams &= ( true == hXdmPP->stDMState.stDecode.bFirstPPBSeen );

   /* If all the conditions have been met, execute the callback. */
   if ( bSignalPictureParams )
   {
      pLocalState->uiCallbackTriggered |= BXDM_PPDBG_Callback_PictureParameters;

#if 0
      BDBG_DISPMGR_MSG(("BXDM_PPCB_S_PictureParameters::"));

      BKNI_Printf("%x:[%02x.xxx] PPCB: %d x %d (%d)\n",
                  hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                  BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ),
                  hXdmPP->stDMContextState.PictureParameterInfo.ulDisplayHorizontalSize,
                  hXdmPP->stDMContextState.PictureParameterInfo.ulDisplayVerticalSize,
                  hXdmPP->stDMContextState.PictureParameterInfo.eFrameRateCode);
#endif

      /* Snapshot the start time. */
      BXDM_PPTMR_P_SnapshotCallbackStartTime( hXdmPP, BXDM_PictureProvider_Callback_ePictureParameters );

      hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_ePictureParameters].stFunction.fPictureParameters (
                                 hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_ePictureParameters].pPrivateContext,
                                 hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_ePictureParameters].iPrivateParam,
                                 &hXdmPP->stDMStatus.stPictureParameterInfo
                                 ) ;

      /* Snapshot the end time. */
      BXDM_PPTMR_P_SnapshotCallbackEndTime( hXdmPP, BXDM_PictureProvider_Callback_ePictureParameters );
   }

   BDBG_LEAVE(BXDM_PPCB_S_PictureParameters);

   return;

}    /* end of BXDM_PPCB_S_PictureParameters() */

static void BXDM_PPCB_S_TsmPassInASTMMode(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState
   )
{
   bool bSignalASTMPass;

   BDBG_ENTER(BXDM_PPCB_S_TsmPassInASTMMode);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pLocalState );

   /* First check that a callback function has been registered */
   bSignalASTMPass = ( NULL != hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eTSMPassInASTMMode].stFunction.fTSMPassInASTMMode );

   /* Then check that the callback mask bit is set. */
   bSignalASTMPass &= ( true == hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eTSMPassInASTMMode].bEnable );

   /* Finally check that the callback criteria has been met. */
   bSignalASTMPass &= ( true == pLocalState->bASTMModeTSMPass );

   /* If all the conditions have been met, execute the callback. */
   if( bSignalASTMPass )
   {
      pLocalState->uiCallbackTriggered |= BXDM_PPDBG_Callback_TsmPassInASTMMode;

#if 0
      BDBG_DISPMGR_MSG(("BXDM_PPCB_S_TsmPassInASTMMode::"));
#endif
      /* Snapshot the start time. */
      BXDM_PPTMR_P_SnapshotCallbackStartTime( hXdmPP, BXDM_PictureProvider_Callback_eTSMPassInASTMMode );


      hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eTSMPassInASTMMode].stFunction.fTSMPassInASTMMode (
                                 hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eTSMPassInASTMMode].pPrivateContext,
                                 hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eTSMPassInASTMMode].iPrivateParam,
                                 &hXdmPP->stDMStatus.stCurrentPTS
                                 ) ;

      /* Snapshot the end time. */
      BXDM_PPTMR_P_SnapshotCallbackEndTime( hXdmPP, BXDM_PictureProvider_Callback_eTSMPassInASTMMode );

   }

   BDBG_LEAVE(BXDM_PPCB_S_TsmPassInASTMMode);

   return;

}    /* end of BXDM_PPCB_S_TsmPassInASTMMode() */

static void BXDM_PPCB_S_PictureUnderEvaluation(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState
   )
{
   bool bSignalPictureUnderEvaluation;
   BXDM_PictureProvider_P_Picture_Context *pstPictureUnderEvaluation = BXDM_PPQM_P_GetFirstPictureContext(hXdmPP);

   BDBG_ENTER(BXDM_PPCB_S_PictureUnderEvaluation);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pLocalState );

   /* First check that a callback function has been registered */
   bSignalPictureUnderEvaluation = ( NULL != hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_ePictureUnderEvaluation].stFunction.fPictureUnderEvaluation );

   /* Then check that the callback mask bit is set. */
   bSignalPictureUnderEvaluation &= ( true == hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_ePictureUnderEvaluation].bEnable );

   /* Finally check that the callback criteria has been met. */
   bSignalPictureUnderEvaluation &= ( true == pLocalState->bPictureUnderEvaluation );

   /* If all the conditions have been met, execute the callback. */
   if( bSignalPictureUnderEvaluation )
   {
      pLocalState->uiCallbackTriggered |= BXDM_PPDBG_Callback_PictureUnderEvaluation;

#if 0
      BDBG_DISPMGR_MSG(("BXDM_PPCB_S_PictureUnderEvaluation::"));
#endif
      /* Snapshot the start time. */
      BXDM_PPTMR_P_SnapshotCallbackStartTime( hXdmPP, BXDM_PictureProvider_Callback_ePictureUnderEvaluation );

      hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_ePictureUnderEvaluation].stFunction.fPictureUnderEvaluation (
                                 hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_ePictureUnderEvaluation].pPrivateContext,
                                 hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_ePictureUnderEvaluation].iPrivateParam,
                                 pstPictureUnderEvaluation->pstUnifiedPicture
                                 ) ;

      /* Snapshot the end time. */
      BXDM_PPTMR_P_SnapshotCallbackEndTime( hXdmPP, BXDM_PictureProvider_Callback_ePictureUnderEvaluation );

   }

   BDBG_LEAVE(BXDM_PPCB_S_PictureUnderEvaluation);

   return;

}    /* end of BXDM_PPCB_S_PictureUnderEvaluation() */

static void BXDM_PPCB_S_RequestSTC(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState
   )
{
   bool bSignalRequestStc;
   BXDM_PictureProvider_PTSInfo stPTSInfo;

   BDBG_ENTER(BXDM_PPCB_S_RequestSTC);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pLocalState );

   /* First check that a callback function has been registered */
   bSignalRequestStc = ( NULL != hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eRequestSTC].stFunction.fRequestSTC );

   /* Then check that the callback mask bit is set. */
   bSignalRequestStc &= ( true == hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eRequestSTC].bEnable );

   /* Finally check that the callback criteria has been met. */
   if ( ( true == hXdmPP->stDMState.stChannel.stClipInfo.stCurrentClip.stTime.bValid )
        && ( false == hXdmPP->stDMState.stChannel.stClipInfo.stCurrentClip.stDisplayEvent.bOccurred )
        && ( false == hXdmPP->stDMState.stChannel.stClipInfo.stCurrentClip.stRequestEvent.bOccurred ) )

   {
      bSignalRequestStc &= ( true == hXdmPP->stDMState.stChannel.stClipInfo.stCurrentClip.stStartEvent.bOccurred );

      BKNI_Memset( &stPTSInfo, 0, sizeof ( BXDM_PictureProvider_PTSInfo ) );
      stPTSInfo.ui32RunningPTS = hXdmPP->stDMState.stChannel.stClipInfo.stCurrentClip.stStartEvent.uiTime;

      if ( true == bSignalRequestStc )
      {
         hXdmPP->stDMState.stChannel.stClipInfo.stCurrentClip.stRequestEvent.bOccurred = true;
         hXdmPP->stDMState.stChannel.stClipInfo.stCurrentClip.stRequestEvent.uiTime = stPTSInfo.ui32RunningPTS;
      }
   }
   else
   {
      bSignalRequestStc &= ( ( BXDM_PictureProvider_PTSType_eCoded == hXdmPP->stDMStatus.stCurrentPTS.ePTSType )
                             || ( BXDM_PictureProvider_PTSType_eInterpolatedFromValidPTS == hXdmPP->stDMStatus.stCurrentPTS.ePTSType ) );
      stPTSInfo = hXdmPP->stDMStatus.stCurrentPTS;
   }

   /* If all the conditions have been met, execute the callback. */
   if ( bSignalRequestStc )
   {
      pLocalState->uiCallbackTriggered |= BXDM_PPDBG_Callback_RequestSTC;

      hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eRequestSTC].bEnable = false;

      BXVD_DBG_MSG(hXdmPP, ("%x:[%02x.xxx] BXDM_PictureProvider_Callback_eRequestSTC(%08x)",
                hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ),
                stPTSInfo.ui32RunningPTS ));

      /* Snapshot the start time. */
      BXDM_PPTMR_P_SnapshotCallbackStartTime( hXdmPP, BXDM_PictureProvider_Callback_eRequestSTC );

      hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eRequestSTC].stFunction.fRequestSTC (
         hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eRequestSTC].pPrivateContext,
         hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eRequestSTC].iPrivateParam,
         &stPTSInfo
         );

      /* Snapshot the end time. */
      BXDM_PPTMR_P_SnapshotCallbackEndTime( hXdmPP, BXDM_PictureProvider_Callback_eRequestSTC );

   }

   BDBG_LEAVE(BXDM_PPCB_S_RequestSTC);

   return;

}    /* end of BXDM_PPCB_S_RequestSTC() */

static void BXDM_PPCB_S_ClipStart(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState
   )
{
   bool bSignalClipStart;

   BDBG_ENTER(BXDM_PPCB_S_ClipStart);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pLocalState );

   /* First check that a callback function has been registered */
   bSignalClipStart = ( NULL != hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eClipStart].stFunction.fClipStart );

   /* Check that the callback was triggered. */
   bSignalClipStart &= ( true == pLocalState->stCallbackInfo.stClipStart.bTriggered );

   /* If all the conditions have been met, execute the callback. */
   if ( bSignalClipStart )
   {
      pLocalState->uiCallbackTriggered |= BXDM_PPDBG_Callback_ClipStart;

      BXVD_DBG_MSG(hXdmPP, ("%x:[%02x.xxx] BXDM_PictureProvider_Callback_eClipStart(%08x)",
                                 hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                                 BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ),
                                 &pLocalState->stCallbackInfo.stClipStart.stClipEventInfo ));

      /* Snapshot the start time. */
      BXDM_PPTMR_P_SnapshotCallbackStartTime( hXdmPP, BXDM_PictureProvider_Callback_eClipStart );

      hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eClipStart].stFunction.fClipStart (
         hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eClipStart].pPrivateContext,
         hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eClipStart].iPrivateParam,
         &pLocalState->stCallbackInfo.stClipStart.stClipEventInfo
         );

      /* Snapshot the end time. */
      BXDM_PPTMR_P_SnapshotCallbackEndTime( hXdmPP, BXDM_PictureProvider_Callback_eClipStart );

   }

   BDBG_LEAVE(BXDM_PPCB_S_ClipStart);

   return;

}    /* end of BXDM_PPCB_S_ClipStart() */

static void BXDM_PPCB_S_ClipStop(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState
   )
{
   bool bSignalClipStop;

   BDBG_ENTER(BXDM_PPCB_S_ClipStop);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pLocalState );

   /* First check that a callback function has been registered */
   bSignalClipStop = ( NULL != hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eClipStop].stFunction.fClipStop );

   /* Check that the callback was triggered. */
   bSignalClipStop &= ( true == pLocalState->stCallbackInfo.stClipStop.bTriggered );

   /* If all the conditions have been met, execute the callback. */
   if ( bSignalClipStop )
   {
      pLocalState->uiCallbackTriggered |= BXDM_PPDBG_Callback_ClipStop;

      BXVD_DBG_MSG(hXdmPP, ("%x:[%02x.xxx] BXDM_PictureProvider_Callback_eClipStop(%08x)",
                                 hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                                 BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ),
                                 &pLocalState->stCallbackInfo.stClipStop.stClipEventInfo ));

      /* Snapshot the start time. */
      BXDM_PPTMR_P_SnapshotCallbackStartTime( hXdmPP, BXDM_PictureProvider_Callback_eClipStop );

      hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eClipStop].stFunction.fClipStop (
         hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eClipStop].pPrivateContext,
         hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eClipStop].iPrivateParam,
         &pLocalState->stCallbackInfo.stClipStop.stClipEventInfo
         );

      /* Snapshot the end time. */
      BXDM_PPTMR_P_SnapshotCallbackEndTime( hXdmPP, BXDM_PictureProvider_Callback_eClipStop );

   }

   BDBG_LEAVE(BXDM_PPCB_S_ClipStop);

   return;

}    /* end of BXDM_PPCB_S_ClipStop() */

static void BXDM_PPCB_S_PictureMarker(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState
   )
{
   bool bSignalPictureMarker;

   BDBG_ENTER(BXDM_PPCB_S_PictureMarker);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pLocalState );

   /* First check that a callback function has been registered */
   bSignalPictureMarker = ( NULL != hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_ePictureMarker].stFunction.fPictureMarker );

   /* Check that the callback was triggered. */
   bSignalPictureMarker &= ( true == pLocalState->stCallbackInfo.stPictureMarker.bTriggered );

   /* If all the conditions have been met, execute the callback. */
   if ( bSignalPictureMarker )
   {
      pLocalState->uiCallbackTriggered |= BXDM_PPDBG_Callback_PictureMarker;

      BXVD_DBG_MSG(hXdmPP, ("%x:[%02x.xxx] BXDM_PictureProvider_Callback_ePictureMarker(%08x)",
                                 hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                                 BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ),
                                 &pLocalState->stCallbackInfo.stPictureMarker.stClipEventInfo));

      /* Snapshot the start time. */
      BXDM_PPTMR_P_SnapshotCallbackStartTime( hXdmPP, BXDM_PictureProvider_Callback_ePictureMarker );

      hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_ePictureMarker].stFunction.fPictureMarker (
         hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_ePictureMarker].pPrivateContext,
         hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_ePictureMarker].iPrivateParam,
         &pLocalState->stCallbackInfo.stPictureMarker.stClipEventInfo
         );

      /* Snapshot the end time. */
      BXDM_PPTMR_P_SnapshotCallbackEndTime( hXdmPP, BXDM_PictureProvider_Callback_ePictureMarker );

   }

   BDBG_LEAVE(BXDM_PPCB_S_PictureMarker);

   return;

}    /* end of BXDM_PPCB_S_PictureMarker() */


static void BXDM_PPCB_S_DecodeError(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState
   )
{
   bool bSignalDecodeError;
   BXDM_PictureProvider_Callback_DecodeErrorInfo stDecodeErrorInfo;

   BDBG_ENTER(BXDM_PPCB_S_DecodeError);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pLocalState );

   /* First check that a callback function has been registered */
   bSignalDecodeError = ( NULL != hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eDecodeError].stFunction.fDecodeError );

   /* Then check that the callback mask bit is set. */
   bSignalDecodeError &= ( true == hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eDecodeError].bEnable );

   /* Finally check that the callback criteria has been met. */
   bSignalDecodeError &= ( true == pLocalState->bDecodeError );

   /* If all the conditions have been met, execute the callback. */
   if( bSignalDecodeError )
   {
      BKNI_Memset( &stDecodeErrorInfo, 0, sizeof ( BXDM_PictureProvider_Callback_DecodeErrorInfo ) );

      pLocalState->uiCallbackTriggered |= BXDM_PPDBG_Callback_DecodeError;

      BXVD_DBG_MSG(hXdmPP, ("%x:[%02x.xxx] BXDM_PPCB_S_DecodeError(%d)",
                hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ),
                hXdmPP->stDMStatus.stCounters.uiDecodeErrorCount ));

      /* Snapshot the start time. */
      BXDM_PPTMR_P_SnapshotCallbackStartTime( hXdmPP, BXDM_PictureProvider_Callback_eDecodeError );

      stDecodeErrorInfo.uiDecodeErrorCount = hXdmPP->stDMStatus.stCounters.uiDecodeErrorCount;

      hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eDecodeError].stFunction.fDecodeError (
                                 hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eDecodeError].pPrivateContext,
                                 hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eDecodeError].iPrivateParam,
                                 &stDecodeErrorInfo
                                 ) ;

      /* Snapshot the end time. */
      BXDM_PPTMR_P_SnapshotCallbackEndTime( hXdmPP, BXDM_PictureProvider_Callback_eDecodeError );

   }


   BDBG_LEAVE(BXDM_PPCB_S_DecodeError);

   return;

}    /* end of BXDM_PPCB_S_DecodeError() */

static void BXDM_PPCB_S_TSMResult(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState
   )
{
   bool bSignalTSMResult;
   BXDM_PictureProvider_P_Picture_Context *pstPictureUnderEvaluation = BXDM_PPQM_P_GetFirstPictureContext(hXdmPP);

   BDBG_ENTER(BXDM_PPCB_S_TSMResult);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pLocalState );

   /* First check that a callback function has been registered */
   bSignalTSMResult = ( NULL != hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eTSMResult].stFunction.fTSMResult );

   /* Then check that the callback mask bit is set. */
   bSignalTSMResult &= ( true == hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eTSMResult].bEnable );

   /* If all the conditions have been met, execute the callback. */
   if( bSignalTSMResult )
   {
      BXDM_PictureProvider_TSMInfo stTSMInfo;

      BKNI_Memset(&stTSMInfo, 0, sizeof(BXDM_PictureProvider_TSMInfo));

      stTSMInfo.eTSMResult = pstPictureUnderEvaluation->stPicParms.stTSM.stDynamic.eTsmResult;
      stTSMInfo.ePictureHandlingMode = pstPictureUnderEvaluation->stPicParms.stTSM.stDynamic.ePictureHandlingMode;

      pLocalState->uiCallbackTriggered |= BXDM_PPDBG_Callback_TSMResult;

#if 0
      BDBG_DISPMGR_MSG(("BXDM_PPCB_S_TSMResult::"));
#endif

      /* Snapshot the start time. */
      BXDM_PPTMR_P_SnapshotCallbackStartTime( hXdmPP, BXDM_PictureProvider_Callback_eTSMResult );

      hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eTSMResult].stFunction.fTSMResult (
                                 hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eTSMResult].pPrivateContext,
                                 hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eTSMResult].iPrivateParam,
                                 &stTSMInfo
                                 ) ;

      /* Snapshot the end time. */
      BXDM_PPTMR_P_SnapshotCallbackEndTime( hXdmPP, BXDM_PictureProvider_Callback_eTSMResult );


      /* Override this picture's picture handling mode based on the
       * app's request */
      pstPictureUnderEvaluation->stPicParms.stTSM.stDynamic.ePictureHandlingMode = stTSMInfo.ePictureHandlingMode;
   }

   BDBG_LEAVE(BXDM_PPCB_S_TSMResult);

   return;

}    /* end of BXDM_PPCB_S_TSMResult() */

static void BXDM_PPCB_S_PrintExtensionData(
   BXDM_PictureProvider_Handle hXdmPP,
   const BXDM_Picture_ExtensionInfo * pstExtensionInfo
   )
{
   uint32_t i;

   for( i=0; i < pstExtensionInfo->uiCount; i++ )
   {
      switch( pstExtensionInfo->astExtensionData[i].eType )
      {
         case BXDM_Picture_ExtensionType_eBluRay3DGraphicsOffset:
            if ( pstExtensionInfo->astExtensionData[i].data.stBluRay3DGraphicsOffset.uiCount )
            {
               BXVD_DBG_MSG(hXdmPP, ("MVC graphics offset:: uiCount %d puiOffsetData 0x%x *puiOffsetData %d",
                     pstExtensionInfo->astExtensionData[i].data.stBluRay3DGraphicsOffset.uiCount,
                     pstExtensionInfo->astExtensionData[i].data.stBluRay3DGraphicsOffset.puiOffsetData,
                     *pstExtensionInfo->astExtensionData[i].data.stBluRay3DGraphicsOffset.puiOffsetData
                     ));
            }
            else
            {
               BXVD_DBG_MSG(hXdmPP, ("MVC graphics offset:: uiCount %d puiOffsetData 0x%x",
                     pstExtensionInfo->astExtensionData[i].data.stBluRay3DGraphicsOffset.uiCount,
                     pstExtensionInfo->astExtensionData[i].data.stBluRay3DGraphicsOffset.puiOffsetData
                     ));
            }
            break;

         case BXDM_Picture_ExtensionType_eSEIMsg_FramePacking:
         {
            /* SW7405-4500: the following needs to be two lines to avoid a compiler warning when DEBUG=n */

            BXDM_Picture_Extension_SEIFramePacking * pstSEIFramePacking;
            pstSEIFramePacking = pstExtensionInfo->astExtensionData[i].data.stSEIFramePacking.pstSeiData;

            BXVD_DBG_MSG(hXdmPP, ("SEIFramePacking flag:0x%x msgFlag:0x%x msgData: %d %d %d %d %d %d %d %d %d",
                     pstExtensionInfo->astExtensionData[i].data.stSEIFramePacking.uiFlags,
                     pstSEIFramePacking->uiFlags,
                     pstSEIFramePacking->uiFramePackingArrangementId,
                     pstSEIFramePacking->uiFramePackingArrangementType,
                     pstSEIFramePacking->uiContentInterpretationType,
                     pstSEIFramePacking->uiFrame0GridPositionX,
                     pstSEIFramePacking->uiFrame0GridPositionY,
                     pstSEIFramePacking->uiFrame1GridPositionX,
                     pstSEIFramePacking->uiFrame1GridPositionY,
                     pstSEIFramePacking->uiFramePackingArrangementReservedByte,
                     pstSEIFramePacking->uiFramePackingArrangementRepetitionPeriod
                     ));
            break;
         }

         case BXDM_Picture_ExtensionType_eSEIFramePacking:  /* DEPRECATED: SW7405-4560: use BXDM_Picture_ExtensionType_eSEIMsg_FramePacking */
         default:
            BXVD_DBG_MSG(hXdmPP, ("\t eType %d", pstExtensionInfo->astExtensionData[i].eType ));
            break;

      }     /* end of switch( eType )*/
   }        /* end of for( pstExtensionInfo->uiCount ) */

   if ( NULL != pstExtensionInfo->pNext )
   {
      BXDM_PPCB_S_PrintExtensionData( hXdmPP, pstExtensionInfo->pNext );
   }

   return;

}     /* end of BXDM_PPCB_S_PrintExtensionData */

static void BXDM_PPCB_S_PictureExtensionData(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_P_LocalState* pLocalState,
         BXDM_PictureProvider_P_Picture_Context * pstPicture
   )
{
   bool bSignalExtensionData;

   BDBG_ENTER(BXDM_PPCB_S_PictureExtensionData);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pLocalState );
   BDBG_ASSERT( pstPicture );

   /* SW7405-4804: Typically the extension data callback will be executed when a
    * picture is delivered to VDC via the "BXDM_PPCB_P_ExecuteCallbacks" function.
    * However this does not happen for pictures that are dropped.  This case is
    * now handled by calling "BXDM_PPCB_P_ExecuteSingleCallbackExt" whenever a
    * picture is released.  The flag "bExtensionDataDelivered" ensures that the
    * callback is only executed once per picture.
    */

   /* First check that a callback function has been registered */
   bSignalExtensionData = ( NULL != hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_ePictureExtensionData].stFunction.fPictureExtensionData );

   /* Then check that the callback mask bit is set. */
   bSignalExtensionData &= ( true == hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_ePictureExtensionData].bEnable );

   bSignalExtensionData &= ( NULL != pstPicture->pstUnifiedPicture );

   /* SW7405-4560: only deliver the data once per picture. */
   bSignalExtensionData &= ( false == pstPicture->stPicParms.stDisplay.stDynamic.bExtensionDataDelivered );

   /* If all the conditions have been met, execute the callback. */
   if( bSignalExtensionData )
   {
      pLocalState->uiCallbackTriggered |= BXDM_PPDBG_Callback_ExtensionData;

      pstPicture->stPicParms.stDisplay.stDynamic.bExtensionDataDelivered = true;

      if ( 0 != pstPicture->pstUnifiedPicture->stExtensionInfo.uiCount
            || ( NULL != pstPicture->pstUnifiedPicture->stExtensionInfo.pNext
               && 0 != pstPicture->pstUnifiedPicture->stExtensionInfo.pNext->uiCount )
         )
      {
         BXVD_DBG_MSG(hXdmPP, ("%x:[%02x.%03x] BXDM_PPCB_S_PictureExtensionData uiCount %d",
                                    hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                                    BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ),
                                    pstPicture->stPicParms.uiPPBIndex & 0xFFF,
                                    pstPicture->pstUnifiedPicture->stExtensionInfo.uiCount
                                    ));

         BXDM_PPCB_S_PrintExtensionData( hXdmPP, (BXDM_Picture_ExtensionInfo *)&(pstPicture->pstUnifiedPicture->stExtensionInfo) );
      }

      /* Snapshot the start time. */
      BXDM_PPTMR_P_SnapshotCallbackStartTime( hXdmPP, BXDM_PictureProvider_Callback_ePictureExtensionData );

      hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_ePictureExtensionData].stFunction.fPictureExtensionData (
               hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_ePictureExtensionData].pPrivateContext,
               hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_ePictureExtensionData].iPrivateParam,
               &(pstPicture->pstUnifiedPicture->stExtensionInfo)
               );

      /* Snapshot the end time. */
      BXDM_PPTMR_P_SnapshotCallbackEndTime( hXdmPP, BXDM_PictureProvider_Callback_ePictureExtensionData );

   }


   BDBG_LEAVE(BXDM_PPCB_S_PictureExtensionData);

   return;

}    /* end of BXDM_PPCB_S_PictureExtensionData() */

/* SW7425-3358: FNRT support */

static void BXDM_PPCB_S_ChunkDone(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState
   )
{
   bool bSignalChunkDone;
   BXDM_PictureProvider_Callback_ChunkDoneInfo stChunkDoneInfo;

   BDBG_ENTER(BXDM_PPCB_S_ChunkDone);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pLocalState );

   /* First check that a callback function has been registered */
   bSignalChunkDone = ( NULL != hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eChunkDone].stFunction.fChunkDone );

   /* Then check that the callback mask bit is set. */
   bSignalChunkDone &= ( true == hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eChunkDone].bEnable );

   /* Finally check that the callback criteria has been met. */
   bSignalChunkDone &= ( true == hXdmPP->stDMState.stDecode.stNonRealTime.bChunkDone );

   /* If all the conditions have been met, execute the callback. */
   if( bSignalChunkDone )
   {
      BKNI_Memset( &stChunkDoneInfo, 0, sizeof ( BXDM_PictureProvider_Callback_ChunkDoneInfo ) );

      pLocalState->uiCallbackTriggered |= BXDM_PPDBG_Callback_ChunkDone;

      stChunkDoneInfo.uiChunkId = hXdmPP->stDMState.stDecode.stNonRealTime.uiChunkId;

      /* A one shot, re-enable explicitly or by calling BXVD_StartDecode. */
      hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eChunkDone].bEnable = false;

      BXVD_DBG_MSG(hXdmPP, ("%x:[%02x.xxx] BXDM_PPCB_S_ChunkDone:: uiChunkId:%08x",
                hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ),
                hXdmPP->stDMState.stDecode.stNonRealTime.uiChunkId ));

      /* Snapshot the start time. */
      BXDM_PPTMR_P_SnapshotCallbackStartTime( hXdmPP, BXDM_PictureProvider_Callback_eChunkDone );

      hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eChunkDone].stFunction.fChunkDone (
                                 hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eChunkDone].pPrivateContext,
                                 hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eChunkDone].iPrivateParam,
                                 &stChunkDoneInfo
                                 ) ;

      /* Snapshot the end time. */
      BXDM_PPTMR_P_SnapshotCallbackEndTime( hXdmPP, BXDM_PictureProvider_Callback_eChunkDone );

   }

   BDBG_LEAVE(BXDM_PPCB_S_ChunkDone);

   return;

}    /* end of BXDM_PPCB_S_ChunkDone() */


static void BXDM_PPCB_S_EvaluateCriteria_PtsStcOffset(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState
   )
{
   uint32_t    uiDeltaOfDeltas;
   uint32_t    uiCurrentStcPtsDelta;
   BXDM_PictureProvider_P_Picture_Context *  pstSelectedPicture;

   BDBG_ENTER(BXDM_PPCB_S_EvaluateCriteria_PtsStcOffset);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pLocalState );

   /* initialize the local variables */
   pstSelectedPicture = &(hXdmPP->stDMState.stChannel.stSelectedPicture);
   uiCurrentStcPtsDelta = pstSelectedPicture->stPicParms.stTSM.stDynamic.iStcPtsDifferenceActual;

   /* Only consider the STC/PTS delta if
    * - the TSM result is a "BXDM_PictureProvider_P_TSMState_ePass"
    * - AND the "selection" mode for this picture is TSM.
    *   (It could be overriden to "vsync" if auto vsync mode has kicked in.)
    * - AND the context has been validated
    * - AND either
    *   - the callback has NOT been called at least once (SW7405-3774/SW7550-379)
    *   - or this element is being displayed for the first time
    */
   if( BXDM_PictureProvider_TSMResult_ePass == pstSelectedPicture->stPicParms.stTSM.stDynamic.eTsmResult
         && BXDM_PictureProvider_DisplayMode_eTSM == pstSelectedPicture->stPicParms.stTSM.stDynamic.eSelectionMode
         && true == pstSelectedPicture->bValidated
         &&  ( false == hXdmPP->stDMState.stDecode.bExecutedInitialPtsStcOffsetCB
               || false == pstSelectedPicture->stPicParms.stDisplay.stDynamic.bElementRepeated )
      )
   {

      /* "iStcPtsDifferenceActual" should always be a postive value.
       * Initially we had a bug where it was negative.
       * Keep this check for now in case an issue creeps back in.
       * TODO: eventually remove this check.
       */
      if ( pstSelectedPicture->stPicParms.stTSM.stDynamic.iStcPtsDifferenceActual < 0 )
      {
         uiCurrentStcPtsDelta = -pstSelectedPicture->stPicParms.stTSM.stDynamic.iStcPtsDifferenceActual;
         BXVD_DBG_ERR(hXdmPP, ("%x:[%02x.%03x] BXDM_PPCB_S_EvaluateCriteria_PtsStcOffset:: negative PTS/STC delta %d",
                                    hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                                    BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ),
                                    pstSelectedPicture->stPicParms.uiPPBIndex & 0xFFF,
                                    pstSelectedPicture->stPicParms.stTSM.stDynamic.iStcPtsDifferenceActual ));
      }

      if ( true == pLocalState->bFICChanged )
      {
         /* Do not generate a StcPtsOffset interrupt if the Field
          * Inversion Correction has changed this vsync */
         pLocalState->bPtsStcOffset =  false;
      }
      /*
       * The following will be true the first time through after "BXVD_StartDecode()" has been called.
       */
      else if ( hXdmPP->stDMState.stDecode.uiStcPtsDeltaSentToApp == (uint32_t)(-1) )
      {
         /*
          * Always want to send the first STC/PTS delta to the application.
          */
         pLocalState->bPtsStcOffset = true;
         hXdmPP->stDMState.stDecode.uiStcPtsDeltaSentToApp = uiCurrentStcPtsDelta;
      }
      else
      {
         /* Now generate the delta of the delta's, i.e. by how much do
          * the new delta and previous one differ?
          */
         if ( hXdmPP->stDMState.stDecode.uiStcPtsDeltaSentToApp > uiCurrentStcPtsDelta )
         {
            uiDeltaOfDeltas = hXdmPP->stDMState.stDecode.uiStcPtsDeltaSentToApp - uiCurrentStcPtsDelta;
         }
         else
         {
            uiDeltaOfDeltas = uiCurrentStcPtsDelta - hXdmPP->stDMState.stDecode.uiStcPtsDeltaSentToApp;
         }

         /* TODO: in DM V1, "p_st_skip_pic->ui32NumLateOccurrence" factored
          * into the logic when generating the callback.  Is something similar still needed?
          */
         if ( uiDeltaOfDeltas > pstSelectedPicture->stPicParms.stTSM.stStatic.uiPtsVsyncDiffThreshold )
         {
            pLocalState->bPtsStcOffset =  true;
            hXdmPP->stDMState.stDecode.uiStcPtsDeltaSentToApp = uiCurrentStcPtsDelta;
         }
         else
         {
            /* TODO: is this really needed? */
            pLocalState->bPtsStcOffset =  false;
         }

      }

   }


   BDBG_LEAVE(BXDM_PPCB_S_EvaluateCriteria_PtsStcOffset);

   return;

}  /* end of BXDM_PPCB_P_EvaluateCriteria_PtsStcOffset() */


/*******************************************************************************
 *
 * Public routines.
 *
 ******************************************************************************/

void BXDM_PPCB_P_EvaluateCriteria_IFrame(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState,
   BXDM_PictureProvider_P_Picture_Context * pstSelectedPic
   )
{
   BDBG_ENTER(BXDM_PPCB_P_EvaluateCriteria_IFrame);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pLocalState );
   BDBG_ASSERT( pstSelectedPic );

   if ( true == pstSelectedPic->bValidated
         && ( NULL != pstSelectedPic->pstUnifiedPicture )
         && ( BXDM_Picture_Coding_eI == pstSelectedPic->pstUnifiedPicture->stPictureType.eCoding )
       )
   {
      hXdmPP->stDMStatus.bIPictureFound = true;
      pLocalState->bIFrameReceived = true;
   }

   BDBG_LEAVE(BXDM_PPCB_P_EvaluateCriteria_IFrame);

   return;

}    /* end of BXDM_PPCB_P_EvaluateCriteria_IFrame() */


void BXDM_PPCB_P_EnableAutomaticCallbacks(
   BXDM_PictureProvider_Handle hXdmPP
   )
{
   BDBG_ENTER(BXDM_PPCB_P_EnableAutomaticCallbacks );

   BDBG_ASSERT( hXdmPP );

   /* The following callbacks need to be enabled every time "BXVD_StartDecode"
    * or "BXVD_ProcessWatchdog" is called.
    */
   hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eStcPtsOffset].bEnable = true;
   hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eFirstPTSReady].bEnable = true;
   hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eFirstPTSPassed].bEnable = true;
   hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_ePTSError].bEnable = true;
   hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eIFrame].bEnable = true;
   hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_ePictureParameters].bEnable= true;
   hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eTSMPassInASTMMode].bEnable = true;
   hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eRequestSTC].bEnable = true;
   hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_ePictureUnderEvaluation].bEnable = true;
   hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eTSMResult].bEnable = true;
   hXdmPP->stCallbacks[BXDM_PictureProvider_Callback_eChunkDone].bEnable = true;

   BDBG_LEAVE(BXDM_PPCB_P_EnableAutomaticCallbacks );

   return;

}  /* end of BXDM_PPCB_P_EnableAutomaticCallbacks() */


void BXDM_PPCB_P_ExecuteSingleCallbackExt(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState,
   BXDM_PictureProvider_P_Picture_Context * pstPicture,
   BXDM_PictureProvider_Callback eCallbackName
   )
{
   BDBG_ENTER(BXDM_PPCB_P_ExecuteSingleCallbackExt);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pLocalState );

   switch( eCallbackName )
   {
   case BXDM_PictureProvider_Callback_eFirstPTSReady:
      BXDM_PPCB_S_FirstPtsReady( hXdmPP, pLocalState );
      break;

   case BXDM_PictureProvider_Callback_eFirstCodedPTSReady:
      BXDM_PPCB_S_FirstCodedPtsReady( hXdmPP, pLocalState );
      break;

   case BXDM_PictureProvider_Callback_eFirstPTSPassed:
      BXDM_PPCB_S_FirstPtsPassed( hXdmPP, pLocalState );
      break;

   case BXDM_PictureProvider_Callback_ePTSError:
      BXDM_PPCB_S_PtsError( hXdmPP, pLocalState );
      break;

   case BXDM_PictureProvider_Callback_eStcPtsOffset:
      BXDM_PPCB_S_PtsStcOffset( hXdmPP, pLocalState );
      break;

   case BXDM_PictureProvider_Callback_eIFrame:
      BXDM_PPCB_S_IFrame( hXdmPP, pLocalState );
      break;

   case BXDM_PictureProvider_Callback_ePictureParameters:
      BXDM_PPCB_S_PictureParameters( hXdmPP, pLocalState );
      break;

   case BXDM_PictureProvider_Callback_eTSMPassInASTMMode:
      BXDM_PPCB_S_TsmPassInASTMMode( hXdmPP, pLocalState );
      break;

   case BXDM_PictureProvider_Callback_eRequestSTC:
      BXDM_PPCB_S_RequestSTC( hXdmPP, pLocalState );
      break;

   case BXDM_PictureProvider_Callback_eClipStart:
      BXDM_PPCB_S_ClipStart( hXdmPP, pLocalState );
      break;

   case BXDM_PictureProvider_Callback_eClipStop:
      BXDM_PPCB_S_ClipStop( hXdmPP, pLocalState );
      break;

   case BXDM_PictureProvider_Callback_ePictureMarker:
      BXDM_PPCB_S_PictureMarker( hXdmPP, pLocalState );
      break;
   case BXDM_PictureProvider_Callback_ePictureUnderEvaluation:
      BXDM_PPCB_S_PictureUnderEvaluation( hXdmPP, pLocalState );
      break;

   case BXDM_PictureProvider_Callback_eDecodeError:
      BXDM_PPCB_S_DecodeError( hXdmPP, pLocalState );
      break;

   case BXDM_PictureProvider_Callback_eTSMResult:
      BXDM_PPCB_S_TSMResult( hXdmPP, pLocalState );
      break;

   case BXDM_PictureProvider_Callback_ePictureExtensionData:
      BXDM_PPCB_S_PictureExtensionData( hXdmPP, pLocalState, pstPicture );
      break;

   case BXDM_PictureProvider_Callback_eChunkDone:
      BXDM_PPCB_S_ChunkDone( hXdmPP, pLocalState );
      break;

   default:
      break;

   }

   BDBG_LEAVE(BXDM_PPCB_P_ExecuteSingleCallbackExt);

   return;

 }    /* end of BXDM_PPCB_P_ExecuteSingleCallbackExt() */


void BXDM_PPCB_P_ExecuteSingleCallback(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState,
   BXDM_PictureProvider_Callback eCallbackName
   )
{
   BDBG_ENTER(BXDM_PPCB_P_ExecuteSingleCallback);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pLocalState );

   BXDM_PPCB_P_ExecuteSingleCallbackExt( hXdmPP, pLocalState, NULL, eCallbackName );

   BDBG_LEAVE(BXDM_PPCB_P_ExecuteSingleCallback);

   return;

 }    /* end of BXDM_PPCB_P_ExecuteSingleCallback() */

void BXDM_PPCB_P_ExecuteCallbacks(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState
   )
{
   BDBG_ENTER(BXDM_PPCB_P_ExecuteCallbacks);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pLocalState );

   /* Has the STC/PTS delta changed enough that a callback is needed?
    * Note: This routine can be called anytime after the picture element has been selected.
    * Call it from here just to keep the routine local to this file.
    */
   BXDM_PPCB_S_EvaluateCriteria_PtsStcOffset( hXdmPP, pLocalState );

   BXDM_PPCB_P_ExecuteSingleCallbackExt( hXdmPP, pLocalState, NULL, BXDM_PictureProvider_Callback_eClipStop );
   BXDM_PPCB_P_ExecuteSingleCallbackExt( hXdmPP, pLocalState, NULL, BXDM_PictureProvider_Callback_ePictureMarker );
   BXDM_PPCB_P_ExecuteSingleCallbackExt( hXdmPP, pLocalState, NULL, BXDM_PictureProvider_Callback_eClipStart );
   BXDM_PPCB_P_ExecuteSingleCallbackExt( hXdmPP, pLocalState, NULL, BXDM_PictureProvider_Callback_eRequestSTC );
   BXDM_PPCB_P_ExecuteSingleCallbackExt( hXdmPP, pLocalState, NULL, BXDM_PictureProvider_Callback_eFirstPTSReady );
   BXDM_PPCB_P_ExecuteSingleCallbackExt( hXdmPP, pLocalState, NULL, BXDM_PictureProvider_Callback_eFirstCodedPTSReady );
   BXDM_PPCB_P_ExecuteSingleCallbackExt( hXdmPP, pLocalState, NULL, BXDM_PictureProvider_Callback_eFirstPTSPassed );
   BXDM_PPCB_P_ExecuteSingleCallbackExt( hXdmPP, pLocalState, NULL, BXDM_PictureProvider_Callback_ePTSError );
   BXDM_PPCB_P_ExecuteSingleCallbackExt( hXdmPP, pLocalState, NULL, BXDM_PictureProvider_Callback_eStcPtsOffset );
   BXDM_PPCB_P_ExecuteSingleCallbackExt( hXdmPP, pLocalState, NULL, BXDM_PictureProvider_Callback_eIFrame );
   BXDM_PPCB_P_ExecuteSingleCallbackExt( hXdmPP, pLocalState, NULL, BXDM_PictureProvider_Callback_ePictureParameters );
   BXDM_PPCB_P_ExecuteSingleCallbackExt( hXdmPP, pLocalState, NULL, BXDM_PictureProvider_Callback_eTSMPassInASTMMode );
   BXDM_PPCB_P_ExecuteSingleCallbackExt( hXdmPP, pLocalState, NULL, BXDM_PictureProvider_Callback_eDecodeError );
   BXDM_PPCB_P_ExecuteSingleCallbackExt( hXdmPP, pLocalState, NULL, BXDM_PictureProvider_Callback_eChunkDone );

   /* SW7422-72: the "ePictureExtensionData" callback is now executed in BXDM_PPOUT_S_3DFixup.
    * This is to allow the middleware/application a chance to call BXDM_PictureProvider_Set3D_isr
    * before the MFD picture structure is filled in.
    */
#if 0
   BXDM_PPCB_P_ExecuteSingleCallbackExt(
               hXdmPP,
               pLocalState,
               &(hXdmPP->stDMState.stChannel.stSelectedPicture),
               BXDM_PictureProvider_Callback_ePictureExtensionData
               );
#endif
   BXDM_PPDBG_P_CallbackTriggeredLog(hXdmPP, pLocalState->uiCallbackTriggered);


   BDBG_LEAVE(BXDM_PPCB_P_ExecuteCallbacks);

   return;

}  /* end of BXDM_PPCB_P_ExecuteCallBacks() */



