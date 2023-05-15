/***************************************************************************
 *     Copyright (c) 2003-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmuxlib_fnrt_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/17 $
 * $brcm_Date: 5/8/13 3:13p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/muxlib/src/common/bmuxlib_fnrt_priv.c $
 * 
 * Hydra_Software_Devel/17   5/8/13 3:13p nilesh
 * SW7425-4710: Linearize PTS/DTS by maintaining delta(oPTS,PTS) across
 * chunk boundaries.
 * 
 * Hydra_Software_Devel/16   5/1/13 4:30p nilesh
 * SW7425-4710: Widen criteria for detecting frame drops across chunk
 * boundaries
 * 
 * Hydra_Software_Devel/15   4/30/13 11:09a nilesh
 * SW7425-4710: Keep track of the largest PTS to account for B-frames
 * 
 * Hydra_Software_Devel/14   4/30/13 11:03a nilesh
 * SW7425-4710: Keep track of largest oPTS to account for B-frames
 * 
 * Hydra_Software_Devel/13   4/24/13 2:21p nilesh
 * SW7425-4710: Prevent linearized PTS values from going backwards.
 * 
 * Hydra_Software_Devel/12   4/11/13 2:58p nilesh
 * SW7425-4710: Updated PTS/DTS linearize logic to account for dropped
 * frames
 * 
 * Hydra_Software_Devel/11   4/11/13 11:42a nilesh
 * SW7425-4355: Removed debug code
 * 
 * Hydra_Software_Devel/10   4/11/13 11:41a nilesh
 * SW7425-4355: Fixed compile time warning for using 64-bit constants
 * 
 * Hydra_Software_Devel/9   4/10/13 4:46p nilesh
 * SW7425-4355: Fix precision error when shifting 32-bit value into 33-bit
 * range
 * 
 * Hydra_Software_Devel/8   4/10/13 4:35p nilesh
 * SW7425-4355: Limit adjusted PTS value to 33 bits
 * 
 * Hydra_Software_Devel/7   4/8/13 2:31p nilesh
 * SW7425-4355: Print more info in the ESCR reset debug message
 * 
 * Hydra_Software_Devel/6   3/5/13 10:36a nilesh
 * SW7425-4355: Changed "ESCR Reset" BDBG_ERR to BDBG_MSG
 * 
 * Hydra_Software_Devel/5   2/28/13 4:21p nilesh
 * SW7425-4355: Linearize ESCR based on ESCR(n-1), bitrate(n-1), length(n-
 * 1). Added ESCR reset provision if ESCR(n) is less than ETS(n).
 * 
 * Hydra_Software_Devel/4   10/17/12 5:22p nilesh
 * SW7435-304: Framework for discontinuity support
 * 
 * Hydra_Software_Devel/3   9/19/12 4:43p delkert
 * SW7425-3370: Fix problem with handling of very last EOS descriptor
 *
 * Hydra_Software_Devel/2   8/14/12 1:18p nilesh
 * SW7425-3362: Fix coverity error
 *
 * Hydra_Software_Devel/1   8/13/12 12:27p nilesh
 * SW7425-3362: Merged FNRT support
 *
 * Hydra_Software_Devel/SW7425-3362/21   8/7/12 3:48p delkert
 * SW7425-3370:Add indication of master group to config file dump. Add
 * config file flush at each chunk.
 *
 * Hydra_Software_Devel/SW7425-3362/20   8/3/12 12:48p nilesh
 * SW7425-3362: Fixed EOS handling
 *
 * Hydra_Software_Devel/SW7425-3362/19   8/2/12 2:30p delkert
 * SW7425-3362: Encase input descriptor processing state machine in while
 * loop to ensure processing continues after state change.
 * (fixes a discrepancy in descriptor count needed for testing)
 *
 * Hydra_Software_Devel/SW7425-3362/18   7/27/12 10:18p nilesh
 * SW7425-3362: Drop initial frames from slave until original PTS is valid
 *
 * Hydra_Software_Devel/SW7425-3362/17   7/27/12 7:52a nilesh
 * SW7425-3362: Only calculate lipsync adjustment when originalPTS is
 * valid
 *
 * Hydra_Software_Devel/SW7425-3362/16   7/27/12 7:29a nilesh
 * SW7425-3362: Fixed slave lipsync adjustment
 *
 * Hydra_Software_Devel/SW7425-3362/15   7/26/12 5:57p delkert
 * SW7425-3370: Add buffer status config file updates for audio
 *
 * Hydra_Software_Devel/SW7425-3362/14   7/26/12 5:39p delkert
 * SW7425-3370: Add group ID to config file. Add status updates (for base
 * addresses) to config file. Add back-link from group to main context.
 *
 * Hydra_Software_Devel/SW7425-3362/13   7/26/12 3:25p nilesh
 * SW7425-3362: Initial A/V lipsync support
 *
 * Hydra_Software_Devel/SW7425-3362/12   7/26/12 11:29a nilesh
 * SW7425-3362: Initial audio support. No lipsync or discontunity support.
 *
 * Hydra_Software_Devel/SW7425-3362/11   7/25/12 4:11p delkert
 * SW7425-3370: Move descriptor dump to before offset update so dump shows
 * original and modified offsets
 *
 * Hydra_Software_Devel/SW7425-3362/10   7/24/12 2:26p nilesh
 * SW7425-3357: Removed used of
 * BAVC_CompressedBufferDescriptor.pBaseBufferAddress
 *
 * Hydra_Software_Devel/SW7425-3362/9   7/19/12 11:33a delkert
 * SW7425-3370: Move descriptor dump so it accurately reflects base
 * address
 *
 * Hydra_Software_Devel/SW7425-3362/8   7/17/12 5:54p nilesh
 * SW7425-3362: Commented out verbose debug messages
 *
 * Hydra_Software_Devel/SW7425-3362/7   7/13/12 11:27a hongtaoz
 * SW7425-3362: added bookkeeping of input index tag for each FNRT
 * descriptors; fixed chunk DTS offset in 90Khz instead of 27Mhz; free
 * consumed descriptors for all inputs instead of early break of for loop
 * to avoid hang;
 *
 * Hydra_Software_Devel/SW7425-3362/6   7/6/12 9:14a nilesh
 * SW7425-3362: Changes debug WRN to MSG
 *
 * Hydra_Software_Devel/SW7425-3362/5   7/5/12 5:46p nilesh
 * SW7425-3362: Added basic timning adjustment logic.  Various bug fixes.
 *
 * Hydra_Software_Devel/SW7425-3362/4   7/5/12 3:16p nilesh
 * SW7425-3362: Added chunk id to debug log
 *
 * Hydra_Software_Devel/SW7425-3362/3   7/5/12 1:18p nilesh
 * SW7425-3362: Fixed compilation error
 *
 * Hydra_Software_Devel/SW7425-3362/2   7/5/12 12:59p nilesh
 * SW7425-3362: Added BMUXLIB_FNRT_DUMP_DESC support
 *
 * Hydra_Software_Devel/SW7425-3362/1   7/3/12 5:26p nilesh
 * SW7425-3362: Initial EncoderGroup implementation
 *
 ***************************************************************************/

#include "bstd.h" /* also includes berr, bdbg, etc */
#include "bkni.h"

#include "bmuxlib_fnrt.h"
#include "bmuxlib_fnrt_priv.h"

BDBG_MODULE(BMUXLIB_FNRT_PRIV);

extern const char* BMUXlib_FNRT_P_InputType_FriendlyNameLUT[BMUXlib_FNRT_P_InputType_eMax];

