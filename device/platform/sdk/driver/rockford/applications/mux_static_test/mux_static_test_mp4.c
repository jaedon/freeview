/***************************************************************************
 *     Copyright (c) 2010-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mux_static_test_mp4.c $
 * $brcm_Revision: Hydra_Software_Devel/20 $
 * $brcm_Date: 6/29/11 11:38a $
 *
 * [File Description:]
 * Mux static test for MP4
 *
 * Note that this test does NOT use hardware - does not use Transport.
 * Output is File I/O
 *
 * Revision History:
 * $brcm_Log: /rockford/applications/mux_static_test/mux_static_test_mp4.c $
 * 
 * Hydra_Software_Devel/20   6/29/11 11:38a delkert
 * SW7425-673: Clear out status instead of presetting to fixed values
 *
 * Hydra_Software_Devel/19   6/28/11 10:06a delkert
 * SW7425-341: Use muxlib common code for storage I/O
 *
 * Hydra_Software_Devel/18   6/7/11 4:24p delkert
 * SW7425-325: Run AFAP until input done
 *
 * Hydra_Software_Devel/17   6/3/11 10:07a delkert
 * SW7425-634: Enable progressive download, and speed up execution
 * (particularly for finalization stage)
 *
 * Hydra_Software_Devel/16   5/25/11 1:44p delkert
 * SW7425-634: Add API for stop encode, and insert EOS descriptor into
 * output stream when done.
 *
 * Hydra_Software_Devel/15   5/9/11 9:05a delkert
 * SW7425-301: Fix memory leak (Finish event not freed)
 *
 * Hydra_Software_Devel/14   5/2/11 1:38p delkert
 * SW7425-327: Fix mux static test so it builds with new inc files. Fix
 * logic on KEEP_TEMP_FILES.
 *
 * Hydra_Software_Devel/13   4/7/11 1:27p delkert
 * SW7425-73: Modifications to match changes to Storage
 * DestroyStorageObject() interface
 *
 * Hydra_Software_Devel/12   4/6/11 9:14p delkert
 * SW7425-73: Modifications to match Storage API changes
 *
 * Hydra_Software_Devel/11   4/6/11 1:19p delkert
 * SW7425-73: Add audio ESCR pacing
 *
 * Hydra_Software_Devel/10   4/6/11 12:59p delkert
 * SW7425-73: Add initial audio encoder stub for audio static test
 *
 * Hydra_Software_Devel/9   4/4/11 6:21p delkert
 * SW7425-73: Merge mp4_mux_devel to mainline
 *
 * Hydra_Software_Devel/mp4_mux_devel/3   3/31/11 2:47p delkert
 * SW7425-73: Improve debugging output
 *
 * Hydra_Software_Devel/mp4_mux_devel/2   3/18/11 4:53p delkert
 * SW7425-73: Add finish event to properly wait for Finish to complete
 * before stopping. Add read capability to storage descriptor processing
 *
 * Hydra_Software_Devel/mp4_mux_devel/1   3/15/11 10:55a delkert
 * SW7425-73: Set the video protocol to allow NALU processing.
 *
 * Hydra_Software_Devel/8   3/10/11 5:55p delkert
 * SW7425-73: Add setting of CreateTime, to allow testing of header
 * generation
 *
 * Hydra_Software_Devel/7   3/10/11 11:41a delkert
 * SW7425-73: Fix up AddDescriptors() for storage API - must always seek
 * to the specified offset
 *
 * Hydra_Software_Devel/6   3/8/11 10:11a delkert
 * SW7425-73: Add seek/update capability to the storage interface
 *
 * Hydra_Software_Devel/5   3/7/11 10:59a delkert
 * SW7425-73: Process 5 seconds of file, then force Finish() and wait 1
 * second for finalization.  Change encoder interval to 100ms, and only
 * process data if in "started" state (i.e. allow a "finishing" period)
 *
 * Hydra_Software_Devel/4   3/2/11 11:46a delkert
 * SW7425-73: Add a free-running timer for use in profiling.  Don't remove
 * temporary files so we can analyse their contents.
 *
 * Hydra_Software_Devel/3   2/23/11 1:31p delkert
 * SW7425-73: Fix up Storage_AddDescriptors() function to correctly update
 * queued descriptor count if error occurs
 *
 * Hydra_Software_Devel/2   2/22/11 7:43p delkert
 * SW7425-73: Add DoMux() task and add output descriptor processing to
 * write to output file(s)
 *
 * Hydra_Software_Devel/1   2/15/11 5:00p delkert
 * SW7425-73: Initial MP4 Mux static test framework
 *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include "bstd.h"
#include "btst_kni.h"       /* test interface */
#include "framework.h"
#include "bavc.h"
#include "bavc_vce.h"
#include "video_encoder_stub.h"
#include "audio_encoder_stub.h"
#include "bmuxlib_file_mp4.h"
#include "muxer_file.h"

