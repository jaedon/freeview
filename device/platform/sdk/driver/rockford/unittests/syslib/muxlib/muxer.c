/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: muxer.c $
 * $brcm_Revision: Hydra_Software_Devel/42 $
 * $brcm_Date: 8/14/12 10:12a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/syslib/muxlib/muxer.c $
 * 
 * Hydra_Software_Devel/42   8/14/12 10:12a nilesh
 * SW7425-3362: Moved FNRT support to separate command to be backwards
 * compatible with old scripts
 * 
 * Hydra_Software_Devel/41   8/13/12 12:28p nilesh
 * SW7425-3362: Merged FNRT support
 * 
 * Hydra_Software_Devel/SW7425-3362/2   7/26/12 1:43p nilesh
 * SW7425-3362: Updated FNRT API
 * 
 * Hydra_Software_Devel/SW7425-3362/1   7/5/12 1:54p nilesh
 * SW7425-3362: Added FNRTlib support
 * 
 * Hydra_Software_Devel/40   6/19/12 1:37p nilesh
 * SW7425-3076: Stop mux thread and then call stop.
 * 
 * Hydra_Software_Devel/39   4/12/12 12:47p nilesh
 * SW7425-2568: Added explicit config_mux/cfgm command
 * 
 * Hydra_Software_Devel/38   3/7/12 4:09p nilesh
 * SW7435-1: VCE Refactoring
 * 
 * Hydra_Software_Devel/SW7435-1/1   3/7/12 2:43p nilesh
 * SW7435-1: Refactored to support quad transcode
 * 
 * Hydra_Software_Devel/37   2/9/12 3:11p nilesh
 * SW7425-2221: Added support for immediate stop (stop w/o finish)
 * 
 * Hydra_Software_Devel/36   11/10/11 12:16p nilesh
 * SW7425-1691: File mux output is now open/closed at start/stop capture
 * instead of open/close
 * 
 * Hydra_Software_Devel/35   10/3/11 5:07p nilesh
 * SW7425-891: Added dual simul encoder support
 * 
 * Hydra_Software_Devel/34   6/22/11 9:55a delkert
 * SW7425-447: Conditionally compile ASF support
 *
 * Hydra_Software_Devel/33   5/11/11 12:01p nilesh
 * SW7425-447: Added ASF Mux support
 *
 * Hydra_Software_Devel/32   4/4/11 12:12p nilesh
 * SW7425-73: Merged MP4 Mux support
 *
 * Hydra_Software_Devel/mp4_mux_devel/2   3/30/11 3:10p nilesh
 * SW7425-73: Added MP4 support
 *
 * Hydra_Software_Devel/mp4_mux_devel/1   3/30/11 2:00a nilesh
 * SW7425-73: Re-factored muxer to work with different containers
 *
 * Hydra_Software_Devel/31   2/7/11 11:01a nilesh
 * SW7425-38: Removed unused mux mode setting
 *
 * Hydra_Software_Devel/30   2/3/11 4:42p nilesh
 * SW7425-38: Fixed Finished Event
 *
 * Hydra_Software_Devel/29   2/3/11 4:10p nilesh
 * SW7425-38: API Changes
 *
 * Hydra_Software_Devel/28   2/2/11 2:16p nilesh
 * SW7425-38: Changed BMUXlib_TS_AddSystemDataBuffers() prototype
 *
 * Hydra_Software_Devel/27   2/2/11 1:29p nilesh
 * SW7425-38: Changed BMUXlib_TS_AddTransportDescriptors() prototype
 *
 * Hydra_Software_Devel/26   2/2/11 11:39a nilesh
 * SW7425-38: Add streamID support
 *
 * Hydra_Software_Devel/25   1/31/11 5:56p nilesh
 * SW7425-38: Changed DoMux() API
 *
 * Hydra_Software_Devel/24   1/25/11 10:57a nilesh
 * SW7425-38: Fixed debug messages.
 *
 * Hydra_Software_Devel/23   1/19/11 2:04p nilesh
 * SW7425-38: XPT PB Channel Status now checked to see if current
 * transport descriptor has completed.
 *
 * Hydra_Software_Devel/22   1/14/11 12:20p nilesh
 * SW7425-38: Fixed writing TS files larger than 2GB
 *
 * Hydra_Software_Devel/21   12/30/10 2:50p nilesh
 * SW7425-38: Fix shutdown seg fault and memory leaks
 *
 * Hydra_Software_Devel/20   12/29/10 3:41p nilesh
 * SW7425-38: Print TS bytes written
 *
 * Hydra_Software_Devel/19   12/28/10 2:55p nilesh
 * SW7425-38: Update to use new BXPT_TsMux_Create function prototype
 *
 * Hydra_Software_Devel/18   12/27/10 5:57p nilesh
 * SW7425-38: Set BXPT_Playback_PvrDescriptor8 fields. Use PCROffset block
 * 1.
 *
 * Hydra_Software_Devel/17   12/23/10 4:16p nilesh
 * SW7425-38: Fixed filenames for intermediate PES/TS output.
 *
 * Hydra_Software_Devel/16   12/23/10 12:20p nilesh
 * SW7425-38: Added XPT PB Pacing support
 *
 * Hydra_Software_Devel/15   12/16/10 11:23a nilesh
 * SW7425-38: Fixed issue with BEMU_Client_Fwrite
 *
 * Hydra_Software_Devel/14   12/15/10 12:28a nilesh
 * SW7425-38: Set BandHoldEn=true.  Set ITB/CDB threshold depths.
 *
 * Hydra_Software_Devel/13   12/14/10 8:35p nilesh
 * SW7425-38: Fixed BXPT_AddPlaybackDescriptor call
 *
 * Hydra_Software_Devel/12   12/12/10 11:19p hongtaoz
 * SW7425-32: fixed VDC_TEST_ONLY compile error;
 *
 * Hydra_Software_Devel/11   12/12/10 12:31a hongtaoz
 * SW7425-32: resolve xpt conflict between muxer and decoder;
 *
 * Hydra_Software_Devel/10   12/10/10 3:35p hongtaoz
 * SW7425-32: RAVE channel number is 0;
 *
 * Hydra_Software_Devel/9   12/10/10 2:02p hongtaoz
 * SW7425-32: use EMU utility to access between file and device memory;
 * tried to separate xpt muxer PID/rave channels from playback decoder's
 * channels;
 *
 * Hydra_Software_Devel/8   12/8/10 5:05p hongtaoz
 * SW7425-38: added emulation client page prefetch before fwrite;
 *
 * Hydra_Software_Devel/7   11/18/10 4:17p nilesh
 * SW7425-38: Removed eCodec
 *
 * Hydra_Software_Devel/6   11/15/10 11:05a nilesh
 * SW7425-38: Fixed XPT config
 *
 * Hydra_Software_Devel/5   11/11/10 3:33p nilesh
 * SW7425-38: Fixed PID Parser Channel
 *
 * Hydra_Software_Devel/4   11/9/10 11:42a nilesh
 * SW7425-38: Fix XPT Crash
 *
 * Hydra_Software_Devel/3   11/8/10 5:41p nilesh
 * SW7425-38: Integrated XPT PI
 *
 * Hydra_Software_Devel/2   11/3/10 4:21p nilesh
 * SW7425-38: Tweaks
 *
 * Hydra_Software_Devel/1   11/3/10 2:40p nilesh
 * SW7425-38: Added TS Muxlib syslib support
 *
 ***************************************************************************/

