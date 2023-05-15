/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: muxer_asf.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 8/13/12 12:28p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/syslib/muxlib/muxer_asf.c $
 * 
 * Hydra_Software_Devel/5   8/13/12 12:28p nilesh
 * SW7425-3362: Merged FNRT support
 * 
 * Hydra_Software_Devel/SW7425-3362/1   7/5/12 1:54p nilesh
 * SW7425-3362: Added FNRTlib support
 * 
 * Hydra_Software_Devel/4   11/10/11 12:16p nilesh
 * SW7425-1691: File mux output is now open/closed at start/stop capture
 * instead of open/close
 * 
 * Hydra_Software_Devel/3   10/3/11 5:07p nilesh
 * SW7425-891: Added dual simul encoder support
 * 
 * Hydra_Software_Devel/2   9/12/11 1:45p nilesh
 * SW7425-891: Added dual encode support
 * 
 * Hydra_Software_Devel/1   5/11/11 12:01p nilesh
 * SW7425-447: Added ASF Mux support
 * 
 ***************************************************************************/

#include "bstd.h"
#include "muxer_asf.h"
#include "muxer_file.h"
#include "muxer_priv.h"

#include "bmuxlib_file_asf.h"

#include "bvce.h"
#include "vce_encoder_priv.h"

#include "string.h"
#include <sys/time.h>

BDBG_MODULE(MUXER_ASF);

#define BTST_ASF_MAX_FILENAME 256

typedef struct BTST_ASF_Muxer_Info
{
      BMUXlib_File_ASF_Handle hASFMux;
      char szFilename[BTST_ASF_MAX_FILENAME];
      BMUXlib_StorageSystemInterface stStorage;
      BMUXlib_StorageObjectInterface stOutput;
} BTST_ASF_Muxer_Info;

void* BTST_ASF_OpenMux
   (BTST_P_Context* pContext)
{
   BTST_ASF_Muxer_Info *pstASFMuxerInfo = (BTST_ASF_Muxer_Info *) BKNI_Malloc( sizeof( BTST_ASF_Muxer_Info ) );

   BKNI_Memset(
            pstASFMuxerInfo,
            0,
            sizeof( BTST_ASF_Muxer_Info )
            );

   BSTD_UNUSED( pContext );

   /* Create ASF Mux */
   {
      BMUXlib_File_ASF_CreateSettings stCreateSettings;
      BDBG_MSG(("Creating Muxer"));

      BMUXlib_File_ASF_GetDefaultCreateSettings(&stCreateSettings);
      /* Nothing needs to be overridden right now */
      if (BERR_SUCCESS != BMUXlib_File_ASF_Create(&pstASFMuxerInfo->hASFMux,&stCreateSettings))
      {
         BDBG_ERR(("Failure Creating Mux"));
      }
   }

   return pstASFMuxerInfo;
}

void BTST_ASF_StartMux
   (
      void* pMuxContext,
      void* pOutputContext
      )
{
   BTST_ASF_Muxer_Info *pstASFMuxerInfo = (BTST_ASF_Muxer_Info *) pMuxContext;
   BMUXlib_VideoEncoderInterface *pstInputInterface = (BMUXlib_VideoEncoderInterface *) pOutputContext;

   /* Start Mux */
   {
      BMUXlib_File_ASF_StartSettings stStartSettings;

      BDBG_MSG(("Starting Mux"));
      BMUXlib_File_ASF_GetDefaultStartSettings(&stStartSettings);

      /* configure required settings */
      stStartSettings.stStorage = pstASFMuxerInfo->stStorage;
      stStartSettings.stOutput = pstASFMuxerInfo->stOutput;

      /* configure the inputs (no audio) */
      stStartSettings.stVideoInputs[0].uiStreamNumber = 1;
      stStartSettings.stVideoInputs[0].stInterface = *pstInputInterface;
      /* FIXME: hard coded for now */
      stStartSettings.stVideoInputs[0].stInterface.stBufferInfo.eProtocol = BAVC_VideoCompressionStd_eVC1SimpleMain;
      stStartSettings.stVideoInputs[0].stInterface.stBufferInfo.eProfile = BAVC_VideoCompressionProfile_eSimple;
      stStartSettings.uiNumVideoInputs = 1;
      stStartSettings.uiNumAudioInputs = 0;

      if (BERR_SUCCESS != BMUXlib_File_ASF_Start(pstASFMuxerInfo->hASFMux, &stStartSettings))
      {
         BDBG_ERR(("Failure to start Mux"));
      }
   }
}

