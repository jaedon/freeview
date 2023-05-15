/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmuxlib_file_asf.c $
 * $brcm_Revision: Hydra_Software_Devel/31 $
 * $brcm_Date: 3/14/12 2:05p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/muxlib/7425/file/asf/bmuxlib_file_asf.c $
 * 
 * Hydra_Software_Devel/31   3/14/12 2:05p nilesh
 * SW7425-2618: AAC/AAC+ LOAS is not supported
 * 
 * Hydra_Software_Devel/30   2/2/12 5:08p nilesh
 * SW7425-2315: Added support for
 * BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EMPTY_FRAME
 * 
 * Hydra_Software_Devel/29   1/9/12 3:47p delkert
 * SW7425-2121: Modify input and output create settings to match changes
 * to APIs
 *
 * Hydra_Software_Devel/28   1/4/12 1:51p nilesh
 * SW7425-998: Added auto-finish support when EOS seen on all inputs
 *
 * Hydra_Software_Devel/27   12/20/11 1:45p nilesh
 * SW7425-1869: Merged BMUXlib_InputGroup support
 *
 * Hydra_Software_Devel/SW7425-1869/1   12/9/11 8:40a delkert
 * SW7425-1869: Modify ASF to use InputGroup for input selection.
 *
 * Hydra_Software_Devel/26   12/12/11 12:13p nilesh
 * SW7425-1476: Enabled WMA Standard audio support
 *
 * Hydra_Software_Devel/25   11/15/11 4:47p delkert
 * SWCHECKFRAME-114: Move accessor macros to private header, and rename
 * them to include _P_
 *
 * Hydra_Software_Devel/24   11/15/11 4:25p nilesh
 * SW7425-1691: Reset stStartSettings and stFinishSettings on stop()
 *
 * Hydra_Software_Devel/23   11/15/11 3:20p delkert
 * SWCHECKFRAME-114: Add accessor macros for mux state for use in testing.
 * Modify the mux to use these macros instead of directly accessing the
 * state.
 *
 * Hydra_Software_Devel/22   11/10/11 4:44p nilesh
 * SW7425-1691: Added recovery logic for "failed Start" --> "Start"
 *
 * Hydra_Software_Devel/21   11/10/11 12:49p nilesh
 * SW7425-1691: Merge to mainline
 *
 * Hydra_Software_Devel/SW7425-1691/1   11/10/11 11:56a nilesh
 * SW7425-1691: Added support for stop-->start without requiring
 * destroy/create
 *
 * Hydra_Software_Devel/20   11/9/11 10:46a nilesh
 * SW7425-1707: Changed uiInstance/uiOutputInstance to uiMuxId
 *
 * Hydra_Software_Devel/19   11/9/11 10:01a nilesh
 * SW7425-1707: Added support for setting
 * BMUXlib_Input_CreateSettings.uiOutputInstance
 *
 * Hydra_Software_Devel/18   11/9/11 9:36a nilesh
 * SW7425-1707: Set BMUXlib_Input's uiTypeInstance to facilitate logging
 * of multiple audio/video streams
 *
 * Hydra_Software_Devel/17   10/11/11 8:39p nilesh
 * SW7425-324: No longer print output blocked as a warning message
 *
 * Hydra_Software_Devel/16   10/10/11 1:53p delkert
 * SW7425-1405: Change asserts in Start() to use error checking instead
 * (so Start with defaults doesn't bomb the system)
 *
 * Hydra_Software_Devel/15   10/10/11 8:40a delkert
 * SW7425-1405: Add parameter checks for output interface.  Verify stream
 * numbers are not already used.
 * Add error checking on audio protocol. Stop() should return "success" if
 * executed from stopped state.
 * Set output handle to NULL after Destroy(). Add check for no output
 * object when executing DoMux() - can happen
 * if called in the initial stopped state.
 *
 * Hydra_Software_Devel/14   10/6/11 2:25p delkert
 * SW7425-1405: Moved Max Packet Length to the public API.
 *
 * Hydra_Software_Devel/13   10/6/11 1:50p delkert
 * SW7425-1405: Add signature to context struct and add checking to all
 * APIs. Add packet length upper limit checks in Create()
 *
 * Hydra_Software_Devel/12   10/5/11 5:56p delkert
 * SW7425-1415: Fix coverity issues
 *
 * Hydra_Software_Devel/11   9/16/11 11:15a nilesh
 * SW7425-1281: Fixed assert when stopping mux before media has started
 * writing.
 *
 * Hydra_Software_Devel/10   9/1/11 12:08p nilesh
 * SW7425-720: Integrated new ASF Packetizer
 *
 * Hydra_Software_Devel/9   8/12/11 11:05a nilesh
 * SW7425-936: Reset blocked state in DoMux
 *
 * Hydra_Software_Devel/8   8/12/11 10:18a nilesh
 * SW7425-936: Increase output descriptor count to 16384. Blocked state is
 * now set if there are descriptors waiting to be queued.
 *
 * Hydra_Software_Devel/7   7/18/11 6:51p nilesh
 * SW7425-937: Added H.264 video support
 *
 * Hydra_Software_Devel/6   6/29/11 11:05a nilesh
 * SW7425-673: BMUXlib_DoMux_Status is now memset to 0 in DoMux()
 *
 * Hydra_Software_Devel/5   6/14/11 12:47p nilesh
 * SW7425-325: Added support for BMUXlib_DoMux_Status.bBlockedOutput
 *
 * Hydra_Software_Devel/4   5/17/11 3:41p nilesh
 * SW7425-447: Added AAC Audio Support
 *
 * Hydra_Software_Devel/3   5/16/11 3:27p nilesh
 * SW7425-447: Set packet media object size.  Set payload parsing padding
 * size.
 *
 * Hydra_Software_Devel/2   5/11/11 3:22p nilesh
 * SW7425-447: Post-Process ASF.
 *
 * Hydra_Software_Devel/1   5/11/11 11:56a nilesh
 * SW7425-447: ASF Mux
 *
 ***************************************************************************/

#include "bstd.h" /* also includes berr, bdbg, etc */
#include "bkni.h"

#include "bmuxlib_file_asf.h"
#include "bmuxlib_file_asf_priv.h"
#include "bmuxlib_file_asf_packetizer.h"

#include "bmuxlib_file_asf_header.h"
#include "bmuxlib_list.h"
#include "bmuxlib_input.h"
#include "bmuxlib_output.h"

BDBG_MODULE(BMUXLIB_FILE_ASF);

/* TODO List *
 *
 * - Phase 3.0
 *    X Finalize File
 *    - Add AAC Audio Support
 *    - Add Proper VC1 Metadata Support
 *    - Add in-band metadata support
 *    - Add MP3 Audio Support
 * - Future Improvements
 *    X Explicitly add padding
 *    - Avoid padding and pack payloads together
 *    - Index Creation
 */

/**************************
    Static Definitions
**************************/
static const BMUXlib_File_ASF_CreateSettings s_stDefaultCreateSettings =
{
 BMUXLIB_FILE_ASF_P_SIGNATURE_CREATESETTINGS, /* Signature */

 BMUXLIB_FILE_ASF_P_DEFAULT_PACKETLENGTH,     /* Packet Length */
 0                                            /* uiMuxId */
};

static const BMUXlib_File_ASF_FinishSettings s_stDefaultFinishSettings =
{
 BMUXLIB_FILE_ASF_P_SIGNATURE_FINISHSETTINGS, /* Signature */

 BMUXLIB_FILE_ASF_P_DEFAULT_FINISHMODE        /* Finish Mode */
};

static void BMUXlib_File_ASF_P_Reset(
   BMUXlib_File_ASF_Handle          hASFMux
   );

/*************************
*   Create/Destroy API   *
*************************/

void BMUXlib_File_ASF_GetDefaultCreateSettings(
            BMUXlib_File_ASF_CreateSettings  *pCreateSettings)
{
   BDBG_ENTER( BMUXlib_File_ASF_GetDefaultCreateSettings );

   BDBG_ASSERT( pCreateSettings );

   *pCreateSettings = s_stDefaultCreateSettings;

   BDBG_LEAVE( BMUXlib_File_ASF_GetDefaultCreateSettings );
}

BERR_Code BMUXlib_File_ASF_Create(
            BMUXlib_File_ASF_Handle          *phASFMux,
      const BMUXlib_File_ASF_CreateSettings  *pCreateSettings)
{
   BERR_Code rc = BERR_UNKNOWN;
   BMUXlib_File_ASF_Handle hASFMux;

   BDBG_ENTER( BMUXlib_File_ASF_Create );

   BDBG_ASSERT( phASFMux );
   BDBG_ASSERT( pCreateSettings );
   BDBG_ASSERT( BMUXLIB_FILE_ASF_P_SIGNATURE_CREATESETTINGS == pCreateSettings->uiSignature );

   *phASFMux = NULL;
   if (pCreateSettings->uiPacketLength > BMUXLIB_FILE_ASF_MAX_PACKET_LENGTH)
   {
      BDBG_LEAVE( BMUXlib_File_ASF_Create );
      return BERR_TRACE( BERR_INVALID_PARAMETER );
   }

   hASFMux = ( BMUXlib_File_ASF_Handle ) BKNI_Malloc( sizeof( BMUXlib_File_ASF_P_Context ) );

   if ( NULL != hASFMux )
   {
      BKNI_Memset( hASFMux, 0, sizeof( BMUXlib_File_ASF_P_Context  ) );

      hASFMux->stCreateSettings = *pCreateSettings;

      hASFMux->uiHeaderBufferSize = BMUXLIB_FILE_ASF_P_MAX_HEADERLENGTH;
      hASFMux->pHeaderBuffer = (uint8_t*) BKNI_Malloc( hASFMux->uiHeaderBufferSize );
      if ( NULL == hASFMux->pHeaderBuffer )
      {
         rc = BERR_TRACE( BERR_OUT_OF_SYSTEM_MEMORY );
         BMUXlib_File_ASF_Destroy( hASFMux );
      }
      else
      {
         {
            BMUXlib_File_ASF_P_Packetizer_CreateSettings stPacketizerCreateSettings;

            BMUXlib_File_ASF_P_Packetizer_GetDefaultCreateSettings( &stPacketizerCreateSettings );

            rc = BERR_TRACE( BMUXlib_File_ASF_P_Packetizer_Create( &hASFMux->hPacketizer, &stPacketizerCreateSettings ) );

            if ( BERR_SUCCESS != rc )
            {
               BMUXlib_File_ASF_Destroy( hASFMux );
            }
            else
            {
               *phASFMux = hASFMux;
               /* fill in the signature in the context */
               hASFMux->uiSignature = BMUXLIB_FILE_ASF_P_SIGNATURE_CONTEXT;

               /* Reset State */
               BMUXlib_File_ASF_P_Reset( hASFMux );

               rc = BERR_TRACE( BERR_SUCCESS );
            }
         }
      }
   }
   else
   {
      rc = BERR_TRACE( BERR_OUT_OF_SYSTEM_MEMORY );
   }

   BDBG_LEAVE( BMUXlib_File_ASF_Create );

   return BERR_TRACE( rc );
}

void BMUXlib_File_ASF_Destroy(
            BMUXlib_File_ASF_Handle          hASFMux)
{
   BDBG_ENTER( BMUXlib_File_ASF_Destroy );

   BDBG_ASSERT( hASFMux );

   /* the following signifies an attempt to free up something that was either
      a) not created by Create()
      b) has already been destroyed
   */
   BDBG_ASSERT(BMUXLIB_FILE_ASF_P_SIGNATURE_CONTEXT == hASFMux->uiSignature);

   if ( NULL != hASFMux->hPacketizer )
   {
      BMUXlib_File_ASF_P_Packetizer_Destroy( hASFMux->hPacketizer );
      hASFMux->hPacketizer = NULL;
   }

   if ( NULL != hASFMux->pHeaderBuffer )
   {
      BKNI_Free( hASFMux->pHeaderBuffer );
      hASFMux->pHeaderBuffer = NULL;
   }

   /* clear signature (prevents accidental reuse of memory) */
   hASFMux->uiSignature = 0;

   BKNI_Free( hASFMux );

   BDBG_LEAVE( BMUXlib_File_ASF_Destroy );
}

/****************************
*   Start/Finish/Stop API   *
****************************/

void BMUXlib_File_ASF_GetDefaultStartSettings(
            BMUXlib_File_ASF_StartSettings  *pStartSettings)
{
   BDBG_ENTER( BMUXlib_File_ASF_GetDefaultStartSettings );

   BDBG_ASSERT( pStartSettings );

   BKNI_Memset( pStartSettings,
                0,
                sizeof( BMUXlib_File_ASF_StartSettings )
              );

   pStartSettings->uiSignature = BMUXLIB_FILE_ASF_P_SIGNATURE_STARTSETTINGS;

   BDBG_LEAVE( BMUXlib_File_ASF_GetDefaultStartSettings );
}

BERR_Code BMUXlib_File_ASF_Start(
            BMUXlib_File_ASF_Handle          hASFMux,
      const BMUXlib_File_ASF_StartSettings  *pStartSettings)
{
   BERR_Code rc = BERR_UNKNOWN;

   BDBG_ENTER( BMUXlib_File_ASF_Start );

   BDBG_ASSERT( hASFMux );
   BDBG_ASSERT( BMUXLIB_FILE_ASF_P_SIGNATURE_CONTEXT == hASFMux->uiSignature);
   BDBG_ASSERT( pStartSettings );
   BDBG_ASSERT( BMUXLIB_FILE_ASF_P_SIGNATURE_STARTSETTINGS == pStartSettings->uiSignature );

   rc = BERR_SUCCESS;

   hASFMux->status.stStartSettings = *pStartSettings;

   /* NOTE: the stStorage member of start settings is currently not used - it is present
      for future support of index generation */

   /* verify that an output interface has been supplied */
   if ((NULL == pStartSettings->stOutput.pfAddDescriptors) ||
       (NULL == pStartSettings->stOutput.pfGetCompleteDescriptors))
      return BERR_TRACE(BERR_INVALID_PARAMETER);

   if ( BMUXlib_State_eStopped == BMUXLIB_FILE_ASF_P_GET_MUX_STATE(hASFMux) )
   {
      unsigned uiInputIndex;
      BMUXlib_Input_CreateSettings stInputCreateSettings;
      BMUXlib_Input_Handle aInputTable[BMUXLIB_FILE_ASF_P_MAX_INPUTS];

      /* Check for active video inputs */
      for ( uiInputIndex = 0; ( uiInputIndex < BMUXLIB_FILE_ASF_MAX_VIDEO_INPUTS ) && ( uiInputIndex < hASFMux->status.stStartSettings.uiNumVideoInputs ); uiInputIndex++ )
      {
         unsigned int uiStreamNum;

         /* verify the interface settings */
         if ((NULL == hASFMux->status.stStartSettings.stVideoInputs[uiInputIndex].stInterface.fGetBufferDescriptors) ||
            (NULL == hASFMux->status.stStartSettings.stVideoInputs[uiInputIndex].stInterface.fConsumeBufferDescriptors) ||
            (NULL == hASFMux->status.stStartSettings.stVideoInputs[uiInputIndex].stInterface.fGetBufferStatus) ||
            /* verify stream number is valid */
            (0 == hASFMux->status.stStartSettings.stVideoInputs[uiInputIndex].uiStreamNumber) ||
            (hASFMux->status.stStartSettings.stVideoInputs[uiInputIndex].uiStreamNumber > 127))
         {
            BDBG_ERR(("Video Input [%d] settings are invalid", uiInputIndex));
            BMUXlib_File_ASF_P_Reset( hASFMux );
            return BERR_TRACE( BERR_INVALID_PARAMETER );
         }

         switch ( hASFMux->status.stStartSettings.stVideoInputs[uiInputIndex].stInterface.stBufferInfo.eProtocol )
         {
            case BAVC_VideoCompressionStd_eVC1SimpleMain:
            case BAVC_VideoCompressionStd_eH264:
               break;

            default:
               BDBG_ERR(("Unsupported protocol: %d", hASFMux->status.stStartSettings.stVideoInputs[uiInputIndex].stInterface.stBufferInfo.eProtocol));
               BMUXlib_File_ASF_P_Reset( hASFMux );
               return BERR_TRACE( BERR_INVALID_PARAMETER );
         }

         /* verify stream number is not already in use... */
         for (uiStreamNum = 0; uiStreamNum < hASFMux->status.uiNumActiveInputs; uiStreamNum++)
            if (hASFMux->status.astActiveInputs[uiStreamNum].uiStreamNumber == hASFMux->status.stStartSettings.stVideoInputs[uiInputIndex].uiStreamNumber)
            {
               BDBG_ERR(("Duplicate stream numbers are not allowed"));
               BMUXlib_File_ASF_P_Reset( hASFMux );
               return BERR_TRACE( BERR_INVALID_PARAMETER );
            }


         BMUXlib_Input_GetDefaultCreateSettings( &stInputCreateSettings );
         stInputCreateSettings.eType = BMUXlib_Input_Type_eVideo;
         stInputCreateSettings.interface.stVideo = hASFMux->status.stStartSettings.stVideoInputs[uiInputIndex].stInterface;
         stInputCreateSettings.eBurstMode = BMUXlib_Input_BurstMode_eFrame;
         stInputCreateSettings.pMetadata = &hASFMux->status.astActiveInputs[hASFMux->status.uiNumActiveInputs];
         stInputCreateSettings.uiMuxId = hASFMux->stCreateSettings.uiMuxId;
         stInputCreateSettings.uiTypeInstance = uiInputIndex;

         rc = BERR_TRACE( BMUXlib_Input_Create( &hASFMux->status.astActiveInputs[hASFMux->status.uiNumActiveInputs].hInput,
                               &stInputCreateSettings
                             ));

         if ( BERR_SUCCESS != rc )
         {
            BDBG_ERR(("Error creating input"));
            BMUXlib_File_ASF_P_Reset( hASFMux );
            return BERR_TRACE( rc );
         }
         else
         {
            hASFMux->status.astActiveInputs[hASFMux->status.uiNumActiveInputs].uiStreamNumber = hASFMux->status.stStartSettings.stVideoInputs[uiInputIndex].uiStreamNumber;
            hASFMux->status.astActiveInputs[hASFMux->status.uiNumActiveInputs].eType = BMUXlib_Input_Type_eVideo;
            hASFMux->status.astActiveInputs[hASFMux->status.uiNumActiveInputs].bufferInfo.pstVideo = &hASFMux->status.stStartSettings.stVideoInputs[uiInputIndex].stInterface.stBufferInfo;
            aInputTable[hASFMux->status.uiNumActiveInputs] = hASFMux->status.astActiveInputs[hASFMux->status.uiNumActiveInputs].hInput;
            hASFMux->status.uiNumActiveInputs++;
         }
      }

      /* Check for active audio inputs */
      for ( uiInputIndex = 0; ( uiInputIndex < BMUXLIB_FILE_ASF_MAX_AUDIO_INPUTS ) && ( uiInputIndex < hASFMux->status.stStartSettings.uiNumAudioInputs ); uiInputIndex++ )
      {
         unsigned int uiStreamNum;

         /* verify the interface settings */
         if ((NULL == hASFMux->status.stStartSettings.stAudioInputs[uiInputIndex].stInterface.fGetBufferDescriptors) ||
            (NULL == hASFMux->status.stStartSettings.stAudioInputs[uiInputIndex].stInterface.fConsumeBufferDescriptors) ||
            (NULL == hASFMux->status.stStartSettings.stAudioInputs[uiInputIndex].stInterface.fGetBufferStatus) ||
            /* verify stream number is valid */
            (0 == hASFMux->status.stStartSettings.stAudioInputs[uiInputIndex].uiStreamNumber) ||
            (hASFMux->status.stStartSettings.stAudioInputs[uiInputIndex].uiStreamNumber > 127))
         {
            BDBG_ERR(("Audio Input [%d] settings are invalid", uiInputIndex));
            BMUXlib_File_ASF_P_Reset( hASFMux );
            return BERR_TRACE( BERR_INVALID_PARAMETER );
         }

         switch ( hASFMux->status.stStartSettings.stAudioInputs[uiInputIndex].stInterface.stBufferInfo.eProtocol )
         {
            case BAVC_AudioCompressionStd_eAacAdts:           /* AAC (ADTS) */
            case BAVC_AudioCompressionStd_eAacPlusAdts:       /* AAC Plus (HE/SBR) ADTS */
            case BAVC_AudioCompressionStd_eMpegL3:            /* MP3 */
            case BAVC_AudioCompressionStd_eWmaStd:            /* WMA Standard */
               break;

            default:
               BDBG_ERR(("Unsupported protocol: %d", hASFMux->status.stStartSettings.stAudioInputs[uiInputIndex].stInterface.stBufferInfo.eProtocol));
               BMUXlib_File_ASF_P_Reset( hASFMux );
               return BERR_TRACE( BERR_INVALID_PARAMETER );
         }

         /* verify stream number is not already in use... */
         for (uiStreamNum = 0; uiStreamNum < hASFMux->status.uiNumActiveInputs; uiStreamNum++)
            if (hASFMux->status.astActiveInputs[uiStreamNum].uiStreamNumber == hASFMux->status.stStartSettings.stAudioInputs[uiInputIndex].uiStreamNumber)
            {
               BDBG_ERR(("Duplicate stream numbers are not allowed"));
               BMUXlib_File_ASF_P_Reset( hASFMux );
               return BERR_TRACE( BERR_INVALID_PARAMETER );
            }


         BMUXlib_Input_GetDefaultCreateSettings( &stInputCreateSettings );
         stInputCreateSettings.eType = BMUXlib_Input_Type_eAudio;
         stInputCreateSettings.interface.stAudio = hASFMux->status.stStartSettings.stAudioInputs[uiInputIndex].stInterface;
         stInputCreateSettings.eBurstMode = BMUXlib_Input_BurstMode_eFrame;
         stInputCreateSettings.pMetadata = &hASFMux->status.astActiveInputs[hASFMux->status.uiNumActiveInputs];
         stInputCreateSettings.uiMuxId = hASFMux->stCreateSettings.uiMuxId;
         stInputCreateSettings.uiTypeInstance = uiInputIndex;

         rc = BERR_TRACE( BMUXlib_Input_Create( &hASFMux->status.astActiveInputs[hASFMux->status.uiNumActiveInputs].hInput,
                               &stInputCreateSettings
                             ));

         if ( BERR_SUCCESS != rc )
         {
            BDBG_ERR(("Error creating input"));
            BMUXlib_File_ASF_P_Reset( hASFMux );
            return BERR_TRACE( rc );
         }
         else
         {
            hASFMux->status.astActiveInputs[hASFMux->status.uiNumActiveInputs].uiStreamNumber = hASFMux->status.stStartSettings.stAudioInputs[uiInputIndex].uiStreamNumber;
            hASFMux->status.astActiveInputs[hASFMux->status.uiNumActiveInputs].eType = BMUXlib_Input_Type_eAudio;
            hASFMux->status.astActiveInputs[hASFMux->status.uiNumActiveInputs].bufferInfo.pstAudio = &hASFMux->status.stStartSettings.stAudioInputs[uiInputIndex].stInterface.stBufferInfo;
            aInputTable[hASFMux->status.uiNumActiveInputs] = hASFMux->status.astActiveInputs[hASFMux->status.uiNumActiveInputs].hInput;
            hASFMux->status.uiNumActiveInputs++;
         }
      }

      if ( 0 == hASFMux->status.uiNumActiveInputs )
      {
         BDBG_ERR(("No active inputs"));
         BMUXlib_File_ASF_P_Reset( hASFMux );
         return BERR_TRACE( BERR_INVALID_PARAMETER );
      }

      {
         BMUXlib_Output_CreateSettings stOutputCreateSettings;

         BMUXlib_Output_GetDefaultCreateSettings( &stOutputCreateSettings );

         stOutputCreateSettings.uiCount = BMUXLIB_FILE_ASF_P_MAX_OUTPUT_DESCRIPTORS;
         stOutputCreateSettings.stStorage = hASFMux->status.stStartSettings.stOutput;

         rc = BMUXlib_Output_Create( &hASFMux->hOutput, &stOutputCreateSettings );
      }
      if ( BERR_SUCCESS == rc)
      {
         /* create the input group for the active inputs */
         BMUXlib_InputGroup_CreateSettings stCreateSettings;
         BMUXlib_InputGroup_GetDefaultCreateSettings(&stCreateSettings);
         stCreateSettings.uiInputCount = hASFMux->status.uiNumActiveInputs;
         stCreateSettings.pInputTable = aInputTable;
         rc = BMUXlib_InputGroup_Create(&hASFMux->hInputGroup, &stCreateSettings);
         if (BERR_SUCCESS == rc)
         {
            BMUXlib_InputGroup_Settings stSettings;
            BMUXlib_InputGroup_GetSettings(hASFMux->hInputGroup, &stSettings);
            /* ASF needs to wait for all inputs during input selection  */
            stSettings.bWaitForAllInputs = true;
            rc = BMUXlib_InputGroup_SetSettings(hASFMux->hInputGroup, &stSettings);
         }
      }

      if ( BERR_SUCCESS == rc )
      {
         BMUXLIB_FILE_ASF_P_SET_MUX_STATE(hASFMux, BMUXlib_State_eStarted);
         hASFMux->status.eMuxStartedState = BMUXlib_File_ASF_StartedState_eWaitForMetadata;
      }
      else
      {
         BDBG_ERR(("Error creating output"));
         BMUXlib_File_ASF_P_Reset( hASFMux );
      }
   }
   else
   {
      BDBG_ERR(("Must be in a stopped state before starting."));
      rc = BERR_TRACE( BERR_INVALID_PARAMETER );
   }

   BDBG_LEAVE( BMUXlib_File_ASF_Start );

   return BERR_TRACE( rc );
}

void BMUXlib_File_ASF_GetDefaultFinishSettings(
            BMUXlib_File_ASF_FinishSettings *pFinishSettings)
{
   BDBG_ENTER( BMUXlib_File_ASF_GetDefaultFinishSettings );

   BDBG_ASSERT( pFinishSettings );

   *pFinishSettings = s_stDefaultFinishSettings;

   BDBG_LEAVE( BMUXlib_File_ASF_GetDefaultFinishSettings );
}

BERR_Code BMUXlib_File_ASF_Finish(
            BMUXlib_File_ASF_Handle          hASFMux,
      const BMUXlib_File_ASF_FinishSettings *pFinishSettings)
{
   BERR_Code rc = BERR_UNKNOWN;

   BDBG_ENTER( BMUXlib_File_ASF_Finish );

   BDBG_ASSERT( hASFMux );
   BDBG_ASSERT( BMUXLIB_FILE_ASF_P_SIGNATURE_CONTEXT == hASFMux->uiSignature);
   BDBG_ASSERT( pFinishSettings );
   BDBG_ASSERT( BMUXLIB_FILE_ASF_P_SIGNATURE_FINISHSETTINGS == pFinishSettings->uiSignature );

   switch( BMUXLIB_FILE_ASF_P_GET_MUX_STATE(hASFMux) )
   {
      case BMUXlib_State_eStarted:
         {
            hASFMux->status.stFinishSettings = *pFinishSettings;

            switch (hASFMux->status.stFinishSettings.eFinishMode)
            {
               case BMUXlib_FinishMode_ePrepareForStop:
                  /* mux is now "finishing input" ... */
                  BMUXLIB_FILE_ASF_P_SET_MUX_STATE(hASFMux, BMUXlib_State_eFinishingInput);
                  rc = BERR_SUCCESS;
                  break;
               default:
                  /* unrecognized finish mode - do nothing */
                  rc = BERR_TRACE( BERR_INVALID_PARAMETER );
                  break;
            }
         }
         break;

      case BMUXlib_State_eFinishingInput:
      case BMUXlib_State_eFinishingOutput:
      case BMUXlib_State_eFinished:
         /* do nothing if invoked from these states - already finishing! */
         rc = BERR_SUCCESS;
         break;

      default:
         rc = BERR_TRACE( BERR_NOT_SUPPORTED );
         break;
   }

   BDBG_LEAVE( BMUXlib_File_ASF_Finish );

   return BERR_TRACE( rc );
}

BERR_Code BMUXlib_File_ASF_Stop(
            BMUXlib_File_ASF_Handle          hASFMux)
{
   BERR_Code rc = BERR_UNKNOWN;

   BDBG_ENTER( BMUXlib_File_ASF_Stop );

   BDBG_ASSERT( hASFMux );
   BDBG_ASSERT( BMUXLIB_FILE_ASF_P_SIGNATURE_CONTEXT == hASFMux->uiSignature);

   switch ( BMUXLIB_FILE_ASF_P_GET_MUX_STATE(hASFMux) )
   {
      case BMUXlib_State_eStarted:
      case BMUXlib_State_eFinishingInput:
         /* coverity[unterminated_case] */
      case BMUXlib_State_eFinishingOutput:
         BDBG_WRN(("Not finished yet. Output may be corrupt!"));

         /* coverity[fallthrough] */
      case BMUXlib_State_eFinished:
         BMUXLIB_FILE_ASF_P_SET_MUX_STATE(hASFMux, BMUXlib_State_eStopped);
         rc = BERR_TRACE( BERR_SUCCESS );
         break;

      case BMUXlib_State_eStopped:
         BDBG_WRN(("Already Stopped"));
         rc = BERR_TRACE( BERR_SUCCESS );
         break;

      default:
         rc = BERR_TRACE( BERR_INVALID_PARAMETER );
         break;
   }

   /* Reset State */
   BMUXlib_File_ASF_P_Reset( hASFMux );

   BDBG_LEAVE( BMUXlib_File_ASF_Stop );

   return BERR_TRACE( rc );
}

/*************************
*    Mux Execute API     *
*************************/

BERR_Code BMUXlib_File_ASF_DoMux(
            BMUXlib_File_ASF_Handle          hASFMux,
            BMUXlib_DoMux_Status            *pStatus)
{
   BERR_Code rc = BERR_UNKNOWN;

   BDBG_ENTER( BMUXlib_File_ASF_DoMux );

   BDBG_ASSERT( hASFMux );
   BDBG_ASSERT( pStatus );
   BDBG_ASSERT( BMUXLIB_FILE_ASF_P_SIGNATURE_CONTEXT == hASFMux->uiSignature);

   BKNI_Memset( pStatus, 0, sizeof( BMUXlib_DoMux_Status ) );

   rc = BERR_SUCCESS;
   hASFMux->status.eBlockedState = BMUXlib_File_ASF_P_Blocked_State_eNotBlocked;

   switch( BMUXLIB_FILE_ASF_P_GET_MUX_STATE(hASFMux) )
   {
      case BMUXlib_State_eStopped:
         /* either not started, or done muxing ... do nothing */
         /* requires a Start() to move state */
         rc = BERR_SUCCESS;
         break;

      case BMUXlib_State_eStarted:
         /* requires a Finish() or Stop() to move state (will stay here otherwise, even if inputs "dry up") */
         switch ( hASFMux->status.eMuxStartedState )
         {
            rc = BERR_SUCCESS;

            case BMUXlib_File_ASF_StartedState_eWaitForMetadata:
               BERR_TRACE( BMUXlib_File_ASF_P_UpdateInputDescriptors( hASFMux ) );
               if ( true == BMUXlib_File_ASF_P_WaitForMetadata( hASFMux ) )
               {
                  BDBG_MSG(("StartedState_eWaitForMetadata --> StartedState_eWriteHeader"));
                  hASFMux->status.eMuxStartedState = BMUXlib_File_ASF_StartedState_eWriteHeader;
               }
               break;

            case BMUXlib_File_ASF_StartedState_eWriteHeader:
               if ( true == BMUXlib_File_ASF_P_Header_WriteHeader( hASFMux ) )
               {
                  BDBG_MSG(("StartedState_eWriteHeader --> StartedState_eWriteMedia"));
                  hASFMux->status.eMuxStartedState = BMUXlib_File_ASF_StartedState_eWriteMedia;
               }
               BMUXlib_Output_ProcessNewDescriptors( hASFMux->hOutput );
               break;

            case BMUXlib_File_ASF_StartedState_eWriteMedia:
               BMUXlib_Output_ProcessCompletedDescriptors( hASFMux->hOutput );
               BMUXlib_File_ASF_P_ProcessCompletedInputDescriptors( hASFMux );
               BERR_TRACE( BMUXlib_File_ASF_P_UpdateInputDescriptors( hASFMux ) );

               /* SW7425-998: Auto-Finish if EOS is seen on all active inputs */
               if ( false == BMUXlib_File_ASF_P_ProcessInputDescriptors( hASFMux ) )
               {
                  BMUXlib_File_ASF_FinishSettings stFinishSettings;

                  BMUXlib_File_ASF_GetDefaultFinishSettings( &stFinishSettings );

                  BDBG_MSG(("Auto Finish"));
                  BMUXlib_File_ASF_Finish(
                           hASFMux,
                           &stFinishSettings
                           );
               }
               BMUXlib_Output_ProcessNewDescriptors( hASFMux->hOutput );
               break;

            default:
               BDBG_ERR(("Unknown State"));
               rc = BERR_TRACE( BERR_UNKNOWN );
         }
         break;

      case BMUXlib_State_eFinishingInput:
         BMUXlib_Output_ProcessCompletedDescriptors( hASFMux->hOutput );
         BMUXlib_File_ASF_P_ProcessCompletedInputDescriptors( hASFMux );
         BERR_TRACE( BMUXlib_File_ASF_P_UpdateInputDescriptors( hASFMux ) );
         if ( ( BMUXlib_File_ASF_StartedState_eWriteMedia != hASFMux->status.eMuxStartedState )
              || ( false == BMUXlib_File_ASF_P_ProcessInputDescriptors( hASFMux ) ) )
         {
            BDBG_MSG(("State_eFinishingInput --> State_eFinishingOutput"));
            BMUXLIB_FILE_ASF_P_SET_MUX_STATE(hASFMux, BMUXlib_State_eFinishingOutput);
         }
         BMUXlib_Output_ProcessNewDescriptors( hASFMux->hOutput );

         break;

      case BMUXlib_State_eFinishingOutput:
         BMUXlib_Output_ProcessCompletedDescriptors( hASFMux->hOutput );
         BMUXlib_File_ASF_P_ProcessCompletedInputDescriptors( hASFMux );

         /* Populate missing header info if not in broadcast-only mode */
         if ( false == hASFMux->status.stStartSettings.bBroadcastOnly )
         {
            BMUXlib_File_ASF_P_Header_PostProcessHeader( hASFMux );
         }
         BMUXlib_Output_ProcessNewDescriptors( hASFMux->hOutput );

         if ( false == BMUXlib_Output_IsDescriptorPendingCompletion( hASFMux->hOutput ) )
         {
            BDBG_MSG(("State_eFinishingOutput --> State_eFinished"));
            BMUXLIB_FILE_ASF_P_SET_MUX_STATE(hASFMux, BMUXlib_State_eFinished);
         }
         break;

      case BMUXlib_State_eFinished:
         /* final output completed and is valid */
         /* nothing more to be done - need a Stop() call to move state */
         rc = BERR_TRACE( BERR_SUCCESS );
         break;

      default:
         /* error: invalid state */
         rc = BERR_TRACE( BERR_NOT_SUPPORTED );
         break;
   }

   if ( (NULL != hASFMux->hOutput) && (true == BMUXlib_Output_IsDescriptorPendingQueue( hASFMux->hOutput )) )
   {
      hASFMux->status.eBlockedState = BMUXlib_File_ASF_P_Blocked_State_eOutput;
   }

   if ( BMUXlib_File_ASF_P_Blocked_State_eOutput == hASFMux->status.eBlockedState )
   {
      pStatus->uiNextExecutionTime = 0;
      pStatus->bBlockedOutput = true;
      BDBG_MSG(("Output is blocked!"));
   }
   else
   {
      pStatus->uiNextExecutionTime = BMUXLIB_FILE_ASF_P_MUX_SERVICE_PERIOD;
   }
   pStatus->eState = BMUXLIB_FILE_ASF_P_GET_MUX_STATE(hASFMux);

   BDBG_LEAVE( BMUXlib_File_ASF_DoMux );

   return BERR_TRACE( rc );
}

static void BMUXlib_File_ASF_P_Reset(
   BMUXlib_File_ASF_Handle          hASFMux
   )
{
   BDBG_ENTER( BMUXlib_File_ASF_P_Reset );

   if ( NULL != hASFMux->hOutput )
   {
      BMUXlib_Output_Destroy( hASFMux->hOutput );
      hASFMux->hOutput = NULL;
   }

   if ( NULL != hASFMux->hInputGroup )
   {
      BMUXlib_InputGroup_Destroy( hASFMux->hInputGroup );
      hASFMux->hInputGroup = NULL;
   }

   {
      unsigned uiInputIndex;

      for ( uiInputIndex = 0; uiInputIndex < hASFMux->status.uiNumActiveInputs; uiInputIndex++ )
      {
         if ( NULL != hASFMux->status.astActiveInputs[uiInputIndex].hInput )
         {
            BMUXlib_Input_Destroy( hASFMux->status.astActiveInputs[uiInputIndex].hInput );
            hASFMux->status.astActiveInputs[uiInputIndex].hInput = NULL;
         }
      }
   }

   BKNI_Memset( &hASFMux->status, 0, sizeof( hASFMux->status ) );

   BMUXLIB_FILE_ASF_P_SET_MUX_STATE(hASFMux, BMUXlib_State_eStopped);

   BKNI_Memset( hASFMux->pHeaderBuffer, 0, hASFMux->uiHeaderBufferSize );

   BMUXlib_File_ASF_P_Packetizer_Reset( hASFMux->hPacketizer );

   BDBG_LEAVE( BMUXlib_File_ASF_P_Reset );
}

