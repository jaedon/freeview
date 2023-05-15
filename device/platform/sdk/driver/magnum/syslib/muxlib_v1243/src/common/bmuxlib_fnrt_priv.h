/***************************************************************************
 *     Copyright (c) 2003-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmuxlib_fnrt_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 4/24/13 2:21p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/muxlib/src/common/bmuxlib_fnrt_priv.h $
 * 
 * Hydra_Software_Devel/5   4/24/13 2:21p nilesh
 * SW7425-4710: Prevent linearized PTS values from going backwards.
 * 
 * Hydra_Software_Devel/4   4/11/13 2:58p nilesh
 * SW7425-4710: Updated PTS/DTS linearize logic to account for dropped
 * frames
 * 
 * Hydra_Software_Devel/3   2/28/13 4:22p nilesh
 * SW7425-4355: Linearize ESCR based on ESCR(n-1), bitrate(n-1), length(n-
 * 1). Added ESCR reset provision if ESCR(n) is less than ETS(n).
 * 
 * Hydra_Software_Devel/2   10/17/12 5:22p nilesh
 * SW7435-304: Framework for discontinuity support
 * 
 * Hydra_Software_Devel/1   8/13/12 12:28p nilesh
 * SW7425-3362: Merged FNRT support
 * 
 * Hydra_Software_Devel/SW7425-3362/12   8/3/12 12:48p nilesh
 * SW7425-3362: Fixed EOS handling
 * 
 * Hydra_Software_Devel/SW7425-3362/11   7/27/12 10:18p nilesh
 * SW7425-3362: Drop initial frames from slave until original PTS is valid
 * 
 * Hydra_Software_Devel/SW7425-3362/10   7/26/12 5:39p delkert
 * SW7425-3370: Add group ID to config file. Add status updates (for base
 * addresses) to config file. Add back-link from group to main context.
 *
 * Hydra_Software_Devel/SW7425-3362/9   7/26/12 3:25p nilesh
 * SW7425-3362: Initial A/V lipsync support
 *
 * Hydra_Software_Devel/SW7425-3362/8   7/26/12 11:29a nilesh
 * SW7425-3362: Initial audio support. No lipsync or discontunity support.
 *
 * Hydra_Software_Devel/SW7425-3362/7   7/25/12 1:44p nilesh
 * SW7425-3362: Updated FNRTlib API to support multiple groups per input
 * type
 *
 * Hydra_Software_Devel/SW7425-3362/6   7/24/12 3:57p delkert
 * SW7425-3370: Add configuration dump when in test mode, for tracking
 * inputs.
 *
 * Hydra_Software_Devel/SW7425-3362/5   7/24/12 2:26p nilesh
 * SW7425-3357: Removed used of
 * BAVC_CompressedBufferDescriptor.pBaseBufferAddress
 *
 * Hydra_Software_Devel/SW7425-3362/4   7/13/12 11:27a hongtaoz
 * SW7425-3362: added bookkeeping of input index tag for each FNRT
 * descriptors; fixed chunk DTS offset in 90Khz instead of 27Mhz; free
 * consumed descriptors for all inputs instead of early break of for loop
 * to avoid hang;
 *
 * Hydra_Software_Devel/SW7425-3362/3   7/5/12 5:47p nilesh
 * SW7425-3362: Added basic timning adjustment logic.  Various bug fixes.
 *
 * Hydra_Software_Devel/SW7425-3362/2   7/5/12 12:59p nilesh
 * SW7425-3362: Added BMUXLIB_FNRT_DUMP_DESC support
 *
 * Hydra_Software_Devel/SW7425-3362/1   7/3/12 5:26p nilesh
 * SW7425-3362: Initial EncoderGroup implementation
 *
 ***************************************************************************/

#ifndef BMUXLIB_FNRT_PRIV_H_
#define BMUXLIB_FNRT_PRIV_H_

#if BMUXLIB_FNRT_P_DUMP_DESC
#include <stdio.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define BMUXLIB_FNRT_P_SIGNATURE_CREATESETTINGS 0x464e5201
#define BMUXLIB_FNRT_P_SIGNATURE_HANDLE         0x464e5202
#define BMUXLIB_FNRT_P_SIGNATURE_INPUTSETTINGS  0x464e5203
#define BMUXLIB_FNRT_P_SIGNATURE_OUTPUTSETTINGS 0x464e5204