BERR_Code
BMUXlib_FNRT_P_EncoderGroup_GetBufferDescriptorsForInput(
   BMUXlib_FNRT_P_EncoderGroup_Handle hMuxFNRTEncoderGroup,
   unsigned uiInputIndex
   )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER( BMUXlib_FNRT_P_EncoderGroup_GetBufferDescriptorsForInput );

   /* Get the buffer descriptors for this input */
   switch ( hMuxFNRTEncoderGroup->eType )
   {
      case BMUXlib_FNRT_P_InputType_eVideo:
      {
         BMUXlib_VideoEncoderInterface *pInterface = &hMuxFNRTEncoderGroup->input[uiInputIndex].interface.stVideoInterface;

         rc = BERR_TRACE( pInterface->fGetBufferDescriptors(
            pInterface->pContext,
            &hMuxFNRTEncoderGroup->chunk.descriptors.astVideo[0],
            &hMuxFNRTEncoderGroup->chunk.auiNumDescriptors[0],
            &hMuxFNRTEncoderGroup->chunk.descriptors.astVideo[1],
            &hMuxFNRTEncoderGroup->chunk.auiNumDescriptors[1]
         ));

         if ( ( BERR_SUCCESS == rc )
              && ( false == hMuxFNRTEncoderGroup->chunk.bBufferStatusValid ) )
         {
            BAVC_VideoBufferStatus stVideoBufferStatus;

            rc = BERR_TRACE( pInterface->fGetBufferStatus(
               pInterface->pContext,
               &stVideoBufferStatus
               ));

            hMuxFNRTEncoderGroup->chunk.stBufferStatus = stVideoBufferStatus.stCommon;

            if ( ( BERR_SUCCESS == rc )
                 && ( false == hMuxFNRTEncoderGroup->output.bBufferStatusValid ) )
            {
               rc = BMUXlib_FNRT_P_EncoderGroup_GetVideoBufferStatus(
                  hMuxFNRTEncoderGroup,
                  &stVideoBufferStatus
                  );

            }
         }
      }
         break;

      case BMUXlib_FNRT_P_InputType_eAudio:
      {
         BMUXlib_AudioEncoderInterface *pInterface = &hMuxFNRTEncoderGroup->input[uiInputIndex].interface.stAudioInterface;

         rc = BERR_TRACE( pInterface->fGetBufferDescriptors(
            pInterface->pContext,
            &hMuxFNRTEncoderGroup->chunk.descriptors.astAudio[0],
            &hMuxFNRTEncoderGroup->chunk.auiNumDescriptors[0],
            &hMuxFNRTEncoderGroup->chunk.descriptors.astAudio[1],
            &hMuxFNRTEncoderGroup->chunk.auiNumDescriptors[1]
         ));

         if ( ( BERR_SUCCESS == rc )
              && ( false == hMuxFNRTEncoderGroup->chunk.bBufferStatusValid ) )
         {
            BAVC_AudioBufferStatus stAudioBufferStatus;

            rc = BERR_TRACE( pInterface->fGetBufferStatus(
               pInterface->pContext,
               &stAudioBufferStatus
               ));

            hMuxFNRTEncoderGroup->chunk.stBufferStatus = stAudioBufferStatus.stCommon;

            if ( ( BERR_SUCCESS == rc )
                 && ( false == hMuxFNRTEncoderGroup->output.bBufferStatusValid ) )
            {
               rc = BMUXlib_FNRT_P_EncoderGroup_GetAudioBufferStatus(
                  hMuxFNRTEncoderGroup,
                  &stAudioBufferStatus
                  );
            }
         }
      }
         break;

      default:
         BDBG_ERR(("Unrecognized input type...disabling input"));
         hMuxFNRTEncoderGroup->input[uiInputIndex].bEnable = false;
         hMuxFNRTEncoderGroup->uiNumActiveInputs--;
         break;
   }

   BDBG_LEAVE( BMUXlib_FNRT_P_EncoderGroup_GetBufferDescriptorsForInput );

   return BERR_TRACE(rc);
}

#if BMUXLIB_FNRT_P_DUMP_DESC
void
BMUXlib_FNRT_P_EncoderGroup_DumpDescriptor(
   FILE *hFile,
   unsigned uiCount,
   unsigned uiChunkId,
   void *pDescriptor,
   int iInputIndex,
   BMUXlib_FNRT_P_InputType eType
   )
{
   BAVC_CompressedBufferDescriptor *pstCommon = (BAVC_CompressedBufferDescriptor *) pDescriptor;
   BAVC_VideoBufferDescriptor *pstVideo = (BAVC_VideoBufferDescriptor *) pDescriptor;
   BAVC_AudioBufferDescriptor *pstAudio = (BAVC_AudioBufferDescriptor *) pDescriptor;

   /* Common */
   fprintf( hFile, "%d,%d,%d,0x%08x,%u,%u,%u,%llu,%u,%u,%d",
      uiCount,
      uiChunkId,
      iInputIndex,
      pstCommon->uiFlags,
      pstCommon->uiOffset,
      pstCommon->uiLength,
      pstCommon->uiOriginalPTS,
      pstCommon->uiPTS,
      pstCommon->uiESCR,
      pstCommon->uiTicksPerBit,
      pstCommon->iSHR
      );

   /* Type Specific */
   switch ( eType )
   {
      case BMUXlib_FNRT_P_InputType_eVideo:
         fprintf( hFile, ",0x%08x,%llu,%u,%d",
            pstVideo->uiVideoFlags,
            pstVideo->uiDTS,
            pstVideo->uiDataUnitType,
            (pstVideo->uiVideoFlags & BAVC_VIDEOBUFFERDESCRIPTOR_FLAGS_RAP) ? 1 : 0
            );
         break;
      case BMUXlib_FNRT_P_InputType_eAudio:
         fprintf( hFile, ",%u,%u,%u",
            pstAudio->uiRawDataOffset,
            pstAudio->uiRawDataLength,
            pstAudio->uiDataUnitType
            );
         break;
      default:
         break;
   }
}
#endif

void
BMUXlib_FNRT_P_EncoderGroup_LinearizeESCR(
   BMUXlib_FNRT_P_EncoderGroup_Handle hMuxFNRTEncoderGroup,
   BAVC_CompressedBufferDescriptor *pstCommon
   )
{
   /* Bookkeeping for ESCR calculations */
   if ( 0 != ( pstCommon->uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_ESCR_VALID ) )
   {
      if ( true == hMuxFNRTEncoderGroup->bEtsDtsOffsetValid )
      {
         /* ETS DTS Offset is valid, so we can linearize the ESCR */

         if ( false == hMuxFNRTEncoderGroup->linearizeESCR.bValid )
         {
            /* This is ESCR(0), so use ESCR as-is */
         }
         else
         {
            /* Calculate ESCR(n) using ESCR(n-1), Bitrate(n-1), and Length(n-1) */
            uint64_t uiDeltaESCR = hMuxFNRTEncoderGroup->linearizeESCR.uiPreviousLength * 8;
            uiDeltaESCR *= hMuxFNRTEncoderGroup->linearizeESCR.uiPreviousTicksPerBit;

            if ( hMuxFNRTEncoderGroup->linearizeESCR.iPreviousSHR > 0 )
            {
               uiDeltaESCR >>= hMuxFNRTEncoderGroup->linearizeESCR.iPreviousSHR;
            }
            else
            {
               uiDeltaESCR <<= -hMuxFNRTEncoderGroup->linearizeESCR.iPreviousSHR;
            }

            pstCommon->uiESCR = hMuxFNRTEncoderGroup->linearizeESCR.uiPreviousESCR + uiDeltaESCR;

            /* SW7425-4355: Check to make sure ESCR meets lower bound */
            if ( ( true == hMuxFNRTEncoderGroup->bEtsDtsOffsetValid )
                 && ( true == hMuxFNRTEncoderGroup->bPreviousDTSValid ) )
            {
               unsigned uiESCRLowerBound = ((uint64_t)(hMuxFNRTEncoderGroup->uiPreviousDTS*300 - hMuxFNRTEncoderGroup->uiEtsDtsOffset)) & 0xFFFFFFFF;
#if 0
               BKNI_Printf("ESCR= %08x,DTS=%09llx,EtsDtsOffset=%08x,ESCRlower=%08x\n",
                  pstCommon->uiESCR,
                  hMuxFNRTEncoderGroup->uiPreviousDTS,
                  hMuxFNRTEncoderGroup->uiEtsDtsOffset,
                  uiESCRLowerBound
               );
#endif
               if ( ( ( uiESCRLowerBound > pstCommon->uiESCR ) && ( ( uiESCRLowerBound - pstCommon->uiESCR ) < 0x80000000 ) )
                    || ( ( uiESCRLowerBound < pstCommon->uiESCR ) && ( ( pstCommon->uiESCR - uiESCRLowerBound ) > 0x80000000 ) ) )
               {
                  /* Do ESCR Reset */
                  BDBG_MSG(("[%2d][%2d][%4d] ESCR Reset (%08x --> %08x)", hMuxFNRTEncoderGroup->eType, hMuxFNRTEncoderGroup->iActiveInputIndex, hMuxFNRTEncoderGroup->uiChunkId, pstCommon->uiESCR, uiESCRLowerBound));
                  pstCommon->uiESCR = uiESCRLowerBound;
               }
            }
         }
      }
      else
      {
         /* ETS DTS offset is NOT valid, so default to the old ESCR adjustment logic */
         pstCommon->uiESCR += hMuxFNRTEncoderGroup->chunk.uiTimingOffsetIn90Khz * 300;
      }

      /* If the ESCR is valid, we are at the beginning of a frame, so we reset the stats */
      hMuxFNRTEncoderGroup->linearizeESCR.bValid = true;
      hMuxFNRTEncoderGroup->linearizeESCR.uiPreviousESCR = pstCommon->uiESCR;
      hMuxFNRTEncoderGroup->linearizeESCR.uiPreviousTicksPerBit = pstCommon->uiTicksPerBit;
      hMuxFNRTEncoderGroup->linearizeESCR.iPreviousSHR = pstCommon->iSHR;
      hMuxFNRTEncoderGroup->linearizeESCR.uiPreviousLength = pstCommon->uiLength;
   }
   else if ( 0 == ( pstCommon->uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_METADATA ) )
   {
      /* If the ESCR is not valid and this is not metadata, we are in the middle of a frame, so accumulate the length */
      hMuxFNRTEncoderGroup->linearizeESCR.uiPreviousLength += pstCommon->uiLength;
   }
}