/* test include */
#include "config.h"
#include "test_util.h"
#include "interactive.h"
#include "commands.h"

#include "bstd.h"
#include "muxer_priv.h"
#include "muxer_ts.h"
#include "muxer_mp4.h"
#ifdef ASF_SUPPORTED
#include "muxer_asf.h"
#endif

#include "bvce.h"
#include "vce_encoder_priv.h"

#include "bmuxlib_fnrt.h"

#if defined(EMULATION) /* prepare emulation client memory paging to avoid fwrite failure */
#include "pagelib.h"
#endif

BDBG_MODULE(MUXER);

void BTST_OpenMux
        (BTST_P_Context *pContext)
{
   BTST_Muxer_Info *pstMuxerInfo = NULL;

   if ( NULL == pContext->pMuxerInfo )
   {
      pstMuxerInfo = (BTST_Muxer_Info *) BKNI_Malloc( sizeof( BTST_Muxer_Info ) );

      if ( NULL == pstMuxerInfo )
      {
         BDBG_ERR(("Error allocating muxer context"));
         BERR_TRACE( BERR_OUT_OF_SYSTEM_MEMORY );
         return;
      }

      BKNI_Memset(
               pstMuxerInfo,
               0,
               sizeof ( BTST_Muxer_Info )
               );

      pContext->pMuxerInfo = (void *) pstMuxerInfo;
   }
   else
   {
      pstMuxerInfo = (BTST_Muxer_Info *) pContext->pMuxerInfo;
   }

   {
      unsigned uiMuxInstance = 0;
      unsigned uiMuxerType;
      printf("Mux Instance: ");

      BTST_P_Scanf(pContext, "%u", &uiMuxInstance);
      BDBG_ASSERT( uiMuxInstance < MUX_NUM_INSTANCES );

#ifdef ASF_SUPPORTED
   printf("Select Muxer Type (0 = TS, 1 = MP4, 2 = ASF): ");
#else
   printf("Select Muxer Type (0 = TS, 1 = MP4): ");
#endif
      BTST_P_Scanf(pContext, "%u", &uiMuxerType);

      pstMuxerInfo->muxer_interface[uiMuxInstance].eMuxerType = uiMuxerType;

      switch ( pstMuxerInfo->muxer_interface[uiMuxInstance].eMuxerType )
      {
#ifdef ASF_SUPPORTED
         case BTST_Muxer_Type_eASF:
            pstMuxerInfo->muxer_interface[uiMuxInstance].fOpenMux = BTST_ASF_OpenMux;
            pstMuxerInfo->muxer_interface[uiMuxInstance].fConfigMux = NULL;
            pstMuxerInfo->muxer_interface[uiMuxInstance].fCloseMux = BTST_ASF_CloseMux;
            pstMuxerInfo->muxer_interface[uiMuxInstance].fStartMux = BTST_ASF_StartMux;
            pstMuxerInfo->muxer_interface[uiMuxInstance].fFinishMux = BTST_ASF_FinishMux;
            pstMuxerInfo->muxer_interface[uiMuxInstance].fStopMux = BTST_ASF_StopMux;
            pstMuxerInfo->muxer_interface[uiMuxInstance].fDoMux = BTST_ASF_DoMux;
            pstMuxerInfo->muxer_interface[uiMuxInstance].fStartCapture = BTST_ASF_StartCapture;
            pstMuxerInfo->muxer_interface[uiMuxInstance].fStopCapture = BTST_ASF_StopCapture;
            break;
#endif

         case BTST_Muxer_Type_eMP4:
            pstMuxerInfo->muxer_interface[uiMuxInstance].fOpenMux = BTST_MP4_OpenMux;
            pstMuxerInfo->muxer_interface[uiMuxInstance].fConfigMux = NULL;
            pstMuxerInfo->muxer_interface[uiMuxInstance].fCloseMux = BTST_MP4_CloseMux;
            pstMuxerInfo->muxer_interface[uiMuxInstance].fStartMux = BTST_MP4_StartMux;
            pstMuxerInfo->muxer_interface[uiMuxInstance].fFinishMux = BTST_MP4_FinishMux;
            pstMuxerInfo->muxer_interface[uiMuxInstance].fStopMux = BTST_MP4_StopMux;
            pstMuxerInfo->muxer_interface[uiMuxInstance].fDoMux = BTST_MP4_DoMux;
            pstMuxerInfo->muxer_interface[uiMuxInstance].fStartCapture = BTST_MP4_StartCapture;
            pstMuxerInfo->muxer_interface[uiMuxInstance].fStopCapture = BTST_MP4_StopCapture;
            break;

         default:
            printf("Defaulting to TS\n");
            pstMuxerInfo->muxer_interface[uiMuxInstance].eMuxerType = BTST_Muxer_Type_eTS;

         case BTST_Muxer_Type_eTS:
            pstMuxerInfo->muxer_interface[uiMuxInstance].fOpenMux = BTST_TS_OpenMux;
            pstMuxerInfo->muxer_interface[uiMuxInstance].fConfigMux = BTST_TS_ConfigMux;
            pstMuxerInfo->muxer_interface[uiMuxInstance].fCloseMux = BTST_TS_CloseMux;
            pstMuxerInfo->muxer_interface[uiMuxInstance].fStartMux = BTST_TS_StartMux;
            pstMuxerInfo->muxer_interface[uiMuxInstance].fFinishMux = BTST_TS_FinishMux;
            pstMuxerInfo->muxer_interface[uiMuxInstance].fStopMux = BTST_TS_StopMux;
            pstMuxerInfo->muxer_interface[uiMuxInstance].fDoMux = BTST_TS_DoMux;
            pstMuxerInfo->muxer_interface[uiMuxInstance].fStartCapture = BTST_TS_StartCapture;
            pstMuxerInfo->muxer_interface[uiMuxInstance].fStopCapture = BTST_TS_StopCapture;
            break;
      }

      /* Setup Callbacks/Events */
      {
         BDBG_MSG(("Setting up finished event"));
         BKNI_CreateEvent( &pstMuxerInfo->hFinishedEvent[uiMuxInstance] );
      }

      if ( NULL != pstMuxerInfo->muxer_interface[uiMuxInstance].fOpenMux ) pstMuxerInfo->muxer_interface[uiMuxInstance].pContext = pstMuxerInfo->muxer_interface[uiMuxInstance].fOpenMux(pContext);

      pstMuxerInfo->uiNumInstances++;
   }

   return;
}

