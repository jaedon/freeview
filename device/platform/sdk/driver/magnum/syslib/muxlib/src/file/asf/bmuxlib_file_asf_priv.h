/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmuxlib_file_asf_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/17 $
 * $brcm_Date: 6/20/12 10:24a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/muxlib/7425/file/asf/bmuxlib_file_asf_priv.h $
 * 
 * Hydra_Software_Devel/17   6/20/12 10:24a delkert
 * SW7425-3272: Explicitly cast input data in the SetXX macros to
 * (hopefully) resolve coverity complaints.
 *
 * Hydra_Software_Devel/16   2/2/12 5:08p nilesh
 * SW7425-2315: Added support for
 * BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EMPTY_FRAME
 *
 * Hydra_Software_Devel/15   12/20/11 1:45p nilesh
 * SW7425-1869: Merged BMUXlib_InputGroup support
 *
 * Hydra_Software_Devel/SW7425-1869/1   12/9/11 8:40a delkert
 * SW7425-1869: Modify ASF to use InputGroup for input selection.
 *
 * Hydra_Software_Devel/14   11/15/11 4:47p delkert
 * SWCHECKFRAME-114: Move accessor macros to private header, and rename
 * them to include _P_
 *
 * Hydra_Software_Devel/13   11/15/11 4:26p nilesh
 * SW7425-1691: Reset stStartSettings and stFinishSettings on stop()
 *
 * Hydra_Software_Devel/12   11/10/11 12:49p nilesh
 * SW7425-1691: Merge to mainline
 *
 * Hydra_Software_Devel/SW7425-1691/1   11/10/11 11:56a nilesh
 * SW7425-1691: Added support for stop-->start without requiring
 * destroy/create
 *
 * Hydra_Software_Devel/11   10/6/11 2:25p delkert
 * SW7425-1405: Moved Max Packet Length to the public API.
 *
 * Hydra_Software_Devel/10   10/6/11 1:50p delkert
 * SW7425-1405: Add signature to context struct and add checking to all
 * APIs. Add packet length upper limit checks in Create()
 *
 * Hydra_Software_Devel/9   9/1/11 12:08p nilesh
 * SW7425-720: Integrated new ASF Packetizer
 *
 * Hydra_Software_Devel/8   8/12/11 10:17a nilesh
 * SW7425-936: Reduce packet length from 64K to 8K
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
 * Hydra_Software_Devel/4   5/17/11 10:49a nilesh
 * SW7425-447: Reduced default packet length from 8192 to 1024 to minimize
 * wasted space caused by padding
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

#ifndef BMUXLIB_FILE_ASF_PRIV_H_
#define BMUXLIB_FILE_ASF_PRIV_H_

#include "bmuxlib_list.h"
#include "bmuxlib_input.h"
#include "bmuxlib_output.h"

#include "bmuxlib_file_asf_header.h"
#include "bmuxlib_file_asf_packetizer.h"

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif

/****************************
*  D E F I N I T I O N S    *
****************************/

#define BMUXlib_File_ASF_P_USE_AAC_RAW 1

#define BMUXLIB_FILE_ASF_P_MAX_OUTPUT_DESCRIPTORS       16384
#define BMUXLIB_FILE_ASF_P_MAX_INPUT_DESCRIPTORS        BMUXLIB_FILE_ASF_P_MAX_OUTPUT_DESCRIPTORS
#define BMUXLIB_FILE_ASF_P_DEFAULT_PACKETLENGTH         8*1024
#define BMUXLIB_FILE_ASF_P_DEFAULT_FINISHMODE           BMUXlib_FinishMode_ePrepareForStop
#define BMUXLIB_FILE_ASF_P_MUX_SERVICE_PERIOD           50
#define BMUXLIB_FILE_ASF_P_MAX_INPUTS                   (BMUXLIB_FILE_ASF_MAX_VIDEO_INPUTS + BMUXLIB_FILE_ASF_MAX_AUDIO_INPUTS)
#define BMUXLIB_FILE_ASF_P_MAX_HEADERLENGTH             1024
#define BMUXLIB_FILE_ASF_P_MAX_FRAME_SIZE                3*1024*1024
#define BMUXLIB_FILE_ASF_P_MAX_FRAMES_PENDING            3
#define BMUXLIB_FILE_ASF_P_MAX_PAYLOADS_PER_PACKET       1

/**************/
/* Signatures */
/**************/
#define BMUXLIB_FILE_ASF_P_SIGNATURE_CREATESETTINGS   0x65837901
#define BMUXLIB_FILE_ASF_P_SIGNATURE_STARTSETTINGS    0x65837902
#define BMUXLIB_FILE_ASF_P_SIGNATURE_FINISHSETTINGS   0x65837903
#define BMUXLIB_FILE_ASF_P_SIGNATURE_CONTEXT          0x65837904
#define BMUXLIB_FILE_ASF_P_PACKETIZER_SIGNATURE_CREATESETTINGS   0x65837905

