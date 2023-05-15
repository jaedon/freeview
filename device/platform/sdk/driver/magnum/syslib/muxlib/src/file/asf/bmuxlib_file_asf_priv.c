/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmuxlib_file_asf_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/21 $
 * $brcm_Date: 5/29/12 1:50p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/muxlib/7425/file/asf/bmuxlib_file_asf_priv.c $
 * 
 * Hydra_Software_Devel/21   5/29/12 1:50p delkert
 * SW7425-2567: Add return code checks to fix coverity errors
 *
 * Hydra_Software_Devel/20   2/16/12 1:11p nilesh
 * SW7425-2315: Removed unneccessary assert
 *
 * Hydra_Software_Devel/19   2/7/12 11:17a delkert
 * SW7425-2315: Fix coverity error (check return)
 *
 * Hydra_Software_Devel/18   2/2/12 5:08p nilesh
 * SW7425-2315: Added support for
 * BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EMPTY_FRAME
 *
 * Hydra_Software_Devel/17   12/20/11 1:45p nilesh
 * SW7425-1869: Merged BMUXlib_InputGroup support
 *
 * Hydra_Software_Devel/SW7425-1869/1   12/9/11 8:40a delkert
 * SW7425-1869: Modify ASF to use InputGroup for input selection.
 *
 * Hydra_Software_Devel/16   12/9/11 3:10p delkert
 * SW7425-1869: Fix usage of bit-wise operation when logical should be
 * used.
 *
 * Hydra_Software_Devel/15   12/8/11 4:23p delkert
 * SW7425-1869: Remove break statement which causes
 * ProcessInputDescriptors() to spin (block) in eSelectInput state
 *
 * Hydra_Software_Devel/14   11/10/11 12:49p nilesh
 * SW7425-1691: Merge to mainline
 *
 * Hydra_Software_Devel/SW7425-1691/1   11/10/11 11:56a nilesh
 * SW7425-1691: Added support for stop-->start without requiring
 * destroy/create
 *
 * Hydra_Software_Devel/13   10/5/11 5:56p delkert
 * SW7425-1415: Fix coverity issues
 *
 * Hydra_Software_Devel/12   10/5/11 4:47p nilesh
 * SW7425-721: Updated to use BMUXlib_Input_GetNextInput()
 *
 * Hydra_Software_Devel/11   9/1/11 12:08p nilesh
 * SW7425-720: Integrated new ASF Packetizer
 *
 * Hydra_Software_Devel/10   8/24/11 3:51p nilesh
 * SW7425-1185: Presentation Time stored in replicated data is now the DTS
 * and not PTS
 *
 * Hydra_Software_Devel/9   8/12/11 2:37p nilesh
 * SW7425-1094: Added check for when input PTS values are too far apart
 *
 * Hydra_Software_Devel/8   8/12/11 11:05a nilesh
 * SW7425-936: Reset blocked state in DoMux
 *
 * Hydra_Software_Devel/7   6/14/11 12:47p nilesh
 * SW7425-325: Added support for BMUXlib_DoMux_Status.bBlockedOutput
 *
 * Hydra_Software_Devel/6   5/19/11 5:34p nilesh
 * SW7425-477: Fixed AAC Audio
 *
 * Hydra_Software_Devel/5   5/17/11 3:41p nilesh
 * SW7425-447: Added AAC Audio Support
 *
 * Hydra_Software_Devel/4   5/16/11 3:27p nilesh
 * SW7425-447: Set packet media object size.  Set payload parsing padding
 * size.
 *
 * Hydra_Software_Devel/3   5/11/11 4:03p nilesh
 * SW7425-447: Offset presentation by pre-roll
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
#include "bdbg.h"           /* debug interface */

#include "bmuxlib_file_asf.h"
#include "bmuxlib_file_asf_priv.h"

#include "bmuxlib_file_asf_header.h"

#include "bmuxlib_list.h"
#include "bmuxlib_input.h"

BDBG_MODULE(BMUXLIB_FILE_ASF_PRIV);
BDBG_FILE_MODULE(BMUXLIB_FILE_ASF_STATE);