BERR_Code
BMUXlib_FNRT_P_EncoderGroup_GetNewDescriptors(
   BMUXlib_FNRT_P_EncoderGroup_Handle hMuxFNRTEncoderGroup
   )
{
   BERR_Code rc = BERR_SUCCESS;
   bool bDone = false;
   BDBG_ENTER( BMUXlib_FNRT_P_EncoderGroup_GetNewDescriptors );

   while (!bDone && rc == BERR_SUCCESS)
   {
      switch ( hMuxFNRTEncoderGroup->eInputState )
      {
         case BMUXlib_FNRT_P_InputState_eSelectInput:
            /* Select next input */
         {
            unsigned i;

            /* If a master encoder group is set, wait until the master info is valid before
             * selecting an input in this encoder group.
             */
            if ( ( NULL != hMuxFNRTEncoderGroup->hMasterEncoderGroup )
                 && ( false == hMuxFNRTEncoderGroup->hMasterEncoderGroup->master.bValid ) )
            {
               BDBG_WRN(("Waiting for master to be validated"));
               bDone = true;
               break;
            }

            /* Cycle through all enabled inputs */
            for ( i = 0; ( i < BMUXLIB_FNRT_P_ENCODER_GROUP_MAX_INPUTS ) && ( -1 == hMuxFNRTEncoderGroup->iActiveInputIndex ); i++ )
            {
               const BAVC_CompressedBufferDescriptor *pstCommon = NULL;

               if ( false == hMuxFNRTEncoderGroup->input[i].bEnable ) continue;

               rc = BERR_TRACE( BMUXlib_FNRT_P_EncoderGroup_GetBufferDescriptorsForInput(
                  hMuxFNRTEncoderGroup,
                  i
               ));

               if ( BERR_SUCCESS == rc )
               {
                  /* Get the buffer descriptors for this input */
                  switch ( hMuxFNRTEncoderGroup->eType )
                  {
                     case BMUXlib_FNRT_P_InputType_eVideo:
                     {
                        if ( hMuxFNRTEncoderGroup->input[i].uiPendingDescriptors < hMuxFNRTEncoderGroup->chunk.auiNumDescriptors[0] )
                        {
                           pstCommon = &hMuxFNRTEncoderGroup->chunk.descriptors.astVideo[0][hMuxFNRTEncoderGroup->input[i].uiPendingDescriptors].stCommon;
                        }
                        else
                        {
                           pstCommon = &hMuxFNRTEncoderGroup->chunk.descriptors.astVideo[1][hMuxFNRTEncoderGroup->input[i].uiPendingDescriptors - hMuxFNRTEncoderGroup->chunk.auiNumDescriptors[0]].stCommon;
                        }
                     }
                        break;

                     case BMUXlib_FNRT_P_InputType_eAudio:
                     {
                        if ( hMuxFNRTEncoderGroup->input[i].uiPendingDescriptors < hMuxFNRTEncoderGroup->chunk.auiNumDescriptors[0] )
                        {
                           pstCommon = &hMuxFNRTEncoderGroup->chunk.descriptors.astAudio[0][hMuxFNRTEncoderGroup->input[i].uiPendingDescriptors].stCommon;
                        }
                        else
                        {
                           pstCommon = &hMuxFNRTEncoderGroup->chunk.descriptors.astAudio[1][hMuxFNRTEncoderGroup->input[i].uiPendingDescriptors - hMuxFNRTEncoderGroup->chunk.auiNumDescriptors[0]].stCommon;
                        }
                     }
                        break;

                     default:
                        BDBG_ERR(("Unrecognized input type...disabling input"));
                        hMuxFNRTEncoderGroup->input[i].bEnable = false;
                        hMuxFNRTEncoderGroup->uiNumActiveInputs--;
                        break;
                  }

                  if ( NULL != pstCommon )
                  {
                     /* Verify that the first descriptor is a metadata descriptor */
                     if ( 0 != ( pstCommon->uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_METADATA ) )
                     {
                        unsigned uiChunkId = ~hMuxFNRTEncoderGroup->uiChunkId;
                        const void *pBufferBaseAddress = hMuxFNRTEncoderGroup->chunk.stBufferStatus.pMetadataBufferBaseAddress;

                        BDBG_ASSERT( pBufferBaseAddress );

                        /* Get the input's chunk ID */
                        switch ( hMuxFNRTEncoderGroup->eType )
                        {
                           case BMUXlib_FNRT_P_InputType_eVideo:
                           {
                              switch ( ( ( BAVC_VideoBufferDescriptor *) pstCommon )->uiDataUnitType )
                              {
                                 case BAVC_VideoMetadataType_eCommon:
                                 {
                                    BAVC_VideoMetadataDescriptor *pstVideoMetadata = (BAVC_VideoMetadataDescriptor *) ((unsigned) pBufferBaseAddress + pstCommon->uiOffset);
                                    uiChunkId = pstVideoMetadata->stTiming.uiChunkId;
                                    hMuxFNRTEncoderGroup->uiEtsDtsOffset = pstVideoMetadata->stTiming.uiEtsDtsOffset;
                                    hMuxFNRTEncoderGroup->bEtsDtsOffsetValid = (0 != ( pstVideoMetadata->uiMetadataFlags & BAVC_VIDEOMETADATADESCRIPTOR_FLAGS_TIMING_ETS_DTS_OFFSET_VALID));
                                 }
                                 break;

                                 default:
                                    BDBG_ERR(("Unrecognized video metadata type...disabling input!"));
                                    hMuxFNRTEncoderGroup->input[i].bEnable = false;
                                    hMuxFNRTEncoderGroup->uiNumActiveInputs--;
                              }
                           }
                           break;

                           case BMUXlib_FNRT_P_InputType_eAudio:
                           {
                              switch ( ( ( BAVC_AudioBufferDescriptor *) pstCommon )->uiDataUnitType )
                              {
                                 case BAVC_AudioMetadataType_eCommon:
                                 {
                                    BAVC_AudioMetadataDescriptor *pstAudioMetadata = (BAVC_AudioMetadataDescriptor *) ((unsigned) pBufferBaseAddress + pstCommon->uiOffset);
                                    uiChunkId = pstAudioMetadata->stTiming.uiChunkId;
                                 }
                                    break;

                                 default:
                                    BDBG_ERR(("Unrecognized Audio metadata type...disabling input!"));
                                    hMuxFNRTEncoderGroup->input[i].bEnable = false;
                                    hMuxFNRTEncoderGroup->uiNumActiveInputs--;
                              }
                           }
                           break;

                           default:
                              BDBG_ERR(("Unrecognized input type...disabling input!"));
                              hMuxFNRTEncoderGroup->input[i].bEnable = false;
                              hMuxFNRTEncoderGroup->uiNumActiveInputs--;
                              break;
                        }
                        BDBG_MSG(("[%2d][%2d][%4d] Target Chunk Id=%u", hMuxFNRTEncoderGroup->eType, i, uiChunkId, hMuxFNRTEncoderGroup->uiChunkId));
                        /* See if the input's chunk ID matches the next chunk ID we need */
                        if ( uiChunkId == hMuxFNRTEncoderGroup->uiChunkId )
                        {
                           hMuxFNRTEncoderGroup->iActiveInputIndex = i;
                           hMuxFNRTEncoderGroup->chunk.bBufferStatusValid = true;
                           hMuxFNRTEncoderGroup->eInputState = BMUXlib_FNRT_P_InputState_eProcessInput;
                           BDBG_MSG(("[%2d][%2d][%4d] Selected Input", hMuxFNRTEncoderGroup->eType, hMuxFNRTEncoderGroup->iActiveInputIndex, hMuxFNRTEncoderGroup->uiChunkId));
#if BMUXLIB_FNRT_P_TEST_MODE
                           /* write the buffer status update to the config file */
                           if (NULL != hMuxFNRTEncoderGroup->hMain->hConfigFile)
                           {
                              fprintf(hMuxFNRTEncoderGroup->hMain->hConfigFile, "%d,%d,buf_status,%s,%d,%p,%p\n",
                                 *hMuxFNRTEncoderGroup->puiDescCount,
                                 0, /* Group ID */
                                 BMUXlib_FNRT_P_InputType_FriendlyNameLUT[hMuxFNRTEncoderGroup->eType],
                                 i,
                                 hMuxFNRTEncoderGroup->chunk.stBufferStatus.pFrameBufferBaseAddress,
                                 hMuxFNRTEncoderGroup->chunk.stBufferStatus.pMetadataBufferBaseAddress
                                 );
                              fflush(hMuxFNRTEncoderGroup->hMain->hConfigFile);
                           }
#endif
                        }
                     }
                     else
                     {
                        BDBG_ERR(("First descriptor of input %d is not a metadata descriptor...disabling input!", i));
                        hMuxFNRTEncoderGroup->input[i].bEnable = false;
                        hMuxFNRTEncoderGroup->uiNumActiveInputs--;
                     }
                  }
               }
            } /* end: for each enabled input */
            /* if state did not change, then no input found, so we're done until next iteration */
            bDone = (hMuxFNRTEncoderGroup->eInputState == BMUXlib_FNRT_P_InputState_eSelectInput);
         }
         break;

         case BMUXlib_FNRT_P_InputState_eProcessInput:
         {
            /* Get *NEW* Buffers From Active Input */
            rc = BERR_TRACE( BMUXlib_FNRT_P_EncoderGroup_GetBufferDescriptorsForInput(
               hMuxFNRTEncoderGroup,
               hMuxFNRTEncoderGroup->iActiveInputIndex
            ));

            if ( BERR_SUCCESS == rc )
            {
               unsigned i;

               /* Skip over pending descriptors */
               for ( i = hMuxFNRTEncoderGroup->input[hMuxFNRTEncoderGroup->iActiveInputIndex].uiPendingDescriptors; ( i < (hMuxFNRTEncoderGroup->chunk.auiNumDescriptors[0] + hMuxFNRTEncoderGroup->chunk.auiNumDescriptors[1]) ) && ( BMUXlib_FNRT_P_InputState_eProcessInput == hMuxFNRTEncoderGroup->eInputState ); i++ )
               {
                  unsigned uiTempWriteOffset = ( hMuxFNRTEncoderGroup->serialized.uiWriteOffset + 1 ) % BMUXLIB_FNRT_P_ENCODER_GROUP_MAX_DESCRIPTORS;
                  if ( uiTempWriteOffset == hMuxFNRTEncoderGroup->serialized.uiReadOffset )
                  {
#if 0
                     BDBG_MSG(("Output Queue is Full"));
#endif
                     bDone = true;
                     break; /* Output queue is full */
                  }

                  /* Copy new entries to output queue */
                  if ( i < hMuxFNRTEncoderGroup->chunk.auiNumDescriptors[0] )
                  {
                     switch ( hMuxFNRTEncoderGroup->eType )
                     {
                        case BMUXlib_FNRT_P_InputType_eVideo:
                           hMuxFNRTEncoderGroup->serialized.descriptors.astVideoDescriptor[hMuxFNRTEncoderGroup->serialized.uiWriteOffset] = hMuxFNRTEncoderGroup->chunk.descriptors.astVideo[0][i];
                           break;
                        case BMUXlib_FNRT_P_InputType_eAudio:
                           hMuxFNRTEncoderGroup->serialized.descriptors.astAudioDescriptor[hMuxFNRTEncoderGroup->serialized.uiWriteOffset] = hMuxFNRTEncoderGroup->chunk.descriptors.astAudio[0][i];
                           break;
                        default:
                           break;
                     }
                  }
                  else
                  {
                     switch ( hMuxFNRTEncoderGroup->eType )
                     {
                        case BMUXlib_FNRT_P_InputType_eVideo:
                           hMuxFNRTEncoderGroup->serialized.descriptors.astVideoDescriptor[hMuxFNRTEncoderGroup->serialized.uiWriteOffset] = hMuxFNRTEncoderGroup->chunk.descriptors.astVideo[1][i - hMuxFNRTEncoderGroup->chunk.auiNumDescriptors[0]];
                           break;
                        case BMUXlib_FNRT_P_InputType_eAudio:
                           hMuxFNRTEncoderGroup->serialized.descriptors.astAudioDescriptor[hMuxFNRTEncoderGroup->serialized.uiWriteOffset] = hMuxFNRTEncoderGroup->chunk.descriptors.astAudio[1][i - hMuxFNRTEncoderGroup->chunk.auiNumDescriptors[0]];
                           break;
                        default:
                           break;
                     }
                  }

                  /* Adjust output descriptors values */
                  {
                     BAVC_CompressedBufferDescriptor *pstCommon = NULL;

                     switch ( hMuxFNRTEncoderGroup->eType )
                     {
                        case BMUXlib_FNRT_P_InputType_eVideo:
                           pstCommon = &hMuxFNRTEncoderGroup->serialized.descriptors.astVideoDescriptor[hMuxFNRTEncoderGroup->serialized.uiWriteOffset].stCommon;
                           break;
                        case BMUXlib_FNRT_P_InputType_eAudio:
                           pstCommon = &hMuxFNRTEncoderGroup->serialized.descriptors.astAudioDescriptor[hMuxFNRTEncoderGroup->serialized.uiWriteOffset].stCommon;
                           break;
                        default:
                           break;
                     }

                     if ( NULL != pstCommon )
                     {
                        /* If we are a master encoder group, validate the master information */
                        if ( ( NULL == hMuxFNRTEncoderGroup->hMasterEncoderGroup ) /* We are a master */
                             && ( false == hMuxFNRTEncoderGroup->master.bValid ) /* We haven't validated our master info, yet */
                             && ( 0 == ( pstCommon->uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_METADATA ) ) /* The current descriptor is not metadata */
                             && ( 0 != ( pstCommon->uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_ORIGINALPTS_VALID ) ) /* Original PTS is valid */
                             && ( 0 != ( pstCommon->uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_PTS_VALID ) ) /* PTS is valid */
                             && ( 0 == ( pstCommon->uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EMPTY_FRAME ) ) /* Not an empty frame */
                             )
                        {
                           switch ( hMuxFNRTEncoderGroup->eType )
                           {
                              case BMUXlib_FNRT_P_InputType_eVideo:
                                 if ( 0 != ( hMuxFNRTEncoderGroup->serialized.descriptors.astVideoDescriptor[hMuxFNRTEncoderGroup->serialized.uiWriteOffset].uiVideoFlags & BAVC_VIDEOBUFFERDESCRIPTOR_FLAGS_RAP ) ) /* RAP point */
                                 {
                                    hMuxFNRTEncoderGroup->master.bValid = true;
                                 }
                                 break;

                              case BMUXlib_FNRT_P_InputType_eAudio:
                                 hMuxFNRTEncoderGroup->master.bValid = true;
                                 break;

                              default:
                                 break;
                           }

                           if ( true == hMuxFNRTEncoderGroup->master.bValid )
                           {
                              hMuxFNRTEncoderGroup->master.uiTargetDeltaNewVsOriginalPts = pstCommon->uiPTS - ( ((uint64_t)pstCommon->uiOriginalPTS) << 1 );
                              BDBG_MSG(("Target Delta New/Original PTS: %lld", hMuxFNRTEncoderGroup->master.uiTargetDeltaNewVsOriginalPts));
                              hMuxFNRTEncoderGroup->master.bValid = true;
                           }
                        }

                        /* Translate buffer offset values */
                        BDBG_ASSERT( true == hMuxFNRTEncoderGroup->output.bBufferStatusValid );
                        BDBG_CASSERT( sizeof( void* ) == sizeof( unsigned ) );

#if BMUXLIB_FNRT_P_DUMP_DESC
                        BMUXlib_FNRT_P_EncoderGroup_DumpDescriptor(
                           hMuxFNRTEncoderGroup->hDescDumpFile,
                           *hMuxFNRTEncoderGroup->puiDescCount,
                           hMuxFNRTEncoderGroup->uiChunkId,
                           pstCommon,
                           hMuxFNRTEncoderGroup->iActiveInputIndex,
                           hMuxFNRTEncoderGroup->eType
                           );
                        fprintf( hMuxFNRTEncoderGroup->hDescDumpFile, ",IN");
                        fprintf( hMuxFNRTEncoderGroup->hDescDumpFile, "\n" );
#endif

                        if ( 0 == ( pstCommon->uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_METADATA ) )
                        {
                           pstCommon->uiOffset += (unsigned) hMuxFNRTEncoderGroup->chunk.stBufferStatus.pFrameBufferBaseAddress - (unsigned) hMuxFNRTEncoderGroup->output.uBufferStatus.stCommon.pFrameBufferBaseAddress;
                        }
                        else
                        {
                           pstCommon->uiOffset += (unsigned) hMuxFNRTEncoderGroup->chunk.stBufferStatus.pMetadataBufferBaseAddress - (unsigned) hMuxFNRTEncoderGroup->output.uBufferStatus.stCommon.pMetadataBufferBaseAddress;
                        }

                        /* Reset Drop Until RAP */
                        if ( true == hMuxFNRTEncoderGroup->bDropUntilRAP )
                        {
                           if ( 0 != ( pstCommon->uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_METADATA ) )
                           {
                              hMuxFNRTEncoderGroup->bDropUntilRAP = false;
                           }
                           else if ( BMUXlib_FNRT_P_InputType_eVideo == hMuxFNRTEncoderGroup->eType )
                           {
                              if ( 0 != ( hMuxFNRTEncoderGroup->serialized.descriptors.astVideoDescriptor[hMuxFNRTEncoderGroup->serialized.uiWriteOffset].uiVideoFlags & BAVC_VIDEOBUFFERDESCRIPTOR_FLAGS_RAP ) )
                              {
                                 hMuxFNRTEncoderGroup->bDropUntilRAP = false;
                              }
                           }
                           else
                           {
                              hMuxFNRTEncoderGroup->bDropUntilRAP = false;
                           }
                        }

                        if ( ( 0 == ( pstCommon->uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_METADATA ) )
                             && ( 0 != ( pstCommon->uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_PTS_VALID ) )
                             && ( 0 == ( pstCommon->uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EMPTY_FRAME ) ) ) /* Not an empty frame */
                        {
                           /* Calculate current DTS */
                           uint64_t uiCurrentDTS;
                           uint64_t uiPTSMask = (((uint64_t)0x1) << 32) | (0xFFFFFFFF);

                           if ( BMUXlib_FNRT_P_InputType_eVideo == hMuxFNRTEncoderGroup->eType )
                           {
                              uiCurrentDTS = hMuxFNRTEncoderGroup->serialized.descriptors.astVideoDescriptor[hMuxFNRTEncoderGroup->serialized.uiWriteOffset].uiDTS;
                           }
                           else
                           {
                              uiCurrentDTS = pstCommon->uiPTS ;
                           }

                           /* Calculate deltaDTS */
                           if ( true == hMuxFNRTEncoderGroup->chunk.bPreviousOriginalDTSValid )
                           {
                              /* Only update deltaDTS if a wrap didn't occur */
                              if ( uiCurrentDTS > hMuxFNRTEncoderGroup->chunk.uiPreviousOriginalDTS )
                              {
                                 unsigned uiNewDeltaDTS = uiCurrentDTS - hMuxFNRTEncoderGroup->chunk.uiPreviousOriginalDTS;

                                 /* SW7425-4710: If a frame drop occurs in the 2nd to last picture, then the uiDeltaDTS is
                                  * larger than the nominal. So, we only allow a larger deltaDTS within a small range to
                                  * account for PTS jitter */
                                 if ( true == hMuxFNRTEncoderGroup->bDeltaDTSValid )
                                 {
                                    if ( ( uiNewDeltaDTS > hMuxFNRTEncoderGroup->uiDeltaDTS )
                                         && ( ( uiNewDeltaDTS - hMuxFNRTEncoderGroup->uiDeltaDTS ) > 2 ) )
                                    {
                                       BDBG_MSG(("[%2d][%2d][%4d] New Delta DTS > Old Delta DTS (%d > %d). Using previous deltaDTS",
                                          hMuxFNRTEncoderGroup->eType, hMuxFNRTEncoderGroup->iActiveInputIndex, hMuxFNRTEncoderGroup->uiChunkId,
                                          uiNewDeltaDTS,
                                          hMuxFNRTEncoderGroup->uiDeltaDTS
                                          ));

                                       uiNewDeltaDTS = hMuxFNRTEncoderGroup->uiDeltaDTS;
                                    }
                                 }

                                 hMuxFNRTEncoderGroup->uiDeltaDTS = uiNewDeltaDTS;
                                 hMuxFNRTEncoderGroup->bDeltaDTSValid = true;
                              }
                           }
#if 0
                           BDBG_MSG(("++++chunkID=%u W[%u] curDts=%llu preDts=%llu timeOffset=%llu dDts=%u", hMuxFNRTEncoderGroup->uiChunkId,
                              hMuxFNRTEncoderGroup->serialized.uiWriteOffset, uiCurrentDTS, hMuxFNRTEncoderGroup->uiPreviousDTS,
                              hMuxFNRTEncoderGroup->chunk.uiTimingOffsetIn27Mhz, hMuxFNRTEncoderGroup->uiDeltaDTS));
#endif
                           /* Save current DTS */
                           hMuxFNRTEncoderGroup->chunk.uiPreviousOriginalDTS = uiCurrentDTS;
                           hMuxFNRTEncoderGroup->chunk.bPreviousOriginalDTSValid = true;

                           /* Calculate timing adjustment */
                           if ( ( false == hMuxFNRTEncoderGroup->chunk.bTimingOffsetValid )
                                && ( 0 != ( pstCommon->uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_ORIGINALPTS_VALID ) ) )
                           {
                              if ( 0 == hMuxFNRTEncoderGroup->uiChunkId )
                              {
                                 hMuxFNRTEncoderGroup->chunk.bTimingOffsetValid = true;
                                 hMuxFNRTEncoderGroup->chunk.uiTimingOffsetIn90Khz = 0;
                              }
                              else
                              {
                                 /* Calculate timing offset for this chunk */
                                 uint64_t uiCurrentDeltaNewVsOriginalPTS;
                                 BMUXlib_FNRT_P_EncoderGroup_Handle hMasterEncoderGroup = ( NULL != hMuxFNRTEncoderGroup->hMasterEncoderGroup ) ? hMuxFNRTEncoderGroup->hMasterEncoderGroup : hMuxFNRTEncoderGroup;

                                 uiCurrentDeltaNewVsOriginalPTS = pstCommon->uiPTS - ( ((uint64_t)pstCommon->uiOriginalPTS) << 1 );
                                 hMuxFNRTEncoderGroup->chunk.uiTimingOffsetIn90Khz = ( hMasterEncoderGroup->master.uiTargetDeltaNewVsOriginalPts - uiCurrentDeltaNewVsOriginalPTS );

                                 BDBG_MSG(("Timing Offset = %llu (PTS=%llu - oPTS=%llu = %lld) target=%lld",
                                    hMuxFNRTEncoderGroup->chunk.uiTimingOffsetIn90Khz,
                                    pstCommon->uiPTS,
                                    ((uint64_t)pstCommon->uiOriginalPTS)<<1,
                                    uiCurrentDeltaNewVsOriginalPTS,
                                    hMasterEncoderGroup->master.uiTargetDeltaNewVsOriginalPts
                                    ));

                                 /* Adjust the the timing offset to make sure it results in a deltaPTS that is a multiple of deltaDTS */
                                 {
                                    int32_t iDeltaPTS = ( (pstCommon->uiPTS + hMuxFNRTEncoderGroup->chunk.uiTimingOffsetIn90Khz) & uiPTSMask) - hMuxFNRTEncoderGroup->uiPreviousPTS;
                                    int32_t iFRCCorrection;
                                    if ( iDeltaPTS < 0 )
                                    {
                                       iFRCCorrection = -iDeltaPTS + hMuxFNRTEncoderGroup->uiDeltaDTS;
                                    }
                                    else if ( (uint32_t) iDeltaPTS > hMuxFNRTEncoderGroup->uiDeltaDTS )
                                    {
                                       iFRCCorrection = -(iDeltaPTS % hMuxFNRTEncoderGroup->uiDeltaDTS);
                                    }
                                    else
                                    {
                                       iFRCCorrection = hMuxFNRTEncoderGroup->uiDeltaDTS - iDeltaPTS;
                                    }

                                    BDBG_MSG(("Applying FRC Correction of %d (PTS=%llu + offset=%llu - pPTS=%llu = delta=%d) (deltaDTS = %u)",
                                       iFRCCorrection,
                                       pstCommon->uiPTS,
                                       hMuxFNRTEncoderGroup->chunk.uiTimingOffsetIn90Khz,
                                       hMuxFNRTEncoderGroup->uiPreviousPTS,
                                       iDeltaPTS,
                                       hMuxFNRTEncoderGroup->uiDeltaDTS
                                       ));
                                    hMuxFNRTEncoderGroup->chunk.uiTimingOffsetIn90Khz += iFRCCorrection;
                                 }

                                 hMuxFNRTEncoderGroup->chunk.bTimingOffsetValid = true;
                              }

                              /* If not master, calculate the lipsync adjustment */
                              if ( NULL != hMuxFNRTEncoderGroup->hMasterEncoderGroup )
                              {
                                 uint64_t uiCurrentDeltaNewVsOriginalPTS;

                                 BDBG_ASSERT( true == hMuxFNRTEncoderGroup->hMasterEncoderGroup->master.bValid );
                                 uiCurrentDeltaNewVsOriginalPTS = ( uiCurrentDTS + hMuxFNRTEncoderGroup->chunk.uiTimingOffsetIn90Khz ) - ( ((uint64_t)pstCommon->uiOriginalPTS) << 1 );
                                 hMuxFNRTEncoderGroup->chunk.uiTimingOffsetIn90Khz += ( hMuxFNRTEncoderGroup->hMasterEncoderGroup->master.uiTargetDeltaNewVsOriginalPts - uiCurrentDeltaNewVsOriginalPTS );
                              }

                              BDBG_MSG(("[%2d][%2d][%4d] Timing Adjustment for (%u --> %llu + %lld = %llu)",
                                 hMuxFNRTEncoderGroup->eType, hMuxFNRTEncoderGroup->iActiveInputIndex, hMuxFNRTEncoderGroup->uiChunkId,
                                 pstCommon->uiOriginalPTS,
                                 pstCommon->uiPTS,
                                 hMuxFNRTEncoderGroup->chunk.uiTimingOffsetIn90Khz,
                                 (pstCommon->uiPTS + hMuxFNRTEncoderGroup->chunk.uiTimingOffsetIn90Khz) & uiPTSMask));
                           }

                           if ( true == hMuxFNRTEncoderGroup->chunk.bTimingOffsetValid )
                           {
                              pstCommon->uiPTS += hMuxFNRTEncoderGroup->chunk.uiTimingOffsetIn90Khz;
                              pstCommon->uiPTS &= uiPTSMask;

                              /* SW7425-4710: Keep track of largest oPTS to account for B-frames */
                              if ( ( false == hMuxFNRTEncoderGroup->bPreviousOPTSValid )
                                   || ( ( (int32_t) ( pstCommon->uiOriginalPTS - hMuxFNRTEncoderGroup->uiPreviousOPTS ) ) > 0 ) )
                              {
                                 hMuxFNRTEncoderGroup->uiPreviousOPTS = pstCommon->uiOriginalPTS;
                              }

                              /* SW7425-4710: Keep track of the largest PTS to account for B-frames */
                              if ( ( false == hMuxFNRTEncoderGroup->bPreviousPTSValid )
                                   || ( ( (int32_t) ( ( (uint32_t) (pstCommon->uiPTS >> 1) ) - ( (uint32_t) (hMuxFNRTEncoderGroup->uiPreviousPTS >> 1) ) ) ) > 0 ) )
                              {
                                 hMuxFNRTEncoderGroup->uiPreviousPTS = pstCommon->uiPTS;
                              }

                              if ( BMUXlib_FNRT_P_InputType_eVideo == hMuxFNRTEncoderGroup->eType )
                              {
                                 hMuxFNRTEncoderGroup->serialized.descriptors.astVideoDescriptor[hMuxFNRTEncoderGroup->serialized.uiWriteOffset].uiDTS += hMuxFNRTEncoderGroup->chunk.uiTimingOffsetIn90Khz;
                                 hMuxFNRTEncoderGroup->serialized.descriptors.astVideoDescriptor[hMuxFNRTEncoderGroup->serialized.uiWriteOffset].uiDTS &= uiPTSMask;

                                 /* SW7425-4710: Fix negative DTS values: This is a work around for incorrect DTS behavior when the VCE FW drops frames */
                                 if ( ( true == hMuxFNRTEncoderGroup->bPreviousDTSValid )
                                      && ( ( hMuxFNRTEncoderGroup->serialized.descriptors.astVideoDescriptor[hMuxFNRTEncoderGroup->serialized.uiWriteOffset].uiDTS < hMuxFNRTEncoderGroup->uiPreviousDTS )
                                         || ( ( hMuxFNRTEncoderGroup->serialized.descriptors.astVideoDescriptor[hMuxFNRTEncoderGroup->serialized.uiWriteOffset].uiDTS - hMuxFNRTEncoderGroup->uiPreviousDTS ) < hMuxFNRTEncoderGroup->uiDeltaDTS ) ) )
                                 {
                                    hMuxFNRTEncoderGroup->serialized.descriptors.astVideoDescriptor[hMuxFNRTEncoderGroup->serialized.uiWriteOffset].uiDTS = hMuxFNRTEncoderGroup->uiPreviousDTS + hMuxFNRTEncoderGroup->uiDeltaDTS;
                                    hMuxFNRTEncoderGroup->serialized.descriptors.astVideoDescriptor[hMuxFNRTEncoderGroup->serialized.uiWriteOffset].uiDTS &= uiPTSMask;
                                 }

                                 hMuxFNRTEncoderGroup->uiPreviousDTS = hMuxFNRTEncoderGroup->serialized.descriptors.astVideoDescriptor[hMuxFNRTEncoderGroup->serialized.uiWriteOffset].uiDTS;
                              }
                              else
                              {
                                 hMuxFNRTEncoderGroup->uiPreviousDTS = pstCommon->uiPTS;
                              }

                              hMuxFNRTEncoderGroup->bPreviousPTSValid = true;
                              hMuxFNRTEncoderGroup->bPreviousDTSValid = true;
                              hMuxFNRTEncoderGroup->bPreviousOPTSValid = true;
                           }
                           else if ( NULL != hMuxFNRTEncoderGroup->hMasterEncoderGroup ) /* timing offset is not valid and we are slave, so we need to drop this frame */
                           {
                              BDBG_MSG(("[%2d][%2d][%4d] Frame Drop", hMuxFNRTEncoderGroup->eType, hMuxFNRTEncoderGroup->iActiveInputIndex, hMuxFNRTEncoderGroup->uiChunkId));
                              hMuxFNRTEncoderGroup->bDropUntilRAP = true;
                           }
                        }

                        /* SW7425-4355: Linearize ESCR using ESCR(0), deltaESCR, and ETS */
                        BMUXlib_FNRT_P_EncoderGroup_LinearizeESCR(
                           hMuxFNRTEncoderGroup,
                           pstCommon
                           );

                        /* Add End Of Chunk (EOC) processing */
                        if ( 0 != ( pstCommon->uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EOC ) )
                        {
#if 0 /* Do we want/need to do this? */
                           /* Filter EOC flag */
                           pstCommon->uiFlags &= ~BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EOC;
#endif
                           BDBG_MSG(("[%2d][%2d][%4d] EOC Seen", hMuxFNRTEncoderGroup->eType, hMuxFNRTEncoderGroup->iActiveInputIndex, hMuxFNRTEncoderGroup->uiChunkId));
                           hMuxFNRTEncoderGroup->chunk.bEOCSeen = true;/* TODO: it's possible to terminate chunk when EOC is seen instead of waiting for next chunk */
                        }

                        /* Add End of Stream (EOS) processing */
                        if ( 0 != ( pstCommon->uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EOS ) )
                        {
                           /* Disable this input */
                           hMuxFNRTEncoderGroup->input[hMuxFNRTEncoderGroup->iActiveInputIndex].bEnable = false;
                           hMuxFNRTEncoderGroup->uiNumActiveInputs--;

                           BDBG_MSG(("[%2d][%2d][%4d] EOS Seen (Active: %u)",
                              hMuxFNRTEncoderGroup->eType, hMuxFNRTEncoderGroup->iActiveInputIndex, hMuxFNRTEncoderGroup->uiChunkId,
                              hMuxFNRTEncoderGroup->uiNumActiveInputs
                              ));

                           if ( 0 == hMuxFNRTEncoderGroup->uiNumActiveInputs )
                           {
                              BDBG_MSG(("[%2d][--][%4d] EOS Seen on ALL inputs...Done!", hMuxFNRTEncoderGroup->eType, hMuxFNRTEncoderGroup->uiChunkId));
                              hMuxFNRTEncoderGroup->eInputState = BMUXlib_FNRT_P_InputState_eDone;
                           }
                           else
                           {
                              /* Drop this EOS descriptor since it is not the last chunk */
                              hMuxFNRTEncoderGroup->bDropUntilRAP = true;
                           }
                        }

                        if ( ( ( true == hMuxFNRTEncoderGroup->chunk.bEOCSeen )
                              && ( 0 != ( pstCommon->uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_METADATA ) ) )
                             || ( 0 != ( pstCommon->uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EOS ) ) )
                        {
                           /* We've seen an EOC and see the next metadata entry OR seen an EOS, so we know the current chunk is done */
                           BDBG_MSG(("[%2d][%2d][%4d] Chunk Done", hMuxFNRTEncoderGroup->eType, hMuxFNRTEncoderGroup->iActiveInputIndex, hMuxFNRTEncoderGroup->uiChunkId));
                           BKNI_Memset( &hMuxFNRTEncoderGroup->chunk, 0, sizeof( hMuxFNRTEncoderGroup->chunk ) );
                           if ( hMuxFNRTEncoderGroup->eInputState != BMUXlib_FNRT_P_InputState_eDone )
                           {
                              hMuxFNRTEncoderGroup->iActiveInputIndex = -1;
                              hMuxFNRTEncoderGroup->uiChunkId++;
                              hMuxFNRTEncoderGroup->eInputState = BMUXlib_FNRT_P_InputState_eSelectInput;
                           }
                        }

#if BMUXLIB_FNRT_P_DUMP_DESC
                        BMUXlib_FNRT_P_EncoderGroup_DumpDescriptor(
                           hMuxFNRTEncoderGroup->hDescDumpFile,
                           *hMuxFNRTEncoderGroup->puiDescCount,
                           hMuxFNRTEncoderGroup->uiChunkId,
                           pstCommon,
                           hMuxFNRTEncoderGroup->iActiveInputIndex,
                           hMuxFNRTEncoderGroup->eType
                           );
#endif

                        if ( BMUXlib_FNRT_P_InputState_eSelectInput != hMuxFNRTEncoderGroup->eInputState )
                        {
                           /* bookkeeping the processed descriptors count, and associated input index tag for later recycle use. */
                           hMuxFNRTEncoderGroup->serialized.stMetadata[hMuxFNRTEncoderGroup->serialized.uiWriteOffset].iInputIndex = hMuxFNRTEncoderGroup->iActiveInputIndex;
                           hMuxFNRTEncoderGroup->serialized.stMetadata[hMuxFNRTEncoderGroup->serialized.uiWriteOffset].uiNumDescriptors++;
                           hMuxFNRTEncoderGroup->input[hMuxFNRTEncoderGroup->iActiveInputIndex].uiPendingDescriptors++;

                           /* Filter metadata entries for chunks 1..n */
                           if ( ( ( 0 != ( pstCommon->uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_METADATA ) )
                                  && ( 0 == hMuxFNRTEncoderGroup->uiChunkId ) )
                                || ( ( 0 == ( pstCommon->uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_METADATA ) )
                                     && ( false == hMuxFNRTEncoderGroup->bDropUntilRAP ) ) )
                           {
                              /* Commit this descriptor to the output. This controls the descriptors count seen at output; but actual input descriptors count is kept above */
                              hMuxFNRTEncoderGroup->serialized.uiWriteOffset = uiTempWriteOffset;
#if BMUXLIB_FNRT_P_DUMP_DESC
                              fprintf( hMuxFNRTEncoderGroup->hDescDumpFile, ",OUT");
#endif
                           }
                           else
                           {
#if BMUXLIB_FNRT_P_DUMP_DESC
                              fprintf( hMuxFNRTEncoderGroup->hDescDumpFile, ",DROPPED");
#endif
                           }
                        }
                        else
                        {
#if BMUXLIB_FNRT_P_DUMP_DESC
                           fprintf( hMuxFNRTEncoderGroup->hDescDumpFile, ",NEXT");
#endif
                        }

#if BMUXLIB_FNRT_P_DUMP_DESC
                        fprintf( hMuxFNRTEncoderGroup->hDescDumpFile, "\n" );
                        fflush( hMuxFNRTEncoderGroup->hDescDumpFile );
                        (*hMuxFNRTEncoderGroup->puiDescCount)++;
#endif
                     }
                  }
               } /* end: for each descriptor */
               /* if done already set, or if state did not change (i.e. more descriptors to process next
                  iteration) then we're done until next iteration */
               bDone = bDone || (BMUXlib_FNRT_P_InputState_eProcessInput == hMuxFNRTEncoderGroup->eInputState);
            }
         }
         break;

         case BMUXlib_FNRT_P_InputState_eDone:
            bDone = true;
            break;
      }
   } /* end: while not done */

   BDBG_LEAVE( BMUXlib_FNRT_P_EncoderGroup_GetNewDescriptors );
   return BERR_TRACE( rc );
}