void BTST_ConfigMux
        (BTST_P_Context *pContext)
{
   BTST_Muxer_Info *pstMuxerInfo = (BTST_Muxer_Info *) pContext->pMuxerInfo;

   {
      unsigned uiMuxInstance = 0;

      printf("Mux Instance: ");
      BTST_P_Scanf(pContext, "%u", &uiMuxInstance);
      BDBG_ASSERT( uiMuxInstance < MUX_NUM_INSTANCES );

      if ( NULL != pstMuxerInfo->muxer_interface[uiMuxInstance].fConfigMux )
         pstMuxerInfo->muxer_interface[uiMuxInstance].fConfigMux(
            pstMuxerInfo->muxer_interface[uiMuxInstance].pContext
            );
   }

   return;
}

void BTST_MuxerHandler(
         BTST_TaskHandle hTask,
         void *pvContext
         )
{
   BERR_Code rc = BERR_UNKNOWN;
   BTST_Muxer_Info *pstMuxerInfo = (BTST_Muxer_Info *) ((BTST_P_Context *)pvContext)->pMuxerInfo;
   unsigned uiMuxInstance = 0;

   BMUXlib_DoMux_Status stMuxerStatus;

   BSTD_UNUSED(hTask);

   do
   {
      for ( uiMuxInstance = 0; uiMuxInstance < MUX_NUM_INSTANCES; uiMuxInstance++ )
      {
         if ( NULL != pstMuxerInfo->muxer_interface[uiMuxInstance].fDoMux )
         {
            rc = pstMuxerInfo->muxer_interface[uiMuxInstance].fDoMux(
                  pstMuxerInfo->muxer_interface[uiMuxInstance].pContext,
                  &stMuxerStatus
                  );

            if ( 0 != stMuxerStatus.uiNextExecutionTime )
            {
      #if 1
               BKNI_Sleep( stMuxerStatus.uiNextExecutionTime );

      #if BTST_ENABLE_SELF_PACED_MUX
               BTST_TransportIncrementTime(
                        pstMuxerInfo->hDeviceTransport,
                        uiNextExecutionTime
                        );
      #endif

      #elif 0
               sleep( 1 );

      #if BTST_ENABLE_SELF_PACED_MUX
               BTST_TransportIncrementTime(
                        pstMuxerInfo->hDeviceTransport,
                        100
                        );
      #endif
      #endif
            }
            else
            {
               /* Sleep for 50 ms by default */
               BKNI_Sleep( 50 );
            }

            if ( BMUXlib_State_eFinished == stMuxerStatus.eState )
            {
               /* We're done muxing */
               BKNI_SetEvent(pstMuxerInfo->hFinishedEvent[uiMuxInstance]);
            }
         }
      }
   } while ( ( BERR_SUCCESS == rc )
             && ( 0 != pstMuxerInfo->uiEnableMuxerHandler )
           );
}