bool BMUXlib_File_ASF_P_ProcessMetadata(
         BMUXlib_File_ASF_P_Input *pInput,
         BMUXlib_Input_Descriptor *pstInputDescriptor
         )
{
   BDBG_ENTER( BMUXlib_File_ASF_P_ProcessMetadata );

   BDBG_ASSERT( BMUXLIB_INPUT_DESCRIPTOR_IS_METADATA( pstInputDescriptor ) );

   switch ( pInput->eType )
   {
      case BMUXlib_Input_Type_eVideo:
      {
         if ( BAVC_VideoMetadataType_eCommon == pstInputDescriptor->descriptor.pstVideo->uiDataUnitType )
         {
            BAVC_VideoMetadataDescriptor *pMetadata = (BAVC_VideoMetadataDescriptor * ) ( pstInputDescriptor->descriptor.pstCommon->uiOffset + (unsigned) pstInputDescriptor->pBaseAddress );

            pInput->stCodecSpecificData.video.data.stCommon = *pMetadata;
            pInput->stCodecSpecificData.video.data.bCommonValid = true;
         }
         else
         {
            BDBG_WRN(("Unknown Video Metadata Type (%d)", pstInputDescriptor->descriptor.pstVideo->uiDataUnitType));
         }

         /* TODO: When adding VC1 metadata, need to check to make sure VC1 metadata
          * exists, too
          */
         if ( true == pInput->stCodecSpecificData.video.data.bCommonValid )
         {
            pInput->bCodecSpecificDataValid = true;
         }
      }
         break;
      case BMUXlib_Input_Type_eAudio:
         if ( BAVC_AudioMetadataType_eCommon == pstInputDescriptor->descriptor.pstAudio->uiDataUnitType )
         {
            BAVC_AudioMetadataDescriptor *pMetadata = ( BAVC_AudioMetadataDescriptor * ) ( pstInputDescriptor->descriptor.pstCommon->uiOffset + (unsigned) pstInputDescriptor->pBaseAddress );

            pInput->stCodecSpecificData.audio.data.stCommon = *pMetadata;
            pInput->stCodecSpecificData.audio.data.bCommonValid = true;
         }
         else
         {
            BDBG_WRN(("Unknown Audio Metadata Type (%d)", pstInputDescriptor->descriptor.pstAudio->uiDataUnitType));
         }

         if ( true == pInput->stCodecSpecificData.audio.data.bCommonValid )
         {
            pInput->bCodecSpecificDataValid = true;
         }

         break;
      default:
         BDBG_ERR(("Unknown input type"));
   }

   BDBG_LEAVE( BMUXlib_File_ASF_P_ProcessMetadata );

   return pInput->bCodecSpecificDataValid;
}

/* BMUXlib_File_ASF_WaitForMetadata - will return true when the metadata for all inputs
 * has been parsed
 */
bool
BMUXlib_File_ASF_P_WaitForMetadata(
         BMUXlib_File_ASF_Handle hASFMux
         )
{
   bool bDone = true;
   unsigned uiInputIndex;

   BDBG_ENTER( BMUXlib_File_ASF_P_WaitForMetadata );

   /* Process input */
   for ( uiInputIndex = 0; ( uiInputIndex < hASFMux->status.uiNumActiveInputs ); uiInputIndex++ )
   {
      BMUXlib_File_ASF_P_Input *pInput = &hASFMux->status.astActiveInputs[uiInputIndex];

      if ( false == pInput->bCodecSpecificDataValid )
      {
         BMUXlib_Input_Descriptor stInputDescriptor;

         if ( true == BMUXlib_Input_IsDescriptorAvailable( pInput->hInput ) )
         {
            bool bDescAvail;
            /* NOTE: the following should not fail, since we check for descriptors */
            bDescAvail = BMUXlib_Input_GetNextDescriptor( pInput->hInput, &stInputDescriptor );
            BDBG_ASSERT(bDescAvail == true);

            if ( BMUXLIB_INPUT_DESCRIPTOR_IS_METADATA(&stInputDescriptor) )
            {
               pInput->bCodecSpecificDataValid = BMUXlib_File_ASF_P_ProcessMetadata( pInput, &stInputDescriptor );
            }
            else
            {
               BDBG_ERR(("Dropping non-metadata descriptor"));
            }

            BMUXlib_Input_ConsumeDescriptors( pInput->hInput, 1);
         }
      }
   }

   for ( uiInputIndex = 0; ( uiInputIndex < hASFMux->status.uiNumActiveInputs ); uiInputIndex++ )
   {
      BMUXlib_File_ASF_P_Input *pInput = &hASFMux->status.astActiveInputs[uiInputIndex];

      if ( false == pInput->bCodecSpecificDataValid )
      {
         bDone = false;
         break;
      }
   }

   BDBG_LEAVE( BMUXlib_File_ASF_P_WaitForMetadata );

   return bDone;
}