BERR_Code
BMUXlib_FNRT_P_UpdateDescriptors(
   BMUXlib_FNRT_Handle hMuxFNRT
   )
{
   BERR_Code rc = BERR_SUCCESS;
   BMUXlib_FNRT_P_EncoderGroup_Handle hMuxFNRTEncoderGroup = NULL;
   unsigned i;

   /* Get new input descriptors from ALL input groups */
   for ( i = 0; i < BMUXlib_FNRT_P_InputType_eMax; i++ )
   {
      hMuxFNRTEncoderGroup = &hMuxFNRT->astEncoderGroup[i];

      rc = BMUXlib_FNRT_P_EncoderGroup_GetNewDescriptors( hMuxFNRTEncoderGroup );
      if ( BERR_SUCCESS != rc ) return BERR_TRACE( rc );
   }

   /* Perform Sync */
   /* TODO: Add discontinity handling.  For now just copy the serialized descriptors to the output list */
   for ( i = 0; i < BMUXlib_FNRT_P_InputType_eMax; i++ )
   {
      hMuxFNRTEncoderGroup = &hMuxFNRT->astEncoderGroup[i];

      while ( hMuxFNRTEncoderGroup->serialized.uiQueuedOffset != hMuxFNRTEncoderGroup->serialized.uiWriteOffset )
      {
         switch ( hMuxFNRTEncoderGroup->eType )
         {
            case BMUXlib_FNRT_P_InputType_eVideo:
               hMuxFNRTEncoderGroup->output.descriptors.astVideoDescriptor[hMuxFNRTEncoderGroup->output.uiWriteOffset] = hMuxFNRTEncoderGroup->serialized.descriptors.astVideoDescriptor[hMuxFNRTEncoderGroup->serialized.uiQueuedOffset];
               break;
            case BMUXlib_FNRT_P_InputType_eAudio:
               hMuxFNRTEncoderGroup->output.descriptors.astAudioDescriptor[hMuxFNRTEncoderGroup->output.uiWriteOffset] = hMuxFNRTEncoderGroup->serialized.descriptors.astAudioDescriptor[hMuxFNRTEncoderGroup->serialized.uiQueuedOffset];
               break;
            default:
               break;
         }

         hMuxFNRTEncoderGroup->output.stMetadata[hMuxFNRTEncoderGroup->output.uiWriteOffset].uiNumDescriptors++;
         hMuxFNRTEncoderGroup->output.uiWriteOffset++;
         hMuxFNRTEncoderGroup->output.uiWriteOffset %= BMUXLIB_FNRT_P_ENCODER_GROUP_MAX_DESCRIPTORS;

         hMuxFNRTEncoderGroup->serialized.uiQueuedOffset++;
         hMuxFNRTEncoderGroup->serialized.uiQueuedOffset %= BMUXLIB_FNRT_P_ENCODER_GROUP_MAX_DESCRIPTORS;
      }
   }

   return BERR_TRACE( rc );
}