typedef enum BMUXlib_FNRT_P_InputType
{
   BMUXlib_FNRT_P_InputType_eVideo,
   BMUXlib_FNRT_P_InputType_eAudio,

   BMUXlib_FNRT_P_InputType_eMax
} BMUXlib_FNRT_P_InputType;

typedef enum BMUXlib_FNRT_P_InputState
{
   BMUXlib_FNRT_P_InputState_eSelectInput,
   BMUXlib_FNRT_P_InputState_eProcessInput,
   BMUXlib_FNRT_P_InputState_eDone
} BMUXlib_FNRT_P_InputState;

typedef struct BMUXlib_FNRT_P_EncoderGroup_Metadata
{
   signed iInputIndex;
   unsigned uiNumDescriptors;
} BMUXlib_FNRT_P_EncoderGroup_Metadata;

typedef struct BMUXlib_FNRT_P_EncoderGroup_Context *BMUXlib_FNRT_P_EncoderGroup_Handle;

#define BMUXLIB_FNRT_P_ENCODER_GROUP_MAX_INPUTS 4
#define BMUXLIB_FNRT_P_ENCODER_GROUP_MAX_DESCRIPTORS 512

typedef struct BMUXlib_FNRT_P_EncoderGroup_Context
{
   BMUXlib_FNRT_P_InputType eType;

   struct
   {
      bool bEnable;
      union
      {
         BMUXlib_VideoEncoderInterface stVideoInterface;
         BMUXlib_AudioEncoderInterface stAudioInterface;
      } interface;
      unsigned uiPendingDescriptors;
   } input[BMUXLIB_FNRT_P_ENCODER_GROUP_MAX_INPUTS];
   unsigned uiNumActiveInputs;

   struct
   {
      unsigned uiWriteOffset;
      unsigned uiReadOffset;
      unsigned uiQueuedOffset;

      union
      {
         BAVC_VideoBufferDescriptor astVideoDescriptor[BMUXLIB_FNRT_P_ENCODER_GROUP_MAX_DESCRIPTORS];
         BAVC_AudioBufferDescriptor astAudioDescriptor[BMUXLIB_FNRT_P_ENCODER_GROUP_MAX_DESCRIPTORS];
      } descriptors;

      BMUXlib_FNRT_P_EncoderGroup_Metadata stMetadata[BMUXLIB_FNRT_P_ENCODER_GROUP_MAX_DESCRIPTORS];
   } serialized;

   struct
   {
      unsigned uiWriteOffset;
      unsigned uiReadOffset;

      union
      {
         BAVC_VideoBufferDescriptor astVideoDescriptor[BMUXLIB_FNRT_P_ENCODER_GROUP_MAX_DESCRIPTORS];
         BAVC_AudioBufferDescriptor astAudioDescriptor[BMUXLIB_FNRT_P_ENCODER_GROUP_MAX_DESCRIPTORS];
      } descriptors;

      BMUXlib_FNRT_P_EncoderGroup_Metadata stMetadata[BMUXLIB_FNRT_P_ENCODER_GROUP_MAX_DESCRIPTORS];

      union
      {
         BAVC_VideoBufferInfo stVideoBufferInfo;
         BAVC_AudioBufferInfo stAudioBufferInfo;
      } bufferInfo;

      union
      {
         BAVC_CompressedBufferStatus stCommon;
         BAVC_VideoBufferStatus stVideo;
         BAVC_AudioBufferStatus stAudio;
      } uBufferStatus;
      bool bBufferStatusValid;
   } output;

   BMUXlib_FNRT_P_InputState eInputState;
   signed iActiveInputIndex; /* -1 indicates no active input */
   unsigned uiChunkId;
   BMUXlib_FNRT_P_EncoderGroup_Handle hMasterEncoderGroup; /* If NULL, then this is the master */
   struct
   {
      uint64_t uiTargetDeltaNewVsOriginalPts;
      bool bValid;
   } master;
   bool bDropUntilRAP;

   uint64_t uiPreviousPTS;
   bool bPreviousPTSValid;

   uint64_t uiPreviousDTS;
   bool bPreviousDTSValid;

   uint32_t uiPreviousOPTS;
   bool bPreviousOPTSValid;

   unsigned uiDeltaDTS;
   bool bDeltaDTSValid;

   unsigned uiEtsDtsOffset;
   bool bEtsDtsOffsetValid;

   struct
   {
      uint64_t uiTimingOffsetIn90Khz;
      bool bTimingOffsetValid;

      uint64_t uiPreviousOriginalDTS;
      bool bPreviousOriginalDTSValid;

      size_t auiNumDescriptors[2];
      union
      {
            const BAVC_CompressedBufferDescriptor *astCommon[2];
            const BAVC_VideoBufferDescriptor *astVideo[2];
            const BAVC_AudioBufferDescriptor *astAudio[2];
      } descriptors;

      BAVC_CompressedBufferStatus stBufferStatus;
      bool bBufferStatusValid;
      bool bEOCSeen;
   } chunk;

   struct
   {
      bool bValid;
      unsigned uiPreviousESCR;
      uint16_t uiPreviousTicksPerBit;
      int16_t iPreviousSHR;
      size_t uiPreviousLength;
   } linearizeESCR;

   struct BMUXlib_FNRT_P_Context *hMain;  /* reference back to main containing context */

#if BMUXLIB_FNRT_P_DUMP_DESC
   FILE *hDescDumpFile;
   unsigned *puiDescCount;
#endif
} BMUXlib_FNRT_P_EncoderGroup_Context;