#define VIDEO_ENABLED 1
#define AUDIO_ENABLED 0

BDBG_MODULE(mux_static_test);
BDBG_FILE_MODULE(mux_storage);

typedef struct
{
  BMUXlib_File_MP4_Handle hMP4Mux;

  uint32_t uiVideoEncoderCount;
  VideoEncoderHandle *ahVideoEncoder;
  uint32_t uiAudioEncoderCount;
  AudioEncoderHandle *ahAudioEncoder;
  BKNI_EventHandle hFinishedEvent;
  BKNI_EventHandle hInputDoneEvent;
} TaskHandles;

BTMR_TimerHandle ghTimer;

#define APP_SIMULATION_TIME_INCREMENT 50

void app_MuxTask(BTST_TaskHandle hTask, void *pvContext);

int app_main( int argc, char **argv )
{
   /* Framework and System specific declarations */
   BERR_Code iErr = 0;
   BSystem_Info sysInfo;
   BFramework_Info frmInfo;

#if VIDEO_ENABLED == 1
   VideoEncoderHandle ahVideoEncoder[BMUXLIB_FILE_MP4_MAX_VIDEO_INPUTS];
#endif
#if AUDIO_ENABLED == 1
   AudioEncoderHandle ahAudioEncoder[BMUXLIB_FILE_MP4_MAX_AUDIO_INPUTS];
#endif
   BMUXlib_File_MP4_Handle hMP4Mux;
   BMUXlib_StorageSystemInterface stStorageSystem;
   BMUXlib_StorageObjectInterface stOutput;
   TaskHandles stHandles;
   BTST_TaskHandle hTask;

   BSTD_UNUSED(argc);
   BSTD_UNUSED(argv);

   BSystem_Init(argc, argv, &sysInfo);
   BFramework_Init(&sysInfo, &frmInfo);

   /* DBG Modules */

   /*BDBG_SetModuleLevel("mux_static_test", BDBG_eMsg);*/
   /*BDBG_SetModuleLevel("mux_storage", BDBG_eMsg);*/

   BDBG_SetModuleLevel("BMUXLIB_FILE_MP4", BDBG_eMsg);
   BDBG_SetModuleLevel("BMUXLIB_FILE_MP4_PRIV", BDBG_eMsg);
   BDBG_SetModuleLevel("BMUXLIB_FILE_MP4_BOXES", BDBG_eMsg);
   BDBG_SetModuleLevel("BMUX_MP4_USAGE", BDBG_eMsg);
/*   BDBG_SetModuleLevel("BMUX_MP4_IN_DESC", BDBG_eMsg);
   BDBG_SetModuleLevel("BMUX_MP4_DU", BDBG_eMsg);*/
   /*BDBG_SetModuleLevel("BMUX_MP4_FINISH", BDBG_eMsg);*/
   /*BDBG_SetModuleLevel("BMUX_MP4_OUTPUT", BDBG_eMsg);*/
   /*BDBG_SetModuleLevel("BMUX_MP4_STATE", BDBG_eMsg);*/

#if VIDEO_ENABLED == 1
   /*BDBG_SetModuleLevel("VIDEO_ENCODER_STUB", BDBG_eMsg);*/
   /*BDBG_SetModuleLevel("VIDEO_ENCODER_STUB_NALU", BDBG_eMsg);*/
#endif
#if AUDIO_ENABLED == 1
   /*BDBG_SetModuleLevel("AUDIO_ENCODER_STUB", BDBG_eMsg);*/
#endif
   /* create a timer device for profiling */
   BDBG_MSG(("Opening free-run Timer ... "));
   {
      BTMR_Settings Settings = { BTMR_Type_eSharedFreeRun, NULL, NULL, 0, false };
      /* open a free-run timer */
      BTMR_CreateTimer(frmInfo.hTmr, &ghTimer, &Settings);
   }

#if VIDEO_ENABLED == 1
   /* Open Video Encoder Stub */
   BDBG_MSG(("Open Video Encoder"));

   BKNI_Memset(ahVideoEncoder, 0, sizeof(VideoEncoderHandle) * BMUXLIB_FILE_MP4_MAX_VIDEO_INPUTS );
   {
         VideoEncoderSettings stSettings;
         BKNI_Memset(&stSettings, 0, sizeof( VideoEncoderSettings ));
         stSettings.hMem = frmInfo.hMem;
         app_OpenVideoEncoder(&ahVideoEncoder[0], &stSettings);
   }
#endif

#if AUDIO_ENABLED == 1
   /* Open Audio Encoder Stub */
   BDBG_MSG(("Open Audio Encoder"));

   BKNI_Memset(ahAudioEncoder, 0, sizeof(AudioEncoderHandle) * BMUXLIB_FILE_MP4_MAX_AUDIO_INPUTS );
   {
         AudioEncoderSettings stSettings;
         BKNI_Memset(&stSettings, 0, sizeof(AudioEncoderSettings ));
         stSettings.hMem = frmInfo.hMem;
         app_OpenAudioEncoder(&ahAudioEncoder[0], &stSettings);
         BDBG_MSG(("Done Open Audio Encoder"));
   }
#endif
   BKNI_CreateEvent( &stHandles.hFinishedEvent );
   BKNI_CreateEvent( &stHandles.hInputDoneEvent );

/********************************************
   MP4 Mux Code goes here
********************************************/

   /* Create MP4 Mux */
   {
      BMUXlib_File_MP4_CreateSettings stCreateSettings;
      BDBG_MSG(("Creating Muxer"));

      BMUXlib_File_MP4_GetDefaultCreateSettings(&stCreateSettings);
      /* Nothing needs to be overridden right now */
      if (BERR_SUCCESS != BMUXlib_File_MP4_Create(&hMP4Mux,&stCreateSettings))
      {
         BDBG_ERR(("Failure Creating Mux"));
      }
   }

   /* Start Mux */
   {
      BMUXlib_File_MP4_StartSettings stStartSettings;
      struct timeval currentTime;

      BDBG_MSG(("Starting Mux"));
      BMUXlib_File_MP4_GetDefaultStartSettings(&stStartSettings);

      /* configure required settings */
      /* setup storage */
      if (BERR_SUCCESS != App_StorageCreate(&stStorageSystem))
      {
         BDBG_ERR(("Unable to create the storage system"));
      }
      stStartSettings.stStorage = stStorageSystem;

      /* create an output interface for the final output MP4 content */
      stOutput.pContext = (App_StorageInterfaceContext *)BKNI_Malloc(sizeof(App_StorageInterfaceContext));
      if (NULL == stOutput.pContext )
      {
         BDBG_ERR(("Unable to create the output storage"));
      }
      else
      {
         App_StorageInterfaceContext *pContext = stOutput.pContext;
         BKNI_Memset(pContext, 0, sizeof(App_StorageInterfaceContext));
         stOutput.pfAddDescriptors = (BMUXlib_AddStorageDescriptors)App_StorageInterfaceAddDescriptors;
         stOutput.pfGetCompleteDescriptors = (BMUXlib_GetCompletedStorageDescriptors)App_StorageInterfaceGetCompletedDescriptors;
         /* open the output file for r/w binary */
         strcpy(pContext->fname, "muxtest.mp4");
         BDBG_MODULE_MSG(mux_storage, ("Creating Storage for interface: %p (file: %s)", stOutput.pContext, pContext->fname));
         pContext->fp = fopen(pContext->fname, "w+b");
         if (NULL == pContext->fp)
            BDBG_ERR(("Unable to create output storage"));
         stStartSettings.stOutput = stOutput;
      }

      /* configure the inputs */
#if VIDEO_ENABLED == 1
      stStartSettings.stVideoInputs[0].uiTrackID = 1;
      stStartSettings.stVideoInputs[0].stInterface.pContext = ahVideoEncoder[0];
      stStartSettings.stVideoInputs[0].stInterface.fGetBufferDescriptors = (BMUXlib_GetVideoBufferDescriptors)app_GetVideoBufferDescriptors;
      stStartSettings.stVideoInputs[0].stInterface.fConsumeBufferDescriptors = (BMUXlib_ConsumeVideoBufferDescriptors)app_ConsumeVideoBufferDescriptors;
      stStartSettings.stVideoInputs[0].stInterface.fGetBufferStatus = (BMUXlib_GetVideoBufferStatus)app_GetVideoBufferStatus;
      /* FIXME: hard coded for now */
      stStartSettings.stVideoInputs[0].stInterface.stBufferInfo.eProtocol = BAVC_VideoCompressionStd_eH264;
      stStartSettings.uiNumVideoInputs = 1;
#endif

#if AUDIO_ENABLED == 1
      stStartSettings.stAudioInputs[0].uiTrackID = 2;
      stStartSettings.stAudioInputs[0].stInterface.pContext = ahAudioEncoder[0];
      stStartSettings.stAudioInputs[0].stInterface.fGetBufferDescriptors = (BMUXlib_GetAudioBufferDescriptors)app_GetAudioBufferDescriptors;
      stStartSettings.stAudioInputs[0].stInterface.fConsumeBufferDescriptors = (BMUXlib_ConsumeAudioBufferDescriptors)app_ConsumeAudioBufferDescriptors;
      stStartSettings.stAudioInputs[0].stInterface.fGetBufferStatus = (BMUXlib_GetAudioBufferStatus)app_GetAudioBufferStatus;
      /* FIXME: hard coded for now */
      stStartSettings.stAudioInputs[0].stInterface.stBufferInfo.eProtocol = BAVC_AudioCompressionStd_eAac;
      stStartSettings.uiNumAudioInputs = 1;
#endif

      /* configure rest of settings */
      stStartSettings.uiExpectedDurationMs = 0;
      stStartSettings.bProgressiveDownloadCompatible = true;
      stStartSettings.bUseEditList = true;
      gettimeofday(&currentTime, NULL);
      /* current time is returned as seconds since 00:00:00 UTC (Coordinated Universal Time), January 1 1970
         so convert to seconds since midnight Jan 1, 1904 as required by MP4.  Thus, this is a difference of
         66 years = 2,082,844,800 seconds */
      stStartSettings.uiCreateTimeUTC = currentTime.tv_sec + 2082844800;

      if (BERR_SUCCESS != BMUXlib_File_MP4_Start(hMP4Mux, &stStartSettings))
      {
         BDBG_ERR(("Failure to start Mux"));
      }
   }

   /* Start Domux Thread */
   {
      BDBG_MSG(("Starting Muxer Thread"));

      stHandles.hMP4Mux = hMP4Mux;
#if VIDEO_ENABLED == 1
      stHandles.uiVideoEncoderCount = BMUXLIB_FILE_MP4_MAX_VIDEO_INPUTS;
      stHandles.ahVideoEncoder = ahVideoEncoder;
#endif

#if AUDIO_ENABLED == 1
      stHandles.uiAudioEncoderCount = BMUXLIB_FILE_MP4_MAX_AUDIO_INPUTS;
      stHandles.ahAudioEncoder = ahAudioEncoder;
#endif
      BTST_CreateTask(&hTask, app_MuxTask, &stHandles);
   }

   /* wait for input to complete ... */
   if (BERR_SUCCESS != BKNI_WaitForEvent( stHandles.hInputDoneEvent, 5000 ))
   {
      BDBG_WRN(("Input Not Completed ... Forcing Finish!"));
   }
   else
      BDBG_MSG(("Input Done ... Finishing"));

   /* Finish() */
   {
      BMUXlib_File_MP4_FinishSettings stFinishSettings;
      BMUXlib_File_MP4_GetDefaultFinishSettings(&stFinishSettings);
      /* first stop the encoder */
#if VIDEO_ENABLED == 1
      BDBG_MSG(("Stop Video Encoder"));
      app_StopVideoEncoder(ahVideoEncoder[0]);
#endif
#if AUDIO_ENABLED == 1
      BDBG_MSG(("Stop Audio Encoder"));
      app_StopAudioEncoder(ahAudioEncoder[0]);
#endif
      /* then stop the mux */
      if (BERR_SUCCESS != BMUXlib_File_MP4_Finish(hMP4Mux, &stFinishSettings))
      {
         BDBG_ERR(("Failure to Finish mux"));
      }
   }
   /* when finished, stop the mux ... */
   BDBG_MSG(("Wait for Finish"));

   if (BERR_SUCCESS != BKNI_WaitForEvent( stHandles.hFinishedEvent, 1000 ))
   {
      BDBG_ERR(("Finish Event Timed Out!"));
   }

   /* Stop Mux */
   BDBG_MSG(("Stopping Mux"));
   BMUXlib_File_MP4_Stop(hMP4Mux);

   /* FIXME: May need to sleep in here to allow mux time to complete the stop() */
   /* Destroy Mux Thread */
   BDBG_MSG(("Destroy Mux Thread"));
   BTST_DestroyTask(hTask);

   BDBG_MSG(("Destroy Output Storage Interface"));
   {
      App_StorageInterfaceContext *pInterfaceContext = stOutput.pContext;
      BDBG_MODULE_MSG(mux_storage, ("Destroying Storage for Context: %p (file: %s)", pInterfaceContext->fname));
      /* if file open, close it */
      if (NULL != pInterfaceContext->fp)
         fclose(pInterfaceContext->fp);
      if (NULL != pInterfaceContext)
         BKNI_Free(pInterfaceContext);
      BKNI_Memset(&stOutput, 0, sizeof(BMUXlib_StorageObjectInterface));
   }

   BDBG_MSG(("Destroy Storage System"));
   App_StorageDestroy(&stStorageSystem);

   BDBG_MSG(("Destroy Muxer"));
   BMUXlib_File_MP4_Destroy(hMP4Mux);

/***********************************************

***********************************************/
#if VIDEO_ENABLED == 1
   /* Close Video Encoder Stub */
   BDBG_MSG(("Close Video Encoder"));
   app_CloseVideoEncoder(ahVideoEncoder[0]);
#endif

#if AUDIO_ENABLED == 1
   /* Close Audio Encoder Stub */
   BDBG_MSG(("Close Audio Encoder"));
   app_CloseAudioEncoder(ahAudioEncoder[0]);
#endif

   BDBG_MSG(("Destroy Finish Event"));
   BKNI_DestroyEvent( stHandles.hFinishedEvent );
   BDBG_MSG(("Destroy Input Done Event"));
   BKNI_DestroyEvent( stHandles.hInputDoneEvent );
   BDBG_MSG(("Destroy Free-Run Timer"));
	BTMR_DestroyTimer(ghTimer);
   BDBG_MSG(("Uninitialize Framework"));
   BFramework_Uninit(&frmInfo);
   BSystem_Uninit(&sysInfo);
   BDBG_MSG(("Done"));
   return iErr;
}