BERR_Code
BMUXlib_FNRT_P_EncoderGroup_GetVideoBufferDescriptors(
   BMUXlib_FNRT_P_EncoderGroup_Handle hMuxFNRTEncoderGroup,
   const BAVC_VideoBufferDescriptor **astDescriptors0,
   size_t *puiNumDescriptors0,
   const BAVC_VideoBufferDescriptor **astDescriptors1,
   size_t *puiNumDescriptors1
   )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER( BMUXlib_FNRT_P_EncoderGroup_GetVideoBufferDescriptors );
   if ( ( NULL == astDescriptors0 )
        || ( NULL == puiNumDescriptors0 )
        || ( NULL == astDescriptors1 )
        || ( NULL == puiNumDescriptors1 ) )
   {
      rc = BERR_TRACE( BERR_INVALID_PARAMETER );
   }

   if ( BERR_SUCCESS == rc )
   {
      *astDescriptors0 = NULL;
      *puiNumDescriptors0 = 0;
      *astDescriptors1 = NULL;
      *puiNumDescriptors1 = 0;


      rc = BMUXlib_FNRT_P_UpdateDescriptors( hMuxFNRTEncoderGroup->hMain );

      if ( BERR_SUCCESS == rc )
      {
         /* Assign astDescriptorsX and puiNumDescriptorsX */
         if ( hMuxFNRTEncoderGroup->output.uiWriteOffset >= hMuxFNRTEncoderGroup->output.uiReadOffset )
         {
            *astDescriptors0 = &hMuxFNRTEncoderGroup->output.descriptors.astVideoDescriptor[hMuxFNRTEncoderGroup->output.uiReadOffset];
            *puiNumDescriptors0 = hMuxFNRTEncoderGroup->output.uiWriteOffset - hMuxFNRTEncoderGroup->output.uiReadOffset;

            *astDescriptors1 = NULL;
            *puiNumDescriptors1 = 0;
         }
         else
         {
            *astDescriptors0 = &hMuxFNRTEncoderGroup->output.descriptors.astVideoDescriptor[hMuxFNRTEncoderGroup->output.uiReadOffset];
            *puiNumDescriptors0 = BMUXLIB_FNRT_P_ENCODER_GROUP_MAX_DESCRIPTORS - hMuxFNRTEncoderGroup->output.uiReadOffset;

            *astDescriptors1 = &hMuxFNRTEncoderGroup->output.descriptors.astVideoDescriptor[0];
            *puiNumDescriptors1 = hMuxFNRTEncoderGroup->output.uiWriteOffset;
         }
      }
   }

   BDBG_LEAVE( BMUXlib_FNRT_P_EncoderGroup_GetVideoBufferDescriptors );

   return BERR_TRACE( rc );
}