BERR_Code
BMUXlib_FNRT_P_EncoderGroup_GetVideoBufferDescriptors(
   BMUXlib_FNRT_P_EncoderGroup_Handle hMuxFNRTEncoderGroup,
   const BAVC_VideoBufferDescriptor **astDescriptors0,
   size_t *puiNumDescriptors0,
   const BAVC_VideoBufferDescriptor **astDescriptors1,
   size_t *puiNumDescriptors1
   );

BERR_Code
BMUXlib_FNRT_P_EncoderGroup_GetVideoBufferStatus(
   BMUXlib_FNRT_P_EncoderGroup_Handle hMuxFNRTEncoderGroup,
   BAVC_VideoBufferStatus *pBufferStatus
   );

BERR_Code
BMUXlib_FNRT_P_EncoderGroup_GetAudioBufferDescriptors(
   BMUXlib_FNRT_P_EncoderGroup_Handle hMuxFNRTEncoderGroup,
   const BAVC_AudioBufferDescriptor **astDescriptors0,
   size_t *puiNumDescriptors0,
   const BAVC_AudioBufferDescriptor **astDescriptors1,
   size_t *puiNumDescriptors1
   );

BERR_Code
BMUXlib_FNRT_P_EncoderGroup_GetAudioBufferStatus(
   BMUXlib_FNRT_P_EncoderGroup_Handle hMuxFNRTEncoderGroup,
   BAVC_AudioBufferStatus *pBufferStatus
   );

BERR_Code
BMUXlib_FNRT_P_EncoderGroup_ConsumeBufferDescriptors(
   BMUXlib_FNRT_P_EncoderGroup_Handle hMuxFNRTEncoderGroup,
   size_t uiNumBufferDescriptors
   );

typedef struct BMUXlib_FNRT_P_Context
{
   uint32_t uiSignature;
   BMUXlib_FNRT_CreateSettings stCreateSettings;

   BMUXlib_FNRT_P_EncoderGroup_Context astEncoderGroup[BMUXlib_FNRT_P_InputType_eMax];

#if BMUXLIB_FNRT_P_DUMP_DESC
   unsigned uiDescCount;
#endif
#if BMUXLIB_FNRT_P_TEST_MODE
   FILE *hConfigFile;
#endif
} BMUXlib_FNRT_P_Context;

#ifdef __cplusplus
}
#endif

#endif /* BMUXLIB_FNRT_PRIV_H_ */
/* End of File */
