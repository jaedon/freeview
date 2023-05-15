/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: muxer_mp4.c $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 8/13/12 12:28p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/syslib/muxlib/muxer_mp4.c $
 * 
 * Hydra_Software_Devel/7   8/13/12 12:28p nilesh
 * SW7425-3362: Merged FNRT support
 * 
 * Hydra_Software_Devel/SW7425-3362/1   7/5/12 1:54p nilesh
 * SW7425-3362: Added FNRTlib support
 * 
 * Hydra_Software_Devel/6   11/10/11 12:16p nilesh
 * SW7425-1691: File mux output is now open/closed at start/stop capture
 * instead of open/close
 * 
 * Hydra_Software_Devel/5   10/3/11 5:07p nilesh
 * SW7425-891: Added dual simul encoder support
 * 
 * Hydra_Software_Devel/4   9/12/11 1:46p nilesh
 * SW7425-891: Added dual encode support
 * 
 * Hydra_Software_Devel/3   5/11/11 12:01p nilesh
 * SW7425-447: Moved storage implementation to a separate file
 * 
 * Hydra_Software_Devel/2   4/8/11 2:21p delkert
 * SW7425-73: Modifications to match changes to Storage API
 *
 * Hydra_Software_Devel/1   4/4/11 12:12p nilesh
 * SW7425-73: Merged MP4 Mux support
 *
 * Hydra_Software_Devel/mp4_mux_devel/2   3/30/11 4:52p nilesh
 * SW7425-73: Fixed memory leak on close
 *
 * Hydra_Software_Devel/mp4_mux_devel/1   3/30/11 3:10p nilesh
 * SW7425-73: Added MP4 support
 *
 ***************************************************************************/

#include "bstd.h"
#include "muxer_mp4.h"
#include "muxer_file.h"
#include "muxer_priv.h"

#include "bmuxlib_file_mp4.h"

#include "bvce.h"
#include "vce_encoder_priv.h"

#include "string.h"
#include <sys/time.h>

BDBG_MODULE(MUXER_MP4);

#define BTST_MP4_MAX_FILENAME 256

typedef struct BTST_MP4_Muxer_Info
{
      BMUXlib_File_MP4_Handle hMP4Mux;
      char szFilename[BTST_MP4_MAX_FILENAME];
      BMUXlib_StorageSystemInterface stStorage;
      BMUXlib_StorageObjectInterface stOutput;
} BTST_MP4_Muxer_Info;

void* BTST_MP4_OpenMux
   (BTST_P_Context *pContext)
{
   BTST_MP4_Muxer_Info *pstMp4MuxerInfo = (BTST_MP4_Muxer_Info *) BKNI_Malloc( sizeof( BTST_MP4_Muxer_Info ) );

   BSTD_UNUSED( pContext );

   BKNI_Memset(
            pstMp4MuxerInfo,
            0,
            sizeof( BTST_MP4_Muxer_Info )
            );

   /* Create MP4 Mux */
   {
      BMUXlib_File_MP4_CreateSettings stCreateSettings;
      BDBG_MSG(("Creating Muxer"));

      BMUXlib_File_MP4_GetDefaultCreateSettings(&stCreateSettings);
      /* Nothing needs to be overridden right now */
      if (BERR_SUCCESS != BMUXlib_File_MP4_Create(&pstMp4MuxerInfo->hMP4Mux,&stCreateSettings))
      {
         BDBG_ERR(("Failure Creating Mux"));
      }
   }

   return pstMp4MuxerInfo;
}

void BTST_MP4_StartMux
   (
      void* pMuxContext,
      void *pOutputContext
   )
{
   BTST_MP4_Muxer_Info *pstMp4MuxerInfo = (BTST_MP4_Muxer_Info *) pMuxContext;
   BMUXlib_VideoEncoderInterface *pstInputInterface = (BMUXlib_VideoEncoderInterface *) pOutputContext;

   /* Start Mux */
   {
      BMUXlib_File_MP4_StartSettings stStartSettings;
      struct timeval currentTime;

      BDBG_MSG(("Starting Mux"));
      BMUXlib_File_MP4_GetDefaultStartSettings(&stStartSettings);

      /* configure required settings */
      stStartSettings.stStorage = pstMp4MuxerInfo->stStorage;
      stStartSettings.stOutput = pstMp4MuxerInfo->stOutput;

      /* configure the inputs (no audio) */
      stStartSettings.stVideoInputs[0].uiTrackID = 1;
      stStartSettings.stVideoInputs[0].stInterface = *pstInputInterface;
      /* FIXME: hard coded for now */
      stStartSettings.stVideoInputs[0].stInterface.stBufferInfo.eProtocol = BAVC_VideoCompressionStd_eH264;
      stStartSettings.uiNumVideoInputs = 1;
      stStartSettings.uiNumAudioInputs = 0;

      /* configure rest of settings */
      stStartSettings.uiExpectedDurationMs = 0;
      stStartSettings.bProgressiveDownloadCompatible = /*true*/false;
      gettimeofday(&currentTime, NULL);
      /* current time is returned as seconds since 00:00:00 UTC (Coordinated Universal Time), January 1 1970
         so convert to seconds since midnight Jan 1, 1904 as required by MP4.  Thus, this is a difference of
         66 years = 2,082,844,800 seconds */
      stStartSettings.uiCreateTimeUTC = currentTime.tv_sec + 2082844800;

      if (BERR_SUCCESS != BMUXlib_File_MP4_Start(pstMp4MuxerInfo->hMP4Mux, &stStartSettings))
      {
         BDBG_ERR(("Failure to start Mux"));
      }
   }
}