void BTST_ConfigFNRT
        (BTST_P_Context *pContext)
{
   BTST_Muxer_Info *pstMuxerInfo = (BTST_Muxer_Info *) pContext->pMuxerInfo;
   BTST_VCEEncoder_Info *pstEncoderInfo = (BTST_VCEEncoder_Info *) pContext->pEncoderInfo;
   unsigned uiNumFNRTInputs = 0;
   unsigned uiMuxInstance = 0;

   printf("Mux Instance: ");
   BTST_P_Scanf(pContext, "%u", &uiMuxInstance);
   BDBG_ASSERT( uiMuxInstance < MUX_NUM_INSTANCES );

   printf("Num FNRT Inputs: ");
   BTST_P_Scanf(pContext, "%u", &uiNumFNRTInputs);


   if ( 0 != uiNumFNRTInputs )
   {
      unsigned i;
      unsigned uiDevice;
      unsigned uiChannel;
      BMUXlib_FNRT_CreateSettings stCreateSettings;

       /* Allocate FNRT */
      BMUXlib_FNRT_GetDefaultCreateSettings(&stCreateSettings);

      BMUXlib_FNRT_Create(
         &pstMuxerInfo->muxer_interface[uiMuxInstance].hMuxFNRT,
         &stCreateSettings
         );

      for ( i = 0; i < uiNumFNRTInputs; i++ )
      {
         BMUXlib_VideoEncoderInterface stVideoEncoderInterface;
         BKNI_Memset( &stVideoEncoderInterface, 0, sizeof( BMUXlib_VideoEncoderInterface ) );

         printf("[%d] Encode Channel Device: ", i);
         BTST_P_Scanf(pContext, "%u", &uiDevice);

         printf("[%d] Encode Channel Channel: ", i);
         BTST_P_Scanf(pContext, "%u", &uiChannel);

         stVideoEncoderInterface.pContext = pstEncoderInfo->instance[uiDevice].output[uiChannel].hVceOutput;
         stVideoEncoderInterface.fGetBufferDescriptors = (BMUXlib_GetVideoBufferDescriptors) BVCE_Output_GetBufferDescriptors;
         stVideoEncoderInterface.fConsumeBufferDescriptors = (BMUXlib_ConsumeVideoBufferDescriptors) BVCE_Output_ConsumeBufferDescriptors;
         stVideoEncoderInterface.fGetBufferStatus = (BMUXlib_GetVideoBufferStatus) BVCE_Output_GetBufferStatus;

         BMUXlib_FNRT_AddVideoInputInterface(
            pstMuxerInfo->muxer_interface[uiMuxInstance].hMuxFNRT,
            &stVideoEncoderInterface,
            NULL
            );
      }
   }
}

