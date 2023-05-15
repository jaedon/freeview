/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxdm_pp_timer.c $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 9/11/12 1:02p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/xdm/bxdm_pp_timer.c $
 * 
 * Hydra_Software_Devel/7   9/11/12 1:02p btosi
 * SW7405-4736: (Enhance XDM debug capabilities) standardized the instance
 * Id in the debug messages
 * 
 * Hydra_Software_Devel/6   7/6/12 10:40a btosi
 * SW7425-3358: added support for FNRT
 * 
 * Hydra_Software_Devel/5   8/12/10 4:28p btosi
 * SW7405-4736: moved location of BXDM_PPTMR_lutVsyncsPersSecond
 * 
 * Hydra_Software_Devel/4   8/12/10 9:38a btosi
 * SW7405-4736: modified QM debug messages
 * 
 * Hydra_Software_Devel/3   6/29/10 11:25a btosi
 * SW7405-4365: revamped the timer code to selectively enable the function
 * and callback timing
 * 
 * Hydra_Software_Devel/2   3/1/10 5:10p nilesh
 * SW7405-2993: Fixed coverity warnings
 * 
 * Hydra_Software_Devel/1   2/16/10 10:51a nilesh
 * SW7405-2993: Initial XDM version
 *
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "bdbg.h"

#include "bxdm_pp.h"
#include "bxdm_pp_priv.h"
#include "bxdm_pp_timer.h"

BDBG_MODULE(BXDM_PPTMR);

const char BXDM_PP_P_DISPMGR_TIMER_NODE[]="DMDBG:\t""$brcm_Revision: Hydra_Software_Devel/7 $";

const uint32_t BXDM_PPTMR_lutVsyncsPersSecond[BXDM_PictureProvider_MonitorRefreshRate_eMax]=
{
   60,   /* BXDM_PictureProvider_MonitorRefreshRate_eUnknown */
   24,   /* BXDM_PictureProvider_MonitorRefreshRate_e23_97 */
   24,   /* BXDM_PictureProvider_MonitorRefreshRate_e24 */
   25,   /* BXDM_PictureProvider_MonitorRefreshRate_e25 */
   30,   /* BXDM_PictureProvider_MonitorRefreshRate_e29_97 */
   30,   /* BXDM_PictureProvider_MonitorRefreshRate_e30 */
   50,   /* BXDM_PictureProvider_MonitorRefreshRate_e50 */
   60,   /* BXDM_PictureProvider_MonitorRefreshRate_e59_94 */
   60,   /* BXDM_PictureProvider_MonitorRefreshRate_e60 */
   48    /* BXDM_PictureProvider_MonitorRefreshRate_e48 */

}; /* end of BXDM_PPTMR_lutVsyncsPersSecond */

#if BDBG_DEBUG_BUILD

#if ( BXDM_PPTMR_P_ENABLE_FUNCTION_TIMING || BXDM_PPTMR_P_ENABLE_CALLBACK_TIMING )

/*
 * Result will be printed every BXDM_PPTMR_S_VSYNCS_PER_PRINT vsync's
 * For a value of '0', results are printed once per second.
 */
#define BXDM_PPTMR_S_VSYNCS_PER_PRINT  0

/*
 * Local function prototypes
 */
void BXDM_PPTMR_S_CookResults( BXDM_PictureProvider_Handle hXdmPP );
void BXDM_PPTMR_S_ResetTimerData( BXDM_PPTIMER_P_Data * pTimerData );

/*
 * Private functions
 */

void BXDM_PPTMR_S_GetTime(
   BXDM_PictureProvider_Handle hXdmPP,
   uint32_t * pTimeUsec
   )
{
   if( NULL != hXdmPP->hTimer )
   {
      BTMR_ReadTimer_isr( hXdmPP->hTimer, pTimeUsec );
   }
   else 
   {
      *pTimeUsec = 0;
   }
   
   return;
}