BERR_Code
BTST_MP4_DoMux(
         void* pMuxContext,
         BMUXlib_DoMux_Status *pstStatus
   )
{
   BTST_MP4_Muxer_Info *pstMp4MuxerInfo = (BTST_MP4_Muxer_Info *) pMuxContext;

   return BMUXlib_File_MP4_DoMux(pstMp4MuxerInfo->hMP4Mux, pstStatus);
}

void
BTST_MP4_StartCapture(
         void* pMuxContext,
         char* szFilename
         )
{
   BTST_MP4_Muxer_Info *pstMp4MuxerInfo = (BTST_MP4_Muxer_Info *) pMuxContext;

   /* Setup Storage */
   {
      BMUXlib_StorageObjectInterface *pOutput = &pstMp4MuxerInfo->stOutput;

      /* setup storage */
      if (BERR_SUCCESS != App_StorageCreate(&pstMp4MuxerInfo->stStorage))
      {
         BDBG_ERR(("Unable to create the storage system"));
      }

      /* create an output interface for the final output MP4 content */
      pOutput->pContext = (App_StorageInterfaceContext *)BKNI_Malloc(sizeof(App_StorageInterfaceContext));
      if (NULL == pOutput->pContext )
      {
         BDBG_ERR(("Unable to create the output storage"));
      }
      else
      {
         App_StorageInterfaceContext *pContext = pOutput->pContext;
         BKNI_Memset(pContext, 0, sizeof(App_StorageInterfaceContext));
         pOutput->pfAddDescriptors = (BMUXlib_AddStorageDescriptors)App_StorageInterfaceAddDescriptors;
         pOutput->pfGetCompleteDescriptors = (BMUXlib_GetCompletedStorageDescriptors)App_StorageInterfaceGetCompletedDescriptors;
         /* open the output file for r/w binary */
         sprintf(pstMp4MuxerInfo->szFilename, "%s.mp4", szFilename);
         strcpy(pContext->fname, pstMp4MuxerInfo->szFilename);
         BKNI_Printf("Creating Storage for interface: %p (file: %s)\n", pOutput->pContext, pContext->fname);
         pContext->fp = fopen(pContext->fname, "w+b");
         if (NULL == pContext->fp)
            BDBG_ERR(("Unable to create output storage"));
      }
   }
}

void
BTST_MP4_StopCapture(
         void* pMuxContext
         )
{
   BTST_MP4_Muxer_Info *pstMp4MuxerInfo = (BTST_MP4_Muxer_Info *) pMuxContext;

   BDBG_MSG(("Destroy Output Storage Interface"));
   {
      App_StorageInterfaceContext *pInterfaceContext = pstMp4MuxerInfo->stOutput.pContext;
      BKNI_Printf("Destroying Storage for Context: %p (file: %s)\n", pInterfaceContext, pInterfaceContext->fname);
      /* if file open, close it */
      if (NULL != pInterfaceContext->fp)
         fclose(pInterfaceContext->fp);
      if (NULL != pInterfaceContext)
         BKNI_Free(pInterfaceContext);
      BKNI_Memset(&pstMp4MuxerInfo->stOutput, 0, sizeof(BMUXlib_StorageObjectInterface));
   }

   BDBG_MSG(("Destroy Storage System"));
   App_StorageDestroy(&pstMp4MuxerInfo->stStorage);
}

void BTST_MP4_FinishMux
   (void* pMuxContext)
{
   BTST_MP4_Muxer_Info *pstMp4MuxerInfo = (BTST_MP4_Muxer_Info *) pMuxContext;

   /* force Finish() */
   {
      BMUXlib_File_MP4_FinishSettings stFinishSettings;
      BDBG_MSG(("Prepare for Stop"));
      BMUXlib_File_MP4_GetDefaultFinishSettings(&stFinishSettings);
      if (BERR_SUCCESS != BMUXlib_File_MP4_Finish(pstMp4MuxerInfo->hMP4Mux, &stFinishSettings))
      {
         BDBG_ERR(("Failure to Finish mux"));
      }
   }
}

void BTST_MP4_StopMux
   (void* pMuxContext)
{
   BTST_MP4_Muxer_Info *pstMp4MuxerInfo = (BTST_MP4_Muxer_Info *) pMuxContext;

   BMUXlib_File_MP4_Stop(pstMp4MuxerInfo->hMP4Mux);
}

void BTST_MP4_CloseMux
   (void* pMuxContext)
{
   BTST_MP4_Muxer_Info *pstMp4MuxerInfo = (BTST_MP4_Muxer_Info *) pMuxContext;

   BDBG_MSG(("Destroy Muxer"));
   BMUXlib_File_MP4_Destroy(pstMp4MuxerInfo->hMP4Mux);

   BKNI_Free(pstMp4MuxerInfo);
}