BERR_Code
BMUXlib_FNRT_P_EncoderGroup_GetVideoBufferStatus(
   BMUXlib_FNRT_P_EncoderGroup_Handle hMuxFNRTEncoderGroup,
   BAVC_VideoBufferStatus *pBufferStatus
   )
{
   BERR_Code rc = BERR_UNKNOWN;

   BDBG_ENTER( BMUXlib_FNRT_P_EncoderGroup_GetVideoBufferStatus );

   BDBG_ASSERT( hMuxFNRTEncoderGroup );
   if ( NULL == hMuxFNRTEncoderGroup )
   {
      BDBG_ERR(("FNRT Encoder Group handle is NULL!"));
      BDBG_LEAVE( BMUXlib_FNRT_P_EncoderGroup_GetVideoBufferStatus );
      return BERR_TRACE( BERR_INVALID_PARAMETER );
   }

   if ( NULL == pBufferStatus)
   {
      BDBG_ERR(("Buffer status pointer is NULL!"));
      BDBG_LEAVE( BMUXlib_FNRT_P_EncoderGroup_GetVideoBufferStatus );
      return BERR_TRACE( BERR_INVALID_PARAMETER );
   }

   BKNI_Memset( pBufferStatus, 0, sizeof( BAVC_VideoBufferStatus ) );

   if  ( false == hMuxFNRTEncoderGroup->output.bBufferStatusValid )
   {
      unsigned uiInput;

      for ( uiInput = 0; ( uiInput < BMUXLIB_FNRT_P_ENCODER_GROUP_MAX_INPUTS ); uiInput++ )
      {
         if ( false == hMuxFNRTEncoderGroup->input[uiInput].bEnable ) continue;

         rc = hMuxFNRTEncoderGroup->input[uiInput].interface.stVideoInterface.fGetBufferStatus(
            hMuxFNRTEncoderGroup->input[uiInput].interface.stVideoInterface.pContext,
            pBufferStatus
         );
      }

      if ( BERR_SUCCESS == rc )
      {
         hMuxFNRTEncoderGroup->output.uBufferStatus.stVideo = *pBufferStatus;
         hMuxFNRTEncoderGroup->output.bBufferStatusValid = true;
#if BMUXLIB_FNRT_P_TEST_MODE
         /* write the buffer status update to the config file */
         if (NULL != hMuxFNRTEncoderGroup->hMain->hConfigFile)
         {
            fprintf(hMuxFNRTEncoderGroup->hMain->hConfigFile, "%d,%d,buf_status,video,-1,%p,%p\n",
               *hMuxFNRTEncoderGroup->puiDescCount,
               0, /* Group ID */
               pBufferStatus->stCommon.pFrameBufferBaseAddress,
               pBufferStatus->stCommon.pMetadataBufferBaseAddress
               );
   }
#endif
      }
   }
   else
   {
      rc = BERR_SUCCESS;
      *pBufferStatus = hMuxFNRTEncoderGroup->output.uBufferStatus.stVideo;
   }

   BDBG_LEAVE( BMUXlib_FNRT_P_EncoderGroup_GetVideoBufferStatus );

   return BERR_TRACE( rc );
}