void BXDM_PPTMR_S_SnapshotEndTimeAndUpdateElapse(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PPTIMER_P_Sample * pTimerSample
   )
{
   BDBG_ENTER(BXDM_PPTMR_S_SnapshotEndTimeAndUpdateElapse);

   BXDM_PPTMR_S_GetTime( hXdmPP, &(pTimerSample->uiEndTimeUsecs) );

   /* Calculate the elapse time. */
   pTimerSample->uiElapseTimeUsecs = ( pTimerSample->uiEndTimeUsecs - pTimerSample->uiStartTimeUsecs );

   /* Check if the time is a new max elapse time. */
   if ( pTimerSample->uiElapseTimeUsecs > pTimerSample->uiMaxTimeUsecs )
   {
      pTimerSample->uiMaxTimeUsecs = pTimerSample->uiElapseTimeUsecs;
   }

   /* Check if the time is a new min elapse time. */
   if ( 0 == pTimerSample->uiMinTimeUsecs )
   {
      pTimerSample->uiMinTimeUsecs = pTimerSample->uiElapseTimeUsecs;
   }
   else if ( pTimerSample->uiElapseTimeUsecs < pTimerSample->uiMinTimeUsecs )
   {
      pTimerSample->uiMinTimeUsecs = pTimerSample->uiElapseTimeUsecs;
   }

   /* Add the elapse time for this callback to the total. */
   pTimerSample->uiTotalTimeUsecs += pTimerSample->uiElapseTimeUsecs;

   pTimerSample->uiNumSamples++;

   BDBG_LEAVE(BXDM_PPTMR_S_SnapshotEndTimeAndUpdateElapse);

   return;
}


void BXDM_PPTMR_S_CookResults(
   BXDM_PictureProvider_Handle hXdmPP
   )
{
   BXDM_PPTIMER_P_Data * pTimerData = &(hXdmPP->stDMState.stDecode.stTimerData);

   uint32_t i;
   
   BDBG_ENTER(BXDM_PPTMR_S_CookResults);

#if BXDM_PPTMR_P_ENABLE_FUNCTION_TIMING

   for ( i = 0; i < BXDM_PPTIMER_P_Function_eMax; i++ )
   {
      if ( 0 != pTimerData->astFunctions[ i ].uiNumSamples )
      {
         pTimerData->astFunctions[ i ].uiAverageTimeUsecs = pTimerData->astFunctions[ i ].uiTotalTimeUsecs / pTimerData->astFunctions[ i ].uiNumSamples;
      }
   }

#endif

   for ( i=0; i < BXDM_PictureProvider_Callback_eMax; i++ )
   {
      if ( 0 != pTimerData->astCallbacks[ i ].uiNumSamples )
      {
         pTimerData->astCallbacks[ i ].uiAverageTimeUsecs = pTimerData->astCallbacks[ i ].uiTotalTimeUsecs / pTimerData->astCallbacks[ i ].uiNumSamples;
      }

   }

   BDBG_LEAVE(BXDM_PPTMR_S_CookResults);

   return;
}

#endif 

/*
 * Public functions for measuring the elapse time of functions.
 * Conditionally compiled out to reduce overhead during normal play.
 */
void BXDM_PPTMR_P_SnapshotFunctionStartTime(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PPTIMER_P_Function eFunctionIndex
   )
{
   BDBG_ENTER(BXDM_PPTMR_P_SnapshotFunctionStartTime);

#if BXDM_PPTMR_P_ENABLE_FUNCTION_TIMING

   BXDM_PPTMR_S_GetTime( 
         hXdmPP,
         &(hXdmPP->stDMState.stDecode.stTimerData.astFunctions[ eFunctionIndex ].uiStartTimeUsecs)
         );

#else
   BSTD_UNUSED( hXdmPP );
   BSTD_UNUSED( eFunctionIndex );
#endif

   BDBG_LEAVE(BXDM_PPTMR_P_SnapshotFunctionStartTime);

   return;
}

void BXDM_PPTMR_P_SnapshotFunctionEndTime(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PPTIMER_P_Function eFunctionIndex
   )
{
   BDBG_ENTER(BXDM_PPTMR_P_SnapshotFunctionEndTime);

#if BXDM_PPTMR_P_ENABLE_FUNCTION_TIMING

   BXDM_PPTMR_S_SnapshotEndTimeAndUpdateElapse(
         hXdmPP,
         &(hXdmPP->stDMState.stDecode.stTimerData.astFunctions[ eFunctionIndex ])
         );

#else
   BSTD_UNUSED( hXdmPP );
   BSTD_UNUSED( eFunctionIndex );
#endif

   BDBG_LEAVE(BXDM_PPTMR_P_SnapshotFunctionEndTime);

   return;
}

/*
 * Public functions for measuring the elapse time of callbacks.
 */