/************************************************
  DoMux() Thread
************************************************/
void app_MuxTask(BTST_TaskHandle hTask, void *pvContext)
{
   BERR_Code rc;
   TaskHandles *pstHandles = (TaskHandles*) pvContext;
   uint32_t i;

   BMUXlib_DoMux_Status stStatus;
   BKNI_Memset(&stStatus, 0, sizeof(BMUXlib_DoMux_Status));

   BSTD_UNUSED(hTask);

   do
   {
      BDBG_MSG(("Executing Mux Thread"));
      BKNI_EnterCriticalSection();

      rc = BMUXlib_File_MP4_DoMux(pstHandles->hMP4Mux, &stStatus);

      BKNI_LeaveCriticalSection();
      BDBG_MSG(("Do Mux returning: 0x%x (state: %d)", rc, stStatus.eState));

      /* no need to have any sort of time delay ... this can run AFAP */
      /*BKNI_Sleep(stStatus.uiNextExecutionTime);*/

      if (BMUXlib_State_eStarted == stStatus.eState)
      {
#if VIDEO_ENABLED == 1
         for (i = 0; i < pstHandles->uiVideoEncoderCount; i++ )
         {
            if ( NULL != pstHandles->ahVideoEncoder[i] )
            {
               app_VideoEncoderIncrementTime(pstHandles->ahVideoEncoder[i], APP_SIMULATION_TIME_INCREMENT);
               if (app_IsInputDone(pstHandles->ahVideoEncoder[i]))
                  BKNI_SetEvent_isr(pstHandles->hInputDoneEvent);
            }
         }
#endif
#if AUDIO_ENABLED == 1
         for (i = 0; i < pstHandles->uiAudioEncoderCount; i++ )
         {
            if ( NULL != pstHandles->ahAudioEncoder[i] )
            {
               app_AudioEncoderIncrementTime(pstHandles->ahAudioEncoder[i], APP_SIMULATION_TIME_INCREMENT);
            }
         }
#endif
      }
   } while (BMUXlib_State_eFinished != stStatus.eState);
   BKNI_SetEvent_isr(pstHandles->hFinishedEvent);
}