BERR_Code
BMUXlib_FNRT_P_EncoderGroup_GetAudioBufferDescriptors(
   BMUXlib_FNRT_P_EncoderGroup_Handle hMuxFNRTEncoderGroup,
   const BAVC_AudioBufferDescriptor **astDescriptors0,
   size_t *puiNumDescriptors0,
   const BAVC_AudioBufferDescriptor **astDescriptors1,
   size_t *puiNumDescriptors1
   )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER( BMUXlib_FNRT_P_EncoderGroup_GetAudioBufferDescriptors );
   if ( ( NULL == astDescriptors0 )
        || ( NULL == puiNumDescriptors0 )
        || ( NULL == astDescriptors1 )
        || ( NULL == puiNumDescriptors1 ) )
   {
      rc = BERR_TRACE( BERR_INVALID_PARAMETER );
   }

   if ( BERR_SUCCESS == rc )
   {
      *astDescriptors0 = NULL;
      *puiNumDescriptors0 = 0;
      *astDescriptors1 = NULL;
      *puiNumDescriptors1 = 0;

      rc = BMUXlib_FNRT_P_UpdateDescriptors( hMuxFNRTEncoderGroup->hMain );

      if ( BERR_SUCCESS == rc )
      {
         /* Assign astDescriptorsX and puiNumDescriptorsX */
         if ( hMuxFNRTEncoderGroup->output.uiWriteOffset >= hMuxFNRTEncoderGroup->output.uiReadOffset )
         {
            *astDescriptors0 = &hMuxFNRTEncoderGroup->output.descriptors.astAudioDescriptor[hMuxFNRTEncoderGroup->output.uiReadOffset];
            *puiNumDescriptors0 = hMuxFNRTEncoderGroup->output.uiWriteOffset - hMuxFNRTEncoderGroup->output.uiReadOffset;

            *astDescriptors1 = NULL;
            *puiNumDescriptors1 = 0;
         }
         else
         {
            *astDescriptors0 = &hMuxFNRTEncoderGroup->output.descriptors.astAudioDescriptor[hMuxFNRTEncoderGroup->output.uiReadOffset];
            *puiNumDescriptors0 = BMUXLIB_FNRT_P_ENCODER_GROUP_MAX_DESCRIPTORS - hMuxFNRTEncoderGroup->output.uiReadOffset;

            *astDescriptors1 = &hMuxFNRTEncoderGroup->output.descriptors.astAudioDescriptor[0];
            *puiNumDescriptors1 = hMuxFNRTEncoderGroup->output.uiWriteOffset;
         }
      }
   }

   BDBG_LEAVE( BMUXlib_FNRT_P_EncoderGroup_GetAudioBufferDescriptors );

   return BERR_TRACE( rc );
}