void BXDM_PPTMR_P_SnapshotCallbackStartTime(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_Callback eCallbackIndex
   )
{
   BDBG_ENTER(BXDM_PPTMR_P_SnapshotCallbackStartTime);

#if BXDM_PPTMR_P_ENABLE_CALLBACK_TIMING

   BXDM_PPTMR_S_GetTime( 
         hXdmPP,
         &(hXdmPP->stDMState.stDecode.stTimerData.astCallbacks[ eCallbackIndex ].uiStartTimeUsecs)
         );

#else
   BSTD_UNUSED( hXdmPP );
   BSTD_UNUSED( eCallbackIndex );
#endif


   BDBG_LEAVE(BXDM_PPTMR_P_SnapshotCallbackStartTime);

   return;
}

void BXDM_PPTMR_P_SnapshotCallbackEndTime(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_Callback eCallbackIndex
   )
{
   BDBG_ENTER(BXDM_PPTMR_P_SnapshotCallbackEndTime);

#if BXDM_PPTMR_P_ENABLE_CALLBACK_TIMING

   BXDM_PPTMR_S_SnapshotEndTimeAndUpdateElapse(
         hXdmPP,
         &(hXdmPP->stDMState.stDecode.stTimerData.astCallbacks[ eCallbackIndex ])
         );

#else
   BSTD_UNUSED( hXdmPP );
   BSTD_UNUSED( eCallbackIndex );
#endif


   BDBG_LEAVE(BXDM_PPTMR_P_SnapshotCallbackEndTime);

   return;
}

/*
 * Public function for printing the results.
 */
#if BXDM_PPTMR_P_ENABLE_FUNCTION_TIMING

static const char* sFunctionNameLUT[BXDM_PPTIMER_P_Function_eMax] =
{
   "Main",
   "Init",
   "DecIsr",
   "Select",
   "Pic1",
   "Pic2",
   "Pic3",
   "ValPCHead",
   "ValPCTail",
   "Eval",
   "VDC",
   "Update",
   "CallBck",
   "Post",
   "GetCnt",
   "PeekPic",
   "GetPic",
   "RelPic",
   "DrP",
   "DrR",
};

#endif

#if BXDM_PPTMR_P_ENABLE_CALLBACK_TIMING

static const char* sCallbackNameLUT[BXDM_PictureProvider_Callback_eMax] =
{
   "StcPtsOff",   /* BXDM_PictureProvider_Callback_eStcPtsOffset */
   "FirstPic",    /* BXDM_PictureProvider_Callback_eFirstPTSReady */
   "PTSCoded",    /* BXDM_PictureProvider_Callback_eFirstCodedPTSReady */
   "PTSPass",     /* BXDM_PictureProvider_Callback_eFirstPTSPassed */
   "PTSError",    /* BXDM_PictureProvider_Callback_ePTSError */
   "IFrame",      /* BXDM_PictureProvider_Callback_eIFrame */
   "PicParms",    /* BXDM_PictureProvider_Callback_ePictureParameters */
   "ASTM",        /* BXDM_PictureProvider_Callback_eTSMPassInASTMMode */
   "ClpStr",      /* BXDM_PictureProvider_Callback_eClipStart */
   "ClpStp",      /* BXDM_PictureProvider_Callback_eClipStop */
   "Marker",      /* BXDM_PictureProvider_Callback_ePictureMarker */
   "ReqStc",      /* BXDM_PictureProvider_Callback_eRequestSTC */
   "PPBParms",    /* BXDM_PictureProvider_Callback_ePictureUnderEvaluation */
   "TSM",         /* BXDM_PictureProvider_Callback_eTSMResult */
   "ExtData",     /* BXDM_PictureProvider_Callback_ePictureExtensionData */
   "DecErr",      /* BXDM_PictureProvider_Callback_eDecodeError */
   "CnkDone"      /* BXDM_PictureProvider_Callback_eChunkDone */
};

#endif