BERR_Code
BTST_ASF_DoMux(
         void* pMuxContext,
         BMUXlib_DoMux_Status *pstStatus
   )
{
   BTST_ASF_Muxer_Info *pstASFMuxerInfo = (BTST_ASF_Muxer_Info *) pMuxContext;

   return BMUXlib_File_ASF_DoMux(pstASFMuxerInfo->hASFMux, pstStatus);
}

void
BTST_ASF_StartCapture(
         void* pMuxContext,
         char* szFilename
         )
{
   BTST_ASF_Muxer_Info *pstASFMuxerInfo = (BTST_ASF_Muxer_Info *) pMuxContext;

   /* Setup Storage */
   {
      BMUXlib_StorageObjectInterface *pOutput = &pstASFMuxerInfo->stOutput;

      /* setup storage */
      if (BERR_SUCCESS != App_StorageCreate(&pstASFMuxerInfo->stStorage))
      {
         BDBG_ERR(("Unable to create the storage system"));
      }

      /* create an output interface for the final output ASF content */
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
         sprintf(pstASFMuxerInfo->szFilename, "%s.asf", szFilename);
         strcpy(pContext->fname, pstASFMuxerInfo->szFilename);
         BKNI_Printf("Creating Storage for interface: %p (file: %s)\n", pOutput->pContext, pContext->fname);
         pContext->fp = fopen(pContext->fname, "w+b");
         if (NULL == pContext->fp)
            BDBG_ERR(("Unable to create output storage"));
      }
   }
}

void
BTST_ASF_StopCapture(
         void* pMuxContext
         )
{
   BTST_ASF_Muxer_Info *pstASFMuxerInfo = (BTST_ASF_Muxer_Info *) pMuxContext;

   BDBG_MSG(("Destroy Output Storage Interface"));
   {
      App_StorageInterfaceContext *pInterfaceContext = pstASFMuxerInfo->stOutput.pContext;
      BKNI_Printf("Destroying Storage for Context: %p (file: %s)\n", pInterfaceContext, pInterfaceContext->fname);
      /* if file open, close it */
      if (NULL != pInterfaceContext->fp)
         fclose(pInterfaceContext->fp);
      if (NULL != pInterfaceContext)
         BKNI_Free(pInterfaceContext);
      BKNI_Memset(&pstASFMuxerInfo->stOutput, 0, sizeof(BMUXlib_StorageObjectInterface));
   }

   BDBG_MSG(("Destroy Storage System"));
   App_StorageDestroy(&pstASFMuxerInfo->stStorage);
}

void BTST_ASF_FinishMux
   (void* pMuxContext)
{
   BTST_ASF_Muxer_Info *pstASFMuxerInfo = (BTST_ASF_Muxer_Info *) pMuxContext;

   /* force Finish() */
   {
      BMUXlib_File_ASF_FinishSettings stFinishSettings;
      BDBG_MSG(("Prepare for Stop"));
      BMUXlib_File_ASF_GetDefaultFinishSettings(&stFinishSettings);
      if (BERR_SUCCESS != BMUXlib_File_ASF_Finish(pstASFMuxerInfo->hASFMux, &stFinishSettings))
      {
         BDBG_ERR(("Failure to Finish mux"));
      }
   }
}

void BTST_ASF_StopMux
   (void* pMuxContext)
{
   BTST_ASF_Muxer_Info *pstASFMuxerInfo = (BTST_ASF_Muxer_Info *) pMuxContext;

   BMUXlib_File_ASF_Stop(pstASFMuxerInfo->hASFMux);
}

void BTST_ASF_CloseMux
   (void* pMuxContext)
{
   BTST_ASF_Muxer_Info *pstASFMuxerInfo = (BTST_ASF_Muxer_Info *) pMuxContext;

   BDBG_MSG(("Destroy Muxer"));
   BMUXlib_File_ASF_Destroy(pstASFMuxerInfo->hASFMux);

   BKNI_Free(pstASFMuxerInfo);
}