BERR_Code
BMUXlib_FNRT_P_EncoderGroup_GetAudioBufferStatus(
   BMUXlib_FNRT_P_EncoderGroup_Handle hMuxFNRTEncoderGroup,
   BAVC_AudioBufferStatus *pBufferStatus
   )
{
   BERR_Code rc = BERR_UNKNOWN;

   BDBG_ENTER( BMUXlib_FNRT_P_EncoderGroup_GetAudioBufferStatus );

   BDBG_ASSERT( hMuxFNRTEncoderGroup );
   if ( NULL == hMuxFNRTEncoderGroup )
   {
      BDBG_ERR(("FNRT Encoder Group handle is NULL!"));
      BDBG_LEAVE( BMUXlib_FNRT_P_EncoderGroup_GetAudioBufferStatus );
      return BERR_TRACE( BERR_INVALID_PARAMETER );
   }

   if ( NULL == pBufferStatus)
   {
      BDBG_ERR(("Buffer status pointer is NULL!"));
      BDBG_LEAVE( BMUXlib_FNRT_P_EncoderGroup_GetAudioBufferStatus );
      return BERR_TRACE( BERR_INVALID_PARAMETER );
   }

   BKNI_Memset( pBufferStatus, 0, sizeof( BAVC_AudioBufferStatus ) );

   if  ( false == hMuxFNRTEncoderGroup->output.bBufferStatusValid )
   {
      unsigned uiInput;

      for ( uiInput = 0; ( uiInput < BMUXLIB_FNRT_P_ENCODER_GROUP_MAX_INPUTS ); uiInput++ )
      {
         if ( false == hMuxFNRTEncoderGroup->input[uiInput].bEnable ) continue;

         rc = hMuxFNRTEncoderGroup->input[uiInput].interface.stAudioInterface.fGetBufferStatus(
            hMuxFNRTEncoderGroup->input[uiInput].interface.stAudioInterface.pContext,
            pBufferStatus
         );
         break;
      }

      if ( BERR_SUCCESS == rc )
      {
         hMuxFNRTEncoderGroup->output.uBufferStatus.stAudio = *pBufferStatus;
         hMuxFNRTEncoderGroup->output.bBufferStatusValid = true;
#if BMUXLIB_FNRT_P_TEST_MODE
         /* write the buffer status update to the config file */
         if (NULL != hMuxFNRTEncoderGroup->hMain->hConfigFile)
         {
            fprintf(hMuxFNRTEncoderGroup->hMain->hConfigFile, "%d,%d,buf_status,audio,-1,%p,%p\n",
               *hMuxFNRTEncoderGroup->puiDescCount,
               0, /* Group ID */
               pBufferStatus->stCommon.pFrameBufferBaseAddress,
               pBufferStatus->stCommon.pMetadataBufferBaseAddress
               );
   }
#endif
      }
   }
   else
   {
      rc = BERR_SUCCESS;
      *pBufferStatus = hMuxFNRTEncoderGroup->output.uBufferStatus.stAudio;
   }

   BDBG_LEAVE( BMUXlib_FNRT_P_EncoderGroup_GetAudioBufferStatus );

   return BERR_TRACE( rc );
}

BERR_Code
BMUXlib_FNRT_P_EncoderGroup_ConsumeBufferDescriptors(
   BMUXlib_FNRT_P_EncoderGroup_Handle hMuxFNRTEncoderGroup,
   size_t uiNumBufferDescriptors
   )
{
   unsigned i,j;
   unsigned auiNumBufferDescriptorsConsumed[BMUXLIB_FNRT_P_ENCODER_GROUP_MAX_INPUTS];

   BDBG_ENTER( BMUXlib_FNRT_P_EncoderGroup_ConsumeBufferDescriptors );

   BDBG_ASSERT( hMuxFNRTEncoderGroup );
   if ( NULL == hMuxFNRTEncoderGroup )
   {
      BDBG_ERR(("FNRT Encoder Group handle is NULL!"));
      BDBG_LEAVE( BMUXlib_FNRT_P_EncoderGroup_ConsumeBufferDescriptors );
      return BERR_TRACE( BERR_INVALID_PARAMETER );
   }

   BKNI_Memset( auiNumBufferDescriptorsConsumed, 0, sizeof( unsigned ) * BMUXLIB_FNRT_P_ENCODER_GROUP_MAX_INPUTS );

   /* Determine which input the output descriptors came from and accumulate how many descriptors to free on each input */
   for ( i = 0; i < uiNumBufferDescriptors; i++ )
   {
      unsigned uiNumSerializedDescriptors = hMuxFNRTEncoderGroup->output.stMetadata[hMuxFNRTEncoderGroup->output.uiReadOffset].uiNumDescriptors;

      for ( j = 0; j < uiNumSerializedDescriptors; j++ )
      {
         unsigned uiNumInputDescriptors = hMuxFNRTEncoderGroup->serialized.stMetadata[hMuxFNRTEncoderGroup->serialized.uiReadOffset].uiNumDescriptors;

         auiNumBufferDescriptorsConsumed[hMuxFNRTEncoderGroup->serialized.stMetadata[hMuxFNRTEncoderGroup->serialized.uiReadOffset].iInputIndex] += uiNumInputDescriptors;

         hMuxFNRTEncoderGroup->serialized.stMetadata[hMuxFNRTEncoderGroup->serialized.uiReadOffset].uiNumDescriptors = 0;
         hMuxFNRTEncoderGroup->serialized.uiReadOffset ++;
         hMuxFNRTEncoderGroup->serialized.uiReadOffset %= BMUXLIB_FNRT_P_ENCODER_GROUP_MAX_DESCRIPTORS;
      }

      hMuxFNRTEncoderGroup->output.stMetadata[hMuxFNRTEncoderGroup->output.uiReadOffset].uiNumDescriptors = 0;
      hMuxFNRTEncoderGroup->output.uiReadOffset ++;
      hMuxFNRTEncoderGroup->output.uiReadOffset %= BMUXLIB_FNRT_P_ENCODER_GROUP_MAX_DESCRIPTORS;
   }

   /* Free the descriptors for the input(s) */
   for ( i = 0; ( i < BMUXLIB_FNRT_P_ENCODER_GROUP_MAX_INPUTS ); i++ )
   {
      /* iterate through all inputs to recycle the input descriptors */
      if( 0 == auiNumBufferDescriptorsConsumed[i] ) continue;
      switch ( hMuxFNRTEncoderGroup->eType )
      {
         case BMUXlib_FNRT_P_InputType_eVideo:
            hMuxFNRTEncoderGroup->input[i].interface.stVideoInterface.fConsumeBufferDescriptors(
               hMuxFNRTEncoderGroup->input[i].interface.stVideoInterface.pContext,
               auiNumBufferDescriptorsConsumed[i]
            );
            break;
         case BMUXlib_FNRT_P_InputType_eAudio:
            hMuxFNRTEncoderGroup->input[i].interface.stAudioInterface.fConsumeBufferDescriptors(
               hMuxFNRTEncoderGroup->input[i].interface.stAudioInterface.pContext,
               auiNumBufferDescriptorsConsumed[i]
            );
            break;
         default:
            BDBG_ERR(("Unsupported input type"));
            BDBG_LEAVE( BMUXlib_FNRT_P_EncoderGroup_ConsumeBufferDescriptors );
            return BERR_TRACE( BERR_INVALID_PARAMETER );
      }

      hMuxFNRTEncoderGroup->input[i].uiPendingDescriptors -= auiNumBufferDescriptorsConsumed[i];
#if 0
      BDBG_MSG(("in[%u] pending=%u numDescConsumed=%u", i, hMuxFNRTEncoderGroup->input[i].uiPendingDescriptors, auiNumBufferDescriptorsConsumed[i]));
#endif
   }

   BDBG_LEAVE( BMUXlib_FNRT_P_EncoderGroup_ConsumeBufferDescriptors );

   return BERR_TRACE( BERR_SUCCESS );
}