void BXDM_PPTMR_P_PrintResults(
   BXDM_PictureProvider_Handle hXdmPP
   )
{
   char           szTemp[BXDM_PP_TIMER_P_MAX_STR_LEN];
   uint32_t       uiOffset=0;
   uint32_t       i;
   uint32_t       uiVsyncThreshold;

   BXDM_PPTIMER_P_Data *         pTimerData;
   BXDM_PictureProvider_MonitorRefreshRate  eMonitorRefreshRate;
   
#if BXDM_PPTMR_P_ENABLE_FUNCTION_TIMING
   bool           bPrintFunctionTimes = false;
#endif

   BDBG_ENTER(BXDM_PPTMR_P_PrintResults);

#if ( BXDM_PPTMR_P_ENABLE_FUNCTION_TIMING || BXDM_PPTMR_P_ENABLE_CALLBACK_TIMING )

   pTimerData = &(hXdmPP->stDMState.stDecode.stTimerData);

   /* Bump the vsync count to know when to print the results. */

   pTimerData->uiVsyncCnt++;

   /* Calculate the "print" threshold.  */

   uiVsyncThreshold = BXDM_PPTMR_S_VSYNCS_PER_PRINT;

   if ( 0 == uiVsyncThreshold )
   {

      if ( hXdmPP->stDMConfig.eMonitorRefreshRate >= BXDM_PictureProvider_MonitorRefreshRate_eMax )
      {
         eMonitorRefreshRate = BXDM_PictureProvider_MonitorRefreshRate_e60Hz;
      }
      else
      {
         eMonitorRefreshRate = hXdmPP->stDMConfig.eMonitorRefreshRate;
      }

      uiVsyncThreshold = BXDM_PPTMR_lutVsyncsPersSecond[ eMonitorRefreshRate ];
   }


   if ( pTimerData->uiVsyncCnt >= uiVsyncThreshold )
   {
      BXDM_PPTMR_S_CookResults( hXdmPP );

#if BXDM_PPTMR_P_ENABLE_FUNCTION_TIMING

      for ( i = 0; i < BXDM_PPTIMER_P_Function_eMax; i++ )
      {
         if ( 0 != pTimerData->astFunctions[i].uiNumSamples )
         {
            uiOffset += BKNI_Snprintf(
                           szTemp + uiOffset,
                           BXDM_PP_TIMER_P_MAX_STR_LEN - uiOffset,
                           "%s: %u/%u/%u(%d) ",
                           sFunctionNameLUT[i],
                           pTimerData->astFunctions[i].uiAverageTimeUsecs,
                           pTimerData->astFunctions[i].uiMinTimeUsecs,
                           pTimerData->astFunctions[i].uiMaxTimeUsecs,
                           pTimerData->astFunctions[i].uiNumSamples
                           );

            bPrintFunctionTimes = true;
         }
      }

      if ( true == bPrintFunctionTimes )
      {
         BDBG_MSG(("%x:[%02x.xxx] %s",
                     hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                     BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ),
                     szTemp ));

         uiOffset = 0;
      }

#endif

#if BXDM_PPTMR_P_ENABLE_CALLBACK_TIMING

      for ( i=0; i < BXDM_PictureProvider_Callback_eMax; i++ )
      {
         if ( 0 != pTimerData->astCallbacks[ i ].uiAverageTimeUsecs )
         {
            uiOffset += BKNI_Snprintf(
                           szTemp + uiOffset,
                           BXDM_PP_TIMER_P_MAX_STR_LEN - uiOffset,
                           "%s: %u/%u/%u(%d) ",
                           sCallbackNameLUT[i],
                           pTimerData->astCallbacks[i].uiAverageTimeUsecs,
                           pTimerData->astCallbacks[i].uiMinTimeUsecs,
                           pTimerData->astCallbacks[i].uiMaxTimeUsecs,
                           pTimerData->astCallbacks[i].uiNumSamples
                           );
         }
      }

      BDBG_MSG(("%x:[%02x.xxx] %s",
                     hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                     BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ),
                     szTemp ));
#endif

      BXDM_PPTMR_S_ResetTimerData( &(hXdmPP->stDMState.stDecode.stTimerData) );
   }

#else

   BSTD_UNUSED( szTemp );
   BSTD_UNUSED( uiOffset );
   BSTD_UNUSED( i );
   BSTD_UNUSED( uiVsyncThreshold );
   BSTD_UNUSED( pTimerData );
   BSTD_UNUSED( eMonitorRefreshRate );
   BSTD_UNUSED( hXdmPP );

#endif

   BDBG_LEAVE(BXDM_PPTMR_P_PrintResults);

   return;
}

void BXDM_PPTMR_S_ResetTimerData(
   BXDM_PPTIMER_P_Data * pTimerData
   )
{
   BDBG_ENTER(BXDM_PPTMR_S_ResetTimerData);

   BKNI_Memset_isr( pTimerData, 0, sizeof( BXDM_PPTIMER_P_Data ) );

   BDBG_LEAVE(BXDM_PPTMR_S_ResetTimerData);
   return;
}

#endif   /* if debug build */