/* Update Available Input Descriptors */
BERR_Code
BMUXlib_File_ASF_P_UpdateInputDescriptors(
         BMUXlib_File_ASF_Handle hASFMux
         )
{
   BERR_Code rc = BERR_SUCCESS;
   unsigned uiInputNum;

   BDBG_ENTER( BMUXlib_File_ASF_P_UpdateInputDescriptors );

   for ( uiInputNum = 0; uiInputNum < hASFMux->status.uiNumActiveInputs; uiInputNum++ )
   {
      rc = BMUXlib_Input_ProcessNewDescriptors( hASFMux->status.astActiveInputs[uiInputNum].hInput );
      if ( BERR_SUCCESS != rc ) break;
   }

   BDBG_LEAVE( BMUXlib_File_ASF_P_UpdateInputDescriptors );

   return BERR_TRACE( rc );
}

BERR_Code
BMUXlib_File_ASF_P_ProcessCompletedInputDescriptors(
         BMUXlib_File_ASF_Handle hASFMux
         )
{
   BERR_Code rc = BERR_SUCCESS;
   unsigned uiInputNum;

   BDBG_ENTER( BMUXlib_File_ASF_P_ProcessCompletedInputDescriptors );

   for ( uiInputNum = 0; uiInputNum < hASFMux->status.uiNumActiveInputs; uiInputNum++ )
   {
      unsigned uiCompletedDescriptors = 0;

      /* Find out how many contiguous descriptors have been completed */
      while ( hASFMux->status.astActiveInputs[uiInputNum].stReleaseQueue.uiReadOffset != hASFMux->status.astActiveInputs[uiInputNum].stReleaseQueue.uiWriteOffset )
      {
         if ( true == hASFMux->status.astActiveInputs[uiInputNum].stReleaseQueue.bReleased[hASFMux->status.astActiveInputs[uiInputNum].stReleaseQueue.uiReadOffset] )
         {
            uiCompletedDescriptors++;
            hASFMux->status.astActiveInputs[uiInputNum].stReleaseQueue.bReleased[hASFMux->status.astActiveInputs[uiInputNum].stReleaseQueue.uiReadOffset] = false;
            hASFMux->status.astActiveInputs[uiInputNum].stReleaseQueue.uiReadOffset++;
            hASFMux->status.astActiveInputs[uiInputNum].stReleaseQueue.uiReadOffset %= BMUXLIB_FILE_ASF_P_MAX_INPUT_DESCRIPTORS;
         }
         else
         {
            break;
         }
      }

      if ( 0 != uiCompletedDescriptors )
      {
         rc = BMUXlib_Input_ConsumeDescriptors(
                  hASFMux->status.astActiveInputs[uiInputNum].hInput,
                  uiCompletedDescriptors
                  );

         if ( BERR_SUCCESS != rc ) return BERR_TRACE( rc );
      }
   }

   BDBG_LEAVE( BMUXlib_File_ASF_P_ProcessCompletedInputDescriptors );

   return BERR_TRACE( rc );
}