void BTST_StartMux
        (BTST_P_Context *pContext)
{
   BTST_Muxer_Info *pstMuxerInfo = (BTST_Muxer_Info *) pContext->pMuxerInfo;
   BTST_VCEEncoder_Info *pstEncoderInfo = (BTST_VCEEncoder_Info *) pContext->pEncoderInfo;
   BMUXlib_VideoEncoderInterface stVideoEncoderInterface;
   {
      unsigned uiMuxInstance = 0;
      unsigned uiVideoEncoderInstance = 0;


      printf("Mux Instance: ");
      BTST_P_Scanf(pContext, "%u", &uiMuxInstance);
      BDBG_ASSERT( uiMuxInstance < MUX_NUM_INSTANCES );

      if ( true == pstEncoderInfo->state.bCompatibilityMode )
      {
         printf("Video Encoder Channel: ");
         BTST_P_Scanf(pContext, "%u", &uiVideoEncoderInstance);
         BDBG_ASSERT( uiVideoEncoderInstance < VCE_ENCODER_NUM_OUTPUT_CHANNELS );
      }
      else
      {
         uiVideoEncoderInstance = pstEncoderInfo->state.uiChannel;
      }


      BKNI_Memset( &stVideoEncoderInterface, 0, sizeof( BMUXlib_VideoEncoderInterface ) );

      if ( NULL != pstMuxerInfo->muxer_interface[uiMuxInstance].hMuxFNRT )
      {
         BMUXlib_FNRT_GetVideoOutputInterface(
            pstMuxerInfo->muxer_interface[uiMuxInstance].hMuxFNRT,
            &stVideoEncoderInterface,
            NULL
            );
      }
      else
      {
         stVideoEncoderInterface.pContext = pstEncoderInfo->instance[pstEncoderInfo->state.uiInstance].output[uiVideoEncoderInstance].hVceOutput;
         stVideoEncoderInterface.fGetBufferDescriptors = (BMUXlib_GetVideoBufferDescriptors) BVCE_Output_GetBufferDescriptors;
         stVideoEncoderInterface.fConsumeBufferDescriptors = (BMUXlib_ConsumeVideoBufferDescriptors) BVCE_Output_ConsumeBufferDescriptors;
         stVideoEncoderInterface.fGetBufferStatus = (BMUXlib_GetVideoBufferStatus) BVCE_Output_GetBufferStatus;
      }

      if ( NULL != pstMuxerInfo->muxer_interface[uiMuxInstance].fStartMux )
         pstMuxerInfo->muxer_interface[uiMuxInstance].fStartMux(
            pstMuxerInfo->muxer_interface[uiMuxInstance].pContext,
            &stVideoEncoderInterface
            );

      pstMuxerInfo->uiEnableMuxerHandler++;
   }

   /* Start User Thread */
   if ( 1 == pstMuxerInfo->uiEnableMuxerHandler )
   {
      BDBG_MSG(("Starting User Thread"));

      BTST_CreateTask(
               &pstMuxerInfo->hMuxerTask,
               BTST_MuxerHandler,
               pContext
               );
   }

   return;
}

