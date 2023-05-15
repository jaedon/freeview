/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmuxlib_input.c $
 * $brcm_Revision: Hydra_Software_Devel/33 $
 * $brcm_Date: 8/14/12 3:37p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/muxlib/noarch/bmuxlib_input.c $
 * 
 * Hydra_Software_Devel/33   8/14/12 3:37p nilesh
 * SW7425-3684: Improved accuracy of reported duration
 * 
 * Hydra_Software_Devel/32   8/14/12 2:21p nilesh
 * SW7425-3684: Pick smallest duration amongst active inputs
 * 
 * Hydra_Software_Devel/31   8/14/12 1:09p nilesh
 * SW7425-3684: Added support for reporting mux duration
 * 
 * Hydra_Software_Devel/30   8/13/12 12:19p nilesh
 * SW7425-3361: Merged FNRT support
 * 
 * Hydra_Software_Devel/SW7425-3361/5   8/13/12 12:12p nilesh
 * SW7425-3361: Rebase from mainline
 * 
 * Hydra_Software_Devel/29   8/1/12 11:45a delkert
 * SW7425-3370: fix coverity "uninitialized variable" error cause by
 * previous fix.
 *
 * Hydra_Software_Devel/28   8/1/12 11:10a delkert
 * SW7425-3370: Fix coverity forward-null error (was checking wrong
 * pointer)
 *
 * Hydra_Software_Devel/27   7/30/12 11:39a nilesh
 * SW7425-3560: Removed unnecessary assert
 *
 * Hydra_Software_Devel/26   7/30/12 11:38a nilesh
 * SW7425-3560: Fixed forward null (coverity)
 *
 * Hydra_Software_Devel/25   7/3/12 12:32p nilesh
 * SW7425-3061: Disable skew detection logic
 *
 * Hydra_Software_Devel/SW7425-3361/4   7/26/12 1:40p delkert
 * SW7425-3370: Fix up descriptor dump to write input descriptor base
 * address (common descriptor no longer contains a base address)
 *
 * Hydra_Software_Devel/SW7425-3361/3   7/24/12 2:26p nilesh
 * SW7425-3357: Removed used of
 * BAVC_CompressedBufferDescriptor.pBaseBufferAddress
 *
 * Hydra_Software_Devel/SW7425-3361/2   7/5/12 2:56p delkert
 * SW7425-3370: Add dump of new fields (base address, stc snapshot and
 * chunk id) to descriptor/metadata dump files
 *
 * Hydra_Software_Devel/SW7425-3361/1   6/28/12 2:28p nilesh
 * SW7425-3361: Use BAVC_CompressedBufferDescriptor.pBufferBaseAddress if
 * non-null
 *
 * Hydra_Software_Devel/24   5/30/12 6:00p delkert
 * SW7425-2567: Added dump of metadata descriptors to CSV file
 *
 * Hydra_Software_Devel/23   5/29/12 1:49p delkert
 * SW7425-2567: Change PeekAtnextDescriptor and GetNextDescriptor APIs to
 * return bool. Make GetNextInput a void return. Add relevant assertions
 * for lack of descriptors.
 *
 * Hydra_Software_Devel/22   5/11/12 10:20a nilesh
 * SW7435-78: Added FRAME_END support
 *
 * Hydra_Software_Devel/21   4/12/12 10:20a delkert
 * SW7425-2567: Fix usage of BDBG_DEBUG_BUILD
 *
 * Hydra_Software_Devel/20   4/5/12 9:30a delkert
 * SW7425-2567: Add common count field to descriptor dump.  Remove the
 * input handle from the dump filename in test mode.
 *
 * Hydra_Software_Devel/19   2/28/12 2:52p delkert
 * SW7425-2474: Clean up 3-second ESCR diff warning
 *
 * Hydra_Software_Devel/18   2/24/12 9:09a delkert
 * SW7425-2474: Fix cut-n-paste error.
 *
 * Hydra_Software_Devel/17   2/24/12 8:29a delkert
 * SW7425-2474: Augment ESCR diff warning to indicate Video/Audio and show
 * the delta to allow tracking of drift
 *
 * Hydra_Software_Devel/16   2/16/12 4:30p nilesh
 * SW7425-2217: Merged TS VBI Userdata insertion support
 *
 * Hydra_Software_Devel/SW7425-2217/2   2/15/12 10:37a nilesh
 * SW7425-2217: Rebase from mainline
 *
 * Hydra_Software_Devel/15   2/14/12 3:23p nilesh
 * SW7425-2078: Added RAI to video descriptor log
 *
 * Hydra_Software_Devel/14   1/31/12 1:39p delkert
 * SW7425-2271: Fix missing parameters from ESCR and DTS separation
 * warnings in descriptor selectors
 *
 * Hydra_Software_Devel/SW7425-2217/1   2/14/12 1:18p delkert
 * SW7425-2217: Fix warning (missing arguments)
 *
 * Hydra_Software_Devel/13   1/18/12 11:09a nilesh
 * SW7425-1378: Fixed indexing problem when Peek/Get called after Consume
 * but before ProcessNew
 *
 * Hydra_Software_Devel/12   1/9/12 3:45p delkert
 * SW7425-2121: change create settings so interfaces are copied, not
 * pointers. Modify create setting defaults and error checking to match.
 *
 * Hydra_Software_Devel/11   12/20/11 1:46p nilesh
 * SW7425-1869: Merged BMUXlib_InputGroup support
 *
 * Hydra_Software_Devel/SW7425-1869/4   12/14/11 4:50p delkert
 * SW7425-1494: InputGroup_Create() should check table contains valid
 * input handles
 *
 * Hydra_Software_Devel/SW7425-1869/3   12/14/11 2:54p delkert
 * SW7425-1494: Fix bug in ActivateInput() when input not found in table
 *
 * Hydra_Software_Devel/SW7425-1869/2   12/13/11 4:16p delkert
 * SW7425-1494: Add error checking for create settings. Move setting of
 * context signature so that Destroy() does not assert when called from a
 * failed Create()
 *
 * Hydra_Software_Devel/SW7425-1869/1   12/9/11 8:31a delkert
 * SW7425-1869: Fix bug in DTS_VALID macro. Add metadata pointer to
 * InputCreateSettings for use by mux. Add InputGroup API. Improve error
 * checking. Modify GetNextInput to be part of InputGroup and simplify
 * sorting method. Move comparison functions from ASF and TS to make them
 * generic.
 *
 * Hydra_Software_Devel/10   12/12/11 4:53p nilesh
 * SW7425-1019: Fixed input log bug causing entries to be skipped
 *
 * Hydra_Software_Devel/9   11/14/11 10:52a nilesh
 * SW7425-1707: Added BMUXlib_Input_Handle value to input descriptor log
 * filename
 *
 * Hydra_Software_Devel/8   11/9/11 10:46a nilesh
 * SW7425-1707: Changed uiInstance/uiOutputInstance to uiMuxId
 *
 * Hydra_Software_Devel/7   11/9/11 9:58a nilesh
 * SW7425-1707: Added uiOutputInstance to facilitate debug logging for
 * multiple transcode
 *
 * Hydra_Software_Devel/6   10/11/11 5:53p hongtaoz
 * SW7425-1281: fixed pts/opts order in dump header; added option to dump
 * frame descriptors only;
 *
 * Hydra_Software_Devel/5   10/6/11 4:58p nilesh
 * SW7425-721: Fixed seg fault in BMUXlib_Input_GetNextInput() input
 * doesn't have any descriptors
 *
 * Hydra_Software_Devel/4   10/5/11 4:44p nilesh
 * SW7425-721: Added input selection logic
 *
 * Hydra_Software_Devel/3   9/16/11 11:08a nilesh
 * SW7425-1281: Added input descriptor dump support
 *
 * Hydra_Software_Devel/2   5/16/11 3:26p nilesh
 * SW7425-447: Added eBurstMode setting.  Added frame size computation.
 *
 * Hydra_Software_Devel/1   5/11/11 11:56a nilesh
 * SW7425-447: ASF Mux
 *
 ***************************************************************************/

#include "bstd.h" /* also includes berr, bdbg, etc */
#include "bkni.h"

#include "bmuxlib_input.h"

#if BMUXLIB_INPUT_P_DUMP_DESC
#include <stdio.h>
#endif

BDBG_MODULE(BMUXLIB_INPUT);

/**************/
/* Signatures */
/**************/
#define BMUXLIB_INPUT_P_SIGNATURE_CREATESETTINGS      0x494E5000
#define BMUXLIB_INPUT_P_SIGNATURE_CONTEXT             0x494E5001

#define BMUXLIB_INPUTGROUP_P_SIGNATURE_CREATESETTINGS 0x494E5002
#define BMUXLIB_INPUTGROUP_P_SIGNATURE_CONTEXT        0x494E5003
#define BMUXLIB_INPUTGROUP_P_SIGNATURE_SETTINGS       0x494E5004

#define BMUXLIB_INPUTGROUP_P_SCALED_DTS_MIDRANGE      0x80000000
#define BMUXLIB_INPUTGROUP_P_ESCR_MIDRANGE            0x80000000

static const BMUXlib_InputGroup_Settings s_stDefaultInputGroupSettings =
{
   BMUXLIB_INPUTGROUP_P_SIGNATURE_SETTINGS,
   false,                                             /* bWaitForAllInputs */
   BMUXlib_InputGroup_DescriptorSelectLowestDTS       /* fSelector */
};