void
BMUXlib_File_ASF_P_ProcessCompletedInputDescriptor(
         void* pPrivateData,
         const BMUXlib_Output_Descriptor *pOutputDescriptor
         )
{
   bool *pbReleased = (bool *) pPrivateData;

   BDBG_ENTER( BMUXlib_File_ASF_P_ProcessCompletedInputDescriptor);

   BSTD_UNUSED( pOutputDescriptor );

   *pbReleased = true;

   BDBG_LEAVE( BMUXlib_File_ASF_P_ProcessCompletedInputDescriptor );
}

bool
BMUXlib_File_ASF_P_ProcessInputDescriptors(
         BMUXlib_File_ASF_Handle hASFMux
         )
{
   bool bResult = true;

   BDBG_ENTER( BMUXlib_File_ASF_P_ProcessInputDescriptors );

   do {
      switch ( hASFMux->status.eProcessInputState )
      {
         case BMUXlib_File_ASF_P_ProcessInputState_eSelectInput: /* Here, we pick the input that has the frame with the smallest DTS */
         {
            BMUXlib_Input_Handle hSelectedInput = NULL;

            hASFMux->status.pCurrentInput = NULL;

            /* Decide which input to use based on DTS */
            BMUXlib_InputGroup_GetNextInput( hASFMux->hInputGroup, &hSelectedInput );

            if ( NULL != hSelectedInput )
            {
               BMUXlib_Input_CreateSettings stSettings;
               BMUXlib_Input_GetCreateSettings(hSelectedInput, &stSettings);
               BDBG_ASSERT(stSettings.pMetadata != NULL);
               hASFMux->status.pCurrentInput = stSettings.pMetadata;
            }

            if ( NULL != hASFMux->status.pCurrentInput )
            {
               BDBG_MODULE_MSG( BMUXLIB_FILE_ASF_STATE, ("eSelectInput --> eProcessInput (Stream: %d, Type: %d)", hASFMux->status.pCurrentInput->uiStreamNumber, hASFMux->status.pCurrentInput->eType) );
               hASFMux->status.eProcessInputState = BMUXlib_File_ASF_P_ProcessInputState_eProcessInput;
            }
            else
            {
               /* Stay in the current state */
               hASFMux->status.eBlockedState = BMUXlib_File_ASF_P_Blocked_State_eInput;
            }
         }
         break;

         case BMUXlib_File_ASF_P_ProcessInputState_eProcessInput:
            if ( ( true == BMUXlib_File_ASF_P_Packetizer_IsSpaceAvailable( hASFMux->hPacketizer ) )
                 && ( hASFMux->status.pCurrentInput->stReleaseQueue.uiReadOffset != ( ( hASFMux->status.pCurrentInput->stReleaseQueue.uiWriteOffset + 1) % BMUXLIB_FILE_ASF_P_MAX_INPUT_DESCRIPTORS ) ) )
            {
               BMUXlib_Input_Descriptor stInputDescriptor;
               BMUXlib_File_ASF_P_Packetizer_Descriptor stPacketizerDescriptor;
               BMUXlib_Output_CompletedCallbackInfo stCallbackInfo;
               unsigned uiDataOffset = 0;
               BMUXlib_File_ASF_P_Input *pCurrentInput = hASFMux->status.pCurrentInput;
               bool bDescAvail;

               BKNI_Memset( &stInputDescriptor, 0, sizeof( BMUXlib_Input_Descriptor ) );
               BKNI_Memset( &stPacketizerDescriptor, 0, sizeof( BMUXlib_File_ASF_P_Packetizer_Descriptor ) );
               BKNI_Memset( &stCallbackInfo, 0, sizeof( BMUXlib_Output_CompletedCallbackInfo ) );

               bDescAvail = BMUXlib_Input_GetNextDescriptor( pCurrentInput->hInput, &stInputDescriptor );
               /* In theory, this should never happen - indicates a programming problem */
               BDBG_ASSERT(bDescAvail == true);

               /* Set descriptors buffer address and length */
#ifdef BMUXlib_File_ASF_P_USE_AAC_RAW
               if ( BMUXlib_Input_Type_eAudio == pCurrentInput->eType )
               {
                  uiDataOffset = BMUXLIB_INPUT_DESCRIPTOR_LENGTH( &stInputDescriptor ) - BMUXLIB_INPUT_DESCRIPTOR_AUDIO_RAWLENGTH( &stInputDescriptor );
               }
#endif
               stPacketizerDescriptor.pBuffer = (void*)
                        ( (unsigned) stInputDescriptor.pBaseAddress
                          + BMUXLIB_INPUT_DESCRIPTOR_OFFSET( &stInputDescriptor )
                          + uiDataOffset);

               stPacketizerDescriptor.uiLength = BMUXLIB_INPUT_DESCRIPTOR_LENGTH( &stInputDescriptor ) - uiDataOffset;

               /* Set descriptor metadata */
               stPacketizerDescriptor.uiStreamNumber = pCurrentInput->uiStreamNumber;
               stPacketizerDescriptor.uiStreamOffset = pCurrentInput->uiCurrentStreamOffset;
               pCurrentInput->uiCurrentStreamOffset += stPacketizerDescriptor.uiLength;

               if ( true == BMUXLIB_INPUT_DESCRIPTOR_IS_EMPTYFRAME( &stInputDescriptor ) )
               {
                  /* Do nothing, the descriptor will be skipped */
               }
               else if ( true == BMUXLIB_INPUT_DESCRIPTOR_IS_FRAMESTART( &stInputDescriptor ) )
               {
                  pCurrentInput->stCurrentFrameInfo.bNewFrame = true;
                  pCurrentInput->stCurrentFrameInfo.bKeyFrame = BMUXLIB_INPUT_DESCRIPTOR_IS_KEYFRAME( &stInputDescriptor );
                  pCurrentInput->stCurrentFrameInfo.uiFrameNumber = pCurrentInput->uiCurrentFrameNumber;
                  pCurrentInput->uiCurrentFrameNumber++;
#if 0
                  pCurrentInput->stCurrentFrameInfo.uiSendTime = BMUXLIB_INPUT_DESCRIPTOR_ESCR( &stInputDescriptor ) / 27000;
#else
                  pCurrentInput->stCurrentFrameInfo.uiSendTime = BMUXLIB_INPUT_DESCRIPTOR_DTS( &stInputDescriptor ) / 90;
#endif

                  pCurrentInput->stCurrentFrameInfo.uiPresentationTime = BMUXLIB_INPUT_DESCRIPTOR_DTS( &stInputDescriptor ) / 90;
                  pCurrentInput->stCurrentFrameInfo.uiPresentationTime += BMUXlib_File_ASF_P_FilePropertiesObject_PREROLL_DEFAULT;
                  pCurrentInput->stCurrentFrameInfo.uiFrameSize = BMUXLIB_INPUT_DESCRIPTOR_FRAMESIZE( &stInputDescriptor ) - uiDataOffset;
               }
               else
               {
                  pCurrentInput->stCurrentFrameInfo.bNewFrame = false;
               }

               stPacketizerDescriptor.stFrameInfo = pCurrentInput->stCurrentFrameInfo;

               if ( 0 != stPacketizerDescriptor.uiLength )
               {
                  /* We're done with this input descriptor, so set a callback */
                  stCallbackInfo.pCallback = BMUXlib_File_ASF_P_ProcessCompletedInputDescriptor;
                  stCallbackInfo.pCallbackData = &pCurrentInput->stReleaseQueue.bReleased[pCurrentInput->stReleaseQueue.uiWriteOffset];

                  /* Add descriptor to packetizer */
                  BMUXlib_File_ASF_P_Packetizer_AddDescriptor(
                           hASFMux->hPacketizer,
                           &stPacketizerDescriptor,
                           &stCallbackInfo
                           );
               }
               else
               {
                  /* 0 length packets can just be passed through, so we instead, just set the bReleased to true for the descriptor */
                  pCurrentInput->stReleaseQueue.bReleased[pCurrentInput->stReleaseQueue.uiWriteOffset] = true;
               }

               pCurrentInput->stReleaseQueue.uiWriteOffset++;
               pCurrentInput->stReleaseQueue.uiWriteOffset %= BMUXLIB_FILE_ASF_P_MAX_INPUT_DESCRIPTORS;

               /* Determine next state */
               if ( true == BMUXLIB_INPUT_DESCRIPTOR_IS_EOS( &stInputDescriptor ) )
               {
                  /* set this input so it is ignored by input selection process */
                  BMUXlib_InputGroup_ActivateInput(hASFMux->hInputGroup, pCurrentInput->hInput, false);
                  BDBG_MODULE_MSG( BMUXLIB_FILE_ASF_STATE, ("eProcessInput --> eSelectInput (Stream: %d EOS)", pCurrentInput->uiStreamNumber) );
                  hASFMux->status.eProcessInputState = BMUXlib_File_ASF_P_ProcessInputState_eSelectInput;
               }
               else
               {
                  BDBG_MODULE_MSG( BMUXLIB_FILE_ASF_STATE, ("eProcessInput --> eProcessNextDescriptor") );
                  hASFMux->status.eProcessInputState = BMUXlib_File_ASF_P_ProcessInputState_eProcessNextDescriptor;
               }
            }
            else
            {
               hASFMux->status.eBlockedState = BMUXlib_File_ASF_P_Blocked_State_eOutput;
            }
            break;

         case BMUXlib_File_ASF_P_ProcessInputState_eProcessNextDescriptor: /* We need to wait for the next descriptor and if it is a frame start, go back to selecting the next input */
            if ( true == BMUXlib_Input_IsDescriptorAvailable( hASFMux->status.pCurrentInput->hInput ) )
            {
               bool bDescAvail;
               BMUXlib_Input_Descriptor stInputDescriptor;
               bDescAvail = BMUXlib_Input_PeekAtNextDescriptor( hASFMux->status.pCurrentInput->hInput, &stInputDescriptor );
               /* since we checked for availability, we expect this to always succeed */
               BDBG_ASSERT(bDescAvail == true);

               if ( true == BMUXLIB_INPUT_DESCRIPTOR_IS_FRAMESTART( &stInputDescriptor ) )
               {
                  /* The next descriptor is a frame start, so go back to selecting a new input */
                  BDBG_MODULE_MSG( BMUXLIB_FILE_ASF_STATE, ("eProcessNextDescriptor --> eSelectInput (New Frame)") );
                  hASFMux->status.eProcessInputState = BMUXlib_File_ASF_P_ProcessInputState_eSelectInput;
               }
               else
               {
                  /* The next input is part of the current frame, so process the current input */
                  BDBG_MODULE_MSG( BMUXLIB_FILE_ASF_STATE, ("eProcessNextDescriptor --> eProcessInput (Same Frame)") );
                  hASFMux->status.eProcessInputState = BMUXlib_File_ASF_P_ProcessInputState_eProcessInput;
               }
            }
            else
            {
               /* Stay in the current state */
               hASFMux->status.eBlockedState = BMUXlib_File_ASF_P_Blocked_State_eInput;
            }

            break;

      }
   } while ( BMUXlib_File_ASF_P_Blocked_State_eNotBlocked == hASFMux->status.eBlockedState );

   BMUXlib_File_ASF_P_Packetizer_ProcessNewDescriptors( hASFMux->hPacketizer );
   BMUXlib_File_ASF_P_Packetizer_ScheduleProcessedPackets( hASFMux->hPacketizer, hASFMux->hOutput );

   hASFMux->status.stStats.uiFileSize = BMUXlib_Output_GetCurrentOffset( hASFMux->hOutput );
   hASFMux->status.stStats.uiDataPacketsCount = BMUXlib_File_ASF_P_Packetizer_GetDataPacketCount( hASFMux->hPacketizer );

   {
      BMUXlib_InputGroup_Status stStatus;
      BMUXlib_InputGroup_GetStatus(hASFMux->hInputGroup, &stStatus);
      bResult = ( 0 != stStatus.uiNumActiveInputs );
   }
   bResult = bResult || BMUXlib_File_ASF_P_Packetizer_IsDescriptorPendingQueue( hASFMux->hPacketizer );

   BDBG_LEAVE( BMUXlib_File_ASF_P_ProcessInputDescriptors );

   return bResult;
}
