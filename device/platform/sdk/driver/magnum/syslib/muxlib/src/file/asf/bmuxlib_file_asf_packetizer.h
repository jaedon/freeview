/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmuxlib_file_asf_packetizer.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 11/10/11 12:49p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/muxlib/7425/file/asf/bmuxlib_file_asf_packetizer.h $
 * 
 * Hydra_Software_Devel/2   11/10/11 12:49p nilesh
 * SW7425-1691: Merge to mainline
 * 
 * Hydra_Software_Devel/SW7425-1691/1   11/10/11 11:56a nilesh
 * SW7425-1691: Added support for stop-->start without requiring
 * destroy/create
 * 
 * Hydra_Software_Devel/1   9/1/11 12:04p nilesh
 * SW7425-720: ASF Packetizer
 * 
 ***************************************************************************/

#ifndef BMUXLIB_FILE_ASF_PACKETIZER_H_
#define BMUXLIB_FILE_ASF_PACKETIZER_H_

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif

typedef struct BMUXlib_File_ASF_P_Packetizer_Context *BMUXlib_File_ASF_P_Packetizer_Handle;

typedef struct BMUXlib_File_ASF_P_Packetizer_CreateSettings
{
      uint32_t uiSignature;

      size_t uiPacketSize;
} BMUXlib_File_ASF_P_Packetizer_CreateSettings;

typedef struct BMUXlib_File_ASF_P_Packetizer_FrameInfo
{
      bool bNewFrame;
      bool bKeyFrame;
      size_t uiFrameSize;
      uint8_t uiFrameNumber;
      uint32_t uiSendTime; /* in milliseconds */
      uint32_t uiPresentationTime; /* in milliseconds */
} BMUXlib_File_ASF_P_Packetizer_FrameInfo;

typedef struct BMUXlib_File_ASF_P_Packetizer_Descriptor
{
      void *pBuffer;
      size_t uiLength;

      uint8_t uiStreamNumber;
      size_t uiStreamOffset; /* Absolute offset into this stream */

      BMUXlib_File_ASF_P_Packetizer_FrameInfo stFrameInfo; /* Data for this frame.  Duplicated for all descriptors for the same frame */
} BMUXlib_File_ASF_P_Packetizer_Descriptor;

void
BMUXlib_File_ASF_P_Packetizer_GetDefaultCreateSettings(
         BMUXlib_File_ASF_P_Packetizer_CreateSettings *pstSettings
         );

BERR_Code
BMUXlib_File_ASF_P_Packetizer_Create(
         BMUXlib_File_ASF_P_Packetizer_Handle *phPacketizer,
         const BMUXlib_File_ASF_P_Packetizer_CreateSettings *pstSettings
         );

void
BMUXlib_File_ASF_P_Packetizer_Destroy(
         BMUXlib_File_ASF_P_Packetizer_Handle hPacketizer
         );

void
BMUXlib_File_ASF_P_Packetizer_Reset(
         BMUXlib_File_ASF_P_Packetizer_Handle hPacketizer
         );

bool
BMUXlib_File_ASF_P_Packetizer_IsSpaceAvailable(
         BMUXlib_File_ASF_P_Packetizer_Handle hPacketizer
         );

BERR_Code
BMUXlib_File_ASF_P_Packetizer_AddDescriptor(
         BMUXlib_File_ASF_P_Packetizer_Handle hPacketizer,
         BMUXlib_File_ASF_P_Packetizer_Descriptor *pstDescriptor,
         BMUXlib_Output_CompletedCallbackInfo *pstCompletedCallbackInfo
         );

BERR_Code
BMUXlib_File_ASF_P_Packetizer_ProcessNewDescriptors(
         BMUXlib_File_ASF_P_Packetizer_Handle hPacketizer
         );

BERR_Code
BMUXlib_File_ASF_P_Packetizer_ScheduleProcessedPackets(
         BMUXlib_File_ASF_P_Packetizer_Handle hPacketizer,
         BMUXlib_Output_Handle hOutput
         );

bool
BMUXlib_File_ASF_P_Packetizer_IsDescriptorPendingQueue(
         BMUXlib_File_ASF_P_Packetizer_Handle hPacketizer
         );

uint64_t
BMUXlib_File_ASF_P_Packetizer_GetDataPacketCount(
         BMUXlib_File_ASF_P_Packetizer_Handle hPacketizer
         );

#ifdef __cplusplus
}
#endif

#endif /* BMUXLIB_FILE_ASF_PACKETIZER_H_ */