typedef struct BMUXlib_Input_P_Context
{
   uint32_t uiSignature;
   BMUXlib_Input_CreateSettings stCreateSettings;

   size_t uiReadOffset; /* Keeps track of # of input descriptors that have been fully consumed since ProcessNewDescriptors was called.
                         * uiReadOffset = 0 when ProcessNewDescriptors is called */
   size_t uiPendingOffset;  /* Keeps track of # of input descriptors that have been retrieved by the mux via GetNextDescriptor.
                             * uiReadOffset <= uiPendingOffset <= uiWriteOffset
                             */
   size_t uiWriteOffset; /* Keeps track of the # of input descriptors after calling ProcessNewDescriptors */

   size_t auiNumDescriptors[2];
   union
   {
         const BAVC_CompressedBufferDescriptor *astCommon[2];
         const BAVC_VideoBufferDescriptor *astVideo[2];
         const BAVC_AudioBufferDescriptor *astAudio[2];
   } descriptors;

   bool bBufferStatusValid;
   BAVC_CompressedBufferStatus stBufferStatus;

   size_t uiCurrentFrameSize;

   struct
   {
      BMUXlib_Input_Status stStatus;
   } stats;

#if BMUXLIB_INPUT_P_DUMP_DESC
   FILE *hDescDumpFile;
   uint32_t *puiDescCount;    /* pointer to count to be used for this input */
   uint32_t uiDescCount;      /* local desc count used if this input does not belong to a group */
#if BMUXLIB_INPUT_P_DUMP_METADATA_DESC
   FILE *hMetadataDumpFile;
#endif
#endif
} BMUXlib_Input_P_Context;

typedef struct BMUXlib_InputGroup_P_InputEntry
{
   BMUXlib_Input_Handle       hInput;           /* the handle of the input for this entry */
   BMUXlib_Input_Descriptor   stDescriptor;     /* the current descriptor available for this input */
   bool                       bDescriptorValid; /* this inputs current descriptor is valid */
   bool                       bActive;          /* this input is active (is considered during input selection) */
} BMUXlib_InputGroup_P_InputEntry;

typedef struct BMUXlib_InputGroup_P_Context
{
   uint32_t uiSignature;

   BMUXlib_InputGroup_Settings stSettings;
   BMUXlib_InputGroup_Status stStatus;

   BMUXlib_InputGroup_P_InputEntry *pInputTable;         /* table of all available inputs in the group */
   uint32_t uiNumInputs;

   BMUXlib_InputGroup_P_InputEntry **pInputSelectTable;  /* table of active input entries for input selection */
   /* NOTE: not all available inputs may be active */
#if BMUXLIB_INPUT_P_DUMP_DESC
   uint32_t uiDescCount;   /* common descriptor count for associating descriptors from different inputs */
#endif
} BMUXlib_InputGroup_P_Context;

/*****************
   Prototypes
*****************/
static void BMUXlib_Input_P_PeekAtDescriptor(BMUXlib_Input_Handle hInput, unsigned int uiDescNum, BMUXlib_Input_Descriptor *pstDescriptor);
static void BMUXlib_InputGroup_P_SortInputs(BMUXlib_InputGroup_P_InputEntry *aData[], uint32_t uiCount, BMUXlib_InputGroup_DescriptorSelector fSelect);
#if BMUXLIB_INPUT_P_DUMP_DESC
static void BMuxlib_Input_P_DumpNewDescriptors(BMUXlib_Input_Handle hInput, unsigned int uiNumOldDescriptors);
#endif

/*****************
   Input API
****************/
void
BMUXlib_Input_GetDefaultCreateSettings(
         BMUXlib_Input_CreateSettings *pstSettings
         )
{
   BDBG_ENTER( BMUXlib_Input_GetDefaultCreateSettings );

   BDBG_ASSERT( pstSettings );

   BKNI_Memset(
            pstSettings,
            0,
            sizeof ( BMUXlib_Input_CreateSettings )
            );

   pstSettings->uiSignature = BMUXLIB_INPUT_P_SIGNATURE_CREATESETTINGS;
   pstSettings->eType = BMUXlib_Input_Type_eVideo;
   pstSettings->eBurstMode = BMUXlib_Input_BurstMode_eDescriptor;

   BDBG_LEAVE( BMUXlib_Input_GetDefaultCreateSettings );
}