void BTST_StopMux
        (BTST_P_Context *pContext)
{
   BTST_Muxer_Info *pstMuxerInfo = (BTST_Muxer_Info *) pContext->pMuxerInfo;

   {
      unsigned uiMuxInstance = 0;
      unsigned uiImmediateStop = 0;
      printf("Mux Instance: ");
      BTST_P_Scanf(pContext, "%u", &uiMuxInstance);
      BDBG_ASSERT( uiMuxInstance < MUX_NUM_INSTANCES );

      printf("Stop Immediately?: (0=no,1=yes): ");
      BTST_P_Scanf(pContext, "%u", &uiImmediateStop);

      if ( 0 == uiImmediateStop )
      {
         if ( NULL != pstMuxerInfo->muxer_interface[uiMuxInstance].fFinishMux ) pstMuxerInfo->muxer_interface[uiMuxInstance].fFinishMux(pstMuxerInfo->muxer_interface[uiMuxInstance].pContext);

         /* Wait for Finish */
         {
            BERR_Code rc;
            BDBG_MSG(("Wait for Finish"));

            rc = BKNI_WaitForEvent( pstMuxerInfo->hFinishedEvent[uiMuxInstance], 5000 );

            if ( BERR_SUCCESS != rc )
            {
               BDBG_ERR(("Finish Event Timed Out!"));
            }
         }
      }

      pstMuxerInfo->uiEnableMuxerHandler--;

      /* Stop User Thread */
      if ( 0 == pstMuxerInfo->uiEnableMuxerHandler )
      {
         BKNI_Sleep(100);
         BDBG_MSG(("Stopping User Thread"));
         BKNI_EnterCriticalSection();
         BKNI_LeaveCriticalSection();

         BTST_DestroyTask(
                  pstMuxerInfo->hMuxerTask
                  );
      }

      if ( NULL != pstMuxerInfo->muxer_interface[uiMuxInstance].fStopMux ) pstMuxerInfo->muxer_interface[uiMuxInstance].fStopMux(pstMuxerInfo->muxer_interface[uiMuxInstance].pContext);

      if ( NULL != pstMuxerInfo->muxer_interface[uiMuxInstance].hMuxFNRT )
      {
         BMUXlib_FNRT_Destroy(
            pstMuxerInfo->muxer_interface[uiMuxInstance].hMuxFNRT
            );

         pstMuxerInfo->muxer_interface[uiMuxInstance].hMuxFNRT = NULL;
      }
   }
}