/**************
* M A C R O S *
***************/
#define BMUXlib_File_ASF_P_Set64_LE( pBuffer, offset, data ) \
{\
   ((uint8_t*)(pBuffer))[offset+0] = (uint8_t) (((uint64_t)(data) >> 0 ) & 0xFF); \
   ((uint8_t*)(pBuffer))[offset+1] = (uint8_t) (((uint64_t)(data) >> 8 ) & 0xFF); \
   ((uint8_t*)(pBuffer))[offset+2] = (uint8_t) (((uint64_t)(data) >> 16) & 0xFF); \
   ((uint8_t*)(pBuffer))[offset+3] = (uint8_t) (((uint64_t)(data) >> 24) & 0xFF); \
   ((uint8_t*)(pBuffer))[offset+4] = (uint8_t) (((uint64_t)(data) >> 32) & 0xFF); \
   ((uint8_t*)(pBuffer))[offset+5] = (uint8_t) (((uint64_t)(data) >> 40) & 0xFF); \
   ((uint8_t*)(pBuffer))[offset+6] = (uint8_t) (((uint64_t)(data) >> 48) & 0xFF); \
   ((uint8_t*)(pBuffer))[offset+7] = (uint8_t) (((uint64_t)(data) >> 56) & 0xFF); \
}

#define BMUXlib_File_ASF_P_Set32_LE( pBuffer, offset, data ) \
{\
   ((uint8_t*)(pBuffer))[offset+0] = (uint8_t) (((uint32_t)(data) >> 0 ) & 0xFF); \
   ((uint8_t*)(pBuffer))[offset+1] = (uint8_t) (((uint32_t)(data) >> 8 ) & 0xFF); \
   ((uint8_t*)(pBuffer))[offset+2] = (uint8_t) (((uint32_t)(data) >> 16) & 0xFF); \
   ((uint8_t*)(pBuffer))[offset+3] = (uint8_t) (((uint32_t)(data) >> 24) & 0xFF); \
}

#define BMUXlib_File_ASF_P_Set16_LE( pBuffer, offset, data ) \
{\
   ((uint8_t*)(pBuffer))[offset+0] = (uint8_t) (((uint16_t)(data) >> 0 ) & 0xFF); \
   ((uint8_t*)(pBuffer))[offset+1] = (uint8_t) (((uint16_t)(data) >> 8 ) & 0xFF); \
}

#define BMUXlib_File_ASF_P_Set8( pBuffer, offset, data ) \
{\
   ((uint8_t*)(pBuffer))[offset+0] = (uint8_t) (data); \
}

#define BMUXlib_File_ASF_P_Set8_Mask( pBuffer, offset, mask, shift, data ) \
{\
   ((uint8_t*)(pBuffer))[offset+0] &= ~mask; \
   ((uint8_t*)(pBuffer))[offset+0] |= (uint8_t) (((data) << shift ) & mask); \
}

#define BMUXlib_File_ASF_P_Set32_BE( pBuffer, offset, data ) \
{\
   ((uint8_t*)(pBuffer))[offset+0] = (uint8_t) (((uint32_t)(data) >> 24) & 0xFF); \
   ((uint8_t*)(pBuffer))[offset+1] = (uint8_t) (((uint32_t)(data) >> 16) & 0xFF); \
   ((uint8_t*)(pBuffer))[offset+2] = (uint8_t) (((uint32_t)(data) >> 8 ) & 0xFF); \
   ((uint8_t*)(pBuffer))[offset+3] = (uint8_t) (((uint32_t)(data) >> 0 ) & 0xFF); \
}

/* accessor macro to allow test code to access the mux state from within the context */
#define BMUXLIB_FILE_ASF_P_SET_MUX_STATE(handle, state)     ((handle)->status.eMuxState = (state))
#define BMUXLIB_FILE_ASF_P_GET_MUX_STATE(handle)            ((handle)->status.eMuxState)

/****************************
*        T Y P E S          *
****************************/
typedef enum BMUXlib_File_ASF_StartedState
{
   BMUXlib_File_ASF_StartedState_eWaitForMetadata, /* Before ASF Muxing can start, all inputs must have a metadata descriptor available in order to generate the headers */
   BMUXlib_File_ASF_StartedState_eWriteHeader,     /* Write the header */
   BMUXlib_File_ASF_StartedState_eWriteMedia,      /* Write the media */

   BMUXlib_File_ASF_StartedState_eMax
} BMUXlib_File_ASF_StartedState;

typedef union BMUXlib_File_ASF_P_CodecSpecificData
{
      struct
      {
            struct
            {
                  bool bCommonValid;
                  BAVC_VideoMetadataDescriptor stCommon;
            } data;
      } video;

      struct
      {
            struct
            {
                  bool bCommonValid;
                  BAVC_AudioMetadataDescriptor stCommon;
            } data;
      } audio;
} BMUXlib_File_ASF_P_CodecSpecificData;