BERR_Code
BMUXlib_Input_Create(
         BMUXlib_Input_Handle *phInput,
         const BMUXlib_Input_CreateSettings *pstSettings
         )
{
   BMUXlib_Input_Handle hInput = NULL;

   BDBG_ENTER( BMUXlib_Input_Create );

   BDBG_ASSERT( phInput );
   BDBG_ASSERT( pstSettings );
   BDBG_ASSERT( BMUXLIB_INPUT_P_SIGNATURE_CREATESETTINGS == pstSettings->uiSignature );

   *phInput = NULL;

   /* verify the settings (as a minimum, we need a type and an interface) ...
      NOTE: it is valid for pMetadata to not be set */
   if (((pstSettings->eType != BMUXlib_Input_Type_eVideo) && (pstSettings->eType != BMUXlib_Input_Type_eAudio))
      || ((pstSettings->eBurstMode != BMUXlib_Input_BurstMode_eDescriptor) && (pstSettings->eBurstMode != BMUXlib_Input_BurstMode_eFrame))
      )
   {
      BDBG_LEAVE( BMUXlib_Input_Create );
      return BERR_TRACE( BERR_INVALID_PARAMETER );
   }
   /* verify the interface settings ... */
   switch (pstSettings->eType)
   {
      case BMUXlib_Input_Type_eVideo:
      {
         const BMUXlib_VideoEncoderInterface *pInterface = &pstSettings->interface.stVideo;
         if ((NULL == pInterface->fGetBufferDescriptors) ||
            (NULL == pInterface->fConsumeBufferDescriptors) ||
            (NULL == pInterface->fGetBufferStatus))
         {
            BDBG_LEAVE( BMUXlib_Input_Create );
            return BERR_TRACE( BERR_INVALID_PARAMETER );
         }
         break;
      }
      case BMUXlib_Input_Type_eAudio:
      {
         const BMUXlib_AudioEncoderInterface *pInterface = &pstSettings->interface.stAudio;
         if ((NULL == pInterface->fGetBufferDescriptors) ||
            (NULL == pInterface->fConsumeBufferDescriptors) ||
            (NULL == pInterface->fGetBufferStatus))
         {
            BDBG_LEAVE( BMUXlib_Input_Create );
            return BERR_TRACE( BERR_INVALID_PARAMETER );
         }
         break;
      }
      default: /* can't happen */
         break;
   }

   hInput = ( BMUXlib_Input_Handle ) BKNI_Malloc( sizeof( BMUXlib_Input_P_Context ) );

   if ( NULL == hInput )
   {
      BDBG_LEAVE( BMUXlib_Input_Create );
      return BERR_TRACE( BERR_OUT_OF_SYSTEM_MEMORY );
   }

   BKNI_Memset(
            hInput,
            0,
            sizeof( BMUXlib_Input_P_Context )
            );

   hInput->stCreateSettings = *pstSettings;

#if BMUXLIB_INPUT_P_DUMP_DESC
   {
      char fname[256];
      sprintf(fname, "BMUXlib_INPUT_DESC_");

      switch ( hInput->stCreateSettings.eType )
      {
         case BMUXlib_Input_Type_eVideo:
            sprintf(fname, "%sVIDEO_", fname);
            break;

         case BMUXlib_Input_Type_eAudio:
            sprintf(fname, "%sAUDIO_", fname);
            break;

         default:
            sprintf(fname, "%sUNKNOWN_", fname);
      }
#if BMUXLIB_INPUT_P_TEST_MODE
      /* when running in test mode, do not name file with handle to the input, since this will change from run-to-run */
      sprintf(fname, "%s%02d_%02d.csv", fname, hInput->stCreateSettings.uiTypeInstance, hInput->stCreateSettings.uiMuxId);
#else
      sprintf(fname, "%s%02d_%02d_%08x.csv", fname, hInput->stCreateSettings.uiTypeInstance, hInput->stCreateSettings.uiMuxId, (unsigned)hInput);
#endif
      BKNI_Printf("Creating Input Descriptor Dump File (%s)\n", fname);

      hInput->hDescDumpFile = fopen(fname, "wb");
      if ( NULL == hInput->hDescDumpFile )
      {
         BDBG_ERR(("Error Creating Input Descriptor Dump File (%s)", fname));
      }
      else
      {
         fprintf( hInput->hDescDumpFile, "count,base_addr,frame_size,flags,offset,length,opts_45khz,pts,escr,tpb,shr,stc_snapshot" );
         switch ( hInput->stCreateSettings.eType )
         {
            case BMUXlib_Input_Type_eVideo:
               fprintf( hInput->hDescDumpFile, ",vflags,dts,dut,rai" );
               break;
            case BMUXlib_Input_Type_eAudio:
               fprintf( hInput->hDescDumpFile, ",raw offset,raw length,dut" );
               break;
            default:
               break;
         }
         fprintf( hInput->hDescDumpFile, "\n" );
      }
      hInput->puiDescCount = &hInput->uiDescCount; /* increment local copy unless this input added to a group */
   }
#if BMUXLIB_INPUT_P_DUMP_METADATA_DESC && !BMUXLIB_INPUT_P_DUMP_FRAME_DESC
   {
      char fname[256];
      sprintf(fname, "BMUXlib_METADATA_DESC_");

      switch ( hInput->stCreateSettings.eType )
      {
         case BMUXlib_Input_Type_eVideo:
            sprintf(fname, "%sVIDEO_", fname);
            break;

         case BMUXlib_Input_Type_eAudio:
            sprintf(fname, "%sAUDIO_", fname);
            break;

         default:
            sprintf(fname, "%sUNKNOWN_", fname);
      }
#if BMUXLIB_INPUT_P_TEST_MODE
      /* when running in test mode, do not name file with handle to the input, since this will change from run-to-run */
      sprintf(fname, "%s%02d_%02d.csv", fname, hInput->stCreateSettings.uiTypeInstance, hInput->stCreateSettings.uiMuxId);
#else
      sprintf(fname, "%s%02d_%02d_%08x.csv", fname, hInput->stCreateSettings.uiTypeInstance, hInput->stCreateSettings.uiMuxId, (unsigned)hInput);
#endif
      BKNI_Printf("Creating Input Metadata Descriptor Dump File (%s)\n", fname);
      hInput->hMetadataDumpFile = fopen(fname, "wb");
      if ( NULL == hInput->hMetadataDumpFile )
      {
         BDBG_ERR(("Error Creating Input Metadata Descriptor Dump File (%s)", fname));
      }
      else
      {
         switch ( hInput->stCreateSettings.eType )
         {
            case BMUXlib_Input_Type_eVideo:
               fprintf(hInput->hMetadataDumpFile, "flags, protocol, profile, level, max_bitrate, frame_rate, coded_width, coded_height, "
                  "vc1_seqb_cbr, vc1_seqb_hrd_buffer, vc1_seqc_frmrtq_post_proc, vc1_seqc_butrtq_postproc, vc1_seqc_loop_filt, "
                  "vc1_seqc_multi_res, vc1_seqc_fastuvmc, vc1_seqc_ext_mv, vc1_seqc_dquant, vc1_seqc_vtransform, vc1_seqc_overlap, "
                  "vc1_seqc_sync_mark, vc1_seqc_range_red, vc1_seqc_maxbframes, vc1_seqc_quant, vc1_seqc_finterp, stc_snapshot, chunk_id");
               break;
            case BMUXlib_Input_Type_eAudio:
               fprintf(hInput->hMetadataDumpFile, "flags, max_bitrate, samp_freq, stc_snapshot, chunk_id, protocol, aac_audio_spec_length_bits, aac_audio_spec_data, "
                  "wma_samp_per_block, wma_enc_options, wma_super_block_align, wma_block_align, wma_num_channels");
               break;
            default:
               break;
         }
         fprintf( hInput->hMetadataDumpFile, "\n" );
      }
   }
#endif /* end: metadata dump */
#endif /* end: descriptor dump */

   hInput->uiSignature = BMUXLIB_INPUT_P_SIGNATURE_CONTEXT;
   *phInput = hInput;

   BDBG_LEAVE( BMUXlib_Input_Create );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BMUXlib_Input_GetCreateSettings(
         BMUXlib_Input_Handle hInput,
         BMUXlib_Input_CreateSettings *pstSettings
         )
{
   BDBG_ASSERT( hInput );
   BDBG_ASSERT( BMUXLIB_INPUT_P_SIGNATURE_CONTEXT == hInput->uiSignature );

   BDBG_ENTER( BMUXlib_Input_GetCreateSettings );

   *pstSettings = hInput->stCreateSettings;

   BDBG_LEAVE( BMUXlib_Input_GetCreateSettings );
   return BERR_SUCCESS;
}

BERR_Code
BMUXlib_Input_Destroy(
         BMUXlib_Input_Handle hInput
         )
{
   BDBG_ENTER( BMUXlib_Input_Destroy );

   BDBG_ASSERT( hInput );
   BDBG_ASSERT( BMUXLIB_INPUT_P_SIGNATURE_CONTEXT == hInput->uiSignature );


#if BMUXLIB_INPUT_P_DUMP_DESC
   if ( NULL != hInput->hDescDumpFile )
   {
      fclose( hInput->hDescDumpFile );
      hInput->hDescDumpFile = NULL;
   }
#if BMUXLIB_INPUT_P_DUMP_METADATA_DESC && !BMUXLIB_INPUT_P_DUMP_FRAME_DESC
   if ( NULL != hInput->hMetadataDumpFile )
   {
      fclose ( hInput->hMetadataDumpFile );
      hInput->hMetadataDumpFile = NULL;
   }
#endif
#endif

   hInput->uiSignature = 0;
   BKNI_Free( hInput );

   BDBG_LEAVE( BMUXlib_Input_Destroy );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BMUXlib_Input_ProcessNewDescriptors(
         BMUXlib_Input_Handle hInput
         )
{
   BERR_Code rc = BERR_SUCCESS;
#if BMUXLIB_INPUT_P_DUMP_DESC
   size_t uiNumOldDescriptors = hInput->uiWriteOffset - hInput->uiReadOffset;
#endif

   BDBG_ASSERT( hInput );
   BDBG_ASSERT( BMUXLIB_INPUT_P_SIGNATURE_CONTEXT == hInput->uiSignature );

   BDBG_ENTER( BMUXlib_Input_ProcessNewDescriptors );

   switch ( hInput->stCreateSettings.eType )
   {
      case BMUXlib_Input_Type_eVideo:
      {
         BMUXlib_VideoEncoderInterface *pInterface = &hInput->stCreateSettings.interface.stVideo;

         /* Get Descriptors */
         rc = BERR_TRACE( pInterface->fGetBufferDescriptors(
                  pInterface->pContext,
                  &hInput->descriptors.astVideo[0],
                  &hInput->auiNumDescriptors[0],
                  &hInput->descriptors.astVideo[1],
                  &hInput->auiNumDescriptors[1]
                                             ));
         /* Check for error */
         if ( BERR_SUCCESS != rc )
         {
            BDBG_ERR(("VIDEO: Error getting buffer descriptors"));
         }
      }
      break;

      case BMUXlib_Input_Type_eAudio:
      {
         BMUXlib_AudioEncoderInterface *pInterface = &hInput->stCreateSettings.interface.stAudio;

         /* Get Descriptors */
         rc = BERR_TRACE( pInterface->fGetBufferDescriptors(
                  pInterface->pContext,
                  &hInput->descriptors.astAudio[0],
                  &hInput->auiNumDescriptors[0],
                  &hInput->descriptors.astAudio[1],
                  &hInput->auiNumDescriptors[1]
                                             ));

         /* Check for error */
         if ( BERR_SUCCESS != rc )
         {
            BDBG_ERR(("AUDIO: Error getting buffer descriptors"));
         }
      }
      break;

      default:
         /* should not happen - checked for at Create() time */
         BDBG_ERR(("Unknown input type: %d", hInput->stCreateSettings.eType));
         rc = BERR_INVALID_PARAMETER;
   }

   if ( BERR_SUCCESS == rc )
   {
      if ( ( false == hInput->bBufferStatusValid )
           && ( 0 != ( hInput->auiNumDescriptors[0] + hInput->auiNumDescriptors[1] ) )
         )
      {
         switch ( hInput->stCreateSettings.eType )
         {
            case BMUXlib_Input_Type_eVideo:
            {
               BMUXlib_VideoEncoderInterface *pInterface = &hInput->stCreateSettings.interface.stVideo;
               BAVC_VideoBufferStatus stVideoBufferStatus;

               rc = BERR_TRACE( pInterface->fGetBufferStatus(
                        pInterface->pContext,
                        &stVideoBufferStatus
                        ));

               if ( BERR_SUCCESS != rc)
               {
                  BDBG_ERR(("VIDEO: Error getting buffer status"));
               }
               else
               {
                  hInput->stBufferStatus = stVideoBufferStatus.stCommon;
                  hInput->bBufferStatusValid = true;
               }
            }
               break;
            case BMUXlib_Input_Type_eAudio:
            {
               BMUXlib_AudioEncoderInterface *pInterface = &hInput->stCreateSettings.interface.stAudio;
               BAVC_AudioBufferStatus stAudioBufferStatus;

               rc = BERR_TRACE( pInterface->fGetBufferStatus(
                        pInterface->pContext,
                        &stAudioBufferStatus
                        ));

               if ( BERR_SUCCESS != rc)
               {
                  BDBG_ERR(("AUDIO: Error getting buffer status"));
               }
               else
               {
                  hInput->stBufferStatus = stAudioBufferStatus.stCommon;
                  hInput->bBufferStatusValid = true;
               }
            }
               break;

            default:
               /* should not happen - checked for at Create() time */
               BDBG_ERR(("Unknown input type: %d", hInput->stCreateSettings.eType));
               rc = BERR_INVALID_PARAMETER;
         }
      }

      if ( BERR_SUCCESS == rc )
      {
         /* See if we need to filter descriptors based on the burst mode */
         if ( BMUXlib_Input_BurstMode_eFrame == hInput->stCreateSettings.eBurstMode )
         {
            size_t i;
            size_t uiNumDescriptors = hInput->auiNumDescriptors[0] + hInput->auiNumDescriptors[1];
            size_t uiNumNewDescriptors = 0;
            size_t uiNumDescriptorsInCurrentFrame = 0;
            BMUXlib_Input_Descriptor stDescriptor;

            /* Do not include new descriptors until all descriptors for the frame are available.
             * This means that there needs to be a subsequent descriptor with a FRAME_START or EOS.
             */

            for ( i = 0; i < uiNumDescriptors; i++ )
            {
               BMUXlib_Input_P_PeekAtDescriptor( hInput, i, &stDescriptor );
               /* the following should not happen */
               BDBG_ASSERT(stDescriptor.descriptor.pstCommon != NULL);

               if ( BMUXLIB_INPUT_DESCRIPTOR_IS_FRAMESTART( &stDescriptor ) )
               {
                  uiNumNewDescriptors += uiNumDescriptorsInCurrentFrame;
                  uiNumDescriptorsInCurrentFrame = 0;
                  if ( BMUXLIB_INPUT_DESCRIPTOR_IS_EOS( &stDescriptor )
                       || BMUXLIB_INPUT_DESCRIPTOR_IS_FRAMEEND( &stDescriptor )
                     )
                  {
                     uiNumNewDescriptors++; /* If this descriptor is also an EOS or FRAME_END, then this frame is complete */
                  }
                  else
                  {
                     uiNumDescriptorsInCurrentFrame++; /* Include this descriptor as the beginning of the next frame */
                  }
               }
               else if ( BMUXLIB_INPUT_DESCRIPTOR_IS_EOS( &stDescriptor )
                         || BMUXLIB_INPUT_DESCRIPTOR_IS_FRAMEEND( &stDescriptor )
                       )
               {
                  uiNumNewDescriptors += uiNumDescriptorsInCurrentFrame;
                  uiNumDescriptorsInCurrentFrame = 0;
                  uiNumNewDescriptors++; /* Include this descriptor as part of this new frame */
               }
               else
               {
                  uiNumDescriptorsInCurrentFrame++;  /* Include this descriptor as part of the next frame */
               }
            }

            /* Adjust num descriptors to ignore incomplete frames */
            if ( uiNumNewDescriptors < hInput->auiNumDescriptors[0] )
            {
               hInput->auiNumDescriptors[0] = uiNumNewDescriptors;
               hInput->auiNumDescriptors[1] = 0;
            }
            else
            {
               uiNumNewDescriptors -= hInput->auiNumDescriptors[0];
               hInput->auiNumDescriptors[1] = uiNumNewDescriptors;
            }
         }

         hInput->uiWriteOffset = hInput->auiNumDescriptors[0] + hInput->auiNumDescriptors[1];
         hInput->uiPendingOffset -= hInput->uiReadOffset;
         hInput->uiReadOffset = 0;

         /* SW7425-5205: Validate uiPendingOffset against uiWriteOffset to handle scenarios where the input
          * resets itself since the last call to GetBufferDescriptors.
          * E.g. the input watchdogged and/or the input had stale descriptors between stop/start which were flushed
          */
         if ( hInput->uiPendingOffset > hInput->uiWriteOffset )
         {
           /* BDBG_WRN(("Pending descriptors in mux (%d) is larger than the available descriptors in the input (%d)", hInput->uiPendingOffset, hInput->uiWriteOffset));*/
            hInput->uiPendingOffset = hInput->uiWriteOffset;
         }

#if BMUXLIB_INPUT_P_DUMP_DESC
         BMuxlib_Input_P_DumpNewDescriptors(hInput, uiNumOldDescriptors);
#endif
      }
   }

   BDBG_LEAVE( BMUXlib_Input_ProcessNewDescriptors );

   return BERR_TRACE( rc );
}

bool
BMUXlib_Input_IsDescriptorAvailable(
         BMUXlib_Input_Handle hInput
         )
{
   BDBG_ENTER( BMUXlib_Input_IsDescriptorAvailable );

   BDBG_ASSERT( hInput );
   BDBG_ASSERT( BMUXLIB_INPUT_P_SIGNATURE_CONTEXT == hInput->uiSignature );

   BDBG_MSG(("Available: %d Pending %d", hInput->uiWriteOffset, hInput->uiPendingOffset));

   BDBG_LEAVE( BMUXlib_Input_IsDescriptorAvailable );

   return ( hInput->uiWriteOffset > hInput->uiPendingOffset );
}

/* Returns true if a valid descriptor is available in pstDescriptor
   if no more descriptors, then pstDescriptor->descriptor.pstCommon will be NULL and it will return false
*/
bool
BMUXlib_Input_PeekAtNextDescriptor(
         BMUXlib_Input_Handle hInput,
         BMUXlib_Input_Descriptor *pstDescriptor
         )
{
   BDBG_ASSERT( hInput );
   BDBG_ASSERT( BMUXLIB_INPUT_P_SIGNATURE_CONTEXT == hInput->uiSignature );
   BDBG_ASSERT( pstDescriptor );

   BDBG_ENTER( BMUXlib_Input_PeekAtNextDescriptor );

   if ( hInput->uiWriteOffset == hInput->uiPendingOffset )
   {
      /* No more descriptors */
      pstDescriptor->descriptor.pstCommon = NULL;
      return false;
   }
   else
   {
      BMUXlib_Input_P_PeekAtDescriptor( hInput, hInput->uiPendingOffset, pstDescriptor );
      /* the following should not happen since we checked for descriptors available, above */
      BDBG_ASSERT(pstDescriptor->descriptor.pstCommon != NULL);

      /* See if we need to compute the frame size */
      if ( BMUXlib_Input_BurstMode_eFrame == hInput->stCreateSettings.eBurstMode )
      {
         if ( BMUXLIB_INPUT_DESCRIPTOR_IS_FRAMESTART( pstDescriptor )
              || BMUXLIB_INPUT_DESCRIPTOR_IS_EOS( pstDescriptor ) )
         {
            BMUXlib_Input_Descriptor stDescriptor;
            size_t i;
            /* Compute frame size */

            hInput->uiCurrentFrameSize = BMUXLIB_INPUT_DESCRIPTOR_LENGTH ( pstDescriptor );

            for ( i = hInput->uiPendingOffset + 1; i < hInput->uiWriteOffset; i++ )
            {
               BMUXlib_Input_P_PeekAtDescriptor( hInput, i, &stDescriptor );
               /* the following should not happen  */
               BDBG_ASSERT(stDescriptor.descriptor.pstCommon != NULL);

               if ( BMUXLIB_INPUT_DESCRIPTOR_IS_FRAMESTART( &stDescriptor )
                    || BMUXLIB_INPUT_DESCRIPTOR_IS_EOS( &stDescriptor ) )
               {
                  break;
               }
               else
               {
                  hInput->uiCurrentFrameSize += BMUXLIB_INPUT_DESCRIPTOR_LENGTH ( &stDescriptor );
               }
            }
         }

         pstDescriptor->uiFrameSize = hInput->uiCurrentFrameSize;
      }
   }

   BDBG_LEAVE( BMUXlib_Input_PeekAtNextDescriptor );
   return true;   /* descriptor is available in pstDescriptor */
}

/* Returns true if a valid descriptor is available in pstDescriptor
   if no more descriptors, then pstDescriptor->descriptor.pstCommon will be NULL and it will return false
*/
bool
BMUXlib_Input_GetNextDescriptor(
         BMUXlib_Input_Handle hInput,
         BMUXlib_Input_Descriptor *pstDescriptor
         )
{
   bool bDescAvail;

   BDBG_ASSERT( hInput );
   BDBG_ASSERT( BMUXLIB_INPUT_P_SIGNATURE_CONTEXT == hInput->uiSignature );
   BDBG_ASSERT( pstDescriptor );

   BDBG_ENTER( BMUXlib_Input_GetNextDescriptor );

   bDescAvail = BMUXlib_Input_PeekAtNextDescriptor( hInput, pstDescriptor );
   if (bDescAvail)
   {
      /* descriptor is available, so return it to the user */
      hInput->uiPendingOffset++;
   }

   BDBG_LEAVE( BMUXlib_Input_GetNextDescriptor );
   return bDescAvail;
}

BERR_Code
BMUXlib_Input_ConsumeDescriptors(
         BMUXlib_Input_Handle hInput,
         size_t uiCount
         )
{
   BERR_Code rc = BERR_SUCCESS;

   BDBG_ENTER( BMUXlib_Input_ConsumeDescriptors );

   BDBG_ASSERT( hInput );
   BDBG_ASSERT( BMUXLIB_INPUT_P_SIGNATURE_CONTEXT == hInput->uiSignature );

   /* SW7425-5205: There was a mismatch between the pending descriptors and the available descriptors,
    * reduce the released count to match the available descriptors
    */
   if ( ( hInput->uiPendingOffset - hInput->uiReadOffset ) < uiCount )
   {
      BDBG_WRN(("Available descriptors in input (%d) is less than the number consumed (%d)", hInput->uiPendingOffset - hInput->uiReadOffset, uiCount));
      uiCount = hInput->uiPendingOffset - hInput->uiReadOffset;
   }

   if( 0 == uiCount )
   {
      BDBG_LEAVE( BMUXlib_Input_ConsumeDescriptors );
      return BERR_TRACE( rc );
   }

   switch ( hInput->stCreateSettings.eType )
   {
      case BMUXlib_Input_Type_eVideo:
      {
         BMUXlib_VideoEncoderInterface *pInterface = &hInput->stCreateSettings.interface.stVideo;

         /* Consume Descriptors */
         rc = BERR_TRACE(pInterface->fConsumeBufferDescriptors(
                  pInterface->pContext,
                  uiCount
                  ));

         /* Check for error */
         if ( BERR_SUCCESS != rc )
         {
            BDBG_ERR(("VIDEO Error consuming buffer descriptors"));
            return false;
         }
      }
         break;

      case BMUXlib_Input_Type_eAudio:
      {
         BMUXlib_AudioEncoderInterface *pInterface = &hInput->stCreateSettings.interface.stAudio;

         /* Consume Descriptors */
         rc = BERR_TRACE(pInterface->fConsumeBufferDescriptors(
                  pInterface->pContext,
                  uiCount
                  ));

         /* Check for error */
         if ( BERR_SUCCESS != rc )
         {
            BDBG_ERR(("AUDIO Error consuming buffer descriptors"));
            return false;
         }
      }
         break;

      default:
         BDBG_ERR(("Unknown input type"));
         return false;
   }

   if ( uiCount > 0 )
   {
      unsigned i;

      /* find the latest consumed PTS */
      for ( i = hInput->uiReadOffset + uiCount - 1; i >= hInput->uiReadOffset; i-- )
      {
         BMUXlib_Input_Descriptor stDescriptor;

         BMUXlib_Input_P_PeekAtDescriptor( hInput, i, &stDescriptor );
         /* the following should not happen  */
         BDBG_ASSERT(stDescriptor.descriptor.pstCommon != NULL);

         if ( BMUXLIB_INPUT_DESCRIPTOR_IS_PTS_VALID( &stDescriptor ) )
         {
            hInput->stats.stStatus.uiCurrentDTS = BMUXLIB_INPUT_DESCRIPTOR_DTS(&stDescriptor);

            if ( false == hInput->stats.stStatus.bInitialDTSValid )
            {
               hInput->stats.stStatus.uiInitialDTS = hInput->stats.stStatus.uiCurrentDTS;
               hInput->stats.stStatus.bInitialDTSValid = true;
            }

            break;
         }
         if ( i == 0 ) break;
      }
   }

   hInput->uiReadOffset += uiCount;

   BDBG_LEAVE( BMUXlib_Input_ConsumeDescriptors );

   return BERR_TRACE( rc );
}

BERR_Code
BMUXlib_Input_GetStatus(
         BMUXlib_Input_Handle hInput,
         BMUXlib_Input_Status *pstStatus
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ASSERT( hInput );
   BDBG_ASSERT( BMUXLIB_INPUT_P_SIGNATURE_CONTEXT == hInput->uiSignature );
   BDBG_ASSERT( pstStatus );

   BDBG_ENTER( BMUXlib_Input_GetStatus );

   *pstStatus = hInput->stats.stStatus;

   BDBG_LEAVE( BMUXlib_Input_GetStatus );
   return BERR_TRACE( rc );
}

#if BMUXLIB_INPUT_P_DUMP_DESC
void BMUXlib_Input_P_SetCountLocation(BMUXlib_Input_Handle hInput, uint32_t *puiCount)
{
   if (NULL != puiCount)
      hInput->puiDescCount = puiCount;
}
#endif

/****************************************
  Input Group Processing
****************************************/
void
BMUXlib_InputGroup_GetDefaultCreateSettings(
         BMUXlib_InputGroup_CreateSettings *pstSettings
         )
{
   BDBG_ENTER( BMUXlib_InputGroup_GetDefaultCreateSettings );
   BDBG_ASSERT( pstSettings );

   /* this ensures there are no inputs in the group, and we do not wait for inputs */
   BKNI_Memset(pstSettings, 0, sizeof(BMUXlib_InputGroup_CreateSettings));
   pstSettings->uiSignature = BMUXLIB_INPUTGROUP_P_SIGNATURE_CREATESETTINGS;

   BDBG_LEAVE( BMUXlib_InputGroup_GetDefaultCreateSettings );
}

BERR_Code
BMUXlib_InputGroup_Create(
         BMUXlib_InputGroup_Handle *phInputGroup,
         const BMUXlib_InputGroup_CreateSettings *pstSettings
         )
{
   BMUXlib_InputGroup_Handle hInputGroup;
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ASSERT(phInputGroup);
   BDBG_ASSERT(pstSettings);
   BDBG_ASSERT(BMUXLIB_INPUTGROUP_P_SIGNATURE_CREATESETTINGS == pstSettings->uiSignature);

   BDBG_ENTER(BMUXlib_InputGroup_Create);

   *phInputGroup = NULL;

   /* verify the settings ... */
   if ((0 == pstSettings->uiInputCount)
      || (NULL == pstSettings->pInputTable))
   {
      BDBG_LEAVE(BMUXlib_InputGroup_Create);
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   hInputGroup = BKNI_Malloc(sizeof(struct BMUXlib_InputGroup_P_Context));
   if (NULL != hInputGroup)
   {
      uint32_t uiInputIndex;
      BKNI_Memset( hInputGroup, 0, sizeof( struct BMUXlib_InputGroup_P_Context ));
      hInputGroup->uiSignature = BMUXLIB_INPUTGROUP_P_SIGNATURE_CONTEXT;
      hInputGroup->pInputTable = BKNI_Malloc(sizeof( BMUXlib_InputGroup_P_InputEntry ) * pstSettings->uiInputCount);
      hInputGroup->pInputSelectTable = BKNI_Malloc(sizeof( BMUXlib_InputGroup_P_InputEntry *) * pstSettings->uiInputCount);

      if ((NULL == hInputGroup->pInputTable) ||
          (NULL == hInputGroup->pInputSelectTable))
      {
         BMUXlib_InputGroup_Destroy(hInputGroup);
         BDBG_LEAVE(BMUXlib_InputGroup_Create);
         return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
      }
      /* clear the two tables... */
      BKNI_Memset(hInputGroup->pInputTable, 0, sizeof( BMUXlib_InputGroup_P_InputEntry ) * pstSettings->uiInputCount);
      BKNI_Memset(hInputGroup->pInputSelectTable, 0 , sizeof( BMUXlib_InputGroup_P_InputEntry *) * pstSettings->uiInputCount);

      for (uiInputIndex = 0; uiInputIndex < pstSettings->uiInputCount; uiInputIndex++)
      {
         BMUXlib_InputGroup_P_InputEntry *pEntry = &(hInputGroup->pInputTable)[uiInputIndex];
         BMUXlib_Input_Handle hInput = (pstSettings->pInputTable)[uiInputIndex];
         /* verify table contains valid inputs */
         if ((NULL == hInput)
            || (hInput->uiSignature != BMUXLIB_INPUT_P_SIGNATURE_CONTEXT))
         {
            rc = BERR_TRACE(BERR_INVALID_PARAMETER);
            break;
         }
         pEntry->hInput = hInput;
         pEntry->bActive = true; /* initially, all inputs are "active" */
         /* descriptor will be filled in as needed during input selection */
#if BMUXLIB_INPUT_P_DUMP_DESC
         BMUXlib_Input_P_SetCountLocation(hInput, &hInputGroup->uiDescCount);
#endif
      }

      if (BERR_SUCCESS == rc)
      {
         BMUXlib_InputGroup_GetDefaultSettings(&hInputGroup->stSettings);
         hInputGroup->uiNumInputs = pstSettings->uiInputCount;
         /* initially, all inputs are "active" */
         hInputGroup->stStatus.uiNumActiveInputs = pstSettings->uiInputCount;
         *phInputGroup = hInputGroup;
      }
      else
         BMUXlib_InputGroup_Destroy(hInputGroup);
   }
   else
      rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);

   BDBG_LEAVE(BMUXlib_InputGroup_Create);
   return rc;
}

void
BMUXlib_InputGroup_Destroy(
         BMUXlib_InputGroup_Handle hInputGroup
         )
{
   BDBG_ASSERT(hInputGroup);
   /* the following signifies an attempt to free up something that was either
      a) not created by Create()
      b) has already been destroyed
   */
   BDBG_ASSERT(BMUXLIB_INPUTGROUP_P_SIGNATURE_CONTEXT == hInputGroup->uiSignature);

   BDBG_ENTER(BMUXlib_Input_DestroyGroup);

   if (NULL != hInputGroup->pInputTable)
      BKNI_Free(hInputGroup->pInputTable);
   if (NULL != hInputGroup->pInputSelectTable)
      BKNI_Free(hInputGroup->pInputSelectTable);
   hInputGroup->uiSignature = 0;
   BKNI_Free(hInputGroup);

   BDBG_LEAVE(BMUXlib_Input_DestroyGroup);
}

void
BMUXlib_InputGroup_GetDefaultSettings(
         BMUXlib_InputGroup_Settings *pstSettings
         )
{
   BDBG_ASSERT( pstSettings );

   BDBG_ENTER( BMUXlib_InputGroup_GetDefaultSettings );

   BKNI_Memcpy( pstSettings, &s_stDefaultInputGroupSettings, sizeof(BMUXlib_InputGroup_Settings) );

   BDBG_LEAVE( BMUXlib_InputGroup_GetDefaultSettings );
}

BERR_Code
BMUXlib_InputGroup_SetSettings(
         BMUXlib_InputGroup_Handle hInputGroup,
         const BMUXlib_InputGroup_Settings *pstSettings)
{
   BERR_Code rc = BERR_UNKNOWN;
   BDBG_ASSERT( hInputGroup );
   BDBG_ASSERT( BMUXLIB_INPUTGROUP_P_SIGNATURE_CONTEXT == hInputGroup->uiSignature );
   BDBG_ASSERT( pstSettings );
   BDBG_ASSERT( BMUXLIB_INPUTGROUP_P_SIGNATURE_SETTINGS == pstSettings->uiSignature );

   BDBG_ENTER( BMUXlib_InputGroup_SetSettings );
   if (NULL == pstSettings->fSelector)
   {
      /* one or more settings is invalid ... */
      rc = BERR_TRACE(BERR_INVALID_PARAMETER);
   }
   else
   {
      hInputGroup->stSettings = *pstSettings;
      rc = BERR_SUCCESS;
   }
   BDBG_LEAVE( BMUXlib_InputGroup_SetSettings );
   return rc;
}

BERR_Code
BMUXlib_InputGroup_GetSettings(
         BMUXlib_InputGroup_Handle hInputGroup,
         BMUXlib_InputGroup_Settings *pstSettings)
{
   BDBG_ASSERT( hInputGroup );
   BDBG_ASSERT( BMUXLIB_INPUTGROUP_P_SIGNATURE_CONTEXT == hInputGroup->uiSignature );
   BDBG_ASSERT( pstSettings );

   BDBG_ENTER( BMUXlib_InputGroup_GetSettings );

   *pstSettings = hInputGroup->stSettings;

   BDBG_LEAVE( BMUXlib_InputGroup_GetSettings );
   return BERR_SUCCESS;
}

BERR_Code
BMUXlib_InputGroup_ActivateInput(
         BMUXlib_InputGroup_Handle hInputGroup,
         BMUXlib_Input_Handle hInput,
         bool bActive
         )
{
   uint32_t uiInputIndex;
   BMUXlib_InputGroup_P_InputEntry *pEntry = NULL;

   BDBG_ASSERT( hInputGroup );
   BDBG_ASSERT( BMUXLIB_INPUTGROUP_P_SIGNATURE_CONTEXT == hInputGroup->uiSignature );
   BDBG_ASSERT( hInput );
   BDBG_ASSERT( BMUXLIB_INPUT_P_SIGNATURE_CONTEXT == hInput->uiSignature );

   BDBG_ENTER( BMUXlib_InputGroup_ActivateInput );

   /* find the entry for this input */
   for (uiInputIndex = 0; uiInputIndex < hInputGroup->uiNumInputs; uiInputIndex++)
   {
      pEntry = &(hInputGroup->pInputTable)[uiInputIndex];
      if (pEntry->hInput == hInput)
         break;
   }
   if (uiInputIndex == hInputGroup->uiNumInputs)
   {
      /* invalid input specified ... */
      BDBG_LEAVE( BMUXlib_InputGroup_DisableInput );
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   if (bActive && !pEntry->bActive)
      hInputGroup->stStatus.uiNumActiveInputs++;

   if (!bActive && pEntry->bActive)
      hInputGroup->stStatus.uiNumActiveInputs--;

   pEntry->bActive = bActive;

   BDBG_LEAVE( BMUXlib_InputGroup_ActivateInput );
   return BERR_SUCCESS;
}

BERR_Code
BMUXlib_InputGroup_GetStatus(
         BMUXlib_InputGroup_Handle hInputGroup,
         BMUXlib_InputGroup_Status *pstStatus
         )
{
   unsigned uiInputNum;
   uint64_t uiSmallestInitialDTS = -1;
   uint64_t uiSmallestCurrentDTS = -1;

   BDBG_ASSERT( hInputGroup );
   BDBG_ASSERT( BMUXLIB_INPUTGROUP_P_SIGNATURE_CONTEXT == hInputGroup->uiSignature );
   BDBG_ASSERT( pstStatus );

   BDBG_ENTER( BMUXlib_InputGroup_GetStatus );

   /* Calculate completed duration */
   for ( uiInputNum = 0; uiInputNum < hInputGroup->uiNumInputs; uiInputNum++ )
   {
      BMUXlib_InputGroup_P_InputEntry *pEntry = &(hInputGroup->pInputTable)[uiInputNum];
      BMUXlib_Input_Handle hInput = pEntry->hInput;
      BMUXlib_Input_Status stInputStatus;

      BDBG_ASSERT( hInput );
      if (!pEntry->bActive)
         /* this input is not active, so skip it ... */
         continue;

      BMUXlib_Input_GetStatus(hInput, &stInputStatus);
      if ( true == stInputStatus.bInitialDTSValid )
      {
         if ( uiSmallestInitialDTS > stInputStatus.uiInitialDTS )
         {
            uiSmallestInitialDTS = stInputStatus.uiInitialDTS;
         }

         if ( uiSmallestCurrentDTS > stInputStatus.uiCurrentDTS )
         {
            uiSmallestCurrentDTS = stInputStatus.uiCurrentDTS;
         }
      }
   }

   if ( hInputGroup->stStatus.uiNumActiveInputs > 0 )
   {
      hInputGroup->stStatus.uiDuration = (uiSmallestCurrentDTS - uiSmallestInitialDTS)/90;
   }

   *pstStatus = hInputGroup->stStatus;

   BDBG_LEAVE( BMUXlib_InputGroup_GetStatus );
   return BERR_SUCCESS;
}

/* NOTE: this function always succeeds.
   The caller should check *phInput to ensure it is non-NULL
   (i.e. an input was selected).
*/
void
BMUXlib_InputGroup_GetNextInput(
         BMUXlib_InputGroup_Handle hInputGroup,
         BMUXlib_Input_Handle *phInput
         )
{
   unsigned uiInputNum;
   uint32_t uiActiveInputCount = 0;

   BDBG_ENTER( BMUXlib_InputGroup_GetNextInput );
   BDBG_ASSERT( hInputGroup );
   BDBG_ASSERT( BMUXLIB_INPUTGROUP_P_SIGNATURE_CONTEXT == hInputGroup->uiSignature );
   BDBG_ASSERT( phInput );

   *phInput = NULL;
   for ( uiInputNum = 0; uiInputNum < hInputGroup->uiNumInputs; uiInputNum++ )
   {
      BMUXlib_InputGroup_P_InputEntry *pEntry = &(hInputGroup->pInputTable)[uiInputNum];
      BMUXlib_Input_Handle hInput = pEntry->hInput;
      BDBG_ASSERT( hInput );
      if (!pEntry->bActive)
         /* this input is not active, so skip it ... */
         continue;

      if (!pEntry->bDescriptorValid)
      {
         /* the descriptor for this input is no longer the latest, so check for an update */
         if ( true == BMUXlib_Input_IsDescriptorAvailable( hInput ) )
         {
            bool bDescAvail;
            bDescAvail = BMUXlib_Input_PeekAtNextDescriptor( hInput, &pEntry->stDescriptor );
            /* there should always be a descriptor available, since we checked above */
            BDBG_ASSERT(bDescAvail == true);
            pEntry->bDescriptorValid = bDescAvail;
         }
         else
         {
            /* no descriptor available, so invalidate the descriptor in the entry */
            pEntry->bDescriptorValid = false;
            if ( true == hInputGroup->stSettings.bWaitForAllInputs )
            {
               BDBG_MSG(( "Input[%d] is dry", uiInputNum ));
               BDBG_LEAVE( BMUXlib_InputGroup_GetNextInput );
               return;       /* wait until all inputs have descriptors available */
            }
         }
      }
      /* if current descriptor is valid, simply store this input in the active list */
      if (pEntry->bDescriptorValid)
      {
         (hInputGroup->pInputSelectTable)[uiActiveInputCount++] = pEntry;
      }
   }  /* end: for each input */
   if (uiActiveInputCount > 0)
   {
      /* sort the input entry array, based on the selector function supplied */
      BMUXlib_InputGroup_P_SortInputs(hInputGroup->pInputSelectTable, uiActiveInputCount, hInputGroup->stSettings.fSelector);
      *phInput = (hInputGroup->pInputSelectTable)[0]->hInput;        /* select the first input in the sorted array */
      (hInputGroup->pInputSelectTable)[0]->bDescriptorValid = false; /* invalidate this entry's descriptor since it is likely to be consumed */
   }

   BDBG_LEAVE( BMUXlib_InputGroup_GetNextInput );
}

#define BMUXLIB_INPUTGROUP_P_SKEW_DETECTION_THRESHOLD 15
BMUXlib_InputGroup_SelectionResult
BMUXlib_InputGroup_DescriptorSelectLowestDTS (
   BMUXlib_Input_Descriptor *pstDescriptorA,
   BMUXlib_Input_Descriptor *pstDescriptorB
   )
{
   BDBG_ENTER( BMUXlib_InputGroup_DescriptorSelectLowestDTS );

   BDBG_ASSERT( pstDescriptorA );
   BDBG_ASSERT( pstDescriptorB );

   BDBG_ASSERT( BMUXLIB_INPUT_DESCRIPTOR_IS_FRAMESTART( pstDescriptorA ) );
   BDBG_ASSERT( BMUXLIB_INPUT_DESCRIPTOR_IS_ESCR_VALID( pstDescriptorA ) );
   BDBG_ASSERT( BMUXLIB_INPUT_DESCRIPTOR_IS_DTS_VALID( pstDescriptorA ) );
   BDBG_ASSERT( BMUXLIB_INPUT_DESCRIPTOR_IS_PTS_VALID( pstDescriptorA ) );

   BDBG_ASSERT( BMUXLIB_INPUT_DESCRIPTOR_IS_FRAMESTART( pstDescriptorB ) );
   BDBG_ASSERT( BMUXLIB_INPUT_DESCRIPTOR_IS_ESCR_VALID( pstDescriptorB ) );
   BDBG_ASSERT( BMUXLIB_INPUT_DESCRIPTOR_IS_DTS_VALID( pstDescriptorB ) );
   BDBG_ASSERT( BMUXLIB_INPUT_DESCRIPTOR_IS_PTS_VALID( pstDescriptorB ) );

   /* NOTE: This uses a scaled DTS to restrict the range to 32-bits
      (to avoid the need for 64-bit math and 64-bit constants).
      This therefore makes the assumption that two DTS values are not separated
      by only their LSB (very unlikely). */
   {
      uint32_t uiDTSA = BMUXLIB_INPUT_DESCRIPTOR_DTS( pstDescriptorA ) >> 1;
      uint32_t uiDTSB = BMUXLIB_INPUT_DESCRIPTOR_DTS( pstDescriptorB ) >> 1;

#if BDBG_DEBUG_BUILD && 0
      if ( uiDTSB != uiDTSA )
      {
         unsigned uiDeltaDTS;
         if ( uiDTSB < uiDTSA )
         {
            uiDeltaDTS = uiDTSA - uiDTSB;
         }
         else
         {
            uiDeltaDTS = uiDTSB - uiDTSA;
         }

         if ( ( uiDeltaDTS < ( 0xFFFFFFFF - ( BMUXLIB_INPUTGROUP_P_SKEW_DETECTION_THRESHOLD*45000 - 1 ) ) )
            && ( ( uiDeltaDTS / 45000 ) > BMUXLIB_INPUTGROUP_P_SKEW_DETECTION_THRESHOLD ) )
         {
            BDBG_WRN(("Input DTS values are more than %d seconds apart! (%d:%08x vs %d:%08x)", BMUXLIB_INPUTGROUP_P_SKEW_DETECTION_THRESHOLD, uiDTSB, uiDTSB, uiDTSA, uiDTSA ));
         }
      }
#endif
      if ( ( ( uiDTSA < uiDTSB ) && ( ( uiDTSB - uiDTSA ) < BMUXLIB_INPUTGROUP_P_SCALED_DTS_MIDRANGE ) )
           || ( ( uiDTSA > uiDTSB ) && ( ( uiDTSA - uiDTSB ) >= BMUXLIB_INPUTGROUP_P_SCALED_DTS_MIDRANGE ) )
         )
      {
         /* A.DTS is < B.DTS */
         BDBG_LEAVE( BMUXlib_InputGroup_DescriptorSelectLowestDTS );
         return BMUXlib_InputGroup_SelectionResult_eSelectA;
      }
      else
      {
         BDBG_LEAVE( BMUXlib_InputGroup_DescriptorSelectLowestDTS );
         return BMUXlib_InputGroup_SelectionResult_eSelectB;
      }
   }
}

BMUXlib_InputGroup_SelectionResult
BMUXlib_InputGroup_DescriptorSelectLowestESCR (
   BMUXlib_Input_Descriptor *pstDescriptorA,
   BMUXlib_Input_Descriptor *pstDescriptorB
   )
{
   BMUXlib_InputGroup_SelectionResult eResult;
   BDBG_ENTER( BMUXlib_InputGroup_DescriptorSelectLowestESCR );

   BDBG_ASSERT( pstDescriptorA );
   BDBG_ASSERT( pstDescriptorB );

   if ( ( BMUXLIB_INPUT_DESCRIPTOR_IS_ESCR_VALID( pstDescriptorA ) )
        && ( BMUXLIB_INPUT_DESCRIPTOR_IS_ESCR_VALID( pstDescriptorB ) ) )
   {
      /* scaled ESCR values ... */
      uint32_t uiESCRA = BMUXLIB_INPUT_DESCRIPTOR_ESCR( pstDescriptorA );
      uint32_t uiESCRB = BMUXLIB_INPUT_DESCRIPTOR_ESCR( pstDescriptorB );
      int32_t iDeltaESCR = uiESCRA - uiESCRB;
#if BDBG_DEBUG_BUILD && 0
      {
         unsigned uiAbsDeltaESCR = iDeltaESCR;
         if (iDeltaESCR < 0)
            uiAbsDeltaESCR = -iDeltaESCR;
         if (uiAbsDeltaESCR > BMUXLIB_INPUTGROUP_P_SKEW_DETECTION_THRESHOLD * 27000000)
         {
            BDBG_WRN(("Input ESCR values are more than %d seconds apart! [%s]:%08x vs [%s]:%08x (delta:%x)",
               BMUXLIB_INPUTGROUP_P_SKEW_DETECTION_THRESHOLD,
               (BMUXlib_Input_Type_eVideo == pstDescriptorB->eType)?"Video":"Audio", uiESCRB,
               (BMUXlib_Input_Type_eVideo == pstDescriptorA->eType)?"Video":"Audio", uiESCRA,
               uiAbsDeltaESCR));
         }
      }
#endif /* End: Debug */

      if (iDeltaESCR < 0)
      {
         /*  A.ESCR < B.ESCR - i.e. A is the selected input */
         eResult = BMUXlib_InputGroup_SelectionResult_eSelectA;
      }
      else
      {
         eResult = BMUXlib_InputGroup_SelectionResult_eSelectB;
      }
   }
   else if ( false == BMUXLIB_INPUT_DESCRIPTOR_IS_ESCR_VALID( pstDescriptorA ) )
   {
      /* A is missing ESCR, or both are missing ESCR */
      /* NOTE: if the ESCR is invalid, the descriptor is likely to be part of the current frame
         so we want to consume those BEFORE selecting a different input */
      eResult = BMUXlib_InputGroup_SelectionResult_eSelectA;
   }
   else
   {
      /* B is missing ESCR */
      eResult = BMUXlib_InputGroup_SelectionResult_eSelectB;
   }

   BDBG_LEAVE( BMUXlib_InputGroup_DescriptorSelectLowestESCR );

   return eResult;
}

/***********************
   Static Functions
***********************/
static void
BMUXlib_Input_P_PeekAtDescriptor(
         BMUXlib_Input_Handle hInput,
         unsigned int uiDescNum,
         BMUXlib_Input_Descriptor *pstDescriptor
         )
{
   BDBG_ASSERT( hInput );
   BDBG_ASSERT( BMUXLIB_INPUT_P_SIGNATURE_CONTEXT == hInput->uiSignature );
   BDBG_ASSERT( pstDescriptor );

   BDBG_ENTER( BMUXlib_Input_P_PeekAtDescriptor );

   pstDescriptor->descriptor.pstCommon = NULL;
   /* check for attempt to peek at non-existent descriptor */
   BDBG_ASSERT( uiDescNum < (hInput->auiNumDescriptors[0] + hInput->auiNumDescriptors[1]) );

   pstDescriptor->eType = hInput->stCreateSettings.eType;
   switch ( hInput->stCreateSettings.eType )
   {
      case BMUXlib_Input_Type_eVideo:
         if ( uiDescNum < hInput->auiNumDescriptors[0] )
         {
            pstDescriptor->descriptor.pstVideo = &hInput->descriptors.astVideo[0][uiDescNum];
         }
         else
         {
            pstDescriptor->descriptor.pstVideo = &hInput->descriptors.astVideo[1][uiDescNum - hInput->auiNumDescriptors[0]];
         }
         break;
      case BMUXlib_Input_Type_eAudio:
         if ( uiDescNum < hInput->auiNumDescriptors[0] )
         {
            pstDescriptor->descriptor.pstAudio = &hInput->descriptors.astAudio[0][uiDescNum];
         }
         else
         {
            pstDescriptor->descriptor.pstAudio = &hInput->descriptors.astAudio[1][uiDescNum - hInput->auiNumDescriptors[0]];
         }
         break;
      default:
         /* NOTE: since eType is checked at Create-time, this should not happen */
         break;
   }
   BDBG_ASSERT( true == hInput->bBufferStatusValid );

   if ( NULL != pstDescriptor->descriptor.pstCommon )
   {
      if ( BMUXLIB_INPUT_DESCRIPTOR_IS_METADATA( pstDescriptor ) )
      {
         pstDescriptor->pBaseAddress = hInput->stBufferStatus.pMetadataBufferBaseAddress;
      }
      else
      {
         pstDescriptor->pBaseAddress = hInput->stBufferStatus.pFrameBufferBaseAddress;
      }
   }
   else
   {
      pstDescriptor->pBaseAddress = NULL;
   }
   pstDescriptor->uiFrameSize = 0;  /* framesize not known yet */

   BDBG_LEAVE( BMUXlib_Input_P_PeekAtDescriptor );
}

/* rudimentary shell sort using gaps of n/2
   Shell sort is used since we expect uiCount to be small (<= 32, typically count is 2!)
   and shell sort is a non-recursive in-place sort.
   NOTE: the selector function is a function that compares input descriptors */
static void BMUXlib_InputGroup_P_SortInputs(BMUXlib_InputGroup_P_InputEntry *aData[], uint32_t uiCount, BMUXlib_InputGroup_DescriptorSelector fSelect)
{
   BMUXlib_InputGroup_P_InputEntry *pTemp;
   uint32_t uiGap, i;
   int j;
   for (uiGap = uiCount/2; uiGap != 0; uiGap /= 2)
      for (i = uiGap; i < uiCount; i++)
         for (j = i - uiGap; j >= 0 && fSelect(&aData[j]->stDescriptor, &aData[j+uiGap]->stDescriptor) > 0; j -= uiGap)
         {
            pTemp = aData[j];
            aData[j] = aData[j+uiGap];
            aData[j+uiGap] = pTemp;
         }
}

#if BMUXLIB_INPUT_P_DUMP_DESC
static void BMuxlib_Input_P_DumpNewDescriptors(BMUXlib_Input_Handle hInput, unsigned int uiNumOldDescriptors)
{
   if ( NULL != hInput->hDescDumpFile )
   {
      unsigned uiIndex;

      for ( uiIndex = uiNumOldDescriptors; uiIndex < hInput->uiWriteOffset; uiIndex++ )
      {
         BMUXlib_Input_Descriptor stDescriptor;
         BMUXlib_Input_P_PeekAtDescriptor( hInput, uiIndex, &stDescriptor );
         /* the following should not happen */
         BDBG_ASSERT(stDescriptor.descriptor.pstCommon != NULL);
         /* all descriptors for a given input are of the type for the input */
         BDBG_ASSERT(BMUXLIB_INPUT_DESCRIPTOR_TYPE(&stDescriptor) == hInput->stCreateSettings.eType);

#if BMUXLIB_INPUT_P_DUMP_FRAME_DESC
         if ( BMUXLIB_INPUT_DESCRIPTOR_IS_FRAMESTART( &stDescriptor ) )
#endif
         {
         /* input descriptor specific */
         fprintf(hInput->hDescDumpFile, "%d,%p,%d,",
            *hInput->puiDescCount,
            BMUXLIB_INPUT_DESCRIPTOR_BASE_ADDRESS(&stDescriptor),
            (hInput->stCreateSettings.eBurstMode == BMUXlib_Input_BurstMode_eFrame)?BMUXLIB_INPUT_DESCRIPTOR_FRAMESIZE(&stDescriptor):0);
         /* Common */
         fprintf( hInput->hDescDumpFile, "0x%08x,%u,%u,%u,%llu,%u,%u,%d,%llu",
            BMUXLIB_INPUT_DESCRIPTOR_FLAGS(&stDescriptor),
            BMUXLIB_INPUT_DESCRIPTOR_OFFSET(&stDescriptor),
            BMUXLIB_INPUT_DESCRIPTOR_LENGTH(&stDescriptor),
            BMUXLIB_INPUT_DESCRIPTOR_ORIGINAL_PTS(&stDescriptor),
            BMUXLIB_INPUT_DESCRIPTOR_PTS(&stDescriptor),
            BMUXLIB_INPUT_DESCRIPTOR_ESCR(&stDescriptor),
            BMUXLIB_INPUT_DESCRIPTOR_TICKS_PER_BIT(&stDescriptor),
            BMUXLIB_INPUT_DESCRIPTOR_SHR(&stDescriptor),
            BMUXLIB_INPUT_DESCRIPTOR_STC_SNAPSHOT(&stDescriptor)
            );

         /* Type Specific */
         switch ( BMUXLIB_INPUT_DESCRIPTOR_TYPE(&stDescriptor) )
         {
            case BMUXlib_Input_Type_eVideo:
               fprintf( hInput->hDescDumpFile, ",0x%08x,%llu,%u,%d",
                  BMUXLIB_INPUT_DESCRIPTOR_VIDEO_FLAGS(&stDescriptor),
                  BMUXLIB_INPUT_DESCRIPTOR_VIDEO_DTS(&stDescriptor),
                  BMUXLIB_INPUT_DESCRIPTOR_VIDEO_DATA_UNIT_TYPE(&stDescriptor),
                  BMUXLIB_INPUT_DESCRIPTOR_IS_KEYFRAME(&stDescriptor)
                  );
               break;
            case BMUXlib_Input_Type_eAudio:
               fprintf( hInput->hDescDumpFile, ",%u,%u,%u",
                  BMUXLIB_INPUT_DESCRIPTOR_AUDIO_RAWOFFSET(&stDescriptor),
                  BMUXLIB_INPUT_DESCRIPTOR_AUDIO_RAWLENGTH(&stDescriptor),
                  BMUXLIB_INPUT_DESCRIPTOR_AUDIO_DATA_UNIT_TYPE(&stDescriptor)
                  );
               break;
            default:
               break;
         }

         fprintf( hInput->hDescDumpFile, "\n" );
         fflush( hInput->hDescDumpFile );
         (*hInput->puiDescCount)++;
         }
#if BMUXLIB_INPUT_P_DUMP_METADATA_DESC && !BMUXLIB_INPUT_P_DUMP_FRAME_DESC
         /* we only want to output metadata if we are dumping all descriptors */
         if (hInput->hMetadataDumpFile != NULL)
         {
            BMUXlib_Input_Descriptor *pDesc = &stDescriptor;
            if ( BMUXLIB_INPUT_DESCRIPTOR_IS_METADATA(pDesc) && (NULL != pDesc->pBaseAddress))
            {
               switch ( hInput->stCreateSettings.eType )
               {
                  case BMUXlib_Input_Type_eVideo:
                     /* read the data unit type to determine the metadata type */
                     if (BAVC_VideoMetadataType_eCommon == BMUXLIB_INPUT_DESCRIPTOR_VIDEO_DATA_UNIT_TYPE(pDesc))
                     {
                        BAVC_VideoMetadataDescriptor *pMetadata = (BAVC_VideoMetadataDescriptor *)BMUXLIB_INPUT_DESCRIPTOR_VIRTUAL_ADDRESS(pDesc);
                        fprintf(hInput->hMetadataDumpFile, "0x%x, %d, %d, %d, %d, %d, %d, %d, ",
                           pMetadata->uiMetadataFlags, pMetadata->stBufferInfo.eProtocol,
                           pMetadata->stBufferInfo.eProfile, pMetadata->stBufferInfo.eLevel,
                           pMetadata->stBitrate.uiMax, pMetadata->stFrameRate.eFrameRateCode,
                           pMetadata->stDimension.coded.uiWidth, pMetadata->stDimension.coded.uiHeight);
                        switch (pMetadata->stBufferInfo.eProtocol)
                        {
                           case BAVC_VideoCompressionStd_eVC1SimpleMain:
                              /* print VC1 codec-specific data */
                              fprintf(hInput->hMetadataDumpFile, "%d, %d, ",
                                 pMetadata->uProtocolData.stVC1.sequenceHeaderB.bCBR,
                                 pMetadata->uProtocolData.stVC1.sequenceHeaderB.uiHRDBuffer);
                              fprintf(hInput->hMetadataDumpFile, "%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, ",
                                 pMetadata->uProtocolData.stVC1.sequenceHeaderC.uiQuantizedFrameRatePostProcessing,
                                 pMetadata->uProtocolData.stVC1.sequenceHeaderC.uiQuantizedBitratePostProcessing,
                                 pMetadata->uProtocolData.stVC1.sequenceHeaderC.bLoopFilter,
                                 pMetadata->uProtocolData.stVC1.sequenceHeaderC.bMultiResolution,
                                 pMetadata->uProtocolData.stVC1.sequenceHeaderC.bFastUVMotionCompensation,
                                 pMetadata->uProtocolData.stVC1.sequenceHeaderC.bExtendedMotionVector,
                                 pMetadata->uProtocolData.stVC1.sequenceHeaderC.uiMacroblockQuantization,
                                 pMetadata->uProtocolData.stVC1.sequenceHeaderC.bVariableSizedTransform,
                                 pMetadata->uProtocolData.stVC1.sequenceHeaderC.bOverlappedTransform,
                                 pMetadata->uProtocolData.stVC1.sequenceHeaderC.bSyncMarker,
                                 pMetadata->uProtocolData.stVC1.sequenceHeaderC.bRangeReduction,
                                 pMetadata->uProtocolData.stVC1.sequenceHeaderC.uiMaxBFrames,
                                 pMetadata->uProtocolData.stVC1.sequenceHeaderC.uiQuantizer,
                                 pMetadata->uProtocolData.stVC1.sequenceHeaderC.bFrameInterpolation);
                              break;
                           default:
                              /* print zeros for the codec-specific data */
                              fprintf(hInput->hMetadataDumpFile, "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ");
                              break;
                        }
                        fprintf(hInput->hMetadataDumpFile, "0x%llx", pMetadata->stTiming.uiSTCSnapshot);
                        fprintf(hInput->hMetadataDumpFile, "%d\n", pMetadata->stTiming.uiChunkId);
                     }
                     /* else, ignore this descriptor (unknown contents) */
                     break;
                  case BMUXlib_Input_Type_eAudio:
                     if (BAVC_AudioMetadataType_eCommon == BMUXLIB_INPUT_DESCRIPTOR_AUDIO_DATA_UNIT_TYPE(pDesc))
                     {
                        BAVC_AudioMetadataDescriptor *pMetadata = (BAVC_AudioMetadataDescriptor *)BMUXLIB_INPUT_DESCRIPTOR_VIRTUAL_ADDRESS(pDesc);
                        fprintf(hInput->hMetadataDumpFile, "0x%x, %d, %d, 0x%llx, %d, %d, ",
                           pMetadata->uiMetadataFlags, pMetadata->stBitrate.uiMax, pMetadata->stSamplingFrequency.uiSamplingFrequency,
                           pMetadata->stTiming.uiSTCSnapshot, pMetadata->stTiming.uiChunkId, pMetadata->eProtocol);
                        switch(pMetadata->eProtocol)
                        {
                           case BAVC_AudioCompressionStd_eAacAdts:
                           case BAVC_AudioCompressionStd_eAacLoas:
                           case BAVC_AudioCompressionStd_eAacPlusAdts:
                           case BAVC_AudioCompressionStd_eAacPlusLoas:
                           {
                              uint32_t uiByteCount;
                              /* print aac info */
                              fprintf(hInput->hMetadataDumpFile, "%d, 0x", pMetadata->uProtocolData.stAac.uiASCLengthBits);
                              for (uiByteCount = 0; uiByteCount < pMetadata->uProtocolData.stAac.uiASCLengthBytes; uiByteCount++)
                                 fprintf(hInput->hMetadataDumpFile, "%2.2x", pMetadata->uProtocolData.stAac.auiASC[uiByteCount]);
                              fprintf(hInput->hMetadataDumpFile, ", ");
                              /* print zeros for wma info */
                              fprintf(hInput->hMetadataDumpFile, "0, 0, 0, 0, 0");
                              break;
                           }
                           case BAVC_AudioCompressionStd_eWmaStd :
                              /* print zeros for aac info */
                              fprintf(hInput->hMetadataDumpFile, "0, 0, ");
                              /* print wma info */
                              fprintf(hInput->hMetadataDumpFile, "%d, %d, %d, %d, %d",
                                 pMetadata->uProtocolData.stWmaStd.uiSamplesPerBlock,
                                 pMetadata->uProtocolData.stWmaStd.uiEncodeOptions,
                                 pMetadata->uProtocolData.stWmaStd.uiSuperBlockAlign,
                                 pMetadata->uProtocolData.stWmaStd.uiBlockAlign,
                                 pMetadata->uProtocolData.stWmaStd.uiNumChannels);
                              break;
                           default:
                              /* unknown/unsupported protocol: print zeros for all fields */
                              fprintf(hInput->hMetadataDumpFile, "0, 0, ");         /* aac */
                              fprintf(hInput->hMetadataDumpFile, "0, 0, 0, 0, 0");  /* wma */
                              break;
                        }
                        fprintf(hInput->hMetadataDumpFile, "\n");
                     }
                     /* else, ignore this descriptor (unknown contents) */
                     break;
                  default:
                     break;
               }
            } /* end: is metadata descriptor */
            fflush( hInput->hMetadataDumpFile );
         } /* end: hMetadataDumpFile != NULL */
#endif
      } /* end: for each new descriptor */
   } /* end: hDescDumpFile != NULL */
}
#endif

/*****************************************************************************
* EOF
******************************************************************************/