void BTST_CloseMux
        (BTST_P_Context *pContext)
{
   BTST_Muxer_Info *pstMuxerInfo = (BTST_Muxer_Info *) pContext->pMuxerInfo;

   {
      unsigned uiMuxInstance = 0;
      printf("Mux Instance: ");
      BTST_P_Scanf(pContext, "%u", &uiMuxInstance);
      BDBG_ASSERT( uiMuxInstance < MUX_NUM_INSTANCES );

      /* Teardown Callbacks/Events */
      {
         BDBG_MSG(("Teardown Callbacks/Events"));
         BKNI_DestroyEvent( pstMuxerInfo->hFinishedEvent[uiMuxInstance] );
      }

      if ( NULL != pstMuxerInfo->muxer_interface[uiMuxInstance].fCloseMux ) pstMuxerInfo->muxer_interface[uiMuxInstance].fCloseMux(pstMuxerInfo->muxer_interface[uiMuxInstance].pContext);

      BKNI_Memset( &pstMuxerInfo->muxer_interface[uiMuxInstance], 0, sizeof( pstMuxerInfo->muxer_interface[uiMuxInstance] ) );
      pstMuxerInfo->uiNumInstances--;
   }

   if ( 0 == pstMuxerInfo->uiNumInstances )
   {
      BKNI_Free( pContext->pMuxerInfo );
      pContext->pMuxerInfo = NULL;
   }

   return;
}

void BTST_GetMuxedFrames
        (BTST_P_Context *pContext)
{
   BTST_Muxer_Info *pstMuxerInfo = (BTST_Muxer_Info *) pContext->pMuxerInfo;
   unsigned uiMuxInstance = 0;
   printf("Mux Instance: ");
   BTST_P_Scanf(pContext, "%u", &uiMuxInstance);
   BDBG_ASSERT( uiMuxInstance < MUX_NUM_INSTANCES );

   /* toggle it */
   pstMuxerInfo->bEnableCaptureMuxOutputHandler[uiMuxInstance] ^= true;

   if( true == pstMuxerInfo->bEnableCaptureMuxOutputHandler[uiMuxInstance] )
   {
      char filename[1024];
      sprintf(filename, "encode_output_[%02d]", uiMuxInstance);
      printf("start to capture muxed buffers [%d]\n", uiMuxInstance);
      if ( NULL != pstMuxerInfo->muxer_interface[uiMuxInstance].fStartCapture ) pstMuxerInfo->muxer_interface[uiMuxInstance].fStartCapture(pstMuxerInfo->muxer_interface[uiMuxInstance].pContext,filename);
   }
   else
   {
      printf("stop to capture muxed buffers [%d]\n", uiMuxInstance);
      if ( NULL != pstMuxerInfo->muxer_interface[uiMuxInstance].fStopCapture ) pstMuxerInfo->muxer_interface[uiMuxInstance].fStopCapture(pstMuxerInfo->muxer_interface[uiMuxInstance].pContext);
   }
   return;
}