typedef struct BMUXlib_File_ASF_P_Input
{
      BMUXlib_Input_Type eType;
      BMUXlib_Input_Handle hInput;

      /* NOTE: for ASF compliance, Stream Numbers are range limited to 7 bits in the range 1-127. */
      uint8_t uiStreamNumber;   /* the stream number to use for this source (must NOT be zero) */

      bool bCodecSpecificDataValid;
      BMUXlib_File_ASF_P_CodecSpecificData stCodecSpecificData;

      union
      {
            BAVC_VideoBufferInfo *pstVideo;
            BAVC_AudioBufferInfo *pstAudio;
      } bufferInfo;

      size_t uiCurrentStreamOffset;
      size_t uiCurrentFrameNumber;
      BMUXlib_File_ASF_P_Packetizer_FrameInfo stCurrentFrameInfo;

      struct
      {
         bool bReleased[BMUXLIB_FILE_ASF_P_MAX_INPUT_DESCRIPTORS];
         unsigned uiReadOffset;
         unsigned uiWriteOffset;
      } stReleaseQueue;
} BMUXlib_File_ASF_P_Input;

typedef enum BMUXlib_File_ASF_P_Blocked_State
{
   BMUXlib_File_ASF_P_Blocked_State_eNotBlocked,
   BMUXlib_File_ASF_P_Blocked_State_eInput,
   BMUXlib_File_ASF_P_Blocked_State_eOutput
} BMUXlib_File_ASF_P_Blocked_State;

typedef enum BMUXlib_File_ASF_P_ProcessInputState
{
   BMUXlib_File_ASF_P_ProcessInputState_eSelectInput,
   BMUXlib_File_ASF_P_ProcessInputState_eProcessInput,
   BMUXlib_File_ASF_P_ProcessInputState_eProcessNextDescriptor
} BMUXlib_File_ASF_P_ProcessInputState;

typedef struct BMUXlib_File_ASF_P_Context
{
      uint32_t uiSignature;     /* [DO NOT MODIFY] Populated by BMUXlib_File_ASF_Create() */

      /* Settings */
      BMUXlib_File_ASF_CreateSettings stCreateSettings;

      /* Output Handle */
      BMUXlib_Output_Handle hOutput;

      BMUXlib_InputGroup_Handle hInputGroup;

      /* Packetizer Handle */
      BMUXlib_File_ASF_P_Packetizer_Handle hPacketizer;

      /* Header */
      uint8_t *pHeaderBuffer;
      size_t uiHeaderBufferSize;

      /* Status */
      struct
      {
         BMUXlib_State                 eMuxState;
         BMUXlib_File_ASF_StartedState eMuxStartedState;

         BMUXlib_File_ASF_StartSettings stStartSettings;
         BMUXlib_File_ASF_FinishSettings stFinishSettings;

         /* Input Metadata State */
         BMUXlib_File_ASF_P_Input astActiveInputs[BMUXLIB_FILE_ASF_P_MAX_INPUTS];
         size_t uiNumActiveInputs;

         BMUXlib_File_ASF_P_ProcessInputState eProcessInputState;
         BMUXlib_File_ASF_P_Input* pCurrentInput;

         struct
         {
               uint64_t uiFileSize;
               uint8_t auiFileSize[8];
               uint64_t uiDataPacketsCount;
               uint8_t auiDataPacketsCount[8];
               uint64_t uiPlayDuration;
               uint8_t auiPlayDuration[8];
               uint64_t uiSendDuration;
               uint8_t auiSendDuration[8];

               uint8_t auiDataObjectSize[8];

               uint8_t auiFileFlags[4];
         } stStats;

         struct
         {
               uint64_t uiFileProperties;
               uint64_t uiDataObject;
         } stOffsets;

         BMUXlib_File_ASF_P_PostProcess_State ePostProcessState;

         BMUXlib_File_ASF_P_Blocked_State eBlockedState;
      } status;
} BMUXlib_File_ASF_P_Context;

/**********************
* P R O T O T Y P E S *
**********************/
bool BMUXlib_File_ASF_P_WaitForMetadata( BMUXlib_File_ASF_Handle          hASFMux );

BERR_Code
BMUXlib_File_ASF_P_UpdateInputDescriptors(
         BMUXlib_File_ASF_Handle hASFMux
         );

bool
BMUXlib_File_ASF_P_ProcessInputDescriptors(
         BMUXlib_File_ASF_Handle hASFMux
         );

BERR_Code
BMUXlib_File_ASF_P_ProcessCompletedInputDescriptors(
         BMUXlib_File_ASF_Handle hASFMux
         );

#ifdef __cplusplus
}
#endif

#endif /* BMUXLIB_FILE_ASF_PRIV_H_ */
